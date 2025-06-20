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
#include "windows.h"

static lv_obj_t * to_update[max_updating_objects];
static uint8_t update_count = 0;

static void add_to_update(lv_obj_t * p)
{
	to_update[update_count] = p;
	update_count ++;
	ASSERT(update_count < max_updating_objects);
}

void win_modes_handler(lv_event_t * e)
{
	enum { btn_num = 8 };

	if (! e) // init window
	{
		lv_obj_t * cont = gui_win_get_content();

		static btn_t btns [btn_num] = {
				{ "LSB", SUBMODE_LSB, }, { "AM", SUBMODE_AM, },
				{ "CW",  SUBMODE_CW, },  { "DGL", SUBMODE_DGL, },
				{ "USB", SUBMODE_USB, }, { "NFM", SUBMODE_NFM, },
				{ "CWR", SUBMODE_CWR, }, { "DGU", SUBMODE_DGU, },
		};

		create_button_matrix(cont, btns, "", btn_num, 4, s86x44, win_modes_handler);

		return;
	}

	lv_obj_t * btn = (lv_obj_t *) lv_event_get_target(e);
	btn_t * btnu = lv_obj_get_user_data(btn);

	if (btnu->is_clicked)
	{
		hamradio_change_submode(btnu->payload, 1);
		win_close();
	}

}

// ***********************************************

static uint32_t mems[memory_cells_count];

static uint32_t load_mems(uint8_t cell, uint8_t set)
{
	if (cell == 0)
		load_memory_cells(mems, memory_cells_count);

	if (set)
		hamradio_set_freq(mems[cell]);

	return mems[cell];
}

static void clean_mems(uint8_t cell)
{
	ASSERT(cell < memory_cells_count);
	mems[cell] = 0;
	write_memory_cells(mems, memory_cells_count);
}

static void write_mems(uint8_t cell, uint32_t freq)
{
	ASSERT(cell < memory_cells_count);
	mems[cell] = freq;
	write_memory_cells(mems, memory_cells_count);
}

void win_memory_handler(lv_event_t * e)
{
	enum { btn_num = memory_cells_count };

	if (! e) // init window
	{
		lv_obj_t * cont = gui_win_get_content();

		static btn_t btns[btn_num];

		for(uint8_t i = 0; i < btn_num; i++)
		{
			unsigned freq = load_mems(i, 0);
			if (freq > 0)
			{
				btns[i].payload = (1 << 31) | i;
				snprintf(btns[i].text, 20, "%dk", freq / 1000);
			}
			else
			{
				btns[i].payload = (0 << 31) | i;
				snprintf(btns[i].text, 20, "---");
			}
		}

		create_button_matrix(cont, btns, "", btn_num, 5, s86x44, win_memory_handler);

		lv_obj_t * obj = lv_obj_create(cont);
		lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 0, 5, LV_GRID_ALIGN_STRETCH, 4, 1);
		lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);
		lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_set_style_border_width(obj, 0, 0);

		lv_obj_t * label = lv_label_create(obj);
		lv_label_set_text(label, "Long press on empty cell - save, on saved cell - clean");
		lv_obj_set_style_text_color(label, lv_color_white(), 0);
		lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
		lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

		return;
	}

	lv_obj_t * btn = (lv_obj_t *) lv_event_get_target(e);
	btn_t * btnu = lv_obj_get_user_data(btn);

	uint8_t idx = btnu->payload & 0xff;
	uint8_t p = (btnu->payload >> 31) & 0xff;

	if (btnu->is_clicked && p)
	{
		load_mems(idx, 1);
		win_close();
	}
	else if (btnu->is_long_pressed)
	{
		if (p)
		{
			btnu->payload = (0 << 31) | idx;
			clean_mems(idx);
			lv_obj_t * label = lv_obj_get_child(btn, 0);
			lv_label_set_text_fmt(label, "---");
		}
		else
		{
			btnu->payload = (1 << 31) | idx;
			uint32_t freq = hamradio_get_freq_rx();
			write_mems(idx, freq);
			lv_obj_t * label = lv_obj_get_child(btn, 0);
			lv_label_set_text_fmt(label, "%dk", freq / 1000);
		}
	}
}

// ***********************************************

