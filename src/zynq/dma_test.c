#include "hardware.h"
#include "board.h"
#include "audio.h"
#include "formats.h"
#include <math.h>

#if CPUSTYLE_XC7Z
#include "xaxidma.h"
#include "xaxidma_bdring.h"
#include "xparameters.h"
#include "xil_types.h"
#include "xstatus.h"

#define REPEATS 		5
#define PERIODSAMPLES 	128
#define BUFLEN			(PERIODSAMPLES * REPEATS)
static u32 buf[BUFLEN];

XAxiDma xc7z_axidma;
XAxiDma_Bd xc7z_axidmaBDspace[REPEATS] __attribute__((aligned(XAXIDMA_BD_MINIMUM_ALIGNMENT)));

void xc7z_dma_init(void)
{
	int Status = XAxiDma_CfgInitialize(&xc7z_axidma, XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID));
	if(XST_SUCCESS != Status)
	{
		PRINTF("XAxiDma_CfgInitialize fail %d\n", Status);
		ASSERT(0);
	}

	if(!XAxiDma_HasSg(&xc7z_axidma))
	{
		PRINTF("AxiDma configured as simple mode\n");
		ASSERT(0);
	}

	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(&xc7z_axidma);

	// Disable all TX interrupts before TxBD space setup
	XAxiDma_BdRingIntDisable(TxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	// Setup TxBD space
	u32 BdCount = XAxiDma_BdRingCntCalc(XAXIDMA_BD_MINIMUM_ALIGNMENT,(u32) sizeof(xc7z_axidmaBDspace));

	Status = XAxiDma_BdRingCreate(TxRingPtr, (UINTPTR)&xc7z_axidmaBDspace[0], (UINTPTR)&xc7z_axidmaBDspace[0], XAXIDMA_BD_MINIMUM_ALIGNMENT, BdCount);
	if (Status != XST_SUCCESS)
	{
		PRINTF("XAxiDma_BdRingCreate fail %d\n", Status);
		ASSERT(0);
	}

	// Like the RxBD space, we create a template and set all BDs to be the
	// same as the template. The sender has to set up the BDs as needed.
	XAxiDma_Bd BdTemplate;
	XAxiDma_BdClear(&BdTemplate);
	Status = XAxiDma_BdRingClone(TxRingPtr, &BdTemplate);
	if (Status != XST_SUCCESS)
	{
		PRINTF("XAxiDma_BdRingClone fail %d\n", Status);
		ASSERT(0);
	}
	// Start the TX channel
	Status = XAxiDma_BdRingStart(TxRingPtr);
	//Status = XAxiDma_StartBdRingHw(TxRingPtr);
	if (Status != XST_SUCCESS)
	{
		PRINTF("XAxiDma_BdRingStart fail %d\n", Status);
		ASSERT(0);
	}
	XAxiDma_BdRingIntEnable(TxRingPtr, XAXIDMA_IRQ_IOC_MASK);
	arm_hardware_set_handler_realtime(XPAR_FABRIC_AXIDMA_0_VEC_ID, xc7z_dma_intHandler);

	double amp = 16384;
	for(int i = 0; i < BUFLEN; ++ i)
	{
		short left = (short) (cos((double) i / PERIODSAMPLES * 2 * M_PI) * amp);
		short right = (short) (sin((double) i / PERIODSAMPLES * 2 * M_PI) * amp);
		buf[i] = (left << 16) + (right & 0xFFFF);
	}

	xc7z_dma_transmit(buf, BUFLEN, REPEATS);
}

void xc7z_dmaFreeProcessedBDs(void)
{
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(&xc7z_axidma);

	// Get all processed BDs from hardware
	XAxiDma_Bd *BdPtr;
	int BdCount = XAxiDma_BdRingFromHw(TxRingPtr, XAXIDMA_ALL_BDS, &BdPtr);

	// Free all processed BDs for future transmission
	int Status = XAxiDma_BdRingFree(TxRingPtr, BdCount, BdPtr);
	if (Status != XST_SUCCESS) {
		PRINTF("XAxiDma_BdRingFree fail %d\n", Status);
	}
}

