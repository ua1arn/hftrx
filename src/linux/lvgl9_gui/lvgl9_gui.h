#ifndef LINUX_LVGL_GUI_H
#define LINUX_LVGL_GUI_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIN_MIN_W	200
#define WIN_MIN_H	150

extern lv_style_t mainst, fbtnst, fbtnlockst, fcontst, flbl, winst;

typedef void (* event_handler_t)(lv_event_t * e);
typedef void (* phandler_t)(lv_obj_t * p);
typedef void (* handler_t)(void);

enum {
	fbtn_w = 86,
	fbtn_h = 44,
	memory_cells_count = 20,
};

typedef struct {
	char name[20];
	char text[20];
	bool long_press;
	phandler_t h;
	int win_id;
	// *****
	int32_t payload;
	bool is_pressed;
	bool is_clicked;
	bool is_long_pressed;
	lv_timer_t * long_press_timer;
	event_handler_t eh;
} btn_t;

void lvgl_gui_init(lv_obj_t * parent);

lv_obj_t * gui_win_add_title(lv_obj_t * win, const char * txt);
lv_obj_t * gui_win_get_content(void);
void update_win_size(lv_obj_t * win);
void win_close(void);
lv_obj_t * win_open(uint8_t id);
void init_gui_styles(void);
void gui_set_parent(lv_obj_t * p);
lv_obj_t * gui_get_window(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LINUX_LVGL_GUI_H*/