void win_wnb_handler(lv_event_t * e)
{
	static lv_obj_t  * label, * slider;

	if (! e) // init window
	{
		lv_obj_t * cont = gui_win_get_content();

		slider = lv_slider_create(cont);
		lv_obj_center(slider);
		lv_obj_set_style_anim_duration(slider, 2000, 0);
		label = lv_label_create(cont);

		uint8_t t = wnb_get_threshold();
		lv_label_set_text_fmt(label, "%d", t);
		lv_obj_align_to(label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

		uint16_t min, max;
		wnb_get_limits(& min, & max);
		lv_slider_set_range(slider, min, max);
		lv_slider_set_value(slider, t, 0);
		lv_obj_add_event_cb(slider, win_wnb_handler, LV_EVENT_VALUE_CHANGED, NULL);

		return;
	}

	lv_event_code_t code = lv_event_get_code(e);
	if (code != LV_EVENT_VALUE_CHANGED) return;

	wnb_set_threshold(lv_slider_get_value(slider));

	lv_label_set_text_fmt(label, "%d", wnb_get_threshold());
	lv_obj_align_to(label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

// ***********************************************

void win_af_filter_handler(lv_event_t * e)
{
	static lv_obj_t * label_h, * label_l;
	static uint8_t h_mul, bp_type;
	const char * label_text[2][2] = { "Width", "Pitch", "Low cut", "High cut" };

	if (! e) // init window
	{
		lv_obj_t * cont = gui_win_get_content();

		uint16_t low = hamradio_get_low_bp(0);
		uint16_t high = hamradio_get_high_bp(0);

		static btn_t btns_h [] = {
				{ "-", -1,  0, NULL, },
				{ "+", 1, 0, NULL, },
		};

		static btn_t btns_l [] = {
				{ "-", -1,  0, NULL, },
				{ "+", 1, 0, NULL, },
		};

		bp_type = !! hamradio_get_bp_type_wide();
		h_mul = bp_type ? 100 : 10;

		static int32_t cols_dsc[] = { 120, 100, LV_GRID_TEMPLATE_LAST };
		static int32_t rows_dsc[] = { 40, 40, LV_GRID_TEMPLATE_LAST };
		lv_obj_set_style_grid_column_dsc_array(cont, cols_dsc, 0);
		lv_obj_set_style_grid_row_dsc_array(cont, rows_dsc, 0);
		lv_obj_center(cont);
		lv_obj_set_layout(cont, LV_LAYOUT_GRID);

		label_l = lv_label_create(cont);
		lv_obj_add_style(label_l, & winlblst, 0);
		lv_obj_set_grid_cell(label_l, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);
		lv_label_set_text_fmt(label_l, "%s: %d", label_text[bp_type][0], low * 10);

		label_h = lv_label_create(cont);
		lv_obj_add_style(label_h, & winlblst, 0);
		lv_obj_set_grid_cell(label_h, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 1);
		lv_label_set_text_fmt(label_h, "%s: %d", label_text[bp_type][1], high * h_mul);

		lv_obj_t * cont_l = lv_obj_create(cont);
		lv_obj_set_style_pad_all(cont_l, 0, 0);
		lv_obj_set_style_bg_opa(cont_l, LV_OPA_TRANSP, 0);
		lv_obj_set_style_border_width(cont_l, 0, 0);
		lv_obj_set_grid_cell(cont_l, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);
		create_button_matrix(cont_l, btns_l, "btns_l", 2, 2, s40x40, win_af_filter_handler);

		lv_obj_t * cont_h = lv_obj_create(cont);
		lv_obj_set_style_pad_all(cont_h, 0, 0);
		lv_obj_set_style_bg_opa(cont_h, LV_OPA_TRANSP, 0);
		lv_obj_set_style_border_width(cont_h, 0, 0);
		lv_obj_set_grid_cell(cont_h, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 1, 1);
		create_button_matrix(cont_h, btns_h, "btns_h", 2, 2, s40x40, win_af_filter_handler);

		return;
	}

	// todo: добавить графический вывод фильтра

	lv_event_code_t code = lv_event_get_code(e);

	lv_obj_t * btn = (lv_obj_t *) lv_event_get_target(e);
	btn_t * btnu = lv_obj_get_user_data(btn);

	if (btnu->is_clicked)
	{
		if (strstr(btnu->name, "btns_h"))		// high
		{
			uint8_t hh = hamradio_get_high_bp(btnu->payload);
			lv_label_set_text_fmt(label_h, "%s: %d", label_text[bp_type][1], hh * h_mul);
		}
		else if (strstr(btnu->name, "btns_l"))	// low
		{
			uint8_t ll = hamradio_get_low_bp(btnu->payload * (bp_type ? 10 : 5));
			lv_label_set_text_fmt(label_l, "%s: %d", label_text[bp_type][0], ll * 10);
		}
	}
}

// ***********************************************

void win_receive_handler(lv_event_t * e)
{
	enum { btn_num = 5 };

	if (! e) // init window
	{
		lv_obj_t * cont = gui_win_get_content();

		static btn_t btnm [btn_num] = {
				{ "Att", 	0, 0, NULL, },
				{ "Preamp", 0, 0, NULL, },
				{ "DNR", 	0, 0, NULL, },
				{ "WNB", 	0, 1, NULL, },
				{ "Filter",	0, 0, NULL, },
		};

		create_button_matrix(cont, btnm, "btnm", btn_num, 4, s100x44, win_receive_handler);

		lv_obj_t * btn = find_button(cont, "btnm_WNB");
		button_set_lock(btn, wnb_state_switch(0));

		btn = find_button(cont, "btnm_DNR");
		button_set_lock(btn, hamradio_change_nr(0));

		btn = find_button(cont, "btnm_Preamp");
		button_set_lock(btn, hamradio_change_preamp(0));

		return;
	}

	lv_obj_t * btn = (lv_obj_t *) lv_event_get_target(e);
	btn_t * btnu = lv_obj_get_user_data(btn);

	if (btnu->h)
	{
		btnu->h(btn);
	}
	else if (btnu->is_clicked)
	{
		switch(btnu->index)
		{
		case 0:
			hamradio_change_att();
			break;

		case 1:
			button_set_lock(btn, hamradio_change_preamp(1));
			break;

		case 2:
			button_set_lock(btn, hamradio_change_nr(1));
			break;

		case 3:
			button_set_lock(btn, wnb_state_switch(1));
			break;

		case 4:
			win_open(WIN_AF);
			break;

		default:
			break;
		}
	}
	else if (btnu->is_long_pressed)
	{
		if (btnu->index == 3)
			win_open(WIN_WNB);
	}
}

// ***********************************************

static void btn_txrx_handler(lv_obj_t * p)
{
	btn_t * ext = lv_obj_get_user_data(p);

	if (ext->is_long_pressed)
	{
		hamradio_tunemode(1);
	}
	else
	{
		hamradio_moxmode(1);
	}

	bool tune = hamradio_tunemode(0);
	bool mox = hamradio_moxmode(0);

	if (tune)
	{
		button_set_text(p, "Tune");
		button_lock(p);
	}
	else if (! tune && mox)
	{
		button_set_text(p, "TX");
		button_lock(p);
	}
	else
	{
		button_set_text(p, "RX");
		button_unlock(p);
	}

}

// ***********************************************

static void btn_footer_handler(lv_event_t * e)
{
	lv_obj_t * btn = (lv_obj_t *) lv_event_get_target(e);
	btn_t * fb = lv_obj_get_user_data(btn);

	if (fb->is_clicked)
	{
		if (fb->h)
			fb->h(btn);
		else
			win_open(fb->win_id);
	}
	else if (fb->is_long_pressed && fb->h)
		fb->h(btn);
}

static void footer_buttons_init(lv_obj_t * p)
{
	static btn_t footer_buttons [] = {
			{ "RX", 	  0, 1, btn_txrx_handler, },
			{ "Modes", 	  0, 0, NULL, WIN_MODES, },
			{ "Memory",   0, 0, NULL, WIN_MEMORY, },
			{ "Receive",  0, 0, NULL, WIN_RECEIVE, },
			{ "Button 4", 0, 0, NULL, WIN_MEMORY, },
			{ "Button 5", 0, 0, NULL, WIN_MEMORY, },
			{ "Button 6", 0, 0, NULL, WIN_MEMORY, },
			{ "Button 7", 0, 0, NULL, WIN_MEMORY, },
			{ "Button 8", 0, 0, NULL, WIN_MEMORY, },
	};

	lv_obj_t * cont = lv_obj_create(p);
	lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_style_pad_all(cont, 0, 0);

	lv_obj_align(cont, LV_ALIGN_BOTTOM_LEFT, 1, -1);
	lv_obj_add_style(cont, & fcontst, 0);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_layout(cont, LV_LAYOUT_GRID);

	static const lv_coord_t col_dsc[] = { fbtn_w, fbtn_w, fbtn_w, fbtn_w,
			fbtn_w, fbtn_w, fbtn_w, fbtn_w, fbtn_w, LV_GRID_TEMPLATE_LAST };
	lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);

	static int32_t row_dsc[] = { fbtn_h, LV_GRID_TEMPLATE_LAST };
	lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

	for (int i = 0; i < ARRAY_SIZE(footer_buttons); i++) {
		lv_obj_t * btn = lv_button_create(cont);
		lv_obj_add_style(btn, & btnst, 0);
		lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, i, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

		btn_t * fb = & footer_buttons[i];
		fb->payload = i;
		fb->eh = btn_footer_handler;

		lv_obj_set_user_data(btn, fb);
		lv_obj_add_event_cb(btn, buttons_handler, LV_EVENT_ALL, NULL);

		lv_obj_t * lbl = lv_label_create(btn);
		lv_label_set_text(lbl, fb->text);
		lv_obj_add_style(lbl, & lblst, 0);
	}
}

// ***********************************************

static infobar_t infobar;

const uint8_t infobar_places[infobar_count] = {
		INFOBAR_DUMMY,
		INFOBAR_DUMMY | infobar_noaction,
		INFOBAR_DUMMY,
		INFOBAR_DNR | infobar_switch | infobar_need_update,
		INFOBAR_DUMMY,
		INFOBAR_DUMMY,
		INFOBAR_CPU_TEMP | infobar_noaction | infobar_need_update,
		INFOBAR_DUMMY
};

static void infobar_action(uint8_t index)
{
	switch(index)
	{
	case INFOBAR_DNR:
	{
		hamradio_change_nr(1);
		break;
	}

	default:
		break;
	}
}

static void infobar_lock(lv_obj_t * btn, uint8_t v)
{
	if (v)
		lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_ORANGE), 0);
	else
		lv_obj_set_style_bg_color(btn, lv_color_black(), 0);
}

