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
#include "gui_windows.h"
#include "gui_objects.h"

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
	if ((uint8_t) gui_nvram.ft8_callsign [0] == UINT8_MAX)
		local_snprintf_P(gui_nvram.ft8_callsign, ARRAY_SIZE(gui_nvram.ft8_callsign), "RA4ASN");

	if ((uint8_t) gui_nvram.ft8_snr [0] == UINT8_MAX)
		local_snprintf_P(gui_nvram.ft8_snr, ARRAY_SIZE(gui_nvram.ft8_snr), "-20");

	if ((uint8_t) gui_nvram.ft8_qth [0] == UINT8_MAX)
		local_snprintf_P(gui_nvram.ft8_qth, ARRAY_SIZE(gui_nvram.ft8_qth), "LO10");

	if ((uint8_t) gui_nvram.ft8_end [0] == UINT8_MAX)
		local_snprintf_P(gui_nvram.ft8_end, ARRAY_SIZE(gui_nvram.ft8_end), "RR73");

	if ((uint8_t) gui_nvram.ft8_band == UINT8_MAX)
		gui_nvram.ft8_band = ft8_band_default;

	if (gui_nvram.ft8_txfreq_val == UINT32_MAX)
		gui_nvram.ft8_txfreq_val = ft8_txfreq_default;

	if (gui_nvram.ft8_txfreq_equal == UINT8_MAX)
		gui_nvram.ft8_txfreq_equal = ft8_txfreq_equal_default;
#endif /* WITHFT8 */

	if (gui_nvram.micprofile != micprofile_default && gui_nvram.micprofile < NMICPROFCELLS)
		hamradio_load_mic_profile(gui_nvram.micprofile, 1);
	else
		gui_nvram.micprofile = micprofile_default;

#if WITHAD936XIIO
	if (hamradio_get_freq_rx() >= NOXVRTUNE_TOP && ! get_ad936x_stream_status())
		hamradio_set_freq(7012000uL);
#endif /* WITHAD936XIIO */
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
			textfield_add_string_old(tf_debug, str, COLORPIP_WHITE);
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

void gui_user_actions_after_close_window(void)
{
	hamradio_disable_encoder2_redirect();
	hamradio_set_lock(0);
}

static void keyboard_edit_string(uintptr_t s, unsigned strlen, unsigned clean)
{
	gui_keyboard.str = (char *) s;
	gui_keyboard.clean = clean;
	gui_keyboard.max_len = strlen;
	gui_keyboard.digits_only = 0;
	window_t * win_kbd = get_win(WINDOW_KBD);
	win_kbd->parent_id = get_parent_window();
	open_window(win_kbd);
}

static void keyboard_edit_digits(int * val)
{
	gui_keyboard.num = val;
	gui_keyboard.digits_only = 1;
	window_t * win_kbd = get_win(WINDOW_KBD);
	win_kbd->parent_id = get_parent_window();
	open_window(win_kbd);
}

// *********************************************************************************************************************************************************************

void window_infobar_menu_process(void)
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
		win->bh_ptr = (button_t *) malloc(buttons_size);
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
				button_t * bh = (button_t *) find_gui_obj(TYPE_BUTTON, win, btn_name);
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
				button_t * bh = (button_t *) find_gui_obj(TYPE_BUTTON, win, btn_name);

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
				button_t * bh = (button_t *) find_gui_obj(TYPE_BUTTON, win, btn_name);
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
				button_t * bh = (button_t *) find_gui_obj(TYPE_BUTTON, win, btn_name);
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

			hamradio_change_nr(1);
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
					footer_buttons_state(DISABLED, NULL);
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
					footer_buttons_state(DISABLED, NULL);
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
			footer_buttons_state(DISABLED, NULL);
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

/* Установки статуса основных кнопок */
/* При DISABLED в качестве необязательного параметра передать указатель на активную кнопку или NULL для блокирования всех */
void footer_buttons_state (uint_fast8_t state, ...)
{
	window_t * win = get_win(WINDOW_MAIN);
	va_list arg;
	button_t * bt = NULL;
	uint_fast8_t is_name;
	static uint_fast16_t bitmask_locked_buttons = 0;

	if (state == DISABLED)
	{
		va_start(arg, state);
		bt = va_arg(arg, button_t *);
		va_end(arg);
	}

	if (state == DISABLED && bt != NULL)
		bitmask_locked_buttons = 0;

	for (uint_fast8_t i = 0; i < win->bh_count; i ++)
	{
		button_t * bh = & win->bh_ptr [i];

		if (state == DISABLED && bt != NULL)
			bitmask_locked_buttons |= bh->is_locked << i;

		if (state == DISABLED)
		{
			bh->state = bh == bt ? CANCELLED : DISABLED;
			bh->is_locked = bh->state == CANCELLED ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		}
		else if (state == CANCELLED && get_parent_window() == NO_PARENT_WINDOW)
		{
			bh->state = CANCELLED;
			bh->is_locked = ((bitmask_locked_buttons >> i) & 1) ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		}
	}
}

void gui_main_process(void)
{
	window_t * const win = get_win(WINDOW_MAIN);

	const gxdrawb_t * db = gui_get_drawbuf();
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

		tf_debug = (text_field_t*) find_gui_obj(TYPE_TEXT_FIELD, win, "tf_debug");
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
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_var",  	 	"", 				},
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_ft8", 		"", 				},
			{ 86, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_Options", 	"Options", 			},
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t *) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

#if WITHFT8
		button_t * btn_ft8 = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_ft8");
		local_snprintf_P(btn_ft8->text, ARRAY_SIZE(btn_ft8->text), PSTR("FT8"));
#endif /* WITHFT8 */
#if WITHAUDIOSAMPLESREC
		button_t * btn_var = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_var");
		local_snprintf_P(btn_var->text, ARRAY_SIZE(btn_var->text), "AF|samples");
#endif /* WITHAUDIOSAMPLESREC */

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

		touch_area_t * ta_freq = (touch_area_t *) find_gui_obj(TYPE_TOUCH_AREA, win, "ta_freq");
		ta_freq->x1 = 0;
		ta_freq->y1 = infobar_2nd_str_y + SMALLCHARH2 + 5;
		ta_freq->w = WITHGUIMAXX;
		ta_freq->h = WITHGUIMAXY - FOOTER_HEIGHT - ta_freq->y1;
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
		objects_state(win);
		hamradio_set_lock(0);

#if WITHGUIDEBUG
		for (unsigned i = 0; i < tmpstr_index; i ++)
		{
			textfield_add_string_old(tf_debug, tmpbuf[i].text, COLORPIP_WHITE);
		}
#endif /* WITHGUIDEBUG */
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_AREA_MOVE)
		{
			touch_area_t * th = (touch_area_t *) ptr;
			touch_area_t * ta_freq = (touch_area_t *) find_gui_obj(TYPE_TOUCH_AREA, win, "ta_freq");

			if (th == ta_freq && gui_nvram.freq_swipe_enable && get_parent_window() == NO_PARENT_WINDOW)
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
				if (get_parent_window() == WINDOW_INFOBAR_MENU)
				{
					close_window(DONT_OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
				}
				else if (get_parent_window() == NO_PARENT_WINDOW)
				{
					window_t * const win = get_win(WINDOW_INFOBAR_MENU);
					open_window(win);
					footer_buttons_state(DISABLED, NULL);
				}
			}
		}
#endif /* GUI_SHOW_INFOBAR */

		if (IS_BUTTON_PRESS)	// обработка короткого нажатия кнопок
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_txrx = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_txrx");
			button_t * btn_notch = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_notch");
			button_t * btn_Bands = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_Bands");
			button_t * btn_Memory = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_Memory");
			button_t * btn_Options = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_Options");
			button_t * btn_speaker = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_speaker");
			button_t * btn_Receive = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_Receive");
			button_t * btn_var = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_var");
#if WITHFT8
			button_t * btn_ft8 = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_ft8");
#endif

			if (bh == btn_notch)
			{
				hamradio_set_gnotch(! hamradio_get_gnotch());
				update = 1;
			}
#if WITHFT8
			else if (bh == btn_ft8)
			{
				if (get_parent_window() != NO_PARENT_WINDOW)
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
				if (get_parent_window() != NO_PARENT_WINDOW)
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
				if (get_parent_window() != NO_PARENT_WINDOW)
				{
					close_window(OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
					hamradio_set_lock(0);
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
				if (get_parent_window() != NO_PARENT_WINDOW)
				{
					close_window(OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
					hamradio_set_lock(0);
					hamradio_disable_keyboard_redirect();
				}
				else
				{
					window_t * const win = get_win(WINDOW_RECEIVE);
					open_window(win);
					footer_buttons_state(DISABLED, btn_Receive);
				}
			}
#if WITHAUDIOSAMPLESREC
			else if (bh == btn_var)
			{
				window_t * const win = get_win(WINDOW_AS);
				if (win->state == NON_VISIBLE)
				{
					open_window(win);
					footer_buttons_state(DISABLED, btn_var);
				}
				else
				{
					close_window(OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
				}
			}
#endif /* WITHAUDIOSAMPLESREC */
#if WITHTX
			else if (bh == btn_txrx)
			{
				hamradio_gui_set_reqautotune2(0);
				hamradio_moxmode(1);
				update = 1;
			}
#endif /* WITHTX */
		}
		else if (IS_BUTTON_LONG_PRESS)			// обработка длинного нажатия
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_txrx = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_txrx");
			button_t * btn_notch = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_notch");
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

		button_t * btn_notch = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_notch");
		btn_notch->is_locked = hamradio_get_gnotch() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		unsigned notch_type = hamradio_get_gnotchtype();
		if (notch_type == 1)
			local_snprintf_P(btn_notch->text, ARRAY_SIZE(btn_notch->text), PSTR("Notch|manual"));
		else if (notch_type == 2)
			local_snprintf_P(btn_notch->text, ARRAY_SIZE(btn_notch->text), PSTR("Notch|auto"));

		button_t * btn_speaker = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_speaker");
#if WITHSPKMUTE
		btn_speaker->is_locked = hamradio_get_gmutespkr() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(btn_speaker->text, ARRAY_SIZE(btn_speaker->text), PSTR("Speaker|%s"), btn_speaker->is_locked ? "muted" : "on air");
#else
		btn_speaker->state = DISABLED;
		local_snprintf_P(btn_speaker->text, ARRAY_SIZE(btn_speaker->text), PSTR("Speaker|on air"));
#endif /* #if WITHSPKMUTE */

		button_t * btn_txrx = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_txrx");
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

	const window_t * const pwin = get_win(get_parent_window());
	if (pwin->size_mode != WINDOW_POSITION_FULLSCREEN)
	{
		// разметка инфобара
		const unsigned y_mid = infobar_1st_str_y + (infobar_2nd_str_y - infobar_1st_str_y) / 2;
		const unsigned infobar_hl = (infobar_selected < infobar_num_places) && (get_parent_window() == WINDOW_INFOBAR_MENU);

		for(unsigned i = 1; i < infobar_num_places; i++)
		{
			uint_fast16_t x = infobar_label_width * i;
			colpip_line(db, x, infobar_1st_str_y, x, infobar_2nd_str_y + SMALLCHARH2, COLORPIP_GREEN, 0);
		}

		if (infobar_hl)
		{
			uint16_t x1 = infobar_selected * infobar_label_width + 2;
			uint16_t x2 = x1 + infobar_label_width - 4;

			colmain_rounded_rect(db, x1, infobar_1st_str_y, x2, infobar_2nd_str_y + SMALLCHARH2 - 2, 5, COLORPIP_YELLOW, 1);
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
				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, str_color);
				local_snprintf_P(buf, buflen, "VFO %s", hamradio_get_gvfoab() ? "2" : "1");
				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, str_color);
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
				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, str_color);
				if (hamradio_get_gmutespkr())
					local_snprintf_P(buf, buflen, PSTR("muted"));
				else
					local_snprintf_P(buf, buflen, PSTR("%d"), vol);
				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, str_color);
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
				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, str_color);
				local_snprintf_P(buf, buflen, PSTR("Tune %d\%%"), (int) tune_pwr);
				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, str_color);
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
				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, state ? str_color : COLORPIP_GRAY);
				local_snprintf_P(buf, buflen, state ? "on" : "off");
				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, state ? str_color : COLORPIP_GRAY);
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
				colpip_string2_tbg(db, xx, y_mid, buf, str_color);
				xx += SMALLCHARW2 * 3;
				local_snprintf_P(buf, buflen, bp_wide ? (PSTR("L %u")) : (PSTR("W %u")), bp_low);
				colpip_string2_tbg(db, xx, infobar_1st_str_y, buf, str_color);
				local_snprintf_P(buf, buflen, bp_wide ? (PSTR("H %u")) : (PSTR("P %u")), bp_high);
				colpip_string2_tbg(db, xx, infobar_2nd_str_y, buf, str_color);
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
					colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, str_color);
					local_snprintf_P(buf, buflen, if_shift == 0 ? PSTR("%d") : PSTR("%+d Hz"), if_shift);
					colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, str_color);
				}
				else
				{
					local_snprintf_P(buf, buflen, PSTR("IF shift"));
					colpip_string2_tbg(db, xx - strwidth2(buf) / 2, y_mid, buf, COLORPIP_GRAY);
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
				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, str_color);
				if (atten)
					local_snprintf_P(buf, buflen, PSTR("%d db"), atten);
				else
					local_snprintf_P(buf, buflen, PSTR("off"));

				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, str_color);
			}
				break;

			case INFOBAR_SPAN:
			// ширина панорамы
			{
	#if WITHIF4DSP
				static int_fast32_t z;
				uint_fast16_t xx;

				if (update)
					z = display2_zoomedbw() / 1000;
				local_snprintf_P(buf, buflen, PSTR("SPAN"));
				xx = current_place * infobar_label_width + infobar_label_width / 2;
				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, str_color);
				local_snprintf_P(buf, buflen, PSTR("%dk"), z);
				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, str_color);
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
				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, yy, buf, str_color);
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

					colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, str_color);
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
				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, COLORPIP_WHITE);
				local_snprintf_P(buf, buflen, PSTR("%2.1f"), cpu_temp);
				colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, cpu_temp > 60.0 ? COLORPIP_RED : COLORPIP_WHITE);
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
					colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, COLORPIP_WHITE);
					local_snprintf_P(buf, buflen, PSTR("%s"), gui_enc2_menu.val);
					remove_end_line_spaces(buf);
					COLORPIP_T color_lbl = gui_enc2_menu.state == 2 ? COLORPIP_YELLOW : COLORPIP_WHITE;
					colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, color_lbl);
				}
				else
				{
	#if defined (RTC1_TYPE)
					// текущее время
					local_snprintf_P(buf, buflen, PSTR("%02d.%02d"), day, month);
					unsigned xx = current_place * infobar_label_width + infobar_label_width / 2;
					colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_1st_str_y, buf, COLORPIP_WHITE);
					local_snprintf_P(buf, buflen, PSTR("%02d%c%02d"), hour, ((seconds & 1) ? ' ' : ':'), minute);
					colpip_string2_tbg(db, xx - strwidth2(buf) / 2, infobar_2nd_str_y, buf, COLORPIP_WHITE);
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
		display_transparency(db, tf_debug->x1 - 5, tf_debug->y1 - 5, tf_debug->x1 + tf_debug->w + 5, tf_debug->y1 + tf_debug->h + 5, DEFAULT_ALPHA);
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

