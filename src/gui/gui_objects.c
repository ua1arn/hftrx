/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "hardware.h"
#include "formats.h"
#include "src/display/display.h"
#include "src/display/fontmaps.h"

#if WITHTOUCHGUI

#include "gui.h"
#include "gui_user.h"
#include "gui_system.h"
#include "gui_structs.h"
#include "gui_settings.h"
#include "gui_windows.h"

const label_t label_default = 	{ 0, CANCELLED, 0, NON_VISIBLE, "", "", FONT_MEDIUM, COLORPIP_WHITE, };
const button_t button_default = { 0, 0, CANCELLED, BUTTON_NON_LOCKED, 0, 1, 0, NON_VISIBLE, INT32_MAX, "", "", };
const text_field_t tf_default = { 0, 0, CANCELLED, 0, NON_VISIBLE, UP, NULL, "", };
const touch_area_t ta_default = { 0, 0, 0, 0, 0, "", 0, 0, 0, 0, 0, };

// *************** Labels ***************

/* Получение ширины метки в пикселях  */
uint16_t get_label_width(const label_t * const lh)
{
	return lh->width_pix;
}

/* Получение высоты метки в пикселях  */
uint16_t get_label_height(const label_t * const lh)
{
	return lh->height_pix;
}

uint16_t get_label_width2(const char * name)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);

	return lh->width_pix;
}

uint16_t get_label_height2(const char * name)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);

	return lh->height_pix;
}

// *************** Text fields ***************

/* Рассчитать размеры текстового поля */
void textfield_update_size(text_field_t * tf)
{
	ASSERT(tf != NULL);
	if (tf->font)
	{
		tf->w = tf->font->width * tf->w_sim;
		tf->h = tf->font->height * tf->h_str;
	}
	else
	{
		tf->w = SMALLCHARW2 * tf->w_sim;
		tf->h = SMALLCHARH2 * tf->h_str;
	}
	ASSERT(tf->w < WITHGUIMAXX);
	ASSERT(tf->h < WITHGUIMAXY - window_title_height);
}

/* Добавить строку в текстовое поле */
void textfield_add_string_old(text_field_t * tf, const char * str, COLORPIP_T color)
{
	ASSERT(tf != NULL);

	tf_entry_t * rec = &  tf->string[tf->index];
	strncpy(rec->text, str, TEXT_ARRAY_SIZE - 1);
	rec->color_line = color;
	tf->index ++;
	tf->index = tf->index >= tf->h_str ? 0 : tf->index;
}

void textfield_add_string(const char * name, const char * str, COLORPIP_T color)
{
	window_t * win = get_win(get_parent_window());
	text_field_t * tf = (text_field_t *) find_gui_obj(TYPE_TEXT_FIELD, win, name);

	tf_entry_t * rec = &  tf->string[tf->index];
	strncpy(rec->text, str, TEXT_ARRAY_SIZE - 1);
	rec->color_line = color;
	tf->index ++;
	tf->index = tf->index >= tf->h_str ? 0 : tf->index;
}

/* Очистить текстовое поле */
void textfield_clean(const char * name)
{
	window_t * win = get_win(get_parent_window());
	text_field_t * tf = (text_field_t *) find_gui_obj(TYPE_TEXT_FIELD, win, name);

	tf->index = 0;
	memset(tf->string, 0, tf->h_str * sizeof(tf_entry_t));
}

// *************** Common ***************

obj_type_t parse_obj_name(const char * name)
{
	ASSERT(name);

	if (! strncmp(name, "btn_", 4))
		return TYPE_BUTTON;
	else if (! strncmp(name, "lbl_", 4))
		return TYPE_LABEL;
	else if (! strncmp(name, "sl_", 3))
		return TYPE_SLIDER;
	else if (! strncmp(name, "btc_", 4))
		return TYPE_CLOSE_BUTTON;
	else if (! strncmp(name, "ta_", 3))
		return TYPE_TOUCH_AREA;
	else if (! strncmp(name, "tf_", 3))
		return TYPE_TEXT_FIELD;
	else
	{
		PRINTF("Unrecognized GUI object type: %s\n", name);
		ASSERT(0);
		return TYPE_DUMMY;
	}
}

