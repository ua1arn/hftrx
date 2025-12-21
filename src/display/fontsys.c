/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"

#if LCDMODE_LTDC

#include "formats.h"
#include "display.h"

#include <string.h>



static uint_fast16_t colorpip_draw_char(
	const gxdrawb_t * db,
	uint_fast16_t xpix,
	uint_fast16_t ypix,
	const unifont_t * font,
	char cc,
	COLORPIP_T fg
	)
{
	savewhere = __func__;
	return font->font_drawci(db, xpix, ypix, font, font->decode(font, cc), fg);
}


// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
uint_fast16_t
colpip_string(
	const gxdrawb_t * db,
	uint_fast16_t xpix,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t ypix,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const unifont_t * font,
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	)
{
	char c;

	ASSERT(s != NULL);
	while ((c = * s ++) != '\0')
	{
		xpix = colorpip_draw_char(db, xpix, ypix, font, c, fg);
	}
	return xpix;
}

// получить оба размера текстовой строки
uint_fast16_t
colpip_string_widthheight(
	const unifont_t * font,
	const char * s,
	uint_fast16_t * height
	)
{
	uint_fast16_t w = 0;
	char c;

	ASSERT(font);
	ASSERT(s);
	ASSERT(height);

	* height = font->font_drawheight(font);

	while ((c = * s ++) != '\0')
		w += font->font_drawwidthci(font, font->decode(font, c));
	return w;
}

// обычный шрифт
uint_fast16_t display_wrdata_begin(uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp)
{
	* yp = GRID2Y(ycell);
	return GRID2X(xcell);
}

typedef struct ufcache_tag
{
	unsigned picx;// = BIGCHARW * RENDERCHARS;
	unsigned picy;// = BIGCHARH;
	PACKEDCOLORPIP_T * rendered;// [GXSIZE(BIGCHARW * RENDERCHARS, BIGCHARH)];
	gxdrawb_t dbv;
} ufcache_t;

static uint_fast16_t ufcached_decode(const struct unifont_tag * font, char cc)
{
	const unifont_t * const pf = (const unifont_t *) font->fontraster;	// parent unifont_t object
	return pf->decode(pf, cc);
}

static uint_fast8_t ufcached_drawwidth(const struct unifont_tag * font, uint_fast16_t ci)	// ширина в пиксеях данного символа (может быть меньше чем поле width)
{
	const unifont_t * const pf = (const unifont_t *) font->fontraster;	// parent unifont_t object
	return pf->font_drawwidthci(pf, ci);
}
static uint_fast8_t ufcached_drawheight(const struct unifont_tag * font)	// высота в пикселях (се символы шрифта одной высоты)
{
	const unifont_t * const pf = (const unifont_t *) font->fontraster;	// parent unifont_t object
	return pf->font_drawheight(pf);
}

static const void * ufcached_getcharraster(const struct unifont_tag * font, uint_fast16_t ci)	// получение начального адреса растра для символа
{
	const unifont_t * const pf = (const unifont_t *) font->fontraster;	// parent unifont_t object
	return pf->getcharrasterci(pf, ci);
}
static uint_fast16_t ufcached_drawci(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, const struct unifont_tag * font, uint_fast16_t ci, COLORPIP_T fg)
{
	const unifont_t * const pf = (const unifont_t *) font->fontraster;	// parent unifont_t object
	return pf->font_drawci(db, xpix, ypix, pf, ci, fg);
}

void ufcached_prerender(const unifont_t * font, COLORPIP_T fg, COLORPIP_T bg)
{
	const unifont_t * const pf = (const unifont_t *) font->fontraster;	// parent unifont_t object
	ufcache_t * const cache = (ufcache_t *) font->fontdata;
	cache->picx = 16;//pf->font_drawwidthci(pf, 0);	// самый большой размер
	cache->picy = pf->font_drawheight(pf);
	const uint_fast16_t cicount = 32;
	PACKEDCOLORPIP_T * b = (PACKEDCOLORPIP_T *) calloc(sizeof (PACKEDCOLORPIP_T), GXSIZE(cache->picx * cicount, cache->picy));
	ASSERT(b);
	gxdrawb_initialize(& cache->dbv, b, cache->picx, cache->picy);
	colpip_fillrect(& cache->dbv, 0, 0, cache->picx, cache->picy, bg);	/* при alpha==0 все биты цвета становятся 0 */
}

