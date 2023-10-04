/* $Id$ */
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

// dimensions are:
// old_bits new_bits
static RAMDTCM int8_t graydecoder [4][4] =
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

static IRQLSPINLOCK_t enc1lock;
static IRQLSPINLOCK_t enc2lock;

static RAMDTCM uint_fast8_t old_val;

void spool_encinterrupt(void)
{
	const uint_fast8_t new_val = hardware_get_encoder_bits();	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& enc1lock, & oldIrql);
#if ENCODER_REVERSE
	position1 -= graydecoder [old_val][new_val];

#else
	position1 += graydecoder [old_val][new_val];

#endif
	old_val = new_val;
	IRQLSPIN_UNLOCK(& enc1lock, oldIrql);
}

static RAMDTCM uint_fast8_t old_val2;

void spool_encinterrupt2(void)
{
	const uint_fast8_t new_val = hardware_get_encoder2_bits();	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& enc2lock, & oldIrql);

#if ENCODER2_REVERSE
	position2 -= graydecoder [old_val2][new_val];

#else
	position2 += graydecoder [old_val2][new_val];

#endif
	old_val2 = new_val;
	IRQLSPIN_UNLOCK(& enc2lock, oldIrql);
}

// вызывается в контексте обработчика прерываний
static int safegetposition1(void)
{
#if WITHHARDINTERLOCK
	uint32_t r;
	do
	{
		r = __LDREXW(& position1);
		__NOP();
	} while (__STREXW(0, & position1));
	return (int32_t) r;

#else /* WITHHARDINTERLOCK */
	int r = position1;
	position1 = 0;
	return r;

#endif /* WITHHARDINTERLOCK */
}

// вызывается в контексте обработчика прерываний
static int safegetposition2(void)
{
#if WITHHARDINTERLOCK
	uint32_t r;
	do
	{
		r = __LDREXW(& position2);
		__NOP();
	} while (__STREXW(0, & position2));
	return (int32_t) r;

#else /* WITHHARDINTERLOCK */
	int r = position2;
	position2 = 0;
	return r;

#endif /* WITHHARDINTERLOCK */
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


// вызывается из обработчика таймерного прерывания - клавиатура.
void encoder_kbdctl(
	uint_fast8_t code, 		// код клавиши
	uint_fast8_t accel		// 0 - одиночное нажатие на клавишу, иначе автоповтор
	)
{
#if WITHKBDENCODER
	int_fast8_t d = code == ENC_CODE_STEP_UP ? 1 : -1;
	position_kbd += d * (accel ? 5 : 1);
#endif
}



/* накопитель прерываний от валкодера - знаковое число */
static RAMDTCM int rotate1;
static RAMDTCM int rotate_kbd;
static RAMDTCM int backup_rotate;

/* накопитель прерываний от валкодера #2 - знаковое число */
static RAMDTCM int rotate2;
static RAMDTCM int backup_rotate2;

#define HISTLEN 4		// кое-где дальше есть код, в неявном виде использующий это значение
#define TICKSMAX NTICKS(125)

static RAMDTCM unsigned enchist [HISTLEN];
static RAMDTCM uint_fast8_t tichist;	// Должно поместиться число от 0 до TICKSMAX включительно

static RAMDTCM uint_fast8_t enchistindex;

/* значение используется вне прерываний - модификация без запрета прерываний */

static RAMDTCM unsigned encoder_actual_resolution = 128 * 4; //(encoder_resolution * 4 * ENCRESSCALE)	// Number of increments/decrements per revolution
static RAMDTCM uint_fast8_t encoder_dynamic = 1;

//#define ENCODER_ACTUAL_RESOLUTION (encoder_resolution * 4 * ENCRESSCALE)	// Number of increments/decrements per revolution
//static uint_fast8_t encoder_resolution;

void encoder_set_resolution(uint_fast8_t v, uint_fast8_t encdynamic)
{
	//encoder_resolution = v;	/* используется учетверение шагов */
	encoder_actual_resolution = v * 4 * ENCRESSCALE;	/* используется учетверение шагов */
	encoder_dynamic = encdynamic;
}

// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
// Расчёт средней скорости вращения валкодера (подготовка данных для расчёта вне прерываний).
static void
enc_spool(void * ctx)
{
	IRQL_t oldIrql;

	IRQLSPIN_LOCK(& enc1lock, & oldIrql);

	const int p1 = safegetposition1();	// Валкодер #1
	const int p1kbd = safegetposition_kbd();
	rotate1 += p1;		/* учёт количества импульсов (для прямого отсчёта) */
#if WITHKBDENCODER
	rotate_kbd += p1kbd;		/* учёт количества импульсов (для прямого отсчёта) */
#endif
	IRQLSPIN_UNLOCK(& enc1lock, oldIrql);

	/* запоминание данных для расчёта скорости вращения валкодера */
	/* при расчёте скорости игнорируется направление вращения - улучшается обработка синтуйии, когда при уже
	   включившемся ускорении пользователь меняет направление - движется назад к пропущенной частоте. при этом для
	   предсказуемости перестройки ускорение не должно изменяться.
	*/
	enchist [enchistindex] += abs(p1) + abs(p1kbd);
	if (++ tichist >= TICKSMAX)	// уменьшение предела - уменьшает "постояную времени" измерителя скорости валкодера
	{	
		tichist  = 0;
		enchistindex = (enchistindex + 1) % HISTLEN;
		enchist [enchistindex] = 0;		// Очередная ячейка накопления шагов очищается перед использованием.
	}

	// Валкодер #2

	IRQLSPIN_LOCK(& enc2lock, & oldIrql);
	const int p2 = safegetposition2();
	rotate2 += p2;		/* учёт количества импульсов (для прямого отсчёта) */
	IRQLSPIN_UNLOCK(& enc2lock, oldIrql);
}


static ticker_t encticker;
static ticker_t encticker2;

/* Обработка данных от валколдера */

void encoder_clear(void)
{
	backup_rotate = 0;
	backup_rotate2 = 0;
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& enc1lock, & oldIrql);
	rotate1 = 0;
	rotate_kbd = 0;

	// HISTLEN == 4
	enchist [0] = enchist [1] = enchist [2] = enchist [3] = 0; 
	tichist = 0;

	IRQLSPIN_UNLOCK(& enc1lock, oldIrql);
}

