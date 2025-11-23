#ifndef GUI_FONTS_H_INCLUDED
#define GUI_FONTS_H_INCLUDED

#include "src/gui/gui_port_include.h"

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

void gui_UB_Font_DrawString32(const void * db, uint16_t x, uint16_t y, const char * ptr, const UB_Font32 * font, uint32_t vg);
void gui_UB_Font_DrawPString32(const void * db, uint16_t x, uint16_t y, const char * ptr, const UB_pFont32 * font, uint32_t vg);

uint16_t gui_getwidth_Pstring32(const char * str, const UB_pFont32 * font);
uint16_t gui_getwidth_Mstring32(const char * str, const UB_Font32 * font);

extern UB_Font32 arial_10x12_mono;
extern UB_pFont32 arial_10x12_prop;
extern UB_Font32 arial_14x13_mono;
extern UB_pFont32 arial_14x13_prop;
extern UB_Font32 arial_18x16_mono;
extern UB_pFont32 arial_18x16_prop;
extern UB_pFont32 msgothic_15x17_prop;
extern UB_pFont32 msgothic_13x16_prop;

#endif /* WITHTOUCHGUI */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GUI_FONTS_H_INCLUDED */