static ufcache_t unifont_big0;
const unifont_t unifont_big =
{
	.decode = ufcached_decode,
	.getcharrasterci = ufcached_getcharraster,
	.font_drawwidthci = ufcached_drawwidth,
	.font_drawheight = ufcached_drawheight,
	.font_drawci = ufcached_drawci,
	//
	.fontraster = & unifont_big_raw,
	.fontdata = & unifont_big0,
	.label = "unifont_bigcached"
};

static ufcache_t unifont_half0;
const unifont_t unifont_half =
{
	.decode = ufcached_decode,
	.getcharrasterci = ufcached_getcharraster,
	.font_drawwidthci = ufcached_drawwidth,
	.font_drawheight = ufcached_drawheight,
	.font_drawci = ufcached_drawci,
	//
	.fontraster = & unifont_half_raw,
	.fontdata = & unifont_half0,
	.label = "unifont_halfcached"
};


void rendered_value_big_initialize(const gxstyle_t * gxstylep)
{
	const COLORPIP_T fg = gxstylep->textcolor;
	const COLORPIP_T bg = gxstylep->bgcolor;

	ufcached_prerender(& unifont_big, fg, bg);
	ufcached_prerender(& unifont_half, fg, bg);
}

#if WITHPRERENDER && 0
/* использование предварительно построенных изображений при отображении частоты */

enum { RENDERCHARS = 14 }; /* valid chars: "0123456789 #._" */
//static const char renderchars [] = "0123456789 #._";
/* размеры в пикселях альманаха больших символов отображения частоты */
static const unsigned picx_big = BIGCHARW * RENDERCHARS;
static const unsigned picy_big = BIGCHARH;

/* размеры в пикселях альманаха средних символов отображения частоты */
static const unsigned picx_half = HALFCHARW * RENDERCHARS;
static const unsigned picy_half = HALFCHARH;

/* Изображения символов располагаются в буфере горизонтально, слева направо */
static PACKEDCOLORPIP_T rendered_big [GXSIZE(BIGCHARW * RENDERCHARS, BIGCHARH)];
static PACKEDCOLORPIP_T rendered_half [GXSIZE(HALFCHARW * RENDERCHARS, HALFCHARH)];
static gxdrawb_t dbvbig;
static gxdrawb_t dbvhalf;

