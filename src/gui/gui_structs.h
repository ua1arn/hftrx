#ifndef GUI_STRUCTS_H_INCLUDED
#define GUI_STRUCTS_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI

#include "src/gui/gui_settings.h"

#define IS_BUTTON_PRESS			(type == TYPE_BUTTON && action == PRESSED)
#define IS_BUTTON_LONG_PRESS	(type == TYPE_BUTTON && action == LONG_PRESSED)
#define IS_SLIDER_MOVE			(type == TYPE_SLIDER && action == PRESSED)
#define IS_LABEL_PRESS			(type == TYPE_LABEL && action == PRESSED)
#define IS_LABEL_MOVE			(type == TYPE_LABEL && action == MOVING)
#define IS_AREA_TOUCHED 		(type == TYPE_TOUCH_AREA && action == PRESSED)
#define IS_AREA_MOVE	 		(type == TYPE_TOUCH_AREA && action == MOVING)

typedef enum {
	TYPE_DUMMY,
	TYPE_BUTTON,
	TYPE_LABEL,
	TYPE_SLIDER,
	TYPE_CLOSE_BUTTON,
	TYPE_TOUCH_AREA,
	TYPE_TEXT_FIELD
} element_type_t;

enum {
	PRESSED,						// нажато
	RELEASED,						// отпущено после нажатия внутри элемента
	CANCELLED,						// первоначальное состояние или отпущено после нажатия вне элемента
	DISABLED,						// заблокировано для нажатия
	LONG_PRESSED,
	PRESS_REPEATING,
	MOVING
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
	NO_PARENT_WINDOW = UINT8_MAX,
};

enum {
	WINDOW_POSITION_AUTO,
	WINDOW_POSITION_MANUAL_SIZE,
	WINDOW_POSITION_MANUAL_POSITION,
	WINDOW_POSITION_FULLSCREEN
};

// выравнивание заголовка окна, по умолчанию - слева
typedef enum {
	TITLE_ALIGNMENT_LEFT,
	TITLE_ALIGNMENT_RIGHT,
	TITLE_ALIGNMENT_CENTER
} title_align_t;

enum {
	NAME_ARRAY_SIZE = 40,
	MENU_ARRAY_SIZE = 50,
	TEXT_ARRAY_SIZE = 70,
	GUI_ELEMENTS_ARRAY_SIZE = 60
};

typedef enum {
	UP,
	DOWN,
} tf_direction_t;

typedef struct {
	uint16_t w;
	uint16_t h;
	PACKEDCOLORPIP_T * bg_non_pressed;
	PACKEDCOLORPIP_T * bg_pressed;
	PACKEDCOLORPIP_T * bg_locked;
	PACKEDCOLORPIP_T * bg_locked_pressed;
	PACKEDCOLORPIP_T * bg_disabled;
} btn_bg_t;

typedef struct {
   char text[TEXT_ARRAY_SIZE];
   COLORPIP_T color_line;
} tf_entry_t;

typedef struct {
	uint16_t w_sim;		// ширина строки в символах
	uint16_t h_str;		// число строк
	uint8_t state;
	uint8_t parent;
	uint8_t visible;
	tf_direction_t direction;
	UB_Font * font;		// NULL - SMALLCHAR2
	char name [NAME_ARRAY_SIZE];
	uint8_t index;
	tf_entry_t * string;
	uint16_t x1;
	uint16_t y1;
	uint16_t w;
	uint16_t h;
} text_field_t;

typedef struct {
	uint8_t state;
	uint8_t parent;
	uint8_t visible;
	uint8_t is_trackable;
	int32_t payload;
	char name [NAME_ARRAY_SIZE];
	uint8_t index;
	uint16_t x1;
	uint16_t y1;
	uint16_t w;
	uint16_t h;
} touch_area_t;

typedef struct {
	uint16_t w;
	uint16_t h;
	uint8_t state;					// текущее состояние кнопки
	uint8_t is_locked;				// признак фиксации кнопки
	uint8_t is_repeating;			// повтор действия при длительном удержании кнопки
	uint8_t is_long_press;			// разрешение обработки долгого нажатия
	uint8_t parent;					// индекс окна, в котором будет отображаться кнопка
	uint8_t visible;				// рисовать ли кнопку на экране
	int32_t payload;
	char name [NAME_ARRAY_SIZE];
	char text [TEXT_ARRAY_SIZE]; 	// текст внутри кнопки, разделитель строк |, не более 2х строк
	uint8_t index;
	uint16_t x1;					// координаты от начала окна
	uint16_t y1;
} button_t;

typedef enum {
	FONT_LARGE,						// S1D13781_smallfont_LTDC
	FONT_MEDIUM,					// S1D13781_smallfont2_LTDC
	FONT_SMALL						// S1D13781_smallfont3_LTDC
} font_size_t;

typedef struct {
	uint8_t parent;
	uint8_t state;
	uint8_t is_trackable;
	uint8_t visible;
	char name [NAME_ARRAY_SIZE];
	char text [TEXT_ARRAY_SIZE];
	font_size_t font_size;
	PACKEDCOLORPIP_T color;
	uint8_t index;
	uint8_t width;			// ширина в символах
	uint16_t width_pix;		// ширина в пикселях
	uint16_t x;
	uint16_t y;
} label_t;

typedef enum  {
	ORIENTATION_VERTICAL,
	ORIENTATION_HORIZONTAL
} orientation_t;

