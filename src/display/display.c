/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Доработки для LS020 Василий Линывый, livas60@mail.ru
//

#include "hardware.h"
#include "board.h"
#include "display.h"
#include "formats.h"
#include "spi.h"	// hardware_spi_master_send_frame
#include "display2.h"
#include <string.h>


const char * savestring = "no data";
const char * savewhere = "no func";

#if LCDMODE_LTDC

#define FONTSHERE 1

#include "fontmaps.h"

/* рисование линии на основном экране произвольным цветом
*/
void
display_line(const gxdrawb_t * db,
	int x1, int y1,
	int x2, int y2,
	COLORPIP_T color
	)
{

	colpip_line(db, x1, y1, x2, y2, color, 0);
}

#if ! LCDMODE_LTDC_L24
#include "./byte2crun.h"
#endif /* ! LCDMODE_LTDC_L24 */

static PACKEDCOLORPIP_T ltdc_fg = COLORPIP_WHITE, ltdc_bg = COLORPIP_BLACK;

#if ! LCDMODE_LTDC_L24
static const FLASHMEM PACKEDCOLORPIP_T (* byte2runpip) [256][8] = & byte2runpip_COLORPIP_WHITE_COLORPIP_BLACK;
#endif /* ! LCDMODE_LTDC_L24 */

void colmain_setcolors(COLORPIP_T fg, COLORPIP_T bg)
{

#if ! LCDMODE_LTDC_L24
	ltdc_fg = fg;
	ltdc_bg = bg;
#else /* ! LCDMODE_LTDC_L24 */
	ltdc_fg.r = COLORPIP_R(fg);
	ltdc_fg.g = COLORPIP_G(fg);
	ltdc_fg.b = COLORPIP_B(fg);
	ltdc_bg.r = COLORPIP_R(bg);
	ltdc_bg.g = COLORPIP_G(bg);
	ltdc_bg.b = COLORPIP_B(bg);

#endif /* ! LCDMODE_LTDC_L24 */

#if ! LCDMODE_LTDC_L24

	COLORPIP_SELECTOR(byte2runpip);

#endif /* ! LCDMODE_LTDC_L24 */

	//COLORPIP_SELECTOR(byte2runpip);

}

void colmain_setcolors3(COLORPIP_T fg, COLORPIP_T bg, COLORPIP_T fgbg)
{
	colmain_setcolors(fg, bg);
}

/* индивидуальные функции драйвера дисплея - реализованы в соответствующем из файлов */
// Заполниить цветом фона
void display_clear(const gxdrawb_t * db)
{
	const COLORPIP_T bg = display2_getbgcolor();

	colpip_fillrect(db, 0, 0, DIM_X, DIM_Y, bg);
}

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// display_wrdatabar_begin() и display_wrdatabar_end().
void display_bar(
	const gxdrawb_t * db,
	uint_fast16_t x,
	uint_fast16_t y,
	uint_fast8_t width,	/* количество знакомест, занимаемых индикатором */
	uint_fast8_t value,		/* значение, которое надо отобразить */
	uint_fast8_t tracevalue,		/* значение маркера, которое надо отобразить */
	uint_fast8_t topvalue,	/* значение, соответствующее полностью заполненному индикатору */
	uint_fast8_t vpattern,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL */
	uint_fast8_t patternmax,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL - для отображения запомненного значения */
	uint_fast8_t emptyp			/* паттерн для заполнения между штрихами */
	)
{
	ASSERT(value <= topvalue);
	ASSERT(tracevalue <= topvalue);
	const uint_fast16_t wfull = GRID2X(width);
	const uint_fast16_t h = SMALLCHARH; //GRID2Y(1);
	const uint_fast16_t wpart = (uint_fast32_t) wfull * value / topvalue;
	const uint_fast16_t wmark = (uint_fast32_t) wfull * tracevalue / topvalue;
	const uint_fast8_t hpattern = 0x33;

	colpip_fillrect(db, 	x, y, 			wpart, h, 			ltdc_fg);
	colpip_fillrect(db, 	x + wpart, y, 	wfull - wpart, h, 	ltdc_bg);
	if (wmark < wfull && wmark >= wpart)
		colpip_fillrect(db, x + wmark, y, 	1, h, 				ltdc_fg);
}

void gxdrawb_initialize(gxdrawb_t * db, PACKEDCOLORPIP_T * buffer, uint_fast16_t dx, uint_fast16_t dy)
{
	ASSERT(buffer);
	db->buffer = buffer;
	db->dx = dx;
	db->dy = dy;
	db->cachebase = (uintptr_t) buffer;
	db->cachesize = GXSIZE(dx, dy) * sizeof (PACKEDCOLORPIP_T);
	db->stride = GXADJ(dx) * sizeof (PACKEDCOLORPIP_T);
	db->layerv = NULL;
}

// большие и средние цифры (частота)
uint_fast16_t display_wrdatabig_begin(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp);
uint_fast16_t display_put_char_big(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, uint_fast8_t lowhalf);
uint_fast16_t display_put_char_half(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, uint_fast8_t lowhalf);
void display_wrdatabig_end(const gxdrawb_t * db);
// обычный шрифт
uint_fast16_t display_wrdata_begin(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp);
uint_fast16_t display_put_char_small(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc);
uint_fast16_t display_put_char_small_xy(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, COLOR565_T fg);
void display_wrdata_end(const gxdrawb_t * db);

// большие и средние цифры (частота)
uint_fast16_t render_wrdatabig_begin(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp);
uint_fast16_t render_char_big(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, uint_fast8_t lowhalf);
uint_fast16_t render_char_half(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, uint_fast8_t lowhalf);
void render_wrdatabig_end(const gxdrawb_t * db);

// самый маленький шрифт
uint_fast16_t display_wrdata2_begin(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast16_t * yp)
{
	//ltdc_secondoffs = 0;
	//ltdc_h = SMALLCHARH;

	* yp = GRID2Y(y);
	return GRID2X(x);
}

void display_wrdata2_end(const gxdrawb_t * db)
{
}


#if 1//! WITHLVGL


// Выдать один цветной пиксель (фон/символ)
static void
ltdc_pixel(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	uint_fast8_t v			// 0 - цвет background, иначе - foreground
	)
{
	PACKEDCOLORPIP_T * const tgr = colpip_mem_at(db, x, y);
	* tgr = v ? ltdc_fg : ltdc_bg;
}


// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
void RAMFUNC ltdc_horizontal_pixels(
	PACKEDCOLORPIP_T * tgr,		// target raster
	const FLASHMEM uint8_t * raster,
	uint_fast16_t width	// number of bits (start from LSB first byte in raster)
	)
{
	uint_fast16_t col;
	uint_fast16_t w = width;

	for (col = 0; w >= 8; col += 8, w -= 8)
	{
		const FLASHMEM PACKEDCOLORPIP_T * const pcl = (* byte2runpip) [* raster ++];
		memcpy(tgr + col, pcl, sizeof (* tgr) * 8);
	}
	if (w != 0)
	{
		const FLASHMEM PACKEDCOLORPIP_T * const pcl = (* byte2runpip) [* raster ++];
		memcpy(tgr + col, pcl, sizeof (* tgr) * w);
	}
	//dcache_clean((uintptr_t) tgr, sizeof (* tgr) * width);
}

#endif /* ! WITHLVGL */

// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
// Фон не трогаем
static void RAMFUNC ltdcmain_horizontal_pixels_tbg(
	PACKEDCOLORPIP_T * __restrict tgr,		// target raster
	const FLASHMEM uint8_t * __restrict raster,
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
			++ tgr;
			vlast >>= 1;
		} while (-- w);
	}
}

// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
// Фон не трогаем
// удвоенный по ширине растр
static void RAMFUNC ltdcmain_horizontal_x2_pixels_tbg(
	PACKEDCOLORPIP_T * __restrict tgr,		// target raster
	const FLASHMEM uint8_t * __restrict raster,
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

static void RAMFUNC
ltdc_put_char_unified(
	const FLASHMEM uint8_t * fontraster,
	uint_fast8_t width,		// пикселей в символе по горизонтали знакогнератора
	uint_fast8_t height,	// строк в символе по вертикали
	uint_fast8_t bytesw,	// байтов в одной строке символа
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	uint_fast8_t ci,	// индекс символа в знакогенераторе
	uint_fast8_t width2	// пикселей в символе по горизонтали отображается (для уменьшеных в ширину символов большиз шрифтов)
	)
{
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < height; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(db, xpix, ypix + cgrow);
		ltdc_horizontal_pixels(tgr, & fontraster [(ci * height + cgrow) * bytesw], width2);
	}
}

