#include "hardware.h"

#if WITHLTDCHW

#include "display.h"
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include "formats.h"	// for debug prints

#if CPUSTYLE_XC7Z

#include <xdebug.h>
#include <xil_io.h>
#include "zynq_vdma.h"

static XVtc vtc;
volatile uint8_t vdma_sync = 0;

void vtc_inthandler (void)
{
	u32 PendingIntr = XVtc_IntrGetPending(& vtc);
	XVtc_IntrClear(& vtc, PendingIntr);

	if (PendingIntr == XVTC_IXR_G_VBLANK_MASK)
	{
		vdma_sync = 1;
		//dbg_putchar('.');
	}
}

/************************************************************************/
/*																		*/
/*	display_ctrl.c	--	Digilent Display Controller Driver				*/
/*																		*/
/************************************************************************/
/*	Author: Sam Bobrowicz												*/
/*	Copyright 2014, Digilent Inc.										*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*		This module provides an easy to use API for controlling a    	*/
/*		Display attached to a Digilent system board via VGA or HDMI. 	*/
/*		run-time resolution setting and seamless framebuffer-swapping 	*/
/*		for tear-free animation. 										*/
/*																		*/
/*		To use this driver, you must have a Xilinx Video Timing 		*/
/* 		Controller core (vtc), Xilinx axi_vdma core, a Digilent 		*/
/*		axi_dynclk core, a Xilinx AXI Stream to Video core, and either  */
/*		a Digilent RGB2VGA or RGB2DVI core all present in your design.  */
/*		See the Video in or Display out reference projects for your     */
/*		system board to see how they need to be connected. Digilent     */
/*		reference projects and IP cores can be found at 				*/
/*		www.github.com/Digilent.			 							*/
/*																		*/
/*		The following steps should be followed to use this driver:		*/
/*		1) Create a DisplayCtrl object and pass a pointer to it to 		*/
/*		   DisplayInitialize.											*/
/*		2) Call DisplaySetMode to set the desired mode					*/
/*		3) Call DisplayStart to begin outputting data to the display	*/
/*		4) To create a seamless animation, draw the next image to a		*/
/*		   framebuffer currently not being displayed. Then call 		*/
/*		   DisplayChangeFrame to begin displaying that frame.			*/
/*		   Repeat as needed, only ever modifying inactive frames.		*/
/*		5) To change the resolution, call DisplaySetMode, followed by	*/
/*		   DisplayStart again.											*/
/*																		*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/* 																		*/
/*		2/20/2014(SamB): Created										*/
/*		11/25/2015(SamB): Changed from axi_dispctrl to Xilinx cores		*/
/*						  Separated Clock functions into dynclk library */
/*																		*/
/************************************************************************/
/*
 * TODO: It would be nice to remove the need for users above this to access
 *       members of the DisplayCtrl struct manually. This can be done by
 *       implementing get/set functions for things like video mode, state,
 *       etc.
 */


/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

/*
 * Uncomment for Debugging messages over UART
 */
//#define DEBUG


/* ------------------------------------------------------------ */
/*				Procedure Definitions							*/
/* ------------------------------------------------------------ */

