/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "audio.h"

#include "src/display/display.h"
#include "formats.h"

#include <string.h>
#include <math.h>
#include "src/touch/touch.h"

#include "keyboard.h"
#include "src/display/fontmaps.h"
#include "list.h"
#include "codecs.h"

#include "src/gui/gui.h"
#include "src/gui/gui_user.h"
#include "src/gui/gui_system.h"
#include "src/gui/gui_structs.h"
#include "src/gui/gui_settings.h"

#if WITHTOUCHGUI

static void update_gui_elements_list(void);
window_t * get_win(uint8_t window_id);
void close_window(uint_fast8_t parent);
void close_all_windows(void);
uint_fast8_t check_for_parent_window(void);

static btn_bg_t btn_bg [] = {
	{ 100, 44, },
	{ 86, 44, },
	{ 50, 50, },
	{ 40, 40, },
};
enum { BG_COUNT = ARRAY_SIZE(btn_bg) };

static gui_t gui = { 0, 0, KBD_CODE_MAX, TYPE_DUMMY, NULL, CANCELLED, 0, 0, 0, 0, 0, 1, };
static gui_element_t gui_elements [GUI_ELEMENTS_ARRAY_SIZE];
static uint_fast8_t gui_element_count = 0;
static button_t close_button = { 0, 0, 0, 0, CANCELLED, BUTTON_NON_LOCKED, 0, NO_PARENT_WINDOW, NON_VISIBLE, UINTPTR_MAX, "btn_close", "", };
static enc2_stack_t enc2_stack;

void clean_enc2_stack(void)
{
	memset(enc2_stack.data, 0, sizeof(enc2_stack.data));
	enc2_stack.size = 0;
}

void push_enc2_stack(const int_fast8_t value)
{
    if (enc2_stack.size >= ENC2_STACK_SIZE)
        return;

    enc2_stack.data [enc2_stack.size] = value;
    enc2_stack.size ++;
}

int_fast8_t pop_enc2_stack(void)
{
    if (enc2_stack.size == 0)
        return 0;

    // суммирование содержимого стека перед возвращением
    int_fast8_t v = 0;
    do {
    	enc2_stack.size --;
    	v += enc2_stack.data [enc2_stack.size];
    } while (enc2_stack.size > 0);

    clean_enc2_stack();
    return v;
}

// WM_MESSAGE_ACTION: element_type type, uintptr_t element_ptr, uint_fast8_t action
// WM_MESSAGE_UPDATE: nothing
uint_fast8_t put_to_wm_queue(window_t * win, wm_message_t message, ...)
{
	if (win->queue.size >= WM_MAX_QUEUE_SIZE)
		return 0;					// очередь переполнена, ошибка

	va_list arg;

	switch (message)
	{
	case WM_MESSAGE_ACTION:

		va_start(arg, message);

		win->queue.data [win->queue.size].message = WM_MESSAGE_ACTION;
		win->queue.data [win->queue.size].type = va_arg(arg, uint_fast8_t);
		win->queue.data [win->queue.size].ptr = va_arg(arg, uintptr_t);
		win->queue.data [win->queue.size].action = va_arg(arg, uint_fast8_t);

		va_end(arg);
		win->queue.size ++;

		return 1;
		break;

	case WM_MESSAGE_UPDATE:

		if (win->queue.data [win->queue.size - 1].message != WM_MESSAGE_UPDATE)		// удаление дублей сообщения WM_MESSAGE_UPDATE
		{
			win->queue.data [win->queue.size].message = WM_MESSAGE_UPDATE;
			win->queue.data [win->queue.size].type = UINT8_MAX;
			win->queue.data [win->queue.size].ptr = UINTPTR_MAX;
			win->queue.data [win->queue.size].action = UINT8_MAX;

			win->queue.size ++;
		}

		return 1;
		break;

	case WM_NO_MESSAGE:
	default:
		return 0;
		break;
	}

	PRINTF("put_to_wm_queue: no valid type of messages found\n");
	ASSERT(0);
	return 0;
}


void clean_wm_stack(window_t * win)
{
	win->queue.size = 0;
}

wm_message_t get_from_wm_queue(window_t * win, ...)
{
	if (! win->queue.size)
		return WM_NO_MESSAGE;							// очередь сообщений пустая

	va_list arg;
	win->queue.size --;

	if (win->queue.data [win->queue.size].message == WM_MESSAGE_ACTION)
	{
		va_start(arg, win);

		* va_arg(arg, uint_fast8_t *) = win->queue.data [win->queue.size].type;
		* va_arg(arg, uintptr_t *) = 	win->queue.data [win->queue.size].ptr;
		* va_arg(arg, uint_fast8_t *) = win->queue.data [win->queue.size].action;

		va_end(arg);
	}

	return win->queue.data [win->queue.size].message;
}

/* Запрос на обновление состояния элементов GUI */
void gui_update(void * arg)
{
	gui.timer_1sec_updated = 1;								// todo: удалить после перехода на очередь сообщений
	put_to_wm_queue(get_win(WINDOW_MAIN), WM_MESSAGE_UPDATE);	// главное окно всегда нужно обновлять

	if (check_for_parent_window() != NO_PARENT_WINDOW)		// если открыто второе окно,
	{
		window_t * win2 = get_win(gui.win [1]);
		if (win2->is_need_update)							// и если оно имеет статус обновляемого,
			put_to_wm_queue(win2, WM_MESSAGE_UPDATE);			// добавить сообщение на обновление в очередь
	}
}

/* Получить состояние таймера GUI */
uint_fast8_t get_gui_1sec_timer(void)
{
	return gui.timer_1sec_updated;
}

/* Сброс данных трекинга тачскрина */
void reset_tracking(void)
{
	gui.vector_move_x = 0;
	gui.vector_move_y = 0;
}

/* Получить данные трекинга */
void get_gui_tracking(int_fast8_t * x, int_fast8_t * y)
{
	* x = gui.vector_move_x;
	* y = gui.vector_move_y;
}

