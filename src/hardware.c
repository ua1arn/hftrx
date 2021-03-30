/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "keyboard.h"

#include <string.h>
#include <math.h>

#include "board.h"
#include "audio.h"
#include "formats.h"	// for debug prints
#include "gpio.h"
#include "spi.h"

/* 
	Машинно-независимый обработчик прерываний.
	Вызывается с периодом 1/ELKEY_DISCRETE от длительности точки
*/
RAMFUNC void spool_elkeybundle(void)
{
#if WITHOPERA4BEACON
	spool_0p128();
#elif WITHELKEY
	elkey_spool_dots();		// вызывается с периодом 1/ELKEY_DISCRETE от длительности точки
#endif /* WITHOPERA4BEACON */
#if WITHENCODER2 && defined (ENCODER2_BITS)
	spool_encinterrupt2();	/* прерывание по изменению сигнала на входах от валкодера #2*/
#endif /* WITHENCODER2 && ENCODER2_BITS */
}

/* 
	Машинно-независимый обработчик прерываний.
	Вызывается при изменении состояния входов электронного ключа,
    входа манипуляции от CAT (CAT_DTR).
*/
RAMFUNC void spool_elkeyinputsbundle(void)
{
	//key_spool_inputs();	// опрос состояния электронного ключа и запоминание факта нажатия
}


static LIST_ENTRY tickers;
static LIST_ENTRY adcdones;
//static unsigned nowtick;

void ticker_initialize(ticker_t * p, unsigned nticks, void (* cb)(void *), void * ctx)
{
	p->period = nticks;
	//p->fired = nowtick;
	p->ticks = 0;
	p->cb = cb;
	p->ctx = ctx;
	InsertHeadList(& tickers, & p->item);
}

static void tickers_spool(void)
{

	//++ nowtick;
	PLIST_ENTRY t;
	for (t = tickers.Blink; t != & tickers; t = t->Blink)
	{
		ticker_t * const p = CONTAINING_RECORD(t, ticker_t, item);
	
		//if (p->next <= nowtick)
		if (++ p->ticks >= p->period)
		{
			//p->fired = nowtick;
			p->ticks = 0;
			if (p->cb != NULL)
				(p->cb)(p->ctx);
		}
	}
}

void tickers_initialize(void)
{
	InitializeListHead(& tickers);

}

// регистрируются обработчики конца преобразвания АЦП
void adcdones_initialize(void)
{
	InitializeListHead(& adcdones);
}

// регистрируются обработчики конца преобразвания АЦП
void adcdone_initialize(adcdone_t * p, void (* cb)(void *), void * ctx)
{
	p->cb = cb;
	p->ctx = ctx;
	InsertHeadList(& adcdones, & p->item);
}

static void adcdones_spool(void)
{

	//++ nowtick;
	PLIST_ENTRY t;
	for (t = adcdones.Blink; t != & adcdones; t = t->Blink)
	{
		adcdone_t * const p = CONTAINING_RECORD(t, adcdone_t, item);

		if (p->cb != NULL)
			(p->cb)(p->ctx);
	}
}

#if WITHLWIP
static volatile uint32_t sys_now_counter;
uint32_t sys_now(void)
{
	return sys_now_counter;
}
#endif /* WITHLWIP */

/* Машинно-независимый обработчик прерываний. */
// Функции с побочным эффектом - отсчитывание времени.
// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
RAMFUNC void spool_systimerbundle1(void)
{
	//beacon_255();

#if WITHLWIP
	sys_now_counter += (1000 / TICKS_FREQUENCY);
#endif /* WITHLWIP */

	//spool_lfm();
	tickers_spool();
}

/* Машинно-независимый обработчик прерываний. */
// Функции с побочным эффектом редиспетчеризации.
// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
RAMFUNC void spool_systimerbundle2(void)
{
#if 1 && defined (BOARD_BLINK_SETSTATE)
	{
	#if WITHISBOOTLOADER
		const unsigned thalf = 100;	// Toggle every 100 ms
	#else /* WITHISBOOTLOADER */
		const unsigned thalf = 500;	// Toggle every 500 ms
	#endif /* WITHISBOOTLOADER */
		// BLINK test
		static unsigned count;
		if (++ count >= NTICKS(thalf))
		{
			count = 0;
			static uint_fast8_t state;
			state = ! state;
			BOARD_BLINK_SETSTATE(state);
		}
	}
#endif /* defined (BOARD_BLINK_SETSTATE) */

#if WITHCPUADCHW
	hardware_adc_startonescan();	// хотя бы один вход (s-метр) есть.
#endif /* WITHCPUADCHW */
}

#if WITHCPUADCHW
/* 
	Машинно-независимый обработчик прерываний.
	Вызывается с периодом 1/TIMETICKS по окончании получения данных всех каналов АЦП,
	перечисленных в таблице adcinputs.
*/

RAMFUNC void spool_adcdonebundle(void)
{
	adcdones_spool();
}
#endif /* WITHCPUADCHW */


#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32F
/* прерывания от валколера при наличии в системе вложенных прерываний вызываются на уровне приоритета REALTINE */
static RAMFUNC void stm32fxxx_pinirq(portholder_t pr)
{
#if WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT)
	if ((pr & (ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT)) != 0)
	{
		spool_elkeyinputsbundle();
	}
#endif /* WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT) */
#if WITHENCODER && defined (ENCODER_BITS)
	if ((pr & ENCODER_BITS) != 0)
	{
		spool_encinterrupt();	/* прерывание по изменению сигнала на входах от валкодера #1*/
	}
#endif /* WITHENCODER && defined (ENCODER_BITS) */
#if WITHENCODER2 && defined (ENCODER2_BITS)
	if ((pr & ENCODER2_BITS) != 0)
	{
		//spool_encinterrupt2();	/* прерывание по изменению сигнала на входах от валкодера #2*/
	}
#endif /* WITHENCODER && ENCODER2_BITS */
#if BOARD_GT911_INT_PIN
	if ((pr & BOARD_GT911_INT_PIN) != 0)
	{
		gt911_interrupt_handler();	/* прерывание по изменению сигнала на входе от тач */
	}
#endif
}

#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32F */

#if CPUSTYLE_STM32MP1

	void EXTI0_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM0;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI1_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM1;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI2_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM2;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI3_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM3;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI4_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM4;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI5_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM5;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI6_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM6;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI7_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM7;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI8_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM8;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI9_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM9;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI10_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM10;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI11_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM11;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI12_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM12;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI13_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM13;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI14_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM14;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

	void EXTI15_IRQHandler(void)
	{
		const uint_fast32_t mask = EXTI_IMR1_IM15;
		const portholder_t prf = EXTI->FPR1 & mask;
		EXTI->FPR1 = prf;		// reset all faling requests
		const portholder_t prr = EXTI->RPR1 & mask;
		EXTI->RPR1 = prr;		// reset all rising requests
		stm32fxxx_pinirq(prf | prr);
	}

#elif CPUSTYLE_STM32F

	#if CPUSTYLE_STM32L0XX

	void EXTI0_1_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_IM0 | EXTI_IMR_IM1);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}
	void EXTI2_3_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_IM2 | EXTI_IMR_IM3);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}
	void EXTI4_15_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (
				EXTI_IMR_IM15 | EXTI_IMR_IM14 | EXTI_IMR_IM13 | EXTI_IMR_IM12 | 
				EXTI_IMR_IM11 | EXTI_IMR_IM10 | EXTI_IMR_IM9 | EXTI_IMR_IM8 | 
				EXTI_IMR_IM7 | EXTI_IMR_IM6 | EXTI_IMR_IM5 | EXTI_IMR_IM4
				);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	#elif CPUSTYLE_STM32H7XX

	void EXTI0_IRQHandler(void)
	{
		const portholder_t pr = EXTI_D1->PR1 & (EXTI_PR1_PR0);
		EXTI_D1->PR1 = pr;		// reset all existing requests
		(void) EXTI_D1->PR1;
		stm32fxxx_pinirq(pr);
	}

	
	void EXTI1_IRQHandler(void)
	{
		const portholder_t pr = EXTI_D1->PR1 & (EXTI_PR1_PR1);
		EXTI_D1->PR1 = pr;		// reset all existing requests
		(void) EXTI_D1->PR1;
		stm32fxxx_pinirq(pr);
	}

	void EXTI2_IRQHandler(void)
	{
		const portholder_t pr = EXTI_D1->PR1 & (EXTI_PR1_PR2);
		EXTI_D1->PR1 = pr;		// reset all existing requests
		(void) EXTI_D1->PR1;
		stm32fxxx_pinirq(pr);
	}

	void EXTI3_IRQHandler(void)
	{
		const portholder_t pr = EXTI_D1->PR1 & (EXTI_PR1_PR3);
		EXTI_D1->PR1 = pr;		// reset all existing requests
		(void) EXTI_D1->PR1;
		stm32fxxx_pinirq(pr);
	}

	void EXTI4_IRQHandler(void)
	{
		const portholder_t pr = EXTI_D1->PR1 & (EXTI_PR1_PR4);
		EXTI_D1->PR1 = pr;		// reset all existing requests
		(void) EXTI_D1->PR1;
		stm32fxxx_pinirq(pr);
	}

	void EXTI9_5_IRQHandler(void)
	{
		const portholder_t pr = EXTI_D1->PR1 & (EXTI_PR1_PR9 | EXTI_PR1_PR8 | EXTI_PR1_PR7 | EXTI_PR1_PR6 | EXTI_PR1_PR5);
		EXTI_D1->PR1 = pr;		// reset all existing requests
		(void) EXTI_D1->PR1;
		stm32fxxx_pinirq(pr);
	}
	void EXTI15_10_IRQHandler(void)
	{
		const portholder_t pr = EXTI_D1->PR1 & (EXTI_PR1_PR15 | EXTI_PR1_PR14 | EXTI_PR1_PR13 | EXTI_PR1_PR12 | EXTI_PR1_PR11 | EXTI_PR1_PR10);
		EXTI_D1->PR1 = pr;		// reset all existing requests
		(void) EXTI_D1->PR1;
		stm32fxxx_pinirq(pr);
	}

	#else

	void EXTI0_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR0);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void EXTI0_1_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR0 | EXTI_IMR_MR1);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void EXTI1_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR1);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void EXTI2_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR2);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void EXTI2_3_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR2 | EXTI_IMR_MR3);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void EXTI4_15_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (
				EXTI_IMR_MR15 | EXTI_IMR_MR14 | EXTI_IMR_MR13 | EXTI_IMR_MR12 | 
				EXTI_IMR_MR11 | EXTI_IMR_MR10 | EXTI_IMR_MR9 | EXTI_IMR_MR8 | 
				EXTI_IMR_MR7 | EXTI_IMR_MR6 | EXTI_IMR_MR5 | EXTI_IMR_MR4
				);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void EXTI3_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR3);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void EXTI4_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR4);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void EXTI9_5_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR9 | EXTI_IMR_MR8 | EXTI_IMR_MR7 | EXTI_IMR_MR6 | EXTI_IMR_MR5);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}
	void EXTI15_10_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR15 | EXTI_IMR_MR14 | EXTI_IMR_MR13 | EXTI_IMR_MR12 | EXTI_IMR_MR11 | EXTI_IMR_MR10);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}
	#endif

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	void RAMFUNC_NONILINE
	PIOA_Handler(void)
	{
		//display_menu_label(PSTR("PIOA_IrqHandler"));
		//for (;;)
		//	;
		// When the software reads PIO_ISR, all the interrupts are automatically cleared. This signifies that
		// all the interrupts that are pending when PIO_ISR is read must be handled.
		const portholder_t state = PIOA->PIO_ISR;
	#if WITHENCODER && defined (ENCODER_BITS)
		if ((state & (ENCODER_BITS)) != 0) // re-enable interrupt from PIO
		{
			spool_encinterrupt();	/* прерывание по изменению сигнала на входах от валкодера */
		}
	#endif /* WITHENCODER && defined (ENCODER_BITS) */
	#if WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT)
		if ((state & (ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT)) != 0) // re-enable interrupt from PIO
		{
			spool_elkeyinputsbundle();
		}
	#endif /* WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT) */
	#if WITHNMEA
		if ((state & FROMCAT_BIT_DTR) != 0 && (FROMCAT_TARGET_PIN_DTR & FROMCAT_BIT_DTR) != 0)
		{
			spool_nmeapps();
		}
	#endif /* WITHNMEA */
	#if BOARD_GT911_INT_PIN
		if ((state & BOARD_GT911_INT_PIN) != 0)
		{
			gt911_interrupt_handler();	/* прерывание по изменению сигнала на входе от тач */
		}
	#endif
	}

#elif CPUSTYLE_AT91SAM7S

	RAMFUNC_NONILINE void AT91F_PIOA_IRQHandler(void)
	{
		// When the software reads PIO_ISR, all the interrupts are automatically cleared. This signifies that
		// all the interrupts that are pending when PIO_ISR is read must be handled.
		const portholder_t state = AT91C_BASE_PIOA->PIO_ISR;
	#if WITHENCODER && defined (ENCODER_BITS)
		if ((state & (ENCODER_BITS)) != 0) // re-enable interrupt from PIO
		{
			spool_encinterrupt();	/* прерывание по изменению сигнала на входах от валкодера */
		}
	#endif /* WITHENCODER && defined (ENCODER_BITS) */
	#if WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT)
		if ((state & (ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT)) != 0) // re-enable interrupt from PIO
		{
			spool_elkeyinputsbundle();
		}
	#endif /* WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT) */
	#if WITHNMEA
		if ((state & FROMCAT_BIT_DTR) != 0 && (FROMCAT_TARGET_PIN_DTR & FROMCAT_BIT_DTR) != 0)
		{
			spool_nmeapps();
		}
	#endif /* WITHNMEA */
	#if BOARD_GT911_INT_PIN
		if ((state & BOARD_GT911_INT_PIN) != 0)
		{
			gt911_interrupt_handler();	/* прерывание по изменению сигнала на входе от тач */
		}
	#endif
	}

#elif CPUSTYLE_ATMEGA

	ISR(INT0_vect)
	{
		spool_encinterrupt();	/* прерывание по изменению сигнала на входах от валкодера */
	}

	ISR(INT1_vect)
	{
		spool_encinterrupt();	/* прерывание по изменению сигнала на входах от валкодера */
	}


	// Timer 1 output compare A interrupt service routine
	ISR(TIMER1_COMPA_vect)
	{
		spool_elkeybundle();
	}
	// Обработчик по изменению состояния входов PTT и электронного ключа
	#if CPUSTYLE_ATMEGA_XXX4
		#if WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT)
			// PC7 - PTT input, PC6 & PC5 - eectronic key inputs
			ISR(PCIVECT)
			{
				spool_elkeyinputsbundle();
			}
		#endif /* (WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT)) */
		#if defined (FROMCAT_BIT_DTR) && defined (DTRPCICR_BIT) && (PCICR_BIT != DTRPCICR_BIT)
			ISR(DTRPCIVECT)
			{
				spool_elkeyinputsbundle();	// по изменению PTT
			}
		#endif
	#endif /* CPUSTYLE_ATMEGA_XXX4 && defined (PCIVECT) */

#else

	//#warning Undefined CPUSTYLE_XXX encoder interrrupts handlers
#endif

//static volatile uint_fast8_t hardware_reqshutdown;
/* возвращаем запрос на выключение - от компаратора питания */
uint_fast8_t 
hardware_getshutdown(void)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F30X || CPUSTYLE_STM32L0XX
	//return hardware_reqshutdown;
	return (PWR->CR & PWR_CR_PVDE) && (PWR->CSR & PWR_CSR_PVDO);

#else
	return 0;
#endif
}

void 
hardware_encoder_initialize(void)
{
#if WITHENCODER
	ENCODER_INITIALIZE();
#endif /* WITHENCODER */
}

/* Чтение состояния выходов валкодера #1 - в два младших бита */
/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

uint_fast8_t 
hardware_get_encoder_bits(void)
{
#if WITHENCODER && defined (ENCODER_BITS) && defined (ENCODER_SHIFT)
	return (ENCODER_INPUT_PORT & ENCODER_BITS) >> ENCODER_SHIFT;	// Биты валкодера #1
#elif WITHENCODER && defined (ENCODER_BITS)
	const portholder_t v = ENCODER_INPUT_PORT;
	return ((v & ENCODER_BITA) != 0) * 2 + ((v & ENCODER_BITB) != 0);	// Биты идут не подряд
#else /* WITHENCODER */
	return 0;
#endif /* WITHENCODER */
}

/* Чтение состояния выходов валкодера #2 - в два младших бита */
/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

uint_fast8_t 
hardware_get_encoder2_bits(void)
{
#if WITHENCODER && ENCODER2_BITS && defined (ENCODER2_SHIFT)
	return (ENCODER2_INPUT_PORT & ENCODER2_BITS) >> ENCODER2_SHIFT;	// Биты валкодера #2
#elif WITHENCODER && ENCODER2_BITS
	const portholder_t v = ENCODER2_INPUT_PORT;
	return ((v & ENCODER2_BITA) != 0) * 2 + ((v & ENCODER2_BITB) != 0);	// Биты идут не подряд
#elif WITHENCODER && CPUSTYLE_XC7Z && defined (ENCODER2_GPIO_BANK)
	return ((XGpioPs_Read(& xc7z_gpio, ENCODER2_GPIO_BANK) >> ENCODER2_GPIO_SHIFT) & ENCODER2_GPIO_MASK);
#else /* WITHENCODER */
	return 0;
#endif /* WITHENCODER */
}

// ADC intgerface functions

#if WITHCPUADCHW


// Проверка что индекс входа АЦП относится ко встроенной периферии процессора
uint_fast8_t
isadchw(uint_fast8_t adci)
{
	return adci < BOARD_ADCX0BASE;
}

//#define ADCINPUTS_COUNT (board_get_adcinputs())

static uint_fast8_t adc_input;

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

void RAMFUNC_NONILINE ADC_Handler(void)
{
	//const unsigned long sr = ADC->ADC_ISR;	// ADC_IER_DRDY, ADC_ISR_DRDY
	(void) ADC->ADC_ISR;	// ADC_IER_DRDY, ADC_ISR_DRDY

	// ATSAM3Sxx считывает только 10 или 12 бит
	// Read the 8 most significant bits
	// of the AD conversion result
	board_adc_store_data(board_get_adcch(adc_input), ADC->ADC_LCDR & ADC_LCDR_LDATA_Msk);	// на этом цикле используем результат
	// Select next ADC input
	for (;;)
	{
		if (++ adc_input >= board_get_adcinputs())
		{
			spool_adcdonebundle();
			break;
		}
		else
		{
			const uint_fast8_t adci = board_get_adcch(adc_input);
			if (isadchw(adci))
			{
				// Select next ADC input (only one)
				const portholder_t mask = ADC_CHER_CH0 << adci;
				ADC->ADC_CHER = mask; /* enable ADC */
				ADC->ADC_CHDR = ~ mask; /* disable ADC */
				ADC->ADC_CR = ADC_CR_START;	// Start the AD conversion
				break;
			}
		}
	}
}
	
#elif CPUSTYLE_AT91SAM7S

RAMFUNC_NONILINE void AT91F_ADC_IRQHandler(void)
{
	(void) AT91C_BASE_ADC->ADC_SR;
	// Read the 8 most significant bits
	// of the AD conversion result
	board_adc_store_data(board_get_adcch(adc_input), AT91C_BASE_ADC->ADC_LCDR & AT91C_ADC_LDATA);	// на этом цикле используем результат
	// Select next ADC input
	for (;;)
	{
		if (++ adc_input >= board_get_adcinputs())
		{
			spool_adcdonebundle();
			break;
		}
		else
		{
			// Select next ADC input (only one)
			const uint_fast8_t adci = board_get_adcch(adc_input);
			if (isadchw(adci))
			{
				const portholder_t mask = AT91C_ADC_CH0 << adci;
				AT91C_BASE_ADC->ADC_CHDR = ~ mask; /* disable ADC inputs */
				AT91C_BASE_ADC->ADC_CHER = mask; /* enable ADC */
				AT91C_BASE_ADC->ADC_CR = AT91C_ADC_START;	// Start the AD conversion
				break;
			}
		}
	}
}

