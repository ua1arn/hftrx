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
#include "codecs.h"

#include "src/gui/gui.h"

#if WITHTOUCHGUI && WITHGUISTYLE_MINI		// версия GUI для разрешения 480х272

#include "src/gui/gui_user.h"
#include "src/gui/gui_system.h"
#include "src/gui/gui_structs.h"
#include "src/gui/gui_settings.h"

static void minigui_main_process(void);

static window_t windows [] = {
//     window_id,   		 parent_id, 			align_mode,     x1, y1, w, h,   title,     		is_show, first_call, is_close, onVisibleProcess
	{ WINDOW_MAIN, 			 NO_PARENT_WINDOW, 		ALIGN_LEFT_X,	0, 0, 0, 0, "",  	   	   			 NON_VISIBLE, 0, 0, minigui_main_process, },
};


/* Возврат ссылки на окно */
window_t * get_win(uint8_t window_id)
{
	ASSERT(window_id < WINDOWS_COUNT);
	return & windows [window_id];
}

void gui_user_actions_ater_close_window(void)
{

}

// *********************************************************************************************************************************************************************

static void btn_main_handler(void)
{

}

void minigui_main_process(void)
{
	window_t * win = get_win(WINDOW_MAIN);
	static uint_fast32_t freq;

	if (win->first_call)
	{
		freq = hamradio_get_freq_rx();

		uint_fast8_t interval_btn = 3;
		uint_fast16_t x = 0;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_long_press, parent,   	visible,      payload,	 name, 		text
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_1", 	"1", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_2",  	"2", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_3", 	"3", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, INT32_MAX, "btn_4",  	"4", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		GUI_MEM_ASSERT(win->bh_ptr);
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

		return;
	}

	hamradio_set_freq(freq += 100);

}

// *********************************************************************************************************************************************************************

#endif