/* Возврат ссылки на запись в структуре по названию и типу окна */
void * find_gui_element(element_type_t type, window_t * win, const char * name)
{
	switch (type)
	{
	case TYPE_BUTTON:
		for (uint_fast8_t i = 0; i < win->bh_count; i ++)
		{
			button_t * bh = & win->bh_ptr [i];
			if (!strcmp(bh->name, name))
				return (button_t *) bh;
		}
		PRINTF("find_gui_element: button '%s' not found\n", name);
		ASSERT(0);
		return NULL;
		break;

	case TYPE_LABEL:
		for (uint_fast8_t i = 0; i < win->lh_count; i ++)
		{
			label_t * lh = & win->lh_ptr [i];
			if (!strcmp(lh->name, name))
				return (label_t *) lh;
		}
		PRINTF("find_gui_element: label '%s' not found\n", name);
		ASSERT(0);
		return NULL;
		break;

	case TYPE_SLIDER:
		for (uint_fast8_t i = 0; i < win->sh_count; i ++)
		{
			slider_t * sh = & win->sh_ptr [i];
			if (!strcmp(sh->name, name))
				return (slider_t *) sh;
		}
		PRINTF("find_gui_element: slider '%s' not found\n", name);
		ASSERT(0);
		return NULL;
		break;

	default:
		PRINTF("find_gui_element: undefined type/n");
		ASSERT(0);
		return NULL;
	}
}

/* Получение ширины метки в пикселях  */
uint_fast8_t get_label_width(const label_t * const lh)
{
	if (lh->font_size == FONT_LARGE)
		return strlen(lh->text) * SMALLCHARW;
	else if (lh->font_size == FONT_MEDIUM)
		return strlen(lh->text) * SMALLCHARW2;
	else if (lh->font_size == FONT_SMALL)
		return strlen(lh->text) * SMALLCHARW3;
	return 0;
}

/* Получение высоты метки в пикселях  */
uint_fast8_t get_label_height(const label_t * const lh)
{
	if (lh->font_size == FONT_LARGE)
		return SMALLCHARH;
	else if (lh->font_size == FONT_MEDIUM)
		return SMALLCHARH2;
	else if (lh->font_size == FONT_SMALL)
		return SMALLCHARH3;
	return 0;
}

/* Установки статуса основных кнопок */
/* При DISABLED в качестве необязательного параметра передать указатель на активную кнопку или NULL для блокирования всех */
void footer_buttons_state (uint_fast8_t state, ...)
{
	window_t * win = get_win(WINDOW_MAIN);
	va_list arg;
	button_t * bt = NULL;
	uint_fast8_t is_name;
	static uint_fast16_t bitmask_locked_buttons = 0;

	if (state == DISABLED)
	{
		va_start(arg, state);
		bt = va_arg(arg, button_t *);
		va_end(arg);
	}

	if (state == DISABLED && bt != NULL)
		bitmask_locked_buttons = 0;

	for (uint_fast8_t i = 0; i < win->bh_count; i ++)
	{
		button_t * bh = & win->bh_ptr [i];

		if (state == DISABLED && bt != NULL)
			bitmask_locked_buttons |= bh->is_locked << i;

		if (state == DISABLED)
		{
			bh->state = bh == bt ? CANCELLED : DISABLED;
			bh->is_locked = bh->state == CANCELLED ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		}
		else if (state == CANCELLED && gui.win [1] == NO_PARENT_WINDOW)
		{
			bh->state = CANCELLED;
			bh->is_locked = ((bitmask_locked_buttons >> i) & 1) ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		}
	}
}

//todo: добавить признак инициализированности элементов и учитывать его при закрытии окна
/* Установка статуса элементов после инициализации */
void elements_state (window_t * win)
{
	uint_fast8_t j = 0;
	int debug_num = 0;

	button_t * b = win->bh_ptr;
	if (b != NULL)
	{
		j = 0;
		for (uint_fast8_t i = 0; i < win->bh_count; i ++)
		{
			button_t * bh = & b [i];
			if (win->state)
			{
				ASSERT(gui_element_count < GUI_ELEMENTS_ARRAY_SIZE);
				gui_elements [gui_element_count].link = bh;
				gui_elements [gui_element_count].win = win;
				gui_elements [gui_element_count].type = TYPE_BUTTON;
				gui_elements [gui_element_count].pos = j ++;
				gui_element_count ++;
				debug_num ++;
			}
			else
			{
				debug_num --;
				gui_element_count --;
				bh->visible = NON_VISIBLE;
				ASSERT(gui_element_count >= footer_buttons_count);
			}
		}
	}

	label_t * l = win->lh_ptr;
	if(l != NULL)
	{
		j = 0;
		for (uint_fast8_t i = 0; i < win->lh_count; i ++)
		{
			label_t * lh = & l [i];
			if (win->state)
			{
				ASSERT(gui_element_count < GUI_ELEMENTS_ARRAY_SIZE);
				gui_elements [gui_element_count].link = lh;
				gui_elements [gui_element_count].win = win;
				gui_elements [gui_element_count].type = TYPE_LABEL;
				gui_elements [gui_element_count].pos = j ++;
				gui_element_count ++;
				debug_num ++;
			}
			else
			{
				debug_num --;
				gui_element_count --;
				lh->visible = NON_VISIBLE;
				ASSERT(gui_element_count >= footer_buttons_count);
			}
		}
	}

	slider_t * s = win->sh_ptr;
	if(s != NULL)
	{
		j = 0;
		for (uint_fast8_t i = 0; i < win->sh_count; i ++)
		{
			slider_t * sh = & s [i];
			if (win->state)
			{
				ASSERT(gui_element_count < GUI_ELEMENTS_ARRAY_SIZE);
				gui_elements [gui_element_count].link = (slider_t *) sh;
				gui_elements [gui_element_count].win = win;
				gui_elements [gui_element_count].type = TYPE_SLIDER;
				gui_elements [gui_element_count].pos = j ++;
				gui_element_count ++;
				debug_num ++;
			}
			else
			{
				debug_num --;
				gui_element_count --;
				sh->visible = NON_VISIBLE;
				ASSERT(gui_element_count >= footer_buttons_count);
			}
		}
	}

	// инициализировать системную кнопку закрытия окна, если разрешено
	if(win->is_close)
	{
		if (win->state)
		{
			close_button.x1 = win->w - window_close_button_size + 1;
			close_button.y1 = 1;
			close_button.w = window_close_button_size - 3;
			close_button.h = window_close_button_size - 3;
			close_button.parent = win->window_id;
			close_button.visible = VISIBLE;
			close_button.state = CANCELLED;

			ASSERT(gui_element_count < GUI_ELEMENTS_ARRAY_SIZE);
			gui_elements [gui_element_count].link = (button_t *) & close_button;
			gui_elements [gui_element_count].win = win;
			gui_elements [gui_element_count].type = TYPE_CLOSE_BUTTON;
			gui_element_count ++;
			debug_num ++;
		}
		else
		{
			debug_num --;
			gui_element_count --;
			close_button.visible = NON_VISIBLE;
			ASSERT(gui_element_count >= footer_buttons_count);
		}
	}
//	PRINTF("line %d: %s gui_element_count: %d %+d\n", __LINE__, win->name, gui_element_count, debug_num);
}