static void info_button_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);
    btn_t * ext = lv_obj_get_user_data(btn);
    uint8_t btn_index = ext->index;
    lv_obj_t * p = infobar.popups[btn_index];
    uint8_t sw = (infobar_places[btn_index] >> infobar_switch_pos) & 1;

    if (sw)
    	infobar_action(ext->payload);
    else
    {
		if (infobar.active_popup_index == btn_index)
		{
			lv_obj_add_flag(p, LV_OBJ_FLAG_HIDDEN);
			infobar.active_popup_index = 0xFF;

			lv_obj_t * cont = lv_obj_get_parent(btn);
			uint32_t child_cnt = lv_obj_get_child_count(cont);
			for(uint32_t i = 0; i < child_cnt; i ++)
			{
				lv_obj_t * child = lv_obj_get_child(cont, i);
				if (lv_obj_check_type(child, &lv_button_class))
					lv_obj_add_flag(child, LV_OBJ_FLAG_CLICKABLE);
			}
		}
		else
		{
			lv_obj_t * cont = lv_obj_get_parent(btn);
			uint32_t child_cnt = lv_obj_get_child_count(cont);
			for(uint32_t i = 0; i < child_cnt; i ++)
			{
				lv_obj_t * child = lv_obj_get_child(cont, i);
				if (lv_obj_check_type(child, & lv_button_class))
					lv_obj_clear_flag(child, LV_OBJ_FLAG_CLICKABLE);
			}

			lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);

			infobar.active_popup_index = btn_index;

			if (btn_index == infobar_count - 1)
				lv_obj_align_to(p, btn, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 5);
			else
				lv_obj_align_to(p, btn, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

			lv_obj_clear_flag(p, LV_OBJ_FLAG_HIDDEN);
		}
    }
}

