/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "keyboard.h"
#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if WITHKEYBOARD

#if KEYBOARD_USE_ADC
	// добавочная двойка - последне элементы для кнопок валколера и выключения питания
	#if KEYBOARD_USE_ADC6_V1
		#define NQMKEYS (KI_COUNT * 6 + 2)
	#elif KEYBOARD_USE_ADC6
		#define NQMKEYS (KI_COUNT * 6 + 2)
	#else /* KEYBOARD_USE_ADC6 */
		#define NQMKEYS (KI_COUNT * 4 + 2)
	#endif /* KEYBOARD_USE_ADC6 */
#elif KEYBOARD_SINGLE
	#define NQMKEYS (1)
#elif KEYBOARD_MATRIX_4x4
	#define NQMKEYS (16 + 1)
#else
	#define NQMKEYS (12 + 2)		// сейчас обслуживается до 12-ти кнопок.
#endif


#if KEYB_7BUTTONSOLD // ! KEYBOARD_USE_ADC

	// Коды для случая работы с семью кнопками, подключёнными прямо на выводы порта

	static const struct qmkey qmdefs [NQMKEYS] =
	{
		{ KIF_SLOW, KBD_CODE_BAND_DOWN, KBD_CODE_BAND_DOWN,  },
		{ KIF_NONE, KBD_CODE_MODE, KBD_CODE_MODEMOD,  },
		{ KIF_EXTMENU, KBD_CODE_DISPMODE, KBD_CODE_MENU,  },
		{ KIF_SLOW, KBD_CODE_BAND_UP, KBD_CODE_BAND_UP,  },
		{ KIF_NONE, KBD_CODE_BW, KBD_CODE_AGC,  },
		{ KIF_NONE, KBD_CODE_ATT, KBD_CODE_PAMP,  },
		{ KIF_ERASE, KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED, }, 
		{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
		{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
		{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
		{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
		{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
	};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_7BUTTONS
	// Коды для случая работы с семью кнопками, подключёнными прямо на ADC7 и ADC6

	static const struct qmkey qmdefs [NQMKEYS] =
	{
		{ KIF_NONE, KBD_CODE_BW, KBD_CODE_AGC,  },	// 0
		{ KIF_NONE, KBD_CODE_MODE, KBD_CODE_MODEMOD,  },	// 1
		{ KIF_NONE, KBD_CODE_ATT, KBD_CODE_PAMP,  },	// 2
		{ KIF_EXTMENU, KBD_CODE_DISPMODE, KBD_CODE_MENU,  },	// 3

		{ KIF_SLOW, KBD_CODE_BAND_DOWN, KBD_CODE_BAND_DOWN,  },	// 4
		{ KIF_SLOW, KBD_CODE_BAND_UP, KBD_CODE_BAND_UP,  },	// 5
		{ KIF_ERASE, KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED, }, // 6
		{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },

		{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
		{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
		{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
		{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
		// encoder2 & power buttons
		{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
		{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_VERTICAL

// повернутое на 90 градусов - 4 строки по 3 кнопки
// увеличение индекса в массиве соответствует направлению перебора кнопок снизу вверх
/* расположение кнопок для плат "Павлин" */

static const struct qmkey qmdefs [NQMKEYS] =
{
	//ki2-правый ряд
	{ KIF_FAST,ENC_CODE_STEP_UP,ENC_CODE_STEP_UP, },
	{ KIF_SLOW,KBD_CODE_BAND_DOWN,KBD_CODE_BAND_DOWN, },
	{ KIF_SLOW,KBD_CODE_BAND_UP,KBD_CODE_BAND_UP, },
	{ KIF_NONE,KBD_CODE_MODE,KBD_CODE_MODEMOD, },
	//ki1-центральный ряд
	{ KIF_NONE,KBD_CODE_LOCK,KBD_CODE_LOCK_HOLDED, },
	{ KIF_NONE,KBD_CODE_BW,KBD_CODE_AGC, },
	{ KIF_NONE,KBD_CODE_ATT,KBD_CODE_PAMP, },
	{ KIF_NONE,KBD_CODE_MOX,KBD_CODE_TXTUNE, },
	//ki0-левый ряд
	{ KIF_FAST,ENC_CODE_STEP_DOWN,ENC_CODE_STEP_DOWN, },
	{ KIF_NONE,KBD_CODE_MAX,KBD_CODE_MAX, },
	{ KIF_ERASE,KBD_CODE_SPLIT,KBD_CODE_SPLIT_HOLDED, },
	{ KIF_EXTMENU,KBD_CODE_DISPMODE,KBD_CODE_MENU, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_RAVEN24

/* расположение кнопок для плат "Воробей" с DSP обработкой */
static const struct qmkey qmdefs [NQMKEYS] =
{
	//ki5-самый правый ряд
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX,		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	//ki4-правый ряд
	{ KIF_NONE,		KBD_CODE_MODE,		KBD_CODE_MODEMOD, 		'3', },
	{ KIF_SLOW,		KBD_CODE_BAND_UP,	KBD_CODE_BAND_UP, 		'6', },
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,	KBD_CODE_BAND_DOWN, 	'9', },
	//{ KIF_FAST,		ENC_CODE_STEP_UP,	ENC_CODE_STEP_UP, 		'*', },
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE, KBD_CODE_RECORD_HOLDED, '*', },
	//ki3-самый правый ряд
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX,		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	//ki2-центральный ряд
	{ KIF_NONE,		KBD_CODE_MOX,		KBD_CODE_TXTUNE, 		'2', },
	{ KIF_NONE,		KBD_CODE_ATT,		KBD_CODE_PAMP, 			'5', },
	{ KIF_NONE,		KBD_CODE_BW,		KBD_CODE_MAX, 			'8', },	// AGC не выключается с клавиатуры
	{ KIF_NONE,		KBD_CODE_LOCK,		KBD_CODE_LOCK_HOLDED, 	'0', },
	//ki1-самый правый ряд
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX,		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	//ki0-левый ряд
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,	KBD_CODE_MENU, 			'1', },
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED, 	'4', },
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,	KBD_CODE_ENTERFREQDONE, '7', },
	//{ KIF_FAST,		ENC_CODE_STEP_DOWN,	ENC_CODE_STEP_DOWN, 	'#', },
	{ KIF_NONE,		KBD_CODE_DWATCHTOGGLE, KBD_CODE_DWATCHHOLD, '#', },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_RAVEN24_RA1AGO

/* расположение кнопок для плат "Воробей" с DSP обработкой - выстаочный экземпляр */
static const struct qmkey qmdefs [NQMKEYS] =
{
	{ KIF_NONE,		KBD_CODE_BW,		KBD_CODE_AGC, 			' ', },	// #0: F4
	{ KIF_SLOW4,	KBD_CODE_AMBANDPASSUP,		KBD_CODE_AMBANDPASSUP, 		' ', },	// #1: F3
	{ KIF_SLOW4,	KBD_CODE_AMBANDPASSDOWN,	KBD_CODE_AMBANDPASSDOWN,	' ', },	// #2: F2
	{ KIF_NONE,		KBD_CODE_ATT,		KBD_CODE_PAMP,			' ', },	// #3: F1

	{ KIF_NONE,		KBD_CODE_ENTERFREQ,	KBD_CODE_ENTERFREQDONE, ' ', },	// #4: Enter
	{ KIF_SLOW,		KBD_CODE_5,			KBD_CODE_MAX, 			'#', },	// #5: DEL
	{ KIF_SLOW,		KBD_CODE_6,			KBD_CODE_MAX, 			'0', },	// #6: 0
	{ KIF_NONE,		KBD_CODE_7,			KBD_CODE_MAX,			'9', },	// #7: 9

	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,	KBD_CODE_MENU, 			' ', },	// #8: CMD
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,	KBD_CODE_BAND_DOWN, 	' ', },	// #9: DOWN
	{ KIF_SLOW,		KBD_CODE_BAND_UP,	KBD_CODE_BAND_UP,		' ', },	// #10: UP
	{ KIF_NONE,		KBD_CODE_MODE,		KBD_CODE_MODEMOD, 		' ', },	// #11: MDE

	{ KIF_NONE,		KBD_CODE_12,		KBD_CODE_MAX,			'8', },	// #12: 8
	{ KIF_NONE,		KBD_CODE_13,		KBD_CODE_MAX, 			'7', },	// #13: 7
	{ KIF_NONE,		KBD_CODE_14,		KBD_CODE_MAX, 			'6', },	// #14: 6
	{ KIF_NONE,		KBD_CODE_15,		KBD_CODE_MAX,			'5', },	// #15: 5

	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE,	KBD_CODE_RECORD_HOLDED, 		' ', },	// #16: RECORD
	{ KIF_NONE,		KBD_CODE_17,		KBD_CODE_MAX, 			' ', },	// #17: MW
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED,		' ', },	// #18: MR
	{ KIF_NONE,		KBD_CODE_LOCK,		KBD_CODE_LOCK_HOLDED, 	' ', },	// #19: X

	{ KIF_NONE,		KBD_CODE_20,		KBD_CODE_MAX, 			'4', },	// #20: 4
	{ KIF_NONE,		KBD_CODE_21,		KBD_CODE_MAX,			'3', },	// #21: 3
	{ KIF_NONE,		KBD_CODE_22,		KBD_CODE_MAX,			'2', },	// #22: 2
	{ KIF_NONE,		KBD_CODE_23,		KBD_CODE_MAX,			'1', },	// #23: 1

	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_RAVEN20_V5

/* расположение кнопок для плат "Воробей" с DSP обработкой */
static const struct qmkey qmdefs [NQMKEYS] =
{
	//ki4-правый ряд
	{ KIF_NONE,		KBD_CODE_MODE,		KBD_CODE_MODEMOD, 		'3', },
	{ KIF_SLOW,		KBD_CODE_BAND_UP,	KBD_CODE_BAND_UP, 		'6', },
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,	KBD_CODE_BAND_DOWN, 	'9', },
	//{ KIF_FAST,		ENC_CODE_STEP_UP,	ENC_CODE_STEP_UP, 		'*', },
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE, KBD_CODE_RECORD_HOLDED, '*', },
	//ki3-самый правый ряд
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX,		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	//ki2-центральный ряд
#if WITHAUTOTUNER
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,	KBD_CODE_ATUSTART,		'2', },		// BYP/ATU: was PWR
#else /* WITHAUTOTUNER */
	{ KIF_NONE,		KBD_CODE_MOX,		KBD_CODE_TXTUNE, 		'2', },
#endif /* WITHAUTOTUNER */
	{ KIF_NONE,		KBD_CODE_ATT,		KBD_CODE_PAMP, 			'5', },
	{ KIF_NONE,		KBD_CODE_BW,		KBD_CODE_NR, 			'8', },	// BW/NR
	{ KIF_NONE,		KBD_CODE_LOCK,		KBD_CODE_LOCK_HOLDED, 	'0', },
	//ki1-самый правый ряд
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX,		' ', },
	{ KIF_NONE,		KBD_CODE_MAX,	KBD_CODE_MAX, 		' ', },
	//ki0-левый ряд
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,	KBD_CODE_MENU, 			'1', },
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED, 	'4', },
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,	KBD_CODE_ENTERFREQDONE, '7', },
	//{ KIF_FAST,		ENC_CODE_STEP_DOWN,	ENC_CODE_STEP_DOWN, 	'#', },
	{ KIF_NONE,		KBD_CODE_DWATCHTOGGLE, KBD_CODE_DWATCHHOLD, '#', },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_FPANEL20_V0

/* 20 кнопок на 5 линий - плата rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении */
static const struct qmkey qmdefs [NQMKEYS] =
{
	/* ! ряд справа от индикатора - снизу вверх */
	{ KIF_NONE,		KBD_CODE_LOCK,		KBD_CODE_LOCK_HOLDED, 	' ', },
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,	KBD_CODE_BAND_DOWN, 	' ', },
	{ KIF_SLOW,		KBD_CODE_BAND_UP,	KBD_CODE_BAND_UP, 		' ', },
	{ KIF_NONE,		KBD_CODE_MODE,		KBD_CODE_MODEMOD, 		' ', },

	/* ! матрица слева от индикатора - ряд 1 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE, KBD_CODE_RECORD_HOLDED, ' ', },
	{ KIF_NONE,		KBD_CODE_MOX,		KBD_CODE_TXTUNE, 	' ', },
	{ KIF_NONE,		KBD_CODE_DWATCHTOGGLE, KBD_CODE_DWATCHHOLD, ' ', },
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,	KBD_CODE_MENU, 		' ', },

	/* ! матрица слева от индикатора - ряд 4 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,	KBD_CODE_ENTERFREQDONE, '#', },
	{ KIF_NONE,		KBD_CODE_BW,		KBD_CODE_MAX, 			'9', },	// AGC не выключается с клавиатуры
	{ KIF_NONE,		KBD_CODE_ATT,		KBD_CODE_PAMP, 			'6', },
	{ KIF_NONE,		KBD_CODE_23,		KBD_CODE_23, 			'3', },

	/* матрица слева от индикатора - ряд 3 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_LDSPTGL,		KBD_CODE_22,	'0', },
	{ KIF_NONE,		KBD_CODE_21,		KBD_CODE_21, 		'9', },
	{ KIF_NONE,		KBD_CODE_20,		KBD_CODE_20, 		'5', },
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED, '2', },

	/* матрица слева от индикатора - ряд 2 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,	KBD_CODE_ATUSTART, '*', },		// BYP/ATU
	{ KIF_NONE,		KBD_CODE_19,	KBD_CODE_19, 		'7', },
	{ KIF_NONE,		KBD_CODE_18,	KBD_CODE_18,		'4', },
	{ KIF_NONE,		KBD_CODE_ANTENNA,	KBD_CODE_ANTENNA_HOLDED, 		'1', },

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_MAX,		KBD_CODE_POWEROFF, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_FPANEL20_V0A_UY5UM

/* 20 кнопок на 5 линий - плата rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении */
/* Новый вариант под корпус с надписями от RK1AQ */
static const struct qmkey qmdefs [NQMKEYS] =
{
	/* ! ряд справа от индикатора - снизу вверх */
	{ KIF_NONE,		KBD_CODE_LOCK,			KBD_CODE_GEN_HAM,		' ', },		// LOCK / GEN_HAM
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// BAND DOWN
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },		// BAND UP
	{ KIF_NONE,		KBD_CODE_MODE,			KBD_CODE_MODEMOD, 		' ', },		// MODE/SUBMODE

	/* ! матрица слева от индикатора - ряд 1 по счету слева - снизу вверх */
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,		KBD_CODE_MENU, 			' ', },		// DISPLAY/MENU
	{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE,		' ', },		// MOX/TUNE
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,		KBD_CODE_ATUSTART,		' ', },		// TUNER BYP/ATU START
	{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED,		' ', },		// SPLIT/SPLIT OFF

	/* ! матрица слева от индикатора - ряд 4 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_MENU_CWSPEED,	KBD_CODE_11, 			' ', },		// CW SPEED
	{ KIF_NONE,		KBD_CODE_NOTCHTOGGLE,	KBD_CODE_NOTCHFREQ, 	' ', },		// NOTCH
	{ KIF_NONE,		KBD_CODE_BW,			KBD_CODE_NR, 			' ', },		// BW/NR
	{ KIF_NONE,		KBD_CODE_DWATCHTOGGLE,	KBD_CODE_DWATCHHOLD,	' ', },		// DUAL

	/* матрица слева от индикатора - ряд 3 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_BKIN,			KBD_CODE_GEN_HAM, 		' ', },		// BREAK-IN
	{ KIF_NONE,		KBD_CODE_LDSPTGL,		KBD_CODE_DATATOGGLE,	' ', },		// SPK/DATA
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			' ', },		// ATT/PRE
	{ KIF_NONE,		KBD_CODE_A_EX_B,			KBD_CODE_18, 		' ', },		// A/B

	/* матрица слева от индикатора - ряд 2 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_VOXTOGGLE,		KBD_CODE_MAX,			' ', },		// VOX
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE,	KBD_CODE_RECORD_HOLDED,	' ', },		// REC
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_ANTENNA_HOLDED, 			' ', },		// ANTENNA
	{ KIF_NONE,		KBD_CODE_A_EQ_B,			KBD_CODE_21, 		' ', },		// A=B

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_MAX,		KBD_CODE_POWEROFF, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_FPANEL20_V0A

/* 20 кнопок на 5 линий - плата Rfrontpanel_v7a, Rfrontpanel_v4b, Rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении */
/* Новый вариант под корпус с надписями от RK1AQ */
static const struct qmkey qmdefs [NQMKEYS] =
{
	/* ! ряд справа от индикатора - снизу вверх */
	{ KIF_NONE,		KBD_CODE_LOCK,			KBD_CODE_LOCK_HOLDED, 	' ', },		// LOCK
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// BAND DOWN
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },		// BAND UP
	{ KIF_NONE,		KBD_CODE_MODE,			KBD_CODE_MODEMOD, 		' ', },		// MODE/SUBMODE

	/* ! матрица слева от индикатора - ряд 1 по счету слева - снизу вверх */
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,		KBD_CODE_MENU, 			' ', },		// DISPLAY/MENU
	{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE,		' ', },		// MOX/TUNE
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,		KBD_CODE_ATUSTART,		' ', },		// TUNER BYP/ATU START
	{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED,		' ', },		// SPLIT/SPLIT OFF

	/* ! матрица слева от индикатора - ряд 4 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_MENU_CWSPEED,	KBD_CODE_11, 			' ', },		// CW SPEED
	{ KIF_NONE,		KBD_CODE_NOTCHTOGGLE,	KBD_CODE_NR, 			' ', },		// NOTCH
	{ KIF_NONE,		KBD_CODE_BW,			KBD_CODE_12, 			' ', },		// BW/NR
	{ KIF_NONE,		KBD_CODE_DWATCHTOGGLE,	KBD_CODE_DWATCHHOLD,	' ', },		// DUAL

	/* матрица слева от индикатора - ряд 3 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_BKIN,			KBD_CODE_BKIN_HOLDED, 	' ', },		// BREAK-IN
	{ KIF_NONE,		KBD_CODE_LDSPTGL,		KBD_CODE_15,			' ', },		// SPK
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			' ', },		// ATT/PRE
	{ KIF_NONE,		KBD_CODE_A_EX_B,		KBD_CODE_17, 			' ', },		// A/B
    //{ KIF_NONE,     KBD_CODE_CWMSG2,        KBD_CODE_CWMSG4,         ' ', },        // A/B - CW MESSAGES

	/* матрица слева от индикатора - ряд 2 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_VOXTOGGLE,		KBD_CODE_DATATOGGLE,	' ', },		// VOX/DATA
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE,	KBD_CODE_RECORD_HOLDED,	' ', },		// REC
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_ANTENNA_HOLDED,' ', },		// ANTENNA
	{ KIF_NONE,		KBD_CODE_A_EQ_B,		KBD_CODE_21, 			' ', },		// A=B
    //{ KIF_NONE,     KBD_CODE_CWMSG1,        KBD_CODE_CWMSG3,         ' ', },        // A=B - CW MESSAGES

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_MAX,		KBD_CODE_POWEROFF, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYBOARD_SINGLE	// кнопка только на энкодере

static const struct qmkey qmdefs [NQMKEYS] =
{
	{ KIF_ERASE,		KBD_ENC2_PRESS,			KBD_ENC2_HOLD, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_FPANEL20_V0B

/* 20 кнопок на 5 линий - плата Rfrontpanel_v7a, Rfrontpanel_v4b, Rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении */
/* Новый вариант под корпус с надписями от RK1AQ */
static const struct qmkey qmdefs [NQMKEYS] =
{
	/* вход кнопки валкодера - KI5 */
	{ KIF_NONE,		KBD_ENC2_PRESS,			KBD_ENC2_HOLD,			' ', },
	{ KIF_NONE,		KBD_CODE_MAX,			KBD_CODE_MAX,			' ', },		//  MH31 FST
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },		//  MH31 UP
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		//  MH31 DOWN

	/* ряд справа от индикатора - снизу вверх KI4 */
	{ KIF_NONE,		KBD_CODE_LOCK,			KBD_CODE_LOCK_HOLDED, 	' ', },		// LOCK
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// BAND DOWN
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },		// BAND UP
	{ KIF_NONE,		KBD_CODE_MODE,			KBD_CODE_MODEMOD, 		' ', },		// MODE/SUBMODE

	/* матрица слева от индикатора - ряд 1 по счету слева - снизу вверх KI3 */
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,		KBD_CODE_MENU, 			' ', },		// DISPLAY/MENU
	{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE,		' ', },		// MOX/TUNE
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,		KBD_CODE_ATUSTART,		' ', },		// TUNER BYP/ATU START
	{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED,		' ', },		// SPLIT/SPLIT OFF

	/* матрица слева от индикатора - ряд 4 по счету слева - снизу вверх KI2 */
	{ KIF_NONE,		KBD_CODE_MENU_CWSPEED,	KBD_CODE_11, 			' ', },		// CW SPEED
	{ KIF_NONE,		KBD_CODE_NOTCHTOGGLE,	KBD_CODE_NR, 			' ', },		// NOTCH
	{ KIF_NONE,		KBD_CODE_BW,			KBD_CODE_12, 			' ', },		// BW/NR
	{ KIF_NONE,		KBD_CODE_DWATCHTOGGLE,	KBD_CODE_DWATCHHOLD,	' ', },		// DUAL

	/* матрица слева от индикатора - ряд 3 по счету слева - снизу вверх KI1 */
	{ KIF_NONE,		KBD_CODE_BKIN,			KBD_CODE_BKIN_HOLDED, 	' ', },		// BREAK-IN
	{ KIF_NONE,		KBD_CODE_LDSPTGL,		KBD_CODE_15,			' ', },		// SPK
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			' ', },		// ATT/PRE
	{ KIF_NONE,		KBD_CODE_A_EX_B,		KBD_CODE_17, 			' ', },		// A/B
    //{ KIF_NONE,     KBD_CODE_CWMSG2,        KBD_CODE_CWMSG4,         ' ', },        // A/B - CW MESSAGES

	/* матрица слева от индикатора - ряд 2 по счету слева - снизу вверх KI0 */
	{ KIF_NONE,		KBD_CODE_VOXTOGGLE,		KBD_CODE_DATATOGGLE,	' ', },		// VOX/DATA
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE,	KBD_CODE_RECORD_HOLDED,	' ', },		// REC
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_ANTENNA_HOLDED,' ', },		// ANTENNA
	{ KIF_NONE,		KBD_CODE_A_EQ_B,		KBD_CODE_21, 			' ', },		// A=B
    //{ KIF_NONE,     KBD_CODE_CWMSG1,        KBD_CODE_CWMSG3,         ' ', },        // A=B - CW MESSAGES

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_MAX,		KBD_CODE_POWEROFF, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_VELOCI24_V0_UA1CEI

