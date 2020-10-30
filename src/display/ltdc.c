/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// STM32xxx LCD-TFT Controller (LTDC)
// RENESAS Video Display Controller 5
//	Video Display Controller 5 (5): Image Synthesizer
//	Video Display Controller 5 (7): Output Controller

#include "hardware.h"

#include "display.h"
#include <stdint.h>
#include <string.h>

#include "formats.h"	// for debug prints
#include "gpio.h"

#if LCDMODETX_TC358778XBG
#include "mipi_dsi.h"
#endif /* LCDMODETX_TC358778XBG */

#if WITHLTDCHW

#if LCDMODE_LQ043T3DX02K
	// Sony PSP-1000 display panel
	// LQ043T3DX02K panel (272*480)
	// RK043FN48H-CT672B  panel (272*480) - плата STM32F746G-DISCO
	/** 
	  * @brief  RK043FN48H Size  
	  */    
	enum
	{
		WIDTH = 480,				/* LCD PIXEL WIDTH            */
		HEIGHT = 272,			/* LCD PIXEL HEIGHT           */
		/** 
		  * @brief  RK043FN48H Timing  
		  */     
		HSYNC = 41,				/* Horizontal synchronization */
		HBP = 2,				/* Horizontal back porch      */
		HFP = 2,				/* Horizontal front porch     */

		VSYNC = 10,				/* Vertical synchronization   */
		VBP = 2,					/* Vertical back porch        */
		VFP = 4,					/* Vertical front porch       */

		/* Accumulated parameters for this display */
		LEFTMARGIN = HSYNC + HBP,	/* horizontal delay before DE start */
		TOPMARGIN = VSYNC + VBP,	/* vertical delay before DE start */

		// MODE: DE/SYNC mode select.
		// DE MODE: MODE="1", VS and HS must pull high.
		// SYNC MODE: MODE="0". DE must be grounded
		VSYNCNEG = 1,			/* Negative polarity required for VSYNC signal */
		HSYNCNEG = 1,			/* Negative polarity required for HSYNC signal */
		DENEG = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
		BOARD_DEMODE = 0		/* 0: static signal, 1: DE controlled */
	};
	/* SONY PSP-1000 display (4.3") required. */
	/* Используется при BOARD_DEMODE = 0 */
	#define BOARD_DERESET 1		/* требуется формирование сигнала RESET для панели по этому выводу после начала формирования синхронизации */
	#define LTDC_DOTCLK	9000000uL	// частота пикселей при работе с интерфейсом RGB

#elif LCDMODE_AT070TN90

	/* AT070TN90 panel (800*480) - 7" display HV mode */
	enum
	{
		WIDTH = 800,			/* LCD PIXEL WIDTH            */
		HEIGHT = 480,			/* LCD PIXEL HEIGHT           */
		/** 
		  * @brief  AT070TN90 Timing  
		  * MODE=0 (DE)
		  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
		  * MODE=1 (SYNC)
		  * When selected sync mode, de must be grounded.
		  */     
		HSYNC = 40,				/* Horizontal synchronization 1..40 */
		HFP = 210,				/* Horizontal front porch  16..354   */

		VSYNC = 20,				/* Vertical synchronization 1..20  */
		VFP = 22,				/* Vertical front porch  7..147     */

		/* Accumulated parameters for this display */
		LEFTMARGIN = 46,		/* horizontal blanking EXACTLY */
		TOPMARGIN = 23,			/* vertical blanking EXACTLY */

		// MODE: DE/SYNC mode select.
		// DE MODE: MODE="1", VS and HS must pull high.
		// SYNC MODE: MODE="0". DE must be grounded
		VSYNCNEG = 1,			/* Negative polarity required for VSYNC signal */
		HSYNCNEG = 1,			/* Negative polarity required for HSYNC signal */
		DENEG = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
#if WITHLCDDEMODE
		BOARD_DEMODE = 1		/* 0: static signal, 1: DE controlled */
#else /* WITHLCDSYNCMODE */
		BOARD_DEMODE = 0		/* 0: static signal, 1: DE controlled */
#endif /* WITHLCDSYNCMODE */
	};
	#define LTDC_DOTCLK	30000000uL	// частота пикселей при работе с интерфейсом RGB

#elif LCDMODE_AT070TNA2

	/* AT070TNA2 panel (1024*600) - 7" display HV mode */
	// HX8282-A01.pdf, page 38
	enum
	{
		WIDTH = 1024,			/* LCD PIXEL WIDTH            */
		HEIGHT = 600,			/* LCD PIXEL HEIGHT           */
		/** 
		  * @brief  AT070TN90 Timing  
		  * MODE=0 (DE)
		  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
		  * MODE=1 (SYNC)
		  * When selected sync mode, de must be grounded.
		  */     
		HSYNC = 140,			/* Horizontal synchronization 1..140 */
		HFP = 160,				/* Horizontal front porch  16..216   */

		VSYNC = 20,				/* Vertical synchronization 1..20  */
		VFP = 12,				/* Vertical front porch  1..127     */

		/* Accumulated parameters for this display */
		LEFTMARGIN = 160,		/* horizontal blanking EXACTLY */
		TOPMARGIN = 23,			/* vertical blanking EXACTLY */

		// MODE: DE/SYNC mode select.
		// DE MODE: MODE="1", VS and HS must pull high.
		// SYNC MODE: MODE="0". DE must be grounded
		VSYNCNEG = 1,			/* Negative polarity required for VSYNC signal */
		HSYNCNEG = 1,			/* Negative polarity required for HSYNC signal */
		DENEG = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
		BOARD_DEMODE = 0		/* 0: static signal, 1: DE controlled */
	};
	#define LTDC_DOTCLK	51200000uL	// частота пикселей при работе с интерфейсом RGB 40.8..67.2

#elif LCDMODE_ILI8961
	// HHT270C-8961-6A6 (320*240)
	enum
	{
		WIDTH = 320 * 3,				/* LCD PIXEL WIDTH            */
		HEIGHT = 240,			/* LCD PIXEL HEIGHT           */

		/** 
		  * @brief  RK043FN48H Timing  
		  */     
		HSYNC = 1,				/* Horizontal synchronization */
		HBP = 2,				/* Horizontal back porch      */
		HFP = 2,				/* Horizontal front porch     */

		VSYNC = 1,				/* Vertical synchronization   */
		VBP = 2,					/* Vertical back porch        */
		VFP = 2,					/* Vertical front porch       */

		/* TODO: should be changed Accumulated parameters for this display */
		LEFTMARGIN = HSYNC + HBP,	/* horizontal delay before DE start */
		TOPMARGIN = VSYNC + VBP,	/* vertical delay before DE start */

		VSYNCNEG = 1,			/* Negative polarity required for VSYNC signal */
		HSYNCNEG = 1,			/* Negative polarity required for HSYNC signal */
		DENEG = 0,				/* DE polarity: (normal: DE is 0 while sync) */
		BOARD_DEMODE = 0		/* 0: static signal, 1: DE controlled */
	};
	#define LTDC_DOTCLK	24000000uL	// частота пикселей при работе с интерфейсом RGB

#elif LCDMODE_ILI9341
	// SF-TC240T-9370-T (320*240)
	enum
	{

		WIDTH = 240,				/* LCD PIXEL WIDTH            */
		HEIGHT = 320,			/* LCD PIXEL HEIGHT           */

		/** 
		  * @brief  ILI9341 Timing  
		  */     
		HSYNC = 10,				/* Horizontal synchronization */
		HBP = 20,				/* Horizontal back porch      */
		HFP = 10,				/* Horizontal front porch     */

		VSYNC = 2,				/* Vertical synchronization   */
		VBP = 2,					/* Vertical back porch        */
		VFP = 4,					/* Vertical front porch       */

		/* Accumulated parameters for this display */
		LEFTMARGIN = HSYNC + HBP,	/* horizontal delay before DE start */
		TOPMARGIN = VSYNC + VBP,	/* vertical delay before DE start */

		VSYNCNEG = 1,			/* Negative polarity required for VSYNC signal */
		HSYNCNEG = 1,			/* Negative polarity required for HSYNC signal */
		DENEG = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
		BOARD_DEMODE = 0		/* 0: static signal, 1: DE controlled */
	};
	#define LTDC_DOTCLK	3000000uL	// частота пикселей при работе с интерфейсом RGB

#elif LCDMODE_H497TLB01P4
	/* 720xRGBx1280 - 5" AMOELD Panel H497TLB01.4 */
	// See also:
	// https://github.com/bbelos/rk3188-kernel/blob/master/drivers/video/rockchip/transmitter/tc358768.c
	// https://github.com/tanish2k09/venom_kernel_aio_otfp/blob/master/drivers/input/touchscreen/mediatek/S3202/synaptics_dsx_i2c.c
	// https://stash.phytec.com/projects/TIRTOS/repos/vps-phytec/raw/src/boards/src/bsp_boardPriv.h?at=e8b92520f41e6523301d120dae15db975ad6d0da
	//https://code.ihub.org.cn/projects/825/repositories/874/file_edit_page?file_name=am57xx-idk-common.dtsi&path=arch%2Farm%2Fboot%2Fdts%2Fam57xx-idk-common.dtsi&rev=master
	enum
	{
		WIDTH = 720,			/* LCD PIXEL WIDTH            */
		HEIGHT = 1280,			/* LCD PIXEL HEIGHT           */
		/**
		  * @brief  AT070TN90 Timing
		  * MODE=0 (DE)
		  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
		  * MODE=1 (SYNC)
		  * When selected sync mode, de must be grounded.
		  */
		HSYNC = 5,				/* Horizontal synchronization 1..40 */
		HBP = 11,				/* Horizontal back porch      */
		HFP = 16,				/* Horizontal front porch  16..354   */

		VSYNC = 5,				/* Vertical synchronization 1..20  */
		VBP = 11,					/* Vertical back porch        */
		VFP = 16,				/* Vertical front porch  7..147     */

		/* Accumulated parameters for this display */
		LEFTMARGIN = HSYNC + HBP,	/* horizontal delay before DE start */
		TOPMARGIN = VSYNC + VBP,	/* vertical delay before DE start */

		// MODE: DE/SYNC mode select.
		// DE MODE: MODE="1", VS and HS must pull high.
		// SYNC MODE: MODE="0". DE must be grounded
		VSYNCNEG = 1,			/* Negative polarity required for VSYNC signal */
		HSYNCNEG = 1,			/* Negative polarity required for HSYNC signal */
		DENEG = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
#if WITHLCDDEMODE
		BOARD_DEMODE = 1		/* 0: static signal, 1: DE controlled */
#else /* WITHLCDSYNCMODE */
		BOARD_DEMODE = 0		/* 0: static signal, 1: DE controlled */
#endif /* WITHLCDSYNCMODE */
	};
	#define LTDC_DOTCLK	57153600UL	// частота пикселей при работе с интерфейсом RGB

#else
	#error Unsupported LCDMODE_xxx
	#define LTDC_DOTCLK	3000000uL	// частота пикселей при работе с интерфейсом RGB

#endif

enum
{
	HFULL = LEFTMARGIN + WIDTH + HFP,	/* horizontal full period */
	VFULL = TOPMARGIN + HEIGHT + VFP	/* vertical full period */
};

#if CPUSTYLE_R7S721

void vdc5_update(
	volatile uint32_t * reg,
	const char * label,
	uint_fast32_t mask
	)
{
	* reg |= mask;
	(void) * reg;
	uint_fast32_t count = 1000;
	while ((* reg & mask) != 0)
	{
		local_delay_ms(1);
		if (-- count == 0)
		{
			PRINTF(PSTR("wait reg=%p %s mask=%08lX, stay=%08lX\n"), reg, label, mask, * reg & mask);
			return;
		}
	}
}

/* Set value at specified position of I/O register */
/* Waiting for set */
#define SETREG32_UPDATE(reg, iwidth, ipos, ival) do { \
	uint_fast32_t val = (ival); \
	uint_fast8_t width = (iwidth); \
	const uint_fast8_t pos = (ipos); \
	uint_fast32_t mask = 0; \
	while (width --) \
		mask = (mask << 1) | 1uL; \
	mask <<= (pos); \
	val <<= (pos); \
	ASSERT((val & mask) == val); \
	* (reg) = (* (reg) & ~ (mask)) | (val & mask); \
	(void) * (reg);	/* dummy read */ \
	uint_fast32_t count = 1000; \
	do { \
		if (count -- == 0) {PRINTF(PSTR("wait %s/%d\n"), __FILE__, __LINE__); break; } \
		local_delay_ms(1); \
	} while (((* (reg)) & mask) != 0); /* wait for bit chamge to zero */ \
} while (0)

/* Set value at specified position of I/O register */
#define SETREG32_CK(reg, iwidth, ipos, ival) do { \
	uint_fast32_t val = (ival); \
	uint_fast8_t width = (iwidth); \
	const uint_fast8_t pos = (ipos); \
	uint_fast32_t mask = 0; \
	while (width --) \
		mask = (mask << 1) | 1uL; \
	mask <<= (pos); \
	val <<= (pos); \
	ASSERT((val & mask) == val); \
	* (reg) = (* (reg) & ~ (mask)) | (val & mask); \
	(void) * (reg);	/* dummy read */ \
	ASSERT(((* (reg)) & mask) == val); \
} while (0)


/************************************************************************/

static void
VDC5_fillLUT_L8(
	volatile uint32_t * reg,
	const COLOR24_T * xltrgb24
	)
{
	unsigned i;

	for (i = 0; i < 256; ++ i)
	{
		const uint_fast32_t color = xltrgb24 [i];
		/* запись значений в регистры палитры */
		SETREG32_CK(reg + i, 8, 24, 0xFF);	// alpha
		SETREG32_CK(reg + i, 8, 16, COLOR24_R(color));
		SETREG32_CK(reg + i, 8, 8, COLOR24_G(color));
		SETREG32_CK(reg + i, 8, 0, COLOR24_B(color));
	}
}

static void vdc5fb_init_syscnt(struct st_vdc5 * vdc)
{

	/* Ignore all irqs here */
	vdc->SYSCNT_INT4 = 0x00000000;
	vdc->SYSCNT_INT5 = 0x00000000;

	/* Clear all pending irqs */
	vdc->SYSCNT_INT1 = 0x00000000;
	vdc->SYSCNT_INT2 = 0x00000000;

	/* Setup panel clock */
	// I/O Clock Frequency (MHz) = 60 MHz
	SETREG32_CK(& vdc->SYSCNT_PANEL_CLK, 1, 8, 0);	/* PANEL_ICKEN */
	SETREG32_CK(& vdc->SYSCNT_PANEL_CLK, 2, 12, 0x03);	/* Divided Clock Source Select: 3: Peripheral clock 1 */
	SETREG32_CK(& vdc->SYSCNT_PANEL_CLK, 6, 0, calcdivround2(P1CLOCK_FREQ, display_getdotclock()));	/* Clock Frequency Division Ratio Note: Settings other than those in Table 35.5 are prohibited. */
	SETREG32_CK(& vdc->SYSCNT_PANEL_CLK, 1, 8, 1);	/* PANEL_ICKEN */

}


static void vdc5fb_init_sync(struct st_vdc5 * const vdc)
{
	//SETREG32_CK(& vdc->SC0_SCL0_FRC1, 16, 16, 0);	// SC0_RES_VMASK
	SETREG32_CK(& vdc->SC0_SCL0_FRC1, 1, 0, 0);		// SC0_RES_VMASK_ON 0: Repeated Vsync signal masking control is disabled.
	//SETREG32_CK(& vdc->SC0_SCL0_FRC2, 16, 16, 0);	// SC0_RES_VLACK
	SETREG32_CK(& vdc->SC0_SCL0_FRC2, 1, 0, 0);		// SC0_RES_VLACK_ON	0: Compensation of missing Vsync signals is disabled.

	SETREG32_CK(& vdc->SC0_SCL0_FRC5, 1, 8, 1);		// SC0_RES_FLD_DLY_SEL
	SETREG32_CK(& vdc->SC0_SCL0_FRC5, 8, 0, 1);		// SC0_RES_VSDLY

	SETREG32_CK(& vdc->SC0_SCL0_FRC4, 11, 16, VFULL - 1);// SC0_RES_FV Free-Running Vsync Period Setting
	SETREG32_CK(& vdc->SC0_SCL0_FRC4, 11, 0, HFULL - 1);	// SC0_RES_FH Hsync Period Setting

	SETREG32_CK(& vdc->SC0_SCL0_FRC6, 11, 16, TOPMARGIN);	// SC0_RES_F_VS VSYNC + V backporch lines)
	SETREG32_CK(& vdc->SC0_SCL0_FRC6, 11, 0, HEIGHT);			// SC0_RES_F_VW

	SETREG32_CK(& vdc->SC0_SCL0_FRC7, 11, 16, LEFTMARGIN);	// SC0_RES_F_HS HSYNC+H backporch pixel-clock cycles
	SETREG32_CK(& vdc->SC0_SCL0_FRC7, 11, 0, WIDTH);			// SC0_RES_F_HW

	SETREG32_CK(& vdc->SC0_SCL0_FRC3, 1, 0, 0x01);	// SC0_RES_VS_SEL Vsync Signal Output Select 1: Internally generated free-running Vsync signal
	vdc5_update(& vdc->SC0_SCL0_UPDATE, "SC0_SCL0_UPDATE",
			(1 << 8) |	// SC0_SCL0_UPDATE	SYNC Control Register Update
			0
		);
	vdc5_update(& vdc->SC0_SCL0_UPDATE, "SC0_SCL0_UPDATE",
			(1 << 8) |	// SC0_SCL0_UPDATE	SYNC Control Register Update
			(1 << 4) |	// SC0_SCL0_VEN_B	Synchronization Control and Scaling-up Control Register Update
			0
		);
	vdc5_update(& vdc->GR_VIN_UPDATE, "GR_VIN_UPDATE",
			(1 << 8) |	// GR_VIN_UPDATE Graphics Display Register Update
			(1 << 4) |	// GR_VIN_P_VEN Graphics Display Register Update
			0
		);

}

