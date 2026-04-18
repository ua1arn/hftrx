// Simple GUI от RA4ASN

#include "gui_port_include.h"

#if WITHTOUCHGUI

#include "gui.h"
#include "framework/gui_system.h"
#include "framework/gui_structs.h"
#include "framework/gui_settings.h"
#include "framework/gui_windows.h"
#include "framework/gui_objects.h"
#include "framework/utils.h"
#include "gui_user.h"

val_step_t enc2step[] = {
	{ 100, "100 Hz", },
	{ 500, "500 Hz", },
};

val_step_t freq_swipe_step[] = {
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

static tf_entry_t tmpbuf[TEXT_ARRAY_SIZE];
static uint8_t tmpstr_index = 0, debug_view = 0;

//todo: перенести раскладку инфобара в конфиг
static uint8_t infobar_selected = UINT8_MAX;
const uint8_t infobar_places[infobar_num_places] = {
#if defined(INFOBAR_CUSTOM)
		INFOBAR_CUSTOM
#else
		INFOBAR_AF,
		INFOBAR_AF_VOLUME,
		INFOBAR_ATT,
		INFOBAR_DNR,
		INFOBAR_TX_POWER,
#if WITHVOLTLEVEL || WITHVOLTLEVEL_1117
		INFOBAR_VOLTAGE | INFOBAR_NOACTION,
#elif WITHUSEDUALWATCH
		INFOBAR_DUAL_RX,
#else
		INFOBAR_EMPTY,
#endif /* WITHPS7BOARD_EBAZ4205 */
		INFOBAR_CPU_TEMP | INFOBAR_NOACTION,
		INFOBAR_2ND_ENC_MENU
#endif /* defined(INFOBAR_CUSTOM) */
};

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

	if (gui_nvram.micprofile != micprofile_default && gui_nvram.micprofile < NMICPROFCELLS)
		hamradio_load_mic_profile(gui_nvram.micprofile, 1);
	else
		gui_nvram.micprofile = micprofile_default;
}

void save_settings(void)
{
	hamradio_save_gui_settings(& gui_nvram);
}

#if WITHGUIDEBUG
void gui_add_debug(char d)
{
	static char str[TEXT_ARRAY_SIZE] = { 0 };
	static uint8_t i = 0;

	if (d == '\r')
	{
		i = 0;
		return;
	}

	if (d != '\n')
	{
		if (i < TEXT_ARRAY_SIZE)
			str[i ++ ] = d;
	}
	else
	{
		i = 0;
		if (debug_view)
			textfield_add_string("tf_debug", str, COLORPIP_WHITE);
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
	debug_view = ! debug_view;
	gui_obj_set_prop("tf_debug", GUI_OBJ_VISIBLE, debug_view);
}
#endif /* WITHGUIDEBUG */

void gui_user_actions_after_close_window(void)
{
	hamradio_disable_encoder2_redirect();
	hamradio_set_lock(0);
}

static void keyboard_edit_string(uintptr_t s, uint8_t strlen, uint8_t clean)
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
	uint8_t interval = 5, yy = 0, need_close = 0, need_open = 255;
	uint8_t infobar = infobar_places[infobar_selected] & INFOBAR_VALID_MASK;
	char btn_name[NAME_ARRAY_SIZE];

	if (is_win_init())
	{
		for (uint8_t i = 0; i < 6; i ++)
		{
			local_snprintf_P(btn_name, NAME_ARRAY_SIZE, "btn_%d", i);
			gui_obj_create(btn_name, 86, 30, 0, 0, "");
			gui_obj_set_prop(btn_name, GUI_OBJ_VISIBLE, 0);
			gui_obj_set_prop(btn_name, GUI_OBJ_FONT, & msgothic_13x16_prop);
		}

		switch (infobar)
		{
#if WITHUSEDUALWATCH
		case INFOBAR_DUAL_RX:
		{
			for (uint8_t i = 0; i < 3; i ++)
			{
				local_snprintf_P(btn_name, ARRAY_SIZE(btn_name), "btn_%d", i);
				gui_obj_set_prop(btn_name, GUI_OBJ_VISIBLE, 1);
				gui_obj_set_prop(btn_name, GUI_OBJ_PAYLOAD, i + 1);

				if (i == 0)
				{
					uint_fast8_t state;
					hamradio_get_vfomode3_value(& state);
					gui_obj_set_prop(btn_name, GUI_OBJ_LOCK, state != 0);
					gui_obj_set_prop(btn_name, GUI_OBJ_TEXT, "SPLIT");
				}
				else if (i == 1)
					gui_obj_set_prop(btn_name, GUI_OBJ_TEXT, "A<->B");
				else if (i == 2)
					gui_obj_set_prop(btn_name, GUI_OBJ_TEXT, hamradio_get_mainsubrxmode3_value_P());
			}

			gui_arrange_objects_from("btn_0", 3, 1, 6);
		}
			break;
#endif /* WITHUSEDUALWATCH */
		case INFOBAR_AF_VOLUME:
		{
			uint8_t i = 0;
#if ! WITHPOTAFGAIN
			local_snprintf_P(btn_name, ARRAY_SIZE(btn_name), "btn_%d", i ++);
			gui_obj_set_prop(btn_name, GUI_OBJ_VISIBLE, 1);
			gui_obj_set_prop(btn_name, GUI_OBJ_PAYLOAD, 1);
			gui_obj_set_prop(btn_name, GUI_OBJ_TEXT, "+");

			local_snprintf_P(btn_name, ARRAY_SIZE(btn_name), "btn_%d", i ++);
			gui_obj_set_prop(btn_name, GUI_OBJ_VISIBLE, 1);
			gui_obj_set_prop(btn_name, GUI_OBJ_PAYLOAD, -1);
			gui_obj_set_prop(btn_name, GUI_OBJ_TEXT, "-");
#endif /* ! WITHPOTAFGAIN */
			local_snprintf_P(btn_name, ARRAY_SIZE(btn_name), "btn_%d", i ++);
			gui_obj_set_prop(btn_name, GUI_OBJ_VISIBLE, 1);
			gui_obj_set_prop(btn_name, GUI_OBJ_PAYLOAD, 10);
			gui_obj_set_prop(btn_name, GUI_OBJ_TEXT, "mute");
			gui_obj_set_prop(btn_name, GUI_OBJ_LOCK, hamradio_get_gmutespkr());
#if WITHAFEQUALIZER
			local_snprintf_P(btn_name, ARRAY_SIZE(btn_name), "btn_%d", i ++);
			gui_obj_set_prop(btn_name, GUI_OBJ_VISIBLE, 1);
			gui_obj_set_prop(btn_name, GUI_OBJ_PAYLOAD, 20);
			gui_obj_set_prop(btn_name, GUI_OBJ_TEXT, "EQ");
#endif /* WITHAFEQUALIZER */

			gui_arrange_objects_from("btn_0", i, 1, 6);
		}
			break;

		case INFOBAR_ATT:
		{
			uint_fast8_t atts[6];
			uint8_t count = hamradio_get_att_dbs(atts, 6);

			for (uint8_t i = 0; i < count; i ++)
			{
				local_snprintf_P(btn_name, ARRAY_SIZE(btn_name), "btn_%d", i);
				gui_obj_set_prop(btn_name, GUI_OBJ_VISIBLE, 1);
				gui_obj_set_prop(btn_name, GUI_OBJ_PAYLOAD, i);
				if (atts[i])
					gui_obj_set_prop(btn_name, GUI_OBJ_TEXT_FMT, "%d db", atts[i] / 10);
				else
					gui_obj_set_prop(btn_name, GUI_OBJ_TEXT, "off");
			}

			gui_arrange_objects_from("btn_0", count, 1, 6);
		}
			break;

		case INFOBAR_AF:
		{
			bws_t bws;
			uint8_t count = hamradio_get_bws(& bws, 5);

			for (uint8_t i = 0; i < 6; i ++)
			{
				local_snprintf_P(btn_name, ARRAY_SIZE(btn_name), "btn_%d", i);
				gui_obj_set_prop(btn_name, GUI_OBJ_VISIBLE, 1);

				if (i >= count)
				{
					gui_obj_set_prop(btn_name, GUI_OBJ_PAYLOAD, 255);
					gui_obj_set_prop(btn_name, GUI_OBJ_TEXT, "Filter");
					break;
				}

				gui_obj_set_prop(btn_name, GUI_OBJ_PAYLOAD, i);
				gui_obj_set_prop(btn_name, GUI_OBJ_TEXT_FMT, "%s", bws.label[i]);
			}

			gui_arrange_objects_from("btn_0", count + 1, 1, 6); // +1 - кнопка настраиваемого фильтра
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

		calculate_window_position(WINDOW_POSITION_MANUAL_POSITION, infobar_selected * infobar_label_width, infobar_2nd_str_y + SMALLCHARH2);
	}

	GET_FROM_WM_QUEUE(WINDOW_INFOBAR_MENU)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			int32_t p = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);

			switch (infobar)
			{
#if WITHUSEDUALWATCH
			case INFOBAR_DUAL_RX:
			{
				if (p == 1) // SPLIT on/off
					gui_obj_set_prop(name, GUI_OBJ_LOCK, hamradio_split_toggle());
				else if (p == 2) // SPLIT swap
					hamradio_split_vfo_swap();
				else if (p == 3) // SPLIT mode
				{
					hamradio_split_mode_toggle();
					gui_obj_set_prop(name, GUI_OBJ_TEXT, hamradio_get_mainsubrxmode3_value_P());
				}
			}
				break;
#endif /* WITHUSEDUALWATCH */
			case INFOBAR_AF_VOLUME:
			{
#if ! WITHPOTAFGAIN
				if (p == -1 || p == 1)
					hamradio_set_afgain(hamradio_get_afgain() + p);
#endif /* ! WITHPOTAFGAIN */
				if (p == 10)
				{
					hamradio_set_gmutespkr(! hamradio_get_gmutespkr());
					gui_obj_set_prop(name, GUI_OBJ_LOCK, hamradio_get_gmutespkr());
				}

				if (p == 20)
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
				hamradio_set_att_db(p);
				close_all_windows();
			}
				break;

			case INFOBAR_AF:
			{
				if (p == 255)
				{
					close_all_windows();
					open_window(get_win(WINDOW_AF));
					footer_buttons_state(DISABLED, NULL);
				}
				else
				{
					hamradio_set_bw(p);
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
			open_window(get_win(WINDOW_TX_POWER));
			footer_buttons_state(DISABLED, NULL);
		}
			break;

		default:
			break;
		}
	}

	if (need_close)
		close_all_windows();
}

// *********************************************************************************************************************************************************************

/* Установки статуса основных кнопок */
/* При DISABLED в качестве необязательного параметра передать имя активной кнопки или NULL для блокирования всех */
void footer_buttons_state(uint_fast8_t state, ...)
{
	char name[NAME_ARRAY_SIZE];
	va_list arg;
	static uint_fast16_t bitmask_locked_buttons = 0;
	char * bt_name = NULL;

	if (state == DISABLED)
	{
		va_start(arg, state);
		bt_name = va_arg(arg, char *);
		va_end(arg);
	}

	if (state == DISABLED && bt_name)
		bitmask_locked_buttons = 0;

	for (uint8_t i = 0; i < 9; i ++)
	{
		local_snprintf_P(name, NAME_ARRAY_SIZE, "btn_main%02d", i);

		if (state == DISABLED && bt_name)
			bitmask_locked_buttons |= gui_obj_get_int_prop(name, GUI_OBJ_LOCK) << i;

		if (state == DISABLED)
		{
			uint8_t state = safe_strcmp(name, bt_name) ? DISABLED : CANCELLED;
			gui_obj_set_prop(name, GUI_OBJ_STATE, state);
			gui_obj_set_prop(name, GUI_OBJ_LOCK, state == CANCELLED ? BUTTON_LOCKED : BUTTON_NON_LOCKED);
		}
		else if (state == CANCELLED && get_parent_window() == NO_PARENT_WINDOW)
		{
			gui_obj_set_prop(name, GUI_OBJ_STATE, CANCELLED);
			gui_obj_set_prop(name, GUI_OBJ_LOCK, (bitmask_locked_buttons >> i) & 1);
		}
	}
}

void gui_main_process(void)
{
	char buf[TEXT_ARRAY_SIZE];
	const uint8_t buflen = ARRAY_SIZE(buf);
	uint8_t update = 0;
	static uint8_t freq_swipe;

	if (is_winmain_init())
	{
		gui_enc2_menu.updated = 1;
		gui_keyboard.clean = 0;
		gui_keyboard.digits_only = 0;
		update = 1;

#if WITHGUIDEBUG
		gui_obj_create("tf_debug", 70, 25, DOWN, & msgothic_11x13_mono);
		gui_obj_set_prop("tf_debug", GUI_OBJ_VISIBLE, 0);

		uint16_t w = gui_obj_get_int_prop("tf_debug", GUI_OBJ_WIDTH);
		uint16_t h = gui_obj_get_int_prop("tf_debug", GUI_OBJ_HEIGHT);
#endif /* WITHGUIDEBUG */

		gui_obj_create("btn_main00", COMMON_BUTTON_STYLE, 0, 1, "RX");				// btn_txrx
		gui_obj_create("btn_main01", COMMON_BUTTON_STYLE, 0, 0, "Bands");			// btn_Bands
		gui_obj_create("btn_main02", COMMON_BUTTON_STYLE, 0, 0, "Memory");			// btn_Memory
		gui_obj_create("btn_main03", COMMON_BUTTON_STYLE, 0, 0, "Receive|options");	// btn_Receive
		gui_obj_create("btn_main04", COMMON_BUTTON_STYLE, 0, 1, "");				// btn_notch
		gui_obj_create("btn_main05", COMMON_BUTTON_STYLE, 0, 0, "Speaker|on air");	// btn_speaker
		gui_obj_create("btn_main06", COMMON_BUTTON_STYLE, 0, 0, "");				// btn_var
		gui_obj_create("btn_main07", COMMON_BUTTON_STYLE, 0, 0, "");				// btn_ft8
		gui_obj_create("btn_main08", COMMON_BUTTON_STYLE, 0, 0, "Options");			// btn_Options

#if WITHAUDIOSAMPLESREC
		gui_obj_set_prop("btn_main06", GUI_OBJ_TEXT, "AF|samples");
#endif /* WITHAUDIOSAMPLESREC */

		for (int i = 0; i < infobar_num_places; i ++)
		{
			local_snprintf_P(buf, buflen, "ta_infobar_%d", i);
			gui_obj_create(buf, i * infobar_label_width, infobar_1st_str_y, infobar_label_width, infobar_2nd_str_y, 0);
		}

		uint16_t p = infobar_2nd_str_y + get_strheight_prop(& INFOBAR_FONTP) + 5;
		gui_obj_create("ta_freq", 0, p, WITHGUIMAXX, WITHGUIMAXY - FOOTER_HEIGHT - p, 1);

		char * btn0 = get_obj_name_by_idx(TYPE_BUTTON, 0);
		gui_obj_set_prop(btn0, GUI_OBJ_POS, 0, WITHGUIMAXY - common_btn_height - 1);
		gui_arrange_objects_from(btn0, 9, 9, common_btn_interval);

		load_settings();
		hamradio_set_lock(0);
#if WITHLFM
		hamradio_set_lfmmode(0);
#endif /* WITHLFM */

#if WITHGUIDEBUG
		uint16_t winw = gui_get_window_draw_width();
		uint16_t winh = gui_get_window_draw_height();

		gui_obj_set_prop("tf_debug", GUI_OBJ_POS, winw / 2 - w / 2, winh / 2 - h / 2);

		for (uint8_t i = 0; i < tmpstr_index; i ++)
			textfield_add_string("tf_debug", tmpbuf[i].text, COLORPIP_WHITE);
#endif /* WITHGUIDEBUG */

		calculate_window_position(WINDOW_POSITION_FULLSCREEN);
		return;
	}

	GET_FROM_WM_QUEUE(WINDOW_MAIN)
	{
	case WM_MESSAGE_ACTION:
		if (IS_AREA_MOVE)
		{
			if (gui_check_obj(name, "ta_freq") && gui_nvram.freq_swipe_enable && get_parent_window() == NO_PARENT_WINDOW)
			{
				int_fast16_t move_x = 0, move_y = 0;
				get_gui_tracking(& move_x, & move_y);
				if (move_x != 0)
					hamradio_set_freq(hamradio_get_freq_rx() - (int) (move_x  * freq_swipe));
			}
		}

		if (IS_AREA_TOUCHED)
		{
			if (gui_check_obj(name, "ta_freq")) break;

			infobar_selected = gui_obj_get_int_prop(name, GUI_OBJ_INDEX);
			GUI_ASSERT(infobar_selected < infobar_num_places);
			uint8_t infobar = infobar_places[infobar_selected];

			if (infobar != INFOBAR_EMPTY && ! ((infobar & INFOBAR_NOACTION) >> INFOBAR_NOACTION_POS))
			{
				if (get_parent_window() == WINDOW_INFOBAR_MENU)
				{
					close_window(DONT_OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
				}
				else if (get_parent_window() == NO_PARENT_WINDOW)
				{
					open_window(get_win(WINDOW_INFOBAR_MENU));
					footer_buttons_state(DISABLED, NULL);
				}
			}
		}

		if (IS_BUTTON_PRESS)	// обработка короткого нажатия кнопок
		{
			if (gui_check_obj(name, "btn_main04"))
			{
				hamradio_set_gnotch(! hamradio_get_gnotch());
				update = 1;
			}
#if WITHSPKMUTE
			else if (gui_check_obj(name, "btn_main05"))
			{
				hamradio_set_gmutespkr(! hamradio_get_gmutespkr());
				update = 1;
			}
#endif /* #if WITHSPKMUTE */
			else if (gui_check_obj(name, "btn_main01"))
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
					footer_buttons_state(DISABLED, "btn_main01");
				}
			}
			else if (gui_check_obj(name, "btn_main02"))
			{
				window_t * const win = get_win(WINDOW_MEMORY);
				if (win->state == NON_VISIBLE)
				{
					open_window(win);
					footer_buttons_state(DISABLED, "btn_main02");
				}
				else
				{
					close_window(OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
				}
			}
			else if (gui_check_obj(name, "btn_main08"))
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
					footer_buttons_state(DISABLED, "btn_main08");
				}
			}
			else if (gui_check_obj(name, "btn_main03"))
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
					footer_buttons_state(DISABLED, "btn_main03");
				}
			}
