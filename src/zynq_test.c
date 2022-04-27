#include "hardware.h"
#include "board.h"
#include "audio.h"
#include "formats.h"
#include <math.h>

#if (CPUSTYLE_XC7Z || CPUSTYLE_XCZU) && ! WITHISBOOTLOADER

#include "xaxidma.h"
#include "xaxidma_bdring.h"
#include "xparameters.h"
#include "xil_types.h"
#include "xstatus.h"
#include "xllfifo.h"

#include "zynq_test.h"

XAxiDma xcz_axidma_if_tx;
XLlFifo fifo_mic_rx, fifo_ph_tx, fifo_if_rx;
u32 sinbuf32[DMABUFFSIZE16];

uintptr_t dma_invalidate32rx(uintptr_t addr);
int XLlFifo_iRead_Aligned(XLlFifo *InstancePtr, void *BufPtr, unsigned WordCount);
int XLlFifo_iWrite_Aligned(XLlFifo *InstancePtr, void *BufPtr, unsigned WordCount);

void xcz_rxtx_state(uint8_t tx)
{
	Xil_Out32(XPAR_TRX_CONTROL2_0_S00_AXI_BASEADDR + 0, (tx != 0));
}

void xcz_dds_ftw(const uint_least64_t * val)
{
	Xil_Out32(XPAR_TRX_CONTROL2_0_S00_AXI_BASEADDR + 4, * val);
}

void xcz_dds_rts(const uint_least64_t * val)
{
	Xil_Out32(XPAR_TRX_CONTROL2_0_S00_AXI_BASEADDR + 8, * val);
}

void xcz_rx_iq_shift(uint32_t val) // 48
{
//	val = val > 56 ? 56 : val;
	Xil_Out32(XPAR_TRX_CONTROL2_0_S00_AXI_BASEADDR + 12, val);
}

void xcz_dds_ftw_sub(const uint_least64_t * val)
{
	Xil_Out32(XPAR_TRX_CONTROL2_0_S00_AXI_BASEADDR + 28, * val);
}

void xcz_rx_cic_shift(uint32_t val)
{
	Xil_Out32(XPAR_TRX_CONTROL2_0_S00_AXI_BASEADDR + 16, val);
}

void xcz_tx_shift(uint32_t val)
{
	//val = val > 94 ? 94 : val;
	Xil_Out32(XPAR_TRX_CONTROL2_0_S00_AXI_BASEADDR + 20, val);
}

void xcz_adcrand_set(uint8_t val)
{
	Xil_Out32(XPAR_TRX_CONTROL2_0_S00_AXI_BASEADDR + 24, (val != 0));
}


void xcz_ah_preinit(void)
{
	axi_i2s_adi_WriteReg(XPAR_AXI_I2S_ADI_0_BASEADDR, AUDIO_REG_I2S_CLK_CTRL, (64 / 2 - 1) << 16 | (4 / 2 - 1));
	axi_i2s_adi_WriteReg(XPAR_AXI_I2S_ADI_0_BASEADDR, AUDIO_REG_I2S_PERIOD, DMABUFFSIZE16);
	axi_i2s_adi_WriteReg(XPAR_AXI_I2S_ADI_0_BASEADDR, AUDIO_REG_I2S_CTRL, TX_ENABLE_MASK | RX_ENABLE_MASK);

	xcz_rxtx_state(1);
	xcz_rxtx_state(0);
	xcz_rx_iq_shift(42);
//	xcz_rx_cic_shift(87);
	xcz_tx_shift(23);		// 23
	xcz_adcrand_set(1);

	uint16_t ss = DMABUFFSIZE16;
	uint16_t amp = 16384;
    for(int i = 0; i < ss; ++i)
    {
    	uint16_t ii = cos(i / ss * 2 * M_PI) * amp;
		uint16_t qq = sin(i / ss * 2 * M_PI) * amp;
		sinbuf32[i] = (ii << 16) + (qq & 0xFFFF);
    }
}

// ****************** IF RX ******************

