/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "buffers.h"
#include "keyboard.h"
#include "encoder.h"
#include "board.h"

#if WITHKEYBOARD
enum 
{
	KBD_BEEP_LENGTH =			NTICKS(25),	// длительность озвучивания нажатия

	KBD_STABIL_PRESS =			NTICKS(15),	// время для регистраци нажатия
	KBD_STABIL_RELEASE =		NTICKS(15),	// время для регистраци отпускания

	KBD_MAX_PRESS_DELAY_LONG =	NTICKS(600), // RK4CI:1600 время для регистрации удержания кнопки с медленным автоповтором
	KBD_PRESS_REPEAT_SLOW =		NTICKS(400),	// время между символами по медленному автоповтору

	KBD_MAX_PRESS_DELAY_LONG4 =	NTICKS(200), // RK4CI:1600 время для регистрации удержания кнопки с медленным автоповтором
	KBD_PRESS_REPEAT_SLOW4 =	NTICKS(100),	// время между символами по медленному автоповтору

	//KBD_MED_STAGE1 = NTICKS(200),			// моменты, на которых вырабатывается очередной символ клавиши с быстрым автоповтором
	//KBD_MED_STAGE2 = NTICKS(300),
	//KBD_MED_STAGE3 = NTICKS(400),
	//KBD_MED_STAGE4 = NTICKS(500),
	KBD_TIME_SWITCH_QUICK = NTICKS(200), // с этого времени начинается быстрый автоповтор
	KBD_PRESS_REPEAT_QUICK1 =	NTICKS(20),	// время между символами по быстрому автоповтору
	KBD_PRESS_REPEAT_QUICK2 =	NTICKS(5)	// время между символами по очень быстрому автоповтору
};


static uint_fast8_t
getstablev8(const volatile uint_fast8_t * p)
{
	uint_fast8_t v1 = * p;
	uint_fast8_t v2;
	do
	{
		v2 = v1;
		v1 = * p;
	} while (v2 != v1);
	return v1;

}

static uint_fast16_t
getstablev16(const volatile uint_fast16_t * p)
{
	uint_fast8_t v1 = * p;
	uint_fast8_t v2;
	do
	{
		v2 = v1;
		v1 = * p;
	} while (v2 != v1);
	return v1;

}

// сделаны по 8 бит - при 200 герц прерываний 600 мс всего 120 тиков.
static volatile uint_fast16_t kbd_press;	/* время с момента нажатия */
static uint_fast16_t kbd_release;		/* время после отпускания - запрет нового нажатия */

static uint_fast8_t kbd_last;	/* последний скан-код (возврат при отпускании кнопки) */
static uint_fast8_t kbd_slowcount; /* количество сгенерированных символов с медленным автоповтором */

static uint_fast8_t kbd_beep;	/* время с момента нажатия */

/* получение скан-кода клавиши или 0 в случае отсутствия.
 * если клавиша удержана, возвращается скан-код из соответствующего поля массива структур qmdefs
 * ОТЛАЖИВАЕТСЯ
 */