#if WITHAUDIOSAMPLESREC
			else if (gui_check_obj(name, "btn_main06"))
			{
				window_t * const win = get_win(WINDOW_AS);
				if (win->state == NON_VISIBLE)
				{
					open_window(win);
					footer_buttons_state(DISABLED, "btn_main06");
				}
				else
				{
					close_window(OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
				}
			}
#endif /* WITHAUDIOSAMPLESREC */
#if WITHTX
			else if (gui_check_obj(name, "btn_main00"))
			{
				hamradio_moxmode(1);
				update = 1;
			}
#endif /* WITHTX */
		}
		else if (IS_BUTTON_LONG_PRESS)			// обработка длинного нажатия
		{
#if WITHTX
			if (gui_check_obj(name, "btn_main00"))
			{
				hamradio_tunemode(1);
				update = 1;
			}
			else
#endif /* WITHTX */
			if (gui_check_obj(name, "btn_main04"))
			{
				window_t * const win = get_win(WINDOW_NOTCH);
				if (win->state == NON_VISIBLE)
				{
					open_window(win);
					footer_buttons_state(DISABLED, "btn_main04");
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
		uint16_t step = enc2step[gui_nvram.enc2step_pos].step;
		uint32_t freq = hamradio_get_freq_rx();
		uint32_t f_rem = freq % step;

		if (action > 0)
			hamradio_set_freq(freq + (step - f_rem) * abs(action));
		else if (action < 0)
			hamradio_set_freq(freq - (f_rem ? f_rem : step) * abs(action));
	}
		break;

	default:
		break;
	}

	if (update)											// обновление состояния элементов при действиях с ними, а также при запросах из базовой системы
	{
		freq_swipe = freq_swipe_step[gui_nvram.freq_swipe_step].step;

		gui_obj_set_prop("btn_main04", GUI_OBJ_LOCK, hamradio_get_gnotch());
		uint8_t notch_type = hamradio_get_gnotchtype();
		if (notch_type == 1)
			gui_obj_set_prop("btn_main04", GUI_OBJ_TEXT, "Notch|manual");
		else if (notch_type == 2)
			gui_obj_set_prop("btn_main04", GUI_OBJ_TEXT, "Notch|auto");

#if WITHSPKMUTE
		uint8_t s = hamradio_get_gmutespkr();
		gui_obj_set_prop("btn_main05", GUI_OBJ_LOCK, s);
		gui_obj_set_prop("btn_main05", GUI_OBJ_TEXT_FMT, "Speaker|%s", s ? "muted" : "on air");
#else
		gui_obj_set_prop("btn_main05", GUI_OBJ_STATE, DISABLED);
#endif /* #if WITHSPKMUTE */

#if WITHTX
		uint8_t tune = hamradio_tunemode(0);
		uint8_t mox = hamradio_moxmode(0);

		if (tune)
		{
			gui_obj_set_prop("btn_main00", GUI_OBJ_LOCK, BUTTON_LOCKED);
			gui_obj_set_prop("btn_main00", GUI_OBJ_TEXT, "TX|tune");
		}
		else if (! tune && mox)
		{
			gui_obj_set_prop("btn_main00", GUI_OBJ_LOCK, BUTTON_LOCKED);
			gui_obj_set_prop("btn_main00", GUI_OBJ_TEXT, "TX");
		}
		else if (! tune && ! mox)
		{
			gui_obj_set_prop("btn_main00", GUI_OBJ_LOCK, BUTTON_NON_LOCKED);
			gui_obj_set_prop("btn_main00", GUI_OBJ_TEXT, "RX");
		}
#else
		gui_obj_set_prop("btn_main00", GUI_OBJ_STATE, DISABLED);
		gui_obj_set_prop("btn_main00", GUI_OBJ_TEXT, "RX");
#endif /* WITHTX */

#if defined (RTC1_TYPE)
		board_rtc_cached_getdatetime(& year, & month, & day, & hour, & minute, & seconds);
#endif /* defined (RTC1_TYPE) */
	}

	// разметка инфобара
	const uint16_t y_mid = infobar_1st_str_y + (infobar_2nd_str_y - infobar_1st_str_y) / 2;
	const uint16_t infobar_hl = (infobar_selected < infobar_num_places) && (get_parent_window() == WINDOW_INFOBAR_MENU);

	for(uint8_t i = 1; i < infobar_num_places; i++)
	{
		uint_fast16_t x = infobar_label_width * i;
		gui_drawline(x, infobar_1st_str_y, x, infobar_2nd_str_y + SMALLCHARH2, COLORPIP_GREEN);
	}

	if (infobar_hl)
	{
		uint16_t x = infobar_selected * infobar_label_width + 2;
		uint16_t w = infobar_label_width - 4;
		uint16_t h = infobar_2nd_str_y - infobar_1st_str_y + SMALLCHARH2 - 2;

		gui_drawrect_rounded(x, infobar_1st_str_y, w, h, 5, COLORPIP_YELLOW, 1);
	}

	for (uint8_t current_place = 0; current_place < infobar_num_places; current_place ++)
	{
		uint_fast8_t infobar = infobar_places[current_place] & INFOBAR_VALID_MASK;
		gui_color_t str_color = (current_place == infobar_selected) && infobar_hl ? COLORPIP_BLACK : COLORPIP_WHITE;
		uint16_t strl;

		switch (infobar)
		{
		case INFOBAR_DUAL_RX:
		{
#if WITHUSEDUALWATCH
			static uint_fast8_t val = 0;
			uint16_t xx = current_place * infobar_label_width + infobar_label_width / 2;

			if (update)
				hamradio_get_vfomode3_value(& val);

			local_snprintf_P(buf, buflen, "Dual RX");
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_1st_str_y, buf, & INFOBAR_FONTP, str_color);
			local_snprintf_P(buf, buflen, "VFO %s", hamradio_get_gvfoab() ? "2" : "1");
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_2nd_str_y, buf, & INFOBAR_FONTP, str_color);
#endif /* WITHUSEDUALWATCH */
		}
			break;

		case INFOBAR_AF_VOLUME:
		{
			static uint_fast8_t vol;
			uint16_t xx = current_place * infobar_label_width + infobar_label_width / 2;

			if (update) vol = hamradio_get_afgain();

			local_snprintf_P(buf, buflen, "AF gain");
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_1st_str_y, buf, & INFOBAR_FONTP, str_color);
			if (hamradio_get_gmutespkr())
				local_snprintf_P(buf, buflen, "muted");
			else
				local_snprintf_P(buf, buflen, "%d", vol);
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_2nd_str_y, buf, & INFOBAR_FONTP , str_color);
		}
			break;

		case INFOBAR_TX_POWER:
		{
		#if WITHTX
			static uint_fast8_t tx_pwr, tune_pwr;
			uint16_t xx = current_place * infobar_label_width + infobar_label_width / 2;

			if (update)
			{
				tx_pwr = hamradio_get_tx_power();
				tune_pwr = hamradio_get_tx_tune_power();
			}

			local_snprintf_P(buf, buflen, "TX %d\%%", (int) tx_pwr);
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_1st_str_y, buf, & INFOBAR_FONTP, str_color);
			local_snprintf_P(buf, buflen, "Tune %d\%%", (int) tune_pwr);
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_2nd_str_y, buf, & INFOBAR_FONTP, str_color);
		#endif /* WITHTX */
					}
			break;

		case INFOBAR_DNR:
		{
			static int_fast32_t grade = 0;
			static uint_fast8_t state = 0;
			uint16_t xx = current_place * infobar_label_width + infobar_label_width / 2;

			if (update) state = hamradio_get_nrvalue(& grade);

			local_snprintf_P(buf, buflen, "DNR");
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_1st_str_y, buf, & INFOBAR_FONTP, state ? str_color : COLORPIP_GRAY);
			local_snprintf_P(buf, buflen, state ? "on" : "off");
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_2nd_str_y, buf, & INFOBAR_FONTP, state ? str_color : COLORPIP_GRAY);
		}

			break;

		case INFOBAR_AF:
		// параметры полосы пропускания фильтра
		{
			static uint_fast8_t bp_wide;
			static uint_fast16_t bp_low, bp_high;
			uint16_t xx = current_place * infobar_label_width + 7;

			if (update)
			{
				bp_wide = hamradio_get_bp_type_wide();
				bp_high = hamradio_get_high_bp(0);
				bp_low = hamradio_get_low_bp(0) * 10;
				bp_high = bp_wide ? (bp_high * 100) : (bp_high * 10);
			}

			local_snprintf_P(buf, buflen, "AF");
			gui_print_prop(xx, y_mid, buf, & INFOBAR_FONTP, str_color);
			xx += 30;
			local_snprintf_P(buf, buflen, bp_wide ? "L %u" : "W %u", bp_low);
			gui_print_prop(xx, infobar_1st_str_y, buf, & INFOBAR_FONTP, str_color);
			local_snprintf_P(buf, buflen, bp_wide ? "H %u" : "P %u", bp_high);
			gui_print_prop(xx, infobar_2nd_str_y, buf, & INFOBAR_FONTP, str_color);
		}
			break;

		case INFOBAR_IF_SHIFT:
		// значение сдвига частоты
		{
			static int16_t if_shift;
			uint16_t xx = current_place * infobar_label_width + infobar_label_width / 2;

			if (update) if_shift = hamradio_if_shift(0);

			if (if_shift)
			{
				local_snprintf_P(buf, buflen, "IF shift");
				strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
				gui_print_prop(xx - strl / 2, infobar_1st_str_y, buf, & INFOBAR_FONTP, str_color);
				local_snprintf_P(buf, buflen, if_shift == 0 ? "%d" : "%+d Hz", if_shift);
				strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
				gui_print_prop(xx - strl / 2, infobar_2nd_str_y, buf, & INFOBAR_FONTP, str_color);
			}
			else
			{
				local_snprintf_P(buf, buflen, "IF shift");
				strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
				gui_print_prop(xx - strl / 2, y_mid, buf, & INFOBAR_FONTP, COLORPIP_GRAY);
			}
		}
			break;

		case INFOBAR_ATT:
		// attenuator
		{
			static uint8_t atten;
			uint16_t xx = current_place * infobar_label_width + infobar_label_width / 2;

			if (update) atten  = hamradio_get_att_db();

			local_snprintf_P(buf, buflen, "ATT");
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_1st_str_y, buf, & INFOBAR_FONTP, str_color);
			if (atten)
				local_snprintf_P(buf, buflen, "%d db", atten);
			else
				local_snprintf_P(buf, buflen, "off");

			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_2nd_str_y, buf, & INFOBAR_FONTP, str_color);
		}
			break;

		case INFOBAR_SPAN:
		// ширина панорамы
		{
#if WITHIF4DSP
			static int_fast32_t z;
			uint16_t xx = current_place * infobar_label_width + infobar_label_width / 2;

			if (update) z = display2_zoomedbw() / 1000;

			local_snprintf_P(buf, buflen, "SPAN");
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_1st_str_y, buf, & INFOBAR_FONTP, str_color);
			local_snprintf_P(buf, buflen, "%dk", z);
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_2nd_str_y, buf, & INFOBAR_FONTP, str_color);
#endif /* WITHIF4DSP */
		}
			break;

		case INFOBAR_VOLTAGE:
		{
#if WITHVOLTLEVEL || WITHVOLTLEVEL_1117
			// напряжение питания
			static ldiv_t v;
			uint16_t xx = current_place * infobar_label_width + infobar_label_width / 2;

			if (update) v = ldiv(hamradio_get_volt_value(), 10);

#if WITHCURRLEVEL || WITHCURRLEVEL2 || WITHCURRLEVEL_1117
			uint_fast16_t yy = hamradio_get_tx() ? infobar_1st_str_y : y_mid;
#else
			uint_fast16_t yy = y_mid;
#endif /* WITHCURRLEVEL || WITHCURRLEVEL2 */

			local_snprintf_P(buf, buflen, "%d.%1dV", (int) v.quot, (int) v.rem);
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, yy, buf, & INFOBAR_FONTP, str_color);
#endif /* WITHVOLTLEVEL || WITHVOLTLEVEL_1117 */

