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

#define FONTSHERE 1

#include "fontmaps.h"

#include <string.h>

/* valid chars: "0123456789 #._" */
// Возвращает индекс символа в знакогенераторе
static uint_fast16_t
bighalffont_decode(const unifont_t * font, char cc)
{
	(void) font;
	const uint_fast8_t c = (unsigned char) cc;
	// '#' - узкий пробел
	if (c == ' ' || c == '#')
		return 11;
	if (c == '_')
		return 10;		// курсор - позиция редактирвания частоты
	if (c == '.')
		return 12;		// точка
	if (c > '9')
		return 10;		// ошибка - курсор - позиция редактирвания частоты
	return c - '0';		// остальные - цифры 0..9
}

// Возвращает индекс символа в знакогенераторе
static uint_fast16_t
smallfont_decode(const unifont_t * font, char cc)
{
	(void) font;
	const uint_fast8_t c = (unsigned char) cc;
	if (c < ' ' || c > 0x7F)
		return '$' - ' ';
	return c - ' ';
}

#if defined (BIGCHARW_NARROW) && defined (BIGCHARW)
static uint_fast8_t bigfont_width(const unifont_t * font, char cc)
{
	(void) font;
	(void) cc;
	return (cc == '.' || cc == '#') ? BIGCHARW_NARROW  : BIGCHARW;	// полная ширина символа в пикселях
}

static uint_fast8_t bigfont_height(const unifont_t * font, char cc)
{
	(void) font;
	(void) cc;
	return BIGCHARH;
}
#endif /* defined (BIGCHARW_NARROW) && defined (BIGCHARW) */

#if defined (HALFCHARW)
static uint_fast8_t halffont_width(const unifont_t * font, char cc)
{
	(void) font;
	(void) cc;
	return HALFCHARW;	// полная ширина символа в пикселях
}

static uint_fast8_t halffont_height(const unifont_t * font, char cc)
{
	(void) font;
	(void) cc;
	return HALFCHARH;
}

#endif /* defined (HALFCHARW) */

#if defined (SMALLCHARW)
static uint_fast8_t smallfont_width(const unifont_t * font, char cc)
{
	(void) font;
	(void) cc;
	return SMALLCHARW;	// полная ширина символа в пикселях
}

static uint_fast8_t smallfont_height(const unifont_t * font, char cc)
{
	(void) font;
	(void) cc;
	return SMALLCHARH;
}

#endif /* defined (SMALLCHARW) */

// Тривиальная функция получения начала растра символа в массиве шрифта - вск символы с одинаковыми размерами
// Для моноширинных знакогенераторов
static const void * unifont_getcharraster(const unifont_t * font, char cc)
{
	const uint_fast16_t ci = font->decode(font, cc);
	const uint8_t * const charraster = (const uint8_t *) font->fontraster + ci * font->font_charheight(font, cc) * font->bytesw;
	return charraster;
}

// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
// Фон не трогаем
static void ltdc_horizontal_pixels_tbg(
	PACKEDCOLORPIP_T * __restrict tgr,		// target raster
	const uint8_t * __restrict raster,
	uint_fast16_t width,	// number of bits (start from LSB first byte in raster)
	COLORPIP_T fg
	)
{
	uint_fast16_t w = width;

	for (; w >= 8; w -= 8, tgr += 8)
	{
		const uint_fast8_t v = * raster ++;
		if (v & 0x01)	tgr [0] = fg;
		if (v & 0x02)	tgr [1] = fg;
		if (v & 0x04)	tgr [2] = fg;
		if (v & 0x08)	tgr [3] = fg;
		if (v & 0x10)	tgr [4] = fg;
		if (v & 0x20)	tgr [5] = fg;
		if (v & 0x40)	tgr [6] = fg;
		if (v & 0x80)	tgr [7] = fg;
	}
	if (w != 0)
	{
		uint_fast8_t vlast = * raster;
		do
		{
			if (vlast & 0x01)
				* tgr = fg;
			tgr += 1;
			vlast >>= 1;
		} while (-- w);
	}
}

// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
// Фон не трогаем
static void ltdc_horizontal_pixels16_tbg(
	PACKEDCOLORPIP_T * __restrict tgr,		// target raster
	const uint16_t * __restrict raster,
	uint_fast16_t width,	// number of bits (start from LSB first byte in raster)
	COLORPIP_T fg
	)
{
	uint_fast16_t w = width;

	for (; w >= 16; w -= 16, tgr += 16)
	{
		const uint_fast16_t v = * raster ++;
		if (v & 0x0001)	tgr [0x00] = fg;
		if (v & 0x0002)	tgr [0x01] = fg;
		if (v & 0x0004)	tgr [0x02] = fg;
		if (v & 0x0008)	tgr [0x03] = fg;
		if (v & 0x0010)	tgr [0x04] = fg;
		if (v & 0x0020)	tgr [0x05] = fg;
		if (v & 0x0040)	tgr [0x06] = fg;
		if (v & 0x0080)	tgr [0x07] = fg;
		if (v & 0x0100)	tgr [0x08] = fg;
		if (v & 0x0200)	tgr [0x09] = fg;
		if (v & 0x0400)	tgr [0x0A] = fg;
		if (v & 0x0800)	tgr [0x0B] = fg;
		if (v & 0x1000)	tgr [0x0C] = fg;
		if (v & 0x2000)	tgr [0x0D] = fg;
		if (v & 0x4000)	tgr [0x0E] = fg;
		if (v & 0x8000)	tgr [0x0F] = fg;
	}
	if (w != 0)
	{
		uint_fast16_t vlast = * raster;
		do
		{
			if (vlast & 0x01)
				* tgr = fg;
			tgr += 1;
			vlast >>= 1;
		} while (-- w);
	}
}

// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
// Фон не трогаем
// удвоенный по ширине растр
static void ltdc_horizontal_x2_pixels_tbg(
	PACKEDCOLORPIP_T * __restrict tgr,		// target raster
	const uint8_t * __restrict raster,
	uint_fast16_t width,	// number of bits (start from LSB first byte in raster)
	COLORPIP_T fg
	)
{
	uint_fast16_t w = width;

	for (; w >= 8; w -= 8, tgr += 16)
	{
		const uint_fast8_t v = * raster ++;
		if (v & 0x01)	{ tgr [ 0] = tgr [ 1] = fg; }
		if (v & 0x02)	{ tgr [ 2] = tgr [ 3] = fg; }
		if (v & 0x04)	{ tgr [ 4] = tgr [ 5] = fg; }
		if (v & 0x08)	{ tgr [ 6] = tgr [ 7] = fg; }
		if (v & 0x10)	{ tgr [ 8] = tgr [ 9] = fg; }
		if (v & 0x20)	{ tgr [10] = tgr [11] = fg; }
		if (v & 0x40)	{ tgr [12] = tgr [13] = fg; }
		if (v & 0x80)	{ tgr [14] = tgr [15] = fg; }
	}
	if (w != 0)
	{
		uint_fast8_t vlast = * raster;
		do
		{
			if (vlast & 0x01)
				tgr [ 0] = tgr [ 1] = fg;
			tgr += 2;
			vlast >>= 1;
		} while (-- w);
	}
}

// return new x coordinate
static uint_fast16_t
unifont_put_char(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	const uint8_t * const charraster,
	uint_fast16_t width2,
	uint_fast16_t height2,
	uint_fast16_t bytesw,
	COLORPIP_T fg
	)
{
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < height2; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(db, xpix, ypix + cgrow);
		ltdc_horizontal_pixels_tbg(tgr, & charraster [cgrow * bytesw], width2, fg);
	}
	return xpix + width2;
}

// return new x coordinate
static uint_fast16_t
unifont_put_char16(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	const uint16_t * const charraster,
	uint_fast16_t width2,
	uint_fast16_t height2,
	uint_fast16_t bytesw,
	COLORPIP_T fg
	)
{
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < height2; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(db, xpix, ypix + cgrow);
		ltdc_horizontal_pixels16_tbg(tgr, & charraster [cgrow * bytesw], width2, fg);
	}
	return xpix + width2;
}

// return new x coordinate
static uint_fast16_t
unifont_put_char_x2(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	const uint8_t * const charraster,
	uint_fast16_t width2,
	uint_fast16_t height2,
	uint_fast16_t bytesw,
	COLORPIP_T fg
	)
{
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < height2; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr0 = colpip_mem_at(db, xpix, ypix + cgrow * 2 + 0);
		ltdc_horizontal_x2_pixels_tbg(tgr0, & charraster [cgrow * bytesw], width2, fg);
		PACKEDCOLORPIP_T * const tgr1 = colpip_mem_at(db, xpix, ypix + cgrow * 2 + 1);
		ltdc_horizontal_x2_pixels_tbg(tgr1, & charraster [cgrow * bytesw], width2, fg);
	}
	return xpix + width2 * 2;
}