static uint_fast8_t
kbd_scan_local(uint_fast8_t * key)
{
	const uint_fast8_t chinp = board_get_pressed_key();
	const uint_fast8_t notstab = (kbd_press < (KBD_STABIL_PRESS + 1));

	if (chinp != KEYBOARD_NOKEY)
	{
		if (/*kbd_release != 0 && */ ! notstab && (kbd_last != chinp))		// клавиша сменилась в состоянии стабильного нажатия
		{
			// самое первое нажатие
			kbd_last = chinp;
			kbd_press = 1;
			kbd_release = 0;
			kbd_slowcount = 0;
			//dbg_putchar('t');
			return 0;
		}
		else if (kbd_release != 0 && ! notstab)
		{
			// Уже было застабилизировавшееся значение - не меняем ничего.
			// kbd_last уже содержит правильное значение
			// Нажатие должно исчезнуть когда-то наконец.
			//dbg_putchar('k');
		}
		else if (kbd_press == 0)
		{
			// самое первое нажатие
			kbd_last = chinp;
			kbd_press = 1;
			kbd_release = 0;
			kbd_slowcount = 0;
			//dbg_putchar('l');
			return 0;
		}	
		else if (notstab && (kbd_last != chinp))		// Ожидание стабилизации кода клавиши
		{	
			kbd_last = chinp;
			kbd_press = 1;
			kbd_release = 0;
			kbd_slowcount = 0;
			//dbg_putchar('m');
			return 0;
		}

		kbd_release = KBD_STABIL_RELEASE;
		
		const uint_fast8_t flags = qmdefs [kbd_last].flags;
		/* сравнение кодов клавиш, для которых допустим медленный автоповтор при длительном удержании */
		if ((flags & KIF_SLOW) != 0)	//(is_slow_repeat(kbd_last))
		{
			// клавиша может работать с медленным автоповтором
			switch (++ kbd_press)
			{
			case KBD_MAX_PRESS_DELAY_LONG + KBD_PRESS_REPEAT_SLOW:
				kbd_press = KBD_MAX_PRESS_DELAY_LONG;	// позволяем ещё раз сюда попасть.

			case KBD_MAX_PRESS_DELAY_LONG:
				* key = qmdefs [kbd_last].code;	
				kbd_release = 0;
				return 1;
			}
			return 0;
		}
		/* сравнение кодов клавиш, для которых допустим медленный автоповтор при длительном удержании */
		else if ((flags & KIF_SLOW4) != 0)	//(is_slow_repeat(kbd_last))
		{
			// клавиша может работать с медленным автоповтором
			switch (++ kbd_press)
			{
			case KBD_MAX_PRESS_DELAY_LONG4 + KBD_PRESS_REPEAT_SLOW4:
				kbd_press = KBD_MAX_PRESS_DELAY_LONG4;	// позволяем ещё раз сюда попасть.

			case KBD_MAX_PRESS_DELAY_LONG4:
				* key = qmdefs [kbd_last].code;	
				kbd_release = 0;
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
			switch (++ kbd_press)
			{
			case KBD_TIME_SWITCH_QUICK:
				if (kbd_slowcount < 20)
				{
					//++ kbd_slowcount;	// закомментировано - никогда не ускоряемся
					kbd_press = KBD_TIME_SWITCH_QUICK - KBD_PRESS_REPEAT_QUICK1;
				}
				else
					kbd_press = KBD_TIME_SWITCH_QUICK - KBD_PRESS_REPEAT_QUICK2;
				// формирование символа в автоповторе
				encoder_kbdctl(qmdefs [kbd_last].code, 1);
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
			// клавиша реагирует только после окончания длительного нажатия
			if (kbd_press < KBD_MAX_PRESS_DELAY_LONG)
				++ kbd_press;
			return 0;	// код будет возвращен по отпусканию
		}
		else
		{
			// клавиша может работать с длинным нажатием
			if (kbd_press == KBD_MAX_PRESS_DELAY_LONG)
				return 0;	// lond_press symbol already returned
			if (kbd_press < KBD_MAX_PRESS_DELAY_LONG)
			{
				if (++ kbd_press == KBD_MAX_PRESS_DELAY_LONG)
				{	
					* key = qmdefs [kbd_last].holded; // lond_press symbol
					//
					return 1;
				}
			}
		}
		return 0;

	}
	else if (kbd_release != 0) // Нет нажатой клавишии - было нажатие
	{
		if (notstab)
		{
			kbd_press = 0;		// слишком короткие нажатия игнорируем
			kbd_release = 0;
			//dbg_putchar('J');
			return 0;
		}
		//dbg_putchar('r');
		// keyboard keys released, time is not expire.
		if (-- kbd_release == 0)
		{
			// time is expire
			if ((qmdefs [kbd_last].flags & KIF_FAST) != 0)
			{
#if WITHKBDENCODER
				// Перестройка клавишами вместо валкодера
				//if (kbd_press < KBD_MED_STAGE1)
				if (kbd_slowcount == 0)
				{
					encoder_kbdctl(qmdefs [kbd_last].code, 0);
					//dbg_putchar('Q');
					//dbg_putchar('0' + kbd_last);
				}
				//else
				//	dbg_putchar('F');

				kbd_press = 0;
				kbd_slowcount = 0;
				return 0;		// уже было срабатывание по быстрому автоповтору
#endif /* WITHKBDENCODER */
			}
			else if (kbd_press < KBD_MAX_PRESS_DELAY_LONG)
			{
				* key = qmdefs [kbd_last].code;
				kbd_press = 0;
				kbd_slowcount = 0;

				return 1;		// срабатывание по кратковременному нажатию на клавишу.
			}
			else if ((qmdefs [kbd_last].flags & KIF_POWER) != 0)
			{
				// клавиша реагирует только после окончания длительного нажатия
				* key = qmdefs [kbd_last].code;	
				kbd_press = 0;
				kbd_slowcount = 0;

				return 1;		// срабатывание по длинному нажатию на клавишу.
			}
			else
			{
				kbd_press = 0;
				kbd_slowcount = 0;

				return 0;		// уже было срабатывание по автоповтору или по длинному нажатию.
			}
		}
		return 0;

	}
	else // нет нажатой клавиши и небыло нажатичя перед этим
		return 0;
}

static IRQLSPINLOCK_t irqllock;
static volatile uint_fast8_t kbd_ready;

static u8queue_t kbdq;

// вызывается с частотой TICKS_FREQUENCY герц
// после завершения полного цикла ADC по всем входам.
static void
kbd_spool(void * ctx)
{
	uint_fast8_t code;
	if (kbd_scan_local(& code) != 0)
	{
		kbd_beep = KBD_BEEP_LENGTH;
		board_keybeep_enable(1);	/* начать формирование звукового сигнала */

#if ! WITHBBOX
		IRQL_t oldIrql;
		IRQLSPIN_LOCK(& irqllock, & oldIrql);

		uint8_queue_put(& kbdq, code);

		IRQLSPIN_UNLOCK(& irqllock, oldIrql);
#endif /* ! WITHBBOX */
	}
	else
	{
		if (kbd_beep != 0 && -- kbd_beep == 0)
			board_keybeep_enable(0);
	}
	kbd_ready = 1;
}

uint_fast8_t 
kbd_is_tready(void)
{
#if CPUSTYLE_STM32F30X

	// Пока на этих процессорах не запущен АЦП
	#warning TODO: remove this
	return 1;

#elif WITHCPUADCHW && KEYBOARD_USE_ADC

	uint_fast8_t f;

	f = getstablev8(& kbd_ready);
	return f;

#else /* WITHCPUADCHW */
	// Конфигурация без АЦП
	return 1;
#endif /* WITHCPUADCHW */
}

/* Проверка, нажата ли клавиша c указанным флагом
// KIF_ERASE или KIF_EXTMENU
 */
uint_fast8_t kbd_get_ishold(uint_fast8_t flag)
{
#if WITHBBOX
	return 0;
#else /* WITHBBOX */
	uint_fast8_t r;
	uint_fast16_t f = getstablev16(& kbd_press);

	r = (f != 0) && (qmdefs [kbd_last].flags & flag);
	return r;
#endif /* WITHBBOX */
}

uint_fast8_t kbd_scan(uint_fast8_t * v)
{
	uint_fast8_t f;
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& irqllock, & oldIrql);

	f = uint8_queue_get(& kbdq, v);

	IRQLSPIN_UNLOCK(& irqllock, oldIrql);
	return f;
}

void kbd_pass(void)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);

	kbd_spool(NULL);

	LowerIrql(oldIrql);
}

/* инициализация переменных работы с клавиатурой */
void kbd_initialize(void)
{
	static ticker_t kbdticker;
	static adcdone_t aevent;

	IRQLSPINLOCK_INITIALIZE(& irqllock, IRQL_SYSTEM);


	static uint8_t kbdfifo [16];

	uint8_queue_init(& kbdq, kbdfifo, sizeof kbdfifo / sizeof kbdfifo [0]);

	// todo: все присвоения нулями могут быть убраны.

	////kbd_press = 0;
	////kbd_release = 0;
	////kbd_repeat = 0;
#if KEYBOARD_USE_ADC
	adcdone_initialize(& aevent, kbd_spool, NULL);
	adcdone_add(& aevent);
#else /* KEYBOARD_USE_ADC */
	ticker_initialize(& kbdticker, 1, kbd_spool, NULL);
	ticker_add(& kbdticker);
#endif /* KEYBOARD_USE_ADC */
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


void kbd_pass(void)
{
}

#endif /* WITHKEYBOARD */
