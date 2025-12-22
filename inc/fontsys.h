/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef INC_FONTSYS_H_
#define INC_FONTSYS_H_


#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#include "src/display/display.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if GUI_EXTERNAL_FONTS

//typedef void /* struct gxdrawb_tag */ gxdrawb_t;
typedef void /* struct unifont_tag */ unifont_t;

#else /* GUI_EXTERNAL_FONTS */
//--------------------------------------------------------------
// Структура шрифта одного размера (не более 16 пикселей шириной)
//--------------------------------------------------------------
typedef struct UB_Font_t {
  const uint16_t *table; // Таблица с данными
  uint16_t width;        // Ширина символа (в пикселях)
  uint16_t height;       // Высота символа (в пикселях)
}UB_Font;


//--------------------------------------------------------------
// Структура шрифта одного размера (не более 32 пикселей шириной)
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
// Структура пропорционального шрифта (не более 32 пикселей шириной)
//--------------------------------------------------------------
typedef struct UB_pFont32_t {
  const uint32_t *table; // Таблица с данными
  uint16_t height;       // Высота символа (в пикселях)
  uint16_t first_char;   // Первый символ  (Ascii код)
  uint16_t last_char;    // Последний символ (Ascii код)
}UB_pFont32;

// marked for delete
#if 0

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

// Возвращает ширину строки в пикселях, моноширинный шрифт
uint16_t getwidth_Mstring(const char * str, const UB_Font * font);
#endif


// Поддержка шрифтов Adafruit-GFX-Library
//	https://github.com/adafruit/Adafruit-GFX-Library.git

typedef struct hftrx_GFXglyph_tag
{
	uint16_t bitmapOffset; ///< Pointer into GFXfont->bitmap
	uint8_t width;         ///< Bitmap dimensions in pixels
	uint8_t height;        ///< Bitmap dimensions in pixels
	uint8_t xAdvance;      ///< Distance to advance cursor (x axis)
	int8_t xOffset;        ///< X dist from cursor pos to UL corner
	int8_t yOffset;        ///< Y dist from cursor pos to UL corner
} hftrx_GFXglyph_t;

typedef struct hftrx_GFXfont_tag
{
	uint8_t *bitmap;  ///< Glyph bitmaps, concatenated
	hftrx_GFXglyph_t *glyph;  ///< Glyph array
	uint16_t first;   ///< ASCII extents (first char)
	uint16_t last;    ///< ASCII extents (last char)
	uint8_t yAdvance; ///< Newline distance (y axis)
} hftrx_GFXfont_t;


// Готовые шрифты
extern UB_pFont gothic_12x16_p;
extern UB_Font gothic_11x13;

typedef struct gxdrawb_tag gxdrawb_t;

typedef struct unifont_tag
{
	uint_fast16_t (* decode)(const struct unifont_tag * font, char cc);	// получение ci
	uint_fast16_t (* totalci)(const struct unifont_tag * font);
	const void * (* getcharrasterci)(const struct unifont_tag * font, uint_fast16_t ci);	// получение начального адреса растра для символа
	uint_fast8_t (* font_drawwidthci)(const struct unifont_tag * font, uint_fast16_t ci);	// ширина в пиксеях данного символа (может быть меньше чем поле width)
	uint_fast8_t (* font_drawheight)(const struct unifont_tag * font);	// высота в пикселях (се символы шрифта одной высоты)
	uint_fast16_t (* font_drawci)(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, const struct unifont_tag * font, uint_fast16_t ci, COLORPIP_T fg);
	const void * fontraster;		// начало знакогенератора в памяти
	void * fontdata;			// например, для adafruint требуется место с предрасчитаной высотой шрифта и смещением baseline
	const char * label;		// название для диагностики
} unifont_t;

#define TEZXTSZIE_AUTO UINT16_MAX

// печать строки
uint_fast16_t
unifont_text(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const unifont_t * font,
	const char * s,
	size_t slength,		// количество символов для печати или TEZXTSZIE_AUTO - если до '\0'
	COLORPIP_T fg		// цвет вывода текста
	);

// получить оба размера текстовой строки
uint_fast16_t
unifont_textsize(
	const unifont_t * font,
	const char * s,
	size_t slength,		// количество символов для печати или TEZXTSZIE_AUTO - если до '\0'
	uint_fast16_t * height
	);