static void vdc5fb_init_scalers(struct st_vdc5 * const vdc)
{

	////////////////////////////////////////////////////////////////
	// SC0

	// down-scaler off
	// depend on SC0_SCL0_VEN_A
	SETREG32_CK(& vdc->SC0_SCL0_DS1, 1, 4, 0);	// SC0_RES_DS_V_ON Vertical Scale Down On/Off 0: Off
	SETREG32_CK(& vdc->SC0_SCL0_DS1, 1, 0, 0);	// SC0_RES_DS_H_ON
	// up-scaler off
	// depend on SC0_SCL0_VEN_B
	SETREG32_CK(& vdc->SC0_SCL0_US1, 1, 4, 0);	// SC0_RES_US_V_ON
	SETREG32_CK(& vdc->SC0_SCL0_US1, 1, 0, 0);	// SC0_RES_US_V_ON

	SETREG32_CK(& vdc->SC0_SCL0_OVR1, 24, 0, 0x00008080);	// Background Color Setting RGB

#if 0
	SETREG32_CK(& vdc->SC0_SCL0_DS7, 11, 16, HEIGHT);// SC0_RES_OUT_VW Number of Valid Lines in Vertical Direction Output by Scaling-down Control Block (lines)
	SETREG32_CK(& vdc->SC0_SCL0_DS7, 11, 0, WIDTH);	// SC0_RES_OUT_HW Number of Valid Horizontal Pixels Output by Scaling-Down Control Block (video-image clock cycles)


	

	SETREG32_CK(& vdc->SC0_SCL0_US8, 1, 4,	1); // SC0_RES_IBUS_SYNC_SEL 1: Sync signals from the graphics processing block
	SETREG32_CK(& vdc->SC0_SCL0_US8, 1, 4,	1); // SC0_RES_DISP_ON 1: Frame display off
#endif
	////////////////////////////////////////////////////////////////
	// SC1
}

static void vdc5fb_init_graphics(struct st_vdc5 * const vdc)
{
	const unsigned MAINROWSIZE = sizeof (PACKEDCOLORMAIN_T) * GXADJ(DIM_SECOND);	// размер одной строки в байтах
	// Таблица используемой при отображении палитры
	COLOR24_T xltrgb24 [256];
	display2_xltrgb24(xltrgb24);

#if LCDMODE_MAIN_L8
	const unsigned grx_format_MAIN = 0x05;	// GRx_FORMAT 5: CLUT8
	const unsigned grx_rdswa_MAIN = 0x07;	// GRx_RDSWA 111: (8) (7) (6) (5) (4) (3) (2) (1) [32-bit swap + 16-bit swap + 8-bit swap]
#else /* LCDMODE_MAIN_L8 */
	const unsigned grx_format_MAIN = 0x00;	// GRx_FORMAT 0: RGB565
	const unsigned grx_rdswa_MAIN = 0x06;	// GRx_RDSWA 110: (7) (8) (5) (6) (3) (4) (1) (2) [32-bit swap + 16-bit swap]
#endif /* LCDMODE_MAIN_L8 */

#if LCDMODE_PIP_L8
	const unsigned grx_format_PIP = 0x05;	// GRx_FORMAT 5: CLUT8
	const unsigned grx_rdswa_PIP = 0x07;	// GRx_RDSWA 111: (8) (7) (6) (5) (4) (3) (2) (1) [32-bit swap + 16-bit swap + 8-bit swap]
#else
	// LCDMODE_PIP_RGB565
	const unsigned grx_format_PIP = 0x00;	// GRx_FORMAT 0: RGB565
	const unsigned grx_rdswa_PIP = 0x06;	// GRx_RDSWA 110: (7) (8) (5) (6) (3) (4) (1) (2) [32-bit swap + 16-bit swap]
#endif
	////////////////////////////////////////////////////////////////
	// GR0
	SETREG32_CK(& vdc->GR0_FLM_RD, 1, 0, 0);	// GR0_R_ENB Frame Buffer Read Enable
	SETREG32_CK(& vdc->GR0_FLM1, 2, 8, 0x01);	// GR0_FLM_SEL 1: Selects GR0_FLM_NUM.
	//SETREG32_CK(& vdc->GR0_FLM2, 32, 0, (uintptr_t) & framebuff);	// GR0_BASE
	SETREG32_CK(& vdc->GR0_FLM3, 15, 16, MAINROWSIZE);	// GR0_LN_OFF
	SETREG32_CK(& vdc->GR0_FLM3, 10, 0, 0x00);	// GR0_FLM_NUM
	SETREG32_CK(& vdc->GR0_FLM4, 23, 0, MAINROWSIZE * HEIGHT);	// GR0_FLM_OFF
	SETREG32_CK(& vdc->GR0_FLM5, 11, 16, HEIGHT - 1);	// GR0_FLM_LNUM Sets the number of lines in a frame
	SETREG32_CK(& vdc->GR0_FLM5, 11, 0, HEIGHT - 1);	// GR0_FLM_LOOP
	SETREG32_CK(& vdc->GR0_FLM6, 11, 16, WIDTH - 1);	// GR0_HW Sets the width of the horizontal valid period.
	SETREG32_CK(& vdc->GR0_FLM6, 4, 28, grx_format_MAIN);	// GR0_FORMAT 0: RGB565
	SETREG32_CK(& vdc->GR0_FLM6, 3, 10, grx_rdswa_MAIN);	// GR0_RDSWA 110: (7) (8) (5) (6) (3) (4) (1) (2) [32-bit swap + 16-bit swap]
	SETREG32_CK(& vdc->GR0_AB1, 2, 0,	0x00);			// GR0_DISP_SEL 0: background color
	SETREG32_CK(& vdc->GR0_BASE, 24, 0, 0x00FF0000);	// GREEN GR0_BASE GBR Background Color B,Gb & R Signal
	SETREG32_CK(& vdc->GR0_AB2, 11, 16, TOPMARGIN);	// GR0_GRC_VS
	SETREG32_CK(& vdc->GR0_AB2, 11, 0, HEIGHT);		// GR0_GRC_VW
	SETREG32_CK(& vdc->GR0_AB3, 11, 16, LEFTMARGIN);	// GR0_GRC_HS HSYNC + HBP
	SETREG32_CK(& vdc->GR0_AB3, 11, 0, WIDTH);			// GR0_GRC_HW

	////////////////////////////////////////////////////////////////
	// GR2 - main screen

	SETREG32_CK(& vdc->GR2_FLM_RD, 1, 0, 0);	// GR2_R_ENB 0: Frame buffer reading is disabled.
	SETREG32_CK(& vdc->GR2_FLM1, 2, 8, 0x01);	// GR2_FLM_SEL 1: Selects GR2_FLM_NUM.
	//SETREG32_CK(& vdc->GR2_FLM2, 32, 0, (uintptr_t) & framebuff);	// GR2_BASE
	SETREG32_CK(& vdc->GR2_FLM3, 15, 16, MAINROWSIZE);	// GR2_LN_OFF
	SETREG32_CK(& vdc->GR2_FLM3, 10, 0, 0x00);	// GR0_FLM_NUM
	SETREG32_CK(& vdc->GR2_FLM4, 23, 0, MAINROWSIZE * HEIGHT);	// GR2_FLM_OFF
	SETREG32_CK(& vdc->GR2_FLM5, 11, 16, HEIGHT - 1);	// GR2_FLM_LNUM Sets the number of lines in a frame
	SETREG32_CK(& vdc->GR2_FLM5, 11, 0, HEIGHT - 1);	// GR2_FLM_LOOP Sets the number of lines in a frame
	SETREG32_CK(& vdc->GR2_FLM6, 11, 16, WIDTH - 1);	// GR2_HW Sets the width of the horizontal valid period.
	SETREG32_CK(& vdc->GR2_FLM6, 4, 28, grx_format_MAIN);	// GR2_FORMAT 0: RGB565
	SETREG32_CK(& vdc->GR2_FLM6, 3, 10, grx_rdswa_MAIN);	// GR2_RDSWA 110: (7) (8) (5) (6) (3) (4) (1) (2) [32-bit swap + 16-bit swap]
	SETREG32_CK(& vdc->GR2_AB1, 2, 0,	0x00);			// GR2_DISP_SEL 0: Background color display
	SETREG32_CK(& vdc->GR2_BASE, 24, 0, 0x00000000);	// BLUE GR2_BASE GBR Background Color B,Gb & R Signal
	SETREG32_CK(& vdc->GR2_AB2, 11, 16, TOPMARGIN);	// GR2_GRC_VS
	SETREG32_CK(& vdc->GR2_AB2, 11, 0, HEIGHT);		// GR2_GRC_VW
	SETREG32_CK(& vdc->GR2_AB3, 11, 16, LEFTMARGIN);	// GR2_GRC_HS
	SETREG32_CK(& vdc->GR2_AB3, 11, 0, WIDTH);			// GR2_GRC_HW

//#if LCDMODE_MAIN_L8
	// 33.1.15 CLUT Table
	#define     VDC5_CH0_GR0_CLUT_TBL           (*(volatile uint32_t*)0xFCFF6000)
	//#define     VDC5_CH0_GR1_CLUT_TBL           (*(volatile uint32_t*)0xFCFF6400)
	#define     VDC5_CH0_GR2_CLUT_TBL           (*(volatile uint32_t*)0xFCFF6800)
	#define     VDC5_CH0_GR3_CLUT_TBL           (*(volatile uint32_t*)0xFCFF6C00)

	SETREG32_CK(& vdc->GR2_CLUT, 1, 16, 0x00);			// GR2_CLT_SEL
	VDC5_fillLUT_L8(& VDC5_CH0_GR2_CLUT_TBL, xltrgb24);
	SETREG32_CK(& vdc->GR2_CLUT, 1, 16, 0x01);			// GR2_CLT_SEL
	//vdc->GR2_CLUT ^= (1uL << 16);	// GR2_CLT_SEL Switch to filled table
//#endif /* LCDMODE_MAIN_L8 */

//#if LCDMODE_PIP_L8
	// PIP on GR3
	SETREG32_CK(& vdc->GR3_CLUT_INT, 1, 16, 0x00);			// GR3_CLT_SEL
	VDC5_fillLUT_L8(& VDC5_CH0_GR3_CLUT_TBL, xltrgb24);
	SETREG32_CK(& vdc->GR3_CLUT_INT, 1, 16, 0x01);			// GR3_CLT_SEL
//#endif /* LCDMODE_PIP_L8 */

	////////////////////////////////////////////////////////////////
	// GR3 - PIP screen
	pipparams_t pipwnd;
	display2_getpipparams(& pipwnd);

	SETREG32_CK(& vdc->GR3_FLM_RD, 1, 0, 0);			// GR3_R_ENB Frame Buffer Read Disable
	SETREG32_CK(& vdc->GR3_FLM1, 2, 8, 0x01);			// GR3_FLM_SEL 1: Selects GR3_FLM_NUM.
	//SETREG32_CK(& vdc->GR3_FLM2, 32, 0, (uintptr_t) & framebuff);	// GR3_BASE
	SETREG32_CK(& vdc->GR3_FLM3, 15, 16, MAINROWSIZE);		// GR3_LN_OFF
	SETREG32_CK(& vdc->GR3_FLM3, 10, 0, 0x00);			// GR3_FLM_NUM
	SETREG32_CK(& vdc->GR3_FLM4, 23, 0, MAINROWSIZE * HEIGHT);	// GR0_FLM_OFF
	SETREG32_CK(& vdc->GR3_FLM5, 11, 16, HEIGHT - 1);	// GR3_FLM_LNUM Sets the number of lines in a frame
	SETREG32_CK(& vdc->GR3_FLM5, 11, 0, HEIGHT - 1);	// GR3_FLM_LOOP Sets the number of lines in a frame
	SETREG32_CK(& vdc->GR3_FLM6, 11, 16, WIDTH - 1);	// GR3_HW Sets the width of the horizontal valid period.
	SETREG32_CK(& vdc->GR3_FLM6, 4, 28, grx_format_PIP);	// GR3_FORMAT 0: RGB565 or 5: CLUT8
	SETREG32_CK(& vdc->GR3_FLM6, 3, 10, grx_rdswa_PIP);	// GR3_RDSWA 110: (7) (8) (5) (6) (3) (4) (1) (2) [32-bit swap + 16-bit swap]
	SETREG32_CK(& vdc->GR3_AB1, 2, 0,	0x01);			// GR3_DISP_SEL 1: Lower-layer graphics display
	SETREG32_CK(& vdc->GR3_BASE, 24, 0, 0x000000FF);	// RED GR3_BASE GBR Background Color B,Gb & R Signal
	SETREG32_CK(& vdc->GR3_AB2, 11, 16, TOPMARGIN);	// GR3_GRC_VS
	SETREG32_CK(& vdc->GR3_AB2, 11, 0, HEIGHT);		// GR3_GRC_VW
	SETREG32_CK(& vdc->GR3_AB3, 11, 16, LEFTMARGIN);	// GR3_GRC_HS
	SETREG32_CK(& vdc->GR3_AB3, 11, 0, WIDTH);			// GR3_GRC_HW

//#if LCDMODE_PIP_L8 || LCDMODE_PIP_RGB565

	/* Adjust GR3 parameters for PIP mode (GR2 - mani window, GR3 - PIP) */

	SETREG32_CK(& vdc->GR3_FLM_RD, 1, 0, 0);			// GR3_R_ENB Frame Buffer Read Enable
	SETREG32_CK(& vdc->GR3_AB1, 2, 0,	0x01);			// GR3_DISP_SEL 1: Lower-layer graphics display
	SETREG32_CK(& vdc->GR3_FLM3, 15, 16, GXADJ(pipwnd.w) * sizeof (PACKEDCOLORPIP_T));		// GR3_LN_OFF
	SETREG32_CK(& vdc->GR3_FLM3, 10, 0, 0x00);			// GR3_FLM_NUM
	SETREG32_CK(& vdc->GR3_FLM4, 23, 0, GXADJ(pipwnd.w) * pipwnd.h * sizeof (PACKEDCOLORPIP_T));	// GR3_FLM_OFF
	SETREG32_CK(& vdc->GR3_FLM5, 11, 16, pipwnd.h - 1);	// GR3_FLM_LNUM Sets the number of lines in a frame
	SETREG32_CK(& vdc->GR3_FLM5, 11, 0, pipwnd.h - 1);	// GR3_FLM_LOOP Sets the number of lines in a frame
	SETREG32_CK(& vdc->GR3_FLM6, 11, 16, pipwnd.w - 1);	// GR3_HW Sets the width of the horizontal valid period.
	SETREG32_CK(& vdc->GR3_AB2, 11, 16, TOPMARGIN + pipwnd.y);	// GR3_GRC_VS
	SETREG32_CK(& vdc->GR3_AB2, 11, 0, pipwnd.h);		// GR3_GRC_VW
	SETREG32_CK(& vdc->GR3_AB3, 11, 16, LEFTMARGIN + pipwnd.x);	// GR3_GRC_HS
	SETREG32_CK(& vdc->GR3_AB3, 11, 0, pipwnd.w);			// GR3_GRC_HW

//#endif /* LCDMODE_PIP_L8 || LCDMODE_PIP_RGB565 */
}
/* Palette reload */
static void vdc5fb_L8_palette(struct st_vdc5 * const vdc)
{
#define     VDC5_CH0_GR0_CLUT_TBL           (*(volatile uint32_t*)0xFCFF6000)
//#define     VDC5_CH0_GR1_CLUT_TBL           (*(volatile uint32_t*)0xFCFF6400)
#define     VDC5_CH0_GR2_CLUT_TBL           (*(volatile uint32_t*)0xFCFF6800)
#define     VDC5_CH0_GR3_CLUT_TBL           (*(volatile uint32_t*)0xFCFF6C00)
	// Таблица используемой при отображении палитры
	COLOR24_T xltrgb24 [256];
	display2_xltrgb24(xltrgb24);
#if LCDMODE_MAIN_L8
	SETREG32_CK(& vdc->GR2_CLUT, 1, 16, 0x00);			// GR2_CLT_SEL
	VDC5_fillLUT_L8(& VDC5_CH0_GR2_CLUT_TBL, xltrgb24);
	SETREG32_CK(& vdc->GR2_CLUT, 1, 16, 0x01);			// GR2_CLT_SEL
#endif /* LCDMODE_PIP_L8 */
#if LCDMODE_PIP_L8
	// PIP on GR3
	SETREG32_CK(& vdc->GR3_CLUT_INT, 1, 16, 0x00);			// GR3_CLT_SEL
	VDC5_fillLUT_L8(& VDC5_CH0_GR3_CLUT_TBL, xltrgb24);
	SETREG32_CK(& vdc->GR3_CLUT_INT, 1, 16, 0x01);			// GR3_CLT_SEL
#endif /* LCDMODE_PIP_L8 */
}

