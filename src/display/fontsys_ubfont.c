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
uint_fast8_t ubmfont_width(const unifont_t * font, uint_fast16_t ci)
{
	const UB_Font * const ub = (const UB_Font *) font->fontraster;
	(void) ci;
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
const void * ubmfont_getcharraster(const unifont_t * font, uint_fast16_t ci)
{
	const UB_Font * const ub = (const UB_Font *) font->fontraster;
	const uint16_t * const table = ub->table; // Таблица с данными
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
	const uint16_t * const charraster = (const uint16_t *) font->getcharrasterci(font, ci);
	const uint_fast16_t width2 = font->font_drawwidthci(font, ci);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_drawheight(font);	// number of rows
	//const uint_fast16_t bytesw = font->bytesw;	// bytes in each chargen row (unused)
//	PRINTF("ubmfont_render_char16: cc=%02X(%c),width2=%u\n", (unsigned char) cc, cc, width2);
//	printhex16(0, charraster, height2 * 2);
	return ubxfont_put_char16(db, xpix, ypix, font, charraster, UINT16_C(1) << 15, width2, height2, 0 /* (unused) */, fg);
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
const void * ubpfont_getcharraster(const unifont_t * font, uint_fast16_t ci)
{
	const UB_pFont * const ubp = (const UB_pFont *) font->fontraster;
	const uint16_t * const table = ubp->table; // Таблица с данными
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
	const uint16_t * const charraster = (const uint16_t *) font->getcharrasterci(font, ci);
	const uint_fast16_t width2 = font->font_drawwidthci(font, ci);	// number of bits (start from LSB first byte in raster)
	const uint_fast16_t height2 = font->font_drawheight(font);	// number of rows
	return ubxfont_put_char16(db, xpix, ypix, font, charraster, UINT16_C(1) << width2, width2, height2, 0 /* (unused) */, fg);
}



#endif /* LCDMODE_LTDC */

