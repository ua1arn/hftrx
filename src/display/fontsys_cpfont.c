/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"

#if LCDMODE_LTDC || WITHTOUCHGUI

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
static const uint8_t * cpfont_bitpacknormal_getcharraster(const struct unifont_tag * font, uint_fast16_t ci)
{
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	const uint_fast8_t bytespervertical = (cpfont_bitpacknormal_height(font) + 7) / 8;
	enum { PARAMSIZE = 6 };
	const uint_fast16_t paramstart = cpfont_bitpacknormal_paramstart(font, ci);	// параметры для данного символа
	const uint_fast16_t rastersstart = USBD_peek_u16(blob + paramstart + 4); //indexstart + totalci + totalci * PARAMSIZE;
	const uint_fast16_t width2 = blob [paramstart + 2];
	const uint_fast16_t height2 = font->font_drawheight(font);
	unsigned bytestotal = (width2 * height2 + 7) / 8;
//	PRINTF("cpfont_bitpacknormal_getcharraster: ci=%u, bytespervertical=%u, w2/h2=%u/%u\n", ci, bytespervertical, width2, height2);
//	printhex(paramstart, blob + paramstart, PARAMSIZE);
//	printhex(rastersstart, blob + rastersstart, bytestotal);
	return blob + rastersstart;
}

#if 1

//выводит символ Ch в позицию x, y шрифтом PtrFont, цветом Fcolor, на фоне цвета Bcolor
//возвращает ширину выведенного символа
//в отличии от стандартной функции  void TFT_Write_Char(unsigned int c, unsigned int x, unsigned int y)
//затирает ранее выведенные символы
//использует стандартные шрифты MIKROELEKTRONIKA
uint_fast16_t xTFT_PutChar(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char Ch, const uint8_t *PtrFont, COLORPIP_T fg)
{
	unsigned char first_ch, last_ch; //номера первого и последнего символа в массиве шрифта
	unsigned int ch_addr;             //адрес символа в массиве
	unsigned char height_ch;         //высота символа

	//загрузка параметров шрифта
	first_ch = PtrFont[2];            //номер первого символа
	last_ch = PtrFont[4];            //номер последнего символа
	height_ch = PtrFont[6];           //высота символа

	//проверяем есть ли шрифт для символа
	if ((Ch >= first_ch) || (Ch <= last_ch)) //если символ находится в диапазоне, то рисуем
			{
		unsigned int glyph_addr;          //адрес глифа для символа
		unsigned char i, k, p;
		static const unsigned char bit_mask [8] =
		{
				0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
				0x40, 0x80
		};
		unsigned char width_ch,          //ширина символа
				width_b;           //количество полных байт в ширине символа
		glyph_addr = (Ch - 32) * 4 + 8;     //адрес глифа символа

		ch_addr = (PtrFont[glyph_addr + 2] * 256 + PtrFont[glyph_addr + 1]); //адрес символа
		width_ch = PtrFont[glyph_addr + 0];                     //ширина символа


		if (width_ch > 8)
			height_ch = height_ch + PtrFont[glyph_addr + 0] + 2; //если ширина символа 2 байта
//		if (width_ch > 16)
//			height_ch = height_ch + PtrFont[glyph_addr + 0] + 2; //если ширина символа 3 байта
//		if (width_ch > 24)
//			height_ch = height_ch + PtrFont[glyph_addr + 0] + 2; //если ширина символа 4 байта
//		if (width_ch > 32)
//			height_ch = height_ch + PtrFont[glyph_addr + 0] + 2; //если ширина символа 5 байт
//		if (width_ch > 40)
//			height_ch = height_ch + PtrFont[glyph_addr + 0] + 2; //если ширина символа 6 байт
//		if (width_ch > 48)
//			height_ch = height_ch + PtrFont[glyph_addr + 0] + 2; //если ширина символа 7 байт
//		if (width_ch > 56)
//			height_ch = height_ch + PtrFont[glyph_addr + 0] + 2; //если ширина символа 8 байт
//		if (width_ch > 64)
//			height_ch = height_ch + PtrFont[glyph_addr + 0] + 2;

		for (i = 0; i <= height_ch - 1;) {
			for (k = 0; k <= width_ch - 1; k++) //выводим width_ch точек в строке
					{
				p = k & 0x07;
				if (k == 8)
					i = i + 1;                //если ширина два байта
				if (k == 16)
					i = i + 1;                //если ширина символа 3 байта
				if (k == 24)
					i = i + 1;                //если ширина символа 4 байта
				if (k == 32)
					i = i + 1;                //если ширина символа 5 байт
				if (k == 40)
					i = i + 1;                //если ширина символа 6 байт
				if (k == 48)
					i = i + 1;                //если ширина символа 7 байт
				if (k == 56)
					i = i + 1;                //если ширина символа 8 байт
				if (k == 64)
					i = i + 1;                //если ширина символа 8 байт

				if (PtrFont[ch_addr + i] & bit_mask[p]) //выводим байт по точкам
				{
					colpip_putpixel(db, xpix + k, ypix + i, fg); //зажигаем точку (цвет шрифта)
				}
			}
			i++;
		}
		return xpix + width_ch;
	}
	return xpix;

}
#endif

uint_fast16_t cpfont_bitpacknormal_draw(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, const struct unifont_tag * font, uint_fast16_t ci, COLORPIP_T fg)
{
	const uint8_t * const blob = (const uint8_t * const) font->fontraster;
	const uint_fast16_t paramstart = cpfont_bitpacknormal_paramstart(font, ci);	// параметры для данного символа
	const uint_fast16_t width2 = blob [paramstart + 2];
	const uint_fast16_t height2 = font->font_drawheight(font);
	const uint8_t * raster = cpfont_bitpacknormal_getcharraster(font, ci);
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
	.font_drawwidthci = cpfont_bitpacknormal_width,
	.font_drawheight = cpfont_bitpacknormal_height,
	.font_drawci = cpfont_bitpacknormal_draw,
	//
	.fontraster = Tahoma_Regular_88x77,
	.label = "Tahoma_Regular_88x77"
};

#endif /* LCDMODE_LTDC || WITHTOUCHGUI */
