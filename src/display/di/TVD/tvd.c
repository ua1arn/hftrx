#include "hardware.h"
#include "formats.h"

#if WITHTVDHW
#include "buffers.h"
#include "gpio.h"
#include "src/display/display.h"
#include "bsp_tvd.h"

#include <string.h>

static void TVD_Clock(void)                   //включает все нужные клоки, гейты, снимает с ресета
{
	const uint_fast32_t needfreq = 27000000;
	TVD_CCU_BGR_REG &= ~ (UINT32_C(1) << 17) & ~ (UINT32_C(1) << 16);      //assert TVD & TVD_TOP reset

	{
		const unsigned divider = calcdivround2(allwnrt113_get_video0_x1_freq(), needfreq);
		//PRINTF("TVD_Clock: needfreq=%u Hz, divider=%u\n", (unsigned) needfreq, (unsigned) divider);
		ASSERT(divider <= 32);
		TVD_CCU_CLK_REG =
			0x01 * (UINT32_C(1) << 24) |	// CLK_SRC_SEL 001: PLL_VIDEO0(1X)
			(divider - 1) * (UINT32_C(1) << 0) |	// FACTOR_M (0x00..0x1F: 1..32)
			0;
		TVD_CCU_CLK_REG |= (UINT32_C(1) << 31);
		local_delay_us(10);

		//PRINTF("TVD_Clock: allwnrt113_get_tvd_freq()=%u Hz\n", (unsigned) allwnrt113_get_tvd_freq());
	}

	TVD_CCU_BGR_REG |= (UINT32_C(1) << 1) | (UINT32_C(1) << 0);                //pass TVD & TVD_TOP clock

	CCU->MBUS_MAT_CLK_GATING_REG |= (UINT32_C(1) << 7);	// TVIN_MCLK_EN

	TVD_CCU_BGR_REG |= (UINT32_C(1) << 17) | (UINT32_C(1) << 16);         //de-assert TVD & TVD_TOP reset
}

//static uint8_t FilterAddress[TVD_3D_COMP_BUFFER_SIZE] __attribute__ ((aligned(128))); //буфер для фильтра
static uint8_t *FilterAddress=NULL;                                                     //не используется

static void TVD_Init(uint32_t mode)                          //mode: NTSC, PAL
{
	const unsigned sel = 0;

	TVD_Clock();
	tvd_top_set_reg_base(TVDECODER_TOP_BASE);  //установка базового адреса для TVD_TOP
	tvd_set_reg_base(sel, TVDECODER_BASE);         //установка базового адреса для TVD0
	tvd_reset(sel);                                   //сброс
	tvd_input_sel(WITHTVDHW_INPUT);                               //выбран TVIN0
	tvd_init(sel, CVBS_INTERFACE);              //CVBS - композитный видеосигнал
	tvd_config(sel, CVBS_INTERFACE, mode);              //NTSC или PAL
	tvd_set_wb_fmt(sel, TVD_PL_YUV420); //TVD_PL_YUV420 - планарный формат: компонента яркости и компоненты цвета отдельно)

	if (mode == NTSC)
	{
		//NTSC
	#if 0                                          //для случая TVD_MB_YUV420
		tvd_set_wb_width(sel, 704);
		tvd_set_wb_width_jump(sel, 704);
		tvd_set_wb_height(sel, 448 / 2);
	#else                                          //для остальных случаев (в том числе для TVD_PL_YUV420)
		tvd_set_wb_width(sel, 720);
		tvd_set_wb_width_jump(sel, 720);
		tvd_set_wb_height(sel, 480 / 2);
	#endif

	}
	else
	{
		//PAL
	#if 0                                          //для случая TVD_MB_YUV420
		tvd_set_wb_width(sel, 704);
		tvd_set_wb_width_jump(sel, 704);
		tvd_set_wb_height(sel, 448 / 2);
	#else                                          //для остальных случаев (в том числе для TVD_PL_YUV420)
		tvd_set_wb_width(sel, 720);
		tvd_set_wb_width_jump(sel, 720);
		tvd_set_wb_height(sel, 576 / 2);
	#endif
	}

	tvd_set_wb_uv_swap(sel, 0); //0 - V4L2_PIX_FMT_NV12 и V4L2_PIX_FMT_NV16, 1 - V4L2_PIX_FMT_NV21 и V4L2_PIX_FMT_NV61

	tvd_3d_mode(sel, 0, (uintptr_t) FilterAddress);    //0 - disable, 1 - enable

	tvd_agc_auto_config(sel);                         //AGC
	tvd_cagc_config(sel, 1);                           //chroma AGC
	tvd_blue_display_mode(sel, 0);         //0 - disabled, 1 - enabled, 2 - auto

	// mgs
	{
		const uintptr_t vram = allocate_dmabuffercolmain1fb();
		dcache_clean_invalidate(vram, cachesize_dmabuffercolmain1fb());
		const uintptr_t vram0 = vram;
		const uintptr_t vram1 = vram0 + TVD_SIZE;
		uintptr_t old = tvd_set_wb_addr2(sel, vram0, vram1);
		//PRINTF("old = %08X\n", (unsigned) old);
		ASSERT(old == 0);
	}

	tvd_irq_status_clear(sel, TVD_IRQ_FRAME_END); //очищаем флаг прерывания FRAME_END
	tvd_irq_enable(sel, TVD_IRQ_FRAME_END);     //разрешаем прерывания FRAME_END
}