#if WITHCURRLEVEL || WITHCURRLEVEL2 || WITHCURRLEVEL_1117
		// ток PA (при передаче)
			if (hamradio_get_tx())
			{
				static int16_t drain;
				if (update)
					drain = hamradio_get_pacurrent_value();	// Ток в десятках милиампер (может быть отрицательным - для чего?)

#if (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A)
				// для больших токов (более 9 ампер)
				const div_t t = div(drain / 10, 10);
				local_snprintf_P(buf, buflen, "%2d.%01dA", t.quot, abs(t.rem));

#else /* (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A) */
				// Датчик тока до 5 ампер
				const div_t t = div(drain, 100);
				local_snprintf_P(buf, buflen, "%d.%02dA", t.quot, abs(t.rem));

#endif /* (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A) */

				strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
				gui_print_prop(xx - strl / 2, infobar_2nd_str_y, buf, & INFOBAR_FONTP, str_color);
			}
#endif /* WITHCURRLEVEL */
		}
			break;

		case INFOBAR_CPU_TEMP:
		{
#if defined (GET_CPU_TEMPERATURE)
			// вывод температуры процессора, если поддерживается
			static float cpu_temp = 0;
			uint16_t xx = current_place * infobar_label_width + infobar_label_width / 2;

			if (update) cpu_temp = GET_CPU_TEMPERATURE();

			local_snprintf_P(buf, buflen, "CPU temp");
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_1st_str_y, buf, & INFOBAR_FONTP, COLORPIP_WHITE);
			local_snprintf_P(buf, buflen, "%2.1f", cpu_temp);
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_2nd_str_y, buf, & INFOBAR_FONTP, cpu_temp > 60.0 ? COLORPIP_RED : COLORPIP_WHITE);
#endif /* defined (GET_CPU_TEMPERATURE) */
		}
			break;

		case INFOBAR_BATTERY:
		{
#if defined (GET_BATTERY_CAPACITY)
			static int capacity = 0;
			uint16_t xx = current_place * infobar_label_width + infobar_label_width / 2;

			if (update) capacity = GET_BATTERY_CAPACITY();

			local_snprintf_P(buf, buflen, "Battery");
			strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
			gui_print_prop(xx - strl / 2, infobar_1st_str_y, buf, & INFOBAR_FONTP, COLORPIP_WHITE);
			if (capacity >= 0)
			{
				local_snprintf_P(buf, buflen, "%d%%", capacity);
				strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
				gui_print_prop(xx - strl / 2, infobar_2nd_str_y, buf, & INFOBAR_FONTP, capacity < 30 ? COLORPIP_RED : COLORPIP_WHITE);
			}
			else
				gui_print_prop(xx - strl / 2, infobar_2nd_str_y, "error", & INFOBAR_FONTP, COLORPIP_RED);
#endif /* defined (GET_BATTERY_CAPACITY) */
		}
			break;

		case INFOBAR_2ND_ENC_MENU:
		{
			// быстрое меню 2-го энкодера
			hamradio_gui_enc2_update();
			uint16_t xx = current_place * infobar_label_width + infobar_label_width / 2;

			if (gui_enc2_menu.state)
			{
				local_snprintf_P(buf, buflen, "%s", gui_enc2_menu.param);
				remove_end_line_spaces(buf);
				strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
				gui_print_prop(xx - strl / 2, infobar_1st_str_y, buf, & INFOBAR_FONTP, COLORPIP_WHITE);
				local_snprintf_P(buf, buflen, "%s", gui_enc2_menu.val);
				remove_end_line_spaces(buf);
				strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
				gui_color_t color_lbl = gui_enc2_menu.state == 2 ? COLORPIP_YELLOW : COLORPIP_WHITE;
				gui_print_prop(xx - strl / 2, infobar_2nd_str_y, buf, & INFOBAR_FONTP, color_lbl);
			}
			else
			{
#if defined (RTC1_TYPE)
				// текущее время
				local_snprintf_P(buf, buflen, "%02d.%02d", day, month);
				strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
				gui_print_prop(xx - strl / 2, infobar_1st_str_y, buf, & INFOBAR_FONTP, COLORPIP_WHITE);
				local_snprintf_P(buf, buflen, "%02d%c%02d", hour, ((seconds & 1) ? ' ' : ':'), minute);
				strl = get_strwidth_prop(buf, & INFOBAR_FONTP);
				gui_print_prop(xx - strl / 2, infobar_2nd_str_y, buf, & INFOBAR_FONTP, COLORPIP_WHITE);
#endif 	/* defined (RTC1_TYPE) */
			}
		}
			break;

		case INFOBAR_EMPTY:
		default:
			break;
		}
	}

#if 0 //WITHTHERMOLEVEL	// температура выходных транзисторов (при передаче)
	if (hamradio_get_tx())
	{
		const ldiv_t t = ldiv(hamradio_get_PAtemp_value(), 10);
		local_snprintf_P(buf, buflen, "%d.%dC ", t.quot, t.rem);
		GUI_DEBUG_PRINT("%s\n", buf);		// пока вывод в консоль
	}
#endif /* WITHTHERMOLEVEL */

#if WITHGUIDEBUG
	if (debug_view)
	{
		uint16_t x = gui_obj_get_int_prop("tf_debug", GUI_OBJ_POS_X);
		uint16_t y = gui_obj_get_int_prop("tf_debug", GUI_OBJ_POS_Y);
		uint16_t w = gui_obj_get_int_prop("tf_debug", GUI_OBJ_WIDTH);
		uint16_t h = gui_obj_get_int_prop("tf_debug", GUI_OBJ_HEIGHT);
		gui_drawrect_transparent(x - 5, y - 5, x + w + 5, y + h + 5, DEFAULT_ALPHA);
	}
#endif /* WITHGUIDEBUG */
}

// *********************************************************************************************************************************************************************

static uint32_t mems[memory_cells_count];

static uint32_t load_mems(uint_fast8_t cell, uint_fast8_t set)
{
	return hamradio_load_memory_cells(cell, set);
}

static void clean_mems(uint_fast8_t cell)
{
	hamradio_clean_memory_cells(cell);
}

static void write_mems(uint_fast8_t cell)
{
	hamradio_save_memory_cells(cell);
}

