#ifndef GUI_USER_H_INCLUDED
#define GUI_USER_H_INCLUDED

#include "gui_port_include.h"

#if WITHTOUCHGUI

#include "gui.h"
#include "framework/gui_settings.h"
#include "gui_user_ext.h"

void gui_user_actions_after_close_window(void);
void footer_buttons_state(uint_fast8_t state, ...);

void gui_main_process(void);
void window_mode_process(void);
void window_af_process(void);
void window_freq_process (void);
void window_swrscan_process(void);
void window_tx_process(void);
void window_tx_power_process(void);
void window_audiosettings_process(void);
void window_ap_reverb_process(void);
void window_ap_mic_eq_process(void);
void window_ap_mic_process(void);
void window_ap_mic_prof_process(void);
void window_menu_process(void);
void window_uif_process(void);
void window_options_process(void);
void window_utilites_process(void);
void window_bands_process(void);
void window_memory_process(void);
void window_display_process(void);
void window_receive_process(void);
void window_notch_process(void);
void window_gui_settings_process(void);
void window_ft8_process(void);
void window_ft8_bands_process(void);
void window_ft8_settings_process(void);
void window_infobar_menu_process(void);
void window_af_eq_process(void);
void window_shift_process(void);
void window_menu_params_process(void);
void window_time_process(void);
void window_kbd_process(void);
void window_kbd_test_process(void);
void window_3d_process(void);
void window_lfm_process(void);
void window_lfm_spectre_process(void);
void window_stream_process(void);
void window_wnbconfig_process(void);
void window_ad936x_process(void);
void window_as_process(void);
void window_fanpwm_process(void);

uint8_t as_get_state(void);
uint8_t as_get_progress(void);
void as_toggle_record(void);
void as_toggle_play(void);
void as_toggle_trx(void);
void as_draw_spectrogram(uint32_t * d, uint16_t len, uint16_t lim);
void as_tx(uint32_t * buf);
void as_rx(uint32_t * buf);
void linux_exit(void);
char * get_alsa_out(void);
void alsa_switch_out(void);
uint8_t linux_fan_pwm_set(int8_t d);
uint8_t iio_ad936x_find(const char * uri);
uint8_t iio_ad936x_start(const char * uri);
uint8_t iio_ad936x_stop(void);
void iio_ad936x_set_gain(uint8_t type, int gain);
void wnb_set_threshold(uint16_t v);
void wnb_get_limits(uint16_t * min, uint16_t * max);
uint16_t wnb_get_threshold(void);
uint8_t wnb_state_switch(uint8_t v);
void load_memory_cells(uint32_t * mem, uint8_t cnt);
void write_memory_cells(uint32_t * mem, uint8_t cnt);

enum {
	AS_IDLE,
	AS_RECORDING,
	AS_PLAYING,
	AS_TX,
	AS_READY,

	AS_COUNT,
};

enum {
	MENU_OFF,
	MENU_GROUPS,
	MENU_PARAMS,
	MENU_VALS,

	MENU_COUNT
};

typedef struct {
	char name[TEXT_ARRAY_SIZE];
	uint16_t menupos;
	uint8_t exitkey;
} menu_by_name_t;

enum {
	TYPE_BP_LOW,
	TYPE_BP_HIGH,
	TYPE_AFR,
};

enum {
	TYPE_NOTCH_FREQ,
	TYPE_NOTCH_WIDTH
};

enum {
	TYPE_DISPLAY_SP_TOP,
	TYPE_DISPLAY_SP_BOTTOM,
	TYPE_DISPLAY_SP_PART,
};

enum {
	GUI_TOPDBMIN = WITHTOPDBMIN + DBVALOFFSET_BASE,
	GUI_TOPDBMAX = WITHTOPDBMAX + DBVALOFFSET_BASE,
	GUI_BOTTOMDBMIN = WITHBOTTOMDBMIN + DBVALOFFSET_BASE,
	GUI_BOTTOMDBMAX = WITHBOTTOMDBMAX + DBVALOFFSET_BASE,
};

typedef struct {
	uint_fast8_t updated;
	uint_fast8_t select;
	int8_t change;
} enc_var_t;

typedef struct {
	uint_fast16_t step;
	char label[10];
} val_step_t;

typedef struct {
	char * str;				// указатель на редактируемую строку
	int * num;				// указатель на редактируемое числовое зачение
	uint8_t clean;			// признак очистки строки перед открытием окна
	uint8_t digits_only;	// только цифровая клавиатура
	uint8_t max_len;		// максимальная длина редактируемой строки
} keyb_t ;

enum {
	enc2step_default = 1,
	freq_swipe_step_default = 2,
	freq_swipe_enable_default = 0,
	micprofile_default = UINT8_MAX,
};

enum {
	infobar_1st_str_y = 125,
	infobar_2nd_str_y = 145,
	infobar_num_places = 8,
	infobar_label_width = 100,
};

#define INFOBAR_EMPTY 		0x3F
#define INFOBAR_NOACTION	0x40
#define INFOBAR_NOACTION_POS	6
#define INFOBAR_VALID_MASK	0x3F

enum {
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
	INFOBAR_BATTERY,
};

#endif /* WITHTOUCHGUI */
#endif /* GUI_USER_H_INCLUDED */
