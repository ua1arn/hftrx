#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#if LCDMODE_LTDC_PIPL8

	#define COLORPIP_BASE 240	// should be match to PALETTESIZE

	// Заполнение палитры производится в display2_xltrgb24()

	#define COLORPIP_YELLOW      (COLORPIP_BASE + 0) // TFTRGB(0xFF, 0xFF, 0x00)
	#define COLORPIP_ORANGE      (COLORPIP_BASE + 1) // TFTRGB(0xFF, 0xA5, 0x00)
	#define COLORPIP_BLACK       (COLORPIP_BASE + 2) // TFTRGB(0x00, 0x00, 0x00)
	#define COLORPIP_WHITE       (COLORPIP_BASE + 3) // TFTRGB(0xFF, 0xFF, 0xFF)
	#define COLORPIP_GRAY        (COLORPIP_BASE + 4) // TFTRGB(0x80, 0x80, 0x80)
	#define COLORPIP_DARKGREEN   (COLORPIP_BASE + 5) // TFTRGB(0x00, 0x64, 0x00)
	#define COLORPIP_BLUE        (COLORPIP_BASE + 6) // TFTRGB(0x00, 0x00, 0xFF)
	#define COLORPIP_GREEN       (COLORPIP_BASE + 7) // TFTRGB(0x00, 0xFF, 0x00)
	#define COLORPIP_RED         (COLORPIP_BASE + 8) // TFTRGB(0xFF, 0x00, 0x00)
	#define COLORPIP_LOCKED  	 (COLORPIP_BASE + 9) // TFTRGB(0x3C, 0x3C, 0x00)

	#define COLORPIP_GRIDCOLOR		(COLORPIP_BASE + 10) // TFTRGB565(128, 0, 0)		//COLOR_GRAY - center marker
	#define COLORPIP_GRIDCOLOR2		(COLORPIP_BASE + 11) // TFTRGB565(96, 96, 96)		//COLOR_DARKRED - other markers
	#define COLORPIP_SPECTRUMBG		(COLORPIP_BASE + 12) // TFTRGB565(0, 64, 24)			//
	#define COLORPIP_SPECTRUMBG2	(COLORPIP_BASE + 13) // TFTRGB565(0, 24, 8)		//COLOR_xxx - полоса пропускания приемника
	#define COLORPIP_SPECTRUMFG		(COLORPIP_BASE + 14) // TFTRGB565(0, 255, 0)		//COLOR_GREEN
	#define COLORPIP_SPECTRUMFENCE	(COLORPIP_BASE + 15) // TFTRGB565(255, 255, 255)	//COLOR_WHITE

#if COLORSTYLE_ATS52
		// new (for ats52).
		#define COLORPIP_SPECTRUMLINE	COLORPIP_YELLOW

	#else
		// old
		//#define COLORPIP_SPECTRUMLINE	COLORPIP_GREEN
		#define COLORPIP_SPECTRUMLINE	COLORPIP_YELLOW

	#endif

	// Цвета, используемые на основном экране
	#define COLORMAIN_BLACK COLORPIP_BLACK
	#define COLORMAIN_WHITE COLORPIP_WHITE
	#define COLORMAIN_BLUE COLORPIP_BLUE
	#define COLORMAIN_GREEN COLORPIP_GREEN
	#define COLORMAIN_RED COLORPIP_RED
	#define COLORMAIN_GRAY COLORPIP_GRAY
	#define COLORMAIN_DARKGREEN COLORPIP_DARKGREEN
	#define COLORMAIN_YELLOW COLORPIP_YELLOW