void window_memory_process(void)
{
	if (is_win_init())
	{
		char name[NAME_ARRAY_SIZE], text[NAME_ARRAY_SIZE];

		for (int i = 0; i < memory_cells_count; i ++)
		{
			local_snprintf_P(name, NAME_ARRAY_SIZE, "btn_memory_%d", i);

			long freq = load_mems(i, 0);
			if (freq > 0)
				local_snprintf_P(text, NAME_ARRAY_SIZE, "%ldk", freq / 1000);
			else
				local_snprintf_P(text, NAME_ARRAY_SIZE, "---");

			gui_obj_create(name, LONG_BUTTON_STYLE, 0, 1, text);
			gui_obj_set_prop(name, GUI_OBJ_PAYLOAD, freq > 0);
		}

		char * btn0 = get_obj_name_by_idx(TYPE_BUTTON, 0);
		gui_arrange_objects_from(btn0, memory_cells_count, 5, 6);

		gui_obj_create("lbl_note1", COLORPIP_WHITE, 54);
		gui_obj_set_prop("lbl_note1", GUI_OBJ_TEXT, "Long press on empty cell - save,on saved cell - clean");
		gui_obj_set_prop("lbl_note1", GUI_OBJ_POS_Y, common_btn_height * 5);
		gui_obj_set_prop("lbl_note1", GUI_OBJ_FONT, & msgothic_11x13_mono);

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_MEMORY)
	{
	case WM_MESSAGE_ACTION:
		if (type == TYPE_BUTTON)
		{
			if (IS_BUTTON_PRESS)
			{
				uint8_t cell_id = gui_obj_get_int_prop(name, GUI_OBJ_INDEX);
				uint8_t p = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);

				if (p)
				{
					load_mems(cell_id, 1);
					close_window(DONT_OPEN_PARENT_WINDOW);
					footer_buttons_state(CANCELLED);
					return;
				}
			}
			else if (IS_BUTTON_LONG_PRESS)
			{
				uint8_t cell_id = gui_obj_get_int_prop(name, GUI_OBJ_INDEX);
				uint8_t p = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);

				if (p)
				{
					gui_obj_set_prop(name, GUI_OBJ_PAYLOAD, 0);
					gui_obj_set_prop(name, GUI_OBJ_TEXT, "---");
					clean_mems(cell_id);
				}
				else
				{
					uint32_t freq = hamradio_get_freq_rx();
					gui_obj_set_prop(name, GUI_OBJ_TEXT_FMT, "%dk", freq / 1000);
					gui_obj_set_prop(name, GUI_OBJ_PAYLOAD, 1);
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
	static band_array_t * bands = NULL;

	if (is_win_init())
	{
		char name[NAME_ARRAY_SIZE], text[NAME_ARRAY_SIZE];
		uint8_t bandnum = hamradio_get_bands(NULL, 1, 1), i = 0;
		bands = (band_array_t *) calloc(bandnum, sizeof (band_array_t));
		GUI_MEM_ASSERT(bands);
		hamradio_get_bands(bands, 0, 1);

		gui_obj_create("lbl_ham", COLORPIP_WHITE, 10);
		gui_obj_create("lbl_bcast", COLORPIP_WHITE, 16);

		gui_obj_set_prop("lbl_ham", GUI_OBJ_TEXT, "HAM bands");
		gui_obj_set_prop("lbl_bcast", GUI_OBJ_TEXT, "Broadcast bands");

		gui_obj_align_to("lbl_bcast", "lbl_ham", ALIGN_RIGHT_UP, common_btn_width * 2);

		for (; i < bandnum; i ++)
		{
			if (bands[i].type != BAND_TYPE_HAM)
			{
				// кнопка прямого ввода частоты
				gui_obj_create("btn_freq", COMMON_BUTTON_STYLE, 0, 0, "Freq|enter");
				i ++;

				break;
			}

			local_snprintf_P(name, NAME_ARRAY_SIZE, "btn_ham_%d", i);
			char * div = strchr(bands[i].name, ' ');
			if(div)
				memcpy(div, "|", 1);

			strncpy(text, bands[i].name, NAME_ARRAY_SIZE - 1);

			gui_obj_create(name, COMMON_BUTTON_STYLE, 0, 0, text);

			if (hamradio_check_current_freq_by_band(bands[i].index))
				gui_obj_set_prop(name, GUI_OBJ_LOCK, BUTTON_LOCKED);

			gui_obj_set_prop(name, GUI_OBJ_PAYLOAD, bands[i].init_freq);
		}

		char * btn0 = get_obj_name_by_idx(TYPE_BUTTON, 0);
		gui_obj_align_to(btn0, "lbl_ham", ALIGN_DOWN_LEFT, 6);
		gui_arrange_objects_from(btn0, i, 3, 6);

		uint8_t j = i;

		for (; j < bandnum + 1; j ++)
		{
			local_snprintf_P(name, NAME_ARRAY_SIZE, "btn_bcast_%d", j);
			char * div = strchr(bands[j - 1].name, ' ');
			if(div)
				memcpy(div, "|", 1);

			strncpy(text, bands[j - 1].name, NAME_ARRAY_SIZE - 1);

			gui_obj_create(name, COMMON_BUTTON_STYLE, 0, 0, text);

			if (hamradio_check_current_freq_by_band(bands[j - 1].index))
				gui_obj_set_prop(name, GUI_OBJ_LOCK, BUTTON_LOCKED);

			gui_obj_set_prop(name, GUI_OBJ_PAYLOAD, bands[j - 1].init_freq);
		}

		btn0 = get_obj_name_by_idx(TYPE_BUTTON, i);
		gui_obj_align_to(btn0, "lbl_bcast", ALIGN_DOWN_LEFT, 6);
		gui_arrange_objects_from(btn0, j - i, 3, 6);

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_BANDS)
	{
	case WM_MESSAGE_ACTION:

		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_freq"))
			{
				open_window(get_win(WINDOW_FREQ));
				hamradio_set_lock(1);
				hamradio_enable_keyboard_redirect();
			}
			else
			{
				int f = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
				hamradio_goto_band_by_freq(f);
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
	if (is_win_init())
	{
		uint8_t idx;

		idx = gui_obj_create("btn_SysMenu", LONG_BUTTON_STYLE, 0, 0, "System|settings");
		idx = gui_obj_create("btn_AUDsett", LONG_BUTTON_STYLE, 0, 0, "Audio|settings");
		idx = gui_obj_create("btn_TXsett", LONG_BUTTON_STYLE, 0, 0, "Transmit|settings");
		idx = gui_obj_create("btn_Display", LONG_BUTTON_STYLE, 0, 0, "Display|settings");
		idx = gui_obj_create("btn_gui", LONG_BUTTON_STYLE, 0, 0, "GUI|settings");
		idx = gui_obj_create("btn_Utils", LONG_BUTTON_STYLE, 0, 0, "Utils");
#if defined (RTC1_TYPE)
		idx = gui_obj_create("btn_Time", LONG_BUTTON_STYLE, 0, 0, "Set time|& date");
#endif /* defined (RTC1_TYPE) */

		gui_arrange_objects_from(get_obj_name_by_idx(TYPE_BUTTON, 0), idx + 1, 4, 6);

		hamradio_disable_keyboard_redirect();
		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_OPTIONS)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_gui"))
				open_window(get_win(WINDOW_GUI_SETTINGS));
#if defined (RTC1_TYPE)
			else if (gui_check_obj(name, "btn_Time"))
				open_window(get_win(WINDOW_TIME));
#endif /* defined (RTC1_TYPE) */
			else if (gui_check_obj(name, "btn_Utils"))
				open_window(get_win(WINDOW_UTILS));
			else if (gui_check_obj(name, "btn_TXsett"))
				open_window(get_win(WINDOW_TX_SETTINGS));
			else if (gui_check_obj(name, "btn_AUDsett"))
				open_window(get_win(WINDOW_AUDIOSETTINGS));
			else if (gui_check_obj(name, "btn_SysMenu"))
			{
				open_window(get_win(WINDOW_MENU));
				hamradio_enable_encoder2_redirect();
			}
			else if (gui_check_obj(name, "btn_Display"))
				open_window(get_win(WINDOW_DISPLAY));
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
	uint8_t update = 0;

	if (is_win_init())
	{
		uint8_t interval = 30;
		update = 1;

		gui_obj_create("lbl_topSP", COLORPIP_WHITE, 18);
		gui_obj_create("lbl_bottomSP", COLORPIP_WHITE, 18);
		gui_obj_create("lbl_partSP", COLORPIP_WHITE, 18);
		gui_obj_create("sl_topSP", ORIENTATION_HORIZONTAL, 300, 3);			// index = TYPE_DISPLAY_SP_TOP
		gui_obj_create("sl_bottomSP", ORIENTATION_HORIZONTAL, 300, 3);		// index = TYPE_DISPLAY_SP_BOTTOM
		gui_obj_create("sl_partSP", ORIENTATION_HORIZONTAL, 300, 3); 		// index = TYPE_DISPLAY_SP_PART
		gui_obj_create("btn_zoom", LONG_BUTTON_STYLE, 0, 0, "");
		gui_obj_create("btn_view", LONG_BUTTON_STYLE, 0, 0, "");

		gui_arrange_objects_from("lbl_topSP", 3, 1, interval);
		gui_obj_align_to("sl_topSP", "lbl_topSP", ALIGN_RIGHT_UP_MID, interval);
		gui_obj_align_to("sl_bottomSP", "lbl_bottomSP", ALIGN_RIGHT_UP_MID, interval);
		gui_obj_align_to("sl_partSP", "lbl_partSP", ALIGN_RIGHT_UP_MID, interval);
		gui_obj_align_to("btn_zoom", "lbl_partSP", ALIGN_DOWN_LEFT, interval);
		gui_arrange_objects_from("btn_zoom", 2, 2, interval);

		gui_obj_set_prop("sl_partSP", GUI_OBJ_PAYLOAD, normalize(hamradio_get_spectrumpart(), WITHSPPARTMIN, WITHSPPARTMAX, 100));
		gui_obj_set_prop("sl_topSP", GUI_OBJ_PAYLOAD, snormalize(hamradio_get_topdb(), GUI_TOPDBMIN, GUI_TOPDBMAX, 100));
		gui_obj_set_prop("sl_bottomSP", GUI_OBJ_PAYLOAD, snormalize(hamradio_get_bottomdb(), GUI_BOTTOMDBMIN, GUI_BOTTOMDBMAX, 100));

		hamradio_enable_encoder2_redirect();
		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_DISPLAY)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_view"))
				hamradio_change_view_style(1);
			else if (gui_check_obj(name, "btn_zoom"))
			{
				uint8_t z = (hamradio_get_gzoomxpow2() + 1) % (BOARD_FFTZOOM_POW2MAX + 1);
				hamradio_set_gzoomxpow2(z);
			}

			update = 1;
		}
		else if (IS_SLIDER_MOVE)
		{
			uint8_t v = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			uint8_t i = gui_obj_get_int_prop(name, GUI_OBJ_INDEX);

			if (i == TYPE_DISPLAY_SP_TOP)
				hamradio_set_topdb(GUI_TOPDBMIN + snormalize(v, 0, 100, GUI_TOPDBMAX - GUI_TOPDBMIN));
			else if (i == TYPE_DISPLAY_SP_BOTTOM)
				hamradio_set_bottomdb(GUI_BOTTOMDBMIN + snormalize(v, 0, 100, GUI_BOTTOMDBMAX - GUI_BOTTOMDBMIN));
			else if (i == TYPE_DISPLAY_SP_PART)
				hamradio_set_spectrumpart(WITHSPPARTMIN + normalize(v, 0, 100, WITHSPPARTMAX - WITHSPPARTMIN));
//todo: добавить обработку ситуации, когда bottom превышает top

			update = 1;
		}

		break;

	default:
		break;
	}

	if (update)
	{
		update = 0;

		gui_obj_set_prop("btn_view", GUI_OBJ_TEXT_FMT, "View|%s", hamradio_change_view_style(0));
		gui_obj_set_prop("btn_zoom", GUI_OBJ_TEXT_FMT, "Zoom|x%d", 1 << hamradio_get_gzoomxpow2());
		gui_obj_set_prop("lbl_topSP", GUI_OBJ_TEXT_FMT,    "SP Top    : %3d db", hamradio_get_topdb());
		gui_obj_set_prop("lbl_bottomSP", GUI_OBJ_TEXT_FMT, "SP Bottom : %3d db", hamradio_get_bottomdb());
		gui_obj_set_prop("lbl_partSP", GUI_OBJ_TEXT_FMT,   "SP part   : %3d %%", hamradio_get_spectrumpart());
	}
#endif /* WITHSPECTRUMWF && WITHMENU */
}

// *********************************************************************************************************************************************************************

void window_utilites_process(void)
{
	if (is_win_init())
	{
		uint8_t idx;

#if WITHGUIDEBUG
		idx = gui_obj_create("btn_debug", LONG_BUTTON_STYLE, 0, 0, "Debug|view");
#endif /* WITHGUIDEBUG */
#if WITHSWRSCAN
		idx = gui_obj_create("btn_SWRscan", LONG_BUTTON_STYLE, 0, 0, "SWR|scanner");
#endif /* WITHSWRSCAN */
		idx = gui_obj_create("btn_3d", LONG_BUTTON_STYLE, 0, 0, "Donut|3d");
#if WITHLFM
		idx = gui_obj_create("btn_lfm", LONG_BUTTON_STYLE, 0, 0, "LFM|receive");
#endif /* WITHLFM  */
#if WITHIQSHIFT
		idx = gui_obj_create("btn_shift", LONG_BUTTON_STYLE, 0, 0, "IQ shift");
#endif /* WITHIQSHIFT */
#if 0
		idx = gui_obj_create("btn_kbdtest", LONG_BUTTON_STYLE, 0, 0, "Keyboard|test");
#endif
#if WITHCPUFANPWM
		idx = gui_obj_create("btn_fanpwm", LONG_BUTTON_STYLE, 0, 0, "Fan PWM|control");
#endif /* WITHCPUFANPWM */

		gui_arrange_objects_from(get_obj_name_by_idx(TYPE_BUTTON, 0), idx + 1, 4, 6);

#if WITHGUIDEBUG
		gui_obj_set_prop("btn_debug", GUI_OBJ_LOCK, debug_view);
#endif /* WITHGUIDEBUG */

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_UTILS)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_3d"))
				open_window(get_win(WINDOW_3D));
#if WITHSWRSCAN
			else if (gui_check_obj(name, "btn_SWRscan"))
				open_window(get_win(WINDOW_SWR_SCANNER));
#endif /* WITHSWRSCAN */
#if WITHLFM
			else if (gui_check_obj(name, "btn_lfm"))
				open_window(get_win(WINDOW_LFM));
#endif /* WITHLFM  */
#if WITHGUIDEBUG
			else if (gui_check_obj(name, "btn_debug"))
			{
				gui_open_debug_window();
				close_all_windows();
			}
#endif /* WITHGUIDEBUG */
#if WITHIQSHIFT
			else if (gui_check_obj(name, "btn_shift"))
				open_window(get_win(WINDOW_SHIFT));
#endif /* WITHIQSHIFT */
			else if (gui_check_obj(name, "btn_kbdtest"))
				open_window(get_win(WINDOW_KBD_TEST));
#if WITHCPUFANPWM
			else if (gui_check_obj(name, "btn_fanpwm"))
				open_window(get_win(WINDOW_FANPWM));
#endif /* WITHCPUFANPWM */
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
	uint16_t gr_w = 500, gr_h = 200, mid_w = gr_w / 2;				// размеры области графика
	uint8_t interval = 20;
	const uint16_t x0 = interval, y0 = gr_h;						// нулевые координаты графика
	const uint16_t x1 = x0 + gr_w - interval * 2, y1 = gr_h - y0;	// размеры осей графика
	static uint16_t freq_step = 0;
	static uint16_t i, current_freq_x;
	static uint_fast32_t lim_bottom, lim_top, swr_freq, backup_freq;
	static uint8_t backup_power;
	static uint8_t swr_scan_done = 0, is_swr_scanning = 0;
	static uint8_t swr_scan_enable = 0;			// флаг разрешения сканирования КСВ
	static uint8_t swr_scan_stop = 0;			// флаг нажатия кнопки Stop во время сканирования
	static uint8_t * y_vals;					// массив КСВ в виде отсчетов по оси Y графика
	const uint8_t averageFactor = 3;

	if (is_win_init())
	{
		i = 0;
		y_vals = calloc(x1 - x0, sizeof(uint8_t));
		GUI_MEM_ASSERT(y_vals);
		swr_scan_done = 0;
		is_swr_scanning = 0;
		swr_scan_stop = 0;
		backup_freq = hamradio_get_freq_rx();

		gui_obj_create("btn_swr_start", COMMON_BUTTON_STYLE, 0, 0, "Start");
		gui_obj_create("btn_swr_OK",    COMMON_BUTTON_STYLE, 0, 0, "OK");

		gui_obj_create("lbl_swr_bottom", COLORPIP_WHITE, 7);
		gui_obj_create("lbl_swr_top",    COLORPIP_WHITE, 7);
		gui_obj_create("lbl_swr_error",  COLORPIP_WHITE, 16);

		gui_obj_set_prop("lbl_swr_top", GUI_OBJ_POS, x1 - 30, y0 + interval);
		gui_obj_set_prop("lbl_swr_bottom", GUI_OBJ_POS, x0, y0 + interval);

		gui_obj_align_to("btn_swr_start", "lbl_swr_bottom", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("btn_swr_OK", "lbl_swr_top", ALIGN_DOWN_RIGHT, interval);

		if (hamradio_verify_freq_bands(backup_freq, & lim_bottom, & lim_top))
		{
			gui_obj_set_prop("lbl_swr_bottom", GUI_OBJ_TEXT_FMT, "%dk", lim_bottom / 1000);
			gui_obj_set_prop("lbl_swr_top", GUI_OBJ_TEXT_FMT, "%dk", lim_top / 1000);
			gui_obj_set_prop("lbl_swr_error", GUI_OBJ_VISIBLE, 0);

			swr_freq = lim_bottom;
			freq_step = (lim_top - lim_bottom) / (x1 - x0);
			current_freq_x = normalize(backup_freq / 1000, lim_bottom / 1000, lim_top / 1000, x1 - x0);
//			backup_power = hamradio_get_tx_power();
		}
		else
		{	// если текущая частота не входит ни в один из любительских диапазонов, вывод сообщения об ошибке
			gui_obj_set_prop("btn_swr_start", GUI_OBJ_STATE, DISABLED);
			gui_obj_set_prop("lbl_swr_top", GUI_OBJ_VISIBLE, 0);
			gui_obj_set_prop("lbl_swr_bottom", GUI_OBJ_VISIBLE, 0);
			gui_obj_set_prop("lbl_swr_error", GUI_OBJ_VISIBLE, 1);
			gui_obj_set_prop("lbl_swr_error", GUI_OBJ_TEXT_FMT, "%dk not into HAM bands", backup_freq / 1000);
			gui_obj_set_prop("lbl_swr_error", GUI_OBJ_POS, mid_w - get_label_width2("lbl_swr_error") / 2, gr_h / 2);
		}

		calculate_window_position(WINDOW_POSITION_AUTO);
		return;
	}

	GET_FROM_WM_QUEUE(WINDOW_SWR_SCANNER)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_swr_start"))
			{
				if (is_swr_scanning)
					swr_scan_stop = 1;
				else
					swr_scan_enable = 1;
			}
			else if (gui_check_obj(name, "btn_swr_OK"))
			{
				close_all_windows();
				free(y_vals);
				return;
			}
		}
		break;

	case WM_MESSAGE_CLOSE:
		return;

	default:
		break;
	}

	if (swr_scan_enable)						// нажата кнопка Start
	{
		swr_scan_enable = 0;
		gui_obj_set_prop("btn_swr_start", GUI_OBJ_TEXT, "Stop");
		gui_obj_set_prop("btn_swr_OK", GUI_OBJ_STATE, DISABLED);
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
			gui_obj_set_prop("btn_swr_start", GUI_OBJ_TEXT, "Start");
			gui_obj_set_prop("btn_swr_OK", GUI_OBJ_STATE, CANCELLED);
			hamradio_set_tune(0);
			hamradio_set_freq(backup_freq);
//			hamradio_set_tx_power(backup_power);
		}

		const uint_fast16_t swr_fullscale = (SWRMIN * 40 / 10) - SWRMIN;	// количество рисок на шкале индикатора
		y_vals[i] = normalize(get_swr(swr_fullscale), 0, swr_fullscale, y0 - y1);
		if (i)
			y_vals[i] = (y_vals[i - 1] * (averageFactor - 1) + y_vals[i]) / averageFactor;
		i++;
	}

		// отрисовка фона графика и разметки
		gui_drawline(x0, y0, x0, y1, COLORPIP_WHITE);
		gui_drawline(x0, y0, x1, y0, COLORPIP_WHITE);

		char buf[5];
		uint_fast8_t l = 1, row_step = roundf((y0 - y1) / 3);
		uint8_t charw = msgothic_11x13_mono.width, charh = msgothic_11x13_mono.height;

		local_snprintf_P(buf, ARRAY_SIZE(buf), "%d", l ++);
		gui_print_mono(x0 - charw, y0 - charh / 2, buf, & msgothic_11x13_mono, COLORPIP_WHITE);

		for(int_fast16_t yy = y0 - row_step; yy > y1; yy -= row_step)
		{
			if (yy < 0)
				break;

			gui_drawline(x0 + 1, yy, x1, yy, COLORPIP_DARKGREEN);
			local_snprintf_P(buf, ARRAY_SIZE(buf), "%d", l ++);
			gui_print_mono(x0 - charw, yy - charh / 2, buf, & msgothic_11x13_mono, COLORPIP_WHITE);
		}

		if (gui_obj_get_int_prop("lbl_swr_error", GUI_OBJ_VISIBLE))				// фон сообщения об ошибке
		{
			int lx1 = 0, ly1 = gr_h / 2 - 3, lx2 = gr_w - 1;
			int ly2 = ly1 + gui_obj_get_int_prop("lbl_swr_error", GUI_OBJ_HEIGHT) + 4;
			gui_drawrect(lx1, ly1, lx2, ly2, COLORPIP_RED, 1);
		}
		else																	// маркер текущей частоты
			gui_drawline(x0 + current_freq_x, y0, x0 + current_freq_x, y1, COLORPIP_RED);

		if (is_swr_scanning || swr_scan_done)									// вывод графика во время сканирования и по завершении
		{
			for(uint_fast16_t j = 2; j <= i; j ++)
				gui_drawline(x0 + j - 2, y0 - y_vals[j - 2], x0 + j - 1, y0 - y_vals[j - 1], COLORPIP_YELLOW);
		}
#endif /* WITHSWRSCAN */
}

// *********************************************************************************************************************************************************************

void window_tx_process(void)
{
	window_t * const win = get_win(WINDOW_TX_SETTINGS);

	if (is_win_init())
	{
		gui_obj_create("btn_tx_power", LONG_BUTTON_STYLE, 0, 0, "TX power");

#if ! WITHPOWERTRIM
		gui_obj_set_prop("btn_tx_power", GUI_OBJ_STATE, DISABLED);
#endif /* ! WITHPOWERTRIM */

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_TX_SETTINGS)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
#if WITHPOWERTRIM && WITHTX
			if (gui_check_obj(name, "btn_tx_power"))
				open_window(get_win(WINDOW_TX_POWER));
#endif /* WITHPOWERTRIM  && WITHTX*/
		}
		break;

	default:
		break;
	}
}

// *********************************************************************************************************************************************************************

