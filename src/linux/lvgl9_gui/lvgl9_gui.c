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

static lv_obj_t * main_gui_obj = NULL, * current_window = NULL;
static lv_style_t mainst, fbtnst, fbtnlockst, fcontst, flbl, winst;
static uint8_t cid = 255;

static int32_t col_dsc_60[] = { 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST };
static int32_t col_dsc_80[] = { 80, 80, 80, 80, 80, 80, LV_GRID_TEMPLATE_LAST };
static int32_t row_dsc_40[] = { 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST };

void win_close(void);

static void long_press_timer_cb(lv_timer_t * timer)
{
	user_t * ext = lv_timer_get_user_data(timer);
    ext->is_pressed = false;
    ext->is_long_pressed = true;
    lv_timer_del(ext->long_press_timer);
}

static void buttons_handler(lv_event_t * e)
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

// ***********************************************

void win_modes_handler(lv_event_t * e)
{
	enum { btn_num = 8 };

	if (! e) // init window
	{
		const char * btnm_map[btn_num] =
		{
				"LSB", "USB", "CW",  "CWR",
				"AM",  "NFM", "DGL", "DGU",
		};

		static user_t btu[btn_num];

		lv_obj_t * cont = gui_win_get_content(current_window);

		lv_obj_set_style_grid_column_dsc_array(cont, col_dsc_60, 0);
		lv_obj_set_style_grid_row_dsc_array(cont, row_dsc_40, 0);
		lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_center(cont);
		lv_obj_set_layout(cont, LV_LAYOUT_GRID);

		for(uint8_t i = 0; i < btn_num; i++)
		{
			uint8_t col = i / 2;
			uint8_t row = i % 2;

			lv_obj_t * btn = lv_button_create(cont);
	        lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, col, 1,
	                             LV_GRID_ALIGN_STRETCH, row, 1);

	        lv_obj_t * label = lv_label_create(btn);
	        lv_label_set_text_fmt(label, "%s", btnm_map[i]);
	        lv_obj_center(label);

	        btu[i].payload = i;
	        lv_obj_set_user_data(btn, & btu[i]);
	        lv_obj_add_event_cb(btn, win_modes_handler, LV_EVENT_CLICKED, NULL);
		}

		return;
	}

	const int modes [btn_num] = {
			SUBMODE_LSB, SUBMODE_USB, SUBMODE_CW, SUBMODE_CWR,
			SUBMODE_AM, SUBMODE_NFM, SUBMODE_DGL, SUBMODE_DGU,
	};

	lv_event_code_t code = lv_event_get_code(e);
	if(code == LV_EVENT_CLICKED)
	{
		lv_obj_t * btn = lv_event_get_target_obj(e);
		user_t * btu = lv_obj_get_user_data(btn);
		hamradio_change_submode(modes[btu->payload], 1);
		win_close();
	}
}

