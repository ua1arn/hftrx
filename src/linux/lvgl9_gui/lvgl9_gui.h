#ifndef LINUX_LVGL_GUI_H
#define LINUX_LVGL_GUI_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIN_MIN_W	200
#define WIN_MIN_H	150

typedef void (* event_handler_t)(lv_event_t * e);
typedef void (* phandler_t)(lv_obj_t * p);
typedef void (* handler_t)(void);

enum {
	fbtn_w = 86,
	fbtn_h = 44,
	memory_cells_count = 20,
};

typedef struct {
    lv_obj_t * obj;
    int32_t payload;
    char name[30];
    handler_t h;
    event_handler_t eh;
    lv_timer_t * long_press_timer;
    bool is_pressed;
    bool is_clicked;
    bool is_long_pressed;
} user_t;

typedef struct {
	char name[20];
	char text[20];
	bool long_press;
	phandler_t h;
	int win_id;
} btn_t;

void lvgl_gui_init(lv_obj_t * parent);

lv_obj_t * gui_win_create(lv_obj_t * parent);
lv_obj_t * gui_win_add_title(lv_obj_t * win, const char * txt);
lv_obj_t * gui_win_get_content(lv_obj_t * win);
void update_win_size(lv_obj_t * win);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LINUX_LVGL_GUI_H*/
