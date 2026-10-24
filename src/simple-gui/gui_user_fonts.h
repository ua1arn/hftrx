#include "gui_port_include.h"
#include "gui_port.h"
#include "fontsys.h"

typedef unifont_t gui_font_t;

extern const unifont_t unifont_gothic_12x16p;
#define BUTTONS_FONTP_DEFAULT unifont_gothic_12x16p

extern const unifont_t unifont_gothic_11x13;
#define LABELS_FONT_DEFAULT unifont_gothic_11x13

#define BUTTONS_FONTP_DEFAULT	unifont_gothic_12x16p
#define LABELS_FONT_DEFAULT		unifont_gothic_11x13
#define INFOBAR_FONTP			unifont_gothic_12x16p
#define WINDOW_TITLE_FONTP		unifont_gothic_12x16p

static inline void __gui_print_text(const gui_drawbuf_t * gdb, uint16_t x, uint16_t y,
                      const char* text, const gui_font_t * font, gui_color_t color)
{
    if (! text || ! font || ! gdb) return;

    unifont_text(gdb, x, y, font, text, TEXTSIZE_AUTO, color);
}

// Получение ширины строки в пикселях
static inline uint16_t get_strwidth(const char * str, const gui_font_t * font)
{
    if (! str || ! font) return 0;

    uint_fast16_t width = 0;
    uint_fast16_t height = 0;

    width = unifont_textsize(font, str, TEXTSIZE_AUTO, & height);

    return width;
}

// Получение высоты шрифта
static inline uint16_t get_strheight(const unifont_t * font)
{
    if (! font || ! font->font_drawheight) return 0;

    return font->font_drawheight(font);
}
