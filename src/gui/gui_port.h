#ifndef GUI_PORT_H_INCLUDED
#define GUI_PORT_H_INCLUDED

#if WITHTOUCHGUI

#if LCDMODE_MAIN_L8
	// Indexed, one byte colors
	#define GUI_WINDOWTITLECOLOR		20
	#define GUI_WINDOWBGCOLOR			2
	#define GUI_SLIDERLAYOUTCOLOR		100
	#define GUI_MENUSELECTCOLOR			109
	#define COLOR_BUTTON_NON_LOCKED		COLORPIP_GREEN
	#define COLOR_BUTTON_PR_NON_LOCKED	COLORPIP_DARKGREEN	// was: COLORPIP_DARKGREEN2
	#define COLOR_BUTTON_LOCKED			COLORPIP_YELLOW
	#define COLOR_BUTTON_PR_LOCKED		DSGN_LOCKED // TFTRGB(0x3C, 0x3C, 0x00)
	#define COLOR_BUTTON_DISABLED		COLORPIP_GRAY // TFTRGB(0x50, 0x50, 0x50) FIXME: use right value

#elif 0
	// Can be used TFTRGB(r, g, b) macro
	// Experemental

	#define GUI_WINDOWTITLECOLOR		COLOR_SKYBLUE
	#define GUI_WINDOWBGCOLOR			COLOR_DARKGRAY
	#define GUI_SLIDERLAYOUTCOLOR		COLOR_GREEN
	#define GUI_MENUSELECTCOLOR			COLOR_GREEN
    #define COLOR_BUTTON_NON_LOCKED     COLOR_WHITEALL
    #define COLOR_BUTTON_PR_NON_LOCKED  COLOR_WHITEALL
	#define COLOR_BUTTON_LOCKED			COLOR_YELLOW
	#define COLOR_BUTTON_PR_LOCKED		DSGN_LOCKED
	#define COLOR_BUTTON_DISABLED		COLOR_GRAY

#else /* LCDMODE_MAIN_L8 */
	// Can be used TFTRGB(r, g, b) macro
	#define GUI_WINDOWTITLECOLOR		COLOR_SKYBLUE
	#define GUI_WINDOWBGCOLOR			COLOR_DARKGRAY
	#define GUI_SLIDERLAYOUTCOLOR		COLOR_GREEN
	#define GUI_MENUSELECTCOLOR			COLOR_GREEN
	#define COLOR_BUTTON_NON_LOCKED		COLOR_GREEN
	#define COLOR_BUTTON_PR_NON_LOCKED	COLOR_DARKGREEN
	#define COLOR_BUTTON_LOCKED			COLOR_YELLOW
	#define COLOR_BUTTON_PR_LOCKED		DSGN_LOCKED
	#define COLOR_BUTTON_DISABLED		COLOR_GRAY

#endif /* LCDMODE_MAIN_L8 */

#if WITHSDL2VIDEO

#include <SDL2/SDL.h>

typedef struct {
	SDL_Renderer * renderer;
	SDL_Texture * texture;
	uint8_t slave;
} gui_drawbuf_t;

typedef gui_drawbuf_t		gui_objbgbuf_t;
typedef COLORPIP_T 			gui_color_t;

extern gui_drawbuf_t * drawbuf;

#define GUI_DEFAULTCOLOR			0	// fully transparent color

static inline void __gui_set_drawbuf(const void * buf)
{
	if (drawbuf == NULL) drawbuf = malloc(sizeof(gui_drawbuf_t));
	drawbuf->renderer = (SDL_Renderer *) buf;
	drawbuf->texture = SDL_GetRenderTarget(drawbuf->renderer);
}

static inline const gui_drawbuf_t * __gui_get_drawbuf(void)
{
	return drawbuf;
}

// Инициализация буфера, если это необходимо в портируемом окружении
static inline void __gui_drawbuf_init(gui_drawbuf_t * buf, gui_drawbuf_t * extbuf,
		unsigned int w, unsigned int h)
{
	ASSERT(buf);
	ASSERT(extbuf);
	ASSERT(extbuf->texture);

	if (extbuf->slave)
	{
		extbuf->slave = 0;
		SDL_SetRenderTarget(extbuf->renderer, extbuf->texture);
	}

	buf->renderer = extbuf->renderer;
	buf->texture = extbuf->texture;
}

static inline void __gui_drawbuf_copy(const gui_drawbuf_t * dstbuf, gui_drawbuf_t * srcbuf,
		unsigned int dst_x, unsigned int dst_y, unsigned int src_w, unsigned int src_h)
{
	SDL_Rect destRect = { dst_x, dst_y, src_w, src_h };
	SDL_RenderCopy(dstbuf->renderer, srcbuf->texture, NULL, & destRect);
}

static inline void __gui_drawbuf_end(gui_drawbuf_t * buf)
{
	ASSERT(buf);

	const gui_drawbuf_t * db = __gui_get_drawbuf();
	SDL_SetRenderTarget(db->renderer, db->texture);
}

static inline gui_objbgbuf_t * __gui_object_bgbuf_init(unsigned int w, unsigned int h)
{
	gui_objbgbuf_t * buf = calloc(1, sizeof(gui_objbgbuf_t));
	ASSERT(buf);

	const gui_drawbuf_t * db = __gui_get_drawbuf();

	buf->slave = 1;
	buf->renderer = db->renderer;
	buf->texture = SDL_CreateTexture(db->renderer, SDL_PIXELFORMAT_ARGB8888,
		    SDL_TEXTUREACCESS_TARGET, w, h);
	ASSERT(buf->texture);
	SDL_SetTextureBlendMode(buf->texture, SDL_BLENDMODE_BLEND);

	return buf;
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

	SDL_SetRenderDrawBlendMode(buf->renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(buf->renderer, r, g, b, a);

	if (fill)
		SDL_RenderFillRect(buf->renderer, & rect);
	else
		SDL_RenderDrawRect(buf->renderer, & rect);

	SDL_SetRenderDrawBlendMode(buf->renderer, SDL_BLENDMODE_NONE);
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
typedef COLORPIP_T			gui_objbgbuf_t;

extern gui_drawbuf_t * drawbuf;

#define GUI_DEFAULTCOLOR			COLOR_BLACK

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

static inline void __gui_drawbuf_end(gui_drawbuf_t * buf)
{

}

static inline gui_objbgbuf_t * __gui_object_bgbuf_init(unsigned int w, unsigned int h)
{
	gui_objbgbuf_t * buf = (gui_objbgbuf_t *) calloc(GXSIZE(w, h), sizeof (gui_objbgbuf_t));
	ASSERT(buf);
	return buf;
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

#endif /*  */

#endif /* WITHTOUCHGUI */

#endif /* GUI_PORT_H_INCLUDED */