static void vdc5fb_init_outcnt(struct st_vdc5 * const vdc)
{
	////////////////////////////////////////////////////////////////
	// OUT
	SETREG32_CK(& vdc->OUT_CLK_PHASE, 1, 8, 0x00);	// OUTCNT_LCD_EDGE 0: Output changed at the rising edge of LCD_CLK pin, data latched at falling edge
	SETREG32_CK(& vdc->OUT_SET, 2, 8, 0x00);	// OUT_FRQ_SEL Clock Frequency Control 0: 100% speed — (parallel RGB)
	SETREG32_CK(& vdc->OUT_SET, 2, 12, 0x02);	// OUT_FORMAT Output Format Select 2: RGB565
}

static void vdc5fb_init_tcon(struct st_vdc5 * const vdc)
{

	////////////////////////////////////////////////////////////////
	// TCON

	// Vertical sync generation parameters

	// VSYNC signal
	SETREG32_CK(& vdc->TCON_TIM_STVA1, 11, 16, 0);		// TCON_STVA_VS
	SETREG32_CK(& vdc->TCON_TIM_STVA1, 11, 0, VSYNC);	// TCON_STVA_VW

	// Vertical enable signal
	SETREG32_CK(& vdc->TCON_TIM_STVB1, 11, 16, TOPMARGIN);	// TCON_STVB_VS
	SETREG32_CK(& vdc->TCON_TIM_STVB1, 11, 0, HEIGHT);	// TCON_STVB_VW

	// Horisontal sync generation parameters
	SETREG32_CK(& vdc->TCON_TIM, 11, 16, HFULL);		// TCON_HALF
	SETREG32_CK(& vdc->TCON_TIM, 11, 0, 0);				// TCON_OFFSET

	//SETREG32_CK(& vdc->TCON_TIM_POLA2, 2, 12, 0x00);	// TCON_POLA_MD
	//SETREG32_CK(& vdc->TCON_TIM_POLB2, 2, 12, 0x00);	// TCON_POLB_MD

	// HSYNC signal
	SETREG32_CK(& vdc->TCON_TIM_STH1, 11, 16,	0);		// TCON_STH_HS
	SETREG32_CK(& vdc->TCON_TIM_STH1, 11, 0, HSYNC);	// TCON_STH_HW
	// Source strobe signal - used as DE
	SETREG32_CK(& vdc->TCON_TIM_STB1, 11, 16, LEFTMARGIN);		// TCON_STB_HS
	SETREG32_CK(& vdc->TCON_TIM_STB1, 11, 0, WIDTH);	// TCON_STB_HW

	/* hardware-dependent control signals */
	// LCD0_TCON4 - VSYNC P7_5
	// LCD0_TCON5 - HSYNC P7_6
	// LCD0_TCON6 - DE P7_7
	// Output pins route
	//SETREG32_CK(& vdc->TCON_TIM_STVA2, 3, 0, 0xXX);	// Output Signal Select for LCD_TCON0 pin - 
	//SETREG32_CK(& vdc->TCON_TIM_STVB2, 3, 0, 0xXX);	// Output Signal Select for LCD_TCON1 pin - 
	//SETREG32_CK(& vdc->TCON_TIM_STH2, 3, 0, 0xXX);	// Output Signal Select for LCD_TCON2 pin - 
	SETREG32_CK(& vdc->TCON_TIM_CPV2, 3, 0, 0x00);		// Output Signal Select for LCD_TCON4 Pin - VSYNC 0: STVA/VS
	SETREG32_CK(& vdc->TCON_TIM_POLA2, 3, 0, 0x02);		// Output Signal Select for LCD_TCON5 Pin - HSYNC 2: STH/SP/HS
	SETREG32_CK(& vdc->TCON_TIM_POLB2, 3, 0, 0x07);		// Output Signal Select for LCD_TCON6 Pin - DE 7: DE
	// HSYMC polarity
	SETREG32_CK(& vdc->TCON_TIM_STH2, 1, 4, HSYNCNEG * 0x01);		// TCON_STH_INV
	// VSYNC polarity
	SETREG32_CK(& vdc->TCON_TIM_STVA2, 1, 4, VSYNCNEG * 0x01);		// TCON_STVA_INV
	// DE polarity
	SETREG32_CK(& vdc->TCON_TIM_DE, 1, 0, DENEG * 0x01);			// TCON_DE_INV

#if 0
	static const unsigned char tcon_sel[LCD_MAX_TCON]
		= { 0, 1, 2, 7, 4, 5, 6, };
	struct fb_videomode *mode = priv->videomode;
	struct vdc5fb_pdata *pdata = priv_to_pdata(priv);
	uint32_t vs_s, vs_w, ve_s, ve_w;
	uint32_t hs_s, hs_w, he_s, he_w;
	uint32_t tmp1, tmp2;

	tmp1 = TCON_OFFSET(0);
	tmp1 |= TCON_HALF(priv->res_fh / 2);
	vdc5fb_write(priv, TCON_TIM, tmp1);
	tmp2 = 0;
#if 0
	tmp2 = TCON_DE_INV;
#endif
	vdc5fb_write(priv, TCON_TIM_DE, tmp2);

	vs_s = (2 * 0);
	vs_w = (2 * mode->vsync_len);
	ve_s = (2 * (mode->vsync_len + mode->upper_margin));
	ve_w = (2 * priv->panel_pixel_yres);

	tmp1 = TCON_VW(vs_w);
	tmp1 |= TCON_VS(vs_s);
	vdc5fb_write(priv, TCON_TIM_STVA1, tmp1);
	if (pdata->tcon_sel[LCD_TCON0] == TCON_SEL_UNUSED)
		tmp2 = TCON_SEL(tcon_sel[LCD_TCON0]);
	else
		tmp2 = TCON_SEL(pdata->tcon_sel[LCD_TCON0]);
	if (!(mode->sync & FB_SYNC_VERT_HIGH_ACT))
		tmp2 |= TCON_INV;
	vdc5fb_write(priv, TCON_TIM_STVA2, tmp2);

	tmp1 = TCON_VW(ve_w);
	tmp1 |= TCON_VS(ve_s);
	vdc5fb_write(priv, TCON_TIM_STVB1, tmp1);
	if (pdata->tcon_sel[LCD_TCON1] == TCON_SEL_UNUSED)
		tmp2 = TCON_SEL(tcon_sel[LCD_TCON1]);
	else
		tmp2 = TCON_SEL(pdata->tcon_sel[LCD_TCON1]);
#if 0
	tmp2 |= TCON_INV;
#endif
	vdc5fb_write(priv, TCON_TIM_STVB2, tmp2);

	hs_s = 0;
	hs_w = mode->hsync_len;
	he_s = (mode->hsync_len + mode->left_margin);
	he_w = priv->panel_pixel_xres;

	tmp1 = TCON_HW(hs_w);
	tmp1 |= TCON_HS(hs_s);
	vdc5fb_write(priv, TCON_TIM_STH1, tmp1);
	if (pdata->tcon_sel[LCD_TCON2] == TCON_SEL_UNUSED)
		tmp2 = TCON_SEL(tcon_sel[LCD_TCON2]);
	else
		tmp2 = TCON_SEL(pdata->tcon_sel[LCD_TCON2]);
	if (!(mode->sync & FB_SYNC_HOR_HIGH_ACT))
		tmp2 |= TCON_INV;
#if 0
	tmp2 |= TCON_HS_SEL;
#endif
	vdc5fb_write(priv, TCON_TIM_STH2, tmp2);

	tmp1 = TCON_HW(he_w);
	tmp1 |= TCON_HS(he_s);
	vdc5fb_write(priv, TCON_TIM_STB1, tmp1);
	if (pdata->tcon_sel[LCD_TCON3] == TCON_SEL_UNUSED)
		tmp2 = TCON_SEL(tcon_sel[LCD_TCON3]);
	else
		tmp2 = TCON_SEL(pdata->tcon_sel[LCD_TCON3]);
#if 0
	tmp2 |= TCON_INV;
	tmp2 |= TCON_HS_SEL;
#endif
	vdc5fb_write(priv, TCON_TIM_STB2, tmp2);

	tmp1 = TCON_HW(hs_w);
	tmp1 |= TCON_HS(hs_s);
	vdc5fb_write(priv, TCON_TIM_CPV1, tmp1);
	if (pdata->tcon_sel[LCD_TCON4] == TCON_SEL_UNUSED)
		tmp2 = TCON_SEL(tcon_sel[LCD_TCON4]);
	else
		tmp2 = TCON_SEL(pdata->tcon_sel[LCD_TCON4]);
#if 0
	tmp2 |= TCON_INV;
	tmp2 |= TCON_HS_SEL;
#endif
	vdc5fb_write(priv, TCON_TIM_CPV2, tmp2);

	tmp1 = TCON_HW(he_w);
	tmp1 |= TCON_HS(he_s);
	vdc5fb_write(priv, TCON_TIM_POLA1, tmp1);
	if (pdata->tcon_sel[LCD_TCON5] == TCON_SEL_UNUSED)
		tmp2 = TCON_SEL(tcon_sel[LCD_TCON5]);
	else
		tmp2 = TCON_SEL(pdata->tcon_sel[LCD_TCON5]);
#if 0
	tmp2 |= TCON_HS_SEL;
	tmp2 |= TCON_INV;
	tmp2 |= TCON_MD;
#endif
	vdc5fb_write(priv, TCON_TIM_POLA2, tmp2);

	tmp1 = TCON_HW(he_w);
	tmp1 |= TCON_HS(he_s);
	vdc5fb_write(priv, TCON_TIM_POLB1, tmp1);
	if (pdata->tcon_sel[LCD_TCON6] == TCON_SEL_UNUSED)
		tmp2 = TCON_SEL(tcon_sel[LCD_TCON6]);
	else
		tmp2 = TCON_SEL(pdata->tcon_sel[LCD_TCON6]);
#if 0
	tmp2 |= TCON_INV;
	tmp2 |= TCON_HS_SEL;
	tmp2 |= TCON_MD;
#endif
	vdc5fb_write(priv, TCON_TIM_POLB2, tmp2);

#endif
}

static void vdc5fb_update_all(struct st_vdc5 * const vdc)
{
		
	/* update all registers */

	
	//vdc5_update(& vdc->IMGCNT_UPDATE, "IMGCNT_UPDATE",
	//		(1 << 0) |	// IMGCNT_VEN Image Quality Adjustment Block Register Update
	//		0
	//	);

	vdc5_update(& vdc->SC0_SCL0_UPDATE, "SC0_SCL0_UPDATE",
			(1 << 13) |	// SC0_SCL0_VEN_D	Scaling-Up Control and Frame Buffer Read Control Register Update
		//	(1 << 12) |	// SC0_SCL0_VEN_C	Scaling-Down Control and Frame Buffer Read Control Register Upda
			(1 << 8) |	// SC0_SCL0_UPDATE	SYNC Control Register Update
			(1 << 4) |	// SC0_SCL0_VEN_B	Synchronization Control and Scaling-up Control Register Update
		//	(1 << 0) |	// SC0_SCL0_VEN_A	Scaling-Down Control Register Update
			0
		);

#if 0
	vdc5_update(& vdc->SC0_SCL1_UPDATE, "SC0_SCL1_UPDATE",
			(1 << 20) |	// SC0_SCL1_UPDATE_B
			(1 << 16) |	// SC0_SCL1_UPDATE_A
			(1 << 4) |	// SC0_SCL1_VEN_B
			(1 << 0) |	// SC0_SCL1_VEN_A
			0
		);
#endif

	vdc5_update(& vdc->GR0_UPDATE, "GR0_UPDATE",
			(1 << 8) |	// GR0_UPDATE Frame Buffer Read Control Register Update
			(1 << 4) |	// GR0_P_VEN Graphics Display Register Update
			(1 << 0) |	// GR0_IBUS_VEN Frame Buffer Read Control Register Update
			0
		);
	vdc5_update(& vdc->GR2_UPDATE, "GR2_UPDATE",
			(1 << 8) |	// GR2_UPDATE Frame Buffer Read Control Register Update
			(1 << 4) |	// GR2_P_VEN Graphics Display Register Update
			(1 << 0) |	// GR2_IBUS_VEN Frame Buffer Read Control Register Update
			0
		);
	vdc5_update(& vdc->GR3_UPDATE, "GR3_UPDATE",
			(1 << 8) |	// GR3_UPDATE Frame Buffer Read Control Register Update
			(1 << 4) |	// GR3_P_VEN Graphics Display Register Update
			(1 << 0) |	// GR3_IBUS_VEN Frame Buffer Read Control Register Update
			0
		);

	vdc5_update(& vdc->GR_VIN_UPDATE, "GR_VIN_UPDATE",
			(1 << 8) |	// GR_VIN_UPDATE Graphics Display Register Update
			(1 << 4) |	// GR_VIN_P_VEN Graphics Display Register Update
			0
		);

	vdc5_update(& vdc->OUT_UPDATE, "OUT_UPDATE",
			(1 << 0) |	// OUTCNT_VEN
			0
		);

	vdc5_update(& vdc->TCON_UPDATE, "TCON_UPDATE",
			(1 << 0) |	// TCON_VEN
			0
		);

#if 0
	uint32_t tmp;

	tmp = IMGCNT_VEN;
	vdc5fb_update_regs(priv, IMGCNT_UPDATE, tmp, 1);

	tmp = (SC_SCL0_VEN_A | SC_SCL0_VEN_B | SC_SCL0_UPDATE
		| SC_SCL0_VEN_C | SC_SCL0_VEN_D);
	vdc5fb_update_regs(priv, SC0_SCL0_UPDATE, tmp, 1);

	tmp = (SC_SCL1_VEN_A | SC_SCL1_VEN_B | SC_SCL1_UPDATE_A
		| SC_SCL1_UPDATE_B);
	vdc5fb_update_regs(priv, SC0_SCL1_UPDATE, tmp, 1);

	tmp = (GR_IBUS_VEN | GR_P_VEN | GR_UPDATE);
	vdc5fb_update_regs(priv, GR0_UPDATE, tmp, 1);
	vdc5fb_update_regs(priv, GR1_UPDATE, tmp, 1);

	tmp = ADJ_VEN;
	vdc5fb_write(priv, ADJ0_UPDATE, tmp);
	vdc5fb_write(priv, ADJ1_UPDATE, tmp);

	tmp = (GR_IBUS_VEN | GR_P_VEN | GR_UPDATE);
	vdc5fb_update_regs(priv, GR2_UPDATE, tmp, 1);
	vdc5fb_update_regs(priv, GR3_UPDATE, tmp, 1);

	tmp = (GR_P_VEN | GR_UPDATE);
	vdc5fb_update_regs(priv, GR_VIN_UPDATE, tmp, 1);

#ifdef OUTPUT_IMAGE_GENERATOR
	tmp = (SC_SCL_VEN_A | SC_SCL_VEN_B | SC_SCL_UPDATE
		| SC_SCL_VEN_C | SC_SCL_VEN_D);
	vdc5fb_update_regs(priv, OIR_SCL0_UPDATE, tmp, 1);
	vdc5fb_update_regs(priv, OIR_SCL1_UPDATE, tmp, 1);

	tmp = (GR_IBUS_VEN | GR_P_VEN | GR_UPDATE);
	vdc5fb_update_regs(priv, GR_OIR_UPDATE, tmp, 1);
#endif

	tmp = OUTCNT_VEN;
	vdc5fb_update_regs(priv, OUT_UPDATE, tmp, 1);
	tmp = GAM_VEN;
	vdc5fb_update_regs(priv, GAM_G_UPDATE, tmp, 1);
	vdc5fb_update_regs(priv, GAM_B_UPDATE, tmp, 1);
	vdc5fb_update_regs(priv, GAM_R_UPDATE, tmp, 1);
	tmp = TCON_VEN;
	vdc5fb_update_regs(priv, TCON_UPDATE, tmp, 1);

#endif
}

