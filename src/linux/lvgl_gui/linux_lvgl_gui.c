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

#if WITHLVGL && LINUX_SUBSYSTEM

#include "lvgl.h"

static lv_obj_t * main_gui_obj = NULL;
static lv_style_t mainst, fbtnst, fbtnlockst, fcontst, flbl;

enum {
	max_objects = 100,
	fbtn_w = 86,
	fbtn_h = 44,
};

typedef void (*ui_event_cb_t)(lv_event_t *);

typedef struct {
    lv_obj_t * obj;
    char name[30];
    ui_event_cb_t event_cb;
} reg_t;

typedef struct {
	char name[20];
	char text[20];
} btn_t;

static btn_t footer_buttons [] = {
		{ "fbtn_trx", "RX", 	},
		{ "fbtn_1", "Button 1", },
		{ "fbtn_2", "Button 2", },
		{ "fbtn_3", "Button 3", },
		{ "fbtn_4", "Button 4", },
		{ "fbtn_5", "Button 5", },
		{ "fbtn_6", "Button 6", },
		{ "fbtn_7", "Button 7", },
		{ "fbtn_8", "Button 8", },
};

static reg_t object_registry[max_objects];
static int registry_count = 0;

static void register_object(lv_obj_t * obj, const char * name, ui_event_cb_t event_cb) {
	if (registry_count < max_objects) {
		object_registry[registry_count].obj = obj;
		strncpy(object_registry[registry_count].name, name, 20);
		if (event_cb)
			object_registry[registry_count].event_cb = event_cb;
		registry_count++;
	}
}

static lv_obj_t * find_object_by_name(const char * name) {
	for (int i = 0; i < registry_count; i++) {
		if (strcmp(object_registry[i].name, name) == 0)
			return object_registry[i].obj;
	}
	return NULL;
}

static const char * find_object_n(lv_obj_t * obj) {
	for (int i = 0; i < registry_count; i++) {
		if (obj == object_registry[i].obj)
			return object_registry[i].name;
	}
	return NULL;
}

static const lv_obj_t * find_object(lv_obj_t * obj) {
	for (int i = 0; i < registry_count; i++) {
		if (obj == object_registry[i].obj)
			return object_registry[i].obj;
	}
	return NULL;
}

static void fbtn_event_handler(lv_event_t * e)
{
	lv_obj_t * obj = (lv_obj_t *) lv_event_get_target(e);
	const char * btnname = find_object_n(obj);
	printf("Button %s clicked\n", btnname);

	if (! strcmp(btnname, "fbtn_trx"))
	{
		hamradio_moxmode(1);

		lv_obj_t * lbl = lv_obj_get_child(obj, 0);

		if (hamradio_moxmode(0))
		{
			lv_label_set_text_fmt(lbl, "%s", "TX");
			lv_obj_remove_style_all(obj);
			lv_obj_add_style(obj, & fbtnlockst, 0);
		}
		else
		{
			lv_label_set_text_fmt(lbl, "%s", "RX");
			lv_obj_remove_style_all(obj);
			lv_obj_add_style(obj, & fbtnst, 0);
		}
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

	for (int i = 0; i < ARRAY_SIZE(footer_buttons); i++) {
		lv_obj_t * btn = lv_button_create(cont);
		lv_obj_add_style(btn, & fbtnst, 0);

		btn_t fb = footer_buttons[i];
		register_object(btn, fb.name, fbtn_event_handler);

		lv_obj_add_event_cb(btn, fbtn_event_handler, LV_EVENT_CLICKED, NULL);
		lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, i, 1,
				LV_GRID_ALIGN_STRETCH, 0, 1);

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
