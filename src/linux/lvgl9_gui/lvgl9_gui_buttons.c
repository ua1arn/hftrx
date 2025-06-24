/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "hardware.h"
#include "formats.h"
#include "display2.h"
#include "radio.h"

#if WITHLVGL && LINUX_SUBSYSTEM

#include "lvgl9_gui.h"

static void long_press_timer_cb(lv_timer_t * timer)
{
	user_t * ext = lv_timer_get_user_data(timer);
    ext->is_pressed = false;
    ext->is_long_pressed = true;
    lv_timer_del(ext->long_press_timer);
}

void buttons_handler(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code != LV_EVENT_PRESSED && code != LV_EVENT_RELEASED && code != LV_EVENT_PRESSING &&
			code != LV_EVENT_PRESS_LOST && code != LV_EVENT_CLICKED) return;

	lv_obj_t * btn = (lv_obj_t *) lv_event_get_target(e);
	user_t * ext = lv_obj_get_user_data(btn);

	if(code == LV_EVENT_PRESSED)
	{
		ext->is_pressed = true;
		ext->long_press_timer = lv_timer_create(long_press_timer_cb, 1000, ext); // 1 секунда
	}
	else if (code == LV_EVENT_PRESSING && ext->is_long_pressed)
	{
//		LV_LOG_USER("Long press %s", ext->name);
		ext->eh(e);
		ext->is_long_pressed = 0;
	}
	else if(code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST)
	{
		if(ext->is_pressed)
		{
//			LV_LOG_USER("Short press %s", ext->name);
			lv_timer_del(ext->long_press_timer);
			ext->is_pressed = false;
			ext->is_clicked = true;
			ext->eh(e);
			ext->is_clicked = false;
		}
	}
}

lv_obj_t * find_button(lv_obj_t * cont, const char * name)
{
	uint32_t cnt = lv_obj_get_child_count(cont);

	for (int i = 0; i < cnt; i ++)
	{
		lv_obj_t * btn = lv_obj_get_child(cont, i);
		user_t * ext = lv_obj_get_user_data(btn);

		if (! strcmp(name, ext->name) && lv_obj_check_type(btn, & lv_button_class))
			return btn;
	}

	printf("%s not found\n", name);
	ASSERT(0);
	return NULL;
}

void button_set_text(lv_obj_t * btn, const char * text)
{
	lv_obj_t * lbl = lv_obj_get_child(btn, 0);
	lv_label_set_text_fmt(lbl, "%s", text);
	lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_center(lbl);
}

void button_lock(lv_obj_t * btn)
{
	lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_ORANGE), 0);
}

void button_unlock(lv_obj_t * btn)
{
	lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_LIGHT_GREEN), 0);
}

void button_set_lock(lv_obj_t * btn, uint8_t v)
{
	if (v)
		button_lock(btn);
	else
		button_unlock(btn);
}

lv_obj_t * add_button(lv_obj_t * cont, user_t * ext, uint8_t idx, const char * text, btns_size_t s, event_handler_t eh)
{
	uint8_t w_st = (s >> 8) & 0xFF;
	uint8_t h_st = (s >> 0) & 0xFF;

	user_t * btu = & ext[idx];

	lv_obj_t * btn = lv_button_create(cont);
	lv_obj_add_style(btn, & btnst, 0);
	lv_obj_set_size(btn, w_st, h_st);

	lv_obj_t * label = lv_label_create(btn);
	lv_obj_add_style(label, & lblst, 0);
	button_set_text(btn, text);

	btu->index = idx;
	btu->eh = eh;
	if (! strlen(btu->name))
		snprintf(btu->name, 20, "btn_%s", text, btu->text);

	lv_obj_set_user_data(btn, btu);
	lv_obj_add_event_cb(btn, buttons_handler, LV_EVENT_ALL, NULL);

	return btn;
}

void create_button_matrix(lv_obj_t * cont, user_t * btu, const char * mname, const uint8_t btns, const uint8_t cols, btns_size_t s, event_handler_t eh)
{
	static lv_coord_t cols_dsc[10], rows_dsc[10];
	uint8_t w_st = (s >> 8) & 0xFF;
	uint8_t h_st = (s >> 0) & 0xFF;

	uint8_t i = 0;
	for (; i < 9; i ++)
	{
		cols_dsc[i] = w_st;
		rows_dsc[i] = h_st;
	}

	cols_dsc[i + 1] = LV_GRID_TEMPLATE_LAST;
	rows_dsc[i + 1] = LV_GRID_TEMPLATE_LAST;

	lv_obj_set_style_grid_column_dsc_array(cont, cols_dsc, 0);
	lv_obj_set_style_grid_row_dsc_array(cont, rows_dsc, 0);
	lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_center(cont);
	lv_obj_set_layout(cont, LV_LAYOUT_GRID);

	uint8_t row = 0, col = 0;

	for(uint8_t i = 0; i < btns; i ++)
	{
		int8_t row = i / cols;
		int8_t col = i % cols;

		lv_obj_t * btn = lv_button_create(cont);
		lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, col, 1,
							 LV_GRID_ALIGN_STRETCH, row, 1);

		lv_obj_add_style(btn, & btnst, 0);

		lv_obj_t * label = lv_label_create(btn);
		lv_label_set_text_fmt(label, "%s", btu[i].text);
		lv_obj_center(label);
		lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
		lv_obj_add_style(label, & lblst, 0);

		btu[i].index = i;
		btu[i].eh = eh;

		if (! strlen(btu[i].name))
			snprintf(btu[i].name, 20, "%s_%s", mname, btu[i].text);

		if (btu[i].state == LOCKED)
			button_lock(btn);
		if (btu[i].state == DISABLED)
			lv_obj_set_state(btn, LV_STATE_DISABLED, 1);

		lv_obj_set_user_data(btn, & btu[i]);
		lv_obj_add_event_cb(btn, buttons_handler, LV_EVENT_ALL, NULL);
	}
}

#endif /* WITHLVGL && LINUX_SUBSYSTEM */