void
arm_hardware_ltdc_initialize(void)
{
	struct st_vdc5 * const vdc = & VDC50;

	PRINTF(PSTR("arm_hardware_ltdc_initialize start, WIDTH=%d, HEIGHT=%d\n"), WIDTH, HEIGHT);
	//const unsigned ROWSIZE = sizeof framebuff [0];	// размер одной строки в байтах


	/* ---- Supply clock to the video display controller 5  ---- */
	CPG.STBCR9 &= ~ CPG_STBCR9_MSTP91;	// Module Stop 91 0: The video display controller 5 runs.
	(void) CPG.STBCR9;			/* Dummy read */

	vdc5fb_init_syscnt(vdc);
	vdc5fb_init_sync(vdc);
	vdc5fb_init_scalers(vdc);
	vdc5fb_init_graphics(vdc);
	vdc5fb_init_outcnt(vdc);
	vdc5fb_init_tcon(vdc);

	vdc5fb_update_all(vdc);




	/* Configure the LCD Control pins */
	HARDWARE_LTDC_INITIALIZE(BOARD_DEMODE);	// подключение к выводам процессора сигналов периферийного контроллера

#if defined (BOARD_DEVALUE)
	HARDWARE_LTDC_SET_DISP(BOARD_DEMODE, BOARD_DEVALUE);
#elif defined (BOARD_DERESET)
	/* SONY PSP-1000 display (4.3") required. */
	HARDWARE_LTDC_SET_DISP(BOARD_DEMODE, 0);
	local_delay_ms(150);
	HARDWARE_LTDC_SET_DISP(BOARD_DEMODE, 1);
#endif
#if defined (BOARD_MODEVALUE)
	HARDWARE_LTDC_SET_MODE(BOARD_MODEVALUE);
#endif

#if LCDMODE_PIP_RGB565 || LCDMODE_PIP_L8
	arm_hardware_ltdc_pip_off();
#endif /* LCDMODE_PIP_RGB565 || LCDMODE_PIP_L8 */

	PRINTF(PSTR("arm_hardware_ltdc_initialize done\n"));
}

/* Palette reload */
void arm_hardware_ltdc_L8_palette(void)
{
	struct st_vdc5 * const vdc = & VDC50;
	vdc5fb_L8_palette(vdc);
}

/* set bottom buffer start */
void arm_hardware_ltdc_pip_set(uintptr_t p)
{
	struct st_vdc5 * const vdc = & VDC50;

	SETREG32_CK(& vdc->GR3_FLM2, 32, 0, p);			// GR3_BASE
	SETREG32_CK(& vdc->GR3_FLM_RD, 1, 0, 1);		// GR3_R_ENB Frame Buffer Read Enable 1: Frame buffer reading is enabled.
	SETREG32_CK(& vdc->GR3_AB1, 2, 0,	0x03);		// GR3_DISP_SEL 3: Blended display of lower-layer graphics and current graphics

	// GR3_IBUS_VEN in GR3_UPDATE is 1.
	// GR3_IBUS_VEN and GR3_P_VEN in GR3_UPDATE are 1.
	// GR3_P_VEN in GR3_UPDATE is 1.

	//vdc5_update(& vdc->GR3_UPDATE, "GR3_UPDATE",
		vdc->GR3_UPDATE = (
			(1 << 8) |	// GR3_UPDATE Frame Buffer Read Control Register Update
			(1 << 4) |	// GR3_P_VEN Graphics Display Register Update
			(1 << 0) |	// GR3_IBUS_VEN Frame Buffer Read Control Register Update
			0
		);
}

void arm_hardware_ltdc_pip_off(void)	// set PIP framebuffer address
{
	struct st_vdc5 * const vdc = & VDC50;

	SETREG32_CK(& vdc->GR3_FLM_RD, 1, 0, 0);			// GR3_R_ENB Frame Buffer Read Enable 0: Frame buffer reading is disabled.
	SETREG32_CK(& vdc->GR3_AB1, 2, 0,	0x01);			// GR3_DISP_SEL 1: Lower-layer graphics display

	//vdc5_update(& vdc->GR3_UPDATE, "GR3_UPDATE",
		vdc->GR3_UPDATE = (
			(1 << 8) |	// GR3_UPDATE Frame Buffer Read Control Register Update
			(1 << 4) |	// GR3_P_VEN Graphics Display Register Update
			(1 << 0) |	// GR3_IBUS_VEN Frame Buffer Read Control Register Update
			0
		);
}
/* set bottom buffer start */
void arm_hardware_ltdc_main_set(uintptr_t p)
{
	struct st_vdc5 * const vdc = & VDC50;

	SETREG32_CK(& vdc->GR2_FLM_RD, 1, 0, 1);		// GR2_R_ENB Frame Buffer Read Enable 1: Frame buffer reading is enabled.
	SETREG32_CK(& vdc->GR2_FLM2, 32, 0, p);			// GR2_BASE
	SETREG32_CK(& vdc->GR2_AB1, 2, 0,	0x02);		// GR2_DISP_SEL 2: Current graphics display

	// GR2_IBUS_VEN in GR2_UPDATE is 1.
	// GR2_IBUS_VEN and GR2_P_VEN in GR2_UPDATE are 1.
	// GR2_P_VEN in GR2_UPDATE is 1.

	//vdc5_update(& vdc->GR3_UPDATE, "GR3_UPDATE",
		vdc->GR2_UPDATE = (
			(1 << 8) |	// GR2_UPDATE Frame Buffer Read Control Register Update
			(1 << 4) |	// GR2_P_VEN Graphics Display Register Update
			(1 << 0) |	// GR2_IBUS_VEN Frame Buffer Read Control Register Update
			0
		);
}

#elif CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

/** @defgroup LTDC_Pixelformat 
  * @{
  */
#define LTDC_Pixelformat_ARGB8888                  ((uint32_t)0x00000000)
#define LTDC_Pixelformat_RGB888                    ((uint32_t)0x00000001)
#define LTDC_Pixelformat_RGB565                    ((uint32_t)0x00000002)
#define LTDC_Pixelformat_ARGB1555                  ((uint32_t)0x00000003)
#define LTDC_Pixelformat_ARGB4444                  ((uint32_t)0x00000004)
#define LTDC_Pixelformat_L8                        ((uint32_t)0x00000005)
#define LTDC_Pixelformat_AL44                      ((uint32_t)0x00000006)
#define LTDC_Pixelformat_AL88                      ((uint32_t)0x00000007)
/** @defgroup LTDC_BlendingFactor1 
  * @{
  */

/*
This register defines the blending factors F1 and F2.
The general blending formula is: BC = BF1 x C + BF2 x Cs
• BC = Blended color
• BF1 = Blend Factor 1
• C = Current layer color
• BF2 = Blend Factor 2
• Cs = subjacent layers blended color
*/
#define LTDC_BlendingFactor1_CA                       (4 << LTDC_LxBFCR_BF1_Pos)	//((uint32_t)0x00000400)
#define LTDC_BlendingFactor1_PAxCA                    (6 << LTDC_LxBFCR_BF1_Pos)	//((uint32_t)0x00000600)

/**
  * @}
  */
      
/** @defgroup LTDC_BlendingFactor2
  * @{
  */

#define LTDC_BlendingFactor2_CA                       (5 << LTDC_LxBFCR_BF2_Pos)	//((uint32_t)0x00000005)
#define LTDC_BlendingFactor2_PAxCA                    (7 << LTDC_LxBFCR_BF2_Pos)	//((uint32_t)0x00000007)

/**
  * @}
  */

/** @defgroup LTDC_Reload 
  * @{
  */
#define LTDC_IMReload                     LTDC_SRCR_IMR                         /*!< Immediately Reload. */
#define LTDC_VBReload                     LTDC_SRCR_VBR                         /*!< Vertical Blanking Reload. */

/**
  * @}
  */
  
/** @defgroup LTDC_HSPolarity 
  * @{
  */
#define LTDC_HSPolarity_AL                0                /*!< Horizontal Synchronization is active low. */
#define LTDC_HSPolarity_AH                LTDC_GCR_HSPOL                        /*!< Horizontal Synchronization is active high. */

/**
  * @}
  */
  
/** @defgroup LTDC_VSPolarity 
  * @{
  */
#define LTDC_VSPolarity_AL                0                /*!< Vertical Synchronization is active low. */
#define LTDC_VSPolarity_AH                LTDC_GCR_VSPOL                        /*!< Vertical Synchronization is active high. */

/**
  * @}
  */
  
/** @defgroup LTDC_DEPolarity 
  * @{
  */
// 0: Not Data Enable polarity is active low
// 1: Not Data Enable polarity is active high 
#define LTDC_DEPolarity_AL                0                /*!< Data Enable, is active low. */
#define LTDC_DEPolarity_AH                LTDC_GCR_DEPOL                        /*!< Data Enable, is active high. */

/**
  * @}
  */

/** @defgroup LTDC_PCPolarity 
  * @{
  */
#define LTDC_PCPolarity_IPC               0                /*!< input pixel clock. */
#define LTDC_PCPolarity_IIPC              LTDC_GCR_PCPOL                        /*!< inverted input pixel clock. */


/* Exported types ------------------------------------------------------------*/
 
/** 
  * @brief  LTDC Init structure definition  
  */

typedef struct
{
  uint32_t LTDC_HSPolarity;                 /*!< configures the horizontal synchronization polarity.
                                                 This parameter can be one value of @ref LTDC_HSPolarity */

  uint32_t LTDC_VSPolarity;                 /*!< configures the vertical synchronization polarity.
                                                 This parameter can be one value of @ref LTDC_VSPolarity */

  uint32_t LTDC_DEPolarity;                 /*!< configures the data enable polarity. This parameter can
                                                 be one of value of @ref LTDC_DEPolarity */

  uint32_t LTDC_PCPolarity;                 /*!< configures the pixel clock polarity. This parameter can
                                                 be one of value of @ref LTDC_PCPolarity */

  uint32_t LTDC_HorizontalSync;             /*!< configures the number of Horizontal synchronization 
                                                 width. This parameter must range from 0x000 to 0xFFF. */

  uint32_t LTDC_VerticalSync;               /*!< configures the number of Vertical synchronization 
                                                 heigh. This parameter must range from 0x000 to 0x7FF. */

  uint32_t LTDC_AccumulatedHBP;             /*!< configures the accumulated horizontal back porch width.
                                                 This parameter must range from LTDC_HorizontalSync to 0xFFF. */

  uint32_t LTDC_AccumulatedVBP;             /*!< configures the accumulated vertical back porch heigh.
                                                 This parameter must range from LTDC_VerticalSync to 0x7FF. */
            
  uint32_t LTDC_AccumulatedActiveW;         /*!< configures the accumulated active width. This parameter 
                                                 must range from LTDC_AccumulatedHBP to 0xFFF. */

  uint32_t LTDC_AccumulatedActiveH;         /*!< configures the accumulated active heigh. This parameter 
                                                 must range from LTDC_AccumulatedVBP to 0x7FF. */

  uint32_t LTDC_TotalWidth;                 /*!< configures the total width. This parameter 
                                                 must range from LTDC_AccumulatedActiveW to 0xFFF. */

  uint32_t LTDC_TotalHeigh;                 /*!< configures the total heigh. This parameter 
                                                 must range from LTDC_AccumulatedActiveH to 0x7FF. */
            

  uint32_t LTDC_BackgroundColor;         /*!< configures the background  */

} LTDC_InitTypeDef;

/** 
  * @brief  LTDC Layer structure definition  
  */

typedef struct
{
  uint32_t LTDC_HorizontalStart;            /*!< Configures the Window Horizontal Start Position.
                                                 This parameter must range from 0x000 to 0xFFF. */
            
  uint32_t LTDC_HorizontalStop;             /*!< Configures the Window Horizontal Stop Position.
                                                 This parameter must range from 0x0000 to 0xFFFF. */
  
  uint32_t LTDC_VerticalStart;              /*!< Configures the Window vertical Start Position.
                                                 This parameter must range from 0x000 to 0xFFF. */

  uint32_t LTDC_VerticalStop;               /*!< Configures the Window vaertical Stop Position.
                                                 This parameter must range from 0x0000 to 0xFFFF. */
  
  uint32_t LTDC_PixelFormat;                /*!< Specifies the pixel format. This parameter can be 
                                                 one of value of @ref LTDC_Pixelformat */

  uint32_t LTDC_ConstantAlpha;              /*!< Specifies the constant alpha used for blending.
                                                 This parameter must range from 0x00 to 0xFF. */

  uint32_t LTDC_DefaultColor;           /*!< Configures the default color value.. */


  uint32_t LTDC_BlendingFactor_1;           /*!< Select the blending factor 1. This parameter 
                                                 can be one of value of @ref LTDC_BlendingFactor1 */

  uint32_t LTDC_BlendingFactor_2;           /*!< Select the blending factor 2. This parameter 
                                                 can be one of value of @ref LTDC_BlendingFactor2 */
            
  uint32_t LTDC_CFBStartAdress;             /*!< Configures the color frame buffer address */

  uint32_t LTDC_CFBLineLength;              /*!< Configures the color frame buffer line length. 
                                                 This parameter must range from 0x0000 to 0x1FFF. */

  uint32_t LTDC_CFBPitch;                   /*!< Configures the color frame buffer pitch in bytes.
                                                 This parameter must range from 0x0000 to 0x1FFF. */
                                                 
  uint32_t LTDC_CFBLineNumber;              /*!< Specifies the number of line in frame buffer. 
                                                 This parameter must range from 0x000 to 0x7FF. */
} LTDC_Layer_InitTypeDef;

#if LCDMODE_MAIN_L24


// Создаём палитру выполняющую просто трансляцию значения
static void
fillLUT_L24(
	LTDC_Layer_TypeDef* LTDC_Layerx
	)
{
	unsigned color;

	for (color = 0; color < 256; ++ color)
	{
		uint_fast8_t r = color, g = color, b = color;
		
		/* запись значений в регистры палитры */
		LTDC_Layerx->CLUTWR = 
			((color << 24) & LTDC_LxCLUTWR_CLUTADD) |
			((r << 16) & LTDC_LxCLUTWR_RED) |
			((g << 8) & LTDC_LxCLUTWR_GREEN) |
			((b << 0) & LTDC_LxCLUTWR_BLUE);
	}

	LTDC_Layerx->CR |= LTDC_LxCR_CLUTEN;
}

#elif LCDMODE_MAIN_L8

static void
fillLUT_L8(
	LTDC_Layer_TypeDef* LTDC_Layerx,
	const COLOR24_T * xltrgb24
	)
{
	unsigned i;

	for (i = 0; i < 256; ++ i)
	{
		uint_fast32_t color = xltrgb24 [i];
		/* запись значений в регистры палитры */
		LTDC_Layerx->CLUTWR = 
			((i << LTDC_LxCLUTWR_CLUTADD_Pos) & LTDC_LxCLUTWR_CLUTADD) |
			((COLOR24_R(color) << LTDC_LxCLUTWR_RED_Pos) & LTDC_LxCLUTWR_RED) |
			((COLOR24_G(color) << LTDC_LxCLUTWR_GREEN_Pos) & LTDC_LxCLUTWR_GREEN) |
			((COLOR24_B(color) << LTDC_LxCLUTWR_BLUE_Pos) & LTDC_LxCLUTWR_BLUE) |
			0;
	}

	LTDC_Layerx->CR |= LTDC_LxCR_CLUTEN;
}
#else

#endif /* LCDMODE_MAIN_L8 */


/**
  * @brief  Initializes the LTDC Layer according to the specified parameters
  *         in the LTDC_LayerStruct.
  * @note   This function can be used only when the LTDC is disabled.
  * @param  LTDC_layerx: Select the layer to be configured, this parameter can be 
  *         one of the following values: LTDC_Layer1, LTDC_Layer2    
  * @param  LTDC_LayerStruct: pointer to a LTDC_LayerTypeDef structure that contains
  *         the configuration information for the specified LTDC peripheral.
  * @retval None
  */

