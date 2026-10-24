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

#if LCDMODE_LTDC || WITHTOUCHGUI

#include "formats.h"
#include "display.h"
#include "fontsys.h"
#include <Adafruit_GFX.h>

typedef struct adafruitfont_data_tag
{
	int16_t	baseline;
	int16_t	height;
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
		return INT16_MAX;
	if (c > gfxfont->last)
		return INT16_MAX;
	return c - gfxfont->first;
}

static uint_fast16_t
adafruitfont_totalci(const unifont_t * font)
{
	const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
	return gfxfont->last - gfxfont->first + 1;
}
static uint_fast8_t adafruitfont_disabled(const hftrx_GFXglyph_t * const glyph)
{
	return ! glyph->height && ! glyph->width && ! glyph->xOffset && ! glyph->yOffset;
}

static uint_fast8_t adafruitfont_width(const unifont_t * font, uint_fast16_t ci)
{
	const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
	const hftrx_GFXglyph_t * const glyph = & gfxfont->glyph [ci];
	if (INT16_MAX == ci) return 0;
	return adafruitfont_disabled(glyph) ? 0 : glyph->xAdvance;
}

static const uint8_t * adafruitfont_getcharraster(const unifont_t * font, uint_fast16_t ci)
{
	const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
	const hftrx_GFXglyph_t * const glyph = & gfxfont->glyph [ci];
	if (INT16_MAX == ci) return NULL;
	if (adafruitfont_disabled(glyph))
		return NULL;
	return (glyph->width && glyph->height) ? & gfxfont->bitmap [glyph->bitmapOffset] : NULL;
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
	const uint8_t * const charraster = adafruitfont_getcharraster(font, ci);
	const int_fast16_t baseline = adafruitfont_preparedata(font)->baseline;

	const hftrx_GFXglyph_t * const glyph = & gfxfont->glyph [ci];
	if (adafruitfont_disabled(glyph))
		return xpix;
	if (charraster != NULL)
	{
		int_fast16_t row;	// source bitmap pos
		for (row = 0; row < glyph->height; ++ row)
		{
			const unsigned bitrowpos = row * glyph->width;
			PACKEDCOLORPIP_T * tgr = colpip_mem_at(db, (int_fast16_t) xpix + glyph->xOffset, (int_fast16_t) ypix + row + glyph->yOffset + baseline);
			int_fast16_t col;	// source bitmap pos
			for (col = 0; col < glyph->width; ++ col, ++ tgr)
			{
				const unsigned bitpos = bitrowpos + col;
				const unsigned byteoffset = bitpos / 8;
				const unsigned bitoffset = 7 - bitpos % 8;
				if ((charraster [byteoffset] >> bitoffset) & 0x01)
					* tgr = fg;
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
static adafruitfont_data_t unifontdata_16x15;
const unifont_t unifont_small =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & adafruit_16x15,
	.fontdata = & unifontdata_16x15,
	.label = "adafruit_16x15"
};
#endif

#if 1

#include "fonts/adafruit_8x8.h"
static adafruitfont_data_t unifontdata_8x8;
const unifont_t unifont_small3 =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & adafruit_8x8,
	.fontdata = & unifontdata_8x8,
	.label = "adafruit_8x8"
};
#endif

#if 1

#include "fonts/adafruit_16x10.h"
static adafruitfont_data_t unifontdata_16x10;
const unifont_t unifont_small2 =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & adafruit_16x10,
	.fontdata = & unifontdata_16x10,
	.label = "adafruit_16x16"
};
#endif

#if WITHALTERNATIVEFONTS

#include "fonts/CenturyGothic_28x54.h"
static adafruitfont_data_t unifontdata_28x54;
const unifont_t unifont_half_raw =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & CenturyGothic_28x54,
	.fontdata = & unifontdata_28x54,
	.label = "CenturyGothic_28x54"
};

#include "fonts/CenturyGothic_36x54.h"
static adafruitfont_data_t unifontdata_36x54;
const unifont_t unifont_big_raw =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & CenturyGothic_36x54,
	.fontdata = & unifontdata_36x54,
	.label = "CenturyGothic_36x54"
};

#include "fonts/CenturyGothic_21x40.h"
static adafruitfont_data_t unifontdata_21x40;
const unifont_t unifont_half2_raw =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & CenturyGothic_21x40,
	.fontdata = & unifontdata_21x40,
	.label = "CenturyGothic_21x40"
};

#include "fonts/CenturyGothic_27x40.h"
static adafruitfont_data_t unifontdata_27x40;
const unifont_t unifont_big2_raw =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & CenturyGothic_27x40,
	.fontdata = & unifontdata_27x40,
	.label = "CenturyGothic_27x40"
};
#else /* WITHALTERNATIVEFONTS */

#include "fonts/adafruit_28x54.h"
static adafruitfont_data_t unifontdata_28x54;
const unifont_t unifont_half_raw =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & adafruit_28x54,
	.fontdata = & unifontdata_28x54,
	.label = "adafruit_28x54"
};

#include "fonts/adafruit_36x54.h"
static adafruitfont_data_t unifontdata_36x54;
const unifont_t unifont_big_raw =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & adafruit_36x54,
	.fontdata = & unifontdata_36x54,
	.label = "adafruit_36x54"
};

#include "fonts/adafruit_21x40.h"
static adafruitfont_data_t unifontdata_21x40;
const unifont_t unifont_half2_raw =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & adafruit_21x40,
	.fontdata = & unifontdata_21x40,
	.label = "adafruit_21x40"
};

#include "fonts/adafruit_27x40.h"
static adafruitfont_data_t unifontdata_27x40;
const unifont_t unifont_big2_raw =
{
	.decode = adafruitfont_decode,
	.totalci = adafruitfont_totalci,
	.font_drawwidthci = adafruitfont_width,
	.font_drawheight = adafruitfont_height,
	.font_drawci = adafruitfont_render_char,
	//
	.fontraster = & adafruit_27x40,
	.fontdata = & unifontdata_27x40,
	.label = "adafruit_27x40"
};

#endif /* WITHALTERNATIVEFONTS */
#endif	/* LCDMODE_LTDC || WITHTOUCHGUI */
