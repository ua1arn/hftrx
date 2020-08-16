/* $Id$ */
/* Pin manipulation functioms */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef DISPLAY2_H_INCLUDED
#define DISPLAY2_H_INCLUDED

#include "src/display/display.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef struct editfreq_tag
{
	uint32_t val;
	uint8_t num;
	uint8_t key;
} editfreq_t;

typedef struct editfreq2_tag
{
	uint32_t freq;
	uint8_t blinkpos;		// позиция (степень 10) редактируесого символа
	uint8_t blinkstate;	// в месте редактируемого символа отображается подчёркивание (0 - пробел)
} editfreq2_t;

typedef struct dctx_tag
{
	enum { DCTX_FREQ, DCTX_MENU } type;
	const void * pv;
} dctx_t;

void display2_smeter15(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);

void
display2_smeter15_init(
	uint_fast8_t xgrid,
	uint_fast8_t ygrid,
	dctx_t * pctx
	);

// FUNC item label
void display2_fnlabel9(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);
// FUNC item value
void display2_fnvalue9(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);


// Вызывается из display2.c
//Отображение многострочного меню для больших экранов (группы)
void display2_multilinemenu_block_groups(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);
//Отображение многострочного меню для больших экранов (параметры)
void display2_multilinemenu_block_params(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);
//Отображение многострочного меню для больших экранов (значения)
void display2_multilinemenu_block_vals(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);
// Вызывается из display2.c
// группа, в которой находится редактируемый параметр
void display2_menu_group(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);

// Вызывается из display2.c
// значение параметра
void display2_menu_valxx(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);

// Вызывается из display2.c
// название редактируемого параметра или группы
void display2_menu_lblst(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);

// Вызывается из display2.c
// название редактируемого параметра
// если группа - ничего не отображаем
void display2_menu_lblng(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);

// Вызывается из display2.c
// код редактируемого параметра
void display2_menu_lblc3(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);

uint_fast8_t display_getpagesmax(void);	// количество разных вариантов отображения (menuset)
uint_fast8_t display_getpagesleep(void);	// номер варианта отображения для "сна"
uint_fast8_t display_getfreqformat(uint_fast8_t * prjv);	// получить параметры отображения частоты (для функции прямого ввода)

void display2_bgprocess(void);	// выполнение шагов state machine отображения дисплея
void display2_bgreset(void);	// сброс state machine отображения дисплея

void display2_dispfreq_a2(
	uint_fast32_t freq,
	uint_fast8_t blinkpos,		// позиция (степень 10) редактируесого символа
	uint_fast8_t blinkstate,	// в месте редактируемого символа отображается подчёркивание (0 - пробел)
	uint_fast8_t menuset	/* индекс режима отображения (0..3) */
	);

void display2_dispfreq_ab(
	uint_fast8_t menuset	/* индекс режима отображения (0..3) */
	);
void display2_volts(
	uint_fast8_t menuset,	/* индекс режима отображения (0..3) */
	uint_fast8_t extra		/* находимся в режиме отображения настроек */
	);

// Статическая часть отображения режима работы
void display2_mode_subset(
	uint_fast8_t menuset	/* индекс режима отображения (0..3) */
	);

// S-meter, SWR-meter, voltmeter
void display2_barmeters_subset(
	uint_fast8_t menuset,	/* индекс режима отображения (0..3) */
	uint_fast8_t extra		/* находимся в режиме отображения настроек */
	);

struct menudef;


// Обновление изоражения экрана при нахождении в режиме меню
void display2_menu(
	const FLASHMEM struct menudef * mp,
	uint_fast8_t byname			/* был выполнен прямой вход в меню */
	);

// Вызывается из display2.c
void
display2_bars(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);
// Вызывается из display2.c
void
display2_bars_rx(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);
// Вызывается из display2.c
void
display2_bars_tx(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);
// Вызывается из display2.c
void
display2_adctest(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);

// Вызывается из display2.c (версия для CTLSTYLE_RA4YBO_AM0)
void
display2_bars_amv0(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);

// Параметры отображения многострочного меню для больших экранов
typedef struct multimenuwnd_tag
{
	uint8_t multilinemenu_max_rows;
	uint8_t menurow_count;
	uint8_t ystep;
	uint8_t reverse;	// 0/1
	uint8_t valuew;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
} multimenuwnd_t;

#define LABELW 8

