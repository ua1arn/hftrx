/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if WITHETHHW && CPUSTYLE_STM32MP1 && WITHLWIP

#include "gpio.h"

//#include "stm32mp1xx_hal_eth.h"

//#ifndef HAL_ETH_MODULE_ENABLED
//	#error Check HAL_ETH_MODULE_ENABLED in src/hal/stm32mp1xx_hal_conf.h
//#endif

#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "lwip/ip.h"

#include <string.h>

// ETH clocks init
static void ethhw_initialize(void)
{
	HARDWARE_ETH_INITIALIZE();

	RCC->ETHCKSELR =
			0x00 * (UINT32_C(1) << 4) |	// PTP divider: 0x0: bypass (default after reset)
			//0x0 * (UINT32_C(1) << 0) |	// 0x0: pll4_p_ck clock selected as kernel peripheral clock (default after reset)
			0x1 * (UINT32_C(1) << 0) |	// 0x1: pll3_q_ck clock selected as kernel peripheral clock
			0;

	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_ETHMACEN_Msk;
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_ETHTXEN_Msk;
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_ETHRXEN_Msk;
	(void) RCC->MP_AHB6ENSETR;

	RCC->AHB6RSTSETR = RCC_AHB6RSTSETR_ETHMACRST_Msk;	// assert reset
	(void) RCC->AHB6RSTSETR;
	RCC->AHB6RSTCLRR = RCC_AHB6RSTCLRR_ETHMACRST_Msk;	// de-assert reset
	(void) RCC->AHB6RSTCLRR;

}

static void ethhw_deinitialize(void)
{
	RCC->AHB6RSTSETR = RCC_AHB6RSTSETR_ETHMACRST_Msk;	// assert reset
	(void) RCC->AHB6RSTSETR;

	RCC->MP_AHB6ENCLRR = RCC_MP_AHB6ENCLRR_ETHTXEN_Msk;
	(void) RCC->MP_AHB6ENCLRR;
	RCC->MP_AHB6ENCLRR = RCC_MP_AHB6ENCLRR_ETHRXEN_Msk;
	(void) RCC->MP_AHB6ENCLRR;
	RCC->MP_AHB6ENCLRR = RCC_MP_AHB6ENCLRR_ETHMACEN_Msk;
	(void) RCC->MP_AHB6ENCLRR;
}

#define ETHHW_BUFFSIZE 4096

void ethhw_rxfilldesc(volatile uint32_t * desc, uint8_t * buff1, uint8_t * buff2, unsigned buffsize)
{
	desc [0] = (uintptr_t) buff1;
	desc [1] = (uintptr_t) buff2;
	desc [2] =
		1 * (UINT32_C(1) << 31) | // IOC
		0 * (UINT32_C(1) << 30) | // TTSE
		(0xFFFF & buffsize) * (UINT32_C(1) << 16) | // B2L = buffer 2 length
		(0xFFFF & buffsize) * (UINT32_C(1) << 0) | // B1L = buffer 1 length
		0;

	desc [3] =
		1 * (UINT32_C(1) << 31) | // Own bit
		1 * (UINT32_C(1) << 29) | // First Descriptor
		1 * (UINT32_C(1) << 28) | // Last Descriptor
		0;
//	memset(desc, 0xff, 16);
}

void ethhw_txfilldesc(volatile uint32_t * desc, uint8_t * buff1, uint8_t * buff2, unsigned buffsize)
{
	desc [0] = (uintptr_t) buff1;
	desc [1] = (uintptr_t) buff2;
	desc [2] =
		1 * (UINT32_C(1) << 31) | // IOC
		0 * (UINT32_C(1) << 30) | // TTSE
		(0xFFFF & buffsize) * (UINT32_C(1) << 16) | // B2L = buffer 2 length
		(0xFFFF & buffsize) * (UINT32_C(1) << 0) | // B1L = buffer 1 length
		0;

	desc [3] =
		1 * (UINT32_C(1) << 31) | // Own bit
		1 * (UINT32_C(1) << 29) | // First Descriptor
		1 * (UINT32_C(1) << 28) | // Last Descriptor
		0;
//	memset(desc, 0xff, 16);
}

int nic_can_send(void)
{
	return 0;
}

void nic_send(const uint8_t * data, int size)
{
//	static __ALIGN_BEGIN RAMNC uint8_t buff [4096] __ALIGN_END; /* Ethernet Rx DMA Descriptors */
//	uint32_t i=0;
//
//
//	ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT];
//	memcpy(buff, data, size);
//	memset(Txbuffer, 0 , ETH_TX_DESC_CNT*sizeof(ETH_BufferTypeDef));
//
//	Txbuffer[i].buffer = buff;
//	Txbuffer[i].len = size;
//
//	Txbuffer[i].next = NULL;
//
//	TxConfig.Length =  size;
//	TxConfig.TxBuffer = Txbuffer;
//
//	HAL_ETH_Transmit(&heth, &TxConfig, ETH_DMA_TRANSMIT_TIMEOUT);

}

