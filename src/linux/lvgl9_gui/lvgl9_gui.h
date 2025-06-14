#ifndef LINUX_LVGL_GUI_H
#define LINUX_LVGL_GUI_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIN_MIN_W	200
#define WIN_MIN_H	150

typedef void (* handler_t)(lv_event_t * e);

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

void lvgl_gui_init(lv_obj_t * parent);

lv_obj_t * gui_win_create(lv_obj_t * parent);
lv_obj_t * gui_win_add_title(lv_obj_t * win, const char * txt);
lv_obj_t * gui_win_get_content(lv_obj_t * win);
void update_win_size(lv_obj_t * win);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LINUX_LVGL_GUI_H*/
