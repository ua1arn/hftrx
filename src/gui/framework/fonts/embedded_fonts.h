#ifndef EMBEDDED_FONTS_H_INCLUDED
#define EMBEDDED_FONTS_H_INCLUDED

#include "../../gui_port_include.h"

#if WITHTOUCHGUI && ! GUI_EXTERNAL_FONTS

#include "../../gui_port.h"

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

void __gui_print_mono(const gui_drawbuf_t * gdb, uint16_t x, uint16_t y, const char * text, const gui_mono_font_t * font, gui_color_t color);
void __gui_print_prop(const gui_drawbuf_t * gdb, uint16_t x, uint16_t y, const char * text, const gui_prop_font_t * font, gui_color_t color);

void gui_print_mono(uint16_t x, uint16_t y, const char * text, const gui_mono_font_t * font, gui_color_t color);
void gui_print_prop(uint16_t x, uint16_t y, const char * text, const gui_prop_font_t * font, gui_color_t color);
uint16_t get_strwidth_mono(const char * str, const gui_mono_font_t * font);
uint16_t get_strwidth_prop(const char * str, const gui_prop_font_t * font);
uint16_t get_strheight_mono(const UB_Font32 * font);
uint16_t get_strheight_prop(const UB_pFont32 * font);

extern UB_pFont32 msgothic_10x13_prop;
extern UB_pFont32 msgothic_15x17_prop;
extern UB_pFont32 msgothic_13x16_prop;
extern UB_Font32 msgothic_15x17_mono;
extern UB_Font32 msgothic_11x13_mono;

#endif /* WITHTOUCHGUI && ! GUI_EXTERNAL_FONTS */

#endif /* EMBEDDED_FONTS_H_INCLUDED */
