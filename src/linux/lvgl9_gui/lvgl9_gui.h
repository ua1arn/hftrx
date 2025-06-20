#ifndef LINUX_LVGL_GUI_H
#define LINUX_LVGL_GUI_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIN_MIN_W	200
#define WIN_MIN_H	150

extern lv_style_t mainst, btnst, popupbtnst, btnlockst, fcontst, lblst, winlblst, winst;

typedef void (* event_handler_t)(lv_event_t * e);
typedef void (* phandler_t)(lv_obj_t * p);
typedef void (* handler_t)(void);

typedef enum {
	s130x35 = 130 << 8 | 35,
	s100x44 = 100 << 8 | 44,
	s86x44 = 86 << 8 | 44,
	s40x40 = 40 << 8 | 40
} btns_size_t;

enum {
	fbtn_w = 86,
	fbtn_h = 44,
	memory_cells_count = 20,
	max_updating_objects = 30,
	infobar_count = 8,
	infobar_empty = 0x0F,
	infobar_noaction = 0x10,
	infobar_noaction_pos = 4,
	infobar_need_update = 0x20,
	infobar_need_update_pos = 5,
	infobar_switch = 0x40,
	infobar_switch_pos = 6,
	infobar_valid_mask = 0x0F
};

enum {
	infobar_place_1,
	infobar_place_2,
	infobar_place_3,
	infobar_place_4,
	infobar_place_5,
	infobar_place_6,
	infobar_place_7,
	infobar_place_8
};

enum {
	INFOBAR_DUMMY,
	INFOBAR_AF,
	INFOBAR_ATT,
	INFOBAR_IF_SHIFT,
	INFOBAR_SPAN,
	INFOBAR_VOLTAGE,
	INFOBAR_CPU_TEMP,
	INFOBAR_2ND_ENC_MENU,
	INFOBAR_TX_POWER,
	INFOBAR_AF_VOLUME,
	INFOBAR_DNR,
	INFOBAR_DUAL_RX,
};

typedef struct {
	lv_obj_t * main;
	lv_obj_t * popup;
	uint8_t active_popup_index;			// 0xFF — ничего не открыто
} infobar_t;

typedef struct {
	char text[20];
	int32_t payload;
	bool long_press;
	phandler_t h;
	int win_id;
	// *****
	char name[20];
	bool is_pressed;
	bool is_clicked;
	bool is_long_pressed;
	uint8_t index;
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
lv_obj_t * gui_get_main(void);

void buttons_handler(lv_event_t * e);
lv_obj_t * find_button(lv_obj_t * cont, const char * name);
void button_set_text(lv_obj_t * btn, const char * text);
void button_lock(lv_obj_t * btn);
void button_unlock(lv_obj_t * btn);
void button_set_lock(lv_obj_t * btn, uint8_t v);
void create_button_matrix(lv_obj_t * cont, btn_t * btu, const char * mname, const uint8_t btns, const uint8_t cols, btns_size_t s, event_handler_t eh);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LINUX_LVGL_GUI_H*/
