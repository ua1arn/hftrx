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

#include "keyboard.h"
#include "src/codecs/nau8822.h"

#if WITHTOUCHGUI

#include "src/gui/gui.h"
#include "src/gui/gui_user.h"
#include "src/gui/gui_system.h"
#include "src/gui/gui_structs.h"
#include "src/gui/gui_settings.h"

static void gui_main_process(void);
static void window_mode_process(void);
static void window_af_process(void);
static void window_agc_process(void);
static void window_freq_process (void);
static void window_swrscan_process(void);
static void window_tx_process(void);
static void window_tx_vox_process(void);
static void window_tx_power_process(void);
static void window_audiosettings_process(void);
static void window_ap_reverb_process(void);
static void window_ap_mic_eq_process(void);
static void window_ap_mic_process(void);
static void window_ap_mic_prof_process(void);
static void window_menu_process(void);
static void window_uif_process(void);
static void window_enc2_process(void);
static void window_options_process(void);
static void window_utilites_process(void);
static void window_bands_process(void);
static void window_memory_process(void);

static window_t windows [] = {
//     window_id,   		 parent_id, 			align_mode,     x1, y1, w, h,   title,     		is_show, first_call, is_close, onVisibleProcess
	{ WINDOW_MAIN, 			 UINT8_MAX, 			ALIGN_LEFT_X,	0, 0, 0, 0, "",  	   	   			 NON_VISIBLE, 0, 0, gui_main_process, },
	{ WINDOW_MODES, 		 UINT8_MAX, 			ALIGN_CENTER_X, 0, 0, 0, 0, "Select mode", 			 NON_VISIBLE, 0, 1, window_mode_process, },
	{ WINDOW_BP,    		 UINT8_MAX,				ALIGN_CENTER_X, 0, 0, 0, 0, "AF settings",    		 NON_VISIBLE, 0, 1, window_af_process, },
	{ WINDOW_AGC,   		 WINDOW_OPTIONS,		ALIGN_CENTER_X, 0, 0, 0, 0, "AGC control", 			 NON_VISIBLE, 0, 1, window_agc_process, },
	{ WINDOW_FREQ,  		 WINDOW_OPTIONS,		ALIGN_CENTER_X, 0, 0, 0, 0, "Freq:", 	   			 NON_VISIBLE, 0, 1, window_freq_process, },
	{ WINDOW_MENU,  		 WINDOW_OPTIONS,		ALIGN_CENTER_X, 0, 0, 0, 0, "Settings",	   		 	 NON_VISIBLE, 0, 1, window_menu_process, },
	{ WINDOW_ENC2, 			 UINT8_MAX, 			ALIGN_RIGHT_X, 	0, 0, 0, 0, "",  			 		 NON_VISIBLE, 0, 0, window_enc2_process, },
	{ WINDOW_UIF, 			 UINT8_MAX, 			ALIGN_LEFT_X, 	0, 0, 0, 0, "",   		   	 		 NON_VISIBLE, 0, 0, window_uif_process, },
	{ WINDOW_SWR_SCANNER,	 WINDOW_UTILS, 			ALIGN_CENTER_X, 0, 0, 0, 0, "SWR band scanner",		 NON_VISIBLE, 0, 0, window_swrscan_process, },
	{ WINDOW_AUDIOSETTINGS,  WINDOW_OPTIONS,		ALIGN_CENTER_X, 0, 0, 0, 0, "Audio settings", 		 NON_VISIBLE, 0, 1, window_audiosettings_process, },
	{ WINDOW_AP_MIC_EQ, 	 WINDOW_AUDIOSETTINGS, 	ALIGN_CENTER_X, 0, 0, 0, 0, "MIC TX equalizer",		 NON_VISIBLE, 0, 1, window_ap_mic_eq_process, },
	{ WINDOW_AP_REVERB_SETT, WINDOW_AUDIOSETTINGS, 	ALIGN_CENTER_X, 0, 0, 0, 0, "Reverberator settings", NON_VISIBLE, 0, 1, window_ap_reverb_process, },
	{ WINDOW_AP_MIC_SETT, 	 WINDOW_AUDIOSETTINGS, 	ALIGN_CENTER_X, 0, 0, 0, 0, "Microphone settings", 	 NON_VISIBLE, 0, 1, window_ap_mic_process, },
	{ WINDOW_AP_MIC_PROF, 	 WINDOW_AUDIOSETTINGS, 	ALIGN_CENTER_X, 0, 0, 0, 0, "Microphone profiles", 	 NON_VISIBLE, 0, 1, window_ap_mic_prof_process, },
	{ WINDOW_TX_SETTINGS, 	 WINDOW_OPTIONS, 		ALIGN_CENTER_X, 0, 0, 0, 0, "Transmit settings", 	 NON_VISIBLE, 0, 1, window_tx_process, },
	{ WINDOW_TX_VOX_SETT, 	 WINDOW_TX_SETTINGS, 	ALIGN_CENTER_X, 0, 0, 0, 0, "VOX settings", 	 	 NON_VISIBLE, 0, 1, window_tx_vox_process, },
	{ WINDOW_TX_POWER, 		 WINDOW_TX_SETTINGS, 	ALIGN_CENTER_X, 0, 0, 0, 0, "TX power", 	 	 	 NON_VISIBLE, 0, 1, window_tx_power_process, },
	{ WINDOW_OPTIONS, 		 UINT8_MAX, 			ALIGN_CENTER_X,	0, 0, 0, 0, "Options",  	   	   	 NON_VISIBLE, 0, 1, window_options_process, },
	{ WINDOW_UTILS, 		 WINDOW_OPTIONS,		ALIGN_CENTER_X,	0, 0, 0, 0, "Utilites",  	   	   	 NON_VISIBLE, 0, 1, window_utilites_process, },
	{ WINDOW_BANDS, 		 UINT8_MAX,				ALIGN_CENTER_X,	0, 0, 0, 0, "Bands",  	   	   	 	 NON_VISIBLE, 0, 1, window_bands_process, },
	{ WINDOW_MEMORY, 		 UINT8_MAX,				ALIGN_CENTER_X,	0, 0, 0, 0, "Memory",  	   	   	 	 NON_VISIBLE, 0, 1, window_memory_process, },
};

static uint_fast8_t swr_scan_enable = 0;		// флаг разрешения сканирования КСВ
static uint_fast8_t swr_scan_stop = 0;			// флаг нажатия кнопки Stop во время сканирования
static uint_fast8_t * y_vals;					// массив КСВ в виде отсчетов по оси Y графика

static editfreq_t editfreq;
static enc2_menu_t * gui_enc2_menu;
static enc2_t encoder2 = { 0, 0, 0, 1, 1, };
static menu_by_name_t menu_uif;

static menu_t menu [MENU_COUNT];
static uint_fast8_t menu_label_touched = 0;
static uint_fast8_t menu_level;

static band_array_t bands [30];

static bp_var_t bp_t;

static float32_t updated_spectre [FIRBUFSIZE];
float32_t fftbuf [FIRBUFSIZE * 2];
static uint_fast8_t is_sp_ready = 0;

/* Возврат ссылки на окно */
window_t * get_win(window_id_t window_id)
{
	ASSERT(window_id < WINDOWS_COUNT);
	return & windows [window_id];
}

void gui_copy_audio_buf(float32_t * buf)
{
	if (! is_sp_ready)
	{
		arm_copy_f32(buf, updated_spectre, FIRBUFSIZE);
		is_sp_ready = 1;
	}
}

// *********************************************************************************************************************************************************************

static void btn_main_handler(void)
{
	if (is_short_pressed())
	{
		window_t * winMain = get_win(WINDOW_MAIN);
		button_t * btn_Bands = find_gui_element(TYPE_BUTTON, winMain, "btn_Bands");
		button_t * btn_Memory = find_gui_element(TYPE_BUTTON, winMain, "btn_Memory");
		button_t * btn_Mode = find_gui_element(TYPE_BUTTON, winMain, "btn_Mode");
		button_t * btn_Options = find_gui_element(TYPE_BUTTON, winMain, "btn_Options");
		button_t * btn_AF = find_gui_element(TYPE_BUTTON, winMain, "btn_AF");
		button_t * btn_AutoNotch = find_gui_element(TYPE_BUTTON, winMain, "btn_AutoNotch");

		if (get_selected_element() == btn_AutoNotch)
		{
			btn_AutoNotch->payload = ! hamradio_get_autonotch();
			local_snprintf_P(btn_AutoNotch->text, ARRAY_SIZE(btn_AutoNotch->text), PSTR("ANotch|%s"), btn_AutoNotch->payload ? "ON" : "OFF");
			hamradio_set_autonotch(btn_AutoNotch->payload);
		}
		else if (get_selected_element() == btn_AF)
		{
			window_t * win = get_win(WINDOW_BP);
			if (win->state == NON_VISIBLE)
			{
				open_window(win);
				footer_buttons_state(DISABLED, btn_AF);
				hamradio_enable_keyboard_redirect();
			}
			else
			{
				close_window(OPEN_PARENT_WINDOW);
				footer_buttons_state(CANCELLED);
				hamradio_disable_keyboard_redirect();
			}
		}
		else if (get_selected_element() == btn_Mode)
		{
			window_t * win = get_win(WINDOW_MODES);
			if (win->state == NON_VISIBLE)
			{
				open_window(win);
				footer_buttons_state(DISABLED, btn_Mode);
			}
			else
			{
				close_window(OPEN_PARENT_WINDOW);
				footer_buttons_state(CANCELLED);
			}
		}
		else if (get_selected_element() == btn_Bands)
		{
			window_t * win = get_win(WINDOW_BANDS);
			if (win->state == NON_VISIBLE)
			{
				open_window(win);
				footer_buttons_state(DISABLED, btn_Bands);
			}
			else
			{
				close_window(OPEN_PARENT_WINDOW);
				footer_buttons_state(CANCELLED);
			}
		}
		else if (get_selected_element() == btn_Memory)
		{
			window_t * win = get_win(WINDOW_MEMORY);
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
		else if (get_selected_element() == btn_Options)
		{
			if (check_for_parent_window() != UINT8_MAX)
			{
				close_window(OPEN_PARENT_WINDOW);
				footer_buttons_state(CANCELLED);
				hamradio_set_lockmode(0);
				hamradio_disable_keyboard_redirect();
			}
			else
			{
				window_t * win = get_win(WINDOW_OPTIONS);
				open_window(win);
				footer_buttons_state(DISABLED, btn_Options);
			}

		}
	}
}

static void gui_main_process(void)
{
	window_t * win = get_win(WINDOW_MAIN);

	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	char buf [TEXT_ARRAY_SIZE];
	const uint_fast8_t buflen = ARRAY_SIZE(buf);
	uint_fast16_t yt, xt, y1 = 125, y2 = 145, current_place = 0, xx;
	const uint_fast8_t num_places = 7;
	const uint_fast8_t lbl_place_width = 100;
	const uint_fast16_t x_width = lbl_place_width * 2 - 6;

	if (win->first_call)
	{
		uint_fast8_t interval_btn = 3;
		uint_fast16_t x = 0;
		win->first_call = 0;
		gui_enc2_menu->updated = 1;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_Bands", 	 "Bands", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_Memory",  "Memory", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_Mode", 	 "Mode", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_AF",  	 "AF|filter", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_AutoNotch", "", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_6", 	 	 "", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_7", 	 	 "", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_8", 	 	 "", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_Options", "Options", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		for (uint_fast8_t id = 0; id < win->bh_count; id ++)
		{
			button_t * bh = & win->bh_ptr [id];
			bh->x1 = x;
			bh->y1 = WITHGUIMAXY - bh->h;
			bh->visible = VISIBLE;
			x = x + interval_btn + bh->w;
		}

		elements_state(win);

		button_t * btn_AutoNotch = find_gui_element(TYPE_BUTTON, win, "btn_AutoNotch");
		btn_AutoNotch->payload = hamradio_get_autonotch();
		local_snprintf_P(btn_AutoNotch->text, ARRAY_SIZE(btn_AutoNotch->text), PSTR("ANotch|%s"), btn_AutoNotch->payload ? "ON" : "OFF");

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
	if (get_gui_1sec_timer())
		board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);
	local_snprintf_P(buf, buflen, PSTR("%02d.%02d"), day, month);
	xx = current_place * lbl_place_width + lbl_place_width / 2;
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y1, buf, COLORMAIN_WHITE);
	local_snprintf_P(buf, buflen, PSTR("%02d%c%02d"), hour, ((secounds & 1) ? ' ' : ':'), minute);
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y2, buf, COLORMAIN_WHITE);
#endif 	/* defined (RTC1_TYPE) */

	current_place ++;

	// напряжение питания
#if WITHVOLTLEVEL
	static ldiv_t v;
	if (get_gui_1sec_timer())
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
		if (get_gui_1sec_timer())
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

	current_place ++;

	// ширина панорамы