/* Возврат id parent window */
uint_fast8_t check_for_parent_window(void)
{
	return gui.win [1];
}

/* Закрыть все окна (обработчик системной кнопки закрытия окна) */
void close_all_windows(void)
{
	close_window(DONT_OPEN_PARENT_WINDOW);
	footer_buttons_state(CANCELLED);
	hamradio_set_lockmode(0);
	hamradio_disable_keyboard_redirect();
	gui_user_actions_after_close_window();
}

/* Освободить выделенную память в куче и обнулить счетчики элементов окна */
static void free_win_ptr (window_t * win)
{
	free(win->bh_ptr);
	free(win->lh_ptr);
	free(win->sh_ptr);

	win->bh_count = 0;
	win->lh_count = 0;
	win->sh_count = 0;

	win->bh_ptr = NULL;
	win->lh_ptr = NULL;
	win->sh_ptr = NULL;
//	PRINTF("free: %d %s\n", win->window_id, win->name);
}

/* Установка признака видимости окна */
void close_window(uint_fast8_t parent_action) // 0 - не открывать parent window, 1 - открыть
{
	if(gui.win [1] != NO_PARENT_WINDOW)
	{
		window_t * win = get_win(gui.win [1]);
		win->state = NON_VISIBLE;
		elements_state(win);
		free_win_ptr(win);
		gui.win [1] = NO_PARENT_WINDOW;

		if (win->parent_id != NO_PARENT_WINDOW && parent_action)	// При закрытии child window открыть parent window, если есть и если разрешено
		{
			window_t * pwin = get_win(win->parent_id);
			pwin->state = VISIBLE;
			gui.win [1] = pwin->window_id;
			free_win_ptr(pwin);
			pwin->first_call = 1;
		}
		gui_user_actions_after_close_window();
	}
}

/* Открыть окно */
void open_window(window_t * win)
{
	win->state = VISIBLE;
	win->first_call = 1;
	if (win->parent_id != NO_PARENT_WINDOW && gui.win [1] == win->parent_id)	// Если открыто parent window, закрыть его и оставить child window
	{
		window_t * pwin = get_win(win->parent_id);
		pwin->state = NON_VISIBLE;
		elements_state(pwin);
		free_win_ptr(pwin);
	}
	gui.win [1] = win->window_id;
}

/* Расчет экранных координат окна */
/* при mode = WINDOW_POSITION_MANUAL в качестве необязательных параметров передать xmax и ymax */
void calculate_window_position(window_t * win, uint_fast8_t mode, ...)
{
	uint_fast8_t edge_step = 20;
	uint_fast8_t title_length = strlen(win->name) * SMALLCHARW;
	uint_fast16_t xmax = 0, ymax = 0;
	va_list arg;

	if (mode)					// WINDOW_POSITION_MANUAL
	{
		va_start(arg, mode);
		xmax = va_arg(arg, uint_fast16_t);
		ymax = va_arg(arg, uint_fast16_t);
		va_end(arg);
	}
	else						// WINDOW_POSITION_AUTO
	{
		if (win->bh_ptr != NULL)
		{
			for (uint_fast8_t i = 0; i < win->bh_count; i++)
			{
				button_t * bh = & win->bh_ptr [i];
				xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
				ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
				ASSERT(xmax < WITHGUIMAXX);
				ASSERT(ymax < WITHGUIMAXY);
			}
		}

		if (win->lh_ptr != NULL)
		{
			for (uint_fast8_t i = 0; i < win->lh_count; i++)
			{
				label_t * lh = & win->lh_ptr [i];
				xmax = (xmax > lh->x + get_label_width(lh)) ? xmax : (lh->x + get_label_width(lh));
				ymax = (ymax > lh->y + get_label_height(lh)) ? ymax : (lh->y + get_label_height(lh));
				ASSERT(xmax < WITHGUIMAXX);
				ASSERT(ymax < WITHGUIMAXY);
			}
		}

		if (win->sh_ptr != NULL)
		{
			for (uint_fast8_t i = 0; i < win->sh_count; i++)
			{
				slider_t * sh = & win->sh_ptr [i];
				if (sh->orientation)	// ORIENTATION_HORIZONTAL
				{
					xmax = (xmax > sh->x + sh->size + sliders_w) ? xmax : (sh->x + sh->size + sliders_w);
					ymax = (ymax > sh->y + sliders_h * 2) ? ymax : (sh->y + sliders_h * 2);
				}
				else					// ORIENTATION_VERTICAL
				{
					xmax = (xmax > sh->x + sliders_w * 2) ? xmax : (sh->x + sliders_w * 2);
					ymax = (ymax > sh->y + sh->size + sliders_h) ? ymax : (sh->y + sh->size + sliders_h);
				}
				ASSERT(xmax < WITHGUIMAXX);
				ASSERT(ymax < WITHGUIMAXY);
			}
		}
	}

	win->w = xmax > title_length ? (xmax + edge_step) : (title_length + edge_step * 2);
	win->w = (win->is_close && win->w < title_length + window_close_button_size * 2) ? (win->w + window_close_button_size) : win->w;
	win->h = ymax + edge_step;

	win->y1 = ALIGN_Y - win->h / 2;

	switch (win->align_mode)
	{
	case ALIGN_LEFT_X:
		if (ALIGN_LEFT_X - win->w / 2 < 0)
			win->x1 = 0;
		else
			win->x1 = ALIGN_LEFT_X - win->w / 2;
		break;

	case ALIGN_RIGHT_X:
		if (ALIGN_RIGHT_X + win->w / 2 > WITHGUIMAXX)
			win->x1 = WITHGUIMAXX - win->w;
		else
			win->x1 = ALIGN_RIGHT_X - win->w / 2;
		break;

	case ALIGN_CENTER_X:
	default:
		win->x1 = ALIGN_CENTER_X - win->w / 2;
		break;
	}

	ASSERT(win->x1 + win->w < WITHGUIMAXX);
	ASSERT(win->y1 + win->h < WITHGUIMAXY);

	elements_state(win);
	clean_wm_stack(win);	// временная мера, пока все окна не перейдут на использование буфера сообщений от WM
}

