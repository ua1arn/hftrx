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
// todo: нет зазора между символами
static uint_fast8_t aptechfont_width(const unifont_t * font, uint_fast16_t ci)
{
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	const uint_fast16_t   font_Size_in_Bytes_over_all_included_Size_it_self = USBD_peek_u16(blob + 0); // size of zero indicates fixed width font, actual length is width * height
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

static const void * aptechfont_getcharraster(const unifont_t * font, uint_fast16_t ci)
{
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
	uint_fast16_t ci,
	COLORPIP_T fg
	)
{
	const uint8_t * const charraster = (const uint8_t * const) font->getcharrasterci(font, ci);
	const uint_fast16_t width2 = font->font_drawwidthci(font, ci);
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

#if LCDMODE_LTDC

#include "fonts_x/roboto32.h"
//#include "fonts_x/SystemFont5x7.h"
//#include "fonts_x/fixednums15x31.h"

const unifont_t unifont_roboto32 =
{
	.decode = aptechfont_decode,
	.getcharrasterci = aptechfont_getcharraster,
	.font_drawwidthci = aptechfont_width,
	.font_drawheight = aptechfont_height,
	.font_drawci = aptechfont_render_char,
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
	.getcharrasterci = aptechfont_getcharraster,
	.font_drawwidthci = aptechfont_width,
	.font_drawheight = aptechfont_height,
	.font_drawci = aptechfont_render_char,
	//
	.fontraster = helvNeueTh70,
	.label = "helvNeueTh70"
};

#endif /* LCDMODE_LTDC */
