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
				case 0x00:
					break;
				case ' ':
					uart3_req();
					break;
				default:
					PRINTF("bkey=%02X\n", (unsigned char) c);
					break;

				}
			}
		}
	}
}

void uint8_queue_init(u8queue_t * q)
{
	q->qg = q->qp = 0;
}

uint_fast8_t uint8_queue_put(u8queue_t * q, uint_fast8_t c)
{
	unsigned qpt = q->qp;
	const unsigned next = (qpt + 1) % qSZ;
	if (next != q->qg)
	{
		q->buffer [qpt] = c;
		q->qp = next;
		hardware_uart3_enabletx(1);
		return 1;
	}
	return 0;
}

uint_fast8_t uint8_queue_get(u8queue_t * q, uint_fast8_t * pc)
{
	if (q->qp != q->qg)
	{
		* pc = q->buffer [q->qg];
		q->qg = (q->qg + 1) % qSZ;
		return 1;
	}
	return 0;
}

uint_fast8_t uint8_queue_empty(const u8queue_t * q)
{
	return q->qp == q->qg;
}

#endif /* WITHCTRLBOARDT507 */
