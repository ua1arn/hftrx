#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include "hardware.h"
#include "../display/display.h"
#include "display2.h"

#if DIM_X == 480 && DIM_Y == 272
	#define WITHGUIMAXX				480						// для разрешения 480х272 используется мини-версия touch GUI
	#define WITHGUIMAXY				272
	#define WITHGUISTYLE_MINI		1
	#define GUI_OLDBUTTONSTYLE		1						// Кнопки без закругления углов
#elif DIM_X >= 800 && DIM_Y >= 480							// при разрешении больше чем 800х480 интерфейс будет сжат до 800х480.
	#define WITHGUIMAXX				800
	#define WITHGUIMAXY				480
	#define WITHGUISTYLE_COMMON		1
	#define GUI_TRANSPARENT_WINDOWS	1						// Прозрачный фон окон
#endif


#if (DIM_X < 480 || DIM_Y < 272) && WITHTOUCHGUI			// не соблюдены минимальные требования к разрешению экрана
	#undef WITHTOUCHGUI										// для функционирования touch GUI
#endif

#if WITHTOUCHGUI

#if ! defined WITHUSEMALLOC									// необходима поддержка динамического управления памятью
	#define WITHUSEMALLOC		1
#endif /* ! defined WITHUSEMALLOC */

#if ! defined WITHGUIHEAP || WITHGUIHEAP < (80 * 1024uL)	// требуемый размер кучи для touch GUI
	#undef WITHGUIHEAP
	#define WITHGUIHEAP 		(80 * 1024uL)
#endif /* ! defined WITHGUIHEAP || WITHGUIHEAP < (80 * 1024uL) */

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
	memory_cells_count = 20
};

typedef enum {
	BAND_TYPE_HAM,
	BAND_TYPE_BROADCAST
} gui_band_type_t;

typedef struct {
	uint_fast32_t init_freq;
	uint_fast8_t index;
	gui_band_type_t type;
	char name[10];
} band_array_t;

uint_fast8_t hamradio_get_multilinemenu_block_groups(menu_names_t * vals);
uint_fast8_t hamradio_get_multilinemenu_block_params(menu_names_t * vals, uint_fast8_t index);
void hamradio_get_multilinemenu_block_vals(menu_names_t * vals, uint_fast8_t index, uint_fast8_t cnt);
const char * hamradio_gui_edit_menu_item(uint_fast8_t index, int_least16_t rotate);
void hamradio_clean_memory_cells(uint_fast8_t i);
void hamradio_save_memory_cells(uint_fast8_t i);
uint_fast32_t hamradio_load_memory_cells(uint_fast8_t cell, uint_fast8_t set);
uint_fast8_t hamradio_get_submode(void);
const char * hamradio_get_submode_label(uint_fast8_t v);
uint_fast8_t hamradio_load_mic_profile(uint_fast8_t cell, uint_fast8_t set);
void hamradio_save_mic_profile(uint_fast8_t cell);
void hamradio_clean_mic_profile(uint_fast8_t cell);
uint_fast8_t hamradio_get_bands(band_array_t * bands);
void hamradio_goto_band_by_freq(uint_fast32_t f);
uint_fast8_t hamradio_check_current_freq_by_band(uint_fast8_t band);

void gui_encoder2_menu(enc2_menu_t * enc2_menu);
void gui_WM_walktrough(uint_fast8_t x, uint_fast8_t y, dctx_t * pctx);
void gui_initialize(void);
void gui_check_encoder2(int_least16_t rotate);
void gui_set_encoder2_state(uint_fast8_t code);
void gui_put_keyb_code(uint_fast8_t kbch);
void gui_uif_editmenu(const char * name, uint_fast16_t menupos, uint_fast8_t exitkey);
void gui_open_sys_menu(void);
void gui_update (void * arg);

#endif /* WITHTOUCHGUI */
#endif /* GUI_H_INCLUDED */
