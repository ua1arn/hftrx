/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN


#include <string.h>
#include <math.h>

#include "hardware.h"
#include "keyboard.h"
#include "formats.h"
#include "mslist.h"
#include "src/touch/touch.h"
#include "src/display/display.h"
#include "src/display/fontmaps.h"

#include "gui.h"
#include "gui_user.h"
#include "gui_system.h"
#include "gui_structs.h"
#include "gui_settings.h"
#include "gui_windows.h"
#include "gui_objects.h"
#include "utils.h"

#if WITHTOUCHGUI

static btn_bg_t btn_bg[] = {
	{ 130, 35, },
	{ 100, 44, },
	{ 86, 44, },
};
enum { BG_COUNT = ARRAY_SIZE(btn_bg) };

static gui_t gui = { 0, 0, TYPE_DUMMY, NULL, CANCELLED, 0, 0, 0, 0, 0, };
static LIST_ENTRY gui_objects_list;
static uint_fast8_t gui_object_count = 0;
static button_t close_button = { 0, 0, CANCELLED, BUTTON_NON_LOCKED, 0, 0, NO_PARENT_WINDOW, NON_VISIBLE, INT32_MAX, "btс_close", "", };
static uint8_t opened_windows_count = 1;
wm_data_t wm_message_default = { WM_NO_MESSAGE, TYPE_DUMMY, 0, "", };

/* Возврат id parent window */
uint_fast8_t get_parent_window(void)
{
	return gui.win[1];
}

void set_parent_window(uint8_t p)
{
	gui.win[1] = p;
	opened_windows_count = p == NO_PARENT_WINDOW ? 1 : 2;
}

void gui_set_encoder2_rotate (int_least16_t rotate)
{
	if (rotate != 0)
	{
		// информация о вращении 2-го энкодера направляется только в активное окно
		if (get_parent_window() == NO_PARENT_WINDOW)
			put_to_wm_queue(get_win(WINDOW_MAIN), WM_MESSAGE_ENC2_ROTATE, rotate);
		else
			put_to_wm_queue(get_win(gui.win[1]), WM_MESSAGE_ENC2_ROTATE, rotate);
	}
}

void dump_queue(window_t * win)
{
	if (! win->queue_size)
		return;

	PRINTF("dump WM queue window '%s'\n", win->title);

	if (win->queue_size == WM_MAX_QUEUE_SIZE)
		PRINTF("WM stack full!\n");

	for (PLIST_ENTRY t = win->queue_head.Flink; t != & win->queue_head; t = t->Flink)
	{
		wm_data_t * const p = CONTAINING_RECORD(t, wm_data_t, list_entry);

		switch(p->message)
		{
		case WM_MESSAGE_UPDATE:
			PRINTF("WM_MESSAGE_UPDATE\n");
			break;

		case WM_MESSAGE_ACTION:
			PRINTF("WM_MESSAGE_ACTION: object type - %d, action - %d\n", p->type, p->action);
			break;

		case WM_MESSAGE_ENC2_ROTATE:
			PRINTF("WM_MESSAGE_ENC2_ROTATE: direction - %d\n", p->action);
			break;

		case WM_MESSAGE_KEYB_CODE:
			PRINTF("WM_MESSAGE_KEYB_CODE: code - %d\n", p->action);
			break;

		default:
			PRINTF("unknown message type! - %d\n", p->message);
			break;
		}
	}
}

