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
	editfreq_t * ef = get_editfreq_env();
	if (bh->parent == WINDOW_FREQ && ef->key == BUTTON_CODE_DONE)
		ef->key = bh->payload;
}

void window_freq_process (void)
{
	static label_t * lbl_freq;
	static editfreq_t * ef;
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

		ef = get_editfreq_env();
		ef->val = 0;
		ef->num = 0;
		ef->key = BUTTON_CODE_DONE;

		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}

	if (ef->key != BUTTON_CODE_DONE)
	{
		if (lbl_freq->color == COLORMAIN_RED)
		{
			ef->val = 0;
			ef->num = 0;
		}

		lbl_freq->color = COLORMAIN_WHITE;
		char buf[TEXT_ARRAY_SIZE];
		switch (ef->key)
		{
		case BUTTON_CODE_BK:
			if (ef->num > 0)
			{
				ef->val /= 10;
				ef->num --;
			}
			break;

		case BUTTON_CODE_OK:
			if(hamradio_set_freq(ef->val * 1000))
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
			if (ef->num < 6)
			{
				ef->val  = ef->val * 10 + ef->key;
				if (ef->val)
					ef->num ++;
			}
		}
		ef->key = BUTTON_CODE_DONE;
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%6d k"), ef->val);
		strcpy(lbl_freq->text, buf);
	}
}


#endif /* WITHTOUCHGUI */
