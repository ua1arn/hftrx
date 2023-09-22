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

// компас - RS232
// отладка - RS232

void user_uart0_onrxchar(uint_fast8_t c)
{

}

void user_uart0_ontxchar(void * ctx)
{

}

void user_uart0_initialize(void)
{
#if ! (WITHDEBUG && WITHDEBUG_USART0)

	hardware_uart0_initialize(0);
	hardware_uart0_set_speed(115200);
	hardware_uart0_enablerx(0);
	hardware_uart0_enabletx(0);

#endif
}

void uart0_spool(void)
{
#if ! (WITHDEBUG && WITHDEBUG_USART0)

#endif
}

#endif /* WITHCTRLBOARDT507 */
