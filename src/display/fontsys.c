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

static uint_fast8_t bigfont_height(const unifont_t * font)
{
	(void) font;
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

static uint_fast8_t halffont_height(const unifont_t * font)
{
	(void) font;
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

static uint_fast8_t smallfont_height(const unifont_t * font)
{
	(void) font;
	return SMALLCHARH;
}
static uint_fast8_t smallfont_x2_width(const unifont_t * font, char cc)
{
	(void) font;
	(void) cc;
	return SMALLCHARW * 2;	// полная ширина символа в пикселях
}

static uint_fast8_t smallfont_x2_height(const unifont_t * font)
{
	(void) font;
	return SMALLCHARH * 2;
}

#endif /* defined (SMALLCHARW) */

// Тривиальная функция получения начала растра символа в массиве шрифта - вск символы с одинаковыми размерами
// Для моноширинных знакогенераторов
static const void * unifont_getcharraster(const unifont_t * font, char cc)
{
	const uint_fast16_t ci = font->decode(font, cc);
	const uint8_t * const charraster = (const uint8_t *) font->fontraster + ci * font->font_drawheight(font) * font->bytesw;
	return charraster;
}

// Тривиальная функция получения начала растра символа в массиве шрифта - вск символы с одинаковыми размерами
// Для моноширинных знакогенераторов
static const void * unifont_getcharraster_x2(const unifont_t * font, char cc)
{
	const uint_fast16_t ci = font->decode(font, cc);
	const uint8_t * const charraster = (const uint8_t *) font->fontraster + ci * font->font_drawheight(font) / 2 * font->bytesw;
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
// Самый старший (левый) из указанного количества бит выдвигается первым.
// 16/32 bit raster support
static void ubpfont_pixels(
	PACKEDCOLORPIP_T * __restrict tgr,		// target raster
	uint_fast32_t v,
	uint_fast16_t width,	// number of bits (start from LSB first byte in raster)
	uint_fast32_t mask,	// начальный бит маски (сдвигаем вправо)
	COLORPIP_T fg
	)
{
	if (width != 0)
	{
		do
		{
			if (v & mask) * tgr = fg;
			tgr += 1;
			mask >>= 1;
		} while (-- width);
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

	for (; w >= 16; w -= 16, tgr += 16)
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
		} while (w -= 2);
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
// Одна строка целиком в элементе массива charraster
static uint_fast16_t
ubxfont_put_char16(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	const uint16_t * const charraster,
	uint_fast16_t mask,	// начальный бит маски (сдвигаем вправо)
	uint_fast16_t width2,
	uint_fast16_t height2,
	uint_fast16_t bytesw_unused,
	COLORPIP_T fg
	)
{
	(void) bytesw_unused;
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < height2; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(db, xpix, ypix + cgrow);
		ubpfont_pixels(tgr, charraster [cgrow], width2, mask, fg);
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
	uint_fast16_t width2,	// размер выходного растра
	uint_fast16_t height2,	// размер выходного растра
	uint_fast16_t bytesw,
	COLORPIP_T fg
	)
{
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < height2 / 2; cgrow += 1)
	{
		PACKEDCOLORPIP_T * const tgr0 = colpip_mem_at(db, xpix, ypix + cgrow * 2 + 0);
		ltdc_horizontal_x2_pixels_tbg(tgr0, & charraster [cgrow * bytesw], width2, fg);
		PACKEDCOLORPIP_T * const tgr1 = colpip_mem_at(db, xpix, ypix + cgrow * 2 + 1);
		ltdc_horizontal_x2_pixels_tbg(tgr1, & charraster [cgrow * bytesw], width2, fg);
	}
	return xpix + width2;
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
	const uint_fast16_t width2 = font->font_drawwidth(font, cc);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_drawheight(font);	// number of rows
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
	const uint_fast16_t width2 = font->font_drawwidth(font, cc);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_drawheight(font);	// number of rows
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
	const uint_fast16_t width2 = font->font_drawwidth(font, cc);	// number of bits (start from LSB first byte in raster)
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
	const uint_fast16_t width2 = font->font_drawwidth(font, cc);	// number of bits (start from LSB first byte in raster)
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
	const uint8_t * const charraster = (const uint8_t *) font->fontraster + ci * font->font_drawheight(font) * font->bytesw;
	const uint_fast16_t width2 = font->font_drawwidth(font, cc);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_drawheight(font);	// number of rows
	const uint_fast16_t bytesw = font->bytesw;	// bytes in each chargen row
	return unifont_put_char(db, xpix, ypix, font, charraster, width2, height2, bytesw, fg);
}

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
	return font->font_draw(db, xpix, ypix, font, cc, fg);
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
		w += font->font_drawwidth(font, c);
	return w;
}

#if defined (SMALLCHARH2) && defined (SMALLCHARW2)

static uint_fast8_t smallfont2_width(const unifont_t * font, char cc)
{
	(void) cc;
	return SMALLCHARW2;	// полная ширина символа в пикселях
}

static uint_fast8_t smallfont2_height(const unifont_t * font)
{
	return SMALLCHARH2;	// полная ширина символа в пикселях
}

#endif /* defined (SMALLCHARH2) && defined (SMALLCHARW2) */



#if defined (SMALLCHARH3) && defined (SMALLCHARW3)

static uint_fast8_t smallfont3_width(const unifont_t * font, char cc)
{
	(void) cc;
	return SMALLCHARW3;	// полная ширина символа в пикселях
}

static uint_fast8_t smallfont3_height(const unifont_t * font)
{
	return SMALLCHARH3;	// полная ширина символа в пикселях
}

#endif /* defined (SMALLCHARH3) && defined (SMALLCHARW3)) */

#if defined (SMALLCHARH) && defined (SMALLCHARW)

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
uint_fast16_t
colpip_string_small(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	)
{
	return colpip_string(db, x, y, & unifont_small, s, fg);
}


#endif /* defined (SMALLCHARH) && defined (SMALLCHARW) */

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
	.font_drawwidth = bigfont_width,
	.font_drawheight = bigfont_height,
#if WITHALTERNATIVEFONTS
	.bytesw = sizeof ltdc_CenturyGothic_big [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = ltdc_CenturyGothic_big,		// начало знакогенератора в памяти
#else /* WITHALTERNATIVEFONTS */
	.bytesw = sizeof S1D13781_bigfont_LTDC [0] [0],		// байтов в одной строке знакогенератора символа
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
	.bytesw = sizeof ltdc_CenturyGothic_half [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = ltdc_CenturyGothic_half,		// начало знакогенератора в памяти
#else /* WITHALTERNATIVEFONTS */
	.bytesw = sizeof S1D13781_halffont_LTDC [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = S1D13781_halffont_LTDC,		// начало знакогенератора в памяти
#endif /* WITHALTERNATIVEFONTS */
	.font_draw = unifont_put_char_half_rendered,
	.font_prerender = unifont_put_char_bighalf_prerender,
	.label = "unifont_half"
};


#if defined (SMALLCHARH) && defined (SMALLCHARW)

const unifont_t unifont_small =
{
	.decode = smallfont_decode,
	.getcharraster = unifont_getcharraster,
	.font_drawwidth = smallfont_width,
	.font_drawheight = smallfont_height,
	.bytesw = sizeof S1D13781_smallfont_LTDC [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = S1D13781_smallfont_LTDC,		// начало знакогенератора в памяти
	.font_draw = unifont_put_char_small,
	.font_prerender = NULL,
	.label = "unifont_small"
};

const unifont_t unifont_small_x2 =
{
	.decode = smallfont_decode,
	.getcharraster = unifont_getcharraster_x2,
	.font_drawwidth = smallfont_x2_width,
	.font_drawheight = smallfont_x2_height,
	.bytesw = sizeof S1D13781_smallfont_LTDC [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = S1D13781_smallfont_LTDC,		// начало знакогенератора в памяти
	.font_draw = unifont_put_char_small_x2,
	.font_prerender = NULL,
	.label = "unifont_small_x2"
};
#endif /* defined (SMALLCHARH) && defined (SMALLCHARW) */

#if defined (SMALLCHARH2) && defined (SMALLCHARW2)

const unifont_t unifont_small2 =
{
	.decode = smallfont_decode,
	.getcharraster = unifont_getcharraster,
	.font_drawwidth = smallfont2_width,
	.font_drawheight = smallfont2_height,
	.bytesw = sizeof S1D13781_smallfont2_LTDC [0] [0],		// байтов в одной строке знакогенератора символа
	.fontraster = S1D13781_smallfont2_LTDC,		// начало знакогенератора в памяти
	.font_draw = unifont_put_char_small,
	.font_prerender = NULL,
	.label = "unifont_small2"
};
#endif /* defined (SMALLCHARH2) && defined (SMALLCHARW2) */

#if defined (SMALLCHARH3) && defined (SMALLCHARW3)

const unifont_t unifont_small3 =
{
	.decode = smallfont_decode,
	.getcharraster = unifont_getcharraster,
	.font_drawwidth = smallfont3_width,
	.font_drawheight = smallfont3_height,
	.bytesw = sizeof S1D13781_smallfont3_LTDC [0][0],		// байтов в одной строке знакогенератора символа
	.fontraster = S1D13781_smallfont3_LTDC,		// начало знакогенератора в памяти
	.font_draw = unifont_put_char_small,
	.font_prerender = NULL,
	.label = "unifont_small3"
};
#endif /* defined (SMALLCHARH3) && defined (SMALLCHARW3) */


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

// ***************************************


// Для моноширинных знакогенераторов
uint_fast8_t ubmfont_width(const unifont_t * font, char cc)
{
	const UB_Font * const ub = (const UB_Font *) font->fontraster;
	(void) cc;
	return ub->width;	// полная ширина символа в пикселях
}

// Для моноширинных знакогенераторов
uint_fast8_t ubmfont_height(const unifont_t * font)
{
	const UB_Font * const ub = (const UB_Font *) font->fontraster;
	return ub->height;
}

// Возвращает индекс символа в знакогенераторе
// Для моноширинных знакогенераторов
uint_fast16_t
ubmfont_decode(const unifont_t * font, char cc)
{
	//const UB_Font * const ub = (const UB_Font *) font->fontraster;
	const unsigned char c = (unsigned char) cc;
	if (c < 0x20)
		return 0;
	return c - 0x20;
}

// Для моноширинных знакогенераторов
const void * ubmfont_getcharraster(const unifont_t * font, char cc)
{
	const UB_Font * const ub = (const UB_Font *) font->fontraster;
	const uint_fast16_t ci = font->decode(font, cc);
	const uint16_t * const table = ub->table; // Таблица с данными
	return table + ci * ub->height;
}

// Для моноширинных знакогенераторов
uint_fast16_t
ubmfont_render_char16(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	//const UB_Font * const ub = (const UB_Font *) font->fontraster;
	const uint16_t * const charraster = (const uint16_t *) font->getcharraster(font, cc);
	const uint_fast16_t width2 = font->font_drawwidth(font, cc);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_drawheight(font);	// number of rows
	//const uint_fast16_t bytesw = font->bytesw;	// bytes in each chargen row (unused)
//	PRINTF("ubmfont_render_char16: cc=%02X(%c),width2=%u\n", (unsigned char) cc, cc, width2);
//	printhex16(0, charraster, height2 * 2);
	return ubxfont_put_char16(db, xpix, ypix, font, charraster, UINT16_C(1) << 15, width2, height2, 0 /* (unused) */, fg);
}

// Для пропорциональных знакогенераторов
uint_fast8_t ubpfont_width(const unifont_t * font, char cc)
{
	const UB_pFont * const ubp = (const UB_pFont *) font->fontraster;
	const uint16_t * const table = ubp->table; // Таблица с данными
	const uint_fast16_t ci = font->decode(font, cc);
	const uint_fast8_t width = table [ci * (ubp->height + 1)];	// первый элемент содержит ширину
	return width;
}

// Для пропорциональных знакогенераторов
uint_fast8_t ubpfont_height(const unifont_t * font)
{
	const UB_pFont * const ubp = (const UB_pFont *) font->fontraster;
	return ubp->height;
}

// Возвращает индекс символа в знакогенераторе
// Для пропорциональных знакогенераторов
uint_fast16_t
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
const void * ubpfont_getcharraster(const unifont_t * font, char cc)
{
	const UB_pFont * const ubp = (const UB_pFont *) font->fontraster;
	const uint_fast16_t ci = font->decode(font, cc);
	const uint16_t * const table = ubp->table; // Таблица с данными
	return table + ci * (ubp->height + 1) + 1;
}

// Для пропорциональных знакогенераторов
uint_fast16_t
ubpfont_render_char16(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	const uint16_t * const charraster = (const uint16_t *) font->getcharraster(font, cc);
	const uint_fast16_t width2 = font->font_drawwidth(font, cc);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_drawheight(font);	// number of rows
	return ubxfont_put_char16(db, xpix, ypix, font, charraster, UINT16_C(1) << (15 - width2), width2, height2, 0 /* (unused) */, fg);
}

// *********************************************************************************************************************

#if 0
// CP Font Generator support

#include "Tahoma_Regular_88x77.h"
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

// http://www.apetech.de/fontCreator
// GLCD library?

// **********************************

static uint_fast16_t
aptechfont_decode(const unifont_t * font, char cc)
{
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	const uint_fast16_t   font_Size_in_Bytes_over_all_included_Size_it_self;
	const uint_fast8_t    font_Width_in_Pixel_for_fixed_drawing = blob [2];
	const uint_fast8_t    font_Height_in_Pixel_for_all_characters = blob [3];
	const uint_fast8_t    font_First_Char = blob [4];
	const uint_fast8_t    font_Char_Count = blob [5];
	const uint_fast8_t c = (unsigned char) cc;
	if (c < font_First_Char)
		return 0;
	if (c >= (font_First_Char + font_Char_Count))
		return 0;
	return c - font_First_Char;
}

// Для пропорциональных знакогенераторов
static uint_fast8_t aptechfont_width(const unifont_t * font, char cc)
{
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	const uint_fast16_t   font_Size_in_Bytes_over_all_included_Size_it_self = USBD_peek_u16(blob + 0); // size of zero indicates fixed width font, actual length is width * height
	const uint_fast16_t ci = font->decode(font, cc);
	const uint_fast8_t    font_Width_in_Pixel_for_fixed_drawing = blob [2];
	const uint_fast8_t    font_Height_in_Pixel_for_all_characters = blob [3];
	const uint_fast8_t    font_First_Char = blob [4];
	const uint_fast8_t    font_Char_Count = blob [5];

	if (font_Size_in_Bytes_over_all_included_Size_it_self == 0)
	{
		return font_Width_in_Pixel_for_fixed_drawing;
	}
	// Proportional widths
	const uint_fast8_t w = blob [6 + ci];
	return w == 0 ? font_Width_in_Pixel_for_fixed_drawing : w;
}

static const void * aptechfont_getcharraster(const unifont_t * font, char cc)
{
	const uint_fast16_t ci = font->decode(font, cc);
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	const uint_fast16_t   font_Size_in_Bytes_over_all_included_Size_it_self = USBD_peek_u16(blob + 0); // size of zero indicates fixed width font, actual length is width * height
	const uint_fast8_t    font_First_Char = blob [4];
	const uint_fast8_t    font_Char_Count = blob [5];
	const uint_fast8_t    font_Height_in_Pixel_for_all_characters = blob [3];
	const uint_fast8_t bytespervertical = (font_Height_in_Pixel_for_all_characters + 7) / 8;
	if (font_Size_in_Bytes_over_all_included_Size_it_self == 0)
	{
		// fixed chars
	    // Fixed width; char width table not used !!!!
		const uint_fast8_t    font_Width_in_Pixel_for_fixed_drawing = blob [2];
		unsigned charbytes = bytespervertical * font_Width_in_Pixel_for_fixed_drawing;
		//PRINTF("aptechfont_getcharraster: cc=%02X, ci=%u, charbytes=%u\n", cc, ci, charbytes);
		return blob + 6 + ci * charbytes;
	}
	uint_fast16_t i;
	uint_fast16_t dataoffs = 6 + font_Char_Count;	// Начало таблицы растров - сразу за таблицей ширины символов
	//printhex(0xdead, blob + dataoffs, 10);
	const uint_fast16_t		widthsoffset = 6;
//	PRINTF("width table:\n");
//	printhex(0, blob + widthsoffset, font_Char_Count);
	if (blob [widthsoffset + ci] == 0)
		return NULL;	// Для этого символа растра нет
	for (i = 0; i < ci; ++ i)
	{
		const unsigned w = blob [widthsoffset + i];
		//PRINTF("i=%u, ci=%u, widthsoffset=%u, dataoffs=%u, w=%u, bytespervertical=%u\n", i, ci, widthsoffset, dataoffs, w, bytespervertical);
		dataoffs += w * bytespervertical;
	}
	return blob + dataoffs;
}

// Для пропорциональных знакогенераторов
static uint_fast8_t aptechfont_height(const unifont_t * font)
{
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	const uint_fast16_t   font_Size_in_Bytes_over_all_included_Size_it_self = USBD_peek_u16(blob + 0); // size of zero indicates fixed width font, actual length is width * height
	const uint_fast8_t    font_Width_in_Pixel_for_fixed_drawing = blob [2];
	const uint_fast8_t    font_Height_in_Pixel_for_all_characters = blob [3];

	return font_Height_in_Pixel_for_all_characters;
}

static uint_fast16_t
aptechfont_render_char(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	const uint8_t * const charraster = (const uint8_t * const) font->getcharraster(font, cc);
	const uint_fast16_t width2 = font->font_drawwidth(font, cc);
	const uint_fast16_t height2 = font->font_drawheight(font);

	// Пиксели идут вертикальной полосой слеыв направо
	// Младший бит - верхний
	// заполняется полосой 8 бит, далее перходит на следующую полосу

	if (charraster != NULL)
	{
		const unsigned bytespervertical = (height2 + 7) / 8;
	//	PRINTF("aptechfont_render_char: cc=%02X (%c), width2=%u, height2=%u, bytespervertical=%u\n", (unsigned char) cc, cc, width2, height2, bytespervertical);
	//	printhex(0, charraster, bytespervertical * width2);
		uint_fast16_t row;
		for (row = 0; row < height2; ++ row)
		{
			uint_fast16_t col;
			for (col = 0; col < width2; ++ col)
			{
				const unsigned strype = row / 8;
				const unsigned byteoffset = strype * width2 + col;
				const unsigned byteshift = row % 8;

				//ASSERT(byteshift == row);
				if (charraster [byteoffset] & (1u << byteshift))
				{
					* colpip_mem_at(db, xpix + col, ypix + row) = fg;
					//PRINTF("*");
				}
				else
				{
					//PRINTF("-");
				}
			}
			//PRINTF("\n");
		}
	}
	return xpix + width2;
}

#endif /* LCDMODE_LTDC */

// **********************************

#if LCDMODE_LTDC

#include "fonts_x/roboto32.h"
//#include "fonts_x/SystemFont5x7.h"
//#include "fonts_x/fixednums15x31.h"

const unifont_t unifont_roboto32 =
{
	.decode = aptechfont_decode,
	.getcharraster = aptechfont_getcharraster,
	.font_drawwidth = aptechfont_width,
	.font_drawheight = aptechfont_height,
	.font_draw = aptechfont_render_char,
	//
	.fontraster = roboto32,
	.label = "Tahoma_Regular_88x77"
};

#include "fonts_x/helvNeueTh70.h"
//#include "fonts_x/SystemFont5x7.h"
//#include "fonts_x/fixednums15x31.h"

const unifont_t unifont_helvNeueTh70 =
{
	.decode = aptechfont_decode,
	.getcharraster = aptechfont_getcharraster,
	.font_drawwidth = aptechfont_width,
	.font_drawheight = aptechfont_height,
	.font_draw = aptechfont_render_char,
	//
	.fontraster = helvNeueTh70,
	.label = "helvNeueTh70"
};

#endif /* LCDMODE_LTDC */
