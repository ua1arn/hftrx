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
	char param[20];
	char val[20];
	uint8_t state;
	uint8_t updated;
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

void hamradio_ft8_toggle_state(void);
void hamradio_ft8_start_fill(void);

void gui_gnssupdate(void);
uint_fast8_t wnb_state_switch(uint_fast8_t v);
void wnb_set_threshold(uint16_t v);
uint16_t wnb_get_threshold(void);

void gui_encoder2_menu(enc2_menu_t * enc2_menu);
void gui_initialize(void);
void gui_set_encoder2_rotate(int_least16_t rotate);
void gui_put_keyb_code(uint8_t kbch);
void gui_uif_editmenu(const char * name, uint16_t menupos, uint8_t exitkey);
void gui_open_sys_menu(void);
void gui_update(void);
void gui_add_debug(char d);
void gui_open_debug_window(void);

#endif /* WITHTOUCHGUI */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GUI_H_INCLUDED */