static RAMNC __attribute__((aligned(32))) uint8_t  dmac0tx_buff [2][ETHHW_BUFFSIZE];
static RAMNC __attribute__((aligned(32))) uint8_t  dmac0rx_buff [2][ETHHW_BUFFSIZE];
static RAMNC __attribute__((aligned(32))) uint8_t  dmac1tx_buff [2][ETHHW_BUFFSIZE];

static RAMNC __attribute__((aligned(32))) volatile uint32_t  dmac0tx_desc [64];
static RAMNC __attribute__((aligned(32))) volatile uint32_t  dmac0rx_desc [64];
static RAMNC __attribute__((aligned(32))) volatile uint32_t  dmac1tx_desc [64];

static void ETH1_Handler(void)
{
	TP();
	{
		const uint_fast32_t macisr = ETH->MACISR;
		if (macisr & ETH_MACISR_RGSMIIIS_Msk)
			PRINTF("MACPHYCSR=%08X, macisr=%08X\n", (unsigned) ETH->MACPHYCSR, (unsigned) macisr);
	}
}

void nic_initialize(void)
{
	const uint8_t hwaddr [6] = { HWADDR };
	// Ethernet controller tests
	ethhw_initialize();

	memset(dmac0rx_buff, 0xFF, sizeof dmac0rx_buff);
	/* Configure the CSR Clock Range */
	//ETH->MACMDIOAR = ETH_MACMDIOAR_CR_DIV102;

	// 63.9.1 DMA initialization
	ETH->DMAMR |= ETH_DMAMR_SWR_Msk;
	while ((ETH->DMAMR & ETH_DMAMR_SWR_Msk) != 0)	// waiting for reset complete
		;
	ETH->DMASBMR = 0x01010000;

	// 63.9.2 MTL initialization
	(void) ETH->MTLOMR;

	// 63.9.3 MAC initialization

	ETH->MACCR = 0;
	ETH->MACCR |=
			0 * ETH_MACCR_PS_Msk | 	// Select 1000 Mbps operation
			0 * ETH_MACCR_FES_Msk |	// This bit selects the speed in the 10/100 Mbps mode -  1: 100 Mbps
			1 * ETH_MACCR_DM_Msk |	// Select duplex operation
			0;

	ETH->MACA0HR = USBD_peek_u16(hwaddr + 4);	// upper 16 bits of the first 6-byte MAC address
	ETH->MACA0LR = USBD_peek_u32(hwaddr + 0);	// lower 32 bits of the 6-byte first MAC address

	// Packet filter modes
	ETH->MACPFR = 0;
	//ETH->MACPFR |= ETH_MACPFR_PR_Msk;
	ETH->MACPFR |= ETH_MACPFR_RA_Msk;	// Receive All

	PRINTF("ETH->MACCR=%08X\n", (unsigned) ETH->MACCR);
	PRINTF("ETH->DMASBMR=%08X\n", (unsigned) ETH->DMASBMR);

	ethhw_txfilldesc(dmac0tx_desc, dmac0tx_buff [0], dmac0tx_buff [1], ETHHW_BUFFSIZE);
	ethhw_rxfilldesc(dmac0rx_desc, dmac0rx_buff [0], dmac0rx_buff [1], ETHHW_BUFFSIZE);
	ethhw_txfilldesc(dmac1tx_desc, dmac1tx_buff [0], dmac1tx_buff [1], ETHHW_BUFFSIZE);

	dcache_clean_invalidate((uintptr_t) dmac0tx_desc, sizeof dmac0tx_desc);
	dcache_clean_invalidate((uintptr_t) dmac0rx_desc, sizeof dmac0rx_desc);
	dcache_clean_invalidate((uintptr_t) dmac1tx_desc, sizeof dmac1tx_desc);

	dcache_clean_invalidate((uintptr_t) dmac0tx_buff, sizeof dmac0tx_buff);
	dcache_clean_invalidate((uintptr_t) dmac0rx_buff, sizeof dmac0rx_buff);
	dcache_clean_invalidate((uintptr_t) dmac1tx_buff, sizeof dmac1tx_buff);

	if (1)
	{
		// CH0: RX
		ETH->DMAC0RXCR =
			((ETHHW_BUFFSIZE - 1) << ETH_DMAC0RXCR_RBSZ_Pos) |
			ETH_DMAC0RXCR_RPF_Msk |
			0;
		ETH->DMAC0RXDLAR = (uintptr_t) dmac0rx_desc;	// Channel 0 Rx descriptor list address register
		ETH->DMAC0RXDTPR = (uintptr_t) dmac0rx_desc;	// Channel 0 Rx descriptor tail pointer register
		ETH->DMAC0RXRLR = // Channel 0 Rx descriptor ring length register
			(1 - 1) * (UINT32_C(1) << ETH_DMAC0RXRLR_RDRL_Pos) |	// RDRL
			0 * (UINT32_C(17) << 0) |	// ARBS
			0;
		ETH->DMAC0RXCR |= ETH_DMAC0RXCR_SR_Msk;	// Channel 0 receive control register

		// CH0: TX
		//ETH->DMAC0TXCR = (ETHHW_BUFFSIZE) << ETH_DMAC0TXCR_TBSZ_Pos;
		ETH->DMAC0TXDLAR = (uintptr_t) dmac0tx_desc;	// Channel 0 Tx descriptor list address register
		ETH->DMAC0TXDTPR = (uintptr_t) dmac0tx_desc;	// Channel 0 Tx descriptor tail pointer register
		ETH->DMAC0TXRLR =
			(1 - 1) * (UINT32_C(1) << ETH_DMAC0TXRLR_TDRL_Pos) |	// Channel 0 Tx descriptor ring length register
			0;
		ETH->DMAC0TXCR |= ETH_DMAC0TXCR_ST_Msk;	// Channel 0 transmit control register
	}

	if (0)
	{
		// CH1: TX
		//ETH->DMAC1TXCR = (ETHHW_BUFFSIZE) << ETH_DMAC1TXCR_TBSZ_Pos;
		ETH->DMAC1TXDLAR = (uintptr_t) dmac1tx_desc;	// Channel 1 Tx descriptor list address register
		ETH->DMAC1TXDTPR = (uintptr_t) dmac1tx_desc;	// Channel 1 Tx descriptor tail pointer register
		ETH->DMAC1TXRLR =
			1 * (UINT32_C(1) << ETH_DMAC1TXRLR_TDRL_Pos) |	// Channel 1 Tx descriptor ring length register
			0;
		ETH->DMAC1TXCR |= ETH_DMAC1TXCR_ST_Msk;	// Channel 1 transmit control register
	}

	ETH->MACCR |= ETH_MACCR_TE_Msk;
	ETH->MACCR |= ETH_MACCR_RE_Msk;

	ETH->MACIER =
			ETH_MACIER_RXSTSIE_Msk |
			ETH_MACIER_TXSTSIE_Msk |
			0;

	ETH->DMAC0IER =
		ETH_DMAC0IER_TIE_Msk |
		ETH_DMAC0IER_RIE_Msk |
		0;

	ETH->DMAC1IER =
			ETH_DMAC1IER_TIE_Msk |
		0;

	ETH->MACIER = 0;
	ETH->MACIER |= ETH_MACIER_PHYIE_Msk;
	ETH->MACIER |= ETH_MACIER_RGSMIIIE_Msk;

	/* Enable the MAC transmission */
	ETH->MACCR |= ETH_MACCR_TE;

	/* Enable the MAC reception */
	ETH->MACCR |= ETH_MACCR_RE;

//	/* Set the Flush Transmit FIFO bit */
//	ETH->MTLTQOMR |= ETH_MTLTQOMR_FTQ;
//
//	/* Enable the DMA transmission */
//	ETH->DMAC0TXCR |= ETH_DMACTCR_ST;
//
//	/* Enable the DMA reception */
//	ETH->DMAC0RCR |= ETH_DMACRCR_SR;
//
//	/* Clear Tx and Rx process stopped flags */
//	ETH->DMAC0SR |= (ETH_DMACSR_TPS | ETH_DMACSR_RPS);

	arm_hardware_set_handler_system(ETH1_IRQn, ETH1_Handler);

//	for (;;)
//	{
//		PRINTF("DMADSR=%08X, MACRXTXSR=%08X, DMAC0RXCR=%08X, DMAC0SR=%08X ", (unsigned) ETH->DMADSR, (unsigned) ETH->MACRXTXSR, (unsigned) ETH->DMAC0RXCR, (unsigned) ETH->DMAC0SR);
//		printhex(0, dmac0rx_buff, 16);
//		//printhex(0, (const void *) dmac0rx_desc, 16);
//		local_delay_ms(250);
//	}
}
#endif /* WITHETHHW */

