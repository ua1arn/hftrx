/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "hardware.h"
#include "board.h"
#include "audio.h"
#include "dspdefines.h"

#include "src/display/display.h"
#include "src/display/fontmaps.h"
#include "formats.h"

#include <string.h>
#include <math.h>

#include "keyboard.h"
#include "codecs.h"

#include "gui.h"
#include "xc7z_inc.h"

#if WITHTOUCHGUI

#include "gui_user.h"
#include "gui_system.h"
#include "gui_structs.h"
#include "gui_settings.h"

val_step_t enc2step [] = {
	{ 100, "100 Hz", },
	{ 500, "500 Hz", },
};

val_step_t freq_swipe_step [] = {
	{ 10, "10 Hz", },
	{ 50, "50 Hz", },
	{ 100, "100 Hz", },
	{ 500, "500 Hz", },
};

static uint_fast16_t year;
static uint_fast8_t month, day, hour, minute, seconds;

struct gui_nvram_t gui_nvram;
static enc2_menu_t gui_enc2_menu = { "", "", 0, 0, };

static menu_by_name_t menu_uif;

static keyb_t gui_keyboard;

enum { enc2step_vals = ARRAY_SIZE(enc2step) };
enum { freq_swipe_step_vals = ARRAY_SIZE(freq_swipe_step) };

static text_field_t * tf_debug = NULL;
static tf_entry_t tmpbuf[TEXT_ARRAY_SIZE];
static uint_fast8_t tmpstr_index = 0;

#if GUI_SHOW_INFOBAR

//todo: перенести раскладку инфобара в конфиг
static uint8_t infobar_selected = UINT8_MAX;
const uint8_t infobar_places [infobar_num_places] = {
		INFOBAR_AF,
		INFOBAR_AF_VOLUME,
		INFOBAR_ATT,
		INFOBAR_DNR,
		INFOBAR_TX_POWER,
#if WITHVOLTLEVEL
		INFOBAR_VOLTAGE | INFOBAR_NOACTION,
#elif WITHUSEDUALWATCH
		INFOBAR_DUAL_RX,
#else
		INFOBAR_EMPTY,
#endif /* WITHPS7BOARD_EBAZ4205 */
		INFOBAR_CPU_TEMP | INFOBAR_NOACTION,
		INFOBAR_2ND_ENC_MENU
};

#endif /* GUI_SHOW_INFOBAR */

#if WITHFT8

#include "ft8.h"

const uint_fast32_t ft8_bands [] = {
		1840000uL,
		3573000uL,
		5357000uL,
		7074000uL,
		10136000uL,
		14074000uL,
		18100000uL,
		21074000uL,
		24915000uL,
		28074000uL,
};

enum {
	ft8_band_default = 3, 						// 40m
	ft8_txfreq_default = 1500,
	ft8_txfreq_equal_default = 1,
	ft8_mode = SUBMODE_USB,
	ft8_bands_count = ARRAY_SIZE(ft8_bands),
};

typedef enum {
	FT8_STATE_INIT,
	FT8_STATE_TX_REQ,	// передача вызываемому корреспонденту рапорта
	FT8_STATE_RX_ANS,	// прием от корреспондента ответа
	FT8_STATE_TX_END,	// передача корреспонденту завершения связи 73
	FT8_STATE_RX_ACK,	// прием от корреспондента подтверждения 73
} ft8_state_t;

ft8_state_t ft8_state;

typedef struct {
	label_t * ptr;
} lh_array_t;

#endif /* #if WITHFT8 */

#if WITHLWIP

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/ip.h"
#include "lwip/udp.h"
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/tcp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/timeouts.h"

int ping_check_response(void);
int ping_send_ip(const char * ip_str);

#endif

void gui_encoder2_menu (enc2_menu_t * enc2_menu)
{
	memcpy(& gui_enc2_menu, enc2_menu, sizeof (gui_enc2_menu));
	gui_enc2_menu.updated = 1;
}

void load_settings(void)
{
	hamradio_load_gui_settings(& gui_nvram);

	if (gui_nvram.enc2step_pos == UINT8_MAX)
		gui_nvram.enc2step_pos = enc2step_default;

	if (gui_nvram.freq_swipe_enable == UINT8_MAX)
		gui_nvram.freq_swipe_enable = freq_swipe_enable_default;

	if (gui_nvram.freq_swipe_step == UINT8_MAX)
		gui_nvram.freq_swipe_step = freq_swipe_step_default;

#if WITHFT8
	if (gui_nvram.ft8_callsign [0] == UINT8_MAX)
		local_snprintf_P(gui_nvram.ft8_callsign, ARRAY_SIZE(gui_nvram.ft8_callsign), "RA4ASN");

	if (gui_nvram.ft8_snr [0] == UINT8_MAX)
		local_snprintf_P(gui_nvram.ft8_snr, ARRAY_SIZE(gui_nvram.ft8_snr), "-20");

	if (gui_nvram.ft8_qth [0] == UINT8_MAX)
		local_snprintf_P(gui_nvram.ft8_qth, ARRAY_SIZE(gui_nvram.ft8_qth), "LO10");

	if (gui_nvram.ft8_end [0] == UINT8_MAX)
		local_snprintf_P(gui_nvram.ft8_end, ARRAY_SIZE(gui_nvram.ft8_end), "RR73");

	if (gui_nvram.ft8_band == UINT8_MAX)
		gui_nvram.ft8_band = ft8_band_default;

	if (gui_nvram.ft8_txfreq_val == UINT32_MAX)
		gui_nvram.ft8_txfreq_val = ft8_txfreq_default;

	if (gui_nvram.ft8_txfreq_equal == UINT8_MAX)
		gui_nvram.ft8_txfreq_equal = ft8_txfreq_equal_default;
#endif /* WITHFT8 */

#if WITHAFCODEC1HAVEPROC
	if (gui_nvram.micprofile != micprofile_default && gui_nvram.micprofile < NMICPROFCELLS)
		hamradio_load_mic_profile(gui_nvram.micprofile, 1);
	else
		gui_nvram.micprofile = micprofile_default;
#endif /* WITHAFCODEC1HAVEPROC */
}

void save_settings(void)
{
	hamradio_save_gui_settings(& gui_nvram);
}

#if WITHGUIDEBUG
void gui_add_debug(char d)
{
	static char str [TEXT_ARRAY_SIZE] = { 0 };
	static unsigned i = 0;

	if (d == '\r')
	{
		i = 0;
		return;
	}

	if (d != '\n')
	{
		if (i < TEXT_ARRAY_SIZE)
			str [i ++ ] = d;
	}
	else
	{
		i = 0;
		if (tf_debug)
			textfield_add_string(tf_debug, str, COLORPIP_WHITE);
		else
		{
			if (tmpstr_index < TEXT_ARRAY_SIZE)
			{
				memcpy(tmpbuf[tmpstr_index].text, str, TEXT_ARRAY_SIZE);
				tmpstr_index ++;
			}
		}
		memset(str, 0, ARRAY_SIZE(str));
	}
}

void gui_open_debug_window(void)
{
	if (tf_debug)
		tf_debug->visible = ! tf_debug->visible;
}
#endif /* WITHGUIDEBUG */

static window_t windows [] = {
//     window_id,   		 parent_id, 			align_mode,     title,     				is_close, onVisibleProcess
	{ WINDOW_MAIN, 			 NO_PARENT_WINDOW, 		ALIGN_LEFT_X,	"",  	   	   			 0, gui_main_process, },
	{ WINDOW_MODES, 		 WINDOW_RECEIVE, 		ALIGN_CENTER_X, "Select mode", 			 1, window_mode_process, },
	{ WINDOW_AF,    		 WINDOW_RECEIVE,		ALIGN_CENTER_X, "AF settings",    		 1, window_af_process, },
	{ WINDOW_FREQ,  		 WINDOW_BANDS,			ALIGN_CENTER_X, "Freq:", 	   			 1, window_freq_process, },
	{ WINDOW_MENU,  		 WINDOW_OPTIONS,		ALIGN_CENTER_X, "Settings",	   		 	 1, window_menu_process, },
	{ WINDOW_MENU_PARAMS,    WINDOW_MENU,		    ALIGN_CENTER_X, "Settings",	   		 	 1, window_menu_params_process, },
	{ WINDOW_UIF, 			 NO_PARENT_WINDOW, 		ALIGN_LEFT_X, 	"",   		   	 		 0, window_uif_process, },
#if WITHSWRSCAN
	{ WINDOW_SWR_SCANNER,	 WINDOW_UTILS, 			ALIGN_CENTER_X, "SWR band scanner",		 0, window_swrscan_process, },
#endif /* WITHSWRSCAN */
	{ WINDOW_AUDIOSETTINGS,  WINDOW_OPTIONS,		ALIGN_CENTER_X, "Audio settings", 		 1, window_audiosettings_process, },
	{ WINDOW_AP_MIC_EQ, 	 WINDOW_AUDIOSETTINGS, 	ALIGN_CENTER_X, "MIC TX equalizer",		 1, window_ap_mic_eq_process, },
#if WITHREVERB
	{ WINDOW_AP_REVERB_SETT, WINDOW_AUDIOSETTINGS, 	ALIGN_CENTER_X, "Reverberator settings", 1, window_ap_reverb_process, },
#endif /* WITHREVERB */
	{ WINDOW_AP_MIC_SETT, 	 WINDOW_AUDIOSETTINGS, 	ALIGN_CENTER_X, "Microphone settings", 	 1, window_ap_mic_process, },
	{ WINDOW_AP_MIC_PROF, 	 WINDOW_AUDIOSETTINGS, 	ALIGN_CENTER_X, "Microphone profiles", 	 1, window_ap_mic_prof_process, },
	{ WINDOW_TX_SETTINGS, 	 WINDOW_OPTIONS, 		ALIGN_CENTER_X, "Transmit settings", 	 1, window_tx_process, },
	{ WINDOW_TX_VOX_SETT, 	 WINDOW_TX_SETTINGS, 	ALIGN_CENTER_X, "VOX settings", 	 	 1, window_tx_vox_process, },
	{ WINDOW_TX_POWER, 		 WINDOW_TX_SETTINGS, 	ALIGN_CENTER_X, "TX power", 	 	 	 1, window_tx_power_process, },
	{ WINDOW_OPTIONS, 		 NO_PARENT_WINDOW, 		ALIGN_CENTER_X,	"Options",  	   	   	 1, window_options_process, },
	{ WINDOW_UTILS, 		 WINDOW_OPTIONS,		ALIGN_CENTER_X,	"Utilites",  	   	   	 1, window_utilites_process, },
	{ WINDOW_BANDS, 		 NO_PARENT_WINDOW,		ALIGN_CENTER_X,	"Bands",  	   	   	 	 1,	window_bands_process, },
	{ WINDOW_MEMORY, 		 NO_PARENT_WINDOW,		ALIGN_CENTER_X,	"Memory",  	   	   	 	 1, window_memory_process, },
	{ WINDOW_DISPLAY, 		 WINDOW_OPTIONS,		ALIGN_CENTER_X,	"Display settings",  	 1, window_display_process, },
	{ WINDOW_RECEIVE, 		 NO_PARENT_WINDOW, 		ALIGN_CENTER_X, "Receive settings", 	 1, window_receive_process, },
	{ WINDOW_NOTCH, 		 NO_PARENT_WINDOW, 		ALIGN_CENTER_X, "Notch", 	 	 		 1, window_notch_process, },
	{ WINDOW_GUI_SETTINGS, 	 WINDOW_OPTIONS, 		ALIGN_CENTER_X, "GUI settings",	 		 1, window_gui_settings_process, },
#if WITHFT8
	{ WINDOW_FT8, 			 NO_PARENT_WINDOW,		ALIGN_CENTER_X, "FT8 terminal",		 	 1, window_ft8_process, },
	{ WINDOW_FT8_BANDS, 	 WINDOW_FT8,			ALIGN_CENTER_X, "FT8 bands",		 	 0, window_ft8_bands_process, },
	{ WINDOW_FT8_SETTINGS, 	 WINDOW_FT8,			ALIGN_CENTER_X, "FT8 settings",		 	 0, window_ft8_settings_process, },
#endif /* #if WITHFT8 */
	{ WINDOW_INFOBAR_MENU, 	 NO_PARENT_WINDOW,		ALIGN_MANUAL,   "",		 				 0, window_infobar_menu_process, },
	{ WINDOW_AF_EQ, 	 	 NO_PARENT_WINDOW,		ALIGN_CENTER_X, "AF equalizer",			 1, window_af_eq_process, },
#if WITHIQSHIFT
	{ WINDOW_SHIFT, 	 	 WINDOW_UTILS,			ALIGN_CENTER_X, "IQ shift",				 1, window_shift_process, },
#endif /* WITHIQSHIFT */
	{ WINDOW_TIME, 	 	 	 WINDOW_OPTIONS,		ALIGN_CENTER_X, "Date & time set",		 1, window_time_process, },
	{ WINDOW_KBD, 	 	 	 NO_PARENT_WINDOW,		ALIGN_CENTER_X, "Keyboard",		 		 0, window_kbd_process, },
	{ WINDOW_KBD_TEST, 		 WINDOW_UTILS,			ALIGN_CENTER_X, "Keyboard demo",	 	 1, window_kbd_test_process, },
#if WITHLWIP
	{ WINDOW_PING, 		 	 WINDOW_UTILS,			ALIGN_CENTER_X, "Network ping test",	 1, window_ping_process, },
#endif /* WITHLWIP */
	{ WINDOW_3D, 		 	 WINDOW_UTILS,			ALIGN_CENTER_X, "Donut 3d ASCII demo",	 1, window_3d_process, },
#if WITHLFM
	{ WINDOW_LFM, 		 	 WINDOW_UTILS,			ALIGN_RIGHT_X,  "LFM",			 		 1, window_lfm_process, },
	{ WINDOW_LFM_SPECTRE, 	 WINDOW_LFM,			ALIGN_CENTER_X, "LFM spectre draw",	 	 1, window_lfm_spectre_process, },
#endif /* WITHLFM  */
#if WITHEXTIO_LAN
	{ WINDOW_EXTIOLAN, 		 WINDOW_UTILS,			ALIGN_CENTER_X,  "LAN IQ Stream server", 1, window_stream_process, },
#endif /* WITHEXTIO_LAN */
#if WITHWNB
	{ WINDOW_WNBCONFIG, 	 WINDOW_RECEIVE,		ALIGN_CENTER_X,  "WNB config", 			 1, window_wnbconfig_process, },
#endif /* WITHWNB */
#if WITHAD936XIIO
	{ WINDOW_IIOCONFIG,  	 NO_PARENT_WINDOW,		ALIGN_CENTER_X,  "AD936x IIO config", 	 1, window_iioconfig_process, },
#endif /* WITHAD936XIIO */
};

/* Возврат ссылки на окно */
window_t * get_win(uint8_t window_id)
{
	if (window_id == NO_PARENT_WINDOW)	//	костыль
		return & windows [0];

	ASSERT(window_id < WINDOWS_COUNT);
	return & windows [window_id];
}

void gui_user_actions_after_close_window(void)
{
	hamradio_disable_encoder2_redirect();
	hamradio_set_lockmode(0);
	gui_update();
}

static void keyboard_edit_string(uintptr_t s, unsigned strlen, unsigned clean)
{
	gui_keyboard.str = (char *) s;
	gui_keyboard.clean = clean;
	gui_keyboard.max_len = strlen;
	gui_keyboard.digits_only = 0;
	window_t * win_kbd = get_win(WINDOW_KBD);
	win_kbd->parent_id = check_for_parent_window();
	open_window(win_kbd);
}

static void keyboard_edit_digits(uint32_t * val)
{
	gui_keyboard.num = val;
	gui_keyboard.digits_only = 1;
	window_t * win_kbd = get_win(WINDOW_KBD);
	win_kbd->parent_id = check_for_parent_window();
	open_window(win_kbd);
}

// *********************************************************************************************************************************************************************

static void window_infobar_menu_process(void)
{
#if GUI_SHOW_INFOBAR
	window_t * const win = get_win(WINDOW_INFOBAR_MENU);
	unsigned interval = 5, yy = 0, need_close = 0, need_open = 255;
	unsigned infobar = infobar_places [infobar_selected] & INFOBAR_VALID_MASK;

	if (win->first_call)
	{
		win->first_call = 0;

		static const button_t buttons [] = {
			{ 86, 30, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_INFOBAR_MENU, NON_VISIBLE, INT32_MAX, "btn_0", "",	},
			{ 86, 30, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_INFOBAR_MENU, NON_VISIBLE, INT32_MAX, "btn_1", "",	},
			{ 86, 30, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_INFOBAR_MENU, NON_VISIBLE, INT32_MAX, "btn_2", "",	},
			{ 86, 30, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_INFOBAR_MENU, NON_VISIBLE, INT32_MAX, "btn_3", "",	},
			{ 86, 30, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_INFOBAR_MENU, NON_VISIBLE, INT32_MAX, "btn_4", "", },
			{ 86, 30, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_INFOBAR_MENU, NON_VISIBLE, INT32_MAX, "btn_5", "",	},
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		switch (infobar)
		{
#if WITHUSEDUALWATCH
		case INFOBAR_DUAL_RX:
		{
			for (unsigned i = 0; i < 3; i ++)
			{
				char btn_name [6] = { 0 };
				local_snprintf_P(btn_name, ARRAY_SIZE(btn_name), PSTR("btn_%d"), i);
				button_t * bh = (button_t*) find_gui_element(TYPE_BUTTON, win, btn_name);
				bh->x1 = 0;
				bh->y1 = yy;
				bh->visible = VISIBLE;
				bh->payload = i + 1;

				if (i == 0)
				{
					uint_fast8_t state;
					hamradio_get_vfomode3_value(& state);
					bh->is_locked = state != 0;
					local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), "SPLIT");
				}
				else if (i == 1)
				{
					local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), "A<->B");
				}
				else if (i == 2)
				{
					local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), hamradio_get_mainsubrxmode3_value_P());
				}
				yy = yy + interval + bh->h;
			}
		}
			break;
#endif /* WITHUSEDUALWATCH */
		case INFOBAR_AF_VOLUME:
		{
			for (unsigned i = 0; i < 4; i ++)
			{
				char btn_name [6] = { 0 };
				local_snprintf_P(btn_name, ARRAY_SIZE(btn_name), PSTR("btn_%d"), i);
				button_t * bh = (button_t*) find_gui_element(TYPE_BUTTON, win, btn_name);

#if ! WITHPOTAFGAIN
				if (i == 0)
				{
					bh->payload = 1;
					bh->is_repeating = 1;
					bh->x1 = 0;
					bh->y1 = yy;
					bh->visible = VISIBLE;
					local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), "+");
					yy = yy + interval + bh->h;
				}
				else if (i == 1)
				{
					bh->payload = -1;
					bh->is_repeating = 1;
					bh->x1 = 0;
					bh->y1 = yy;
					bh->visible = VISIBLE;
					local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), "-");
					yy = yy + interval + bh->h;
				}
				else
#endif /* WITHPOTAFGAIN */
				if (i == 2)
				{
					bh->payload = 10;
					bh->x1 = 0;
					bh->y1 = yy;
					bh->visible = VISIBLE;
					local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), "mute");
					bh->is_locked = hamradio_get_gmutespkr() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
					yy = yy + interval + bh->h;
				}
#if WITHAFEQUALIZER
				else if (i == 3)
				{
					bh->payload = 20;
					bh->x1 = 0;
					bh->y1 = yy;
					bh->visible = VISIBLE;
					local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), "EQ");
					yy = yy + interval + bh->h;
				}
#endif /* #if WITHAFEQUALIZER */
			}
		}
			break;

		case INFOBAR_ATT:
		{
			uint_fast8_t atts [6];
			unsigned count = hamradio_get_att_dbs(atts, 6);

			for (unsigned i = 0; i < count; i ++)
			{
				char btn_name [6] = { 0 };
				local_snprintf_P(btn_name, ARRAY_SIZE(btn_name), PSTR("btn_%d"), i);
				button_t * bh = (button_t*) find_gui_element(TYPE_BUTTON, win, btn_name);
				bh->x1 = 0;
				bh->y1 = yy;
				bh->visible = VISIBLE;
				bh->payload = i;
				if (atts [i])
					local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), "%d db", atts [i] / 10);
				else
					local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), "off");
				yy = yy + interval + bh->h;
			}

		}
			break;

		case INFOBAR_AF:
		{
			bws_t bws;
			unsigned count = hamradio_get_bws(& bws, 5);

			for (unsigned i = 0; i < 6; i ++)
			{
				char btn_name [6] = { 0 };
				local_snprintf_P(btn_name, ARRAY_SIZE(btn_name), PSTR("btn_%d"), i);
				button_t * bh = (button_t*) find_gui_element(TYPE_BUTTON, win, btn_name);
				bh->x1 = 0;
				bh->y1 = yy;
				bh->visible = VISIBLE;

				if (i >= count)
				{
					bh->payload = 255;
					local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), "Filter");
					break;
				}

				bh->payload = i;
				local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), "%s", bws.label [i]);
				yy = yy + interval + bh->h;
			}

		}
			break;

		case INFOBAR_TX_POWER:

			need_open = infobar;
			break;

		case INFOBAR_DNR:

			hamradio_change_nr();
			need_close = 1;
			break;

		default:

			need_close = 1;
			break;
		}

		calculate_window_position(win, WINDOW_POSITION_MANUAL_POSITION, infobar_selected * infobar_label_width, infobar_2nd_str_y + SMALLCHARH2);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;

			switch (infobar)
			{
#if WITHUSEDUALWATCH
			case INFOBAR_DUAL_RX:
			{
				if (bh->payload == 1) // SPLIT on/off
				{
					bh->is_locked = hamradio_split_toggle();
				}
				else if (bh->payload == 2) // SPLIT swap
				{
					hamradio_split_vfo_swap();
				}
				else if (bh->payload == 3) // SPLIT mode
				{
					hamradio_split_mode_toggle();
					local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), hamradio_get_mainsubrxmode3_value_P());
				}
			}
				break;
#endif /* WITHUSEDUALWATCH */
			case INFOBAR_AF_VOLUME:
			{
#if ! WITHPOTAFGAIN
				if (bh->payload == -1 || bh->payload == 1)
					hamradio_set_afgain(hamradio_get_afgain() + bh->payload);
#endif /* ! WITHPOTAFGAIN */
				if (bh->payload == 10)
				{
					hamradio_set_gmutespkr(! hamradio_get_gmutespkr());
					bh->is_locked = hamradio_get_gmutespkr() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
				}

				if (bh->payload == 20)
				{
					close_all_windows();
					window_t * const win2 = get_win(WINDOW_AF_EQ);
					open_window(win2);
					footer_buttons_state(DISABLED);
				}
			}
				break;

			case INFOBAR_ATT:
			{
				hamradio_set_att_db(bh->payload);
				close_all_windows();
			}
				break;

			case INFOBAR_AF:
			{
				if (bh->payload == 255)
				{
					close_all_windows();
					window_t * const win2 = get_win(WINDOW_AF);
					open_window(win2);
					footer_buttons_state(DISABLED);
				}
				else
				{
					hamradio_set_bw(bh->payload);
					close_all_windows();
				}
			}
				break;

			default:
				break;
			}
		}
		break;

		case WM_MESSAGE_ENC2_ROTATE:
		{
			if (infobar == INFOBAR_AF_VOLUME)
			{
#if ! WITHPOTAFGAIN
				hamradio_set_afgain(hamradio_get_afgain() + action);
#endif /* ! WITHPOTAFGAIN */
			}
		}
		break;

		default:
			break;
	}

	if (need_open != 255)
	{
		switch (need_open)
		{
		case INFOBAR_TX_POWER:
		{
			close_all_windows();
			window_t * const win2 = get_win(WINDOW_TX_POWER);
			open_window(win2);
			footer_buttons_state(DISABLED);
		}
			break;

		default:
			break;
		}
	}

	if (need_close)
		close_all_windows();
#endif /* GUI_SHOW_INFOBAR */
}

// *********************************************************************************************************************************************************************