#elif CPUSTYLE_ATMEGA
	///////adc
	// получение кода выбора входа
	static uint_fast8_t hardware_atmega_admux(uint_fast8_t ch)
	{
		enum { ATMEGA_ADC_VREF_TYPE = ((0UL << REFS1) | (1UL << REFS0))	}; // AVCC used as reference volage
		#if HARDWARE_ADCBITS == 8
			return ch | ATMEGA_ADC_VREF_TYPE | (1UL << ADLAR);
		#else
			return ch | ATMEGA_ADC_VREF_TYPE;
		#endif
	}

	ISR(ADC_vect)
	{
		#if HARDWARE_ADCBITS == 8
			// Read the 8 most significant bits
			// of the AD conversion result
			board_adc_store_data(board_get_adcch(adc_input), ADCH);
		#else
			// Read the AD conversion result
			board_adc_store_data(board_get_adcch(adc_input), ADCW);
		#endif 
		// Select next ADC input
		for (;;)
		{
			if (++ adc_input >= board_get_adcinputs())
			{
				spool_adcdonebundle();
				break;
			}
			else
			{
				// Select next ADC input (only one)
				const uint_fast8_t adci = board_get_adcch(adc_input);
				if (isadchw(adci))
				{
					ADMUX = hardware_atmega_admux(adci);
					ADCSRA |= (1U << ADSC);			// Start the AD conversion
					break;
				}
			}
		}
	}
#elif CPUSTYLE_ATXMEGAXXXA4

	#warning TODO: write atxmega code for ADC interrupt handler


	// adc
	ISR(ADCA_CH0_vect)
	{
			// на этом цикле используем результат
		#if HARDWARE_ADCBITS == 8
			// Select next ADC input
			// Read the 8 most significant bits
			// of the AD conversion result
			board_adc_store_data(board_get_adcch(adc_input), ADCA.CH0.RESH);
		#else
			// Read the AD conversion result
			board_adc_store_data(board_get_adcch(adc_input), ADCA.CH0.RESH * 256 + ADCA.CH0.RESL);
		#endif 
		// Select next ADC input
		for (;;)
		{
			if (++ adc_input >= board_get_adcinputs())
			{
				spool_adcdonebundle();
				break;
			}
			else
			{
				// Select next ADC input (only one)
				const uint_fast8_t adci = board_get_adcch(adc_input);
				if (isadchw(adci))
				{
					ADCA.CH0.MUXCTRL = adci;
					ADCA.CH0.CTRL |= (1U << ADC_CH_START_bp);			// Start the AD conversion
					break;
				}
			}
		}
	}


#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

// For SM32H7XXX: ADC_IRQn is a same vector as ADC1_2_IRQn (decimal 18)


const adcinmap_t * getadcmap(uint_fast8_t adci)
{
	static const adcinmap_t adcinmaps [] =
	{
#if CPUSTYLE_STM32H7XX
		{	16,	ADC1,	ADC12_COMMON,	15,	},	// @0:	PA0	ADC1_INP16 (PA0_C ADC12_INP0)
		{	17,	ADC1,	ADC12_COMMON,	15,	},	// @1:	PA1	ADC1_INP17 (PA1_C ADC12_INP1)
		{	14,	ADC1,	ADC12_COMMON,	15,	},	// @2:	PA2	ADC12_INP14
		{	15,	ADC1,	ADC12_COMMON,	15,	},	// @3:	PA3	ADC12_INP15
		{	18,	ADC1,	ADC12_COMMON,	15,	},	// @4:	PA4	ADC12_INP18
		{	19,	ADC1,	ADC12_COMMON,	15,	},	// @5:	PA5	ADC12_INP19
		{	3,	ADC1,	ADC12_COMMON,	15,	},	// @6:	PA6	ADC12_INP3
		{	7,	ADC1,	ADC12_COMMON,	15,	},	// @7:	PA7	ADC12_INP7
		{	9,	ADC1,	ADC12_COMMON,	15,	},	// @8:	PB0	ADC12_INP9
		{	5,	ADC1,	ADC12_COMMON,	15,	},	// @9:	PB1	ADC12_INP5
		{	10,	ADC1,	ADC12_COMMON,	15,	},	// @10:	PC0	ADC123_INP10
		{	11,	ADC1,	ADC12_COMMON,	15,	},	// @11:	PC1	ADC123_INP11
		{	12,	ADC1,	ADC12_COMMON,	15,	},	// @12:	PC2	ADC123_INP12 (PC2_C ADC3_INP0)
		{	13,	ADC1,	ADC12_COMMON,	15,	},	// @13:	PC3	ADC12_INP13 (PC3_C ADC3_INP1)
		{	4,	ADC1,	ADC12_COMMON,	15,	},	// @14:	PC4	ADC12_INP4
		{	8,	ADC1,	ADC12_COMMON,	15,	},	// @15:	PC5	ADC12_INP8
		{	18,	ADC3,	ADC3_COMMON,	90,	},	// @16:	Temperature sensor (VSENSE) - 9.0 uS required
		{	19,	ADC3,	ADC3_COMMON,	43,	},	// @17:	Reference voltage (VREFINT) - 4.3 uS required
#elif CPUSTYLE_STM32MP1
		{	16,	ADC1,	ADC12_COMMON,	15,	},	// @0:	PA0	ADC1_INP16
		{	17,	ADC1,	ADC12_COMMON,	15,	},	// @1:	PA1	ADC1_INP17
		{	14,	ADC1,	ADC12_COMMON,	15,	},	// @2:	PA2	ADC1_INP14
		{	15,	ADC1,	ADC12_COMMON,	15,	},	// @3:	PA3	ADC1_INP15
		{	18,	ADC1,	ADC12_COMMON,	15,	},	// @4:	PA4	ADC1_INP18, ADC2_INP18
		{	19,	ADC1,	ADC12_COMMON,	15,	},	// @5:	PA5	ADC1_INP19, ADC2_INP19
		{	3,	ADC1,	ADC12_COMMON,	15,	},	// @6:	PA6	ADC1_INP3
		{	7,	ADC1,	ADC12_COMMON,	15,	},	// @7:	PA7	ADC1_INP7
		{	9,	ADC1,	ADC12_COMMON,	15,	},	// @8:	PB0	ADC1_INP9, ADC2_INP9
		{	5,	ADC1,	ADC12_COMMON,	15,	},	// @9:	PB1	ADC1_INP5, ADC2_INP5
		{	10,	ADC1,	ADC12_COMMON,	15,	},	// @10:	PC0	ADC1_INP10, ADC2_INP10
		{	11,	ADC1,	ADC12_COMMON,	15,	},	// @11:	PC1	ADC1_INP11, ADC2_INP11
		{	12,	ADC1,	ADC12_COMMON,	15,	},	// @12:	PC2	ADC1_INP12
		{	13,	ADC1,	ADC12_COMMON,	15,	},	// @13:	PC3	ADC1_INP13
		{	4,	ADC1,	ADC12_COMMON,	15,	},	// @14:	PC4	ADC1_INP4, ADC2_INP4
		{	8,	ADC1,	ADC12_COMMON,	15,	},	// @15:	PC5	ADC1_INP8, ADC2_INP8
		//{	18,	ADC3,	ADC3_COMMON,	90,	},	// @16:	Temperature sensor (VSENSE) - 9.0 uS required
		//{	19,	ADC3,	ADC3_COMMON,	43,	},	// @17:	Reference voltage (VREFINT) - 4.3 uS required
#endif /* CPUSTYLE_STM32H7XX, CPUSTYLE_STM32MP1 */
	};

	ASSERT(adci < (sizeof adcinmaps / sizeof adcinmaps [0]));
	return & adcinmaps [adci];
}

static void 
ADCs_IRQHandler(ADC_TypeDef * p)
{
	ASSERT(adc_input < board_get_adcinputs());
	const adcinmap_t * const adcmap = getadcmap(board_get_adcch(adc_input));
	ADC_TypeDef * const adc = adcmap->adc;
	ASSERT(adc == p);
	ASSERT((adc->ISR & ADC_ISR_OVR) == 0);
	ASSERT((adc->CR & (ADC_CR_JADSTART | ADC_CR_ADSTART)) == 0);
	if ((adc->ISR & ADC_ISR_EOS) != 0)
	{
		adc->ISR = ADC_ISR_EOS;		// EOS (end of regular sequence) flag
		board_adc_store_data(board_get_adcch(adc_input), (adc->DR & ADC_DR_RDATA) >> ADC_DR_RDATA_Pos);	// на этом цикле используем результат
		// Select next ADC input
		for (;;)
		{
			if (++ adc_input >= board_get_adcinputs())
			{
				spool_adcdonebundle();
				break;
			}
			else
			{
				// Select next ADC input (only one)
				const uint_fast8_t adci = board_get_adcch(adc_input);
				if (isadchw(adci))
				{
					// Установить следующий вход (блок ADC может измениться)
					const adcinmap_t * const adcmap = getadcmap(adci);
					ADC_TypeDef * const adc = adcmap->adc;

					ASSERT((adc->CR & (ADC_CR_JADSTART | ADC_CR_ADSTART)) == 0);
					adc->SQR1 = (adc->SQR1 & ~ (ADC_SQR1_L | ADC_SQR1_SQ1)) |
						0 * ADC_SQR1_L_0 |	// Выбираем преобразование с одного канала. Сканирования нет.
						adcmap->ch * ADC_SQR1_SQ1_0 |
						0;
					adc->CR |= ADC_CR_ADSTART;	// Запуск преобразования
					break;
				}
			}
		}
	}
	else
	{
		ASSERT(0);
	}
}

#if CPUSTYLE_STM32H7XX

	void
	ADC_IRQHandler(void)
	{
		ADCs_IRQHandler(ADC1);
	}

	void
	ADC3_IRQHandler(void)
	{
		ADCs_IRQHandler(ADC3);
	}

#elif CPUSTYLE_STM32MP1

	void
	ADC1_IRQHandler(void)
	{
		ADCs_IRQHandler(ADC1);
	}

	void
	ADC2_IRQHandler(void)
	{
		ADCs_IRQHandler(ADC2);
	}

#endif

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

static void
adcs_stm32f4xx_irq_handler(void)
{
	ASSERT(adc_input < board_get_adcinputs());
	//const unsigned long sr = ADC1->SR;
	ADC1->SR = 0;		// Сбрасываем все запросы прерывания.
	board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_DATA);	// на этом цикле используем результат
	// Select next ADC input
	for (;;)
	{
		if (++ adc_input >= board_get_adcinputs())
		{
			spool_adcdonebundle();
			break;
		}
		else
		{
			// Select next ADC input (only one)
			const uint_fast8_t adci = board_get_adcch(adc_input);
			if (isadchw(adci))
			{
				ADC1->SQR3 = (ADC1->SQR3 & ~ ADC_SQR3_SQ1) | (ADC_SQR3_SQ1_0 * adci);
				#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX
				ADC1->CR2 |= ADC_CR2_SWSTART;	// !!!!
				#endif
				break;
			}
		}
	}
}

void
ADC_IRQHandler(void)
{
	adcs_stm32f4xx_irq_handler();
}

void
ADC1_2_IRQHandler(void)
{
	adcs_stm32f4xx_irq_handler();
}



#elif CPUSTYLE_STM32F0XX
	#if STM32F0XX_MD
		void 
		ADC1_COMP_IRQHandler(void)
		{
			ASSERT(adc_input < board_get_adcinputs());
			board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_DATA);	// используем результат
			ADC1->ISR = ADC_ISR_EOC;
			ADC1->CHSELR = 1UL <<  board_get_adcch(adc_input);
			// Select next ADC input
			for (;;)
			{
				if (++ adc_input >= board_get_adcinputs())
				{
					spool_adcdonebundle();
					break;
				}
				else
				{
					// Select next ADC input (only one)
					const uint_fast8_t adci = board_get_adcch(adc_input);
					if (isadchw(adci))
					{
						ADC1->CHSELR = 1UL <<  board_get_adcch(adc_input);
						ADC1->CR = ADC_CR_ADSTART;	// ADC Start of Regular conversion
						break;
					}
				}
			}
		}
	#else /* STM32F0XX_MD */
		void 
		ADC1_IRQHandler(void)
		{
			ASSERT(adc_input < board_get_adcinputs());
			board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_DATA);	// используем результат
			ADC1->ISR = ADC_ISR_EOC;
			ADC1->CHSELR = 1UL <<  board_get_adcch(adc_input);
			// Select next ADC input
			for (;;)
			{
				if (++ adc_input >= board_get_adcinputs())
				{
					spool_adcdonebundle();
					break;
				}
				else
				{
					// Select next ADC input (only one)
					const uint_fast8_t adci = board_get_adcch(adc_input);
					if (isadchw(adci))
					{
						ADC1->CHSELR = 1UL << adci;
						ADC1->CR = ADC_CR_ADSTART;	// ADC Start of Regular conversion
						break;
					}
				}
			}
		}
	#endif /* STM32F0XX_MD */

#elif CPUSTYLE_STM32L0XX

	void 
	ADC1_COMP_IRQHandler(void)
	{
		ASSERT(adc_input < board_get_adcinputs());
		board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_DATA);	// используем результат
		ADC1->ISR = ADC_ISR_EOC;
		ADC1->CHSELR |= 1UL <<  board_get_adcch(adc_input);
		// Select next ADC input
		for (;;)
		{
			if (++ adc_input >= board_get_adcinputs())
			{
				spool_adcdonebundle();
				break;
			}
			else
			{
				// Select next ADC input (only one)
				const uint_fast8_t adci = board_get_adcch(adc_input);
				if (isadchw(adci))
				{
					ADC1->CHSELR |= 1UL << adci;
					ADC1->CR = ADC_CR_ADSTART;	// ADC Start of Regular conversion
					break;
				}
			}
		}
	}

#elif CPUSTYLE_STM32F30X

void 
ADC1_2_IRQHandler(void)
{
	ASSERT(adc_input < board_get_adcinputs());
	//const unsigned long sr = ADC1->SR;
	ADC1->ISR = 0;		// Сбрасываем все запросы прерывания.
	board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_RDATA);	// на этом цикле используем результат
	// Select next ADC input
	for (;;)
	{
		if (++ adc_input >= board_get_adcinputs())
		{
			spool_adcdonebundle();
			break;
		}
		else
		{
			// Select next ADC input (only one)
			const uint_fast8_t adci = board_get_adcch(adc_input);
			if (isadchw(adci))
			{
				ADC1->SQR1 = (ADC1->SQR1 & ~ ADC_SQR1_SQ1) | (ADC_SQR1_SQ1_0 * adci); 
				ADC1->CR |= ADC_CR_ADSTART;	// ADC Start of Regular conversion
				break;
			}
		}
	}
}

#elif CPUSTYLE_R7S721

void RAMFUNC_NONILINE
r7s721_adi_irq_handler(void)
{

	//dbg_putchar('.');
	//dbg_putchar('0' + adc_input);
	//dbg_putchar('0' + board_get_adcch(adc_input));

	//dbg_putchar(' ');
	/*
	static const volatile uint16_t * const adcports [HARDWARE_ADCINPUTS] =
	{
		& ADC.ADDRA,
		& ADC.ADDRB,
		& ADC.ADDRC,
		& ADC.ADDRD,
		& ADC.ADDRE,
		& ADC.ADDRF,
		& ADC.ADDRG,
		& ADC.ADDRH,
	};
	*/
	ASSERT(adc_input < board_get_adcinputs());
	//const unsigned long sr = ADC1->SR;
	////ADC1->ISR = 0;		// Сбрасываем все запросы прерывания.
	board_adc_store_data(board_get_adcch(adc_input), (& ADC.ADDRA) [board_get_adcch(adc_input)] >> 4);	// на этом цикле используем результат
	// Select next ADC input
	for (;;)
	{
		if (++ adc_input >= board_get_adcinputs())
		{
			ADC.ADCSR &= ~ ADC_SR_ADF;	// ADF: A/D end flag - Cleared by reading ADF while ADF = 1, then writing 0 to ADF
			// Это был последний вход
			spool_adcdonebundle();
			break;
		}
		else
		{
			// Select next ADC input (only one)
			const uint_fast8_t adci = board_get_adcch(adc_input);
			if (isadchw(adci))
			{
				ADC.ADCSR = (ADC.ADCSR & ~ (ADC_SR_ADF | ADC_SR_CH)) | 
					(adci << ADC_SR_CH_SHIFT) |	// канал для преобразования
					1 * ADC_SR_ADST |	// ADST: Single mode: A/D conversion starts
					0;
				break;
			}
		}
	}
}

#else
	#error No CPUSTYLE_XXXXX defined
#endif


// хотя бы один вход (s-метр) есть.
void
hardware_adc_startonescan(void)
{
	//ASSERT((adc_input == 0) || (adc_input == board_get_adcinputs()));	// проверяем, успело ли отработать ранее запущенное преобразование
	if ((adc_input != 0) && (adc_input < board_get_adcinputs()))
		return;	// не успели
	// Ищем первый АЦП из встроеных в процессор
	for (adc_input = 0; adc_input < board_get_adcinputs(); ++ adc_input)
	{
		const uint_fast8_t adci = board_get_adcch(adc_input);
		if (isadchw(adci))
			break;
	}
	if (adc_input >= board_get_adcinputs())
		return;

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// Select next ADC input (only one)
	const portholder_t mask = ADC_CHER_CH0 << board_get_adcch(adc_input);
	ADC->ADC_CHER = mask; /* enable ADC */
	ADC->ADC_CHDR = ~ mask; /* disable ADC */
	ADC->ADC_CR = ADC_CR_START;	// Start the AD conversion

#elif CPUSTYLE_AT91SAM7S

	// Select next ADC input (only one)
	const portholder_t mask = AT91C_ADC_CH0 << board_get_adcch(adc_input);
	AT91C_BASE_ADC->ADC_CHDR = ~ mask; /* disable ADC inputs */
	AT91C_BASE_ADC->ADC_CHER = mask; /* enable ADC */
	AT91C_BASE_ADC->ADC_CR = AT91C_ADC_START;	// Start the AD conversion

#elif CPUSTYLE_ATMEGA

	ADMUX = hardware_atmega_admux(board_get_adcch(adc_input));
	// Start the AD conversion
	ADCSRA |= (1U << ADSC);

#elif CPUSTYLE_ATXMEGAXXXA4

	#warning TODO: write atxmega code - ADC start
	ADCA.CH0.MUXCTRL = board_get_adcch(adc_input);
	// Start the AD conversion
	ADCA.CH0.CTRL |= (1U << ADC_CH_START_bp);			// Start the AD conversion

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	// Установить следующий вход (блок ADC может измениться)
	const adcinmap_t * const adcmap = getadcmap(board_get_adcch(adc_input));
	ADC_TypeDef * const adc = adcmap->adc;

	if ((adc->CR & ADC_CR_ADEN) == 0)
		return;
	if ((adc->CR & ADC_CR_ADSTART) != 0)
		return;	// еще не закончилось ранее запущеное преобразование

	ASSERT((adc->CR & ADC_CR_JADSTART) == 0);
	ASSERT((adc->CR & ADC_CR_ADSTART) == 0);
	ASSERT((adc->CR & (ADC_CR_JADSTART | ADC_CR_ADSTART)) == 0);

	adc->SQR1 = (adc->SQR1 & ~ (ADC_SQR1_L | ADC_SQR1_SQ1)) |
		0 * ADC_SQR1_L_0 |	//Выбираем преобразование с одного канала. Сканирования нет.
		adcmap->ch * ADC_SQR1_SQ1_0 |
		0;
	adc->CR |= ADC_CR_ADSTART;	// Запуск преобразования

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX
	//#warning TODO: to be implemented for CPUSTYLE_STM32F1XX

	// Установить следующий вход
	ADC1->SQR3 = (ADC1->SQR3 & ~ ADC_SQR3_SQ1) | (ADC_SQR3_SQ1_0 * board_get_adcch(adc_input));
	#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX
		ADC1->CR2 |= ADC_CR2_SWSTART;	// !!!!
	#endif

#elif CPUSTYLE_STM32F30X
	#warning TODO: Add code for STM32F30X support

	// Установить следующий вход
	ADC1->SQR1 = (ADC1->SQR1 & ~ ADC_SQR1_SQ1) | (ADC_SQR1_SQ1_0 * board_get_adcch(adc_input));
	ADC1->CR |= ADC_CR_ADSTART;	// ADC Start of Regular conversion

#elif CPUSTYLE_STM32L0XX

	ADC1->CHSELR = 1UL <<  board_get_adcch(adc_input);
	ADC1->CR = ADC_CR_ADSTART;	// ADC Start of Regular conversion

#elif CPUSTYLE_R7S721
	//#warning TODO: Add code for R7S721 ADC support
	// 27.4.1 Single Mode
	// Установить следующий вход
	ADC.ADCSR = (ADC.ADCSR & ~ (ADC_SR_CH)) |
		(board_get_adcch(adc_input) << ADC_SR_CH_SHIFT) |	// канал для преобразования
		1 * ADC_SR_ADST |	// ADST: Single mode: A/D conversion starts
		0;

#elif CPUSTYLE_STM32F0XX
	#warning: #warning Must be implemented for this CPU

#else

	#warning Undefined CPUSTYLE_XXX

#endif
}

