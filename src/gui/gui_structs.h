#ifndef GUI_STRUCTS_H_INCLUDED
#define GUI_STRUCTS_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI

#include "src/gui/gui_settings.h"

typedef enum {
	TYPE_DUMMY,
	TYPE_BUTTON,
	TYPE_LABEL,
	TYPE_SLIDER,
	TYPE_CLOSE_BUTTON
} element_type_t;

enum {
	PRESSED,						// нажато
	RELEASED,						// отпущено после нажатия внутри элемента
	CANCELLED,						// первоначальное состояние или отпущено после нажатия вне элемента
	DISABLED,						// заблокировано для нажатия
	LONG_PRESSED
};

enum {
	BUTTON_NON_LOCKED,
	BUTTON_LOCKED
};

enum {
	BUTTON_CODE_BK = 20,
	BUTTON_CODE_OK = 30,
	BUTTON_CODE_DONE = 99
};

enum {
	NON_VISIBLE,					// parent window закрыто, кнопка не отрисовывается
	VISIBLE							// parent window на экране, кнопка отрисовывается
};

enum {
	DONT_OPEN_PARENT_WINDOW,
	OPEN_PARENT_WINDOW
};

enum {
	NAME_ARRAY_SIZE = 30,
	TEXT_ARRAY_SIZE = 30,
	MENU_ARRAY_SIZE = 30,
	GUI_ELEMENTS_ARRAY_SIZE = 50
};

typedef struct {
	uint16_t w;
	uint16_t h;
	PACKEDCOLORMAIN_T * bg_non_pressed;
	PACKEDCOLORMAIN_T * bg_pressed;
	PACKEDCOLORMAIN_T * bg_locked;
	PACKEDCOLORMAIN_T * bg_locked_pressed;
	PACKEDCOLORMAIN_T * bg_disabled;
} btn_bg_t;

typedef struct {
	uint16_t x1;				// координаты от начала экрана
	uint16_t y1;
	uint16_t w;
	uint16_t h;
	void(*onClickHandler) (void);	// обработчик события RELEASED
	uint8_t state;				// текущее состояние кнопки
	uint8_t is_locked;			// признак фиксации кнопки
	uint8_t is_long_press;		// разрешение обработки долгого нажатия
	window_id_t parent;			// индекс окна, в котором будет отображаться кнопка
	uint8_t visible;			// рисовать ли кнопку на экране
	int32_t payload;
	char name [NAME_ARRAY_SIZE];
	char text [TEXT_ARRAY_SIZE]; // текст внутри кнопки, разделитель строк |, не более 2х строк
} button_t;

typedef enum {
	FONT_LARGE,		// S1D13781_smallfont_LTDC
	FONT_MEDIUM,	// S1D13781_smallfont2_LTDC
	FONT_SMALL		// S1D13781_smallfont3_LTDC
} font_size_t;

typedef struct {
	uint16_t x;
	uint16_t y;
	window_id_t parent;
	uint8_t state;
	uint8_t is_trackable;
	uint8_t visible;
	char name [NAME_ARRAY_SIZE];
	char text [TEXT_ARRAY_SIZE];
	font_size_t font_size;
	PACKEDCOLORMAIN_T color;
	void (*onClickHandler) (void);
} label_t;

typedef enum  {
	ORIENTATION_VERTICAL,
	ORIENTATION_HORIZONTAL
} orientation_t;

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t x1_p;			// координаты ползунка
	uint16_t y1_p;			// для update_touch_list_list
	uint16_t x2_p;
	uint16_t y2_p;
	orientation_t orientation;
	window_id_t parent;
	char name [NAME_ARRAY_SIZE];
	uint8_t state;
	uint8_t visible;
	uint16_t size;			// длина шкалы в пикселях
	uint8_t value;			// 0..100 %
	uint8_t value_old;		// для перерасчетов при изменении значения
	uint16_t value_p;		// в пикселях от начала шкалы
	float step;
} slider_t;

typedef enum {
	ALIGN_LEFT_X 	= WITHGUIMAXX >> 2,					// вертикальное выравнивание по центру левой половины экрана
	ALIGN_CENTER_X 	= WITHGUIMAXX >> 1,					// вертикальное выравнивание по центру экрана
	ALIGN_RIGHT_X 	= ALIGN_LEFT_X + ALIGN_CENTER_X,	// вертикальное выравнивание по центру правой половины экрана
	ALIGN_Y 		= WITHGUIMAXY >> 1					// горизонтальное выравнивание всегда по центру экрана
} window_align_t;

typedef struct {
	const window_id_t window_id;// в окне будут отображаться элементы с соответствующим полем for_window
	window_id_t parent_id;		// UINT8_MAX - нет parent window
	window_align_t align_mode;			// вертикаль выравнивания окна
	uint16_t x1;
	uint16_t y1;
	uint16_t w;
	uint16_t h;
	char name[NAME_ARRAY_SIZE];	// текст, выводимый в заголовке окна
	uint8_t state;
	uint8_t first_call;			// признак первого вызова для различных инициализаций
	uint8_t is_close;			// разрешение или запрет вывода кнопки закрытия окна
	void (*onVisibleProcess) (void);
	button_t * bh_ptr;			// указатели на массивы оконных элементов
	uint8_t bh_count;
	label_t * lh_ptr;
	uint8_t lh_count;
	slider_t * sh_ptr;
	uint8_t sh_count;
} window_t;

typedef struct {
	element_type_t type;		// тип элемента, поддерживающего реакцию на касания
	window_t * win;
	void * link;
	uint8_t pos;
	uint8_t state;				// текущее состояние элемента
	uint8_t visible;			// текущая видимость элемента
	uint8_t is_trackable;		// поддерживает ли элемент возврат относительных координат перемещения точки нажатия
	uint8_t is_long_press;		// разрешение обработки долгого нажатия
	uint16_t x1;				// координаты окна
	uint16_t y1;
	uint16_t x2;
	uint16_t y2;
	LIST_ENTRY item;
} gui_element_t;

enum { win_gui_count = 2 };		  // на экране не более 2х окон, одно из которых - основное на весь экран

typedef struct {
	uint16_t last_pressed_x; 	  // последняя точка касания экрана
	uint16_t last_pressed_y;
	uint8_t kbd_code;
	element_type_t selected_type; // тип последнего выбранного элемента
	gui_element_t * selected_link;	  // ссылка на выбранный элемент
	uint8_t state;				  // последнее состояние
	uint8_t is_touching_screen;   // есть ли касание экрана в данный момент
	uint8_t is_after_touch; 	  // есть ли касание экрана после выхода точки касания из элемента (при is_tracking == 0)
	uint8_t is_tracking;		  // получение относительных координат точки перемещения нажатия
	int16_t vector_move_x;	 	  // в т.ч. и за границами элемента, при state == PRESSED
	int16_t vector_move_y;
	uint8_t timer_1sec_updated;	  // для периодических обновлений состояния
	window_id_t win[win_gui_count];	// на экране не более 2х окон, одно из которых - основное на весь экран
} gui_t;

#endif /* WITHTOUCHGUI */
#endif /* GUI_STRUCTS_H_INCLUDED */
