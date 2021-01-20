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

#define DMA_BDUFFERSIZE 4000
#define PERIODSAMPLES 128
static u32 buf[PERIODSAMPLES];

typedef struct
{
	u8 spiChipAddr;
	int spiFifoWordsize;

	XAxiDma dmaAxiController;
	XAxiDma_Bd dmaBdBuffer[DMA_BDUFFERSIZE] __attribute__((aligned(XAXIDMA_BD_MINIMUM_ALIGNMENT)));
	int dmaWritten;
} test_config;
test_config pDevice;

void xc7z_dma_init(void)
{
	int Status = XAxiDma_CfgInitialize(&pDevice.dmaAxiController, XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID));
	if(XST_SUCCESS != Status)
	{
		PRINTF("DMA init fail\n");
		ASSERT(0);
	}

	if(!XAxiDma_HasSg(&pDevice.dmaAxiController))
	{
		PRINTF("Device configured as simple mode\n");
		ASSERT(0);
	}

	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(&pDevice.dmaAxiController);

	pDevice.dmaWritten = FALSE;

	// Disable all TX interrupts before TxBD space setup
	XAxiDma_BdRingIntDisable(TxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	// Setup TxBD space
	u32 BdCount = XAxiDma_BdRingCntCalc(XAXIDMA_BD_MINIMUM_ALIGNMENT,(u32) sizeof(pDevice.dmaBdBuffer));

	Status = XAxiDma_BdRingCreate(TxRingPtr, (UINTPTR)&pDevice.dmaBdBuffer[0], (UINTPTR)&pDevice.dmaBdBuffer[0], XAXIDMA_BD_MINIMUM_ALIGNMENT, BdCount);
	if (Status != XST_SUCCESS)
	{
		PRINTF("DMA init fail\n");
		ASSERT(0);
	}

	// Like the RxBD space, we create a template and set all BDs to be the
	// same as the template. The sender has to set up the BDs as needed.
	XAxiDma_Bd BdTemplate;
	XAxiDma_BdClear(&BdTemplate);
	Status = XAxiDma_BdRingClone(TxRingPtr, &BdTemplate);
	if (Status != XST_SUCCESS)
	{
		PRINTF("DMA init fail\n");
		ASSERT(0);
	}
	// Start the TX channel
	Status = XAxiDma_BdRingStart(TxRingPtr);
	//Status = XAxiDma_StartBdRingHw(TxRingPtr);
	if (Status != XST_SUCCESS)
	{
		PRINTF("DMA init fail\n");
		ASSERT(0);
	}
	XAxiDma_BdRingIntEnable(TxRingPtr, XAXIDMA_IRQ_IOC_MASK);

	double amp = 16384;
	for(int i = 0; i < PERIODSAMPLES; ++ i)
	{
		short left = (short) (cos((double) i / PERIODSAMPLES * 2 * M_PI) * amp);
		short right = (short) (sin((double) i / PERIODSAMPLES * 2 * M_PI) * amp);
		buf[i] = (left << 16) + (right & 0xFFFF);
	}
	xc7z_dma_transmit(buf, PERIODSAMPLES, 100);
}

void xc7z_dmaFreeProcessedBDs(void)
{
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(&pDevice.dmaAxiController);

	// Get all processed BDs from hardware
	XAxiDma_Bd *BdPtr;
	int BdCount = XAxiDma_BdRingFromHw(TxRingPtr, XAXIDMA_ALL_BDS, &BdPtr);

	// Free all processed BDs for future transmission
	int Status = XAxiDma_BdRingFree(TxRingPtr, BdCount, BdPtr);
	if (Status != XST_SUCCESS) {
		PRINTF("Failed to free BDs\n");
		ASSERT(0);
	}
}

void xc7z_dma_transmit(u32 *buffer, size_t buffer_len, u32 nRepeats)
{
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(&pDevice.dmaAxiController);

	// Free the processed BDs from previous run.
	xc7z_dmaFreeProcessedBDs();

	// Flush the SrcBuffer before the DMA transfer, in case the Data Cache is enabled
	Xil_DCacheFlushRange((u32)buffer, buffer_len * sizeof(u32));

	XAxiDma_Bd *BdPtr = NULL;
	int Status = XAxiDma_BdRingAlloc(TxRingPtr, nRepeats, &BdPtr);
	if (Status != XST_SUCCESS) {
		PRINTF("Failed bd alloc\n");
		ASSERT(0);
	}

	XAxiDma_Bd *BdCurPtr = BdPtr;;
	for(int i=0;i<nRepeats;++i)
	{
		Status = XAxiDma_BdSetBufAddr(BdCurPtr, (UINTPTR)buffer);
		if (Status != XST_SUCCESS) {
			PRINTF("Tx set buffer addr failed\n");
			ASSERT(0);
		}

		Status = XAxiDma_BdSetLength(BdCurPtr, buffer_len*sizeof(u32),	TxRingPtr->MaxTransferLen);
		if (Status != XST_SUCCESS) {
			PRINTF("Tx set length failed\n");
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
		PRINTF("Failed to hw\n");
		ASSERT(0);
	}

	pDevice.dmaWritten = TRUE;
}

void xc7z_dma_intHandler(void)
{
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(&pDevice.dmaAxiController);
	XAxiDma_BdRingAckIrq(TxRingPtr, XAXIDMA_IRQ_IOC_MASK);
	xc7z_dma_transmit(buf, PERIODSAMPLES, 4000);
}

#endif /* CPUSTYLE_XC7Z */