static void popup_dummy(uint8_t i)
{
	infobar.popups[i] = lv_obj_create(gui_get_main());

	lv_obj_t * p = infobar.popups[i];
	lv_obj_set_style_bg_color(p, lv_color_hex(0x333333), 0);
	lv_obj_set_style_border_color(p, lv_color_white(), 0);
	lv_obj_set_style_border_width(p, 1, 0);

	char txt[32];
	snprintf(txt, sizeof(txt), "Popup\n %d", i + 1);
	lv_obj_t * label = lv_label_create(p);
	lv_label_set_text(label, txt);
	lv_obj_add_style(label, & winlblst, 0);

	lv_obj_add_flag(p, LV_OBJ_FLAG_HIDDEN);
}

static void infobar_init(lv_obj_t * p)
{
	infobar.active_popup_index = 0xFF;

	lv_obj_t * cont = lv_obj_create(p);
	lv_obj_set_size(cont, DIM_X, 40);
	lv_obj_set_pos(cont, 0, 130);

	lv_obj_set_style_pad_all(cont, 0, 0);
	lv_obj_set_style_pad_gap(cont, 0, 0);
	lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
	lv_obj_set_style_border_width(cont, 0, 0);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_layout(cont, LV_LAYOUT_GRID);

	static lv_coord_t col_dsc[] = {
			LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
			LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
			LV_GRID_TEMPLATE_LAST };
	static lv_coord_t row_dsc[] = { LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };

	lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
	lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

	static btn_t ext[infobar_count];

	for (int i = 0; i < infobar_count; i++)
	{
		lv_obj_t * btn = lv_button_create(cont);
		lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, i, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

		lv_obj_set_style_bg_color(btn, lv_color_black(), 0);
		lv_obj_set_style_border_width(btn, 2, 0);
		lv_obj_set_style_border_color(btn, lv_palette_main(LV_PALETTE_GREEN), 0);
		lv_obj_set_style_radius(btn, 0, 0);

		if(i == 0)
			lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_RIGHT, 0);
		else if(i == 7)
			lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_LEFT, 0);
		else
			lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT, 0);

		char txt[32];
		lv_obj_t * label = lv_label_create(btn);

		uint8_t place = infobar_places[i] & infobar_valid_mask;
		uint8_t need_update = (infobar_places[i] >> infobar_need_update_pos) & 1;

		if (need_update)
			add_to_update(btn);

		if (place)
			ext[i].payload = place;
		else
		{
			snprintf(txt, sizeof(txt), "Btn\n%d", i + 1);
			lv_label_set_text(label, txt);
			lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
			lv_obj_center(label);
			popup_dummy(i);
		}

		uint8_t clickable = ! ((infobar_places[i] >> infobar_noaction_pos) & 1);
		if (clickable)
		{
			ext[i].index = i;
			lv_obj_add_event_cb(btn, info_button_event_cb, LV_EVENT_CLICKED, NULL);
		}

		lv_obj_set_user_data(btn, & ext[i]);
	}

	gui_update();
}

// ***********************************************

void gui_update(void)
{
	char buf[32];

	for (int i = 0; i < update_count; i ++)
	{
		lv_obj_t * p = to_update[i];
		btn_t * ext = lv_obj_get_user_data(p);

		switch(ext->payload)
		{
		case INFOBAR_CPU_TEMP:
		{
#if defined (GET_CPU_TEMPERATURE)
			float cpu_temp = GET_CPU_TEMPERATURE();
			snprintf(buf, sizeof(buf), "CPU temp\n%2.1f", cpu_temp);
			button_set_text(p, buf);
#endif /* defined (GET_CPU_TEMPERATURE) */
			break;
		}
		case INFOBAR_DNR:
		{
			uint8_t s = hamradio_change_nr(0);
			snprintf(buf, sizeof(buf), "DNR\n%s", s ? "on" : "off");
			button_set_text(p, buf);
			infobar_lock(p, s);
			break;
		}

		default:
			break;
		}
	}
}

void lvgl_gui_init(lv_obj_t * parent)
{
	init_gui_styles();
	gui_set_parent(parent);
	footer_buttons_init(parent);
	infobar_init(parent);
}

#endif /* WITHLVGL && LINUX_SUBSYSTEM */