// WM_MESSAGE_ACTION: 		obj_type_t type, uintptr_t obj_ptr, int action, char * name
// WM_MESSAGE_ENC2_ROTATE:  int_fast8_t rotate
// WM_MESSAGE_KEYB_CODE:	int_fast8_t keyb_code
// WM_MESSAGE_UPDATE: 		nothing
// WM_MESSAGE_CLOSE: 		nothing
uint_fast8_t put_to_wm_queue(window_t * win, wm_message_t message, ...)
{
	if (win->queue_size >= WM_MAX_QUEUE_SIZE)
		return 0;					// очередь переполнена, ошибка

	//dump_queue(win);

	va_list arg;
	wm_data_t * lastq;
	wm_data_t * newq = (wm_data_t *) calloc(1, sizeof(wm_data_t));

	if (IsListEmpty(& win->queue_head))
		lastq = & wm_message_default;
	else
		lastq = CONTAINING_RECORD(& win->queue_head.Blink, wm_data_t, list_entry);

	switch (message)
	{
	case WM_MESSAGE_ACTION:
	{
		va_start(arg, message);

		uint32_t type = va_arg(arg, uint32_t);
		int32_t action = va_arg(arg, int32_t);
		char * name = va_arg(arg, char *);

		va_end(arg);

		if (lastq->message == WM_MESSAGE_ACTION && lastq->type == type && lastq->action == action)
			return 1;
		else
		{
			newq->message = WM_MESSAGE_ACTION;
			newq->type = (obj_type_t) type;
			newq->action = action;
			strncpy(newq->name, name, NAME_ARRAY_SIZE - 1);
			win->queue_size ++;
			InsertTailList(& win->queue_head, & newq->list_entry);
		}

		return 1;
	}
		break;

	case WM_MESSAGE_ENC2_ROTATE:
	{
		va_start(arg, message);
		int32_t r = va_arg(arg, int32_t);
		va_end(arg);

		// если первое в очереди сообщение - WM_MESSAGE_ENC2_ROTATE,
		// просуммировать текущее и новое значения поворота,
		// иначе добавить новое сообщение
		if (lastq->message == WM_MESSAGE_ENC2_ROTATE)
			lastq->action += r;
		else
		{
			newq->message = WM_MESSAGE_ENC2_ROTATE;
			newq->type = (obj_type_t) UINT8_MAX;
			newq->action = r;
			win->queue_size ++;
			InsertTailList(& win->queue_head, & newq->list_entry);
		}

		return 1;
	}
		break;

	case WM_MESSAGE_KEYB_CODE:
	{
		va_start(arg, message);

		newq->message = WM_MESSAGE_KEYB_CODE;
		newq->type = (obj_type_t) UINT8_MAX;
		newq->action = va_arg(arg, int32_t);
		win->queue_size ++;
		InsertTailList(& win->queue_head, & newq->list_entry);

		va_end(arg);

		return 1;
	}
		break;

	case WM_MESSAGE_UPDATE:
	{
		if (lastq->message != WM_MESSAGE_UPDATE)		// предотвращение дублей сообщения WM_MESSAGE_UPDATE
		{
			newq->message = WM_MESSAGE_UPDATE;
			newq->type = (obj_type_t) UINT8_MAX;
			newq->action = INT8_MAX;
			win->queue_size ++;
			InsertTailList(& win->queue_head, & newq->list_entry);
		}
	}
		return 1;
		break;

	case WM_MESSAGE_CLOSE:
	{
		if (lastq->message != WM_MESSAGE_CLOSE)		// предотвращение дублей сообщения WM_MESSAGE_CLOSE
		{
			newq->message = WM_MESSAGE_CLOSE;
			newq->type = (obj_type_t) UINT8_MAX;
			newq->action = INT8_MAX;
			win->queue_size ++;
			InsertTailList(& win->queue_head, & newq->list_entry);
		}
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

wm_message_t get_from_wm_queue(uint8_t win_id, uint8_t * type, int8_t * action, char * name)
{
	window_t * win = get_win(win_id);
	wm_data_t * q;
	uint8_t need_free = 0;

	if (! win->queue_size)
		return WM_NO_MESSAGE;										// очередь сообщений пустая

	win->queue_size --;

	if (IsListEmpty(& win->queue_head))
		q = & wm_message_default;
	else
	{
		PLIST_ENTRY t = RemoveTailList(& win->queue_head);
		q = CONTAINING_RECORD(t, wm_data_t, list_entry);
		need_free = 1;
	}

	* type = q->type;
	* action = q->action;

	if (q->message == WM_MESSAGE_ACTION)
	{
		char obj_name[NAME_ARRAY_SIZE] = { 0 };
		strncpy(obj_name, q->name, NAME_ARRAY_SIZE - 1);
		char * r = strrchr(obj_name, '#');
		obj_name[r - obj_name] = '\0';
		strncpy(name, obj_name, NAME_ARRAY_SIZE - 1);
	}

	wm_message_t m = q->message;
	if (need_free) free(q);

	return m;
}

void clean_wm_queue (window_t * win)
{
	win->queue_size = 0;
	if (IsListEmpty(& win->queue_head)) return;

	for (PLIST_ENTRY t = win->queue_head.Blink; t != & win->queue_head; t = t->Blink)
	{
		wm_data_t * q = CONTAINING_RECORD(t, wm_data_t, list_entry);
		free(q);
	}
}

/* Запрос на обновление состояния элементов GUI */
void gui_update(void)
{
	put_to_wm_queue(get_win(WINDOW_MAIN), WM_MESSAGE_UPDATE);	// главное окно всегда нужно обновлять

	uint_fast8_t win2 = get_parent_window();

	if (win2 != NO_PARENT_WINDOW)								// если открыто второе окно,
		put_to_wm_queue(get_win(win2), WM_MESSAGE_UPDATE);		// добавить сообщение на обновление в его очередь
}

/* Получить относительные координаты перемещения точки касания экрана */
void get_gui_tracking(int_fast16_t * x, int_fast16_t * y)
{
	* x = gui.vector_move_x;
	* y = gui.vector_move_y;

	gui.vector_move_x = 0;
	gui.vector_move_y = 0;
}

/* Возврат ссылки на запись в структуре по названию и типу окна */
void * find_gui_obj(obj_type_t type, window_t * win, const char * name)
{
	ASSERT(win);
	ASSERT(name);

	char obj_name[NAME_ARRAY_SIZE] = { 0 };
	local_snprintf_P(obj_name, NAME_ARRAY_SIZE, "%s#%02d", name, win->window_id);

	switch (type)
	{
	case TYPE_BUTTON:
		for (uint_fast8_t i = 0; i < win->bh_count; i ++)
		{
			button_t * bh = & win->bh_ptr[i];
			if (! strcmp(bh->name, obj_name))
				return (button_t *) bh;
		}
		goto not_found;
		break;

	case TYPE_LABEL:
		for (uint_fast8_t i = 0; i < win->lh_count; i ++)
		{
			label_t * lh = & win->lh_ptr[i];
			if (! strcmp(lh->name, obj_name))
				return (label_t *) lh;
		}
		goto not_found;
		break;

	case TYPE_SLIDER:
		for (uint_fast8_t i = 0; i < win->sh_count; i ++)
		{
			slider_t * sh = & win->sh_ptr[i];
			if (! strcmp(sh->name, obj_name))
				return (slider_t *) sh;
		}
		goto not_found;
		break;

	case TYPE_TOUCH_AREA:
		for (uint_fast8_t i = 0; i < win->ta_count; i ++)
		{
			touch_area_t * ta = & win->ta_ptr[i];
			if (! strcmp(ta->name, obj_name))
				return (touch_area_t *) ta;
		}
		goto not_found;
		break;

	case TYPE_TEXT_FIELD:
		for (uint_fast8_t i = 0; i < win->tf_count; i ++)
		{
			text_field_t * tf = & win->tf_ptr[i];
			if (! strcmp(tf->name, obj_name))
				return (text_field_t *) tf;
		}
		goto not_found;
		break;

	default:
		PRINTF("%s: undefined type %d\n", __func__, type);
		ASSERT(0);
		return NULL;
	}

not_found:
	if (win->window_id == WINDOW_MAIN)
	{
		PRINTF("%s: object '%s' not found\n", __func__, name);
		ASSERT(0);
		return NULL;
	}
	else
		return find_gui_obj(type, get_win(WINDOW_MAIN), name);
}

const gxdrawb_t * gui_get_drawbuf(void)
{
	return gui.gdb;
}

/* Установка статуса элементов после инициализации */
void objects_state (window_t * win)
{
	ASSERT(win != NULL);
	int debug_num = 0;

	PLIST_ENTRY current_entry = gui_objects_list.Flink;
	while (current_entry != & gui_objects_list)
	{
		PLIST_ENTRY next_entry = current_entry->Flink; // Сохраняем следующий элемент, так как текущий может быть удален
		gui_object_t * obj = CONTAINING_RECORD(current_entry, gui_object_t, list_entry);
		if (obj->win == win) {
			RemoveEntryList(& obj->list_entry);
			free(obj);
			gui_object_count--;
		}
		current_entry = next_entry;
	}

	button_t * b = win->bh_ptr;
	if (b != NULL)
	{
		for (uint_fast8_t i = 0; i < win->bh_count; i ++)
		{
			button_t * bh = & b[i];
			if (win->state)
			{
				if (bh->is_long_press && bh->is_repeating)
				{
					PRINTF("ERROR: invalid combination of properties 'is_long_press' and 'is_repeating' on button %s\n", bh->name);
					ASSERT(0);
				}
				gui_object_t * new_obj = (gui_object_t *) calloc(1, sizeof(gui_object_t));
				GUI_MEM_ASSERT(new_obj);

				new_obj->link = bh;
				new_obj->win = win;
				new_obj->type = TYPE_BUTTON;

				InsertTailList(& gui_objects_list, & new_obj->list_entry);
				gui_object_count ++;
				debug_num ++;
			}
			else
			{
				debug_num --;
				bh->visible = NON_VISIBLE;
				ASSERT(gui_object_count >= gui.footer_buttons_count);
			}
		}
	}

	label_t * l = win->lh_ptr;
	if(l != NULL)
	{
		for (uint_fast8_t i = 0; i < win->lh_count; i ++)
		{
			label_t * lh = & l[i];
			if (win->state)
			{
				gui_object_t * new_obj = (gui_object_t *) calloc(1, sizeof(gui_object_t));
				GUI_MEM_ASSERT(new_obj);

				new_obj->link = lh;
				new_obj->win = win;
				new_obj->type = TYPE_LABEL;

				InsertTailList(& gui_objects_list, & new_obj->list_entry);
				gui_object_count ++;
				debug_num ++;
			}
			else
			{
				debug_num --;
				lh->visible = NON_VISIBLE;
				ASSERT(gui_object_count >= gui.footer_buttons_count);
			}
		}
	}

	slider_t * s = win->sh_ptr;
	if(s != NULL)
	{
		for (uint_fast8_t i = 0; i < win->sh_count; i ++)
		{
			slider_t * sh = & s[i];
			if (win->state)
			{
				gui_object_t * new_obj = (gui_object_t *) calloc(1, sizeof(gui_object_t));
				GUI_MEM_ASSERT(new_obj);

				new_obj->link = (slider_t *) sh;
				new_obj->win = win;
				new_obj->type = TYPE_SLIDER;

				InsertTailList(& gui_objects_list, & new_obj->list_entry);
				gui_object_count ++;
				debug_num ++;
			}
			else
			{
				debug_num --;
				sh->visible = NON_VISIBLE;
				ASSERT(gui_object_count >= gui.footer_buttons_count);
			}
		}
	}

	touch_area_t * t = win->ta_ptr;
	if(t != NULL)
	{
		for (uint_fast8_t i = 0; i < win->ta_count; i ++)
		{
			touch_area_t * ta = & t[i];
			if (win->state)
			{
				gui_object_t * new_obj = (gui_object_t *) calloc(1, sizeof(gui_object_t));
				GUI_MEM_ASSERT(new_obj);

				new_obj->link = (touch_area_t *) ta;
				new_obj->win = win;
				new_obj->type = TYPE_TOUCH_AREA;

				InsertTailList(& gui_objects_list, & new_obj->list_entry);
				gui_object_count ++;
				debug_num ++;
			}
			else
			{
				debug_num --;
				ta->visible = NON_VISIBLE;
				ASSERT(gui_object_count >= gui.footer_buttons_count);
			}
		}
	}

	text_field_t * tf = win->tf_ptr;
	if(tf != NULL)
	{
		for (uint_fast8_t i = 0; i < win->tf_count; i ++)
		{
			text_field_t * tff = & tf[i];
			if (win->state)
			{
				gui_object_t * new_obj = (gui_object_t *) calloc(1, sizeof(gui_object_t));
				GUI_MEM_ASSERT(new_obj);

				new_obj->link = (text_field_t *) tff;
				new_obj->win = win;
				new_obj->type = TYPE_TEXT_FIELD;

				InsertTailList(& gui_objects_list, & new_obj->list_entry);
				gui_object_count ++;
				debug_num ++;

				tff->string = (tf_entry_t *) calloc(tff->h_str, sizeof(tf_entry_t));
				GUI_MEM_ASSERT(tff->string);
				tff->index = 0;
			}
			else
			{
				debug_num --;
				tff->visible = NON_VISIBLE;
				free(tff->string);
				tff->string = NULL;
				ASSERT(gui_object_count >= gui.footer_buttons_count);
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

			gui_object_t * new_obj = (gui_object_t *) calloc(1, sizeof(gui_object_t));
			GUI_MEM_ASSERT(new_obj);

			new_obj->link = (button_t *) & close_button;
			new_obj->win = win;
			new_obj->type = TYPE_CLOSE_BUTTON;

			InsertTailList(& gui_objects_list, & new_obj->list_entry);
			gui_object_count ++;
			debug_num ++;
		}
		else
		{
			debug_num --;
			close_button.visible = NON_VISIBLE;
			ASSERT(gui_object_count >= gui.footer_buttons_count);
		}
	}
//	PRINTF("line %d: %s gui_object_count: %d %+d\n", __LINE__, win->title, gui_object_count, debug_num);
	clean_wm_queue(win);
}

/* Передать менеджеру GUI код нажатой кнопки на клавиатуре */
void gui_put_keyb_code (uint_fast8_t kbch)
{
	// перенаправить код нажатой аппаратной кнопки в активное окно
	if (get_parent_window() == NO_PARENT_WINDOW)
		put_to_wm_queue(get_win(WINDOW_MAIN), WM_MESSAGE_KEYB_CODE, kbch);
	else
		put_to_wm_queue(get_win(gui.win[1]), WM_MESSAGE_KEYB_CODE, kbch);
}

/* Отрисовка слайдера */
static void draw_slider(slider_t * sl)
{
	window_t * win = get_win(sl->parent);

	if (sl->orientation)		// ORIENTATION_HORIZONTAL
	{
		if (sl->value_old != sl->value)
		{
			uint_fast16_t mid_w = sl->y + sliders_scale_thickness / 2;
			sl->value_p = sl->x + sl->size * sl->value / 100;
			sl->y1_p = mid_w - sliders_w;
			sl->x1_p = sl->value_p - sliders_h;
			sl->y2_p = mid_w + sliders_w;
			sl->x2_p = sl->value_p + sliders_h;
			sl->value_old = sl->value;
		}
		colpip_rect(gui.gdb, win->x1 + sl->x, win->y1 + sl->y,  win->x1 + sl->x + sl->size, win->y1 + sl->y + sliders_scale_thickness, COLORPIP_BLACK, 1);
		colpip_rect(gui.gdb, win->x1 + sl->x, win->y1 + sl->y,  win->x1 + sl->x + sl->size, win->y1 + sl->y + sliders_scale_thickness, COLORPIP_WHITE, 0);
		colpip_rect(gui.gdb, win->x1 + sl->x1_p, win->y1 + sl->y1_p,  win->x1 + sl->x2_p, win->y1 + sl->y2_p, sl->state == PRESSED ? COLOR_BUTTON_PR_NON_LOCKED : COLOR_BUTTON_NON_LOCKED, 1);
		colpip_line(gui.gdb, win->x1 + sl->value_p, win->y1 + sl->y1_p,  win->x1 + sl->value_p, win->y1 + sl->y2_p, COLORPIP_WHITE, 0);
	}
	else						// ORIENTATION_VERTICAL
	{
		if (sl->value_old != sl->value)
		{
			uint_fast16_t mid_w = sl->x + sliders_scale_thickness / 2;
			sl->value_p = sl->y + sl->size * sl->value / 100;
			sl->x1_p = mid_w - sliders_w;
			sl->y1_p = sl->value_p - sliders_h;
			sl->x2_p = mid_w + sliders_w;
			sl->y2_p = sl->value_p + sliders_h;
			sl->value_old = sl->value;
		}
		colpip_rect(gui.gdb, win->x1 + sl->x + 1, win->y1 + sl->y + 1, win->x1 + sl->x + sliders_scale_thickness - 1, win->y1 + sl->y + sl->size - 1, COLORPIP_BLACK, 1);
		colpip_rect(gui.gdb, win->x1 + sl->x, win->y1 + sl->y, win->x1 + sl->x + sliders_scale_thickness, win->y1 + sl->y + sl->size, COLORPIP_WHITE, 0);
		colpip_rect(gui.gdb, win->x1 + sl->x1_p, win->y1 + sl->y1_p, win->x1 + sl->x2_p, win->y1 + sl->y2_p, sl->state == PRESSED ? COLOR_BUTTON_PR_NON_LOCKED : COLOR_BUTTON_NON_LOCKED, 1);
		colpip_line(gui.gdb, win->x1 + sl->x1_p, win->y1 + sl->value_p, win->x1 + sl->x2_p, win->y1 + sl->value_p, COLORPIP_WHITE, 0);
	}
}

/* Системный обработчик слайдера в момент его перемещения */
static void slider_process(slider_t * sl)
{
	int v = sl->value + roundf((sl->orientation ? gui.vector_move_x : gui.vector_move_y) / sl->step);
	if (v >= 0 && v <= sl->size / sl->step)
		sl->value = v;

	gui.vector_move_x = 0;
	gui.vector_move_y = 0;
}

static void fill_button_bg_buf(btn_bg_t * v)
{
	const uint_fast16_t w = v->w;
	const uint_fast16_t h = v->h;
	const size_t s = GXSIZE(w, h) * sizeof (PACKEDCOLORPIP_T);

	v->bg_non_pressed = 	(PACKEDCOLORPIP_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_non_pressed);
	v->bg_pressed = 		(PACKEDCOLORPIP_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_pressed);
	v->bg_locked = 			(PACKEDCOLORPIP_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_locked);
	v->bg_locked_pressed = 	(PACKEDCOLORPIP_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_locked_pressed);
	v->bg_disabled = 		(PACKEDCOLORPIP_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_disabled);

	{
		gxdrawb_t butdbv;
		gxdrawb_initialize(& butdbv, v->bg_non_pressed, w, h);
	#if GUI_OLDBUTTONSTYLE
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLOR_BUTTON_NON_LOCKED, 1);
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(& butdbv, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);
	#else
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, GUI_DEFAULTCOLOR, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_NON_LOCKED, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLORPIP_GRAY, 0);
		colmain_rounded_rect(& butdbv, 2, 2, w - 3, h - 3, button_round_radius, COLORPIP_BLACK, 0);
	#endif /* GUI_OLDBUTTONSTYLE */
	}

	{
		gxdrawb_t butdbv;
		gxdrawb_initialize(& butdbv, v->bg_pressed, w, h);
	#if GUI_OLDBUTTONSTYLE
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLOR_BUTTON_PR_NON_LOCKED, 1);
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_line(& butdbv, 2, 3, w - 3, 3, COLORPIP_BLACK, 0);
		colpip_line(& butdbv, 2, 2, w - 3, 2, COLORPIP_BLACK, 0);
		colpip_line(& butdbv, 3, 3, 3, h - 3, COLORPIP_BLACK, 0);
		colpip_line(& butdbv, 2, 2, 2, h - 2, COLORPIP_BLACK, 0);
	#else
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, GUI_DEFAULTCOLOR, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_PR_NON_LOCKED, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLORPIP_GRAY, 0);
		colmain_rounded_rect(& butdbv, 2, 2, w - 3, h - 3, button_round_radius, COLORPIP_BLACK, 0);
	#endif /* GUI_OLDBUTTONSTYLE */
	}

	{
		gxdrawb_t butdbv;
		gxdrawb_initialize(& butdbv, v->bg_locked, w, h);
	#if GUI_OLDBUTTONSTYLE
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLOR_BUTTON_LOCKED, 1);
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(& butdbv, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);
	#else
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, GUI_DEFAULTCOLOR, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_LOCKED, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLORPIP_GRAY, 0);
		colmain_rounded_rect(& butdbv, 2, 2, w - 3, h - 3, button_round_radius, COLORPIP_BLACK, 0);
	#endif /* GUI_OLDBUTTONSTYLE */
	}

	{
		gxdrawb_t butdbv;
		gxdrawb_initialize(& butdbv, v->bg_locked_pressed, w, h);
	#if GUI_OLDBUTTONSTYLE
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLOR_BUTTON_PR_LOCKED, 1);
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_line(& butdbv, 2, 3, w - 3, 3, COLORPIP_BLACK, 0);
		colpip_line(& butdbv, 2, 2, w - 3, 2, COLORPIP_BLACK, 0);
		colpip_line(& butdbv, 3, 3, 3, h - 3, COLORPIP_BLACK, 0);
		colpip_line(& butdbv, 2, 2, 2, h - 2, COLORPIP_BLACK, 0);
	#else
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, GUI_DEFAULTCOLOR, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_PR_LOCKED, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLORPIP_GRAY, 0);
		colmain_rounded_rect(& butdbv, 2, 2, w - 3, h - 3, button_round_radius, COLORPIP_BLACK, 0);
	#endif /* GUI_OLDBUTTONSTYLE */
	}

	{
		gxdrawb_t butdbv;
		gxdrawb_initialize(& butdbv, v->bg_disabled, w, h);
	#if GUI_OLDBUTTONSTYLE
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLOR_BUTTON_DISABLED, 1);
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(& butdbv, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);
	#else
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, GUI_DEFAULTCOLOR, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_DISABLED, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLORPIP_GRAY, 0);
		colmain_rounded_rect(& butdbv, 2, 2, w - 3, h - 3, button_round_radius, COLORPIP_BLACK, 0);
	#endif /* GUI_OLDBUTTONSTYLE */
	}
}