static void gui_main_process(void)
{
	window_t * const win = get_win(WINDOW_MAIN);

	PACKEDCOLORPIP_T * const fr = colmain_fb_draw();
	char buf [TEXT_ARRAY_SIZE];
	const unsigned buflen = ARRAY_SIZE(buf);
	unsigned update = 0;
	static unsigned freq_swipe;

	if (win->first_call)
	{
		unsigned interval_btn = 3, x = 0;
		ASSERT(win != NULL);
		win->first_call = 0;
		gui_enc2_menu.updated = 1;
		gui_keyboard.clean = 0;
		gui_keyboard.digits_only = 0;
		update = 1;

#if WITHLFM
		hamradio_set_lfmmode(0);
#endif /* WITHLFM */

#if WITHGUIDEBUG
		static const text_field_t text_field [] = {
			{ TEXT_ARRAY_SIZE, 25, CANCELLED, WINDOW_MAIN, NON_VISIBLE, DOWN, NULL, "tf_debug", },
		};
		win->tf_count = ARRAY_SIZE(text_field);
		unsigned tf_size = sizeof(text_field);
		win->tf_ptr = (text_field_t*) malloc(tf_size);
		GUI_MEM_ASSERT(win->tf_ptr);
		memcpy(win->tf_ptr, text_field, tf_size);

		tf_debug = (text_field_t*) find_gui_element(TYPE_TEXT_FIELD, win, "tf_debug");
		textfield_update_size(tf_debug);
		tf_debug->x1 = win->w / 2 - tf_debug->w / 2;
		tf_debug->y1 = win->h / 2 - tf_debug->h / 2;
#endif /* WITHGUIDEBUG */

		static const button_t buttons [] = {
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 1, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_txrx", 		"RX", 				},
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_Bands", 	"Bands", 			},
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_Memory",  	"Memory", 			},
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_Receive", 	"Receive|options", 	},
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 1, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_notch",   	"", 				},
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_speaker", 	"Speaker|on air", 	},
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_iio",  	 	"AD936x", 			},
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_ft8", 		"", 				},
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_Options", 	"Options", 			},
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

#if WITHFT8
		button_t * btn_ft8 = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_ft8");
		local_snprintf_P(btn_ft8->text, ARRAY_SIZE(btn_ft8->text), PSTR("FT8"));
#endif /* WITHFT8 */

#if ! WITHWNB
		button_t * btn_wnb = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_wnb");
		local_snprintf_P(btn_wnb->text, ARRAY_SIZE(btn_wnb->text), "");
#endif /* ! WITHWNB */

#if GUI_SHOW_INFOBAR

		static const touch_area_t tas [] = {
			{ CANCELLED, WINDOW_MAIN, NON_VISIBLE, 0, INT32_MAX, "ta_infobar_1", },
			{ CANCELLED, WINDOW_MAIN, NON_VISIBLE, 0, INT32_MAX, "ta_infobar_2", },
			{ CANCELLED, WINDOW_MAIN, NON_VISIBLE, 0, INT32_MAX, "ta_infobar_3", },
			{ CANCELLED, WINDOW_MAIN, NON_VISIBLE, 0, INT32_MAX, "ta_infobar_4", },
			{ CANCELLED, WINDOW_MAIN, NON_VISIBLE, 0, INT32_MAX, "ta_infobar_5", },
			{ CANCELLED, WINDOW_MAIN, NON_VISIBLE, 0, INT32_MAX, "ta_infobar_6", },
			{ CANCELLED, WINDOW_MAIN, NON_VISIBLE, 0, INT32_MAX, "ta_infobar_7", },
			{ CANCELLED, WINDOW_MAIN, NON_VISIBLE, 1, INT32_MAX, "ta_freq", },
		};
		win->ta_count = ARRAY_SIZE(tas);
		unsigned ta_size = sizeof(tas);
		win->ta_ptr = (touch_area_t *) malloc(ta_size);
		GUI_MEM_ASSERT(win->ta_ptr);
		memcpy(win->ta_ptr, tas, ta_size);

		for (unsigned id = 0; id < infobar_num_places; id ++)
		{
			touch_area_t * ta = & win->ta_ptr [id];
			ta->x1 = id * infobar_label_width;
			ta->y1 = infobar_1st_str_y;
			ta->w = infobar_label_width;
			ta->h = infobar_2nd_str_y;
			ta->index = id;
			ta->visible = VISIBLE;
		}

		touch_area_t * ta_freq = (touch_area_t *) find_gui_element(TYPE_TOUCH_AREA, win, "ta_freq");
		ta_freq->x1 = 0;
		ta_freq->y1 = infobar_2nd_str_y + SMALLCHARH2 + 5;
		ta_freq->w = DIM_X;
		ta_freq->h = DIM_Y - FOOTER_HEIGHT - ta_freq->y1;
		ta_freq->index = 255;
		ta_freq->visible = VISIBLE;

#endif /* GUI_SHOW_INFOBAR */

		for (unsigned id = 0; id < win->bh_count; id ++)
		{
			button_t * bh = & win->bh_ptr [id];
			bh->x1 = x;
			bh->y1 = WITHGUIMAXY - bh->h - 1;
			bh->visible = VISIBLE;
			x = x + interval_btn + bh->w;
		}

		load_settings();
		elements_state(win);
		hamradio_set_lockmode(0);

#if WITHGUIDEBUG
		for (unsigned i = 0; i < tmpstr_index; i ++)
		{
			textfield_add_string(tf_debug, tmpbuf[i].text, COLORPIP_WHITE);
		}
#endif /* WITHGUIDEBUG */
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_AREA_MOVE)
		{
			touch_area_t * th = (touch_area_t *) ptr;
			touch_area_t * ta_freq = (touch_area_t *) find_gui_element(TYPE_TOUCH_AREA, win, "ta_freq");

			if (th == ta_freq && gui_nvram.freq_swipe_enable && check_for_parent_window() == NO_PARENT_WINDOW)
			{
				int_fast16_t move_x = 0, move_y = 0;
				get_gui_tracking(& move_x, & move_y);
				if (move_x != 0)
					hamradio_set_freq(hamradio_get_freq_rx() - (int) (move_x  * freq_swipe));
			}
		}

#if GUI_SHOW_INFOBAR
		if (IS_AREA_TOUCHED)
		{
			touch_area_t * ta = (touch_area_t *) ptr;
			if (ta->index == 255)
				break;

			infobar_selected = ta->index;
			ASSERT(infobar_selected < infobar_num_places);
			unsigned infobar = infobar_places [infobar_selected];

			if (infobar != INFOBAR_EMPTY && ! ((infobar & INFOBAR_NOACTION) >> INFOBAR_NOACTION_POS))
			{
				if (check_for_parent_window() == WINDOW_INFOBAR_MENU)
				{
					close_window(DONT_OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
				}
				else if (check_for_parent_window() == NO_PARENT_WINDOW)
				{
					window_t * const win = get_win(WINDOW_INFOBAR_MENU);
					open_window(win);
					footer_buttons_state(DISABLED);
				}
			}
		}
#endif /* GUI_SHOW_INFOBAR */

		if (IS_BUTTON_PRESS)	// обработка короткого нажатия кнопок
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_txrx = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_txrx");
			button_t * btn_notch = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_notch");
			button_t * btn_Bands = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_Bands");
			button_t * btn_Memory = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_Memory");
			button_t * btn_Options = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_Options");
			button_t * btn_speaker = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_speaker");
			button_t * btn_Receive = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_Receive");
			button_t * btn_ft8 = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_ft8");

			if (bh == btn_notch)
			{
				hamradio_set_gnotch(! hamradio_get_gnotch());
				update = 1;
			}
#if WITHFT8
			else if (bh == btn_ft8)
			{
				if (check_for_parent_window() != NO_PARENT_WINDOW)
				{
					close_window(OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
				}
				else
				{
					window_t * const win = get_win(WINDOW_FT8);
					open_window(win);
					footer_buttons_state(DISABLED, btn_ft8);
				}
			}
#endif /* WITHFT8 */
#if WITHSPKMUTE
			else if (bh == btn_speaker)
			{
				hamradio_set_gmutespkr(! hamradio_get_gmutespkr());
				update = 1;
			}
#endif /* #if WITHSPKMUTE */
			else if (bh == btn_Bands)
			{
				if (check_for_parent_window() != NO_PARENT_WINDOW)
				{
					close_window(OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
				}
				else
				{
					window_t * const win = get_win(WINDOW_BANDS);
					open_window(win);
					footer_buttons_state(DISABLED, btn_Bands);
				}
			}
			else if (bh == btn_Memory)
			{
				window_t * const win = get_win(WINDOW_MEMORY);
				if (win->state == NON_VISIBLE)
				{
					open_window(win);
					footer_buttons_state(DISABLED, btn_Memory);
				}
				else
				{
					close_window(OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
				}
			}
			else if (bh == btn_Options)
			{
				if (check_for_parent_window() != NO_PARENT_WINDOW)
				{
					close_window(OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
					hamradio_set_lockmode(0);
					hamradio_disable_keyboard_redirect();
				}
				else
				{
					window_t * const win = get_win(WINDOW_OPTIONS);
					open_window(win);
					footer_buttons_state(DISABLED, btn_Options);
				}
			}
			else if (bh == btn_Receive)
			{
				if (check_for_parent_window() != NO_PARENT_WINDOW)
				{
					close_window(OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
					hamradio_set_lockmode(0);
					hamradio_disable_keyboard_redirect();
				}
				else
				{
					window_t * const win = get_win(WINDOW_RECEIVE);
					open_window(win);
					footer_buttons_state(DISABLED, btn_Receive);
				}
			}
#if WITHTX
			else if (bh == btn_txrx)
			{
				hamradio_gui_set_reqautotune2(0);
				hamradio_moxmode(1);
				update = 1;
			}
#endif /* WITHTX */
#if LINUX_SUBSYSTEM && WITHAD936XIIO
			else if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_iio"))
			{
				open_window(get_win(WINDOW_IIOCONFIG));
			}
#endif /* LINUX_SUBSYSTEM && WITHAD936XIIO */
		}
		else if (IS_BUTTON_LONG_PRESS)			// обработка длинного нажатия
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_txrx = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_txrx");
			button_t * btn_notch = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_notch");
			button_t * btn_wnb = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_wnb");
#if WITHTX
			if (bh == btn_txrx)
			{
				hamradio_gui_set_reqautotune2(1);
				hamradio_tunemode(1);
				update = 1;
			}
			else
#endif /* WITHTX */
			if (bh == btn_notch)
			{
				window_t * const win = get_win(WINDOW_NOTCH);
				if (win->state == NON_VISIBLE)
				{
					open_window(win);
					footer_buttons_state(DISABLED, btn_notch);
				}
				else
				{
					close_window(OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
				}
			}
#if WITHWNB
			else if (bh == btn_wnb)
			{
				window_t * const win = get_win(WINDOW_WNBCONFIG);
				if (win->state == NON_VISIBLE)
				{
					open_window(win);
					footer_buttons_state(DISABLED, btn_wnb);
				}
				else
				{
					close_window(OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
				}
			}
#endif /* WITHWNB */
		}
		break;

	case WM_MESSAGE_UPDATE:

		update = 1;
		break;

	case WM_MESSAGE_ENC2_ROTATE:	// если не открыто 2-е окно, 2-й энкодер подстраивает частоту с округлением 500 гц от текущего значения
	{
		unsigned step = enc2step [gui_nvram.enc2step_pos].step;
		unsigned freq = hamradio_get_freq_rx();
		unsigned f_rem = freq % step;

		if (action > 0)
		{
			hamradio_set_freq(freq + (step - f_rem) * abs(action));
		}
		else if (action < 0)
		{
			hamradio_set_freq(freq - (f_rem ? f_rem : step) * abs(action));
		}
	}
		break;

	default:

		break;
	}

	if (update)											// обновление состояния элементов при действиях с ними, а также при запросах из базовой системы
	{
		freq_swipe = freq_swipe_step[gui_nvram.freq_swipe_step].step;

		button_t * btn_notch = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_notch");
		btn_notch->is_locked = hamradio_get_gnotch() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		unsigned notch_type = hamradio_get_gnotchtype();
		if (notch_type == 1)
			local_snprintf_P(btn_notch->text, ARRAY_SIZE(btn_notch->text), PSTR("Notch|manual"));
		else if (notch_type == 2)
			local_snprintf_P(btn_notch->text, ARRAY_SIZE(btn_notch->text), PSTR("Notch|auto"));

		button_t * btn_speaker = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_speaker");
#if WITHSPKMUTE
		btn_speaker->is_locked = hamradio_get_gmutespkr() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(btn_speaker->text, ARRAY_SIZE(btn_speaker->text), PSTR("Speaker|%s"), btn_speaker->is_locked ? "muted" : "on air");
#else
		btn_speaker->state = DISABLED;
		local_snprintf_P(btn_speaker->text, ARRAY_SIZE(btn_speaker->text), PSTR("Speaker|on air"));
#endif /* #if WITHSPKMUTE */

		button_t * btn_txrx = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_txrx");
#if WITHTX
		unsigned tune = hamradio_tunemode(0);
		unsigned mox = hamradio_moxmode(0);

		if (tune)
		{
			btn_txrx->is_locked = BUTTON_LOCKED;
			local_snprintf_P(btn_txrx->text, ARRAY_SIZE(btn_txrx->text), PSTR("TX|tune"));
		}
		else if (! tune && mox)
		{
			btn_txrx->is_locked = BUTTON_LOCKED;
			local_snprintf_P(btn_txrx->text, ARRAY_SIZE(btn_txrx->text), PSTR("TX"));
		}
		else if (! tune && ! mox)
		{
			btn_txrx->is_locked = BUTTON_NON_LOCKED;
			local_snprintf_P(btn_txrx->text, ARRAY_SIZE(btn_txrx->text), PSTR("RX"));
		}
#else
		btn_txrx->state = DISABLED;
		local_snprintf_P(btn_txrx->text, ARRAY_SIZE(btn_txrx->text), PSTR("RX"));
#endif /* WITHTX */

#if defined (RTC1_TYPE)
		board_rtc_cached_getdatetime(& year, & month, & day, & hour, & minute, & seconds);
#endif /* defined (RTC1_TYPE) */
	}

#if GUI_SHOW_INFOBAR

	const window_t * const pwin = get_win(check_for_parent_window());
	if (pwin->size_mode != WINDOW_POSITION_FULLSCREEN)
	{
		// разметка инфобара
		const unsigned y_mid = infobar_1st_str_y + (infobar_2nd_str_y - infobar_1st_str_y) / 2;
		const unsigned infobar_hl = (infobar_selected < infobar_num_places) && (check_for_parent_window() == WINDOW_INFOBAR_MENU);

		for(unsigned i = 1; i < infobar_num_places; i++)
		{
			uint_fast16_t x = infobar_label_width * i;
			colpip_line(fr, DIM_X, DIM_Y, x, infobar_1st_str_y, x, infobar_2nd_str_y + SMALLCHARH2, COLORPIP_GREEN, 0);
		}

		if (infobar_hl)
		{
			uint16_t x1 = infobar_selected * infobar_label_width + 2;
			uint16_t x2 = x1 + infobar_label_width - 4;

			colmain_rounded_rect(fr, DIM_X, DIM_Y, x1, infobar_1st_str_y, x2, infobar_2nd_str_y + SMALLCHARH2 - 2, 5, COLORPIP_YELLOW, 1);
		}

		for (unsigned current_place = 0; current_place < infobar_num_places; current_place ++)
		{
			uint_fast8_t infobar = infobar_places [current_place] & INFOBAR_VALID_MASK;
			COLORPIP_T str_color = (current_place == infobar_selected) && infobar_hl ? COLORPIP_BLACK : COLORPIP_WHITE;

			switch (infobar)
			{
			case INFOBAR_DUAL_RX:
			{
#if WITHUSEDUALWATCH
				static uint_fast8_t val = 0;
				unsigned xx = current_place * infobar_label_width + infobar_label_width / 2;

				if (update)
					hamradio_get_vfomode3_value(& val);

				local_snprintf_P(buf, buflen, "Dual RX");
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, str_color);
				local_snprintf_P(buf, buflen, "VFO %s", hamradio_get_gvfoab() ? "2" : "1");
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, str_color);
#endif /* WITHUSEDUALWATCH */
			}
				break;

			case INFOBAR_AF_VOLUME:
			{
				static uint_fast8_t vol;
				unsigned xx = current_place * infobar_label_width + infobar_label_width / 2;

				if (update)
					vol = hamradio_get_afgain();

				local_snprintf_P(buf, buflen, PSTR("AF gain"));
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, str_color);
				if (hamradio_get_gmutespkr())
					local_snprintf_P(buf, buflen, PSTR("muted"));
				else
					local_snprintf_P(buf, buflen, PSTR("%d"), vol);
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, str_color);
			}
				break;

			case INFOBAR_TX_POWER:
			{
#if WITHTX
				static uint_fast8_t tx_pwr, tune_pwr;
				unsigned xx = current_place * infobar_label_width + infobar_label_width / 2;

				if (update)
				{
					tx_pwr = hamradio_get_tx_power();
					tune_pwr = hamradio_get_tx_tune_power();
				}

				local_snprintf_P(buf, buflen, PSTR("TX %d\%%"), (int) tx_pwr);
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, str_color);
				local_snprintf_P(buf, buflen, PSTR("Tune %d\%%"), (int) tune_pwr);
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, str_color);
#endif /* WITHTX */
			}
				break;

			case INFOBAR_DNR:
			{
				static int_fast32_t grade = 0;
				static uint_fast8_t state = 0;
				unsigned xx = current_place * infobar_label_width + infobar_label_width / 2;

				if (update)
				{
					state = hamradio_get_nrvalue(& grade);
				}

				local_snprintf_P(buf, buflen, PSTR("DNR"));
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, state ? str_color : COLORPIP_GRAY);
				local_snprintf_P(buf, buflen, state ? "on" : "off");
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, state ? str_color : COLORPIP_GRAY);
			}

				break;

			case INFOBAR_AF:
			// параметры полосы пропускания фильтра
			{
				static uint_fast8_t bp_wide;
				static uint_fast16_t bp_low, bp_high;
				uint_fast16_t xx;

				if (update)
				{
					bp_wide = hamradio_get_bp_type_wide();
					bp_high = hamradio_get_high_bp(0);
					bp_low = hamradio_get_low_bp(0) * 10;
					bp_high = bp_wide ? (bp_high * 100) : (bp_high * 10);
				}
				local_snprintf_P(buf, buflen, PSTR("AF"));
				xx = current_place * infobar_label_width + 7;
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx, y_mid, buf, str_color);
				xx += SMALLCHARW2 * 3;
				local_snprintf_P(buf, buflen, bp_wide ? (PSTR("L %u")) : (PSTR("W %u")), bp_low);
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx, infobar_1st_str_y, buf, str_color);
				local_snprintf_P(buf, buflen, bp_wide ? (PSTR("H %u")) : (PSTR("P %u")), bp_high);
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx, infobar_2nd_str_y, buf, str_color);
			}
				break;

			case INFOBAR_IF_SHIFT:
			// значение сдвига частоты
			{
				static int_fast16_t if_shift;
				uint_fast16_t xx;

				if (update)
					if_shift = hamradio_if_shift(0);
				xx = current_place * infobar_label_width + infobar_label_width / 2;
				if (if_shift)
				{
					local_snprintf_P(buf, buflen, PSTR("IF shift"));
					colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, str_color);
					local_snprintf_P(buf, buflen, if_shift == 0 ? PSTR("%d") : PSTR("%+d Hz"), if_shift);
					colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, str_color);
				}
				else
				{
					local_snprintf_P(buf, buflen, PSTR("IF shift"));
					colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y_mid, buf, COLORPIP_GRAY);
				}
			}
				break;

			case INFOBAR_ATT:
			// attenuator
			{
				static uint8_t atten;
				uint_fast16_t xx;

				if (update)
				{
					atten  = hamradio_get_att_db();
				}
				xx = current_place * infobar_label_width + infobar_label_width / 2;
				local_snprintf_P(buf, buflen, PSTR("ATT"));
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, str_color);
				if (atten)
					local_snprintf_P(buf, buflen, PSTR("%d db"), atten);
				else
					local_snprintf_P(buf, buflen, PSTR("off"));

				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, str_color);
			}
				break;

			case INFOBAR_SPAN:
			// ширина панорамы
			{
	#if WITHIF4DSP
				static int_fast32_t z;
				uint_fast16_t xx;

				if (update)
					z = display_zoomedbw() / 1000;
				local_snprintf_P(buf, buflen, PSTR("SPAN"));
				xx = current_place * infobar_label_width + infobar_label_width / 2;
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, str_color);
				local_snprintf_P(buf, buflen, PSTR("%dk"), z);
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, str_color);
	#endif /* WITHIF4DSP */
			}
				break;

			case INFOBAR_VOLTAGE:
			{
	#if WITHVOLTLEVEL
				// напряжение питания
				static ldiv_t v;
				uint_fast16_t xx;

				if (update)
					v = ldiv(hamradio_get_volt_value(), 10);
				local_snprintf_P(buf, buflen, PSTR("%d.%1dV"), (int) v.quot, (int) v.rem);
				xx = current_place * infobar_label_width + infobar_label_width / 2;
	#if WITHCURRLEVEL || WITHCURRLEVEL2
				uint_fast16_t yy = hamradio_get_tx() ? infobar_1st_str_y : y_mid;
	#else
				uint_fast16_t yy = y_mid;
	#endif /* WITHCURRLEVEL || WITHCURRLEVEL2 */
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, yy, buf, str_color);
	#endif /* WITHVOLTLEVEL */

	#if WITHCURRLEVEL || WITHCURRLEVEL2
			// ток PA (при передаче)
				if (hamradio_get_tx())
				{
					uint_fast16_t xx;
					xx = current_place * infobar_label_width + infobar_label_width / 2;

					static int_fast16_t drain;
					if (update)
					{
						drain = hamradio_get_pacurrent_value();	// Ток в десятках милиампер (может быть отрицательным)
		//				if (drain < 0)
		//				{
		//					drain = 0;	// FIXME: без калибровки нуля (как у нас сейчас) могут быть ошибки установки тока
		//				}
					}

	#if (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A)
					// для больших токов (более 9 ампер)
					const div_t t = div(drain / 10, 10);
					local_snprintf_P(buf, buflen, PSTR("%2d.%01dA"), t.quot, abs(t.rem));

	#else /* (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A) */
					// Датчик тока до 5 ампер
					const div_t t = div(drain, 100);
					local_snprintf_P(buf, buflen, PSTR("%d.%02dA"), t.quot, abs(t.rem));

	#endif /* (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A) */

					colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, str_color);
				}
	#endif /* WITHCURRLEVEL */
			}
				break;

			case INFOBAR_CPU_TEMP:
			{
	#if defined (GET_CPU_TEMPERATURE)
				// вывод температуры процессора, если поддерживается
				static float cpu_temp = 0;
				if (update)
					cpu_temp = GET_CPU_TEMPERATURE();

				unsigned xx = current_place * infobar_label_width + infobar_label_width / 2;
				local_snprintf_P(buf, buflen, PSTR("CPU temp"));
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, COLORPIP_WHITE);
				local_snprintf_P(buf, buflen, PSTR("%2.1f"), cpu_temp);
				colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, cpu_temp > 60.0 ? COLORPIP_RED : COLORPIP_WHITE);
	#endif /* defined (GET_CPU_TEMPERATURE) */
			}
				break;

			case INFOBAR_2ND_ENC_MENU:
			{
				// быстрое меню 2-го энкодера
				hamradio_gui_enc2_update();

				if (gui_enc2_menu.state)
				{
					local_snprintf_P(buf, buflen, PSTR("%s"), gui_enc2_menu.param);
					remove_end_line_spaces(buf);
					unsigned xx = current_place * infobar_label_width + infobar_label_width / 2;
					colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, COLORPIP_WHITE);
					local_snprintf_P(buf, buflen, PSTR("%s"), gui_enc2_menu.val);
					remove_end_line_spaces(buf);
					COLORPIP_T color_lbl = gui_enc2_menu.state == 2 ? COLORPIP_YELLOW : COLORPIP_WHITE;
					colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, color_lbl);
				}
				else
				{
	#if defined (RTC1_TYPE)
					// текущее время
					local_snprintf_P(buf, buflen, PSTR("%02d.%02d"), day, month);
					unsigned xx = current_place * infobar_label_width + infobar_label_width / 2;
					colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, COLORPIP_WHITE);
					local_snprintf_P(buf, buflen, PSTR("%02d%c%02d"), hour, ((seconds & 1) ? ' ' : ':'), minute);
					colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, COLORPIP_WHITE);
	#endif 	/* defined (RTC1_TYPE) */
				}
			}
				break;

			case INFOBAR_EMPTY:
			default:
				break;

			}
		}
	}

#endif /* GUI_SHOW_INFOBAR */

#if 0 //WITHTHERMOLEVEL	// температура выходных транзисторов (при передаче)
		if (hamradio_get_tx())
		{
			const ldiv_t t = ldiv(hamradio_get_PAtemp_value(), 10);
			local_snprintf_P(buf, buflen, PSTR("%d.%dC "), t.quot, t.rem);
			PRINTF("%s\n", buf);		// пока вывод в консоль
		}
#endif /* WITHTHERMOLEVEL */

#if WITHFT8 && ! LINUX_SUBSYSTEM
		ft8_walkthrough_core0(seconds);
#endif /* WITHFT8 && ! LINUX_SUBSYSTEM */

#if WITHGUIDEBUG
	if (tf_debug->visible)
	{
		display_transparency(tf_debug->x1 - 5, tf_debug->y1 - 5, tf_debug->x1 + tf_debug->w + 5, tf_debug->y1 + tf_debug->h + 5, DEFAULT_ALPHA);
	}
#endif /* WITHGUIDEBUG */
}

// *********************************************************************************************************************************************************************

static uint32_t mems[memory_cells_count];

static uint32_t load_mems(uint_fast8_t cell, uint_fast8_t set)
{
#if LINUX_SUBSYSTEM
	if (cell == 0)
		load_memory_cells(mems, memory_cells_count);

	if (set)
		hamradio_set_freq(mems[cell]);

	return mems[cell];
#else
	return hamradio_load_memory_cells(cell, set);
#endif /* LINUX_SUBSYSTEM */
}

static void clean_mems(uint_fast8_t cell)
{
#if LINUX_SUBSYSTEM
	ASSERT(cell < memory_cells_count);
	mems[cell] = 0;
	write_memory_cells(mems, memory_cells_count);
#else
	hamradio_clean_memory_cells(cell);
#endif /* LINUX_SUBSYSTEM */
}

static void write_mems(uint_fast8_t cell)
{
#if LINUX_SUBSYSTEM
	ASSERT(cell < memory_cells_count);
	mems[cell] = hamradio_get_freq_rx();
	write_memory_cells(mems, memory_cells_count);
#else
	hamradio_save_memory_cells(cell);
#endif /* LINUX_SUBSYSTEM */
}

static void window_memory_process(void)
{
	window_t * const win = get_win(WINDOW_MEMORY);

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 6, row_count = 5;
		win->first_call = 0;

		win->bh_count = memory_cells_count;
		unsigned buttons_size = win->bh_count * sizeof (button_t);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);

		add_element("lbl_note1", 0, FONT_MEDIUM, COLORPIP_WHITE, 30);

		x = 0;
		y = 0;
		button_t * bh = NULL;

		for (unsigned i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;
			bh->w = 100;
			bh->h = 44;
			bh->state = CANCELLED;
			bh->parent = WINDOW_MEMORY;
			bh->index = i;
			bh->is_long_press = 1;
			bh->is_repeating = 0;
			bh->is_locked = BUTTON_NON_LOCKED;
			local_snprintf_P(bh->name, ARRAY_SIZE(bh->name), PSTR("btn_memory_%02d"), i);

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = 0;
				y = y + bh->h + interval;
			}

			unsigned freq = load_mems(i, 0);
			if (freq > 0)
			{
				local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("%dk"), freq / 1000);
				bh->payload = 1;
			}
			else
			{
				local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("---"));
				bh->payload = 0;
			}
		}

		ASSERT(bh != NULL);
		label_t * lbl_note1 = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_note1");
		lbl_note1->x = 0;
		lbl_note1->y = bh->y1 + bh->h + get_label_height(lbl_note1);
		lbl_note1->visible = VISIBLE;
		local_snprintf_P(lbl_note1->text, ARRAY_SIZE(lbl_note1->text), PSTR("Long press on empty cell - save,on saved cell - clean"));

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (type == TYPE_BUTTON)
		{
			if (IS_BUTTON_PRESS)
			{
				button_t * bh = (button_t *) ptr;
				unsigned cell_id = bh->index;

				if (bh->payload)
				{
					load_mems(cell_id, 1);
					close_window(DONT_OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
					return;
				}
			}
			else if (IS_BUTTON_LONG_PRESS)
			{
				button_t * bh = (button_t *) ptr;
				unsigned cell_id = bh->index;

				if (bh->payload)
				{
					bh->payload = 0;
					clean_mems(cell_id);
					local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("---"));
				}
				else
				{
					bh->payload = 1;
					unsigned freq = hamradio_get_freq_rx();
					local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("%dk"), freq / 1000);
					write_mems(cell_id);
				}
			}
		}

		break;

	default:

		break;
	}
}

// *********************************************************************************************************************************************************************

static void window_bands_process(void)
{
	window_t * const win = get_win(WINDOW_BANDS);
	static band_array_t * bands = NULL;

	if (win->first_call)
	{
		unsigned x = 0, y = 0, max_x = 0, interval = 6, row_count = 3, i = 0;
		button_t * bh = NULL;
		win->first_call = 0;

		uint_fast8_t bandnum = hamradio_get_bands(NULL, 1, 1);
		bands = (band_array_t *) calloc(bandnum, sizeof (band_array_t));
		GUI_MEM_ASSERT(bands);
		hamradio_get_bands(bands, 0, 1);

		add_element("lbl_ham", 0, FONT_LARGE, COLORPIP_WHITE, 10);
		add_element("lbl_bcast", 0, FONT_LARGE, COLORPIP_WHITE, 16);

		win->bh_count = bandnum + 1;
		unsigned buttons_size = win->bh_count * sizeof (button_t);
		win->bh_ptr = (button_t*) calloc(win->bh_count, sizeof (button_t));
		GUI_MEM_ASSERT(win->bh_ptr);

		label_t * lh1 = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_ham");
		lh1->x = 0;
		lh1->y = 0;
		lh1->visible = VISIBLE;
		local_snprintf_P(lh1->text, ARRAY_SIZE(lh1->text), "HAM bands");

		x = 0;
		y = lh1->y + get_label_height(lh1) * 2;

		for (unsigned r = 1; i < win->bh_count; i ++, r ++)
		{
			bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			bh->w = 86;
			bh->h = 44;
			bh->state = CANCELLED;
			bh->parent = WINDOW_BANDS;

			max_x = (bh->x1 + bh->w > max_x) ? (bh->x1 + bh->w) : max_x;

			if (bands [i].type != BAND_TYPE_HAM)
			{
				// кнопка прямого ввода частоты
				local_snprintf_P(bh->name, ARRAY_SIZE(bh->name), PSTR("btn_freq"));
				local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Freq|enter"));
				i ++;

				break;
			}

			bh->payload = bands [i].init_freq;

			char * div = strchr(bands [i].name, ' ');
			if(div)
				memcpy(div, "|", 1);

			local_snprintf_P(bh->name, ARRAY_SIZE(bh->name), PSTR("btn_ham_%d"), i);
			strcpy(bh->text, bands [i].name);

			if (hamradio_check_current_freq_by_band(bands [i].index))
				bh->is_locked = BUTTON_LOCKED;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = 0;
				y = y + bh->h + interval;
			}
		}

		label_t * lh2 = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_bcast");
		lh2->x = max_x + 50;
		lh2->y = 0;
		lh2->visible = VISIBLE;
		local_snprintf_P(lh2->text, ARRAY_SIZE(lh2->text), "Broadcast bands");

		x = lh2->x;
		y = lh1->y + get_label_height(lh1) * 2;

		for (unsigned r = 1; i < win->bh_count; i ++, r ++)
		{
			bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			bh->w = 86;
			bh->h = 44;
			bh->state = CANCELLED;
			bh->parent = WINDOW_BANDS;
			bh->payload = bands [i - 1].init_freq;
			local_snprintf_P(bh->name, ARRAY_SIZE(bh->name), PSTR("btn_bcast_%d"), i);
			strcpy(bh->text, bands [i - 1].name);

			if (hamradio_check_current_freq_by_band(bands [i - 1].index))
				bh->is_locked = BUTTON_LOCKED;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = lh2->x;
				y = y + bh->h + interval;
			}
		}

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_Freq = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_freq");

			if (bh == btn_Freq)
			{
				window_t * const win = get_win(WINDOW_FREQ);
				open_window(win);
				hamradio_set_lockmode(1);
				hamradio_enable_keyboard_redirect();
			}
			else
			{
				hamradio_goto_band_by_freq(bh->payload);
				close_all_windows();
			}
		}
		break;

	case WM_MESSAGE_CLOSE:

		free(bands);
		break;

	default:

		break;
	}

}

// *********************************************************************************************************************************************************************

static void window_options_process(void)
{
	window_t * const win = get_win(WINDOW_OPTIONS);

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 6, row_count = 4;
		win->first_call = 0;

		add_element("btn_SysMenu", 100, 44, 0, 0, "System|settings");
		add_element("btn_AUDsett", 100, 44, 0, 0, "Audio|settings");
		add_element("btn_TXsett",  100, 44, 0, 0, "Transmit|settings");
		add_element("btn_Display", 100, 44, 0, 0, "Display|settings");
		add_element("btn_gui", 	   100, 44, 0, 0, "GUI|settings");
		add_element("btn_Utils",   100, 44, 0, 0, "Utils");
#if defined (RTC1_TYPE) && ! LINUX_SUBSYSTEM
		add_element("btn_Time",    100, 44, 0, 0, "Set time|& date");
#endif /* defined (RTC1_TYPE) && ! LINUX_SUBSYSTEM */
#if LINUX_SUBSYSTEM
		add_element("btn_exit",		100, 44, 0, 0, "Terminate|program");
#endif /* LINUX_SUBSYSTEM */

		for (unsigned i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = 0;
				y = y + bh->h + interval;
			}
		}

#if ! (WITHSPECTRUMWF && WITHMENU)
		button_t * btn_Display = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_Display");
		btn_Display->state = DISABLED;
#endif

		hamradio_disable_keyboard_redirect();
		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_gui = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_gui");
			button_t * btn_Utils = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_Utils");
			button_t * btn_TXsett = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_TXsett");
			button_t * btn_AUDsett = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_AUDsett");
			button_t * btn_SysMenu = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_SysMenu");
			button_t * btn_Display = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_Display");
#if defined (RTC1_TYPE) && ! LINUX_SUBSYSTEM
			button_t * btn_Time = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_Time");
			if (bh == btn_Time)
			{
				window_t * const win = get_win(WINDOW_TIME);
				open_window(win);
			}
			else
