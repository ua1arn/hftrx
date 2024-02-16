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
#include "formats.h"	// for debug prints

#if WITHLTDCHW

#include "display.h"
#include <stdint.h>
#include <string.h>
#include <limits.h>

#include "gpio.h"
#include "src/touch/touch.h"

#define WITHLVDSHW (WITHFLATLINK && defined (HARDWARE_LVDS_INITIALIZE))
#define WITHDSIHW (WITHMIPIDSISHW && defined (HARDWARE_LVDS_INITIALIZE))
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
#if defined (HARDWARE_MIPIDSI_INITIALIZE) && WITHMIPIDSISHW
		HARDWARE_MIPIDSI_INITIALIZE();
#elif defined (HARDWARE_LVDS_INITIALIZE) && WITHLVDSHW
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
		mask = (mask << 1) | UINT32_C(1); \
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
		mask = (mask << 1) | UINT32_C(1); \
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
	const unsigned HTOTAL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VTOTAL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

	//SETREG32_CK(& vdc->SC0_SCL0_FRC1, 16, 16, 0);	// SC0_RES_VMASK
	SETREG32_CK(& vdc->SC0_SCL0_FRC1, 1, 0, 0);		// SC0_RES_VMASK_ON 0: Repeated Vsync signal masking control is disabled.
	//SETREG32_CK(& vdc->SC0_SCL0_FRC2, 16, 16, 0);	// SC0_RES_VLACK
	SETREG32_CK(& vdc->SC0_SCL0_FRC2, 1, 0, 0);		// SC0_RES_VLACK_ON	0: Compensation of missing Vsync signals is disabled.

	SETREG32_CK(& vdc->SC0_SCL0_FRC5, 1, 8, 1);		// SC0_RES_FLD_DLY_SEL
	SETREG32_CK(& vdc->SC0_SCL0_FRC5, 8, 0, 1);		// SC0_RES_VSDLY

	SETREG32_CK(& vdc->SC0_SCL0_FRC4, 11, 16, VTOTAL - 1);// SC0_RES_FV Free-Running Vsync Period Setting
	SETREG32_CK(& vdc->SC0_SCL0_FRC4, 11, 0, HTOTAL - 1);	// SC0_RES_FH Hsync Period Setting

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
	const unsigned HTOTAL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VTOTAL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

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
	const unsigned HTOTAL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VTOTAL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

	const unsigned MAINROWSIZE = sizeof (PACKEDCOLORPIP_T) * GXADJ(DIM_SECOND);	// размер одной строки в байтах
	// Таблица используемой при отображении палитры
	COLOR24_T xltrgb24 [256];
	display2_xltrgb24(xltrgb24);

#if LCDMODE_MAIN_L8
	const unsigned grx_format_MAIN = 0x05;	// GRx_FORMAT 5: CLUT8
	const unsigned grx_rdswa_MAIN = 0x07;	// GRx_RDSWA 111: (8) (7) (6) (5) (4) (3) (2) (1) [32-bit swap + 16-bit swap + 8-bit swap]
#elif LCDMODE_MAIN_ARGB8888
	const unsigned grx_format_MAIN = 0x04;	// GRx_FORMAT 4: ARGB8888, 1: RGB888
	const unsigned grx_rdswa_MAIN = 0x04;	// GRx_RDSWA 100: (5) (6) (7) (8) (1) (2) (3) (4) [Swapped in 32-bit units]
#else /* LCDMODE_MAIN_L8 */
	const unsigned grx_format_MAIN = 0x00;	// GRx_FORMAT 0: RGB565
	const unsigned grx_rdswa_MAIN = 0x06;	// GRx_RDSWA 110: (7) (8) (5) (6) (3) (4) (1) (2) [32-bit swap + 16-bit swap]
#endif /* LCDMODE_MAIN_L8 */

	// LCDMODE_PIP_RGB565
//	const unsigned grx_format_PIP = 0x00;	// GRx_FORMAT 0: RGB565
//	const unsigned grx_rdswa_PIP = 0x06;	// GRx_RDSWA 110: (7) (8) (5) (6) (3) (4) (1) (2) [32-bit swap + 16-bit swap]

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
	//vdc->GR2_CLUT ^= (UINT32_C(1) << 16);	// GR2_CLT_SEL Switch to filled table
//#endif /* LCDMODE_MAIN_L8 */

	// PIP on GR3
	SETREG32_CK(& vdc->GR3_CLUT_INT, 1, 16, 0x00);			// GR3_CLT_SEL
	VDC5_fillLUT_L8(& VDC5_CH0_GR3_CLUT_TBL, xltrgb24);
	SETREG32_CK(& vdc->GR3_CLUT_INT, 1, 16, 0x01);			// GR3_CLT_SEL

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
	SETREG32_CK(& vdc->GR3_FLM6, 4, 28, grx_format_MAIN);	// GR3_FORMAT 0: RGB565 or 5: CLUT8
	SETREG32_CK(& vdc->GR3_FLM6, 3, 10, grx_rdswa_MAIN);	// GR3_RDSWA 110: (7) (8) (5) (6) (3) (4) (1) (2) [32-bit swap + 16-bit swap]
	SETREG32_CK(& vdc->GR3_AB1, 2, 0,	0x01);			// GR3_DISP_SEL 1: Lower-layer graphics display
	SETREG32_CK(& vdc->GR3_BASE, 24, 0, 0x000000FF);	// RED GR3_BASE GBR Background Color B,Gb & R Signal
	SETREG32_CK(& vdc->GR3_AB2, 11, 16, TOPMARGIN);	// GR3_GRC_VS
	SETREG32_CK(& vdc->GR3_AB2, 11, 0, HEIGHT);		// GR3_GRC_VW
	SETREG32_CK(& vdc->GR3_AB3, 11, 16, LEFTMARGIN);	// GR3_GRC_HS
	SETREG32_CK(& vdc->GR3_AB3, 11, 0, WIDTH);			// GR3_GRC_HW

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
#endif /* */
#if 0
	// PIP on GR3
	SETREG32_CK(& vdc->GR3_CLUT_INT, 1, 16, 0x00);			// GR3_CLT_SEL
	VDC5_fillLUT_L8(& VDC5_CH0_GR3_CLUT_TBL, xltrgb24);
	SETREG32_CK(& vdc->GR3_CLUT_INT, 1, 16, 0x01);			// GR3_CLT_SEL
#endif /* */
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
	const unsigned HTOTAL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VTOTAL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

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
	SETREG32_CK(& vdc->TCON_TIM, 11, 16, HTOTAL);		// TCON_HALF
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
	const unsigned HTOTAL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VTOTAL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

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
	LTDC_InitStruct.LTDC_TotalWidth = (HTOTAL - 1);

	/* Configure vertical synchronization height */
	LTDC_InitStruct.LTDC_VerticalSync = (vdmode->vsync - 1);
	/* Configure accumulated vertical back porch */
	LTDC_InitStruct.LTDC_AccumulatedVBP = (TOPMARGIN - 1);
	/* Configure accumulated active height */
	LTDC_InitStruct.LTDC_AccumulatedActiveH = (TOPMARGIN + HEIGHT - 1);
	/* Configure total height */
	LTDC_InitStruct.LTDC_TotalHeigh = (VTOTAL - 1);

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

#elif LCDMODE_MAIN_ARGB8888

	/* Без палитры */
	LCDx_LayerInit(LAYER_MAIN, LEFTMARGIN, TOPMARGIN, & mainwnd, LTDC_Pixelformat_ARGB8888, 1, sizeof (PACKEDCOLORPIP_T));

#else
	/* Без палитры */
	LCDx_LayerInit(LAYER_MAIN, LEFTMARGIN, TOPMARGIN, & mainwnd, LTDC_Pixelformat_RGB565, 1, sizeof (PACKEDCOLORPIP_T));

#endif /* LCDMODE_MAIN_L8 */

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
	fillLUT_L24(LAYER_MAIN);	// прямая трансляция всех байтов из памяти на выход. загрузка палитры - имеет смысл до Reload
#elif LCDMODE_MAIN_L8
	fillLUT_L8(LAYER_MAIN, xltrgb24);	// загрузка палитры - имеет смысл до Reload
#endif /* LCDMODE_MAIN_L8 */

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
#endif /* */
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

	LTDC->SRCR |= LTDC_SRCR_VBR_Msk;	/* Vertical Blanking Reload. */
	(void) LTDC->SRCR;

	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		hardware_nonguiyield();
}

