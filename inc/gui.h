#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

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
	void window_test_process (void);

	enum {								// button_handler.type
		TYPE_FOOTER_BUTTON,				// группа постоянных кнопок внизу экрана
		TYPE_PIP_BUTTON					// динамически рисуемая кнопка на PIP без фиксированных координат
	};

	enum {								// button_handler.state
		BUTTON_PRESSED,					// нажато
		BUTTON_RELEASED,				// отпущено после нажатия внутри элемента
		BUTTON_CANCELLED				// первоначальное состояние или отпущено после нажатия вне элемента
	};

	enum {								// button_handler.visible & windowpip.is_show
		NON_VISIBLE,					// parent window закрыто, кнопка не отрисовывается
		VISIBLE							// parent window на экране, кнопка отрисовывается
	};

	enum {								// button_handler.for_window & windowpip.window_id
		FOOTER,							// заглушка для заполнения структуры для кнопок внизу экрана
		WINDOW_MODES,					// переключение режимов работы, видов модуляции
		WINDOW_TEST
	};

	typedef struct {
		uint_fast16_t x1;				// для TYPE_FOOTER_BUTTON координаты от начала экрана,
		uint_fast16_t y1;				// для TYPE_PIP_BUTTON координаты от начала PIP
		uint_fast16_t x2;
		uint_fast16_t y2;
		void(*onClickHandler) (void);	// обработчик события RELEASED
		uint_fast8_t state;				// текущее состояние кнопки
		uint_fast8_t need_redraw;		// запрос на перерисовку после изменения состояния
		uint_fast8_t type;				// тип кнопки - постоянная или динамическая
		uint_fast8_t parent;			// индекс окна, в котором будет отображаться кнопка при type = TYPE_PIP_BUTTON
		uint_fast8_t visible;			// рисовать ли кнопку на экране
		uintptr_t payload;
		char * text;					// текст внутри кнопки
	} button_handler;

	typedef struct {
		uint_fast16_t x;
		uint_fast16_t y;
		uint_fast8_t parent;
		uint_fast8_t visible;
		char * value;
		COLOR565_T color;
	} label_handler;

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
	} element1_t;

	typedef struct {
		uint_fast8_t window_id;			// в окне будут отображаться кнопки с соответствующим полем for_window
		uint_fast16_t x1;
		uint_fast16_t y1;
		uint_fast16_t x2;
		uint_fast16_t y2;
		char * title;					// текст, выводимый в заголовке окна
		uint_fast8_t is_show;			// запрос на отрисовку окна
		void(*onVisibleProcess) (void);
	} windowpip;

	static windowpip windows[] = {
	//     window_id,   x1,  y1, x2,  y2,  title,         is_show,     onVisibleProcess
		{ },
		{ WINDOW_MODES, 214, 20, 586, 175, "Select mode", NON_VISIBLE, },
		{ WINDOW_TEST,  214, 20, 586, 175, "Test",        NON_VISIBLE, window_test_process},
	};
	enum { windows_count = sizeof windows / sizeof windows[0] };

	static button_handler button_handlers [] = {
	//   x1,   y1,  x2,  y2,  onClickHandler,            state,     redraw,    type,         parent,       visible,      payload,	text
		{ 0,   430, 79,  479, button1_handler, 	    BUTTON_CANCELLED, 1, TYPE_FOOTER_BUTTON, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "Mode", },
		{ 82,  430, 161, 479, button2_handler, 	    BUTTON_CANCELLED, 1, TYPE_FOOTER_BUTTON, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "Test", },
		{ 164, 430, 243, 479, button3_handler, 	    BUTTON_CANCELLED, 1, TYPE_FOOTER_BUTTON, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 246, 430, 325, 479, button4_handler, 	    BUTTON_CANCELLED, 1, TYPE_FOOTER_BUTTON, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 328, 430, 407, 479, button5_handler, 	    BUTTON_CANCELLED, 1, TYPE_FOOTER_BUTTON, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 410, 430, 489, 479, button6_handler, 	    BUTTON_CANCELLED, 1, TYPE_FOOTER_BUTTON, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 492, 430, 571, 479, button7_handler, 	    BUTTON_CANCELLED, 1, TYPE_FOOTER_BUTTON, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 574, 430, 653, 479, button8_handler, 	    BUTTON_CANCELLED, 1, TYPE_FOOTER_BUTTON, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "", },
		{ 234,  55, 314, 105, buttons_mode_handler, BUTTON_CANCELLED, 1, TYPE_PIP_BUTTON, 	 WINDOW_MODES, NON_VISIBLE, SUBMODE_LSB, "LSB", },
		{ 319,  55, 399, 105, buttons_mode_handler, BUTTON_CANCELLED, 1, TYPE_PIP_BUTTON, 	 WINDOW_MODES, NON_VISIBLE, SUBMODE_CW,  "CW", },
		{ 404,  55, 484, 105, buttons_mode_handler, BUTTON_CANCELLED, 1, TYPE_PIP_BUTTON, 	 WINDOW_MODES, NON_VISIBLE, SUBMODE_AM,  "AM", },
		{ 489,  55, 569, 105, buttons_mode_handler, BUTTON_CANCELLED, 1, TYPE_PIP_BUTTON, 	 WINDOW_MODES, NON_VISIBLE, SUBMODE_DGL, "DGL", },
		{ 234, 110, 314, 160, buttons_mode_handler, BUTTON_CANCELLED, 1, TYPE_PIP_BUTTON, 	 WINDOW_MODES, NON_VISIBLE, SUBMODE_USB, "USB", },
		{ 319, 110, 399, 160, buttons_mode_handler, BUTTON_CANCELLED, 1, TYPE_PIP_BUTTON, 	 WINDOW_MODES, NON_VISIBLE, SUBMODE_CWR, "CWR", },
		{ 404, 110, 484, 160, buttons_mode_handler, BUTTON_CANCELLED, 1, TYPE_PIP_BUTTON, 	 WINDOW_MODES, NON_VISIBLE, SUBMODE_NFM, "NFM", },
		{ 489, 110, 569, 160, buttons_mode_handler, BUTTON_CANCELLED, 1, TYPE_PIP_BUTTON, 	 WINDOW_MODES, NON_VISIBLE, SUBMODE_DGU, "DGU", },
	};
	enum { button_handlers_count = sizeof button_handlers / sizeof button_handlers[0] };

	static label_handler labels[] = {
	//     x,   y,  parent,      visible,     value,  color
		{ 250, 140, WINDOW_TEST, NON_VISIBLE, "Low",  COLOR565_YELLOW, },
		{ 490, 140, WINDOW_TEST, NON_VISIBLE, "High", COLOR565_YELLOW, },
	};
	enum { labels_count = sizeof labels / sizeof labels[0] };

	static element1_t element = { 0, 0, 0, BUTTON_CANCELLED, 0, 0, 1, 0, 0, 0, 1, };


#endif /* #if WITHTOUCHTEST */
#endif /* GUI_H_INCLUDED */
