/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "audio.h"

#include "display/display.h"
#include "formats.h"

#include <string.h>
#include <math.h>
#include "gui.h"


#if LCDMODE_LTDC

#include "./display/fontmaps.h"

void display_radius(int xc, int yc, unsigned gs, unsigned r1, unsigned r2, COLOR_T color);
void display_segm(int xc, int yc, unsigned gs, unsigned ge, unsigned r, int step, COLOR_T color);


static uint_fast16_t normalize(uint_fast16_t raw, uint_fast16_t rawmin,	uint_fast16_t rawmax, uint_fast16_t range)
	{
		if (rawmin < rawmax)
		{
			// Normal direction
			const uint_fast16_t distance = rawmax - rawmin;
			if (raw < rawmin)
				return 0;
			raw = raw - rawmin;
			if (raw > distance)
				return range;
			return (uint_fast32_t) raw * range / distance;
		}
		else
		{
			// reverse direction
			const uint_fast16_t distance = rawmin - rawmax;
			if (raw >= rawmin)
				return 0;
			raw = rawmin - raw;
			if (raw > distance)
				return range;
			return (uint_fast32_t) raw * range / distance;
		}
	}

	void
	display_smeter2(
			uint_fast8_t x,
			uint_fast8_t y,
			void * pv
			)
	{
		enum { ADDRCELLHEIGHT = 15 };
		int i;
		enum { halfsect = 30 };
		enum { gm = 270 };
		enum { gs = gm - halfsect };
		int ge = gm + halfsect;
		int stripewidth = 12; //16;
		int r1 = 7 * ADDRCELLHEIGHT - 8;	//350;
		int r2 = r1 - stripewidth;
		int yc = y * ADDRCELLHEIGHT;	//560;
		int xc = x * ADDRCELLHEIGHT;	//120;
		COLOR_T ct;

		uint_fast8_t tracemax;
		static int gv, gv_trace;;
		static int gv_old = gs, gv_trace_old = gs;
		if (hamradio_get_tx())														// угол поворота стрелки; 246 минимум, 270 середина, 294 максимум
		{																			// добавить учет калибровок
			gv = gs + normalize(board_getpwrmeter(& tracemax), 0, 200, ge - gs);
			gv_trace = gs + normalize(tracemax, 0, 200, ge - gs);
		}
		else
		{
			gv = gs + normalize(board_getsmeter(& tracemax, 0, UINT8_MAX, 0), 120, 250, ge - gs); //270 + 24;
			gv_trace = gs + normalize(tracemax, 120, 250, ge - gs);
		}

		int rv1 = 7 * ADDRCELLHEIGHT;	//350;
		int rv2 = rv1 - 3 * ADDRCELLHEIGHT;
		enum { step1 = 3 };		// шаг для оцифровки S
		enum { step2 = 4 };		// шаг для оцифровки плюсов
		static const int markers [] =
		{
			//gs + 0 * step1,
			gs + 2 * step1,		// S1
			gs + 4 * step1,		// S3
			gs + 6 * step1,		// S5
			gs + 8 * step1,		// S7
			gs + 10 * step1,	// S9
		};
		static const int markersR [] =
		{
			gm + 2 * step2,	//
			gm + 4 * step2,
			gm + 6 * step2,
		};
		static const int markers2 [] =
		{
			//gs + 1 * step1,
			gs + 3 * step1,		// S2
			gs + 5 * step1,		// S4
			gs + 7 * step1,		// S6
			gs + 9 * step1,		// S8
		};
		static const int markers2R [] =
		{
			gm + 1 * step2,
			gm + 3 * step2,
			gm + 5 * step2,
		};

		const COLOR_T smeter = COLORMAIN_WHITE;
		const COLOR_T smeterplus = COLORMAIN_DARKRED;

		display_segm(xc, yc, gs, gm, r1, 1, smeter);
		display_segm(xc, yc, gm, ge, r1, 1, smeterplus);

		display_segm(xc, yc, gs, ge, r2, 1, COLORMAIN_WHITE);		// POWER
		//display_radius(xc, yc, gs, r1, r2, COLOR_RED);
		//display_radius(xc, yc, ge, r1, r2, COLOR_RED);

		for (i = 0; i < sizeof markers / sizeof markers [0]; ++ i)
		{
			display_radius(xc, yc, markers [i], r1, r1 + 8, smeter);
		}
		for (i = 0; i < sizeof markers2 / sizeof markers2 [0]; ++ i)
		{
			display_radius(xc, yc, markers2 [i], r1, r1 + 4, smeter);
		}
		for (i = 0; i < sizeof markersR / sizeof markersR [0]; ++ i)
		{
			display_radius(xc, yc, markersR [i], r1, r1 + 8, smeterplus);
		}
		for (i = 0; i < sizeof markers2R / sizeof markers2R [0]; ++ i)
		{
			display_radius(xc, yc, markers2R [i], r1, r1 + 4, smeterplus);
		}

	//		display_segm(xc, yc, gs, ge, rv1, 1, COLORMAIN_BLUE);	// показывает зоны перемещения стрелки
	//		display_segm(xc, yc, gs, ge, rv2, 1, COLORMAIN_BLUE);	// показывает зоны перемещения стрелки

		display_radius(xc - 1, yc, gv_trace_old, r1 - 1, r2 + 1, COLORMAIN_BLACK);
		display_radius(xc, yc, gv_trace_old, r1 - 1, r2 + 1, COLORMAIN_BLACK);
		display_radius(xc + 1, yc, gv_trace_old, r1 - 1, r2 + 1, COLORMAIN_BLACK);

		ct = gv_trace > gm ? COLORMAIN_RED : COLORMAIN_GREEN;
		display_radius(xc - 1, yc, gv_trace, r1 - 1, r2 + 1, ct);
		display_radius(xc, yc, gv_trace, r1 - 1, r2 + 1, ct);
		display_radius(xc + 1, yc, gv_trace, r1 - 1, r2 + 1, ct);

		display_radius(xc - 1, yc, gv_old, rv1, rv2, COLORMAIN_BLACK);
		display_radius(xc, yc, gv_old, rv1, rv2, COLORMAIN_BLACK);
		display_radius(xc + 1, yc, gv_old, rv1, rv2, COLORMAIN_BLACK);

		ct = gv > gm ? COLORMAIN_RED : COLORMAIN_GREEN;
		display_radius(xc - 1, yc, gv, rv1, rv2, ct);
		display_radius(xc, yc, gv, rv1, rv2, ct);
		display_radius(xc + 1, yc, gv, rv1, rv2, ct);

		gv_old = gv;
		gv_trace_old = gv_trace;
		(void) pv;
	}

#if WITHTOUCHGUI
	#include "keyboard.h"
	#include "list.h"