static uint_fast16_t
unifont_put_char_small(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	const uint8_t * const charraster = (const uint8_t *) font->getcharraster(font, cc);
	const uint_fast16_t width2 = font->font_charwidth(font, cc);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_charheight(font, cc);	// number of rows
	const uint_fast16_t bytesw = font->bytesw;	// bytes in each chargen row
	return unifont_put_char(db, xpix, ypix, font, charraster, width2, height2, bytesw, fg);
}

static uint_fast16_t
unifont_put_char_small_x2(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	const uint8_t * const charraster = (const uint8_t *) font->getcharraster(font, cc);
	const uint_fast16_t width2 = font->font_charwidth(font, cc);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_charheight(font, cc);	// number of rows
	const uint_fast16_t bytesw = font->bytesw;	// bytes in each chargen row
	return unifont_put_char_x2(db, xpix, ypix, font, charraster, width2, height2, bytesw, fg);
}


#if WITHPRERENDER
/* использование предварительно построенных изображений при отображении частоты */

enum { RENDERCHARS = 14 }; /* valid chars: "0123456789 #._" */
static const char renderchars [] = "0123456789 #._";
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
			ASSERT(font->font_prerender);
			font->font_prerender(& dbvbig, xpix, ypix, font, renderchars [ci], fg);	// BIGCHARW
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
			ASSERT(font->font_prerender);
			font->font_prerender(& dbvhalf, xpix, ypix, font, renderchars [ci], fg);	// HALFCHARW
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
	const uint_fast16_t width2 = font->font_charwidth(font, cc);	// number of bits (start from LSB first byte in raster)
//	const uint_fast16_t height2 = font->font_charheight(font, cc);	// number of rows
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
	const uint_fast16_t width2 = font->font_charwidth(font, cc);	// number of bits (start from LSB first byte in raster)
//	const uint_fast16_t height2 = font->font_charheight(font, cc);	// number of rows
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
static uint_fast16_t
unifont_put_char_big_rendered(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	return unifont_put_char_small(db, xpix, ypix, font, cc, fg);
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
	return unifont_put_char_small(db, xpix, ypix, font, cc, fg);
}

#endif /* WITHPRERENDER */

static uint_fast16_t
unifont_put_char_bighalf_prerender(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	const uint_fast16_t ci = font->decode(font, cc);
	const uint8_t * const charraster = (const uint8_t *) font->fontraster + ci * font->font_charheight(font, cc) * font->bytesw;
	const uint_fast16_t width2 = font->font_charwidth(font, cc);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_charheight(font, cc);	// number of rows
	const uint_fast16_t bytesw = font->bytesw;	// bytes in each chargen row
	return unifont_put_char(db, xpix, ypix, font, charraster, width2, height2, bytesw, fg);
}

uint_fast16_t colorpip_put_char_any(
	const gxdrawb_t * db,
	uint_fast16_t xpix,
	uint_fast16_t ypix,
	const unifont_t * font,
	char cc,
	COLORPIP_T fg
	)
{
	savewhere = __func__;
	return font->font_draw(db, xpix, ypix, font, cc, fg);
}

#if defined (SMALLCHARW)
// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
uint_fast16_t colorpip_put_char_small(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	savewhere = __func__;
	return colorpip_put_char_any(db, xpix, ypix, & unifont_small, cc, fg);
}

// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
uint_fast16_t colorpip_x2_put_char_small(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	const unifont_t * const font = & unifont_small_x2;
	return font->font_draw(db, xpix, ypix, font, cc, fg);
}

// Возвращает ширину строки в пикселях
uint_fast16_t strwidth(
	const char * s
	)
{
	ASSERT(s != NULL);
	const unifont_t * const font = & unifont_small;
	uint_fast16_t w = 0;
	char cc;
	while ((cc = * s ++) != '\0')
		w += font->font_charwidth(font, cc);
	return w;
}

#endif /* defined (SMALLCHARW) */


#if defined (SMALLCHARH2)

uint_fast8_t smallfont2_width(const unifont_t * font, char cc)
{
	(void) cc;
	return SMALLCHARW2;	// полная ширина символа в пикселях
}

uint_fast8_t smallfont2_height(const unifont_t * font, char cc)
{
	return SMALLCHARH2;	// полная ширина символа в пикселях
}

// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
uint_fast16_t colorpip_put_char_small2(
	const gxdrawb_t * db,
	uint_fast16_t xpix,
	uint_fast16_t ypix,
	char cc,
	COLORPIP_T fg
	)
{
	savewhere = __func__;
	return colorpip_put_char_any(db, xpix, ypix, & unifont_small2, cc, fg);
}

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string2_tbg(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	)
{
	char c;
	ASSERT(s != NULL);
	while ((c = * s ++) != '\0')
	{
		x = colorpip_put_char_small2(db, x, y, c, fg);
	}
}

// Возвращает ширину строки в пикселях
uint_fast16_t strwidth2(
	const char * s
	)
{
	ASSERT(s != NULL);
	const unifont_t * const font = & unifont_small2;
	uint_fast16_t w = 0;
	char cc;
	while ((cc = * s ++) != '\0')
		w += font->font_charwidth(font, cc);
	return w;
}

#endif /* defined (SMALLCHARW2) */



#if SMALLCHARH3

uint_fast8_t smallfont3_width(const unifont_t * font, char cc)
{
	(void) cc;
	return SMALLCHARW3;	// полная ширина символа в пикселях
}

uint_fast8_t smallfont3_height(const unifont_t * font, char cc)
{
	return SMALLCHARH3;	// полная ширина символа в пикселях
}

static uint_fast16_t
ltdc_horizontal_put_char_small3(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,
	char cc,
	COLORPIP_T fg
	)
{
	savewhere = __func__;
	return colorpip_put_char_any(db, xpix, ypix, & unifont_small3, cc, fg);
}

// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
static uint_fast16_t colorpip_put_char_small3_tbg(
	const gxdrawb_t * db,
	uint_fast16_t xpix,
	uint_fast16_t ypix,
	char cc,
	COLORPIP_T fg
	)
{
	const unifont_t * const font = & unifont_small3;
	return font->font_draw(db, xpix, ypix, font, cc, fg);
}

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string3_tbg(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	)
{
	char c;

	ASSERT(s != NULL);
	while ((c = * s ++) != '\0')
	{
		x = colorpip_put_char_small3_tbg(db, x, y, c, fg);
	}
}

void
display_string3(
	const gxdrawb_t * db,
	uint_fast16_t x,
	uint_fast16_t y,
	uint_fast16_t w,
	uint_fast16_t h,
	const char * __restrict s,
	COLORPIP_T fg, COLORPIP_T bg
	)
{
	char c;
	colpip_fillrect(db, x, y, w, h, bg);
	while ((c = * s ++) != '\0')
		x = colorpip_put_char_small3_tbg(db, x, y, c, fg);
}

// Возвращает ширину строки в пикселях
uint_fast16_t strwidth3(
	const char * s
	)
{

	ASSERT(s != NULL);
	const unifont_t * const font = & unifont_small3;
	uint_fast16_t w = 0;
	char cc;
	while ((cc = * s ++) != '\0')
		w += font->font_charwidth(font, cc);
	return w;
}

#endif /* defined (SMALLCHARW3) */



#if defined (SMALLCHARW)

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string_tbg(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	)
{
	char c;

	ASSERT(s != NULL);
	while ((c = * s ++) != '\0')
	{
		x = colorpip_put_char_small(db, x, y, c, fg);
	}
}

#endif /* defined (SMALLCHARW) */


// обычный шрифт
uint_fast16_t display_put_char_small(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, char cc, const gxstyle_t * dbstyle)
{
	const unifont_t * const font = dbstyle->font;
	if (font == NULL)
		return x;
	savewhere = __func__;
	return font->font_draw(db, x, y, font, cc, dbstyle->textcolor);
}

// большой шрифт
uint_fast16_t display_put_char_big(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, char cc, const gxstyle_t * dbstyle)
{
	const unifont_t * const font = & unifont_big;
	if (font == NULL)
		return x;
	savewhere = __func__;
	return font->font_draw(db, x, y, font, cc, dbstyle->textcolor);
}

uint_fast16_t display_put_char_half(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, char cc, const gxstyle_t * dbstyle)
{
	const unifont_t * const font = & unifont_half;
	if (font == NULL)
		return x;
	savewhere = __func__;
	return font->font_draw(db, x, y, font, cc, dbstyle->textcolor);
}

// обычный шрифт
uint_fast16_t display_wrdata_begin(uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp)
{
	* yp = GRID2Y(ycell);
	return GRID2X(xcell);
}