static void 
LTDC_LayerInit(LTDC_Layer_TypeDef* LTDC_Layerx, const LTDC_Layer_InitTypeDef* LTDC_Layer_InitStruct)
{

	uint32_t whsppos = 0;
	uint32_t wvsppos = 0;
	uint32_t cfbp = 0;

	/* Configures the horizontal start and stop position */
	whsppos = LTDC_Layer_InitStruct->LTDC_HorizontalStop << LTDC_LxWHPCR_WHSPPOS_Pos;
	LTDC_Layerx->WHPCR &= ~(LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS);
	LTDC_Layerx->WHPCR |= (LTDC_Layer_InitStruct->LTDC_HorizontalStart | whsppos);

	/* Configures the vertical start and stop position */
	wvsppos = LTDC_Layer_InitStruct->LTDC_VerticalStop << LTDC_LxWVPCR_WVSPPOS_Pos;
	LTDC_Layerx->WVPCR &= ~(LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS);
	LTDC_Layerx->WVPCR |= (LTDC_Layer_InitStruct->LTDC_VerticalStart | wvsppos);

	/* Specifies the pixel format */
	LTDC_Layerx->PFCR &= ~(LTDC_LxPFCR_PF);
	LTDC_Layerx->PFCR |= (LTDC_Layer_InitStruct->LTDC_PixelFormat);

	/* Configures the default color values */
	LTDC_Layerx->DCCR = LTDC_Layer_InitStruct->LTDC_DefaultColor;

	/* Specifies the constant alpha value */      
	// alpha канал если в видеобуфере не хранится значение в каждом пикселе
	LTDC_Layerx->CACR = (LTDC_Layerx->CACR & ~ (LTDC_LxCACR_CONSTA)) |
		(LTDC_Layer_InitStruct->LTDC_ConstantAlpha << LTDC_LxCACR_CONSTA_Pos) |
		0;

	/* Specifies the blending factors */
	LTDC_Layerx->BFCR &= ~(LTDC_LxBFCR_BF2 | LTDC_LxBFCR_BF1);
	LTDC_Layerx->BFCR |= (LTDC_Layer_InitStruct->LTDC_BlendingFactor_1 | LTDC_Layer_InitStruct->LTDC_BlendingFactor_2);

	/* Configures the color frame buffer start address */
//	LTDC_Layerx->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
//	LTDC_Layerx->CFBAR |= (LTDC_Layer_InitStruct->LTDC_CFBStartAdress);

	/* Configures the color frame buffer pitch in byte */
	cfbp = (LTDC_Layer_InitStruct->LTDC_CFBPitch << LTDC_LxCFBLR_CFBP_Pos);
	LTDC_Layerx->CFBLR  &= ~(LTDC_LxCFBLR_CFBLL | LTDC_LxCFBLR_CFBP);
	LTDC_Layerx->CFBLR  |= (LTDC_Layer_InitStruct->LTDC_CFBLineLength | cfbp);

	/* Configures the frame buffer line number */
	LTDC_Layerx->CFBLNR  &= ~(LTDC_LxCFBLNR_CFBLNBR);
	LTDC_Layerx->CFBLNR  |= (LTDC_Layer_InitStruct->LTDC_CFBLineNumber);

}
/**
  * @brief  Initializes the LTDC peripheral according to the specified parameters
  *         in the LTDC_InitStruct.
  * @note   This function can be used only when the LTDC is disabled.
  * @param  LTDC_InitStruct: pointer to a LTDC_InitTypeDef structure that contains
  *         the configuration information for the specified LTDC peripheral.
  * @retval None
  */

static void LTDC_Init(LTDC_InitTypeDef* LTDC_InitStruct)
{
	uint32_t horizontalsync = 0;
	uint32_t accumulatedHBP = 0;
	uint32_t accumulatedactiveW = 0;
	uint32_t totalwidth = 0;

	/* Sets Synchronization size */
	LTDC->SSCR &= ~(LTDC_SSCR_VSH | LTDC_SSCR_HSW);
	horizontalsync = (LTDC_InitStruct->LTDC_HorizontalSync << LTDC_SSCR_HSW_Pos);
	LTDC->SSCR |= (horizontalsync | LTDC_InitStruct->LTDC_VerticalSync);

	/* Sets Accumulated Back porch */
	LTDC->BPCR &= ~(LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);
	accumulatedHBP = (LTDC_InitStruct->LTDC_AccumulatedHBP << LTDC_BPCR_AHBP_Pos);
	LTDC->BPCR |= (accumulatedHBP | LTDC_InitStruct->LTDC_AccumulatedVBP);

	/* Sets Accumulated Active Width */
	LTDC->AWCR &= ~(LTDC_AWCR_AAH | LTDC_AWCR_AAW);
	accumulatedactiveW = (LTDC_InitStruct->LTDC_AccumulatedActiveW << LTDC_AWCR_AAW_Pos);
	LTDC->AWCR |= (accumulatedactiveW | LTDC_InitStruct->LTDC_AccumulatedActiveH);

	/* Sets Total Width */
	LTDC->TWCR &= ~ (LTDC_TWCR_TOTALH | LTDC_TWCR_TOTALW);
	totalwidth = (LTDC_InitStruct->LTDC_TotalWidth << LTDC_TWCR_TOTALW_Pos);
	LTDC->TWCR |= (totalwidth | LTDC_InitStruct->LTDC_TotalHeigh);

	LTDC->GCR = 0;
	LTDC->GCR |=  (uint32_t)(LTDC_InitStruct->LTDC_HSPolarity | LTDC_InitStruct->LTDC_VSPolarity |
			   LTDC_InitStruct->LTDC_DEPolarity | LTDC_InitStruct->LTDC_PCPolarity);

	/* sets the background color value */
	LTDC->BCCR = (LTDC->BCCR & ~ (LTDC_BCCR_BCBLUE_Msk | LTDC_BCCR_BCGREEN_Msk | LTDC_BCCR_BCRED_Msk)) |
		LTDC_InitStruct->LTDC_BackgroundColor |
		0;
}

static void LCD_LayerInit(
	LTDC_Layer_TypeDef* LTDC_Layerx, 
	unsigned hs,	// same as AccumulatedHBP + 1
	unsigned vs,		// same as LTDC_AccumulatedVBP + 1
	const pipparams_t * wnd,
	uint32_t LTDC_PixelFormat,
	unsigned scale_h,
	unsigned pixelsize	// для расчета размера строки в байтах
	)
{
	const unsigned long ROWSIZE = pixelsize * wnd->w;	// размер одной строки в байтах
	const unsigned long ROWMEMINC = pixelsize * GXADJ(wnd->w);	// размер одной строки в байтах

	LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct; 
	/* Windowing configuration */
	/* In this case all the active display area is used to display a picture then :
	Horizontal start = horizontal synchronization + Horizontal back porch = 30 
	Horizontal stop = Horizontal start + window width -1 = 30 + 240 -1
	Vertical start   = vertical synchronization + vertical back porch     = 4
	Vertical stop   = Vertical start + window height -1  = 4 + 320 -1      */      
	LTDC_Layer_InitStruct.LTDC_HorizontalStart = hs + wnd->x * scale_h;
	LTDC_Layer_InitStruct.LTDC_HorizontalStop = hs + wnd->x * scale_h + wnd->w * scale_h - 1; 
	LTDC_Layer_InitStruct.LTDC_VerticalStart = vs + wnd->y;
	LTDC_Layer_InitStruct.LTDC_VerticalStop = vs + wnd->y + wnd->h - 1;

	/* Pixel Format configuration*/
	LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_PixelFormat;
	/* Alpha constant (255 totally opaque = непрозрачный) */
	LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255; 
	/* Default Color configuration (configure A,R,G,B component values) */          
	LTDC_Layer_InitStruct.LTDC_DefaultColor = 0; // transparent=прозрачный black color. outside active layer area
	/* Configure blending factors */       
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA; // умножитель для пикселя из текущего слоя
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA; // умножитель для пикселя из расположенного ниже слоя

	/* the length of one line of pixels in bytes + 3 then :
	Line Lenth = Active high width x number of bytes per pixel + 3 
	Active high width         = DIM_SECOND 
	number of bytes per pixel = 2    (pixel_format : RGB565) 
	number of bytes per pixel = 1    (pixel_format : L8) 
	*/
#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	LTDC_Layer_InitStruct.LTDC_CFBLineLength = ROWSIZE + 7;
#else /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */
	LTDC_Layer_InitStruct.LTDC_CFBLineLength = ROWSIZE + 3;
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */

	/* the pitch is the increment from the start of one line of pixels to the 
	start of the next line in bytes, then :
	Pitch = Active high width x number of bytes per pixel */ 
	LTDC_Layer_InitStruct.LTDC_CFBPitch = ROWMEMINC;

	/* Configure the number of lines */  
	LTDC_Layer_InitStruct.LTDC_CFBLineNumber = wnd->h;

	/* Start Address configuration : the LCD Frame buffer is defined on SDRAM */    
	//LTDC_Layer_InitStruct.LTDC_CFBStartAdress = wnd->frame;
	//LTDC_Layer1->CFBAR = (uint32_t) & framebuff;

	/* Initialize LTDC layer 1 */
	LTDC_LayerInit(LTDC_Layerx, & LTDC_Layer_InitStruct);

	/* Enable foreground & background Layers */
	//LTDC_LayerCmd(LTDC_Layer1, ENABLE); 
	//LTDC_LayerCmd(LTDC_Layer2, ENABLE);
	/* Enable LTDC_Layer by setting LEN bit */
	////LTDC_Layerx->CR |= LTDC_LxCR_LEN;

	/* LTDC configuration reload */  
}


//#define WITHUSELTDCTRANSPARENCY	1

#if WITHUSELTDCTRANSPARENCY

// работа с прозрачностью над зоной PIP
#define LAYER_PIP	LTDC_Layer1		// PIP layer
#define LAYER_MAIN	LTDC_Layer2

#else /* WITHUSELTDCTRANSPARENCY */

// PIP перекрывает слой под ним
#define LAYER_PIP	LTDC_Layer2		// PIP layer = RGB565 format
#define LAYER_MAIN	LTDC_Layer1		// L8 or RGB565 format

#endif /* WITHUSELTDCTRANSPARENCY */

/* Изменение настроек для работы слоя как "верхнего" при формированиии наложения */
static void LCD_LayerInitMain(
	LTDC_Layer_TypeDef* LTDC_Layerx
	)
{
	// преобразование из упакованного пикселя RGB565 по правилам pfc LTDC
	// в требующийся RGB888
	const unsigned long key = COLOR24_KEY;
	const unsigned long keyr = (key >> 11) & 0x1F;
	const unsigned long keyg = (key >> 6) & 0x3F;
	const unsigned long keyb = (key >> 0) & 0x1F;
	const unsigned long keyrpfc = ((keyr << 3) | (keyr >> 3)) & 0xFF;
	const unsigned long keygpfc = ((keyg << 2) | (keyg >> 4)) & 0xFF;
	const unsigned long keybpfc = ((keyb << 3) | (keyb >> 3)) & 0xFF;

	LTDC_Layerx->CKCR = 
		(keyrpfc << LTDC_LxCKCR_CKRED_Pos) |
		(keygpfc << LTDC_LxCKCR_CKGREEN_Pos) |
		(keybpfc << LTDC_LxCKCR_CKBLUE_Pos) |
		0;

#if WITHUSELTDCTRANSPARENCY
	LTDC_Layerx->CR |= LTDC_LxCR_COLKEN;	
#endif /* WITHUSELTDCTRANSPARENCY */

#if 1
	// alpha канал если в видеобуфере не хранится значение в каждом пикселе
	LTDC_Layerx->CACR = (LTDC_Layerx->CACR & ~ (LTDC_LxCACR_CONSTA)) |
		(255 << LTDC_LxCACR_CONSTA_Pos) |	/* Alpha constant (255 totally opaque=непрозрачный) */
		0;
#endif
}

/* Изменение настроек для работы слоя как "нижнего" при формированиии наложения */
static void LCD_LayerInitPIP(
	LTDC_Layer_TypeDef* LTDC_Layerx
	)
{
#if 1
	// alpha канал если в видеобуфере не хранится значение в каждом пикселе
	LTDC_Layerx->CACR = (LTDC_Layerx->CACR & ~ (LTDC_LxCACR_CONSTA)) |
		(255 << LTDC_LxCACR_CONSTA_Pos) |	/* Alpha constant (255 totally opaque=непрозрачный) */
		0;
#endif
}

