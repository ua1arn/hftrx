#include "hardware.h"
#include "formats.h"

#if WIHSTVDHW
#include "buffers.h"

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
	const uint_fast32_t needfreq = 27000000;
	CCU->TVD_BGR_REG&=~((UINT32_C(1) << 17)|(UINT32_C(1) << 16));      //assert TVD & TVD_TOP reset

	{
		unsigned divider;
		unsigned prei = calcdivider(calcdivround2(allwnrt113_get_video0_x1_freq(), 27000000), 4, (8 | 4 | 2 | 1), & divider, 1);
		//PRINTF("TVD_Clock: needfreq=%u Hz, prei=%u, divider=%u\n", (unsigned) needfreq, (unsigned) prei, (unsigned) divider);
		ASSERT(divider < 16);
		CCU->TVD_CLK_REG = (CCU->TVD_CLK_REG & ~ ((UINT32_C(0x07) << 24) | (UINT32_C(0x03) << 8) | (UINT32_C(0x0F) << 0))) |
		0x01 * (UINT32_C(1) << 24) |	// CLK_SRC_SEL 001: PLL_VIDEO0(1X)
		prei * (UINT32_C(1) << 8) |	// FACTOR_N 0..3: 1..8
		divider * (UINT32_C(1) << 0) |	// FACTOR_M (0x00..0x0F: 1..16)
		0;
		CCU->TVD_CLK_REG |= (UINT32_C(1) << 31);
		local_delay_us(10);

		//PRINTF("TVD_Clock: allwnrt113_get_tvd_freq()=%u Hz\n", (unsigned) allwnrt113_get_tvd_freq());
	}

	CCU->TVD_BGR_REG|=(UINT32_C(1) << 1)|1;                //pass TVD & TVD_TOP clock

	CCU->MBUS_MAT_CLK_GATING_REG |= (UINT32_C(1) << 7);	// TVIN_MCLK_EN

	CCU->TVD_BGR_REG|=(UINT32_C(1) << 17)|(UINT32_C(1) << 16);         //de-assert TVD & TVD_TOP reset
}