// Функции (поля unifont_t) для работы с UB_pFont и UB_Font

// Для моноширинных знакогенераторов
uint_fast8_t ubmfont_width(const struct unifont_tag * font, uint_fast16_t ci);
uint_fast8_t ubmfont_width32(const struct unifont_tag * font, uint_fast16_t ci);
uint_fast8_t ubmfont_height(const unifont_t * font);
uint_fast8_t ubmfont_height32(const unifont_t * font);
uint_fast16_t ubmfont_decode(const unifont_t * font, char cc);
uint_fast16_t ubmfont_decode32(const unifont_t * font, char cc);
uint_fast16_t ubmfont_totalci(const unifont_t * font);
uint_fast16_t ubmfont_totalci32(const unifont_t * font);
const void * ubmfont_getcharraster16(const unifont_t * font, uint_fast16_t ci);
const void * ubmfont_getcharraster32(const unifont_t * font, uint_fast16_t ci);
uint_fast16_t ubmfont_render_char16(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	uint_fast16_t ci,
	COLORPIP_T fg
	);
uint_fast16_t ubmfont_render_char32(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	uint_fast16_t ci,
	COLORPIP_T fg
	);

// Для пропорциональных знакогенераторов
uint_fast8_t ubpfont_width(const struct unifont_tag * font, uint_fast16_t ci);
uint_fast8_t ubpfont_width32(const struct unifont_tag * font, uint_fast16_t ci);
uint_fast8_t ubpfont_height(const unifont_t * font);
uint_fast8_t ubpfont_height32(const unifont_t * font);
uint_fast16_t ubpfont_decode(const unifont_t * font, char cc);
uint_fast16_t ubpfont_decode32(const unifont_t * font, char cc);
uint_fast16_t ubpfont_totalci(const unifont_t * font);
uint_fast16_t ubpfont_totalci32(const unifont_t * font);
const void * ubpfont_getcharraster16(const struct unifont_tag * font, uint_fast16_t ci);
const void * ubpfont_getcharraster32(const struct unifont_tag * font, uint_fast16_t ci);
uint_fast16_t ubpfont_render_char16(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	uint_fast16_t ci,
	COLORPIP_T fg
	);
uint_fast16_t ubpfont_render_char32(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	uint_fast16_t ci,
	COLORPIP_T fg
	);

extern const unifont_t unifont_big_raw;	// non-cached version
extern const unifont_t unifont_half_raw;	// non-cached version
extern const unifont_t unifont_big;
extern const unifont_t unifont_half;
extern const unifont_t unifont_small;
//extern const unifont_t unifont_small_x2;
extern const unifont_t unifont_small2;
extern const unifont_t unifont_small3;
extern const unifont_t unifont_gothic_11x13;
extern const unifont_t unifont_gothic_12x16p;	// proportional
extern const unifont_t unifont_Tahoma_Regular_88x77;
extern const unifont_t unifont_roboto32;		// aptech
extern const unifont_t unifont_helvNeueTh70;	// aptech
extern const unifont_t unifont_FreeMono9pt7b;	// Adafruit-GFX-Library
extern const unifont_t unifont_FreeMono12pt7b;	// Adafruit-GFX-Library
extern const unifont_t unifont_FreeMono18pt7b;	// Adafruit-GFX-Library
extern const unifont_t unifont_FreeMono24pt7b;	// Adafruit-GFX-Library
extern const unifont_t unifont_FreeSans12pt7b;	// Adafruit-GFX-Library

extern const unifont_t unifont_msgothic_10x13_prop;
extern const unifont_t unifont_msgothic_11x13_mono;
extern const unifont_t unifont_msgothic_13x16_prop;
extern const unifont_t unifont_msgothic_15x17_prop;
extern const unifont_t unifont_msgothic_15x17_mono;

// todo: заменить на функции получения размера от шрифта
#define SMALLCHARH 15 /* Font height */
#define SMALLCHARW 16 /* Font width */
#define SMALLCHARH2 16 /* Font height */
#define SMALLCHARW2 10 /* Font width */

#endif /* GUI_EXTERNAL_FONTS */



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* INC_FONTSYS_H_ */
