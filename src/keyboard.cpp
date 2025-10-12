/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "keyboard.h"
#include "buffers.h"
#include "encoder.h"
#include "board.h"

#include <atomic>

#if WITHKEYBOARD


enum 
{
	KBD_BEEP_LENGTH =			KBDNTICKS(25),	// длительность озвучивания нажатия

	KBD_STABIL_PRESS =			KBDNTICKS(15),	// время для регистраци нажатия
	KBD_STABIL_RELEASE =		KBDNTICKS(15),	// время для регистраци отпускания

	KBD_MAX_PRESS_DELAY_LONG =	KBDNTICKS(600), // время для регистрации удержания кнопки с медленным автоповтором
	KBD_MAX_PRESS_DELAY_POWER =	KBDNTICKS(2000), // время для регистрации удержания кнопки выключения питания
	KBD_PRESS_REPEAT_SLOW =		KBDNTICKS(400),	// время между символами по медленному автоповтору

	KBD_MAX_PRESS_DELAY_LONG4 =	KBDNTICKS(200), // RK4CI:1600 время для регистрации удержания кнопки с медленным автоповтором
	KBD_PRESS_REPEAT_SLOW4 =	KBDNTICKS(100),	// время между символами по медленному автоповтору

	//KBD_MED_STAGE1 = KBDNTICKS(200),			// моменты, на которых вырабатывается очередной символ клавиши с быстрым автоповтором
	//KBD_MED_STAGE2 = KBDNTICKS(300),
	//KBD_MED_STAGE3 = KBDNTICKS(400),
	//KBD_MED_STAGE4 = KBDNTICKS(500),
	KBD_TIME_SWITCH_QUICK = KBDNTICKS(200), // с этого времени начинается быстрый автоповтор
	KBD_PRESS_REPEAT_QUICK1 =	KBDNTICKS(20),	// время между символами по быстрому автоповтору
	KBD_PRESS_REPEAT_QUICK2 =	KBDNTICKS(5)	// время между символами по очень быстрому автоповтору
};

static void kbd_spool(void * ctx);

typedef struct kbdst_tag
{
	// сделаны по 8 бит - при 200 герц прерываний 600 мс всего 120 тиков.
	std::atomic<unsigned> kbd_press;	/* время с момента нажатия */
	uint_fast16_t kbd_release;		/* время после отпускания - запрет нового нажатия */

	uint_fast8_t kbd_last;	/* последний скан-код (возврат при отпускании кнопки) */
	uint_fast8_t kbd_slowcount; /* количество сгенерированных символов с медленным автоповтором */

	uint_fast8_t kbd_beep;	/* время с момента нажатия */

	volatile uint_fast8_t kbd_ready;

	u8queue_t kbdq;

	ticker_t kbdticker;
	adcdone_t aevent;
	uint8_t kbdfifo [16];
	uint_fast8_t (* get_pressed_key)(void);
	const struct qmkey * qmdefsp;
} kbdst_t;

static kbdst_t kbd0;
#if WITHSUBTONES
static kbdst_t dtmf_kbd;
#endif /* WITHSUBTONES */

static void kbdst_initialize(kbdst_t * kbdp, uint_fast8_t (* get_pressed_key_cb)(void), const struct qmkey * aqmdefsp)
{
	kbdp->qmdefsp = aqmdefsp;
	kbdp->get_pressed_key = get_pressed_key_cb;
	kbdp->kbd_press = 0;	/* время с момента нажатия */
	kbdp->kbd_release = 0;		/* время после отпускания - запрет нового нажатия */

	kbdp->kbd_last = 0;	/* последний скан-код (возврат при отпускании кнопки) */
	kbdp->kbd_slowcount = 0; /* количество сгенерированных символов с медленным автоповтором */

	kbdp->kbd_beep = 0;	/* время с момента нажатия */

	kbdp->kbd_ready = 0;

	uint8_queue_init(& kbdp->kbdq, kbdp->kbdfifo, sizeof kbdp->kbdfifo / sizeof kbdp->kbdfifo [0]);

	kbdp->kbd_last = kbdp->get_pressed_key();
	if (kbdp->kbd_last != KEYBOARD_NOKEY)
	{
		// самое первое нажатие
		kbdp->kbd_press = (kbdp->qmdefsp [kbdp->kbd_last].flags & KIF_POWER) ? (KBD_MAX_PRESS_DELAY_LONG + 1) : 1;	// длинное нажатие уже зарегистрировано
		kbdp->kbd_release = 0;
		kbdp->kbd_slowcount = 0;
	}

#if WITHCPUADCHW
	adcdone_initialize(& kbdp->aevent, kbd_spool, kbdp);
	adcdone_add(& kbdp->aevent);
#else /* KEYBOARD_USE_ADC */
	ticker_initialize(& kbdp->kbdticker, NTICKS(KBD_TICKS_PERIOD), kbd_spool, kbdp);
	ticker_add(& kbdp->kbdticker);
#endif /* KEYBOARD_USE_ADC */
}