/* Set MAIN frame buffer address. Wait for VSYNC. */
void hardware_ltdc_main_set(uintptr_t p)
{
	LAYER_MAIN->CFBAR = p;

	(void) LAYER_MAIN->CFBAR;
	LAYER_MAIN->CR |= LTDC_LxCR_LEN_Msk;
	(void) LAYER_MAIN->CR;

	hardware_ltdc_vsync();
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
	linux_framebuffer_init();
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

#elif CPUSTYLE_ALLWINNER


#if CPUSTYLE_T113 || CPUSTYLE_F133

// RT-MIXER base
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

#elif (CPUSTYLE_T507 || CPUSTYLE_H616)

	// https://github.com/RMerl/asuswrt-merlin.ng/blob/master/release/src-rt-5.04axhnd.675x/bootloaders/u-boot-2019.07/arch/arm/include/asm/arch-sunxi/display2.h#L16
	// struct de_clk
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

	# define SUNXI_DE2_MUX_GLB_REGS			0x00000
	# define SUNXI_DE2_MUX_BLD_REGS			0x01000
	# define SUNXI_DE2_MUX_CHAN_REGS		0x02000
	//# define SUNXI_DE2_MUX_CHAN_SZ			0x1000
	# define SUNXI_DE2_MUX_VSU_REGS			0x20000
	# define SUNXI_DE2_MUX_GSU1_REGS		0x30000
	# define SUNXI_DE2_MUX_GSU2_REGS		0x40000
	# define SUNXI_DE2_MUX_GSU3_REGS		0x50000
	# define SUNXI_DE2_MUX_FCE_REGS			0xa0000
	# define SUNXI_DE2_MUX_BWS_REGS			0xa2000
	# define SUNXI_DE2_MUX_LTI_REGS			0xa4000
	# define SUNXI_DE2_MUX_PEAK_REGS		0xa6000
	# define SUNXI_DE2_MUX_ASE_REGS			0xa8000
	# define SUNXI_DE2_MUX_FCC_REGS			0xaa000
	# define SUNXI_DE2_MUX_DCSC_REGS		0xb0000

#else
	//#error Undefined CPUSTYLE_xxx
#endif
// 1.5 Register Description
//struct de_clk_t {
//	uint32_t gate_cfg;		// SCLK_GATE DE SCLK Gating Register
//	uint32_t bus_cfg;		// ? HCLK_GATE ? DE HCLK Gating Register
//	uint32_t rst_cfg;		// AHB_RESET DE AHB Reset register
//	uint32_t div_cfg;		// SCLK_DIV DE SCLK Division register
//	uint32_t sel_cfg;		// ? DE2TCON ? MUX register
//};

// Allwinner_DE2.0_Spec_V1.0
// 5.10.3.4 Blender
// GLB
//struct de_glb_t {
//	uint32_t ctl;		/** Offset 0x000 Global control register */
//	uint32_t status;	/** Offset 0x004 Global status register */
//	uint32_t dbuff;		/** Offset 0x008 Global double buffer control register */
//	uint32_t size;		/** Offset 0x00C Global size register */
//};

// Allwinner_DE2.0_Spec_V1.0
// 5.10.3.4 Blender
// BLD
//struct de_bld_t {
//	uint32_t fcolor_ctl;	/** BLD_FILL_COLOR_CTL Offset 0x000 BLD fill color control register */
//	struct {
//		uint32_t fcolor;	/**  BLD fill color register */
//		uint32_t insize;	/**  BLD input memory size register */
//		uint32_t offset;	/**  BLD input memory offset register */
//		uint32_t dum;		/**  filler */
//	} attr[4];
//	uint32_t dum0[15];
//	uint32_t route;			/** BLD_CH_RTCTL Offset 0x080 BLD routing control register */
//	uint32_t premultiply;	/** Offset 0x080 BLD pre-multiply control register */
//	uint32_t bkcolor;
//	uint32_t output_size;
//	uint32_t bld_mode[4];	/* BLD_CTL */
//	uint32_t dum1[4];
//	uint32_t ck_ctl;
//	uint32_t ck_cfg;
//	uint32_t dum2[2];
//	uint32_t ck_max[4];
//	uint32_t dum3[4];
//	uint32_t ck_min[4];
//	uint32_t dum4[3];
//	uint32_t out_ctl;
//};

//struct de_vi_t {
//	struct {
//		uint32_t attr;
//		uint32_t size;
//		uint32_t coord;
//		uint32_t pitch[3];
//		uint32_t top_laddr[3];
//		uint32_t bot_laddr[3];
//	} cfg [4];
//	uint32_t fcolor[4];
//	uint32_t top_haddr[3];
//	uint32_t bot_haddr[3];
//	uint32_t ovl_size[2];
//	uint32_t hori[2];
//	uint32_t vert[2];
//};

// 5.10.3.4 Blender
// part
//struct de_ui_t {
//	struct {
//		uint32_t attr;
//		uint32_t size;
//		uint32_t coord;
//		uint32_t pitch;
//		uint32_t top_laddr;
//		uint32_t bot_laddr;
//		uint32_t fcolor;
//		uint32_t dum;
//	} cfg [4];
//	uint32_t top_haddr;
//	uint32_t bot_haddr;
//	uint32_t ovl_size;
//};

#define UI_CFG_INDEX 0	/* 0..3 используется одна конфигурация */
#define VI_CFG_INDEX 0

#define RTMIXIX 1

#if CPUSTYLE_T113 || CPUSTYLE_F133
	#define VI_LASTIX 1
	#define UI_LASTIX 1
	/* BLD_EN_COLOR_CTL positions 8..11 */
	#define VI_POS_BIT(vi) (1u << ((vi) + 8 - 1))
	#define UI_POS_BIT(ui) (1u << ((ui) + (8 + VI_LASTIX) - 1))
#elif CPUSTYLE_T507 || CPUSTYLE_H616 || CPUSTYLE_A64
	#define VI_LASTIX 3
	#define UI_LASTIX 3
	/* BLD_EN_COLOR_CTL positions 8..13 */
	#define VI_POS_BIT(vi) (1u << ((vi) + 8 - 1))
	#define UI_POS_BIT(ui) (1u << ((ui) + (8 + VI_LASTIX) - 1))
#endif

static DE_GLB_TypeDef * de3_getglb(int rtmixix)
{
#ifdef DEb_GLB
	return rtmixix == 1 ? DE_GLB : DEb_GLB;
#else
	return rtmixix == 1 ? DE_GLB : NULL;
#endif
}

static DE_VI_TypeDef * de3_getvi(int rtmixix, int ix)
{
	switch (ix)
	{
	default: return NULL;
#ifdef DEb_VI1
	case 1: return rtmixix == 1 ? DE_VI1 : DEb_VI1;
#else
	case 1: return rtmixix == 1 ? DE_VI1 : NULL;
#endif
#if VI_LASTIX > 1
	case 2: return DE_VI2;
	case 3: return DE_VI3;
#endif
	}

}

static DE_UI_TypeDef * de3_getui(int rtmixix, int ix)
{
	switch (ix)
	{
	default: return NULL;
	case 1: return rtmixix == 1 ? DE_UI1 : NULL;
#if UI_LASTIX > 1
	case 2: return DE_UI2;
	case 3: return DE_UI3;
#endif
	}

}

static DE_BLD_TypeDef * de3_getbld(int ix)
{
#if CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64

	switch (ix)
	{
	default: return NULL;
	case 1: return DE_BLD1;
	case 2: return DE_BLD2;
	}

#else
	switch (ix)
	{
	default: return NULL;
	case 1: return DE_BLD1;
	case 2: return DE_BLD2;
	}
#endif
}

//static void write32(uintptr_t a, uint32_t v)
//{
//	* (volatile uint32_t *) a = v;
//}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshift-count-overflow"
static uint32_t ptr_hi32(uintptr_t v)
{
	if (sizeof v == sizeof (uint32_t))
		return 0;
	return v >> 32;
}

static uint32_t ptr_lo32(uintptr_t v)
{
	return (uint32_t) v;
}
#pragma GCC diagnostic pop

#if LCDMODE_MAIN_ARGB8888
	static const uint32_t ui_format = 0x00;	//  0x00: ARGB_8888
	//const uint32_t ui_format = 0x04;	// 0x04: XRGB_8888
#elif LCDMODE_MAIN_RGB565
	static const uint32_t ui_format = 0x0A;	// 0x0A: RGB_565
#else
	#error Unsupported framebuffer format. Looks like you need remove WITHLTDCHW
	static const uint32_t ui_format = 0x0A;
#endif

static void t113_de_update(void)
{
	DE_GLB_TypeDef * const glb = de3_getglb(RTMIXIX);
	if (glb == NULL)
		return;
	glb->GLB_DBUFFER = UINT32_C(1);		// 1: register value be ready for update (self-cleaning bit)
	while ((glb->GLB_DBUFFER & UINT32_C(1)) != 0)
		;
}

/* VI (VI0) */
static void t113_de_set_address_vi(uintptr_t vram, int vich)
{
	DE_VI_TypeDef * const vi = de3_getvi(RTMIXIX, vich);

	if (vi == NULL)
		return;

	if (vram == 0)
	{
		vi->CFG [VI_CFG_INDEX].ATTR = 0;
		return;
	}
	const uint_fast32_t attr =
		((vram != 0) << 0) |	// enable
#if 0
		(UINT32_C(255) << 24) | // LAY_GLBALPHA
		(UINT32_C(1) << 1) | 	// LAY_ALPHA _MODE: 0x1:Globe alpha enable
#endif
		(ui_format << 8) |		// нижний слой: 32 bit ABGR 8:8:8:8 без пиксельной альфы
		(UINT32_C(1) << 15) |	// Video_UI_SEL 0: Video Overlay(using Video Overlay Layer Input data format) 1: UI Overlay(using UI Overlay Layer Input data format)
		//(UINT32_C(1) << 4) |	// LAY_FILLCOLOR_EN - замещает данные, идущие по DMA
		0;

	vi->CFG [VI_CFG_INDEX].TOP_LADDR [0] = ptr_lo32(vram);	// The setting of this register is U/UV channel address.
	vi->TOP_HADDR [0] = (ptr_hi32(vram) & 0xFF) << VI_CFG_INDEX;						// The setting of this register is U/UV channel address.
	vi->CFG [VI_CFG_INDEX].ATTR = attr;

	ASSERT(vi->CFG [VI_CFG_INDEX].TOP_LADDR [0] == ptr_lo32(vram));
	ASSERT(vi->CFG [VI_CFG_INDEX].ATTR == attr);

}

static inline void t113_de_set_address_ui(uintptr_t vram, int uich)
{
	DE_UI_TypeDef * const ui = de3_getui(RTMIXIX, uich);

	if (ui == NULL)
		return;

	if (vram == 0)
	{
		ui->CFG [UI_CFG_INDEX].ATTR = 0;
		return;
	}
	const uint_fast32_t attr =
		((vram != 0) << 0) |	// enable
		(ui_format << 8) | 		//верхний слой: 32 bit ABGR 8:8:8:8 с пиксельной альфой
#if 0
		(UINT32_C(255) << 24) | // LAY_GLBALPHA
		(UINT32_C(1) << 1) | 	// LAY_ALPHA _MODE: 0x1:Globe alpha enable
#endif
		0;

	ui->CFG [UI_CFG_INDEX].TOP_LADDR = ptr_lo32(vram);
	ui->TOP_HADDR = (0xFF & ptr_hi32(vram)) << (8 * UI_CFG_INDEX);
	ui->CFG [UI_CFG_INDEX].ATTR = attr;

	ASSERT(ui->CFG [UI_CFG_INDEX].ATTR == attr);
}

static inline void t113_de_set_mode(const videomode_t * vdmode, int ix, unsigned color)
{
	DE_BLD_TypeDef * const bld = de3_getbld(ix);
	if (bld == NULL)
		return;
	// Allwinner_DE2.0_Spec_V1.0.pdf
	// 5.10.8.2 OVL_UI memory block size register
	// 28..16: LAY_HEIGHT
	// 12..0: LAY_WIDTH
	const uint32_t ovl_ui_mbsize = (((vdmode->height - 1) << 16) | (vdmode->width - 1));
	const uint32_t uipitch = LCDMODE_PIXELSIZE * GXADJ(vdmode->width);

	int i;

	/* DE submodules */

	DE_GLB_TypeDef * const glb = de3_getglb(RTMIXIX);
	if (glb == NULL)
		return;

	glb->GLB_SIZE = ovl_ui_mbsize;
	ASSERT(glb->GLB_SIZE == ovl_ui_mbsize);

	// 5.10.9.1 BLD fill color control register
	// BLD_FILL_COLOR_CTL
	bld->BLD_EN_COLOR_CTL = 0;

	// 5.10.9.5 BLD routing control register
	// BLD_CH_RTCTL
	// 0x03020100 - default state

//	DE_BLD->ROUTE =
//			(UINT32_C(0) << 0) |		// pipe 0 from ch 0
//			(UINT32_C(1) << 4) |		// pipe 1 from ch 1
//			(UINT32_C(2) << 8) |		// pipe 2 from ch 2
//			(UINT32_C(3) << 12) |		// pipe 3 from ch 3
//			0;
	bld->PREMULTIPLY = 0;
	bld->BKCOLOR = color; /* 24 bit. Отображается, когда нет данных от входного pipe */

//	PRINTF("2 bld->ROUTE=%08" PRIX32 "\n", bld->ROUTE);
//	PRINTF("2 bld->BKCOLOR=%08" PRIX32 "\n", bld->BKCOLOR);
	//printhex32(DE_BLD_BASE, DE_BLD, 256);
	// 5.10.9.1 BLD fill color control register
	// BLD_CTL
	// в примерах только 0 и 1 индексы
//	for(i = 0; i < ARRAY_SIZE(bld->BLD_MODE); i++)
//	{
//	    unsigned bld_mode = 0x03010301;		// default
//		//PRINTF("DE_BLD->BLD_MODE [%d]=%08X\n", i, (unsigned) DE_BLD->BLD_MODE [i]);
////	    unsigned bld_mode = 0x03020302;           //Fs=Ad, Fd=1-As, Qs=Ad, Qd=1-As
//	    //bld->BLD_MODE [i] = bld_mode;
//		ASSERT(bld->BLD_MODE [i] == bld_mode);
//	}

	bld->OUTPUT_SIZE = ovl_ui_mbsize;
	bld->OUT_CTL = 0;	// ITLMOD_EN=0, PREMUL_EN=0
	bld->CK_CTL = 0;
	for (i = 0; i < ARRAY_SIZE(bld->CH); i++)
	{
		bld->CH [i].BLD_FILL_COLOR = 0x00000000; //0*0xff000000;
		bld->CH [i].BLD_CH_ISIZE = ovl_ui_mbsize;
		bld->CH [i].BLD_CH_OFFSET = 0;
		ASSERT(bld->CH [i].BLD_CH_ISIZE == ovl_ui_mbsize);
	}

	int vich = 1;
	for (vich = 1; vich <= VI_LASTIX; vich ++)
	{
		DE_VI_TypeDef * const vi = de3_getvi(RTMIXIX, vich);
		if (vi == NULL)
			continue;

		const uint32_t attr = 0;	// disabled

		vi->CFG [VI_CFG_INDEX].ATTR = attr;
		vi->CFG [VI_CFG_INDEX].SIZE = ovl_ui_mbsize;
		vi->CFG [VI_CFG_INDEX].COORD = 0;
		vi->CFG [VI_CFG_INDEX].PITCH [0] = uipitch;	// PLANE 0 - The setting of this register is Y channel.
		vi->OVL_SIZE [0] = ovl_ui_mbsize;
		vi->HORI [0] = 0;
		vi->VERT [0] = 0;
		vi->FCOLOR [0] = 0xFFFF0000;	// при LAY_FILLCOLOR_EN - ALPGA + R + G + B - при LAY_FILLCOLOR_EN - замещает данные, идущие по DMA
	}

	int uich = 1;
	for (uich = 1; uich <= UI_LASTIX; ++ uich)
	{
		//DE_UI_TypeDef * const ui = (DE_UI_TypeDef *) (DE_BASE + T113_DE_MUX_CHAN + 0x1000 * uich);
		DE_UI_TypeDef * const ui = de3_getui(RTMIXIX, uich);
		if (ui == NULL)
			continue;

		const uint32_t attr = 0;	// disabled

		ui->CFG [UI_CFG_INDEX].ATTR = attr;
		ui->CFG [UI_CFG_INDEX].SIZE = ovl_ui_mbsize;
		ui->CFG [UI_CFG_INDEX].COORD = 0;
		ui->CFG [UI_CFG_INDEX].PITCH = uipitch;
		ui->OVL_SIZE = ovl_ui_mbsize;

		ASSERT(ui->CFG [UI_CFG_INDEX].ATTR == attr);
		ASSERT(ui->CFG [UI_CFG_INDEX].SIZE == ovl_ui_mbsize);
		ASSERT(ui->CFG [UI_CFG_INDEX].COORD == 0);
		ASSERT(ui->OVL_SIZE == ovl_ui_mbsize);
	}

	/* Не все блоки могут быть в t113-s3 */
//	write32(DE_BASE + T113_DE_MUX_VSU, 0);
//	write32(DE_BASE + T113_DE_MUX_GSU1, 0);
//	write32(DE_BASE + T113_DE_MUX_GSU2, 0);
//	write32(DE_BASE + T113_DE_MUX_GSU3, 0);
//	write32(DE_BASE + T113_DE_MUX_FCE, 0);
//	write32(DE_BASE + T113_DE_MUX_BWS, 0);
//	write32(DE_BASE + T113_DE_MUX_LTI, 0);
//	write32(DE_BASE + T113_DE_MUX_PEAK, 0);
//	write32(DE_BASE + T113_DE_MUX_ASE, 0);
//	write32(DE_BASE + T113_DE_MUX_FCC, 0);
//	write32(DE_BASE + T113_DE_MUX_DCSC, 0);

#if (CPUSTYLE_T507 || CPUSTYLE_H616)

//	PRINTF("bld->CSC_CTL=%08X @%p\n", bld->CSC_CTL, & bld->CSC_CTL);
//	bld->CSC_CTL = 0;

	* ((volatile uint32_t *) DE_VSU_BASE) = 0;
	* ((volatile uint32_t *) DE_FCE_BASE) = 0;
	* ((volatile uint32_t *) DE_BLS_BASE) = 0;

	// Allwinner_DE2.0_Spec_V1.0.pdf

#endif
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
		TCONLCD_PTR->LCD_FRM_CTL_REG = TCON_FRM_MODE_VAL;
	}
}

