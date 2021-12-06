#ifndef UB_FONTS_H_INCLUDED
#define UB_FONTS_H_INCLUDED

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "../display.h"

#if WITHALTERNATIVEFONTS

//--------------------------------------------------------------
// Структура пропорционального шрифта (не более 16 пикселей шириной)
//--------------------------------------------------------------
typedef struct UB_pFont_t {
  const uint16_t *table; // Таблица с данными
  uint16_t height;       // Высота символа (в пикселях)
  uint16_t first_char;   // Первый символ  (Ascii код)
  uint16_t last_char;    // Последний символ (Ascii код)
} UB_pFont;

//--------------------------------------------------------------
// Структура пропорционального шрифта (не более 32 пикселей шириной)
//--------------------------------------------------------------
typedef struct UB_pFont32_t {
  const uint32_t *table; // Таблица с данными
  uint16_t height;       // Высота символа (в пикселях)
  uint16_t first_char;   // Первый символ  (Ascii код)
  uint16_t last_char;    // Последний символ (Ascii код)
} UB_pFont32;

//--------------------------------------------------------------
// Рисование строки пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 16 пикселей в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawPString(PACKEDCOLORMAIN_T * __restrict buffer,
		uint16_t dx, uint16_t dy,
		uint16_t x, uint16_t y,
		const char * ptr, UB_pFont * font,
		COLORMAIN_T vg);

//--------------------------------------------------------------
// Рисование строку пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawPString32(PACKEDCOLORMAIN_T * __restrict buffer,
		uint16_t dx, uint16_t dy,
		uint16_t x, uint16_t y,
		const char * ptr, UB_pFont32 * font,
		COLORMAIN_T vg);

// Возврат ширины строки в пикселях, пропорциональный шрифт 32 бит
uint16_t getwidth_Pstring32(const char * str, UB_pFont32 * font);

// Возврат ширины строки в пикселях, пропорциональный шрифт меньше 32 бит
uint16_t getwidth_Pstring(const char * str, UB_pFont * font);

extern UB_pFont gothic_12x16;

#endif /* WITHALTERNATIVEFONTS */
#endif /* UB_FONTS_H_INCLUDED */
