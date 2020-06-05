/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "audio.h"

#include "src/display/display.h"
#include "src/display/fontmaps.h"
#include "formats.h"

#include <string.h>
#include <math.h>
#include "src/gui/gui.h"
#include "src/gui/gui_system.h"
#include "src/gui/gui_structs.h"

#include "keyboard.h"
#include "list.h"
#include <src/codecs/nau8822.h>

#if WITHTOUCHGUI

static uint_fast8_t swr_scan_enable = 0;		// флаг разрешения сканирования КСВ
static uint_fast8_t swr_scan_stop = 0;
static uint_fast8_t * y_vals;					// массив КСВ
static editfreq_t editfreq;

// *********************************************************************************************************************************************************************

void btn_main_handler(void)
{
	gui_t * gui = get_gui_env();
	enc2_t * encoder2 = get_enc2_env();

	if(gui->selected_type == TYPE_BUTTON)
	{
		window_t * winMain = get_win(WINDOW_MAIN);
		button_t * btn_Mode = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_Mode");
		button_t * btn_AF = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_AF");
		button_t * btn_AGC = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_AGC");
		button_t * btn_Freq = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_Freq");
		button_t * btn_5 = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_5");
		button_t * btn_SWRscan = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_SWRscan");
		button_t * btn_TXsett = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_TXsett");
		button_t * btn_AUDsett = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_AUDsett");
		button_t * btn_SysMenu = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_SysMenu");

		if (gui->selected_link->link == btn_Mode)
		{
			window_t * win = get_win(WINDOW_MODES);
			if (win->state == NON_VISIBLE)
			{
				set_window(win, VISIBLE);
				footer_buttons_state(DISABLED, ((button_t *)gui->selected_link)->name);
			}
			else
			{
				set_window(win, NON_VISIBLE);
				footer_buttons_state(CANCELLED);
			}
		}
		else if (gui->selected_link->link == btn_AF)
		{
			window_t * win = get_win(WINDOW_BP);
			if (win->state == NON_VISIBLE)
			{
				encoder2->busy = 1;
				set_window(win, VISIBLE);
				footer_buttons_state(DISABLED, ((button_t *)gui->selected_link)->name);
				hamradio_enable_keyboard_redirect();
			}
			else
			{
				set_window(win, NON_VISIBLE);
				encoder2->busy = 0;
				footer_buttons_state(CANCELLED);
				hamradio_disable_keyboard_redirect();
			}
		}
		else if (gui->selected_link->link == btn_AGC)
		{
			window_t * win = get_win(WINDOW_AGC);
			if (win->state == NON_VISIBLE)
			{
				set_window(win, VISIBLE);
				footer_buttons_state(DISABLED, ((button_t *)gui->selected_link)->name);
			}
			else
			{
				set_window(win, NON_VISIBLE);
				footer_buttons_state(CANCELLED);
			}
		}
		else if (gui->selected_link->link == btn_Freq)
		{
			window_t * win = get_win(WINDOW_FREQ);
			if (win->state == NON_VISIBLE)
			{
				set_window(win, VISIBLE);
				hamradio_set_lockmode(1);
				hamradio_enable_keyboard_redirect();
				footer_buttons_state(DISABLED, ((button_t *)gui->selected_link)->name);
			}
			else
			{
				set_window(win, NON_VISIBLE);
				hamradio_set_lockmode(0);
				hamradio_disable_keyboard_redirect();
				footer_buttons_state(CANCELLED);
			}
		}
		else if (gui->selected_link->link == btn_5)
		{

		}
		else if (gui->selected_link->link == btn_SWRscan)
		{
			window_t * win = get_win(WINDOW_SWR_SCANNER);
			if (win->state == NON_VISIBLE)
			{
				set_window(win, VISIBLE);
				footer_buttons_state(DISABLED, ((button_t *)gui->selected_link)->name);
				hamradio_set_lockmode(1);
				hamradio_enable_keyboard_redirect();
			}
			else
			{
				set_window(win, NON_VISIBLE);
				footer_buttons_state(CANCELLED);
				if(swr_scan_enable)
				{
					swr_scan_enable = 0;
					hamradio_set_tune(0);
				}
				free(y_vals);
				hamradio_set_lockmode(0);
				hamradio_disable_keyboard_redirect();
			}
		}
		else if (gui->selected_link->link == btn_TXsett)
		{
			window_t * win = get_win(WINDOW_TX_SETTINGS);
			if (win->state == NON_VISIBLE)
			{
				set_window(win, VISIBLE);
				footer_buttons_state(DISABLED, ((button_t *)gui->selected_link)->name);
			}
			else
			{
				set_window(win, NON_VISIBLE);
				footer_buttons_state(CANCELLED);
			}
		}
		else if (gui->selected_link->link == btn_AUDsett)
		{
			window_t * win = get_win(WINDOW_AUDIOSETTINGS);
			if (win->state == NON_VISIBLE)
			{
				set_window(win, VISIBLE);
				footer_buttons_state(DISABLED, ((button_t *)gui->selected_link)->name);
			}
			else
			{
				set_window(win, NON_VISIBLE);
				footer_buttons_state(CANCELLED);
			}
		}
		else if (gui->selected_link->link == btn_SysMenu)
		{
			window_t * win = get_win(WINDOW_MENU);
			if (win->state == NON_VISIBLE)
			{
				set_window(win, VISIBLE);
				footer_buttons_state(DISABLED, ((button_t *)gui->selected_link)->name);
				encoder2->busy = 1;
			}
			else
			{
				set_window(win, NON_VISIBLE);
				footer_buttons_state(CANCELLED);
				encoder2->busy = 0;
				hamradio_set_menu_cond(NON_VISIBLE);
			}
		}
	}
}

void gui_main_process(void)
{
	window_t * win = get_win(WINDOW_MAIN);
	gui_t * gui = get_gui_env();
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	char buf [TEXT_ARRAY_SIZE];
	const uint_fast8_t buflen = ARRAY_SIZE(buf);
	uint_fast16_t yt, xt, y1 = 125, y2 = 145, current_place = 0, xx;
	uint_fast8_t num_places = 8, lbl_place_width = 100;

	if (win->first_call)
	{
		uint_fast8_t interval_btn = 3, id_start, id_end;
		uint_fast16_t x = 0;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_Mode", 	 "Mode", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_AF",  	 "AF|filter", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_AGC",  	 "AGC", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_Freq",    "Freq|enter", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_5",  	 "", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_SWRscan", "SWR|scanner", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_TXsett",  "Transmit|settings", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_AUDsett", "Audio|settings", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_SysMenu", "System|settings", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		for (uint_fast8_t id = 1; id < win->bh_count; id ++)
		{
			button_t * bh = & win->bh_ptr[id];
			bh->x1 = x;
			bh->y1 = WITHGUIMAXY - bh->h;
			bh->visible = VISIBLE;
			x = x + interval_btn + bh->w;
		}

		elements_state(win);
		return;
	}

	// разметка
	for(uint_fast8_t i = 1; i < num_places; i++)
	{
		uint_fast16_t x = lbl_place_width * i;
		colmain_line(fr, DIM_X, DIM_Y, x, y1, x, y2 + SMALLCHARH2, COLORMAIN_GREEN, 0);
	}

	// текущее время
#if defined (RTC1_TYPE)
	static uint_fast16_t year;
	static uint_fast8_t month, day, hour, minute, secounds;
	if(gui->timer_1sec_updated)
		board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);
	local_snprintf_P(buf, buflen, PSTR("%02d.%02d"), day, month);
	xx = current_place * lbl_place_width + lbl_place_width / 2;
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y1, buf, COLORMAIN_WHITE);
	local_snprintf_P(buf, buflen, PSTR("%02d%c%02d"), hour, ((secounds & 1) ? ' ' : ':'), minute);
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y2, buf, COLORMAIN_WHITE);
#endif 	/* defined (RTC1_TYPE) */

	current_place++;

	// напряжение питания
