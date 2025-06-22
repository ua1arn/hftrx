#include "hardware.h"
#include "formats.h"
#include "display2.h"

#if WITHLVGL && LINUX_SUBSYSTEM

#include "lvgl9_gui.h"
#include "windows.h"

static void lv_win_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);

static lv_obj_t * main_gui_obj = NULL, * current_window = NULL, * win_title;
static uint8_t cid = WIN_MAIN;

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

void gui_set_parent(lv_obj_t * p)
{
	main_gui_obj = p;
}

static lv_obj_t * gui_win_create(lv_obj_t * parent)
{
    lv_obj_t * obj = lv_obj_class_create_obj(& gui_win_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

lv_obj_t * gui_win_get_content(void)
{
    return lv_obj_get_child(current_window, 1);
}

void gui_win_add_title(lv_obj_t * win, const char * txt)
{
    lv_label_set_text(win_title, txt);
}

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

lv_obj_t * gui_get_window(void)
{
	return current_window;
}

lv_obj_t * gui_get_main(void)
{
	return main_gui_obj;
}

static void window_resized_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);
    lv_obj_t * win = lv_event_get_user_data(e);
    lv_coord_t cont_h = lv_obj_get_height(cont);
    lv_coord_t header_h = lv_obj_get_height(lv_obj_get_child(win, 0));

    lv_coord_t total_h = header_h + cont_h;

    lv_coord_t cont_w = lv_obj_get_width(cont);
    lv_coord_t win_w = LV_MAX(cont_w, WIN_MIN_W);

    lv_obj_set_size(win, win_w, total_h);
    lv_obj_center(win);
}

static void window_close_event_cb(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code != LV_EVENT_CLICKED) return;

	win_close();
}

static void lv_win_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{

	LV_UNUSED(class_p);

	lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);

	lv_obj_t * header = lv_obj_create(obj);
	lv_obj_set_size(header, LV_PCT(100), 30);
	lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

	win_title = lv_label_create(header);
	lv_label_set_long_mode(win_title, LV_LABEL_LONG_MODE_DOTS);
	lv_obj_set_flex_grow(win_title, 2);

	lv_obj_t * close_btn = lv_button_create(header);
	lv_obj_set_size(close_btn, 30, 30);
	lv_obj_set_style_radius(close_btn, 0, 0);
	lv_obj_set_style_bg_color(close_btn, lv_color_hex(0xff4444), 0);
	lv_obj_align(close_btn, LV_ALIGN_TOP_RIGHT, 0, 0);
	lv_obj_add_event_cb(close_btn, window_close_event_cb, LV_EVENT_CLICKED, NULL);

	lv_obj_t * label = lv_label_create(close_btn);
	lv_label_set_text(label, LV_SYMBOL_CLOSE);
	lv_obj_center(label);

	lv_obj_t * cont = lv_obj_create(obj);
	lv_obj_set_width(cont, LV_PCT(100));
	lv_obj_set_style_min_width(cont, WIN_MIN_W, 0);
	lv_obj_set_style_pad_all(cont, 10, 10);
	lv_obj_set_style_bg_opa(cont, LV_OPA_0, 0);
	lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
	lv_obj_set_style_border_width(cont, 0, 0);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

	lv_obj_center(obj);

	lv_obj_add_event_cb(cont, window_resized_event_cb, LV_EVENT_SIZE_CHANGED, obj);
}

#endif /* WITHLVGL && LINUX_SUBSYSTEM */