#endif /* WITHCPUADCHW */


uint32_t hardware_get_random(void)
{
#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	#if defined(STM32F446xx)
		#warning RNG not exist
		return 0;
	#elif defined(STM32F401xC)
		#warning RNG not exist
		return 0;
	#else
		if ((RCC->AHB2ENR & RCC_AHB2ENR_RNGEN) == 0)
		{
			RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;	/* RNG clock enable */
			__DSB();
			RNG->CR |= RNG_CR_RNGEN;
		}

		while ((RNG->SR & RNG_SR_DRDY) == 0)
			;
		return RNG->DR;
	#endif

#elif CPUSTYLE_STM32L0XX && defined (RCC_AHBENR_RNGEN)

	if ((RCC->AHBENR & RCC_AHBENR_RNGEN) == 0)
	{
		RCC->AHBENR |= RCC_AHBENR_RNGEN;	/* RNG clock enable */
		__DSB();
		RNG->CR |= RNG_CR_RNGEN;
	}

	while ((RNG->SR & RNG_SR_DRDY) == 0)
		;
	return RNG->DR;

#else

	//#warning RNG not exist - hardware_get_random not work
	return 0;

#endif


}


#if CPUSTYLE_ARM || CPUSTYPE_TMS320F2833X

// количество циклов на микросекунду
static unsigned long
local_delay_uscycles(unsigned timeUS, unsigned cpufreq_MHz)
{
#if CPUSTYLE_AT91SAM7S
	#warning TODO: calibrate constant	 looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 175uL / cpufreq_MHz;
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_ATSAM3S
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 270uL / cpufreq_MHz;
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_ATSAM4S
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 270uL / cpufreq_MHz;
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_STM32F0XX
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 190uL / cpufreq_MHz;
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_STM32L0XX
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 20uL / cpufreq_MHz;
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_STM32F1XX
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 345uL / cpufreq_MHz;
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_STM32F30X
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 430uL / cpufreq_MHz;
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_STM32F4XX
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 3800uL / cpufreq_MHz;
#elif CPUSTYLE_STM32F7XX
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = 55uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_STM32H7XX
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = 77uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_R7S721
	const unsigned long top = 105uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_XC7Z
	const unsigned long top = 125uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYLE_STM32MP1
	// калибровано для 800 МГц процессора
	const unsigned long top = 120uL * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYPE_TMS320F2833X && 1 // RAM code0
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = timeUS * 760uL / cpufreq_MHz;	// tested @ 100 MHz Execute from RAM
	//const unsigned long top = 55 * cpufreq_MHz * timeUS / 1000;
#elif CPUSTYPE_TMS320F2833X	&& 0	// FLASH code
	#warning TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = 55uL * cpufreq_MHz * timeUS / 1000;
#else
	#error TODO: calibrate constant looks like CPUSTYLE_STM32MP1
	const unsigned long top = 55uL * cpufreq_MHz * timeUS / 1000;
#endif
	return top;
}
// Атрибут RAMFUNC_NONILINE убран, так как функция
// используется в инициализации SDRAM на процессорах STM32F746.
// TODO: перекалибровать для FLASH контроллеров.
void /* RAMFUNC_NONILINE */ local_delay_us(int timeUS)
{
	// Частота процессора приволится к мегагерцам.
	const unsigned long top = local_delay_uscycles(timeUS, CPU_FREQ / 1000000uL);
	//
	volatile unsigned long t;
	for (t = 0; t < top; ++ t)
	{
	}
}
// exactly as required
//
void local_delay_ms(int timeMS)
{
	// Частота процессора приволится к мегагерцам.
	const unsigned long top = local_delay_uscycles(1000, CPU_FREQ / 1000000uL);
	int n;
	for (n = 0; n < timeMS; ++ n)
	{
		volatile unsigned long t;
		for (t = 0; t < top; ++ t)
		{
		}
	}
}

#endif /* CPUSTYLE_ARM || CPUSTYPE_TMS320F2833X */


#if CPUSTYLE_STM32H7XX

// MPU initialize
static void lowlevel_stm32h7xx_mpu_initialize(void)
{
	/* Disables the MPU */
	MPU->CTRL = (MPU->CTRL & ~ (MPU_CTRL_ENABLE_Msk)) |
		0 * MPU_CTRL_ENABLE_Msk |
		0;


 
#define INNER_NORMAL_WB_RWA_TYPE(x)   (( 0x04 << MPU_RASR_TEX_Pos ) | ( DISABLE  << MPU_RASR_C_Pos ) | ( ENABLE  << MPU_RASR_B_Pos )  | ( x << MPU_RASR_S_Pos )) 
#define INNER_NORMAL_WB_NWA_TYPE(x)   (( 0x04 << MPU_RASR_TEX_Pos ) | ( ENABLE  << MPU_RASR_C_Pos )  | ( ENABLE  << MPU_RASR_B_Pos )  | ( x << MPU_RASR_S_Pos )) 
#define STRONGLY_ORDERED_SHAREABLE_TYPE      (( 0x00 << MPU_RASR_TEX_Pos ) | ( DISABLE << MPU_RASR_C_Pos ) | ( DISABLE << MPU_RASR_B_Pos ))     // DO not care // 
#define SHAREABLE_DEVICE_TYPE                (( 0x00 << MPU_RASR_TEX_Pos ) | ( DISABLE << MPU_RASR_C_Pos ) | ( ENABLE  << MPU_RASR_B_Pos ))     // DO not care // 
 

	// SRAM
	/* Set the Region base address and region number */
	MPU->RBAR = D1_AXISRAM_BASE | MPU_RBAR_VALID_Msk | 0x00;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x12 << MPU_RASR_SIZE_Pos) |	// Size 512 kB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	// ITCM
	/* Set the Region base address and region number */
	MPU->RBAR = D1_ITCMRAM_BASE | MPU_RBAR_VALID_Msk | 0x01;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x0F << MPU_RASR_SIZE_Pos) |	// Size 64 kB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	// DTCM
	/* Set the Region base address and region number */
	MPU->RBAR = D1_DTCMRAM_BASE | MPU_RBAR_VALID_Msk | 0x02;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x10 << MPU_RASR_SIZE_Pos) |	// Size 128 kB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	// FLASH
	/* Set the Region base address and region number */
	MPU->RBAR = D1_AXIFLASH_BASE | MPU_RBAR_VALID_Msk | 0x03;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x14 << MPU_RASR_SIZE_Pos) |	// Size 2 MB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	// DEVICE
	/* Set the Region base address and region number */
	MPU->RBAR = PERIPH_BASE | MPU_RBAR_VALID_Msk | 0x04;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x00 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x00 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x1B << MPU_RASR_SIZE_Pos) |	// Size 256 MB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	/* Enables the MPU */
	MPU->CTRL = (MPU->CTRL & ~ (MPU_CTRL_ENABLE_Msk)) |
		1 * MPU_CTRL_ENABLE_Msk |
		0;
}

#endif /* CPUSTYLE_STM32H7XX */

#if (__CORTEX_A != 0)

uint_fast8_t arm_hardware_cpuid(void)
{
	return __get_MPIDR() & 0x03;
}

//	MRC p15, 0, <Rt>, c6, c0, 2 ; Read IFAR into Rt
//	MCR p15, 0, <Rt>, c6, c0, 2 ; Write Rt to IFAR

/** \brief  Get IFAR
\return		Instruction Fault Address register value
*/
__STATIC_FORCEINLINE uint32_t __get_IFAR(void)
{
	uint32_t result;
	__get_CP(15, 0, result, 6, 0, 2);
	return result;
}

//	MRC p15, 0, <Rt>, c6, c0, 0 ; Read DFAR into Rt
//	MCR p15, 0, <Rt>, c6, c0, 0 ; Write Rt to DFAR

/** \brief  Get DFAR
\return		Data Fault Address register value
*/
__STATIC_FORCEINLINE uint32_t __get_DFAR(void)
{
	uint32_t result;
	__get_CP(15, 0, result, 6, 0, 0);
	return result;
}

void Undef_Handler(void)
{
	dbg_puts_impl_P(PSTR("UndefHandler trapped.\n"));
	dbg_puts_impl_P((__get_MPIDR() & 0x03) ? PSTR("CPUID=1\n") : PSTR("CPUID=0\n"));
	for (;;)
		;
}

void SWI_Handler(void)
{
	dbg_puts_impl_P(PSTR("SWIHandler trapped.\n"));
	dbg_puts_impl_P((__get_MPIDR() & 0x03) ? PSTR("CPUID=1\n") : PSTR("CPUID=0\n"));
	for (;;)
		;
}

// Prefetch Abort
void PAbort_Handler(void)
{
	const volatile uint32_t marker = 0xDEADBEEF;
	dbg_puts_impl_P(PSTR("PAbort_Handler trapped.\n"));
	dbg_puts_impl_P((__get_MPIDR() & 0x03) ? PSTR("CPUID=1\n") : PSTR("CPUID=0\n"));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
	PRINTF(PSTR("DFSR=%08lX, IFAR=%08lX, pc=%08lX, sp~%08lx\n"), __get_DFSR(), __get_IFAR(), (& marker) [2], (unsigned long) & marker);
#pragma GCC diagnostic pop
	const int WnR = (__get_DFSR() & (1uL << 11)) != 0;
	const int Status = (__get_DFSR() & (0x0FuL << 0));
	/*
		1. 0b000001 alignment fault
		2. 0b000100 instruction cache maintenance fault
		3. 0bx01100 1st level translation, synchronous external abort
		4. 0bx01110 2nd level translation, synchronous external abort
		5. 0b000101 translation fault, section
		6. 0b000111 translation fault, page
		7. 0b000011 access flag fault, section
		8. 0b000110 access flag fault, page
		9. 0b001001 domain fault, section
		10. 0b001011 domain fault, page
		11. 0b001101 permission fault, section
		12. 0b001111 permission fault, page
		13. 0bx01000 synchronous external abort, nontranslation
		14. 0bx10110 asynchronous external abort
		15. 0b000010 debug event.
	*/
	PRINTF(PSTR(" WnR=%d, Status=%02X\n"), (int) WnR, (unsigned) Status);
	switch (Status)
	{
	case 0x01: PRINTF(PSTR("alignment fault\n")); break;
	case 0x04: PRINTF(PSTR("instruction cache maintenance fault\n")); break;
	case 0x0C: PRINTF(PSTR("1st level translation, synchronous external abort\n")); break;
	case 0x0E: PRINTF(PSTR("2nd level translation, synchronous external abort\n")); break;
	case 0x05: PRINTF(PSTR("translation fault, section\n")); break;
	case 0x07: PRINTF(PSTR("translation fault, page\n")); break;
	case 0x03: PRINTF(PSTR("access flag fault, section\n")); break;
	case 0x06: PRINTF(PSTR("access flag fault, page\n")); break;
	case 0x09: PRINTF(PSTR("domain fault, section\n")); break;
	case 0x0B: PRINTF(PSTR("domain fault, page\n")); break;
	case 0x0D: PRINTF(PSTR("permission fault, section\n")); break;
	case 0x0F: PRINTF(PSTR("permission fault, page\n")); break;
	case 0x08: PRINTF(PSTR("synchronous external abort, nontranslation\n")); break;
	case 0x16: PRINTF(PSTR("asynchronous external abort\n")); break;
	case 0x02: PRINTF(PSTR("debug event.\n")); break;
	default: PRINTF(PSTR("undefined Status=%02X\n"), Status); break;
	}
//	unsigned i;
//	for (i = 0; i < 8; ++ i)
//	{
//		PRINTF("marker [%2d] = %08lX\n", i, (& marker) [i]);
//	}
	for (;;)
	{
#if defined (BOARD_BLINK_SETSTATE)
		BOARD_BLINK_SETSTATE(1);
		local_delay_ms(250);
		BOARD_BLINK_SETSTATE(0);
		local_delay_ms(250);
#endif /* defined (BOARD_BLINK_SETSTATE) */
	}
}

// Data Abort.
void DAbort_Handler(void)
{
	const volatile uint32_t marker = 0xDEADBEEF;
	dbg_puts_impl_P(PSTR("DAbort_Handler trapped.\n"));
	dbg_puts_impl_P((__get_MPIDR() & 0x03) ? PSTR("CPUID=1\n") : PSTR("CPUID=0\n"));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
	PRINTF(PSTR("DFSR=%08lX, DFAR=%08lX, pc=%08lX, sp~%08lx\n"), __get_DFSR(), __get_DFAR(), (& marker) [2], (unsigned long) & marker);
#pragma GCC diagnostic pop
	const int WnR = (__get_DFSR() & (1uL << 11)) != 0;
	const int Status = (__get_DFSR() & (0x0FuL << 0));
	/*
		1. 0b000001 alignment fault
		2. 0b000100 instruction cache maintenance fault
		3. 0bx01100 1st level translation, synchronous external abort
		4. 0bx01110 2nd level translation, synchronous external abort
		5. 0b000101 translation fault, section
		6. 0b000111 translation fault, page
		7. 0b000011 access flag fault, section
		8. 0b000110 access flag fault, page
		9. 0b001001 domain fault, section
		10. 0b001011 domain fault, page
		11. 0b001101 permission fault, section
		12. 0b001111 permission fault, page
		13. 0bx01000 synchronous external abort, nontranslation
		14. 0bx10110 asynchronous external abort
		15. 0b000010 debug event.
	*/
	PRINTF(PSTR(" WnR=%d, Status=%02X\n"), (int) WnR, (unsigned) Status);
	switch (Status)
	{
	case 0x01: PRINTF(PSTR("alignment fault\n")); break;
	case 0x04: PRINTF(PSTR("instruction cache maintenance fault\n")); break;
	case 0x0C: PRINTF(PSTR("1st level translation, synchronous external abort\n")); break;
	case 0x0E: PRINTF(PSTR("2nd level translation, synchronous external abort\n")); break;
	case 0x05: PRINTF(PSTR("translation fault, section\n")); break;
	case 0x07: PRINTF(PSTR("translation fault, page\n")); break;
	case 0x03: PRINTF(PSTR("access flag fault, section\n")); break;
	case 0x06: PRINTF(PSTR("access flag fault, page\n")); break;
	case 0x09: PRINTF(PSTR("domain fault, section\n")); break;
	case 0x0B: PRINTF(PSTR("domain fault, page\n")); break;
	case 0x0D: PRINTF(PSTR("permission fault, section\n")); break;
	case 0x0F: PRINTF(PSTR("permission fault, page\n")); break;
	case 0x08: PRINTF(PSTR("synchronous external abort, nontranslation\n")); break;
	case 0x16: PRINTF(PSTR("asynchronous external abort\n")); break;
	case 0x02: PRINTF(PSTR("debug event.\n")); break;
	default: PRINTF(PSTR("undefined Status=%02X\n"), Status); break;
	}
	unsigned i;
	for (i = 0; i < 8; ++ i)
	{
		PRINTF("marker [%2d] = %08lX\n", i, (& marker) [i]);
	}
	for (;;)
	{
#if defined (BOARD_BLINK_SETSTATE)
		BOARD_BLINK_SETSTATE(1);
		local_delay_ms(1250);
		BOARD_BLINK_SETSTATE(0);
		local_delay_ms(1250);
#endif /* defined (BOARD_BLINK_SETSTATE) */
	}
}

void FIQ_Handler(void)
{
	dbg_puts_impl_P(PSTR("FIQHandler trapped.\n"));
	dbg_puts_impl_P((__get_MPIDR() & 0x03) ? PSTR("CPUID=1\n") : PSTR("CPUID=0\n"));
	for (;;)
		;
}

void Hyp_Handler(void)
{
	dbg_puts_impl_P(PSTR("Hyp_Handler trapped.\n"));
	dbg_puts_impl_P((__get_MPIDR() & 0x03) ? PSTR("CPUID=1\n") : PSTR("CPUID=0\n"));
	for (;;)
		;
}

#endif /* (__CORTEX_A != 0) */

#if CPUSTYLE_ARM_CM7

// Сейчас в эту память будем читать по DMA
// Убрать копию этой области из кэша
// Используется только в startup
void arm_hardware_invalidate(uintptr_t base, int_fast32_t dsize)
{
	//ASSERT((base % 32) == 0);		// при работе с BACKUP SRAM невыровненно
	SCB_InvalidateDCache_by_Addr((void *) base, dsize);	// DCIMVAC register used.
}

// Сейчас эта память будет записываться по DMA куда-то
// Записать содержимое кэша данных в память
void arm_hardware_flush(uintptr_t base, int_fast32_t dsize)
{
	//ASSERT((base % 32) == 0);		// при работе с BACKUP SRAM невыровненно
	SCB_CleanDCache_by_Addr((void *) base, dsize);	// DCCMVAC register used.
}

// Записать содержимое кэша данных в память
// применяетмся после начальной инициализации среды выполнния
void arm_hardware_flush_all(void)
{
	SCB_CleanDCache();	// DCCMVAC register used.
}

// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
// Записать содержимое кэша данных в память
// Убрать копию этой области из кэша
void arm_hardware_flush_invalidate(uintptr_t base, int_fast32_t dsize)
{
	//ASSERT((base % 32) == 0);		// при работе с BACKUP SRAM невыровненно
	SCB_CleanInvalidateDCache_by_Addr((void *) base, dsize);	// DCCIMVAC register used.
}

#elif (__CORTEX_A != 0)