// label: font_size, color, width_by_symbols
// button: w, h, is_repeating, is_long_press, text,
// text_field: w_sim, h_str, direction, font *
// touch area: x, y, w, h, is_trackable
// возвращает индекс созданного объекта

uint8_t gui_obj_create(const char * obj_name, ...)
{
	uint8_t idx, window_id = get_parent_window();
	window_t * win = get_win(window_id);
	obj_type_t type = parse_obj_name(obj_name);
	va_list arg;
	va_start(arg, obj_name);

	switch (type)
	{
	case TYPE_LABEL:
	{
		win->lh_ptr = (label_t *) realloc(win->lh_ptr, sizeof(label_t) * (win->lh_count + 1));
		GUI_MEM_ASSERT(win->lh_ptr);

		label_t * lh = & win->lh_ptr[win->lh_count];
		memcpy(lh, & label_default, sizeof(label_t));

		lh->parent = window_id;
		lh->font_size = (font_size_t) va_arg(arg, int);
		lh->color = va_arg(arg, COLORPIP_T);
		lh->visible = 1;
		lh->index = win->lh_count;
		lh->x = 0;
		lh->y = 0;

		strncpy(lh->name, obj_name, NAME_ARRAY_SIZE - 1);
		lh->width = va_arg(arg, uint32_t);
		memset(lh->text, '*', lh->width);		// для совместимости, потом убрать

		if (lh->font_size == FONT_LARGE)
		{
			lh->width_pix = lh->width * SMALLCHARW;
			lh->height_pix = SMALLCHARH;
		}
		else if (lh->font_size == FONT_MEDIUM)
		{
			lh->width_pix = lh->width * SMALLCHARW2;
			lh->height_pix = SMALLCHARH2;
		}
		else if (lh->font_size == FONT_SMALL)
		{
			lh->width_pix = lh->width * SMALLCHARW3;
			lh->height_pix = SMALLCHARH3;
		}

		idx = win->lh_count;
		win->lh_count ++;
		break;
	}

	case TYPE_BUTTON:
	{
		win->bh_ptr = (button_t *) realloc(win->bh_ptr, sizeof(button_t) * (win->bh_count + 1));
		GUI_MEM_ASSERT(win->bh_ptr);

		button_t * bh = & win->bh_ptr[win->bh_count];
		memcpy(bh, & button_default, sizeof(button_t));

		bh->parent = window_id;
		bh->w = va_arg(arg, uint_fast16_t);
		bh->h = va_arg(arg, uint_fast16_t);
		bh->is_repeating = va_arg(arg, uint32_t);
		bh->is_long_press = va_arg(arg, uint32_t);
		strncpy(bh->name, obj_name, NAME_ARRAY_SIZE - 1);
		strncpy(bh->text, va_arg(arg, char *), TEXT_ARRAY_SIZE - 1);
		bh->visible = 1;
		bh->index = win->bh_count;
		bh->x1 = 0;
		bh->y1 = 0;

		win->bh_count;
		win->bh_count ++;
		break;
	}

	case TYPE_TEXT_FIELD:
	{
		win->tf_ptr = (text_field_t *) realloc(win->tf_ptr, sizeof(text_field_t) * (win->tf_count + 1));
		GUI_MEM_ASSERT(win->tf_ptr);

		text_field_t * tf = & win->tf_ptr[win->tf_count];
		memcpy(tf, & tf_default, sizeof(text_field_t));

		tf->parent = window_id;
		tf->w_sim = va_arg(arg, uint32_t);
		tf->h_str = va_arg(arg, uint32_t);
		tf->direction = (tf_direction_t) va_arg(arg, uint32_t);
		tf->font = va_arg(arg, UB_Font *);
		strncpy(tf->name, obj_name, NAME_ARRAY_SIZE - 1);
		tf->visible = 1;
		tf->index = win->tf_count;
		tf->x1 = 0;
		tf->y1 = 0;

		textfield_update_size(tf);

		win->tf_count;
		win->tf_count ++;
		break;
	}

	case TYPE_TOUCH_AREA:
	{
		win->ta_ptr = (touch_area_t *) realloc(win->ta_ptr, sizeof(touch_area_t) * (win->ta_count + 1));
		GUI_MEM_ASSERT(win->ta_ptr);

		touch_area_t * ta = & win->ta_ptr[win->ta_count];
		memcpy(ta, & ta_default, sizeof(touch_area_t));

		ta->parent = window_id;
		ta->x1 = va_arg(arg, uint_fast16_t);
		ta->y1 = va_arg(arg, uint_fast16_t);
		ta->w = va_arg(arg, uint_fast16_t);
		ta->h = va_arg(arg, uint_fast16_t);
		ta->is_trackable = va_arg(arg, uint_fast16_t);
		strncpy(ta->name, obj_name, NAME_ARRAY_SIZE - 1);
		ta->visible = 1;
		ta->index = win->ta_count;

		win->ta_count;
		win->ta_count ++;
		break;
	}

	case TYPE_SLIDER:
	{
		win->sh_ptr = (slider_t *) realloc(win->sh_ptr, sizeof(slider_t) * (win->sh_count + 1));
		GUI_MEM_ASSERT(win->sh_ptr);

		slider_t * sh = & win->sh_ptr[win->sh_count];
		memset(sh, 0, sizeof(slider_t));

		sh->parent = window_id;
		sh->orientation = va_arg(arg, int);
		strncpy(sh->name, obj_name, NAME_ARRAY_SIZE - 1);
		sh->state = CANCELLED;
		sh->visible = 1;
		sh->size = va_arg(arg, int);
		sh->step = va_arg(arg, int);
		sh->value = 0;
		sh->value_old = 255;

		if (sh->orientation)	// ORIENTATION_HORIZONTAL
		{
			sh->width = sh->size;
			sh->height = sliders_h * 2;
		}
		else					// ORIENTATION_VERTICAL
		{
			sh->width = sliders_w;
			sh->height = sh->size;
		}

		sh->index = win->sh_count;
		win->sh_count ++;
		break;
	}

	default:
		break;
	}

	va_end(arg);
	return idx;
}

