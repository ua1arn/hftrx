/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "encoder.h"
#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "keyboard.h"	/* функция опроса электронного ключа */
//#include "display.h"	/* test */
#include "formats.h"	/* test */

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
static RAMDTCM int_fast8_t graydecoder [4][4] =
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

static RAMDTCM uint_fast8_t old_val;

void spool_encinterrupt(void)
{
	const uint_fast8_t new_val = hardware_get_encoder_bits();	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

#if ENCODER_REVERSE
	position1 -= graydecoder [old_val][new_val];
#else
	position1 += graydecoder [old_val][new_val];
#endif
	old_val = new_val;
}

static RAMDTCM uint_fast8_t old_val2;

void spool_encinterrupt2(void)
{
	const uint_fast8_t new_val = hardware_get_encoder2_bits();	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

#if ENCODER2_REVERSE
	position2 -= graydecoder [old_val2][new_val];
#else
	position2 += graydecoder [old_val2][new_val];
#endif
	old_val2 = new_val;
}

static int safegetposition1(void)
{
#if WITHHARDINTERLOCK
	uint32_t r;
	do
		r = __LDREXW(& position1);
	while (__STREXW(0, & position1));
	return (int32_t) r;
#else /* WITHHARDINTERLOCK */
	global_disableIRQ();
	int r = position1;
	position1 = 0;
	global_enableIRQ();
	return r;
#endif /* WITHHARDINTERLOCK */
}

static int safegetposition2(void)
{
#if WITHHARDINTERLOCK
	uint32_t r;
	do
		r = __LDREXW(& position2);
	while (__STREXW(0, & position2));
	return (int32_t) r;
#else /* WITHHARDINTERLOCK */
	global_disableIRQ();
	int r = position2;
	position2 = 0;
	global_enableIRQ();
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

#if WITHKBDENCODER

// вызывается из обработчика таймерного прерывания - клавиатура.
void encoder_kbdctl(
	uint_fast8_t code, 		// код клавиши
	uint_fast8_t accel		// 0 - одиночное нажатие на клавишу, иначе автоповтор
	)
{
	int_fast8_t d = code == ENC_CODE_STEP_UP ? 1 : -1;
	position_kbd += d * (accel ? 5 : 1);

}

#endif


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
	const int p1 = safegetposition1();	// Валкодер #1
	const int p1kbd = safegetposition_kbd();
	rotate1 += p1;		/* учёт количества импульсов (для прямого отсчёта) */
#if WITHKBDENCODER
	rotate_kbd += p1kbd;		/* учёт количества импульсов (для прямого отсчёта) */
#endif

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
	const int p2 = safegetposition2();
	rotate2 += p2;		/* учёт количества импульсов (для прямого отсчёта) */
}


static ticker_t encticker;

/* Обработка данных от валколдера */

void encoder_clear(void)
{
	backup_rotate = 0;
	backup_rotate2 = 0;
	disableIRQ();
	rotate1 = 0;
	rotate_kbd = 0;

	// HISTLEN == 4
	enchist [0] = enchist [1] = enchist [2] = enchist [3] = 0; 
	tichist = 0;

	enableIRQ();
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

	disableIRQ();

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


	enableIRQ();

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

	disableIRQ();
	hrotate = rotate2;
	rotate2 = 0;
	enableIRQ();

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
	uint_fast8_t * jumpsize	/* jumpsize - во сколько раз увеличивается скорость перестройки */
	)
{
	#if defined (BOARD_ENCODER2_DIVIDE)
		const unsigned loresdiv = BOARD_ENCODER2_DIVIDE;
	#else /* defined (BOARD_ENCODER2_DIVIDE) */
		const unsigned loresdiv = 2;	/* значение для валкодера PEC16-4220F-n0024 (с трещёткой") */
	#endif /* defined (BOARD_ENCODER2_DIVIDE) */

	unsigned speed;
	int nrotate = encoder2_get_snapshot(& speed, loresdiv);

	* jumpsize = 1;
	return nrotate;
}

/* вызывается при запрещенных прерываниях */
void encoder_initialize(void)
{
	//rotate = backup_rotate = 0;
	//enchistindex = 0;
	//tichist [enchistindex] = 0;
	//enchist [enchistindex] = 0;


	old_val = hardware_get_encoder_bits();	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
	old_val2 = hardware_get_encoder2_bits();	/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
#if WITHENCODER
	ticker_initialize(& encticker, 1, enc_spool, NULL);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
#endif /* WITHENCODER */
}
