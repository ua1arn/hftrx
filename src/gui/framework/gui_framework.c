/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN


#include "../gui_port_include.h"

#if WITHTOUCHGUI

#include "gui.h"
#include "gui_system.h"
#include "gui_structs.h"
#include "gui_settings.h"
#include "gui_windows.h"
#include "gui_objects.h"
#include "gui_events.h"
#include "gui_cache.h"
#include "utils.h"
#include "../gui_user.h"
#include "../gui_port.h"

static gui_t gui = { 0, 0, CANCELLED, 0, 0, 0, 0, 0, };
static LIST_ENTRY gui_objects_list;
static uint8_t gui_object_count = 0;
static button_t close_button = { 0, 0, CANCELLED, BUTTON_NON_LOCKED, 0, 0, 0, NO_PARENT_WINDOW, NON_VISIBLE, INT32_MAX, "btс_close", "", };
static uint8_t opened_windows_count = 1;
gui_drawbuf_t * drawbuf = NULL;

/* Возврат id parent window */
uint8_t get_parent_window(void)
{
	if (opened_windows_count == 2)
		return gui.win[1];
	else
		return NO_PARENT_WINDOW;
}

uint8_t get_current_drawing_window(void)
{
	return gui.current_drawing_window;
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
	if (! win->queue.size)
		return;

	PRINTF("dump WM queue window '%s'\n", win->title);

	if (win->queue.size == WM_MAX_QUEUE_SIZE)
		PRINTF("WM stack full!\n");

	for (uint8_t i = 0; i < win->queue.size; i ++)
	{
		switch(win->queue.data[i].message)
		{
		case WM_MESSAGE_UPDATE:
			PRINTF("%d: WM_MESSAGE_UPDATE\n", i);
			break;

		case WM_MESSAGE_ACTION:
			PRINTF("%d: WM_MESSAGE_ACTION: object type - %d, action - %d\n", i, (int) win->queue.data[i].type, (int) win->queue.data[i].action);
			break;

		case WM_MESSAGE_ENC2_ROTATE:
			PRINTF("%d: WM_MESSAGE_ENC2_ROTATE: direction - %d\n", i, (int) win->queue.data[i].action);
			break;

		case WM_MESSAGE_KEYB_CODE:
			PRINTF("%d: WM_MESSAGE_KEYB_CODE: code - %d\n", i, (int) win->queue.data[i].action);
			break;

		default:
			PRINTF("%d: unknown message type! - %d\n", i, (int) win->queue.data[i].message);
			break;
		}
	}
}