/* Передать менеджеру GUI код нажатой кнопки на клавиатуре */
void gui_put_keyb_code (uint_fast8_t kbch)
{
	gui.kbd_code = gui.kbd_code == KBD_CODE_MAX ? kbch : gui.kbd_code;
}

/* Получить переданный код аппаратной кнопки */
uint_fast8_t get_gui_keyb_code(void)
{
	uint_fast8_t code = gui.kbd_code;
	gui.kbd_code = KBD_CODE_MAX;
	return code;
}

/* Удаление пробелов в конце строки */
void remove_end_line_spaces(char * str)
{
	size_t i = strlen(str);
	if (i == 0)
		return;

	for (; -- i > 0;)
	{
		if (str [i] != ' ')
			break;
	}
	str [i + 1] = '\0';
}

/* Удаление пробелов в начале строки */
const char * remove_start_line_spaces(const char * str)
{
	size_t len = strlen(str);
	uint_fast8_t i = 0;

	if (len == 0)
		return NULL;

	for (; i < len; i ++)
	{
		if (str[i] != ' ')
			break;
	}

	if (i >= len)
		return NULL;
	else
		return str + i;
}

/* Отрисовка кнопки */
static void draw_button(const button_t * const bh)
{
	PACKEDCOLORMAIN_T * bg = NULL;
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	window_t * win = get_win(bh->parent);
	uint_fast8_t i = 0;
	static const char delimeters [] = "|";
	uint_fast16_t x1 = win->x1 + bh->x1;
	uint_fast16_t y1 = win->y1 + bh->y1;

	btn_bg_t * b1 = NULL;
	do {
		if (bh->h == btn_bg [i].h && bh->w == btn_bg [i].w)
		{
			b1 = & btn_bg [i];
			break;
		}
	} while ( ++i < BG_COUNT);

	if (b1 == NULL)				// если не найден заполненный буфер фона по размерам, программная отрисовка
	{

		PACKEDCOLORMAIN_T c1, c2;
		c1 = bh->state == DISABLED ? COLOR_BUTTON_DISABLED : (bh->is_locked ? COLOR_BUTTON_LOCKED : COLOR_BUTTON_NON_LOCKED);
		c2 = bh->state == DISABLED ? COLOR_BUTTON_DISABLED : (bh->is_locked ? COLOR_BUTTON_PR_LOCKED : COLOR_BUTTON_PR_NON_LOCKED);
#if GUI_OLDBUTTONSTYLE
		colpip_rect(fr, DIM_X, DIM_Y, x1, y1, x1 + bh->w, y1 + bh->h - 2, bh->state == PRESSED ? c2 : c1, 1);
		colpip_rect(fr, DIM_X, DIM_Y, x1, y1, x1 + bh->w, y1 + bh->h - 1, COLORMAIN_GRAY, 0);
		colpip_rect(fr, DIM_X, DIM_Y, x1 + 2, y1 + 2, x1 + bh->w - 2, y1 + bh->h - 3, COLORMAIN_BLACK, 0);
#else
		colmain_rounded_rect(fr, DIM_X, DIM_Y, x1, y1, x1 + bh->w, y1 + bh->h - 2, button_round_radius, bh->state == PRESSED ? c2 : c1, 1);
		colmain_rounded_rect(fr, DIM_X, DIM_Y, x1, y1, x1 + bh->w, y1 + bh->h - 1, button_round_radius, COLORMAIN_GRAY, 0);
		colmain_rounded_rect(fr, DIM_X, DIM_Y, x1 + 2, y1 + 2, x1 + bh->w - 2, y1 + bh->h - 3, button_round_radius, COLORMAIN_BLACK, 0);
#endif /* GUI_OLDBUTTONSTYLE */
	}
	else
	{
		if (bh->state == DISABLED)
			bg = b1->bg_disabled;
		else if (bh->is_locked && bh->state == PRESSED)
			bg = b1->bg_locked_pressed;
		else if (bh->is_locked && bh->state != PRESSED)
			bg = b1->bg_locked;
		else if (! bh->is_locked && bh->state == PRESSED)
			bg = b1->bg_pressed;
		else if (! bh->is_locked && bh->state != PRESSED)
			bg = b1->bg_non_pressed;
#if GUI_OLDBUTTONSTYLE
		colpip_plot((uintptr_t) fr, GXSIZE(DIM_X, DIM_Y), fr, DIM_X, DIM_Y, x1, y1, (uintptr_t) bg, GXSIZE(bh->w, bh->h), bg, bh->w, bh->h);
#else
		PACKEDCOLORMAIN_T * src = NULL, * dst = NULL, * row = NULL;
		for (uint16_t yy = y1, yb = 0; yy < y1 + bh->h; yy ++, yb ++)
		{
			row = colmain_mem_at(bg, b1->w, b1->h, 0, yb);
			if (* row == GUI_DEFAULTCOLOR)										// если в первой позиции строки буфера не прозрачный цвет,
			{																	// скопировать ее целиком, иначе попиксельно с проверкой
				for (uint16_t xx = x1, xb = 0; xx < x1 + bh->w; xx ++, xb ++)
				{
					src = colmain_mem_at(bg, b1->w, b1->h, xb, yb);
					if (* src == GUI_DEFAULTCOLOR)
						continue;
					dst = colmain_mem_at(fr, DIM_X, DIM_Y, xx, yy);
					memcpy(dst, src, sizeof(PACKEDCOLORMAIN_T));
				}
			}
			else
			{
				dst = colmain_mem_at(fr, DIM_X, DIM_Y, x1, yy);
				memcpy(dst, row, b1->w * sizeof(PACKEDCOLORMAIN_T));
			}
		}
#endif /* GUI_OLDBUTTONSTYLE */
	}

#if GUI_OLDBUTTONSTYLE
	uint_fast8_t shift = bh->state == PRESSED ? 1 : 0;
#else
	uint_fast8_t shift = 0;
#endif /* GUI_OLDBUTTONSTYLE */

	if (strchr(bh->text, delimeters [0]) == NULL)
	{
		/* Однострочная надпись */
		colpip_string2_tbg(fr, DIM_X, DIM_Y, shift + x1 + (bh->w - (strwidth2(bh->text))) / 2,
				shift + y1 + (bh->h - SMALLCHARH2) / 2, bh->text, COLORMAIN_BLACK);
	}
	else
	{
		/* Двухстрочная надпись */
		uint_fast8_t j = (bh->h - SMALLCHARH2 * 2) / 2;
		char buf [TEXT_ARRAY_SIZE];
		strcpy(buf, bh->text);
		char * text2 = strtok(buf, delimeters);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, shift + x1 + (bh->w - (strwidth2(text2))) / 2,
				shift + y1 + j, text2, COLORMAIN_BLACK);

		text2 = strtok(NULL, delimeters);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, shift + x1 + (bh->w - (strwidth2(text2))) / 2,
				shift + bh->h + y1 - SMALLCHARH2 - j, text2, COLORMAIN_BLACK);
	}
}

