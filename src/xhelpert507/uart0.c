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


// отладка - RS232
void user_uart0_onrxchar(uint_fast8_t c)
{

}

void user_uart0_ontxchar(void * ctx)
{

}

void user_uart0_initialize(void)
{
	hardware_uart0_initialize(1);
	hardware_uart0_set_speed(9600);
	hardware_uart0_enablerx(0);
	hardware_uart0_enabletx(0);
}

#endif /* WITHCTRLBOARDT507 */