/* 24 кнопки на 6 линий */
static const struct qmkey qmdefs [NQMKEYS] =
{
	{ KIF_NONE,		KBD_ENC2_PRESS,			KBD_ENC2_HOLD,			' ', },		// 0  reserved
	{ KIF_NONE,		KBD_CODE_MAX,			KBD_CODE_MAX,			' ', },		// 1  MH31 FST
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },		// 2  MH31 UP
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// 3  MH31 DOWN
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP,	 	' ', },		// 4  BAND UP on front panel
	{ KIF_NONE,		KBD_CODE_LOCK,			KBD_CODE_LOCK_HOLDED, 	' ', },		// 5  LOCK
	{ KIF_NONE,		KBD_CODE_MAX,			KBD_CODE_MAX, 			' ', },		// 6  STEP?
	{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX, 			' ', },		// 7  F4
	{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX, 			' ', },		// 8  F3
	{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX,			' ', },		// 9  F2
	{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX,			' ', },		// 10 F1
	{ KIF_NONE,		KBD_CODE_BW,			KBD_ENC2F_HOLD, 		' ', },		// 11 FILTER - 2nd small encoder (encoder_ENC2F object)
	{ KIF_NONE,		KBD_CODE_MEMO,			KBD_CODE_MEMO_HOLDED, 	' ', },		// 12 MEMO
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// 13 BAND DOWN on front panel
	{ KIF_NONE,		KBD_CODE_MODE,			KBD_CODE_MODEMOD,		' ', },		// 14 FUNC A - 3rd small encoder (encoder_ENC3F object)
	{ KIF_NONE,		KBD_CODE_DISPMODE,		KBD_CODE_MENU,			' ', },		// 15 FUNC B - 4th small encoder (encoder_ENC4F object)
	{ KIF_NONE,		KBD_CODE_A_EQ_B,		KBD_CODE_MAX,			' ', },		// 16 A=B
	{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED,	' ', },		// 17 RIT
	{ KIF_NONE,		KBD_CODE_A_EX_B,		KBD_CODE_MAX,			' ', },		// 18 A/B
	{ KIF_NONE,		KBD_ENC1F_PRESS,		KBD_ENC1F_HOLD,			' ', },		// 19 RF/SPD - 1st small encoder (encoder_ENC1F object)
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			' ', },		// 20 ATT/IPO ATT/PRE
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_ANTENNA_HOLDED,' ', },		// 21 ANT1/ANT2
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,		KBD_CODE_ATUSTART,		' ', },		// 22 TUNER BYP/ATU START
	{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE,		' ', },		// 23 MOX/TUNE

	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_LDSPTGL,		KBD_CODE_POWEROFF, 		' ', },		// 24 SPK/POWER
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_VELOCI24_V0