/* valid chars: "0123456789 #._" */
static uint_fast8_t
bigfont_decode(char cc)
{
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

/* valid chars: "0123456789 #._" */
static uint_fast8_t
halffont_decode(char cc)
{
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

static uint_fast8_t
smallfont_decode(char cc)
{
	const uint_fast8_t c = (unsigned char) cc;
	if (c < ' ' || c > 0x7F)
		return '$' - ' ';
	return c - ' ';
}

#if defined (BIGCHARW_NARROW) && defined (BIGCHARW)
uint_fast8_t bigfont_width(char cc)
{
	return (cc == '.' || cc == '#') ? BIGCHARW_NARROW  : BIGCHARW;	// полная ширина символа в пикселях
}
#endif /* defined (BIGCHARW_NARROW) && defined (BIGCHARW) */

#if defined (HALFCHARW)
uint_fast8_t halffont_width(char cc)
{
	(void) cc;
	return HALFCHARW;	// полная ширина символа в пикселях
}
#endif /* defined (HALFCHARW) */

#if defined (SMALLCHARW)
uint_fast8_t smallfont_width(char cc)
{
	(void) cc;
	return SMALLCHARW;	// полная ширина символа в пикселях
}
#endif /* defined (SMALLCHARW) */

#if defined (SMALLCHARH2)
uint_fast8_t smallfont2_width(char cc)
{
	(void) cc;
	return SMALLCHARW2;	// полная ширина символа в пикселях
}
#endif /* defined (SMALLCHARH2) */

#if defined (SMALLCHARH3)
uint_fast8_t smallfont3_width(char cc)
{
	(void) cc;
	return SMALLCHARW3;	// полная ширина символа в пикселях
}
#endif /* defined (SMALLCHARH3) */

// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// return new x
static uint_fast16_t
RAMFUNC
ltdc_put_char_small(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t ci)
{
	ASSERT(xpix < DIM_X);
	ASSERT(ypix < DIM_Y);
	ltdc_put_char_unified(
			S1D13781_smallfont_LTDC [0] [0], SMALLCHARW, SMALLCHARH, sizeof S1D13781_smallfont_LTDC [0] [0],  	// параметры растра со шрифтом
			db, xpix, ypix, ci, SMALLCHARW);
	return xpix + SMALLCHARW;
}

// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
// return new x coordinate
static uint_fast16_t RAMFUNC ltdc_put_char_big(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t ci, uint_fast8_t width2)
{
	ASSERT(xpix < DIM_X);
	ASSERT(ypix < DIM_Y);

#if WITHALTERNATIVEFONTS
	uint8_t const * const font_big = ltdc_CenturyGothic_big [0] [0];
	const size_t size_bigfont = sizeof ltdc_CenturyGothic_big [0] [0];
#else
	uint8_t const * const font_big = S1D13781_bigfont_LTDC [0] [0];
	const size_t size_bigfont = sizeof S1D13781_bigfont_LTDC [0] [0];
#endif /* WITHALTERNATIVEFONTS */

	ltdc_put_char_unified(font_big, BIGCHARW, BIGCHARH, size_bigfont,  	// параметры растра со шрифтом
			db, xpix, ypix, ci, width2);
 	return xpix + width2;
}

// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
// return new x coordinate
static uint_fast16_t RAMFUNC ltdc_put_char_half(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t ci, uint_fast8_t width2)
{
	ASSERT(xpix < DIM_X);
	ASSERT(ypix < DIM_Y);

#if WITHALTERNATIVEFONTS
	uint8_t const * const font_half = ltdc_CenturyGothic_half [0] [0];
	const size_t size_halffont = sizeof ltdc_CenturyGothic_half [0] [0];
#else
	uint8_t const * const font_half = S1D13781_halffont_LTDC [0] [0];
	const size_t size_halffont = sizeof S1D13781_halffont_LTDC [0] [0];
#endif /* WITHALTERNATIVEFONTS */

	ltdc_put_char_unified(
			font_half, HALFCHARW, HALFCHARH, size_halffont, 	// параметры растра со шрифтом
			db, xpix, ypix, ci, width2);
	return xpix + width2;
}

#if 0//SMALLCHARW
// return new x coordinate
static uint_fast16_t
RAMFUNC_NONILINE
ltdcmain_horizontal_put_char_small(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,
	uint_fast16_t y,
	char cc
	)
{
	const uint_fast8_t width = SMALLCHARW;
	const uint_fast8_t c = smallfont_decode(cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(db, x, y + cgrow);
		colorpip_pixels(tgr, S1D13781_smallfont_LTDC [c] [cgrow], width);
	}
	return x + width;
}
#endif /* SMALLCHARW */

#if defined (SMALLCHARW)
// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
static uint_fast16_t RAMFUNC_NONILINE colorpip_put_char_small_tbg(
	const gxdrawb_t * db,
	uint_fast16_t x,
	uint_fast16_t y,
	char cc,
	COLOR565_T fg
	)
{
	const uint_fast8_t width = SMALLCHARW;
	const uint_fast8_t c = smallfont_decode(cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(db, x, y + cgrow);
		ltdcmain_horizontal_pixels_tbg(tgr, S1D13781_smallfont_LTDC [c] [cgrow], width, fg);
	}
	return x + width;
}

// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
static uint_fast16_t RAMFUNC_NONILINE colorpip_x2_put_char_small_tbg(
	const gxdrawb_t * db,
	uint_fast16_t x,
	uint_fast16_t y,
	char cc,
	COLOR565_T fg
	)
{
	const uint_fast8_t width = SMALLCHARW;
	const uint_fast8_t c = smallfont_decode(cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr0 = colpip_mem_at(db, x, y + cgrow * 2 + 0);
		ltdcmain_horizontal_x2_pixels_tbg(tgr0, S1D13781_smallfont_LTDC [c] [cgrow], width, fg);
		PACKEDCOLORPIP_T * const tgr1 = colpip_mem_at(db, x, y + cgrow * 2 + 1);
		ltdcmain_horizontal_x2_pixels_tbg(tgr1, S1D13781_smallfont_LTDC [c] [cgrow], width, fg);
	}
	return x + width * 2;
}

uint_fast16_t display_put_char_small_xy(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, char c, COLOR565_T fg)
{
	return colorpip_put_char_small_tbg(db, x, y, c, fg);
}
#endif /* defined (SMALLCHARW) */

#if SMALLCHARW2
// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
static uint_fast16_t RAMFUNC_NONILINE colorpip_put_char_small2_tbg(
	const gxdrawb_t * db,
	uint_fast16_t x,
	uint_fast16_t y,
	char cc,
	COLORPIP_T fg
	)
{
	const uint_fast8_t width = SMALLCHARW2;
	const uint_fast8_t c = smallfont_decode(cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH2; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(db, x, y + cgrow);
		ltdcmain_horizontal_pixels_tbg(tgr, S1D13781_smallfont2_LTDC [c] [cgrow], width, fg);
	}
	return x + width;
}
#endif /* SMALLCHARW2 */

#if SMALLCHARW3
// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
static uint_fast16_t RAMFUNC_NONILINE colorpip_put_char_small3_tbg(
	const gxdrawb_t * db,
	uint_fast16_t x,
	uint_fast16_t y,
	char cc,
	COLORPIP_T fg
	)
{
	const uint_fast8_t width = SMALLCHARW3;
	const uint_fast8_t c = smallfont_decode(cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH3; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(db, x, y + cgrow);
		ltdcmain_horizontal_pixels_tbg(tgr, & S1D13781_smallfont3_LTDC [c] [cgrow], width, fg);
	}
	return x + width;
}
#endif /* SMALLCHARW3 */



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
		x = colorpip_put_char_small_tbg(db, x, y, c, fg);
	}
}
// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string_x2_tbg(
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
		x = colorpip_x2_put_char_small_tbg(db, x, y, c, fg);
	}
}

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_text(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T fg,		// цвет вывода текста
	const char * s,		// строка для вывода
	size_t len			// количество символов
	)
{
	ASSERT(s != NULL);
	while (len --)
	{
		const char c = * s ++;
		x = colorpip_put_char_small_tbg(db, x, y, c, fg);
	}
}
// Используется при выводе на графический индикатор,
void
colpip_text_x2(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T fg,		// цвет вывода текста
	const char * s,		// строка для вывода
	size_t len			// количество символов
	)
{
	ASSERT(s != NULL);
	while (len --)
	{
		const char c = * s ++;
		x = colorpip_x2_put_char_small_tbg(db, x, y, c, fg);
	}
}

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string_x2ra90_count(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T fg,		// цвет вывода текста
	COLORPIP_T bg,		// цвет вывода текста
	const char * s,		// строка для вывода
	size_t len			// количество символов
	)
{
	enum { TDX = SMALLCHARW * 2, TDY = SMALLCHARH * 2 };
	static RAMFRAMEBUFF ALIGNX_BEGIN PACKEDCOLORPIP_T scratch [GXSIZE(TDX, TDY)] ALIGNX_END;
	gxdrawb_t sdbv;
	gxdrawb_initialize(& sdbv, scratch, TDX, TDY);
	ASSERT(s != NULL);
	while (len --)
	{
		const char c = * s ++;
		colpip_fillrect(& sdbv, 0, 0, TDX, TDY, bg);
		colorpip_x2_put_char_small_tbg(& sdbv, 0, 0, c, fg);
		hwaccel_ra90(db, x, y, & sdbv);
		y += TDX;
	}
}
#endif /* defined (SMALLCHARW) */

#if defined (SMALLCHARW2)

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
		x = colorpip_put_char_small2_tbg(db, x, y, c, fg);
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

#if defined (SMALLCHARW3)
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

// Возвращает ширину строки в пикселях
uint_fast16_t strwidth3(
	const char * s
	)
{
	ASSERT(s != NULL);
	return SMALLCHARW3 * strlen(s);
}

#endif /* defined (SMALLCHARW3) */


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

// полоса индикатора
uint_fast16_t display_wrdatabar_begin(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp)
{
	* yp = GRID2Y(ycell);
	return GRID2X(xcell);
}

void display_wrdatabar_end(const gxdrawb_t * db)
{
}

// большие и средние цифры (частота)
uint_fast16_t display_wrdatabig_begin(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp)
{
	* yp = GRID2Y(ycell);
	return GRID2X(xcell);
}

// большой шрифт
uint_fast16_t display_put_char_big(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, char cc, uint_fast8_t lowhalf)
{
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
    const uint_fast8_t width = bigfont_width(cc);
    const uint_fast8_t ci = bigfont_decode(cc);
	savewhere = __func__;
	return ltdc_put_char_big(db, x, y, ci, width);
}

uint_fast16_t display_put_char_half(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, char cc, uint_fast8_t lowhalf)
{
	const uint_fast8_t width = halffont_width(cc);
	const uint_fast8_t ci = halffont_decode(cc);
	savewhere = __func__;
	return ltdc_put_char_half(db, x, y, ci, width);
}

uint_fast16_t display_put_char_small(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc)
{
	const uint_fast8_t ci = smallfont_decode(cc);

	return ltdc_put_char_small(db, xpix, ypix, ci);
}

void display_wrdata_end(const gxdrawb_t * db)
{
}

// большой шрифт
void display_wrdatabig_end(const gxdrawb_t * db)
{
}