#endif

// LVDS: mstep1, HV: step1: Select HV interface type
static void t113_select_HV_interface_type(const videomode_t * vdmode)
{
	uint32_t start_dly;

	// ctrl
	//start_dly = (vdmode->vfp + vdmode->vbp + vdmode->vsync) / 2;
	start_dly = 2;//0x1F;	// 1,2 - need for 4.3 inch panel 272*480 - should be tested
	TCONLCD_PTR->LCD_CTL_REG =
		//1 * (UINT32_C(1) << 31) |		// LCD_EN - done in t113_open_module_enable
		0 * (UINT32_C(1) << 24) |		// LCD_IF 0x00: HV (Sync+DE), 01: 8080 I/F
		0 * (UINT32_C(1) << 23) |		// LCD_RB_SWAP
		1 * (UINT32_C(1) << 20) |		// LCD_INTERLACE_EN (has no effect)
		((start_dly & 0x1fu) << 4) |	// LCD_START_DLY
		0 * (UINT32_C(1) << 0) |			// LCD_SRC_SEL: 000: DE, 1..7 - tests: 1: color check, 2: grayscale check
		0;
}

static void t113_tconlcd_CCU_configuration(const videomode_t * vdmode, unsigned prei, unsigned divider, uint_fast32_t needfreq)
{
    divider = ulmax16(1, ulmin16(16, divider));	// Make range in 1..16
#if (CPUSTYLE_T507 || CPUSTYLE_H616)

	const unsigned ix = TCONLCD_IX;	// TCON_LCD0

	CCU->DISPLAY_IF_TOP_BGR_REG |= (UINT32_C(1) << 0);	// DISPLAY_IF_TOP_GATING
	CCU->DISPLAY_IF_TOP_BGR_REG &= ~ (UINT32_C(1) << 16);	// DISPLAY_IF_TOP_RST Assert
	CCU->DISPLAY_IF_TOP_BGR_REG |= (UINT32_C(1) << 16);	// DISPLAY_IF_TOP_RST De-assert writable mask 0x00010001

    //DISP_IF_TOP->MODULE_GATING |= (UINT32_C(1) << 31);
    //PRINTF("DISP_IF_TOP->MODULE_GATING=%08X\n", (unsigned) DISP_IF_TOP->MODULE_GATING);

    DISP_IF_TOP->DE_PORT_PERH_SEL = (DISP_IF_TOP->DE_PORT_PERH_SEL & ~ (UINT32_C(0x0F) << 4) & ~ (UINT32_C(0x0F) << 0)) |
    		0x00 * (UINT32_C(1) << 0) | // DE_PORT0_PERIPH_SEL: TCON_LCD0
    		0x01 * (UINT32_C(1) << 4) | // DE_PORT1_PERIPH_SEL: TCON_LCD1
			0;
    if (needfreq != 0)
    {
    	// LVDS mode
       	const uint_fast32_t pllreg = CCU->PLL_VIDEO1_CTRL_REG;
		const uint_fast32_t M = UINT32_C(1) + ((pllreg >> 1) & 0x01);	// PLL_INPUT_DIV_M
		uint_fast32_t N = calcdivround2(needfreq * M * 4, allwnrt113_get_hosc_freq());
		N = ulmin16(N, 256);
		N = ulmax16(N, 1);

		CCU->PLL_VIDEO1_CTRL_REG &= ~ (UINT32_C(1) << 31) & ~ (UINT32_C(1) << 29) & ~ (UINT32_C(1) << 27) & ~ (UINT32_C(0xFF) << 8);
		CCU->PLL_VIDEO1_CTRL_REG |= (N - 1) * UINT32_C(1) << 8;
		CCU->PLL_VIDEO1_CTRL_REG |= UINT32_C(1) << 31;	// PLL ENABLE
		CCU->PLL_VIDEO1_CTRL_REG |= UINT32_C(1) << 29;	// LOCK_ENABLE
		while ((CCU->PLL_VIDEO1_CTRL_REG & (UINT32_C(1) << 28)) == 0)
			;
		CCU->PLL_VIDEO1_CTRL_REG |= UINT32_C(1) << 27;	// PLL_OUTPUT_ENABLE

		PRINTF("t113_tconlcd_CCU_configuration: needfreq=%u MHz, N=%u\n", (unsigned) (needfreq / 1000 / 1000), (unsigned) N);
    	TCONLCD_CCU_CLK_REG = (TCONLCD_CCU_CLK_REG & ~ (UINT32_C(0x07) << 24)) |
			2 * (UINT32_C(1) << 24) | // 010: PLL_VIDEO1(1X)
    		0;
    }
    else
    {
    	TCONLCD_CCU_CLK_REG = (TCONLCD_CCU_CLK_REG & ~ (UINT32_C(0x07) << 24)) |
			2 * (UINT32_C(1) << 24) | // 010: PLL_VIDEO1(1X)
    		0;
    }
	TCONLCD_CCU_CLK_REG |= UINT32_C(1) << 31;	// SCLK_GATING
	//PRINTF("t113_tconlcd_CCU_configuration: BOARD_TCONLCDFREQ=%" PRIuFAST32 " MHz\n", (uint_fast32_t) BOARD_TCONLCDFREQ / 1000 / 1000);

	CCU->TCON_LCD_BGR_REG |= (UINT32_C(1) << (0 + ix));	// Clock Gating
	CCU->TCON_LCD_BGR_REG &= ~ (UINT32_C(1) << (16 + ix));	// Assert Reset
	CCU->TCON_LCD_BGR_REG |= (UINT32_C(1) << (16 + ix));	// De-assert Reset (bits 19..16 and 3..0 writable) mask 0x000F000F

#if WITHLVDSHW
    CCU->LVDS_BGR_REG |= (UINT32_C(1) << 16); // LVDS0_RST: De-assert reset (оба LVDS набора выходов разрешаются только одним битом)
//    PRINTF("CCU->LVDS_BGR_REG=%08X\n", (unsigned) CCU->LVDS_BGR_REG);
//    CCU->LVDS_BGR_REG |= (UINT32_C(1) << 16); // LVDS0_RST: De-assert reset (bits 19..16 writable)

    PRCM->VDD_SYS_PWROFF_GATING_REG |= (UINT32_C(1) << 4); // ANA_VDDON_GATING
    local_delay_ms(10);

//    CCU->HDMI0_CLK_REG |= (UINT32_C(1) << 31);
//    CCU->HDMI0_SLOW_CLK_REG |= (UINT32_C(1) << 31);
//
//    CCU->HDMI_BGR_REG |= (UINT32_C(1) << 17) | (UINT32_C(1) << 16) | (UINT32_C(1) << 0); // writble bits mask: 0x000F0005
//    PRINTF("CCU->HDMI_BGR_REG=%08X\n", (unsigned) CCU->HDMI_BGR_REG);

#endif /* WITHLVDSHW */

    local_delay_us(10);

    TCONLCD_PTR->LCD_IO_TRI_REG = UINT32_C(0xFFFFFFFF);

#elif (CPUSTYLE_T113 || CPUSTYLE_F133)

	/* Configure TCONLCD clock */
    if (needfreq != 0)
    {
    	prei = 0;
    	divider = calcdivround2(allwnrt113_get_video0pllx4_freq(), needfreq);
		//PRINTF("t113_tconlcd_CCU_configuration: needfreq=%u MHz, prei=%u, divider=%u\n", (unsigned) (needfreq / 1000 / 1000), (unsigned) prei, (unsigned) divider);
    	ASSERT(divider >= 1 && divider <= 16);
    	// LVDS
        TCONLCD_CCU_CLK_REG = (TCONLCD_CCU_CLK_REG & ~ ((UINT32_C(7) << 24) | (UINT32_C(3) << 8) | (UINT32_C(0x0F) << 0))) |
    		1 * (UINT32_C(1) << 24) |	// CLK_SRC_SEL 001: PLL_VIDEO0(4X)
    		(prei << 8) |	// FACTOR_N 0..3: 1..8
    		((divider - 1) << 0) |	// FACTOR_M (0x00..0x0F: 1..16)
    		0;
        TCONLCD_CCU_CLK_REG |= (UINT32_C(1) << 31);
    }
    else
    {
    	ASSERT(prei >= 0 && prei <= 3);
    	ASSERT(divider >= 1 && divider <= 16);
        TCONLCD_CCU_CLK_REG = (TCONLCD_CCU_CLK_REG & ~ ((UINT32_C(7) << 24) | (UINT32_C(3) << 8) | (UINT32_C(0x0F) << 0))) |
    		0 * (UINT32_C(1) << 24) |	// CLK_SRC_SEL 000: PLL_VIDEO0(1X)
    		(prei << 8) |	// FACTOR_N 0..3: 1..8
    		((divider - 1) << 0) |	// FACTOR_M (0x00..0x0F: 1..16)
    		0;
        TCONLCD_CCU_CLK_REG |= (UINT32_C(1) << 31);
    }
	//PRINTF("t113_tconlcd_CCU_configuration: BOARD_TCONLCDFREQ=%u MHz\n", (unsigned) (BOARD_TCONLCDFREQ / 1000 / 1000));
    local_delay_us(10);

    CCU->TCONLCD_BGR_REG |= (UINT32_C(1) << 0);	// Open the clock gate

#if WITHLVDSHW || WITHDSIHW
    CCU->LVDS_BGR_REG |= (UINT32_C(1) << 16); // LVDS0_RST: De-assert reset
#endif /* WITHLVDSHW || WITHDSIHW */

    CCU->TCONLCD_BGR_REG |= (UINT32_C(1) << 16);	// Release the LVDS reset of TCON LCD BUS GATING RESET register;
    local_delay_us(10);

#else

#endif
}

