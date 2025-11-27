#ifndef GUI_PORT_H_INCLUDED
#define GUI_PORT_H_INCLUDED

#if WITHTOUCHGUI

#if WITHSDL2VIDEO

#include <SDL2/SDL.h>

typedef COLORPIP_T 			gui_color_t;
typedef struct {
	SDL_Renderer * renderer;
} gui_drawbuf_t;

extern gui_drawbuf_t * drawbuf;

static inline void __gui_set_drawbuf(const void * buf)
{
	if (drawbuf == NULL) drawbuf = malloc(sizeof(gui_drawbuf_t));
	drawbuf->renderer = (SDL_Renderer *) buf;
}

static inline const gui_drawbuf_t * __gui_get_drawbuf(void)
{
	return drawbuf;
}

// Инициализация буфера, если это необходимо в портируемом окружении
static inline void __gui_drawbuf_init(gui_drawbuf_t * buf, void * extbuf,
		unsigned int w, unsigned int h)
{

}

static inline void __gui_drawbuf_copy(const gui_drawbuf_t * dstbuf, gui_drawbuf_t * srcbuf,
		unsigned int dst_x, unsigned int dst_y, unsigned int src_w, unsigned int src_h)
{

}

// Отрисовка закрашенного прямоугольника
static inline void __gui_draw_rect(const gui_drawbuf_t * buf, unsigned int x, unsigned int y,
		unsigned int w, unsigned int h, gui_color_t color, unsigned int fill)
{
	if (! buf->renderer) return;

	uint8_t r = (color >> 16) & 0xFF;
	uint8_t g = (color >> 8) & 0xFF;
	uint8_t b = (color >> 0) & 0xFF;
	uint8_t a = (color >> 24) & 0xFF;

	SDL_Rect rect = {
		.x = x,
		.y = y,
		.w = w,
		.h = h
	};

	SDL_SetRenderDrawColor(buf->renderer, r, g, b, a);

	if (fill)
		SDL_RenderFillRect(buf->renderer, & rect);
	else
		SDL_RenderDrawRect(buf->renderer, & rect);
}

// Отрисовка прямоугольника со скругленными углами
static inline void __gui_draw_rounded_rect(const gui_drawbuf_t * buf, unsigned int x, unsigned int y,
		unsigned int w, unsigned int h, unsigned int radius, gui_color_t color, unsigned int fill)
{
	if (! buf->renderer || w == 0 || h == 0) return;

	uint8_t r = radius;
	if (r > w / 2) r = w / 2;
	if (r > h / 2) r = h / 2;
	if (r == 0) {
		__gui_draw_rect(buf, x, y, w, h, color, fill);
		return;
	}

	uint8_t cr = (color >> 16) & 0xFF;
	uint8_t cg = (color >> 8) & 0xFF;
	uint8_t cb = (color >> 0) & 0xFF;
	uint8_t ca = (color >> 24) & 0xFF;
	SDL_SetRenderDrawColor(buf->renderer, cr, cg, cb, ca);

	int x0 = (int)x;
	int y0 = (int)y;
	int x1 = x0 + (int)w - 1;
	int y1 = y0 + (int)h - 1;

	if (fill) {
		// Центральные прямоугольники
		SDL_RenderFillRect(buf->renderer, &(SDL_Rect){x0 + r, y0, (int)w - 2 * r, (int)h});
		SDL_RenderFillRect(buf->renderer, &(SDL_Rect){x0, y0 + r, r, (int)h - 2 * r});
		SDL_RenderFillRect(buf->renderer, &(SDL_Rect){x1 - r + 1, y0 + r, r, (int)h - 2 * r});

		// Углы
		for (int dy = 0; dy <= r; dy ++)
		{
			int dx = (int)sqrt(r * r - dy * dy);
			// Top-left
			SDL_RenderDrawLine(buf->renderer,
					x0 + r - dx, y0 + r - dy, x0 + r, y0 + r - dy);
			// Top-right
			SDL_RenderDrawLine(buf->renderer,
					x1 - r, y0 + r - dy, x1 - r + dx, y0 + r - dy);
			// Bottom-left
			SDL_RenderDrawLine(buf->renderer,
					x0 + r - dx, y1 - r + dy, x0 + r, y1 - r + dy);
			// Bottom-right
			SDL_RenderDrawLine(buf->renderer,
					x1 - r, y1 - r + dy, x1 - r + dx, y1 - r + dy);
		}
	}
	else
	{
		SDL_RenderDrawLine(buf->renderer, x0 + r, y0, x1 - r, y0); // top
		SDL_RenderDrawLine(buf->renderer, x0 + r, y1, x1 - r, y1); // bottom
		SDL_RenderDrawLine(buf->renderer, x0, y0 + r, x0, y1 - r); // left
		SDL_RenderDrawLine(buf->renderer, x1, y0 + r, x1, y1 - r); // right

		// === ДУГИ ===
		const int segments = r;
		SDL_Point pts[4 * (segments + 1)];
		int idx = 0;

		// Top-left arc: from (x0+r, y0) to (x0, y0+r)
		for (int i = 0; i <= segments; i ++)
		{
			double t = (M_PI / 2.0) * i / segments; // t: 0 → π/2
			int px = x0 + r - (int)(r * cos(t));
			int py = y0 + r - (int)(r * sin(t));
			pts[idx++] = (SDL_Point){px, py};
		}

		// Top-right arc: from (x1-r, y0) to (x1, y0+r)
		for (int i = 0; i <= segments; i ++)
		{
			double t = (M_PI / 2.0) * i / segments;
			int px = x1 - r + (int)(r * cos(t));
			int py = y0 + r - (int)(r * sin(t));
			pts[idx++] = (SDL_Point){px, py};
		}

		// Bottom-right arc: from (x1, y1-r) to (x1-r, y1)
		for (int i = 0; i <= segments; i ++)
		{
			double t = (M_PI / 2.0) * i / segments;
			int px = x1 - r + (int)(r * cos(t));
			int py = y1 - r + (int)(r * sin(t));
			pts[idx++] = (SDL_Point){px, py};
		}

		// Bottom-left arc: from (x0, y1-r) to (x0+r, y1)
		for (int i = 0; i <= segments; i ++)
		{
			double t = (M_PI / 2.0) * i / segments;
			int px = x0 + r - (int)(r * cos(t));
			int py = y1 - r + (int)(r * sin(t));
			pts[idx++] = (SDL_Point){px, py};
		}

		SDL_RenderDrawPoints(buf->renderer, pts, idx);
	}
}