void display2_getmultimenu(multimenuwnd_t * p); /* получение параметров окна для меню */

void display_smeter(
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t value,
	uint_fast8_t tracemax,
	uint_fast8_t level9,	// s9 level
	uint_fast8_t delta1,	// s9 - s0 delta
	uint_fast8_t delta2		// s9+50 - s9 delta
	);

void display_smeter_amv0(
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t value,
	uint_fast8_t tracemax,
	uint_fast8_t level9,	// s9 level
	uint_fast8_t delta1,	// s9 - s0 delta
	uint_fast8_t delta2		// s9+50 - s9 delta
	);

// Вызывается из display2_bars_amv0 (версия для CTLSTYLE_RA4YBO_AM0)
void display_modulationmeter_amv0(
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t value,
	uint_fast8_t maxvalue
	);

void display_pwrmeter(
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t pwr,
	uint_fast8_t tracemax,
	uint_fast8_t maxpwrcali		// значение для отклонения на всю шкалу
	);

void display_pwrmeter_amvo(
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t pwr,
	uint_fast8_t tracemax,
	uint_fast8_t maxpwrcali		// значение для отклонения на всю шкалу
	);

uint_fast8_t display2_getswrmax(void);

uint_fast16_t normalize(
	uint_fast16_t raw,
	uint_fast16_t rawmin,
	uint_fast16_t rawmax,
	uint_fast16_t range
	);

uint_fast16_t get_swr(uint_fast16_t swr_fullscale);

#define BGCOLOR (display_getbgcolor())

void display_2states_P(
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t state,
	const FLASHMEM char * state1,	// активное
	const FLASHMEM char * state0
	);

void display_2states(
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t state,
	const char * state1,	// активное
	const char * state0
	);
// параметры, не меняющие состояния цветом
void display_1state_P(
	uint_fast8_t x,
	uint_fast8_t y,
	const FLASHMEM char * label
	);
// параметры, не меняющие состояния цветом
void display_1state(
	uint_fast8_t x,
	uint_fast8_t y,
	const char * label
	);

// FUNC menu

void display_2fmenus_P(
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t state,
	const FLASHMEM char * state1,	// активное
	const FLASHMEM char * state0
	);

void display_2fmenus(
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t state,
	const char * state1,	// активное
	const char * state0
	);

// параметры, не меняющие состояния цветом
void display_1fmenu_P(
	uint_fast8_t x,
	uint_fast8_t y,
	const FLASHMEM char * label
	);
// параметры, не меняющие состояния цветом
void display_1fmenu(
	uint_fast8_t x,
	uint_fast8_t y,
	const char * label
	);


#define SWRMIN 10	// минимум - соответствует SWR = 1.0, точность = 0.1

#ifdef __cplusplus
}
#endif /* __cplusplus */