void xc7z_dma_transmit(u32 *buffer, size_t buffer_len, u32 nRepeats)
{
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(&xc7z_axidma);

	// Free the processed BDs from previous run.
	xc7z_dmaFreeProcessedBDs();

	// Flush the SrcBuffer before the DMA transfer, in case the Data Cache is enabled
	Xil_DCacheFlushRange((u32)buffer, buffer_len * sizeof(u32));

	XAxiDma_Bd *BdPtr = NULL;
	int Status = XAxiDma_BdRingAlloc(TxRingPtr, nRepeats, &BdPtr);
	if (Status != XST_SUCCESS) {
		PRINTF("XAxiDma_BdRingAlloc fail %d\n", Status);
		ASSERT(0);
	}

	XAxiDma_Bd *BdCurPtr = BdPtr;;
	for(int i=0;i<nRepeats;++i)
	{
		Status = XAxiDma_BdSetBufAddr(BdCurPtr, (UINTPTR)buffer);
		if (Status != XST_SUCCESS) {
			PRINTF("XAxiDma_BdSetBufAddr fail %d\n", Status);
			ASSERT(0);
		}

		Status = XAxiDma_BdSetLength(BdCurPtr, buffer_len*sizeof(aubufv_t),	TxRingPtr->MaxTransferLen);
		if (Status != XST_SUCCESS) {
			PRINTF("XAxiDma_BdSetLength fail %d\n", Status);
			ASSERT(0);
		}

		u32 CrBits = 0;
		if (i==0) {
			CrBits |= XAXIDMA_BD_CTRL_TXSOF_MASK; // First BD
		}
		if (i==nRepeats-1) {
			CrBits |= XAXIDMA_BD_CTRL_TXEOF_MASK; // Last BD
		}
		XAxiDma_BdSetCtrl(BdCurPtr, CrBits);

		XAxiDma_BdSetId(BdCurPtr, (UINTPTR)buffer);

		BdCurPtr = (XAxiDma_Bd *)XAxiDma_BdRingNext(TxRingPtr, BdCurPtr);
	}

	// Give the BD to hardware
	Status = XAxiDma_BdRingToHw(TxRingPtr, nRepeats, BdPtr);
	if (Status != XST_SUCCESS) {
		PRINTF("XAxiDma_BdRingToHw fail %d\n", Status);
		ASSERT(0);
	}
}

uintptr_t dma_invalidate32rx(uintptr_t addr);

void xc7z_dma_intHandler(void)
{
	uintptr_t a1 = dma_invalidate32rx(allocate_dmabuffer32rx());
	processing_dmabuffer32rx(a1);
	release_dmabuffer32rx(a1);

	a1 = dma_invalidate32rx(allocate_dmabuffer32rx());
	processing_dmabuffer32rx(a1);
	release_dmabuffer32rx(a1);

	a1 = dma_invalidate32rx(allocate_dmabuffer32rx());
	processing_dmabuffer32rx(a1);
	release_dmabuffer32rx(a1);

	a1 = dma_invalidate32rx(allocate_dmabuffer32rx());
	processing_dmabuffer32rx(a1);
	release_dmabuffer32rx(a1);




	//dbg_putchar('-');
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(&xc7z_axidma);
	XAxiDma_BdRingAckIrq(TxRingPtr, XAXIDMA_IRQ_ALL_MASK);
	uintptr_t addr = getfilled_dmabuffer16phones();
	//xc7z_dma_transmit(buf, BUFLEN, REPEATS);
	xc7z_dma_transmit((u32 *) addr, DMABUFFSIZE16, 1);
	release_dmabuffer16(addr);
	//dbg_putchar('.');
}

#endif /* CPUSTYLE_XC7Z */