// HV step2 - Clock configuration
static void t113_HV_clock_configuration(const videomode_t * vdmode)
{
	unsigned val;
	// dclk
	// 31..28: TCON0_Dclk_En
	// 6..0: TCON0_Dclk_Div
	val = BOARD_TCONLCDFREQ / display_getdotclock(vdmode);
	//PRINTF("ltdc_divider=%u, dclk=%u kHz\n", val, (unsigned) (display_getdotclock(vdmode) / 1000));
	ASSERT(val >= 1 && val <= 127);
	TCONLCD_PTR->LCD_DCLK_REG =
		0x0F * (UINT32_C(1) << 28) |		// LCD_DCLK_EN
		val * (UINT32_C(1) << 0) |			// LCD_DCLK_DIV
		0;
    local_delay_us(10);
}

// LVDS step2 - Clock configuration
static void t113_LVDS_clock_configuration(const videomode_t * vdmode)
{
    TCONLCD_PTR->LCD_DCLK_REG =
		0x0F * (UINT32_C(1) << 28) |		// LCD_DCLK_EN
		7 * (UINT32_C(1) << 0) |			// LCD_DCLK_DIV
		0;
    local_delay_us(10);
}

// LVDS step2 - Clock configuration
// TODO: this is only placeholder!

static void t113_MIPIDSI_clock_configuration(const videomode_t * vdmode)
{
    TCONLCD_PTR->LCD_DCLK_REG =
		(UINT32_C(0x0F) << 28) |	// LCD_DCLK_EN
		(UINT32_C(7) << 0) |	// LCD_DCLK_DIV
		0;
    local_delay_us(10);
}

// step5 - set LVDS digital logic configuration
static void t113_set_LVDS_digital_logic(const videomode_t * vdmode)
{
#if defined (LCD_LVDS_IF_REG_VALUE)

	TCONLCD_PTR->LCD_LVDS_IF_REG = LCD_LVDS_IF_REG_VALUE;

#else /* defined (LCD_LVDS_IF_REG_VALUE) */

	TCONLCD_PTR->LCD_LVDS_IF_REG =
	(UINT32_C(1) << 31) |	/* LCD_LVDS_EN */
	(0u << 30) |	/* LCD_LVDS_LINK: 0: single link */
	(! UINT32_C(1) << 27) |	/* LCD_LVDS_MODE 1: JEIDA mode (0 for THC63LVDF84B converter) */
	(0u << 26) |	/* LCD_LVDS_BITWIDTH 0: 24-bit */
	(UINT32_C(1) << 20) |	/* LCD_LVDS_CLK_SEL 1: LCD CLK */
	0 * (UINT32_C(1) << 25) |		/* LCD_LVDS_DEBUG_EN */
	0 * (UINT32_C(1) << 24) |		/* LCD_LVDS_DEBUG_MODE */
	0 * (UINT32_C(1) << 4) |				/* LCD_LVDS_CLK_POL: 0: reverse, 1: normal */
	0 * 0x0F * (UINT32_C(1) << 0) |		/* LCD_LVDS_DATA_POL: 0: reverse, 1: normal */
	0;

#endif /* defined (LCD_LVDS_IF_REG_VALUE) */
}

#if 0
	// 0x0545103C - bit 0 is "1"
	// __de_dsi_dphy_dev_t taken from
	// https://github.com/mangopi-sbc/tina-linux-5.4/blob/a0e8ac494c8b05e2a4f8eb9a2f687e39db463ffe/drivers/video/fbdev/sunxi/disp2/disp/de/lowlevel_v2x/de_dsi_type.h#L1084
	union combo_phy_reg0_t {
		uint32_t dwval;
		struct {
			uint32_t en_cp               :  1 ;    //default: 0;
			uint32_t en_comboldo         :  1 ;    //default: 0;
			uint32_t en_lvds             :  1 ;    //default: 0;
			uint32_t en_mipi             :  1 ;    //default: 0;
			uint32_t en_test_0p8         :  1 ;    //default: 0;
			uint32_t en_test_comboldo    :  1 ;    //default: 0;
			uint32_t res0                :  26;    //default: 0;
		} bits;
	};

	union combo_phy_reg1_t {
		uint32_t dwval;
		struct {
			uint32_t reg_vref0p8         :  3 ;    //default: 0;
			uint32_t res0                :  1 ;    //default: 0;
			uint32_t reg_vref1p6         :  3 ;    //default: 0;
			uint32_t res1                :  25;    //default: 0;
		} bits;
	};
	//
	//	union combo_phy_reg2_t {
	//		uint32_t dwval;
	//		struct {
	//			uint32_t hs_stop_dly         :  8 ;    //default: 0;
	//			uint32_t res0                :  24;    //default: 0;
	//		} bits;
	//	};

//	memset((void *) DISPLAY_TOP_BASE, 0xFF, 256);
	//printhex32(DSI_DPHY_BASE, (void *) DSI_DPHY, 256);
	volatile union combo_phy_reg0_t * const phy0 = (volatile union combo_phy_reg0_t *) & DSI_DPHY->COMBO_PHY_REG0;
	volatile union combo_phy_reg1_t * const phy1 = (volatile union combo_phy_reg1_t *) & DSI_DPHY->COMBO_PHY_REG1;
//	DSI_DPHY->COMBO_PHY_REG0 |= ~ 0u;
//	DSI_DPHY->COMBO_PHY_REG1 |= ~ 0u;
	//	For PHY0:
	//	 Configure the reg_verf1p6 (differential mode voltage) in reg0x1114 to 4;
	//	 Configure the reg_vref0p8 reg0x1114 (common mode voltage) in reg0x1114 to 3;
	//	 Start en_cp, en_mipi, en_lvds, and en_comboldo in reg0x1110, in turn.

	phy1->bits.reg_vref0p8 = 0x03;
	phy1->bits.reg_vref1p6 = 0x04;
	phy0->bits.en_cp = 1;
	phy0->bits.en_mipi = 1;
	phy0->bits.en_lvds = 1;
	phy0->bits.en_comboldo = 1;
	//	CON_LCD0->COMBO_PHY_REG0=0000000F
	//	CON_LCD0->COMBO_PHY_REG1=00000043
	PRINTF("CON_LCD0->COMBO_PHY_REG0=%08X\n", (unsigned) DSI_DPHY->COMBO_PHY_REG0);
	PRINTF("CON_LCD0->COMBO_PHY_REG1=%08X\n", (unsigned) DSI_DPHY->COMBO_PHY_REG1);