#endif /* defined (RTC1_TYPE) && ! LINUX_SUBSYSTEM */
			if (bh == btn_Utils)
			{
				window_t * const win = get_win(WINDOW_UTILS);
				open_window(win);
			}
			else if (bh == btn_gui)
			{
				window_t * const win = get_win(WINDOW_GUI_SETTINGS);
				open_window(win);
			}
			else if (bh == btn_TXsett)
			{
				window_t * const win = get_win(WINDOW_TX_SETTINGS);
				open_window(win);
			}
			else if (bh == btn_AUDsett)
			{
				window_t * const win = get_win(WINDOW_AUDIOSETTINGS);
				open_window(win);
			}
			else if (bh == btn_SysMenu)
			{
				window_t * const win = get_win(WINDOW_MENU);
				open_window(win);
				hamradio_enable_encoder2_redirect();
			}
#if WITHSPECTRUMWF && WITHMENU
			else if (bh == btn_Display)
			{
				window_t * const win = get_win(WINDOW_DISPLAY);
				open_window(win);
			}
#endif /* WITHSPECTRUMWF && WITHMENU */
#if LINUX_SUBSYSTEM
			else if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_exit"))
			{
				linux_exit();		// Terminate all
			}
#endif /* LINUX_SUBSYSTEM */
		}
		break;

	default:

		break;
	}
}

// *********************************************************************************************************************************************************************

static void window_display_process(void)
{
#if WITHSPECTRUMWF && WITHMENU
	window_t * const win = get_win(WINDOW_DISPLAY);
	static enc_var_t display_t;

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 50;
		win->first_call = 0;
		display_t.change = 0;
		display_t.updated = 1;

		static const button_t buttons [] = {
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_DISPLAY, NON_VISIBLE, INT32_MAX, "btn_zoom", 		"", },
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_DISPLAY, NON_VISIBLE, INT32_MAX, "btn_view", 		"", },
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_DISPLAY, NON_VISIBLE, INT32_MAX, "btn_params", 		"", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_DISPLAY, NON_VISIBLE, -1, 		"btn_topSP_m", 		"-", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_DISPLAY, NON_VISIBLE, 1,  		"btn_topSP_p", 		"+", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_DISPLAY, NON_VISIBLE, -1, 		"btn_bottomSP_m", 	"-", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_DISPLAY, NON_VISIBLE, 1,  		"btn_bottomSP_p", 	"+", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_DISPLAY, NON_VISIBLE, -1, 		"btn_topWF_m", 		"-", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_DISPLAY, NON_VISIBLE, 1,  		"btn_topWF_p", 		"+", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_DISPLAY, NON_VISIBLE, -1, 		"btn_bottomWF_m", 	"-", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_DISPLAY, NON_VISIBLE, 1,  		"btn_bottomWF_p", 	"+", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		static const label_t labels [] = {
			{	WINDOW_DISPLAY, CANCELLED, 0, NON_VISIBLE, "lbl_topSP",    "Top    : xxx db", FONT_MEDIUM, COLORPIP_WHITE, TYPE_DISPLAY_SP_TOP, 	},
			{	WINDOW_DISPLAY, CANCELLED, 0, NON_VISIBLE, "lbl_bottomSP", "Bottom : xxx db", FONT_MEDIUM, COLORPIP_WHITE, TYPE_DISPLAY_SP_BOTTOM, },
			{	WINDOW_DISPLAY, CANCELLED, 0, NON_VISIBLE, "lbl_topWF",    "Top    : xxx db", FONT_MEDIUM, COLORPIP_WHITE, TYPE_DISPLAY_WF_TOP, 	},
			{	WINDOW_DISPLAY, CANCELLED, 0, NON_VISIBLE, "lbl_bottomWF", "Bottom : xxx db", FONT_MEDIUM, COLORPIP_WHITE, TYPE_DISPLAY_WF_BOTTOM, },
			{	WINDOW_DISPLAY, DISABLED,  0, NON_VISIBLE, "lbl_sp",	   "Spectrum", 		  FONT_LARGE,  COLORPIP_WHITE, },
			{	WINDOW_DISPLAY, DISABLED,  0, NON_VISIBLE, "lbl_wf", 	   "Waterfall", 	  FONT_LARGE,  COLORPIP_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t*) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		label_t * lbl_sp = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_sp");
		label_t * lbl_wf = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_wf");
		label_t * lbl_topSP = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_topSP");
		label_t * lbl_bottomSP = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_bottomSP");
		label_t * lbl_topWF = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_topWF");
		label_t * lbl_bottomWF = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_bottomWF");

		button_t * btn_bottomSP_m = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_bottomSP_m");
		button_t * btn_bottomSP_p = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_bottomSP_p");
		button_t * btn_topSP_m = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_topSP_m");
		button_t * btn_topSP_p = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_topSP_p");
		button_t * btn_bottomWF_m = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_bottomWF_m");
		button_t * btn_bottomWF_p = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_bottomWF_p");
		button_t * btn_topWF_m = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_topWF_m");
		button_t * btn_topWF_p = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_topWF_p");

		button_t * btn_zoom = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_zoom");
		button_t * btn_view = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_view");
		button_t * btn_params = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_params");

		lbl_sp->y = 0;
		lbl_sp->visible = VISIBLE;
		lbl_topSP->x = 0;
		lbl_topSP->y = lbl_sp->y + interval;
		lbl_topSP->visible = VISIBLE;
		lbl_bottomSP->x = lbl_topSP->x;
		lbl_bottomSP->y = lbl_topSP->y + interval;
		lbl_bottomSP->visible = VISIBLE;
		btn_topSP_m->x1 = lbl_topSP->x + get_label_width(lbl_topSP) + 10;
		btn_topSP_m->y1 = lbl_topSP->y + get_label_height(lbl_topSP) / 2 - btn_topSP_m->h / 2;
		btn_topSP_m->visible = VISIBLE;
		btn_topSP_p->x1 = btn_topSP_m->x1 + btn_topSP_m->w + 10;
		btn_topSP_p->y1 = btn_topSP_m->y1;
		btn_topSP_p->visible = VISIBLE;
		btn_bottomSP_m->x1 = btn_topSP_m->x1;
		btn_bottomSP_m->y1 = lbl_bottomSP->y + get_label_height(lbl_bottomSP) / 2 - btn_bottomSP_m->h / 2;
		btn_bottomSP_m->visible = VISIBLE;
		btn_bottomSP_p->x1 = btn_bottomSP_m->x1 + btn_bottomSP_m->w + 10;
		btn_bottomSP_p->y1 = btn_bottomSP_m->y1;
		btn_bottomSP_p->visible = VISIBLE;
		lbl_sp->x = (lbl_topSP->x + btn_topSP_p->x1 + btn_topSP_p->w) / 2 - get_label_width(lbl_sp) / 2;

		lbl_topWF->x = btn_topSP_p->x1 + btn_topSP_p->w + interval / 2;
		lbl_topWF->y = lbl_topSP->y;
		lbl_topWF->visible = VISIBLE;
		lbl_bottomWF->x = lbl_topWF->x;
		lbl_bottomWF->y = lbl_topWF->y + interval;
		lbl_bottomWF->visible = VISIBLE;
		btn_topWF_m->x1 = lbl_topWF->x + get_label_width(lbl_topWF) + 10;
		btn_topWF_m->y1 = lbl_topWF->y + get_label_height(lbl_topWF) / 2 - btn_topWF_m->h / 2;
		btn_topWF_m->visible = VISIBLE;
		btn_topWF_p->x1 = btn_topWF_m->x1 + btn_topWF_m->w + 10;
		btn_topWF_p->y1 = btn_topWF_m->y1;
		btn_topWF_p->visible = VISIBLE;
		btn_bottomWF_m->x1 = btn_topWF_m->x1;
		btn_bottomWF_m->y1 = lbl_bottomWF->y + get_label_height(lbl_bottomWF) / 2 - btn_bottomWF_m->h / 2;
		btn_bottomWF_m->visible = VISIBLE;
		btn_bottomWF_p->x1 = btn_bottomWF_m->x1 + btn_bottomWF_m->w + 10;
		btn_bottomWF_p->y1 = btn_bottomWF_m->y1;
		btn_bottomWF_p->visible = VISIBLE;
		lbl_wf->x = (lbl_topWF->x + btn_topWF_p->x1 + btn_topWF_p->w) / 2 - get_label_width(lbl_wf) / 2;
		lbl_wf->y = lbl_sp->y;
		lbl_wf->visible = VISIBLE;

		btn_view->x1 = (lbl_sp->x + lbl_wf->x + get_label_width(lbl_wf)) / 2 - btn_view->w / 2;
		btn_view->y1 = btn_bottomWF_p->y1 + btn_bottomWF_p->h + interval / 2;
		btn_view->visible = VISIBLE;
		btn_params->x1 = btn_view->x1 + btn_view->w + interval / 2;
		btn_params->y1 = btn_view->y1;
		btn_params->visible = VISIBLE;
		btn_zoom->x1 = btn_view->x1 - btn_zoom->w - interval / 2;
		btn_zoom->y1 = btn_view->y1;
		btn_zoom->visible = VISIBLE;

		hamradio_enable_encoder2_redirect();
		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_bottomSP_m = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_bottomSP_m");
			button_t * btn_bottomSP_p = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_bottomSP_p");
			button_t * btn_topSP_m = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_topSP_m");
			button_t * btn_topSP_p = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_topSP_p");
			button_t * btn_bottomWF_m = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_bottomWF_m");
			button_t * btn_bottomWF_p = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_bottomWF_p");
			button_t * btn_topWF_m = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_topWF_m");
			button_t * btn_topWF_p = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_topWF_p");
			button_t * btn_zoom = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_zoom");
			button_t * btn_view = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_view");
			button_t * btn_params = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_params");

			if (bh == btn_view)
			{
				hamradio_change_view_style(1);
				display_t.updated = 1;
			}
			else if (bh == btn_zoom)
			{
				unsigned z = (hamradio_get_gzoomxpow2() + 1) % (BOARD_FFTZOOM_POW2MAX + 1);
				hamradio_set_gzoomxpow2(z);
				display_t.updated = 1;
			}
			else if (bh == btn_params)
			{
				hamradio_set_gwflevelsep(! hamradio_get_gwflevelsep());
				display_t.updated = 1;
			}
			else if (bh == btn_topSP_m || bh == btn_topSP_p)
			{
				hamradio_gtopdbsp(bh->payload);
				display_t.updated = 1;
			}
			else if (bh == btn_bottomSP_m || bh == btn_bottomSP_p)
			{
				hamradio_gbottomdbsp(bh->payload);
				display_t.updated = 1;
			}
			else if (bh == btn_topWF_m || bh == btn_topWF_p)
			{
				hamradio_gtopdbwf(bh->payload);
				display_t.updated = 1;
			}
			else if (bh == btn_bottomWF_m || bh == btn_bottomWF_p)
			{
				hamradio_gbottomdbwf(bh->payload);
				display_t.updated = 1;
			}
		}
		else if (IS_LABEL_PRESS)
		{
			label_t * lh = (label_t *) ptr;

			display_t.select = lh->index;
			display_t.change = 0;
			display_t.updated = 1;
		}

		break;

	case WM_MESSAGE_ENC2_ROTATE:

		display_t.change = action;
		display_t.updated = 1;

		break;

	case WM_MESSAGE_UPDATE:

		display_t.updated = 1;
		display_t.change = 0;
		break;

	default:

		break;
	}

	if (display_t.updated)
	{
		display_t.updated = 0;

		button_t * btn_view = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_view");
		local_snprintf_P(btn_view->text, ARRAY_SIZE(btn_view->text), PSTR("View|%s"), hamradio_change_view_style(0));
		remove_end_line_spaces(btn_view->text);

		button_t * btn_zoom = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_zoom");
		local_snprintf_P(btn_zoom->text, ARRAY_SIZE(btn_zoom->text), PSTR("Zoom|x%d"), 1 << hamradio_get_gzoomxpow2());

		button_t * btn_params = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_params");
		local_snprintf_P(btn_params->text, ARRAY_SIZE(btn_params->text), PSTR("WF params|%s"), hamradio_get_gwflevelsep() ? "separate" : "from SP");

		for(unsigned i = 0; i < win->lh_count; i ++)
			win->lh_ptr [i].color = COLORPIP_WHITE;

		ASSERT(display_t.select < win->lh_count);
		win->lh_ptr [display_t.select].color = COLORPIP_YELLOW;

		label_t * lbl_topSP = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_topSP");
		label_t * lbl_bottomSP = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_bottomSP");
		label_t * lbl_topWF = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_topWF");
		label_t * lbl_bottomWF = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_bottomWF");

		local_snprintf_P(lbl_topSP->text, ARRAY_SIZE(lbl_topSP->text), PSTR("Top    : %3d db"),
				hamradio_gtopdbsp(display_t.select == TYPE_DISPLAY_SP_TOP ? display_t.change : 0));

		local_snprintf_P(lbl_bottomSP->text, ARRAY_SIZE(lbl_bottomSP->text), PSTR("Bottom : %3d db"),
				hamradio_gbottomdbsp(display_t.select == TYPE_DISPLAY_SP_BOTTOM ? display_t.change : 0));

		local_snprintf_P(lbl_topWF->text, ARRAY_SIZE(lbl_topWF->text), PSTR("Top    : %3d db"),
				hamradio_gtopdbwf(display_t.select == TYPE_DISPLAY_WF_TOP ? display_t.change : 0));

		local_snprintf_P(lbl_bottomWF->text, ARRAY_SIZE(lbl_bottomWF->text), PSTR("Bottom : %3d db"),
				hamradio_gbottomdbwf(display_t.select == TYPE_DISPLAY_WF_BOTTOM ? display_t.change : 0));

		display_t.change = 0;
	}
#endif /* WITHSPECTRUMWF && WITHMENU */
}

// *********************************************************************************************************************************************************************

static void window_utilites_process(void)
{
	window_t * const win = get_win(WINDOW_UTILS);

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 6, row_count = 4;
		win->first_call = 0;
#if WITHGUIDEBUG
		add_element("btn_debug", 100, 44, 0, 0, "Debug|view");
#endif /* WITHGUIDEBUG */
#if WITHSWRSCAN
		add_element("btn_SWRscan", 100, 44, 0, 0, "SWR|scanner");
#endif /* WITHSWRSCAN */
		add_element("btn_3d", 100, 44, 0, 0, "Donut|3d");
#if WITHLWIP
		add_element("btn_pingtest", 100, 44, 0, 0, "IP ping|test");
#endif /* WITHLWIP */
#if WITHLFM
		add_element("btn_lfm", 100, 44, 0, 0, "LFM|receive");
#endif /* WITHLFM  */
#if WITHIQSHIFT
		add_element("btn_shift", 100, 44, 0, 0, "IQ shift");
#endif /* WITHIQSHIFT */
#if LINUX_SUBSYSTEM && WITHEXTIO_LAN
		add_element("btn_stream", 100, 44, 0, 0, "IQ LAN|Stream");
#endif /* LINUX_SUBSYSTEM && WITHEXTIO_LAN */

		x = 0;
		y = 0;

		for (unsigned i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = 0;
				y = y + bh->h + interval;
			}
		}

#if WITHGUIDEBUG
		button_t * bh = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_debug");
		bh->is_locked = tf_debug->visible != 0;
#endif /* WITHGUIDEBUG */

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;

			if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_3d"))
			{
				open_window(get_win(WINDOW_3D));
			}
#if WITHSWRSCAN
			else if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_SWRscan"))
			{
				open_window(get_win(WINDOW_SWR_SCANNER));
			}
#endif /* WITHSWRSCAN */
#if WITHLWIP
			else if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_pingtest"))
			{
				open_window(get_win(WINDOW_PING));
			}
#endif /* WITHLWIP */
#if WITHLFM
			else if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_lfm"))
			{
				open_window(get_win(WINDOW_LFM));
			}
#endif /* WITHLFM  */
#if WITHGUIDEBUG
			else if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_debug"))
			{
				gui_open_debug_window();
				close_all_windows();
			}
#endif /* WITHGUIDEBUG */
#if WITHIQSHIFT
			else if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_shift"))
			{
				open_window(get_win(WINDOW_SHIFT));
			}
#endif /* WITHIQSHIFT */
#if LINUX_SUBSYSTEM && WITHEXTIO_LAN
			else if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_stream"))
			{
				open_window(get_win(WINDOW_EXTIOLAN));
			}
#endif /* LINUX_SUBSYSTEM && WITHEXTIO_LAN */
		}
		break;

	default:

		break;
	}
}

// *********************************************************************************************************************************************************************

// переделать полностью
static void window_swrscan_process(void)
{
#if WITHSWRSCAN
	PACKEDCOLORPIP_T * const fr = colmain_fb_draw();
	uint_fast16_t gr_w = 500, gr_h = 250;												// размеры области графика
	uint_fast8_t interval = 20;
	uint_fast16_t x0 = edge_step + interval * 2, y0 = edge_step + gr_h - interval * 2;	// нулевые координаты графика
	uint_fast16_t x1 = x0 + gr_w - interval * 4, y1 = gr_h - y0 + interval * 2;			// размеры осей графика
	static uint_fast16_t mid_w = 0, freq_step = 0;
	static uint_fast16_t i, current_freq_x;
	static uint_fast32_t lim_bottom, lim_top, swr_freq, backup_freq;
	static label_t * lbl_swr_error;
	static button_t * btn_swr_start, * btn_Options, * btn_swr_OK;
	static uint_fast8_t backup_power;
	static uint_fast8_t swr_scan_done = 0, is_swr_scanning = 0;
	static uint_fast8_t swr_scan_enable = 0;		// флаг разрешения сканирования КСВ
	static uint_fast8_t swr_scan_stop = 0;			// флаг нажатия кнопки Stop во время сканирования
	static uint_fast8_t * y_vals;					// массив КСВ в виде отсчетов по оси Y графика
	window_t * const win = get_win(WINDOW_SWR_SCANNER);
	uint_fast8_t averageFactor = 3;

	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		win->first_call = 0;
		button_t * bh = NULL;

		add_element("btn_swr_start", 86, 44, 0, 0, "Start");
		add_element("btn_swr_OK",    86, 44, 0, 0, "OK");

		add_element("lbl_swr_bottom", 0, FONT_SMALL, COLORPIP_WHITE, 16);
		add_element("lbl_swr_top",    0, FONT_SMALL, COLORPIP_WHITE, 16);
		add_element("lbl_swr_error",  0, FONT_MEDIUM, COLORPIP_WHITE, 16);

		mid_w = 0 + gr_w / 2;
		btn_swr_start = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_swr_start");
		btn_swr_start->x1 = mid_w - btn_swr_start->w - interval;
		btn_swr_start->y1 = 0 + gr_h + 0;
		strcpy(btn_swr_start->text, "Start");
		btn_swr_start->visible = VISIBLE;

		btn_swr_OK = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_swr_OK");
		btn_swr_OK->x1 = mid_w + interval;
		btn_swr_OK->y1 = btn_swr_start->y1;
		btn_swr_OK->visible = VISIBLE;

		lbl_swr_error = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_swr_error");
		btn_Options = (button_t*) find_gui_element(TYPE_BUTTON, get_win(WINDOW_MAIN), "btn_Options");

		backup_freq = hamradio_get_freq_rx();
		if (hamradio_verify_freq_bands(backup_freq, & lim_bottom, & lim_top))
		{
			label_t * lbl_swr_bottom = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_swr_bottom");
			local_snprintf_P(lbl_swr_bottom->text, ARRAY_SIZE(lbl_swr_bottom->text), PSTR("%dk"), lim_bottom / 1000);
			lbl_swr_bottom->x = x0 - get_label_width(lbl_swr_bottom) / 2;
			lbl_swr_bottom->y = y0 + get_label_height(lbl_swr_bottom) * 2;
			lbl_swr_bottom->visible = VISIBLE;

			label_t * lbl_swr_top = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_swr_top");
			local_snprintf_P(lbl_swr_top->text, ARRAY_SIZE(lbl_swr_top->text), PSTR("%dk"), lim_top / 1000);
			lbl_swr_top->x = x1 - get_label_width(lbl_swr_bottom) / 2;
			lbl_swr_top->y = lbl_swr_bottom->y;
			lbl_swr_top->visible = VISIBLE;

			btn_swr_start->state = CANCELLED;
			swr_freq = lim_bottom;
			freq_step = (lim_top - lim_bottom) / (x1 - x0);
			current_freq_x = normalize(backup_freq / 1000, lim_bottom / 1000, lim_top / 1000, x1 - x0);
//			backup_power = hamradio_get_tx_power();
		}
		else
		{	// если текущая частота не входит ни в один из любительских диапазонов, вывод сообщения об ошибке
			local_snprintf_P(lbl_swr_error->text, ARRAY_SIZE(lbl_swr_error->text), PSTR("%dk not into HAM bands"), backup_freq / 1000);
			lbl_swr_error->x = mid_w - get_label_width(lbl_swr_error) / 2;
			lbl_swr_error->y = (0 + gr_h) / 2;
			lbl_swr_error->visible = VISIBLE;
			btn_swr_start->state = DISABLED;
		}

		xmax = 0 + gr_w;
		ymax = btn_swr_OK->y1 + btn_swr_OK->h;

		calculate_window_position(win, WINDOW_POSITION_MANUAL_SIZE, xmax, ymax);

		i = 0;
		y_vals = calloc(x1 - x0, sizeof(uint_fast8_t));
		swr_scan_done = 0;
		is_swr_scanning = 0;
		swr_scan_stop = 0;
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_swr_start = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_swr_start");
			button_t * btn_swr_OK = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_swr_OK");

			if (bh == btn_swr_start && ! strcmp(btn_swr_start->text, "Start"))
			{
				swr_scan_enable = 1;
			}
			else if (bh == btn_swr_start && ! strcmp(btn_swr_start->text, "Stop"))
			{
				swr_scan_stop = 1;
			}
			else if (bh == btn_swr_OK)
			{
				close_all_windows();
				free(y_vals);
				return;
			}
		}
		break;

	default:

		break;
	}

	if (swr_scan_enable)						// нажата кнопка Start
	{
		swr_scan_enable = 0;
		strcpy(btn_swr_start->text, "Stop");
		btn_Options->state = DISABLED;
		btn_swr_OK->state = DISABLED;
//		hamradio_set_tx_power(50);
		hamradio_set_tune(1);
		is_swr_scanning = 1;
		i = 0;
		swr_freq = lim_bottom;
		memset(y_vals, 0, x1 - x0);
	}

	if (is_swr_scanning)						// сканирование
	{
		hamradio_set_freq(swr_freq);
		swr_freq += freq_step;
		if (swr_freq >= lim_top || swr_scan_stop)
		{										// нажата кнопка Stop или сканируемая частота выше границы диапазона
			swr_scan_done = 1;
			is_swr_scanning = 0;
			swr_scan_stop = 0;
			strcpy(btn_swr_start->text, "Start");
			btn_Options->state = CANCELLED;
			btn_swr_OK->state = CANCELLED;
			hamradio_set_tune(0);
			hamradio_set_freq(backup_freq);
//			hamradio_set_tx_power(backup_power);
		}

		const uint_fast16_t swr_fullscale = (SWRMIN * 40 / 10) - SWRMIN;	// количество рисок в шкале индикатора
		y_vals [i] = normalize(get_swr(swr_fullscale), 0, swr_fullscale, y0 - y1);
		if (i)
			y_vals [i] = (y_vals [i - 1] * (averageFactor - 1) + y_vals [i]) / averageFactor;
		i++;
	}

	if (! win->first_call)
	{
		// отрисовка фона графика и разметки
		uint_fast16_t gr_x = win->x1 + x0, gr_y = win->y1 + y0;
		colpip_line(fr, DIM_X, DIM_Y, gr_x, gr_y, gr_x, win->y1 + y1, COLORPIP_WHITE, 0);
		colpip_line(fr, DIM_X, DIM_Y, gr_x, gr_y, win->x1 + x1, gr_y, COLORPIP_WHITE, 0);

		char buf [5];
		uint_fast8_t l = 1, row_step = roundf((y0 - y1) / 3);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), l++);
		colpip_string3_tbg(fr, DIM_X, DIM_Y, gr_x - SMALLCHARW3 * 2, gr_y - SMALLCHARH3 / 2, buf, COLORPIP_WHITE);
		for(int_fast16_t yy = y0 - row_step; yy > y1; yy -= row_step)
		{
			if (yy < 0)
				break;

			colpip_line(fr, DIM_X, DIM_Y, gr_x, win->y1 + yy, win->x1 + x1, win->y1 + yy, COLORPIP_DARKGREEN, 0);
			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), l++);
			colpip_string3_tbg(fr, DIM_X, DIM_Y, gr_x - SMALLCHARW3 * 2, win->y1 + yy - SMALLCHARH3 / 2, buf, COLORPIP_WHITE);
		}

		if (lbl_swr_error->visible)				// фон сообщения об ошибке
		{
			colpip_fillrect(fr, DIM_X, DIM_Y, win->x1 + 0, win->y1 + lbl_swr_error->y - 5, gr_w, get_label_height(lbl_swr_error) + 5, COLORPIP_RED);
		}
		else									// маркер текущей частоты
		{
			colpip_line(fr, DIM_X, DIM_Y, gr_x + current_freq_x, gr_y, gr_x + current_freq_x, win->y1 + y1, COLORPIP_RED, 0);
		}

		if (is_swr_scanning || swr_scan_done)	// вывод графика во время сканирования и по завершении
		{
			for(uint_fast16_t j = 2; j <= i; j ++)
				colpip_line(fr, DIM_X, DIM_Y, gr_x + j - 2, gr_y - y_vals [j - 2], gr_x + j - 1, gr_y - y_vals [j - 1], COLORPIP_YELLOW, 1);
		}
	}
#endif /* WITHSWRSCAN */
}

// *********************************************************************************************************************************************************************

static void window_tx_process(void)
{
	window_t * const win = get_win(WINDOW_TX_SETTINGS);
	unsigned update = 0;

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 6, row_count = 3;
		button_t * bh = NULL;
		win->first_call = 0;
		update = 1;

		add_element("btn_tx_vox",          100, 44, 0, 0, "VOX|OFF");
		add_element("btn_tx_vox_settings", 100, 44, 0, 0, "VOX|settings");
		add_element("btn_tx_power",        100, 44, 0, 0, "TX power");

		x = 0;
		y = 0;

		for (unsigned i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = 0;
				y = y + bh->h + interval;
			}
		}

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * const bh = (button_t *) ptr;
			window_t * const winTX = get_win(WINDOW_TX_SETTINGS);
			window_t * const winPower = get_win(WINDOW_TX_POWER);
			window_t * const winVOX = get_win(WINDOW_TX_VOX_SETT);
			button_t * const btn_tx_vox = (button_t*) find_gui_element(TYPE_BUTTON, winTX, "btn_tx_vox");
			button_t * const btn_tx_power = (button_t*) find_gui_element(TYPE_BUTTON, winTX, "btn_tx_power");
			button_t * const btn_tx_vox_settings = (button_t*) find_gui_element(TYPE_BUTTON, winTX, "btn_tx_vox_settings");

#if WITHPOWERTRIM && WITHTX
			if (bh == btn_tx_power)
			{
				open_window(winPower);
				return;
			}
#endif /* WITHPOWERTRIM  && WITHTX*/
#if WITHVOX
			if (bh == btn_tx_vox)
			{
				hamradio_set_gvoxenable(! hamradio_get_gvoxenable());
				update = 1;
			}
			else if (bh == btn_tx_vox_settings)
			{
				open_window(winVOX);
				return;
			}