#if WITHVOLTLEVEL
	static ldiv_t v;
	if(gui->timer_1sec_updated)
		v = ldiv(hamradio_get_volt_value(), 10);
	local_snprintf_P(buf, buflen, PSTR("%d.%1dV"), v.quot, v.rem);
	xx = current_place * lbl_place_width + lbl_place_width / 2;
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, hamradio_get_tx() ? y1 : (y1 + (y2 - y1) / 2), buf, COLORMAIN_WHITE);
#endif /* WITHVOLTLEVEL */

	// ток PA (при передаче)
#if WITHCURRLEVEL
	if (hamradio_get_tx())
	{
		static int_fast16_t drain;
		if (gui->timer_1sec_updated)
		{
			drain = hamradio_get_pacurrent_value();	// Ток в десятках милиампер (может быть отрицательным)
			if (drain < 0)
			{
				drain = 0;	// FIXME: без калибровки нуля (как у нас сейчас) могут быть ошибки установки тока
			}
		}

	#if (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A)
		// для больших токов (более 9 ампер)
		ldiv_t t = ldiv(drain / 10, 10);
		local_snprintf_P(buf, buflen, PSTR("%2d.%01dA"), t.quot, t.rem);

	#else /* (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A) */
		// Датчик тока до 5 ампер
		ldiv_t t = ldiv(drain, 100);
		local_snprintf_P(buf, buflen, PSTR("%d.%02dA"), t.quot, t.rem);

	#endif /* (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A) */

		colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y2, buf, COLORMAIN_WHITE);
	}
#endif /* WITHCURRLEVEL */

	current_place++;

	// ширина панорамы
#if WITHIF4DSP
	static int_fast32_t z;
	if(gui->timer_1sec_updated)
		z = display_zoomedbw() / 1000;
	local_snprintf_P(buf, buflen, PSTR("SPAN"));
	xx = current_place * lbl_place_width + lbl_place_width / 2;
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y1, buf, COLORMAIN_WHITE);
	local_snprintf_P(buf, buflen, PSTR("%dk"), z);
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y2, buf, COLORMAIN_WHITE);
#endif /* WITHIF4DSP */

	current_place++;

	// параметры полосы пропускания фильтра
	static uint_fast8_t bp_type, bp_low, bp_high;
	if(gui->timer_1sec_updated)
	{
		bp_high = hamradio_get_high_bp(0);
		bp_low = hamradio_get_low_bp(0) * 10;
		bp_type = hamradio_get_bp_type();
		bp_high = bp_type ? bp_high * 100 : bp_high * 10;
	}
	local_snprintf_P(buf, buflen, PSTR("AF"));
	xx = current_place * lbl_place_width + 7;
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx, y1 + (y2 - y1) / 2, buf, COLORMAIN_WHITE);
	xx += SMALLCHARW2 * 3;
	local_snprintf_P(buf, buflen, bp_type ? (PSTR("L %d")) : (PSTR("W %d")), bp_low);
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx, y1, buf, COLORMAIN_WHITE);
	local_snprintf_P(buf, buflen, bp_type ? (PSTR("H %d")) : (PSTR("P %d")), bp_high);
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx, y2, buf, COLORMAIN_WHITE);

	current_place++;

	// значение сдвига частоты
	static int_fast16_t if_shift;
	if (gui->timer_1sec_updated)
		if_shift = hamradio_get_if_shift();
	xx = current_place * lbl_place_width + lbl_place_width / 2;
	if (if_shift)
	{
		local_snprintf_P(buf, buflen, PSTR("IF shift"));
		colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y1, buf, COLORMAIN_WHITE);
		local_snprintf_P(buf, buflen, if_shift == 0 ? PSTR("%d") : PSTR("%+dk"), if_shift);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y2, buf, COLORMAIN_WHITE);
	}
	else
	{
		local_snprintf_P(buf, buflen, PSTR("IF shift"));
		colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y1 + (y2 - y1) / 2, buf, COLORMAIN_GRAY);
	}

//	#if WITHTHERMOLEVEL	// температура выходных транзисторов (при передаче)
//		static ldiv_t t;
//		if (hamradio_get_tx())// && gui->timer_1sec_updated)
//		{
//			t = ldiv(hamradio_get_temperature_value(), 10);
//			local_snprintf_P(buf, buflen, PSTR("%d.%dC "), t.quot, t.rem);
//			PRINTF("%s\n", buf);		// пока вывод в консоль
//		}
//	#endif /* WITHTHERMOLEVEL */

	gui->timer_1sec_updated = 0;
}

// *********************************************************************************************************************************************************************

void buttons_mode_handler(void)
{
	window_t * win = get_win(WINDOW_MODES);
	gui_t * gui = get_gui_env();
	if(gui->selected_type == TYPE_BUTTON)
	{
		button_t * bh = (button_t *)gui->selected_link->link;
		if (win->state && bh->parent == win->window_id)
		{
			if (bh->payload != UINTPTR_MAX)
				hamradio_change_submode(bh->payload);

			set_window(win, NON_VISIBLE);
			footer_buttons_state(CANCELLED);
			gui->timer_1sec_updated = 1;
		}
	}
}

void window_mode_process(void)
{
	window_t * win = get_win(WINDOW_MODES);
	gui_t * gui = get_gui_env();
	if (win->first_call)
	{
		uint_fast16_t x, y;
		uint_fast16_t xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 4;
		uint_fast8_t id_start, id_end;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,      state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_LSB, "btnModeLSB", "LSB", },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_CW,  "btnModeCW", "CW", },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_AM,  "btnModeAM", "AM", },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGL, "btnModeDGL", "DGL", },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_USB, "btnModeUSB", "USB", },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_CWR, "btnModeCWR", "CWR", },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_NFM, "btnModeNFM", "NFM", },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGU, "btnModeDGU", "DGU", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 1, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr[i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;
			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = col1_int;
				y = row1_int + bh->h + interval;
			}
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}
		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}
}

// *********************************************************************************************************************************************************************

void buttons_bp_handler(void)
{
	window_t * win = get_win(WINDOW_BP);
	gui_t * gui = get_gui_env();
	enc2_t * encoder2 = get_enc2_env();

	if(gui->selected_type == TYPE_BUTTON)
	{
		button_t * button_high = find_gui_element_ref(TYPE_BUTTON, win, "btnAF_2");
		button_t * button_low = find_gui_element_ref(TYPE_BUTTON, win, "btnAF_1");
		button_t * button_OK = find_gui_element_ref(TYPE_BUTTON, win, "btnAF_OK");

		if (gui->selected_link->link == button_low)
		{
			button_high->is_locked = 0;
			button_low->is_locked = 1;
		}
		else if (gui->selected_link->link == button_high)
		{
			button_high->is_locked = 1;
			button_low->is_locked = 0;
		}
		else if (gui->selected_link->link == button_OK)
		{
			set_window(win, NON_VISIBLE);
			encoder2->busy = 0;
			footer_buttons_state(CANCELLED);
			hamradio_disable_keyboard_redirect();
		}
	}
}

