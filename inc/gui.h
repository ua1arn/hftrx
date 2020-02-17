#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

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
	void buttons_mode_handler(void);
	void buttons_bp_handler (void);
	void buttons_freq_handler (void);
	void window_bp_process (void);

	enum {								// button_handler.state
		BUTTON_PRESSED,					// нажато
		BUTTON_RELEASED,				// отпущено после нажатия внутри элемента
		BUTTON_CANCELLED				// первоначальное состояние или отпущено после нажатия вне элемента
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
		WINDOW_FREQ
	};

	typedef struct {
		uint_fast16_t x1;				// для TYPE_FOOTER_BUTTON координаты от начала экрана,
		uint_fast16_t y1;				// для TYPE_PIP_BUTTON координаты от начала PIP
		uint_fast16_t x2;
		uint_fast16_t y2;
		void(*onClickHandler) (void);	// обработчик события RELEASED
		uint_fast8_t state;				// текущее состояние кнопки
		uint_fast8_t is_locked;			// признак фиксации кнопки
		uint_fast8_t parent;			// индекс окна, в котором будет отображаться кнопка при type = TYPE_PIP_BUTTON
		uint_fast8_t visible;			// рисовать ли кнопку на экране
		uintptr_t payload;
		char * text;					// текст внутри кнопки
		char * text2;
	} button_t;

	static button_t button_handlers [] = {
	//   x1,   y1,  x2,  y2,  onClickHandler,            state,       is_locked,		parent,       visible,      payload,	text
		{ },
		{ 0,   254, 86,  304, button1_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "Mode", },
		{ 89,  254, 175, 304, button2_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "Test", },
		{ 178, 254, 264, 304, button3_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "AGC", },
		{ 267, 254, 353, 304, button4_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "Freq", },
		{ 356, 254, 442, 304, button5_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 445, 254, 531, 304, button6_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 534, 254, 620, 304, button7_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 623, 254, 709, 304, button8_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 712, 254, 798, 304, button8_handler, 	    BUTTON_CANCELLED, BUTTON_NON_LOCKED, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 234,  55, 314, 105, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_LSB, "LSB", },
		{ 319,  55, 399, 105, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_CW,  "CW", },
		{ 404,  55, 484, 105, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_AM,  "AM", },
		{ 489,  55, 569, 105, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGL, "DGL", },
		{ 234, 110, 314, 160, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_USB, "USB", },
		{ 319, 110, 399, 160, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_CWR, "CWR", },
		{ 404, 110, 484, 160, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_NFM, "NFM", },
		{ 489, 110, 569, 160, buttons_mode_handler, BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGU, "DGU", },
		{ 251, 155, 337, 205, buttons_bp_handler,	BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, "Low", "cut", },
		{ 357, 155, 443, 205, buttons_bp_handler, 	BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, "OK", },
		{ 463, 155, 549, 205, buttons_bp_handler, 	BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, "High", "cut", },
		{ 251, 70,  337, 120, set_agc_off, 			BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, "AGC", "off", },
		{ 357, 70,  443, 120, set_agc_slow, 		BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, "AGC", "slow", },
		{ 463, 70,  549, 120, set_agc_fast, 		BUTTON_CANCELLED, BUTTON_NON_LOCKED, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, "AGC", "fast", },
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
		const char * name;
		char text[10];
		COLOR565_T color;
	} label_t;

	static label_t labels[] = {
	//     x,   y,  parent,      visible,     name,   Text,  color
		{ },
		{ 250, 120, WINDOW_BP, NON_VISIBLE, "lbl_low",  "          ", COLOR565_YELLOW, },
		{ 490, 120, WINDOW_BP, NON_VISIBLE, "lbl_high", "          ", COLOR565_YELLOW, },
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
		uint_fast8_t enc2done;
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
		{ WINDOW_BP,    214, 20, 586, 225, "Bandpass",    NON_VISIBLE, 0, window_bp_process},
		{ WINDOW_AGC,   214, 20, 586, 140, "AGC control", NON_VISIBLE, 0, },
		{ WINDOW_FREQ,   100, 0, 350, 200, "Freq", 		  NON_VISIBLE, 0, },
	};
	enum { windows_count = sizeof windows / sizeof windows[1] };

#define COLOR_BUTTON_NON_LOCKED		COLOR565_GREEN
#define COLOR_BUTTON_PR_NON_LOCKED	COLOR565_DARKGREEN2
#define COLOR_BUTTON_LOCKED			COLOR565_YELLOW
#define COLOR_BUTTON_PR_LOCKED		TFTRGB565(0x3C, 0x3C, 0x00)

#endif /* #if WITHTOUCHTEST */
#endif /* GUI_H_INCLUDED */
