#include "hardware.h"
#include "formats.h"
#include "display2.h"

#if WITHLVGL && LINUX_SUBSYSTEM

#include "lvgl9_gui.h"

static void lv_win_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);

struct _lv_win_t {
    lv_obj_t obj;
};

const lv_obj_class_t gui_win_class = {
    .constructor_cb = lv_win_constructor,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .base_class = &lv_obj_class,
    .instance_size = sizeof(lv_win_t),
    .name = "lv_win",
};

lv_obj_t * gui_win_create(lv_obj_t * parent)
{
    lv_obj_t * obj = lv_obj_class_create_obj(& gui_win_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

lv_obj_t * gui_win_get_header(lv_obj_t * win)
{
    return lv_obj_get_child(win, 0);
}

lv_obj_t * gui_win_get_content(lv_obj_t * win)
{
    return lv_obj_get_child(win, 1);
}

lv_obj_t * gui_win_add_title(lv_obj_t * win, const char * txt)
{
    lv_obj_t * header = gui_win_get_header(win);
    lv_obj_t * title = lv_label_create(header);
    lv_label_set_long_mode(title, LV_LABEL_LONG_MODE_DOTS);
    lv_label_set_text(title, txt);
    lv_obj_set_flex_grow(title, 2);
    return title;
}

static void window_resized_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);
    lv_obj_t * win = lv_event_get_user_data(e);
    lv_coord_t cont_h = lv_obj_get_height(cont);
    lv_coord_t header_h = lv_obj_get_height(lv_win_get_header(win));

    lv_coord_t total_h = header_h + cont_h;

    lv_coord_t cont_w = lv_obj_get_width(cont);
    lv_coord_t win_w = LV_MAX(cont_w, WIN_MIN_W);

    lv_obj_set_size(win, win_w, total_h);
    lv_obj_center(win);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_win_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{

	LV_UNUSED(class_p);

	lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);

	lv_obj_t * header = lv_obj_create(obj);
	lv_obj_set_size(header, LV_PCT(100), 30);
	lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	lv_obj_t * cont = lv_obj_create(obj);
	lv_obj_set_width(cont, LV_PCT(100));
	lv_obj_set_flex_grow(cont, 1);
	lv_obj_set_style_min_width(cont, WIN_MIN_W, 0);
	lv_obj_set_style_pad_all(cont, 10, 10);
	lv_obj_set_style_bg_opa(cont, LV_OPA_0, 0);
	lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
	lv_obj_set_style_border_width(cont, 0, 0);

	lv_obj_set_style_min_width(obj, WIN_MIN_W, 0);
	lv_obj_set_style_min_height(obj, WIN_MIN_H, 0);

	lv_obj_center(obj);

	lv_obj_add_event_cb(cont, window_resized_event_cb, LV_EVENT_SIZE_CHANGED, obj);
}

#endif /* WITHLVGL && LINUX_SUBSYSTEM */
