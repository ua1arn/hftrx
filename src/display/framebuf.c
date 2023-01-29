/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Доработки для LS020 Василий Линывый, livas60@mail.ru
//
// Функции построения изображений в буфере - вне зависимости от того, есть ли в процессоре LTDC.'
// Например при offscreen composition растровых изображений для SPI дисплеев
//

#include "hardware.h"

#include "board.h"
#include "display.h"
#include "formats.h"
#include "spi.h"	// hardware_spi_master_send_frame
#include "display2.h"
#include "fontmaps.h"
#include <string.h>

#if (CPUSTYLE_T113 || CPUSTYLE_F133) && WITHMDMAHW
	/* Использование G2D для формирования изображений */

	#include "g2d_driver.h"

#if LCDMODE_MAIN_ARGB888
	#define DstImageFormat 0x00	//G2D_FMT_ARGB_AYUV8888
	#define WB_DstImageFormat 0x00	//G2D_FMT_ARGB_AYUV8888

#elif LCDMODE_MAIN_RGB565
	#define DstImageFormat G2D_FMT_RGB565
	#define WB_DstImageFormat 0x0A

#else
	#error Unsupported framebuffer format. Looks like you need remove WITHLTDCHW
#endif

//#include "debug_f133.h"

static unsigned awxx_get_ui_attr(void)
{
	unsigned ui_attr = 0;
	ui_attr = 255 << 24;
	//	if (img->bpremul)
	//		ui_attr |= 0x1 << 17;
	ui_attr |= DstImageFormat << 8;
	//ui_attr |= G2D_GLOBAL_ALPHA << 1; // linux sample use G2D_PIXEL_ALPHA -> 0xFF000401
	ui_attr |= G2D_PIXEL_ALPHA << 1; // нужно для работы color key linux sample use G2D_PIXEL_ALPHA -> 0xFF000401
	//ui_attr |= (1u << 4);	/* Use FILLC register */
	ui_attr |= 1;
	return ui_attr;
}

static unsigned awxx_get_vi_attr(void)
{
	unsigned vi_attr = 0;
	vi_attr = 255 << 24;
	//	if (img->bpremul)
	//		vi_attr |= 0x1 << 17;
	vi_attr |= DstImageFormat << 8;
	//vi_attr |= G2D_GLOBAL_ALPHA << 1; // linux sample use G2D_PIXEL_ALPHA -> 0xFF000401
	vi_attr |= G2D_PIXEL_ALPHA << 1; // нужно для работы color key linux sample use G2D_PIXEL_ALPHA -> 0xFF000401
	//vi_attr |= (1u << 4);	/* Use FILLC register */
	vi_attr |= 1;
	return vi_attr;
}

static void t113_fillrect(
	uintptr_t taddr,
	uint_fast32_t tstride,
	uint_fast32_t tsizehw,
	unsigned alpha,
	COLOR24_T color24
	)
{
//	memset(G2D_V0, 0, sizeof * G2D_V0);
//	memset(G2D_UI0, 0, sizeof * G2D_UI0);
//	memset(G2D_UI1, 0, sizeof * G2D_UI1);
//	memset(G2D_UI2, 0, sizeof * G2D_UI2);
//	memset(G2D_BLD, 0, sizeof * G2D_BLD);
//	memset(G2D_WB, 0, sizeof * G2D_WB);

	/* Отключаем все источники */
	G2D_BLD->BLD_FILL_COLOR_CTL = 0;
	G2D_V0->V0_ATTCTL = 0;
	G2D_UI0->UI_ATTR = 0;
	G2D_UI1->UI_ATTR = 0;
	G2D_UI2->UI_ATTR = 0;

	G2D_BLD->BLD_SIZE = tsizehw;	// размр выходного буфера
	G2D_BLD->BLD_CH_ISIZE [0] = tsizehw;
	G2D_BLD->BLD_CH_OFFSET [0] = 0;// ((row) << 16) | ((col) << 0);
	G2D_BLD->ROP_CTL = 0*0x00F0;	// 0x00F0 G2D_V0, 0x55F0 UI1, 0xAAF0 UI2
	//G2D_BLD->BLD_CTL = 0x00010001;	// G2D_BLD_COPY
	G2D_BLD->BLD_CTL = 0x03010301;	// G2D_BLD_SRCOVER - default value
	//G2D_BLD->BLD_CTL = 0x00000000;	// G2D_BLD_CLEAR

	G2D_BLD->BLD_PREMUL_CTL=0*0x00000001; /* 0x00000001 */
	G2D_BLD->BLD_OUT_COLOR=0*0x002; //0*0x00000001; /* 0x00000001 */

	//G2D_BLD->BLD_BK_COLOR = /*(alpha << 24) | */(color24 & 0xFFFFFF); // ~ 0u; //c24;	/* всегда RGB888. */

	//	G2D_BLD->BLD_PREMUL_CTL |= (1u << 0);	// 0 or 1 - sel 1 or sel 0
	/* Используем для заполнения BLD_FILLC0 цвет и прозрачность
	 */
	G2D_BLD->BLD_FILL_COLOR [0] = (alpha << 24) | (color24 & 0xFFFFFF); // цвет и alpha канал
	G2D_BLD->BLD_CH_ISIZE [0] = tsizehw;

	// BLD_FILL_COLOR_CTL: BLD_FILLC [0] или BLD_BK_COLOR
	G2D_BLD->BLD_FILL_COLOR_CTL =
		(1u << 8) |    	// P0_EN: Pipe0 enable
		(1u << 0) |		// P0_FCEN: Pipe0 fill color enable
		0;

	/* Write-back settings */
	G2D_WB->WB_ATT = WB_DstImageFormat;
	G2D_WB->WB_SIZE = tsizehw;
	G2D_WB->WB_PITCH0 = tstride;
	G2D_WB->WB_LADD0 = taddr;
	G2D_WB->WB_HADD0 = taddr >> 32;
}

//#include "debug_f133.h"
//
//void debug_g2d(const char * place, int line)
//{
//	PRINTF("**** %s/%d\n", place, line);
////	G2D_UI_Type_print(G2D_UI0, "G2D_UI0");
////	G2D_UI_Type_print(G2D_UI1, "G2D_UI1");
////	G2D_UI_Type_print(G2D_UI2, "G2D_UI2");
////	G2D_LAY_Type_print(G2D_V0, "G2D_V0");
//	G2D_BLD_Type_print(G2D_BLD, "G2D_BLD");
////	G2D_WB_Type_print(G2D_WB, "G2D_WB");
//}

#endif /* (CPUSTYLE_T113 || CPUSTYLE_F133) */

/*
	Dead time value in the AXI clock cycle inserted between two consecutive accesses on
	the AXI master port. These bits represent the minimum guaranteed number of cycles
	between two consecutive AXI accesses
 */
#define DMA2D_AMTCR_DT_VALUE 255uL	/* 0..255 */
#define DMA2D_AMTCR_DT_ENABLE 1uL	/* 0..1 */

#define MDMA_CH		MDMA_Channel0
#define MDMA_DATA	(MDMA_Channel1->CSAR)	// регистр выделенного канала MDMA используется для хранения значения цвета. Переиферия не кэшируется.

#ifndef DMA2D_CR_LOM
	#define DMA2D_CR_LOM	(1u << 6)	/* documented but missing in headers. */
#endif

#define MDMA_CCR_PL_VALUE 0uL	// PL: priority 0..3: min..max

#if LCDMODE_LTDC_L24
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN		(1 * DMA2D_FGPFCCR_CM_0)	/* 0001: RGB888 */
	#define MDMA_CTCR_xSIZE_MAIN			0x00	// 1 byte
	//#define DMA2D_OPFCCR_CM_VALUE_MAIN	(1 * DMA2D_OPFCCR_CM_0)	/* 001: RGB888 */

#elif LCDMODE_MAIN_L8
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN		(5 * DMA2D_FGPFCCR_CM_0)	/* 0101: L8 */
	#define MDMA_CTCR_xSIZE_MAIN			0x00	// 1 byte
	////#define DMA2D_OPFCCR_CM_VALUE	(x * DMA2D_OPFCCR_CM_0)	/* not supported */

#elif LCDMODE_MAIN_ARGB888
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN		(0 * DMA2D_FGPFCCR_CM_0)	/* 0000: ARGB888 */
	#define MDMA_CTCR_xSIZE_MAIN			0x02	// 10: Word (32-bit)
	#define DMA2D_OPFCCR_CM_VALUE_MAIN		(0 * DMA2D_OPFCCR_CM_0)	/* 0: 000: ARGB8888 */

#elif LCDMODE_MAIN_RGB565
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN		(2 * DMA2D_FGPFCCR_CM_0)	/* 0010: RGB565 */
	#define MDMA_CTCR_xSIZE_MAIN			0x01	// 2 byte
	#define DMA2D_OPFCCR_CM_VALUE_MAIN		(2 * DMA2D_OPFCCR_CM_0)	/* 010: RGB565 */

#endif /* LCDMODE_MAIN_L8 */

#if LCDMODE_PIP_L8
	#define DMA2D_FGPFCCR_CM_VALUE_PIP	(5 * DMA2D_FGPFCCR_CM_0)	/* 0101: L8 */
	#define MDMA_CTCR_xSIZE_PIP			0x00	// 1 byte
	////#define DMA2D_OPFCCR_CM_VALUE_MAIN	(x * DMA2D_OPFCCR_CM_0)	/* not supported */

#elif LCDMODE_PIP_RGB565
	#define DMA2D_FGPFCCR_CM_VALUE_PIP	(2 * DMA2D_FGPFCCR_CM_0)	/* 0010: RGB565 */
	#define DMA2D_OPFCCR_CM_VALUE_PIP	(2 * DMA2D_OPFCCR_CM_0)	/* 010: RGB565 */
	#define MDMA_CTCR_xSIZE_PIP			0x01	// 2 byte

#else /* LCDMODE_MAIN_L8 */
	#define DMA2D_FGPFCCR_CM_VALUE_PIP	DMA2D_FGPFCCR_CM_VALUE_MAIN
	#define DMA2D_OPFCCR_CM_VALUE_PIP	DMA2D_OPFCCR_CM_VALUE_MAIN
	#define MDMA_CTCR_xSIZE_PIP			MDMA_CTCR_xSIZE_MAIN

#endif /* LCDMODE_MAIN_L8 */

//#define DMA2D_FGPFCCR_CM_VALUE_L24	(1 * DMA2D_FGPFCCR_CM_0)	/* 0001: RGB888 */
//#define DMA2D_FGPFCCR_CM_VALUE_L16	(2 * DMA2D_FGPFCCR_CM_0)	/* 0010: RGB565 */
//#define DMA2D_FGPFCCR_CM_VALUE_L8	(5 * DMA2D_FGPFCCR_CM_0)	/* 0101: L8 */

//#define MDMA_CTCR_xSIZE_U32			0x02	// 4 byte
//#define MDMA_CTCR_xSIZE_U16			0x01	// 2 byte
//#define MDMA_CTCR_xSIZE_U8			0x00	// 1 byte
//#define MDMA_CTCR_xSIZE_RGB565		0x01	// 2 byte

#if WITHMDMAHW && (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)

static uint_fast8_t
mdma_getbus(uintptr_t addr)
{
#if CPUSTYLE_STM32H7XX
	addr &= 0xFF000000uL;
	return (addr == 0x00000000uL || addr == 0x20000000uL);

#elif CPUSTYLE_STM32MP1 && CORE_CA7
	// SYSMEM
	// DDRCTRL
	/*
	 * 0: The system/AXI bus is used on channel x.
	 * 1: The AHB bus/TCM is used on channel x.
	 */
	addr &= 0xFF000000uL;
	return (addr == 0x00000000uL || addr == 0x20000000uL);

#elif CPUSTYLE_STM32MP1 && CORE_CM4
	#error M4 core not supported
	/*
	 * 0: The system/AXI bus is used on channel x.
	 * 1: The AHB bus/TCM is used on channel x.
	 */
	addr &= 0xFF000000uL;
	return (addr == 0x00000000uL || addr == 0x20000000uL);

#else
	return 0;

#endif
}


static uint_fast8_t
mdma_tlen(uint_fast32_t nb, uint_fast8_t ds)
{
	return (nb < 128 ? nb : 128) / ds * ds;
}

//	For the TCM memory accesses, the burst access is only allowed when the increment
//	and data size are identical and lower than or equal to 32-bit.

// DBURST value must be programmed as to ensure that the burst size is lower than the
// Transfer Length. If this is not ensured, the result is unpredictable.

// И еще несколько условий..

static uint_fast8_t
mdma_getburst(uint_fast16_t tlen, uint_fast8_t bus, uint_fast8_t xinc)
{
	if (bus == 0 && xinc == 0)
		return 4;
	if (xinc == 0)
		return 0;

	if (bus != 0)
		return 0;

	if (tlen >= 128)
		return 7;
	if (tlen >= 64)
		return 6;
	if (tlen >= 32)
		return 5;
	if (tlen >= 16)
		return 4;
	if (tlen >= 8)
		return 3;
	if (tlen >= 4)
		return 2;
	if (tlen >= 2)
		return 1;
	return 0;
}

static void
mdma_stop_unused(void)
{
	MDMA_CH->CCR &= ~ MDMA_CCR_EN_Msk;
	while ((MDMA_CH->CCR & MDMA_CCR_EN_Msk) != 0)
		;
}