static void draw_button(const button_t * const bh)
{
	PACKEDCOLORPIP_T * bg = NULL;
	window_t * win = get_win(bh->parent);
	const gxdrawb_t * gdb = gui_get_drawbuf();
	uint_fast8_t i = 0;
	static const char delimeters[] = "|";
	uint_fast16_t x1 = win->x1 + bh->x1;
	uint_fast16_t y1 = win->y1 + bh->y1;

	if ((x1 + bh->w >= WITHGUIMAXX) || (y1 + bh->h >= WITHGUIMAXY))
	{
		PRINTF("%s %s x+w: %d y+h: %d\n", bh->name, bh->text, x1 + bh->w, y1 + bh->h);
		ASSERT(0);
	}

	btn_bg_t * b1 = NULL;
	do {
		if (bh->h == btn_bg[i].h && bh->w == btn_bg[i].w)
		{
			b1 = & btn_bg[i];
			break;
		}
	} while ( ++i < BG_COUNT);

	// если не найден заполненный буфер фона по размерам, программная отрисовка
	if (b1 == NULL)
	{
		PACKEDCOLORPIP_T c1, c2;
		c1 = bh->state == DISABLED ? COLOR_BUTTON_DISABLED : (bh->is_locked ? COLOR_BUTTON_LOCKED : COLOR_BUTTON_NON_LOCKED);
		c2 = bh->state == DISABLED ? COLOR_BUTTON_DISABLED : (bh->is_locked ? COLOR_BUTTON_PR_LOCKED : COLOR_BUTTON_PR_NON_LOCKED);

		colpip_rectangle(gdb, x1 + 1, y1 + 1, bh->w - 1, bh->h - 1, GUI_DEFAULTCOLOR, FILL_FLAG_NONE);
		colmain_rounded_rect(gdb, x1, y1, x1 + bh->w, y1 + bh->h - 2, button_round_radius, bh->state == PRESSED ? c2 : c1, 1);
		colmain_rounded_rect(gdb, x1, y1, x1 + bh->w, y1 + bh->h - 1, button_round_radius, COLORPIP_GRAY, 0);
		colmain_rounded_rect(gdb, x1 + 2, y1 + 2, x1 + bh->w - 2, y1 + bh->h - 3, button_round_radius, COLORPIP_BLACK, 0);
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
		ASSERT(bg != NULL);

		gxdrawb_t bgv;
		gxdrawb_initialize(& bgv, bg, bh->w, bh->h);
		colpip_bitblt(
				gdb->cachebase, gdb->cachesize,	// cache parameters
				gdb, 	// target window
				x1, y1,	// target position
				bgv.cachebase, bgv.cachesize, 	// cache parameters
				& bgv, 0, 0, bh->w, bh->h, BITBLT_FLAG_NONE, 0);
	}

	const uint_fast16_t shiftX = bh->state == PRESSED ? 1 : 0;
	const uint_fast16_t shiftY = bh->state == PRESSED ? 1 : 0;
	const COLORPIP_T textcolor = COLORPIP_BLACK;

	if (strchr(bh->text, delimeters[0]) == NULL)
	{
		/* Однострочная надпись */
#if WITHALTERNATIVEFONTS
		UB_Font_DrawPString(gdb, shiftX + x1 + (bh->w - (getwidth_Pstring(bh->text, & FONT_BUTTONS))) / 2,
				shiftY + y1 + (bh->h - FONT_BUTTONS.height) / 2, bh->text, & FONT_BUTTONS, textcolor);
#else
		colpip_string2_tbg(gdb, shiftX + x1 + (bh->w - (strwidth2(bh->text))) / 2,
				shiftY + y1 + (bh->h - SMALLCHARH2) / 2, bh->text, textcolor);
#endif /* WITHALTERNATIVEFONTS */
	}
	else
	{
		char * next;
		/* Двухстрочная надпись */
		uint_fast8_t j = (bh->h - SMALLCHARH2 * 2) / 2;
		char buf[TEXT_ARRAY_SIZE];
		strcpy(buf, bh->text);
		char * text2 = strtok_r(buf, delimeters, & next);
#if WITHALTERNATIVEFONTS
		UB_Font_DrawPString(gdb,
				shiftX + x1 + (bh->w - (getwidth_Pstring(text2, & FONT_BUTTONS))) / 2,
				shiftY + y1 + j,
				text2, & FONT_BUTTONS, textcolor
				);

		text2 = strtok_r(NULL, delimeters, & next);
		UB_Font_DrawPString(gdb,
				shiftX + x1 + (bh->w - (getwidth_Pstring(text2, & FONT_BUTTONS))) / 2,
				shiftY + bh->h + y1 - FONT_BUTTONS.height - j,
				text2, & FONT_BUTTONS, textcolor
				);
#else
		colpip_string2_tbg(gdb, shiftX + x1 + (bh->w - (strwidth2(text2))) / 2,
				shiftY + y1 + j, text2, COLORPIP_BLACK);

		text2 = strtok_r(NULL, delimeters, & next);
		colpip_string2_tbg(gdb, shiftX + x1 + (bh->w - (strwidth2(text2))) / 2,
				shiftY + bh->h + y1 - SMALLCHARH2 - j, text2, COLORPIP_BLACK);
#endif /* WITHALTERNATIVEFONTS */
	}
}