/* Отрисовка слайдера */
static void draw_slider(slider_t * sl)
{
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	window_t * win = get_win(sl->parent);

	if (sl->orientation)		// ORIENTATION_HORIZONTAL
	{
		if (sl->value_old != sl->value)
		{
			uint_fast16_t mid_w = sl->y + sliders_width / 2;
			sl->value_p = sl->x + sl->size * sl->value / 100;
			sl->y1_p = mid_w - sliders_w;
			sl->x1_p = sl->value_p - sliders_h;
			sl->y2_p = mid_w + sliders_w;
			sl->x2_p = sl->value_p + sliders_h;
			sl->value_old = sl->value;
		}
		colpip_rect(fr, DIM_X, DIM_Y, win->x1 + sl->x, win->y1 + sl->y,  win->x1 + sl->x + sl->size, win->y1 + sl->y + sliders_width, COLORMAIN_BLACK, 1);
		colpip_rect(fr, DIM_X, DIM_Y, win->x1 + sl->x, win->y1 + sl->y,  win->x1 + sl->x + sl->size, win->y1 + sl->y + sliders_width, COLORMAIN_WHITE, 0);
		colpip_rect(fr, DIM_X, DIM_Y, win->x1 + sl->x1_p, win->y1 + sl->y1_p,  win->x1 + sl->x2_p, win->y1 + sl->y2_p, sl->state == PRESSED ? COLOR_BUTTON_PR_NON_LOCKED : COLOR_BUTTON_NON_LOCKED, 1);
		colmain_line(fr, DIM_X, DIM_Y, win->x1 + sl->value_p, win->y1 + sl->y1_p,  win->x1 + sl->value_p, win->y1 + sl->y2_p, COLORMAIN_WHITE, 0);
	}
	else						// ORIENTATION_VERTICAL
	{
		if (sl->value_old != sl->value)
		{
			uint_fast16_t mid_w = sl->x + sliders_width / 2;
			sl->value_p = sl->y + sl->size * sl->value / 100;
			sl->x1_p = mid_w - sliders_w;
			sl->y1_p = sl->value_p - sliders_h;
			sl->x2_p = mid_w + sliders_w;
			sl->y2_p = sl->value_p + sliders_h;
			sl->value_old = sl->value;
		}
		colpip_rect(fr, DIM_X, DIM_Y, win->x1 + sl->x + 1, win->y1 + sl->y + 1, win->x1 + sl->x + sliders_width - 1, win->y1 + sl->y + sl->size - 1, COLORMAIN_BLACK, 1);
		colpip_rect(fr, DIM_X, DIM_Y, win->x1 + sl->x, win->y1 + sl->y, win->x1 + sl->x + sliders_width, win->y1 + sl->y + sl->size, COLORMAIN_WHITE, 0);
		colpip_rect(fr, DIM_X, DIM_Y, win->x1 + sl->x1_p, win->y1 + sl->y1_p, win->x1 + sl->x2_p, win->y1 + sl->y2_p, sl->state == PRESSED ? COLOR_BUTTON_PR_NON_LOCKED : COLOR_BUTTON_NON_LOCKED, 1);
		colmain_line(fr, DIM_X, DIM_Y, win->x1 + sl->x1_p, win->y1 + sl->value_p, win->x1 + sl->x2_p, win->y1 + sl->value_p, COLORMAIN_WHITE, 0);
	}
}