const unifont_t unifont_big =
{
	.decode = bighalffont_decode,
	.getcharraster = unifont_getcharraster,
	.font_charwidth = bigfont_width,
	.font_charheight = bigfont_height,
#if WITHALTERNATIVEFONTS
	.bytesw = sizeof ltdc_CenturyGothic_big [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = ltdc_CenturyGothic_big [0] [0],		// начало знакогенератора в памяти
#else /* WITHALTERNATIVEFONTS */
	.bytesw = sizeof S1D13781_bigfont_LTDC [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = S1D13781_bigfont_LTDC [0] [0],		// начало знакогенератора в памяти
#endif /* WITHALTERNATIVEFONTS */
	.font_draw = unifont_put_char_big_rendered,
	.font_prerender = unifont_put_char_bighalf_prerender,
	.label = "unifont_big"
};

const unifont_t unifont_half =
{
	.decode = bighalffont_decode,
	.getcharraster = unifont_getcharraster,
	.font_charwidth = halffont_width,
	.font_charheight = halffont_height,
#if WITHALTERNATIVEFONTS
	.bytesw = sizeof ltdc_CenturyGothic_half [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = ltdc_CenturyGothic_half [0] [0],		// начало знакогенератора в памяти
#else /* WITHALTERNATIVEFONTS */
	.bytesw = sizeof S1D13781_halffont_LTDC [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = S1D13781_halffont_LTDC [0] [0],		// начало знакогенератора в памяти
#endif /* WITHALTERNATIVEFONTS */
	.font_draw = unifont_put_char_half_rendered,
	.font_prerender = unifont_put_char_bighalf_prerender,
	.label = "unifont_half"
};

#if SMALLCHARH

const unifont_t unifont_small =
{
	.decode = smallfont_decode,
	.getcharraster = unifont_getcharraster,
	.font_charwidth = smallfont_width,
	.font_charheight = smallfont_height,
	.bytesw = sizeof S1D13781_smallfont_LTDC [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = S1D13781_smallfont_LTDC [0] [0],		// начало знакогенератора в памяти
	.font_draw = unifont_put_char_small,
	.font_prerender = NULL,
	.label = "unifont_small"
};

const unifont_t unifont_small_x2 =
{
	.decode = smallfont_decode,
	.getcharraster = unifont_getcharraster,
	.font_charwidth = smallfont_width,
	.font_charheight = smallfont_height,
	.bytesw = sizeof S1D13781_smallfont_LTDC [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = S1D13781_smallfont_LTDC [0] [0],		// начало знакогенератора в памяти
	.font_draw = unifont_put_char_small_x2,
	.font_prerender = NULL,
	.label = "unifont_small_x2"
};
#endif /* SMALLCHARH */

#if SMALLCHARH2

const unifont_t unifont_small2 =
{
	.decode = smallfont_decode,
	.getcharraster = unifont_getcharraster,
	.font_charwidth = smallfont2_width,
	.font_charheight = smallfont2_height,
	.bytesw = sizeof S1D13781_smallfont2_LTDC [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = S1D13781_smallfont2_LTDC [0] [0],		// начало знакогенератора в памяти
	.font_draw = unifont_put_char_small,
	.font_prerender = NULL,
	.label = "unifont_small2"
};
#endif /* SMALLCHARH2 */

#if SMALLCHARH3

const unifont_t unifont_small3 =
{
	.decode = smallfont_decode,
	.getcharraster = unifont_getcharraster,
	.font_charwidth = smallfont3_width,
	.font_charheight = smallfont3_height,
	.bytesw = sizeof S1D13781_smallfont3_LTDC [0][0],		// байтов в одной строке знакогенератора символа
	.fontraster = S1D13781_smallfont3_LTDC [0],		// начало знакогенератора в памяти
	.font_draw = unifont_put_char_small,
	.font_prerender = NULL,
	.label = "unifont_small3"
};
#endif /* SMALLCHARH3 */


#if WITHALTERNATIVEFONTS

//--------------------------------------------------------------
// Рисует ASCII символ шрифтом одного размера на позиции х, у.
// Цвет шрифта и фон (шрифт = макс 16 пикселей в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawChar(const gxdrawb_t * db,
		uint16_t x, uint16_t y, uint8_t ascii, const UB_Font *font, COLORPIP_T vg)
{
	uint_fast16_t xn,yn,start_maske,maske;
  const uint16_t *wert;


  ascii -= 32;
  wert=&font->table[ascii * font->height];

    start_maske=0x80;
    if (font->width>8) start_maske=0x8000;

    for(yn = 0; yn < font->height; yn++) {
      maske=start_maske;

      for(xn = 0; xn < font->width; xn++) {
        if((wert[yn] & maske) != 0x00)
        	* colpip_mem_at(db, x + xn, yn + y) = vg;

        maske=(maske>>1);
      }
    }

}


//--------------------------------------------------------------
// Рисует строку шрифтом одного размера на позиции х, у.
// Цвет шрифта и фон (шрифт = макс 16 пикселей в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawString(const gxdrawb_t * db,
	uint_fast16_t x, uint_fast16_t y, const char *ptr, const UB_Font * font, COLORPIP_T vg)
{
	uint_fast16_t pos = x;

	while (* ptr != 0) {
		UB_Font_DrawChar(db, pos, y, * ptr, font, vg);
		pos += font->width;
		ptr ++;
	}

}


//--------------------------------------------------------------
// Рисует ASCII символ шрифтом одного размера на позиции х, у.
// Цвет шрифта и фон (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawChar32(const gxdrawb_t * db,
		uint_fast16_t x, uint_fast16_t y,
		uint8_t ascii, const UB_Font32 *font, COLORPIP_T vg)
{
	uint_fast16_t xn,yn;
  uint_fast32_t start_maske,maske;
  const uint32_t *wert;


  ascii -= 32;
  wert=&font->table[ascii * font->height];

    start_maske=0x80;
    if(font->width>8) start_maske=0x8000;
    if(font->width>16) start_maske=0x80000000;

    for(yn = 0; yn < font->height; yn++) {
      maske=start_maske;
      // Установка курсора

      for(xn = 0; xn < font->width; xn++) {
        if((wert[yn] & maske))

        	* colpip_mem_at(db, x + xn, yn + y) = vg;

        maske=(maske>>1);
      }
    }

}


//--------------------------------------------------------------
// Рисует строку шрифтом одного размера на позиции х, у.
// Цвет шрифта и фон (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawString32(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y,
		const char *ptr, const UB_Font32 *font, COLORPIP_T vg)
{
	uint_fast16_t pos;

    pos=x;
    while (*ptr != '\0') {
      UB_Font_DrawChar32(db, pos,y,*ptr,font,vg);
      pos+=font->width;
      ptr++;
    }

}

//--------------------------------------------------------------
// Рисование ASCII символ пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 16 пикселей в ширину)
// Шрифт должен быть передан с оператором &
// Возвращает: ширину нарисованного символа
//--------------------------------------------------------------
uint16_t UB_Font_DrawPChar(const gxdrawb_t * db,
		uint_fast16_t x, uint_fast16_t y,
		uint8_t ascii, const UB_pFont * font,
		COLORPIP_T vg)
{
	uint_fast16_t xn, yn, start_maske, maske, width;
	const uint16_t * wert;

	// Проверка границы символа
	if(ascii < font->first_char)
		return 0;

	if(ascii > font->last_char)
		return 0;

	ascii -= font->first_char;
	wert = & font->table [ascii * (font->height + 1)];
	width = wert [0];


	start_maske = 0x01;
	start_maske = start_maske << (width - 1);

	for(yn = 0; yn < font->height; yn++)
	{
		maske=start_maske;
		// Установка курсора

		for(xn = 0; xn < width; xn++)
		{
			if((wert [yn+1] & maske))
				* colpip_mem_at(db, x + xn, yn + y) = vg;

			maske = (maske >> 1);
		}
	}

	return(width);
}


//--------------------------------------------------------------
// Рисование строки пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 16 пикселей в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawPStringDbg(
		const char * file, int line,
		const gxdrawb_t * db,
		uint_fast16_t x, uint_fast16_t y,
		const char * ptr, const UB_pFont * font,
		COLORPIP_T vg)
{
	uint_fast16_t pos = x, width;

	savestring = ptr;
	savewhere = __func__;
	if (x >= db->dx)
	{
		PRINTF("%s called from %s/%d: x/dx=%d/%d\n", __func__, file, line, (int) x, (int) db->dx);
	}
	if (y >= db->dy)
	{
		PRINTF("%s called from %s/%d: y/dy=%d/%d\n", __func__, file, line, (int) y, (int) db->dy);
	}
	ASSERT(y < db->dy);
	while (*ptr != '\0')
	{
		ASSERT(pos < db->dx);
		width = UB_Font_DrawPChar(db, pos, y, * ptr, font, vg);
		pos += width;
		ptr ++;
	}
}

//--------------------------------------------------------------
// Рисование ASCII символ пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
// Возвращает: ширину нарисованного символа
//--------------------------------------------------------------
uint16_t UB_Font_DrawPChar32(const gxdrawb_t * db,
		uint_fast16_t x, uint_fast16_t y,
		uint8_t ascii, const UB_pFont32 * font,
		COLORPIP_T vg)
{
	uint_fast16_t xn, yn, width;
	uint_fast32_t start_maske, maske;
	const uint32_t * wert;

	// Проверка границы символа
	if(ascii<font->first_char)
		return 0;

	if(ascii>font->last_char)
		return 0;

	ascii -= font->first_char;
	wert = & font->table [ascii * (font->height + 1)];
	width = wert [0];
	start_maske = 0x01;
	start_maske = start_maske << (width - 1);

	for(yn = 0; yn < font->height; yn++)
	{
		maske=start_maske;
		// Установка курсора

		for(xn = 0; xn < width; xn++)
		{
			if((wert [yn + 1] & maske))
				* colpip_mem_at(db, x + xn, yn + y) = vg;

			maske = (maske >> 1);
		}
	}

	return(width);
}

//--------------------------------------------------------------
// Рисование строку пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawPString32(const gxdrawb_t * db,
		uint_fast16_t x, uint_fast16_t y,
		const char * ptr, const UB_pFont32 * font,
		COLORPIP_T vg)
{
	uint_fast16_t pos = x, width;
	savestring = ptr;
	savewhere = __func__;
	while (* ptr != 0)
	{
		ASSERT(pos < db->dx);
		width = UB_Font_DrawPChar32(db, pos, y, * ptr, font, vg);
		pos += width;
		ptr ++;
	}
}

uint16_t UB_Font_getPcharw32(uint8_t ascii, const UB_pFont32 * font)
{
	uint_fast16_t width;
	uint32_t start_maske, maske;
	const uint32_t * wert;

	// Проверка границы символа
	if(ascii < font->first_char)
		return 0;

	if(ascii > font->last_char)
		return 0;

	ascii -= font->first_char;
	wert = & font->table [ascii * (font->height + 1)];
	width = wert [0];

	return(width);
}

// Возврат ширины строки в пикселях, пропорциональный шрифт 32 бит
uint16_t getwidth_Pstring32(const char * str, const UB_pFont32 * font)
{
	uint_fast16_t width = 0;
	savestring = str;
	savewhere = __func__;
	while (* str != 0)
	{
		width += UB_Font_getPcharw32(* str, font);
		str ++;
	}

	return width;
}

uint16_t UB_Font_getPcharw(uint8_t ascii, const UB_pFont * font)
{
	uint_fast16_t width;
	uint_fast32_t start_maske, maske;
	const uint16_t * wert;

	// Проверка границы символа
	if(ascii < font->first_char)
		return 0;

	if(ascii > font->last_char)
		return 0;

	ascii -= font->first_char;
	wert = & font->table [ascii * (font->height + 1)];
	width = wert [0];

	return(width);
}

// Возврат ширины строки в пикселях, пропорциональный шрифт меньше 32 бит
uint16_t getwidth_Pstring(const char * str, const UB_pFont * font)
{
	uint_fast16_t width = 0;
	ASSERT(str != NULL);
	ASSERT(font != NULL);
	while (* str != 0)
	{
		width += UB_Font_getPcharw(* str, font);
		str ++;
	}

	return width;
}

// Возвращает ширину строки в пикселях, моноширинный шрифт
uint16_t getwidth_Mstring(const char * str, const UB_Font * font)
{
	ASSERT(str != NULL);
	return strlen(str) * font->width;
}

// *********************************************************************************************************************

//--------------------------------------------------------------
// Font-Daten
// erstellt von UB mit PixelFontGenerator 1.8
// Source-Font :
// Name:MS PGothic  /  Size:13  /  Style:[B]
// First-Ascii : 32
// Last-Ascii  : 126
//--------------------------------------------------------------
const uint16_t gothic_12x16_Table[] = {
0x06,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:32 = [ ]
0x05,0x0000,0x0000,0x0000,0x0018,0x0018,0x0018,0x0018,0x0018,0x0018,0x0018,0x0018,0x0000,0x0000,0x0018,0x0000,0x0000, // Ascii:33 = [!]
0x0A,0x0000,0x00D8,0x01B0,0x0360,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:34 = ["]
0x0A,0x0000,0x0000,0x0000,0x00D8,0x00D8,0x00D8,0x03FC,0x00D8,0x01B0,0x01B0,0x03FC,0x01B0,0x01B0,0x01B0,0x0000,0x0000, // Ascii:35 = [#]
0x0A,0x0000,0x0000,0x00C0,0x01F0,0x03D8,0x03D8,0x03C0,0x01C0,0x00E0,0x00F0,0x03D8,0x03D8,0x03D8,0x01F0,0x00C0,0x0000, // Ascii:36 = [$]
0x0A,0x0000,0x0000,0x0000,0x0198,0x03F0,0x03F0,0x03F0,0x03E0,0x01F8,0x00FC,0x00FC,0x00FC,0x01BC,0x0198,0x0000,0x0000, // Ascii:37 = [%]
0x0B,0x0000,0x0000,0x0000,0x01E0,0x0330,0x0330,0x0360,0x01C0,0x0380,0x06CC,0x066C,0x063C,0x061C,0x03F6,0x0000,0x0000, // Ascii:38 = [&]
0x04,0x0000,0x0000,0x0000,0x0006,0x0006,0x000C,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:39 = [']
0x06,0x0006,0x000C,0x0018,0x0018,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0018,0x0018,0x000C,0x0006,0x0000, // Ascii:40 = [(]
0x06,0x0030,0x0018,0x000C,0x000C,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x000C,0x000C,0x0018,0x0030,0x0000, // Ascii:41 = [)]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0060,0x0060,0x03FC,0x00F0,0x00F0,0x03FC,0x0060,0x0060,0x0000,0x0000,0x0000,0x0000, // Ascii:42 = [*]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0060,0x0060,0x0060,0x03FC,0x0060,0x0060,0x0060,0x0000,0x0000,0x0000,0x0000, // Ascii:43 = [+]
0x04,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0006,0x0006,0x000C, // Ascii:44 = [,]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x03FC,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:45 = [-]
0x04,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0006,0x0000,0x0000, // Ascii:46 = [.]
0x0A,0x0000,0x0000,0x000C,0x0018,0x0018,0x0030,0x0030,0x0030,0x0060,0x0060,0x00C0,0x00C0,0x0180,0x0180,0x0300,0x0000, // Ascii:47 = [/]
0x0A,0x0000,0x0000,0x0000,0x00F0,0x0198,0x030C,0x030C,0x030C,0x030C,0x030C,0x030C,0x030C,0x0198,0x00F0,0x0000,0x0000, // Ascii:48 = [0]
0x0A,0x0000,0x0000,0x0000,0x0060,0x00E0,0x01E0,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0000,0x0000, // Ascii:49 = [1]
0x0A,0x0000,0x0000,0x0000,0x00E0,0x01B0,0x0318,0x0318,0x0018,0x0030,0x0060,0x00C0,0x0180,0x0300,0x03F8,0x0000,0x0000, // Ascii:50 = [2]
0x0A,0x0000,0x0000,0x0000,0x01F0,0x0318,0x0318,0x0018,0x0018,0x0070,0x0018,0x0318,0x0318,0x0318,0x01F0,0x0000,0x0000, // Ascii:51 = [3]
0x0A,0x0000,0x0000,0x0000,0x0018,0x0038,0x0078,0x00D8,0x00D8,0x0198,0x0318,0x03FC,0x0018,0x0018,0x0018,0x0000,0x0000, // Ascii:52 = [4]
0x0A,0x0000,0x0000,0x0000,0x03F8,0x0300,0x0300,0x03F0,0x0318,0x0018,0x0018,0x0018,0x0318,0x0318,0x01F0,0x0000,0x0000, // Ascii:53 = [5]
0x0A,0x0000,0x0000,0x0000,0x00F0,0x0198,0x0318,0x0300,0x03F0,0x0398,0x0318,0x0318,0x0318,0x0198,0x00F0,0x0000,0x0000, // Ascii:54 = [6]
0x0A,0x0000,0x0000,0x0000,0x03F8,0x0018,0x0018,0x0030,0x0030,0x0030,0x0060,0x0060,0x0060,0x00C0,0x00C0,0x0000,0x0000, // Ascii:55 = [7]
0x0A,0x0000,0x0000,0x0000,0x00F0,0x0198,0x030C,0x030C,0x0198,0x00F0,0x0198,0x030C,0x030C,0x0198,0x00F0,0x0000,0x0000, // Ascii:56 = [8]
0x0A,0x0000,0x0000,0x0000,0x01E0,0x0330,0x0318,0x0318,0x0318,0x0338,0x01F8,0x0018,0x0318,0x0330,0x01E0,0x0000,0x0000, // Ascii:57 = [9]
0x04,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0006,0x0000,0x0000,0x0000,0x0000,0x0000,0x0006,0x0000,0x0000, // Ascii:58 = [:]
0x04,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0006,0x0000,0x0000,0x0000,0x0000,0x0000,0x0006,0x0006,0x000C, // Ascii:59 = [;]
0x0A,0x0000,0x0000,0x0000,0x0018,0x0030,0x0060,0x00C0,0x0180,0x0300,0x0180,0x00C0,0x0060,0x0030,0x0018,0x0000,0x0000, // Ascii:60 = [<]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x03FC,0x0000,0x0000,0x0000,0x03FC,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:61 = [=]
0x0A,0x0000,0x0000,0x0000,0x0300,0x0180,0x00C0,0x0060,0x0030,0x0018,0x0030,0x0060,0x00C0,0x0180,0x0300,0x0000,0x0000, // Ascii:62 = [>]
0x09,0x0000,0x0000,0x0000,0x00F8,0x018C,0x018C,0x000C,0x0018,0x0030,0x0060,0x0060,0x0000,0x0000,0x0060,0x0000,0x0000, // Ascii:63 = [?]
0x0C,0x0000,0x0000,0x0000,0x01E0,0x0738,0x06F8,0x0DBC,0x0F3C,0x0F6C,0x0F6C,0x0F6C,0x07F8,0x0700,0x01F0,0x0000,0x0000, // Ascii:64 = [@]
0x0C,0x0000,0x0000,0x0000,0x00E0,0x00E0,0x01B0,0x01B0,0x01B0,0x0318,0x0318,0x07FC,0x060C,0x060C,0x0C06,0x0000,0x0000, // Ascii:65 = [A]
0x0C,0x0000,0x0000,0x0000,0x07F0,0x0618,0x060C,0x060C,0x0618,0x07F0,0x0618,0x060C,0x060C,0x0618,0x07F0,0x0000,0x0000, // Ascii:66 = [B]
0x0C,0x0000,0x0000,0x0000,0x01F0,0x0318,0x060C,0x0C0C,0x0C00,0x0C00,0x0C00,0x0C0C,0x060C,0x0318,0x01F0,0x0000,0x0000, // Ascii:67 = [C]
0x0C,0x0000,0x0000,0x0000,0x07E0,0x0630,0x0618,0x060C,0x060C,0x060C,0x060C,0x060C,0x0618,0x0630,0x07E0,0x0000,0x0000, // Ascii:68 = [D]
0x0B,0x0000,0x0000,0x0000,0x03FC,0x0300,0x0300,0x0300,0x0300,0x03F8,0x0300,0x0300,0x0300,0x0300,0x03FC,0x0000,0x0000, // Ascii:69 = [E]
0x0A,0x0000,0x0000,0x0000,0x01FE,0x0180,0x0180,0x0180,0x0180,0x01FC,0x0180,0x0180,0x0180,0x0180,0x0180,0x0000,0x0000, // Ascii:70 = [F]
0x0D,0x0000,0x0000,0x0000,0x03E0,0x0630,0x0C18,0x1818,0x1800,0x1878,0x1818,0x1818,0x0C18,0x0638,0x03F8,0x0000,0x0000, // Ascii:71 = [G]
0x0C,0x0000,0x0000,0x0000,0x060C,0x060C,0x060C,0x060C,0x060C,0x07FC,0x060C,0x060C,0x060C,0x060C,0x060C,0x0000,0x0000, // Ascii:72 = [H]
0x05,0x0000,0x0000,0x0000,0x001E,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x001E,0x0000,0x0000, // Ascii:73 = [I]
0x0A,0x0000,0x0000,0x0000,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x030C,0x030C,0x0198,0x00F0,0x0000,0x0000, // Ascii:74 = [J]
0x0B,0x0000,0x0000,0x0000,0x030C,0x0318,0x0330,0x0360,0x03C0,0x03E0,0x0330,0x0330,0x0318,0x030C,0x0306,0x0000,0x0000, // Ascii:75 = [K]
0x0A,0x0000,0x0000,0x0000,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x01FE,0x0000,0x0000, // Ascii:76 = [L]
0x0E,0x0000,0x0000,0x0000,0x1806,0x1C0E,0x1C0E,0x1E1E,0x1E1E,0x1E1E,0x1B36,0x1B36,0x19E6,0x19E6,0x18C6,0x0000,0x0000, // Ascii:77 = [M]
0x0C,0x0000,0x0000,0x0000,0x060C,0x070C,0x078C,0x078C,0x06CC,0x06CC,0x066C,0x063C,0x063C,0x061C,0x060C,0x0000,0x0000, // Ascii:78 = [N]
0x0D,0x0000,0x0000,0x0000,0x03C0,0x0E70,0x0C30,0x1818,0x1818,0x1818,0x1818,0x1818,0x0C30,0x0E70,0x03C0,0x0000,0x0000, // Ascii:79 = [O]
0x0B,0x0000,0x0000,0x0000,0x03F8,0x030C,0x0306,0x0306,0x0306,0x030C,0x03F8,0x0300,0x0300,0x0300,0x0300,0x0000,0x0000, // Ascii:80 = [P]
0x0D,0x0000,0x0000,0x0000,0x03C0,0x0E70,0x0C30,0x1818,0x1818,0x1818,0x1818,0x18D8,0x0CF0,0x0E60,0x03F0,0x0000,0x0000, // Ascii:81 = [Q]
0x0C,0x0000,0x0000,0x0000,0x07F0,0x0618,0x060C,0x060C,0x0618,0x07F0,0x0630,0x0618,0x0618,0x0618,0x060C,0x0000,0x0000, // Ascii:82 = [R]
0x0B,0x0000,0x0000,0x0000,0x00F8,0x018C,0x0306,0x0300,0x01C0,0x0078,0x000C,0x0306,0x0306,0x018C,0x00F8,0x0000,0x0000, // Ascii:83 = [S]
0x0B,0x0000,0x0000,0x0000,0x07FE,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0000,0x0000, // Ascii:84 = [T]
0x0C,0x0000,0x0000,0x0000,0x060C,0x060C,0x060C,0x060C,0x060C,0x060C,0x060C,0x060C,0x060C,0x0318,0x01F0,0x0000,0x0000, // Ascii:85 = [U]
0x0C,0x0000,0x0000,0x0000,0x0C06,0x060C,0x060C,0x060C,0x0318,0x0318,0x01B0,0x01B0,0x01B0,0x00E0,0x00E0,0x0000,0x0000, // Ascii:86 = [V]
0x0E,0x0000,0x0000,0x0000,0x31C6,0x31C6,0x31C6,0x1B6C,0x1B6C,0x1B6C,0x1E3C,0x1E3C,0x1E3C,0x0C18,0x0C18,0x0000,0x0000, // Ascii:87 = [W]
0x0B,0x0000,0x0000,0x0000,0x0606,0x030C,0x0198,0x00F0,0x00F0,0x0060,0x00F0,0x00F0,0x0198,0x030C,0x0606,0x0000,0x0000, // Ascii:88 = [X]
0x0B,0x0000,0x0000,0x0000,0x0606,0x030C,0x0198,0x0198,0x00F0,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0000,0x0000, // Ascii:89 = [Y]
0x0B,0x0000,0x0000,0x0000,0x07F8,0x0018,0x0030,0x0060,0x0060,0x00C0,0x0180,0x0300,0x0300,0x0600,0x07F8,0x0000,0x0000, // Ascii:90 = [Z]
0x07,0x003E,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x003E,0x0000, // Ascii:91 = [[]
0x0A,0x0000,0x0000,0x0000,0x030C,0x0198,0x0198,0x00F0,0x00F0,0x01F8,0x0060,0x01F8,0x0060,0x0060,0x0060,0x0000,0x0000, // Ascii:92 = [\]
0x07,0x007C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x007C,0x0000, // Ascii:93 = []]
0x08,0x0000,0x0038,0x006C,0x00C6,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:94 = [^]
0x06,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x003E, // Ascii:95 = [_]
0x08,0x0000,0x0070,0x0038,0x000C,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:96 = [`]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00F8,0x018C,0x000C,0x00FC,0x018C,0x019C,0x00FC,0x0000,0x0000, // Ascii:97 = [a]
0x09,0x0000,0x0000,0x0000,0x00C0,0x00C0,0x00C0,0x00C0,0x00FC,0x00E6,0x00C6,0x00C6,0x00C6,0x00E6,0x00FC,0x0000,0x0000, // Ascii:98 = [b]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00F8,0x018C,0x0300,0x0300,0x0300,0x018C,0x00F8,0x0000,0x0000, // Ascii:99 = [c]
0x09,0x0000,0x0000,0x0000,0x000C,0x000C,0x000C,0x000C,0x00FC,0x019C,0x018C,0x018C,0x018C,0x019C,0x00FC,0x0000,0x0000, // Ascii:100 = [d]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00F0,0x0198,0x030C,0x03FC,0x0300,0x018C,0x00F8,0x0000,0x0000, // Ascii:101 = [e]
0x07,0x0000,0x0000,0x0000,0x000F,0x0018,0x0018,0x0018,0x007E,0x0018,0x0018,0x0018,0x0018,0x0018,0x0018,0x0000,0x0000, // Ascii:102 = [f]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x007E,0x00CC,0x00CC,0x00CC,0x0078,0x00C0,0x00FC,0x0186,0x00FC, // Ascii:103 = [g]
0x0A,0x0000,0x0000,0x0000,0x0180,0x0180,0x0180,0x0180,0x01F8,0x01CC,0x018C,0x018C,0x018C,0x018C,0x018C,0x0000,0x0000, // Ascii:104 = [h]
0x05,0x0000,0x0000,0x0000,0x000C,0x000C,0x0000,0x0000,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x0000,0x0000, // Ascii:105 = [i]
0x05,0x0000,0x0000,0x0000,0x0006,0x0006,0x0000,0x0000,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x001C, // Ascii:106 = [j]
0x09,0x0000,0x0000,0x0000,0x00C0,0x00C0,0x00C0,0x00C0,0x00CC,0x00D8,0x00F0,0x00F8,0x00D8,0x00CC,0x00C6,0x0000,0x0000, // Ascii:107 = [k]
0x05,0x0000,0x0000,0x0000,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x0000,0x0000, // Ascii:108 = [l]
0x0D,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0FB8,0x0EEC,0x0CCC,0x0CCC,0x0CCC,0x0CCC,0x0CCC,0x0000,0x0000, // Ascii:109 = [m]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x01F8,0x01CC,0x018C,0x018C,0x018C,0x018C,0x018C,0x0000,0x0000, // Ascii:110 = [n]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00F0,0x0198,0x030C,0x030C,0x030C,0x0198,0x00F0,0x0000,0x0000, // Ascii:111 = [o]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00FC,0x00E6,0x00C6,0x00C6,0x00C6,0x00E6,0x00FC,0x00C0,0x00C0, // Ascii:112 = [p]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00FC,0x019C,0x018C,0x018C,0x018C,0x019C,0x00FC,0x000C,0x000C, // Ascii:113 = [q]
0x07,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0037,0x003C,0x0030,0x0030,0x0030,0x0030,0x0030,0x0000,0x0000, // Ascii:114 = [r]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00F8,0x018C,0x0180,0x00F8,0x000C,0x018C,0x00F8,0x0000,0x0000, // Ascii:115 = [s]
0x07,0x0000,0x0000,0x0000,0x0000,0x0000,0x0018,0x0018,0x007E,0x0018,0x0018,0x0018,0x0018,0x0018,0x000E,0x0000,0x0000, // Ascii:116 = [t]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x018C,0x018C,0x018C,0x018C,0x018C,0x019C,0x00FC,0x0000,0x0000, // Ascii:117 = [u]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0186,0x00CC,0x00CC,0x00CC,0x0078,0x0078,0x0030,0x0000,0x0000, // Ascii:118 = [v]
0x0C,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0CE6,0x0CE6,0x06EC,0x07BC,0x07BC,0x0318,0x0318,0x0000,0x0000, // Ascii:119 = [w]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0186,0x00CC,0x0078,0x0030,0x0078,0x00CC,0x0186,0x0000,0x0000, // Ascii:120 = [x]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0186,0x00CC,0x00CC,0x00CC,0x0078,0x0078,0x0030,0x0030,0x00E0, // Ascii:121 = [y]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x01FC,0x0018,0x0030,0x0060,0x00C0,0x0180,0x01FC,0x0000,0x0000, // Ascii:122 = [z]
0x05,0x000E,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x0018,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000E,0x0000, // Ascii:123 = [{]
0x05,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C, // Ascii:124 = [|]
0x05,0x001C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x0006,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x001C,0x0000, // Ascii:125 = [}]
0x08,0x0076,0x00DE,0x00DC,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:126 = [~]
};


//--------------------------------------------------------------
// Font-Struktur
//--------------------------------------------------------------
UB_pFont gothic_12x16_p = {
  gothic_12x16_Table, // Font-Daten
  16,              // Hoehe eines Zeichens  (in Pixel)
  32,              // erstes Zeichen  (Ascii-Nr)
  126,              // letztes Zeichen (Ascii-Nr)
};

//--------------------------------------------------------------
// Font-Daten
// erstellt von UB mit PixelFontGenerator 1.8
// Source-Font :
// Name:MS PGothic  /  Size:10  /  Style:[B]
// First-Ascii : 32
// Last-Ascii  : 126
//--------------------------------------------------------------
const uint16_t gothic_11x13_Table[] = {
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:32 = [ ]
0x0000,0x0000,0xC000,0xC000,0xC000,0xC000,0xC000,0xC000,0x0000,0x0000,0xC000,0x0000,0x0000, // Ascii:33 = [!]
0x6C00,0xD800,0xD800,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:34 = ["]
0x0000,0x3C00,0x3C00,0xFE00,0x3C00,0x3C00,0x7800,0x7800,0xFE00,0x7800,0x7800,0x0000,0x0000, // Ascii:35 = [#]
0x0000,0x3000,0x7C00,0xF600,0xF600,0x7000,0x3800,0x3C00,0xF600,0xF600,0x7C00,0x3000,0x0000, // Ascii:36 = [$]
0x0000,0x0000,0x6600,0xFC00,0xFC00,0xF800,0x7C00,0x3E00,0x7E00,0x7E00,0xCC00,0x0000,0x0000, // Ascii:37 = [%]
0x0000,0x0000,0x3C00,0x6600,0x6600,0x3C00,0x3800,0x6F00,0xC600,0xC600,0x7F00,0x0000,0x0000, // Ascii:38 = [&]
0x0000,0x0000,0x6000,0x6000,0xC000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:39 = [']
0x3000,0x6000,0x6000,0xC000,0xC000,0xC000,0xC000,0xC000,0xC000,0x6000,0x6000,0x3000,0x0000, // Ascii:40 = [(]
0xC000,0x6000,0x6000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x6000,0x6000,0xC000,0x0000, // Ascii:41 = [)]
0x0000,0x0000,0x0000,0x3000,0xFC00,0x7800,0x7800,0xFC00,0x3000,0x0000,0x0000,0x0000,0x0000, // Ascii:42 = [*]
0x0000,0x0000,0x0000,0x1800,0x1800,0x1800,0xFE00,0x1800,0x1800,0x1800,0x0000,0x0000,0x0000, // Ascii:43 = [+]
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x6000,0x6000,0xC000,0x0000, // Ascii:44 = [,]
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xFE00,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:45 = [-]
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x6000,0x0000,0x0000, // Ascii:46 = [.]
0x0000,0x0600,0x0C00,0x0C00,0x1800,0x1800,0x3000,0x3000,0x6000,0x6000,0xC000,0x0000,0x0000, // Ascii:47 = [/]
0x0000,0x0000,0x3800,0x6C00,0xC600,0xC600,0xC600,0xC600,0xC600,0x6C00,0x3800,0x0000,0x0000, // Ascii:48 = [0]
0x0000,0x0000,0x1800,0x7800,0x1800,0x1800,0x1800,0x1800,0x1800,0x1800,0x1800,0x0000,0x0000, // Ascii:49 = [1]
0x0000,0x0000,0x3C00,0x6600,0xC600,0xC600,0x0C00,0x1800,0x3000,0x6000,0xFE00,0x0000,0x0000, // Ascii:50 = [2]
0x0000,0x0000,0x7C00,0xC600,0xC600,0x0600,0x3C00,0x0600,0xC600,0xC600,0x7C00,0x0000,0x0000, // Ascii:51 = [3]
0x0000,0x0000,0x1C00,0x1C00,0x3C00,0x3C00,0x6C00,0xCC00,0xFE00,0x0C00,0x0C00,0x0000,0x0000, // Ascii:52 = [4]
0x0000,0x0000,0xFE00,0xC000,0xC000,0xF800,0xCC00,0x0600,0x0600,0xCC00,0x7800,0x0000,0x0000, // Ascii:53 = [5]
0x0000,0x0000,0x3C00,0x6600,0xC000,0xFC00,0xE600,0xC600,0xC600,0x6600,0x3C00,0x0000,0x0000, // Ascii:54 = [6]
0x0000,0x0000,0xFE00,0x0600,0x0C00,0x0C00,0x1800,0x1800,0x1800,0x3000,0x3000,0x0000,0x0000, // Ascii:55 = [7]
0x0000,0x0000,0x7C00,0xC600,0xC600,0xC600,0x7C00,0xC600,0xC600,0xC600,0x7C00,0x0000,0x0000, // Ascii:56 = [8]
0x0000,0x0000,0x7800,0xCC00,0xC600,0xC600,0xCE00,0x7E00,0x0600,0xCC00,0x7800,0x0000,0x0000, // Ascii:57 = [9]
0x0000,0x0000,0x0000,0x0000,0x6000,0x0000,0x0000,0x0000,0x0000,0x6000,0x0000,0x0000,0x0000, // Ascii:58 = [:]
0x0000,0x0000,0x0000,0x0000,0x6000,0x0000,0x0000,0x0000,0x0000,0x6000,0x6000,0xC000,0x0000, // Ascii:59 = [;]
0x0000,0x0000,0x0600,0x0C00,0x1800,0x3000,0x6000,0x3000,0x1800,0x0C00,0x0600,0x0000,0x0000, // Ascii:60 = [<]
0x0000,0x0000,0x0000,0x0000,0xFE00,0x0000,0x0000,0xFE00,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:61 = [=]
0x0000,0x0000,0xC000,0x6000,0x3000,0x1800,0x0C00,0x1800,0x3000,0x6000,0xC000,0x0000,0x0000, // Ascii:62 = [>]
0x0000,0x0000,0x7800,0xCC00,0xCC00,0x0C00,0x1800,0x3000,0x3000,0x0000,0x3000,0x0000,0x0000, // Ascii:63 = [?]
0x0000,0x0000,0x1E00,0x7300,0x6F80,0xDF80,0xF780,0xF780,0x7F00,0x7000,0x1F00,0x0000,0x0000, // Ascii:64 = [@]
0x0000,0x0000,0x1800,0x3C00,0x3C00,0x3C00,0x6600,0x6600,0xFF00,0xC300,0xC300,0x0000,0x0000, // Ascii:65 = [A]
0x0000,0x0000,0xFE00,0xC300,0xC300,0xC300,0xFE00,0xC300,0xC300,0xC300,0xFE00,0x0000,0x0000, // Ascii:66 = [B]
0x0000,0x0000,0x3E00,0x6300,0xC180,0xC000,0xC000,0xC180,0xC180,0x6300,0x3E00,0x0000,0x0000, // Ascii:67 = [C]
0x0000,0x0000,0xFC00,0xC600,0xC300,0xC300,0xC300,0xC300,0xC300,0xC600,0xFC00,0x0000,0x0000, // Ascii:68 = [D]
0x0000,0x0000,0xFE00,0xC000,0xC000,0xC000,0xFC00,0xC000,0xC000,0xC000,0xFE00,0x0000,0x0000, // Ascii:69 = [E]
0x0000,0x0000,0xFE00,0xC000,0xC000,0xC000,0xFC00,0xC000,0xC000,0xC000,0xC000,0x0000,0x0000, // Ascii:70 = [F]
0x0000,0x0000,0x3E00,0x6300,0xC180,0xC000,0xCF80,0xC180,0xC180,0x6380,0x3F80,0x0000,0x0000, // Ascii:71 = [G]
0x0000,0x0000,0xC300,0xC300,0xC300,0xC300,0xFF00,0xC300,0xC300,0xC300,0xC300,0x0000,0x0000, // Ascii:72 = [H]
0x0000,0x0000,0xF000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0xF000,0x0000,0x0000, // Ascii:73 = [I]
0x0000,0x0000,0x0600,0x0600,0x0600,0x0600,0x0600,0xC600,0xC600,0x6C00,0x3800,0x0000,0x0000, // Ascii:74 = [J]
0x0000,0x0000,0xC300,0xC600,0xDC00,0xF000,0xF000,0xD800,0xCC00,0xC600,0xC300,0x0000,0x0000, // Ascii:75 = [K]
0x0000,0x0000,0xC000,0xC000,0xC000,0xC000,0xC000,0xC000,0xC000,0xC000,0xFE00,0x0000,0x0000, // Ascii:76 = [L]
0x0000,0x0000,0xC0C0,0xE1C0,0xE1C0,0xF3C0,0xF3C0,0xF3C0,0xDEC0,0xDEC0,0xCCC0,0x0000,0x0000, // Ascii:77 = [M]
0x0000,0x0000,0xC300,0xE300,0xF300,0xF300,0xDB00,0xDB00,0xCF00,0xC700,0xC300,0x0000,0x0000, // Ascii:78 = [N]
0x0000,0x0000,0x3E00,0x6300,0xC180,0xC180,0xC180,0xC180,0xC180,0x6300,0x3E00,0x0000,0x0000, // Ascii:79 = [O]
0x0000,0x0000,0xFE00,0xC300,0xC300,0xC300,0xFE00,0xC000,0xC000,0xC000,0xC000,0x0000,0x0000, // Ascii:80 = [P]
0x0000,0x0000,0x3E00,0x6300,0xC180,0xC180,0xC180,0xCD80,0xCD80,0x6700,0x3D80,0x0000,0x0000, // Ascii:81 = [Q]
0x0000,0x0000,0xFE00,0xC300,0xC300,0xC300,0xFE00,0xCC00,0xC600,0xC600,0xC300,0x0000,0x0000, // Ascii:82 = [R]
0x0000,0x0000,0x7E00,0xC300,0xC300,0x7000,0x1C00,0x0600,0xC300,0xC300,0x7E00,0x0000,0x0000, // Ascii:83 = [S]
0x0000,0x0000,0xFF00,0x1800,0x1800,0x1800,0x1800,0x1800,0x1800,0x1800,0x1800,0x0000,0x0000, // Ascii:84 = [T]
0x0000,0x0000,0xC300,0xC300,0xC300,0xC300,0xC300,0xC300,0xC300,0x6600,0x3C00,0x0000,0x0000, // Ascii:85 = [U]
0x0000,0x0000,0xC300,0xC300,0x6600,0x6600,0x6600,0x3C00,0x3C00,0x3C00,0x1800,0x0000,0x0000, // Ascii:86 = [V]
0x0000,0x0000,0xCCC0,0xCCC0,0xCCC0,0x7EC0,0x7F80,0x7F80,0x7F80,0x3300,0x3300,0x0000,0x0000, // Ascii:87 = [W]
0x0000,0x0000,0xC300,0x6600,0x6600,0x3C00,0x1800,0x3C00,0x6600,0x6600,0xC300,0x0000,0x0000, // Ascii:88 = [X]
0x0000,0x0000,0xC300,0xC300,0x6600,0x6600,0x3C00,0x1800,0x1800,0x1800,0x1800,0x0000,0x0000, // Ascii:89 = [Y]
0x0000,0x0000,0xFE00,0x0600,0x0C00,0x1800,0x1800,0x3000,0x6000,0xC000,0xFE00,0x0000,0x0000, // Ascii:90 = [Z]
0xF000,0xC000,0xC000,0xC000,0xC000,0xC000,0xC000,0xC000,0xC000,0xC000,0xC000,0xF000,0x0000, // Ascii:91 = [[]
0x0000,0x0000,0xCC00,0xCC00,0x7800,0x7800,0xFC00,0x3000,0xFC00,0x3000,0x3000,0x0000,0x0000, // Ascii:92 = [\]
0xF000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0xF000,0x0000, // Ascii:93 = []]
0x0000,0x7000,0xD800,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:94 = [^]
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xF800, // Ascii:95 = [_]
0x0000,0xE000,0x6000,0x3000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:96 = [`]
0x0000,0x0000,0x0000,0x0000,0x0000,0xF800,0x0C00,0x7C00,0xCC00,0xCC00,0x7C00,0x0000,0x0000, // Ascii:97 = [a]
0x0000,0x0000,0xC000,0xC000,0xC000,0xF800,0xCC00,0xCC00,0xCC00,0xCC00,0xF800,0x0000,0x0000, // Ascii:98 = [b]
0x0000,0x0000,0x0000,0x0000,0x0000,0x7C00,0xC600,0xC000,0xC000,0xC600,0x7C00,0x0000,0x0000, // Ascii:99 = [c]
0x0000,0x0000,0x0C00,0x0C00,0x0C00,0x7C00,0xCC00,0xCC00,0xCC00,0xCC00,0x7C00,0x0000,0x0000, // Ascii:100 = [d]
0x0000,0x0000,0x0000,0x0000,0x0000,0x7C00,0xC600,0xFE00,0xC000,0xC600,0x7C00,0x0000,0x0000, // Ascii:101 = [e]
0x0000,0x0000,0x3000,0x6000,0x6000,0xF000,0x6000,0x6000,0x6000,0x6000,0x6000,0x0000,0x0000, // Ascii:102 = [f]
0x0000,0x0000,0x0000,0x0000,0x0000,0x7C00,0xD800,0x7000,0xC000,0xF800,0xCC00,0x7800,0x0000, // Ascii:103 = [g]
0x0000,0x0000,0xC000,0xC000,0xC000,0xFC00,0xE600,0xC600,0xC600,0xC600,0xC600,0x0000,0x0000, // Ascii:104 = [h]
0x0000,0x0000,0x6000,0x6000,0x0000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x0000,0x0000, // Ascii:105 = [i]
0x0000,0x0000,0x6000,0x6000,0x0000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0xC000, // Ascii:106 = [j]
0x0000,0x0000,0xC000,0xC000,0xC000,0xCC00,0xD800,0xF000,0xF800,0xD800,0xCC00,0x0000,0x0000, // Ascii:107 = [k]
0x0000,0x0000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x0000,0x0000, // Ascii:108 = [l]
0x0000,0x0000,0x0000,0x0000,0x0000,0xFB80,0xEEC0,0xCCC0,0xCCC0,0xCCC0,0xCCC0,0x0000,0x0000, // Ascii:109 = [m]
0x0000,0x0000,0x0000,0x0000,0x0000,0xFC00,0xE600,0xC600,0xC600,0xC600,0xC600,0x0000,0x0000, // Ascii:110 = [n]
0x0000,0x0000,0x0000,0x0000,0x0000,0x7C00,0xC600,0xC600,0xC600,0xC600,0x7C00,0x0000,0x0000, // Ascii:111 = [o]
0x0000,0x0000,0x0000,0x0000,0x0000,0xF800,0xCC00,0xCC00,0xCC00,0xCC00,0xF800,0xC000,0xC000, // Ascii:112 = [p]
0x0000,0x0000,0x0000,0x0000,0x0000,0x7C00,0xCC00,0xCC00,0xCC00,0xCC00,0x7C00,0x0C00,0x0C00, // Ascii:113 = [q]
0x0000,0x0000,0x0000,0x0000,0x0000,0xF800,0xE000,0xC000,0xC000,0xC000,0xC000,0x0000,0x0000, // Ascii:114 = [r]
0x0000,0x0000,0x0000,0x0000,0x0000,0x7800,0xCC00,0x6000,0x3800,0xCC00,0x7800,0x0000,0x0000, // Ascii:115 = [s]
0x0000,0x0000,0x0000,0x6000,0x6000,0xF800,0x6000,0x6000,0x6000,0x6000,0x3800,0x0000,0x0000, // Ascii:116 = [t]
0x0000,0x0000,0x0000,0x0000,0x0000,0xC600,0xC600,0xC600,0xC600,0xCE00,0x7E00,0x0000,0x0000, // Ascii:117 = [u]
0x0000,0x0000,0x0000,0x0000,0x0000,0xCC00,0xCC00,0xCC00,0x7800,0x7800,0x3000,0x0000,0x0000, // Ascii:118 = [v]
0x0000,0x0000,0x0000,0x0000,0x0000,0xDB00,0xDB00,0xFF00,0xFF00,0x6600,0x6600,0x0000,0x0000, // Ascii:119 = [w]
0x0000,0x0000,0x0000,0x0000,0x0000,0xCC00,0x7800,0x3000,0x3000,0x7800,0xCC00,0x0000,0x0000, // Ascii:120 = [x]
0x0000,0x0000,0x0000,0x0000,0x0000,0xCC00,0xCC00,0x7800,0x7800,0x3000,0x3000,0x6000,0xC000, // Ascii:121 = [y]
0x0000,0x0000,0x0000,0x0000,0x0000,0xFC00,0x1800,0x3000,0x6000,0xC000,0xFC00,0x0000,0x0000, // Ascii:122 = [z]
0x7000,0x6000,0x6000,0x6000,0x6000,0xE000,0x6000,0x6000,0x6000,0x6000,0x6000,0x7000,0x0000, // Ascii:123 = [{]
0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000, // Ascii:124 = [|]
0xE000,0x6000,0x6000,0x6000,0x6000,0x7000,0x6000,0x6000,0x6000,0x6000,0x6000,0xE000,0x0000, // Ascii:125 = [}]
0x0000,0x0000,0x0000,0x0000,0x7C00,0xF800,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:126 = [~]
};


//--------------------------------------------------------------
// Font-Struktur
//--------------------------------------------------------------
UB_Font gothic_11x13 = {
	gothic_11x13_Table, // Font-Daten
	11,               // Breite eines Zeichens (in Pixel)
	13,              // Hoehe eines Zeichens  (in Pixel)
};

// Для моноширинных знакогенераторов
static uint_fast8_t ubfont_width(const unifont_t * font, char cc)
{
	const UB_Font * const ub = (const UB_Font *) font->fontraster;
	(void) cc;
	return ub->width;	// полная ширина символа в пикселях
}

// Для моноширинных знакогенераторов
static uint_fast8_t ubfont_height(const unifont_t * font, char cc)
{
	const UB_Font * const ub = (const UB_Font *) font->fontraster;
	(void) cc;
	return ub->height;
}

// Возвращает индекс символа в знакогенераторе
// Для моноширинных знакогенераторов
static uint_fast16_t
ubfont_decode(const unifont_t * font, char cc)
{
	const UB_Font * const ub = (const UB_Font *) font->fontraster;
	return (unsigned char) cc;
}

// Для моноширинных знакогенераторов
static const void * ubfont_getcharraster(const unifont_t * font, char cc)
{
	const UB_Font * const ub = (const UB_Font *) font->fontraster;
	const uint_fast16_t ci = font->decode(font, cc);
	const uint16_t * const table = ub->table; // Таблица с данными
	return table + ci * ub->height;
}

// Для моноширинных знакогенераторов
static uint_fast16_t
ubfont_put_char_small(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	const UB_Font * const ub = (const UB_Font *) font->fontraster;
	const uint16_t * const table = ub->table; // Таблица с данными
	const uint16_t * const charraster = (const uint16_t *) font->getcharraster(font, cc);
	const uint_fast16_t width2 = font->font_charwidth(font, cc);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_charheight(font, cc);	// number of rows
	const uint_fast16_t bytesw = font->bytesw;	// bytes in each chargen row
	return unifont_put_char16(db, xpix, ypix, font, charraster, width2, height2, bytesw, fg);
}

// Для моноширинных знакогенераторов
const unifont_t unifont_gothic_11x13 =
{
	.decode = ubfont_decode,
	.getcharraster = ubfont_getcharraster,
	.font_charwidth = ubfont_width,
	.font_charheight = ubfont_height,
	.font_draw = ubfont_put_char_small,
	.font_prerender = NULL,
	.fontraster = (const void *) & gothic_11x13,
	.label = "unifont_gothic_11x13"
};

// Для пропорциональных знакогенераторов
static uint_fast8_t ubpfont_width(const unifont_t * font, char cc)
{
	const UB_pFont * const ubp = (const UB_pFont *) font->fontraster;
	const uint16_t * const table = ubp->table; // Таблица с данными
	const uint_fast8_t width = table [0];	// первый элемент содержит ширину
	return width;
}

// Для пропорциональных знакогенераторов
static uint_fast8_t ubpfont_height(const unifont_t * font, char cc)
{
	const UB_pFont * const ubp = (const UB_pFont *) font->fontraster;
	(void) cc;
	return ubp->height;
}

// Возвращает индекс символа в знакогенераторе
// Для пропорциональных знакогенераторов
static uint_fast16_t
ubpfont_decode(const unifont_t * font, char cc)
{
	const UB_pFont * const ubp = (const UB_pFont *) font->fontraster;
	const uint_fast8_t c = (unsigned char) cc;
	if (c < ubp->first_char)
		return 0;
	if (c > ubp->last_char)
		return 0;
	return c - ubp->first_char;
}

// Для пропорциональных знакогенераторов
static const void * ubpfont_getcharraster(const unifont_t * font, char cc)
{
	const UB_pFont * const ubp = (const UB_pFont *) font->fontraster;
	const uint_fast16_t ci = font->decode(font, cc);
	const uint16_t * const table = ubp->table; // Таблица с данными
	return table + ci * (ubp->height + 1);
}

// Для пропорциональных знакогенераторов
static uint_fast16_t
ubpfont_put_char_small(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	const UB_pFont * const ubp = (const UB_pFont *) font->fontraster;
	const uint16_t * const table = ubp->table; // Таблица с данными
	const uint16_t * const charraster = (const uint16_t *) font->getcharraster(font, cc);
	const uint_fast16_t width2 = font->font_charwidth(font, cc);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_charheight(font, cc);	// number of rows
	const uint_fast16_t bytesw = font->bytesw;	// bytes in each chargen row
	return unifont_put_char16(db, xpix, ypix, font, charraster, width2, height2, bytesw, fg);
}

// Для пропорциональных знакогенераторов
const unifont_t unifont_gothic_12x16p =
{
	.decode = ubpfont_decode,
	.getcharraster = ubpfont_getcharraster,
	.font_charwidth = ubpfont_width,
	.font_charheight = ubpfont_height,
	.font_draw = ubpfont_put_char_small,
	.font_prerender = NULL,
	.fontraster = (const void *) & gothic_12x16_p,
	.label = "unifont_gothic_12x16p"
};

#endif /* WITHALTERNATIVEFONTS */

#endif /* LCDMODE_LTDC */