static void objects_init(void)
{
	// Buttons background init
	for (int i = 0; i < BG_COUNT; i ++)
		fill_button_bg_buf(& btn_bg[i]);
}

/* Инициализация GUI */
void gui_initialize (void)
{
	InitializeListHead(& gui_objects_list);

	window_t * win = get_win(WINDOW_MAIN);
	win->x1 = 0;
	win->y1 = 0;
	win->w = WITHGUIMAXX - 1;
	win->h = WITHGUIMAXY - FOOTER_HEIGHT - 1;
	open_window(win);
	gui.win[1] = NO_PARENT_WINDOW;
	gui.footer_buttons_count = win->bh_count;
	objects_init();
}

/* Обновление данных в списке элементов открытых окон */
static void update_gui_objects_list(void)
{
	PLIST_ENTRY current_entry = gui_objects_list.Flink;
	while (current_entry != & gui_objects_list)
	{
		gui_object_t * p = CONTAINING_RECORD(current_entry, gui_object_t, list_entry);

		if (p->type == TYPE_BUTTON || p->type == TYPE_CLOSE_BUTTON)
		{
			button_t * bh = (button_t *) p->link;
			p->x1 = (bh->x1 - touch_area_enlarge) < 0 ? 0 : (bh->x1 - touch_area_enlarge);
			p->x2 = (bh->x1 + bh->w + touch_area_enlarge) > WITHGUIMAXX ? WITHGUIMAXX : (bh->x1 + bh->w + touch_area_enlarge);
			p->y1 = (bh->y1 - touch_area_enlarge) < 0 ? 0 : (bh->y1 - touch_area_enlarge);
			p->y2 = (bh->y1 + bh->h + touch_area_enlarge) > WITHGUIMAXY ? WITHGUIMAXY : (bh->y1 + bh->h + touch_area_enlarge);
			p->state = bh->state;
			p->visible = bh->visible;
			p->is_trackable = 0;
			p->is_long_press = bh->is_long_press;
			p->is_repeating = bh->is_repeating;
		}
		else if (p->type == TYPE_LABEL)
		{
			label_t * lh = (label_t *) p->link;
			p->x1 = (lh->x - touch_area_enlarge) < 0 ? 0 : (lh->x - touch_area_enlarge);
			p->x2 = (lh->x + get_label_width(lh) + touch_area_enlarge) > WITHGUIMAXX ? WITHGUIMAXX : (lh->x + get_label_width(lh) + touch_area_enlarge);
			p->y1 = (lh->y - get_label_height(lh) - touch_area_enlarge) < 0 ? 0 : (lh->y - get_label_height(lh) - touch_area_enlarge);
			p->y2 = (lh->y + get_label_height(lh) * 2 + touch_area_enlarge) > WITHGUIMAXY ? WITHGUIMAXY : (lh->y + get_label_height(lh) * 2 + touch_area_enlarge);
			p->state = lh->state;
			p->visible = lh->visible;
			p->is_trackable = lh->is_trackable;
			p->is_long_press = 0;
			p->is_repeating = 0;
		}
		else if (p->type == TYPE_SLIDER)
		{
			slider_t * sh = (slider_t *) p->link;
			p->x1 = (sh->x1_p - touch_area_enlarge) < 0 ? 0 : (sh->x1_p - touch_area_enlarge);
			p->x2 = (sh->x2_p + touch_area_enlarge) > WITHGUIMAXX ? WITHGUIMAXX : (sh->x2_p + touch_area_enlarge);
			p->y1 = (sh->y1_p - touch_area_enlarge) < 0 ? 0 : (sh->y1_p - touch_area_enlarge);
			p->y2 = (sh->y2_p + touch_area_enlarge) > WITHGUIMAXY ? WITHGUIMAXY : (sh->y2_p + touch_area_enlarge);
			p->state = sh->state;
			p->visible = sh->visible;
			p->is_trackable = 1;
			p->is_long_press = 0;
			p->is_repeating = 0;
		}
		else if (p->type == TYPE_TOUCH_AREA)
		{
			touch_area_t * ta = (touch_area_t *) p->link;
			p->x1 = (ta->x1) < 0 ? 0 : (ta->x1);
			p->x2 = (ta->x1 + ta->w) > WITHGUIMAXX ? WITHGUIMAXX : (ta->x1 + ta->w);
			p->y1 = (ta->y1) < 0 ? 0 : (ta->y1);
			p->y2 = (ta->y1 + ta->h) > WITHGUIMAXY ? WITHGUIMAXY : (ta->y1 + ta->h);
			p->state = ta->state;
			p->visible = ta->visible;
			p->is_trackable = ta->is_trackable;
			p->is_long_press = 0;
			p->is_repeating = 0;
		}
		else if (p->type == TYPE_TEXT_FIELD)
		{
			text_field_t * tf = (text_field_t *) p->link;
			p->x1 = (tf->x1) < 0 ? 0 : (tf->x1);
			p->x2 = (tf->x1 + tf->w) > WITHGUIMAXX ? WITHGUIMAXX : (tf->x1 + tf->w);
			p->y1 = (tf->y1) < 0 ? 0 : (tf->y1);
			p->y2 = (tf->y1 + tf->h) > WITHGUIMAXY ? WITHGUIMAXY : (tf->y1 + tf->h);
			p->state = tf->state;
			p->visible = tf->visible;
			p->is_trackable = 0;
			p->is_long_press = 0;
			p->is_repeating = 0;
		}
		current_entry = current_entry->Flink; // Переход к следующему элементу
	}
}

