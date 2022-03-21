/*
 * rza1xx_hal.c
 *
 *  Created on: May 6, 2021
 *      Author: gena
 */



#include "hardware.h"

#if CPUSTYLE_XC7Z

#include "board.h"
#include "formats.h"
#include "zynq7000_hal.h"

//uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */

__WEAK void HAL_Delay(uint32_t Delay)
{
	local_delay_ms(Delay);

//  uint32_t tickstart = HAL_GetTick();
//  uint32_t wait = Delay;
//
//  /* Add a freq to guarantee minimum wait */
//  if (wait < HAL_MAX_DELAY)
//  {
//    wait += (uint32_t)(uwTickFreq);
//  }
//
//  while ((HAL_GetTick() - tickstart) < wait)
//  {
//  }
}

__IO uint32_t uwTick;

const uint32_t uwTickFreq = 1000 / TICKS_FREQUENCY;

/**
  * @brief This function is called to increment  a global variable "uwTick"
  *        used as application time base.
  * @note In the default implementation, this variable is incremented each 1ms
  *       in Systick ISR.
 * @note This function is declared as __weak to be overwritten in case of other
  *      implementations in user file.
  * @retval None
  */
__WEAK void HAL_IncTick(void)
{
  uwTick += (uint32_t)uwTickFreq;
}

HAL_StatusTypeDef HAL_Init(void)
{
	  return HAL_OK;
}

#endif /* CPUSTYLE_XC7Z */


#if CPUSTYLE_XC7Z
// https://xilinx.github.io/embeddedsw.github.io/usbps/doc/html/api/xusbps__hw_8h.html

//	USB ULPI Viewport Register (ULPIVIEW) bit positions.
#define XUSBPS_ULPIVIEW_DATWR_MASK   	0x000000FF	// 	ULPI Data Write. More...
#define	XUSBPS_ULPIVIEW_DATWR_SHIFT		0
#define XUSBPS_ULPIVIEW_DATRD_MASK   	0x0000FF00	// 	ULPI Data Read. More...
#define XUSBPS_ULPIVIEW_DATRD_SHIFT	   	8			// 	ULPI Data Address. More...
#define XUSBPS_ULPIVIEW_ADDR_MASK   	0x00FF0000	// 	ULPI Data Address. More...
#define XUSBPS_ULPIVIEW_ADDR_SHIFT	   	16			// 	ULPI Data Address. More...
#define XUSBPS_ULPIVIEW_PORT_MASK   	0x07000000	// 	ULPI Port Number. More...
#define XUSBPS_ULPIVIEW_SS_MASK   		0x08000000	// 	ULPI Synchronous State. More...
#define XUSBPS_ULPIVIEW_RW_MASK   		0x20000000	// 	ULPI Read/Write Control. More...
#define XUSBPS_ULPIVIEW_RUN_MASK   		0x40000000	// 	ULPI Run. More...
#define XUSBPS_ULPIVIEW_WU_MASK   		0x80000000	// 	ULPI Wakeup. More...

#define XUSBPS_ULPIVIEW			0xE0002170	// USB0
#define XUSBPS_ULPIVIEW2		((uintptr_t) WITHUSBHW_EHCI + 0x00000170)	// USB0

#define ULPIVIEW (* ((volatile uint32_t *) XUSBPS_ULPIVIEW))

static void ulpi_reg_set(uint_fast8_t addr, uint_fast8_t data)
{
	ULPIVIEW = (ULPIVIEW & ~ (XUSBPS_ULPIVIEW_ADDR_MASK | XUSBPS_ULPIVIEW_DATWR_MASK)) |
			(((uint_fast32_t) addr << XUSBPS_ULPIVIEW_ADDR_SHIFT) & XUSBPS_ULPIVIEW_ADDR_MASK ) |
			(((uint_fast32_t) data << XUSBPS_ULPIVIEW_DATWR_SHIFT) & XUSBPS_ULPIVIEW_DATWR_MASK ) |
			0;
	ULPIVIEW |= XUSBPS_ULPIVIEW_RW_MASK;	// Select write direction
	ULPIVIEW |= XUSBPS_ULPIVIEW_RUN_MASK;
	while ((ULPIVIEW & XUSBPS_ULPIVIEW_RUN_MASK) != 0)
		;
}

static uint_fast8_t ulpi_reg_get(uint_fast8_t addr)
{
	ULPIVIEW = (ULPIVIEW & ~ (XUSBPS_ULPIVIEW_ADDR_MASK | XUSBPS_ULPIVIEW_DATWR_MASK)) |
			(((uint_fast32_t) addr << XUSBPS_ULPIVIEW_ADDR_SHIFT) & XUSBPS_ULPIVIEW_ADDR_MASK ) |
			0;

	ULPIVIEW &= ~ XUSBPS_ULPIVIEW_RW_MASK;	// Select read direction
	ULPIVIEW |= XUSBPS_ULPIVIEW_RUN_MASK;
	while ((ULPIVIEW & XUSBPS_ULPIVIEW_RUN_MASK) != 0)
		;
	return (ULPIVIEW & XUSBPS_ULPIVIEW_DATRD_MASK) >> XUSBPS_ULPIVIEW_DATRD_SHIFT;
}

void ulpi_chip_initialize(void)
{
	PRINTF("ulpi_chip_initialize: XUSBPS_ULPIVIEW=%08lX, calc=%08lX, ULPIVIEW=%08lX\n", XUSBPS_ULPIVIEW, XUSBPS_ULPIVIEW2, ULPIVIEW);
	// USB3340
	// Address = 00h (read only) Vendor ID Low = 0x24
	// Address = 01h (read only) Vendor ID High = 0x04
	// Address = 02h (read only) Product ID Low = 0x09
	// Address = 03h (read only)  Product ID High = 0x00
	const uint_fast8_t v0 = ulpi_reg_get(0x00);
	const uint_fast8_t v1 = ulpi_reg_get(0x01);
	const uint_fast8_t v2 = ulpi_reg_get(0x02);
	const uint_fast8_t v3 = ulpi_reg_get(0x03);
	const uint_fast16_t vid = v1 * 256 + v0;
	const uint_fast16_t pid = v3 * 256 + v2;
	PRINTF("ULPI chip: VendorID=%04X, productID=%04X\n", vid, pid);

	if (vid != 0X0424 || pid != 0x0009)
		return;

	// Address = 19-1Bh (read), 19h (write), 1Ah (set), 1Bh (clear)
	// Bit 0x01 - IdGndDrv set to 1
	PRINTF("ULPI chip: reg19=%02X\n", ulpi_reg_get(0x19));
	ulpi_reg_set(0x1A, 0x02);	// Set IdGndDrv bit
	PRINTF("ULPI chip: reg19=%02X\n", ulpi_reg_get(0x19));

}
#endif /* CPUSTYLE_XC7Z */