/* получение количества шагов и скорости вращения. */
int 
encoder_get_snapshot(
	unsigned * speed, 
	const uint_fast8_t derate)
{
	int hrotate;
	
	unsigned s;				// количество шагов за время измерения
	unsigned tdelta;	// Время измерения

	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& enc1lock, & oldIrql);

	// параметры изменерения скорости не модифицируем
	// 1. количество шагов за время измерения
	// HISTLEN == 4
	s =
		enchist [0] + enchist [1] + enchist [2] + enchist [3]; // количество шагов валкодера за время наблюдений
	// 2. Время измерения
	tdelta = tichist + TICKSMAX * (HISTLEN - 1); // во всех остальных слотах, кроме текущего, количество тиков максимальное.

	hrotate = rotate1 + rotate_kbd * derate;	/* работа в меню от клавиш - реагируем сразу */
	rotate1 = 0;
	rotate_kbd = 0;

	IRQLSPIN_UNLOCK(& enc1lock, oldIrql);

	// Расчёт скорости. Результат - (1 / ENCODER_NORMALIZED_RESOLUTION) долей оборота за секунду
	// Если результат ENCODER_NORMALIZED_RESOLUTION это обозначает один оборот в секунду
	// ((s * TICKS_FREQUENCY) / t) - результат в размерности "импульсов в секунду".
	* speed = ((s * (unsigned long) TICKS_FREQUENCY * ENCODER_NORMALIZED_RESOLUTION) / (tdelta * (unsigned long) encoder_actual_resolution));
	
	/* Уменьшение разрешения валкодера в зависимости от установок в меню */
	const div_t h = div(hrotate + backup_rotate, derate);

	backup_rotate = h.rem;

	return h.quot;
}

/* получение количества шагов и скорости вращения. */
int 
encoder2_get_snapshot(
	unsigned * speed, 
	const uint_fast8_t derate)
{
	int hrotate;

	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& enc2lock, & oldIrql);
	hrotate = rotate2;
	rotate2 = 0;
	IRQLSPIN_UNLOCK(& enc2lock, oldIrql);

	/* Уменьшение разрешения валкодера в зависимости от установок в меню */
	const div_t h = div(hrotate + backup_rotate2, derate);
	backup_rotate2 = h.rem;
	* speed = 0;
	return h.quot;
}

/* получение "редуцированного" количества прерываний от валкодера.
 * То что осталось после деления на scale, остается в накопителе
 */
int getRotateLoRes(uint_fast8_t hiresdiv)
{
	unsigned speed;
	return encoder_get_snapshot(& speed, encoder_actual_resolution * hiresdiv / ENCODER_MENU_STEPS);
}


// Управление вращением валколера из CAT
void encoder_pushback(int outsteps, uint_fast8_t hiresdiv)
{
	backup_rotate += (outsteps * (int) hiresdiv);
}
/* получение накопленного значения прерываний от валкодера.
		накопитель сбрасывается */
