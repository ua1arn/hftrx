#ifndef GUI_PORT_H_INCLUDED
#define GUI_PORT_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI

typedef PACKEDCOLORPIP_T 	gui_color_t;
typedef gxdrawb_t 			gui_drawbuf_t;

static const gui_drawbuf_t * drawbuf = NULL;

static inline void __gui_set_drawbuf(const gui_drawbuf_t * buf)
{
	drawbuf = buf;
}

static inline const gui_drawbuf_t * __gui_get_drawbuf(void)
{
	return drawbuf;
}

// Инициализация буфера, если это необходимо в портируемом окружении
static inline void __gui_drawbuf_init(gui_drawbuf_t * buf, gui_color_t * extbuf,
		unsigned int w, unsigned int h)
{
	gxdrawb_initialize(buf, extbuf, w, h);
}

static inline void __gui_drawbuf_copy(const gui_drawbuf_t * dstbuf, gui_drawbuf_t * srcbuf,
		unsigned int dst_x, unsigned int dst_y, unsigned int src_w, unsigned int src_h)
{
	colpip_bitblt(
			dstbuf->cachebase, dstbuf->cachesize,	// cache parameters
			dstbuf, 	// target window
			dst_x, dst_y,	// target position
			srcbuf->cachebase, srcbuf->cachesize, 	// cache parameters
			srcbuf, 0, 0, src_w, src_h, BITBLT_FLAG_NONE, 0);
}

// Отрисовка закрашенного прямоугольника
static inline void __gui_draw_rect(const gui_drawbuf_t * buf, unsigned int x, unsigned int y,
		unsigned int w, unsigned int h, gui_color_t color, unsigned int fill)
{
	colpip_rect(buf, x, y, x + w, y + h, color, fill);
}

// Отрисовка прямоугольника со скругленными углами
static inline void __gui_draw_rounded_rect(const gui_drawbuf_t * buf, unsigned int x, unsigned int y,
		unsigned int w, unsigned int h, unsigned int radius, gui_color_t color, unsigned int fill)
{
	colmain_rounded_rect(buf, x, y, x + w, y + h, radius, color, fill);
}

static inline void __gui_draw_line(const gui_drawbuf_t * buf, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, gui_color_t color)
{
	colpip_line(buf, x1, y1, x2, y2, color, 1);
}


#endif /* WITHTOUCHGUI */

#endif /* GUI_PORT_H_INCLUDED */
