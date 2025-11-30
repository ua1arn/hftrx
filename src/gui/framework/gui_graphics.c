/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "../gui_port_include.h"

#if WITHTOUCHGUI

#include "gui.h"
#include "gui_system.h"
#include "gui_structs.h"
#include "gui_settings.h"
#include "gui_windows.h"
#include "gui_fonts.h"
#include "../gui_port.h"

uint16_t gui_get_window_draw_width(void)
{
	window_t * win = get_win(get_parent_window());
	return win->draw_x2 - win->draw_x1;
}

uint16_t gui_get_window_draw_height(void)
{
	window_t * win = get_win(get_parent_window());
	return win->draw_y2 - win->draw_y1;
}

// Нарисовать линию в границах окна
void gui_drawline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, gui_color_t color)
{
	window_t * win = get_win(get_current_drawing_window());
	const gui_drawbuf_t * gdb = __gui_get_drawbuf();

	const uint16_t xn = x1 + win->draw_x1;
	const uint16_t yn = y1 + win->draw_y1;
	const uint16_t xk = x2 + win->draw_x1;
	const uint16_t yk = y2 + win->draw_y1;

	ASSERT(xn < win->draw_x2);
	ASSERT(xk < win->draw_x2);
	ASSERT(yn < win->draw_y2);
	ASSERT(yk < win->draw_y2);

	__gui_draw_line(gdb, xn, yn, xk, yk, color);
}

void gui_drawrect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, gui_color_t color, uint8_t fill)
{
	window_t * win = get_win(get_current_drawing_window());
	const gui_drawbuf_t * gdb = __gui_get_drawbuf();

	const uint16_t xn = x1 + win->draw_x1;
	const uint16_t yn = y1 + win->draw_y1;
	const uint16_t xk = x2 + win->draw_x1;
	const uint16_t yk = y2 + win->draw_y1;

	ASSERT(xn < win->draw_x2);
	ASSERT(xk < win->draw_x2);
	ASSERT(yn < win->draw_y2);
	ASSERT(yk < win->draw_y2);

	__gui_draw_rect(gdb, xn, yn, xk, yk, color, fill);
}

void gui_drawrect_rounded(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t radius, gui_color_t color, uint8_t fill)
{
	window_t * win = get_win(get_current_drawing_window());
	const gui_drawbuf_t * gdb = __gui_get_drawbuf();

	const uint16_t xn = x1 + win->draw_x1;
	const uint16_t yn = y1 + win->draw_y1;
	const uint16_t xk = x2 + win->draw_x1;
	const uint16_t yk = y2 + win->draw_y1;

	ASSERT(xn < win->draw_x2);
	ASSERT(xk < win->draw_x2);
	ASSERT(yn < win->draw_y2);
	ASSERT(yk < win->draw_y2);

	__gui_draw_rounded_rect(gdb, xn, yn, xk, yk, radius, color, fill);
}

void gui_drawrect_transparent(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t alpha)
{
	window_t * win = get_win(get_current_drawing_window());
	const gui_drawbuf_t * gdb = __gui_get_drawbuf();

	const uint16_t xn = x1 + win->draw_x1;
	const uint16_t yn = y1 + win->draw_y1;
	const uint16_t xk = x2 + win->draw_x1;
	const uint16_t yk = y2 + win->draw_y1;

	ASSERT(xn < win->draw_x2);
	ASSERT(xk < win->draw_x2);
	ASSERT(yn < win->draw_y2);
	ASSERT(yk < win->draw_y2);

	__gui_draw_semitransparent_rect(gdb, xn, yn, xk, yk, alpha);
}

void gui_drawpoint(uint16_t x1, uint16_t y1, gui_color_t color)
{
	window_t * win = get_win(get_current_drawing_window());
	const gui_drawbuf_t * gdb = __gui_get_drawbuf();

	const uint16_t xp = x1 + win->draw_x1;
	const uint16_t yp = y1 + win->draw_y1;

	ASSERT(xp < win->draw_x2);
	ASSERT(xp < win->draw_x2);

	__gui_draw_point(gdb, xp, yp, color);
}



void gui_drawDashedRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t dashLength, gui_color_t color)
{
    if (width == 0 || height == 0 || dashLength == 0) return;

    const gui_drawbuf_t * gdb = __gui_get_drawbuf();
    uint16_t x1 = x;
    uint16_t y1 = y;
    uint16_t x2 = x + width - 1;
    uint16_t y2 = y + height - 1;
    uint16_t pos = 0;

    // Верхняя линия (слева направо)
    for (uint16_t i = 0; i < width; i ++)
        if ((pos ++ / dashLength) % 2 == 0)
        	__gui_draw_point(gdb, x1 + i, y1, color);

    // Правая линия (сверху вниз)
    for (uint16_t i = 1; i < height; i ++)
        if ((pos ++ / dashLength) % 2 == 0)
        	__gui_draw_point(gdb, x2, y1 + i, color);

    // Нижняя линия (справа налево)
    for (uint16_t i = 1; i < width; i ++)
        if ((pos ++ / dashLength) % 2 == 0)
        	__gui_draw_point(gdb, x2 - i, y2, color);

    // Левая линия (снизу вверх)
    for (uint16_t i = 1; i < height - 1; i ++)
        if ((pos ++ / dashLength) % 2 == 0)
        	__gui_draw_point(gdb, x1, y2 - i, color);
}

void gui_print_mono(uint16_t x, uint16_t y, const char * text, const gui_mono_font_t * font, gui_color_t color)
{
	window_t * win = get_win(get_current_drawing_window());
	const gui_drawbuf_t * gdb = __gui_get_drawbuf();

	const uint16_t xn = x + win->draw_x1;
	const uint16_t yn = y + win->draw_y1;

	__gui_draw_string_mono(gdb, xn, yn,	text, font, color);
}

void gui_print_prop(uint16_t x, uint16_t y, const char * text, const gui_prop_font_t * font, gui_color_t color)
{
	window_t * win = get_win(get_current_drawing_window());
	const gui_drawbuf_t * gdb = __gui_get_drawbuf();

	const uint16_t xn = x + win->draw_x1;
	const uint16_t yn = y + win->draw_y1;

	__gui_draw_string_prop(gdb, xn, yn,	text, font, color);
}

#endif /* WITHTOUCHGUI */
