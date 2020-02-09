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
#else
	#define NQMKEYS (12 + 2)		// сейчас обслуживается до 12-ти кнопок.
#endif


#if KEYB_7BUTTONSOLD // ! KEYBOARD_USE_ADC

	// Коды для случая работы с семью кнопками, подключёнными прямо на выводы порта

	const struct qmkey qmdefs [NQMKEYS] =
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_7BUTTONS
	// Коды для случая работы с семью кнопками, подключёнными прямо на ADC7 и ADC6

	const struct qmkey qmdefs [NQMKEYS] =
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYBSTYLE_SW2013SF

// Кнопки на передней панели SW-2012-SF
//
// PRE - FUN - MOD - xxx (encoder)
// UP  - DN  - SPL - xxx (additional key lock)
// PWR - TUN - LOCK - BW

const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{	KIF_NONE, 		KBD_CODE_ATT, 		KBD_CODE_MAX, },		// PRE
	{	KIF_EXTMENU,	KBD_CODE_DISPMODE, 	KBD_CODE_MENU, },		// FUN
	{	KIF_NONE, 		KBD_CODE_MODE, 		KBD_CODE_MODEMOD, },	// MOD
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },		// ---
	// ADC6 (ki1)
	{	KIF_SLOW, 		KBD_CODE_BAND_UP, 	KBD_CODE_MAX, },		// UP
	{	KIF_SLOW, 		KBD_CODE_BAND_DOWN,	KBD_CODE_MAX, },		// DOWN
	{	KIF_ERASE, 		KBD_CODE_SPLIT, 	KBD_CODE_SPLIT_HOLDED, },	// SPL
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },		// ---
	// ADC5 (ki0)
	{	KIF_NONE, 		KBD_CODE_PWRTOGGLE,	KBD_CODE_MAX, },		// PWR
	{ 	KIF_NONE,		KBD_CODE_MOX,		KBD_CODE_TXTUNE, },		// TUN
	{	KIF_NONE, 		KBD_CODE_LOCK, 		KBD_CODE_LOCK_HOLDED, },// LOCK
	{	KIF_NONE, 		KBD_CODE_BW,		KBD_CODE_MAX, },		// FST
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif KEYBSTYLE_SW2013SF_US2IT

// Кнопки на передней панели SW-2012-SF
//
// PRE - FUN - MOD - xxx (encoder)
// UP  - DN  - SPL - xxx (additional key lock)
// PWR - TUN - LOCK - BW

const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{	KIF_NONE, 		KBD_CODE_ATT, 			KBD_CODE_PAMP, },		// PRE
	{	KIF_EXTMENU,	KBD_CODE_DISPMODE, 		KBD_CODE_MENU, },		// FUN
	{	KIF_NONE, 		KBD_CODE_MODE, 			KBD_CODE_MODEMOD, },	// MOD
	{	KIF_NONE, 		KBD_CODE_MAX,			KBD_CODE_MAX, },		// ---
	// ADC6 (ki1)
	{	KIF_SLOW, 		KBD_CODE_BAND_UP, 		KBD_CODE_MAX, },		// UP
	{	KIF_SLOW, 		KBD_CODE_BAND_DOWN,		KBD_CODE_MAX, },		// DOWN
	{	KIF_ERASE, 		KBD_CODE_SPLIT, 		KBD_CODE_SPLIT_HOLDED, },	// SPL
	{	KIF_NONE, 		KBD_CODE_MAX,			KBD_CODE_MAX, },		// ---
	// ADC5 (ki0)
	{	KIF_NONE, 		KBD_CODE_PWRTOGGLE,		KBD_CODE_TXTUNE, },		// PWR/TUN
	{ 	KIF_NONE,		KBD_CODE_ATUBYPASS,		KBD_CODE_ATUSTART, },		// BYP/ATU
	{	KIF_NONE, 		KBD_CODE_BW,			KBD_CODE_LOCK, },		// BW/LOC
	{	KIF_NONE, 		KBD_CODE_IFSHIFT,		KBD_CODE_MENU_CWSPEED, },		// IF SH / WPM
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif KEYBSTYLE_SW2016RDX && WITHAUTOTUNER

// Кнопки на передней панели SW2016 с индикатором RDX0154
//
// PRE - FUN - MOD - LOCK
// UP  - DN  - SPL - PWR
// XXX - XXX - XXX - XXX

const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{	KIF_NONE, 		KBD_CODE_ATT, 		KBD_CODE_TXTUNE, },		// ATT
	{	KIF_EXTMENU,	KBD_CODE_BW, 		KBD_CODE_MENU, },		// FUN
	{	KIF_NONE, 		KBD_CODE_MODE, 		KBD_CODE_MODEMOD, },	// MOD
	{	KIF_NONE, 		KBD_CODE_LOCK, 		KBD_CODE_PWRTOGGLE, },// LOCK
	// ADC6 (ki1)
	{	KIF_SLOW, 		KBD_CODE_BAND_UP, 	KBD_CODE_MAX, },		// UP
	{	KIF_SLOW, 		KBD_CODE_BAND_DOWN,	KBD_CODE_MAX, },		// DOWN
	{	KIF_ERASE, 		KBD_CODE_SPLIT, 	KBD_CODE_SPLIT_HOLDED, },	// SPL
	{ 	KIF_NONE,		KBD_CODE_ATUBYPASS,	KBD_CODE_ATUSTART, },		// BYP/ATU: was PWR
	// ADC5 (ki0)
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },	
	//{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX, },
	//{	KIF_NONE, 		KBD_CODE_MAX, 		KBD_CODE_MAX, },
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif KEYBSTYLE_SW2013RDX && WITHAUTOTUNER

// Кнопки на передней панели SW-2012-SF с индикатором RDX0154
//
// PRE - FUN - MOD - LOCK
// UP  - DN  - SPL - PWR
// XXX - XXX - XXX - XXX

const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{	KIF_NONE, 		KBD_CODE_ATT, 		KBD_CODE_TXTUNE, },		// PRE
	{	KIF_EXTMENU,	KBD_CODE_BW, 		KBD_CODE_MENU, },		// FUN
	{	KIF_NONE, 		KBD_CODE_MODE, 		KBD_CODE_MODEMOD, },	// MOD
	{	KIF_NONE, 		KBD_CODE_LOCK, 		KBD_CODE_PWRTOGGLE, },// LOCK
	// ADC6 (ki1)
	{	KIF_SLOW, 		KBD_CODE_BAND_UP, 	KBD_CODE_MAX, },		// UP
	{	KIF_SLOW, 		KBD_CODE_BAND_DOWN,	KBD_CODE_MAX, },		// DOWN
	{	KIF_ERASE, 		KBD_CODE_SPLIT, 	KBD_CODE_SPLIT_HOLDED, },	// SPL
	{ 	KIF_NONE,		KBD_CODE_ATUBYPASS,	KBD_CODE_ATUSTART, },		// BYP/ATU: was PWR
	// ADC5 (ki0)
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },	
	//{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX, },
	//{	KIF_NONE, 		KBD_CODE_MAX, 		KBD_CODE_MAX, },
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif KEYBSTYLE_SW2016RDX

// Кнопки на передней панели SW2016 с индикатором RDX0154
//
// PRE - FUN - MOD - LOCK
// UP  - DN  - SPL - PWR
// XXX - XXX - XXX - XXX

const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{	KIF_NONE, 		KBD_CODE_ATT, 		KBD_CODE_MAX, },		// PRE
	{	KIF_EXTMENU,	KBD_CODE_BW, 		KBD_CODE_MENU, },		// FUN
	{	KIF_NONE, 		KBD_CODE_MODE, 		KBD_CODE_MODEMOD, },	// MOD
	{	KIF_NONE, 		KBD_CODE_LOCK, 		KBD_CODE_MAX, },// LOCK
	// ADC6 (ki1)
	{	KIF_NONE, 		KBD_CODE_BAND_UP, 	KBD_CODE_TXTUNE, },		// TUNE(UP)
	{	KIF_SLOW, 		KBD_CODE_BAND_DOWN,	KBD_CODE_MAX, },		// DOWN
	{	KIF_ERASE, 		KBD_CODE_SPLIT, 	KBD_CODE_SPLIT_HOLDED, },	// SPL
	{	KIF_NONE, 		KBD_CODE_PWRTOGGLE,	KBD_CODE_MAX, },		// PWR
	// ADC5 (ki0)
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },	
	//{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX, },
	//{	KIF_NONE, 		KBD_CODE_MAX, 		KBD_CODE_MAX, },
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif KEYBSTYLE_SW2018XVR

