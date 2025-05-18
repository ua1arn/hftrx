/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if WITHLWIP && WITHETHHW && (CPUSTYLE_T507 || CPUSTYLE_H616)

#include "gpio.h"
#include "formats.h"

#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "lwip/ip.h"

#include <string.h>

static nic_rxproc_t on_packet;

#define ETH_HEADER_SIZE                 14
#define ETH_MIN_PACKET_SIZE             60
#define ETH_MAX_PACKET_SIZE             (ETH_HEADER_SIZE + NIC_MTU)
//#define EMAC_HEADER_SIZE               (sizeof (emac_data_packet_t))
//#define EMAC_RX_BUFFER_SIZE            (EMAC_HEADER_SIZE + ETH_MAX_PACKET_SIZE)

enum { EMAC_FRAMESZ = 2048 - 4 };
static RAMNC uint8_t rxbuff [EMAC_FRAMESZ];
static RAMNC __ALIGNED(4) uint32_t emac_rxdesc [1] [4];
static RAMNC uint8_t txbuff [EMAC_FRAMESZ];
static RAMNC __ALIGNED(4) uint32_t emac_txdesc [1] [4];

int nic_can_send(void)
{
    int i = 0;
	return ((emac_txdesc [i][0] & (UINT32_C(1) << 31)) == 0);
}

void nic_send(const uint8_t * data, int isize)
{
    int i = 0;
	unsigned size = ulmin32(sizeof txbuff, isize);

	memcpy(txbuff, data, size);

	emac_txdesc [i][0] =	// status
		1 * (UINT32_C(1) << 31) |	// TX_DESC_CTL
		0;
	// CRC_CTL=0 и CHECKSUM_CTL=3: просто передаёт заказанный в дескрипторе размер
	// CRC_CTL=0 и CHECKSUM_CTL=2: просто передаёт заказанный в дескрипторе размер
	// CRC_CTL=0 и CHECKSUM_CTL=1: просто передаёт заказанный в дескрипторе размер
	// CRC_CTL=0 и CHECKSUM_CTL=0: просто передаёт заказанный в дескрипторе размер
	// CRC_CTL=1 и CHECKSUM_CTL=3: передаёт на 4 меньше
	// CRC_CTL=1 и CHECKSUM_CTL=2: передаёт на 4 меньше
	// CRC_CTL=1 и CHECKSUM_CTL=1: передаёт на 4 меньше
	// CRC_CTL=1 и CHECKSUM_CTL=0: передаёт на 4 меньше
	emac_txdesc [i][1] =	// ctl
		1 * (UINT32_C(1) << 31) |	// TX_INT_CTL
		1 * (UINT32_C(1) << 30) |	// LAST_DESC
		1 * (UINT32_C(1) << 29) |	// FIR_DESC
		//0x03 * (UINT32_C(1) << 27) |	// CHECKSUM_CTL
		//1 * (UINT32_C(1) << 26) |	// CRC_CTL When it is set, the CRC field is not transmitted.
		1 * (UINT32_C(1) << 24) |	// magic. Without it, packets never be sent on H3 SoC
		(size) * (UINT32_C(1) << 0) |	// 10:0 BUF_SIZE
		0;
	emac_txdesc [i][2] = (uintptr_t) txbuff;	// BUF_ADDR
	emac_txdesc [i][3] = (uintptr_t) emac_txdesc [i];	// NEXT_DESC_ADDR

	dcache_clean((uintptr_t) txbuff, sizeof txbuff);

	dcache_clean((uintptr_t) emac_txdesc, sizeof emac_txdesc);
	HARDWARE_EMAC_PTR->EMAC_TX_DMA_DESC_LIST = (uintptr_t) & emac_txdesc [i];

	HARDWARE_EMAC_PTR->EMAC_TX_CTL0 =
		1 * (UINT32_C(1) << 31) |	// TX_EN
		//1 * (UINT32_C(1) << 30) |	// TX_FRM_LEN_CTL
		0;

	//HARDWARE_EMAC_PTR->EMAC_TX_CTL1 &= ~ (UINT32_C(1) << 30);	// DMA EN
	HARDWARE_EMAC_PTR->EMAC_TX_CTL1 |= (UINT32_C(1) << 1);	// TX_MD 1: TX start after TX DMA FIFO located a full frame
	HARDWARE_EMAC_PTR->EMAC_TX_CTL1 |= (UINT32_C(1) << 30);	// DMA EN
	HARDWARE_EMAC_PTR->EMAC_TX_CTL1 |= (UINT32_C(1) << 31);	// TX_DMA_START (auto-clear)
	while (HARDWARE_EMAC_PTR->EMAC_TX_CTL1 & (UINT32_C(1) << 31))
		;
}