/* Заполнение буферов фонов кнопок при инициализации GUI */
static void fill_button_bg_buf(btn_bg_t * v)
{
	PACKEDCOLORMAIN_T * buf;
	uint_fast8_t w, h;

	w = v->w;
	h = v->h;
	size_t s = GXSIZE(w, h) * sizeof (PACKEDCOLORMAIN_T);

	v->bg_non_pressed = 	(PACKEDCOLORMAIN_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_non_pressed);
	v->bg_pressed = 		(PACKEDCOLORMAIN_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_pressed);
	v->bg_locked = 			(PACKEDCOLORMAIN_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_locked);
	v->bg_locked_pressed = 	(PACKEDCOLORMAIN_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_locked_pressed);
	v->bg_disabled = 		(PACKEDCOLORMAIN_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_disabled);

	buf = v->bg_non_pressed;
#if GUI_OLDBUTTONSTYLE
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_NON_LOCKED, 1);
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORMAIN_GRAY, 0);
	colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORMAIN_BLACK, 0);
#else
	memset(buf, GUI_DEFAULTCOLOR, s);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_NON_LOCKED, 1);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLORMAIN_GRAY, 0);
	colmain_rounded_rect(buf, w, h, 2, 2, w - 3, h - 3, button_round_radius, COLORMAIN_BLACK, 0);
#endif /* GUI_OLDBUTTONSTYLE */

	buf = v->bg_pressed;
#if GUI_OLDBUTTONSTYLE
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_PR_NON_LOCKED, 1);
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORMAIN_GRAY, 0);
	colmain_line(buf, w, h, 2, 3, w - 3, 3, COLORMAIN_BLACK, 0);
	colmain_line(buf, w, h, 2, 2, w - 3, 2, COLORMAIN_BLACK, 0);
	colmain_line(buf, w, h, 3, 3, 3, h - 3, COLORMAIN_BLACK, 0);
	colmain_line(buf, w, h, 2, 2, 2, h - 2, COLORMAIN_BLACK, 0);
#else
	memset(buf, GUI_DEFAULTCOLOR, s);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_PR_NON_LOCKED, 1);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLORMAIN_GRAY, 0);
	colmain_rounded_rect(buf, w, h, 2, 2, w - 3, h - 3, button_round_radius, COLORMAIN_BLACK, 0);
#endif /* GUI_OLDBUTTONSTYLE */

	buf = v->bg_locked;
#if GUI_OLDBUTTONSTYLE
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_LOCKED, 1);
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORMAIN_GRAY, 0);
	colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORMAIN_BLACK, 0);
#else
	memset(buf, GUI_DEFAULTCOLOR, s);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_LOCKED, 1);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLORMAIN_GRAY, 0);
	colmain_rounded_rect(buf, w, h, 2, 2, w - 3, h - 3, button_round_radius, COLORMAIN_BLACK, 0);
#endif /* GUI_OLDBUTTONSTYLE */

	buf = v->bg_locked_pressed;
#if GUI_OLDBUTTONSTYLE
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_PR_LOCKED, 1);
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORMAIN_GRAY, 0);
	colmain_line(buf, w, h, 2, 3, w - 3, 3, COLORMAIN_BLACK, 0);
	colmain_line(buf, w, h, 2, 2, w - 3, 2, COLORMAIN_BLACK, 0);
	colmain_line(buf, w, h, 3, 3, 3, h - 3, COLORMAIN_BLACK, 0);
	colmain_line(buf, w, h, 2, 2, 2, h - 2, COLORMAIN_BLACK, 0);
#else
	memset(buf, GUI_DEFAULTCOLOR, s);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_PR_LOCKED, 1);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLORMAIN_GRAY, 0);
	colmain_rounded_rect(buf, w, h, 2, 2, w - 3, h - 3, button_round_radius, COLORMAIN_BLACK, 0);
#endif /* GUI_OLDBUTTONSTYLE */

	buf = v->bg_disabled;
#if GUI_OLDBUTTONSTYLE
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_DISABLED, 1);
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORMAIN_GRAY, 0);
	colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORMAIN_BLACK, 0);
#else
	memset(buf, GUI_DEFAULTCOLOR, s);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_DISABLED, 1);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLORMAIN_GRAY, 0);
	colmain_rounded_rect(buf, w, h, 2, 2, w - 3, h - 3, button_round_radius, COLORMAIN_BLACK, 0);
#endif /* GUI_OLDBUTTONSTYLE */
}

/* Инициализация GUI */
void gui_initialize (void)
{
	uint_fast8_t i = 0;
	window_t * win = get_win(WINDOW_MAIN);

	open_window(win);
	gui.win [1] = NO_PARENT_WINDOW;

	do {
		fill_button_bg_buf(& btn_bg [i]);
	} while ( ++i < BG_COUNT) ;
}

/* Обновление данных в массиве элементов открытых окон */
static void update_gui_elements_list(void)
{
	for (uint_fast8_t i = 0; i < gui_element_count; i ++)
	{
		gui_element_t * p = & gui_elements [i];
		if (p->type == TYPE_BUTTON || p->type == TYPE_CLOSE_BUTTON)
		{
			button_t * bh = (button_t *) p->link;
			p->x1 = bh->x1;
			p->x2 = bh->x1 + bh->w;
			p->y1 = bh->y1;
			p->y2 = bh->y1 + bh->h;
			p->state = bh->state;
			p->visible = bh->visible;
			p->is_trackable = 0;
			p->is_long_press = bh->is_long_press;
		}
		else if (p->type == TYPE_LABEL)
		{
			label_t * lh = (label_t *) p->link;
			p->x1 = lh->x;
			p->x2 = lh->x + get_label_width(lh);
			p->y1 = lh->y - get_label_height(lh);
			p->y2 = lh->y + get_label_height(lh) * 2;
			p->state = lh->state;
			p->visible = lh->visible;
			p->is_trackable = lh->is_trackable;
			p->is_long_press = 0;
		}
		else if (p->type == TYPE_SLIDER)
		{
			slider_t * sh = (slider_t *) p->link;
			p->x1 = sh->x1_p;
			p->x2 = sh->x2_p;
			p->y1 = sh->y1_p;
			p->y2 = sh->y2_p;
			p->state = sh->state;
			p->visible = sh->visible;
			p->is_trackable = 1;
			p->is_long_press = 0;
		}
	}
}