void xcz_if_rx_init(void)
{
	XLlFifo_Config * config = XLlFfio_LookupConfig(XPAR_AXI_FIFO_IQ_RX_DEVICE_ID);
	int xStatus = XLlFifo_CfgInitialize(& fifo_if_rx, config, config->BaseAddress);
	if(XST_SUCCESS != xStatus) {
		PRINTF("fifo_if_rx CfgInitialize fail %d \n", xStatus);
		ASSERT(0);
	}

	// Check for the Reset value
	u32 Status = XLlFifo_Status(& fifo_if_rx);
	XLlFifo_IntClear(& fifo_if_rx, 0xffffffff);
	Status = XLlFifo_Status(& fifo_if_rx);
	if(Status != 0) {
		PRINTF("fifo_if_rx reset fail %x \n", Status);
		ASSERT(0);
	}
}

void xcz_fifo_if_rx_inthandler(void)
{
	u32 ss = XLlFifo_Status(& fifo_if_rx);
	XLlFifo_IntClear(& fifo_if_rx, 0xffffffff);
	u32 occ = XLlFifo_RxOccupancy(& fifo_if_rx);

	static uint16_t iii = 0;

	if ((occ >= DMABUFFSIZE32RX) && (ss & XLLF_INT_RC_MASK))
	{
		uintptr_t addr = allocate_dmabuffer32rx();
		XLlFifo_iRead_Aligned(& fifo_if_rx, (IFADCvalue_t *) addr, DMABUFFSIZE32RX);
		processing_dmabuffer32rx(addr);
		release_dmabuffer32rx(addr);
	}
}

void xcz_if_rx_enable(uint_fast8_t state)
{
	XLlFifo_IntDisable(& fifo_if_rx, XLLF_INT_ALL_MASK);
	XLlFifo_IntEnable(& fifo_if_rx, XLLF_INT_RC_MASK);
	arm_hardware_set_handler_realtime(XPAR_FABRIC_AXI_FIFO_IQ_RX_INTERRUPT_INTR, xcz_fifo_if_rx_inthandler);
}

// ****************** IF TX ******************

void xcz_if_tx_init(void)
{
	XAxiDma_Config * iftxConfig = XAxiDma_LookupConfig(XPAR_AXI_DMA_IQ_TX_DEVICE_ID);
	int Status = XAxiDma_CfgInitialize(& xcz_axidma_if_tx, iftxConfig);

	if (Status != XST_SUCCESS) {
		PRINTF("xcz_axidma_if_tx Initialization failed %d\r\n", Status);
		ASSERT(0);
	}

	if(XAxiDma_HasSg(& xcz_axidma_if_tx))
	{
		PRINTF("xcz_axidma_if_tx Device configured as SG mode \r\n");
		ASSERT(0);
	}
}

void xcz_dma_transmit_if_tx(UINTPTR buffer)
{
	size_t len = DMABUFFSIZE32TX * sizeof(IFDACvalue_t);
	arm_hardware_flush((uintptr_t) buffer, len);
//	while(XAxiDma_Busy(& xcz_axidma_if_tx, XAXIDMA_DMA_TO_DEVICE));
	int Status = XAxiDma_SimpleTransfer(& xcz_axidma_if_tx, buffer, len, XAXIDMA_DMA_TO_DEVICE);
	if (Status != XST_SUCCESS)
	{
		PRINTF("xcz_axidma_if_tx transmit error %d\n", Status);
		ASSERT(0);
	}
}

void xcz_dma_if_tx_inthandler(void)
{
	XAxiDma_IntrAckIrq(& xcz_axidma_if_tx, XAXIDMA_IRQ_IOC_MASK, XAXIDMA_DMA_TO_DEVICE);
	const uintptr_t addr = getfilled_dmabuffer32tx_main();
	xcz_dma_transmit_if_tx(addr);
	release_dmabuffer32tx(addr);
}

void xcz_if_tx_enable(uint_fast8_t state)
{
//	XAxiDma_IntrDisable(& xcz_axidma_if_tx, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
//	XAxiDma_IntrEnable(& xcz_axidma_if_tx, XAXIDMA_IRQ_IOC_MASK, XAXIDMA_DMA_TO_DEVICE);
//	arm_hardware_set_handler_realtime(XPAR_FABRIC_AXI_DMA_IQ_TX_MM2S_INTROUT_INTR, xcz_dma_if_tx_inthandler);
//	xcz_dma_transmit_if_tx((uintptr_t) sinbuf32);	// пнуть, чтобы заработало
}

// ****************** Audio MIC receive ******************

