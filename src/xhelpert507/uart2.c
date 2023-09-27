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

// БИНС поправки
// RS-485 38400 8N1

#define PERIODSPOOL 2000
//#define RXTOUT 50

void user_uart2_onrxchar(uint_fast8_t c)
{

}

void user_uart2_ontxchar(void * ctx)
{

}

void user_uart2_initialize(void)
{
	hardware_uart2_initialize(1, 38400, 8, 0, 0);
	hardware_uart2_set_speed(38400);
	hardware_uart2_enablerx(0);
	hardware_uart2_enabletx(0);
}

void uart2_spool(void)
{
}

#endif /* WITHCTRLBOARDT507 */