/* считать конфигурационные параметры data cache */
static void ca9_ca7_cache_diag(void)
{
	PRINTF("ca9_ca7_cache_diag: CLIDR=%08lX\n", (unsigned long) __get_CLIDR());
	uint32_t ccsidr0 [8];	// data cache parameters
	uint32_t ccsidr1 [8];	// instruction cache parameters

	uint_fast32_t leveli;
	for (leveli = 0; leveli <= ARM_CA9_CACHELEVELMAX; ++ leveli)
	{
		__set_CSSELR(leveli * 2 + 0);	// data cache select
		ccsidr0 [leveli] = __get_CCSIDR();

		//const uint32_t assoc0 = (ccsidr0 >> 3) & 0x3FF;
		//const int passoc0 = countbits2(assoc0);
		//const uint32_t maxsets0 = (ccsidr0 >> 13) & 0x7FFF;

		__set_CSSELR(leveli * 2 + 1);	// instruction cache select
		ccsidr1 [leveli] = __get_CCSIDR();

		//const uint32_t assoc1 = (ccsidr1 >> 3) & 0x3FF;
		//const int passoc1 = countbits2(assoc1);
		//const uint32_t maxsets1 = (ccsidr1 >> 13) & 0x7FFF;

		// Установка размера строки кэша
		unsigned long xDCACHEROWSIZE = 4uL << (((ccsidr0 [leveli] >> 0) & 0x07) + 2);
		unsigned long xICACHEROWSIZE = 4uL << (((ccsidr1 [leveli] >> 0) & 0x07) + 2);

		PRINTF("ca9_ca7_cache_diag: DCACHE[%d] ROWSIZE=%d, ICACHE[%d] ROWSIZE=%d\n", leveli, (int) xDCACHEROWSIZE, leveli, (int) xICACHEROWSIZE);
	}
}

// Записать содержимое кэша данных в память
// применяетмся после начальной инициализации среды выполнния
void arm_hardware_flush_all(void)
{
	L1C_CleanDCacheAll();
#if (__L2C_PRESENT == 1)
	L2C_InvAllByWay();
#endif
}

#define MK_MVA(addr) ((uintptr_t) (addr) & ~ (uintptr_t) (DCACHEROWSIZE - 1))

// Сейчас в эту память будем читать по DMA
// Используется только в startup
void arm_hardware_invalidate(uintptr_t addr, int_fast32_t dsize)
{
	ASSERT((addr % DCACHEROWSIZE) == 0);
	//ASSERT((dsize % DCACHEROWSIZE) == 0);

	if (dsize > 0)
	{
		int32_t op_size = dsize + (((uint32_t) addr) & (DCACHEROWSIZE - 1U));
		uint32_t op_addr = (uint32_t) addr /* & ~(DCACHEROWSIZE - 1U) */;

		__DSB();

		do
		{
			const uintptr_t mva = MK_MVA(op_addr);	/* register accepts only 32byte aligned values, only bits 31..5 are valid */
			L1C_InvalidateDCacheMVA((void *) mva);	// очистить кэш
		#if (__L2C_PRESENT == 1)
			// Clean cache by physical address
			L2C_InvPa((void *) mva);
		#endif
			op_addr += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);

		__DSB();
		__ISB();
	}
}

// Сейчас эта память будет записываться по DMA куда-то
void arm_hardware_flush(uintptr_t addr, int_fast32_t dsize)
{
	ASSERT((addr % DCACHEROWSIZE) == 0);
	//ASSERT((dsize % DCACHEROWSIZE) == 0);

	if (dsize > 0)
	{
		int32_t op_size = dsize + (((uint32_t) addr) & (DCACHEROWSIZE - 1U));
		uint32_t op_addr = (uint32_t) addr /* & ~(DCACHEROWSIZE - 1U) */;

		__DSB();

		do
		{
			const uintptr_t mva = MK_MVA(op_addr);	/* register accepts only 32byte aligned values, only bits 31..5 are valid */
			L1C_CleanDCacheMVA((void *) mva);		// записать буфер, кэш продолжает хранить
		#if (__L2C_PRESENT == 1)
			// предполагается, что размер строки L2 и L2 cache равны
			// Clean cache by physical address
			L2C_CleanPa((void *) mva);
		#endif
			op_addr += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);

		__DSB();
		__ISB();
	}
}

// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
void arm_hardware_flush_invalidate(uintptr_t addr, int_fast32_t dsize)
{
	ASSERT((addr % DCACHEROWSIZE) == 0);
	//ASSERT((dsize % DCACHEROWSIZE) == 0);

	if (dsize > 0)
	{
		int32_t op_size = dsize + (((uint32_t) addr) & (DCACHEROWSIZE - 1U));
		uint32_t op_addr = (uint32_t) addr /* & ~(DCACHEROWSIZE - 1U) */;

		__DSB();

		do
		{
			const uintptr_t mva = MK_MVA(op_addr);	/* register accepts only 32byte aligned values, only bits 31..5 are valid */
			L1C_CleanInvalidateDCacheMVA((void *) mva);	// записать буфер, очистить кэш
		#if (__L2C_PRESENT == 1)
			// предполагается, что размер строки L2 и L2 cache равны
			// Clean cache by physical address
			L2C_CleanInvPa((void *) mva);
		#endif
			op_addr += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);

		__DSB();
		__ISB();
	}
}

#else

// Заглушки
// Сейчас в эту память будем читать по DMA
// Используется только в startup
void arm_hardware_invalidate(uintptr_t base, int_fast32_t dsize)
{
}

// Сейчас эта память будет записываться по DMA куда-то
void arm_hardware_flush(uintptr_t base, int_fast32_t dsize)
{
}

// Записать содержимое кэша данных в память
// применяетмся после начальной инициализации среды выполнния
void arm_hardware_flush_all(void)
{
}

// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
void arm_hardware_flush_invalidate(uintptr_t base, int_fast32_t dsize)
{
}

#endif /* CPUSTYLE_ARM_CM7 */


// получение из аппаратного счетчика монотонно увеличивающегося кода
// see SystemInit() in hardware.c
uint_fast32_t cpu_getdebugticks(void)
{
#if CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7
	return DWT->CYCCNT;	// use TIMESTAMP_GET();
#elif (__CORTEX_A != 0)
	{
		uint32_t result;
		// Read CCNT Register
		//	MRC p15, 0, <Rt>, c9, c13, 0 : Read PMCCNTR into Rt
		//	MCR p15, 0, <Rt>, c9, c13, 0 : Write Rt to PMCCNTR
		//asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(value));  
		__get_CP(15, 0, result, 9, 13, 0);
		return(result);
	}
#else
	//#warning Wromg CPUSTYLE_xxx - cpu_getdebugticks not work
	return 0;
#endif
}


#if 0

typedef struct irqlog_tag
{
	IRQn_ID_t irqn;
	int pos;	// in/out
} irqlog_t;
enum { IRQLOG_LEN = 1024 };

static volatile unsigned irqlog_enabled;
static volatile unsigned irqlog_count;
static irqlog_t irqlogs [IRQLOG_LEN];

void irqlog_start(void)
{
	irqlog_enabled = 0;
	irqlog_count = 0;
	irqlog_enabled = 1;
}

void irqlog_stop(void)
{
	irqlog_enabled = 0;
}

void irqlog_record(int pos, IRQn_ID_t irqn)
{
	if (irqlog_enabled == 0)
		return;

	if (irqlog_count >= IRQLOG_LEN)
		return;

	irqlog_t * const p = & irqlogs [irqlog_count ++];
	p->pos = pos;
	p->irqn = irqn;
}

void irqlog_print(void)
{
	PRINTF("irqlog_count=%u\n", irqlog_count);
	unsigned i;
	for (i = 0; i < irqlog_count; ++ i)
	{
		const irqlog_t * const p = & irqlogs [i];
		PRINTF(" pos=%d, IRQ=%3u (0x%03X)\n", (int) p->pos, (unsigned) p->irqn, (unsigned) p->irqn);
	}
}
#endif

#if (__CORTEX_A != 0)

#include "hardware.h"
#include "formats.h"
//#include "hardware_r7s721.h"

//#define INTC_LEVEL_SENSITIVE    (0)     /* Level sense  */
//#define INTC_EDGE_TRIGGER       (1)     /* Edge trigger */

/* ==== Interrupt detection ==== */

#if defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U)

/* Вызывается из crt_r7s721.s со сброшенным флагом прерываний */
void IRQ_Handler_GICv1(void)
{
	//dbg_putchar('/');
	const IRQn_ID_t irqn = IRQ_GetActiveIRQ();
	//irqlog_record(1, irqn);
	//static const char hex [16] = "0123456789ABCDEF";
	//dbg_putchar(hex [(irqn >> 8) & 0x0F]);
	//dbg_putchar(hex [(irqn >> 4) & 0x0F]);
	//dbg_putchar(hex [(irqn >> 0) & 0x0F]);
	////ASSERT(irqn != 0x3FC && irqn != 0x3FD);
	IRQHandler_t const handler = IRQ_GetHandler(irqn);

#if 0
	switch (irqn)
	{
	//case PL310ERR_IRQn:
	//	break;
	default:
		PRINTF(PSTR("IRQ_Handler_GICv1: irq=%d, handler=%p\n"), (int) irqn, (void *) handler);
		break;
	}
#endif
	if (handler != NULL)
	{
#if WITHNESTEDINTERRUPTS

		__enable_irq();						/* modify I bit in CPSR */
		(* handler)();	    /* Call interrupt handler */
		__disable_irq();					/* modify I bit in CPSR */

#else /* WITHNESTEDINTERRUPTS */

		(* handler)();	    /* Call interrupt handler */

#endif /* WITHNESTEDINTERRUPTS */
	}
	//irqlog_record(2, irqn);
	//dbg_putchar('\\');
	IRQ_EndOfInterrupt(irqn);
}

#define INT_ID_MASK		0x3ffuL
/* Interrupt IDs reported by the HPPIR and IAR registers */
#define PENDING_G1_INTID	1022uL
/* Constant to indicate a spurious interrupt in all GIC versions */
#define GIC_SPURIOUS_INTERRUPT		1023uL
/*
 * Constant passed to the interrupt handler in the 'id' field when the
 * framework does not read the gic registers to determine the interrupt id.
 */
#define INTR_ID_UNAVAILABLE		0xFFFFFFFFuL

/*******************************************************************************
 * This function returns the id of the highest priority pending interrupt at
 * the GIC cpu interface. GIC_SPURIOUS_INTERRUPT is returned when there is no
 * interrupt pending.
 ******************************************************************************/
unsigned int gicv2_get_pending_interrupt_id(void)
{
	unsigned int id;

	id = GIC_GetHighPendingIRQ() & INT_ID_MASK;	// HIPPR

	/*
	 * Find out which non-secure interrupt it is under the assumption that
	 * the GICC_CTLR.AckCtl bit is 0.
	 */
	if (id == PENDING_G1_INTID)
		id = GICInterface->AHPPIR & INT_ID_MASK;

	return id;
}

static RAMDTCM SPINLOCK_t giclock = SPINLOCK_INIT;

/* Вызывается из crt_stm32mp1.s со сброшенным флагом прерываний */
// Sww ARM IHI 0048B.b document
void IRQ_Handler_GICv2(void)
{
	// per-cpu:
	// GICC_AHPPIR
	// GICC_HPPIR
	// GICC_IAR
	// GICC_EOIR
	// GICC_BPR
	// GICC_PMR
	//
	// global:
	// GICD_IPRIORITYR

	//dbg_putchar('1');
	//const uint_fast32_t gicc_hppir = gicv2_get_pending_interrupt_id(); //GICInterface->HPPIR; //GIC_GetHighPendingIRQ();	/* GICC_HPPIR */
	const uint_fast32_t gicc_iar = GICInterface->IAR; // CPUID, Interrupt ID
	const IRQn_ID_t int_id = gicc_iar & INT_ID_MASK;

	// IHI0048B_b_gic_architecture_specification.pdf
	// See ARM IHI 0048B.b 3.4.2 Special interrupt numbers when a GIC supports interrupt grouping

	if (int_id == 1022)
	{
	}

	if (int_id >= 1020)
	{
		//dbg_putchar('2');
//		SPIN_LOCK(& giclock);
//		GIC_SetPriority(0, GIC_GetPriority(0));	// GICD_IPRIORITYRn(0) = GICD_IPRIORITYRn(0);
//		SPIN_UNLOCK(& giclock);

	}
	else if (int_id != 0 /* || GIC_GetIRQStatus(0) != 0 */)
	{
		const IRQHandler_t f = IRQ_GetHandler(int_id);

	#if WITHNESTEDINTERRUPTS

		if (f != (IRQHandler_t) 0)
		{
//			static const char hex [16] = "0123456789ABCDEF";
//			if ((int_id >> 8) & 0x0F)
//				dbg_putchar(hex [(int_id >> 8) & 0x0F]);
//			dbg_putchar(hex [(int_id >> 4) & 0x0F]);
//			dbg_putchar(hex [(int_id >> 0) & 0x0F]);
			__enable_irq();						/* modify I bit in CPSR */
			(* f)();	    /* Call interrupt handler */
			__disable_irq();					/* modify I bit in CPSR */
			//dbg_putchar('_');
		}

	#else /* WITHNESTEDINTERRUPTS */

		if (f != (IRQHandler_t) 0)
		{
			(* f)();	    /* Call interrupt handler */
		}

	#endif /* WITHNESTEDINTERRUPTS */

		//dbg_putchar('5');
	}
	else
	{
//		SPIN_LOCK(& giclock);
//		GIC_SetPriority(0, GIC_GetPriority(0));	// GICD_IPRIORITYRn(0) = GICD_IPRIORITYRn(0);
//		SPIN_UNLOCK(& giclock);
	}
	//dbg_putchar(' ');

	//GIC_EndInterrupt(gicc_iar);	/* CPUID, EOINTID */
	GICInterface->EOIR = gicc_iar;
}


#endif /* defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U) */

#if 0
/* Вызывается из crt_r7s721.s со сброшенным флагом прерываний */
void IRQ_HandlerOld(void)
{
	/* const uint32_t icchpir = */ (void) GICC_HPPIR;	/* GICC_HPPIR */
	const uint32_t icciar = GICC_IAR;				/* GICC_IAR */
	const IRQn_ID_t int_id = icciar & INTC_ICCIAR_ACKINTID;

	// See R01UH0437EJ0200 Rev.2.00 7.8.3 Reading Interrupt ID Values from Interrupt Acknowledge Register (ICCIAR)
	// IHI0048B_b_gic_architecture_specification.pdf
	// See ARM IHI 0048B.b 3.4.2 Special interrupt numbers when a GIC supports interrupt grouping

	if (int_id >= 1020)
	{
		GICD_IPRIORITYRn(0) = GICD_IPRIORITYRn(0);
	}
	else if (int_id != 0 || (INTC.ICDABR0 & 0x0001) != 0)
	{
		void (* f)(void) = IRQ_GetHandler(int_id);	    /* Call interrupt handler */

	#if WITHNESTEDINTERRUPTS

		__enable_irq();						/* modify I bit in CPSR */
		(* f)();	    /* Call interrupt handler */
		__disable_irq();					/* modify I bit in CPSR */

	#else /* WITHNESTEDINTERRUPTS */

		(* f)();	    /* Call interrupt handler */

	#endif /* WITHNESTEDINTERRUPTS */
		INTC.ICCEOIR = int_id;				/* GICC_EOIR */
	}
	else
	{
		GICD_IPRIORITYRn(0) = GICD_IPRIORITYRn(0);
	}
}

/******************************************************************************
* Function Name: r7s721_intc_initialize
* Description  : Executes initial setting for the INTC.
*              : The interrupt mask level is set to 31 to receive interrupts
*              : with the interrupt priority level 0 to 30.
* Arguments    : none
* Return Value : none
******************************************************************************/
static void r7s721_intc_initializeOld(void)
{

	/* ==== Total number of registers ==== */
	enum { INTC_ICDISR_REG_TOTAL   = (IRQ_GIC_LINE_COUNT + 31) / 32 };	// 19 == INTC_ICDISR0_COUNT
	enum { INTC_ICDICFR_REG_TOTAL  = (IRQ_GIC_LINE_COUNT + 15) / 16 };	// 37 == INTC_ICDICFR0_COUNT
	enum { INTC_ICDIPR_REG_TOTAL   = (IRQ_GIC_LINE_COUNT + 3) /  4 };	// 147 == INTC_ICDIPR0_COUNT
	enum { INTC_ICDIPTR_REG_TOTAL  = (IRQ_GIC_LINE_COUNT + 3) /  4 };	// 147 == INTC_ICDIPTR0_COUNT
	//enum { INTC_ICDISER_REG_TOTAL  = (IRQ_GIC_LINE_COUNT + 31) / 32 };	// 19 == INTC_ICDISER0_COUNT
	enum { INTC_ICDICER_REG_TOTAL  = (IRQ_GIC_LINE_COUNT + 31) / 32 };	// 19 == INTC_ICDICER0_COUNT

	/* Initial value table of Interrupt Configuration Registers */
	// Table 4-19 GICD_ICFGR Int_config[0] encoding in some early GIC implementations
	// каждая пара бит кодирует:

	// [0] == 0: Corresponding interrupt is handled using the N-N model.
	// [0] == 1: Corresponding interrupt is handled using the 1-N model.
	// [1] == 0: Corresponding interrupt is level-sensitive.
	// [1] == 1: Corresponding interrupt is edge-triggered.
	static const uint32_t intc_icdicfrn_table [INTC_ICDICFR_REG_TOTAL] =
	{                          /*           Interrupt ID */
		0xAAAAAAAA,            /* ICDICFR0  :  15 to   0 */
		0x00000055,            /* ICDICFR1  :  19 to  16 */
		0xFFFD5555,            /* ICDICFR2  :  47 to  32 */
		0x555FFFFF,            /* ICDICFR3  :  63 to  48 */
		0x55555555,            /* ICDICFR4  :  79 to  64 */
		0x55555555,            /* ICDICFR5  :  95 to  80 */
		0x55555555,            /* ICDICFR6  : 111 to  96 */
		0x55555555,            /* ICDICFR7  : 127 to 112 */
		0x5555F555,            /* ICDICFR8  : 143 to 128 */
		0x55555555,            /* ICDICFR9  : 159 to 144 */
		0x55555555,            /* ICDICFR10 : 175 to 160 */
		0xF5555555,            /* ICDICFR11 : 191 to 176 */
		0xF555F555,            /* ICDICFR12 : 207 to 192 */
		0x5555F555,            /* ICDICFR13 : 223 to 208 */
		0x55555555,            /* ICDICFR14 : 239 to 224 */
		0x55555555,            /* ICDICFR15 : 255 to 240 */
		0x55555555,            /* ICDICFR16 : 271 to 256 */
		0xFD555555,            /* ICDICFR17 : 287 to 272 */
		0x55555557,            /* ICDICFR18 : 303 to 288 */
		0x55555555,            /* ICDICFR19 : 319 to 304 */
		0x55555555,            /* ICDICFR20 : 335 to 320 */
		0x5F555555,            /* ICDICFR21 : 351 to 336 */
		0xFD55555F,            /* ICDICFR22 : 367 to 352 */
		0x55555557,            /* ICDICFR23 : 383 to 368 */
		0x55555555,            /* ICDICFR24 : 399 to 384 */
		0x55555555,            /* ICDICFR25 : 415 to 400 */
		0x55555555,            /* ICDICFR26 : 431 to 416 */
		0x55555555,            /* ICDICFR27 : 447 to 432 */
		0x55555555,            /* ICDICFR28 : 463 to 448 */
		0x55555555,            /* ICDICFR29 : 479 to 464 */
		0x55555555,            /* ICDICFR30 : 495 to 480 */
		0x55555555,            /* ICDICFR31 : 511 to 496 */
		0x55555555,            /* ICDICFR32 : 527 to 512 */
		0x55555555,            /* ICDICFR33 : 543 to 528 */
		0x55555555,            /* ICDICFR34 : 559 to 544 */
		0x55555555,            /* ICDICFR35 : 575 to 560 */
		0x00155555             /* ICDICFR36 : 586 to 576 */
	};

    uint16_t offset;
    volatile uint32_t * addr;

	//GIC_Enable();	// инициализирует не совсем так как надо для работы

	/* default interrut handlers setup */
    for (offset = 0; offset < IRQ_GIC_LINE_COUNT; ++ offset)
    {
        //intc_func_table [offset] = Userdef_INTC_Dummy_Interrupt;    /* Set all interrupts default handlers */
    }

    /* ==== Initial setting 1 to receive GIC interrupt request ==== */
    /* Interrupt Security Registers setting */
    addr = (volatile uint32_t *) & INTC.ICDISR0;
    for (offset = 0; offset < INTC_ICDISR_REG_TOTAL; ++ offset)
    {
        * (addr + offset) = 0x00000000uL;    /* Set all interrupts to be secured */
    }

    /* Interrupt Configuration Registers setting */
    addr = (volatile uint32_t *) & INTC.ICDICFR0;
    for (offset = 0; offset < INTC_ICDICFR_REG_TOTAL; ++ offset)
    {
        * (addr + offset) = intc_icdicfrn_table [offset];
    }

    /* Interrupt Priority Registers setting */
    addr = (volatile uint32_t *) & GICD_IPRIORITYRn(0);
    for (offset = 0; offset < INTC_ICDIPR_REG_TOTAL; ++ offset)
    {
        /* Set the priority for all interrupts to 31 */
        * (addr + offset) = 31 * 0x01010101uL;
    }

    /* Interrupt Processor Targets Registers setting */
    /* Initialise ICDIPTR8 to ICDIPTRn                     */
    /* (n = The number of interrupt sources / 4)           */
    /*   - ICDIPTR0 to ICDIPTR4 are dedicated for main CPU */
    /*   - ICDIPTR5 is dedicated for sub CPU               */
    /*   - ICDIPTR6 to 7 are reserved                      */
    addr = (volatile uint32_t *) & INTC.ICDIPTR0;
    for (offset = 8; offset < INTC_ICDIPTR_REG_TOTAL; ++ offset)
    {
    	/* Set the target for all interrupts to main CPU */
        * (addr + offset) = 0x01010101uL;
    }

    /* Interrupt Clear-Enable Registers setting */
    addr = (volatile uint32_t *) & INTC.ICDICER0;
    for (offset = 0; offset < INTC_ICDICER_REG_TOTAL; ++ offset)
    {
    	 /* Set all interrupts to be disabled */
    	* (addr + offset) = 0xFFFFFFFFuL;
    }
}

