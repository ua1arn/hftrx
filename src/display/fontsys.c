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
static const uint8_t * unifont_getcharraster(const unifont_t * font, char cc)
{
	const uint_fast16_t ci = font->decode(font, cc);
	const uint8_t * const charraster = font->fontraster + ci * font->font_charheight(font, cc) * font->bytesw;
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
	const uint8_t * const charraster = font->getcharraster(font, cc);
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
	const uint8_t * const charraster = font->getcharraster(font, cc);
	const uint_fast16_t width2 = font->font_charwidth(font, cc);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_charheight(font, cc);	// number of rows
	const uint_fast16_t bytesw = font->bytesw;	// bytes in each chargen row
	return unifont_put_char_x2(db, xpix, ypix, font, charraster, width2, height2, bytesw, fg);
}

static uint_fast16_t
unifont_put_char_bighalf(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	const uint_fast16_t ci = font->decode(font, cc);
	const uint8_t * const charraster = font->fontraster + ci * font->font_charheight(font, cc) * font->bytesw;
	const uint_fast16_t width2 = font->font_charwidth(font, cc);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_charheight(font, cc);	// number of rows
	const uint_fast16_t bytesw = font->bytesw;	// bytes in each chargen row
	return unifont_put_char(db, xpix, ypix, font, charraster, width2, height2, bytesw, fg);
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
	const unifont_t * const font = & unifont_small;
	return font->font_draw(db, xpix, ypix, font, cc, fg);
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
	const unifont_t * const font = & unifont_small2;
	return font->font_draw(db, xpix, ypix, font, cc, fg);
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
	return SMALLCHARW2 * strlen(s);
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
	const unifont_t * const font = & unifont_small3;
	return font->font_draw(db, xpix, ypix, font, cc, fg);
}

// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
static uint_fast16_t new_colorpip_put_char_small3_tbg(
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
	const uint_fast8_t width = font->font_charwidth(font, cc);
	const uint_fast8_t height = font->font_charheight(font, cc);
	const uint_fast8_t ci = font->decode(font, cc);
	const uint8_t * const charraster = & S1D13781_smallfont3_LTDC [ci] [0];
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < height; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(db, xpix, ypix + cgrow);
		ltdc_horizontal_pixels_tbg(tgr, charraster + cgrow, width, fg);
	}
	return xpix + width;
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
	return SMALLCHARW3 * strlen(s);
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



#if defined (SMALLCHARW) && defined (SMALLCHARH)
// Возвращает ширину строки в пикселях
uint_fast16_t strwidth(
	const char * s
	)
{
	ASSERT(s != NULL);
	return SMALLCHARW * strlen(s);
}

#endif /* defined (SMALLCHARW) && defined (SMALLCHARH) */

// обычный шрифт
uint_fast16_t display_put_char_small(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, char cc, const gxstyle_t * dbstyle)
{
	if (dbstyle->fontsmall == NULL)
		return x;
	savewhere = __func__;
	//PRINTF("display_put_char_small: '%c'\n", cc);
	return dbstyle->fontsmall->font_draw(db, x, y, dbstyle->fontsmall, cc, dbstyle->textcolor);
}

// большой шрифт
uint_fast16_t display_put_char_big(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, char cc, const gxstyle_t * dbstyle)
{
	const unifont_t * const font = & unifont_big;
	if (font == NULL)
		return x;
	savewhere = __func__;
	const uint_fast16_t ci = font->decode(font, cc);
	// todo: use pre-rendered chars
	//PRINTF("display_put_char_big: '%c' ci=%u\n", cc, font->decode(font, cc));
	return font->font_draw(db, x, y, font, cc, dbstyle->textcolor);
}

uint_fast16_t display_put_char_half(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, char cc, const gxstyle_t * dbstyle)
{
	const unifont_t * const font = & unifont_half;
	if (font == NULL)
		return x;
	savewhere = __func__;
	// todo: use pre-rendered chars
	//PRINTF("display_put_char_half: '%c' ci=%u\n", cc, font->decode(font, cc));
	return font->font_draw(db, x, y, font, cc, dbstyle->textcolor);
}

// обычный шрифт
uint_fast16_t display_wrdata_begin(uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp)
{
	* yp = GRID2Y(ycell);
	return GRID2X(xcell);
}

#if WITHPRERENDER
/* использование предварительно построенных изображений при отображении частоты */

enum { RENDERCHARS = 14 }; /* valid chars: "0123456789 #._" */

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
			font->font_draw(& dbvbig, xpix, ypix, font, ' ', ci, BIGCHARW, fg);
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
			font->font_draw(& dbvhalf, xpix, ypix, font, ' ', ci, HALFCHARW, fg);
			display_do_AA(& dbvhalf, xpix, ypix, HALFCHARW, HALFCHARH);
			xpix += HALFCHARW;
		}
		dcache_clean((uintptr_t) rendered_half, sizeof rendered_half [0] * GXSIZE(HALFCHARW * RENDERCHARS, HALFCHARH));
	}
}

uint_fast16_t render_char_big(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, const unifont_t * font, char cc)
{
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
    const uint_fast8_t width = font->font_charwidth(font, cc);
    const uint_fast8_t ci = font->decode(font, cc);
	savewhere = __func__;

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
			width, BIGCHARH, // размер окна источника
			BITBLT_FLAG_NONE, COLORPIP_KEY
			);

	return xpix + width;
}

uint_fast16_t render_char_half(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, const unifont_t * font, char cc)
{
	const uint_fast8_t width = font->font_charwidth(font, cc);
	const uint_fast8_t ci = font->decode(font, cc);
	savewhere = __func__;

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
			width, HALFCHARH, // размер окна источника
			BITBLT_FLAG_NONE, COLORPIP_KEY
			);

	return xpix + width;
}

#endif /* WITHPRERENDER */


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
	.font_draw = unifont_put_char_bighalf,
	.font_prerender = unifont_put_char_bighalf,
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
	.font_draw = unifont_put_char_bighalf,
	.font_prerender = unifont_put_char_bighalf,
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
	.font_prerender = unifont_put_char_small,
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
	.font_prerender = unifont_put_char_small_x2,
	.label = "unifont_small"
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
	.font_prerender = unifont_put_char_small,
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
	.bytesw = sizeof S1D13781_smallfont3_LTDC [0],		// байтов в одной строке знакогенератора символа
	.fontraster = S1D13781_smallfont3_LTDC [0],		// начало знакогенератора в памяти
	.font_draw = unifont_put_char_small,
	.font_prerender = unifont_put_char_small,
	.label = "unifont_small3"
};
#endif /* SMALLCHARH3 */

#endif /* LCDMODE_LTDC */
