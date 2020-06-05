#ifndef GUI_USER_H_INCLUDED
#define GUI_USER_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI
#include "src/gui/gui_structs.h"

void buttons_mode_handler(void);
void window_mode_process(void);
void buttons_bp_handler(void);
void window_bp_process(void);
void window_agc_process(void);
void buttons_freq_handler (void);
void window_freq_process (void);

#endif /* WITHTOUCHGUI */
#endif /* GUI_USER_H_INCLUDED */
