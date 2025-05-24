#ifndef STYLES_H_INCLUDED
#define STYLES_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

extern lv_style_t style_mainscreen;
extern lv_style_t style_footer_button;
extern lv_style_t style_freq_main;
extern lv_style_t style_label_btn;
extern lv_style_t style_window;

void styles_init(void);
void lvgl_test(void);
void lvgl_dev_init(void);
void lvgl_init(void);
void wfl_init(void);
PACKEDCOLORPIP_T * wfl_proccess(void);

#ifdef __cplusplus
}
#endif

#endif /* STYLES_H_INCLUDED */