/* 24 кнопки на 6 линий */
static const struct qmkey qmdefs [NQMKEYS] =
{
	{ KIF_NONE,		KBD_ENC2_PRESS,			KBD_ENC2_HOLD,			' ', },		// 0  reserved
	{ KIF_NONE,		KBD_CODE_MAX,			KBD_CODE_MAX,			' ', },		// 1  MH31 FST
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },		// 2  MH31 UP
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// 3  MH31 DOWN
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP,	 	' ', },		// 4  BAND UP on front panel
	{ KIF_NONE,		KBD_CODE_LOCK,			KBD_CODE_LOCK_HOLDED, 	' ', },		// 5  LOCK
	{ KIF_NONE,		KBD_CODE_MAX,			KBD_CODE_MAX, 			' ', },		// 6  STEP?
	{ KIF_NONE,		KBD_CODE_CWMSG4,		KBD_CODE_MAX, 			' ', },		// 7  F4
	{ KIF_NONE,		KBD_CODE_CWMSG3,		KBD_CODE_MAX, 			' ', },		// 8  F3
	{ KIF_NONE,		KBD_CODE_CWMSG2,		KBD_CODE_MAX,			' ', },		// 9  F2
	{ KIF_NONE,		KBD_CODE_CWMSG1,		KBD_CODE_MAX,			' ', },		// 10 F1
	{ KIF_NONE,		KBD_CODE_BW,			KBD_ENC2F_HOLD, 		' ', },		// 11 FILTER - 2nd small encoder (encoder_ENC2F object)
	{ KIF_NONE,		KBD_CODE_MEMO,			KBD_CODE_MEMO_HOLDED, 	' ', },		// 12 MEMO
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// 13 BAND DOWN on front panel
	{ KIF_NONE,		KBD_CODE_MODE,			KBD_CODE_MODEMOD,		' ', },		// 14 FUNC A - 3rd small encoder (encoder_ENC3F object)
	{ KIF_NONE,		KBD_CODE_DISPMODE,		KBD_CODE_MENU,			' ', },		// 15 FUNC B - 4th small encoder (encoder_ENC4F object)
	{ KIF_NONE,		KBD_CODE_A_EQ_B,		KBD_CODE_MAX,			' ', },		// 16 A=B
	{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED,	' ', },		// 17 RIT
	{ KIF_NONE,		KBD_CODE_A_EX_B,		KBD_CODE_MAX,			' ', },		// 18 A/B
	{ KIF_NONE,		KBD_ENC1F_PRESS,		KBD_ENC1F_HOLD,			' ', },		// 19 RF/SPD - 1st small encoder (encoder_ENC1F object)
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			' ', },		// 20 ATT/IPO ATT/PRE
	{ KIF_NONE,		KBD_CODE_ANTENNA_HOLDED,		KBD_CODE_MAX,' ', },		// 21 ANT1/ANT2
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,		KBD_CODE_ATUSTART,		' ', },		// 22 TUNER BYP/ATU START
	{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE,		' ', },		// 23 MOX/TUNE

	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_LDSPTGL,		KBD_CODE_POWEROFF, 		' ', },		// 24 SPK/POWER
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_FPANEL20_V0A_RA4ASN

