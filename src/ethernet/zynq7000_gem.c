/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if WITHETHHW && CPUSTYLE_XC7Z && WITHLWIP

#include "gpio.h"

int nic_can_send(void)
{
	return 0;
}

void nic_send(const uint8_t * data, int isize)
{
}
void nic_initialize(void)
{
	PRINTF("nic_initialize:\n");

	SCLR->SLCR_UNLOCK = 0x0000DF0DU;
	SCLR->APER_CLK_CTRL |= (0x01uL << 6);	// APER_CLK_CTRL.GEM0_CPU_1XCLKACT
	SCLR->GEM0_CLK_CTRL = //(SCLR->GEM0_CLK_CTRL & ~ (0x00uL)) |
			((uint_fast32_t) 0x08 << 20) |	// DIVISOR1
			((uint_fast32_t) 0x05 << 8) |	// DIVISOR
			((uint_fast32_t) 0x00 << 4) |	// SRCSEL: 00x: IO PLL
			((uint_fast32_t) 0x01 << 0) |	// CLKACT
			0;
	SCLR->GEM0_RCLK_CTRL = 0x0000001uL;

	ASSERT(GEM0->MODULE_ID == 0x00020118uL);
	PRINTF("nic_initialize done\n");

	//	Net:   ZYNQ GEM: e000b000, phyaddr 7, interface rgmii-id
	//	eth0: ethernet@e000b000
	//	U-BOOT for myd_y7z020_10_07

}

#endif /* #if WITHETHHW && CPUSTYLE_XC7Z && WITHLWIP */