void button1_handler(void);
	void button2_handler(void);
	void button3_handler(void);
	void button4_handler(void);
	void button5_handler(void);
	void button6_handler(void);
	void button7_handler(void);
	void button8_handler(void);
	void button9_handler(void);
	void labels_menu_handler (void);
	void buttons_mode_handler(void);
	void buttons_bp_handler(void);
	void buttons_freq_handler(void);
	void buttons_menu_handler(void);
	void window_bp_process(void);
	void window_menu_process(void);
	void window_freq_process(void);
	void encoder2_menu (enc2_menu_t * enc2_menu);

	enum {
		TYPE_DUMMY,
		TYPE_BUTTON,
		TYPE_LABEL
	};

	enum {
		PRESSED,						// нажато
		RELEASED,						// отпущено после нажатия внутри элемента
		CANCELLED,						// первоначальное состояние или отпущено после нажатия вне элемента
		DISABLED						// заблокировано для нажатия
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
		WINDOW_MENU,
		WINDOW_ENC2
	};

	typedef struct {
		uint_fast16_t x1;				// координаты от начала PIP
		uint_fast16_t y1;
		uint_fast16_t x2;
		uint_fast16_t y2;
		void(*onClickHandler) (void);	// обработчик события RELEASED
		uint_fast8_t state;				// текущее состояние кнопки
		uint_fast8_t is_locked;			// признак фиксации кнопки
		uint_fast8_t is_trackable;		// получение относительных координат точки перемещения нажатия, нужно ли для кнопок?
		uint_fast8_t parent;			// индекс окна, в котором будет отображаться кнопка
		uint_fast8_t visible;			// рисовать ли кнопку на экране
		uintptr_t payload;
		char text[20];					// текст внутри кнопки, разделитель строк |, не более 2х строк
	} button_t;

	static button_t button_handlers [] = {
	//   x1,   y1,  x2,  y2,  onClickHandler,         state,   is_locked, is_trackable,	parent,    visible,      payload,	text
		{ },
		{   0, 260,  86, 304, button1_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "Mode", },
		{  89, 260, 175, 304, button2_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "AF|filter", },
		{ 178, 260, 264, 304, button3_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "AGC", },
		{ 267, 260, 353, 304, button4_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "Freq", },
		{ 356, 260, 442, 304, button5_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 445, 260, 531, 304, button6_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 534, 260, 620, 304, button7_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 623, 260, 709, 304, button8_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 712, 260, 798, 304, button9_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "System|settings", },
		{ 234,  55, 314, 105, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_LSB, "LSB", },
		{ 319,  55, 399, 105, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_CW,  "CW", },
		{ 404,  55, 484, 105, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_AM,  "AM", },
		{ 489,  55, 569, 105, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGL, "DGL", },
		{ 234, 110, 314, 160, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_USB, "USB", },
		{ 319, 110, 399, 160, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_CWR, "CWR", },
		{ 404, 110, 484, 160, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_NFM, "NFM", },
		{ 489, 110, 569, 160, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGU, "DGU", },
		{ 251, 155, 337, 195, buttons_bp_handler,	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, "Low|cut", },
		{ 357, 155, 443, 195, buttons_bp_handler, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, "OK", },
		{ 463, 155, 549, 195, buttons_bp_handler, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, "High|cut", },
		{ 251, 155, 337, 195, buttons_bp_handler,	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, "Width", },
		{ 463, 155, 549, 195, buttons_bp_handler, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, "Pitch", },
		{ 251,  70, 337, 110, set_agc_off, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, "AGC|off", },
		{ 357,  70, 443, 110, set_agc_slow, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, "AGC|slow", },
		{ 463,  70, 549, 110, set_agc_fast, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, "AGC|fast", },
		{ 120,  28, 170,  78, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_SPLIT,  		"1", },
		{ 173,  28, 223,  78, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_A_EQ_B, 		"2", },
		{ 226,  28, 276,  78, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_A_EX_B, 		"3", },
		{ 279,  28, 329,  78, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_BKIN,   		"<-", },
		{ 120,  81, 170, 131, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_ATUBYPASS, 	"4", },
		{ 173,  81, 223, 131, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_ANTENNA, 		"5", },
		{ 226,  81, 276, 131, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_ATT,     		"6", },
		{ 279,  81, 329, 131, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_ENTERFREQDONE, "OK", },
		{ 120, 134, 170, 184, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_MOX, 			"7", },
		{ 173, 134, 223, 184, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_RECORDTOGGLE, 	"8", },
		{ 226, 134, 276, 184, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_LDSPTGL, 		"9", },
		{ 279, 134, 329, 184, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_VOXTOGGLE, 	"0", },
		{   0, 	 0,   0,   0, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU,  NON_VISIBLE, UINTPTR_MAX, 			"-", } ,
		{   0, 	 0,   0,   0, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU,  NON_VISIBLE, UINTPTR_MAX, 			"+", } ,
	};
	enum { button_handlers_count = sizeof button_handlers / sizeof button_handlers[0] };

	typedef struct {
		uint_fast16_t x;
		uint_fast16_t y;
		uint_fast8_t parent;
		uint_fast8_t state;
		uint_fast8_t is_trackable;
		uint_fast8_t visible;
		const char name[20];
		char text[20];
		PACKEDCOLOR565_T color;
		void(*onClickHandler) (void);
	} label_t;

	static label_t labels[] = {
	//     x,   y,  x2, y2,     parent,      state,     visible,     name,   Text,  color
		{ },
		{ 250, 120, WINDOW_BP,   DISABLED, 0, NON_VISIBLE, "lbl_low",  "", COLORPIP_YELLOW, },
		{ 490, 120, WINDOW_BP,   DISABLED, 0, NON_VISIBLE, "lbl_high", "", COLORPIP_YELLOW, },
		{ 100,  50, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 100,  85, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 100, 120, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 100, 155, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 100, 190, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 250,  50, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 250,  85, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 250, 120, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 250, 155, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 250, 190, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 450,  50, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 450,  85, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 450, 120, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 450, 155, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 450, 190, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 580,  60, WINDOW_ENC2, DISABLED, 0, NON_VISIBLE, "lbl_enc2_param", "", COLORPIP_WHITE, },
		{ 580,  90, WINDOW_ENC2, DISABLED, 0, NON_VISIBLE, "lbl_enc2_val", "", COLORPIP_WHITE, },
	};
	enum { labels_count = sizeof labels / sizeof labels[0] };

	typedef struct {
		uint_fast16_t last_pressed_x; 	 // последняя точка касания экрана
		uint_fast16_t last_pressed_y;
		uint_fast8_t selected_id;		 // индекс последнего выбранного элемента
		uint_fast8_t selected_type;		 // тип последнего выбранного элемента
		uint_fast8_t state;				 // последнее состояние
		uint_fast8_t is_touching_screen; // есть ли касание экрана в данный момент
		uint_fast8_t is_after_touch; 	 // есть ли касание экрана после выхода точки касания из элемента (при is_tracking == 0)
		uint_fast8_t fix;				 // первые координаты после нажатия от контролера тачскрина приходят старые, пропускаем
		uint_fast8_t window_to_draw;	 // индекс записи с описанием запрошенного к отображению окна
		uint_fast16_t pip_width;		 // параметры pip
		uint_fast16_t pip_height;
		uint_fast16_t pip_x;
		uint_fast16_t pip_y;
		uint_fast8_t is_tracking;		 // получение относительных координат точки перемещения нажатия
		int_least16_t vector_move_x;	 // в т.ч. и за границами элемента, при state == PRESSED
		int_least16_t vector_move_y;
	} gui_t;

	static gui_t gui = { 0, 0, 0, TYPE_DUMMY, CANCELLED, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, };

	typedef struct {
		uint_fast8_t window_id;			// в окне будут отображаться элементы с соответствующим полем for_window
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
		{ WINDOW_FREQ,  100,  0, 350, 200, "Freq", 		  NON_VISIBLE, 0, window_freq_process, },
		{ WINDOW_MENU,   50, 10, 600, 220, "Settings",	  NON_VISIBLE, 0, window_menu_process, },
		{ WINDOW_ENC2, 	550, 15, 735, 120, "Fast menu",   NON_VISIBLE, 0, },
	};
	enum { windows_count = sizeof windows / sizeof windows[0] };

	typedef struct {
		int_least16_t rotate;		// признак поворота второго энкодера
		uint_fast8_t press;			// короткое нажание
		uint_fast8_t hold;			// длинное нажатие
		uint_fast8_t busy;			// второй энкодер выделен для обработки данных окна
		uint_fast8_t rotate_done;	// событие поворота от энкодера обработано, можно получать новые данные
		uint_fast8_t press_done;	// событие нажатия от энкодера обработано, можно получать новые данные
	} enc2_t;

	static enc2_t encoder2 = { 0, 0, 0, 0, 1, 1, };

	enum {
		MENU_OFF,
		MENU_GROUPS,
		MENU_PARAMS,
		MENU_VALS,
		MENU_COUNT
	};

	typedef struct {
		uint_fast8_t first_id;			// первое вхождение номера метки уровня
		uint_fast8_t last_id;			// последнее вхождение номера метки уровня
		uint_fast8_t num_rows;			// число меток уровня
		uint_fast8_t count;				// число значений уровня
		int_fast8_t selected_str;		// выбранная строка уровня
		int_fast8_t selected_label;		// выбранная метка уровня
		uint_fast8_t add_id;			// номер строки уровня, отображаемой первой
		menu_names_t menu_block[30];	// массив значений уровня меню
	} menu_t;

	menu_t menu[MENU_COUNT];

	typedef struct {
		uint_fast16_t type;				// тип элемента, поддерживающего реакцию на касания
		uint_fast8_t state;				// текущее состояние элемента
		uint_fast8_t visible;			// текущая видимость элемента
		uint_fast8_t id;				// номер элемента из структуры описания
		uint_fast8_t is_trackable;		// поддерживает ли элемент возврат относительных координат перемещения точки нажатия
		uint_fast16_t x1;
		uint_fast16_t y1;
		uint_fast16_t x2;
		uint_fast16_t y2;
		LIST_ENTRY item;
	} list_template_t;

	LIST_ENTRY touch_list;
	list_template_t touch_elements[40];
	uint_fast8_t touch_list_count = 0;
	uint_fast8_t menu_label_touched = 0;
	uint_fast8_t menu_level;

	void gui_initialize (void)
	{
		pipparams_t pip;
		uint_fast8_t i = 1;
		InitializeListHead(& touch_list);
		do {
			touch_elements[touch_list_count].id = i;
			touch_elements[touch_list_count].type = TYPE_BUTTON;
			InsertHeadList(& touch_list, & touch_elements[touch_list_count].item);
			touch_list_count++;
		} while (button_handlers[++i].parent == FOOTER);
		display2_getpipparams(& pip);
		gui.pip_width = pip.w;
		gui.pip_height = pip.h;
		gui.pip_x = pip.x;
		gui.pip_y = pip.y;
	}

	void update_touch_list(void)
	{
		PLIST_ENTRY t;

		for (t = touch_list.Blink; t != & touch_list; t = t->Blink)
		{
			list_template_t * p = CONTAINING_RECORD(t, list_template_t, item);
			if (p->type == TYPE_BUTTON)
			{
				p->x1 = button_handlers[p->id].x1;
				p->x2 = button_handlers[p->id].x2;
				p->y1 = button_handlers[p->id].y1;
				p->y2 = button_handlers[p->id].y2;
				p->state = button_handlers[p->id].state;
				p->visible = button_handlers[p->id].visible;
				p->is_trackable = button_handlers[p->id].is_trackable;
			}
			else if (p->type == TYPE_LABEL)
			{
				p->x1 = labels[p->id].x;
				p->x2 = labels[p->id].x + display_colorbuff_string_width(NULL, gui.pip_width, gui.pip_height, labels[p->id].text);
				p->y1 = labels[p->id].y - 8;
				p->y2 = labels[p->id].y + display_colorbuff_string_height(NULL, gui.pip_width, gui.pip_height, labels[p->id].text) + 8;
				p->state = labels[p->id].state;
				p->visible = labels[p->id].visible;
				p->is_trackable = labels[p->id].is_trackable;
			}
		}
	}

	uint_fast8_t find_button (uint_fast8_t id_window, const char * val)				// возврат id кнопки окна по ее названию
	{
		for (uint_fast8_t i = 1; i < button_handlers_count; i++)
		{
			if (button_handlers[i].parent == id_window && strcmp(button_handlers[i].text, val) == 0)
				return i;
		}
		return 0;
	}

	uint_fast8_t find_label (uint_fast8_t id_window, const char * val)				// возврат id метки окна по ее названию
	{
		for (uint_fast8_t i = 1; i < labels_count; i++)
		{
			if (labels[i].parent == id_window && strcmp(labels[i].name, val) == 0)
				return i;
		}
		return 0;
	}

	void footer_buttons_state (uint_fast8_t state, const char * name)					// блокируются все, кроме name == text
	{
		static uint_fast8_t id = 0;
		if (state == DISABLED)
		{
			id = find_button(FOOTER, name);
			button_handlers[id].is_locked = BUTTON_LOCKED;
		} else
			button_handlers[id].is_locked = BUTTON_NON_LOCKED;

		for (uint_fast8_t i = 1; i < button_handlers_count; i++)
		{
			if (button_handlers[i].parent != FOOTER)
				break;
			button_handlers[i].state = button_handlers[i].text == name ? DISABLED : state;
		}
	}

	void set_window(uint_fast8_t parent, uint_fast8_t value)
	{
		PLIST_ENTRY p;
		for (uint_fast8_t i = 1; i < button_handlers_count; i++)
		{
			if (button_handlers[i].parent == parent)
			{
				button_handlers[i].visible = value ? VISIBLE : NON_VISIBLE;
				button_handlers[i].is_locked = 0;
				if (button_handlers[i].visible)
				{
					touch_elements[touch_list_count].id = i;
					touch_elements[touch_list_count].type = TYPE_BUTTON;
					InsertHeadList(& touch_list, & touch_elements[touch_list_count].item);
					touch_list_count++;
				}
				else
				{
					p = RemoveHeadList(& touch_list);
					touch_list_count--;
				}
			}
		}
		for (uint_fast8_t i = 1; i < labels_count; i++)
		{
			if (labels[i].parent == parent)
			{
				labels[i].visible = value ? VISIBLE : NON_VISIBLE;
				if (labels[i].visible && labels[i].onClickHandler)
				{
					touch_elements[touch_list_count].id = i;
					touch_elements[touch_list_count].type = TYPE_LABEL;
					InsertHeadList(& touch_list, & touch_elements[touch_list_count].item);
					touch_list_count++;
				}
				if(! labels[i].visible && labels[i].onClickHandler)
				{
					p = RemoveHeadList(& touch_list);
					touch_list_count--;
				}
			}
		}
		windows[parent].is_show = value ? VISIBLE : NON_VISIBLE;
		gui.window_to_draw = value ? parent : 0;
		(void) p;
	}

	void window_bp_process (void)
	{
		PACKEDCOLORPIP_T * const colorpip = getscratchpip();
		static uint_fast8_t val_high, val_low, val_c, val_w, bw_type;
		static uint_fast16_t x_h, x_l, x_c;
		char buf[10];
		static uint_fast8_t id_button_high, id_button_low, id_button_width, id_button_pitch, id_lbl_high, id_lbl_low;

		if (windows[WINDOW_BP].first_call == 1)
		{
			windows[WINDOW_BP].first_call = 0;

			id_lbl_low = find_label(WINDOW_BP, "lbl_low");
			id_lbl_high = find_label(WINDOW_BP, "lbl_high");
			id_button_high = find_button(WINDOW_BP, "High|cut");
			id_button_low = find_button(WINDOW_BP, "Low|cut");
			id_button_width = find_button(WINDOW_BP, "Width");
			id_button_pitch = find_button(WINDOW_BP, "Pitch");

			bw_type = get_bp_type();
			if (bw_type)	// BWSET_WIDE
			{
				button_handlers[id_button_high].visible = VISIBLE;
				button_handlers[id_button_low].visible = VISIBLE;
				button_handlers[id_button_width].visible = NON_VISIBLE;
				button_handlers[id_button_pitch].visible = NON_VISIBLE;
				button_handlers[id_button_high].is_locked = 1;

				val_high = get_high_bp(0);
				val_low = get_low_bp(0);

				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_high * 100);
				strcpy (labels[id_lbl_high].text, buf);
				x_h = normalize(val_high, 0, 50, 290) + 290;
				labels[id_lbl_high].x = x_h + 64 > 550 ? 486 : x_h;

				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_low * 10);
				strcpy (labels[id_lbl_low].text, buf);
				x_l = normalize(val_low, 0, 500, 290) + 290;
				labels[id_lbl_low].x = x_l - strlen(buf) * SMALLCHARW;
			}
			else			// BWSET_NARROW
			{
				button_handlers[id_button_high].visible = NON_VISIBLE;
				button_handlers[id_button_low].visible = NON_VISIBLE;
				button_handlers[id_button_width].visible = VISIBLE;
				button_handlers[id_button_pitch].visible = VISIBLE;

				button_handlers[id_button_width].is_locked = 1;

				val_c = get_high_bp(0);
				x_c = 400; //normalize(val_c, 0, 500, 290) + 290;
				val_w = get_low_bp(0) / 2;
				x_l = normalize(190 - val_w , 0, 500, 290) + 290;
				x_h = normalize(190 + val_w , 0, 500, 290) + 290;

				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_w * 20);
				labels[id_lbl_high].x = x_c - strlen(buf) * 8;
				strcpy (labels[id_lbl_high].text, buf);
				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("P %d"), val_c * 10);
				strcpy (labels[id_lbl_low].text, buf);
				labels[id_lbl_low].x = 550 - strlen(buf) * SMALLCHARW;
			}
		}

		if (encoder2.rotate != 0)
		{
			bw_type = get_bp_type();
			if (bw_type)	// BWSET_WIDE
			{
				if (button_handlers[id_button_high].is_locked == 1)
				{
					val_high = get_high_bp(encoder2.rotate);
					encoder2.rotate_done = 1;
					local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_high * 100);
					strcpy (labels[id_lbl_high].text, buf);
					x_h = normalize(val_high, 0, 50, 290) + 290;
					labels[id_lbl_high].x = x_h + 64 > 550 ? 486 : x_h;
				}
				else if (button_handlers[id_button_low].is_locked == 1)
				{
					val_low = get_low_bp(encoder2.rotate * 10);
					encoder2.rotate_done = 1;
					local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_low * 10);
					strcpy (labels[id_lbl_low].text, buf);
					x_l = normalize(val_low / 10, 0, 50, 290) + 290;
					labels[id_lbl_low].x = x_l - strlen(buf) * SMALLCHARW;
				}
			}
			else				// BWSET_NARROW
			{
				if (button_handlers[id_button_pitch].is_locked == 1)
				{
					val_c = get_high_bp(encoder2.rotate);
					val_w = get_low_bp(0) / 2;
				}
				else if (button_handlers[id_button_width].is_locked == 1)
				{
					val_c = get_high_bp(0);
					val_w = get_low_bp(encoder2.rotate) / 2;
				}
				encoder2.rotate_done = 1;
				x_c = 400;
				x_l = normalize(190 - val_w , 0, 500, 290) + 290;
				x_h = normalize(190 + val_w , 0, 500, 290) + 290;

				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_w * 20);
				labels[id_lbl_high].x = x_c - strlen(buf) * SMALLCHARW / 2;
				strcpy (labels[id_lbl_high].text, buf);
				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("P %d"), val_c * 10);
				strcpy (labels[id_lbl_low].text, buf);
				labels[id_lbl_low].x = 550 - strlen(buf) * SMALLCHARW;
			}
		}

		display_colorbuffer_line_set(colorpip, gui.pip_width, gui.pip_height, 251, 110, 549, 110, COLORPIP_GRAY);
		display_colorbuffer_line_set(colorpip, gui.pip_width, gui.pip_height, 290, 70, 290, 120, COLORPIP_GRAY);
		display_colorbuffer_rect(colorpip, gui.pip_width, gui.pip_height, x_l, 70, x_h, 108, COLORPIP_YELLOW, 1);
		if (! bw_type)
			display_colorbuffer_line_set(colorpip, gui.pip_width, gui.pip_height, x_c, 60, x_c, 120, COLORPIP_RED);
}

	void window_freq_process (void)
	{
		if (windows[WINDOW_FREQ].first_call == 1)
		{
			windows[WINDOW_FREQ].first_call = 0;
			button_handlers[find_button(WINDOW_FREQ, "OK")].is_locked = BUTTON_LOCKED;
		}
	}

	void labels_menu_handler (void)
	{
		if (gui.selected_type == TYPE_LABEL)
		{
			if(strcmp(labels[gui.selected_id].name, "lbl_group") == 0)
			{
				menu[MENU_GROUPS].selected_label = gui.selected_id - menu[MENU_GROUPS].first_id;
				menu_label_touched = 1;
				menu_level = MENU_GROUPS;
			}
			else if(strcmp(labels[gui.selected_id].name, "lbl_params") == 0)
			{
				menu[MENU_PARAMS].selected_label = gui.selected_id - menu[MENU_PARAMS].first_id;
				menu_label_touched = 1;
				menu_level = MENU_PARAMS;
			}
			else if(strcmp(labels[gui.selected_id].name, "lbl_vals") == 0)
			{
				menu[MENU_VALS].selected_label = gui.selected_id - menu[MENU_VALS].first_id;
				menu[MENU_PARAMS].selected_label = menu[MENU_VALS].selected_label;
				menu_label_touched = 1;
				menu_level = MENU_VALS;
			}
		}
	}

	void window_menu_process(void)
	{
		PACKEDCOLORPIP_T * const colorpip = getscratchpip();
		static uint_fast8_t str_step = 0, menu_is_scrolling = 0, start_str_group = 0, start_str_params = 0;
		static uint_fast8_t id_button_up = 0, id_button_down = 0, button_pressed = 0;
		uint_fast8_t button_menu_w = 40, button_menu_h = 40;

		if (windows[WINDOW_MENU].first_call == 1)
		{
			windows[WINDOW_MENU].first_call = 0;
			set_menu_cond(VISIBLE);

			id_button_up = find_button(WINDOW_MENU, "+");
			id_button_down = find_button(WINDOW_MENU, "-");
			button_handlers[id_button_up].visible = NON_VISIBLE;
			button_handlers[id_button_down].visible = NON_VISIBLE;

			menu[MENU_GROUPS].add_id = 0;
			menu[MENU_GROUPS].selected_str = 0;
			menu[MENU_GROUPS].selected_label = 0;
			menu[MENU_PARAMS].add_id = 0;
			menu[MENU_PARAMS].selected_str = 0;
			menu[MENU_PARAMS].selected_label = 0;
			menu[MENU_VALS].add_id = 0;
			menu[MENU_VALS].selected_str = 0;
			menu[MENU_VALS].selected_label = 0;

			menu[MENU_GROUPS].first_id = 1;
			while (labels[++menu[MENU_GROUPS].first_id].parent != WINDOW_MENU);				// первое вхождение метки group
			menu[MENU_GROUPS].last_id = menu[MENU_GROUPS].first_id;
			while (strcmp(labels[++menu[MENU_GROUPS].last_id].name, "lbl_group") == 0);
			menu[MENU_GROUPS].last_id--;													// последнее вхождение метки group
			menu[MENU_GROUPS].num_rows = menu[MENU_GROUPS].last_id - menu[MENU_GROUPS].first_id;

			menu[MENU_PARAMS].first_id = menu[MENU_GROUPS].last_id + 1;						// первое вхождение метки params
			menu[MENU_PARAMS].last_id = menu[MENU_PARAMS].first_id;
			while (strcmp(labels[++menu[MENU_PARAMS].last_id].name, "lbl_params") == 0);
			menu[MENU_PARAMS].last_id--;													// последнее вхождение метки params
			menu[MENU_PARAMS].num_rows = menu[MENU_PARAMS].last_id - menu[MENU_PARAMS].first_id;

			menu[MENU_VALS].first_id = menu[MENU_PARAMS].last_id + 1;						// первое вхождение метки vals
			menu[MENU_VALS].last_id = menu[MENU_VALS].first_id;
			while (strcmp(labels[++menu[MENU_VALS].last_id].name, "lbl_vals") == 0);
			menu[MENU_VALS].last_id--;														// последнее вхождение метки vals
			menu[MENU_VALS].num_rows = menu[MENU_VALS].last_id - menu[MENU_VALS].first_id;

			menu[MENU_GROUPS].count = get_multilinemenu_block_groups(menu[MENU_GROUPS].menu_block) - 1;
			for(uint_fast8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
			{
				strcpy(labels[menu[MENU_GROUPS].first_id + i].text, menu[MENU_GROUPS].menu_block[i + menu[MENU_GROUPS].add_id].name);
				labels[menu[MENU_GROUPS].first_id + i].visible = VISIBLE;
				labels[menu[MENU_GROUPS].first_id + i].color = COLORPIP_WHITE;
			}

			menu[MENU_PARAMS].count = get_multilinemenu_block_params(menu[MENU_PARAMS].menu_block, menu[MENU_GROUPS].menu_block[menu[MENU_GROUPS].selected_str].index) - 1;
			for(uint_fast8_t i = 0; i <= menu[MENU_PARAMS].num_rows; i++)
			{
				labels[menu[MENU_PARAMS].first_id + i].visible = NON_VISIBLE;
				labels[menu[MENU_PARAMS].first_id + i].color = COLORPIP_WHITE;
				if (menu[MENU_PARAMS].count < i)
					continue;
				strcpy(labels[menu[MENU_PARAMS].first_id + i].text, menu[MENU_PARAMS].menu_block[i + menu[MENU_PARAMS].add_id].name);
				labels[menu[MENU_PARAMS].first_id + i].visible = VISIBLE;
			}

			menu[MENU_PARAMS].selected_str = 0;
			menu[MENU_VALS].count = menu[MENU_PARAMS].count < menu[MENU_VALS].num_rows ? menu[MENU_PARAMS].count :  menu[MENU_VALS].num_rows;
			get_multilinemenu_block_vals(menu[MENU_VALS].menu_block, menu[MENU_PARAMS].menu_block[menu[MENU_PARAMS].selected_str].index, menu[MENU_VALS].count);
			for(uint_fast8_t i = 0; i <= menu[MENU_VALS].num_rows; i++)
			{
				labels[menu[MENU_VALS].first_id + i].visible = NON_VISIBLE;
				labels[menu[MENU_VALS].first_id + i].color = COLORPIP_WHITE;
				if (menu[MENU_VALS].count < i)
					continue;
				strcpy(labels[menu[MENU_VALS].first_id + i].text, menu[MENU_VALS].menu_block[i + menu[MENU_VALS].add_id].name);
				labels[menu[MENU_VALS].first_id + i].visible = VISIBLE;
			};
			str_step = labels[menu[MENU_GROUPS].first_id + 1].y - labels[menu[MENU_GROUPS].first_id].y;
			menu_level = MENU_GROUPS;
		}

		if(gui.is_tracking && gui.selected_type == TYPE_LABEL && gui.vector_move_y != 0)
		{
			if (! menu_is_scrolling)
			{
				start_str_group = menu[MENU_GROUPS].add_id;
				start_str_params = menu[MENU_PARAMS].add_id;
			}
			ldiv_t r = ldiv(gui.vector_move_y, str_step);
			if(strcmp(labels[gui.selected_id].name, "lbl_group") == 0)
			{
				int_fast8_t q = start_str_group - r.quot;
				menu[MENU_GROUPS].add_id = q <= 0 ? 0 : q;
				menu[MENU_GROUPS].add_id = (menu[MENU_GROUPS].add_id + menu[MENU_GROUPS].num_rows) > menu[MENU_GROUPS].count ?
						(menu[MENU_GROUPS].count - menu[MENU_GROUPS].num_rows) : menu[MENU_GROUPS].add_id;
				menu[MENU_GROUPS].selected_str = menu[MENU_GROUPS].selected_label + menu[MENU_GROUPS].add_id;
				menu_level = MENU_GROUPS;
				menu[MENU_PARAMS].add_id = 0;
				menu[MENU_PARAMS].selected_str = 0;
				menu[MENU_PARAMS].selected_label = 0;
				menu[MENU_VALS].add_id = 0;
				menu[MENU_VALS].selected_str = 0;
				menu[MENU_VALS].selected_label = 0;
			}
			else if(strcmp(labels[gui.selected_id].name, "lbl_params") == 0 &&
					menu[MENU_PARAMS].count > menu[MENU_PARAMS].num_rows)
			{
				int_fast8_t q = start_str_params - r.quot;
				menu[MENU_PARAMS].add_id = q <= 0 ? 0 : q;
				menu[MENU_PARAMS].add_id = (menu[MENU_PARAMS].add_id + menu[MENU_PARAMS].num_rows) > menu[MENU_PARAMS].count ?
						(menu[MENU_PARAMS].count - menu[MENU_PARAMS].num_rows) : menu[MENU_PARAMS].add_id;
				menu[MENU_PARAMS].selected_str = menu[MENU_PARAMS].selected_label + menu[MENU_PARAMS].add_id;
				menu[MENU_VALS].add_id = menu[MENU_PARAMS].add_id;
				menu[MENU_VALS].selected_str = menu[MENU_PARAMS].selected_str;
				menu[MENU_VALS].selected_label = menu[MENU_PARAMS].selected_label;
				menu_level = MENU_PARAMS;
			}
			menu_is_scrolling = 1;
		}

		if(! gui.is_tracking && menu_is_scrolling)
		{
			menu_is_scrolling = 0;
			gui.vector_move_y = 0;
			gui.vector_move_x = 0;
		}

		if (! encoder2.press_done || menu_label_touched || menu_is_scrolling)
		{
			// выход из режима редактирования параметра  - краткое или длинное нажатие на энкодер
			if (encoder2.press && menu_level == MENU_VALS)
			{
				menu_level = MENU_PARAMS;
				encoder2.press = 0;
			}
			if (encoder2.press)
				menu_level = ++menu_level > MENU_VALS ? MENU_VALS : menu_level;
			if (encoder2.hold)
			{
				menu_level = --menu_level == MENU_OFF ? MENU_OFF : menu_level;
				if (menu_level == MENU_GROUPS)
				{
					menu[MENU_PARAMS].add_id = 0;
					menu[MENU_PARAMS].selected_str = 0;
					menu[MENU_PARAMS].selected_label = 0;
					menu[MENU_VALS].add_id = 0;
					menu[MENU_VALS].selected_str = 0;
					menu[MENU_VALS].selected_label = 0;
				}
			}

			// при переходе на следующий уровень пункт меню подсвечивается
			if (menu_level == MENU_VALS)
			{
				uint_fast8_t id_sel_label = menu[MENU_VALS].first_id + menu[MENU_VALS].selected_label;

				button_handlers[id_button_down].visible = VISIBLE;
				button_handlers[id_button_down].x1 = labels[id_sel_label].x - button_menu_w - 10;
				button_handlers[id_button_down].x2 = button_handlers[id_button_down].x1 + button_menu_w;
				button_handlers[id_button_down].y1 = (labels[id_sel_label].y + SMALLCHARH / 2) - (button_menu_h / 2);
				button_handlers[id_button_down].y2 = button_handlers[id_button_down].y1 + button_menu_h;

				button_handlers[id_button_up].visible = VISIBLE;
				button_handlers[id_button_up].x1 = labels[id_sel_label].x + strlen(labels[id_sel_label].text) * SMALLCHARW + 10;
				button_handlers[id_button_up].x2 = button_handlers[id_button_up].x1 + button_menu_w;
				button_handlers[id_button_up].y1 = button_handlers[id_button_down].y1;
				button_handlers[id_button_up].y2 = button_handlers[id_button_down].y2;
				for (uint8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
				{
					labels[i + menu[MENU_GROUPS].first_id].color = i == menu[MENU_GROUPS].selected_label ? COLORPIP_YELLOW : COLORPIP_DARKGRAY;
					labels[i + menu[MENU_PARAMS].first_id].color = i == menu[MENU_PARAMS].selected_label ? COLORPIP_YELLOW : COLORPIP_DARKGRAY;
					labels[i + menu[MENU_VALS].first_id].color = i == menu[MENU_PARAMS].selected_label ? COLORPIP_YELLOW : COLORPIP_DARKGRAY;
				}
				menu_label_touched = 0;
			}
			if (menu_level == MENU_PARAMS)
			{
				button_handlers[id_button_down].visible = NON_VISIBLE;
				button_handlers[id_button_up].visible = NON_VISIBLE;
				for (uint8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
				{
					labels[i + menu[MENU_GROUPS].first_id].color = i == menu[MENU_GROUPS].selected_label ? COLORPIP_YELLOW : COLORPIP_DARKGRAY;
					labels[i + menu[MENU_PARAMS].first_id].color = COLORPIP_WHITE;
					labels[i + menu[MENU_VALS].first_id].color = COLORPIP_WHITE;
				}
			}
			if (menu_level == MENU_GROUPS)
			{
				button_handlers[id_button_down].visible = NON_VISIBLE;
				button_handlers[id_button_up].visible = NON_VISIBLE;
				for (uint8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
				{
					labels[i + menu[MENU_GROUPS].first_id].color = COLORPIP_WHITE;
					labels[i + menu[MENU_PARAMS].first_id].color = COLORPIP_WHITE;
					labels[i + menu[MENU_VALS].first_id].color = COLORPIP_WHITE;
				}
			}

			encoder2.press = 0;
			encoder2.hold = 0;
			encoder2.press_done = 1;
		}

		if (menu_level == MENU_OFF)
		{
			set_window(WINDOW_MENU, NON_VISIBLE);
			encoder2.busy = 0;
			footer_buttons_state(CANCELLED, "");
			set_menu_cond(NON_VISIBLE);
			return;
		}

		if (encoder2.rotate != 0 && menu_level == MENU_VALS)
		{
			encoder2.rotate_done = 1;
			menu[MENU_PARAMS].selected_str = menu[MENU_PARAMS].selected_label + menu[MENU_PARAMS].add_id;
			strcpy(labels[menu[MENU_VALS].first_id + menu[MENU_PARAMS].selected_label].text,
					gui_edit_menu_item(menu[MENU_PARAMS].menu_block[menu[MENU_PARAMS].selected_str].index, encoder2.rotate));
		}

		if ((menu_label_touched || menu_is_scrolling || encoder2.rotate != 0) && menu_level != MENU_VALS)
		{
			encoder2.rotate_done = 1;

			if (encoder2.rotate != 0)
			{
				menu[menu_level].selected_str = (menu[menu_level].selected_str + encoder2.rotate) <= 0 ? 0 : menu[menu_level].selected_str + encoder2.rotate;
				menu[menu_level].selected_str = menu[menu_level].selected_str > menu[menu_level].count ? menu[menu_level].count : menu[menu_level].selected_str;
			}
			else if (menu_label_touched)
				menu[menu_level].selected_str = menu[menu_level].selected_label + menu[menu_level].add_id;

			menu[MENU_PARAMS].count = get_multilinemenu_block_params(menu[MENU_PARAMS].menu_block, menu[MENU_GROUPS].menu_block[menu[MENU_GROUPS].selected_str].index) - 1;

			if (encoder2.rotate > 0)
			{
				// указатель подошел к нижней границе списка
				if (++menu[menu_level].selected_label > (menu[menu_level].count < menu[menu_level].num_rows ? menu[menu_level].count : menu[menu_level].num_rows))
				{
					menu[menu_level].selected_label = (menu[menu_level].count < menu[menu_level].num_rows ? menu[menu_level].count : menu[menu_level].num_rows);
					menu[menu_level].add_id = menu[menu_level].selected_str - menu[menu_level].selected_label;
				}
			}
			if (encoder2.rotate < 0)
			{
				// указатель подошел к верхней границе списка
				if (--menu[menu_level].selected_label < 0)
				{
					menu[menu_level].selected_label = 0;
					menu[menu_level].add_id = menu[menu_level].selected_str;
				}
			}

			if (menu_level == MENU_GROUPS)
				for(uint_fast8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
					strcpy(labels[menu[MENU_GROUPS].first_id + i].text, menu[MENU_GROUPS].menu_block[i + menu[MENU_GROUPS].add_id].name);

			menu[MENU_VALS].count = menu[MENU_PARAMS].count < menu[MENU_VALS].num_rows ? menu[MENU_PARAMS].count : menu[MENU_VALS].num_rows;
			get_multilinemenu_block_vals(menu[MENU_VALS].menu_block,  menu[MENU_PARAMS].menu_block[menu[MENU_PARAMS].add_id].index, menu[MENU_VALS].count);

			for(uint_fast8_t i = 0; i <= menu[MENU_PARAMS].num_rows; i++)
			{
				labels[menu[MENU_PARAMS].first_id + i].visible = NON_VISIBLE;
				labels[menu[MENU_PARAMS].first_id + i].state = DISABLED;
				labels[menu[MENU_VALS].first_id + i].visible = NON_VISIBLE;
				labels[menu[MENU_VALS].first_id + i].state = DISABLED;
				if (i > menu[MENU_PARAMS].count)
					continue;
				strcpy(labels[menu[MENU_PARAMS].first_id + i].text, menu[MENU_PARAMS].menu_block[i + menu[MENU_PARAMS].add_id].name);
				strcpy(labels[menu[MENU_VALS].first_id + i].text,   menu[MENU_VALS].menu_block[i].name);
				labels[menu[MENU_PARAMS].first_id + i].visible = VISIBLE;
				labels[menu[MENU_PARAMS].first_id + i].state = CANCELLED;
				labels[menu[MENU_VALS].first_id + i].visible = VISIBLE;
				labels[menu[MENU_VALS].first_id + i].state = CANCELLED;
			}
			menu_label_touched = 0;
			PRINTF("%d %s %d\n", menu[menu_level].selected_str, menu[menu_level].menu_block[menu[menu_level].selected_str].name, menu[menu_level].selected_label);
		}
		if (menu_level != MENU_VALS)
			display_colorbuff_string_tbg(colorpip, gui.pip_width, gui.pip_height, labels[menu[menu_level].selected_label + menu[menu_level].first_id].x - 16,
										 labels[menu[menu_level].selected_label + menu[menu_level].first_id].y, ">", COLORPIP_GREEN);
	}

	void buttons_menu_handler(void)
	{
		if (gui.selected_type == TYPE_BUTTON && gui.selected_id == find_button(WINDOW_MENU, "+"))
			encoder2.rotate = 1;
		else if (gui.selected_type == TYPE_BUTTON && gui.selected_id == find_button(WINDOW_MENU, "-"))
			encoder2.rotate = -1;
	}

	uint_fast8_t check_encoder2 (int_least16_t rotate)
	{
		if (encoder2.rotate_done || encoder2.rotate == 0)
		{
			encoder2.rotate = rotate;
			encoder2.rotate_done = 0;
		}
		return encoder2.busy;
	}

	void set_encoder2_state (uint_fast8_t code)
	{
		if (code == KBD_ENC2_PRESS)
			encoder2.press = 1;
		if (code == KBD_ENC2_HOLD)
			encoder2.hold = 1;
		encoder2.press_done = 0;
	}

	void remove_end_line_spaces(char * str)
	{
		uint_fast8_t i;
		char c[1];
		for(i = strlen(str) - 1; i > 0; i--)
		{
			strncpy(c, & str[i], 1);
			if(strcmp(c, " ") != 0)
				break;
		}
		str[i + 1] = 0;
	}

	void encoder2_menu (enc2_menu_t * enc2_menu)
	{
		uint_fast8_t id_lbl_param = find_label(WINDOW_ENC2, "lbl_enc2_param");
		uint_fast8_t id_lbl_val = find_label(WINDOW_ENC2, "lbl_enc2_val");
		gui.window_to_draw = WINDOW_ENC2;
		set_window(gui.window_to_draw, enc2_menu->state != 0);
		if (windows[gui.window_to_draw].is_show == VISIBLE)
		{
			footer_buttons_state(DISABLED, "");
			strcpy(labels[id_lbl_param].text, enc2_menu->param);
			remove_end_line_spaces(labels[id_lbl_param].text);
			PRINTF("%d\n", strlen(labels[id_lbl_param].text));
			strcpy(labels[id_lbl_val].text, enc2_menu->val);
			labels[id_lbl_val].color = enc2_menu->state == 2 ? COLORPIP_YELLOW : COLORPIP_WHITE;
			labels[id_lbl_val].x = windows[WINDOW_ENC2].x1 + ((windows[WINDOW_ENC2].x2 - windows[WINDOW_ENC2].x1) - (strlen (labels[id_lbl_val].text) * SMALLCHARW)) / 2;
			labels[id_lbl_param].x = windows[WINDOW_ENC2].x1 + ((windows[WINDOW_ENC2].x2 - windows[WINDOW_ENC2].x1) - (strlen (labels[id_lbl_param].text) * SMALLCHARW)) / 2;
		} else
			footer_buttons_state(CANCELLED, "");
	}

	void buttons_mode_handler(void)
	{
		if (windows[WINDOW_MODES].is_show && button_handlers[gui.selected_id].parent == WINDOW_MODES)
		{
			if (button_handlers[gui.selected_id].payload != UINTPTR_MAX)
				change_submode(button_handlers[gui.selected_id].payload);

			set_window(WINDOW_MODES, NON_VISIBLE);
			footer_buttons_state(CANCELLED, "");
		}
	}

	void buttons_bp_handler (void) // переделать
	{
		if (gui.selected_id == find_button(WINDOW_BP, "Low|cut"))
		{
			button_handlers[find_button(WINDOW_BP, "High cut")].is_locked = 0;
			button_handlers[find_button(WINDOW_BP, "Low cut")].is_locked = 1;
		}
		else if (gui.selected_id == find_button(WINDOW_BP, "High|cut"))
		{
			button_handlers[find_button(WINDOW_BP, "High|cut")].is_locked = 1;
			button_handlers[find_button(WINDOW_BP, "Low|cut")].is_locked = 0;
		}
		else if (gui.selected_id == find_button(WINDOW_BP, "Width"))
		{
			button_handlers[find_button(WINDOW_BP, "Width")].is_locked = 1;
			button_handlers[find_button(WINDOW_BP, "Pitch")].is_locked = 0;
		}
		else if (gui.selected_id == find_button(WINDOW_BP, "Pitch"))
		{
			button_handlers[find_button(WINDOW_BP, "Width")].is_locked = 0;
			button_handlers[find_button(WINDOW_BP, "Pitch")].is_locked = 1;
		}
		else if (gui.selected_id == find_button(WINDOW_BP, "OK"))
		{
			set_window(WINDOW_BP, NON_VISIBLE);
			encoder2.busy = 0;
			footer_buttons_state(CANCELLED, "");
		}
	}

	void buttons_freq_handler (void)
	{
		uint_fast8_t editfreqmode = 0;
		if (gui.window_to_draw == WINDOW_FREQ)
			editfreqmode = send_key_code(button_handlers[gui.selected_id].payload);
		if (editfreqmode == 0)
		{
			set_window(WINDOW_FREQ, NON_VISIBLE);
			footer_buttons_state(CANCELLED, "");
			enable_keyboard();
		}
	}

	void button1_handler(void)
	{
		gui.window_to_draw = WINDOW_MODES;

		if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
		{
			set_window(gui.window_to_draw, VISIBLE);
			windows[gui.window_to_draw].first_call = 1;
			footer_buttons_state(DISABLED, button_handlers[gui.selected_id].text);
		}
		else
		{
			set_window(gui.window_to_draw, NON_VISIBLE);
			footer_buttons_state(CANCELLED, "");
		}
	}

	void button2_handler(void)
	{
		gui.window_to_draw = WINDOW_BP;

		if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
		{
			encoder2.busy = 1;
			set_window(gui.window_to_draw, VISIBLE);
			windows[gui.window_to_draw].first_call = 1;
			footer_buttons_state(DISABLED, button_handlers[gui.selected_id].text);
		}
		else
		{
			set_window(gui.window_to_draw, NON_VISIBLE);
			encoder2.busy = 0;
			footer_buttons_state(CANCELLED, "");
		}
	}

	void button3_handler(void)
	{
		gui.window_to_draw = WINDOW_AGC;

		if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
		{
			set_window(gui.window_to_draw, VISIBLE);
			windows[gui.window_to_draw].first_call = 1;
			footer_buttons_state(DISABLED, button_handlers[gui.selected_id].text);
		}
		else
		{
			set_window(gui.window_to_draw, NON_VISIBLE);
			footer_buttons_state(CANCELLED, "");
		}
	}

	void button4_handler(void)
	{
		gui.window_to_draw = WINDOW_FREQ;

		if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
		{
			set_window(gui.window_to_draw, VISIBLE);
			windows[gui.window_to_draw].first_call = 1;
			send_key_code(KBD_CODE_ENTERFREQ);
			disable_keyboard();
			footer_buttons_state(DISABLED, button_handlers[gui.selected_id].text);
		}
		else
		{
			set_window(gui.window_to_draw, NON_VISIBLE);
			send_key_code(KBD_CODE_ENTERFREQDONE);
			enable_keyboard();
			footer_buttons_state(CANCELLED, "");
		}
	}

	void button5_handler(void)
	{

	}

	void button6_handler(void)
	{

	}

	void button7_handler(void)
	{

	}

	void button8_handler(void)
	{

	}

	void button9_handler(void)
	{
		gui.window_to_draw = WINDOW_MENU;

		if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
		{
			set_window(gui.window_to_draw, VISIBLE);
			windows[gui.window_to_draw].first_call = 1;
			footer_buttons_state(DISABLED, button_handlers[gui.selected_id].text);
			encoder2.busy = 1;
		}
		else
		{
			set_window(gui.window_to_draw, NON_VISIBLE);
			footer_buttons_state(CANCELLED, "");
			encoder2.busy = 0;
			set_menu_cond(NON_VISIBLE);
		}
	}

	/* Кнопка без надписей */
	void draw_button_pip(
		uint_fast16_t x1, uint_fast16_t y1,
		uint_fast16_t x2, uint_fast16_t y2,
		uint_fast8_t pressed, uint_fast8_t is_locked, uint_fast8_t is_disabled) // pressed = 0
	{
		PACKEDCOLORPIP_T * const colorpip = getscratchpip();
		PACKEDCOLOR565_T c1, c2;
		c1 = is_disabled ? COLOR_BUTTON_DISABLED : (is_locked ? COLOR_BUTTON_LOCKED : COLOR_BUTTON_NON_LOCKED);
		c2 = is_disabled ? COLOR_BUTTON_DISABLED : (is_locked ? COLOR_BUTTON_PR_LOCKED : COLOR_BUTTON_PR_NON_LOCKED);
		display_colorbuffer_rect(colorpip, gui.pip_width, gui.pip_height, x1,	y1, x2, y2, pressed ? c1 : c2, 1);
		display_colorbuffer_rect(colorpip, gui.pip_width, gui.pip_height, x1,	y1, x2, y2, COLORPIP_GRAY, 0);
		display_colorbuffer_rect(colorpip, gui.pip_width, gui.pip_height, x1 + 2, y1 + 2, x2 - 2, y2 - 2, COLORPIP_BLACK, 0);
	}

	void display_pip_update(uint_fast8_t x, uint_fast8_t y, void * pv)
	{
		PACKEDCOLORPIP_T * const colorpip = getscratchpip();
		uint_fast16_t yt, xt;
		uint_fast8_t alpha = 10; // на сколько затемнять цвета
		char buff [30];
		char * text2 = NULL;
		uint_fast8_t str_len = 0;

		// вывод на PIP служебной информации
	#if WITHTHERMOLEVEL
		if (hamradio_get_tx())
		{
			int_fast16_t temp = hamradio_get_temperature_value();
			str_len += local_snprintf_P(&buff[str_len], sizeof buff / sizeof buff [0] - str_len, PSTR("%d.%dC "),
					temp / 10, temp % 10);
		}
	#endif /* WITHTHERMOLEVEL */
	#if WITHCURRLEVEL	// ток PA (при передаче)
		if (hamradio_get_tx())
		{
			int_fast16_t drain = hamradio_get_pacurrent_value();
			if (drain < 0) drain = 0;
			str_len += local_snprintf_P(&buff[str_len], sizeof buff / sizeof buff [0] - str_len, PSTR("%d.%02dA "),
					drain / 100, drain % 100);
		}
	#endif /* WITHCURRLEVEL */
	#if WITHVOLTLEVEL	// напряжение питания
		str_len += local_snprintf_P(&buff[str_len], sizeof buff / sizeof buff [0] - str_len,
									PSTR("%d.%1dV "), hamradio_get_volt_value() / 10, hamradio_get_volt_value() % 10);
	#endif /* WITHVOLTLEVEL */
	#if WITHIF4DSP						// ширина панорамы
		str_len += local_snprintf_P(&buff[str_len], sizeof buff / sizeof buff [0] - str_len, PSTR("SPAN:%3dk"),
				(int) ((display_zoomedbw() + 0) / 1000));
	#endif /* WITHIF4DSP */
		xt = gui.pip_width - 10 - str_len * 10;
		pip_transparency_rect(colorpip, gui.pip_width, gui.pip_height, xt - 5, 225, gui.pip_width - 5, 248, alpha);
		display_colorbuff_string2_tbg(colorpip, gui.pip_width, gui.pip_height, xt, 230, buff, COLORPIP_YELLOW);

	#if defined (RTC1_TYPE)				// текущее время
		uint_fast16_t year;
		uint_fast8_t month, day, hour, minute, secounds;
		str_len = 0;
		board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);
		str_len += local_snprintf_P(&buff[str_len], sizeof buff / sizeof buff [0] - str_len,
				PSTR("%02d.%02d.%04d %02d%c%02d"), day, month, year, hour, ((secounds & 1) ? ' ' : ':'), minute);
		pip_transparency_rect(colorpip, gui.pip_width, gui.pip_height, 5, 225, str_len * 10 + 15, 248, alpha);
		display_colorbuff_string2_tbg(colorpip, gui.pip_width, gui.pip_height, 10, 230, buff, COLORPIP_YELLOW);
	#endif 	/* defined (RTC1_TYPE) */

		if (windows[gui.window_to_draw].is_show)
		{
			pip_transparency_rect(colorpip, gui.pip_width, gui.pip_height,
					windows[gui.window_to_draw].x1, windows[gui.window_to_draw].y1,
					windows[gui.window_to_draw].x2, windows[gui.window_to_draw].y2, alpha);

			if (windows[gui.window_to_draw].onVisibleProcess != 0)							// запуск процедуры фоновой обработки
				windows[gui.window_to_draw].onVisibleProcess();								// для окна, если есть

			// вывод заголовка окна
			display_colorbuff_string_tbg(colorpip, gui.pip_width, gui.pip_height, windows[gui.window_to_draw].x1 + 20,
					windows[gui.window_to_draw].y1 + 10, windows[gui.window_to_draw].title, COLORPIP_YELLOW);
			// отрисовка принадлежащих окну элементов

			for (uint_fast8_t i = 1; i < labels_count; i++)
			{
				if (labels[i].parent == gui.window_to_draw && labels[i].visible == VISIBLE)	// метки
					display_colorbuff_string_tbg(colorpip, gui.pip_width, gui.pip_height, labels[i].x, labels[i].y,
							labels[i].text, labels[i].color);
			}
		}
		for (uint_fast8_t i = 1; i < button_handlers_count; i++)
		{
			const button_t * const bh = & button_handlers[i];

			if ((bh->parent == gui.window_to_draw && bh->visible == VISIBLE && windows[gui.window_to_draw].is_show)
					|| bh->parent == FOOTER)									// кнопки
			{
				/* Кнопка без надписей (фон для надписей) */
				draw_button_pip(bh->x1, bh->y1, bh->x2, bh->y2,
						bh->state, bh->is_locked, bh->state == DISABLED ? 1 : 0);

				if (strchr(bh->text, '|') == NULL)
				{
					/* Однострочная надпись */
					display_colorbuff_string2_tbg(colorpip, gui.pip_width, gui.pip_height,
							bh->x1 + ((bh->x2 - bh->x1) - (strlen (bh->text) * SMALLCHARW2)) / 2,
							bh->y1 + ((bh->y2 - bh->y1) - SMALLCHARH2) / 2,
							bh->text, COLORPIP_BLACK);
				} else
				{
					/* Двухстрочная надпись */
					static const char delimeters [] = "|";
					uint_fast8_t j = (bh->y2 - bh->y1 - SMALLCHARH2 * 2) / 2;

					strcpy(buff, bh->text);
					text2 = strtok(buff, delimeters);
					display_colorbuff_string2_tbg(colorpip, gui.pip_width, gui.pip_height, bh->x1 +
							((bh->x2 - bh->x1) - (strlen (text2) * SMALLCHARW2)) / 2,
							bh->y1 + j, text2, COLORPIP_BLACK);

					text2 = strtok(NULL, delimeters);
					display_colorbuff_string2_tbg(colorpip, gui.pip_width, gui.pip_height, bh->x1 +
							((bh->x2 - bh->x1) - (strlen (text2) * SMALLCHARW2)) / 2,
							bh->y2 - SMALLCHARH2 - j, text2, COLORPIP_BLACK);
				}
			}
		}
	}

	void set_state_record(list_template_t * val)
	{
		gui.selected_id = val->id;								// добавить везде проверку на gui.selected_type
		switch (val->type)
		{
			case TYPE_BUTTON:
				gui.selected_type = TYPE_BUTTON;
				button_handlers[val->id].state = val->state;
				if (button_handlers[val->id].onClickHandler && button_handlers[val->id].state == RELEASED)
					button_handlers[val->id].onClickHandler();
				break;

			case TYPE_LABEL:
				gui.selected_type = TYPE_LABEL;
				labels[val->id].state = val->state;
				if (labels[val->id].onClickHandler && labels[val->id].state == RELEASED)
					labels[val->id].onClickHandler();
				break;
		}
	}

	void process_gui(void)
	{
		uint_fast16_t tx, ty;
		static uint_fast16_t x_old = 0, y_old = 0;
		static list_template_t * p = NULL;

#if defined (TSC1_TYPE)

		if (board_tsc_is_pressed())
		{
			board_tsc_getxy(& tx, & ty);
			if (gui.fix && ty > gui.pip_y)			// первые координаты после нажатия от контролера тачскрина приходят старые, пропускаем
			{
				gui.last_pressed_x = tx;
				gui.last_pressed_y = ty - gui.pip_y;
				gui.is_touching_screen = 1;
				update_touch_list();
				debug_printf_P(PSTR("pip x: %d, pip y: %d\n"), gui.last_pressed_x, gui.last_pressed_y);
			}
			gui.fix = 1;
		}
		else
#endif /* defined (TSC1_TYPE) */
		{
			gui.is_touching_screen = 0;
			gui.is_after_touch = 0;
			gui.fix = 0;
		}

		if (gui.state == CANCELLED && gui.is_touching_screen && ! gui.is_after_touch)
		{
			PLIST_ENTRY t;
			for (t = touch_list.Blink; t != & touch_list; t = t->Blink)
			{
				p = CONTAINING_RECORD(t, list_template_t, item);

				if (p->x1 < gui.last_pressed_x && p->x2 > gui.last_pressed_x
				 && p->y1 < gui.last_pressed_y && p->y2 > gui.last_pressed_y
				 && p->state != DISABLED && p->visible == VISIBLE)
				{
					gui.state = PRESSED;
					break;
				}
			}
		}

		if (gui.is_tracking && ! gui.is_touching_screen)
		{
			gui.is_tracking = 0;
			gui.vector_move_x = 0;
			gui.vector_move_y = 0;
			x_old = 0;
			y_old = 0;
		}

		if (gui.state == PRESSED)
		{
			ASSERT(p != NULL);
			if (p->is_trackable && gui.is_touching_screen)
			{
				gui.vector_move_x = x_old ? gui.vector_move_x + gui.last_pressed_x - x_old : 0; // т.к. process_gui и display_pip_update
				gui.vector_move_y = y_old ? gui.vector_move_y + gui.last_pressed_y - y_old : 0; // вызываются с разной частотой, необходимо
				p->state = PRESSED;																// накопление вектора перемещения точки
				set_state_record(p);
				if (gui.vector_move_x != 0 || gui.vector_move_y != 0)
				{
					gui.is_tracking = 1;
//					debug_printf_P(PSTR("move x: %d, move y: %d\n"), gui.vector_move_x, gui.vector_move_y);
				}
				x_old = gui.last_pressed_x;
				y_old = gui.last_pressed_y;
			}
			else if (p->x1 < gui.last_pressed_x && p->x2 > gui.last_pressed_x
			 && p->y1 < gui.last_pressed_y && p->y2 > gui.last_pressed_y && ! gui.is_after_touch)
			{
				if (gui.is_touching_screen)
				{
					p->state = PRESSED;
					set_state_record(p);
				}
				else
					gui.state = RELEASED;
			}
			else
			{
				gui.state = CANCELLED;
				p->state = CANCELLED;
				set_state_record(p);
				gui.is_after_touch = 1; 	// точка непрерывного нажатия вышла за пределы выбранного элемента, не поддерживающего tracking
			}
		}
		if (gui.state == RELEASED)
		{
			p->state = RELEASED;			// для запуска обработчика нажатия
			set_state_record(p);
			p->state = CANCELLED;
			set_state_record(p);
			gui.is_after_touch = 0;
			gui.state = CANCELLED;
			gui.is_tracking = 0;
		}
	}
#endif /* WITHTOUCHGUI */

#endif /* LCDMODE_LTDC */
