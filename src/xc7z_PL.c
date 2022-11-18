#include "hardware.h"
#include "board.h"
#include "audio.h"
#include "formats.h"
#include <math.h>

#if (CPUSTYLE_XC7Z) && ! WITHISBOOTLOADER

#include "xc7z_inc.h"

/* Audio register map definitions */
#define AUDIO_REG_I2S_RESET 		 0x00   //Write only
#define AUDIO_REG_I2S_CTRL			 0x04
#define AUDIO_REG_I2S_CLK_CTRL 		 0x08
#define AUDIO_REG_I2S_PERIOD 		 0x18
#define AUDIO_REG_I2S_RX_FIFO 		 0x28   //Read only
#define AUDIO_REG_I2S_TX_FIFO 		 0x2C	//Write only


/* I2S reset mask definitions  */
#define TX_FIFO_RESET_MASK 		 	0x00000002
#define RX_FIFO_RESET_MASK 		 	0x00000004

/* I2S Control mask definitions  */
#define TX_ENABLE_MASK 		 		0x00000001
#define RX_ENABLE_MASK 		 		0x00000002

/* I2S Clock Control mask definitions  */
#define BCLK_DIV_MASK 		 		0x000000FF
#define LRCLK_DIV_MASK 		 		0x00FF0000

#define axi_i2s_adi_WriteReg(BaseAddr, RegOffset, Data)	\
		Xil_Out32((BaseAddr) + (u32)(RegOffset), (u32)(Data))

#if WITHTX
XAxiDma dma_if_tx;
#endif /* WITHTX */

XLlFifo fifo_mic_rx, fifo_if_rx, fifo_phones;
ALIGNX_BEGIN u32 sinbuf32tx[DMABUFFSIZE16TX] ALIGNX_END;

uintptr_t dma_invalidate32rx(uintptr_t addr);
int XLlFifo_iRead_Aligned(XLlFifo *InstancePtr, void *BufPtr, unsigned WordCount);
int XLlFifo_iWrite_Aligned(XLlFifo *InstancePtr, void *BufPtr, unsigned WordCount);

void xcz_fifo_if_tx_inthandler(void);
void xcz_fifo_mic_inthandler(void);
void xcz_fifo_phones_inthandler(void);

static uintptr_t dma_flush32tx(uintptr_t addr)
{
	ASSERT((addr % DCACHEROWSIZE) == 0);
	ASSERT((buffers_dmabuffer32txcachesize() % DCACHEROWSIZE) == 0);
	arm_hardware_flush_invalidate(addr,  buffers_dmabuffer32txcachesize());
	return addr;
}

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
	Xil_Out32(XPAR_TRX_CONTROL2_0_S00_AXI_BASEADDR + 20, val);
}

#if WITHRTS96

void xcz_ah_preinit(void)
{
#if defined (XPAR_AXI_I2S_ADI_0_BASEADDR)
	axi_i2s_adi_WriteReg(XPAR_AXI_I2S_ADI_0_BASEADDR, AUDIO_REG_I2S_CLK_CTRL, (64 / 2 - 1) << 16 | (4 / 2 - 1));
	axi_i2s_adi_WriteReg(XPAR_AXI_I2S_ADI_0_BASEADDR, AUDIO_REG_I2S_PERIOD, DMABUFFSIZE16TX);
	axi_i2s_adi_WriteReg(XPAR_AXI_I2S_ADI_0_BASEADDR, AUDIO_REG_I2S_CTRL, TX_ENABLE_MASK | RX_ENABLE_MASK);
#endif /* defined (XPAR_AXI_I2S_ADI_0_BASEADDR) */

	xcz_rxtx_state(1);
	xcz_rxtx_state(0);
	xcz_rx_iq_shift(45); 	// 45
	xcz_tx_shift(24);		// 24

	uint16_t ss = DMABUFFSIZE16TX;
	uint16_t amp = 16384;
    for(int i = 0; i < ss; ++i)
    {
    	uint16_t ii = cos(i / ss * 2 * M_PI) * amp;
		uint16_t qq = sin(i / ss * 2 * M_PI) * amp;
		sinbuf32tx[i] = (ii << 16) + (qq & 0xFFFF);
    }
}

// ****************** IF RX ******************

static uintptr_t addr32rx;