void xcz_audio_rx_init(void)
{
	XLlFifo_Config * pConfig = XLlFfio_LookupConfig(XPAR_AXI_FIFO_MIC_DEVICE_ID);
	int xStatus = XLlFifo_CfgInitialize(& fifo_mic_rx, pConfig, pConfig->BaseAddress);
	if(XST_SUCCESS != xStatus) {
		PRINTF("fifo_mic_rx CfgInitialize fail %d \n", xStatus);
		ASSERT(0);
	}

	// Check for the Reset value
	u32 Status = XLlFifo_Status(& fifo_mic_rx);
	XLlFifo_IntClear(& fifo_mic_rx, 0xffffffff);
	Status = XLlFifo_Status(& fifo_mic_rx);
	if(Status != 0) {
		PRINTF("fifo_mic_rx reset fail %x \n", Status);
		ASSERT(0);
	}
}

void xcz_fifo_mic_inthandler(void)
{
	u32 ss = XLlFifo_Status(& fifo_mic_rx);
	XLlFifo_IntClear(& fifo_mic_rx, 0xffffffff);
	u32 occ = XLlFifo_RxOccupancy(& fifo_mic_rx);

	if ((occ > DMABUFFSIZE16) && (ss & XLLF_INT_RC_MASK))
	{
		uintptr_t addr = allocate_dmabuffer16();
		XLlFifo_iRead_Aligned(& fifo_mic_rx, (aubufv_t *) addr, DMABUFFSIZE16);
		processing_dmabuffer16rx(addr);
	}
}

void xcz_audio_rx_enable(uint_fast8_t state)
{
//	XLlFifo_IntDisable(& fifo_mic_rx, XLLF_INT_ALL_MASK);
//	XLlFifo_IntEnable(& fifo_mic_rx, XLLF_INT_RC_MASK);
//	arm_hardware_set_handler_realtime(XPAR_FABRIC_AXI_FIFO_MIC_INTERRUPT_INTR, xcz_fifo_mic_inthandler);
}

// ****************** Audio phones transmit ******************

void xcz_audio_tx_init(void)
{
	XLlFifo_Config * pConfig_phtx = XLlFfio_LookupConfig(XPAR_AXI_FIFO_PHONES_DEVICE_ID);
	int xStatus = XLlFifo_CfgInitialize(& fifo_ph_tx, pConfig_phtx, pConfig_phtx->BaseAddress);
	if(XST_SUCCESS != xStatus) {
		PRINTF("fifo_ph_tx CfgInitialize fail %d \n", xStatus);
		ASSERT(0);
	}

	// Check for the Reset value
	u32 Status = XLlFifo_Status(& fifo_ph_tx);
	XLlFifo_IntClear(& fifo_ph_tx, 0xffffffff);
	Status = XLlFifo_Status(& fifo_ph_tx);
	if(Status != 0) {
		PRINTF("fifo_ph_tx reset fail %x \n", Status);
		ASSERT(0);
	}
}

void xcz_fifo_if_tx_inthandler(void)
{
	u32 ss = XLlFifo_Status(& fifo_ph_tx);
	XLlFifo_IntClear(& fifo_ph_tx, 0xffffffff);
	u32 vac = XLlFifo_TxVacancy(& fifo_ph_tx);

	if (ss & XLLF_INT_TC_MASK)
	{
		const uintptr_t addr = getfilled_dmabuffer16phones();
		XLlFifo_iWrite_Aligned(& fifo_ph_tx, (u32 *) addr, DMABUFFSIZE16);
		XLlFifo_iTxSetLen(& fifo_ph_tx, DMABUFFSIZE16 * 4);
		release_dmabuffer16(addr);
	}
}

void xcz_audio_tx_enable(uint_fast8_t state)
{
	XLlFifo_IntDisable(& fifo_ph_tx, XLLF_INT_ALL_MASK);
	XLlFifo_IntEnable(& fifo_ph_tx, XLLF_INT_TC_MASK);
	arm_hardware_set_handler_realtime(XPAR_FABRIC_AXI_FIFO_PHONES_INTERRUPT_INTR, xcz_fifo_if_tx_inthandler);
	XLlFifo_iWrite_Aligned(& fifo_ph_tx, (u32 *) sinbuf32, DMABUFFSIZE16);	// пнуть, чтобы заработало
	XLlFifo_iTxSetLen(& fifo_ph_tx, DMABUFFSIZE16 * 4);
}

#endif /* CPUSTYLE_XC7Z */
