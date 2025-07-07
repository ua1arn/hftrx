/*******************************************************************************
 * Size: 8 px
 * Bpp: 2
 * Opts: --bpp 2 --size 8 --no-compress --font Rubik-Medium.ttf --range 32-127 --format lvgl -o Rubik_Medium_8_w2.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef RUBIK_MEDIUM_8_W2
#define RUBIK_MEDIUM_8_W2 1
#endif

#if RUBIK_MEDIUM_8_W2

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+0021 "!" */
    0x66, 0x66, 0x60,

    /* U+0022 "\"" */
    0xad, 0x60,

    /* U+0023 "#" */
    0x19, 0x47, 0xfc, 0x26, 0x7, 0xf8, 0x22, 0x0,

    /* U+0024 "$" */
    0xc, 0x1e, 0xdb, 0x0, 0xbc, 0x1, 0xde, 0xe0,
    0xc0,

    /* U+0025 "%" */
    0x68, 0x98, 0x94, 0x6a, 0x90, 0x96, 0x21, 0xa0,

    /* U+0026 "&" */
    0x2e, 0x3, 0x30, 0x3d, 0x99, 0x3c, 0x3e, 0xd0,

    /* U+0027 "'" */
    0x95,

    /* U+0028 "(" */
    0x29, 0x89, 0x24, 0x91, 0x82, 0x80,

    /* U+0029 ")" */
    0xa0, 0xc2, 0x45, 0x24, 0xca, 0x0,

    /* U+002A "*" */
    0x24, 0x7c, 0x14,

    /* U+002B "+" */
    0x0, 0x3, 0xb, 0xf4, 0x30, 0xc, 0x0,

    /* U+002C "," */
    0x59,

    /* U+002D "-" */
    0x7d,

    /* U+002E "." */
    0xa,

    /* U+002F "/" */
    0x1, 0x6, 0xc, 0x18, 0x30, 0x20, 0x90,

    /* U+0030 "0" */
    0x3f, 0x18, 0x6a, 0x1d, 0x86, 0x3f, 0x0,

    /* U+0031 "1" */
    0x2c, 0xac, 0xc, 0xc, 0xc,

    /* U+0032 "2" */
    0x3f, 0x14, 0xd0, 0x70, 0xa0, 0xbf, 0x40,

    /* U+0033 "3" */
    0x7f, 0x42, 0x81, 0xb5, 0xa, 0x3f, 0x0,

    /* U+0034 "4" */
    0xb, 0x6, 0xc7, 0x32, 0xff, 0x3, 0x0,

    /* U+0035 "5" */
    0x3f, 0x1f, 0x80, 0x2a, 0x4a, 0x3f, 0x0,

    /* U+0036 "6" */
    0xc, 0xb, 0x86, 0x2a, 0x8a, 0x3f, 0x0,

    /* U+0037 "7" */
    0xbf, 0x1, 0xc0, 0xd0, 0x70, 0x34, 0x0,

    /* U+0038 "8" */
    0x3f, 0x1c, 0xa3, 0xf6, 0x87, 0x3f, 0x40,

    /* U+0039 "9" */
    0x7f, 0x28, 0xa3, 0xf4, 0x38, 0x2c, 0x0,

    /* U+003A ":" */
    0x60, 0x6,

    /* U+003B ";" */
    0x70, 0x59,

    /* U+003C "<" */
    0x0, 0x1d, 0xb0, 0x38, 0x5,

    /* U+003D "=" */
    0x7f, 0x0, 0x7, 0xf0,

    /* U+003E ">" */
    0x0, 0x74, 0xe, 0x2c, 0x60,

    /* U+003F "?" */
    0x7e, 0x14, 0xc0, 0xd0, 0x20, 0x1c, 0x0,

    /* U+0040 "@" */
    0x1a, 0x91, 0xaa, 0x89, 0x89, 0x62, 0xa8, 0x60,
    0x10, 0x6a, 0x0,

    /* U+0041 "A" */
    0xe, 0x1, 0xb0, 0x32, 0x47, 0xfc, 0x90, 0xd0,

    /* U+0042 "B" */
    0x7f, 0x86, 0xc, 0x7f, 0x86, 0xc, 0x7f, 0x80,

    /* U+0043 "C" */
    0x2f, 0x46, 0xc, 0xa0, 0x6, 0xc, 0x2f, 0x40,

    /* U+0044 "D" */
    0x7f, 0x46, 0xc, 0x60, 0xc6, 0xc, 0x7f, 0x80,

    /* U+0045 "E" */
    0x7f, 0x58, 0x7, 0xf5, 0x80, 0x7f, 0x80,

    /* U+0046 "F" */
    0x7f, 0x58, 0x7, 0xf5, 0x80, 0x60, 0x0,

    /* U+0047 "G" */
    0x2f, 0x86, 0x8, 0xa3, 0xc6, 0xc, 0x2f, 0x40,

    /* U+0048 "H" */
    0x60, 0xc6, 0xc, 0x7f, 0xc6, 0xc, 0x60, 0xc0,

    /* U+0049 "I" */
    0x77, 0x77, 0x70,

    /* U+004A "J" */
    0x7f, 0x80, 0xa0, 0x2a, 0x4a, 0x3f, 0x0,

    /* U+004B "K" */
    0x63, 0x5f, 0x47, 0xc1, 0xe8, 0x62, 0x80,

    /* U+004C "L" */
    0x60, 0x1c, 0x7, 0x1, 0xc0, 0x7f, 0x40,

    /* U+004D "M" */
    0x70, 0x77, 0x8f, 0x6e, 0xb6, 0x63, 0x60, 0x30,

    /* U+004E "N" */
    0x70, 0xc7, 0x8c, 0x6d, 0xc6, 0x3c, 0x61, 0xc0,

    /* U+004F "O" */
    0x2f, 0x46, 0xc, 0xa0, 0xc6, 0xc, 0x2f, 0x40,

    /* U+0050 "P" */
    0x7f, 0x87, 0xc, 0x7f, 0x47, 0x0, 0x70, 0x0,

    /* U+0051 "Q" */
    0x2f, 0x46, 0xc, 0xa0, 0xc6, 0xc, 0x2f, 0x80,
    0x4,

    /* U+0052 "R" */
    0x7f, 0x46, 0x1c, 0x7f, 0x46, 0x34, 0x61, 0xc0,

    /* U+0053 "S" */
    0x3f, 0x28, 0x52, 0xa1, 0x47, 0x3f, 0x40,

    /* U+0054 "T" */
    0xbf, 0x83, 0x0, 0xc0, 0x30, 0xc, 0x0,

    /* U+0055 "U" */
    0x60, 0xc6, 0xc, 0x60, 0xc7, 0xc, 0x2f, 0x40,

    /* U+0056 "V" */
    0x90, 0xc7, 0x18, 0x33, 0x42, 0xb0, 0xe, 0x0,

    /* U+0057 "W" */
    0x90, 0x31, 0x9c, 0xc3, 0xba, 0xf, 0x78, 0x34,
    0xd0,

    /* U+0058 "X" */
    0x61, 0xc3, 0xb0, 0xe, 0x3, 0xb0, 0xa1, 0xc0,

    /* U+0059 "Y" */
    0xa0, 0xc3, 0x34, 0x1e, 0x0, 0xc0, 0xc, 0x0,

    /* U+005A "Z" */
    0xbf, 0x81, 0xc0, 0xc0, 0xd0, 0xbf, 0x80,

    /* U+005B "[" */
    0x75, 0x86, 0x18, 0x61, 0x87, 0x80,

    /* U+005C "\\" */
    0x40, 0x60, 0x30, 0x24, 0xc, 0x9, 0x2,

    /* U+005D "]" */
    0xb4, 0x92, 0x49, 0x24, 0x9b, 0x40,

    /* U+005E "^" */
    0x28,

    /* U+005F "_" */
    0x7f, 0xd0,

    /* U+0060 "`" */
    0x30,

    /* U+0061 "a" */
    0x3a, 0x5, 0xca, 0x31, 0xec,

    /* U+0062 "b" */
    0x60, 0x1f, 0xc6, 0x25, 0x89, 0x7f, 0x0,

    /* U+0063 "c" */
    0x3e, 0x28, 0x4a, 0x10, 0xf8,

    /* U+0064 "d" */
    0x2, 0x4f, 0xda, 0x36, 0x8d, 0x3f, 0x40,

    /* U+0065 "e" */
    0x3a, 0x2e, 0xca, 0x10, 0xf8,

    /* U+0066 "f" */
    0x3c, 0xbc, 0x30, 0x30, 0x30,

    /* U+0067 "g" */
    0x3f, 0x68, 0xda, 0x34, 0xfd, 0x53, 0x4b, 0x80,

    /* U+0068 "h" */
    0x60, 0x1f, 0xc6, 0x29, 0x8a, 0x62, 0x80,

    /* U+0069 "i" */
    0x66, 0x66, 0x60,

    /* U+006A "j" */
    0x18, 0x61, 0x86, 0x19, 0xd0,

    /* U+006B "k" */
    0x60, 0x19, 0xc7, 0x81, 0xf0, 0x67, 0x0,

    /* U+006C "l" */
    0x66, 0x66, 0x60,

    /* U+006D "m" */
    0x7f, 0xb5, 0x8c, 0x66, 0x31, 0x98, 0xc6,

    /* U+006E "n" */
    0x7b, 0x18, 0x96, 0x29, 0x89,

    /* U+006F "o" */
    0x3e, 0x28, 0xda, 0x34, 0xf8,

    /* U+0070 "p" */
    0x7b, 0x18, 0x96, 0x25, 0xfc, 0x60, 0x0,

    /* U+0071 "q" */
    0x3f, 0x68, 0xda, 0x34, 0xfd, 0x2, 0x40,

    /* U+0072 "r" */
    0x7c, 0x60, 0x60, 0x60,

    /* U+0073 "s" */
    0x7d, 0x74, 0xb, 0x7e,

    /* U+0074 "t" */
    0x30, 0xbc, 0x30, 0x30, 0x2c,

    /* U+0075 "u" */
    0xa2, 0x68, 0x96, 0x24, 0xfd,

    /* U+0076 "v" */
    0x93, 0x5c, 0xc3, 0x90, 0x70,

    /* U+0077 "w" */
    0x97, 0x31, 0xad, 0xc3, 0xde, 0xa, 0x34,

    /* U+0078 "x" */
    0x63, 0xb, 0x43, 0xd2, 0x8c,

    /* U+0079 "y" */
    0x93, 0x4d, 0xc2, 0xd0, 0x70, 0x24, 0x0,

    /* U+007A "z" */
    0x7f, 0xc, 0x34, 0xbe,

    /* U+007B "{" */
    0x1c, 0xc3, 0x28, 0x30, 0xc1, 0xc0,

    /* U+007C "|" */
    0x5, 0x55, 0x55, 0x55, 0x50,

    /* U+007D "}" */
    0xa0, 0x30, 0x24, 0x1c, 0x24, 0x30, 0xa0,

    /* U+007E "~" */
    0x7e
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 29, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 34, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 53, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 5, .adv_w = 88, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 13, .adv_w = 82, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 22, .adv_w = 100, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 30, .adv_w = 92, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 38, .adv_w = 29, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 39, .adv_w = 46, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 45, .adv_w = 46, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 51, .adv_w = 57, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 54, .adv_w = 79, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 61, .adv_w = 34, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 62, .adv_w = 62, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 63, .adv_w = 34, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 64, .adv_w = 65, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 71, .adv_w = 84, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 78, .adv_w = 59, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 83, .adv_w = 79, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 90, .adv_w = 81, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 97, .adv_w = 83, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 104, .adv_w = 79, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 111, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 118, .adv_w = 70, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 125, .adv_w = 84, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 132, .adv_w = 79, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 139, .adv_w = 35, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 141, .adv_w = 37, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 143, .adv_w = 65, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 148, .adv_w = 73, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 152, .adv_w = 65, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 157, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 164, .adv_w = 107, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 175, .adv_w = 89, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 183, .adv_w = 88, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 191, .adv_w = 88, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 199, .adv_w = 90, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 207, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 214, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 221, .adv_w = 90, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 229, .adv_w = 93, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 237, .adv_w = 38, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 240, .adv_w = 83, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 247, .adv_w = 81, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 254, .adv_w = 74, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 261, .adv_w = 104, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 269, .adv_w = 90, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 277, .adv_w = 89, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 285, .adv_w = 84, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 293, .adv_w = 89, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 302, .adv_w = 86, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 310, .adv_w = 82, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 317, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 324, .adv_w = 91, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 332, .adv_w = 86, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 340, .adv_w = 104, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 349, .adv_w = 85, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 357, .adv_w = 85, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 365, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 372, .adv_w = 45, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 378, .adv_w = 65, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 385, .adv_w = 45, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 391, .adv_w = 57, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 392, .adv_w = 95, .box_w = 6, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 394, .adv_w = 47, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 395, .adv_w = 73, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 400, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 407, .adv_w = 73, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 412, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 419, .adv_w = 74, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 424, .adv_w = 53, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 429, .adv_w = 79, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 437, .adv_w = 81, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 444, .adv_w = 34, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 447, .adv_w = 36, .box_w = 3, .box_h = 6, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 452, .adv_w = 70, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 459, .adv_w = 34, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 462, .adv_w = 115, .box_w = 7, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 469, .adv_w = 80, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 474, .adv_w = 76, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 479, .adv_w = 79, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 486, .adv_w = 79, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 493, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 497, .adv_w = 68, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 501, .adv_w = 55, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 506, .adv_w = 79, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 511, .adv_w = 73, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 516, .adv_w = 105, .box_w = 7, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 523, .adv_w = 72, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 528, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 535, .adv_w = 68, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 539, .adv_w = 51, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 545, .adv_w = 32, .box_w = 2, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 550, .adv_w = 51, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 557, .adv_w = 71, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = 2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    3, 7,
    3, 13,
    3, 14,
    3, 15,
    3, 16,
    3, 21,
    3, 23,
    3, 27,
    3, 28,
    3, 33,
    4, 24,
    7, 3,
    7, 8,
    8, 7,
    8, 13,
    8, 14,
    8, 15,
    8, 16,
    8, 21,
    8, 23,
    8, 27,
    8, 28,
    8, 33,
    9, 9,
    9, 17,
    9, 18,
    9, 19,
    9, 21,
    9, 22,
    9, 23,
    9, 25,
    9, 26,
    9, 92,
    10, 10,
    10, 62,
    10, 94,
    12, 18,
    12, 19,
    12, 20,
    12, 24,
    13, 3,
    13, 8,
    13, 14,
    13, 17,
    13, 18,
    13, 21,
    13, 23,
    13, 24,
    13, 25,
    13, 26,
    14, 3,
    14, 8,
    14, 18,
    14, 19,
    14, 20,
    14, 22,
    14, 24,
    14, 26,
    15, 3,
    15, 8,
    15, 14,
    15, 17,
    15, 18,
    15, 21,
    15, 23,
    15, 24,
    15, 25,
    15, 26,
    16, 16,
    16, 17,
    16, 18,
    16, 19,
    16, 21,
    16, 22,
    16, 23,
    16, 25,
    16, 26,
    17, 10,
    17, 13,
    17, 15,
    17, 16,
    17, 61,
    17, 62,
    17, 94,
    19, 10,
    19, 12,
    19, 14,
    19, 21,
    19, 61,
    19, 62,
    19, 94,
    20, 16,
    20, 18,
    20, 61,
    21, 3,
    21, 8,
    21, 10,
    21, 13,
    21, 15,
    21, 16,
    21, 18,
    21, 24,
    21, 26,
    21, 61,
    21, 62,
    21, 94,
    22, 13,
    22, 15,
    22, 16,
    22, 18,
    22, 61,
    23, 3,
    23, 8,
    23, 10,
    23, 16,
    23, 18,
    23, 24,
    23, 61,
    23, 62,
    23, 94,
    24, 4,
    24, 12,
    24, 13,
    24, 14,
    24, 15,
    24, 16,
    24, 21,
    24, 23,
    24, 30,
    25, 10,
    25, 13,
    25, 15,
    25, 16,
    25, 61,
    25, 62,
    25, 94,
    26, 10,
    26, 13,
    26, 14,
    26, 15,
    26, 16,
    26, 20,
    26, 21,
    26, 23,
    26, 61,
    26, 62,
    26, 94,
    27, 3,
    27, 8,
    28, 3,
    28, 8,
    30, 24,
    33, 3,
    33, 8,
    60, 9,
    60, 17,
    60, 18,
    60, 19,
    60, 21,
    60, 22,
    60, 23,
    60, 25,
    60, 26,
    60, 92,
    61, 3,
    61, 8,
    61, 17,
    61, 18,
    61, 20,
    61, 21,
    61, 22,
    61, 23,
    61, 24,
    61, 25,
    61, 26,
    61, 61,
    92, 9,
    92, 17,
    92, 18,
    92, 19,
    92, 21,
    92, 22,
    92, 23,
    92, 25,
    92, 26,
    92, 92,
    94, 10,
    94, 62,
    94, 94
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -2, -13, -6, -13, -8, -6, -4, -1,
    -1, -1, -1, -4, -4, -2, -13, -6,
    -13, -8, -6, -4, -1, -1, -1, -1,
    -3, -4, -2, -5, -2, -6, -3, -2,
    -3, -1, -1, -1, -3, -3, -1, -6,
    -13, -13, -5, -1, -8, -1, -1, -4,
    -1, -6, -6, -6, -4, -4, -2, -1,
    -7, -1, -13, -13, -5, -1, -8, -1,
    -1, -4, -1, -6, -28, -3, -1, -2,
    -7, -1, -7, -3, -1, -3, -1, -1,
    -3, -3, -3, -3, -1, -1, -2, -1,
    -2, -1, -1, -1, -2, -1, -3, -3,
    -3, -1, -1, -1, -3, -3, -1, -4,
    -3, -3, -1, -1, -1, -1, -1, -4,
    -4, -4, -1, -3, -4, -5, -4, -4,
    -1, -4, -8, -4, -8, -9, -4, -4,
    -1, -3, -1, -1, -1, -3, -3, -3,
    -3, -7, -1, -7, -7, -1, -1, -1,
    -2, -2, -2, -1, -1, -1, -1, -2,
    -1, -1, -1, -3, -4, -1, -4, -2,
    -4, -2, -2, -3, -8, -8, -3, -8,
    -1, -1, -1, -2, -3, -1, -5, -28,
    -1, -3, -4, -1, -4, -2, -5, -3,
    -2, -3, -3, -3, -3
};

/*Collect the kern pair's data in one place*/
static const lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 189,
    .glyph_ids_size = 0
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_pairs,
    .kern_scale = 16,
    .cmap_num = 1,
    .bpp = 2,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t Rubik_Medium_8_w2 = {
#else
lv_font_t Rubik_Medium_8_w2 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 10,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = 0,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if RUBIK_MEDIUM_8_W2*/

