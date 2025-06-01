#ifndef UB_FONTS_H_INCLUDED
#define UB_FONTS_H_INCLUDED

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "../display.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if WITHALTERNATIVEFONTS

//--------------------------------------------------------------
// Структура шрифта одного размера (не более 16 пикселей шириной)
//--------------------------------------------------------------
typedef struct UB_Font_t {
  const uint16_t *table; // Таблица с данными
  uint16_t width;        // Ширина символа (в пикселях)
  uint16_t height;       // Высота символа (в пикселях)
}UB_Font;


//--------------------------------------------------------------
// Структура шрифта одного размера (неболее 32 пикселей шириной)
//--------------------------------------------------------------
typedef struct UB_Font32_t {
  const uint32_t *table; // Таблица с данными
  uint16_t width;        // Ширина символа (в пикселях)
  uint16_t height;       // Высота символа (в пикселях)
}UB_Font32;

//--------------------------------------------------------------
// Структура пропорционального шрифта (не более 16 пикселей шириной)
//--------------------------------------------------------------
typedef struct UB_pFont_t {
  const uint16_t *table; // Таблица с данными
  uint16_t height;       // Высота символа (в пикселях)
  uint16_t first_char;   // Первый символ  (Ascii код)
  uint16_t last_char;    // Последний символ (Ascii код)
}UB_pFont;

//--------------------------------------------------------------
// Рисует строку шрифтом одного размера на позиции х, у.
// Цвет шрифта и фон (шрифт = макс 16 пикселей в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawString(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, const char *ptr, const UB_Font *font, COLORPIP_T vg);

//--------------------------------------------------------------
// Рисует строку шрифтом одного размера на позиции х, у.
// Цвет шрифта и фон (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawString32(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, const char *ptr, const UB_Font32 *font, COLORPIP_T vg);


//--------------------------------------------------------------
// Структура пропорционального шрифта (не более 32 пикселей шириной)
//--------------------------------------------------------------
typedef struct UB_pFont32_t {
  const uint32_t *table; // Таблица с данными
  uint16_t height;       // Высота символа (в пикселях)
  uint16_t first_char;   // Первый символ  (Ascii код)
  uint16_t last_char;    // Последний символ (Ascii код)
}UB_pFont32;


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
		COLORPIP_T vg);

#define UB_Font_DrawPString(...) do { \
	UB_Font_DrawPStringDbg(__FILE__, __LINE__, __VA_ARGS__); \
	} while (0)

//--------------------------------------------------------------
// Рисование строку пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawPString32(const gxdrawb_t * db,
		uint_fast16_t x, uint_fast16_t y,
		const char * ptr, const UB_pFont32 * font,
		COLORPIP_T vg);

// Возврат ширины строки в пикселях, пропорциональный шрифт 32 бит
uint16_t getwidth_Pstring32(const char * str, const UB_pFont32 * font);

// Возврат ширины строки в пикселях, пропорциональный шрифт меньше 32 бит
uint16_t getwidth_Pstring(const char * str, const UB_pFont * font);

extern UB_pFont gothic_12x16_p;
extern UB_Font gothic_11x13;

#endif /* WITHALTERNATIVEFONTS */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UB_FONTS_H_INCLUDED */
