/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN


#include <string.h>
#include <math.h>

#include "hardware.h"
#include "keyboard.h"
#include "formats.h"

#include "gui.h"
#include "gui_system.h"
#include "gui_structs.h"
#include "gui_settings.h"
#include "gui_windows.h"
#include "window_list.h"

#if WITHTOUCHGUI

void gui_main_process(void);
void window_mode_process(void);
void window_af_process(void);
void window_freq_process (void);
void window_swrscan_process(void);
void window_tx_process(void);
void window_tx_vox_process(void);
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

static window_t windows[] = {
	#define X(window_id, parent_id, align_mode, title, is_close, onVisibleProcess) \
		{ WINDOW_##window_id, WINDOW_##parent_id, align_mode, title, is_close, onVisibleProcess, },
	WINDOW_LIST(X)
	#undef X
	#undef WINDOW_LIST
};

/* Возврат ссылки на окно */
window_t * get_win(uint8_t window_id)
{
	if (window_id == NO_PARENT_WINDOW)	//	костыль
		return & windows [0];

	ASSERT(window_id < WINDOWS_COUNT);
	return & windows [window_id];
}

#endif /* WITHTOUCHGUI */
