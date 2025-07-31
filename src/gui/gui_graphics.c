/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "hardware.h"
#include "formats.h"
#include "display/display.h"
#include "display/fontmaps.h"

#if WITHTOUCHGUI

#include "gui.h"
#include "gui_system.h"
#include "gui_structs.h"
#include "gui_settings.h"
#include "gui_windows.h"

void gui_drawstring(uint_fast16_t x, uint_fast16_t y, const char * str, font_size_t font, COLORPIP_T color)
{
	window_t * win = get_win(check_for_parent_window());
	const gxdrawb_t * gdb = gui_get_drawbuf();

	const uint_fast16_t x1 = x + win->draw_x1;
	const uint_fast16_t y1 = y + win->draw_y1;

	ASSERT(x1 < win->draw_x2);
	ASSERT(y1 < win->draw_y2);

	if (font == FONT_LARGE)
		colpip_string_tbg(gdb,  x1, y1, str, color);
	else if (font == FONT_MEDIUM)
		colpip_string2_tbg(gdb, x1, y1, str, color);
	else if (font == FONT_SMALL)
		colpip_string3_tbg(gdb, x1, y1, str, color);
}

uint_fast16_t gui_get_window_draw_width(void)
{
	window_t * win = get_win(check_for_parent_window());
	return win->draw_x2 - win->draw_x1;
}

uint_fast16_t gui_get_window_draw_height(void)
{
	window_t * win = get_win(check_for_parent_window());
	return win->draw_y2 - win->draw_y1;
}

// Нарисовать линию в границах окна
void gui_drawline(uint_fast16_t x1, uint_fast16_t y1, uint_fast16_t x2, uint_fast16_t y2, COLORPIP_T color)
{
	window_t * win = get_win(check_for_parent_window());
	const gxdrawb_t * gdb = gui_get_drawbuf();

	const uint_fast16_t xn = x1 + win->draw_x1;
	const uint_fast16_t yn = y1 + win->draw_y1;
	const uint_fast16_t xk = x2 + win->draw_x1;
	const uint_fast16_t yk = y2 + win->draw_y1;

	ASSERT(xn < win->draw_x2);
	ASSERT(xk < win->draw_x2);
	ASSERT(yn < win->draw_y2);
	ASSERT(yk < win->draw_y2);

	colpip_line(gdb, xn, yn, xk, yk, color, 1);
}

void gui_drawrect(uint_fast16_t x1, uint_fast16_t y1, uint_fast16_t x2, uint_fast16_t y2, COLORPIP_T color, uint_fast8_t fill)
{
	window_t * win = get_win(check_for_parent_window());
	const gxdrawb_t * gdb = gui_get_drawbuf();

	const uint_fast16_t xn = x1 + win->draw_x1;
	const uint_fast16_t yn = y1 + win->draw_y1;
	const uint_fast16_t xk = x2 + win->draw_x1;
	const uint_fast16_t yk = y2 + win->draw_y1;

	ASSERT(xn < win->draw_x2);
	ASSERT(xk < win->draw_x2);
	ASSERT(yn < win->draw_y2);
	ASSERT(yk < win->draw_y2);

	colpip_rect(gdb, xn, yn, xk, yk, color, fill);
}

void gui_drawpoint(uint_fast16_t x1, uint_fast16_t y1, COLORPIP_T color)
{
	window_t * win = get_win(check_for_parent_window());
	const gxdrawb_t * gdb = gui_get_drawbuf();

	const uint_fast16_t xp = x1 + win->draw_x1;
	const uint_fast16_t yp = y1 + win->draw_y1;


	ASSERT(xp < win->draw_x2);
	ASSERT(xp < win->draw_x2);

	colpip_point(gdb, xp, yp, color);
}

void gui_print_UB(uint16_t x, uint16_t y, const char * text, const UB_Font * font, COLORPIP_T color)
{
	window_t * win = get_win(check_for_parent_window());
	const gxdrawb_t * gdb = gui_get_drawbuf();

	const uint_fast16_t xn = x + win->draw_x1;
	const uint_fast16_t yn = y + win->draw_y1;

	UB_Font_DrawString(gdb, xn, yn,	text, font, color);
}

#endif /* WITHTOUCHGUI */