#if WITHIF4DSP
	static int_fast32_t z;
	if (get_gui_1sec_timer())
		z = display_zoomedbw() / 1000;
	local_snprintf_P(buf, buflen, PSTR("SPAN"));
	xx = current_place * lbl_place_width + lbl_place_width / 2;
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y1, buf, COLORMAIN_WHITE);
	local_snprintf_P(buf, buflen, PSTR("%dk"), z);
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y2, buf, COLORMAIN_WHITE);
#endif /* WITHIF4DSP */

	current_place ++;

	// значение сдвига частоты
	static int_fast16_t if_shift;
	if (get_gui_1sec_timer())
		if_shift = hamradio_if_shift(0);
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

	current_place ++;

	// пусто

	current_place ++;

	// параметры полосы пропускания фильтра
	static uint_fast8_t bp_type, bp_low, bp_high;
	if (get_gui_1sec_timer())
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

	current_place ++;

	// отображение НЧ спектра
	xx = current_place * lbl_place_width + 3;

	if (is_sp_ready)
	{
		float32_t max_val = 0;
		static uint_fast8_t y_old_array [FIRBUFSIZE];
		const uint_fast16_t visiblefftsize = 95;
		uint_fast16_t fft_step = x_width / visiblefftsize;

		if (! hamradio_get_tx())
		{
			is_sp_ready = 0;
			fftzoom_x2(updated_spectre);

			for (uint_fast16_t i = 0; i < FIRBUFSIZE; i ++)
			{
				fftbuf [i * 2 + 0] = updated_spectre [i];
				fftbuf [i * 2 + 1] = 0;
			}

			apply_window_function(fftbuf, FIRBUFSIZE);
			arm_cfft_f32(FFTCONFIGSpectrum, fftbuf, 0, 1);
			arm_cmplx_mag_f32(fftbuf, fftbuf, FIRBUFSIZE);
			arm_max_no_idx_f32(fftbuf, FIRBUFSIZE, & max_val);
		}

		for (uint_fast16_t i = 3; i < x_width; i ++)
		{
			uint_fast16_t fftpos = FIRBUFSIZE - round(i / fft_step);
			const FLOAT_t val = normalize(fftbuf [fftpos], 0, max_val, 38);
			const FLOAT_t yy = y_old_array [i] * 0.8 + 0.2 * val;
			y_old_array [i] = yy;
			colmain_line(fr, DIM_X, DIM_Y, xx + i - 3, y2 + SMALLCHARH2 - yy, xx + i - 3, y2 + SMALLCHARH2, COLORMAIN_YELLOW, 0);
		}
	}

//	#if WITHTHERMOLEVEL	// температура выходных транзисторов (при передаче)
//		static ldiv_t t;
//		if (hamradio_get_tx())// && get_gui_1sec_timer())
//		{
//			t = ldiv(hamradio_get_temperature_value(), 10);
//			local_snprintf_P(buf, buflen, PSTR("%d.%dC "), t.quot, t.rem);
//			PRINTF("%s\n", buf);		// пока вывод в консоль
//		}
//	#endif /* WITHTHERMOLEVEL */

}

// *********************************************************************************************************************************************************************

static void buttons_memory_handler(void)
{

	button_t * btn_cell = (button_t *) get_selected_element();
	uint_fast8_t cell_id = get_selected_element_pos();

	if (is_short_pressed())
	{
		if (btn_cell->payload)
		{
			hamradio_load_memory_cells(cell_id, 1);
		}
	}

	if (is_long_pressed())
	{
		if (btn_cell->payload)
		{
			btn_cell->payload = 0;
			hamradio_clean_memory_cells(cell_id);
			local_snprintf_P(btn_cell->text, ARRAY_SIZE(btn_cell->text), PSTR("---"));
		}
		else
		{
			btn_cell->payload = 1;
			uint_fast32_t freq = hamradio_get_freq_rx();
			local_snprintf_P(btn_cell->text, ARRAY_SIZE(btn_cell->text), PSTR("%dk"), freq / 1000);
			hamradio_save_memory_cells(cell_id);
		}
	}
}

static void window_memory_process(void)
{
	window_t * win = get_win(WINDOW_MEMORY);
	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 5;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,     	 	payload,	 name, 		text
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell0", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell1", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell2", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell3", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell4", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell5", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell6", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell7", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell8", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell9", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell10", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell11", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell12", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell13", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell14", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell15", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell16", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell17", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell18", "---", },
			{ 0, 0, 100, 44, buttons_memory_handler, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_MEMORY, NON_VISIBLE, INT32_MAX, "btn_cell19", "---", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;
			bh->payload = 0;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = col1_int;
				y = y + bh->h + interval;
			}

			uint_fast32_t freq = hamradio_load_memory_cells(i, 0);
			if (freq > 0)
			{
				local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("%dk"), freq / 1000);
				bh->payload = 1;
			}

			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}
		elements_state(win);
		calculate_window_position(win, xmax, ymax);
		return;
	}
}

// *********************************************************************************************************************************************************************

static void buttons_bands_handler(void)
{
	if (is_short_pressed())
	{
		button_t * bh = (button_t *) get_selected_element();
		hamradio_goto_band_by_freq(bh->payload);
		close_all_windows();
	}
}

static void window_bands_process(void)
{
	window_t * win = get_win(WINDOW_BANDS);

	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 3, i = 0;
		button_t * bh = NULL;
		label_t * lh1 = NULL, * lh2 = NULL;
		win->first_call = 0;

		uint_fast8_t bands_count = hamradio_get_bands(bands);

		label_t labels [] = {
		//    x, y,  parent, state, is_trackable, visible,  	name, 		Text, 				font_size, 	color, onClickHandler
			{ 0, 0, WINDOW_BANDS, DISABLED,  0, NON_VISIBLE, "lbl_ham",   "HAM bands",		 FONT_LARGE, COLORMAIN_WHITE, },
			{ 0, 0, WINDOW_BANDS, DISABLED,  0, NON_VISIBLE, "lbl_bcast", "Broadcast bands", FONT_LARGE, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		ASSERT(win->lh_ptr != NULL);
		memcpy(win->lh_ptr, labels, labels_size);

		win->bh_count = bands_count;
		uint_fast16_t buttons_size = win->bh_count * sizeof (button_t);
		win->bh_ptr = calloc(win->bh_count, sizeof (button_t));
		ASSERT(win->bh_ptr != NULL);

		lh1 = find_gui_element(TYPE_LABEL, win, "lbl_ham");
		lh1->x = col1_int;
		lh1->y = row1_int;
		lh1->visible = VISIBLE;

		x = col1_int;
		y = lh1->y + get_label_height(lh1) * 2;

		for (uint_fast8_t r = 1; i < win->bh_count; i ++, r ++)
		{
			if (bands [i].type != BAND_TYPE_HAM)
				break;

			bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			bh->w = 86;
			bh->h = 44;
			bh->onClickHandler = buttons_bands_handler;
			bh->state = CANCELLED;
			bh->parent = WINDOW_BANDS;
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
				x = col1_int;
				y = y + bh->h + interval;
			}
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}

		lh2 = find_gui_element(TYPE_LABEL, win, "lbl_bcast");
		lh2->x = bh->x1 + bh->w + 50;
		lh2->y = row1_int;
		lh2->visible = VISIBLE;

		x = lh2->x;
		y = lh1->y + get_label_height(lh1) * 2;

		for (uint_fast8_t r = 1; i < win->bh_count; i ++, r ++)
		{
			bh = & win->bh_ptr [i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			bh->w = 86;
			bh->h = 44;
			bh->onClickHandler = buttons_bands_handler;
			bh->state = CANCELLED;
			bh->parent = WINDOW_BANDS;
			bh->payload = bands [i].init_freq;
			local_snprintf_P(bh->name, ARRAY_SIZE(bh->name), PSTR("btn_bcast_%d"), i);
			strcpy(bh->text, bands [i].name);

			if (hamradio_check_current_freq_by_band(bands [i].index))
				bh->is_locked = BUTTON_LOCKED;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = lh2->x;
				y = y + bh->h + interval;
			}
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}

		elements_state(win);
		calculate_window_position(win, xmax, ymax);
		return;
	}
}

// *********************************************************************************************************************************************************************

static void buttons_options_handler(void)
{
	if (is_short_pressed())
	{
		window_t * win = get_win(WINDOW_OPTIONS);
		button_t * btn_AGC = find_gui_element(TYPE_BUTTON, win, "btn_AGC");
		button_t * btn_Freq = find_gui_element(TYPE_BUTTON, win, "btn_Freq");
		button_t * btn_TXsett = find_gui_element(TYPE_BUTTON, win, "btn_TXsett");
		button_t * btn_AUDsett = find_gui_element(TYPE_BUTTON, win, "btn_AUDsett");
		button_t * btn_SysMenu = find_gui_element(TYPE_BUTTON, win, "btn_SysMenu");
		button_t * btn_Utils = find_gui_element(TYPE_BUTTON, win, "btn_Utils");

		if (get_selected_element() == btn_AGC)
		{
			window_t * win = get_win(WINDOW_AGC);
			open_window(win);
		}
		else if (get_selected_element() == btn_Utils)
		{
			window_t * win = get_win(WINDOW_UTILS);
			open_window(win);
		}
		else if (get_selected_element() == btn_Freq)
		{
			window_t * win = get_win(WINDOW_FREQ);
			open_window(win);
			hamradio_set_lockmode(1);
			hamradio_enable_keyboard_redirect();
		}
		else if (get_selected_element() == btn_TXsett)
		{
			window_t * win = get_win(WINDOW_TX_SETTINGS);
			open_window(win);
		}
		else if (get_selected_element() == btn_AUDsett)
		{
			window_t * win = get_win(WINDOW_AUDIOSETTINGS);
			open_window(win);
		}
		else if (get_selected_element() == btn_SysMenu)
		{
			window_t * win = get_win(WINDOW_MENU);
			open_window(win);
		}
	}
}

static void window_options_process(void)
{
	window_t * win = get_win(WINDOW_OPTIONS);

	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 3;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0, 100, 44, buttons_options_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_OPTIONS, NON_VISIBLE, INT32_MAX, "btn_SysMenu",   "System|settings", },
			{ 0, 0, 100, 44, buttons_options_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_OPTIONS, NON_VISIBLE, INT32_MAX, "btn_AUDsett",   "Audio|settings", },
			{ 0, 0, 100, 44, buttons_options_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_OPTIONS, NON_VISIBLE, INT32_MAX, "btn_TXsett",    "Transmit|settings", },
			{ 0, 0, 100, 44, buttons_options_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_OPTIONS, NON_VISIBLE, INT32_MAX, "btn_Freq",      "Freq|enter", },
			{ 0, 0, 100, 44, buttons_options_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_OPTIONS, NON_VISIBLE, INT32_MAX, "btn_AGC",  	  "AGC", },
			{ 0, 0, 100, 44, buttons_options_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_OPTIONS, NON_VISIBLE, INT32_MAX, "btn_Utils", 	  "Utils", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
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
		elements_state(win);
		calculate_window_position(win, xmax, ymax);
		hamradio_disable_keyboard_redirect();
		return;
	}
}

// *********************************************************************************************************************************************************************

static void buttons_utilites_handler(void)
{
	if (is_short_pressed())
	{
		window_t * win = get_win(WINDOW_UTILS);
		button_t * btn_SWRscan = find_gui_element(TYPE_BUTTON, win, "btn_SWRscan");						// SWR scanner

		if (get_selected_element() == btn_SWRscan)
		{
			window_t * winSWR = get_win(WINDOW_SWR_SCANNER);
			open_window(winSWR);
		}
	}
}

static void window_utilites_process(void)
{
	window_t * win = get_win(WINDOW_UTILS);
	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 4;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0, 100, 44, buttons_utilites_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_UTILS, NON_VISIBLE, INT32_MAX, "btn_SWRscan", "SWR|scanner", },

		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
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
		elements_state(win);
		calculate_window_position(win, xmax, ymax);
		return;
	}
}

// *********************************************************************************************************************************************************************

static void buttons_mode_handler(void)
{
	if (is_short_pressed())
	{
		window_t * win = get_win(WINDOW_MODES);
		button_t * bh = (button_t *)get_selected_element();
		if (win->state && bh->parent == win->window_id)
		{
			if (bh->payload != INT32_MAX)
				hamradio_change_submode(bh->payload, 1);

			close_window(OPEN_PARENT_WINDOW);
			footer_buttons_state(CANCELLED);
			gui_timer_update(NULL);
		}
	}
}