#if defined (COLORPIP_SHADED)

	// цвета
	// 0..COLORPIP_BASE-1 - волопад
	// COLORPIP_BASE..127 - надписи и элементы дизайна
	// то же с кодом больше на 128 - затененные цвета для получения полупрозрачности
	// 0..95 - палитра водопада
	// 96..111 - норм цвета
	// 112..127 - первая степень AA
	// Заполнение палитры производится в display2_xltrgb24()

	#define COLORPIP_YELLOW      (COLORPIP_BASE + 0) // TFTRGB(0xFF, 0xFF, 0x00)
	#define COLORPIP_ORANGE      (COLORPIP_BASE + 1) // TFTRGB(0xFF, 0xA5, 0x00)
	#define COLORPIP_BLACK       (COLORPIP_BASE + 2) // TFTRGB(0x00, 0x00, 0x00)
	#define COLORPIP_WHITE       (COLORPIP_BASE + 3) // TFTRGB(0xFF, 0xFF, 0xFF)
	#define COLORPIP_GRAY        (COLORPIP_BASE + 4) // TFTRGB(0x80, 0x80, 0x80)
	#define COLORPIP_DARKGRAY    (COLORPIP_BASE + 5) // TFTRGB(0x70, 0x70, 0x70) FIXME: use right value
	#define COLORPIP_DARKGREEN   (COLORPIP_BASE + 5) // TFTRGB(0x00, 0x64, 0x00)
	#define COLORPIP_BLUE        (COLORPIP_BASE + 6) // TFTRGB(0x00, 0x00, 0xFF)
	#define COLORPIP_GREEN       (COLORPIP_BASE + 7) // TFTRGB(0x00, 0xFF, 0x00)
	#define COLORPIP_RED         (COLORPIP_BASE + 8) // TFTRGB(0xFF, 0x00, 0x00)

	#define COLORPIP_LOCKED  	 	(COLORPIP_BASE + 9) // TFTRGB(0x3C, 0x3C, 0x00)
	#define COLORPIP_GRIDCOLOR		(COLORPIP_BASE + 10) // TFTRGB565(128, 0, 0)		//COLOR_GRAY - center marker
	#define COLORPIP_GRIDCOLOR2		(COLORPIP_BASE + 11) // TFTRGB565(96, 96, 96)		//COLOR_DARKRED - other markers
	#define COLORPIP_SPECTRUMBG		(COLORPIP_BASE + 12) // TFTRGB565(0, 64, 24)			//
	#define COLORMAIN_SPECTRUMBG2	(COLORPIP_BASE + 13) // TFTRGB565(0, 24, 8)		//COLOR_xxx - полоса пропускания приемника
	#define COLORPIP_SPECTRUMFG		(COLORPIP_BASE + 14) // TFTRGB565(0, 255, 0)		//COLOR_GREEN

	#if COLORSTYLE_ATS52
		// new (for ats52).
		#define COLORPIP_SPECTRUMLINE	COLORPIP_YELLOW
		#define DESIGNCOLOR_SPECTRUMFENCE	COLORPIP_WHITE

	#else
		// old
		//#define COLORPIP_SPECTRUMLINE	COLORPIP_GREEN
		#define COLORPIP_SPECTRUMLINE	COLORPIP_YELLOW
		#define DESIGNCOLOR_SPECTRUMFENCE	COLORPIP_WHITE

	#endif

	#if LCDMODE_MAIN_L8
		// Цвета, используемые на основном экране
		#define COLORMAIN_BLACK COLORPIP_BLACK
		#define COLORMAIN_WHITE COLORPIP_WHITE
		#define COLORMAIN_BLUE COLORPIP_BLUE
		#define COLORMAIN_GREEN COLORPIP_GREEN
		#define COLORMAIN_RED COLORPIP_RED
		#define COLORMAIN_GRAY COLORPIP_GRAY
		#define COLORMAIN_DARKGREEN COLORPIP_DARKGREEN
		#define COLORMAIN_YELLOW COLORPIP_YELLOW
		#define COLORMAIN_DARKRED  COLORPIP_GRIDCOLOR2	// COLORPIP_DARKRED
	#endif /* LCDMODE_MAIN_L8 */