/* 20 кнопок на 5 линий - плата Rfrontpanel_v7a, Rfrontpanel_v4b, Rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении */
/* Новый вариант под корпус с надписями от RK1AQ */
/* дополнительно коды для прямого ввода частоты */
static const struct qmkey qmdefs [NQMKEYS] =
{
	/* ! ряд справа от индикатора - снизу вверх */
	{ KIF_NONE,		KBD_CODE_LOCK,			KBD_CODE_LOCK_HOLDED, 	' ', },		// LOCK
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// BAND DOWN
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },		// BAND UP
	{ KIF_NONE,		KBD_CODE_MODE,			KBD_CODE_MODEMOD, 		' ', },		// MODE/SUBMODE

	/* ! матрица слева от индикатора - ряд 1 по счету слева - снизу вверх */
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,		KBD_CODE_MENU, 			' ', },		// DISPLAY/MENU
	{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE,		'7', },		// MOX/TUNE
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,		KBD_CODE_ATUSTART,		'4', },		// TUNER BYP/ATU START
	{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED,	'1', },		// SPLIT/SPLIT OFF

	/* ! матрица слева от индикатора - ряд 4 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_MENU_CWSPEED,	KBD_CODE_11, 			' ', },		// CW SPEED
	{ KIF_NONE,		KBD_CODE_NOTCHTOGGLE,	KBD_CODE_NOTCHFREQ, 	' ', },		// NOTCH
	{ KIF_NONE,		KBD_CODE_BW,			KBD_CODE_NR, 			' ', },		// BW/NR
	{ KIF_NONE,		KBD_CODE_DWATCHTOGGLE,	KBD_CODE_DWATCHHOLD,	' ', },		// DUAL

	/* матрица слева от индикатора - ряд 3 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_BKIN,			KBD_CODE_BKIN_HOLDED, 	'#', },		// BREAK-IN
	{ KIF_NONE,		KBD_CODE_LDSPTGL,		KBD_CODE_15,			'9', },		// SPK
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			'6', },		// ATT/PRE
	{ KIF_NONE,		KBD_CODE_A_EX_B,		KBD_CODE_17, 			'3', },		// A/B

	/* матрица слева от индикатора - ряд 2 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_VOXTOGGLE,		KBD_CODE_DATATOGGLE,	'0', },		// VOX/DATA
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE,	KBD_CODE_RECORD_HOLDED,	'8', },		// REC
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_ANTENNA_HOLDED, 			'5', },		// ANTENNA
	{ KIF_NONE,		KBD_CODE_A_EQ_B,		KBD_CODE_21, 			'2', },		// A=B

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_MAX,		KBD_CODE_POWEROFF, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYBOARD_MATRIX_4x4

static const struct qmkey qmdefs [NQMKEYS] =
{
	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,			KBD_ENC2_HOLD, 			' ', },

	/* ! матрица слева от индикатора - ряд 1 по счету слева - снизу вверх */
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,		KBD_CODE_MENU, 			' ', },		// DISPLAY/MENU
	{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE,		'7', },		// MOX/TUNE
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,		KBD_CODE_ATUSTART,		'4', },		// TUNER BYP/ATU START
	{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED,	'1', },		// SPLIT/SPLIT OFF

	/* ! матрица слева от индикатора - ряд 4 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_MENU_CWSPEED,	KBD_CODE_11, 			' ', },		// CW SPEED
	{ KIF_NONE,		KBD_CODE_NOTCHTOGGLE,	KBD_CODE_NOTCHFREQ, 	' ', },		// NOTCH
	{ KIF_NONE,		KBD_CODE_BW,			KBD_CODE_NR, 			' ', },		// BW/NR
	{ KIF_NONE,		KBD_CODE_DWATCHTOGGLE,	KBD_CODE_DWATCHHOLD,	' ', },		// DUAL

	/* матрица слева от индикатора - ряд 3 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_BKIN,			KBD_CODE_BKIN_HOLDED, 	'#', },		// BREAK-IN
	{ KIF_NONE,		KBD_CODE_LDSPTGL,		KBD_CODE_15,			'9', },		// SPK
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			'6', },		// ATT/PRE
	{ KIF_NONE,		KBD_CODE_A_EX_B,		KBD_CODE_17, 			'3', },		// A/B

	/* матрица слева от индикатора - ряд 2 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_VOXTOGGLE,		KBD_CODE_DATATOGGLE,	'0', },		// VOX/DATA
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE,	KBD_CODE_RECORD_HOLDED,	'8', },		// REC
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_ANTENNA_HOLDED, 			'5', },		// ANTENNA
	{ KIF_NONE,		KBD_CODE_A_EQ_B,		KBD_CODE_21, 			'2', },		// A=B
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_FPANEL20_V0A_PLAYFILE

/* 20 кнопок на 5 линий - плата Rfrontpanel_v7a, Rfrontpanel_v4b, Rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении */
/* Новый вариант под корпус с надписями от RK1AQ */
static const struct qmkey qmdefs [NQMKEYS] =
{
		/* ! ряд справа от индикатора - снизу вверх */
		{ KIF_NONE,		KBD_CODE_LOCK,			KBD_CODE_LOCK_HOLDED, 	' ', },		// LOCK
		{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// BAND DOWN
		{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },		// BAND UP
		{ KIF_NONE,		KBD_CODE_MODE,			KBD_CODE_MODEMOD, 		' ', },		// MODE/SUBMODE

		/* ! матрица слева от индикатора - ряд 1 по счету слева - снизу вверх */
		{ KIF_EXTMENU,	KBD_CODE_DISPMODE,		KBD_CODE_MENU, 			' ', },		// DISPLAY/MENU
		{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE,		' ', },		// MOX/TUNE
		{ KIF_NONE,		KBD_CODE_PLAYFILE1,		KBD_CODE_11,			' ', },		// TUNER BYP/ATU START
		{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED,	' ', },		// SPLIT/SPLIT OFF

		/* ! матрица слева от индикатора - ряд 4 по счету слева - снизу вверх */
		{ KIF_NONE,		KBD_CODE_MENU_CWSPEED,	KBD_CODE_12, 			' ', },		// CW SPEED
		{ KIF_NONE,		KBD_CODE_NOTCHTOGGLE,	KBD_CODE_NOTCHFREQ, 	' ', },		// NOTCH
		{ KIF_NONE,		KBD_CODE_BW,			KBD_CODE_NR, 			' ', },		// BW/NR
		{ KIF_NONE,		KBD_CODE_PLAYFILE5,		KBD_CODE_13,			' ', },		// DUAL

		/* матрица слева от индикатора - ряд 3 по счету слева - снизу вверх */
		{ KIF_NONE,		KBD_CODE_BKIN,			KBD_CODE_BKIN_HOLDED, 	' ', },		// BREAK-IN
		{ KIF_NONE,		KBD_CODE_LDSPTGL,		KBD_CODE_14,			' ', },		// SPK
		{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			' ', },		// ATT/PRE
		{ KIF_NONE,		KBD_CODE_PLAYFILE4,		KBD_CODE_15, 			' ', },		// A/B

		/* матрица слева от индикатора - ряд 2 по счету слева - снизу вверх */
		{ KIF_NONE,		KBD_CODE_VOXTOGGLE,		KBD_CODE_DATATOGGLE,	' ', },		// VOX/DATA
		{ KIF_NONE,		KBD_CODE_PLAYSTOP,		KBD_CODE_PLAYSTOP,		' ', },		// REC
		{ KIF_NONE,		KBD_CODE_PLAYFILE2,		KBD_CODE_19, 			' ', },		// ANTENNA
		{ KIF_NONE,		KBD_CODE_PLAYFILE3,		KBD_CODE_21, 			' ', },		// A=B

		/* кнопка дополнительного валкодера */
		{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
		/* кнопка выключения питания (включение аппаратно) */
		{ KIF_POWER, 	KBD_CODE_MAX,		KBD_CODE_POWEROFF, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_FPANEL30_V3_BASE

/* 30 кнопок на 5 линий - плата rfrontpanel_v3 + LCDMODE_S1D13781 & LCDMODE_LQ043T3DX02K в нормальном расположении */
static const struct qmkey qmdefs [NQMKEYS] =
{
	/* справа от индикатора, крайний правый (дальний от инлдикатора) - снизу вверх */
	{ KIF_NONE,		KBD_CODE_LOCK,			KBD_CODE_LOCK_HOLDED, 	' ', },		// LOCK
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// BAND DOWN
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },		// BAND UP
	{ KIF_NONE,		KBD_CODE_MODE,			KBD_CODE_MODEMOD, 		' ', },		// MODE/SUBMODE
	{ KIF_NONE,		KBD_CODE_1,				KBD_CODE_2,				' ', },
	{ KIF_NONE,		KBD_CODE_3,				KBD_CODE_4,				' ', },

	/* Cлева от индикатора - ряд 1 по счету слева - снизу вверх */
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,		KBD_CODE_MENU, 			' ', },		// DISPLAY/MENU
	{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE,		' ', },		// MOX/TUNE
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,		KBD_CODE_ATUSTART,		' ', },		// TUNER BYP/ATU START
	{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED,	' ', },		// SPLIT/SPLIT OFF
	{ KIF_NONE,		KBD_CODE_5,				KBD_CODE_6,				' ', },
	{ KIF_NONE,		KBD_CODE_7,				KBD_CODE_8,				' ', },

	/* справа от индикатора, второй от края - снизу вверх */
	{ KIF_NONE,		KBD_CODE_MENU_CWSPEED,	KBD_CODE_11, 			' ', },		// CW SPEED
	{ KIF_NONE,		KBD_CODE_NOTCHTOGGLE,	KBD_CODE_NOTCHFREQ, 	' ', },		// NOTCH
	{ KIF_NONE,		KBD_CODE_BW,			KBD_CODE_NR, 			' ', },		// BW/NR
	{ KIF_NONE,		KBD_CODE_DWATCHTOGGLE,	KBD_CODE_DWATCHHOLD,	' ', },		// DUAL
	{ KIF_NONE,		KBD_CODE_9,				KBD_CODE_10,			' ', },
	{ KIF_NONE,		KBD_CODE_11,			KBD_CODE_12,			' ', },

	/* справа от индикатора, ближний к индикатиору - снизу вверх */
	{ KIF_NONE,		KBD_CODE_BKIN,			KBD_CODE_BKIN_HOLDED, 	' ', },		// BREAK-IN
	{ KIF_NONE,		KBD_CODE_LDSPTGL,		KBD_CODE_DATATOGGLE,	' ', },		// SPK/DATA
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			' ', },		// ATT/PRE
	{ KIF_NONE,		KBD_CODE_A_EX_B,		KBD_CODE_18, 			' ', },		// A/B
	{ KIF_NONE,		KBD_CODE_13,			KBD_CODE_14,			' ', },
	{ KIF_NONE,		KBD_CODE_15,			KBD_CODE_16,			' ', },

	/* Cлева от индикатора - ряд 2 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_VOXTOGGLE,		KBD_CODE_MAX,			' ', },		// VOX
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE,	KBD_CODE_RECORD_HOLDED,	' ', },		// REC
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_ANTENNA_HOLDED, 			' ', },		// ANTENNA
	{ KIF_NONE,		KBD_CODE_A_EQ_B,		KBD_CODE_22, 			' ', },		// A=B
	{ KIF_NONE,		KBD_CODE_17,			KBD_CODE_18,			' ', },
	{ KIF_NONE,		KBD_CODE_19,			KBD_CODE_20,			' ', },

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_MAX,		KBD_CODE_POWEROFF, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_FPANEL30_V3

/* 30 кнопок на 5 линий - плата rfrontpanel_v3 + LCDMODE_S1D13781 & LCDMODE_LQ043T3DX02K в нормальном расположении */
static const struct qmkey qmdefs [NQMKEYS] =
{
	/* RRR справа от индикатора, крайний правый (дальний от инлдикатора) - снизу вверх */
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// BAND DOWN
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },		// BAND UP
	{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED, 	'#', },		// SPLIT/SPLIT OFF
	{ KIF_NONE,		KBD_CODE_BAND_24M9,		KBD_CODE_MENU_CWSPEED, 	'9', },		// CW SPEED
	{ KIF_NONE,		KBD_CODE_BAND_14M0,		KBD_CODE_1,				'6', },		//
	{ KIF_NONE,		KBD_CODE_BAND_5M3,		KBD_CODE_A_EX_B,		'3', },		// A/B

	/* LLL Cлева от индикатора - ряд 1 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE,		' ', },		// MOX/TUNE
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,		KBD_CODE_ATUSTART,		' ', },		// TUNER BYP/ATU START
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE,	KBD_CODE_RECORD_HOLDED,	' ', },		// REC/SD FORMAT
	{ KIF_NONE,		KBD_CODE_NOTCHTOGGLE,	KBD_CODE_NOTCHFREQ, 	' ', },		// NOTCH
	{ KIF_NONE,		KBD_CODE_PAMP,			KBD_CODE_5, 			' ', },		// PRE
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_6,				' ', },		// ATT

	/* RRR справа от индикатора, второй от края - снизу вверх */
	{ KIF_NONE,		KBD_CODE_LOCK,			KBD_CODE_LOCK_HOLDED, 	' ', },		// LOCK
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,		KBD_CODE_ENTERFREQDONE,	' ', },		// enter frequency
	{ KIF_NONE,		KBD_CODE_BAND_50M0,		KBD_CODE_BKIN,			'0', },		// BREAK-IN
	{ KIF_NONE,		KBD_CODE_BAND_21M0,		KBD_CODE_LDSPTGL,		'8', },		// Seaker on/off
	{ KIF_NONE,		KBD_CODE_BAND_10M1,		KBD_CODE_BAND_10M1,		'5', },		// 11
	{ KIF_NONE,		KBD_CODE_BAND_3M5,		KBD_CODE_A_EQ_B,		'2', },		// A=B

	/* RRR справа от индикатора, ближний к индикатиору - снизу вверх */
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,		KBD_CODE_MENU,			' ', },		// MENU
	{ KIF_NONE,		KBD_CODE_A_EX_B,		KBD_CODE_10, 			' ', },		// A/B
	{ KIF_NONE,		KBD_CODE_BAND_28M0,		KBD_CODE_VOXTOGGLE,		'.', },		// VOX
	{ KIF_NONE,		KBD_CODE_BAND_18M0,		KBD_CODE_DATATOGGLE,	'7', },		// DATA MODE
	{ KIF_NONE,		KBD_CODE_BAND_7M0,		KBD_CODE_ANTENNA,		'4', },		// ANTENNA
	{ KIF_NONE,		KBD_CODE_BAND_1M8,		KBD_CODE_11,			'1', },		//

	/* LLL Cлева от индикатора - ряд 2 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_BW,			KBD_CODE_NR, 			' ', },		// BW/NR
	{ KIF_NONE,		KBD_CODE_LDSPTGL,		KBD_CODE_DATATOGGLE,	' ', },		// BW/NR
	{ KIF_NONE,		KBD_CODE_MODE_3,		KBD_CODE_MODEMOD_3, 	' ', },		// MODE/SUBMODE
	{ KIF_NONE,		KBD_CODE_MODE_2,		KBD_CODE_MODEMOD_2, 	' ', },		// MODE/SUBMODE
	{ KIF_NONE,		KBD_CODE_MODE_1,		KBD_CODE_MODEMOD_1, 	' ', },		// MODE/SUBMODE
	{ KIF_NONE,		KBD_CODE_MODE_0,		KBD_CODE_MODEMOD_0, 	' ', },		// MODE/SUBMODE

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_MAX,		KBD_CODE_POWEROFF, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_FPANEL20_V0A_RA1AGO

/* 20 кнопок на 5 линий - перевернутые */
static const struct qmkey qmdefs [NQMKEYS] =
{
	/* ! ряд слева от индикатора - сверху вниз */
	{ KIF_NONE,		KBD_CODE_MODE,			KBD_CODE_MODEMOD, 		' ', },		// MODE/SUBMODE
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },		// BAND UP
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// BAND DOWN
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,		KBD_CODE_MENU, 			' ', },		// DISPLAY/MENU

	/* ! матрица справа от индикатора - ряд 1 4 по счету справа - сверху вниз */
	{ KIF_NONE,		KBD_CODE_DWATCHTOGGLE,	KBD_CODE_DWATCHHOLD,	'3', },		// DUAL
	{ KIF_NONE,		KBD_CODE_BW,			KBD_CODE_NR, 			'6', },		// BW/NR
	{ KIF_NONE,		KBD_CODE_NOTCHTOGGLE,	KBD_CODE_NOTCHFREQ, 	'9', },		// NOTCH
	{ KIF_NONE,		KBD_CODE_MENU_CWSPEED,	KBD_CODE_11, 			'#', },		// CW SPEED

	/* ! матрица справа от индикатора - ряд 4 1 по счету справа - сверху вниз */
	{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED,	' ', },		// SPLIT/SPLIT OFF
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,		KBD_CODE_ATUSTART,		' ', },		// TUNER BYP/ATU START
	{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE,		' ', },		// MOX/TUNE
	{ KIF_NONE,		KBD_CODE_LOCK,			KBD_CODE_LOCK_HOLDED, 	' ', },		// LOCK

	/* матрица справа от индикатора - ряд 3 2 по счету справа - сверху вниз */
