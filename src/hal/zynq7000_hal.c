/*
 * zynq7000_hal.c
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