void window_memory_process(void)
{
	window_t * const win = get_win(WINDOW_MEMORY);

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 6, row_count = 5;
		win->first_call = 0;

		win->bh_count = memory_cells_count;
		unsigned buttons_size = win->bh_count * sizeof (button_t);
		win->bh_ptr = (button_t *) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);

		gui_obj_create("lbl_note1", 0, FONT_MEDIUM, COLORPIP_WHITE, 30);

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
		label_t * lbl_note1 = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_note1");
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

void window_bands_process(void)
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

		gui_obj_create("lbl_ham", 0, FONT_LARGE, COLORPIP_WHITE, 10);
		gui_obj_create("lbl_bcast", 0, FONT_LARGE, COLORPIP_WHITE, 16);

		win->bh_count = bandnum + 1;
		unsigned buttons_size = win->bh_count * sizeof (button_t);
		win->bh_ptr = (button_t *) calloc(win->bh_count, sizeof (button_t));
		GUI_MEM_ASSERT(win->bh_ptr);

		label_t * lh1 = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_ham");
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

#if WITHAD936XIIO || WITHAD936XDEV
			if ((get_ad936x_stream_status() && bh->payload < NOXVRTUNE_TOP) ||
					(! get_ad936x_stream_status() && bh->payload > NOXVRTUNE_TOP))
				bh->state = DISABLED;
#endif /* WITHAD936XIIO || WITHAD936XDEV */

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = 0;
				y = y + bh->h + interval;
			}
		}

		label_t * lh2 = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_bcast");
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

#if WITHAD936XIIO || WITHAD936XDEV
			if ((get_ad936x_stream_status() && bh->payload < NOXVRTUNE_TOP) ||
					(! get_ad936x_stream_status() && bh->payload > NOXVRTUNE_TOP))
				bh->state = DISABLED;
#endif /* WITHAD936XIIO || WITHAD936XDEV */

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
			button_t * btn_Freq = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_freq");

			if (bh == btn_Freq)
			{
				window_t * const win = get_win(WINDOW_FREQ);
				open_window(win);
				hamradio_set_lock(1);
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

void window_options_process(void)
{
	window_t * const win = get_win(WINDOW_OPTIONS);

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 6, row_count = 4;
		win->first_call = 0;

		gui_obj_create("btn_SysMenu", 100, 44, 0, 0, "System|settings");
		gui_obj_create("btn_AUDsett", 100, 44, 0, 0, "Audio|settings");
		gui_obj_create("btn_TXsett",  100, 44, 0, 0, "Transmit|settings");
		gui_obj_create("btn_Display", 100, 44, 0, 0, "Display|settings");
		gui_obj_create("btn_gui", 	   100, 44, 0, 0, "GUI|settings");
		gui_obj_create("btn_Utils",   100, 44, 0, 0, "Utils");
#if WITHAD936XIIO || WITHAD936XDEV
		gui_obj_create("btn_936x",    100, 44, 0, 0, "AD936x");
#endif /* WITHAD936XIIO || WITHAD936XDEV */
#if defined (RTC1_TYPE)
		gui_obj_create("btn_Time",    100, 44, 0, 0, "Set time|& date");
#endif /* defined (RTC1_TYPE) */
#if LINUX_SUBSYSTEM
		gui_obj_create("btn_exit",		100, 44, 0, 0, "Terminate|program");
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
		button_t * btn_Display = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_Display");
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
			button_t * btn_gui = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_gui");
			button_t * btn_Utils = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_Utils");
			button_t * btn_TXsett = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_TXsett");
			button_t * btn_AUDsett = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_AUDsett");
			button_t * btn_SysMenu = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_SysMenu");
			button_t * btn_Display = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_Display");
#if defined (RTC1_TYPE)
			button_t * btn_Time = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_Time");
			if (bh == btn_Time)
			{
				window_t * const win = get_win(WINDOW_TIME);
				open_window(win);
			}
			else
#endif /* defined (RTC1_TYPE) */
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
			else if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_exit"))
			{
				linux_exit();		// Terminate all
			}
#endif /* LINUX_SUBSYSTEM */
#if WITHAD936XIIO || WITHAD936XDEV
			else if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_936x"))
			{
				open_window(get_win(WINDOW_AD936X));
			}
#endif /* WITHAD936XIIO || WITHAD936XDEV*/
		}
		break;

	default:

		break;
	}
}

// *********************************************************************************************************************************************************************

void window_display_process(void)
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
		win->bh_ptr = (button_t *) malloc(buttons_size);
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
		win->lh_ptr = (label_t *) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		label_t * lbl_sp = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_sp");
		label_t * lbl_wf = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_wf");
		label_t * lbl_topSP = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_topSP");
		label_t * lbl_bottomSP = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_bottomSP");
		label_t * lbl_topWF = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_topWF");
		label_t * lbl_bottomWF = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_bottomWF");

		button_t * btn_bottomSP_m = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_bottomSP_m");
		button_t * btn_bottomSP_p = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_bottomSP_p");
		button_t * btn_topSP_m = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_topSP_m");
		button_t * btn_topSP_p = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_topSP_p");
		button_t * btn_bottomWF_m = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_bottomWF_m");
		button_t * btn_bottomWF_p = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_bottomWF_p");
		button_t * btn_topWF_m = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_topWF_m");
		button_t * btn_topWF_p = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_topWF_p");

		button_t * btn_zoom = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_zoom");
		button_t * btn_view = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_view");
		button_t * btn_params = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_params");

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
			button_t * btn_bottomSP_m = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_bottomSP_m");
			button_t * btn_bottomSP_p = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_bottomSP_p");
			button_t * btn_topSP_m = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_topSP_m");
			button_t * btn_topSP_p = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_topSP_p");
			button_t * btn_bottomWF_m = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_bottomWF_m");
			button_t * btn_bottomWF_p = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_bottomWF_p");
			button_t * btn_topWF_m = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_topWF_m");
			button_t * btn_topWF_p = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_topWF_p");
			button_t * btn_zoom = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_zoom");
			button_t * btn_view = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_view");
			button_t * btn_params = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_params");

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

			display_t.select = lh->payload;
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

		button_t * btn_view = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_view");
		local_snprintf_P(btn_view->text, ARRAY_SIZE(btn_view->text), PSTR("View|%s"), hamradio_change_view_style(0));
		remove_end_line_spaces(btn_view->text);

		button_t * btn_zoom = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_zoom");
		local_snprintf_P(btn_zoom->text, ARRAY_SIZE(btn_zoom->text), PSTR("Zoom|x%d"), 1 << hamradio_get_gzoomxpow2());

		button_t * btn_params = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_params");
		local_snprintf_P(btn_params->text, ARRAY_SIZE(btn_params->text), PSTR("WF params|%s"), hamradio_get_gwflevelsep() ? "separate" : "from SP");

		for(unsigned i = 0; i < win->lh_count; i ++)
			win->lh_ptr [i].color = COLORPIP_WHITE;

		ASSERT(display_t.select < win->lh_count);
		win->lh_ptr [display_t.select].color = COLORPIP_YELLOW;

		label_t * lbl_topSP = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_topSP");
		label_t * lbl_bottomSP = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_bottomSP");
		label_t * lbl_topWF = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_topWF");
		label_t * lbl_bottomWF = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_bottomWF");

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

void window_utilites_process(void)
{
	window_t * const win = get_win(WINDOW_UTILS);

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 6, row_count = 4;
		win->first_call = 0;
#if WITHGUIDEBUG
		gui_obj_create("btn_debug", 100, 44, 0, 0, "Debug|view");
#endif /* WITHGUIDEBUG */
#if WITHSWRSCAN
		gui_obj_create("btn_SWRscan", 100, 44, 0, 0, "SWR|scanner");
#endif /* WITHSWRSCAN */
		gui_obj_create("btn_3d", 100, 44, 0, 0, "Donut|3d");
#if WITHLFM
		gui_obj_create("btn_lfm", 100, 44, 0, 0, "LFM|receive");
#endif /* WITHLFM  */
#if WITHIQSHIFT
		gui_obj_create("btn_shift", 100, 44, 0, 0, "IQ shift");
#endif /* WITHIQSHIFT */
#if LINUX_SUBSYSTEM && WITHEXTIO_LAN
		gui_obj_create("btn_stream", 100, 44, 0, 0, "IQ LAN|Stream");
#endif /* LINUX_SUBSYSTEM && WITHEXTIO_LAN */
#if 0
		gui_obj_create("btn_kbdtest", 100, 44, 0, 0, "Keyboard|test");
#endif

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
		button_t * bh = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_debug");
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

			if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_3d"))
			{
				open_window(get_win(WINDOW_3D));
			}
#if WITHSWRSCAN
			else if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_SWRscan"))
			{
				open_window(get_win(WINDOW_SWR_SCANNER));
			}
#endif /* WITHSWRSCAN */
#if WITHLFM
			else if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_lfm"))
			{
				open_window(get_win(WINDOW_LFM));
			}
#endif /* WITHLFM  */
#if WITHGUIDEBUG
			else if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_debug"))
			{
				gui_open_debug_window();
				close_all_windows();
			}
#endif /* WITHGUIDEBUG */
#if WITHIQSHIFT
			else if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_shift"))
			{
				open_window(get_win(WINDOW_SHIFT));
			}
#endif /* WITHIQSHIFT */
#if LINUX_SUBSYSTEM && WITHEXTIO_LAN
			else if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_stream"))
			{
				open_window(get_win(WINDOW_EXTIOLAN));
			}
#endif /* LINUX_SUBSYSTEM && WITHEXTIO_LAN */
			else if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_kbdtest"))
				open_window(get_win(WINDOW_KBD_TEST));
		}
		break;

	default:

		break;
	}
}

// *********************************************************************************************************************************************************************

void window_swrscan_process(void)
{
#if WITHSWRSCAN
	uint_fast16_t gr_w = 500, gr_h = 200;							// размеры области графика
	uint_fast8_t interval = 20;
	uint_fast16_t x0 = interval, y0 = gr_h;							// нулевые координаты графика
	uint_fast16_t x1 = x0 + gr_w - interval * 2, y1 = gr_h - y0;	// размеры осей графика
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
	const uint8_t averageFactor = 3;

	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		win->first_call = 0;
		button_t * bh = NULL;

		gui_obj_create("btn_swr_start", 86, 44, 0, 0, "Start");
		gui_obj_create("btn_swr_OK",    86, 44, 0, 0, "OK");

		gui_obj_create("lbl_swr_bottom", 0, FONT_SMALL, COLORPIP_WHITE, 16);
		gui_obj_create("lbl_swr_top",    0, FONT_SMALL, COLORPIP_WHITE, 16);
		gui_obj_create("lbl_swr_error",  0, FONT_MEDIUM, COLORPIP_WHITE, 16);

		mid_w = 0 + gr_w / 2;
		btn_swr_start = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_swr_start");
		btn_swr_start->x1 = mid_w - btn_swr_start->w - interval;
		btn_swr_start->y1 = gr_h + interval;
		strcpy(btn_swr_start->text, "Start");
		btn_swr_start->visible = VISIBLE;

		btn_swr_OK = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_swr_OK");
		btn_swr_OK->x1 = mid_w + interval;
		btn_swr_OK->y1 = btn_swr_start->y1;
		btn_swr_OK->visible = VISIBLE;

		lbl_swr_error = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_swr_error");
		btn_Options = (button_t *) find_gui_obj(TYPE_BUTTON, get_win(WINDOW_MAIN), "btn_Options");

		backup_freq = hamradio_get_freq_rx();
		if (hamradio_verify_freq_bands(backup_freq, & lim_bottom, & lim_top))
		{
			label_t * lbl_swr_bottom = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_swr_bottom");
			local_snprintf_P(lbl_swr_bottom->text, ARRAY_SIZE(lbl_swr_bottom->text), PSTR("%dk"), lim_bottom / 1000);
			lbl_swr_bottom->x = x0;
			lbl_swr_bottom->y = y0 + get_label_height(lbl_swr_bottom) * 2;
			lbl_swr_bottom->visible = VISIBLE;

			label_t * lbl_swr_top = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_swr_top");
			local_snprintf_P(lbl_swr_top->text, ARRAY_SIZE(lbl_swr_top->text), PSTR("%dk"), lim_top / 1000);
			lbl_swr_top->x = x1 - get_label_width(lbl_swr_bottom);
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
			lbl_swr_error->y = gr_h / 2;
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
			button_t * btn_swr_start = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_swr_start");
			button_t * btn_swr_OK = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_swr_OK");

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
		gui_drawline(x0, y0, x0, y1, COLORPIP_WHITE);
		gui_drawline(x0, y0, x1, y0, COLORPIP_WHITE);

		char buf [5];
		uint_fast8_t l = 1, row_step = roundf((y0 - y1) / 3);
		uint8_t charw = gothic_11x13.width, charh = gothic_11x13.height;

		local_snprintf_P(buf, ARRAY_SIZE(buf), "%d", l ++);
		gui_print_UB(x0 - charw, y0 - charh / 2, buf, & gothic_11x13, COLORPIP_WHITE);

		for(int_fast16_t yy = y0 - row_step; yy > y1; yy -= row_step)
		{
			if (yy < 0)
				break;

			gui_drawline(x0 + 1, yy, x1, yy, COLORPIP_DARKGREEN);
			local_snprintf_P(buf, ARRAY_SIZE(buf), "%d", l ++);
			gui_print_UB(x0 - charw, yy - charh / 2, buf, & gothic_11x13, COLORPIP_WHITE);
		}

		if (lbl_swr_error->visible)				// фон сообщения об ошибке
		{
			int lx1 = 0, ly1 = gr_h / 2 - 3, lx2 = gr_w - 1, ly2 = ly1 + get_label_height(lbl_swr_error) + 4;
			gui_drawrect(lx1, ly1, lx2, ly2, COLORPIP_RED, 1);
		}
		else									// маркер текущей частоты
		{
			gui_drawline(x0 + current_freq_x, y0, x0 + current_freq_x, y1, COLORPIP_RED);
		}

		if (is_swr_scanning || swr_scan_done)	// вывод графика во время сканирования и по завершении
		{
			for(uint_fast16_t j = 2; j <= i; j ++)
				gui_drawline(x0 + j - 2, y0 - y_vals [j - 2], x0 + j - 1, y0 - y_vals [j - 1], COLORPIP_YELLOW);
		}
	}
#endif /* WITHSWRSCAN */
}

// *********************************************************************************************************************************************************************