void window_tx_power_process(void)
{
#if WITHPOWERTRIM && WITHTX
	static uint_fast8_t power_min, power_max, update = 0;;

	if (is_win_init())
	{
		uint8_t interval = 30;
		update = 1;

		hamradio_get_tx_power_limits(& power_min, & power_max);

		gui_obj_create("lbl_power", COLORPIP_WHITE, 16);
		gui_obj_create("lbl_tune", COLORPIP_WHITE, 16);
		gui_obj_create("sl_power", ORIENTATION_HORIZONTAL, 300, 3);
		gui_obj_create("sl_tune", ORIENTATION_HORIZONTAL, 300, 3);
		gui_obj_create("btn_ok", SMALL_BUTTON_STYLE, 0, 0, "OK");

		gui_obj_set_prop("lbl_power", GUI_OBJ_POS_Y, 10);
		gui_obj_set_prop("sl_power", GUI_OBJ_PAYLOAD, normalize(hamradio_get_tx_power(), power_min, power_max, 100));
		gui_obj_set_prop("sl_tune", GUI_OBJ_PAYLOAD, normalize(hamradio_get_tx_tune_power(), power_min, power_max, 100));

		gui_obj_align_to("lbl_tune", "lbl_power", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("sl_power", "lbl_power", ALIGN_RIGHT_UP_MID, interval);
		gui_obj_align_to("sl_tune", "lbl_tune", ALIGN_RIGHT_UP_MID, interval);
		gui_obj_align_to("btn_ok", "sl_tune", ALIGN_DOWN_RIGHT, interval);

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_TX_POWER)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			close_all_windows();
			return;
		}
		else if (IS_SLIDER_MOVE)
		{
			uint16_t val = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			uint16_t p = power_min + normalize(val, 0, 100, power_max - power_min);

			if (gui_check_obj(name, "sl_power"))
				hamradio_set_tx_power(p);
			else if (gui_check_obj(name, "sl_tune"))
				hamradio_set_tx_tune_power(p);

			save_settings();
			update = 1;
		}
		break;

	default:
		break;
	}

	if (update)
	{
		update = 0;

		gui_obj_set_prop("lbl_power", GUI_OBJ_TEXT_FMT, "TX power  : %3d", hamradio_get_tx_power());
		gui_obj_set_prop("lbl_tune", GUI_OBJ_TEXT_FMT, "Tune power: %3d", hamradio_get_tx_tune_power());
	}

#endif /* WITHPOWERTRIM && WITHTX */
}

// *********************************************************************************************************************************************************************

void window_audiosettings_process(void)
{
	uint8_t update = 0;

	if (is_win_init())
	{
		update = 1;

		gui_obj_create("btn_mic_settings", LONG_BUTTON_STYLE, 0, 0, "MIC|settings");
		gui_obj_create("btn_mic_profiles", LONG_BUTTON_STYLE, 0, 0, "MIC|profiles");
		gui_obj_create("btn_reverb_settings", LONG_BUTTON_STYLE, 0, 0, "Reverb|settings");
		gui_obj_create("btn_monitor", LONG_BUTTON_STYLE, 0, 0, "Monitor|disabled");
		gui_obj_create("btn_af_eq", LONG_BUTTON_STYLE, 0, 0, "AF|equalizer");
		gui_obj_create("btn_audio_switch", LONG_BUTTON_STYLE, 0, 0, "Audio|switch");

		gui_arrange_objects_from("btn_mic_settings", 6, 4, 10);

#if ! defined (WITHAFEQUALIZER) || WITHAFEQUALIZER == 0
		gui_obj_set_prop("btn_af_eq", GUI_OBJ_STATE, DISABLED);
#endif /* ! WITHAFEQUALIZER */
#if ! defined(CODEC1_TYPE) && (CODEC1_TYPE != CODEC_TYPE_ALSA) || BLUETOOTH_ALSA == 0
		gui_obj_set_prop("btn_audio_switch", GUI_OBJ_STATE, DISABLED);
#endif /* ! defined(CODEC1_TYPE) && (CODEC1_TYPE != CODEC_TYPE_ALSA) || BLUETOOTH_ALSA == 0 */

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_AUDIOSETTINGS)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_monitor"))
			{
				hamradio_set_gmoniflag(! hamradio_get_gmoniflag());
				update = 1;
			}
#if WITHREVERB
			else if (gui_check_obj(name, "btn_reverb_settings"))
				open_window(get_win(WINDOW_AP_REVERB_SETT));
#endif /* WITHREVERB */
			else if (gui_check_obj(name, "btn_mic_settings"))
			{
				open_window(get_win(WINDOW_AP_MIC_SETT));
				return;
			}
			else if (gui_check_obj(name, "btn_mic_profiles"))
			{
				open_window(get_win(WINDOW_AP_MIC_PROF));
				return;
			}
#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_ALSA) && BLUETOOTH_ALSA
			else if (gui_check_obj(name, "btn_audio_switch"))
			{
				alsa_switch_out();
				update = 1;
			}
#endif /* defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_ALSA) && BLUETOOTH_ALSA */
#if WITHAFEQUALIZER
			else if (gui_check_obj(name, "btn_af_eq"))
				open_window(get_win(WINDOW_AF_EQ));
#endif /* WITHAFEQUALIZER */
		}
		break;

	default:
		break;
	}

	if (update)
	{
		update = 0;

		uint8_t v = hamradio_get_gmoniflag();
		gui_obj_set_prop("btn_monitor", GUI_OBJ_LOCK, v);
		gui_obj_set_prop("btn_monitor", GUI_OBJ_TEXT_FMT, "Monitor|%s", v ? "enabled" : "disabled");

#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_ALSA) && BLUETOOTH_ALSA
		gui_obj_set_prop("btn_audio_switch", GUI_OBJ_TEXT_FMT, "Monitor|%s", "Audio|%s", get_alsa_out());
#endif /* defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_ALSA) && BLUETOOTH_ALSA */
	}
}

// *********************************************************************************************************************************************************************

void window_ap_reverb_process(void)
{
#if WITHREVERB
	static uint_fast8_t delay_min, delay_max, loss_min, loss_max;
	uint8_t update = 0;

	if (is_win_init())
	{
		uint8_t interval = 30;
		update = 1;

		hamradio_get_reverb_delay_limits(& delay_min, & delay_max);
		hamradio_get_reverb_loss_limits(& loss_min, & loss_max);

		gui_obj_create("sl_delay", ORIENTATION_HORIZONTAL, 300, 3);
		gui_obj_create("sl_loss", ORIENTATION_HORIZONTAL, 300, 3);
		gui_obj_create("lbl_delay", COLORPIP_WHITE, 14);
		gui_obj_create("lbl_loss", COLORPIP_WHITE, 14);
		gui_obj_create("btn_en", SMALL_BUTTON_STYLE, 0, 0, "EN");
		gui_obj_create("btn_ok", SMALL_BUTTON_STYLE, 0, 0, "OK");

		gui_obj_set_prop("lbl_delay", GUI_OBJ_POS_Y, 10);
		gui_obj_set_prop("sl_delay", GUI_OBJ_PAYLOAD, normalize(hamradio_get_reverb_delay(), delay_min, delay_max, 100));
		gui_obj_set_prop("sl_loss", GUI_OBJ_PAYLOAD, normalize(hamradio_get_reverb_loss(), loss_min, loss_max, 100));

		gui_obj_align_to("lbl_loss", "lbl_delay", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("sl_delay", "lbl_delay", ALIGN_RIGHT_UP_MID, interval);
		gui_obj_align_to("sl_loss", "lbl_loss", ALIGN_RIGHT_UP_MID, interval);
		gui_obj_align_to("btn_en", "sl_delay", ALIGN_RIGHT_UP, interval);
		gui_obj_align_to("btn_ok", "btn_en", ALIGN_DOWN_LEFT, interval);

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_AP_REVERB_SETT)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_ok"))
			{
				close_window(DONT_OPEN_PARENT_WINDOW);
				footer_buttons_state(CANCELLED);
				return;
			}
			else if (gui_check_obj(name, "btn_en"))
			{
				hamradio_set_greverb(! hamradio_get_greverb());
				update = 1;
			}
		}
		else if (IS_SLIDER_MOVE)
		{
			uint16_t val = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);

			if (gui_check_obj(name, "sl_delay"))
			{
				uint16_t delay = delay_min + normalize(val, 0, 100, delay_max - delay_min);
				hamradio_set_reverb_delay(delay);
			}
			else if (gui_check_obj(name, "sl_loss"))
			{
				uint16_t loss = loss_min + normalize(val, 0, 100, loss_max - loss_min);
				hamradio_set_reverb_loss(loss);
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

		gui_obj_set_prop("lbl_delay", GUI_OBJ_TEXT_FMT, "Delay: %3d ms", hamradio_get_reverb_delay());
		gui_obj_set_prop("lbl_loss", GUI_OBJ_TEXT_FMT, "Loss :  %2d dB", hamradio_get_reverb_loss());
		gui_obj_set_prop("btn_en", GUI_OBJ_LOCK, hamradio_get_greverb());
	}
#endif /* WITHREVERB */
}

// *********************************************************************************************************************************************************************

void window_af_eq_process(void)
{
#if WITHAFEQUALIZER
	static uint32_t eq_w, eq_limit, eq_base = 0;
	static int16_t mid_y = 0;

	if (is_win_init())
	{

		eq_base = hamradio_get_af_equalizer_base();
		eq_limit = abs(eq_base) * 2;
		uint8_t v = hamradio_get_geqrx();

		gui_obj_create("sl_eq400", ORIENTATION_VERTICAL, 200, 2);
		gui_obj_create("sl_eq1500", ORIENTATION_VERTICAL, 200, 2);
		gui_obj_create("sl_eq2700", ORIENTATION_VERTICAL, 200, 2);
		gui_obj_create("lbl_eq400", COLORPIP_WHITE, 6);
		gui_obj_create("lbl_eq1500", COLORPIP_WHITE, 7);
		gui_obj_create("lbl_eq2700", COLORPIP_WHITE, 7);
		gui_obj_set_prop("lbl_eq400", GUI_OBJ_FONT, & msgothic_11x13_mono);
		gui_obj_set_prop("lbl_eq1500", GUI_OBJ_FONT, & msgothic_11x13_mono);
		gui_obj_set_prop("lbl_eq2700", GUI_OBJ_FONT, & msgothic_11x13_mono);
		gui_obj_create("btn_ok", SMALL_BUTTON_STYLE, 0, 0, "OK");
		gui_obj_create("btn_en", SMALL_BUTTON_STYLE, 0, 0, "OK");

		gui_obj_set_prop("sl_eq400", GUI_OBJ_POS, 50, 10);
		gui_arrange_objects_from("sl_eq400", 3, 3, 70);
		gui_obj_align_to("lbl_eq400", "sl_eq400", ALIGN_DOWN_MID, 20);
		gui_obj_align_to("lbl_eq1500", "sl_eq1500", ALIGN_DOWN_MID, 20);
		gui_obj_align_to("lbl_eq2700", "sl_eq2700", ALIGN_DOWN_MID, 20);
		gui_obj_align_to("btn_en", "sl_eq2700", ALIGN_RIGHT_UP, 20);
		gui_obj_align_to("btn_ok", "btn_en", ALIGN_DOWN_LEFT, 20);

		gui_obj_set_prop("sl_eq400", GUI_OBJ_PAYLOAD, normalize(hamradio_get_af_equalizer_gain_rx(0), eq_limit, 0, 100));
		gui_obj_set_prop("sl_eq1500", GUI_OBJ_PAYLOAD, normalize(hamradio_get_af_equalizer_gain_rx(1), eq_limit, 0, 100));
		gui_obj_set_prop("sl_eq2700", GUI_OBJ_PAYLOAD, normalize(hamradio_get_af_equalizer_gain_rx(2), eq_limit, 0, 100));
		gui_obj_set_prop("lbl_eq400", GUI_OBJ_TEXT, "400 Hz");
		gui_obj_set_prop("lbl_eq1500", GUI_OBJ_TEXT, "1500 Hz");
		gui_obj_set_prop("lbl_eq2700", GUI_OBJ_TEXT, "2700 Hz");
		gui_obj_set_prop("btn_en", GUI_OBJ_LOCK, v);
		gui_obj_set_prop("btn_en", GUI_OBJ_TEXT, v ? "En" : "Dis");

		mid_y = gui_obj_get_int_prop("sl_eq400", GUI_OBJ_POS_Y) + gui_obj_get_int_prop("sl_eq400", GUI_OBJ_SIZE) / 2;

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_AF_EQ)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_ok"))
			{
				close_all_windows();
				return;
			}
			else if (gui_check_obj(name, "btn_en"))
			{
				hamradio_set_geqrx(! hamradio_get_geqrx());
				uint8_t v = hamradio_get_geqrx();
				gui_obj_set_prop("btn_en", GUI_OBJ_LOCK, v);
				gui_obj_set_prop("btn_en", GUI_OBJ_TEXT, v ? "En" : "Dis");
			}
		}

		if (IS_SLIDER_MOVE)
		{
			uint8_t id = gui_obj_get_int_prop(name, GUI_OBJ_INDEX);
			uint16_t val = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			hamradio_set_af_equalizer_gain_rx(id, normalize(val, 100, 0, eq_limit));
		}
		break;

	default:
		break;
	}

	uint16_t ww = gui_get_window_draw_width() - 55;

	// разметка шкал
	for (uint8_t i = 0; i <= abs(eq_base); i += 3)
	{
		char buf[10];
		uint_fast16_t yy = normalize(i, 0, abs(eq_base), 100);
		gui_drawline(30, mid_y + yy, ww, mid_y + yy, GUI_SLIDERLAYOUTCOLOR);
		local_snprintf_P(buf, ARRAY_SIZE(buf), i == 0 ? "%d" : "-%d", i);
		uint16_t w = get_strwidth_prop(buf, & msgothic_10x13_prop);
		gui_print_prop(30 - w - 5, mid_y + yy - 8, buf, & msgothic_10x13_prop, COLORPIP_WHITE);

		if (i == 0)
			continue;

		gui_drawline(30, mid_y - yy, ww, mid_y - yy, GUI_SLIDERLAYOUTCOLOR);
		local_snprintf_P(buf, ARRAY_SIZE(buf), "%d", i);
		w = get_strwidth_prop(buf, & msgothic_10x13_prop);
		gui_print_prop(30 - w - 5, mid_y - yy - 8, buf, & msgothic_10x13_prop, COLORPIP_WHITE);
	}
#endif /* WITHAFEQUALIZER */
}

// *********************************************************************************************************************************************************************

