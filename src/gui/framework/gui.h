#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include "../gui_port_include.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if WITHTOUCHGUI

#define WITHGUIMAXX				DIM_X
#define WITHGUIMAXY				DIM_Y
#define GUI_TRANSPARENT_WINDOWS	1
#define FOOTER_HEIGHT			(WITHGUIMAXX / 16)

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

enum {
	memory_cells_count = 20,
	callsign_max_lenght = 15,
	qth_max_lenght = 10,
	end_max_lenght = 6,
};

/* структура для размещения в конфигурационном ОЗУ */
struct gui_nvram_t {
	uint8_t enc2step_pos;
	uint8_t micprofile;
	uint8_t tune_powerdown_enable;
	uint8_t tune_powerdown_value;
	uint8_t freq_swipe_enable;
	uint8_t freq_swipe_step;
#if WITHFT8
	char ft8_callsign[callsign_max_lenght];
	char ft8_qth[qth_max_lenght];
	char ft8_snr[end_max_lenght];
	char ft8_end[end_max_lenght];
	uint8_t ft8_band;
	uint32_t ft8_txfreq_val;
	uint8_t ft8_txfreq_equal;
#endif /* WITHFT8 */
};

uint_fast16_t hamradio_get_multilinemenu_block_groups(menu_names_t * vals);
uint_fast16_t hamradio_get_multilinemenu_block_params(menu_names_t * vals, uint_fast8_t index, uint_fast8_t max_count);
void hamradio_get_multilinemenu_block_vals(menu_names_t * vals, uint_fast8_t index, uint_fast8_t cnt);
const char * hamradio_gui_edit_menu_item(uint_fast16_t index, int_least16_t rotate);
void hamradio_clean_memory_cells(uint_fast8_t i);
void hamradio_save_memory_cells(uint_fast8_t i);
uint_fast32_t hamradio_load_memory_cells(uint_fast8_t cell, uint_fast8_t set);
uint_fast8_t hamradio_get_submode(void);
const char * hamradio_get_submode_label(uint_fast8_t v);
uint_fast8_t hamradio_load_mic_profile(uint_fast8_t cell, uint_fast8_t set);
void hamradio_save_mic_profile(uint_fast8_t cell);
void hamradio_clean_mic_profile(uint_fast8_t cell);
uint_fast8_t hamradio_get_bands(band_array_t * bands, uint_fast8_t count_only, uint_fast8_t is_bcast_need);
void hamradio_goto_band_by_freq(uint_fast32_t f);
uint_fast8_t hamradio_check_current_freq_by_band(uint_fast8_t band);
void hamradio_load_gui_settings(void * ptr);
void hamradio_save_gui_settings(const void * ptr);
void hamradio_gui_enc2_update(void);
void hamradio_ft8_toggle_state(void);
void hamradio_ft8_start_fill(void);
uint_fast8_t hamradio_get_att_dbs(uint_fast8_t * values, uint_fast8_t limit);
uint_fast8_t hamradio_get_att_db(void);
void hamradio_set_att_db(uint_fast8_t db);
void hamradio_set_bw(uint_fast8_t v);
uint_fast8_t hamradio_change_nr(uint_fast8_t v);
uint_fast8_t hamradio_split_toggle(void);
void hamradio_split_vfo_swap(void);
void hamradio_split_mode_toggle(void);
uint_fast8_t hamradio_get_gvfoab(void);
void display2_set_page_temp(uint_fast8_t page);
uint_fast8_t display_getpage0(void);
uint_fast8_t display_getpagegui(void);
uint_fast8_t hamradio_get_lfmmode(void);
void hamradio_set_lfmmode(uint_fast8_t v);
uint_fast16_t hamradio_get_lfmstop100k(void);
void hamradio_set_lfmstop100k(uint_fast16_t v);
uint_fast16_t hamradio_get_lfmtoffset(void);
void hamradio_set_lfmtoffset(uint_fast16_t v);
uint_fast16_t hamradio_get_lfmtinterval(void);
void hamradio_set_lfmtinterval(uint_fast16_t v);
void hamradio_lfm_disable(void);
void hamradio_get_nmea_time(char * p, size_t sz);
void gui_gnssupdate(void);
uint8_t wnb_state_switch(uint8_t v);
void wnb_set_threshold(uint16_t v);
uint16_t wnb_get_threshold(void);

void gui_encoder2_menu(enc2_menu_t * enc2_menu);
void gui_initialize(void);
void gui_set_encoder2_rotate(int_least16_t rotate);
void gui_put_keyb_code(uint_fast8_t kbch);
void gui_uif_editmenu(const char * name, uint_fast16_t menupos, uint_fast8_t exitkey);
void gui_open_sys_menu(void);
void gui_update(void);
void gui_add_debug(char d);
void gui_open_debug_window(void);

#endif /* WITHTOUCHGUI */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GUI_H_INCLUDED */