static void window_mode_process(void)
{
	window_t * win = get_win(WINDOW_MODES);

	if (win->first_call)
	{
		uint_fast16_t x, y;
		uint_fast16_t xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 4;
		uint_fast8_t id_start, id_end;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,      state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
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
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
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

static void buttons_af_handler(void)
{
	if (is_short_pressed())
	{
		window_t * win = get_win(WINDOW_BP);
		button_t * btnlowm = find_gui_element(TYPE_BUTTON, win, "btnlow-");
		button_t * btnlowp = find_gui_element(TYPE_BUTTON, win, "btnlow+");
		button_t * btnhighm = find_gui_element(TYPE_BUTTON, win, "btnhigh-");
		button_t * btnhighp = find_gui_element(TYPE_BUTTON, win, "btnhigh+");
		button_t * btnifshiftm = find_gui_element(TYPE_BUTTON, win, "btnifshift-");
		button_t * btnifshiftp = find_gui_element(TYPE_BUTTON, win, "btnifshift+");
		button_t * sel_el = get_selected_element();

		if (sel_el == btnlowm || sel_el == btnlowp)
		{
			bp_t.select = TYPE_BP_LOW;
			bp_t.change = sel_el->payload;
			bp_t.updated = 1;
		}
		else if (sel_el == btnhighm || sel_el == btnhighp)
		{
			bp_t.select = TYPE_BP_HIGH;
			bp_t.change = sel_el->payload;
			bp_t.updated = 1;
		}
		else if (sel_el == btnifshiftm || sel_el == btnifshiftp)
		{
			bp_t.select = TYPE_IF_SHIFT;
			bp_t.change = sel_el->payload;
			bp_t.updated = 1;
		}
	}
}

static void window_af_process(void)
{
	window_t * win = get_win(WINDOW_BP);
	static label_t * lbl_low = NULL, * lbl_high = NULL, * lbl_ifshift = NULL;
	static char str_low [TEXT_ARRAY_SIZE], str_high [TEXT_ARRAY_SIZE];
	uint_fast8_t val_high = 0, val_low = 0;
	static uint_fast8_t bp_type = 0;

	if (win->first_call)
	{
		win->first_call = 0;
		uint_fast8_t interval = 50, col1_int = 20;
		uint_fast16_t xmax = 0, ymax = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   visible,   payload,	 name, 			text
			{ 0, 0, 40, 40, buttons_af_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP, NON_VISIBLE, -1, "btnlow-", 		"-", },
			{ 0, 0, 40, 40, buttons_af_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP, NON_VISIBLE, 1,  "btnlow+", 		"+", },
			{ 0, 0, 40, 40, buttons_af_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP, NON_VISIBLE, -1, "btnhigh-", 		"-", },
			{ 0, 0, 40, 40, buttons_af_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP, NON_VISIBLE, 1,  "btnhigh+", 		"+", },
			{ 0, 0, 40, 40, buttons_af_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP, NON_VISIBLE, -1, "btnifshift-", 	"-", },
			{ 0, 0, 40, 40, buttons_af_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP, NON_VISIBLE, 1,  "btnifshift+", 	"+", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		label_t labels [] = {
		//    x, y,  parent, state, is_trackable, visible,  name, 			Text, 				font_size, 	color, 		onClickHandler
			{ 0, 0, WINDOW_BP, DISABLED,  0, NON_VISIBLE, "lbl_low",     "Low  cut : **** ",  FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0, WINDOW_BP, DISABLED,  0, NON_VISIBLE, "lbl_high",    "High cut : **** ",  FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0, WINDOW_BP, DISABLED,  0, NON_VISIBLE, "lbl_ifshift", "IF shift : **** ",  FONT_MEDIUM, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		ASSERT(win->lh_ptr != NULL);
		memcpy(win->lh_ptr, labels, labels_size);

		lbl_low = find_gui_element(TYPE_LABEL, win, "lbl_low");
		lbl_high = find_gui_element(TYPE_LABEL, win, "lbl_high");
		lbl_ifshift = find_gui_element(TYPE_LABEL, win, "lbl_ifshift");

		button_t * bh = & win->bh_ptr [0];

		lbl_low->x = col1_int;
		lbl_low->y = interval;
		lbl_low->visible = VISIBLE;

		lbl_high->x = lbl_low->x;
		lbl_high->y = lbl_low->y + interval;
		lbl_high->visible = VISIBLE;

		lbl_ifshift->x = lbl_high->x;
		lbl_ifshift->y = lbl_high->y + interval;
		lbl_ifshift->visible = VISIBLE;
		int16_t shift = hamradio_if_shift(0);
		if (shift == 0)
			local_snprintf_P(lbl_ifshift->text, ARRAY_SIZE(lbl_ifshift->text), PSTR("IF shift :  OFF"));
		else
			local_snprintf_P(lbl_ifshift->text, ARRAY_SIZE(lbl_ifshift->text), PSTR("IF shift :%+5d"), shift);

		uint_fast16_t x = lbl_low->x + get_label_width(lbl_low);
		uint_fast16_t y = lbl_low->y + get_label_height(lbl_low) / 2 - bh->h / 2;

		for (uint_fast8_t i = 0, r = 1; i < win->bh_count; i ++, r ++)
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
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}

		bp_type = hamradio_get_bp_type();
		val_low = hamradio_get_low_bp(0);
		val_high = hamradio_get_high_bp(0);

		if (bp_type)						// BWSET_WIDE
		{
			strcpy(str_low,  "Low  cut ");
			strcpy(str_high, "High cut ");
			val_high *= 100;
		}
		else								// BWSET_NARROW
		{
			strcpy(str_low,  "Width    ");
			strcpy(str_high, "Pitch    ");
			val_high *= 10;
		}

		local_snprintf_P(lbl_low->text, ARRAY_SIZE(lbl_low->text), PSTR("%s: %4d "), str_low, val_low * 10);
		local_snprintf_P(lbl_high->text, ARRAY_SIZE(lbl_high->text), PSTR("%s: %4d "), str_high, val_high);

		calculate_window_position(win, xmax, ymax);
		elements_state(win);

		return;
	}

	if (bp_t.updated)
	{
		bp_t.updated = 0;
		gui_timer_update(NULL);
		int16_t shift;

		switch(bp_t.select)
		{
		case TYPE_BP_LOW:

			val_low = hamradio_get_low_bp(bp_t.change * 5);
			local_snprintf_P(lbl_low->text, ARRAY_SIZE(lbl_low->text), PSTR("%s: %4d "), str_low, val_low * 10);
			break;

		case TYPE_BP_HIGH:

			val_high = hamradio_get_high_bp(bp_t.change);
			if (bp_type)
				val_high *= 100;
			else
				val_high *= 10;
			local_snprintf_P(lbl_high->text, ARRAY_SIZE(lbl_high->text), PSTR("%s: %4d "), str_high, val_high);

			break;

		case TYPE_IF_SHIFT:

			shift = hamradio_if_shift(bp_t.change);
			if (shift == 0)
				local_snprintf_P(lbl_ifshift->text, ARRAY_SIZE(lbl_ifshift->text), PSTR("IF shift :  OFF"));
			else
				local_snprintf_P(lbl_ifshift->text, ARRAY_SIZE(lbl_ifshift->text), PSTR("IF shift :%+5d"), shift);
			break;
		}
	}
}

// *********************************************************************************************************************************************************************

static void buttons_agc_handler(void)
{
	if (is_short_pressed())
	{
		window_t * win = get_win(WINDOW_AGC);
		button_t * btnAGCoff = find_gui_element(TYPE_BUTTON, win, "btnAGCoff");
		button_t * btnAGCslow = find_gui_element(TYPE_BUTTON, win, "btnAGCslow");
		button_t * btnAGCfast = find_gui_element(TYPE_BUTTON, win, "btnAGCfast");

		if (get_selected_element() == btnAGCoff)
		{
			hamradio_set_agc_off();
			close_all_windows();
		}
		else if (get_selected_element() == btnAGCslow)
		{
			hamradio_set_agc_slow();
			close_all_windows();
		}
		else if (get_selected_element() == btnAGCfast)
		{
			hamradio_set_agc_fast();
			close_all_windows();
		}
	}
}

static void window_agc_process(void)
{
	window_t * win = get_win(WINDOW_AGC);
	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 4;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0, 86, 44, buttons_agc_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC, NON_VISIBLE, INT32_MAX, "btnAGCoff",  "AGC|off", },
			{ 0, 0, 86, 44, buttons_agc_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC, NON_VISIBLE, INT32_MAX, "btnAGCslow", "AGC|slow", },
			{ 0, 0, 86, 44, buttons_agc_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC, NON_VISIBLE, INT32_MAX, "btnAGCfast", "AGC|fast", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
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

static void buttons_freq_handler (void)
{
	if (is_short_pressed())
	{
		button_t * bh =  get_selected_element();
		if (bh->parent == WINDOW_FREQ && editfreq.key == BUTTON_CODE_DONE)
			editfreq.key = bh->payload;
	}
}

static void window_freq_process (void)
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
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
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
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		label_t labels [] = {
		//    x, y,  parent, state, is_trackable, visible, name, Text, font_size, 	color, onClickHandler
			{ 0, 0,	WINDOW_FREQ, DISABLED, 0, NON_VISIBLE, "lbl_freq_val", "", FONT_LARGE, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		ASSERT(win->lh_ptr != NULL);
		memcpy(win->lh_ptr, labels, labels_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			bh = & win->bh_ptr [i];
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

		bh = find_gui_element(TYPE_BUTTON, win, "btnFreqOK");
		bh->is_locked = BUTTON_LOCKED;

		lbl_freq = find_gui_element(TYPE_LABEL, win, "lbl_freq_val");
		lbl_freq->x = strwidth(win->name) + strwidth(" ") + 20;
		lbl_freq->y = 5;
		strcpy(lbl_freq->text, "    0 k");
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
				lbl_freq->color = COLORMAIN_RED;

			break;

		default:
			if (editfreq.num < 5)
			{
				editfreq.val  = editfreq.val * 10 + editfreq.key;
				if (editfreq.val)
					editfreq.num ++;
			}
		}
		editfreq.key = BUTTON_CODE_DONE;

		local_snprintf_P(lbl_freq->text, ARRAY_SIZE(lbl_freq->text), PSTR("%5d k"), editfreq.val);
	}
}

// *********************************************************************************************************************************************************************

static void buttons_swrscan_process(void)
{
	if (is_short_pressed())
	{
		window_t * win = get_win(WINDOW_SWR_SCANNER);
		button_t * btn_swr_start = find_gui_element(TYPE_BUTTON, win, "btn_swr_start");
		button_t * btn_swr_OK = find_gui_element(TYPE_BUTTON, win, "btn_swr_OK");

		if (get_selected_element() == btn_swr_start && ! strcmp(btn_swr_start->text, "Start"))
		{
			swr_scan_enable = 1;
		}
		else if (get_selected_element() == btn_swr_start && ! strcmp(btn_swr_start->text, "Stop"))
		{
			swr_scan_stop = 1;
		}
		else if (get_selected_element() == btn_swr_OK)
		{
			close_all_windows();
			free(y_vals);
		}
	}
}

static void window_swrscan_process(void)
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
	static button_t * btn_swr_start, * btn_Options, * btn_swr_OK;
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
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0, 86, 44, buttons_swrscan_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_SWR_SCANNER, 	NON_VISIBLE, INT32_MAX,  "btn_swr_start", "Start", },
			{ 0, 0, 86, 44, buttons_swrscan_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_SWR_SCANNER, 	NON_VISIBLE, INT32_MAX,  "btn_swr_OK", 	"OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		label_t labels [] = {
		//    x, y,  parent,    state, is_trackable, visible,   name,   Text, font_size, 	color, 	 onClickHandler
			{ 0, 0,	WINDOW_SWR_SCANNER, DISABLED,  0, NON_VISIBLE, "lbl_swr_bottom", "", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_SWR_SCANNER, DISABLED,  0, NON_VISIBLE, "lbl_swr_top", 	 "", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_SWR_SCANNER, DISABLED,  0, NON_VISIBLE, "lbl_swr_error",  "", FONT_MEDIUM, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		ASSERT(win->lh_ptr != NULL);
		memcpy(win->lh_ptr, labels, labels_size);

		mid_w = col1_int + gr_w / 2;
		btn_swr_start = find_gui_element(TYPE_BUTTON, win, "btn_swr_start");
		btn_swr_start->x1 = mid_w - btn_swr_start->w - interval;
		btn_swr_start->y1 = row1_int + gr_h + col1_int;
		strcpy(btn_swr_start->text, "Start");
		btn_swr_start->visible = VISIBLE;

		btn_swr_OK = find_gui_element(TYPE_BUTTON, win, "btn_swr_OK");
		btn_swr_OK->x1 = mid_w + interval;
		btn_swr_OK->y1 = btn_swr_start->y1;
		btn_swr_OK->visible = VISIBLE;

		lbl_swr_error = find_gui_element(TYPE_LABEL, win, "lbl_swr_error");
		btn_Options = find_gui_element(TYPE_BUTTON, get_win(WINDOW_MAIN), "btn_Options");

		backup_freq = hamradio_get_freq_rx();
		if (hamradio_verify_freq_bands(backup_freq, & lim_bottom, & lim_top))
		{
			label_t * lbl_swr_bottom = find_gui_element(TYPE_LABEL, win, "lbl_swr_bottom");
			local_snprintf_P(lbl_swr_bottom->text, ARRAY_SIZE(lbl_swr_bottom->text), PSTR("%dk"), lim_bottom / 1000);
			lbl_swr_bottom->x = x0 - get_label_width(lbl_swr_bottom) / 2;
			lbl_swr_bottom->y = y0 + get_label_height(lbl_swr_bottom) * 2;
			lbl_swr_bottom->visible = VISIBLE;

			label_t * lbl_swr_top = find_gui_element(TYPE_LABEL, win, "lbl_swr_top");
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
		{	// если текущая частота не входит ни в один из любительских диапазонов, вывод сообщения об ошибке
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

		const uint_fast16_t swr_fullscale = (SWRMIN * 40 / 10) - SWRMIN;	// количество рисок в шкале ииндикатора
		y_vals [i] = normalize(get_swr(swr_fullscale), 0, swr_fullscale, y0 - y1);
		if (i)
			y_vals [i] = (y_vals [i - 1] * (averageFactor - 1) + y_vals [i]) / averageFactor;
		i++;
	}

	if (! win->first_call)
	{
		// отрисовка фона графика и разметки
		uint_fast16_t gr_x = win->x1 + x0, gr_y = win->y1 + y0;
		colpip_fillrect(fr, DIM_X, DIM_Y, win->x1 + col1_int, win->y1 + row1_int, gr_w, gr_h, COLORMAIN_BLACK);
		colmain_line(fr, DIM_X, DIM_Y, gr_x, gr_y, gr_x, win->y1 + y1, COLORMAIN_WHITE, 0);
		colmain_line(fr, DIM_X, DIM_Y, gr_x, gr_y, win->x1 + x1, gr_y, COLORMAIN_WHITE, 0);

		char buf [5];
		uint_fast8_t l = 1, row_step = round((y0 - y1) / 3);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), l++);
		colpip_string3_tbg(fr, DIM_X, DIM_Y, gr_x - SMALLCHARW3 * 2, gr_y - SMALLCHARH3 / 2, buf, COLORMAIN_WHITE);
		for(int_fast16_t yy = y0 - row_step; yy > y1; yy -= row_step)
		{
			if (yy < 0)
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
				colmain_line(fr, DIM_X, DIM_Y, gr_x + j - 2, gr_y - y_vals [j - 2], gr_x + j - 1, gr_y - y_vals [j - 1], COLORMAIN_YELLOW, 1);
		}
	}
}

// *********************************************************************************************************************************************************************

static void buttons_tx_sett_process(void)
{
	if (is_short_pressed())
	{
		window_t * winTX = get_win(WINDOW_TX_SETTINGS);
		window_t * winPower = get_win(WINDOW_TX_POWER);
		window_t * winVOX = get_win(WINDOW_TX_VOX_SETT);
		button_t * btn_tx_vox = find_gui_element(TYPE_BUTTON, winTX, "btn_tx_vox");
		button_t * btn_tx_power = find_gui_element(TYPE_BUTTON, winTX, "btn_tx_power");
		button_t * btn_tx_vox_settings = find_gui_element(TYPE_BUTTON, winTX, "btn_tx_vox_settings");
		if (get_selected_element() == btn_tx_vox)
		{
			btn_tx_vox->is_locked = hamradio_get_gvoxenable() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_tx_vox->text, ARRAY_SIZE(btn_tx_vox->text), PSTR("VOX|%s"), btn_tx_vox->is_locked ? "ON" : "OFF");
			hamradio_set_gvoxenable(btn_tx_vox->is_locked);
			btn_tx_vox_settings->state = hamradio_get_gvoxenable() ? CANCELLED : DISABLED;
		}
		else if (get_selected_element() == btn_tx_vox_settings)
		{
			open_window(winVOX);
		}
		else if (get_selected_element() == btn_tx_power)
		{
			open_window(winPower);
		}
	}
}

static void window_tx_process(void)
{
	window_t * win = get_win(WINDOW_TX_SETTINGS);

	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 3;
		button_t * bh = NULL;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0, 100, 44, buttons_tx_sett_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_SETTINGS, 	NON_VISIBLE, INT32_MAX, "btn_tx_vox", 	 	 "VOX|OFF", },
			{ 0, 0, 100, 44, buttons_tx_sett_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_SETTINGS, 	NON_VISIBLE, INT32_MAX, "btn_tx_vox_settings", "VOX|settings", },
			{ 0, 0, 100, 44, buttons_tx_sett_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_SETTINGS, 	NON_VISIBLE, INT32_MAX, "btn_tx_power", 	 	 "TX power", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
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
		bh = find_gui_element(TYPE_BUTTON, win, "btn_tx_vox"); 						// vox on/off
		bh->is_locked = hamradio_get_gvoxenable() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("VOX|%s"), hamradio_get_gvoxenable() ? "ON" : "OFF");

		bh = find_gui_element(TYPE_BUTTON, win, "btn_tx_vox_settings");				// vox settings
		bh->state = hamradio_get_gvoxenable() ? CANCELLED : DISABLED;
#else
		bh = find_gui_element(TYPE_BUTTON, win, "btn_tx_vox");						// reverb on/off disable
		bh->state = DISABLED;

		bh = find_gui_element(TYPE_BUTTON, win, "btn_tx_vox_settings"); 			// reverb settings disable
		bh->state = DISABLED;
#endif /* WITHVOX */
		elements_state(win);
		return;
	}
}

// *********************************************************************************************************************************************************************

static void buttons_tx_vox_process(void)
{
	if (is_short_pressed())
	{
		window_t * win = get_win(WINDOW_TX_VOX_SETT);
		button_t * btn_tx_vox_OK = find_gui_element(TYPE_BUTTON, win, "btn_tx_vox_OK");
		if (get_selected_element() == btn_tx_vox_OK)
		{
			close_all_windows();
		}
	}
}

static void window_tx_vox_process(void)
{
	window_t * win = get_win(WINDOW_TX_VOX_SETT);

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
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0,  44, 44, buttons_tx_vox_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_VOX_SETT, NON_VISIBLE, INT32_MAX,	"btn_tx_vox_OK", "OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		slider_t sliders [] = {
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, "sl_vox_delay",  CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, "sl_vox_level",  CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, "sl_avox_level", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		};
		win->sh_count = ARRAY_SIZE(sliders);
		uint_fast16_t sliders_size = sizeof(sliders);
		win->sh_ptr = malloc(sliders_size);
		ASSERT(win->sh_ptr != NULL);
		memcpy(win->sh_ptr, sliders, sliders_size);

		label_t labels [] = {
		//    x, y,  parent,  state, is_trackable, visible,   name,   Text, font_size, 	color, 	onClickHandler
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
		ASSERT(win->lh_ptr != NULL);
		memcpy(win->lh_ptr, labels, labels_size);

		hamradio_get_vox_delay_limits(& delay_min, & delay_max);
		hamradio_get_vox_level_limits(& level_min, & level_max);
		hamradio_get_antivox_delay_limits(& alevel_min, & alevel_max);

		sl_vox_delay = find_gui_element(TYPE_SLIDER, win, "sl_vox_delay");
		sl_vox_level = find_gui_element(TYPE_SLIDER, win, "sl_vox_level");
		sl_avox_level = find_gui_element(TYPE_SLIDER, win, "sl_avox_level");
		lbl_vox_delay = find_gui_element(TYPE_LABEL, win, "lbl_vox_delay");
		lbl_vox_level = find_gui_element(TYPE_LABEL, win, "lbl_vox_level");
		lbl_avox_level = find_gui_element(TYPE_LABEL, win, "lbl_avox_level");

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

		button_t * bh = find_gui_element(TYPE_BUTTON, win, "btn_tx_vox_OK");
		bh->x1 = (sl_vox_delay->x + sl_vox_delay->size + col1_int * 2) / 2 - (bh->w / 2);
		bh->y1 = lbl_avox_level->y + interval;
		bh->visible = VISIBLE;

		d = ldiv(delay_min, 100);
		label_t * lbl_vox_delay_min = find_gui_element(TYPE_LABEL, win, "lbl_vox_delay_min");
		local_snprintf_P(lbl_vox_delay_min->text, ARRAY_SIZE(lbl_vox_delay_min->text), PSTR("%d.%d sec"), d.quot, d.rem / 10);
		lbl_vox_delay_min->x = sl_vox_delay->x - get_label_width(lbl_vox_delay_min) / 2;
		lbl_vox_delay_min->y = sl_vox_delay->y + get_label_height(lbl_vox_delay_min) * 3;
		lbl_vox_delay_min->visible = VISIBLE;

		d = ldiv(delay_max, 100);
		label_t * lbl_vox_delay_max = find_gui_element(TYPE_LABEL, win, "lbl_vox_delay_max");
		local_snprintf_P(lbl_vox_delay_max->text, ARRAY_SIZE(lbl_vox_delay_max->text), PSTR("%d.%d sec"), d.quot, d.rem / 10);
		lbl_vox_delay_max->x = sl_vox_delay->x + sl_vox_delay->size - get_label_width(lbl_vox_delay_max) / 2;
		lbl_vox_delay_max->y = sl_vox_delay->y + get_label_height(lbl_vox_delay_max) * 3;
		lbl_vox_delay_max->visible = VISIBLE;

		label_t * lbl_vox_level_min = find_gui_element(TYPE_LABEL, win, "lbl_vox_level_min");
		local_snprintf_P(lbl_vox_level_min->text, ARRAY_SIZE(lbl_vox_level_min->text), PSTR("%d"), level_min);
		lbl_vox_level_min->x = sl_vox_level->x - get_label_width(lbl_vox_level_min) / 2;
		lbl_vox_level_min->y = sl_vox_level->y + get_label_height(lbl_vox_level_min) * 3;
		lbl_vox_level_min->visible = VISIBLE;

		label_t * lbl_vox_level_max = find_gui_element(TYPE_LABEL, win, "lbl_vox_level_max");
		local_snprintf_P(lbl_vox_level_max->text, ARRAY_SIZE(lbl_vox_level_max->text), PSTR("%d"), level_max);
		lbl_vox_level_max->x = sl_vox_level->x + sl_vox_level->size - get_label_width(lbl_vox_level_max) / 2;
		lbl_vox_level_max->y = sl_vox_level->y + get_label_height(lbl_vox_level_max) * 3;
		lbl_vox_level_max->visible = VISIBLE;

		label_t * lbl_avox_level_min = find_gui_element(TYPE_LABEL, win, "lbl_avox_level_min");
		local_snprintf_P(lbl_avox_level_min->text, ARRAY_SIZE(lbl_avox_level_min->text), PSTR("%d"), alevel_min);
		lbl_avox_level_min->x = sl_avox_level->x - get_label_width(lbl_avox_level_min) / 2;
		lbl_avox_level_min->y = sl_avox_level->y + get_label_height(lbl_avox_level_min) * 3;
		lbl_avox_level_min->visible = VISIBLE;

		label_t * lbl_avox_level_max = find_gui_element(TYPE_LABEL, win, "lbl_avox_level_max");
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

	if (is_moving_slider())
	{
		char buf [TEXT_ARRAY_SIZE];

		/* костыль через костыль */
		sl = (slider_t *) get_selected_element();

		if (sl == sl_vox_delay)
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

static void buttons_tx_power_process(void)
{
	if (is_short_pressed())
	{
		window_t * win = get_win(WINDOW_TX_POWER);
		button_t * btn_tx_pwr_OK = find_gui_element(TYPE_BUTTON, win, "btn_tx_pwr_OK");
		if (get_selected_element() == btn_tx_pwr_OK)
		{
			close_all_windows();
		}
	}
}

static void window_tx_power_process(void)
{
	window_t * win = get_win(WINDOW_TX_POWER);

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
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0,  44, 44, buttons_tx_power_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_POWER,  NON_VISIBLE, INT32_MAX, "btn_tx_pwr_OK", "OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		slider_t sliders [] = {
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_POWER, "sl_pwr_level",   	   CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_POWER, "sl_pwr_tuner_level", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		};
		win->sh_count = ARRAY_SIZE(sliders);
		uint_fast16_t sliders_size = sizeof(sliders);
		win->sh_ptr = malloc(sliders_size);
		ASSERT(win->sh_ptr != NULL);
		memcpy(win->sh_ptr, sliders, sliders_size);

		label_t labels [] = {
		//    x, y,  parent,  state, is_trackable, visible,   name,    Text, font_size, 	color, 		onClickHandler
			{ 0, 0,	WINDOW_TX_POWER, DISABLED,  0, NON_VISIBLE, "lbl_tx_power",   "", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_POWER, DISABLED,  0, NON_VISIBLE, "lbl_tune_power", "", FONT_MEDIUM, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		ASSERT(win->lh_ptr != NULL);
		memcpy(win->lh_ptr, labels, labels_size);

		sl_pwr_level = find_gui_element(TYPE_SLIDER, win, "sl_pwr_level");
		sl_pwr_tuner_level = find_gui_element(TYPE_SLIDER, win, "sl_pwr_tuner_level");

		lbl_tx_power = find_gui_element(TYPE_LABEL, win, "lbl_tx_power");
		lbl_tune_power = find_gui_element(TYPE_LABEL, win, "lbl_tune_power");

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

		button_t * bh = find_gui_element(TYPE_BUTTON, win, "btn_tx_pwr_OK");
		bh->x1 = (sl_pwr_level->x + sl_pwr_level->size + col1_int * 2) / 2 - (bh->w / 2);
		bh->y1 = lbl_tune_power->y + interval;
		bh->visible = VISIBLE;

		xmax = sl_pwr_tuner_level->x + sl_pwr_tuner_level->size + col1_int;
		ymax = bh->y1 + bh->h;
		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}

	if (is_moving_slider())
	{
		char buf [TEXT_ARRAY_SIZE];

		/* костыль через костыль */
		sl = (slider_t *) get_selected_element();

		if (sl == sl_pwr_level)
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

static void buttons_audiosettings_process(void)
{
	if (is_short_pressed())
	{
		window_t * winAP = get_win(WINDOW_AUDIOSETTINGS);
		window_t * winEQ = get_win(WINDOW_AP_MIC_EQ);
		window_t * winRS = get_win(WINDOW_AP_REVERB_SETT);
		window_t * winMIC = get_win(WINDOW_AP_MIC_SETT);
		window_t * winMICpr = get_win(WINDOW_AP_MIC_PROF);
		button_t * btn_reverb = find_gui_element(TYPE_BUTTON, winAP, "btn_reverb");						// reverb on/off
		button_t * btn_reverb_settings = find_gui_element(TYPE_BUTTON, winAP, "btn_reverb_settings");	// reverb settings
		button_t * btn_monitor = find_gui_element(TYPE_BUTTON, winAP, "btn_monitor");					// monitor on/off
		button_t * btn_speaker = find_gui_element(TYPE_BUTTON, winAP, "btn_speaker");					// speaker mute
		button_t * btn_mic_eq = find_gui_element(TYPE_BUTTON, winAP, "btn_mic_eq");						// MIC EQ on/off
		button_t * btn_mic_eq_settings = find_gui_element(TYPE_BUTTON, winAP, "btn_mic_eq_settings");	// MIC EQ settingss
		button_t * btn_mic_settings = find_gui_element(TYPE_BUTTON, winAP, "btn_mic_settings");			// mic settings
		button_t * btn_mic_profiles = find_gui_element(TYPE_BUTTON, winAP, "btn_mic_profiles");			// mic profiles

		if (get_selected_element() == btn_reverb)
		{
			btn_reverb->is_locked = hamradio_get_greverb() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_reverb->text, ARRAY_SIZE(btn_reverb->text), PSTR("Reverb|%s"), btn_reverb->is_locked ? "ON" : "OFF");
			hamradio_set_greverb(btn_reverb->is_locked);
			btn_reverb_settings->state = btn_reverb->is_locked ? CANCELLED : DISABLED;

		}
		else if (get_selected_element() == btn_reverb_settings)
		{
			open_window(winRS);
		}

		else if (get_selected_element() == btn_monitor)
		{
			btn_monitor->is_locked = hamradio_get_gmoniflag() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_monitor->text, ARRAY_SIZE(btn_monitor->text), PSTR("Monitor|%s"), btn_monitor->is_locked ? "enabled" : "disabled");
			hamradio_set_gmoniflag(btn_monitor->is_locked);
		}
		else if (get_selected_element() == btn_speaker)
		{
			btn_speaker->is_locked = hamradio_get_gmutespkr() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_speaker->text, ARRAY_SIZE(btn_speaker->text), PSTR("Speaker|%s"), btn_speaker->is_locked ? "muted" : "on air");
			hamradio_set_gmutespkr(btn_speaker->is_locked);
		}
		else if (get_selected_element() == btn_mic_eq)
		{
			btn_mic_eq->is_locked = hamradio_get_gmikeequalizer() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_mic_eq->text, ARRAY_SIZE(btn_mic_eq->text), PSTR("MIC EQ|%s"), btn_mic_eq->is_locked ? "ON" : "OFF");
			hamradio_set_gmikeequalizer(btn_mic_eq->is_locked);
			btn_mic_eq_settings->state = btn_mic_eq->is_locked ? CANCELLED : DISABLED;
		}
		else if (get_selected_element() == btn_mic_eq_settings)
		{
			open_window(winEQ);
		}
		else if (get_selected_element() == btn_mic_settings)
		{
			open_window(winMIC);
		}
		else if (get_selected_element() == btn_mic_profiles)
		{
			open_window(winMICpr);
		}
	}
}

static void window_audiosettings_process(void)
{
	window_t * win = get_win(WINDOW_AUDIOSETTINGS);

	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 4;
		button_t * bh = NULL;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0, 100, 44, buttons_audiosettings_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, INT32_MAX, "btn_reverb", 			"Reverb|OFF", },
			{ 0, 0, 100, 44, buttons_audiosettings_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, INT32_MAX, "btn_reverb_settings", 	"Reverb|settings", },
			{ 0, 0, 100, 44, buttons_audiosettings_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, INT32_MAX, "btn_monitor", 			"Monitor|disabled", },
			{ 0, 0, 100, 44, buttons_audiosettings_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, INT32_MAX, "btn_speaker", 			"Speaker|on air", },
			{ 0, 0, 100, 44, buttons_audiosettings_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, INT32_MAX, "btn_mic_eq", 			"MIC EQ|OFF", },
			{ 0, 0, 100, 44, buttons_audiosettings_process,	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, INT32_MAX, "btn_mic_eq_settings", 	"MIC EQ|settings", },
			{ 0, 0, 100, 44, buttons_audiosettings_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, INT32_MAX, "btn_mic_settings", 		"MIC|settings", },
			{ 0, 0, 100, 44, buttons_audiosettings_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, INT32_MAX, "btn_mic_profiles", 		"MIC|profiles", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
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
		bh = find_gui_element(TYPE_BUTTON, win, "btn_reverb"); 						// reverb on/off
		bh->is_locked = hamradio_get_greverb() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Reverb|%s"), hamradio_get_greverb() ? "ON" : "OFF");

		bh = find_gui_element(TYPE_BUTTON, win, "btn_reverb_settings");				// reverb settings
		bh->state = hamradio_get_greverb() ? CANCELLED : DISABLED;
#else
		bh = find_gui_element(TYPE_BUTTON, win, "btn_reverb");						// reverb on/off disable
		bh->state = DISABLED;

		bh = find_gui_element(TYPE_BUTTON, win, "btn_reverb_settings"); 			// reverb settings disable
		bh->state = DISABLED;
#endif /* WITHREVERB */

		bh = find_gui_element(TYPE_BUTTON, win, "btn_monitor");						// monitor on/off
		bh->is_locked = hamradio_get_gmoniflag() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Monitor|%s"), bh->is_locked ? "enabled" : "disabled");

#if WITHAFCODEC1HAVEPROC
		bh = find_gui_element(TYPE_BUTTON, win, "btn_mic_eq");						// MIC EQ on/off
		bh->is_locked = hamradio_get_gmikeequalizer() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("MIC EQ|%s"), bh->is_locked ? "ON" : "OFF");

		bh = find_gui_element(TYPE_BUTTON, win, "btn_mic_eq_settings");				// MIC EQ settings
		bh->state = hamradio_get_gmikeequalizer() ? CANCELLED : DISABLED;
#else
		bh = find_gui_element(TYPE_BUTTON, win, "btn_mic_eq");						// MIC EQ on/off disable
		bh->state = DISABLED;

		bh = find_gui_element(TYPE_BUTTON, win, "btn_mic_eq_settings"); 			// MIC EQ settings disable
		bh->state = DISABLED;
#endif /* WITHAFCODEC1HAVEPROC */

		bh = find_gui_element(TYPE_BUTTON, win, "btn_speaker");
#if WITHSPKMUTE
		bh->is_locked = hamradio_get_gmutespkr();
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Speaker|%s"), bh->is_locked ? "muted" : "on air");
#else
		bh->state = DISABLED;
#endif /* WITHSPKMUTE */

		elements_state(win);
		return;
	}
}