void window_ap_mic_process(void)
{
	static uint_fast8_t level_min, level_max, clip_min, clip_max, agc_min, agc_max;

	if (is_win_init())
	{
		hamradio_get_mic_level_limits(& level_min, & level_max);
		hamradio_get_mic_clip_limits(& clip_min, & clip_max);
		hamradio_get_mic_agc_limits(& agc_min, & agc_max);

		gui_obj_create("lbl_micLevel", COLORPIP_WHITE, 10);
		gui_obj_create("lbl_micClip", COLORPIP_WHITE, 10);
		gui_obj_create("lbl_micAGC", COLORPIP_WHITE, 10);
		gui_obj_create("sl_micLevel", ORIENTATION_HORIZONTAL, 300, 3);
		gui_obj_create("sl_micClip", ORIENTATION_HORIZONTAL, 300, 3);
		gui_obj_create("sl_micAGC", ORIENTATION_HORIZONTAL, 300, 3);
		gui_obj_create("btn_mic_agc", COMMON_BUTTON_STYLE, 0, 0, "AGC|OFF");
		gui_obj_create("btn_mic_boost", COMMON_BUTTON_STYLE, 0, 0, "Boost|OFF");
		gui_obj_create("btn_mic_OK", COMMON_BUTTON_STYLE, 0, 0, "OK");

		gui_obj_set_prop("lbl_micLevel", GUI_OBJ_POS_Y, 10);
		gui_arrange_objects_from("lbl_micLevel", 3, 1, 50);
		gui_obj_align_to("sl_micLevel", "lbl_micLevel", ALIGN_RIGHT_UP_MID, 30);
		gui_obj_align_to("sl_micClip", "lbl_micClip", ALIGN_RIGHT_UP_MID, 30);
		gui_obj_align_to("sl_micAGC", "lbl_micAGC", ALIGN_RIGHT_UP_MID, 30);
		gui_obj_align_to("btn_mic_agc", "lbl_micAGC", ALIGN_DOWN_LEFT, 30);
		gui_arrange_objects_from("btn_mic_agc", 3, 3, 50);

		gui_obj_set_prop("lbl_micLevel", GUI_OBJ_TEXT_FMT, "Level: %3d", hamradio_get_mik1level());
		gui_obj_set_prop("lbl_micClip", GUI_OBJ_TEXT_FMT, "Clip : %3d", hamradio_get_gmikehclip());
		gui_obj_set_prop("lbl_micAGC", GUI_OBJ_TEXT_FMT, "AGC  : %3d", hamradio_get_gmikeagcgain());
		gui_obj_set_prop("sl_micLevel", GUI_OBJ_PAYLOAD, normalize(hamradio_get_mik1level(), level_min, level_max, 100));
		gui_obj_set_prop("sl_micClip", GUI_OBJ_PAYLOAD, normalize(hamradio_get_gmikehclip(), clip_min, clip_max, 100));
		gui_obj_set_prop("sl_micAGC", GUI_OBJ_PAYLOAD, normalize(hamradio_get_gmikeagcgain(), agc_min, agc_max, 100));
		gui_obj_set_prop("sl_micAGC", GUI_OBJ_LOCK, hamradio_get_gmikeagc());


#if WITHAFCODEC1HAVEPROC
		gui_obj_set_prop("btn_mic_boost", GUI_OBJ_LOCK, hamradio_get_gmikeboost20db() ? BUTTON_LOCKED : BUTTON_NON_LOCKED);
#else
		gui_obj_set_prop("btn_mic_boost", GUI_OBJ_STATE, DISABLED);
#endif /* WITHAFCODEC1HAVEPROC */

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_AP_MIC_SETT)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_mic_boost"))
			{
#if WITHAFCODEC1HAVEPROC
				uint8_t v = hamradio_get_gmikeboost20db();
				gui_obj_set_prop("btn_mic_boost", GUI_OBJ_LOCK, v ? BUTTON_NON_LOCKED : BUTTON_LOCKED);
				gui_obj_set_prop("btn_mic_boost", GUI_OBJ_TEXT_FMT, "Boost|%s", v ? "OFF" : "ON");
				hamradio_set_gmikeboost20db(! v);
#endif /* WITHAFCODEC1HAVEPROC */
			}
			else if (gui_check_obj(name, "btn_mic_agc"))
			{
				uint8_t v = hamradio_get_gmikeagc();
				gui_obj_set_prop("btn_mic_agc", GUI_OBJ_LOCK, v ? BUTTON_NON_LOCKED : BUTTON_LOCKED);
				gui_obj_set_prop("btn_mic_agc", GUI_OBJ_TEXT_FMT, "AGC|%s", v ? "OFF" : "ON");
				hamradio_set_gmikeagc(! v);
			}
			else if (gui_check_obj(name, "btn_mic_OK"))
			{
				close_all_windows();
				return;
			}
		}
		else if (IS_SLIDER_MOVE)
		{
			uint8_t v = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);

			if (gui_check_obj(name, "sl_micLevel"))
			{
				uint_fast16_t level = level_min + normalize(v, 0, 100, level_max - level_min);
				gui_obj_set_prop("lbl_micLevel", GUI_OBJ_TEXT_FMT, "Level: %3d", level);
				hamradio_set_mik1level(level);
			}
			else if (gui_check_obj(name, "sl_micClip"))
			{
				uint_fast16_t clip = clip_min + normalize(v, 0, 100, clip_max - clip_min);
				gui_obj_set_prop("lbl_micClip", GUI_OBJ_TEXT_FMT, "Clip : %3d", clip);
				hamradio_set_gmikehclip(clip);
			}
			else if (gui_check_obj(name, "sl_micAGC"))
			{
				uint_fast16_t agc = agc_min + normalize(v, 0, 100, agc_max - agc_min);
				gui_obj_set_prop("lbl_micAGC", GUI_OBJ_TEXT_FMT, "AGC  : %3d", agc);
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
	uint8_t update = 0;

	if (is_win_init())
	{
		update = 1;
		char name[NAME_ARRAY_SIZE];

		for (uint8_t i = 0; i < NMICPROFCELLS; i ++)
		{
			local_snprintf_P(name, NAME_ARRAY_SIZE, "btn_mic_profile_%01d", i);
			gui_obj_create(name, LONG_BUTTON_STYLE, 0, 1, "");
			uint8_t cell_saved = hamradio_load_mic_profile(i, 0);
			gui_obj_set_prop(name, GUI_OBJ_PAYLOAD, (i << 16) | cell_saved);
		}

		gui_arrange_objects_from("btn_mic_profile_0", NMICPROFCELLS, 3, 6);

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_AP_MIC_PROF)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			uint32_t p = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			uint8_t cell_saved = p & 1;
			uint8_t profile_id = p >> 16;

			if (cell_saved)
			{
				hamradio_load_mic_profile(profile_id, 1);
				close_window(DONT_OPEN_PARENT_WINDOW);
				footer_buttons_state(CANCELLED);
				return;
			}
		}
		else if (IS_BUTTON_LONG_PRESS)
		{
			uint32_t p = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			uint8_t cell_saved = p & 1;
			uint8_t profile_id = p >> 16;

			gui_obj_set_prop(name, GUI_OBJ_PAYLOAD, (profile_id << 16) | ! cell_saved);

			if (cell_saved)
				hamradio_clean_mic_profile(profile_id);
			else
				hamradio_save_mic_profile(profile_id);

			update = 1;
		}
		break;

	default:
		break;
	}

	if (update)
	{
		update = 0;

		for (uint8_t i = 0; i < NMICPROFCELLS; i ++)
		{
			char * name = get_obj_name_by_idx(TYPE_BUTTON, i);
			uint32_t p = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			gui_obj_set_prop(name, GUI_OBJ_TEXT_FMT, "%d|%s", i + 1, p & 1 ? "saved" : "clean");
		}
	}
}

// *********************************************************************************************************************************************************************

void window_notch_process(void)
{
	static enc_var_t notch;
	static uint8_t lbls[2];

	if (is_win_init())
	{
		uint8_t interval = 30;
		notch.change = 0;
		notch.updated = 1;

		lbls[0] = gui_obj_create("lbl_freq", COLORPIP_WHITE, 14);
		lbls[1] = gui_obj_create("lbl_width", COLORPIP_WHITE, 14);
		gui_obj_create("btn_type", COMMON_BUTTON_STYLE, 0, 0, "");
		gui_obj_create("btn_add", SMALL_BUTTON_STYLE, 0, 0, "+");
		gui_obj_create("btn_sub", SMALL_BUTTON_STYLE, 0, 0, "-");

		gui_arrange_objects_from("lbl_freq", 2, 1, interval);
		gui_obj_align_to("btn_add", "lbl_freq", ALIGN_RIGHT_UP, interval);
		gui_obj_align_to("btn_sub", "btn_add", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("btn_type", "lbl_width", ALIGN_DOWN_LEFT, interval);

		gui_obj_set_prop("lbl_freq", GUI_OBJ_PAYLOAD, TYPE_NOTCH_FREQ);
		gui_obj_set_prop("lbl_width", GUI_OBJ_PAYLOAD, TYPE_NOTCH_WIDTH);
		gui_obj_set_prop("btn_add", GUI_OBJ_PAYLOAD, 1);
		gui_obj_set_prop("btn_sub", GUI_OBJ_PAYLOAD, -1);

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_NOTCH)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_type"))
			{
				uint8_t type = hamradio_get_gnotchtype();
				hamradio_set_gnotchtype(type == BOARD_NOTCH_AUTO ? 1 : 0);
				notch.change = 0;
				notch.updated = 1;
			}
			else
			{
				notch.change = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
				notch.updated = 1;
			}
		}
		else if (IS_LABEL_PRESS)
		{
			notch.select = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
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
		notch.updated = 0;
		uint8_t type = hamradio_get_gnotchtype();

		for(uint8_t i = 0; i < ARRAY_SIZE(lbls); i ++)
			gui_obj_set_prop(get_obj_name_by_idx(TYPE_LABEL, lbls[i]), GUI_OBJ_COLOR,
					lbls[i] == notch.select ? COLORPIP_YELLOW : COLORPIP_WHITE);

		gui_obj_set_prop("btn_type", GUI_OBJ_LOCK, type == BOARD_NOTCH_AUTO ? BUTTON_LOCKED : BUTTON_NON_LOCKED);
		gui_obj_set_prop("btn_type", GUI_OBJ_TEXT, type == BOARD_NOTCH_AUTO ? "Auto" : "Manual");
		gui_obj_set_prop("lbl_freq", GUI_OBJ_TEXT_FMT, "Freq:%5d Hz",
				hamradio_notch_freq(notch.select == TYPE_NOTCH_FREQ ? notch.change : 0));
		gui_obj_set_prop("lbl_width", GUI_OBJ_TEXT_FMT, "Width:%4d Hz",
				hamradio_notch_width(notch.select == TYPE_NOTCH_WIDTH ? notch.change : 0));
	}
}

// *********************************************************************************************************************************************************************

void window_gui_settings_process(void)
{
	uint8_t update = 0;

	if (is_win_init())
	{
		update = 1;

		gui_obj_create("btn_enc2_step", LONG_BUTTON_STYLE, 0, 0, "");
		gui_obj_create("btn_freq_swipe", LONG_BUTTON_STYLE, 0, 0, "");
		gui_obj_create("btn_freq_swipe_step", LONG_BUTTON_STYLE, 0, 0, "");

		gui_arrange_objects_from("btn_enc2_step", 3, 3, 6);

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_GUI_SETTINGS)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_enc2_step"))
				gui_nvram.enc2step_pos = (gui_nvram.enc2step_pos + 1 ) % enc2step_vals;
			else if (gui_check_obj(name, "btn_freq_swipe"))
				gui_nvram.freq_swipe_enable = ! gui_nvram.freq_swipe_enable;
			else if (gui_check_obj(name, "btn_freq_swipe_step"))
				gui_nvram.freq_swipe_step = (gui_nvram.freq_swipe_step + 1 ) % freq_swipe_step_vals;

			save_settings();
			update = 1;
		}
		break;

	default:
		break;
	}

	if (update)
	{
		update = 0;

		gui_obj_set_prop("btn_enc2_step", GUI_OBJ_TEXT_FMT, "Enc2 step|%s", enc2step[gui_nvram.enc2step_pos].label);
		gui_obj_set_prop("btn_freq_swipe", GUI_OBJ_TEXT_FMT, "Freq swipe|%s", gui_nvram.freq_swipe_enable ? "enable" : "disable");
		gui_obj_set_prop("btn_freq_swipe", GUI_OBJ_LOCK, gui_nvram.freq_swipe_enable);
		gui_obj_set_prop("btn_freq_swipe_step", GUI_OBJ_TEXT_FMT, "Swipe step|%s", freq_swipe_step[gui_nvram.freq_swipe_step].label);
		gui_obj_set_prop("btn_freq_swipe_step", GUI_OBJ_STATE, gui_nvram.freq_swipe_enable ? CANCELLED : DISABLED);
	}
}

// *********************************************************************************************************************************************************************

void window_shift_process(void)
{
#if WITHIQSHIFT
	static uint8_t cic_test = 0;
	static enc_var_t enc;
	static uint8_t lbls[3];

	if (is_win_init())
	{
		enc.updated = 1;
		enc.select = 0;
		enc.change = 0;

		lbls[0] = gui_obj_create("lbl_rx_cic_shift", COLORPIP_WHITE, 13);
		lbls[1] = gui_obj_create("lbl_rx_fir_shift", COLORPIP_WHITE, 13);
		lbls[2] = gui_obj_create("lbl_tx_shift", COLORPIP_WHITE, 13);
		gui_obj_create("lbl_iq_test", COLORPIP_WHITE, 23);

		gui_obj_create("btn_p", SMALL_BUTTON_STYLE, 0, 0, "+");
		gui_obj_create("btn_m", SMALL_BUTTON_STYLE, 0, 0, "-");
		gui_obj_create("btn_test", COMMON_BUTTON_STYLE, 0, 0, "CIC|test");

		gui_obj_set_prop("lbl_rx_cic_shift", GUI_OBJ_PAYLOAD, 0);
		gui_obj_set_prop("lbl_rx_fir_shift", GUI_OBJ_PAYLOAD, 1);
		gui_obj_set_prop("lbl_tx_shift", GUI_OBJ_PAYLOAD, 2);
		gui_obj_set_prop("lbl_iq_test", GUI_OBJ_TEXT, "MAX IQ test:");
		gui_obj_set_prop("lbl_iq_test", GUI_OBJ_STATE, DISABLED);
		gui_obj_set_prop("btn_p", GUI_OBJ_PAYLOAD, 1);
		gui_obj_set_prop("btn_m", GUI_OBJ_PAYLOAD, -1);
		gui_obj_set_prop("btn_p", GUI_OBJ_REPEAT, 1);
		gui_obj_set_prop("btn_m", GUI_OBJ_REPEAT, 1);

		gui_arrange_objects_from("lbl_rx_cic_shift", 4, 1, 20);

		gui_obj_align_to("btn_m", "lbl_rx_cic_shift", ALIGN_RIGHT_UP, 15);
		gui_obj_align_to("btn_p", "btn_m", ALIGN_RIGHT_UP, 15);
		gui_obj_align_to("btn_test", "btn_m", ALIGN_DOWN_LEFT, 15);

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_SHIFT)
	{
	case WM_MESSAGE_ACTION:
		if (IS_LABEL_PRESS)
		{
			enc.select = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			enc.change = 0;
			enc.updated = 1;
		}
		else if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_test"))
			{
				cic_test = ! cic_test;
				iq_cic_test(cic_test);
				gui_obj_set_prop("btn_test", GUI_OBJ_LOCK, cic_test);
			}
			else
			{
				enc.change = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
				enc.updated = 1;
			}
		}
		break;

	case WM_MESSAGE_ENC2_ROTATE:
		enc.change = action;
		enc.updated = 1;
		break;

	case WM_MESSAGE_UPDATE:
		enc.updated = 1;
		enc.change = 0;
		break;

	default:
		break;
	}

	if (enc.updated)
	{
		enc.updated = 0;

		for(int i = 0; i < ARRAY_SIZE(lbls); i ++)
			gui_obj_set_prop(get_obj_name_by_idx(TYPE_LABEL, i), GUI_OBJ_COLOR, enc.select == i ? COLORPIP_YELLOW : COLORPIP_WHITE);

		if (enc.select == 0)
			iq_shift_cic_rx(iq_shift_cic_rx(0) + enc.change);
		else if (enc.select == 1)
			iq_shift_fir_rx(iq_shift_fir_rx(0) + enc.change);
		else if (enc.select == 2)
			iq_shift_tx(iq_shift_tx(0) + enc.change);

		gui_obj_set_prop("lbl_rx_cic_shift", GUI_OBJ_TEXT_FMT, "RX CIC: %d", iq_shift_cic_rx(0));
		gui_obj_set_prop("lbl_rx_fir_shift", GUI_OBJ_TEXT_FMT, "RX FIR: %d", iq_shift_fir_rx(0));
		gui_obj_set_prop("lbl_tx_shift", GUI_OBJ_TEXT_FMT, "TX CIC: %d", iq_shift_tx(0));
		gui_obj_set_prop("btn_test", GUI_OBJ_LOCK, cic_test);

		if (cic_test)
			gui_obj_set_prop("lbl_iq_test", GUI_OBJ_TEXT_FMT, "MAX IQ test: 0x%08lx", iq_cic_test_process());
	}