void gui_obj_align_to(const char * name1, const char * name2, object_alignment_t align, uint16_t offset)
{
	window_t * win = get_win(get_parent_window());
	obj_type_t type1 = parse_obj_name(name1);
	obj_type_t type2 = parse_obj_name(name2);
	void * oh1 = find_gui_obj(type1, win, name1);
	void * oh2 = find_gui_obj(type2, win, name2);

	if (oh1 == oh2)
		return;

	uint16_t x2, y2, w2, h2;

	switch(type2)
	{
	case TYPE_LABEL:
		label_t * lh2 = (label_t *) oh2;
		x2 = lh2->x;
		y2 = lh2->y;
		w2 = get_label_width(lh2);
		h2 = get_label_height(lh2);
		break;

	case TYPE_BUTTON:
		button_t * bh2 = (button_t *) oh2;
		x2 = bh2->x1;
		y2 = bh2->y1;
		w2 = bh2->w;
		h2 = bh2->h;
		break;

	case TYPE_TEXT_FIELD:
		text_field_t * tf2 = (text_field_t *) oh2;
		x2 = tf2->x1;
		y2 = tf2->y1;
		w2 = tf2->w;
		h2 = tf2->h;
		break;

	case TYPE_SLIDER:
		slider_t * sh2 = (slider_t *) oh2;
		x2 = sh2->x;
		y2 = sh2->y;
		w2 = sh2->width;
		h2 = sh2->height;

	default:
		break;
	}

	switch(type1)
	{
	case TYPE_LABEL:
		label_t * lh1 = (label_t *) oh1;

		if (align == ALIGN_RIGHT_UP) { lh1->x = x2 + w2 + offset; lh1->y = y2; }
		else if (align == ALIGN_RIGHT_UP_MID) { lh1->x = x2 + w2 + offset; lh1->y = y2 + (h2 / 2 - get_label_height(lh1) / 2); }
		else if (align == ALIGN_LEFT_UP)  { lh1->x = x2 - get_label_width(lh1) - offset; lh1->y = y2; }
		else if (align == ALIGN_DOWN_LEFT) { lh1->x = x2; lh1->y = y2 + h2 + offset; }
		else if (align == ALIGN_DOWN_MID) { lh1->x = x2 + w2 / 2 - get_label_width(lh1) / 2; lh1->y = y2 + h2 + offset; }
		break;

	case TYPE_BUTTON:
		button_t * bh1 = (button_t *) oh1;

		if (align == ALIGN_RIGHT_UP) { bh1->x1 = x2 + w2 + offset; bh1->y1 = y2; }
		else if (align == ALIGN_RIGHT_UP_MID) { bh1->x1 = x2 + w2 + offset; bh1->y1 = y2 + (h2 / 2 - bh1->h / 2); }
		else if (align == ALIGN_LEFT_UP)  { bh1->x1 = x2 - bh1->w - offset; bh1->y1 = y2; }
		else if (align == ALIGN_DOWN_LEFT) { bh1->x1 = x2; bh1->y1 = y2 + h2 + offset; }
		else if (align == ALIGN_DOWN_MID) { bh1->x1 = x2 + w2 / 2 - bh1->w / 2; bh1->y1 = y2 + h2 + offset; }
		break;

	case TYPE_SLIDER:
		slider_t * sh1 = (slider_t *) oh1;

		if (align == ALIGN_RIGHT_UP) { sh1->x = x2 + w2 + offset; sh1->y = y2; }
		else if (align == ALIGN_RIGHT_UP_MID) { sh1->x = x2 + w2 + offset; sh1->y = y2 + (h2 / 2 - sh1->height / 2); }
		else if (align == ALIGN_LEFT_UP)  { sh1->x = x2 - sh1->width - offset; sh1->y = y2; }
		else if (align == ALIGN_DOWN_LEFT) { sh1->x = x2; sh1->y = y2 + h2 + offset; }
		else if (align == ALIGN_DOWN_MID) { sh1->x = x2 + w2 / 2 - sh1->width / 2; sh1->y = y2 + h2 + offset; }

		break;

	default:
		break;
	}
}