#endif /* WITHVOX */
		}
		break;

	default:

		break;
	}

	if (update)
	{
#if ! WITHPOWERTRIM
		button_t * btn_tx_power= (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_tx_power");
		btn_tx_power->state = DISABLED;
#endif /* ! WITHPOWERTRIM */

		button_t * btn_tx_vox= (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_tx_vox"); 						// vox on/off
#if WITHVOX
		btn_tx_vox->is_locked = hamradio_get_gvoxenable() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(btn_tx_vox->text, ARRAY_SIZE(btn_tx_vox->text), PSTR("VOX|%s"), btn_tx_vox->is_locked ? "ON" : "OFF");
#else
		btn_tx_vox->state = DISABLED;
		local_snprintf_P(btn_tx_vox->text, ARRAY_SIZE(btn_tx_vox->text), PSTR("VOX"));
#endif /* WITHVOX */

		button_t * btn_tx_vox_settings = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_tx_vox_settings");		// vox settings
#if WITHVOX
		btn_tx_vox_settings->state = hamradio_get_gvoxenable() ? CANCELLED : DISABLED;
#else
		btn_tx_vox_settings->state = DISABLED;
#endif /* WITHVOX */
	}
}

// *********************************************************************************************************************************************************************

static void window_tx_vox_process(void)
{
#if WITHVOX
	window_t * const win = get_win(WINDOW_TX_VOX_SETT);

	static slider_t * sl_vox_delay = NULL, * sl_vox_level = NULL, * sl_avox_level = NULL;
	static label_t * lbl_vox_delay = NULL, * lbl_vox_level = NULL, * lbl_avox_level = NULL;
	static uint_fast8_t delay_min, delay_max, level_min, level_max, alevel_min, alevel_max;
	slider_t * sl;

	if (win->first_call)
	{
		unsigned interval = 50;
		win->first_call = 0;

		static const button_t buttons [] = {
			{  44, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TX_VOX_SETT, NON_VISIBLE, INT32_MAX, "btn_tx_vox_OK", "OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		static const slider_t sliders [] = {
			{ ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, "sl_vox_delay",  CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, "sl_vox_level",  CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, "sl_avox_level", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		};
		win->sh_count = ARRAY_SIZE(sliders);
		unsigned sliders_size = sizeof(sliders);
		win->sh_ptr = (slider_t *) malloc(sliders_size);
		GUI_MEM_ASSERT(win->sh_ptr);
		memcpy(win->sh_ptr, sliders, sliders_size);

		static const label_t labels [] = {
			{	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_vox_delay",    	 "", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_vox_level",    	 "", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_avox_level",   	 "", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_vox_delay_min",  "", FONT_SMALL, COLORPIP_WHITE, },
			{	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_vox_delay_max",  "", FONT_SMALL, COLORPIP_WHITE, },
			{	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_vox_level_min",  "", FONT_SMALL, COLORPIP_WHITE, },
			{	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_vox_level_max",  "", FONT_SMALL, COLORPIP_WHITE, },
			{	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_avox_level_min", "", FONT_SMALL, COLORPIP_WHITE, },
			{	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_avox_level_max", "", FONT_SMALL, COLORPIP_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t*) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		hamradio_get_vox_delay_limits(& delay_min, & delay_max);
		hamradio_get_vox_level_limits(& level_min, & level_max);
		hamradio_get_antivox_delay_limits(& alevel_min, & alevel_max);

		sl_vox_delay = (slider_t *) find_gui_element(TYPE_SLIDER, win, "sl_vox_delay");
		sl_vox_level = (slider_t *) find_gui_element(TYPE_SLIDER, win, "sl_vox_level");
		sl_avox_level = (slider_t *) find_gui_element(TYPE_SLIDER, win, "sl_avox_level");
		lbl_vox_delay = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_vox_delay");
		lbl_vox_level = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_vox_level");
		lbl_avox_level = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_avox_level");

		ldiv_t d = ldiv(hamradio_get_vox_delay(), 100);
		lbl_vox_delay->x = 0;
		lbl_vox_delay->y = 10;
		lbl_vox_delay->visible = VISIBLE;
		local_snprintf_P(lbl_vox_delay->text, ARRAY_SIZE(lbl_vox_delay->text), PSTR("Delay: %d.%d"), (int) d.quot, (int) (d.rem / 10));

		lbl_vox_level->x = lbl_vox_delay->x;
		lbl_vox_level->y = lbl_vox_delay->y + interval;
		lbl_vox_level->visible = VISIBLE;
		local_snprintf_P(lbl_vox_level->text, ARRAY_SIZE(lbl_vox_level->text), PSTR("Level: %3d"), hamradio_get_vox_level());

		lbl_avox_level->x = lbl_vox_level->x;
		lbl_avox_level->y = lbl_vox_level->y + interval;
		lbl_avox_level->visible = VISIBLE;
		local_snprintf_P(lbl_avox_level->text, ARRAY_SIZE(lbl_avox_level->text), PSTR("AVOX : %3d"), hamradio_get_antivox_level());

		sl_vox_delay->x = lbl_vox_delay->x + interval * 2 + interval / 2;
		sl_vox_delay->y = lbl_vox_delay->y;
		sl_vox_delay->visible = VISIBLE;
		sl_vox_delay->size = 300;
		sl_vox_delay->step = 3;
		sl_vox_delay->value = normalize(hamradio_get_vox_delay(), delay_min, delay_max, 100);

		sl_vox_level->x = sl_vox_delay->x;
		sl_vox_level->y = lbl_vox_level->y;
		sl_vox_level->visible = VISIBLE;
		sl_vox_level->size = 300;
		sl_vox_level->step = 3;
		sl_vox_level->value = normalize(hamradio_get_vox_level(), level_min, level_max, 100);

		sl_avox_level->x = sl_vox_delay->x;
		sl_avox_level->y = lbl_avox_level->y;
		sl_avox_level->visible = VISIBLE;
		sl_avox_level->size = 300;
		sl_avox_level->step = 3;
		sl_avox_level->value = normalize(hamradio_get_antivox_level(), alevel_min, alevel_max, 100);

		button_t * bh = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_tx_vox_OK");
		bh->x1 = (sl_vox_delay->x + sl_vox_delay->size) / 2 - (bh->w / 2);
		bh->y1 = lbl_avox_level->y + interval;
		bh->visible = VISIBLE;

		d = ldiv(delay_min, 100);
		label_t * lbl_vox_delay_min = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_vox_delay_min");
		local_snprintf_P(lbl_vox_delay_min->text, ARRAY_SIZE(lbl_vox_delay_min->text), PSTR("%d.%d sec"), (int) d.quot, (int) (d.rem / 10));
		lbl_vox_delay_min->x = sl_vox_delay->x - get_label_width(lbl_vox_delay_min) / 2;
		lbl_vox_delay_min->y = sl_vox_delay->y + get_label_height(lbl_vox_delay_min) * 3;
		lbl_vox_delay_min->visible = VISIBLE;

		d = ldiv(delay_max, 100);
		label_t * lbl_vox_delay_max = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_vox_delay_max");
		local_snprintf_P(lbl_vox_delay_max->text, ARRAY_SIZE(lbl_vox_delay_max->text), PSTR("%d.%d sec"), (int) d.quot, (int) (d.rem / 10));
		lbl_vox_delay_max->x = sl_vox_delay->x + sl_vox_delay->size - get_label_width(lbl_vox_delay_max) / 2;
		lbl_vox_delay_max->y = sl_vox_delay->y + get_label_height(lbl_vox_delay_max) * 3;
		lbl_vox_delay_max->visible = VISIBLE;

		label_t * lbl_vox_level_min = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_vox_level_min");
		local_snprintf_P(lbl_vox_level_min->text, ARRAY_SIZE(lbl_vox_level_min->text), PSTR("%d"), level_min);
		lbl_vox_level_min->x = sl_vox_level->x - get_label_width(lbl_vox_level_min) / 2;
		lbl_vox_level_min->y = sl_vox_level->y + get_label_height(lbl_vox_level_min) * 3;
		lbl_vox_level_min->visible = VISIBLE;

		label_t * lbl_vox_level_max = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_vox_level_max");
		local_snprintf_P(lbl_vox_level_max->text, ARRAY_SIZE(lbl_vox_level_max->text), PSTR("%d"), level_max);
		lbl_vox_level_max->x = sl_vox_level->x + sl_vox_level->size - get_label_width(lbl_vox_level_max) / 2;
		lbl_vox_level_max->y = sl_vox_level->y + get_label_height(lbl_vox_level_max) * 3;
		lbl_vox_level_max->visible = VISIBLE;

		label_t * lbl_avox_level_min = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_avox_level_min");
		local_snprintf_P(lbl_avox_level_min->text, ARRAY_SIZE(lbl_avox_level_min->text), PSTR("%d"), alevel_min);
		lbl_avox_level_min->x = sl_avox_level->x - get_label_width(lbl_avox_level_min) / 2;
		lbl_avox_level_min->y = sl_avox_level->y + get_label_height(lbl_avox_level_min) * 3;
		lbl_avox_level_min->visible = VISIBLE;

		label_t * lbl_avox_level_max = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_avox_level_max");
		local_snprintf_P(lbl_avox_level_max->text, ARRAY_SIZE(lbl_avox_level_max->text), PSTR("%d"), alevel_max);
		lbl_avox_level_max->x = sl_avox_level->x + sl_vox_level->size - get_label_width(lbl_avox_level_max) / 2;
		lbl_avox_level_max->y = sl_avox_level->y + get_label_height(lbl_avox_level_max) * 3;
		lbl_avox_level_max->visible = VISIBLE;

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;

			button_t * btn_tx_vox_OK = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_tx_vox_OK");
			if (bh == btn_tx_vox_OK)
			{
				close_all_windows();
			}
		}
		else if (IS_SLIDER_MOVE)
		{
			slider_t * sl = (slider_t *) ptr;

			if (sl == sl_vox_delay)
			{
				uint_fast16_t delay = delay_min + normalize(sl->value, 0, 100, delay_max - delay_min);
				ldiv_t d = ldiv(delay, 100);
				local_snprintf_P(lbl_vox_delay->text, ARRAY_SIZE(lbl_vox_delay->text), PSTR("Delay: %d.%d"), (int) d.quot, (int) (d.rem / 10));
				hamradio_set_vox_delay(delay);
			}
			else if (sl == sl_vox_level)
			{
				uint_fast16_t level = level_min + normalize(sl->value, 0, 100, level_max - level_min);
				local_snprintf_P(lbl_vox_level->text, ARRAY_SIZE(lbl_vox_level->text), PSTR("Level: %3d"), level);
				hamradio_set_vox_level(level);
			}
			else if (sl == sl_avox_level)
			{
				uint_fast16_t alevel = alevel_min + normalize(sl->value, 0, 100, alevel_max - alevel_min);
				local_snprintf_P(lbl_avox_level->text, ARRAY_SIZE(lbl_avox_level->text), PSTR("AVOX : %3d"), alevel);
				hamradio_set_antivox_level(alevel);
			}
		}
		break;

	default:

		break;
	}
#endif /* WITHVOX */
}

// *********************************************************************************************************************************************************************

static void window_tx_power_process(void)
{
#if WITHPOWERTRIM && WITHTX
	window_t * const win = get_win(WINDOW_TX_POWER);

	static label_t * lbl_tx_power = NULL, * lbl_tune_power = NULL;
	static enc_var_t pw;
	static uint_fast8_t power_min, power_max, power_full, power_tune;
	slider_t * sl;

	if (win->first_call)
	{
		unsigned interval = 50;
		win->first_call = 0;
		pw.change = 0;
		pw.updated = 1;

		static const button_t buttons [] = {
			{  44, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TX_POWER, NON_VISIBLE, INT32_MAX, "btn_tx_pwr_OK", 	   "OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		static const label_t labels [] = {
			{	WINDOW_TX_POWER, CANCELLED, 0, NON_VISIBLE, "lbl_tx_power",   "", FONT_MEDIUM, COLORPIP_WHITE, 0, },
			{	WINDOW_TX_POWER, CANCELLED, 0, NON_VISIBLE, "lbl_tune_power", "", FONT_MEDIUM, COLORPIP_WHITE, 1, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t*) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		lbl_tx_power = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_tx_power");
		lbl_tune_power = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_tune_power");

		hamradio_get_tx_power_limits(& power_min, & power_max);
		power_full = hamradio_get_tx_power();
		power_tune = hamradio_get_tx_tune_power();

		lbl_tx_power->x = 0;
		lbl_tx_power->y = 10;
		lbl_tx_power->visible = VISIBLE;
		local_snprintf_P(lbl_tx_power->text, ARRAY_SIZE(lbl_tx_power->text), PSTR("TX power  : %3d"), power_full);

		lbl_tune_power->x = lbl_tx_power->x;
		lbl_tune_power->y = lbl_tx_power->y + interval;
		lbl_tune_power->visible = VISIBLE;
		local_snprintf_P(lbl_tune_power->text, ARRAY_SIZE(lbl_tune_power->text), PSTR("Tune power: %3d"), power_tune);

		button_t * btn_tx_pwr_OK = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_tx_pwr_OK");
		btn_tx_pwr_OK->x1 = 0;
				btn_tx_pwr_OK->y1 = lbl_tune_power->y + interval;
		btn_tx_pwr_OK->visible = VISIBLE;

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_tx_pwr_OK = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_tx_pwr_OK");

			if (bh == btn_tx_pwr_OK)
			{
				close_all_windows();
			}
		}
		else if (IS_LABEL_PRESS)
		{
			label_t * lh = (label_t *) ptr;
			pw.select = lh->index;
			pw.change = 0;
			pw.updated = 1;
		}
		break;

	case WM_MESSAGE_ENC2_ROTATE:

		pw.change = action;
		pw.updated = 1;
		break;

	case WM_MESSAGE_UPDATE:

		pw.change = 0;
		pw.updated = 1;
		break;

	default:
		break;
	}

	if (pw.updated)
	{
		pw.updated = 0;

		if (pw.change != 0)
		{
			switch (pw.select)
			{
			case 0:
				if (power_full + pw.change <= power_max && power_full + pw.change >= power_min)
					power_full += pw.change;

				local_snprintf_P(lbl_tx_power->text, ARRAY_SIZE(lbl_tx_power->text), PSTR("TX power  : %3d"), power_full);
				hamradio_set_tx_power(power_full);
				save_settings();

				break;

			case 1:
				if (power_tune + pw.change <= power_max && power_tune + pw.change >= power_min)
					power_tune += pw.change;

				local_snprintf_P(lbl_tune_power->text, ARRAY_SIZE(lbl_tune_power->text), PSTR("Tune power: %3d"), power_tune);
				hamradio_set_tx_tune_power(power_tune);
				save_settings();

				break;

			default:
				break;
			}
		}

		for(unsigned i = 0; i < win->lh_count; i ++)
			win->lh_ptr [i].color = COLORPIP_WHITE;

		ASSERT(pw.select < win->lh_count);
		win->lh_ptr [pw.select].color = COLORPIP_YELLOW;
	}

#endif /* WITHPOWERTRIM && WITHTX */
}

// *********************************************************************************************************************************************************************

static void window_audiosettings_process(void)
{
	window_t * const win = get_win(WINDOW_AUDIOSETTINGS);
	unsigned update = 0;

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 6, row_count = 4;
		button_t * bh = NULL;
		win->first_call = 0;
		update = 1;

		static const button_t buttons [] = {
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AUDIOSETTINGS, NON_VISIBLE, INT32_MAX, "btn_reverb", 		 "Reverb|OFF", 		 },
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AUDIOSETTINGS, NON_VISIBLE, INT32_MAX, "btn_mic_eq", 		 "MIC EQ|OFF", 		 },
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AUDIOSETTINGS, NON_VISIBLE, INT32_MAX, "btn_mic_profiles", 	 "MIC|profiles", 	 },
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AUDIOSETTINGS, NON_VISIBLE, INT32_MAX, "btn_monitor", 		 "Monitor|disabled", },
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AUDIOSETTINGS, NON_VISIBLE, INT32_MAX, "btn_reverb_settings", "Reverb|settings",  },
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AUDIOSETTINGS, NON_VISIBLE, INT32_MAX, "btn_mic_eq_settings", "MIC EQ|settings",  },
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AUDIOSETTINGS, NON_VISIBLE, INT32_MAX, "btn_mic_settings", 	 "MIC|settings", 	 },

		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = 0;
		y = 0;

		for (unsigned i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = 0;
				y = y + bh->h + interval;
			}
		}

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * const bh = (button_t *) ptr;
			window_t * const winAP = get_win(WINDOW_AUDIOSETTINGS);
			window_t * const winEQ = get_win(WINDOW_AP_MIC_EQ);
			window_t * const winRS = get_win(WINDOW_AP_REVERB_SETT);
			window_t * const winMIC = get_win(WINDOW_AP_MIC_SETT);
			window_t * const winMICpr = get_win(WINDOW_AP_MIC_PROF);
			button_t * const btn_reverb = (button_t*) find_gui_element(TYPE_BUTTON, winAP, "btn_reverb");						// reverb on/off
			button_t * const btn_reverb_settings = (button_t*) find_gui_element(TYPE_BUTTON, winAP, "btn_reverb_settings");		// reverb settings
			button_t * const btn_monitor = (button_t*) find_gui_element(TYPE_BUTTON, winAP, "btn_monitor");						// monitor on/off
			button_t * const btn_mic_eq = (button_t*) find_gui_element(TYPE_BUTTON, winAP, "btn_mic_eq");						// MIC EQ on/off
			button_t * const btn_mic_eq_settings = (button_t*) find_gui_element(TYPE_BUTTON, winAP, "btn_mic_eq_settings");		// MIC EQ settingss
			button_t * const btn_mic_settings = (button_t*) find_gui_element(TYPE_BUTTON, winAP, "btn_mic_settings");			// mic settings
			button_t * const btn_mic_profiles = (button_t*) find_gui_element(TYPE_BUTTON, winAP, "btn_mic_profiles");			// mic profiles

			if (bh == btn_monitor)
			{
				hamradio_set_gmoniflag(! hamradio_get_gmoniflag());
				update = 1;
			}
#if WITHREVERB
			else if (bh == btn_reverb_settings)
			{
				open_window(winRS);
			}
#endif /* WITHREVERB */
#if WITHAFCODEC1HAVEPROC
			else if (bh == btn_mic_eq)
			{
				hamradio_set_gmikeequalizer(! hamradio_get_gmikeequalizer());
				update = 1;
			}
			else if (bh == btn_mic_eq_settings)
			{
				open_window(winEQ);
				return;
			}
			else if (bh == btn_mic_settings)
			{
				open_window(winMIC);
				return;
			}
			else if (bh == btn_mic_profiles)
			{
				open_window(winMICpr);
				return;
			}
#endif /* WITHAFCODEC1HAVEPROC */
#if WITHREVERB
			else if (bh == btn_reverb)
			{
				hamradio_set_greverb(! hamradio_get_greverb());
				update = 1;
			}
#endif /* WITHREVERB */
		}
		break;

	default:

		break;
	}

	if (update)
	{
		button_t * bh = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_reverb"); 			// reverb on/off
#if WITHREVERB
		bh->is_locked = hamradio_get_greverb() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Reverb|%s"), hamradio_get_greverb() ? "ON" : "OFF");
#else
		bh->state = DISABLED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Reverb|OFF"));
#endif /* WITHREVERB */

		bh = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_reverb_settings");				// reverb settings
#if WITHREVERB
		bh->state = hamradio_get_greverb() ? CANCELLED : DISABLED;
#else
		bh->state = DISABLED;
#endif /* WITHREVERB */

		bh = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_monitor");						// monitor on/off
		bh->is_locked = hamradio_get_gmoniflag() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Monitor|%s"), bh->is_locked ? "enabled" : "disabled");

		bh = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_mic_eq");						// MIC EQ on/off
#if WITHAFCODEC1HAVEPROC
		bh->is_locked = hamradio_get_gmikeequalizer() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("MIC EQ|%s"), bh->is_locked ? "ON" : "OFF");
#else
		bh->state = DISABLED;
#endif /* WITHAFCODEC1HAVEPROC */

		bh = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_mic_eq_settings");				// MIC EQ settings
#if WITHAFCODEC1HAVEPROC
		bh->state = hamradio_get_gmikeequalizer() ? CANCELLED : DISABLED;
#else
		bh->state = DISABLED;
#endif /* WITHAFCODEC1HAVEPROC */

		bh = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_mic_profiles");				// MIC profiles
#if ! WITHAFCODEC1HAVEPROC
		bh->state = DISABLED;
#endif /* ! WITHAFCODEC1HAVEPROC */

		bh = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_mic_settings");				// MIC settings
#if ! WITHAFCODEC1HAVEPROC
		bh->state = DISABLED;
#endif /* ! WITHAFCODEC1HAVEPROC */
	}
}

// *********************************************************************************************************************************************************************