#if WITHAMHIGHKBDADJ
	{ KIF_SLOW4,	KBD_CODE_AMBANDPASSDOWN,	KBD_CODE_AMBANDPASSDOWN, 	'1', },	// #1: F3
#else /* WITHAMHIGHKBDADJ */
	{ KIF_NONE,		KBD_CODE_A_EQ_B,		KBD_CODE_21, 			'1', },		// A=B
#endif /* WITHAMHIGHKBDADJ */
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_ANTENNA_HOLDED, 			'4', },		// ANTENNA
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE,	KBD_CODE_RECORD_HOLDED,	'7', },		// REC
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,		KBD_CODE_ENTERFREQDONE,	'*', },		// direct frequencu enter

	/* матрица справа от индикатора - ряд 2 3 по счету справа - сверху вниз */
#if WITHAMHIGHKBDADJ
	{ KIF_SLOW4,	KBD_CODE_AMBANDPASSUP,		KBD_CODE_AMBANDPASSUP,	'2', },	// #2: F2
#else /* WITHAMHIGHKBDADJ */
	{ KIF_NONE,		KBD_CODE_A_EX_B,			KBD_CODE_18, 		'2', },		// A/B
#endif /* WITHAMHIGHKBDADJ */
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			'5', },		// ATT/PRE
	{ KIF_NONE,		KBD_CODE_LDSPTGL,		KBD_CODE_DATATOGGLE,	'8', },		// SPK/DATA
	{ KIF_NONE,		KBD_CODE_BKIN,			KBD_CODE_BKIN_HOLDED, 	'0', },		// BREAK-IN

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_MAX,		KBD_CODE_POWEROFF, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_FPANEL20_V0A_AVB