#endif /* WITHIQSHIFT */
}

// *********************************************************************************************************************************************************************

//todo: необходима проверка работоспособности этих окон на трансивере с кнопками
void gui_uif_editmenu(const char * name, uint16_t menupos, uint8_t exitkey)
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
	static uint16_t window_center_x, reinit = 0;
	int rotate = 0;

	if (is_win_init())
	{
		reinit = 1;

		gui_obj_create("btn_UIF-", SMALL_BUTTON_STYLE, 0, 0, "+");
		gui_obj_create("btn_UIF+", SMALL_BUTTON_STYLE, 0, 0, "+");
		gui_obj_create("lbl_uif_val", COLORPIP_WHITE, 10);

		gui_obj_align_to("btn_UIF-", "lbl_uif_val", ALIGN_DOWN_LEFT, 6);
		gui_obj_align_to("btn_UIF+", "btn_UIF-", ALIGN_RIGHT_UP, 6);

		gui_obj_set_prop("lbl_uif_val", GUI_OBJ_TEXT, hamradio_gui_edit_menu_item(menu_uif.menupos, 0));

		window_set_title(menu_uif.name);
		window_set_title_align(TITLE_ALIGNMENT_CENTER);

		hamradio_enable_keyboard_redirect();
		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_UIF)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
			rotate = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);

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
		window_set_title(menu_uif.name);

		gui_obj_set_prop("lbl_uif_val", GUI_OBJ_TEXT, hamradio_gui_edit_menu_item(menu_uif.menupos, 0));

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
		open_window(get_win(WINDOW_MENU));
		footer_buttons_state(DISABLED, NULL);
	}
}

// ****** Common windows ***********************************************************************

void window_af_process(void)
{
	static enc_var_t bp_t;
	const char * const lbl_array[3] = { "lbl_low", "lbl_high", "lbl_afr", };

	if (is_win_init())
	{
		uint8_t interval = 20;
		bp_t.select = TYPE_BP_LOW;
		bp_t.change = 0;
		bp_t.updated = 1;

		gui_obj_create("lbl_low", COLORPIP_WHITE, 16);
		gui_obj_create("lbl_high", COLORPIP_WHITE, 16);
		gui_obj_create("lbl_afr", COLORPIP_WHITE, 16);
		gui_obj_create("btn_add", SMALL_BUTTON_STYLE, 0, 0, "+");
		gui_obj_create("btn_sub", SMALL_BUTTON_STYLE, 0, 0, "-");

		gui_obj_set_prop("lbl_low", GUI_OBJ_PAYLOAD, TYPE_BP_LOW);
		gui_obj_set_prop("lbl_high", GUI_OBJ_PAYLOAD, TYPE_BP_HIGH);
		gui_obj_set_prop("lbl_afr", GUI_OBJ_PAYLOAD, TYPE_AFR);
		gui_obj_set_prop("btn_add", GUI_OBJ_PAYLOAD, 1);
		gui_obj_set_prop("btn_sub", GUI_OBJ_PAYLOAD, -1);

		gui_obj_align_to("lbl_high", "lbl_low", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("lbl_afr", "lbl_high", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("btn_add", "lbl_low", ALIGN_RIGHT_UP, interval);
		gui_obj_align_to("btn_sub", "btn_add", ALIGN_DOWN_LEFT, interval);

		hamradio_enable_encoder2_redirect();
		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_AF)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_add") || gui_check_obj(name, "btn_sub"))
			{
				bp_t.change = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
				bp_t.updated = 1;
			}
		}
		else if (IS_LABEL_PRESS)
		{
			bp_t.select = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
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

		char str_low[TEXT_ARRAY_SIZE], str_high[TEXT_ARRAY_SIZE];
		const uint8_t bp_wide = hamradio_get_bp_type_wide();
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

		for(uint8_t i = 0; i < ARRAY_SIZE(lbl_array); i ++)
			gui_obj_set_prop(lbl_array[i], GUI_OBJ_COLOR, COLORPIP_WHITE);

		GUI_ASSERT(bp_t.select < 3);
		gui_obj_set_prop(lbl_array[bp_t.select], GUI_OBJ_COLOR, COLORPIP_YELLOW);

		uint16_t val_low = hamradio_get_low_bp(bp_t.select == TYPE_BP_LOW ? (bp_t.change * 5) : 0);
		gui_obj_set_prop("lbl_low", GUI_OBJ_TEXT_FMT, "%s: %4u", str_low, val_low * 10);

		uint16_t val_high = hamradio_get_high_bp(bp_t.select == TYPE_BP_HIGH ?
				bp_t.change : 0) * (bp_wide ? 100 : 10);
		gui_obj_set_prop("lbl_high", GUI_OBJ_TEXT_FMT, "%s: %4u", str_high, val_high);

		gui_obj_set_prop("lbl_afr", GUI_OBJ_TEXT_FMT, "AFR      : %+4d",
				hamradio_afresponce(bp_t.select == TYPE_AFR ? bp_t.change : 0));

		bp_t.change = 0;
	}
}

// *********************************************************************************************************************************************************************

void window_mode_process(void)
{
	if (is_win_init())
	{
		gui_obj_create("btn_ModeLSB", COMMON_BUTTON_STYLE, 0, 0, "LSB");
		gui_obj_create("btn_ModeCW", COMMON_BUTTON_STYLE, 0, 0, "CW");
		gui_obj_create("btn_ModeAM", COMMON_BUTTON_STYLE, 0, 0, "AM");
		gui_obj_create("btn_ModeDGL", COMMON_BUTTON_STYLE, 0, 0, "DGL");
		gui_obj_create("btn_ModeUSB", COMMON_BUTTON_STYLE, 0, 0, "USB");
		gui_obj_create("btn_ModeCWR", COMMON_BUTTON_STYLE, 0, 0, "CWR");
		gui_obj_create("btn_ModeNFM", COMMON_BUTTON_STYLE, 0, 0, "NFM");
		gui_obj_create("btn_ModeDGU", COMMON_BUTTON_STYLE, 0, 0, "DGU");

		gui_obj_set_prop("btn_ModeLSB", GUI_OBJ_PAYLOAD, SUBMODE_LSB);
		gui_obj_set_prop("btn_ModeCW", GUI_OBJ_PAYLOAD, SUBMODE_CW);
		gui_obj_set_prop("btn_ModeAM", GUI_OBJ_PAYLOAD, SUBMODE_AM);
		gui_obj_set_prop("btn_ModeDGL", GUI_OBJ_PAYLOAD, SUBMODE_DGL);
		gui_obj_set_prop("btn_ModeUSB", GUI_OBJ_PAYLOAD, SUBMODE_USB);
		gui_obj_set_prop("btn_ModeCWR", GUI_OBJ_PAYLOAD, SUBMODE_CWR);
		gui_obj_set_prop("btn_ModeNFM", GUI_OBJ_PAYLOAD, SUBMODE_NFM);
		gui_obj_set_prop("btn_ModeDGU", GUI_OBJ_PAYLOAD, SUBMODE_DGU);

		gui_arrange_objects_from("btn_ModeLSB", 8, 4, 6);

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_MODES)
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
	uint8_t update = 0;

	if (is_win_init())
	{
		update = 1;

		const char btn_names[][NAME_ARRAY_SIZE] = { "btn_att", "btn_agc", "btn_mode", "btn_preamp", "btn_af", "btn_dnr", "btn_wnb", };

		for (int i = 0; i < ARRAY_SIZE(btn_names); i ++)
			gui_obj_create(btn_names[i], LONG_BUTTON_STYLE, 0, 0, "");

		gui_arrange_objects(btn_names, ARRAY_SIZE(btn_names), 4, 6);

		gui_obj_set_prop("btn_mode", GUI_OBJ_TEXT, "Mode");
		gui_obj_set_prop("btn_af", GUI_OBJ_TEXT, "AF|filter");
		gui_obj_set_prop("btn_dnr", GUI_OBJ_TEXT, "DNR");
		gui_obj_set_prop("btn_wnb", GUI_OBJ_TEXT, "WNB");
		gui_obj_set_prop("btn_wnb", GUI_OBJ_LONG_PRESS, 1);
		gui_obj_set_prop("btn_preamp", GUI_OBJ_TEXT, "Preamp");

#if WITHWNB
		gui_obj_set_prop("btn_wnb", GUI_OBJ_LOCK, wnb_state_switch(0));
#endif /* WITHWNB */

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_RECEIVE)
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
		const char * a = remove_start_line_spaces(hamradio_get_att_value_P());
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
	uint8_t update = 0;
	char buf[NAME_ARRAY_SIZE];

	if (is_win_init())
	{
		char btns_text[12][3] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "<", "0", "OK" };
		uint8_t btns_payload[12] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, BUTTON_CODE_BK, 0, BUTTON_CODE_OK };
		char btn_name[NAME_ARRAY_SIZE];

		for (uint8_t i = 0; i < 12; i ++)
		{
			local_snprintf_P(btn_name, NAME_ARRAY_SIZE, "btn_%02d", btns_payload[i]);
			gui_obj_create(btn_name, 50, 50, 0, 0, btns_text[i]);
			gui_obj_set_prop(btn_name, GUI_OBJ_PAYLOAD, btns_payload[i]);
		}

		gui_arrange_objects_from("btn_01", 12, 3, 6);
		gui_obj_set_prop("btn_30", GUI_OBJ_LOCK, 1);

		editfreq.val = 0;
		editfreq.num = 0;
		editfreq.key = BUTTON_CODE_DONE;

		local_snprintf_P(buf, NAME_ARRAY_SIZE, "%d k", (int) editfreq.val);
		window_set_title(buf);
		window_set_title_align(TITLE_ALIGNMENT_CENTER);

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_FREQ)
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
				{
					close_all_windows();
					return;
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

			local_snprintf_P(buf, NAME_ARRAY_SIZE, "%d k", (int) editfreq.val);
			window_set_title(editfreq.error ? "ERROR" : buf);
		}
	}
}

// *****************************************************************************************************************************


void window_time_process(void)
{
#if defined (RTC1_TYPE)
	static uint_fast16_t year;
	static uint_fast8_t month, day, hour, minute, second, update;

	if (is_win_init())
	{
		update = 1;
		uint8_t interval = 20;

		gui_obj_create("lbl_day", COLORPIP_WHITE, 4);
		gui_obj_create("lbl_month", COLORPIP_WHITE, 4);
		gui_obj_create("lbl_year", COLORPIP_WHITE, 4);
		gui_obj_create("lbl_hour", COLORPIP_WHITE, 4);
		gui_obj_create("lbl_minute", COLORPIP_WHITE, 4);
		gui_obj_create("lbl_second", COLORPIP_WHITE, 4);

		gui_obj_create("btn_day+", SMALL_BUTTON_STYLE, 0, 0, "+");
		gui_obj_create("btn_month+", SMALL_BUTTON_STYLE, 0, 0, "+");
		gui_obj_create("btn_year+", SMALL_BUTTON_STYLE, 0, 0, "+");
		gui_obj_create("btn_hour+", SMALL_BUTTON_STYLE, 0, 0, "+");
		gui_obj_create("btn_minute+", SMALL_BUTTON_STYLE, 0, 0, "+");
		gui_obj_create("btn_second+", SMALL_BUTTON_STYLE, 0, 0, "+");
		gui_obj_create("btn_sec0", SMALL_BUTTON_STYLE, 0, 0, ">0");
		gui_obj_create("btn_day-", SMALL_BUTTON_STYLE, 0, 0, "-");
		gui_obj_create("btn_month-", SMALL_BUTTON_STYLE, 0, 0, "-");
		gui_obj_create("btn_year-", SMALL_BUTTON_STYLE, 0, 0, "-");
		gui_obj_create("btn_hour-", SMALL_BUTTON_STYLE, 0, 0, "-");
		gui_obj_create("btn_minute-", SMALL_BUTTON_STYLE, 0, 0, "-");
		gui_obj_create("btn_second-", SMALL_BUTTON_STYLE, 0, 0, "-");
		gui_obj_create("btn_set", SMALL_BUTTON_STYLE, 0, 0, "Set");

		gui_arrange_objects_from("lbl_day", 6, 6, interval);
		gui_obj_align_to("btn_day+", "lbl_day", ALIGN_DOWN_LEFT, interval);
		gui_arrange_objects_from("btn_day+", 14, 7, interval);

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
		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_TIME)
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

		gui_obj_set_prop("lbl_year", GUI_OBJ_TEXT_FMT, "Y:%d", year - 2000);
		gui_obj_set_prop("lbl_month", GUI_OBJ_TEXT_FMT, "M:%02d", month);
		gui_obj_set_prop("lbl_day", GUI_OBJ_TEXT_FMT, "D:%02d", day);
		gui_obj_set_prop("lbl_hour", GUI_OBJ_TEXT_FMT, "H:%02d", hour);
		gui_obj_set_prop("lbl_minute", GUI_OBJ_TEXT_FMT, "M:%02d", minute);
		gui_obj_set_prop("lbl_second", GUI_OBJ_TEXT_FMT, "S:%02d", second);
	}
#endif /* defined (RTC1_TYPE) */
}

void window_kbd_process(void)
{
	static uint8_t update = 0, is_shift = 0;
	const char kbd_cap[] = ",.!@#*()?;QWERTYUIOPASDFGHJKLZXCVBNM";
	const char kbd_low[] = "1234567890qwertyuiopasdfghjklzxcvbnm";
	const uint8_t len1 = 10, len2 = 10, len3 = 9, len4 = 7, kbd_len = len1 + len2 + len3 + len4, btn_size = 40;
	const char kbd_func[5][20] = { "btn_kbd_caps", "btn_kbd_backspace", "btn_kbd_space", "btn_kbd_enter", "btn_kbd_esc", };
	static char edit_str[TEXT_ARRAY_SIZE];
	char btn_name[NAME_ARRAY_SIZE];

	if (is_win_init())
	{
		update = 1;
		is_shift = 0;
		uint16_t x = 0, y = 0, interval = 5, i = 0;

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
				local_snprintf_P(btn_name, NAME_ARRAY_SIZE, "%s", kbd_func[i - kbd_len]);

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

		window_set_title(edit_str);
		window_set_title_align(TITLE_ALIGNMENT_CENTER);

		calculate_window_position(WINDOW_POSITION_AUTO);
		return;
	}

	GET_FROM_WM_QUEUE(WINDOW_KBD)
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
				uint8_t l = strlen(edit_str);
				if (l)
					edit_str[l - 1] = '\0';
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

			window_set_title(edit_str);
		}
		break;

	default:
		break;
	}

	if (update)
	{
		update = 0;

		for(uint8_t i = 0; i < kbd_len; i ++)
		{
			local_snprintf_P(btn_name, NAME_ARRAY_SIZE, "btn_kbd_%02d", i);
			char p = is_shift ? kbd_cap[i] : kbd_low[i];
			gui_obj_set_prop(btn_name, GUI_OBJ_TEXT_FMT, "%c", p);
			gui_obj_set_prop(btn_name, GUI_OBJ_PAYLOAD, p);
		}
	}
}

