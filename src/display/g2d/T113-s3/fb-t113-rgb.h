#ifndef _FB_T113_RGB_H_
#define _FB_T113_RGB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "src/display/display.h"
#define VIDEO_MEMORY0 ((uintptr_t) colmain_fb_draw())
#define VIDEO_MEMORY1 ((uintptr_t) colmain_fb_draw())

#define LCD_PIXEL_WIDTH  DIM_X
#define LCD_PIXEL_HEIGHT DIM_Y
#define BYTE_PER_PIXEL     4

#define DE2_FORMAT_ARGB_8888	0x00
#define DE2_FORMAT_ABGR_8888	0x01
#define DE2_FORMAT_BGRA_8888	0x03
#define DE2_FORMAT_XRGB_8888	0x04
#define DE2_FORMAT_XBGR_8888	0x05
#define DE2_FORMAT_RGB_888	0x08
#define DE2_FORMAT_BGR_888	0x09
#define DE2_FORMAT_RGB_565	0x0A
#define DE2_FORMAT_ARGB_1555	0x10
#define DE2_FORMAT_ABGR_1555	0x11

#ifdef __cplusplus
}
#endif

#endif