#endif

static void t113_DSI_controller_configuration(const videomode_t * vdmode)
{
#if (CPUSTYLE_T113 || CPUSTYLE_F133)
	// __de_dsi_dphy_dev_t
	// https://github.com/mangopi-sbc/tina-linux-5.4/blob/0d4903ebd9d2194ad914686d5b0fc1ddacf11a9d/drivers/video/fbdev/sunxi/disp2/disp/de/lowlevel_v2x/de_lcd.c#L388

	CCU->DSI_CLK_REG = (CCU->DSI_CLK_REG & ~ ((UINT32_C(7) << 24) | UINT32_C(0x0F) << 0)) |
		(UINT32_C(2) << 24) |	// 010: PLL_VIDEO0(2X)	= 594 MHz
		//(UINT32_C(3) << 24) |	// 011: PLL_VIDEO1(2X)	= 594 MHz
		((UINT32_C(4) - 1) << 0) |
		0;

	CCU->DSI_CLK_REG |= UINT32_C(1) << 31;		// DSI_CLK_GATING
	(void) CCU->DSI_CLK_REG;

	CCU->DSI_BGR_REG |= UINT32_C(1) << 0;	// DSI_GATING
	CCU->DSI_BGR_REG |= UINT32_C(1) << 16;	// DSI_RST
	(void) CCU->DSI_BGR_REG;

//	PRINTF("allwnrt113_get_dsi_freq()=%" PRIuFAST32 "\n", allwnrt113_get_dsi_freq());
//	printhex32(DSI0_BASE, DSI0, sizeof * DSI0);

	{
		// Taken from https://github.com/mangopi-sbc/tina-linux-5.4/blob/a0e8ac494c8b05e2a4f8eb9a2f687e39db463ffe/drivers/video/fbdev/sunxi/disp2/disp/de/lowlevel_v2x/de_dsi_type.h#L977

		DSI_DPHY->COMBO_PHY_REG1 = 0x43;
		DSI_DPHY->COMBO_PHY_REG0 = 0x1;
		local_delay_us(5);
		DSI_DPHY->COMBO_PHY_REG0 = 0x5;
		local_delay_us(5);
		DSI_DPHY->COMBO_PHY_REG0 = 0x7;
		local_delay_us(5);
		DSI_DPHY->COMBO_PHY_REG0 = 0xf;
		local_delay_us(5);

		DSI_DPHY->DPHY_ANA4 = 0x84000000;
		DSI_DPHY->DPHY_ANA3 = 0x01040000;
		DSI_DPHY->DPHY_ANA2 = DSI_DPHY->DPHY_ANA2 & (0x0u << 1);	/* ;) */
		DSI_DPHY->DPHY_ANA1 = 0x0;

	}
#elif (CPUSTYLE_T507 || CPUSTYLE_H616)
#endif /* (CPUSTYLE_T113 || CPUSTYLE_F133) */
}

// https://github.com/dumtux/Allwinner-H616/blob/e900407aca767f1429ba4a6a990b8b7c9f200914/u-boot/arch/arm/include/asm/arch-sunxi/lcdc.h#L105
// step6 - LVDS controller configuration
static void t113_LVDS_controller_configuration(const videomode_t * vdmode, unsigned lvds_num)
{
#if (CPUSTYLE_T507 || CPUSTYLE_H616)
	// Documented as LCD_LVDS_ANA0_REG
	//const unsigned lvds_num = 0;	/* 0: LVDS0, 1: LVDS1 */
	// Step 5 LVDS digital logic configuration

	// Step 6 LVDS controller configuration
	// LVDS_HPREN_DRVC and LVDS_HPREN_DRV
	TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] =
		(UINT32_C(1) << 24) |	// LVDS_HPREN_DRVC
		(UINT32_C(0x0F) << 20) |	// When LVDS signal is 18-bit, LVDS_HPREN_DRV=0x7; when LVDS signal is 24-bit, LVDS_HPREN_DRV=0xF;
		(UINT32_C(0x04) << 17) |	// Configure LVDS0_REG_C (differential mode voltage) to 4; 100: 336 mV
		(UINT32_C(3) << 8) |	// ?LVDS_REG_R Configure LVDS0_REG_V (common mode voltage) to 3;
		0;
	// test
	//TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] |= (UINT32_C(1) << 16);	// LVDS_REG_DENC
	//TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] |= (UINT32_C(0x0F) << 12);	// LVDS_REG_DEN

//	TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] |= (UINT32_C(1) << 30);	// en_ldo
//	local_delay_ms(1);

	// 	Lastly, start module voltage, and enable EN_LVDS and EN_24M.
	TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] |= (UINT32_C(1) << 31);	// ?LVDS_EN_MB start module voltage
	local_delay_ms(1);
//	TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] |= (UINT32_C(1) << 29);	// enable EN_LVDS
//	local_delay_ms(1);
//	TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] |= (UINT32_C(1) << 28);	// EN_24M
//	local_delay_ms(1);
	//PRINTF("TCONLCD_PTR->LCD_LVDS_ANA_REG [%u]=%08X\n", lvds_num, (unsigned) TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num]);

#elif (CPUSTYLE_T113 || CPUSTYLE_F133)
	// Documented as LCD_LVDS_ANA0_REG
	//const unsigned lvds_num = 0;	/* 0: LVDS0, 1: LVDS1 */
	// Step 5 LVDS digital logic configuration

	// Step 6 LVDS controller configuration
	// LVDS_HPREN_DRVC and LVDS_HPREN_DRV
	TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] =
		(UINT32_C(0x0F) << 20) |	// When LVDS signal is 18-bit, LVDS_HPREN_DRV=0x7; when LVDS signal is 24-bit, LVDS_HPREN_DRV=0xF;
		(UINT32_C(1) << 24) |	// LVDS_HPREN_DRVC
		(UINT32_C(0x04) << 17) |	// Configure LVDS0_REG_C (differential mode voltage) to 4; 100: 336 mV
		(UINT32_C(3) << 8) |	// ?LVDS_REG_R Configure LVDS0_REG_V (common mode voltage) to 3;
		0;
	// test
	//TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] |= (UINT32_C(1) << 16);	// LVDS_REG_DENC
	//TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] |= (UINT32_C(0x0F) << 12);	// LVDS_REG_DEN

	TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] |= (UINT32_C(1) << 30);	// en_ldo
	local_delay_ms(1);

	// 	Lastly, start module voltage, and enable EN_LVDS and EN_24M.
	TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] |= (UINT32_C(1) << 31);	// ?LVDS_EN_MB start module voltage
	local_delay_ms(1);
	TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] |= (UINT32_C(1) << 29);	// enable EN_LVDS
	local_delay_ms(1);
	TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] |= (UINT32_C(1) << 28);	// EN_24M
	local_delay_ms(1);
	//PRINTF("TCONLCD_PTR->LCD_LVDS_ANA_REG [%u]=%08X\n", lvds_num, (unsigned) TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num]);

#else
#endif
}

// Set sequuence parameters
static void t113_set_sequence_parameters(const videomode_t * vdmode)
{
	/* Accumulated parameters for this display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned HSYNC = vdmode->hsync;	/*  */
	const unsigned VSYNC = vdmode->vsync;	/*  */
	const unsigned LEFTMARGIN = HSYNC + vdmode->hbp;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = VSYNC + vdmode->vbp;	/* vertical delay before DE start */
	const unsigned HTOTAL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VTOTAL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

//	PRINTF("TCONLCD_PTR:\n");
//	printhex32(TCON_LCD0_BASE, (void* ) TCON_LCD0_BASE, sizeof * TCONLCD_PTR);
	// timing0 (window)
	TCONLCD_PTR->LCD_BASIC0_REG = (
		((WIDTH - 1) << 16) | ((HEIGHT - 1) << 0)
		);
	// timing1
	TCONLCD_PTR->LCD_BASIC1_REG =
		((HTOTAL - 1) << 16) |		// TOTAL
		((LEFTMARGIN - 1) << 0) |	// HBP
		0;
	// timing2
	TCONLCD_PTR->LCD_BASIC2_REG =
		((VTOTAL * 2) << 16) | 	// VT Tvt = (VT)/2 * Thsync
		((TOPMARGIN - 1) << 0) |		// VBP Tvbp = (VBP+1) * Thsync
		0;
	// timing3
	TCONLCD_PTR->LCD_BASIC3_REG =
		((HSYNC - 1) << 16) |	// HSPW Thspw = (HSPW+1) * Tdclk
		((VSYNC - 1) << 0) |	// VSPW Tvspw = (VSPW+1) * Thsync
		0;

//	PRINTF("TCONLCD_PTR:\n");
//	printhex32(TCON_LCD0_BASE, (void* ) TCON_LCD0_BASE, sizeof * TCONLCD_PTR);
}

// Step4 - Open IO output
static void t113_open_IO_output(const videomode_t * vdmode)
{
	// io_tristate
	//write32((uintptr_t) & tcon->io_tristate, 0);
	TCONLCD_PTR->LCD_IO_TRI_REG = 0;
	// 5.1.6.20 0x0088 LCD IO Polarity Register (Default Value: 0x0000_0000)
	// io_polarity
	{
		int h_sync_active;	// 1 - negatibe pulses, 0 - positice pulses
		int v_sync_active;	// 1 - negatibe pulses, 0 - positice pulses
		int den_active;		// 1 - negatibe pulses, 0 - positice pulses
		int clk_active;		// 1 - negatibe pulses, 0 - positice pulses

		h_sync_active = vdmode->vsyncneg;
		v_sync_active = vdmode->hsyncneg;
		den_active = ! vdmode->deneg;
		clk_active = 0;

		uint32_t val;
#if 1//(CPUSTYLE_T507 || CPUSTYLE_H616)
		// вызывает сдвиг на пиксель
		val = 0;
#else
		val =
			(UINT32_C(1) << 31) | 	// IO_Output_Sel: 0: normal, 1: sync to dclk
			(UINT32_C(1) << 28) |	// DCLK_Sel: 0x00: DCLK0 (normal phase offset), 0x01: DCLK1(1/3 phase offset
			0;
#endif

		if (! h_sync_active)
			val |= (UINT32_C(1) << 25);	// IO1_Inv 0 HSYMC
		if (! v_sync_active)
			val |= (UINT32_C(1) << 24);	// IO0_Inv - VSYNC
		if (! den_active)
			val |= (UINT32_C(1) << 27);	// IO3_Inv - DE
		if (!! clk_active)
			val |= (UINT32_C(1) << 26);	// IO2_Inv - DCLK

		TCONLCD_PTR->LCD_IO_POL_REG = val;

	}
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
		TCONLCD_PTR->LCD_FRM_CTL_REG = TCON_FRM_MODE_VAL;

	}
}