// *********************************************************************************************************************************************************************

static void buttons_ap_reverb_process(void)
{
	if (is_short_pressed())
	{
		close_window(OPEN_PARENT_WINDOW);
	}
}

static void window_ap_reverb_process(void)
{
	window_t * win = get_win(WINDOW_AP_REVERB_SETT);

	static label_t * lbl_reverbDelay = NULL, * lbl_reverbLoss = NULL;
	static slider_t * sl_reverbDelay = NULL, * sl_reverbLoss = NULL;
	static uint_fast16_t delay_min, delay_max, loss_min, loss_max;
	slider_t * sl = NULL;

	if (win->first_call)
	{
		uint_fast8_t interval = 60, col1_int = 20;
		uint_fast16_t xmax = 0, ymax = 0;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0,  40, 40, buttons_ap_reverb_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_REVERB_SETT,	NON_VISIBLE, INT32_MAX, "btn_REVs_ok", "OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		label_t labels [] = {
		//    x, y,  parent,     		state, is_trackable, visible,   name,       		Text, font_size, 	color, 	onClickHandler
			{ 0, 0,	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbDelay",		"", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbLoss", 		"", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbDelay_min", 	"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbDelay_max", 	"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbLoss_min", 	"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbLoss_max", 	"", FONT_SMALL, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		ASSERT(win->lh_ptr != NULL);
		memcpy(win->lh_ptr, labels, labels_size);

		slider_t sliders [] = {
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_AP_REVERB_SETT, 	"reverbDelay", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_AP_REVERB_SETT, 	"reverbLoss",  CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		};
		win->sh_count = ARRAY_SIZE(sliders);
		uint_fast16_t sliders_size = sizeof(sliders);
		win->sh_ptr = malloc(sliders_size);
		ASSERT(win->sh_ptr != NULL);
		memcpy(win->sh_ptr, sliders, sliders_size);

		label_t * lbl_reverbDelay_min = find_gui_element(TYPE_LABEL, win, "lbl_reverbDelay_min");
		label_t * lbl_reverbDelay_max = find_gui_element(TYPE_LABEL, win, "lbl_reverbDelay_max");
		label_t * lbl_reverbLoss_min = find_gui_element(TYPE_LABEL, win, "lbl_reverbLoss_min");
		label_t * lbl_reverbLoss_max = find_gui_element(TYPE_LABEL, win, "lbl_reverbLoss_max");
		lbl_reverbDelay = find_gui_element(TYPE_LABEL, win, "lbl_reverbDelay");
		lbl_reverbLoss = find_gui_element(TYPE_LABEL, win, "lbl_reverbLoss");
		sl_reverbDelay = find_gui_element(TYPE_SLIDER, win, "reverbDelay");
		sl_reverbLoss = find_gui_element(TYPE_SLIDER, win, "reverbLoss");

		hamradio_get_reverb_delay_limits(& delay_min, & delay_max);
		hamradio_get_reverb_loss_limits(& loss_min, & loss_max);

		lbl_reverbDelay->x = col1_int;
		lbl_reverbDelay->y = interval;
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

		button_t * bh = find_gui_element(TYPE_BUTTON, win, "btn_REVs_ok");
		bh->x1 = sl_reverbLoss->x + sl_reverbLoss->size + interval / 2;
		bh->y1 = lbl_reverbLoss->y;
		bh->visible = VISIBLE;

		xmax = bh->x1 + bh->w;
		ymax = bh->y1 + bh->h;
		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}

	if (is_moving_slider())
	{
		char buf [TEXT_ARRAY_SIZE];

		/* костыль через костыль */
		sl = (slider_t *) get_selected_element();

		if (sl == sl_reverbDelay)
		{
			uint_fast16_t delay = delay_min + normalize(sl->value, 0, 100, delay_max - delay_min);
			local_snprintf_P(lbl_reverbDelay->text, ARRAY_SIZE(lbl_reverbDelay->text), PSTR("Delay: %3d ms"), delay);
			hamradio_set_reverb_delay(delay);
		}
		else if (sl == sl_reverbLoss)
		{
			uint_fast16_t loss = loss_min + normalize(sl->value, 0, 100, loss_max - loss_min);
			local_snprintf_P(lbl_reverbLoss->text, ARRAY_SIZE(lbl_reverbLoss->text), PSTR("Loss :  %2d dB"), loss);
			hamradio_set_reverb_loss(loss);
		}
	}
}

// *********************************************************************************************************************************************************************

static void buttons_ap_mic_eq_process(void)
{
	if (is_short_pressed())
	{
		close_window(OPEN_PARENT_WINDOW);
	}
}

static void window_ap_mic_eq_process(void)
{
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	window_t * win = get_win(WINDOW_AP_MIC_EQ);

	slider_t * sl = NULL;
	label_t * lbl = NULL;
	static uint_fast8_t eq_limit, eq_base = 0;
	char buf [TEXT_ARRAY_SIZE];
	static int_fast16_t mid_y = 0;
	static uint_fast8_t id = 0;
	static button_t * btn_EQ_ok;

	if (win->first_call)
	{
		uint_fast16_t x, y, mid_w;
		uint_fast16_t xmax = 0, ymax = 0;
		uint_fast8_t interval = 70, col1_int = 70, row1_int = window_title_height + 20;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0,  40, 40, buttons_ap_mic_eq_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_MIC_EQ, 	NON_VISIBLE, INT32_MAX, "btn_EQ_ok", "OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		label_t labels [] = {
		//    x, y,  parent, state, is_trackable, visible,   name,    Text, font_size, 	color, 	onClickHandler
			{ 0, 0,	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq0.08_val", 	"", FONT_LARGE, COLORMAIN_YELLOW, },
			{ 0, 0,	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq0.23_val", 	"", FONT_LARGE, COLORMAIN_YELLOW, },
			{ 0, 0,	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq0.65_val",  "", FONT_LARGE, COLORMAIN_YELLOW, },
			{ 0, 0,	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq1.8_val",   "", FONT_LARGE, COLORMAIN_YELLOW, },
			{ 0, 0,	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq5.3_val",   "", FONT_LARGE, COLORMAIN_YELLOW, },
			{ 0, 0,	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq0.08_name", "", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq0.23_name", "", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq0.65_name", "", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq1.8_name",  "", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_MIC_EQ, DISABLED,  0, NON_VISIBLE, "lbl_eq5.3_name",  "", FONT_MEDIUM, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		ASSERT(win->lh_ptr != NULL);
		memcpy(win->lh_ptr, labels, labels_size);

		slider_t sliders [] = {
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, WINDOW_AP_MIC_EQ, "eq0.08", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, WINDOW_AP_MIC_EQ, "eq0.23", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, WINDOW_AP_MIC_EQ, "eq0.65", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, WINDOW_AP_MIC_EQ, "eq1.8",  CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, WINDOW_AP_MIC_EQ, "eq5.3",  CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		};
		win->sh_count = ARRAY_SIZE(sliders);
		uint_fast16_t sliders_size = sizeof(sliders);
		win->sh_ptr = malloc(sliders_size);
		ASSERT(win->sh_ptr != NULL);
		memcpy(win->sh_ptr, sliders, sliders_size);

		eq_base = hamradio_getequalizerbase();
		eq_limit = abs(eq_base) * 2;

		x = col1_int;
		y = row1_int;

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
			lbl = find_gui_element(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%sk"), strchr(sl->name, 'q') + 1);
			lbl->x = mid_w - get_label_width(lbl) / 2;
			lbl->y = y;
			lbl->visible = VISIBLE;

			y = lbl->y + get_label_height(lbl) * 2;

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_val"), sl->name);
			lbl = find_gui_element(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%d"), hamradio_get_gmikeequalizerparams(id) + eq_base);
			lbl->x = mid_w - get_label_width(lbl) / 2;
			lbl->y = y;
			lbl->visible = VISIBLE;

			sl->y = lbl->y + get_label_height(lbl) * 2 + 10;

			x = x + interval;
			y = row1_int;
		}

		btn_EQ_ok = find_gui_element(TYPE_BUTTON, win, "btn_EQ_ok");
		btn_EQ_ok->x1 = sl->x + sliders_width + btn_EQ_ok->w;
		btn_EQ_ok->y1 = sl->y + sl->size - btn_EQ_ok->h;
		btn_EQ_ok->visible = VISIBLE;

		xmax = btn_EQ_ok->x1 + btn_EQ_ok->w;
		ymax = btn_EQ_ok->y1 + btn_EQ_ok->h;
		calculate_window_position(win, xmax, ymax);
		elements_state(win);

		mid_y = win->y1 + sl->y + sl->size / 2;

		return;
	}

	if (is_moving_slider())
	{
		/* костыль через костыль */
		sl = get_selected_element();
		uint_fast8_t id = get_selected_element_pos();

		hamradio_set_gmikeequalizerparams(id, normalize(sl->value, 100, 0, eq_limit));

		uint_fast16_t mid_w = sl->x + sliders_width / 2;
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_val"), sl->name);
		lbl = find_gui_element(TYPE_LABEL, win, buf);
		local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%d"), hamradio_get_gmikeequalizerparams(id) + eq_base);
		lbl->x = mid_w - get_label_width(lbl) / 2;
	}

	for (uint_fast16_t i = 0; i <= abs(eq_base); i += 3)
	{
		uint_fast16_t yy = normalize(i, 0, abs(eq_base), 100);
		colmain_line(fr, DIM_X, DIM_Y, win->x1 + 50, mid_y + yy, win->x1 + win->w - (btn_EQ_ok->w << 1), mid_y + yy, GUI_SLIDERLAYOUTCOLOR, 0);
		local_snprintf_P(buf, ARRAY_SIZE(buf), i == 0 ? PSTR("%d") : PSTR("-%d"), i);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, win->x1 + 50 - strwidth2(buf) - 5, mid_y + yy - SMALLCHARH2 / 2, buf, COLORMAIN_WHITE);

		if (i == 0)
			continue;
		colmain_line(fr, DIM_X, DIM_Y, win->x1 + 50, mid_y - yy, win->x1 + win->w - (btn_EQ_ok->w << 1), mid_y - yy, GUI_SLIDERLAYOUTCOLOR, 0);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), i);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, win->x1 + 50 - strwidth2(buf) - 5, mid_y - yy - SMALLCHARH2 / 2, buf, COLORMAIN_WHITE);
	}
}

// *********************************************************************************************************************************************************************

static void buttons_ap_mic_process(void)
{
	if (is_short_pressed())
	{
		window_t * win = get_win(WINDOW_AP_MIC_SETT);
		button_t * btn_mic_boost = find_gui_element(TYPE_BUTTON, win, "btn_mic_boost");
		button_t * btn_mic_agc = find_gui_element(TYPE_BUTTON, win, "btn_mic_agc");
		button_t * btn_mic_OK = find_gui_element(TYPE_BUTTON, win, "btn_mic_OK");

		if (get_selected_element() == btn_mic_boost)
		{
			btn_mic_boost->is_locked = hamradio_get_gmikebust20db() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_mic_boost->text, ARRAY_SIZE(btn_mic_boost->text), PSTR("Boost|%s"), btn_mic_boost->is_locked ? "ON" : "OFF");
			hamradio_set_gmikebust20db(btn_mic_boost->is_locked);
		}
		else if (get_selected_element() == btn_mic_agc)
		{
			btn_mic_agc->is_locked = hamradio_get_gmikeagc() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_mic_agc->text, ARRAY_SIZE(btn_mic_agc->text), PSTR("AGC|%s"), btn_mic_agc->is_locked ? "ON" : "OFF");
			hamradio_set_gmikeagc(btn_mic_agc->is_locked);
		}
		else if (get_selected_element() == btn_mic_OK)
		{
			close_all_windows();
		}
	}
}

static void window_ap_mic_process(void)
{
	window_t * win = get_win(WINDOW_AP_MIC_SETT);

	static slider_t * sl_micLevel = NULL, * sl_micClip = NULL, * sl_micAGC = NULL;
	static label_t * lbl_micLevel = NULL, * lbl_micClip = NULL, * lbl_micAGC = NULL;
	static uint_fast16_t level_min, level_max, clip_min, clip_max, agc_min, agc_max;
	slider_t * sl;

	if (win->first_call)
	{
		uint_fast8_t interval = 50, col1_int = 20;
		uint_fast16_t xmax = 0, ymax = 0;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0,  86, 44, buttons_ap_mic_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_MIC_SETT,  	NON_VISIBLE, INT32_MAX, "btn_mic_agc", 	"AGC|OFF", },
			{ 0, 0,  86, 44, buttons_ap_mic_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_MIC_SETT,  	NON_VISIBLE, INT32_MAX, "btn_mic_boost", 	"Boost|OFF", },
			{ 0, 0,  86, 44, buttons_ap_mic_process, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_MIC_SETT,  	NON_VISIBLE, INT32_MAX, "btn_mic_OK", 	"OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		label_t labels [] = {
		//    x, y,  parent,     		state, is_trackable, visible,   name,       		Text, font_size, 	color, 			onClickHandler
			{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micLevel", 			"", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micClip",  			"", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micAGC",   			"", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micLevel_min", 		"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micLevel_max", 		"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micClip_min",  		"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micClip_max",  		"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micAGC_min",   		"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micAGC_max",   		"", FONT_SMALL, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		ASSERT(win->lh_ptr != NULL);
		memcpy(win->lh_ptr, labels, labels_size);

		slider_t sliders [] = {
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_AP_MIC_SETT, 	"sl_micLevel", 			CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_AP_MIC_SETT, 	"sl_micClip",  			CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_AP_MIC_SETT, 	"sl_micAGC",   			CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		};
		win->sh_count = ARRAY_SIZE(sliders);
		uint_fast16_t sliders_size = sizeof(sliders);
		win->sh_ptr = malloc(sliders_size);
		ASSERT(win->sh_ptr != NULL);
		memcpy(win->sh_ptr, sliders, sliders_size);

		hamradio_get_mic_level_limits(& level_min, & level_max);
		hamradio_get_mic_clip_limits(& clip_min, & clip_max);
		hamradio_get_mic_agc_limits(& agc_min, & agc_max);

		sl_micLevel = find_gui_element(TYPE_SLIDER, win, "sl_micLevel");
		sl_micClip = find_gui_element(TYPE_SLIDER, win, "sl_micClip");
		sl_micAGC = find_gui_element(TYPE_SLIDER, win, "sl_micAGC");
		lbl_micLevel = find_gui_element(TYPE_LABEL, win, "lbl_micLevel");
		lbl_micClip = find_gui_element(TYPE_LABEL, win, "lbl_micClip");
		lbl_micAGC = find_gui_element(TYPE_LABEL, win, "lbl_micAGC");

		lbl_micLevel->x = col1_int;
		lbl_micLevel->y = interval;
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

		button_t * bh2 = find_gui_element(TYPE_BUTTON, win, "btn_mic_boost");
		bh2->x1 = (sl_micLevel->x + sl_micLevel->size + col1_int * 2) / 2 - (bh2->w / 2);
		bh2->y1 = lbl_micAGC->y + interval;
		bh2->is_locked = hamradio_get_gmikebust20db() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh2->text, ARRAY_SIZE(bh2->text), PSTR("Boost|%s"), bh2->is_locked ? "ON" : "OFF");
		bh2->visible = VISIBLE;

		button_t * bh1 = find_gui_element(TYPE_BUTTON, win, "btn_mic_agc");
		bh1->x1 = bh2->x1 - bh1->w - interval;
		bh1->y1 = bh2->y1;
		bh1->is_locked = hamradio_get_gmikeagc() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh1->text, ARRAY_SIZE(bh1->text), PSTR("AGC|%s"), bh1->is_locked ? "ON" : "OFF");
		bh1->visible = VISIBLE;

		bh1 = find_gui_element(TYPE_BUTTON, win, "btn_mic_OK");
		bh1->x1 = bh2->x1 + bh2->w + interval;
		bh1->y1 = bh2->y1;
		bh1->visible = VISIBLE;

		label_t * lbl_micLevel_min = find_gui_element(TYPE_LABEL, win, "lbl_micLevel_min");
		local_snprintf_P(lbl_micLevel_min->text, ARRAY_SIZE(lbl_micLevel_min->text), PSTR("%d"), level_min);
		lbl_micLevel_min->x = sl_micLevel->x - get_label_width(lbl_micLevel_min) / 2;
		lbl_micLevel_min->y = sl_micLevel->y + get_label_height(lbl_micLevel_min) * 3;
		lbl_micLevel_min->visible = VISIBLE;

		label_t * lbl_micLevel_max = find_gui_element(TYPE_LABEL, win, "lbl_micLevel_max");
		local_snprintf_P(lbl_micLevel_max->text, ARRAY_SIZE(lbl_micLevel_max->text), PSTR("%d"), level_max);
		lbl_micLevel_max->x = sl_micLevel->x + sl_micLevel->size - get_label_width(lbl_micLevel_max) / 2;
		lbl_micLevel_max->y = sl_micLevel->y + get_label_height(lbl_micLevel_max) * 3;
		lbl_micLevel_max->visible = VISIBLE;

		label_t * lbl_micClip_min = find_gui_element(TYPE_LABEL, win, "lbl_micClip_min");
		local_snprintf_P(lbl_micClip_min->text, ARRAY_SIZE(lbl_micClip_min->text), PSTR("%d"), clip_min);
		lbl_micClip_min->x = sl_micClip->x - get_label_width(lbl_micClip_min) / 2;
		lbl_micClip_min->y = sl_micClip->y + get_label_height(lbl_micClip_min) * 3;
		lbl_micClip_min->visible = VISIBLE;

		label_t * lbl_micClip_max = find_gui_element(TYPE_LABEL, win, "lbl_micClip_max");
		local_snprintf_P(lbl_micClip_max->text, ARRAY_SIZE(lbl_micClip_max->text), PSTR("%d"), clip_max);
		lbl_micClip_max->x = sl_micClip->x + sl_micClip->size - get_label_width(lbl_micClip_max) / 2;
		lbl_micClip_max->y = sl_micClip->y + get_label_height(lbl_micClip_max) * 3;
		lbl_micClip_max->visible = VISIBLE;

		label_t * lbl_micAGC_min = find_gui_element(TYPE_LABEL, win, "lbl_micAGC_min");
		local_snprintf_P(lbl_micAGC_min->text, ARRAY_SIZE(lbl_micAGC_min->text), PSTR("%d"), agc_min);
		lbl_micAGC_min->x = sl_micAGC->x - get_label_width(lbl_micAGC_min) / 2;
		lbl_micAGC_min->y = sl_micAGC->y + get_label_height(lbl_micAGC_min) * 3;
		lbl_micAGC_min->visible = VISIBLE;

		label_t * lbl_micAGC_max = find_gui_element(TYPE_LABEL, win, "lbl_micAGC_max");
		local_snprintf_P(lbl_micAGC_max->text, ARRAY_SIZE(lbl_micAGC_max->text), PSTR("%d"), agc_max);
		lbl_micAGC_max->x = sl_micAGC->x + sl_micClip->size - get_label_width(lbl_micAGC_max) / 2;
		lbl_micAGC_max->y = sl_micAGC->y + get_label_height(lbl_micAGC_max) * 3;
		lbl_micAGC_max->visible = VISIBLE;

		xmax = sl_micAGC->x + sl_micAGC->size + col1_int;
		ymax = bh1->y1 + bh1->h;
		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}

	if (is_moving_slider())
	{
		char buf [TEXT_ARRAY_SIZE];

		/* костыль через костыль */
		sl = (slider_t *) get_selected_element();

		if (sl == sl_micLevel)
		{
			uint_fast16_t level = level_min + normalize(sl->value, 0, 100, level_max - level_min);
			local_snprintf_P(lbl_micLevel->text, ARRAY_SIZE(lbl_micLevel->text), PSTR("Level: %3d"), level);
			hamradio_set_mik1level(level);
		}
		else if (sl == sl_micClip)
		{
			uint_fast16_t clip = clip_min + normalize(sl->value, 0, 100, clip_max - clip_min);
			local_snprintf_P(lbl_micClip->text, ARRAY_SIZE(lbl_micClip->text), PSTR("Clip : %3d"), clip);
			hamradio_set_gmikehclip(clip);
		}
		else if (sl == sl_micAGC)
		{
			uint_fast16_t agc = agc_min + normalize(sl->value, 0, 100, agc_max - agc_min);
			local_snprintf_P(lbl_micAGC->text, ARRAY_SIZE(lbl_micAGC->text), PSTR("AGC  : %3d"), agc);
			hamradio_set_gmikeagcgain(agc);
		}
	}
}

// *********************************************************************************************************************************************************************

static void buttons_ap_mic_prof_process(void)
{
	button_t * btn_profile = (button_t *) get_selected_element();
	uint_fast8_t profile_id = get_selected_element_pos();

	if (is_short_pressed())
	{
		if (btn_profile->payload)
		{
			hamradio_load_mic_profile(profile_id, 1);
		}
	}

	if (is_long_pressed())
	{
		if (btn_profile->payload)
		{
			hamradio_clean_mic_profile(profile_id);
			local_snprintf_P(btn_profile->text, ARRAY_SIZE(btn_profile->text), PSTR("Profile %d|empty"), profile_id + 1);
			btn_profile->payload = 0;
		}
		else
		{
			hamradio_save_mic_profile(profile_id);
			local_snprintf_P(btn_profile->text, ARRAY_SIZE(btn_profile->text), PSTR("Profile %d|saved"), profile_id + 1);
			btn_profile->payload = 1;
		}
	}
}

//static micprof_t micprof_cells [micprof_cells_count];
static void window_ap_mic_prof_process(void)
{
	window_t * win = get_win(WINDOW_AP_MIC_PROF);

	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 3;
		button_t * bh = NULL;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   				state,   	is_locked, is_long_press, parent,   	visible,      payload,	 	name, 				text
			{ 0, 0, 100, 44, buttons_ap_mic_prof_process, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_AP_MIC_PROF, 	NON_VISIBLE, INT32_MAX, "btn_mic_profile_1", "", },
			{ 0, 0, 100, 44, buttons_ap_mic_prof_process, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_AP_MIC_PROF, 	NON_VISIBLE, INT32_MAX, "btn_mic_profile_2", "", },
			{ 0, 0, 100, 44, buttons_ap_mic_prof_process, CANCELLED, BUTTON_NON_LOCKED, 1, WINDOW_AP_MIC_PROF, 	NON_VISIBLE, INT32_MAX, "btn_mic_profile_3", "", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 0, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr [i];
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
			uint_fast8_t cell_saved = hamradio_load_mic_profile(i, 0);
			local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Profile %d|%s"), i + 1, cell_saved ? "saved" : "empty");
			bh->payload = cell_saved;
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}
		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}
}