typedef struct {
	orientation_t orientation;
	uint8_t parent;
	char name [NAME_ARRAY_SIZE];
	uint8_t state;
	uint8_t visible;
	uint16_t size;					// длина шкалы в пикселях
	uint8_t value;					// 0..100 %
	uint8_t value_old;				// для перерасчетов при изменении значения
	uint16_t value_p;				// в пикселях от начала шкалы
	float step;
	uint8_t index;
	uint16_t x;
	uint16_t y;
	uint16_t x1_p;					// координаты ползунка
	uint16_t y1_p;					// для update_touch_list_list
	uint16_t x2_p;
	uint16_t y2_p;
} slider_t;

typedef enum {
	ALIGN_LEFT_X 	= WITHGUIMAXX / 4,					// вертикальное выравнивание по центру левой половины экрана
	ALIGN_CENTER_X 	= WITHGUIMAXX / 2,					// вертикальное выравнивание по центру экрана
	ALIGN_RIGHT_X 	= ALIGN_LEFT_X + ALIGN_CENTER_X,	// вертикальное выравнивание по центру правой половины экрана
	ALIGN_MANUAL 	= 0,								// ручное указание координат
	ALIGN_Y 	= WITHGUIMAXY / 2 - FOOTER_HEIGHT / 2	// горизонтальное выравнивание всегда по центру экрана
} window_align_t;

enum {
	WIN_GUI_COUNT = 2,		 		// на экране не более 2х окон, одно из которых - основное на весь экран
	WM_MAX_QUEUE_SIZE = 5			// размер очереди сообщений WM
};

typedef enum {
	WM_NO_MESSAGE,
	WM_MESSAGE_UPDATE,				// запрос на обновление состояния элементов GUI в зависимости от состояния базовой системы
	WM_MESSAGE_ACTION,				// необходима реакция на действия с элементами
	WM_MESSAGE_ENC2_ROTATE,			// необходима обработка результата вращения 2-го энкодера
	WM_MESSAGE_KEYB_CODE,			// необходима обработка переданного кода аппаратной кнопки
	WM_MESSAGE_CLOSE				// выполнение операций после закрытия окна
} wm_message_t;

typedef struct {
	wm_message_t message;			// тип сообщения
	element_type_t type;			// тип элемента
	uintptr_t ptr;
	int_fast8_t action;
} wm_data_t;

typedef struct {					// очередь сообщений окнам от WM о взаимодействии с элементами GUI
	wm_data_t data[WM_MAX_QUEUE_SIZE];
    size_t size;
} wm_queue_t;

typedef struct {
//	*** обязательные для указания вручную элементы описателя ***
	const uint8_t window_id;		// в окне будут отображаться элементы с соответствующим полем for_window
	uint8_t parent_id;				// UINT8_MAX - нет parent window
	window_align_t align_mode;		// вертикаль выравнивания окна
	char title [NAME_ARRAY_SIZE];	// текст, выводимый в заголовке окна
	uint8_t is_close;				// разрешение или запрет вывода кнопки закрытия окна
	void (*onVisibleProcess) (void);
	char name [NAME_ARRAY_SIZE];	// имя окна
//	*** служебные и автоматически заполняемые элементы структуры ***
	button_t * bh_ptr;				// указатели на массивы оконных элементов
	uint8_t bh_count;
	label_t * lh_ptr;
	uint8_t lh_count;
	slider_t * sh_ptr;
	uint8_t sh_count;
	touch_area_t * ta_ptr;
	uint8_t ta_count;
	text_field_t * tf_ptr;
	uint8_t tf_count;
	wm_queue_t queue;
	uint8_t first_call;				// признак первого вызова для различных инициализаций
	uint8_t state;
	uint16_t x1;
	uint16_t y1;
	uint16_t w;
	uint16_t h;
	uint16_t draw_x1;				// координаты и размеры внутриоконной области,
	uint16_t draw_y1;				// доступной для вывода графики
	uint16_t draw_x2;
	uint16_t draw_y2;
	title_align_t title_align;
	uint8_t size_mode;
	uint8_t is_moving;
} window_t;

typedef struct {
	element_type_t type;			// тип элемента, поддерживающего реакцию на касания
	window_t * win;					// указатель на parent window
	void * link;					// указатель на запись массива окна с описанием элемента
	uint8_t state;					// текущее состояние элемента
	uint8_t visible;				// текущая видимость элемента
	uint8_t is_trackable;			// поддерживает ли элемент возврат относительных координат перемещения точки нажатия
	uint8_t is_repeating;			// повтор действия при длительном удержании
	uint8_t is_long_press;			// разрешение обработки долгого нажатия
	uint16_t x1;					// координаты области реакции на касание элемента
	uint16_t y1;
	uint16_t x2;
	uint16_t y2;
} gui_element_t;

typedef struct {
	uint16_t last_pressed_x; 	  	// последняя точка касания экрана
	uint16_t last_pressed_y;
	element_type_t selected_type; 	// тип последнего выбранного элемента
	gui_element_t * selected_link;	// ссылка на выбранный элемент
	uint8_t state;				  	// последнее состояние
	uint8_t is_touching_screen;   	// есть ли касание экрана в данный момент
	uint8_t is_after_touch; 	  	// есть ли касание экрана после выхода точки касания из элемента (при is_tracking == 0)
	uint8_t is_tracking;		  	// получение относительных координат точки перемещения нажатия
	int16_t vector_move_x;	 	  	// в т.ч. и за границами элемента, при state == PRESSED
	int16_t vector_move_y;
	uint8_t win[WIN_GUI_COUNT];		// на экране не более 2х окон, одно из которых - основное на весь экран
	uint8_t footer_buttons_count;
} gui_t;

#endif /* WITHTOUCHGUI */
#endif /* GUI_STRUCTS_H_INCLUDED */
