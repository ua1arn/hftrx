/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// STM32xxx
//	LCD-TFT Controller (LTDC)
// RENESAS Video Display Controller 5
//	Video Display Controller 5 (5): Image Synthesizer
//	Video Display Controller 5 (7): Output Controller
// Allwinner
//	DE (Display Engine)
//	TCON (Timing Controller)

#include "hardware.h"
#include "formats.h"	// for debug prints

#if WITHLTDCHW

#define LTDCBIT32(x) (UINT32_C(1) << (x))

#include "display.h"
#include "audio.h"
#include "buffers.h"
#include <stdint.h>
#include <string.h>
#include <limits.h>

#include "utils.h"
#include "gpio.h"
#include "clocks.h"
#include "src/touch/touch.h"

#define WITHLVDSHW (WITHFLATLINK && defined (HARDWARE_LVDS_INITIALIZE))
#define WITHDSIHW (WITHMIPIDSISHW && defined (HARDWARE_MIPIDSI_INITIALIZE))
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

static void hardware_ltdc_vsync(int rtmixid);

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

	const unsigned MAINROWSIZE = sizeof (PACKEDCOLORPIP_T) * GXADJ(DIM_X);	// размер одной строки в байтах
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
hardware_ltdc_initialize(const videomode_t * vdmode)
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
void hardware_ltdc_main_set_no_vsync(int rtmixid, uintptr_t addr)
{
	struct st_vdc5 * const vdc = & VDC50;

	SETREG32_CK(& vdc->GR2_FLM_RD, 1, 0, 1);		// GR2_R_ENB Frame Buffer Read Enable 1: Frame buffer reading is enabled.
	SETREG32_CK(& vdc->GR2_FLM2, 32, 0, addr);			// GR2_BASE
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
static void hardware_ltdc_vsync(int rtmixid)
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
void hardware_ltdc_main_set(int rtmixid, uintptr_t p)
{
	struct st_vdc5 * const vdc = & VDC50;

	SETREG32_CK(& vdc->GR2_FLM_RD, 1, 0, 1);		// GR2_R_ENB Frame Buffer Read Enable 1: Frame buffer reading is enabled.
	SETREG32_CK(& vdc->GR2_FLM2, 32, 0, p);			// GR2_BASE
	SETREG32_CK(& vdc->GR2_AB1, 2, 0,	0x02);		// GR2_DISP_SEL 2: Current graphics display

	hardware_ltdc_vsync(rtmixid);	/* ожидаем начало кадра */
}

/* Set MAIN frame buffer address. Waiting for VSYNC. */
void hardware_ltdc_main_set4(int rtmixid, uintptr_t layer0, uintptr_t layer1, uintptr_t layer2, uintptr_t layer3)
{
	ASSERT(layer2 == 0);
	ASSERT(layer3 == 0);
	hardware_ltdc_pip_off();
	hardware_ltdc_main_set(rtmixid, layer0);
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
	LTDC_Layerx->WHPCR &= ~ (LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS);
	LTDC_Layerx->WHPCR |= (LTDC_Layer_InitStruct->LTDC_HorizontalStart | whsppos);

	/* Configures the vertical start and stop position */
	wvsppos = LTDC_Layer_InitStruct->LTDC_VerticalStop << LTDC_LxWVPCR_WVSPPOS_Pos;
	LTDC_Layerx->WVPCR &= ~ (LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS);
	LTDC_Layerx->WVPCR |= (LTDC_Layer_InitStruct->LTDC_VerticalStart | wvsppos);

	/* Specifies the pixel format */
	LTDC_Layerx->PFCR &= ~ (LTDC_LxPFCR_PF);
	LTDC_Layerx->PFCR |= (LTDC_Layer_InitStruct->LTDC_PixelFormat);

	/* Configures the default color values */
	LTDC_Layerx->DCCR = LTDC_Layer_InitStruct->LTDC_DefaultColor;

	/* Specifies the constant alpha value */      
	// alpha канал если в видеобуфере не хранится значение в каждом пикселе
	LTDC_Layerx->CACR = (LTDC_Layerx->CACR & ~ (LTDC_LxCACR_CONSTA)) |
		(LTDC_Layer_InitStruct->LTDC_ConstantAlpha << LTDC_LxCACR_CONSTA_Pos) |
		0;

	/* Specifies the blending factors */
	LTDC_Layerx->BFCR &= ~ (LTDC_LxBFCR_BF2 | LTDC_LxBFCR_BF1);
	LTDC_Layerx->BFCR |= (LTDC_Layer_InitStruct->LTDC_BlendingFactor_1 | LTDC_Layer_InitStruct->LTDC_BlendingFactor_2);

	/* Configures the color frame buffer start address */
//	LTDC_Layerx->CFBAR &= ~ (LTDC_LxCFBAR_CFBADD);
//	LTDC_Layerx->CFBAR |= (LTDC_Layer_InitStruct->LTDC_CFBStartAdress);

	/* Configures the color frame buffer pitch in byte */
	cfbp = (LTDC_Layer_InitStruct->LTDC_CFBPitch << LTDC_LxCFBLR_CFBP_Pos);
	LTDC_Layerx->CFBLR  &= ~ (LTDC_LxCFBLR_CFBLL | LTDC_LxCFBLR_CFBP);
	LTDC_Layerx->CFBLR  |= (LTDC_Layer_InitStruct->LTDC_CFBLineLength | cfbp);

	/* Configures the frame buffer line number */
	LTDC_Layerx->CFBLNR  &= ~ (LTDC_LxCFBLNR_CFBLNBR);
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
	LTDC->SSCR &= ~ (LTDC_SSCR_VSH | LTDC_SSCR_HSW);
	horizontalsync = (LTDC_InitStruct->LTDC_HorizontalSync << LTDC_SSCR_HSW_Pos);
	LTDC->SSCR |= (horizontalsync | LTDC_InitStruct->LTDC_VerticalSync);

	/* Sets Accumulated Back porch */
	LTDC->BPCR &= ~ (LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);
	accumulatedHBP = (LTDC_InitStruct->LTDC_AccumulatedHBP << LTDC_BPCR_AHBP_Pos);
	LTDC->BPCR |= (accumulatedHBP | LTDC_InitStruct->LTDC_AccumulatedVBP);

	/* Sets Accumulated Active Width */
	LTDC->AWCR &= ~ (LTDC_AWCR_AAH | LTDC_AWCR_AAW);
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
	Active high width         = DIM_X 
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
hardware_ltdc_initialize(const videomode_t * vdmode)
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

	pipparams_t mainwnd = { 0, 0, DIM_X, DIM_Y };

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
		;
	LTDC->SRCR |= LTDC_SRCR_IMR_Msk;	/*!< Immediately Reload. */
	(void) LTDC->SRCR;
	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		;

	/* Enable the LTDC */
	LTDC->GCR |= LTDC_GCR_LTDCEN;

#if LCDMODE_MAIN_L24
	fillLUT_L24(LAYER_MAIN);	// прямая трансляция всех байтов из памяти на выход. загрузка палитры - имеет смысл до Reload
#elif LCDMODE_MAIN_L8
	fillLUT_L8(LAYER_MAIN, xltrgb24);	// загрузка палитры - имеет смысл до Reload
#endif /* LCDMODE_MAIN_L8 */

	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		;
	/* LTDC reload configuration */  
	LTDC->SRCR |= LTDC_SRCR_IMR_Msk;	/* Immediately Reload. */
	(void) LTDC->SRCR;
	while ((LTDC->SRCR & LTDC_SRCR_IMR_Msk) != 0)
		;

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
		;
	LTDC->SRCR |= LTDC_SRCR_VBR_Msk;	/* Vertical Blanking Reload. */
	(void) LTDC->SRCR;
	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		;
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
		;
#endif /* */
}


/* Set MAIN frame buffer address. No waiting for VSYNC. */
void hardware_ltdc_main_set_no_vsync(int rtmixid, uintptr_t addr)
{
	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
//	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
//		;
	LAYER_MAIN->CFBAR = addr;
	(void) LAYER_MAIN->CFBAR;
	LAYER_MAIN->CR |= LTDC_LxCR_LEN_Msk;
	(void) LAYER_MAIN->CR;

	LTDC->SRCR |= LTDC_SRCR_IMR_Msk;	/* Immediate Reload. */
	(void) LTDC->SRCR;
	while ((LTDC->SRCR & LTDC_SRCR_IMR_Msk) != 0)
		;
}

/* ожидаем начало кадра */
static void hardware_ltdc_vsync(int rtmixid)
{
	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		;

	LTDC->SRCR |= LTDC_SRCR_VBR_Msk;	/* Vertical Blanking Reload. */
	(void) LTDC->SRCR;

	/* дождаться, пока не будет использовано ранее заказанное переключение отображаемой страницы экрана */
	while ((LTDC->SRCR & (LTDC_SRCR_VBR_Msk | LTDC_SRCR_IMR_Msk)) != 0)
		;
}

/* Set MAIN frame buffer address. Wait for VSYNC. */
void hardware_ltdc_main_set(int rtmixid, uintptr_t p)
{
	LAYER_MAIN->CFBAR = p;

	(void) LAYER_MAIN->CFBAR;
	LAYER_MAIN->CR |= LTDC_LxCR_LEN_Msk;
	(void) LAYER_MAIN->CR;

	hardware_ltdc_vsync(rtmixid);
}

/* Set MAIN frame buffer address. Waiting for VSYNC. */
void hardware_ltdc_main_set4(int rtmixid, uintptr_t layer0, uintptr_t layer1, uintptr_t layer2, uintptr_t layer3)
{
	ASSERT(layer2 == 0);
	ASSERT(layer3 == 0);
	hardware_ltdc_pip_off();
	hardware_ltdc_main_set(rtmixid, layer0);
}

#elif LINUX_SUBSYSTEM && WITHFBDEV && ! WITHLVGL

void hardware_ltdc_initialize(const videomode_t * vdmode)
{
	linux_framebuffer_init();
}

/* Palette reload (dummy fuction) */
void hardware_ltdc_L8_palette(void)
{
}

/* Set MAIN frame buffer address. No waiting for VSYNC. */
void hardware_ltdc_main_set_no_vsync(int rtmixid, uintptr_t addr)
{
	uint32_t size;
	uint32_t * linux_fb = linux_get_fb(& size);
	memcpy(linux_fb, (uint32_t *) addr, size);
}

/* Set MAIN frame buffer address. */
void hardware_ltdc_main_set(int rtmixid, uintptr_t addr)
{
	uint32_t size;
	uint32_t * linux_fb = linux_get_fb(& size);
	memcpy(linux_fb, (uint32_t *) addr, size);
}

/* ожидаем начало кадра */
static void hardware_ltdc_vsync(int rtmixid)
{
}

#elif LINUX_SUBSYSTEM && WITHSDL2VIDEO

void hardware_ltdc_initialize(const videomode_t * vdmode)
{
	ASSERT(sdl2_render_init());
}

/* Palette reload (dummy fuction) */
void hardware_ltdc_L8_palette(void)
{
}

/* Set MAIN frame buffer address. No waiting for VSYNC. */
void hardware_ltdc_main_set_no_vsync(int rtmixid, uintptr_t addr)
{
	sdl2_render_update(addr);
}

/* Set MAIN frame buffer address. */
void hardware_ltdc_main_set(int rtmixid, uintptr_t addr)
{
	sdl2_render_update(addr);
}

/* ожидаем начало кадра */
static void hardware_ltdc_vsync(int rtmixid)
{
}

#elif (CPUSTYLE_XC7Z) && ! LINUX_SUBSYSTEM && ! WITHLVGL

#include "zynq_vdma.h"

static DisplayCtrl dispCtrl;

void hardware_ltdc_initialize(const videomode_t * vdmode)
{
	int Status;
	static XAxiVdma AxiVdma;
	uintptr_t frames [LCDMODE_MAIN_PAGES];

	colmain_fb_list(frames);		// получение массива планирующихся для работы framebuffers

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
void hardware_ltdc_main_set_no_vsync(int rtmixid, uintptr_t addr)
{
	DisplayChangeFrame(&dispCtrl, colmain_getindexbyaddr(addr));
}

/* Set MAIN frame buffer address. */
void hardware_ltdc_main_set(int rtmixid, uintptr_t addr)
{
	(void) rtmixid;
	DisplayChangeFrame(&dispCtrl, colmain_getindexbyaddr(addr));
}

/* ожидаем начало кадра */
static void hardware_ltdc_vsync(int rtmixid)
{
}

/* Set MAIN frame buffer address. Waiting for VSYNC. */
void hardware_ltdc_main_set4(int rtmixid, uintptr_t layer0, uintptr_t layer1, uintptr_t layer2, uintptr_t layer3)
{
	ASSERT(layer2 == 0);
	ASSERT(layer3 == 0);
	//hardware_ltdc_pip_off();
	hardware_ltdc_main_set(rtmixid, layer0);
}

#elif CPUSTYLE_ALLWINNER

#define UI_CFG_INDEX 0	/* 0..3 используется одна конфигурация */
#define VI_CFG_INDEX 0


#if CPUSTYLE_T507 || CPUSTYLE_H616

// Требуется заполнение в соответствии с инициализацией DE_PORT2CHN_MUX

//	RTMIX0: VI1, UI1
//	RTMIX1: VI2, UI2

static DE_VI_TypeDef * rtmix0_vimap [] =
{
		DE_VI1,
		NULL,
		NULL,
};

static DE_VI_TypeDef * rtmix1_vimap [] =
{
		DE_VI2,
		NULL,
		NULL,
};

static DE_UI_TypeDef * rtmix0_uimap [] =
{
		DE_UI1,
		NULL,
		NULL,
};

static DE_UI_TypeDef * rtmix1_uimap [] =
{
		DE_UI2,
		NULL,
		NULL,
};

#elif CPUSTYLE_T113 || CPUSTYLE_F133

static DE_VI_TypeDef * const rtmix0_vimap [] =
{
		DE_MIXER0_VI1,
		NULL,
		NULL,
};

static DE_VI_TypeDef * const rtmix1_vimap [] =
{
		DE_MIXER1_VI1,
		NULL,
		NULL,
};

static DE_UI_TypeDef * const rtmix0_uimap [] =
{
		DE_MIXER0_UI1,
		NULL,
		NULL,
};

static DE_UI_TypeDef * const rtmix1_uimap [] =
{
		NULL,
		NULL,
		NULL,
};


#elif CPUSTYLE_A64 || CPUSTYLE_H3

static DE_VI_TypeDef * const rtmix0_vimap [] =
{
		DE_MIXER0_VI1,
};

static DE_VI_TypeDef * const rtmix1_vimap [] =
{
		DE_MIXER1_VI1,
};

static DE_UI_TypeDef * const rtmix0_uimap [] =
{
		DE_MIXER0_UI1,
		DE_MIXER0_UI2,
		DE_MIXER0_UI3,
};

static DE_UI_TypeDef * const rtmix1_uimap [] =
{
		DE_MIXER1_UI1,
		DE_MIXER1_UI2,
		DE_MIXER1_UI3,
};

#else
	#error Unsupported CPUSTYLE_xxx
#endif

#if CPUSTYLE_T113 || CPUSTYLE_F133
	#define VI_LASTIX(rtmixid) 1
	#define UI_LASTIX(rtmixid) 1	// В RT-Mixer 1 отсутствуют UI
	/* BLD_EN_COLOR_CTL positions 8..11 */
#elif CPUSTYLE_T507 || CPUSTYLE_H616
	#define VI_LASTIX(rtmixid) 1
	#define UI_LASTIX(rtmixid) 1
	/* BLD_EN_COLOR_CTL positions 8..13 */
#elif CPUSTYLE_A64 || CPUSTYLE_H3
	#define VI_LASTIX(rtmixid) 1
	#define UI_LASTIX(rtmixid) 3
	/* BLD_EN_COLOR_CTL positions 8..13 */
#else
	#error Unexpected CPUSTYLE_xxx
	#define VI_LASTIX(rtmixid) 1
	#define UI_LASTIX(rtmixid) 1
#endif

#define VI_POS_BIT(rtmixid, vi) (UINT32_C(1) << ((vi) + 8 - 1))
#define UI_POS_BIT(rtmixid, ui) (UINT32_C(1) << ((ui) + (8 + VI_LASTIX(rtmixid)) - 1))


#ifndef SETMASK
#define SETMASK(width, shift)   (((width)?((-INT32_C(1)) >> (32-(width))):0)  << (shift))
#endif

#ifndef CLRMASK
#define CLRMASK(width, shift)   (~ (SETMASK((width), (shift))))
#endif

#ifndef GET_BITS
#define GET_BITS(shift, width, reg) (((reg)&SETMASK((width), (shift))) >> (shift))
#endif

#ifndef SET_BITS
#define SET_BITS(shift, width, reg, val)                                       \
	(((reg)&CLRMASK((width), (shift))) | ((val) << (shift)))
#endif


static int32_t de_feat_get_num_vi_chns(uint32_t disp)
{
	return 3;
	const unsigned rtmixid = disp + 1;
	return (VI_LASTIX(rtmixid) - 1) + 1;
}

static int32_t de_feat_get_num_chns(uint32_t disp)
{
	return 6;
	const unsigned rtmixid = disp + 1;
	return (VI_LASTIX(rtmixid) - 1) + 1 + (UI_LASTIX(rtmixid) - 1) + 1;
}

struct lcd_timing
{
	uint16_t hp;
	uint16_t vp;
	uint16_t hbp;
	uint16_t vbp;
	uint16_t hspw;
	uint16_t vspw;
};

#if CPUSTYLE_T113 || CPUSTYLE_F133


#elif CPUSTYLE_T507 || CPUSTYLE_H616

	// https://github.com/RMerl/asuswrt-merlin.ng/blob/master/release/src-rt-5.04axhnd.675x/bootloaders/u-boot-2019.07/arch/arm/include/asm/arch-sunxi/display2.h#L16
	// struct de_clk
	//

#define DE_TOP_REG_OFFSET (0x8000)
#define DE_TOP_REG_SIZE   (0x0220)

#define DE_TOP_RTWB_OFFSET (0x010000)

#define DE_TOP_RTMX_OFFSET (0x100000)

#define DE_CHN_SIZE            (0x20000) /* 128K */
#define DE_CHN_OFFSET(phy_chn) (0x100000 + DE_CHN_SIZE * (phy_chn))

//	#define DE_BLD_BASE ((uintptr_t) 0x01281000)          /*!< DE_BLD Display Engine (DE) - Blender Base */
//	#define DEb_BLD_BASE ((uintptr_t) 0x012A1000)         /*!< DE_BLD Display Engine (DE) - Blender Base */
#define DE_DISP_SIZE           (0x20000) /* 128K */
#define DE_DISP_OFFSET(disp)   (0x280000 + DE_DISP_SIZE * (disp))

#define CHN_CCSC_OFFSET    (0x00800)

// #define DE_VI1_BASE ((uintptr_t) 0x01101000)          /*!< DE_VI Display Engine (DE) - VI surface Base */
#define CHN_OVL_OFFSET     (0x01000)

#define CHN_DBV_OFFSET     (0x02000)

// #define DE_VSU_BASE ((uintptr_t) 0x01104000)          /*!< DE_VSU Video Scaler Unit (VSU) Base */
#define CHN_SCALER_OFFSET  (0x04000)

#define CHN_FBD_ATW_OFFSET (0x05000)
#define CHN_CDC_OFFSET     (0x08000)

// #define DE_FCE_BASE ((uintptr_t) 0x01110000)          /*!< DE_FCE Fresh and Contrast Enhancement (FCE) Base */
#define CHN_FCE_OFFSET     (0x10000)

#define CHN_PEAK_OFFSET    (0x10800)
#define CHN_LTI_OFFSET     (0x10C00)

// #define DE_BLS_BASE ((uintptr_t) 0x01111000)          /*!< DE_BLS Blue Level Stretch (BLS) Base */
#define CHN_BLS_OFFSET     (0x11000)

// #define DE_FCC_BASE ((uintptr_t) 0x01111400)          /*!< DE_FCC Fancy color curvature (FCC) Base */
#define CHN_FCC_OFFSET     (0x11400)

// #define DE_DNS_BASE ((uintptr_t) 0x01114000)          /*!< DE_DNS Denoise (DNS) Base */
#define CHN_DNS_OFFSET     (0x14000)

#define CHN_DI300_OFFSET   (0x14400)
#define CHN_SNR_OFFSET     (0x14000)

// #define DE_BLD_BASE ((uintptr_t) 0x01281000)          /*!< DE_BLD Display Engine (DE) - Blender Base */
#define DISP_BLD_OFFSET    (0x01000)

#define DISP_DEP_OFFSET    (0x02000)
#define DISP_DB3_OFFSET    (0x03000)
#define DISP_FMT_OFFSET    (0x05000)
#define DISP_DSC_OFFSET    (0x06000)
#define DISP_KSC_OFFSET    (0x08000)

#define RTWB_WB_OFFSET     (0x01000)
#define RTWB_CDC_OFFSET    (0x08000)

#define DE_MBUS_CLOCK_ADDR           (0x8008)
#define DE2TCON_MUX_OFFSET           (0x8010)
#define DE_VER_CTL_OFFSET            (0x8014)
#define DE_RTWB_MUX_OFFSET           (0x8020)
//#define DE_CHN2CORE_MUX_OFFSET       (0x8024)
//#define DE_PORT2CHN_MUX_OFFSET(disp) (0x8028 + (disp) * 0x4)
#define DE_DEBUG_CTL_OFFSET          (0x80E0)
#define RTMX_GLB_CTL_OFFSET(disp)    (0x8100 + (disp) * 0x40)
#define RTMX_GLB_STS_OFFSET(disp)    (0x8104 + (disp) * 0x40)
#define RTMX_OUT_SIZE_OFFSET(disp)   (0x8108 + (disp) * 0x40)
#define RTMX_AUTO_CLK_OFFSET(disp)   (0x810C + (disp) * 0x40)
#define RTMX_RCQ_CTL_OFFSET(disp)    (0x8110 + (disp) * 0x40)

#define RTWB_RCQ_IRQ_OFFSET          (0x8200)
#define RTWB_RCQ_STS_OFFSET          (0x8204)
#define RTWB_RCQ_CTL_OFFSET          (0x8210)

//
// D:\user\Allwinner-H616-develop\kernel\drivers\gpu\drm\sun4i\sun8i_dw_hdmi.h

#define SUN8I_HDMI_PHY_DBG_CTRL_REG 0x0000
#define SUN8I_HDMI_PHY_DBG_CTRL_PX_LOCK LTDCBIT32(0)
#define SUN8I_HDMI_PHY_DBG_CTRL_POL_MASK GENMASK(15, 8)
#define SUN8I_HDMI_PHY_DBG_CTRL_POL_NHSYNC LTDCBIT32(8)
#define SUN8I_HDMI_PHY_DBG_CTRL_POL_NVSYNC LTDCBIT32(9)
#define SUN8I_HDMI_PHY_DBG_CTRL_ADDR_MASK GENMASK(23, 16)
#define SUN8I_HDMI_PHY_DBG_CTRL_ADDR(addr) (addr << 16)

#define SUN8I_HDMI_PHY_REXT_CTRL_REG 0x0004
#define SUN8I_HDMI_PHY_REXT_CTRL_REXT_EN LTDCBIT32(31)

#define SUN8I_HDMI_PHY_READ_EN_REG 0x0010
#define SUN8I_HDMI_PHY_READ_EN_MAGIC 0x54524545

#define SUN8I_HDMI_PHY_UNSCRAMBLE_REG 0x0014
#define SUN8I_HDMI_PHY_UNSCRAMBLE_MAGIC 0x42494E47

#define SUN8I_HDMI_PHY_ANA_CFG1_REG 0x0020
#define SUN8I_HDMI_PHY_ANA_CFG1_REG_SWI LTDCBIT32(31)
#define SUN8I_HDMI_PHY_ANA_CFG1_REG_PWEND LTDCBIT32(30)
#define SUN8I_HDMI_PHY_ANA_CFG1_REG_PWENC LTDCBIT32(29)
#define SUN8I_HDMI_PHY_ANA_CFG1_REG_CALSW LTDCBIT32(28)
#define SUN8I_HDMI_PHY_ANA_CFG1_REG_SVRCAL(x) ((x) << 26)
#define SUN8I_HDMI_PHY_ANA_CFG1_REG_SVBH(x) ((x) << 24)
#define SUN8I_HDMI_PHY_ANA_CFG1_AMP_OPT LTDCBIT32(23)
#define SUN8I_HDMI_PHY_ANA_CFG1_EMP_OPT LTDCBIT32(22)
#define SUN8I_HDMI_PHY_ANA_CFG1_AMPCK_OPT LTDCBIT32(21)
#define SUN8I_HDMI_PHY_ANA_CFG1_EMPCK_OPT LTDCBIT32(20)
#define SUN8I_HDMI_PHY_ANA_CFG1_ENRCAL LTDCBIT32(19)
#define SUN8I_HDMI_PHY_ANA_CFG1_ENCALOG LTDCBIT32(18)
#define SUN8I_HDMI_PHY_ANA_CFG1_REG_SCKTMDS LTDCBIT32(17)
#define SUN8I_HDMI_PHY_ANA_CFG1_TMDSCLK_EN LTDCBIT32(16)
#define SUN8I_HDMI_PHY_ANA_CFG1_TXEN_MASK GENMASK(15, 12)
#define SUN8I_HDMI_PHY_ANA_CFG1_TXEN_ALL (0xf << 12)
#define SUN8I_HDMI_PHY_ANA_CFG1_BIASEN_TMDSCLK LTDCBIT32(11)
#define SUN8I_HDMI_PHY_ANA_CFG1_BIASEN_TMDS2 LTDCBIT32(10)
#define SUN8I_HDMI_PHY_ANA_CFG1_BIASEN_TMDS1 LTDCBIT32(9)
#define SUN8I_HDMI_PHY_ANA_CFG1_BIASEN_TMDS0 LTDCBIT32(8)
#define SUN8I_HDMI_PHY_ANA_CFG1_ENP2S_TMDSCLK LTDCBIT32(7)
#define SUN8I_HDMI_PHY_ANA_CFG1_ENP2S_TMDS2 LTDCBIT32(6)
#define SUN8I_HDMI_PHY_ANA_CFG1_ENP2S_TMDS1 LTDCBIT32(5)
#define SUN8I_HDMI_PHY_ANA_CFG1_ENP2S_TMDS0 LTDCBIT32(4)
#define SUN8I_HDMI_PHY_ANA_CFG1_CKEN LTDCBIT32(3)
#define SUN8I_HDMI_PHY_ANA_CFG1_LDOEN LTDCBIT32(2)
#define SUN8I_HDMI_PHY_ANA_CFG1_ENVBS LTDCBIT32(1)
#define SUN8I_HDMI_PHY_ANA_CFG1_ENBI LTDCBIT32(0)

#define SUN8I_HDMI_PHY_ANA_CFG2_REG 0x0024
#define SUN8I_HDMI_PHY_ANA_CFG2_M_EN LTDCBIT32(31)
#define SUN8I_HDMI_PHY_ANA_CFG2_PLLDBEN LTDCBIT32(30)
#define SUN8I_HDMI_PHY_ANA_CFG2_SEN LTDCBIT32(29)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_HPDPD LTDCBIT32(28)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_HPDEN LTDCBIT32(27)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_PLRCK LTDCBIT32(26)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_PLR(x) ((x) << 23)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_DENCK LTDCBIT32(22)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_DEN LTDCBIT32(21)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_CD(x) ((x) << 19)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_CKSS(x) ((x) << 17)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_BIGSWCK LTDCBIT32(16)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_BIGSW LTDCBIT32(15)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_CSMPS(x) ((x) << 13)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_SLV(x) ((x) << 10)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_BOOSTCK(x) ((x) << 8)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_BOOST(x) ((x) << 6)
#define SUN8I_HDMI_PHY_ANA_CFG2_REG_RESDI(x) ((x) << 0)

#define SUN8I_HDMI_PHY_ANA_CFG3_REG 0x0028
#define SUN8I_HDMI_PHY_ANA_CFG3_REG_SLOWCK(x) ((x) << 30)
#define SUN8I_HDMI_PHY_ANA_CFG3_REG_SLOW(x) ((x) << 28)
#define SUN8I_HDMI_PHY_ANA_CFG3_REG_WIRE(x) ((x) << 18)
#define SUN8I_HDMI_PHY_ANA_CFG3_REG_AMPCK(x) ((x) << 14)
#define SUN8I_HDMI_PHY_ANA_CFG3_REG_EMPCK(x) ((x) << 11)
#define SUN8I_HDMI_PHY_ANA_CFG3_REG_AMP(x) ((x) << 7)
#define SUN8I_HDMI_PHY_ANA_CFG3_REG_EMP(x) ((x) << 4)
#define SUN8I_HDMI_PHY_ANA_CFG3_SDAPD LTDCBIT32(3)
#define SUN8I_HDMI_PHY_ANA_CFG3_SDAEN LTDCBIT32(2)
#define SUN8I_HDMI_PHY_ANA_CFG3_SCLPD LTDCBIT32(1)
#define SUN8I_HDMI_PHY_ANA_CFG3_SCLEN LTDCBIT32(0)

#define SUN8I_HDMI_PHY_PLL_CFG1_REG 0x002c
#define SUN8I_HDMI_PHY_PLL_CFG1_REG_OD1 LTDCBIT32(31)
#define SUN8I_HDMI_PHY_PLL_CFG1_REG_OD LTDCBIT32(30)
#define SUN8I_HDMI_PHY_PLL_CFG1_LDO2_EN LTDCBIT32(29)
#define SUN8I_HDMI_PHY_PLL_CFG1_LDO1_EN LTDCBIT32(28)
#define SUN8I_HDMI_PHY_PLL_CFG1_HV_IS_33 LTDCBIT32(27)
#define SUN8I_HDMI_PHY_PLL_CFG1_CKIN_SEL_MSK LTDCBIT32(26)
#define SUN8I_HDMI_PHY_PLL_CFG1_CKIN_SEL_SHIFT 26
#define SUN8I_HDMI_PHY_PLL_CFG1_PLLEN LTDCBIT32(25)
#define SUN8I_HDMI_PHY_PLL_CFG1_LDO_VSET(x) ((x) << 22)
#define SUN8I_HDMI_PHY_PLL_CFG1_UNKNOWN(x) ((x) << 20)
#define SUN8I_HDMI_PHY_PLL_CFG1_PLLDBEN LTDCBIT32(19)
#define SUN8I_HDMI_PHY_PLL_CFG1_CS LTDCBIT32(18)
#define SUN8I_HDMI_PHY_PLL_CFG1_CP_S(x) ((x) << 13)
#define SUN8I_HDMI_PHY_PLL_CFG1_CNT_INT(x) ((x) << 7)
#define SUN8I_HDMI_PHY_PLL_CFG1_BWS LTDCBIT32(6)
#define SUN8I_HDMI_PHY_PLL_CFG1_B_IN_MSK GENMASK(5, 0)
#define SUN8I_HDMI_PHY_PLL_CFG1_B_IN_SHIFT 0

#define SUN8I_HDMI_PHY_PLL_CFG2_REG 0x0030
#define SUN8I_HDMI_PHY_PLL_CFG2_SV_H LTDCBIT32(31)
#define SUN8I_HDMI_PHY_PLL_CFG2_PDCLKSEL(x) ((x) << 29)
#define SUN8I_HDMI_PHY_PLL_CFG2_CLKSTEP(x) ((x) << 27)
#define SUN8I_HDMI_PHY_PLL_CFG2_PSET(x) ((x) << 24)
#define SUN8I_HDMI_PHY_PLL_CFG2_PCLK_SEL LTDCBIT32(23)
#define SUN8I_HDMI_PHY_PLL_CFG2_AUTOSYNC_DIS LTDCBIT32(22)
#define SUN8I_HDMI_PHY_PLL_CFG2_VREG2_OUT_EN LTDCBIT32(21)
#define SUN8I_HDMI_PHY_PLL_CFG2_VREG1_OUT_EN LTDCBIT32(20)
#define SUN8I_HDMI_PHY_PLL_CFG2_VCOGAIN_EN LTDCBIT32(19)
#define SUN8I_HDMI_PHY_PLL_CFG2_VCOGAIN(x) ((x) << 16)
#define SUN8I_HDMI_PHY_PLL_CFG2_VCO_S(x) ((x) << 12)
#define SUN8I_HDMI_PHY_PLL_CFG2_VCO_RST_IN LTDCBIT32(11)
#define SUN8I_HDMI_PHY_PLL_CFG2_SINT_FRAC LTDCBIT32(10)
#define SUN8I_HDMI_PHY_PLL_CFG2_SDIV2 LTDCBIT32(9)
#define SUN8I_HDMI_PHY_PLL_CFG2_S(x) ((x) << 6)
#define SUN8I_HDMI_PHY_PLL_CFG2_S6P25_7P5 LTDCBIT32(5)
#define SUN8I_HDMI_PHY_PLL_CFG2_S5_7 LTDCBIT32(4)
#define SUN8I_HDMI_PHY_PLL_CFG2_PREDIV_MSK GENMASK(3, 0)
#define SUN8I_HDMI_PHY_PLL_CFG2_PREDIV_SHIFT 0
#define SUN8I_HDMI_PHY_PLL_CFG2_PREDIV(x) (((x)-1) << 0)

#define SUN8I_HDMI_PHY_PLL_CFG3_REG 0x0034
#define SUN8I_HDMI_PHY_PLL_CFG3_SOUT_DIV2 LTDCBIT32(0)

#define SUN8I_HDMI_PHY_ANA_STS_REG 0x0038
#define SUN8I_HDMI_PHY_ANA_STS_B_OUT_SHIFT 11
#define SUN8I_HDMI_PHY_ANA_STS_B_OUT_MSK GENMASK(16, 11)
#define SUN8I_HDMI_PHY_ANA_STS_RCALEND2D LTDCBIT32(7)
#define SUN8I_HDMI_PHY_ANA_STS_RCAL_MASK GENMASK(5, 0)

#define SUN8I_HDMI_PHY_CEC_REG 0x003c

#if 0
static int32_t de_top_set_chn2core_mux(
	uint32_t phy_chn, uint32_t phy_disp)
{
	//uintptr_t reg_base;
	uint32_t reg_val;
	uint32_t width = 2;
	uint32_t shift = phy_chn * 2;

	//reg_base = DE_BASE + DE_CHN2CORE_MUX_OFFSET;
	//reg_val = readl(reg_base);
	reg_val = DE_TOP->DE_CHN2CORE_MUX;

	reg_val = SET_BITS(shift, width, reg_val, phy_disp);

	//writel(reg_val, reg_base);
	DE_TOP->DE_CHN2CORE_MUX = reg_val;
	return 0;
}

static int32_t de_top_set_port2chn_mux(uint32_t phy_disp,
	uint32_t port, uint32_t phy_chn)
{
//	uintptr_t reg_base = DE_BASE
//		+ DE_PORT2CHN_MUX_OFFSET(phy_disp);
	uint32_t width = 4;
	uint32_t shift = port * 4;
	//uint32_t reg_val = readl(reg_base);
	uint32_t reg_val = DE_TOP->DE_PORT2CHN_MUX [phy_disp];

	reg_val = SET_BITS(shift, width, reg_val, phy_chn);
	//writel(reg_val, reg_base);
	DE_TOP->DE_PORT2CHN_MUX [phy_disp] = reg_val;
	return 0;
}

////


static int32_t de_feat_get_phymap_chn_id(uint32_t disp, uint32_t chn)
{
	return chn < 3 ? chn : (chn - 3) | 0x08;
}

static int32_t de_feat_get_phy_chn_id(uint32_t disp, uint32_t chn)
{
#ifdef SUPPORT_FEAT_INIT_CONFIG

	int32_t phy_chn = -1;
	if ((disp < de_feat_get_num_screens())
		&& (chn < de_feat_get_num_chns(disp))) {
		uint32_t index = 0;
		uint32_t i;
		for (i = 0; i < disp; ++i)
			index += de_feat_get_num_chns(i);
		index += chn;
		phy_chn = de_cur_features->chn_id_lut[index];
	}
	return phy_chn;

#else

	return chn < 3 ? chn : chn + 3;	// T507/H616 specific

#endif /* #if SUPPORT_FEAT_INIT_CONFIG */
}

static int32_t de_rtmx_set_chn_mux(uint32_t disp)
{
	uint32_t chn, chn_num, v_chn_num;

	chn_num = de_feat_get_num_chns(disp);
	for (chn = 0; chn < chn_num; ++chn) {
		uint32_t phy_chn, phymap_chn;
		//phy_chn = de_feat_get_phy_chn_id(disp, chn);
		phymap_chn = de_feat_get_phymap_chn_id(disp, chn);
		de_top_set_chn2core_mux(phymap_chn, disp);
		de_top_set_port2chn_mux(disp, chn, phymap_chn);
	}

	return 0;
}
#endif
#else
	//#error Undefined CPUSTYLE_xxx
#endif

#if WITHHDMITVHW

// 1 - ok, 0 - err
static int hdmi_i2c_read(HDMI_TX_TypeDef * const hdmi, unsigned slave, unsigned start, uint8_t * buff, unsigned len)
{
	hdmi->HDMI_I2CM_SLAVE = slave;	// monitor address
	unsigned i;
	hdmi->HDMI_IH_I2CM_STAT0 = 0x03;	// Сброс флагов прерываний, если остались от предидущего обмена
	//const unsigned len = 0x80;
	for (i = 0; i < len; ++ i, ++ start)
	{
		hdmi->HDMI_I2CM_ADDRESS = start;
		hdmi->HDMI_I2CM_OPERATION = 0x01;	// read operation start
		for (;;)
		{
			const uint_fast8_t sts = hdmi->HDMI_IH_I2CM_STAT0;
			if (sts & 0x01)
			{
				// Error
				hdmi->HDMI_IH_I2CM_STAT0 = 0x01;
				return 0;
			}
			if (sts & 0x02)
			{
				// Done
				hdmi->HDMI_IH_I2CM_STAT0 = 0x02;
				buff [i] = hdmi->HDMI_I2CM_DATAI;
				break;
			}
		}
	}
	return 1;
}
// https://v2020e.ru/blog/emulyatsiya-edid-informatsii-hdmi-interfejsa-na-fpga
// Taken from https://github.com/MYIR-ALLWINNER/myir-t5-kernel/blob/develop/drivers/gpu/drm/bridge/dw-hdmi.h#L14

// 1 - ok, 0 - err
static int hdmi_edid_read(HDMI_TX_TypeDef * const hdmi, unsigned page, uint8_t * buff)
{
	unsigned i;
	const unsigned len = 0x80;
	if (hdmi_i2c_read(hdmi, 0x50, page * len, buff, len))
	{
		// Checksum verify
		unsigned cks;
		for (cks = 0, i = 0; i < len; ++ i)
		{
			cks += buff [i];
		}
		if ((cks & 0xFF) != 0)
			return 0;
		return 1;

	}
	return 0;
}

//	EDID Structure Version & Revision: 01 03
//	00000000: 00 FF FF FF FF FF FF 00 12 E5 00 21 50 2D 31 01  ...........!P-1.
//	00000010: 1C 13 01 03 81 2F 1A 78 2E 35 85 A6 56 48 9A 24  ...../.x.5..VH.$
//	00000020: 12 50 54 AF EF 00 01 01 01 01 01 01 01 01 01 01  .PT.............
//	00000030: 01 01 01 01 01 01 A1 13 00 40 41 58 19 20 2C 58  .........@AX. ,X
//	00000040: 36 00 DC 0C 11 00 00 1A 00 00 00 FF 00 30 0A 0A  6............0..
//	00000050: 0A 0A 0A 0A 0A 0A 0A 0A 0A 0A 00 00 00 FD 00 38  ...............8
//	00000060: 4B 1E 53 15 00 0A 20 20 20 20 20 20 00 00 00 FC  K.S...      ....
//	00000070: 00 48 44 4D 49 0A 0A 0A 0A 0A 0A 0A 0A 0A 01 F9  .HDMI...........
//	00000080: 02 03 21 71 4E 06 07 02 03 15 96 11 12 13 04 14  ..!qN...........
//	00000090: 05 1F 90 23 09 07 07 83 01 00 00 65 03 0C 00 10  ...#.......e....
//	000000A0: 00 8C 0A D0 90 20 40 31 20 0C 40 55 00 B9 88 21  ..... @1 .@U...!
//	000000B0: 00 00 18 01 1D 80 18 71 1C 16 20 58 2C 25 00 B9  .......q.. X,%..
//	000000C0: 88 21 00 00 9E 01 1D 80 D0 72 1C 16 20 10 2C 25  .!.......r.. .,%
//	000000D0: 80 B9 88 21 00 00 9E 01 1D 00 BC 52 D0 1E 20 B8  ...!.......R.. .
//	000000E0: 28 55 40 B9 88 21 00 00 1E 02 3A 80 D0 72 38 2D  (U@..!....:..r8-
//	000000F0: 40 10 2C 45 80 B9 88 21 00 00 1E 00 00 00 00 D0  @.,E...!........

//	EDID Structure Version & Revision: 01 03
//	00000000: 00 FF FF FF FF FF FF 00 41 0C 55 D0 46 11 00 00  ........A.U.F...
//	00000010: 0F 12 01 03 80 29 1A 78 2A 78 F5 A6 55 48 9B 26  .....).x*x..UH.&
//	00000020: 12 50 54 BD CF 00 81 40 81 80 95 0F 01 01 01 01  .PT....@........
//	00000030: 01 01 01 01 01 01 9A 29 A0 D0 51 84 22 30 50 98  .......)..Q."0P.
//	00000040: 36 00 9A 00 11 00 00 1C AB 22 A0 A0 50 84 1A 30  6........"..P..0
//	00000050: 30 20 36 00 10 0A 00 00 00 1A 00 00 00 FD 00 38  0 6............8
//	00000060: 4C 1E 50 0F 00 0A 20 20 20 20 20 20 00 00 00 FC  L.P...      ....
//	00000070: 00 31 39 50 46 4C 33 34 30 33 45 55 0A 20 01 CD  .19PFL3403EU. ..
//	00000080: 02 03 29 F1 4F 13 04 14 05 06 07 02 03 11 12 15  ..).O...........
//	00000090: 16 10 1F 01 23 09 7F 07 83 01 00 00 6C 03 0C 00  ....#.......l...
//	000000A0: 20 00 00 00 C0 01 01 01 01 01 1D 00 BC 52 D0 1E   ............R..
//	000000B0: 20 B8 28 55 40 34 CD 31 00 00 1E 01 1D 80 18 71   .(U@4.1.......q
//	000000C0: 1C 16 20 58 2C 25 00 34 CD 31 00 00 9E 01 1D 00  .. X,%.4.1......
//	000000D0: 72 51 D0 1E 20 6E 28 55 00 34 CD 31 00 00 1E 01  rQ.. n(U.4.1....
//	000000E0: 1D 80 D0 72 1C 16 20 10 2C 25 80 34 CD 31 00 00  ...r.. .,%.4.1..
//	000000F0: 9E 00 00 00 00 00 00 00 00 00 00 00 00 00 00 3F  ...............?


// See http://www.edidreader.com/
// https://github.com/dgallegos/edidreader

static void hdmi_edid_parse(const uint8_t * edid, unsigned len)
{
	static const uint8_t header [8] =
	{
		0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
	};
	if (len <= sizeof header && memcmp(edid, header, sizeof header))
	{
		PRINTF("No EDID header\n");
		return;
	}
	PRINTF("EDID Structure Version & Revision: %02X %02X\n", edid [0x12], edid [0x13]);
	printhex(0, edid, len);
	unsigned index = 0x80;
	uint8_t detail_timing_offset, tag_code, data_payload;
	int i;

	if (edid[index++] != 0x2) /* only support cea ext block now */
		return;
	if (edid[index++] != 0x3) /* only support version 3 */
		return;

	detail_timing_offset = edid[index++];

	PRINTF("cfg->cea_underscan = %d\n", (edid[index] >> 7) & 0x1);
	PRINTF("cfg->cea_basicaudio = %d\n", (edid[index] >> 6) & 0x1);
	PRINTF("cfg->cea_ycbcr444 = %d\n", (edid[index] >> 5) & 0x1);
	PRINTF("cfg->cea_ycbcr422 = %d\n", (edid[index] >> 4) & 0x1);

	/* Parse data block */
	while (++index < detail_timing_offset) {
		tag_code = (edid[index] >> 5) & 0x7;
		data_payload = edid[index] & 0x1f;

		if (tag_code == 0x2) {
			for (i = 0; i < data_payload; i++)
			{
				PRINTF("cfg->video_cap[%d] = %d\n", i, edid[index + 1 + i]);
			}
		}

		/* Find vendor block to check HDMI capable */
		if (tag_code == 0x3) {
			if ((edid[index + 1] == 0x03) &&
			    (edid[index + 2] == 0x0c) &&
			    (edid[index + 3] == 0x00))
			{
				PRINTF("cfg->hdmi_cap = 1 /* true */\n");
			}
		}

		index += data_payload;
	}
}

static int hdmi_connector_detect(void)
{
	HDMI_TX_TypeDef * const hdmi = HDMI_TX0;
	return hdmi->HDMI_PHY_STAT0 & 0x02 ?	// HDMI_PHY_HPD
		1 : 0;

}

static void t507_hdmi_edid_test(void)
{
	HDMI_TX_TypeDef * const hdmi = HDMI_TX0;
	// I2C speed
	unsigned i2c_speed_divider = 0xFFFF;
	hdmi->HDMI_PHY_I2CM_SS_SCL_HCNT_1_ADDR = 0xFF;
	hdmi->HDMI_PHY_I2CM_SS_SCL_LCNT_1_ADDR = 0xFF;
	hdmi->HDMI_PHY_I2CM_SS_SCL_HCNT_0_ADDR = 0xFF;
	hdmi->HDMI_PHY_I2CM_SS_SCL_LCNT_0_ADDR = 0xFF;

	hdmi->HDMI_PHY_I2CM_FS_SCL_HCNT_1_ADDR = 0xFF;
	hdmi->HDMI_PHY_I2CM_FS_SCL_LCNT_1_ADDR = 0xFF;
	hdmi->HDMI_PHY_I2CM_FS_SCL_HCNT_0_ADDR = 0xFF;
	hdmi->HDMI_PHY_I2CM_FS_SCL_LCNT_0_ADDR = 0xFF;

	hdmi->HDMI_PHY_I2CM_DIV_ADDR;	// select FS or SS mode
	PRINTF("t507_hdmi_edid_test before reset\n");
	// I2C reset
	hdmi->HDMI_PHY_I2CM_SOFTRSTZ_ADDR = 0x00;
	while ((hdmi->HDMI_PHY_I2CM_SOFTRSTZ_ADDR & 0x01) == 0)
		;

	PRINTF("t507_hdmi_edid_test after reset\n");
	//PRINTF("hdmi->HDMI_I2CM_DIV=%02X\n", (unsigned) hdmi->HDMI_I2CM_DIV);

	if (hdmi_connector_detect() == 0)
	{
		PRINTF("No HDMI device\n");
		//return;
	}

	// EDID ("Extended display identification data") read
	// Test I2C bus read
	static uint8_t edid [256];
	if (hdmi_edid_read(hdmi, 0, edid + 0x00))
	{
		unsigned edidlen = 0x80;
		if (edid [0x7E]) // need read ext block?
		{
			if (hdmi_edid_read(hdmi, 1, edid + 0x80))
				edidlen = 0x100;
		}
		hdmi_edid_parse(edid, edidlen);
	}
}

#endif

static DE_GLB_TypeDef * de3_getglb(int rtmixid)
{
	switch (rtmixid)
	{
	default: return NULL;
	case 1: return DE_MIXER0_GLB;
	case 2: return DE_MIXER1_GLB;
	}
}

static DE_VI_TypeDef * de3_getvi(int rtmixid, int id)
{
	ASSERT(rtmixid >= 1);
	ASSERT(id >= 1);
	switch (rtmixid)
	{
	default: return NULL;
	case 1: return id <= ARRAY_SIZE(rtmix0_vimap) ? rtmix0_vimap [id - 1] : NULL;
	case 2: return id <= ARRAY_SIZE(rtmix1_vimap) ? rtmix1_vimap [id - 1] : NULL;
	}
}

static DE_UI_TypeDef * de3_getui(int rtmixid, int id)
{
	ASSERT(rtmixid >= 1);
	ASSERT(id >= 1);
	switch (rtmixid)
	{
	default: return NULL;
	case 1: return id <= ARRAY_SIZE(rtmix0_uimap) ? rtmix0_uimap [id - 1] : NULL;
	case 2: return id <= ARRAY_SIZE(rtmix1_uimap) ? rtmix1_uimap [id - 1] : NULL;
	}
}

static DE_BLD_TypeDef * de3_getbld(int rtmixid)
{
	switch (rtmixid)
	{
	default: return NULL;
	case 1: return DE_MIXER0_BLD;
	case 2: return DE_MIXER1_BLD;
	}
}


static DE_VSU_TypeDef * de3_getvsu(int rtmixid, int vich)
{
	ASSERT(vich == 1);
#if CPUSTYLE_T507 || CPUSTYLE_H616
	switch (rtmixid)
	{
	default: return NULL;
	case 1: return DE_VSU1;	// VI1
	case 2: return DE_VSU2;	// VI2
	}
#elif CPUSTYLE_H3 || CPUSTYLE_A64
	switch (rtmixid)
	{
	default: return NULL;
	case 1: return DE_MIXER0_VSU1;	// VI1
	case 2: return DE_MIXER1_VSU1;	// VI1
	}
#elif CPUSTYLE_T113 || CPUSTYLE_F133
	switch (rtmixid)
	{
	default: return NULL;
	case 1: return DE_MIXER0_VSU1;	// VI1
	case 2: return DE_MIXER1_VSU1;	// VI1
	}
#endif /* CPUSTYLE_T507 || CPUSTYLE_H616 || CPUSTYLE_A64 */
}


static DE_UIS_TypeDef * de3_getuis(int rtmixid, int uich)
{
	ASSERT(uich == 1);
#if CPUSTYLE_T507 || CPUSTYLE_H616
	switch (rtmixid)
	{
	default: return NULL;
	case 1: return DE_UIS1;	// UI1
	case 2: return DE_UIS2;	// UI2
	}
#elif CPUSTYLE_H3 || CPUSTYLE_A64
	switch (rtmixid)
	{
	default: return NULL;
	case 1: return DE_MIXER0_UIS1;	// UI1
	case 2: return DE_MIXER1_UIS1;	// UI1
	}
#elif CPUSTYLE_T113 || CPUSTYLE_F133
	switch (rtmixid)
	{
	default: return NULL;
	case 1: return DE_MIXER0_UIS1;	// UI1 - need clarification
	case 2: return DE_MIXER1_UIS1;	// UI1 - need clarification
	}
#endif /* CPUSTYLE_T507 || CPUSTYLE_H616 || CPUSTYLE_A64 */
	return NULL;
}


//static void write32(uintptr_t a, uint32_t v)
//{
//	* (volatile uint32_t *) a = v;
//}


//для VI
#define DE2_FORMAT_YUV420_V1U1V0U0 0x08
#define DE2_FORMAT_YUV420_U1V1U0V0 0x09
#define DE2_FORMAT_YUV420_PLANAR   0x0A

//для UI
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

#if LCDMODE_MAIN_ARGB8888
	static const uint32_t ui_format = 0x00;	//  0x00: ARGB_8888
	//const uint32_t ui_format = 0x04;	// 0x04: XRGB_8888
#elif LCDMODE_MAIN_RGB565
	static const uint32_t ui_format = 0x0A;	// 0x0A: RGB_565
#else
	#error Unsupported framebuffer format. Looks like you need remove WITHLTDCHW
	static const uint32_t ui_format = 0x0A;
#endif


#if CPUSTYLE_H3
	#define LCD_VB_INT_EN  		(UINT32_C(1) << 31)	// Enable the Vb interrupt
	#define LCD_VB_INT_FLAG  	(UINT32_C(1) << 15)	// Asserted during vertical no-display period every frame

	#define TVOUT_VB_INT_EN  	(UINT32_C(1) << 30)	// Enable the Vb interrupt
	#define TVOUT_VB_INT_FLAG  	(UINT32_C(1) << 14)	// Asserted during vertical no-display period every frame

#elif CPUSTYLE_A64
	#define LCD_VB_INT_EN  		(UINT32_C(1) << 31)	// TCON0 Enable the Vb interrupt
	#define LCD_VB_INT_FLAG  	(UINT32_C(1) << 15)	// Asserted during vertical no-display period every frame

	#define TVOUT_VB_INT_EN  	(UINT32_C(1) << 30)	// TCON1 Enable the Vb interrupt
	#define TVOUT_VB_INT_FLAG  	(UINT32_C(1) << 14)	// Asserted during vertical no-display period every frame

#else
	#define LCD_VB_INT_EN  		(UINT32_C(1) << 31)	// Enable the Vb interrupt
	#define LCD_VB_INT_FLAG  	(UINT32_C(1) << 15)	// Asserted during vertical no-display period every frame

	#define TVOUT_VB_INT_EN  	(UINT32_C(1) << 30)	// Enable the Vb interrupt
	#define TVOUT_VB_INT_FLAG  	(UINT32_C(1) << 14)	// Asserted during vertical no-display period every frame

#endif

/* ожидаем начало кадра - используется если не по прерываниям*/
static void hardware_ltdc_vsync(int rtmixid)
{
	switch (rtmixid)
	{
#if defined RTMIXIDLCD
	case RTMIXIDLCD:
		TCONLCD_GINT0_REG &= ~ LCD_VB_INT_FLAG;         //clear LCD_VB_INT_FLAG
		while ((TCONLCD_GINT0_REG & LCD_VB_INT_FLAG) == 0) //wait  LCD_VB_INT_FLAG
			;
		break;
#endif
#if defined RTMIXIDTV
	case RTMIXIDTV:
		TCONTV_GINT0_REG &= ~ TVOUT_VB_INT_FLAG;         //clear TCON1_VB_INT_FLAG
	    while ((TCONTV_GINT0_REG & TVOUT_VB_INT_FLAG) == 0) //wait  TCON1_VB_INT_FLAG
	        ;
		break;
#endif
	}
}

/* ожидаем начало кадра - используется если не по прерываниям*/
//static void hardware_tvout_ltdc_vsync(void)
//{
//#if defined (TCONTV_PTR)
//	TCONLCD_GINT0_REG &= ~ TVOUT_VB_INT_FLAG;         //clear TVOUT_VB_INT_FLAG
//    while ((TCONLCD_GINT0_REG & TVOUT_VB_INT_FLAG) == 0) //wait  TVOUT_VB_INT_FLAG
//        ;
//#endif /* defined (TCONLCD_PTR) */
//}

/* Update registers */
static void t113_de_update(int rtmixid)
{
	DE_GLB_TypeDef * const glb = de3_getglb(rtmixid);
	if (glb == NULL)
		return;

    glb->GLB_DBUFFER = UINT32_C(1);		// 1: register value be ready for update (self-cleaning bit)
	while ((glb->GLB_DBUFFER & UINT32_C(1)) != 0)
		;
}

/* VI (VI0) */
static void t113_de_set_address_vi(int rtmixid, uintptr_t vram, int vich)
{
	DE_VI_TypeDef * const vi = de3_getvi(rtmixid, vich);

	if (vi == NULL)
		return;

	if (vram == 0)
	{
		vi->CFG [VI_CFG_INDEX].ATTR = 0;
		return;
	}
	const uint32_t ovl_ui_mbsize = (((DIM_Y - 1) << 16) | (DIM_X - 1));
	const uint32_t uipitch = LCDMODE_PIXELSIZE * GXADJ(DIM_X);
	const uint_fast32_t attr =
		((vram != 0) << 0) |	// enable
#if 0
		(UINT32_C(255) << 24) | // LAY_GLBALPHA
		(UINT32_C(1) << 1) | 	// LAY_ALPHA _MODE: 0x1:Globe alpha enable
#endif
		ui_format * (UINT32_C(1) << 8) |		// нижний слой: 32 bit ABGR 8:8:8:8 без пиксельной альфы
		(UINT32_C(1) << 15) |	// Video_UI_SEL 0: Video Overlay(using Video Overlay Layer Input data format) 1: UI Overlay(using UI Overlay Layer Input data format)
		//(UINT32_C(1) << 4) |	// LAY_FILLCOLOR_EN - замещает данные, идущие по DMA
		0;

	vi->CFG [VI_CFG_INDEX].TOP_LADDR [0] = ptr_lo32(vram);	// The setting of this register is U/UV channel address.
	vi->TOP_HADDR [0] = (ptr_hi32(vram) & 0xFF) << (8 * VI_CFG_INDEX);						// The setting of this register is U/UV channel address.
	vi->CFG [VI_CFG_INDEX].ATTR = attr;

	vi->CFG [VI_CFG_INDEX].SIZE = ovl_ui_mbsize;
	vi->CFG [VI_CFG_INDEX].COORD = 0;
	vi->CFG [VI_CFG_INDEX].PITCH [0] = uipitch;	// PLANE 0 - The setting of this register is Y channel.
	vi->OVL_SIZE [0] = ovl_ui_mbsize;	// Y
//	vi->HORI [0] = 0;
//	vi->VERT [0] = 0;

//	ASSERT(vi->CFG [VI_CFG_INDEX].TOP_LADDR [0] == ptr_lo32(vram));
//	ASSERT(vi->CFG [VI_CFG_INDEX].ATTR == attr);

}

/* VI (VI0) - No UI format attr */
static void t113_de_set_address_vi2(int rtmixid, uintptr_t vram, int vich, uint_fast8_t vi_format)
{
	const videomode_t * const vdmode_CRT = get_videomode_CRT();
	DE_VI_TypeDef * const vi = de3_getvi(rtmixid, vich);

	if (vi == NULL)
		return;

	if (vram == 0)
	{
		vi->CFG [VI_CFG_INDEX].ATTR = 0;
		return;
	}
//	const uint32_t ovl_ui_mbsize = (((vdmode_CRT->height - 1) << 16) | (vdmode_CRT->width - 1));
//	const uint32_t uipitch = vdmode_CRT->width;//LCDMODE_PIXELSIZE * GXADJ(vdmode_CRT->width);
	const uint32_t ovl_ui_mbsize = (((TVD_HEIGHT - 1) << 16) | (TVD_WIDTH - 1));
	const uint32_t uipitch = TVD_WIDTH;
	const uint_fast32_t attr =
		((vram != 0) << 0) |	// enable
#if 0
		(UINT32_C(255) << 24) | // LAY_GLBALPHA
		(UINT32_C(1) << 1) | 	// LAY_ALPHA _MODE: 0x1:Globe alpha enable
#endif
		vi_format * (UINT32_C(1) << 8) |		// нижний слой: 32 bit ABGR 8:8:8:8 без пиксельной альфы
		0*(UINT32_C(1) << 15) |	// Video_UI_SEL 0: Video Overlay(using Video Overlay Layer Input data format) 1: UI Overlay(using UI Overlay Layer Input data format)
		//(UINT32_C(1) << 4) |	// LAY_FILLCOLOR_EN - замещает данные, идущие по DMA
		0;

	const uintptr_t vram0 = vram;
	const uintptr_t vram1 = vram0 + TVD_SIZE;//(vdmode_CRT->height * vdmode_CRT->width);

	vi->CFG [VI_CFG_INDEX].TOP_LADDR [0] = ptr_lo32(vram0);	// The setting of this register is U/UV channel address.
	vi->TOP_HADDR [0] = (ptr_hi32(vram0) & 0xFF) << (8 * VI_CFG_INDEX);						// The setting of this register is U/UV channel address.
	vi->CFG [VI_CFG_INDEX].TOP_LADDR [1] = ptr_lo32(vram1);	// The setting of this register is U/UV channel address.
	vi->TOP_HADDR [1] = (ptr_hi32(vram1) & 0xFF) << (8 * VI_CFG_INDEX);						// The setting of this register is U/UV channel address.

	vi->CFG [VI_CFG_INDEX].SIZE = ovl_ui_mbsize;
	vi->CFG [VI_CFG_INDEX].COORD = 0;
	vi->CFG [VI_CFG_INDEX].PITCH [0] = uipitch;	// PLANE 0 - The setting of this register is Y channel.
	vi->CFG [VI_CFG_INDEX].PITCH [1] = uipitch;	// PLANE 0 - The setting of this register is U/UV channel.
//	vi->CFG [VI_CFG_INDEX].PITCH [2] = uipitch;	// PLANE 0 - The setting of this register is V channel.
	vi->OVL_SIZE [0] = ovl_ui_mbsize;	// Y
	vi->OVL_SIZE [1] = ovl_ui_mbsize;	// UV
//	vi->HORI [0] = 0;
//	vi->VERT [0] = 0;
//	vi->FCOLOR [0] = 0xFFFFFFFF;	// Opaque RED. при LAY_FILLCOLOR_EN - ALPGA + R + G + B - при LAY_FILLCOLOR_EN - замещает данные, идущие по DMA
	vi->CFG [VI_CFG_INDEX].ATTR = attr;

//	ASSERT(vi->CFG [VI_CFG_INDEX].TOP_LADDR [0] == ptr_lo32(vram0));
//	ASSERT(vi->CFG [VI_CFG_INDEX].TOP_LADDR [1] == ptr_lo32(vram1));
//	ASSERT(vi->CFG [VI_CFG_INDEX].ATTR == attr);

}

static void t113_de_set_address_ui(int rtmixid, uintptr_t vram, int uich)
{
	DE_UI_TypeDef * const ui = de3_getui(rtmixid, uich);

	if (ui == NULL)
		return;

	if (vram == 0)
	{
		ui->CFG [UI_CFG_INDEX].ATTR = 0;
		ASSERT(ui->CFG [UI_CFG_INDEX].ATTR == 0);
		return;
	}
	const uint32_t ovl_ui_mbsize = (((DIM_Y - 1) << 16) | (DIM_X - 1));
	const uint32_t uipitch = LCDMODE_PIXELSIZE * GXADJ(DIM_X);
	const uint_fast32_t attr =
		(vram != 0) * (UINT32_C(1) << 0) |	// enable
		ui_format * (UINT32_C(1) << 8) |		// нижний слой: 32 bit ABGR 8:8:8:8 без пиксельной альфы
#if 0
		(UINT32_C(255) << 24) | // LAY_GLBALPHA
		(UINT32_C(1) << 1) | 	// LAY_ALPHA _MODE: 0x1:Globe alpha enable
#endif
		0;

	ui->CFG [UI_CFG_INDEX].TOP_LADDR = ptr_lo32(vram);
	ui->TOP_HADDR = (0xFF & ptr_hi32(vram)) << (8 * UI_CFG_INDEX);
	ui->CFG [UI_CFG_INDEX].SIZE = ovl_ui_mbsize;
	ui->CFG [UI_CFG_INDEX].COORD = 0;
	ui->CFG [UI_CFG_INDEX].PITCH = uipitch;
	ui->CFG [UI_CFG_INDEX].FCOLOR = 0xFF0000FF;	// Opaque BLUE
	ui->CFG [UI_CFG_INDEX].ATTR = attr;

	ui->OVL_SIZE = ovl_ui_mbsize;

	ASSERT(ui->CFG [UI_CFG_INDEX].ATTR == attr);
	ASSERT(ui->CFG [UI_CFG_INDEX].SIZE == ovl_ui_mbsize);
	ASSERT(ui->CFG [UI_CFG_INDEX].COORD == 0);
	ASSERT(ui->OVL_SIZE == ovl_ui_mbsize);
	ASSERT(ui->CFG [UI_CFG_INDEX].ATTR == attr);
}

/* Для обнуления регистров периферии не очень
 * подходит системный memset - иногда может группировать обращения к соседним ячейкам
*/
static void memset32(void * d, uint32_t v, unsigned sz)
{
	uint32_t volatile * dst = (uint32_t volatile *) d;
	while (sz > 4)
	{
		* dst ++ = v;
		sz -= 4;
	}
}

static void t113_de_rtmix_initialize(int rtmixid)
{
	DE_GLB_TypeDef * const glb = de3_getglb(rtmixid);
	if (glb == NULL)
		return;
	glb->GLB_CTL =
			(UINT32_C(1) << 12) |	// OUT_DATA_WB 0:RT-WB fetch data after DEP port
			(UINT32_C(1) << 0) |		// EN RT enable/disable
			0;
#if CPUSTYLE_T507 || CPUSTYLE_H616
	glb->GLB_CLK |= (UINT32_C(1) << 0);
#endif

	ASSERT(glb->GLB_CTL & (UINT32_C(1) << 0));
	memset32(de3_getvi(rtmixid, 1), 0, sizeof * de3_getvi(rtmixid, 1));	// Требуется на H3
}

static void t113_de_bld_initialize(int rtmixid, const videomode_t * vdmode, uint_least32_t color24)
{
	int i;
	DE_BLD_TypeDef * const bld = de3_getbld(rtmixid);
	if (bld == NULL)
		return;
	DE_GLB_TypeDef * const glb = de3_getglb(rtmixid);
	// Allwinner_DE2.0_Spec_V1.0.pdf
	// 5.10.8.2 OVL_UI memory block size register
	// 28..16: LAY_HEIGHT
	// 12..0: LAY_WIDTH
	const uint32_t ovl_ui_mbsize = (((vdmode->height - 1) << 16) | (vdmode->width - 1));


	/* DE submodules */
	if (glb)
	{
		// Зачем ставится тут размер?
		glb->GLB_SIZE = ovl_ui_mbsize;
		ASSERT(glb->GLB_SIZE == ovl_ui_mbsize);
	}

	// 5.10.9.1 BLD fill color control register
	// BLD_FILL_COLOR_CTL
	bld->BLD_EN_COLOR_CTL = 0;

	// 5.10.9.5 BLD routing control register
	// BLD_CH_RTCTL
	// 0x03020100 - default state

	bld->ROUTE =
			(UINT32_C(0) << 0) |		// pipe 0 from ch 0
			(UINT32_C(1) << 4) |		// pipe 1 from ch 1
			(UINT32_C(2) << 8) |		// pipe 2 from ch 2
			(UINT32_C(3) << 12) |		// pipe 3 from ch 3
			(UINT32_C(4) << 16) |		// pipe 4 from ch 4
			(UINT32_C(5) << 18) |		// pipe 5 from ch 5
			0;
	bld->PREMULTIPLY = 0;
	bld->BKCOLOR = color24; /* 24 bit. Отображается, когда нет данных от входного pipe */

//	PRINTF("2 bld->ROUTE=%08" PRIX32 "\n", bld->ROUTE);
//	PRINTF("2 bld->BKCOLOR=%08" PRIX32 "\n", bld->BKCOLOR);
	//printhex32(DE_BLD_BASE, DE_BLD, 256);
	// 5.10.9.1 BLD fill color control register
	// BLD_CTL
	// в примерах только 0 и 1 индексы
//	for(i = 0; i < ARRAY_SIZE(bld->BLD_MODE); i++)
//	{
//	    unsigned bld_mode = 0x03020302;		// default
//		PRINTF("1 DE_BLD->BLD_MODE [%d]=%08X\n", i, (unsigned) bld->BLD_MODE [i]);
////	    unsigned bld_mode = 0x03020302;           //Fs=Ad, Fd=1-As, Qs=Ad, Qd=1-As
//	    bld->BLD_MODE [i] = bld_mode;
//		ASSERT(bld->BLD_MODE [i] == bld_mode);
//		PRINTF("2 DE_BLD->BLD_MODE [%d]=%08X\n", i, (unsigned) bld->BLD_MODE [i]);
//	}

	bld->OUTPUT_SIZE = ovl_ui_mbsize;
	bld->OUT_CTL = 0;	// ITLMOD_EN=0, PREMUL_EN=0
	bld->CK_CTL = 0;
	for (i = 0; i < ARRAY_SIZE(bld->CH); i++)
	{
		//PRINTF("BLD_CH rtmixid=%d, %u\n", rtmixid, i);
		bld->CH [i].BLD_FILL_COLOR = 0xff000000;
		bld->CH [i].BLD_CH_ISIZE = ovl_ui_mbsize;
		bld->CH [i].BLD_CH_OFFSET = 0;
		ASSERT(bld->CH [i].BLD_CH_ISIZE == ovl_ui_mbsize);
		ASSERT(bld->CH [i].BLD_FILL_COLOR == 0xff000000);
	}

	for (i = 1; ; ++ i)
	{
		if (de3_getui(rtmixid, i) == NULL)
			break;
		t113_de_set_address_ui(rtmixid, 0, i);
	}
	for (i = 1; ; ++ i)
	{
		if (de3_getvi(rtmixid, i) == NULL)
			break;
		t113_de_set_address_vi(rtmixid, 0, i);
	}
}

// LVDS: mstep1, HV: step1: Select HV interface type
static void t113_select_HV_interface_type(const videomode_t * vdmode)
{
#if defined (TCONLCD_PTR)
#if CPUSTYLE_H3
	uint32_t start_dly = 2; //0x1F;	// 1,2 - need for 4.3 inch panel 272*480 - should be tested
	TCONLCD_PTR->TCON1_CTL_REG =
		start_dly * (UINT32_C(1) << 4) |	// Start_Delay (was: 60)
		0;
#elif CPUSTYLE_A64
	TCONLCD_PTR->TCON1_CTL_REG =
		0;
#else
	//uint32_t start_dly = (vdmode->vfp + vdmode->vbp + vdmode->vsync) / 2;
	uint32_t start_dly = 2; //0x1F;	// 1,2 - need for 4.3 inch panel 272*480 - should be tested
	TCONLCD_PTR->LCD_CTL_REG =
		//1 * (UINT32_C(1) << 31) |		// LCD_EN - done in t113_open_module_enable
		0 * (UINT32_C(1) << 24) |		// LCD_IF 0x00: HV (Sync+DE), 01: 8080 I/F
		0 * (UINT32_C(1) << 23) |		// LCD_RB_SWAP
		1 * (UINT32_C(1) << 20) |		// LCD_INTERLACE_EN (has no effect)
		(start_dly & 0x1F) * (UINT32_C(1) << 4) |	// LCD_START_DLY
		0 * (UINT32_C(1) << 0) |			// LCD_SRC_SEL: 000: DE, 1..7 - tests: 1: color check, 2: grayscale check
		0;
#endif
#endif /* defined (TCONLCD_PTR) */
}


// Used for HV and LVDS outputs
// T113: PLL_VIDEO1
// T507: PLL_VIDEO1
static void t113_tconlcd_CCU_configuration(void)
{
	const unsigned prei = 0;
	const unsigned divider = 1;

#if defined (TCONLCD_PTR)

#if CPUSTYLE_H3

#elif CPUSTYLE_A64

#elif CPUSTYLE_T507 || CPUSTYLE_H616

	const unsigned ix = TCONLCD_IX;	// TCON_LCD0

	CCU->DISPLAY_IF_TOP_BGR_REG |= (UINT32_C(1) << 0);	// DISPLAY_IF_TOP_GATING
	//CCU->DISPLAY_IF_TOP_BGR_REG &= ~ (UINT32_C(1) << 16);	// DISPLAY_IF_TOP_RST Assert
	CCU->DISPLAY_IF_TOP_BGR_REG |= (UINT32_C(1) << 16);	// DISPLAY_IF_TOP_RST De-assert writable mask 0x00010001

    //DISP_IF_TOP->MODULE_GATING |= (UINT32_C(1) << 31);
    //PRINTF("DISP_IF_TOP->MODULE_GATING=%08X\n", (unsigned) DISP_IF_TOP->MODULE_GATING);
	TCONLCD_CCU_CLK_REG = (TCONLCD_CCU_CLK_REG & ~ (UINT32_C(0x07) << 24)) |
		2 * (UINT32_C(1) << 24) | // 010: PLL_VIDEO1(1X)
		0;

	TCONLCD_CCU_CLK_REG |= UINT32_C(1) << 31;	// SCLK_GATING
	//PRINTF("t113_tconlcd_CCU_configuration: BOARD_TCONLCDFREQ=%" PRIuFAST32 " MHz\n", (uint_fast32_t) BOARD_TCONLCDFREQ / 1000 / 1000);

	TCONLCD_CCU_BGR_REG |= (UINT32_C(1) << (0 + ix));	// Clock Gating
	TCONLCD_CCU_BGR_REG &= ~ (UINT32_C(1) << (16 + ix));	// Assert Reset
	TCONLCD_CCU_BGR_REG |= (UINT32_C(1) << (16 + ix));	// De-assert Reset (bits 19..16 and 3..0 writable) mask 0x000F000F

    local_delay_us(10);

    TCONLCD_PTR->LCD_IO_TRI_REG = UINT32_C(0xFFFFFFFF);

#elif CPUSTYLE_T113 || CPUSTYLE_F133

	/* Configure TCONLCD clock */
	ASSERT(divider >= 1 && divider <= 16);
    TCONLCD_CCU_CLK_REG = (TCONLCD_CCU_CLK_REG & ~ (UINT32_C(0x07) << 24) & ~ (UINT32_C(0x03) << 8) & ~ (UINT32_C(0x0F) << 0)) |
		0x02 * (UINT32_C(1) << 24) |	// CLK_SRC_SEL 010: PLL_VIDEO1(1X)
		prei * (UINT32_C(1) << 8) |	// FACTOR_N 0..3: 1..8
		((divider - 1) << 0) |	// FACTOR_M (0x00..0x0F: 1..16)
		0;
    TCONLCD_CCU_CLK_REG |= (UINT32_C(1) << 31);
	//PRINTF("t113_tconlcd_CCU_configuration: BOARD_TCONLCDFREQ=%u MHz\n", (unsigned) (BOARD_TCONLCDFREQ / 1000 / 1000));
    local_delay_us(10);

    CCU->TCONLCD_BGR_REG |= (UINT32_C(1) << 0);	// Open the clock gate

    CCU->TCONLCD_BGR_REG &= ~ (UINT32_C(1) << 16);	// Set the LVDS reset of TCON LCD BUS GATING RESET register;
    CCU->TCONLCD_BGR_REG |= (UINT32_C(1) << 16);	// Release the LVDS reset of TCON LCD BUS GATING RESET register;
    local_delay_us(10);

    // DISPLAY_TOP access
	CCU->DPSS_TOP_BGR_REG |= (UINT32_C(1) << 0);	// DPSS_TOP_GATING Open the clock gate
	CCU->DPSS_TOP_BGR_REG |= (UINT32_C(1) << 16);	// DPSS_TOP_RST De-assert reset

#else

#endif
#endif /* defined (TCONLCD_PTR) */
}

// Used for LVDS outputs
// T113: PLL_VIDEO1
// T507: PLL_VIDEO1
static void t113_tconlvds_PLL_configuration(uint_fast32_t needfreq)
{
#if defined (TCONLCD_PTR)

#if CPUSTYLE_H3

#elif CPUSTYLE_A64


#elif CPUSTYLE_T507 || CPUSTYLE_H616

	// LVDS mode
	// The default value of PLL_VIDEO1(4X) is 1188 MHz
   	const uint_fast32_t pllreg = CCU->PLL_VIDEO1_CTRL_REG;
	const uint_fast32_t M = UINT32_C(1) + ((pllreg >> 1) & 0x01);	// PLL_INPUT_DIV_M
	uint_fast32_t N = calcdivround2(needfreq * M * 4, allwnr_t507_get_hosc_freq());
	N = ulmin16(N, 256);
	N = ulmax16(N, 1);

	allwnr_t507_module_pll_spr(& CCU->PLL_VIDEO1_CTRL_REG, & CCU->PLL_VIDEO1_PAT0_CTRL_REG);	// Set Spread Frequency Mode
	allwnr_t507_module_pll_enable(& CCU->PLL_VIDEO1_CTRL_REG, N);

#elif CPUSTYLE_T113 || CPUSTYLE_F133

	// LVDS mode
	// не меняем параметры по умолчанию
	// default frequency of PLL_VIDEO1(4X) is 1188 MHz.
	allwnr_t113_module_pll_spr(& CCU->PLL_VIDEO1_CTRL_REG, & CCU->PLL_VIDEO1_PAT0_CTRL_REG);	// Set Spread Frequency Mode
	allwnr_t113_module_pll_enable(& CCU->PLL_VIDEO1_CTRL_REG);

#else

#endif
#endif /* defined (TCONLCD_PTR) */
}

// Used for LVDS outputs
// T113: PLL_VIDEO1
// T507: PLL_VIDEO1
static void t113_tconlvds_CCU_configuration(uint_fast32_t needfreq)
{
#if defined (TCONLCD_PTR)

#if CPUSTYLE_H3

#elif CPUSTYLE_A64

#elif CPUSTYLE_T507 || CPUSTYLE_H616

	const unsigned ix = TCONLCD_IX;	// TCON_LCD0

	CCU->DISPLAY_IF_TOP_BGR_REG |= (UINT32_C(1) << 0);	// DISPLAY_IF_TOP_GATING
	CCU->DISPLAY_IF_TOP_BGR_REG |= (UINT32_C(1) << 16);	// DISPLAY_IF_TOP_RST De-assert writable mask 0x00010001

	//PRINTF("t113_tconlvds_CCU_configuration: needfreq=%u MHz, N=%u\n", (unsigned) (needfreq / 1000 / 1000), (unsigned) N);
	TCONLCD_CCU_CLK_REG = (TCONLCD_CCU_CLK_REG & ~ (UINT32_C(0x07) << 24)) |
		2 * (UINT32_C(1) << 24) | // 010: PLL_VIDEO1(1X)
		0;
 	TCONLCD_CCU_CLK_REG |= UINT32_C(1) << 31;	// SCLK_GATING
	//PRINTF("t113_tconlvds_CCU_configuration: BOARD_TCONLCDFREQ=%" PRIuFAST32 " MHz\n", (uint_fast32_t) BOARD_TCONLCDFREQ / 1000 / 1000);

	TCONLCD_CCU_BGR_REG |= (UINT32_C(1) << (0 + ix));	// Clock Gating
	TCONLCD_CCU_BGR_REG &= ~ (UINT32_C(1) << (16 + ix));	// Assert Reset
	TCONLCD_CCU_BGR_REG |= (UINT32_C(1) << (16 + ix));	// De-assert Reset (bits 19..16 and 3..0 writable) mask 0x000F000F

    CCU->LVDS_BGR_REG |= (UINT32_C(1) << 16); // LVDS0_RST: De-assert reset (оба LVDS набора выходов разрешаются только одним битом)
//    PRINTF("CCU->LVDS_BGR_REG=%08X\n", (unsigned) CCU->LVDS_BGR_REG);
//    CCU->LVDS_BGR_REG |= (UINT32_C(1) << 16); // LVDS0_RST: De-assert reset (bits 19..16 writable)

	//	Note: Before operating ADDA/GPADC/RES_CAL/CSI/DSI/LVDS/HDMI (only
	//	for T507-H/T517-H)/TVOUT modules, please make sure that this bit is
	//	configured as 1
    PRCM->VDD_SYS_PWROFF_GATING_REG |= (UINT32_C(1) << 4); // ANA_VDDON_GATING
    local_delay_ms(10);

    local_delay_us(10);

    TCONLCD_PTR->LCD_IO_TRI_REG = UINT32_C(0xFFFFFFFF);

#elif CPUSTYLE_T113 || CPUSTYLE_F133

	/* Configure TCONLCD clock */
	unsigned prei = 0;
	const unsigned divider = calcdivround2(allwnr_t113_get_video1pllx4_freq(), needfreq);
	//PRINTF("t113_tconlvds_CCU_configuration: needfreq=%u MHz, prei=%u, divider=%u\n", (unsigned) (needfreq / 1000 / 1000), (unsigned) prei, (unsigned) divider);
	ASSERT(divider >= 1 && divider <= 16);
	// LVDS
    TCONLCD_CCU_CLK_REG = (TCONLCD_CCU_CLK_REG & ~ (UINT32_C(0x07) << 24) & ~ (UINT32_C(0x03) << 8) & ~ (UINT32_C(0x0F) << 0)) |
		0x03 * (UINT32_C(1) << 24) |	// CLK_SRC_SEL 011: PLL_VIDEO1(4X)
		prei * (UINT32_C(1) << 8) |	// FACTOR_N 0..3: 1..8
		((divider - 1) << 0) |	// FACTOR_M (0x00..0x0F: 1..16)
		0;
    TCONLCD_CCU_CLK_REG |= (UINT32_C(1) << 31);
	//PRINTF("t113_tconlvds_CCU_configuration: BOARD_TCONLCDFREQ=%u MHz\n", (unsigned) (BOARD_TCONLCDFREQ / 1000 / 1000));
    local_delay_us(10);

    CCU->TCONLCD_BGR_REG |= (UINT32_C(1) << 0);	// Open the clock gate

    CCU->LVDS_BGR_REG &= ~ (UINT32_C(1) << 16); // LVDS0_RST: Assert reset
    CCU->LVDS_BGR_REG |= (UINT32_C(1) << 16); // LVDS0_RST: De-assert reset

    CCU->TCONLCD_BGR_REG &= ~ (UINT32_C(1) << 16);	// Set the LVDS reset of TCON LCD BUS GATING RESET register;
    CCU->TCONLCD_BGR_REG |= (UINT32_C(1) << 16);	// Release the LVDS reset of TCON LCD BUS GATING RESET register;
    local_delay_us(10);

    // DISPLAY_TOP access
	CCU->DPSS_TOP_BGR_REG |= (UINT32_C(1) << 0);	// DPSS_TOP_GATING Open the clock gate
	CCU->DPSS_TOP_BGR_REG |= (UINT32_C(1) << 16);	// DPSS_TOP_RST De-assert reset

#else

#endif
#endif /* defined (TCONLCD_PTR) */
}

// HV step2 - Clock configuration
static void t113_HV_clock_configuration(const videomode_t * vdmode)
{
#if defined (TCONLCD_PTR)
#if CPUSTYLE_H3
	// No HV outpit
#elif CPUSTYLE_A64
	// No HV outpit
#else
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
#endif
#endif /* defined (TCONLCD_PTR) */
}


// округление тактовой частоты дисплейного контроллера к возможностям системы синхронизации
unsigned long hardware_get_dotclock(unsigned long dotfreq)
{
	return dotfreq;
//	const uint_fast32_t pll4divq = calcdivround2(stm32mp1_get_pll4_freq(), dotfreq);
//	return stm32mp1_get_pll4_freq() / pll4divq;
}

// LVDS step2 - Clock configuration
static void t113_LVDS_clock_configuration(const videomode_t * vdmode)
{
#if defined (TCONLCD_PTR)
#if CPUSTYLE_H3
	// No LVDS outpit
#elif CPUSTYLE_A64
	// No LVDS outpit
#else
    TCONLCD_PTR->LCD_DCLK_REG =
		0x0F * (UINT32_C(1) << 28) |		// LCD_DCLK_EN
		7 * (UINT32_C(1) << 0) |			// LCD_DCLK_DIV
		0;
    local_delay_us(10);
#endif
#endif /* defined (TCONLCD_PTR) */
}

// LVDS step2 - Clock configuration
// TODO: this is only placeholder!

static void t113_MIPIDSI_clock_configuration(const videomode_t * vdmode, unsigned onepixelcloks)
{
#if defined (TCONLCD_PTR)
#if CPUSTYLE_H3
	// No MIPIDSI outpit
#elif CPUSTYLE_A64
	// TODO: init MIPIDSI outpit
#else
#endif
#endif /* defined (TCONLCD_PTR) */
}

// step5 - set LVDS digital logic configuration
static void t113_set_LVDS_digital_logic(const videomode_t * vdmode)
{
#if defined (TCONLCD_PTR)
#if defined (LCD_LVDS_IF_REG_VALUE)

	TCONLCD_PTR->LCD_LVDS_IF_REG = LCD_LVDS_IF_REG_VALUE;

#elif CPUSTYLE_H3
	// No LVDS outpit

#elif CPUSTYLE_A64
	// No LVDS outpit

#else /* defined (LCD_LVDS_IF_REG_VALUE) */

	TCONLCD_PTR->LCD_LVDS_IF_REG =
		1 * (UINT32_C(1) << 31) |	/* LCD_LVDS_EN */
		0 * (UINT32_C(1) << 30) |	/* LCD_LVDS_LINK: 0: single link */
		0 * (UINT32_C(1) << 27) |	/* LCD_LVDS_MODE 1: JEIDA mode (0 for THC63LVDF84B converter) */
		0 * (UINT32_C(1) << 26) |	/* LCD_LVDS_BITWIDTH 0: 24-bit */
		1 * (UINT32_C(1) << 20) |	/* LCD_LVDS_CLK_SEL 1: LCD CLK */
		0 * (UINT32_C(1) << 25) |		/* LCD_LVDS_DEBUG_EN */
		0 * (UINT32_C(1) << 24) |		/* LCD_LVDS_DEBUG_MODE */
		0 * (UINT32_C(1) << 4) |				/* LCD_LVDS_CLK_POL: 0: reverse, 1: normal */
		0 * 0x0F * (UINT32_C(1) << 0) |		/* LCD_LVDS_DATA_POL: 0: reverse, 1: normal */
		0;

#endif /* defined (LCD_LVDS_IF_REG_VALUE) */
#endif /* defined (TCONLCD_PTR) */
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
#if CPUSTYLE_T113 || CPUSTYLE_F133
	// __de_dsi_dphy_dev_t
	// https://github.com/mangopi-sbc/tina-linux-5.4/blob/0d4903ebd9d2194ad914686d5b0fc1ddacf11a9d/drivers/video/fbdev/sunxi/disp2/disp/de/lowlevel_v2x/de_lcd.c#L388

	CCU->DSI_CLK_REG = (CCU->DSI_CLK_REG & ~ ((UINT32_C(7) << 24) | UINT32_C(0x0F) << 0)) |
		0x02 * (UINT32_C(1) << 24) |	// 010: PLL_VIDEO0(2X)	= 594 MHz
		//(UINT32_C(3) << 24) |	// 011: PLL_VIDEO1(2X)	= 594 MHz
		((UINT32_C(4) - 1) << 0) |
		0;

	CCU->DSI_CLK_REG |= UINT32_C(1) << 31;		// DSI_CLK_GATING

	CCU->DSI_BGR_REG |= UINT32_C(1) << 0;	// DSI_GATING
	CCU->DSI_BGR_REG |= UINT32_C(1) << 16;	// DSI_RST

//	PRINTF("allwnr_t113_get_dsi_freq()=%" PRIuFAST32 "\n", allwnr_t113_get_dsi_freq());
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
#elif CPUSTYLE_T507 || CPUSTYLE_H616
#endif /* */
}

// https://github.com/dumtux/Allwinner-H616/blob/e900407aca767f1429ba4a6a990b8b7c9f200914/u-boot/arch/arm/include/asm/arch-sunxi/lcdc.h#L105
// step6 - LVDS controller configuration
static void t113_LVDS_controller_configuration(const videomode_t * vdmode, unsigned lvds_num)
{
#if defined (TCONLCD_PTR)
#if CPUSTYLE_T507 || CPUSTYLE_H616
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

#elif CPUSTYLE_T113 || CPUSTYLE_F133
	// Documented as LCD_LVDS_ANA0_REG
	//const unsigned lvds_num = 0;	/* 0: LVDS0, 1: LVDS1 */
	// Step 5 LVDS digital logic configuration

	// Step 6 LVDS controller configuration
	// LVDS_HPREN_DRVC and LVDS_HPREN_DRV
	TCONLCD_PTR->LCD_LVDS_ANA_REG [lvds_num] =
		0x0F * (UINT32_C(1) << 20) |	// When LVDS signal is 18-bit, LVDS_HPREN_DRV=0x7; when LVDS signal is 24-bit, LVDS_HPREN_DRV=0xF;
		0x01 * (UINT32_C(1) << 24) |	// LVDS_HPREN_DRVC
		0x04 * (UINT32_C(1) << 17) |	// Configure LVDS0_REG_C (differential mode voltage) to 4; 100: 336 mV
		0x03 * (UINT32_C(1) << 8) |	// ?LVDS_REG_R Configure LVDS0_REG_V (common mode voltage) to 3;
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

	// так же инициализируется DSI_COMBO_PHY
#else
#endif
#endif /* defined (TCONLCD_PTR) */
}

// Set sequuence parameters
static void t113_set_sequence_parameters(const videomode_t * vdmode)
{
#if defined (TCONLCD_PTR)
	/* Accumulated parameters for this display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned HSYNC = vdmode->hsync;	/*  */
	const unsigned VSYNC = vdmode->vsync;	/*  */
	const unsigned LEFTMARGIN = HSYNC + vdmode->hbp;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = VSYNC + vdmode->vbp;	/* vertical delay before DE start */
	const unsigned HTOTAL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VTOTAL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */
	const unsigned HBP = vdmode->hbp;	/* horizontal back porch */
	const unsigned VBP = vdmode->vbp;	/* vertical back porch */

#if CPUSTYLE_H3
	// TCONLCD0 should be used
	TCONLCD_PTR->TCON1_BASIC0_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);	// TCON1_XI TCON1_YI
	TCONLCD_PTR->TCON1_BASIC1_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);	// LS_XO LS_YO
	TCONLCD_PTR->TCON1_BASIC2_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);	// TCON1_XO TCON1_YO
	TCONLCD_PTR->TCON1_BASIC3_REG = ((HTOTAL - 1) << 16) | ((HBP - 1) << 0);	// HT HBP
	TCONLCD_PTR->TCON1_BASIC4_REG = ((VTOTAL * 2) << 16) | ((VBP - 1) << 0);			// VT VBP
	TCONLCD_PTR->TCON1_BASIC5_REG = ((HSYNC - 1) << 16) | ((VSYNC - 1) << 0);			// HSPW VSPW
#elif CPUSTYLE_A64
	// timing0 (window)
	TCONLCD_PTR->TCON1_BASIC0_REG = (
		((WIDTH - 1) << 16) | ((HEIGHT - 1) << 0)
		);
	// timing1
	TCONLCD_PTR->TCON1_BASIC1_REG =
		((HTOTAL - 1) << 16) |		// TOTAL
		((LEFTMARGIN - 1) << 0) |	// HBP
		0;
	// timing2
	TCONLCD_PTR->TCON1_BASIC2_REG =
		((VTOTAL * 2) << 16) | 	// VT Tvt = (VT)/2 * Thsync
		((TOPMARGIN - 1) << 0) |		// VBP Tvbp = (VBP+1) * Thsync
		0;
	// timing3
	TCONLCD_PTR->TCON1_BASIC3_REG =
		((HSYNC - 1) << 16) |	// HSPW Thspw = (HSPW+1) * Tdclk
		((VSYNC - 1) << 0) |	// VSPW Tvspw = (VSPW+1) * Thsync
		0;

#else
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

#endif
#endif /* defined (TCONLCD_PTR) */
}


// Open module enable
static void t113_open_module_enable(const videomode_t * vdmode)
{
#if defined (TCONLCD_PTR)
#if CPUSTYLE_H3
	TCONLCD_PTR->TCON1_CTL_REG |= (UINT32_C(1) << 31);	// LCD_EN
	TCONLCD_PTR->TCON_GCTL_REG |= (UINT32_C(1) << 31);	// LCD_EN
#elif CPUSTYLE_A64
	TCONLCD_PTR->TCON1_CTL_REG |= (UINT32_C(1) << 31);	// LCD_EN
	TCONLCD_PTR->TCON_GCTL_REG |= (UINT32_C(1) << 31);	// LCD_EN
#else
	TCONLCD_PTR->LCD_CTL_REG |= (UINT32_C(1) << 31);	// LCD_EN
	TCONLCD_PTR->LCD_GCTL_REG |= (UINT32_C(1) << 31);	// LCD_EN
#endif
#endif /* defined (TCONLCD_PTR) */
}

// Step4 - Open volatile output
static void t113_open_IO_output(const videomode_t * vdmode)
{
#if defined (TCONLCD_PTR)

	int h_sync_active;	// 1 - negatibe pulses, 0 - positice pulses
	int v_sync_active;	// 1 - negatibe pulses, 0 - positice pulses
	int den_active;		// 1 - negatibe pulses, 0 - positice pulses
	int clk_active;		// 1 - negatibe pulses, 0 - positice pulses

	h_sync_active = vdmode->vsyncneg;
	v_sync_active = vdmode->hsyncneg;
	den_active = ! vdmode->deneg;
	clk_active = 0;

	uint32_t val;
	val = 0;

	if (! h_sync_active)
		val |= (UINT32_C(1) << 25);	// IO1_Inv 0 HSYMC
	if (! v_sync_active)
		val |= (UINT32_C(1) << 24);	// IO0_Inv - VSYNC
	if (! den_active)
		val |= (UINT32_C(1) << 27);	// IO3_Inv - DE
	if (!! clk_active)
		val |= (UINT32_C(1) << 26);	// IO2_Inv - DCLK

#if CPUSTYLE_H3
#elif CPUSTYLE_A64
#else
	// io_tristate
	TCONLCD_PTR->LCD_IO_TRI_REG = 0;
	// 5.1.6.20 0x0088 LCD volatile Polarity Register (Default Value: 0x0000_0000)
	// io_polarity
	TCONLCD_PTR->LCD_IO_POL_REG = val;
	//t113_tconlcd_set_dither(pdat);
	// 31: TCON_FRM_EN: 0: disable, 1: enable
	// 6: TCON_FRM_MODE_R: 0 - 6 bit, 1: 5 bit
	// 5: TCON_FRM_MODE_G: 0 - 6 bit, 1: 5 bit
	// 4: TCON_FRM_MODE_B: 0 - 6 bit, 1: 5 bit
	/* режим и формат выхода */
	TCONLCD_PTR->LCD_FRM_CTL_REG = TCON_FRM_MODE_VAL;
#endif
#endif /* defined (TCONLCD_PTR) */
}

// Step4 - Open volatile output
static void t113_tcontv_open_IO_output(const videomode_t * vdmode)
{
#if defined (TCONTV_PTR)

	int h_sync_active;	// 1 - negatibe pulses, 0 - positice pulses
	int v_sync_active;	// 1 - negatibe pulses, 0 - positice pulses
	int den_active;		// 1 - negatibe pulses, 0 - positice pulses
	int clk_active;		// 1 - negatibe pulses, 0 - positice pulses

	h_sync_active = vdmode->vsyncneg;
	v_sync_active = vdmode->hsyncneg;
	den_active = ! vdmode->deneg;
	clk_active = 0;

	uint32_t val;
	val = 0;

	if (! h_sync_active)
		val |= (UINT32_C(1) << 25);	// IO1_Inv 0 HSYMC
	if (! v_sync_active)
		val |= (UINT32_C(1) << 24);	// IO0_Inv - VSYNC
	if (! den_active)
		val |= (UINT32_C(1) << 27);	// IO3_Inv - DE
	if (!! clk_active)
		val |= (UINT32_C(1) << 26);	// IO2_Inv - DCLK

#if CPUSTYLE_H3
	// io_tristate
	TCONTV_PTR->TCON1_IO_TRI_REG = 0;
	// 5.1.6.20 0x0088 LCD volatile Polarity Register (Default Value: 0x0000_0000)
	// io_polarity
	TCONTV_PTR->TCON1_IO_POL_REG = val;
#elif CPUSTYLE_A64
	// io_tristate
	TCONTV_PTR->TCON1_IO_TRI_REG = 0;
	// 5.1.6.20 0x0088 LCD volatile Polarity Register (Default Value: 0x0000_0000)
	// io_polarity
	TCONTV_PTR->TCON1_IO_POL_REG = val;
#else
	// io_tristate
	TCONTV_PTR->TV_IO_TRI_REG = 0;
	// 5.1.6.20 0x0088 LCD volatile Polarity Register (Default Value: 0x0000_0000)
	// io_polarity
	TCONTV_PTR->TV_IO_POL_REG = val;

#endif
#endif /* defined (TCONTV_PTR) */
}

#if WITHLTDCHWVBLANKIRQ
#if defined (TCONLCD_PTR)

// overrealtime priority handler
static void TCONLCD_IRQHandler(void)
{
	//PRINTF("TCON_LCD_VB_IRQHandler:\n");
	const uint_fast32_t reg = TCONLCD_GINT0_REG;

	if (reg & LCD_VB_INT_FLAG)
	{
		//dbg_putchar('l');
		TCONLCD_GINT0_REG = reg & ~ LCD_VB_INT_FLAG;
		//PRINTF("TCON_LCD_VB_IRQHandler:LCD_GINT0_REG 0x%x\n", (unsigned) TCONLCD_PTR->LCD_GINT0_REG);
		hardware_ltdc_vblank(RTMIXIDLCD);	// Update framebuffer if needed
	}
}
#endif

#if defined (TCONTV_PTR)

// overrealtime priority handler
static void TCONTV_IRQHandler(void)
{
	//PRINTF("TCON_LCD_VB_IRQHandler:\n");
	const uint_fast32_t reg = TCONTV_GINT0_REG;

	if (reg & TVOUT_VB_INT_FLAG)
	{
		//dbg_putchar('t');
		TCONTV_GINT0_REG = reg & ~ TVOUT_VB_INT_FLAG;
		//PRINTF("TCON_LCD_VB_IRQHandler:LCD_GINT0_REG 0x%x\n", (unsigned) TCONLCD_PTR->LCD_GINT0_REG);
		hardware_ltdc_vblank(RTMIXIDTV);	// Update framebuffer if needed
	}
}

#endif

#if defined (TCONTV_PTR) && 0

// overrealtime priority handler
static void TCONTV_IRQHandler_XXXXX(void)
{
	//PRINTF("TCON_LCD_VB_IRQHandler:\n");
	const uint_fast32_t reg = TCONTV_GINT0_REG;

	if (reg & TVOUT_VB_INT_FLAG)
	{
		TCONTV_GINT0_REG = reg & ~ TVOUT_VB_INT_FLAG;
		if ((TCONTV_PTR->TV_DEBUG_REG & (UINT32_C(1) << 28)) == 0) // TV_FIELD_POL: 0: Second field, 1: First field
		{
			hardware_ltdc_vblank(RTMIXIDTV);	// Update framebuffer if needed
		}
	}
}

#endif /* defined (TCONTV_PTR) */

#endif /* WITHLTDCHWVBLANKIRQ */

// Set and open interrupt function
static void t113_set_and_open_interrupt_function(void)
{
#if defined (TCONLCD_PTR)
	// enabling the irq after io settings
#if WITHLTDCHWVBLANKIRQ
	TCONLCD_GINT0_REG = LCD_VB_INT_EN;
	arm_hardware_set_handler_overrealtime(TCONLCD_IRQ, TCONLCD_IRQHandler);
	//PRINTF("TCON_LCD_set_handler:TCON_LCD0->LCD_GINT0_REG 0x%x\n", TCON_LCD0->LCD_GINT0_REG);
#endif /* WITHLTDCHWVBLANKIRQ */
#endif /* defined (TCONLCD_PTR) */
}

// Set and open interrupt function
static void t113_set_and_open_tcontv_interrupt_function(void)
{
#if defined (TCONTV_PTR)
	// enabling the irq after io settings
#if WITHLTDCHWVBLANKIRQ
	TCONTV_GINT0_REG = TVOUT_VB_INT_EN;
	arm_hardware_set_handler_overrealtime(TCONTV_IRQ, TCONTV_IRQHandler);
	//PRINTF("TCON_LCD_set_handler:TCON_LCD0->LCD_GINT0_REG 0x%x\n", TCON_LCD0->LCD_GINT0_REG);
#endif /* WITHLTDCHWVBLANKIRQ */
#endif /* defined (TCONTV_PTR) */
}

#if defined (TCONTV_PTR)


/* +++++ */

//---------------------------------------

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

#define UI_BYTE_PER_PIXEL     4

#define DE2_FORMAT DE2_FORMAT_ABGR_8888

enum disp_tv_mode
{
	DISP_TV_MOD_480I                = 0,
	DISP_TV_MOD_576I                = 1,
	DISP_TV_MOD_480P                = 2,
	DISP_TV_MOD_576P                = 3,
	DISP_TV_MOD_720P_50HZ           = 4,
	DISP_TV_MOD_720P_60HZ           = 5,
	DISP_TV_MOD_1080I_50HZ          = 6,
	DISP_TV_MOD_1080I_60HZ          = 7,
	DISP_TV_MOD_1080P_24HZ          = 8,
	DISP_TV_MOD_1080P_50HZ          = 9,
	DISP_TV_MOD_1080P_60HZ          = 0xa,
	DISP_TV_MOD_1080P_24HZ_3D_FP    = 0x17,
	DISP_TV_MOD_720P_50HZ_3D_FP     = 0x18,
	DISP_TV_MOD_720P_60HZ_3D_FP     = 0x19,
	DISP_TV_MOD_1080P_25HZ          = 0x1a,
	DISP_TV_MOD_1080P_30HZ          = 0x1b,
	DISP_TV_MOD_PAL                 = 0xb,
	DISP_TV_MOD_PAL_SVIDEO          = 0xc,
	DISP_TV_MOD_NTSC                = 0xe,
	DISP_TV_MOD_NTSC_SVIDEO         = 0xf,
	DISP_TV_MOD_PAL_M               = 0x11,
	DISP_TV_MOD_PAL_M_SVIDEO        = 0x12,
	DISP_TV_MOD_PAL_NC              = 0x14,
	DISP_TV_MOD_PAL_NC_SVIDEO       = 0x15,
	DISP_TV_MOD_3840_2160P_30HZ     = 0x1c,
	DISP_TV_MOD_3840_2160P_25HZ     = 0x1d,
	DISP_TV_MOD_3840_2160P_24HZ     = 0x1e,
	DISP_TV_MOD_4096_2160P_24HZ     = 0x1f,
	DISP_TV_MOD_4096_2160P_25HZ     = 0x20,
	DISP_TV_MOD_4096_2160P_30HZ     = 0x21,
	DISP_TV_MOD_3840_2160P_60HZ     = 0x22,
	DISP_TV_MOD_4096_2160P_60HZ     = 0x23,
	DISP_TV_MOD_3840_2160P_50HZ     = 0x24,
	DISP_TV_MOD_4096_2160P_50HZ     = 0x25,
	DISP_TV_MOD_2560_1440P_60HZ     = 0x26,
	DISP_TV_MOD_1440_2560P_70HZ     = 0x27,
	DISP_TV_MOD_1080_1920P_60HZ	= 0x28,
	DISP_TV_MOD_1280_1024P_60HZ     = 0x41,
	DISP_TV_MOD_1024_768P_60HZ      = 0x42,
	DISP_TV_MOD_900_540P_60HZ       = 0x43,
	DISP_TV_MOD_1920_720P_60HZ      = 0x44,

	/*Just for the solution of hdmi edid detailed timiming block*/
	DISP_HDMI_MOD_DT0                = 0x4a,
	DISP_HDMI_MOD_DT1                = 0x4b,
	DISP_HDMI_MOD_DT2                = 0x4c,
	DISP_HDMI_MOD_DT3                = 0x4d,
	/*
	 * vga
	 * NOTE:macro'value of new solution must between
	 * DISP_VGA_MOD_640_480P_60 and DISP_VGA_MOD_MAX_NUM
	 * or you have to modify is_vag_mode function in drv_tv.h
	 */
	DISP_VGA_MOD_640_480P_60         = 0x50,
	DISP_VGA_MOD_800_600P_60         = 0x51,
	DISP_VGA_MOD_1024_768P_60        = 0x52,
	DISP_VGA_MOD_1280_768P_60        = 0x53,
	DISP_VGA_MOD_1280_800P_60        = 0x54,
	DISP_VGA_MOD_1366_768P_60        = 0x55,
	DISP_VGA_MOD_1440_900P_60        = 0x56,
	DISP_VGA_MOD_1920_1080P_60       = 0x57,
	DISP_VGA_MOD_1920_1200P_60 = 0x58,
	DISP_TV_MOD_3840_1080P_30 = 0x59,
	DISP_VGA_MOD_1280_720P_60        = 0x5a,
	DISP_VGA_MOD_1600_900P_60        = 0x5b,
	DISP_VGA_MOD_MAX_NUM             = 0x5c,

	DISP_TV_MODE_NUM
};

/* tv encoder registers offset */
#define TVE_000    (0x000)
#define TVE_004    (0x004)
#define TVE_008    (0x008)
#define TVE_00C    (0x00c)
#define TVE_010    (0x010)
#define TVE_014    (0x014)
#define TVE_018    (0x018)
#define TVE_01C    (0x01c)
#define TVE_020    (0x020)
#define TVE_024    (0x024)
#define TVE_030    (0X030)
#define TVE_034    (0x034)
#define TVE_038    (0x038)
#define TVE_03C    (0x03c)
#define TVE_040    (0x040)
#define TVE_044    (0x044)
#define TVE_048    (0x048)
#define TVE_04C    (0x04c)
#define TVE_0F8    (0x0f8)
#define TVE_0FC    (0x0fc)
#define TVE_100    (0x100)
#define TVE_104    (0x104)
#define TVE_108    (0x108)
#define TVE_10C    (0x10c)
#define TVE_110    (0x110)
#define TVE_114    (0x114)
#define TVE_118    (0x118)
#define TVE_11C    (0x11c)
#define TVE_120    (0x120)
#define TVE_124    (0x124)
#define TVE_128    (0x128)
#define TVE_12C    (0x12c)
#define TVE_130    (0x130)
#define TVE_134    (0x134)
#define TVE_138    (0x138)
#define TVE_13C    (0x13C)
#define TVE_300    (0x300)
#define TVE_304    (0x304)
#define TVE_308    (0x308)
#define TVE_30C    (0x30c)
#define TVE_380    (0x380)
#define TVE_3A0    (0x3a0)

#define TVE_TOP_020    (0x020)
#define TVE_TOP_024    (0x024)
#define TVE_TOP_028    (0x028)	// TVEE_TOP-TVE_DAC_CFG
#define TVE_TOP_02C    (0x02C)
#define TVE_TOP_030    (0x030)
#define TVE_TOP_034    (0x034)
#define TVE_TOP_0F0    (0x0F0)

#define TVE_GET_REG_BASE(sel)                   (TVENCODER_BASE)
#define TVE_WUINT32(sel,offset,value)           (*((volatile uint32_t *)( TVE_GET_REG_BASE(sel) + (offset) ))=(value))
#define TVE_RUINT32(sel,offset)                 (*((volatile uint32_t *)( TVE_GET_REG_BASE(sel) + (offset) )))
#define TVE_SET_BIT(sel,offset,bit)             (*((volatile uint32_t *)( TVE_GET_REG_BASE(sel) + (offset) )) |= (bit))
#define TVE_CLR_BIT(sel,offset,bit)             (*((volatile uint32_t *)( TVE_GET_REG_BASE(sel) + (offset) )) &= (~ (bit)))
#define TVE_INIT_BIT(sel,offset,c,s)            (*((volatile uint32_t *)( TVE_GET_REG_BASE(sel) + (offset) )) = \
                                                (((*(volatile uint32_t *)( TVE_GET_REG_BASE(sel) + (offset) )) & (~ (c))) | (s)))

#define TVE_TOP_GET_REG_BASE                    (TVE_TOP_BASE)
#define TVE_TOP_WUINT32(offset,value)           (*((volatile uint32_t *)( TVE_TOP_GET_REG_BASE + (offset) ))=(value))
#define TVE_TOP_RUINT32(offset)                 (*((volatile uint32_t *)( TVE_TOP_GET_REG_BASE + (offset) )))
#define TVE_TOP_SET_BIT(offset,bit)             (*((volatile uint32_t *)( TVE_TOP_GET_REG_BASE + (offset) )) |= (bit))
#define TVE_TOP_CLR_BIT(offset,bit)             (*((volatile uint32_t *)( TVE_TOP_GET_REG_BASE + (offset) )) &= (~ (bit)))
#define TVE_TOP_INIT_BIT(offset,c,s)            (*((volatile uint32_t *)( TVE_TOP_GET_REG_BASE + (offset) )) = \
                                               (((*(volatile uint32_t *)( TVE_TOP_GET_REG_BASE + (offset) )) & (~ (c))) | (s)))

#ifdef TVENCODER_BASE


static int32_t tve_low_init(uint32_t sel /* , uint32_t *dac_no, uint32_t *cali, int32_t *offset, uint32_t *dac_type, uint32_t num */ )
{
	uint32_t i = 0;

	int32_t dac_cal = 512;

//	uint32_t dac_type_set = 0;

	/* init dac config as disable status first */
//	for (i = 0; i < TVE_DAC_NUM; i++)
//		dac_type_info[sel][i] = 7;

//	for (i = 0; i < num; i++) {
//		dac_info[sel][dac_no[i]] = 1;
//		dac_type_info[sel][dac_no[i]] = dac_type[i];
//	}

//	for (i = 0; i < TVE_DAC_NUM; i++) {
//		if (dac_info[sel][i]) {
//			dac_cal = (int32_t)cali[i] + offset[i];
			TVE_TOP_WUINT32(TVE_TOP_020 + 0x020*i,
					(i<<4)|((sel+1)<<0));
			TVE_TOP_WUINT32(TVE_TOP_02C + 0x020*i, 0x023a);
			TVE_TOP_WUINT32(TVE_TOP_030 + 0x020*i, 0x0010);
			TVE_TOP_WUINT32(TVE_TOP_028 + 0x020*i,
					((0x8000| dac_cal )<<16)|0x4200);
//		}

//	}

//	for (i = 0; i < TVE_DAC_NUM; i++)
//		dac_type_set |= dac_type_info[sel][i]<<(4+3*i);
	TVE_WUINT32(sel, TVE_008, 0 /*dac_type_set*/ );

	return 0;
}

static int32_t tve_low_dac_enable(uint32_t sel)
{
	uint32_t i = 0;

//	for (i = 0; i < TVE_DAC_NUM; i++) {
//		if (dac_info[sel][i])
			TVE_TOP_SET_BIT(TVE_TOP_028 + 0x020*i, 1<<0);
//	}

	return 0;
}

static int32_t tve_low_dac_disable(uint32_t sel)
{
	uint32_t i = 0;

//	for (i = 0; i < TVE_DAC_NUM; i++) {
//		if (dac_info[sel][i])
			TVE_TOP_CLR_BIT(TVE_TOP_028 + 0x020*i, 1<<0);
//	}

	return 0;
}

static int32_t tve_low_open(uint32_t sel)
{
	TVE_SET_BIT(sel, TVE_000, 0x1<<31);
	TVE_SET_BIT(sel, TVE_000, 0x1<<0);
	return 0;
}

static int32_t tve_low_close(uint32_t sel)
{
	TVE_CLR_BIT(sel, TVE_000, 0x1<<31);
	TVE_CLR_BIT(sel, TVE_000, 0x1<<0);
	return 0;
}

static int32_t tve_low_set_tv_mode(uint32_t sel, enum disp_tv_mode mode, uint32_t cali)
{
	uint32_t deflick  = 0;
	uint32_t reg_sync = 0;

	switch (mode) {
	case DISP_TV_MOD_PAL:
		deflick  = 1;
		reg_sync = 0x2005000a;
		TVE_WUINT32(sel, TVE_004, 0x07060001 | (1<<19)); //19 bit: Input_Chroma_Data_Sampling_Rate_Sel => 0: 4:4:4, 1: 4:2:2
		TVE_WUINT32(sel, TVE_00C, 0x02004000);
		TVE_WUINT32(sel, TVE_010, 0x2a098acb);
		TVE_WUINT32(sel, TVE_014, 0x008a0018);
		TVE_WUINT32(sel, TVE_018, 0x00000016);
		TVE_WUINT32(sel, TVE_01C, 0x00160271);
		TVE_WUINT32(sel, TVE_020, 0x00fc00fc);
		TVE_WUINT32(sel, TVE_100, 0x00000000);
		TVE_WUINT32(sel, TVE_104, 0x00000001);
		TVE_WUINT32(sel, TVE_108, 0x00000005);
		TVE_WUINT32(sel, TVE_10C, 0x00002929);
		TVE_WUINT32(sel, TVE_110, 0x00000000);
		TVE_WUINT32(sel, TVE_114, 0x0016447e);
		TVE_WUINT32(sel, TVE_118, 0x0000a8a8);
		TVE_WUINT32(sel, TVE_11C, 0x001000fc);
		TVE_WUINT32(sel, TVE_124, 0x000005a0);
		TVE_WUINT32(sel, TVE_128, 0x00010000);
		TVE_WUINT32(sel, TVE_12C, 0x00000101);
		TVE_WUINT32(sel, TVE_130, 0x200f000c);
		TVE_WUINT32(sel, TVE_134, 0x00000000);
		TVE_WUINT32(sel, TVE_138, 0x00000000);
		TVE_WUINT32(sel, TVE_13C, 0x00000000);
		TVE_WUINT32(sel, TVE_3A0, 0x00030001);
		TVE_WUINT32(sel, TVE_0F8, 0x00000200);
		TVE_WUINT32(sel, TVE_120, 0x01e80320);
		TVE_WUINT32(sel, TVE_380, 0x00000000);
		TVE_INIT_BIT(sel, TVE_004, 0x1<<16, 1<<16);
		TVE_WUINT32(sel, TVE_000, 0x00300000);
		TVE_CLR_BIT(sel, TVE_000, 0x1<<0);
		TVE_WUINT32(sel, TVE_130, reg_sync);
		TVE_WUINT32(sel, TVE_014, 0x00820020);
		TVE_WUINT32(sel, TVE_130, 0x20050013);
		TVE_WUINT32(sel, TVE_380, (deflick == 0)
					? 0x00000000 : 0x0<<10 | 0x3<<8);
		break;
	case DISP_TV_MOD_NTSC:
		deflick  = 1;
		reg_sync = 0x20050368;
		TVE_WUINT32(sel, TVE_004, 0x07060000 | (1<<19)); //19 bit: Input_Chroma_Data_Sampling_Rate_Sel => 0: 4:4:4, 1: 4:2:2
		TVE_WUINT32(sel, TVE_00C, 0x02004000);
		TVE_WUINT32(sel, TVE_010, 0x21f07c1f);
		TVE_WUINT32(sel, TVE_014, 0x00760020);
		TVE_WUINT32(sel, TVE_018, 0x00000016);
		TVE_WUINT32(sel, TVE_01C, 0x0016020d);
		TVE_WUINT32(sel, TVE_020, 0x00f000f0);
		TVE_WUINT32(sel, TVE_100, 0x00000001);
		TVE_WUINT32(sel, TVE_104, 0x00000000);
		TVE_WUINT32(sel, TVE_108, 0x00000002);
		TVE_WUINT32(sel, TVE_10C, 0x0000004f);
		TVE_WUINT32(sel, TVE_110, 0x00000000);
		TVE_WUINT32(sel, TVE_114, 0x0016447e);
		TVE_WUINT32(sel, TVE_118, 0x0000a0a0);
		TVE_WUINT32(sel, TVE_11C, 0x001000f0);
		TVE_WUINT32(sel, TVE_124, 0x000005a0);
		TVE_WUINT32(sel, TVE_128, 0x00000000);
		TVE_WUINT32(sel, TVE_12C, 0x00000101);
		TVE_WUINT32(sel, TVE_130, 0x200f000c);
		TVE_WUINT32(sel, TVE_134, 0x00000000);
		TVE_WUINT32(sel, TVE_138, 0x00000000);
		TVE_WUINT32(sel, TVE_13C, 0x00000001);
		TVE_WUINT32(sel, TVE_0F8, 0x00000200);
		TVE_WUINT32(sel, TVE_120, 0x01e80320);
		TVE_WUINT32(sel, TVE_380, 0x00000000);
		TVE_INIT_BIT(sel, TVE_004, 0x1<<16, 1<<16);
		TVE_WUINT32(sel, TVE_000, 0x00300000);
		TVE_CLR_BIT(sel, TVE_000, 0x1<<0);
		TVE_WUINT32(sel, TVE_130, reg_sync);
		TVE_WUINT32(sel, TVE_130, 0x20050364);
		TVE_WUINT32(sel, TVE_380, (deflick == 0)
					? 0x00000000 : 0x0<<10 | 0x3<<8);
		break;
	case DISP_TV_MOD_480I:
		TVE_WUINT32(sel, TVE_004, 0x07060000);
		TVE_WUINT32(sel, TVE_00C, 0x02004000);
		TVE_WUINT32(sel, TVE_010, 0x21f07c1f);
		TVE_WUINT32(sel, TVE_014, 0x00760020);
		TVE_WUINT32(sel, TVE_018, 0x00000016);
		TVE_WUINT32(sel, TVE_01C, 0x0016020d);
		TVE_WUINT32(sel, TVE_020, 0x00f000f0);
		TVE_WUINT32(sel, TVE_100, 0x00000001);
		TVE_WUINT32(sel, TVE_104, 0x00000000);
		TVE_WUINT32(sel, TVE_108, 0x00000002);
		TVE_WUINT32(sel, TVE_10C, 0x0000004f);
		TVE_WUINT32(sel, TVE_110, 0x00000000);
		TVE_WUINT32(sel, TVE_114, 0x0016447e);
		TVE_WUINT32(sel, TVE_118, 0x0000a0a0);
		TVE_WUINT32(sel, TVE_11C, 0x001000f0);
		TVE_WUINT32(sel, TVE_124, 0x000005a0);
		TVE_WUINT32(sel, TVE_128, 0x00000000);
		TVE_WUINT32(sel, TVE_12C, 0x00000101);
		TVE_WUINT32(sel, TVE_130, 0x200f000c);
		TVE_WUINT32(sel, TVE_134, 0x00000000);
		TVE_WUINT32(sel, TVE_138, 0x00000000);
		TVE_WUINT32(sel, TVE_13C, 0x00000001);
		TVE_WUINT32(sel, TVE_0F8, 0x00000200);
		TVE_WUINT32(sel, TVE_120, 0x01e80320);
		TVE_WUINT32(sel, TVE_380, 0x00000000);
		TVE_INIT_BIT(sel, TVE_004, 0x1<<16, 1<<16);
		TVE_WUINT32(sel, TVE_000, 0x00300000);

		break;
	case DISP_TV_MOD_576I:
		TVE_WUINT32(sel, TVE_004, 0x07060001);
		TVE_WUINT32(sel, TVE_00C, 0x02004000);
		TVE_WUINT32(sel, TVE_010, 0x2a098acb);
		TVE_WUINT32(sel, TVE_014, 0x008a0018);
		TVE_WUINT32(sel, TVE_018, 0x00000016);
		TVE_WUINT32(sel, TVE_01C, 0x00160271);
		TVE_WUINT32(sel, TVE_020, 0x00fc00fc);
		TVE_WUINT32(sel, TVE_100, 0x00000000);
		TVE_WUINT32(sel, TVE_104, 0x00000001);
		TVE_WUINT32(sel, TVE_108, 0x00000005);
		TVE_WUINT32(sel, TVE_10C, 0x00002929);
		TVE_WUINT32(sel, TVE_110, 0x00000000);
		TVE_WUINT32(sel, TVE_114, 0x0016447e);
		TVE_WUINT32(sel, TVE_118, 0x0000a8a8);
		TVE_WUINT32(sel, TVE_11C, 0x001000fc);
		TVE_WUINT32(sel, TVE_124, 0x000005a0);
		TVE_WUINT32(sel, TVE_128, 0x00010000);
		TVE_WUINT32(sel, TVE_12C, 0x00000101);
		TVE_WUINT32(sel, TVE_130, 0x200f000c);
		TVE_WUINT32(sel, TVE_134, 0x00000000);
		TVE_WUINT32(sel, TVE_138, 0x00000000);
		TVE_WUINT32(sel, TVE_13C, 0x00000000);
		TVE_WUINT32(sel, TVE_3A0, 0x00030001);
		TVE_WUINT32(sel, TVE_0F8, 0x00000200);
		TVE_WUINT32(sel, TVE_120, 0x01e80320);
		TVE_WUINT32(sel, TVE_380, 0x00000000);
		TVE_INIT_BIT(sel, TVE_004, 0x1<<16, 1<<16);
		TVE_WUINT32(sel, TVE_000, 0x00300000);
		break;
	case DISP_TV_MOD_480P:
		TVE_WUINT32(sel, TVE_004, 0x07040002);
		TVE_WUINT32(sel, TVE_00C, 0x00002000);
		TVE_WUINT32(sel, TVE_014, 0x00760020);
		TVE_WUINT32(sel, TVE_018, 0x00000016);
		TVE_WUINT32(sel, TVE_01C, 0x002C020D);
		TVE_WUINT32(sel, TVE_020, 0x00fc00fc);
		TVE_WUINT32(sel, TVE_100, 0x00000001);
		TVE_WUINT32(sel, TVE_104, 0x00000000);
		TVE_WUINT32(sel, TVE_108, 0x00000002);
		TVE_WUINT32(sel, TVE_110, 0x00000000);
		TVE_WUINT32(sel, TVE_114, 0x0016447e);
		TVE_WUINT32(sel, TVE_118, 0x0000a8a8);
		TVE_WUINT32(sel, TVE_11C, 0x001000f0);
		TVE_WUINT32(sel, TVE_124, 0x000005a0);
		TVE_WUINT32(sel, TVE_128, 0x00010000);
		TVE_WUINT32(sel, TVE_12C, 0x00000101);
		TVE_WUINT32(sel, TVE_130, 0x000E000C);
		TVE_WUINT32(sel, TVE_134, 0x00000000);
		TVE_WUINT32(sel, TVE_138, 0x00000000);
		TVE_WUINT32(sel, TVE_13C, 0x00000000);
		TVE_WUINT32(sel, TVE_0F8, 0x00000200);
		TVE_WUINT32(sel, TVE_120, 0x01e80320);
		TVE_WUINT32(sel, TVE_380, 0x00000000);
		TVE_INIT_BIT(sel, TVE_004, 0x1<<16, 0<<16);
		TVE_WUINT32(sel, TVE_000, 0x00400000);
		break;
	case DISP_TV_MOD_576P:
		TVE_WUINT32(sel, TVE_004, 0x07060003);
		TVE_WUINT32(sel, TVE_00C, 0x00002000);
		TVE_WUINT32(sel, TVE_014, 0x008a0018);
		TVE_WUINT32(sel, TVE_018, 0x00000016);
		TVE_WUINT32(sel, TVE_01C, 0x002C0271);
		TVE_WUINT32(sel, TVE_020, 0x00fc00fc);
		TVE_WUINT32(sel, TVE_100, 0x00000000);
		TVE_WUINT32(sel, TVE_104, 0x00000001);
		TVE_WUINT32(sel, TVE_108, 0x00000005);
		TVE_WUINT32(sel, TVE_110, 0x00000000);
		TVE_WUINT32(sel, TVE_114, 0x0016447e);
		TVE_WUINT32(sel, TVE_118, 0x0000a8a8);
		TVE_WUINT32(sel, TVE_11C, 0x001000F0);
		TVE_WUINT32(sel, TVE_124, 0x000005a0);
		TVE_WUINT32(sel, TVE_128, 0x00010000);
		TVE_WUINT32(sel, TVE_12C, 0x00000101);
		TVE_WUINT32(sel, TVE_130, 0x800B000C);
		TVE_WUINT32(sel, TVE_134, 0x00000000);
		TVE_WUINT32(sel, TVE_138, 0x00000000);
		TVE_WUINT32(sel, TVE_13C, 0x00000000);
		TVE_WUINT32(sel, TVE_0F8, 0x00000200);
		TVE_WUINT32(sel, TVE_120, 0x01e80320);
		TVE_WUINT32(sel, TVE_380, 0x00000000);
		TVE_INIT_BIT(sel, TVE_004, 0x1<<16, 0<<16);
		TVE_WUINT32(sel, TVE_000, 0x00400000);
		break;
	case DISP_TV_MOD_720P_60HZ:
		TVE_WUINT32(sel, TVE_004, 0x0706000a);
		TVE_WUINT32(sel, TVE_00C, 0x00003000);
		TVE_WUINT32(sel, TVE_014, 0x01040046);
		TVE_WUINT32(sel, TVE_018, 0x05000046);
		TVE_WUINT32(sel, TVE_01C, 0x001902EE);
		TVE_WUINT32(sel, TVE_020, 0x00fc00fc);
		TVE_WUINT32(sel, TVE_100, 0x00000000);
		TVE_WUINT32(sel, TVE_104, 0x00000001);
		TVE_WUINT32(sel, TVE_108, 0x00000005);
		TVE_WUINT32(sel, TVE_110, 0x00000000);
		TVE_WUINT32(sel, TVE_114, 0xDC280228);
		TVE_WUINT32(sel, TVE_118, 0x0000a8a8);
		TVE_WUINT32(sel, TVE_11C, 0x001000F0);
		TVE_WUINT32(sel, TVE_124, 0x00000500);
		TVE_WUINT32(sel, TVE_128, 0x00030000);
		TVE_WUINT32(sel, TVE_12C, 0x00000101);
		TVE_WUINT32(sel, TVE_130, 0x000C0008);
		TVE_WUINT32(sel, TVE_134, 0x00000000);
		TVE_WUINT32(sel, TVE_138, 0x00000000);
		TVE_WUINT32(sel, TVE_13C, 0x07000000);
		TVE_WUINT32(sel, TVE_0F8, 0x00000200);
		TVE_WUINT32(sel, TVE_120, 0x01e80320);
		TVE_WUINT32(sel, TVE_380, 0x00000000);
		TVE_INIT_BIT(sel, TVE_004, 0x1<<16, 0<<16);
		TVE_WUINT32(sel, TVE_000, 0x00000000);
		break;
	case DISP_TV_MOD_720P_50HZ:
		TVE_WUINT32(sel, TVE_004, 0x0706000a);
		TVE_WUINT32(sel, TVE_00C, 0x00003000);
		TVE_WUINT32(sel, TVE_014, 0x01040190);
		TVE_WUINT32(sel, TVE_018, 0x05000190);
		TVE_WUINT32(sel, TVE_01C, 0x001902EE);
		TVE_WUINT32(sel, TVE_020, 0x00fc00fc);
		TVE_WUINT32(sel, TVE_100, 0x00000000);
		TVE_WUINT32(sel, TVE_104, 0x00000001);
		TVE_WUINT32(sel, TVE_108, 0x00000005);
		TVE_WUINT32(sel, TVE_110, 0x00000000);
		TVE_WUINT32(sel, TVE_114, 0xDC280228);
		TVE_WUINT32(sel, TVE_118, 0x0000a8a8);
		TVE_WUINT32(sel, TVE_11C, 0x001000F0);
		TVE_WUINT32(sel, TVE_124, 0x00000500);
		TVE_WUINT32(sel, TVE_128, 0x00030000);
		TVE_WUINT32(sel, TVE_12C, 0x00000101);
		TVE_WUINT32(sel, TVE_130, 0x000E000C);
		TVE_WUINT32(sel, TVE_134, 0x00000000);
		TVE_WUINT32(sel, TVE_138, 0x00000000);
		TVE_WUINT32(sel, TVE_13C, 0x07000000);
		TVE_WUINT32(sel, TVE_0F8, 0x00000200);
		TVE_WUINT32(sel, TVE_120, 0x01e80320);
		TVE_WUINT32(sel, TVE_380, 0x00000000);
		TVE_INIT_BIT(sel, TVE_004, 0x1<<16, 0<<16);
		TVE_WUINT32(sel, TVE_000, 0x00000000);
		break;
	case DISP_TV_MOD_1080I_60HZ:
		TVE_WUINT32(sel, TVE_004, 0x0706000c);
		TVE_WUINT32(sel, TVE_00C, 0x00003010);
		TVE_WUINT32(sel, TVE_014, 0x00C0002C);
		TVE_WUINT32(sel, TVE_018, 0x0370002C);
		TVE_WUINT32(sel, TVE_01C, 0x00140465);
		TVE_WUINT32(sel, TVE_020, 0x00fc00fc);
		TVE_WUINT32(sel, TVE_100, 0x00000000);
		TVE_WUINT32(sel, TVE_104, 0x00000000);
		TVE_WUINT32(sel, TVE_108, 0x00000005);
		TVE_WUINT32(sel, TVE_110, 0x00000000);
		TVE_WUINT32(sel, TVE_114, 0x582C442C);
		TVE_WUINT32(sel, TVE_118, 0x0000a8a8);
		TVE_WUINT32(sel, TVE_11C, 0x001000F0);
		TVE_WUINT32(sel, TVE_124, 0x00000780);
		TVE_WUINT32(sel, TVE_128, 0x00030000);
		TVE_WUINT32(sel, TVE_12C, 0x00000101);
		TVE_WUINT32(sel, TVE_130, 0x000E0008);
		TVE_WUINT32(sel, TVE_134, 0x00000000);
		TVE_WUINT32(sel, TVE_138, 0x00000000);
		TVE_WUINT32(sel, TVE_13C, 0x07000000);
		TVE_WUINT32(sel, TVE_0F8, 0x00000200);
		TVE_WUINT32(sel, TVE_120, 0x01e80320);
		TVE_WUINT32(sel, TVE_380, 0x00000000);
		TVE_INIT_BIT(sel, TVE_004, 0x1<<16, 0<<16);
		TVE_WUINT32(sel, TVE_000, 0x00000000);
		break;
	case DISP_TV_MOD_1080I_50HZ:
		TVE_WUINT32(sel, TVE_004, 0x0706000c);
		TVE_WUINT32(sel, TVE_00C, 0x00003010);
		TVE_WUINT32(sel, TVE_014, 0x00C001E4);
		TVE_WUINT32(sel, TVE_018, 0x03700108);
		TVE_WUINT32(sel, TVE_01C, 0x00140465);
		TVE_WUINT32(sel, TVE_020, 0x00fc00fc);
		TVE_WUINT32(sel, TVE_100, 0x00000000);
		TVE_WUINT32(sel, TVE_104, 0x00000000);
		TVE_WUINT32(sel, TVE_108, 0x00000005);
		TVE_WUINT32(sel, TVE_110, 0x00000000);
		TVE_WUINT32(sel, TVE_114, 0x582C442C);
		TVE_WUINT32(sel, TVE_118, 0x0000a8a8);
		TVE_WUINT32(sel, TVE_11C, 0x001000F0);
		TVE_WUINT32(sel, TVE_124, 0x00000780);
		TVE_WUINT32(sel, TVE_128, 0x00030000);
		TVE_WUINT32(sel, TVE_12C, 0x00000101);
		TVE_WUINT32(sel, TVE_130, 0x000E0008);
		TVE_WUINT32(sel, TVE_134, 0x00000000);
		TVE_WUINT32(sel, TVE_138, 0x00000000);
		TVE_WUINT32(sel, TVE_13C, 0x07000000);
		TVE_WUINT32(sel, TVE_0F8, 0x00000200);
		TVE_WUINT32(sel, TVE_120, 0x01e80320);
		TVE_WUINT32(sel, TVE_380, 0x00000000);
		TVE_INIT_BIT(sel, TVE_004, 0x1<<16, 0<<16);
		TVE_WUINT32(sel, TVE_000, 0x00000000);
		break;
	case DISP_TV_MOD_1080P_60HZ:
		TVE_WUINT32(sel, TVE_004, 0x0004000e);
		TVE_WUINT32(sel, TVE_00C, 0x00003010);
		TVE_WUINT32(sel, TVE_010, 0x21f07c1f);
		TVE_WUINT32(sel, TVE_014, 0x00c0002c);
		TVE_WUINT32(sel, TVE_018, 0x07bc002c);
		TVE_WUINT32(sel, TVE_01C, 0x00290465);
		TVE_WUINT32(sel, TVE_020, 0x00fc00fc);
		TVE_WUINT32(sel, TVE_100, 0x00000001);
		TVE_WUINT32(sel, TVE_104, 0x00000000);
		TVE_WUINT32(sel, TVE_108, 0x00000005);
		TVE_WUINT32(sel, TVE_110, 0x00000000);
		TVE_WUINT32(sel, TVE_114, 0x582c022c);
		TVE_WUINT32(sel, TVE_118, 0x0000a0a0);
		TVE_WUINT32(sel, TVE_11C, 0x001000f0);
		TVE_WUINT32(sel, TVE_124, 0x00000780);
		TVE_WUINT32(sel, TVE_128, 0x00030000);
		TVE_WUINT32(sel, TVE_12C, 0x00000101);
		TVE_WUINT32(sel, TVE_130, 0x000e000c);
		TVE_WUINT32(sel, TVE_134, 0x00000000);
		TVE_WUINT32(sel, TVE_138, 0x00000000);
		TVE_WUINT32(sel, TVE_13C, 0x07000000);
		TVE_WUINT32(sel, TVE_0F8, 0x00000200);
		TVE_WUINT32(sel, TVE_120, 0x01e80320);
		TVE_WUINT32(sel, TVE_380, 0x00000000);
		TVE_INIT_BIT(sel, TVE_004, 0x1<<16, 0<<16);
		TVE_WUINT32(sel, TVE_000, 0x00000000);
		break;
	case DISP_TV_MOD_1080P_50HZ:
		TVE_WUINT32(sel, TVE_004, 0x0004000e);
		TVE_WUINT32(sel, TVE_00C, 0x00003010);
		TVE_WUINT32(sel, TVE_010, 0x21f07c1f);
		TVE_WUINT32(sel, TVE_014, 0x00C001E4);
		TVE_WUINT32(sel, TVE_018, 0x07BC01E4);
		TVE_WUINT32(sel, TVE_01C, 0x00290465);
		TVE_WUINT32(sel, TVE_020, 0x00fc00fc);
		TVE_WUINT32(sel, TVE_100, 0x00000000);
		TVE_WUINT32(sel, TVE_104, 0x00000000);
		TVE_WUINT32(sel, TVE_108, 0x00000005);
		TVE_WUINT32(sel, TVE_110, 0x00000000);
		TVE_WUINT32(sel, TVE_114, 0x582c022c);
		TVE_WUINT32(sel, TVE_118, 0x0000a0a0);
		TVE_WUINT32(sel, TVE_11C, 0x001000f0);
		TVE_WUINT32(sel, TVE_124, 0x00000780);
		TVE_WUINT32(sel, TVE_128, 0x00030000);
		TVE_WUINT32(sel, TVE_12C, 0x00000101);
		TVE_WUINT32(sel, TVE_130, 0x000e000c);
		TVE_WUINT32(sel, TVE_134, 0x00000000);
		TVE_WUINT32(sel, TVE_138, 0x00000000);
		TVE_WUINT32(sel, TVE_13C, 0x07000000);
		TVE_WUINT32(sel, TVE_0F8, 0x00000200);
		TVE_WUINT32(sel, TVE_120, 0x01e80320);
		TVE_WUINT32(sel, TVE_380, 0x00000000);
		TVE_INIT_BIT(sel, TVE_004, 0x1<<16, 0<<16);
		TVE_WUINT32(sel, TVE_000, 0x00000000);
		break;

	case DISP_VGA_MOD_640_480P_60:
	case DISP_VGA_MOD_800_600P_60:
	case DISP_VGA_MOD_1024_768P_60:
	case DISP_VGA_MOD_1280_768P_60:
	case DISP_VGA_MOD_1280_800P_60:
	case DISP_VGA_MOD_1366_768P_60:
	case DISP_VGA_MOD_1440_900P_60:
	case DISP_VGA_MOD_1600_900P_60:
	case DISP_VGA_MOD_1280_720P_60:
	case DISP_VGA_MOD_1920_1080P_60:
	case DISP_VGA_MOD_1920_1200P_60:
		TVE_WUINT32(sel, TVE_004, 0x08000000);
		break;

	default:
		break;
	}

	return 0;
}

/* 0:unconnected; 1:connected; */
static int32_t tve_low_get_dac_status(uint32_t sel)
{
	uint32_t readval = 0;
	uint32_t result = 0;
	uint32_t i = 0;

//	for (i = 0; i < TVE_DAC_NUM; i++) {
//		if (dac_info[sel][i] == 1) {
			readval = (TVE_RUINT32(sel, TVE_038)>>(8*i))&3;

//			if (readval == 1) {
//				result = 1;
//				break;
//			}
//		}
//	}

	return readval; //result;
}

static int32_t tve_low_dac_autocheck_enable(uint32_t sel)
{
	uint8_t i = 0;

	TVE_SET_BIT(sel, TVE_000, 0x80000000);	/* tvclk enable */
	TVE_WUINT32(sel, TVE_0F8, 0x00000200);

#if defined(CONFIG_MACH_SUN50IW9)
	TVE_WUINT32(sel, TVE_0FC, 0x014700FF);	/* 10ms x 10 */
	TVE_SET_BIT(sel, TVE_030, 0x80000000);  /* new detect mode */
#else
	TVE_WUINT32(sel, TVE_0FC, 0x0A3C00FF);	/* 20ms x 10 */
#endif

	TVE_WUINT32(sel, TVE_03C, 0x00009999);
//	for (i = 0; i < TVE_DAC_NUM; i++) {
//		if (dac_info[sel][i] == 1)
			TVE_SET_BIT(sel, TVE_030, 1<<i);/* detect enable */
//	}

	return 0;
}

static int32_t tve_low_dac_autocheck_disable(uint32_t sel)
{
	uint8_t i = 0;

//	for (i = 0; i < TVE_DAC_NUM; i++) {
//		if (dac_info[sel][i] == 1)
			TVE_CLR_BIT(sel, TVE_030, 1<<i);
//	}
	return 0;
}

#if 0
#if defined (CONFIG_MACH_SUN50IW9)
static uint32_t tve_low_get_sid(uint32_t index)
{
	uint32_t efuse = 0;

	efuse = (readl(0x0300622c) >> 16) + (readl(0x03006230) << 16);

	if (efuse > 5)
		efuse -= 5;

	return efuse;
}
#else
static uint32_t tve_low_get_sid(uint32_t index)
{
	int32_t ret = 0;
	int32_t buf_len = 32 * TVE_DAC_NUM;
	uint32_t efuse[TVE_DAC_NUM] = {0};

	if (index > TVE_DAC_NUM - 1)
		return 0;

	ret = sunxi_efuse_read(EFUSE_TV_OUT_NAME, &efuse, &buf_len);
	if (ret < 0)
		return 0;

	return efuse[index];
}
#endif
#endif

static int32_t tve_low_enhance(uint32_t sel, uint32_t mode)
{
	if (mode == 0) {
		TVE_CLR_BIT(sel, TVE_000, 0xf<<10); /* deflick off */
		TVE_SET_BIT(sel, TVE_000, 0x5<<10); /* deflick is 5 */
		TVE_SET_BIT(sel, TVE_00C, 0x1<<31); /* lti on */
		TVE_SET_BIT(sel, TVE_00C, 0x1<<16); /* notch off */
	} else if (mode == 1) {
		TVE_CLR_BIT(sel, TVE_000, 0xf<<10);
		TVE_SET_BIT(sel, TVE_000, 0x5<<10);
		TVE_SET_BIT(sel, TVE_00C, 0x1<<31);
		TVE_CLR_BIT(sel, TVE_00C, 0x1<<16);
	} else if (mode == 2) {
		TVE_CLR_BIT(sel, TVE_000, 0xf<<10);
		TVE_CLR_BIT(sel, TVE_00C, 0x1<<31);
		TVE_SET_BIT(sel, TVE_00C, 0x1<<16);
	}
	return 0;
}

#endif
//
//__PACKED_STRUCT de_vi_t {
//	struct {
//		uint32_t attr;
//		uint32_t size;
//		uint32_t coord;
//		uint32_t pitch[3];
//		uint32_t top_laddr[3];
//		uint32_t bot_laddr[3];
//	} cfg[4];
//	uint32_t fcolor[4];
//	uint32_t top_haddr[3];
//	uint32_t bot_haddr[3];
//	uint32_t ovl_size[2];
//	uint32_t hori[2];
//	uint32_t vert[2];
//};
//
//__PACKED_STRUCT de_ui_t {
//	struct {
//		uint32_t attr;
//		uint32_t size;
//		uint32_t coord;
//		uint32_t pitch;
//		uint32_t top_laddr;
//		uint32_t bot_laddr;
//		uint32_t fcolor;
//		uint32_t dum;
//	} cfg[4];
//	uint32_t top_haddr;
//	uint32_t bot_haddr;
//	uint32_t ovl_size;
//};

/* ********************************* */
#if defined (TVENCODER_PTR)

static void t113_tve_DAC_configuration(const videomode_t * vdmode)
{
	const unsigned sel = 0;
	const unsigned mode = vdmode->ntsc ? DISP_TV_MOD_NTSC : DISP_TV_MOD_PAL;

	tve_low_init(sel);
	tve_low_dac_autocheck_disable(sel);
	// tve_low_dac_autocheck_enable(sel);
	tve_low_set_tv_mode(sel, mode, 0);
	tve_low_dac_enable(sel);
	tve_low_open(sel);
	tve_low_enhance(sel, 0); //0,1,2

	// if(tve_low_get_dac_status(0))PRINTF("DAC connected!\n");
	// else                         PRINTF("DAC NOT connected!\n");
}


// 216 МГц тактирования на TVE
static void t113_tve_CCU_configuration(const videomode_t * vdmode)
{
	const uint_fast32_t needfreq = 216000000;
#if CPUSTYLE_A64
#elif CPUSTYLE_T507 || CPUSTYLE_H616

	//	CLK_SRC_SEL
	//	Clock Source Select
	//	Clock Source Select
	//	000: PLL_VIDEO0(1X)
	//	001: PLL_VIDEO0(4X)
	//	010: PLL_VIDEO1(1X)
	//	011: PLL_VIDEO1(4X)


	unsigned divider;
	unsigned prei = calcdivider(calcdivround2(allwnr_t507_get_pll_video0_x4_freq(), needfreq), 4, (8 | 4 | 2 | 1), & divider, 1);
	//PRINTF("t113_tve_CCU_configuration: needfreq=%u MHz, prei=%u, divider=%u\n", (unsigned) (needfreq / 1000 / 1000), (unsigned) prei, (unsigned) divider);
	ASSERT(divider < 16);
	TVE_CCU_CLK_REG = (TVE_CCU_CLK_REG & ~ (UINT32_C(0x07) << 24) & ~ (UINT32_C(0x03) << 8) & ~ (UINT32_C(0x0F) << 0)) |
		0x01 * (UINT32_C(1) << 24) |	// CLK_SRC_SEL 001: PLL_VIDEO0(4X)
		prei * (UINT32_C(1) << 8) |	// FACTOR_N 0..3: 1..8
		divider * (UINT32_C(1) << 0) |	// FACTOR_M (0x00..0x0F: 1..16)
		0;
	TVE_CCU_CLK_REG |= (UINT32_C(1) << 31);

	TVE_CCU_BGR_REG |= (UINT32_C(1) << 1) | (UINT32_C(1) << 0);                     //gate pass for TVE0_GATING & TVE_TOP
	TVE_CCU_BGR_REG &= ~ (UINT32_C(1) << 17) & ~ (UINT32_C(1) << 16); 	//                 //assert reset for TVE0_RST & TVE_TOP_RST
	TVE_CCU_BGR_REG |= (UINT32_C(1) << 17) | (UINT32_C(1) << 16);                   // de-assert reset for TVE0_RST & TVE_TOP_RST

	//PRINTF("t113_tve_CCU_configuration: BOARD_TVEFREQ=%u MHz\n", (unsigned) (BOARD_TVEFREQ / 1000 / 1000));
	local_delay_us(10);
#elif CPUSTYLE_T113 || CPUSTYLE_F133

	//	CLK_SRC_SEL
	//	Clock Source Select
	//	000: PLL_VIDEO0(1X)
	//	001: PLL_VIDEO0(4X)
	//	010: PLL_VIDEO1(1X)
	//	011: PLL_VIDEO1(4X)
	//	100: PLL_PERI(2X)
	//	101: PLL_AUDIO1(DIV2)

	unsigned divider;
	unsigned prei = calcdivider(calcdivround2(allwnr_t113_get_video0_x4_freq(), needfreq), 4, (8 | 4 | 2 | 1), & divider, 1);
	//PRINTF("t113_tve_CCU_configuration: needfreq=%u MHz, prei=%u, divider=%u\n", (unsigned) (needfreq / 1000 / 1000), (unsigned) prei, (unsigned) divider);
	ASSERT(divider < 16);
	TVE_CCU_CLK_REG = (TVE_CCU_CLK_REG & ~ (UINT32_C(0x07) << 24) & ~ (UINT32_C(0x03) << 8) & ~ (UINT32_C(0x0F) << 0)) |
		0x01 * (UINT32_C(1) << 24) |	// CLK_SRC_SEL 001: PLL_VIDEO0(4X)
		prei * (UINT32_C(1) << 8) |	// FACTOR_N 0..3: 1..8
		divider * (UINT32_C(1) << 0) |	// FACTOR_M (0x00..0x0F: 1..16)
		0;
	TVE_CCU_CLK_REG |= (UINT32_C(1) << 31);

	TVE_CCU_BGR_REG |= (UINT32_C(1) << 1) | (UINT32_C(1) << 0);                     //gate pass for TVE & TVE_TOP
	TVE_CCU_BGR_REG &= ~ (UINT32_C(1) << 17) & ~ (UINT32_C(1) << 16);                 //assert reset for TVE & TVE_TOP
	TVE_CCU_BGR_REG |= (UINT32_C(1) << 17) | (UINT32_C(1) << 16);                   // de-assert reset for TVE & TVE_TOP

	//PRINTF("t113_tve_CCU_configuration: BOARD_TVEFREQ=%u MHz\n", (unsigned) (BOARD_TVEFREQ / 1000 / 1000));
	local_delay_us(10);
#else
#endif

}

static void t113_tvoutDAC_init(const videomode_t * vdmode)
{
	t113_tve_CCU_configuration(vdmode);
	t113_tve_DAC_configuration(vdmode);
}

static void t113_tvout_initsteps(const videomode_t * vdmode)
{
	t113_tcontv_initsteps(vdmode);
	t113_tvoutDAC_init(vdmode);
}

#endif /* defined (TVENCODER_PTR) */

/* ----- */

#endif /* defined (TCONTV_PTR) */

#if 0
struct de_csc_t {
	uint32_t csc_ctl; //0x00
	uint8_t res[0xc];

	uint32_t coef11;  //0x10
	uint32_t coef12;  //0x14
	uint32_t coef13;  //0x18
	uint32_t coef14;  //0x1C

	uint32_t coef21; //0x20
	uint32_t coef22; //0x24
	uint32_t coef23; //0x28
	uint32_t coef24; //0x2C

	uint32_t coef31; //0x30
	uint32_t coef32; //0x34
	uint32_t coef33; //0x38
	uint32_t coef34; //0x3C

        uint32_t globalpha; //0x40
};
#endif


//#define T113_DE_BASE		DE_BASE //(0x05000000)

#define T113_DE_MUX_GLB		(0x00100000 + 0x00000)
#define T113_DE_MUX_BLD		(0x00100000 + 0x01000)
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


static uint32_t read32(uintptr_t addr)
{
	return * (volatile uint32_t *) addr;
}

static void write32(uintptr_t addr, uint32_t value)
{
	* (volatile uint32_t *) addr = value;
}

#define IS_DE3 0

#define HSUB 2
#define VSUB 2

//#define DE2_VI_SCALER_UNIT_BASE 0x20000
//#define DE2_VI_SCALER_UNIT_SIZE 0x20000
//
//#define DE3_VI_SCALER_UNIT_BASE 0x20000
//#define DE3_VI_SCALER_UNIT_SIZE 0x08000


#define SUN8I_VI_SCALER_COEFF_COUNT		32

#define SUN8I_VI_SCALER_PHASE_FRAC		20
#define SUN8I_VI_SCALER_SCALE_FRAC		20

#define SUN8I_VI_SCALER_SIZE(w, h)		(((h) - 1) << 16 | ((w) - 1))

#define SUN50I_SCALER_VSU_SCALE_MODE_UI		0
#define SUN50I_SCALER_VSU_SCALE_MODE_NORMAL	1

#define SUN8I_SCALER_VSU_CTRL(base)		((base) + 0x0)
#define SUN50I_SCALER_VSU_SCALE_MODE(base)	((base) + 0x10)

#define SUN8I_SCALER_VSU_OUTSIZE(base)		((base) + 0x40)
#define SUN8I_SCALER_VSU_YINSIZE(base)		((base) + 0x80)
#define SUN8I_SCALER_VSU_YHSTEP(base)		((base) + 0x88)
#define SUN8I_SCALER_VSU_YVSTEP(base)		((base) + 0x8c)
#define SUN8I_SCALER_VSU_YHPHASE(base)		((base) + 0x90)
#define SUN8I_SCALER_VSU_YVPHASE(base)		((base) + 0x98)
#define SUN8I_SCALER_VSU_CINSIZE(base)		((base) + 0xc0)
#define SUN8I_SCALER_VSU_CHSTEP(base)		((base) + 0xc8)
#define SUN8I_SCALER_VSU_CVSTEP(base)		((base) + 0xcc)
#define SUN8I_SCALER_VSU_CHPHASE(base)		((base) + 0xd0)
#define SUN8I_SCALER_VSU_CVPHASE(base)		((base) + 0xd8)

#define SUN8I_SCALER_VSU_YHCOEFF0(base, i)	((base) + 0x200 + 0x4 * (i))
#define SUN8I_SCALER_VSU_YHCOEFF1(base, i)	((base) + 0x300 + 0x4 * (i))
#define SUN8I_SCALER_VSU_YVCOEFF(base, i)	((base) + 0x400 + 0x4 * (i))
#define SUN8I_SCALER_VSU_CHCOEFF0(base, i)	((base) + 0x600 + 0x4 * (i))
#define SUN8I_SCALER_VSU_CHCOEFF1(base, i)	((base) + 0x700 + 0x4 * (i))
#define SUN8I_SCALER_VSU_CVCOEFF(base, i)	((base) + 0x800 + 0x4 * (i))

#define SUN8I_SCALER_VSU_CTRL_EN		LTDCBIT32(0)
#define SUN8I_SCALER_VSU_CTRL_COEFF_RDY		LTDCBIT32(4)
// T507
//	de_base(1)=0x00FE4000
//	de_base(2)=0x01004000
//	de_vsu_base(1)=0x01104000	DE_VSU1_BASE
//	de_vsu_base(2)=0x01124000 	DE_VSU2_BASE
//	PRINTF("de_vsu_base(1)=0x%08X\n", T113_DE_MUX_VSU+T113_DE_BASE_N(1));
//	PRINTF("de_vsu_base(2)=0x%08X\n", T113_DE_MUX_VSU+T113_DE_BASE_N(2));

#if CPUSTYLE_T507 || CPUSTYLE_H616
	#define T113_VSU_BASE_N(id) ((id) == 2 ? DE_VSU2_BASE: DE_VSU1_BASE)
#else
	#define T113_VSU_BASE_N(id) ((id) == 2 ? DE_MIXER1_VSU1_BASE: DE_MIXER0_VSU1_BASE)
#endif

#define regmap_vsu_write(id,y,z) do { (*(volatile uint32_t*)(T113_VSU_BASE_N((id))+(y)))=(z); } while (0)
#define regmap_vsu_read(id,y) (*(volatile uint32_t*) (T113_VSU_BASE_N((id))+(y)))

static const uint32_t bicubic8coefftab32_left[480] = {
	0x40000000, 0x40ff0000, 0x3ffe0000, 0x3efe0000,
	0x3dfd0000, 0x3cfc0000, 0x3bfc0000, 0x39fc0000,
	0x36fc0000, 0x35fb0000, 0x33fb0000, 0x31fb0000,
	0x2ffb0000, 0x2cfb0000, 0x29fc0000, 0x27fc0000,
	0x24fc0000, 0x21fc0000, 0x1efd0000, 0x1cfd0000,
	0x19fd0000, 0x16fe0000, 0x14fe0000, 0x11fe0000,
	0x0dff0000, 0x0cff0000, 0x0aff0000, 0x08ff0000,
	0x05000000, 0x03000000, 0x02000000, 0x01000000,

	0x3904ff00, 0x3903ff00, 0x3902ff00, 0x38010000,
	0x37000000, 0x36ff0000, 0x35ff0000, 0x34fe0000,
	0x32fe0000, 0x31fd0000, 0x30fd0000, 0x2efc0000,
	0x2cfc0000, 0x2afc0000, 0x28fc0000, 0x26fc0000,
	0x24fc0000, 0x22fc0000, 0x20fc0000, 0x1efc0000,
	0x1cfc0000, 0x19fc0000, 0x17fc0000, 0x15fd0000,
	0x12fd0000, 0x11fd0000, 0x0ffd0000, 0x0dfe0000,
	0x0bfe0000, 0x09fe0000, 0x08fe0000, 0x06ff0000,

	0x3209fe00, 0x3407fe00, 0x3306fe00, 0x3305fe00,
	0x3204fe00, 0x3102ff00, 0x3102ff00, 0x3001ff00,
	0x2f00ff00, 0x2effff00, 0x2cff0000, 0x2bfe0000,
	0x29fe0000, 0x28fe0000, 0x26fd0000, 0x24fd0000,
	0x23fd0000, 0x21fd0000, 0x20fc0000, 0x1efc0000,
	0x1dfc0000, 0x1bfc0000, 0x19fc0000, 0x17fc0000,
	0x16fc0000, 0x14fc0000, 0x12fc0000, 0x10fd0000,
	0x0ffd0000, 0x0dfd0000, 0x0cfd0000, 0x0afd0000,

	0x2e0cfd00, 0x2e0bfd00, 0x2e09fd00, 0x2e08fd00,
	0x2e07fd00, 0x2c06fe00, 0x2c05fe00, 0x2b04fe00,
	0x2b03fe00, 0x2a02fe00, 0x2901fe00, 0x2701ff00,
	0x2700ff00, 0x26ffff00, 0x24ffff00, 0x23ffff00,
	0x22feff00, 0x20fe0000, 0x1ffe0000, 0x1efd0000,
	0x1dfd0000, 0x1bfd0000, 0x1afd0000, 0x19fd0000,
	0x17fd0000, 0x15fd0000, 0x13fd0000, 0x12fd0000,
	0x11fd0000, 0x10fd0000, 0x0ffd0000, 0x0cfd0000,

	0x2a0efd00, 0x2a0dfd00, 0x2a0cfd00, 0x290bfd00,
	0x290afd00, 0x2909fd00, 0x2908fd00, 0x2807fd00,
	0x2706fd00, 0x2705fd00, 0x2604fe00, 0x2603fe00,
	0x2502fe00, 0x2402fe00, 0x2401fe00, 0x2200fe00,
	0x2200fe00, 0x2000ff00, 0x1fffff00, 0x1effff00,
	0x1dfeff00, 0x1cfeff00, 0x1afeff00, 0x19feff00,
	0x17fe0000, 0x16fd0000, 0x15fd0000, 0x14fd0000,
	0x12fd0000, 0x11fd0000, 0x10fd0000, 0x0ffd0000,

	0x2610fd00, 0x260ffd00, 0x260efd00, 0x260dfd00,
	0x260cfd00, 0x260bfd00, 0x260afd00, 0x2609fd00,
	0x2508fd00, 0x2507fd00, 0x2406fd00, 0x2406fd00,
	0x2305fd00, 0x2304fd00, 0x2203fe00, 0x2103fe00,
	0x2002fe00, 0x1f01fe00, 0x1e01fe00, 0x1e00fe00,
	0x1c00fe00, 0x1b00fe00, 0x1afffe00, 0x19ffff00,
	0x18ffff00, 0x17feff00, 0x16feff00, 0x15feff00,
	0x14feff00, 0x13feff00, 0x11feff00, 0x10fd0000,

	0x2411feff, 0x2410feff, 0x240ffeff, 0x230efeff,
	0x240dfeff, 0x240cfeff, 0x230cfd00, 0x230bfd00,
	0x230afd00, 0x2309fd00, 0x2208fd00, 0x2108fd00,
	0x2007fd00, 0x2106fd00, 0x2005fd00, 0x1f05fd00,
	0x1f04fd00, 0x1e03fd00, 0x1d03fe00, 0x1c02fe00,
	0x1b02fe00, 0x1a01fe00, 0x1a01fe00, 0x1900fe00,
	0x1800fe00, 0x1700fe00, 0x16fffe00, 0x15fffe00,
	0x13ffff00, 0x12ffff00, 0x12feff00, 0x11feff00,

	0x2212fffe, 0x2211fffe, 0x2210ffff, 0x220ffeff,
	0x220efeff, 0x210efeff, 0x210dfeff, 0x210cfeff,
	0x210bfeff, 0x200bfeff, 0x200afeff, 0x1f09feff,
	0x1f08feff, 0x1d08fe00, 0x1e07fd00, 0x1e06fd00,
	0x1d06fd00, 0x1c05fd00, 0x1b04fe00, 0x1a04fe00,
	0x1a03fe00, 0x1903fe00, 0x1802fe00, 0x1802fe00,
	0x1701fe00, 0x1601fe00, 0x1501fe00, 0x1500fe00,
	0x1400fe00, 0x1400fe00, 0x13fffe00, 0x12fffe00,

	0x201200fe, 0x201100fe, 0x1f1100fe, 0x2010fffe,
	0x200ffffe, 0x1f0ffffe, 0x1f0efffe, 0x1e0dffff,
	0x1f0cfeff, 0x1e0cfeff, 0x1e0bfeff, 0x1e0afeff,
	0x1d0afeff, 0x1d09feff, 0x1c08feff, 0x1b08feff,
	0x1b07feff, 0x1a07feff, 0x1a06feff, 0x1a05feff,
	0x1805fe00, 0x1904fe00, 0x1704fe00, 0x1703fe00,
	0x1603fe00, 0x1602fe00, 0x1402fe00, 0x1402fe00,
	0x1401fe00, 0x1301fe00, 0x1201fe00, 0x1200fe00,

	0x1c1202fe, 0x1c1102fe, 0x1b1102fe, 0x1c1001fe,
	0x1b1001fe, 0x1c0f01fe, 0x1b0f00fe, 0x1b0e00fe,
	0x1b0e00fe, 0x1b0d00fe, 0x1b0c00fe, 0x1a0cfffe,
	0x1a0bfffe, 0x1a0bfffe, 0x190afffe, 0x190afffe,
	0x1909fffe, 0x1709ffff, 0x1808ffff, 0x1708feff,
	0x1707feff, 0x1707feff, 0x1606feff, 0x1506feff,
	0x1505feff, 0x1505feff, 0x1404feff, 0x1404feff,
	0x1404feff, 0x1303feff, 0x1203feff, 0x1202feff,

	0x191104fe, 0x191104fe, 0x191003fe, 0x191003fe,
	0x171003fe, 0x180f03fe, 0x180f02fe, 0x180e02fe,
	0x180e02fe, 0x180d01fe, 0x180d01fe, 0x180d01fe,
	0x170c01fe, 0x160c01fe, 0x170b00fe, 0x170b00fe,
	0x160a00fe, 0x160a00fe, 0x160a00fe, 0x150900fe,
	0x1509fffe, 0x1508fffe, 0x1508fffe, 0x1408fffe,
	0x1407fffe, 0x1307ffff, 0x1306ffff, 0x1206ffff,
	0x1206ffff, 0x1205ffff, 0x1205ffff, 0x1104feff,

	0x161006ff, 0x161005ff, 0x161005ff, 0x160f05ff,
	0x160f04ff, 0x150f04ff, 0x150e04ff, 0x150e04ff,
	0x150e03ff, 0x150d03ff, 0x150d03ff, 0x150d02ff,
	0x140c02ff, 0x150c02fe, 0x150c02fe, 0x150b02fe,
	0x140b01fe, 0x140b01fe, 0x140a01fe, 0x140a01fe,
	0x140a01fe, 0x130900fe, 0x130900fe, 0x130900fe,
	0x130800fe, 0x120800fe, 0x120800fe, 0x120700fe,
	0x120700fe, 0x1107fffe, 0x1106fffe, 0x1106fffe,

	0x140f0700, 0x140f0600, 0x140f0600, 0x140f0600,
	0x140e0600, 0x130e0500, 0x140e05ff, 0x130e05ff,
	0x140d05ff, 0x130d04ff, 0x130d04ff, 0x120d04ff,
	0x130c04ff, 0x130c03ff, 0x130c03ff, 0x120c03ff,
	0x120b03ff, 0x120b02ff, 0x120b02ff, 0x120a02ff,
	0x120a02ff, 0x110a02ff, 0x110a01ff, 0x120901ff,
	0x100901ff, 0x100901ff, 0x110801ff, 0x110801ff,
	0x100800ff, 0x100800ff, 0x100700ff, 0x100700fe,

	0x120f0701, 0x120e0701, 0x120e0701, 0x120e0701,
	0x120e0600, 0x110e0600, 0x120d0600, 0x120d0600,
	0x120d0500, 0x120d0500, 0x110d0500, 0x110c0500,
	0x110c0500, 0x110c0400, 0x110c0400, 0x110b04ff,
	0x110b04ff, 0x110b04ff, 0x110b03ff, 0x110b03ff,
	0x110a03ff, 0x110a03ff, 0x100a03ff, 0x110a02ff,
	0x100902ff, 0x100902ff, 0x100902ff, 0x0f0902ff,
	0x0e0902ff, 0x100801ff, 0x0f0801ff, 0x0f0801ff,

	0x100e0802, 0x100e0802, 0x110e0702, 0x110d0701,
	0x110d0701, 0x100d0701, 0x100d0701, 0x110d0601,
	0x110d0601, 0x110c0601, 0x110c0601, 0x100c0600,
	0x100c0500, 0x100c0500, 0x100c0500, 0x100b0500,
	0x100b0500, 0x100b0400, 0x100b0400, 0x0f0b0400,
	0x100a0400, 0x0f0a0400, 0x0f0a0400, 0x0f0a0300,
	0x0f0a03ff, 0x0f0903ff, 0x0f0903ff, 0x0f0903ff,
	0x0f0903ff, 0x0f0902ff, 0x0f0902ff, 0x0f0802ff
};

static const uint32_t bicubic8coefftab32_right[480] = {
	0x00000000, 0x00000001, 0x00000003, 0x00000004,
	0x00000006, 0x0000ff09, 0x0000ff0a, 0x0000ff0c,
	0x0000ff0f, 0x0000fe12, 0x0000fe14, 0x0000fe16,
	0x0000fd19, 0x0000fd1c, 0x0000fd1e, 0x0000fc21,
	0x0000fc24, 0x0000fc27, 0x0000fc29, 0x0000fb2c,
	0x0000fb2f, 0x0000fb31, 0x0000fb33, 0x0000fb36,
	0x0000fc38, 0x0000fc39, 0x0000fc3b, 0x0000fc3d,
	0x0000fd3e, 0x0000fe3f, 0x0000fe40, 0x0000ff40,

	0x0000ff05, 0x0000ff06, 0x0000fe08, 0x0000fe09,
	0x0000fe0b, 0x0000fe0d, 0x0000fd0f, 0x0000fd11,
	0x0000fd13, 0x0000fd15, 0x0000fc17, 0x0000fc1a,
	0x0000fc1c, 0x0000fc1e, 0x0000fc20, 0x0000fc22,
	0x0000fc24, 0x0000fc26, 0x0000fc28, 0x0000fc2a,
	0x0000fc2c, 0x0000fc2f, 0x0000fd30, 0x0000fd31,
	0x0000fe33, 0x0000fe34, 0x0000ff35, 0x0000ff36,
	0x00000037, 0x00000138, 0x00ff0239, 0x00ff0339,

	0x0000fe09, 0x0000fd0a, 0x0000fd0c, 0x0000fd0d,
	0x0000fd0f, 0x0000fd11, 0x0000fc12, 0x0000fc14,
	0x0000fc16, 0x0000fc18, 0x0000fc19, 0x0000fc1b,
	0x0000fc1d, 0x0000fc1e, 0x0000fc21, 0x0000fd22,
	0x0000fd23, 0x0000fd25, 0x0000fd27, 0x0000fe28,
	0x0000fe29, 0x0000fe2b, 0x0000ff2c, 0x00ffff2f,
	0x00ff002f, 0x00ff0130, 0x00ff0231, 0x00ff0232,
	0x00fe0432, 0x00fe0533, 0x00fe0633, 0x00fe0734,

	0x0000fd0c, 0x0000fd0d, 0x0000fd0f, 0x0000fd10,
	0x0000fd11, 0x0000fd13, 0x0000fd14, 0x0000fd16,
	0x0000fd17, 0x0000fd19, 0x0000fd1b, 0x0000fd1c,
	0x0000fd1d, 0x0000fd1f, 0x0000fe20, 0x0000fe21,
	0x00fffe24, 0x00ffff24, 0x00ffff25, 0x00ffff27,
	0x00ff0027, 0x00ff0128, 0x00fe012a, 0x00fe022a,
	0x00fe032b, 0x00fe042c, 0x00fe052d, 0x00fe062d,
	0x00fd072e, 0x00fd082e, 0x00fd092e, 0x00fd0b2f,

	0x0000fd0e, 0x0000fd0f, 0x0000fd10, 0x0000fd12,
	0x0000fd13, 0x0000fd14, 0x0000fd15, 0x0000fd17,
	0x0000fe18, 0x00fffe1a, 0x00fffe1b, 0x00fffe1c,
	0x00fffe1e, 0x00ffff1e, 0x00ffff1f, 0x00ff0021,
	0x00fe0022, 0x00fe0023, 0x00fe0124, 0x00fe0224,
	0x00fe0226, 0x00fe0326, 0x00fe0427, 0x00fd0528,
	0x00fd0628, 0x00fd0729, 0x00fd0829, 0x00fd0929,
	0x00fd0a2a, 0x00fd0b2a, 0x00fd0c2a, 0x00fd0d2a,

	0x0000fd10, 0x0000fd11, 0x00fffe12, 0x00fffe13,
	0x00fffe14, 0x00fffe15, 0x00fffe16, 0x00fffe17,
	0x00ffff18, 0x00ffff19, 0x00feff1c, 0x00fe001b,
	0x00fe001d, 0x00fe001e, 0x00fe011e, 0x00fe011f,
	0x00fe0220, 0x00fe0321, 0x00fe0322, 0x00fd0423,
	0x00fd0524, 0x00fd0624, 0x00fd0626, 0x00fd0725,
	0x00fd0825, 0x00fd0926, 0x00fd0a26, 0x00fd0b26,
	0x00fd0c26, 0x00fd0d26, 0x00fd0e27, 0x00fd0f27,

	0x00fffe11, 0x00fffe12, 0x00fffe13, 0x00ffff14,
	0x00ffff14, 0x00feff16, 0x00feff17, 0x00fe0017,
	0x00fe0018, 0x00fe0019, 0x00fe011a, 0x00fe011b,
	0x00fe021c, 0x00fe021c, 0x00fe031d, 0x00fd031f,
	0x00fd041f, 0x00fd0520, 0x00fd0520, 0x00fd0621,
	0x00fd0721, 0x00fd0822, 0x00fd0822, 0x00fd0923,
	0x00fd0a23, 0x00fd0b23, 0x00fd0b25, 0x00fe0c24,
	0x00fe0d24, 0x00fe0e24, 0x00fe0f24, 0x00fe1024,

	0x00feff12, 0x00feff13, 0x00feff13, 0x00fe0014,
	0x00fe0015, 0x00fe0016, 0x00fe0116, 0x00fe0117,
	0x00fe0118, 0x00fe0218, 0x00fe0219, 0x00fe031a,
	0x00fe031b, 0x00fe041b, 0x00fd041d, 0x00fd051d,
	0x00fd061d, 0x00fd061f, 0x00fe071e, 0x00fe081e,
	0x00fe081f, 0x00fe091f, 0x00fe0a20, 0x00fe0a20,
	0x00fe0b21, 0x00fe0c21, 0x00fe0d21, 0x00fe0d22,
	0x00fe0e22, 0x00fe0f21, 0x00ff1021, 0x00ff1022,

	0x00fe0012, 0x00fe0013, 0x00fe0113, 0x00fe0114,
	0x00fe0115, 0x00fe0215, 0x00fe0216, 0x00fe0217,
	0x00fe0317, 0x00fe0318, 0x00fe0418, 0x00fe0419,
	0x00fe0519, 0x00fe051a, 0x00fe061b, 0x00fe071b,
	0x00fe071c, 0x00fe081c, 0x00fe081d, 0x00fe091d,
	0x00fe0a1d, 0x00fe0a1d, 0x00fe0b1e, 0x00fe0c1e,
	0x00ff0c1e, 0x00ff0d1e, 0x00ff0e1f, 0x00ff0e1f,
	0x00ff0f1f, 0x00ff0f20, 0x0000101f, 0x0000111f,

	0x00fe0212, 0x00fe0312, 0x00fe0313, 0x00fe0314,
	0x00fe0414, 0x00fe0414, 0x00fe0515, 0x00fe0516,
	0x00fe0516, 0x00fe0616, 0x00fe0617, 0x00fe0718,
	0x00fe0719, 0x00fe0818, 0x00ff0819, 0x00ff0918,
	0x00ff0919, 0x00ff0a19, 0x00ff0a19, 0x00ff0b1a,
	0x00ff0b1b, 0x00ff0c1a, 0x00000c1b, 0x00000d1b,
	0x00000d1c, 0x00000e1b, 0x00000e1d, 0x00010f1b,
	0x00010f1b, 0x0001101c, 0x0001101d, 0x0002111c,

	0x00fe0412, 0x00fe0412, 0x00ff0512, 0x00ff0512,
	0x00ff0613, 0x00ff0613, 0x00ff0614, 0x00ff0714,
	0x00ff0714, 0x00ff0815, 0x00ff0815, 0x00ff0815,
	0x00ff0916, 0x00000916, 0x00000a16, 0x00000a16,
	0x00000a18, 0x00000b17, 0x00000b17, 0x00010c17,
	0x00010c18, 0x00010d18, 0x00010d18, 0x00010d19,
	0x00020e18, 0x00020e18, 0x00020f18, 0x00030f18,
	0x00030f18, 0x00031018, 0x00031018, 0x00041119,

	0x00ff0610, 0x00ff0611, 0x00ff0611, 0x00ff0711,
	0x00000711, 0x00000712, 0x00000812, 0x00000812,
	0x00000813, 0x00000913, 0x00000913, 0x00000914,
	0x00010a14, 0x00010a14, 0x00010a14, 0x00010b14,
	0x00010b16, 0x00020b15, 0x00020c15, 0x00020c15,
	0x00020c15, 0x00020d17, 0x00030d16, 0x00030d16,
	0x00030e16, 0x00040e16, 0x00040e16, 0x00040f16,
	0x00040f16, 0x00050f17, 0x00051017, 0x00051017,

	0x0000070f, 0x00000710, 0x00000710, 0x00000710,
	0x00000810, 0x00010811, 0x00010811, 0x00010911,
	0x00010911, 0x00010913, 0x00010913, 0x00020a12,
	0x00020a12, 0x00020a13, 0x00020b12, 0x00020b13,
	0x00030b13, 0x00030c13, 0x00030c13, 0x00030c14,
	0x00040c13, 0x00040d13, 0x00040d14, 0x00040d14,
	0x00050e14, 0x00050e14, 0x00050e14, 0x00050e14,
	0x00060f14, 0x00060f14, 0x00060f15, 0x00061015,

	0x0001070f, 0x0001080f, 0x0001080f, 0x0001080f,
	0x00010811, 0x00020910, 0x00020910, 0x00020910,
	0x00020911, 0x00020a10, 0x00030a10, 0x00030a11,
	0x00030a11, 0x00030b11, 0x00030b11, 0x00040b12,
	0x00040b12, 0x00040c11, 0x00040c12, 0x00040c12,
	0x00050c12, 0x00050c12, 0x00050d12, 0x00050d12,
	0x00060d13, 0x00060d13, 0x00060e12, 0x00060e13,
	0x00070e13, 0x00070e13, 0x00070f13, 0x00070f13,

	0x0002080e, 0x0002080e, 0x0002080e, 0x00020810,
	0x0002090f, 0x0003090f, 0x0003090f, 0x0003090f,
	0x0003090f, 0x00030a0f, 0x00030a0f, 0x00040a10,
	0x00040a11, 0x00040b10, 0x00040b10, 0x00040b11,
	0x00050b10, 0x00050b11, 0x00050c10, 0x00050c11,
	0x00050c11, 0x00060c11, 0x00060c11, 0x00060d11,
	0x00060d12, 0x00070d12, 0x00070d12, 0x00070e11,
	0x00070e11, 0x00070e12, 0x00080e11, 0x00080e12
};

static const uint32_t bicubic4coefftab32[480] = {
	0x00004000, 0x000140ff, 0x00033ffe, 0x00043ffd,
	0x00063dfd, 0xff083dfc, 0xff0a3bfc, 0xff0c39fc,
	0xff0e37fc, 0xfe1136fb, 0xfe1433fb, 0xfe1631fb,
	0xfd192ffb, 0xfd1c2cfb, 0xfd1e29fc, 0xfc2127fc,
	0xfc2424fc, 0xfc2721fc, 0xfc291efd, 0xfb2c1cfd,
	0xfb2f19fd, 0xfb3116fe, 0xfb3314fe, 0xfb3611fe,
	0xfc370eff, 0xfc390cff, 0xfc3b0aff, 0xfc3d08ff,
	0xfd3d0600, 0xfd3f0400, 0xfe3f0300, 0xff400100,

	0xfe053904, 0xfe063903, 0xfe083901, 0xfe0a3800,
	0xfd0b3800, 0xfe0d36ff, 0xfd0f35ff, 0xfd1134fe,
	0xfd1332fe, 0xfd1531fd, 0xfc1730fd, 0xfc1a2efc,
	0xfc1c2cfc, 0xfc1e2afc, 0xfc2028fc, 0xfc2226fc,
	0xfc2424fc, 0xfc2622fc, 0xfc2820fc, 0xfc2a1efc,
	0xfc2c1cfc, 0xfc2e1afc, 0xfd3017fc, 0xfd3115fd,
	0xfe3213fd, 0xfe3411fd, 0xff350ffd, 0xff360dfe,
	0x00370bfe, 0x013809fe, 0x023808fe, 0x033806ff,

	0xfd093208, 0xfd0a3207, 0xfd0c3205, 0xfd0d3204,
	0xfc0f3203, 0xfc113102, 0xfc123002, 0xfc143000,
	0xfc152f00, 0xfc172d00, 0xfc192cff, 0xfc1b2bfe,
	0xfc1d29fe, 0xfc1e28fe, 0xfc2027fd, 0xfd2125fd,
	0xfd2323fd, 0xfd2521fd, 0xfd2720fc, 0xfe281efc,
	0xfe291dfc, 0xfe2b1bfc, 0xff2c19fc, 0x002d17fc,
	0x002e16fc, 0x012f14fc, 0x022f12fd, 0x023110fd,
	0x03310ffd, 0x05310dfd, 0x06320bfd, 0x07320afd,

	0xfc0c2d0b, 0xfc0d2d0a, 0xfc0e2d09, 0xfc102d07,
	0xfc112c07, 0xfc132c05, 0xfc142c04, 0xfc162b03,
	0xfc172a03, 0xfc192a01, 0xfc1a2901, 0xfd1b2800,
	0xfd1c2700, 0xfd1e2500, 0xfe1f24ff, 0xfe2023ff,
	0xfe2222fe, 0xff2320fe, 0xff241ffe, 0x00251efd,
	0x00271cfd, 0x01271bfd, 0x01281afd, 0x022918fd,
	0x032a16fd, 0x032b15fd, 0x042b14fd, 0x052c12fd,
	0x072c10fd, 0x082c0ffd, 0x092c0efd, 0x0a2c0dfd,

	0xfd0d290d, 0xfd0e290c, 0xfd0f290b, 0xfd11280a,
	0xfd122809, 0xfd132808, 0xfd142807, 0xfd162706,
	0xfd172705, 0xfd192604, 0xfe1a2503, 0xfe1b2502,
	0xfe1c2402, 0xfe1d2302, 0xff1e2201, 0xff1f2101,
	0x00202000, 0x00211f00, 0x01221eff, 0x02221dff,
	0x02241cfe, 0x03241bfe, 0x042519fe, 0x042618fe,
	0x052617fe, 0x062716fd, 0x072714fe, 0x082713fe,
	0x092812fd, 0x0a2811fd, 0x0b2810fd, 0x0c280ffd,

	0xfd0f250f, 0xfd10250e, 0xfd11250d, 0xfd12250c,
	0xfd13250b, 0xfe13250a, 0xfe152409, 0xfe162408,
	0xfe172308, 0xff182306, 0xff192305, 0xff1a2205,
	0x001b2104, 0x001c2103, 0x001d2003, 0x011e1f02,
	0x011f1f01, 0x021f1e01, 0x03201d00, 0x03211c00,
	0x04211b00, 0x05221aff, 0x062219ff, 0x062318ff,
	0x082316ff, 0x082316ff, 0x092415fe, 0x0a2414fe,
	0x0b2413fe, 0x0c2412fe, 0x0d2411fe, 0x0e2410fe,

	0xfe10230f, 0xfe11230e, 0xfe12220e, 0xfe13220d,
	0xfe14220c, 0xff14220b, 0xff15220a, 0xff16210a,
	0x00162109, 0x00172108, 0x00182008, 0x01192006,
	0x011a1f06, 0x021a1f05, 0x021b1e05, 0x031c1d04,
	0x031d1d03, 0x041d1c03, 0x041e1b03, 0x051e1b02,
	0x061f1a01, 0x06201901, 0x07201801, 0x08201800,
	0x09201700, 0x0a211500, 0x0b2115ff, 0x0c2114ff,
	0x0c2213ff, 0x0d2212ff, 0x0e2211ff, 0x0f2211fe,

	0xff112010, 0xff12200f, 0xff12200f, 0xff13200e,
	0x0013200d, 0x0014200c, 0x00151f0c, 0x00161f0b,
	0x01161f0a, 0x01171e0a, 0x02171e09, 0x02181e08,
	0x03191d07, 0x03191d07, 0x041a1c06, 0x041b1c05,
	0x051b1b05, 0x051c1b04, 0x061c1a04, 0x071c1a03,
	0x071d1903, 0x081e1802, 0x091d1802, 0x091e1702,
	0x0a1f1601, 0x0b1f1600, 0x0b1f1501, 0x0c201400,
	0x0d1f1400, 0x0e2013ff, 0x0f1f1200, 0x102011ff,

	0x00111f10, 0x00121e10, 0x00131e0f, 0x00131e0f,
	0x01131e0e, 0x01141e0d, 0x01151d0d, 0x02151d0c,
	0x02161d0b, 0x03161d0a, 0x03171c0a, 0x04171c09,
	0x04181c08, 0x05181b08, 0x05191b07, 0x06191a07,
	0x061a1a06, 0x071a1906, 0x071b1905, 0x081b1805,
	0x081c1804, 0x091c1704, 0x0a1c1703, 0x0a1d1603,
	0x0b1d1602, 0x0c1d1502, 0x0c1d1502, 0x0d1e1401,
	0x0e1d1401, 0x0e1e1301, 0x0f1e1300, 0x101e1200,

	0x02111c11, 0x02121c10, 0x02131b10, 0x03131b0f,
	0x03131b0f, 0x03141b0e, 0x04141b0d, 0x04151a0d,
	0x05151a0c, 0x05151a0c, 0x05161a0b, 0x0616190b,
	0x0616190b, 0x0716190a, 0x0717180a, 0x08171809,
	0x08181808, 0x09181708, 0x09181708, 0x0a181707,
	0x0a191607, 0x0b191606, 0x0b1a1605, 0x0c1a1505,
	0x0c1a1505, 0x0d1a1504, 0x0d1b1404, 0x0e1b1403,
	0x0f1b1303, 0x0f1b1303, 0x101b1302, 0x101c1202,

	0x04111a11, 0x04121911, 0x04131910, 0x0513190f,
	0x0513190f, 0x0513190f, 0x0613190e, 0x0614180e,
	0x0714180d, 0x0714180d, 0x0715180c, 0x0814180c,
	0x0815170c, 0x0816170b, 0x0916170a, 0x0916170a,
	0x0a16160a, 0x0a171609, 0x0a171609, 0x0b171608,
	0x0b171509, 0x0c171508, 0x0c181507, 0x0d171507,
	0x0d181407, 0x0e181406, 0x0e181406, 0x0e191306,
	0x0f191305, 0x0f191305, 0x10191304, 0x10191205,

	0x05121811, 0x06121810, 0x06121810, 0x06131710,
	0x0713170f, 0x0713170f, 0x0713170f, 0x0813170e,
	0x0813170e, 0x0814170d, 0x0914160d, 0x0914160d,
	0x0914160d, 0x0a14160c, 0x0a15160b, 0x0a15150c,
	0x0b15150b, 0x0b15150b, 0x0b16150a, 0x0c15150a,
	0x0c16140a, 0x0d161409, 0x0d161409, 0x0d171408,
	0x0e161408, 0x0e171308, 0x0e171308, 0x0f171307,
	0x0f171307, 0x10171306, 0x10181206, 0x10181206,

	0x07111711, 0x07121710, 0x07121611, 0x08121610,
	0x08121610, 0x0813160f, 0x0912160f, 0x0913160e,
	0x0913160e, 0x0913160e, 0x0a14150d, 0x0a14150d,
	0x0a14150d, 0x0b14150c, 0x0b14150c, 0x0b14150c,
	0x0c14140c, 0x0c15140b, 0x0c15140b, 0x0c15140b,
	0x0d15140a, 0x0d15140a, 0x0d15140a, 0x0e161309,
	0x0e161309, 0x0e161309, 0x0f151309, 0x0f161308,
	0x0f161209, 0x10161208, 0x10161208, 0x10171207,

	0x0a111411, 0x0b111410, 0x0b111410, 0x0b111410,
	0x0b111410, 0x0b12140f, 0x0b12140f, 0x0c12130f,
	0x0c12130f, 0x0c12130f, 0x0c12130f, 0x0c12130f,
	0x0d12130e, 0x0d12130e, 0x0d12130e, 0x0d13130d,
	0x0d13130d, 0x0d13130d, 0x0e12130d, 0x0e13120d,
	0x0e13120d, 0x0e13120d, 0x0e13120d, 0x0f13120c,
	0x0f13120c, 0x0f13120c, 0x0f14120b, 0x0f14120b,
	0x1013120b, 0x1013120b, 0x1013120b, 0x1014110b,

	0x0c111310, 0x0c111310, 0x0c111310, 0x0d101310,
	0x0d101310, 0x0d111210, 0x0d111210, 0x0d111210,
	0x0d12120f, 0x0d12120f, 0x0d12120f, 0x0d12120f,
	0x0e11120f, 0x0e12120e, 0x0e12120e, 0x0e12120e,
	0x0e12120e, 0x0e12120e, 0x0e12120e, 0x0e12120e,
	0x0f11120e, 0x0f12120d, 0x0f12120d, 0x0f12120d,
	0x0f12120d, 0x0f12110e, 0x0f12110e, 0x0f12110e,
	0x1012110d, 0x1012110d, 0x1013110c, 0x1013110c,
};

static const uint32_t lan3coefftab32_left[480] = {
	0x40000000, 0x40fe0000, 0x3ffd0100, 0x3efc0100,
	0x3efb0100, 0x3dfa0200, 0x3cf90200, 0x3bf80200,
	0x39f70200, 0x37f70200, 0x35f70200, 0x33f70200,
	0x31f70200, 0x2ef70200, 0x2cf70200, 0x2af70200,
	0x27f70200, 0x24f80100, 0x22f80100, 0x1ef90100,
	0x1cf90100, 0x19fa0100, 0x17fa0100, 0x14fb0100,
	0x11fc0000, 0x0ffc0000, 0x0cfd0000, 0x0afd0000,
	0x08fe0000, 0x05ff0000, 0x03ff0000, 0x02000000,

	0x3806fc02, 0x3805fc02, 0x3803fd01, 0x3801fe01,
	0x3700fe01, 0x35ffff01, 0x35fdff01, 0x34fc0001,
	0x34fb0000, 0x33fa0000, 0x31fa0100, 0x2ff90100,
	0x2df80200, 0x2bf80200, 0x2af70200, 0x28f70200,
	0x27f70200, 0x24f70300, 0x22f70300, 0x1ff70300,
	0x1ef70300, 0x1cf70300, 0x1af70300, 0x18f70300,
	0x16f80300, 0x13f80300, 0x11f90300, 0x0ef90300,
	0x0efa0200, 0x0cfa0200, 0x0afb0200, 0x08fb0200,

	0x320bfa02, 0x3309fa02, 0x3208fb02, 0x3206fb02,
	0x3205fb02, 0x3104fc02, 0x3102fc01, 0x3001fd01,
	0x3000fd01, 0x2ffffd01, 0x2efefe01, 0x2dfdfe01,
	0x2bfcff01, 0x29fcff01, 0x28fbff01, 0x27fa0001,
	0x26fa0000, 0x24f90000, 0x22f90100, 0x20f90100,
	0x1ff80100, 0x1ef80100, 0x1cf80100, 0x1af80200,
	0x18f80200, 0x17f80200, 0x15f80200, 0x12f80200,
	0x11f90200, 0x0ff90200, 0x0df90200, 0x0cfa0200,

	0x2e0efa01, 0x2f0dfa01, 0x2f0bfa01, 0x2e0afa01,
	0x2e09fa01, 0x2e07fb01, 0x2d06fb01, 0x2d05fb01,
	0x2c04fb01, 0x2b03fc01, 0x2a02fc01, 0x2a01fc01,
	0x2800fd01, 0x28fffd01, 0x26fefd01, 0x25fefe01,
	0x24fdfe01, 0x23fcfe01, 0x21fcff01, 0x20fbff01,
	0x1efbff01, 0x1efbff00, 0x1cfa0000, 0x1bfa0000,
	0x19fa0000, 0x18fa0000, 0x17f90000, 0x15f90100,
	0x14f90100, 0x12f90100, 0x11f90100, 0x0ff90100,

	0x2b10fa00, 0x2b0ffa00, 0x2b0efa00, 0x2b0cfa00,
	0x2b0bfa00, 0x2a0afb01, 0x2a09fb01, 0x2908fb01,
	0x2807fb01, 0x2806fb01, 0x2805fb01, 0x2604fc01,
	0x2503fc01, 0x2502fc01, 0x2401fc01, 0x2301fc01,
	0x2100fd01, 0x21fffd01, 0x21fffd01, 0x20fefd01,
	0x1dfefe01, 0x1cfdfe01, 0x1cfdfe00, 0x1bfcfe00,
	0x19fcff00, 0x19fbff00, 0x17fbff00, 0x16fbff00,
	0x15fbff00, 0x14fb0000, 0x13fa0000, 0x11fa0000,

	0x2811fcff, 0x2810fcff, 0x280ffbff, 0x280efbff,
	0x270dfb00, 0x270cfb00, 0x270bfb00, 0x260afb00,
	0x2609fb00, 0x2508fb00, 0x2507fb00, 0x2407fb00,
	0x2406fc00, 0x2305fc00, 0x2204fc00, 0x2203fc00,
	0x2103fc00, 0x2002fc00, 0x1f01fd00, 0x1e01fd00,
	0x1d00fd00, 0x1dfffd00, 0x1cfffd00, 0x1bfefd00,
	0x1afefe00, 0x19fefe00, 0x18fdfe00, 0x17fdfe00,
	0x16fdfe00, 0x15fcff00, 0x13fcff00, 0x12fcff00,

	0x2512fdfe, 0x2511fdff, 0x2410fdff, 0x240ffdff,
	0x240efcff, 0x240dfcff, 0x240dfcff, 0x240cfcff,
	0x230bfcff, 0x230afc00, 0x2209fc00, 0x2108fc00,
	0x2108fc00, 0x2007fc00, 0x2006fc00, 0x2005fc00,
	0x1f05fc00, 0x1e04fc00, 0x1e03fc00, 0x1c03fd00,
	0x1c02fd00, 0x1b02fd00, 0x1b01fd00, 0x1a00fd00,
	0x1900fd00, 0x1800fd00, 0x17fffe00, 0x16fffe00,
	0x16fefe00, 0x14fefe00, 0x13fefe00, 0x13fdfe00,

	0x2212fffe, 0x2211fefe, 0x2211fefe, 0x2110fefe,
	0x210ffeff, 0x220efdff, 0x210dfdff, 0x210dfdff,
	0x210cfdff, 0x210bfdff, 0x200afdff, 0x200afdff,
	0x1f09fdff, 0x1f08fdff, 0x1d08fd00, 0x1c07fd00,
	0x1d06fd00, 0x1b06fd00, 0x1b05fd00, 0x1c04fd00,
	0x1b04fd00, 0x1a03fd00, 0x1a03fd00, 0x1902fd00,
	0x1802fd00, 0x1801fd00, 0x1701fd00, 0x1600fd00,
	0x1400fe00, 0x1400fe00, 0x14fffe00, 0x13fffe00,

	0x201200fe, 0x201100fe, 0x1f11fffe, 0x2010fffe,
	0x1f0ffffe, 0x1e0ffffe, 0x1f0efeff, 0x1f0dfeff,
	0x1f0dfeff, 0x1e0cfeff, 0x1e0bfeff, 0x1d0bfeff,
	0x1d0afeff, 0x1d09fdff, 0x1d09fdff, 0x1c08fdff,
	0x1c07fdff, 0x1b07fd00, 0x1b06fd00, 0x1a06fd00,
	0x1a05fd00, 0x1805fd00, 0x1904fd00, 0x1804fd00,
	0x1703fd00, 0x1703fd00, 0x1602fe00, 0x1502fe00,
	0x1501fe00, 0x1401fe00, 0x1301fe00, 0x1300fe00,

	0x1c1202fe, 0x1c1102fe, 0x1b1102fe, 0x1c1001fe,
	0x1b1001fe, 0x1b0f01ff, 0x1b0e00ff, 0x1b0e00ff,
	0x1b0d00ff, 0x1a0d00ff, 0x1a0c00ff, 0x1a0cffff,
	0x1a0bffff, 0x1a0bffff, 0x1a0affff, 0x180affff,
	0x1909ffff, 0x1809ffff, 0x1808ffff, 0x1808feff,
	0x1807feff, 0x1707fe00, 0x1606fe00, 0x1506fe00,
	0x1605fe00, 0x1505fe00, 0x1504fe00, 0x1304fe00,
	0x1304fe00, 0x1303fe00, 0x1203fe00, 0x1203fe00,

	0x181104ff, 0x191103ff, 0x191003ff, 0x181003ff,
	0x180f03ff, 0x190f02ff, 0x190e02ff, 0x180e02ff,
	0x180d02ff, 0x180d01ff, 0x180d01ff, 0x180c01ff,
	0x180c01ff, 0x180b00ff, 0x170b00ff, 0x170a00ff,
	0x170a00ff, 0x170900ff, 0x160900ff, 0x160900ff,
	0x1608ffff, 0x1508ffff, 0x1507ff00, 0x1507ff00,
	0x1407ff00, 0x1306ff00, 0x1306ff00, 0x1305ff00,
	0x1205ff00, 0x1105ff00, 0x1204ff00, 0x1104ff00,

	0x171005ff, 0x171005ff, 0x171004ff, 0x170f04ff,
	0x160f04ff, 0x170f03ff, 0x170e03ff, 0x160e03ff,
	0x160d03ff, 0x160d02ff, 0x160d02ff, 0x160c02ff,
	0x160c02ff, 0x160c02ff, 0x160b01ff, 0x150b01ff,
	0x150a01ff, 0x150a01ff, 0x150a01ff, 0x140901ff,
	0x14090000, 0x14090000, 0x14080000, 0x13080000,
	0x13070000, 0x12070000, 0x12070000, 0x12060000,
	0x11060000, 0x11060000, 0x11050000, 0x1105ff00,

	0x14100600, 0x15100500, 0x150f0500, 0x150f0500,
	0x140f0500, 0x150e0400, 0x140e0400, 0x130e0400,
	0x140d0400, 0x150d0300, 0x130d0300, 0x140c0300,
	0x140c0300, 0x140c0200, 0x140b0200, 0x130b0200,
	0x120b0200, 0x130a0200, 0x130a0200, 0x130a0100,
	0x13090100, 0x12090100, 0x11090100, 0x12080100,
	0x11080100, 0x10080100, 0x11070100, 0x11070000,
	0x10070000, 0x11060000, 0x10060000, 0x10060000,

	0x140f0600, 0x140f0600, 0x130f0600, 0x140f0500,
	0x140e0500, 0x130e0500, 0x130e0500, 0x140d0400,
	0x140d0400, 0x130d0400, 0x120d0400, 0x130c0400,
	0x130c0300, 0x130c0300, 0x130b0300, 0x130b0300,
	0x110b0300, 0x130a0200, 0x120a0200, 0x120a0200,
	0x120a0200, 0x12090200, 0x10090200, 0x11090100,
	0x11080100, 0x11080100, 0x10080100, 0x10080100,
	0x10070100, 0x10070100, 0x0f070100, 0x10060100,

	0x120f0701, 0x130f0601, 0x130e0601, 0x130e0601,
	0x120e0601, 0x130e0501, 0x130e0500, 0x130d0500,
	0x120d0500, 0x120d0500, 0x130c0400, 0x130c0400,
	0x120c0400, 0x110c0400, 0x120b0400, 0x120b0300,
	0x120b0300, 0x120b0300, 0x120a0300, 0x110a0300,
	0x110a0200, 0x11090200, 0x11090200, 0x10090200,
	0x10090200, 0x10080200, 0x10080200, 0x10080100,
	0x0f080100, 0x10070100, 0x0f070100, 0x0f070100
};

static const uint32_t lan3coefftab32_right[480] = {
	0x00000000, 0x00000002, 0x0000ff04, 0x0000ff06,
	0x0000fe08, 0x0000fd0a, 0x0000fd0c, 0x0000fc0f,
	0x0000fc12, 0x0001fb14, 0x0001fa17, 0x0001fa19,
	0x0001f91c, 0x0001f91f, 0x0001f822, 0x0001f824,
	0x0002f727, 0x0002f72a, 0x0002f72c, 0x0002f72f,
	0x0002f731, 0x0002f733, 0x0002f735, 0x0002f737,
	0x0002f73a, 0x0002f83b, 0x0002f93c, 0x0002fa3d,
	0x0001fb3e, 0x0001fc3f, 0x0001fd40, 0x0000fe40,

	0x0002fc06, 0x0002fb08, 0x0002fb0a, 0x0002fa0c,
	0x0002fa0e, 0x0003f910, 0x0003f912, 0x0003f814,
	0x0003f816, 0x0003f719, 0x0003f71a, 0x0003f71d,
	0x0003f71f, 0x0003f721, 0x0003f723, 0x0003f725,
	0x0002f727, 0x0002f729, 0x0002f72b, 0x0002f82d,
	0x0002f82e, 0x0001f930, 0x0001fa31, 0x0000fa34,
	0x0000fb34, 0x0100fc35, 0x01fffd36, 0x01ffff37,
	0x01fe0037, 0x01fe0138, 0x01fd0338, 0x02fc0538,

	0x0002fa0b, 0x0002fa0c, 0x0002f90e, 0x0002f910,
	0x0002f911, 0x0002f813, 0x0002f816, 0x0002f817,
	0x0002f818, 0x0002f81a, 0x0001f81c, 0x0001f81e,
	0x0001f820, 0x0001f921, 0x0001f923, 0x0000f925,
	0x0000fa26, 0x0100fa28, 0x01fffb29, 0x01fffc2a,
	0x01fffc2c, 0x01fefd2d, 0x01fefe2e, 0x01fdff2f,
	0x01fd0030, 0x01fd0130, 0x01fc0232, 0x02fc0432,
	0x02fb0532, 0x02fb0633, 0x02fb0833, 0x02fa0933,

	0x0001fa0e, 0x0001f90f, 0x0001f911, 0x0001f913,
	0x0001f914, 0x0001f915, 0x0000f918, 0x0000fa18,
	0x0000fa1a, 0x0000fa1b, 0x0000fa1d, 0x00fffb1e,
	0x01fffb1f, 0x01fffb20, 0x01fffc22, 0x01fefc23,
	0x01fefd24, 0x01fefe25, 0x01fdfe27, 0x01fdff28,
	0x01fd0029, 0x01fc012a, 0x01fc022b, 0x01fc032b,
	0x01fb042d, 0x01fb052d, 0x01fb062e, 0x01fb072e,
	0x01fa092e, 0x01fa0a2f, 0x01fa0b2f, 0x01fa0d2f,

	0x0000fa11, 0x0000fa12, 0x0000fa13, 0x0000fb14,
	0x00fffb16, 0x00fffb16, 0x00fffb17, 0x00fffb19,
	0x00fffc1a, 0x00fefc1c, 0x00fefd1c, 0x01fefd1d,
	0x01fefe1e, 0x01fdfe20, 0x01fdff21, 0x01fdff22,
	0x01fd0023, 0x01fc0124, 0x01fc0124, 0x01fc0225,
	0x01fc0326, 0x01fc0427, 0x01fb0528, 0x01fb0629,
	0x01fb0729, 0x01fb0829, 0x01fb092a, 0x01fb0a2a,
	0x00fa0b2c, 0x00fa0c2b, 0x00fa0e2b, 0x00fa0f2c,

	0x00fffc11, 0x00fffc12, 0x00fffc14, 0x00fffc15,
	0x00fefd16, 0x00fefd17, 0x00fefd18, 0x00fefe19,
	0x00fefe1a, 0x00fdfe1d, 0x00fdff1d, 0x00fdff1e,
	0x00fd001d, 0x00fd011e, 0x00fd0120, 0x00fc0221,
	0x00fc0321, 0x00fc0323, 0x00fc0423, 0x00fc0523,
	0x00fc0624, 0x00fb0725, 0x00fb0726, 0x00fb0827,
	0x00fb0926, 0x00fb0a26, 0x00fb0b27, 0x00fb0c27,
	0x00fb0d27, 0xfffb0e28, 0xfffb0f29, 0xfffc1028,

	0x00fefd13, 0x00fefd13, 0x00fefe14, 0x00fefe15,
	0x00fefe17, 0x00feff17, 0x00feff17, 0x00fd0018,
	0x00fd001a, 0x00fd001a, 0x00fd011b, 0x00fd021c,
	0x00fd021c, 0x00fd031d, 0x00fc031f, 0x00fc041f,
	0x00fc051f, 0x00fc0521, 0x00fc0621, 0x00fc0721,
	0x00fc0821, 0x00fc0822, 0x00fc0922, 0x00fc0a23,
	0xfffc0b24, 0xfffc0c24, 0xfffc0d24, 0xfffc0d25,
	0xfffc0e25, 0xfffd0f25, 0xfffd1025, 0xfffd1125,

	0x00feff12, 0x00feff14, 0x00feff14, 0x00fe0015,
	0x00fe0015, 0x00fd0017, 0x00fd0118, 0x00fd0118,
	0x00fd0218, 0x00fd0219, 0x00fd031a, 0x00fd031a,
	0x00fd041b, 0x00fd041c, 0x00fd051c, 0x00fd061d,
	0x00fd061d, 0x00fd071e, 0x00fd081e, 0xfffd081f,
	0xfffd091f, 0xfffd0a20, 0xfffd0a20, 0xfffd0b21,
	0xfffd0c21, 0xfffd0d21, 0xfffd0d22, 0xfffd0e23,
	0xfffe0f22, 0xfefe1022, 0xfefe1122, 0xfefe1123,

	0x00fe0012, 0x00fe0013, 0x00fe0114, 0x00fe0114,
	0x00fe0116, 0x00fe0216, 0x00fe0216, 0x00fd0317,
	0x00fd0317, 0x00fd0418, 0x00fd0419, 0x00fd0519,
	0x00fd051a, 0x00fd061b, 0x00fd061b, 0x00fd071c,
	0xfffd071e, 0xfffd081d, 0xfffd091d, 0xfffd091e,
	0xfffe0a1d, 0xfffe0b1e, 0xfffe0b1e, 0xfffe0c1e,
	0xfffe0d1f, 0xfffe0d1f, 0xfffe0e1f, 0xfeff0f1f,
	0xfeff0f20, 0xfeff1020, 0xfeff1120, 0xfe001120,

	0x00fe0212, 0x00fe0312, 0x00fe0313, 0x00fe0314,
	0x00fe0414, 0x00fe0414, 0x00fe0416, 0x00fe0515,
	0x00fe0516, 0x00fe0616, 0x00fe0617, 0x00fe0717,
	0xfffe0719, 0xfffe0818, 0xffff0818, 0xffff0919,
	0xffff0919, 0xffff0a19, 0xffff0a1a, 0xffff0b1a,
	0xffff0b1b, 0xffff0c1a, 0xff000c1b, 0xff000d1b,
	0xff000d1b, 0xff000e1b, 0xff000e1c, 0xff010f1c,
	0xfe01101c, 0xfe01101d, 0xfe02111c, 0xfe02111c,

	0x00ff0411, 0x00ff0411, 0x00ff0412, 0x00ff0512,
	0x00ff0513, 0x00ff0513, 0x00ff0613, 0x00ff0614,
	0x00ff0714, 0x00ff0715, 0x00ff0715, 0xffff0816,
	0xffff0816, 0xff000916, 0xff000917, 0xff000918,
	0xff000a17, 0xff000a18, 0xff000b18, 0xff000b18,
	0xff010c18, 0xff010c19, 0xff010d18, 0xff010d18,
	0xff020d18, 0xff020e19, 0xff020e19, 0xff020f19,
	0xff030f19, 0xff031019, 0xff031019, 0xff031119,

	0x00ff0511, 0x00ff0511, 0x00000511, 0x00000611,
	0x00000612, 0x00000612, 0x00000712, 0x00000713,
	0x00000714, 0x00000814, 0x00000814, 0x00000914,
	0x00000914, 0xff010914, 0xff010a15, 0xff010a16,
	0xff010a17, 0xff010b16, 0xff010b16, 0xff020c16,
	0xff020c16, 0xff020c16, 0xff020d16, 0xff020d17,
	0xff030d17, 0xff030e17, 0xff030e17, 0xff030f17,
	0xff040f17, 0xff040f17, 0xff041017, 0xff051017,

	0x00000610, 0x00000610, 0x00000611, 0x00000611,
	0x00000711, 0x00000712, 0x00010712, 0x00010812,
	0x00010812, 0x00010812, 0x00010913, 0x00010913,
	0x00010913, 0x00010a13, 0x00020a13, 0x00020a14,
	0x00020b14, 0x00020b14, 0x00020b14, 0x00020c14,
	0x00030c14, 0x00030c15, 0x00030d15, 0x00030d15,
	0x00040d15, 0x00040e15, 0x00040e15, 0x00040e16,
	0x00050f15, 0x00050f15, 0x00050f16, 0x00051015,

	0x00000611, 0x00010610, 0x00010710, 0x00010710,
	0x00010711, 0x00010811, 0x00010811, 0x00010812,
	0x00010812, 0x00010912, 0x00020912, 0x00020912,
	0x00020a12, 0x00020a12, 0x00020a13, 0x00020a13,
	0x00030b13, 0x00030b13, 0x00030b14, 0x00030c13,
	0x00030c13, 0x00040c13, 0x00040d14, 0x00040d14,
	0x00040d15, 0x00040d15, 0x00050e14, 0x00050e14,
	0x00050e15, 0x00050f14, 0x00060f14, 0x00060f14,

	0x0001070f, 0x0001070f, 0x00010710, 0x00010710,
	0x00010810, 0x00010810, 0x00020810, 0x00020811,
	0x00020911, 0x00020911, 0x00020912, 0x00020912,
	0x00020a12, 0x00030a12, 0x00030a12, 0x00030b12,
	0x00030b12, 0x00030b12, 0x00040b12, 0x00040c12,
	0x00040c13, 0x00040c14, 0x00040c14, 0x00050d13,
	0x00050d13, 0x00050d14, 0x00050e13, 0x01050e13,
	0x01060e13, 0x01060e13, 0x01060e14, 0x01060f13
};

static const uint32_t lan2coefftab32[480] = {
	0x00004000, 0x000140ff, 0x00033ffe, 0x00043ffd,
	0x00063efc, 0xff083dfc, 0x000a3bfb, 0xff0d39fb,
	0xff0f37fb, 0xff1136fa, 0xfe1433fb, 0xfe1631fb,
	0xfd192ffb, 0xfd1c2cfb, 0xfd1f29fb, 0xfc2127fc,
	0xfc2424fc, 0xfc2721fc, 0xfb291ffd, 0xfb2c1cfd,
	0xfb2f19fd, 0xfb3116fe, 0xfb3314fe, 0xfa3611ff,
	0xfb370fff, 0xfb390dff, 0xfb3b0a00, 0xfc3d08ff,
	0xfc3e0600, 0xfd3f0400, 0xfe3f0300, 0xff400100,

	0xff053804, 0xff063803, 0xff083801, 0xff093701,
	0xff0a3700, 0xff0c3500, 0xff0e34ff, 0xff1033fe,
	0xff1232fd, 0xfe1431fd, 0xfe162ffd, 0xfe182dfd,
	0xfd1b2cfc, 0xfd1d2afc, 0xfd1f28fc, 0xfd2126fc,
	0xfd2323fd, 0xfc2621fd, 0xfc281ffd, 0xfc2a1dfd,
	0xfc2c1bfd, 0xfd2d18fe, 0xfd2f16fe, 0xfd3114fe,
	0xfd3212ff, 0xfe3310ff, 0xff340eff, 0x00350cff,
	0x00360a00, 0x01360900, 0x02370700, 0x03370600,

	0xff083207, 0xff093206, 0xff0a3205, 0xff0c3203,
	0xff0d3103, 0xff0e3102, 0xfe113001, 0xfe132f00,
	0xfe142e00, 0xfe162dff, 0xfe182bff, 0xfe192aff,
	0xfe1b29fe, 0xfe1d27fe, 0xfe1f25fe, 0xfd2124fe,
	0xfe2222fe, 0xfe2421fd, 0xfe251ffe, 0xfe271dfe,
	0xfe291bfe, 0xff2a19fe, 0xff2b18fe, 0xff2d16fe,
	0x002e14fe, 0x002f12ff, 0x013010ff, 0x02300fff,
	0x03310dff, 0x04310cff, 0x05310a00, 0x06310900,

	0xff0a2e09, 0xff0b2e08, 0xff0c2e07, 0xff0e2d06,
	0xff0f2d05, 0xff102d04, 0xff122c03, 0xfe142c02,
	0xfe152b02, 0xfe172a01, 0xfe182901, 0xfe1a2800,
	0xfe1b2700, 0xfe1d2500, 0xff1e24ff, 0xfe2023ff,
	0xff2121ff, 0xff2320fe, 0xff241eff, 0x00251dfe,
	0x00261bff, 0x00281afe, 0x012818ff, 0x012a16ff,
	0x022a15ff, 0x032b13ff, 0x032c12ff, 0x052c10ff,
	0x052d0fff, 0x062d0d00, 0x072d0c00, 0x082d0b00,

	0xff0c2a0b, 0xff0d2a0a, 0xff0e2a09, 0xff0f2a08,
	0xff102a07, 0xff112a06, 0xff132905, 0xff142904,
	0xff162803, 0xff172703, 0xff182702, 0xff1a2601,
	0xff1b2501, 0xff1c2401, 0xff1e2300, 0xff1f2200,
	0x00202000, 0x00211f00, 0x01221d00, 0x01231c00,
	0x01251bff, 0x02251aff, 0x032618ff, 0x032717ff,
	0x042815ff, 0x052814ff, 0x052913ff, 0x06291100,
	0x072a10ff, 0x082a0e00, 0x092a0d00, 0x0a2a0c00,

	0xff0d280c, 0xff0e280b, 0xff0f280a, 0xff102809,
	0xff112808, 0xff122708, 0xff142706, 0xff152705,
	0xff162605, 0xff172604, 0xff192503, 0xff1a2403,
	0x001b2302, 0x001c2202, 0x001d2201, 0x001e2101,
	0x011f1f01, 0x01211e00, 0x01221d00, 0x02221c00,
	0x02231b00, 0x03241900, 0x04241800, 0x04251700,
	0x052616ff, 0x06261400, 0x072713ff, 0x08271100,
	0x08271100, 0x09271000, 0x0a280e00, 0x0b280d00,

	0xff0e260d, 0xff0f260c, 0xff10260b, 0xff11260a,
	0xff122609, 0xff132608, 0xff142508, 0xff152507,
	0x00152506, 0x00172405, 0x00182305, 0x00192304,
	0x001b2203, 0x001c2103, 0x011d2002, 0x011d2002,
	0x011f1f01, 0x021f1e01, 0x02201d01, 0x03211c00,
	0x03221b00, 0x04221a00, 0x04231801, 0x05241700,
	0x06241600, 0x07241500, 0x08251300, 0x09251200,
	0x09261100, 0x0a261000, 0x0b260f00, 0x0c260e00,

	0xff0e250e, 0xff0f250d, 0xff10250c, 0xff11250b,
	0x0011250a, 0x00132409, 0x00142408, 0x00152407,
	0x00162307, 0x00172306, 0x00182206, 0x00192205,
	0x011a2104, 0x011b2004, 0x011c2003, 0x021c1f03,
	0x021e1e02, 0x031e1d02, 0x03201c01, 0x04201b01,
	0x04211a01, 0x05221900, 0x05221801, 0x06231700,
	0x07231600, 0x07241500, 0x08241400, 0x09241300,
	0x0a241200, 0x0b241100, 0x0c241000, 0x0d240f00,

	0x000e240e, 0x000f240d, 0x0010240c, 0x0011240b,
	0x0013230a, 0x0013230a, 0x00142309, 0x00152308,
	0x00162208, 0x00172207, 0x01182106, 0x01192105,
	0x011a2005, 0x021b1f04, 0x021b1f04, 0x021d1e03,
	0x031d1d03, 0x031e1d02, 0x041e1c02, 0x041f1b02,
	0x05201a01, 0x05211901, 0x06211801, 0x07221700,
	0x07221601, 0x08231500, 0x09231400, 0x0a231300,
	0x0a231300, 0x0b231200, 0x0c231100, 0x0d231000,

	0x000f220f, 0x0010220e, 0x0011220d, 0x0012220c,
	0x0013220b, 0x0013220b, 0x0015210a, 0x0015210a,
	0x01162108, 0x01172008, 0x01182007, 0x02191f06,
	0x02191f06, 0x021a1e06, 0x031a1e05, 0x031c1d04,
	0x041c1c04, 0x041d1c03, 0x051d1b03, 0x051e1a03,
	0x061f1902, 0x061f1902, 0x07201801, 0x08201701,
	0x08211601, 0x09211501, 0x0a211500, 0x0b211400,
	0x0b221300, 0x0c221200, 0x0d221100, 0x0e221000,

	0x0010210f, 0x0011210e, 0x0011210e, 0x0012210d,
	0x0013210c, 0x0014200c, 0x0114200b, 0x0115200a,
	0x01161f0a, 0x01171f09, 0x02171f08, 0x02181e08,
	0x03181e07, 0x031a1d06, 0x031a1d06, 0x041b1c05,
	0x041c1c04, 0x051c1b04, 0x051d1a04, 0x061d1a03,
	0x071d1903, 0x071e1803, 0x081e1802, 0x081f1702,
	0x091f1602, 0x0a201501, 0x0b1f1501, 0x0b201401,
	0x0c211300, 0x0d211200, 0x0e201200, 0x0e211100,

	0x00102010, 0x0011200f, 0x0012200e, 0x0013200d,
	0x0013200d, 0x01141f0c, 0x01151f0b, 0x01151f0b,
	0x01161f0a, 0x02171e09, 0x02171e09, 0x03181d08,
	0x03191d07, 0x03191d07, 0x041a1c06, 0x041b1c05,
	0x051b1b05, 0x051c1b04, 0x061c1a04, 0x071d1903,
	0x071d1903, 0x081d1803, 0x081e1703, 0x091e1702,
	0x0a1f1601, 0x0a1f1502, 0x0b1f1501, 0x0c1f1401,
	0x0d201300, 0x0d201300, 0x0e201200, 0x0f201100,

	0x00102010, 0x0011200f, 0x00121f0f, 0x00131f0e,
	0x00141f0d, 0x01141f0c, 0x01141f0c, 0x01151e0c,
	0x02161e0a, 0x02171e09, 0x03171d09, 0x03181d08,
	0x03181d08, 0x04191c07, 0x041a1c06, 0x051a1b06,
	0x051b1b05, 0x061b1a05, 0x061c1a04, 0x071c1904,
	0x081c1903, 0x081d1803, 0x091d1703, 0x091e1702,
	0x0a1e1602, 0x0b1e1502, 0x0c1e1501, 0x0c1f1401,
	0x0d1f1400, 0x0e1f1300, 0x0e1f1201, 0x0f1f1200,

	0x00111e11, 0x00121e10, 0x00131e0f, 0x00131e0f,
	0x01131e0e, 0x01141d0e, 0x02151d0c, 0x02151d0c,
	0x02161d0b, 0x03161c0b, 0x03171c0a, 0x04171c09,
	0x04181b09, 0x05181b08, 0x05191b07, 0x06191a07,
	0x061a1a06, 0x071a1906, 0x071b1905, 0x081b1805,
	0x091b1804, 0x091c1704, 0x0a1c1703, 0x0a1c1604,
	0x0b1d1602, 0x0c1d1502, 0x0c1d1502, 0x0d1d1402,
	0x0e1d1401, 0x0e1e1301, 0x0f1e1300, 0x101e1200,

	0x00111e11, 0x00121e10, 0x00131d10, 0x01131d0f,
	0x01141d0e, 0x01141d0e, 0x02151c0d, 0x02151c0d,
	0x03161c0b, 0x03161c0b, 0x04171b0a, 0x04171b0a,
	0x05171b09, 0x05181a09, 0x06181a08, 0x06191a07,
	0x07191907, 0x071a1906, 0x081a1806, 0x081a1806,
	0x091a1805, 0x0a1b1704, 0x0a1b1704, 0x0b1c1603,
	0x0b1c1603, 0x0c1c1503, 0x0d1c1502, 0x0d1d1402,
	0x0e1d1401, 0x0f1d1301, 0x0f1d1301, 0x101e1200,
};

static uintptr_t sun8i_vi_scaler_base(int rtmixid)
{
 return 0;

/*
	if (IS_DE3)
		return DE3_VI_SCALER_UNIT_BASE +
		       DE3_VI_SCALER_UNIT_SIZE * channel;
	else
		return DE2_VI_SCALER_UNIT_BASE +
		       DE2_VI_SCALER_UNIT_SIZE * channel;
*/

}

static int sun8i_vi_scaler_coef_index(unsigned int step)
{
	unsigned int scale, int_part, float_part;

	scale = step >> (SUN8I_VI_SCALER_SCALE_FRAC - 3);
	int_part = scale >> 3;
	float_part = scale & 0x7;

	switch (int_part) {
	case 0:
		return 0;
	case 1:
		return float_part;
	case 2:
		return 8 + (float_part >> 1);
	case 3:
		return 12;
	case 4:
		return 13;
	default:
		return 14;
	}
}

static void sun8i_vi_scaler_set_coeff(int rtmixid, uintptr_t base, uint32_t hstep, uint32_t vstep)
{
	const uint32_t *ch_left, *ch_right, *cy;
	int offset, i;

	if ((HSUB == 1) && (VSUB == 1)) {
		ch_left = lan3coefftab32_left;
		ch_right = lan3coefftab32_right;
		cy = lan2coefftab32;
	} else {
		ch_left = bicubic8coefftab32_left;
		ch_right = bicubic8coefftab32_right;
		cy = bicubic4coefftab32;
	}

	offset = sun8i_vi_scaler_coef_index(hstep) *
			SUN8I_VI_SCALER_COEFF_COUNT;
	for (i = 0; i < SUN8I_VI_SCALER_COEFF_COUNT; i++) {
		regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_YHCOEFF0(base, i),
			     lan3coefftab32_left[offset + i]);
		regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_YHCOEFF1(base, i),
			     lan3coefftab32_right[offset + i]);
		regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_CHCOEFF0(base, i),
			     ch_left[offset + i]);
		regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_CHCOEFF1(base, i),
			     ch_right[offset + i]);
	}

	offset = sun8i_vi_scaler_coef_index(hstep) *
			SUN8I_VI_SCALER_COEFF_COUNT;
	for (i = 0; i < SUN8I_VI_SCALER_COEFF_COUNT; i++) {
		regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_YVCOEFF(base, i),
			     lan2coefftab32[offset + i]);
		regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_CVCOEFF(base, i),
			     cy[offset + i]);
	}
}

static void sun8i_vi_scaler_setup(int rtmixid, uint32_t src_w, uint32_t src_h, uint32_t dst_w, uint32_t dst_h, uint32_t hscale, uint32_t vscale, uint32_t hphase, uint32_t vphase)
{
	uint32_t chphase, cvphase;
	uint32_t insize, outsize;
	uintptr_t base;

	base = sun8i_vi_scaler_base(rtmixid);

	hphase <<= SUN8I_VI_SCALER_PHASE_FRAC - 16;
	vphase <<= SUN8I_VI_SCALER_PHASE_FRAC - 16;
	hscale <<= SUN8I_VI_SCALER_SCALE_FRAC - 16;
	vscale <<= SUN8I_VI_SCALER_SCALE_FRAC - 16;

	insize = SUN8I_VI_SCALER_SIZE(src_w, src_h);
	outsize = SUN8I_VI_SCALER_SIZE(dst_w, dst_h);

	/*
	 * This is chroma V/H phase calculation as it appears in
	 * BSP driver. There is no detailed explanation. YUV 420
	 * chroma is threated specialy for some reason.
	 */
	if ((HSUB == 2) && (VSUB == 2)) {
		chphase = hphase >> 1;
		cvphase = (vphase >> 1) -
			(1UL << (SUN8I_VI_SCALER_SCALE_FRAC - 2));
	} else {
		chphase = hphase;
		cvphase = vphase;
	}

	if (IS_DE3) {
		uint32_t val;

		if ((HSUB == 1) && (VSUB) == 1)
			val = SUN50I_SCALER_VSU_SCALE_MODE_UI;
		else
			val = SUN50I_SCALER_VSU_SCALE_MODE_NORMAL;

		regmap_vsu_write(rtmixid, SUN50I_SCALER_VSU_SCALE_MODE(base), val);

		ASSERT(regmap_vsu_read(rtmixid, SUN50I_SCALER_VSU_SCALE_MODE(base)) == val);
	}

	regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_OUTSIZE(base), outsize);
	ASSERT(regmap_vsu_read(rtmixid, SUN8I_SCALER_VSU_OUTSIZE(base)) == outsize);
	regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_YINSIZE(base), insize);
	regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_YHSTEP(base), hscale);
	regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_YVSTEP(base), vscale);
	regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_YHPHASE(base), hphase);
	regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_YVPHASE(base), vphase);
	regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_CINSIZE(base), SUN8I_VI_SCALER_SIZE(src_w / HSUB, src_h / VSUB));
	regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_CHSTEP(base), hscale / HSUB);
	regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_CVSTEP(base), vscale / VSUB);

	regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_CHPHASE(base), chphase);
	ASSERT(regmap_vsu_read(rtmixid, SUN8I_SCALER_VSU_CHPHASE(base)) == chphase);

	regmap_vsu_write(rtmixid, SUN8I_SCALER_VSU_CVPHASE(base), cvphase);
	//ASSERT(regmap_vsu_read(rtmixid, SUN8I_SCALER_VSU_CVPHASE(base)) == cvphase);

	sun8i_vi_scaler_set_coeff(rtmixid, base, hscale, vscale);
}

static void sun8i_vi_scaler_enable(int rtmixid, uint8_t enable)
{
	uint32_t val;
	uintptr_t base;

	base = sun8i_vi_scaler_base(rtmixid);

	if (enable)
		val = SUN8I_SCALER_VSU_CTRL_EN |
		      SUN8I_SCALER_VSU_CTRL_COEFF_RDY;
	else
		val = 0;

	regmap_vsu_write(rtmixid,
		     SUN8I_SCALER_VSU_CTRL(base), val);
}

/* ********************************* */

static void t113_vi_scaler_setup(int rtmixid, const videomode_t * srcvdmode, const videomode_t * dstvdmode)
{
	if (srcvdmode->width == dstvdmode->width && srcvdmode->height == dstvdmode->height)
	{
		sun8i_vi_scaler_enable(rtmixid, 0);
		return;
	}
	uint32_t src_w = srcvdmode->width;
	uint32_t src_h = srcvdmode->height;

	uint32_t dst_w = dstvdmode->width;//LCD_PIXEL_WIDTH;
	uint32_t dst_h = dstvdmode->height;//LCD_PIXEL_HEIGHT;

	uint32_t hscale = ((uint64_t) src_w << 16) / dst_w;	// расчет в 16 бит точности (далее будет сдвиг)
	uint32_t vscale = ((uint64_t) src_h << 16) / dst_h;	// расчет в 16 бит точности (далее будет сдвиг)

	sun8i_vi_scaler_setup(rtmixid, src_w, src_h, dst_w, dst_h, hscale, vscale, 0, 0);
	sun8i_vi_scaler_enable(rtmixid, 1);
}

// H3: PLL_VIDEO
// A64: PLL_VIDEO0
// T113: PLL_VIDEO0
// T507: PLL_VIDEO0
static void t113_tcontv_PLL_configuration(uint_fast32_t dotclock)
{
#if defined (TCONTV_PTR)
#if CPUSTYLE_H3
	// Set up shared and dedicated clocks for HDMI, LCD/TCON and DE2
	// 297MHz
	CCU->PLL_VIDEO_CTRL_REG =
		(UINT32_C(1) << 31) |	// PLL_ENABLE
		//(UINT32_C(1) << 25) |	// FRAC_CLK_OUT
		(UINT32_C(1) << 24) |	// PLL_MODE_SEL 1: integer mode
		((99 - 1) * (UINT32_C(1) << 8)) |
		((8 - 1) * (UINT32_C(1) << 0)) |
		0;
	while ((CCU->PLL_VIDEO_CTRL_REG & (UINT32_C(1) << 28)) == 0)	 //Wait pll stable
		;
	local_delay_ms(50);

#elif CPUSTYLE_A64

	// 297MHz
	CCU->PLL_VIDEO0_CTRL_REG =
		(UINT32_C(1) << 31) |	// PLL_ENABLE
		//(UINT32_C(1) << 25) |	// FRAC_CLK_OUT
		(UINT32_C(1) << 24) |	// PLL_MODE_SEL = integer
		((99 - 1) * (UINT32_C(1) << 8)) |
		((8 - 1) * (UINT32_C(1) << 0)) |
		0;
	while ((CCU->PLL_VIDEO0_CTRL_REG & (UINT32_C(1) << 28)) == 0)	 //Wait pll stable
		;
	local_delay_ms(50);

#elif CPUSTYLE_T507 || CPUSTYLE_H616

	allwnr_t507_module_pll_spr(& CCU->PLL_VIDEO0_CTRL_REG, & CCU->PLL_VIDEO0_PAT0_CTRL_REG);	// Set Spread Frequency Mode
	allwnr_t507_module_pll_enable(& CCU->PLL_VIDEO0_CTRL_REG, 99);

#elif CPUSTYLE_T113 || CPUSTYLE_F133

	const uint_fast32_t N = 72;     // N=72 => PLL_VIDEO0(4x) = 24*N/M = 24*72/2 = 864 MHz
	const uint_fast32_t M = 4;

	CCU->PLL_VIDEO0_CTRL_REG = 0;
	allwnr_t113_module_pll_spr(& CCU->PLL_VIDEO0_CTRL_REG, & CCU->PLL_VIDEO0_PAT0_CTRL_REG);	// Set Spread Frequency Mode
	//allwnr_t113_module_pll_enable(& CCU->PLL_VIDEO0_CTRL_REG);

	CCU->PLL_VIDEO0_CTRL_REG |=
		1 * (UINT32_C(1) << 31) |	// PLL_EN
		1 * (UINT32_C(1) << 30) |	// PLL_LDO_EN
		1 * (UINT32_C(1) << 27) |	// PLL_OUTPUT_GATE
		(N - 1) * (UINT32_C(1) << 8) |
		(M - 1) * (UINT32_C(1) << 0) |
		0;
#error CHECK M field!!!

	CCU->PLL_VIDEO0_CTRL_REG |= (UINT32_C(1) << 31);

	CCU->PLL_VIDEO0_CTRL_REG |= (UINT32_C(1) << 29);          //Lock enable

	while ((CCU->PLL_VIDEO0_CTRL_REG & (UINT32_C(1) << 28)) == 0)	 //Wait pll stable
		;

	CCU->PLL_VIDEO0_CTRL_REG &= ~ (UINT32_C(1) << 29);         //Lock disable

//	PRINTF("allwnr_t113_get_video0pllx4_freq()=%u MHz\n", (unsigned) (allwnr_t113_get_video0pllx4_freq() / 1000 / 1000));
//	PRINTF("allwnr_t113_get_video1pllx4_freq()=%u MHz\n", (unsigned) (allwnr_t113_get_video1pllx4_freq() / 1000 / 1000));

#else

#endif
#endif /* defined (TCONTV_PTR) */
}

#if WITHHDMITVHW
static uint_fast32_t hdmi_realclock(const videomode_t * vdmode)
{
#if defined (TCONTV_PTR)
#if CPUSTYLE_H3
	return allwnr_h3_get_hdmi_freq();
#elif CPUSTYLE_A64
	return allwnr_a64_get_hdmi_freq();
#elif CPUSTYLE_T507 || CPUSTYLE_H616
	return allwnr_t507_get_hdmi0_freq();
#elif CPUSTYLE_T113 || CPUSTYLE_F133
	return allwnr_t113_get_hdmi_freq();
#endif
#endif /* defined (TCONTV_PTR) */
	return display_getdotclock(vdmode);
}
#endif

static void t113_TCONTV_CCU_configuration(uint_fast32_t dotclock)
{
#if defined (TCONTV_PTR)
#if CPUSTYLE_H3

	CCU->BUS_CLK_GATING_REG1 |= (UINT32_C(1) << 11) | (UINT32_C(1) << 3); // Enable DE, HDMI, TCON0
	CCU->BUS_SOFT_RST_REG1 |= ( UINT32_C(1) << 11) | ( UINT32_C(1) << 10) | (UINT32_C(1) << 3); // De-assert reset of DE, HDMI0/1, TCON0

	unsigned M = calcdivround2(allwnr_h3_get_pll_video_freq(), dotclock);
	CCU->HDMI_CLK_REG = (UINT32_C(1) << 31) | (M - 1); // Enable HDMI clk (use PLL_VIDEO)
	CCU->TCON0_CLK_REG = (UINT32_C(1) << 31) | (M - 1); // (use PLL_VIDEO) Enable TCONLCD_PTR clk, divide by 2
	CCU->HDMI_SLOW_CLK_REG = (UINT32_C(1) << 31); // Enable HDMI slow clk

	//	7 allwnr_h3_get_hdmi_freq()=148500 kHz
	//	7 BOARD_TCONTVFREQ()=148500 kHz
	PRINTF("7 allwnr_h3_get_hdmi_freq()=%u kHz\n", (unsigned) (allwnr_h3_get_hdmi_freq() / 1000));	// 148.5 MHz or 74.25 MHz
	PRINTF("7 BOARD_TCONTVFREQ()=%u kHz\n", (unsigned) (BOARD_TCONTVFREQ / 1000));	// 148.5 MHz

#elif CPUSTYLE_A64

	unsigned TCONTV_DIV = calcdivround2(allwnr_a64_get_pll_video0_x1_freq(), dotclock / 2);	// TCONTV divider
	unsigned HDMI_DIV = calcdivround2(allwnr_a64_get_pll_video0_x1_freq(), dotclock);		// HDMI divider
//	PRINTF("MX2=%u\n", TCONTV_DIV);
//	PRINTF("MX1=%u\n", HDMI_DIV);

	CCU->BUS_CLK_GATING_REG1 |= (UINT32_C(1) << 11); // Enable HDMI
	CCU->BUS_SOFT_RST_REG1 |= (UINT32_C(1) << 11) | (UINT32_C(1) << 10); // De-assert reset of HDMI0/1 - требуюия оба

	CCU->BUS_CLK_GATING_REG1 |= (UINT32_C(1) << (3 + TCONTV_IX));	// TCONx_GATING
	CCU->BUS_SOFT_RST_REG1 |= (UINT32_C(1) << (3 + TCONTV_IX));	// TCONx_RST De-assert

	if (TCONTV_IX == 0)
	{
		// Делителя нет
		CCU->TCON0_CLK_REG = 0;
		CCU->TCON0_CLK_REG = (CCU->TCON0_CLK_REG & ~ (UINT32_C(0x07) << 24)) |
			0x02 * (UINT32_C(1) << 24) | // 000: PLL_MIPI, 010: PLL_VIDEO0(2X)
			0;
		CCU->TCON0_CLK_REG |= UINT32_C(1) << 31;	// SCLK_GATING
	}
	else if (TCONTV_IX == 1)
	{
		CCU->TCON1_CLK_REG = 0;
		CCU->TCON1_CLK_REG = (CCU->TCON1_CLK_REG & ~ (UINT32_C(0x07) << 24)) |
			0x00 * (UINT32_C(1) << 24) | // 00: PLL_VIDEO0(1X), 10: PLL_VIDEO1(1X)
			(TCONTV_DIV - 1) * (UINT32_C(1) << 0) | // dvcider / 2
			0;
		CCU->TCON1_CLK_REG |= UINT32_C(1) << 31;	// SCLK_GATING
	}


	CCU->HDMI_CLK_REG = 0x00 * (UINT32_C(1) << 24) | (HDMI_DIV - 1); // Enable HDMI clk 00: PLL_VIDEO0(1X), 01: PLL_VIDEO1(1X)
	CCU->HDMI_CLK_REG |= (UINT32_C(1) << 31); // Enable HDMI clk

	CCU->HDMI_SLOW_CLK_REG = (UINT32_C(1) << 31); // Enable HDMI slow clk

	//	7 allwnr_a64_get_hdmi_freq()=148500 kHz
	//	7 BOARD_TCONTVFREQ()=74250 kHz
//	PRINTF("7 allwnr_a64_get_hdmi_freq()=%u kHz\n", (unsigned) (allwnr_a64_get_hdmi_freq() / 1000));	// 148.5 MHz or 74.25 MHz
//	PRINTF("7 BOARD_TCONTVFREQ()=%u kHz\n", (unsigned) (BOARD_TCONTVFREQ / 1000));	// 74.25 MHz

#elif CPUSTYLE_T507 || CPUSTYLE_H616

	unsigned ix = TCONTV_IX;

	//	Note: Before operating ADDA/GPADC/RES_CAL/CSI/DSI/LVDS/HDMI (only
	//	for T507-H/T517-H)/TVOUT modules, please make sure that this bit is
	//	configured as 1
    PRCM->VDD_SYS_PWROFF_GATING_REG |= (UINT32_C(1) << 4); // ANA_VDDON_GATING
    local_delay_ms(10);
    //PRINTF("PRCM->VDD_SYS_PWROFF_GATING_REG=%08X\n", (unsigned) PRCM->VDD_SYS_PWROFF_GATING_REG);

 	CCU->DISPLAY_IF_TOP_BGR_REG |= (UINT32_C(1) << 0);	// DISPLAY_IF_TOP_GATING
	CCU->DISPLAY_IF_TOP_BGR_REG |= (UINT32_C(1) << 16);	// DISPLAY_IF_TOP_RST De-assert writable mask 0x00010001

    DISP_IF_TOP->MODULE_GATING |= (UINT32_C(1) << (20 + ix));	//  TV0_GATE, TV1_GATE
	DISP_IF_TOP->MODULE_GATING |= (UINT32_C(1) << 28);	// TV0_HDMI_GATE ???? may be not need
	//PRINTF("DISP_IF_TOP->MODULE_GATING=%08X\n", (unsigned) DISP_IF_TOP->MODULE_GATING);

	// PLL_VIDEO0 as source
	const uint_fast32_t pllout = allwnr_t507_get_pll_video0_x1_freq();
	unsigned M_HDMI = ulmax(1, ulmin(calcdivround2(pllout, dotclock), 16));
	//PRINTF("7 dotclock=%u kHz\n", (unsigned) (dotclock / 1000));
	//PRINTF("7 M_HDMI=%u\n", M_HDMI);

    // CCU_32K select as CEC clock as default
    // https://github.com/intel/mOS/blob/f67dfb38e6805f01ab96387597b24d4e3c285562/drivers/clk/sunxi-ng/ccu-sun50i-h616.c#L1135
	unsigned tcontv_divider;
	unsigned tcontv_prei = calcdivider(calcdivround2(allwnr_t507_get_pll_video0_x1_freq(), dotclock), 4, (8 | 4 | 2 | 1), & tcontv_divider, 1);

	TCONTV_CCU_CLK_REG =
			0x00 * (UINT32_C(1) << 24) | 	// 000: PLL_VIDEO0(1X) 001: PLL_VIDEO0(4X)
			(tcontv_prei) * (UINT32_C(1) << 8) | // prescaler code
			(tcontv_divider) * (UINT32_C(1) << 0) |
			0;
	TCONTV_CCU_CLK_REG |= UINT32_C(1) << 31;	// SCLK_GATING
    //PRINTF("TCONTV_CCU_CLK_REG=%08X\n", (unsigned) TCONTV_CCU_CLK_REG);

	const unsigned HDMI_CLK_REG_M = M_HDMI;
	CCU->HDMI0_CLK_REG = 0x00 * (UINT32_C(1) << 24) | (HDMI_CLK_REG_M - 1);	// 00: PLL_VIDEO0(1X)
    CCU->HDMI0_CLK_REG |= (UINT32_C(1) << 31);
    //PRINTF("CCU->HDMI0_CLK_REG=%08X\n", (unsigned) CCU->HDMI0_CLK_REG);

    CCU->HDMI_BGR_REG |= (UINT32_C(1) << 0);	// HDMI0_GATING
    CCU->HDMI_BGR_REG |= (UINT32_C(1) << 17) | (UINT32_C(1) << 16);	// HDMI0_SUB_RST HDMI0_MAIN_RST (19 & 18 - hdmi1 ?)
    //PRINTF("CCU->HDMI_BGR_REG=%08X\n", (unsigned) CCU->HDMI_BGR_REG);

    CCU->HDMI0_SLOW_CLK_REG |= (UINT32_C(1) << 31);
    //PRINTF("CCU->HDMI0_SLOW_CLK_REG=%08X\n", (unsigned) CCU->HDMI0_SLOW_CLK_REG);

	// HDCP: High-bandwidth Digital Content Protection
    CCU->HDMI_HDCP_CLK_REG = (UINT32_C(1) << 31) | 0x00 * (UINT32_C(1) << 24) | (2 - 1);	// SCLK_GATING
    //CCU->HDMI_HDCP_CLK_REG = 0x81000001;
    //PRINTF("CCU->HDMI_HDCP_CLK_REG=%08X\n", (unsigned) CCU->HDMI_HDCP_CLK_REG);

    CCU->HDMI_HDCP_BGR_REG |= (UINT32_C(1) << 0);	// HDMI_HDCP_GATING
    CCU->HDMI_HDCP_BGR_REG |= (UINT32_C(1) << 16);	// HDMI_HDCP_RST
    //PRINTF("CCU->HDMI_HDCP_BGR_REG=%08X\n", (unsigned) CCU->HDMI_HDCP_BGR_REG);

	CCU->TCON_TV_BGR_REG |= (UINT32_C(1) << (0 + ix));	// Clock Gating
	//CCU->TCON_TV_BGR_REG &= ~ (UINT32_C(1) << (16 + ix));	// Assert Reset
	CCU->TCON_TV_BGR_REG |= (UINT32_C(1) << (16 + ix));	// De-assert Reset (bits 19..16 and 3..0 writable) mask 0x000F000F

	/*
	 * First clock parent (osc32K) is unusable for CEC. But since there
	 * is no good way to force parent switch (both run with same frequency),
	 * just set second clock parent here.
	 */
	CCU->HDMI_CEC_CLK_REG = 0x01 * (UINT32_C(1) << 24);

    CCU->HDMI_CEC_CLK_REG |= (UINT32_C(1) << 31);	// SCLK_GATING
    CCU->HDMI_CEC_CLK_REG |= (UINT32_C(1) << 30);	// PLL_PERI_GATING
    //PRINTF("CCU->HDMI_CEC_CLK_REG=%08X\n", (unsigned) CCU->HDMI_CEC_CLK_REG);

//    CCU->TVE0_CLK_REG = 0x82000001;
//    CCU->TVE_BGR_REG = 0x00030003;

    local_delay_us(10);

// https://github.com/MYIR-ALLWINNER/myir-t5-kernel/blob/a7089355dd727f5aaedade642f5fbc5b354b215a/drivers/video/fbdev/sunxi/disp2/disp/de/lowlevel_v3x/de_lcd_type.h
//  0x200
//	union tcon_mux_ctl_reg_t {
//		u32 dwval;
//		struct {
//			u32 dsi_src:2;
//			u32 res0:6;
//			u32 hdmi_src:2;
//			u32 res1:22;
//		} bits;
//	};

	//	7 allwnr_t507_get_hdmi_hdcp_freq()=300000 kHz
	//	7 allwnr_t507_get_hdmi_freq()=297000 kHz
	//	7 BOARD_TCONLCDFREQ()=148500 kHz

	//PRINTF("7 allwnr_t507_get_hdmi_hdcp_freq()=%u kHz\n", (unsigned) (allwnr_t507_get_hdmi_hdcp_freq() / 1000));
	//PRINTF("7 allwnr_t507_get_hdmi_freq()=%u kHz\n", (unsigned) (allwnr_t507_get_hdmi0_freq() / 1000));
	//PRINTF("7 BOARD_TCONTVFREQ()=%u kHz\n", (unsigned) (BOARD_TCONTVFREQ / 1000));

#elif CPUSTYLE_T113 || CPUSTYLE_F133

    // DISPLAY_TOP access
	CCU->DPSS_TOP_BGR_REG |= (UINT32_C(1) << 0);	// DPSS_TOP_GATING Open the clock gate
	CCU->DPSS_TOP_BGR_REG |= (UINT32_C(1) << 16);	// DPSS_TOP_RST De-assert reset

	DISPLAY_TOP->TV_CLK_SRC_RGB_SRC &= ~ (UINT32_C(1));	// 0 - CCU clock, 1 - TVE clock
	DISPLAY_TOP->MODULE_GATING |= (UINT32_C(1) << 20); // enable clk for TCON_TV0

#endif
#endif /* defined (TCONTV_PTR) */
}

#if WITHDSIHW

#endif /* WITHDSIHW */

static void hardware_de_initialize(const videomode_t * vdmode, int rtmixid)
{
#if CPUSTYLE_A64

	// https://github.com/bigtreetech/CB1-Kernel/blob/244c0fd1a2a8e7f2748b2a9ae3a84b8670465351/u-boot/drivers/video/sunxi/sunxi_de2.c#L128

	/* переключить память к DE & VI */
    // https://github.com/bigtreetech/CB1-Kernel/blob/244c0fd1a2a8e7f2748b2a9ae3a84b8670465351/u-boot/drivers/video/sunxi/sunxi_de2.c#L39
	//#define SUNXI_SRAMC_BASE 0x03000000
    // Under CONFIG_MACH_SUN50I
	/* переключить память к DE & VI */
    // https://github.com/bigtreetech/CB1-Kernel/blob/244c0fd1a2a8e7f2748b2a9ae3a84b8670465351/u-boot/drivers/video/sunxi/sunxi_de2.c#L39
	SYS_CFG->MEMMAP_REG &= ~ (UINT32_C(1) << 24);

	// Set up shared and dedicated clocks for HDMI, LCD/TCON and DE2
	CCU->PLL_DE_CTRL_REG = (UINT32_C(1) << 31) | (UINT32_C(1) << 24) | ((18 - 1) * (UINT32_C(1) << 8)) | ((1 - 1) * (UINT32_C(1) << 0)); // 432MHz
	local_delay_ms(50);

	CCU->BUS_CLK_GATING_REG1 |= (UINT32_C(1) << 12); // Enable DE, HDMI, TCON0/1
	CCU->BUS_SOFT_RST_REG1 |= (UINT32_C(1) << 12); // De-assert reset of DE, HDMI0/1, TCON0/1
	const unsigned DE_CLK_DIV_RATIO_M = 2;
	CCU->DE_CLK_REG = (UINT32_C(1) << 31) | (UINT32_C(1) << 24) | (DE_CLK_DIV_RATIO_M - 1) * (UINT32_C(1) << 0); // Enable DE clock, set source to PLL_DE

    CCU->BUS_CLK_GATING_REG1 |= (UINT32_C(1) << 12);	// DE_GATING
    CCU->BUS_SOFT_RST_REG1 &= ~ (UINT32_C(1) << 12);	// DE_RST
    CCU->BUS_SOFT_RST_REG1 |= (UINT32_C(1) << 12);	// DE_RST

	//PRINTF("2 allwnr_a64_get_de_freq()=%u MHz\n", (unsigned) (allwnr_a64_get_de_freq() / 1000 / 1000));
    //PRINTF("allwnr_a64_get_mbus_freq()=%" PRIuFAST32 " kHz\n", allwnr_a64_get_mbus_freq() / 1000);
    local_delay_us(10);
 	/* Global DE settings */

	// https://github.com/BPI-SINOVOIP/BPI-M2U-bsp/blob/2adcf0fe39e54b9bcacbd5bcd3ecb6077e081122/linux-sunxi/drivers/video/sunxi/disp2/disp/de/lowlevel_v3x/de_clock.c#L91
//
// 	DE_TOP->DE_SCLK_DIV =
//		7 * (UINT32_C(1) << 8) |	// wb-div
//		7 * (UINT32_C(1) << 4) |	// mixer1-div
//		7 * (UINT32_C(1) << 0) |	// mixer0-div
//		0;

    {
	 	DE_TOP->SCLK_GATE |= ~0u; //UINT32_C(1) << 0;	// CORE0_SCLK_GATE
	 	DE_TOP->SCLK_GATE |= ~0u; //UINT32_C(1) << 1;	// CORE1_SCLK_GATE
	 	DE_TOP->HCLK_GATE |= ~0u; //UINT32_C(1) << 0;	// CORE0_HCLK_GATE
	 	DE_TOP->HCLK_GATE |= ~0u; //UINT32_C(1) << 1;	// CORE1_HCLK_GATE

	 	DE_TOP->AHB_RESET = 0;	// All cores reset
		DE_TOP->AHB_RESET |= ~0u; //(UINT32_C(1) << 0);		// CORE0_AHB_RESET
		DE_TOP->AHB_RESET |= ~0u; //(UINT32_C(1) << 1);		// CORE1_AHB_RESET

		// All registers are 0xFF
//		PRINTF("DE_TOP->AHB_RESET=%08X\n", (unsigned) DE_TOP->AHB_RESET);
//		PRINTF("DE_TOP->SCLK_GATE=%08X\n", (unsigned) DE_TOP->SCLK_GATE);
//		PRINTF("DE_TOP->HCLK_GATE=%08X\n", (unsigned) DE_TOP->HCLK_GATE);
    }

#elif CPUSTYLE_T507 || CPUSTYLE_H616

	// https://github.com/bigtreetech/CB1-Kernel/blob/244c0fd1a2a8e7f2748b2a9ae3a84b8670465351/u-boot/drivers/video/sunxi/sunxi_de2.c#L128

	/* переключить память к DE & VI */
    // https://github.com/bigtreetech/CB1-Kernel/blob/244c0fd1a2a8e7f2748b2a9ae3a84b8670465351/u-boot/drivers/video/sunxi/sunxi_de2.c#L39
	SYS_CFG->MEMMAP_REG &= ~ (UINT32_C(1) << 24);

//	allwnr_t507_module_pll_spr(& CCU->PLL_DE_CTRL_REG, & CCU->PLL_DE_PAT0_CTRL_REG);	// Set Spread Frequency Mode
//	allwnr_t507_module_pll_enable(& CCU->PLL_DE_CTRL_REG, 36);

	/* Configure DE clock (no FACTOR_N on T507/H616 CPU) */
	//	CLK_SRC_SEL.
	//	Clock Source Select
	//	0: PLL_DE
	//	1: PLL_PERI0(2X)
	unsigned divider = 4;
    CCU->DE_CLK_REG = (CCU->DE_CLK_REG & ~ (UINT32_C(1) << 24) & ~ (UINT32_C(0x0F) << 0)) |
		0x01 * (UINT32_C(1) << 24) |	// CLK_SRC_SEL 0: PLL_DE 1: PLL_PERI0(2X)
		(divider - 1) * (UINT32_C(1) << 0) |	// FACTOR_M 300 MHz
		0;
    CCU->DE_CLK_REG |= (UINT32_C(1) << 31);	// SCLK_GATING

    local_delay_us(10);

	//PRINTF("allwnr_t507_get_de_freq()=%" PRIuFAST32 " MHz\n", allwnr_t507_get_de_freq() / 1000 / 1000);
	//PRINTF("allwnr_t507_get_mbus_freq()=%" PRIuFAST32 " MHz\n", allwnr_t507_get_mbus_freq() / 1000 / 1000);

    CCU->DE_BGR_REG |= (UINT32_C(1) << 0);		// Open the clock gate
    CCU->DE_BGR_REG |= (UINT32_C(1) << 16);		// De-assert reset
    local_delay_us(10);

 	/* Global DE settings */
    const int disp = rtmixid - 1;

	// https://github.com/BPI-SINOVOIP/BPI-M2U-bsp/blob/2adcf0fe39e54b9bcacbd5bcd3ecb6077e081122/linux-sunxi/drivers/video/sunxi/disp2/disp/de/lowlevel_v3x/de_clock.c#L91
	// https://github.com/rvboards/linux_kernel_for_d1/blob/5703a18aa3ca12829027b0b20cd197e9741c4c0f/drivers/video/fbdev/sunxi/disp2/disp/de/lowlevel_v33x/de330/de_top.c#L245
	// CORE0..CORE3 bits valid - valid bits 0x01F

 	DE_TOP->DE_SCLK_GATE |= UINT32_C(1) << disp;	// COREx_SCLK_GATE
 	DE_TOP->DE_HCLK_GATE |= UINT32_C(1) << disp;	// COREx_HCLK_GATE
 	// Only one bit writable
 	//DE_TOP->DE_AHB_RESET &= ~ (UINT32_C(1) << 0);	// CORE0_AHB_RESET
	DE_TOP->DE_AHB_RESET |= (UINT32_C(1) << 0);		// CORE0_AHB_RESET

#elif CPUSTYLE_T113 || CPUSTYLE_F133
	// PLL_VIDEO1 may be used for LVDS synchronization
	/* Configure DE clock (no FACTOR_N on this CPU) */
	unsigned divider = 4;
	ASSERT(divider >= 1 && divider <= 31);

	//	CLK_SRC_SEL
	//	Clock Source Select
	//	000: PLL_PERI(2X)
	//	001: PLL_VIDEO0(4X)
	//	010: PLL_VIDEO1(4X)
	//	011: PLL_AUDIO1(DIV2)

	CCU->DE_CLK_REG = (CCU->DE_CLK_REG & ~ ((UINT32_C(7) << 24) | (UINT32_C(0x1F) << 0))) |
		0 * (UINT32_C(1) << 24) |	// CLK_SRC_SEL 001: 000: PLL_PERI(2X)
		(divider - 1) * (UINT32_C(1) << 0) |	// FACTOR_M 300 MHz
		0;
    CCU->DE_CLK_REG |= (UINT32_C(1) << 31);	// SCLK_GATING
    local_delay_us(10);
	//PRINTF("allwnr_t113_get_de_freq()=%" PRIuFAST32 " MHz\n", allwnr_t113_get_de_freq() / 1000 / 1000);

    CCU->DE_BGR_REG |= (UINT32_C(1) << 0);		// Open the clock gate
    CCU->DE_BGR_REG |= (UINT32_C(1) << 16);		// De-assert reset
    local_delay_us(10);


	/* Global DE settings */
//	PRINTF("DE_TOP before:\n");
//	printhex32(DE_TOP_BASE, DE_TOP, 0x160);

#if 0
	//PRINTF("DE_TOP after:\n");
	//printhex32(DE_TOP_BASE, DE_TOP, 0x160);
	//    DE_IP_CFG=01020905
	//    DE_IP_CFG.RTD1_DI_NO=0
	//    DE_IP_CFG.RTD1_UI_NO=0
	//    DE_IP_CFG.RTD1_VIDEO_NO=1
	//    DE_IP_CFG.RTD1_FBD_NO=0
	//    DE_IP_CFG.RTD1_DNS_NO=0
	//    DE_IP_CFG.RTD1_VEP_NO=1
	//    DE_IP_CFG.RTD1_DEP_NO=0
	//    DE_IP_CFG.RTD0_DI_NO=0
	//    DE_IP_CFG.RTD0_UI_NO=1
	//    DE_IP_CFG.RTD0_VIDEO_NO=1
	//    DE_IP_CFG.RTD0_FBD_NO=0
	//    DE_IP_CFG.RTD0_DNS_NO=0
	//    DE_IP_CFG.RTD0_VEP_NO=2
	//    DE_IP_CFG.RTD0_DEP_NO=1

	PRINTF("DE_IP_CFG=%08X\n", (unsigned) DE_TOP->DE_IP_CFG);

	PRINTF("DE_IP_CFG.RTD1_DI_NO=%u\n", (unsigned) (DE_TOP->DE_IP_CFG >> 30) & 0x03);
	PRINTF("DE_IP_CFG.RTD1_UI_NO=%u\n", (unsigned) (DE_TOP->DE_IP_CFG >> 27) & 0x07);
	PRINTF("DE_IP_CFG.RTD1_VIDEO_NO=%u\n", (unsigned) (DE_TOP->DE_IP_CFG >> 24) & 0x07);
	PRINTF("DE_IP_CFG.RTD1_FBD_NO=%u\n", (unsigned) (DE_TOP->DE_IP_CFG >> 21) & 0x07);
	PRINTF("DE_IP_CFG.RTD1_DNS_NO=%u\n", (unsigned) (DE_TOP->DE_IP_CFG >> 19) & 0x03);
	PRINTF("DE_IP_CFG.RTD1_VEP_NO=%u\n", (unsigned) (DE_TOP->DE_IP_CFG >> 17) & 0x03);
	PRINTF("DE_IP_CFG.RTD1_DEP_NO=%u\n", (unsigned) (DE_TOP->DE_IP_CFG >> 16) & 0x01);

	PRINTF("DE_IP_CFG.RTD0_DI_NO=%u\n", (unsigned) (DE_TOP->DE_IP_CFG >> 14) & 0x03);
	PRINTF("DE_IP_CFG.RTD0_UI_NO=%u\n", (unsigned) (DE_TOP->DE_IP_CFG >> 11) & 0x07);
	PRINTF("DE_IP_CFG.RTD0_VIDEO_NO=%u\n", (unsigned) (DE_TOP->DE_IP_CFG >> 8) & 0x07);
	PRINTF("DE_IP_CFG.RTD0_FBD_NO=%u\n", (unsigned) (DE_TOP->DE_IP_CFG >> 5) & 0x07);
	PRINTF("DE_IP_CFG.RTD0_DNS_NO=%u\n", (unsigned) (DE_TOP->DE_IP_CFG >> 3) & 0x03);
	PRINTF("DE_IP_CFG.RTD0_VEP_NO=%u\n", (unsigned) (DE_TOP->DE_IP_CFG >> 1) & 0x03);
	PRINTF("DE_IP_CFG.RTD0_DEP_NO=%u\n", (unsigned) (DE_TOP->DE_IP_CFG >> 0) & 0x01);
#endif

	{
		/* Эта часть - как и разрешение тактирования RT Mixer 0 - должна присутствовать для работы RT Mixer 1 */
		const int rtmixid = 1;
		// Enable RT-Mixer 0
		DE_TOP->GATE_CFG |= UINT32_C(1) << 0;
		DE_TOP->BUS_CFG |= UINT32_C(1) << 0;
		DE_TOP->RST_CFG &= ~ (UINT32_C(1) << 0);
		DE_TOP->RST_CFG |= UINT32_C(1) << 0;
	}
    {
    	const int rtmixid = 2;
       // Enable RT-Mixer 1
    	DE_TOP->GATE_CFG |= UINT32_C(1) << 1;
    	DE_TOP->BUS_CFG |= UINT32_C(1) << 1;
    	DE_TOP->RST_CFG &= ~ (UINT32_C(1) << 1);
    	DE_TOP->RST_CFG |= UINT32_C(1) << 1;
    }

#elif CPUSTYLE_H3

 	// Set up shared and dedicated clocks for HDMI, LCD/TCON and DE2
	CCU->PLL_DE_CTRL_REG = (UINT32_C(1) << 31) | (UINT32_C(1) << 24) | ((18 - 1) * (UINT32_C(1) << 8)) | ((1 - 1) * (UINT32_C(1) << 0)); // 432MHz
	local_delay_ms(50);

    // clocks

	CCU->BUS_CLK_GATING_REG1 |= (UINT32_C(1) << 12); // Enable DE
	CCU->BUS_SOFT_RST_REG1 |= (UINT32_C(1) << 12); // De-assert reset of DE
	CCU->DE_CLK_REG = (UINT32_C(1) << 31) | (UINT32_C(1) << 24); // Enable DE clock, set source to PLL_DE

	// надо разбираться с битами включения. В регистрах задействованы 8 младших бит.
	// Вероятно без 0-го канала DE не работает 1-й
	{
		const int pos = 1 * 2;
		DE_TOP->SCLK_GATE |= (UINT32_C(3) << pos);
		DE_TOP->HCLK_GATE |= (UINT32_C(3) << pos);
		DE_TOP->AHB_RESET &= ~ (UINT32_C(3) << pos);
		DE_TOP->AHB_RESET |= (UINT32_C(3) << pos);
	}
	{
		const int pos = 0 * 2;
		DE_TOP->SCLK_GATE |= (UINT32_C(3) << pos);
		DE_TOP->HCLK_GATE |= (UINT32_C(3) << pos);
		DE_TOP->AHB_RESET &= ~ (UINT32_C(3) << pos);
		DE_TOP->AHB_RESET |= (UINT32_C(3) << pos);
	}

//	PRINTF("DE_TOP->AHB_RESET=%08X\n", (unsigned) DE_TOP->AHB_RESET);
//	PRINTF("DE_TOP->SCLK_GATE=%08X\n", (unsigned) DE_TOP->SCLK_GATE);
//	PRINTF("DE_TOP->HCLK_GATE=%08X\n", (unsigned) DE_TOP->HCLK_GATE);


#else
	#error Undefined CPUSTYLE_xxx
#endif
}

static void awxx_deoutmapping(void)
{
#if CPUSTYLE_A64
	// Only bit 0 valid
	//PRINTF("1 DE_TOP->DE2TCON_MUX=%08X\n", (unsigned) DE_TOP->DE2TCON_MUX);
	DE_TOP->DE2TCON_MUX = 0;	// TCON0 connect to LVDS, TCON1 connect to HDMI
	//DE_TOP->DE2TCON_MUX = (RTMIXIDTV == 1) * (UINT32_C(1) << 0);	//
	//PRINTF("3 DE_TOP->DE2TCON_MUX=%08X\n", (unsigned) DE_TOP->DE2TCON_MUX);

#elif CPUSTYLE_H3
	// This function configured DE2 as follows:
	// MIXER0 -> WB -> MIXER1 -> HDMI
	//DE_TOP->DE2TCON_MUX = (DE_TOP->DE2TCON_MUX & ~ (UINT32_C(1) << 0)) | !! disp * (UINT32_C(1) << 0);
	DE_TOP->DE2TCON_MUX = 0x01;
	PRINTF("3 DE_TOP->DE2TCON_MUX=%08X\n", (unsigned) DE_TOP->DE2TCON_MUX);

#elif CPUSTYLE_T507 || CPUSTYLE_H616
	// DE_PORT2CHN_MUX [0]=9x00A98210
	// bits 3:0 - BLD_EN_COLOR_CTL bit 8 (pipe0)
	// bits 7:4 - BLD_EN_COLOR_CTL bit 9 (pipe1)
	// ...
	// каждлая четверка битов в DE_PORT2CHN_MUX говорит, какому из битов-источников в
	// bld->BLD_EN_COLOR_CTL соответствует оверлей. Номера оверлеев начиная с 0 - VI, с 8 - UI

	//de_rtmx_set_chn_mux(disp);

	// T507, H616
	//	RTMIX0: VI1, UI1
	//	RTMIX1: VI2, UI2
	// каждлая четверка битов в DE_PORT2CHN_MUX говорит, какому из битов-источников в
	// bld->BLD_EN_COLOR_CTL соответствует оверлей. Номера оверлеев начиная с 0 - VI, с 8 - UI
	{
		DE_TOP->DE_PORT2CHN_MUX [0] =
			0x00 * (UINT32_C(1) << (4 * 0)) | 	// VI1
			0x08 * (UINT32_C(1) << (4 * 1)) | 	// UI1
			0;
		DE_TOP->DE_PORT2CHN_MUX [1] =
			0x01 * (UINT32_C(1) << (4 * 0)) | 	// VI2
			0x09 * (UINT32_C(1) << (4 * 1)) | 	// UI2
			0;

		DE_TOP->DE_CHN2CORE_MUX =
				0x00 * (UINT32_C(1) << (2 * 0x00)) | 	// VI1 - CORE0
				0x00 * (UINT32_C(1) << (2 * 0x08)) | 	// UI1 - CORE0
				0x01 * (UINT32_C(1) << (2 * 0x01)) | 	// VI2 - CORE1
				0x01 * (UINT32_C(1) << (2 * 0x09)) | 	// UI2 - CORE1
				0;

//		PRINTF("DE_PORT2CHN_MUX[0]=%08" PRIX32 "\n", DE_TOP->DE_PORT2CHN_MUX [0]);
//		PRINTF("DE_PORT2CHN_MUX[1]=%08" PRIX32 "\n", DE_TOP->DE_PORT2CHN_MUX [1]);
//		PRINTF("DE_CHN2CORE_MUX=%08" PRIX32 "\n", DE_TOP->DE_CHN2CORE_MUX);
	}

	/* перенаправление выхода DE */
	// 0x000000E4 initial value
//	PRINTF("1 DE_TOP->DE2TCON_MUX=%08X\n", (unsigned) DE_TOP->DE2TCON_MUX);
	// На один TCON может быть направлен только один DE даже если второй выключен)
//	DE_TOP->DE2TCON_MUX = SET_BITS(0 * 2, 2, DE_TOP->DE2TCON_MUX, 3);
//	DE_TOP->DE2TCON_MUX = SET_BITS(1 * 2, 2, DE_TOP->DE2TCON_MUX, 3);
//	DE_TOP->DE2TCON_MUX = SET_BITS(2 * 2, 2, DE_TOP->DE2TCON_MUX, 3);
//	DE_TOP->DE2TCON_MUX = SET_BITS(3 * 2, 2, DE_TOP->DE2TCON_MUX, 3);

//	DE_TOP->DE2TCON_MUX = SET_BITS(disp * 2, 2, DE_TOP->DE2TCON_MUX, targetix);

	// Target codes (unconfirmed)
	// de_top_set_de2tcon_mux
	//
	// Undocumented (2 bits)
	enum
	{
		TG_DE2TCONLCD0 = 0,
		TG_DE2TCONLCD1,
		TG_DE2TCONTV0,
		TG_DE2TCONTV1,
	};
	// Documented (4 bits)
	enum
	{
		TG_DE_PORT_PERH_TCONLCD0 = 0,
		TG_DE_PORT_PERH_TCONLCD1,
		TG_DE_PORT_PERH_TCONTV0,
		TG_DE_PORT_PERH_TCONTV1,
	};

	// Documented
//    DISP_IF_TOP->DE_PORT_PERH_SEL = (DISP_IF_TOP->DE_PORT_PERH_SEL & ~ (UINT32_C(0x0F) << 4) & ~ (UINT32_C(0x0F) << 0)) |
//		TG_DE_PORT_PERH_TCONLCD0 * (UINT32_C(1) << 0) | // DE_PORT0_PERIPH_SEL: TCON_LCD0
//		TG_DE_PORT_PERH_TCONTV0 * (UINT32_C(1) << 4) | // DE_PORT1_PERIPH_SEL: TCON_TV0
//		0;

//	PRINTF("3 DE_TOP->DE2TCON_MUX=%08X\n", (unsigned) DE_TOP->DE2TCON_MUX);
//	PRINTF("3 DISP_IF_TOP->DE_PORT_PERH_SEL=%08X\n", (unsigned) DISP_IF_TOP->DE_PORT_PERH_SEL);
	// +++++++++++++++++ ПРОВЕРЕНО ++++++++++++++++
	// DE_PORT1->TCON_TV0, DE_PORT0->TCON_LCD0
	// Для работы LVDS на RTMIX0 и TV0->HDMI на RTMIX1
	DISP_IF_TOP->DE_PORT_PERH_SEL = 0x00000020;
	DE_TOP->DE2TCON_MUX = 0x000000E4;
	// ----------------- ПРОВЕРЕНО ----------------

//	PRINTF("3 DE_TOP->DE2TCON_MUX=%08X\n", (unsigned) DE_TOP->DE2TCON_MUX);
//	PRINTF("3 DISP_IF_TOP->DE_PORT_PERH_SEL=%08X\n", (unsigned) DISP_IF_TOP->DE_PORT_PERH_SEL);

	// Undocumented
//	DE_TOP->DE2TCON_MUX =
//		TG_DE2TCONTV1 * (UINT32_C(1) << (3 * 2)) |		/* CORE3 output */
//		TG_DE2TCONLCD1 * (UINT32_C(1) << (2 * 2)) |		/* CORE2 output */
//		TG_DE2TCONTV0 * (UINT32_C(1) << (1 * 2)) |		/* CORE1 output */
//		TG_DE2TCONLCD0 * (UINT32_C(1) << (0 * 2)) |		/* CORE0 output */
//		0;
//	PRINTF("3 DE_TOP->DE2TCON_MUX=%08X\n", (unsigned) DE_TOP->DE2TCON_MUX);
//	PRINTF("3 DISP_IF_TOP->DE_PORT_PERH_SEL=%08X\n", (unsigned) DISP_IF_TOP->DE_PORT_PERH_SEL);

// https://github.com/MYIR-ALLWINNER/myir-t5-kernel/blob/a7089355dd727f5aaedade642f5fbc5b354b215a/drivers/video/fbdev/sunxi/disp2/disp/de/lowlevel_v3x/de_lcd_type.h
//  0x200
//	union tcon_mux_ctl_reg_t {
//		u32 dwval;
//		struct {
//			u32 dsi_src:2;
//			u32 res0:6;
//			u32 hdmi_src:2;
//			u32 res1:22;
//		} bits;
//	};

//	TCONTV_PTR->tcon_mul_ctl = (TCONTV_PTR->tcon_mul_ctl & ~ 0x300) |
//			0x01 * (UINT32_C(1) << 8);
//	PRINTF("3 TCONTV_PTR->tcon_mul_ctl=%08X\n", (unsigned) TCONTV_PTR->tcon_mul_ctl);
//	memset(TCONTV_PTR, 0xFF, sizeof * TCONTV_PTR);
//	printhex(TCONTV_PTR, TCONTV_PTR, sizeof * TCONTV_PTR);

#elif CPUSTYLE_T113 || CPUSTYLE_F133

//	/* перенаправление выхода DE */
//	DE_TOP->SEL_CFG = SET_BITS(0, 1, DE_TOP->SEL_CFG, !! disp);	/* MIXER0->TCON1; MIXER1->TCON0 */
//	//DE_TOP->SEL_CFG=0x00000001;
//
//	//PRINTF("DE_TOP->SEL_CFG=%08X\n", (unsigned) DE_TOP->SEL_CFG);
//	{
//		// Undocumented registers
//
//		//PRINTF("Before: DISPLAY_TOP->DE_PORT_PERH_SEL=%08X\n", (unsigned) DISPLAY_TOP->DE_PORT_PERH_SEL);
//		uint32_t v= DISPLAY_TOP->DE_PORT_PERH_SEL;
//		v&=0xFFFFFFF0;
//		v|=0x00000002;	        //0 - DE to TCON_LCD, 2 - DE to TCON_TV
//		v = 0;//0x00010;
//		//DISPLAY_TOP->DE_PORT_PERH_SEL = v;
//		//PRINTF("After: DISPLAY_TOP->DE_PORT_PERH_SEL=%08X\n", (unsigned) DISPLAY_TOP->DE_PORT_PERH_SEL);
//	}

#else
	#error Undefined CPUSTYLE_xxx
#endif
}


void hardware_edid_test(void)
{
#if WITHHDMITVHW
	t507_hdmi_edid_test();
#endif /* WITHHDMITVHW */

}


#if defined (HDMI_TX0) && WITHHDMITVHW


#include "dw-hdmi.h"


/* hdmi initialization step b.4 */
static void hdmi_enable_video_path(HDMI_TX_TypeDef * const hdmi, int audio)
{
	unsigned clkdis;

	/* control period minimum duration */
	hdmi->HDMI_FC_CTRLDUR = 12;
	hdmi->HDMI_FC_EXCTRLDUR = 32;
	hdmi->HDMI_FC_EXCTRLSPAC = 1;

	/* set to fill tmds data channels */
	hdmi->HDMI_FC_CH0PREAM = 0x0b;
	hdmi->HDMI_FC_CH1PREAM = 0x16;
	hdmi->HDMI_FC_CH2PREAM = 0x21;

	hdmi->HDMI_MC_FLOWCTRL = HDMI_MC_FLOWCTRL_FEED_THROUGH_OFF_CSC_BYPASS;

	/* enable pixel clock and tmds data path */
	clkdis = 0x7f;
	clkdis &= ~HDMI_MC_CLKDIS_PIXELCLK_DISABLE;
	hdmi->HDMI_MC_CLKDIS = clkdis;

	clkdis &= ~HDMI_MC_CLKDIS_TMDSCLK_DISABLE;
	hdmi->HDMI_MC_CLKDIS = clkdis;

	if (audio) {
		clkdis &= ~HDMI_MC_CLKDIS_AUDCLK_DISABLE;
		hdmi->HDMI_MC_CLKDIS = clkdis;
	}
}

static void hdmi_enable_audio_clk(HDMI_TX_TypeDef * const hdmi)
{
	hdmi->HDMI_MC_CLKDIS &= ~ HDMI_MC_CLKDIS_AUDCLK_DISABLE;
}

static void hdmi_phy_gen2_pddq(HDMI_TX_TypeDef *hdmi, unsigned enable)
{
	hdmi->HDMI_PHY_CONF0 = (hdmi->HDMI_PHY_CONF0 & ~ HDMI_PHY_CONF0_GEN2_PDDQ_MASK) |
			!! enable * (1 << HDMI_PHY_CONF0_GEN2_PDDQ_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_GEN2_PDDQ_MASK,
//		 enable << HDMI_PHY_CONF0_GEN2_PDDQ_OFFSET);
}
static inline void hdmi_phy_test_clear(HDMI_TX_TypeDef *hdmi, unsigned bit)
{
	hdmi->HDMI_PHY_TST0 = (hdmi->HDMI_PHY_TST0 & ~ HDMI_PHY_TST0_TSTCLR_MASK) |
			!! bit * (1 << HDMI_PHY_TST0_TSTCLR_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_TST0, HDMI_PHY_TST0_TSTCLR_MASK,
//		 bit << HDMI_PHY_TST0_TSTCLR_OFFSET);
}

static void hdmi_phy_gen2_txpwron(HDMI_TX_TypeDef *hdmi, unsigned enable)
{
	hdmi->HDMI_PHY_CONF0 = (hdmi->HDMI_PHY_CONF0 & ~ HDMI_PHY_CONF0_GEN2_TXPWRON_MASK) |
			!! enable * (1 << HDMI_PHY_CONF0_GEN2_TXPWRON_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_CONF0,
//		 HDMI_PHY_CONF0_GEN2_TXPWRON_MASK,
//		 enable << HDMI_PHY_CONF0_GEN2_TXPWRON_OFFSET);
}

static void hdmi_phy_sel_data_en_pol(HDMI_TX_TypeDef *hdmi, unsigned enable)
{
	hdmi->HDMI_PHY_CONF0 = (hdmi->HDMI_PHY_CONF0 & ~ HDMI_PHY_CONF0_SELDATAENPOL_MASK) |
			!! enable * (1 << HDMI_PHY_CONF0_SELDATAENPOL_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_CONF0,
//		 HDMI_PHY_CONF0_SELDATAENPOL_MASK,
//		 enable << HDMI_PHY_CONF0_SELDATAENPOL_OFFSET);
}

static void hdmi_phy_sel_interface_control(HDMI_TX_TypeDef *hdmi, unsigned enable)
{
	hdmi->HDMI_PHY_CONF0 = (hdmi->HDMI_PHY_CONF0 & ~ HDMI_PHY_CONF0_SELDIPIF_MASK) |
			!! enable * (1 << HDMI_PHY_CONF0_SELDIPIF_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_SELDIPIF_MASK,
//		 enable << HDMI_PHY_CONF0_SELDIPIF_OFFSET);
}

static void hdmi_phy_enable_power(HDMI_TX_TypeDef *hdmi, unsigned enable)
{
	hdmi->HDMI_PHY_CONF0 = (hdmi->HDMI_PHY_CONF0 & ~ HDMI_PHY_CONF0_PDZ_MASK) |
			!! enable * (1 << HDMI_PHY_CONF0_PDZ_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_PDZ_MASK,
//		 enable << HDMI_PHY_CONF0_PDZ_OFFSET);
}

static void hdmi_phy_enable_tmds(HDMI_TX_TypeDef *hdmi, unsigned enable)
{
	hdmi->HDMI_PHY_CONF0 = (hdmi->HDMI_PHY_CONF0 & ~ HDMI_PHY_CONF0_ENTMDS_MASK) |
			!! enable * (1 << HDMI_PHY_CONF0_ENTMDS_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_ENTMDS_MASK,
//		 enable << HDMI_PHY_CONF0_ENTMDS_OFFSET);
}

static void hdmi_phy_enable_spare(HDMI_TX_TypeDef *hdmi, unsigned enable)
{
	hdmi->HDMI_PHY_CONF0 = (hdmi->HDMI_PHY_CONF0 & ~ HDMI_PHY_CONF0_SPARECTRL_MASK) |
			!! enable * (1 << HDMI_PHY_CONF0_SPARECTRL_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_SPARECTRL_MASK,
//		 enable << HDMI_PHY_CONF0_SPARECTRL_OFFSET);
}

static uint_fast32_t hdmi_compute_n(uint_fast32_t freq, uint_fast64_t pixel_clk)
{
	unsigned int n = (128 * freq) / 1000;
	unsigned int mult = 1;

	while (freq > 48000) {
		mult *= 2;
		freq /= 2;
	}

	switch (freq) {
	case 32000:
		if (pixel_clk == 25175000)
			n = 4576;
		else if (pixel_clk == 27027000)
			n = 4096;
		else if (pixel_clk == 74176000 || pixel_clk == 148352000)
			n = 11648;
		else
			n = 4096;
		n *= mult;
		break;

	case 44100:
		if (pixel_clk == 25175000)
			n = 7007;
		else if (pixel_clk == 74176000)
			n = 17836;
		else if (pixel_clk == 148352000)
			n = 8918;
		else
			n = 6272;
		n *= mult;
		break;

	case 48000:
		if (pixel_clk == 25175000)
			n = 6864;
		else if (pixel_clk == 27027000)
			n = 6144;
		else if (pixel_clk == 74176000)
			n = 11648;
		else if (pixel_clk == 148352000)
			n = 5824;
		else
			n = 6144;
		n *= mult;
		break;

	default:
		break;
	}

	return n;
}

static void hdmi_set_cts_n(HDMI_TX_TypeDef * const hdmi, unsigned int cts,
			   unsigned int n)
{
	/* Must be set/cleared first */
	hdmi->HDMI_AUD_CTS3 &= (uint8_t) ~ HDMI_AUD_CTS3_CTS_MANUAL;
//	hdmi_modb(hdmi, 0, HDMI_AUD_CTS3_CTS_MANUAL, HDMI_AUD_CTS3);

	/* nshift factor = 0 */
	hdmi->HDMI_AUD_CTS3 &= (uint8_t) ~ HDMI_AUD_CTS3_N_SHIFT_MASK;
//	hdmi_modb(hdmi, 0, HDMI_AUD_CTS3_N_SHIFT_MASK, HDMI_AUD_CTS3);

	//  HDMI Audio Clock Regenerator CTS calculated value
	hdmi->HDMI_AUD_CTS3 = ((cts >> 16) & HDMI_AUD_CTS3_AUDCTS19_16_MASK) | HDMI_AUD_CTS3_CTS_MANUAL;
//	hdmi_writeb(hdmi, ((cts >> 16) & HDMI_AUD_CTS3_AUDCTS19_16_MASK) |
//		    HDMI_AUD_CTS3_CTS_MANUAL, HDMI_AUD_CTS3);
	hdmi->HDMI_AUD_CTS2 = (cts >> 8) & 0xff;
//	hdmi_writeb(hdmi, (cts >> 8) & 0xff, HDMI_AUD_CTS2);
	hdmi->HDMI_AUD_CTS1 = cts & 0xff;
//	hdmi_writeb(hdmi, cts & 0xff, HDMI_AUD_CTS1);

	// HDMI Audio Clock Regenerator N valu
	hdmi->HDMI_AUD_N3 = (n >> 16) & 0x0f;
//	hdmi_writeb(hdmi, (n >> 16) & 0x0f, HDMI_AUD_N3);
	hdmi->HDMI_AUD_N2 = (n >> 8) & 0xff;
//	hdmi_writeb(hdmi, (n >> 8) & 0xff, HDMI_AUD_N2);
	hdmi->HDMI_AUD_N1 = n & 0xff;
//	hdmi_writeb(hdmi, n & 0xff, HDMI_AUD_N1);
}

static int hdmi_phy_wait_i2c_done(HDMI_TX_TypeDef *hdmi, unsigned msec)
{
//	local_delay_ms(msec);
//	return 0;
	for (;;)
	{
		const uint32_t val = hdmi->HDMI_IH_I2CMPHY_STAT0;
		if (val & 0x03)
		{
			 hdmi->HDMI_IH_I2CMPHY_STAT0 = val;
			 return (val & 0x01) ? 1 : 0;	// i2cmphyerror chack
		}
	}
	// timeout
	return 1;
//	ulong start;
//	u32 val;
//
//	start = get_timer(0);
//	do {
//		val = hdmi_read(hdmi, HDMI_IH_I2CMPHY_STAT0);
//		if (val & 0x3) {
//			hdmi_write(hdmi, val, HDMI_IH_I2CMPHY_STAT0);
//			return 0;
//		}
//
//		udelay(100);
//	} while (get_timer(start) < msec);
//
	return 1;
}

static void hdmi_phy_i2c_write(HDMI_TX_TypeDef *hdmi, unsigned data, unsigned addr)
{
	// I2C reset
//	hdmi->HDMI_PHY_I2CM_SOFTRSTZ_ADDR = 0x00;
//	while ((hdmi->HDMI_PHY_I2CM_SOFTRSTZ_ADDR & 0x01) == 0)
//		;
	hdmi->HDMI_PHY_I2CM_SLAVE_ADDR = HDMI_PHY_I2CM_SLAVE_ADDR_PHY_GEN2;
	hdmi->HDMI_IH_I2CMPHY_STAT0 = 0xFF;
	hdmi->HDMI_PHY_I2CM_ADDRESS_ADDR = addr;
	hdmi->HDMI_PHY_I2CM_DATAO_1_ADDR = (data >> 8);
	hdmi->HDMI_PHY_I2CM_DATAO_0_ADDR = (data >> 0);
	hdmi->HDMI_PHY_I2CM_OPERATION_ADDR = HDMI_PHY_I2CM_OPERATION_ADDR_WRITE;

//	hdmi_write(hdmi, 0xff, HDMI_IH_I2CMPHY_STAT0);
//	hdmi_write(hdmi, addr, HDMI_PHY_I2CM_ADDRESS_ADDR);
//	hdmi_write(hdmi, (u8)(data >> 8), HDMI_PHY_I2CM_DATAO_1_ADDR);
//	hdmi_write(hdmi, (u8)(data >> 0), HDMI_PHY_I2CM_DATAO_0_ADDR);
//	hdmi_write(hdmi, HDMI_PHY_I2CM_OPERATION_ADDR_WRITE,
//		   HDMI_PHY_I2CM_OPERATION_ADDR);

	hdmi_phy_wait_i2c_done(hdmi, 1000);
	hdmi->HDMI_PHY_I2CM_OPERATION_ADDR = 0;
}

static void hdmi_phy_i2c_read(HDMI_TX_TypeDef *hdmi, unsigned * data, unsigned addr)
{
	// I2C reset
//	hdmi->HDMI_PHY_I2CM_SOFTRSTZ_ADDR = 0x00;
//	while ((hdmi->HDMI_PHY_I2CM_SOFTRSTZ_ADDR & 0x01) == 0)
//		;
	hdmi->HDMI_PHY_I2CM_SLAVE_ADDR = HDMI_PHY_I2CM_SLAVE_ADDR_PHY_GEN2;
	hdmi->HDMI_IH_I2CMPHY_STAT0 = 0xFF;
	hdmi->HDMI_PHY_I2CM_ADDRESS_ADDR = addr;
	hdmi->HDMI_PHY_I2CM_OPERATION_ADDR = HDMI_PHY_I2CM_OPERATION_ADDR_READ;

//	hdmi_write(hdmi, 0xff, HDMI_IH_I2CMPHY_STAT0);
//	hdmi_write(hdmi, addr, HDMI_PHY_I2CM_ADDRESS_ADDR);
//	hdmi_write(hdmi, (u8)(data >> 8), HDMI_PHY_I2CM_DATAO_1_ADDR);
//	hdmi_write(hdmi, (u8)(data >> 0), HDMI_PHY_I2CM_DATAO_0_ADDR);
//	hdmi_write(hdmi, HDMI_PHY_I2CM_OPERATION_ADDR_WRITE,
//		   HDMI_PHY_I2CM_OPERATION_ADDR);

	* data = 0xDEADBEEF;
	if (0 == hdmi_phy_wait_i2c_done(hdmi, 1000))
	{
		* data = hdmi->HDMI_PHY_I2CM_DATAI_1_ADDR * 256 + hdmi->HDMI_PHY_I2CM_DATAI_0_ADDR;
	}
	hdmi->HDMI_PHY_I2CM_OPERATION_ADDR = 0;
}

enum {
	DW_HDMI_RES_8,
	DW_HDMI_RES_10,
	DW_HDMI_RES_12,
	DW_HDMI_RES_MAX,
};

struct dw_hdmi_mpll_config {
	unsigned long mpixelclock;
	struct {
		uint16_t cpce;
		uint16_t gmp;
	} res[DW_HDMI_RES_MAX];
};

struct dw_hdmi_curr_ctrl {
	unsigned long mpixelclock;
	uint16_t curr[DW_HDMI_RES_MAX];
};

struct dw_hdmi_phy_config {
	uint64_t mpixelclock;
	uint32_t sym_ctr;    /* clock symbol and transmitter control */
	uint32_t term;       /* transmission termination value */
	uint32_t vlev_ctr;   /* voltage level control */
};

// https://github.com/EchoHeim/Allwinner-H616/blob/c499413803e4128439cadf2f56972e207721abe4/kernel/drivers/gpu/drm/sun4i/sun8i_hdmi_phy.c#L18

static const struct dw_hdmi_mpll_config sun50i_h616_mpll_cfg[] = {
    {
        27000000,
        {
            {0x00b3, 0x0003},
            {0x2153, 0x0003},
            {0x40f3, 0x0003},
        },
    },
    {
        74250000,
        {
            {0x0072, 0x0003},
            {0x2145, 0x0003},
            {0x4061, 0x0003},
        },
    },
    {
        148500000,
        {
            {0x0051, 0x0003},
            {0x214c, 0x0003},
            {0x4064, 0x0003},
        },
    },
    {
        297000000,
        {
            {0x0040, 0x0003},
            {0x3b4c, 0x0003},
            {0x5a64, 0x0003},
        },
    },
    {
        594000000,
        {
            {0x1a40, 0x0003},
            {0x3b4c, 0x0003},
            {0x5a64, 0x0003},
        },
    },
    {
        ~0UL,
        {
            {0x0000, 0x0000},
            {0x0000, 0x0000},
            {0x0000, 0x0000},
        },
    }};

static const struct dw_hdmi_curr_ctrl sun50i_h616_cur_ctr[] = {
    /* pixelclk    bpp8    bpp10   bpp12 */
    {
        27000000,
        {0x0012, 0x0000, 0x0000},
    },
    {
        74250000,
        {0x0013, 0x0013, 0x0013},
    },
    {
        148500000,
        {0x0019, 0x0019, 0x0019},
    },
    {
        297000000,
        {0x0019, 0x001b, 0x0019},
    },
    {
        594000000,
        {0x0010, 0x0010, 0x0010},
    },
    {
        ~0UL,
        {0x0000, 0x0000, 0x0000},
    }};

static const struct dw_hdmi_phy_config sun50i_h616_phy_config[] = {
    /*pixelclk   symbol   term   vlev*/
    {27000000, 0x8009, 0x0007, 0x02b0},
    {74250000, 0x8019, 0x0004, 0x0290},
    {148500000, 0x8019, 0x0004, 0x0290},
    {297000000, 0x8039, 0x0004, 0x022b},
    {594000000, 0x8029, 0x0000, 0x008a},
    {~0UL, 0x0000, 0x0000, 0x0000}};

// See also https://github.com/MYIR-ALLWINNER/myir-t5-kernel/blob/a7089355dd727f5aaedade642f5fbc5b354b215a/drivers/gpu/drm/bridge/dw-hdmi.c#L733

static int hdmi_phy_configure(HDMI_TX_TypeDef * const hdmi, uint_fast32_t mpixelclock, unsigned res, int cscon)
{
	//PRINTF("hdmi->HDMI_PHY_STAT0=%08X\n", (unsigned) hdmi->HDMI_PHY_STAT0);

	/* Enable csc path */
	HDMI_TX0->HDMI_MC_FLOWCTRL = cscon ? HDMI_MC_FLOWCTRL_FEED_THROUGH_OFF_CSC_IN_PATH : HDMI_MC_FLOWCTRL_FEED_THROUGH_OFF_CSC_BYPASS;

	/* gen2 tx power off */
	hdmi_phy_gen2_txpwron(hdmi, 0);
//
//	/* gen2 pddq */
	hdmi_phy_gen2_pddq(hdmi, 1);

	/* phy reset */
	HDMI_TX0->HDMI_MC_PHYRSTZ = HDMI_MC_PHYRSTZ_DEASSERT;
	HDMI_TX0->HDMI_MC_PHYRSTZ = HDMI_MC_PHYRSTZ_ASSERT;
	HDMI_TX0->HDMI_MC_HEACPHY_RST = HDMI_MC_HEACPHY_RST_ASSERT;

//	HDMI_TX0->HDMI_MC_PHYRSTZ = HDMI_MC_PHYRSTZ_DEASSERT;
//	HDMI_TX0->HDMI_MC_HEACPHY_RST = HDMI_MC_HEACPHY_RST_DEASSERT;

	hdmi_phy_test_clear(hdmi, 1);
	hdmi->HDMI_PHY_I2CM_SLAVE_ADDR = HDMI_PHY_I2CM_SLAVE_ADDR_PHY_GEN2;
	hdmi_phy_test_clear(hdmi, 0);

	const struct dw_hdmi_mpll_config *mpll_config = sun50i_h616_mpll_cfg;
	const struct dw_hdmi_curr_ctrl *curr_ctrl = sun50i_h616_cur_ctr;
	const struct dw_hdmi_phy_config *phy_config = sun50i_h616_phy_config;

	/* PLL/MPLL Cfg - always match on final entry */
	for (; mpll_config->mpixelclock != ~0UL; mpll_config++)
		if (mpixelclock <=
		    mpll_config->mpixelclock)
			break;

	for (; curr_ctrl->mpixelclock != ~0UL; curr_ctrl++)
		if (mpixelclock <=
		    curr_ctrl->mpixelclock)
			break;

	for (; phy_config->mpixelclock != ~0UL; phy_config++)
		if (mpixelclock <=
		    phy_config->mpixelclock)
			break;

	if (mpll_config->mpixelclock == ~0UL ||
	    curr_ctrl->mpixelclock == ~0UL ||
	    phy_config->mpixelclock == ~0UL) {
		PRINTF("Pixel clock %u - unsupported by HDMI\n",
			(unsigned) mpixelclock);
		return -1;
	}


	const int resix = DW_HDMI_RES_8;
	hdmi_phy_i2c_write(hdmi, mpll_config->res[resix].cpce, PHY_OPMODE_PLLCFG);
	hdmi_phy_i2c_write(hdmi, mpll_config->res[resix].gmp, PHY_PLLGMPCTRL);
	hdmi_phy_i2c_write(hdmi, curr_ctrl->curr[resix], PHY_PLLCURRCTRL);

	hdmi_phy_i2c_write(hdmi, 0x0000, PHY_PLLPHBYCTRL);
	hdmi_phy_i2c_write(hdmi, 0x0006, PHY_PLLCLKBISTPHASE);

//	for (i = 0; hdmi->phy_cfg[i].mpixelclock != (~0ul); i++)
//		if (mpixelclock <= hdmi->phy_cfg[i].mpixelclock)
//			break;

	/*
	 * resistance term 133ohm cfg
	 * preemp cgf 0.00
	 * tx/ck lvl 10
	 */
	hdmi_phy_i2c_write(hdmi, phy_config->term, PHY_TXTERM);
	hdmi_phy_i2c_write(hdmi, phy_config->sym_ctr, PHY_CKSYMTXCTRL);
	hdmi_phy_i2c_write(hdmi, phy_config->vlev_ctr, PHY_VLEVCTRL);

	/* remove clk term */
	hdmi_phy_i2c_write(hdmi, 0x8000, PHY_CKCALCTRL);

	//PRINTF("hdmi->HDMI_PHY_STAT0=%08X\n", (unsigned) hdmi->HDMI_PHY_STAT0);

	hdmi_phy_enable_power(hdmi, 1);

	/* toggle tmds enable */
	hdmi_phy_enable_tmds(hdmi, 0);
	hdmi_phy_enable_tmds(hdmi, 1);

	/* gen2 tx power on */
	hdmi_phy_gen2_txpwron(hdmi, 1);
	hdmi_phy_gen2_pddq(hdmi, 0);

	hdmi_phy_enable_spare(hdmi, 1);

	/* wait for phy pll lock */
	//TP();
	while ((hdmi->HDMI_PHY_STAT0 & HDMI_PHY_TX_PHY_LOCK) == 0)
		;
	//PRINTF("hdmi->HDMI_PHY_STAT0=%08X\n", (unsigned) hdmi->HDMI_PHY_STAT0);

//	start = get_timer(0);
//	do {
//		val = hdmi_read(hdmi, HDMI_PHY_STAT0);
//		if (!(val & HDMI_PHY_TX_PHY_LOCK))
//			return 0;
//
//		udelay(100);
//	} while (get_timer(start) < 5);
	return 0;	// OK
}

static void hdmi_tx_hdcp_config(HDMI_TX_TypeDef * const hdmi)
{
	uint8_t de;

	if (1 /*hdmi->hdmi_data.video_mode.mdataenablepolarity */)
		de = HDMI_A_VIDPOLCFG_DATAENPOL_ACTIVE_HIGH;
	else
		de = HDMI_A_VIDPOLCFG_DATAENPOL_ACTIVE_LOW;

	/* disable rx detect */
	hdmi->HDMI_A_HDCPCFG0 = (hdmi->HDMI_A_HDCPCFG0 & ~ HDMI_A_HDCPCFG0_RXDETECT_MASK) |
			HDMI_A_HDCPCFG0_RXDETECT_DISABLE;
//	hdmi_modb(hdmi, HDMI_A_HDCPCFG0_RXDETECT_DISABLE,
//		  HDMI_A_HDCPCFG0_RXDETECT_MASK, HDMI_A_HDCPCFG0);

	hdmi->HDMI_A_VIDPOLCFG = (hdmi->HDMI_A_VIDPOLCFG & ~ HDMI_A_VIDPOLCFG_DATAENPOL_MASK) |
			de;
//	hdmi_modb(hdmi, de, HDMI_A_VIDPOLCFG_DATAENPOL_MASK, HDMI_A_VIDPOLCFG);

	hdmi->HDMI_A_HDCPCFG1 = (hdmi->HDMI_A_HDCPCFG0 & ~ HDMI_A_HDCPCFG1_ENCRYPTIONDISABLE_MASK) |
			HDMI_A_HDCPCFG1_ENCRYPTIONDISABLE_DISABLE;
//	hdmi_modb(hdmi, HDMI_A_HDCPCFG1_ENCRYPTIONDISABLE_DISABLE,
//		  HDMI_A_HDCPCFG1_ENCRYPTIONDISABLE_MASK, HDMI_A_HDCPCFG1);
}

static void hdmi_video_sample(HDMI_TX_TypeDef * const hdmi)
{
	uint32_t val;
	// https://github.com/MYIR-ALLWINNER/myir-t5-kernel/blob/a7089355dd727f5aaedade642f5fbc5b354b215a/drivers/gpu/drm/bridge/dw-hdmi.c#L366
	unsigned color_format = 0x01;	// RGB, 8 bit

	val = HDMI_TX_INVID0_INTERNAL_DE_GENERATOR_DISABLE |
		((color_format << HDMI_TX_INVID0_VIDEO_MAPPING_OFFSET) &
		HDMI_TX_INVID0_VIDEO_MAPPING_MASK);
	hdmi->HDMI_TX_INVID0 = val;

	/* Enable TX stuffing: When DE is inactive, fix the output data to 0 */
	val = HDMI_TX_INSTUFFING_BDBDATA_STUFFING_ENABLE |
		HDMI_TX_INSTUFFING_RCRDATA_STUFFING_ENABLE |
		HDMI_TX_INSTUFFING_GYDATA_STUFFING_ENABLE;
	hdmi->HDMI_TX_INSTUFFING = val;
	hdmi->HDMI_TX_GYDATA0 = 0x00;
	hdmi->HDMI_TX_GYDATA1 = 0x00;
	hdmi->HDMI_TX_RCRDATA0 = 0x00;
	hdmi->HDMI_TX_RCRDATA1 = 0x00;
	hdmi->HDMI_TX_BCBDATA0 = 0x00;
	hdmi->HDMI_TX_BCBDATA1 = 0x00;
}

static void dw_hdmi_i2s_setup(HDMI_TX_TypeDef * const hdmi)
{
	/* Reset the FIFOs before applying new params */
	hdmi->HDMI_AUD_CONF0 = HDMI_AUD_CONF0_SW_RESET;
	hdmi->HDMI_MC_SWRSTZ = ~HDMI_MC_SWRSTZ_I2SSWRST_REQ;

	unsigned int inputclkfs	= HDMI_AUD_INPUTCLKFS_64FS;
	unsigned int conf0 = (HDMI_AUD_CONF0_I2S_SELECT | HDMI_AUD_CONF0_I2S_EN0);
	unsigned int conf1 = 0;

	//	switch (hparms->channels) {
	//	case 7 ... 8:
	//		conf0 |= HDMI_AUD_CONF0_I2S_EN3;
	//		fallthrough;
	//	case 5 ... 6:
	//		conf0 |= HDMI_AUD_CONF0_I2S_EN2;
	//		fallthrough;
	//	case 3 ... 4:
	//		conf0 |= HDMI_AUD_CONF0_I2S_EN1;
	//		/* Fall-thru */
	//	}

	//	switch (hparms->sample_width) {
	//	case 16:
	//		conf1 = HDMI_AUD_CONF1_WIDTH_16;
	//		break;
	//	case 24:
	//	case 32:
	//		conf1 = HDMI_AUD_CONF1_WIDTH_24;
	//		break;
	//	}

	//	switch (fmt->fmt) {
	//	case HDMI_I2S:
	//		conf1 |= HDMI_AUD_CONF1_MODE_I2S;
	//		break;
	//	case HDMI_RIGHT_J:
	//		conf1 |= HDMI_AUD_CONF1_MODE_RIGHT_J;
	//		break;
	//	case HDMI_LEFT_J:
	//		conf1 |= HDMI_AUD_CONF1_MODE_LEFT_J;
	//		break;
	//	case HDMI_DSP_A:
	//		conf1 |= HDMI_AUD_CONF1_MODE_BURST_1;
	//		break;
	//	case HDMI_DSP_B:
	//		conf1 |= HDMI_AUD_CONF1_MODE_BURST_2;
	//		break;
	//	default:
	//		dev_err(dev, "unsupported format\n");
	//		return -EINVAL;
	//	}

	//	dw_hdmi_set_sample_rate(hdmi, hparms->sample_rate);
	//	dw_hdmi_set_channel_status(hdmi, hparms->iec.status);
	//	dw_hdmi_set_channel_count(hdmi, hparms->channels);
	//	dw_hdmi_set_channel_allocation(hdmi, hparms->cea.channel_allocation);
	//


	conf1 = WITHADAPTERHDMIWIDTH == 16 ? HDMI_AUD_CONF1_WIDTH_16 : HDMI_AUD_CONF1_WIDTH_24;
	conf1 |= HDMI_AUD_CONF1_MODE_I2S;

	hdmi->HDMI_AUD_INPUTCLKFS = inputclkfs;
	hdmi->HDMI_AUD_CONF0 = conf0;
	hdmi->HDMI_AUD_CONF1 = conf1;
	//	hdmi_write(audio, inputclkfs, HDMI_AUD_INPUTCLKFS);
	//	hdmi_write(audio, conf0, HDMI_AUD_CONF0);
	//	hdmi_write(audio, conf1, HDMI_AUD_CONF1);

	//	PRINTF("hdmi->HDMI_AUD_INPUTCLKFS=%08X\n", (unsigned) hdmi->HDMI_AUD_INPUTCLKFS);
	//	PRINTF("hdmi->HDMI_AUD_CONF0=%08X\n", (unsigned) hdmi->HDMI_AUD_CONF0);
	//	PRINTF("hdmi->HDMI_AUD_CONF1=%08X\n", (unsigned) hdmi->HDMI_AUD_CONF1);
}

static void dw_hdmi_clear_overflow(HDMI_TX_TypeDef * const hdmi)
{
	int count;
	uint8_t val;

	/* TMDS software reset */
	hdmi->HDMI_MC_SWRSTZ = (uint8_t) ~ HDMI_MC_SWRSTZ_TMDSSWRST_REQ;
	while ((hdmi->HDMI_MC_SWRSTZ & HDMI_MC_SWRSTZ_TMDSSWRST_REQ) == 0)
		;

	val = hdmi->HDMI_FC_INVIDCONF;
//	if (hdmi->dev_type == IMX6DL_HDMI) {
//		hdmi->HDMI_FC_INVIDCONF = val;
//		return;
//	}

	for (count = 0; count < 4; count++)
		hdmi->HDMI_FC_INVIDCONF = val;
}

//static void sun8i_hdmi_phy_unlock(struct sun8i_hdmi_phy *phy)
//{
//    /* enable read access to HDMI controller */
//    regmap_vsu_write(phy->regs, SUN8I_HDMI_PHY_READ_EN_REG,
//                 SUN8I_HDMI_PHY_READ_EN_MAGIC);
//
//    /* unscramble register offsets */
//    regmap_vsu_write(phy->regs, SUN8I_HDMI_PHY_UNSCRAMBLE_REG,
//                 SUN8I_HDMI_PHY_UNSCRAMBLE_MAGIC);
//}
//
//static void sun50i_hdmi_phy_init_h616(struct sun8i_hdmi_phy *phy)
//{
//    regmap_update_bits(phy->regs, SUN8I_HDMI_PHY_REXT_CTRL_REG,
//                       SUN8I_HDMI_PHY_REXT_CTRL_REXT_EN,
//                       SUN8I_HDMI_PHY_REXT_CTRL_REXT_EN);
//
//    regmap_update_bits(phy->regs, SUN8I_HDMI_PHY_REXT_CTRL_REG,
//                       0xffff0000, 0x80c00000);
//}
//##	06010000: 00000000 FFFF0000 00184210 00000002 00000000 000F990B 00000004 00000000
//##	06010020: 00000F80 0C0040D8 02700203 000C6023 00000000 00000000 00000000 00000000
//##	06010040: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
//##	06010060: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
//##	06010080: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
//##	060100A0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
//##	060100C0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
//##	060100E0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

#if defined (HDMI_PHY)
static void h3_hdmi_phy_init(uint_fast32_t dotclock)
{
	HDMI_PHY_TypeDef * const phy = HDMI_PHY;


//	/* enable read access to HDMI controller */
//	HDMI_PHY->HDMI_PHY_READ_EN = 0x54524545;
//	/* descramble register offsets */
//	HDMI_PHY->HDMI_PHY_UNSCRAMBLE = 0x42494E47;
//#if CPUSTYLE_T507 || CPUSTYLE_H616
//	PRINTF("phy->REXT_CTRL=%08X\n", (unsigned) phy->REXT_CTRL);
//	local_delay_ms(10);
//	phy->REXT_CTRL |= SUN8I_HDMI_PHY_REXT_CTRL_REXT_EN;
//	local_delay_ms(10);
//	phy->REXT_CTRL = (HDMI_PHY->REXT_CTRL & 0xFFFF0000) | 0x80C00000;
//	local_delay_ms(10);
//	PRINTF("phy->REXT_CTRL=%08X\n", (unsigned) phy->REXT_CTRL);
//#endif

	//PRINTF("phy->HDMI_PHY_STS=%08X\n", (unsigned) phy->HDMI_PHY_STS);
	// HDMI PHY init, the following black magic is based on the procedure documented at:
	// http://linux-sunxi.org/images/3/38/AW_HDMI_TX_PHY_S40_Spec_V0.1.pdf
	phy->HDMI_PHY_CFG1 = 0;
	phy->HDMI_PHY_CFG1 = 1;
	local_delay_us(5);
	phy->HDMI_PHY_CFG1 |= (UINT32_C(1) << 16);
	phy->HDMI_PHY_CFG1 |= (UINT32_C(1) << 1);
	local_delay_us(10);
	phy->HDMI_PHY_CFG1 |= (UINT32_C(1) << 2);
	local_delay_us(5);
	phy->HDMI_PHY_CFG1 |= (UINT32_C(1) << 3);
	local_delay_us(40);
	phy->HDMI_PHY_CFG1 |= (UINT32_C(1) << 19);
	local_delay_us(100);
	phy->HDMI_PHY_CFG1 |= (UINT32_C(1) << 18);
	phy->HDMI_PHY_CFG1 |= (7 << 4);
	int z = 0;
	while (z ++ < 10 && (phy->HDMI_PHY_STS & 0x80) == 0)
		local_delay_ms(10);
	//PRINTF("phy->HDMI_PHY_STS=%08X\n", (unsigned) phy->HDMI_PHY_STS);

	phy->HDMI_PHY_CFG1 |= (0xf << 4);
	phy->HDMI_PHY_CFG1 |= (0xf << 8);
	phy->HDMI_PHY_CFG3 |= (UINT32_C(1) << 0) | (UINT32_C(1) << 2);

	phy->HDMI_PHY_PLL1 &= ~ (UINT32_C(1) << 26);
	phy->HDMI_PHY_CEC = 0;


	// 148.5 MHz
	uint_fast32_t PHY_PLL1_VAL = (0x3ddc5040 & ~ ((UINT32_C(1) << 25)) & ~ ((UINT32_C(1) << 26)));
	uint_fast32_t PHY_PLL2_VAL = 0x80084381;
	uint_fast32_t PHY_PLL3_VAL = 0x00000001;

	uint_fast32_t PHY_CFG1_VAL = 0x01FFFF7F;
	uint_fast32_t PHY_CFG2_VAL = 0x8063A800;
	uint_fast32_t PHY_CFG3_VAL = 0x0F81C485;

	uint_fast32_t tmp_rcal_200 = 0;

	switch (dotclock)
	{
	default:
	case 148500000:
		// 148.5 MHz
		PHY_PLL1_VAL = (0x3ddc5040 & ~ ((UINT32_C(1) << 25)) & ~ ((UINT32_C(1) << 26)));
		PHY_PLL2_VAL = 0x80084381;
		PHY_PLL3_VAL = 0x00000001;

		PHY_CFG1_VAL = 0x01FFFF7F;
		PHY_CFG2_VAL = 0x8063A800;
		PHY_CFG3_VAL = 0x0F81C485;
		break;

	case 74250000:
		// 74.25 MHz
		PHY_PLL1_VAL = 0x3ddc5040;
		PHY_PLL2_VAL = 0x80084343;
		PHY_PLL3_VAL = 0x00000001;

		tmp_rcal_200 = 0;
		PHY_CFG1_VAL = 0x01FFFF7F;
		PHY_CFG2_VAL = 0x80623000 | tmp_rcal_200;
		PHY_CFG3_VAL = 0x0F814385;
		break;
	}

	switch (dotclock)
	{
	default:
		PRINTF("Unspecified dot clock %u Hz\n", (unsigned) dotclock);
		//break;
	case 74250000:
	case 148500000:
		phy->HDMI_PHY_PLL1 = PHY_PLL1_VAL;
		phy->HDMI_PHY_PLL2 = PHY_PLL2_VAL;
		local_delay_us(10000);
		phy->HDMI_PHY_PLL3 = PHY_PLL3_VAL;
		phy->HDMI_PHY_PLL1 |= (UINT32_C(1) << 25);

		local_delay_us(10000);
		{
			uint32_t tmp = (phy->HDMI_PHY_STS & 0x1F800) >> 11;
			phy->HDMI_PHY_PLL1 |= (UINT32_C(1) << 31) | (UINT32_C(1) << 30) | tmp;
		}

		phy->HDMI_PHY_CFG1 = PHY_CFG1_VAL;
		phy->HDMI_PHY_CFG2 = PHY_CFG2_VAL;
		phy->HDMI_PHY_CFG3 = PHY_CFG3_VAL;
		break;

//	case 297000000:
//
//		phy->HDMI_PHY_PLL1 = 0x3ddc5040;
//		phy->HDMI_PHY_PLL2 = 0x80084381;
//		phy->HDMI_PHY_PLL3 = 1;
//
//		phy->HDMI_PHY_CFG1 = 0x01FFFF7F;
//		phy->HDMI_PHY_CFG2 = 0x8063A800;
//		phy->HDMI_PHY_CFG3 = 0x0F81C485;
//		break;
//
//
//	case 27000000:
//		break;

	}

	/* enable read access to HDMI controller */
	phy->HDMI_PHY_READ_EN = 0x54524545;
	/* descramble register offsets */
	phy->HDMI_PHY_UNSCRAMBLE = 0x42494E47;
}
#endif /* defined (HDMI_PHY) */

static void t507_hdmi_phy_init(uint_fast32_t dotclock)
{
	HDMI_TX_TypeDef * const hdmi = HDMI_TX0;
	/* PHY_I2CM_SLAVE_ADDR field values */
//		HDMI_PHY_I2CM_SLAVE_ADDR_PHY_GEN2 = 0x69,
//		HDMI_PHY_I2CM_SLAVE_ADDR_HEAC_PHY = 0x49,
	int cscon = 0;//hdmi->sink_is_hdmi && is_color_space_conversion(hdmi);

	/* hdmi phy spec says to do the phy initialization sequence twice */
	int i;
	for (i = 0; i < 2; i++) {
		hdmi_phy_sel_data_en_pol(hdmi, 1);
		hdmi_phy_sel_interface_control(hdmi, 0);
		hdmi_phy_enable_tmds(hdmi, 0);
		hdmi_phy_enable_power(hdmi, 0);

		int ret = hdmi_phy_configure(hdmi, dotclock, 8, cscon);
//		if (ret) {
//			debug("hdmi phy config failure %d\n", ret);
//			return ret;
//		}
	}

}

static void h3_hdmi_init(const videomode_t * vdmode)
{
	HDMI_TX_TypeDef * const hdmi = HDMI_TX0;

	/* Accuhdmithis display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned HSYNC = vdmode->hsync;	/*  */
	const unsigned VSYNC = vdmode->vsync;	/*  */
	const unsigned HFP = vdmode->hfp;	/* horizontal front porch */
	const unsigned VFP = vdmode->vfp;	/* vertical front porch */
	const unsigned HBP = vdmode->hbp;	/* horizontal back porch */
	const unsigned VBP = vdmode->vbp;	/* vertical back porch */
	const unsigned LEFTMARGIN = HSYNC + HBP;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = VSYNC + VBP;	/* vertical delay before DE start */
	const unsigned HBLANKING = HBP + HSYNC +  HFP;	/* Horizontal Blanking = hsync + hbp + hfp */
	const unsigned VBLANKING = VBP + VSYNC +  VFP;	/* Vertical Blanking = vsync + vbp + vfp */
	const unsigned HTOTAL = HBLANKING + WIDTH;	/* horizontal full period */
	const unsigned VTOTAL = VBLANKING + HEIGHT;	/* vertical full period */

//	PRINTF("LEFTMARGIN=%u\n", LEFTMARGIN);
//	PRINTF("TOPMARGIN=%u\n", TOPMARGIN);
//	PRINTF("HTOTAL=%u\n", HTOTAL);
//	PRINTF("VTOTAL=%u\n", VTOTAL);
//	PRINTF("HBLANKING=%u\n", HBLANKING);
//	PRINTF("VBLANKING=%u\n", VBLANKING);
//	PRINTF("HSYNC=%u\n", HSYNC);
//	PRINTF("VSYNC=%u\n", VSYNC);
//	PRINTF("HFP=%u\n", HFP);
//	PRINTF("VFP=%u\n", VFP);
//
//	PRINTF("dot clock = %u\n", display_getdotclock(vdmode));
	// HDMI Config, based on the documentation at:
	// https://people.freebsd.org/~gonzo/arm/iMX6-HDMI.pdf
	hdmi->HDMI_FC_INVIDCONF = (UINT32_C(1) << 6) | (UINT32_C(1) << 5) | (UINT32_C(1) << 4) | (UINT32_C(1) << 3); // Polarity etc
	hdmi->HDMI_FC_INHACTV0 = (WIDTH & 0xff);    // Horizontal pixels
	hdmi->HDMI_FC_INHACTV1 = (WIDTH >> 8);      // Horizontal pixels
	hdmi->HDMI_FC_INHBLANK0 = (HBLANKING & 0xff);     // Horizontal blanking
	hdmi->HDMI_FC_INHBLANK1 = (HBLANKING >> 8);       // Horizontal blanking

	hdmi->HDMI_FC_INVACTV0 = (HEIGHT & 0xff);    // Vertical pixels
	hdmi->HDMI_FC_INVACTV1 = (HEIGHT >> 8);      // Vertical pixels
	hdmi->HDMI_FC_INVBLANK = VBLANKING;               // Vertical blanking

	hdmi->HDMI_FC_HSYNCINDELAY0 = (HFP & 0xff);
	hdmi->HDMI_FC_HSYNCINDELAY1 = (HFP >> 8);    // Horizontal Front porch
	hdmi->HDMI_FC_HSYNCINWIDTH0 = (HSYNC & 0xff);  // Horizontal sync pulse
	hdmi->HDMI_FC_HSYNCINWIDTH1 = (HSYNC >> 8);    // Horizontal sync pulse

	hdmi->HDMI_FC_VSYNCINDELAY = VFP;
	hdmi->HDMI_FC_VSYNCINWIDTH = VSYNC;            // Vertical sync pulse

//	hdmi->HDMI_FC_CTRLDUR = 12;   // Frame Composer Control Period Duration
//	hdmi->HDMI_FC_EXCTRLDUR = 32; // Frame Composer Extended Control Period Duration
//	hdmi->HDMI_FC_EXCTRLSPAC = 1; // Frame Composer Extended Control Period Maximum Spacing
//	hdmi->HDMI_FC_CH0PREAM = 0x0b; // Frame Composer Channel 0 Non-Preamble Data
//	hdmi->HDMI_FC_CH1PREAM = 0x16; // Frame Composer Channel 1 Non-Preamble Data
//	hdmi->HDMI_FC_CH2PREAM = 0x21; // Frame Composer Channel 2 Non-Preamble Data
//	hdmi->HDMI_MC_FLOWCTRL = HDMI_MC_FLOWCTRL_FEED_THROUGH_OFF_CSC_BYPASS;    // Main Controller Feed Through Control
//	hdmi->HDMI_MC_CLKDIS = 0x74; // Main Controller Synchronous Clock Domain Disable

//	PRINTF("Detected HDMI controller 0x%x:0x%x:0x%x:0x%x\n",
//			hdmi->HDMI_DESIGN_ID,
//			hdmi->HDMI_REVISION_ID,
//			hdmi->HDMI_PRODUCT_ID0,
//			hdmi->HDMI_PRODUCT_ID1
//			);
//
//	PRINTF(" Config 0x%x:0x%x:0x%x:0x%x\n",
//			hdmi->HDMI_CONFIG0_ID,
//			hdmi->HDMI_CONFIG1_ID,
//			hdmi->HDMI_CONFIG2_ID,
//			hdmi->HDMI_CONFIG3_ID
//			);
}

#endif /* defined (HDMI_TX0) */

#if 0
// A64
// See https://groups.google.com/g/linux-sunxi/c/96vcJ1kdXXw
static void de2_tcon_enable(struct lcd *lcd)
{
	struct drm_crtc *crtc = &lcd->crtc;
	const struct drm_display_mode *mode = &crtc->mode;
	int interlace = mode->flags & DRM_MODE_FLAG_INTERLACE ? 2 : 1;
	int start_delay;
	u32 data;

	orl_relaxed(lcd->mmio + TCON_GCTL_REG, TCON_GCTL_TCON_ENABLE);

	data = XY(mode->hdisplay - 1, mode->vdisplay / interlace - 1);
	writel_relaxed(data, lcd->mmio + TCON1_BASIC0_REG);
	writel_relaxed(data, lcd->mmio + TCON1_BASIC1_REG);
	writel_relaxed(data, lcd->mmio + TCON1_BASIC2_REG);
	writel_relaxed(XY(mode->htotal - 1, mode->htotal - mode->hsync_start - 1), lcd->mmio + TCON1_BASIC3_REG);
	writel_relaxed(
			XY(mode->vtotal * (3 - interlace),
					mode->vtotal - mode->vsync_start - 1),
			lcd->mmio + TCON1_BASIC4_REG);
	writel_relaxed(
			XY(mode->hsync_end - mode->hsync_start - 1,
					mode->vsync_end - mode->vsync_start - 1),
			lcd->mmio + TCON1_BASIC5_REG);

	data = TCON1_IO_POL_IO2_inv;
	if (mode->flags & DRM_MODE_FLAG_PVSYNC)
		data |= TCON1_IO_POL_IO0_inv;
	if (mode->flags & DRM_MODE_FLAG_PHSYNC)
		data |= TCON1_IO_POL_IO1_inv;
	writel_relaxed(data, lcd->mmio + TCON1_IO_POL_REG);

	andl_relaxed(lcd->mmio + TCON_CEU_CTL_REG, ~TCON_CEU_CTL_ceu_en);

	if (interlace == 2)
		orl_relaxed(lcd->mmio + TCON1_CTL_REG, TCON1_CTL_INTERLACE_ENABLE);
	else
		andl_relaxed(lcd->mmio + TCON1_CTL_REG, ~TCON1_CTL_INTERLACE_ENABLE);

	writel_relaxed(0, lcd->mmio + TCON1_FILL_CTL_REG);
	writel_relaxed(mode->vtotal + 1, lcd->mmio + TCON1_FILL_START0_REG);
	writel_relaxed(mode->vtotal, lcd->mmio + TCON1_FILL_END0_REG);
	writel_relaxed(0, lcd->mmio + TCON1_FILL_DATA0_REG);

	start_delay = (mode->vtotal - mode->vdisplay) / interlace - 5;
	if (start_delay > 31)
		start_delay = 31;
	data = readl_relaxed(lcd->mmio + TCON1_CTL_REG);
	data &= ~TCON1_CTL_Start_Delay_MASK;
	data |= start_delay << TCON1_CTL_Start_Delay_SHIFT;
	writel_relaxed(data, lcd->mmio + TCON1_CTL_REG);

	orl_relaxed(lcd->mmio + TCON1_CTL_REG, TCON1_CTL_TCON_ENABLE);
}
#endif

static void t113_set_tcontv_sequence_parameters(const videomode_t * vdmode)
{
#if defined (TCONTV_PTR)

	const unsigned interlace = vdmode->interlaced ? 2 : 1;
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned HSYNC = vdmode->hsync;	/*  */
	const unsigned VSYNC = vdmode->vsync;	/*  */
	const unsigned HFP = vdmode->hfp;	/* horizontal front porch */
	const unsigned VFP = vdmode->vfp;	/* vertical front porch */
	const unsigned HBP = vdmode->hbp;	/* horizontal back porch */
	const unsigned VBP = vdmode->vbp;	/* vertical back porch */
	/* Accumulated parameters for this display */
//	const unsigned LEFTMARGIN = HSYNC + HBP;	/* horizontal delay before DE start */
//	const unsigned TOPMARGIN = VSYNC + VBP;	/* vertical delay before DE start */
	const unsigned HBLANKING = HBP + HSYNC + HFP;	/* Horizontal Blanking = hsync + hbp + hfp */
	const unsigned VBLANKING = VBP + VSYNC + VFP;	/* Vertical Blanking = vsync + vbp + vfp */
	const unsigned HTOTAL = HBLANKING + WIDTH;	/* horizontal full period */
	const unsigned VTOTAL = VBLANKING + HEIGHT;	/* vertical full period */

//	PRINTF("LEFTMARGIN=%u\n", LEFTMARGIN);
//	PRINTF("TOPMARGIN=%u\n", TOPMARGIN);
//	PRINTF("HTOTAL=%u\n", HTOTAL);
//	PRINTF("VTOTAL=%u\n", VTOTAL);
//	PRINTF("HBLANKING=%u\n", HBLANKING);
//	PRINTF("VBLANKING=%u\n", VBLANKING);
//	PRINTF("HSYNC=%u\n", HSYNC);
//	PRINTF("VSYNC=%u\n", VSYNC);
//	PRINTF("HBP=%u\n", HBP);
//	PRINTF("VBP=%u\n", VBP);

	//PRINTF("start delay=%u\n", (VTOTAL - HEIGHT) / interlace - 5);

#if CPUSTYLE_A64
	// A64
	// See https://groups.google.com/g/linux-sunxi/c/96vcJ1kdXXw
	TCONTV_PTR->TCON_GCTL_REG = 0;
	TCONTV_GINT0_REG = 0;
	TCONTV_PTR->TCON1_CTL_REG =
		//(UINT32_C(1) << 31) |	// TCON1_En
		(interlace == 2) * (UINT32_C(1) << 20) |	// TCON1_CTL_INTERLACE_ENABLE
		ulmin16(31, (VTOTAL - HEIGHT) / interlace - 5) * (UINT32_C(1) << 4) | // Start_Delay
		0;

	TCONTV_PTR->TCON1_BASIC0_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);			// TCON1_XI TCON1_YI
	TCONTV_PTR->TCON1_BASIC1_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);			// LS_XO LS_YO
	TCONTV_PTR->TCON1_BASIC2_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);			// TCON1_XO TCON1_YO
	TCONTV_PTR->TCON1_BASIC3_REG = ((HTOTAL - 1) << 16) | ((HBP - 1) << 0);	// HT HBP
	TCONTV_PTR->TCON1_BASIC4_REG = ((VTOTAL * (3 - interlace)) << 16) | ((VBP - 1) << 0);	// VT VBP
	TCONTV_PTR->TCON1_BASIC5_REG = ((HSYNC - 1) << 16) | ((VSYNC - 1) << 0);	// HSPW VSPW

	TCONTV_PTR->TCON_CEU_CTL_REG &= ~ (UINT32_C(1) << 31);
	TCONTV_PTR->TCON1_IO_POL_REG = 0;	// не влияет
	TCONTV_PTR->TCON1_IO_TRI_REG = 0;	// вличет

	if (TCONTV_IX == 0)
		TCONTV_PTR->TCON_GCTL_REG &= ~ (UINT32_C(1) << 0); // IO_Map_Sel: 0: TCON0, 1: TCON1
	else
		TCONTV_PTR->TCON_GCTL_REG |= (UINT32_C(1) << 0); // IO_Map_Sel: 0: TCON0, 1: TCON1


#elif CPUSTYLE_H3
	// H3
	TCONTV_PTR->TCON_GCTL_REG = 0;
	TCONTV_GINT0_REG = 0;
	TCONTV_PTR->TCON1_CTL_REG =
		//(UINT32_C(1) << 31) |	// TCON1_En
		(interlace == 2) * (UINT32_C(1) << 20) |	// TCON1_CTL_INTERLACE_ENABLE
		ulmin16(31, (VTOTAL - HEIGHT) / interlace - 5) * (UINT32_C(1) << 4) | // Start_Delay
		0;

	TCONTV_PTR->TCON1_BASIC0_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);	// TCON1_XI TCON1_YI
	TCONTV_PTR->TCON1_BASIC1_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);	// LS_XO LS_YO
	TCONTV_PTR->TCON1_BASIC2_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);	// TCON1_XO TCON1_YO
	TCONTV_PTR->TCON1_BASIC3_REG = ((HTOTAL - 1) << 16) | ((HBP - 1) << 0);	// HT HBP
	TCONTV_PTR->TCON1_BASIC4_REG = ((VTOTAL * (3 - interlace)) << 16) | ((VBP - 1) << 0);	// VT VBP
	TCONTV_PTR->TCON1_BASIC5_REG = ((HSYNC - 1) << 16) | ((VSYNC - 1) << 0);			// HSPW VSPW

	TCONTV_PTR->TCON_CEU_CTL_REG &= ~ (UINT32_C(1) << 31);

#elif (CPUSTYLE_T507 || CPUSTYLE_H616)

	TCONTV_PTR->TV_GCTL_REG = 0;
	TCONTV_GINT0_REG = 0;
	TCONTV_PTR->TV_CTL_REG =
		(interlace == 2) * (UINT32_C(1) << 20) |	// TCON1_CTL_INTERLACE_ENABLE
		ulmin16(31, (VTOTAL - HEIGHT) / interlace - 5) * (UINT32_C(1) << 4) | // Start_Delay
		0;

	TCONTV_PTR->TV_BASIC0_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);	// TV_XI TV_YI
	TCONTV_PTR->TV_BASIC1_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);	// LS_XO LS_YO
	TCONTV_PTR->TV_BASIC2_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);	// TV_XO TV_YO
	TCONTV_PTR->TV_BASIC3_REG = ((HTOTAL - 1) << 16) | ((HBP - 1) << 0);	// HT HBP
	TCONTV_PTR->TV_BASIC4_REG = ((VTOTAL * (3 - interlace)) << 16) | ((VBP - 1) << 0);	// VT VBP
	TCONTV_PTR->TV_BASIC5_REG = ((HSYNC - 1) << 16) | ((VSYNC - 1) << 0);			// HSPW VSPW

//	TCONTV_PTR->TV_IO_POL_REG = 0x03000000;
//	TCONTV_PTR->TV_IO_TRI_REG = 0x0cffffff;

	//	 TV_SRC_SEL
	//	 TV Source Select
	//	 000: DE
	//	 001: Color Check
	//	 010: Grayscale Check
	//	 011: Black by White Check
	//	 100: Reserved
	//	 101: Reserved
	//	 111: Gridding Check

	TCONTV_PTR->TV_CEU_CTL_REG &= ~ (UINT32_C(1) << 31);	// едокументированно, но требуется
	TCONTV_PTR->TV_SRC_CTL_REG = 0;             //0 - DE, 1..7 - test
	TCONTV_PTR->TV_GCTL_REG |= (UINT32_C(1) << 1); //enable TCONTV - не документирвано, но без жтого не работает
//	TCONTV_PTR->TV_DATA_IO_TRI0_REG = 0;
//	TCONTV_PTR->TV_DATA_IO_TRI1_REG = 0;

#else
	//#error CPUSTYLE_xxx error
#endif
#endif /* defined (TCONTV_PTR) */
}

// Open module enable
static void t113_open_tcontv_module_enable(void)
{
#if defined (TCONTV_PTR)
#if CPUSTYLE_H3
	TCONTV_PTR->TCON1_CTL_REG |= (UINT32_C(1) << 31);	// LCD_EN
	TCONTV_PTR->TCON_GCTL_REG |= (UINT32_C(1) << 31);	// LCD_EN
	ASSERT(TCONTV_PTR->TCON_GCTL_REG & (UINT32_C(1) << 31));
#elif CPUSTYLE_A64
	TCONTV_PTR->TCON1_CTL_REG |= (UINT32_C(1) << 31);	// LCD_EN
	TCONTV_PTR->TCON_GCTL_REG |= (UINT32_C(1) << 31);	// LCD_EN
	ASSERT(TCONTV_PTR->TCON_GCTL_REG & (UINT32_C(1) << 31));
#else
	TCONTV_PTR->TV_CTL_REG |= (UINT32_C(1) << 31);	// LCD_EN
	TCONTV_PTR->TV_GCTL_REG |= (UINT32_C(1) << 31);	// LCD_EN
	ASSERT(TCONTV_PTR->TV_GCTL_REG & (UINT32_C(1) << 31));
#endif
#endif /* defined (TCONTV_PTR) */
}


static void h3_de2_vsu_init(int rtmixid, const videomode_t * vdmodeDESIGN, const videomode_t * vdmodeHDMI)
{
	DE_VSU_TypeDef * const vsu = de3_getvsu(rtmixid, 1);
	if (vsu == NULL)
		return;

//	ASSERT(DIM_X == vdmodeDESIGN->width);
//	ASSERT(DIM_Y == vdmodeDESIGN->height);
	if (vdmodeDESIGN->width == vdmodeHDMI->width && vdmodeDESIGN->height == vdmodeHDMI->height)
	{
		vsu->VSU_CTRL_REG = 0;	// EN Video Scaler Unit disable
		return;
	}
	enum { FRAСTWIDTH = 19 };	// При масштабе 1:1 о ширине изображения нет - для теста делаю уменьшение на 0.9
	const unsigned HEIGHT = vdmodeHDMI->height;	/* height */
	const unsigned WIDTH = vdmodeHDMI->width;	/* width */
	const uint32_t APPDIMS_SIZE = ((vdmodeDESIGN->height - 1) << 16) | (vdmodeDESIGN->width - 1);	// source size

	const uint_fast32_t HSTEP = (((uint_fast64_t) vdmodeDESIGN->width << FRAСTWIDTH) / WIDTH) << 1;
	const uint_fast32_t VSTEP = (((uint_fast64_t) vdmodeDESIGN->height << FRAСTWIDTH) / HEIGHT) << 1;

	vsu->VSU_CTRL_REG     = (UINT32_C(1) << 30); // CORE_RST
	vsu->VSU_CTRL_REG     = 0*(UINT32_C(1) << 0);	// EN Video Scaler Unit enable

	vsu->VSU_CTRL_REG = (UINT32_C(1) << 0);
	vsu->VSU_OUT_SIZE_REG = ((HEIGHT - 1) << 16) | (WIDTH - 1);
	vsu->VSU_Y_SIZE_REG = APPDIMS_SIZE;
	vsu->VSU_Y_HSTEP_REG = HSTEP;
	vsu->VSU_Y_VSTEP_REG = VSTEP;
	vsu->VSU_C_SIZE_REG = APPDIMS_SIZE;
	vsu->VSU_C_HSTEP_REG = HSTEP;
	vsu->VSU_C_VSTEP_REG = VSTEP;

	for (int n = 0; n < 32; n ++)
	{
		vsu->VSU_Y_HCOEF0_REGN [n] = 0x40000000;	// 0x200
		vsu->VSU_Y_HCOEF1_REGN [n] = 0;			// 0x300
		vsu->VSU_Y_VCOEF_REGN [n] = 0x00004000;	// 0x400
		vsu->VSU_C_HCOEF0_REGN [n] = 0x40000000;	// 0x600
		vsu->VSU_C_HCOEF1_REGN [n] = 0;			// 0x700
		vsu->VSU_C_VCOEF_REGN [n] = 0x00004000;	// 0x800
	}

	vsu->VSU_CTRL_REG = (UINT32_C(1) << 0) | (UINT32_C(1) << 4);
}


static void t113_tcontv_initsteps(const videomode_t * vdmode)
{
#if defined (TCONTV_PTR)
	const uint_fast32_t dotclock = display_getdotclock(vdmode);
	// step0 - CCU configuration
	t113_tcontv_PLL_configuration(dotclock);
	t113_TCONTV_CCU_configuration(dotclock);
	// step1 - same as step1 in HV mode: Select HV interface type
	//t113_select_HV_interface_type(vdmode);
	// step2 - Clock configuration
	//t113_LVDS_clock_configuration(vdmode);
	// step3 - same as step3 in HV mode: Set sequuence parameters
	t113_set_tcontv_sequence_parameters(vdmode);
	// step4 - same as step4 in HV mode: Open volatile output
	t113_tcontv_open_IO_output(vdmode);
	// step5 - set LVDS digital logic configuration
	//t113_set_LVDS_digital_logic(vdmode);
	// step6 - LVDS controller configuration
	//t113_DSI_controller_configuration(vdmode);
	//t113_LVDS_controller_configuration(vdmode, TCONLCD_LVDSIX);
	// step7 - same as step5 in HV mode: Set and open interrupt function
	t113_set_and_open_tcontv_interrupt_function();
	// step8 - same as step6 in HV mode: Open module enable
	t113_open_tcontv_module_enable();
#endif /* defined (TCONTV_PTR) */
}

#if WITHHDMITVHW

#endif

static void t113_hdmi_init(const videomode_t * vdmode)
{
	HDMI_TX_TypeDef * const hdmi = HDMI_TX0;
#if WITHHDMITVHW
	const uint_fast32_t dotclock = hdmi_realclock(vdmode);

#if CPUSTYLE_T507 || CPUSTYLE_H616
	t507_hdmi_phy_init(dotclock);
#else
	h3_hdmi_phy_init(dotclock);
#endif


	static const unsigned int n_table[21] = {
		32000, 3072, 4096, 44100, 4704, 6272, 88200, 4704 * 2, 6272 * 2,
		176400, 4704 * 4, 6272 * 4, 48000, 5120, 6144, 96000, 5120 * 2,
		6144 * 2, 192000, 5120 * 4, 6144 * 4,
	};

//	PRINTF("hdmi->HDMI_CONFIG3_ID=%02X, HDMI_CONFIG3_AHBAUDDMA=%02X\n", hdmi->HDMI_CONFIG3_ID, HDMI_CONFIG3_AHBAUDDMA);
//	PRINTF("hdmi->HDMI_CONFIG0_ID=%02X, HDMI_CONFIG0_I2S=%02X\n", hdmi->HDMI_CONFIG0_ID, HDMI_CONFIG0_I2S);

//	hdmi_write(0xE04B, 0x00 | (audio->sample_bit == 16)
//			? 0x02 : ((audio->sample_bit == 24) ? 0xb : 0x0));
//	hdmi_write(0x0251, audio->sample_bit);
//	n = 6272;
//
//	for (i = 0; i < 21; i += 3) {
//		if (audio->sample_rate == n_table[i]) {
//			if (ptbl[id].para[1] == 1)
//				n = n_table[i + 1];
//			else
//				n = n_table[i + 2];
///* cts = (n / 128) * (glb_video.tmds_clk / 100) / (audio->sample_rate / 100); */
//			    break;
//		}
//	}
	/* cts = (n / 128) * (glb_video.tmds_clk / 100) / (audio->sample_rate / 100); */
	unsigned audio_cts = 10;
	const int_fast32_t samplerate = dsp_get_sampleraterx();
	unsigned audio_n = hdmi_compute_n(samplerate, dotclock);
	/*
	 * Compute the CTS value from the N value.  Note that CTS and N
	 * can be up to 20 bits in total, so we need 64-bit math.  Also
	 * note that our TDMS clock is not fully accurate; it is accurate
	 * to kHz.  This can introduce an unnecessary remainder in the
	 * calculation below, so we don't try to warn about that.
	 */
	uint_fast64_t tmp = (uint_fast64_t)dotclock /*ftdms*/ * audio_n;
	tmp /= 128 * samplerate;
	audio_cts = tmp;
	// hdmi audio CTS=74250, N=6144
	//PRINTF("hdmi audio CTS=%u, N=%u\n", audio_cts, audio_n);

	h3_hdmi_init(vdmode);
	hdmi_enable_video_path(hdmi, 1);
	hdmi_video_sample(hdmi);
	hdmi_tx_hdcp_config(hdmi);
	// audio enable
	hdmi_set_cts_n(hdmi, audio_cts, audio_n);
	hdmi_enable_audio_clk(hdmi);
	dw_hdmi_i2s_setup(hdmi);
	dw_hdmi_clear_overflow(hdmi);
//	t507_hdmi_edid_test();

#endif /* WITHHDMITVHW */
}

static void t113_tcon_lvds_initsteps(const videomode_t * vdmode)
{
#if defined (TCONLCD_PTR)
	const uint_fast32_t lvdsfreq = display_getdotclock(vdmode) * 7;
	// step0 - CCU configuration
	t113_tconlvds_PLL_configuration(lvdsfreq);
	t113_tconlvds_CCU_configuration(lvdsfreq);
	// step1 - same as step1 in HV mode: Select HV interface type
	t113_select_HV_interface_type(vdmode);
	// step2 - Clock configuration
	t113_LVDS_clock_configuration(vdmode);
	// step3 - same as step3 in HV mode: Set sequuence parameters
	t113_set_sequence_parameters(vdmode);
	// step4 - same as step4 in HV mode: Open volatile output
	t113_open_IO_output(vdmode);
	// step5 - set LVDS digital logic configuration
	t113_set_LVDS_digital_logic(vdmode);
	// step6 - LVDS controller configuration
	t113_DSI_controller_configuration(vdmode);	// t113 требует инициализации DSI_COMBO_PHY
	t113_LVDS_controller_configuration(vdmode, TCONLCD_LVDSIX);
	// step7 - same as step5 in HV mode: Set and open interrupt function
	t113_set_and_open_interrupt_function();
	// step8 - same as step6 in HV mode: Open module enable
	t113_open_module_enable(vdmode);

#endif /* defined (TCONLCD_PTR) */
}

#if WITHMIPIDSISHW

//	disp 0, clk: pll(792000000),clk(792000000),dclk(33000000) dsi_rate(33000000)
//	clk real:pll(792000000),clk(792000000),dclk(198000000) dsi_rate(150000000)

// What is DPSS_TOP_BGR_REG ?
static void t113_tcon_dsi_initsteps(const videomode_t * vdmode)
{
	unsigned pixdepth = 24;
	const unsigned nlanes = WITHMIPIDSISHW_LANES;
	const unsigned onepixelclocks = pixdepth / nlanes;
	const uint_fast32_t dsifreq = display_getdotclock(vdmode) * pixdepth / nlanes;
	PRINTF("t113_tcon_dsi_initsteps: dsifreq=%" PRIuFAST32 " MHz, lanes=%u, depth=%u, pixelclock=%" PRIuFAST32 " MHz\n", dsifreq / 1000 / 1000, nlanes, pixdepth, display_getdotclock(vdmode) / 1000 / 1000);
	// step0 - CCU configuration
	t113_tcondsi_CCU_configuration(vdmode, dsifreq);
	// step1 - same as step1 in HV mode: Select HV interface type
	t113_select_HV_interface_type(vdmode);
	// step2 - Clock configuration
	t113_MIPIDSI_clock_configuration(vdmode, onepixelclocks);
	// step3 - same as step3 in HV mode: Set sequuence parameters
	t113_set_sequence_parameters(vdmode);
	// step4 - same as step4 in HV mode: Open volatile output
	t113_open_IO_output(vdmode);
	// step5 - set LVDS digital logic configuration
	t113_set_LVDS_digital_logic(vdmode);
	// step6 - LVDS controller configuration
#if CPUSTYLE_T507 || CPUSTYLE_H616
	// These CPUs not support DSI at all

#elif CPUSTYLE_T113 || CPUSTYLE_F133

	(void) DSI0->DSI_CTL;

#else

#endif
	t113_DSI_controller_configuration(vdmode);
	t113_LVDS_controller_configuration(vdmode, TCONLCD_LVDSIX);
	// step7 - same as step5 in HV mode: Set and open interrupt function
	t113_set_and_open_interrupt_function();
	// step8 - same as step6 in HV mode: Open module enable
	t113_open_module_enable(vdmode);

}

#else // HW


// H3: PLL_VIDEO
// A64: PLL_VIDEO0
// T113: PLL_VIDEO1
// T507: PLL_VIDEO1
static void t113_tcon_PLL_configuration(void)
{
#if CPUSTYLE_H3
	// Set up shared and dedicated clocks for HDMI, LCD/TCON and DE2
	CCU->PLL_VIDEO_CTRL_REG = (UINT32_C(1) << 31) | (UINT32_C(1) << 25) | (UINT32_C(1) << 24) | ((99 - 1) * (UINT32_C(1) << 8)) | ((8 - 1) * (UINT32_C(1) << 0)); // 297MHz
	while ((CCU->PLL_VIDEO_CTRL_REG & (UINT32_C(1) << 28)) == 0)	 //Wait pll stable
		;
	local_delay_ms(50);

#elif CPUSTYLE_A64

	CCU->PLL_VIDEO0_CTRL_REG = (UINT32_C(1) << 31) | (UINT32_C(1) << 25) | (UINT32_C(1) << 24) | ((99 - 1) * (UINT32_C(1) << 8)) | ((8 - 1) * (UINT32_C(1) << 0)); // 297MHz
	while ((CCU->PLL_VIDEO0_CTRL_REG & (UINT32_C(1) << 28)) == 0)	 //Wait pll stable
		;
	local_delay_ms(50);

#elif CPUSTYLE_T507 || CPUSTYLE_H616

	// не меняем параметры по умолчанию (частота может поменяться для LVDS)
	CCU->PLL_VIDEO1_CTRL_REG |= (UINT32_C(1) << 31) | (UINT32_C(1) << 30);

	/* Lock enable */
	CCU->PLL_VIDEO1_CTRL_REG |= (UINT32_C(1) << 29);

	/* Wait pll stable */
	while (! (CCU->PLL_VIDEO1_CTRL_REG & (UINT32_C(1) << 28)))
		;

#elif CPUSTYLE_T113 || CPUSTYLE_F133

	allwnr_t113_module_pll_spr(& CCU->PLL_VIDEO1_CTRL_REG, & CCU->PLL_VIDEO1_PAT0_CTRL_REG);	// Set Spread Frequency Mode
	allwnr_t113_module_pll_enable(& CCU->PLL_VIDEO1_CTRL_REG);	// не меняем параметры по умолчанию (частота может поменяться для LVDS)

#else
#endif
}

// Paraleal RGB mode, HDMI mode
static void t113_tcon_hw_initsteps(const videomode_t * vdmode)
{
	// step0 - CCU configuration
	t113_tcon_PLL_configuration();
	t113_tconlcd_CCU_configuration();
	// step1 - Select HV interface type
	t113_select_HV_interface_type(vdmode);
	// step2 - Clock configuration
	t113_HV_clock_configuration(vdmode);
	// step3 - Set sequuence parameters
	t113_set_sequence_parameters(vdmode);
	// step4 - Open volatile output
	t113_open_IO_output(vdmode);
	// step5 - Set and open interrupt function
	t113_set_and_open_interrupt_function();
	// step6 - Open module enable
	t113_open_module_enable(vdmode);
}
#endif

static void t113_hdmi_initsteps(const videomode_t * vdmode)
{
	t113_tcontv_initsteps(vdmode);
	t113_hdmi_init(vdmode);
}

static void hardware_tcon_initsteps(const videomode_t * vdmode)
{
#if WITHLVDSHW
	t113_tcon_lvds_initsteps(vdmode);
#elif WITHMIPIDSISHW
	t113_tcon_dsi_initsteps(vdmode);
#else /* WITHLVDSHW */
	t113_tcon_hw_initsteps(vdmode);	// HW & HDMI
#endif /* WITHLVDSHW */
}

void hardware_ltdc_initialize(const videomode_t * vdmodeX)
{
    //PRINTF("hardware_ltdc_initialize\n");

	typedef struct initstruct_tag
	{
		int rtmixid;
		const videomode_t * (* vdmodef)(void);
		uint_least32_t defcolor;
		void (* tcon_init)(const videomode_t * vdmode);
	} initstruct_t;

	static const initstruct_t initstructs [] =
	{
#if defined RTMIXIDLCD
		{ RTMIXIDLCD, get_videomode_DESIGN, COLOR24(0, 255, 0), hardware_tcon_initsteps, },
#endif /* RTMIXIDLCD */
#if WITHHDMITVHW
		{ RTMIXIDTV, get_videomode_HDMI, COLOR24(255, 0, 0), t113_hdmi_initsteps, },
#endif /* WITHHDMITVHW */
	};

	unsigned i;
	for (i = 0; i < ARRAY_SIZE(initstructs); ++ i)
	{
		int rtmixid = initstructs [i].rtmixid;
		const videomode_t * const vdmode = initstructs [i].vdmodef();

	 	hardware_de_initialize(vdmode, rtmixid);
	 	initstructs [i].tcon_init(vdmode);
		ltdc_tfcon_cfg(vdmode);	// Set DE MODE if need, mapping GPIO pins
		t113_de_rtmix_initialize(rtmixid);
		awxx_deoutmapping();				// после инициализации и TCON и DE

		//PRINTF("Init rtmixid=%d\n", rtmixid);
		//TP();
		/* эта инициализация после корректного соединения с работающим TCON */
		t113_de_bld_initialize(rtmixid, vdmode, initstructs [i].defcolor);	// RED
		//TP();

		// проверка различных scalers
	#if 1
		h3_de2_vsu_init(rtmixid, get_videomode_DESIGN(), vdmode);
	#else
    	// On T507 defectiveimafe
		// On H3 - HSUB=1 VSUB=1 IS_DE3=0 or IS_DE3=1 - work
		// On A64
		t113_vi_scaler_setup(rtmixid, get_videomode_DESIGN(), vdmode);
	#endif

		// save settings
		t113_de_update(rtmixid);	/* Update registers */
		//PRINTF("Init rtmixid=%d done\n", rtmixid);
	}

    //PRINTF("hardware_ltdc_initialize done.\n");
}

void
hardware_ltdc_deinitialize(void)
{
}

/* Set MAIN frame buffer address. Waiting for VSYNC. */
void hardware_ltdc_main_set4(int rtmixid, uintptr_t layer0, uintptr_t layer1, uintptr_t layer2, uintptr_t layer3)
{
	DE_BLD_TypeDef * const bld = de3_getbld(rtmixid);
	if (bld == NULL)
		return;

	// Note: the layer priority is layer3>layer2>layer1>layer0
	t113_de_set_address_vi(rtmixid, layer0, 1);	// VI1
	t113_de_set_address_ui(rtmixid, layer1, 1);	// UI1
	t113_de_set_address_ui(rtmixid, layer2, 2);	// UI2
	t113_de_set_address_ui(rtmixid, layer3, 3);	// UI3

	//DE_TOP->DE_PORT2CHN_MUX [0] = 0x0000A980;

	bld->BLD_EN_COLOR_CTL =
		((de3_getvi(rtmixid, 1) != NULL) * (layer0 != 0) * VI_POS_BIT(rtmixid, 1))	| // pipe0 enable - from VI1
		((de3_getui(rtmixid, 1) != NULL) * (layer1 != 0) * UI_POS_BIT(rtmixid, 1))	| // pipe1 enable - from UI1
		((de3_getui(rtmixid, 2) != NULL) * (layer2 != 0) * UI_POS_BIT(rtmixid, 2))	| // pipe1 enable - from UI2
		((de3_getui(rtmixid, 3) != NULL) * (layer3 != 0) * UI_POS_BIT(rtmixid, 3))	| // pipe1 enable - from UI3
		0;

	hardware_ltdc_vsync(rtmixid);		/* ожидаем начало кадра */
	t113_de_update(rtmixid);	/* Update registers */
}

/* set visible buffer start. Wait VSYNC. */
void hardware_ltdc_main_set(int rtmixid, uintptr_t p1)
{
	DE_BLD_TypeDef * const bld = de3_getbld(rtmixid);
	if (bld == NULL)
		return;
	t113_de_set_address_vi(rtmixid, p1, 1);
	//t113_de_set_address_ui(rtmixid, p1, 1);

	bld->BLD_EN_COLOR_CTL =
			((de3_getvi(rtmixid, 1) != NULL) * (p1 != 0) * VI_POS_BIT(rtmixid, 1))	| // pipe0 enable - from VI1
			//((de3_getui(rtmixid, 1) != NULL) * (p1 != 0) * UI_POS_BIT(rtmixid, 1))	| // pipe1 enable - from UI1
			0;

	hardware_ltdc_vsync(rtmixid);		/* ожидаем начало кадра */
	t113_de_update(rtmixid);	/* Update registers */
}

/* Set MAIN frame buffer address. No waiting for VSYNC. */
void hardware_ltdc_main_set_no_vsync(int rtmixid, uintptr_t p1)
{
	const int vich = 1;
	DE_BLD_TypeDef * const bld = de3_getbld(rtmixid);
	if (bld == NULL)
		return;

	t113_de_set_address_vi(rtmixid, p1, vich);
	// 5.10.9.1 BLD fill color control register
	// BLD_FILL_COLOR_CTL
	bld->BLD_EN_COLOR_CTL =
			((de3_getvi(rtmixid, vich) != NULL) * (p1 != 0) * VI_POS_BIT(rtmixid, vich)) |
		0;
	t113_de_update(rtmixid);	/* Update registers */
}

/* Palette reload */
void hardware_ltdc_L8_palette(void)
{
}

#else
	//#error Wrong CPUSTYLE_xxxx

void hardware_ltdc_initialize(const videomode_t * vdmode)
{
}

/* Set MAIN frame buffer address. No waiting for VSYNC. */
void hardware_ltdc_main_set_no_vsync(int rtmixid, uintptr_t addr)
{
}

/* set visible buffer start. Wait VSYNC. */
void hardware_ltdc_main_set(int rtmixid, uintptr_t p)
{
}

/* Set MAIN frame buffer address. Waiting for VSYNC. */
void hardware_ltdc_main_set4(uintptr_t layer0, uintptr_t layer1, uintptr_t layer2, uintptr_t layer3)
{
}

/* ожидаем начало кадра */
static void hardware_ltdc_vsync(int rtmixid)
{
}

/* Palette reload */
void hardware_ltdc_L8_palette(void)
{
}

#endif /* CPUSTYLE_STM32F || CPUSTYLE_STM32MP1 */

#else /* WITHLTDCHW */

#endif /* WITHLTDCHW */