#else /* LCDMODE_LTDC_PIPL8 */

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
	#define COLORPIP_DARKGREEN   TFTRGB565(0x00, 0x64, 0x00)
	#define COLORPIP_BLUE        TFTRGB565(0x00, 0x00, 0xFF)
	#define COLORPIP_GREEN       TFTRGB565(0x00, 0xFF, 0x00)
	#define COLORPIP_RED         TFTRGB565(0xFF, 0x00, 0x00)
	#define COLORPIP_LOCKED  	 TFTRGB565(0x3C, 0x3C, 0x00)

	#if COLORSTYLE_ATS52
		// new (for ats52).
		#define COLORPIP_GRIDCOLOR		TFTRGB565(128, 0, 0)		//COLOR_GRAY - center marker
		#define COLORPIP_GRIDCOLOR2		TFTRGB565(96, 96, 96)		//COLOR_DARKRED - other markers
		#define COLORPIP_SPECTRUMBG		TFTRGB565(0, 64, 24)			//
		#define COLORPIP_SPECTRUMBG2	TFTRGB565(0, 24, 8)		//COLOR_xxx - полоса пропускания приемника
		#define COLORPIP_SPECTRUMFG		TFTRGB565(0, 255, 0)		//COLOR_GREEN
		#define COLORPIP_SPECTRUMFENCE	TFTRGB565(255, 255, 255)	//COLOR_WHITE
		#define COLORPIP_SPECTRUMLINE	COLORPIP_YELLOW

	#else
		// old
		#define COLORPIP_GRIDCOLOR      TFTRGB565(128, 128, 0)        //COLOR_GRAY - center marker
		#define COLORPIP_GRIDCOLOR2     TFTRGB565(128, 0, 0x00)        //COLOR_DARKRED - other markers
		#define COLORPIP_SPECTRUMBG     TFTRGB565(0, 0, 0)            //COLOR_BLACK
		#define COLORPIP_SPECTRUMBG2    TFTRGB565(0, 128, 128)        //COLOR_CYAN - полоса пропускания приемника
		#define COLORPIP_SPECTRUMFG		TFTRGB565(0, 255, 0)		//COLOR_GREEN
		#define COLORPIP_SPECTRUMFENCE	TFTRGB565(255, 255, 255)	//COLOR_WHITE
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

#endif /* LCDMODE_LTDC_PIPL8 */

#if LCDMODE_LTDC_PIPL8
	#define COLOR_BUTTON_NON_LOCKED		COLORPIP_GREEN
	#define COLOR_BUTTON_PR_NON_LOCKED	COLORPIP_BLUE	// was: COLORPIP_DARKGREEN2
	#define COLOR_BUTTON_LOCKED			COLORPIP_YELLOW
	#define COLOR_BUTTON_PR_LOCKED		COLORPIP_LOCKED // TFTRGB565(0x3C, 0x3C, 0x00)

#else /* LCDMODE_LTDC_PIPL8 */
	#define COLOR_BUTTON_NON_LOCKED		COLORPIP_GREEN
	#define COLOR_BUTTON_PR_NON_LOCKED	COLORPIP_DARKGREEN
	#define COLOR_BUTTON_LOCKED			COLORPIP_YELLOW
	#define COLOR_BUTTON_PR_LOCKED		COLORPIP_LOCKED // TFTRGB565(0x3C, 0x3C, 0x00)
	#define COLOR_BUTTON_DISABLED		TFTRGB565(0x50, 0x50, 0x50)

#endif /* LCDMODE_LTDC_PIPL8 */

// Цвета используемые для отображения
// различных элементов на основном экране.

#define LCOLOR	COLORMAIN_GREEN		// цвет левой половины S-метра
#define RCOLOR	COLORMAIN_RED			// цвет правой половины S-метра
#define PWRCOLOR	COLORMAIN_RED		// цвет измерителя мощности
#define SWRCOLOR	COLORMAIN_YELLOW		// цвет SWR-метра

#define OVFCOLOR COLORMAIN_RED
#define LOCKCOLOR COLORMAIN_RED
#define MODECOLOR COLORMAIN_WHITE
#define TXRXMODECOLOR COLORMAIN_BLACK
#define MODECOLORBG_TX COLORMAIN_RED
#define MODECOLORBG_RX	COLORMAIN_GREEN

#define MENUGROUPCOLOR COLORMAIN_YELLOW
#define MENUCOLOR COLORMAIN_WHITE
#define MNUVALCOLOR COLORMAIN_WHITE
#define MENUSELCOLOR	COLORMAIN_GREEN

#if COLORSTYLE_RED
	// "All-in-red": FT1000 inspired color scheme
	#define DESIGNBIGCOLOR COLORMAIN_RED 		// DARK RED
	//#define DESIGNBIGCOLORHALF COLORMAIN_RED 	// DARK RED
	#define DESIGNCOLORSTATE	COLORMAIN_RED
	#define DESIGNCOLORDARKSTATE	COLORMAIN_DARKRED

#else /* COLORSTYLE_RED */
	#define DESIGNBIGCOLOR COLORMAIN_YELLOW 		// GOLD
	//#define DESIGNBIGCOLORHALF COLORMAIN_YELLOW
	#define DESIGNCOLORSTATE	COLORMAIN_GREEN
	#define DESIGNCOLORDARKSTATE	COLORMAIN_DARKGREEN

#endif /* COLORSTYLE_RED */

#include "keyboard.h"