// Подготовка отображения больщих символов
/* valid chars: "0123456789 #._" */
void rendered_value_big_initialize(const gxstyle_t * gxstylep)
{
	const COLORPIP_T fg = gxstylep->textcolor;
	const COLORPIP_T bg = gxstylep->bgcolor;

	ufcached_prerender(& unifont_bigcached, fg, bg);
	ufcached_prerender(& unifont_halfcached, fg, bg);
	return;

	const COLORPIP_T keycolor = COLORPIP_KEY;
	const unsigned picalpha = 255;
	gxdrawb_initialize(& dbvbig, rendered_big, picx_big, picy_big);
	gxdrawb_initialize(& dbvhalf, rendered_half, picx_half, picy_half);

	colpip_fillrect(& dbvbig, 0, 0, picx_big, picy_big, bg);	/* при alpha==0 все биты цвета становятся 0 */
	colpip_fillrect(& dbvhalf, 0, 0, picx_half, picy_half, bg);	/* при alpha==0 все биты цвета становятся 0 */

	uint_fast8_t ci;

	/* Возможно использование подготовленных изображений */
//	#include "fonts/BigDigits.png.h"
//	#include "fonts/HalfDigits.png.h"
//
//	PACKEDCOLORPIP_T * const fb = colmain_fb_draw();
//	LuImage * BigDigits_png = luPngReadMemory((char *) BigDigits_png);
//	LuImage * HalfDigits_png = luPngReadMemory((char *) HalfDigits_png);
//
//	PACKEDCOLORPIP_T * const fbpic = (PACKEDCOLORPIP_T *) BigDigits_png->data;
//	const COLORPIP_T keycolor = TFTRGB(BigDigits_png->data [0], BigDigits_png->data [1], BigDigits_png->data [2]);	/* угловой пиксель - надо правильно преобразовать из ABGR*/
//	const unsigned picdx = BigDigits_png->width;//GXADJ(png->width);
//	const unsigned picw = BigDigits_png->width;
//	const unsigned pich = BigDigits_png->height;
//	PRINTF("testpng: sz=%u data=%p, dataSize=%u, depth=%u, w=%u, h=%u\n", (unsigned) sizeof fbpic [0], png, (unsigned) png->dataSize,  (unsigned) png->depth, (unsigned) png->width, (unsigned) png->height);

	/* big-size characters */
	{
		const unifont_t * const font = & unifont_big;
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdata_begin(0, 0, & ypix);
		for (ci = 0; ci < RENDERCHARS; ++ ci)
		{
			/* формирование изображений символов, возможно с эффектами антиалиасинга */
			/* Изображения символов располагаются в буфере горизонтально, слева направо */
			ASSERT(xpix == ci * BIGCHARW);
			ASSERT(font->font_prerenderci);
			font->font_prerenderci(& dbvbig, xpix, ypix, font, ci, fg);	// BIGCHARW
			display_do_AA(& dbvbig, xpix, ypix, BIGCHARW, BIGCHARH);
			xpix += BIGCHARW;
		}
		dcache_clean((uintptr_t) rendered_big, sizeof rendered_big [0] * GXSIZE(BIGCHARW * RENDERCHARS, BIGCHARH));
	}
	/* half-size characters */
	{
		const unifont_t * const font = & unifont_half;
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdata_begin(0, 0, & ypix);
		for (ci = 0; ci < RENDERCHARS; ++ ci)
		{
			/* формирование изображений символов, возможно с эффектами антиалиасинга */
			/* Изображения символов располагаются в буфере горизонтально, слева направо */
			ASSERT(xpix == ci * HALFCHARW);
			ASSERT(font->font_prerenderci);
			font->font_prerenderci(& dbvhalf, xpix, ypix, font, ci, fg);	// HALFCHARW
			display_do_AA(& dbvhalf, xpix, ypix, HALFCHARW, HALFCHARH);
			xpix += HALFCHARW;
		}
		dcache_clean((uintptr_t) rendered_half, sizeof rendered_half [0] * GXSIZE(HALFCHARW * RENDERCHARS, HALFCHARH));
	}
}

static uint_fast16_t
unifont_put_char_half_rendered(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	const uint_fast16_t ci = font->decode(font, cc);
	const uint_fast16_t width2 = font->font_drawwidthci(font, ci);	// number of bits (start from LSB first byte in raster)
//	const uint_fast16_t height2 = font->font_drawheight(font, cc);	// number of rows
//	const uint_fast16_t bytesw = font->bytesw;	// bytes in each chargen row
//	return unifont_put_char(db, xpix, ypix, font, charraster, width2, height2, bytesw, fg);
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	/* копируем изображение БЕЗ цветового ключа */
	/* dcache_clean исходного изображения уже выполнено при построении изображения. */
	colpip_bitblt(
			db->cachebase, db->cachesize,
			db,
			xpix, ypix,	// координаты в окне получатля
			dbvhalf.cachebase, 0 * dbvhalf.cachesize,
			& dbvhalf,
			ci * HALFCHARW, 0,	// координаты окна источника
			width2, HALFCHARH, // размер окна источника
			BITBLT_FLAG_NONE, COLORPIP_KEY
			);

	return xpix + width2;
}