/***	DisplayStop(DisplayCtrl *dispPtr)
**
**	Parameters:
**		dispPtr - Pointer to the initialized DisplayCtrl struct
**
**	Return Value: int
**		XST_SUCCESS if successful.
**		XST_DMA_ERROR if an error was detected on the DMA channel. The
**			Display is still successfully stopped, and the error is
**			cleared so that subsequent DisplayStart calls will be
**			successful. This typically indicates insufficient bandwidth
**			on the AXI Memory-Map Interconnect (VDMA<->DDR)
**
**	Description:
**		Halts output to the display
**
*/
int DisplayStop(DisplayCtrl *dispPtr)
{
	/*
	 * If already stopped, do nothing
	 */
	if (dispPtr->state == DISPLAY_STOPPED)
	{
		return XST_SUCCESS;
	}

	/*
	 * Disable the disp_ctrl core, and wait for the current frame to finish (the core cannot stop
	 * mid-frame)
	 */
	XVtc_DisableGenerator(&dispPtr->vtc);

	/*
	 * Stop the VDMA core
	 */
	XAxiVdma_DmaStop(dispPtr->vdma, XAXIVDMA_READ);
	while(XAxiVdma_IsBusy(dispPtr->vdma, XAXIVDMA_READ));

	/*
	 * Update Struct state
	 */
	dispPtr->state = DISPLAY_STOPPED;

	//TODO: consider stopping the clock here, perhaps after a check to see if the VTC is finished

	if (XAxiVdma_GetDmaChannelErrors(dispPtr->vdma, XAXIVDMA_READ))
	{
		PRINTF("Clearing DMA errors...\n");
		XAxiVdma_ClearDmaChannelErrors(dispPtr->vdma, XAXIVDMA_READ, 0xFFFFFFFF);
		return XST_DMA_ERROR;
	}

	return XST_SUCCESS;
}
/* ------------------------------------------------------------ */

