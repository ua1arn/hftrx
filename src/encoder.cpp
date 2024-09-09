/* $Id$ */
//
// Rotary encoders support
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "encoder.h"
#include "keyboard.h"	/* функция опроса электронного ключа */
//#include "display.h"	/* test */
#include "formats.h"	// for debug prints

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

/* обработчики прерывания от валкодера */

#if WITHHARDINTERLOCK
static volatile uint32_t position1;		/* накопитель от валкодера - знаковое число */
static volatile uint32_t position2;		/* накопитель от валкодера - знаковое число */
#else /* WITHHARDINTERLOCK */
static volatile int position1;		/* накопитель от валкодера - знаковое число */
static volatile int position2;		/* накопитель от валкодера - знаковое число */
#endif /* WITHHARDINTERLOCK */
#if WITHKBDENCODER
static int position_kbd;	/* накопитель от клавиатуры - знаковое число */
#endif /* WITHKBDENCODER */

void encoder_initialize(encoder_t * e, uint_fast8_t (* agetpins)(void))
{
	e->old_val = agetpins();
	e->getpins = agetpins;
	e->position = 0;
	e->backup_position = 0;
	e->reverse = 0;

	e->backup_rotate = 0;
	e->rotate = 0;
	e->enchistindex = 0;
	e->enchist [0] = e->enchist [1] = e->enchist [2] = e->enchist [3] = 0;
	ASSERT(HISTLEN == 4);

	IRQLSPINLOCK_INITIALIZE(& e->enclock);
	IRQLSPINLOCK_INITIALIZE(& e->encspeedlock);
}

/* прерывание по изменению сигнала на входе A от валкодера - направление по B */
void spool_encinterrupts4(void * ctx)
{
	encoder_t * const e = (encoder_t *) ctx;
	const int_fast8_t step = (e->getpins() & GETENCBIT_B) ? - 1 : + 1;	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& e->enclock, & oldIrql, ENCODER_IRQL);

	if (e->reverse)
		e->position -= step;
	else
		e->position += step;

	IRQLSPIN_UNLOCK(& e->enclock, oldIrql);
}

/* прерывание по изменению сигнала на входах от валкодера */
void spool_encinterrupts(void * ctx)
{
	encoder_t * const e = (encoder_t *) ctx;
	// dimensions are:
	// old_bits new_bits
	static int8_t graydecoder [4][4] =
	{
		{
			+0,		/* 00 -> 00 stopped				*/
			-1,		/* 00 -> 01 rotate left			*/
			+1,		/* 00 -> 10 rotate right		*/
			+0,		/* 00 -> 11 invalid combination */
		},
		{
			+1,		/* 01 -> 00 rotate right		*/
			+0,		/* 01 -> 01 stopped				*/
			+0,		/* 01 -> 10 invalid combination */
			-1,		/* 01 -> 11 rotate left			*/
		},
		{
			-1,		/* 10 -> 00 rotate left			*/
			+0,		/* 10 -> 01 invalid combination */
			+0,		/* 10 -> 10 stopped				*/
			+1,		/* 10 -> 11 rotate right		*/
		},
		{
			+0,		/* 11 -> 00 invalid combination */
			+1,		/* 11 -> 01 rotate right		*/
			-1,		/* 11 -> 10 rotate left			*/
			+0,		/* 11 -> 11 stopped				*/
		},
	};
	// GETENCBIT_A, GETENCBIT_B
	const uint_fast8_t new_val = e->getpins();	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
	const int delta = graydecoder [e->old_val][new_val];
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& e->enclock, & oldIrql, ENCODER_IRQL);
	if (e->reverse)
		e->position -= delta;
	else
		e->position += delta;
	e->old_val = new_val;
	IRQLSPIN_UNLOCK(& e->enclock, oldIrql);
}

