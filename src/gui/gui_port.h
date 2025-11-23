#ifndef GUI_PORT_H_INCLUDED
#define GUI_PORT_H_INCLUDED

#if WITHTOUCHGUI

typedef PACKEDCOLORPIP_T 	gui_color_t;
typedef gxdrawb_t 			gui_drawbuf_t;
typedef UB_Font32			gui_mono_font_t;
typedef UB_pFont32			gui_prop_font_t;

extern const gui_drawbuf_t * drawbuf;

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

static inline void __gui_draw_line(const gui_drawbuf_t * buf, unsigned int x1, unsigned int y1,
		unsigned int x2, unsigned int y2, gui_color_t color)
{
	colpip_line(buf, x1, y1, x2, y2, color, 1);
}

static inline void __gui_draw_point(const gui_drawbuf_t * buf, unsigned int x, unsigned int y, gui_color_t color)
{
	colpip_point(buf, x, y, color);
}

static inline void __gui_draw_semitransparent_rect(const gui_drawbuf_t * buf, unsigned int x1, unsigned int y1,
		unsigned int x2, unsigned int y2, unsigned int alpha)
{
	display_transparency(buf, x1, y1, x2, y2, alpha);
}

static inline void __gui_draw_string_prop(const gui_drawbuf_t * buf, unsigned int x, unsigned int y,
		const char * text, const gui_prop_font_t * font, gui_color_t color)
{
	gui_UB_Font_DrawPString32(buf, x, y, text, font, color);
}

static inline void __gui_draw_string_mono(const gui_drawbuf_t * buf, unsigned int x, unsigned int y,
		const char * text, const gui_mono_font_t * font, gui_color_t color)
{
	gui_UB_Font_DrawString32(buf, x, y, text, font, color);
}

static inline uint16_t __gui_get_pixw_string_mono(const char * str, const gui_mono_font_t * font)
{
	return gui_getwidth_Mstring32(str, font);
}

static inline uint16_t __gui_get_pixw_string_prop(const char * str, const gui_prop_font_t * font)
{
	return gui_getwidth_Pstring32(str, font);
}

#endif /* WITHTOUCHGUI */

#endif /* GUI_PORT_H_INCLUDED */
