/*
 * ctlboardt507.c
 *
 *  Created on: 14 сент. 2023 г.
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

void user_uart1_onrxchar(uint_fast8_t c)
{

}

void user_uart1_ontxchar(void * ctx)
{

}

void user_uart2_onrxchar(uint_fast8_t c)
{

}

void user_uart2_ontxchar(void * ctx)
{

}

void user_uart3_onrxchar(uint_fast8_t c)
{

}

void user_uart3_ontxchar(void * ctx)
{

}

// тушка - RS485
void user_uart4_onrxchar(uint_fast8_t c)
{

}

void user_uart4_ontxchar(void * ctx)
{

}

void user_uart5_onrxchar(uint_fast8_t c)
{

}

void user_uart5_ontxchar(void * ctx)
{

}

/////////////////////////
///
void ctlboardt507_mainloop(void)
{
	PRINTF("ctlboardt507_mainloop [%p]\n", ctlboardt507_mainloop);

	hardware_uart0_initialize(1);
	hardware_uart0_set_speed(9600);
	hardware_uart0_enablerx(0);
	hardware_uart0_enabletx(0);

	hardware_uart1_initialize(1);
	hardware_uart1_set_speed(9600);
	hardware_uart1_enablerx(0);
	hardware_uart1_enabletx(0);

	hardware_uart2_initialize(1);
	hardware_uart2_set_speed(9600);
	hardware_uart2_enablerx(0);
	hardware_uart2_enabletx(0);

	hardware_uart3_initialize(1);
	hardware_uart3_set_speed(9600);
	hardware_uart3_enablerx(0);
	hardware_uart3_enabletx(0);

	hardware_uart4_initialize(1);
	hardware_uart4_set_speed(9600);
	hardware_uart4_enablerx(0);
	hardware_uart4_enabletx(0);

	hardware_uart5_initialize(1);
	hardware_uart5_set_speed(9600);
	hardware_uart5_enablerx(0);
	hardware_uart5_enabletx(0);


	PRINTF("ctlboardt507_mainloop: wait user loop [%p]\n", ctlboardt507_mainloop);
	PRINTF("ctlboardt507_mainloop: wait user loop, CPU_FREQ=%u MHz\n", (unsigned) (CPU_FREQ / 1000 / 1000));

	int state = 0;
	/* Обеспечение работы USB DFU */
	for (;;)
	{
		uint_fast8_t kbch, kbready;
		processmessages(& kbch, & kbready, 0, NULL);

		if (hardware_uart0_putchar('0'))
			;
		if (hardware_uart1_putchar('1'))
			;
		if (hardware_uart2_putchar('2'))
			;
		if (hardware_uart3_putchar('3'))
			;
		if (hardware_uart4_putchar('4'))
			;
		if (hardware_uart5_putchar('5'))
			;

		state = ! state;
		gpioX_setstate(GPIOA, BOARD_BLINK_BIT1, !! (state) * BOARD_BLINK_BIT1);

		if (kbready)
			PRINTF("bkbch=%02x\n", kbch);

		{
			/* здесь можно добавить обработку каких-либо команд с debug порта */
			char c;
			if (dbg_getchar(& c))
			{
				switch (c)
				{
				case 0x00:
					break;
				default:
					PRINTF("bkey=%02X\n", (unsigned char) c);
					break;

				}
			}
		}
	}
}

#endif /* WITHCTRLBOARDT507 */