static void TVD_CaptureOn(void)                    //включить кэпчуринг
{
	tvd_capture_on(0);
}

static void TVD_CaptureOff(void)                   //выключить кэпчуринг
{
	tvd_capture_off(0);
}

static uint32_t TVD_Status(void)                        //состояние: 0 - кадр не готов, 1 - кадр готов
{
	uint32_t status;
	tvd_irq_status_get(0,TVD_IRQ_FRAME_END,&status);
	if(status!=0)tvd_irq_status_clear(0,TVD_IRQ_FRAME_END);
	return status;
}

#include "../DI/sunxi_di.h"

static const unsigned tbx [] =
{
		50,
		51,
		52,
		53,
		54,
		55,
		56,
		57,
		58,
		59,
		60,
		61,
		62,
		63,
		64,
		65,
		66,
		67,
		68,
		69,
		70,
		71,
		72,
		73,
		74,
		75,
		76,
		77,
		78,
		79,
		80,
		81,
		82,
		83,
		84,
		85,
		86,
		87,
		88,
		89,
		90,
		91,
		92,
		93,
		94,
		95,
		96,
		97,
		98,
		99,
		100,
		101,
		102,
		103,
		104,
		105,
		106,
		107,
		108,
		109,
		110,
		111,
		112,
		113,
		114,
		115,
		116,
		117,
		118,
		119,
		120,
		121,
		122,
		123,
		124,
		125,
		126,
		127,
		128,
		129,
		198,
		199,
		200,
		201,
		202,
		203,
		204,
		205,
		206,
		207,
		208,
		209,
		210,
		211,
		212,
		213,
		214,
		215,
		216,
		217,
		218,
		219,
		220,
		221,
		222,
		223,
		224,
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7,
		8,
		9,
		10,
		11,
		12,
		13,
		14,
		15,
		16,
		17,
		18,
		19,
		20,
		21,
		22,
		23,
		24,
		25,
		26,
		27,
		28,
		29,
		30,
		31,
		32,
		33,
		34,
		35,
		36,
		37,
		38,
		39,
		40,
		41,
		42,
		43,
		44,
		45,
		46,
		47,
		48,
		49,
		225,
		226,
		227,
		228,
		229,
		230,
		231,
		232,
		233,
		234,
		235,
		236,
		237,
		238,
		239,
		240,
		241,
		242,
		243,
		244,
		245,
		246,
		247,
		248,
		249,
		250,
		251,
		252,
		253,
		254,
		255,
		256,
		257,
		258,
		259,
		260,
		261,
		262,
		263,
		264,
		265,
		266,
		267,
		268,
		269,
		270,
		271,
		272,
		273,
		274,
		275,
		276,
		277,
		278,
		279,
		280,
		281,
		282,
		283,
		284,
		285,
		286,
		287,
		130,
		131,
		132,
		133,
		134,
		135,
		136,
		137,
		138,
		139,
		140,
		141,
		142,
		143,
		144,
		145,
		146,
		147,
		148,
		149,
		150,
		151,
		152,
		153,
		154,
		155,
		156,
		157,
		158,
		159,
		160,
		161,
		162,
		163,
		164,
		165,
		166,
		167,
		168,
		169,
		170,
		171,
		172,
		173,
		174,
		175,
		176,
		177,
		178,
		179,
		180,
		181,
		182,
		183,
		184,
		185,
		186,
		187,
		188,
		189,
		190,
		191,
		192,
		193,
		194,
		195,
		196,
		197,
};