static void window_ap_reverb_process(void)
{
#if WITHREVERB
	window_t * const win = get_win(WINDOW_AP_REVERB_SETT);

	static label_t * lbl_reverbDelay = NULL, * lbl_reverbLoss = NULL;
	static slider_t * sl_reverbDelay = NULL, * sl_reverbLoss = NULL;
	static uint_fast8_t delay_min, delay_max, loss_min, loss_max;

	if (win->first_call)
	{
		uint_fast8_t interval = 60;
		win->first_call = 0;

		static const button_t buttons [] = {
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AP_REVERB_SETT,	NON_VISIBLE, INT32_MAX, "btn_REVs_ok", "OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		static const label_t labels [] = {
			{	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbDelay",		"", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbLoss", 		"", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbDelay_min", 	"", FONT_SMALL, COLORPIP_WHITE, },
			{	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbDelay_max", 	"", FONT_SMALL, COLORPIP_WHITE, },
			{	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbLoss_min", 	"", FONT_SMALL, COLORPIP_WHITE, },
			{	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbLoss_max", 	"", FONT_SMALL, COLORPIP_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t*) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		slider_t sliders [] = {
			{ ORIENTATION_HORIZONTAL, WINDOW_AP_REVERB_SETT, 	"reverbDelay", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ ORIENTATION_HORIZONTAL, WINDOW_AP_REVERB_SETT, 	"reverbLoss",  CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		};
		win->sh_count = ARRAY_SIZE(sliders);
		unsigned sliders_size = sizeof(sliders);
		win->sh_ptr = (slider_t *) malloc(sliders_size);
		GUI_MEM_ASSERT(win->sh_ptr);
		memcpy(win->sh_ptr, sliders, sliders_size);

		label_t * lbl_reverbDelay_min = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_reverbDelay_min");
		label_t * lbl_reverbDelay_max = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_reverbDelay_max");
		label_t * lbl_reverbLoss_min = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_reverbLoss_min");
		label_t * lbl_reverbLoss_max = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_reverbLoss_max");
		lbl_reverbDelay = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_reverbDelay");
		lbl_reverbLoss = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_reverbLoss");
		sl_reverbDelay = (slider_t *) find_gui_element(TYPE_SLIDER, win, "reverbDelay");
		sl_reverbLoss = (slider_t *) find_gui_element(TYPE_SLIDER, win, "reverbLoss");

		hamradio_get_reverb_delay_limits(& delay_min, & delay_max);
		hamradio_get_reverb_loss_limits(& loss_min, & loss_max);

		lbl_reverbDelay->x = 0;
		lbl_reverbDelay->y = 10;
		lbl_reverbDelay->visible = VISIBLE;
		local_snprintf_P(lbl_reverbDelay->text, ARRAY_SIZE(lbl_reverbDelay->text), PSTR("Delay: %3d ms"), hamradio_get_reverb_delay());

		lbl_reverbLoss->x = lbl_reverbDelay->x;
		lbl_reverbLoss->y = lbl_reverbDelay->y + interval;
		lbl_reverbLoss->visible = VISIBLE;
		local_snprintf_P(lbl_reverbLoss->text, ARRAY_SIZE(lbl_reverbLoss->text), PSTR("Loss :  %2d dB"), hamradio_get_reverb_loss());

		sl_reverbDelay->x = lbl_reverbDelay->x + interval * 3;
		sl_reverbDelay->y = lbl_reverbDelay->y;
		sl_reverbDelay->visible = VISIBLE;
		sl_reverbDelay->size = 300;
		sl_reverbDelay->step = 3;
		sl_reverbDelay->value = normalize(hamradio_get_reverb_delay(), delay_min, delay_max, 100);

		local_snprintf_P(lbl_reverbDelay_min->text, ARRAY_SIZE(lbl_reverbDelay_min->text), PSTR("%d ms"), delay_min);
		lbl_reverbDelay_min->x = sl_reverbDelay->x - get_label_width(lbl_reverbDelay_min) / 2;
		lbl_reverbDelay_min->y = sl_reverbDelay->y + get_label_height(lbl_reverbDelay_min) * 3;
		lbl_reverbDelay_min->visible = VISIBLE;

		local_snprintf_P(lbl_reverbDelay_max->text, ARRAY_SIZE(lbl_reverbDelay_max->text), PSTR("%d ms"), delay_max);
		lbl_reverbDelay_max->x = sl_reverbDelay->x + sl_reverbDelay->size - get_label_width(lbl_reverbDelay_max) / 2;
		lbl_reverbDelay_max->y = sl_reverbDelay->y + get_label_height(lbl_reverbDelay_max) * 3;
		lbl_reverbDelay_max->visible = VISIBLE;

		sl_reverbLoss->x = lbl_reverbLoss->x + interval * 3;
		sl_reverbLoss->y = lbl_reverbLoss->y;
		sl_reverbLoss->visible = VISIBLE;
		sl_reverbLoss->size = 300;
		sl_reverbLoss->step = 3;
		sl_reverbLoss->value = normalize(hamradio_get_reverb_loss(), loss_min, loss_max, 100);

		local_snprintf_P(lbl_reverbLoss_min->text, ARRAY_SIZE(lbl_reverbLoss_min->text), PSTR("%d dB"), loss_min);
		lbl_reverbLoss_min->x = sl_reverbLoss->x - get_label_width(lbl_reverbLoss_min) / 2;
		lbl_reverbLoss_min->y = sl_reverbLoss->y + get_label_height(lbl_reverbLoss_min) * 3;
		lbl_reverbLoss_min->visible = VISIBLE;

		local_snprintf_P(lbl_reverbLoss_max->text, ARRAY_SIZE(lbl_reverbLoss_max->text), PSTR("%d dB"), loss_max);
		lbl_reverbLoss_max->x = sl_reverbLoss->x + sl_reverbLoss->size - get_label_width(lbl_reverbLoss_max) / 2;
		lbl_reverbLoss_max->y = sl_reverbLoss->y + get_label_height(lbl_reverbLoss_max) * 3;
		lbl_reverbLoss_max->visible = VISIBLE;

		button_t * bh = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_REVs_ok");
		bh->x1 = sl_reverbLoss->x + sl_reverbLoss->size + interval / 2;
		bh->y1 = lbl_reverbLoss->y;
		bh->visible = VISIBLE;

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			close_window(OPEN_PARENT_WINDOW); // единственная кнопка
			return;
		}
		else if (IS_SLIDER_MOVE)
		{
			slider_t * sh = (slider_t *) ptr;

			if (sh == sl_reverbDelay)
			{
				uint_fast16_t delay = delay_min + normalize(sl_reverbDelay->value, 0, 100, delay_max - delay_min);
				local_snprintf_P(lbl_reverbDelay->text, ARRAY_SIZE(lbl_reverbDelay->text), PSTR("Delay: %3d ms"), delay);
				hamradio_set_reverb_delay(delay);
			}
			else if (sh == sl_reverbLoss)
			{
				uint_fast16_t loss = loss_min + normalize(sl_reverbLoss->value, 0, 100, loss_max - loss_min);
				local_snprintf_P(lbl_reverbLoss->text, ARRAY_SIZE(lbl_reverbLoss->text), PSTR("Loss :  %2d dB"), loss);
				hamradio_set_reverb_loss(loss);
			}
		}
		break;

	default:

		break;
	}
#endif /* WITHREVERB */
}

// *********************************************************************************************************************************************************************

static void window_ap_mic_eq_process(void)
{
#if WITHAFCODEC1HAVEPROC
	PACKEDCOLORPIP_T * const fr = colmain_fb_draw();
	window_t * const win = get_win(WINDOW_AP_MIC_EQ);

	label_t * lbl = NULL;
	static unsigned eq_limit, eq_base = 0, id = 0;
	char buf [TEXT_ARRAY_SIZE];
	static int mid_y = 0;
	static button_t * btn_EQ_ok;

	if (win->first_call)
	{
		unsigned x, y, mid_w, interval = 70;
		win->first_call = 0;

		static const button_t buttons [] = {
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AP_MIC_EQ, 	NON_VISIBLE, INT32_MAX, "btn_EQ_ok", "OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		static const label_t labels [] = {
			{	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq0.08_val", 	"", FONT_LARGE, COLORPIP_YELLOW, },
			{	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq0.23_val", 	"", FONT_LARGE, COLORPIP_YELLOW, },
			{	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq0.65_val",  "", FONT_LARGE, COLORPIP_YELLOW, },
			{	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq1.8_val",   "", FONT_LARGE, COLORPIP_YELLOW, },
			{	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq5.3_val",   "", FONT_LARGE, COLORPIP_YELLOW, },
			{	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq0.08_name", "", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq0.23_name", "", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq0.65_name", "", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq1.8_name",  "", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq5.3_name",  "", FONT_MEDIUM, COLORPIP_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t*) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		static const slider_t sliders [] = {
			{ ORIENTATION_VERTICAL, WINDOW_AP_MIC_EQ, "eq0.08", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, 0, },
			{ ORIENTATION_VERTICAL, WINDOW_AP_MIC_EQ, "eq0.23", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, 1, },
			{ ORIENTATION_VERTICAL, WINDOW_AP_MIC_EQ, "eq0.65", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, 2, },
			{ ORIENTATION_VERTICAL, WINDOW_AP_MIC_EQ, "eq1.8",  CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, 3, },
			{ ORIENTATION_VERTICAL, WINDOW_AP_MIC_EQ, "eq5.3",  CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, 4, },
		};
		win->sh_count = ARRAY_SIZE(sliders);
		unsigned sliders_size = sizeof(sliders);
		win->sh_ptr = (slider_t *) malloc(sliders_size);
		GUI_MEM_ASSERT(win->sh_ptr);
		memcpy(win->sh_ptr, sliders, sliders_size);

		eq_base = hamradio_getequalizerbase();
		eq_limit = abs(eq_base) * 2;

		x = 50;
		y = 0;
		slider_t * sl = NULL;

		for (id = 0; id < win->sh_count; id++)
		{
			sl = & win->sh_ptr [id];

			sl->x = x;
			sl->size = 200;
			sl->step = 2;
			sl->value = normalize(hamradio_get_gmikeequalizerparams(id), eq_limit, 0, 100);
			sl->visible = VISIBLE;

			mid_w = sl->x + sliders_width / 2;		// центр шкалы слайдера по x

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_name"), sl->name);
			lbl = (label_t*) find_gui_element(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%sk"), strchr(sl->name, 'q') + 1);
			lbl->x = mid_w - get_label_width(lbl) / 2;
			lbl->y = y;
			lbl->visible = VISIBLE;

			y = lbl->y + get_label_height(lbl) * 2;

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_val"), sl->name);
			lbl = (label_t*) find_gui_element(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%d"), hamradio_get_gmikeequalizerparams(id) + eq_base);
			lbl->x = mid_w - get_label_width(lbl) / 2;
			lbl->y = y;
			lbl->visible = VISIBLE;

			sl->y = lbl->y + get_label_height(lbl) * 2 + 10;

			x = x + interval;
			y = 0;
		}

		btn_EQ_ok = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_EQ_ok");
		btn_EQ_ok->x1 = sl->x + sliders_width + btn_EQ_ok->w;
		btn_EQ_ok->y1 = sl->y + sl->size - btn_EQ_ok->h;
		btn_EQ_ok->visible = VISIBLE;

		calculate_window_position(win, WINDOW_POSITION_AUTO);
		mid_y = win->y1 + sl->y + sl->size / 2;						//todo: абсолютные координаты! переделать
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			close_all_windows();
			return;
		}
		else if (IS_SLIDER_MOVE)
		{
			slider_t * sh = (slider_t *) ptr;
			unsigned id = sh->index;

			hamradio_set_gmikeequalizerparams(id, normalize(sh->value, 100, 0, eq_limit));

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_val"), sh->name);
			lbl = (label_t*) find_gui_element(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%d"), hamradio_get_gmikeequalizerparams(id) + eq_base);
			lbl->x = sh->x + sliders_width / 2 - get_label_width(lbl) / 2;
		}
		break;

	default:

		break;
	}

	// разметка шкал
	for (unsigned i = 0; i <= abs(eq_base); i += 3)
	{
		uint_fast16_t yy = normalize(i, 0, abs(eq_base), 100);
		colpip_line(fr, DIM_X, DIM_Y, win->x1 + 50, mid_y + yy, win->x1 + win->w - (btn_EQ_ok->w << 1), mid_y + yy, GUI_SLIDERLAYOUTCOLOR, 0);
		local_snprintf_P(buf, ARRAY_SIZE(buf), i == 0 ? PSTR("%d") : PSTR("-%d"), i);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, win->x1 + 50 - strwidth2(buf) - 5, mid_y + yy - SMALLCHARH2 / 2, buf, COLORPIP_WHITE);

		if (i == 0)
			continue;
		colpip_line(fr, DIM_X, DIM_Y, win->x1 + 50, mid_y - yy, win->x1 + win->w - (btn_EQ_ok->w << 1), mid_y - yy, GUI_SLIDERLAYOUTCOLOR, 0);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), i);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, win->x1 + 50 - strwidth2(buf) - 5, mid_y - yy - SMALLCHARH2 / 2, buf, COLORPIP_WHITE);
	}
#endif /* WITHAFCODEC1HAVEPROC */
}

// *********************************************************************************************************************************************************************

static void window_af_eq_process(void)
{
#if WITHAFEQUALIZER
	window_t * const win = get_win(WINDOW_AF_EQ);

	label_t * lbl = NULL;
	static uint_fast32_t eq_limit, eq_base = 0;
	char buf [TEXT_ARRAY_SIZE];
	static int_fast16_t mid_y = 0;
	static uint_fast32_t id = 0, eq_w = 0;

	if (win->first_call)
	{
		uint_fast16_t x, y, mid_w;
		uint_fast8_t interval = 70;
		win->first_call = 0;

		static const button_t buttons [] = {
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AF_EQ, NON_VISIBLE, INT32_MAX, "btn_EQ_ok", "OK", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AF_EQ, NON_VISIBLE, INT32_MAX, "btn_EQ_enable", "", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		static const label_t labels [] = {
			{	WINDOW_AF_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq400_val", 	"", FONT_LARGE, COLORPIP_YELLOW, },
			{	WINDOW_AF_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq1500_val", 	"", FONT_LARGE, COLORPIP_YELLOW, },
			{	WINDOW_AF_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq2700_val",  "", FONT_LARGE, COLORPIP_YELLOW, },
			{	WINDOW_AF_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq400_name",  "", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_AF_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq1500_name", "", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_AF_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq2700_name", "", FONT_MEDIUM, COLORPIP_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t*) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		static const slider_t sliders [] = {
			{ ORIENTATION_VERTICAL, WINDOW_AF_EQ, "eq400", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, 0, },
			{ ORIENTATION_VERTICAL, WINDOW_AF_EQ, "eq1500", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, 1, },
			{ ORIENTATION_VERTICAL, WINDOW_AF_EQ, "eq2700", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, 2, },
		};
		win->sh_count = ARRAY_SIZE(sliders);
		unsigned sliders_size = sizeof(sliders);
		win->sh_ptr = (slider_t *) malloc(sliders_size);
		GUI_MEM_ASSERT(win->sh_ptr);
		memcpy(win->sh_ptr, sliders, sliders_size);

		eq_base = hamradio_get_af_equalizer_base();
		eq_limit = abs(eq_base) * 2;

		x = 50;
		y = 0;
		slider_t * sl = NULL;

		for (id = 0; id < win->sh_count; id++)
		{
			sl = & win->sh_ptr [id];

			sl->x = x;
			sl->size = 200;
			sl->step = 2;
			sl->value = normalize(hamradio_get_af_equalizer_gain_rx(id), eq_limit, 0, 100);
			sl->visible = VISIBLE;

			mid_w = sl->x + sliders_width / 2;		// центр шкалы слайдера по x

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_name"), sl->name);
			lbl = (label_t*) find_gui_element(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%s"), strchr(sl->name, 'q') + 1);
			lbl->x = mid_w - get_label_width(lbl) / 2;
			lbl->y = y;
			lbl->visible = VISIBLE;

			y = lbl->y + get_label_height(lbl) * 2;

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_val"), sl->name);
			lbl = (label_t*) find_gui_element(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%d"), hamradio_get_af_equalizer_gain_rx(id) + eq_base);
			lbl->x = mid_w - get_label_width(lbl) / 2;
			lbl->y = y;
			lbl->visible = VISIBLE;

			sl->y = lbl->y + get_label_height(lbl) * 2 + 10;

			x = x + interval;
			y = 0;
		}

		button_t * btn_EQ_ok = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_EQ_ok");
		btn_EQ_ok->x1 = sl->x + sliders_width + btn_EQ_ok->w;
		btn_EQ_ok->y1 = sl->y + sl->size - btn_EQ_ok->h;
		btn_EQ_ok->visible = VISIBLE;

		button_t * btn_EQ_enable = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_EQ_enable");
		btn_EQ_enable->x1 = btn_EQ_ok->x1;
		btn_EQ_enable->y1 = btn_EQ_ok->y1 - btn_EQ_enable->h * 2;
		btn_EQ_enable->visible = VISIBLE;
		btn_EQ_enable->is_locked = hamradio_get_geqrx();
		local_snprintf_P(btn_EQ_enable->text, ARRAY_SIZE(btn_EQ_enable->text), PSTR("%s"), btn_EQ_enable->is_locked ? "En" : "Dis");

		eq_w = btn_EQ_ok->w << 1;
		mid_y = sl->y + sl->size / 2;

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_EQ_ok = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_EQ_ok");
			button_t * btn_EQ_enable = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_EQ_enable");

			if (bh == btn_EQ_ok)
			{
				close_all_windows();
				return;
			}

			if (bh == btn_EQ_enable)
			{
				hamradio_set_geqrx(! hamradio_get_geqrx());
				btn_EQ_enable->is_locked = hamradio_get_geqrx();
				local_snprintf_P(btn_EQ_enable->text, ARRAY_SIZE(btn_EQ_enable->text), PSTR("%s"), btn_EQ_enable->is_locked ? "En" : "Dis");
			}
		}

		if (IS_SLIDER_MOVE)
		{
			slider_t * sh = (slider_t *) ptr;
			uint_fast8_t id = sh->index;

			hamradio_set_af_equalizer_gain_rx(id, normalize(sh->value, 100, 0, eq_limit));

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_val"), sh->name);
			lbl = (label_t*) find_gui_element(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%d"), hamradio_get_af_equalizer_gain_rx(id) + eq_base);
			lbl->x = sh->x + sliders_width / 2 - get_label_width(lbl) / 2;
		}
		break;

	default:

		break;
	}

	// разметка шкал
	for (unsigned i = 0; i <= abs(eq_base); i += 3)
	{
		uint_fast16_t yy = normalize(i, 0, abs(eq_base), 100);
		gui_drawline(30, mid_y + yy, win->w - eq_w, mid_y + yy, GUI_SLIDERLAYOUTCOLOR);
		local_snprintf_P(buf, ARRAY_SIZE(buf), i == 0 ? PSTR("%d") : PSTR("-%d"), i);
		gui_drawstring(30 - strwidth2(buf) - 5, mid_y + yy - SMALLCHARH2 / 2, buf, FONT_MEDIUM, COLORPIP_WHITE);

		if (i == 0)
			continue;

		gui_drawline(30, mid_y - yy, win->w - eq_w, mid_y - yy, GUI_SLIDERLAYOUTCOLOR);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), i);
		gui_drawstring(30 - strwidth2(buf) - 5, mid_y - yy - SMALLCHARH2 / 2, buf, FONT_MEDIUM, COLORPIP_WHITE);
	}
#endif /* WITHAFEQUALIZER */
}

// *********************************************************************************************************************************************************************

static void window_ap_mic_process(void)
{
#if WITHAFCODEC1HAVEPROC
	window_t * const win = get_win(WINDOW_AP_MIC_SETT);

	static slider_t * sl_micLevel = NULL, * sl_micClip = NULL, * sl_micAGC = NULL;
	static label_t * lbl_micLevel = NULL, * lbl_micClip = NULL, * lbl_micAGC = NULL;
	static uint_fast16_t level_min, level_max, clip_min, clip_max, agc_min, agc_max;

	if (win->first_call)
	{
		uint_fast8_t interval = 50;
		win->first_call = 0;

		static const button_t buttons [] = {
			{  86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AP_MIC_SETT, NON_VISIBLE, INT32_MAX, "btn_mic_agc",   "AGC|OFF",   },
			{  86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AP_MIC_SETT, NON_VISIBLE, INT32_MAX, "btn_mic_boost", "Boost|OFF", },
			{  86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AP_MIC_SETT, NON_VISIBLE, INT32_MAX, "btn_mic_OK",    "OK", 		  },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		static const label_t labels [] = {
			{	WINDOW_AP_MIC_SETT, DISABLED, 0, NON_VISIBLE, "lbl_micLevel", 	  "", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_AP_MIC_SETT, DISABLED, 0, NON_VISIBLE, "lbl_micClip",  	  "", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_AP_MIC_SETT, DISABLED, 0, NON_VISIBLE, "lbl_micAGC",   	  "", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_AP_MIC_SETT, DISABLED, 0, NON_VISIBLE, "lbl_micLevel_min", "", FONT_SMALL,  COLORPIP_WHITE, },
			{	WINDOW_AP_MIC_SETT, DISABLED, 0, NON_VISIBLE, "lbl_micLevel_max", "", FONT_SMALL,  COLORPIP_WHITE, },
			{	WINDOW_AP_MIC_SETT, DISABLED, 0, NON_VISIBLE, "lbl_micClip_min",  "", FONT_SMALL,  COLORPIP_WHITE, },
			{	WINDOW_AP_MIC_SETT, DISABLED, 0, NON_VISIBLE, "lbl_micClip_max",  "", FONT_SMALL,  COLORPIP_WHITE, },
			{	WINDOW_AP_MIC_SETT, DISABLED, 0, NON_VISIBLE, "lbl_micAGC_min",   "", FONT_SMALL,  COLORPIP_WHITE, },
			{	WINDOW_AP_MIC_SETT, DISABLED, 0, NON_VISIBLE, "lbl_micAGC_max",   "", FONT_SMALL,  COLORPIP_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t*) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		static const slider_t sliders [] = {
			{ ORIENTATION_HORIZONTAL, WINDOW_AP_MIC_SETT, "sl_micLevel", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ ORIENTATION_HORIZONTAL, WINDOW_AP_MIC_SETT, "sl_micClip",  CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ ORIENTATION_HORIZONTAL, WINDOW_AP_MIC_SETT, "sl_micAGC",   CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		};
		win->sh_count = ARRAY_SIZE(sliders);
		unsigned sliders_size = sizeof(sliders);
		win->sh_ptr = (slider_t *) malloc(sliders_size);
		GUI_MEM_ASSERT(win->sh_ptr);
		memcpy(win->sh_ptr, sliders, sliders_size);

		hamradio_get_mic_level_limits(& level_min, & level_max);
		hamradio_get_mic_clip_limits(& clip_min, & clip_max);
		hamradio_get_mic_agc_limits(& agc_min, & agc_max);

		sl_micLevel = (slider_t *) find_gui_element(TYPE_SLIDER, win, "sl_micLevel");
		sl_micClip = (slider_t *) find_gui_element(TYPE_SLIDER, win, "sl_micClip");
		sl_micAGC = (slider_t *) find_gui_element(TYPE_SLIDER, win, "sl_micAGC");
		lbl_micLevel = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_micLevel");
		lbl_micClip = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_micClip");
		lbl_micAGC = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_micAGC");

		lbl_micLevel->x = 0;
		lbl_micLevel->y = 10;
		lbl_micLevel->visible = VISIBLE;
		local_snprintf_P(lbl_micLevel->text, ARRAY_SIZE(lbl_micLevel->text), PSTR("Level: %3d"), hamradio_get_mik1level());

		lbl_micClip->x = lbl_micLevel->x;
		lbl_micClip->y = lbl_micLevel->y + interval;
		lbl_micClip->visible = VISIBLE;
		local_snprintf_P(lbl_micClip->text, ARRAY_SIZE(lbl_micClip->text), PSTR("Clip : %3d"), hamradio_get_gmikehclip());

		lbl_micAGC->x = lbl_micClip->x;
		lbl_micAGC->y = lbl_micClip->y + interval;
		lbl_micAGC->visible = VISIBLE;
		local_snprintf_P(lbl_micAGC->text, ARRAY_SIZE(lbl_micAGC->text), PSTR("AGC  : %3d"), hamradio_get_gmikeagcgain());

		sl_micLevel->x = lbl_micLevel->x + interval * 2 + interval / 2;
		sl_micLevel->y = lbl_micLevel->y;
		sl_micLevel->visible = VISIBLE;
		sl_micLevel->size = 300;
		sl_micLevel->step = 3;
		sl_micLevel->value = normalize(hamradio_get_mik1level(), level_min, level_max, 100);

		sl_micClip->x = sl_micLevel->x;
		sl_micClip->y = lbl_micClip->y;
		sl_micClip->visible = VISIBLE;
		sl_micClip->size = 300;
		sl_micClip->step = 3;
		sl_micClip->value = normalize(hamradio_get_gmikehclip(), clip_min, clip_max, 100);

		sl_micAGC->x = sl_micLevel->x;
		sl_micAGC->y = lbl_micAGC->y;
		sl_micAGC->visible = VISIBLE;
		sl_micAGC->size = 300;
		sl_micAGC->step = 3;
		sl_micAGC->value = normalize(hamradio_get_gmikeagcgain(), agc_min, agc_max, 100);

		button_t * bh2 = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_mic_boost");
		bh2->x1 = (sl_micLevel->x + sl_micLevel->size) / 2 - (bh2->w / 2);
		bh2->y1 = lbl_micAGC->y + interval;
		bh2->is_locked = hamradio_get_gmikeboost20db() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh2->text, ARRAY_SIZE(bh2->text), PSTR("Boost|%s"), bh2->is_locked ? "ON" : "OFF");
		bh2->visible = VISIBLE;

		button_t * bh1 = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_mic_agc");
		bh1->x1 = bh2->x1 - bh1->w - interval;
		bh1->y1 = bh2->y1;
		bh1->is_locked = hamradio_get_gmikeagc() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh1->text, ARRAY_SIZE(bh1->text), PSTR("AGC|%s"), bh1->is_locked ? "ON" : "OFF");
		bh1->visible = VISIBLE;

		bh1 = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_mic_OK");
		bh1->x1 = bh2->x1 + bh2->w + interval;
		bh1->y1 = bh2->y1;
		bh1->visible = VISIBLE;

		label_t * lbl_micLevel_min = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_micLevel_min");
		local_snprintf_P(lbl_micLevel_min->text, ARRAY_SIZE(lbl_micLevel_min->text), PSTR("%d"), level_min);
		lbl_micLevel_min->x = sl_micLevel->x - get_label_width(lbl_micLevel_min) / 2;
		lbl_micLevel_min->y = sl_micLevel->y + get_label_height(lbl_micLevel_min) * 3;
		lbl_micLevel_min->visible = VISIBLE;

		label_t * lbl_micLevel_max = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_micLevel_max");
		local_snprintf_P(lbl_micLevel_max->text, ARRAY_SIZE(lbl_micLevel_max->text), PSTR("%d"), level_max);
		lbl_micLevel_max->x = sl_micLevel->x + sl_micLevel->size - get_label_width(lbl_micLevel_max) / 2;
		lbl_micLevel_max->y = sl_micLevel->y + get_label_height(lbl_micLevel_max) * 3;
		lbl_micLevel_max->visible = VISIBLE;

		label_t * lbl_micClip_min = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_micClip_min");
		local_snprintf_P(lbl_micClip_min->text, ARRAY_SIZE(lbl_micClip_min->text), PSTR("%d"), clip_min);
		lbl_micClip_min->x = sl_micClip->x - get_label_width(lbl_micClip_min) / 2;
		lbl_micClip_min->y = sl_micClip->y + get_label_height(lbl_micClip_min) * 3;
		lbl_micClip_min->visible = VISIBLE;

		label_t * lbl_micClip_max = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_micClip_max");
		local_snprintf_P(lbl_micClip_max->text, ARRAY_SIZE(lbl_micClip_max->text), PSTR("%d"), clip_max);
		lbl_micClip_max->x = sl_micClip->x + sl_micClip->size - get_label_width(lbl_micClip_max) / 2;
		lbl_micClip_max->y = sl_micClip->y + get_label_height(lbl_micClip_max) * 3;
		lbl_micClip_max->visible = VISIBLE;

		label_t * lbl_micAGC_min = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_micAGC_min");
		local_snprintf_P(lbl_micAGC_min->text, ARRAY_SIZE(lbl_micAGC_min->text), PSTR("%d"), agc_min);
		lbl_micAGC_min->x = sl_micAGC->x - get_label_width(lbl_micAGC_min) / 2;
		lbl_micAGC_min->y = sl_micAGC->y + get_label_height(lbl_micAGC_min) * 3;
		lbl_micAGC_min->visible = VISIBLE;

		label_t * lbl_micAGC_max = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_micAGC_max");
		local_snprintf_P(lbl_micAGC_max->text, ARRAY_SIZE(lbl_micAGC_max->text), PSTR("%d"), agc_max);
		lbl_micAGC_max->x = sl_micAGC->x + sl_micClip->size - get_label_width(lbl_micAGC_max) / 2;
		lbl_micAGC_max->y = sl_micAGC->y + get_label_height(lbl_micAGC_max) * 3;
		lbl_micAGC_max->visible = VISIBLE;

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_mic_boost = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_mic_boost");
			button_t * btn_mic_agc = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_mic_agc");
			button_t * btn_mic_OK = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_mic_OK");

			if (bh == btn_mic_boost)
			{
				btn_mic_boost->is_locked = hamradio_get_gmikeboost20db() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
				local_snprintf_P(btn_mic_boost->text, ARRAY_SIZE(btn_mic_boost->text), PSTR("Boost|%s"), btn_mic_boost->is_locked ? "ON" : "OFF");
				hamradio_set_gmikeboost20db(btn_mic_boost->is_locked);
			}
			else if (bh == btn_mic_agc)
			{
				btn_mic_agc->is_locked = hamradio_get_gmikeagc() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
				local_snprintf_P(btn_mic_agc->text, ARRAY_SIZE(btn_mic_agc->text), PSTR("AGC|%s"), btn_mic_agc->is_locked ? "ON" : "OFF");
				hamradio_set_gmikeagc(btn_mic_agc->is_locked);
			}
			else if (bh == btn_mic_OK)
			{
				close_all_windows();
				return;
			}
		}
		else if (IS_SLIDER_MOVE)
		{
			slider_t * sh = (slider_t *) ptr;
			if (sh == sl_micLevel)
			{
				uint_fast16_t level = level_min + normalize(sh->value, 0, 100, level_max - level_min);
				local_snprintf_P(lbl_micLevel->text, ARRAY_SIZE(lbl_micLevel->text), PSTR("Level: %3d"), level);
				hamradio_set_mik1level(level);
			}
			else if (sh == sl_micClip)
			{
				uint_fast16_t clip = clip_min + normalize(sh->value, 0, 100, clip_max - clip_min);
				local_snprintf_P(lbl_micClip->text, ARRAY_SIZE(lbl_micClip->text), PSTR("Clip : %3d"), clip);
				hamradio_set_gmikehclip(clip);
			}
			else if (sh == sl_micAGC)
			{
				uint_fast16_t agc = agc_min + normalize(sh->value, 0, 100, agc_max - agc_min);
				local_snprintf_P(lbl_micAGC->text, ARRAY_SIZE(lbl_micAGC->text), PSTR("AGC  : %3d"), agc);
				hamradio_set_gmikeagcgain(agc);
			}
		}
		break;

	default:

		break;
	}
#endif /* WITHAFCODEC1HAVEPROC */
}

// *********************************************************************************************************************************************************************

static void window_ap_mic_prof_process(void)
{
#if WITHAFCODEC1HAVEPROC
	window_t * const win = get_win(WINDOW_AP_MIC_PROF);

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 6, row_count = 3;
		win->first_call = 0;

		static const button_t buttons [] = {
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 1, WINDOW_AP_MIC_PROF, 	NON_VISIBLE, INT32_MAX, "btn_mic_profile_1", "", 0, },
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 1, WINDOW_AP_MIC_PROF, 	NON_VISIBLE, INT32_MAX, "btn_mic_profile_2", "", 1, },
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 1, WINDOW_AP_MIC_PROF, 	NON_VISIBLE, INT32_MAX, "btn_mic_profile_3", "", 2, },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = 0;
		y = 0;

		unsigned i, r;
		for (i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = 0;
				y = y + bh->h + interval;
			}
			unsigned cell_saved = hamradio_load_mic_profile(i, 0);
			local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("%d|%s"), i + 1, cell_saved ? "saved" : "clean");
			bh->payload = cell_saved;

			if (gui_nvram.micprofile == i && bh->payload)
				bh->is_locked = BUTTON_LOCKED;
		}

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			unsigned profile_id = bh->index;
			if (bh->payload)
			{
				hamradio_load_mic_profile(profile_id, 1);
				gui_nvram.micprofile = profile_id;
				save_settings();
				close_window(DONT_OPEN_PARENT_WINDOW);
				footer_buttons_state(CANCELLED);
				return;
			}
		}
		else if (IS_BUTTON_LONG_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			unsigned profile_id = bh->index;
			if (bh->payload)
			{
				hamradio_clean_mic_profile(profile_id);
				local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("%d|clean"), profile_id + 1);
				bh->payload = 0;
				if (gui_nvram.micprofile == profile_id)
				{
					gui_nvram.micprofile = micprofile_default;
					save_settings();
					bh->is_locked = BUTTON_NON_LOCKED;
				}
			}
			else
			{
				hamradio_save_mic_profile(profile_id);
				local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("%d|saved"), profile_id + 1);
				bh->payload = 1;
			}
		}
		break;

	default:

		break;
	}
#endif /* WITHAFCODEC1HAVEPROC */
}

// *********************************************************************************************************************************************************************

static void window_notch_process(void)
{
	window_t * const win = get_win(WINDOW_NOTCH);
	static enc_var_t notch;

	if (win->first_call)
	{
		win->first_call = 0;
		unsigned interval = 50;
		notch.change = 0;
		notch.updated = 1;

		static const button_t buttons [] = {
			{ 40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_NOTCH, NON_VISIBLE, -1, "btn_freq-",  "-", },
			{ 40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_NOTCH, NON_VISIBLE, 1,  "btn_freq+",  "+", },
			{ 40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_NOTCH, NON_VISIBLE, -1, "btn_width-", "-", },
			{ 40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_NOTCH, NON_VISIBLE, 1,  "btn_width+", "+", },
			{ 80, 35, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_NOTCH, NON_VISIBLE, 0,  "btn_Auto",   "Auto",   },
			{ 80, 35, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_NOTCH, NON_VISIBLE, 1,  "btn_Manual", "Manual", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		static const label_t labels [] = {
			{ WINDOW_NOTCH, CANCELLED, 0, NON_VISIBLE, "lbl_freq",  "Freq:  *******",  FONT_MEDIUM, COLORPIP_YELLOW, },
			{ WINDOW_NOTCH, CANCELLED, 0, NON_VISIBLE, "lbl_width", "Width: *******",  FONT_MEDIUM, COLORPIP_WHITE,  },
			{ WINDOW_NOTCH, CANCELLED, 0, NON_VISIBLE, "lbl_type",  "Type: ",  		   FONT_MEDIUM, COLORPIP_WHITE,  },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t*) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		label_t * lbl_freq = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_freq");
		label_t * lbl_width = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_width");
		label_t * lbl_type = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_type");

		lbl_freq->x = 0;
		lbl_freq->y = 15;
		lbl_freq->visible = VISIBLE;

		lbl_width->x = lbl_freq->x;
		lbl_width->y = lbl_freq->y + interval;
		lbl_width->visible = VISIBLE;

		lbl_type->x = lbl_width->x;
		lbl_type->y = lbl_width->y + interval;
		lbl_type->visible = VISIBLE;

		button_t * btn_freqm = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_freq-");
		button_t * btn_freqp = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_freq+");
		button_t * btn_widthm = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_width-");
		button_t * btn_widthp = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_width+");
		button_t * btn_Auto = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_Auto");
		button_t * btn_Manual = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_Manual");

		btn_freqm->x1 = lbl_freq->x + get_label_width(lbl_freq);
		btn_freqm->y1 = lbl_freq->y + get_label_height(lbl_freq) / 2 - btn_freqm->h / 2;
		btn_freqm->visible = VISIBLE;

		btn_freqp->x1 = btn_freqm->x1 + btn_freqm->w + 10;
		btn_freqp->y1 = btn_freqm->y1;
		btn_freqp->visible = VISIBLE;

		btn_widthm->x1 = btn_freqm->x1;
		btn_widthm->y1 = btn_freqm->y1 + interval;
		btn_widthm->visible = VISIBLE;

		btn_widthp->x1 = btn_freqp->x1;
		btn_widthp->y1 = btn_widthm->y1;
		btn_widthp->visible = VISIBLE;

		btn_Auto->x1 = lbl_type->x + get_label_width(lbl_type);
		btn_Auto->y1 = lbl_type->y + get_label_height(lbl_type) / 2 - btn_Auto->h / 2;
		btn_Auto->visible = VISIBLE;

		btn_Manual->x1 = btn_Auto->x1 + btn_Auto->w + 10;
		btn_Manual->y1 = btn_Auto->y1;
		btn_Manual->visible = VISIBLE;

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_freqm = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_freq-");
			button_t * btn_freqp = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_freq+");
			button_t * btn_widthm = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_width-");
			button_t * btn_widthp = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_width+");
			button_t * btn_Auto = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_Auto");
			button_t * btn_Manual = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_Manual");

			if (bh == btn_freqm || bh == btn_freqp)
			{
				notch.select = TYPE_NOTCH_FREQ;
				notch.change = bh->payload;
				notch.updated = 1;
			}
			else if (bh == btn_widthm || bh == btn_widthp)
			{
				notch.select = TYPE_NOTCH_WIDTH;
				notch.change = bh->payload;
				notch.updated = 1;
			}
			else if (bh == btn_Manual || bh == btn_Auto)
			{
				hamradio_set_gnotchtype(bh->payload);
				notch.change = 0;
				notch.updated = 1;
			}
		}
		else if (IS_LABEL_PRESS)
		{
			label_t * lh = (label_t *) ptr;
			label_t * lbl_freq = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_freq");
			label_t * lbl_width = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_width");

			if (lh == lbl_freq)
				notch.select = TYPE_NOTCH_FREQ;
			else if (lh == lbl_width)
				notch.select = TYPE_NOTCH_WIDTH;

			notch.change = 0;
			notch.updated = 1;
		}
		break;

	case WM_MESSAGE_ENC2_ROTATE:

		notch.change = action;
		notch.updated = 1;
		break;

	case WM_MESSAGE_UPDATE:

		notch.change = 0;
		notch.updated = 1;
		break;

	default:

		break;
	}

	if (notch.updated)
	{
		button_t * btn_Auto = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_Auto");
		button_t * btn_Manual = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_Manual");
		label_t * lbl_freq = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_freq");
		label_t * lbl_width = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_width");

		notch.updated = 0;

		for(unsigned i = 0; i < win->lh_count; i ++)
			win->lh_ptr [i].color = COLORPIP_WHITE;

		ASSERT(notch.select < win->lh_count);
		win->lh_ptr [notch.select].color = COLORPIP_YELLOW;

		unsigned type = hamradio_get_gnotchtype();
		btn_Auto->is_locked = type == BOARD_NOTCH_AUTO ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		btn_Manual->is_locked = type == BOARD_NOTCH_MANUAL ? BUTTON_LOCKED : BUTTON_NON_LOCKED;

		local_snprintf_P(lbl_freq->text, ARRAY_SIZE(lbl_freq->text), PSTR("Freq:%5d Hz"), hamradio_notch_freq(notch.select == TYPE_NOTCH_FREQ ? notch.change : 0));
		local_snprintf_P(lbl_width->text, ARRAY_SIZE(lbl_width->text), PSTR("Width:%4d Hz"), hamradio_notch_width(notch.select == TYPE_NOTCH_WIDTH ? notch.change : 0));
	}
}

// *********************************************************************************************************************************************************************

static void window_gui_settings_process(void)
{
	window_t * const win = get_win(WINDOW_GUI_SETTINGS);
	unsigned update = 0;

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 6, row_count = 4;
		win->first_call = 0;

		static const button_t buttons [] = {
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_GUI_SETTINGS, NON_VISIBLE, INT32_MAX, "btn_enc2_step",  "", },
			{ 110, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_GUI_SETTINGS, NON_VISIBLE, INT32_MAX, "btn_freq_swipe", "", },
			{ 110, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_GUI_SETTINGS, NON_VISIBLE, INT32_MAX, "btn_freq_swipe_step", "", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		for (unsigned i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = 0;
				y = y + bh->h + interval;
			}
		}

		update = 1;
		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_enc2_step = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_enc2_step");
			button_t * btn_freq_swipe = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_freq_swipe");
			button_t * btn_freq_swipe_step = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_freq_swipe_step");

			if (bh == btn_enc2_step)
			{
				gui_nvram.enc2step_pos = (gui_nvram.enc2step_pos + 1 ) % enc2step_vals;
				save_settings();
				update = 1;
			}

			if (bh == btn_freq_swipe)
			{
				gui_nvram.freq_swipe_enable = ! gui_nvram.freq_swipe_enable;
				save_settings();
				update = 1;
			}

			if (bh == btn_freq_swipe_step)
			{
				gui_nvram.freq_swipe_step = (gui_nvram.freq_swipe_step + 1 ) % freq_swipe_step_vals;
				save_settings();
				update = 1;
			}
		}
		break;

	default:

		break;
	}

	if (update)
	{
		update = 0;
		button_t * btn_enc2_step = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_enc2_step");
		local_snprintf_P(btn_enc2_step->text, ARRAY_SIZE(btn_enc2_step->text), "Enc2 step|%s", enc2step [gui_nvram.enc2step_pos].label);

		button_t * btn_freq_swipe = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_freq_swipe");
		btn_freq_swipe->is_locked = gui_nvram.freq_swipe_enable != 0;
		local_snprintf_P(btn_freq_swipe->text, ARRAY_SIZE(btn_freq_swipe->text), "Freq swipe|%s", btn_freq_swipe->is_locked ? "enable" : "disable");

		button_t * btn_freq_swipe_step = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_freq_swipe_step");
		btn_freq_swipe_step->state = btn_freq_swipe->is_locked ? CANCELLED : DISABLED;
		local_snprintf_P(btn_freq_swipe_step->text, ARRAY_SIZE(btn_freq_swipe_step->text), "Swipe step|%s", freq_swipe_step[gui_nvram.freq_swipe_step].label);
	}
}

// *********************************************************************************************************************************************************************

static void window_shift_process(void)
{
#if WITHIQSHIFT
	window_t * const win = get_win(WINDOW_SHIFT);

	static unsigned update = 0, cic_test = 0;
	static enc_var_t enc;

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 6;
		win->first_call = 0;
		update = 1;
		enc.updated = 1;
		enc.select = 0;
		enc.change = 0;

		add_element("lbl_rx_cic_shift", 0, FONT_MEDIUM, COLORPIP_WHITE, 13);
		add_element("lbl_rx_fir_shift", 0, FONT_MEDIUM, COLORPIP_WHITE, 13);
		add_element("lbl_tx_shift", 0, FONT_MEDIUM, COLORPIP_WHITE, 13);
		add_element("lbl_iq_test", 0, FONT_MEDIUM, COLORPIP_WHITE, 23);

		add_element("btn_p", 50, 50, 0, 0, "+");
		add_element("btn_m", 50, 50, 0, 0, "-");
		add_element("btn_test", 50, 50, 0, 0, "CIC|test");

		for (unsigned i = 0; i < win->lh_count; i ++)
		{
			label_t * lh = & win->lh_ptr [i];

			lh->x = x;
			lh->y = y;
			lh->index = i;
			lh->visible = VISIBLE;
			lh->state = CANCELLED;

			y += get_label_height(lh) + interval * 3;
		}

		label_t * lh = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_rx_cic_shift");
		x = lh->x + get_label_width(lh) + interval * 2;

		for (unsigned i = 0; i < win->bh_count; i ++)
		{
			button_t * bh = & win->bh_ptr [i];

			bh->x1 = x;
			bh->y1 = 0;
			bh->visible = VISIBLE;

			x += bh->w + interval * 2;
		}

		label_t * lbl_iq_test = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_iq_test");
		local_snprintf_P(lbl_iq_test->text, ARRAY_SIZE(lbl_iq_test->text), "MAX IQ test:");

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:
		if (IS_LABEL_PRESS)
		{
			label_t * lh = (label_t *) ptr;
			enc.select = lh->index;
			enc.change = 0;
			enc.updated = 1;
		}
		else if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_test"))
			{
				cic_test = ! cic_test;
				iq_cic_test(cic_test);
				bh->is_locked = cic_test;
			}
			else if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_p"))
			{
				enc.change = 1;
				enc.updated = 1;
			}
			else if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_m"))
			{
				enc.change = -1;
				enc.updated = 1;
			}
		}
		break;

	case WM_MESSAGE_ENC2_ROTATE:

		enc.change = action;
		enc.updated = 1;
		break;

	case WM_MESSAGE_UPDATE:

		update = 1;
		break;

	default:

		break;
	}

	if (enc.updated)
	{
		enc.updated = 0;

		for(unsigned i = 0; i < win->lh_count; i ++)
			win->lh_ptr [i].color = COLORPIP_WHITE;

		ASSERT(enc.select < win->lh_count);

		if (enc.select == 0)
		{
			win->lh_ptr [0].color = COLORPIP_YELLOW;
			unsigned v = iq_shift_cic_rx(0);
			iq_shift_cic_rx(v + enc.change);
		}
		else if (enc.select == 1)
		{
			win->lh_ptr [1].color = COLORPIP_YELLOW;
			unsigned v = iq_shift_fir_rx(0);
			iq_shift_fir_rx(v + enc.change);
		}
		else if (enc.select == 2)
		{
			win->lh_ptr [2].color = COLORPIP_YELLOW;
			unsigned v = iq_shift_tx(0);
			iq_shift_tx(v + enc.change);
		}

		update = 1;
	}

	if (update)
	{
		update = 0;

		label_t * lbl_rx_cic_shift = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_rx_cic_shift");
		local_snprintf_P(lbl_rx_cic_shift->text, ARRAY_SIZE(lbl_rx_cic_shift->text), "RX CIC: %d", (int) iq_shift_cic_rx(0));
		label_t * lbl_rx_fir_shift = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_rx_fir_shift");
		local_snprintf_P(lbl_rx_fir_shift->text, ARRAY_SIZE(lbl_rx_fir_shift->text), "RX FIR: %d", (int) iq_shift_fir_rx(0));
		label_t * lbl_tx_shift = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_tx_shift");
		local_snprintf_P(lbl_tx_shift->text, ARRAY_SIZE(lbl_tx_shift->text), "TX CIC: %d", (int) iq_shift_tx(0));

		button_t * btn_test = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_test");
		btn_test->is_locked = cic_test;

		if (cic_test)
		{
			label_t * lbl_iq_test = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_iq_test");
			local_snprintf_P(lbl_iq_test->text, ARRAY_SIZE(lbl_iq_test->text), "MAX IQ test: 0x%08lx", iq_cic_test_process());
		}
	}
#endif /* WITHIQSHIFT */
}

// *********************************************************************************************************************************************************************

void gui_uif_editmenu(const char * name, uint_fast16_t menupos, uint_fast8_t exitkey)
{
	window_t * const win = get_win(WINDOW_UIF);
	if (win->state == NON_VISIBLE)
	{
		close_window(DONT_OPEN_PARENT_WINDOW);
		open_window(win);
		footer_buttons_state(DISABLED, NULL);
		strcpy(menu_uif.name, name);
		menu_uif.menupos = menupos;
		menu_uif.exitkey = exitkey;
		hamradio_enable_encoder2_redirect();
	}
	else if (win->state == VISIBLE)
	{
		close_window(DONT_OPEN_PARENT_WINDOW);
		footer_buttons_state(CANCELLED);
		hamradio_disable_encoder2_redirect();
	}
}

static void window_uif_process(void)
{
	static label_t * lbl_uif_val;
	static button_t * button_up, * button_down;
	static unsigned window_center_x, reinit = 0;
	window_t * const win = get_win(WINDOW_UIF);
	int rotate = 0;

	if (win->first_call)
	{
		win->first_call = 0;
		reinit = 1;
		static const unsigned win_width = 170;

		static const button_t buttons [] = {
			{ 40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_UIF, NON_VISIBLE, -1, "btn_UIF-", "-", },
			{ 40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_UIF, NON_VISIBLE, 1,  "btn_UIF+", "+", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		static const label_t labels [] = {
			{	WINDOW_UIF,  DISABLED,  0, NON_VISIBLE, "lbl_uif_val", "**", FONT_LARGE, COLORPIP_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t*) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		button_down = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_UIF-");
		button_up = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_UIF+");
		lbl_uif_val = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_uif_val");

		const char * v = hamradio_gui_edit_menu_item(menu_uif.menupos, 0);
		strcpy(lbl_uif_val->text, v);

		button_down->x1 = 0;
		button_down->y1 = 0;
		button_down->visible = VISIBLE;

		button_up->x1 = button_down->x1 + button_down->w + 30 + get_label_width(lbl_uif_val);
		button_up->y1 = button_down->y1;
		button_up->visible = VISIBLE;

		window_center_x = (button_up->x1 + button_up->w) / 2;

		lbl_uif_val->x = window_center_x - get_label_width(lbl_uif_val) / 2;
		lbl_uif_val->y = button_up->h / 2 - get_label_height(lbl_uif_val) / 2;
		lbl_uif_val->visible = VISIBLE;

		strcpy(win->name, menu_uif.name);

		hamradio_enable_keyboard_redirect();
		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			rotate = bh->payload;
		}
		break;

	case WM_MESSAGE_KEYB_CODE:

		if (action == menu_uif.exitkey)
		{
			hamradio_disable_keyboard_redirect();
			close_window(DONT_OPEN_PARENT_WINDOW);
			footer_buttons_state(CANCELLED);
			return;
		}

		break;

	default:

		break;
	}

	if (reinit)
	{
		reinit = 0;
		strcpy(win->name, menu_uif.name);

		const char * v = hamradio_gui_edit_menu_item(menu_uif.menupos, 0);
		strcpy(lbl_uif_val->text, v);

		return;
	}

	if (rotate != 0)
	{
		hamradio_gui_edit_menu_item(menu_uif.menupos, rotate);
		reinit = 1;
		gui_update();
	}
}

// *********************************************************************************************************************************************************************

void gui_open_sys_menu(void)
{
	if (check_for_parent_window() != NO_PARENT_WINDOW)
	{
		close_window(OPEN_PARENT_WINDOW);
		footer_buttons_state(CANCELLED);
	}
	else
	{
		window_t * const win = get_win(WINDOW_MENU);
		open_window(win);
		footer_buttons_state(DISABLED);
	}
}

// ****** Common windows ***********************************************************************

#if WITHFT8

static unsigned parse_ft8buf = 0, idx_cqcall = 0, cq_filter = 0;
static char cq_call [6][10];

void hamradio_gui_parse_ft8buf(void)
{
	parse_ft8buf = 1;
	idx_cqcall = 0;
	memset(cq_call, 0, sizeof(cq_call));
}

static void parse_ft8_answer(char * str, COLORPIP_T * color, unsigned * cq_flag)
{
	* color = COLORPIP_WHITE;
	* cq_flag = 0;
	char tmpstr [TEXT_ARRAY_SIZE];
	char lexem [10][10]; // time; freq; snr; text 2 - 4 pcs
	char * next;
	strcpy(tmpstr, str);

	char * l = strtok_r(tmpstr, " ", & next);
	for (unsigned i = 0; i < ARRAY_SIZE(lexem); i ++)
	{
		if (l == NULL)
			break;

		strcpy(lexem[i], l);
		l = strtok_r(NULL, " ", & next);
	}

	if (! strcmp(lexem[3], "CQ") && strcmp(lexem[4], "CQ") && strcmp(lexem[4], "DX"))
	{
		* color = COLORPIP_GREEN;
		* cq_flag = 1;
		if (idx_cqcall < ARRAY_SIZE(cq_call))
			strcpy(cq_call [idx_cqcall ++], lexem[4]);
	}
	else if (! strcmp(lexem[3], gui_nvram.ft8_callsign))
	{
		* color = COLORPIP_RED;
		* cq_flag = 1;
		strcpy(cq_call [idx_cqcall ++], lexem[4]);
	}
}

static void window_ft8_bands_process(void)
{
	window_t * const win = get_win(WINDOW_FT8_BANDS);

	if (win->first_call)
	{
		win->first_call = 0;
		unsigned cols = ft8_bands_count > 10 ? 4 : 5, interval = 15, x = 0, y = 0;
		button_t * bh;

		win->bh_count = ft8_bands_count;
		unsigned buttons_size = win->bh_count * sizeof (button_t);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);

		unsigned freq = hamradio_get_freq_rx();

		for (unsigned i = 0; i < win->bh_count; i ++)
		{
			bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->w = 86;
			bh->h = 44;
			bh->state = CANCELLED;
			bh->visible = VISIBLE;
			bh->parent = WINDOW_FT8_BANDS;
			bh->index = i;
			bh->is_long_press = 0;
			bh->is_repeating = 0;
			local_snprintf_P(bh->name, ARRAY_SIZE(bh->name), "btn_bands_%02d", i);
			local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), "%dk", ft8_bands [i] / 1000);
			bh->payload = ft8_bands [i];
			bh->is_locked = freq == bh->payload ? BUTTON_LOCKED : BUTTON_NON_LOCKED;

			x = x + interval + bh->w;
			if ((i + 1) % cols == 0)
			{
				x = 0;
				y = y + bh->h + interval;
			}
		}

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			gui_nvram.ft8_band = bh->index;
			hamradio_set_freq(ft8_bands [gui_nvram.ft8_band]);
			close_window(OPEN_PARENT_WINDOW);
		}

		break;

	default:

		break;
	}
}