char * gui_obj_get_string_prop(const char * name, object_prop_t prop)
{
	window_t * win = get_win(get_parent_window());
	obj_type_t type = parse_obj_name(name);
	void * obj = find_gui_obj(type, win, name);

	switch(type)
	{
	case TYPE_LABEL:
		label_t * lh = (label_t *) obj;
		if (prop == GUI_OBJ_TEXT) return lh->text;
		break;
	case TYPE_BUTTON:
		button_t * bh = (button_t *) obj;
		if (prop == GUI_OBJ_TEXT) return bh->text;
		break;
	default:
		break;
	}

	return NULL;
}

int gui_obj_get_int_prop(const char * name, object_prop_t prop)
{
	window_t * win = get_win(get_parent_window());
	obj_type_t type = parse_obj_name(name);
	void * obj = find_gui_obj(type, win, name);

	switch(type)
	{
	case TYPE_LABEL:
		label_t * lh = (label_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) return lh->visible;
		else if (prop == GUI_OBJ_POS_X) return lh->x;
		else if (prop == GUI_OBJ_POS_Y) return lh->y;
		else if (prop == GUI_OBJ_PAYLOAD) return lh->payload;
		else if (prop == GUI_OBJ_STATE) return lh->state;
		else if (prop == GUI_OBJ_WIDTH) return lh->width_pix;
		else if (prop == GUI_OBJ_HEIGHT) return lh->height_pix;
		else if (prop == GUI_OBJ_INDEX) return lh->index;
		break;

	case TYPE_BUTTON:
		button_t * bh = (button_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) return bh->visible;
		else if (prop == GUI_OBJ_POS_X) return bh->x1;
		else if (prop == GUI_OBJ_POS_Y) return bh->y1;
		else if (prop == GUI_OBJ_PAYLOAD) return bh->payload;
		else if (prop == GUI_OBJ_STATE) return bh->state;
		else if (prop == GUI_OBJ_LOCK) return bh->is_locked;
		else if (prop == GUI_OBJ_WIDTH) return bh->w;
		else if (prop == GUI_OBJ_HEIGHT) return bh->h;
		else if (prop == GUI_OBJ_INDEX) return bh->index;
		break;

	case TYPE_SLIDER:
		slider_t * sh = (slider_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) return sh->visible;
		else if (prop == GUI_OBJ_POS_X) return sh->x;
		else if (prop == GUI_OBJ_POS_Y) return sh->y;
		else if (prop == GUI_OBJ_STATE) return sh->state;
		else if (prop == GUI_OBJ_WIDTH) return sh->width;
		else if (prop == GUI_OBJ_HEIGHT) return sh->height;
		else if (prop == GUI_OBJ_PAYLOAD) return sh->value;
		else if (prop == GUI_OBJ_SIZE) return sh->size;
		else if (prop == GUI_OBJ_INDEX) return sh->index;
		break;

	default:
		break;
	}
}