#endif

uint8_t __attribute__ ((section(".stack"), used, aligned(64))) mystack [2048];
/******************************************************************************/

// TTB initialize

// SAM9XE512 bits
//#define TLB_NCNB 0x0DF2 // Noncachable, Nonbufferable 11 0 1111 1 00 10
//#define TLB_WT 0x0DFA // Write-through 11 0 1111 1 10 10
//#define TLB_WB 0x0DFE // Write-back 11 0 1111 1 11 10
//#define TLB_INVALID 0x0000 // Write-back 11 0 1111 1 11 10


/*

; ---- Parameter setting to level1 descriptor (bits 19:0) ----
; setting for Strongly-ordered memory
TTB_PARA_STRGLY             EQU     2_00000000000000000000110111100010
; setting for Outer and inner not cache normal memory
TTB_PARA_NORMAL_NOT_CACHE   EQU     2_00000000000000000001110111100010
; setting for Outer and inner write back, write allocate normal memory (Cacheable)
TTB_PARA_NORMAL_CACHE       EQU     2_00000000000000000001110111101110

; ---- Memory area size (MB) ----
M_SIZE_NOR      EQU     128             ; [Area00] CS0, CS1 area (for NOR flash)
M_SIZE_SDRAM    EQU     128             ; [Area01] CS2, CS3 area (for SDRAM)
M_SIZE_CS45     EQU     128             ; [Area02] CS4, CS5 area
M_SIZE_SPI      EQU     128             ; [Area03] SPI, SP2 area (for Serial flash)
M_SIZE_RAM      EQU     10              ; [Area04] Internal RAM
M_SIZE_IO_1     EQU     502             ; [Area05] I/O area 1
M_SIZE_NOR_M    EQU     128             ; [Area06] CS0, CS1 area (for NOR flash) (mirror)
M_SIZE_SDRAM_M  EQU     128             ; [Area07] CS2, CS3 area (for SDRAM) (mirror)
M_SIZE_CS45_M   EQU     128             ; [Area08] CS4, CS5 area (mirror)
M_SIZE_SPI_M    EQU     128             ; [Area09] SPI, SP2 area (for Serial flash) (mirror)
M_SIZE_RAM_M    EQU     10              ; [Area10] Internal RAM (mirror)
M_SIZE_IO_2     EQU     2550            ; [Area11] I/O area 2
;===================================================================
; Cortex-A9 MMU Configuration
; Set translation table base
;===================================================================
    ;;; Cortex-A9 supports two translation tables
    ;;; Configure translation table base (TTB) control register cp15,c2
    ;;; to a value of all zeros, indicates we are using TTB register 0.
    MOV  r0,#0x0
    MCR  p15, 0, r0, c2, c0, 2      ;;; TTBCR

    ;;; write the address of our page table base to TTB register 0
    LDR  r0,=||Image$$TTB$$ZI$$Base||
    MOV  r1, #0x08                  ;;; RGN=b01  (outer cacheable write-back cached, write allocate)
                                    ;;; S=0      (translation table walk to non-shared memory)
    ORR  r1,r1,#0x40                ;;; IRGN=b01 (inner cacheability for the translation table walk is Write-back Write-allocate)
    ORR  r0,r0,r1
    MCR  p15, 0, r0, c2, c0, 0      ;;; TTBR0

;===================================================================
; PAGE TABLE generation
; Generate the page tables
; Build a flat translation table for the whole address space.
; ie: Create 4096 1MB sections from 0x000xxxxx to 0xFFFxxxxx
; 31                 20 19  18  17  16 15  14   12 11 10  9  8     5   4    3 2   1 0
; |section base address| 0  0  |nG| S |AP2|  TEX  |  AP | P | Domain | XN | C B | 1 0|
;
; Bits[31:20]   - Top 12 bits of VA is pointer into table
; nG[17]=0      - Non global, enables matching against ASID in the TLB when set.
; S[16]=0       - Indicates normal memory is shared when set.
; AP2[15]=0
; AP[11:10]=11  - Configure for full read/write access in all modes
; TEX[14:12]=000
; CB[3:2]= 00   - Set attributes to Strongly-ordered memory.
;                 (except for the descriptor where code segment is based, see below)
; IMPP[9]=0     - Ignored
; Domain[5:8]=1111   - Set all pages to use domain 15
; XN[4]=0       - Execute never disabled
; Bits[1:0]=10  - Indicate entry is a 1MB section
;===================================================================

  */

// Short-descriptor format memory region attributes, without TEX remap
// When using the Short-descriptor translation table formats, TEX remap is disabled when SCTLR.TRE is set to 0.

// For TRE - see
// B4.1.127 PRRR, Primary Region Remap Register, VMSA

#define APRWval 		0x03	/* Full access */
#define APROval 		0x06	/* All write accesses generate Permission faults */
#define DOMAINval	0x0F
#define SECTIONval	0x02

/* Table B3-10 TEX, C, and B encodings when TRE == 0 */

#define TEXval_STGORD		0x00	/* Strongly-ordered, shareable */
#define Bval_STGORD			0x00	/* Strongly-ordered, shareable */
#define Cval_STGORD			0x00	/* Strongly-ordered, shareable */

#if 0///CPUSTYLE_STM32MP1
	// for debug: no Write-Allocate
	#define TEXval_WBCACHE		0x00	/* Outer and Inner Write-Back, no Write-Allocate */
	#define Bval_WBCACHE		0x01	/* Outer and Inner Write-Back, no Write-Allocate */
	#define Cval_WBCACHE		0x01	/* Outer and Inner Write-Back, no Write-Allocate */
#else /* CPUSTYLE_STM32MP1 */
	#define TEXval_WBCACHE		0x01	/* Outer and Inner Write-Back, Write-Allocate */
	#define Bval_WBCACHE		0x01	/* Outer and Inner Write-Back, Write-Allocate */
	#define Cval_WBCACHE		0x01	/* Outer and Inner Write-Back, Write-Allocate */
#endif /* CPUSTYLE_STM32MP1 */

#define TEXval_NOCACHE		0x01	/* Outer and Inner Non-cacheable, S bit */
#define Bval_NOCACHE		0x00	/* Outer and Inner Non-cacheable, S bit */
#define Cval_NOCACHE		0x00	/* Outer and Inner Non-cacheable, S bit */

#if 0
	// test
	#define TEXval_DEVICE		0x00	/* Shareable Device */
	#define Bval_DEVICE			0x00	/* Shareable Device */
	#define Cval_DEVICE			0x01	/* Shareable Device */
#else
	// normal
	#define TEXval_DEVICE		0x02	/* Non-shareable Device */
	#define Bval_DEVICE			0x00	/* Non-shareable Device */
	#define Cval_DEVICE			0x00	/* Non-shareable Device */
#endif

#if WITHSMPSYSTEM
	#define SHAREDval 1		// required for ldrex.. and strex.. functionality
#else /* WITHSMPSYSTEM */
	#define SHAREDval 0
#endif /* WITHSMPSYSTEM */

#define NoSHAREDval 0

// See B3.5.2 in DDI0406C_C_arm_architecture_reference_manual.pdf


#define	TTB_PARA(TEX, B, C, DOMAIN, SHARED, AP, XN) ( \
		(SECTIONval) * (1uL << 0) |	/* 0b10, Section or Supersection */ \
		(B) * (1uL << 2) |	/* B */ \
		(C) * (1uL << 3) |	/* C */ \
		(XN) * (1uL << 4) |	/* XN The Execute-never bit. */ \
		(DOMAIN) * (1uL << 5) |	/* DOMAIN */ \
		0 * (1uL << 9) |	/* implementation defined */ \
		(((AP) >> 0) & 0x03) * (1uL << 10) |	/* AP [1..0] */ \
		(TEX) * (1uL << 12) |	/* TEX */ \
		(((AP) >> 2) & 0x01) * (1uL << 15) |	/* AP[2] */ \
		(SHARED) * (1uL << 16) |	/* S */ \
		0 * (1uL << 17) |	/* nG */ \
		0 * (1uL << 18) |	/* 0 */ \
		0 * (1uL << 19) |	/* NS */ \
		0 \
	)

//; setting for Strongly-ordered memory
//#define	TTB_PARA_STRGLY             0b_0000_0000_1101_1110_0010
// not used
#define	TTB_PARA_STRGLY TTB_PARA(TEXval_STGORD, Bval_STGORD, Cval_STGORD, DOMAINval, 0 /* Shareable mot depend of this bit */, APRWval, 1)


//; setting for Outer and inner not cache normal memory
// not used
//#define	TTB_PARA_NORMAL_NOT_CACHE(ro, xn) TTB_PARA(TEXval_NOCACHE, Bval_NOCACHE, Cval_NOCACHE, DOMAINval, SHAREDval, (ro) ? APROval : APRWval, (xn) != 0)

//; setting for Outer and inner write back, write allocate normal memory (Cacheable)
//#define	TTB_PARA_NORMAL_CACHE       0b_0000_0001_1101_1110_1110
#define	TTB_PARA_NORMAL_CACHE(ro, xn) TTB_PARA(TEXval_WBCACHE, Bval_WBCACHE, Cval_WBCACHE, DOMAINval, SHAREDval, (ro) ? APROval : APRWval, (xn) != 0)

#define	TTB_PARA_DEVICE TTB_PARA(TEXval_DEVICE, Bval_DEVICE, Cval_DEVICE, DOMAINval, NoSHAREDval, APRWval, 1 /* XN=1 */)

#define	TTB_PARA_NO_ACCESS 0

/*
 * https://community.st.com/s/question/0D73W000000UagD/what-a-type-of-mmu-memory-regions-recommended-for-regions-with-peripheralsstronglyordered-or-device?s1oid=00Db0000000YtG6&s1nid=0DB0X000000DYbd&emkind=chatterCommentNotification&s1uid=0050X000007vtUt&emtm=1599464922440&fromEmail=1&s1ext=0&t=1599470826880
 *
 *
 * PatrickF (ST Employee)

2 hours ago
Hello,

"Device" should be used for all peripherals to get best performance.
There is no rationale to use "Strongly-Ordered" with Cortex-A7
(explanation should be found by deep analysis/understanding of various ARM documents).
 *
 *
*/

static uint32_t
FLASHMEMINITFUNC
ttb_accessbits(uintptr_t a, int ro, int xn)
{
	const uint32_t addrbase = a & 0xFFF00000uL;

#if CPUSTYLE_R7S721020

	if (a == 0x00000000uL)
		return addrbase | TTB_PARA_NO_ACCESS;		// NULL pointers access trap

	if (a >= 0x18000000uL && a < 0x20000000uL)			// FIXME: QSPI memory mapped should be R/O, but...
		return addrbase | TTB_PARA_NORMAL_CACHE(ro || 0, 0);

	if (a >= 0x00000000uL && a < 0x00A00000uL)			// up to 10 MB
		return addrbase | TTB_PARA_NORMAL_CACHE(ro, 0);
	if (a >= 0x20000000uL && a < 0x20A00000uL)			// up to 10 MB
		return addrbase | TTB_PARA_NORMAL_CACHE(ro, 0);

	return addrbase | TTB_PARA_DEVICE;

#elif CPUSTYLE_STM32MP1

	if (a < 0x10000000uL)			// BOOT
		return addrbase | TTB_PARA_NO_ACCESS;			// NULL pointers access trap

	if (a >= 0x20000000uL && a < 0x30000000uL)			// SYSRAM
		return addrbase | TTB_PARA_NORMAL_CACHE(ro, 0);

	if (a >= 0x40000000uL && a < 0x60000000uL)			//  peripherials 1, peripherials 2
		return addrbase | TTB_PARA_DEVICE;

	if (a >= 0x60000000uL && a < 0x70000000uL)			//  FMC NOR
		return addrbase | TTB_PARA_NORMAL_CACHE(ro, 0);

	if (a >= 0x70000000uL && a < 0xA0000000uL)			//  QUADSPI, FMC NAND, ...
		return addrbase | TTB_PARA_NORMAL_CACHE(ro || 1, 0);

	if (a >= 0xA0000000uL && a < 0xC0000000uL)			//  GIC
		return addrbase | TTB_PARA_DEVICE;

	if (a >= 0xC0000000uL && a < 0xE0000000uL)			// DDR memory
		return addrbase | TTB_PARA_NORMAL_CACHE(ro, 0);

	if (a >= 0xE0000000uL)								//  DEBUG
		return addrbase | TTB_PARA_DEVICE;

	return addrbase | TTB_PARA_NO_ACCESS;

#elif CPUSTYLE_XC7Z
	if (a >= 0xFC000000uL && a < 0xFE000000uL)			//  Quad-SPI linear address for linear mode
		return addrbase | TTB_PARA_NORMAL_CACHE(ro || 1, 0);

	if (a >= 0x40000000uL && a < 0xFC000000uL)
		return addrbase | TTB_PARA_DEVICE;

	return addrbase | TTB_PARA_NORMAL_CACHE(ro, 0);	// OCM (On Chip Memory)

#else
	#warning ttb_accessbits: Unhandled CPUSTYLE_xxxx

	return addrbase | TTB_PARA_DEVICE;

#endif
}
/* Загрузка TTBR, инвалидация кеш памяти и включение MMU */
static void FLASHMEMINITFUNC
sysinit_ttbr_initialize(void)
{
	extern volatile uint32_t __TTB_BASE;		// получено из скрипта линкера
	volatile uint32_t * const tlbbase = & __TTB_BASE;

#if 0
	/* Set location of level 1 page table
	; 31:14 - Translation table base addr (31:14-TTBCR.N, TTBCR.N is 0 out of reset)
	; 13:7  - 0x0
	; 6     - IRGN[0] 0x1  (Inner WB WA)
	; 5     - NOS     0x0  (Non-shared)
	; 4:3   - RGN     0x01 (Outer WB WA)
	; 2     - IMP     0x0  (Implementation Defined)
	; 1     - S       0x0  (Non-shared)
	; 0     - IRGN[1] 0x0  (Inner WB WA) */
	__set_TTBR0(((uint32_t)&Image$$TTB$$ZI$$Base) | 0x48);
	__ISB();

	/* Set up domain access control register
	; We set domain 0 to Client and all other domains to No Access.
	; All translation table entries specify domain 0 */
	__set_DACR(1);
	__ISB();
#else
	//CP15_writeTTBCR(0);
	   /* Set location of level 1 page table
	    ; 31:14 - Translation table base addr (31:14-TTBCR.N, TTBCR.N is 0 out of reset)
	    ; 13:7  - 0x0
	    ; 6     - IRGN[0] 0x1  (Inner WB WA)
	    ; 5     - NOS     0x0  (Non-shared)
	    ; 4:3   - RGN     0x01 (Outer WB WA)
	    ; 2     - IMP     0x0  (Implementation Defined)
	    ; 1     - S       0x0  (Non-shared)
	    ; 0     - IRGN[1] 0x0  (Inner WB WA) */
	__set_TTBR0((unsigned int) tlbbase | 0x48);	// TTBR0
	//CP15_writeTTB1((unsigned int) tlbbase | 0x48);	// TTBR1
	  __ISB();

	// Program the domain access register
	//__set_DACR(0x55555555); // domain 15: access are not checked
	__set_DACR(0xFFFFFFFF); // domain 15: access are not checked
#endif

	MMU_InvalidateTLB();

	// Обеспечиваем нормальную обработку RESEТ
	L1C_InvalidateDCacheAll();
	L1C_InvalidateICacheAll();
	L1C_InvalidateBTAC();
#if (__L2C_PRESENT == 1)
	L2C_InvAllByWay();
#endif

	MMU_Enable();
}

static void FLASHMEMINITFUNC
ttb_initialize(uint32_t (* accessbits)(uintptr_t a, int ro, int xn), uintptr_t textstart, uint_fast32_t textsize)
{
	extern volatile uint32_t __TTB_BASE;		// получено из скрипта линкера
	volatile uint32_t * const tlbbase = & __TTB_BASE;
	unsigned i;
	const uint_fast32_t pagesize = (1uL << 20);

	for (i = 0; i < 4096; ++ i)
	{
		const uintptr_t address = (uintptr_t) i << 20;
		tlbbase [i] =  accessbits(address, 0, 0);
	}
	/* Установить R/O атрибуты для указанной области */
	while (textsize >= pagesize)
	{
		tlbbase [textstart / pagesize] =  accessbits(textstart, 0 * 1, 0);
		textsize -= pagesize;
		textstart += pagesize;
	}

#if (CPUSTYLE_STM32MP1 || CPUSTYLE_XC7Z) && ! WITHISBOOTLOADER
	/* R/O, XN for pages table. - 16 MB size. */
	tlbbase [(uintptr_t) tlbbase / pagesize] = accessbits((uintptr_t) tlbbase, 1, 1);
#endif /* CPUSTYLE_STM32MP1 && ! WITHISBOOTLOADER */
}