/***	DisplayStart(DisplayCtrl *dispPtr)
**
**	Parameters:
**		dispPtr - Pointer to the initialized DisplayCtrl struct
**
**	Return Value: int
**		XST_SUCCESS if successful, XST_FAILURE otherwise
**
**	Errors:
**
**	Description:
**		Starts the display.
**
*/
int DisplayStart(DisplayCtrl *dispPtr)
{
	int Status;

	int i;
	XVtc_Timing vtcTiming;
	XVtc_SourceSelect SourceSelect;

	PRINTF( "ZYNQ VDMA: display start entered\n");
	/*
	 * If already started, do nothing
	 */
	if (dispPtr->state == DISPLAY_RUNNING)
	{
		return XST_SUCCESS;
	}

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	/*
	 * Configure the vtc core with the display mode timing parameters
	 */
	vtcTiming.HActiveVideo = dispPtr->vMode.width;						    /* Horizontal Active Video Size */
	vtcTiming.HFrontPorch = dispPtr->vMode.hfp; //dispPtr->vMode.hps - dispPtr->vMode.width;	    /* Horizontal Front Porch Size */
	vtcTiming.HSyncWidth = dispPtr->vMode.hsync; //dispPtr->vMode.hpe - dispPtr->vMode.hps;		    /* Horizontal Sync Width */
	vtcTiming.HBackPorch = dispPtr->vMode.hbp; //dispPtr->vMode.hmax - dispPtr->vMode.hpe + 1;    /* Horizontal Back Porch Size */
	vtcTiming.HSyncPolarity = ! dispPtr->vMode.hsyncneg;	                        /* Horizontal Sync Polarity */
	vtcTiming.VActiveVideo = dispPtr->vMode.height;
	/* Vertical Active Video Size */
	vtcTiming.V0FrontPorch = dispPtr->vMode.vfp; //dispPtr->vMode.vps - dispPtr->vMode.height;    /* Vertical Front Porch Size */
	vtcTiming.V0SyncWidth = dispPtr->vMode.vsync; //dispPtr->vMode.vpe - dispPtr->vMode.vps;	    /* Vertical Sync Width */
	vtcTiming.V0BackPorch = dispPtr->vMode.vbp; //dispPtr->vMode.vmax - dispPtr->vMode.vpe + 1;	/* Horizontal Back Porch Size */

	vtcTiming.V1FrontPorch = dispPtr->vMode.vfp; //dispPtr->vMode.vps - dispPtr->vMode.height;	/* Vertical Front Porch Size */
	vtcTiming.V1SyncWidth = dispPtr->vMode.vsync; //dispPtr->vMode.vpe - dispPtr->vMode.vps;	    /* Vertical Sync Width */
	vtcTiming.V1BackPorch = dispPtr->vMode.vbp; //dispPtr->vMode.vmax - dispPtr->vMode.vpe + 1;	/* Horizontal Back Porch Size */

	vtcTiming.VSyncPolarity = ! dispPtr->vMode.vsyncneg;	                        /* Vertical Sync Polarity */
	vtcTiming.Interlaced = 0;		                                        /* Interlaced / Progressive video */

	/* Setup the VTC Source Select config structure. */
	/* 1=Generator registers are source */
	/* 0=Detector registers are source */
	memset((void *)&SourceSelect, 0, sizeof(SourceSelect));
	SourceSelect.VBlankPolSrc = 1;
	SourceSelect.VSyncPolSrc = 1;
	SourceSelect.HBlankPolSrc = 1;
	SourceSelect.HSyncPolSrc = 1;
	SourceSelect.ActiveVideoPolSrc = 1;
	SourceSelect.ActiveChromaPolSrc= 1;
	SourceSelect.VChromaSrc = 1;
	SourceSelect.VActiveSrc = 1;
	SourceSelect.VBackPorchSrc = 1;
	SourceSelect.VSyncSrc = 1;
	SourceSelect.VFrontPorchSrc = 1;
	SourceSelect.VTotalSrc = 1;
	SourceSelect.HActiveSrc = 1;
	SourceSelect.HBackPorchSrc = 1;
	SourceSelect.HSyncSrc = 1;
	SourceSelect.HFrontPorchSrc = 1;
	SourceSelect.HTotalSrc = 1;

	XVtc_SelfTest(&(dispPtr->vtc));

	XVtc_RegUpdateEnable(&(dispPtr->vtc));
	XVtc_SetGeneratorTiming(&(dispPtr->vtc), &vtcTiming);
	XVtc_SetSource(&(dispPtr->vtc), &SourceSelect);
    /*
	 * Enable VTC core, releasing backpressure on VDMA
	 */
	XVtc_EnableGenerator(&dispPtr->vtc);

	/*
	 * Configure the VDMA to access a frame with the same dimensions as the
	 * current mode
	 */
	dispPtr->vdmaConfig.VertSizeInput = dispPtr->vMode.height;
	dispPtr->vdmaConfig.HoriSizeInput = (dispPtr->vMode.width) * 4;
	dispPtr->vdmaConfig.FixedFrameStoreAddr = dispPtr->curFrame;
	/*
	 *Also reset the stride and address values, in case the user manually changed them
	 */
	dispPtr->vdmaConfig.Stride = dispPtr->stride;
	for (i = 0; i < LCDMODE_MAIN_PAGES; i++)
	{
		dispPtr->vdmaConfig.FrameStoreStartAddr[i] = dispPtr->framePhyAddr[i];
	}

	/*
	 * Perform the VDMA driver calls required to start a transfer. Note that no data is actually
	 * transferred until the disp_ctrl core signals the VDMA core by pulsing fsync.
	 */



	PRINTF( "ZYNQ VDMA: preform vdma transfer\n");

	Status = XAxiVdma_DmaConfig(dispPtr->vdma, XAXIVDMA_READ, &(dispPtr->vdmaConfig));
	if (Status != XST_SUCCESS)
	{
		PRINTF( "ZYNQ VDMA: Read channel config failed %d\n", Status);
		return XST_FAILURE;
	}
	Status = XAxiVdma_DmaSetBufferAddr(dispPtr->vdma, XAXIVDMA_READ, dispPtr->vdmaConfig.FrameStoreStartAddr);
	if (Status != XST_SUCCESS)
	{
		PRINTF( "ZYNQ VDMA: Read channel set buffer address failed %d\n", Status);
		return XST_FAILURE;
	}
	Status = XAxiVdma_DmaStart(dispPtr->vdma, XAXIVDMA_READ);
	if (Status != XST_SUCCESS)
	{
		PRINTF( "ZYNQ VDMA: Start read transfer failed %d\n", Status);
		return XST_FAILURE;
	}
	Status = XAxiVdma_StartParking(dispPtr->vdma, dispPtr->curFrame, XAXIVDMA_READ);
	if (Status != XST_SUCCESS)
	{
		PRINTF( "ZYNQ VDMA: Unable to park the channel %d\n", Status);
		return XST_FAILURE;
	}

	dispPtr->state = DISPLAY_RUNNING;

	return XST_SUCCESS;
}