void
arm_hardware_ltdc_initialize(void)
{
	PRINTF(PSTR("arm_hardware_ltdc_initialize start, WIDTH=%d, HEIGHT=%d\n"), WIDTH, HEIGHT);

	//const unsigned ROWSIZE = sizeof framebuff [0];	// размер одной строки в байтах
	//const unsigned rowsize2 = (sizeof (PACKEDCOLOR_T) * DIM_SECOND);
	//ASSERT(ROWSIZE == rowsize2);
	//PRINTF(PSTR("arm_hardware_ltdc_initialize: framebuff=%p\n"), framebuff);

	/* Initialize the LCD */

	hardware_set_dotclock(display_getdotclock());

#if CPUSTYLE_STM32H7XX
	/* Enable the LTDC Clock */
	RCC->APB3ENR |= RCC_APB3ENR_LTDCEN;	/* LTDC clock enable */
	(void) RCC->APB3ENR;

#elif CPUSTYLE_STM32MP1

	{
		/* SYSCFG clock enable */
		RCC->MP_APB3ENSETR = RCC_MC_APB3ENSETR_SYSCFGEN;
		(void) RCC->MP_APB3ENSETR;
		RCC->MP_APB3LPENSETR = RCC_MC_APB3LPENSETR_SYSCFGLPEN;
		(void) RCC->MP_APB3LPENSETR;
		/*
		 * Interconnect update : select master using the port 1.
		 * LTDC = AXI_M9.
		 * MDMA = AXI_M7.
		 */
		SYSCFG->ICNR |= SYSCFG_ICNR_AXI_M9;
		(void) SYSCFG->ICNR;
	}

	/* Enable the LTDC Clock */
	RCC->MP_APB4ENSETR = RCC_MP_APB4ENSETR_LTDCEN;	/* LTDC clock enable */
	(void) RCC->MP_APB4ENSETR;
	/* Enable the LTDC Clock in low-power mode */
	RCC->MP_APB4LPENSETR = RCC_MP_APB4LPENSETR_LTDCLPEN;	/* LTDC clock enable */
	(void) RCC->MP_APB4LPENSETR;

#else /* CPUSTYLE_STM32H7XX */
	/* Enable the LTDC Clock */
	RCC->APB2ENR |= RCC_APB2ENR_LTDCEN;	/* LTDC clock enable */
	(void) RCC->APB2ENR;
#endif /* CPUSTYLE_STM32H7XX */

	// Таблица используемой при отображении палитры
	COLOR24_T xltrgb24 [256];
	display2_xltrgb24(xltrgb24);

	/* Configure the LCD Control pins */
	HARDWARE_LTDC_INITIALIZE(BOARD_DEMODE);	// подключение к выводам процессора сигналов периферийного контроллера

	/* LTDC Initialization -------------------------------------------------------*/
	LTDC_InitTypeDef LTDC_InitStruct;

	pipparams_t mainwnd = { 0, 0, DIM_SECOND, DIM_FIRST };

#if LCDMODE_PIP_RGB565 || LCDMODE_PIP_L8
	pipparams_t pipwnd;
	display2_getpipparams(& pipwnd);

	PRINTF(PSTR("arm_hardware_ltdc_initialize: pip: x/y=%u/%u, w/h=%u/%u\n"), pipwnd.x, pipwnd.y, pipwnd.w, pipwnd.h);
#endif /* LCDMODE_PIP_RGB565 || LCDMODE_PIP_L8 */

	LTDC_InitStruct.LTDC_HSPolarity = HSYNCNEG ? LTDC_HSPolarity_AL : LTDC_HSPolarity_AH;     
	//LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AH;     
	/* Initialize the vertical synchronization polarity as active low */  
	LTDC_InitStruct.LTDC_VSPolarity = VSYNCNEG ? LTDC_VSPolarity_AL : LTDC_VSPolarity_AH;     
	//LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AH;     
	/* Initialize the data enable polarity as active low */ 
	//LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AH;		// While VSYNC is low, do not change DISP signal "Low" or "High"
	// LTDC datasheet say: "DE: Not data enable"
	LTDC_InitStruct.LTDC_DEPolarity = DENEG ? LTDC_DEPolarity_AH : LTDC_DEPolarity_AL;		// While VSYNC is low, do not change DISP signal "Low" or "High"
	/* Initialize the pixel clock polarity as input pixel clock */ 
	LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;

	/* Timing configuration */
	/* Configure horizontal synchronization width */     
	LTDC_InitStruct.LTDC_HorizontalSync = (HSYNC - 1);
	/* Configure accumulated horizontal back porch */
	LTDC_InitStruct.LTDC_AccumulatedHBP = (LEFTMARGIN - 1);
	/* Configure accumulated active width */  
	LTDC_InitStruct.LTDC_AccumulatedActiveW = (LEFTMARGIN + WIDTH - 1);
	/* Configure total width */
	LTDC_InitStruct.LTDC_TotalWidth = (HFULL - 1);

	/* Configure vertical synchronization height */
	LTDC_InitStruct.LTDC_VerticalSync = (VSYNC - 1);
	/* Configure accumulated vertical back porch */
	LTDC_InitStruct.LTDC_AccumulatedVBP = (TOPMARGIN - 1);
	/* Configure accumulated active height */
	LTDC_InitStruct.LTDC_AccumulatedActiveH = (TOPMARGIN + HEIGHT - 1);
	/* Configure total height */
	LTDC_InitStruct.LTDC_TotalHeigh = (VFULL - 1);

	/* Configure R,G,B component values for LCD background color */                   
	LTDC_InitStruct.LTDC_BackgroundColor = 0;		// all 0 - black

	LTDC_Init(& LTDC_InitStruct);

	LTDC_Init(& LTDC_InitStruct);


	/* LTDC initialization end ---------------------------------------------------*/

	// Top layer - LTDC_Layer2
	// Bottom layer - LTDC_Layer1
#if LCDMODE_MAIN_L24

	LCD_LayerInit(LAYER_MAIN, LEFTMARGIN, TOPMARGIN, & mainwnd, LTDC_Pixelformat_L8, 3, sizeof (PACKEDCOLORMAIN_T));

#elif LCDMODE_MAIN_L8

	LCD_LayerInit(LAYER_MAIN, LEFTMARGIN, TOPMARGIN, & mainwnd, LTDC_Pixelformat_L8, 1, sizeof (PACKEDCOLORMAIN_T));

#else
	/* Без палитры */
	LCD_LayerInit(LAYER_MAIN, LEFTMARGIN, TOPMARGIN, & mainwnd, LTDC_Pixelformat_RGB565, 1, sizeof (PACKEDCOLORMAIN_T));

#endif /* LCDMODE_MAIN_L8 */

#if LCDMODE_PIP_L8

	LCD_LayerInitMain(LAYER_MAIN);	// довести инициализацию

	// Bottom layer
	LCD_LayerInit(LAYER_PIP, LEFTMARGIN, TOPMARGIN, & pipwnd, LTDC_Pixelformat_L8, 1, sizeof (PACKEDCOLORPIP_T));
	LCD_LayerInitPIP(LAYER_PIP);	// довести инициализацию

#elif LCDMODE_PIP_RGB565

	LCD_LayerInitMain(LAYER_MAIN);	// довести инициализацию

	// Bottom layer
	LCD_LayerInit(LAYER_PIP, LEFTMARGIN, TOPMARGIN, & pipwnd, LTDC_Pixelformat_RGB565, 1, sizeof (PACKEDCOLORPIP_T));
	LCD_LayerInitPIP(LAYER_PIP);	// довести инициализацию

#endif /* LCDMODE_PIP_RGB565 */

	LTDC->SRCR = LTDC_SRCR_IMR;	/*!< Immediately Reload. */

	/* Enable the LTDC */
	LTDC->GCR |= LTDC_GCR_LTDCEN;

#if LCDMODE_MAIN_L24
	fillLUT_L24(LAYER_MAIN);	// прямая трансляция всех ьайтов из памяти на выход. загрузка палитры - имеет смысл до Reload
#elif LCDMODE_MAIN_L8
	fillLUT_L8(LAYER_MAIN, xltrgb24);	// загрузка палитры - имеет смысл до Reload
#endif /* LCDMODE_MAIN_L8 */
#if LCDMODE_PIP_L8
	fillLUT_L8(LAYER_PIP, xltrgb24);	// загрузка палитры - имеет смысл до Reload
#endif /* LCDMODE_PIP_L8 */

	/* LTDC reload configuration */  
	LTDC->SRCR = LTDC_SRCR_IMR;	/* Immediately Reload. */

	// LQ043T3DX02K rules: While “VSYNC” is “Low”, don’t change “DISP” signal “Low” to “High”.
#if defined (BOARD_DEVALUE)
	HARDWARE_LTDC_SET_DISP(BOARD_DEMODE, BOARD_DEVALUE);
#elif defined (BOARD_DERESET)
	/* SONY PSP-1000 display (4.3") required. */
	HARDWARE_LTDC_SET_DISP(BOARD_DEMODE, 0);
	local_delay_ms(150);
	HARDWARE_LTDC_SET_DISP(BOARD_DEMODE, 1);
#endif
#if defined (BOARD_MODEVALUE)
	HARDWARE_LTDC_SET_MODE(BOARD_MODEVALUE);
#endif
	PRINTF(PSTR("arm_hardware_ltdc_initialize done\n"));
}

/* set bottom buffer start */
/* Set PIP frame buffer address. */
void arm_hardware_ltdc_pip_set(uintptr_t p)
{
#if 0
	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & LTDC_SRCR_VBR) != 0)
		hardware_nonguiyield();
#endif
	LAYER_PIP->CFBAR = p;
	(void) LAYER_PIP->CFBAR;
	LAYER_PIP->CR |= LTDC_LxCR_LEN;
	(void) LAYER_PIP->CR;
	LTDC->SRCR = LTDC_SRCR_VBR;	/* Vertical Blanking Reload. */
}

/* Turn PIP off (main layer only). */
void arm_hardware_ltdc_pip_off(void)
{
	LAYER_PIP->CR &= ~ LTDC_LxCR_LEN;
	(void) LAYER_PIP->CR;
	LTDC->SRCR = LTDC_SRCR_VBR;	/* Vertical Blanking Reload. */
}

/* Palette reload */
void arm_hardware_ltdc_L8_palette(void)
{
	// Таблица используемой при отображении палитры
	COLOR24_T xltrgb24 [256];
	display2_xltrgb24(xltrgb24);
#if LCDMODE_MAIN_L8
	fillLUT_L8(LAYER_MAIN, xltrgb24);	// загрузка палитры - имеет смысл до Reload
	/* LTDC reload configuration */
	LTDC->SRCR = LTDC_SRCR_IMR;	/* Immediately Reload. */
#endif /* LCDMODE_PIP_L8 */
#if LCDMODE_PIP_L8
	/* LTDC reload configuration */
	LTDC->SRCR = LTDC_SRCR_IMR;	/* Immediately Reload. */
	fillLUT_L8(LAYER_PIP, xltrgb24);	// загрузка палитры - имеет смысл до Reload
#endif /* LCDMODE_PIP_L8 */
}
//LCDMODE_MAIN_L8

/* Set MAIN frame buffer address. */
void arm_hardware_ltdc_main_set(uintptr_t p)
{
#if 0
	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & LTDC_SRCR_VBR) != 0)
		hardware_nonguiyield();
#endif
	LAYER_MAIN->CFBAR = p;
	(void) LAYER_MAIN->CFBAR;
	LAYER_MAIN->CR |= LTDC_LxCR_LEN;
	(void) LAYER_MAIN->CR;
	LTDC->SRCR = LTDC_SRCR_VBR_Msk;	/* Vertical Blanking Reload. */
}

#endif /* CPUSTYLE_STM32F || CPUSTYLE_STM32MP1 */

uint_fast32_t display_getdotclock(void)
{
	return LTDC_DOTCLK;
}

#else /* WITHLTDCHW */

uint_fast32_t display_getdotclock(void)
{
	return 1000000uL;
}

#endif /* WITHLTDCHW */

#if LCDMODETX_TC358778XBG

// See:
//	http://www.staroceans.org/projects/beagleboard/drivers/gpu/drm/omapdrm/displays/encoder-tc358768.c

/* Global (16-bit addressable) */
#define TC358768_CHIPID			0x0000
#define TC358768_SYSCTL			0x0002
#define TC358768_CONFCTL		0x0004	// Input Control Register
#define TC358768_VSDLY			0x0006
#define TC358768_DATAFMT		0x0008
#define TC358768_GPIOEN			0x000E
#define TC358768_GPIODIR		0x0010
#define TC358768_GPIOIN			0x0012
#define TC358768_GPIOOUT		0x0014
#define TC358768_PLLCTL0		0x0016
#define TC358768_PLLCTL1		0x0018
#define TC358768_CMDBYTE		0x0022
#define TC358768_PP_MISC		0x0032
#define TC358768_DSITX_DT		0x0050
#define TC358768_FIFOSTATUS		0x00F8

/* Debug (16-bit addressable) */
#define TC358768_VBUFCTRL		0x00E0
#define TC358768_DBG_WIDTH		0x00E2
#define TC358768_DBG_VBLANK		0x00E4
#define TC358768_DBG_DATA		0x00E8

/* TX PHY (32-bit addressable) */
#define TC358768_CLW_DPHYCONTTX		0x0100
#define TC358768_D0W_DPHYCONTTX		0x0104
#define TC358768_D1W_DPHYCONTTX		0x0108
#define TC358768_D2W_DPHYCONTTX		0x010C
#define TC358768_D3W_DPHYCONTTX		0x0110
#define TC358768_CLW_CNTRL		0x0140
#define TC358768_D0W_CNTRL		0x0144
#define TC358768_D1W_CNTRL		0x0148
#define TC358768_D2W_CNTRL		0x014C
#define TC358768_D3W_CNTRL		0x0150

/* TX PPI (32-bit addressable) */
#define TC358768_STARTCNTRL		0x0204
#define TC358768_DSITXSTATUS		0x0208
#define TC358768_LINEINITCNT		0x0210
#define TC358768_LPTXTIMECNT		0x0214
#define TC358768_TCLK_HEADERCNT		0x0218
#define TC358768_TCLK_TRAILCNT		0x021C
#define TC358768_THS_HEADERCNT		0x0220
#define TC358768_TWAKEUP		0x0224
#define TC358768_TCLK_POSTCNT		0x0228
#define TC358768_THS_TRAILCNT		0x022C
#define TC358768_HSTXVREGCNT		0x0230
#define TC358768_HSTXVREGEN		0x0234
#define TC358768_TXOPTIONCNTRL		0x0238
#define TC358768_BTACNTRL1		0x023C

/* TX CTRL (32-bit addressable) */
#define TC358768_DSI_STATUS		0x0410
#define TC358768_DSI_INT		0x0414
#define TC358768_DSICMD_RXFIFO		0x0430
#define TC358768_DSI_ACKERR		0x0434
#define TC358768_DSI_RXERR		0x0440
#define TC358768_DSI_ERR		0x044C
#define TC358768_DSI_CONFW		0x0500
#define TC358768_DSI_RESET		0x0504
#define TC358768_DSI_INT_CLR		0x050C
#define TC358768_DSI_START		0x0518

/* DSITX CTRL (16-bit addressable) */
#define TC358768_DSICMD_TX		0x0600
#define TC358768_DSICMD_TYPE		0x0602
#define TC358768_DSICMD_WC		0x0604
#define TC358768_DSICMD_WD0		0x0610
#define TC358768_DSICMD_WD1		0x0612
#define TC358768_DSICMD_WD2		0x0614
#define TC358768_DSICMD_WD3		0x0616
#define TC358768_DSI_EVENT		0x0620
#define TC358768_DSI_VSW		0x0622
#define TC358768_DSI_VBPR		0x0624
#define TC358768_DSI_VACT		0x0626
#define TC358768_DSI_HSW		0x0628
#define TC358768_DSI_HBPR		0x062A
#define TC358768_DSI_HACT		0x062C


#define TC358768_I2C_ADDR (0x0E * 2)

unsigned long
tc358768_rd_reg_16bits(unsigned register_id)
{
	const unsigned i2caddr = TC358768_I2C_ADDR;

	uint8_t v1, v2, v3, v4;

	i2c_start(i2caddr | 0x00);
	i2c_write(register_id >> 8);
	i2c_write_withrestart(register_id >> 0);
	i2c_start(i2caddr | 0x01);
	i2c_read(& v1, I2C_READ_ACK_1);	// ||
	i2c_read(& v2, I2C_READ_ACK);	// ||
	i2c_read(& v3, I2C_READ_ACK);	// ||
	i2c_read(& v4, I2C_READ_NACK);	// ||

	return
			(((unsigned long) v1) << 8) |
			(((unsigned long) v2) << 0) |
			0;
}

void
tc358768_wr_reg_32bits(unsigned long value)
{
	const unsigned i2caddr = TC358768_I2C_ADDR;

	i2c_start(i2caddr | 0x00);
	i2c_write(value >> 24);		// addres hi
	i2c_write(value >> 16);		// addres lo
	i2c_write(value >> 8);		// data hi
	i2c_write(value >> 0);		// data lo
	i2c_waitsend();
    i2c_stop();
}


struct tc358768_drv_data
{
	int dev;
	unsigned fbd, prd, frs;
	unsigned bitclk;
};

struct tc358768_drv_data dev0;

static int tc358768_write(
	struct tc358768_drv_data *ddata,
	unsigned int reg,
	unsigned int val
	)
{
	const unsigned i2caddr = TC358768_I2C_ADDR;

	if (reg < 0x100 || reg >= 0x600)
	{
		// 16-bit register
		i2c_start(i2caddr | 0x00);
		i2c_write(reg >> 8);		// addres hi
		i2c_write(reg >> 0);		// addres lo
		i2c_write(val >> 8);		// data hi
		i2c_write(val >> 0);		// data lo
		i2c_waitsend();
	    i2c_stop();
	}
	else
	{
		// 32-bit register
		i2c_start(i2caddr | 0x00);
		i2c_write(reg >> 8);		// addres hi
		i2c_write(reg >> 0);		// addres lo
		i2c_write(val >> 24);		// data hi
		i2c_write(val >> 16);		// data lo
		i2c_waitsend();
	    i2c_stop();
	}
	return 0;
}

static int tc358768_read(
	struct tc358768_drv_data *ddata,
	unsigned int reg,
	unsigned int * val
	)
{
	const unsigned i2caddr = TC358768_I2C_ADDR;

	if (reg < 0x100 || reg >= 0x600)
	{
		// 16-bit register

		uint8_t v1, v2;

		i2c_start(i2caddr | 0x00);
		i2c_write(reg >> 8);
		i2c_write_withrestart(reg >> 0);
		i2c_start(i2caddr | 0x01);
		i2c_read(& v1, I2C_READ_ACK_1);	// ||
		i2c_read(& v2, I2C_READ_NACK);	// ||

		* val =
				(((unsigned long) v1) << 8) |
				(((unsigned long) v2) << 0) |
				0;
	}
	else
	{
		// 32-bit register

		uint8_t v1, v2, v3, v4;

		i2c_start(i2caddr | 0x00);
		i2c_write(reg >> 8);
		i2c_write_withrestart(reg >> 0);
		i2c_start(i2caddr | 0x01);
		i2c_read(& v1, I2C_READ_ACK_1);	// ||
		i2c_read(& v2, I2C_READ_ACK);	// ||
		i2c_read(& v3, I2C_READ_ACK);	// ||
		i2c_read(& v4, I2C_READ_NACK);	// ||

		* val =
				(((unsigned long) v1) << 8) |
				(((unsigned long) v2) << 0) |
				(((unsigned long) v3) << 24) |
				(((unsigned long) v4) << 16) |
				0;
	}

	return 0;
}

static int tc358768_update_bits(struct tc358768_drv_data *ddata,
	unsigned int reg, unsigned int mask, unsigned int val)
{
	int ret;
	unsigned int tmp, orig;

	ret = tc358768_read(ddata, reg, &orig);
	if (ret != 0)
		return ret;

	tmp = orig & ~mask;
	tmp |= val & mask;

	//dev_dbg(ddata->dev, "UPD \t%04x\t%08x -> %08x\n", reg, orig, tmp);

	if (tmp != orig)
		ret = tc358768_write(ddata, reg, tmp);

	return ret;
}