static void encoder_clear(encoder_t * e)
{
	IRQL_t oldIrql;
	IRQL_t oldIrql2;

	IRQLSPIN_LOCK(& e->encspeedlock, & oldIrql, TICKER_IRQL);
	IRQLSPIN_LOCK(& e->enclock, & oldIrql2, ENCODER_IRQL);
	e->old_val = e->getpins();
	e->position = 0;
	e->backup_position = 0;
	IRQLSPIN_UNLOCK(& e->enclock, oldIrql2);
	e->rotate = 0;
	e->backup_rotate = 0;
	e->rotate_kbd = 0;
	e->enchist [0] = e->enchist [1] = e->enchist [2] = e->enchist [3] = 0;
	ASSERT(HISTLEN == 4);
	e->tichist = 0;
	IRQLSPIN_UNLOCK(& e->encspeedlock, oldIrql);
}

static int safegetposition_kbd(void)
{
#if WITHKBDENCODER
	int r = position_kbd;
	position_kbd = 0;
	return r;
#else
	return 0;
#endif
}

/* накопитель прерываний от валкодера - знаковое число */

/* значение используется вне прерываний - модификация без запрета прерываний */

static unsigned encoder1_actual_resolution = 128 * 4; //(encoder_resolution * 4 * ENCRESSCALE)	// Number of increments/decrements per revolution
static uint_fast8_t encoder1_dynamic = 1;

static unsigned encoder2_actual_resolution = 128 * 4; //(encoder_resolution * 4 * ENCRESSCALE)	// Number of increments/decrements per revolution
static uint_fast8_t encoder2_dynamic = 1;

//#define ENCODER_ACTUAL_RESOLUTION (encoder_resolution * 4 * ENCRESSCALE)	// Number of increments/decrements per revolution
//static uint_fast8_t encoder_resolution;

void encoderA_set_resolution(uint_fast8_t v, uint_fast8_t encdynamic)
{
	//encoder_resolution = v;	/* используется учетверение шагов */
	encoder1_actual_resolution = v * 4 * ENCRESSCALE;	/* используется учетверение шагов */
	encoder1_dynamic = encdynamic;
}

void encoderB_set_resolution(uint_fast8_t v, uint_fast8_t encdynamic)
{
	//encoder_resolution = v;	/* используется учетверение шагов */
	encoder2_actual_resolution = v * 4 * ENCRESSCALE;	/* используется учетверение шагов */
	encoder2_dynamic = encdynamic;
}

// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
// Расчёт средней скорости вращения валкодера (подготовка данных для расчёта вне прерываний).
static void
encspeed_spool(void * ctx)
{
	encoder_t * const e = (encoder_t *) ctx;
	const int p1 = encoder_get_delta(e, 1);
	const int p1kbd = 0;//safegetposition_kbd();

	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& e->encspeedlock, & oldIrql, TICKER_IRQL);

	e->rotate += p1;		/* учёт количества импульсов (для прямого отсчёта) */
#if WITHKBDENCODER
	rotate_kbd += p1kbd;		/* учёт количества импульсов (для прямого отсчёта) */
#endif

	/* запоминание данных для расчёта скорости вращения валкодера */
	/* при расчёте скорости игнорируется направление вращения - улучшается обработка синтуйии, когда при уже
	   включившемся ускорении пользователь меняет направление - движется назад к пропущенной частоте. при этом для
	   предсказуемости перестройки ускорение не должно изменяться.
	*/
	e->enchist [e->enchistindex] += abs(p1) + 0;//abs(p1kbd);
	if (++ e->tichist >= TICKSMAX)	// уменьшение предела - уменьшает "постояную времени" измерителя скорости валкодера
	{	
		e->tichist  = 0;
		e->enchistindex = (e->enchistindex + 1) % HISTLEN;
		e->enchist [e->enchistindex] = 0;		// Очередная ячейка накопления шагов очищается перед использованием.
	}

	IRQLSPIN_UNLOCK(& e->encspeedlock, oldIrql);
}

/* Обработка данных от валколдера */

void encoders_clear(void)
{
	encoder_clear(& encoder1);
#if WITHENCODER_SUB
	encoder_clear(& encoder_sub);
#endif /* WITHENCODER_SUB */
#if WITHENCODER2
	encoder_clear(& encoder2);
#endif /* WITHENCODER2 */
	encoder_clear(& encoder_ENC1F);
	encoder_clear(& encoder_ENC2F);
	encoder_clear(& encoder_ENC3F);
	encoder_clear(& encoder_ENC4F);
}

