/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Поддержка шрифтов Adafruit-GFX-Library
//	https://github.com/adafruit/Adafruit-GFX-Library.git

//	You can also use this GFX Font Customiser tool (web version here)
// 	https://github.com/tchapi/Adafruit-GFX-Font-Customiser
//	https://tchapi.github.io/Adafruit-GFX-Font-Customiser/
//	to customize or correct the output from fontconvert,
//	and create fonts with only a subset of characters to optimize size.

#include "hardware.h"

#if LCDMODE_LTDC

#include "formats.h"
#include "display.h"
#include <Adafruit_GFX.h>

typedef struct adafruitfont_data_tag
{
	uint16_t	baseline;
	uint16_t	height;
} adafruitfont_data_t;


static int imin(int a, int b) { return a < b ? a : b; }
static int imax(int a, int b) { return a > b ? a : b; }

static const adafruitfont_data_t * adafruitfont_preparedata(const unifont_t * font)
{
	adafruitfont_data_t * data = (adafruitfont_data_t *) font->fontdata;
	if (data->baseline == 0 && data->height == 0)
	{
		const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
		uint_fast16_t ci;
		int_fast16_t yOffset = 0;
		int_fast16_t bottom = 0;
		for (ci = 0; ci < (gfxfont->last - gfxfont->first + 1); ++ ci)
		{
			const hftrx_GFXglyph_t * const glyph = & gfxfont->glyph [ci];
			yOffset = imin(yOffset, glyph->yOffset);
			bottom = imax(bottom, glyph->yOffset + (int) glyph->height);
		}
		data->baseline = - yOffset;	// 0 - включет нижний пиксель растра
		data->height = data->baseline + bottom;
	}
	return data;
}

static uint_fast16_t
adafruitfont_decode(const unifont_t * font, char cc)
{
	const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
	const uint_fast16_t c = (unsigned char) cc;
	if (c < gfxfont->first)
		return 0;
	if (c > gfxfont->last)
		return 0;
	return c - gfxfont->first;
}

static uint_fast16_t
adafruitfont_totalci(const unifont_t * font)
{
	const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
	return gfxfont->last - gfxfont->first + 1;
}

// Для пропорциональных знакогенераторов
static uint_fast8_t adafruitfont_width(const unifont_t * font, uint_fast16_t ci)
{
	const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
	const hftrx_GFXglyph_t * const glyph = & gfxfont->glyph [ci];
	return glyph->xAdvance;
}

static const void * adafruitfont_getcharraster(const unifont_t * font, uint_fast16_t ci)
{
	const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
	const hftrx_GFXglyph_t * const glyph = & gfxfont->glyph [ci];
	return glyph->width ? & gfxfont->bitmap [glyph->bitmapOffset] : NULL;
}

// Для пропорциональных знакогенераторов
static uint_fast8_t adafruitfont_height(const unifont_t * font)
{
	const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
	//return gfxfont->yAdvance;
	return adafruitfont_preparedata(font)->height;
}

static uint_fast16_t
adafruitfont_render_char(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	uint_fast16_t ci,		// код символа для отображения
	COLORPIP_T fg
	)
{
	const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
	const hftrx_GFXglyph_t * const glyph = & gfxfont->glyph [ci];
	const uint8_t * const charraster = (const uint8_t *) font->getcharrasterci(font, ci);
	const int_fast16_t baseline = adafruitfont_preparedata(font)->baseline;

	if (charraster != NULL)
	{
		int_fast16_t row;	// source bitmap pos
		for (row = 0; row < glyph->height; ++ row)
		{
			int_fast16_t col;	// source bitmap pos
			for (col = 0; col < glyph->width; ++ col)
			{
				const unsigned bitpos = row * glyph->width + col;
				const unsigned byteoffset = bitpos / 8;
				const unsigned bitoffset = 7 - bitpos % 8;
				if ((charraster [byteoffset] >> bitoffset) & 0x01)
				{
					* colpip_mem_at(db, xpix + col + glyph->xOffset, ypix + row + glyph->yOffset + baseline) = fg;
				}
			}
		}
	}
	return xpix + glyph->xAdvance;
}


#if 1

#include "fonts/FreeMono9pt7b.h"
static adafruitfont_data_t unifontdata_FreeMono9pt7b;
const unifont_t unifont_FreeMono9pt7b =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.getcharrasterci = adafruitfont_getcharraster,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & FreeMono9pt7b,
	.fontdata = & unifontdata_FreeMono9pt7b,
	.label = "FreeMono9pt7b"
};
#endif