// Set and open interrupt function
static void t113_set_and_open_interrupt_function(const videomode_t * vdmode)
{
}


// Open module enable
static void t113_open_module_enable(const videomode_t * vdmode)
{
	TCONLCD_PTR->LCD_CTL_REG |= (UINT32_C(1) << 31);	// LCD_EN
	TCONLCD_PTR->LCD_GCTL_REG |= (UINT32_C(1) << 31);	// LCD_EN
}

// Paraleal RGB mode
static void t113_tcon_hw_initsteps(const videomode_t * vdmode)
{
	unsigned prei = 0;
	unsigned divider = 1;
	// step0 - CCU configuration
	t113_tconlcd_CCU_configuration(vdmode, prei, divider, 0);
	// step1 - Select HV interface type
	t113_select_HV_interface_type(vdmode);
	// step2 - Clock configuration
	t113_HV_clock_configuration(vdmode);
	// step3 - Set sequuence parameters
	t113_set_sequence_parameters(vdmode);
	// step4 - Open IO output
	t113_open_IO_output(vdmode);
	// step5 - Set and open interrupt function
	t113_set_and_open_interrupt_function(vdmode);
	// step6 - Open module enable
	t113_open_module_enable(vdmode);
}
#if 0

static void lvds_t507_corrections(void)
{
#if CPUSTYLE_T507
	unsigned i;
#if 0
	// HDMI_PHY
	static const uint32_t hdmi_phy [] =
	{
		0x00000000, 0x80c00000, 0x00184210, 0x00000002,
		0x00000000, 0x000f990b, 0x00000000, 0x00000000,
		0x00000f80, 0x0c0040d8, 0x02700203, 0x000c6023,
	};
	for (i = 0; i < ARRAY_SIZE(hdmi_phy); ++ i)
	{
		* (volatile uint32_t *) (HDMI_PHY_BASE + i * 4) = hdmi_phy [i];
	}
#endif

#if 0

	// TCON_LCD0
	* (volatile uint32_t *) 0x0000000006511004 = 0x80000002;
	* (volatile uint32_t *) 0x0000000006511010 = 0x00000000;

	* (volatile uint32_t *) 0x0000000006511040 = 0x800001f0;
	* (volatile uint32_t *) 0x0000000006511044 = 0xf0000007;
	* (volatile uint32_t *) 0x0000000006511050 = 0x05140022;
	* (volatile uint32_t *) 0x0000000006511054 = 0x00450000;

	* (volatile uint32_t *) 0x0000000006511084 = 0x80100000;
	* (volatile uint32_t *) 0x000000000651108c = 0xe0000000;
	* (volatile uint32_t *) 0x00000000065110fc = 0x219c4000;	// LCD Debug Register

	* (volatile uint32_t *) 0x00000000065111f0 = 0x02fd0023;
	* (volatile uint32_t *) 0x0000000006511220 = 0xc1f40320;
	* (volatile uint32_t *) 0x000000000651123c = 0x0003e814;
	* (volatile uint32_t *) 0x0000000006511240 = 0x01f401f4;
#endif
#if 0
	// HDMI_TX0
	* (volatile uint32_t *) 0x0000000006000000 = 0x00000021;
	* (volatile uint32_t *) 0x0000000006000004 = 0x0000009f;
	* (volatile uint32_t *) 0x0000000006000170 = 0x00000002;	// !!!!
	* (volatile uint32_t *) 0x0000000006000180 = 0x00000018;
	* (volatile uint32_t *) 0x0000000006000200 = 0x00000001;
#endif
#endif /* CPUSTYLE_T507 */
}


void
zprinthex32(uintptr_t voffs, const void * vbuff, unsigned length)
{
	const volatile uint32_t * buff = (const volatile uint32_t *) vbuff;
	enum { ROWSIZE = 4 };	/* elements in one row */
	unsigned i, j;
	unsigned rows = ((length + 3) / 4 + ROWSIZE - 1) / ROWSIZE;

	for (i = 0; i < rows; ++ i)
	{
		const int remaining = (length + 3) / 4 - i * ROWSIZE;
		const int trl = (ROWSIZE < remaining) ? ROWSIZE : remaining;
		debug_printf_P(PSTR("0x%016" PRIx32 ":"), (uint32_t) (voffs + i * ROWSIZE * 4));
		for (j = 0; j < trl; ++ j)
			debug_printf_P(PSTR(" 0x%08" PRIx32), buff [i * ROWSIZE + j]);

		debug_printf_P(PSTR("\n"));
	}
}

static void
performdump(const char * name, unsigned long base, unsigned long size)
{
#if CPUSTYLE_T507
	PRINTF("%s\n", name);
	zprinthex32(base, (void *) base, size);
	PRINTF("---\n");
#endif /* CPUSTYLE_T507 */
}
#endif

static void t113_tcon_lvds_initsteps(const videomode_t * vdmode)
{
	unsigned prei = 0;
	unsigned divider = BOARD_TCONLCDFREQ / (display_getdotclock(vdmode) * 7);
	// step0 - CCU configuration
	t113_tconlcd_CCU_configuration(vdmode, prei, divider, display_getdotclock(vdmode) * 7);
	// step1 - same as step1 in HV mode: Select HV interface type
	t113_select_HV_interface_type(vdmode);
	// step2 - Clock configuration
	t113_LVDS_clock_configuration(vdmode);
	// step3 - same as step3 in HV mode: Set sequuence parameters
	t113_set_sequence_parameters(vdmode);
	// step4 - same as step4 in HV mode: Open IO output
	t113_open_IO_output(vdmode);
	// step5 - set LVDS digital logic configuration
	t113_set_LVDS_digital_logic(vdmode);
	// step6 - LVDS controller configuration
	t113_DSI_controller_configuration(vdmode);
	t113_LVDS_controller_configuration(vdmode, TCONLCD_LVDSIX);
	// step7 - same as step5 in HV mode: Set and open interrupt function
	t113_set_and_open_interrupt_function(vdmode);
	// step8 - same as step6 in HV mode: Open module enable
	t113_open_module_enable(vdmode);

}

// What is DPSS_TOP_BGR_REG ?
static void t113_tcon_dsi_initsteps(const videomode_t * vdmode)
{
	unsigned prei = 0;
	unsigned divider = BOARD_TCONLCDFREQ / (display_getdotclock(vdmode) * 7);
	// step0 - CCU configuration
	t113_tconlcd_CCU_configuration(vdmode, prei, divider, display_getdotclock(vdmode) * 7);
	// step1 - same as step1 in HV mode: Select HV interface type
	t113_select_HV_interface_type(vdmode);
	// step2 - Clock configuration
	t113_MIPIDSI_clock_configuration(vdmode);
	// step3 - same as step3 in HV mode: Set sequuence parameters
	t113_set_sequence_parameters(vdmode);
	// step4 - same as step4 in HV mode: Open IO output
	t113_open_IO_output(vdmode);
	// step5 - set LVDS digital logic configuration
	t113_set_LVDS_digital_logic(vdmode);
	// step6 - LVDS controller configuration
#if (CPUSTYLE_T507 || CPUSTYLE_H616)
	// These CPUs not support DSI at all

#elif (CPUSTYLE_T113 || CPUSTYLE_F133)
	CCU->DPSS_TOP_BGR_REG |= UINT32_C(1) << 1;	// DPSS_TOP_GATING
	CCU->DPSS_TOP_BGR_REG |= UINT32_C(1) << 16;	// DPSS_TOP_RST
	(void) DSI0->DSI_CTL;
#else

#endif
	t113_DSI_controller_configuration(vdmode);
	t113_LVDS_controller_configuration(vdmode, TCONLCD_LVDSIX);
	// step7 - same as step5 in HV mode: Set and open interrupt function
	t113_set_and_open_interrupt_function(vdmode);
	// step8 - same as step6 in HV mode: Open module enable
	t113_open_module_enable(vdmode);

}

