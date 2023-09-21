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

// руль машинка
// RS-485

void user_uart1_onrxchar(uint_fast8_t c)
{

}

void user_uart1_ontxchar(void * ctx)
{

}

void user_uart1_initialize(void)
{
	hardware_uart1_initialize(1);
	hardware_uart1_set_speed(115200);
	hardware_uart1_enablerx(0);
	hardware_uart1_enabletx(0);
}

#endif /* WITHCTRLBOARDT507 */

