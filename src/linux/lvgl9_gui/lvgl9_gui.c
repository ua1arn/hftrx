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

void win_close(void);

void win_modes_handler(lv_event_t * e)
{
	if (! e) // init window
	{
		static const char * btnm_map[] = {
				"LSB", "CW",  "AM",  "DGL", "\n",
				"USB", "CWR", "NFM", "DGU", "\n", "", };

		lv_obj_t * btnm1 = lv_buttonmatrix_create(current_window);
		lv_obj_set_style_bg_opa(btnm1, LV_OPA_0, 0);
		lv_obj_add_style(btnm1, & fbtnst, 0);
		lv_buttonmatrix_set_map(btnm1, btnm_map);
		lv_obj_align(btnm1, LV_ALIGN_CENTER, 0, 0);
		lv_obj_add_event_cb(btnm1, win_modes_handler, LV_EVENT_CLICKED, NULL);

		return;
	}

	const int btnm_data [] = {
			SUBMODE_LSB, SUBMODE_CW, SUBMODE_AM, SUBMODE_DGL,
			SUBMODE_USB, SUBMODE_CWR, SUBMODE_NFM, SUBMODE_DGU,
	};

	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target_obj(e);
	if(code == LV_EVENT_CLICKED)
	{
		uint32_t id = lv_buttonmatrix_get_selected_button(obj);
		if (id == LV_BUTTONMATRIX_BUTTON_NONE) return;
		const char * txt = lv_buttonmatrix_get_button_text(obj, id);
		hamradio_change_submode(btnm_data[id], 1);
		win_close();
	}
}

void win_test_handler(lv_event_t * e)
{

}



enum {
	max_objects = 100,
	fbtn_w = 86,
	fbtn_h = 44,
};

typedef struct {
    lv_obj_t * obj;
    uint32_t id;
    char name[30];
    handler_t handler;
} user_t;

typedef struct {
	char name[20];
	char text[20];
} btn_t;

static btn_t footer_buttons [] = {
		{ "fbtn_trx", "RX", 	},
		{ "fbtn_modes", "Modes", },
		{ "fbtn_2", "Button 2", },
		{ "fbtn_3", "Button 3", },
		{ "fbtn_4", "Button 4", },
		{ "fbtn_5", "Button 5", },
		{ "fbtn_6", "Button 6", },
		{ "fbtn_7", "Button 7", },
		{ "fbtn_8", "Button 8", },
};

void win_close(void)
{
	if (current_window)
		lv_obj_del(current_window);

	current_window = NULL;
}

lv_obj_t * win_open(uint8_t id)
{
	win_close();

	ASSERT(id < WINDOW_COUNT);
	const window_t * win_def = & windows[id];

	current_window = gui_win_create(main_gui_obj);
	lv_obj_add_style(current_window, & winst, 0);
	gui_win_add_title(current_window, win_def->title);

	win_def->h(NULL);

	return current_window;
}

static void fbtn_event_handler(lv_event_t * e)
{
	lv_obj_t * btn = (lv_obj_t *) lv_event_get_target(e);
	user_t * btnd = lv_obj_get_user_data(btn);
	char * btnname = btnd->name;

	printf("Button %s clicked\n", btnname);

	if (! strcmp(btnname, "fbtn_trx"))
	{
		hamradio_moxmode(1);

		lv_obj_t * lbl = lv_obj_get_child(btn, 0);

		if (hamradio_moxmode(0))
		{
			lv_label_set_text_fmt(lbl, "%s", "TX");
			lv_obj_remove_style_all(btn);
			lv_obj_add_style(btn, & fbtnlockst, 0);
		}
		else
		{
			lv_label_set_text_fmt(lbl, "%s", "RX");
			lv_obj_remove_style_all(btn);
			lv_obj_add_style(btn, & fbtnst, 0);
		}
	}
	else if (! strcmp(btnname, "fbtn_modes"))
	{
		win_open(WIN_MODES);

	}
	else if (! strcmp(btnname, "fbtn_2"))
	{
		win_open(WIN_TEST);
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
	lv_style_set_min_width(& winst, 150);
	lv_style_set_min_height(& winst, 100);
//	lv_style_set_width(& winst, LV_SIZE_CONTENT);
//	lv_style_set_width(& winst, LV_SIZE_CONTENT);
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

	static int32_t row_dsc[] = { fbtn_h, fbtn_h, fbtn_h, fbtn_h, fbtn_h, fbtn_h,
			fbtn_h, fbtn_h, fbtn_h, LV_GRID_TEMPLATE_LAST };
	lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

	static user_t btns_def[ARRAY_SIZE(footer_buttons)];

	for (int i = 0; i < ARRAY_SIZE(footer_buttons); i++) {
		lv_obj_t * btn = lv_button_create(cont);
		lv_obj_add_style(btn, & fbtnst, 0);

		btn_t fb = footer_buttons[i];
		user_t * btnd = & btns_def[i];

		lv_obj_add_event_cb(btn, fbtn_event_handler, LV_EVENT_CLICKED, NULL);
		lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, i, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

		btnd->id = i;
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