/* получение скан-кода клавиши или 0 в случае отсутствия.
 * если клавиша удержана, возвращается скан-код из соответствующего поля массива структур kbdp->qmdefsp
 * ОТЛАЖИВАЕТСЯ
 */
static uint_fast8_t
kbd_scan_local(kbdst_t * kbdp, uint_fast8_t * key)
{
	const uint_fast8_t chinp = kbdp->get_pressed_key();
	const uint_fast8_t notstab = (kbdp->kbd_press < (KBD_STABIL_PRESS + 1));

	if (chinp != KEYBOARD_NOKEY)
	{
		if (/*kbd_release != 0 && */ ! notstab && (kbdp->kbd_last != chinp))		// клавиша сменилась в состоянии стабильного нажатия
		{
			// самое первое нажатие
			kbdp->kbd_last = chinp;
			kbdp->kbd_press = 1;
			kbdp->kbd_release = 0;
			kbdp->kbd_slowcount = 0;
			//dbg_putchar('t');
			return 0;
		}
		else if (kbdp->kbd_release != 0 && ! notstab)
		{
			// Уже было застабилизировавшееся значение - не меняем ничего.
			// kbd_last уже содержит правильное значение
			// Нажатие должно исчезнуть когда-то наконец.
			//dbg_putchar('k');
		}
		else if (kbdp->kbd_press == 0)
		{
			// самое первое нажатие
			kbdp->kbd_last = chinp;
			kbdp->kbd_press = 1;
			kbdp->kbd_release = 0;
			kbdp->kbd_slowcount = 0;
			//dbg_putchar('l');
			return 0;
		}	
		else if (notstab && (kbdp->kbd_last != chinp))		// Ожидание стабилизации кода клавиши
		{	
			kbdp->kbd_last = chinp;
			kbdp->kbd_press = 1;
			kbdp->kbd_release = 0;
			kbdp->kbd_slowcount = 0;
			//dbg_putchar('m');
			return 0;
		}

		kbdp->kbd_release = KBD_STABIL_RELEASE;
		
		const uint_fast8_t flags = kbdp->qmdefsp [kbdp->kbd_last].flags;
		/* сравнение кодов клавиш, для которых допустим медленный автоповтор при длительном удержании */
		if ((flags & KIF_SLOW) != 0)	//(is_slow_repeat(kbd_last))
		{
			// клавиша может работать с медленным автоповтором
			switch (++ kbdp->kbd_press)
			{
			case KBD_MAX_PRESS_DELAY_LONG + KBD_PRESS_REPEAT_SLOW:
			kbdp->kbd_press = KBD_MAX_PRESS_DELAY_LONG;	// позволяем ещё раз сюда попасть.
				// @suppress("No break at end of case")

			case KBD_MAX_PRESS_DELAY_LONG:
				* key = kbdp->qmdefsp [kbdp->kbd_last].code;
				kbdp->kbd_release = 0;
				return 1;
			}
			return 0;
		}
		/* сравнение кодов клавиш, для которых допустим медленный автоповтор при длительном удержании */
		else if ((flags & KIF_SLOW4) != 0)	//(is_slow_repeat(kbd_last))
		{
			// клавиша может работать с медленным автоповтором
			switch (++ kbdp->kbd_press)
			{
			case KBD_MAX_PRESS_DELAY_LONG4 + KBD_PRESS_REPEAT_SLOW4:
			kbdp->kbd_press = KBD_MAX_PRESS_DELAY_LONG4;	// позволяем ещё раз сюда попасть.
	    		// @suppress("No break at end of case")

			case KBD_MAX_PRESS_DELAY_LONG4:
				* key = kbdp->qmdefsp [kbdp->kbd_last].code;
				kbdp->kbd_release = 0;
				return 1;
			}
			return 0;
		}
		/* сравнение кодов клавиш, для которых допустим быстрый автоповтор при длительном удержании */
		else if ((flags & KIF_FAST) != 0)	//(is_fast_repeat(kbd_last))
		{
#if WITHKBDENCODER
			// клавиша может работать с быстрым автоповтором
			// Перестройка клавишами вместо валкодера
			switch (++ kbdp->kbd_press)
			{
			case KBD_TIME_SWITCH_QUICK:
				if (kbdp->kbd_slowcount < 20)
				{
					//++ kbd_slowcount;	// закомментировано - никогда не ускоряемся
					kbdp->kbd_press = KBD_TIME_SWITCH_QUICK - KBD_PRESS_REPEAT_QUICK1;
				}
				else
					kbdp->kbd_press = KBD_TIME_SWITCH_QUICK - KBD_PRESS_REPEAT_QUICK2;
				// формирование символа в автоповторе
				encoder_kbdctl(kbdp->qmdefsp [kbdp->kbd_last].code, 1);
				//dbg_putchar('R');
				//dbg_putchar('0' + kbd_last);
				break;
			default:
				//dbg_putchar('U');
				break;
			}
			return 0;
#endif /* WITHKBDENCODER */
		}
		else if ((flags & KIF_POWER) != 0)
		{
			// клавиша может работать с длинным нажатием
			if (kbdp->kbd_press == KBD_MAX_PRESS_DELAY_POWER)
				return 0;	// lond_press symbol already returned
			if (kbdp->kbd_press < KBD_MAX_PRESS_DELAY_POWER)
			{
				if (++ kbdp->kbd_press == KBD_MAX_PRESS_DELAY_POWER)
				{
					* key = kbdp->qmdefsp [kbdp->kbd_last].holded; // lond_press symbol
					//
					return 1;
				}
			}
			return 0;
		}
		else
		{
			// клавиша может работать с длинным нажатием
			if (kbdp->kbd_press == KBD_MAX_PRESS_DELAY_LONG)
				return 0;	// lond_press symbol already returned
			if (kbdp->kbd_press < KBD_MAX_PRESS_DELAY_LONG)
			{
				if (++ kbdp->kbd_press == KBD_MAX_PRESS_DELAY_LONG)
				{	
					* key = kbdp->qmdefsp [kbdp->kbd_last].holded; // lond_press symbol
					//
					return 1;
				}
			}
		}
		return 0;

	}
	else if (kbdp->kbd_release != 0) // Нет нажатой клавишии - было нажатие
	{
		if (notstab)
		{
			kbdp->kbd_press = 0;		// слишком короткие нажатия игнорируем
			kbdp->kbd_release = 0;
			//dbg_putchar('J');
			return 0;
		}
		//dbg_putchar('r');
		// keyboard keys released, time is not expire.
		if (-- kbdp->kbd_release == 0)
		{
			const uint_fast8_t flags = kbdp->qmdefsp [kbdp->kbd_last].flags;
			// time is expire
			if ((flags & KIF_FAST) != 0)
			{
#if WITHKBDENCODER
				// Перестройка клавишами вместо валкодера
				//if (kbd_press < KBD_MED_STAGE1)
				if (kbdp->kbd_slowcount == 0)
				{
					encoder_kbdctl(kbdp->qmdefsp [kbdp->kbd_last].code, 0);
					//dbg_putchar('Q');
					//dbg_putchar('0' + kbd_last);
				}
				//else
				//	dbg_putchar('F');

				kbdp->kbd_press = 0;
				kbdp->kbd_slowcount = 0;
				return 0;		// уже было срабатывание по быстрому автоповтору
#endif /* WITHKBDENCODER */
			}
			else if (kbdp->kbd_press < KBD_MAX_PRESS_DELAY_LONG)
			{
				* key = kbdp->qmdefsp [kbdp->kbd_last].code;
				kbdp->kbd_press = 0;
				kbdp->kbd_slowcount = 0;

				return 1;		// срабатывание по кратковременному нажатию на клавишу.
			}
			else
			{
				kbdp->kbd_press = 0;
				kbdp->kbd_slowcount = 0;

				return 0;		// уже было срабатывание по автоповтору или по длинному нажатию.
			}
		}
		return 0;

	}
	else // нет нажатой клавиши и небыло нажатичя перед этим
		return 0;
}

