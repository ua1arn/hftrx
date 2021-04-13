#include "hardware.h"
#include "board.h"
#include "audio.h"
#include "formats.h"
#include <math.h>

#if CPUSTYLE_XC7Z
#include "lib/zynq/src/xaxidma.h"
#include "lib/zynq/src/xaxidma_bdring.h"
#include "lib/zynq/src/xparameters.h"
#include "lib/zynq/src/xil_types.h"
#include "lib/zynq/src/xstatus.h"

XAxiDma xc7z_axidma_af_tx;
XAxiDma_Bd xc7z_axidmaBDspace_af_tx[1] __attribute__((aligned(XAXIDMA_BD_MINIMUM_ALIGNMENT)));

uintptr_t dma_invalidate32rx(uintptr_t addr);
void xc7z_dma_intHandler_af_tx(void);
void xc7z_dma_transmit(XAxiDma * dmaptr, u32 *buffer, size_t buffer_len);

static void xc7z_dma_init_tx(u32 axi_dma_id, XAxiDma * dmaptr, XAxiDma_Bd * dmaBDptr)
{
	int Status = XAxiDma_CfgInitialize(dmaptr, XAxiDma_LookupConfig(axi_dma_id));
	if(XST_SUCCESS != Status)
	{
		PRINTF("XAxiDma_CfgInitialize fail %d\n", Status);
		ASSERT(0);
	}

	if(!XAxiDma_HasSg(dmaptr))
	{
		PRINTF("AxiDma configured as simple mode\n");
		ASSERT(0);
	}

	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(dmaptr);

	// Disable all TX interrupts before TxBD space setup
	XAxiDma_BdRingIntDisable(TxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	// Setup TxBD space
	u32 BdCount = XAxiDma_BdRingCntCalc(XAXIDMA_BD_MINIMUM_ALIGNMENT,(u32) sizeof(xc7z_axidmaBDspace_af_tx));

	Status = XAxiDma_BdRingCreate(TxRingPtr, (UINTPTR)dmaBDptr[0], (UINTPTR)dmaBDptr[0], XAXIDMA_BD_MINIMUM_ALIGNMENT, BdCount);
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
}

void xc7z_dma_init_af_tx(void)
{
	xc7z_dma_init_tx(XPAR_AXI_DMA_0_DEVICE_ID, & xc7z_axidma_af_tx, xc7z_axidmaBDspace_af_tx);
	arm_hardware_set_handler_realtime(XPAR_FABRIC_AXIDMA_0_VEC_ID, xc7z_dma_intHandler_af_tx);

	// пнуть для запуска прерываний, без этого не идут
	double amp = 16383;
	static u32 buf[128] __attribute__((aligned(64)));
	for(int i = 0; i < 128; ++ i)
	{
		short left = (short) (cosf((double) i / 128 * 2 * M_PI) * amp);
		short right = (short) (sinf((double) i / 128 * 2 * M_PI) * amp);
		buf[i] = (left << 16) + (right & 0xFFFF);
	}
	xc7z_dma_transmit(& xc7z_axidma_af_tx, buf, 128);
}

void xc7z_dma_transmit(XAxiDma * dmaptr, u32 *buffer, size_t buffer_len)
{
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(dmaptr);

	XAxiDma_Bd *BdPtr;
	int BdCount = XAxiDma_BdRingFromHw(TxRingPtr, XAXIDMA_ALL_BDS, &BdPtr);

	// Free all processed BDs for future transmission
	int Status = XAxiDma_BdRingFree(TxRingPtr, BdCount, BdPtr);
	if (Status != XST_SUCCESS) {
		PRINTF("XAxiDma_BdRingFree fail %d\n", Status);
	}

	// Flush the SrcBuffer before the DMA transfer, in case the Data Cache is enabled
	Xil_DCacheFlushRange((u32)buffer, buffer_len * sizeof(aubufv_t));

	//* BdPtr = NULL;
	Status = XAxiDma_BdRingAlloc(TxRingPtr, 1, &BdPtr);
	if (Status != XST_SUCCESS) {
		PRINTF("XAxiDma_BdRingAlloc fail %d\n", Status);
		ASSERT(0);
	}

	XAxiDma_Bd *BdCurPtr = BdPtr;

	Status = XAxiDma_BdSetBufAddr(BdCurPtr, (UINTPTR)buffer);
	if (Status != XST_SUCCESS) {
		PRINTF("XAxiDma_BdSetBufAddr fail %d\n", Status);
		ASSERT(0);
	}

	Status = XAxiDma_BdSetLength(BdCurPtr, buffer_len * sizeof(aubufv_t), TxRingPtr->MaxTransferLen);
	if (Status != XST_SUCCESS) {
		PRINTF("XAxiDma_BdSetLength fail %d\n", Status);
		ASSERT(0);
	}

	u32 CrBits = 0;
	CrBits |= XAXIDMA_BD_CTRL_TXSOF_MASK; // First BD
	CrBits |= XAXIDMA_BD_CTRL_TXEOF_MASK; // Last BD

	XAxiDma_BdSetCtrl(BdCurPtr, CrBits);
	XAxiDma_BdSetId(BdCurPtr, (UINTPTR)buffer);

	BdCurPtr = (XAxiDma_Bd *)XAxiDma_BdRingNext(TxRingPtr, BdCurPtr);

	// Give the BD to hardware
	Status = XAxiDma_BdRingToHw(TxRingPtr, 1, BdPtr);
	if (Status != XST_SUCCESS) {
		PRINTF("XAxiDma_BdRingToHw fail %d\n", Status);
		ASSERT(0);
	}
}

// Сейчас эта память будет записываться по DMA куда-то
// Потом содержимое не требуется
static uintptr_t
xdma_flush16tx(uintptr_t addr)
{
	ASSERT((addr % DCACHEROWSIZE) == 0);
	//ASSERT((buffers_dmabuffer16cachesize() % DCACHEROWSIZE) == 0);
	arm_hardware_flush(addr, buffers_dmabuffer16cachesize());
	return addr;
}
void xc7z_dma_intHandler_af_tx(void)
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
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(& xc7z_axidma_af_tx);
	XAxiDma_BdRingAckIrq(TxRingPtr, XAXIDMA_IRQ_ALL_MASK);
	uintptr_t addr = getfilled_dmabuffer16phones();
	//xc7z_dma_transmit(buf, BUFLEN, REPEATS);
	xc7z_dma_transmit(& xc7z_axidma_af_tx, (u32 *) addr, DMABUFFSIZE16);
	release_dmabuffer16(addr);
	//dbg_putchar('.');
}

#endif /* CPUSTYLE_XC7Z */