/* 20 кнопок на 5 линий - перевернутые */
static const struct qmkey qmdefs [NQMKEYS] =
{
	/* ! ряд слева от индикатора - сверху вниз */
	{ KIF_NONE,		KBD_CODE_MODE,			KBD_CODE_MODEMOD, 		' ', },		// MODE/SUBMODE
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },		// BAND UP
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// BAND DOWN
	{ KIF_NONE,		KBD_CODE_LOCK,			KBD_CODE_LOCK_HOLDED, 	' ', },		// LOCK

	/* ! матрица справа от индикатора - ряд 1 4 по счету справа - сверху вниз */
	{ KIF_NONE,		KBD_CODE_DWATCHTOGGLE,	KBD_CODE_DWATCHHOLD,	'3', },		// DUAL
	{ KIF_NONE,		KBD_CODE_BW,			KBD_CODE_NR, 			'6', },		// BW/NR
	{ KIF_NONE,		KBD_CODE_NOTCHTOGGLE,	KBD_CODE_NOTCHFREQ, 	'9', },		// NOTCH
	{ KIF_NONE,		KBD_CODE_MENU_CWSPEED,	KBD_CODE_11, 			'#', },		// CW SPEED

	/* ! матрица справа от индикатора - ряд 4 1 по счету справа - сверху вниз */
	{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED,	' ', },		// SPLIT/SPLIT OFF
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,		KBD_CODE_ATUSTART,		' ', },		// TUNER BYP/ATU START
	{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE,		' ', },		// MOX/TUNE
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,		KBD_CODE_MENU, 			' ', },		// DISPLAY/MENU

	/* матрица справа от индикатора - ряд 3 2 по счету справа - сверху вниз */
#if WITHAMHIGHKBDADJ
	{ KIF_SLOW4,	KBD_CODE_AMBANDPASSDOWN,	KBD_CODE_AMBANDPASSDOWN, 	'1', },	// #1: F3
#else /* WITHAMHIGHKBDADJ */
	{ KIF_NONE,		KBD_CODE_A_EQ_B,		KBD_CODE_21, 			'1', },		// A=B
#endif /* WITHAMHIGHKBDADJ */
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_ANTENNA_HOLDED, '4', },		// ANTENNA
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE,	KBD_CODE_RECORD_HOLDED,	'7', },		// REC
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,		KBD_CODE_ENTERFREQDONE,	'*', },		// direct frequencu enter

	/* матрица справа от индикатора - ряд 2 3 по счету справа - сверху вниз */
#if WITHAMHIGHKBDADJ
	{ KIF_SLOW4,	KBD_CODE_AMBANDPASSUP,		KBD_CODE_AMBANDPASSUP,	'2', },	// #2: F2
#else /* WITHAMHIGHKBDADJ */
	{ KIF_NONE,		KBD_CODE_A_EX_B,			KBD_CODE_18, 		'2', },		// A/B
#endif /* WITHAMHIGHKBDADJ */
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			'5', },		// ATT/PRE
	{ KIF_NONE,		KBD_CODE_LDSPTGL,		KBD_CODE_DATATOGGLE,	'8', },		// SPK/DATA
	{ KIF_NONE,		KBD_CODE_BKIN,			KBD_CODE_BKIN_HOLDED, 	'0', },		// BREAK-IN

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	// power button
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_FPANEL20_V0A_AVB_NIKULSKI

/* 20 кнопок на 5 линий - перевернутые - в ряду с диапазонами кнопка меню */
static const struct qmkey qmdefs [NQMKEYS] =
{
	/* ! ряд слева от индикатора - сверху вниз */
	{ KIF_NONE,		KBD_CODE_MODE,			KBD_CODE_MODEMOD, 		' ', },		// MODE/SUBMODE
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },		// BAND UP
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// BAND DOWN
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,		KBD_CODE_MENU, 			' ', },		// DISPLAY/MENU

	/* ! матрица справа от индикатора - ряд 1 4 по счету справа - сверху вниз */
	{ KIF_NONE,		KBD_CODE_DWATCHTOGGLE,	KBD_CODE_DWATCHHOLD,	'3', },		// DUAL
	{ KIF_NONE,		KBD_CODE_BW,			KBD_CODE_NR, 			'6', },		// BW/NR
	{ KIF_NONE,		KBD_CODE_NOTCHTOGGLE,	KBD_CODE_NOTCHFREQ, 	'9', },		// NOTCH
	{ KIF_NONE,		KBD_CODE_MENU_CWSPEED,	KBD_CODE_11, 			'#', },		// CW SPEED

	/* ! матрица справа от индикатора - ряд 4 1 по счету справа - сверху вниз */
	{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED,	' ', },		// SPLIT/SPLIT OFF
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,		KBD_CODE_ATUSTART,		' ', },		// TUNER BYP/ATU START
	{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE,		' ', },		// MOX/TUNE
	{ KIF_NONE,		KBD_CODE_LOCK,			KBD_CODE_LOCK_HOLDED, 	' ', },		// LOCK

	/* матрица справа от индикатора - ряд 3 2 по счету справа - сверху вниз */
#if WITHAMHIGHKBDADJ
	{ KIF_SLOW4,	KBD_CODE_AMBANDPASSDOWN,	KBD_CODE_AMBANDPASSDOWN, 	'1', },	// #1: F3
#else /* WITHAMHIGHKBDADJ */
	{ KIF_NONE,		KBD_CODE_A_EQ_B,		KBD_CODE_21, 			'1', },		// A=B
#endif /* WITHAMHIGHKBDADJ */
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_ANTENNA_HOLDED, '4', },		// ANTENNA
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE,	KBD_CODE_RECORD_HOLDED,	'7', },		// REC
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,		KBD_CODE_ENTERFREQDONE,	'*', },		// direct frequencu enter

	/* матрица справа от индикатора - ряд 2 3 по счету справа - сверху вниз */
#if WITHAMHIGHKBDADJ
	{ KIF_SLOW4,	KBD_CODE_AMBANDPASSUP,		KBD_CODE_AMBANDPASSUP,	'2', },	// #2: F2
#else /* WITHAMHIGHKBDADJ */
	{ KIF_NONE,		KBD_CODE_A_EX_B,			KBD_CODE_18, 		'2', },		// A/B
#endif /* WITHAMHIGHKBDADJ */
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			'5', },		// ATT/PRE
	{ KIF_NONE,		KBD_CODE_LDSPTGL,		KBD_CODE_DATATOGGLE,	'8', },		// SPK/DATA
	{ KIF_NONE,		KBD_CODE_BKIN,			KBD_CODE_BKIN_HOLDED, 	'0', },		// BREAK-IN

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	// power button
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_FPANEL20_V0A_UA1CEI

/* 20 кнопок на 5 линий - плата rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении */
/* Новый вариант под корпус с надписями от RK1AQ */
static const struct qmkey qmdefs [NQMKEYS] =
{
	/* ! ряд справа от индикатора - снизу вверх */
	{ KIF_NONE,		KBD_CODE_USEFAST,	KBD_CODE_LOCK, 		' ', },
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },		// BAND DOWN
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },		// BAND UP
	{ KIF_NONE,		KBD_CODE_MODE,			KBD_CODE_MODEMOD, 		' ', },		// MODE/SUBMODE

	/* ! матрица слева от индикатора - ряд 1 по счету слева - снизу вверх */
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,		KBD_CODE_MENU, 			' ', },		// DISPLAY/MENU
	{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE,		' ', },		// MOX/TUNE
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,		KBD_CODE_ATUSTART,		' ', },		// TUNER BYP/ATU START
	{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED,		' ', },		// SPLIT/SPLIT OFF

	/* ! матрица слева от индикатора - ряд 4 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_MENU_CWSPEED,	KBD_CODE_11, 			' ', },		// CW SPEED
	{ KIF_NONE,		KBD_CODE_NOTCHTOGGLE,	KBD_CODE_NOTCHFREQ, 	' ', },		// NOTCH
	{ KIF_NONE,		KBD_CODE_BW,			KBD_CODE_13, 			' ', },		// BW/AGC не выключается с клавиатуры
	{ KIF_NONE,		KBD_CODE_DWATCHTOGGLE,	KBD_CODE_DWATCHHOLD,	' ', },		// DUAL

	/* матрица слева от индикатора - ряд 3 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_BKIN,			KBD_CODE_BKIN_HOLDED, 	' ', },		// BREAK-IN
	{ KIF_NONE,		KBD_CODE_LDSPTGL,		KBD_CODE_DATATOGGLE,	' ', },		// SPK/DATA
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			' ', },		// ATT/PRE
	{ KIF_NONE,		KBD_CODE_A_EX_B,		KBD_CODE_18, 			' ', },		// A/B

	/* матрица слева от индикатора - ряд 2 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_VOXTOGGLE,		KBD_CODE_MAX,			' ', },		// VOX
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE,	KBD_CODE_RECORD_HOLDED,	' ', },		// REC
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_ANTENNA_HOLDED,' ', },		// ANTENNA
	{ KIF_NONE,		KBD_CODE_A_EQ_B,		KBD_CODE_21, 			' ', },		// A=B

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_MAX,		KBD_CODE_POWEROFF, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_VERTICAL_REV

/* расположение кнопок для плат "Воробей" и "Колибри" */
static const struct qmkey qmdefs [NQMKEYS] =
{
	//ki2-правый ряд
	{ KIF_NONE,		KBD_CODE_MODE,		KBD_CODE_MODEMOD, 		'3', },
	{ KIF_SLOW,		KBD_CODE_BAND_UP,	KBD_CODE_BAND_UP, 		'6', },
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,	KBD_CODE_BAND_DOWN, 	'9', },
	{ KIF_FAST,		ENC_CODE_STEP_UP,	ENC_CODE_STEP_UP, 		'*', },
	//ki1-центральный ряд
	{ KIF_NONE,		KBD_CODE_MOX,		KBD_CODE_TXTUNE, 		'2', },
	{ KIF_NONE,		KBD_CODE_ATT,		KBD_CODE_PAMP, 			'5', },
	{ KIF_NONE,		KBD_CODE_BW,		KBD_CODE_AGC, 			'8', },
	{ KIF_NONE,		KBD_CODE_LOCK,		KBD_CODE_LOCK_HOLDED, 	'0', },
	//ki0-левый ряд
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,	KBD_CODE_MENU, 			'1', },
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED, 	'4', },
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,	KBD_CODE_ENTERFREQDONE, '7', },
	{ KIF_FAST,		ENC_CODE_STEP_DOWN,	ENC_CODE_STEP_DOWN, 	'#', },

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_MAX,		KBD_CODE_POWEROFF, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_VERTICAL_REV_SIMPLEFREQ

