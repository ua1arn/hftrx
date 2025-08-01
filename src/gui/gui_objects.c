/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "hardware.h"
#include "formats.h"
#include "display/display.h"
#include "display/fontmaps.h"

#if WITHTOUCHGUI

#include "gui.h"
#include "gui_user.h"
#include "gui_system.h"
#include "gui_structs.h"
#include "gui_settings.h"
#include "gui_windows.h"

// *************** Labels ***************

/* Получение ширины метки в пикселях  */
uint16_t get_label_width(const label_t * const lh)
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
uint16_t get_label_height(const label_t * const lh)
{
	if (lh->font_size == FONT_LARGE)
		return SMALLCHARH;
	else if (lh->font_size == FONT_MEDIUM)
		return SMALLCHARH2;
	else if (lh->font_size == FONT_SMALL)
		return SMALLCHARH3;
	return 0;
}

uint16_t get_label_width2(const char * name)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);

	if (lh->font_size == FONT_LARGE)
		return strlen(lh->text) * SMALLCHARW;
	else if (lh->font_size == FONT_MEDIUM)
		return strlen(lh->text) * SMALLCHARW2;
	else if (lh->font_size == FONT_SMALL)
		return strlen(lh->text) * SMALLCHARW3;
	return 0;
}

uint16_t get_label_height2(const char * name)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);

	if (lh->font_size == FONT_LARGE)
		return SMALLCHARH;
	else if (lh->font_size == FONT_MEDIUM)
		return SMALLCHARH2;
	else if (lh->font_size == FONT_SMALL)
		return SMALLCHARH3;
	return 0;
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
void textfield_add_string(text_field_t * tf, const char * str, COLORPIP_T color)
{
	ASSERT(tf != NULL);

	tf_entry_t * rec = &  tf->string [tf->index];
	strncpy(rec->text, str, TEXT_ARRAY_SIZE - 1);
	rec->color_line = color;
	tf->index ++;
	tf->index = tf->index >= tf->h_str ? 0 : tf->index;
}

/* Очистить текстовое поле */
void textfield_clean(text_field_t * tf)
{
	ASSERT(tf != NULL);
	tf->index = 0;
	memset(tf->string, 0, tf->h_str * sizeof(tf_entry_t));
}

// *************** Common ***************

void gui_obj_align_to(const char * name1, const char * name2, object_alignment_t align, uint16_t offset)
{
	window_t * win = get_win(get_parent_window());
	obj_type_t type1 = parse_obj_name(name1);
	obj_type_t type2 = parse_obj_name(name2);
	void * oh1 = find_gui_obj(type1, win, name1);
	void * oh2 = find_gui_obj(type2, win, name2);

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

	default:
		break;
	}

	switch(type1)
	{
	case TYPE_LABEL:
		label_t * lh1 = (label_t *) oh1;

		if (align == ALIGN_RIGHT_UP) { lh1->x = x2 + w2 + offset; lh1->y = y2; }
		else if (align == ALIGN_LEFT_UP)  { lh1->x = x2 - get_label_width(lh1) - offset; lh1->y = y2; }
		else if (align == ALIGN_DOWN_LEFT) { lh1->x = x2; lh1->y = y2 + h2 + offset; }
		break;

	case TYPE_BUTTON:
		button_t * bh1 = (button_t *) oh1;

		if (align == ALIGN_RIGHT_UP) { bh1->x1 = x2 + w2 + offset; bh1->y1 = y2; }
		else if (align == ALIGN_LEFT_UP)  { bh1->x1 = x2 - bh1->w - offset; bh1->y1 = y2; }
		else if (align == ALIGN_DOWN_LEFT) { bh1->x1 = x2; bh1->y1 = y2 + h2 + offset; }
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
		else if (prop == GUI_OBJ_TEXT) strncpy(lh->text, va_arg(arg, char *), TEXT_ARRAY_SIZE - 1);
		else if (prop == GUI_OBJ_TEXT_FMT) vsnprintf(lh->text, TEXT_ARRAY_SIZE - 1, va_arg(arg, char *), arg);
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
		break;
	default:
		break;
	}

	va_end(arg);
}

#endif /* WITHTOUCHGUI */
