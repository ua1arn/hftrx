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

void update_win_size(lv_obj_t * win)
{
	lv_obj_t * cont = lv_obj_get_child(win, 1);
	lv_obj_update_layout(cont);
	lv_coord_t new_h = 30 + lv_obj_get_height(cont);
	lv_obj_set_height(win, LV_MAX(100, new_h));
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
    lv_obj_t * win = lv_event_get_target(e);

    lv_obj_t * header = gui_win_get_header(win);
    lv_obj_set_width(header, lv_obj_get_width(win));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_win_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_min_width(obj, 200, 0);
    lv_obj_set_style_min_height(obj, 100, 0);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * header = lv_obj_create(obj);
    lv_obj_set_size(header, LV_SIZE_CONTENT, 30);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(obj, window_resized_event_cb, LV_EVENT_SIZE_CHANGED, NULL);
}

#endif /* WITHLVGL && LINUX_SUBSYSTEM */