void gui_obj_set_prop(const char * name, object_prop_t prop, ...)
{
	window_t * win = get_win(get_parent_window());
	obj_type_t type = parse_obj_name(name);
	void * obj = find_gui_obj(type, win, name);
	va_list arg;
	va_start(arg, prop);

	switch(type)
	{
	case TYPE_LABEL:
		label_t * lh = (label_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) lh->visible = !! va_arg(arg, int);
		else if (prop == GUI_OBJ_POS_X) lh->x = va_arg(arg, int);
		else if (prop == GUI_OBJ_POS_Y) lh->y = va_arg(arg, int);
		else if (prop == GUI_OBJ_POS) { lh->x = va_arg(arg, int); lh->y = va_arg(arg, int); }
		else if (prop == GUI_OBJ_PAYLOAD) lh->payload = va_arg(arg, int);
		else if (prop == GUI_OBJ_TEXT) strncpy(lh->text, va_arg(arg, char *), TEXT_ARRAY_SIZE - 1);
		else if (prop == GUI_OBJ_TEXT_FMT) vsnprintf(lh->text, TEXT_ARRAY_SIZE - 1, va_arg(arg, char *), arg);
		else if (prop == GUI_OBJ_STATE) lh->state = va_arg(arg, int);
		else if (prop == GUI_OBJ_COLOR) lh->color = va_arg(arg, PACKEDCOLORPIP_T);
		break;

	case TYPE_BUTTON:
		button_t * bh = (button_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) bh->visible = !! va_arg(arg, int);
		else if (prop == GUI_OBJ_POS_X) bh->x1 = va_arg(arg, int);
		else if (prop == GUI_OBJ_POS_Y) bh->y1 = va_arg(arg, int);
		else if (prop == GUI_OBJ_POS) { bh->x1 = va_arg(arg, int); bh->y1 = va_arg(arg, int); }
		else if (prop == GUI_OBJ_PAYLOAD) bh->payload = va_arg(arg, int);
		else if (prop == GUI_OBJ_TEXT) strncpy(bh->text, va_arg(arg, char *), TEXT_ARRAY_SIZE - 1);
		else if (prop == GUI_OBJ_TEXT_FMT) vsnprintf(bh->text, TEXT_ARRAY_SIZE - 1, va_arg(arg, char *), arg);
		else if (prop == GUI_OBJ_STATE) bh->state = va_arg(arg, int);
		else if (prop == GUI_OBJ_LOCK) bh->is_locked = va_arg(arg, int);
		else if (prop == GUI_OBJ_WIDTH) bh->w = va_arg(arg, int);
		else if (prop == GUI_OBJ_HEIGHT) bh->h = va_arg(arg, int);
		else if (prop == GUI_OBJ_SIZE) { bh->w = va_arg(arg, int); bh->h = va_arg(arg, int); }
		else if (prop == GUI_OBJ_REPEAT) bh->is_repeating = va_arg(arg, int);
		break;

	case TYPE_SLIDER:
		slider_t * sh = (slider_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) sh->visible = !! va_arg(arg, int);
		else if (prop == GUI_OBJ_POS_X) sh->x = va_arg(arg, int);
		else if (prop == GUI_OBJ_POS_Y) sh->y = va_arg(arg, int);
		else if (prop == GUI_OBJ_POS) { sh->x = va_arg(arg, int); sh->y = va_arg(arg, int); }
		else if (prop == GUI_OBJ_PAYLOAD) sh->value = va_arg(arg, int);
		break;

	default:
		break;
	}

	va_end(arg);
}

uint8_t gui_check_obj(const char * name1, const char * name2)
{
	return strcmp(name1, name2) == 0;
}