/* расположение кнопок для плат "Воробей" и "Колибри" - только прямой ввод частоты */
static const struct qmkey qmdefs [NQMKEYS] =
{
	//ki2-правый ряд
	{ KIF_NONE,		KBD_CODE_0,		KBD_CODE_0, 	'3', },
	{ KIF_NONE,		KBD_CODE_1,		KBD_CODE_1, 	'6', },
	{ KIF_NONE,		KBD_CODE_2,		KBD_CODE_2, 	'9', },
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,		KBD_CODE_ENTERFREQDONE, 	'*', },
	//ki1-центральный ряд
	{ KIF_NONE,		KBD_CODE_4,		KBD_CODE_4, 	'2', },
	{ KIF_NONE,		KBD_CODE_5,		KBD_CODE_5, 	'5', },
	{ KIF_NONE,		KBD_CODE_6,		KBD_CODE_6, 	'8', },
	{ KIF_NONE,		KBD_CODE_7,		KBD_CODE_7, 	'0', },
	//ki0-левый ряд
	{ KIF_NONE,		KBD_CODE_8,		KBD_CODE_8, 	'1', },
	{ KIF_NONE,		KBD_CODE_9,		KBD_CODE_9, 	'4', },
	{ KIF_NONE,		KBD_CODE_10,	KBD_CODE_10, 	'7', },
	{ KIF_NONE,		KBD_CODE_11,	KBD_CODE_11, 	'#', },

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_MAX,		KBD_CODE_POWEROFF, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_VERTICAL_VALERY

/* расположение кнопок для плат "Воробей" и "Колибри" */
static const struct qmkey qmdefs [NQMKEYS] =
{
	{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX, 			'0', },
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,	KBD_CODE_ENTERFREQDONE, '1', },
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED, 	'4', },
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,	KBD_CODE_MENU, 			'7', },

	{ KIF_NONE,		KBD_CODE_LOCK,		KBD_CODE_LOCK_HOLDED, 	'.', },
	{ KIF_NONE,		KBD_CODE_BW,		KBD_CODE_AGC, 			'2', },
	{ KIF_NONE,		KBD_CODE_ATT,		KBD_CODE_PAMP, 			'5', },
	{ KIF_NONE,		KBD_CODE_MOX,		KBD_CODE_TXTUNE, 		'8', },

	{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX, 			'=', },
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,	KBD_CODE_BAND_DOWN, 	'3', },
	{ KIF_SLOW,		KBD_CODE_BAND_UP,	KBD_CODE_BAND_UP, 		'6', },
	{ KIF_NONE,		KBD_CODE_MODE,		KBD_CODE_MODEMOD, 		'9', },

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_MAX,		KBD_CODE_POWEROFF, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_VERTICAL_REV_IGOR

/* усечённый набор функций */
static const struct qmkey qmdefs [NQMKEYS] =
{
	//ki2-правый ряд
	{ KIF_NONE,		KBD_CODE_0,			KBD_CODE_0, 		'3', },
	{ KIF_SLOW,		KBD_CODE_BAND_UP,	KBD_CODE_BAND_UP, 		'6', },
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,	KBD_CODE_BAND_DOWN, 	'9', },
	{ KIF_FAST,		ENC_CODE_STEP_UP,	ENC_CODE_STEP_UP, 		'*', },
	//ki1-центральный ряд
	{ KIF_NONE,		KBD_CODE_4,			KBD_CODE_4, 		'2', },
	{ KIF_NONE,		KBD_CODE_5,			KBD_CODE_5, 			'5', },
	{ KIF_NONE,		KBD_CODE_6,			KBD_CODE_6, 			'8', },
	{ KIF_NONE,		KBD_CODE_LOCK,		KBD_CODE_7,			'0', },
	//ki0-левый ряд
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,		KBD_CODE_MENU, 			'1', },
	{ KIF_ERASE,	KBD_CODE_ERASECONFIG,			KBD_CODE_9, 	'4', },
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,	KBD_CODE_ENTERFREQDONE, '7', },
	{ KIF_FAST,		ENC_CODE_STEP_DOWN,	ENC_CODE_STEP_DOWN, 	'#', },

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_MAX,		KBD_CODE_POWEROFF, 			' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_ERASECONFIG;
}

#elif KEYB18_GENERIC

/* 

	клавиша	функция
	S1	1 Показ S-метра , MENU
	S2	2
	S3	3
	S4	4
	S5	5
	S6	6
	S7	7
	S8	8
	S9	9
	S10	0, LOCK
	S11	BAND DOWN
	S12	BAND UP
	S13	ATT, PREAMP
	S14	BW, AGC
	S15	MODE, SUBMODE
	S16	SPLIT/A/B, SPLIT OFF
	S17	ENTER FREQ, ENTER OFF
	S18	TX, TUNE
 */

static const struct qmkey qmdefs [NQMKEYS] =
{
	//ki2-левый ряд
	{ KIF_NONE,		KBD_CODE_ATT,		KBD_CODE_PAMP, 			' ', },	// S13
	{ KIF_NONE,		KBD_CODE_BW,		KBD_CODE_AGC, 			' ', },	// S14
	{ KIF_NONE,		KBD_CODE_MODE,		KBD_CODE_MODEMOD, 		' ', },	// S15
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED, 	' ', },	// S16
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,	KBD_CODE_ENTERFREQDONE, ' ', },	// S17
	{ KIF_NONE,		KBD_CODE_MOX,		KBD_CODE_TXTUNE, 		' ', },	// S18
	//ki1-центральный ряд
	{ KIF_NONE,		KBD_CODE_6,			KBD_CODE_6, 			'7', },	// S7
	{ KIF_NONE,		KBD_CODE_7,			KBD_CODE_7, 			'8', },	// S8
	{ KIF_NONE,		KBD_CODE_8,			KBD_CODE_8, 			'9', },	// S9
	{ KIF_NONE,		KBD_CODE_9,			KBD_CODE_LOCK, 			'0', },	// S10
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,	KBD_CODE_BAND_DOWN,		' ', },	// S11
	{ KIF_SLOW,		KBD_CODE_BAND_UP,	KBD_CODE_BAND_UP, 		' ', },	// S12
	//ki0-правый ряд
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,	KBD_CODE_MENU, 			'1', },	// S1
	{ KIF_SLOW,		KBD_CODE_1,			KBD_CODE_1, 			'2', },	// S2
	{ KIF_SLOW,		KBD_CODE_2,			KBD_CODE_2, 			'3', },	// S3
	{ KIF_NONE,		KBD_CODE_3,			KBD_CODE_3, 			'4', }, // S4
	{ KIF_NONE,		KBD_CODE_4,			KBD_CODE_4, 			'5', },	// S5
	{ KIF_NONE,		KBD_CODE_5,			KBD_CODE_5, 			'6', },	// S6
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_VERTICAL_REV_TOPDOWN

/* расположение кнопок для ПЕРЕВЁРНУТЫХ плат "Воробей" и "Колибри" */
static const struct qmkey qmdefs [NQMKEYS] =
{
	//ki2-левый ряд
	{ KIF_FAST,		ENC_CODE_STEP_DOWN,	ENC_CODE_STEP_DOWN, 	'#', },
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,	KBD_CODE_ENTERFREQDONE, '7', },
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED, 	'4', },
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,	KBD_CODE_MENU, 			'1', },
	//ki1-центральный ряд
	{ KIF_NONE,		KBD_CODE_LOCK,		KBD_CODE_LOCK_HOLDED, 	'0', },
	{ KIF_NONE,		KBD_CODE_BW,		KBD_CODE_AGC, 			'8', },
	{ KIF_NONE,		KBD_CODE_ATT,		KBD_CODE_PAMP, 			'5', },
	{ KIF_NONE,		KBD_CODE_MOX,		KBD_CODE_TXTUNE, 		'2', },
	//ki0-правый ряд
	{ KIF_FAST,		ENC_CODE_STEP_UP,	ENC_CODE_STEP_UP, 		'*', },
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,	KBD_CODE_BAND_DOWN, 	'9', },
	{ KIF_SLOW,		KBD_CODE_BAND_UP,	KBD_CODE_BAND_UP, 		'6', },
	{ KIF_NONE,		KBD_CODE_MODE,		KBD_CODE_MODEMOD, 		'3', },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_M0SERG