#else /* LCDMODE_PIP_L8 */

	// определение основных цветов
	///

	/* RGB 24-bits color table definition (RGB888). */
	#define COLOR_BLACK          TFTRGB(0x00, 0x00, 0x00)
	#define COLOR_WHITE          TFTRGB(0xFF, 0xFF, 0xFF)
	#define COLOR_BLUE           TFTRGB(0x00, 0x00, 0xFF)
	#define COLOR_GREEN          TFTRGB(0x00, 0xFF, 0x00)
	#define COLOR_RED            TFTRGB(0xFF, 0x00, 0x00)
	#define COLOR_NAVY           TFTRGB(0x00, 0x00, 0x80)
	#define COLOR_DARKBLUE       TFTRGB(0x00, 0x00, 0x8B)
	#define COLOR_DARKGREEN      TFTRGB(0x00, 0x64, 0x00)
	#define COLOR_DARKGREEN2     TFTRGB(0x00, 0x20, 0x00)
	#define COLOR_DARKCYAN       TFTRGB(0x00, 0x8B, 0x8B)
	#define COLOR_CYAN           TFTRGB(0x00, 0xFF, 0xFF)
	#define COLOR_TURQUOISE      TFTRGB(0x40, 0xE0, 0xD0)
	#define COLOR_INDIGO         TFTRGB(0x4B, 0x00, 0x82)
	#define COLOR_DARKRED        TFTRGB(0x80, 0x00, 0x00)
	#define COLOR_DARKRED2       TFTRGB(0x40, 0x00, 0x00)
	#define COLOR_OLIVE          TFTRGB(0x80, 0x80, 0x00)
	#define COLOR_GRAY           TFTRGB(0x80, 0x80, 0x80)
	#define COLOR_SKYBLUE        TFTRGB(0x87, 0xCE, 0xEB)
	#define COLOR_BLUEVIOLET     TFTRGB(0x8A, 0x2B, 0xE2)
	#define COLOR_LIGHTGREEN     TFTRGB(0x90, 0xEE, 0x90)
	#define COLOR_DARKVIOLET     TFTRGB(0x94, 0x00, 0xD3)
	#define COLOR_YELLOWGREEN    TFTRGB(0x9A, 0xCD, 0x32)
	#define COLOR_BROWN          TFTRGB(0xA5, 0x2A, 0x2A)
	#define COLOR_DARKGRAY       TFTRGB(0xA9, 0xA9, 0xA9)
	#define COLOR_SIENNA         TFTRGB(0xA0, 0x52, 0x2D)
	#define COLOR_LIGHTBLUE      TFTRGB(0xAD, 0xD8, 0xE6)
	#define COLOR_GREENYELLOW    TFTRGB(0xAD, 0xFF, 0x2F)
	#define COLOR_SILVER         TFTRGB(0xC0, 0xC0, 0xC0)
	#define COLOR_LIGHTGREY      TFTRGB(0xD3, 0xD3, 0xD3)
	#define COLOR_LIGHTCYAN      TFTRGB(0xE0, 0xFF, 0xFF)
	#define COLOR_VIOLET         TFTRGB(0xEE, 0x82, 0xEE)
	#define COLOR_AZUR           TFTRGB(0xF0, 0xFF, 0xFF)
	#define COLOR_BEIGE          TFTRGB(0xF5, 0xF5, 0xDC)
	#define COLOR_MAGENTA        TFTRGB(0xFF, 0x00, 0xFF)
	#define COLOR_TOMATO         TFTRGB(0xFF, 0x63, 0x47)
	#define COLOR_GOLD           TFTRGB(0xFF, 0xD7, 0x00)
	#define COLOR_ORANGE         TFTRGB(0xFF, 0xA5, 0x00)
	#define COLOR_SNOW           TFTRGB(0xFF, 0xFA, 0xFA)
	#define COLOR_YELLOW         TFTRGB(0xFF, 0xFF, 0x00)
	#define COLOR_BROWN   		 TFTRGB(0xA5, 0x2A, 0x2A)	// коричневый
	#define COLOR_PEAR    		 TFTRGB(0xD1, 0xE2, 0x31)	// грушевый

	// Заполнение палитры производится в display2_xltrgb24()

	#define COLORPIP_YELLOW      TFTRGB565(0xFF, 0xFF, 0x00)
	#define COLORPIP_ORANGE      TFTRGB565(0xFF, 0xA5, 0x00)
	#define COLORPIP_BLACK       TFTRGB565(0x00, 0x00, 0x00)
	#define COLORPIP_WHITE       TFTRGB565(0xFF, 0xFF, 0xFF)
	#define COLORPIP_GRAY        TFTRGB565(0x80, 0x80, 0x80)
	#define COLORPIP_DARKGRAY    TFTRGB565(0x70, 0x70, 0x70)
	#define COLORPIP_DARKGREEN   TFTRGB565(0x00, 0x40, 0x00)
	#define COLORPIP_BLUE        TFTRGB565(0x00, 0x00, 0xFF)
	#define COLORPIP_GREEN       TFTRGB565(0x00, 0xFF, 0x00)
	#define COLORPIP_RED         TFTRGB565(0xFF, 0x00, 0x00)
	#define COLORPIP_LOCKED  	 TFTRGB565(0x3C, 0x3C, 0x00)

	#if COLORSTYLE_ATS52
		// new (for ats52).
		#define COLORPIP_GRIDCOLOR		TFTRGB565(128, 0, 0)		//COLOR_GRAY - center marker
		#define COLORPIP_GRIDCOLOR2		TFTRGB565(96, 96, 96)		//COLOR_DARKRED - other markers
		#define COLORPIP_SPECTRUMBG		TFTRGB565(0, 64, 24)			//
		#define COLORMAIN_SPECTRUMBG2	TFTRGB565(0, 24, 8)		//COLOR_xxx - полоса пропускания приемника
		#define COLORPIP_SPECTRUMFG		TFTRGB565(0, 255, 0)		//COLOR_GREEN
		#define DESIGNCOLOR_SPECTRUMFENCE	TFTRGB565(255, 255, 255)	//COLOR_WHITE
		#define COLORPIP_SPECTRUMLINE	COLORPIP_YELLOW

	#else
		// old
		#define COLORPIP_GRIDCOLOR      TFTRGB565(128, 128, 0)        //COLOR_GRAY - center marker
		#define COLORPIP_GRIDCOLOR2     TFTRGB565(128, 0, 0x00)        //COLOR_DARKRED - other markers
		#define COLORPIP_SPECTRUMBG     TFTRGB565(0, 0, 0)            //COLOR_BLACK
		#define COLORMAIN_SPECTRUMBG2    TFTRGB565(0, 128, 128)        //COLOR_CYAN - полоса пропускания приемника
		#define COLORPIP_SPECTRUMFG		TFTRGB565(0, 255, 0)		//COLOR_GREEN
		#define DESIGNCOLOR_SPECTRUMFENCE	TFTRGB565(255, 255, 255)	//COLOR_WHITE
		//#define COLORPIP_SPECTRUMLINE	COLORPIP_GREEN
		#define COLORPIP_SPECTRUMLINE	COLORPIP_YELLOW

	#endif


	// Цвета, используемые на основном экране
	#define COLORMAIN_BLACK COLOR_BLACK
	#define COLORMAIN_WHITE COLOR_WHITE
	#define COLORMAIN_BLUE COLOR_BLUE
	#define COLORMAIN_GREEN COLOR_GREEN
	#define COLORMAIN_RED COLOR_RED
	#define COLORMAIN_GRAY COLOR_GRAY
	#define COLORMAIN_DARKGREEN COLOR_DARKGREEN
	#define COLORMAIN_YELLOW COLOR_YELLOW
	#define COLORMAIN_DARKRED  COLOR_DARKRED

