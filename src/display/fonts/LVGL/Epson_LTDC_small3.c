#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LV_FONT_EPSON_LTDC_SMALL3
#define LV_FONT_EPSON_LTDC_SMALL3 1
#endif

#if LV_FONT_EPSON_LTDC_SMALL3

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
	/* ch offset = 0, code = 0x20 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	/* ch offset = 8, code = 0x21 */
	0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00, 
	/* ch offset = 16, code = 0x22 */
	0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 
	/* ch offset = 24, code = 0x23 */
	0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x6C, 0x6C, 0x00, 
	/* ch offset = 32, code = 0x24 */
	0x18, 0x7E, 0xC0, 0x7C, 0x06, 0xFC, 0x18, 0x00, 
	/* ch offset = 40, code = 0x25 */
	0x00, 0xC6, 0x0C, 0x18, 0x30, 0x60, 0xC6, 0x00, 
	/* ch offset = 48, code = 0x26 */
	0x38, 0x6C, 0x38, 0x76, 0xCC, 0xCC, 0x76, 0x00, 
	/* ch offset = 56, code = 0x27 */
	0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 
	/* ch offset = 64, code = 0x28 */
	0x18, 0x30, 0x60, 0x60, 0x60, 0x30, 0x18, 0x00, 
	/* ch offset = 72, code = 0x29 */
	0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60, 0x00, 
	/* ch offset = 80, code = 0x2A */
	0x00, 0xEE, 0x7C, 0xFE, 0x7C, 0xEE, 0x00, 0x00, 
	/* ch offset = 88, code = 0x2B */
	0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00, 
	/* ch offset = 96, code = 0x2C */
	0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30, 0x00, 
	/* ch offset = 104, code = 0x2D */
	0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 
	/* ch offset = 112, code = 0x2E */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 
	/* ch offset = 120, code = 0x2F */
	0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x00, 
	/* ch offset = 128, code = 0x30 */
	0x7C, 0xC6, 0xCE, 0xDE, 0xF6, 0xE6, 0x7C, 0x00, 
	/* ch offset = 136, code = 0x31 */
	0x18, 0x78, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00, 
	/* ch offset = 144, code = 0x32 */
	0x7C, 0xC6, 0x0C, 0x18, 0x30, 0x66, 0xFE, 0x00, 
	/* ch offset = 152, code = 0x33 */
	0x7C, 0xC6, 0x06, 0x3C, 0x06, 0xC6, 0x7C, 0x00, 
	/* ch offset = 160, code = 0x34 */
	0x0C, 0x1C, 0x3C, 0x6C, 0xFE, 0x0C, 0x0C, 0x00, 
	/* ch offset = 168, code = 0x35 */
	0xFE, 0xC0, 0xFC, 0x06, 0x06, 0xC6, 0x7C, 0x00, 
	/* ch offset = 176, code = 0x36 */
	0x7C, 0xC6, 0xC0, 0xFC, 0xC6, 0xC6, 0x7C, 0x00, 
	/* ch offset = 184, code = 0x37 */
	0xFE, 0xC6, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x00, 
	/* ch offset = 192, code = 0x38 */
	0x7C, 0xC6, 0xC6, 0x7C, 0xC6, 0xC6, 0x7C, 0x00, 
	/* ch offset = 200, code = 0x39 */
	0x7C, 0xC6, 0xC6, 0x7E, 0x06, 0xC6, 0x7C, 0x00, 
	/* ch offset = 208, code = 0x3A */
	0x00, 0x1C, 0x1C, 0x00, 0x00, 0x1C, 0x1C, 0x00, 
	/* ch offset = 216, code = 0x3B */
	0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x30, 
	/* ch offset = 224, code = 0x3C */
	0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x00, 
	/* ch offset = 232, code = 0x3D */
	0x00, 0x00, 0xFE, 0x00, 0x00, 0xFE, 0x00, 0x00, 
	/* ch offset = 240, code = 0x3E */
	0x60, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x60, 0x00, 
	/* ch offset = 248, code = 0x3F */
	0x7C, 0xC6, 0x06, 0x0C, 0x18, 0x00, 0x18, 0x00, 
	/* ch offset = 256, code = 0x40 */
	0x7C, 0xC6, 0xC6, 0xDE, 0xDC, 0xC0, 0x7E, 0x00, 
	/* ch offset = 264, code = 0x41 */
	0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0x00, 
	/* ch offset = 272, code = 0x42 */
	0xFC, 0x66, 0x66, 0x7C, 0x66, 0x66, 0xFC, 0x00, 
	/* ch offset = 280, code = 0x43 */
	0x3C, 0x66, 0xC0, 0xC0, 0xC0, 0x66, 0x3C, 0x00, 
	/* ch offset = 288, code = 0x44 */
	0xF8, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0xF8, 0x00, 
	/* ch offset = 296, code = 0x45 */
	0xFE, 0xC2, 0xC0, 0xF8, 0xC0, 0xC2, 0xFE, 0x00, 
	/* ch offset = 304, code = 0x46 */
	0xFE, 0x62, 0x60, 0x7C, 0x60, 0x60, 0xF0, 0x00, 
	/* ch offset = 312, code = 0x47 */
	0x7C, 0xC6, 0xC0, 0xC0, 0xDE, 0xC6, 0x7C, 0x00, 
	/* ch offset = 320, code = 0x48 */
	0xC6, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0xC6, 0x00, 
	/* ch offset = 328, code = 0x49 */
	0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 
	/* ch offset = 336, code = 0x4A */
	0x3C, 0x18, 0x18, 0x18, 0xD8, 0xD8, 0x70, 0x00, 
	/* ch offset = 344, code = 0x4B */
	0xC6, 0xCC, 0xD8, 0xF0, 0xD8, 0xCC, 0xC6, 0x00, 
	/* ch offset = 352, code = 0x4C */
	0xF0, 0x60, 0x60, 0x60, 0x60, 0x62, 0xFE, 0x00, 
	/* ch offset = 360, code = 0x4D */
	0xC6, 0xEE, 0xFE, 0xD6, 0xD6, 0xC6, 0xC6, 0x00, 
	/* ch offset = 368, code = 0x4E */
	0xC6, 0xE6, 0xE6, 0xF6, 0xDE, 0xCE, 0xC6, 0x00, 
	/* ch offset = 376, code = 0x4F */
	0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00, 
	/* ch offset = 384, code = 0x50 */
	0xFC, 0x66, 0x66, 0x7C, 0x60, 0x60, 0xF0, 0x00, 
	/* ch offset = 392, code = 0x51 */
	0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0xD6, 0x7C, 0x06, 
	/* ch offset = 400, code = 0x52 */
	0xFC, 0xC6, 0xC6, 0xFC, 0xD8, 0xCC, 0xC6, 0x00, 
	/* ch offset = 408, code = 0x53 */
	0x7C, 0xC6, 0xC0, 0x7C, 0x06, 0xC6, 0x7C, 0x00, 
	/* ch offset = 416, code = 0x54 */
	0x7E, 0x5A, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 
	/* ch offset = 424, code = 0x55 */
	0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00, 
	/* ch offset = 432, code = 0x56 */
	0xC6, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x10, 0x00, 
	/* ch offset = 440, code = 0x57 */
	0xC6, 0xC6, 0xD6, 0xD6, 0xFE, 0xEE, 0xC6, 0x00, 
	/* ch offset = 448, code = 0x58 */
	0xC6, 0x6C, 0x38, 0x38, 0x38, 0x6C, 0xC6, 0x00, 
	/* ch offset = 456, code = 0x59 */
	0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x3C, 0x00, 
	/* ch offset = 464, code = 0x5A */
	0xFE, 0x86, 0x0C, 0x18, 0x30, 0x62, 0xFE, 0x00, 
	/* ch offset = 472, code = 0x5B */
	0x7C, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7C, 0x00, 
	/* ch offset = 480, code = 0x5C */
	0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x02, 0x00, 
	/* ch offset = 488, code = 0x5D */
	0x7C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x7C, 0x00, 
	/* ch offset = 496, code = 0x5E */
	0x10, 0x38, 0x6C, 0xC6, 0x00, 0x00, 0x00, 0x00, 
	/* ch offset = 504, code = 0x5F */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 
	/* ch offset = 512, code = 0x60 */
	0x30, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
	/* ch offset = 520, code = 0x61 */
	0x00, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0x7E, 0x00, 
	/* ch offset = 528, code = 0x62 */
	0xE0, 0x60, 0x7C, 0x66, 0x66, 0x66, 0xFC, 0x00, 
	/* ch offset = 536, code = 0x63 */
	0x00, 0x00, 0x7C, 0xC6, 0xC0, 0xC6, 0x7C, 0x00, 
	/* ch offset = 544, code = 0x64 */
	0x1C, 0x0C, 0x7C, 0xCC, 0xCC, 0xCC, 0x7E, 0x00, 
	/* ch offset = 552, code = 0x65 */
	0x00, 0x00, 0x7C, 0xC6, 0xFE, 0xC0, 0x7C, 0x00, 
	/* ch offset = 560, code = 0x66 */
	0x1C, 0x36, 0x30, 0xFC, 0x30, 0x30, 0x78, 0x00, 
	/* ch offset = 568, code = 0x67 */
	0x00, 0x00, 0x76, 0xCE, 0xC6, 0x7E, 0x06, 0x7C, 
	/* ch offset = 576, code = 0x68 */
	0xE0, 0x60, 0x7C, 0x66, 0x66, 0x66, 0xE6, 0x00, 
	/* ch offset = 584, code = 0x69 */
	0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x00, 
	/* ch offset = 592, code = 0x6A */
	0x0C, 0x00, 0x1C, 0x0C, 0x0C, 0x0C, 0xCC, 0x78, 
	/* ch offset = 600, code = 0x6B */
	0xE0, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0xE6, 0x00, 
	/* ch offset = 608, code = 0x6C */
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1C, 0x00, 
	/* ch offset = 616, code = 0x6D */
	0x00, 0x00, 0x6C, 0xFE, 0xD6, 0xD6, 0xC6, 0x00, 
	/* ch offset = 624, code = 0x6E */
	0x00, 0x00, 0xDC, 0x66, 0x66, 0x66, 0x66, 0x00, 
	/* ch offset = 632, code = 0x6F */
	0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0x7C, 0x00, 
	/* ch offset = 640, code = 0x70 */
	0x00, 0x00, 0xDC, 0x66, 0x66, 0x7C, 0x60, 0xF0, 
	/* ch offset = 648, code = 0x71 */
	0x00, 0x00, 0x76, 0xCC, 0xCC, 0x7C, 0x0C, 0x1E, 
	/* ch offset = 656, code = 0x72 */
	0x00, 0x00, 0xDC, 0x66, 0x60, 0x60, 0xF0, 0x00, 
	/* ch offset = 664, code = 0x73 */
	0x00, 0x00, 0x7C, 0xC0, 0x7C, 0x06, 0x7C, 0x00, 
	/* ch offset = 672, code = 0x74 */
	0x30, 0x30, 0xFC, 0x30, 0x30, 0x36, 0x1C, 0x00, 
	/* ch offset = 680, code = 0x75 */
	0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0x76, 0x00, 
	/* ch offset = 688, code = 0x76 */
	0x00, 0x00, 0xC6, 0xC6, 0x6C, 0x38, 0x10, 0x00, 
	/* ch offset = 696, code = 0x77 */
	0x00, 0x00, 0xC6, 0xC6, 0xD6, 0xFE, 0x6C, 0x00, 
	/* ch offset = 704, code = 0x78 */
	0x00, 0x00, 0xC6, 0x6C, 0x38, 0x6C, 0xC6, 0x00, 
	/* ch offset = 712, code = 0x79 */
	0x00, 0x00, 0xC6, 0xC6, 0xCE, 0x76, 0x06, 0x7C, 
	/* ch offset = 720, code = 0x7A */
	0x00, 0x00, 0xFC, 0x98, 0x30, 0x64, 0xFC, 0x00, 
	/* ch offset = 728, code = 0x7B */
	0x0E, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0E, 0x00, 
	/* ch offset = 736, code = 0x7C */
	0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00, 
	/* ch offset = 744, code = 0x7D */
	0x70, 0x18, 0x18, 0x0E, 0x18, 0x18, 0x70, 0x00, 
	/* ch offset = 752, code = 0x7E */
	0x76, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	/* ch offset = 760, code = 0x7F */
	0x00, 0x10, 0x38, 0x38, 0x6C, 0x6C, 0xFE, 0x00, 
};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
	{.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
	{.bitmap_index = 0, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch=' ' */
	{.bitmap_index = 8, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='!' */
	{.bitmap_index = 16, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='"' */
	{.bitmap_index = 24, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='#' */
	{.bitmap_index = 32, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='$' */
	{.bitmap_index = 40, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='%' */
	{.bitmap_index = 48, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='&' */
	{.bitmap_index = 56, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch=''' */
	{.bitmap_index = 64, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='(' */
	{.bitmap_index = 72, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch=')' */
	{.bitmap_index = 80, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='*' */
	{.bitmap_index = 88, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='+' */
	{.bitmap_index = 96, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch=',' */
	{.bitmap_index = 104, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='-' */
	{.bitmap_index = 112, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='.' */
	{.bitmap_index = 120, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='/' */
	{.bitmap_index = 128, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='0' */
	{.bitmap_index = 136, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='1' */
	{.bitmap_index = 144, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='2' */
	{.bitmap_index = 152, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='3' */
	{.bitmap_index = 160, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='4' */
	{.bitmap_index = 168, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='5' */
	{.bitmap_index = 176, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='6' */
	{.bitmap_index = 184, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='7' */
	{.bitmap_index = 192, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='8' */
	{.bitmap_index = 200, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='9' */
	{.bitmap_index = 208, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch=':' */
	{.bitmap_index = 216, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch=';' */
	{.bitmap_index = 224, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='<' */
	{.bitmap_index = 232, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='=' */
	{.bitmap_index = 240, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='>' */
	{.bitmap_index = 248, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='?' */
	{.bitmap_index = 256, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='@' */
	{.bitmap_index = 264, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='A' */
	{.bitmap_index = 272, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='B' */
	{.bitmap_index = 280, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='C' */
	{.bitmap_index = 288, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='D' */
	{.bitmap_index = 296, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='E' */
	{.bitmap_index = 304, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='F' */
	{.bitmap_index = 312, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='G' */
	{.bitmap_index = 320, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='H' */
	{.bitmap_index = 328, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='I' */
	{.bitmap_index = 336, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='J' */
	{.bitmap_index = 344, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='K' */
	{.bitmap_index = 352, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='L' */
	{.bitmap_index = 360, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='M' */
	{.bitmap_index = 368, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='N' */
	{.bitmap_index = 376, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='O' */
	{.bitmap_index = 384, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='P' */
	{.bitmap_index = 392, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='Q' */
	{.bitmap_index = 400, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='R' */
	{.bitmap_index = 408, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='S' */
	{.bitmap_index = 416, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='T' */
	{.bitmap_index = 424, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='U' */
	{.bitmap_index = 432, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='V' */
	{.bitmap_index = 440, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='W' */
	{.bitmap_index = 448, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='X' */
	{.bitmap_index = 456, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='Y' */
	{.bitmap_index = 464, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='Z' */
	{.bitmap_index = 472, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='[' */
	{.bitmap_index = 480, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='\' */
	{.bitmap_index = 488, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch=']' */
	{.bitmap_index = 496, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='^' */
	{.bitmap_index = 504, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='_' */
	{.bitmap_index = 512, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='`' */
	{.bitmap_index = 520, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='a' */
	{.bitmap_index = 528, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='b' */
	{.bitmap_index = 536, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='c' */
	{.bitmap_index = 544, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='d' */
	{.bitmap_index = 552, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='e' */
	{.bitmap_index = 560, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='f' */
	{.bitmap_index = 568, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='g' */
	{.bitmap_index = 576, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='h' */
	{.bitmap_index = 584, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='i' */
	{.bitmap_index = 592, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='j' */
	{.bitmap_index = 600, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='k' */
	{.bitmap_index = 608, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='l' */
	{.bitmap_index = 616, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='m' */
	{.bitmap_index = 624, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='n' */
	{.bitmap_index = 632, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='o' */
	{.bitmap_index = 640, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='p' */
	{.bitmap_index = 648, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='q' */
	{.bitmap_index = 656, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='r' */
	{.bitmap_index = 664, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='s' */
	{.bitmap_index = 672, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='t' */
	{.bitmap_index = 680, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='u' */
	{.bitmap_index = 688, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='v' */
	{.bitmap_index = 696, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='w' */
	{.bitmap_index = 704, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='x' */
	{.bitmap_index = 712, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='y' */
	{.bitmap_index = 720, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='z' */
	{.bitmap_index = 728, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='{' */
	{.bitmap_index = 736, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='|' */
	{.bitmap_index = 744, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='}' */
	{.bitmap_index = 752, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch='~' */
	{.bitmap_index = 760, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0}, /* ch=0x7F */
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
	{
		.range_start = 32, .range_length = 96, .glyph_id_start = 1,
		.unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
	},
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
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
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
const lv_font_t Epson_LTDC_small3 = {
#else
lv_font_t Epson_LTDC_small3 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 8,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
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

#endif /* LV_FONT_EPSON_LTDC_SMALL3 */

