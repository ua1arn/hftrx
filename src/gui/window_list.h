#ifndef X
	#error PLEASE, DO NOT USE THIS FILE DIRECTLY
#endif

#define WINDOW_LIST(X) 																									\
    X(MAIN,              DUMMY,    	    ALIGN_LEFT_X,   "",                          0, gui_main_process) 				\
    X(MODES,             RECEIVE,       ALIGN_CENTER_X, "Select mode",               1, window_mode_process) 			\
    X(AF,                RECEIVE,       ALIGN_CENTER_X, "AF settings",               1, window_af_process) 				\
    X(FREQ,              BANDS,         ALIGN_CENTER_X, "Freq:",                     1, window_freq_process) 			\
    X(MENU,              OPTIONS,       ALIGN_CENTER_X, "Settings",                  1, window_menu_process) 			\
    X(MENU_PARAMS,       MENU,          ALIGN_CENTER_X, "Settings",                  1, window_menu_params_process) 	\
    X(UIF,               DUMMY,         ALIGN_LEFT_X,   "",                          0, window_uif_process) 			\
    X(AUDIOSETTINGS,     OPTIONS,       ALIGN_CENTER_X, "Audio settings",            1, window_audiosettings_process) 	\
    X(AP_MIC_SETT,       AUDIOSETTINGS, ALIGN_CENTER_X, "Microphone settings",       1, window_ap_mic_process) 			\
    X(AP_MIC_PROF,       AUDIOSETTINGS, ALIGN_CENTER_X, "Microphone profiles",       1, window_ap_mic_prof_process) 	\
    X(TX_SETTINGS,       OPTIONS,       ALIGN_CENTER_X, "Transmit settings",         1, window_tx_process) 				\
    X(TX_VOX_SETT,       TX_SETTINGS,   ALIGN_CENTER_X, "VOX settings",              1, window_tx_vox_process) 			\
    X(TX_POWER,          TX_SETTINGS,   ALIGN_CENTER_X, "TX power",                  1, window_tx_power_process) 		\
    X(OPTIONS,           DUMMY,         ALIGN_CENTER_X, "Options",                   1, window_options_process) 		\
    X(UTILS,             OPTIONS,       ALIGN_CENTER_X, "Utilites",                  1, window_utilites_process) 		\
    X(BANDS,             DUMMY,         ALIGN_CENTER_X, "Bands",                     1, window_bands_process) 			\
    X(MEMORY,            DUMMY,         ALIGN_CENTER_X, "Memory",                    1, window_memory_process) 			\
    X(DISPLAY,           OPTIONS,       ALIGN_CENTER_X, "Display settings",          1, window_display_process) 		\
    X(RECEIVE,           DUMMY,         ALIGN_CENTER_X, "Receive settings",          1, window_receive_process) 		\
    X(NOTCH,             DUMMY,         ALIGN_CENTER_X, "Notch",                     1, window_notch_process) 			\
    X(GUI_SETTINGS,      OPTIONS,       ALIGN_CENTER_X, "GUI settings",              1, window_gui_settings_process) 	\
    X(INFOBAR_MENU,      DUMMY,         ALIGN_MANUAL,   "",                          0, window_infobar_menu_process) 	\
    X(AF_EQ,             AUDIOSETTINGS, ALIGN_CENTER_X, "AF equalizer",              1, window_af_eq_process) 			\
    X(TIME,              OPTIONS,       ALIGN_CENTER_X, "Date & time set",           1, window_time_process) 			\
    X(KBD,               DUMMY,         ALIGN_CENTER_X, "Keyboard",                  0, window_kbd_process) 			\
    X(KBD_TEST,          UTILS,         ALIGN_CENTER_X, "Keyboard demo",             1, window_kbd_test_process) 		\
    X(3D,                UTILS,         ALIGN_CENTER_X, "Donut 3d ASCII demo",       1, window_3d_process) 				\
	X(SWR_SCANNER,       UTILS,         ALIGN_CENTER_X, "SWR band scanner",          0, window_swrscan_process) 		\
	X(AP_REVERB_SETT,    AUDIOSETTINGS, ALIGN_CENTER_X, "Reverberator settings",     1, window_ap_reverb_process) 		\
    X(FT8,               DUMMY,         ALIGN_CENTER_X, "FT8 terminal",              1, window_ft8_process) 			\
    X(FT8_BANDS,         FT8,           ALIGN_CENTER_X, "FT8 bands",                 0, window_ft8_bands_process) 		\
    X(FT8_SETTINGS,      FT8,           ALIGN_CENTER_X, "FT8 settings",              0, window_ft8_settings_process) 	\
	X(SHIFT,             UTILS,         ALIGN_CENTER_X, "IQ shift",                  1, window_shift_process) 			\
    X(LFM,               UTILS,         ALIGN_RIGHT_X,  "LFM",                       1, window_lfm_process) 			\
    X(LFM_SPECTRE,       LFM,           ALIGN_CENTER_X, "LFM spectre draw",          1, window_lfm_spectre_process)		\
	X(EXTIOLAN,          UTILS,         ALIGN_CENTER_X, "LAN IQ Stream server",      1, window_stream_process) 			\
	X(WNBCONFIG,         RECEIVE,       ALIGN_CENTER_X, "WNB config",                1, window_wnbconfig_process) 		\
	X(AD936X,            OPTIONS,       ALIGN_CENTER_X, "AD936x config",             1, window_ad936x_process) 			\
	X(AS,                DUMMY,         ALIGN_CENTER_X, "AF samples",                1, window_as_process)