// обычный шрифт
uint_fast16_t display_wrdata_begin(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp)
{
	* yp = GRID2Y(ycell);
	return GRID2X(xcell);
}

#if SMALLCHARH3

static uint_fast16_t
RAMFUNC_NONILINE ltdc_horizontal_put_char_small3(
	const gxdrawb_t * db,
	uint_fast16_t x, uint_fast16_t y,
	char cc
	)
{
	const uint_fast8_t ci = smallfont_decode(cc);
	ltdc_put_char_unified(
			S1D13781_smallfont3_LTDC [0], SMALLCHARW3, SMALLCHARH3, sizeof S1D13781_smallfont3_LTDC [0],  	// параметры растра со шрифтом
			db, x, y, ci, SMALLCHARW3);
	return x + SMALLCHARW3;
}

static void
display_string3(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, const char * s, uint_fast8_t lowhalf)
{
	char c;
//	ltdc_secondoffs = 0;
//	ltdc_h = SMALLCHARH3;
	while ((c = * s ++) != '\0')
		x = ltdc_horizontal_put_char_small3(db, x, y, c);
}

void
colpip_string3_at_xy(
	const gxdrawb_t * db,
	uint_fast16_t x,
	uint_fast16_t y,
	const char * __restrict s
	)
{
	char c;
//	ltdc_secondoffs = 0;
//	ltdc_h = SMALLCHARH3;
	while ((c = * s ++) != '\0')
		x = ltdc_horizontal_put_char_small3(db, x, y, c);
}

void
display_string3_at_xy(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, const char * __restrict s, COLORPIP_T fg, COLORPIP_T bg)
{
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	colmain_setcolors(fg, bg);
	do
	{
		display_string3(db, x, y + lowhalf, s, lowhalf);
	} while (lowhalf --);
}

#endif /* SMALLCHARH3 */

#if WITHPRERENDER
/* использование предварительно построенных изображений при отображении частоты */

enum { RENDERCHARS = 14 }; /* valid chars: "0123456789 #._" */

static const unsigned picx_big = BIGCHARW * RENDERCHARS;
static const unsigned picy_big = BIGCHARH;

static const unsigned picx_half = HALFCHARW * RENDERCHARS;
static const unsigned picy_half = HALFCHARH;

/* Изображения символов располагаются в буфере горизонтально, слева направо */
static PACKEDCOLORPIP_T rendered_big [GXSIZE(BIGCHARW * RENDERCHARS, BIGCHARH)];
static PACKEDCOLORPIP_T rendered_half [GXSIZE(HALFCHARW * RENDERCHARS, HALFCHARH)];
static gxdrawb_t dbvbig;
static gxdrawb_t dbvhalf;

// Подготовка отображения больщих символов
/* valid chars: "0123456789 #._" */
void render_value_big_initialize(void)
{
	COLORPIP_T keycolor = COLORPIP_KEY;
	unsigned picalpha = 255;
	gxdrawb_initialize(& dbvbig, rendered_big, picx_big, picy_big);
	gxdrawb_initialize(& dbvhalf, rendered_half, picx_half, picy_half);

	colpip_fillrect(& dbvbig, 0, 0, picx_big, picy_big, TFTALPHA(picalpha, COLORPIP_RED));	/* при alpha==0 все биты цвета становятся 0 */
	colpip_fillrect(& dbvhalf, 0, 0, picx_half, picy_half, TFTALPHA(picalpha, COLORPIP_YELLOW));	/* при alpha==0 все биты цвета становятся 0 */

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
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdatabig_begin(& dbvbig, 0, 0, & ypix);
		for (ci = 0; ci < RENDERCHARS; ++ ci)
		{
			/* формирование изображений символов, возможно с эффектами антиалиасинга */
			/* Изображения символов располагаются в буфере горизонтально, слева направо */
			ASSERT(xpix == ci * BIGCHARW);
			ltdc_put_char_big(& dbvbig, xpix, ypix, ci, BIGCHARW);
			display_do_AA(& dbvbig, xpix, ypix, BIGCHARW, BIGCHARH);
			xpix += BIGCHARW;
		}
		display_wrdatabig_end(& dbvbig);
		dcache_clean((uintptr_t) rendered_big, sizeof rendered_big [0] * GXSIZE(BIGCHARW * RENDERCHARS, BIGCHARH));
	}
	/* half-size characters */
	{
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdatabig_begin(& dbvhalf, 0, 0, & ypix);
		for (ci = 0; ci < RENDERCHARS; ++ ci)
		{
			/* формирование изображений символов, возможно с эффектами антиалиасинга */
			/* Изображения символов располагаются в буфере горизонтально, слева направо */
			ASSERT(xpix == ci * HALFCHARW);
			ltdc_put_char_half(& dbvhalf, xpix, ypix, ci, HALFCHARW);
			display_do_AA(& dbvhalf, xpix, ypix, HALFCHARW, HALFCHARH);
			xpix += HALFCHARW;
		}
		display_wrdatabig_end(& dbvhalf);
		dcache_clean((uintptr_t) rendered_half, sizeof rendered_half [0] * GXSIZE(HALFCHARW * RENDERCHARS, HALFCHARH));
	}
}

// большой шрифт
uint_fast16_t render_wrdatabig_begin(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp)
{
	* yp = GRID2Y(ycell);
	return GRID2X(xcell);
}

// большой шрифт
void render_wrdatabig_end(const gxdrawb_t * db)
{
}

uint_fast16_t render_char_big(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, uint_fast8_t lowhalf)
{
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
    const uint_fast8_t width = bigfont_width(cc);
    const uint_fast8_t ci = bigfont_decode(cc);
	savewhere = __func__;

	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	/* копируем изображение БЕЗ цветового ключа */
	/* dcache_clean исходного изображения уже выполнено при построении изображения. */
	colpip_bitblt(
			(uintptr_t) db->buffer, GXSIZE(db->dx, db->dy) * sizeof buffer [0],
			db,
			xpix, ypix,	// координаты в окне получатля
			(uintptr_t) rendered_big, 0 * GXSIZE(picx_big, picy_big) * sizeof rendered_big [0],
			& dbvbig,
			ci * BIGCHARW, 0,	// координаты окна источника
			width, BIGCHARH, // размер окна источника
			BITBLT_FLAG_NONE, COLORPIP_KEY
			);

	return xpix + width;
}

uint_fast16_t render_char_half(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, uint_fast8_t lowhalf)
{
    const uint_fast8_t width = halffont_width(cc);
	const uint_fast8_t ci = halffont_decode(cc);
	savewhere = __func__;

	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	/* копируем изображение БЕЗ цветового ключа */
	/* dcache_clean исходного изображения уже выполнено при построении изображения. */
	colpip_bitblt(
			(uintptr_t) db->buffer, GXSIZE(db->dx, db->dy) * sizeof (PACKEDCOLORPIP_T),
			db,
			xpix, ypix,	// координаты в окне получатля
			(uintptr_t) rendered_half, 0 * GXSIZE(picx_half, picy_half) * sizeof rendered_half [0],
			& dbvhalf,
			ci * HALFCHARW, 0,	// координаты окна источника
			width, HALFCHARH, // размер окна источника
			BITBLT_FLAG_NONE, COLORPIP_KEY
			);

	return xpix + width;
}

#endif /* WITHPRERENDER */

/* аппаратный сброс дисплея - перед инициализаций */
/* вызывается при разрешённых прерываниях. */
void
display_reset(void)
{
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(1); // Delay 1ms
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	local_delay_ms(10); // Delay 10ms
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(50); // Delay 50 ms
}

/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
#if WITHLVGL
	display_lvgl_initialize();
#endif /* WITHLVGL */
}

/* Разряжаем конденсаторы питания */
void display_uninitialize(void)
{
}


#if WITHLVGL //&& ! LINUX_SUBSYSTEM

#include "lvgl.h"
//#include "../demos/lv_demos.h"
//#include "../demos/vector_graphic/lv_demo_vector_graphic.h"
#include "src/lvgl_gui/styles.h"

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/

#if defined (RTMIXIDLCD)

static void maindisplay_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
	if (lv_display_is_double_buffered(disp) && lv_display_flush_is_last(disp))
	{
    	dcache_clean(
    		(uintptr_t) px_map,
    		lv_color_format_get_size(lv_display_get_color_format(disp)) * GXSIZE(lv_display_get_horizontal_resolution(disp), lv_display_get_vertical_resolution(disp))
    		);
    	hardware_ltdc_main_set(RTMIXIDLCD, (uintptr_t) px_map);	/* set visible buffer start. Wait VSYNC. */
#if WITHHDMITVHW && defined (RTMIXIDTV) && 1
    	// Дубль жкрана
    	hardware_ltdc_main_set(RTMIXIDTV, (uintptr_t) px_map);	/* set visible buffer start. Wait VSYNC. */
#endif
#if WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC
    	{
    		gxdrawb_t dbv;
    		//gxdrawb_initlvgl(& dbv, lv_display_get_layer_top(disp));
    		gxdrawb_initialize(& dbv, (PACKEDCOLORPIP_T *) px_map, lv_display_get_horizontal_resolution(disp), lv_display_get_vertical_resolution(disp));
    		display_snapshot(& dbv);	/* запись видимого изображения */
    	}
#endif /* WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC */
    }
	/*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_display_flush_ready(disp);
}

#endif /* defined (RTMIXIDLCD) */

#if defined (RTMIXIDTV)

static void subdisplay_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
	if (lv_display_is_double_buffered(disp) && lv_display_flush_is_last(disp))
	{
    	dcache_clean(
    		(uintptr_t) px_map,
    		lv_color_format_get_size(lv_display_get_color_format(disp)) * GXSIZE(lv_display_get_horizontal_resolution(disp), lv_display_get_vertical_resolution(disp))
    		);
    	hardware_ltdc_main_set(RTMIXIDTV, (uintptr_t) px_map);	/* set visible buffer start. Wait VSYNC. */
    }
	/*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_display_flush_ready(disp);
}

#endif /* defined (RTMIXIDTV) */