// три группы по ШЕСТЬ линейки кнопок друг за другом в один ряд.
static const struct qmkey qmdefs [NQMKEYS] =
{
	{ KIF_FAST,		ENC_CODE_STEP_UP,	ENC_CODE_STEP_UP, 		'*', },	// 0
	{ KIF_NONE,		KBD_CODE_LOCK,		KBD_CODE_LOCK_HOLDED, 	'0', },	// 1
	{ KIF_FAST,		ENC_CODE_STEP_DOWN,	ENC_CODE_STEP_DOWN, 	'#', },	// 2
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,	KBD_CODE_BAND_DOWN, 	'9', },	// 3
	{ KIF_NONE,		KBD_CODE_BW,		KBD_CODE_AGC, 			'8', },	// 4
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,	KBD_CODE_ENTERFREQDONE, '7', },	// 5
	{ KIF_SLOW,		KBD_CODE_BAND_UP,	KBD_CODE_BAND_UP, 		'6', },	// 6
	{ KIF_NONE,		KBD_CODE_ATT,		KBD_CODE_PAMP, 			'5', },	// 7
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED, 	'4', },	// 8
	{ KIF_NONE,		KBD_CODE_MODE,		KBD_CODE_MODEMOD, 		'3', },	// 9
	{ KIF_NONE,		KBD_CODE_MOX,		KBD_CODE_TXTUNE, 		'2', },	// 10
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,	KBD_CODE_MENU, 			'1', },	// 11
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_RA1AGG

// три группы по четыре линейки кнопок друг за другом в один ряд.
static const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7(ki2) - middle
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
	{ KIF_NONE, KBD_CODE_ATT, KBD_CODE_PAMP,  },
	{ KIF_NONE, KBD_CODE_BW, KBD_CODE_AGC,  },
	{ KIF_NONE, KBD_CODE_MODE, KBD_CODE_MODEMOD,  },
	// ADC6(ki1) - right
	{ KIF_ERASE, KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED,  },
	{ KIF_SLOW, KBD_CODE_BAND_DOWN, KBD_CODE_BAND_DOWN,  },
	{ KIF_SLOW, KBD_CODE_BAND_UP, KBD_CODE_BAND_UP,  },
	{ KIF_NONE, KBD_CODE_LOCK, KBD_CODE_LOCK_HOLDED,  },
	// ADC5(ki0) - left
	{ KIF_EXTMENU, KBD_CODE_DISPMODE, KBD_CODE_MENU,  },
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
	{ KIF_NONE, KBD_CODE_GEN_HAM, KBD_CODE_GEN_HAM,  },
	{ KIF_NONE, KBD_CODE_MOX, KBD_CODE_TXTUNE,  },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_RA1AGO

/* расположение кнопок для RA1AGO */

static const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2) - middle
	{ KIF_NONE, KBD_CODE_MOX, KBD_CODE_TXTUNE,  },
	{ KIF_NONE, KBD_CODE_LOCK, KBD_CODE_LOCK_HOLDED, }, 
	{ KIF_ERASE, KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED,  },
	{ KIF_EXTMENU, KBD_CODE_DISPMODE, KBD_CODE_MENU,  },
	// ADC6 (ki1) - right
	{ KIF_NONE, KBD_CODE_GEN_HAM, KBD_CODE_GEN_HAM,  },
	{ KIF_FAST, ENC_CODE_STEP_DOWN, ENC_CODE_STEP_DOWN,  },
	{ KIF_FAST, ENC_CODE_STEP_UP, ENC_CODE_STEP_UP,  },
	{ KIF_SLOW, KBD_CODE_BAND_DOWN, KBD_CODE_BAND_DOWN,  },
	// ADC5 (ki0) - left
	{ KIF_SLOW, KBD_CODE_BAND_UP, KBD_CODE_BAND_UP,  },
	{ KIF_NONE, KBD_CODE_ATT, KBD_CODE_PAMP,  },
	{ KIF_NONE, KBD_CODE_MODE, KBD_CODE_MODEMOD,  },
	{ KIF_NONE, KBD_CODE_BW, KBD_CODE_AGC,  },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYB_UA1CEI

static const struct qmkey qmdefs [NQMKEYS] =
{
	// ki2
	{ KIF_NONE,  ENC_CODE_STEP_DOWN, ENC_CODE_STEP_DOWN,   },
	{ KIF_NONE,  ENC_CODE_STEP_UP,  ENC_CODE_STEP_UP,   },
	{ KIF_SLOW,  KBD_CODE_BAND_DOWN,  KBD_CODE_BAND_DOWN,   },
	{ KIF_SLOW,  KBD_CODE_BAND_UP,  KBD_CODE_BAND_UP,   },
	// ki1
	{ KIF_NONE,  KBD_CODE_ATT,  KBD_CODE_PAMP,   },
	{ KIF_NONE,  KBD_CODE_MOX,  KBD_CODE_TXTUNE,   },
	{ KIF_NONE,  KBD_CODE_GEN_HAM,  KBD_CODE_GEN_HAM,   },
	{ KIF_NONE,  KBD_CODE_MODE,  KBD_CODE_MODEMOD,   },
	// ki0
	{ KIF_ERASE, KBD_CODE_SPLIT,  KBD_CODE_SPLIT_HOLDED,   },
	{ KIF_NONE,  KBD_CODE_LOCK,  KBD_CODE_LOCK_HOLDED,   },
	{ KIF_EXTMENU, KBD_CODE_DISPMODE,  KBD_CODE_MENU,   },
	{ KIF_NONE,  KBD_CODE_BW,  KBD_CODE_AGC,   },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#elif KEYBOARD_6BTN
	/* Синтезатор 4Z5KY с двухстрочником http://ur5yfv.ucoz.ua/forum/28-19-2 */

	/*
		FUN - 0
		SET - 67
		UP - 89
		DOWN - 128
		RIT - 175
		LOC - 205 
	*/


	static const struct qmkey qmdefs [NQMKEYS] =
	{
		{ KIF_EXTMENU,  KBD_CODE_DISPMODE, KBD_CODE_MENU,   },
		{ KIF_NONE,  KBD_CODE_MODE,  KBD_CODE_MODEMOD,   },
		{ KIF_SLOW,  KBD_CODE_BAND_UP,  KBD_CODE_BAND_UP,   },
		{ KIF_SLOW,  KBD_CODE_BAND_DOWN,  KBD_CODE_BAND_DOWN,   },
		{ KIF_ERASE,  KBD_CODE_SPLIT,  KBD_CODE_SPLIT_HOLDED,   },
		{ KIF_NONE,  KBD_CODE_ATT,  KBD_CODE_LOCK,   },
		// encoder2 & power buttons
		{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
		{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	};

	uint_fast8_t geterasekey(void)
	{
		return KBD_CODE_SPLIT;
	}

#else

#warning KEYB_XXXX not defined

// базовое расположение клавиш (в соответствии с тем, что в .xls документе описано).
static const struct qmkey qmdefs [NQMKEYS] =
{
	// ki2
	{ KIF_SLOW, KBD_CODE_0, KBD_CODE_0,  },
	{ KIF_SLOW, KBD_CODE_1, KBD_CODE_1,  },
	{ KIF_SLOW, KBD_CODE_2, KBD_CODE_2,  },
	{ KIF_SLOW, KBD_CODE_3, KBD_CODE_3,  },
	// ki1
	{ KIF_SLOW, KBD_CODE_4, KBD_CODE_4,  },
	{ KIF_SLOW, KBD_CODE_5, KBD_CODE_5,  },
	{ KIF_SLOW, KBD_CODE_6, KBD_CODE_6,  },
	{ KIF_SLOW, KBD_CODE_7, KBD_CODE_7,  },
	// ki0
	{ KIF_SLOW, KBD_CODE_8, KBD_CODE_8,  },
	{ KIF_SLOW, KBD_CODE_9, KBD_CODE_9,  },
	{ KIF_SLOW, KBD_CODE_10, KBD_CODE_10,  },
	{ KIF_SLOW, KBD_CODE_11, KBD_CODE_11,  },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_SPLIT;
}

#endif

/* получить код клавиши цифровой клавиатуры по коду, полученному как код нажатия */
uint_fast8_t front_getnumpad(uint_fast8_t key)
{
	uint_fast8_t i;

	for (i = 0; i < NQMKEYS; ++ i)
	{
		if (qmdefs [i].code == key)
			return qmdefs [i].numpad;
	}
	return 0;
}

const struct qmkey * front_get_pressed_pkey(void)
{
	const uint_fast8_t code = board_get_pressed_key();
	return code == KEYBOARD_NOKEY ? NULL : & qmdefs [code];
}

#else /* WITHKEYBOARD */

uint_fast8_t geterasekey(void)
{
	return KBD_CODE_MAX;
}
const struct qmkey * front_get_pressed_pkey(void)
{
	return NULL;
}

#endif /* WITHKEYBOARD */