/* получение количества шагов и скорости вращения. */
int_least16_t
encoder_get_snapshotproportional(
	encoder_t * e,
	unsigned * speed, 
	const uint_fast8_t derate)
{
	int hrotate;
	
	unsigned s;				// количество шагов за время измерения
	unsigned tdelta;	// Время измерения

	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& e->encspeedlock, & oldIrql, TICKER_IRQL);

	// параметры изменерения скорости не модифицируем
	// 1. количество шагов за время измерения
	// HISTLEN == 4
	s = e->enchist [0] + e->enchist [1] + e->enchist [2] + e->enchist [3]; // количество шагов валкодера за время наблюдений
	ASSERT(HISTLEN == 4);
	// 2. Время измерения
	tdelta = e->tichist + TICKSMAX * (HISTLEN - 1); // во всех остальных слотах, кроме текущего, количество тиков максимальное.

	hrotate = e->rotate + e->rotate_kbd * derate;	/* работа в меню от клавиш - реагируем сразу */
	e->rotate = 0;
	e->rotate_kbd = 0;

	// Расчёт скорости. Результат - (1 / ENCODER_NORMALIZED_RESOLUTION) долей оборота за секунду
	// Если результат ENCODER_NORMALIZED_RESOLUTION это обозначает один оборот в секунду
	// ((s * TICKS_FREQUENCY) / t) - результат в размерности "импульсов в секунду".
	* speed = ((s * (uint_fast32_t) TICKS_FREQUENCY * ENCODER_NORMALIZED_RESOLUTION) / (tdelta * (uint_fast32_t) encoder1_actual_resolution));
	
	/* Уменьшение разрешения валкодера в зависимости от установок в меню */
	const div_t h = div(hrotate + e->backup_rotate, derate);

	e->backup_rotate = h.rem;

	IRQLSPIN_UNLOCK(& e->encspeedlock, oldIrql);

	return h.quot;
}


/* получение количества шагов и скорости вращения. */
static int_least16_t
encoder_get_snapshot(
	encoder_t * const e,
	const uint_fast8_t derate
	)
{
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& e->encspeedlock, & oldIrql, TICKER_IRQL);

	/* Уменьшение разрешения валкодера в зависимости от установок в меню */
	const div_t h = div(e->rotate + e->backup_rotate, derate);
	e->backup_rotate = h.rem;
	e->rotate = 0;

	IRQLSPIN_UNLOCK(& e->encspeedlock, oldIrql);

	return h.quot;
}

/* получение количества шагов, накопленного с момента предыдущего опроса */
int_least16_t
encoder_get_delta(
	encoder_t * const e,
	const uint_fast8_t derate
	)
{
	int position;

	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& e->enclock, & oldIrql, ENCODER_IRQL);
	position = e->position;
	e->position = 0;

	if (derate != 1)
	{
		/* Уменьшение разрешения валкодера в зависимости от установок в меню */
		const div_t h = div(position + e->backup_position, derate);
		e->backup_position = h.rem;
		IRQLSPIN_UNLOCK(& e->enclock, oldIrql);

		return h.quot;
	}
	else
	{
		IRQLSPIN_UNLOCK(& e->enclock, oldIrql);

		return position;
	}
}

void encoder_pushback(encoder_t * const e, int outsteps, uint_fast8_t hiresdiv)
{
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& e->encspeedlock, & oldIrql, TICKER_IRQL);

	e->backup_rotate += (outsteps * (int) hiresdiv);

	IRQLSPIN_UNLOCK(& e->encspeedlock, oldIrql);
}

encoder_t encoder1;	// Main RX tuning knob
#if WITHENCODER2
encoder_t encoder2;	// FN knob or Sub RX tuning knob
#endif /* WITHENCODER2 */
#if WITHENCODER_SUB
encoder_t encoder_sub;	// Sub RX tuning knob
#endif /* WITHENCODER_SUB */
encoder_t encoder_ENC1F;
encoder_t encoder_ENC2F;
encoder_t encoder_ENC3F;
encoder_t encoder_ENC4F;
encoder_t encoder_kbd;