void window_bp_process(void)
{
	static uint_fast8_t val_high, val_low, val_c, val_w;
	static uint_fast16_t x_h, x_l, x_c;
	window_t * win = get_win(WINDOW_BP);
	gui_t * gui = get_gui_env();
	uint_fast16_t x_size = 290, x_0 = 50, y_0 = 90;
	static label_t * lbl_low, * lbl_high;
	static button_t * button_high, * button_low;
	static enc2_t * encoder2;

	if (win->first_call)
	{
		uint_fast16_t id = 0, x, y, xmax = 0, ymax = 0;
		uint_fast8_t interval = 20, col1_int = 35, row1_int = window_title_height + 20;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,     state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 86, 44, buttons_bp_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP, NON_VISIBLE, UINTPTR_MAX, "btnAF_1",  "", },
			{ 0, 0, 86, 44, buttons_bp_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP, NON_VISIBLE, UINTPTR_MAX, "btnAF_OK", "OK", },
			{ 0, 0, 86, 44, buttons_bp_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP, NON_VISIBLE, UINTPTR_MAX, "btnAF_2",  "", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		label_t labels[] = {
		//    x, y,  parent, state, is_trackable, visible,  name, Text, font_size, 	color, onClickHandler
			{ },
			{ 0, 0, WINDOW_BP, DISABLED,  0, NON_VISIBLE, "lbl_low", "", FONT_LARGE, COLORMAIN_YELLOW, },
			{ 0, 0, WINDOW_BP, DISABLED,  0, NON_VISIBLE, "lbl_high", "", FONT_LARGE, COLORMAIN_YELLOW, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		memcpy(win->lh_ptr, labels, labels_size);

		button_high = find_gui_element_ref(TYPE_BUTTON, win, "btnAF_2");
		button_low = find_gui_element_ref(TYPE_BUTTON, win, "btnAF_1");

		lbl_low = find_gui_element_ref(TYPE_LABEL, win, "lbl_low");
		lbl_high = find_gui_element_ref(TYPE_LABEL, win, "lbl_high");

		lbl_low->y = y_0 + get_label_height(lbl_low);
		lbl_high->y = lbl_low->y;

		lbl_low->visible = VISIBLE;
		lbl_high->visible = VISIBLE;

		x = col1_int;
		y = lbl_high->y + get_label_height(lbl_high) * 2;
		for (uint_fast8_t id = 1; id < win->bh_count; id ++)
		{
			button_t * bh = & win->bh_ptr[id];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;
			x = x + interval + bh->w;
		}

		val_high = hamradio_get_high_bp(0);
		val_low = hamradio_get_low_bp(0);
		if (hamradio_get_bp_type())			// BWSET_WIDE
		{
			strcpy(button_high->text, "High|cut");
			strcpy(button_low->text, "Low|cut");
			button_high->is_locked = 1;
		}
		else								// BWSET_NARROW
		{
			strcpy(button_high->text, "Pitch");
			strcpy(button_low->text, "Width");
			button_low->is_locked = 1;
		}

		xmax = button_high->x1 + button_high->w;
		ymax = button_high->y1 + button_high->h;
		calculate_window_position(win, xmax, ymax);
		elements_state(win);

		encoder2 = get_enc2_env();
	}

	if (encoder2->rotate != 0 || win->first_call)
	{
		char buf[TEXT_ARRAY_SIZE];

		if (win->first_call)
			win->first_call = 0;

		if (hamradio_get_bp_type())			// BWSET_WIDE
		{
			if (button_high->is_locked == 1)
				val_high = hamradio_get_high_bp(encoder2->rotate);
			else if (button_low->is_locked == 1)
				val_low = hamradio_get_low_bp(encoder2->rotate * 10);
			encoder2->rotate_done = 1;

			x_h = x_0 + normalize(val_high, 0, 50, x_size);
			x_l = x_0 + normalize(val_low / 10, 0, 50, x_size);
			x_c = x_l + (x_h - x_l) / 2;

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), val_high * 100);
			strcpy(lbl_high->text, buf);
			lbl_high->x = (x_h + get_label_width(lbl_high) > x_0 + x_size) ?
					(x_0 + x_size - get_label_width(lbl_high)) : x_h;

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), val_low * 10);
			strcpy(lbl_low->text, buf);
			lbl_low->x = (x_l - get_label_width(lbl_low) < x_0 - 10) ? (x_0 - 10) : (x_l - get_label_width(lbl_low));
		}
		else						// BWSET_NARROW
		{
			if (button_high->is_locked == 1)
			{
				val_c = hamradio_get_high_bp(encoder2->rotate);
				val_w = hamradio_get_low_bp(0) / 2;
			}
			else if (button_low->is_locked == 1)
			{
				val_c = hamradio_get_high_bp(0);
				val_w = hamradio_get_low_bp(encoder2->rotate) / 2;
			}
			encoder2->rotate_done = 1;
			x_c = x_0 + x_size / 2;
			x_l = x_c - normalize(val_w , 0, 500, x_size);
			x_h = x_c + normalize(val_w , 0, 500, x_size);

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), val_w * 20);
			strcpy(lbl_high->text, buf);
			lbl_high->x = x_c - get_label_width(lbl_high) / 2;

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("P %d"), val_c * 10);
			strcpy(lbl_low->text, buf);
			lbl_low->x = x_0 + x_size - get_label_width(lbl_low);
		}
		gui->timer_1sec_updated = 1;
	}
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	colmain_line(fr, DIM_X, DIM_Y, win->x1 + x_0 - 10, win->y1 + y_0, win->x1 + x_0 + x_size, win->y1 + y_0, COLORMAIN_WHITE, 0);
	colmain_line(fr, DIM_X, DIM_Y, win->x1 + x_0, win->y1 + y_0 - 45, win->x1 + x_0, win->y1 + y_0 + 5, COLORMAIN_WHITE, 0);
	colmain_line(fr, DIM_X, DIM_Y, win->x1 + x_l, win->y1 + y_0 - 40, win->x1 + x_l - 4, win->y1 + y_0 - 3, COLORMAIN_YELLOW, 1);
	colmain_line(fr, DIM_X, DIM_Y, win->x1 + x_h, win->y1 + y_0 - 40, win->x1 + x_h + 4, win->y1 + y_0 - 3, COLORMAIN_YELLOW, 1);
	colmain_line(fr, DIM_X, DIM_Y, win->x1 + x_l, win->y1 + y_0 - 40, win->x1 + x_h, win->y1 + y_0 - 40, COLORMAIN_YELLOW, 0);
	colmain_line(fr, DIM_X, DIM_Y, win->x1 + x_c, win->y1 + y_0 - 45, win->x1 + x_c, win->y1 + y_0 + 5, COLORMAIN_RED, 0);
}

// *********************************************************************************************************************************************************************

void window_agc_process(void)
{
	window_t * win = get_win(WINDOW_AGC);
	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 4, id_start, id_end;
		win->first_call = 0;
		button_t * bh = NULL;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 86, 44, hamradio_set_agc_off,  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC, NON_VISIBLE, UINTPTR_MAX, "btnAGCoff",  "AGC|off", },
			{ 0, 0, 86, 44, hamradio_set_agc_slow, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC, NON_VISIBLE, UINTPTR_MAX, "btnAGCslow", "AGC|slow", },
			{ 0, 0, 86, 44, hamradio_set_agc_fast, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC, NON_VISIBLE, UINTPTR_MAX, "btnAGCfast", "AGC|fast", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 1, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr[i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = col1_int;
				y = y + bh->h + interval;
			}
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}
		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}
}