/* ------------------------------------------------------------ */

/***	DisplayInitialize(DisplayCtrl *dispPtr, XAxiVdma *vdma, u16 vtcId, u32 dynClkAddr, u8 *framePtr[LCDMODE_MAIN_PAGES], u32 stride)
**
**	Parameters:
**		dispPtr - Pointer to the struct that will be initialized
**		vdma - Pointer to initialized VDMA struct
**		vtcId - Device ID of the VTC core as found in xparameters.h
**		dynClkAddr - BASE ADDRESS of the axi_dynclk core
**		framePtr - array of pointers to the frame buffers. The frame buffers must be instantiated above this driver, and there must be 3
**		stride - line stride of the frame buffers. This is the number of bytes between the start of one line and the start of another.
**
**	Return Value: int
**		XST_SUCCESS if successful, XST_FAILURE otherwise
**
**	Errors:
**
**	Description:
**		Initializes the driver struct for use.
**
*/
int DisplayInitialize(DisplayCtrl *dispPtr, XAxiVdma *vdma, u16 vtcId, u32 dynClkAddr, const uintptr_t * frames, u32 stride, const videomode_t * VMODE)
{
	int Status;
	int i;
	XVtc_Config *vtcConfig;


	/*
	 * Initialize all the fields in the DisplayCtrl struct
	 */
	dispPtr->curFrame = 0;
	dispPtr->dynClkAddr = dynClkAddr;
	for (i = 0; i < LCDMODE_MAIN_PAGES; i++)
	{
		dispPtr->framePhyAddr[i] = frames [i];
	}
	dispPtr->state = DISPLAY_STOPPED;
	dispPtr->stride = stride;

	/* Supported resolution */
	dispPtr->vMode = * VMODE;


	/* Initialize the VTC driver so that it's ready to use look up
	 * configuration in the config table, then initialize it.
	 */
	vtcConfig = XVtc_LookupConfig(vtcId);
	Status = XVtc_CfgInitialize(&(dispPtr->vtc), vtcConfig, vtcConfig->BaseAddress);
	/* Checking status */
	if (Status != (XST_SUCCESS)) {
		return (XST_FAILURE);
	}

	XVtc_IntrEnable(&(dispPtr->vtc), XVTC_IXR_G_VBLANK_MASK);
	arm_hardware_set_handler_system(XPAR_FABRIC_VIDEO_V_TC_0_IRQ_INTR, vtc_inthandler);
	vtc = dispPtr->vtc;

	dispPtr->vdma = vdma;

	/*
	 * Initialize the VDMA Read configuration struct
	 */
	dispPtr->vdmaConfig.FrameDelay = 0;
	dispPtr->vdmaConfig.EnableCircularBuf = 1;
	dispPtr->vdmaConfig.EnableSync = 0;
	dispPtr->vdmaConfig.PointNum = 0;
	dispPtr->vdmaConfig.EnableFrameCounter = 0;

	return XST_SUCCESS;
}
/* ------------------------------------------------------------ */

/***	DisplaySetMode(DisplayCtrl *dispPtr, const VideoMode *newMode)
**
**	Parameters:
**		dispPtr - Pointer to the initialized DisplayCtrl struct
**		newMode - The VideoMode struct describing the new mode.
**
**	Return Value: int
**		XST_SUCCESS if successful, XST_FAILURE otherwise
**
**	Errors:
**
**	Description:
**		Changes the resolution being output to the display. If the display
**		is currently started, it is automatically stopped (DisplayStart must
**		be called again).
**
*/
int DisplaySetMode(DisplayCtrl *dispPtr, const videomode_t *newMode)
{
	int Status;

	/*
	 * If currently running, stop
	 */
	if (dispPtr->state == DISPLAY_RUNNING)
	{
		Status = DisplayStop(dispPtr);
		if (Status != XST_SUCCESS)
		{
			PRINTF("ZYNQ VDMA: Cannot change mode, unable to stop display %d\n", Status);
			return XST_FAILURE;
		}
	}

	dispPtr->vMode = *newMode;

	return XST_SUCCESS;
}
/* ------------------------------------------------------------ */