// вызывается из обработчика таймерного прерывания - клавиатура.
void encoder_kbdctl(
	uint_fast8_t code, 		// код клавиши
	uint_fast8_t accel		// 0 - одиночное нажатие на клавишу, иначе автоповтор
	)
{
#if WITHKBDENCODER
	int_fast8_t d = code == ENC_CODE_STEP_UP ? 1 : -1;
	int v = d * (accel ? 5 : 1);
	position_kbd += v;
	//encoder_pushback(& encoder_kbd, v, 1);
#endif
}

/* получение "редуцированного" количества прерываний от валкодера.
 * То что осталось после деления на scale, остается в накопителе
 */
int_least16_t getRotateLoRes_A(uint_fast8_t hiresdiv)
{
	encoder_t * const e = & encoder1;
	unsigned speed;
	return encoder_get_snapshotproportional(& encoder1, & speed, encoder1_actual_resolution * hiresdiv / ENCODER_MENU_STEPS);
}

// Управление вращением валколера из CAT
void encoderA_pushback(int outsteps, uint_fast8_t hiresdiv)
{
	encoder_pushback(& encoder1, outsteps, hiresdiv);
}
/* получение накопленного значения прерываний от валкодера.
		накопитель сбрасывается */
static
int_least16_t 
getRotateHiRes_X(
	encoder_t * const e,
	uint_fast8_t * jumpsize,	/* jumpsize - во сколько раз увеличивается скорость перестройки */
	uint_fast8_t hiresdiv
	)
{
	typedef struct accel_tag
	{
		unsigned speed;
		uint_fast8_t muliplier;
	} accel;

	static const accel velotable [] =
	{
		{	ENCODER_NORMALIZED_RESOLUTION * 45U / 10U,	200U },	// 4.5 оборота в секунду
		{	ENCODER_NORMALIZED_RESOLUTION * 25U / 10U,	20U },	// 2.5 оборота в секунду
		{	ENCODER_NORMALIZED_RESOLUTION * 16U / 10U,	5U },	// 1.6 оборота в секунду
		{	ENCODER_NORMALIZED_RESOLUTION * 8U / 10U, 2U },	// 0.8 оборота в секунду - удвоение шага
	};


	unsigned speed;
	int_least16_t nrotate = encoder_get_snapshotproportional(e, & speed, hiresdiv);

	if (encoder1_dynamic != 0)
	{
		uint_fast8_t i;

		for (i = 0; i < (sizeof velotable / sizeof velotable [0]); ++ i)
		{
			const unsigned vtspeed = velotable [i].speed;
			//if (vtspeed == 0)
			//	break;			/* случай не до конца заполненной таблицы */
			if (speed >= vtspeed)
			{
				* jumpsize = velotable [i].muliplier;
				return nrotate;
			}
		}
	}

	* jumpsize = 1;
#if REQUEST_BA

	/* Уменьшение разрешения валкодера для скоростей меньших, чем в таблице делается равным 48 */
	const div_t h = div(nrotate, encoder1_actual_resolution / ENCODER_SLOW_STEPS);

	e->backup_rotate += h.rem * hiresdiv;

	return h.quot;
#else
	return nrotate;
#endif
}

int_least16_t
getRotateHiRes_A(
	uint_fast8_t * jumpsize,	/* jumpsize - во сколько раз увеличивается скорость перестройки */
	uint_fast8_t derate
	)
{
	return getRotateHiRes_X(& encoder1, jumpsize, derate);
}

/* получение накопленного значения прерываний от валкодера.
		накопитель сбрасывается */
