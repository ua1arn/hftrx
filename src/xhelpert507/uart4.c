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

// управление
// RS-485

//#define PERIODSPOOL 2000
#define RXTOUT 100

void user_uart4_onrxchar(uint_fast8_t c)
{

}

void user_uart4_ontxchar(void * ctx)
{

}

void user_uart4_initialize(void)
{
	hardware_uart4_initialize(1, 9600, 8, 0, 0);
	hardware_uart4_set_speed(9600);
	hardware_uart4_enablerx(0);
	hardware_uart4_enabletx(0);
}

void uart4_spool(void)
{
}

#endif /* WITHCTRLBOARDT507 */