static uint32_t myhardgeticks(void)
{
	return sys_now();
}

struct driverdata
{
	int rtmixid;
};

// Инициадизация нужного количества дисплеев
void display_lvgl_initialize(void)
{
	lv_init();	// LVGL library initialize

#if defined (RTMIXIDLCD)
	if (1)
	{
		static struct driverdata maindisplay =
		{
				.rtmixid = RTMIXIDLCD
		};
		// main display
	    lv_display_t * disp = lv_display_create(DIM_X, DIM_Y);
	    lv_display_set_driver_data(disp, & maindisplay);
	    lv_display_set_flush_cb(disp, maindisplay_flush);

	    static LV_ATTRIBUTE_MEM_ALIGN RAMFRAMEBUFF uint8_t fb1 [GXSIZE(DIM_X, DIM_Y) * LCDMODE_PIXELSIZE];
	    static LV_ATTRIBUTE_MEM_ALIGN RAMFRAMEBUFF uint8_t fb2 [GXSIZE(DIM_X, DIM_Y) * LCDMODE_PIXELSIZE];

	    //LV_DRAW_BUF_DEFINE_STATIC(dbuf_3_3, DIM_X, DIM_Y, LV_COLOR_FORMAT_ARGB8888);

	    lv_display_set_buffers_with_stride(
	    		disp,
				fb1, fb2, sizeof(fb1),
	    		GXADJ(DIM_X) * LCDMODE_PIXELSIZE,
				LV_DISPLAY_RENDER_MODE_DIRECT);
	    //lv_display_set_3rd_draw_buffer(disp, & dbuf_3_3);
	    lv_display_set_color_format(disp, (lv_color_format_t) display_get_lvformat());
	    lv_display_set_antialiasing(disp, false);

	    lv_obj_t * const obj = lv_label_create(lv_display_get_screen_active(disp));
	    lv_label_set_text_static(obj, "LCD/LVDS display");

	    lv_display_set_default(disp);
	}
#endif /* defined (RTMIXIDTV) */

#if WITHHDMITVHW && defined (RTMIXIDTV) && 0
	if (1)
	{
		static struct driverdata subdisplay =
		{
				.rtmixid = RTMIXIDTV
		};
		// tv display
	    lv_display_t * disp = lv_display_create(DIM_X, DIM_Y);
	    lv_display_set_driver_data(disp, & subdisplay);
	    lv_display_set_flush_cb(disp, subdisplay_flush);

	    static LV_ATTRIBUTE_MEM_ALIGN RAMFRAMEBUFF uint8_t fb1 [GXSIZE(TVD_WIDTH, TVD_HEIGHT) * TVMODE_PIXELSIZE];
	    static LV_ATTRIBUTE_MEM_ALIGN RAMFRAMEBUFF uint8_t fb2 [GXSIZE(TVD_WIDTH, TVD_HEIGHT) * TVMODE_PIXELSIZE];

	    //LV_DRAW_BUF_DEFINE_STATIC(dbuf_3_3, DIM_X, DIM_Y, LV_COLOR_FORMAT_ARGB8888);

	    lv_display_set_buffers_with_stride(
	    		disp,
				fb1, fb2, sizeof(fb1),
    		GXADJ(DIM_X) * TVMODE_PIXELSIZE,
				LV_DISPLAY_RENDER_MODE_DIRECT);
	    //lv_display_set_3rd_draw_buffer(disp, & dbuf_3_3);
	    lv_display_set_color_format(disp, (lv_color_format_t) display_get_lvformat());
	    lv_display_set_antialiasing(disp, false);

	    lv_obj_t * const obj = lv_label_create(lv_display_get_screen_active(disp));
	    lv_label_set_text_static(obj, "HDMI display");

	    //lv_display_set_default(disp);
	}
#endif /* WITHHDMITVHW && defined (RTMIXIDTV) */

	// Add custom draw unit
	lvglhw_initialize();

	// lvgl будет получать тики
	lv_tick_set_cb(myhardgeticks);
}

void gxdrawb_initlvgl(gxdrawb_t * db, void * layerv)
{
	lv_layer_t * layer = (lv_layer_t *) layerv;
	gxdrawb_initialize(db,
			(PACKEDCOLORPIP_T *) lv_draw_buf_goto_xy(layer->draw_buf, 0, 0),
			layer->draw_buf->header.w, layer->draw_buf->header.h);
	db->layerv = layerv;
	db->cachebase = (uintptr_t) layer->draw_buf->data;
	db->cachesize = layer->draw_buf->data_size;
	db->stride = layer->draw_buf->header.stride;
}


#endif /* WITHLVGL //&& ! LINUX_SUBSYSTEM */


// Используется при выводе на графический индикатор,
void
display_text(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, const char * s, uint_fast8_t xspan)
{
	char c;
	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdata_begin(db, xcell, ycell, & ypix);

	savestring = s;
	savewhere = __func__;

#if WITHLVGL
	lv_layer_t * const layer = (lv_layer_t *) db->layerv;
	if (layer)
	{
		const size_t len = strlen(s);
		unsigned w = xspan < len ? len : xspan;
		w = xspan;
		//PRINTF("x/y=%d/%d '%s'\n", xpix, ypix, s);
		lv_draw_rect_dsc_t d;
	    lv_draw_label_dsc_t l;
		lv_area_t coords;
	    lv_draw_label_dsc_init(& l);
		lv_draw_rect_dsc_init(& d);
		lv_area_set(& coords, xpix, ypix, xpix + GRID2X(CHARS2GRID(w)) - 1, ypix + DISPLAY_AT_H - 1);
	    d.bg_color = display_lvlcolor(ltdc_bg);
	    l.color = display_lvlcolor(ltdc_fg);
	    l.align = LV_TEXT_ALIGN_LEFT;
	    l.text = s;
	    l.font = DISPLAY_AT_FONT;
	    //PRINTF("display_string: x/y=%d/%d '%s'\n", (int) xpix, (int) xpix, s);
		lv_draw_rect(layer, & d, & coords);
        lv_draw_label(layer, & l, & coords);

    	display_wrdata_end(db);
        return;
	}
#endif

	size_t len = strlen(s);
	for (len = strlen(s); len < xspan; ++ len)
		xpix = display_put_char_small(db, xpix, ypix, ' ');
	while((c = * s ++) != '\0' && xspan --)
		xpix = display_put_char_small(db, xpix, ypix, c);

	display_wrdata_end(db);
}

#if LCDMODE_S1D13781

	// младший бит левее
	static const uint_fast16_t mapcolumn [16] =
	{
		0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, // биты для манипуляций с видеобуфером
		0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000,
	};

#elif LCDMODE_UC1608 || LCDMODE_UC1601

	/* старшие биты соответствуют верхним пикселям изображения */
	// млдший бит ниже в растре
	static const uint_fast8_t mapcolumn [8] =
	{
		0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, // биты для манипуляций с видеобуфером
	};
#else /* LCDMODE_UC1608 || LCDMODE_UC1601 */

	/* младшие биты соответствуют верхним пикселям изображения */
	// млдший бит выше в растре
	static const uint_fast8_t mapcolumn [8] =
	{
		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, // биты для манипуляций с видеобуфером
	};

#endif /* LCDMODE_UC1608 || LCDMODE_UC1601 */

static const FLASHMEM int32_t vals10 [] =
{
	1000000000U,
	100000000U,
	10000000U,
	1000000U,
	100000U,
	10000U,
	1000U,
	100U,
	10U,
	1U,
};


// Отображение цифр в поле "больших цифр" - индикатор основной частоты настройки аппарата.
void
NOINLINEAT
pix_display_value_big(const gxdrawb_t * db,
	uint_fast16_t xpix,	// x координата начала вывода значения
	uint_fast16_t ypix,	// y координата начала вывода значения
	uint_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t comma2,	// = comma + 3;		// comma position (from right, inside width)
	uint_fast8_t rj,	// = 1;		// right truncated
	uint_fast8_t blinkpos,		// позиция, где символ заменён пробелом
	uint_fast8_t blinkstate,	// 0 - пробел, 1 - курсор
	uint_fast8_t withhalf,		// 0 - только большие цифры
	uint_fast8_t lowhalf		// lower half
	)
{
	//	if (width > ARRAY_SIZE(vals10))
	//		width = ARRAY_SIZE(vals10);
		//const uint_fast8_t comma2 = comma + 3;		// comma position (from right, inside width)
	const uint_fast8_t j = ARRAY_SIZE(vals10) - rj;
	uint_fast8_t i = (j - width);
	uint_fast8_t z = blinkpos == 255 ? 1 : 0;	// only zeroes
	uint_fast8_t half = 0;	// отображаем после второй запатой - маленьким шрифтом
	for (; i < j; ++ i)
	{
		const ldiv_t res = ldiv(freq, vals10 [i]);
		const uint_fast8_t g = (j - i);		// десятичная степень текущего разряда на отображении

		// разделитель десятков мегагерц
		if (comma2 == g)
		{
			xpix = display_put_char_big(db, xpix, ypix, (z == 0) ? '.' : '#', lowhalf);	// '#' - узкий пробел. Точка всегда узкая
		}
		else if (comma == g)
		{
			z = 0;
			half = withhalf;
			xpix = display_put_char_big(db, xpix, ypix, '.', lowhalf);
		}

		if (blinkpos == g)
		{
			const uint_fast8_t bc = blinkstate ? '_' : ' ';
			// эта позиция редактирования частоты. Справа от неё включаем все нули
			z = 0;
			if (half)
				xpix = display_put_char_half(db, xpix, ypix, bc, lowhalf);
			else
				xpix = display_put_char_big(db, xpix, ypix, bc, lowhalf);
		}
		else if (z == 1 && (i + 1) < j && res.quot == 0)
			xpix = display_put_char_big(db, xpix, ypix, ' ', lowhalf);	// supress zero
		else
		{
			z = 0;
			if (half)
				xpix = display_put_char_half(db, xpix, ypix, '0' + res.quot, lowhalf);
			else
				xpix = display_put_char_big(db, xpix, ypix, '0' + res.quot, lowhalf);
		}
		freq = res.rem;
	}
}