static IRQLSPINLOCK_t irqllock = IRQLSPINLOCK_INIT;

// вызывается с частотой TICKS_FREQUENCY герц
// после завершения полного цикла ADC по всем входам.
static void
kbd_spool(void * ctx)
{
	kbdst_t * const kbdp = (kbdst_t *) ctx;
	uint_fast8_t code;
	if (kbd_scan_local(kbdp, & code) != 0)
	{
		kbdp->kbd_beep = KBD_BEEP_LENGTH;
		board_keybeep_enable(1);	/* начать формирование звукового сигнала */

		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllock, & oldIrql, IRQL_SYSTEM);

		uint8_queue_put(& kbdp->kbdq, code);

		IRQLSPIN_UNLOCK(& irqllock, oldIrql);
	}
	else
	{
		if (kbdp->kbd_beep != 0 && -- kbdp->kbd_beep == 0)
			board_keybeep_enable(0);
	}
	kbdp->kbd_ready = 1;
}

static uint_fast8_t
kbdx_is_tready(kbdst_t * kbdp)
{
#if CPUSTYLE_STM32F30X

	// Пока на этих процессорах не запущен АЦП
	#warning TODO: remove this
	return 1;

#elif KEYBOARD_USE_ADC

	uint_fast8_t f;

	f = kbdp->kbd_ready;
	return f;

#else /* WITHCPUADCHW */
	// Конфигурация без АЦП
	return 1;
#endif /* WITHCPUADCHW */
}