// Кнопки на передней панели SW2016 с индикатором RDX0154
//
// PRE - FUN - MOD - LOCK
// UP  - DN  - SPL - PWR
// XXX - XXX - XXX - XXX

const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{	KIF_NONE, 		KBD_CODE_ATT, 		KBD_CODE_TXTUNE, },		// PRE
	{	KIF_EXTMENU,	KBD_CODE_BW, 		KBD_CODE_MENU, },		// FUN
	{	KIF_NONE, 		KBD_CODE_MODE, 		KBD_CODE_MODEMOD, },	// MOD
	{	KIF_NONE, 		KBD_CODE_LOCK, 		KBD_CODE_MAX, },// LOCK
	// ADC6 (ki1)
	{	KIF_NONE, 		KBD_CODE_BAND_UP, 	KBD_CODE_MAX, },		// TUNE(UP)
	{	KIF_SLOW, 		KBD_CODE_BAND_DOWN,	KBD_CODE_MAX, },		// DOWN
	{	KIF_ERASE, 		KBD_CODE_SPLIT, 	KBD_CODE_SPLIT_HOLDED, },	// SPL
	{	KIF_NONE, 		KBD_CODE_PWRTOGGLE,	KBD_CODE_MAX, },		// PWR
	// ADC5 (ki0)
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },	
	//{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX, },
	//{	KIF_NONE, 		KBD_CODE_MAX, 		KBD_CODE_MAX, },
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif KEYBSTYLE_SW2016RDX_UY5UM_WO240

// Кнопки на передней панели SW2016 с индикатором WO240128
//
// PRE - FUN - MOD - LOCK
// UP  - DN  - SPL - PWR
// XXX - XXX - XXX - XXX
// 
const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{	KIF_NONE, 		KBD_CODE_ATT, 		KBD_CODE_MAX, },		// PRE
	{	KIF_EXTMENU,	KBD_CODE_BW, 		KBD_CODE_MENU, },		// FUN
	{	KIF_NONE, 		KBD_CODE_MODE, 		KBD_CODE_MODEMOD, },	// MOD
	{	KIF_NONE, 		KBD_CODE_ANTENNA, 		KBD_CODE_LOCK, },// LOCK
	// ADC6 (ki1)
	{	KIF_NONE, 		KBD_CODE_BAND_UP, 	KBD_CODE_TXTUNE, },		// TUNE(UP)
	{	KIF_SLOW, 		KBD_CODE_BAND_DOWN,	KBD_CODE_MAX, },		// DOWN
	{	KIF_ERASE, 		KBD_CODE_SPLIT, 	KBD_CODE_SPLIT_HOLDED, },	// SPL
	{	KIF_NONE, 		KBD_CODE_NOTCHTOGGLE,	KBD_CODE_PWRTOGGLE,	},		// PWR
	// ADC5 (ki0)
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },	
	//{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX, },
	//{	KIF_NONE, 		KBD_CODE_MAX, 		KBD_CODE_MAX, },
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif KEYBSTYLE_SW2016RDX_UY5UM

// Кнопки на передней панели SW2016 с индикатором RDX0154
//
// PRE - FUN - MOD - LOCK
// UP  - DN  - SPL - PWR
// XXX - XXX - XXX - XXX

const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{	KIF_NONE, 		KBD_CODE_ATT, 		KBD_CODE_MAX, },		// PRE
	{	KIF_EXTMENU,	KBD_CODE_BW, 		KBD_CODE_MENU, },		// FUN
	{	KIF_NONE, 		KBD_CODE_MODE, 		KBD_CODE_MODEMOD, },	// MOD
	//{	KIF_NONE, 		KBD_CODE_LOCK, 		KBD_CODE_MAX, },// LOCK
	{	KIF_NONE, 		KBD_CODE_ANTENNA, 		KBD_CODE_LOCK, },// ANTENNA UY5UM
	//{	KIF_NONE, 		KBD_CODE_MENU_CWSPEED, 		KBD_CODE_LOCK, },// LOCK UY5UM
	// ADC6 (ki1)
	{	KIF_NONE, 		KBD_CODE_BAND_UP, 	KBD_CODE_TXTUNE, },		// TUNE(UP)
	{	KIF_SLOW, 		KBD_CODE_BAND_DOWN,	KBD_CODE_MAX, },		// DOWN
	{	KIF_ERASE, 		KBD_CODE_SPLIT, 	KBD_CODE_SPLIT_HOLDED, },	// SPL
	{	KIF_NONE, 		KBD_CODE_PWRTOGGLE,	KBD_CODE_MAX, },		// PWR
	// ADC5 (ki0)
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },	
	//{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX, },
	//{	KIF_NONE, 		KBD_CODE_MAX, 		KBD_CODE_MAX, },
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif KEYBSTYLE_SW2013RDX

// Кнопки на передней панели SW-2012-SF с индикатором RDX0154
//
// PRE - FUN - MOD - LOCK
// UP  - DN  - SPL - PWR
// XXX - XXX - XXX - XXX

const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{	KIF_NONE, 		KBD_CODE_ATT, 		KBD_CODE_TXTUNE, },		// PRE
	{	KIF_EXTMENU,	KBD_CODE_BW, 		KBD_CODE_MENU, },		// FUN
	{	KIF_NONE, 		KBD_CODE_MODE, 		KBD_CODE_MODEMOD, },	// MOD
	{	KIF_NONE, 		KBD_CODE_LOCK, 		KBD_CODE_MAX, },// LOCK
	//{	KIF_NONE, 		KBD_CODE_MENU_CWSPEED, 		KBD_CODE_LOCK, },// LOCK UY5UM
	// ADC6 (ki1)
	{	KIF_SLOW, 		KBD_CODE_BAND_UP, 	KBD_CODE_MAX, },		// UP
	{	KIF_SLOW, 		KBD_CODE_BAND_DOWN,	KBD_CODE_MAX, },		// DOWN
	{	KIF_ERASE, 		KBD_CODE_SPLIT, 	KBD_CODE_SPLIT_HOLDED, },	// SPL
	{	KIF_NONE, 		KBD_CODE_PWRTOGGLE,	KBD_CODE_MAX, },		// PWR
	// ADC5 (ki0)
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },	
	//{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX, },
	//{	KIF_NONE, 		KBD_CODE_MAX, 		KBD_CODE_MAX, },
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif KEYBSTYLE_SW2013RDX_UT7LC

// Кнопки на передней панели SW-2012-SF с индикатором RDX0154
//
// PRE - FUN - MOD - LOCK
// UP  - DN  - SPL - PWR (additional key lock)
// TX  - BW  - XXX - XXX

const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{	KIF_NONE, 		KBD_CODE_ATT, 		KBD_CODE_MAX, },		// PRE
	{	KIF_EXTMENU,	KBD_CODE_MAX, 		KBD_CODE_MENU, },		// FUN
	{	KIF_NONE, 		KBD_CODE_MODE, 		KBD_CODE_MODEMOD, },	// MOD
	{	KIF_NONE, 		KBD_CODE_LOCK, 		KBD_CODE_LOCK_HOLDED, },// LOCK
	// ADC6 (ki1)
	{	KIF_SLOW, 		KBD_CODE_BAND_UP, 	KBD_CODE_MAX, },		// UP
	{	KIF_SLOW, 		KBD_CODE_BAND_DOWN,	KBD_CODE_MAX, },		// DOWN
	{	KIF_ERASE, 		KBD_CODE_SPLIT, 	KBD_CODE_SPLIT_HOLDED, },	// SPL
	{	KIF_NONE, 		KBD_CODE_PWRTOGGLE,	KBD_CODE_MAX, },		// PWR
	// ADC5 (ki0)
	{	KIF_NONE, 		KBD_CODE_MOX,		KBD_CODE_TXTUNE, },		// TX/TUNE
	{ 	KIF_NONE,		KBD_CODE_BW,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX, 		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_MENU;
}

#elif KEYBSTYLE_SW2013RDX_LTIYUR

// Кнопки на передней панели SW-2012-SF с индикатором RDX0154
// Версия LTIYUR
//
// PRE - FUN - MOD - LOCK
// UP  - DN  - SPL - PWR (
// XXX - XXX - XXX - NOTCH