// Отображение цифр в поле "больших цифр" - индикатор основной частоты настройки аппарата.
void
NOINLINEAT
display_value_big(const gxdrawb_t * db,
	uint_fast8_t xcell,	// x координата начала вывода значения
	uint_fast8_t ycell,	// y координата начала вывода значения
	uint_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t comma2,	// = comma + 3;		// comma position (from right, inside width)
	uint_fast8_t rj,	// = 1;		// right truncated
	uint_fast8_t blinkpos,		// позиция, где символ заменён пробелом
	uint_fast8_t blinkstate,	// 0 - пробел, 1 - курсор
	uint_fast8_t withhalf,		// 0 - только большие цифры
	uint_fast8_t lowhalf		// lower half
	)
{

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdatabig_begin(db, xcell, ycell, & ypix);
	pix_display_value_big(db, xpix, ypix, freq, width, comma, comma2, rj, blinkpos, blinkstate, withhalf, lowhalf);
	display_wrdatabig_end(db);
}

#if WITHPRERENDER


// Отображение цифр в поле "больших цифр" - индикатор основной частоты настройки аппарата.
/* использование предварительно построенных изображений при отображении частоты */
void
NOINLINEAT
pix_render_value_big(const gxdrawb_t * db,
	uint_fast16_t xpix,	// x координата начала вывода значения
	uint_fast16_t ypix,	// y координата начала вывода значения
	uint_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t comma2,	// = comma + 3;		// comma position (from right, inside width)
	uint_fast8_t rj,	// = 1;		// right truncated
	uint_fast8_t blinkpos,		// позиция, где символ заменён пробелом
	uint_fast8_t blinkstate,	// 0 - пробел, 1 - курсор
	uint_fast8_t withhalf,		// 0 - только большие цифры
	uint_fast8_t lowhalf		// lower half
	)
{
	//	if (width > ARRAY_SIZE(vals10))
	//		width = ARRAY_SIZE(vals10);
	//const uint_fast8_t comma2 = comma + 3;		// comma position (from right, inside width)
	const uint_fast8_t j = ARRAY_SIZE(vals10) - rj;
	uint_fast8_t i = (j - width);
	uint_fast8_t z = blinkpos == 255 ? 1 : 0;	// only zeroes
	uint_fast8_t half = 0;	// отображаем после второй запатой - маленьким шрифтом
	for (; i < j; ++ i)
	{
		const ldiv_t res = ldiv(freq, vals10 [i]);
		const uint_fast8_t g = (j - i);		// десятичная степень текущего разряда на отображении

		// разделитель десятков мегагерц
		if (comma2 == g)
		{
			xpix = render_char_big(db, xpix, ypix, (z == 0) ? '.' : '#', lowhalf);	// '#' - узкий пробел. Точка всегда узкая
		}
		else if (comma == g)
		{
			z = 0;
			half = withhalf;
			xpix = render_char_big(db, xpix, ypix, '.', lowhalf);
		}

		if (blinkpos == g)
		{
			const uint_fast8_t bc = blinkstate ? '_' : ' ';
			// эта позиция редактирования частоты. Справа от неё включаем все нули
			z = 0;
			if (half)
				xpix = render_char_half(db, xpix, ypix, bc, lowhalf);
			else
				xpix = render_char_big(db, xpix, ypix, bc, lowhalf);
		}
		else if (z == 1 && (i + 1) < j && res.quot == 0)
			xpix = render_char_big(db, xpix, ypix, ' ', lowhalf);	// supress zero
		else
		{
			z = 0;
			if (half)
				xpix = render_char_half(db, xpix, ypix, '0' + res.quot, lowhalf);

			else
				xpix = render_char_big(db, xpix, ypix, '0' + res.quot, lowhalf);
		}
		freq = res.rem;
	}
}

// Отображение цифр в поле "больших цифр" - индикатор основной частоты настройки аппарата.
/* использование предварительно построенных изображений при отображении частоты */
void
NOINLINEAT
render_value_big(const gxdrawb_t * db,
	uint_fast8_t xcell,	// x координата начала вывода значения
	uint_fast8_t ycell,	// y координата начала вывода значения
	uint_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t comma2,	// = comma + 3;		// comma position (from right, inside width)
	uint_fast8_t rj,	// = 1;		// right truncated
	uint_fast8_t blinkpos,		// позиция, где символ заменён пробелом
	uint_fast8_t blinkstate,	// 0 - пробел, 1 - курсор
	uint_fast8_t withhalf,		// 0 - только большие цифры
	uint_fast8_t lowhalf		// lower half
	)
{

	uint_fast16_t ypix;
	uint_fast16_t xpix = render_wrdatabig_begin(db, xcell, ycell, & ypix);
	pix_render_value_big(db, xpix, ypix, freq, width, comma, comma2, rj, blinkpos, blinkstate, withhalf, lowhalf);
	render_wrdatabig_end(db);
}

#endif /* WITHPRERENDER */

void
NOINLINEAT
display_value_lower(const gxdrawb_t * db,
	uint_fast8_t xcell,	// x координата начала вывода значения
	uint_fast8_t ycell,	// y координата начала вывода значения
	uint_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t rj	// = 1;		// right truncated
	)
{
//	if (width > ARRAY_SIZE(vals10))
//		width = ARRAY_SIZE(vals10);
	const uint_fast8_t j = ARRAY_SIZE(vals10) - rj;
	uint_fast8_t i = (j - width);
	uint_fast8_t z = 1;	// only zeroes
	uint_fast8_t half = 0;	// отображаем после второй запатой - маленьким шрифтом

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdatabig_begin(db, xcell, ycell, & ypix);
	for (; i < j; ++ i)
	{
		const ldiv_t res = ldiv(freq, vals10 [i]);
		const uint_fast8_t g = (j - i);		// десятичная степень текущего разряда на отображении

		if (comma == g || comma + 3 == g)
		{
			z = 0;
			xpix = display_put_char_big(db, xpix, ypix, '.', 0);
		}

		if (z == 1 && (i + 1) < j && res.quot == 0)
			xpix = display_put_char_big(db, xpix, ypix, ' ', 0);	// supress zero
		else
		{
			z = 0;
			xpix = display_put_char_half(db, xpix, ypix, '0' + res.quot, 0);
		}
		freq = res.rem;
	}
	display_wrdatabig_end(db);
}

void
NOINLINEAT
display_value_small(const gxdrawb_t * db,
	uint_fast8_t x,	// x координата начала вывода значения
	uint_fast8_t y,	// y координата начала вывода значения
	int_fast32_t freq,
	uint_fast8_t width,	// full width (if >= 128 - display with sign)
	uint_fast8_t comma,		// comma position (from right, inside width)
	uint_fast8_t comma2,
	uint_fast8_t rj,		// right truncated
	uint_fast8_t lowhalf
	)
{
//	if (width > ARRAY_SIZE(vals10))
//		width = ARRAY_SIZE(vals10);
	const uint_fast8_t wsign = (width & WSIGNFLAG) != 0;
	const uint_fast8_t wminus = (width & WMINUSFLAG) != 0;
	const uint_fast8_t j = ARRAY_SIZE(vals10) - rj;
	uint_fast8_t i = j - (width & WWIDTHFLAG);	// Номер цифры по порядку
	uint_fast8_t z = 1;	// если в позиции встретился '0' - не отоображать

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdata_begin(db, x, y, & ypix);
	if (wsign || wminus)
	{
		// отображение со знаком.
		z = 0;
		if (freq < 0)
		{
			xpix = display_put_char_small(db, xpix, ypix, '-');
			freq = - freq;
		}
		else if (wsign)
			xpix = display_put_char_small(db, xpix, ypix, '+');
		else
			xpix = display_put_char_small(db, xpix, ypix, ' ');
	}
	for (; i < j; ++ i)
	{
		const ldiv_t res = ldiv(freq, vals10 [i]);
		const uint_fast8_t g = (j - i);
		// разделитель десятков мегагерц
		if (comma2 == g)
		{
			xpix = display_put_char_small(db, xpix, ypix, (z == 0) ? '.' : ' ');
		}
		else if (comma == g)
		{
			z = 0;
			xpix = display_put_char_small(db, xpix, ypix, '.');
		}

		if (z == 1 && (i + 1) < j && res.quot == 0)
			xpix = display_put_char_small(db, xpix, ypix, ' ');	// supress zero
		else
		{
			z = 0;
			xpix = display_put_char_small(db, xpix, ypix, '0' + res.quot);
		}
		freq = res.rem;
	}
	display_wrdata_end(db);
}

#endif /* LCDMODE_LTDC */

#if WITHLTDCHW || 1

#if LCDMODE_LQ043T3DX02K
	// Sony PSP-1000 display panel
	// LQ043T3DX02K panel (272*480)
	// RK043FN48H-CT672B  panel (272*480) - плата STM32F746G-DISCO
	/**
	  * @brief  RK043FN48H Size
	  */
static const videomode_t vdmode0 =
{
	.width = 480,				/* LCD PIXEL WIDTH            */
	.height = 272,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  RK043FN48H Timing
	  */
	.hsync = 41,				/* Horizontal synchronization */
	.hbp = 2,				/* Horizontal back porch      */
	.hfp = 2,				/* Horizontal front porch     */

	.vsync = 10,				/* Vertical synchronization   */
	.vbp = 2,					/* Vertical back porch        */
	.vfp = 4,					/* Vertical front porch       */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 1,		/* требуется формирование сигнала RESET для панели по этому выводу после начала формирования синхронизации */
	//.ltdc_dotclk = 9000000uL,	// частота пикселей при работе с интерфейсом RGB
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};
	/* SONY PSP-1000 display (4.3") required. */
	/* Используется при BOARD_DEMODE = 0 */
	//#define BOARD_DERESET 1		/* требуется формирование сигнала RESET для панели по этому выводу после начала формирования синхронизации */

