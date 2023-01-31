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

#if WITHLTDCHW

#include "display.h"
#include <stdint.h>
#include <string.h>
#include <limits.h>

#include "formats.h"	// for debug prints
#include "gpio.h"
#include "src/touch/touch.h"

// LQ043T3DX02K rules: While “VSYNC” is “Low”, don’t change “DISP” signal “Low” to “High”.

static void ltdc_tfcon_cfg(const videomode_t * vdmode)
{
	// LQ043T3DX02K rules: While “VSYNC” is “Low”, don’t change “DISP” signal “Low” to “High”.
	if (vdmode->lq43reset)
	{
		/* Configure the LCD Control pins */
#if defined (HARDWARE_LTDC_INITIALIZE)
		HARDWARE_LTDC_INITIALIZE(0);
#endif /* defined (HARDWARE_LTDC_INITIALIZE) */
#if defined (HARDWARE_LTDC_SET_DISP)
		/* управление состоянием сигнала DISP панели */
		/* SONY PSP-1000 display (4.3") required. */
		HARDWARE_LTDC_SET_DISP(0);
		local_delay_ms(150);
		HARDWARE_LTDC_SET_DISP(1);
#endif /* defined (HARDWARE_LTDC_SET_DISP) */
	}
	else
	{
#if defined (HARDWARE_LVDS_INITIALIZE) && 0
		/* Configure the LCD Control pins */
		HARDWARE_LVDS_INITIALIZE();
#elif defined (HARDWARE_LTDC_INITIALIZE)
		/* Configure the LCD Control pins */
#if WITHLCDDEMODE
		HARDWARE_LTDC_INITIALIZE(1);	// подключение к выводам процессора сигналов периферийного контроллера
#else /* WITHLCDDEMODE */
		HARDWARE_LTDC_INITIALIZE(0);	// подключение к выводам процессора сигналов периферийного контроллера
#endif /* WITHLCDDEMODE */
#endif /* defined (HARDWARE_LTDC_INITIALIZE) */
	}
}

static void hardware_ltdc_vsync(void);

#if CPUSTYLE_R7S721

// Записываем и ожидаем пока указанные биты сбросятся в 0
static void vdc5_update(
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
			PRINTF(PSTR("vdc5_update: wait reg=%p %s mask=%08X, stay=%08X\n"), reg, label, (unsigned) mask, (unsigned) (* reg & mask));
			return;
		}
	}
}

