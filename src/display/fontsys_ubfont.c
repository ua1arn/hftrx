/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"

#if LCDMODE_LTDC

#if ! GUI_EXTERNAL_FONTS

#include "formats.h"
#include "display.h"
#include "fontsys.h"

#include <string.h>

// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
// Фон не трогаем
// Самый старший (левый) из указанного количества бит выдвигается первым.
// 16/32 bit raster support
static void ubxfont_pixels(
	PACKEDCOLORPIP_T * __restrict tgr,		// target raster
	uint_fast32_t rasterrow,
	uint_fast32_t mask,	// начальный бит маски (сдвигаем вправо)
	COLORPIP_T fg
	)
{
	if (mask != 0)
	{
		do
		{
			if (rasterrow & mask)
				* tgr = fg;
			tgr += 1;
		} while (mask >>= 1);
	}
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
		ubxfont_pixels(tgr, charraster [cgrow], mask, fg);
	}
	return xpix + width2;
}

static uint_fast16_t
ubxfont_put_char32(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	const uint32_t * const charraster,
	uint_fast32_t mask,	// начальный бит маски (сдвигаем вправо)
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
		ubxfont_pixels(tgr, charraster [cgrow], mask, fg);
	}
	return xpix + width2;
}

// ***********************

// Для моноширинных знакогенераторов
uint_fast8_t ubmfont_width(const unifont_t * font, uint_fast16_t ci)
{
	const UB_Font * const ub = (const UB_Font *) font->fontraster;
	(void) ci;
	return ub->width;	// полная ширина символа в пикселях
}

// Для моноширинных знакогенераторов
uint_fast8_t ubmfont_width32(const unifont_t * font, uint_fast16_t ci)
{
	const UB_Font32 * const ub = (const UB_Font32 *) font->fontraster;
	(void) ci;
	return ub->width;	// полная ширина символа в пикселях
}

// Для моноширинных знакогенераторов
uint_fast8_t ubmfont_height(const unifont_t * font)
{
	const UB_Font * const ub = (const UB_Font *) font->fontraster;
	return ub->height;
}