// *********************************************************************************************************************************************************************

void buttons_freq_handler (void)
{
	gui_t * gui = get_gui_env();
	button_t * bh =  gui->selected_link->link;
	if (bh->parent == WINDOW_FREQ && editfreq.key == BUTTON_CODE_DONE)
		editfreq.key = bh->payload;
}

void window_freq_process (void)
{
	static label_t * lbl_freq;
	window_t * win = get_win(WINDOW_FREQ);

	if (win->first_call)
	{
		uint_fast16_t x, y, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 4;
		win->first_call = 0;
		button_t * bh = NULL;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ, NON_VISIBLE, 1, 		 		"btnFreq1",  "1", },
			{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ, NON_VISIBLE, 2, 		 		"btnFreq2",  "2", },
			{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ, NON_VISIBLE, 3, 		 		"btnFreq3",  "3", },
			{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ, NON_VISIBLE, BUTTON_CODE_BK, 	"btnFreqBK", "<-", },
			{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ, NON_VISIBLE, 4, 	 			"btnFreq4",  "4", },
			{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ, NON_VISIBLE, 5, 				"btnFreq5",  "5", },
			{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ, NON_VISIBLE, 6, 				"btnFreq6",  "6", },
			{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ, NON_VISIBLE, BUTTON_CODE_OK, 	"btnFreqOK", "OK", },
			{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ, NON_VISIBLE, 7, 				"btnFreq7",  "7", },
			{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ, NON_VISIBLE, 8,  				"btnFreq8",  "8", },
			{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ, NON_VISIBLE, 9, 		 		"btnFreq9",  "9", },
			{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ, NON_VISIBLE, 0, 	 			"btnFreq0",  "0", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		label_t labels[] = {
		//    x, y,  parent,     		state, is_trackable, visible,   name,       		Text, font_size, 	color, 			onClickHandler
			{ },
			{ 0, 0,	WINDOW_FREQ, 			DISABLED,  0, NON_VISIBLE, "lbl_freq_val",  		"", FONT_LARGE, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		memcpy(win->lh_ptr, labels, labels_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 1, r = 1; i < win->bh_count; i ++, r ++)
		{
			bh = & win->bh_ptr[i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = col1_int;
				y = y + bh->h + interval;
			}
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btnFreqOK");
		bh->is_locked = BUTTON_LOCKED;

		lbl_freq = find_gui_element_ref(TYPE_LABEL, win, "lbl_freq_val");
		lbl_freq->x = strwidth(win->name) + strwidth(" ") + 20;
		lbl_freq->y = 5;
		strcpy(lbl_freq->text, "     0 k");
		lbl_freq->color = COLORMAIN_WHITE;
		lbl_freq->visible = VISIBLE;

		editfreq.val = 0;
		editfreq.num = 0;
		editfreq.key = BUTTON_CODE_DONE;

		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}

	if (editfreq.key != BUTTON_CODE_DONE)
	{
		if (lbl_freq->color == COLORMAIN_RED)
		{
			editfreq.val = 0;
			editfreq.num = 0;
		}

		lbl_freq->color = COLORMAIN_WHITE;
		char buf[TEXT_ARRAY_SIZE];
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
			if(hamradio_set_freq(editfreq.val * 1000))
			{
				set_window(win, NON_VISIBLE);
				footer_buttons_state(CANCELLED);
				hamradio_set_lockmode(0);
				hamradio_disable_keyboard_redirect();
			}
			else
				lbl_freq->color = COLORMAIN_RED;

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
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%6d k"), editfreq.val);
		strcpy(lbl_freq->text, buf);
	}
}

// *********************************************************************************************************************************************************************

void buttons_swrscan_process(void)
{
	window_t * win = get_win(WINDOW_SWR_SCANNER);
	gui_t * gui = get_gui_env();

	if(gui->selected_type == TYPE_BUTTON)
	{
		button_t * btn_swr_start = find_gui_element_ref(TYPE_BUTTON, win, "btn_swr_start");
		button_t * btn_swr_OK = find_gui_element_ref(TYPE_BUTTON, win, "btn_swr_OK");

		if (gui->selected_link->link == btn_swr_start && ! strcmp(btn_swr_start->text, "Start"))
		{
			swr_scan_enable = 1;
		}
		else if (gui->selected_link->link == btn_swr_start && ! strcmp(btn_swr_start->text, "Stop"))
		{
			swr_scan_stop = 1;
		}
		else if (gui->selected_link->link == btn_swr_OK)
		{
			set_window(win, NON_VISIBLE);
			footer_buttons_state(CANCELLED);
			hamradio_set_lockmode(0);
			hamradio_disable_keyboard_redirect();
			free(y_vals);
		}
	}
}

void window_swrscan_process(void)
{
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	uint_fast16_t gr_w = 500, gr_h = 250;												// размеры области графика
	uint_fast8_t interval = 20, col1_int = 20, row1_int = 30;
	uint_fast16_t x0 = col1_int + interval * 2, y0 = row1_int + gr_h - interval * 2;	// нулевые координаты графика
	uint_fast16_t x1 = x0 + gr_w - interval * 4, y1 = gr_h - y0 + interval * 2;			// размеры осей графика
	static uint_fast16_t mid_w = 0, freq_step = 0;
	static uint_fast16_t i, current_freq_x;
	static uint_fast32_t lim_bottom, lim_top, swr_freq, backup_freq;
	static label_t * lbl_swr_error;
	static button_t * btn_swr_start, * btnSWRscan, * btn_swr_OK;
	static uint_fast8_t backup_power;
	static uint_fast8_t swr_scan_done = 0, is_swr_scanning = 0;
	window_t * win = get_win(WINDOW_SWR_SCANNER);
	uint_fast8_t averageFactor = 3;

	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		win->first_call = 0;
		button_t * bh = NULL;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 86, 44, buttons_swrscan_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_SWR_SCANNER, 	NON_VISIBLE, UINTPTR_MAX,  "btn_swr_start", "Start", },
			{ 0, 0, 86, 44, buttons_swrscan_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_SWR_SCANNER, 	NON_VISIBLE, UINTPTR_MAX,  "btn_swr_OK", 	"OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		label_t labels[] = {
		//    x, y,  parent,    state, is_trackable, visible,   name,   Text, font_size, 	color, 	 onClickHandler
			{ },
			{ 0, 0,	WINDOW_SWR_SCANNER, DISABLED,  0, NON_VISIBLE, "lbl_swr_bottom", "", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_SWR_SCANNER, DISABLED,  0, NON_VISIBLE, "lbl_swr_top", 	 "", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_SWR_SCANNER, DISABLED,  0, NON_VISIBLE, "lbl_swr_error",  "", FONT_MEDIUM, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		memcpy(win->lh_ptr, labels, labels_size);

		mid_w = col1_int + gr_w / 2;
		btn_swr_start = find_gui_element_ref(TYPE_BUTTON, win, "btn_swr_start");
		btn_swr_start->x1 = mid_w - btn_swr_start->w - interval;
		btn_swr_start->y1 = row1_int + gr_h + col1_int;
		strcpy(btn_swr_start->text, "Start");
		btn_swr_start->visible = VISIBLE;

		btn_swr_OK = find_gui_element_ref(TYPE_BUTTON, win, "btn_swr_OK");
		btn_swr_OK->x1 = mid_w + interval;
		btn_swr_OK->y1 = btn_swr_start->y1;
		btn_swr_OK->visible = VISIBLE;

		lbl_swr_error = find_gui_element_ref(TYPE_LABEL, win, "lbl_swr_error");
		btnSWRscan = find_gui_element_ref(TYPE_BUTTON, get_win(WINDOW_MAIN), "btn_SWRscan");

		backup_freq = hamradio_get_freq_rx();
		if(hamradio_verify_freq_bands(backup_freq, & lim_bottom, & lim_top))
		{
			label_t * lbl_swr_bottom = find_gui_element_ref(TYPE_LABEL, win, "lbl_swr_bottom");
			local_snprintf_P(lbl_swr_bottom->text, ARRAY_SIZE(lbl_swr_bottom->text), PSTR("%dk"), lim_bottom / 1000);
			lbl_swr_bottom->x = x0 - get_label_width(lbl_swr_bottom) / 2;
			lbl_swr_bottom->y = y0 + get_label_height(lbl_swr_bottom) * 2;
			lbl_swr_bottom->visible = VISIBLE;

			label_t * lbl_swr_top = find_gui_element_ref(TYPE_LABEL, win, "lbl_swr_top");
			local_snprintf_P(lbl_swr_top->text, ARRAY_SIZE(lbl_swr_top->text), PSTR("%dk"), lim_top / 1000);
			lbl_swr_top->x = x1 - get_label_width(lbl_swr_bottom) / 2;
			lbl_swr_top->y = lbl_swr_bottom->y;
			lbl_swr_top->visible = VISIBLE;

			btn_swr_start->state = CANCELLED;
			swr_freq = lim_bottom;
			freq_step = (lim_top - lim_bottom) / (x1 - x0);
			current_freq_x = normalize(backup_freq / 1000, lim_bottom / 1000, lim_top / 1000, x1 - x0);
//				backup_power = hamradio_get_tx_power();
		}
		else
		{	// если текущая частота не входит ни в один из диапазонов, вывод сообщения об ошибке
			local_snprintf_P(lbl_swr_error->text, ARRAY_SIZE(lbl_swr_error->text), PSTR("%dk not into HAM bands"), backup_freq / 1000);
			lbl_swr_error->x = mid_w - get_label_width(lbl_swr_error) / 2;
			lbl_swr_error->y = (row1_int + gr_h) / 2;
			lbl_swr_error->visible = VISIBLE;
			btn_swr_start->state = DISABLED;
		}

		xmax = col1_int + gr_w;
		ymax = btn_swr_OK->y1 + btn_swr_OK->h;
		calculate_window_position(win, xmax, ymax);
		elements_state(win);

		i = 0;
		y_vals = calloc(x1 - x0, sizeof(uint_fast8_t));
		swr_scan_done = 0;
		is_swr_scanning = 0;
		swr_scan_stop = 0;
		return;
	}

	if (swr_scan_enable)						// нажата кнопка Start
	{
		swr_scan_enable = 0;
		strcpy(btn_swr_start->text, "Stop");
		btnSWRscan->state = DISABLED;
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
			btnSWRscan->state = CANCELLED;
			btn_swr_OK->state = CANCELLED;
			hamradio_set_tune(0);
			hamradio_set_freq(backup_freq);
//			hamradio_set_tx_power(backup_power);
		}
		y_vals[i] = normalize(get_swr(), 0, (SWRMIN * 40 / 10) - SWRMIN, y0 - y1);
		if (i)
			y_vals[i] = (y_vals[i - 1] * (averageFactor - 1) + y_vals[i]) / averageFactor;
		i++;
	}

	if (! win->first_call)
	{
		// отрисовка фона графика и разметки
		uint_fast16_t gr_x = win->x1 + x0, gr_y = win->y1 + y0;
		colpip_fillrect(fr, DIM_X, DIM_Y, win->x1 + col1_int, win->y1 + row1_int, gr_w, gr_h, COLORMAIN_BLACK);
		colmain_line(fr, DIM_X, DIM_Y, gr_x, gr_y, gr_x, win->y1 + y1, COLORMAIN_WHITE, 0);
		colmain_line(fr, DIM_X, DIM_Y, gr_x, gr_y, win->x1 + x1, gr_y, COLORMAIN_WHITE, 0);

		char buf[5];
		uint_fast8_t l = 1, row_step = round((y0 - y1) / 3);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), l++);
		colpip_string3_tbg(fr, DIM_X, DIM_Y, gr_x - SMALLCHARW3 * 2, gr_y - SMALLCHARH3 / 2, buf, COLORMAIN_WHITE);
		for(int_fast16_t yy = y0 - row_step; yy > y1; yy -= row_step)
		{
			if(yy < 0)
				break;

			colmain_line(fr, DIM_X, DIM_Y, gr_x, win->y1 + yy, win->x1 + x1, win->y1 + yy, COLORMAIN_DARKGREEN, 0);
			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), l++);
			colpip_string3_tbg(fr, DIM_X, DIM_Y, gr_x - SMALLCHARW3 * 2, win->y1 + yy - SMALLCHARH3 / 2, buf, COLORMAIN_WHITE);
		}

		if (lbl_swr_error->visible)				// фон сообщения об ошибке
		{
			colpip_fillrect(fr, DIM_X, DIM_Y, win->x1 + col1_int, win->y1 + lbl_swr_error->y - 5, gr_w, get_label_height(lbl_swr_error) + 5, COLORMAIN_RED);
		}
		else									// маркер текущей частоты
		{
			colmain_line(fr, DIM_X, DIM_Y, gr_x + current_freq_x, gr_y, gr_x + current_freq_x, win->y1 + y1, COLORMAIN_RED, 0);
		}

		if (is_swr_scanning || swr_scan_done)	// вывод графика во время сканирования и по завершении
		{
			for(uint_fast16_t j = 2; j <= i; j ++)
				colmain_line(fr, DIM_X, DIM_Y, gr_x + j - 2, gr_y - y_vals[j - 2], gr_x + j - 1, gr_y - y_vals[j - 1], COLORMAIN_YELLOW, 1);
		}
	}
}

