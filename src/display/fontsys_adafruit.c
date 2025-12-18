/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Поддержка шрифтов Adafruit-GFX-Library
//	https://github.com/adafruit/Adafruit-GFX-Library.git

#include "hardware.h"

#if LCDMODE_LTDC

#include "formats.h"
#include "display.h"

static uint_fast16_t
adafruitfont_decode(const unifont_t * font, char cc)
{
	const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
	const uint_fast16_t c = (unsigned char) cc;
	if (c < gfxfont->first)
		return 0;
	if (c >= (gfxfont->last))
		return 0;
	return c - gfxfont->first;
}

// Для пропорциональных знакогенераторов
static uint_fast8_t adafruitfont_width(const unifont_t * font, char cc)
{
	const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
	const uint_fast16_t ci = font->decode(font, cc);
	const hftrx_GFXglyph_t * const glyph = & gfxfont->glyph [ci];
	return glyph->width;
}

static const void * adafruitfont_getcharraster(const unifont_t * font, char cc)
{
	const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
	const uint_fast16_t ci = font->decode(font, cc);
	const hftrx_GFXglyph_t * const glyph = & gfxfont->glyph [ci];
	return glyph->width ? & gfxfont->bitmap [glyph->bitmapOffset] : NULL;
}

// Для пропорциональных знакогенераторов
static uint_fast8_t adafruitfont_height(const unifont_t * font)
{
	const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
	uint_fast16_t ci;
	uint_fast16_t hmax = 0;
	for (ci = 0; ci < (gfxfont->last - gfxfont->first + 1); ++ ci)
	{
		const hftrx_GFXglyph_t * const glyph = & gfxfont->glyph [ci];
		hmax = ulmax16(hmax, glyph->yOffset + glyph->height);
	}
	return hmax;
}

static uint_fast16_t
adafruitfont_render_char(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	)
{
	const hftrx_GFXfont_t * const gfxfont = (const hftrx_GFXfont_t * const) font->fontraster;
	const uint_fast16_t ci = font->decode(font, cc);
	const hftrx_GFXglyph_t * const glyph = & gfxfont->glyph [ci];
	const uint8_t * const charraster = (const uint8_t *) font->getcharraster(font, cc);

	if (charraster != NULL)
	{
		const uint_fast16_t width2 = font->font_drawwidth(font, cc);
		const uint_fast16_t height2 = font->font_drawheight(font);
//		const unsigned bytespervertical = (height2 + 7) / 8;
		PRINTF("adafruitfont_render_char: cc=%02X (%c), width2=%u, height2=%u, bytespervertical=%u\n", (unsigned char) cc, cc, width2, height2, bytespervertical);
		printhex(0, charraster, (glyph->height * glyph->width + 7) / 8);
		uint_fast16_t row;
		for (row = glyph->yOffset; row < (glyph->yOffset + glyph->height); ++ row)
		{
			uint_fast16_t col;
			for (col = glyph->xOffset; col < (glyph->xOffset + glyph->width); ++ col)
			{
			}
		}
	}
	return xpix + glyph->xAdvance;
}



#include "FreeMono24pt7b.h"

const unifont_t unifont_FreeMono24pt7b =
{
		.decode = adafruitfont_decode,
		.getcharraster = adafruitfont_getcharraster,
		.font_drawwidth = adafruitfont_width,
		.font_drawheight = adafruitfont_height,
		.font_draw = adafruitfont_render_char,
		//
		.fontraster = & FreeMono24pt7b,
		.label = "FreeMono24pt7b"
};

#endif	/* LCDMODE_LTDC */