// Для моноширинных знакогенераторов
uint_fast8_t ubmfont_height32(const unifont_t * font)
{
	const UB_Font32 * const ub = (const UB_Font32 *) font->fontraster;
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

// Возвращает индекс символа в знакогенераторе
// Для моноширинных знакогенераторов
uint_fast16_t
ubmfont_decode32(const unifont_t * font, char cc)
{
	//const UB_Font32 * const ub = (const UB_Font32 *) font->fontraster;
	const unsigned char c = (unsigned char) cc;
	if (c < 0x20)
		return 0;
	return c - 0x20;
}

uint_fast16_t
ubmfont_totalci(const unifont_t * font)
{
	//const UB_Font * const ub = (const UB_Font *) font->fontraster;
	return 0x7F - 0x20;
}

uint_fast16_t
ubmfont_totalci32(const unifont_t * font)
{
	//const UB_Font32 * const ub = (const UB_Font32 *) font->fontraster;
	return 0x7F - 0x20;
}

// Для моноширинных знакогенераторов
static const uint16_t * ubmfont_getcharraster16(const unifont_t * font, uint_fast16_t ci)
{
	const UB_Font * const ub = (const UB_Font *) font->fontraster;
	const uint16_t * const table = ub->table; // Таблица с данными
	return table + ci * ub->height;
}

// Для моноширинных знакогенераторов
static const uint32_t * ubmfont_getcharraster32(const unifont_t * font, uint_fast16_t ci)
{
	const UB_Font32 * const ub = (const UB_Font32 *) font->fontraster;
	const uint32_t * const table = ub->table; // Таблица с данными
	return table + ci * ub->height;
}

// Для моноширинных знакогенераторов
uint_fast16_t
ubmfont_render_char16(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	uint_fast16_t ci,
	COLORPIP_T fg
	)
{
	//const UB_Font * const ub = (const UB_Font *) font->fontraster;
	const uint16_t * const charraster = ubmfont_getcharraster16(font, ci);
	const uint_fast16_t width2 = font->font_drawwidthci(font, ci);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_drawheight(font);	// number of rows
	//const uint_fast16_t bytesw = font->bytesw;	// bytes in each chargen row (unused)
//	PRINTF("ubmfont_render_char16: cc=%02X(%c),width2=%u\n", (unsigned char) cc, cc, width2);
//	printhex16(0, charraster, height2 * 2);
	return ubxfont_put_char16(db, xpix, ypix, font, charraster, UINT16_C(1) << 15, width2, height2, 0 /* (unused) */, fg);
}

// Для моноширинных знакогенераторов
uint_fast16_t
ubmfont_render_char32(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	uint_fast16_t ci,
	COLORPIP_T fg
	)
{
	//const UB_Font * const ub = (const UB_Font *) font->fontraster;
	const uint32_t * const charraster = ubmfont_getcharraster32(font, ci);
	const uint_fast16_t width2 = font->font_drawwidthci(font, ci);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_drawheight(font);	// number of rows
	//const uint_fast16_t bytesw = font->bytesw;	// bytes in each chargen row (unused)
//	PRINTF("ubmfont_render_char16: cc=%02X(%c),width2=%u\n", (unsigned char) cc, cc, width2);
//	printhex16(0, charraster, height2 * 2);
	return ubxfont_put_char32(db, xpix, ypix, font, charraster, UINT16_C(1) << 15, width2, height2, 0 /* (unused) */, fg);
}

// Для пропорциональных знакогенераторов
uint_fast8_t ubpfont_width(const struct unifont_tag * font, uint_fast16_t ci)
{
	const UB_pFont * const ubp = (const UB_pFont *) font->fontraster;
	const uint16_t * const table = ubp->table; // Таблица с данными
	const uint_fast8_t width = table [ci * (ubp->height + 1)];	// первый элемент содержит ширину
	return width;
}

// Для пропорциональных знакогенераторов
uint_fast8_t ubpfont_width32(const struct unifont_tag * font, uint_fast16_t ci)
{
	const UB_pFont32 * const ubp = (const UB_pFont32 *) font->fontraster;
	const uint32_t * const table = ubp->table; // Таблица с данными
	const uint_fast8_t width = table [ci * (ubp->height + 1)];	// первый элемент содержит ширину
	return width;
}

// Для пропорциональных знакогенераторов
uint_fast8_t ubpfont_height(const unifont_t * font)
{
	const UB_pFont * const ubp = (const UB_pFont *) font->fontraster;
	return ubp->height;
}

// Для пропорциональных знакогенераторов
uint_fast8_t ubpfont_height32(const unifont_t * font)
{
	const UB_pFont32 * const ubp = (const UB_pFont32 *) font->fontraster;
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

// Возвращает индекс символа в знакогенераторе
// Для пропорциональных знакогенераторов
uint_fast16_t
ubpfont_decode32(const unifont_t * font, char cc)
{
	const UB_pFont32 * const ubp = (const UB_pFont32 *) font->fontraster;
	const uint_fast8_t c = (unsigned char) cc;
	if (c < ubp->first_char)
		return 0;
	if (c > ubp->last_char)
		return 0;
	return c - ubp->first_char;
}

uint_fast16_t
ubpfont_totalci(const unifont_t * font)
{
	const UB_pFont * const ubp = (const UB_pFont *) font->fontraster;
	return ubp->last_char - ubp->first_char + 1;
}

uint_fast16_t
ubpfont_totalci32(const unifont_t * font)
{
	const UB_pFont32 * const ubp = (const UB_pFont32 *) font->fontraster;
	return ubp->last_char - ubp->first_char + 1;
}

// Для пропорциональных знакогенераторов
static const uint16_t * ubpfont_getcharraster16(const unifont_t * font, uint_fast16_t ci)
{
	const UB_pFont * const ubp = (const UB_pFont *) font->fontraster;
	const uint16_t * const table = ubp->table; // Таблица с данными
	return table + ci * (ubp->height + 1) + 1;
}

// Для пропорциональных знакогенераторов
static const uint32_t * ubpfont_getcharraster32(const unifont_t * font, uint_fast16_t ci)
{
	const UB_pFont32 * const ubp = (const UB_pFont32 *) font->fontraster;
	const uint32_t * const table = ubp->table; // Таблица с данными
	return table + ci * (ubp->height + 1) + 1;
}

// Для пропорциональных знакогенераторов
uint_fast16_t
ubpfont_render_char16(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	uint_fast16_t ci,
	COLORPIP_T fg
	)
{
	const uint16_t * const charraster = ubpfont_getcharraster16(font, ci);
	const uint_fast16_t width2 = font->font_drawwidthci(font, ci);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_drawheight(font);	// number of rows
	return ubxfont_put_char16(db, xpix, ypix, font, charraster, UINT16_C(1) << width2, width2, height2, 0 /* (unused) */, fg);
}

// Для пропорциональных знакогенераторов
uint_fast16_t
ubpfont_render_char32(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	uint_fast16_t ci,
	COLORPIP_T fg
	)
{
	const uint32_t * const charraster = ubpfont_getcharraster32(font, ci);
	const uint_fast16_t width2 = font->font_drawwidthci(font, ci);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_drawheight(font);	// number of rows
	return ubxfont_put_char32(db, xpix, ypix, font, charraster, UINT32_C(1) << width2, width2, height2, 0 /* (unused) */, fg);
}

#endif /* ! GUI_EXTERNAL_FONTS */


#endif /* LCDMODE_LTDC */

