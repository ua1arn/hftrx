/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if WITHLWIP && WITHETHHW && (CPUSTYLE_T507 || CPUSTYLE_T113 || CPUSTYLE_F133)


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

/////////////////
/// AI-generated
///

#include "lwip/netif.h"
#include "lwip/timeouts.h"

/* Realtek RTL8211F PHY Registers */
#define RTL8211F_PHYSR          26	// PHYSR (PHY Specific Status Register, Page 0xa43, Address 0x1A)

/* RTL8211F PHYSR (Register 26) Bit Definitions */
#define PHYSR_LINK_STATUS       (1 << 2)
#define PHYSR_DUPLEX_STATUS     (1 << 3)
#define PHYSR_SPEED_MASK        (3 << 4)
#define PHYSR_SPEED_10          (0 << 4)
#define PHYSR_SPEED_100         (1 << 4)
#define PHYSR_SPEED_1000        (2 << 4)

/* Allwinner EMAC Register Offsets and Base Addresses */
//#define SYS_CTRL_BASE           0x01C00000
//#define EMAC_CLK_REG            (*(volatile uint32_t *)(SYS_CTRL_BASE + 0x30))
//
//#define EMAC_BASE               0x01C30000
//#define EMAC_BASIC_CTL_0        (*(volatile uint32_t *)(EMAC_BASE + 0x00))
//#define EMAC_MII_CMD            (*(volatile uint32_t *)(EMAC_BASE + 0x48))
//#define EMAC_MII_DATA           (*(volatile uint32_t *)(EMAC_BASE + 0x4C))

/* Allwinner EMAC Bit Definitions */
#define EMAC_MII_BUSY           (1 << 0)
#define EMAC_MII_WRITE          (1 << 1)
#define EMAC_MII_CLK_DIV_64     (2 << 2)

#define EMAC_CTL_SPEED_1000     (0 << 2)
#define EMAC_CTL_SPEED_100      (3 << 2)
#define EMAC_CTL_SPEED_10       (2 << 2)
#define EMAC_CTL_DUPLEX_FULL    (1 << 0)

/* Internal driver states */
static uint8_t last_link_state = 0xFF;

/**
 * @brief Reads a 16-bit register from the PHY via MDIO interface.
 */
static uint16_t emac_mdio_read(uint8_t phy_addr, uint8_t reg_addr) {
    /* Wait until MDIO interface is idle */
    while (HARDWARE_EMAC_PTR->EMAC_MII_CMD & EMAC_MII_BUSY);

    /* Form read command with safe clock divider */
    uint32_t cmd = ((phy_addr & 0x1F) << 16) |
                   ((reg_addr & 0x1F) << 4)  |
                   EMAC_MII_CLK_DIV_64       |
                   EMAC_MII_BUSY;

    HARDWARE_EMAC_PTR->EMAC_MII_CMD = cmd;

    /* Wait for command execution completion */
    while (HARDWARE_EMAC_PTR->EMAC_MII_CMD & EMAC_MII_BUSY);

    return (uint16_t)(HARDWARE_EMAC_PTR->EMAC_MII_DATA & 0xFFFF);
}

/**
 * @brief Updates Allwinner EMAC internal MAC controller speed and duplex settings.
 */
static void allwinner_emac_update_mac_speed(uint32_t speed, uint32_t is_full_duplex) {
    uint32_t ctl_val = HARDWARE_EMAC_PTR->EMAC_BASIC_CTL0;
    PRINTF("allwinner_emac_update_mac_speed: speed=%u, is_full_duplex=%u\n", speed, is_full_duplex);

    /* Clear existing speed (bits 3:2) and duplex (bit 0) configurations */
    ctl_val &= ~((3 << 2) | (1 << 0));

    /* Apply full or half duplex state */
    if (is_full_duplex) {
        ctl_val |= EMAC_CTL_DUPLEX_FULL;
    }

    /* Apply internal MAC speed bits (SYS_CTRL / CCU clocks are bypassed) */
    if (speed == 1000) {
        ctl_val |= EMAC_CTL_SPEED_1000;
    }
    else if (speed == 100) {
        ctl_val |= EMAC_CTL_SPEED_100;
    }
    else if (speed == 10) {
        ctl_val |= EMAC_CTL_SPEED_10;
    }

    /* Write updated values back to the EMAC configuration register */
    HARDWARE_EMAC_PTR->EMAC_BASIC_CTL0 = ctl_val;
}