/* Селектор запуска функций обработки событий */
static void set_state_record(gui_object_t * val)
{
	ASSERT(val != NULL);
	switch (val->type)
	{
		case TYPE_CLOSE_BUTTON:
			close_all_windows();
			break;

		case TYPE_BUTTON:
		{
			ASSERT(val->link != NULL);
			button_t * bh = (button_t *) val->link;
			gui.selected_type = TYPE_BUTTON;
			gui.selected_link = val;
			bh->state = val->state;
			if (bh->state == RELEASED || bh->state == LONG_PRESSED || bh->state == PRESS_REPEATING)
			{
				if (! put_to_wm_queue(val->win, WM_MESSAGE_ACTION, TYPE_BUTTON, bh->state == LONG_PRESSED ? LONG_PRESSED : PRESSED, bh->name))
					dump_queue(val->win);
			}
		}
			break;

		case TYPE_LABEL:
		{
			ASSERT(val->link != NULL);
			label_t * lh = (label_t *) val->link;
			gui.selected_type = TYPE_LABEL;
			gui.selected_link = val;
			lh->state = val->state;
			if (lh->state == RELEASED)
			{
				if (! put_to_wm_queue(val->win, WM_MESSAGE_ACTION, TYPE_LABEL, PRESSED, lh->name))
					dump_queue(val->win);
			}
			else if (lh->state == PRESSED && lh->is_trackable)
			{
				if (! put_to_wm_queue(val->win, WM_MESSAGE_ACTION, TYPE_LABEL, MOVING, lh->name))
					dump_queue(val->win);
			}
		}
			break;

		case TYPE_SLIDER:
		{
			ASSERT(val->link != NULL);
			slider_t * sh = (slider_t *) val->link;
			gui.selected_type = TYPE_SLIDER;
			gui.selected_link = val;
			sh->state = val->state;
			if (sh->state == PRESSED)
			{
				slider_process(sh);
				if (! put_to_wm_queue(val->win, WM_MESSAGE_ACTION, TYPE_SLIDER, PRESSED, sh->name))
					dump_queue(val->win);
			}
		}
			break;

		case TYPE_TOUCH_AREA:
		{
			ASSERT(val->link != NULL);
			touch_area_t * ta = (touch_area_t *) val->link;
			gui.selected_type = TYPE_TOUCH_AREA;
			gui.selected_link = val;
			ta->state = val->state;
			if (ta->state == RELEASED)
			{
				if (! put_to_wm_queue(val->win, WM_MESSAGE_ACTION, TYPE_TOUCH_AREA, PRESSED, ta->name))
					dump_queue(val->win);
			}
			else if (ta->state == PRESSED && ta->is_trackable)
			{
				if (! strcmp(ta->name, "ta_winmove"))
				{
					move_window(val->win, gui.vector_move_x, gui.vector_move_y);
					gui.vector_move_x = 0;
					gui.vector_move_y = 0;
				}
				else if (! put_to_wm_queue(val->win, WM_MESSAGE_ACTION, TYPE_TOUCH_AREA, MOVING, ta->name))
					dump_queue(val->win);
			}
		}
			break;

		case TYPE_TEXT_FIELD:
			break;

		default:
		{
			PRINTF("set_state_record: undefined type\n");
			ASSERT(0);
		}
			break;
	}
}