// Ожидаем пока указанные биты сбросятся в 0
static void vdc5_wait(
	volatile uint32_t * reg,
	const char * label,
	uint_fast32_t mask
	)
{
	uint_fast32_t count = 1000;
	while ((* reg & mask) != 0)
	{
		local_delay_ms(1);
		if (-- count == 0)
		{
			PRINTF(PSTR("vdc5_wait: wait reg=%p %s mask=%08X, stay=%08X\n"), reg, label, (unsigned) mask, (unsigned) (* reg & mask));
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
		mask = (mask << 1) | 1u; \
	mask <<= (pos); \
	val <<= (pos); \
	ASSERT((val & mask) == val); \
	* (reg) = (* (reg) & ~ (mask)) | (val & mask); \
	(void) * (reg);	/* dummy read */ \
	uint_fast32_t count = 1000; \
	do { \
		if (count -- == 0) {PRINTF(PSTR("SETREG32_UPDATE: wait %s/%d\n"), __FILE__, __LINE__); break; } \
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
		mask = (mask << 1) | 1u; \
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

static void vdc5fb_init_syscnt(struct st_vdc5 * vdc, const videomode_t * vdmode)
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
	SETREG32_CK(& vdc->SYSCNT_PANEL_CLK, 6, 0, calcdivround2(P1CLOCK_FREQ, display_getdotclock(vdmode)));	/* Clock Frequency Division Ratio Note: Settings other than those in Table 35.5 are prohibited. */
	SETREG32_CK(& vdc->SYSCNT_PANEL_CLK, 1, 8, 1);	/* PANEL_ICKEN */

}


static void vdc5fb_init_sync(struct st_vdc5 * const vdc, const videomode_t * vdmode)
{
	/* Accumulated parameters for this display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned HSYNC = vdmode->hsync;	/*  */
	const unsigned VSYNC = vdmode->vsync;	/*  */
	const unsigned LEFTMARGIN = HSYNC + vdmode->hbp;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = VSYNC + vdmode->vbp;	/* vertical delay before DE start */
	const unsigned HFULL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VFULL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

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

static void vdc5fb_init_scalers(struct st_vdc5 * const vdc, const videomode_t * vdmode)
{
	/* Accumulated parameters for this display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned LEFTMARGIN = vdmode->hsync + vdmode->hbp;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = vdmode->vsync + vdmode->vbp;	/* vertical delay before DE start */
	const unsigned HFULL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VFULL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

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

static void vdc5fb_init_graphics(struct st_vdc5 * const vdc, const videomode_t * vdmode)
{
	/* Accumulated parameters for this display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned LEFTMARGIN = vdmode->hsync + vdmode->hbp;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = vdmode->vsync + vdmode->vbp;	/* vertical delay before DE start */
	const unsigned HFULL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VFULL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

	const unsigned MAINROWSIZE = sizeof (PACKEDCOLORPIP_T) * GXADJ(DIM_SECOND);	// размер одной строки в байтах
	// Таблица используемой при отображении палитры
	COLOR24_T xltrgb24 [256];
	display2_xltrgb24(xltrgb24);

#if LCDMODE_MAIN_L8
	const unsigned grx_format_MAIN = 0x05;	// GRx_FORMAT 5: CLUT8
	const unsigned grx_rdswa_MAIN = 0x07;	// GRx_RDSWA 111: (8) (7) (6) (5) (4) (3) (2) (1) [32-bit swap + 16-bit swap + 8-bit swap]
#elif LCDMODE_MAIN_ARGB888
	const unsigned grx_format_MAIN = 0x04;	// GRx_FORMAT 4: ARGB8888, 1: RGB888
	const unsigned grx_rdswa_MAIN = 0x04;	// GRx_RDSWA 100: (5) (6) (7) (8) (1) (2) (3) (4) [Swapped in 32-bit units]
#else /* LCDMODE_MAIN_L8 */
	const unsigned grx_format_MAIN = 0x00;	// GRx_FORMAT 0: RGB565
	const unsigned grx_rdswa_MAIN = 0x06;	// GRx_RDSWA 110: (7) (8) (5) (6) (3) (4) (1) (2) [32-bit swap + 16-bit swap]
#endif /* LCDMODE_MAIN_L8 */

#if LCDMODE_PIP_L8
	const unsigned grx_format_PIP = 0x05;	// GRx_FORMAT 5: CLUT8
	const unsigned grx_rdswa_PIP = 0x07;	// GRx_RDSWA 111: (8) (7) (6) (5) (4) (3) (2) (1) [32-bit swap + 16-bit swap + 8-bit swap]
#elif LCDMODE_PIP_ARGB888
	const unsigned grx_format_MAIN = 0x04;	// GRx_FORMAT 4: ARGB8888, 1: RGB888
	const unsigned grx_rdswa_MAIN = 0x04;	// GRx_RDSWA 100: (5) (6) (7) (8) (1) (2) (3) (4) [Swapped in 32-bit units]
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
	//vdc->GR2_CLUT ^= (1u << 16);	// GR2_CLT_SEL Switch to filled table
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

static void vdc5fb_init_outcnt(struct st_vdc5 * const vdc, const videomode_t * vdmode)
{
	////////////////////////////////////////////////////////////////
	// OUT
	SETREG32_CK(& vdc->OUT_CLK_PHASE, 1, 8, 0x00);	// OUTCNT_LCD_EDGE 0: Output changed at the rising edge of LCD_CLK pin, data latched at falling edge
	SETREG32_CK(& vdc->OUT_SET, 2, 8, 0x00);	// OUT_FRQ_SEL Clock Frequency Control 0: 100% speed — (parallel RGB)
	SETREG32_CK(& vdc->OUT_SET, 2, 12, 0x02);	// OUT_FORMAT Output Format Select 2: RGB565
}

static void vdc5fb_init_tcon(struct st_vdc5 * const vdc, const videomode_t * vdmode)
{
	/* Accumulated parameters for this display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned LEFTMARGIN = vdmode->hsync + vdmode->hbp;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = vdmode->vsync + vdmode->vbp;	/* vertical delay before DE start */
	const unsigned HFULL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VFULL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

	////////////////////////////////////////////////////////////////
	// TCON

	// Vertical sync generation parameters

	// VSYNC signal
	SETREG32_CK(& vdc->TCON_TIM_STVA1, 11, 16, 0);		// TCON_STVA_VS
	SETREG32_CK(& vdc->TCON_TIM_STVA1, 11, 0, vdmode->vsync);	// TCON_STVA_VW

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
	SETREG32_CK(& vdc->TCON_TIM_STH1, 11, 0, vdmode->hsync);	// TCON_STH_HW
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
	SETREG32_CK(& vdc->TCON_TIM_STH2, 1, 4, vdmode->hsyncneg * 0x01);		// TCON_STH_INV
	// VSYNC polarity
	SETREG32_CK(& vdc->TCON_TIM_STVA2, 1, 4, vdmode->vsyncneg * 0x01);		// TCON_STVA_INV
	// DE polarity
	SETREG32_CK(& vdc->TCON_TIM_DE, 1, 0, vdmode->deneg * 0x01);			// TCON_DE_INV

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
hardware_ltdc_initialize(const uintptr_t * frames, const videomode_t * vdmode)
{
	struct st_vdc5 * const vdc = & VDC50;
	//PRINTF(PSTR("hardware_ltdc_initialize start, WIDTH=%d, HEIGHT=%d\n"), WIDTH, HEIGHT);
	//const unsigned ROWSIZE = sizeof framebuff [0];	// размер одной строки в байтах


	/* ---- Supply clock to the video display controller 5  ---- */
	CPG.STBCR9 &= ~ CPG_STBCR9_MSTP91;	// Module Stop 91 0: The video display controller 5 runs.
	(void) CPG.STBCR9;			/* Dummy read */

	vdc5fb_init_syscnt(vdc, vdmode);
	vdc5fb_init_sync(vdc, vdmode);
	vdc5fb_init_scalers(vdc, vdmode);
	vdc5fb_init_graphics(vdc, vdmode);
	vdc5fb_init_outcnt(vdc, vdmode);
	vdc5fb_init_tcon(vdc, vdmode);

	vdc5fb_update_all(vdc);


	ltdc_tfcon_cfg(vdmode);

#if LCDMODE_PIP_RGB565 || LCDMODE_PIP_L8
	hardware_ltdc_pip_off();
#endif /* LCDMODE_PIP_RGB565 || LCDMODE_PIP_L8 */

	//PRINTF(PSTR("hardware_ltdc_initialize done\n"));
}

void
hardware_ltdc_deinitialize(void)
{
	/* ---- Stop clock to the video display controller 5  ---- */
	CPG.STBCR9 |= CPG_STBCR9_MSTP91;	// Module Stop 91 0: The video display controller 5 runs.
	(void) CPG.STBCR9;			/* Dummy read */
}

/* Palette reload */
void hardware_ltdc_L8_palette(void)
{
	struct st_vdc5 * const vdc = & VDC50;
	vdc5fb_L8_palette(vdc);
}

/* set top buffer start */
void hardware_ltdc_pip_set(uintptr_t p)
{
	struct st_vdc5 * const vdc = & VDC50;

	SETREG32_CK(& vdc->GR3_FLM2, 32, 0, p);			// GR3_BASE
	SETREG32_CK(& vdc->GR3_FLM_RD, 1, 0, 1);		// GR3_R_ENB Frame Buffer Read Enable 1: Frame buffer reading is enabled.
	SETREG32_CK(& vdc->GR3_AB1, 2, 0,	0x03);		// GR3_DISP_SEL 3: Blended display of lower-layer graphics and current graphics

	// GR3_IBUS_VEN in GR3_UPDATE is 1.
	// GR3_IBUS_VEN and GR3_P_VEN in GR3_UPDATE are 1.
	// GR3_P_VEN in GR3_UPDATE is 1.

	vdc->GR3_UPDATE = (
		(1 << 8) |	// GR3_UPDATE Frame Buffer Read Control Register Update
		(1 << 4) |	// GR3_P_VEN Graphics Display Register Update
		(1 << 0) |	// GR3_IBUS_VEN Frame Buffer Read Control Register Update
		0);
	(void) vdc->GR3_UPDATE;

	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	vdc5_wait(& vdc->GR3_UPDATE, "GR3_UPDATE",
			(1 << 8) |	// GR3_UPDATE Frame Buffer Read Control Register Update
			(1 << 4) |	// GR3_P_VEN Graphics Display Register Update
			(1 << 0) |	// GR3_IBUS_VEN Frame Buffer Read Control Register Update
			0
		);
}

void hardware_ltdc_pip_off(void)	// set PIP framebuffer address
{
	struct st_vdc5 * const vdc = & VDC50;

	SETREG32_CK(& vdc->GR3_FLM_RD, 1, 0, 0);			// GR3_R_ENB Frame Buffer Read Enable 0: Frame buffer reading is disabled.
	SETREG32_CK(& vdc->GR3_AB1, 2, 0,	0x01);			// GR3_DISP_SEL 1: Lower-layer graphics display

	vdc->GR3_UPDATE = (
		(1 << 8) |	// GR3_UPDATE Frame Buffer Read Control Register Update
		(1 << 4) |	// GR3_P_VEN Graphics Display Register Update
		(1 << 0) |	// GR3_IBUS_VEN Frame Buffer Read Control Register Update
		0);
	(void) vdc->GR3_UPDATE;
}

/* Set MAIN frame buffer address. No waiting for VSYNC. */
/* Вызывается из display_flush, используется только в тестах */
void hardware_ltdc_main_set_no_vsync(uintptr_t p)
{
	struct st_vdc5 * const vdc = & VDC50;

	SETREG32_CK(& vdc->GR2_FLM_RD, 1, 0, 1);		// GR2_R_ENB Frame Buffer Read Enable 1: Frame buffer reading is enabled.
	SETREG32_CK(& vdc->GR2_FLM2, 32, 0, p);			// GR2_BASE
	SETREG32_CK(& vdc->GR2_AB1, 2, 0,	0x02);		// GR2_DISP_SEL 2: Current graphics display

	// GR2_IBUS_VEN in GR2_UPDATE is 1.
	// GR2_IBUS_VEN and GR2_P_VEN in GR2_UPDATE are 1.
	// GR2_P_VEN in GR2_UPDATE is 1.

	vdc->GR2_UPDATE = (
		(1 << 8) |	// GR2_UPDATE Frame Buffer Read Control Register Update
		//(1 << 4) |	// GR2_P_VEN Graphics Display Register Update
		//(1 << 0) |	// GR2_IBUS_VEN Frame Buffer Read Control Register Update
		0);
	(void) vdc->GR2_UPDATE;
}

/* ожидаем начало кадра */
static void hardware_ltdc_vsync(void)
{
	struct st_vdc5 * const vdc = & VDC50;

	// GR2_IBUS_VEN in GR2_UPDATE is 1.
	// GR2_IBUS_VEN and GR2_P_VEN in GR2_UPDATE are 1.
	// GR2_P_VEN in GR2_UPDATE is 1.

	vdc->GR2_UPDATE = (
		(1 << 8) |	// GR2_UPDATE Frame Buffer Read Control Register Update
		(1 << 4) |	// GR2_P_VEN Graphics Display Register Update
		(1 << 0) |	// GR2_IBUS_VEN Frame Buffer Read Control Register Update
		0);
	(void) vdc->GR2_UPDATE;

	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	vdc5_wait(& vdc->GR2_UPDATE, "GR2_UPDATE",
			(1 << 8) |	// GR2_UPDATE Frame Buffer Read Control Register Update
			(1 << 4) |	// GR2_P_VEN Graphics Display Register Update
			(1 << 0) |	// GR2_IBUS_VEN Frame Buffer Read Control Register Update
			0);
}

/* set visible buffer start. Wait VSYNC. */
/* Set MAIN frame buffer address. Wait for VSYNC. */
void hardware_ltdc_main_set(uintptr_t p)
{
	struct st_vdc5 * const vdc = & VDC50;

	SETREG32_CK(& vdc->GR2_FLM_RD, 1, 0, 1);		// GR2_R_ENB Frame Buffer Read Enable 1: Frame buffer reading is enabled.
	SETREG32_CK(& vdc->GR2_FLM2, 32, 0, p);			// GR2_BASE
	SETREG32_CK(& vdc->GR2_AB1, 2, 0,	0x02);		// GR2_DISP_SEL 2: Current graphics display

	hardware_ltdc_vsync();	/* ожидаем начало кадра */
}

/* Set MAIN frame buffer address. Waiting for VSYNC. */
void hardware_ltdc_main_set4(uintptr_t layer0, uintptr_t layer1, uintptr_t layer2, uintptr_t layer3)
{
	ASSERT(layer2 == 0);
	ASSERT(layer3 == 0);
	hardware_ltdc_pip_off();
	hardware_ltdc_main_set(layer0);
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
#define LTDC_IMReload                     LTDC_SRCR_IMR_Msk                         /*!< Immediately Reload. */
#define LTDC_VBReload                     LTDC_SRCR_VBR_Msk                         /*!< Vertical Blanking Reload. */

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

} LTDCx_InitTypeDef;

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
  * @param  LTDC_InitStruct: pointer to a LTDCx_InitTypeDef structure that contains
  *         the configuration information for the specified LTDC peripheral.
  * @retval None
  */

static void LTDCx_Init(LTDCx_InitTypeDef* LTDC_InitStruct)
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

static void LCDx_LayerInit(
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
	/* Alpha constant (255 = непрозрачный) */
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
	////LTDC_Layerx->CR |= LTDC_LxCR_LEN_Msk;

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
	const COLORPIP_T key = COLORPIP_KEY;
	const unsigned keyr = COLORPIP_R(key);
	const unsigned keyg = COLORPIP_G(key);
	const unsigned keyb = COLORPIP_B(key);

	LTDC_Layerx->CKCR = 
		(keyr << LTDC_LxCKCR_CKRED_Pos) |
		(keyg << LTDC_LxCKCR_CKGREEN_Pos) |
		(keyb << LTDC_LxCKCR_CKBLUE_Pos) |
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
static void LCDx_LayerInitPIP(
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
hardware_ltdc_initialize(const uintptr_t * frames, const videomode_t * vdmode)
{
	/* Accumulated parameters for this display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned LEFTMARGIN = vdmode->hsync + vdmode->hbp;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = vdmode->vsync + vdmode->vbp;	/* vertical delay before DE start */
	const unsigned HFULL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VFULL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

	//PRINTF(PSTR("hardware_ltdc_initialize start, WIDTH=%d, HEIGHT=%d\n"), WIDTH, HEIGHT);

	/* Initialize the LCD */

	hardware_set_dotclock(display_getdotclock(vdmode));

#if CPUSTYLE_STM32H7XX

	/* Enable the LTDC Clock */
	RCC->APB3ENR |= RCC_APB3ENR_LTDCEN;	/* LTDC clock enable */
	(void) RCC->APB3ENR;
	RCC->APB3LPENR |= RCC_APB3LPENR_LTDCLPEN_Msk;	/* LTDC clock enable */
	(void) RCC->APB3LPENR;


#elif CPUSTYLE_STM32MP1

	{
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

	/* LTDC Initialization -------------------------------------------------------*/
	LTDCx_InitTypeDef LTDC_InitStruct;

	pipparams_t mainwnd = { 0, 0, DIM_SECOND, DIM_FIRST };

#if LCDMODE_PIP_RGB565 || LCDMODE_PIP_L8
	pipparams_t pipwnd;
	display2_getpipparams(& pipwnd);

	//PRINTF(PSTR("hardware_ltdc_initialize: pip: x/y=%u/%u, w/h=%u/%u\n"), pipwnd.x, pipwnd.y, pipwnd.w, pipwnd.h);
#endif /* LCDMODE_PIP_RGB565 || LCDMODE_PIP_L8 */

	LTDC_InitStruct.LTDC_HSPolarity = vdmode->hsyncneg ? LTDC_HSPolarity_AL : LTDC_HSPolarity_AH;
	//LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AH;     
	/* Initialize the vertical synchronization polarity as active low */  
	LTDC_InitStruct.LTDC_VSPolarity = vdmode->vsyncneg ? LTDC_VSPolarity_AL : LTDC_VSPolarity_AH;
	//LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AH;     
	/* Initialize the data enable polarity as active low */ 
	//LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AH;		// While VSYNC is low, do not change DISP signal "Low" or "High"
	// LTDC datasheet say: "DE: Not data enable"
	LTDC_InitStruct.LTDC_DEPolarity = vdmode->deneg ? LTDC_DEPolarity_AH : LTDC_DEPolarity_AL;		// While VSYNC is low, do not change DISP signal "Low" or "High"
	/* Initialize the pixel clock polarity as input pixel clock */ 
	LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;

	/* Timing configuration */
	/* Configure horizontal synchronization width */     
	LTDC_InitStruct.LTDC_HorizontalSync = (vdmode->hsync - 1);
	/* Configure accumulated horizontal back porch */
	LTDC_InitStruct.LTDC_AccumulatedHBP = (LEFTMARGIN - 1);
	/* Configure accumulated active width */  
	LTDC_InitStruct.LTDC_AccumulatedActiveW = (LEFTMARGIN + WIDTH - 1);
	/* Configure total width */
	LTDC_InitStruct.LTDC_TotalWidth = (HFULL - 1);

	/* Configure vertical synchronization height */
	LTDC_InitStruct.LTDC_VerticalSync = (vdmode->vsync - 1);
	/* Configure accumulated vertical back porch */
	LTDC_InitStruct.LTDC_AccumulatedVBP = (TOPMARGIN - 1);
	/* Configure accumulated active height */
	LTDC_InitStruct.LTDC_AccumulatedActiveH = (TOPMARGIN + HEIGHT - 1);
	/* Configure total height */
	LTDC_InitStruct.LTDC_TotalHeigh = (VFULL - 1);

	/* Configure R,G,B component values for LCD background color */                   
	LTDC_InitStruct.LTDC_BackgroundColor = 0;		// all 0 - black

	LTDCx_Init(& LTDC_InitStruct);

	LTDCx_Init(& LTDC_InitStruct);


	/* LTDC initialization end ---------------------------------------------------*/

	// Top layer - LTDC_Layer2
	// Bottom layer - LTDC_Layer1
#if LCDMODE_MAIN_L24

	LCDx_LayerInit(LAYER_MAIN, LEFTMARGIN, TOPMARGIN, & mainwnd, LTDC_Pixelformat_L8, 3, sizeof (PACKEDCOLORPIP_T));

#elif LCDMODE_MAIN_L8

	LCDx_LayerInit(LAYER_MAIN, LEFTMARGIN, TOPMARGIN, & mainwnd, LTDC_Pixelformat_L8, 1, sizeof (PACKEDCOLORPIP_T));

#elif LCDMODE_MAIN_ARGB888

	/* Без палитры */
	LCDx_LayerInit(LAYER_MAIN, LEFTMARGIN, TOPMARGIN, & mainwnd, LTDC_Pixelformat_ARGB8888, 1, sizeof (PACKEDCOLORPIP_T));

#else
	/* Без палитры */
	LCDx_LayerInit(LAYER_MAIN, LEFTMARGIN, TOPMARGIN, & mainwnd, LTDC_Pixelformat_RGB565, 1, sizeof (PACKEDCOLORPIP_T));

#endif /* LCDMODE_MAIN_L8 */

#if LCDMODE_PIP_L8

	LCD_LayerInitMain(LAYER_MAIN);	// довести инициализацию

	// Bottom layer
	LCDx_LayerInit(LAYER_PIP, LEFTMARGIN, TOPMARGIN, & pipwnd, LTDC_Pixelformat_L8, 1, sizeof (PACKEDCOLORPIP_T));
	LCDx_LayerInitPIP(LAYER_PIP);	// довести инициализацию

#elif LCDMODE_PIP_RGB565

	LCD_LayerInitMain(LAYER_MAIN);	// довести инициализацию

	// Bottom layer
	LCDx_LayerInit(LAYER_PIP, LEFTMARGIN, TOPMARGIN, & pipwnd, LTDC_Pixelformat_RGB565, 1, sizeof (PACKEDCOLORPIP_T));
	LCDx_LayerInitPIP(LAYER_PIP);	// довести инициализацию

#elif LCDMODE_PIP_RGB888

	LCD_LayerInitMain(LAYER_MAIN);	// довести инициализацию

	// Bottom layer
	LCDx_LayerInit(LAYER_PIP, LEFTMARGIN, TOPMARGIN, & pipwnd, LTDC_Pixelformat_ARGB8888, 1, sizeof (PACKEDCOLORPIP_T));
	LCDx_LayerInitPIP(LAYER_PIP);	// довести инициализацию

#endif /* LCDMODE_PIP_RGB565 */

	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		; //hardware_nonguiyield();
	LTDC->SRCR |= LTDC_SRCR_IMR_Msk;	/*!< Immediately Reload. */
	(void) LTDC->SRCR;
	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		; //hardware_nonguiyield();

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

	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		; //hardware_nonguiyield();
	/* LTDC reload configuration */  
	LTDC->SRCR |= LTDC_SRCR_IMR_Msk;	/* Immediately Reload. */
	(void) LTDC->SRCR;
	while ((LTDC->SRCR & LTDC_SRCR_IMR_Msk) != 0)
		;//hardware_nonguiyield();

	ltdc_tfcon_cfg(vdmode);
	//PRINTF(PSTR("hardware_ltdc_initialize done\n"));
}

void
hardware_ltdc_deinitialize(void)
{
	LAYER_PIP->CR &= ~ LTDC_LxCR_LEN_Msk;
	(void) LAYER_PIP->CR;
	LAYER_MAIN->CR &= ~ LTDC_LxCR_LEN_Msk;
	(void) LAYER_MAIN->CR;
	LTDC->SRCR |= LTDC_SRCR_IMR_Msk;	/* Immediately Reload. */
	//while ((LTDC->SRCR & LTDC_SRCR_IMR_Msk) != 0)
	//	;//hardware_nonguiyield();

#if CPUSTYLE_STM32H7XX
    /* Reset pulse to LTDC */
	RCC->APB3RSTR |= RCC_APB3RSTR_LTDCRST_Msk;
	(void) RCC->APB3RSTR;
	RCC->APB3RSTR &= ~ RCC_APB3RSTR_LTDCRST_Msk;
	(void) RCC->APB3RSTR;

	/* Disable the LTDC Clock */
	RCC->APB3ENR &= ~ RCC_APB3ENR_LTDCEN;	/* LTDC clock disable */
	(void) RCC->APB3ENR;

#elif CPUSTYLE_STM32MP1
    /* Reset pulse to LTDC */
	RCC->APB4RSTSETR = RCC_APB4RSTSETR_LTDCRST_Msk;
	(void) RCC->APB4RSTSETR;
	RCC->APB4RSTCLRR = RCC_APB4RSTCLRR_LTDCRST_Msk;
	(void) RCC->APB4RSTCLRR;

	/* Disable the LTDC Clock */
	RCC->MP_APB4ENCLRR = RCC_MP_APB4ENCLRR_LTDCEN;	/* LTDC clock disable */
	(void) RCC->MP_APB4ENCLRR;
	/* Disable the LTDC Clock in low-power mode */
	RCC->MP_APB4LPENCLRR = RCC_MP_APB4LPENCLRR_LTDCLPEN;	/* LTDC clock disable */
	(void) RCC->MP_APB4LPENCLRR;

#endif
}

/* set visible buffer start. Wait VSYNC. */
/* Set PIP frame buffer address. */
void hardware_ltdc_pip_set(uintptr_t p)
{
	LAYER_PIP->CFBAR = p;
	(void) LAYER_PIP->CFBAR;
	LAYER_PIP->CR |= LTDC_LxCR_LEN_Msk;
	(void) LAYER_PIP->CR;
	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		hardware_nonguiyield();
	LTDC->SRCR |= LTDC_SRCR_VBR_Msk;	/* Vertical Blanking Reload. */
	(void) LTDC->SRCR;
	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		hardware_nonguiyield();
}

/* Turn PIP off (main layer only). */
void hardware_ltdc_pip_off(void)
{
	LAYER_PIP->CR &= ~ LTDC_LxCR_LEN_Msk;
	(void) LAYER_PIP->CR;
	LTDC->SRCR |= LTDC_SRCR_VBR_Msk;	/* Vertical Blanking Reload. */
	(void) LTDC->SRCR;
}

/* Palette reload */
void hardware_ltdc_L8_palette(void)
{
	// Таблица используемой при отображении палитры
	COLOR24_T xltrgb24 [256];
	display2_xltrgb24(xltrgb24);
#if LCDMODE_MAIN_L8
	fillLUT_L8(LAYER_MAIN, xltrgb24);	// загрузка палитры - имеет смысл до Reload

	/* LTDC reload configuration */
	LTDC->SRCR |= LTDC_SRCR_IMR_Msk;	/* Immediately Reload. */
	(void) LTDC->SRCR;
	while ((LTDC->SRCR & LTDC_SRCR_IMR_Msk) != 0)
		;//hardware_nonguiyield();
#endif /* LCDMODE_PIP_L8 */
#if LCDMODE_PIP_L8
	fillLUT_L8(LAYER_PIP, xltrgb24);	// загрузка палитры - имеет смысл до Reload

	/* LTDC reload configuration */
	LTDC->SRCR |= LTDC_SRCR_IMR_Msk;	/* Immediately Reload. */
	(void) LTDC->SRCR;
	while ((LTDC->SRCR & LTDC_SRCR_IMR_Msk) != 0)
		;//hardware_nonguiyield();
#endif /* LCDMODE_PIP_L8 */
}


/* Set MAIN frame buffer address. No waiting for VSYNC. */
/* Вызывается из display_flush, используется только в тестах */
void hardware_ltdc_main_set_no_vsync(uintptr_t p)
{
	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		hardware_nonguiyield();
	LAYER_MAIN->CFBAR = p;
	(void) LAYER_MAIN->CFBAR;
	LAYER_MAIN->CR |= LTDC_LxCR_LEN_Msk;
	(void) LAYER_MAIN->CR;

	LTDC->SRCR |= LTDC_SRCR_IMR_Msk;	/* Immediate Reload. */
	(void) LTDC->SRCR;
	while ((LTDC->SRCR & LTDC_SRCR_IMR_Msk) != 0)
		hardware_nonguiyield();
}

/* ожидаем начало кадра */
static void hardware_ltdc_vsync(void)
{
	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		hardware_nonguiyield();

	(void) LAYER_MAIN->CFBAR;
	LAYER_MAIN->CR |= LTDC_LxCR_LEN_Msk;
	(void) LAYER_MAIN->CR;
	LTDC->SRCR |= LTDC_SRCR_VBR_Msk;	/* Vertical Blanking Reload. */
	(void) LTDC->SRCR;

	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		hardware_nonguiyield();
}

/* Set MAIN frame buffer address. Wait for VSYNC. */
void hardware_ltdc_main_set(uintptr_t p)
{
	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		hardware_nonguiyield();

	LAYER_MAIN->CFBAR = p;

	(void) LAYER_MAIN->CFBAR;
	LAYER_MAIN->CR |= LTDC_LxCR_LEN_Msk;
	(void) LAYER_MAIN->CR;
	LTDC->SRCR |= LTDC_SRCR_VBR_Msk;	/* Vertical Blanking Reload. */
	(void) LTDC->SRCR;

	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		hardware_nonguiyield();
}

/* Set MAIN frame buffer address. Waiting for VSYNC. */
void hardware_ltdc_main_set4(uintptr_t layer0, uintptr_t layer1, uintptr_t layer2, uintptr_t layer3)
{
	ASSERT(layer2 == 0);
	ASSERT(layer3 == 0);
	hardware_ltdc_pip_off();
	hardware_ltdc_main_set(layer0);
}

#elif LINUX_SUBSYSTEM

void hardware_ltdc_initialize(const uintptr_t * frames, const videomode_t * vdmode)
{
}

/* Palette reload (dummy fuction) */
void hardware_ltdc_L8_palette(void)
{
}

/* Set MAIN frame buffer address. No waiting for VSYNC. */
/* Вызывается из display_flush, используется только в тестах */
void hardware_ltdc_main_set_no_vsync(uintptr_t addr)
{
	uint32_t size;
	uint32_t * linux_fb = linux_get_fb(& size);
	memcpy(linux_fb, (uint32_t *) addr, size);
}

/* Set MAIN frame buffer address. */
void hardware_ltdc_main_set(uintptr_t addr)
{
	uint32_t size;
	uint32_t * linux_fb = linux_get_fb(& size);
	memcpy(linux_fb, (uint32_t *) addr, size);
}

/* ожидаем начало кадра */
static void hardware_ltdc_vsync(void)
{
}

#elif (CPUSTYLE_XC7Z) && 1

#include "zynq_vdma.h"

static DisplayCtrl dispCtrl;

void hardware_ltdc_initialize(const uintptr_t * frames, const videomode_t * vdmode)
{
	int Status;
	static XAxiVdma AxiVdma;

	Vdma_Init(&AxiVdma, AXI_VDMA_DEV_ID);

	hardware_set_dotclock(display_getdotclock(vdmode));
	Status = DisplayInitialize(& dispCtrl, & AxiVdma, DISP_VTC_ID, DYNCLK_BASEADDR, frames, (unsigned long) GXADJ(DIM_X) * LCDMODE_PIXELSIZE, vdmode);
	if (Status != XST_SUCCESS)
	{
		PRINTF("Display Ctrl initialization failed: %d\r\n", Status);
	}

	Status = DisplayStart(& dispCtrl);
	if (Status != XST_SUCCESS)
	{
		PRINTF("Couldn't start display: %d\r\n", Status);
	}

	ltdc_tfcon_cfg(vdmode);
}

/* Palette reload (dummy fuction) */
void hardware_ltdc_L8_palette(void)
{
}

/* Set MAIN frame buffer address. No waiting for VSYNC. */
/* Вызывается из display_flush, используется только в тестах */
void hardware_ltdc_main_set_no_vsync(uintptr_t addr)
{
	DisplayChangeFrame(&dispCtrl, colmain_getindexbyaddr(addr));
}

/* Set MAIN frame buffer address. */
void hardware_ltdc_main_set(uintptr_t addr)
{
	DisplayChangeFrame(&dispCtrl, colmain_getindexbyaddr(addr));
}

/* ожидаем начало кадра */
static void hardware_ltdc_vsync(void)
{
}

/* Set MAIN frame buffer address. Waiting for VSYNC. */
void hardware_ltdc_main_set4(uintptr_t layer0, uintptr_t layer1, uintptr_t layer2, uintptr_t layer3)
{
	ASSERT(layer2 == 0);
	ASSERT(layer3 == 0);
	//hardware_ltdc_pip_off();
	hardware_ltdc_main_set(layer0);
}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133)


#define T113_DE_BASE		DE_BASE
//
//#define T113_DE_MUX_GLB		(0x00100000 + 0x00000)
//#define T113_DE_MUX_BLD		(0x00100000 + 0x01000)	/* 5.10.3.4 Blender */
#define T113_DE_MUX_CHAN	(0x00100000 + 0x02000)
#define T113_DE_MUX_VSU		(0x00100000 + 0x20000)
#define T113_DE_MUX_GSU1	(0x00100000 + 0x30000)
#define T113_DE_MUX_GSU2	(0x00100000 + 0x40000)
#define T113_DE_MUX_GSU3	(0x00100000 + 0x50000)
#define T113_DE_MUX_FCE		(0x00100000 + 0xa0000)
#define T113_DE_MUX_BWS		(0x00100000 + 0xa2000)
#define T113_DE_MUX_LTI		(0x00100000 + 0xa4000)
#define T113_DE_MUX_PEAK	(0x00100000 + 0xa6000)
#define T113_DE_MUX_ASE		(0x00100000 + 0xa8000)
#define T113_DE_MUX_FCC		(0x00100000 + 0xaa000)
#define T113_DE_MUX_DCSC	(0x00100000 + 0xb0000)

// 1.5 Register Description
struct de_clk_t {
	uint32_t gate_cfg;		// SCLK_GATE DE SCLK Gating Register
	uint32_t bus_cfg;		// ? HCLK_GATE ? DE HCLK Gating Register
	uint32_t rst_cfg;		// AHB_RESET DE AHB Reset register
	uint32_t div_cfg;		// SCLK_DIV DE SCLK Division register
	uint32_t sel_cfg;		// ? DE2TCON ? MUX register
};

// Allwinner_DE2.0_Spec_V1.0
// 5.10.3.4 Blender
// GLB
struct de_glb_t {
	uint32_t ctl;		/** Offset 0x000 Global control register */
	uint32_t status;	/** Offset 0x004 Global status register */
	uint32_t dbuff;		/** Offset 0x008 Global double buffer control register */
	uint32_t size;		/** Offset 0x00C Global size register */
};

// Allwinner_DE2.0_Spec_V1.0
// 5.10.3.4 Blender
// BLD
struct de_bld_t {
	uint32_t fcolor_ctl;	/** BLD_FILL_COLOR_CTL Offset 0x000 BLD fill color control register */
	struct {
		uint32_t fcolor;	/**  BLD fill color register */
		uint32_t insize;	/**  BLD input memory size register */
		uint32_t offset;	/**  BLD input memory offset register */
		uint32_t dum;		/**  filler */
	} attr[4];
	uint32_t dum0[15];
	uint32_t route;			/** BLD_CH_RTCTL Offset 0x080 BLD routing control register */
	uint32_t premultiply;	/** Offset 0x080 BLD pre-multiply control register */
	uint32_t bkcolor;
	uint32_t output_size;
	uint32_t bld_mode[4];	/* BLD_CTL */
	uint32_t dum1[4];
	uint32_t ck_ctl;
	uint32_t ck_cfg;
	uint32_t dum2[2];
	uint32_t ck_max[4];
	uint32_t dum3[4];
	uint32_t ck_min[4];
	uint32_t dum4[3];
	uint32_t out_ctl;
};

struct de_vi_t {
	struct {
		uint32_t attr;
		uint32_t size;
		uint32_t coord;
		uint32_t pitch[3];
		uint32_t top_laddr[3];
		uint32_t bot_laddr[3];
	} cfg[4];
	uint32_t fcolor[4];
	uint32_t top_haddr[3];
	uint32_t bot_haddr[3];
	uint32_t ovl_size[2];
	uint32_t hori[2];
	uint32_t vert[2];
};

// 5.10.3.4 Blender
// part
struct de_ui_t {
	struct {
		uint32_t attr;
		uint32_t size;
		uint32_t coord;
		uint32_t pitch;
		uint32_t top_laddr;
		uint32_t bot_laddr;
		uint32_t fcolor;
		uint32_t dum;
	} cfg[4];
	uint32_t top_haddr;
	uint32_t bot_haddr;
	uint32_t ovl_size;
};

#define UI_CFG_INDEX 0	/* 0..3 используется одна конфигурация */
#define VI_CFG_INDEX 0

static uint32_t read32(uintptr_t a)
{
	return * (volatile uint32_t *) a;
}

static void write32(uintptr_t a, uint32_t v)
{
	* (volatile uint32_t *) a = v;
}

struct fb_t113_rgb_pdata_t
{
	//uintptr_t virt_de;	// 0: struct de_vi_t, 1..3: struct de_ui_t
	//uintptr_t virt_tconlcd;

	//char * clk_de;
	//char * clk_tconlcd;
	//int rst_de;
	//int rst_tconlcd;
	int width;
	int height;
	//int pwidth;
	//int pheight;
	//int bits_per_pixel;
	//int bytes_per_pixel;
	//int pixlen;
	//int index;
	//uintptr_t vram [2];
	//struct region_list_t * nrl, * orl;

//	struct led_t * backlight;
//	int brightness;
};

#if LCDMODE_MAIN_ARGB888
	static const uint32_t ui_vi_format = 0x00;	//  0x08: ARGB_8888
	//const uint32_t ui_vi_format = 0x04;	// 0x04: XRGB_8888
#elif LCDMODE_MAIN_RGB565
	static const uint32_t ui_vi_format = 0x0A;	// 0x0A: RGB_565
#else
	#error Unsupported framebuffer format. Looks like you need remove WITHLTDCHW
	static const uint32_t ui_vi_format = 0x0A;
#endif

static void inline t113_de_enable(struct fb_t113_rgb_pdata_t * pdat)
{
	struct de_glb_t * const glb = (struct de_glb_t *) DE_GLB_BASE;
	write32((uintptr_t) & glb->dbuff, 1u);	// 1: register value be ready for update (self-cleaning bit)
	while ((read32((uintptr_t) & glb->dbuff) & 1u) != 0)
		;
}

static inline void t113_de_set_address_vi(struct fb_t113_rgb_pdata_t * pdat, uintptr_t vram)
{
	struct de_vi_t * const vi = (struct de_vi_t *) (DE_BASE + T113_DE_MUX_CHAN + 0x1000 * 0);

	write32((uintptr_t)&vi->cfg[VI_CFG_INDEX].attr,
			((vram != 0) << 0) |	// enable
			(ui_vi_format<<8)|//нижний слой: 32 bit ABGR 8:8:8:8 без пиксельной альфы
			(1<<15)|	// Video_UI_SEL 0: Video Overlay(using Video Overlay Layer Input data format) 1: UI Overlay(using UI Overlay Layer Input data format)
			0
			);
	write32((uintptr_t) & vi->cfg [UI_CFG_INDEX].top_laddr, vram);
	write32((uintptr_t) & vi->top_haddr, (0xFF & (vram >> 32)) << (8 * UI_CFG_INDEX));
}

static inline void t113_de_set_address_ui(struct fb_t113_rgb_pdata_t * pdat, uintptr_t vram, int uich)
{
	ASSERT(uich >= 1 && uich <= 3);
	struct de_ui_t * const ui = (struct de_ui_t *) (DE_BASE + T113_DE_MUX_CHAN + 0x1000 * uich);

	write32((uintptr_t)&ui->cfg[UI_CFG_INDEX].attr,
			((vram != 0) << 0) |	// enable
			(ui_vi_format<<8)| //верхний слой: 32 bit ABGR 8:8:8:8 с пиксельной альфой
			(255<<24)|	// LAY_GLBALPHA
			(1<<16)| 	// LAY_PREMUL_CTL
			0
			);
	write32((uintptr_t) & ui->cfg [UI_CFG_INDEX].top_laddr, vram);
	write32((uintptr_t) & ui->top_haddr, (0xFF & (vram >> 32)) << (8 * UI_CFG_INDEX));
}

static inline void t113_de_set_mode(struct fb_t113_rgb_pdata_t * pdat)
{
	struct de_clk_t * const clk = (struct de_clk_t *) DE_CLK_BASE;
	struct de_glb_t * const glb = (struct de_glb_t *) DE_GLB_BASE;		// Global control register
	struct de_bld_t * const bld = (struct de_bld_t *) DE_BLD_BASE;

	// Allwinner_DE2.0_Spec_V1.0.pdf
	// 5.10.8.2 OVL_UI memory block size register
	// 28..16: LAY_HEIGHT
	// 12..0: LAY_WIDTH
	const uint32_t ovl_ui_mbsize = (((pdat->height - 1) << 16) | (pdat->width - 1));
	const uint32_t uipitch = LCDMODE_PIXELSIZE * GXADJ(DIM_X);

	uint32_t val;

	int i;

//	PRINTF("div_cfg=%08X\n", read32((uintptr_t) & clk->div_cfg));
//	write32((uintptr_t) & clk->div_cfg, ~ 0);
//	PRINTF("div_cfg=%08X\n", read32((uintptr_t) & clk->div_cfg));

	val = read32((uintptr_t) & clk->rst_cfg);
	val |= 1u << 0;
	write32((uintptr_t) & clk->rst_cfg, val);

	val = read32((uintptr_t) & clk->gate_cfg);
	val |= 1u << 0;
	write32((uintptr_t) & clk->gate_cfg, val);

	val = read32((uintptr_t) & clk->bus_cfg);
	val |= 1u << 0;
	write32((uintptr_t) & clk->bus_cfg, val);

	val = read32((uintptr_t) & clk->sel_cfg);
	val &= ~(1u << 0);
	write32((uintptr_t) & clk->sel_cfg, val);

	write32((uintptr_t) & glb->ctl,
			(1u << 12) |	// OUT_DATA_WB 0:RT-WB fetch data after DEP port
			(1u << 0) |		// EN RT enable/disable
			0
			);

	write32((uintptr_t) & glb->status, 0x00u);

	write32((uintptr_t) & glb->dbuff, 1u);		// 1: register value be ready for update (self-cleaning bit)
	while ((read32((uintptr_t) & glb->dbuff) & 1u) != 0)
		;
	write32((uintptr_t) & glb->size, ovl_ui_mbsize);

//	for(i = 0; i < 4; i++)
//	{
//		void * chan = (void *)(T113_DE_BASE + T113_DE_MUX_CHAN + 0x1000 * i);
//
//		// peripherial registers
//		memset(chan, 0, i == 0 ? sizeof (struct de_vi_t) : sizeof (struct de_ui_t));
//	}

	// peripherial registers
	//memset(bld, 0, sizeof (struct de_bld_t));

	// 5.10.9.1 BLD fill color control register
	// BLD_FILL_COLOR_CTL
	write32((uintptr_t) & bld->fcolor_ctl,
			0
			);

	// 5.10.9.5 BLD routing control register
	// BLD_CH_RTCTL
	// 0x03020100 - default state
	write32((uintptr_t) & bld->route,
			(0u << 0) |		// pipe 0 from ch 0
			(1u << 4) |		// pipe 1 from ch 1
			(2u << 8) |		// pipe 2 from ch 2
			(3u << 12) |	// pipe 3 from ch 3
			0
			);
	write32((uintptr_t) & bld->premultiply,
			0);
	write32((uintptr_t) & bld->bkcolor,
			0*0xff771111
			);

	// 5.10.9.1 BLD fill color control register
	// BLD_CTL
	// в примерах только 0 и 1 индексы
	for(i = 0; i < 4; i++)
	{
	    unsigned bld_mode = 0x03010301;		// default
//	    unsigned bld_mode = 0x03020302;           //Fs=Ad, Fd=1-As, Qs=Ad, Qd=1-As
		write32((uintptr_t) & bld->bld_mode [i],
				bld_mode
				);
		//PRINTF("bld->bld_mode [%d]=%08X\n", i, (unsigned) read32((uintptr_t) & bld->bld_mode [i]));
	}

	write32((uintptr_t) & bld->output_size,
			ovl_ui_mbsize
			);
	write32((uintptr_t) & bld->out_ctl,
			0);
	write32((uintptr_t) & bld->ck_ctl,
			0);
	for(i = 0; i < 4; i++)
	{
		write32((uintptr_t) & bld->attr [i].fcolor, 0*0xff000000);
		write32((uintptr_t) & bld->attr [i].insize, ovl_ui_mbsize);
		write32((uintptr_t) & bld->attr [i].offset, 0);
	}

	{
		struct de_vi_t * const vi = (struct de_vi_t *) (DE_BASE + T113_DE_MUX_CHAN + 0x1000 * 0);

		//CH0 VI ----------------------------------------------------------------------------

		write32((uintptr_t)&vi->cfg[0].attr,
				//(1<<0)|		// enable - разрешаем при назначении адреса
				(ui_vi_format<<8)|//нижний слой: 32 bit ABGR 8:8:8:8 без пиксельной альфы
				(1<<15)
				);
		write32((uintptr_t)&vi->cfg[0].size, ovl_ui_mbsize);
		write32((uintptr_t)&vi->cfg[0].coord, 0);
		write32((uintptr_t)&vi->cfg[0].pitch[0], uipitch);
		//write32((uintptr_t)&vi->cfg[0].top_laddr[0], pdat->vram [0]);                               //VIDEO_MEMORY0
		write32((uintptr_t)&vi->ovl_size[0], ovl_ui_mbsize);
	}

	int uich = 1;
	for (uich = 1; uich <= 3; ++ uich)
	{
		ASSERT(uich >= 1 && uich <= 3);
		struct de_ui_t * const ui = (struct de_ui_t *) (DE_BASE + T113_DE_MUX_CHAN + 0x1000 * uich);

		//CH1 UI -----------------------------------------------------------------------------

		write32((uintptr_t)&ui->cfg[UI_CFG_INDEX].attr,
				//(1<<0)|		// enable - разрешаем при назначении адреса
				(ui_vi_format<<8)| //верхний слой: 32 bit ABGR 8:8:8:8 с пиксельной альфой
				(0xff<<24)|
				(1<<16)	// LAY_PREMUL_CTL
				);
		write32((uintptr_t)&ui->cfg[UI_CFG_INDEX].size, ovl_ui_mbsize);
		write32((uintptr_t)&ui->cfg[UI_CFG_INDEX].coord, 0);
		write32((uintptr_t)&ui->cfg[UI_CFG_INDEX].pitch, uipitch);
		//write32((uintptr_t)&ui->cfg[0].top_laddr,pdat->vram [1]);                                  //VIDEO_MEMORY1
		write32((uintptr_t)&ui->ovl_size, ovl_ui_mbsize);
	}


	write32(DE_BASE + T113_DE_MUX_VSU, 0);
	write32(DE_BASE + T113_DE_MUX_GSU1, 0);
	write32(DE_BASE + T113_DE_MUX_GSU2, 0);
	write32(DE_BASE + T113_DE_MUX_GSU3, 0);
	write32(DE_BASE + T113_DE_MUX_FCE, 0);
	write32(DE_BASE + T113_DE_MUX_BWS, 0);
	write32(DE_BASE + T113_DE_MUX_LTI, 0);
	write32(DE_BASE + T113_DE_MUX_PEAK, 0);
	write32(DE_BASE + T113_DE_MUX_ASE, 0);
	write32(DE_BASE + T113_DE_MUX_FCC, 0);
	write32(DE_BASE + T113_DE_MUX_DCSC, 0);


	// Allwinner_DE2.0_Spec_V1.0.pdf
}

static void t113_tconlcd_enable(struct fb_t113_rgb_pdata_t * pdat)
{
//	struct t113_tconlcd_reg_t * const tcon = (struct t113_tconlcd_reg_t *) TCON_LCD0_BASE;
//	uint32_t val;
//
//	val = read32((uintptr_t) & tcon->gctrl);
//	val |= (1u << 31);
//	write32((uintptr_t) & tcon->gctrl, val);
	TCON_LCD0->LCD_GCTL_REG =
		(1u << 31) |		// LCD_EN
		0 * (1u << 30) |	// LCD_GAMMA_EN
		0;
}

static void t113_tconlcd_disable(struct fb_t113_rgb_pdata_t * pdat)
{
//	struct t113_tconlcd_reg_t * const tcon = (struct t113_tconlcd_reg_t *) TCON_LCD0_BASE;
//	uint32_t val;
//
//	val = read32((uintptr_t) & tcon->dclk);
//	val &= ~(0xf << 28);
//	write32((uintptr_t) & tcon->dclk, val);
//
//	write32((uintptr_t) & tcon->gctrl, 0);
//	write32((uintptr_t) & tcon->gint0, 0);

	TCON_LCD0->LCD_DCLK_REG &= ~ (0xfu << 28);
	TCON_LCD0->LCD_GCTL_REG = 0;
	TCON_LCD0->LCD_GINT0_REG = 0;
}

static void t113_tconlcd_set_timing(struct fb_t113_rgb_pdata_t * pdat, const videomode_t * vdmode)
{

	struct {
		//int pixel_clock_hz;
		int h_front_porch;
		int h_back_porch;
		int h_sync_len;
		int v_front_porch;
		int v_back_porch;
		int v_sync_len;
		int h_sync_active;	// 1 - negatibe pulses, 0 - positice pulses
		int v_sync_active;	// 1 - negatibe pulses, 0 - positice pulses
		int den_active;		// 1 - negatibe pulses, 0 - positice pulses
		int clk_active;		// 1 - negatibe pulses, 0 - positice pulses
	} timing;

	//timing.pixel_clock_hz = display_getdotclock(vdmode);
	timing.h_front_porch = vdmode->hfp;
	timing.h_back_porch = vdmode->hbp;
	timing.h_sync_len =  vdmode->hsync;
	timing.v_front_porch = vdmode->vfp;
	timing.v_back_porch = vdmode->vbp;
	timing.v_sync_len = vdmode->vsync;
	timing.h_sync_active = vdmode->vsyncneg;
	timing.v_sync_active = vdmode->hsyncneg;
	timing.den_active = ! vdmode->deneg;
	timing.clk_active = 0;

	//pdat->backlight = NULL;
	//struct t113_tconlcd_reg_t * const tcon = (struct t113_tconlcd_reg_t *) TCON_LCD0_BASE;
	int vbp, vtotal;
	int hbp, htotal;
	uint32_t val;

	// ctrl
	val = (timing.v_front_porch + timing.v_back_porch + timing.v_sync_len) / 2;
//	write32((uintptr_t) & tcon->ctrl,
//			(1u << 31) |
//			(0x00u << 24) |
//			(0x00u << 23) |
//			((val & 0x1f) << 4) |
//			(0x00u << 0)
//			);
	TCON_LCD0->LCD_CTL_REG = (
		(1u << 31) |		// LCD_EN
		(0x00u << 24) |		// LCD_IF 0x00: HV (Sync+DE), 01: 8080 I/F
		(0x00u << 23) |		// LCD_RB_SWAP
		((val & 0x1f) << 4) |	// LCD_START_DLY
		(0x00u << 0) |			// LCD_SRC_SEL: 000: DE, 1..7 - tests
		0
		);

	// dclk
	// 31..28: TCON0_Dclk_En
	// 6..0: TCON0_Dclk_Div
	val = allwnrt113_get_video0_x2_freq() / display_getdotclock(vdmode);
//	write32((uintptr_t) & tcon->dclk,
//			(0x0Fu << 28) | (val << 0));
	TCON_LCD0->LCD_DCLK_REG = (
			(0x0Fu << 28) |		// LCD_DCLK_EN
			(val << 0)			// LCD_DCLK_DIV
			);

	// timing0 (window)
//	write32((uintptr_t) & tcon->timing0,
//			((pdat->width - 1) << 16) | ((pdat->height - 1) << 0)
//			);
	TCON_LCD0->LCD_BASIC0_REG = (
		((pdat->width - 1) << 16) | ((pdat->height - 1) << 0)
		);

	// timing1 (horizontal)
	hbp = timing.h_sync_len + timing.h_back_porch;
	htotal = pdat->width + timing.h_front_porch + hbp;
//	write32((uintptr_t) & tcon->timing1,
//			((htotal - 1) << 16) | ((hbp - 1) << 0)
//			);
	TCON_LCD0->LCD_BASIC1_REG = (
		((htotal - 1) << 16) | ((hbp - 1) << 0)
		);

	// timing2 (vertical)
	vbp = timing.v_sync_len + timing.v_back_porch;
	vtotal = pdat->height + timing.v_front_porch + vbp;
//	write32((uintptr_t) & tcon->timing2,
//			((vtotal * 2) << 16) | ((vbp - 1) << 0)
//			);
	TCON_LCD0->LCD_BASIC2_REG = (
		((vtotal * 2) << 16) | ((vbp - 1) << 0)
		);

	// timing3
//	write32((uintptr_t) & tcon->timing3,
//			((timing.h_sync_len - 1) << 16) |
//			((timing.v_sync_len - 1) << 0)
//			);
	TCON_LCD0->LCD_BASIC3_REG = (
		((timing.h_sync_len - 1) << 16) |
		((timing.v_sync_len - 1) << 0)
		);

	// Sochip_VE_S3_Datasheet_V1.0.pdf
	// 7.2.5.19. TCON0_IO_POL_REG
	// io_polarity
	val = (0x00u << 31) | 	// IO_Output_Sel: 0: nirmal, 1: sync to dclk
			(1u << 28);	// DCLK_Sel: 0x00: DCLK0 (normal phase offset), 0x01: DCLK1(1/3 phase offset

	if(!timing.h_sync_active)
		val |= (1u << 25);	// IO1_Inv
	if(!timing.v_sync_active)
		val |= (1u << 24);	// IO0_Inv
	if(!timing.den_active)
		val |= (1u << 27);	// IO3_Inv
	if(!timing.clk_active)
		val |= (1u << 26);	// IO2_Inv
	//write32((uintptr_t) & tcon->io_polarity, val);
	TCON_LCD0->LCD_IO_POL_REG = val;

	// io_tristate
	//write32((uintptr_t) & tcon->io_tristate, 0);
	TCON_LCD0->LCD_IO_TRI_REG = 0;

	// 5.1.6.14 0x0058 LCD HV Panel Interface Register (Default Value: 0x0000_0000)
	/// LCD_HV_IF_REG
	TCON_LCD0->LCD_HV_IF_REG = 0;
	TCON_LCD0->LCD_CPU_IF_REG = 0;
	TCON_LCD0->LCD_CPU_WR_REG = 0;
	TCON_LCD0->LCD_LVDS_IF_REG =
		0 * (1u << 31) |		// LCD_LVDS_EN
		0 * (1u << 25) |		// LCD_LVDS_DEBUG_EN
		0 * (1u << 24) |		// LCD_LVDS_DEBUG_MODE
		1 * (1u << 3) |		// LCD_LVDS_CLK_POL
		0x0F * (1u << 0) |		// LCD_LVDS_DATA_POL
		0;
}

#if 0

static void t113_tconlcd_set_dither(struct fb_t113_rgb_pdata_t * pdat)
{
	struct t113_tconlcd_reg_t * tcon = (struct t113_tconlcd_reg_t *)TCON_LCD0_BASE;

	if((pdat->bits_per_pixel == 16) || (pdat->bits_per_pixel == 18))
	{
		write32((uintptr_t) & tcon->frm_seed[0], 0x11111111);
		write32((uintptr_t) & tcon->frm_seed[1], 0x11111111);
		write32((uintptr_t) & tcon->frm_seed[2], 0x11111111);
		write32((uintptr_t) & tcon->frm_seed[3], 0x11111111);
		write32((uintptr_t) & tcon->frm_seed[4], 0x11111111);
		write32((uintptr_t) & tcon->frm_seed[5], 0x11111111);
		write32((uintptr_t) & tcon->frm_table[0], 0x01010000);
		write32((uintptr_t) & tcon->frm_table[1], 0x15151111);
		write32((uintptr_t) & tcon->frm_table[2], 0x57575555);
		write32((uintptr_t) & tcon->frm_table[3], 0x7f7f7777);

		// Sochip_VE_S3_Datasheet_V1.0.pdf
		// TCON0_TRM_CTL_REG offset 0x0010
		// User manual:
		// LCD FRM Control Register (Default Value: 0x0000_0000)
		// 31: TCON_FRM_EN: 0: disable, 1: enable
		// 6: TCON_FRM_MODE_R: 0 - 6 bit, 1: 5 bit
		// 5: TCON_FRM_MODE_G: 0 - 6 bit, 1: 5 bit
		// 4: TCON_FRM_MODE_B: 0 - 6 bit, 1: 5 bit
		write32((uintptr_t) & tcon->frm_ctrl, TCON_FRM_MODE_VAL);
		/* режим и формат выхода */
		TCON_LCD0->LCD_FRM_CTL_REG = TCON_FRM_MODE_VAL;
	}
}

#endif


static struct fb_t113_rgb_pdata_t pdat0;

void hardware_ltdc_initialize(const uintptr_t * frames, const videomode_t * vdmode)
{
	struct fb_t113_rgb_pdata_t * const pdat = & pdat0;
	uint32_t val;

//	pdat->virt_tconlcd = T113_TCONLCD_BASE;
//    pdat->virt_de = T113_DE_BASE;
	//pdat->clk_tconlcd = (void *) allwnrt113_get_video0_x2_freq();
    //pdat->clk_de = (void *) 396000000;
	pdat->width = vdmode->width;
	pdat->height =  vdmode->height;
	//pdat->pwidth =  216;
	//pdat->pheight = 135;
	//pdat->bits_per_pixel = 18; // panel connection type
	//pdat->bytes_per_pixel = LCDMODE_PIXELSIZE;
	//pdat->pixlen = pdat->width * pdat->height * pdat->bytes_per_pixel;
//	pdat->vram [0] = frames [0];
//	pdat->vram [1] = frames [1];
//	pdat->index = 0;

    CCU->DE_CLK_REG |= (1u << 31) | (0x03u << 0);	// 300 MHz

    CCU->DE_BGR_REG |= (1u << 0);		// Open the clock gate
    CCU->DE_BGR_REG |= (1u << 16);		// Deassert reset

    CCU->TCONLCD_CLK_REG |= (1u << 31);
    CCU->TCONLCD_BGR_REG |= (1u << 0);	// Open the clock gate
    CCU->TCONLCD_BGR_REG |= (1u << 16); // Deassert reset

#if 0
    // step 2
    CCU->LVDS_BGR_REG |= (1u << 16); // LVDS0_RST: Deassert reset

    // step 5
    TCON_LCD0->LCD_LVDS_IF_REG =
		0;
    TCON_LCD0->LCD_DCLK_REG =
		(1u << 28) |	// LCD_DCLK_EN
		(0x07u << 0) |	// LCD_DCLK_DIV
		0;
#endif

	t113_tconlcd_disable(pdat);
	t113_tconlcd_set_timing(pdat, vdmode);
	//t113_tconlcd_set_dither(pdat);
	{
		// Sochip_VE_S3_Datasheet_V1.0.pdf
		// TCON0_TRM_CTL_REG offset 0x0010
		// User manual:
		// LCD FRM Control Register (Default Value: 0x0000_0000)
		// 31: TCON_FRM_EN: 0: disable, 1: enable
		// 6: TCON_FRM_MODE_R: 0 - 6 bit, 1: 5 bit
		// 5: TCON_FRM_MODE_G: 0 - 6 bit, 1: 5 bit
		// 4: TCON_FRM_MODE_B: 0 - 6 bit, 1: 5 bit
		/* режим и формат выхода */
		TCON_LCD0->LCD_FRM_CTL_REG = TCON_FRM_MODE_VAL;

	}
	t113_tconlcd_enable(pdat);

	t113_de_set_mode(pdat);
	t113_de_enable(pdat);

	// Set DE MODE if need
	ltdc_tfcon_cfg(vdmode);
}

/* Set MAIN frame buffer address. No waiting for VSYNC. */
/* Вызывается из display_flush, используется только в тестах */
void hardware_ltdc_main_set_no_vsync(uintptr_t p1)
{
	struct fb_t113_rgb_pdata_t * const pdat = & pdat0;
	struct de_bld_t * const bld = (struct de_bld_t *) DE_BLD_BASE;

	t113_de_set_address_vi(pdat, p1);
	// 5.10.9.1 BLD fill color control register
	// BLD_FILL_COLOR_CTL
	write32((uintptr_t) & bld->fcolor_ctl,
			((p1 != 0) << 8)	| // pipe0 enable RED - from VI
//			((p2 != 0) << 9)	| // pipe1 enable GREEN - from UI1
//			((p3 != 0) << 10)	| // pipe2 enable - no display (t113-s3 not have hardware)
//			((p4 != 0) << 11)	| // pipe3 enable - no display (t113-s3 not have hardware)
			0
			);

	t113_de_enable(pdat);
}

/* Set MAIN frame buffer address. Waiting for VSYNC. */
void hardware_ltdc_main_set4(uintptr_t layer0, uintptr_t layer1, uintptr_t layer2, uintptr_t layer3)
{
	struct fb_t113_rgb_pdata_t * const pdat = & pdat0;
	struct de_bld_t * const bld = (struct de_bld_t *) DE_BLD_BASE;

	// Note: the layer priority is layer3>layer2>layer1>layer0
	t113_de_set_address_vi(pdat, layer0);		// VI
	t113_de_set_address_ui(pdat, layer1, 1);	// UI1
	t113_de_set_address_ui(pdat, layer2, 2);	// UI2
	t113_de_set_address_ui(pdat, layer3, 3);	// UI3

	// 5.10.9.1 BLD fill color control register
	// BLD_FILL_COLOR_CTL
	write32((uintptr_t) & bld->fcolor_ctl,
			((layer0 != 0) << 8)	| // pipe0 enable - from VI
			((layer1 != 0) << 9)	| // pipe1 enable - from UI1
			((layer2 != 0) << 10)	| // pipe2 enable - no display (t113-s3 not have hardware)
			((layer3 != 0) << 11)	| // pipe3 enable - no display (t113-s3 not have hardware)
			0
			);

	hardware_ltdc_vsync();	/* ожидаем начало кадра */
	t113_de_enable(pdat);
}

/* ожидаем начало кадра */
static void hardware_ltdc_vsync(void)
{

	//TCON_LCD0->LCD_GINT0_REG |= (1u << 31); 		//Enable the Vertical Blank interrupt
	TCON_LCD0->LCD_GINT0_REG &= ~ (1u << 15);         //clear LCD_VB_INT_FLAG
	while((TCON_LCD0->LCD_GINT0_REG & (1u << 15)) == 0) //wait  LCD_VB_INT_FLAG
		hardware_nonguiyield();
}

/* set visible buffer start. Wait VSYNC. */
void hardware_ltdc_main_set(uintptr_t p1)
{
	struct fb_t113_rgb_pdata_t * const pdat = & pdat0;
	struct de_bld_t * const bld = (struct de_bld_t *) DE_BLD_BASE;

	t113_de_set_address_vi(pdat, p1);
	// 5.10.9.1 BLD fill color control register
	// BLD_FILL_COLOR_CTL
	write32((uintptr_t) & bld->fcolor_ctl,
			((p1 != 0) << 8)	| // pipe0 enable RED - from VI
//			((p2 != 0) << 9)	| // pipe1 enable GREEN - from UI1
//			((p3 != 0) << 10)	| // pipe2 enable - no display (t113-s3 not have hardware)
//			((p4 != 0) << 11)	| // pipe3 enable - no display (t113-s3 not have hardware)
			0
			);

	hardware_ltdc_vsync();	/* ожидаем начало кадра */
	t113_de_enable(pdat);
}

/* Palette reload */
void hardware_ltdc_L8_palette(void)
{
}

#else
	//#error Wrong CPUSTYLE_xxxx

void hardware_ltdc_initialize(const uintptr_t * frames, const videomode_t * vdmode)
{
}

/* Set MAIN frame buffer address. No waiting for VSYNC. */
/* Вызывается из display_flush, используется только в тестах */
void hardware_ltdc_main_set_no_vsync(uintptr_t p)
{
}

/* set visible buffer start. Wait VSYNC. */
void hardware_ltdc_main_set(uintptr_t p)
{
}

/* Set MAIN frame buffer address. Waiting for VSYNC. */
void hardware_ltdc_main_set4(uintptr_t layer0, uintptr_t layer1, uintptr_t layer2, uintptr_t layer3)
{
}

/* ожидаем начало кадра */
static void hardware_ltdc_vsync(void)
{
}

/* Palette reload */
void hardware_ltdc_L8_palette(void)
{
}

#endif /* CPUSTYLE_STM32F || CPUSTYLE_STM32MP1 */

/* Получить желаемую частоту pixel clock для данного видеорежима. */
unsigned long display_getdotclock(const videomode_t * vdmode)
{
	/* Accumulated parameters for this display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned HSYNC = vdmode->hsync;	/*  */
	const unsigned VSYNC = vdmode->vsync;	/*  */
	const unsigned LEFTMARGIN = HSYNC + vdmode->hbp;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = VSYNC + vdmode->vbp;	/* vertical delay before DE start */
	const unsigned HFULL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VFULL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

	return (unsigned long) vdmode->fps * HFULL * VFULL;
	//return vdmode->ltdc_dotclk;
}

#else /* WITHLTDCHW */

unsigned long display_getdotclock(const videomode_t * vdmode)
{
	return 1000000uL;
}

#endif /* WITHLTDCHW */

#if WITHGPUHW

void GPU_IRQHandler(void)
{
	PRINTF("GPU_IRQHandler\n");
}

// Graphic processor unit
void board_gpu_initialize(void)
{
	PRINTF("board_gpu_initialize start.\n");

	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_GPUEN;
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6LPENSETR = RCC_MP_AHB6LPENSETR_GPULPEN;
	(void) RCC->MP_AHB6LPENSETR;

	PRINTF("board_gpu_initialize: PRODUCTID=%08lX\n", (unsigned long) GPU->PRODUCTID);

//
	arm_hardware_set_handler_system(GPU_IRQn, GPU_IRQHandler);

	PRINTF("board_gpu_initialize done.\n");
}

#endif /* WITHGPUHW */
