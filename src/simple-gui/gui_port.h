#ifndef GUI_PORT_H_INCLUDED
#define GUI_PORT_H_INCLUDED

#if WITHTOUCHGUI

#define GUI_COLOR_DARKGRAY				TFTRGB(0x80, 0x80, 0x80)
#define GUI_COLOR_YELLOW         		TFTRGB(0xFF, 0xFF, 0x00)
#define GUI_COLOR_WHITE          		TFTRGB(0xFF, 0xFF, 0xFF)
#define GUI_COLOR_GRAY       	 		TFTRGB(0xA9, 0xA9, 0xA9)
#define GUI_COLOR_BLACK          		TFTRGB(0x00, 0x00, 0x00)

#define GUI_WINDOWTITLECOLOR			TFTRGB(0x87, 0xCE, 0xEB)
#define GUI_WINDOWBGCOLOR				GUI_COLOR_DARKGRAY
#define GUI_SLIDERLAYOUTCOLOR			TFTRGB(0x00, 0xFF, 0x00)
#define GUI_MENUSELECTCOLOR				TFTRGB(0x00, 0xFF, 0x00)
#define GUI_COLOR_BUTTON_NON_LOCKED		TFTRGB(0x00, 0xFF, 0x00)
#define GUI_COLOR_BUTTON_PR_NON_LOCKED	TFTRGB(0x00, 0x64, 0x00)
#define GUI_COLOR_BUTTON_LOCKED			GUI_COLOR_YELLOW
#define GUI_COLOR_BUTTON_PR_LOCKED		TFTRGB(0x3C, 0x3C, 0x00)
#define GUI_COLOR_BUTTON_DISABLED		GUI_COLOR_GRAY

#define GUI_DEBUG_PRINT	PRINTF

#define GUI_ASSERT(v) do { if ((v) == 0) { \
		GUI_DEBUG_PRINT("%s(%d): Assert '%s'\n", __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

#define GUI_VERIFY(v) do { if ((v) == 0) { \
		GUI_DEBUG_PRINT("%s(%d): Verify '%s'\n", __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

#define GUI_MEM_ASSERT(v) 	do { if (((v) == NULL)) { \
		GUI_DEBUG_PRINT("%s: %d ('%s') - memory allocate failed!\n", __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

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
	return 1;
}

#define TIME_PROFILE_START(label)      ((void)0)
#define TIME_PROFILE_STOP(label, desc) ((void)0)

#endif /* WITHTOUCHGUI */
#endif /* GUI_PORT_H_INCLUDED */