void xcz_if_rx_init(void)
{
	XLlFifo_Config * config = XLlFfio_LookupConfig(XPAR_AXI_FIFO_IQ_RX_DEVICE_ID);
	int xStatus = XLlFifo_CfgInitialize(& fifo_if_rx, config, config->BaseAddress);
	if(xStatus) {
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
	addr32rx = allocate_dmabuffer32rx();
}

void xcz_fifo_if_rx_inthandler(void)
{
	enum { CNT16TX = DMABUFFSIZE16TX / DMABUFFSTEP16TX };
	enum { CNT32RX = DMABUFFSIZE32RX / DMABUFFSTEP32RX };
	enum { CNT32RTS = DMABUFFSIZE32RTS / DMABUFFSTEP32RTS };
	static unsigned rx_stage = 0;

	u32 ss = XLlFifo_Status(& fifo_if_rx);
	XLlFifo_IntClear(& fifo_if_rx, 0xffffffff);
	u32 occ = XLlFifo_RxOccupancy(& fifo_if_rx);
//	dbg_putchar('*');

	if (ss & XLLF_INT_RFPF_MASK)
	{
		XLlFifo_iRead_Aligned(& fifo_if_rx, (IFADCvalue_t *) addr32rx, DMABUFFSIZE32RX);
		processing_dmabuffer32rts(addr32rx);
		processing_dmabuffer32rx(addr32rx);
		rx_stage += CNT32RX;
	}

	while (rx_stage >= CNT16TX)
	{
		xcz_fifo_phones_inthandler();
		rx_stage -= CNT16TX;
	}
}

void xcz_if_rx_enable(uint_fast8_t state)
{
	XLlFifo_IntDisable(& fifo_if_rx, XLLF_INT_ALL_MASK);
	XLlFifo_IntEnable(& fifo_if_rx, XLLF_INT_RFPF_MASK);
	arm_hardware_set_handler_realtime(XPAR_FABRIC_AXI_FIFO_IQ_RX_INTERRUPT_INTR, xcz_fifo_if_rx_inthandler);
}

// ****************** IF TX ******************

void xcz_if_tx_init(void)
{
#if WITHTX
	XAxiDma_Config * txConfig = XAxiDma_LookupConfig(XPAR_AXI_DMA_IF_TX_DEVICE_ID);
	int Status = XAxiDma_CfgInitialize(& dma_if_tx, txConfig);

	if (Status) {
		xil_printf("Initialization failed %d\r\n", Status);
		ASSERT(0);
	}

	if(XAxiDma_HasSg(& dma_if_tx))
	{
		xil_printf("Device configured as SG mode \r\n");
		ASSERT(0);
	}

	XAxiDma_IntrDisable(& dma_if_tx, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrDisable(& dma_if_tx, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
#endif /* WITHTX */
}

void xcz_dma_transmit_if_tx(UINTPTR buffer, size_t len)
{
#if WITHTX
	int Status = XAxiDma_SimpleTransfer(& dma_if_tx, buffer, len, XAXIDMA_DMA_TO_DEVICE);
	if (Status)
	{
		PRINTF("dma_if_tx transmit error %d\n", Status);
		ASSERT(0);
	}
#endif /* WITHTX */
}

void xcz_dma_if_tx_inthandler(void)
{
#if WITHTX
	XAxiDma_IntrAckIrq(& dma_if_tx, XAXIDMA_IRQ_IOC_MASK, XAXIDMA_DMA_TO_DEVICE);
	uintptr_t addr = dma_flush32tx(getfilled_dmabuffer32tx_main());
	xcz_dma_transmit_if_tx(addr, DMABUFFSIZE32TX * sizeof(IFDACvalue_t));
//	while(XAxiDma_Busy(& dma_if_tx, XAXIDMA_DMA_TO_DEVICE));
	release_dmabuffer32tx(addr);
#endif /* WITHTX */
}

void xcz_if_tx_enable(uint_fast8_t state)
{
#if WITHTX
	XAxiDma_IntrDisable(& dma_if_tx, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrEnable(& dma_if_tx, XAXIDMA_IRQ_IOC_MASK, XAXIDMA_DMA_TO_DEVICE);
	arm_hardware_set_handler_realtime(XPAR_FABRIC_AXI_DMA_IF_TX_MM2S_INTROUT_INTR, xcz_dma_if_tx_inthandler);
	xcz_dma_transmit_if_tx((uintptr_t) sinbuf32tx, DMABUFFSIZE16TX);
#endif /* WITHTX */
}

// ****************** Audio MIC receive ******************

void xcz_audio_rx_init(void)
{
#if WITHTX
	XLlFifo_Config * pConfig = XLlFfio_LookupConfig(XPAR_AXI_FIFO_MIC_DEVICE_ID);
	int xStatus = XLlFifo_CfgInitialize(& fifo_mic_rx, pConfig, pConfig->BaseAddress);
	if(xStatus) {
		PRINTF("fifo_mic_rx CfgInitialize fail %d \n", xStatus);
		ASSERT(0);
	}

	// Check for the Reset value
	u32 Status = XLlFifo_Status(& fifo_mic_rx);
	XLlFifo_IntClear(& fifo_mic_rx, 0xffffffff);
	Status = XLlFifo_Status(& fifo_mic_rx);
	if(Status) {
		PRINTF("fifo_mic_rx reset fail %x \n", Status);
		ASSERT(0);
	}
#endif /* WITHTX */
}

void xcz_fifo_mic_inthandler(void)
{
#if WITHTX
	u32 ss = XLlFifo_Status(& fifo_mic_rx);
	XLlFifo_IntClear(& fifo_mic_rx, 0xffffffff);
	u32 occ = XLlFifo_iRxGetLen(& fifo_mic_rx);
//	dbg_putchar('|');

	if ((ss & XLLF_INT_RFPF_MASK))
//	if (occ > DMABUFFSIZE16)
	{
		uintptr_t addr = allocate_dmabuffer16rx();
		XLlFifo_iRead_Aligned(& fifo_mic_rx, (uint32_t *) addr, DMABUFFSIZE16RX);
		processing_dmabuffer16rx(addr);
	}
#endif /* WITHTX */
}

void xcz_audio_rx_enable(uint_fast8_t state)
{
#if WITHTX
	XLlFifo_IntDisable(& fifo_mic_rx, XLLF_INT_ALL_MASK);
	XLlFifo_IntEnable(& fifo_mic_rx, XLLF_INT_RFPF_MASK);
	arm_hardware_set_handler_realtime(XPAR_FABRIC_AXI_FIFO_MIC_INTERRUPT_INTR, xcz_fifo_mic_inthandler);
#endif /* WITHTX */
}

// ****************** Audio phones transmit ******************

void xcz_audio_tx_init(void)
{
	XLlFifo_Config * pConfig_phtx = XLlFfio_LookupConfig(XPAR_AXI_FIFO_PHONES_DEVICE_ID);
	int xStatus = XLlFifo_CfgInitialize(& fifo_phones, pConfig_phtx, pConfig_phtx->BaseAddress);
	if(XST_SUCCESS != xStatus) {
		PRINTF("fifo_phones CfgInitialize fail %d \n", xStatus);
		ASSERT(0);
	}

	// Check for the Reset value
	u32 Status = XLlFifo_Status(& fifo_phones);
	XLlFifo_IntClear(& fifo_phones, 0xffffffff);
	Status = XLlFifo_Status(& fifo_phones);
	if(Status != 0) {
		PRINTF("fifo_phones reset fail %x \n", Status);
		ASSERT(0);
	}
}

void xcz_fifo_phones_inthandler(void)
{
	u32 ss = XLlFifo_Status(& fifo_phones);
	XLlFifo_IntClear(& fifo_phones, 0xffffffff);
	u32 vac = XLlFifo_TxVacancy(& fifo_phones);

//	if (ss & XLLF_INT_TFPE_MASK)
	{
		const uintptr_t addr = getfilled_dmabuffer16txphones();
		XLlFifo_iWrite_Aligned(& fifo_phones, (u32 *) addr, DMABUFFSIZE16TX);
		XLlFifo_iTxSetLen(& fifo_phones, DMABUFFSIZE16TX * 4);
		release_dmabuffer16tx(addr);
	}
}

void xcz_audio_tx_enable(uint_fast8_t state)
{
//	XLlFifo_IntDisable(& fifo_phones, XLLF_INT_ALL_MASK);
//	XLlFifo_IntEnable(& fifo_phones, XLLF_INT_TFPE_MASK);
//	arm_hardware_set_handler_realtime(XPAR_FABRIC_AXI_FIFO_PHONES_INTERRUPT_INTR, xcz_fifo_phones_inthandler);
//	XLlFifo_iWrite_Aligned(& fifo_phones, (u32 *) sinbuf32, DMABUFFSIZE16);	// пнуть, чтобы заработало
//	XLlFifo_iTxSetLen(& fifo_phones, DMABUFFSIZE16 * 4 / fifo_divider);
}

#endif /* WITHRTS96 */

#else // заглушки для бутлоадера

void xcz_rxtx_state(uint8_t tx)
{

}

#endif /* CPUSTYLE_XC7Z */