static inline void __gui_draw_line(const gui_drawbuf_t * buf, unsigned int x1, unsigned int y1,
		unsigned int x2, unsigned int y2, gui_color_t color)
{
	if (! buf->renderer) return;

	uint8_t r = (color >> 16) & 0xFF;
	uint8_t g = (color >> 8) & 0xFF;
	uint8_t b = (color >> 0) & 0xFF;
	uint8_t a = (color >> 24) & 0xFF;

	SDL_SetRenderDrawColor(buf->renderer, r, g, b, a);
	SDL_RenderDrawLine(buf->renderer, x1, y1, x2, y2);
}

static inline void __gui_draw_point(const gui_drawbuf_t * buf, unsigned int x, unsigned int y, gui_color_t color)
{
	if (! buf->renderer) return;

	uint8_t r = (color >> 16) & 0xFF;
	uint8_t g = (color >> 8) & 0xFF;
	uint8_t b = (color >> 0) & 0xFF;
	uint8_t a = (color >> 24) & 0xFF;

	SDL_SetRenderDrawColor(buf->renderer, r, g, b, a);
	SDL_RenderDrawPoint(buf->renderer, x, y);
}

static inline void __gui_draw_semitransparent_rect(const gui_drawbuf_t * buf, unsigned int x1, unsigned int y1,
		unsigned int x2, unsigned int y2, unsigned int alpha)
{
	if (! buf->renderer) return;

	uint8_t r = (COLORPIP_DARKGRAY >> 16) & 0xFF;
	uint8_t g = (COLORPIP_DARKGRAY >> 8) & 0xFF;
	uint8_t b = (COLORPIP_DARKGRAY >> 0) & 0xFF;
	uint8_t a = WITHALPHA;

	SDL_Rect rect = {
		.x = x1,
		.y = y1,
		.w = x2 - x1,
		.h = y2 - y1
	};

	SDL_SetRenderDrawBlendMode(buf->renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(buf->renderer, r, g, b, a);
	SDL_RenderFillRect(buf->renderer, & rect);
	SDL_SetRenderDrawBlendMode(buf->renderer, SDL_BLENDMODE_NONE);
}

//todo: выводить шрифты библиотекой SDL2_TTF
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

#else

typedef COLORPIP_T 			gui_color_t;
typedef gxdrawb_t 			gui_drawbuf_t;

extern gui_drawbuf_t * drawbuf;

static inline void __gui_set_drawbuf(gui_drawbuf_t * buf)
{
	drawbuf = buf;
}

static inline const gui_drawbuf_t * __gui_get_drawbuf(void)
{
	return drawbuf;
}

// Инициализация буфера, если это необходимо в портируемом окружении
static inline void __gui_drawbuf_init(gui_drawbuf_t * buf, void * extbuf,
		unsigned int w, unsigned int h)
{
	gxdrawb_initialize(buf, (PACKEDCOLORPIP_T *) extbuf, w, h);
}

static inline void __gui_drawbuf_copy(const gui_drawbuf_t * dstbuf, void * srcbuf,
		unsigned int dst_x, unsigned int dst_y, unsigned int src_w, unsigned int src_h)
{
	colpip_bitblt(
			dstbuf->cachebase, dstbuf->cachesize,	// cache parameters
			dstbuf, 	// target window
			dst_x, dst_y,	// target position
			((gui_drawbuf_t *)srcbuf)->cachebase,
			((gui_drawbuf_t *)srcbuf)->cachesize, 	// cache parameters
			srcbuf, 0, 0, src_w, src_h, BITBLT_FLAG_NONE, 0);
}

// Отрисовка закрашенного прямоугольника
static inline void __gui_draw_rect(const gui_drawbuf_t * buf, unsigned int x, unsigned int y,
		unsigned int w, unsigned int h, gui_color_t color, unsigned int fill)
{
	colpip_rect(buf, x, y, x + w - 1, y + h - 1, color, fill);
}

// Отрисовка прямоугольника со скругленными углами
static inline void __gui_draw_rounded_rect(const gui_drawbuf_t * buf, unsigned int x, unsigned int y,
		unsigned int w, unsigned int h, unsigned int radius, gui_color_t color, unsigned int fill)
{
	colmain_rounded_rect(buf, x, y, x + w - 1, y + h - 1, radius, color, fill);
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

#endif /*  */

#endif /* WITHTOUCHGUI */

#endif /* GUI_PORT_H_INCLUDED */
