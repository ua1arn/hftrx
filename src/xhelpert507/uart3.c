/*
 * uart0.c
 *
 *  Created on: 21 сент. 2023 г.
 *      Author: User
 */

#include "hardware.h"
#include "formats.h"
#include "bootloader.h"
#include "board.h"
#include "gpio.h"

#include "xhelper507.h"

#if WITHCTRLBOARDT507

// БИНС основной
// RS-422

void user_uart3_onrxchar(uint_fast8_t c)
{

}

void user_uart3_ontxchar(void * ctx)
{

}

void user_uart3_initialize(void)
{
	hardware_uart3_initialize(1);
	hardware_uart3_set_speed(921600);
	hardware_uart3_enablerx(0);
	hardware_uart3_enabletx(0);
}

#endif /* WITHCTRLBOARDT507 */