// *********************************************************************************************************************************************************************

void buttons_tx_sett_process(void)
{
	gui_t * gui = get_gui_env();

	if(gui->selected_type == TYPE_BUTTON)
	{
		window_t * winTX = get_win(WINDOW_TX_SETTINGS);
		window_t * winPower = get_win(WINDOW_TX_POWER);
		window_t * winVOX = get_win(WINDOW_TX_VOX_SETT);
		button_t * btn_tx_vox = find_gui_element_ref(TYPE_BUTTON, winTX, "btn_tx_vox");
		button_t * btn_tx_power = find_gui_element_ref(TYPE_BUTTON, winTX, "btn_tx_power");
		button_t * btn_tx_vox_settings = find_gui_element_ref(TYPE_BUTTON, winTX, "btn_tx_vox_settings");
		if (gui->selected_link->link == btn_tx_vox)
		{
			btn_tx_vox->is_locked = hamradio_get_gvoxenable() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_tx_vox->text, ARRAY_SIZE(btn_tx_vox->text), PSTR("VOX|%s"), btn_tx_vox->is_locked ? "ON" : "OFF");
			hamradio_set_gvoxenable(btn_tx_vox->is_locked);
			btn_tx_vox_settings->state = hamradio_get_gvoxenable() ? CANCELLED : DISABLED;
		}
		else if (gui->selected_link->link == btn_tx_vox_settings)
		{
			set_window(winVOX, VISIBLE);
		}
		else if (gui->selected_link->link == btn_tx_power)
		{
			set_window(winPower, VISIBLE);
		}
	}
}

