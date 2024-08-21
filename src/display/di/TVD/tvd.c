#include "hardware.h"
#include "formats.h"

#if defined (TVDECODER_PTR)

#include "src/display/display.h"

#include "tvd.h"

//const uint32_t TVD_YBUF[10]= //буфер для компоненты яркости
//{
// VIDEO_MEMORY0+(( 0*TVD_SIZE)/2),
// VIDEO_MEMORY0+(( 3*TVD_SIZE)/2),
// VIDEO_MEMORY0+(( 6*TVD_SIZE)/2),
// VIDEO_MEMORY0+(( 9*TVD_SIZE)/2),
// VIDEO_MEMORY0+((12*TVD_SIZE)/2),
// VIDEO_MEMORY0+((15*TVD_SIZE)/2),
// VIDEO_MEMORY0+((18*TVD_SIZE)/2),
// VIDEO_MEMORY0+((21*TVD_SIZE)/2),
// VIDEO_MEMORY0+((24*TVD_SIZE)/2),
// VIDEO_MEMORY0+((27*TVD_SIZE)/2),
//};
//
//const uint32_t TVD_CBUF[10]= //буфер для компонент цвета
//{
// VIDEO_MEMORY0+(( 2*TVD_SIZE)/2),
// VIDEO_MEMORY0+(( 5*TVD_SIZE)/2),
// VIDEO_MEMORY0+(( 8*TVD_SIZE)/2),
// VIDEO_MEMORY0+((11*TVD_SIZE)/2),
// VIDEO_MEMORY0+((14*TVD_SIZE)/2),
// VIDEO_MEMORY0+((17*TVD_SIZE)/2),
// VIDEO_MEMORY0+((20*TVD_SIZE)/2),
// VIDEO_MEMORY0+((23*TVD_SIZE)/2),
// VIDEO_MEMORY0+((26*TVD_SIZE)/2),
// VIDEO_MEMORY0+((29*TVD_SIZE)/2),
//};

//uint8_t FilterAddress[TVD_3D_COMP_BUFFER_SIZE] __attribute__ ((aligned(128))); //буфер для фильтра
uint8_t *FilterAddress=NULL;                                                     //не используется

void TVD_Clock(void)                   //включает все нужные клоки, гейты, снимает с ресета
{
 CCU->TVD_BGR_REG&=~((UINT32_C(1) << 17)|(UINT32_C(1) << 16));      //assert TVD & TVD_TOP reset

 CCU->TVD_CLK_REG=(UINT32_C(1) <<31)|(UINT32_C(1) << 24)|(11-1); //clock enable TVD PLL_VIDEO0(1x)/11 =297/11 = 27 MHz (CVBS Clock)

 CCU->TVD_BGR_REG|=(UINT32_C(1) << 1)|1;                //pass TVD & TVD_TOP clock

 CCU->MBUS_MAT_CLK_GATING_REG|=(UINT32_C(1) << 7);      //gating MBUS clock for TVIN

 CCU->TVD_BGR_REG|=(UINT32_C(1) << 17)|(UINT32_C(1) << 16);         //de-assert TVD & TVD_TOP reset
}

void TVD_Init(uint32_t mode)                          //mode: NTSC, PAL
{
 TVD_Clock();

 tvd_top_set_reg_base(TVD_TOP_BASE);             //установка базового адреса для TVD_TOP
 tvd_set_reg_base(0,TVD0_BASE);                  //установка базового адреса для TVD0

 tvd_reset(0);                                   //сброс

 tvd_input_sel(0);                               //выбран TVIN0

 tvd_init(0,CVBS_INTERFACE);                     //CVBS - композитный видеосигнал

 tvd_config(0,CVBS_INTERFACE,mode);              //NTSC или PAL

 tvd_set_wb_fmt(0,TVD_PL_YUV420);                //TVD_PL_YUV420 - планарный формат: компонента яркости и компоненты цвета отдельно)

 if(mode==NTSC)
 {
  #if 0                                          //для случая TVD_MB_YUV420
  tvd_set_wb_width(0,704);
  tvd_set_wb_width_jump(0,704);
  tvd_set_wb_height(0,448/2);
  #else                                          //для остальных случаев (в том числе для TVD_PL_YUV420)
  tvd_set_wb_width(0,720);
  tvd_set_wb_width_jump(0,720);
  tvd_set_wb_height(0,480/2);
  #endif

 }
 else //PAL
 {
  #if 0                                          //для случая TVD_MB_YUV420
  tvd_set_wb_width(0,704);
  tvd_set_wb_width_jump(0,704);
  tvd_set_wb_height(0,448/2);
  #else                                          //для остальных случаев (в том числе для TVD_PL_YUV420)
  tvd_set_wb_width(0,720);
  tvd_set_wb_width_jump(0,720);
  tvd_set_wb_height(0,576/2);
  #endif
 }

 tvd_set_wb_uv_swap(0,0);                        //0 - V4L2_PIX_FMT_NV12 и V4L2_PIX_FMT_NV16, 1 - V4L2_PIX_FMT_NV21 и V4L2_PIX_FMT_NV61

 tvd_3d_mode(0,0,(uint32_t)FilterAddress);            //0 - disable, 1 - enable

 tvd_agc_auto_config(0);                         //AGC
 tvd_cagc_config(0,1);                           //chroma AGC
 tvd_blue_display_mode(0,0);                     //0 - disabled, 1 - enabled, 2 - auto
#if 0
 // mgs
 tvd_set_wb_addr(0,TVD_YBUF[0],TVD_CBUF[0]);     //устанавливаем адреса буферов компонент яркости и цвета
#endif

 tvd_irq_status_clear(0,TVD_IRQ_FRAME_END);      //очищаем флаг прерывания FRAME_END
 tvd_irq_enable(0,TVD_IRQ_FRAME_END);            //разрешаем прерывания FRAME_END
}

void TVD_CaptureOn(void)                    //включить кэпчуринг
{
 tvd_capture_on(0);
}

void TVD_CaptureOff(void)                   //выключить кэпчуринг
{
 tvd_capture_off(0);
}

uint32_t TVD_Status(void)                        //состояние: 0 - кадр не готов, 1 - кадр готов
{
 uint32_t status;
 tvd_irq_status_get(0,TVD_IRQ_FRAME_END,&status);
 if(status!=0)tvd_irq_status_clear(0,TVD_IRQ_FRAME_END);
 return status;
}

#endif /* defined (TVDECODER_PTR) */