static int tc358768_dsi_xfer_short(struct tc358768_drv_data *ddata,
	uint8_t data_id, uint8_t data0, uint8_t data1)
{
	const uint8_t packet_type = 0x10; /* DSI Short Packet */
	const uint8_t word_count = 0;

	tc358768_write(ddata, TC358768_DSICMD_TYPE,
		(packet_type << 8) | data_id);
	tc358768_write(ddata, TC358768_DSICMD_WC, (word_count & 0xf));
	tc358768_write(ddata, TC358768_DSICMD_WD0, (data1 << 8) | data0);
	tc358768_write(ddata, TC358768_DSICMD_TX, 1); /* start transfer */

	return 0;
}

static void tc358768_sw_reset(struct tc358768_drv_data *ddata)
{
	/* Assert Reset */
	tc358768_write(ddata, TC358768_SYSCTL, 1);
	/* Release Reset, Exit Sleep */
	tc358768_write(ddata, TC358768_SYSCTL, 0);
}

#define REFCLK 25000000uL
#define DSI_NDL 4
#define DPI_NDL 24

static uint32_t local_ulmin(uint32_t a, uint32_t b) { return a < b ? a : b; }
static uint32_t local_ulmax(uint32_t a, uint32_t b) { return a > b ? a : b; }
static uint64_t div_u64(uint64_t a, uint64_t b) { return a / b; }

static uint32_t tc358768_pll_to_pclk(struct tc358768_drv_data *ddata, uint32_t pll)
{
	uint32_t byteclk;

	byteclk = pll / 2 / 4;

	return (uint32_t)div_u64((uint64_t)byteclk * 8 *DSI_NDL,
		DPI_NDL);
}

static uint32_t tc358768_pclk_to_pll(struct tc358768_drv_data *ddata, uint32_t pclk)
{
	uint32_t byteclk;

	byteclk = (uint32_t)div_u64((uint64_t)pclk * DPI_NDL,
		8 *DSI_NDL);

	return byteclk * 4 * 2;
}

static int tc358768_calc_pll(struct tc358768_drv_data *ddata)
{
	static const unsigned frs_limits[] = {
		1000000000, 500000000, 250000000, 125000000, 62500000
	};
	unsigned fbd, prd, frs;
	uint32_t target_pll;
	unsigned long refclk;
	unsigned i;
	uint32_t max_pll, min_pll;

	uint32_t best_diff, best_pll, best_prd, best_fbd;

	target_pll = tc358768_pclk_to_pll(ddata, LTDC_DOTCLK);

	/* pll_clk = RefClk * [(FBD + 1)/ (PRD + 1)] * [1 / (2^FRS)] */

	frs = UINT_MAX;

	for (i = 0; i < ARRAY_SIZE(frs_limits) - 1; ++i) {
		if (target_pll < frs_limits[i] && target_pll >= frs_limits[i + 1]) {
			frs = i;
			max_pll = frs_limits[i];
			min_pll = frs_limits[i + 1];
			break;
		}
	}

	if (frs == UINT_MAX)
		return -1;

	//refclk = clk_get_rate(REFCLK);
	refclk = (REFCLK);

	best_pll = best_prd = best_fbd = 0;
	best_diff = UINT_MAX;

	for (prd = 0; prd < 16; ++prd) {
		uint32_t divisor = (prd + 1) * (1 << frs);

		for (fbd = 0; fbd < 512; ++fbd) {
			uint32_t pll, diff;

			pll = (uint32_t)div_u64((uint64_t)refclk * (fbd + 1), divisor);

			if (pll >= max_pll || pll < min_pll)
				continue;

			diff = local_ulmax(pll, target_pll) - local_ulmin(pll, target_pll);

			if (diff < best_diff) {
				best_diff = diff;
				best_pll = pll;
				best_prd = prd;
				best_fbd = fbd;
			}

			if (best_diff == 0)
				break;
		}

		if (best_diff == 0)
			break;
	}

	if (best_diff == UINT_MAX) {
		//dev_err(ddata->dev, "could not find suitable PLL setup\n");
		return -1;
	}

	ddata->fbd = best_fbd;
	ddata->prd = best_prd;
	ddata->frs = frs;
	ddata->bitclk = best_pll / 2;

	return 0;
}

static void tc358768_setup_pll(struct tc358768_drv_data *ddata)
{
	unsigned fbd, prd, frs;

	fbd = ddata->fbd;	// Feedback divider setting
	prd = ddata->prd;	// Input divider setting
	frs = ddata->frs;	// Frequency range setting (post divider)

	PRINTF("PLL: refclk %lu, fbd %u, prd %u, frs %u\n",
		(REFCLK), fbd, prd, frs);

	PRINTF("PLL: %u, BitClk %u, ByteClk %u, pclk %u\n",
		ddata->bitclk * 2, ddata->bitclk, ddata->bitclk / 4,
		tc358768_pll_to_pclk(ddata, ddata->bitclk * 2));

	/* PRD[15:12] FBD[8:0] */
	tc358768_write(ddata, TC358768_PLLCTL0, (prd << 12) | fbd);

	/* FRS[11:10] LBWS[9:8] CKEN[4] RESETB[1] EN[0] */
	tc358768_write(ddata, TC358768_PLLCTL1,
		(frs << 10) | (0x2 << 8) | (0 << 4) | (1 << 1) | (1 << 0));

	/* wait for lock */
	local_delay_ms(5);

	/* FRS[11:10] LBWS[9:8] CKEN[4] RESETB[1] EN[0] */
	tc358768_write(ddata, TC358768_PLLCTL1,
		(frs << 10) | (0x2 << 8) | (1 << 4) | (1 << 1) | (1 << 0));
}

static void tc358768_power_on(struct tc358768_drv_data *ddata)
{
	//const struct omap_video_timings *t = &ddata->videomode;

	tc358768_sw_reset(ddata);

	tc358768_setup_pll(ddata);

	/* VSDly[9:0] */
	tc358768_write(ddata, TC358768_VSDLY, 1);
	/* PDFormat[7:4] spmode_en[3] rdswap_en[2] dsitx_en[1] txdt_en[0] */
	tc358768_write(ddata, TC358768_DATAFMT, (0x3 << 4) | (1 << 2) | (1 << 1) | (1 << 0));
	/* dsitx_dt[7:0] 3e = Packed Pixel Stream, 24-bit RGB, 8-8-8 Format*/
	tc358768_write(ddata, TC358768_DSITX_DT, 0x003e);

	/* Enable D-PHY (HiZ->LP11) */
	tc358768_write(ddata, TC358768_CLW_CNTRL, 0x0000);
	tc358768_write(ddata, TC358768_D0W_CNTRL, 0x0000);
	tc358768_write(ddata, TC358768_D1W_CNTRL, 0x0000);
	tc358768_write(ddata, TC358768_D2W_CNTRL, 0x0000);
	tc358768_write(ddata, TC358768_D3W_CNTRL, 0x0000);

	/* DSI Timings */
	/* LP11 = 100 us for D-PHY Rx Init */
	tc358768_write(ddata, TC358768_LINEINITCNT,	0x00002c88);
	tc358768_write(ddata, TC358768_LPTXTIMECNT,	0x00000005);
	tc358768_write(ddata, TC358768_TCLK_HEADERCNT,	0x00001f06);
	tc358768_write(ddata, TC358768_TCLK_TRAILCNT,	0x00000003);
	tc358768_write(ddata, TC358768_THS_HEADERCNT,	0x00000606);
	tc358768_write(ddata, TC358768_TWAKEUP,		0x00004a88);
	tc358768_write(ddata, TC358768_TCLK_POSTCNT,	0x0000000b);
	tc358768_write(ddata, TC358768_THS_TRAILCNT,	0x00000004);
	tc358768_write(ddata, TC358768_HSTXVREGEN,	0x0000001f);

	/* CONTCLKMODE[0] */
	tc358768_write(ddata, TC358768_TXOPTIONCNTRL, 0x1);
	/* TXTAGOCNT[26:16] RXTASURECNT[10:0] */
	tc358768_write(ddata, TC358768_BTACNTRL1, (0x5 << 16) | (0x5));
	/* START[0] */
	tc358768_write(ddata, TC358768_STARTCNTRL, 0x1);

	/* DSI Tx Timing Control */

	/* Set event mode */
	tc358768_write(ddata, TC358768_DSI_EVENT, 1);

	/* vsw (+ vbp) */
	tc358768_write(ddata, TC358768_DSI_VSW, TOPMARGIN);
	/* vbp (not used in event mode) */
	tc358768_write(ddata, TC358768_DSI_VBPR, 0);
	/* vact */
	tc358768_write(ddata, TC358768_DSI_VACT, HEIGHT);

	/* (hsw + hbp) * byteclk * ndl / pclk */
	tc358768_write(ddata, TC358768_DSI_HSW,
		(uint32_t)div_u64(LEFTMARGIN * ((uint64_t)ddata->bitclk / 4) *DSI_NDL, LTDC_DOTCLK)
		);
	/* hbp (not used in event mode) */
	tc358768_write(ddata, TC358768_DSI_HBPR, 0);
	/* hact (bytes) */
	tc358768_write(ddata, TC358768_DSI_HACT, WIDTH * 3);

	/* Start DSI Tx */
	tc358768_write(ddata, TC358768_DSI_START, 0x1);

	/* SET, DSI_Control, 0xa7 */
	/* 0xa7 = HS | CONTCLK | 4-datalines | EoTDisable */
	tc358768_write(ddata, TC358768_DSI_CONFW, (5<<29) | (0x3 << 24) | 0xa7);
	/* CLEAR, DSI_Control, 0x8001 */
	/* 0x8001 = DSIMode */
	tc358768_write(ddata, TC358768_DSI_CONFW, (6<<29) | (0x3 << 24) | 0x8000);

	/* clear FrmStop and RstPtr */
	tc358768_update_bits(ddata, TC358768_PP_MISC, 0x3 << 14, 0);

	/* set PP_en */
	tc358768_update_bits(ddata, TC358768_CONFCTL, 1 << 6, 1 << 6);
}

static void tc358768_power_off(struct tc358768_drv_data *ddata)
{
	/* set FrmStop */
	tc358768_update_bits(ddata, TC358768_PP_MISC, 1 << 15, 1 << 15);

	/* wait at least for one frame */
	local_delay_ms(50);

	/* clear PP_en */
	tc358768_update_bits(ddata, TC358768_CONFCTL, 1 << 6, 0);

	/* set RstPtr */
	tc358768_update_bits(ddata, TC358768_PP_MISC, 1 << 14, 1 << 14);
}
void tc_print(uint32_t addr) {
	//PRINTF("+++++++++++addr->%04x: %04x\n", addr, tc358768_rd_reg_16bits(addr));
}

#define tc358768_wr_regs_32bits(reg_array)  _tc358768_wr_regs_32bits(reg_array, ARRAY_SIZE(reg_array))
int _tc358768_wr_regs_32bits(unsigned int reg_array[], uint32_t n) {

	int i = 0;
	//PRINTF("%s:%d\n", __func__, n);
	for(i = 0; i < n; i++) {
		if(reg_array[i] < 0x00020000) {
		    if(reg_array[i] < 20000)
		    	local_delay_us(reg_array[i]);
		    else {
		    	local_delay_ms(reg_array[i]/1000);
		    }
		} else {
			tc358768_wr_reg_32bits(reg_array[i]);
		}
	}
	return 0;
}

int tc358768_command_tx_less8bytes(unsigned char type, const unsigned char *regs, uint32_t n) {
	int i = 0;
	unsigned int command[] = {
			0x06020000,
			0x06040000,
			0x06100000,
			0x06120000,
			0x06140000,
			0x06160000,
	};

	if(n <= 2)
		command[0] |= 0x1000;   //short packet
	else {
		command[0] |= 0x4000;   //long packet
		command[1] |= n;		//word count byte
	}
	command[0] |= type;         //data type

	//PRINTF("*cmd:\n");
	//PRINTF("0x%08x\n", command[0]);
	//PRINTF("0x%08x\n", command[1]);

	for(i = 0; i < (n + 1)/2; i++) {
		command[i+2] |= regs[i*2];
		if((i*2 + 1) < n)
			command[i+2] |= regs[i*2 + 1] << 8;
		//PRINTF("0x%08x\n", command[i+2]);
	}

	_tc358768_wr_regs_32bits(command, (n + 1)/2 + 2);
	tc358768_wr_reg_32bits(0x06000001);   //Packet Transfer
	//wait until packet is out
	i = 100;
	while(tc358768_rd_reg_16bits(0x0600) & 0x01) {
		if(i-- == 0)
			break;
		tc_print(0x0600);
	}
	//local_delay_us(50);
	return 0;
}

int tc358768_command_tx_more8bytes_hs(unsigned char type, unsigned char regs[], uint32_t n) {

	int i = 0;
	unsigned int dbg_data = 0x00E80000, temp = 0;
	unsigned int command[] = {
			0x05000080,    //HS data 4 lane, EOT is added
			0x0502A300,
			0x00080001,
			0x00500000,    //Data ID setting
			0x00220000,    //Transmission byte count= byte
			0x00E08000,	   //Enable I2C/SPI write to VB
			0x00E20048,    //Total word count = 0x48 (max 0xFFF). This value should be adjusted considering trade off between transmission time and transmission start/stop time delay
			0x00E4007F,    //Vertical blank line = 0x7F
	};


	command[3] |= type;        //data type
	command[4] |= n & 0xffff;           //Transmission byte count

	tc358768_wr_regs_32bits(command);

	for(i = 0; i < (n + 1)/2; i++) {
		temp = dbg_data | regs[i*2];
		if((i*2 + 1) < n)
			temp |= (regs[i*2 + 1] << 8);
		//PRINTF("0x%08x\n", temp);
		tc358768_wr_reg_32bits(temp);
	}
	if((n % 4 == 1) ||  (n % 4 == 2))     //4 bytes align
		tc358768_wr_reg_32bits(dbg_data);

	tc358768_wr_reg_32bits(0x00E0C000);     //Start command transmisison
	tc358768_wr_reg_32bits(0x00E00000);	 //Stop command transmission. This setting should be done just after above setting to prevent multiple output
	local_delay_us(200);
	//Re-Initialize
	//tc358768_wr_regs_32bits(re_initialize);
	return 0;
}

//low power mode only for tc358768a
int tc358768_command_tx_more8bytes_lp(unsigned char type, const unsigned char regs[], uint32_t n) {

	int i = 0;
	unsigned int dbg_data = 0x00E80000, temp = 0;
	unsigned int command[] = {
			0x00080001,
			0x00500000,    //Data ID setting
			0x00220000,    //Transmission byte count= byte
			0x00E08000,	   //Enable I2C/SPI write to VB
	};

	command[1] |= type;        //data type
	command[2] |= n & 0xffff;           //Transmission byte count

	tc358768_wr_regs_32bits(command);

	for(i = 0; i < (n + 1)/2; i++) {
		temp = dbg_data | regs[i*2];
		if((i*2 + 1) < n)
			temp |= (regs[i*2 + 1] << 8);
		//PRINTF("0x%08x\n", temp);
		tc358768_wr_reg_32bits(temp);

	}
	if((n % 4 == 1) ||  (n % 4 == 2))     //4 bytes align
		tc358768_wr_reg_32bits(dbg_data);

	tc358768_wr_reg_32bits(0x00E0E000);     //Start command transmisison
	local_delay_us(1000);
	tc358768_wr_reg_32bits(0x00E02000);	 //Keep Mask High to prevent short packets send out
	tc358768_wr_reg_32bits(0x00E00000);	 //Stop command transmission. This setting should be done just after above setting to prevent multiple output
	local_delay_us(10);
	return 0;
}

int _tc358768_send_packet(unsigned char type, const unsigned char regs[], uint32_t n) {

	if(n <= 8) {
		tc358768_command_tx_less8bytes(type, regs, n);
	} else {
		//tc358768_command_tx_more8bytes_hs(type, regs, n);
		tc358768_command_tx_more8bytes_lp(type, regs, n);
	}
	return 0;
}

int tc358768_send_packet(unsigned char type, unsigned char regs[], uint32_t n) {
	return _tc358768_send_packet(type, regs, n);
}


/*
The DCS is separated into two functional areas: the User Command Set and the Manufacturer Command
Set. Each command is an eight-bit code with 00h to AFh assigned to the User Command Set and all other
codes assigned to the Manufacturer Command Set.
*/
int _mipi_dsi_send_dcs_packet(const unsigned char regs[], uint32_t n) {

	unsigned char type = 0;
	if(n == 1) {
		type = DTYPE_DCS_SWRITE_0P;
	} else if (n == 2) {
		type = DTYPE_DCS_SWRITE_1P;
	} else if (n > 2) {
		type = DTYPE_DCS_LWRITE;
	}
	_tc358768_send_packet(type, regs, n);
	return 0;
}