#endif /* LCDMODE_PIP_L8 */

#if LCDMODE_PIP_L8
	#define COLOR_BUTTON_NON_LOCKED		COLORPIP_GREEN
	#define COLOR_BUTTON_PR_NON_LOCKED	COLORPIP_DARKGREEN	// was: COLORPIP_DARKGREEN2
	#define COLOR_BUTTON_LOCKED			COLORPIP_YELLOW
	#define COLOR_BUTTON_PR_LOCKED		COLORPIP_LOCKED // TFTRGB565(0x3C, 0x3C, 0x00)
	#define COLOR_BUTTON_DISABLED		COLORPIP_GRAY // TFTRGB565(0x50, 0x50, 0x50) FIXME: use right value

#else /* LCDMODE_PIP_L8 */
	#define COLOR_BUTTON_NON_LOCKED		COLORPIP_GREEN
	#define COLOR_BUTTON_PR_NON_LOCKED	COLORPIP_DARKGREEN
	#define COLOR_BUTTON_LOCKED			COLORPIP_YELLOW
	#define COLOR_BUTTON_PR_LOCKED		COLORPIP_LOCKED // TFTRGB565(0x3C, 0x3C, 0x00)
	#define COLOR_BUTTON_DISABLED		COLORPIP_GRAY

#endif /* LCDMODE_PIP_L8 */

// Цвета используемые для отображения
// различных элементов на основном экране.

#define LCOLOR	COLORMAIN_GREEN		// цвет левой половины S-метра
#define RCOLOR	COLORMAIN_RED			// цвет правой половины S-метра
#define PWRCOLOR	COLORMAIN_RED		// цвет измерителя мощности
#define SWRCOLOR	COLORMAIN_YELLOW		// цвет SWR-метра

#define OVFCOLOR COLORMAIN_RED
#define LOCKCOLOR COLORMAIN_RED
#define TXRXMODECOLOR COLORMAIN_BLACK
#define MODECOLORBG_TX COLORMAIN_RED
#define MODECOLORBG_RX	COLORMAIN_GREEN

#define MENUGROUPCOLOR COLORMAIN_YELLOW
#define MENUCOLOR COLORMAIN_WHITE
#define MNUVALCOLOR COLORMAIN_WHITE
#define MENUSELCOLOR	COLORMAIN_GREEN