const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{	KIF_NONE, 		KBD_CODE_ATT, 		KBD_CODE_TXTUNE, },		// PRE
	{	KIF_EXTMENU,	KBD_CODE_BW, 		KBD_CODE_MENU, },		// FUN
	{	KIF_NONE, 		KBD_CODE_MODE, 		KBD_CODE_MODEMOD, },	// MOD
	{	KIF_NONE, 		KBD_CODE_LOCK, 		KBD_CODE_LOCK_HOLDED, },// LOCK
	// ADC6 (ki1)
	{	KIF_SLOW, 		KBD_CODE_BAND_UP, 	KBD_CODE_MAX, },		// UP
	{	KIF_SLOW, 		KBD_CODE_BAND_DOWN,	KBD_CODE_MAX, },		// DOWN
	{	KIF_ERASE, 		KBD_CODE_SPLIT, 	KBD_CODE_SPLIT_HOLDED, },	// SPL
	{	KIF_NONE, 		KBD_CODE_PWRTOGGLE,	KBD_CODE_MAX, },		// PWR
	// ADC5 (ki0)
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },	
	{ 	KIF_NONE,		KBD_CODE_AGC,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_LDSPTGL, 	KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_NOTCHTOGGLE,		KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif KEYBSTYLE_SW2012_MINI || KEYBSTYLE_SW2012MC || KEYBSTYLE_SW2012CN
// Кнопки на передней панели SW-2012-MINI
// сделано - вместо кнопки BAND UP в режиме автоповтора
//
// PRE - MOD - FUN - xxx (encoder)
// UP  - DN  - SPL - xxx (additional key lock)
// xxx - xxx - xxx - xxx
const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki1)
	{ KIF_NONE,		KBD_CODE_ATT, KBD_CODE_TXTUNE, },
	{ KIF_NONE,		KBD_CODE_MODE, KBD_CODE_MODEMOD, },
	{ KIF_EXTMENU,	KBD_CODE_BW, KBD_CODE_MENU, },
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,	KBD_CODE_ATUSTART, },		// BYP/ATU
	// ADC6 (ki0)
	{ KIF_NONE,		KBD_CODE_BAND_UP, KBD_CODE_LOCK, },
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,KBD_CODE_MAX, },
	{ KIF_ERASE,	KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED, },
	{ KIF_NONE,		KBD_CODE_MAX, KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif KEYBSTYLE_SW2012CN5
// Кнопки на передней панели SW-2012-MINI-V5
//
// PRE - MOD - FUN - xxx (encoder)
// UP  - DN  - SPL - xxx (additional key lock)
const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki1)
	{ KIF_NONE,		KBD_CODE_ATT, KBD_CODE_TXTUNE, },
	{ KIF_NONE,		KBD_CODE_MODE, KBD_CODE_MODEMOD, },
	{ KIF_EXTMENU,	KBD_CODE_BW, KBD_CODE_MENU, },
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,	KBD_CODE_ATUSTART, },		// BYP/ATU
	// ADC6 (ki0)
	{ KIF_NONE,		KBD_CODE_BAND_UP, KBD_CODE_LOCK, },
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,KBD_CODE_MAX, },
	{ KIF_ERASE,	KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED, },
	{ KIF_NONE,		KBD_CODE_MAX, KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif KEYBSTYLE_SW2012CN5_UY5UM

const struct qmkey qmdefs [NQMKEYS] =
{
	//ki2-правый ряд
	{ KIF_NONE,		KBD_CODE_IFSHIFT,	KBD_CODE_10, 			'*', },
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,	KBD_CODE_BAND_DOWN, 	'9', },
	{ KIF_SLOW,		KBD_CODE_BAND_UP,	KBD_CODE_BAND_UP, 		'6', },
	{ KIF_NONE,		KBD_CODE_MODE,		KBD_CODE_MODEMOD, 		'3', },
	//ki1-центральный ряд
	{ KIF_NONE,		KBD_CODE_LOCK,		KBD_CODE_LOCK_HOLDED, 	'0', },
	{ KIF_NONE,		KBD_CODE_BW,		KBD_CODE_AGC, 			'8', },
	{ KIF_NONE,		KBD_CODE_ATT,		KBD_CODE_PAMP, 			'5', },
	{ KIF_NONE,		KBD_CODE_MOX,		KBD_CODE_TXTUNE, 		'2', },
	//ki0-левый ряд
	{ KIF_NONE,		KBD_CODE_MENU_CWSPEED,	KBD_CODE_11,		'#', },
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,	KBD_CODE_ENTERFREQDONE, '7', },
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED, 	'4', },
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,	KBD_CODE_MENU, 			'1', },

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 			' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_POWEROFF,	KBD_CODE_MAX, 			' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_MENU;
}

#elif KEYBSTYLE_SW2012CN5_RN3ZOB_V2
// Кнопки на передней панели SW-2012-MINI-V5
//
// PRE - MOD - FUN - xxx (encoder)
// UP  - DN  - SPL - xxx (additional key lock)
const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{ KIF_NONE,		KBD_CODE_ATT,		KBD_CODE_TXTUNE, },
	{ KIF_NONE,		KBD_CODE_MODE,		KBD_CODE_MODEMOD, },
	{ KIF_EXTMENU,	KBD_CODE_BW,		KBD_CODE_MENU, },
	{ KIF_NONE,		KBD_CODE_ATUBYPASS,	KBD_CODE_ATUSTART, },		// BYP/ATU
	// ADC6 (ki1)
	{ KIF_NONE,		KBD_CODE_BAND_UP,	KBD_CODE_LOCK, },
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,	KBD_CODE_MAX, },
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED, },
	{ KIF_NONE,		KBD_CODE_ANTENNA,	KBD_CODE_MAX, },
	// ADC5 (ki0)
	{ KIF_NONE,		KBD_CODE_USER1,		KBD_CODE_MAX, },
	{ KIF_NONE,		KBD_CODE_USER2,		KBD_CODE_MAX, },
	{ KIF_NONE,		KBD_CODE_USER3,		KBD_CODE_MAX, },
	{ KIF_NONE,		KBD_CODE_USER4,		KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif KEYBSTYLE_SW2011RDX

// раскладка под Станислав Гончар UT4UA
// Кнопки на передней панели SW-2011-RDX
// сделано - lock вместо кнопки BAND UP в режиме автовоповтора
//
// PRE - FUN - MOD - xxx (encoder)
// UP  - DN  - SPL - xxx (additional key lock)
// xxx - xxx - xxx - xxx

const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{	KIF_NONE, 		KBD_CODE_ATT, 		KBD_CODE_TXTUNE, },
	{	KIF_EXTMENU,	KBD_CODE_BW, 		KBD_CODE_MENU, },
	{	KIF_NONE, 		KBD_CODE_MODE, 		KBD_CODE_MODEMOD, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_LOCK, },
	// ADC6 (ki1)
	{	KIF_NONE, 		KBD_CODE_BAND_UP, 	KBD_CODE_LOCK, },
	{	KIF_SLOW, 		KBD_CODE_BAND_DOWN,	KBD_CODE_MAX, },
	{	KIF_ERASE, 		KBD_CODE_SPLIT, 	KBD_CODE_SPLIT_HOLDED, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	// ADC5 (ki0)
	//{	KIF_NONE, 		KBD_CODE_MAX, 		KBD_CODE_MAX, },
	//{	KIF_FAST, 		ENC_CODE_STEP_DOWN, ENC_CODE_STEP_DOWN, },
	//{	KIF_FAST, 		ENC_CODE_STEP_UP, 	ENC_CODE_STEP_UP, },
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif KEYBSTYLE_SW2011N

// раскладка под Станислав Гончар UT4UA
// Кнопки на передней панели SW-2011-RDX
// сделано - lock вместо кнопки BAND UP в режиме автовоповтора
//
// PRE - FUN - MOD - xxx (encoder)
// UP  - DN  - SPL - xxx (additional key lock)
// xxx - xxx - xxx - xxx

const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{	KIF_NONE, 		KBD_CODE_ATT, 		KBD_CODE_TXTUNE, },
	{	KIF_EXTMENU,	KBD_CODE_BW, 		KBD_CODE_MENU, },
	{	KIF_NONE, 		KBD_CODE_MODE, 		KBD_CODE_MODEMOD, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_LOCK, },
	// ADC6 (ki1)
	{	KIF_NONE, 		KBD_CODE_BAND_UP, 	KBD_CODE_LOCK, },
	{	KIF_SLOW, 		KBD_CODE_BAND_DOWN,	KBD_CODE_MAX, },
	{	KIF_ERASE, 		KBD_CODE_SPLIT, 	KBD_CODE_SPLIT_HOLDED, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	// ADC5 (ki0)
	//{	KIF_NONE, 		KBD_CODE_MAX, 		KBD_CODE_MAX, },
	//{	KIF_FAST, 		ENC_CODE_STEP_DOWN, ENC_CODE_STEP_DOWN, },
	//{	KIF_FAST, 		ENC_CODE_STEP_UP, 	ENC_CODE_STEP_UP, },
	//{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}

#elif ELKEY328
// 328 
const struct qmkey qmdefs [NQMKEYS] =
{
	// ADC7 (ki2)
	{ KIF_NONE, KBD_CODE_VIBROCTL, 	KBD_CODE_VIBROCTL,  },		
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },	
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },	
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },	
	// ADC6 (ki1)
	{ KIF_SLOW, KBD_CODE_CWSPEEDUP, 	KBD_CODE_CWSPEEDUP,		 },
	{ KIF_NONE, KBD_CODE_MAX,	KBD_CODE_MAX,	 },
	{ KIF_NONE, KBD_CODE_MAX,	KBD_CODE_MAX, },	
	{ KIF_NONE, KBD_CODE_MAX,	KBD_CODE_MAX,	 },
	// ADC5 (ki0)
	{ KIF_SLOW, KBD_CODE_CWSPEEDDOWN,	KBD_CODE_CWSPEEDDOWN, },	
	{ KIF_NONE, KBD_CODE_MAX,	KBD_CODE_MAX, },	
	{ KIF_NONE, KBD_CODE_MAX,	KBD_CODE_MAX, },	
	{ KIF_NONE, KBD_CODE_MAX,	KBD_CODE_MAX, },	
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_MAX;
}

