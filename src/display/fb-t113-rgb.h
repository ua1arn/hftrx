#ifndef _FB_T113_RGB_H_
#define _FB_T113_RGB_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include "Type.h"
//#include "Gate.h"

//---------------------------------------

//����������� �� ����
#define TVD_WIDTH  720
#define TVD_HEIGHT 576

#define TVD_SIZE (TVD_WIDTH*TVD_HEIGHT)

//---------------------------------------

#define TVE_MODE

#define is_composite 1

#if is_composite

extern void sun8i_vi_scaler_setup(uint32_t src_w,uint32_t src_h,uint32_t dst_w,uint32_t dst_h,uint32_t hscale,uint32_t vscale,uint32_t hphase,uint32_t vphase);
extern void sun8i_vi_scaler_enable(uint8_t enable);

#endif

#define LCD_INT_NUMBER 122

//��� VI
#define DE2_FORMAT_YUV420_V1U1V0U0 0x08
#define DE2_FORMAT_YUV420_U1V1U0V0 0x09
#define DE2_FORMAT_YUV420_PLANAR   0x0A

//��� UI
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

#ifdef TVE_MODE

#define LCD_PIXEL_WIDTH  720
#define LCD_PIXEL_HEIGHT 576

#else

#define LCD_PIXEL_WIDTH  800
#define LCD_PIXEL_HEIGHT 480

#endif

#define BYTE_PER_PIXEL     4

#define DE2_FORMAT DE2_FORMAT_ABGR_8888

void LCD_SwitchAddress(uint32_t address,uint8_t plane);

#ifdef __cplusplus
}
#endif

#endif