static void window_ft8_settings_process(void)
{
	window_t * const win = get_win(WINDOW_FT8_SETTINGS);

	if (win->first_call)
	{
		win->first_call = 0;
		unsigned interval = 10;

		add_element("btn_callsign", 86, 30, 0, 0, "Callsign");
		add_element("btn_qth", 86, 30, 0, 0, "QTH");
		add_element("btn_freq", 86, 30, 0, 0, "TX freq");
		add_element("btn_freq_eq", 86, 30, 0, 0, "");
		add_element("btn_time0", 86, 30, 0, 0, "Time >0<");
		add_element("btn_OK", 40, 40, 0, 0, "OK");

		add_element("lbl_callsign", 0, FONT_MEDIUM, COLORPIP_WHITE, 10);
		add_element("lbl_qth", 0, FONT_MEDIUM, COLORPIP_WHITE, 10);
		add_element("lbl_txfreq", 0, FONT_MEDIUM, COLORPIP_WHITE, 10);

		label_t * lbl_callsign = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_callsign");
		label_t * lbl_qth = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_qth");
		label_t * lbl_txfreq = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_txfreq");
		button_t * btn_callsign = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_callsign");
		button_t * btn_qth = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_qth");
		button_t * btn_freq = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_freq");
		button_t * btn_OK = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_OK");
		button_t * btn_time0 = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_time0");

		unsigned lbllen = get_label_width(lbl_callsign);

		btn_freq->x1 = lbllen + interval;
		btn_freq->y1 = 0;
		btn_freq->visible = VISIBLE;

		btn_callsign->x1 = btn_freq->x1;
		btn_callsign->y1 = btn_freq->y1 + btn_freq->h + interval * 2;
		btn_callsign->visible = VISIBLE;
		btn_callsign->payload = 1;

		btn_qth->x1 = btn_callsign->x1;
		btn_qth->y1 = btn_callsign->y1 + btn_callsign->h + interval * 2;
		btn_qth->visible = VISIBLE;
		btn_qth->payload = 2;

		lbl_txfreq->x = 0;
		lbl_txfreq->y = btn_freq->y1 + btn_freq->h / 2 - get_label_height(lbl_txfreq) / 2;
		lbl_txfreq->visible = VISIBLE;
		local_snprintf_P(lbl_txfreq->text, ARRAY_SIZE(lbl_txfreq->text), "%d Hz", (int) gui_nvram.ft8_txfreq_val);

		lbl_callsign->x = 0;
		lbl_callsign->y = btn_callsign->y1 + btn_callsign->h / 2 - get_label_height(lbl_callsign) / 2;
		lbl_callsign->visible = VISIBLE;
		local_snprintf_P(lbl_callsign->text, ARRAY_SIZE(lbl_callsign->text), "%s", gui_nvram.ft8_callsign);

		lbl_qth->x = 0;
		lbl_qth->y = btn_qth->y1 + btn_qth->h / 2 - get_label_height(lbl_qth) / 2;
		lbl_qth->visible = VISIBLE;
		local_snprintf_P(lbl_qth->text, ARRAY_SIZE(lbl_qth->text), "%s", gui_nvram.ft8_qth);

		btn_time0->x1 = btn_qth->x1;
		btn_time0->y1 = btn_qth->y1 + btn_qth->h + interval * 2;
		btn_time0->visible = VISIBLE;

		btn_OK->x1 = 0;
		btn_OK->y1 = btn_time0->y1;
		btn_OK->visible = VISIBLE;

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;

			if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_OK"))
			{
				close_window(OPEN_PARENT_WINDOW);
			}
			else if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_freq"))
			{
				uint32_t f = gui_nvram.ft8_txfreq_val;
				keyboard_edit_digits(& f);
				uint32_t l = hamradio_get_low_bp(0) * 10;
				uint32_t h = hamradio_get_high_bp(0) * 10;
				gui_nvram.ft8_txfreq_val = (f >= l && f <= h) ? f : gui_nvram.ft8_txfreq_val;
			}
			else if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_time0"))
			{
				uint_fast8_t hour, minute, seconds;
				board_rtc_cached_gettime(& hour, & minute, & seconds);
				seconds = 0;
				//board_rtc_settime(hour, minute, seconds);
			}
			else if (bh->payload == 1)
				keyboard_edit_string((uintptr_t) & gui_nvram.ft8_callsign, 10, 0);
			else if (bh->payload == 2)
				keyboard_edit_string((uintptr_t) & gui_nvram.ft8_qth, 10, 0);
		}

		break;

	default:

		break;
	}
}

static void window_ft8_process(void)
{
	window_t * const win = get_win(WINDOW_FT8);
	static unsigned win_x = 0, win_y = 0, x, y, update = 0, selected_label_cq = 255, selected_label_tx = 0, backup_mode = 0, work = 0, labels_tx_update = 0, backup_freq = 0;
	static lh_array_t lh_array_cq [6];
	static lh_array_t lh_array_tx [4];
	static const int snr = -10;
	static uint8_t viewtemp;

	if (win->first_call)
	{
		win->first_call = 0;

		add_element("btn_tx", 86, 44, 0, 0, "Transmit");
		add_element("btn_filter", 86, 44, 0, 0, "View|all");
		add_element("btn_bands", 86, 44, 0, 0, "FT8|bands");
		add_element("btn_settings", 86, 44, 0, 0, "Edit|settings");

		add_element("lbl_cq_title", 0, FONT_LARGE, COLORPIP_GREEN, 3);
		add_element("lbl_tx_title", 0, FONT_LARGE, COLORPIP_GREEN, 3);
		add_element("lbl_cq0", 0, FONT_MEDIUM, COLORPIP_WHITE, 8);
		add_element("lbl_cq1", 0, FONT_MEDIUM, COLORPIP_WHITE, 8);
		add_element("lbl_cq2", 0, FONT_MEDIUM, COLORPIP_WHITE, 8);
		add_element("lbl_cq3", 0, FONT_MEDIUM, COLORPIP_WHITE, 8);
		add_element("lbl_cq4", 0, FONT_MEDIUM, COLORPIP_WHITE, 8);
		add_element("lbl_cq5", 0, FONT_MEDIUM, COLORPIP_WHITE, 8);
		add_element("lbl_txmsg0", 0, FONT_MEDIUM, COLORPIP_WHITE, 10);
		add_element("lbl_txmsg1", 0, FONT_MEDIUM, COLORPIP_WHITE, 10);
		add_element("lbl_txmsg2", 0, FONT_MEDIUM, COLORPIP_WHITE, 10);
		add_element("lbl_txmsg3", 0, FONT_MEDIUM, COLORPIP_WHITE, 10);

		static const text_field_t text_field [] = {
			{ 37, 26, CANCELLED, WINDOW_FT8, NON_VISIBLE, UP, & gothic_11x13, "tf_ft8", },
		};
		win->tf_count = ARRAY_SIZE(text_field);
		unsigned tf_size = sizeof(text_field);
		win->tf_ptr = (text_field_t*) malloc(tf_size);
		GUI_MEM_ASSERT(win->tf_ptr);
		memcpy(win->tf_ptr, text_field, tf_size);

		text_field_t * tf_ft8 = (text_field_t*) find_gui_element(TYPE_TEXT_FIELD, win, "tf_ft8");
		textfield_update_size(tf_ft8);
		tf_ft8->x1 = 0;
		tf_ft8->y1 = 0;
		tf_ft8->visible = VISIBLE;

		label_t * lh = NULL;
		label_t * lbl_cq_title = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_cq_title");
		label_t * lbl_tx_title = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_tx_title");
		button_t * btn_tx = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_tx");
		button_t * btn_filter = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_filter");
		button_t * btn_bands = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_bands");
		button_t * btn_settings = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_settings");

		unsigned interval = get_label_height(lbl_cq_title) * 2;

		lbl_cq_title->x = tf_ft8->x1 + tf_ft8->w + 70;
		lbl_cq_title->y = 0;
		lbl_cq_title->state = DISABLED;
		lbl_cq_title->visible = VISIBLE;
		local_snprintf_P(lbl_cq_title->text, ARRAY_SIZE(lbl_cq_title->text), "CQ:");

		x = lbl_cq_title->x;
		y = lbl_cq_title->y + interval;
		for (unsigned i = 0; i < 6; i ++)
		{
			char lh_name [15];
			local_snprintf_P(lh_name, ARRAY_SIZE(lh_name), "lbl_cq%d", i);
			lh = (label_t*) find_gui_element(TYPE_LABEL, win, lh_name);
			lh->x = x;
			lh->y = y;
			lh->index = i;
			y += interval;
			lh_array_cq [i].ptr = lh;
		}

		lbl_tx_title->x = lbl_cq_title->x + get_label_width(lh_array_cq [0].ptr) + 10;
		lbl_tx_title->y = 0;
		lbl_tx_title->state = DISABLED;
		lbl_tx_title->visible = VISIBLE;
		local_snprintf_P(lbl_tx_title->text, ARRAY_SIZE(lbl_tx_title->text), "TX:");

		x = lbl_tx_title->x;
		y = lbl_tx_title->y + interval;
		for (unsigned i = 0; i < 4; i ++)
		{
			char lh_name [15];
			local_snprintf_P(lh_name, ARRAY_SIZE(lh_name), "lbl_txmsg%d", i);
			lh = (label_t*) find_gui_element(TYPE_LABEL, win, lh_name);
			lh->x = x;
			lh->y = y;
			lh->index = 10 + i;
			lh->visible = VISIBLE;
			y += interval;
			lh_array_tx [i].ptr = lh;
		}

		local_snprintf_P(lh_array_tx [0].ptr->text, ARRAY_SIZE(lh_array_tx [0].ptr->text), "CQ %s %s", gui_nvram.ft8_callsign, gui_nvram.ft8_qth);

		btn_tx->x1 = lh_array_tx [3].ptr->x;
		btn_tx->y1 = lh_array_tx [3].ptr->y + get_label_height(lh_array_tx [3].ptr) * 3;
		btn_tx->visible = VISIBLE;
#if ! WITHTX
		btn_tx->state = DISABLED;
#endif

		btn_filter->x1 = btn_tx->x1 + btn_tx->w + interval;
		btn_filter->y1 = btn_tx->y1;
		btn_filter->visible = VISIBLE;

		btn_bands->x1 = btn_tx->x1;
		btn_bands->y1 = btn_tx->y1 + btn_tx->h + interval;
		btn_bands->visible = VISIBLE;

		btn_settings->x1 = btn_filter->x1;
		btn_settings->y1 = btn_filter->y1 + btn_filter->h + interval;
		btn_settings->visible = VISIBLE;

		calculate_window_position(win, WINDOW_POSITION_FULLSCREEN);
		local_snprintf_P(win->title, ARRAY_SIZE(win->title), "FT8 terminal *** %d k *** %02d:%02d:%02d", ft8_bands [gui_nvram.ft8_band] / 1000, hour, minute, seconds);

		if (! work)
		{
			backup_freq = hamradio_get_freq_rx();
			backup_mode = hamradio_get_submode();
			viewtemp = hamradio_get_viewstyle();
			hamradio_settemp_viewstyle(VIEW_LINE);

			hamradio_set_freq(ft8_bands [gui_nvram.ft8_band]);
			hamradio_change_submode(ft8_mode, 0);
			memset(ft8.rx_text, '\0', ft8_text_records * ft8_text_length);
			ft8_set_state(1);
		}
		work = 0;

		hamradio_set_lockmode(1);
		display2_set_page_temp(display_getpagegui());
	}

	if (parse_ft8buf)
	{
		parse_ft8buf = 0;
		selected_label_cq = 255;
		text_field_t * tf_ft8 = (text_field_t*) find_gui_element(TYPE_TEXT_FIELD, win, "tf_ft8");

		memset(cq_call, 0, sizeof(cq_call));

		for (unsigned i = 0; i < ft8.decoded_messages; i ++)
		{
			char * msg = ft8.rx_text [i];
			COLORPIP_T colorline;
			unsigned cq_flag = 0;
			parse_ft8_answer(msg, & colorline, & cq_flag);
			if (cq_filter)
			{
				if (cq_flag)
					textfield_add_string(tf_ft8, msg, colorline);
			}
			else
				textfield_add_string(tf_ft8, msg, colorline);
		}

		update = 1;
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_tx = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_tx");
			button_t * btn_filter = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_filter");
			button_t * btn_bands = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_bands");
			button_t * btn_settings = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_settings");

			if (bh == btn_tx)
			{
				strcpy(ft8.tx_text, lh_array_tx [selected_label_tx].ptr->text);
				ft8.tx_freq = (float) gui_nvram.ft8_txfreq_val;
				ft8_do_encode();
			}
			else if (bh == btn_filter)
			{
				cq_filter = ! cq_filter;
				local_snprintf_P(btn_filter->text, ARRAY_SIZE(btn_filter->text), "%s", cq_filter ? "View|CQ only" : "View|all");
			}
			else if (bh == btn_settings)
			{
				work = 1;
				window_t * win2 = get_win(WINDOW_FT8_SETTINGS);
				open_window(win2);
			}
			else if (bh == btn_bands)
			{
				work = 1;
				window_t * win2 = get_win(WINDOW_FT8_BANDS);
				open_window(win2);
			}
		}
		else if (IS_LABEL_PRESS)
		{
			label_t * lh = (label_t *) ptr;
			if (lh->index < 10)
			{
				selected_label_cq = lh->index;
				labels_tx_update = 1;
			}
			else
				selected_label_tx = lh->index - 10;
			update = 1;
		}

		break;

	case WM_MESSAGE_CLOSE:

		if (! work)
		{
			hamradio_set_freq(backup_freq);
			hamradio_change_submode(backup_mode, 0);
			hamradio_settemp_viewstyle(viewtemp);
			ft8_set_state(0);
			save_settings();
		}
		display2_set_page_temp(display_getpage0());

		break;

	case WM_MESSAGE_UPDATE:

		update = 1;
		break;

	default:

		break;
	}

	if (update)
	{
		update = 0;

		for (unsigned i = 0; i < 6; i ++)
		{
			label_t * lh = lh_array_cq [i].ptr;
			lh->visible = NON_VISIBLE;
			lh->color = COLORPIP_WHITE;
			if (strlen(cq_call [i]))
			{
				strcpy(lh->text, cq_call [i]);
				lh->visible = VISIBLE;

				if (lh->index == selected_label_cq)
					lh->color = COLORPIP_YELLOW;
			}
		}

		for (unsigned i = 0; i < 4; i ++)
		{
			label_t * lh = lh_array_tx [i].ptr;

			if (i == selected_label_tx)
				lh->color = COLORPIP_YELLOW;
			else
				lh->color = COLORPIP_WHITE;
		}

		if (labels_tx_update)
		{
			labels_tx_update = 0;
			local_snprintf_P(lh_array_tx [1].ptr->text, ARRAY_SIZE(lh_array_tx [1].ptr->text), "%s %s %s", cq_call [selected_label_cq], gui_nvram.ft8_callsign, gui_nvram.ft8_qth);
			local_snprintf_P(lh_array_tx [2].ptr->text, ARRAY_SIZE(lh_array_tx [2].ptr->text), "%s %s %s", cq_call [selected_label_cq], gui_nvram.ft8_callsign, gui_nvram.ft8_snr);
			local_snprintf_P(lh_array_tx [3].ptr->text, ARRAY_SIZE(lh_array_tx [3].ptr->text), "%s %s %s", cq_call [selected_label_cq], gui_nvram.ft8_callsign, gui_nvram.ft8_end);
		}

		local_snprintf_P(win->title, ARRAY_SIZE(win->title), "FT8 terminal *** %d k *** %02d:%02d:%02d", ft8_bands [gui_nvram.ft8_band] / 1000, hour, minute, seconds);
	}
}

#endif /* WITHFT8 */

// *********************************************************************************************************************************************************************