void window_tx_process(void)
{
	window_t * const win = get_win(WINDOW_TX_SETTINGS);
	unsigned update = 0;

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 6, row_count = 3;
		button_t * bh = NULL;
		win->first_call = 0;
		update = 1;

		gui_obj_create("btn_tx_vox",          100, 44, 0, 0, "VOX|OFF");
		gui_obj_create("btn_tx_vox_settings", 100, 44, 0, 0, "VOX|settings");
		gui_obj_create("btn_tx_power",        100, 44, 0, 0, "TX power");

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
			button_t * const btn_tx_vox = (button_t *) find_gui_obj(TYPE_BUTTON, winTX, "btn_tx_vox");
			button_t * const btn_tx_power = (button_t *) find_gui_obj(TYPE_BUTTON, winTX, "btn_tx_power");
			button_t * const btn_tx_vox_settings = (button_t *) find_gui_obj(TYPE_BUTTON, winTX, "btn_tx_vox_settings");

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
		button_t * btn_tx_power = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_tx_power");
		btn_tx_power->state = DISABLED;
#endif /* ! WITHPOWERTRIM */

		button_t * btn_tx_vox = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_tx_vox"); 						// vox on/off
#if WITHVOX
		btn_tx_vox->is_locked = hamradio_get_gvoxenable() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(btn_tx_vox->text, ARRAY_SIZE(btn_tx_vox->text), PSTR("VOX|%s"), btn_tx_vox->is_locked ? "ON" : "OFF");
#else
		btn_tx_vox->state = DISABLED;
		local_snprintf_P(btn_tx_vox->text, ARRAY_SIZE(btn_tx_vox->text), PSTR("VOX"));
#endif /* WITHVOX */

		button_t * btn_tx_vox_settings = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_tx_vox_settings");		// vox settings
#if WITHVOX
		btn_tx_vox_settings->state = hamradio_get_gvoxenable() ? CANCELLED : DISABLED;
#else
		btn_tx_vox_settings->state = DISABLED;
#endif /* WITHVOX */
	}
}

// *********************************************************************************************************************************************************************

void window_tx_vox_process(void)
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
		win->bh_ptr = (button_t *) malloc(buttons_size);
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
		win->lh_ptr = (label_t *) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		hamradio_get_vox_delay_limits(& delay_min, & delay_max);
		hamradio_get_vox_level_limits(& level_min, & level_max);
		hamradio_get_antivox_delay_limits(& alevel_min, & alevel_max);

		sl_vox_delay = (slider_t *) find_gui_obj(TYPE_SLIDER, win, "sl_vox_delay");
		sl_vox_level = (slider_t *) find_gui_obj(TYPE_SLIDER, win, "sl_vox_level");
		sl_avox_level = (slider_t *) find_gui_obj(TYPE_SLIDER, win, "sl_avox_level");
		lbl_vox_delay = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_vox_delay");
		lbl_vox_level = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_vox_level");
		lbl_avox_level = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_avox_level");

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

		button_t * bh = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_tx_vox_OK");
		bh->x1 = (sl_vox_delay->x + sl_vox_delay->size) / 2 - (bh->w / 2);
		bh->y1 = lbl_avox_level->y + interval;
		bh->visible = VISIBLE;

		d = ldiv(delay_min, 100);
		label_t * lbl_vox_delay_min = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_vox_delay_min");
		local_snprintf_P(lbl_vox_delay_min->text, ARRAY_SIZE(lbl_vox_delay_min->text), PSTR("%d.%d sec"), (int) d.quot, (int) (d.rem / 10));
		lbl_vox_delay_min->x = sl_vox_delay->x - get_label_width(lbl_vox_delay_min) / 2;
		lbl_vox_delay_min->y = sl_vox_delay->y + get_label_height(lbl_vox_delay_min) * 3;
		lbl_vox_delay_min->visible = VISIBLE;

		d = ldiv(delay_max, 100);
		label_t * lbl_vox_delay_max = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_vox_delay_max");
		local_snprintf_P(lbl_vox_delay_max->text, ARRAY_SIZE(lbl_vox_delay_max->text), PSTR("%d.%d sec"), (int) d.quot, (int) (d.rem / 10));
		lbl_vox_delay_max->x = sl_vox_delay->x + sl_vox_delay->size - get_label_width(lbl_vox_delay_max) / 2;
		lbl_vox_delay_max->y = sl_vox_delay->y + get_label_height(lbl_vox_delay_max) * 3;
		lbl_vox_delay_max->visible = VISIBLE;

		label_t * lbl_vox_level_min = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_vox_level_min");
		local_snprintf_P(lbl_vox_level_min->text, ARRAY_SIZE(lbl_vox_level_min->text), PSTR("%d"), level_min);
		lbl_vox_level_min->x = sl_vox_level->x - get_label_width(lbl_vox_level_min) / 2;
		lbl_vox_level_min->y = sl_vox_level->y + get_label_height(lbl_vox_level_min) * 3;
		lbl_vox_level_min->visible = VISIBLE;

		label_t * lbl_vox_level_max = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_vox_level_max");
		local_snprintf_P(lbl_vox_level_max->text, ARRAY_SIZE(lbl_vox_level_max->text), PSTR("%d"), level_max);
		lbl_vox_level_max->x = sl_vox_level->x + sl_vox_level->size - get_label_width(lbl_vox_level_max) / 2;
		lbl_vox_level_max->y = sl_vox_level->y + get_label_height(lbl_vox_level_max) * 3;
		lbl_vox_level_max->visible = VISIBLE;

		label_t * lbl_avox_level_min = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_avox_level_min");
		local_snprintf_P(lbl_avox_level_min->text, ARRAY_SIZE(lbl_avox_level_min->text), PSTR("%d"), alevel_min);
		lbl_avox_level_min->x = sl_avox_level->x - get_label_width(lbl_avox_level_min) / 2;
		lbl_avox_level_min->y = sl_avox_level->y + get_label_height(lbl_avox_level_min) * 3;
		lbl_avox_level_min->visible = VISIBLE;

		label_t * lbl_avox_level_max = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_avox_level_max");
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

			button_t * btn_tx_vox_OK = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_tx_vox_OK");
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

void window_tx_power_process(void)
{
#if WITHPOWERTRIM && WITHTX
	window_t * const win = get_win(WINDOW_TX_POWER);

	static label_t * lbl_tx_power = NULL, * lbl_tune_power = NULL;
	static enc_var_t pw;
	static uint_fast8_t power_min, power_max, power_full, power_tune;

	if (win->first_call)
	{
		unsigned interval = 50;
		win->first_call = 0;
		pw.change = 0;
		pw.updated = 1;

		gui_obj_create("btn_tx_pwr_OK", 44, 44, 0, 0, "OK");
		gui_obj_create("btn_p", 44, 44, 1, 0, "+");
		gui_obj_create("btn_n", 44, 44, 1, 0, "-");

		gui_obj_create("lbl_tx_power", 0, FONT_MEDIUM, COLORPIP_WHITE, 16);
		gui_obj_create("lbl_tune_power", 0, FONT_MEDIUM, COLORPIP_WHITE, 16);

		lbl_tx_power = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_tx_power");
		lbl_tune_power = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_tune_power");

		hamradio_get_tx_power_limits(& power_min, & power_max);
		power_full = hamradio_get_tx_power();
		power_tune = hamradio_get_tx_tune_power();

		lbl_tx_power->x = 0;
		lbl_tx_power->y = 10;
		lbl_tx_power->visible = VISIBLE;
		lbl_tx_power->payload = 0;
		local_snprintf_P(lbl_tx_power->text, ARRAY_SIZE(lbl_tx_power->text), PSTR("TX power  : %3d"), power_full);

		lbl_tune_power->x = lbl_tx_power->x;
		lbl_tune_power->y = lbl_tx_power->y + interval;
		lbl_tune_power->visible = VISIBLE;
		lbl_tune_power->payload = 1;
		local_snprintf_P(lbl_tune_power->text, ARRAY_SIZE(lbl_tune_power->text), PSTR("Tune power: %3d"), power_tune);

		button_t * btn_p = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_p");
		btn_p->x1 = 0;
		btn_p->y1 = lbl_tune_power->y + interval;
		btn_p->visible = VISIBLE;
		btn_p->payload = 1;

		button_t * btn_n = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_n");
		btn_n->x1 = btn_p->x1 + interval;
		btn_n->y1 = btn_p->y1 ;
		btn_n->visible = VISIBLE;
		btn_n->payload = -1;

		button_t * btn_tx_pwr_OK = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_tx_pwr_OK");
		btn_tx_pwr_OK->x1 = btn_n->x1 + interval;
		btn_tx_pwr_OK->y1 = btn_n->y1;
		btn_tx_pwr_OK->visible = VISIBLE;

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_tx_pwr_OK = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_tx_pwr_OK");
			button_t * btn_p = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_p");
			button_t * btn_n = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_n");

			if (bh == btn_tx_pwr_OK)
			{
				close_all_windows();
			}
			else if (bh == btn_p || bh == btn_n)
			{
				pw.change = bh->payload;
				pw.updated = 1;
			}
		}
		else if (IS_LABEL_PRESS)
		{
			label_t * lh = (label_t *) ptr;
			pw.select = lh->payload;
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

void window_audiosettings_process(void)
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
#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_ALSA) && BLUETOOTH_ALSA
			{ 100, 44, CANCELLED, BUTTON_NON_LOCKED, 0, 0, WINDOW_AUDIOSETTINGS, NON_VISIBLE, INT32_MAX, "btn_audio_switch", 	 "", 	 },
#endif /* defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_ALSA) && BLUETOOTH_ALSA */
		};
		win->bh_count = ARRAY_SIZE(buttons);
		unsigned buttons_size = sizeof(buttons);
		win->bh_ptr = (button_t *) malloc(buttons_size);
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

#if ! WITHAFCODEC1HAVEPROC
		bh = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_mic_eq");						// MIC EQ on/off
		bh->state = DISABLED;
		bh = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_mic_eq_settings");				// MIC EQ settings
		bh->state = DISABLED;
#endif /* ! WITHAFCODEC1HAVEPROC */

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
			window_t * const winMIC = get_win(WINDOW_AP_MIC_SETT);
			window_t * const winMICpr = get_win(WINDOW_AP_MIC_PROF);
			button_t * const btn_reverb = (button_t *) find_gui_obj(TYPE_BUTTON, winAP, "btn_reverb");						// reverb on/off
			button_t * const btn_reverb_settings = (button_t *) find_gui_obj(TYPE_BUTTON, winAP, "btn_reverb_settings");		// reverb settings
			button_t * const btn_monitor = (button_t *) find_gui_obj(TYPE_BUTTON, winAP, "btn_monitor");						// monitor on/off
			button_t * const btn_mic_eq = (button_t *) find_gui_obj(TYPE_BUTTON, winAP, "btn_mic_eq");						// MIC EQ on/off
			button_t * const btn_mic_eq_settings = (button_t *) find_gui_obj(TYPE_BUTTON, winAP, "btn_mic_eq_settings");		// MIC EQ settingss
			button_t * const btn_mic_settings = (button_t *) find_gui_obj(TYPE_BUTTON, winAP, "btn_mic_settings");			// mic settings
			button_t * const btn_mic_profiles = (button_t *) find_gui_obj(TYPE_BUTTON, winAP, "btn_mic_profiles");			// mic profiles

			if (bh == btn_monitor)
			{
				hamradio_set_gmoniflag(! hamradio_get_gmoniflag());
				update = 1;
			}
#if WITHREVERB
			else if (bh == btn_reverb_settings)
			{
				open_window(get_win(WINDOW_AP_REVERB_SETT));
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
#endif /* WITHAFCODEC1HAVEPROC */
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
#if WITHREVERB
			else if (bh == btn_reverb)
			{
				hamradio_set_greverb(! hamradio_get_greverb());
				update = 1;
			}
#endif /* WITHREVERB */
#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_ALSA) && BLUETOOTH_ALSA
			else if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_audio_switch"))
			{
				alsa_switch_out();
				update = 1;
			}
#endif /* defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_ALSA) && BLUETOOTH_ALSA */
		}
		break;

	default:

		break;
	}

	if (update)
	{
		button_t * bh = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_reverb"); 			// reverb on/off
#if WITHREVERB
		bh->is_locked = hamradio_get_greverb() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Reverb|%s"), hamradio_get_greverb() ? "ON" : "OFF");
#else
		bh->state = DISABLED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Reverb|OFF"));
#endif /* WITHREVERB */

		bh = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_reverb_settings");				// reverb settings
#if WITHREVERB
		bh->state = hamradio_get_greverb() ? CANCELLED : DISABLED;
#else
		bh->state = DISABLED;
#endif /* WITHREVERB */

		bh = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_monitor");						// monitor on/off
		bh->is_locked = hamradio_get_gmoniflag() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Monitor|%s"), bh->is_locked ? "enabled" : "disabled");

#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_ALSA) && BLUETOOTH_ALSA
		bh = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_audio_switch");
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Audio|%s"), get_alsa_out());
#endif /* defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_ALSA) && BLUETOOTH_ALSA */
	}
}

// *********************************************************************************************************************************************************************

void window_ap_reverb_process(void)
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
		win->bh_ptr = (button_t *) malloc(buttons_size);
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
		win->lh_ptr = (label_t *) malloc(labels_size);
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

		label_t * lbl_reverbDelay_min = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_reverbDelay_min");
		label_t * lbl_reverbDelay_max = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_reverbDelay_max");
		label_t * lbl_reverbLoss_min = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_reverbLoss_min");
		label_t * lbl_reverbLoss_max = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_reverbLoss_max");
		lbl_reverbDelay = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_reverbDelay");
		lbl_reverbLoss = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_reverbLoss");
		sl_reverbDelay = (slider_t *) find_gui_obj(TYPE_SLIDER, win, "reverbDelay");
		sl_reverbLoss = (slider_t *) find_gui_obj(TYPE_SLIDER, win, "reverbLoss");

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

		button_t * bh = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_REVs_ok");
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

void window_ap_mic_eq_process(void)
{
#if WITHAFCODEC1HAVEPROC
	window_t * const win = get_win(WINDOW_AP_MIC_EQ);
	const gxdrawb_t * db = gui_get_drawbuf();

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
		win->bh_ptr = (button_t *) malloc(buttons_size);
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
		win->lh_ptr = (label_t *) malloc(labels_size);
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
			lbl = (label_t *) find_gui_obj(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%sk"), strchr(sl->name, 'q') + 1);
			lbl->x = mid_w - get_label_width(lbl) / 2;
			lbl->y = y;
			lbl->visible = VISIBLE;

			y = lbl->y + get_label_height(lbl) * 2;

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_val"), sl->name);
			lbl = (label_t *) find_gui_obj(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%d"), hamradio_get_gmikeequalizerparams(id) + eq_base);
			lbl->x = mid_w - get_label_width(lbl) / 2;
			lbl->y = y;
			lbl->visible = VISIBLE;

			sl->y = lbl->y + get_label_height(lbl) * 2 + 10;

			x = x + interval;
			y = 0;
		}

		btn_EQ_ok = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_EQ_ok");
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
			lbl = (label_t *) find_gui_obj(TYPE_LABEL, win, buf);
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
		colpip_line(db, win->x1 + 50, mid_y + yy, win->x1 + win->w - (btn_EQ_ok->w << 1), mid_y + yy, GUI_SLIDERLAYOUTCOLOR, 0);
		local_snprintf_P(buf, ARRAY_SIZE(buf), i == 0 ? PSTR("%d") : PSTR("-%d"), i);
		colpip_string2_tbg(db, win->x1 + 50 - strwidth2(buf) - 5, mid_y + yy - SMALLCHARH2 / 2, buf, COLORPIP_WHITE);

		if (i == 0)
			continue;
		colpip_line(db, win->x1 + 50, mid_y - yy, win->x1 + win->w - (btn_EQ_ok->w << 1), mid_y - yy, GUI_SLIDERLAYOUTCOLOR, 0);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), i);
		colpip_string2_tbg(db, win->x1 + 50 - strwidth2(buf) - 5, mid_y - yy - SMALLCHARH2 / 2, buf, COLORPIP_WHITE);
	}