static unsigned encode288(unsigned v)
{
	ASSERT(v < ARRAY_SIZE(tbx));
	return tbx [v];
}

static unsigned pass288(unsigned v)
{
	return v;
}

static void vdecode(uintptr_t dst, uintptr_t src, unsigned (* infn)(unsigned v), unsigned (* outfn)(unsigned v))
{
	uintptr_t vramsrc0 = src;
	uintptr_t vramsrc1 = src + TVD_SIZE;
	uintptr_t vramdst0 = dst;
	uintptr_t vramdst1 = dst + TVD_SIZE;

//	memcpy(dst, src, TVD_SIZE * 3 / 2);
//	dcache_clean(dst, TVD_SIZE * 3 / 2);
//	return;
	unsigned row = 0;
	for (row = 0; row < TVD_HEIGHT; row += 2)
	{
		const unsigned srow = 2 * infn(row / 2);
		const unsigned drow = 2 * outfn(row / 2);
		// копирование области двух строк
		memcpy((void *) (vramdst0 + drow * TVD_WIDTH), (void *) (vramsrc0 + srow * TVD_WIDTH), TVD_WIDTH * 2);		// для двух строк
		memcpy((void *) (vramdst1 + drow * TVD_WIDTH / 2), (void *) (vramsrc1 + srow * TVD_WIDTH / 2), TVD_WIDTH);	// для двух строк
	}
	//memcpy(dst, src, TVD_SIZE * 3 / 2);
}

#define EMASK (1u << 11)	// PG11
#define DMASK (1u << 4)		// PG4
void TVD_Handler(void)
{
	//dbg_putchar('<');
	//подтверждение прерывания
	tvd_irq_status_clear(0, TVD_IRQ_FRAME_END);

	//смена буфера TVD
	//TVD_Shift++;

	//установка нового адреса TVD
	uintptr_t vram = allocate_dmabuffercolmain1fb();
	dcache_clean_invalidate(vram, cachesize_dmabuffercolmain1fb());
	const uintptr_t vram0 = vram;
	const uintptr_t vram1 = vram0 + TVD_SIZE;
	uintptr_t old = tvd_set_wb_addr2(0, vram0, vram1);
	if (old)
	{
		if ((gpioX_getinputs(GPIOG) & EMASK) == 0)
		{
			uintptr_t vramout = allocate_dmabuffercolmain1fb();
			vdecode(vramout, old, pass288, encode288);
			save_dmabuffercolmain1fb(vramout);
			release_dmabuffercolmain1fb(old);
		}
		else if ((gpioX_getinputs(GPIOG) & DMASK) == 0)
		{
			uintptr_t vramout = allocate_dmabuffercolmain1fb();
			vdecode(vramout, old, encode288, pass288);
			save_dmabuffercolmain1fb(vramout);
			release_dmabuffercolmain1fb(old);
		}
		else
		{
			save_dmabuffercolmain1fb(old);
		}
		//return;
		//запуск де-интерлейсера
		//di_dev_apply(TVD_Shift+1, DI_Shift+1);
		//di_dev_apply2(old, old, (uintptr_t) doutb);
	}

	//dbg_putchar('>');
}

//void DI_Handler(void)
//{
//	//dbg_putchar('I');
//	di_dev_query_state_with_clear(); //подтверждение прерывания
//
//	//смена буфера DI
//	//DI_Shift++;
//
//	//флаг готовности
//	//Ready_DI=1;
//	//dbg_putchar('i');
//}

void cap_test(void)
{
	arm_hardware_piog_inputs(EMASK);
	arm_hardware_piog_inputs(DMASK);
	PRINTF("ARRAY_SIZE(tbx)=%d\n", ARRAY_SIZE(tbx));
//	DI_INIT();
//	di_dev_query_state_with_clear();                                  //очистка флага прерывания
//	arm_hardware_set_handler_system(DI_IRQn, DI_Handler);

	//di_dev_apply2(allocate_dmabuffercolmain1fb(), allocate_dmabuffercolmain1fb(), diout);

	TVD_Init(PAL);	// PAL
	tvd_irq_status_clear(0, TVD_IRQ_FRAME_END);                        //очистка флага прерывания
	arm_hardware_set_handler_system(TVD_IRQn, TVD_Handler);
	TVD_CaptureOn();

}

#endif /* WITHTVDHW */