static void window_af_process(void)
{
	window_t * const win = get_win(WINDOW_AF);
	static enc_var_t bp_t;

	if (win->first_call)
	{
		win->first_call = 0;
		unsigned interval = 50;
		bp_t.change = 0;
		bp_t.updated = 1;

		static const button_t buttons [] = {
			{ 40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_AF, NON_VISIBLE, -1, "btn_low_m", 	"-", },
			{ 40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_AF, NON_VISIBLE, 1,  "btn_low_p", 	"+", },
			{ 40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_AF, NON_VISIBLE, -1, "btn_high_m", "-", },
			{ 40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_AF, NON_VISIBLE, 1,  "btn_high_p", "+", },
			{ 40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_AF, NON_VISIBLE, -1, "btn_afr_m", 	"-", },
			{ 40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_AF, NON_VISIBLE, 1,  "btn_afr_p", 	"+", },
			{ 40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_AF, NON_VISIBLE, -1, "btn_ifshift_m", "-", },
			{ 40, 40, CANCELLED, BUTTON_NON_LOCKED, 1, 0, WINDOW_AF, NON_VISIBLE, 1,  "btn_ifshift_p", "+", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		static const label_t labels [] = {
			{ WINDOW_AF, CANCELLED, 0, NON_VISIBLE, "lbl_low",     "Low  cut : **** ", FONT_MEDIUM, COLORPIP_WHITE, TYPE_BP_LOW, 	},
			{ WINDOW_AF, CANCELLED, 0, NON_VISIBLE, "lbl_high",    "High cut : **** ", FONT_MEDIUM, COLORPIP_WHITE, TYPE_BP_HIGH, 	},
			{ WINDOW_AF, CANCELLED, 0, NON_VISIBLE, "lbl_afr",     "AFR      : **** ", FONT_MEDIUM, COLORPIP_WHITE, TYPE_AFR, 		},
			{ WINDOW_AF, CANCELLED, 0, NON_VISIBLE, "lbl_ifshift", "IF shift : **** ", FONT_MEDIUM, COLORPIP_WHITE, TYPE_IF_SHIFT, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t*) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		label_t * lbl_low = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_low");
		label_t * lbl_high = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_high");
		label_t * lbl_afr = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_afr");
		label_t * lbl_ifshift = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_ifshift");

		button_t * bh = & win->bh_ptr [0];

		lbl_low->x = 0;
		lbl_low->y = bh->h / 2;
		lbl_low->visible = VISIBLE;

		lbl_high->x = lbl_low->x;
		lbl_high->y = lbl_low->y + interval;
		lbl_high->visible = VISIBLE;

		lbl_afr->x = lbl_high->x;
		lbl_afr->y = lbl_high->y + interval;
		lbl_afr->visible = VISIBLE;

		lbl_ifshift->x = lbl_afr->x;
		lbl_ifshift->y = lbl_afr->y + interval;
		lbl_ifshift->visible = VISIBLE;

		unsigned x = lbl_low->x + get_label_width(lbl_low);
		unsigned y = lbl_low->y + get_label_height(lbl_low) / 2 - bh->h / 2;

		for (unsigned i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + bh->w + 10;
			if (r >= 2)
			{
				r = 0;
				x = lbl_low->x + get_label_width(lbl_low);
				y = y + interval;
			}
		}

		hamradio_enable_encoder2_redirect();
		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_low_m = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_low_m");
			button_t * btn_low_p = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_low_p");
			button_t * btn_high_m = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_high_m");
			button_t * btn_high_p = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_high_p");
			button_t * btn_afr_m = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_afr_m");
			button_t * btn_afr_p = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_afr_p");
			button_t * btn_ifshift_m = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_ifshift_m");
			button_t * btn_ifshift_p = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_ifshift_p");

			if (bh == btn_low_m || bh == btn_low_p)
			{
				bp_t.select = TYPE_BP_LOW;
				bp_t.change = bh->payload;
				bp_t.updated = 1;
			}
			else if (bh == btn_high_m || bh == btn_high_p)
			{
				bp_t.select = TYPE_BP_HIGH;
				bp_t.change = bh->payload;
				bp_t.updated = 1;
			}
			else if (bh == btn_afr_m || bh == btn_afr_p)
			{
				bp_t.select = TYPE_AFR;
				bp_t.change = bh->payload;
				bp_t.updated = 1;
			}
			else if (bh == btn_ifshift_m || bh == btn_ifshift_p)
			{
				bp_t.select = TYPE_IF_SHIFT;
				bp_t.change = bh->payload;
				bp_t.updated = 1;
			}
		}
		else if (IS_LABEL_PRESS)
		{
			label_t * lh = (label_t *) ptr;
			bp_t.select = lh->index;
			bp_t.change = 0;
			bp_t.updated = 1;
		}
		break;

	case WM_MESSAGE_ENC2_ROTATE:

		bp_t.change = action;
		bp_t.updated = 1;
		break;

	case WM_MESSAGE_UPDATE:

		bp_t.change = 0;
		bp_t.updated = 1;
		break;

	default:

		break;
	}

	if (bp_t.updated)
	{
		bp_t.updated = 0;

		char str_low [TEXT_ARRAY_SIZE], str_high [TEXT_ARRAY_SIZE];
		const unsigned bp_wide = hamradio_get_bp_type_wide();
		if (bp_wide)						// BWSET_WIDE
		{
			strcpy(str_low,  "Low  cut ");
			strcpy(str_high, "High cut ");
		}
		else								// BWSET_NARROW
		{
			strcpy(str_low,  "Width    ");
			strcpy(str_high, "Pitch    ");
		}

		for(unsigned i = 0; i < win->lh_count; i ++)
			win->lh_ptr [i].color = COLORPIP_WHITE;

		ASSERT(bp_t.select < win->lh_count);
		win->lh_ptr [bp_t.select].color = COLORPIP_YELLOW;

		label_t * const lbl_low = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_low");
		label_t * const lbl_high = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_high");
		label_t * const lbl_afr = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_afr");
		label_t * const lbl_ifshift = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_ifshift");

		unsigned val_low = hamradio_get_low_bp(bp_t.select == TYPE_BP_LOW ? (bp_t.change * 5) : 0);
		local_snprintf_P(lbl_low->text, ARRAY_SIZE(lbl_low->text), PSTR("%s: %4u"), str_low, val_low * 10);

		unsigned val_high = hamradio_get_high_bp(bp_t.select == TYPE_BP_HIGH ? bp_t.change : 0) * (bp_wide ? 100 : 10);
		local_snprintf_P(lbl_high->text, ARRAY_SIZE(lbl_high->text), PSTR("%s: %4u"), str_high, val_high);

		local_snprintf_P(lbl_afr->text, ARRAY_SIZE(lbl_afr->text), PSTR("AFR      : %+4d "),
				hamradio_afresponce(bp_t.select == TYPE_AFR ? bp_t.change : 0));

		int shift = hamradio_if_shift(bp_t.select == TYPE_IF_SHIFT ? bp_t.change : 0);
		if (shift)
			local_snprintf_P(lbl_ifshift->text, ARRAY_SIZE(lbl_ifshift->text), PSTR("IF shift :%+5d"), shift);
		else
			local_snprintf_P(lbl_ifshift->text, ARRAY_SIZE(lbl_ifshift->text), PSTR("IF shift :  OFF"));

		bp_t.change = 0;
	}
}

// *********************************************************************************************************************************************************************

static void window_mode_process(void)
{
	window_t * const win = get_win(WINDOW_MODES);

	if (win->first_call)
	{
		unsigned x = 0, y, interval = 6, row_count = 4, id_start, id_end;
		win->first_call = 0;

		static const button_t buttons [] = {
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_LSB, "btn_ModeLSB", "LSB", },
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_CW,  "btn_ModeCW",  "CW",  },
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_AM,  "btn_ModeAM",  "AM",  },
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGL, "btn_ModeDGL", "DGL", },
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_USB, "btn_ModeUSB", "USB", },
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_CWR, "btn_ModeCWR", "CWR", },
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_NFM, "btn_ModeNFM", "NFM", },
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGU, "btn_ModeDGU", "DGU", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		y = 0;

		for (unsigned i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;
			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = 0;
				y = 0 + bh->h + interval;
			}
		}

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;

			if (bh->payload != INT32_MAX)
				hamradio_change_submode(bh->payload, 1);

			close_window(DONT_OPEN_PARENT_WINDOW);
			footer_buttons_state(CANCELLED);
			return;
		}
		break;

	default:

		break;
	}
}

// *********************************************************************************************************************************************************************

static void window_receive_process(void)
{
	window_t * const win = get_win(WINDOW_RECEIVE);
	unsigned update = 0;

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 6, row_count = 3;
		win->first_call = 0;
		update = 1;

		static const button_t buttons [] = {
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_RECEIVE, NON_VISIBLE, INT32_MAX, "btn_att",    "", 			},
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_RECEIVE, NON_VISIBLE, INT32_MAX, "btn_AGC", 	 "", 			},
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_RECEIVE, NON_VISIBLE, INT32_MAX, "btn_mode",   "Mode", 		},
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_RECEIVE, NON_VISIBLE, INT32_MAX, "btn_preamp", "", 			},
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_RECEIVE, NON_VISIBLE, INT32_MAX, "btn_AF",  	 "AF|filter", 	},
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_RECEIVE, NON_VISIBLE, INT32_MAX, "btn_DNR",    "DNR", 		},
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 1, WINDOW_RECEIVE, NON_VISIBLE, INT32_MAX, "btn_WNB",    "WNB", 		},
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = 0;
		y = 0;

		for (unsigned i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = 0;
				y = y + bh->h + interval;
			}
		}

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_att = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_att");
			button_t * btn_preamp = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_preamp");
			button_t * btn_AF = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_AF");
			button_t * btn_AGC = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_AGC");
			button_t * btn_mode = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_mode");
			button_t * btn_DNR = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_DNR");
			button_t * btn_WNB = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_WNB");

			if (bh == btn_att)
			{
				hamradio_change_att();
			}
			else if (bh == btn_preamp)
			{
				hamradio_change_preamp();
			}
			else if (bh == btn_AF)
			{
				window_t * const win = get_win(WINDOW_AF);
				open_window(win);
			}
			else if (bh == btn_AGC)
			{
				btn_AGC->payload ? hamradio_set_agc_slow() : hamradio_set_agc_fast();
				btn_AGC->payload = hamradio_get_agc_type();
			}
			else if (bh == btn_mode)
			{
				window_t * const win = get_win(WINDOW_MODES);
				open_window(win);
			}
			else if (bh == btn_DNR)
			{
				hamradio_change_nr();
			}
#if WITHWNB
			else if (bh == btn_WNB)
			{
				btn_WNB->is_locked = wnb_state_switch();
			}
#endif /* WITHWNB */
		}
		else if (IS_BUTTON_LONG_PRESS)			// обработка длинного нажатия
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_WNB = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_WNB");
#if WITHWNB
			if (bh == btn_WNB)
			{
				open_window(get_win(WINDOW_WNBCONFIG));
			}
#endif /* WITHWNB */
		}
		break;

	case WM_MESSAGE_UPDATE:

		update = 1;
		break;

	default:

		break;
	}

	if (update)
	{
		button_t * btn_att = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_att");
		const char * a = remove_start_line_spaces(hamradio_get_att_value());
		local_snprintf_P(btn_att->text, ARRAY_SIZE(btn_att->text), PSTR("Att|%s"), a == NULL ? "off" : a);

		button_t * btn_preamp = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_preamp");
		const char * p = remove_start_line_spaces(hamradio_get_preamp_value());
		local_snprintf_P(btn_preamp->text, ARRAY_SIZE(btn_preamp->text), PSTR("Preamp|%s"), p == NULL ? "off" : "on");

		button_t * btn_AGC = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_AGC");
		local_snprintf_P(btn_AGC->text, ARRAY_SIZE(btn_AGC->text), PSTR("AGC|%s"), btn_AGC->payload ? "fast" : "slow");

		button_t * btn_DNR = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_DNR");
		int_fast32_t grade = 0;
		btn_DNR->is_locked = hamradio_get_nrvalue(& grade) != 0;
	}
}

// *********************************************************************************************

static void window_freq_process (void)
{
	static label_t * lbl_freq;
	static editfreq_t editfreq;
	window_t * const win = get_win(WINDOW_FREQ);
	unsigned update = 0;

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 6, row_count = 3;
		win->first_call = 0;
		button_t * bh = NULL;

		static const button_t buttons [] = {
			{ 50, 50, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_FREQ, NON_VISIBLE, 1, 		 		"btn_Freq1",  "1", },
			{ 50, 50, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_FREQ, NON_VISIBLE, 2, 		 		"btn_Freq2",  "2", },
			{ 50, 50, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_FREQ, NON_VISIBLE, 3, 		 		"btn_Freq3",  "3", },
			{ 50, 50, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_FREQ, NON_VISIBLE, 4, 	 			"btn_Freq4",  "4", },
			{ 50, 50, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_FREQ, NON_VISIBLE, 5, 				"btn_Freq5",  "5", },
			{ 50, 50, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_FREQ, NON_VISIBLE, 6, 				"btn_Freq6",  "6", },
			{ 50, 50, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_FREQ, NON_VISIBLE, 7, 				"btn_Freq7",  "7", },
			{ 50, 50, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_FREQ, NON_VISIBLE, 8,  			"btn_Freq8",  "8", },
			{ 50, 50, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_FREQ, NON_VISIBLE, 9, 		 		"btn_Freq9",  "9", },
			{ 50, 50, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_FREQ, NON_VISIBLE, BUTTON_CODE_BK, "btn_FreqBK", "<-", },
			{ 50, 50, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_FREQ, NON_VISIBLE, 0, 	 			"btn_Freq0",  "0", },
			{ 50, 50, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_FREQ, NON_VISIBLE, BUTTON_CODE_OK, "btn_FreqOK", "OK", },

		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		for (unsigned i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = 0;
				y = y + bh->h + interval;
			}
		}

		bh = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_FreqOK");
		bh->is_locked = BUTTON_LOCKED;

		editfreq.val = 0;
		editfreq.num = 0;
		editfreq.key = BUTTON_CODE_DONE;

		local_snprintf_P(win->title, ARRAY_SIZE(win->title), "%d k", (int) editfreq.val);

		calculate_window_position(win, WINDOW_POSITION_AUTO);
		window_set_title_align(win, TITLE_ALIGNMENT_CENTER);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS && editfreq.key == BUTTON_CODE_DONE)
		{
			button_t * bh = (button_t *) ptr;
			editfreq.key = bh->payload;
			update = 1;
		}
		break;

	default:

		break;
	}

	if (update)
	{
		update = 0;

		if (editfreq.key != BUTTON_CODE_DONE)
		{
			if (editfreq.error)
			{
				editfreq.val = 0;
				editfreq.num = 0;
			}

			editfreq.error = 0;

			switch (editfreq.key)
			{
			case BUTTON_CODE_BK:
				if (editfreq.num > 0)
				{
					editfreq.val /= 10;
					editfreq.num --;
				}
				break;

			case BUTTON_CODE_OK:
				if (hamradio_set_freq(editfreq.val * 1000) || editfreq.val == 0)
				{
					close_all_windows();
				}
				else
					editfreq.error = 1;

				break;

			default:
				if (editfreq.num < 6)
				{
					editfreq.val  = editfreq.val * 10 + editfreq.key;
					if (editfreq.val)
						editfreq.num ++;
				}
			}
			editfreq.key = BUTTON_CODE_DONE;

			if (editfreq.error)
				local_snprintf_P(win->title, ARRAY_SIZE(win->title), "ERROR");
			else
				local_snprintf_P(win->title, ARRAY_SIZE(win->title), "%d k", (int) editfreq.val);
		}
	}
}

// *****************************************************************************************************************************

static void window_time_process(void)
{
#if defined (RTC1_TYPE) && ! LINUX_SUBSYSTEM
	window_t * const win = get_win(WINDOW_TIME);
	static unsigned year, month, day, hour, minute, second, update;

	if (win->first_call)
	{
		win->first_call = 0;
		update = 1;
		uint8_t interval = 60;

		static const label_t labels [] = {
			{	WINDOW_TIME, DISABLED,  0, NON_VISIBLE, "lbl_year",   "****", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_TIME, DISABLED,  0, NON_VISIBLE, "lbl_month", 	"**", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_TIME, DISABLED,  0, NON_VISIBLE, "lbl_day",  	"**", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_TIME, DISABLED,  0, NON_VISIBLE, "lbl_hour",   	"**", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_TIME, DISABLED,  0, NON_VISIBLE, "lbl_minute",   "**", FONT_MEDIUM, COLORPIP_WHITE, },
			{	WINDOW_TIME, DISABLED,  0, NON_VISIBLE, "lbl_second", 	"**", FONT_MEDIUM, COLORPIP_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t*) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		static const button_t buttons [] = {
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TIME, NON_VISIBLE, 1, 	"btn_year+",  "Y+", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TIME, NON_VISIBLE, -1,	"btn_year-",  "Y-", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TIME, NON_VISIBLE, 1, 	"btn_month+",  "M+", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TIME, NON_VISIBLE, -1,	"btn_month-",  "M-", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TIME, NON_VISIBLE, 1, 	"btn_day+",  "D+", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TIME, NON_VISIBLE, -1,	"btn_day-",  "D-", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TIME, NON_VISIBLE, 1, 	"btn_hour+",  "h+", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TIME, NON_VISIBLE, -1,	"btn_hour-",  "h-", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TIME, NON_VISIBLE, 1, 	"btn_minute+",  "m+", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TIME, NON_VISIBLE, -1,	"btn_minute-",  "m-", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TIME, NON_VISIBLE, 1, 	"btn_second+",  "s+", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TIME, NON_VISIBLE, -1,	"btn_second-",  "s-", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TIME, NON_VISIBLE, 0,		"btn_sec0",  "s00", },
			{  40, 40, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_TIME, NON_VISIBLE, 0,		"btn_set",  "Set", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		button_t * bh = NULL;
		label_t * lh = NULL;
		unsigned x = 0, y = 0;

		for (unsigned i = 0; i < win->bh_count; i ++)
		{
			bh = & win->bh_ptr[i];

			if (i % 2 == 0)
			{
				if (bh->payload != 0)
				{
					lh = & win->lh_ptr[i / 2];
					lh->x = x + bh->w / 2 - get_label_width(lh) / 2;
					lh->y = 0;
					lh->visible = VISIBLE;
				}
				y += lh->y + get_label_height(lh) * 2;
			}

			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			y += interval;

			if ((i + 1) % 2 == 0)
			{
				y = 0;
				x += interval;
			}
		}

		button_t * btn_set =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_set");
		btn_set->is_locked = BUTTON_LOCKED;

		board_rtc_cached_getdatetime(& year, & month, & day, & hour, & minute,	& second);
		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_yearp =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_year+");
			button_t * btn_yearn =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_year-");
			button_t * btn_monthp =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_month+");
			button_t * btn_monthn =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_month-");
			button_t * btn_dayp =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_day+");
			button_t * btn_dayn =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_day-");
			button_t * btn_hourp =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_hour+");
			button_t * btn_hourn =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_hour-");
			button_t * btn_minutep =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_minute+");
			button_t * btn_minuten =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_minute-");
			button_t * btn_secondp =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_second+");
			button_t * btn_secondn =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_second-");
			button_t * btn_sec0 =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_sec0");
			button_t * btn_set =  (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_set");

			if (bh == btn_yearp || bh == btn_yearn)
				year += bh->payload;
			else if (bh == btn_monthp || bh == btn_monthn)
				month += bh->payload;
			else if (bh == btn_dayp || bh == btn_dayn)
				day += bh->payload;
			else if (bh == btn_hourp || bh == btn_hourn)
				hour += bh->payload;
			else if (bh == btn_minutep || bh == btn_minuten)
				minute += bh->payload;
			else if (bh == btn_secondp || bh == btn_secondn)
				second += bh->payload;
			else if (bh == btn_sec0)
				second = 0;
			else if (bh == btn_set)
			{
				board_rtc_setdatetime(year, month, day, hour, minute, second);
				close_all_windows();
				return;
			}

			update = 1;
		}
		break;

	default:

		break;
	}

	if (update)
	{
		update = 0;

		label_t * lh =  (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_year");
		local_snprintf_P(lh->text, ARRAY_SIZE(lh->text), "%d", year);

		lh =  (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_month");
		local_snprintf_P(lh->text, ARRAY_SIZE(lh->text), "%02d", month);

		lh =  (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_day");
		local_snprintf_P(lh->text, ARRAY_SIZE(lh->text), "%02d", day);

		lh =  (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_hour");
		local_snprintf_P(lh->text, ARRAY_SIZE(lh->text), "%02d", hour);

		lh =  (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_minute");
		local_snprintf_P(lh->text, ARRAY_SIZE(lh->text), "%02d", minute);

		lh =  (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_second");
		local_snprintf_P(lh->text, ARRAY_SIZE(lh->text), "%02d", second);
	}
#endif /* defined (RTC1_TYPE) && ! LINUX_SUBSYSTEM */
}

static void window_kbd_process(void)
{
	window_t * const win = get_win(WINDOW_KBD);
	static unsigned update = 0, is_shift = 0;
	const char kbd_cap[] = ",.!@#*()?;QWERTYUIOPASDFGHJKLZXCVBNM";
	const char kbd_low[] = "1234567890qwertyuiopasdfghjklzxcvbnm";
	const unsigned len1 = 10, len2 = 10, len3 = 9, len4 = 7, kbd_len = len1 + len2 + len3 + len4, btn_size = 40;
	const char kbd_func [][20] = { "btn_kbd_caps", "btn_kbd_backspace", "btn_kbd_space", "btn_kbd_enter", "btn_kbd_esc", };
	static char edit_str[TEXT_ARRAY_SIZE];

	if (win->first_call)
	{
		win->first_call = 0;
		update = 1;
		is_shift = 0;
		button_t * bh = NULL;
		unsigned x = 0, y = 0, interval = 5, i = 0;

		if (gui_keyboard.clean || gui_keyboard.digits_only)
			memset(edit_str, 0, strlen(edit_str) * sizeof(char));
		else
			strncpy(edit_str, gui_keyboard.str, gui_keyboard.max_len);

		win->bh_count = kbd_len + 5;
		unsigned buttons_size = win->bh_count * sizeof (button_t);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);

		for (; i < win->bh_count; i ++)
		{
			if (i == len1)
			{
				x = bh->w / 2;
				y += bh->h + interval;
			}
			else if (i == len1 + len2)
			{
				x = bh->w + interval;
				y += bh->h + interval;
			}
			else if (i == len1 + len2 + len3)
			{
				x = bh->w * 2;
				y += bh->h + interval;
			}

			bh = & win->bh_ptr[i];
			bh->x1 = x;
			bh->y1 = y;
			bh->w = btn_size;
			bh->h = btn_size;
			bh->state = (gui_keyboard.digits_only && i > len1 - 1 && i < kbd_len) ? DISABLED : CANCELLED;
			bh->visible = VISIBLE;
			bh->parent = WINDOW_KBD;
			bh->index = i;
			bh->is_long_press = 0;
			bh->is_repeating = 0;
			bh->is_locked = BUTTON_NON_LOCKED;
			if (i < kbd_len)
				local_snprintf_P(bh->name, ARRAY_SIZE(bh->name), "btn_kbd_%02d", i);
			else
				local_snprintf_P(bh->name, ARRAY_SIZE(bh->name), "%s", kbd_func [i - kbd_len]);

			x += bh->w + interval;
		}

		button_t * btn_kbd_caps = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_kbd_caps");
		btn_kbd_caps->x1 = 0;
		btn_kbd_caps->y1 = btn_size * 3 + interval * 3;
		btn_kbd_caps->w = 75;
		local_snprintf_P(btn_kbd_caps->text, ARRAY_SIZE(btn_kbd_caps->text), "CAPS");
		btn_kbd_caps->is_locked = is_shift ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		btn_kbd_caps->state = gui_keyboard.digits_only ? DISABLED : CANCELLED;

		button_t * btn_kbd_backspace = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_kbd_backspace");
		btn_kbd_backspace->x1 = btn_size * len1 + interval * len1;
		btn_kbd_backspace->y1 = 0;
		btn_kbd_backspace->is_repeating = 1;
		local_snprintf_P(btn_kbd_backspace->text, ARRAY_SIZE(btn_kbd_backspace->text), "<-");

		button_t * btn_kbd_space = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_kbd_space");
		btn_kbd_space->x1 = btn_kbd_caps->x1 + btn_kbd_caps->w + interval * 8 + btn_size * len4;
		btn_kbd_space->y1 = btn_kbd_caps->y1;
		btn_kbd_space->w = 95;
		local_snprintf_P(btn_kbd_space->text, ARRAY_SIZE(btn_kbd_space->text), "Space");
		btn_kbd_space->payload = (char) ' ';
		btn_kbd_space->state = gui_keyboard.digits_only ? DISABLED : CANCELLED;

		button_t * btn_kbd_esc = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_kbd_esc");
		btn_kbd_esc->x1 = btn_kbd_backspace->x1 + btn_kbd_backspace->w + interval * 3;
		btn_kbd_esc->y1 = btn_kbd_backspace->y1 + btn_kbd_backspace->h / 2;
		btn_kbd_esc->w = 50;
		btn_kbd_esc->h = 50;
		local_snprintf_P(btn_kbd_esc->text, ARRAY_SIZE(btn_kbd_esc->text), "Esc");

		button_t * btn_kbd_enter = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_kbd_enter");
		btn_kbd_enter->x1 = btn_kbd_esc->x1;
		btn_kbd_enter->y1 = btn_kbd_esc->y1 + btn_kbd_esc->h + interval * 3;
		btn_kbd_enter->w = 50;
		btn_kbd_enter->h = 50;
		local_snprintf_P(btn_kbd_enter->text, ARRAY_SIZE(btn_kbd_enter->text), "OK");
		btn_kbd_enter->is_locked = BUTTON_LOCKED;

		local_snprintf_P(win->title, ARRAY_SIZE(win->title), "%s_", edit_str);
		calculate_window_position(win, WINDOW_POSITION_AUTO);
		window_set_title_align(win, TITLE_ALIGNMENT_CENTER);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_kbd_esc = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_kbd_esc");
			button_t * btn_kbd_caps = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_kbd_caps");
			button_t * btn_kbd_enter = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_kbd_enter");
			button_t * btn_kbd_space = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_kbd_space");
			button_t * btn_kbd_backspace = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_kbd_backspace");

			if ((bh->index < kbd_len || bh == btn_kbd_space))
			{
				char text [2];
				text [0] = (char) bh->payload;
				text [1] = '\0';
				if (gui_keyboard.digits_only)
				{
					char tmp_str[TEXT_ARRAY_SIZE];
					strcpy(tmp_str, edit_str);
					strcat(tmp_str, text);
					if (strtoul(tmp_str, NULL, 10) < UINT32_MAX)
					{
						strcat(edit_str, text);
					}
				}
				else if (strlen(edit_str) < gui_keyboard.max_len)
				{
					strcat(edit_str, text);
				}
			}
			else if (bh == btn_kbd_caps)
			{
				is_shift = ! is_shift;
				btn_kbd_caps->is_locked = is_shift ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
				update = 1;
			}
			else if (bh == btn_kbd_backspace)
			{
				unsigned l = strlen(edit_str);
				if (l)
					edit_str [l - 1] = '\0';
			}
			else if (bh == btn_kbd_enter || bh == btn_kbd_esc)
			{
				if (bh == btn_kbd_enter)
				{
					if (gui_keyboard.digits_only)
						* gui_keyboard.num = strtoul(edit_str, NULL, 10);
					else
						strcpy(gui_keyboard.str, edit_str);
				}

				close_window(OPEN_PARENT_WINDOW);
				return;
			}

			local_snprintf_P(win->title, ARRAY_SIZE(win->title), "%s_", edit_str);
		}
		break;

	default:
	break;
	}

	if (update)
	{
		update = 0;

		for(unsigned i = 0; i < kbd_len; i ++)
		{
			button_t * bh = & win->bh_ptr[i];
			char text [2];
			text [0] = is_shift ? kbd_cap [i] : kbd_low [i];
			text [1] = '\0';
			local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), "%s", text);
			bh->payload = (char) text [0];
		}
	}
}

// *********************************************************************************************************************************************************************

static void window_kbd_test_process(void)
{
	window_t * const win = get_win(WINDOW_KBD_TEST);
	static uint32_t num_lbl1 = 12345;
	static char str_lbl2 [TEXT_ARRAY_SIZE] = "qwertyuiopas";
	const unsigned win_id = WINDOW_KBD_TEST;

	if (win->first_call)
	{
		win->first_call = 0;
		const unsigned interval = 50;

		static const button_t buttons [] = {
			{ 86, 30, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_KBD_TEST, VISIBLE, INT32_MAX, "btn_text", "Edit...", },
			{ 86, 30, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_KBD_TEST, VISIBLE, INT32_MAX, "btn_num", "Edit...", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		static const label_t labels [] = {
			{ WINDOW_KBD_TEST, DISABLED, 0, VISIBLE, "lbl_text", "********************", FONT_MEDIUM, COLORPIP_WHITE, },
			{ WINDOW_KBD_TEST, DISABLED, 0, VISIBLE, "lbl_num",  "********************", FONT_MEDIUM, COLORPIP_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t*) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		button_t * btn_text = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_text");
		button_t * btn_num = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_num");
		label_t * lbl_text = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_text");
		label_t * lbl_num = (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_num");

		lbl_text->x = 0;
		lbl_text->y = 0;

		lbl_num->x = 0;
		lbl_num->y = get_label_height(lbl_text) + interval;

		btn_text->x1 = get_label_width(lbl_text) + interval;
		btn_text->y1 = 0;

		btn_num->x1 = btn_text->x1;
		btn_num->y1 = lbl_num->y;

		local_snprintf_P(lbl_num->text, ARRAY_SIZE(lbl_num->text), "%u", (unsigned) num_lbl1);
		strcpy(lbl_text->text, str_lbl2);

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;

			if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_text"))
				keyboard_edit_string((uintptr_t) & str_lbl2, 10, 0);					// передается строка длиной 12
			else if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_num"))
				keyboard_edit_digits(& num_lbl1);
		}
		break;

	default:
	break;
	}
}

// *****************************************************************************************************************************

static void window_ping_process(void)
{
#if WITHLWIP
	window_t * const win = get_win(WINDOW_PING);
	static unsigned is_ping = 0, ping_delay = 0, update = 0;
	static char ip_str [20] = "8.8.8.8";
	static text_field_t * tf_ping = NULL;

	if (win->first_call)
	{
		win->first_call = 0;
		uint_fast8_t interval = 20;
		is_ping = 0;
		ping_delay = 0;
		update = 0;

		add_element("btn_edit", 86, 30, 0, 0, "Set IP");
		add_element("btn_ping", 86, 30, 0, 0, "Ping");
		add_element("lbl_ip", 0, FONT_MEDIUM, COLORPIP_WHITE, 16);
		add_element("tf_ping", 35, 20, UP, & gothic_11x13);

		tf_ping = (text_field_t*) find_gui_element(TYPE_TEXT_FIELD, win, "tf_ping");
		textfield_update_size(tf_ping);
		tf_ping->x1 = 0;
		tf_ping->y1 = 0;
		tf_ping->visible = VISIBLE;

		label_t * lbl_ip =  (label_t*) find_gui_element(TYPE_LABEL, win, "lbl_ip");
		lbl_ip->x = tf_ping->x1 + tf_ping->w + interval;
		lbl_ip->y = 0;
		lbl_ip->visible = VISIBLE;

		button_t * btn_edit = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_edit");
		btn_edit->x1 = lbl_ip->x;
		btn_edit->y1 = lbl_ip->y + get_label_height(lbl_ip) + interval;
		btn_edit->visible = VISIBLE;

		button_t * btn_ping = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_ping");
		btn_ping->x1 = btn_edit->x1;
		btn_ping->y1 = btn_edit->y1 + btn_edit->h + interval;
		btn_ping->visible = VISIBLE;

		calculate_window_position(win, WINDOW_POSITION_AUTO);
		local_snprintf_P(lbl_ip->text, ARRAY_SIZE(lbl_ip->text), PSTR("%s"), ip_str);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_edit = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_edit");
			button_t * btn_ping = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_ping");

			if (bh == btn_ping)
			{
				if(is_ping)
					is_ping = 0;
				else
				{
					int val = ping_send_ip(ip_str);
					if (val)
					{
						char str[30];
						local_snprintf_P(str, ARRAY_SIZE(str), PSTR("Ping %s error=%d"), ip_str, val);
						textfield_add_string(tf_ping, str, COLORPIP_RED);
					}
					else
					{
						is_ping = 1;
						ping_delay = 0;
						textfield_clean(tf_ping);
					}
				}
				update = 1;
			}
			else if (bh == btn_edit)
			{
				keyboard_edit_string((uintptr_t) & ip_str, 20, 0);
			}
		}
		break;

	default:

		break;
	}

	if (is_ping)
	{
		if (ping_delay > 80)
		{
			ping_delay = 0;
			int resp = ping_check_response();

			if (resp)
			{
				char str[30];
				local_snprintf_P(str, ARRAY_SIZE(str), PSTR("Answer from %s: %d ms"), ip_str, resp);
				textfield_add_string(tf_ping, str, COLORPIP_WHITE);

				int send = ping_send_ip(ip_str);
				if(send)
				{
					char str[30];
					local_snprintf_P(str, ARRAY_SIZE(str), PSTR("Ping %s error=%d"), ip_str, send);
					textfield_add_string(tf_ping, str, COLORPIP_RED);
					is_ping = 0;
					update = 1;
				}
			}
		}

		ping_delay ++;
	}

	if (update)
	{
		update = 0;
		button_t * btn_edit = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_edit");
		button_t * btn_ping = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_ping");
		btn_ping->is_locked = is_ping;
		btn_edit->state = is_ping ? DISABLED : CANCELLED;
	}
#endif
}

// *****************************************************************************************************************************

/* https://www.a1k0n.net/2011/07/20/donut-math.html */

static void window_3d_process(void)
{
	window_t * const win = get_win(WINDOW_3D);
	static text_field_t * tf_3d = NULL;
	static float A = 0, B = 0, z[1760];
	char b[22][80];

	if (win->first_call)
	{
		win->first_call = 0;

		static const text_field_t text_field [] = {
			{ 60, 22, CANCELLED, WINDOW_3D, VISIBLE, DOWN, & gothic_11x13, "tf_3d", },
		};
		win->tf_count = ARRAY_SIZE(text_field);
		unsigned tf_size = sizeof(text_field);
		win->tf_ptr = (text_field_t*) malloc(tf_size);
		GUI_MEM_ASSERT(win->tf_ptr);
		memcpy(win->tf_ptr, text_field, tf_size);

		tf_3d = (text_field_t*) find_gui_element(TYPE_TEXT_FIELD, win, "tf_3d");
		textfield_update_size(tf_3d);
		tf_3d->x1 = 0;
		tf_3d->y1 = 0;

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	if (win->state == VISIBLE)
	{
		memset(b,' ', 80 * 22 * sizeof(char));
		memset(z, 0, 1760 * sizeof(float));
		for(float j = 0; 6.28 > j; j += 0.07)
		{
			for(float i = 0; 6.28 > i; i += 0.02)
			{
				float c = arm_sin_f32(i), d = arm_cos_f32(j), e = arm_sin_f32(A), f = arm_sin_f32(j),
						g = arm_cos_f32(A), h = d + 2, D = 1 / (c * h * e + f * g + 5), l = arm_cos_f32(i),
						m = arm_cos_f32(B), n = arm_sin_f32(B), t = c * h * g - f * e;
				int x = 30 + 30 * D * (l * h * m - t * n), y = 12 + 15 * D * (l * h * n + t * m),
						o = x + 80 * y, N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);
				if(22 > y && y > 0 && x > 0 && 80 > x && D > z[o])
				{
					z [o] = D;
					b [y][x] = ".,-~:;=!*#$@" [N > 0 ? N : 0];
				}
			}
		}

		textfield_clean(tf_3d);
		for (int k = 0; k < 22; k ++)
			textfield_add_string(tf_3d, b[k], COLORPIP_WHITE);

		A += 0.08;
		B += 0.04;
	}

	GET_FROM_WM_QUEUE
	{
	default:
		break;
	}
}

// *****************************************************************************************************************************

#define MENU_GROUPS_MAX	30
#define MENU_PARAMS_MAX	50
static unsigned index_param = 0;

static void window_menu_params_process(void)
{
	window_t * const win = get_win(WINDOW_MENU_PARAMS);
	static menu_names_t menup [MENU_PARAMS_MAX], menuv;
	static button_t * bh_sel = NULL;
	static unsigned sel = 0;

	if (win->first_call)
	{
		win->first_call = 0;
		sel = 0;

		unsigned interval = 6, x = 0, y = 0, xmax = 0;
		button_t * bh = NULL;

		const unsigned count = hamradio_get_multilinemenu_block_params(menup, index_param, MENU_PARAMS_MAX);
		unsigned cols = count <= 16 ? 4 : 5;

		win->bh_count = count;
		unsigned buttons_size = win->bh_count * sizeof (button_t);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);

		for (unsigned i = 0; i < win->bh_count; i ++)
		{
			bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->w = 130;
			bh->h = 35;
			bh->state = CANCELLED;
			bh->visible = VISIBLE;
			bh->parent = WINDOW_MENU_PARAMS;
			bh->index = i;
			bh->is_long_press = 0;
			bh->is_repeating = 0;
			bh->is_locked = BUTTON_NON_LOCKED;
			local_snprintf_P(bh->name, ARRAY_SIZE(bh->name), PSTR("btn_params_%02d"), i);
			remove_end_line_spaces(menup[i].name);
			local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), menup[i].name);
			bh->payload = menup[i].index;

			xmax = x > xmax ? x : xmax;

			x = x + interval + bh->w;
			if ((i + 1) % cols == 0)
			{
				x = 0;
				y = y + bh->h + interval;
			}
		}

#if 1	// Добавить экранные кнопки "+" и "-" при необходимости
		add_element("btn_p", 35, 35, 0, 0, "+");
		add_element("btn_m", 35, 35, 0, 0, "-");

		button_t * btn_p = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_p");
		btn_p->x1 = xmax + 130 + interval;
		btn_p->y1 = 0;
		btn_p->visible = VISIBLE;
		btn_p->payload = 1;
		btn_p->index = 90;

		button_t * btn_m = (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_m");
		btn_m->x1 = xmax + 130 + interval;
		btn_m->y1 = 40;
		btn_m->visible = VISIBLE;
		btn_m->payload = -1;
		btn_m->index = 91;
#endif

		calculate_window_position(win, WINDOW_POSITION_AUTO);
		local_snprintf_P(win->title, ARRAY_SIZE(win->title), "Edit param: choose...");
		window_set_title_align(win, TITLE_ALIGNMENT_CENTER);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;

			if (bh->index < 90)
			{
				if (bh_sel)
					bh_sel->is_locked = BUTTON_NON_LOCKED;

				bh->is_locked = BUTTON_LOCKED;
				bh_sel = bh;
				sel = 1;

				hamradio_get_multilinemenu_block_vals(& menuv, bh->payload, 1);
				remove_end_line_spaces(menuv.name);
				local_snprintf_P(win->title, ARRAY_SIZE(win->title), "%s: %s", bh_sel->text, menuv.name);
			}
			else if (bh->index == 90 || bh->index == 91)
				gui_set_encoder2_rotate(bh->payload);
		}
		break;

	case WM_MESSAGE_ENC2_ROTATE:
	{
		char edit_val [20];
		if (sel)
		{
			strcpy(edit_val, hamradio_gui_edit_menu_item(menuv.index, action));
			remove_end_line_spaces(edit_val);
			local_snprintf_P(win->title, ARRAY_SIZE(win->title), "%s: %s", bh_sel->text, edit_val);
		}
	}
		break;

	default:
	break;
	}
}

static void window_menu_process(void)
{
	window_t * const win = get_win(WINDOW_MENU);

	if (win->first_call)
	{
		win->first_call = 0;
		unsigned x = 0, y = 0;
		button_t * bh = NULL;
		menu_names_t menu [MENU_GROUPS_MAX];
		const unsigned cols = 4, interval = 6;

		const unsigned count = hamradio_get_multilinemenu_block_groups(menu);
		ASSERT(count < MENU_GROUPS_MAX);

		win->bh_count = count;
		unsigned buttons_size = win->bh_count * sizeof (button_t);
		win->bh_ptr = (button_t*) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);

		for (unsigned i = 0; i < win->bh_count; i ++)
		{
			bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->w = 130;
			bh->h = 35;
			bh->state = CANCELLED;
			bh->visible = VISIBLE;
			bh->parent = WINDOW_MENU;
			bh->index = i;
			bh->is_long_press = 0;
			bh->is_repeating = 0;
			bh->is_locked = BUTTON_NON_LOCKED;
			local_snprintf_P(bh->name, ARRAY_SIZE(bh->name), PSTR("btn_groups_%02d"), i);
			remove_end_line_spaces(menu[i].name);
			local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), menu[i].name);
			bh->payload = menu[i].index;

			x = x + interval + bh->w;
			if ((i + 1) % cols == 0)
			{
				x = 0;
				y = y + bh->h + interval;
			}
		}
		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			index_param = bh->payload;
			window_t * const win = get_win(WINDOW_MENU_PARAMS);
			open_window(win);
		}
		break;

	default:
	break;
	}
}