void TVD_Init(uint32_t mode)                          //mode: NTSC, PAL
{
	const unsigned sel = 0;
 TVD_Clock();

 tvd_top_set_reg_base(TVD_TOP_BASE);             //установка базового адреса для TVD_TOP
 tvd_set_reg_base(0,TVD0_BASE);                  //установка базового адреса для TVD0

 tvd_reset(sel);                                   //сброс

 tvd_input_sel(0);                               //выбран TVIN0

 tvd_init(sel, CVBS_INTERFACE);                     //CVBS - композитный видеосигнал

 tvd_config(sel, CVBS_INTERFACE,mode);              //NTSC или PAL

 tvd_set_wb_fmt(sel, TVD_PL_YUV420);                //TVD_PL_YUV420 - планарный формат: компонента яркости и компоненты цвета отдельно)

 if(mode==NTSC)
 {
  #if 0                                          //для случая TVD_MB_YUV420
  tvd_set_wb_width(sel, 704);
  tvd_set_wb_width_jump(sel, 704);
  tvd_set_wb_height(sel, 448/2);
  #else                                          //для остальных случаев (в том числе для TVD_PL_YUV420)
  tvd_set_wb_width(sel, 720);
  tvd_set_wb_width_jump(sel, 720);
  tvd_set_wb_height(sel, 480/2);
  #endif

 }
 else //PAL
 {
  #if 0                                          //для случая TVD_MB_YUV420
  tvd_set_wb_width(sel, 704);
  tvd_set_wb_width_jump(sel, 704);
  tvd_set_wb_height(sel, 448/2);
  #else                                          //для остальных случаев (в том числе для TVD_PL_YUV420)
  tvd_set_wb_width(sel, 720);
  tvd_set_wb_width_jump(sel, 720);
  tvd_set_wb_height(sel, 576/2);
  #endif
 }

 tvd_set_wb_uv_swap(sel, 0);                        //0 - V4L2_PIX_FMT_NV12 и V4L2_PIX_FMT_NV16, 1 - V4L2_PIX_FMT_NV21 и V4L2_PIX_FMT_NV61

 tvd_3d_mode(sel, 0,(uint32_t)FilterAddress);            //0 - disable, 1 - enable

 tvd_agc_auto_config(sel);                         //AGC
 tvd_cagc_config(sel, 1);                           //chroma AGC
 tvd_blue_display_mode(sel, 0);                     //0 - disabled, 1 - enabled, 2 - auto

 // mgs
 {
		const uintptr_t vram = allocate_dmabuffercolmain1fb();
		const uintptr_t vram0 = vram;
		const uintptr_t vram1 = vram0 + TVD_SIZE;
		uintptr_t old = tvd_set_wb_addr2(sel,  vram0, vram1);
		//PRINTF("old = %08X\n", (unsigned) old);
		ASSERT(old == 0);
 }

 tvd_irq_status_clear(sel, TVD_IRQ_FRAME_END);      //очищаем флаг прерывания FRAME_END
 tvd_irq_enable(sel, TVD_IRQ_FRAME_END);            //разрешаем прерывания FRAME_END
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

#include "../DI/sunxi_di.h"
//static const uint8_t picture0 [] =
//{
//	#include "src/testdata/picture.h"
//};

static RAMNC uint8_t doutb [(TVD_SIZE * 3) / 2];
void tdout(void)
{
	//printhex(0, doutb, 256);
	PACKEDTVBUFF_T * const fb = tvout_fb_draw();
	//di_dev_apply2((uintptr_t) picture0, (uintptr_t) picture0, (uintptr_t) fb);
	local_delay_ms(100);
	memcpy(fb, doutb, sizeof doutb);
	//memcpy(fb, picture0, datasize_dmabuffer1fb());
	tvout_nextfb();
}

// mgs
void TVD_Handler(void)
{
	//dbg_putchar('<');
	//подтверждение прерывания
	tvd_irq_status_clear(0,TVD_IRQ_FRAME_END);

	//смена буфера TVD
	//TVD_Shift++;

	//установка нового адреса TVD
	uintptr_t vram = allocate_dmabuffercolmain1fb();
	const uintptr_t vram0 = vram;
	const uintptr_t vram1 = vram0 + TVD_SIZE;
	uintptr_t old = tvd_set_wb_addr2(0,vram0, vram1);
	if (old)
	{
		//PRINTF("%08X ", old);
		//save_dmabuffercolmain1fb(old);
		release_dmabuffercolmain1fb(old);
		//return;
		//запуск де-интерлейсера
		//di_dev_apply(TVD_Shift+1, DI_Shift+1);
		di_dev_apply2(old, old, (uintptr_t) doutb);
	}

	//dbg_putchar('>');
}

void DI_Handler(void)
{
	//dbg_putchar('I');
	di_dev_query_state_with_clear(); //подтверждение прерывания

	//смена буфера DI
	//DI_Shift++;

	//флаг готовности
	//Ready_DI=1;
	//dbg_putchar('i');
}

void cap_test(void)
{

	DI_INIT();

//	eGon2_InsINT_Func(TVD_INT_NUMBER,(int*)TVD_Handler,(void*)0);
//	GIC_SetPriority(TVD_INT_NUMBER,0x40);                             //низкий приоритет
	tvd_irq_status_clear(0,TVD_IRQ_FRAME_END);                        //очистка флага прерывания
//	eGon2_EnableInt(TVD_INT_NUMBER);
	arm_hardware_set_handler_system(TVD_IRQn, TVD_Handler);

//	eGon2_InsINT_Func(DI_INT_NUMBER,(int*)DI_Handler,(void*)0);
//	GIC_SetPriority(DI_INT_NUMBER,0x30);                              //средний приоритет
	di_dev_query_state_with_clear();                                  //очистка флага прерывания
//	eGon2_EnableInt(DI_INT_NUMBER);
	arm_hardware_set_handler_system(DI_IRQn, DI_Handler);

	TVD_Init(1);	// PAL


	//di_dev_apply2(allocate_dmabuffercolmain1fb(), allocate_dmabuffercolmain1fb(), diout);

	TVD_CaptureOn();

}

#endif /* WIHSTVDHW */
