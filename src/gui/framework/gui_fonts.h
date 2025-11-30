#ifndef GUI_FONTS_H_INCLUDED
#define GUI_FONTS_H_INCLUDED

#include "../gui_port_include.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if WITHTOUCHGUI

// моноширинные шрифты
typedef struct UB_Font32_t {
  const uint32_t *table; // Таблица с данными
  uint16_t width;        // Ширина символа (в пикселях)
  uint16_t height;       // Высота символа (в пикселях)
} UB_Font32;

// пропорциональные шрифты
typedef struct UB_pFont32_t {
  const uint32_t *table; // Таблица с данными
  uint16_t height;       // Высота символа (в пикселях)
  uint16_t first_char;   // Первый символ  (Ascii код)
  uint16_t last_char;    // Последний символ (Ascii код)
} UB_pFont32;

typedef UB_Font32	gui_mono_font_t;
typedef UB_pFont32	gui_prop_font_t;

void gui_UB_Font_DrawString32(const void * db, uint16_t x, uint16_t y, const char * ptr, const UB_Font32 * font, uint32_t vg);
void gui_UB_Font_DrawPString32(const void * db, uint16_t x, uint16_t y, const char * ptr, const UB_pFont32 * font, uint32_t vg);

extern UB_pFont32 msgothic_10x13_prop;
extern UB_pFont32 msgothic_15x17_prop;
extern UB_pFont32 msgothic_13x16_prop;
extern UB_Font32 msgothic_15x17_mono;
extern UB_Font32 msgothic_11x13_mono;

#endif /* WITHTOUCHGUI */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GUI_FONTS_H_INCLUDED */