/* запустить пересылку и дождаться завершения. */
static void
mdma_startandwait(void)
{
	// MDMA implementation
	MDMA_CH->CIFCR =
		MDMA_CIFCR_CLTCIF_Msk |
		MDMA_CIFCR_CBTIF_Msk |
		MDMA_CIFCR_CBRTIF_Msk |
		MDMA_CIFCR_CCTCIF_Msk |
		MDMA_CIFCR_CTEIF_Msk |
		0;
	(void) MDMA_CH->CIFCR;

	// Set priority
	MDMA_CH->CCR = (MDMA_CH->CCR & ~ (MDMA_CCR_PL_Msk)) |
			(MDMA_CCR_PL_VALUE << MDMA_CCR_PL_Pos) |
			0;
	(void) MDMA_CH->CCR;

	MDMA_CH->CCR |= MDMA_CCR_EN_Msk;
	(void) MDMA_CH->CCR;
	/* start transfer */
	MDMA_CH->CCR |= MDMA_CCR_SWRQ_Msk;
	(void) MDMA_CH->CCR;

	/* wait for complete */
	while ((MDMA_CH->CISR & MDMA_CISR_CTCIF_Msk) == 0)	// Channel x Channel Transfer Complete interrupt flag
	{
		ASSERT((MDMA_CH->CISR & MDMA_CISR_TEIF_Msk) == 0);	/* Channel x transfer error interrupt flag */
		hardware_nonguiyield();
	}
	//__DMB();	//ensure the ordering of data cache maintenance operations and their effects
	ASSERT((MDMA_CH->CISR & MDMA_CISR_TEIF_Msk) == 0);	/* Channel x transfer error interrupt flag */

}

void arm_hardware_mdma_initialize(void)
{
#if CPUSTYLE_STM32MP1
	/* Enable the MDMA Clock */
	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_MDMAEN;	/* MDMA clock enable */
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6LPENSETR = RCC_MP_AHB6LPENSETR_MDMALPEN;	/* MDMA clock enable */
	(void) RCC->MP_AHB6LPENSETR;
	//RCC->MP_TZAHB6ENSETR = RCC_MP_TZAHB6ENSETR_MDMAEN;
	//(void) RCC->MP_TZAHB6ENSETR;

	/* SYSCFG clock enable */
	RCC->MP_APB3ENSETR = RCC_MP_APB3ENSETR_SYSCFGEN;
	(void) RCC->MP_APB3ENSETR;
	RCC->MP_APB3LPENSETR = RCC_MP_APB3LPENSETR_SYSCFGLPEN;
	(void) RCC->MP_APB3LPENSETR;
	/*
	 * Interconnect update : select master using the port 1.
	 * LTDC = AXI_M9.
	 * MDMA = AXI_M7.
	 */
	SYSCFG->ICNR |= SYSCFG_ICNR_AXI_M7;
	(void) SYSCFG->ICNR;

#elif CPUSTYLE_STM32H7XX
	/* Enable the MDMA Clock */
	RCC->AHB3ENR |= RCC_AHB3ENR_MDMAEN_Msk;	/* MDMA clock enable */
	(void) RCC->AHB3ENR;
	RCC->AHB3LPENR |= RCC_AHB3LPENR_MDMALPEN_Msk;
	(void) RCC->AHB3LPENR;

#else /* CPUSTYLE_STM32H7XX */
	/* Enable the MDMA Clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_MDMAEN;	/* MDMA clock enable */
	(void) RCC->AHB1ENR;
	RCC->AHB3LPENR |= RCC_AHB3LPENR_MDMALPEN_Msk;
	(void) RCC->AHB3LPENR;

#endif /* CPUSTYLE_STM32H7XX */
}

#elif WITHMDMAHW & (CPUSTYLE_T113 || CPUSTYLE_F133)

/* Использование G2D для формирования изображений */
// https://github.com/tinalinux/linux-3.10/blob/46f73ef4efcb4014b25e5ad1eca750ad62a1d0ff/drivers/char/sunxi_g2d/g2d_driver.c
// https://github.com/tinalinux/linux-3.10/blob/46f73ef4efcb4014b25e5ad1eca750ad62a1d0ff/drivers/char/sunxi_g2d/g2d_regs.h
// https://github.com/tinalinux/linux-3.10/blob/46f73ef4efcb4014b25e5ad1eca750ad62a1d0ff/drivers/char/sunxi_g2d/g2d_bsp_sun8iw11.c


// https://github.com/lianghuixin/licee4.4/blob/bfee1d63fa355a54630244307296a00a973b70b0/linux-4.4/drivers/char/sunxi_g2d/g2d_bsp_v2.c

/* clear most of the registers value to default */
static uint32_t local_mixer_reg_init(void){
	//uint32_t i;

//	for(i=0;i<=0x148;i+=4)
//		write_wvalue(i, 0);
	//G2D_V0->G2D_SCAN_ORDER_REG = 0x15FF0000;//DMA MBUS Length

//	/* initial the color space converter parameter */
//	csc_coeff_set();
//
//	/* initial the scaler coefficient parameter */
//	scaler_coeff_set();

	return 0;
}

/* Запуск и ожидание завершения работы G2D */
/* 0 - timeout. 1 - OK */
static int hwacc_waitdone(void)
{
	unsigned n = 0x2000000;
	for (;;)
	{
		const uint_fast32_t MASK = (1u << 0);	/* FINISH_IRQ */
		const uint_fast32_t mixer_int = G2D_MIXER->G2D_MIXER_INT;
		const uint_fast32_t rot_int = G2D_ROT->ROT_INT;
		if (((mixer_int & MASK) != 0))
		{
			G2D_MIXER->G2D_MIXER_INT = MASK;
			break;
		}
		if (((rot_int & MASK) != 0))
		{
			G2D_ROT->ROT_INT = MASK;
			break;
		}
		hardware_nonguiyield();
		if (-- n == 0)
		{
			PRINTF("G2D_MIXER->G2D_MIXER_CTL=%08X, G2D_MIXER->G2D_MIXER_INT=%08X\n", (unsigned) G2D_MIXER->G2D_MIXER_CTL, (unsigned) G2D_MIXER->G2D_MIXER_INT);
			return 0;
		}
	}
	return 1;
}

void arm_hardware_mdma_initialize(void)
{
	// https://github.com/lianghuixin/licee4.4/blob/bfee1d63fa355a54630244307296a00a973b70b0/linux-4.4/drivers/char/sunxi_g2d/g2d_bsp_v2.c
	//PRINTF("arm_hardware_mdma_initialize (G2D)\n");
	unsigned M = 2;	/* M = 1..32 */
	unsigned divider = 2;

	CCU->MBUS_CLK_REG |= (1u << 30);				// MBUS Reset 1: De-assert reset
	CCU->MBUS_MAT_CLK_GATING_REG |= (1u << 10);	// Gating MBUS Clock For G2D

	CCU->G2D_CLK_REG = (CCU->G2D_CLK_REG & ~ ((0x07u << 24) | (0x1Fu << 0))) |
		0x01 * (1u << 24) |	// 000: PLL_PERI(2X), 001: PLL_VIDEO0(4X), 010: PLL_VIDEO1(4X), 011: PLL_AUDIO1(DIV2)
		(M - 1) * (1u << 0) | // FACTOR_M
		0;
	CCU->G2D_CLK_REG |= (1u << 31);	// G2D_CLK_GATING

	//CCU->G2D_BGR_REG = 0;
	CCU->G2D_BGR_REG |= (1u << 0);		/* Enable gating clock for G2D 1: Pass */
	CCU->G2D_BGR_REG &= ~ (1u << 16);	/* G2D reset 0: Assert */
	CCU->G2D_BGR_REG |= (1u << 16);	/* G2D reset 1: De-assert */
	(void) CCU->G2D_BGR_REG;

	/* на Allwinner T113-S3 и F133 модифицируемы только младшие 8 бит */
	G2D_TOP->G2D_SCLK_DIV = (G2D_TOP->G2D_SCLK_DIV & ~ 0xFF) |
		divider * (1u << 4) |	// ROT divider (looks like power of 2) CORE1_SCLK_DIV
		divider * (1u << 0) |	// MIXER divider (looks like power of 2) CORE0_SCLK_DIV
		0;
	G2D_TOP->G2D_SCLK_GATE |= (1u << 1) | (1u << 0);	// Gate open: 0x02: rot, 0x01: mixer
	G2D_TOP->G2D_HCLK_GATE |= (1u << 1) | (1u << 0);	// Gate open: 0x02: rot, 0x01: mixer
	G2D_TOP->G2D_AHB_RESET &= ~ (1u << 1) | (1u << 0);	// Assert reset: 0x02: rot, 0x01: mixer
	G2D_TOP->G2D_AHB_RESET |= (1u << 1) | (1u << 0);	// De-assert reset: 0x02: rot, 0x01: mixer

	// peri:   allwnrt113_get_g2d_freq()=600000000
	// video0: allwnrt113_get_g2d_freq()=297000000
	// video1: allwnrt113_get_g2d_freq()=297000000
	// audio1: allwnrt113_get_g2d_freq()=768000000
	//PRINTF("allwnrt113_get_g2d_freq()=%u\n", (unsigned) allwnrt113_get_g2d_freq());

	 //mixer_set_reg_base(G2D_BASE);
	//PRINTF("arm_hardware_mdma_initialize (G2D) done.\n");
}

#endif /* WITHMDMAHW */

#if ! (LCDMODE_DUMMY || LCDMODE_HD44780)