static void hardware_de_initialize(const videomode_t * vdmode)
{
#if CPUSTYLE_A64

	// https://github.com/bigtreetech/CB1-Kernel/blob/244c0fd1a2a8e7f2748b2a9ae3a84b8670465351/u-boot/drivers/video/sunxi/sunxi_de2.c#L128

	/* переключить память к DE & VI */
    // https://github.com/bigtreetech/CB1-Kernel/blob/244c0fd1a2a8e7f2748b2a9ae3a84b8670465351/u-boot/drivers/video/sunxi/sunxi_de2.c#L39
	//#define SUNXI_SRAMC_BASE 0x03000000
    // Under CONFIG_MACH_SUN50I
 	{
		uint32_t reg_value;

		/* set SRAM for video use */
		//reg_value = readl(SUNXI_SRAMC_BASE + 0x04);
		reg_value = * (volatile uint32_t *) (SYS_CFG_BASE + 0x04);
		//PRINTF("1 switch memory: reg_value=%08X\n", (unsigned) reg_value);
		reg_value &= ~ (UINT32_C(1) << 24);
		//writel(reg_value, SUNXI_SRAMC_BASE + 0x04);
		* (volatile uint32_t *) (SYS_CFG_BASE + 0x04) = reg_value;
		//PRINTF("2 switch memory: reg_value=%08X\n", (unsigned) reg_value);

	}
#warning TODO: Enable ahb_reset1_cfg and ahb_gate1
	/* Set ahb gating to pass */
//	setbits_le32(&ccm->ahb_reset1_cfg, 1 << AHB_RESET_OFFSET_DE);
//	setbits_le32(&ccm->ahb_gate1, 1 << AHB_GATE_OFFSET_DE);
	// PLL_VIDEO1 may be used for LVDS synchronization
	/* Configure DE clock (no FACTOR_N on T507/H616 CPU) */
	unsigned divider = 8;
    CCU->DE_CLK_REG = (CCU->DE_CLK_REG & ~ (UINT32_C(0x07) << 24) & ~ (UINT32_C(0x0F) << 0)) |
		0 * (UINT32_C(1) << 24) |	// CLK_SRC_SEL 000: PLL_PERIPH0(2X)
		(divider - 1) * (UINT32_C(1) << 0) |	// FACTOR_M 300 MHz
		0;
    CCU->DE_CLK_REG |= (UINT32_C(1) << 31);	// SCLK_GATING
    local_delay_us(10);

    CCU->BUS_CLK_GATING_REG1 |= (UINT32_C(1) << 12);	// DE_GATING
    CCU->BUS_SOFT_RST_REG1 |= (UINT32_C(1) << 12);	// DE_RST

//	PRINTF("allwnr_t507_get_de_freq()=%" PRIuFAST32 " MHz\n", allwnr_t507_get_de_freq() / 1000 / 1000);
//	PRINTF("allwnr_t507_get_mbus_freq()=%" PRIuFAST32 " MHz\n", allwnr_t507_get_mbus_freq() / 1000 / 1000);

//    CCU->DE_BGR_REG = (UINT32_C(1) << 0);		// Open the clock gate
//    CCU->DE_BGR_REG |= (UINT32_C(1) << 16);		// De-assert reset
    local_delay_us(10);

 	/* Global DE settings */

	// https://github.com/BPI-SINOVOIP/BPI-M2U-bsp/blob/2adcf0fe39e54b9bcacbd5bcd3ecb6077e081122/linux-sunxi/drivers/video/sunxi/disp2/disp/de/lowlevel_v3x/de_clock.c#L91

//	PRINTF("DE_TOP before:\n");
//	printhex32(DE_TOP_BASE, DE_TOP, 256);
//	memset(DE_TOP, 255, 256);
//	PRINTF("DE_TOP fill 0xFF:\n");
//	printhex32(DE_TOP_BASE, DE_TOP, 256);
//	memset(DE_TOP, 0, 256);
//	PRINTF("DE_TOP fill 0x00:\n");
//	printhex32(DE_TOP_BASE, DE_TOP, 256);
//
// 	DE_TOP->DE_SCLK_DIV =
//		7 * (UINT32_C(1) << 8) |	// wb-div
//		7 * (UINT32_C(1) << 4) |	// mixer1-div
//		7 * (UINT32_C(1) << 0) |	// mixer0-div
//		0;

 	DE_TOP->SCLK_GATE |= 0x1F;	//UINT32_C(1) << 0;	// CORE0_SCLK_GATE
 	DE_TOP->HCLK_GATE |= 0x1F;	//UINT32_C(1) << 0;	// CORE0_HCLK_GATE

 	// Only one bit writable
 	DE_TOP->AHB_RESET &= ~ (UINT32_C(1) << 0);	// CORE0_AHB_RESET
	DE_TOP->AHB_RESET |=(UINT32_C(1) << 0);		// CORE0_AHB_RESET

//	PRINTF("DE_TOP AHB reset:\n");
//	printhex32(DE_TOP_BASE, DE_TOP, 256);

	DE_GLB_TypeDef * const glb = de3_getglb(RTMIXIX);
	if (glb == NULL)
		return;
	glb->GLB_CTL =
			(UINT32_C(1) << 12) |	// OUT_DATA_WB 0:RT-WB fetch data after DEP port
			(UINT32_C(1) << 0) |		// EN RT enable/disable
			0;

	//glb->GLB_CLK |= (UINT32_C(1) << 0);

	//* (volatile uint32_t *) (DE_TOP_BASE + 0x00C) = 1;	// это не делитель
	//* (volatile uint32_t *) (DE_TOP_BASE + 0x010) |= 0xFFu;	// вешает. После сброса 0x000000E4
	//* (volatile uint32_t *) (DE_TOP_BASE + 0x010) |= 0xFF000000u;

	//ASSERT(glb->GLB_CTL & (UINT32_C(1) << 0));


	glb->GLB_STS = 0;

    return;
//	PRINTF("DE_TOP AHB final:\n");
//	printhex32(DE_TOP_BASE, DE_TOP, 256);
#elif CPUSTYLE_T507 || CPUSTYLE_H616

	// https://github.com/bigtreetech/CB1-Kernel/blob/244c0fd1a2a8e7f2748b2a9ae3a84b8670465351/u-boot/drivers/video/sunxi/sunxi_de2.c#L128

	/* переключить память к DE & VI */
    // https://github.com/bigtreetech/CB1-Kernel/blob/244c0fd1a2a8e7f2748b2a9ae3a84b8670465351/u-boot/drivers/video/sunxi/sunxi_de2.c#L39
	//#define SUNXI_SRAMC_BASE 0x03000000
    // Under CONFIG_MACH_SUN50I
 	{
		uint32_t reg_value;

		/* set SRAM for video use */
		//reg_value = readl(SUNXI_SRAMC_BASE + 0x04);
		reg_value = * (volatile uint32_t *) (SYS_CFG_BASE + 0x04);
		////PRINTF("1 switch memory: reg_value=%08X\n", (unsigned) reg_value);
		reg_value &= ~ (UINT32_C(1) << 24);
		//writel(reg_value, SUNXI_SRAMC_BASE + 0x04);
		* (volatile uint32_t *) (SYS_CFG_BASE + 0x04) = reg_value;
		////PRINTF("2 switch memory: reg_value=%08X\n", (unsigned) reg_value);

	}
	// PLL_VIDEO1 may be used for LVDS synchronization
	/* Configure DE clock (no FACTOR_N on T507/H616 CPU) */
	unsigned divider = 1;
    CCU->DE_CLK_REG = (CCU->DE_CLK_REG & ~ (UINT32_C(1) << 24) & ~ (UINT32_C(0x0F) << 0)) |
		0 * (UINT32_C(1) << 24) |	// CLK_SRC_SEL 0: PLL_DE 1: PLL_PERI0(2X)
		(divider - 1) * (UINT32_C(1) << 0) |	// FACTOR_M 300 MHz
		0;
    CCU->DE_CLK_REG |= (UINT32_C(1) << 31);	// SCLK_GATING
    local_delay_us(10);

	//PRINTF("allwnr_t507_get_de_freq()=%" PRIuFAST32 " MHz\n", allwnr_t507_get_de_freq() / 1000 / 1000);
	//PRINTF("allwnr_t507_get_mbus_freq()=%" PRIuFAST32 " MHz\n", allwnr_t507_get_mbus_freq() / 1000 / 1000);

    CCU->DE_BGR_REG = (UINT32_C(1) << 0);		// Open the clock gate
    CCU->DE_BGR_REG |= (UINT32_C(1) << 16);		// De-assert reset
    local_delay_us(10);

 	/* Global DE settings */

	// https://github.com/BPI-SINOVOIP/BPI-M2U-bsp/blob/2adcf0fe39e54b9bcacbd5bcd3ecb6077e081122/linux-sunxi/drivers/video/sunxi/disp2/disp/de/lowlevel_v3x/de_clock.c#L91

 	DE_TOP->DE_SCLK_GATE |= UINT32_C(1) << 0;	// CORE0_SCLK_GATE
 	DE_TOP->DE_HCLK_GATE |= UINT32_C(1) << 0;	// CORE0_HCLK_GATE

 	// Only one bit writable
 	DE_TOP->DE_AHB_RESET &= ~ (UINT32_C(1) << 0);	// CORE0_AHB_RESET
	DE_TOP->DE_AHB_RESET |= (UINT32_C(1) << 0);		// CORE0_AHB_RESET

#if 0
	//PRINTF("DE_TOP after:\n");
	//printhex32(DE_TOP_BASE, DE_TOP, 0x160);
	// Offset 0x24: 0x01020905
	PRINTF("DE_IP_CFG=%08X\n", (unsigned) DE_TOP->DE_IP_CFG);
	PRINTF("DE_IP_CFG.RTD1_UI_NO=%08X\n", (unsigned) (DE_TOP->DE_IP_CFG >> 27) & 0x07);
	PRINTF("DE_IP_CFG.RTD1_VIDEO_NO=%08X\n", (unsigned) (DE_TOP->DE_IP_CFG >> 24) & 0x07);
	PRINTF("DE_IP_CFG.RTD0_UI_NO=%08X\n", (unsigned) (DE_TOP->DE_IP_CFG >> 11) & 0x07);
	PRINTF("DE_IP_CFG.RTD0_VIDEO_NO=%08X\n", (unsigned) (DE_TOP->DE_IP_CFG >> 8) & 0x07);
#endif

	DE_GLB_TypeDef * const glb = de3_getglb(RTMIXIX);
	if (glb == NULL)
		return;

	glb->GLB_CTL =
			(UINT32_C(1) << 12) |	// OUT_DATA_WB 0:RT-WB fetch data after DEP port
			(UINT32_C(1) << 0) |		// EN RT enable/disable
			0;

	glb->GLB_CLK |= (UINT32_C(1) << 0);

	//* (volatile uint32_t *) (DE_TOP_BASE + 0x00C) = 1;	// это не делитель
	//* (volatile uint32_t *) (DE_TOP_BASE + 0x010) |= 0xFFu;	// вешает. После сброса 0x000000E4
	//* (volatile uint32_t *) (DE_TOP_BASE + 0x010) |= 0xFF000000u;

	ASSERT(glb->GLB_CTL & (UINT32_C(1) << 0));

	glb->GLB_STS = 0;

#elif CPUSTYLE_T113 || CPUSTYLE_F133
	// PLL_VIDEO1 may be used for LVDS synchronization
	/* Configure DE clock (no FACTOR_N on this CPU) */
	unsigned divider = 4;
	CCU->DE_CLK_REG = (CCU->DE_CLK_REG & ~ ((UINT32_C(7) << 24) | (UINT32_C(0x1F) << 0))) |
		1 * (UINT32_C(1) << 24) |	// CLK_SRC_SEL 001: PLL_VIDEO0(4X)
		(divider - 1) * (UINT32_C(1) << 0) |	// FACTOR_M 300 MHz
		0;
    CCU->DE_CLK_REG |= (UINT32_C(1) << 31);	// SCLK_GATING
    local_delay_us(10);
	PRINTF("allwnrt113_get_de_freq()=%" PRIuFAST32 " MHz\n", allwnrt113_get_de_freq() / 1000 / 1000);

    CCU->DE_BGR_REG |= (UINT32_C(1) << 0);		// Open the clock gate
    CCU->DE_BGR_REG |= (UINT32_C(1) << 16);		// De-assert reset
    local_delay_us(10);

	/* Global DE settings */
//	PRINTF("DE_TOP before:\n");
//	printhex32(DE_TOP_BASE, DE_TOP, 0x160);

    if (RTMIXIX == 1)
    {
        // Enable RT-Mixer 0
    	DE_TOP->GATE_CFG |= UINT32_C(1) << 0;
    	DE_TOP->RST_CFG &= ~ (UINT32_C(1) << 0);
    	DE_TOP->RST_CFG |= UINT32_C(1) << 0;
    	DE_TOP->BUS_CFG |= UINT32_C(1) << 0;

    	DE_TOP->SEL_CFG &= ~ (UINT32_C(1) << 0);	/* MIXER0->TCON0; MIXER1->TCON1 */
    }

    if (RTMIXIX == 2)
    {
        // Enable RT-Mixer 1
    	DE_TOP->GATE_CFG |= UINT32_C(1) << 1;
    	DE_TOP->RST_CFG &= ~ (UINT32_C(1) << 1);
    	DE_TOP->RST_CFG |= UINT32_C(1) << 1;
    	DE_TOP->BUS_CFG |= UINT32_C(1) << 1;

    	DE_TOP->SEL_CFG |= (UINT32_C(1) << 0);	/* MIXER0->TCON1; MIXER1->TCON0 */
    }


#if 0
	//PRINTF("DE_TOP after:\n");
	//printhex32(DE_TOP_BASE, DE_TOP, 0x160);
	// Offset 0x24: 0x01020905
	PRINTF("DE_IP_CFG=%08X\n", (unsigned) DE_TOP->DE_IP_CFG);
	PRINTF("DE_IP_CFG.RTD1_UI_NO=%08X\n", (unsigned) (DE_TOP->DE_IP_CFG >> 27) & 0x07);
	PRINTF("DE_IP_CFG.RTD1_VIDEO_NO=%08X\n", (unsigned) (DE_TOP->DE_IP_CFG >> 24) & 0x07);
	PRINTF("DE_IP_CFG.RTD0_UI_NO=%08X\n", (unsigned) (DE_TOP->DE_IP_CFG >> 11) & 0x07);
	PRINTF("DE_IP_CFG.RTD0_VIDEO_NO=%08X\n", (unsigned) (DE_TOP->DE_IP_CFG >> 8) & 0x07);
#endif

	DE_GLB_TypeDef * const glb = de3_getglb(RTMIXIX);
	if (glb == NULL)
		return;

	glb->GLB_CTL =
			(UINT32_C(1) << 12) |	// OUT_DATA_WB 0:RT-WB fetch data after DEP port
			(UINT32_C(1) << 0) |		// EN RT enable/disable
			0;

//	PRINTF("1 glb->GLB_CLK=%08" PRIX32 "\n", glb->GLB_CLK);
//	glb->GLB_CLK |= (UINT32_C(1) << 0);
//	PRINTF("2 glb->GLB_CLK=%08" PRIX32 "\n", glb->GLB_CLK);

	ASSERT(glb->GLB_CTL & (UINT32_C(1) << 0));

	glb->GLB_STS = 0;

#else
	#error Undefined CPUSTYLE_xxx
#endif

	t113_de_update();

	t113_de_set_mode(vdmode, RTMIXIX, COLOR24(255, 255, 0));	// yellow

	t113_de_update();
}