int_least16_t 
getRotateHiRes(
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
#if REQUEST_BA
		// Для Б.А.
		{	ENCODER_NORMALIZED_RESOLUTION * 22U / 10U,	100U	},	// 2.2 оборота в секунду 100 шагов
		{	ENCODER_NORMALIZED_RESOLUTION * 17U / 10U,	50U	},	// 1.7 оборота в секунду 50 шагов
		{	ENCODER_NORMALIZED_RESOLUTION * 15U / 10U,	10U	},	// 1.5 оборота в секунду 10 шагов
		{	ENCODER_NORMALIZED_RESOLUTION * 8U / 10U, 5U	},	// 0.8 оборота в секунду 5 шагов
		{	ENCODER_NORMALIZED_RESOLUTION * 5U / 10U, 2U },	// 0.5 оборота в секунду - удвоение шага
		{	ENCODER_NORMALIZED_RESOLUTION * 3U / 10U, 1U },	// 0.3 оборота в секунду - нормальный шаг
#elif 1
		{	ENCODER_NORMALIZED_RESOLUTION * 45U / 10U,	200U },	// 4.5 оборота в секунду
		{	ENCODER_NORMALIZED_RESOLUTION * 25U / 10U,	20U },	// 2.5 оборота в секунду
		{	ENCODER_NORMALIZED_RESOLUTION * 16U / 10U,	5U },	// 1.6 оборота в секунду
		{	ENCODER_NORMALIZED_RESOLUTION * 8U / 10U, 2U },	// 0.8 оборота в секунду - удвоение шага
#else
		{	ENCODER_NORMALIZED_RESOLUTION * 22U / 10U,	50U },	// 2.2 оборота в секунду
		{	ENCODER_NORMALIZED_RESOLUTION * 15U / 10U, 10U	},	// 1.5 оборот в секунду
		{	ENCODER_NORMALIZED_RESOLUTION * 8U / 10U, 2U },	// 0.8 оборота в секунду - удвоение шага
		//{	ENCODER_NORMALIZED_RESOLUTION * 2U / 10U, 1U },	// 0.2 оборота в секунду - нормальный шаг
#endif
	};


	unsigned speed;
	int nrotate = encoder_get_snapshot(& speed, hiresdiv);

	if (encoder_dynamic != 0)
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
	const div_t h = div(nrotate, encoder_actual_resolution / ENCODER_SLOW_STEPS);

	backup_rotate += h.rem * hiresdiv;

	return h.quot;
#else
	return nrotate;
#endif
}

/* получение накопленного значения прерываний от валкодера.
		накопитель сбрасывается */
int_least16_t 
getRotateHiRes2(
	uint_fast8_t * jumpsize,	/* jumpsize - во сколько раз увеличивается скорость перестройки */
	uint_fast8_t loresdiv
	)
{
#if WITHENCODER2
	unsigned speed;
	int nrotate = encoder2_get_snapshot(& speed, loresdiv);

	* jumpsize = 1;
	return nrotate;
#else /* WITHENCODER2 */
	return 0;
#endif /* WITHENCODER2 */
}

static void spool_encinterrupt2_local(void * ctx)
{
	spool_encinterrupt2();
}

/* вызывается при запрещённых прерываниях */
void encoder_initialize(void)
{
	//rotate = backup_rotate = 0;
	//enchistindex = 0;
	//tichist [enchistindex] = 0;
	//enchist [enchistindex] = 0;

	IRQLSPINLOCK_INITIALIZE(& enc1lock, IRQL_OVERREALTIME);
	IRQLSPINLOCK_INITIALIZE(& enc2lock, IRQL_OVERREALTIME);

	old_val = hardware_get_encoder_bits();	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
	old_val2 = hardware_get_encoder2_bits();	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
#if WITHENCODER
	ticker_initialize(& encticker, 1, enc_spool, NULL);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
	ticker_add(& encticker);
#endif /* WITHENCODER */
#if WITHENCODER2
	// второй енкодер всегда по опросу
	ticker_initialize(& encticker2, 1, spool_encinterrupt2_local, NULL);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
	ticker_add(& encticker2);
#endif /* WITHENCODER2 */
}

#if WITHLVGL

#include "lv_drivers/indev/evdev.h"

void indev_enc2_spool(void)
{
	enc_spool(NULL);
}

void encoder_indev_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
	static uint32_t last_key = 0;
	static lv_indev_state_t encoder_state;
	static int32_t encoder_diff;
	unsigned speed;

	int r2 = encoder2_get_snapshot(& speed, BOARD_ENCODER2_DIVIDE);
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