/**
 * @brief Periodically monitors RTL8211F Auto-Negotiation and updates lwIP.
 */
static void check_ethernet_link_status(struct netif *netif) {
    /* Read vendor-specific register 26 */
    uint16_t physr = emac_mdio_read(RTL8211F_PHY_ADDR, RTL8211F_PHYSR);

    /* Parse actual Link Status from Bit 2 */
    uint8_t link_up = (physr & PHYSR_LINK_STATUS) ? 1 : 0;

    if (link_up != last_link_state) {
        if (link_up) {
            /* Link established. Extract speed (Bits 5:4) and duplex (Bit 3) */
            uint8_t speed_bits = physr & PHYSR_SPEED_MASK;
            uint8_t duplex_bit = (physr & PHYSR_DUPLEX_STATUS) ? 1 : 0;

            uint32_t speed = 100;
            if (speed_bits == PHYSR_SPEED_10)       speed = 10;
            else if (speed_bits == PHYSR_SPEED_100)  speed = 100;
            else if (speed_bits == PHYSR_SPEED_1000) speed = 1000;

            /* Apply hardware speed adjustments to Allwinner EMAC and CCU */
            allwinner_emac_update_mac_speed(speed, duplex_bit);

            allwinner_emac_update_mac_speed(speed, duplex_bit);

			/* Propagate physical state to lwIP */
			netif_set_link_up(netif);

			/* Start or resume DHCP negotiation when cable is plugged in */
			//dhcp_start(netif);

        } else {
            /* Link disconnected */
            netif_set_link_down(netif);

            /* FORCED RESET: Inform DHCP core that the lease is no longer valid */
            //dhcp_release_and_stop(netif);

            /* Optional: Explicitly zero out IP addresses to force strict state */
            netif_set_ipaddr(netif, IP4_ADDR_ANY4);
            netif_set_netmask(netif, IP4_ADDR_ANY4);
            netif_set_gw(netif, IP4_ADDR_ANY4);
       }
        last_link_state = link_up;
    }
}