#if LCDMODE_PIXELSIZE == 1
// Функция получает координаты и работает над буфером в горизонтальной ориентации.
static void
hwaccel_rect_u8(
	uint8_t * __restrict buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col,	// начальная координата
	uint_fast16_t row,	// начальная координата
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	uint_fast8_t color	// цвет
	)
{
	if (w == 0 || h == 0)
		return;
	enum { PIXEL_SIZE = sizeof * buffer };
	enum { PIXEL_SIZE_CODE = 0 };

#if WITHMDMAHW && (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)
	// MDMA implementation

	//static ALIGNX_BEGIN volatile uint8_t tgcolor [(DCACHEROWSIZE + sizeof (uint8_t) - 1) / sizeof (uint8_t)] ALIGNX_END;	/* значение цвета для заполнения области памяти */
	//tgcolor [0] = color;
	MDMA_DATA = color;	// регистр выделенного канала MDMA используется для хранения значение цвета. Переиферия не кэшируется.
	(void) MDMA_DATA;

	//dcache_clean((uintptr_t) & tgcolor, sizeof tgcolor);
	dcache_clean_invalidate((uintptr_t) buffer, PIXEL_SIZE * GXSIZE(dx, dy));

	MDMA_CH->CDAR = (uintptr_t) colpip_mem_at(buffer, dx, dy, col, row); // dest address
	MDMA_CH->CSAR = (uintptr_t) & MDMA_DATA;
	const uint_fast32_t tlen = mdma_tlen(w * PIXEL_SIZE, PIXEL_SIZE);
	const uint_fast32_t sbus = mdma_getbus(MDMA_CH->CSAR);
	const uint_fast32_t dbus = mdma_getbus(MDMA_CH->CDAR);
	const uint_fast32_t sinc = 0x00; // Source increment mode: 00: Source address pointer is fixed
	const uint_fast32_t dinc = 0x02; // Destination increment mode: 10: Destination address pointer is incremented
	const uint_fast32_t sburst = mdma_getburst(tlen, sbus, sinc);
	const uint_fast32_t dburst = mdma_getburst(tlen, dbus, dinc);
	MDMA_CH->CTCR =
		(sinc << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 00: Source address pointer is fixed
		(PIXEL_SIZE_CODE << MDMA_CTCR_SSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(dinc << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(PIXEL_SIZE_CODE << MDMA_CTCR_DSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00uL << MDMA_CTCR_PKE_Pos) |
		(0x00uL << MDMA_CTCR_PAM_Pos) |
		(0x02uL << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01uL << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01uL << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((PIXEL_SIZE * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00uL << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00uL << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((PIXEL_SIZE * (0)) << MDMA_CBRUR_SUV_Pos) |				// Source address Update Value
		((PIXEL_SIZE * (GXADJ(dx) - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(sbus << MDMA_CTBR_SBUS_Pos) |
		(dbus << MDMA_CTBR_DBUS_Pos) |
		0;

	mdma_startandwait();

#elif WITHMDMAHW && (CPUSTYLE_T113 || CPUSTYLE_F133)
	/* Использование G2D для формирования изображений */
	#warning Implement for (CPUSTYLE_T113 || CPUSTYLE_F133)

#else /* WITHMDMAHW */
	// программная реализация

	const unsigned dxadj = GXADJ(dx);
	uint8_t * tbuffer = colpip_mem_at(buffer, dx, dy, col, row); // dest address
	while (h --)
	{
		memset(tbuffer, color, w);
		tbuffer += dxadj;
	}

#endif /* WITHMDMAHW */
}

#endif /* LCDMODE_PIXELSIZE == 1 */
//
//uint32_t mixer_get_irq(void){
//	uint32_t reg_val = 0;
//
//	reg_val = read_wvalue(G2D_STATUS_REG);
//
//	return reg_val;
//}
//
//uint32_t mixer_get_irq0(void){
//	uint32_t reg_val = 0;
//
//	reg_val = read_wvalue(G2D_CMDQ_STS_REG);
//
//	return reg_val;
//}
//
//uint32_t mixer_clear_init(void){
//
//	G2D_V0->G2D_STATUS_REG, 0x300);
//	G2D_V0->G2D_CONTROL_REG, 0x0);
//
//	return 0;
//}
//
//uint32_t mixer_clear_init0(void){
//
//	G2D_V0->G2D_CMDQ_STS_REG, 0x100);
//	G2D_V0->G2D_CMDQ_CTL_REG, 0x0);
//
//	return 0;
//}

#if LCDMODE_PIXELSIZE == 2

// Функция получает координаты и работает над буфером в горизонтальной ориентации.
static void
hwaccel_rect_u16(
	uint16_t * __restrict buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col,	// начальная координата
	uint_fast16_t row,	// начальная координата
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	uint_fast32_t color	// цвет
	)
{
	if (w == 0 || h == 0)
		return;
	enum { PIXEL_SIZE = sizeof * buffer };
	enum { PIXEL_SIZE_CODE = 1 };

#if WITHMDMAHW && (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)
	// MDMA implementation

	//static ALIGNX_BEGIN volatile uint16_t tgcolor [DCACHEROWSIZE / sizeof (uint16_t)] ALIGNX_END;	/* значение цвета для заполнения области памяти */
	//tgcolor [0] = color;
	MDMA_DATA = color;	// регистр выделенного канала MDMA используется для хранения значение цвета. Переиферия не кэшируется.
	(void) MDMA_DATA;

	//dcache_clean((uintptr_t) & tgcolor, sizeof tgcolor);
	dcache_clean_invalidate((uintptr_t) buffer, PIXEL_SIZE * GXSIZE(dx, dy));

	MDMA_CH->CDAR = (uintptr_t) colpip_mem_at(buffer, dx, dy, col, row); // dest address
	MDMA_CH->CSAR = (uintptr_t) & MDMA_DATA;
	const uint_fast32_t tlen = mdma_tlen(w * PIXEL_SIZE, PIXEL_SIZE);
	const uint_fast32_t sbus = mdma_getbus(MDMA_CH->CSAR);
	const uint_fast32_t dbus = mdma_getbus(MDMA_CH->CDAR);
	const uint_fast32_t sinc = 0x00; // Source increment mode: 00: Source address pointer is fixed
	const uint_fast32_t dinc = 0x02; // Destination increment mode: 10: Destination address pointer is incremented
	const uint_fast32_t sburst = mdma_getburst(tlen, sbus, sinc);
	const uint_fast32_t dburst = mdma_getburst(tlen, dbus, dinc);
	MDMA_CH->CTCR =
		(sinc << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 00: Source address pointer is fixed
		(PIXEL_SIZE_CODE << MDMA_CTCR_SSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(dinc << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(PIXEL_SIZE_CODE << MDMA_CTCR_DSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00uL << MDMA_CTCR_PKE_Pos) |
		(0x00uL << MDMA_CTCR_PAM_Pos) |
		(0x02uL << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01uL << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01uL << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((PIXEL_SIZE * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00uL << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00uL << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((PIXEL_SIZE * (0)) << MDMA_CBRUR_SUV_Pos) |				// Source address Update Value
		((PIXEL_SIZE * (GXADJ(dx) - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(sbus << MDMA_CTBR_SBUS_Pos) |
		(dbus << MDMA_CTBR_DBUS_Pos) |
		0;

	//PRINTF("MDMA_CH->CDAR=%08X,dbus=%d, MDMA_CH->CSAR=%08X,sbus=%d, tlen=%u, BNDT-%08lX\n", MDMA_CH->CDAR, dbus, MDMA_CH->CSAR, sbus, tlen, (MDMA_CH->CBNDTR & MDMA_CBNDTR_BNDT_Msk) >> MDMA_CBNDTR_BNDT_Pos);
	mdma_startandwait();

#elif WITHDMA2DHW
	// DMA2D implementation

	// just writes the color defined in the DMA2D_OCOLR register
	// to the area located at the address pointed by the DMA2D_OMAR
	// and defined in the DMA2D_NLR and DMA2D_OOR.

	dcache_clean_invalidate((uintptr_t) buffer, sizeof (* buffer) * GXSIZE(dx, dy));

	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) & buffer [row * GXADJ(dx) + col];
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((GXADJ(dx) - w) << DMA2D_OOR_LO_Pos) |
		0;

	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		(h << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;

	DMA2D->OCOLR =
		color |
		0;

	DMA2D->OPFCCR = (DMA2D->OPFCCR & ~ (DMA2D_OPFCCR_CM)) |
		(2 * DMA2D_OPFCCR_CM_0) |	/* 010: RGB565 Color mode - framebuffer pixel format */
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		3 * DMA2D_CR_MODE_0 |	// 11: Register-to-memory (no FG nor BG, only output stage active)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		hardware_nonguiyield();
	__DMB();

	ASSERT((DMA2D->ISR & DMA2D_ISR_CEIF) == 0);	// Configuration Error
	ASSERT((DMA2D->ISR & DMA2D_ISR_TEIF) == 0);	// Transfer Error

#elif WITHMDMAHW && (CPUSTYLE_T113 || CPUSTYLE_F133)
	/* Использование G2D для формирования изображений */

	if (w == 1)
	{
		/* Горизонтальные линии в один пиксель рисовать умеет аппаратура. */
		/* программная реализация отрисовки вертикальной линии в один пиксель */
		const unsigned t = GXADJ(dx) - w;
		//buffer += (GXADJ(dx) * row) + col;
		volatile uint16_t * tbuffer = colpip_mem_at(buffer, dx, dy, col, row); // dest address
		while (h --)
		{
			unsigned n = w;
			while (n --)
				* tbuffer ++ = color;
			tbuffer += t;
		}
		return;
	}
	const COLOR24_T c24 = COLOR24(COLORPIP_R(color), COLORPIP_G(color), COLORPIP_B(color));

	ASSERT((G2D_MIXER->G2D_MIXER_CTL & (1uL << 31)) == 0);
	const unsigned tstride = GXADJ(dx) * PIXEL_SIZE;
	const uintptr_t taddr = (uintptr_t) colpip_mem_at(buffer, dx, dy, col, row);
	const uint_fast32_t tsizehw = ((h - 1) << 16) | ((w - 1) << 0);
	dcache_clean_invalidate((uintptr_t) buffer, PIXEL_SIZE * GXSIZE(dx, dy));

	t113_fillrect(taddr, tstride, tsizehw, COLORPIP_A(color), c24);


	//debug_g2d("my");
	G2D_MIXER->G2D_MIXER_CTL |= (1u << 31);	/* start the module */
	if (hwacc_waitdone() == 0)
	{
		PRINTF("hwaccel_rect_u16: timeout x/y, w/h: %u/%u, %u/%u\n", (unsigned) col, (unsigned) row, (unsigned) w, (unsigned) h);
		ASSERT(0);
	}
	ASSERT((G2D_MIXER->G2D_MIXER_CTL & (1u << 31)) == 0);

#else /* WITHMDMAHW, WITHDMA2DHW */
	// программная реализация
	const unsigned t = GXADJ(dx) - w;
	//buffer += (GXADJ(dx) * row) + col;
	volatile uint16_t * tbuffer = colpip_mem_at(buffer, dx, dy, col, row); // dest address
	while (h --)
	{
		unsigned n = w;
		while (n --)
			* tbuffer ++ = color;
		tbuffer += t;
	}

#endif /* WITHMDMAHW, WITHDMA2DHW */
}

#endif /* LCDMODE_PIXELSIZE == 2 */

#if LCDMODE_PIXELSIZE == 3
// Функция получает координаты и работает над буфером в горизонтальной ориентации.
static void
hwaccel_rect_u24(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col,	// начальная координата
	uint_fast16_t row,	// начальная координата
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	uint_fast32_t color	// цвет
	)
{
	if (w == 0 || h == 0)
		return;
	enum { PIXEL_SIZE = sizeof * buffer };
	//enum { PIXEL_SIZE_CODE = 1 };

	ASSERT(sizeof (* buffer) == 3);

#if 0 && WITHMDMAHW && (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)
	// MDMA implementation

	//static ALIGNX_BEGIN volatile PACKEDCOLORPIP_T tgcolor [(DCACHEROWSIZE + sizeof (PACKEDCOLORPIP_T) - 1) / sizeof (PACKEDCOLORPIP_T)] ALIGNX_END;	/* значение цвета для заполнения области памяти */
	//tgcolor [0] = color;
	MDMA_DATA = color;	// регистр выделенного канала MDMA используется для хранения значение цвета. Переиферия не кэшируется.
	(void) MDMA_DATA;
	#error MDMA implementation need

	//dcache_clean((uintptr_t) & tgcolor, sizeof tgcolor);
	dcache_clean_invalidate((uintptr_t) buffer, PIXEL_SIZE * GXSIZE(dx, dy));

	MDMA_CH->CDAR = (uintptr_t) colpip_mem_at(buffer, dx, dy, col, row); // dest address
	MDMA_CH->CSAR = (uintptr_t) & MDMA_DATA;
	const uint_fast32_t tlen = mdma_tlen(w * PIXEL_SIZE, PIXEL_SIZE);
	const uint_fast32_t sbus = mdma_getbus(MDMA_CH->CSAR);
	const uint_fast32_t dbus = mdma_getbus(MDMA_CH->CDAR);
	const uint_fast32_t sinc = 0x00; // Source increment mode: 00: Source address pointer is fixed
	const uint_fast32_t dinc = 0x02; // Destination increment mode: 10: Destination address pointer is incremented
	const uint_fast32_t sburst = mdma_getburst(tlen, sbus, sinc);
	const uint_fast32_t dburst = mdma_getburst(tlen, dbus, dinc);
	MDMA_CH->CTCR =
		(sinc << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 00: Source address pointer is fixed
		(PIXEL_SIZE_CODE << MDMA_CTCR_SSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(dinc << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(PIXEL_SIZE_CODE << MDMA_CTCR_DSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00uL << MDMA_CTCR_PKE_Pos) |
		(0x00uL << MDMA_CTCR_PAM_Pos) |
		(0x02uL << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01uL << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01uL << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((PIXEL_SIZE * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00uL << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00uL << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((PIXEL_SIZE * (0)) << MDMA_CBRUR_SUV_Pos) |				// Source address Update Value
		((PIXEL_SIZE * (GXADJ(dx) - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(sbus << MDMA_CTBR_SBUS_Pos) |
		(dbus << MDMA_CTBR_DBUS_Pos) |
		0;

	mdma_startandwait();

#elif WITHDMA2DHW
	// DMA2D implementation

	// just writes the color defined in the DMA2D_OCOLR register
	// to the area located at the address pointed by the DMA2D_OMAR
	// and defined in the DMA2D_NLR and DMA2D_OOR.

	dcache_clean_invalidate((uintptr_t) buffer, sizeof (* buffer) * GXSIZE(dx, dy));

	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) & buffer [row * GXADJ(dx) + col];
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((GXADJ(dx) - w) << DMA2D_OOR_LO_Pos) |
		0;

	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		(h << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;

	DMA2D->OCOLR =
		color |
		0;

	DMA2D->OPFCCR = (DMA2D->OPFCCR & ~ (DMA2D_OPFCCR_CM)) |
		(1 * DMA2D_OPFCCR_CM_0) |	/* 001: RGB888 Color mode - framebuffer pixel format */
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		3 * DMA2D_CR_MODE_0 |	// 11: Register-to-memory (no FG nor BG, only output stage active)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		hardware_nonguiyield();
	__DMB();

	ASSERT((DMA2D->ISR & DMA2D_ISR_CEIF) == 0);	// Configuration Error
	ASSERT((DMA2D->ISR & DMA2D_ISR_TEIF) == 0);	// Transfer Error

#elif WITHMDMAHW && (CPUSTYLE_T113 || CPUSTYLE_F133)
	/* Использование G2D для формирования изображений */
	#warinig Implement for (CPUSTYLE_T113 || CPUSTYLE_F133)
	const unsigned stride = GXADJ(dx);
	(void) G2D;

	ASSERT((G2D_MIXER->G2D_MIXER_CTL & (1uL << 31)) == 0);

#else
	// программная реализация

	const unsigned t = GXADJ(dx) - w;
	//buffer += (GXADJ(dx) * row) + col;
	volatile PACKEDCOLORPIP_T * tbuffer = colpip_mem_at(buffer, dx, dy, col, row); // dest address
	while (h --)
	{
		//PACKEDCOLORPIP_T * const startmem = buffer;

		unsigned n = w;
		while (n --)
			* tbuffer ++ = color;
		tbuffer += t;
	}

#endif
}

#endif /* LCDMODE_PIXELSIZE == 3 */

#if LCDMODE_PIXELSIZE == 4
// Функция получает координаты и работает над буфером в горизонтальной ориентации.
static void
hwaccel_rect_u32(
	uint32_t * __restrict buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col,	// начальная координата
	uint_fast16_t row,	// начальная координата
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	uint_fast32_t color24	// цвет
	)
{
	if (w == 0 || h == 0)
		return;
	enum { PIXEL_SIZE = sizeof * buffer };
	enum { PIXEL_SIZE_CODE = 2 };	// word (32-bit)

#if WITHMDMAHW && (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)
	// MDMA implementation

	//static ALIGNX_BEGIN volatile uint32_t tgcolor [(DCACHEROWSIZE + sizeof (uint32_t) - 1) / sizeof (uint32_t)] ALIGNX_END;	/* значение цвета для заполнения области памяти */
	//tgcolor [0] = color;
	MDMA_DATA = color24;	// регистр выделенного канала MDMA используется для хранения значение цвета. Переиферия не кэшируется.
	(void) MDMA_DATA;

	//dcache_clean((uintptr_t) & tgcolor, sizeof tgcolor);
	dcache_clean_invalidate((uintptr_t) buffer, PIXEL_SIZE * GXSIZE(dx, dy));

	MDMA_CH->CDAR = (uintptr_t) colpip_mem_at(buffer, dx, dy, col, row); // dest address
	MDMA_CH->CSAR = (uintptr_t) & MDMA_DATA;
	const uint_fast32_t tlen = mdma_tlen(w * PIXEL_SIZE, PIXEL_SIZE);
	const uint_fast32_t sbus = mdma_getbus(MDMA_CH->CSAR);
	const uint_fast32_t dbus = mdma_getbus(MDMA_CH->CDAR);
	const uint_fast32_t sinc = 0x00; // Source increment mode: 00: Source address pointer is fixed
	const uint_fast32_t dinc = 0x02; // Destination increment mode: 10: Destination address pointer is incremented
	const uint_fast32_t sburst = mdma_getburst(tlen, sbus, sinc);
	const uint_fast32_t dburst = mdma_getburst(tlen, dbus, dinc);
	MDMA_CH->CTCR =
		(sinc << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 00: Source address pointer is fixed
		(PIXEL_SIZE_CODE << MDMA_CTCR_SSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(dinc << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(PIXEL_SIZE_CODE << MDMA_CTCR_DSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00uL << MDMA_CTCR_PKE_Pos) |
		(0x00uL << MDMA_CTCR_PAM_Pos) |
		(0x02uL << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01uL << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01uL << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((PIXEL_SIZE * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00uL << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00uL << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((PIXEL_SIZE * (0)) << MDMA_CBRUR_SUV_Pos) |				// Source address Update Value
		((PIXEL_SIZE * (GXADJ(dx) - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(sbus << MDMA_CTBR_SBUS_Pos) |
		(dbus << MDMA_CTBR_DBUS_Pos) |
		0;

	mdma_startandwait();

#elif WITHDMA2DHW
	// DMA2D implementation

	// just writes the color defined in the DMA2D_OCOLR register
	// to the area located at the address pointed by the DMA2D_OMAR
	// and defined in the DMA2D_NLR and DMA2D_OOR.

	dcache_clean_invalidate((uintptr_t) buffer, sizeof (* buffer) * GXSIZE(dx, dy));

	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) & buffer [row * GXADJ(dx) + col];
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((GXADJ(dx) - w) << DMA2D_OOR_LO_Pos) |
		0;

	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		(h << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;

	DMA2D->OCOLR =
		color24 |
		0;

	DMA2D->OPFCCR = (DMA2D->OPFCCR & ~ (DMA2D_OPFCCR_CM)) |
		(2 * DMA2D_OPFCCR_CM_0) |	/* 010: RGB565 Color mode - framebuffer pixel format */
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		3 * DMA2D_CR_MODE_0 |	// 11: Register-to-memory (no FG nor BG, only output stage active)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		hardware_nonguiyield();
	__DMB();

	ASSERT((DMA2D->ISR & DMA2D_ISR_CEIF) == 0);	// Configuration Error
	ASSERT((DMA2D->ISR & DMA2D_ISR_TEIF) == 0);	// Transfer Error

#elif WITHMDMAHW && (CPUSTYLE_T113 || CPUSTYLE_F133)
	/* Использование G2D для формирования изображений */

	if (w == 1)
	{
		/* Горизонтальные линии в один пиксель рисовать умеет аппаратура. */
		/* программная реализация отрисовки вертикальной линии в один пиксель */
		const unsigned t = GXADJ(dx) - w;
		//buffer += (GXADJ(dx) * row) + col;
		volatile uint32_t * tbuffer = colpip_mem_at(buffer, dx, dy, col, row); // dest address
		while (h --)
		{
			unsigned n = w;
			while (n --)
				* tbuffer ++ = color24;
			tbuffer += t;
		}
		return;
	}
	dcache_clean_invalidate((uintptr_t) buffer, PIXEL_SIZE * GXSIZE(dx, dy));

	const uintptr_t taddr = (uintptr_t) colpip_mem_at(buffer, dx, dy, col, row);
	const unsigned tstride = GXADJ(dx) * PIXEL_SIZE;
	const uint_fast32_t tsizehw = ((h - 1) << 16) | ((w - 1) << 0);

	ASSERT((G2D_MIXER->G2D_MIXER_CTL & (1uL << 31)) == 0);

	t113_fillrect(taddr, tstride, tsizehw, COLORPIP_A(color24), (color24 & 0xFFFFFF));

	//PRINTF("G2D_MIXER->G2D_MIXER_CTL=%08X\n", G2D_MIXER->G2D_MIXER_CTL);
	G2D_MIXER->G2D_MIXER_CTL |= (1u << 31);	/* start the module */
	if (hwacc_waitdone() == 0)
	{
		PRINTF("hwaccel_rect_u32: timeout x/y, w/h: %u/%u, %u/%u\n", (unsigned) col, (unsigned) row, (unsigned) w, (unsigned) h);
		ASSERT(0);
	}
	//debug_g2d(__FILE__, __LINE__);
	ASSERT((G2D_MIXER->G2D_MIXER_CTL & (1u << 31)) == 0);

#else /* WITHMDMAHW, WITHDMA2DHW */
	// программная реализация
	const unsigned t = GXADJ(dx) - w;
	//buffer += (GXADJ(dx) * row) + col;
	volatile uint32_t * tbuffer = colpip_mem_at(buffer, dx, dy, col, row); // dest address
	while (h --)
	{
		unsigned n = w;
		while (n --)
			* tbuffer ++ = color24;
		tbuffer += t;
	}

#endif /* WITHMDMAHW, WITHDMA2DHW */
}

#endif /* LCDMODE_PIXELSIZE == 4 */

// получить адрес требуемой позиции в буфере
PACKEDCOLORPIP_T *
colmain_mem_at_debug(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * file,
	int line
	)
{
	if (x >= dx || y >= dy || buffer == NULL)
	{
		PRINTF("colpip_mem_at(%s/%d): dx=%u, dy=%u, x=%d, y=%d, savestring='%s', savewhere='%s'\n", file, line, dx, dy, x, y, savestring, savewhere);
	}
	ASSERT(x < dx);
	ASSERT(y < dy);
	ASSERT(buffer != NULL);
#if LCDMODE_HORFILL
	return & buffer [y * GXADJ(dx) + x];
#else /* LCDMODE_HORFILL */
	return & buffer [y * GXADJ(dx) + x];
#endif /* LCDMODE_HORFILL */
}


/// Нарисовать вертикальную цветную полосу
// Формат RGB565
void
display_colorbuf_xor_vline(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row0,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	COLORPIP_T color
	)
{
	ASSERT(row0 < dy);
	ASSERT((row0 + h) <= dy);
	while (h --)
		colpip_point_xor(buffer, dx, dy, col, row0 ++, color);
}

// Нарисовать вертикальную цветную полосу
// Формат RGB565
void
display_colorbuf_set_vline(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col,	// горизонтальная координата начального пикселя (0..dx-1) слева направо
	uint_fast16_t row0,	// вертикальная координата начального пикселя (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	COLORPIP_T color
	)
{
	ASSERT(row0 < dy);
	ASSERT((row0 + h) <= dy);
	/* рисуем прямоугольник шириной в 1 пиксель */
	//colmain_fillrect(buffer, dx, dy, col, row0, 1, h, color);
	while (h --)
		colpip_point(buffer, dx, dy, col, row0 ++, color);
}

// Нарисовать горизонтальную цветную полосу
// Формат RGB565
void
display_colorbuf_set_hline(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col0,	// горизонтальная координата начального пикселя (0..dx-1) слева направо
	uint_fast16_t row0,	// вертикальная координата начального пикселя (0..dy-1) сверху вниз
	uint_fast16_t w,	// ширина
	COLORPIP_T color
	)
{
	ASSERT(row0 < dy);
	ASSERT((col0 + w) <= dx);
	/* рисуем прямоугольник высотой в 1 пиксель */
	//colmain_fillrect(buffer, dx, dy, col0, row0, w, 1, color);
	while (w --)
		colpip_point(buffer, dx, dy, col0 ++, row0, color);
}

// заполнение прямоугольной области в видеобуфере
void colpip_fillrect(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// начальная координата
	uint_fast16_t y,	// начальная координата
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	COLORPIP_T color	// цвет
	)
{
	ASSERT(x < dx);
	ASSERT((x + w) <= dx);
	ASSERT(y < dy);
	ASSERT((y + h) <= dy);

#if LCDMODE_HORFILL

	#if LCDMODE_PIP_L8
		hwaccel_rect_u8(buffer, dx, dy, x, y, w, h, color);

	#elif LCDMODE_PIP_RGB565
		hwaccel_rect_u16(buffer, dx, dy, x, y, w, h, color);

	#elif LCDMODE_PIP_L24
		hwaccel_rect_u24(buffer, dx, dy, x, y, w, h, color);
	#elif LCDMODE_MAIN_L8
		hwaccel_rect_u8(buffer, dx, dy, x, y, w, h, color);

	#elif LCDMODE_MAIN_RGB565
		hwaccel_rect_u16(buffer, dx, dy, x, y, w, h, color);

	#elif LCDMODE_MAIN_L24
		hwaccel_rect_u24(buffer, dx, dy, x, y, w, h, color);

	#elif LCDMODE_MAIN_ARGB888
		hwaccel_rect_u32(buffer, dx, dy, x, y, w, h, color);

	#endif

#else /* LCDMODE_HORFILL */

	#if LCDMODE_PIP_L8
		hwaccel_rect_u8(buffer, dy, dx, y, x, h, w, color);

	#elif LCDMODE_PIP_RGB565
		hwaccel_rect_u16(buffer, dy, dx, y, x, h, w, color);

	#elif LCDMODE_PIP_L24
		hwaccel_rect_u24((buffer, dy, dx, y, x, h, w, color);

	#elif LCDMODE_MAIN_L8
		hwaccel_rect_u8(buffer, dy, dx, y, x, h, w, color);

	#elif LCDMODE_MAIN_RGB565
		hwaccel_rect_u16(buffer, dy, dx, y, x, h, w, color);

	#elif LCDMODE_MAIN_L24
		hwaccel_rect_u24((buffer, dy, dx, y, x, h, w, color);

	#elif LCDMODE_MAIN_ARGB888
		hwaccel_rect_u32((buffer, dy, dx, y, x, h, w, color);

	#endif

#endif /* LCDMODE_HORFILL */
}



void colmain_putpixel(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	)
{
	ASSERT(x < dx);
	ASSERT(y < dy);
	PACKEDCOLORPIP_T * const tgr = colpip_mem_at(buffer, dx, dy, x, y);
	#if LCDMODE_LTDC_L24
		tgr->r = color >> 16;
		tgr->g = color >> 8;
		tgr->b = color >> 0;
	#else /* LCDMODE_LTDC_L24 */
		* tgr = color;
	#endif /* LCDMODE_LTDC_L24 */
}

/*-----------------------------------------------------  V_Bre
 * void V_Bre (int xn, int yn, int xk, int yk)
 *
 * Подпрограмма иллюстрирующая построение вектора из точки
 * (xn,yn) в точку (xk, yk) методом Брезенхема.
 *
 * Построение ведется от точки с меньшими  координатами
 * к точке с большими координатами с единичным шагом по
 * координате с большим приращением.
 *
 * В общем случае исходный вектор проходит не через вершины
 * растровой сетки, а пересекает ее стороны.
 * Пусть приращение по X больше приращения по Y и оба они > 0.
 * Для очередного значения X нужно выбрать одну двух ближайших
 * координат сетки по Y.
 * Для этого проверяется как проходит  исходный  вектор - выше
 * или ниже середины расстояния между ближайшими значениями Y.
 * Если выше середины,  то Y-координату  надо  увеличить на 1,
 * иначе оставить прежней.
 * Для этой проверки анализируется знак переменной s,
 * соответствующей разности между истинным положением и
 * серединой расстояния между ближайшими Y-узлами сетки.
 */

void colmain_line(
	PACKEDCOLORPIP_T * buffer,
	const uint_fast16_t bx,	// ширина буфера
	const uint_fast16_t by,	// высота буфера
	int xn, int yn,
	int xk, int yk,
	COLORPIP_T color,
	int antialiasing
	)
{
	ASSERT(xn < bx);
	ASSERT(xk < bx);
	ASSERT(yn < by);
	ASSERT(yn < by);
	int  dx, dy, s, sx, sy, kl, incr1, incr2;
	char swap;
	const COLORPIP_T sc = getshadedcolor(color, DEFAULT_ALPHA);

	/* Вычисление приращений и шагов */
	if ((dx = xk - xn) < 0)
	{
		dx = - dx;
		sx = - 1;
	}
	else if (dx > 0)
		sx = + 1;
	else
		sx = 0;

	if ((dy = yk - yn) < 0)
	{
		dy = - dy;
		sy = - 1;
	}
	else if (dy > 0)
		sy = + 1;
	else
		sy = 0;

	/* Учет наклона */
	if ((kl = dx) < (s = dy))
	{
		dx = s;  dy = kl;  kl = s; swap = 1;
	}
	else
	{
		swap = 0;
	}

	s = (incr1 = 2 * dy) - dx; /* incr1 - констан. перевычисления */
	/* разности если текущее s < 0  и  */
	/* s - начальное значение разности */
	incr2 = 2 * dx;         /* Константа для перевычисления    */
	/* разности если текущее s >= 0    */
	colmain_putpixel(buffer, bx, by, xn, yn, color); /* Первый  пиксел вектора       */

	/*static */ uint_fast16_t xold, yold;
	xold = xn;
	yold = yn;
	while (-- kl >= 0)
	{
		if (s >= 0)
		{
			if (swap)
				xn += sx;
			else
				yn += sy;
			s-= incr2;
		}
		if (swap)
			yn += sy;
		else
			xn += sx;
		s += incr1;

		colmain_putpixel(buffer, bx, by, xn, yn, color); /* Текущая  точка  вектора   */

		if (antialiasing)
		{
			if (((xold == xn - 1) || (xold == xn + 1)) && ((yold == yn - 1) || (yold == yn + 1)))
			{
				if (color != * colpip_mem_at(buffer, bx, by, xn, yold))
					colmain_putpixel(buffer, bx, by, xn, yold, sc);

				if (color != * colpip_mem_at(buffer, bx, by, xold, yn))
					colmain_putpixel(buffer, bx, by, xold, yn, sc);
//				colmain_putpixel(buffer, bx, by, xn, yn, sc);		// нужны дополнительные цвета для этих 2х точек
//				colmain_putpixel(buffer, bx, by, xold, yold, sc);
			}
			xold = xn;
			yold = yn;
		}
	}
}  /* V_Bre */


/* заливка замкнутого контура */
void display_floodfill(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// начальная координата
	uint_fast16_t y,	// начальная координата
	COLORPIP_T newColor,
	COLORPIP_T oldColor,
	uint_fast8_t type	// 0 - быстрая закраска (только выпуклый контур), 1 - медленная закраска любого контура
	)
{
	ASSERT(x < dx);
	ASSERT(y < dy);
	PACKEDCOLORPIP_T * tgr = colpip_mem_at(buffer, dx, dy, x, y);

	if (type) 	// медленная закраска любого контура
	{

		if (* tgr == oldColor && * tgr != newColor)
		{
			* tgr = newColor;
			display_floodfill(buffer, dx, dy, x + 1, y, newColor, oldColor, 1);
			display_floodfill(buffer, dx, dy, x - 1, y, newColor, oldColor, 1);
			display_floodfill(buffer, dx, dy, x, y + 1, newColor, oldColor, 1);
			display_floodfill(buffer, dx, dy, x, y - 1, newColor, oldColor, 1);
		}
	}
	else 		// быстрая закраска (только выпуклый контур)
	{
		uint_fast16_t y0 = y, x_l = x, x_p = x;

		while(* tgr != newColor)		// поиск первой строки в контуре для закраски
		{
			tgr = colpip_mem_at(buffer, dx, dy, x, --y0);
		}
		y0++;

		do
		{
			x_l = x;		// добавить проверку на необходимость поиска новых границ
			x_p = x;

			// поиск левой границы строки
			do
			{
				tgr = colpip_mem_at(buffer, dx, dy, --x_l, y0);
			} while(* tgr != newColor);

			// поиск правой границы строки
			do
			{
				tgr = colpip_mem_at(buffer, dx, dy, ++x_p, y0);
			} while(* tgr != newColor);

			// закраска найденной линии
			colmain_line(buffer, dx, dy, x_l, y0, x_p, y0, newColor, 0);

			// переход на следующую строку
			tgr = colpip_mem_at(buffer, dx, dy, x, ++y0);
		} while(* tgr != newColor);
	}
}

// Заполнение буфера сполшным цветом
// Эта функция используется только в тесте
void colpip_fill(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	COLORPIP_T color
	)
{
#if LCDMODE_HORFILL

	#if LCDMODE_PIP_L8
		hwaccel_rect_u8(buffer, dx, dy, 0, 0, dx, dy, color);

	#elif LCDMODE_PIP_RGB565
		hwaccel_rect_u16(buffer, dx, dy, 0, 0, dx, dy, color);

	#elif LCDMODE_PIP_L24
		hwaccel_rect_u24(buffer, dx, dy, 0, 0, dx, dy, color);

	#elif LCDMODE_MAIN_L8
		hwaccel_rect_u8(buffer, dx, dy, 0, 0, dx, dy, color);

	#elif LCDMODE_MAIN_RGB565
		hwaccel_rect_u16(buffer, dx, dy, 0, 0, dx, dy, color);

	#elif LCDMODE_MAIN_L24
		hwaccel_rect_u24(buffer, dx, dy, 0, 0, dx, dy, color);

	#elif LCDMODE_MAIN_ARGB888
		hwaccel_rect_u32(buffer, dx, dy, 0, 0, dx, dy, color);

	#endif

#else /* LCDMODE_HORFILL */

	#if LCDMODE_PIP_L8
		hwaccel_rect_u8(buffer, dy, dx, 0, 0, dy, dx, color);

	#elif LCDMODE_PIP_RGB565
		hwaccel_rect_u16(buffer, dy, dx, 0, 0, dy, dx, color);

	#elif LCDMODE_PIP_L24
		hwaccel_rect_u24(buffer, dy, dx, 0, 0, dy, dx, color);

	#elif LCDMODE_MAIN_L8
		hwaccel_rect_u8(buffer, dy, dx, 0, 0, dy, dx, color);

	#elif LCDMODE_MAIN_RGB565
		hwaccel_rect_u16(buffer, dy, dx, 0, 0, dy, dx, color);

	#elif LCDMODE_MAIN_L24
		hwaccel_rect_u24(buffer, dy, dx, 0, 0, dy, dx, color);

	#elif LCDMODE_MAIN_ARGB888
		hwaccel_rect_u32(buffer, dy, dx, 0, 0, dy, dx, color);

	#endif

#endif /* LCDMODE_HORFILL */
}

// Заполнение буфера сполшным цветом
// Эта функция используется только в тесте
void gtg_fill(const GTG_t * gtg, COLORPIP_T color)
{
	colpip_fill(gtg->buffer, gtg->dx, gtg->dy, color);
}


// поставить цветную точку.
void colpip_point(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	)
{
	* colpip_mem_at(buffer, dx, dy, col, row) = color;
}

// поставить цветную точку.
void gtg_point(
	const GTG_t * gtg,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	)
{
	* colpip_mem_at(gtg->buffer, gtg->dx, gtg->dy, col, row) = color;
}

// поставить цветную точку (модификация с сохранением старого изоьражения).
void colpip_point_xor(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	)
{
	* colpip_mem_at(buffer, dx, dy, col, row) ^= color;
}


// копирование в большее или равное окно
// размер пикселя - определяется конфигурацией.
// MDMA, DMA2D или программа
// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
void hwaccel_bitblt(
	uintptr_t dstinvalidateaddr,	// параметры invalidate получателя
	int_fast32_t dstinvalidatesize,
	PACKEDCOLORPIP_T * __restrict dst,
	uint_fast16_t tdx,	// ширина буфера
	uint_fast16_t tdy,	// высота буфера
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const PACKEDCOLORPIP_T * __restrict src,
	uint_fast16_t sdx,	// ширина буфера
	uint_fast16_t sdy,	// высота буфера
	unsigned keyflag, COLOR24_T keycolor
	)
{
	if (sdx == 0 || sdy == 0)
		return;

#if WITHMDMAHW && (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)
	// MDMA реализация

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	dcache_clean(srcinvalidateaddr, srcinvalidatesize);

	MDMA_CH->CDAR = (uintptr_t) dst;
	MDMA_CH->CSAR = (uintptr_t) src;
	const uint_fast32_t tlen = mdma_tlen(sdx * sizeof (PACKEDCOLORPIP_T), sizeof (PACKEDCOLORPIP_T));
	const uint_fast32_t sbus = mdma_getbus(MDMA_CH->CSAR);
	const uint_fast32_t dbus = mdma_getbus(MDMA_CH->CDAR);
	const uint_fast32_t sinc = 0x02; // Source increment mode: 10: address pointer is incremented
	const uint_fast32_t dinc = 0x02; // Destination increment mode: 10: Destination address pointer is incremented
	const uint_fast32_t sburst = mdma_getburst(tlen, sbus, sinc);
	const uint_fast32_t dburst = mdma_getburst(tlen, dbus, dinc);
	MDMA_CH->CTCR =
		(sinc << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 10: address pointer is incremented
		(MDMA_CTCR_xSIZE_MAIN << MDMA_CTCR_SSIZE_Pos) |
		(MDMA_CTCR_xSIZE_MAIN << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(dinc << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(MDMA_CTCR_xSIZE_MAIN << MDMA_CTCR_DSIZE_Pos) |
		(MDMA_CTCR_xSIZE_MAIN << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00uL << MDMA_CTCR_PKE_Pos) |
		(0x00uL << MDMA_CTCR_PAM_Pos) |
		(0x02uL << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01uL << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01uL << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((sizeof (PACKEDCOLORPIP_T) * (sdx)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00uL << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00uL << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((sdy - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((sizeof (PACKEDCOLORPIP_T) * (GXADJ(sdx) - sdx)) << MDMA_CBRUR_SUV_Pos) |		// Source address Update Value
		((sizeof (PACKEDCOLORPIP_T) * (GXADJ(tdx) - sdx)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(sbus << MDMA_CTBR_SBUS_Pos) |
		(dbus << MDMA_CTBR_DBUS_Pos) |
		0;

	mdma_startandwait();

#elif WITHDMA2DHW
	// DMA2D реализация
	// See DMA2D_FGCMAR for L8
	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	dcache_clean(srcinvalidateaddr, srcinvalidatesize);

	/* исходный растр */
	DMA2D->FGMAR = (uintptr_t) src;
	//	The line offset used for the foreground image, expressed in pixel when the LOM bit is
	//	reset and in byte when the LOM bit is set.
	DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
		((GXADJ(sdx) - sdx) << DMA2D_FGOR_LO_Pos) |
		0;
	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) dst;
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((GXADJ(tdx) - sdx) << DMA2D_OOR_LO_Pos) |
		0;
	/* размер пересылаемого растра */
	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		((sdy) << DMA2D_NLR_NL_Pos) |
		((sdx) << DMA2D_NLR_PL_Pos) |
		0;
	/* формат пикселя */
	DMA2D->FGPFCCR = (DMA2D->FGPFCCR & ~ (DMA2D_FGPFCCR_CM | DMA2D_FGPFCCR_AM)) |
		0 * DMA2D_FGPFCCR_AM |
		DMA2D_FGPFCCR_CM_VALUE_MAIN |	/* Color mode - framebuffer pixel format */
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE | DMA2D_CR_LOM)) |
		0 * DMA2D_CR_LOM | // 0: Line offsets are expressed in pixels
		0 * DMA2D_CR_MODE_0 |	// 000: Memory-to-memory (FG fetch only)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		hardware_nonguiyield();

	ASSERT((DMA2D->ISR & DMA2D_ISR_CEIF) == 0);	// Configuration Error
	ASSERT((DMA2D->ISR & DMA2D_ISR_TEIF) == 0);	// Transfer Error

	__DMB();

#elif WITHMDMAHW && (CPUSTYLE_T113 || CPUSTYLE_F133)
	/* Копирование - использование G2D для формирования изображений */

//	PRINTF("hwaccel_bitblt: tdx/tdy, sdx/sdy: %u/%u, %u/%u\n", (unsigned) tdx, (unsigned) tdy, (unsigned) sdx, (unsigned) sdy);
//	ASSERT(sdx > 1 && sdy > 1);
//	ASSERT(sdx > 2 && sdy > 2);

	enum { PIXEL_SIZE = sizeof * src };
	const unsigned tstride = GXADJ(tdx) * PIXEL_SIZE;
	const unsigned sstride = GXADJ(sdx) * PIXEL_SIZE;
	const uintptr_t taddr = (uintptr_t) dst;
	const uintptr_t saddr = (uintptr_t) src;
	const uint_fast32_t sizehw = ((sdy - 1) << 16) | ((sdx - 1) << 0);

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	dcache_clean(srcinvalidateaddr, srcinvalidatesize);

	//	memset(G2D_V0, 0, sizeof * G2D_V0);
	//	memset(G2D_UI0, 0, sizeof * G2D_UI0);
	//	memset(G2D_UI1, 0, sizeof * G2D_UI1);
	//	memset(G2D_UI2, 0, sizeof * G2D_UI2);
	//	memset(G2D_BLD, 0, sizeof * G2D_BLD);
	//	memset(G2D_WB, 0, sizeof * G2D_WB);

	ASSERT((G2D_MIXER->G2D_MIXER_CTL & (1uL << 31)) == 0);

	/* Отключаем все источники */
	G2D_V0->V0_ATTCTL = 0;
	G2D_UI0->UI_ATTR = 0;
	G2D_UI1->UI_ATTR = 0;
	G2D_UI2->UI_ATTR = 0;
	G2D_BLD->BLD_FILL_COLOR_CTL = 0;	// Нет источников


	if ((keyflag & PLTPARAM_CKEY) != 0)
	{
		G2D_UI2->UI_ATTR = awxx_get_ui_attr();
		G2D_UI2->UI_PITCH = sstride;
		G2D_UI2->UI_FILLC = 0;//TFTRGB(255, 0, 0);	// unused
		G2D_UI2->UI_COOR = 0;			// координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
		G2D_UI2->UI_MBSIZE = sizehw; // сколько брать от исходного буфера
		G2D_UI2->UI_SIZE = sizehw;		// параметры окна исходного буфера
		G2D_UI2->UI_LADD = saddr;
		G2D_UI2->UI_HADD = saddr >> 32;
		//	G2D_BLD->BLD_EN_CTL |= (1u << 8);	// 8: P0_EN Pipe0 enable
		//	G2D_BLD->BLD_EN_CTL |= (1u << 0);	// 1: P0_FCEN
		//	G2D_BLD->ROP_CTL = 0x55F0;	// 0x00F0 G2D_V0, 0x55F0 UI1, 0xAAF0 UI2

		G2D_V0->V0_ATTCTL = awxx_get_vi_attr();
		G2D_V0->V0_PITCH0 = tstride;
		G2D_V0->V0_FILLC = 0;//TFTRGB(255, 0, 0);	// unused
		G2D_V0->V0_COOR = 0;			// координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
		G2D_V0->V0_MBSIZE = sizehw; 	// сколько брать от исходного буфера
		G2D_V0->V0_SIZE = sizehw;		// параметры окна исходного буфера
		G2D_V0->V0_LADD0 = taddr;
		G2D_V0->V0_HADD = 0;//  //$$$$$((saddr >> 32) & 0xFF) << 0;

	}
	else
	{
		G2D_V0->V0_ATTCTL = awxx_get_vi_attr();
		G2D_V0->V0_PITCH0 = sstride;
		G2D_V0->V0_FILLC = 0;//TFTRGB(255, 0, 0);	// unused
		G2D_V0->V0_COOR = 0;			// координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
		G2D_V0->V0_MBSIZE = sizehw; 	// сколько брать от исходного буфера
		G2D_V0->V0_SIZE = sizehw;		// параметры окна исходного буфера
		G2D_V0->V0_LADD0 = saddr;
		G2D_V0->V0_HADD = 0;//  //$$$$$((saddr >> 32) & 0xFF) << 0;

	}

	if ((keyflag & PLTPARAM_CKEY) != 0)
	{
		/* 5.10.9.10 BLD color key control register */
		G2D_BLD->BLD_KEY_CTL = 0x03;	/* G2D_CK_SRC = 0x03, G2D_CK_DST = 0x01 */

		/* 5.10.9.11 BLD color key configuration register */
		//G2D_BLD->BLD_KEY_CON = 0x07;
		G2D_BLD->BLD_KEY_CON= 0x00;

		G2D_BLD->BLD_KEY_MAX = keycolor;
		G2D_BLD->BLD_KEY_MIN = keycolor;

		G2D_BLD->BLD_FILL_COLOR_CTL |= (1u << 8);	// 8: P0_EN Pipe0 enable
		G2D_BLD->BLD_FILL_COLOR_CTL |= (1u << 9);	// 9: P1_EN Pipe1 enable


		G2D_BLD->BLD_CH_ISIZE [0] = sizehw;
		G2D_BLD->BLD_CH_OFFSET [0] = 0;// ((row) << 16) | ((col) << 0);
		G2D_BLD->BLD_CH_ISIZE [1] = sizehw;
		G2D_BLD->BLD_CH_OFFSET [1] = 0;// ((row) << 16) | ((col) << 0);
	}
	else
	{
		G2D_BLD->BLD_FILL_COLOR_CTL |= (1u << 8);	// 8: P0_EN Pipe0 enable

		G2D_BLD->BLD_CH_ISIZE [0] = sizehw;
		G2D_BLD->BLD_CH_OFFSET [0] = 0;// ((row) << 16) | ((col) << 0);

		G2D_BLD->BLD_KEY_CTL = 0;
		G2D_BLD->BLD_KEY_CON = 0;
	}

	G2D_BLD->ROP_CTL = 0x00F0;	// 0x00F0 G2D_V0, 0x55F0 UI1, 0xAAF0 UI2
	//G2D_BLD->BLD_CTL = 0x00010001;	// G2D_BLD_COPY
	//G2D_BLD->BLD_CTL = 0x00000000;	// G2D_BLD_CLEAR
	G2D_BLD->BLD_CTL = 0x03010301;	// G2D_BLD_SRCOVER - default value

	G2D_BLD->BLD_SIZE = sizehw;	// ! may not be zero

	//G2D_BLD->BLD_FILLC0 = ~ 0;
	//G2D_BLD->BLD_PREMUL_CTL |= (1u << 0);	// 0 or 1 - sel 1 or sel 0
	G2D_BLD->BLD_OUT_COLOR=0x00000000; /* 0x00000000 */
	G2D_BLD->BLD_CSC_CTL=0x00000000; /* 0x00000000 */
	G2D_BLD->BLD_BK_COLOR = 0;
	G2D_BLD->BLD_PREMUL_CTL=0x00000000; /* 0x00000000 */

	/* Write-back settings */
	G2D_WB->WB_ATT = WB_DstImageFormat;//G2D_FMT_RGB565; //G2D_FMT_XRGB8888;
	G2D_WB->WB_SIZE = sizehw;
	G2D_WB->WB_PITCH0 = tstride;	/* taddr buffer stride */
	G2D_WB->WB_LADD0 = taddr;
	G2D_WB->WB_HADD0 = taddr >> 32;

	G2D_MIXER->G2D_MIXER_CTL |= (1u << 31);	/* start the module */
	if (hwacc_waitdone() == 0)
	{
		PRINTF("hwaccel_bitblt: timeout tdx/tdy, sdx/sdy: %u/%u, %u/%u\n", (unsigned) tdx, (unsigned) tdy, (unsigned) sdx, (unsigned) sdy);
		ASSERT(0);
	}
	//debug_g2d(__FILE__, __LINE__);
	ASSERT((G2D_MIXER->G2D_MIXER_CTL & (1u << 31)) == 0);

#else
	// программная реализация

	if ((keyflag & PLTPARAM_CKEY) != 0)
	{
		// для случая когда горизонтальные пиксели в видеопямяти источника располагаются подряд
		// работа с color key

		const unsigned stail = GXADJ(sdx) - sdx;
		const unsigned dtail = GXADJ(tdx) - sdx;
		while (sdy --)
		{
			unsigned w = sdx;
			while (w --)
			{
				const COLORPIP_T c = * src ++;
				if (c != keycolor)
					* dst = c;
				dst ++;
			}
			src += stail;
			dst += dtail;
		}
	}
	else
	{
		// для случая когда горизонтальные пиксели в видеопямяти источника располагаются подряд
		if (tdx == sdx)
		{
			const size_t len = (size_t) GXSIZE(sdx, sdy) * sizeof * src;
			// ширина строки одинаковая в получателе и источнике
			memcpy(dst, src, len);
		}
		else
		{
			const size_t len = sdx * sizeof * src;
			while (sdy --)
			{
				memcpy(dst, src, len);
				//dcache_clean((uintptr_t) dst, len);
				src += GXADJ(sdx);
				dst += GXADJ(tdx);
			}
		}
	}

#endif
}

// копирование буфера с поворотом вправо на 90 градусов (четверть оборота).
void hwaccel_ra90(
	PACKEDCOLORPIP_T * __restrict tbuffer,
	uint_fast16_t tdx,	// размер получателя
	uint_fast16_t tdy,
	uint_fast16_t tx,	// горизонтальная координата пикселя (0..dx-1) слева направо - в исходном нижний
	uint_fast16_t ty,	// вертикальная координата пикселя (0..dy-1) сверху вниз - в исходном левый
	const PACKEDCOLORPIP_T * __restrict sbuffer,
	uint_fast16_t sdx,	// размер источника
	uint_fast16_t sdy
	)
{
	if (sdx == 0 || sdy == 0)
		return;

	uint_fast16_t x;	// x получателя
	for (x = 0; x < sdy; ++ x)
	{
		uint_fast16_t y;	// y получателя
		for (y = 0; y < sdx; ++ y)
		{
			const COLORPIP_T pixel = * colpip_mem_at((PACKEDCOLORPIP_T *) sbuffer, sdx, sdy, y, sdy - 1 - x);	// выборка из исхолного битмапа
			* colpip_mem_at(tbuffer, tdx, tdy, tx + x, ty + y) = pixel;
		}
	}
}


// Routine to draw a line in the RGB565 color to the LCD.
// The line is drawn from (xmin,ymin) to (xmax,ymax).
// The algorithm used to draw the line is "Bresenham's line
// algorithm".
#define SWAP(a, b)  do { (a) ^= (b); (b) ^= (a); (a) ^= (b); } while (0)
// Нарисовать линию указанным цветом
void colpip_line(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x0,
	uint_fast16_t y0,
	uint_fast16_t x1,
	uint_fast16_t y1,
	COLORPIP_T color
	)
{
	int xmin = x0;
	int xmax = x1;
	int ymin = y0;
	int ymax = y1;
	int Dx = xmax - xmin;
	int Dy = ymax - ymin;
	int steep = (abs(Dy) >= abs(Dx));
	if (steep) {
	   SWAP(xmin, ymin);
	   SWAP(xmax, ymax);
	   // recompute Dx, Dy after swap
	   Dx = xmax - xmin;
	   Dy = ymax - ymin;
	}
	int xstep = 1;
	if (Dx < 0) {
	   xstep = -1;
	   Dx = -Dx;
	}
	int ystep = 1;
	if (Dy < 0) {
	   ystep = -1;
	   Dy = -Dy;
	}

   int TwoDy = 2 * Dy;
   int TwoDyTwoDx = TwoDy - 2 * Dx; // 2*Dy - 2*Dx
   int E = TwoDy - Dx; //2*Dy - Dx
   int y = ymin;
   int xDraw, yDraw;
   int x;
   for (x = xmin; x != xmax; x += xstep) {
       if (steep) {
           xDraw = y;
           yDraw = x;
       } else {
           xDraw = x;
           yDraw = y;
       }
       // plot
	   colpip_point(buffer, dx, dy, xDraw, yDraw, color);
       // next
       if (E > 0) {
           E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
           y = y + ystep;
       } else {
           E += TwoDy; //E += 2*Dy;
       }
   }
}

#undef SWAP

// Нарисовать закрашенный или пустой прямоугольник
void colpip_rect(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// размер буфера
	uint_fast16_t dy,	// размер буфера
	uint_fast16_t x1,	// начальная координата
	uint_fast16_t y1,	// начальная координата
	uint_fast16_t x2,	// конечная координата (включена в заполняемую облсть)
	uint_fast16_t y2,	// конечная координата (включена в заполняемую облсть)
	COLORPIP_T color,
	uint_fast8_t fill
	)
{
	ASSERT(x2 > x1);
	ASSERT(y2 > y1);
	ASSERT(x2 < dx);
	ASSERT(y2 < dy);

	if (fill != 0)
	{
		const uint_fast16_t w = x2 - x1 + 1;	// размер по горизонтали
		const uint_fast16_t h = y2 - y1 + 1;	// размер по вертикали

		ASSERT((x1 + w) <= dx);
		ASSERT((y1 + h) <= dy);

		if (w < 3 || h < 3)
			return;

		colpip_fillrect(buffer, dx, dy, x1, y1, w, h, color);
	}
	else
	{
		colpip_line(buffer, dx, dy, x1, y1, x2, y1, color);		// верхняя горизонталь
		colpip_line(buffer, dx, dy, x1, y2, x2, y2, color);		// нижняя горизонталь
		colpip_line(buffer, dx, dy, x1, y1, x1, y2, color);		// левая вертикаль
		colpip_line(buffer, dx, dy, x2, y1, x2, y2, color);		// правая вертикаль
	}
}

#if WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC

static uint_fast8_t snapshot_req;
/* запись видимого изображения */
void
display_snapshot(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy
	)
{
	if (snapshot_req != 0)
	{
		snapshot_req = 0;
		/* запись файла */
		display_snapshot_write(buffer, dx, dy);
	}
}

void display_snapshot_req(void)
{
	snapshot_req = 1;
}

#else /* WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC */
/* stub */
/* запись видимого изображения */
void
display_snapshot(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy
	)
{
}

/* stub */
void display_snapshot_req(void)
{
}
#endif /* WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC */

void
colmain_fillrect(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLORPIP_T fgcolor
	)
{
	//PRINTF("colmain_fillrect: x/y, w/h: %u/%u, %u/%u, color=%08lX\n", x, y, w, h, fgcolor);

#if LCDMODE_HORFILL

	#if LCDMODE_MAIN_L8
		hwaccel_rect_u8(buffer, dx, dy, x, y, w, h, fgcolor);
	#elif LCDMODE_LTDC_L24
		hwaccel_rect_u24(buffer, dx, dy, x, y, w, h, fgcolor);
	#elif LCDMODE_MAIN_ARGB888
		hwaccel_rect_u32(buffer, dx, dy, x, y, w, h, fgcolor);
	#else
		hwaccel_rect_u16(buffer, dx, dy, x, y, w, h, fgcolor);
	#endif

#else /* LCDMODE_HORFILL */

	#if LCDMODE_MAIN_L8
		hwaccel_rect_u8(buffer, dy, dx, y, x, h, w, fgcolor);
	#elif LCDMODE_LTDC_L24
		hwaccel_rect_u24(buffer, dy, dx, y, x, h, w, fgcolor);
	#elif LCDMODE_MAIN_ARGB888
		hwaccel_rect_u32(buffer, dy, dx, y, x, h, w, fgcolor);
	#else
		hwaccel_rect_u16(buffer, dy, dx, y, x, h, w, fgcolor);
	#endif

#endif /* LCDMODE_HORFILL */
}

#if LCDMODE_HORFILL
// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
#if 0
// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
static void RAMFUNC colorpip_pixels(
	PACKEDCOLORPIP_T * __restrict tgr,		// target raster
	const FLASHMEM uint8_t * __restrict raster,
	uint_fast16_t width	// number of bits (start from LSB first byte in raster)
	)
{
	uint_fast16_t col;
	uint_fast16_t w = width;

	for (col = 0; w >= 8; col += 8, w -= 8)
	{
		const FLASHMEM PACKEDCOLORPIP_T * const pcl = (* byte2runpip) [* raster ++];
		memcpy(tgr + col, pcl, sizeof (* tgr) * 8);
	}
	if (w != 0)
	{
		const FLASHMEM PACKEDCOLORPIP_T * const pcl = (* byte2runpip) [* raster];
		memcpy(tgr + col, pcl, sizeof (* tgr) * w);
	}
	// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
	//dcache_clean((uintptr_t) tgr, sizeof (* tgr) * width);
}
#endif

// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
// Фон не трогаем
static void RAMFUNC ltdcmain_horizontal_pixels_tbg(
	PACKEDCOLORPIP_T * __restrict tgr,		// target raster
	const FLASHMEM uint8_t * __restrict raster,
	uint_fast16_t width,	// number of bits (start from LSB first byte in raster)
	COLORPIP_T fg
	)
{
	uint_fast16_t w = width;

	for (; w >= 8; w -= 8, tgr += 8)
	{
		const uint_fast8_t v = * raster ++;
		if (v & 0x01)	tgr [0] = fg;
		if (v & 0x02)	tgr [1] = fg;
		if (v & 0x04)	tgr [2] = fg;
		if (v & 0x08)	tgr [3] = fg;
		if (v & 0x10)	tgr [4] = fg;
		if (v & 0x20)	tgr [5] = fg;
		if (v & 0x40)	tgr [6] = fg;
		if (v & 0x80)	tgr [7] = fg;
	}
	if (w != 0)
	{
		uint_fast8_t vlast = * raster;
		do
		{
			if (vlast & 0x01)
				* tgr = fg;
			++ tgr;
			vlast >>= 1;
		} while (-- w);
	}
}

// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
// Фон не трогаем
// удвоенный по ширине растр
static void RAMFUNC ltdcmain_horizontal_x2_pixels_tbg(
	PACKEDCOLORPIP_T * __restrict tgr,		// target raster
	const FLASHMEM uint8_t * __restrict raster,
	uint_fast16_t width,	// number of bits (start from LSB first byte in raster)
	COLORPIP_T fg
	)
{
	uint_fast16_t w = width;

	for (; w >= 8; w -= 8, tgr += 16)
	{
		const uint_fast8_t v = * raster ++;
		if (v & 0x01)	{ tgr [ 0] = tgr [ 1] = fg; }
		if (v & 0x02)	{ tgr [ 2] = tgr [ 3] = fg; }
		if (v & 0x04)	{ tgr [ 4] = tgr [ 5] = fg; }
		if (v & 0x08)	{ tgr [ 6] = tgr [ 7] = fg; }
		if (v & 0x10)	{ tgr [ 8] = tgr [ 9] = fg; }
		if (v & 0x20)	{ tgr [10] = tgr [11] = fg; }
		if (v & 0x40)	{ tgr [12] = tgr [13] = fg; }
		if (v & 0x80)	{ tgr [14] = tgr [15] = fg; }
	}
	if (w != 0)
	{
		uint_fast8_t vlast = * raster;
		do
		{
			if (vlast & 0x01)
				tgr [ 0] = tgr [ 1] = fg;
			tgr += 2;
			vlast >>= 1;
		} while (-- w);
	}
}

#if 0//SMALLCHARW
// return new x coordinate
static uint_fast16_t
RAMFUNC_NONILINE
ltdcmain_horizontal_put_char_small(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,
	uint_fast16_t y,
	char cc
	)
{
	const uint_fast8_t width = SMALLCHARW;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(buffer, dx, dy, x, y + cgrow);
		colorpip_pixels(tgr, S1D13781_smallfont_LTDC [c] [cgrow], width);
	}
	return x + width;
}
#endif /* SMALLCHARW */

#if defined (SMALLCHARW)
// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
static uint_fast16_t RAMFUNC_NONILINE colorpip_put_char_small_tbg(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,
	uint_fast16_t y,
	char cc,
	COLOR565_T fg
	)
{
	const uint_fast8_t width = SMALLCHARW;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(buffer, dx, dy, x, y + cgrow);
		ltdcmain_horizontal_pixels_tbg(tgr, S1D13781_smallfont_LTDC [c] [cgrow], width, fg);
	}
	return x + width;
}

// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
static uint_fast16_t RAMFUNC_NONILINE colorpip_x2_put_char_small_tbg(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,
	uint_fast16_t y,
	char cc,
	COLOR565_T fg
	)
{
	const uint_fast8_t width = SMALLCHARW;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr0 = colpip_mem_at(buffer, dx, dy, x, y + cgrow * 2 + 0);
		ltdcmain_horizontal_x2_pixels_tbg(tgr0, S1D13781_smallfont_LTDC [c] [cgrow], width, fg);
		PACKEDCOLORPIP_T * const tgr1 = colpip_mem_at(buffer, dx, dy, x, y + cgrow * 2 + 1);
		ltdcmain_horizontal_x2_pixels_tbg(tgr1, S1D13781_smallfont_LTDC [c] [cgrow], width, fg);
	}
	return x + width * 2;
}

uint_fast16_t display_put_char_small_xy(uint_fast16_t x, uint_fast16_t y, uint_fast8_t c, COLOR565_T fg)
{
	PACKEDCOLORPIP_T * const fr = colmain_fb_draw();
	return colorpip_put_char_small_tbg(fr, DIM_X, DIM_Y, x, y, c, fg);
}
#endif /* defined (SMALLCHARW) */

#if SMALLCHARW2
// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
static uint_fast16_t RAMFUNC_NONILINE colorpip_put_char_small2_tbg(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,
	uint_fast16_t y,
	char cc,
	COLORPIP_T fg
	)
{
	const uint_fast8_t width = SMALLCHARW2;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH2; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(buffer, dx, dy, x, y + cgrow);
		ltdcmain_horizontal_pixels_tbg(tgr, S1D13781_smallfont2_LTDC [c] [cgrow], width, fg);
	}
	return x + width;
}
#endif /* SMALLCHARW2 */

#if SMALLCHARW3
// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
static uint_fast16_t RAMFUNC_NONILINE colorpip_put_char_small3_tbg(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,
	uint_fast16_t y,
	char cc,
	COLORPIP_T fg
	)
{
	const uint_fast8_t width = SMALLCHARW3;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH3; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(buffer, dx, dy, x, y + cgrow);
		ltdcmain_horizontal_pixels_tbg(tgr, & S1D13781_smallfont3_LTDC [c] [cgrow], width, fg);
	}
	return x + width;
}
#endif /* SMALLCHARW3 */



#if defined (SMALLCHARW)

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string_tbg(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	)
{
	char c;

	ASSERT(s != NULL);
	while ((c = * s ++) != '\0')
	{
		x = colorpip_put_char_small_tbg(buffer, dx, dy, x, y, c, fg);
	}
}
// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string_x2_tbg(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	)
{
	char c;

	ASSERT(s != NULL);
	while ((c = * s ++) != '\0')
	{
		x = colorpip_x2_put_char_small_tbg(buffer, dx, dy, x, y, c, fg);
	}
}

// Используется при выводе на графический индикатор,
// с поворотом
void
colpip_string_x2ra90_tbg(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg,		// цвет вывода текста
	COLORPIP_T bg		// цвет фона
	)
{
	char c;
	enum { TDX = SMALLCHARW * 2, TDY = SMALLCHARH * 2 };
	static RAMFRAMEBUFF ALIGNX_BEGIN PACKEDCOLORPIP_T scratch [GXSIZE(TDX, TDY)] ALIGNX_END;

	ASSERT(s != NULL);
	while ((c = * s ++) != '\0')
	{
		colpip_fillrect(scratch, TDX, TDY, 0, 0, TDX, TDY, bg);
		colorpip_x2_put_char_small_tbg(scratch, TDX, TDY, 0, 0, c, fg);
		hwaccel_ra90(buffer, dx, dy, x, y, scratch, TDX, TDY);
		y += TDX;
	}
}

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_text(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T fg,		// цвет вывода текста
	const char * s,		// строка для вывода
	size_t len			// количество символов
	)
{
	ASSERT(s != NULL);
	while (len --)
	{
		const char c = * s ++;
		x = colorpip_put_char_small_tbg(buffer, dx, dy, x, y, c, fg);
	}
}
// Используется при выводе на графический индикатор,
void
colpip_text_x2(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T fg,		// цвет вывода текста
	const char * s,		// строка для вывода
	size_t len			// количество символов
	)
{
	ASSERT(s != NULL);
	while (len --)
	{
		const char c = * s ++;
		x = colorpip_x2_put_char_small_tbg(buffer, dx, dy, x, y, c, fg);
	}
}

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string_x2ra90_count(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T fg,		// цвет вывода текста
	COLORPIP_T bg,		// цвет вывода текста
	const char * s,		// строка для вывода
	size_t len			// количество символов
	)
{
	enum { TDX = SMALLCHARW * 2, TDY = SMALLCHARH * 2 };
	static RAMFRAMEBUFF ALIGNX_BEGIN PACKEDCOLORPIP_T scratch [GXSIZE(TDX, TDY)] ALIGNX_END;

	ASSERT(s != NULL);
	while (len --)
	{
		const char c = * s ++;
		colpip_fillrect(scratch, TDX, TDY, 0, 0, TDX, TDY, bg);
		colorpip_x2_put_char_small_tbg(scratch, TDX, TDY, 0, 0, c, fg);
		hwaccel_ra90(buffer, dx, dy, x, y, scratch, TDX, TDY);
		y += TDX;
	}
}
#endif /* defined (SMALLCHARW) */

#if defined (SMALLCHARW2)

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string2_tbg(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	)
{
	char c;
	ASSERT(s != NULL);
	while ((c = * s ++) != '\0')
	{
		x = colorpip_put_char_small2_tbg(buffer, dx, dy, x, y, c, fg);
	}
}

// Возвращает ширину строки в пикселях
uint_fast16_t strwidth2(
	const char * s
	)
{
	ASSERT(s != NULL);
	return SMALLCHARW2 * strlen(s);
}

#endif /* defined (SMALLCHARW2) */

#if defined (SMALLCHARW3)
// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string3_tbg(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	)
{
	char c;

	ASSERT(s != NULL);
	while ((c = * s ++) != '\0')
	{
		x = colorpip_put_char_small3_tbg(buffer, dx, dy, x, y, c, fg);
	}
}

// Возвращает ширину строки в пикселях
uint_fast16_t strwidth3(
	const char * s
	)
{
	ASSERT(s != NULL);
	return SMALLCHARW3 * strlen(s);
}

#endif /* defined (SMALLCHARW3) */


#if defined (SMALLCHARW) && defined (SMALLCHARH)
// Возвращает ширину строки в пикселях
uint_fast16_t strwidth(
	const char * s
	)
{
	ASSERT(s != NULL);
	return SMALLCHARW * strlen(s);
}

#endif /* defined (SMALLCHARW) && defined (SMALLCHARH) */

#else /* LCDMODE_HORFILL */

#endif /* LCDMODE_HORFILL */


// скоприовать прямоугольник с типом пикселей соответствующим pip
void colpip_bitblt(
	uintptr_t dstinvalidateaddr,	// параметры clean invalidate получателя
	int_fast32_t dstinvalidatesize,
	PACKEDCOLORPIP_T * dst,	// получатель
	uint_fast16_t tdx,	// получатель Размеры окна в пикселях
	uint_fast16_t tdy,	// получатель
	uint_fast16_t x,	// получатель Позиция
	uint_fast16_t y,	// получатель
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const PACKEDCOLORPIP_T * src, 	// источник
	uint_fast16_t sdx,	// источник Размеры окна в пикселях
	uint_fast16_t sdy	// источник
	)
{
	ASSERT(src != NULL);
	ASSERT(dst != NULL);
	ASSERT(tdx >= sdx);
	ASSERT(tdy >= sdy);

	//ASSERT(((uintptr_t) src % DCACHEROWSIZE) == 0);	// TODO: добавиль парамтр для flush исходного растра
#if LCDMODE_HORFILL
	hwaccel_bitblt(
		dstinvalidateaddr, dstinvalidatesize,	// target area clean invalidate parameters
		colpip_mem_at(dst, tdx, tdy, x, y), tdx, tdy,
		srcinvalidateaddr, srcinvalidatesize,	// параметры clean источника
		src, sdx, sdy,
		PLTPARAM_NONE, 0
		);
#else /* LCDMODE_HORFILL */
	hwaccel_bitblt(
		dstinvalidateaddr, dstinvalidatesize,	// target area clean invalidate parameters
		colpip_mem_at(dst, tdx, tdy, x, y), tdx, tdy,
		srcinvalidateaddr, srcinvalidatesize,	// параметры clean источника
		src, sdx, sdy,
		PLTPARAM_NONE, 0
		);
#endif /* LCDMODE_HORFILL */
}

// скоприовать прямоугольник с типом пикселей соответствующим pip
void colpip_bitblt_key(
	uintptr_t dstinvalidateaddr,	// параметры clean invalidate получателя
	int_fast32_t dstinvalidatesize,
	PACKEDCOLORPIP_T * dst,	// получатель
	uint_fast16_t tdx,	// получатель Размеры окна в пикселях
	uint_fast16_t tdy,	// получатель
	uint_fast16_t x,	// получатель Позиция
	uint_fast16_t y,	// получатель
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const PACKEDCOLORPIP_T * src, 	// источник
	uint_fast16_t sdx,	// источник Размеры окна в пикселях
	uint_fast16_t sdy,	// источник
	unsigned keyflag, COLOR24_T keycolor
	)
{
	ASSERT(src != NULL);
	ASSERT(dst != NULL);
	ASSERT(tdx >= sdx);
	ASSERT(tdy >= sdy);

	//ASSERT(((uintptr_t) src % DCACHEROWSIZE) == 0);	// TODO: добавиль парамтр для flush исходного растра
#if LCDMODE_HORFILL
	hwaccel_bitblt(
		dstinvalidateaddr, dstinvalidatesize,	// target area clean invalidate parameters
		colpip_mem_at(dst, tdx, tdy, x, y), tdx, tdy,
		srcinvalidateaddr, srcinvalidatesize,	// параметры clean источника
		src, sdx, sdy,
		keyflag, keycolor
		);
#else /* LCDMODE_HORFILL */
	hwaccel_bitblt(
		dstinvalidateaddr, dstinvalidatesize,	// target area clean invalidate parameters
		colpip_mem_at(dst, tdx, tdy, x, y), tdx, tdy,
		srcinvalidateaddr, srcinvalidatesize,	// параметры clean источника
		src, sdx, sdy,
		keyflag, keycolor
		);
#endif /* LCDMODE_HORFILL */
}

// скоприовать прямоугольник с типом пикселей соответствующим pip
// с поворотом вправо на 90 градусов
void colpip_bitblt_ra90(
	uintptr_t dstinvalidateaddr,	// параметры clean invalidate получателя
	int_fast32_t dstinvalidatesize,
	PACKEDCOLORPIP_T * dst,	// получатель
	uint_fast16_t tdx,	// получатель Размеры окна в пикселях
	uint_fast16_t tdy,	// получатель
	uint_fast16_t x,	// получатель Позиция
	uint_fast16_t y,	// получатель
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const PACKEDCOLORPIP_T * src, 	// источник
	uint_fast16_t sdx,	// источник Размеры окна в пикселях
	uint_fast16_t sdy	// источник
	)
{
	ASSERT(src != NULL);
	ASSERT(dst != NULL);
	ASSERT(tdx >= sdx);
	ASSERT(tdy >= sdy);

	//ASSERT(((uintptr_t) src % DCACHEROWSIZE) == 0);	// TODO: добавиль парамтр для flush исходного растра
#if LCDMODE_HORFILL
	hwaccel_ra90(
		//dstinvalidateaddr, dstinvalidatesize,	// target area clean invalidate parameters
		dst, tdx, tdy,
		x, y,
		//srcinvalidateaddr, srcinvalidatesize,	// параметры clean источника
		src, sdx, sdy
		);
#else /* LCDMODE_HORFILL */
	hwaccel_ra90(
		//dstinvalidateaddr, dstinvalidatesize,	// target area clean invalidate parameters
		dst, tdx, tdy,
		x, y,
		//srcinvalidateaddr, srcinvalidatesize,	// параметры clean источника
		src, sdx, sdy
		);
#endif /* LCDMODE_HORFILL */
}


#if SMALLCHARH3

static uint_fast16_t
RAMFUNC_NONILINE ltdc_horizontal_put_char_small3(
	PACKEDCOLORPIP_T * const __restrict buffer,
	const uint_fast16_t dx,
	const uint_fast16_t dy,
	uint_fast16_t x, uint_fast16_t y,
	char cc
	)
{
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	return ltdc_horizontal_put_char_unified(S1D13781_smallfont3_LTDC [0], SMALLCHARW3, SMALLCHARW3, SMALLCHARH3, sizeof S1D13781_smallfont3_LTDC [0], buffer, dx, dy, x, y, c);
//	const uint_fast8_t width = SMALLCHARW3;
//	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
//	uint_fast8_t cgrow;
//	for (cgrow = 0; cgrow < SMALLCHARH3; ++ cgrow)
//	{
//		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(buffer, dx, dy, x, y + cgrow);
//		ltdc_horizontal_pixels(tgr, & S1D13781_smallfont3_LTDC [c] [cgrow], width);
//	}
//	return x + width;
}

static void
display_string3(uint_fast16_t x, uint_fast16_t y, const char * s, uint_fast8_t lowhalf)
{
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	char c;
//	ltdc_secondoffs = 0;
//	ltdc_h = SMALLCHARH3;
	while ((c = * s ++) != '\0')
		x = ltdc_horizontal_put_char_small3(buffer, dx, dy, x, y, c);
}

void
colpip_string3_at_xy(
	PACKEDCOLORPIP_T * const __restrict buffer,
	const uint_fast16_t dx,
	const uint_fast16_t dy,
	uint_fast16_t x,
	uint_fast16_t y,
	const char * __restrict s
	)
{
	char c;
//	ltdc_secondoffs = 0;
//	ltdc_h = SMALLCHARH3;
	while ((c = * s ++) != '\0')
		x = ltdc_horizontal_put_char_small3(buffer, dx, dy, x, y, c);
}

void
display_string3_at_xy(uint_fast16_t x, uint_fast16_t y, const char * __restrict s, COLORPIP_T fg, COLORPIP_T bg)
{
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	colmain_setcolors(fg, bg);
	do
	{
		display_string3(x, y + lowhalf, s, lowhalf);
	} while (lowhalf --);
}

#endif /* SMALLCHARH3 */


#if LCDMODE_COLORED

// Установить прозрачность для прямоугольника
void display_transparency(
	uint_fast16_t x1, uint_fast16_t y1,
	uint_fast16_t x2, uint_fast16_t y2,
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	)
{
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
#if 1
	uint_fast16_t y;

	for (y = y1; y <= y2; y ++)
	{
		uint_fast16_t x;
		const uint_fast32_t yt = (uint_fast32_t) GXADJ(dx) * y;
		//ASSERT(y < dy);
		for (x = x1; x <= x2; x ++)
		{
			//ASSERT(x < dx);
			buffer [yt + x] = getshadedcolor(buffer [yt + x], alpha);
		}
	}
#else
	uint_fast16_t y;

	for (y = y1; y <= y2; y ++)
	{
		for (uint_fast16_t x = x1; x <= x2; x ++)
		{
			PACKEDCOLORPIP_T * const p = colpip_mem_at(buffer, dx, dy, x, y);
			* p = getshadedcolor(* p, alpha);
		}
	}
#endif
}

static uint_fast8_t scalecolor(
	uint_fast8_t cv,	// color component value
	uint_fast8_t maxv,	// maximal color component value
	uint_fast8_t rmaxv	// resulting maximal color component value
	)
{
	return (cv * rmaxv) / maxv;
}

/* модифицировать цвет в RGB24 */
static COLOR24_T
color24_shaded(
	COLOR24_T dot,
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	)
{
	//return dot;	// test
	if (dot == 0)
		return COLOR24(alpha >> 2, alpha >> 2, alpha >> 2);
	const uint_fast8_t r = scalecolor((dot >> 16) & 0xFF, 255, alpha);
	const uint_fast8_t g = scalecolor((dot >> 8) & 0xFF, 255, alpha);
	const uint_fast8_t b = scalecolor((dot >> 0) & 0xFF, 255, alpha);
	return COLOR24(r, g, b);
}

/* модифицировать цвет в RGB24 */
static COLOR24_T
color24_aliased(
	COLOR24_T dot
	)
{
	return color24_shaded(dot, DEFAULT_ALPHA);	// test
}


/* модифицировать цвет */
COLORPIP_T getshadedcolor(
	COLORPIP_T dot, // исходный цвет
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	)
{
#if defined (COLORPIP_SHADED)

	return dot |= COLORPIP_SHADED;

#elif LCDMODE_MAIN_RGB565

	if (dot == COLORPIP_BLACK)
	{
		return TFTRGB(alpha, alpha, alpha); // back gray
	}
	else
	{
		// распаковка дисплейного представления
		const uint_fast8_t r = ((dot >> 11) & 0x001f) * 8;	// result in 0..255
		const uint_fast8_t g = ((dot >> 5) & 0x003f) * 4;	// result in 0..255
		const uint_fast8_t b = ((dot >> 0) & 0x001f) * 8;	// result in 0..255

		const COLOR24_T c = color24_shaded(COLOR24(r, g, b), alpha);
		return TFTRGB((c >> 16) & 0xFF, (c >> 8) & 0xFF, (c >> 0) & 0xFF);
	}

#elif LCDMODE_MAIN_ARGB888 && (CPUSTYLE_XC7Z || CPUSTYLE_XCZU) && ! WITHTFT_OVER_LVDS

	if (dot == COLORPIP_BLACK)
	{
		return TFTRGB(alpha, alpha, alpha); // back gray
	}
	else
	{
		// распаковка дисплейного представления
		const uint_fast8_t r = ((dot >> 16) & 0x00FF) * 1;	// result in 0..255
		const uint_fast8_t g = ((dot >> 0) & 0x00FF) * 1;	// result in 0..255
		const uint_fast8_t b = ((dot >> 8) & 0x00FF) * 1;	// result in 0..255

		const COLOR24_T c = color24_shaded(COLOR24(r, g, b), alpha);
		return TFTRGB((c >> 16) & 0xFF, (c >> 8) & 0xFF, (c >> 0) & 0xFF);
	}


#elif LCDMODE_MAIN_ARGB888

	if (dot == COLORPIP_BLACK)
	{
		return TFTRGB565(alpha, alpha, alpha); // back gray
	}
	else
	{
		// распаковка дисплейного представления
		const uint_fast8_t r = ((dot >> 16) & 0x00FF) * 1;	// result in 0..255
		const uint_fast8_t g = ((dot >> 8) & 0x00FF) * 1;	// result in 0..255
		const uint_fast8_t b = ((dot >> 0) & 0x00FF) * 1;	// result in 0..255

		const COLOR24_T c = color24_shaded(COLOR24(r, g, b), alpha);
		return TFTRGB((c >> 16) & 0xFF, (c >> 8) & 0xFF, (c >> 0) & 0xFF);
	}


#elif LCDMODE_PIP_RGB24

	if (dot == COLORPIP_BLACK)
	{
		return COLOR24(alpha, alpha, alpha); // back gray
	}
	else
	{
		return color24_shaded(dot, alpha);
	}



#else /*  */
	//#warning LCDMODE_PIP_L8 or LCDMODE_PIP_RGB565 not defined
	return dot;

#endif /* LCDMODE_PIP_L8 */
}

#if defined (COLORPIP_SHADED)

static void fillpair_xltrgb24(COLOR24_T * xltable, unsigned i, COLOR24_T c)
{
	ASSERT(i < 128);
	xltable [i] = c;
	xltable [i | COLORPIP_SHADED] = color24_shaded(c, DEFAULT_ALPHA);
}

static void fillfour_xltrgb24(COLOR24_T * xltable, unsigned i, COLOR24_T c)
{
	ASSERT(i < 128 - 16);
	xltable [i] = c;
	xltable [i | COLORPIP_SHADED] = color24_shaded(c, DEFAULT_ALPHA);
	xltable [i | COLORPIP_ALIASED] =  color24_aliased(c);
	xltable [i | COLORPIP_SHADED | COLORPIP_ALIASED] = color24_aliased(color24_shaded(c, DEFAULT_ALPHA));
}

#endif /* defined (COLORPIP_SHADED) */

#endif /* LCDMODE_COLORED */

void display2_xltrgb24(COLOR24_T * xltable)
{
	unsigned i;

	//PRINTF("display2_xltrgb24: init indexed colors\n");

	for (i = 0; i < 256; ++ i)
	{
		xltable [i] = COLOR24(i, i, i);
	}

#if defined (COLORPIP_SHADED)

	// часть цветов с 0-го индекса используется в отображении водопада
	// остальные в дизайне
	for (i = 0; i < COLORPIP_BASE; ++ i)
	{
		fillpair_xltrgb24(xltable, i, colorgradient(i, COLORPIP_BASE - 1));
	}

#if 0
	{
		/* тестовое заполнение палитры для проверки целостности сигналов к TFT
		 * Используется совместно с test: вывод палитры на экран
		 */
		enum { TESTSIZE = 64 };
		for (i = 0; i < 256; ++ i)
		{
			xltable [i] = COLOR24(0, 0, 0);
		}

	#if 0
		/* RED */
		for (i = 0; i < TESTSIZE; ++ i)
		{
			uint_fast8_t c = scalecolor(i, TESTSIZE - 1, 255);
			fillpair_xltrgb24(xltable, i, COLOR24(1 * c, 0 * c, 0 * c));	// проверить результат перед попыткой применить целочисленные вычисления!
		}
	#elif 0
		/* GREEN */
		for (i = 0; i < TESTSIZE; ++ i)
		{
			uint_fast8_t c = scalecolor(i, TESTSIZE - 1, 255);
			fillpair_xltrgb24(xltable, i, COLOR24(0 * c, 1 * c, 0 * c));	// проверить результат перед попыткой применить целочисленные вычисления!
		}
	#else
		/* BLUE */
		for (i = 0; i < TESTSIZE; ++ i)
		{
			uint_fast8_t c = scalecolor(i, TESTSIZE - 1, 255);
			fillpair_xltrgb24(xltable, i, COLOR24(0 * c, 0 * c, 1 * c));	// проверить результат перед попыткой применить целочисленные вычисления!
		}
	#endif
	}
#endif
	// Цвета используемые в дизайне

	fillfour_xltrgb24(xltable, COLORPIP_YELLOW    	, COLOR24(0xFF, 0xFF, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_ORANGE    	, COLOR24(0xFF, 0xA5, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_BLACK     	, COLOR24(0x00, 0x00, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_WHITE     	, COLOR24(0xFF, 0xFF, 0xFF));
	fillfour_xltrgb24(xltable, COLORPIP_GRAY      	, COLOR24(0x60, 0x60, 0x60));
	fillfour_xltrgb24(xltable, COLORPIP_DARKGREEN 	, COLOR24(0x00, 0x80, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_BLUE      	, COLOR24(0x00, 0x00, 0xFF));
	fillfour_xltrgb24(xltable, COLORPIP_GREEN     	, COLOR24(0x00, 0xFF, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_RED       	, COLOR24(0xFF, 0x00, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_DARKGRAY    , COLOR24(0x10, 0x10, 0x10));

	fillfour_xltrgb24(xltable, COLORMAIN_LOCKED	  	, COLOR24(0x3C, 0x3C, 0x00));
	// код (COLORPIP_BASE + 15) освободися. GUI_MENUSELECTCOLOR?

#if COLORSTYLE_ATS52
	// new (for ats52)
	fillfour_xltrgb24(xltable, COLORPIP_GRIDCOLOR		, COLOR24(0x80, 0x00, 0x00));		//COLOR_GRAY - center marker
	fillfour_xltrgb24(xltable, COLORPIP_GRIDCOLOR2		, COLOR24(0x60, 0x60, 0x60));		//COLOR_DARKRED - other markers
	fillfour_xltrgb24(xltable, COLORPIP_SPECTRUMBG		, COLOR24(0x00, 0x40, 0x40));		// фон спектра вне полосы пропускания
	fillfour_xltrgb24(xltable, COLORMAIN_SPECTRUMBG2	, COLOR24(0x00, 0x80, 0x80));		// фон спектра - полоса пропускания приемника
	fillfour_xltrgb24(xltable, COLORPIP_SPECTRUMFG		, COLOR24(0x00, 0xFF, 0x00));		//COLOR_GREEN

#else /* COLORSTYLE_ATS52 */
	// old
	fillfour_xltrgb24(xltable, COLORPIP_GRIDCOLOR      	, COLOR24(0x80, 0x80, 0x00));        //COLOR_GRAY - center marker
	fillfour_xltrgb24(xltable, COLORPIP_GRIDCOLOR2     	, COLOR24(0x80, 0x00, 0x00));        //COLOR_DARKRED - other markers
	fillfour_xltrgb24(xltable, COLORPIP_SPECTRUMBG     	, COLOR24(0x00, 0x00, 0x00));            // фон спектра вне полосы пропускания
	fillfour_xltrgb24(xltable, COLORMAIN_SPECTRUMBG2   	, COLOR24(0x00, 0x80, 0x80));        // фон спектра - полоса пропускания приемника
	fillfour_xltrgb24(xltable, COLORPIP_SPECTRUMFG		, COLOR24(0x00, 0xFF, 0x00));		//COLOR_GREEN

#endif /* COLORSTYLE_ATS52 */

#elif LCDMODE_COLORED && ! LCDMODE_DUMMY	/* LCDMODE_MAIN_L8 && LCDMODE_PIP_L8 */
	//PRINTF("display2_xltrgb24: init RRRRRGGG GGGBBBBB colos\n");
	// Обычная таблица - все цвета могут быть использованы как индекс
	// Водопад отображается без использования инлдексов цветов

	for (i = 0; i < 256; ++ i)
	{
		uint_fast8_t r = ((i & 0xe0) << 0) | ((i & 0xe0) >> 3) | ((i & 0xe0) >> 6);		// 3 bit red
		uint_fast8_t g = ((i & 0x1c) << 3) | ((i & 0x1c) << 0) | ((i & 0x1c) >> 3) ;	// 3 bit green
		uint_fast8_t b = ((i & 0x03) << 6) | ((i & 0x03) << 4) | ((i & 0x03) << 2) | ((i & 0x03) << 0);	// 2 bit blue
		xltable [i] = COLOR24(r, g, b);
	}

#else
	#warning Monochrome display without indexing colors
#endif /* LCDMODE_MAIN_L8 && LCDMODE_PIP_L8 */
}

#endif /* ! (LCDMODE_DUMMY || LCDMODE_HD44780) */

#if WITHDMA2DHW

void arm_hardware_dma2d_initialize(void)
{
#if CPUSTYLE_STM32H7XX
	/* Enable the DMA2D Clock */
	RCC->AHB3ENR |= RCC_AHB3ENR_DMA2DEN;	/* DMA2D clock enable */
	(void) RCC->AHB3ENR;

#else /* CPUSTYLE_STM32H7XX */
	/* Enable the DMA2D Clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN;	/* DMA2D clock enable */
	(void) RCC->AHB1ENR;

#endif /* CPUSTYLE_STM32H7XX */

	/* set AXI master timer */
	DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT_Msk | DMA2D_AMTCR_EN_Msk)) |
		(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
		(DMA2D_AMTCR_DT_ENABLE << DMA2D_AMTCR_EN_Pos) |
		0;
#if 0
	static ALIGNX_BEGIN uint32_t clut [256] ALIGNX_END;
	memset(clut, 0xFF, sizeof clut);
	dcache_clean((uintptr_t) clut, sizeof clut);
	DMA2D->FGCMAR = (uintptr_t) clut;
	DMA2D->BGCMAR = (uintptr_t) clut;
#endif
}

#endif /* WITHDMA2DHW */