// WM_MESSAGE_ACTION: 		obj_type_t type, uintptr_t obj_ptr, int action, char * name
// WM_MESSAGE_ENC2_ROTATE:  int_fast8_t rotate
// WM_MESSAGE_KEYB_CODE:	int_fast8_t keyb_code
// WM_MESSAGE_UPDATE: 		nothing
// WM_MESSAGE_CLOSE: 		nothing
uint8_t put_to_wm_queue(window_t * win, wm_message_t message, ...)
{
	if (win->queue.size >= WM_MAX_QUEUE_SIZE)
		return 0;					// очередь переполнена, ошибка

	//dump_queue(win);

	va_list arg;

	switch (message)
	{
	case WM_MESSAGE_ACTION:
	{
		va_start(arg, message);

		uint32_t type = va_arg(arg, uint32_t);
		int32_t action = va_arg(arg, int32_t);
		char * name = va_arg(arg, char *);

		va_end(arg);

		uint8_t ind = win->queue.size ? (win->queue.size - 1) : 0;
		if (win->queue.data[ind].message == WM_MESSAGE_ACTION && win->queue.data[ind].type == type && win->queue.data[ind].action == action)
			return 1;
		else
		{
			win->queue.data[win->queue.size].message = WM_MESSAGE_ACTION;
			win->queue.data[win->queue.size].type = (obj_type_t) type;
			win->queue.data[win->queue.size].action = action;
			strncpy(win->queue.data[win->queue.size].name, name, NAME_ARRAY_SIZE - 1);
			win->queue.size ++;
		}

		return 1;
	}
		break;

	case WM_MESSAGE_ENC2_ROTATE:
	{
		va_start(arg, message);
		int32_t r = va_arg(arg, int32_t);
		va_end(arg);

		uint8_t ind = win->queue.size ? (win->queue.size - 1) : 0;				// если первое в очереди сообщение - WM_MESSAGE_ENC2_ROTATE,
		if (win->queue.data[ind].message == WM_MESSAGE_ENC2_ROTATE)				// просуммировать текущее и новое значения поворота,
		{																			// иначе добавить новое сообщение
			win->queue.data[ind].action += r;
		}
		else
		{
			win->queue.data[win->queue.size].message = WM_MESSAGE_ENC2_ROTATE;
			win->queue.data[win->queue.size].type = (obj_type_t) UINT8_MAX;
			win->queue.data[win->queue.size].action = r;
			win->queue.size ++;
		}

		return 1;
	}
		break;

	case WM_MESSAGE_KEYB_CODE:
	{
		va_start(arg, message);

		win->queue.data[win->queue.size].message = WM_MESSAGE_KEYB_CODE;
		win->queue.data[win->queue.size].type = (obj_type_t) UINT8_MAX;
		win->queue.data[win->queue.size].action = va_arg(arg, int32_t);
		win->queue.size ++;

		va_end(arg);

		return 1;
	}
		break;

	case WM_MESSAGE_UPDATE:
	{
		uint8_t ind = win->queue.size ? (win->queue.size - 1) : 0;
		if (win->queue.data[ind].message != WM_MESSAGE_UPDATE)		// предотвращение дублей сообщения WM_MESSAGE_UPDATE
		{
			win->queue.data[win->queue.size].message = WM_MESSAGE_UPDATE;
			win->queue.data[win->queue.size].type = (obj_type_t) UINT8_MAX;
			win->queue.data[win->queue.size].action = INT32_MAX;
			win->queue.size ++;
		}
	}
		return 1;
		break;

	case WM_MESSAGE_CLOSE:
	{
		uint8_t ind = win->queue.size ? (win->queue.size - 1) : 0;
		if (win->queue.data[ind].message != WM_MESSAGE_CLOSE)		// предотвращение дублей сообщения WM_MESSAGE_CLOSE
		{
			win->queue.data[win->queue.size].message = WM_MESSAGE_CLOSE;
			win->queue.data[win->queue.size].type = (obj_type_t) UINT8_MAX;
			win->queue.data[win->queue.size].action = INT32_MAX;
			win->queue.size ++;
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
	GUI_ASSERT(0);
	return 0;
}

wm_message_t get_from_wm_queue(uint8_t win_id, uint8_t * type, int32_t * action, char * name)
{
	window_t * win = get_win(win_id);

	if (! win->queue.size)
		return WM_NO_MESSAGE;										// очередь сообщений пустая

	win->queue.size --;

	* type = win->queue.data[win->queue.size].type;
	* action = win->queue.data[win->queue.size].action;

	if (win->queue.data[win->queue.size].message == WM_MESSAGE_ACTION)
	{
		char obj_name[NAME_ARRAY_SIZE] = { 0 };
		strncpy(obj_name, win->queue.data[win->queue.size].name, NAME_ARRAY_SIZE - 1);
		char * r = strrchr(obj_name, '#');
		obj_name[r - obj_name] = '\0';
		strncpy(name, obj_name, NAME_ARRAY_SIZE);
	}

	wm_message_t m = win->queue.data[win->queue.size].message;

	win->queue.data[win->queue.size].message = WM_NO_MESSAGE;		// очистить текущую запись
	win->queue.data[win->queue.size].type = TYPE_DUMMY;
	win->queue.data[win->queue.size].action = 0;
	memset(win->queue.data[win->queue.size].name, 0, NAME_ARRAY_SIZE);

	return m;
}

void clean_wm_queue (window_t * win)
{
	win->queue.size = 0;
	memset(win->queue.data, 0, sizeof win->queue.data);
}

/* Запрос на обновление состояния элементов GUI */
void gui_update(void)
{
	put_to_wm_queue(get_win(WINDOW_MAIN), WM_MESSAGE_UPDATE);	// главное окно всегда нужно обновлять

	uint8_t win2 = get_parent_window();

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
	GUI_ASSERT(win);
	GUI_ASSERT(name);

	char obj_name[NAME_ARRAY_SIZE] = { 0 };
	local_snprintf_P(obj_name, NAME_ARRAY_SIZE, "%s#%02d", name, win->window_id);

	switch (type)
	{
	case TYPE_BUTTON:
		for (uint8_t i = 0; i < win->bh_count; i ++)
		{
			button_t * bh = & win->bh_ptr[i];
			if (! strcmp(bh->name, obj_name))
				return (button_t *) bh;
		}
		goto not_found;
		break;

	case TYPE_LABEL:
		for (uint8_t i = 0; i < win->lh_count; i ++)
		{
			label_t * lh = & win->lh_ptr[i];
			if (! strcmp(lh->name, obj_name))
				return (label_t *) lh;
		}
		goto not_found;
		break;

	case TYPE_SLIDER:
		for (uint8_t i = 0; i < win->sh_count; i ++)
		{
			slider_t * sh = & win->sh_ptr[i];
			if (! strcmp(sh->name, obj_name))
				return (slider_t *) sh;
		}
		goto not_found;
		break;

	case TYPE_TOUCH_AREA:
		for (uint8_t i = 0; i < win->ta_count; i ++)
		{
			touch_area_t * ta = & win->ta_ptr[i];
			if (! strcmp(ta->name, obj_name))
				return (touch_area_t *) ta;
		}
		goto not_found;
		break;

	case TYPE_TEXT_FIELD:
		for (uint8_t i = 0; i < win->tf_count; i ++)
		{
			text_field_t * tf = & win->tf_ptr[i];
			if (! strcmp(tf->name, obj_name))
				return (text_field_t *) tf;
		}
		goto not_found;
		break;

	default:
		PRINTF("%s: undefined type %d\n", __func__, type);
		GUI_ASSERT(0);
		return NULL;
	}

not_found:
	if (win->window_id == WINDOW_MAIN)
	{
		PRINTF("%s: object '%s' not found\n", __func__, name);
		GUI_ASSERT(0);
		return NULL;
	}
	else
		return find_gui_obj(type, get_win(WINDOW_MAIN), name);
}

const gui_drawbuf_t * gui_get_drawbuf(void)
{
	return __gui_get_drawbuf();
}

static uint8_t remove_from_gui_list(void * link)
{
	PLIST_ENTRY current_entry = gui_objects_list.Flink;
	while (current_entry != & gui_objects_list)
	{
		PLIST_ENTRY next_entry = current_entry->Flink; // Сохраняем следующий элемент, так как текущий может быть удален
		gui_object_t * obj = CONTAINING_RECORD(current_entry, gui_object_t, list_entry);
		if (link == obj->link)
		{
			RemoveEntryList(& obj->list_entry);
			free(obj);
			return 1;
		}
		current_entry = next_entry;
	}

	return 0;
}

/* Установка статуса элементов после инициализации */
void objects_state (window_t * win)
{
	GUI_ASSERT(win != NULL);
	int debug_num = 0;

	button_t * b = win->bh_ptr;
	if (b != NULL)
	{
		for (uint8_t i = 0; i < win->bh_count; i ++)
		{
			button_t * bh = & b[i];
			if (win->state)
			{
				if (bh->is_long_press && bh->is_repeating)
				{
					PRINTF("ERROR: invalid combination of properties 'is_long_press' and 'is_repeating' on button %s\n", bh->name);
					GUI_ASSERT(0);
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
				VERIFY(remove_from_gui_list(bh));
				debug_num --;
				gui_object_count--;
				bh->visible = NON_VISIBLE;
				GUI_ASSERT(gui_object_count >= footer_buttons_count);
			}
		}
	}

	label_t * l = win->lh_ptr;
	if(l != NULL)
	{
		for (uint8_t i = 0; i < win->lh_count; i ++)
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
				VERIFY(remove_from_gui_list(lh));
				debug_num --;
				gui_object_count--;
				lh->visible = NON_VISIBLE;
				GUI_ASSERT(gui_object_count >= footer_buttons_count);
			}
		}
	}

	slider_t * s = win->sh_ptr;
	if(s != NULL)
	{
		for (uint8_t i = 0; i < win->sh_count; i ++)
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
				VERIFY(remove_from_gui_list(sh));
				debug_num --;
				gui_object_count--;
				sh->visible = NON_VISIBLE;
				GUI_ASSERT(gui_object_count >= footer_buttons_count);
			}
		}
	}

	touch_area_t * t = win->ta_ptr;
	if(t != NULL)
	{
		for (uint8_t i = 0; i < win->ta_count; i ++)
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
				GUI_ASSERT(remove_from_gui_list(ta));
				debug_num --;
				gui_object_count--;
				ta->visible = NON_VISIBLE;
				GUI_ASSERT(gui_object_count >= footer_buttons_count);
			}
		}
	}

	text_field_t * tf = win->tf_ptr;
	if(tf != NULL)
	{
		for (uint8_t i = 0; i < win->tf_count; i ++)
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
			}
			else
			{
				VERIFY(remove_from_gui_list(tff));
				debug_num --;
				gui_object_count--;
				tff->visible = NON_VISIBLE;
				free(tff->string);
				tff->string = NULL;
				GUI_ASSERT(gui_object_count >= footer_buttons_count);
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
#if GUI_USE_CACHE
			close_button.cache = NULL;
#endif /* GUI_USE_CACHE */

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
#if GUI_USE_CACHE
            if (close_button.cache != NULL)
            {
                gui_objects_cache_destroy(close_button.cache);
                close_button.cache = NULL;
            }
#endif /* GUI_USE_CACHE */

			VERIFY(remove_from_gui_list(& close_button));
			debug_num --;
			gui_object_count--;
			close_button.visible = NON_VISIBLE;
			GUI_ASSERT(gui_object_count >= footer_buttons_count);
		}
	}