/* Системный обработчик слайдера в момент его перемещения */
static void slider_process(slider_t * sl)
{
	uint16_t v = sl->value + round((sl->orientation ? gui.vector_move_x : gui.vector_move_y) / sl->step);
	if (v >= 0 && v <= sl->size / sl->step)
		sl->value = v;
	reset_tracking();
}

/* Возврат позиции однотипного элемента */
uint_fast8_t get_selected_element_pos(void)
{
	return gui.selected_link->pos;
}

/* Селектор запуска функций обработки событий */
static void set_state_record(gui_element_t * val)
{
	ASSERT(val != NULL);
	switch (val->type)
	{
		case TYPE_CLOSE_BUTTON:
			close_all_windows();
			break;

		case TYPE_BUTTON:
			ASSERT(val->link != NULL);
			button_t * bh = (button_t *) val->link;
			gui.selected_type = TYPE_BUTTON;
			gui.selected_link = val;
			bh->state = val->state;
			if (bh->state == RELEASED || bh->state == LONG_PRESSED)
			{
				if (! put_to_wm_queue(val->win, WM_MESSAGE_ACTION, TYPE_BUTTON, bh, bh->state == LONG_PRESSED ? LONG_PRESSED : PRESSED))
					PRINTF("WM stack on window '%s' full!\n", val->win->name);
			}
			break;

		case TYPE_LABEL:
			ASSERT(val->link != NULL);
			label_t * lh = (label_t *) val->link;
			gui.selected_type = TYPE_LABEL;
			gui.selected_link = val;
			lh->state = val->state;
			if (lh->state == RELEASED)
			{
				if (! put_to_wm_queue(val->win, WM_MESSAGE_ACTION, TYPE_LABEL, lh, PRESSED))
					PRINTF("WM stack on window '%s' full!\n", val->win->name);
			}
			else if (lh->state == PRESSED && lh->is_trackable)
			{
				if (! put_to_wm_queue(val->win, WM_MESSAGE_ACTION, TYPE_LABEL, lh, MOVING))
					PRINTF("WM stack on window '%s' full!\n", val->win->name);
			}
			break;

		case TYPE_SLIDER:
			ASSERT(val->link != NULL);
			slider_t * sh = (slider_t *) val->link;
			gui.selected_type = TYPE_SLIDER;
			gui.selected_link = val;
			sh->state = val->state;
			if (sh->state == PRESSED)
			{
				slider_process(sh);
				if (! put_to_wm_queue(val->win, WM_MESSAGE_ACTION, TYPE_SLIDER, sh, PRESSED))
					PRINTF("WM stack on window '%s' full!\n", val->win->name);
			}
			break;

		default:
			PRINTF("set_state_record: undefined type\n");
			ASSERT(0);
			break;
	}
}