static void EMAC_Handler(void)
{
	const portholder_t sta = HARDWARE_EMAC_PTR->EMAC_INT_STA;
	if (sta & ((UINT32_C(1) << 8)))	// RX_P
	{
		HARDWARE_EMAC_PTR->EMAC_INT_STA = (UINT32_C(1) << 8);	// RX_P
		if (1) //((HARDWARE_EMAC_PTR->EMAC_RX_DMA_STA & 0x07) == 0x03)
		{
			if (on_packet)
				on_packet(rxbuff, sizeof rxbuff);

			unsigned i = 0;
			emac_rxdesc [i][0] =
				1 * (UINT32_C(1) << 31) |	// RX_DESC_CTL
		//				1 * (UINT32_C(1) << 9) |	// FIR_DESC
		//				1 * (UINT32_C(1) << 8) |	// LAST_DESC
				0;
			dcache_clean_invalidate((uintptr_t) rxbuff, sizeof rxbuff);
			dcache_clean((uintptr_t) emac_rxdesc, sizeof emac_rxdesc);
		}
		else
		{
			TP();
			PRINTF("EMAC_RX_DMA_STA=%08X\n", (unsigned) HARDWARE_EMAC_PTR->EMAC_RX_DMA_STA);
		}
	}
}

static void emac_hw_initialize(void)
{
	const unsigned ix = HARDWARE_EMAC_IX;	// 0: EMAC0, 1: EMAC1
	CCU->EMAC_BGR_REG |= (UINT32_C(1) << ((0 + ix)));	// Gating Clock for EMACx
	CCU->EMAC_BGR_REG &= ~ (UINT32_C(1) << ((16 + ix)));	// EMACx Reset
	CCU->EMAC_BGR_REG |= (UINT32_C(1) << ((16 + ix)));	// EMACx Reset
	//PRINTF("CCU->EMAC_BGR_REG=%08X (@%p)\n", (unsigned) CCU->EMAC_BGR_REG, & CCU->EMAC_BGR_REG);

	HARDWARE_ETH_INITIALIZE();	// Должно быть тут - снять ресет с PHY до инициализации
	{
		// The working clock of EMAC is from AHB3.

		HARDWARE_EMAC_EPHY_CLK_REG = 0x00051c06; // 0x00051c06 0x00053c01
		//PRINTF("EMAC_BASIC_CTL1=%08X\n", (unsigned) HARDWARE_EMAC_PTR->EMAC_BASIC_CTL1);
		//printhex32((uintptr_t) HARDWARE_EMAC_PTR, HARDWARE_EMAC_PTR, 256);
		// Сигнал phyrstb тут уже должен бьыть неактивен
		HARDWARE_EMAC_PTR->EMAC_BASIC_CTL1 |= (UINT32_C(1) << 0);	// Soft reset
		while ((HARDWARE_EMAC_PTR->EMAC_BASIC_CTL1 & (UINT32_C(1) << 0)) != 0)
			;

		HARDWARE_EMAC_PTR->EMAC_BASIC_CTL0 =
			//0x03 * (UINT32_C(1) << 2) |	// SPEED - 00: 1000 Mbit/s, 10: 10 Mbit/s, 11: 100 Mbit/s
			0x01 * (UINT32_C(1) << 0) | // DUPLEX - 1: Full-duplex
			0;
		HARDWARE_EMAC_PTR->EMAC_BASIC_CTL1 =
			0x08 * (UINT32_C(1) << 24) |	// BURST_LEN - The burst length of RX and TX DMA transfer.
			0;

//			PRINTF("EMAC_BASIC_CTL0=%08X\n", (unsigned) HARDWARE_EMAC_PTR->EMAC_BASIC_CTL0);
//			PRINTF("EMAC_BASIC_CTL1=%08X\n", (unsigned) HARDWARE_EMAC_PTR->EMAC_BASIC_CTL1);
//			PRINTF("EMAC_RGMII_STA=%08X\n", (unsigned) HARDWARE_EMAC_PTR->EMAC_RGMII_STA);

		const uint8_t hwaddr [6] = { HWADDR };
		// ether 1A:0C:74:06:AF:64
//		HARDWARE_EMAC_PTR->EMAC_ADDR [0].HIGH = 0x000064AF;
//		HARDWARE_EMAC_PTR->EMAC_ADDR [0].LOW = 0x06740C1A;
		HARDWARE_EMAC_PTR->EMAC_ADDR [0].HIGH = USBD_peek_u16(hwaddr + 4);	// upper 16 bits of the first 6-byte MAC address
		HARDWARE_EMAC_PTR->EMAC_ADDR [0].LOW = USBD_peek_u32(hwaddr + 0);	// lower 32 bits of the 6-byte first MAC address
	}
	// RX init
	{
		unsigned len = EMAC_FRAMESZ;
		unsigned i = 0;
		emac_rxdesc [i][0] =
			1 * (UINT32_C(1) << 31) |	// RX_DESC_CTL
//				1 * (UINT32_C(1) << 9) |	// FIR_DESC
//				1 * (UINT32_C(1) << 8) |	// LAST_DESC
			0;
		emac_rxdesc [i][1] =
				len * (UINT32_C(1) << 0) |	// 10:0 BUF_SIZE
			0;
		emac_rxdesc [i][2] = (uintptr_t) rxbuff;	// BUF_ADDR
		emac_rxdesc [i][3] = (uintptr_t) emac_rxdesc [0];	// NEXT_DESC_ADDR
		//printhex32((uintptr_t) emac_rxdesc, emac_rxdesc, sizeof emac_rxdesc);

		arm_hardware_set_handler_system(HARDWARE_EMAC_IRQ, EMAC_Handler);

		HARDWARE_EMAC_PTR->EMAC_RX_FRM_FLT =
//					1 * (UINT32_C(1) << 31) |	// DIS_ADDR_FILTER
				1 * (UINT32_C(1) << 0) |	// RX_ALL
				0;

		HARDWARE_EMAC_PTR->EMAC_RX_DMA_DESC_LIST = (uintptr_t) emac_rxdesc;
		//HARDWARE_EMAC_PTR->EMAC_RX_CTL0 = 0xb8000000;
		HARDWARE_EMAC_PTR->EMAC_RX_CTL0 =
			1 * (UINT32_C(1) << 31) |	// RX_EN
			//1 * (UINT32_C(1) << 29) |	// JUMBO_FRM_EN
			//1 * (UINT32_C(1) << 28) |	// STRIP_FCS
			1 * (UINT32_C(1) << 27) |	// CHECK_CRC 1: Calculate CRC and check the IPv4 Header Checksum.
			0;
		HARDWARE_EMAC_PTR->EMAC_RX_CTL1 =
				1 * (UINT32_C(1) << 1) |	// 1: RX start read after RX DMA FIFO located a full frame
				1 * (UINT32_C(1) << 30) |	// /RX_DMA_EN
				0;

		HARDWARE_EMAC_PTR->EMAC_INT_EN |= (UINT32_C(1) << 8); // RX_INT_EN

		HARDWARE_EMAC_PTR->EMAC_RX_CTL1 |= (UINT32_C(1) << 31);	// RX_DMA_START (auto-clear)
	}
	// TX init
	{
		unsigned len = EMAC_FRAMESZ;
		unsigned i = 0;
		emac_txdesc [i][0] =
			//1 * (UINT32_C(1) << 31) |	// TX_DESC_CTL
//				1 * (UINT32_C(1) << 9) |	// FIR_DESC
//				1 * (UINT32_C(1) << 8) |	// LAST_DESC
			0;
		emac_txdesc [i][1] =
				len * (UINT32_C(1) << 0) |	// 10:0 BUF_SIZE
			0;
		emac_txdesc [i][2] = (uintptr_t) txbuff;	// BUF_ADDR
		emac_txdesc [i][3] = (uintptr_t) emac_txdesc [0];	// NEXT_DESC_ADDR
		//printhex32((uintptr_t) emac_rxdesc, emac_rxdesc, sizeof emac_rxdesc);

		//arm_hardware_set_handler_system(HARDWARE_EMAC_IRQ, EMAC_Handler);

		//HARDWARE_EMAC_PTR->EMAC_RX_CTL0 = 0xb8000000;

		//HARDWARE_EMAC_PTR->EMAC_INT_EN |= (UINT32_C(1) << 0); // TX_INT_EN

		//HARDWARE_EMAC_PTR->EMAC_TX_CTL1 |= (UINT32_C(1) << 31);	// TX_DMA_START (auto-clear)
	}
}

void nic_initialize(void)
{
	//PRINTF("nic_initialize start\n");
	emac_hw_initialize();
	on_packet = nic_on_packet;
	//PRINTF("nic_initialize done\n");
}

#endif /* WITHLWIP && WITHETHHW && (CPUSTYLE_T507 || CPUSTYLE_H616) */