#endif /* WITHAFCODEC1HAVEPROC */
}

// *********************************************************************************************************************************************************************

void window_af_eq_process(void)
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
		win->bh_ptr = (button_t *) malloc(buttons_size);
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
		win->lh_ptr = (label_t *) malloc(labels_size);
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
			lbl = (label_t *) find_gui_obj(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%s"), strchr(sl->name, 'q') + 1);
			lbl->x = mid_w - get_label_width(lbl) / 2;
			lbl->y = y;
			lbl->visible = VISIBLE;

			y = lbl->y + get_label_height(lbl) * 2;

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_val"), sl->name);
			lbl = (label_t *) find_gui_obj(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%d"), hamradio_get_af_equalizer_gain_rx(id) + eq_base);
			lbl->x = mid_w - get_label_width(lbl) / 2;
			lbl->y = y;
			lbl->visible = VISIBLE;

			sl->y = lbl->y + get_label_height(lbl) * 2 + 10;

			x = x + interval;
			y = 0;
		}

		button_t * btn_EQ_ok = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_EQ_ok");
		btn_EQ_ok->x1 = sl->x + sliders_width + btn_EQ_ok->w;
		btn_EQ_ok->y1 = sl->y + sl->size - btn_EQ_ok->h;
		btn_EQ_ok->visible = VISIBLE;

		button_t * btn_EQ_enable = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_EQ_enable");
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
			button_t * btn_EQ_ok = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_EQ_ok");
			button_t * btn_EQ_enable = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_EQ_enable");

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
			lbl = (label_t *) find_gui_obj(TYPE_LABEL, win, buf);
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

void window_ap_mic_process(void)
{
	window_t * const win = get_win(WINDOW_AP_MIC_SETT);

	static slider_t * sl_micLevel = NULL, * sl_micClip = NULL, * sl_micAGC = NULL;
	static label_t * lbl_micLevel = NULL, * lbl_micClip = NULL, * lbl_micAGC = NULL;
	static uint_fast8_t level_min, level_max, clip_min, clip_max, agc_min, agc_max;

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
		win->bh_ptr = (button_t *) malloc(buttons_size);
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
		win->lh_ptr = (label_t *) malloc(labels_size);
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

		sl_micLevel = (slider_t *) find_gui_obj(TYPE_SLIDER, win, "sl_micLevel");
		sl_micClip = (slider_t *) find_gui_obj(TYPE_SLIDER, win, "sl_micClip");
		sl_micAGC = (slider_t *) find_gui_obj(TYPE_SLIDER, win, "sl_micAGC");
		lbl_micLevel = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_micLevel");
		lbl_micClip = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_micClip");
		lbl_micAGC = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_micAGC");

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

		button_t * bh2 = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_mic_boost");
		bh2->x1 = (sl_micLevel->x + sl_micLevel->size) / 2 - (bh2->w / 2);
		bh2->y1 = lbl_micAGC->y + interval;
#if WITHAFCODEC1HAVEPROC
		bh2->is_locked = hamradio_get_gmikeboost20db() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
#else
		bh2->state = DISABLED;
#endif /* WITHAFCODEC1HAVEPROC */
		local_snprintf_P(bh2->text, ARRAY_SIZE(bh2->text), PSTR("Boost|%s"), bh2->is_locked ? "ON" : "OFF");
		bh2->visible = VISIBLE;

		button_t * bh1 = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_mic_agc");
		bh1->x1 = bh2->x1 - bh1->w - interval;
		bh1->y1 = bh2->y1;
		bh1->is_locked = hamradio_get_gmikeagc() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh1->text, ARRAY_SIZE(bh1->text), PSTR("AGC|%s"), bh1->is_locked ? "ON" : "OFF");
		bh1->visible = VISIBLE;

		bh1 = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_mic_OK");
		bh1->x1 = bh2->x1 + bh2->w + interval;
		bh1->y1 = bh2->y1;
		bh1->visible = VISIBLE;

		label_t * lbl_micLevel_min = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_micLevel_min");
		local_snprintf_P(lbl_micLevel_min->text, ARRAY_SIZE(lbl_micLevel_min->text), PSTR("%d"), level_min);
		lbl_micLevel_min->x = sl_micLevel->x - get_label_width(lbl_micLevel_min) / 2;
		lbl_micLevel_min->y = sl_micLevel->y + get_label_height(lbl_micLevel_min) * 3;
		lbl_micLevel_min->visible = VISIBLE;

		label_t * lbl_micLevel_max = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_micLevel_max");
		local_snprintf_P(lbl_micLevel_max->text, ARRAY_SIZE(lbl_micLevel_max->text), PSTR("%d"), level_max);
		lbl_micLevel_max->x = sl_micLevel->x + sl_micLevel->size - get_label_width(lbl_micLevel_max) / 2;
		lbl_micLevel_max->y = sl_micLevel->y + get_label_height(lbl_micLevel_max) * 3;
		lbl_micLevel_max->visible = VISIBLE;

		label_t * lbl_micClip_min = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_micClip_min");
		local_snprintf_P(lbl_micClip_min->text, ARRAY_SIZE(lbl_micClip_min->text), PSTR("%d"), clip_min);
		lbl_micClip_min->x = sl_micClip->x - get_label_width(lbl_micClip_min) / 2;
		lbl_micClip_min->y = sl_micClip->y + get_label_height(lbl_micClip_min) * 3;
		lbl_micClip_min->visible = VISIBLE;

		label_t * lbl_micClip_max = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_micClip_max");
		local_snprintf_P(lbl_micClip_max->text, ARRAY_SIZE(lbl_micClip_max->text), PSTR("%d"), clip_max);
		lbl_micClip_max->x = sl_micClip->x + sl_micClip->size - get_label_width(lbl_micClip_max) / 2;
		lbl_micClip_max->y = sl_micClip->y + get_label_height(lbl_micClip_max) * 3;
		lbl_micClip_max->visible = VISIBLE;

		label_t * lbl_micAGC_min = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_micAGC_min");
		local_snprintf_P(lbl_micAGC_min->text, ARRAY_SIZE(lbl_micAGC_min->text), PSTR("%d"), agc_min);
		lbl_micAGC_min->x = sl_micAGC->x - get_label_width(lbl_micAGC_min) / 2;
		lbl_micAGC_min->y = sl_micAGC->y + get_label_height(lbl_micAGC_min) * 3;
		lbl_micAGC_min->visible = VISIBLE;

		label_t * lbl_micAGC_max = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_micAGC_max");
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
			button_t * btn_mic_boost = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_mic_boost");
			button_t * btn_mic_agc = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_mic_agc");
			button_t * btn_mic_OK = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_mic_OK");

			if (bh == btn_mic_boost)
			{
#if WITHAFCODEC1HAVEPROC
				btn_mic_boost->is_locked = hamradio_get_gmikeboost20db() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
				local_snprintf_P(btn_mic_boost->text, ARRAY_SIZE(btn_mic_boost->text), PSTR("Boost|%s"), btn_mic_boost->is_locked ? "ON" : "OFF");
				hamradio_set_gmikeboost20db(btn_mic_boost->is_locked);
#endif /* WITHAFCODEC1HAVEPROC */
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
}

// *********************************************************************************************************************************************************************

void window_ap_mic_prof_process(void)
{
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
		win->bh_ptr = (button_t *) malloc(buttons_size);
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
}

// *********************************************************************************************************************************************************************

void window_notch_process(void)
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
		win->bh_ptr = (button_t *) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		static const label_t labels [] = {
			{ WINDOW_NOTCH, CANCELLED, 0, NON_VISIBLE, "lbl_freq",  "Freq:  *******",  FONT_MEDIUM, COLORPIP_YELLOW, },
			{ WINDOW_NOTCH, CANCELLED, 0, NON_VISIBLE, "lbl_width", "Width: *******",  FONT_MEDIUM, COLORPIP_WHITE,  },
			{ WINDOW_NOTCH, CANCELLED, 0, NON_VISIBLE, "lbl_type",  "Type: ",  		   FONT_MEDIUM, COLORPIP_WHITE,  },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t *) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		label_t * lbl_freq = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_freq");
		label_t * lbl_width = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_width");
		label_t * lbl_type = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_type");

		lbl_freq->x = 0;
		lbl_freq->y = 15;
		lbl_freq->visible = VISIBLE;

		lbl_width->x = lbl_freq->x;
		lbl_width->y = lbl_freq->y + interval;
		lbl_width->visible = VISIBLE;

		lbl_type->x = lbl_width->x;
		lbl_type->y = lbl_width->y + interval;
		lbl_type->visible = VISIBLE;

		button_t * btn_freqm = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_freq-");
		button_t * btn_freqp = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_freq+");
		button_t * btn_widthm = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_width-");
		button_t * btn_widthp = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_width+");
		button_t * btn_Auto = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_Auto");
		button_t * btn_Manual = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_Manual");

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
			button_t * btn_freqm = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_freq-");
			button_t * btn_freqp = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_freq+");
			button_t * btn_widthm = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_width-");
			button_t * btn_widthp = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_width+");
			button_t * btn_Auto = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_Auto");
			button_t * btn_Manual = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_Manual");

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
			label_t * lbl_freq = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_freq");
			label_t * lbl_width = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_width");

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
		button_t * btn_Auto = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_Auto");
		button_t * btn_Manual = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_Manual");
		label_t * lbl_freq = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_freq");
		label_t * lbl_width = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_width");

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

void window_gui_settings_process(void)
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
		win->bh_ptr = (button_t *) malloc(buttons_size);
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
			button_t * btn_enc2_step = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_enc2_step");
			button_t * btn_freq_swipe = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_freq_swipe");
			button_t * btn_freq_swipe_step = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_freq_swipe_step");

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
		button_t * btn_enc2_step = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_enc2_step");
		local_snprintf_P(btn_enc2_step->text, ARRAY_SIZE(btn_enc2_step->text), "Enc2 step|%s", enc2step [gui_nvram.enc2step_pos].label);

		button_t * btn_freq_swipe = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_freq_swipe");
		btn_freq_swipe->is_locked = gui_nvram.freq_swipe_enable != 0;
		local_snprintf_P(btn_freq_swipe->text, ARRAY_SIZE(btn_freq_swipe->text), "Freq swipe|%s", btn_freq_swipe->is_locked ? "enable" : "disable");

		button_t * btn_freq_swipe_step = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_freq_swipe_step");
		btn_freq_swipe_step->state = btn_freq_swipe->is_locked ? CANCELLED : DISABLED;
		local_snprintf_P(btn_freq_swipe_step->text, ARRAY_SIZE(btn_freq_swipe_step->text), "Swipe step|%s", freq_swipe_step[gui_nvram.freq_swipe_step].label);
	}
}

// *********************************************************************************************************************************************************************