int mipi_dsi_send_dcs_packet(const unsigned char regs[], uint32_t n) {
	return _mipi_dsi_send_dcs_packet(regs, n);
}


int _tc358768_rd_lcd_regs(unsigned char type, char comd, int size, unsigned char* buf) {

	unsigned char regs[8];
	uint32_t count = 0, data30, data32;
	regs[0] = size;
	regs[1] = 0;
	tc358768_command_tx_less8bytes(0x37, regs, 2);
	tc358768_wr_reg_32bits(0x05040010);
	tc358768_wr_reg_32bits(0x05060000);
	regs[0] = comd;
	tc358768_command_tx_less8bytes(type, regs, 1);

	while (!(tc358768_rd_reg_16bits(0x0410) & 0x20)){
		PRINTF("error 0x0410:%04x\n", tc358768_rd_reg_16bits(0x0410));
		local_delay_ms(1);
		if(count++ > 10) {
			break;
		}
	}

	data30 = tc358768_rd_reg_16bits(0x0430);	  //data id , word count[0:7]
	//PRINTF("0x0430:%04x\n", data30);
	data32 = tc358768_rd_reg_16bits(0x0432);	  //word count[8:15]  ECC
	//PRINTF("0x0432:%04x\n", data32);

	while(size > 0) {
		data30 = tc358768_rd_reg_16bits(0x0430);
		//PRINTF("0x0430:%04x\n", data30);
		data32 = tc358768_rd_reg_16bits(0x0432);
		//PRINTF("0x0432:%04x\n", data32);

		if(size-- > 0)
			*buf++ = (uint8_t)data30;
		else
			break;
		if(size-- > 0)
			*buf++ = (uint8_t) (data30 >> 8);
		else
			break;
		if(size-- > 0) {
			*buf++ = (uint8_t)data32;
			if(size-- > 0)
				*buf++ = (uint8_t) (data32 >> 8);
		}
	}

	data30 = tc358768_rd_reg_16bits(0x0430);
	//PRINTF("0x0430:%04x\n", data30);
	data32 = tc358768_rd_reg_16bits(0x0432);
	//PRINTF("0x0432:%04x\n", data32);
	return 0;
}

int mipi_dsi_read_dcs_packet(unsigned char *data, uint32_t n) {
	//DCS READ
	_tc358768_rd_lcd_regs(0x06, *data, n, data);
	return 0;
}

int tc358768_get_id(void) {

	int id = -1;

	//tc358768_power_up();
	id = tc358768_rd_reg_16bits(0);
	return id;
}


#define TSC_I2C_ADDR (0x20 * 2)

void tscinit(void)
{
	const unsigned i2caddr = TSC_I2C_ADDR;


	i2c_start(i2caddr | 0x00);
	i2c_write(0xFF);		// set page addr
	i2c_write(0x00);		// page #0
	i2c_waitsend();
    i2c_stop();
}

void tscid(void)
{
	const unsigned i2caddr = TSC_I2C_ADDR;

	uint8_t v0;

	i2c_start(i2caddr | 0x00);
	i2c_write_withrestart(0xE1);	//  Manufacturer ID register
	i2c_start(i2caddr | 0x01);
	i2c_read(& v0, I2C_READ_ACK_NACK);	// ||	The Manufacturer ID register always returns data $01.

	PRINTF("tsc id=%08lX (expected 0x01)\n", v0);
}

void tscprint(void)
{
	const unsigned i2caddr = TSC_I2C_ADDR;


	uint8_t v0, v1, v2, v3, v4, v5, v6, v7;

	i2c_start(i2caddr | 0x00);
	i2c_write_withrestart(0x06);	// Address=0x0006 is used to read coordinate.
	i2c_start(i2caddr | 0x01);
	i2c_read(& v0, I2C_READ_ACK_1);	// ||
	i2c_read(& v1, I2C_READ_ACK);	// ||
	i2c_read(& v2, I2C_READ_ACK);	// ||
	i2c_read(& v3, I2C_READ_ACK);	// ||
	i2c_read(& v4, I2C_READ_ACK);	// ||
	i2c_read(& v5, I2C_READ_ACK);	// ||
	i2c_read(& v6, I2C_READ_ACK);	// ||
	i2c_read(& v7, I2C_READ_NACK);	// ||

	unsigned vz1 =
			(((unsigned long) v3) << 24) |
			(((unsigned long) v2) << 16) |
			(((unsigned long) v1) << 8) |
			(((unsigned long) v0) << 0) |
			0;

	unsigned vz2 =
			(((unsigned long) v7) << 24) |
			(((unsigned long) v6) << 16) |
			(((unsigned long) v5) << 8) |
			(((unsigned long) v4) << 0) |
			0;

	PRINTF("tsc=%08lX %08lX ", vz1, vz2);
}
/*
 *
 *
static uint8_t bigon [] =
1, 5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x00,
2, 3, 0xB0, 0x00, 0x10, 0x10,
3, 1, 0xBA, 0x60,
4, 7, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
5, 8, 0xC0, 0xC0, 0x04, 0x00, 0x20, 0x02, 0xE4, 0xE1, 0xC0,
6, 8, 0xC1, 0xC0, 0x04, 0x00, 0x20, 0x04, 0xE4, 0xE1, 0xC0,
7, 5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x02,
8, 5, 0xEA, 0x7F, 0x20, 0x00,, 0x00, 0x00,
9, 1, 0xCA, 0x04,
10, 1, 0xE1, 0x00,
11, 1, 0xE2, 0x0A,
12, 1, 0xE3, 0x40,
13, 4, 0xE7, 0x00, 0x00, 0x00, 0x00,
14, 8, 0xED, 0x48, 0x00, 0xE0, 0x13, 0x08, 0x00, 0x91, 0x08,
15, 6, 0xFD, 0x00, 0x08, 0x1C, 0x00, 0x00, 0x01,
16, 11, 0xC3, 0x11, 0x24, 0x04, 0x0A, 0x02, 0x04, 0x00, 0x1C, 0x10, 0xF0, 0x00
17, 5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x03,
18, 1, 0xE0, 0x00,
19, 6, 0xF1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15,
20, 1, 0xF6, 0x08,
21, 5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x05,
22, 5, 0xC3, 0x00, 0x10, 0x50, 0x50, 0x50,
23, 2, 0xC4, 0x00, 0x14,
24, 1, 0xC9, 0x04,
25, 5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x01,
26, 3, 0xB0, 0x06, 0x06, 0x06,
27, 3, 0xB1, 0x14, 0x14, 0x14,
28, 3, 0xB2, 0x00, 0x00, 0x00,
29, 3, 0xB4, 0x66, 0x66, 0x66,
30, 3, 0xB5, 0x44, 0x44, 0x44,
31, 3, 0xB6, 0x54, 0x54, 0x54,
32, 3, 0xB7, 0x24, 0x24, 0x24,
33, 3, 0xB9, 0x04, 0x04, 0x04,
34, 3, 0xBA, 0x14, 0x14, 0x14,
35, 3, 0xBE, 0x22, 0x38, 0x78,
36, 1, 0x35, 0x00,

 *
 */
static uint8_t bigon [] =
{
	5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x00,
	3, 0xB0, 0x00, 0x10, 0x10,
	1, 0xBA, 0x60,
	7, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	8, 0xC0, 0xC0, 0x04, 0x00, 0x20, 0x02, 0xE4, 0xE1, 0xC0,
	8, 0xC1, 0xC0, 0x04, 0x00, 0x20, 0x04, 0xE4, 0xE1, 0xC0,
	5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x02,
	5, 0xEA, 0x7F, 0x20, 0x00, 0x00, 0x00,
	1, 0xCA, 0x04,
	1, 0xE1, 0x00,
	1, 0xE2, 0x0A,
	1, 0xE3, 0x40,
	4, 0xE7, 0x00, 0x00, 0x00, 0x00,
	8, 0xED, 0x48, 0x00, 0xE0, 0x13, 0x08, 0x00, 0x91, 0x08,
	6, 0xFD, 0x00, 0x08, 0x1C, 0x00, 0x00, 0x01,
	11, 0xC3, 0x11, 0x24, 0x04, 0x0A, 0x02, 0x04, 0x00, 0x1C, 0x10, 0xF0, 0x00,
	5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x03,
	1, 0xE0, 0x00,
	6, 0xF1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15,
	1, 0xF6, 0x08,
	5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x05,
	5, 0xC3, 0x00, 0x10, 0x50, 0x50, 0x50,
	2, 0xC4, 0x00, 0x14,
	1, 0xC9, 0x04,
	5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x01,
	3, 0xB0, 0x06, 0x06, 0x06,
	3, 0xB1, 0x14, 0x14, 0x14,
	3, 0xB2, 0x00, 0x00, 0x00,
	3, 0xB4, 0x66, 0x66, 0x66,
	3, 0xB5, 0x44, 0x44, 0x44,
	3, 0xB6, 0x54, 0x54, 0x54,
	3, 0xB7, 0x24, 0x24, 0x24,
	3, 0xB9, 0x04, 0x04, 0x04,
	3, 0xBA, 0x14, 0x14, 0x14,
	3, 0xBE, 0x22, 0x38, 0x78,
	1, 0x35, 0x00,

	0,
};


/*
	panel {
		compatible = "samsung,s6e8aa0";
		reg = <0>;
		vdd3-supply = <&vcclcd_reg>;
		vci-supply = <&vlcd_reg>;
		reset-gpios = <&gpy4 5 0>;
		power-on-delay= <50>;
		reset-delay = <100>;
		init-delay = <100>;
		panel-width-mm = <58>;
		panel-height-mm = <103>;
		flip-horizontal;
		flip-vertical;
		display-timings {
			timing0: timing-0 {
				clock-frequency = <57153600>;
				hactive = <720>;
				vactive = <1280>;
				hfront-porch = <5>;
				hback-porch = <5>;
				hsync-len = <5>;
				vfront-porch = <16>;
				vback-porch = <11>;
				vsync-len = <5>;
			};
		};
	};
*/
void tc358768_initialize(void)
{
	struct tc358768_drv_data * ddata = & dev0;

	dev0.bitclk = 800000000uL;
	if (toshiba_ddr_power_init())
	{
		PRINTF("TC358768 power init failure\n");
		return;
	}
	stpmic1_dump_regulators();
	// See also:
	// https://github.com/bbelos/rk3188-kernel/blob/master/drivers/video/rockchip/transmitter/tc358768.c
	// https://coral.googlesource.com/linux-imx/+/refs/heads/alpha/arch/arm64/boot/dts/freescale/fsl-imx8mq-evk-dcss-rm67191.dts
	// https://developer.toradex.com/knowledge-base/display-output-resolution-and-timings-linux
	// https://code.woboq.org/linux/linux/Documentation/devicetree/bindings/display/panel/samsung,s6e8aa0.txt.html

	const portholder_t TE = (1uL << 7);	// PC7 (TE) - panel pin 29 Sync signal from driver IC
	const portholder_t OTP_PWR = (1uL << 7);	// PD7 (CTRL - OTP_PWR) - panel pin 30
	arm_hardware_pioc_inputs(TE);
	arm_hardware_piod_outputs(OTP_PWR, 1 * OTP_PWR);
	// active low
	const portholder_t RESET = (1uL << 1);	// PD1 = RESX_18 - pin  28
	arm_hardware_piod_outputs(RESET, 0 * RESET);
	local_delay_ms(5);
	arm_hardware_piod_outputs(RESET, 1 * RESET);

	// TP_RESX - active low
	//	x-gpios = <&gpiog 0 GPIO_ACTIVE_HIGH>; /* TP_RESX_18 */
	const portholder_t TP_RESX = (1uL << 0);	// PG0 - TP_RESX_18 - pin 03
	arm_hardware_piog_outputs(TP_RESX, 0 * TP_RESX);
	local_delay_ms(5);
	arm_hardware_piog_outputs(TP_RESX, 1 * TP_RESX);


	// TC358778XBG conrol
	//	x-gpios = <&gpioa 10 GPIO_ACTIVE_HIGH>; /* Video_RST */
	//	x-gpios = <&gpiof 14 GPIO_ACTIVE_HIGH>; /* Video_MODE: 0: test, 1: normal */
	const portholder_t Video_RST = (1uL << 10);	// PA10
	const portholder_t Video_MODE = (1uL << 14);	// PF14: Video_MODE: 0: test, 1: normal

	arm_hardware_piof_outputs(Video_MODE, 1 * Video_MODE);
	arm_hardware_pioa_outputs(Video_RST, 0 * Video_RST);
	local_delay_ms(5);
	arm_hardware_pioa_outputs(Video_RST, 1 * Video_RST);
	PRINTF("TC358778XBG reset off\n");

	local_delay_ms(300);

//
	// addr 0E: ID=02000144
	// TC358778XBG
#if 0
	// Reset
	tc358768_write(ddata, TC358768_SYSCTL, 0x001);
	tc358768_write(ddata, TC358768_SYSCTL, 0x000);

	PRINTF("TC358778XBG: Chip and Revision ID=%08lX\n", tc358768_rd_reg_16bits(TC358768_CHIPID));
	PRINTF("TC358778XBG: System Control Register=%08lX\n", tc358768_rd_reg_16bits(TC358768_SYSCTL));
	PRINTF("TC358778XBG: Input Control Register=%08lX\n", tc358768_rd_reg_16bits(TC358768_CONFCTL));
	PRINTF("TC358778XBG: Data Format Control Register=%08lX\n", tc358768_rd_reg_16bits(TC358768_DATAFMT));
	tc358768_write(ddata, TC358768_DATAFMT, 0x0300);
	local_delay_ms(100);
	PRINTF("TC358778XBG: Data Format Control Register=%08lX\n", tc358768_rd_reg_16bits(TC358768_DATAFMT));

	PRINTF("TC358778XBG: PLL Control Register 0=%08lX\n", tc358768_rd_reg_16bits(TC358768_PLLCTL0));

	tc358768_write(ddata, TC358768_DSI_VSW, VSYNC);
	tc358768_write(ddata, TC358768_DSI_VBPR, VBP);
	tc358768_write(ddata, TC358768_DSI_VACT, HEIGHT);

	tc358768_write(ddata, TC358768_DSI_HSW, HSYNC);
	tc358768_write(ddata, TC358768_DSI_HBPR, HBP);
	tc358768_write(ddata, TC358768_DSI_HACT, WIDTH);

	PRINTF("TC358778XBG: vact=%ld\n", tc358768_rd_reg_16bits(TC358768_DSI_VACT));
	PRINTF("TC358778XBG: hact=%ld\n", tc358768_rd_reg_16bits(TC358768_DSI_HACT));
#endif

	PRINTF("TC358778XBG: Chip and Revision ID=%04lX\n", tc358768_rd_reg_16bits(TC358768_CHIPID));
	tc358768_calc_pll(ddata);

	tc358768_power_on(ddata);

	PRINTF("TC358778XBG: vact=%ld\n", tc358768_rd_reg_16bits(TC358768_DSI_VACT));
	PRINTF("TC358778XBG: hact=%ld\n", tc358768_rd_reg_16bits(TC358768_DSI_HACT));

#if 1
	tscinit();
	tscid();
	for (;0;)
	{
		tscprint();
	}
#endif

	// RM69052 chip
	static uint8_t sleepout [] = { 0x11, 0x00, };
	static uint8_t displon [] = { 0x29, 0x00, };


	local_delay_ms(200);
	mipi_dsi_send_dcs_packet(sleepout, ARRAY_SIZE(sleepout));
	local_delay_ms(200);
	mipi_dsi_send_dcs_packet(displon, ARRAY_SIZE(displon));
	local_delay_ms(200);

	const uint8_t * pv = bigon;
	for (;;)
	{
		const uint8_t maxv = * pv;
		if (maxv == 0)
			break;
		mipi_dsi_send_dcs_packet(pv + 1, maxv + 1);
		local_delay_ms(25);
		pv += maxv + 2;
	}
	PRINTF("display on\n");

}

#endif /* LCDMODETX_TC358778XBG */

#if WITHGPUHW

// Graphic processor unit
void board_gpu_initialize(void)
{
	PRINTF("board_gpu_initialize start.\n");

	RCC->MP_AHB6ENSETR = RCC_MC_AHB6ENSETR_GPUEN;
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6LPENSETR = RCC_MC_AHB6LPENSETR_GPULPEN;
	(void) RCC->MP_AHB6LPENSETR;

	PRINTF("board_gpu_initialize: PRODUCTID=%08lX\n", (unsigned long) GPU->PRODUCTID);

//	GPU_IRQn

	PRINTF("board_gpu_initialize done.\n");
}

#endif /* WITHGPUHW */