static uint_fast16_t
unifont_put_char_big_rendered(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	const uint_fast16_t ci = font->decode(font, cc);
	const uint_fast16_t width2 = font->font_drawwidthci(font, ci);	// number of bits (start from LSB first byte in raster)
//	const uint_fast16_t height2 = font->font_drawheight(font, cc);	// number of rows
//	const uint_fast16_t bytesw = font->bytesw;	// bytes in each chargen row
//	return unifont_put_char(db, xpix, ypix, font, charraster, width2, height2, bytesw, fg);
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	/* копируем изображение БЕЗ цветового ключа */
	/* dcache_clean исходного изображения уже выполнено при построении изображения. */
	colpip_bitblt(
			db->cachebase, db->cachesize,
			db,
			xpix, ypix,	// координаты в окне получатля
			dbvbig.cachebase, 0 * dbvbig.cachesize,
			& dbvbig,
			ci * BIGCHARW, 0,	// координаты окна источника
			width2, BIGCHARH, // размер окна источника
			BITBLT_FLAG_NONE, COLORPIP_KEY
			);

	return xpix + width2;
}
#else

#endif /* WITHPRERENDER */

#if 0

const unifont_t unifont_big =
{
	.decode = bighalffont_decode,
	.getcharraster = unifont_getcharraster,
	.font_drawwidth = bigfont_width,
	.font_drawheight = bigfont_height,
#if WITHALTERNATIVEFONTS
	.bytesw = (BIGCHARW + 7) / 8,//sizeof ltdc_CenturyGothic_big [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = ltdc_CenturyGothic_big,		// начало знакогенератора в памяти
#else /* WITHALTERNATIVEFONTS */
	.bytesw = (BIGCHARW + 7) / 8,//sizeof S1D13781_bigfont_LTDC [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = S1D13781_bigfont_LTDC,		// начало знакогенератора в памяти
#endif /* WITHALTERNATIVEFONTS */
	.font_draw = unifont_put_char_big_rendered,
	.font_prerender = unifont_put_char_bighalf_prerender,
	.label = "unifont_big"
};

const unifont_t unifont_half =
{
	.decode = bighalffont_decode,
	.getcharraster = unifont_getcharraster,
	.font_drawwidth = halffont_width,
	.font_drawheight = halffont_height,
#if WITHALTERNATIVEFONTS
	.bytesw = (HALFCHARW + 7) / 8,//sizeof ltdc_CenturyGothic_half [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = ltdc_CenturyGothic_half,		// начало знакогенератора в памяти
#else /* WITHALTERNATIVEFONTS */
	.bytesw = (HALFCHARW + 7) / 8,//sizeof S1D13781_halffont_LTDC [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = S1D13781_halffont_LTDC,		// начало знакогенератора в памяти
#endif /* WITHALTERNATIVEFONTS */
	.font_draw = unifont_put_char_half_rendered,
	.font_prerender = unifont_put_char_bighalf_prerender,
	.label = "unifont_half"
};
#endif

// *********************************************************************************************************************

#if 0
// CP Font Generator support

#include "fonts/Tahoma_Regular_88x77.h"
// CP Font Generator support

static uint_fast16_t
cpfont_decode(const unifont_t * font, char cc)
{
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	const unsigned first = USBD_peek_u16(blob + 2);
	const unsigned lasr = USBD_peek_u16(blob + 4);
	const uint_fast8_t c = (unsigned char) cc;
	if (c < ubp->first_char)
		return 0;
	if (c > ubp->last_char)
		return 0;
	return c - ubp->first_char;
}

const unifont_t unifont_Tahoma_Regular_88x77 =
{
	.decode = cpfont_decode,
	.getcharraster = cpfont_getcharraster,
	.font_drawwidth = cpfont_width,
	.font_drawheight = cpfont_height,
	.font_draw = cpfont_render_char16,
	//
	.fontraster = Tahoma_Regular_88x77,
	.label = "Tahoma_Regular_88x77"
};

#endif

#endif /* LCDMODE_LTDC */

// **********************************