// выравнивание однотипных объектов (кнопка, метка, слайдер) с передачей массива имен объектов
void gui_arrange_objects(const char names[][NAME_ARRAY_SIZE], uint8_t count, uint8_t cols, uint8_t interval)
{
	if (count <= 1) return;

	window_t * win = get_win(get_parent_window());

	obj_type_t type = parse_obj_name(names[0]);
	if (type != TYPE_BUTTON && type != TYPE_LABEL && type != TYPE_SLIDER)
	{
		PRINTF("%s: idx %d unsupported object type to arrange\n", __func__, 0);
		ASSERT(0);
	}

	uint16_t x = gui_obj_get_int_prop(names[0], GUI_OBJ_POS_X);
	uint16_t y = gui_obj_get_int_prop(names[0], GUI_OBJ_POS_Y);
	uint16_t w = gui_obj_get_int_prop(names[0], GUI_OBJ_WIDTH);
	uint16_t h = gui_obj_get_int_prop(names[0], GUI_OBJ_HEIGHT);

	for (int i = 1; i < count; i ++)
	{
		uint8_t row = i / cols;
		uint8_t col = i % cols;

		const char * obj = names[i];

		obj_type_t typex = parse_obj_name(obj);
		if (typex != type)
		{
			PRINTF("%s: idx %d - arrange various objects not supported\n", __func__, i);
			ASSERT(0);
		}

		gui_obj_set_prop(obj, GUI_OBJ_POS_X, x + (w + interval) * col);
		gui_obj_set_prop(obj, GUI_OBJ_POS_Y, y + (h + interval) * row);
	}
}

char * get_obj_name_by_idx(obj_type_t type, uint8_t idx)
{
	window_t * win = get_win(get_parent_window());

	if (type == TYPE_BUTTON)
	{
		ASSERT(idx < win->bh_count);
		return win->bh_ptr[idx].name;
	}
	else if (type == TYPE_LABEL)
	{
		ASSERT(idx < win->lh_count);
		return win->lh_ptr[idx].name;
	}
	else if (type == TYPE_SLIDER)
	{
		ASSERT(idx < win->sh_count);
		return win->sh_ptr[idx].name;
	}

	ASSERT(0);
	return NULL;
}

static uint8_t get_obj_idx_by_name(window_t * win, obj_type_t type, const char * name)
{
	void * p = find_gui_obj(type, win, name);

	if (type == TYPE_BUTTON)
		return ((button_t *) p)->index;
	else if (type == TYPE_LABEL)
		return ((label_t *) p)->index;
	else if (type == TYPE_SLIDER)
		return ((slider_t *) p)->index;

	ASSERT(0);
	return 0;
}

// выравнивание однотипных объектов (кнопка, метка, слайдер) с передачей имени первого объекта,
// обработка по возрастанию индекса (в порядке создания)
void gui_arrange_objects_from(const char * name, uint8_t count, uint8_t cols, uint8_t interval)
{
	if (count <= 1) return;

	window_t * win = get_win(get_parent_window());

	obj_type_t type = parse_obj_name(name);
	if (type != TYPE_BUTTON && type != TYPE_LABEL && type != TYPE_SLIDER)
	{
		PRINTF("%s: idx %d unsupported object type to arrange\n", __func__, 0);
		ASSERT(0);
	}

	uint16_t x = gui_obj_get_int_prop(name, GUI_OBJ_POS_X);
	uint16_t y = gui_obj_get_int_prop(name, GUI_OBJ_POS_Y);
	uint16_t w = gui_obj_get_int_prop(name, GUI_OBJ_WIDTH);
	uint16_t h = gui_obj_get_int_prop(name, GUI_OBJ_HEIGHT);
	uint8_t idx = get_obj_idx_by_name(win, type, name) + 1;

	for (int i = 1; i < count; i ++)
	{
		uint8_t row = i / cols;
		uint8_t col = i % cols;

		const char * obj = get_obj_name_by_idx(type, idx ++);

		gui_obj_set_prop(obj, GUI_OBJ_POS_X, x + (w + interval) * col);
		gui_obj_set_prop(obj, GUI_OBJ_POS_Y, y + (h + interval) * row);
	}
}

#endif /* WITHTOUCHGUI */
