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
#define XUSBPS_ULPIVIEW2		((uintptr_t) WITHUSBHW_EHCI + 0x00000170)

static void ulpi_reg_set(uint_fast8_t addr, uint_fast8_t data)
{
	volatile uint32_t * const ulpivew = (volatile uint32_t *) XUSBPS_ULPIVIEW;	// USB0

	* ulpivew = (* ulpivew & ~ (XUSBPS_ULPIVIEW_ADDR_MASK | XUSBPS_ULPIVIEW_DATWR_MASK)) |
			(((uint_fast32_t) addr << XUSBPS_ULPIVIEW_ADDR_SHIFT) & XUSBPS_ULPIVIEW_ADDR_MASK ) |
			(((uint_fast32_t) data << XUSBPS_ULPIVIEW_DATWR_SHIFT) & XUSBPS_ULPIVIEW_DATWR_MASK ) |
			0;
}

static uint_fast8_t ulpi_reg_get(uint_fast8_t addr)
{
	volatile uint32_t * const ulpivew = (volatile uint32_t *) XUSBPS_ULPIVIEW;	// USB0

	* ulpivew = (* ulpivew & ~ (XUSBPS_ULPIVIEW_ADDR_MASK | XUSBPS_ULPIVIEW_DATWR_MASK)) |
			(((uint_fast32_t) addr << XUSBPS_ULPIVIEW_ADDR_SHIFT) & XUSBPS_ULPIVIEW_ADDR_MASK ) |
			0;

	return 0;
}

void ulpi_chip_initialize(void)
{
	PRINTF("ulpi_chip_initialize: XUSBPS_ULPIVIEW=%08lX, calc=%08lX\n", XUSBPS_ULPIVIEW, XUSBPS_ULPIVIEW2);
}
#endif /* CPUSTYLE_XC7Z */