// *****************************************************************************************************************************

static void window_lfm_process(void)
{
#if WITHLFM
	window_t * const win = get_win(WINDOW_LFM);
	static unsigned update = 0;
	static enc_var_t enc;

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 24;
		win->first_call = 0;
		update = 1;
		enc.select = 2;
		enc.change = 0;
		enc.updated = 1;

		static const label_t labels [] = {
			{ WINDOW_LFM, DISABLED,  0, VISIBLE, "lbl_nmeatime",		"NMEA time:", 	FONT_MEDIUM, COLORPIP_WHITE, 0, },
			{ WINDOW_LFM, DISABLED,  0, VISIBLE, "lbl_nmeatime_val",	"00:00:00", 	FONT_MEDIUM, COLORPIP_GREEN, 1, },
			{ WINDOW_LFM, CANCELLED, 0, VISIBLE, "lbl_timeoffset",		"Time offset:",	FONT_MEDIUM, COLORPIP_WHITE, 2, },
			{ WINDOW_LFM, CANCELLED, 0, VISIBLE, "lbl_timeoffset_val",	"xx sec",		FONT_MEDIUM, COLORPIP_WHITE, 3, },
			{ WINDOW_LFM, CANCELLED, 0, VISIBLE, "lbl_stopfreq",    	"Stop freq:",	FONT_MEDIUM, COLORPIP_WHITE, 4, },
			{ WINDOW_LFM, CANCELLED, 0, VISIBLE, "lbl_stopfreq_val",	"xx MHz",		FONT_MEDIUM, COLORPIP_WHITE, 5, },
			{ WINDOW_LFM, CANCELLED, 0, VISIBLE, "lbl_interval",    	"Interval:",	FONT_MEDIUM, COLORPIP_WHITE, 6, },
			{ WINDOW_LFM, CANCELLED, 0, VISIBLE, "lbl_interval_val",	"xx sec",		FONT_MEDIUM, COLORPIP_WHITE, 7, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t*) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		add_element("btn_state", 86, 40, 0, 0, "");
		add_element("btn_draw", 86, 40, 0, 0, "Draw|spectre");

		for (unsigned i = 0; i < win->lh_count; i += 2)
		{
			label_t * lh1 = & win->lh_ptr [i];
			label_t * lh2 = & win->lh_ptr [i + 1];

			lh1->x = x;
			lh1->y = y;
			lh1->visible = VISIBLE;

			lh2->x = x + 130;
			lh2->y = y;
			lh2->visible = VISIBLE;

			y = y + get_label_height(lh1) + interval;
		}

		button_t * btn_state = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_state");
		btn_state->x1 = 5;
		btn_state->y1 = y;
		btn_state->visible = VISIBLE;

		button_t * btn_draw = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_draw");
		btn_draw->x1 = btn_state->x1 + btn_state->w + interval;
		btn_draw->y1 = y;
		btn_draw->visible = VISIBLE;

		hamradio_enable_encoder2_redirect();
		enable_window_move(win);
		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;

			if (bh == (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_state"))
			{
				if (bh->is_locked)
				{
					hamradio_lfm_disable();
					bh->is_locked = 0;
				}
				else
					hamradio_set_lfmmode(! hamradio_get_lfmmode());

				update = 1;
			}
			else if (bh == (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_draw"))
			{
				open_window(get_win(WINDOW_LFM_SPECTRE));
			}
		}
		else if (IS_LABEL_PRESS)
		{
			label_t * lh = (label_t *) ptr;
			enc.select = lh->index;
			enc.change = 0;
			enc.updated = 1;
		}
		break;

	case WM_MESSAGE_ENC2_ROTATE:

		enc.change = action;
		enc.updated = 1;
		break;

	case WM_MESSAGE_UPDATE:

		update = 1;
		break;

	default:
		break;
	}

	if (enc.updated)
	{
		enc.updated = 0;

		for(unsigned i = 2; i < win->lh_count; i ++)
			win->lh_ptr [i].color = COLORPIP_WHITE;

		ASSERT(enc.select < win->lh_count);

		if (enc.select == 2 || enc.select == 3)
		{
			win->lh_ptr [2].color = COLORPIP_YELLOW;
			win->lh_ptr [3].color = COLORPIP_YELLOW;
			uint_fast16_t v = hamradio_get_lfmtoffset();
			hamradio_set_lfmtoffset(v + enc.change);
		}
		else if (enc.select == 4 || enc.select == 5)
		{
			win->lh_ptr [4].color = COLORPIP_YELLOW;
			win->lh_ptr [5].color = COLORPIP_YELLOW;
			uint_fast16_t v = hamradio_get_lfmstop100k();
			hamradio_set_lfmstop100k(v + enc.change * 10);
		}
		else if (enc.select == 6 || enc.select == 7)
		{
			win->lh_ptr [6].color = COLORPIP_YELLOW;
			win->lh_ptr [7].color = COLORPIP_YELLOW;
			uint_fast16_t v = hamradio_get_lfmtinterval();
			hamradio_set_lfmtinterval(v + enc.change);
		}

		update = 1;
	}

	if (update)
	{
		update = 0;

		button_t * btn_state = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_state");
		btn_state->is_locked = iflfmactive() != 0;
		const char states[3][9] = { "Disabled", "Standby", "Scan..." };
		uint_fast16_t s = hamradio_get_lfmmode() == 0 ? 0 : btn_state->is_locked ? 2 : 1;
		local_snprintf_P(btn_state->text, ARRAY_SIZE(btn_state->text), states[s]);

		button_t * btn_draw = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_draw");
		btn_draw->state = s == 2 ? CANCELLED : DISABLED;

		label_t * lbl_nmeatime_val = (label_t *) find_gui_element(TYPE_LABEL, win, "lbl_nmeatime_val");
		hamradio_get_nmea_time(lbl_nmeatime_val->text, ARRAY_SIZE(lbl_nmeatime_val->text));

		label_t * lbl_timeoffset_val = (label_t *) find_gui_element(TYPE_LABEL, win, "lbl_timeoffset_val");
		local_snprintf_P(lbl_timeoffset_val->text, ARRAY_SIZE(lbl_timeoffset_val->text), "%02d sec", hamradio_get_lfmtoffset());

		label_t * lbl_stopfreq_val = (label_t *) find_gui_element(TYPE_LABEL, win, "lbl_stopfreq_val");
		local_snprintf_P(lbl_stopfreq_val->text, ARRAY_SIZE(lbl_stopfreq_val->text), "%02d MHz", hamradio_get_lfmstop100k() / 10);

		label_t * lbl_interval_val = (label_t *) find_gui_element(TYPE_LABEL, win, "lbl_interval_val");
		local_snprintf_P(lbl_interval_val->text, ARRAY_SIZE(lbl_interval_val->text), "%02d sec", hamradio_get_lfmtinterval());
	}
#endif /* WITHLFM  */
}

static void window_lfm_spectre_process(void)
{
#if WITHLFM
	const unsigned xmax = 600, ymax = 200, x1 = (800 / 2) - 100, x2 = (800 / 2) + 100;
	window_t * const win = get_win(WINDOW_LFM_SPECTRE);
	static COLORPIP_T d[600][200];
	static int shift = 0;

	static unsigned x = 0;

	if (win->first_call)
	{
		win->first_call = 0;
		x = 0;
		shift = 0;

		memset(d, 0, sizeof(d));
		calculate_window_position(win, WINDOW_POSITION_MANUAL_SIZE, xmax, ymax);
		return;
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ENC2_ROTATE:

		shift += (action * 5);
		break;

	default:
		break;
	}

	for (int j = 0; j < xmax; j ++)
		for (int k = 0; k < x2 - x1; k ++)
			gui_drawpoint(j, k, d[j][k]);

	if (x < xmax)
	{
		for (int i = x1, y = 0; i < x2; i ++, y ++)
		{
			COLORPIP_T v = display2_get_spectrum(i + shift);
			gui_drawpoint(x, y, v);
			d[x][y] = v;
		}

		x ++;
	}
#endif /* WITHLFM  */
}

#if WITHEXTIO_LAN

static text_field_t * tf_log = NULL;

void stream_log(char * str)
{
	if (tf_log && check_for_parent_window() == WINDOW_EXTIOLAN)
	{
		textfield_add_string(tf_log, str, COLORPIP_WHITE);
		put_to_wm_queue(get_win(WINDOW_EXTIOLAN), WM_MESSAGE_UPDATE);
	}
}

static void window_stream_process(void)
{
	window_t * const win = get_win(WINDOW_EXTIOLAN);
	static uint8_t update = 0;

	if (win->first_call)
	{
		win->first_call = 0;

		add_element("tf_log", 50, 15, DOWN, & gothic_11x13);
		add_element("btn_state", 130, 40, 0, 0, "");

		tf_log = (text_field_t *) find_gui_element(TYPE_TEXT_FIELD, win, "tf_log");
		textfield_update_size(tf_log);
		tf_log->x1 = 0;
		tf_log->y1 = 0;
		tf_log->visible = VISIBLE;

		button_t * btn_state = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_state");
		btn_state->x1 = tf_log->w / 2 - btn_state->w / 2;
		btn_state->y1 = tf_log->h + 10;
		btn_state->visible = VISIBLE;

		update = 1;
		calculate_window_position(win, WINDOW_POSITION_AUTO);
		return;
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;

			if (bh == (button_t*) find_gui_element(TYPE_BUTTON, win, "btn_state"))
			{
				uint8_t state = stream_get_state();

				if (state == 0)
					server_start();
				else
					server_stop();
			}
			update = 1;
		}

		break;

	case WM_MESSAGE_UPDATE:

		update = 1;
		break;

	default:
		break;
	}

	if (update)
	{
		update = 0;

		button_t * btn_state = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_state");
		const char * states[] = { "Start server", "Stop listening", "Disconnect &|stop server", };
		local_snprintf_P(btn_state->text, ARRAY_SIZE(btn_state->text), states[stream_get_state()]);
	}
}

#endif /* # WITHEXTIO_LAN */

#if WITHWNB

static void window_wnbconfig_process(void)
{
	window_t * const win = get_win(WINDOW_WNBCONFIG);
	static unsigned update = 0;
	static enc_var_t enc;

	if (win->first_call)
	{
		win->first_call = 0;
		unsigned x = 0, y = 0, interval = 24;

		static const label_t labels [] = {
			{ WINDOW_WNBCONFIG, CANCELLED, 0, VISIBLE, "lbl_wnbthreshold_name", "Threshold:  ", FONT_MEDIUM, COLORPIP_YELLOW, 0, },
			{ WINDOW_WNBCONFIG, CANCELLED, 0, VISIBLE, "lbl_wnbthreshold_val",  "xxxxx", 		FONT_MEDIUM, COLORPIP_YELLOW, 1, },
		};

		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t*) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		for (unsigned i = 0; i < win->lh_count; i += 2)
		{
			label_t * lh1 = & win->lh_ptr [i];
			label_t * lh2 = & win->lh_ptr [i + 1];

			lh1->x = x;
			lh1->y = y;
			lh1->visible = VISIBLE;

			lh2->x = x + 130;
			lh2->y = y;
			lh2->visible = VISIBLE;

			y = y + get_label_height(lh1) + interval;
		}

		hamradio_enable_encoder2_redirect();
		enable_window_move(win);
		calculate_window_position(win, WINDOW_POSITION_AUTO);
		update = 1;
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ENC2_ROTATE:

		enc.change = action;
		enc.updated = 1;
		break;

	case WM_MESSAGE_UPDATE:

		update = 1;
		break;

	default:
		break;
	}

	if (enc.updated)
	{
		uint_fast16_t v = wnb_get_threshold();
		wnb_set_threshold(v + enc.change);

		enc.updated = 0;
		update = 1;
	}

	if (update)
	{
		update = 0;

		label_t * lbl_wnbthreshold_val = (label_t *) find_gui_element(TYPE_LABEL, win, "lbl_wnbthreshold_val");
		local_snprintf_P(lbl_wnbthreshold_val->text, ARRAY_SIZE(lbl_wnbthreshold_val->text), "%d", wnb_get_threshold());
	}
}

#endif /* WITHWNB */

#if WITHAD936XIIO

static void window_iioconfig_process(void)
{
	window_t * const win = get_win(WINDOW_IIOCONFIG);
	static unsigned update = 0;
	static int status = 10, gain_mode = 0, gain_val = 20;

	const char * status_str[3] = { "AD936x found", "Error", "Streaming" };
	const char * button_str[3] = { "Start", "Find", "Stop" };
	const char * gainmode_str[2] = { "Gain|manual", "Gain|auto" };
	static char uri[20] = "usb:";

	if (win->first_call)
	{
		win->first_call = 0;
		unsigned x = 0, y = 0, interval = 24;

		add_element("lbl_status", 0, FONT_MEDIUM, COLORPIP_WHITE, 9);
		add_element("lbl_status_str", 0, FONT_MEDIUM, COLORPIP_WHITE, 20);
		add_element("lbl_iio_name", 0, FONT_MEDIUM, COLORPIP_WHITE, 9);
		add_element("lbl_iio_val", 0, FONT_MEDIUM, COLORPIP_WHITE, 20);

		add_element("btn_uri_edit", 86, 44, 0, 0, "Edit...");
		add_element("btn_action", 86, 44, 0, 0, "Find");
		add_element("btn_gain_type", 86, 44, 0, 0, "Gain|manual");
		add_element("btn_gain_add", 44, 44, 0, 0, "+");
		add_element("btn_gain_sub", 44, 44, 0, 0, "-");

		label_t * lbl_status = (label_t *) find_gui_element(TYPE_LABEL, win, "lbl_status");
		lbl_status->x = 0;
		lbl_status->y = 0;
		lbl_status->visible = VISIBLE;
		local_snprintf_P(lbl_status->text, ARRAY_SIZE(lbl_status->text), "Status: ");

		label_t * lbl_status_str = (label_t *) find_gui_element(TYPE_LABEL, win, "lbl_status_str");
		lbl_status_str->x = get_label_width(lbl_status) + 10;
		lbl_status_str->y = lbl_status->y;
		lbl_status_str->visible = VISIBLE;
		memset(lbl_status_str->text, 0, ARRAY_SIZE(lbl_status_str->text));

		label_t * lbl_iio_name = (label_t *) find_gui_element(TYPE_LABEL, win, "lbl_iio_name");
		lbl_iio_name->x = 0;
		lbl_iio_name->y = get_label_height(lbl_status) + 10;
		lbl_iio_name->visible = VISIBLE;
		local_snprintf_P(lbl_iio_name->text, ARRAY_SIZE(lbl_iio_name->text), "URI:    ");

		label_t * lbl_iio_val = (label_t *) find_gui_element(TYPE_LABEL, win, "lbl_iio_val");
		lbl_iio_val->x = get_label_width(lbl_iio_name) + 10;
		lbl_iio_val->y = lbl_iio_name->y;
		lbl_iio_val->visible = VISIBLE;

		button_t * btn_uri_edit = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_uri_edit");
		btn_uri_edit->x1 = 0;
		btn_uri_edit->y1 = lbl_iio_name->y + get_label_height(lbl_iio_name) + 10;
		btn_uri_edit->visible = VISIBLE;

		button_t * btn_action = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_action");
		btn_action->x1 = btn_uri_edit->x1 + btn_uri_edit->w + 10;
		btn_action->y1 = btn_uri_edit->y1;
		btn_action->visible = VISIBLE;

		button_t * btn_gain_type = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_gain_type");
		btn_gain_type->x1 = btn_action->x1;
		btn_gain_type->y1 = btn_action->y1 + btn_action->h + 10;
		btn_gain_type->visible = VISIBLE;

		button_t * btn_gain_sub = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_gain_sub");
		btn_gain_sub->x1 = btn_gain_type->x1 - btn_gain_sub->w - 10;
		btn_gain_sub->y1 = btn_gain_type->y1;
		btn_gain_sub->visible = VISIBLE;
		btn_gain_sub->payload = -1;

		button_t * btn_gain_add = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_gain_add");
		btn_gain_add->x1 = btn_gain_type->x1 + btn_gain_type->w + 10;
		btn_gain_add->y1 = btn_gain_type->y1;
		btn_gain_add->visible = VISIBLE;
		btn_gain_add->payload = 1;

		enable_window_move(win);
		calculate_window_position(win, WINDOW_POSITION_AUTO);
		update = 1;
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

	if (IS_BUTTON_PRESS)
	{
		button_t * bh = (button_t *) ptr;

		if (bh == (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_uri_edit"))
		{
			keyboard_edit_string((uintptr_t) & uri, ARRAY_SIZE(uri), 0);
		}
		else if (bh == (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_action"))
		{
			if (status == 10 || status == 1)
				status = gui_ad936x_find(uri);
			else if (status == 0)
				status = gui_ad936x_start();
			else if (status == 2)
				status = gui_ad936x_stop();

			update = 1;
		}
		else if (bh == (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_gain_type"))
		{
			gain_mode = ! gain_mode;
			gui_ad936x_set_gain(gain_mode, gain_val);
			update = 1;
		}
		else if (bh == (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_gain_add") ||
				bh == (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_gain_sub"))
		{
			if (gain_val + bh->payload > 3 || gain_val + bh->payload < 70)
			{
				gain_val += bh->payload;
				gui_ad936x_set_gain(gain_mode, gain_val);
				update = 1;
			}

		}
	}

	break;

	case WM_MESSAGE_UPDATE:

		update = 1;
		break;

	default:
		break;
	}

	if (update)
	{
		update = 0;

		label_t * lbl_iio_val = (label_t *) find_gui_element(TYPE_LABEL, win, "lbl_iio_val");
		local_snprintf_P(lbl_iio_val->text, ARRAY_SIZE(lbl_iio_val->text), "%s", uri);

		button_t * btn_gain_type = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_gain_type");
		btn_gain_type->state = status != 2 ? DISABLED : CANCELLED;
		if (gain_mode)
			local_snprintf_P(btn_gain_type->text, ARRAY_SIZE(btn_gain_type->text), "Gain|auto");
		else
			local_snprintf_P(btn_gain_type->text, ARRAY_SIZE(btn_gain_type->text), "Gain|%d dB", gain_val);

		button_t * btn_gain_add = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_gain_add");
		btn_gain_add->state = (status == 2 && ! gain_mode) ? CANCELLED : DISABLED;

		button_t * btn_gain_sub = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_gain_sub");
		btn_gain_sub->state = (status == 2 && ! gain_mode) ? CANCELLED : DISABLED;

		if (status < 10)
		{
			label_t * lbl_status_str = (label_t *) find_gui_element(TYPE_LABEL, win, "lbl_status_str");
			local_snprintf_P(lbl_status_str->text, ARRAY_SIZE(lbl_status_str->text), "%s",  status_str[status]);

			button_t * btn_action = (button_t *) find_gui_element(TYPE_BUTTON, win, "btn_action");
			local_snprintf_P(btn_action->text, ARRAY_SIZE(btn_action->text), "%s", button_str[status]);
		}
	}
}

#endif /* WITHAD936XIIO */

#endif /* WITHTOUCHGUI */
