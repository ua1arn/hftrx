#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include "hardware.h"
#include "../display/display.h"

typedef struct {
	char name[20];
	uint_fast8_t index;
} menu_names_t;

typedef struct {
	char param[20];
	char val[20];
	uint_fast8_t state;
	uint_fast8_t updated;
} enc2_menu_t;

uint_fast8_t hamradio_get_multilinemenu_block_groups(menu_names_t * vals);
uint_fast8_t hamradio_get_multilinemenu_block_params(menu_names_t * vals, uint_fast8_t index);
void hamradio_get_multilinemenu_block_vals(menu_names_t * vals, uint_fast8_t index, uint_fast8_t cnt);
const char * hamradio_gui_edit_menu_item(uint_fast8_t index, int_least16_t rotate);
void gui_encoder2_menu(enc2_menu_t * enc2_menu);
void gui_WM_walktrough(uint_fast8_t x, uint_fast8_t y, dctx_t * pctx);
void gui_initialize(void);
uint_fast8_t gui_check_encoder2(int_least16_t rotate);
void gui_set_encoder2_state(uint_fast8_t code);
void gui_put_keyb_code(uint_fast8_t kbch);
void gui_uif_editmenu(const char * name, uint_fast16_t menupos, uint_fast8_t exitkey);
void gui_open_sys_menu(void);
void gui_timer_update (void * arg);

#endif /* GUI_H_INCLUDED */
