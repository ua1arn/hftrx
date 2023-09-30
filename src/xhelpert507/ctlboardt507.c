/*
 * ctlboardt507.c
 *
 *  Created on: 14 сент. 2023 г.
 *      Author: User
 */

#include "hardware.h"

#if WITHCTRLBOARDT507

#include "formats.h"
#include "bootloader.h"
#include "board.h"
#include "gpio.h"
#include "xhelper507.h"


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
	PRINTF("ctlboardt507_mainloop: wait user loop, allwnr_t507_get_apb2_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_apb2_freq() / 1000 / 1000));

	int state = 0;
	/* Обеспечение работы USB DFU */
	for (;;)
	{
		uint_fast8_t kbch, kbready;
		processmessages(& kbch, & kbready, 0, NULL);

		uart0_spool();
		uart1_spool();
		uart2_spool();
		uart3_spool();
		uart4_spool();
		uart5_spool();

		if (kbready)
			PRINTF("bkbch=%02x\n", kbch);

		{
			/* здесь можно добавить обработку каких-либо команд с debug порта */
			char c;
			if (dbg_getchar(& c))
			{
				switch (c)
				{
//				case 0x00:
//					break;
//				case ' ':
//					uart1_req();
//					break;
				default:
					PRINTF("bkey=%02X\n", (unsigned char) c);
					break;

				}
			}
		}
	}
}

#endif /* WITHCTRLBOARDT507 */