static void hardware_tcon_initialize(const videomode_t * vdmode)
{
#if WITHLVDSHW
	t113_tcon_lvds_initsteps(vdmode);
#elif WITHMIPIDSISHW
	t113_tcon_dsi_initsteps(vdmode);
#else /* WITHLVDSHW */
	t113_tcon_hw_initsteps(vdmode);
#endif /* WITHLVDSHW */

}

void hardware_ltdc_initialize(const uintptr_t * frames_unused, const videomode_t * vdmode)
{
	hardware_tcon_initialize(vdmode);

	// Set DE MODE if need, mapping GPIO pins
	ltdc_tfcon_cfg(vdmode);

	hardware_de_initialize(vdmode);
}

/* ожидаем начало кадра */
static void hardware_ltdc_vsync(void)
{

	DE_GLB_TypeDef * const glb = de3_getglb(RTMIXIX);
	if (glb == NULL)
		return;
	const uint_fast8_t state = (glb->GLB_STS >> 8) & 1;
	for (;;)
	{
		const uint_fast8_t state2 = (glb->GLB_STS >> 8) & 1;
		if (state != state2)
			break;
		hardware_nonguiyield();
	}
//	TCON_LCD0->LCD_GINT0_REG &= ~ (UINT32_C(1) << 15);         //clear LCD_VB_INT_FLAG
//	while((TCON_LCD0->LCD_GINT0_REG & (UINT32_C(1) << 15)) == 0) //wait  LCD_VB_INT_FLAG
//		hardware_nonguiyield();
}

/* Set MAIN frame buffer address. No waiting for VSYNC. */
/* Вызывается из display_flush, используется только в тестах */
void hardware_ltdc_main_set_no_vsync(uintptr_t p1)
{
	DE_BLD_TypeDef * const bld = de3_getbld(RTMIXIX);
	if (bld == NULL)
		return;
	//struct de_bld_t * const bld = (struct de_bld_t *) DE_BLD_BASE;

	t113_de_set_address_vi(p1, 1);
	// 5.10.9.1 BLD fill color control register
	// BLD_FILL_COLOR_CTL
	bld->BLD_EN_COLOR_CTL =
		((de3_getvi(RTMIXIX, 1) != NULL) * (p1 != 0) * VI_POS_BIT(1))	| // pipe0 enable - from VI1
		0;

	t113_de_update();
}

/* Set MAIN frame buffer address. Waiting for VSYNC. */
void hardware_ltdc_main_set4(uintptr_t layer0, uintptr_t layer1, uintptr_t layer2, uintptr_t layer3)
{
	DE_BLD_TypeDef * const bld = de3_getbld(RTMIXIX);
	if (bld == NULL)
		return;

	// Note: the layer priority is layer3>layer2>layer1>layer0
	t113_de_set_address_vi(layer0, 1);	// VI1
	t113_de_set_address_ui(layer1, 1);	// UI1
	t113_de_set_address_ui(layer2, 2);	// UI2
	t113_de_set_address_ui(layer3, 3);	// UI3

	// 5.10.9.1 BLD fill color control register
	// BLD_EN_COLOR_CTL
	bld->BLD_EN_COLOR_CTL =
		((de3_getvi(RTMIXIX, 1) != NULL) * (layer0 != 0) * VI_POS_BIT(1))	| // pipe0 enable - from VI1
		((de3_getui(RTMIXIX, 1) != NULL) * (layer1 != 0) * UI_POS_BIT(1))	| // pipe1 enable - from UI1
		((de3_getui(RTMIXIX, 2) != NULL) * (layer2 != 0) * UI_POS_BIT(2))	| // pipe1 enable - from UI2
		((de3_getui(RTMIXIX, 3) != NULL) * (layer3 != 0) * UI_POS_BIT(3))	| // pipe1 enable - from UI3
		0;

	hardware_ltdc_vsync();	/* ожидаем начало кадра */
	t113_de_update();
}

/* set visible buffer start. Wait VSYNC. */
void hardware_ltdc_main_set(uintptr_t p1)
{
	t113_de_set_address_vi(p1, 1);
	//t113_de_set_address_ui(p1, 1);
	DE_BLD_TypeDef * const bld = de3_getbld(RTMIXIX);
	if (bld == NULL)
		return;

	bld->BLD_EN_COLOR_CTL =
			((de3_getvi(RTMIXIX, 1) != NULL) * (p1 != 0) * VI_POS_BIT(1))	| // pipe0 enable - from VI1
			//((de3_getui(RTMIXIX, 1) != NULL) * (p1 != 0) * UI_POS_BIT(1))	| // pipe1 enable - from UI1
			0;

	hardware_ltdc_vsync();	/* ожидаем начало кадра */
	t113_de_update();
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
uint_fast32_t display_getdotclock(const videomode_t * vdmode)
{
	/* Accumulated parameters for this display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned HSYNC = vdmode->hsync;	/*  */
	const unsigned VSYNC = vdmode->vsync;	/*  */
	const unsigned LEFTMARGIN = HSYNC + vdmode->hbp;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = VSYNC + vdmode->vbp;	/* vertical delay before DE start */
	const unsigned HTOTAL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VTOTAL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

	return (uint_fast32_t) vdmode->fps * HTOTAL * VTOTAL;
	//return vdmode->ltdc_dotclk;
}

#else /* WITHLTDCHW */

uint_fast32_t display_getdotclock(const videomode_t * vdmode)
{
	return UINT32_C(1000000);
}

#endif /* WITHLTDCHW */

#if WITHGPUHW && (CPUSTYLE_T507 || CPUSTYLE_H616)
#define GPU_CTRLBASE (GPU_BASE + 0x10000)

#if WITHDEBUG

//	01810000  000000FF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF 0003FFFF
//	01810020  00000000 00000000 00000000 00000000 FFFFFFFF 00000007 00000000 00000000
//	01810040  00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
//	01810060  00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

void gpu_dump(void)
{
	int skip = 0;
	static uint8_t pattern [512];
	unsigned offs;
	for (offs = 0x8000 * 2; offs < 4 * 256 * 1024; offs += 512)
	{
		* (volatile uint32_t *) (GPU_BASE + offs) |= 1;
		if (memcmp(pattern, (void *) (GPU_BASE + offs), 512) == 0)
		{
			++ skip;
			PRINTF(".");
			continue;
		}
		if (skip)
		{
			skip = 0;
			PRINTF("\n");
		}
		printhex32(GPU_BASE + offs, (void *) (GPU_BASE + offs), 512);
		PRINTF("---\n");
	}
}
#endif /* WITHDEBUG */

void GPU_IRQHandler(void)
{
	PRINTF("GPU_IRQHandler\n");
}

// Graphic processor unit
void board_gpu_initialize(void)
{
	PRINTF("board_gpu_initialize start.\n");

	CCU->GPU_CLK1_REG |= (UINT32_C(1) << 31);	// PLL_PERI_BAK_CLK_GATING
	CCU->GPU_CLK0_REG |= (UINT32_C(1) << 31);	// SCLK_GATING

	CCU->GPU_BGR_REG |= (UINT32_C(1) << 0);	// Clock Gating
	CCU->GPU_BGR_REG &= ~ (UINT32_C(1) << 16);	// Assert Reset
	CCU->GPU_BGR_REG |= (UINT32_C(1) << 16);	// De-assert Reset

	PRINTF("allwnr_t507_get_gpu_freq()=%" PRIuFAST32 " MHz\n", allwnr_t507_get_gpu_freq() / 1000 / 1000);
	//memset((void *) GPU_CTRLBASE, 0xFF, 4);
	//gpu_dump();

//
	arm_hardware_set_handler_system(GPU_IRQn, GPU_IRQHandler);

	PRINTF("board_gpu_initialize done.\n");
}

#elif CPUSTYLE_STM32MP1

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