// *****************************************************************

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
		lv_obj_t * cont = gui_win_get_content(current_window);

		lv_obj_set_style_grid_column_dsc_array(cont, col_dsc_80, 0);
		lv_obj_set_style_grid_row_dsc_array(cont, row_dsc_40, 0);
		lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_center(cont);
		lv_obj_set_layout(cont, LV_LAYOUT_GRID);

		static user_t btu[btn_num];

		for(uint8_t i = 0; i < btn_num; i++)
		{
			uint8_t row = i / 5;
			uint8_t col = i % 5;

			lv_obj_t * btn = lv_button_create(cont);
	        lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, col, 1,
	                             LV_GRID_ALIGN_STRETCH, row, 1);

	        snprintf(btu[i].name, 30, "btn_mem_%02d", i);
	        btu[i].eh = win_memory_handler;

	        lv_obj_t * label = lv_label_create(btn);

			unsigned freq = load_mems(i, 0);
			if (freq > 0)
			{
				btu[i].payload = (1 << 31) | i;
				lv_label_set_text_fmt(label, "%dk", freq / 1000);
			}
			else
			{
				btu[i].payload = (0 << 31) | i;
				lv_label_set_text_fmt(label, "---");
			}
	        lv_obj_center(label);

	        lv_obj_set_user_data(btn, & btu[i]);
	        lv_obj_add_event_cb(btn, buttons_handler, LV_EVENT_ALL, NULL);
		}

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
	user_t * ext = lv_obj_get_user_data(btn);

	uint8_t idx = ext->payload & 0xff;
	uint8_t p = (ext->payload >> 31) & 0xff;

	if (ext->is_clicked && p)
	{
		load_mems(idx, 1);
		win_close();
	}
	else if (ext->is_long_pressed)
	{
		if (p)
		{
			ext->payload = (0 << 31) | idx;
			clean_mems(idx);
			lv_obj_t * label = lv_obj_get_child(btn, 0);
			lv_label_set_text_fmt(label, "---");
		}
		else
		{
			ext->payload = (1 << 31) | idx;
			uint32_t freq = hamradio_get_freq_rx();
			write_mems(idx, freq);
			lv_obj_t * label = lv_obj_get_child(btn, 0);
			lv_label_set_text_fmt(label, "%dk", freq / 1000);
		}
	}
}

// *************************************************

void btn_txrx_handler(lv_obj_t * p)
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

	lv_obj_t * lbl = lv_obj_get_child(p, 0);
	bool tune = hamradio_tunemode(0);
	bool mox = hamradio_moxmode(0);

	if (tune)
	{
		lv_label_set_text_fmt(lbl, "%s", "Tune");
		lv_obj_remove_style_all(p);
		lv_obj_add_style(p, & fbtnlockst, 0);
	}
	else if (! tune && mox)
	{
		lv_label_set_text_fmt(lbl, "%s", "TX");
		lv_obj_remove_style_all(p);
		lv_obj_add_style(p, & fbtnlockst, 0);
	}
	else
	{
		lv_label_set_text_fmt(lbl, "%s", "RX");
		lv_obj_remove_style_all(p);
		lv_obj_add_style(p, & fbtnst, 0);
	}

}

static btn_t footer_buttons [] = {
		{ "fbtn_trx", 	"RX", 	 	1, btn_txrx_handler, },
		{ "fbtn_modes", "Modes", 	0, NULL, WIN_MODES, },
		{ "fbtn_mem", 	"Memory", 	0, NULL, WIN_MEMORY, },
		{ "fbtn_3", 	"Button 3", 0, NULL, WIN_MEMORY, },
		{ "fbtn_4", 	"Button 4", 0, NULL, WIN_MEMORY, },
		{ "fbtn_5", 	"Button 5", 0, NULL, WIN_MEMORY, },
		{ "fbtn_6", 	"Button 6", 0, NULL, WIN_MEMORY, },
		{ "fbtn_7", 	"Button 7", 0, NULL, WIN_MEMORY, },
		{ "fbtn_8", 	"Button 8", 0, NULL, WIN_MEMORY, },
};

void win_close(void)
{
	if (current_window)
		lv_obj_del(current_window);

	current_window = NULL;
	cid = 255;
}

lv_obj_t * win_open(uint8_t id)
{
	ASSERT(id < WINDOW_COUNT);
	const window_t * win_def = & windows[id];

	if (id != cid)
	{
		win_close();

		current_window = gui_win_create(main_gui_obj);
		lv_obj_add_style(current_window, & winst, 0);
		gui_win_add_title(current_window, win_def->title);
		cid = id;

		win_def->eh(NULL);
	}
	else
		win_close();

	return current_window;
}