#if 1

#include "fonts/FreeMono12pt7b.h"
static adafruitfont_data_t unifontdata_FreeMono12pt7b;
const unifont_t unifont_FreeMono12pt7b =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.getcharrasterci = adafruitfont_getcharraster,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & FreeMono12pt7b,
	.fontdata = & unifontdata_FreeMono12pt7b,
	.label = "FreeMono12pt7b"
};
#endif

#if 1

#include "fonts/FreeMono18pt7b.h"
static adafruitfont_data_t unifontdata_FreeMono18pt7b;
const unifont_t unifont_FreeMono18pt7b =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.getcharrasterci = adafruitfont_getcharraster,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & FreeMono18pt7b,
	.fontdata = & unifontdata_FreeMono18pt7b,
	.label = "FreeMono18pt7b"
};
#endif

#if 1

#include "fonts/FreeMono24pt7b.h"
static adafruitfont_data_t unifontdata_FreeMono24pt7b;
const unifont_t unifont_FreeMono24pt7b =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.getcharrasterci = adafruitfont_getcharraster,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & FreeMono24pt7b,
	.fontdata = & unifontdata_FreeMono24pt7b,
	.label = "FreeMono24pt7b"
};
#endif

#if 1

#include "fonts/FreeSans12pt7b.h"
static adafruitfont_data_t unifontdata_FreeSans12pt7b;
const unifont_t unifont_FreeSans12pt7b =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.getcharrasterci = adafruitfont_getcharraster,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & FreeSans12pt7b,
	.fontdata = & unifontdata_FreeSans12pt7b,
	.label = "FreeSans12pt7b"
};
#endif


#if 1

#include "fonts/adafruit_16x15.h"
static adafruitfont_data_t unifontdata_small16x15;
const unifont_t unifont_small =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.getcharrasterci = adafruitfont_getcharraster,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & adafruit_16x15,
	.fontdata = & unifontdata_small16x15,
	.label = "adafruit_16x15"
};
#endif

#if 1

#include "fonts/adafruit_8x8.h"
static adafruitfont_data_t unifontdata_small8x8;
const unifont_t unifont_small3 =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.getcharrasterci = adafruitfont_getcharraster,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & adafruit_8x8,
	.fontdata = & unifontdata_small8x8,
	.label = "adafruit_8x8"
};
#endif

#if 1

#include "fonts/adafruit_16x10.h"
static adafruitfont_data_t unifontdata_small16x10;
const unifont_t unifont_small2 =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.getcharrasterci = adafruitfont_getcharraster,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & adafruit_16x10,
	.fontdata = & unifontdata_small16x10,
	.label = "adafruit_16x16"
};
#endif

#if WITHALTERNATIVEFONTS

#include "fonts/CenturyHothic_28x54.h"
static adafruitfont_data_t unifontdata_small28x54;
const unifont_t unifont_half_raw =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.getcharrasterci = adafruitfont_getcharraster,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & CenturyHothic_28x54,
	.fontdata = & unifontdata_small28x54,
	.label = "CenturyHothic_28x54"
};

#include "fonts/CenturyHothic_36x54.h"
static adafruitfont_data_t unifontdata_small36x54;
const unifont_t unifont_big_raw =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.getcharrasterci = adafruitfont_getcharraster,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & CenturyHothic_36x54,
	.fontdata = & unifontdata_small36x54,
	.label = "CenturyHothic_36x54"
};
#else /* WITHALTERNATIVEFONTS */

#include "fonts/adafruit_28x54.h"
static adafruitfont_data_t unifontdata_small28x54;
const unifont_t unifont_half_raw =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.getcharrasterci = adafruitfont_getcharraster,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & adafruit_28x54,
	.fontdata = & unifontdata_small28x54,
	.label = "adafruit_28x54"
};

#include "fonts/adafruit_36x54.h"
static adafruitfont_data_t unifontdata_small36x54;
const unifont_t unifont_big_raw =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.getcharrasterci = adafruitfont_getcharraster,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & adafruit_36x54,
	.fontdata = & unifontdata_small36x54,
	.label = "adafruit_36x54"
};

#endif /* WITHALTERNATIVEFONTS */

#endif	/* LCDMODE_LTDC */
