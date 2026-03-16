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

#if LINUX_SUBSYSTEM
	#define GUI_DEBUG_PRINT	printf
#else
	#define GUI_DEBUG_PRINT	PRINTF
#endif

#define GUI_ASSERT(v) do { if ((v) == 0) { \
		GUI_DEBUG_PRINT("%s(%d): Assert '%s'\n", __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

#define GUI_MEM_ASSERT(v) 	do { if (((v) == NULL)) { \
		GUI_DEBUG_PRINT("%s: %d ('%s') - memory allocate failed!\n", __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

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

// 10 ms non-blocking delay
static inline uint8_t lp_delay_10ms(uint8_t init)
{
    static uint64_t oldt = 0;
    struct timespec ts;
    uint64_t t;

    if (clock_gettime(CLOCK_MONOTONIC, & ts) == -1) {
        return 0;
    }

    t = (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;

    if (init)
    {
        oldt = t;
        return 0;
    }
    else
    {
        if (t - oldt > 10)
        {
            oldt = t;
            return 1;
        }
        return 0;
    }
}

static inline void __gui_set_drawbuf(const void * buf)
{
	if (drawbuf == NULL) drawbuf = malloc(sizeof(gui_drawbuf_t));
	drawbuf->renderer = (SDL_Renderer *) buf;
	drawbuf->texture = NULL;
}

static inline const gui_drawbuf_t * __gui_get_drawbuf(void)
{
	return drawbuf;
}

// Инициализация буфера, если это необходимо в портируемом окружении
static inline void __gui_drawbuf_init(gui_drawbuf_t * buf, gui_drawbuf_t * extbuf,
		unsigned int w, unsigned int h)
{
	(void)w; (void)h;
	ASSERT(buf);
	ASSERT(extbuf);

	buf->renderer = extbuf->renderer;
	buf->texture = extbuf->texture;

	if (extbuf->slave)
	{
		SDL_SetRenderTarget(extbuf->renderer, extbuf->texture);
		buf->slave = 1;
		extbuf->slave = 0;
	} else {
		buf->slave = 0;
	}
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
	if (buf->slave) SDL_SetRenderTarget(buf->renderer, NULL);
}

static inline gui_objbgbuf_t * __gui_object_bgbuf_init(unsigned int w, unsigned int h)
{
	gui_objbgbuf_t * buf = calloc(1, sizeof(gui_objbgbuf_t));
	ASSERT(buf);

	const gui_drawbuf_t * db = __gui_get_drawbuf();
	ASSERT(db && db->renderer);

	buf->slave = 1;
	buf->renderer = db->renderer;

	buf->texture = SDL_CreateTexture(db->renderer, SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET, w, h);
	ASSERT(buf->texture);

	SDL_SetTextureBlendMode(buf->texture, SDL_BLENDMODE_BLEND);

	SDL_SetRenderTarget(db->renderer, buf->texture);
	SDL_SetRenderDrawBlendMode(db->renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(db->renderer, 0, 0, 0, 0);
	SDL_RenderClear(db->renderer);

	SDL_SetRenderTarget(db->renderer, db->texture);

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

	SDL_Rect rect = { .x = x, .y = y, .w = w, .h = h };

	if (a < 255)
		SDL_SetRenderDrawBlendMode(buf->renderer, SDL_BLENDMODE_BLEND);
	else
		SDL_SetRenderDrawBlendMode(buf->renderer, SDL_BLENDMODE_NONE);

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

	int x0 = x;
	int y0 = y;
	int x1 = x0 + w - 1;
	int y1 = y0 + h - 1;

	if (fill) {
		/* 1. Центральные прямоугольники (без углов) */
		if (w > 2 * r) {
			SDL_RenderFillRect(buf->renderer, &(SDL_Rect) {x0 + r, y0, w - 2 * r, h});
		}
		if (h > 2 * r) {
			SDL_RenderFillRect(buf->renderer, &(SDL_Rect) {x0, y0 + r, r, h - 2 * r});
			SDL_RenderFillRect(buf->renderer, &(SDL_Rect) {x1 - r + 1, y0 + r, r, h - 2 * r});
		}

		/* Вспомогательная функция для заливки четверти круга */
#define FILL_QUARTER_CIRCLE(cx, cy, sign_x, sign_y) 				\
            do { 													\
                for (int dy = 0; dy <= r; dy++) { 					\
                    int dx = (int)sqrt((double)(r * r - dy * dy)); 	\
                    for (int dx2 = 0; dx2 <= dx; dx2++) { 			\
                        SDL_RenderDrawPoint(buf->renderer, 			\
                            (cx) + (sign_x) * dx2, 					\
                            (cy) + (sign_y) * dy); 					\
                    }												\
                } 													\
            } while(0)

		/* Top-left corner */
		FILL_QUARTER_CIRCLE(x0 + r, y0 + r, -1, -1);
		/* Top-right corner */
		FILL_QUARTER_CIRCLE(x1 - r, y0 + r, 1, -1);
		/* Bottom-left corner */
		FILL_QUARTER_CIRCLE(x0 + r, y1 - r, -1, 1);
		/* Bottom-right corner */
		FILL_QUARTER_CIRCLE(x1 - r, y1 - r, 1, 1);

#undef FILL_QUARTER_CIRCLE
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
	uint8_t a = (uint8_t)alpha;

	SDL_Rect rect = { .x = x1, .y = y1, .w = (x2 - x1), .h = (y2 - y1)};

	SDL_SetRenderDrawBlendMode(buf->renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(buf->renderer, r, g, b, a);
	SDL_RenderFillRect(buf->renderer, & rect);
}

static inline uint8_t __gui_get_touch_event(uint16_t * x, uint16_t * y)
{
	uint_fast16_t xx, yy, p;
	p = board_tsc_getxy(& xx, & yy);
	* x = xx;
	* y = yy;
	return p;
}

#if GUI_USE_CACHE

/* Создание кэш-текстуры для отрисовки */
static inline gui_objbgbuf_t * __gui_cache_texture_create(unsigned int w, unsigned int h)
{
    gui_objbgbuf_t * buf = calloc(1, sizeof(gui_objbgbuf_t));
    ASSERT(buf);

    const gui_drawbuf_t * db = __gui_get_drawbuf();
    ASSERT(db && db->renderer);

    buf->renderer = db->renderer;
    buf->texture = SDL_CreateTexture(db->renderer,
                                      SDL_PIXELFORMAT_ARGB8888,
                                      SDL_TEXTUREACCESS_TARGET,
                                      w, h);
    if (buf->texture == NULL) printf("%s: %s\n", __func__, SDL_GetError());

    /* Устанавливаем режим смешивания для прозрачности */
    SDL_SetTextureBlendMode(buf->texture, SDL_BLENDMODE_BLEND);

    return buf;
}

/* Начало отрисовки в кэш-текстуру */
static inline void __gui_cache_texture_begin(gui_objbgbuf_t * cache)
{
    ASSERT(cache && cache->texture);

    const gui_drawbuf_t * db = __gui_get_drawbuf();
    ASSERT(db && db->renderer);

    /* Сохраняем текущий render target */
    cache->slave = (uint8_t)(uintptr_t)SDL_GetRenderTarget(db->renderer);

    /* Переключаемся на кэш-текстуру */
    SDL_SetRenderTarget(db->renderer, cache->texture);

    /* Очищаем прозрачным цветом */
    SDL_SetRenderDrawBlendMode(db->renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(db->renderer, 0, 0, 0, 0);
    SDL_RenderClear(db->renderer);
}

/* Завершение отрисовки в кэш-текстуру */
static inline void __gui_cache_texture_end(gui_objbgbuf_t * cache)
{
    ASSERT(cache);

    const gui_drawbuf_t * db = __gui_get_drawbuf();
    ASSERT(db && db->renderer);

    /* Восстанавливаем предыдущий render target */
    SDL_SetRenderTarget(db->renderer, (SDL_Texture *)(uintptr_t)cache->slave);
}

/* Отрисовка кэш-текстуры на экран в указанных координатах */
static inline void __gui_cache_texture_draw(gui_objbgbuf_t * cache,
                                             unsigned int dst_x,
                                             unsigned int dst_y)
{
    ASSERT(cache && cache->texture);

    const gui_drawbuf_t * db = __gui_get_drawbuf();
    ASSERT(db && db->renderer);

    int w, h;
    SDL_QueryTexture(cache->texture, NULL, NULL, &w, &h);

    SDL_Rect dstRect = { dst_x, dst_y, w, h };

    /* Копируем текстуру на экран */
    SDL_RenderCopy(db->renderer, cache->texture, NULL, &dstRect);
}

/* Уничтожение кэш-текстуры */
static inline void __gui_cache_texture_destroy(gui_objbgbuf_t * cache)
{
    if (cache) {
        if (cache->texture) {
            SDL_DestroyTexture(cache->texture);
            cache->texture = NULL;
        }
        free(cache);
    }
}

#endif /* GUI_USE_CACHE */

#ifdef GUI_TIME_PROFILER

    #define TIME_PROFILE_START(label) \
        uint32_t _tp_start_##label = SDL_GetTicks(); \
        const char *_tp_name_##label = #label

    #define TIME_PROFILE_STOP(label, description) \
        do { \
            uint32_t _tp_end_##label = SDL_GetTicks(); \
            uint32_t _tp_elapsed_##label = _tp_end_##label - _tp_start_##label; \
            printf("[PROFILE] %-24s | %s:%d | %s | elapsed: %u ms\n", \
                   _tp_name_##label, __FILE__, __LINE__, \
                   (description), _tp_elapsed_##label); \
        } while(0)

#else
    /* Заглушки при отключённом профилировании */
    #define TIME_PROFILE_START(label)      ((void)0)
    #define TIME_PROFILE_STOP(label, desc) ((void)0)
#endif /* GUI_TIME_PROFILER */

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

static inline uint8_t __gui_get_touch_event(uint16_t * x, uint16_t * y)
{
	uint_fast16_t xx, yy, p;
	p = board_tsc_getxy(& xx, & yy);
	* x = xx;
	* y = yy;
	return p;
}

static inline uint8_t lp_delay_10ms(uint8_t init)
{
	return 0;
}

#define TIME_PROFILE_START(label)      ((void)0)
#define TIME_PROFILE_STOP(label, desc) ((void)0)

#endif /*  */

#endif /* WITHTOUCHGUI */

#endif /* GUI_PORT_H_INCLUDED */