// TODO: use MMU_TTSection. See also MMU_TTPage4k MMU_TTPage64k and MMU_CreateTranslationTable
// с точностью до 1 мегабайта
static void
FLASHMEMINITFUNC
ttb_map(
	uintptr_t va,	/* virtual address */
	uintptr_t la,	/* linear (physical) address */
	uint32_t (* accessbits)(uintptr_t a)
	)
{
	volatile extern uint32_t __TTB_BASE;		// получено из скрипта линкера
	volatile uint32_t * const tlbbase = & __TTB_BASE;
	unsigned i = va >> 20;
	tlbbase [i] =  accessbits(la);
}

#endif /* CPUSTYLE_R7S721 */

// PLL and caches inuitialize
static void FLASHMEMINITFUNC
sysinit_fpu_initialize(void)
{
#if CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7
	#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)

		/* FPU enable on Cortex M4F */
		SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));  /* set CP10 and CP11 Full Access */

		#if 0
			/* Lazy stacking enabled, automatic state saving enabled is a default state */
			/* http://infocenter.arm.com/help/topic/com.arm.doc.dai0298a/DAI0298A_cortex_m4f_lazy_stacking_and_context_switching.pdf */
			__set_FPSCR(			/* Floating-Point Context Control Register */
				(__get_FPSCR() & ~ (FPU_FPCCR_LSPEN_Msk)) | /* disable Lazy stacking feature */
				FPU_FPCCR_ASPEN_Msk |
				0);
		#endif

	#endif
	#ifdef UNALIGNED_SUPPORT_DISABLE
		SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
	#endif

#elif (__CORTEX_A != 0)

	// FPU
	//vfp_access_enable();
	//arm_hardware_VFPEnable();
	__FPU_Enable();

#endif /* CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7 */

#if (__CORTEX_M != 0) && CTLSTYLE_V3D
	SCB->CCR &= ~ SCB_CCR_UNALIGN_TRP_Msk;
#endif /* (__CORTEX_M != 0) && CTLSTYLE_V3D */
}

static void FLASHMEMINITFUNC
sysintt_sdram_initialize(void)
{
#if WITHSDRAMHW && WITHISBOOTLOADER
	/* В процессоре есть внешняя память - если уже в ней то не трогаем */
	arm_hardware_sdram_initialize();

#elif WITHSDRAMHW && (CTLSTYLE_V1D || CTLSTYLE_V3D)
	/* В процессоре есть внешняя память - только данные */
	arm_hardware_sdram_initialize();

#endif /* WITHSDRAMHW && WITHISBOOTLOADER */
}

static void FLASHMEMINITFUNC
sysinit_debug_initialize(void)
{
#if CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7

	#if WITHDEBUG && WITHINTEGRATEDDSP && CPUSTYLE_ARM_CM7
		// Поддержка для функций диагностики быстродействия BEGINx_STAMP/ENDx_STAMP - audio.c
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->LAR = 0xC5ACCE55;	// Key value for unlock
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
		DWT->LAR = 0x00000000;	// Key value for lock
	#endif /* WITHDEBUG && WITHINTEGRATEDDSP */

#endif /* CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7 */

#if (__CORTEX_A != 0)

	#if WITHDEBUG
	{
		// Поддержка для функций диагностики быстродействия BEGINx_STAMP/ENDx_STAMP - audio.c
		// From https://stackoverflow.com/questions/3247373/how-to-measure-program-execution-time-in-arm-cortex-a8-processor

		enum { do_reset = 0, enable_divider = 0 };
		// in general enable all counters (including cycle counter)
		int32_t value = 1;

		// peform reset:
		if (do_reset)
		{
			value |= 2;     // reset all counters to zero.
			value |= 4;     // reset cycle counter to zero.
		}

		if (enable_divider)
			value |= 8;     // enable "by 64" divider for CCNT.

		value |= 16;

		// program the performance-counter control-register:
		//asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(value));
		__set_CP(15, 0, value, 9, 12, 0);

		// enable all counters:
		//asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));
		__set_CP(15, 0, 0x8000000f, 9, 12, 1);

		// clear overflows:
		//asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));
		__set_CP(15, 0, 0x8000000f, 9, 12, 3);
	}
	#endif /* WITHDEBUG */

#endif /* (__CORTEX_A != 0) */

#if WITHDEBUG
	HARDWARE_DEBUG_INITIALIZE();
	HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);
#endif /* WITHDEBUG */
}

#if (__CORTEX_A != 0)
/** \brief  Set HVBAR

    This function assigns the given value to the Hyp Vector Base Address Register.

    \param [in]    hvbar  Hyp Vector Base Address Register value to set
 */
__STATIC_FORCEINLINE void __set_HVBAR(uint32_t hvbar)
{
	// cp, op1, Rt, CRn, CRm, op2
  __set_CP(15, 4, hvbar, 12, 0, 0);
}
#endif /* (__CORTEX_A != 0) */

static void FLASHMEMINITFUNC
sysinit_vbar_initialize(void)
{
#if (__CORTEX_A != 0)

	extern unsigned long __Vectors;

	const uintptr_t vbase = (uintptr_t) & __Vectors;

	__set_VBAR(vbase);	 // Set Vector Base Address Register (bits 4..0 should be zero)
	__set_MVBAR(vbase);	 // Set Monitor Vector Base Address Register (bits 4..0 should be zero) - на работу не вличет... но на всякий случай
	//__set_HVBAR(vbase);	 // Set Hyp Vector Base Address Register

	__set_SCTLR(__get_SCTLR() & ~ SCTLR_V_Msk);	// v=0 - use VBAR as vectors address
	__set_SCTLR(__get_SCTLR() & ~ SCTLR_A_Msk);	// 0 = Strict alignment fault checking disabled. This is the reset value.

	//PRINTF("vbar=%08lX, mvbar=%08lX\n", __get_VBAR(), __get_MVBAR());

#endif /* (__CORTEX_A != 0) */
}

static void FLASHMEMINITFUNC
sysinit_mmu_initialize(void)
{
	//PRINTF("sysinit_mmu_initialize\n");
#if (__CORTEX_A != 0)
	// MMU inuitialize

#if 0 && WITHDEBUG
	uint_fast32_t leveli;
	for (leveli = 0; leveli <= ARM_CA9_CACHELEVELMAX; ++ leveli)
	{

		__set_CSSELR(leveli * 2 + 0);	// data cache select
		const uint32_t ccsidr0 = __get_CCSIDR();
		const uint32_t assoc0 = (ccsidr0 >> 3) & 0x3FF;
		const int passoc0 = countbits2(assoc0);
		const uint32_t maxsets0 = (ccsidr0 >> 13) & 0x7FFF;
		const uint32_t linesize0 = 4uL << (((ccsidr0 >> 0) & 0x07) + 2);
		PRINTF(PSTR("cpu_initialize1: level=%d, passoc=%d, assoc=%u, maxsets=%u, data cache row size = %u\n"), leveli, passoc0, assoc0, maxsets0, linesize0);

		__set_CSSELR(leveli * 2 + 1);	// instruction cache select
		const uint32_t ccsidr1 = __get_CCSIDR();
		const uint32_t assoc1 = (ccsidr1 >> 3) & 0x3FF;
		const int passoc1 = countbits2(assoc1);
		const uint32_t maxsets1 = (ccsidr1 >> 13) & 0x7FFF;
		const uint32_t linesize1 = 4uL << (((ccsidr1 >> 0) & 0x07) + 2);
		PRINTF(PSTR("cpu_initialize1: level=%d, passoc=%d, assoc=%u, maxsets=%u, instr cache row size = %u\n"), leveli, passoc1, assoc1, maxsets1, linesize1);
	}
#endif /* WITHDEBUG */

#if WITHISBOOTLOADER || CPUSTYLE_R7S721

	// MMU inuitialize
	ttb_initialize(ttb_accessbits, 0, 0);
	sysinit_ttbr_initialize();	/* Загрузка TTBR, инвалидация кеш памяти и включение MMU */

#elif CPUSTYLE_STM32MP1
	extern uint32_t __data_start__;
	// MMU inuitialize
	ttb_initialize(ttb_accessbits, 0xC0000000, (uintptr_t) & __data_start__ - 0xC0000000);
	sysinit_ttbr_initialize();	/* Загрузка TTBR, инвалидация кеш памяти и включение MMU */

#else
	// MMU inuitialize
	ttb_initialize(ttb_accessbits, 0, 0);
	sysinit_ttbr_initialize();	/* Загрузка TTBR, инвалидация кеш памяти и включение MMU */

#endif

#endif /* (__CORTEX_A != 0) */

	//PRINTF("MMU initialized\n");
}

#if CPUSTYLE_XC7Z


void xc7z_hardware_initialize(void)
{
	int Status;

/*
	static XGpioPs xc7z_gpio;
	// GPIO init
	XGpioPs_Config * ConfigPtr;
	ConfigPtr = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
	Status = XGpioPs_CfgInitialize(& xc7z_gpio, ConfigPtr, ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS)
		PRINTF("PS GPIO init error\n");
*/
}

/* Opcode exit is 0 all the time */
#define OPCODE_EXIT       0U
#define OPCODE_CLEAR      1U
#define OPCODE_WRITE      2U
#define OPCODE_MASKWRITE  3U
#define OPCODE_MASKPOLL   4U
#define OPCODE_MASKDELAY  5U
#define NEW_PS7_ERR_CODE 1

/* Encode number of arguments in last nibble */
#define EMIT_EXIT()                   ( (OPCODE_EXIT      << 4 ) | 0 )
#define EMIT_CLEAR(addr)              ( (OPCODE_CLEAR     << 4 ) | 1 ) , addr
#define EMIT_WRITE(addr,val)          ( (OPCODE_WRITE     << 4 ) | 2 ) , addr, val
#define EMIT_MASKWRITE(addr,mask,val) ( (OPCODE_MASKWRITE << 4 ) | 3 ) , addr, mask, val
#define EMIT_MASKPOLL(addr,mask)      ( (OPCODE_MASKPOLL  << 4 ) | 2 ) , addr, mask
#define EMIT_MASKDELAY(addr,mask)      ( (OPCODE_MASKDELAY << 4 ) | 2 ) , addr, mask

/* Returns codes of ps7_init* */
#define PS7_INIT_SUCCESS		(0)
#define PS7_INIT_CORRUPT		(1)
#define PS7_INIT_TIMEOUT		(2)
#define PS7_POLL_FAILED_DDR_INIT	(3)
#define PS7_POLL_FAILED_DMA		(4)
#define PS7_POLL_FAILED_PLL		(5)

#define PCW_SILICON_VERSION_1	0
#define PCW_SILICON_VERSION_2	1
#define PCW_SILICON_VERSION_3	2

/* For delay calculation using global registers*/
#define SCU_GLOBAL_TIMER_COUNT_L32	0xF8F00200
#define SCU_GLOBAL_TIMER_COUNT_U32	0xF8F00204
#define SCU_GLOBAL_TIMER_CONTROL	0xF8F00208
#define SCU_GLOBAL_TIMER_AUTO_INC	0xF8F00218
#define APU_FREQ  666666666

#define PS7_MASK_POLL_TIME 100000000

#define __arch_getb(a)			(*(volatile unsigned char *)(a))
#define __arch_getw(a)			(*(volatile unsigned short *)(a))
#define __arch_getl(a)			(*(volatile unsigned int *)(a))
#define __arch_getq(a)			(*(volatile unsigned long long *)(a))

#define __arch_putb(v,a)		(*(volatile unsigned char *)(a) = (v))
#define __arch_putw(v,a)		(*(volatile unsigned short *)(a) = (v))
#define __arch_putl(v,a)		(*(volatile unsigned int *)(a) = (v))
#define __arch_putq(v,a)		(*(volatile unsigned long long *)(a) = (v))

#define __raw_writeb(v,a)	__arch_putb(v,a)
#define __raw_writew(v,a)	__arch_putw(v,a)
#define __raw_writel(v,a)	__arch_putl(v,a)
#define __raw_writeq(v,a)	__arch_putq(v,a)

#define __raw_readb(a)		__arch_getb(a)
#define __raw_readw(a)		__arch_getw(a)
#define __raw_readl(a)		__arch_getl(a)
#define __raw_readq(a)		__arch_getq(a)

/* IO accessors. No memory barriers desired. */
static inline void iowrite(unsigned long val, unsigned long addr)
{
	__raw_writel(val, addr);
}

static inline unsigned long ioread(unsigned long addr)
{
	return __raw_readl(addr);
}

/* start timer */
static void perf_start_clock(void)
{
	iowrite((1 << 0) | /* Timer Enable */
		(1 << 3) | /* Auto-increment */
		(0 << 8), /* Pre-scale */
		SCU_GLOBAL_TIMER_CONTROL);
}

/* Compute mask for given delay in miliseconds*/
static unsigned long get_number_of_cycles_for_delay(unsigned long delay)
{
	return (APU_FREQ / (2 * 1000)) * delay;
}

/* stop timer */
static void perf_disable_clock(void)
{
	iowrite(0, SCU_GLOBAL_TIMER_CONTROL);
}

/* stop timer and reset timer count regs */
static void perf_reset_clock(void)
{
	perf_disable_clock();
	iowrite(0, SCU_GLOBAL_TIMER_COUNT_L32);
	iowrite(0, SCU_GLOBAL_TIMER_COUNT_U32);
}

static void perf_reset_and_start_timer(void)
{
	perf_reset_clock();
	perf_start_clock();
}

int ps7_config(const unsigned long * ps7_config_init)
{
	const unsigned long *ptr = ps7_config_init;

    unsigned long  opcode;            // current instruction ..
    unsigned long  args[16];           // no opcode has so many args ...
    int  numargs;           // number of arguments of this instruction
    int  j;                 // general purpose index

    volatile unsigned long *addr;         // some variable to make code readable
    unsigned long  val,mask;              // some variable to make code readable

    int finish = -1 ;           // loop while this is negative !
    int i = 0;                  // Timeout variable

    while( finish < 0 ) {
        numargs = ptr[0] & 0xF;
        opcode = ptr[0] >> 4;

        for( j = 0 ; j < numargs ; j ++ )
            args[j] = ptr[j+1];
        ptr += numargs + 1;


        switch ( opcode ) {

        case OPCODE_EXIT:
            finish = PS7_INIT_SUCCESS;
            break;

        case OPCODE_CLEAR:
            addr = (volatile unsigned long*) args[0];
            *addr = 0;
            break;

        case OPCODE_WRITE:
            addr = (volatile unsigned long*) args[0];
            val = args[1];
            *addr = val;
            break;

        case OPCODE_MASKWRITE:
            addr = (volatile unsigned long*) args[0];
            mask = args[1];
            val = args[2];
            *addr = ( val & mask ) | ( *addr & ~mask);
            break;

        case OPCODE_MASKPOLL:
            addr = (volatile unsigned long*) args[0];
            mask = args[1];
            i = 0;
            while (!(*addr & mask)) {
                if (i == PS7_MASK_POLL_TIME) {
                    finish = PS7_INIT_TIMEOUT;
                    break;
                }
                i++;
            }
            break;
        case OPCODE_MASKDELAY:
            addr = (volatile unsigned long*) args[0];
            mask = args[1];
            int delay = get_number_of_cycles_for_delay(mask);
            perf_reset_and_start_timer();
            while ((*addr < delay)) {
            }
            break;
        default:
            finish = PS7_INIT_CORRUPT;
            break;
        }
    }
    return finish;
}

static const unsigned long ps7_pll_init_data_3_0[] = {
		//EMIT_WRITE(0XF8000008, 0x0000DF0DU), // SLCR_UNLOCK
		EMIT_MASKWRITE(0XF8000110, 0x003FFFF0U ,0x000FA220U),
		EMIT_MASKWRITE(0XF8000100, 0x0007F000U ,0x00028000U),
		EMIT_MASKWRITE(0XF8000100, 0x00000010U ,0x00000010U),
		EMIT_MASKWRITE(0XF8000100, 0x00000001U ,0x00000001U),
		EMIT_MASKWRITE(0XF8000100, 0x00000001U ,0x00000000U),
		EMIT_MASKPOLL(0XF800010C, 0x00000001U),
		EMIT_MASKWRITE(0XF8000100, 0x00000010U ,0x00000000U),
		EMIT_MASKWRITE(0XF8000120, 0x1F003F30U ,0x1F000200U),
		EMIT_MASKWRITE(0XF8000114, 0x003FFFF0U ,0x0012C220U),
		EMIT_MASKWRITE(0XF8000104, 0x0007F000U ,0x00020000U),
		EMIT_MASKWRITE(0XF8000104, 0x00000010U ,0x00000010U),
		EMIT_MASKWRITE(0XF8000104, 0x00000001U ,0x00000001U),
		EMIT_MASKWRITE(0XF8000104, 0x00000001U ,0x00000000U),
		EMIT_MASKPOLL(0XF800010C, 0x00000002U),
		EMIT_MASKWRITE(0XF8000104, 0x00000010U ,0x00000000U),
		EMIT_MASKWRITE(0XF8000124, 0xFFF00003U ,0x0C200003U),
		EMIT_MASKWRITE(0XF8000118, 0x003FFFF0U ,0x000FA240U),
		EMIT_MASKWRITE(0XF8000108, 0x0007F000U ,0x00030000U),
		EMIT_MASKWRITE(0XF8000108, 0x00000010U ,0x00000010U),
		EMIT_MASKWRITE(0XF8000108, 0x00000001U ,0x00000001U),
		EMIT_MASKWRITE(0XF8000108, 0x00000001U ,0x00000000U),
		EMIT_MASKPOLL(0XF800010C, 0x00000004U),
		EMIT_MASKWRITE(0XF8000108, 0x00000010U ,0x00000000U),
		//EMIT_WRITE(0XF8000004, 0x0000767BU),	// SLCR_LOCK
		EMIT_EXIT(),
	};

static const unsigned long ps7_clock_init_data_3_0[] = {
		//EMIT_WRITE(0XF8000008, 0x0000DF0DU), // SLCR_UNLOCK
		EMIT_MASKWRITE(0XF8000128, 0x03F03F01U ,0x00700F01U),	// DCI_CLK_CTRL
		EMIT_MASKWRITE(0XF8000150, 0x00003F33U ,0x00001001U),	// SDIO_CLK_CTRL
		EMIT_MASKWRITE(0XF8000154, 0x00003F33U ,0x00001002U),	// UART_CLK_CTRL
		EMIT_MASKWRITE(0XF8000168, 0x00003F31U ,0x00000801U),	// PCAP_CLK_CTRL
		EMIT_MASKWRITE(0XF8000170, 0x03F03F30U ,0x00400400U),	// FPGA0_CLK_CTRL PL Clock 0 Output control
		EMIT_MASKWRITE(0XF80001C4, 0x00000001U ,0x00000001U),	// CLK_621_TRUE CPU Clock Ratio Mode select
		EMIT_MASKWRITE(0XF800012C, 0x01FFCCCDU ,0x016C040DU),	// APER_CLK_CTRL AMBA Peripheral Clock Control
		//EMIT_WRITE(0XF8000004, 0x0000767BU),// SLCR_LOCK
		EMIT_EXIT(),
	};

