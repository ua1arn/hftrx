#ifndef LINUX_LVGL_GUI_H
#define LINUX_LVGL_GUI_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void lvgl_gui_init(lv_obj_t * parent);

lv_obj_t * gui_win_create(lv_obj_t * parent);
lv_obj_t * gui_win_add_title(lv_obj_t * win, const char * txt);
lv_obj_t * gui_win_get_content(lv_obj_t * win);
void update_win_size(lv_obj_t * win);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LINUX_LVGL_GUI_H*/
