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

#include "lvgl_gui.h"
#include "windows.h"
#include "linux/common.h"

void win_modes_handler(lv_event_t * e)
{
	enum { btn_num = 8 };

	if (! e) // init window
	{
		lv_obj_t * cont = gui_win_get_content();

		static user_t btns [btn_num] = {
				{ "LSB", SUBMODE_LSB, }, { "AM", SUBMODE_AM, },
				{ "CW",  SUBMODE_CW, },  { "DGL", SUBMODE_DGL, },
				{ "USB", SUBMODE_USB, }, { "NFM", SUBMODE_NFM, },
				{ "CWR", SUBMODE_CWR, }, { "DGU", SUBMODE_DGU, },
		};

		create_button_matrix(cont, btns, "", btn_num, 4, s86x44, win_modes_handler);

		return;
	}

	lv_obj_t * btn = (lv_obj_t *) lv_event_get_target(e);
	user_t * btnu = lv_obj_get_user_data(btn);

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

		static user_t btns[btn_num];
		memset(btns, 0, sizeof(btns));

		for(uint8_t i = 0; i < btn_num; i++)
		{
			unsigned freq = load_mems(i, 0);
			if (freq > 0)
			{
				btns[i].payload = (1 << 31) | i;
				snprintf(btns[i].text, 20, "%dk", freq / 1000);

#if WITHAD936XIIO || WITHAD936XDEV
			if ((get_ad936x_stream_status() && freq < NOXVRTUNE_TOP) ||
					(! get_ad936x_stream_status() && freq > NOXVRTUNE_TOP))
				btns[i].state = DISABLED;
#endif /* WITHAD936XIIO || WITHAD936XDEV*/
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
	user_t * btnu = lv_obj_get_user_data(btn);

	if (lv_obj_check_type(btn, & lv_button_class))
	{
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

	lv_obj_t * obj = (lv_obj_t *) lv_event_get_target(e);
	if (lv_obj_check_type(obj, & lv_slider_class))
	{
		wnb_set_threshold(lv_slider_get_value(slider));

		lv_label_set_text_fmt(label, "%d", wnb_get_threshold());
		lv_obj_align_to(label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	}
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

		static user_t btns_h [] = {
				{ "-", -1,  0, NULL, },
				{ "+", 1, 0, NULL, },
		};

		static user_t btns_l [] = {
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
	user_t * btnu = lv_obj_get_user_data(btn);

	if (lv_obj_check_type(btn, & lv_button_class))
	{
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
}

// ***********************************************

void win_receive_handler(lv_event_t * e)
{
	enum { btn_num = 5 };

	if (! e) // init window
	{
		lv_obj_t * cont = gui_win_get_content();

		static user_t btnm [btn_num] = {
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
	user_t * btnu = lv_obj_get_user_data(btn);

	if (lv_obj_check_type(btn, & lv_button_class))
	{
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
}

// ***********************************************

void win_tx_power_handler(lv_event_t * e)
{
	static lv_obj_t * slider_tx, * slider_tune, * lbl_tx, * lbl_tune;

	if (! e) // init window
	{
		lv_obj_t * cont = gui_win_get_content();
		uint8_t tx_pwr = hamradio_get_tx_power();
		uint8_t tune_pwr = hamradio_get_tx_tune_power();

		uint_fast8_t power_min, power_max;
		hamradio_get_tx_power_limits(& power_min, & power_max);

		lbl_tx = lv_label_create(cont);
		lv_obj_add_style(lbl_tx, & winlblst, 0);
		lv_obj_align(lbl_tx, LV_ALIGN_TOP_LEFT, 0, 0);
		lv_label_set_text_fmt(lbl_tx, "TX %d\%%", tx_pwr);

		slider_tx = lv_slider_create(cont);
		lv_obj_align_to(slider_tx, lbl_tx, LV_ALIGN_OUT_RIGHT_MID, 50, 0);
		lv_obj_set_style_anim_duration(slider_tx, 2000, 0);
		lv_slider_set_value(slider_tx, tx_pwr, 0);
		lv_slider_set_range(slider_tx, power_min, power_max);
		lv_obj_add_event_cb(slider_tx, win_tx_power_handler, LV_EVENT_VALUE_CHANGED, NULL);

		lbl_tune = lv_label_create(cont);
		lv_obj_add_style(lbl_tune, & winlblst, 0);
		lv_obj_align_to(lbl_tune, lbl_tx, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 30);
		lv_label_set_text_fmt(lbl_tune, "Tune %d\%%", tune_pwr);

		slider_tune = lv_slider_create(cont);
		lv_obj_align_to(slider_tune, slider_tx, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 30);
		lv_obj_set_style_anim_duration(slider_tune, 2000, 0);
		lv_slider_set_value(slider_tune, tune_pwr, 0);
		lv_slider_set_range(slider_tune, power_min, power_max);
		lv_obj_add_event_cb(slider_tune, win_tx_power_handler, LV_EVENT_VALUE_CHANGED, NULL);

		return;
	}

	lv_event_code_t code = lv_event_get_code(e);
	if (code != LV_EVENT_VALUE_CHANGED) return;

	lv_obj_t * obj = (lv_obj_t *) lv_event_get_target(e);
	if (lv_obj_check_type(obj, & lv_slider_class))
	{
		hamradio_set_tx_power(lv_slider_get_value(slider_tx));
		hamradio_set_tx_tune_power(lv_slider_get_value(slider_tune));

		lv_label_set_text_fmt(lbl_tx, "TX %d\%%", hamradio_get_tx_power());
		lv_label_set_text_fmt(lbl_tune, "Tune %d\%%", hamradio_get_tx_tune_power());
	}
}

// ***********************************************

static void btn_settings_handler(lv_obj_t * p)
{
	static lv_obj_t * menu = NULL;

	if (menu)
	{
		lv_obj_del(menu);
		menu = NULL;
	}
	else
	{
		menu = lv_hamradiomenu_create(gui_get_main());
		lv_obj_set_size(menu, DIM_X, 265);
		lv_obj_set_pos(menu, 0, 170);
	}
}

void win_settings_handler(lv_event_t * e)
{
	if (! e) // init window
	{
		lv_obj_t * cont = gui_win_get_content();

		static user_t btnm [] = {
				{ "System\nsettings", 	0, 0, btn_settings_handler, },
//				{ "Audio\nsettings", 	0, 0, NULL, },
//				{ "Transmit\nsettings", 0, 0, NULL, },
				{ "Display\nsettings", 	0, 0, NULL, WIN_DISPLAY_SETTINGS, },
//				{ "GUI\nsettings",		0, 0, NULL, },
				{ "Terminate\nprogram",	100, 0, NULL, },
		};

		create_button_matrix(cont, btnm, "", ARRAY_SIZE(btnm), 4, s100x44, win_settings_handler);

		return;
	}

	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * btn = (lv_obj_t *) lv_event_get_target(e);
	user_t * btnu = lv_obj_get_user_data(btn);

	if (lv_obj_check_type(btn, & lv_button_class))
	{
		if (btnu->payload == 100)
			linux_exit();
		else if (btnu->win_id)
			win_open(btnu->win_id);
		else if (btnu->h)
		{
			win_close();
			btnu->h(btn);
		}
	}
}

// ***********************************************

void win_display_settings_handler(lv_event_t * e)
{
	char txt[32];
	static lv_obj_t * slider_top, * slider_bottom, * lbl_top, * lbl_bottom;

	if (! e) // init window
	{
		lv_obj_t * cont = gui_win_get_content();
		static user_t ext[2];

		uint8_t topsp = hamradio_get_gtopdbspe();
		uint8_t bottomsp = hamradio_get_gbottomdbspe();

		lbl_top = lv_label_create(cont);
		lv_obj_add_style(lbl_top, & winlblst, 0);
		lv_obj_align(lbl_top, LV_ALIGN_TOP_LEFT, 0, 0);
		lv_label_set_text_fmt(lbl_top, "Top: %d db", topsp);

		slider_top = lv_slider_create(cont);
		lv_obj_align_to(slider_top, lbl_top, LV_ALIGN_OUT_RIGHT_MID, 50, 0);
		lv_obj_set_style_anim_duration(slider_top, 2000, 0);
		lv_slider_set_range(slider_top, WITHTOPDBMIN, WITHTOPDBMAX);
		lv_slider_set_value(slider_top, topsp, 0);
		lv_obj_add_event_cb(slider_top, win_display_settings_handler, LV_EVENT_VALUE_CHANGED, NULL);

		lbl_bottom = lv_label_create(cont);
		lv_obj_add_style(lbl_bottom, & winlblst, 0);
		lv_obj_align_to(lbl_bottom, lbl_top, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 30);
		lv_label_set_text_fmt(lbl_bottom, "Bottom: %d db", bottomsp);

		slider_bottom = lv_slider_create(cont);
		lv_obj_align_to(slider_bottom, slider_top, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 30);
		lv_obj_set_style_anim_duration(slider_bottom, 2000, 0);
		lv_slider_set_range(slider_bottom, WITHBOTTOMDBMIN, WITHBOTTOMDBMAX);
		lv_slider_set_value(slider_bottom, bottomsp, 0);
		lv_obj_add_event_cb(slider_bottom, win_display_settings_handler, LV_EVENT_VALUE_CHANGED, NULL);

		lv_obj_t * btn_view = add_button(cont, ext, 0, "View", s100x44, win_display_settings_handler);
		snprintf(txt, sizeof(txt), "View\n%s", hamradio_change_view_style(0));
		button_set_text(btn_view, txt);
		lv_obj_align_to(btn_view, lbl_bottom, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

		lv_obj_t * btn_zoom = add_button(cont, ext, 1, "Zoom", s100x44, win_display_settings_handler);
		snprintf(txt, sizeof(txt), "Zoom\nx%d", 1 << hamradio_get_gzoomxpow2());
		button_set_text(btn_zoom, txt);
		lv_obj_align_to(btn_zoom, btn_view, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

		return;
	}

	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = (lv_obj_t *) lv_event_get_target(e);
	user_t * ext = lv_obj_get_user_data(obj);

	if (lv_obj_check_type(obj, & lv_button_class))
	{
		if (ext->index == 0)
		{
			snprintf(txt, sizeof(txt), "View\n%s", hamradio_change_view_style(1));
			button_set_text(obj, txt);
		}
		else if (ext->index == 1)
		{
			uint8_t z = (hamradio_get_gzoomxpow2() + 1) % (BOARD_FFTZOOM_POW2MAX + 1);
			hamradio_set_gzoomxpow2(z);
			snprintf(txt, sizeof(txt), "Zoom\nx%d", 1 << hamradio_get_gzoomxpow2());
			button_set_text(obj, txt);
		}
	}
	else if (lv_obj_check_type(obj, & lv_slider_class))
	{
		uint8_t topsp = lv_slider_get_value(slider_top);
		uint8_t bottomsp = lv_slider_get_value(slider_bottom);

		hamradio_set_gtopdbspe(topsp);
		hamradio_set_gbottomdbspe(bottomsp);

		lv_label_set_text_fmt(lbl_top, "Top: %d db", topsp);
		lv_label_set_text_fmt(lbl_bottom, "Bottom: %d db", bottomsp);
	}
}

// ***********************************************

void win_bands_handler(lv_event_t * e)
{
	static band_array_t * bands = NULL;	//	добавить free по событию
	static user_t * ext = NULL;

	if (! e) // init window
	{
		lv_obj_t * cont = gui_win_get_content();

		static const lv_coord_t cols_dsc[8] = { 86, 86, 86, 40,
				86, 86, 86, LV_GRID_TEMPLATE_LAST};
		static const lv_coord_t rows_dsc[10] = { 44, 44, 44, 44, 44, 44,
				44, 44, 44, LV_GRID_TEMPLATE_LAST };

		lv_obj_set_style_grid_column_dsc_array(cont, cols_dsc, 0);
		lv_obj_set_style_grid_row_dsc_array(cont, rows_dsc, 0);
		lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_center(cont);
		lv_obj_set_layout(cont, LV_LAYOUT_GRID);

		uint8_t bandnum = hamradio_get_bands(NULL, 1, 1);
		bands = (band_array_t *) calloc(bandnum, sizeof (band_array_t));
		ASSERT(bands);
		hamradio_get_bands(bands, 0, 1);

		ext = (user_t *) calloc(bandnum + 1, sizeof (user_t));
		ASSERT(ext);

		int i = 0, j = 0, p = 0, n = 0, d = 0;
		for (; i < bandnum; i ++, j ++)
		{
			int8_t row = ((i - d) / 3) - n;
			int8_t col = (j % 3) + p;

			if (! p && bands [i].type != BAND_TYPE_HAM)
			{
				ext[bandnum].payload = INT32_MAX;
				lv_obj_t * btn = add_button(cont, ext, bandnum, "Freq\nenter", s86x44, win_bands_handler);
				lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
				p = 4; // переделать
				n = row;
				j = -1;
				d = 1;
				i --;

				continue;
			}

			ext[i].payload = bands[i].init_freq;

			char * div = strchr(bands[i].name, ' ');
			if(div) memcpy(div, "\n", 1);

			lv_obj_t * btn = add_button(cont, ext, i, bands[i].name, s86x44, win_bands_handler);
			lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);

			if (hamradio_check_current_freq_by_band(bands[i].index))
				button_lock(btn);

#if WITHAD936XIIO || WITHAD936XDEV
			if ((get_ad936x_stream_status() && ext[i].payload < NOXVRTUNE_TOP) ||
					(! get_ad936x_stream_status() && ext[i].payload > NOXVRTUNE_TOP))
				lv_obj_set_state(btn, LV_STATE_DISABLED, 1);
#endif /* WITHAD936XIIO || WITHAD936XDEV*/
		}

		return;
	}

	lv_obj_t * btn = (lv_obj_t *) lv_event_get_target(e);
	user_t * btnu = lv_obj_get_user_data(btn);

	if (lv_obj_check_type(btn, & lv_button_class))
	{
		if (btnu->payload == INT32_MAX)
		{
			win_open(WIN_FREQ);
		}
		else if (btnu->is_clicked)
		{
			hamradio_goto_band_by_freq(btnu->payload);
			win_close();
		}
	}
}

// ***********************************************

void win_freq_handler(lv_event_t * e)
{
	enum {
		BUTTON_CODE_BK = 20,
		BUTTON_CODE_OK = 30,
		BUTTON_CODE_DONE = 99
	};

	static editfreq_t editfreq;
	static lv_obj_t * label_freq = NULL;

	if (! e) // init window
	{
		editfreq.val = 0;
		editfreq.num = 0;
		editfreq.key = BUTTON_CODE_DONE;

		lv_obj_t * cont = gui_win_get_content();

		static user_t btns [] = {
				{ "1", 1, }, { "2", 2, }, { "3", 3, },
				{ "4", 4, }, { "5", 5, }, { "6", 6, },
				{ "7", 7, }, { "8", 8, }, { "9", 9, },
				{ "<-", BUTTON_CODE_BK, },
				{ "0", 0, },
				{ "OK", BUTTON_CODE_OK, },
		};

		label_freq = lv_label_create(cont);
		lv_obj_add_style(label_freq, & winlblst, 0);
		lv_obj_align(label_freq, LV_ALIGN_TOP_MID, 0, 0);
		lv_obj_set_style_text_font(label_freq, & lv_font_montserrat_20, 0);
		lv_label_set_text(label_freq, "Enter freq...");

		lv_obj_t * cont2 = lv_obj_create(cont);
		lv_obj_set_pos(cont2, 0, 40);
		lv_obj_set_style_pad_all(cont2, 0, 0);
		lv_obj_set_style_bg_opa(cont2, LV_OPA_TRANSP, 0);
		lv_obj_set_style_border_width(cont2, 0, 0);

		create_button_matrix(cont2, btns, "", ARRAY_SIZE(btns), 3, s40x40, win_freq_handler);

		return;
	}

	lv_obj_t * btn = (lv_obj_t *) lv_event_get_target(e);
	user_t * btnu = lv_obj_get_user_data(btn);

	if (lv_obj_check_type(btn, & lv_button_class))
	{
		if (btnu->is_clicked && editfreq.key == BUTTON_CODE_DONE)
			editfreq.key = btnu->payload;

		if (editfreq.key != BUTTON_CODE_DONE)
		{
			if (editfreq.error)
			{
				editfreq.val = 0;
				editfreq.num = 0;
			}

			editfreq.error = 0;

			switch (editfreq.key)
			{
			case BUTTON_CODE_BK:
				if (editfreq.num > 0)
				{
					editfreq.val /= 10;
					editfreq.num --;
				}
				break;

			case BUTTON_CODE_OK:
				if (hamradio_set_freq(editfreq.val * 1000) || editfreq.val == 0)
					win_close();
				else
					editfreq.error = 1;

				break;

			default:
				if (editfreq.num < 7)
				{
					editfreq.val  = editfreq.val * 10 + editfreq.key;
					if (editfreq.val)
						editfreq.num ++;
				}
			}
			editfreq.key = BUTTON_CODE_DONE;

			if (editfreq.error)
				lv_label_set_text(label_freq, "ERROR");
			else
				lv_label_set_text_fmt(label_freq, "%d k", (int) editfreq.val);
		}
	}
}

// ***********************************************

void win_ad936xdev_handler(lv_event_t * e)
{
#if WITHAD936XDEV
	static lv_obj_t * slider_cic, * slider_fir, * lbl_cic, * lbl_fir;

	if (! e) // init window
	{
		lv_obj_t * cont = gui_win_get_content();
		uint8_t p = ad936xdev_present();
		uint8_t s = get_ad936x_stream_status();
		static user_t ext[2];

		lbl_cic = lv_label_create(cont);
		lv_obj_add_style(lbl_cic, & winlblst, 0);
		lv_obj_align(lbl_cic, LV_ALIGN_TOP_LEFT, 0, 0);
		lv_label_set_text_fmt(lbl_cic, "CIC %d", ad936x_iq_shift_cic_rx(0));

		slider_cic = lv_slider_create(cont);
		lv_obj_align_to(slider_cic, lbl_cic, LV_ALIGN_OUT_RIGHT_MID, 50, 0);
		lv_obj_set_style_anim_duration(slider_cic, 2000, 0);
		lv_slider_set_value(slider_cic, ad936x_iq_shift_cic_rx(0), 0);
		lv_slider_set_range(slider_cic, rx_cic_shift_min, rx_cic_shift_max);
		lv_obj_add_event_cb(slider_cic, win_ad936xdev_handler, LV_EVENT_VALUE_CHANGED, NULL);

		lbl_fir = lv_label_create(cont);
		lv_obj_add_style(lbl_fir, & winlblst, 0);
		lv_obj_align_to(lbl_fir, lbl_cic, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 30);
		lv_label_set_text_fmt(lbl_fir, "FIR %d", ad936x_iq_shift_fir_rx(0));

		slider_fir = lv_slider_create(cont);
		lv_obj_align_to(slider_fir, slider_cic, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 30);
		lv_obj_set_style_anim_duration(slider_fir, 2000, 0);
		lv_slider_set_value(slider_fir, ad936x_iq_shift_fir_rx(0), 0);
		lv_slider_set_range(slider_fir, rx_fir_shift_min, rx_fir_shift_max);
		lv_obj_add_event_cb(slider_fir, win_ad936xdev_handler, LV_EVENT_VALUE_CHANGED, NULL);

		lv_obj_t * btn_sw = add_button(cont, ext, 0, "", s100x44, win_ad936xdev_handler);
		lv_obj_align_to(btn_sw, lbl_fir, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 30);

		if (s)
			button_set_text(btn_sw, "Switch\nto HF");
		else
		{
			button_set_text(btn_sw, p ? "Switch\nto UHF" : "AD936x\nnot found");
			lv_obj_set_state(btn_sw, LV_STATE_DISABLED, ! p);
		}

		ext[1].payload = ad936xdev_get_fir_state();
		lv_obj_t * btn_fir = add_button(cont, ext, 1, "FIR", s100x44, win_ad936xdev_handler);
		button_set_lock(btn_fir, ext[1].payload);
		lv_obj_set_state(btn_fir, LV_STATE_DISABLED, ! s);
		lv_obj_align_to(btn_fir, btn_sw, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

		return;
	}

	lv_obj_t * obj = (lv_obj_t *) lv_event_get_target(e);
	user_t * ext = lv_obj_get_user_data(obj);
	static uint32_t freq = 7012000;

	if (lv_obj_check_type(obj, & lv_button_class))
	{
		if (ext->is_clicked)
		{
			switch (ext->index)
			{
			case 0:
				if (get_ad936x_stream_status())
				{
					button_set_text(obj, "Switch\nto UHF");
					ad936xdev_sleep();
					hamradio_set_freq(freq);
				}
				else
				{
					button_set_text(obj, "Switch\nto HF");
					ad936xdev_wake();
					freq = hamradio_get_freq_rx();
					hamradio_set_freq(433000000);
				}

				win_close();
				break;

			case 1:
				ext->payload = ! ext->payload;
				ad936xdev_set_fir_state(ext->payload);
				button_set_lock(obj, ext->payload);
				break;

			default:
				break;
			}
		}
	}
	else if (lv_obj_check_type(obj, & lv_slider_class))
	{
		uint8_t cic_shift = lv_slider_get_value(slider_cic);
		uint8_t fir_shift = lv_slider_get_value(slider_fir);

		ad936x_iq_shift_cic_rx(cic_shift);
		ad936x_iq_shift_fir_rx(fir_shift);

		lv_label_set_text_fmt(lbl_cic, "CIC %d", cic_shift);
		lv_label_set_text_fmt(lbl_fir, "FIR %d", fir_shift);
	}
#elif WITHAD936XIIO
	const char * status_str[3] = { "AD936x found", "Error", "Streaming" };
	const char * button_str[3] = { "Start", "Find", "Stop" };
	const char * gainmode_str[2] = { "Gain|manual", "Gain|auto" };
	const char uri[] = "usb:";
	static uint8_t status = 10;
	static lv_obj_t * lbl;

	if (! e) // init window
	{
		lv_obj_t * cont = gui_win_get_content();
		static user_t ext[1];
		status = iio_ad936x_find(uri);

		lbl = lv_label_create(cont);
		lv_obj_add_style(lbl, & winlblst, 0);
		lv_label_set_text_fmt(lbl, "State: %s", status_str[status]);
		lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 0);

		lv_obj_t * btn_sw = add_button(cont, ext, 0, "", s100x44, win_ad936xdev_handler);
		lv_obj_align_to(btn_sw, lbl, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
		button_set_text(btn_sw, button_str[status]);

		return;
	}

	lv_obj_t * obj = (lv_obj_t *) lv_event_get_target(e);
	user_t * ext = lv_obj_get_user_data(obj);

	if (lv_obj_check_type(obj, & lv_button_class))
	{
		if (ext->is_clicked)
		{
			if (status == 10 || status == 1)
				status = iio_ad936x_find(uri);
			else if (status == 0)
				status = iio_ad936x_start(uri);
			else if (status == 2)
				status = iio_ad936x_stop();

			lv_label_set_text_fmt(lbl, "State: %s", status_str[status]);
			button_set_text(obj, button_str[status]);
		}
	}
#endif /* WITHAD936XDEV */
}

// ***********************************************

static void btn_txrx_handler(lv_obj_t * p)
{
	user_t * ext = lv_obj_get_user_data(p);

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
	user_t * fb = lv_obj_get_user_data(btn);

	if (lv_obj_check_type(btn, & lv_button_class))
	{
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
}

static void footer_buttons_init(lv_obj_t * p)
{
	static user_t footer_buttons [] = {
			{ "RX", 	  0, 1, btn_txrx_handler, },
			{ "Modes", 	  0, 0, NULL, WIN_MODES, },
			{ "Memory",   0, 0, NULL, WIN_MEMORY, },
			{ "Receive",  0, 0, NULL, WIN_RECEIVE, },
			{ "Bands",    0, 0, NULL, WIN_BANDS, },
			{ "AD936x",   0, 0, NULL, WIN_AD936X, },
			{ "Button 6", 0, 0, NULL, WIN_MEMORY, },
			{ "Button 7", 0, 0, NULL, WIN_MEMORY, },
			{ "Settings", 0, 0, NULL, WIN_SETTINGS, },
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

	for (int i = 0; i < ARRAY_SIZE(footer_buttons); i ++)
	{
		lv_obj_t * btn = lv_button_create(cont);
		lv_obj_add_style(btn, & btnst, 0);
		lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, i, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

		user_t * fb = & footer_buttons[i];
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

void touch_zone_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_CLICKED && code != LV_EVENT_PRESSING && code != LV_EVENT_RELEASED)
    	return;

    lv_obj_t * obj = lv_event_get_target(e);
    lv_point_t pos;
    lv_indev_get_point(lv_indev_get_act(), & pos);
    static int16_t x_old = 0, v_x = 0;
    static uint8_t is_long_press = 0;

    if(code == LV_EVENT_CLICKED)
	{
		if (is_long_press)
		{
			is_long_press = 0;
			return;
		}

		lv_area_t coords;
		lv_obj_get_coords(obj, & coords);
		int16_t center_x = (coords.x1 + coords.x2) / 2;
		int16_t offset_x = pos.x - center_x;

		uint32_t f = hamradio_get_freq_rx(), bw = display2_zoomedbw();
		uint32_t fp = bw / DIM_X, fn = f + offset_x * fp;

		uint16_t step = 1000;
		uint32_t f_rem = fn % step;

		hamradio_set_freq(fn + (step - f_rem));
	}
    else if(code == LV_EVENT_PRESSING)
    {
        int16_t x = pos.x / 3;
        v_x = x_old ? x - x_old : 0;
        x_old = x;

		uint16_t step = 100;
		uint32_t freq = hamradio_get_freq_rx();
		uint32_t f_rem = freq % step;

		hamradio_set_freq(freq - (step + f_rem) * v_x);

		is_long_press = is_long_press ? 1 : v_x != 0;
    }
    else if(code == LV_EVENT_RELEASED)
    {
    	x_old = 0;
    	v_x = 0;
    }
}

void create_touch_zone(lv_obj_t * parent)
{
    lv_obj_t * touch_zone = lv_obj_create(parent);
    lv_obj_set_size(touch_zone, DIM_X, 260);
	lv_obj_set_pos(touch_zone, 0, 170);

    lv_obj_set_style_bg_opa(touch_zone, LV_OPA_0, 0);
    lv_obj_clear_flag(touch_zone, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_flag(touch_zone, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_border_width(touch_zone, 0, 0);

    lv_obj_add_event_cb(touch_zone, touch_zone_event_cb, LV_EVENT_ALL, NULL);
}

// ***********************************************

void gui_update(void)
{
	infobar_update();
}

// ***********************************************

void lvgl_gui_init(lv_obj_t * parent)
{
	init_gui_styles();
	gui_set_parent(parent);
	footer_buttons_init(parent);
	infobar_init(parent);
	create_touch_zone(parent);
}

#endif /* WITHLVGL && LINUX_SUBSYSTEM */