// *********************************************************************************************************************************************************************

static void labels_menu_handler (void)
{
	label_t * lh = get_selected_element();
	if (strcmp(lh->name, "lbl_group") == 0)
	{
		menu [MENU_GROUPS].selected_label = get_selected_element_pos() % (menu [MENU_GROUPS].num_rows + 1);
		menu_label_touched = 1;
		menu_level = MENU_GROUPS;
	}
	else if (strcmp(lh->name, "lbl_params") == 0)
	{
		menu [MENU_PARAMS].selected_label = get_selected_element_pos() % (menu [MENU_GROUPS].num_rows + 1);
		menu_label_touched = 1;
		menu_level = MENU_PARAMS;
	}
	else if (strcmp(lh->name, "lbl_vals") == 0)
	{
		menu [MENU_VALS].selected_label = get_selected_element_pos() % (menu [MENU_GROUPS].num_rows + 1);
		menu [MENU_PARAMS].selected_label = menu [MENU_VALS].selected_label;
		menu_label_touched = 1;
		menu_level = MENU_VALS;
	}
}

static void buttons_menu_handler(void)
{
	if (is_short_pressed())
	{
		window_t * win = get_win(WINDOW_MENU);
		button_t * btn1 = find_gui_element(TYPE_BUTTON, win, "btnSysMenu+");
		button_t * btn2 = find_gui_element(TYPE_BUTTON, win, "btnSysMenu-");

		if (get_selected_element() == btn1)
		{
			encoder2.rotate = 1;
		}
		else if (get_selected_element() == btn2)
		{
			encoder2.rotate = -1;
		}
	}
}

