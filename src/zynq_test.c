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

#define DATA_FIFO_FABRIC_INTERRUPT		63

XAxiDma xc7z_axidma_af_tx;
XAxiDma xc7z_axidma_if_rx;

uintptr_t dma_invalidate32rx(uintptr_t addr);
void xc7z_dma_intHandler_af_tx(void);

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

void xc7z_dma_transmit(XAxiDma * dmaptr, UINTPTR buffer, size_t buffer_len)
{
	Xil_DCacheFlushRange(buffer, buffer_len);
	int Status = XAxiDma_SimpleTransfer(& xc7z_axidma_af_tx, buffer, buffer_len, XAXIDMA_DMA_TO_DEVICE);
	if (Status != XST_SUCCESS)
	{
		PRINTF("dma transfet error %d\n", Status);
		ASSERT(0);
	}
}

static void xc7z_dma_init_rx(void)
{

}

static void xc7z_dma_init_tx(void)
{
	XAxiDma_Config * txConfig = XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);
	int Status = XAxiDma_CfgInitialize(& xc7z_axidma_af_tx, txConfig);

	if (Status != XST_SUCCESS) {
		xil_printf("Initialization failed %d\r\n", Status);
		ASSERT(0);
	}

	if(XAxiDma_HasSg(& xc7z_axidma_af_tx))
	{
		xil_printf("Device configured as SG mode \r\n");
		ASSERT(0);
	}

	XAxiDma_IntrDisable(& xc7z_axidma_af_tx, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrDisable(& xc7z_axidma_af_tx, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrEnable(& xc7z_axidma_af_tx, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
	arm_hardware_set_handler_realtime(XPAR_FABRIC_AXIDMA_0_VEC_ID, xc7z_dma_intHandler_af_tx);
}

void xc7z_dma_init_af_tx(void)
{
	xc7z_dma_init_tx();

	// пнуть для запуска прерываний, без этого не идут
	double amp = 16383;
	static u32 buf[128] __attribute__((aligned(64)));
	for(int i = 0; i < 128; ++ i)
	{
		short left = (short) (cosf((double) i / 128 * 2 * M_PI) * amp);
		short right = (short) (sinf((double) i / 128 * 2 * M_PI) * amp);
		buf[i] = (left << 16) + (right & 0xFFFF);
	}

	xc7z_dma_transmit(& xc7z_axidma_af_tx, (UINTPTR) buf, 128);
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

	u32 IrqStatus = XAxiDma_IntrGetIrq(& xc7z_axidma_af_tx, XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_IntrAckIrq(& xc7z_axidma_af_tx, IrqStatus, XAXIDMA_DMA_TO_DEVICE);

	uintptr_t addr = getfilled_dmabuffer16phones();
	xc7z_dma_transmit(& xc7z_axidma_af_tx, addr, DMABUFFSIZE16 * sizeof(aubufv_t));

	release_dmabuffer16(addr);
	//dbg_putchar('.');
}

void xc7z_data_fifo(void)
{
	TP();
}

#endif /* CPUSTYLE_XC7Z */