static const unsigned long ps7_ddr_init_data_3_0[] = {
		EMIT_MASKWRITE(0XF8006000, 0x0001FFFFU ,0x00000084U),
		EMIT_MASKWRITE(0XF8006004, 0x0007FFFFU ,0x00001082U),
		EMIT_MASKWRITE(0XF8006008, 0x03FFFFFFU ,0x03C0780FU),
		EMIT_MASKWRITE(0XF800600C, 0x03FFFFFFU ,0x02001001U),
		EMIT_MASKWRITE(0XF8006010, 0x03FFFFFFU ,0x00014001U),
		EMIT_MASKWRITE(0XF8006014, 0x001FFFFFU ,0x0004159BU),
		EMIT_MASKWRITE(0XF8006018, 0xF7FFFFFFU ,0x44E458D3U),
		EMIT_MASKWRITE(0XF800601C, 0xFFFFFFFFU ,0x7282BCE5U),
		EMIT_MASKWRITE(0XF8006020, 0x7FDFFFFCU ,0x270872D0U),
		EMIT_MASKWRITE(0XF8006024, 0x0FFFFFC3U ,0x00000000U),
		EMIT_MASKWRITE(0XF8006028, 0x00003FFFU ,0x00002007U),
		EMIT_MASKWRITE(0XF800602C, 0xFFFFFFFFU ,0x00000008U),
		EMIT_MASKWRITE(0XF8006030, 0xFFFFFFFFU ,0x00040B30U),
		EMIT_MASKWRITE(0XF8006034, 0x13FF3FFFU ,0x000116D4U),
		EMIT_MASKWRITE(0XF8006038, 0x00000003U ,0x00000000U),
		EMIT_MASKWRITE(0XF800603C, 0x000FFFFFU ,0x00000666U),
		EMIT_MASKWRITE(0XF8006040, 0xFFFFFFFFU ,0xFFFF0000U),
		EMIT_MASKWRITE(0XF8006044, 0x0FFFFFFFU ,0x0FF55555U),
		EMIT_MASKWRITE(0XF8006048, 0x0003F03FU ,0x0003C008U),
		EMIT_MASKWRITE(0XF8006050, 0xFF0F8FFFU ,0x77010800U),
		EMIT_MASKWRITE(0XF8006058, 0x00010000U ,0x00000000U),
		EMIT_MASKWRITE(0XF800605C, 0x0000FFFFU ,0x00005003U),
		EMIT_MASKWRITE(0XF8006060, 0x000017FFU ,0x0000003EU),
		EMIT_MASKWRITE(0XF8006064, 0x00021FE0U ,0x00020000U),
		EMIT_MASKWRITE(0XF8006068, 0x03FFFFFFU ,0x00284141U),
		EMIT_MASKWRITE(0XF800606C, 0x0000FFFFU ,0x00001610U),
		EMIT_MASKWRITE(0XF8006078, 0x03FFFFFFU ,0x00466111U),
		EMIT_MASKWRITE(0XF800607C, 0x000FFFFFU ,0x00032222U),
		EMIT_MASKWRITE(0XF80060A4, 0xFFFFFFFFU ,0x10200802U),
		EMIT_MASKWRITE(0XF80060A8, 0x0FFFFFFFU ,0x0690CB73U),
		EMIT_MASKWRITE(0XF80060AC, 0x000001FFU ,0x000001FEU),
		EMIT_MASKWRITE(0XF80060B0, 0x1FFFFFFFU ,0x1CFFFFFFU),
		EMIT_MASKWRITE(0XF80060B4, 0x00000200U ,0x00000200U),
		EMIT_MASKWRITE(0XF80060B8, 0x01FFFFFFU ,0x00200066U),
		EMIT_MASKWRITE(0XF80060C4, 0x00000003U ,0x00000000U),
		EMIT_MASKWRITE(0XF80060C8, 0x000000FFU ,0x00000000U),
		EMIT_MASKWRITE(0XF80060DC, 0x00000001U ,0x00000000U),
		EMIT_MASKWRITE(0XF80060F0, 0x0000FFFFU ,0x00000000U),
		EMIT_MASKWRITE(0XF80060F4, 0x0000000FU ,0x00000008U),
		EMIT_MASKWRITE(0XF8006114, 0x000000FFU ,0x00000000U),
		EMIT_MASKWRITE(0XF8006118, 0x7FFFFFCFU ,0x40000001U),
		EMIT_MASKWRITE(0XF800611C, 0x7FFFFFCFU ,0x40000001U),
		EMIT_MASKWRITE(0XF8006120, 0x7FFFFFCFU ,0x40000000U),
		EMIT_MASKWRITE(0XF8006124, 0x7FFFFFCFU ,0x40000000U),
		EMIT_MASKWRITE(0XF800612C, 0x000FFFFFU ,0x00029000U),
		EMIT_MASKWRITE(0XF8006130, 0x000FFFFFU ,0x00029000U),
		EMIT_MASKWRITE(0XF8006134, 0x000FFFFFU ,0x00029000U),
		EMIT_MASKWRITE(0XF8006138, 0x000FFFFFU ,0x00029000U),
		EMIT_MASKWRITE(0XF8006140, 0x000FFFFFU ,0x00000035U),
		EMIT_MASKWRITE(0XF8006144, 0x000FFFFFU ,0x00000035U),
		EMIT_MASKWRITE(0XF8006148, 0x000FFFFFU ,0x00000035U),
		EMIT_MASKWRITE(0XF800614C, 0x000FFFFFU ,0x00000035U),
		EMIT_MASKWRITE(0XF8006154, 0x000FFFFFU ,0x00000080U),
		EMIT_MASKWRITE(0XF8006158, 0x000FFFFFU ,0x00000080U),
		EMIT_MASKWRITE(0XF800615C, 0x000FFFFFU ,0x00000080U),
		EMIT_MASKWRITE(0XF8006160, 0x000FFFFFU ,0x00000080U),
		EMIT_MASKWRITE(0XF8006168, 0x001FFFFFU ,0x000000F9U),
		EMIT_MASKWRITE(0XF800616C, 0x001FFFFFU ,0x000000F9U),
		EMIT_MASKWRITE(0XF8006170, 0x001FFFFFU ,0x000000F9U),
		EMIT_MASKWRITE(0XF8006174, 0x001FFFFFU ,0x000000F9U),
		EMIT_MASKWRITE(0XF800617C, 0x000FFFFFU ,0x000000C0U),
		EMIT_MASKWRITE(0XF8006180, 0x000FFFFFU ,0x000000C0U),
		EMIT_MASKWRITE(0XF8006184, 0x000FFFFFU ,0x000000C0U),
		EMIT_MASKWRITE(0XF8006188, 0x000FFFFFU ,0x000000C0U),
		EMIT_MASKWRITE(0XF8006190, 0x6FFFFEFEU ,0x00040080U),
		EMIT_MASKWRITE(0XF8006194, 0x000FFFFFU ,0x0001FC82U),
		EMIT_MASKWRITE(0XF8006204, 0xFFFFFFFFU ,0x00000000U),
		EMIT_MASKWRITE(0XF8006208, 0x000703FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF800620C, 0x000703FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF8006210, 0x000703FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF8006214, 0x000703FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF8006218, 0x000F03FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF800621C, 0x000F03FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF8006220, 0x000F03FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF8006224, 0x000F03FFU ,0x000003FFU),
		EMIT_MASKWRITE(0XF80062A8, 0x00000FF5U ,0x00000000U),
		EMIT_MASKWRITE(0XF80062AC, 0xFFFFFFFFU ,0x00000000U),
		EMIT_MASKWRITE(0XF80062B0, 0x003FFFFFU ,0x00005125U),
		EMIT_MASKWRITE(0XF80062B4, 0x0003FFFFU ,0x000012A8U),
		EMIT_MASKPOLL(0XF8000B74, 0x00002000U),
		EMIT_MASKWRITE(0XF8006000, 0x0001FFFFU ,0x00000085U),
		EMIT_MASKPOLL(0XF8006054, 0x00000007U),
		EMIT_EXIT(),
	};

static const unsigned long ps7_mio_init_data_3_0[] = {
		//EMIT_WRITE(0XF8000008, 0x0000DF0DU), // SLCR_UNLOCK
		EMIT_MASKWRITE(0XF8000B40, 0x00000FFFU ,0x00000600U),
		EMIT_MASKWRITE(0XF8000B44, 0x00000FFFU ,0x00000600U),
		EMIT_MASKWRITE(0XF8000B48, 0x00000FFFU ,0x00000672U),
		EMIT_MASKWRITE(0XF8000B4C, 0x00000FFFU ,0x00000800U),
		EMIT_MASKWRITE(0XF8000B50, 0x00000FFFU ,0x00000674U),
		EMIT_MASKWRITE(0XF8000B54, 0x00000FFFU ,0x00000800U),
		EMIT_MASKWRITE(0XF8000B58, 0x00000FFFU ,0x00000600U),
		EMIT_MASKWRITE(0XF8000B5C, 0xFFFFFFFFU ,0x0018C61CU),
		EMIT_MASKWRITE(0XF8000B60, 0xFFFFFFFFU ,0x00F9861CU),
		EMIT_MASKWRITE(0XF8000B64, 0xFFFFFFFFU ,0x00F9861CU),
		EMIT_MASKWRITE(0XF8000B68, 0xFFFFFFFFU ,0x00F9861CU),
		EMIT_MASKWRITE(0XF8000B6C, 0x00007FFFU ,0x00000220U),
		EMIT_MASKWRITE(0XF8000B70, 0x00000001U ,0x00000001U),
		EMIT_MASKWRITE(0XF8000B70, 0x00000021U ,0x00000020U),
		EMIT_MASKWRITE(0XF8000B70, 0x07FEFFFFU ,0x00000823U),
		EMIT_MASKWRITE(0XF80007A0, 0x00003FFFU ,0x00001680U),
		EMIT_MASKWRITE(0XF80007A4, 0x00003FFFU ,0x00001680U),
		EMIT_MASKWRITE(0XF80007A8, 0x00003FFFU ,0x00001680U),
		EMIT_MASKWRITE(0XF80007AC, 0x00003FFFU ,0x00001680U),
		EMIT_MASKWRITE(0XF80007B0, 0x00003FFFU ,0x00001680U),
		EMIT_MASKWRITE(0XF80007B4, 0x00003FFFU ,0x00001680U),
		EMIT_MASKWRITE(0XF8000830, 0x003F003FU ,0x00380037U),
		//EMIT_WRITE(0XF8000004, 0x0000767BU),// SLCR_LOCK
		EMIT_EXIT(),
};

static const unsigned long ps7_peripherals_init_data_3_0[] = {
		//EMIT_WRITE(0XF8000008, 0x0000DF0DU), // SLCR_UNLOCK
		EMIT_MASKWRITE(0XF8000B48, 0x00000180U ,0x00000180U),	// DDRIOB_DATA0
		EMIT_MASKWRITE(0XF8000B4C, 0x00000180U ,0x00000000U),	// DDRIOB_DATA1
		EMIT_MASKWRITE(0XF8000B50, 0x00000180U ,0x00000180U),	// DDRIOB_DIFF0
		EMIT_MASKWRITE(0XF8000B54, 0x00000180U ,0x00000000U),	// DDRIOB_DIFF1
		//EMIT_WRITE(0XF8000004, 0x0000767BU),	// SLCR_LOCK
//		EMIT_MASKWRITE(0XE0001034, 0x000000FFU ,0x00000006U),	// Register (UART) Baud_rate_divider_reg0
//		EMIT_MASKWRITE(0XE0001018, 0x0000FFFFU ,0x0000007CU),
//		EMIT_MASKWRITE(0XE0001000, 0x000001FFU ,0x00000017U),
//		EMIT_MASKWRITE(0XE0001004, 0x000003FFU ,0x00000020U),
		EMIT_MASKWRITE(0XE000D000, 0x00080000U ,0x00080000U),	// XQSPIPS_CR_OFFSET
		EMIT_MASKWRITE(0XF8007000, 0x20000000U ,0x00000000U),	// Register (devcfg) XDCFG_CTRL_OFFSET
		EMIT_EXIT(),
	};

static const unsigned long ps7_post_config_3_0[] = {
		//EMIT_WRITE(0XF8000008, 0x0000DF0DU), // SLCR_UNLOCK
		EMIT_MASKWRITE(0XF8000900, 0x0000000FU ,0x0000000FU),	// LVL_SHFTR_EN
		EMIT_MASKWRITE(0XF8000240, 0xFFFFFFFFU ,0x00000000U),	// FPGA_RST_CTRL
		//EMIT_WRITE(0XF8000004, 0x0000767BU),	// SLCR_LOCK
		EMIT_EXIT(),
	};

static int ps7_init(void)
{
	int ret;

	SCLR->SLCR_UNLOCK = 0x0000DF0DU;

	ret = ps7_config(ps7_mio_init_data_3_0);
	if (ret != PS7_INIT_SUCCESS)
		return ret;

	ret = ps7_config(ps7_pll_init_data_3_0);
	if (ret != PS7_INIT_SUCCESS)
		return ret;

	ret = ps7_config(ps7_clock_init_data_3_0);
	if (ret != PS7_INIT_SUCCESS)
		return ret;

	ret = ps7_config(ps7_ddr_init_data_3_0);
	if (ret != PS7_INIT_SUCCESS)
		return ret;

	ret = ps7_config(ps7_peripherals_init_data_3_0);
	if (ret != PS7_INIT_SUCCESS)
		return ret;

	ret = ps7_config(ps7_post_config_3_0);
		if (ret != PS7_INIT_SUCCESS)
			return ret;

	return PS7_INIT_SUCCESS;
}

#endif /* CPUSTYLE_XC7Z */
/* функция вызывается из start-up до копирования в SRAM всех "быстрых" функций и до инициализации переменных
*/
// watchdog disable, clock initialize, cache enable
void
FLASHMEMINITFUNC
SystemInit(void)
{
#if CPUSTYLE_XC7Z & WITHISBOOTLOADER	// FSBL

	xc7z_hardware_initialize();
	ps7_init();

#endif /* CPUSTYLE_XC7Z & WITHISBOOTLOADER */

	sysinit_fpu_initialize();
	sysinit_pll_cache_initialize();	// PLL and caches inuitialize
	sysinit_debug_initialize();
	sysintt_sdram_initialize();
	sysinit_vbar_initialize();		// interrupt vectors relocate
	sysinit_mmu_initialize();
}


#if  (__CORTEX_A != 0)

static void cortexa_cpuinfo(void)
{
	volatile uint_fast32_t vvv;
	dbg_putchar('$');
	PRINTF(PSTR("CPU%u: VBAR=%p, TTBR0=%p, cpsr=%08lX, SCTLR=%08lX, ACTLR=%08lX, sp=%p\n"), (unsigned) (__get_MPIDR() & 0x03),  (unsigned long) __get_VBAR(), (unsigned long) __get_TTBR0(), (unsigned long) __get_CPSR(), (unsigned long) __get_SCTLR(), (unsigned long) __get_ACTLR(), & vvv);
}

#if CPUSTYLE_STM32MP1


/*
 * Cores secure magic numbers
 * Constant to be stored in bakcup register
 * BOOT_API_MAGIC_NUMBER_TAMP_BCK_REG_IDX
 */
//#define BOOT_API_A7_CORE0_MAGIC_NUMBER				0xCA7FACE0U
#define BOOT_API_A7_CORE1_MAGIC_NUMBER				0xCA7FACE1U

/*
 * TAMP_BCK4R register index
 * This register is used to write a Magic Number in order to restart
 * Cortex A7 Core 1 and make it execute @ branch address from TAMP_BCK5R
 */
//#define BOOT_API_CORE1_MAGIC_NUMBER_TAMP_BCK_REG_IDX		4U

/*
 * TAMP_BCK5R register index
 * This register is used to contain the branch address of
 * Cortex A7 Core 1 when restarted by a TAMP_BCK4R magic number writing
 */
//#define BOOT_API_CORE1_BRANCH_ADDRESS_TAMP_BCK_REG_IDX		5U

/*******************************************************************************
 * STM32MP1 handler called when a power domain is about to be turned on. The
 * mpidr determines the CPU to be turned on.
 * call by core 0 to activate core 1
 ******************************************************************************/
static void cortexa_mp_cpu1_start(uintptr_t startfunc)
{
	PWR->CR1 |= PWR_CR1_DBP;	// 1: Write access to RTC and backup domain registers enabled.
	(void) PWR->CR1;
	while ((PWR->CR1 & PWR_CR1_DBP) == 0)
		;

	RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_RTCAPBEN;
	(void) RCC->MP_APB5ENSETR;
	RCC->MP_APB5LPENSETR = RCC_MP_APB5LPENSETR_RTCAPBLPEN;  // Включить тактирование
	(void) RCC->MP_APB5LPENSETR;

	RCC->MP_AHB5ENSETR = RCC_MC_AHB5ENSETR_BKPSRAMEN;
	(void) RCC->MP_AHB5ENSETR;
	RCC->MP_AHB5LPENSETR = RCC_MC_AHB5LPENSETR_BKPSRAMLPEN;
	(void) RCC->MP_AHB5LPENSETR;

	/* Write entrypoint in backup RAM register */
	TAMP->BKP5R = startfunc;	// Invoke at SVC context
	(void) TAMP->BKP5R;

	/* Write magic number in backup register */
	TAMP->BKP4R = BOOT_API_A7_CORE1_MAGIC_NUMBER;
	(void) TAMP->BKP4R;

//	PWR->CR1 &= ~ PWR_CR1_DBP;	// 0: Write access to RTC and backup domain registers disabled.
//	(void) PWR->CR1;
//	while ((PWR->CR1 & PWR_CR1_DBP) != 0)
//		;

	arm_hardware_flush_all();	// startup code should be copyed in to sysram for example.

	/* Generate an IT to core 1 */
	GIC_SendSGI(SGI8_IRQn, 0x01 << 1, 0x00);	// CPU1, filer=0
}

#elif CPUSTYLE_XC7Z

// See also:
//	https://stackoverflow.com/questions/60873390/zynq-7000-minimum-asm-code-to-init-cpu1-from-cpu0

static void cortexa_mp_cpu1_start(uintptr_t startfunc)
{
	* (volatile uint32_t *) 0xFFFFFFF0 = startfunc;	// Invoke at SVC context
	arm_hardware_flush_all();	// startup code should be copyed in to sysram for example.
	/* Generate an IT to core 1 */
	__SEV();
}

#endif /* CPUSTYLE_STM32MP1 */

static RAMDTCM SPINLOCK_t cpu1init;

void Reset_CPUn_Handler(void)
{
#if (__CORTEX_A == 9U)
	// set the ACTLR.SMP
	// 0x02: L2 Prefetch hint enable
	__set_ACTLR(__get_ACTLR() | ACTLR_SMP_Msk | ACTLR_L1PE_Pos | ACTLR_FW_Msk | 0x02);
	__ISB();
	__DSB();
#elif (__CORTEX_A == 7U)
	// set the ACTLR.SMP
	__set_ACTLR(__get_ACTLR() | ACTLR_SMP_Msk);
	__ISB();
	__DSB();
#endif /* (__CORTEX_A == 9U) */

	sysinit_fpu_initialize();
	sysinit_vbar_initialize();		// interrupt vectors relocate
	sysinit_ttbr_initialize();		// TODO: убрать работу с L2 для второго процессора - Загрузка TTBR, инвалидация кеш памяти и включение MMU

	arm_gic_initialize();
//	GIC_CPUInterfaceInit();
//#if WITHNESTEDINTERRUPTS
//	GIC_SetInterfacePriorityMask(gARM_BASEPRI_ALL_ENABLED);
//#endif /* WITHNESTEDINTERRUPTS */

	L1C_EnableCaches();
	L1C_EnableBTAC();
	__set_ACTLR(__get_ACTLR() | ACTLR_L1PE_Msk);	// Enable Dside prefetch
	__ISB();
	__DSB();
	#if (__L2C_PRESENT == 1)
	  // Enable Level 2 Cache
	  L2C_Enable();
	#endif

	cortexa_cpuinfo();
	__enable_irq();
	SPIN_UNLOCK(& cpu1init);

	// Idle loop
	for (;;)
	{
		__WFI();
	}
}

#endif /*  (__CORTEX_A != 0) */