int_least16_t 
getRotateHiRes_B(
	uint_fast8_t * jumpsize,	/* jumpsize - во сколько раз увеличивается скорость перестройки */
	uint_fast8_t derateSUB,
	uint_fast8_t derateFN
	)
{
#if WITHENCODER2
	* jumpsize = 1;
	return encoder_get_delta(& encoder2, derateFN);
#elif WITHENCODER_SUB
	return getRotateHiRes_X(& encoder_sub, jumpsize, derateSUB);
#else /* WITHENCODER_SUB */
	* jumpsize = 1;
	return 0;
#endif /* WITHENCODER_SUB */
}
#if WITHENCODER2
static void spool_encinterrupt2_local(void * ctx)
{
	spool_encinterrupts(& encoder2);
}
#endif /* WITHENCODER2 */

static uint_fast8_t hardware_get_encoderummy_bits(void)
{
	return 0;
}

/* вызывается при запрещённых прерываниях */
void encoders_initialize(void)
{
	static ticker_t encticker;
	static ticker_t encticker_sub;
	static ticker_t encticker2;

	//rotate = backup_rotate = 0;
	//enchistindex = 0;
	//tichist [enchistindex] = 0;
	//enchist [enchistindex] = 0;
	encoder_initialize(& encoder1, hardware_get_encoder_bits);
#if WITHENCODER_SUB
	encoder_initialize(& encoder_sub, hardware_get_encoder_sub_bits);
#endif /* WITHENCODER_SUB */
#if WITHENCODER2
	encoder_initialize(& encoder2, hardware_get_encoder2_bits);
#endif /* WITHENCODER2 */
	encoder_initialize(& encoder_ENC1F, hardware_get_encoder3_bits);
	encoder_initialize(& encoder_ENC2F, hardware_get_encoder4_bits);
	encoder_initialize(& encoder_ENC3F, hardware_get_encoder5_bits);
	encoder_initialize(& encoder_ENC4F, hardware_get_encoder6_bits);
	encoder_initialize(& encoder_kbd, hardware_get_encoderummy_bits);

#if ENCODER_REVERSE
	encoder1.reverse = 1;
#endif /* ENCODER_REVERSE */
#if ENCODER2_REVERSE
	encoder2.reverse = 1;
#endif /* ENCODER2_REVERSE */


#if WITHENCODER
	ticker_initialize(& encticker, 1, encspeed_spool, & encoder1);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
	ticker_add(& encticker);
#endif /* WITHENCODER */
#if WITHENCODER_SUB
	ticker_initialize(& encticker_sub, 1, encspeed_spool, & encoder_sub);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
	ticker_add(& encticker_sub);
#endif /* WITHENCODER */
#if WITHENCODER2 && ! ENCODER2_NOSPOOL
	// второй енкодер всегда по опросу (если это не назначено при инициализации)
	ticker_initialize(& encticker2, 1, spool_encinterrupt2_local, NULL);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
	ticker_add(& encticker2);
#endif /* WITHENCODER2 && ! ENCODER2_NOSPOOL */
}

#if WITHLVGL && WITHENCODER

#include "lv_drivers/indev/evdev.h"

void indev_enc2_spool(void)
{
	encspeed_spool(& encoder2);
}

void encoder_indev_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
	static uint32_t last_key = 0;
	static lv_indev_state_t encoder_state;
	static int32_t encoder_diff;

	int r2 = encoder_get_snapshot(& encoder2, BOARD_ENCODER2_DIVIDE);
	uint32_t act_key = r2 > 0 ? 3 : r2 < 0 ? 2 : TARGET_ENC2BTN_GET ? 1 : 0;

	if(act_key != 0) {
		switch(act_key) {
			case 1:
			act_key = LV_KEY_ENTER;
			encoder_state = LV_INDEV_STATE_PR;
			break;
		case 2:
			act_key = LV_KEY_LEFT;
			encoder_diff = -1;
			encoder_state = LV_INDEV_STATE_REL;
			break;
		case 3:
			act_key = LV_KEY_RIGHT;
			encoder_state = LV_INDEV_STATE_REL;
			encoder_diff = 1;
			break;
		}
		last_key = act_key;
	}
	else {
		encoder_diff = 0;
		encoder_state = LV_INDEV_STATE_REL;
	}
	data->key = last_key;
	data->enc_diff = encoder_diff;
	data->state = encoder_state;
}

#endif /* WITHLVGL */
