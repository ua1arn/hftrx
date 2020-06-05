#ifndef GUI_USER_H_INCLUDED
#define GUI_USER_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI
#include "src/gui/gui_structs.h"

void gui_main_process(void);
void window_mode_process(void);
void window_bp_process(void);
void window_agc_process(void);
void window_freq_process (void);
void window_swrscan_process(void);
void window_tx_process(void);
void window_tx_vox_process(void);
void buttons_tx_power_process(void);
void window_tx_power_process(void);
void buttons_audiosettings_process(void);
void window_audiosettings_process(void);


#endif /* WITHTOUCHGUI */
#endif /* GUI_USER_H_INCLUDED */