// 10 ms non-blocking delay
uint8_t lp_delay_10ms(uint8_t init)
{
#if LINUX_SUBSYSTEM
	static uint32_t oldt = 0;

	if (init)
	{
		oldt = sys_now();
		return 0;
	}
	else
	{
		uint32_t t = sys_now();
		if (t - oldt > 10)
		{
			oldt = t;
			return 1;
		}
		return 0;
	}
#else
	return 1;
#endif /* LINUX_SUBSYSTEM */
}

/* GUI state mashine */
static void process_gui(void)
{
	uint_fast16_t tx, ty;
	static uint_fast16_t x_old = 0, y_old = 0, long_press_counter = 0;
	static gui_object_t * p = NULL;
	static window_t * w = NULL;
	const uint_fast8_t long_press_limit = 20;
	static uint_fast8_t is_long_press = 0;		// 1 - долгое нажатие уже обработано
	static uint_fast8_t is_repeating = 0, repeating_cnt = 0;

	if (board_tsc_getxy(& tx, & ty))
	{
		gui.last_pressed_x = tx;
		gui.last_pressed_y = ty;
		gui.is_touching_screen = 1;
//		PRINTF("last x/y=%d/%d\n", gui.last_pressed_x, gui.last_pressed_y);
		update_gui_objects_list();
	}
	else
	{
		gui.is_touching_screen = 0;
		gui.is_after_touch = 0;
	}


	if (IsListEmpty(& gui_objects_list))
		return;

	if (gui.state == CANCELLED && gui.is_touching_screen && ! gui.is_after_touch)
	{
		ASSERT(! IsListEmpty(& gui_objects_list));

		PLIST_ENTRY current_entry = gui_objects_list.Blink; // Обход списка в обратном порядке
		while (current_entry != & gui_objects_list)
		{
			p = CONTAINING_RECORD(current_entry, gui_object_t, list_entry);
			w = p->win;
			ASSERT(w != NULL);
			uint_fast16_t x1 = p->x1 + w->x1, y1 = p->y1 + w->y1;
			uint_fast16_t x2 = p->x2 + w->x1, y2 = p->y2 + w->y1;

			if (x1 < gui.last_pressed_x && x2 > gui.last_pressed_x && y1 < gui.last_pressed_y && y2 > gui.last_pressed_y
					&& p->state != DISABLED && p->visible == VISIBLE)
			{
				gui.state = PRESSED;
				is_long_press = 0;
				is_repeating = 0;
				long_press_counter = 0;
				break;
			}
			current_entry = current_entry->Blink; // Переход к предыдущему элементу
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
			gui.vector_move_x = x_old ? gui.last_pressed_x - x_old : 0;
			gui.vector_move_y = y_old ? gui.last_pressed_y - y_old : 0;

			if (gui.vector_move_x != 0 || gui.vector_move_y != 0)
			{
				gui.is_tracking = 1;
//				PRINTF("move x: %d, move y: %d\n", gui.vector_move_x, gui.vector_move_y);
			}

			p->state = PRESSED;
			set_state_record(p);

			x_old = gui.last_pressed_x;
			y_old = gui.last_pressed_y;
		}
		else if (w->x1 + p->x1 < gui.last_pressed_x && w->x1 + p->x2 > gui.last_pressed_x &&
				 w->y1 + p->y1 < gui.last_pressed_y && w->y1 + p->y2 > gui.last_pressed_y && ! gui.is_after_touch)
		{
			if (gui.is_touching_screen)
			{
				ASSERT(p != NULL);
				p->state = PRESSED;

				if (is_repeating)
				{
					repeating_cnt ++;
					if (repeating_cnt > autorepeat_delay)
					{
						repeating_cnt = 0;
						p->state = PRESS_REPEATING;		// для запуска обработчика нажатия
						set_state_record(p);
					}
				}
				else
					set_state_record(p);

				if (p->is_long_press)
				{
					if(gui.state != LONG_PRESSED && ! is_long_press && lp_delay_10ms(0))
						long_press_counter ++;

					if(long_press_counter > long_press_limit)
					{
						long_press_counter = 0;
						gui.state = LONG_PRESSED;
					}
				}
				else if (p->is_repeating)
				{
					if (! is_repeating)
						long_press_counter ++;

					if(long_press_counter > long_press_limit)
					{
						long_press_counter = 0;
						repeating_cnt = 0;
						is_repeating = 1;
					}
				}
			}
			else
				gui.state = RELEASED;
		}
		else
		{
			ASSERT(p != NULL);
			gui.state = CANCELLED;
			p->state = CANCELLED;
			set_state_record(p);
			gui.is_after_touch = 1; 	// точка непрерывного касания вышла за пределы выбранного элемента, не поддерживающего tracking
		}
	}

	if (gui.state == RELEASED)
	{
		ASSERT(p != NULL);
		p->state = RELEASED;			// для запуска обработчика нажатия
		if(! is_long_press)				// если было долгое нажатие, обработчик по короткому не запускать
			set_state_record(p);
		p->state = CANCELLED;
		set_state_record(p);
		gui.is_after_touch = 0;
		gui.state = CANCELLED;
		gui.is_tracking = 0;
	}
	else if (gui.state == LONG_PRESSED)
	{
		p->state = LONG_PRESSED;		// для запуска обработчика нажатия
		set_state_record(p);
		lp_delay_10ms(1);				// инициализация задержки
		p->state = PRESSED;
		gui.state = PRESSED;
		is_long_press = 1;				// долгое нажатие обработано
	}
}

