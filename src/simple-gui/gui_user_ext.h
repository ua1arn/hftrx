#ifndef GUI_USER_EXT_H_INCLUDED
#define GUI_USER_EXT_H_INCLUDED

#include "gui_port_include.h"

#if WITHTOUCHGUI

#include "framework/gui_settings.h"

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

typedef struct {
	uint8_t first_id;			// первое вхождение номера метки уровня
	uint8_t last_id;			// последнее вхождение номера метки уровня
	uint8_t num_rows;			// число меток уровня
	uint8_t count;				// число значений уровня
	int8_t selected_str;		// выбранная строка уровня
	int8_t selected_label;		// выбранная метка уровня
	uint8_t add_id;				// номер строки уровня, отображаемой первой
	menu_names_t menu_block[MENU_ARRAY_SIZE];	// массив значений уровня меню
} menu_t;

void gui_open_sys_menu(void);
void gui_uif_editmenu(const char * name, uint16_t menupos, uint8_t exitkey);
void gui_encoder2_menu(enc2_menu_t * enc2_menu);
void gui_add_debug(char d);
void gui_open_debug_window(void);

#endif /* WITHTOUCHGUI */
#endif /* GUI_USER_EXT_H_INCLUDED */