static void window_menu_process(void)
{
	static uint_fast8_t menu_is_scrolling = 0;
	uint_fast8_t int_cols = 230, int_rows = 35;
	static button_t * button_up = NULL, * button_down = NULL;
	window_t * win = get_win(WINDOW_MENU);
	int_fast8_t move_x, move_y;

	if (win->first_call)
	{
		uint_fast16_t xmax = 0, ymax = 0;
		win->first_call = 0;
		win->align_mode = ALIGN_CENTER_X;						// выравнивание окна системных настроек всегда по центру

		uint_fast8_t col1_int = 20, row1_int = window_title_height + 20, i;
		uint_fast16_t xn, yn;
		label_t * lh;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0, 40, 40, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, INT32_MAX, "btnSysMenu-", "-", },
			{ 0, 0, 40, 40, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, INT32_MAX, "btnSysMenu+", "+", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		label_t labels [] = {
		//    x, y,  parent,  state, is_trackable, visible,   name,  Text, font_size, 	color, 			onClickHandler
			{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group",  "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group",  "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group",  "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group",  "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group",  "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group",  "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals",   "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals",   "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals",   "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals",   "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals",   "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
			{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals",   "", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		ASSERT(win->lh_ptr != NULL);
		memcpy(win->lh_ptr, labels, labels_size);

		button_up = find_gui_element(TYPE_BUTTON, win, "btnSysMenu+");
		button_down = find_gui_element(TYPE_BUTTON, win, "btnSysMenu-");
		button_up->visible = NON_VISIBLE;
		button_down->visible = NON_VISIBLE;

		menu [MENU_GROUPS].add_id = 0;
		menu [MENU_GROUPS].selected_str = 0;
		menu [MENU_GROUPS].selected_label = 0;
		menu [MENU_PARAMS].add_id = 0;
		menu [MENU_PARAMS].selected_str = 0;
		menu [MENU_PARAMS].selected_label = 0;
		menu [MENU_VALS].add_id = 0;
		menu [MENU_VALS].selected_str = 0;
		menu [MENU_VALS].selected_label = 0;

		menu [MENU_GROUPS].first_id = 0;
		for (i = 0; i < win->lh_count; i++)
		{
			lh = & win->lh_ptr [i];
			if (strcmp(lh->name, "lbl_group"))
				break;
		}

		menu [MENU_GROUPS].last_id = --i;
		menu [MENU_GROUPS].num_rows = menu [MENU_GROUPS].last_id - menu [MENU_GROUPS].first_id;

		menu [MENU_PARAMS].first_id = ++i;
		for (; i < win->lh_count; i++)
		{
			lh = & win->lh_ptr [i];
			if (strcmp(lh->name, "lbl_params"))
				break;
		}
		menu [MENU_PARAMS].last_id = --i;
		menu [MENU_PARAMS].num_rows = menu [MENU_PARAMS].last_id - menu [MENU_PARAMS].first_id;

		menu [MENU_VALS].first_id = ++i;
		for (; i < win->lh_count; i++)
		{
			lh = & win->lh_ptr [i];
			if (strcmp(lh->name, "lbl_vals"))
				break;
		}
		menu [MENU_VALS].last_id = --i;
		menu [MENU_VALS].num_rows = menu [MENU_VALS].last_id - menu [MENU_VALS].first_id;

		menu [MENU_GROUPS].count = hamradio_get_multilinemenu_block_groups(menu [MENU_GROUPS].menu_block) - 1;
		xn = col1_int;
		yn = row1_int;
		for(i = 0; i <= menu [MENU_GROUPS].num_rows; i++)
		{
			lh = & win->lh_ptr [menu [MENU_GROUPS].first_id + i];
			strcpy(lh->text, menu [MENU_GROUPS].menu_block [i + menu [MENU_GROUPS].add_id].name);
			lh->visible = VISIBLE;
			lh->color = i == menu [MENU_GROUPS].selected_label ? COLORMAIN_BLACK : COLORMAIN_WHITE;
			lh->x = xn;
			lh->y = yn;
			yn += int_rows;
		}

		menu [MENU_PARAMS].count = hamradio_get_multilinemenu_block_params(menu [MENU_PARAMS].menu_block, menu [MENU_GROUPS].menu_block [menu [MENU_GROUPS].selected_str].index) - 1;
		xn += int_cols;
		yn = row1_int;
		for(i = 0; i <= menu [MENU_PARAMS].num_rows; i++)
		{
			lh = & win->lh_ptr [menu [MENU_PARAMS].first_id + i];
			strcpy(lh->text, menu [MENU_PARAMS].menu_block [i + menu [MENU_PARAMS].add_id].name);
			lh->visible = VISIBLE;
			lh->color = COLORMAIN_WHITE;
			lh->x = xn;
			lh->y = yn;
			yn += int_rows;
		}

		menu [MENU_VALS].count = menu [MENU_PARAMS].count < menu [MENU_VALS].num_rows ? menu [MENU_PARAMS].count : menu [MENU_VALS].num_rows;
		hamradio_get_multilinemenu_block_vals(menu [MENU_VALS].menu_block, menu [MENU_PARAMS].menu_block [menu [MENU_PARAMS].selected_str].index, menu [MENU_VALS].count);
		xn += int_cols;
		yn = row1_int;
		for(lh = NULL, i = 0; i <= menu [MENU_VALS].num_rows; i ++)
		{
			lh = & win->lh_ptr [menu [MENU_VALS].first_id + i];
			lh->x = xn;
			lh->y = yn;
			yn += int_rows;
			lh->visible = NON_VISIBLE;
			lh->color = COLORMAIN_WHITE;
			if (menu [MENU_VALS].count < i)
				continue;
			strcpy(lh->text, menu [MENU_VALS].menu_block [i + menu [MENU_VALS].add_id].name);
			lh->visible = VISIBLE;
		}

		menu_level = MENU_GROUPS;

		ASSERT(lh != NULL);
		xmax = lh->x + 100;
		ymax = lh->y + get_label_height(lh);
		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}

	get_gui_tracking(& move_x, & move_y);
	if (is_moving_label() && move_y != 0)
	{
		static uint_fast8_t start_str_group = 0, start_str_params = 0;
		if (! menu_is_scrolling)
		{
			start_str_group = menu [MENU_GROUPS].add_id;
			start_str_params = menu [MENU_PARAMS].add_id;
		}
		ldiv_t r = ldiv(move_y, int_rows);
		if (strcmp(((label_t *) get_selected_element())->name, "lbl_group") == 0)
		{
			int_fast8_t q = start_str_group - r.quot;
			menu [MENU_GROUPS].add_id = q <= 0 ? 0 : q;
			menu [MENU_GROUPS].add_id = (menu [MENU_GROUPS].add_id + menu [MENU_GROUPS].num_rows) > menu [MENU_GROUPS].count ?
					(menu [MENU_GROUPS].count - menu [MENU_GROUPS].num_rows) : menu [MENU_GROUPS].add_id;
			menu [MENU_GROUPS].selected_str = menu [MENU_GROUPS].selected_label + menu [MENU_GROUPS].add_id;
			menu_level = MENU_GROUPS;
			menu [MENU_PARAMS].add_id = 0;
			menu [MENU_PARAMS].selected_str = 0;
			menu [MENU_PARAMS].selected_label = 0;
			menu [MENU_VALS].add_id = 0;
			menu [MENU_VALS].selected_str = 0;
			menu [MENU_VALS].selected_label = 0;
		}
		else if (strcmp(((label_t *) get_selected_element())->name, "lbl_params") == 0 &&
				menu [MENU_PARAMS].count > menu [MENU_PARAMS].num_rows)
		{
			int_fast8_t q = start_str_params - r.quot;
			menu [MENU_PARAMS].add_id = q <= 0 ? 0 : q;
			menu [MENU_PARAMS].add_id = (menu [MENU_PARAMS].add_id + menu [MENU_PARAMS].num_rows) > menu [MENU_PARAMS].count ?
					(menu [MENU_PARAMS].count - menu [MENU_PARAMS].num_rows) : menu [MENU_PARAMS].add_id;
			menu [MENU_PARAMS].selected_str = menu [MENU_PARAMS].selected_label + menu [MENU_PARAMS].add_id;
			menu [MENU_VALS].add_id = menu [MENU_PARAMS].add_id;
			menu [MENU_VALS].selected_str = menu [MENU_PARAMS].selected_str;
			menu [MENU_VALS].selected_label = menu [MENU_PARAMS].selected_label;
			menu_level = MENU_PARAMS;
		}
		menu_is_scrolling = 1;
	}

	if (! is_moving_label() && menu_is_scrolling)
	{
		menu_is_scrolling = 0;
		reset_tracking();
	}

	if (! encoder2.press_done || menu_label_touched || menu_is_scrolling)
	{
		// выход из режима редактирования параметра  - краткое или длинное нажатие на энкодер
		if (encoder2.press && menu_level == MENU_VALS)
		{
			menu_level = MENU_PARAMS;
			encoder2.press = 0;
		}
		if (encoder2.press)
			menu_level = ++menu_level > MENU_VALS ? MENU_VALS : menu_level;
		if (encoder2.hold)
		{
			menu_level = --menu_level == MENU_OFF ? MENU_OFF : menu_level;
			if (menu_level == MENU_GROUPS)
			{
				menu [MENU_PARAMS].add_id = 0;
				menu [MENU_PARAMS].selected_str = 0;
				menu [MENU_PARAMS].selected_label = 0;
				menu [MENU_VALS].add_id = 0;
				menu [MENU_VALS].selected_str = 0;
				menu [MENU_VALS].selected_label = 0;
			}
		}

		// при переходе между уровнями пункты меню выделяется цветом
		label_t * lh = NULL;
		if (menu_level == MENU_VALS)
		{
			menu [MENU_VALS].selected_label = menu [MENU_PARAMS].selected_label;
			lh = & win->lh_ptr [menu [MENU_VALS].first_id + menu [MENU_VALS].selected_label];

			button_down->visible = VISIBLE;
			button_down->x1 = lh->x - button_down->w - 10;
			button_down->y1 = (lh->y + get_label_height(lh) / 2) - (button_down->h / 2);

			button_up->visible = VISIBLE;
			button_up->x1 = lh->x + get_label_width(lh) + 10;
			button_up->y1 = button_down->y1;
			for (uint_fast8_t i = 0; i <= menu [MENU_GROUPS].num_rows; i++)
			{
				lh = & win->lh_ptr [menu [MENU_GROUPS].first_id + i];
				lh->color = i == menu [MENU_GROUPS].selected_label ? COLORMAIN_BLACK : COLORMAIN_GRAY;

				lh = & win->lh_ptr [menu [MENU_PARAMS].first_id + i];
				lh->color = i == menu [MENU_PARAMS].selected_label ? COLORMAIN_BLACK : COLORMAIN_GRAY;

				lh = & win->lh_ptr [menu [MENU_VALS].first_id + i];
				lh->color = i == menu [MENU_PARAMS].selected_label ? COLORMAIN_YELLOW : COLORMAIN_GRAY;
			}
			menu_label_touched = 0;
		}
		else if (menu_level == MENU_PARAMS)
		{
			button_down->visible = NON_VISIBLE;
			button_up->visible = NON_VISIBLE;
			for (uint_fast8_t i = 0; i <= menu [MENU_GROUPS].num_rows; i++)
			{
				lh = & win->lh_ptr [menu [MENU_GROUPS].first_id + i];
				lh->color = i == menu [MENU_GROUPS].selected_label ? COLORMAIN_BLACK : COLORMAIN_GRAY;

				lh = & win->lh_ptr [menu [MENU_PARAMS].first_id + i];
				lh->color = i == menu [MENU_PARAMS].selected_label ? COLORMAIN_BLACK : COLORMAIN_WHITE;

				lh = & win->lh_ptr [menu [MENU_VALS].first_id + i];
				lh->color = COLORMAIN_WHITE;
			}
		}
		else if (menu_level == MENU_GROUPS)
		{
			button_down->visible = NON_VISIBLE;
			button_up->visible = NON_VISIBLE;
			for (uint_fast8_t i = 0; i <= menu [MENU_GROUPS].num_rows; i++)
			{
				lh = & win->lh_ptr [menu [MENU_GROUPS].first_id + i];
				lh->color = i == menu [MENU_GROUPS].selected_label ? COLORMAIN_BLACK : COLORMAIN_WHITE;

				lh = & win->lh_ptr [menu [MENU_PARAMS].first_id + i];
				lh->color = COLORMAIN_WHITE;

				lh = & win->lh_ptr [menu [MENU_VALS].first_id + i];
				lh->color = COLORMAIN_WHITE;
			}
		}

		encoder2.press = 0;
		encoder2.hold = 0;
		encoder2.press_done = 1;
	}

	if (menu_level == MENU_OFF)
	{
		if (win->parent_id != UINT8_MAX)
		{
			close_window(OPEN_PARENT_WINDOW);
			return;
		} else
			menu_level = MENU_GROUPS;
	}

	if (encoder2.rotate != 0 && menu_level == MENU_VALS)
	{
		encoder2.rotate_done = 1;
		menu [MENU_PARAMS].selected_str = menu [MENU_PARAMS].selected_label + menu [MENU_PARAMS].add_id;
		label_t * lh = & win->lh_ptr [menu [MENU_VALS].first_id + menu [MENU_PARAMS].selected_label];
		strcpy(lh->text, hamradio_gui_edit_menu_item(menu [MENU_PARAMS].menu_block [menu [MENU_PARAMS].selected_str].index, encoder2.rotate));

		lh = & win->lh_ptr [menu [MENU_VALS].first_id + menu [MENU_VALS].selected_label];
		button_up->x1 = lh->x + get_label_width(lh) + 10;
	}

	if ((menu_label_touched || menu_is_scrolling || encoder2.rotate != 0) && menu_level != MENU_VALS)
	{
		encoder2.rotate_done = 1;

		if (encoder2.rotate != 0)
		{
			menu [menu_level].selected_str = (menu [menu_level].selected_str + encoder2.rotate) <= 0 ? 0 : (menu [menu_level].selected_str + encoder2.rotate);
			menu [menu_level].selected_str = menu [menu_level].selected_str > menu [menu_level].count ? menu [menu_level].count : menu [menu_level].selected_str;
		}
		else if (menu_label_touched)
			menu [menu_level].selected_str = menu [menu_level].selected_label + menu [menu_level].add_id;

		menu [MENU_PARAMS].count = hamradio_get_multilinemenu_block_params(menu [MENU_PARAMS].menu_block, menu [MENU_GROUPS].menu_block [menu [MENU_GROUPS].selected_str].index) - 1;

		if (encoder2.rotate > 0)
		{
			// указатель подошел к нижней границе списка
			if (++menu [menu_level].selected_label > (menu [menu_level].count < menu [menu_level].num_rows ? menu [menu_level].count : menu [menu_level].num_rows))
			{
				menu [menu_level].selected_label = (menu [menu_level].count < menu [menu_level].num_rows ? menu [menu_level].count : menu [menu_level].num_rows);
				menu [menu_level].add_id = menu [menu_level].selected_str - menu [menu_level].selected_label;
			}
		}
		if (encoder2.rotate < 0)
		{
			// указатель подошел к верхней границе списка
			if (--menu [menu_level].selected_label < 0)
			{
				menu [menu_level].selected_label = 0;
				menu [menu_level].add_id = menu [menu_level].selected_str;
			}
		}

		if (menu_level == MENU_GROUPS)
			for(uint_fast8_t i = 0; i <= menu [MENU_GROUPS].num_rows; i++)
			{
				label_t * l = & win->lh_ptr [menu [MENU_GROUPS].first_id + i];
				strcpy(l->text, menu [MENU_GROUPS].menu_block [i + menu [MENU_GROUPS].add_id].name);
				l->color = i == menu [MENU_GROUPS].selected_label ? COLORMAIN_BLACK : COLORMAIN_WHITE;
			}

		menu [MENU_VALS].count = menu [MENU_PARAMS].count < menu [MENU_VALS].num_rows ? menu [MENU_PARAMS].count : menu [MENU_VALS].num_rows;
		hamradio_get_multilinemenu_block_vals(menu [MENU_VALS].menu_block,  menu [MENU_PARAMS].menu_block [menu [MENU_PARAMS].add_id].index, menu [MENU_VALS].count);

		for(uint_fast8_t i = 0; i <= menu [MENU_PARAMS].num_rows; i++)
		{
			label_t * lp = & win->lh_ptr [menu [MENU_PARAMS].first_id + i];
			label_t * lv = & win->lh_ptr [menu [MENU_VALS].first_id + i];

			lp->visible = NON_VISIBLE;
			lp->state = DISABLED;
			lv->visible = NON_VISIBLE;
			lv->state = DISABLED;
			if (i > menu [MENU_PARAMS].count)
				continue;
			strcpy(lp->text, menu [MENU_PARAMS].menu_block [i + menu [MENU_PARAMS].add_id].name);
			strcpy(lv->text, menu [MENU_VALS].menu_block [i].name);
			lp->color = i == menu [MENU_PARAMS].selected_label && menu_level > MENU_GROUPS ? COLORMAIN_BLACK : COLORMAIN_WHITE;
			lp->visible = VISIBLE;
			lp->state = CANCELLED;
			lv->visible = VISIBLE;
			lv->state = CANCELLED;
		}
		menu_label_touched = 0;
	}

	label_t * lh;
	switch (menu_level)
	{
	case MENU_PARAMS:
	case MENU_VALS:
		lh = & win->lh_ptr [menu [MENU_PARAMS].first_id + menu [MENU_PARAMS].selected_label];
		colpip_rect(colmain_fb_draw(), DIM_X, DIM_Y, win->x1 + lh->x - 5, win->y1 + lh->y - 5, win->x1 + lh->x + int_cols - 20,
				win->y1 + lh->y + get_label_height(lh) + 5, GUI_MENUSELECTCOLOR, 1);

	case MENU_GROUPS:
		lh = & win->lh_ptr [menu [MENU_GROUPS].first_id + menu [MENU_GROUPS].selected_label];
		colpip_rect(colmain_fb_draw(), DIM_X, DIM_Y, win->x1 + lh->x - 5, win->y1 + lh->y - 5, win->x1 + lh->x + int_cols - 20,
				win->y1 + lh->y + get_label_height(lh) + 5, GUI_MENUSELECTCOLOR, 1);
	}
}

// *********************************************************************************************************************************************************************

void gui_uif_editmenu(const char * name, uint_fast16_t menupos, uint_fast8_t exitkey)
{
	window_t * win = get_win(WINDOW_UIF);
	if (win->state == NON_VISIBLE)
	{
		close_window(DONT_OPEN_PARENT_WINDOW);
		open_window(win);
		footer_buttons_state(DISABLED, NULL);
		strcpy(menu_uif.name, name);
		menu_uif.menupos = menupos;
		menu_uif.exitkey = exitkey;
	}
	else if (win->state == VISIBLE)
	{
		close_window(DONT_OPEN_PARENT_WINDOW);
		footer_buttons_state(CANCELLED);
	}
}

static void buttons_uif_handler(void)
{
	if (is_short_pressed())
	{
		window_t * win = get_win(WINDOW_UIF);
		button_t * btn1 = find_gui_element(TYPE_BUTTON, win, "btnUIF+");
		button_t * btn2 = find_gui_element(TYPE_BUTTON, win, "btnUIF-");
		if (get_selected_element() == btn1)
		{
			encoder2.rotate = 1;
		}
		else if (get_selected_element() == btn2)
		{
			encoder2.rotate = -1;
		}
	}
}

static void window_uif_process(void)
{
	static label_t * lbl_uif_val;
	static button_t * button_up, * button_down;
	static uint_fast16_t window_center_x;
	static uint_fast8_t reinit = 0;
	window_t * win = get_win(WINDOW_UIF);

	if (win->first_call)
	{
		win->first_call = 0;
		reinit = 1;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0, 40, 40, buttons_uif_handler,  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_UIF, NON_VISIBLE, INT32_MAX, "btnUIF-", "-", },
			{ 0, 0, 40, 40, buttons_uif_handler,  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_UIF, NON_VISIBLE, INT32_MAX, "btnUIF+",  "+", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		ASSERT(win->bh_ptr != NULL);
		memcpy(win->bh_ptr, buttons, buttons_size);

		label_t labels [] = {
		//    x, y,  parent,  state, is_trackable, visible,   name,  Text, font_size, 	color, 	 onClickHandler
			{ 0, 0,	WINDOW_UIF,  DISABLED,  0, NON_VISIBLE, "lbl_uif_param", "", FONT_LARGE, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_UIF,  DISABLED,  0, NON_VISIBLE, "lbl_uif_val", 	 "", FONT_LARGE, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		ASSERT(win->lh_ptr != NULL);
		memcpy(win->lh_ptr, labels, labels_size);

		elements_state(win);
	}

	if (reinit)
	{
		reinit = 0;
		uint_fast8_t col1_int = 20, row1_int = window_title_height + 20;
		strcpy(win->name, menu_uif.name);

		button_down = find_gui_element(TYPE_BUTTON, win, "btnUIF-");
		button_up = find_gui_element(TYPE_BUTTON, win, "btnUIF+");
		lbl_uif_val = find_gui_element(TYPE_LABEL, win, "lbl_uif_val");

		const char * v = hamradio_gui_edit_menu_item(menu_uif.menupos, 0);
		strcpy(lbl_uif_val->text, v);

		button_down->x1 = col1_int;
		button_down->y1 = row1_int;
		button_down->visible = VISIBLE;

		button_up->x1 = button_down->x1 + button_down->w + 30 + get_label_width(lbl_uif_val);
		button_up->y1 = button_down->y1;
		button_up->visible = VISIBLE;

		window_center_x = (col1_int + button_up->x1 + button_up->w) / 2;

		lbl_uif_val->x = window_center_x - get_label_width(lbl_uif_val) / 2;
		lbl_uif_val->y = row1_int + button_up->h / 2 - get_label_height(lbl_uif_val) / 2;
		lbl_uif_val->visible = VISIBLE;

		uint_fast16_t xmax = button_up->x1 + button_up->w;
		uint_fast16_t ymax = button_up->y1 + button_up->h;
		calculate_window_position(win, xmax, ymax);

		hamradio_enable_keyboard_redirect();
		return;
	}

	if (encoder2.rotate != 0)
	{
		hamradio_gui_edit_menu_item(menu_uif.menupos, encoder2.rotate);

		reinit = 1;
		encoder2.rotate_done = 1;
		gui_timer_update(NULL);
	}

	uint_fast8_t keyb_code = get_gui_keyb_code();
	if (keyb_code != KBD_CODE_MAX)
	{
		if (keyb_code == menu_uif.exitkey)
		{
			hamradio_disable_keyboard_redirect();
			close_window(DONT_OPEN_PARENT_WINDOW);
			footer_buttons_state(CANCELLED);
		}
	}
}

// *********************************************************************************************************************************************************************

void gui_check_encoder2 (int_least16_t rotate)
{
	if (encoder2.rotate_done || encoder2.rotate == 0)
	{
		encoder2.rotate = rotate;
		encoder2.rotate_done = 0;
	}
}

void gui_set_encoder2_state (uint_fast8_t code)
{
	if (code == KBD_ENC2_PRESS)
		encoder2.press = 1;
	if (code == KBD_ENC2_HOLD)
		encoder2.hold = 1;
	encoder2.press_done = 0;
}

void gui_encoder2_menu (enc2_menu_t * enc2_menu)
{
	window_t * win = get_win(WINDOW_ENC2);

	if (win->state == NON_VISIBLE && enc2_menu->state != 0)
	{
		close_window(DONT_OPEN_PARENT_WINDOW);
		open_window(win);
		footer_buttons_state(DISABLED, NULL);
		gui_enc2_menu = enc2_menu;
	}
	else if (win->state == VISIBLE && enc2_menu->state == 0)
	{
		close_window(DONT_OPEN_PARENT_WINDOW);
		gui_enc2_menu = NULL;
		footer_buttons_state(CANCELLED);
	}
}

static void window_enc2_process(void)
{
	static label_t * lbl_param,  * lbl_val;
	window_t * win = get_win(WINDOW_ENC2);
	uint_fast8_t row1_int = window_title_height + 20;

	if (win->first_call)
	{
		win->first_call = 0;

		label_t labels [] = {
		//    x, y,  parent,  state, is_trackable, visible,   name,   Text, font_size, 	color,  onClickHandler
			{ 0, 0, WINDOW_ENC2,  DISABLED,  0, NON_VISIBLE, "lbl_enc2_val", "", FONT_LARGE, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		ASSERT(win->lh_ptr != NULL);
		memcpy(win->lh_ptr, labels, labels_size);

		elements_state(win);
	}

	if (gui_enc2_menu->updated)
	{
		lbl_val = find_gui_element(TYPE_LABEL, win, "lbl_enc2_val");
		strcpy(win->name, gui_enc2_menu->param);
		remove_end_line_spaces(win->name);
		calculate_window_position(win, 0, window_title_height + get_label_height(lbl_val) * 2);

		strcpy(lbl_val->text, gui_enc2_menu->val);
		lbl_val->x = win->w / 2 - get_label_width(lbl_val) / 2;
		lbl_val->y = row1_int;
		lbl_val->color = gui_enc2_menu->state == 2 ? COLORMAIN_YELLOW : COLORMAIN_WHITE;
		lbl_val->visible = VISIBLE;

		gui_enc2_menu->updated = 0;
		gui_timer_update(NULL);
	}
}

// *********************************************************************************************************************************************************************

void gui_open_sys_menu(void)
{
	window_t * win = get_win(WINDOW_MENU);
	static uint_fast8_t backup_parent = UINT8_MAX;

	if (check_for_parent_window() == UINT8_MAX && win->parent_id != UINT8_MAX)
	{
		backup_parent = win->parent_id;		// для возможности открытия окна напрямую временно "обнулить" parent_id
		win->parent_id = UINT8_MAX;
		open_window(win);
		footer_buttons_state(DISABLED, NULL);
	}
	else if (check_for_parent_window() == WINDOW_MENU && win->parent_id == UINT8_MAX)
	{
		close_window(OPEN_PARENT_WINDOW);
		footer_buttons_state(CANCELLED);
		win->parent_id = backup_parent;
		backup_parent = UINT8_MAX;
	}
}

// *********************************************************************************************************************************************************************
#endif /* WITHTOUCHGUI */