// *********************************************************************************************************************************************************************

void window_kbd_test_process(void)
{
	static int num_lbl1 = 12345;
	static char str_lbl2[TEXT_ARRAY_SIZE] = "qwertyuiopas";
	const uint8_t win_id = WINDOW_KBD_TEST;

	if (is_win_init())
	{
		const uint8_t interval = 30;

		gui_obj_create("lbl_text", COLORPIP_WHITE, 20);
		gui_obj_create("lbl_num", COLORPIP_WHITE, 20);

		gui_obj_create("btn_text", COMMON_BUTTON_STYLE, 0, 0, "Edit...");
		gui_obj_create("btn_num", COMMON_BUTTON_STYLE, 0, 0, "Edit...");

		gui_obj_set_prop("lbl_text", GUI_OBJ_POS_Y, 20);

		gui_obj_align_to("lbl_num", "lbl_text", ALIGN_DOWN_LEFT, interval);
		gui_obj_align_to("btn_text", "lbl_text", ALIGN_RIGHT_UP_MID, interval);
		gui_obj_align_to("btn_num", "lbl_num", ALIGN_RIGHT_UP_MID, interval);

		gui_obj_set_prop("lbl_text", GUI_OBJ_TEXT, str_lbl2);
		gui_obj_set_prop("lbl_num", GUI_OBJ_TEXT_FMT, "%d", num_lbl1);

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_KBD_TEST)
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
	static float A = 0, B = 0, z[1760];
	char b[22][80];

	if (is_win_init())
	{
		gui_obj_create("tf_3d", 60, 22, UP, & msgothic_11x13_mono);

		calculate_window_position(WINDOW_POSITION_AUTO);
		return;
	}

	GET_FROM_WM_QUEUE(WINDOW_3D)
	{
	case WM_MESSAGE_CLOSE:
		return;

	default:
		break;
	}

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
				z[o] = D;
				b[y][x] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
			}
		}
	}

	gui_obj_set_prop("tf_3d", GUI_OBJ_CLEAN);
	for (int k = 0; k < 22; k ++)
		gui_obj_set_prop("tf_3d", GUI_OBJ_TEXT, b[k], COLORPIP_WHITE);

	A += 0.08;
	B += 0.04;
}

// *****************************************************************************************************************************

#define MENU_GROUPS_MAX	30
#define MENU_PARAMS_MAX	50
static uint8_t index_param = 0;

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
	static menu_names_t menup[MENU_PARAMS_MAX], menuv;
	static uint8_t sel = 0;
	static char btn_selected[NAME_ARRAY_SIZE];
	char btn_names[MENU_PARAMS_MAX][NAME_ARRAY_SIZE];
	char buf[NAME_ARRAY_SIZE];

	if (is_win_init())
	{
		sel = 0;

		const uint8_t count = hamradio_get_multilinemenu_block_params(menup, index_param, MENU_PARAMS_MAX);
		uint8_t cols = count <= 16 ? 4 : 5, interval = 6;

		for (uint8_t i = 0; i < count; i ++)
		{
			local_snprintf_P(btn_names[i], TEXT_ARRAY_SIZE, "btn_params_%02d", i);
			remove_end_line_spaces(menup[i].name);

			if (get_strwidth_prop(menup[i].name, & BUTTONS_FONTP_DEFAULT) > 110)   // переделать
				split_string(menup[i].name, '|');

			gui_obj_create(btn_names[i], 120, 40, 0, 0, menup[i].name);
			gui_obj_set_prop(btn_names[i], GUI_OBJ_PAYLOAD, menup[i].index);
		}

		gui_arrange_objects(btn_names, count, cols, interval);

#if 1	// Добавить экранные кнопки "+" и "-" при необходимости
		gui_obj_create("btn_p", SMALL_BUTTON_STYLE, 1, 0, "+");
		gui_obj_create("btn_m", SMALL_BUTTON_STYLE, 1, 0, "-");

		char buf[TEXT_ARRAY_SIZE];
		local_snprintf_P(buf, TEXT_ARRAY_SIZE, "btn_params_%02d", count < cols ? count - 1 : cols - 1);

		gui_obj_align_to("btn_p", buf, ALIGN_RIGHT_UP, interval);
		gui_obj_set_prop("btn_p", GUI_OBJ_PAYLOAD, 1);
		gui_obj_align_to("btn_m", "btn_p", ALIGN_DOWN_LEFT, interval);
		gui_obj_set_prop("btn_m", GUI_OBJ_PAYLOAD, -1);
#endif

		window_set_title("Edit param: choose...");
		window_set_title_align(TITLE_ALIGNMENT_CENTER);
		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_MENU_PARAMS)
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

				strncpy(btn_selected, name, NAME_ARRAY_SIZE);
				gui_obj_set_prop(btn_selected, GUI_OBJ_LOCK, 1);
				sel = 1;

				hamradio_get_multilinemenu_block_vals(& menuv, gui_obj_get_int_prop(btn_selected, GUI_OBJ_PAYLOAD), 1);
				remove_end_line_spaces(menuv.name);
				local_snprintf_P(buf, NAME_ARRAY_SIZE, "%s: %s",
						gui_obj_get_string_prop(btn_selected, GUI_OBJ_TEXT), menuv.name);
				window_set_title(buf);

			}
		}
		break;

	case WM_MESSAGE_ENC2_ROTATE:
	{
		char edit_val[20];
		if (sel)
		{
			strcpy(edit_val, hamradio_gui_edit_menu_item(menuv.index, action));
			remove_end_line_spaces(edit_val);
			local_snprintf_P(buf, NAME_ARRAY_SIZE, "%s: %s",
					gui_obj_get_string_prop(btn_selected, GUI_OBJ_TEXT), edit_val);
			window_set_title(buf);
		}
	}
		break;

	default:
		break;
	}
}

void window_menu_process(void)
{
	if (is_win_init())
	{
		menu_names_t menu[MENU_GROUPS_MAX];
		char btn_names[MENU_GROUPS_MAX][NAME_ARRAY_SIZE];

		const uint8_t count = hamradio_get_multilinemenu_block_groups(menu);
		GUI_ASSERT(count < MENU_GROUPS_MAX);

		for (uint8_t i = 0; i < count; i ++)
		{
			local_snprintf_P(btn_names[i], TEXT_ARRAY_SIZE, "btn_groups_%02d", i);
			remove_end_line_spaces(menu[i].name);

			if (get_strwidth_prop(menu[i].name, & BUTTONS_FONTP_DEFAULT) > 110)  // переделать
				split_string(menu[i].name, '|');

			gui_obj_create(btn_names[i], 120, 40, 0, 0, menu[i].name);
			gui_obj_set_prop(btn_names[i], GUI_OBJ_PAYLOAD, menu[i].index);
		}

		gui_arrange_objects(btn_names, count, 4, 6);

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_MENU)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			index_param = gui_obj_get_int_prop(name, GUI_OBJ_PAYLOAD);
			open_window(get_win(WINDOW_MENU_PARAMS));
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
	static enc_var_t enc;
	static uint8_t lbls[3];

	if (is_win_init())
	{
		uint8_t interval = 24;
		enc.select = 0;
		enc.change = 0;
		enc.updated = 1;

		gui_obj_create("lbl_nmeatime", COLORPIP_WHITE, 20);
		lbls[0] = gui_obj_create("lbl_timeoffset", COLORPIP_WHITE, 20);
		lbls[1] = gui_obj_create("lbl_stopfreq", COLORPIP_WHITE, 20);
		lbls[2] = gui_obj_create("lbl_interval", COLORPIP_WHITE, 20);

		gui_obj_create("btn_state", COMMON_BUTTON_STYLE, 0, 0, "");
		gui_obj_create("btn_draw", COMMON_BUTTON_STYLE, 0, 0, "Draw|spectre");
		gui_obj_create("btn_p", SMALL_BUTTON_STYLE, 0, 0, "+");
		gui_obj_create("btn_m", SMALL_BUTTON_STYLE, 0, 0, "-");

		gui_arrange_objects_from("lbl_nmeatime", 4, 1, interval);
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
		enable_window_move();
		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_LFM)
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

				enc.updated = 1;
			}
			else if (gui_check_obj(name, "btn_draw"))
				open_window(get_win(WINDOW_LFM_SPECTRE));
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
		enc.updated = 1;
		break;

	default:
		break;
	}

	if (enc.updated)
	{
		enc.updated = 0;

		for(uint8_t i = 0; i < ARRAY_SIZE(lbls); i ++)
			gui_obj_set_prop(get_obj_name_by_idx(TYPE_LABEL, lbls[i]), GUI_OBJ_COLOR, COLORPIP_WHITE);

		GUI_ASSERT(enc.select < 3);
		gui_obj_set_prop(get_obj_name_by_idx(TYPE_LABEL, lbls[enc.select]), GUI_OBJ_COLOR, COLORPIP_YELLOW);

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
	static gui_color_t d[xmax][ymax];
	static int shift = 0;

	static uint16_t xx = 0;

	if (is_win_init())
	{
		xx = 0;
		shift = 0;

		memset(d, GUI_DEFAULTCOLOR, sizeof(d));
		calculate_window_position(WINDOW_POSITION_MANUAL_SIZE, xmax, ymax);
		return;
	}

	GET_FROM_WM_QUEUE(WINDOW_LFM_SPECTRE)
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

void window_wnbconfig_process(void)
{
#if WITHWNB
	static enc_var_t enc;

	if (is_win_init())
	{
		enc.updated = 1;

		gui_obj_create("btn_add", SMALL_BUTTON_STYLE, 0, 0, "+");
		gui_obj_create("btn_sub", SMALL_BUTTON_STYLE, 0, 0, "-");
		gui_obj_create("lbl_val", COLORPIP_WHITE, 14);

		gui_obj_set_prop("lbl_val", GUI_OBJ_POS, 0, 15);
		gui_obj_align_to("btn_sub", "lbl_val", ALIGN_RIGHT_UP_MID, 10);
		gui_obj_set_prop("btn_sub", GUI_OBJ_PAYLOAD, -1);
		gui_obj_align_to("btn_add", "btn_sub", ALIGN_RIGHT_UP, 10);
		gui_obj_set_prop("btn_add", GUI_OBJ_PAYLOAD, 1);

		hamradio_enable_encoder2_redirect();
		enable_window_move();
		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_WNBCONFIG)
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
		enc.change = 0;
		enc.updated = 1;
		break;

	default:
		break;
	}

	if (enc.updated)
	{
		enc.updated = 0;

		uint_fast16_t v = wnb_get_threshold();
		wnb_set_threshold(v + enc.change);
		enc.change = 0;

		gui_obj_set_prop("lbl_val", GUI_OBJ_TEXT_FMT, "Threshold: %d", wnb_get_threshold());
	}
#endif /* WITHWNB */
}

void window_as_process(void)
{
#if WITHAUDIOSAMPLESREC
	uint8_t update = 0;
	enum { len = common_btn_width * 4, lim = WITHGUIMAXY / 20 };
	static uint32_t d[len];

	if (is_win_init())
	{
		gui_obj_create("btn_rec", LONG_BUTTON_STYLE, 0, 0, "Record");
		gui_obj_create("btn_play", LONG_BUTTON_STYLE, 0, 0, "Play");
		gui_obj_create("btn_tx", LONG_BUTTON_STYLE, 0, 0, "Transmit");

		gui_obj_set_prop("btn_rec", GUI_OBJ_POS_Y, lim * 3);
		gui_obj_align_to("btn_play", "btn_rec", ALIGN_RIGHT_UP, 10);
		gui_obj_align_to("btn_tx", "btn_play", ALIGN_RIGHT_UP, 10);

		if (as_get_state() == AS_IDLE)
			memset(d, 0, ARRAY_SIZE(d));

		calculate_window_position(WINDOW_POSITION_AUTO);
		update = 1;
	}

	GET_FROM_WM_QUEUE(WINDOW_AS)
	{
	case WM_MESSAGE_ACTION:
		if (IS_BUTTON_PRESS)
		{
			if (gui_check_obj(name, "btn_rec"))
			{
				memset(d, 0, len * sizeof(uint32_t));
				as_toggle_record();
			}
			else if (gui_check_obj(name, "btn_play"))
				as_toggle_play();
			else if (gui_check_obj(name, "btn_tx"))
				as_toggle_trx();

			update = 1;
		}

	case WM_MESSAGE_UPDATE:
		update = 1;
		break;

	case WM_MESSAGE_CLOSE:
		return;

	default:
		break;
	}

	for (int x = 0; x < len; x ++)
		gui_drawline(x, lim - d[x], x, lim + d[x], COLORPIP_WHITE);

	if (as_get_state() == AS_PLAYING || as_get_state() == AS_TX)
	{
		uint16_t pos = len * as_get_progress() / 100;
		gui_drawline(pos, 0, pos, lim * 2, COLORPIP_GREEN);
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

		if ((s == AS_READY && olds == AS_RECORDING) || s == AS_RECORDING)
			as_draw_spectrogram(d, len, lim);

		olds = s;
	}
#endif /* WITHAUDIOSAMPLESREC */
}

void window_fanpwm_process(void)
{
#if WITHCPUFANPWM
	static int8_t duty = 0;

	if (is_win_init())
	{
		duty = linux_fan_pwm_set(-1);

		gui_obj_create("lbl_pwm", COLORPIP_YELLOW, 10);
		gui_obj_set_prop("lbl_pwm", GUI_OBJ_TEXT_FMT, "Duty: %d", duty);

		calculate_window_position(WINDOW_POSITION_AUTO);
	}

	GET_FROM_WM_QUEUE(WINDOW_FANPWM)
	{
		case WM_MESSAGE_ENC2_ROTATE:
			duty += action;
			if (duty < 0) duty = 0;
			if (duty > 100) duty = 100;
			linux_fan_pwm_set(duty);

			gui_obj_set_prop("lbl_pwm", GUI_OBJ_TEXT_FMT, "Duty: %d", duty);
	}

#endif /* WITHCPUFANPWM */
}

#if WITHSDL2VIDEO
#include "gui_port.h"

void gui_sdl2_set_renderer(void * r)
{
	__gui_set_drawbuf(r);
}

void gui_sdl2_walkthrough(void)
{
	process_gui();
}
#else
#include "gui_port.h"
/* Запуск state mashine и отрисовка элементов GUI */
void gui_WM_walkthrough(const gui_drawbuf_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t xpan, uint_fast8_t yspan, dctx_t * pctx)
{
	__gui_set_drawbuf((gui_drawbuf_t *) db);
	process_gui();
}
#endif /* ! WITHSDL2VIDEO */

#endif /* WITHTOUCHGUI */
