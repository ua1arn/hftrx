/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"

#if LCDMODE_LTDC

#include "formats.h"
#include "fontsys.h"

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
unifont_text(
	const gxdrawb_t * db,
	uint_fast16_t xpix,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t ypix,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const unifont_t * font,
	const char * s,
	size_t slength,		// количество символов для печати или TEZXTSZIE_AUTO - если до '\0'
	COLORPIP_T fg		// цвет вывода текста
	)
{
	ASSERT(s != NULL);
	slength = (slength == TEZXTSZIE_AUTO) ? strlen(s) : slength;
	while (slength --)
	{
		xpix = colorpip_draw_char(db, xpix, ypix, font, * s ++, fg);
	}
	return xpix;
}

// получить оба размера текстовой строки
uint_fast16_t
unifont_textsize(
	const unifont_t * font,
	const char * s,
	size_t slength,		// количество символов для печати или TEZXTSZIE_AUTO - если до '\0'
	uint_fast16_t * height
	)
{
	uint_fast16_t w = 0;
	ASSERT(font);
	ASSERT(s);
	ASSERT(height);

	slength = (slength == TEZXTSZIE_AUTO) ? strlen(s) : slength;
	* height = font->font_drawheight(font);
	while (slength --)
		w += font->font_drawwidthci(font, font->decode(font, * s ++));
	return w;
}

// обычный шрифт
uint_fast16_t display_wrdata_begin(uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp)
{
	* yp = GRID2Y(ycell);
	return GRID2X(xcell);
}

// *********************************************************************************************************************


typedef struct ufcache_tag
{
	unsigned cellw;	// размер поля символа по горизонтали
	unsigned cellh;	// размер поля символа по вертикали
	PACKEDCOLORPIP_T * rendered;	// буфер
	gxdrawb_t dbv;
} ufcache_t;

static uint_fast16_t ufcached_decode(const struct unifont_tag * font, char cc)
{
	const unifont_t * const pf = (const unifont_t *) font->fontraster;	// parent unifont_t object
	return pf->decode(pf, cc);
}

static uint_fast16_t ufcached_totalci(const struct unifont_tag * font)
{
	const unifont_t * const pf = (const unifont_t *) font->fontraster;	// parent unifont_t object
	ASSERT(pf->totalci);
	return pf->totalci(pf);
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
#if WITHPRERENDER
	ufcache_t * const cache = (ufcache_t *) font->fontdata;
	if (cache->rendered)
	{
		const uint_fast16_t width2 = pf->font_drawwidthci(pf, ci);	// number of bits (start from LSB first byte in raster)
		// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
		/* копируем изображение БЕЗ цветового ключа */
		/* dcache_clean исходного изображения уже выполнено при построении изображения. */
		colpip_bitblt(
				db->cachebase, db->cachesize,
				db,
				xpix, ypix,	// координаты в окне получатля
				cache->dbv.cachebase, 0 * cache->dbv.cachesize,
				& cache->dbv,
				ci * cache->cellw, 0,	// координаты окна источника
				width2, cache->cellh, // размер окна источника
				BITBLT_FLAG_NONE, COLORPIP_KEY
				);

		return xpix + width2;
	}

#endif /* WITHPRERENDER */

	return pf->font_drawci(db, xpix, ypix, pf, ci, fg);

}

static void ufcached_prerender(const unifont_t * font, COLORPIP_T fg, COLORPIP_T bg)
{
	const unifont_t * const pf = (const unifont_t *) font->fontraster;	// parent unifont_t object
	ufcache_t * const cache = (ufcache_t *) font->fontdata;
	const uint_fast16_t cicount = pf->totalci(pf);
	uint_fast16_t w;
	uint_fast16_t ci;
	for (ci = 0, w = 0; ci < cicount; ++ ci)
	{
		w = ulmax16(w, pf->font_drawwidthci(pf, ci));
	}
	cache->cellw = w;	// самый большой размер
	cache->cellh = pf->font_drawheight(pf);

	PACKEDCOLORPIP_T * const b = (PACKEDCOLORPIP_T *) aligned_alloc(DCACHEROWSIZE, GXSIZE(cache->cellw * cicount, cache->cellh) * sizeof (PACKEDCOLORPIP_T));
	ASSERT(b);
	if (b == NULL)
		return;
	cache->rendered = b;
	gxdrawb_initialize(& cache->dbv, b, cache->cellw * cicount, cache->cellh);
	// заполнение фона
	colpip_fillrect(& cache->dbv, 0, 0, cache->cellw * cicount, cache->cellh, bg);	/* при alpha==0 все биты цвета становятся 0 */

	const uint_fast16_t ypix = 0;
	uint_fast16_t xpix = 0;
	for (ci = 0; ci < cicount; ++ ci, xpix += cache->cellw)
	{
		pf->font_drawci(& cache->dbv, xpix, ypix, pf, ci, fg);	// BIGCHARW
		display_do_AA(& cache->dbv, xpix, ypix, pf->font_drawwidthci(pf, ci), cache->cellh);
	}
	dcache_clean(cache->dbv.cachebase, cache->dbv.cachesize);
}

static ufcache_t unifont_big0;
const unifont_t unifont_big =
{
	.decode = ufcached_decode,
	.totalci = ufcached_totalci,
	.getcharrasterci = ufcached_getcharraster,
	.font_drawwidthci = ufcached_drawwidth,
	.font_drawheight = ufcached_drawheight,
	.font_drawci = ufcached_drawci,
	//
	.fontraster = & unifont_big_raw,
	.fontdata = & unifont_big0,
	.label = "unifont_big"
};

