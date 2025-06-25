#ifndef WINDOWS_H
#define WINDOWS_H

#include "hardware.h"

#if WITHLVGL && LINUX_SUBSYSTEM

#include "lvgl9_gui.h"

#ifdef __cplusplus
extern "C" {
#endif

void win_modes_handler(lv_event_t * e);
void win_memory_handler(lv_event_t * e);
void win_receive_handler(lv_event_t * e);
void win_wnb_handler(lv_event_t * e);
void win_af_filter_handler(lv_event_t * e);
void win_tx_power_handler(lv_event_t * e);
void win_settings_handler(lv_event_t * e);
void win_display_settings_handler(lv_event_t * e);
void win_bands_handler(lv_event_t * e);
void win_freq_handler(lv_event_t * e);

#define WINDOW_LIST(X) \
		X(MAIN,    MAIN, "", "", NULL)      	\
		X(MODES,   MAIN, "win_modes", 	"Modes",   win_modes_handler)      	\
		X(MEMORY,  MAIN, "win_memory",  "Memory",  win_memory_handler)		\
		X(RECEIVE, MAIN, "win_receive", "Receive", win_receive_handler)		\
		X(WNB, RECEIVE,  "win_wnb",		"WNB config", win_wnb_handler)		\
		X(AF,  RECEIVE,  "win_af",		"AF filter",  win_af_filter_handler)	\
		X(TX_POWER, MAIN,  "win_tx_power",	"TX power",  win_tx_power_handler)	\
		X(SETTINGS, MAIN,  "win_settings",	"Settings",  win_settings_handler)	\
		X(DISPLAY_SETTINGS, SETTINGS,  "win_display_settings",	"Display settings",  win_display_settings_handler)	\
		X(BANDS, MAIN,  "win_bands",	"Bands",  win_bands_handler)	\
		X(FREQ, MAIN,  "win_freq",	"Enter frequency",  win_freq_handler)	\

typedef enum {
    #define X(name, parent, cname, title, eh) WIN_##name,
    WINDOW_LIST(X)
    #undef X
    WINDOW_COUNT
} window_index_t;

typedef struct {
    const char name[30];
    const char title[30];
    uint8_t parent;
    event_handler_t eh;
} window_t;

static const window_t windows[] = {
	#define X(name, parent, cname, title, eh) { cname, title, WIN_##parent, eh },
	WINDOW_LIST(X)
	#undef X
};

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* WITHLVGL && LINUX_SUBSYSTEM */

#endif /*WINDOWS_H*/