static void allwinner_emac_hw_initialize(void)
{
	const unsigned ix = HARDWARE_EMAC_IX;	// 0: EMAC0, 1: EMAC1
	CCU->EMAC_BGR_REG |= (UINT32_C(1) << ((0 + ix)));	// Gating Clock for EMACx
	CCU->EMAC_BGR_REG &= ~ (UINT32_C(1) << ((16 + ix)));	// EMACx Reset
	CCU->EMAC_BGR_REG |= (UINT32_C(1) << ((16 + ix)));	// EMACx Reset
	//PRINTF("CCU->EMAC_BGR_REG=%08X (@%p)\n", (unsigned) CCU->EMAC_BGR_REG, & CCU->EMAC_BGR_REG);

	//CCU->EMAC_25M_CLK_REG |= (UINT32_C(1) << 31) | (UINT32_C(1) << 30);	// moved to HARDWARE_ETH_INITIALIZE

	HARDWARE_ETH_INITIALIZE();	// Должно быть тут - снять ресет с PHY до инициализации
	{
		// The working clock of EMAC is from AHB3.
#if (CPUSTYLE_T507)
		HARDWARE_EMAC_EPHY_CLK_REG =
			0x00051c06 | // 0x00051c06 0x00053c01
			0;
		//PRINTF("EMAC_BASIC_CTL1=%08X\n", (unsigned) HARDWARE_EMAC_PTR->EMAC_BASIC_CTL1);
		//printhex32((uintptr_t) HARDWARE_EMAC_PTR, HARDWARE_EMAC_PTR, 256);
#elif (CPUSTYLE_T113 || CPUSTYLE_F133)
		HARDWARE_EMAC_EPHY_CLK_REG =
			1 * (UINT32_C(1) << 13) |
			0;
#endif
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

#if 0
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include <string.h>

/* Realtek RTL8211F PHY Address Configuration */
#define PHY_ADDR                1
#define RTL8211F_PHYSR          26

/* RTL8211F PHYSR (Register 26) Bit Definitions */
#define PHYSR_LINK_STATUS       (1 << 2)
#define PHYSR_DUPLEX_STATUS     (1 << 3)
#define PHYSR_SPEED_MASK        (3 << 4)
#define PHYSR_SPEED_10          (0 << 4)
#define PHYSR_SPEED_100         (1 << 4)
#define PHYSR_SPEED_1000        (2 << 4)

/* Allwinner EMAC Management Interface (MDIO) Bit Definitions */
#define EMAC_MII_BUSY           (1 << 0)
#define EMAC_MII_WRITE          (1 << 1)
#define EMAC_MII_CLK_DIV_64     (2 << 2)  /* MDC safe clock divider based on AHB */

/* Allwinner EMAC Basic Control 0 Bit Definitions */
#define EMAC_CTL_SPEED_1000     (0 << 2)
#define EMAC_CTL_SPEED_100      (3 << 2)
#define EMAC_CTL_SPEED_10       (2 << 2)
#define EMAC_CTL_DUPLEX_FULL    (1 << 0)

/* EMAC DMA RX Ring Descriptor Configuration */
#define EMAC_RX_BUFFERS_COUNT   16
#define EMAC_MAX_PACKET_SIZE    1536

/* Synopsys DesignWare / Allwinner EMAC DMA Descriptor Layout */
struct emac_dma_desc {
    volatile uint32_t status;
    volatile uint32_t control;
    volatile uint32_t buf_addr;
    volatile uint32_t next_desc;
};

#define DESC_OWN_BY_DMA         (1UL << 31)
#define DESC_RX_LAST            (1UL << 8)
#define DESC_RX_FIRST           (1UL << 9)
#define DESC_RX_ERRORS_MASK     (1UL << 15)
#define DESC_RX_FL_MASK         0x3FFF0000
#define DESC_RX_FL_SHIFT        16
#define DESC_RX_BUF_SIZE_MASK   0x7FF
#define DESC_RX_CHAINED         (1UL << 14)

/* Driver private variables tracking state and rings */
static struct emac_dma_desc rx_desc_ring[EMAC_RX_BUFFERS_COUNT] __attribute__((aligned(4)));
static uint8_t rx_buffer_pool[EMAC_RX_BUFFERS_COUNT][EMAC_MAX_PACKET_SIZE] __attribute__((aligned(4)));
static uint32_t rx_index = 0;
static uint8_t last_link_state = 0xFF;

/**
 * @brief Reads a 16-bit register from the RTL8211F PHY via EMAC MDIO interface.
 * @note Utilizes standard CMSIS structure pointer EMAC0.
 */
static uint16_t emac_mdio_read(uint8_t phy_addr, uint8_t reg_addr) {
    /* Wait until the MDIO management interface becomes idle */
    while (EMAC0->EMAC_MII_CMD & EMAC_MII_BUSY);

    /* Construct the MII command with safe clock divider and target addresses */
    uint32_t cmd = ((phy_addr & 0x1F) << 16) |
                   ((reg_addr & 0x1F) << 4)  |
                   EMAC_MII_CLK_DIV_64       |
                   EMAC_MII_BUSY;

    EMAC0->EMAC_MII_CMD = cmd;

    /* Wait for the hardware to finish fetching data from the external PHY */
    while (EMAC0->EMAC_MII_CMD & EMAC_MII_BUSY);

    /* Return the read data from the lower 16 bits of the data register */
    return (uint16_t)(EMAC0->EMAC_MII_DATA & 0xFFFF);
}

/**
 * @brief Updates internal MAC speed and duplex settings within the EMAC0 peripheral.
 */
static void allwinner_emac_update_mac_speed(uint32_t speed, uint32_t is_full_duplex) {
    uint32_t ctl_val = EMAC0->EMAC_BASIC_CTL0;

    /* Strip old speed (bits 3:2) and duplex (bit 0) fields */
    ctl_val &= ~((3 << 2) | (1 << 0));

    if (is_full_duplex) {
        ctl_val |= EMAC_CTL_DUPLEX_FULL;
    }

    if (speed == 1000) {
        ctl_val |= EMAC_CTL_SPEED_1000;
    }
    else if (speed == 100) {
        ctl_val |= EMAC_CTL_SPEED_100;
    }
    else if (speed == 10) {
        ctl_val |= EMAC_CTL_SPEED_10;
    }

    EMAC0->EMAC_BASIC_CTL0 = ctl_val;
}

/**
 * @brief Periodically monitors RTL8211F link status and updates the lwIP network interface.
 * @note Intended to be called inside the main bare-metal superloop.
 */
static void check_ethernet_link_status(struct netif *netif) {
    uint16_t physr = emac_mdio_read(PHY_ADDR, RTL8211F_PHYSR);
    uint8_t link_up = (physr & PHYSR_LINK_STATUS) ? 1 : 0;

    if (link_up != last_link_state) {
        if (link_up) {
            uint8_t speed_bits = physr & PHYSR_SPEED_MASK;
            uint8_t duplex_bit = (physr & PHYSR_DUPLEX_STATUS) ? 1 : 0;

            uint32_t speed = 100;
            if (speed_bits == PHYSR_SPEED_10)       speed = 10;
            else if (speed_bits == PHYSR_SPEED_100)  speed = 100;
            else if (speed_bits == PHYSR_SPEED_1000) speed = 1000;

            /* Sync MAC configuration with actual negotiated line parameters */
            allwinner_emac_update_mac_speed(speed, duplex_bit);

            /* Propagate state flags into the lwIP stack engine */
            netif_set_link_up(netif);
            if (!netif_is_up(netif)) {
                netif_set_up(netif);
            }
        } else {
            /* Drop link state in the core network stack */
            netif_set_link_down(netif);
        }
        last_link_state = link_up;
    }
}

/**
 * @brief Polls the EMAC0 DMA RX ring, extracts packets, and shifts them into lwIP.
 */
static void ethernetif_poll(struct netif *netif) {
    struct emac_dma_desc *current_desc;
    struct pbuf *p = NULL;
    struct pbuf *q;
    uint32_t len;

    while (1) {
        current_desc = &rx_desc_ring[rx_index];

        /* Check if the hardware DMA controller still owns this descriptor segment */
        if (current_desc->status & DESC_OWN_BY_DMA) {
            break;
        }

        /* Enforce processing only on fully assembled error-free incoming frames */
        if ((current_desc->status & DESC_RX_FIRST) &&
            (current_desc->status & DESC_RX_LAST) &&
           !(current_desc->status & DESC_RX_ERRORS_MASK)) {

            len = (current_desc->status & DESC_RX_FL_MASK) >> DESC_RX_FL_SHIFT;

            if (len > 0) {
                p = pbuf_alloc(PBUF_RAW, (uint16_t)len, PBUF_POOL);

                if (p != NULL) {
                    uint32_t bytes_copied = 0;
                    uint8_t *src_ptr = (uint8_t *)current_desc->buf_addr;

                    for (q = p; q != NULL; q = q->next) {
                        memcpy(q->payload, &src_ptr[bytes_copied], q->len);
                        bytes_copied += q->len;
                    }

                    if (netif->input(p, netif) != ERR_OK) {
                        pbuf_free(p);
                    }
                }
            }
        }

        /* Hand the descriptor frame back to the EMAC hardware engine */
        current_desc->status = DESC_OWN_BY_DMA;

        /* Increment and wrap the ring array tracker index */
        rx_index++;
        if (rx_index >= EMAC_RX_BUFFERS_COUNT) {
            rx_index = 0;
        }

        /* Poke the receive DMA poll command register to force ring re-scanning */
        EMAC0->EMAC_RX_CTL0 = 0x1;
    }
}

//

#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include <string.h>

/* Allwinner T507-H EMAC TX Descriptor Configuration */
#define EMAC_TX_BUFFERS_COUNT   16
#define EMAC_TX_MAX_PACKET_SIZE 1536

/* Synopsys DesignWare / Allwinner EMAC DMA TX Descriptor Layout */
struct emac_dma_tx_desc {
    volatile uint32_t status;      /* TDES0: Status / Control */
    volatile uint32_t control;     /* TDES1: Buffer size / Chain flags */
    volatile uint32_t buf_addr;    /* TDES2: Physical memory pointer to payload */
    volatile uint32_t next_desc;   /* TDES3: Next descriptor pointer (Chained) */
};

/* Bit definitions for emac_dma_tx_desc.status (TDES0) */
#define TDES0_OWN_BY_DMA        (1UL << 31)  /* 1 = Hardware owns descriptor, 0 = CPU owns it */
#define TDES0_TX_LAST           (1UL << 30)  /* Last segment of the frame */
#define TDES0_TX_FIRST          (1UL << 29)  /* First segment of the frame */
#define TDES0_CHECKSUM_INSERT   (3UL << 27)  /* Enable IP/TCP/UDP hardware checksum calculation */
#define TDES0_TX_CHAINED        (1UL << 20)  /* Second address is next descriptor link */

/* Bit definitions for emac_dma_tx_desc.control (TDES1) */
#define TDES1_BUFFER_SIZE_MASK  0x7FF        /* Size of transmit buffer */

/* Driver static structures for TX ring execution */
static struct emac_dma_tx_desc tx_desc_ring[EMAC_TX_BUFFERS_COUNT] __attribute__((aligned(64)));
static uint8_t tx_buffer_pool[EMAC_TX_BUFFERS_COUNT][EMAC_TX_MAX_PACKET_SIZE] __attribute__((aligned(64)));
static uint32_t tx_index = 0;

/* External cache maintenance functions (provided by your toolchain/BSP architecture) */
extern void arch_dcache_clean_range(uint32_t start_addr, uint32_t size);
extern void arch_dcache_invalidate_range(uint32_t start_addr, uint32_t size);

/**
 * @brief Transmits an lwIP pbuf packet chain through the Allwinner EMAC0 TX descriptor ring.
 * @param netif Pointer to the lwIP network interface configuration.
 * @param p Pointer to the allocated lwIP packet buffer containing data frames.
 * @return ERR_OK on success, ERR_MEM if the TX ring is saturated.
 */
static err_t low_level_output(struct netif *netif, struct pbuf *p) {
    struct emac_dma_tx_desc *current_desc;
    struct pbuf *q;
    uint32_t total_bytes = 0;
    uint8_t *dst_ptr;

    (void)netif;

    /* Read the current descriptor index state from the ring */
    current_desc = &tx_desc_ring[tx_index];

    /* Invalidate descriptor from D-Cache to get the actual hardware state */
    arch_dcache_invalidate_range((uint32_t)current_desc, sizeof(struct emac_dma_tx_desc));

    /* Check if the descriptor is still held and processed by the EMAC DMA engine */
    if (current_desc->status & TDES0_OWN_BY_DMA) {
        /* TX ring buffer saturation - core stack must retry later */
        return ERR_MEM;
    }

    dst_ptr = &tx_buffer_pool[tx_index][0];

    /* Flatten the multi-segmented lwIP pbuf chain into a linear hardware buffer */
    for (q = p; q != NULL; q = q->next) {
        if ((total_bytes + q->len) > EMAC_TX_MAX_PACKET_SIZE) {
            /* Packet size exceeds hardware buffer limits, safety abort */
            return ERR_VAL;
        }
        memcpy(&dst_ptr[total_bytes], q->payload, q->len);
        total_bytes += q->len;
    }

    /* Push the newly populated data payload out of the CPU data cache into physical DDR memory */
    arch_dcache_clean_range((uint32_t)dst_ptr, total_bytes);

    /* Set buffer parameters and establish the descriptor configuration */
    current_desc->control = (total_bytes & TDES1_BUFFER_SIZE_MASK);
    current_desc->buf_addr = (uint32_t)dst_ptr;

    /* Mark the descriptor as a single complete frame with hardware checksum offloading */
    current_desc->status = TDES0_TX_FIRST | TDES0_TX_LAST | TDES0_TX_CHAINED | TDES0_CHECKSUM_INSERT;

    /* Hand over descriptor ownership to the EMAC hardware engine */
    current_desc->status |= TDES0_OWN_BY_DMA;

    /* Push the modified descriptor out of the CPU cache to ensure DMA visible execution */
    arch_dcache_clean_range((uint32_t)current_desc, sizeof(struct emac_dma_tx_desc));

    /* Increment and wrap around the active TX index tracker */
    tx_index++;
    if (tx_index >= EMAC_TX_BUFFERS_COUNT) {
        tx_index = 0;
    }

    /* Poke the transmit poll command register to awake the TX DMA controller if suspended */
    EMAC0->EMAC_TX_CTL1 = 0x1;

    return ERR_OK;
}

#endif


void nic_initialize(void)
{
	//PRINTF("nic_initialize start\n");
	allwinner_emac_hw_initialize();
	on_packet = nic_on_packet;
	//PRINTF("nic_initialize done\n");

}

void nic_linkspool(void * ctx)
{
	struct netif * const netif = (struct netif *) ctx;
	check_ethernet_link_status(netif);
}


#endif /* WITHLWIP && WITHETHHW && (CPUSTYLE_T507) */