/***	DisplayChangeFrame(DisplayCtrl *dispPtr, u32 frameIndex)
**
**	Parameters:
**		dispPtr - Pointer to the initialized DisplayCtrl struct
**		frameIndex - Index of the framebuffer to change to (must
**				be between 0 and (LCDMODE_MAIN_PAGES - 1))
**
**	Return Value: int
**		XST_SUCCESS if successful, XST_FAILURE otherwise
**
**	Errors:
**
**	Description:
**		Changes the frame currently being displayed.
**
*/

int DisplayChangeFrame(DisplayCtrl *dispPtr, u32 frameIndex)
{
	int Status;

	dispPtr->curFrame = frameIndex;
	/*
	 * If currently running, then the DMA needs to be told to start reading from the desired frame
	 * at the end of the current frame
	 */
	if (dispPtr->state == DISPLAY_RUNNING && vdma_sync)
	{
		IRQL_t oldIrql;
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		vdma_sync = 0;
		LowerIrql(oldIrql);
		Status = XAxiVdma_StartParking(dispPtr->vdma, dispPtr->curFrame, XAXIVDMA_READ);
		if (Status != XST_SUCCESS)
		{
			PRINTF("ZYNQ VDMA: Cannot change frame, unable to start parking %d\n", Status);
			return XST_FAILURE;
		}

		// Ожидание смены фреймбуфера
		u32 new_frameIndex;
		do {
			new_frameIndex = XAxiVdma_CurrFrameStore(dispPtr->vdma, XAXIVDMA_READ);
		} while (new_frameIndex != frameIndex);
	}

	return XST_SUCCESS;
}


/************************************************************************/

/*
void Vdma_Setup_Intr_System(XScuGic *GicInstancePtr, XAxiVdma *InstancePtr, u16 IntrId)
{
        XScuGic_Connect(GicInstancePtr, IntrId,
                        (Xil_ExceptionHandler)XAxiVdma_ReadIntrHandler,
                        InstancePtr);

        XScuGic_Enable(GicInstancePtr, IntrId);

        XAxiVdma_SetCallBack(InstancePtr, XAXIVDMA_HANDLER_ERROR, ReadErrorCallBack, (void *)InstancePtr, XAXIVDMA_READ);
        XAxiVdma_IntrEnable(InstancePtr, XAXIVDMA_IXR_ERROR_MASK, XAXIVDMA_READ);
 }
 */

void ReadErrorCallBack(void *CallbackRef, u32 Mask)
{

	if (Mask & XAXIVDMA_IXR_ERROR_MASK)
	{
		PRINTF("VDMA READ ERROR!\n" );
	}
}

void Vdma_Init(XAxiVdma *InstancePtr, u32 DeviceId)
{

	XAxiVdma_Config *Config;
	int Status;

	Config = XAxiVdma_LookupConfig(DeviceId);
	Status = XAxiVdma_CfgInitialize(InstancePtr, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		PRINTF("ZYNQ VDMA: Configuration Initialization failed %d\n", Status);
		return;
	}

	PRINTF("ZYNQ VDMA: Configuration Initialization success\n");
}

int Vdma_Start(XAxiVdma *InstancePtr)
{
	int Status;

	Status = XAxiVdma_DmaStart(InstancePtr, XAXIVDMA_READ);
	if (Status != XST_SUCCESS) {
		PRINTF("ZYNQ VDMA: Start read transfer failed %d\n", Status);

		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

#endif /* CPUSTYLE_XC7Z */

#endif /* WITHLTDCHW */