#elif LCDMODE_AT070TN90

/* AT070TN90 panel (800*480) - 7" display HV mode */
static const videomode_t vdmode0 =
{
	.width = 800,			/* LCD PIXEL WIDTH            */
	.height = 480,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	.hsync = 40,				/* Horizontal synchronization 1..40 */
	.hbp = 6,				/* Horizontal back porch      */
	.hfp = 210,				/* Horizontal front porch  16..354   */

	.vsync = 20,				/* Vertical synchronization 1..20  */
	.vbp = 3,				/* Vertical back porch      */
	.vfp = 22,				/* Vertical front porch  7..147     */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 30000000uL,	// частота пикселей при работе с интерфейсом RGB
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

#elif 1 && LCDMODE_AT070TNA2

/* AT070TNA2 panel (1024*600) - 7" display HV mode */
// HX8282-A01.pdf, page 38
static const videomode_t vdmode0 =
{
	.width = 1024,			/* LCD PIXEL WIDTH            */
	.height = 600,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	/* from r6dan: thb+thpw=160 is fixed */
	/* EK79001.PDF: */
	.hsync = 20,			/* Horizontal synchronization 1..140 */
	.hbp = 140,				/* Horizontal back porch */
	.hfp = 160,				/* Horizontal front porch  16..216  (r6dan: 140-160-180)  */

	/* from r6dan: tvb+tvpw=23 is fixed */
	.vsync = 3,				/* Vertical synchronization 1..20 */
	.vbp = 20,				/* Vertical back porch */
	.vfp = 12,				/* Vertical front porch  1..127  (r6dan: 2-12-22) */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 51200000uL,	// частота пикселей при работе с интерфейсом RGB 40.8..67.2
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

/* AT070TN90 panel (800*480) - 7" display HV mode */
static const videomode_t vdmode_800x480 =
{
	.width = 800,			/* LCD PIXEL WIDTH            */
	.height = 480,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	.hsync = 40,				/* Horizontal synchronization 1..40 */
	.hbp = 6,				/* Horizontal back porch      */
	.hfp = 210,				/* Horizontal front porch  16..354   */

	.vsync = 20,				/* Vertical synchronization 1..20  */
	.vbp = 3,				/* Vertical back porch      */
	.vfp = 22,				/* Vertical front porch  7..147     */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 30000000uL,	// частота пикселей при работе с интерфейсом RGB
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

#elif LCDMODE_LQ123K3LG01

/* LQ123K3LG01 panel (1280*480) - 12.3" display LVDS mode */
static const videomode_t vdmode0 =
{
	.width = 1280,			/* LCD PIXEL WIDTH            */
	.height = 480,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  LQ123K3LG01 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	.hsync = 6,			/* Horizontal synchronization 1..40 */
	.hbp = 39,				/* Horizontal back porch      */
	.hfp = 368,				/* Horizontal front porch  16..354   */

	.vsync = 1,				/* Vertical synchronization 1..20  */
	.vbp = 24,			/* Vertical back porch      */
	.vfp = 15,				/* Vertical front porch  7..147     */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	///.ltdc_dotclk = 54835000uL,	// частота пикселей при работе с интерфейсом RGB
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

#elif LCDMODE_TCG104XGLPAPNN

/* TCG104XGLPAPNN-AN30 panel (1024*768) - 10.4" display */
// TCG104XGLPAPNN-AN30-1384899.pdf
// horizontal period 1114 / 1344 / 1400
// vertical period 778 / 806 / 845
// Synchronization method should be DE mode
static const videomode_t vdmode0 =
{
	.width = 1024,			/* LCD PIXEL WIDTH            */
	.height = 768,			/* LCD PIXEL HEIGHT           */

	.hsync = 120,			/* Horizontal synchronization 1..140 */
	.hbp = 100,				/* Horizontal back porch  xxx   */
	.hfp = 100,				/* Horizontal front porch  16..216   */

	.vsync = 20,				/* Vertical synchronization 1..20  */
	.vbp = 9,				/* Vertical back porch  xxx   */
	.vfp = 9,				/* Vertical front porch  1..127     */

	// Synchronization method should be DE mode
	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 51200000uL,	// частота пикселей при работе с интерфейсом RGB 40.8..67.2
	.fps = 60,	/* frames per second 50 60 70 */
	.ntsc = 0,
	.interlaced = 0
};

#elif LCDMODE_ILI8961
	// HHT270C-8961-6A6 (320*240)
static const videomode_t vdmode0 =
{
	.width = 320 * 3,				/* LCD PIXEL WIDTH            */
	.height = 240,			/* LCD PIXEL HEIGHT           */

	/**
	  * @brief  RK043FN48H Timing
	  */
	.hsync = 1,				/* Horizontal synchronization */
	.hbp = 2,				/* Horizontal back porch      */
	.hfp = 2,				/* Horizontal front porch     */

	.vsync = 1,				/* Vertical synchronization   */
	.vbp = 2,					/* Vertical back porch        */
	.vfp = 2,					/* Vertical front porch       */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 24000000u,	// частота пикселей при работе с интерфейсом RGB
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

#elif LCDMODE_ILI9341
	// SF-TC240T-9370-T (320*240)
static static const const videomode_t vdmode0 =
{

	.width = 240,				/* LCD PIXEL WIDTH            */
	.height = 320,			/* LCD PIXEL HEIGHT           */

	/**
	  * @brief  ILI9341 Timing
	  */
	.hsync = 10,				/* Horizontal synchronization */
	.hbp = 20,				/* Horizontal back porch      */
	.hfp = 10,				/* Horizontal front porch     */

	.vsync = 2,				/* Vertical synchronization   */
	.vbp = 2,					/* Vertical back porch        */
	.vfp = 4,					/* Vertical front porch       */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 3000000uL,	// частота пикселей при работе с интерфейсом RGB
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

#elif LCDMODE_H497TLB01P4
	/* 720xRGBx1280 - 5" AMOELD Panel H497TLB01.4 */
	// See also:
	// https://github.com/bbelos/rk3188-kernel/blob/master/drivers/video/rockchip/transmitter/tc358768.c
	// https://github.com/tanish2k09/venom_kernel_aio_otfp/blob/master/drivers/input/touchscreen/mediatek/S3202/synaptics_dsx_i2c.c
	// https://stash.phytec.com/projects/TIRTOS/repos/vps-phytec/raw/src/boards/src/bsp_boardPriv.h?at=e8b92520f41e6523301d120dae15db975ad6d0da
	//https://code.ihub.org.cn/projects/825/repositories/874/file_edit_page?file_name=am57xx-idk-common.dtsi&path=arch%2Farm%2Fboot%2Fdts%2Fam57xx-idk-common.dtsi&rev=master
static const videomode_t vdmode0 =
{
	.width = 720,			/* LCD PIXEL WIDTH            */
	.height = 1280,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	.hsync = 5,				/* Horizontal synchronization 1..40 */
	.hbp = 11,				/* Horizontal back porch      */
	.hfp = 16,				/* Horizontal front porch  16..354   */

	.vsync = 5,				/* Vertical synchronization 1..20  */
	.vbp = 11,					/* Vertical back porch        */
	.vfp = 16,				/* Vertical front porch  7..147     */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 3000000uL	// частота пикселей при работе с интерфейсом RGB
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

#elif LCDMODE_TV101WXM
	/* 720xRGBx1280 - 5" AMOELD Panel H497TLB01.4 */
	// See also:
	// https://github.com/bbelos/rk3188-kernel/blob/master/drivers/video/rockchip/transmitter/tc358768.c
	// https://github.com/tanish2k09/venom_kernel_aio_otfp/blob/master/drivers/input/touchscreen/mediatek/S3202/synaptics_dsx_i2c.c
	// https://stash.phytec.com/projects/TIRTOS/repos/vps-phytec/raw/src/boards/src/bsp_boardPriv.h?at=e8b92520f41e6523301d120dae15db975ad6d0da
	//https://code.ihub.org.cn/projects/825/repositories/874/file_edit_page?file_name=am57xx-idk-common.dtsi&path=arch%2Farm%2Fboot%2Fdts%2Fam57xx-idk-common.dtsi&rev=master
static const videomode_t vdmode0 =
{
	.width = 800,			/* LCD PIXEL WIDTH            */
	.height = 1280,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	.hsync = 5,				/* Horizontal synchronization 1..40 */
	.hbp = 11,				/* Horizontal back porch      */
	.hfp = 16,				/* Horizontal front porch  16..354   */

	.vsync = 5,				/* Vertical synchronization 1..20  */
	.vbp = 11,					/* Vertical back porch        */
	.vfp = 16,				/* Vertical front porch  7..147     */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 3000000uL	// частота пикселей при работе с интерфейсом RGB
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

#elif LCDMODE_HSD100IF3
/* 1024 x 600 display LVDS mode */
static const videomode_t vdmode0 =
{
	.width = 1024,      /* LCD PIXEL WIDTH            */
	.height = 600,      /* LCD PIXEL HEIGHT           */
	/**
	* @brief  LQ123K1LG03 Timing
	* MODE=0 (DE)
	* When selected DE mode, VSYNC & HSYNC must pulled HIGH
	* MODE=1 (SYNC)
	* When selected sync mode, de must be grounded.
	*/
	.hsync = 6,//40,      /* Horizontal synchronization 1..40 */
	.hbp = 39,//6,        /* Horizontal back porch      */
	.hfp = 368,///210,        /* Horizontal front porch  16..354   */

	.vsync = 1,//20,        /* Vertical synchronization 1..20  */
	.vbp = 24,//73,        /* Vertical back porch      */
	.vfp = 15,///22,        /* Vertical front porch  7..147     */


	/* Accumulated parameters for this display */
	//LEFTMARGIN = 46,    /* horizontal blanking EXACTLY */
	//TOPMARGIN = 23,      /* vertical blanking EXACTLY */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,      /* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,      /* Negative polarity required for HSYNC signal */
	.deneg = 0,        /* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,  // LQ043T3DX02K require DE reset
	///.ltdc_dotclk = 53000000uL,  // частота пикселей при работе с интерфейсом RGB
	.fps = 60,  /* frames per second */
	.ntsc = 0,
	.interlaced = 0
};
#else
	//#error Unsupported LCDMODE_xxx

#endif

/* NTSC TV out parameters */
/* Aspect ratio 1.5 */
static const videomode_t vdmode_NTSC0 =
{
	.width = 720,			/* LCD PIXEL WIDTH            */
	.height = 480,			/* LCD PIXEL HEIGHT           */

	.hsync = 63,				/* Horizontal synchronization */
	.hbp = 60,				/* Horizontal back porch      */
	.hfp = 15,				/* Horizontal front porch */

	.vsync = 7,			/* Vertical synchronization */
	.vbp = 30,				/* Vertical back porch      */
	.vfp = 8,				/* Vertical front porch */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */

	.fps = 60,	/* frames per second */
	.ntsc = 1,
	.interlaced = 1
};

/* PAL TV out parameters */
/* Aspect ratio 1.25 */
static const videomode_t vdmode_PAL0 =
{
	.width = 720,			/* LCD PIXEL WIDTH            */
	.height = 576,			/* LCD PIXEL HEIGHT           */

	.hsync = 65,			/* Horizontal synchronization */
	.hbp = 68,				/* Horizontal back porch      */
	.hfp = 11,				/* Horizontal front porch  */

	.vsync = 6,				/* Vertical synchronization */
	.vbp = 39,				/* Vertical back porch      */
	.vfp = 4,				/* Vertical front porch */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */

	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 1
};

//	Horizontal Timings
//		Active Pixels       1920
//		Front Porch           88
//		Sync Width            44
//		Back Porch           148
//		Blanking Total       280
//		Total Pixels        2200
//
//	Vertical Timings
//		Active Lines        1080
//		Front Porch            4
//		Sync Width             5
//		Back Porch            36
//		Blanking Total        45
//		Total Lines         1125

// https://github.com/akatrevorjay/edid-generator/tree/master
/* HDMI TV out parameters HD 1920x1080 60 Hz*/
/* Aspect ratio 16:9 (1.7(7)), dot clock = 148.5 MHz */
// https://edid.tv/edid/2253/
// ! TESTED
//	1920x1080 (0x47)  138.5MHz -HSync -VSync *current +preferred
//	 h: width  1920 start 1968 end 2000 total 2080 skew    0 clock   66.6KHz
//	 v: height 1080 start 1088 end 1102 total 1110           clock   60.0Hz
static const videomode_t vdmode_HDMI_1920x1080at60 =
{
	.width = 1920,			/* LCD PIXEL WIDTH            */
	.height = 1080,			/* LCD PIXEL HEIGHT           */

	// Horizontal Blanking XBLANK = hsync + hbp + hfp = 280
	.hsync = 44,			/* Horizontal synchronization XPULSE  */
	.hbp = 148,				/* Horizontal back porch  XBLANK - XOFFSET - XPULSE    */
	.hfp = 88,				/* Horizontal front porch  XOFFSET */

	// Vertical Blanking YBLANK  = vsync + vbp + vfp = 45
	.vsync = 5,				/* Vertical synchronization */
	.vbp = 36,				/* Vertical back porch      */
	.vfp = 4,				/* Vertical front porch */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */

	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

// https://projectf.io/posts/video-timings-vga-720p-1080p/
/* Aspect ratio 16:9 (1.7(7)), dot clock = 80.291 MHz */
// ! TESTED, параметры не подтверждены, работает и на 50 и 60 герц
// 1360×765
// https://billauer.co.il/blog/2015/07/vesa-edid-parameters/
static const videomode_t vdmode_HDMI_1366x768at60 =
{
	.width = 1366,			/* LCD PIXEL WIDTH            */
	.height = 768,			/* LCD PIXEL HEIGHT           */

	// Horizontal Blanking XBLANK = hsync + hbp + hfp = 280
	.hsync = 44,			/* Horizontal synchronization XPULSE  */
	.hbp = 148,				/* Horizontal back porch  XBLANK - XOFFSET - XPULSE    */
	.hfp = 88,				/* Horizontal front porch  XOFFSET */

	// Vertical Blanking YBLANK  = vsync + vbp + vfp = 45
	.vsync = 5,				/* Vertical synchronization */
	.vbp = 36,				/* Vertical back porch      */
	.vfp = 4,				/* Vertical front porch */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */

	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

/* Aspect ratio 16:9 (1.7(7)), dot clock = xxx MHz */
// ! TESTED
// https://tomverbeure.github.io/video_timings_calculator
// CVT-RB Modeline "1024x768_59.87" 56 1024 1072 1104 1184 768 771 775 790 +HSync -VSync
static const videomode_t vdmode_HDMI_1024x768at60 =
{
	.width = 1024,			/* LCD PIXEL WIDTH            */
	.height = 768,			/* LCD PIXEL HEIGHT           */

	// Horizontal Blanking XBLANK = hsync + hbp + hfp = 160
	.hsync = 32,			/* Horizontal synchronization XPULSE  */
	.hbp = 80,				/* Horizontal back porch  XBLANK - XOFFSET - XPULSE    */
	.hfp = 48,				/* Horizontal front porch  XOFFSET */

	// Vertical Blanking YBLANK  = vsync + vbp + vfp = 22
	.vsync = 4,				/* Vertical synchronization */
	.vbp = 15,				/* Vertical back porch      */
	.vfp = 3,				/* Vertical front porch */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */

	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

//	1440x900 (0xb5)  106.5MHz -HSync +VSync
//	h: width  1440 start 1520 end 1672 total 1904 skew    0 clock   55.9KHz
//	v: height  900 start  903 end  909 total  934           clock   59.9Hz
// ! TESTED, параметры не подтверждены, работает на 50 герц
// 74.720 MHz
// https://tomverbeure.github.io/video_timings_calculator
// CVT-RB
static const videomode_t vdmode_HDMI_1440x900at50 =
{
	.width = 1440,			/* LCD PIXEL WIDTH            */
	.height = 900,			/* LCD PIXEL HEIGHT           */

	// Horizontal Blanking XBLANK = hsync + hbp + hfp = 160
    .hsync = 32,            /* Horizontal synchronization XPULSE  */
    .hbp = 80,    			/* Horizontal back porch  XBLANK - XOFFSET - XPULSE    */
    .hfp = 48,              /* Horizontal front porch  XOFFSET */

	// Vblank от DMT - иначе не работает
	// Vertical Blanking YBLANK  = vsync + vbp + vfp = 34
	.vsync = 6,				/* Vertical synchronization */
 	.vbp = 25,                /* Vertical back porch      */
	.vfp = 3,				/* Vertical front porch */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 0,			/* Positive polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */

	.fps = 50,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

// Pixel Clock: 74.25MHz
/* Aspect ratio 16:9 (1.7(7)), dot clock = 74.250 MHz */
// ! TESTED
static const videomode_t vdmode_HDMI_1280x720at50 =
{
	.width = 1280,			/* LCD PIXEL WIDTH            */
	.height = 720,			/* LCD PIXEL HEIGHT           */

	// Horizontal Blanking XBLANK = hsync + hbp + hfp = 700
	.hsync = 40,			/* Horizontal synchronization XPULSE  */
	.hbp = 700 - 40 - 440,	/* Horizontal back porch  XBLANK - XOFFSET - XPULSE    */
	.hfp = 440,				/* Horizontal front porch  XOFFSET */

	// Vertical Blanking YBLANK  = vsync + vbp + vfp = 30
	.vsync = 5,				/* Vertical synchronization */
	.vbp = 30 - 5 - 5,				/* Vertical back porch      */
	.vfp = 5,				/* Vertical front porch */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */

	.fps = 50,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

const videomode_t * get_videomode_CRT(void)
{
	return & vdmode_PAL0;
}

const videomode_t * get_videomode_HDMI(void)
{
	//return & vdmode_HDMI_1024x768at60;	// ! TESTED
	//return & vdmode_HDMI_1366x768at60;	// ! TESTED, параметры не подтверждены, работает и на 50 и 60 герц
	return & vdmode_HDMI_1280x720at50;	// ! TESTED
	//return & vdmode_HDMI_1440x900at50;	// ! TESTED, параметры не подтверждены, работает на 50 герц
	return & vdmode_HDMI_1920x1080at60;	// ! TESTED
}

#endif /* WITHLTDCHW */

#if WITHLTDCHW

/* для эесперементов с масштабиованием в DE - когда DIM_X & DIM_Y не соответствуют подключённому дисплею */
const videomode_t * get_videomode_LCD(void)
{
	//return & vdmode_800x480;
	return & vdmode0;
}

const videomode_t * get_videomode_DESIGN(void)
{
	return & vdmode0;
}

#endif /* WITHLTDCHW */

/*
 * настройка портов для последующей работы с дополнительными (кроме последовательного канала)
 * сигналами дисплея.
 */

/* вызывается при запрещённых прерываниях. */
void display_hardware_initialize(void)
{
	PRINTF(PSTR("display_hardware_initialize start\n"));


#if WITHDMA2DHW
	// Image construction hardware
	arm_hardware_dma2d_initialize();

#endif /* WITHDMA2DHW */
#if WITHMDMAHW
	// Image construction hardware
	arm_hardware_mdma_initialize();

#endif /* WITHMDMAHW */

#if WITHLTDCHW
	{
		hardware_ltdc_initialize(get_videomode_LCD());
		colmain_setcolors(COLORPIP_WHITE, COLORPIP_BLACK);
	}

	//hardware_ltdc_main_set(RTMIXIDLCD, (uintptr_t) colmain_fb_draw());
	hardware_ltdc_L8_palette();
#endif /* WITHLTDCHW */

#if LCDMODETX_TC358778XBG
	const videomode_t * const vdmode = get_videomode_LCD();
	tc358768_initialize(vdmode);
	panel_initialize(vdmode);
#endif /* LCDMODETX_TC358778XBG */
#if LCDMODEX_SII9022A
	/* siiI9022A Lattice Semiconductor Corp HDMI Transmitter */
	sii9022x_initialize(vdmode);
#endif /* LCDMODEX_SII9022A */

	PRINTF(PSTR("display_hardware_initialize done\n"));
}

void display_wakeup(void)
{
#if WITHLTDCHW
	{
		hardware_ltdc_initialize(get_videomode_LCD());
		colmain_setcolors(COLORPIP_WHITE, COLORPIP_BLACK);
	}
#endif /* WITHLTDCHW */
#if LCDMODETX_TC358778XBG
	const videomode_t * const vdmode = get_videomode_LCD();
  tc358768_wakeup(vdmode);
    panel_wakeup();
#endif /* LCDMODETX_TC358778XBG */
#if LCDMODEX_SII9022A
    // siiI9022A Lattice Semiconductor Corp HDMI Transmitter
    sii9022x_wakeup(vdmode);
#endif /* LCDMODEX_SII9022A */
}


// https://habr.com/ru/post/166317/

//	Hue - тон, цикличная угловая координата.
//	Value, Brightness - яркость, воспринимается как альфа-канал, при v=0 пиксель не светится,
//	при v=17 - светится максимально ярко, в зависимости от H и S.
//	Saturation. С отсутствием фона, значения  дадут не серый цвет, а белый разной яркости,
//	поэтому параметр W=Smax-S можно называть Whiteness - он отражает степень "белизны" цвета.
//	При W=0, S=Smax=15 цвет полностью определяется Hue, при S=0, W=Wmax=15 цвет пикселя
//	будет белым.

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_t;

typedef struct {
    uint8_t h;
    uint8_t s;
    uint8_t v;
} HSV_t;

const uint8_t max_whiteness = 15;
const uint8_t max_value = 17;

enum
{
	sixth_hue = 16,
	third_hue = sixth_hue * 2,
	half_hue = sixth_hue * 3,
	two_thirds_hue = sixth_hue * 4,
	five_sixths_hue = sixth_hue * 5,
	full_hue = sixth_hue * 6
};

RGB_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return (RGB_t) { r, g, b };
}

HSV_t hsv(uint8_t h, uint8_t s, uint8_t v)
{
    return (HSV_t) { h, s, v };
}

RGB_t hsv2rgb(HSV_t hsv)
{
	const RGB_t black = { 0, 0, 0 };

    if (hsv.v == 0) return black;

    uint8_t high = hsv.v * max_whiteness;//channel with max value
    if (hsv.s == 0) return rgb(high, high, high);

    uint8_t W = max_whiteness - hsv.s;
    uint8_t low = hsv.v * W;//channel with min value
    uint8_t rising = low;
    uint8_t falling = high;

    const uint8_t h_after_sixth = hsv.h % sixth_hue;
    if (h_after_sixth > 0)
    {
    	//not at primary color? ok, h_after_sixth = 1..sixth_hue - 1
        const uint8_t z = hsv.s * (uint8_t) (hsv.v * h_after_sixth) / sixth_hue;
        rising += z;
        falling -= z + 1;//it's never 255, so ok
    }

    uint8_t H = hsv.h;
    while (H >= full_hue)
    	H -= full_hue;

    if (H < sixth_hue) return rgb(high, rising, low);
    if (H < third_hue) return rgb(falling, high, low);
    if (H < half_hue) return rgb(low, high, rising);
    if (H < two_thirds_hue) return rgb(low, falling, high);
    if (H < five_sixths_hue) return rgb(rising, low, high);
    return rgb(high, low, falling);
}

#if WITHRLEDECOMPRESS

COLORPIP_T convert_565_to_a888(uint16_t color)
{
	uint8_t b5 = (color & 0x1F) << 3;
	uint8_t g6 = ((color & 0x7E0) >> 5) << 2;
	uint8_t r5 = ((color & 0xF800) >> 11) << 3;

	return TFTRGB(r5, g6, b5);
}

void graw_picture_RLE(const gxdrawb_t * db, uint16_t x, uint16_t y, const picRLE_t * picture, PACKEDCOLORPIP_T bg_color)
{
	uint_fast32_t i = 0;
	uint_fast16_t x1 = x, y1 = y;
	uint_fast16_t transparent_color = 0, count = 0;

	while (y1 < y + picture->height)
	{
		if ((int16_t)picture->data [i] < 0) // no repeats
		{
			count = (-(int16_t)picture->data [i]);
			i ++;
			for (uint_fast16_t p = 0; p < count; p ++)
			{
				const COLORPIP_T point = convert_565_to_a888(picture->data [i]);
				colpip_point(db, x1, y1, picture->data [i] == 0 ? bg_color : point);

				x1 ++;
				if (x1 >= x + picture->width)
				{
					x1 = x;
					y1 ++;
				}
				i ++;
			}
		}
		else // repeats
		{
			count = ((int16_t)picture->data [i]);
			i++;

			const COLORPIP_T point = convert_565_to_a888(picture->data [i]);
			for (uint_fast16_t p = 0; p < count; p ++)
			{
				colpip_point(db, x1, y1, picture->data [i] == 0 ? bg_color : point);

				x1 ++;
				if (x1 >= x + picture->width)
				{
					x1 = x;
					y1 ++;
				}
			}
			i ++;
		}
	}
}

void graw_picture_RLE_buf(const gxdrawb_t * db, uint16_t x, uint16_t y, const picRLE_t * picture, COLORPIP_T bg_color)
{
	uint_fast32_t i = 0;
	uint_fast16_t x1 = x, y1 = y;
	uint_fast16_t transparent_color = 0, count = 0;

	while (y1 < y + picture->height)
	{
		if ((int16_t)picture->data [i] < 0) // no repeats
		{
			count = (-(int16_t)picture->data [i]);
			i ++;
			for (uint_fast16_t p = 0; p < count; p++)
			{
				COLORPIP_T point = convert_565_to_a888(picture->data [i]);
				colpip_point(db, x1, y1, picture->data [i] == transparent_color ? bg_color : point);

				x1 ++;
				if (x1 >= x + picture->width)
				{
					x1 = x;
					y1 ++;
				}
				i ++;
			}
		}
		else // repeats
		{
			count = ((int16_t)picture->data [i]);
			i ++;

			PACKEDCOLORPIP_T point = convert_565_to_a888(picture->data [i]);
			for (uint_fast16_t p = 0; p < count; p++)
			{
				colpip_point(db, x1, y1, picture->data[i] == transparent_color ? bg_color : point);

				x1 ++;
				if (x1 >= x + picture->width)
				{
					x1 = x;
					y1 ++;
				}
			}
			i ++;
		}
	}
}

#endif /* WITHRLEDECOMPRESS */

#if LCDMODE_LTDC

void display_do_AA(
	const gxdrawb_t * db,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз)
	uint_fast16_t width,
	uint_fast16_t height
	)
{
	uint_fast16_t x;
	for (x = col; x < (col + width - 1); x ++)
	{
		uint_fast16_t y;
		for (y = row; y < (row + height - 1); y ++)
		{
			const COLORPIP_T p1 = * colpip_mem_at(db, x, y);
			const COLORPIP_T p2 = * colpip_mem_at(db, x + 1, y);
			const COLORPIP_T p3 = * colpip_mem_at(db, x, y + 1);
			const COLORPIP_T p4 = * colpip_mem_at(db, x + 1, y + 1);

			unsigned p1_r, p1_g, p1_b, p2_r, p2_g, p2_b, p3_r, p3_g, p3_b, p4_r, p4_g, p4_b;
			unsigned p_r, p_b, p_g;

			 if ((p1 == p2) && (p1 == p3) && (p1 == p4))	// если смежные пиксели одинакового цвета, пропустить расчёт
				continue;

			p1_r = COLORPIP_R(p1);
			p1_g = COLORPIP_G(p1);
			p1_b = COLORPIP_B(p1);

			p2_r = COLORPIP_R(p2);
			p2_g = COLORPIP_G(p2);
			p2_b = COLORPIP_B(p2);

			p3_r = COLORPIP_R(p3);
			p3_g = COLORPIP_G(p3);
			p3_b = COLORPIP_B(p3);

			p4_r = COLORPIP_R(p4);
			p4_g = COLORPIP_G(p4);
			p4_b = COLORPIP_B(p4);

			p_r = ((uint_fast32_t) p1_r + p2_r + p3_r + p4_r) / 4;
			p_g = ((uint_fast32_t) p1_g + p2_g + p3_g + p4_g) / 4;
			p_b = ((uint_fast32_t) p1_b + p2_b + p3_b + p4_b) / 4;

			colpip_point(db, x, y, TFTRGB(p_r, p_g, p_b));
		}
	}
}

#endif /* LCDMODE_LTDC */


/* Получить желаемую частоту pixel clock для данного видеорежима. */
uint_fast32_t display_getdotclock(const videomode_t * vdmode)
{
	/* Accumulated parameters for this display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned HSYNC = vdmode->hsync;	/*  */
	const unsigned VSYNC = vdmode->vsync;	/*  */
	const unsigned LEFTMARGIN = HSYNC + vdmode->hbp;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = VSYNC + vdmode->vbp;	/* vertical delay before DE start */
	const unsigned HTOTAL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VTOTAL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

	return (uint_fast32_t) vdmode->fps * HTOTAL * VTOTAL;
	//return (uint_fast32_t) vdmode->fps * HTOTAL * VTOTAL / (vdmode->interlaced + 1);
	//return vdmode->ltdc_dotclk;
}
