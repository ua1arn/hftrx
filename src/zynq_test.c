#include "hardware.h"
#include "board.h"
#include "audio.h"
#include "formats.h"
#include <math.h>

#if (CPUSTYLE_XC7Z || CPUSTYLE_XCZU) && ! WITHISBOOTLOADER
#if WITHRTS96
#include "xaxidma.h"
#include "xparameters.h"
#include "xil_types.h"
#include "xstatus.h"
#include "xllfifo.h"

XAxiDma xc7z_axidma_phones;
XLlFifo adc_iq_fifo;

uintptr_t dma_invalidate32rx(uintptr_t addr);
void xc7z_if_fifo_inthandler(void);
int XLlFifo_iRead_Aligned(XLlFifo *InstancePtr, void *BufPtr, unsigned WordCount);

void xc7z_rxtx_state(uint8_t tx)
{

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
	int Status = XAxiDma_SimpleTransfer(& xc7z_axidma_phones, buffer, buffer_len, XAXIDMA_DMA_TO_DEVICE);
	if (Status != XST_SUCCESS)
	{
		PRINTF("dma transmit error %d\n", Status);
		ASSERT(0);
	}
	while(XAxiDma_Busy(& xc7z_axidma_phones, XAXIDMA_DMA_TO_DEVICE));
}

void xc7z_if_fifo_init(void)
{
	XLlFifo_Config * pConfig_rx = XLlFfio_LookupConfig(XPAR_AXI_FIFO_0_DEVICE_ID);
	int xStatus = XLlFifo_CfgInitialize(& adc_iq_fifo, pConfig_rx, pConfig_rx->BaseAddress);
	if(XST_SUCCESS != xStatus) {
		xil_printf("adc_iq_fifo CfgInitialize fail %d \n", xStatus);
		ASSERT(0);
	}

	// Check for the Reset value
	u32 Status = XLlFifo_Status(& adc_iq_fifo);
	XLlFifo_IntClear(& adc_iq_fifo, 0xffffffff);
	Status = XLlFifo_Status(& adc_iq_fifo);
	if(Status != 0) {
		xil_printf("adc_iq_fifo reset fail %x \n", Status);
		ASSERT(0);
	}

	XLlFifo_IntDisable(& adc_iq_fifo, XLLF_INT_ALL_MASK);
	XLlFifo_IntEnable(& adc_iq_fifo, XLLF_INT_RFPF_MASK);
	arm_hardware_set_handler_realtime(XPAR_FABRIC_AXI_FIFO_MM_S_0_INTERRUPT_INTR, xc7z_if_fifo_inthandler);
}

void xc7z_dma_init_af_tx(void)
{
	XAxiDma_Config * txConfig = XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);
	int Status = XAxiDma_CfgInitialize(& xc7z_axidma_phones, txConfig);

	if (Status != XST_SUCCESS) {
		xil_printf("Initialization failed %d\r\n", Status);
		ASSERT(0);
	}

	if(XAxiDma_HasSg(& xc7z_axidma_phones))
	{
		xil_printf("Device configured as SG mode \r\n");
		ASSERT(0);
	}

	XAxiDma_IntrDisable(& xc7z_axidma_phones, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrDisable(& xc7z_axidma_phones, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
}

void xc7z_if_fifo_inthandler(void)
{
	enum { CNT16 = DMABUFFSIZE16 / DMABUFSTEP16 };
	enum { CNT32RX = DMABUFFSIZE32RX / DMABUFSTEP32RX };
	static unsigned rx_stage = 0;

	if (XLlFifo_iRxOccupancy(& adc_iq_fifo) >= DMABUFFSIZE32RX)
	{
		XLlFifo_IntClear(& adc_iq_fifo, XLLF_INT_RFPF_MASK);
		uintptr_t rx_buf = allocate_dmabuffer32rx();
		XLlFifo_iRead_Aligned(& adc_iq_fifo, (uint32_t *) rx_buf, DMABUFFSIZE32RX);
		processing_dmabuffer32rx(rx_buf);
		release_dmabuffer32rx(rx_buf);
		rx_stage += CNT32RX;	// количество сэмплолв прошельших по каналу обмена с FPGA
		buffers_resampleuacin(CNT32RX);
	}

	while (rx_stage >= CNT16)
	{
		const uintptr_t addr = getfilled_dmabuffer16phones();
		xc7z_dma_transmit(addr, DMABUFFSIZE16 * sizeof(aubufv_t));
		release_dmabuffer16(addr);
		rx_stage -= CNT16;
	}
}
#else
void xc7z_if_fifo_init(void)
{
}

void xc7z_dma_init_af_tx(void)
{
}

void xc7z_dds_ftw(const uint_least64_t * val)
{
}

void xc7z_dds_rts(const uint_least64_t * val)
{
}
#endif /* WITHRTS96 */
#endif /* CPUSTYLE_XC7Z */