#elif KEYB_VERTICAL

// повернутое на 90 градусов - 4 строки по 3 кнопки
// увеличение индекса в массиве соответствует направлению перебора кнопок снизу вверх
/* расположение кнопок для плат "Павлин" */

const struct qmkey qmdefs [NQMKEYS] =
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_RAVEN24

/* расположение кнопок для плат "Воробей" с DSP обработкой */
const struct qmkey qmdefs [NQMKEYS] =
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_RAVEN24_RA1AGO

/* расположение кнопок для плат "Воробей" с DSP обработкой - выстаочный экземпляр */
const struct qmkey qmdefs [NQMKEYS] =
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_RAVEN20_V5

/* расположение кнопок для плат "Воробей" с DSP обработкой */
const struct qmkey qmdefs [NQMKEYS] =
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_FPANEL20_V0

/* 20 кнопок на 5 линий - плата rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении */
const struct qmkey qmdefs [NQMKEYS] =
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
	{ KIF_NONE,		KBD_CODE_ANTENNA,	KBD_CODE_17, 		'1', },

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_POWEROFF,	KBD_CODE_MAX, 		' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_FPANEL20_V0A_UY5UM

/* 20 кнопок на 5 линий - плата rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении */
/* Новый вариант под корпус с надписями от RK1AQ */
const struct qmkey qmdefs [NQMKEYS] =
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
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_19, 			' ', },		// ANTEMMA
	{ KIF_NONE,		KBD_CODE_A_EQ_B,			KBD_CODE_21, 		' ', },		// A=B

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_POWEROFF,	KBD_CODE_MAX, 		' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}
#elif KEYB_FPANEL20_V0A

/* 20 кнопок на 5 линий - плата Rfrontpanel_v7a, Rfrontpanel_v4b, Rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении */
/* Новый вариант под корпус с надписями от RK1AQ */
const struct qmkey qmdefs [NQMKEYS] =
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
	{ KIF_NONE,		KBD_CODE_NOTCHTOGGLE,	KBD_CODE_NOTCHFREQ, 	' ', },		// NOTCH
	{ KIF_NONE,		KBD_CODE_BW,			KBD_CODE_NR, 			' ', },		// BW/NR
	{ KIF_NONE,		KBD_CODE_DWATCHTOGGLE,	KBD_CODE_DWATCHHOLD,	' ', },		// DUAL

	/* матрица слева от индикатора - ряд 3 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_BKIN,			KBD_CODE_BKIN_HOLDED, 	' ', },		// BREAK-IN
	{ KIF_NONE,		KBD_CODE_LDSPTGL,		KBD_CODE_15,			' ', },		// SPK
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			' ', },		// ATT/PRE
	{ KIF_NONE,		KBD_CODE_A_EX_B,		KBD_CODE_17, 			' ', },		// A/B

	/* матрица слева от индикатора - ряд 2 по счету слева - снизу вверх */
	{ KIF_NONE,		KBD_CODE_VOXTOGGLE,		KBD_CODE_DATATOGGLE,	' ', },		// VOX/DATA
	{ KIF_NONE,		KBD_CODE_RECORDTOGGLE,	KBD_CODE_RECORD_HOLDED,	' ', },		// REC
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_19, 			' ', },		// ANTEMMA
	{ KIF_NONE,		KBD_CODE_A_EQ_B,		KBD_CODE_21, 			' ', },		// A=B

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_POWEROFF,	KBD_CODE_MAX, 		' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_FPANEL20_V0A_PLAYFILE

