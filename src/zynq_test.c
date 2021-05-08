#include "hardware.h"
#include "board.h"
#include "audio.h"
#include "formats.h"
#include <math.h>

#if CPUSTYLE_XC7Z && ! WITHISBOOTLOADER
#include "lib/zynq/src/xaxidma.h"
#include "lib/zynq/src/xaxidma_bdring.h"
#include "lib/zynq/src/xparameters.h"
#include "lib/zynq/src/xil_types.h"
#include "lib/zynq/src/xstatus.h"
#include "lib/zynq/src/xgpio.h"
#include "lib/zynq/src/xllfifo.h"

XAxiDma xc7z_axidma_af_tx;
XGpio xc7z_nco;
XLlFifo fifo;
uintptr_t rx_buf = 0;
size_t rx_buf_size = DMABUFFSIZE32RX * 2;

uintptr_t dma_invalidate32rx(uintptr_t addr);
void xc7z_dma_intHandler_af_tx(void);
void xc7z_if_fifo_inthandler(void);
int XLlFifo_iRead_Aligned(XLlFifo *InstancePtr, void *BufPtr, unsigned WordCount);

void xc7z_dds_ftw(const uint_least64_t * val)
{
	XGpio_DiscreteWrite(& xc7z_nco, 1, * val);
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

void xc7z_dma_transmit(UINTPTR buffer, size_t buffer_len)
{
	arm_hardware_flush((uintptr_t) buffer, buffer_len);
	int Status = XAxiDma_SimpleTransfer(& xc7z_axidma_af_tx, buffer, buffer_len, XAXIDMA_DMA_TO_DEVICE);
	if (Status != XST_SUCCESS)
	{
		PRINTF("dma transmit error %d\n", Status);
		ASSERT(0);
	}
	while(XAxiDma_Busy(& xc7z_axidma_af_tx, XAXIDMA_DMA_TO_DEVICE));
}

void xc7z_if_fifo_init(void)
{
	XLlFifo_Config *pConfig = XLlFfio_LookupConfig(XPAR_AXI_FIFO_0_DEVICE_ID);
	int xStatus = XLlFifo_CfgInitialize(& fifo,pConfig,pConfig->BaseAddress);
	if(XST_SUCCESS != xStatus) {
		xil_printf("XLlFifo_CfgInitialize fail %d \n", xStatus);
		ASSERT(0);
	}

	// Check for the Reset value
	u32 Status = XLlFifo_Status(& fifo);
	XLlFifo_IntClear(& fifo, 0xffffffff);
	Status = XLlFifo_Status(& fifo);
	if(Status != 0) {
		xil_printf("XLlFifo reset fail %x \n", Status);
		ASSERT(0);
	}

	XLlFifo_IntEnable(& fifo, XLLF_INT_RFPF_MASK);
	arm_hardware_set_handler_realtime(XPAR_FABRIC_LLFIFO_0_VEC_ID, xc7z_if_fifo_inthandler);

	Status = XGpio_Initialize(& xc7z_nco, XPAR_AXI_GPIO_0_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		PRINTF("nco init error %d\n", Status);
		ASSERT(0);
	}
}

void xc7z_dma_init_af_tx(void)
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
}

void xc7z_if_fifo_inthandler(void)
{
	static uint_fast8_t rx_stage = 0;
	static uint_fast8_t rx_index = 0;

	if ( XLlFifo_iRxOccupancy(& fifo) > DMABUFFSIZE32RX)
	{
		rx_buf = allocate_dmabuffer32rx();
		XLlFifo_iRead_Aligned(& fifo, (uint32_t *) rx_buf, DMABUFFSIZE32RX);
		processing_dmabuffer32rx(rx_buf);
		release_dmabuffer32rx(rx_buf);
		XLlFifo_IntClear(& fifo, XLLF_INT_RFPF_MASK);
		rx_stage ++;
	}

	if (rx_stage == 4)
	{
		uintptr_t addr = getfilled_dmabuffer16phones();
		xc7z_dma_transmit(addr, DMABUFFSIZE16 * sizeof(aubufv_t));
		release_dmabuffer16(addr);
		rx_stage = 0;
	}
}

#endif /* CPUSTYLE_XC7Z */