void window_tx_process(void)
{
	window_t * win = get_win(WINDOW_TX_SETTINGS);

	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 3, id_start, id_end;
		button_t * bh = NULL;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 100, 44, buttons_tx_sett_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_SETTINGS, 	NON_VISIBLE, UINTPTR_MAX, "btn_tx_vox", 	 	 "VOX|OFF", },
			{ 0, 0, 100, 44, buttons_tx_sett_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_SETTINGS, 	NON_VISIBLE, UINTPTR_MAX, "btn_tx_vox_settings", "VOX|settings", },
			{ 0, 0, 100, 44, buttons_tx_sett_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_SETTINGS, 	NON_VISIBLE, UINTPTR_MAX, "btn_tx_power", 	 	 "TX power", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 1, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr[i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = col1_int;
				y = y + bh->h + interval;
			}
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}
		calculate_window_position(win, xmax, ymax);

#if WITHVOX
		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_vox"); 						// vox on/off
		bh->is_locked = hamradio_get_gvoxenable() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("VOX|%s"), hamradio_get_gvoxenable() ? "ON" : "OFF");

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_vox_settings");				// vox settings
		bh->state = hamradio_get_gvoxenable() ? CANCELLED : DISABLED;
#else
		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_vox");						// reverb on/off disable
		bh->state = DISABLED;

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_vox_settings"); 			// reverb settings disable
		bh->state = DISABLED;
#endif /* WITHVOX */
		elements_state(win);
		return;
	}
}

// *********************************************************************************************************************************************************************

void buttons_tx_vox_process(void)
{
	gui_t * gui = get_gui_env();
	if(gui->selected_type == TYPE_BUTTON)
	{
		window_t * win = get_win(WINDOW_TX_VOX_SETT);
		button_t * btn_tx_vox_OK = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_vox_OK");
		if (gui->selected_link->link == btn_tx_vox_OK)
			set_window(win, NON_VISIBLE);
	}
}

