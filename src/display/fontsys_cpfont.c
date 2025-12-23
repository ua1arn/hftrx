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

#endif /* LCDMODE_LTDC */