/* 20 кнопок на 5 линий - плата Rfrontpanel_v7a, Rfrontpanel_v4b, Rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении */
/* Новый вариант под корпус с надписями от RK1AQ */
const struct qmkey qmdefs [NQMKEYS] =
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
		{ KIF_NONE,		KBD_CODE_PLAYFILE2,		KBD_CODE_19, 			' ', },		// ANTEMMA
		{ KIF_NONE,		KBD_CODE_PLAYFILE3,		KBD_CODE_21, 			' ', },		// A=B

		/* кнопка дополнительного валкодера */
		{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
		/* кнопка выключения питания (включение аппаратно) */
		{ KIF_POWER, 	KBD_CODE_POWEROFF,	KBD_CODE_MAX, 		' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_FPANEL30_V3_BASE

/* 30 кнопок на 5 линий - плата rfrontpanel_v3 + LCDMODE_S1D13781 & LCDMODE_LQ043T3DX02K в нормальном расположении */
const struct qmkey qmdefs [NQMKEYS] =
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
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_21, 			' ', },		// ANTEMMA
	{ KIF_NONE,		KBD_CODE_A_EQ_B,		KBD_CODE_22, 			' ', },		// A=B
	{ KIF_NONE,		KBD_CODE_17,			KBD_CODE_18,			' ', },
	{ KIF_NONE,		KBD_CODE_19,			KBD_CODE_20,			' ', },

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_POWEROFF,	KBD_CODE_MAX, 		' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_FPANEL30_V3

/* 30 кнопок на 5 линий - плата rfrontpanel_v3 + LCDMODE_S1D13781 & LCDMODE_LQ043T3DX02K в нормальном расположении */
const struct qmkey qmdefs [NQMKEYS] =
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
	{ KIF_NONE,		KBD_CODE_BAND_7M0,		KBD_CODE_ANTENNA,		'4', },		// ANTEMMA
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
	{ KIF_POWER, 	KBD_CODE_POWEROFF,	KBD_CODE_MAX, 		' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_FPANEL20_V0A_RA1AGO

/* 20 кнопок на 5 линий - перевернутые */
const struct qmkey qmdefs [NQMKEYS] =
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
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_19, 			'4', },		// ANTEMMA
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
	{ KIF_POWER, 	KBD_CODE_POWEROFF,	KBD_CODE_MAX, 		' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_FPANEL20_V0A_AVB

/* 20 кнопок на 5 линий - перевернутые */
const struct qmkey qmdefs [NQMKEYS] =
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
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_19, 			'4', },		// ANTEMMA
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_FPANEL20_V0A_UA1CEI

/* 20 кнопок на 5 линий - плата rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении */
/* Новый вариант под корпус с надписями от RK1AQ */
const struct qmkey qmdefs [NQMKEYS] =
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
	{ KIF_NONE,		KBD_CODE_ANTENNA,		KBD_CODE_19, 			' ', },		// ANTEMMA
	{ KIF_NONE,		KBD_CODE_A_EQ_B,		KBD_CODE_21, 			' ', },		// A=B

	/* кнопка дополнительного валкодера */
	{ KIF_NONE,		KBD_ENC2_PRESS,		KBD_ENC2_HOLD, 		' ', },
	/* кнопка выключения питания (включение аппаратно) */
	{ KIF_POWER, 	KBD_CODE_POWEROFF,	KBD_CODE_MAX, 		' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_VERTICAL_REV

/* расположение кнопок для плат "Воробей" и "Колибри" */
const struct qmkey qmdefs [NQMKEYS] =
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
	{ KIF_POWER, 	KBD_CODE_POWEROFF,	KBD_CODE_MAX, 		' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_VERTICAL_REV_SIMPLEFREQ

/* расположение кнопок для плат "Воробей" и "Колибри" - только прямой ввод частоты */
const struct qmkey qmdefs [NQMKEYS] =
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
	{ KIF_POWER, 	KBD_CODE_POWEROFF,	KBD_CODE_MAX, 		' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_MAX;
}

#elif KEYB_VERTICAL_VALERY

/* расположение кнопок для плат "Воробей" и "Колибри" */
const struct qmkey qmdefs [NQMKEYS] =
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
	{ KIF_POWER, 	KBD_CODE_POWEROFF,	KBD_CODE_MAX, 		' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_VERTICAL_REV_IGOR

/* усечённый набор функций */
const struct qmkey qmdefs [NQMKEYS] =
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
	{ KIF_POWER, 	KBD_CODE_POWEROFF,	KBD_CODE_MAX, 		' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB18_RK4CI

/* 

0. Блокировка настройки / Вход в меню.
1. MODE / SUBMODE
2. АТТ / УВЧ
3. Полоса пропускания/ АРУ
4. Перестройка вверх, в режиме меню перелистывание строк
5. Перестройка вниз, и перелистываие строк в режиме меню
6. Включение передачи/настройка в режиме передачи.
7. Прямой ввод 1,9 / . 1 в режиме ввода
8. Прямой ввод 3,5 / 2 в режиме ввода
9. Прямой ввод 7,0 / Отключение динамика (наушники работают). 3 в режиме ввода
10. Прямой ввод 10,1 / 4 в режиме ввода
11. Прямой ввод 14,0 / 5 в режиме ввода.
12. Расстройка / Сплит. При включенном сплите, выход/ частоты меняются местами
13. Прямой ввод 18,0 / 6 в режиме ввода
14. ПРямой ввод 21,0 / 7 в режиме ввода
15. Прямой ввод 24,0 / 8 в режиме ввода
16. Прямой ввод 28, 0 / 9 в режиме ввода
17. Включение прямого ввода частоты/0 в режиме ввода/
 */

const struct qmkey qmdefs [NQMKEYS] =
{
	{ KIF_EXTMENU,	KBD_CODE_LOCK,		KBD_CODE_MENU, 			' ', },	// 0!
	{ KIF_NONE,		KBD_CODE_MODE,		KBD_CODE_MODEMOD, 		' ', },	// 1!
	{ KIF_NONE,		KBD_CODE_ATT,		KBD_CODE_PAMP,			' ', },	// 2!
	{ KIF_NONE,		KBD_CODE_BW,		KBD_CODE_AGC, 			' ', },	// 3!
	{ KIF_SLOW,		KBD_CODE_BAND_UP,	KBD_CODE_BAND_UP, 		' ', },	// 4!
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,	KBD_CODE_BAND_DOWN, 	' ', },	// 5!
	{ KIF_NONE,		KBD_CODE_MOX,		KBD_CODE_TXTUNE, 		' ', },	// 6!
	{ KIF_NONE,		KBD_CODE_BAND_1M8,	KBD_CODE_BAND_1M8, 		'1', },	// 7
	{ KIF_NONE,		KBD_CODE_BAND_3M5,	KBD_CODE_BAND_3M5, 		'2', },	// 8
	{ KIF_NONE,		KBD_CODE_BAND_7M0,	KBD_CODE_LDSPTGL,		'3', },	// 9
	{ KIF_NONE,		KBD_CODE_BAND_10M1,	KBD_CODE_BAND_10M1, 	'4', },	// 10
	{ KIF_NONE,		KBD_CODE_BAND_14M0, KBD_CODE_BAND_14M0,		'5', },	// 11
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED,		' ', },	// 12
	{ KIF_NONE,		KBD_CODE_BAND_18M0,	KBD_CODE_BAND_18M0, 	'6', },	// 13
	{ KIF_NONE,		KBD_CODE_BAND_21M0,	KBD_CODE_BAND_21M0, 	'7', },	// 14
	{ KIF_NONE,		KBD_CODE_BAND_24M9,	KBD_CODE_BAND_24M9, 	'8', },	// 15
	{ KIF_NONE,		KBD_CODE_BAND_28M0,	KBD_CODE_BAND_28M0, 	'9', },	// 16
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,	KBD_CODE_ENTERFREQDONE, '0', },	// 17
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_LOCK;
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

const struct qmkey qmdefs [NQMKEYS] =
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_VERTICAL_REV_RU6BK

/* Новый рисунок = '0' в левом ряду снизу. */
const struct qmkey qmdefs [NQMKEYS] =
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
	{ KIF_FAST,		ENC_CODE_STEP_DOWN,	ENC_CODE_STEP_DOWN, 	'#', },
	//ki0-левый ряд
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,	KBD_CODE_MENU, 			'1', },
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED, 	'4', },
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,	KBD_CODE_ENTERFREQDONE, '7', },
	{ KIF_NONE,		KBD_CODE_LOCK,		KBD_CODE_LOCK_HOLDED, 	'0', },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_VERTICAL_REV_RU6BK_TOPDOWN

/* расположение кнопок для ПЕРЕВЁРНУТЫХ плат "Воробей" и "Колибри" */
const struct qmkey qmdefs [NQMKEYS] =
{
	//ki2-левый ряд
	{ KIF_NONE,		KBD_CODE_LOCK,		KBD_CODE_LOCK_HOLDED, 	'0', },
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,	KBD_CODE_ENTERFREQDONE, '7', },
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED, 	'4', },
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,	KBD_CODE_MENU, 			'1', },
	//ki1-центральный ряд
	{ KIF_FAST,		ENC_CODE_STEP_DOWN,	ENC_CODE_STEP_DOWN, 	'#', },
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_VERTICAL_REV_TOPDOWN

/* расположение кнопок для ПЕРЕВЁРНУТЫХ плат "Воробей" и "Колибри" */
const struct qmkey qmdefs [NQMKEYS] =
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_M0SERG

// три группы по ШЕСТЬ линейки кнопок друг за другом в один ряд.
const struct qmkey qmdefs [NQMKEYS] =
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_RA1AGG

// три группы по четыре линейки кнопок друг за другом в один ряд.
const struct qmkey qmdefs [NQMKEYS] =
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_RA1AGO

/* расположение кнопок для RA1AGO */

const struct qmkey qmdefs [NQMKEYS] =
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_UA1CEI

const struct qmkey qmdefs [NQMKEYS] =
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
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


	const struct qmkey qmdefs [NQMKEYS] =
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

	uint_fast8_t getexitkey(void)
	{
		return KBD_CODE_DISPMODE;
	}

#elif KEYBOARD_XXXXX
/*

	Геннадий привет! Вот набросал как я хотел бы распределить
	назначение кнопок,если это конечно возможно:

	17--	увеличение  скорости ТЛГ-ключа
	16--	уменьшение ск-сти ТЛГ-ключа
	15--	VOX  ВКЛ/Выкл
	14--	MODE                                                                                      
	13--	SPLIT                                                                                                 
	12--	TX  передача
                                                                                          
	11--	Меню
	10--	Запись  ТЛГ ключа в память
	9---	Воспроиз-е ТЛГ ключа из памяти
	8---	Полоса 0,7  ,2,7
	7---	UP  диап-ны
	6---	DOWN  диапазоны
                                                                   
	5---	перестрйка  по диапазону в верх
	4---	блокировка
	3---	перест-ка в низ
	2---	TUNE                                                                                            
	1---	УВЧ   вкл/выкл
	0---	АТТ

*/

