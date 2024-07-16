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
	uint8_t error;
} editfreq_t;

typedef struct editfreq2_tag
{
	uint32_t freq;
	uint8_t blinkpos;		// позиция (степень 10) редактируесого символа
	uint8_t blinkstate;	// в месте редактируемого символа отображается подчёркивание (0 - пробел)
} editfreq2_t;

enum DCTX_t { DCTX_FREQ, DCTX_MENU };
typedef struct dctx_tag
{
	enum DCTX_t type;
	const void * pv;
} dctx_t;

// копирование растра в видеобуфер отображения
void display2_vtty(
	uint_fast8_t x0,
	uint_fast8_t y0,
	dctx_t * pctx
	);
void display2_vtty_init(
	uint_fast8_t x0,
	uint_fast8_t y0,
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

void display2_swrsts22(
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

void display2_keyboard_menu(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);

// Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
void display2_keyboard_screen0(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);

// Обработка клавиатуры и валкодеров при нахождении в режиме меню
void display2_keyboard_menu(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);

uint_fast8_t display_getpagesmax(void);	// количество разных вариантов отображения (menuset)
uint_fast8_t display_getpagesleep(void);	// номер варианта отображения для "сна"
uint_fast8_t display_getfreqformat(uint_fast8_t * prjv);	// получить параметры отображения частоты (для функции прямого ввода)

void display2_bgprocess(void);	// выполнение шагов state machine отображения дисплея
void display2_bgreset(void);	// сброс state machine отображения дисплея
void display2_initialize(void);	//

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

// Обработка клавиатуры и валкодеров
void display2_mode_keyboard(
	uint_fast8_t menuset	/* индекс режима отображения (0..DISPLC_MODCOUNT - 1) */
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

// всплывающее меню
void display2_popup(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);

dctx_t * display2_getcontext(void);

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


void board_set_topdb(int_fast16_t v);			/* верхний предел FFT */
void board_set_bottomdb(int_fast16_t v);		/* нижний предел FFT */
void board_set_topdbwf(int_fast16_t v);			/* верхний предел FFT */
void board_set_bottomdbwf(int_fast16_t v);		/* нижний предел FFT */
void board_set_zoomxpow2(uint_fast8_t v);		/* уменьшение отображаемого участка спектра */
void board_set_wflevelsep(uint_fast8_t v); 		/* чувствительность водопада регулируется отдельной парой параметров */
void board_set_view_style(uint_fast8_t v);		/* стиль отображения спектра и панорамы */
void board_set_view3dss_mark(uint_fast8_t v);	/* Для VIEW_3DSS - индикация полосы пропускания на спектре */
void display2_set_showdbm(uint_fast8_t v);			// Отображение уровня сигнала в dBm или S-memter (в зависимости от настроек)
void board_set_afspeclow(int_fast16_t v);		// нижняя частота отображения спектроанализатора
void board_set_afspechigh(int_fast16_t v);		// верхняя частота отображения спектроанализатора
void display2_set_lvlgridstep(uint_fast8_t v);		/* Шаг сетки уровней в децибелах */
void display2_set_rxbwsatu(uint_fast8_t v);		/* 0..100 - насыщнность цвета заполнения "шторки" - индикатор полосы пропускания примника на спкктре. */

#define SWRMIN 10	// минимум - соответствует SWR = 1.0, точность = 0.1

#ifdef __cplusplus
}
#endif /* __cplusplus */

// Цвета используемые для отображения
// различных элементов на основном экране.

#define LCOLOR	COLORPIP_GREEN		// цвет левой половины S-метра
#define RCOLOR	COLORPIP_RED			// цвет правой половины S-метра
#define PWRCOLOR	COLORPIP_RED		// цвет измерителя мощности
#define SWRCOLOR	COLORPIP_YELLOW		// цвет SWR-метра

#define OVFCOLOR COLORPIP_RED
#define LOCKCOLOR COLORPIP_RED
#define TXRXMODECOLOR COLORPIP_BLACK
#define MODECOLORBG_TX COLORPIP_RED
#define MODECOLORBG_RX	COLORPIP_GREEN

#define MENUGROUPCOLOR COLORPIP_YELLOW
#define MENUCOLOR COLORPIP_WHITE
#define MNUVALCOLOR COLORPIP_WHITE
#define MENUSELCOLOR	COLORPIP_GREEN

//#define AFSPECTRE_COLOR COLORPIP_YELLOW
#define AFSPECTRE_COLOR DSGN_SPECTRUMBG2

#if defined (COLORPIP_SHADED)
	// LCDMODE_MAIN_L8 also defied

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
	#define COLORPIP_DARKGREEN   (COLORPIP_BASE + 5) // TFTRGB(0x70, 0x70, 0x70) FIXME: use right value
	#define COLORPIP_BLUE        (COLORPIP_BASE + 6) // TFTRGB(0x00, 0x00, 0xFF)
	#define COLORPIP_GREEN       (COLORPIP_BASE + 7) // TFTRGB(0x00, 0xFF, 0x00)
	#define COLORPIP_RED         (COLORPIP_BASE + 8) // TFTRGB(0xFF, 0x00, 0x00)

	#define COLORPIP_LOCKED  	 	(COLORPIP_BASE + 9) // TFTRGB(0x3C, 0x3C, 0x00)
	#define DSGN_GRIDCOLOR0		(COLORPIP_BASE + 10) // TFTRGB(128, 0, 0)		//COLOR_GRAY - center marker
	#define DSGN_GRIDCOLOR2		(COLORPIP_BASE + 11) // TFTRGB(96, 96, 96)		//COLOR_DARKRED - other markers
	#define DSGN_SPECTRUMBG		(COLORPIP_BASE + 12) // TFTRGB(0, 64, 24)			//
	#define DSGN_SPECTRUMBG2	(COLORPIP_BASE + 13) // TFTRGB(0, 24, 8)		//COLOR_xxx - полоса пропускания приемника
	#define DSGN_SPECTRUMFG		(COLORPIP_BASE + 14) // TFTRGB(0, 255, 0)		// цвет спектра при сполошном заполнении

	#define COLORPIP_DARKGRAY   (COLORPIP_BASE + 15) // TFTRGB(0x00, 0x64, 0x00)

	#define DSGN_SPECTRUMLINE	COLORPIP_YELLOW
	#define DESIGNCOLOR_SPECTRUMFENCE	COLORPIP_WHITE

	#if LCDMODE_MAIN_L8
		// Цвета, используемые на основном экране
		#define COLORPIP_DARKCYAN   COLORPIP_DARKGREEN
		#define COLORPIP_CYAN       COLORPIP_GREEN

		#define DSGN_GRIDDIGITS  DSGN_GRIDCOLOR2
		#define COLORPIP_DARKRED  DSGN_GRIDCOLOR2

	#endif /* LCDMODE_MAIN_L8 */

#else /* */

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

	#define COLORPIP_YELLOW      COLOR_YELLOW
	#define COLORPIP_ORANGE      COLOR_ORANGE
	#define COLORPIP_BLACK       COLOR_BLACK
	#define COLORPIP_WHITE       COLOR_WHITE
	#define COLORPIP_GRAY        COLOR_GRAY
	#define COLORPIP_CYAN        COLOR_CYAN
	#define COLORPIP_OLIVE		 COLOR_OLIVE
	#define COLORPIP_DARKGRAY    COLOR_DARKGRAY
	#define COLORPIP_DARKGREEN   COLOR_DARKGREEN
	#define COLORPIP_DARKCYAN    COLOR_DARKCYAN
	#define COLORPIP_BLUE        COLOR_BLUE
	#define COLORPIP_GREEN       COLOR_GREEN
	#define COLORPIP_RED         COLOR_RED
	#define COLORPIP_DARKRED   	 COLOR_DARKRED

	#define COLORPIP_LOCKED  	 TFTRGB(0x3C, 0x3C, 0x00)

	#if COLORSTYLE_RED
		//
		#define DSGN_GRIDCOLOR0     	COLORPIP_RED        	// center marker
		#define DSGN_GRIDCOLOR2     	COLORPIP_DARKRED      // other markers
		#define DSGN_GRIDDIGITS 		COLORPIP_RED

		#define DSGN_SPECTRUMBG     	COLORPIP_BLACK
		#define DSGN_SPECTRUMBG2   		COLORPIP_DARKRED      // полоса пропускания приемника
		#define DSGN_SPECTRUMFG			COLORPIP_RED			// цвет спектра при сполошном заполнении
		#define DESIGNCOLOR_SPECTRUMFENCE	COLOR_WHITE
		#define DSGN_SPECTRUMLINE		COLORPIP_RED

	#elif COLORSTYLE_GREEN
		// old
		#define DSGN_GRIDCOLOR0     	COLORPIP_GREEN        	// center marker
		#define DSGN_GRIDCOLOR2     	COLORPIP_DARKGREEN      // other markers
		#define DSGN_GRIDDIGITS 		COLORPIP_GREEN

		#define DSGN_SPECTRUMBG     	COLORPIP_BLACK
		#define DSGN_SPECTRUMBG2   		COLORPIP_DARKGREEN      // полоса пропускания приемника
		#define DSGN_SPECTRUMFG			COLORPIP_GREEN			// цвет спектра при сполошном заполнении
		#define DESIGNCOLOR_SPECTRUMFENCE	COLOR_WHITE
		#define DSGN_SPECTRUMLINE		COLORPIP_GREEN

	#elif COLORSTYLE_UA1CEI
		// old
		#define DSGN_GRIDCOLOR0     	COLOR_WHITE        // center marker
		#define DSGN_GRIDCOLOR2     	COLOR_DARKRED        // other markers
		#define DSGN_GRIDDIGITS 		COLOR_YELLOW

		#define DSGN_SPECTRUMBG     	COLOR_BLACK
		#define DSGN_SPECTRUMBG2    	COLORPIP_DARKCYAN        		//COLOR_CYAN - полоса пропускания приемника
		#define DSGN_SPECTRUMFG			COLOR_GREEN		// цвет спектра при сполошном заполнении
		#define DESIGNCOLOR_SPECTRUMFENCE	COLOR_WHITE	//COLOR_WHITE
		//#define DSGN_SPECTRUMLINE		COLORPIP_GREEN
		#define DSGN_SPECTRUMLINE		COLORPIP_YELLOW

	#elif COLORSTYLE_BLUE

	#else
		// old
		#define DSGN_GRIDCOLOR0      TFTRGB(128, 128, 0)        //COLOR_GRAY - center marker
		#define DSGN_GRIDCOLOR2     COLOR_GRAY        //COLOR_DARKRED - other markers
		#define DSGN_GRIDDIGITS 	COLOR_YELLOW
		#define DSGN_SPECTRUMBG     COLOR_BLACK
		#define DSGN_SPECTRUMBG2    COLOR_CYAN        		//COLOR_CYAN - полоса пропускания приемника
		#define DSGN_SPECTRUMFG		COLOR_GREEN		// цвет спектра при сполошном заполнении
		#define DESIGNCOLOR_SPECTRUMFENCE	COLOR_WHITE	//COLOR_WHITE
		//#define DSGN_SPECTRUMLINE	COLORPIP_GREEN
		#define DSGN_SPECTRUMLINE	COLORPIP_YELLOW

	#endif

#endif /* */

#if COLORSTYLE_RED
	// "All-in-red": FT1000 inspired color scheme
	#define DSGN_BIGCOLOR 		COLORPIP_RED 		// DARK RED

	#define DSGN_BIGCOLORB 	COLORPIP_RED		// цвет частоты дополнительного приемника
	#define DSGN_BIGCOLORBINACTIVE COLORPIP_DARKRED

	#define DSGN_FMENUACTIVETEXT		COLORPIP_WHITE
	#define DSGN_FMENUACTIVEBACK		COLORPIP_DARKRED
	#define DSGN_FMENUINACTIVETEXT	COLORPIP_BLACK
	#define DSGN_FMENUINACTIVEBACK	COLORPIP_RED
	#define DSGN_FMENUTEXT			COLORPIP_RED
	#define DSGN_FMENUBACK			COLORPIP_BLACK

	#define DSGN_LABELACTIVETEXT		COLORPIP_WHITE
	#define DSGN_LABELACTIVEBACK		COLORPIP_DARKRED
	#define DSGN_LABELINACTIVETEXT	COLORPIP_BLACK
	#define DSGN_LABELINACTIVEBACK	COLORPIP_RED
	#define DSGN_LABELTEXT			COLORPIP_RED
	#define DSGN_LABELBACK			COLORPIP_BLACK

	#define DSGN_STATETEXT	COLORPIP_RED	// температура, напряжение - was DSGN_LABELTEXT
	#define DSGN_STATEBACK	COLORPIP_BLACK
	#define DSGN_SMLABELTEXT		COLORPIP_GREEN
	#define DSGN_SMLABELBACK		COLORPIP_BLACK

#elif COLORSTYLE_GREEN
	/* цветовая схема для эксперементов */
	#define DSGN_BIGCOLOR 		COLORPIP_YELLOW 	// GOLD

	#define DSGN_BIGCOLORB 	COLORPIP_YELLOW		// цвет частоты и режима ополнительного приемника
	#define DSGN_BIGCOLORBINACTIVE COLORPIP_DARKGREEN

	#define DSGN_FMENUACTIVETEXT		COLORPIP_WHITE
	#define DSGN_FMENUACTIVEBACK		COLORPIP_DARKGREEN

	#define DSGN_FMENUINACTIVETEXT	COLORPIP_BLACK
	#define DSGN_FMENUINACTIVEBACK	COLORPIP_DARKGREEN

	#define DSGN_FMENUTEXT			COLORPIP_GREEN
	#define DSGN_FMENUBACK			COLORPIP_BLACK

	#define DSGN_LABELACTIVETEXT		COLORPIP_WHITE
	#define DSGN_LABELACTIVEBACK		COLORPIP_DARKGREEN

	#define DSGN_LABELINACTIVETEXT	COLORPIP_BLACK
	#define DSGN_LABELINACTIVEBACK	COLORPIP_DARKGREEN

	#define DSGN_LABELTEXT			COLORPIP_GREEN
	#define DSGN_LABELBACK			COLORPIP_BLACK

	#define DSGN_STATETEXT	COLORPIP_GREEN	// температура, напряжение - was DSGN_LABELTEXT
	#define DSGN_STATEBACK	COLORPIP_BLACK
	#define DSGN_SMLABELTEXT		COLORPIP_GREEN
	#define DSGN_SMLABELBACK		COLORPIP_BLACK

#elif COLORSTYLE_UA1CEI

	#define DSGN_BIGCOLOR 		COLORPIP_WHITE //
	#define DSGN_BIGCOLORB 	COLORPIP_YELLOW // цвет частоты дополнительного приемника
	#define DSGN_BIGCOLORBINACTIVE COLORPIP_DARKCYAN

	#define DSGN_FMENUACTIVETEXT		COLORPIP_WHITE
	#define DSGN_FMENUACTIVEBACK		COLORPIP_BLACK
	#define DSGN_FMENUINACTIVETEXT	COLORPIP_DARKCYAN
	#define DSGN_FMENUINACTIVEBACK	COLORPIP_BLACK

	#define DSGN_LABELACTIVETEXT		COLORPIP_WHITE
	#define DSGN_LABELACTIVEBACK		COLORPIP_BLACK

	#define DSGN_LABELINACTIVETEXT	COLORPIP_DARKCYAN
	#define DSGN_LABELINACTIVEBACK	COLORPIP_BLACK

	#define DSGN_LABELTEXT			COLORPIP_WHITE
	#define DSGN_LABELBACK			COLORPIP_BLACK

	#define DSGN_FMENUTEXT			COLORPIP_GREEN
	#define DSGN_FMENUBACK			COLORPIP_BLACK

	#define DSGN_STATETEXT	COLORPIP_GREEN	// температура, напряжение - was DSGN_LABELTEXT
	#define DSGN_STATEBACK	COLORPIP_BLACK
	#define DSGN_SMLABELTEXT		COLORPIP_GREEN
	#define DSGN_SMLABELBACK		COLORPIP_BLACK


#elif COLORSTYLE_BLUE

	#define DSGN_BIGCOLOR 			COLORPIP_WHITE //COLORPIP_YELLOW 	// GOLD
	#define DSGN_BIGCOLORB 			COLORPIP_WHITE //DSGN_SPECTRUMBG2		// цвет частоты дополнительного приемника
	#define DSGN_BIGCOLORBINACTIVE 	COLORPIP_DARKCYAN
	#define DSGN_FMENUACTIVETEXT	COLORPIP_WHITE
	#define DSGN_FMENUACTIVEBACK	COLORPIP_BLACK
	#define DSGN_FMENUINACTIVETEXT	COLORPIP_DARKCYAN
	#define DSGN_FMENUINACTIVEBACK	COLORPIP_BLACK
	#define DSGN_LABELACTIVETEXT	COLORPIP_WHITE
	#define DSGN_LABELACTIVEBACK	COLORPIP_BLACK
	#define DSGN_LABELINACTIVETEXT	COLORPIP_DARKCYAN
	#define DSGN_LABELINACTIVEBACK	COLORPIP_BLACK
	#define DSGN_LABELTEXT			COLORPIP_WHITE
	#define DSGN_LABELBACK			COLORPIP_BLACK
	#define DSGN_FMENUTEXT			COLORPIP_GREEN
	#define DSGN_FMENUBACK			COLORPIP_BLACK
	#define DSGN_GRIDCOLOR      	COLORPIP_OLIVE        // center marker
	#define DSGN_GRIDCOLOR0     	COLORPIP_DARKRED        // other markers
	#define DSGN_GRIDCOLOR2     	COLORPIP_DARKRED        // other markers
	#define DSGN_SPECTRUMBG     	COLORPIP_BLACK            //COLORPIP_BLACK
	#define DSGN_SPECTRUMBG2    	COLORPIP_DARKCYAN        //  полоса пропускания приемника
	#define DSGN_SPECTRUMFG			COLORPIP_GREEN
	#define DSGN_SPECTRUMFENCE		COLORPIP_WHITE
	#define DSGN_SPECTRUMLINE		COLORPIP_YELLOW
	#define DSGN_GRIDDIGITS 		COLORPIP_YELLOW
	#define DSGN_STATETEXT			COLORPIP_WHITE	// температура, напряжение - was DSGN_LABELTEXT
	#define DSGN_STATEBACK			COLORPIP_BLACK
	#define DSGN_SMLABELTEXT		COLORPIP_GREEN
	#define DSGN_SMLABELBACK		COLORPIP_BLACK

#else /* COLORSTYLE_RED */

	#if LCDMODE_MAIN_L8
		#define DSGN_BIGCOLOR 		COLORPIP_WHITE //COLORPIP_YELLOW 	// GOLD
		#define DSGN_BIGCOLORB 	COLORPIP_WHITE //DSGN_SPECTRUMBG2		// цвет частоты дополнительного приемника
		#define DSGN_BIGCOLORBINACTIVE DSGN_SPECTRUMBG2

		#define DSGN_FMENUACTIVETEXT		COLORPIP_WHITE
		#define DSGN_FMENUACTIVEBACK		COLORPIP_BLACK

		#define DSGN_FMENUINACTIVETEXT	DSGN_SPECTRUMBG2
		#define DSGN_FMENUINACTIVEBACK	COLORPIP_BLACK

		#define DSGN_LABELACTIVETEXT		COLORPIP_WHITE
		#define DSGN_LABELACTIVEBACK		COLORPIP_BLACK

		#define DSGN_LABELINACTIVETEXT	DSGN_SPECTRUMBG2
		#define DSGN_LABELINACTIVEBACK	COLORPIP_BLACK

		#define DSGN_LABELTEXT			COLORPIP_WHITE
		#define DSGN_LABELBACK			COLORPIP_BLACK

		#define DSGN_STATETEXT	COLORPIP_WHITE	// температура, напряжение - was DSGN_LABELTEXT
		#define DSGN_STATEBACK	COLORPIP_BLACK

		#define DSGN_FMENUTEXT			COLORPIP_GREEN
		#define DSGN_FMENUBACK			COLORPIP_BLACK
		#define DSGN_SMLABELTEXT		COLORPIP_GREEN
		#define DSGN_SMLABELBACK		COLORPIP_BLACK

	#else /* LCDMODE_MAIN_L8 */
		#define DSGN_BIGCOLOR 		COLORPIP_WHITE //COLORPIP_YELLOW 	// GOLD
		#define DSGN_BIGCOLORB 	COLORPIP_WHITE //DSGN_SPECTRUMBG2		// цвет частоты дополнительного приемника
		#define DSGN_BIGCOLORBINACTIVE COLORPIP_GRAY

		#define DSGN_FMENUACTIVETEXT		COLORPIP_WHITE
		#define DSGN_FMENUACTIVEBACK		COLORPIP_GRAY
		#define DSGN_FMENUINACTIVETEXT	COLORPIP_BLACK
		#define DSGN_FMENUINACTIVEBACK	COLORPIP_GRAY

		#define DSGN_LABELACTIVETEXT		COLORPIP_WHITE
		#define DSGN_LABELACTIVEBACK		COLORPIP_GRAY

		#define DSGN_LABELINACTIVETEXT	COLORPIP_BLACK
		#define DSGN_LABELINACTIVEBACK	COLORPIP_GRAY

		#define DSGN_LABELTEXT			COLORPIP_WHITE
		#define DSGN_LABELBACK			COLORPIP_BLACK

		#define DSGN_STATETEXT	COLORPIP_WHITE	// температура, напряжение - was DSGN_LABELTEXT
		#define DSGN_STATEBACK	COLORPIP_BLACK

		#define DSGN_FMENUTEXT			COLORPIP_GREEN
		#define DSGN_FMENUBACK			COLORPIP_BLACK
		#define DSGN_SMLABELTEXT		COLORPIP_GREEN
		#define DSGN_SMLABELBACK		COLORPIP_BLACK

	#endif /* LCDMODE_MAIN_L8 */

#endif /* COLORSTYLE_RED */

#endif /* BOARD_H_INCLUDED */