static ufcache_t unifont_half0;
const unifont_t unifont_half =
{
	.decode = ufcached_decode,
	.totalci = ufcached_totalci,
	.getcharrasterci = ufcached_getcharraster,
	.font_drawwidthci = ufcached_drawwidth,
	.font_drawheight = ufcached_drawheight,
	.font_drawci = ufcached_drawci,
	//
	.fontraster = & unifont_half_raw,
	.fontdata = & unifont_half0,
	.label = "unifont_half"
};


void rendered_value_big_initialize(const gxstyle_t * gxstylep)
{
#if WITHPRERENDER
	const COLORPIP_T fg = gxstylep->textcolor;
	const COLORPIP_T bg = gxstylep->bgcolor;

	ufcached_prerender(& unifont_big, fg, bg);
	ufcached_prerender(& unifont_half, fg, bg);
#endif /* WITHPRERENDER */
}

// *********************************************************************************************************************

#if 1
// CP Font Generator support

#include "fonts/Tahoma_Regular_88x77.h"
// CP Font Generator support
// PitPack (Normal) format

uint_fast16_t
cpfont_bitpacknormal_decode(const unifont_t * font, char cc)
{
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	const unsigned first = USBD_peek_u16(blob + 2);
	const unsigned last = USBD_peek_u16(blob + 4);
	const uint_fast8_t c = (unsigned char) cc;
	if (c < first)
		return 0;
	if (c > last)
		return 0;
	return c - first;
}

uint_fast16_t
cpfont_bitpacknormal_totalci(const unifont_t * font)
{
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	const unsigned first = USBD_peek_u16(blob + 2);
	const unsigned last = USBD_peek_u16(blob + 4);
	return last - first + 1;
}

uint_fast8_t cpfont_bitpacknormal_height(const struct unifont_tag * font)	// высота в пикселях (все символы шрифта одной высоты)
{
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	return USBD_peek_u16(blob + 8);
}

// параметры для данного символа
static uint_fast16_t cpfont_bitpacknormal_paramstart(const struct unifont_tag * font, uint_fast16_t ci)
{
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	enum { PARAMSIZE = 6 };
	const uint_fast16_t totalci = font->totalci(font);
	const uint_fast16_t indexstart = 10;
	const uint_fast16_t paramstart = indexstart + totalci + blob [indexstart + ci] * PARAMSIZE;	// параметры для данного символа
	return paramstart;
}

uint_fast8_t cpfont_bitpacknormal_width(const struct unifont_tag * font, uint_fast16_t ci)	// ширина в пиксеях данного символа на экране
{
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	const uint_fast16_t paramstart = cpfont_bitpacknormal_paramstart(font, ci);	// параметры для данного символа
	return blob [paramstart + 3]; // 2 or 3 - candidates ???????????? 2 - raster width, 3 - width ?
}

// получение начального адреса растра для символа
const void * cpfont_bitpacknormal_getcharraster(const struct unifont_tag * font, uint_fast16_t ci)
{
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	const uint_fast8_t bytespervertical = (cpfont_bitpacknormal_height(font) + 7) / 8;
	enum { PARAMSIZE = 6 };
	const uint_fast16_t paramstart = cpfont_bitpacknormal_paramstart(font, ci);	// параметры для данного символа
	const uint_fast16_t rastersstart = USBD_peek_u16(blob + paramstart + 4); //indexstart + totalci + totalci * PARAMSIZE;
	const uint_fast16_t width2 = blob [paramstart + 2];
	const uint_fast16_t height2 = font->font_drawheight(font);
	unsigned bytestotal = (width2 * height2 + 7) / 8;
	PRINTF("cpfont_bitpacknormal_getcharraster: ci=%u, bytespervertical=%u, w2/h2=%u/%u\n", ci, bytespervertical, width2, height2);
	printhex(paramstart, blob + paramstart, PARAMSIZE);
	printhex(rastersstart, blob + rastersstart, bytestotal);
	return blob + rastersstart;
}

uint_fast16_t cpfont_bitpacknormal_draw(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, const struct unifont_tag * font, uint_fast16_t ci, COLORPIP_T fg)
{
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	const uint_fast16_t paramstart = cpfont_bitpacknormal_paramstart(font, ci);	// параметры для данного символа
	const uint_fast16_t width2 = blob [paramstart + 2];
	const uint_fast16_t height2 = font->font_drawheight(font);
	const uint8_t * raster = font->getcharrasterci(font, ci);
	if (raster)
	{
		//printhex(0, raster, 32);
	}
	return xpix + width2;
}


const unifont_t unifont_Tahoma_Regular_88x77 =
{
	.decode = cpfont_bitpacknormal_decode,
	.totalci = cpfont_bitpacknormal_totalci,
	.getcharrasterci = cpfont_bitpacknormal_getcharraster,
	.font_drawwidthci = cpfont_bitpacknormal_width,
	.font_drawheight = cpfont_bitpacknormal_height,
	.font_drawci = cpfont_bitpacknormal_draw,
	//
	.fontraster = Tahoma_Regular_88x77,
	.label = "Tahoma_Regular_88x77"
};

#endif

#endif /* LCDMODE_LTDC */

// **********************************