static void fbtn_event_handler(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code != LV_EVENT_PRESSED && code != LV_EVENT_RELEASED && code != LV_EVENT_PRESSING &&
			code != LV_EVENT_PRESS_LOST && code != LV_EVENT_CLICKED) return;

	lv_obj_t * btn = (lv_obj_t *) lv_event_get_target(e);
	user_t * ext = lv_obj_get_user_data(btn);
	btn_t * fb = & footer_buttons[ext->payload];

	if (fb->long_press)
	{
		if(code == LV_EVENT_PRESSED)
		{
			ext->is_pressed = true;
			ext->long_press_timer = lv_timer_create(long_press_timer_cb, 1000, ext); // 1 секунда
		}
		else if (code == LV_EVENT_PRESSING && ext->is_long_pressed)
		{
//			LV_LOG_USER("Long press detected");
			if (fb->h)
				fb->h(btn);
			ext->is_long_pressed = 0;
		}
		else if(code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST)
		{
			if(ext->is_pressed)
			{
				lv_timer_del(ext->long_press_timer);
				ext->is_pressed = false;
//				LV_LOG_USER("Short press");

				if (fb->h)
					fb->h(btn);
				else
					win_open(fb->win_id);
			}
		}
	}
	else if (code == LV_EVENT_CLICKED)
	{
		if (fb->h)
			fb->h(btn);
		else
			win_open(fb->win_id);
	}
}

static void init_gui_styles(void)
{
	lv_style_init(& mainst);
	lv_style_set_width(& mainst, DIM_X);
	lv_style_set_height(& mainst, DIM_Y);

	lv_style_init(& fbtnst);
	lv_style_set_bg_color(& fbtnst, lv_palette_main(LV_PALETTE_LIGHT_GREEN));
	lv_style_set_border_width(& fbtnst, 2);
	lv_style_set_border_color(& fbtnst, lv_color_black());
	lv_style_set_radius(& fbtnst, 5);
	lv_style_set_bg_opa(& fbtnst, LV_OPA_COVER);
	lv_style_set_pad_all(& fbtnst, 0);

	lv_style_init(& fbtnlockst);
	lv_style_set_bg_color(& fbtnlockst, lv_palette_main(LV_PALETTE_ORANGE));
	lv_style_set_border_width(& fbtnlockst, 2);
	lv_style_set_border_color(& fbtnlockst, lv_color_black());
	lv_style_set_radius(& fbtnlockst, 5);
	lv_style_set_bg_opa(& fbtnlockst, LV_OPA_COVER);
	lv_style_set_pad_all(& fbtnlockst, 0);

	lv_style_init(& flbl);
	lv_style_set_text_color(& flbl, lv_color_black());
	lv_style_set_align(& flbl, LV_ALIGN_CENTER);

	lv_style_init(& fcontst);
	lv_style_set_bg_color(& fcontst, lv_color_black());
	lv_style_set_border_width(& fcontst, 0);
	lv_style_set_pad_column(& fcontst, 3);
	lv_style_set_pad_all(& fcontst, 0);

	lv_style_init(& winst);
	lv_style_set_align(& winst, LV_ALIGN_CENTER);
	lv_style_set_bg_opa(& winst, LV_OPA_30);
}

static void footer_buttons_init(lv_obj_t * parent)
{
	lv_obj_t * cont = lv_obj_create(parent);
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

	static user_t btns_def[ARRAY_SIZE(footer_buttons)];

	for (int i = 0; i < ARRAY_SIZE(footer_buttons); i++) {
		lv_obj_t * btn = lv_button_create(cont);
		lv_obj_add_style(btn, & fbtnst, 0);

		btn_t fb = footer_buttons[i];
		user_t * btnd = & btns_def[i];

		lv_obj_add_event_cb(btn, fbtn_event_handler, LV_EVENT_ALL, NULL);
		lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, i, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

		btnd->payload = i;
		strncpy(btnd->name, fb.name, 30);
		lv_obj_set_user_data(btn, btnd);

		lv_obj_t * lbl = lv_label_create(btn);
		lv_label_set_text(lbl, fb.text);
		lv_obj_add_style(lbl, & flbl, 0);
	}
}

void lvgl_gui_init(lv_obj_t * parent)
{
	init_gui_styles();

	main_gui_obj = parent;

	footer_buttons_init(main_gui_obj);
}

#endif /* WITHLVGL && LINUX_SUBSYSTEM */