void window_shift_process(void)
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

		gui_obj_create("lbl_rx_cic_shift", 0, FONT_MEDIUM, COLORPIP_WHITE, 13);
		gui_obj_create("lbl_rx_fir_shift", 0, FONT_MEDIUM, COLORPIP_WHITE, 13);
		gui_obj_create("lbl_tx_shift", 0, FONT_MEDIUM, COLORPIP_WHITE, 13);
		gui_obj_create("lbl_iq_test", 0, FONT_MEDIUM, COLORPIP_WHITE, 23);

		gui_obj_create("btn_p", 50, 50, 0, 0, "+");
		gui_obj_create("btn_m", 50, 50, 0, 0, "-");
		gui_obj_create("btn_test", 50, 50, 0, 0, "CIC|test");

		for (unsigned i = 0; i < win->lh_count; i ++)
		{
			label_t * lh = & win->lh_ptr [i];

			lh->x = x;
			lh->y = y;
			lh->payload = i;
			lh->visible = VISIBLE;
			lh->state = CANCELLED;

			y += get_label_height(lh) + interval * 3;
		}

		label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_rx_cic_shift");
		x = lh->x + get_label_width(lh) + interval * 2;

		for (unsigned i = 0; i < win->bh_count; i ++)
		{
			button_t * bh = & win->bh_ptr [i];

			bh->x1 = x;
			bh->y1 = 0;
			bh->visible = VISIBLE;

			x += bh->w + interval * 2;
		}

		label_t * lbl_iq_test = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_iq_test");
		local_snprintf_P(lbl_iq_test->text, ARRAY_SIZE(lbl_iq_test->text), "MAX IQ test:");

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:
		if (IS_LABEL_PRESS)
		{
			label_t * lh = (label_t *) ptr;
			enc.select = lh->payload;
			enc.change = 0;
			enc.updated = 1;
		}
		else if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_test"))
			{
				cic_test = ! cic_test;
				iq_cic_test(cic_test);
				bh->is_locked = cic_test;
			}
			else if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_p"))
			{
				enc.change = 1;
				enc.updated = 1;
			}
			else if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_m"))
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

		label_t * lbl_rx_cic_shift = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_rx_cic_shift");
		local_snprintf_P(lbl_rx_cic_shift->text, ARRAY_SIZE(lbl_rx_cic_shift->text), "RX CIC: %d", (int) iq_shift_cic_rx(0));
		label_t * lbl_rx_fir_shift = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_rx_fir_shift");
		local_snprintf_P(lbl_rx_fir_shift->text, ARRAY_SIZE(lbl_rx_fir_shift->text), "RX FIR: %d", (int) iq_shift_fir_rx(0));
		label_t * lbl_tx_shift = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_tx_shift");
		local_snprintf_P(lbl_tx_shift->text, ARRAY_SIZE(lbl_tx_shift->text), "TX CIC: %d", (int) iq_shift_tx(0));

		button_t * btn_test = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_test");
		btn_test->is_locked = cic_test;

		if (cic_test)
		{
			label_t * lbl_iq_test = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_iq_test");
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

void window_uif_process(void)
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
		win->bh_ptr = (button_t *) malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
		memcpy(win->bh_ptr, buttons, buttons_size);

		static const label_t labels [] = {
			{	WINDOW_UIF,  DISABLED,  0, NON_VISIBLE, "lbl_uif_val", "**", FONT_LARGE, COLORPIP_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		unsigned labels_size = sizeof(labels);
		win->lh_ptr = (label_t *) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		button_down = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_UIF-");
		button_up = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_UIF+");
		lbl_uif_val = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_uif_val");

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

		strcpy(win->title, menu_uif.name);

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
		strcpy(win->title, menu_uif.name);

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
	if (get_parent_window() != NO_PARENT_WINDOW)
	{
		close_window(OPEN_PARENT_WINDOW);
		footer_buttons_state(CANCELLED);
	}
	else
	{
		window_t * const win = get_win(WINDOW_MENU);
		open_window(win);
		footer_buttons_state(DISABLED, NULL);
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

void window_ft8_bands_process(void)
{
	window_t * const win = get_win(WINDOW_FT8_BANDS);

	if (win->first_call)
	{
		win->first_call = 0;
		unsigned cols = ft8_bands_count > 10 ? 4 : 5, interval = 15, x = 0, y = 0;
		button_t * bh;

		win->bh_count = ft8_bands_count;
		unsigned buttons_size = win->bh_count * sizeof (button_t);
		win->bh_ptr = (button_t *) malloc(buttons_size);
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

void window_ft8_settings_process(void)
{
	window_t * const win = get_win(WINDOW_FT8_SETTINGS);

	if (win->first_call)
	{
		win->first_call = 0;
		unsigned interval = 10;

		gui_obj_create("btn_callsign", 86, 30, 0, 0, "Callsign");
		gui_obj_create("btn_qth", 86, 30, 0, 0, "QTH");
		gui_obj_create("btn_freq", 86, 30, 0, 0, "TX freq");
		gui_obj_create("btn_freq_eq", 86, 30, 0, 0, "");
		gui_obj_create("btn_time0", 86, 30, 0, 0, "Time >0<");
		gui_obj_create("btn_OK", 40, 40, 0, 0, "OK");

		gui_obj_create("lbl_callsign", 0, FONT_MEDIUM, COLORPIP_WHITE, 10);
		gui_obj_create("lbl_qth", 0, FONT_MEDIUM, COLORPIP_WHITE, 10);
		gui_obj_create("lbl_txfreq", 0, FONT_MEDIUM, COLORPIP_WHITE, 10);

		label_t * lbl_callsign = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_callsign");
		label_t * lbl_qth = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_qth");
		label_t * lbl_txfreq = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_txfreq");
		button_t * btn_callsign = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_callsign");
		button_t * btn_qth = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_qth");
		button_t * btn_freq = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_freq");
		button_t * btn_OK = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_OK");
		button_t * btn_time0 = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_time0");

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

			if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_OK"))
			{
				close_window(OPEN_PARENT_WINDOW);
			}
			else if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_freq"))
			{
				uint32_t f = gui_nvram.ft8_txfreq_val;
				keyboard_edit_digits(& f);
				uint32_t l = hamradio_get_low_bp(0) * 10;
				uint32_t h = hamradio_get_high_bp(0) * 10;
				gui_nvram.ft8_txfreq_val = (f >= l && f <= h) ? f : gui_nvram.ft8_txfreq_val;
			}
			else if (bh == (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_time0"))
			{
				uint_fast8_t hour, minute, seconds;
				board_rtc_cached_gettime(& hour, & minute, & seconds);
				seconds = 0;
				board_rtc_settime(hour, minute, seconds);
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

void window_ft8_process(void)
{
	window_t * const win = get_win(WINDOW_FT8);
	static unsigned win_x = 0, win_y = 0, x, y, update = 0, selected_label_cq = 255, selected_label_tx = 0;
	static unsigned backup_mode = 0, work = 0, labels_tx_update = 0, backup_freq = 0, backup_zoom = 0;
	static label_t * lh_array_cq [6];
	static label_t * lh_array_tx [4];
	static const int snr = -10;
	static uint8_t viewtemp;

	if (win->first_call)
	{
		win->first_call = 0;

		gui_obj_create("btn_tx", 86, 44, 0, 0, "Transmit");
		gui_obj_create("btn_filter", 86, 44, 0, 0, "View|all");
		gui_obj_create("btn_bands", 86, 44, 0, 0, "FT8|bands");
		gui_obj_create("btn_settings", 86, 44, 0, 0, "Edit|settings");

		gui_obj_create("lbl_cq_title", 0, FONT_LARGE, COLORPIP_GREEN, 3);
		gui_obj_create("lbl_tx_title", 0, FONT_LARGE, COLORPIP_GREEN, 3);
		gui_obj_create("lbl_cq0", 0, FONT_MEDIUM, COLORPIP_WHITE, 8);
		gui_obj_create("lbl_cq1", 0, FONT_MEDIUM, COLORPIP_WHITE, 8);
		gui_obj_create("lbl_cq2", 0, FONT_MEDIUM, COLORPIP_WHITE, 8);
		gui_obj_create("lbl_cq3", 0, FONT_MEDIUM, COLORPIP_WHITE, 8);
		gui_obj_create("lbl_cq4", 0, FONT_MEDIUM, COLORPIP_WHITE, 8);
		gui_obj_create("lbl_cq5", 0, FONT_MEDIUM, COLORPIP_WHITE, 8);
		gui_obj_create("lbl_txmsg0", 0, FONT_MEDIUM, COLORPIP_WHITE, 10);
		gui_obj_create("lbl_txmsg1", 0, FONT_MEDIUM, COLORPIP_WHITE, 10);
		gui_obj_create("lbl_txmsg2", 0, FONT_MEDIUM, COLORPIP_WHITE, 10);
		gui_obj_create("lbl_txmsg3", 0, FONT_MEDIUM, COLORPIP_WHITE, 10);

		static const text_field_t text_field [] = {
			{ 37, 26, CANCELLED, WINDOW_FT8, NON_VISIBLE, UP, & gothic_11x13, "tf_ft8", },
		};
		win->tf_count = ARRAY_SIZE(text_field);
		unsigned tf_size = sizeof(text_field);
		win->tf_ptr = (text_field_t*) malloc(tf_size);
		GUI_MEM_ASSERT(win->tf_ptr);
		memcpy(win->tf_ptr, text_field, tf_size);

		text_field_t * tf_ft8 = (text_field_t*) find_gui_obj(TYPE_TEXT_FIELD, win, "tf_ft8");
		textfield_update_size(tf_ft8);
		tf_ft8->x1 = 0;
		tf_ft8->y1 = 0;
		tf_ft8->visible = VISIBLE;

		label_t * lh = NULL;
		label_t * lbl_cq_title = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_cq_title");
		label_t * lbl_tx_title = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_tx_title");
		button_t * btn_tx = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_tx");
		button_t * btn_filter = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_filter");
		button_t * btn_bands = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_bands");
		button_t * btn_settings = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_settings");

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
			lh = (label_t *) find_gui_obj(TYPE_LABEL, win, lh_name);
			lh->x = x;
			lh->y = y;
			lh->payload = i;
			y += interval;
			lh_array_cq [i] = lh;
		}

		lbl_tx_title->x = lbl_cq_title->x + get_label_width(lh_array_cq [0]) + 10;
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
			lh = (label_t *) find_gui_obj(TYPE_LABEL, win, lh_name);
			lh->x = x;
			lh->y = y;
			lh->payload = 10 + i;
			lh->visible = VISIBLE;
			y += interval;
			lh_array_tx [i] = lh;
		}

		local_snprintf_P(lh_array_tx [0]->text, ARRAY_SIZE(lh_array_tx [0]->text), "CQ %s %s", gui_nvram.ft8_callsign, gui_nvram.ft8_qth);

		btn_tx->x1 = lh_array_tx [3]->x;
		btn_tx->y1 = lh_array_tx [3]->y + get_label_height(lh_array_tx [3]) * 3;
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
			backup_zoom = hamradio_get_gzoomxpow2();
			viewtemp = hamradio_get_viewstyle();
			hamradio_settemp_viewstyle(VIEW_LINE);

			hamradio_set_freq(ft8_bands [gui_nvram.ft8_band]);
			hamradio_change_submode(ft8_mode, 0);
			hamradio_set_gzoomxpow2(BOARD_FFTZOOM_POW2MAX);
			memset(ft8.rx_text, '\0', ft8_text_records * ft8_text_length);
			ft8_set_state(1);
		}
		work = 0;

		hamradio_set_lock(1);
		display2_set_page_temp(display_getpagegui());
	}

	if (parse_ft8buf)
	{
		parse_ft8buf = 0;
		selected_label_cq = 255;
		text_field_t * tf_ft8 = (text_field_t*) find_gui_obj(TYPE_TEXT_FIELD, win, "tf_ft8");

		memset(cq_call, 0, sizeof(cq_call));

		for (unsigned i = 0; i < ft8.decoded_messages; i ++)
		{
			char * msg = ft8.rx_text [i];
			remove_end_line_spaces(msg);
			if (! strlen(msg)) break;
			COLORPIP_T colorline;
			unsigned cq_flag = 0;
			parse_ft8_answer(msg, & colorline, & cq_flag);
			if (cq_filter)
			{
				if (cq_flag)
					textfield_add_string_old(tf_ft8, msg, colorline);
			}
			else
				textfield_add_string_old(tf_ft8, msg, colorline);
		}

		update = 1;
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			button_t * bh = (button_t *) ptr;
			button_t * btn_tx = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_tx");
			button_t * btn_filter = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_filter");
			button_t * btn_bands = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_bands");
			button_t * btn_settings = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_settings");

			if (bh == btn_tx)
			{
				strcpy(ft8.tx_text, lh_array_tx [selected_label_tx]->text);
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
			if (lh->payload < 10)
			{
				selected_label_cq = lh->payload;
				labels_tx_update = 1;
			}
			else
				selected_label_tx = lh->payload - 10;
			update = 1;
		}

		break;

	case WM_MESSAGE_CLOSE:

		if (! work)
		{
			hamradio_set_freq(backup_freq);
			hamradio_change_submode(backup_mode, 0);
			hamradio_settemp_viewstyle(viewtemp);
			hamradio_set_gzoomxpow2(backup_zoom);
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
			label_t * lh = lh_array_cq [i];
			lh->visible = NON_VISIBLE;
			lh->color = COLORPIP_WHITE;
			if (strlen(cq_call [i]))
			{
				strcpy(lh->text, cq_call [i]);
				lh->visible = VISIBLE;

				if (lh->payload == selected_label_cq)
					lh->color = COLORPIP_YELLOW;
			}
		}

		for (unsigned i = 0; i < 4; i ++)
		{
			label_t * lh = lh_array_tx [i];

			if (i == selected_label_tx)
				lh->color = COLORPIP_YELLOW;
			else
				lh->color = COLORPIP_WHITE;
		}

		if (labels_tx_update)
		{
			labels_tx_update = 0;
			local_snprintf_P(lh_array_tx [1]->text, ARRAY_SIZE(lh_array_tx [1]->text), "%s %s %s", cq_call [selected_label_cq], gui_nvram.ft8_callsign, gui_nvram.ft8_qth);
			local_snprintf_P(lh_array_tx [2]->text, ARRAY_SIZE(lh_array_tx [2]->text), "%s %s %s", cq_call [selected_label_cq], gui_nvram.ft8_callsign, gui_nvram.ft8_snr);
			local_snprintf_P(lh_array_tx [3]->text, ARRAY_SIZE(lh_array_tx [3]->text), "%s %s %s", cq_call [selected_label_cq], gui_nvram.ft8_callsign, gui_nvram.ft8_end);
		}

		local_snprintf_P(win->title, ARRAY_SIZE(win->title), "FT8 terminal *** %d k *** %02d:%02d:%02d", ft8_bands [gui_nvram.ft8_band] / 1000, hour, minute, seconds);
	}
}

#endif /* WITHFT8 */

// *********************************************************************************************************************************************************************

void window_af_process(void)
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
		win->bh_ptr = (button_t *) malloc(buttons_size);
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
		win->lh_ptr = (label_t *) malloc(labels_size);
		GUI_MEM_ASSERT(win->lh_ptr);
		memcpy(win->lh_ptr, labels, labels_size);

		label_t * lbl_low = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_low");
		label_t * lbl_high = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_high");
		label_t * lbl_afr = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_afr");
		label_t * lbl_ifshift = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_ifshift");

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
			button_t * btn_low_m = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_low_m");
			button_t * btn_low_p = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_low_p");
			button_t * btn_high_m = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_high_m");
			button_t * btn_high_p = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_high_p");
			button_t * btn_afr_m = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_afr_m");
			button_t * btn_afr_p = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_afr_p");
			button_t * btn_ifshift_m = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_ifshift_m");
			button_t * btn_ifshift_p = (button_t *) find_gui_obj(TYPE_BUTTON, win, "btn_ifshift_p");

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
			bp_t.select = lh->payload;
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

		label_t * const lbl_low = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_low");
		label_t * const lbl_high = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_high");
		label_t * const lbl_afr = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_afr");
		label_t * const lbl_ifshift = (label_t *) find_gui_obj(TYPE_LABEL, win, "lbl_ifshift");

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

void window_mode_process(void)
{
	window_t * const win = get_win(WINDOW_MODES);

	if (win->first_call)
	{
		win->first_call = 0;

		gui_obj_create("btn_ModeLSB", 86, 44, 0, 0, "LSB");
		gui_obj_create("btn_ModeCW", 86, 44, 0, 0, "CW");
		gui_obj_create("btn_ModeAM", 86, 44, 0, 0, "AM");
		gui_obj_create("btn_ModeDGL", 86, 44, 0, 0, "DGL");
		gui_obj_create("btn_ModeUSB", 86, 44, 0, 0, "USB");
		gui_obj_create("btn_ModeCWR", 86, 44, 0, 0, "CWR");
		gui_obj_create("btn_ModeNFM", 86, 44, 0, 0, "NFM");
		gui_obj_create("btn_ModeDGU", 86, 44, 0, 0, "DGU");

		gui_obj_set_prop("btn_ModeLSB", GUI_OBJ_PAYLOAD, SUBMODE_LSB);
		gui_obj_set_prop("btn_ModeCW", GUI_OBJ_PAYLOAD, SUBMODE_CW);
		gui_obj_set_prop("btn_ModeAM", GUI_OBJ_PAYLOAD, SUBMODE_AM);
		gui_obj_set_prop("btn_ModeDGL", GUI_OBJ_PAYLOAD, SUBMODE_DGL);
		gui_obj_set_prop("btn_ModeUSB", GUI_OBJ_PAYLOAD, SUBMODE_USB);
		gui_obj_set_prop("btn_ModeCWR", GUI_OBJ_PAYLOAD, SUBMODE_CWR);
		gui_obj_set_prop("btn_ModeNFM", GUI_OBJ_PAYLOAD, SUBMODE_NFM);
		gui_obj_set_prop("btn_ModeDGU", GUI_OBJ_PAYLOAD, SUBMODE_DGU);

		gui_obj_align_to("btn_ModeCW", "btn_ModeLSB", ALIGN_RIGHT_UP, 6);
		gui_obj_align_to("btn_ModeAM", "btn_ModeCW", ALIGN_RIGHT_UP, 6);
		gui_obj_align_to("btn_ModeDGL", "btn_ModeAM", ALIGN_RIGHT_UP, 6);
		gui_obj_align_to("btn_ModeUSB", "btn_ModeLSB", ALIGN_DOWN_LEFT, 6);
		gui_obj_align_to("btn_ModeCWR", "btn_ModeUSB", ALIGN_RIGHT_UP, 6);
		gui_obj_align_to("btn_ModeNFM", "btn_ModeCWR", ALIGN_RIGHT_UP, 6);
		gui_obj_align_to("btn_ModeDGU", "btn_ModeNFM", ALIGN_RIGHT_UP, 6);

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			hamradio_change_submode(gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD), 1);

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

void window_receive_process(void)
{
	window_t * const win = get_win(WINDOW_RECEIVE);
	unsigned update = 0;

	if (win->first_call)
	{
		win->first_call = 0;
		update = 1;

		const char btn_names[][NAME_ARRAY_SIZE] = { "btn_att", "lbl_agc", "btn_mode", "btn_preamp", "btn_af", "btn_dnr", "btn_wnb", };

		for (int i = 0; i < ARRAY_SIZE(btn_names); i ++)
			gui_obj_create(btn_names[i], 100, 44, 0, "");

		gui_arrange_buttons(btn_names, ARRAY_SIZE(btn_names), 4, 6);

		gui_obj_set_prop("btn_mode", GUI_OBJ_TEXT, "Mode");
		gui_obj_set_prop("btn_af", GUI_OBJ_TEXT, "AF|filter");
		gui_obj_set_prop("btn_dnr", GUI_OBJ_TEXT, "DNR");
		gui_obj_set_prop("btn_wnb", GUI_OBJ_TEXT, "WNB");
		gui_obj_set_prop("btn_preamp", GUI_OBJ_TEXT, "Preamp");

#if ! WITHWNB
		gui_obj_set_prop("btn_wnb", GUI_OBJ_STATE, DISABLED);
#endif /* ! WITHWNB */

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_att"))
				hamradio_change_att();
			else if (gui_check_obj(name, "btn_preamp"))
				hamradio_change_preamp(1);
			else if (gui_check_obj(name, "btn_af"))
				open_window(get_win(WINDOW_AF));
			else if (gui_check_obj(name, "btn_agc"))
				hamradio_get_agc_type() ? hamradio_set_agc_slow() : hamradio_set_agc_fast();
			else if (gui_check_obj(name, "btn_mode"))
				open_window(get_win(WINDOW_MODES));
			else if (gui_check_obj(name, "btn_dnr"))
				hamradio_change_nr(1);
#if WITHWNB
			else if (gui_check_obj(name, "btn_wnb"))
				gui_obj_set_prop("btn_wnb", GUI_OBJ_LOCK, wnb_state_switch(1));
#endif /* WITHWNB */
		}
		else if (IS_BUTTON_LONG_PRESS)			// обработка длинного нажатия
		{
#if WITHWNB
			if (gui_check_obj(name, "btn_wnb"))
				open_window(get_win(WINDOW_WNBCONFIG));
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
		const char * a = remove_start_line_spaces(hamradio_get_att_value());
		gui_obj_set_prop("btn_att", GUI_OBJ_TEXT_FMT, "Attenuator|%s", a == NULL ? "off" : a);
		gui_obj_set_prop("btn_preamp", GUI_OBJ_LOCK, hamradio_change_preamp(0));
		gui_obj_set_prop("btn_dnr", GUI_OBJ_LOCK, hamradio_change_nr(0));
		gui_obj_set_prop("btn_agc", GUI_OBJ_TEXT_FMT, "AGC|%s", hamradio_get_agc_type() ? "fast" : "slow");
	}
}

// *********************************************************************************************

void window_freq_process (void)
{
	static label_t * lbl_freq;
	static editfreq_t editfreq;
	window_t * const win = get_win(WINDOW_FREQ);
	unsigned update = 0;

	if (win->first_call)
	{
		win->first_call = 0;

		char btns_text[12][3] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "<", "0", "OK" };
		uint8_t btns_payload[12] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, BUTTON_CODE_BK, 0, BUTTON_CODE_OK };
		char btn_names[12][NAME_ARRAY_SIZE];

		for (unsigned i = 0; i < 12; i ++)
		{
			local_snprintf_P(btn_names[i], NAME_ARRAY_SIZE, "btn_%02d", btns_payload[i]);
			gui_obj_create(btn_names[i], 50, 50, 0, 0, btns_text[i]);
			gui_obj_set_prop(btn_names[i], GUI_OBJ_PAYLOAD, btns_payload[i]);
		}

		gui_arrange_buttons(btn_names, 12, 3, 6);
		gui_obj_set_prop("btn_30", GUI_OBJ_LOCK, 1);

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
			editfreq.key = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
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
					close_all_windows();
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

int is_valid_datetime(int year, int month, int day, int hour, int minute, int second)
{
    if (month < 1 || month > 12)
        return 0;

    if (day < 1 || day > 31)
        return 0;

    if (hour < 0 || minute < 0 || second < 0)
        return 0;

    if (hour > 23 || minute > 59 || second > 59)
        return 0;

    uint8_t days_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // Корректировка количества дней в феврале високосного года
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    	days_in_month[1] = 29;

    if (day > days_in_month[month - 1])
        return 0;

    return 1;
}

void window_time_process(void)
{
#if defined (RTC1_TYPE)
	window_t * const win = get_win(WINDOW_TIME);
	static uint_fast16_t year;
	static uint_fast8_t month, day, hour, minute, second, update;

	if (win->first_call)
	{
		win->first_call = 0;
		update = 1;
		uint8_t interval = 20;

		gui_obj_create("lbl_year", 0, FONT_MEDIUM, COLORPIP_WHITE, 6);
		gui_obj_create("lbl_month", 0, FONT_MEDIUM, COLORPIP_WHITE, 4);
		gui_obj_create("lbl_day", 0, FONT_MEDIUM, COLORPIP_WHITE, 4);
		gui_obj_create("lbl_hour", 0, FONT_MEDIUM, COLORPIP_WHITE, 4);
		gui_obj_create("lbl_minute", 0, FONT_MEDIUM, COLORPIP_WHITE, 4);
		gui_obj_create("lbl_second", 0, FONT_MEDIUM, COLORPIP_WHITE, 4);

		gui_obj_create("btn_year+", 40, 40, 0, 0, "+");
		gui_obj_create("btn_year-", 40, 40, 0, 0, "-");
		gui_obj_create("btn_month+", 40, 40, 0, 0, "+");
		gui_obj_create("btn_month-", 40, 40, 0, 0, "-");
		gui_obj_create("btn_day+", 40, 40, 0, 0, "+");
		gui_obj_create("btn_day-", 40, 40, 0, 0, "-");
		gui_obj_create("btn_hour+", 40, 40, 0, 0, "+");
		gui_obj_create("btn_hour-", 40, 40, 0, 0, "-");
		gui_obj_create("btn_minute+", 40, 40, 0, 0, "+");
		gui_obj_create("btn_minute-", 40, 40, 0, 0, "-");
		gui_obj_create("btn_second+", 40, 40, 0, 0, "+");
		gui_obj_create("btn_second-", 40, 40, 0, 0, "-");
		gui_obj_create("btn_sec0", 40, 40, 0, 0, ">0");
		gui_obj_create("btn_set", 40, 40, 0, 0, "Set");

		gui_obj_align_to("lbl_month", "lbl_year", ALIGN_RIGHT_UP, interval);
		gui_obj_align_to("lbl_day", "lbl_month", ALIGN_RIGHT_UP, interval);
		gui_obj_align_to("lbl_hour", "lbl_day", ALIGN_RIGHT_UP, interval);
		gui_obj_align_to("lbl_minute", "lbl_hour", ALIGN_RIGHT_UP, interval);
		gui_obj_align_to("lbl_second", "lbl_minute", ALIGN_RIGHT_UP, interval);

		gui_obj_align_to("btn_year+", "lbl_year", ALIGN_DOWN_MID, interval);
		gui_obj_align_to("btn_month+", "lbl_month", ALIGN_DOWN_MID, interval);
		gui_obj_align_to("btn_day+", "lbl_day", ALIGN_DOWN_MID, interval);
		gui_obj_align_to("btn_hour+", "lbl_hour", ALIGN_DOWN_MID, interval);
		gui_obj_align_to("btn_minute+", "lbl_minute", ALIGN_DOWN_MID, interval);
		gui_obj_align_to("btn_second+", "lbl_second", ALIGN_DOWN_MID, interval);
		gui_obj_align_to("btn_sec0", "btn_second+", ALIGN_RIGHT_UP, interval);
		gui_obj_align_to("btn_set", "btn_sec0", ALIGN_DOWN_LEFT, interval);

		gui_obj_align_to("btn_year-", "btn_year+", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("btn_month-", "btn_month+", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("btn_day-", "btn_day+", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("btn_hour-", "btn_hour+", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("btn_minute-", "btn_minute+", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("btn_second-", "btn_second+", ALIGN_DOWN_LEFT, interval);

		gui_obj_set_prop("btn_year+", GUI_OBJ_PAYLOAD, 1);
		gui_obj_set_prop("btn_month+", GUI_OBJ_PAYLOAD, 1);
		gui_obj_set_prop("btn_day+", GUI_OBJ_PAYLOAD, 1);
		gui_obj_set_prop("btn_hour+", GUI_OBJ_PAYLOAD, 1);
		gui_obj_set_prop("btn_minute+", GUI_OBJ_PAYLOAD, 1);
		gui_obj_set_prop("btn_second+", GUI_OBJ_PAYLOAD, 1);
		gui_obj_set_prop("btn_year-", GUI_OBJ_PAYLOAD, -1);
		gui_obj_set_prop("btn_month-", GUI_OBJ_PAYLOAD, -1);
		gui_obj_set_prop("btn_day-", GUI_OBJ_PAYLOAD, -1);
		gui_obj_set_prop("btn_hour-", GUI_OBJ_PAYLOAD, -1);
		gui_obj_set_prop("btn_minute-", GUI_OBJ_PAYLOAD, -1);
		gui_obj_set_prop("btn_second-", GUI_OBJ_PAYLOAD, -1);
		gui_obj_set_prop("btn_set", GUI_OBJ_LOCK, 1);

		board_rtc_cached_getdatetime(& year, & month, & day, & hour, & minute,	& second);
		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			int tmp_year = year, tmp_month = month, tmp_day = day;
			int tmp_hour = hour, tmp_minute = minute, tmp_second = second;

			if (gui_check_obj(name, "btn_year+") || gui_check_obj(name, "btn_year-"))
				tmp_year += gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			else if (gui_check_obj(name, "btn_month+") || gui_check_obj(name, "btn_month-"))
				tmp_month += gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			else if (gui_check_obj(name, "btn_day+") || gui_check_obj(name, "btn_day-"))
				tmp_day += gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			else if (gui_check_obj(name, "btn_hour+") || gui_check_obj(name, "btn_hour-"))
				tmp_hour += gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			else if (gui_check_obj(name, "btn_minute+") || gui_check_obj(name, "btn_minute-"))
				tmp_minute += gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			else if (gui_check_obj(name, "btn_second+") || gui_check_obj(name, "btn_second-"))
				tmp_second += gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			else if (gui_check_obj(name, "btn_sec0"))
				tmp_second = 0;
			else if (gui_check_obj(name, "btn_set"))
			{
				board_rtc_setdatetime(year, month, day, hour, minute, second);
				close_all_windows();
				return;
			}

			if (is_valid_datetime(tmp_year, tmp_month, tmp_day, tmp_hour, tmp_minute, tmp_second))
			{
				year = tmp_year;
				month = tmp_month;
				day = tmp_day;
				hour = tmp_hour;
				minute = tmp_minute;
				second = tmp_second;
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

		gui_obj_set_prop("lbl_year", GUI_OBJ_TEXT_FMT, "Y:%d", year);
		gui_obj_set_prop("lbl_month", GUI_OBJ_TEXT_FMT, "m:%02d", month);
		gui_obj_set_prop("lbl_day", GUI_OBJ_TEXT_FMT, "d:%02d", day);
		gui_obj_set_prop("lbl_hour", GUI_OBJ_TEXT_FMT, "h:%02d", hour);
		gui_obj_set_prop("lbl_minute", GUI_OBJ_TEXT_FMT, "m:%02d", minute);
		gui_obj_set_prop("lbl_second", GUI_OBJ_TEXT_FMT, "s:%02d", second);
	}
#endif /* defined (RTC1_TYPE) */
}

void window_kbd_process(void)
{
	window_t * const win = get_win(WINDOW_KBD);
	static unsigned update = 0, is_shift = 0;
	const char kbd_cap[] = ",.!@#*()?;QWERTYUIOPASDFGHJKLZXCVBNM";
	const char kbd_low[] = "1234567890qwertyuiopasdfghjklzxcvbnm";
	const unsigned len1 = 10, len2 = 10, len3 = 9, len4 = 7, kbd_len = len1 + len2 + len3 + len4, btn_size = 40;
	const char kbd_func [5][20] = { "btn_kbd_caps", "btn_kbd_backspace", "btn_kbd_space", "btn_kbd_enter", "btn_kbd_esc", };
	static char edit_str[TEXT_ARRAY_SIZE];
	char btn_name[NAME_ARRAY_SIZE];

	if (win->first_call)
	{
		win->first_call = 0;
		update = 1;
		is_shift = 0;
		unsigned x = 0, y = 0, interval = 5, i = 0;

		if (gui_keyboard.clean)
			memset(edit_str, 0, TEXT_ARRAY_SIZE);
		else if (gui_keyboard.digits_only)
			local_snprintf_P(edit_str, TEXT_ARRAY_SIZE, "%d", * gui_keyboard.num);
		else
			strncpy(edit_str, gui_keyboard.str, gui_keyboard.max_len);

		for (; i < kbd_len + 5; i ++)
		{
			if (i == len1)
			{
				x = btn_size / 2;
				y += btn_size + interval;
			}
			else if (i == len1 + len2)
			{
				x = btn_size + interval;
				y += btn_size + interval;
			}
			else if (i == len1 + len2 + len3)
			{
				x = btn_size * 2;
				y += btn_size + interval;
			}

			if (i < kbd_len)
				local_snprintf_P(btn_name, NAME_ARRAY_SIZE, "btn_kbd_%02d", i);
			else
				local_snprintf_P(btn_name, NAME_ARRAY_SIZE, "%s", kbd_func [i - kbd_len]);

			gui_obj_create(btn_name, btn_size, btn_size, 0, 0, "");
			gui_obj_set_prop(btn_name, GUI_OBJ_STATE, (gui_keyboard.digits_only && i > len1 - 1 && i < kbd_len) ? DISABLED : CANCELLED);
			gui_obj_set_prop(btn_name, GUI_OBJ_POS, x, y);

			x += btn_size + interval;
		}

		gui_obj_set_prop("btn_kbd_caps", GUI_OBJ_POS, 0, btn_size * 3 + interval * 3);
		gui_obj_set_prop("btn_kbd_caps", GUI_OBJ_WIDTH, 75);
		gui_obj_set_prop("btn_kbd_caps", GUI_OBJ_TEXT, "CAPS");
		gui_obj_set_prop("btn_kbd_caps", GUI_OBJ_LOCK, is_shift);
		gui_obj_set_prop("btn_kbd_caps", GUI_OBJ_STATE, gui_keyboard.digits_only ? DISABLED : CANCELLED);

		gui_obj_set_prop("btn_kbd_backspace", GUI_OBJ_POS, btn_size * len1 + interval * len1, 0);
		gui_obj_set_prop("btn_kbd_backspace", GUI_OBJ_REPEAT, 1);
		gui_obj_set_prop("btn_kbd_backspace", GUI_OBJ_TEXT, "<-");

		local_snprintf_P(btn_name, NAME_ARRAY_SIZE, "btn_kbd_%02d", kbd_len - 1);
		gui_obj_align_to("btn_kbd_space", btn_name, ALIGN_RIGHT_UP, interval);
		gui_obj_set_prop("btn_kbd_space", GUI_OBJ_WIDTH, 95);
		gui_obj_set_prop("btn_kbd_space", GUI_OBJ_TEXT, "Space");
		gui_obj_set_prop("btn_kbd_space", GUI_OBJ_PAYLOAD, (char) ' ');
		gui_obj_set_prop("btn_kbd_space", GUI_OBJ_STATE, gui_keyboard.digits_only ? DISABLED : CANCELLED);

		gui_obj_align_to("btn_kbd_esc", "btn_kbd_backspace", ALIGN_RIGHT_UP, interval * 3);
		gui_obj_set_prop("btn_kbd_esc", GUI_OBJ_SIZE, 50, 50);
		gui_obj_set_prop("btn_kbd_esc", GUI_OBJ_TEXT, "Esc");

		gui_obj_align_to("btn_kbd_enter", "btn_kbd_esc", ALIGN_DOWN_LEFT, interval * 2);
		gui_obj_set_prop("btn_kbd_enter", GUI_OBJ_SIZE, 50, 50);
		gui_obj_set_prop("btn_kbd_enter", GUI_OBJ_TEXT, "OK");
		gui_obj_set_prop("btn_kbd_enter", GUI_OBJ_LOCK, 1);

		local_snprintf_P(win->title, ARRAY_SIZE(win->title), "%s_", edit_str);
		calculate_window_position(win, WINDOW_POSITION_AUTO);
		window_set_title_align(win, TITLE_ALIGNMENT_CENTER);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_kbd_caps"))
			{
				is_shift = ! is_shift;
				gui_obj_set_prop("btn_kbd_caps", GUI_OBJ_LOCK, is_shift);
				update = 1;
			}
			else if (gui_check_obj(name, "btn_kbd_backspace"))
			{
				unsigned l = strlen(edit_str);
				if (l)
					edit_str [l - 1] = '\0';
			}
			else if (gui_check_obj(name, "btn_kbd_enter") || gui_check_obj(name, "btn_kbd_esc"))
			{
				if (gui_check_obj(name, "btn_kbd_enter"))
				{
					if (gui_keyboard.digits_only)
						* gui_keyboard.num = strtoul(edit_str, NULL, 10);
					else
						strcpy(gui_keyboard.str, edit_str);
				}

				close_window(OPEN_PARENT_WINDOW);
				return;
			}
			else
			{
				char text[2] = { (char) gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD), '\0', };

				if (gui_keyboard.digits_only)
				{
					char tmp_str[TEXT_ARRAY_SIZE];
					strcpy(tmp_str, edit_str);
					strcat(tmp_str, text);
					if (strtoul(tmp_str, NULL, 10) < UINT32_MAX)
						strcat(edit_str, text);
				}
				else if (strlen(edit_str) < gui_keyboard.max_len)
					strcat(edit_str, text);
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
			local_snprintf_P(btn_name, NAME_ARRAY_SIZE, "btn_kbd_%02d", i);
			char p = is_shift ? kbd_cap [i] : kbd_low [i];
			gui_obj_set_prop(btn_name, GUI_OBJ_TEXT_FMT, "%c", p);
			gui_obj_set_prop(btn_name, GUI_OBJ_PAYLOAD, p);
		}
	}
}

// *********************************************************************************************************************************************************************

void window_kbd_test_process(void)
{
	window_t * const win = get_win(WINDOW_KBD_TEST);
	static int num_lbl1 = 12345;
	static char str_lbl2 [TEXT_ARRAY_SIZE] = "qwertyuiopas";
	const unsigned win_id = WINDOW_KBD_TEST;

	if (win->first_call)
	{
		win->first_call = 0;
		const unsigned interval = 30;

		gui_obj_create("lbl_text", 0, FONT_MEDIUM, COLORPIP_WHITE, 20);
		gui_obj_create("lbl_num", 0, FONT_MEDIUM, COLORPIP_WHITE, 20);

		gui_obj_create("btn_text", 86, 40, 0, 0, "Edit...");
		gui_obj_create("btn_num", 86, 40, 0, 0, "Edit...");

		gui_obj_set_prop("lbl_text", GUI_OBJ_POS_Y, 20);

		gui_obj_align_to("lbl_num", "lbl_text", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("btn_text", "lbl_text", ALIGN_RIGHT_UP_MID, interval);
		gui_obj_align_to("btn_num", "lbl_num", ALIGN_RIGHT_UP_MID, interval);

		gui_obj_set_prop("lbl_text", GUI_OBJ_TEXT, str_lbl2);
		gui_obj_set_prop("lbl_num", GUI_OBJ_TEXT_FMT, "%d", num_lbl1);

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_text"))
				keyboard_edit_string((uintptr_t) & str_lbl2, 10, 0);					// передается строка длиной 12
			else if (gui_check_obj(name, "btn_num"))
				keyboard_edit_digits(& num_lbl1);
		}
		break;

	default:
	break;
	}
}

// *****************************************************************************************************************************

/* https://www.a1k0n.net/2011/07/20/donut-math.html */

void window_3d_process(void)
{
	window_t * const win = get_win(WINDOW_3D);
	static float A = 0, B = 0, z[1760];
	char b[22][80];

	if (win->first_call)
	{
		win->first_call = 0;

		gui_obj_create("tf_3d", 60, 22, UP, & gothic_11x13);

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

		textfield_clean("tf_3d");
		for (int k = 0; k < 22; k ++)
			textfield_add_string("tf_3d", b[k], COLORPIP_WHITE);

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

void split_string(char * str, char ch)
{
    int len = strlen(str);
    int mid = len / 2;
    int pos = -1;
    int min_dist = len;

    for (int i = mid; i < len; i++)
    {
        if (str[i] == ' ')
        {
            int dist = i - mid;
            if (dist < min_dist)
            {
                min_dist = dist;
                pos = i;
            }
        }
    }

    if (pos == -1)
    {
        for (int i = mid - 1; i >= 0; i--)
        {
            if (str[i] == ' ')
            {
                int dist = mid - i;
                if (dist < min_dist)
                {
                    min_dist = dist;
                    pos = i;
                }
            }
        }
    }

    if (pos != -1)
    {
        for (int i = len; i > pos; i--)
            str[i] = str[i - 1];

        str[pos] = ch;
    }
}

void window_menu_params_process(void)
{
	window_t * const win = get_win(WINDOW_MENU_PARAMS);
	static menu_names_t menup [MENU_PARAMS_MAX], menuv;
	static unsigned sel = 0;
	static char btn_selected[NAME_ARRAY_SIZE];
	char btn_names[MENU_PARAMS_MAX][NAME_ARRAY_SIZE];

	if (win->first_call)
	{
		win->first_call = 0;
		sel = 0;

		const unsigned count = hamradio_get_multilinemenu_block_params(menup, index_param, MENU_PARAMS_MAX);
		unsigned cols = count <= 16 ? 4 : 5, interval = 6;

		for (unsigned i = 0; i < count; i ++)
		{
			local_snprintf_P(btn_names[i], TEXT_ARRAY_SIZE, "btn_params_%02d", i);
			remove_end_line_spaces(menup[i].name);

			if (getwidth_Pstring(menup[i].name, & FONT_BUTTONS) > 110)
				split_string(menup[i].name, '|');

			gui_obj_create(btn_names[i], 120, 40, 0, 0, menup[i].name);
			gui_obj_set_prop(btn_names[i], GUI_OBJ_PAYLOAD, menup[i].index);
		}

		gui_arrange_buttons(btn_names, count, cols, interval);

#if 1	// Добавить экранные кнопки "+" и "-" при необходимости
		gui_obj_create("btn_p", 40, 40, 1, 0, "+");
		gui_obj_create("btn_m", 40, 40, 1, 0, "-");

		char buf[TEXT_ARRAY_SIZE];
		local_snprintf_P(buf, TEXT_ARRAY_SIZE, "btn_params_%02d", count < cols ? count - 1 : cols - 1);

		gui_obj_align_to("btn_p", buf, ALIGN_RIGHT_UP, interval);
		gui_obj_set_prop("btn_p", GUI_OBJ_PAYLOAD, 1);
		gui_obj_align_to("btn_m", "btn_p", ALIGN_DOWN_LEFT, interval);
		gui_obj_set_prop("btn_m", GUI_OBJ_PAYLOAD, -1);
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
			if (gui_check_obj(name, "btn_p") || gui_check_obj(name, "btn_m"))
				gui_set_encoder2_rotate(gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD));
			else
			{
				if (sel)
					gui_obj_set_prop(btn_selected, GUI_OBJ_LOCK, 0);

				strncpy(btn_selected, name, NAME_ARRAY_SIZE - 1);
				gui_obj_set_prop(btn_selected, GUI_OBJ_LOCK, 1);
				sel = 1;

				hamradio_get_multilinemenu_block_vals(& menuv, gui_obj_get_int_prop(btn_selected, GUI_OBJ_PAYLOAD), 1);
				remove_end_line_spaces(menuv.name);
				local_snprintf_P(win->title, ARRAY_SIZE(win->title), "%s: %s",
						gui_obj_get_string_prop(btn_selected, GUI_OBJ_TEXT), menuv.name);

			}
		}
		break;

	case WM_MESSAGE_ENC2_ROTATE:
	{
		char edit_val [20];
		if (sel)
		{
			strcpy(edit_val, hamradio_gui_edit_menu_item(menuv.index, action));
			remove_end_line_spaces(edit_val);
			local_snprintf_P(win->title, ARRAY_SIZE(win->title), "%s: %s",
					gui_obj_get_string_prop(btn_selected, GUI_OBJ_TEXT), edit_val);
		}
	}
		break;

	default:
	break;
	}
}

void window_menu_process(void)
{
	window_t * const win = get_win(WINDOW_MENU);

	if (win->first_call)
	{
		win->first_call = 0;
		menu_names_t menu [MENU_GROUPS_MAX];
		char btn_names[MENU_GROUPS_MAX][NAME_ARRAY_SIZE];

		const unsigned count = hamradio_get_multilinemenu_block_groups(menu);
		ASSERT(count < MENU_GROUPS_MAX);

		for (unsigned i = 0; i < count; i ++)
		{
			local_snprintf_P(btn_names[i], TEXT_ARRAY_SIZE, "btn_groups_%02d", i);
			remove_end_line_spaces(menu[i].name);

			if (getwidth_Pstring(menu[i].name, & FONT_BUTTONS) > 110)
				split_string(menu[i].name, '|');

			gui_obj_create(btn_names[i], 120, 40, 0, 0, menu[i].name);
			gui_obj_set_prop(btn_names[i], GUI_OBJ_PAYLOAD, menu[i].index);
		}

		gui_arrange_buttons(btn_names, count, 4, 6);

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			index_param = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			window_t * const win = get_win(WINDOW_MENU_PARAMS);
			open_window(win);
		}
		break;

	default:
	break;
	}
}

// *****************************************************************************************************************************

void window_lfm_process(void)
{
#if WITHLFM
	window_t * const win = get_win(WINDOW_LFM);
	static unsigned update = 0;
	static enc_var_t enc;

	const char * const lbl_array[3] = { "lbl_timeoffset", "lbl_stopfreq", "lbl_interval", };

	if (win->first_call)
	{
		unsigned x = 0, y = 0, interval = 24;
		win->first_call = 0;
		update = 1;
		enc.select = 0;
		enc.change = 0;
		enc.updated = 1;

		gui_obj_create("lbl_nmeatime", 0, FONT_MEDIUM, COLORPIP_WHITE, 20);
		gui_obj_create("lbl_timeoffset", 0, FONT_MEDIUM, COLORPIP_WHITE, 20);
		gui_obj_create("lbl_stopfreq", 0, FONT_MEDIUM, COLORPIP_WHITE, 20);
		gui_obj_create("lbl_interval", 0, FONT_MEDIUM, COLORPIP_WHITE, 20);

		gui_obj_create("btn_state", 86, 40, 0, 0, "");
		gui_obj_create("btn_draw", 86, 40, 0, 0, "Draw|spectre");
		gui_obj_create("btn_p", 35, 35, 0, 0, "+");
		gui_obj_create("btn_m", 35, 35, 0, 0, "-");

		gui_obj_align_to("lbl_timeoffset", "lbl_nmeatime", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("lbl_stopfreq", "lbl_timeoffset", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("lbl_interval", "lbl_stopfreq", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("btn_state", "lbl_interval", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("btn_draw", "btn_state", ALIGN_RIGHT_UP, interval);
		gui_obj_align_to("btn_p", "lbl_timeoffset", ALIGN_RIGHT_UP, interval);
		gui_obj_align_to("btn_m", "btn_p", ALIGN_DOWN_LEFT, interval);

		gui_obj_set_prop("btn_p", GUI_OBJ_PAYLOAD, 1);
		gui_obj_set_prop("btn_m", GUI_OBJ_PAYLOAD, -1);

		gui_obj_set_prop("lbl_nmeatime", GUI_OBJ_STATE, DISABLED);
		gui_obj_set_prop("lbl_timeoffset", GUI_OBJ_PAYLOAD, 0);
		gui_obj_set_prop("lbl_stopfreq", GUI_OBJ_PAYLOAD, 1);
		gui_obj_set_prop("lbl_interval", GUI_OBJ_PAYLOAD, 2);

		hamradio_enable_encoder2_redirect();
		enable_window_move(win);
		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_state"))
			{
				if (gui_obj_get_int_prop("btn_state", GUI_OBJ_LOCK))
				{
					hamradio_lfm_disable();
					gui_obj_set_prop("btn_state", GUI_OBJ_LOCK, 0);
				}
				else
					hamradio_set_lfmmode(! hamradio_get_lfmmode());

				update = 1;
			}
			else if (gui_check_obj(name, "btn_draw"))
			{
				open_window(get_win(WINDOW_LFM_SPECTRE));
			}
			else if (gui_check_obj(name, "btn_p") || gui_check_obj(name, "btn_m"))
				gui_set_encoder2_rotate(gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD));
		}
		else if (IS_LABEL_PRESS)
		{
			enc.select = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
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

		for(unsigned i = 0; i < 3; i ++)
			gui_obj_set_prop(lbl_array[i], GUI_OBJ_COLOR, COLORPIP_WHITE);

		ASSERT(enc.select < 3);
		gui_obj_set_prop(lbl_array[enc.select], GUI_OBJ_COLOR, COLORPIP_YELLOW);

		if (enc.select == 0)
		{
			uint_fast16_t v = hamradio_get_lfmtoffset();
			hamradio_set_lfmtoffset(v + enc.change);
		}
		else if (enc.select == 1)
		{
			uint_fast16_t v = hamradio_get_lfmstop100k();
			hamradio_set_lfmstop100k(v + enc.change * 10);
		}
		else if (enc.select == 2)
		{
			uint_fast16_t v = hamradio_get_lfmtinterval();
			hamradio_set_lfmtinterval(v + enc.change);
		}

		update = 1;
	}

	if (update)
	{
		update = 0;
		uint8_t state = iflfmactive() != 0;
		uint8_t s = hamradio_get_lfmmode() == 0 ? 0 : state ? 2 : 1;
		const char states[3][9] = { "Disabled", "Standby", "Scan..." };
		char buf[30];
		hamradio_get_nmea_time(buf, 30);

		gui_obj_set_prop("btn_draw", GUI_OBJ_STATE, s == 2 ? CANCELLED : DISABLED);
		gui_obj_set_prop("btn_state", GUI_OBJ_LOCK, state);
		gui_obj_set_prop("btn_state", GUI_OBJ_TEXT, states[s]);

		gui_obj_set_prop("lbl_nmeatime", GUI_OBJ_TEXT_FMT,   "NMEA time  : %s", buf);
		gui_obj_set_prop("lbl_timeoffset", GUI_OBJ_TEXT_FMT, "Time offset: %d", hamradio_get_lfmtoffset());
		gui_obj_set_prop("lbl_stopfreq", GUI_OBJ_TEXT_FMT,   "Stop freq  : %02d MHz", hamradio_get_lfmstop100k() / 10);
		gui_obj_set_prop("lbl_interval", GUI_OBJ_TEXT_FMT,   "Interval   : %02d sec", hamradio_get_lfmtinterval());
	}
#endif /* WITHLFM  */
}

void window_lfm_spectre_process(void)
{
#if WITHLFM
	enum { xmax = 600, ymax = 200, i1 = (800 / 2) - 100, i2 = (800 / 2) + 100 };
	window_t * const win = get_win(WINDOW_LFM_SPECTRE);
	static COLORPIP_T d[xmax][ymax];
	static int shift = 0;

	static unsigned xx = 0;

	if (win->first_call)
	{
		win->first_call = 0;
		xx = 0;
		shift = 0;

		memset(d, GUI_DEFAULTCOLOR, sizeof(d));
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

	if (xx >= xmax)
	{
		for (int x = 0; x < xmax - 1; x ++)
			memmove(d[x], d[x + 1], (xmax - 1) * 4);

		xx --;
	}

	for (int i = i1, y = 0; i < i2; i ++, y ++)
		d[xx][y] = display2_get_spectrum(i + shift);

	xx ++;

	for (int x = 0; x < xmax; x ++)
		for (int y = 0; y < ymax; y ++)
			gui_drawpoint(x, y, d[x][y]);

#endif /* WITHLFM  */
}

#if WITHEXTIO_LAN

void stream_log(char * str)
{
	if (get_parent_window() == WINDOW_EXTIOLAN)
	{
		textfield_add_string("tf_log", str, COLORPIP_WHITE);
		put_to_wm_queue(get_win(WINDOW_EXTIOLAN), WM_MESSAGE_UPDATE);
	}
}

void window_stream_process(void)
{
	window_t * const win = get_win(WINDOW_EXTIOLAN);
	static uint8_t update = 0;

	if (win->first_call)
	{
		win->first_call = 0;

		gui_obj_create("tf_log", 50, 15, DOWN, & gothic_11x13);
		gui_obj_create("btn_state", 130, 40, 0, 0, "");

		gui_obj_set_prop("tf_log", GUI_OBJ_POS, 0, 0);
		gui_obj_align_to("btn_state", "tf_log", ALIGN_DOWN_MID, 10);

		update = 1;
		calculate_window_position(win, WINDOW_POSITION_AUTO);
		return;
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_state"))
			{
				if (stream_get_state())
					server_stop();
				else
					server_start();
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

		const char * states[] = { "Start server", "Stop listening", "Disconnect &|stop server", };
		gui_obj_set_prop("btn_state", GUI_OBJ_TEXT, states[stream_get_state()]);
	}
}

#else
void window_stream_process(void) {}
#endif /* # WITHEXTIO_LAN */

void window_wnbconfig_process(void)
{
#if WITHWNB
	window_t * const win = get_win(WINDOW_WNBCONFIG);
	static unsigned update = 0;
	static enc_var_t enc;

	if (win->first_call)
	{
		win->first_call = 0;
		unsigned x = 0, y = 0, interval = 24;

		gui_obj_create("btn_add", 40, 40, 0, 0, "+");
		gui_obj_create("btn_sub", 40, 40, 0, 0, "-");
		gui_obj_create("lbl_val", 0, FONT_MEDIUM, COLORPIP_WHITE, 14);

		gui_obj_set_prop("lbl_val", GUI_OBJ_POS, 0, 15);
		gui_obj_align_to("btn_sub", "lbl_val", ALIGN_RIGHT_UP_MID, 10);
		gui_obj_set_prop("btn_sub", GUI_OBJ_PAYLOAD, -1);
		gui_obj_align_to("btn_add", "btn_sub", ALIGN_RIGHT_UP, 10);
		gui_obj_set_prop("btn_add", GUI_OBJ_PAYLOAD, 1);

		hamradio_enable_encoder2_redirect();
		enable_window_move(win);
		calculate_window_position(win, WINDOW_POSITION_AUTO);
		update = 1;
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			enc.change = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
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
		uint_fast16_t v = wnb_get_threshold();
		wnb_set_threshold(v + enc.change);

		enc.updated = 0;
		update = 1;
	}

	if (update)
	{
		update = 0;

		gui_obj_set_prop("lbl_val", GUI_OBJ_TEXT_FMT, "Threshold: %d", wnb_get_threshold());
	}
#endif /* WITHWNB */
}

#if WITHAD936XIIO
void window_ad936x_process(void)
{
	window_t * const win = get_win(WINDOW_AD936X);
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

		gui_obj_create("lbl_status", 0, FONT_MEDIUM, COLORPIP_WHITE, 9);
		gui_obj_create("lbl_status_str", 0, FONT_MEDIUM, COLORPIP_WHITE, 20);
		gui_obj_create("lbl_iio_name", 0, FONT_MEDIUM, COLORPIP_WHITE, 9);
		gui_obj_create("lbl_iio_val", 0, FONT_MEDIUM, COLORPIP_WHITE, 20);
		gui_obj_create("btn_uri_edit", 86, 44, 0, 0, "Edit...");
		gui_obj_create("btn_action", 86, 44, 0, 0, "Find");
		gui_obj_create("btn_gain_type", 86, 44, 0, 0, "Gain|manual");
		gui_obj_create("btn_gain_add", 44, 44, 0, 0, "+");
		gui_obj_create("btn_gain_sub", 44, 44, 0, 0, "-");

		gui_obj_set_prop("lbl_status", GUI_OBJ_POS, 0, 0);
		gui_obj_set_prop("lbl_status", GUI_OBJ_TEXT, "Status: ");
		gui_obj_align_to("lbl_status_str", "lbl_status", ALIGN_RIGHT_UP, 10);
		gui_obj_set_prop("lbl_status_str", GUI_OBJ_TEXT, "");
		gui_obj_align_to("lbl_iio_name", "lbl_status", ALIGN_DOWN_LEFT, 10);
		gui_obj_set_prop("lbl_iio_name", GUI_OBJ_TEXT, "URI:    ");
		gui_obj_align_to("lbl_iio_val", "lbl_iio_name", ALIGN_RIGHT_UP, 10);
		gui_obj_align_to("btn_uri_edit", "lbl_iio_name", ALIGN_DOWN_LEFT, 10);
		gui_obj_align_to("btn_action", "btn_uri_edit", ALIGN_RIGHT_UP, 10);
		gui_obj_align_to("btn_gain_type", "btn_action", ALIGN_DOWN_LEFT, 10);
		gui_obj_align_to("btn_gain_sub", "btn_gain_type", ALIGN_LEFT_UP, 10);
		gui_obj_set_prop("btn_gain_sub", GUI_OBJ_PAYLOAD, -1);
		gui_obj_align_to("btn_gain_add", "btn_gain_type", ALIGN_RIGHT_UP, 10);
		gui_obj_set_prop("btn_gain_add", GUI_OBJ_PAYLOAD, 1);

		enable_window_move(win);
		calculate_window_position(win, WINDOW_POSITION_AUTO);
		update = 1;
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

	if (IS_BUTTON_PRESS)
	{
		if (gui_check_obj(name, "btn_uri_edit"))
		{
			keyboard_edit_string((uintptr_t) & uri, ARRAY_SIZE(uri), 0);
		}
		else if (gui_check_obj(name, "btn_action"))
		{
			if (status == 10 || status == 1)
				status = iio_ad936x_find(uri);
			else if (status == 0)
				status = iio_ad936x_start(uri);
			else if (status == 2)
				status = iio_ad936x_stop();

			update = 1;
		}
		else if (gui_check_obj(name, "btn_gain_type"))
		{
			gain_mode = ! gain_mode;
			iio_ad936x_set_gain(gain_mode, gain_val);
			update = 1;
		}
		else if (gui_check_obj(name, "btn_gain_add") || gui_check_obj(name, "btn_gain_sub"))
		{
			int p = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);

			if (gain_val + p > 3 || gain_val + p < 70)
			{
				gain_val += p;
				iio_ad936x_set_gain(gain_mode, gain_val);
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

		gui_obj_set_prop("lbl_iio_val", GUI_OBJ_TEXT_FMT, "%s", uri);
		gui_obj_set_prop("btn_gain_type", GUI_OBJ_STATE, status != 2 ? DISABLED : CANCELLED);
		gui_obj_set_prop("btn_gain_add", GUI_OBJ_STATE, (status == 2 && ! gain_mode) ? CANCELLED : DISABLED);
		gui_obj_set_prop("btn_gain_sub", GUI_OBJ_STATE, (status == 2 && ! gain_mode) ? CANCELLED : DISABLED);

		if (gain_mode)
			gui_obj_set_prop("btn_gain_type", GUI_OBJ_TEXT, "Gain|auto");
		else
			gui_obj_set_prop("btn_gain_type", GUI_OBJ_TEXT_FMT, "Gain|%d dB", gain_val);

		if (status < 10)
		{
			gui_obj_set_prop("lbl_status_str", GUI_OBJ_TEXT_FMT, "%s",  status_str[status]);
			gui_obj_set_prop("btn_action", GUI_OBJ_TEXT_FMT, "%s", button_str[status]);
		}
	}
}
#elif WITHAD936XDEV
void window_ad936x_process(void)
{
	window_t * const win = get_win(WINDOW_AD936X);
	static uint32_t freq = 7012000;

	if (win->first_call)
	{
		win->first_call = 0;
		uint8_t p = ad936xdev_present();
		uint8_t s = get_ad936x_stream_status();

		gui_obj_create("btn_switch", 100, 40, 0, 0, "");

		if (s)
			gui_obj_set_prop("btn_switch", GUI_OBJ_TEXT, "Switch|to HF");
		else
		{
			gui_obj_set_prop("btn_switch", GUI_OBJ_TEXT_FMT, "%s", p ? "Switch|to UHF" : "AD936x|not found");
			gui_obj_set_prop("btn_switch", GUI_OBJ_STATE, p ? CANCELLED : DISABLED);
		}

		calculate_window_position(win, WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_switch"))
			{
				if (get_ad936x_stream_status())
				{
					gui_obj_set_prop("btn_switch", GUI_OBJ_TEXT, "Switch|to UHF");
					ad936xdev_sleep();
					hamradio_set_freq(freq);
				}
				else
				{
					gui_obj_set_prop("btn_switch", GUI_OBJ_TEXT, "Switch|to HF");
					ad936xdev_wake();
					freq = hamradio_get_freq_rx();
					hamradio_set_freq(433000000);
					ad936x_set_rtstune_offset(0);
				}
			}
		}

	default:
		break;
	}
}
#else
void window_ad936x_process(void) {}
#endif /* WITHAD936XDEV */

void window_as_process(void)
{
#if WITHAUDIOSAMPLESREC
	window_t * const win = get_win(WINDOW_AS);
	static unsigned update = 0;
	enum { len = 320, lim = 25 };
	static COLORPIP_T d [len];

	if (win->first_call)
	{
		win->first_call = 0;
		unsigned x = 0, y = 65, interval = 10;

		gui_obj_create("btn_rec", 100, 40, 0, 0, "Record");
		gui_obj_create("btn_play", 100, 40, 0, 0, "Play");
		gui_obj_create("btn_tx", 100, 40, 0, 0, "Transmit");

		gui_obj_set_prop("btn_rec", GUI_OBJ_POS, x, y);
		gui_obj_align_to("btn_play", "btn_rec", ALIGN_RIGHT_UP, interval);
		gui_obj_align_to("btn_tx", "btn_play", ALIGN_RIGHT_UP, interval);

		if (as_get_state() == AS_IDLE)
			memset(d, 0, ARRAY_SIZE(d));

		calculate_window_position(win, WINDOW_POSITION_AUTO);
		update = 1;
	}

	for (int x = 0; x < len; x ++)
		gui_drawline(x, lim - d[x], x, lim + d[x], COLORPIP_WHITE);

	if (as_get_state() == AS_PLAYING || as_get_state() == AS_TX)
	{
		uint16_t pos = len * as_get_progress() * 0.01;
		gui_drawline(pos, 0, pos, lim *2, COLORPIP_GREEN);
	}


	GET_FROM_WM_QUEUE
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_rec"))
			{
				as_toggle_record();
				update = 1;
			}
			else if (gui_check_obj(name, "btn_play"))
			{
				as_toggle_play();
				update = 1;
			}
			else if (gui_check_obj(name, "btn_tx"))
			{
				as_toggle_trx();
				update = 1;
			}
		}

	case WM_MESSAGE_UPDATE:

		update = 1;
		break;

	default:
		break;
	}

	if (update)
	{
		update = 0;
		uint8_t s = as_get_state();
		char buf[20];

		gui_obj_set_prop("btn_rec", GUI_OBJ_STATE, (s == AS_PLAYING || s == AS_TX) ? DISABLED : CANCELLED);
		gui_obj_set_prop("btn_play", GUI_OBJ_STATE, (s == AS_PLAYING || s == AS_READY) ? CANCELLED : DISABLED);
		gui_obj_set_prop("btn_tx", GUI_OBJ_STATE, (s == AS_PLAYING || s == AS_RECORDING || s == AS_IDLE) ? DISABLED : CANCELLED);

		if (s == AS_PLAYING)
			gui_obj_set_prop("btn_play", GUI_OBJ_TEXT_FMT, "Playing...|%d%%", as_get_progress());
		else
			gui_obj_set_prop("btn_play", GUI_OBJ_TEXT, "Play");

		if (s == AS_RECORDING)
			gui_obj_set_prop("btn_rec", GUI_OBJ_TEXT_FMT, "Recording...|%d%%", as_get_progress());
		else
			gui_obj_set_prop("btn_rec", GUI_OBJ_TEXT, "Record");

		if (s == AS_TX)
			gui_obj_set_prop("btn_tx", GUI_OBJ_TEXT_FMT, "Transmit...|%d%%", as_get_progress());
		else
			gui_obj_set_prop("btn_tx", GUI_OBJ_TEXT, "Transmit");

		static uint8_t olds = AS_IDLE;

		if (s == AS_READY && olds == AS_RECORDING)
			as_draw_spectrogram(d, len, lim);

		olds = s;
	}
#endif /* WITHAUDIOSAMPLESREC */
}

#endif /* WITHTOUCHGUI */