#if WITHTOUCHTEST
	void button1_handler(void);
	void button2_handler(void);
	void button3_handler(void);
	void button4_handler(void);
	void button5_handler(void);
	void button6_handler(void);
	void button7_handler(void);
	void button8_handler(void);
	void button9_handler(void);
	void buttons_mode_handler(void);
	void buttons_bp_handler (void);
	void buttons_freq_handler (void);
	void buttons_menu_handler (void);
	void window_bp_process (void);
	void window_menu_process (void);
	void window_freq_process (void);

	enum {								// button_handler.state
		BUTTON_PRESSED,					// нажато
		BUTTON_RELEASED,				// отпущено после нажатия внутри элемента
		BUTTON_CANCELLED,				// первоначальное состояние или отпущено после нажатия вне элемента
		BUTTON_DISABLED					// кнопка заблокирована для нажатия
	};

	enum {
		BUTTON_NON_LOCKED,
		BUTTON_LOCKED
	};

	enum {								// button_handler.visible & windowpip.is_show
		NON_VISIBLE,					// parent window закрыто, кнопка не отрисовывается
		VISIBLE							// parent window на экране, кнопка отрисовывается
	};

	enum {								// button_handler.parent & windowpip.window_id
		FOOTER,							// постоянно отображаемые кнопки внизу экрана
		WINDOW_MODES,					// переключение режимов работы, видов модуляции
		WINDOW_BP,						// регулировка полосы пропускания фильтров выбранного режима
		WINDOW_AGC,						// выбор пресетов настроек АРУ для текущего режима модуляции
		WINDOW_FREQ,
		WINDOW_MENU
	};

	typedef struct {
		uint_fast16_t x1;				// координаты от начала PIP
		uint_fast16_t y1;
		uint_fast16_t x2;
		uint_fast16_t y2;
		void(*onClickHandler) (void);	// обработчик события RELEASED
		uint_fast8_t state;				// текущее состояние кнопки
		uint_fast8_t is_locked;			// признак фиксации кнопки
		uint_fast8_t parent;			// индекс окна, в котором будет отображаться кнопка
		uint_fast8_t visible;			// рисовать ли кнопку на экране
		uintptr_t payload;
		char text[20];					// текст внутри кнопки
	} button_t;

	static button_t button_handlers [] = {
	//   x1,   y1,  x2,  y2,  onClickHandler,            state,       is_locked,		parent,       visible,      payload,	text
		{ },
		{ 0,   254, 86,  304, button1_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "Mode", },
		{ 89,  254, 175, 304, button2_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "AF filter", },
		{ 178, 254, 264, 304, button3_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "AGC", },
		{ 267, 254, 353, 304, button4_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "Freq", },
		{ 356, 254, 442, 304, button5_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 445, 254, 531, 304, button6_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 534, 254, 620, 304, button7_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 623, 254, 709, 304, button8_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 712, 254, 798, 304, button9_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "System settings", },
		{ 234,  55, 314, 105, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_LSB, "LSB", },
		{ 319,  55, 399, 105, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_CW,  "CW", },
		{ 404,  55, 484, 105, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_AM,  "AM", },
		{ 489,  55, 569, 105, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGL, "DGL", },
		{ 234, 110, 314, 160, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_USB, "USB", },
		{ 319, 110, 399, 160, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_CWR, "CWR", },
		{ 404, 110, 484, 160, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_NFM, "NFM", },
		{ 489, 110, 569, 160, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGU, "DGU", },
		{ 251, 155, 337, 205, buttons_bp_handler,	BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, "Low cut", },
		{ 357, 155, 443, 205, buttons_bp_handler, 	BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, "OK", },
		{ 463, 155, 549, 205, buttons_bp_handler, 	BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, "High cut", },
		{ 251, 70,  337, 120, set_agc_off, 			BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, "AGC off", },
		{ 357, 70,  443, 120, set_agc_slow, 		BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, "AGC slow", },
		{ 463, 70,  549, 120, set_agc_fast, 		BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, "AGC fast", },
		{ 120, 28,  170, 78,  buttons_freq_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_SPLIT,  		"1", },
		{ 173, 28,  223, 78,  buttons_freq_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_A_EQ_B, 		"2", },
		{ 226, 28,  276, 78,  buttons_freq_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_A_EX_B, 		"3", },
		{ 279, 28,  329, 78,  buttons_freq_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_BKIN,   		"<-", },
		{ 120, 81,  170, 131, buttons_freq_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_ATUBYPASS, 	"4", },
		{ 173, 81,  223, 131, buttons_freq_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_ANTENNA, 		"5", },
		{ 226, 81,  276, 131, buttons_freq_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_ATT,     		"6", },
		{ 279, 81,  329, 131, buttons_freq_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_ENTERFREQDONE, "OK", },
		{ 120, 134, 170, 184, buttons_freq_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_MOX, 			"7", },
		{ 173, 134, 223, 184, buttons_freq_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_RECORDTOGGLE, 	"8", },
		{ 226, 134, 276, 184, buttons_freq_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_LDSPTGL, 		"9", },
		{ 279, 134, 329, 184, buttons_freq_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_VOXTOGGLE, 	"0", },
	};
	enum { button_handlers_count = sizeof button_handlers / sizeof button_handlers[1] };

	typedef struct {
		uint_fast16_t x;
		uint_fast16_t y;
		uint_fast8_t parent;
		uint_fast8_t visible;
		const char name[12];
		char text[20];
		COLOR565_T color;
	} label_t;

	static label_t labels[] = {
	//     x,   y,  parent,      visible,     name,   Text,  color
		{ },
		{ 250, 120, WINDOW_BP, NON_VISIBLE, "lbl_low",  "", COLORPIP_YELLOW, },
		{ 490, 120, WINDOW_BP, NON_VISIBLE, "lbl_high", "", COLORPIP_YELLOW, },
		{ 100,  50, WINDOW_MENU, NON_VISIBLE, "lbl_group_1", "", COLORPIP_YELLOW, },
		{ 100,  75, WINDOW_MENU, NON_VISIBLE, "lbl_group_2", "", COLORPIP_YELLOW, },
		{ 100, 100, WINDOW_MENU, NON_VISIBLE, "lbl_group_3", "", COLORPIP_YELLOW, },
		{ 100, 125, WINDOW_MENU, NON_VISIBLE, "lbl_group_4", "", COLORPIP_YELLOW, },
		{ 100, 150, WINDOW_MENU, NON_VISIBLE, "lbl_group_5", "", COLORPIP_YELLOW, },
		{ 100, 175, WINDOW_MENU, NON_VISIBLE, "lbl_group_6", "", COLORPIP_YELLOW, },
		{ 100, 200, WINDOW_MENU, NON_VISIBLE, "lbl_group_7", "", COLORPIP_YELLOW, },
	};
	enum { labels_count = sizeof labels / sizeof labels[1] };

	typedef struct {
		uint_fast16_t last_pressed_x; 	 // последняя точка касания экрана
		uint_fast16_t last_pressed_y;
		uint_fast8_t selected;			 // индекс последнего выбранного элемента
		uint_fast8_t state;				 // последнее состояние
		uint_fast8_t is_touching_screen; // есть ли касание экрана в данный момент
		uint_fast8_t is_after_touch; 	 // есть ли касание экрана после выхода точки касания из элемента
		uint_fast8_t fix;				 // первые координаты после нажатия от контролера тачскрина приходят старые, пропускаем
		uint_fast8_t window_to_draw;	 // индекс записи с описанием запрошенного к отображению окна
		uint_fast8_t enc2busy;			 // второй энкодер выделен для обработки данных окна
		int_least16_t enc2rotate;		 // признак поворота второго энкодера
		uint_fast8_t enc2done;			 // событие от энкодера обработано, можно получать новые данные
	} gui_t;

	static gui_t gui = { 0, 0, 0, BUTTON_CANCELLED, 0, 0, 1, 0, 0, 0, 1, };

	typedef struct {
		uint_fast8_t window_id;			// в окне будут отображаться кнопки с соответствующим полем for_window
		uint_fast16_t x1;
		uint_fast16_t y1;
		uint_fast16_t x2;
		uint_fast16_t y2;
		char * title;					// текст, выводимый в заголовке окна
		uint_fast8_t is_show;			// запрос на отрисовку окна
		uint_fast8_t first_call;		// признак первого вызова для различных инициализаций
		void(*onVisibleProcess) (void);
	} windowpip_t;

	static windowpip_t windows[] = {
	//     window_id,   x1,  y1, x2,  y2,  title,         is_show, first_call, onVisibleProcess
		{ },
		{ WINDOW_MODES, 214, 20, 586, 175, "Select mode", NON_VISIBLE, 0, },
		{ WINDOW_BP,    214, 20, 586, 225, "Bandpass",    NON_VISIBLE, 0, window_bp_process, },
		{ WINDOW_AGC,   214, 20, 586, 140, "AGC control", NON_VISIBLE, 0, },
		{ WINDOW_FREQ,   100, 0, 350, 200, "Freq", 		  NON_VISIBLE, 0, window_freq_process, },
		{ WINDOW_MENU,   50, 10, 699, 220, "Settings",	  NON_VISIBLE, 0, window_menu_process, },
	};
	enum { windows_count = sizeof windows / sizeof windows[1] };

	#endif /* #if WITHTOUCHTEST */
#endif /* GUI_H_INCLUDED */