//	PRINTF("%s: %s, gui_object_count: %d %+d\n", __func__, win->title, gui_object_count, debug_num);
	clean_wm_queue(win);
}

/* Передать менеджеру GUI код нажатой кнопки на клавиатуре */
void gui_put_keyb_code (uint8_t kbch)
{
	// перенаправить код нажатой аппаратной кнопки в активное окно
	if (get_parent_window() == NO_PARENT_WINDOW)
		put_to_wm_queue(get_win(WINDOW_MAIN), WM_MESSAGE_KEYB_CODE, kbch);
	else
		put_to_wm_queue(get_win(gui.win[1]), WM_MESSAGE_KEYB_CODE, kbch);
}

void gui_put_event(gui_event_type type, uint16_t code)
{
	window_t * win = get_win(get_parent_window());

	if (type == EVENT_TYPE_CONTROL)
	{
		int p = 0;
		if (code == CODE_CURSOR_LEFT) p = -1;
		else if (code == CODE_CURSOR_RIGHT) p = 1;
		else if (code == CODE_KEY_ENTER && win->idx_bh_focus_old != UINT8_MAX)
		{
			button_t * bh = & win->bh_ptr[win->idx_bh_focus];
			if (bh->state != DISABLED)
			{
				if (! put_to_wm_queue(win, WM_MESSAGE_ACTION, TYPE_BUTTON, PRESSED, bh->name))
					dump_queue(win);
			}

			return;
		}
		else if (code == CODE_KEY_ESCAPE)
		{
			close_window(1);
			if (! get_parent_window())
				footer_buttons_state(CANCELLED);
			return;
		}

		if (win->idx_bh_focus_old != UINT8_MAX)
		{
			win->idx_bh_focus += p;
			if (win->idx_bh_focus < 0) win->idx_bh_focus = win->bh_count - 1;
			if (win->idx_bh_focus >= win->bh_count) win->idx_bh_focus = 0;
		}

		win->bh_ptr[win->idx_bh_focus_old].is_focus = 0;
		win->bh_ptr[win->idx_bh_focus].is_focus = 1;

		win->idx_bh_focus_old = win->idx_bh_focus;
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

/* Инициализация GUI */
void gui_initialize (void)
{
	InitializeListHead(& gui_objects_list);
	gui_objects_init();

	open_window(get_win(WINDOW_MAIN));
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
			window_t * win = get_win(sh->parent);
			p->x1 = (sh->x + sh->x1_p - touch_area_enlarge) < 0 ? 0 : (sh->x + sh->x1_p - touch_area_enlarge);
			p->x2 = (sh->x + sh->x2_p + touch_area_enlarge) > WITHGUIMAXX ? WITHGUIMAXX : (sh->x + sh->x2_p + touch_area_enlarge);
			p->y1 = (sh->y + sh->y1_p - touch_area_enlarge) < 0 ? 0 : (sh->y + sh->y1_p - touch_area_enlarge);
			p->y2 = (sh->y + sh->y2_p + touch_area_enlarge) > WITHGUIMAXY ? WITHGUIMAXY : (sh->y + sh->y2_p + touch_area_enlarge);
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
	GUI_ASSERT(val != NULL);
	switch (val->type)
	{
		case TYPE_CLOSE_BUTTON:
		{
			GUI_ASSERT(val->link != NULL);
			button_t * bh = (button_t *) val->link;
			bh->state = val->state;
#if GUI_USE_CACHE
			gui_objects_cache_invalidate(bh->cache);
#endif /* GUI_USE_CACHE */
			if (bh->state == RELEASED) close_all_windows();
		}
			break;

		case TYPE_BUTTON:
		{
			GUI_ASSERT(val->link != NULL);
			button_t * bh = (button_t *) val->link;
			bh->state = val->state;
#if GUI_USE_CACHE
			gui_objects_cache_invalidate(bh->cache);
#endif /* GUI_USE_CACHE */
			if (bh->state == RELEASED || bh->state == LONG_PRESSED || bh->state == PRESS_REPEATING)
			{
				if (! put_to_wm_queue(val->win, WM_MESSAGE_ACTION, TYPE_BUTTON, bh->state == LONG_PRESSED ? LONG_PRESSED : PRESSED, bh->name))
					dump_queue(val->win);
			}
		}
			break;

		case TYPE_LABEL:
		{
			GUI_ASSERT(val->link != NULL);
			label_t * lh = (label_t *) val->link;
			lh->state = val->state;
#if GUI_USE_CACHE
			gui_objects_cache_invalidate(lh->cache);
#endif /* GUI_USE_CACHE */
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
			GUI_ASSERT(val->link != NULL);
			slider_t * sh = (slider_t *) val->link;
			sh->state = val->state;
			if (sh->state == PRESSED)
			{
				slider_process(sh);
				if (! put_to_wm_queue(val->win, WM_MESSAGE_ACTION, TYPE_SLIDER, PRESSED, sh->name))
					dump_queue(val->win);
			}
#if GUI_USE_CACHE
			gui_objects_cache_invalidate(sh->cache);
#endif /* GUI_USE_CACHE */
		}
			break;

		case TYPE_TOUCH_AREA:
		{
			GUI_ASSERT(val->link != NULL);
			touch_area_t * ta = (touch_area_t *) val->link;
			ta->state = val->state;
			if (ta->state == RELEASED)
			{
				if (! put_to_wm_queue(val->win, WM_MESSAGE_ACTION, TYPE_TOUCH_AREA, PRESSED, ta->name))
					dump_queue(val->win);
			}
			else if (ta->state == PRESSED && ta->is_trackable)
			{

				if (strstr(ta->name, "ta_winmove"))
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
			PRINTF("set_state_record: undefined type %d\n", val->type);
			GUI_ASSERT(0);
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
void process_gui(void)
{
	uint_fast16_t tx, ty;
	static uint16_t x_old = 0, y_old = 0, long_press_counter = 0;
	static gui_object_t * p = NULL;
	static window_t * w = NULL;
	const uint8_t long_press_limit = 20;
	static uint8_t is_long_press = 0;		// 1 - долгое нажатие уже обработано
	static uint8_t is_repeating = 0, repeating_cnt = 0;

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

//	if (IsListEmpty(& gui_objects_list))
//		return;

	if (gui.state == CANCELLED && gui.is_touching_screen && ! gui.is_after_touch)
	{
		GUI_ASSERT(! IsListEmpty(& gui_objects_list));

		PLIST_ENTRY current_entry = gui_objects_list.Blink; // Обход списка в обратном порядке
		while (current_entry != & gui_objects_list)
		{
			p = CONTAINING_RECORD(current_entry, gui_object_t, list_entry);
			w = p->win;
			GUI_ASSERT(w != NULL);
			uint16_t x1 = p->x1 + w->x1, y1 = p->y1 + w->y1;
			uint16_t x2 = p->x2 + w->x1, y2 = p->y2 + w->y1;

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
		GUI_ASSERT(p != NULL);
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
				GUI_ASSERT(p != NULL);
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
			GUI_ASSERT(p != NULL);
			gui.state = CANCELLED;
			p->state = CANCELLED;
			set_state_record(p);
			gui.is_after_touch = 1; 	// точка непрерывного касания вышла за пределы выбранного элемента, не поддерживающего tracking
		}
	}

	if (gui.state == RELEASED)
	{
		GUI_ASSERT(p != NULL);
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

	TIME_PROFILE_START(gui);

	for(uint8_t i = 0; i < opened_windows_count; i ++)
	{
		window_t * win = get_win(gui.win[i]);
		gui.current_drawing_window = gui.win[i];

		if (win->state == VISIBLE)
		{
			win->onVisibleProcess();	// запуск процедуры фоновой обработки для окна

			if (! win->first_call)
			{
				draw_window(win);

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
							draw_close_button(bh);
					}
					else if (p->type == TYPE_LABEL)
					{
						label_t * lh = (label_t *) p->link;
						if (lh->visible && lh->parent == win->window_id)
							draw_label(lh);
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
							draw_textfield(tf);
					}

					current_entry = current_entry->Flink;
				}
			}
		}
	}
	TIME_PROFILE_STOP(gui, "");
}

#endif /* WITHTOUCHGUI */
