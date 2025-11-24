#include "src/gui/gui_port_include.h"

#if WITHTOUCHGUI
#include "gui_fonts.h"
#include "gui_port.h"

//--------------------------------------------------------------
// Рисует ASCII символ шрифтом одного размера на позиции х, у.
// Цвет шрифта и фон (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
static void UB_Font_DrawChar32(const void * db, uint16_t x, uint16_t y, uint8_t ascii, const UB_Font32 * font, uint32_t vg)
{
	uint16_t xn, yn;
	uint32_t start_maske, maske;
	const uint32_t * wert;

	ascii -= 32;
	wert = & font->table[ascii * font->height];

	start_maske = 0x80;
	if (font->width > 8)  start_maske = 0x8000;
	if (font->width > 16) start_maske = 0x80000000;

	for (yn = 0; yn < font->height; yn ++)
	{
		maske = start_maske;
		// Установка курсора

		for (xn = 0; xn < font->width; xn ++)
		{
			if ((wert[yn] & maske)) __gui_draw_point((gui_drawbuf_t *) db, x + xn, yn + y, (gui_color_t) vg);

			maske = (maske >> 1);
		}
	}
}

//--------------------------------------------------------------
// Рисует строку шрифтом одного размера на позиции х, у.
// Цвет шрифта и фон (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void gui_UB_Font_DrawString32(const void * db, uint16_t x, uint16_t y, const char * ptr, const UB_Font32 * font, uint32_t vg)
{
	uint16_t pos;

	pos = x;
	while (* ptr != '\0') {
		UB_Font_DrawChar32(db, pos, y, * ptr, font, vg);
		pos += font->width;
		ptr ++;
	}
}

//--------------------------------------------------------------
// Рисование ASCII символ пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
// Возвращает: ширину нарисованного символа
//--------------------------------------------------------------
static uint16_t UB_Font_DrawPChar32(const void * db, uint16_t x, uint16_t y, uint8_t ascii, const UB_pFont32 * font, gui_color_t vg)
{
	uint16_t xn, yn, width;
	uint_fast32_t start_maske, maske;
	const uint32_t * wert;

	// Проверка границы символа
	if (ascii < font->first_char)
		return 0;

	if (ascii > font->last_char)
		return 0;

	ascii -= font->first_char;
	wert = &font->table[ascii * (font->height + 1)];
	width = wert[0];
	start_maske = 0x01;
	start_maske = start_maske << (width - 1);

	for (yn = 0; yn < font->height; yn ++)
	{
		maske = start_maske;
		// Установка курсора

		for (xn = 0; xn < width; xn++)
		{
			if ((wert[yn + 1] & maske)) __gui_draw_point((gui_drawbuf_t *) db, x + xn, yn + y, (gui_color_t) vg);

			maske = (maske >> 1);
		}
	}

	return (width);
}

//--------------------------------------------------------------
// Рисование строку пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void gui_UB_Font_DrawPString32(const void * db, uint16_t x, uint16_t y, const char * ptr, const UB_pFont32 * font, uint32_t vg)
{
	uint16_t pos = x, width;

	while (* ptr != 0)
	{
		width = UB_Font_DrawPChar32(db, pos, y, * ptr, font, vg);
		pos += width;
		ptr ++;
	}
}

static uint16_t UB_Font_getPcharw32(uint8_t ascii, const UB_pFont32 * font)
{
	uint16_t width;
	uint32_t start_maske, maske;
	const uint32_t * wert;

	// Проверка границы символа
	if (ascii < font->first_char)
		return 0;

	if (ascii > font->last_char)
		return 0;

	ascii -= font->first_char;
	wert = & font->table[ascii * (font->height + 1)];
	width = wert[0];

	return (width);
}

// Возврат ширины строки в пикселях, пропорциональный шрифт 32 бит
uint16_t gui_getwidth_Pstring32(const char * str, const UB_pFont32 * font)
{
	uint16_t width = 0;

	while (* str != 0)
	{
		width += UB_Font_getPcharw32(* str, font);
		str ++;
	}

	return width;
}

// Возвращает ширину строки в пикселях, моноширинный шрифт
uint16_t gui_getwidth_Mstring32(const char * str, const UB_Font32 * font) {
	ASSERT(str != NULL);
	return strlen(str) * font->width;
}

#endif /* WITHTOUCHGUI */