/* GUI state mashine */
static void process_gui(void)
{
	uint_fast16_t tx, ty;
	static uint_fast16_t x_old = 0, y_old = 0, long_press_counter = 0;
	static gui_element_t * p = NULL;
	static window_t * w = NULL;
	uint_fast8_t long_press_limit = 20;
	static uint_fast8_t is_long_press = 0;		// 1 - долгое нажатие уже обработано

#if defined (TSC1_TYPE)
	if (board_tsc_getxy(& tx, & ty))
	{
		gui.last_pressed_x = tx;
		gui.last_pressed_y = ty;
		gui.is_touching_screen = 1;
//		PRINTF(PSTR("last x/y=%d/%d\n"), gui.last_pressed_x, gui.last_pressed_y);
		update_gui_elements_list();
	}
	else
#endif /* defined (TSC1_TYPE) */
	{
		gui.is_touching_screen = 0;
		gui.is_after_touch = 0;
	}

	if (gui.state == CANCELLED && gui.is_touching_screen && ! gui.is_after_touch)
	{
		for (uint_fast8_t i = 0; i < gui_element_count; i ++)
		{
			p = & gui_elements [i];
			w = p->win;
			uint_fast16_t x1 = p->x1 + w->x1, y1 = p->y1 + w->y1;
			uint_fast16_t x2 = p->x2 + w->x1, y2 = p->y2 + w->y1;

			if (x1 < gui.last_pressed_x && x2 > gui.last_pressed_x && y1 < gui.last_pressed_y && y2 > gui.last_pressed_y
					&& p->state != DISABLED && p->visible == VISIBLE)
			{
				gui.state = PRESSED;
				is_long_press = 0;
				long_press_counter = 0;
				break;
			}
		}
	}

	if (gui.is_tracking && ! gui.is_touching_screen)
	{
		gui.is_tracking = 0;
		reset_tracking();
		x_old = 0;
		y_old = 0;
	}

	if (gui.state == PRESSED)
	{
		ASSERT(p != NULL);
		if (p->is_trackable && gui.is_touching_screen)
		{
			gui.vector_move_x = x_old ? gui.vector_move_x + gui.last_pressed_x - x_old : 0; // проверить, нужно ли оставить накопление
			gui.vector_move_y = y_old ? gui.vector_move_y + gui.last_pressed_y - y_old : 0;

			if (gui.vector_move_x != 0 || gui.vector_move_y != 0)
			{
				gui.is_tracking = 1;
//				PRINTF(PSTR("move x: %d, move y: %d\n"), gui.vector_move_x, gui.vector_move_y);
			}
			p->state = PRESSED;
			set_state_record(p);

			x_old = gui.last_pressed_x;
			y_old = gui.last_pressed_y;
		}
		else if (w->x1 + p->x1 < gui.last_pressed_x && w->x1 + p->x2 > gui.last_pressed_x
				&& w->y1 + p->y1 < gui.last_pressed_y && w->y1 + p->y2 > gui.last_pressed_y && ! gui.is_after_touch)
		{
			if (gui.is_touching_screen)
			{
				p->state = PRESSED;
				set_state_record(p);

				if(gui.state != LONG_PRESSED && ! is_long_press && p->is_long_press)
					long_press_counter ++;

				if(long_press_counter > long_press_limit && p->is_long_press)
				{
					long_press_counter = 0;
					gui.state = LONG_PRESSED;
				}
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
		if(! is_long_press)				// если было долгое нажатие, обработчик по короткому не запускать
			set_state_record(p);
		p->state = CANCELLED;
		set_state_record(p);
		gui.is_after_touch = 0;
		gui.state = CANCELLED;
		gui.is_tracking = 0;
	}
	if (gui.state == LONG_PRESSED)
	{
		p->state = LONG_PRESSED;		// для запуска обработчика нажатия
		set_state_record(p);
		p->state = PRESSED;
		gui.state = PRESSED;
		is_long_press = 1;				// долгое нажатие обработано
	}
}

/* Запуск state mashine и отрисовка элементов GUI */
void gui_WM_walktrough(uint_fast8_t x, uint_fast8_t y, dctx_t * pctx)
{
	uint_fast8_t alpha = DEFAULT_ALPHA; // на сколько затемнять цвета
	char buf [TEXT_ARRAY_SIZE];
	char * text2 = NULL;
	uint_fast8_t str_len = 0;
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();

	process_gui();

	for(uint_fast8_t i = 0; i < WIN_GUI_COUNT; i ++)
	{
		if (gui.win [i] == NO_PARENT_WINDOW)
			break;

		const window_t * const win = get_win(gui.win [i]);
		uint_fast8_t f = win->first_call;

		if (win->state == VISIBLE)
		{
			// при открытии окна рассчитываются экранные координаты самого окна и его child элементов
			if (! f)
			{
				if (win->window_id != WINDOW_MAIN)
				{
					ASSERT(win->w > 0 || win->h > 0);
#if GUI_TRANSPARENT_WINDOWS
					display_transparency(win->x1, strcmp(win->name, "") ? (win->y1 + window_title_height) : win->y1, win->x1 + win->w - 1, win->y1 + win->h - 1, alpha);
#else
					colpip_fillrect(fr, DIM_X, DIM_Y, win->x1, strcmp(win->name, "") ? (win->y1 + window_title_height) : win->y1, win->w, win->h, GUI_WINDOWBGCOLOR);
#endif /* GUI_TRANSPARENT_WINDOWS */
				}
			}

			// запуск процедуры фоновой обработки для окна
			win->onVisibleProcess();

			if (! f)
			{
				// вывод заголовка окна
				if (strcmp(win->name, ""))
				{
					colpip_fillrect(fr, DIM_X, DIM_Y, win->x1, win->y1, win->w, window_title_height, 20);
					colpip_string_tbg(fr, DIM_X, DIM_Y, win->x1 + window_title_indent, win->y1 + 5, win->name, COLORMAIN_BLACK);
				}

				// отрисовка принадлежащих окну элементов
				for (uint_fast8_t i = 0; i < gui_element_count; i ++)
				{
					gui_element_t * p = & gui_elements [i];

					if (p->type == TYPE_BUTTON)
					{
						button_t * bh = (button_t *) p->link;
						if (bh->visible && bh->parent == win->window_id)
							draw_button(bh);
					}
					else if (p->type == TYPE_CLOSE_BUTTON)
					{
						button_t * bh = (button_t *) p->link;
						if (bh->visible && bh->parent == win->window_id)
						{
							colpip_rect(fr, DIM_X, DIM_Y, win->x1 + bh->x1, win->y1 + bh->y1, win->x1 + bh->x1 + bh->w,  win->y1 + bh->y1 + bh->h, COLORMAIN_BLACK, 0);
							colmain_line(fr, DIM_X, DIM_Y, win->x1 + bh->x1, win->y1 + bh->y1, win->x1 + bh->x1 + bh->w, win->y1 + bh->y1 + bh->h, COLORMAIN_BLACK, 0);
							colmain_line(fr, DIM_X, DIM_Y, win->x1 + bh->x1, win->y1 + bh->y1 + bh->h, win->x1 + bh->x1 + bh->w, win->y1 + bh->y1, COLORMAIN_BLACK, 0);
						}
					}
					else if (p->type == TYPE_LABEL)
					{
						label_t * lh = (label_t *) p->link;
						if (lh->visible && lh->parent == win->window_id)
						{
							if (lh->font_size == FONT_LARGE)
								colpip_string_tbg(fr, DIM_X, DIM_Y,  win->x1 + lh->x, win->y1 + lh->y, lh->text, lh->color);
							else if (lh->font_size == FONT_MEDIUM)
								colpip_string2_tbg(fr, DIM_X, DIM_Y, win->x1 + lh->x, win->y1 + lh->y, lh->text, lh->color);
							else if (lh->font_size == FONT_SMALL)
								colpip_string3_tbg(fr, DIM_X, DIM_Y, win->x1 + lh->x, win->y1 + lh->y, lh->text, lh->color);
						}
					}
					else if (p->type == TYPE_SLIDER)
					{
						slider_t * sh = (slider_t *) p->link;
						if (sh->visible && sh->parent == win->window_id)
							draw_slider(sh);
					}
				}
			}
		}
	}
}
#endif /* WITHTOUCHGUI */