uint_fast8_t
kbd_is_tready(void)
{
	return kbdx_is_tready(& kbd0);
}
/* Проверка, нажата ли клавиша c указанным флагом
// KIF_ERASE или KIF_EXTMENU
 */
uint_fast8_t kbdx_get_ishold(kbdst_t * kbdp, uint_fast8_t flag)
{
	uint_fast8_t r;
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& irqllock, & oldIrql, IRQL_SYSTEM);
	uint_fast8_t f = !! kbdp->kbd_press;

	r = f && (kbdp->qmdefsp [kbdp->kbd_last].flags & flag);
	IRQLSPIN_UNLOCK(& irqllock, oldIrql);
	return r;
}

static uint_fast8_t kbdx_scan(kbdst_t * kbdp, uint_fast8_t * v)
{
	uint_fast8_t f;
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& irqllock, & oldIrql, IRQL_SYSTEM);

	f = uint8_queue_get(& kbdp->kbdq, v);

	IRQLSPIN_UNLOCK(& irqllock, oldIrql);
	return f;
}

void kbdx_pass(kbdst_t * kbdp)
{
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& irqllock, & oldIrql, IRQL_SYSTEM);

	kbd_spool(kbdp);

	IRQLSPIN_UNLOCK(& irqllock, oldIrql);
}

uint_fast8_t kbd_scan(uint_fast8_t * v)
{
	return kbdx_scan(& kbd0, v);
}

uint_fast8_t dtmf_scan(uint_fast8_t * v)
{
#if WITHSUBTONES
	return kbdx_scan(& dtmf_kbd, v);
#else /* WITHSUBTONES */
	return 0;
#endif /* WITHSUBTONES */
}

void kbd_pass(void)
{
	kbdx_pass(& kbd0);
}

uint_fast8_t kbd_get_ishold(uint_fast8_t flag)
{
#if WITHBBOX
	return 0;
#else /* WITHBBOX */
	return kbdx_get_ishold(& kbd0, flag);
#endif /* WITHBBOX */
}

uint_fast8_t
static dummy_get_pressed_key(void)
{
	return KEYBOARD_NOKEY;
}

static const struct qmkey dtmf_qmdefs [] =
{
	{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX, 			' ', },
};

/* инициализация переменных работы с клавиатурой */
void kbd_initialize(void)
{
#if WITHBBOX
	kbdst_initialize(& kbd0, dummy_get_pressed_key, qmdefs);
#else /* WITHBBOX */
	kbdst_initialize(& kbd0, board_get_pressed_key, qmdefs);
#endif /* WITHBBOX */
#if WITHSUBTONES
	kbdst_initialize(& dtmf_kbd, dtmf_get_pressed_key, dtmf_qmdefs);
#endif /* WITHSUBTONES */

	IRQLSPINLOCK_INITIALIZE(& irqllock);



	// todo: все присвоения нулями могут быть убраны.

	////kbd_press = 0;
	////kbd_release = 0;
	////kbd_repeat = 0;
}

#else /* WITHKEYBOARD */

/* инициализация переменных работы с клавиатурой */
void kbd_initialize(void)
{
}

uint_fast8_t kbd_is_tready(void)
{
	return 1;
}

uint_fast8_t kbd_get_ishold(uint_fast8_t flag)
{
	return 0;
}

uint_fast8_t kbd_scan(uint_fast8_t * v)
{
	return 0;
}

uint_fast8_t dtmf_scan(uint_fast8_t * v)
{
	return 0;
}

void kbd_pass(void)
{
}

#endif /* WITHKEYBOARD */