#if COLORSTYLE_RED
	// "All-in-red": FT1000 inspired color scheme
	#define DESIGNBIGCOLOR 		COLORMAIN_RED 		// DARK RED
	#define DESIGNBIGCOLORB 	COLORMAIN_RED		// цвет частоты дополнительного приемника

	#define FMENUACTIVETEXT		COLORMAIN_WHITE
	#define FMENUACTIVEBACK		COLORMAIN_DARKRED
	#define FMENUINACTIVETEXT	COLORMAIN_BLACK
	#define FMENUINACTIVEBACK	COLORMAIN_RED
	#define FMENUTEXT			COLORMAIN_RED
	#define FMENUBACK			COLORMAIN_BLACK

	#define LABELACTIVETEXT		COLORMAIN_WHITE
	#define LABELACTIVEBACK		COLORMAIN_DARKRED
	#define LABELINACTIVETEXT	COLORMAIN_BLACK
	#define LABELINACTIVEBACK	COLORMAIN_RED
	#define LABELTEXT			COLORMAIN_RED
	#define LABELBACK			COLORMAIN_BLACK

#elif COLORSTYLE_ATS52
	/* цветовая схема для Александра ATS52 */
	#define DESIGNBIGCOLOR 		COLORMAIN_YELLOW 	// GOLD
	#define DESIGNBIGCOLORB 	COLORMAIN_YELLOW		// цвет частоты дополнительного приемника

	#define FMENUACTIVETEXT		COLORMAIN_YELLOW
	#define FMENUACTIVEBACK		COLORMAIN_DARKGREEN
	#define FMENUINACTIVETEXT	COLORMAIN_BLACK
	#define FMENUINACTIVEBACK	COLORMAIN_DARKGREEN
	#define FMENUTEXT			COLORMAIN_GREEN
	#define FMENUBACK			COLORMAIN_BLACK


	#define LABELACTIVETEXT		COLORMAIN_YELLOW
	#define LABELACTIVEBACK		COLORMAIN_DARKGREEN
	#define LABELINACTIVETEXT	COLORMAIN_BLACK
	#define LABELINACTIVEBACK	COLORMAIN_DARKGREEN
	#define LABELTEXT			COLORMAIN_GREEN
	#define LABELBACK			COLORMAIN_BLACK

#elif 0
	/* цветовая схема для эксперементов */
	#define DESIGNBIGCOLOR 		COLORMAIN_YELLOW 	// GOLD
	#define DESIGNBIGCOLORB 	COLORMAIN_SPECTRUMBG2		// цвет частоты дополнительного приемника

	#define FMENUACTIVETEXT		COLORMAIN_WHITE
	#define FMENUACTIVEBACK		COLORMAIN_DARKGREEN
	#define FMENUINACTIVETEXT	COLORMAIN_GREEN
	#define FMENUINACTIVEBACK	COLORMAIN_GREEN
	#define FMENUTEXT			COLORMAIN_GREEN
	#define FMENUBACK			COLORMAIN_BLACK

	#define LABELACTIVETEXT		COLORMAIN_WHITE
	#define LABELACTIVEBACK		COLORMAIN_BLACK
	#define LABELINACTIVETEXT	COLORMAIN_GREEN
	#define LABELINACTIVEBACK	COLORMAIN_BLACK
	#define LABELTEXT			COLORMAIN_GREEN
	#define LABELBACK			COLORMAIN_BLACK

#else /* COLORSTYLE_RED */
	#define DESIGNBIGCOLOR 		COLORMAIN_YELLOW 	// GOLD
	#define DESIGNBIGCOLORB 	COLORMAIN_SPECTRUMBG2		// цвет частоты дополнительного приемника

	#define FMENUACTIVETEXT		COLORMAIN_WHITE
	#define FMENUACTIVEBACK		COLORMAIN_DARKGREEN
	#define FMENUINACTIVETEXT	COLORMAIN_BLACK
	#define FMENUINACTIVEBACK	COLORMAIN_GREEN
	#define FMENUTEXT			COLORMAIN_GREEN
	#define FMENUBACK			COLORMAIN_BLACK

	#define LABELACTIVETEXT		COLORMAIN_WHITE
	#define LABELACTIVEBACK		COLORMAIN_DARKGREEN
	#define LABELINACTIVETEXT	COLORMAIN_BLACK
	#define LABELINACTIVEBACK	COLORMAIN_GREEN
	#define LABELTEXT			COLORMAIN_GREEN
	#define LABELBACK			COLORMAIN_BLACK

#endif /* COLORSTYLE_RED */

#endif /* BOARD_H_INCLUDED */