void window_tx_vox_process(void)
{
	window_t * win = get_win(WINDOW_TX_VOX_SETT);
	gui_t * gui = get_gui_env();
	static slider_t * sl_vox_delay = NULL, * sl_vox_level = NULL, * sl_avox_level = NULL;
	static label_t * lbl_vox_delay = NULL, * lbl_vox_level = NULL, * lbl_avox_level = NULL;
	static uint_fast16_t delay_min, delay_max, level_min, level_max, alevel_min, alevel_max;
	slider_t * sl;

	if (win->first_call)
	{
		uint_fast8_t interval = 50, col1_int = 20;
		uint_fast16_t xmax = 0, ymax = 0;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0,  44, 44, buttons_tx_vox_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_VOX_SETT, NON_VISIBLE, UINTPTR_MAX,	"btn_tx_vox_OK", "OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		slider_t sliders [] = {
			{ },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, "sl_vox_delay",  CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, "sl_vox_level",  CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, "sl_avox_level", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		};
		win->sh_count = ARRAY_SIZE(sliders);
		uint_fast16_t sliders_size = sizeof(sliders);
		win->sh_ptr = malloc(sliders_size);
		memcpy(win->sh_ptr, sliders, sliders_size);

		label_t labels[] = {
		//    x, y,  parent,  state, is_trackable, visible,   name,   Text, font_size, 	color, 	onClickHandler
			{ },
			{ 0, 0,	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_vox_delay",    	 "", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_vox_level",    	 "", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_avox_level",   	 "", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_vox_delay_min",  "", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_vox_delay_max",  "", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_vox_level_min",  "", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_vox_level_max",  "", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_avox_level_min", "", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT, DISABLED,  0, NON_VISIBLE, "lbl_avox_level_max", "", FONT_SMALL, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		memcpy(win->lh_ptr, labels, labels_size);

		hamradio_get_vox_delay_limits(& delay_min, & delay_max);
		hamradio_get_vox_level_limits(& level_min, & level_max);
		hamradio_get_antivox_delay_limits(& alevel_min, & alevel_max);

		sl_vox_delay = find_gui_element_ref(TYPE_SLIDER, win, "sl_vox_delay");
		sl_vox_level = find_gui_element_ref(TYPE_SLIDER, win, "sl_vox_level");
		sl_avox_level = find_gui_element_ref(TYPE_SLIDER, win, "sl_avox_level");
		lbl_vox_delay = find_gui_element_ref(TYPE_LABEL, win, "lbl_vox_delay");
		lbl_vox_level = find_gui_element_ref(TYPE_LABEL, win, "lbl_vox_level");
		lbl_avox_level = find_gui_element_ref(TYPE_LABEL, win, "lbl_avox_level");

		ldiv_t d = ldiv(hamradio_get_vox_delay(), 100);
		lbl_vox_delay->x = col1_int;
		lbl_vox_delay->y = interval;
		lbl_vox_delay->visible = VISIBLE;
		local_snprintf_P(lbl_vox_delay->text, ARRAY_SIZE(lbl_vox_delay->text), PSTR("Delay: %d.%d"), d.quot, d.rem / 10);

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

		button_t * bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_vox_OK");
		bh->x1 = (sl_vox_delay->x + sl_vox_delay->size + col1_int * 2) / 2 - (bh->w / 2);
		bh->y1 = lbl_avox_level->y + interval;
		bh->visible = VISIBLE;

		d = ldiv(delay_min, 100);
		label_t * lbl_vox_delay_min = find_gui_element_ref(TYPE_LABEL, win, "lbl_vox_delay_min");
		local_snprintf_P(lbl_vox_delay_min->text, ARRAY_SIZE(lbl_vox_delay_min->text), PSTR("%d.%d sec"), d.quot, d.rem / 10);
		lbl_vox_delay_min->x = sl_vox_delay->x - get_label_width(lbl_vox_delay_min) / 2;
		lbl_vox_delay_min->y = sl_vox_delay->y + get_label_height(lbl_vox_delay_min) * 3;
		lbl_vox_delay_min->visible = VISIBLE;

		d = ldiv(delay_max, 100);
		label_t * lbl_vox_delay_max = find_gui_element_ref(TYPE_LABEL, win, "lbl_vox_delay_max");
		local_snprintf_P(lbl_vox_delay_max->text, ARRAY_SIZE(lbl_vox_delay_max->text), PSTR("%d.%d sec"), d.quot, d.rem / 10);
		lbl_vox_delay_max->x = sl_vox_delay->x + sl_vox_delay->size - get_label_width(lbl_vox_delay_max) / 2;
		lbl_vox_delay_max->y = sl_vox_delay->y + get_label_height(lbl_vox_delay_max) * 3;
		lbl_vox_delay_max->visible = VISIBLE;

		label_t * lbl_vox_level_min = find_gui_element_ref(TYPE_LABEL, win, "lbl_vox_level_min");
		local_snprintf_P(lbl_vox_level_min->text, ARRAY_SIZE(lbl_vox_level_min->text), PSTR("%d"), level_min);
		lbl_vox_level_min->x = sl_vox_level->x - get_label_width(lbl_vox_level_min) / 2;
		lbl_vox_level_min->y = sl_vox_level->y + get_label_height(lbl_vox_level_min) * 3;
		lbl_vox_level_min->visible = VISIBLE;

		label_t * lbl_vox_level_max = find_gui_element_ref(TYPE_LABEL, win, "lbl_vox_level_max");
		local_snprintf_P(lbl_vox_level_max->text, ARRAY_SIZE(lbl_vox_level_max->text), PSTR("%d"), level_max);
		lbl_vox_level_max->x = sl_vox_level->x + sl_vox_level->size - get_label_width(lbl_vox_level_max) / 2;
		lbl_vox_level_max->y = sl_vox_level->y + get_label_height(lbl_vox_level_max) * 3;
		lbl_vox_level_max->visible = VISIBLE;

		label_t * lbl_avox_level_min = find_gui_element_ref(TYPE_LABEL, win, "lbl_avox_level_min");
		local_snprintf_P(lbl_avox_level_min->text, ARRAY_SIZE(lbl_avox_level_min->text), PSTR("%d"), alevel_min);
		lbl_avox_level_min->x = sl_avox_level->x - get_label_width(lbl_avox_level_min) / 2;
		lbl_avox_level_min->y = sl_avox_level->y + get_label_height(lbl_avox_level_min) * 3;
		lbl_avox_level_min->visible = VISIBLE;

		label_t * lbl_avox_level_max = find_gui_element_ref(TYPE_LABEL, win, "lbl_avox_level_max");
		local_snprintf_P(lbl_avox_level_max->text, ARRAY_SIZE(lbl_avox_level_max->text), PSTR("%d"), alevel_max);
		lbl_avox_level_max->x = sl_avox_level->x + sl_vox_level->size - get_label_width(lbl_avox_level_max) / 2;
		lbl_avox_level_max->y = sl_avox_level->y + get_label_height(lbl_avox_level_max) * 3;
		lbl_avox_level_max->visible = VISIBLE;

		xmax = sl_avox_level->x + sl_avox_level->size + col1_int;
		ymax = bh->y1 + bh->h;
		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}

	if (gui->selected_type == TYPE_SLIDER && gui->is_tracking)
	{
		char buf[TEXT_ARRAY_SIZE];

		/* костыль через костыль */
		sl = (slider_t *) gui->selected_link->link;

		if(sl == sl_vox_delay)
		{
			uint_fast16_t delay = delay_min + normalize(sl->value, 0, 100, delay_max - delay_min);
			ldiv_t d = ldiv(delay, 100);
			local_snprintf_P(lbl_vox_delay->text, ARRAY_SIZE(lbl_vox_delay->text), PSTR("Delay: %d.%d"), d.quot, d.rem / 10);
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
}

// *********************************************************************************************************************************************************************

void buttons_tx_power_process(void)
{
	gui_t * gui = get_gui_env();

	if(gui->selected_type == TYPE_BUTTON)
	{
		window_t * win = get_win(WINDOW_TX_POWER);
		button_t * btn_tx_pwr_OK = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_pwr_OK");
		if (gui->selected_link->link == btn_tx_pwr_OK)
			set_window(win, NON_VISIBLE);
	}
}

void window_tx_power_process(void)
{
	window_t * win = get_win(WINDOW_TX_POWER);
	gui_t * gui = get_gui_env();
	static slider_t * sl_pwr_level = NULL, * sl_pwr_tuner_level = NULL;
	static label_t * lbl_tx_power = NULL, * lbl_tune_power = NULL;
	static uint_fast16_t power_min, power_max;
	slider_t * sl;

	if (win->first_call)
	{
		uint_fast8_t interval = 50, col1_int = 20;
		uint_fast16_t xmax = 0, ymax = 0;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0,  44, 44, buttons_tx_power_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_POWER,  NON_VISIBLE, UINTPTR_MAX, "btn_tx_pwr_OK", "OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		slider_t sliders [] = {
			{ },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_POWER, "sl_pwr_level",   	   CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_POWER, "sl_pwr_tuner_level", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		};
		win->sh_count = ARRAY_SIZE(sliders);
		uint_fast16_t sliders_size = sizeof(sliders);
		win->sh_ptr = malloc(sliders_size);
		memcpy(win->sh_ptr, sliders, sliders_size);

		label_t labels[] = {
		//    x, y,  parent,  state, is_trackable, visible,   name,    Text, font_size, 	color, 		onClickHandler
			{ },
			{ 0, 0,	WINDOW_TX_POWER, DISABLED,  0, NON_VISIBLE, "lbl_tx_power",   "", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_POWER, DISABLED,  0, NON_VISIBLE, "lbl_tune_power", "", FONT_MEDIUM, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		memcpy(win->lh_ptr, labels, labels_size);

		sl_pwr_level = find_gui_element_ref(TYPE_SLIDER, win, "sl_pwr_level");
		sl_pwr_tuner_level = find_gui_element_ref(TYPE_SLIDER, win, "sl_pwr_tuner_level");

		lbl_tx_power = find_gui_element_ref(TYPE_LABEL, win, "lbl_tx_power");
		lbl_tune_power = find_gui_element_ref(TYPE_LABEL, win, "lbl_tune_power");

		hamradio_get_tx_power_limits(& power_min, & power_max);
		uint_fast8_t power = hamradio_get_tx_power();
		uint_fast8_t tune_power = hamradio_get_tx_tune_power();

		lbl_tx_power->x = col1_int;
		lbl_tx_power->y = interval;
		lbl_tx_power->visible = VISIBLE;
		local_snprintf_P(lbl_tx_power->text, ARRAY_SIZE(lbl_tx_power->text), PSTR("TX power  : %3d"), power);

		lbl_tune_power->x = lbl_tx_power->x;
		lbl_tune_power->y = lbl_tx_power->y + interval;
		lbl_tune_power->visible = VISIBLE;
		local_snprintf_P(lbl_tune_power->text, ARRAY_SIZE(lbl_tune_power->text), PSTR("Tune power: %3d"), tune_power);

		sl_pwr_level->x = lbl_tx_power->x + interval * 3 + interval / 2;
		sl_pwr_level->y = lbl_tx_power->y;
		sl_pwr_level->visible = VISIBLE;
		sl_pwr_level->size = 300;
		sl_pwr_level->step = 3;
		sl_pwr_level->value = normalize(power, power_min, power_max, 100);

		sl_pwr_tuner_level->x = sl_pwr_level->x;
		sl_pwr_tuner_level->y = lbl_tune_power->y;
		sl_pwr_tuner_level->visible = VISIBLE;
		sl_pwr_tuner_level->size = 300;
		sl_pwr_tuner_level->step = 3;
		sl_pwr_tuner_level->value = normalize(tune_power, power_min, power_max, 100);

		button_t * bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_pwr_OK");
		bh->x1 = (sl_pwr_level->x + sl_pwr_level->size + col1_int * 2) / 2 - (bh->w / 2);
		bh->y1 = lbl_tune_power->y + interval;
		bh->visible = VISIBLE;

		xmax = sl_pwr_tuner_level->x + sl_pwr_tuner_level->size + col1_int;
		ymax = bh->y1 + bh->h;
		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}

	if (gui->selected_type == TYPE_SLIDER && gui->is_tracking)
	{
		char buf[TEXT_ARRAY_SIZE];

		/* костыль через костыль */
		sl = (slider_t *) gui->selected_link->link;

		if(sl == sl_pwr_level)
		{
			uint_fast8_t power = power_min + normalize(sl->value, 0, 100, power_max - power_min);
			local_snprintf_P(lbl_tx_power->text, ARRAY_SIZE(lbl_tx_power->text), PSTR("TX power  : %3d"),power);
			hamradio_set_tx_power(power);
		}
		else if (sl == sl_pwr_tuner_level)
		{
			uint_fast8_t power = power_min + normalize(sl->value, 0, 100, power_max - power_min);
			local_snprintf_P(lbl_tune_power->text, ARRAY_SIZE(lbl_tune_power->text), PSTR("Tune power: %3d"),power);
			hamradio_set_tx_tune_power(power);
		}
	}
}

// *********************************************************************************************************************************************************************

void buttons_audiosettings_process(void)
{
	gui_t * gui = get_gui_env();
	if(gui->selected_type == TYPE_BUTTON)
	{
		window_t * winAP = get_win(WINDOW_AUDIOSETTINGS);
		window_t * winEQ = get_win(WINDOW_AP_MIC_EQ);
		window_t * winRS = get_win(WINDOW_AP_REVERB_SETT);
		window_t * winMIC = get_win(WINDOW_AP_MIC_SETT);
		window_t * winMICpr = get_win(WINDOW_AP_MIC_PROF);
		button_t * btn_reverb = find_gui_element_ref(TYPE_BUTTON, winAP, "btn_reverb");						// reverb on/off
		button_t * btn_reverb_settings = find_gui_element_ref(TYPE_BUTTON, winAP, "btn_reverb_settings");	// reverb settings
		button_t * btn_monitor = find_gui_element_ref(TYPE_BUTTON, winAP, "btn_monitor");					// monitor on/off
		button_t * btn_mic_eq = find_gui_element_ref(TYPE_BUTTON, winAP, "btn_mic_eq");						// MIC EQ on/off
		button_t * btn_mic_eq_settings = find_gui_element_ref(TYPE_BUTTON, winAP, "btn_mic_eq_settings");	// MIC EQ settingss
		button_t * btn_mic_settings = find_gui_element_ref(TYPE_BUTTON, winAP, "btn_mic_settings");			// mic settings
		button_t * btn_mic_profiles = find_gui_element_ref(TYPE_BUTTON, winAP, "btn_mic_profiles");			// mic profiles

#if WITHREVERB
		if (gui->selected_link->link == btn_reverb)
		{
			btn_reverb->is_locked = hamradio_get_greverb() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_reverb->text, ARRAY_SIZE(btn_reverb->text), PSTR("Reverb|%s"), btn_reverb->is_locked ? "ON" : "OFF");
			hamradio_set_greverb(btn_reverb->is_locked);
			btn_reverb_settings->state = btn_reverb->is_locked ? CANCELLED : DISABLED;

		}
		else if (gui->selected_link->link == btn_reverb_settings)
		{
			set_window(winRS, VISIBLE);
		}

		else
#endif /* WITHREVERB */
		if (gui->selected_link->link == btn_monitor)
		{
			btn_monitor->is_locked = hamradio_get_gmoniflag() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_monitor->text, ARRAY_SIZE(btn_monitor->text), PSTR("Monitor|%s"), btn_monitor->is_locked ? "enabled" : "disabled");
			hamradio_set_gmoniflag(btn_monitor->is_locked);
		}
		else if (gui->selected_link->link == btn_mic_eq)
		{
			btn_mic_eq->is_locked = hamradio_get_gmikeequalizer() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_mic_eq->text, ARRAY_SIZE(btn_mic_eq->text), PSTR("MIC EQ|%s"), btn_mic_eq->is_locked ? "ON" : "OFF");
			hamradio_set_gmikeequalizer(btn_mic_eq->is_locked);
			btn_mic_eq_settings->state = btn_mic_eq->is_locked ? CANCELLED : DISABLED;
		}
		else if (gui->selected_link->link == btn_mic_eq_settings)
		{
			set_window(winEQ, VISIBLE);
		}
		else if (gui->selected_link->link == btn_mic_settings)
		{
			set_window(winMIC, VISIBLE);
		}
		else if (gui->selected_link->link == btn_mic_profiles)
		{
			set_window(winMICpr, VISIBLE);
		}
	}
}

void window_audiosettings_process(void)
{
	window_t * win = get_win(WINDOW_AUDIOSETTINGS);

	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 3;
		button_t * bh = NULL;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 100, 44, buttons_audiosettings_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, UINTPTR_MAX, "btn_reverb", 			"Reverb|OFF", },
			{ 0, 0, 100, 44, buttons_audiosettings_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, UINTPTR_MAX, "btn_reverb_settings", 	"Reverb|settings", },
			{ 0, 0, 100, 44, buttons_audiosettings_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, UINTPTR_MAX, "btn_monitor", 			"Monitor|disabled", },
			{ 0, 0, 100, 44, buttons_audiosettings_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, UINTPTR_MAX, "btn_mic_eq", 			"MIC EQ|OFF", },
			{ 0, 0, 100, 44, buttons_audiosettings_process,	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, UINTPTR_MAX, "btn_mic_eq_settings", 	"MIC EQ|settings", },
			{ 0, 0, 100, 44, buttons_audiosettings_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, UINTPTR_MAX, "btn_mic_settings", 		"MIC|settings", },
			{ 0, 0, 100, 44, buttons_audiosettings_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, UINTPTR_MAX, "btn_mic_profiles", 		"MIC|profiles", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 1, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr[i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = col1_int;
				y = y + bh->h + interval;
			}
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}
		calculate_window_position(win, xmax, ymax);

#if WITHREVERB
		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_reverb"); 						// reverb on/off
		bh->is_locked = hamradio_get_greverb() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Reverb|%s"), hamradio_get_greverb() ? "ON" : "OFF");

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_reverb_settings");				// reverb settings
		bh->state = hamradio_get_greverb() ? CANCELLED : DISABLED;
#else
		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_reverb");						// reverb on/off disable
		bh->state = DISABLED;

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_reverb_settings"); 			// reverb settings disable
		bh->state = DISABLED;
#endif /* WITHREVERB */

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_monitor");						// monitor on/off
		bh->is_locked = hamradio_get_gmoniflag() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Monitor|%s"), bh->is_locked ? "enabled" : "disabled");

#if WITHAFCODEC1HAVEPROC
		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_mic_eq");						// MIC EQ on/off
		bh->is_locked = hamradio_get_gmikeequalizer() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("MIC EQ|%s"), bh->is_locked ? "ON" : "OFF");

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_mic_eq_settings");				// MIC EQ settings
		bh->state = hamradio_get_gmikeequalizer() ? CANCELLED : DISABLED;
#else
		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_mic_eq");						// MIC EQ on/off disable
		bh->state = DISABLED;

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_mic_eq_settings"); 			// MIC EQ settings disable
		bh->state = DISABLED;
#endif /* WITHAFCODEC1HAVEPROC */
		elements_state(win);
		return;
	}
}

// *********************************************************************************************************************************************************************

// *********************************************************************************************************************************************************************

// *********************************************************************************************************************************************************************

// *********************************************************************************************************************************************************************

// *********************************************************************************************************************************************************************

// *********************************************************************************************************************************************************************

// *********************************************************************************************************************************************************************

// *********************************************************************************************************************************************************************
#endif /* WITHTOUCHGUI */