/* Запуск state mashine и отрисовка элементов GUI */
void gui_WM_walkthrough(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t xpan, uint_fast8_t yspan, dctx_t * pctx)
{
	uint_fast8_t alpha = DEFAULT_ALPHA; // на сколько затемнять цвета
	char buf[TEXT_ARRAY_SIZE];
	uint_fast8_t str_len = 0;
	gui.gdb = db;

	process_gui();

	for(uint_fast8_t i = 0; i < opened_windows_count; i ++)
	{
		const window_t * const win = get_win(gui.win[i]);
		uint_fast8_t f = win->first_call;

		if (win->state == VISIBLE)
		{
			if (! f) 	// при открытии окна рассчитываются экранные координаты самого окна и его child элементов
			{
				if (win->window_id != WINDOW_MAIN)
				{
					ASSERT(win->w > 0 || win->h > 0);
#if GUI_TRANSPARENT_WINDOWS
					display_transparency(gui.gdb, win->x1, strcmp(win->title, "") ? (win->y1 + window_title_height) :
							win->y1, win->x1 + win->w - 1, win->y1 + win->h - 1, alpha);
#else
					colpip_rectangle(gui.gdb, win->x1, strcmp(win->title, "") ? (win->y1 + window_title_height) :
							win->y1, win->w, win->h, GUI_WINDOWBGCOLOR, FILL_FLAG_NONE);
#endif /* GUI_TRANSPARENT_WINDOWS */
				}
			}

			win->onVisibleProcess();	// запуск процедуры фоновой обработки для окна

			if (! f)
			{
				// вывод заголовка окна
				if (strcmp(win->title, ""))
				{
					uint16_t title_lenght = strlen(win->title) * SMALLCHARW;
					uint16_t xt = 0;

					switch(win->title_align)
					{
					case TITLE_ALIGNMENT_LEFT:
						xt = win->x1 + window_title_indent;
						break;

					case TITLE_ALIGNMENT_RIGHT:
						xt = win->x1 + win->w - title_lenght - window_title_indent - (win->is_close ? window_close_button_size : 0);
						break;

					case TITLE_ALIGNMENT_CENTER:
						xt = win->x1 + win->w / 2 - title_lenght / 2;
						break;

					default:
						PRINTF("Title alignment value %d incorrect for window %s\n", win->title_align, win->title);
						ASSERT(0);
						break;
					}

					colpip_rectangle(gui.gdb, win->x1, win->y1, win->w, window_title_height, GUI_WINDOWTITLECOLOR, FILL_FLAG_NONE);
					colpip_string_tbg(gui.gdb, xt, win->y1 + 5, win->title, COLORPIP_BLACK);
				}

				// отрисовка принадлежащих окну элементов
				PLIST_ENTRY current_entry = gui_objects_list.Flink;
				while (current_entry != & gui_objects_list)
				{
					gui_object_t * p = CONTAINING_RECORD(current_entry, gui_object_t, list_entry);

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
							colpip_rect(gui.gdb, win->x1 + bh->x1, win->y1 + bh->y1,
									win->x1 + bh->x1 + bh->w,  win->y1 + bh->y1 + bh->h, COLORPIP_BLACK, 0);
							colpip_line(gui.gdb, win->x1 + bh->x1, win->y1 + bh->y1,
									win->x1 + bh->x1 + bh->w, win->y1 + bh->y1 + bh->h, COLORPIP_BLACK, 0);
							colpip_line(gui.gdb, win->x1 + bh->x1, win->y1 + bh->y1 + bh->h,
									win->x1 + bh->x1 + bh->w, win->y1 + bh->y1, COLORPIP_BLACK, 0);
						}
					}
					else if (p->type == TYPE_LABEL)
					{
						label_t * lh = (label_t *) p->link;
						if (lh->visible && lh->parent == win->window_id)
						{
							if (lh->font_size == FONT_LARGE)
								colpip_string_tbg(gui.gdb,  win->x1 + lh->x, win->y1 + lh->y, lh->text, lh->color);
							else if (lh->font_size == FONT_MEDIUM)
								colpip_string2_tbg(gui.gdb, win->x1 + lh->x, win->y1 + lh->y, lh->text, lh->color);
							else if (lh->font_size == FONT_SMALL)
								colpip_string3_tbg(gui.gdb, win->x1 + lh->x, win->y1 + lh->y, lh->text, lh->color);
						}
					}
					else if (p->type == TYPE_SLIDER)
					{
						slider_t * sh = (slider_t *) p->link;
						if (sh->visible && sh->parent == win->window_id)
							draw_slider(sh);
					}
					else if (p->type == TYPE_TEXT_FIELD)
					{
						text_field_t * tf = (text_field_t *) p->link;
						if (tf->visible && tf->parent == win->window_id)
						{
							int_fast8_t j = tf->index - 1;
							for (uint8_t i = 0; i < tf->h_str; i ++)
							{
								uint_fast8_t pos = tf->direction ? i : (tf->h_str - i - 1);
								j = j < 0 ? (tf->h_str - 1) : j;

								if (tf->font)
								{
									UB_Font_DrawString(gui.gdb, win->x1 + tf->x1, win->y1 + tf->y1 + tf->font->height * pos,
											tf->string[j].text, tf->font, tf->string[j].color_line);
								}
								else
								{
									colpip_string2_tbg(gui.gdb, win->x1 + tf->x1, win->y1 + tf->y1 + SMALLCHARH2 * pos,
											tf->string[j].text, tf->string[j].color_line);
								}

								j --;
							}
						}
					}
					current_entry = current_entry->Flink;
				}
			}
		}
	}
}

#endif /* WITHTOUCHGUI */