void cpump_initialize(void)
{

#if (__CORTEX_A != 0) || (__CORTEX_A == 9U)
#if WITHSMPSYSTEM

#if (__CORTEX_A == 9U)
	// set the ACTLR.SMP
	// 0x02: L2 Prefetch hint enable
	__set_ACTLR(__get_ACTLR() | ACTLR_SMP_Msk | ACTLR_L1PE_Pos | ACTLR_FW_Msk | 0x02);
	__ISB();
	__DSB();
#elif (__CORTEX_A == 7U)
	// set the ACTLR.SMP
	// STM32MP1: already set
	__set_ACTLR(__get_ACTLR() | ACTLR_SMP_Msk);
	__ISB();
	__DSB();
#endif /* (__CORTEX_A == 9U) */

	cortexa_cpuinfo();
	SPINLOCK_INITIALIZE(& cpu1init);
	SPIN_LOCK(& cpu1init);
	cortexa_mp_cpu1_start((uintptr_t) Reset_CPU1_Handler);
	SPIN_LOCK(& cpu1init);
	SPIN_UNLOCK(& cpu1init);

#else /* WITHSMPSYSTEM */
	cortexa_cpuinfo();

#endif /* WITHSMPSYSTEM */
#endif /* (__CORTEX_A == 7U) || (__CORTEX_A == 9U) */

}

#if CPUSTYLE_ATSAM3S

static void 
arm_cpu_atsam3s_pll_initialize(void)
{
	// Disable Watchdog
	//WDT->WDT_MR = WDT_MR_WDDIS;

	// Embedded Flash Wait State VDDCORE set at 1.65V
	// 17 MHz - 1 cycle = FWS = 0
	// 30 MHz - 2 cycle = FWS = 1
	// 54 MHz - 3 cycle = FWS = 2
	// 64 MHz - 4 cycle = FWS = 3

	// Embedded Flash Wait State VDDCORE set at 1.80V
	// 32 MHz - 1 cycle = FWS = 0
	// 38 MHz - 2 cicle = FWS = 1
	// 64 MHz - 3 cycls = FWS = 2

#if CPU_FREQ == 64000000UL
	enum { OSC_MUL = 32, OSC_DIV = 3, FWS = 2, noneedpll = 0 };	// 12 MHz / 3 * 32 = 128 MHz
#elif CPU_FREQ == 48000000UL
	enum { OSC_MUL = 8, OSC_DIV = 1, FWS = 2, noneedpll = 0 };	// 12 MHz / 1 * 8 = 96 MHz
#elif CPU_FREQ == 32000000UL
	enum { OSC_MUL = 16, OSC_DIV = 3, FWS = 1, noneedpll = 0 };	// 12 MHz / 3 * 16 = 96 MHz
#else
	enum { OSC_MUL = 1, OSC_DIV = 1, FWS = 1, noneedpll = 1 };	// 12 MHz / 3 * 16 = 96 MHz
	//#error Unsupported CPU_FREQ value
#endif

	if (noneedpll)
	{
	}
	else if (1)
	{
		// умножение кварцевого генератора
		lowlevel_sam3s_init_pll_clock_xtal(OSC_MUL, OSC_DIV, FWS);
	}
	else if (0)
	{
		// умножение от внутреннего RC генератора
		lowlevel_sam3s_init_pll_clock_RC12(OSC_MUL, OSC_DIV, FWS);	
	}
}
#endif /* CPUSTYLE_ATSAM3S */

#if CPUSTYLE_ATSAM4S
static void 
arm_cpu_atsam4s_pll_initialize(void)
{
	// Disable Watchdog
	//WDT->WDT_MR = WDT_MR_WDDIS;

	// Embedded Flash Wait State VDDCORE set at 1.65V
	// 17 MHz - 1 cycle = FWS = 0
	// 30 MHz - 2 cycle = FWS = 1
	// 54 MHz - 3 cycle = FWS = 2
	// 64 MHz - 4 cycle = FWS = 3

	// Embedded Flash Wait State VDDCORE set at 1.80V
	// 32 MHz - 1 cycle = FWS = 0
	// 38 MHz - 2 cicle = FWS = 1
	// 64 MHz - 3 cycls = FWS = 2

#if CPU_FREQ == 120000000UL
	enum { OSC_MUL = 60, OSC_DIV = 3, FWS = 5, noneedpll = 0 };	// 12 MHz / 3 * 60 = 240 MHz
#elif CPU_FREQ == 112000000UL
	enum { OSC_MUL = 56, OSC_DIV = 3, FWS = 5, noneedpll = 0 };	// 12 MHz / 3 * 40 = 224 MHz
#elif CPU_FREQ == 104000000UL
	enum { OSC_MUL = 52, OSC_DIV = 3, FWS = 5, noneedpll = 0 };	// 12 MHz / 3 * 52 = 208 MHz
#elif CPU_FREQ == 96000000UL
	enum { OSC_MUL = 48, OSC_DIV = 3, FWS = 5, noneedpll = 0 };	// 12 MHz / 3 * 40 = 160 MHz
#elif CPU_FREQ == 80000000UL
	enum { OSC_MUL = 40, OSC_DIV = 3, FWS = 4, noneedpll = 0 };	// 12 MHz / 3 * 40 = 160 MHz
#elif CPU_FREQ == 64000000UL
	enum { OSC_MUL = 32, OSC_DIV = 3, FWS = 3, noneedpll = 0 };	// 12 MHz / 3 * 32 = 128 MHz
#elif CPU_FREQ == 48000000UL
	enum { OSC_MUL = 8, OSC_DIV = 1, FWS = 2, noneedpll = 0 };	// 12 MHz / 1 * 8 = 96 MHz
#elif CPU_FREQ == 32000000UL
	enum { OSC_MUL = 16, OSC_DIV = 3, FWS = 1, noneedpll = 0 };	// 12 MHz / 3 * 16 = 96 MHz
#else
	enum { OSC_MUL = 1, OSC_DIV = 1, FWS = 1, noneedpll = 1 };	// 12 MHz / 3 * 16 = 96 MHz
	//#error Unsupported CPU_FREQ value
#endif

	if (noneedpll)
	{
	}
	else if (1)
	{
		// умножение кварцевого генератора
		lowlevel_sam4s_init_pll_clock_xtal(OSC_MUL, OSC_DIV, FWS);
	}
	else if (0)
	{
		// умножение от внутреннего RC генератора
		lowlevel_sam4s_init_pll_clock_RC12(OSC_MUL, OSC_DIV, FWS);	
	}
}
#endif /* CPUSTYLE_ATSAM4S */

#if CPUSTYLE_ATXMEGA

static uint8_t CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_t clockSource )
{
	auto void CCPWrite(volatile uint8_t * address, uint8_t value)
	{
		volatile uint8_t * const tmpAddr = address;
		//system_disableIRQ();
	#ifdef RAMPZ
		RAMPZ = 0;
	#endif
		asm volatile(
			"movw r30,  %0"	      "\n\t"
			"ldi  r16,  %2"	      "\n\t"	// take signature to R16
			"out   %3, r16"	      "\n\t"	// write to CCP
			"st     Z,  %1"       "\n\t"	// write to desired register
			:
			: "r" (tmpAddr), "r" (value), "M" (CCP_IOREG_gc), "i" (& CCP)
			: "r16", "r30", "r31"
			);

		//system_enableIRQ();
	}

	const uint8_t clkCtrl = (CLK.CTRL & ~CLK_SCLKSEL_gm) | clockSource;
#if 0
	// Так нельзя, работоспособность кода зависит от оптимизаций.
	CCP = CCP_IOREG_gc;   	
	CLK.CTRL = clkCtrl;
#else
	CCPWrite(& CLK.CTRL, clkCtrl);
#endif
	return (CLK.CTRL & clockSource);
}


static void cpu_atxmega_switchto32MHz()
{
	// switch to 32 MHz
	OSC.CTRL |= OSC_RC32MEN_bm;
	while ((OSC.STATUS & OSC_RC32MRDY_bm ) == 0)
		;
	CLKSYS_Main_ClockSource_Select(CLK_SCLKSEL_RC32M_gc);
	OSC.CTRL &= ~ (OSC_RC2MEN_bm | OSC_RC32KEN_bm);
}

#endif /* CPUSTYLE_ATXMEGA */

#if CPUSTYPE_TMS320F2833X

void cpu_tms320f2833x_pll_initialize(
		uint_fast8_t pllcrDIV, 		// PLL multiplier
		uint_fast8_t pllstsDIVSEL)	// PLL divider (from PLL to CPU)
{
    /* check if running in Limp mode; if yes, abort */
    if (PLLSTS & PLLSTS_MCLKSTS_BIT) {
        //Boot_limpAbort();
    	for (;;)
    		;
    }

    /* set max divide select (DIVSEL = 0) */
    PLLSTS &= ~PLLSTS_DIVSEL_BITS;

    /* temporarily disable failed oscillator detect */
    PLLSTS |= PLLSTS_MCLKOFF_BIT;

    /* set the new PLL multiplier value */
    PLLCR = pllcrDIV;

    /* wait for the PLL to relock */
    while (!(PLLSTS & PLLSTS_PLLLOCKS_BIT)) {
    };

    /* re-enable failed oscillator detection */
    PLLSTS &= ~PLLSTS_MCLKOFF_BIT;

    /* set divide select bits (DIVSEL) */
    PLLSTS |= pllstsDIVSEL << PLLSTS_DIVSEL_SHIFTBITS;
}


#pragma CODE_SECTION(cpu_tms320f2833x_flash_waitstates, "ramfuncs")

static void
cpu_tms320f2833x_flash_waitstates(uint_fast8_t flashws, uint_fast8_t otpws)
{
	// To ensure the FLASH in high power mode
	FPWR = 0x003;
	asm(" RPT #8 || NOP");

	//Enable Flash Pipeline mode to improve performance
	//of code executed from Flash.
	//FlashRegs.FOPT.bit.ENPIPE = 1;
	FOPT |= 0x0001;
	asm(" RPT #8 || NOP");

	//                CAUTION
	//Minimum waitstates required for the flash operating
	//at a given CPU rate must be characterized by TI.
	//Refer to the datasheet for the latest information.

	//Set the Paged Waitstate for the Flash.
	//FlashRegs.FBANKWAIT.bit.PAGEWAIT = 3;
	FBANKWAIT = (FBANKWAIT & ~ 0x0f00) | (flashws << 8);
	asm(" RPT #8 || NOP");

	//Set the Random Waitstate for the Flash.
	//FlashRegs.FBANKWAIT.bit.RANDWAIT = 3;
	FBANKWAIT = (FBANKWAIT & ~ 0x000f) | (flashws << 0);
	asm(" RPT #8 || NOP");

	//Set the Waitstate for the OTP.
	//FlashRegs.FOTPWAIT.bit.OTPWAIT = 5;
	FOTPWAIT = (FOTPWAIT & ~ 0x000f) | (otpws << 0);
	asm(" RPT #8 || NOP");

	//                CAUTION
	//ONLY THE DEFAULT VALUE FOR THESE 2 REGISTERS SHOULD BE USED
	//FlashRegs.FSTDBYWAIT.bit.STDBYWAIT = 0x01FF;
	FSTDBYWAIT = (FSTDBYWAIT & ~ 0x01FF) | 0x01FF;
	asm(" RPT #8 || NOP");
	//FlashRegs.FACTIVEWAIT.bit.ACTIVEWAIT = 0x01FF;
	FACTIVEWAIT = (FACTIVEWAIT & ~ 0x01FF) | 0x01FF;
	asm(" RPT #8 || NOP");

	//Force a pipeline flush to ensure that the write to
	//the last register configured occurs before returning.

	asm(" RPT #8 || NOP");
}
#endif /* CPUSTYPE_TMS320F2833X */

// секция init больше не нужна
void cpu_initdone(void)
{
#if WITHISBOOTLOADER
	#if BOOTLOADER_RAMSIZE
		bootloader_copyapp(BOOTLOADER_RAMAREA);	/* копирование исполняемого образа (если есть) в требуемое место */
	#endif /* BOOTLOADER_RAMSIZE */
#if CPUSTYLE_R7S721

	if ((CPG.STBCR9 & CPG_STBCR9_BIT_MSTP93) == 0)
	{
#if 0
		// Когда загрузочный образ FPGA будт оставаться в SERIAL FLASH, запретить отключение.
		while ((SPIBSC0.CMNSR & (1u << 0)) == 0)	// TEND bit
			;

		SPIBSC0.CMNCR = (SPIBSC0.CMNCR & ~ ((1 << SPIBSC_CMNCR_BSZ))) |	// BSZ
			(1 << SPIBSC_CMNCR_BSZ_SHIFT) |
			0;
		(void) SPIBSC0.CMNCR;	/* Dummy read */

		// SPI multi-io Read Cache Flush
		SPIBSC0.DRCR |= (1u << SPIBSC_DRCR_RCF_SHIFT);	// RCF bit
		(void) SPIBSC0.DRCR;		/* Dummy read */

		local_delay_ms(50);

		SPIBSC0.SMCR = 0;
		(void) SPIBSC0.SMCR;	/* Dummy read */

#endif
		// spi multi-io hang off
		//CPG.STBCR9 |= CPG_STBCR9_BIT_MSTP93;	// Module Stop 93	- 1: Clock supply to channel 0 of the SPI multi I/O bus controller is halted.
		//(void) CPG.STBCR9;			/* Dummy read */
	}

#endif /* CPUSTYLE_R7S721 */

	SPIDF_HANGOFF();	// Отключить процессор от SERIAL FLASH

#endif /* WITHISBOOTLOADER */
}

// optimizer test: from electronix.ru - should be one divmod call
/*
uint8_t xxxxxpos(uint8_t num) // num = 0..8
{
    uint8_t pos = 0x40;
    while (num >= 3) {
        pos += 0x40;
        num -= 3;
    }
    pos += num * 7;
    return pos;
}

*/

#if CPUSTYLE_ARM

// Используется в случае наличия ключа ld -nostartfiles
// Так же смотреть вокруг software_init_hook

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern int main(void);
extern void __libc_init_array(void);

void __NO_RETURN _start(void)
{
	__libc_init_array();	// invoke constructors
    /* Branch to main function */
    main();

     /* Infinite loop */
	for (;;)
		;
}

// call after __preinit_array_xxx and before __init_array_xxx passing
void _init(void)
{
}

void * __dso_handle;

void _fini(void)
{
	for (;;)
		;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#if 1//WITHUSEMALLOC

/*
 *
 * ****************************
 */
#include <sys/stat.h>
#include <string.h>
#include <errno.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int SER_GetChar(void)
{

	return (-1);
}

/*-- GCC - Newlib runtime support --------------------------------------------*/

int __attribute__((used)) (_open)(const char * path, int flags, ...)
{
	return (-1);
}

int __attribute__((used)) (_close)(int fd) {
	return (-1);
}

int __attribute__((used)) (_lseek)(int fd, int ptr, int dir)
{
	return (0);
}

int __attribute__((used)) (_fstat)(int fd, struct stat * st)
{
	memset(st, 0, sizeof(*st));
	st->st_mode = S_IFCHR;
	return (0);
}

int __attribute__((used)) (_isatty)(int fd)
{
	return (1);
}

int __attribute__((used)) (_read)(int fd, char * ptr, int len) {
	char c;
	int i;

	for (i = 0; i < len; i++) {
		c = SER_GetChar();
		if (c == 0x0D)
			break;
		*ptr++ = c;
		dbg_putchar(c);
	}
	return (len - i);
}

int __attribute__((used)) (_write)(int fd, char * ptr, int len)
{
	int i;

	for (i = 0; i < len; i++)
		dbg_putchar(* ptr ++);
	return (i);
}

#if CPUSTYLE_STM32MP1 || (CPUSTYLE_XC7Z && ! WITHISBOOTLOADER)

	static RAMHEAP uint8_t heapplace [64 * 1024uL * 1024uL];

#else /* CPUSTYLE_STM32MP1 */

	static RAMHEAP uint8_t heapplace [8 * 1024uL];

#endif /* CPUSTYLE_STM32MP1 */

extern int __HeapBase;
extern int __HeapLimit;

caddr_t __attribute__((used)) (_sbrk)(int incr)
{
	static char * heap;
	char * prev_heap;

	if (heap == NULL)
	{
		heap = (char *) &__HeapBase;
	}

	//PRINTF(PSTR("_sbrk: incr=%X, new heap=%X, & __HeapBase=%p, & __HeapLimit=%p\n"), incr, heap + incr, & __HeapBase, & __HeapLimit);

	prev_heap = heap;

	if ((heap + incr) > (char *) &__HeapLimit)
	{
		//errno = ENOMEM;
		return (caddr_t) -1;
	}

	heap += incr;

	return (caddr_t) prev_heap;
}

// Corte-A9 reauire


void __attribute__((used)) (_exit)(int code)
{
	for (;;)
		;
}

int __attribute__((used)) (_kill)(int id)
{
	return (-1);
}

int __attribute__((used)) (_getpid)(int id)
{
	return (-1);
}

#if defined(RTC1_TYPE)

#include <time.h>
/* поддержка получения времени */
int _gettimeofday(struct timeval *p, void *tz)
{
	const time_t XSEC_PER_DAY = (time_t) 24 * 60 * 60;

	if (p != NULL)
	{
		// получаем локальное время в секундах из компонент
		uint_fast16_t year;
		uint_fast8_t month, dayofmonth;
		uint_fast8_t hour, minute, secounds;

		board_rtc_getdatetime( & year, & month, & dayofmonth, & hour, & minute, & secounds);

		static const unsigned int years [2] [13] =
		{
			{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 }, /* Normal */
			{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }, /* Leap   */
		};

//		unsigned int leap = 0;
//		float Leap_check = 0.0;

		/* сколько лет прошло с 1980 года? */
		/* делим разность текущего и 1980 годов на 4 */
		/* если результат деления - без остатка, то текущий год - високосный */
		/* если результат деления - с остатком, то текущий год -  нормальный */
//
//		Leap_check = ((float) ((year) - 1980)) / 4.; /* деление на предмет */
//		/* выявления остатка  */
//		leap = ((int) ((year) - 1980)) / 4; /* деление с обрезанием остатка*/
//
//		if (Leap_check == leap)
//			leap = 1; /* сравнение (leap=1 - вис.год) */
//
//		else
//			leap = 0;

		const div_t d = div(year - 1980, 4);
		const int leap = d.rem == 0;

		/* число секунд в этих годах
		 SEC_PER_DAY*(((year)-1980)*365 + ((year)-1980)/4 + 0\1);

		 сколько секунд в предыдущих месяцах текущего года?
		 SEC_PER_DAY*years[leap][(month)-1];

		 сколько секунд прошло за предыдущие дни текущего месяца?
		 SEC_PER_DAY*((day)-1);

		 сколько секунд прошло за предыдущие часы текущего дня?
		 3600*(hour);

		 сколько секунд прошло за предыдущие минуты текущего часа?
		 60*(min);

		 число отдельных секунд
		 (sec);    */

		if (leap != 0)
		{
			p->tv_sec = XSEC_PER_DAY * (((year) - 1980) * 365 + ((year) - 1980) / 4 + years [leap] [(month) - 1] + ((dayofmonth) - 1))
					+ 3600 * (hour) + 60 * (minute) + (secounds);
		}
		else
		{
			p->tv_sec = XSEC_PER_DAY
					* (((year) - 1980) * 365 + ((year) - 1980) / 4 + 1 + years [leap] [(month) - 1] + ((dayofmonth) - 1))
					+ 3600 * (hour) + 60 * (minute) + (secounds);
		}
		p->tv_usec = 0;
	}

	return 0;
}

#else

int _gettimeofday(struct timeval *p, void *tz)
{
	if (p != NULL)
	{
	}
	return 0;
}

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// at all

#endif /* CPUSTYLE_ARM */

