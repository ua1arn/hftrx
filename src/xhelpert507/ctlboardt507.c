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

/////////////////////////
///
void ctlboardt507_mainloop(void)
{
	PRINTF("ctlboardt507_mainloop [%p]\n", ctlboardt507_mainloop);

	user_uart0_initialize();
	user_uart1_initialize();
	user_uart2_initialize();
	user_uart3_initialize();
	user_uart4_initialize();
	user_uart5_initialize();


	PRINTF("ctlboardt507_mainloop: wait user loop [%p]\n", ctlboardt507_mainloop);
	PRINTF("ctlboardt507_mainloop: wait user loop, CPU_FREQ=%u MHz\n", (unsigned) (CPU_FREQ / 1000 / 1000));

	int state = 0;
	/* Обеспечение работы USB DFU */
	for (;;)
	{
		uint_fast8_t kbch, kbready;
		processmessages(& kbch, & kbready, 0, NULL);
		int fl = 0;
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
			fl = 1;

		if (fl)
		{
			state = ! state;
			gpioX_setstate(GPIOA, BOARD_BLINK_BIT1, !! (state) * BOARD_BLINK_BIT1);
		}

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