/* расположение кнопок для 18-ти клавишной клавиатуры */
const struct qmkey qmdefs [NQMKEYS] =
{
	//ki2-правый ряд
	{ KIF_NONE,	KBD_CODE_ATT,	KBD_CODE_MAX, },
	{ KIF_NONE,	KBD_CODE_PAMP,	KBD_CODE_MAX, },
	{ KIF_NONE,	KBD_CODE_TXTUNE,	KBD_CODE_MAX, },
	{ KIF_FAST,	ENC_CODE_STEP_DOWN,	ENC_CODE_STEP_DOWN, },
	{ KIF_FAST, ENC_CODE_STEP_UP, 	ENC_CODE_STEP_UP, },
	{ KIF_NONE, KBD_CODE_LOCK, 	KBD_CODE_MAX, },
	//ki1-центральный ряд
	{ KIF_SLOW,	KBD_CODE_BAND_DOWN,	KBD_CODE_BAND_DOWN, },
	{ KIF_SLOW,	KBD_CODE_BAND_UP,	KBD_CODE_BAND_UP, },
	{ KIF_NONE,	KBD_CODE_BW,	KBD_CODE_MAX, },
	{ KIF_NONE,	KBD_CODE_MAX,	KBD_CODE_MAX, },
	{ KIF_NONE, KBD_CODE_MAX, 	KBD_CODE_MAX, },
	{ KIF_EXTMENU, KBD_CODE_DISPMODE, KBD_CODE_MENU,  },
	//ki0-левый ряд
	{ KIF_NONE,	KBD_CODE_MOX,	KBD_CODE_MAX, },
	{ KIF_ERASE,KBD_CODE_SPLIT,KBD_CODE_SPLIT_HOLDED, },
	{ KIF_NONE, KBD_CODE_MODE, KBD_CODE_MODEMOD,  },
	{ KIF_NONE,	KBD_CODE_VOXTOGGLE,	KBD_CODE_MAX, },
	{ KIF_SLOW, KBD_CODE_CWSPEEDDOWN, 	KBD_CODE_CWSPEEDDOWN, },
	{ KIF_SLOW, KBD_CODE_CWSPEEDUP, 	KBD_CODE_CWSPEEDUP, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif REQUEST_BA

const struct qmkey qmdefs [NQMKEYS] =
{
	// ki2
	{ KIF_EXTMENU, KBD_CODE_DISPMODE, KBD_CODE_MENU, '1', },
	{ KIF_NONE, KBD_CODE_BW, KBD_CODE_AGC, '4', },
	{ KIF_NONE, KBD_CODE_MODE, KBD_CODE_MODEMOD, '7', },
	{ KIF_FAST, ENC_CODE_STEP_DOWN, ENC_CODE_STEP_DOWN, '*' },
	// ki1
	{ KIF_ERASE, KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED, '2', },
	{ KIF_NONE, KBD_CODE_ATT, KBD_CODE_PAMP,  '5', },
	{ KIF_NONE, KBD_CODE_ENTERFREQ, KBD_CODE_ENTERFREQDONE, '8', },
	{ KIF_SLOW, KBD_CODE_BAND_DOWN, KBD_CODE_BAND_DOWN, '0', },
	// ki0
	{ KIF_NONE, KBD_CODE_LOCK, KBD_CODE_LOCK_HOLDED, '3', },
	{ KIF_NONE, KBD_CODE_MOX, KBD_CODE_TXTUNE, '6', },
	{ KIF_SLOW, KBD_CODE_BAND_UP, KBD_CODE_BAND_UP, '9', },
	{ KIF_FAST, ENC_CODE_STEP_UP, ENC_CODE_STEP_UP, '#', },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYBSTYLE_RA4YBO_AM0

const struct qmkey qmdefs [NQMKEYS] =
{
	{ KIF_NONE,		KBD_CODE_ATT, KBD_CODE_MAX, },
	{ KIF_NONE,		KBD_CODE_BAND_UP, KBD_CODE_MAX, },
	{ KIF_NONE,		KBD_CODE_BW, KBD_CODE_MENU, },		// полоса
	{ KIF_NONE,		KBD_CODE_USER2, KBD_CODE_MAX, },		// ревербератор
	{ KIF_NONE,		KBD_CODE_USER1, KBD_CODE_MAX, },		// эквалайзер
	{ KIF_NONE,		KBD_CODE_MAX, KBD_CODE_MAX, },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_BW;
}


#elif KEYB_RA4YBO

/*
	3-  0-  F_AP
	6-  1-  F_DW
	9-  2-  ?  \ IF_FIL_0,3_0,6_1,0_2,4_3,1_7,8_17 кГц
	#-  3-    F___(прямой ввод частоты) \ ?
	2-  4-  PRE_10db_20db_30db \  ?
	5-  5-  LOCK \ AGC_T0,T1,T2,T3
	8-  6-  TUNE
	0-  7-  MENU  \  AF_FIL_ON
	1-  8-  BAND_AP
	4-  9-  BAND_DW
	7-  10-  MODE (LSB,USB,CW,AM,FM,WFM) \ ?
	*-  11-  SPLIT

*/
const struct qmkey qmdefs [NQMKEYS] =
{
	{ KIF_FAST, ENC_CODE_STEP_UP, ENC_CODE_STEP_UP, '3' },
	{ KIF_FAST, ENC_CODE_STEP_DOWN, ENC_CODE_STEP_DOWN, '6' },
	{ KIF_NONE, KBD_CODE_BW, KBD_CODE_MAX, '9' },
	{ KIF_NONE, KBD_CODE_ENTERFREQ, KBD_CODE_ENTERFREQDONE, '#'},

	{ KIF_NONE, KBD_CODE_ATT, KBD_CODE_PAMP, '2' },
	{ KIF_NONE, KBD_CODE_LOCK, KBD_CODE_AGC, '5' },
	{ KIF_NONE, KBD_CODE_MOX, KBD_CODE_TXTUNE, '8' },
	{ KIF_EXTMENU, KBD_CODE_AFNARROW, KBD_CODE_MENU, '0' },

	{ KIF_SLOW, KBD_CODE_BAND_UP, KBD_CODE_BAND_UP, '1' },
	{ KIF_SLOW, KBD_CODE_BAND_DOWN, KBD_CODE_BAND_DOWN, '4' },
	{ KIF_NONE, KBD_CODE_MODE, KBD_CODE_MODEMOD, '7' },
	{ KIF_ERASE, KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED, '*' },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_MAX;
}

#elif KEYB_RA4YBO_V1

const struct qmkey qmdefs [NQMKEYS] =
{
	{ KIF_FAST, ENC_CODE_STEP_UP, ENC_CODE_STEP_UP, '3' },
	{ KIF_FAST, ENC_CODE_STEP_DOWN, ENC_CODE_STEP_DOWN, '6' },
	{ KIF_NONE, KBD_CODE_BW, KBD_CODE_AGC, '9' },
	{ KIF_NONE, KBD_CODE_ENTERFREQ, KBD_CODE_ENTERFREQDONE, '#'},

	{ KIF_NONE, KBD_CODE_ATT, KBD_CODE_PAMP, '2' },
	{ KIF_NONE, KBD_CODE_LOCK, KBD_CODE_MAX, '5' },
	{ KIF_NONE, KBD_CODE_MOX, KBD_CODE_TXTUNE, '8' },
	{ KIF_EXTMENU, KBD_CODE_MAX, KBD_CODE_MENU, '0' },

	{ KIF_SLOW, KBD_CODE_BAND_UP, KBD_CODE_BAND_UP, '1' },
	{ KIF_SLOW, KBD_CODE_BAND_DOWN, KBD_CODE_BAND_DOWN, '4' },
	{ KIF_NONE, KBD_CODE_MODE, KBD_CODE_MODEMOD, '7' },
	{ KIF_ERASE, KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED, '*' },

	{ KIF_NONE,	KBD_CODE_USER1,	KBD_CODE_MAX, ' ' },
	{ KIF_NONE,	KBD_CODE_USER2,	KBD_CODE_MAX, ' ' },
	{ KIF_NONE,	KBD_CODE_ATUBYPASS,	KBD_CODE_ATUSTART, },		// BYP/ATU
	{ KIF_NONE,	KBD_CODE_IFSHIFT,	KBD_CODE_MAX, ' ' },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_MAX;
}

#elif KEYB_RA4YBO_V2

const struct qmkey qmdefs [NQMKEYS] =
{
	{ KIF_FAST, ENC_CODE_STEP_UP, ENC_CODE_STEP_UP, '3' },
	{ KIF_FAST, ENC_CODE_STEP_DOWN, ENC_CODE_STEP_DOWN, '6' },
	{ KIF_NONE, KBD_CODE_BW, KBD_CODE_AGC, '9' },
	{ KIF_NONE, KBD_CODE_ENTERFREQ, KBD_CODE_ENTERFREQDONE, '#'},

	{ KIF_NONE, KBD_CODE_ATT, KBD_CODE_PAMP, '2' },
	{ KIF_NONE, KBD_CODE_LOCK, KBD_CODE_MAX, '5' },
	{ KIF_NONE, KBD_CODE_MOX, KBD_CODE_TXTUNE, '8' },
	{ KIF_EXTMENU, KBD_CODE_MAX, KBD_CODE_MENU, '0' },

	{ KIF_SLOW, KBD_CODE_BAND_UP, KBD_CODE_BAND_UP, '1' },
	{ KIF_SLOW, KBD_CODE_BAND_DOWN, KBD_CODE_BAND_DOWN, '4' },
	{ KIF_NONE, KBD_CODE_MODE, KBD_CODE_MODEMOD, '7' },
	{ KIF_ERASE, KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED, '*' },

	{ KIF_NONE,	KBD_CODE_USER1,	KBD_CODE_MAX, ' ' },
	{ KIF_NONE,	KBD_CODE_USER2,	KBD_CODE_MAX, ' ' },
	{ KIF_NONE,	KBD_CODE_USER3,	KBD_CODE_MAX, ' ' },
	{ KIF_NONE,	KBD_CODE_USER4,	KBD_CODE_MAX, ' ' },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_MAX;
}

#elif KEYB_RA4YBO_V3

const struct qmkey qmdefs [NQMKEYS] =
{
	{ KIF_NONE,	KBD_CODE_ATUBYPASS,	KBD_CODE_ATUSTART, },		// BYP/ATU
	{ KIF_NONE,	KBD_CODE_ANTENNA,	KBD_CODE_MAX, ' ' },		// yagi
	{ KIF_USER2,	KBD_CODE_MAX,	KBD_CODE_MAX, ' ' },			// REC
	{ KIF_USER3,	KBD_CODE_MAX,	KBD_CODE_MAX, ' ' },			// play
	{ KIF_NONE,	KBD_CODE_USER4,	KBD_CODE_MAX, ' ' },			// equal
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX, ' '  },				// клавиша отсутствует

	{ KIF_FAST, ENC_CODE_STEP_UP, ENC_CODE_STEP_UP, '3' },
	{ KIF_FAST, ENC_CODE_STEP_DOWN, ENC_CODE_STEP_DOWN, '6' },
	{ KIF_NONE, KBD_CODE_BW, KBD_CODE_AGC, '9' },
	{ KIF_NONE, KBD_CODE_ENTERFREQ, KBD_CODE_ENTERFREQDONE, '#'},
	{ KIF_NONE, KBD_CODE_LDSPTGL, KBD_CODE_MAX, ' '  },		// speaker
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX, ' '  },		// клавиша отсутствует

	{ KIF_NONE, KBD_CODE_ATT, KBD_CODE_PAMP, '2' },
	{ KIF_NONE, KBD_CODE_AFNARROW, KBD_CODE_LOCK, '5' },
	{ KIF_NONE, KBD_CODE_MOX, KBD_CODE_TXTUNE, '8' },
	{ KIF_EXTMENU, KBD_CODE_DISPMODE, KBD_CODE_MENU, '0' },
	{ KIF_NONE, KBD_CODE_IFSHIFT, KBD_CODE_MENU_CWSPEED, },		// IF SH / WPM
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX, ' '  },		// клавиша отсутствует

	{ KIF_SLOW, KBD_CODE_BAND_UP, KBD_CODE_BAND_UP, '1' },
	{ KIF_SLOW, KBD_CODE_BAND_DOWN, KBD_CODE_BAND_DOWN, '4' },
	{ KIF_NONE, KBD_CODE_MODE, KBD_CODE_MODEMOD, '7' },
	{ KIF_ERASE, KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED, '*' },
	{ KIF_NONE,	KBD_CODE_USER5,	KBD_CODE_MAX, ' ' },	// reverb
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX, ' '  },		// клавиша отсутствует
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_UA3DKC
// Расположение для контроллера UA3DKC (по 6 кнопок в ряд)
// KI2 - верхний ряд
// KI1 - средний ряд
// KI0 - нижний ряд
// К процессору идут левые кнопки каждого ряда.
const struct qmkey qmdefs [NQMKEYS] =
{
	// ki2 - верхний ряд
	{ KIF_EXTMENU, KBD_CODE_DISPMODE, KBD_CODE_MENU, ' '  },
	{ KIF_NONE, KBD_CODE_MODE, KBD_CODE_MODEMOD, ' '  },
	{ KIF_NONE, KBD_CODE_BW, KBD_CODE_MAX, ' '  },
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX, ' '  },
	{ KIF_SLOW, KBD_CODE_BAND_UP, KBD_CODE_BAND_UP, ' '  },
	{ KIF_SLOW, KBD_CODE_BAND_DOWN, KBD_CODE_BAND_DOWN, ' '  },
	// ki1 - средний ряд
	{ KIF_ERASE, KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED, ' '  },
	{ KIF_NONE, KBD_CODE_PAMP, KBD_CODE_MAX, ' '  },
	{ KIF_NONE, KBD_CODE_ATT, KBD_CODE_MAX, ' '  },
	{ KIF_NONE, KBD_CODE_AGC, KBD_CODE_MAX, ' '  },
	{ KIF_SLOW4, KBD_CODE_IND_DOWN, KBD_CODE_IND_DOWN, ' '  },		// L--
	{ KIF_SLOW4, KBD_CODE_IND_UP, KBD_CODE_IND_UP, ' '  },		// L++
	// ki0 - нижний ряд
	{ KIF_NONE, KBD_CODE_LOCK, KBD_CODE_LOCK_HOLDED, ' '  },
	{ KIF_NONE,	KBD_CODE_MOX,		KBD_CODE_TXTUNE, 		' ', },
	{ KIF_NONE,	KBD_CODE_ATUBYPASS,	KBD_CODE_ATUSTART,		' ', },		// BYP/ATU: was PWR
	{ KIF_NONE, KBD_CODE_TUNERTYPE, KBD_CODE_TUNERTYPE, ' '  },		// TUNER TYPE
	{ KIF_SLOW4, KBD_CODE_CAP_UP, KBD_CODE_CAP_UP, ' '  },		// С++
	{ KIF_SLOW4, KBD_CODE_CAP_DOWN, KBD_CODE_CAP_DOWN, ' '  },		// С--
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_V8S_DK1VS
// Расположение для контроллера DK1VS
// KI2 - верхний ряд
// KI1 - средний ряд
// KI0 - нижний ряд
// К процессору идут левые кнопки каждого ряда.
const struct qmkey qmdefs [NQMKEYS] =
{
	// ki2
	{ KIF_EXTMENU, KBD_CODE_DISPMODE, KBD_CODE_MENU, '1'  },
	{ KIF_NONE, KBD_CODE_BW, KBD_CODE_AGC, '2'  },
	{ KIF_NONE, KBD_CODE_MODE, KBD_CODE_MODEMOD, '3'  },
	{ KIF_SLOW, KBD_CODE_BAND_UP, KBD_CODE_BAND_UP, '4'  },
	// ki1
	{ KIF_ERASE, KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED, '5'  },
	{ KIF_NONE, KBD_CODE_ATT, KBD_CODE_PAMP, '6'  },
	{ KIF_NONE, KBD_CODE_ENTERFREQ, KBD_CODE_ENTERFREQDONE, '7', },
	{ KIF_SLOW, KBD_CODE_BAND_DOWN, KBD_CODE_BAND_DOWN, '8'  },
	// ki0
	{ KIF_NONE, KBD_CODE_LOCK, KBD_CODE_LOCK_HOLDED, '9'  },
	{ KIF_NONE, KBD_CODE_MOX, KBD_CODE_TXTUNE, '0'  },
	{ KIF_FAST, ENC_CODE_STEP_DOWN, ENC_CODE_STEP_DOWN, ' '  },
	{ KIF_FAST, ENC_CODE_STEP_UP, ENC_CODE_STEP_UP, ' '  },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_V8S_HORISONTAL_UA3DKC
// Расположение для трансвиера UA3DKC
// KI2 - верхний ряд
// KI1 - средний ряд
// KI0 - нижний ряд
// К процессору идут левые кнопки каждого ряда.
const struct qmkey qmdefs [NQMKEYS] =
{
	// ki2
	{ KIF_EXTMENU, KBD_CODE_DISPMODE, KBD_CODE_MENU, '1'  },
	{ KIF_NONE, KBD_CODE_MODE, KBD_CODE_MODEMOD, '2'  }, // 
	{ KIF_SLOW, KBD_CODE_BAND_UP, KBD_CODE_BAND_UP, '3'  },
	{ KIF_SLOW, KBD_CODE_BAND_DOWN, KBD_CODE_BAND_DOWN, '4'  },
	// ki1
	{ KIF_ERASE, KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED, '5'  },
	{ KIF_NONE, KBD_CODE_ATT, KBD_CODE_1, '6'  },
	{ KIF_NONE, KBD_CODE_PAMP, KBD_CODE_2, '7', },
	{ KIF_NONE, KBD_CODE_BW, KBD_CODE_AGC, '8'  },
	// ki0
	{ KIF_NONE, KBD_CODE_LOCK, KBD_CODE_LOCK_HOLDED, '9'  },
	{ KIF_NONE, KBD_CODE_MOX, KBD_CODE_TXTUNE, '0'  },					// MOX
	{ KIF_NONE,	KBD_CODE_ATUBYPASS,	KBD_CODE_ATUSTART, ' ' },		// BYP/ATU: was PWR
	{ KIF_NONE, KBD_CODE_LDSPTGL, KBD_CODE_4, ' '  },				// SPEAKER
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED

// базовое расположение клавиш (в соответствии с тем, что в .xls документе описано).
const struct qmkey qmdefs [NQMKEYS] =
{
	// ki2
	{ KIF_NONE, KBD_CODE_0, KBD_CODE_0,  },
	{ KIF_NONE, KBD_CODE_1, KBD_CODE_1,  },
	{ KIF_NONE, KBD_CODE_2, KBD_CODE_2,  },
	{ KIF_NONE, KBD_CODE_3, KBD_CODE_3,  },
	// ki1
	{ KIF_NONE, KBD_CODE_4, KBD_CODE_4,  },
	{ KIF_NONE, KBD_CODE_5, KBD_CODE_5,  },
	{ KIF_NONE, KBD_CODE_6, KBD_CODE_6,  },
	{ KIF_NONE, KBD_CODE_7, KBD_CODE_7,  },
	// ki0
	{ KIF_NONE, KBD_CODE_8, KBD_CODE_8,  },
	{ KIF_NONE, KBD_CODE_9, KBD_CODE_9,  },
	{ KIF_NONE, KBD_CODE_10, KBD_CODE_10,  },
	{ KIF_NONE, KBD_CODE_11, KBD_CODE_11,  },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_MAX;
}

#elif KEYB_LADVABEST

	/* 3 * 6 кнопок ladvabest@gmail.com */
const struct qmkey qmdefs [NQMKEYS] =
{
	// ki2
	{ KIF_NONE,		KBD_CODE_ENTERFREQ,		KBD_CODE_ENTERFREQDONE, '7', },
	{ KIF_NONE,		KBD_CODE_BW,			KBD_CODE_MAX, 			'8', },
	{ KIF_SLOW,		KBD_CODE_DIGI,				KBD_CODE_2,				'9', }, // digi
	{ KIF_NONE,		KBD_CODE_LOCK,			KBD_CODE_LOCK_HOLDED, 	'0', },
	{ KIF_NONE,		KBD_CODE_MOX,			KBD_CODE_TXTUNE, 		' ', },
	{ KIF_SLOW,		KBD_CODE_5,				KBD_CODE_5,				' ', },
	// ki1
	{ KIF_NONE,		KBD_CODE_ATT,			KBD_CODE_PAMP, 			'4', },
	{ KIF_SLOW,		KBD_CODE_AGC,			KBD_CODE_NOTCHTOGGLE,	'5', },	// agc / notch
	{ KIF_SLOW,		KBD_CODE_COMPTOGGLE,	KBD_CODE_ALTTOGGLE,		'6', },	// comp / alc
	{ KIF_FAST,		ENC_CODE_STEP_DOWN,		ENC_CODE_STEP_DOWN, 	' ', },
	{ KIF_FAST,		ENC_CODE_STEP_UP,		ENC_CODE_STEP_UP, 		' ', },
	{ KIF_SLOW,		KBD_CODE_11,			KBD_CODE_11,			' ', },
	// ki0
	{ KIF_EXTMENU,	KBD_CODE_DISPMODE,		KBD_CODE_MENU, 			'1', },
	{ KIF_ERASE,	KBD_CODE_SPLIT,			KBD_CODE_SPLIT_HOLDED, 	'2', },
	{ KIF_NONE,		KBD_CODE_MODE,			KBD_CODE_MODEMOD, 		'3', },
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,		KBD_CODE_BAND_DOWN, 	' ', },
	{ KIF_SLOW,		KBD_CODE_BAND_UP,		KBD_CODE_BAND_UP, 		' ', },
	{ KIF_SLOW,		KBD_CODE_17,			KBD_CODE_17,			' ', },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_DISPMODE;
}

#elif KEYB_RA3APW

const struct qmkey qmdefs [NQMKEYS] =
{
	// ki1
	{ KIF_SLOW, KBD_CODE_4, KBD_CODE_4,  },
	{ KIF_SLOW, KBD_CODE_5, KBD_CODE_5,  },
	{ KIF_SLOW, KBD_CODE_6, KBD_CODE_6,  },
	{ KIF_SLOW, KBD_CODE_7, KBD_CODE_7,  },
	#if 0
	// ki0
	{ KIF_SLOW, KBD_CODE_8, KBD_CODE_8,  },
	{ KIF_SLOW, KBD_CODE_9, KBD_CODE_9,  },
	{ KIF_SLOW, KBD_CODE_10, KBD_CODE_10,  },
	{ KIF_SLOW, KBD_CODE_11, KBD_CODE_11,  },
	#endif
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_MAX;
}

#elif KEYB_VERTICAL_UR3VBM

/* Два ряда по три кнопки, вертикально */
// ADC7 pre(0k) mod(2.2k) fun(4.7k)
// ADC6 up(0k) down (2.2k) spl(4.7к) так же как в sw2016

const struct qmkey qmdefs [NQMKEYS] =
{
	//ki1-правый ряд
	{ KIF_SLOW,		KBD_CODE_BAND_UP,	KBD_CODE_BAND_UP, 		' ', },
	{ KIF_SLOW,		KBD_CODE_BAND_DOWN,	KBD_CODE_BAND_DOWN, 	' ', },
	{ KIF_ERASE,	KBD_CODE_SPLIT,		KBD_CODE_SPLIT_HOLDED, 	' ', },
	{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX, ' '  },		// клавиша отсутствует

	//ki0-левый ряд
	{ KIF_NONE,		KBD_CODE_ATT,		KBD_CODE_PAMP, 			' ', },
	{ KIF_NONE,		KBD_CODE_MODE,		KBD_CODE_MODEMOD, 		' ', },
	{ KIF_EXTMENU,	KBD_CODE_TXTUNE,	KBD_CODE_MENU, 			' ', },
	{ KIF_NONE,		KBD_CODE_MAX,		KBD_CODE_MAX, ' '  },		// клавиша отсутствует
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_TXTUNE;
}

#elif KEYB_RV9CQQ

const struct qmkey qmdefs [NQMKEYS] =
{
	// ki1
	{ KIF_EXTMENU, KBD_CODE_DISPMODE, KBD_CODE_MENU,  },
	{ KIF_NONE, KBD_CODE_MODE, KBD_CODE_MODEMOD,  },
	{ KIF_SLOW, KBD_CODE_BAND_DOWN, KBD_CODE_BAND_DOWN,  },
	{ KIF_SLOW, KBD_CODE_BAND_UP, KBD_CODE_BAND_UP,  },
	// ki0
	{ KIF_ERASE, KBD_CODE_SPLIT, KBD_CODE_SPLIT_HOLDED,  },
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
	{ KIF_NONE, KBD_CODE_MAX, KBD_CODE_MAX,  },
	// encoder2 & power buttons
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
	{	KIF_NONE, 		KBD_CODE_MAX,		KBD_CODE_MAX, ' ', },
};

#else

#warning KEYB_XXXX not defined

// базовое расположение клавиш (в соответствии с тем, что в .xls документе описано).
const struct qmkey qmdefs [NQMKEYS] =
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

uint_fast8_t getexitkey(void)
{
	return KBD_CODE_MAX;
}

#endif

/* получить код клавиши цифровой клавиатуры по коду, полученному как код нажатия */
uint_fast8_t kbd_getnumpad(uint_fast8_t key)
{
	uint_fast8_t i;

	for (i = 0; i < NQMKEYS; ++ i)
	{
		if (qmdefs [i].code == key)
			return qmdefs [i].numpad;
	}
	return 0;
}

#endif /* WITHKEYBOARD */

