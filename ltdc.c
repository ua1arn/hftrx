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

#include "display/display.h"
#include <stdint.h>
#include <string.h>

#include "formats.h"	// for debug prints
#include "inc/spi.h"

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

		HFULL = HSYNC + HBP + WIDTH + HFP,	/* horizontal full period */
		VFULL = VSYNC + VBP + HEIGHT + VFP,	/* vertical full period */

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

		HFULL = LEFTMARGIN + WIDTH + HFP,	/* horizontal full period */
		VFULL = TOPMARGIN + HEIGHT + VFP,	/* vertical full period */

		// MODE: DE/SYNC mode select.
		// DE MODE: MODE="1", VS and HS must pull high.
		// SYNC MODE: MODE="0". DE must be grounded
		VSYNCNEG = 1,			/* Negative polarity required for VSYNC signal */
		HSYNCNEG = 1,			/* Negative polarity required for HSYNC signal */
		DENEG = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
		BOARD_DEMODE = 0		/* 0: static signal, 1: DE controlled */
	};

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

		HFULL = LEFTMARGIN + WIDTH + HFP,	/* horizontal full period */
		VFULL = TOPMARGIN + HEIGHT + VFP,	/* vertical full period */

		// MODE: DE/SYNC mode select.
		// DE MODE: MODE="1", VS and HS must pull high.
		// SYNC MODE: MODE="0". DE must be grounded
		VSYNCNEG = 1,			/* Negative polarity required for VSYNC signal */
		HSYNCNEG = 1,			/* Negative polarity required for HSYNC signal */
		DENEG = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
		BOARD_DEMODE = 0		/* 0: static signal, 1: DE controlled */
	};

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

		HFULL = HSYNC + HBP + WIDTH + HFP,	/* horizontal full period */
		VFULL = VSYNC + VBP + HEIGHT + VFP,	/* vertical full period */

		VSYNCNEG = 1,			/* Negative polarity required for VSYNC signal */
		HSYNCNEG = 1,			/* Negative polarity required for HSYNC signal */
		DENEG = 0,				/* DE polarity: (normal: DE is 0 while sync) */
		BOARD_DEMODE = 0		/* 0: static signal, 1: DE controlled */
	};
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

		HFULL = HSYNC + HBP + WIDTH + HFP,	/* horizontal full period */
		VFULL = VSYNC + VBP + HEIGHT + VFP,	/* vertical full period */

		VSYNCNEG = 1,			/* Negative polarity required for VSYNC signal */
		HSYNCNEG = 1,			/* Negative polarity required for HSYNC signal */
		DENEG = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
		BOARD_DEMODE = 0		/* 0: static signal, 1: DE controlled */
	};
#else
	#error Unsupported LCDMODE_xxx
#endif

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
			debug_printf_P(PSTR("wait reg=%p %s mask=%08lX, stay=%08lX\n"), reg, label, mask, * reg & mask);
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
		if (count -- == 0) {debug_printf_P(PSTR("wait %s/%d\n"), __FILE__, __LINE__); break; } \
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
	local_delay_ms(0); \
} while (0)


/************************************************************************/

#if LCDMODE_LTDC_L8

static void
VDC5_fillLUT_L8(
	volatile uint32_t * reg
	)
{
	unsigned color;

	for (color = 0; color < 256; ++ color)
	{
#define XRGB(zr,zg,zb) do { r = (zr), g = (zg), b = (zb); } while (0)
		uint_fast8_t r, g, b;

		switch (color)
		{
		case TFTRGB(0, 0, 0)			/*COLOR_BLACK*/:		XRGB(0, 0, 0);			break;	// 0x00 черный
		case TFTRGB(255, 0, 0)			/*COLOR_RED*/:			XRGB(255, 0, 0);		break; 	// 0xE0 красный
		case TFTRGB(0, 255, 0)			/*COLOR_GREEN*/:		XRGB(0, 255, 0);		break; 	// 0x1C зеленый
		case TFTRGB(0, 0, 255)			/*COLOR_BLUE*/:			XRGB(0, 0, 255);		break; 	// 0x03 синий
		case TFTRGB(128, 0, 0)			/*COLOR_DARKRED*/:		XRGB(128, 0, 0);		break; 	// 
		case TFTRGB(0, 128, 0)			/*COLOR_DARKGREEN*/:	XRGB(0, 128, 0);		break; 	// 
		case TFTRGB(0, 0, 128)			/*COLOR_DARKBLUE*/:		XRGB(0, 0, 128);		break; 	// 
		case TFTRGB(255, 255, 0)		/*COLOR_YELLOW*/:		XRGB(255, 255, 0);		break; 	// 0xFC желтый
		case TFTRGB(255, 0, 255)		/*COLOR_MAGENTA*/:		XRGB(255, 0, 255);		break; 	// 0x83 пурпурный
		case TFTRGB(0, 255, 255)		/*COLOR_CYAN*/:			XRGB(0, 255, 255);		break; 	// 0x1F голубой
		case TFTRGB(255, 255, 255)		/*COLOR_WHITE*/:		XRGB(255, 255, 255);	break;  // 0xff	белый
		case TFTRGB(128, 128, 128)		/*COLOR_GRAY*/:			XRGB(128, 128, 128);	break; 	// серый
		case TFTRGB(0xa5, 0x2a, 0x2a)	/*COLOR_BROWN*/:		XRGB(0xa5, 0x2a, 0x2a);	break; 	// 0x64 коричневый
		case TFTRGB(0xff, 0xd7, 0x00)	/*COLOR_GOLD*/:			XRGB(0xff, 0xd7, 0x00);	break; 	// 0xF4 золото
		case TFTRGB(0xd1, 0xe2, 0x31)	/*COLOR_PEAR*/:			XRGB(0xd1, 0xe2, 0x31);	break; 	// 0xDC грушевый
#undef XRGB
		default:
			r = ((color & 0xe0) << 0) | ((color & 0x80) ? 0x1f : 0);	// red
			g = ((color & 0x1c) << 3) | ((color & 0x10) ? 0x1f : 0);	// green
			b = ((color & 0x03) << 6) | ((color & 0x02) ? 0x3f : 0);	// blue
			break;
		}
		/* запись значений в регистры палитры */
		SETREG32_CK(reg + color, 8, 24, 0x00);	// alpha
		SETREG32_CK(reg + color, 8, 16, r);
		SETREG32_CK(reg + color, 8, 8, g);
		SETREG32_CK(reg + color, 8, 0, b);
	}
}

#endif /* LCDMODE_LTDC_L8 */

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
	SETREG32_CK(& vdc->SYSCNT_PANEL_CLK, 6, 0, calcdivround2(P1CLOCK_FREQ, LTDC_DOTCLK));	/* Clock Frequency Division Ratio Note: Settings other than those in Table 35.5 are prohibited. */
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

#if 0
	struct fb_videomode *mode = priv->videomode;
	uint32_t tmp;

	/* (TODO) Freq. vsync masking and missing vsync
	 * compensation are not supported.
	 */
	vdc5fb_write(priv, SC0_SCL0_FRC1, 0);
	vdc5fb_write(priv, SC0_SCL0_FRC2, 0);
	vdc5fb_write(priv, SC1_SCL0_FRC1, 0);
	vdc5fb_write(priv, SC1_SCL0_FRC2, 0);
#ifdef OUTPUT_IMAGE_GENERATOR
	vdc5fb_write(priv, OIR_SCL0_FRC1, 0);
	vdc5fb_write(priv, OIR_SCL0_FRC2, 0);
#endif

	/* Set the same free-running hsync/vsync period to
	 * all scalers (sc0, sc1 and oir). The hsync/vsync
	 * from scaler 0 is used by all scalers.
	 * (TODO) External input vsync is not supported.
	 */
	tmp = SC_RES_FH(priv->res_fh);
	tmp |= SC_RES_FV(priv->res_fv);
	vdc5fb_write(priv, SC0_SCL0_FRC4, tmp);
	vdc5fb_write(priv, SC1_SCL0_FRC4, tmp);
#ifdef OUTPUT_IMAGE_GENERATOR
	vdc5fb_write(priv, OIR_SCL0_FRC4, tmp);
#endif
	tmp = (SC_RES_FLD_DLY_SEL | SC_RES_VSDLY(1));
	vdc5fb_write(priv, SC0_SCL0_FRC5, tmp);
	vdc5fb_write(priv, SC1_SCL0_FRC5, tmp);
	tmp = SC_RES_VSDLY(1);
#ifdef OUTPUT_IMAGE_GENERATOR
	vdc5fb_write(priv, OIR_SCL0_FRC5, tmp);
#endif

	vdc5fb_write(priv, SC0_SCL0_FRC3, SC_RES_VS_SEL);
	vdc5fb_write(priv, SC1_SCL0_FRC3, (SC_RES_VS_SEL | SC_RES_VS_IN_SEL));
#ifdef OUTPUT_IMAGE_GENERATOR
	vdc5fb_write(priv, OIR_SCL0_FRC3, 0);
#endif
	/* Note that OIR is not enabled here */

	/* Set full-screen size */
	tmp = SC_RES_F_VW(priv->panel_pixel_yres);
	tmp |= SC_RES_F_VS(mode->vsync_len + mode->upper_margin);
	vdc5fb_write(priv, SC0_SCL0_FRC6, tmp);
	vdc5fb_write(priv, SC1_SCL0_FRC6, tmp);
#ifdef OUTPUT_IMAGE_GENERATOR
	vdc5fb_write(priv, OIR_SCL0_FRC6, tmp);
#endif
	tmp = SC_RES_F_HW(priv->panel_pixel_xres);
	tmp |= SC_RES_F_HS(mode->hsync_len + mode->left_margin);
	vdc5fb_write(priv, SC0_SCL0_FRC7, tmp);
	vdc5fb_write(priv, SC1_SCL0_FRC7, tmp);

#ifdef OUTPUT_IMAGE_GENERATOR
	vdc5fb_write(priv, OIR_SCL0_FRC7, tmp);
#endif
	/* Cascade on */
	vdc5fb_setbits(priv, GR1_AB1, GR1_CUS_CON_ON);
	/* Set GR0 as current, GR1 as underlaying */
	/* Set GR1 as current, GR0 as underlaying */
	tmp = vdc5fb_read(priv, GR_VIN_AB1);
	tmp &= ~GR_VIN_SCL_UND_SEL;
	vdc5fb_write(priv, GR_VIN_AB1, tmp);

	/* Do update here. */
	tmp = (SC_SCL0_UPDATE | SC_SCL0_VEN_B);
	vdc5fb_update_regs(priv, SC0_SCL0_UPDATE, tmp, 1);
	vdc5fb_update_regs(priv, SC1_SCL0_UPDATE, tmp, 1);
#ifdef OUTPUT_IMAGE_GENERATOR
	vdc5fb_update_regs(priv, OIR_SCL0_UPDATE, tmp, 1);
#endif
	tmp = (GR_UPDATE | GR_P_VEN);
	vdc5fb_update_regs(priv, GR1_UPDATE, tmp, 1);
	vdc5fb_update_regs(priv, GR_VIN_UPDATE, tmp, 1);

#endif
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


#if 0
	struct fb_videomode *mode = priv->videomode;
	uint32_t tmp;

	/* Enable and setup scaler 0 */
	if( 0 /*priv->pdata->layers[0].xres */) {
		vdc5fb_write(priv, SC0_SCL0_FRC3, SC_RES_VS_SEL);
		vdc5fb_update_regs(priv, SC0_SCL0_UPDATE, SC_SCL0_UPDATE, 1);

		vdc5fb_write(priv, SC0_SCL0_DS1, 0);
		vdc5fb_write(priv, SC0_SCL0_US1, 0);
		vdc5fb_write(priv, SC0_SCL0_OVR1, D_SC_RES_BK_COL);

		tmp = (mode->vsync_len + mode->upper_margin - 1) << 16;
		tmp |= mode->yres;
		vdc5fb_write(priv, SC0_SCL0_DS2, tmp);
		vdc5fb_write(priv, SC0_SCL0_US2, tmp);

		tmp = (mode->hsync_len + mode->left_margin) << 16;
		tmp |= mode->xres;
		vdc5fb_write(priv, SC0_SCL0_DS3, tmp);
		vdc5fb_write(priv, SC0_SCL0_US3, tmp);

		tmp = mode->yres << 16;
		tmp |= mode->xres;
		vdc5fb_write(priv, SC0_SCL0_DS7, tmp);

		tmp = SC_RES_IBUS_SYNC_SEL;
		vdc5fb_write(priv, SC0_SCL0_US8, tmp);
		vdc5fb_write(priv, SC0_SCL0_OVR1, D_SC_RES_BK_COL);
	}
	else {
		/* Disable scaler 0 */
		vdc5fb_write(priv, SC0_SCL0_DS1, 0);
		vdc5fb_write(priv, SC0_SCL0_US1, 0);
		vdc5fb_write(priv, SC0_SCL0_OVR1, D_SC_RES_BK_COL);
	}

	/* Enable and setup scaler 1 */
	if( 0 /*priv->pdata->layers[1].xres */) {
		// RZA1H only
		vdc5fb_write(priv, SC1_SCL0_FRC3, SC_RES_VS_SEL);
		vdc5fb_update_regs(priv, SC1_SCL0_UPDATE, SC_SCL0_UPDATE, 1);

`		vdc5fb_write(priv, SC1_SCL0_DS1, 0);
		vdc5fb_write(priv, SC1_SCL0_US1, 0);
		vdc5fb_write(priv, SC1_SCL0_OVR1, D_SC_RES_BK_COL);

		tmp = (mode->vsync_len + mode->upper_margin - 1) << 16;
		tmp |= mode->yres;
		vdc5fb_write(priv, SC1_SCL0_DS2, tmp);
		vdc5fb_write(priv, SC1_SCL0_US2, tmp);

		tmp = (mode->hsync_len + mode->left_margin) << 16;
		tmp |= mode->xres;
		vdc5fb_write(priv, SC1_SCL0_DS3, tmp);
		vdc5fb_write(priv, SC1_SCL0_US3, tmp);

		tmp = mode->yres << 16;
		tmp |= mode->xres;
		vdc5fb_write(priv, SC1_SCL0_DS7, tmp);

		tmp = SC_RES_IBUS_SYNC_SEL;
		vdc5fb_write(priv, SC1_SCL0_US8, tmp);
		vdc5fb_write(priv, SC1_SCL0_OVR1, D_SC_RES_BK_COL);
	}
	else {
		/* Disable scaler 1 */
		vdc5fb_write(priv, SC1_SCL0_DS1, 0);
		vdc5fb_write(priv, SC1_SCL0_US1, 0);
		vdc5fb_write(priv, SC1_SCL0_OVR1, D_SC_RES_BK_COL);
	}

	/* Enable and setup OIR scaler */
#ifdef OUTPUT_IMAGE_GENERATOR
	vdc5fb_write(priv, OIR_SCL0_FRC3, OIR_RES_EN);
	vdc5fb_update_regs(priv, OIR_SCL0_UPDATE, SC_SCL_UPDATE, 1);

	vdc5fb_write(priv, OIR_SCL0_DS1, 0);
	vdc5fb_write(priv, OIR_SCL0_US1, 0);
	vdc5fb_write(priv, OIR_SCL0_OVR1, D_SC_RES_BK_COL);

	tmp = (mode->vsync_len + mode->upper_margin - 1) << 16;
	tmp |= mode->yres;
	vdc5fb_write(priv, OIR_SCL0_DS2, tmp);
	vdc5fb_write(priv, OIR_SCL0_US2, tmp);

	tmp = (mode->hsync_len + mode->left_margin) << 16;
	tmp |= mode->xres;
	vdc5fb_write(priv, OIR_SCL0_DS3, tmp);
	vdc5fb_write(priv, OIR_SCL0_US3, tmp);

	tmp = mode->yres << 16;
	tmp |= mode->xres;
	vdc5fb_write(priv, OIR_SCL0_DS7, tmp);

	tmp = SC_RES_IBUS_SYNC_SEL;
	vdc5fb_write(priv, OIR_SCL0_US8, tmp);
#endif
#endif
}

#if LCDMODE_LTDC_PIP16

/* set bottom buffer start */
void arm_hardware_ltdc_pip_set(uintptr_t p)
{
	struct st_vdc5 * const vdc = & VDC50;

	SETREG32_CK(& vdc->GR3_FLM_RD, 1, 0, 1);			// GR3_R_ENB Frame Buffer Read Enable 1: Frame buffer reading is enabled.
	SETREG32_CK(& vdc->GR3_FLM2, 32, 0, p);			// GR3_BASE
	SETREG32_CK(& vdc->GR3_AB1, 2, 0,	0x03);			// GR3_DISP_SEL 3: Blended display of lower-layer graphics and current graphics

	// GR3_IBUS_VEN in GR3_UPDATE is 1.
	// GR3_IBUS_VEN and GR3_P_VEN in GR3_UPDATE are 1.
	// GR3_P_VEN in GR3_UPDATE is 1.

	//vdc5_update(& vdc->GR3_UPDATE, "GR3_UPDATE",
		vdc->GR3_UPDATE = (
		//	(1 << 8) |	// GR3_UPDATE Frame Buffer Read Control Register Update
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
		//	(1 << 8) |	// GR3_UPDATE Frame Buffer Read Control Register Update
			(1 << 4) |	// GR3_P_VEN Graphics Display Register Update
			(1 << 0) |	// GR3_IBUS_VEN Frame Buffer Read Control Register Update
			0
		);
}

#endif /* LCDMODE_LTDC_PIP16 */

static void vdc5fb_init_graphics(struct st_vdc5 * const vdc)
{

	const unsigned ROWSIZE = sizeof framebuff [0];	// размер одной строки в байтах

#if LCDMODE_LTDC_L8
	const unsigned grx_format_MAIN = 0x05;	// GRx_FORMAT 5: CLUT8
	const unsigned grx_rdswa_MAIN = 0x07;	// GRx_RDSWA 111: (8) (7) (6) (5) (4) (3) (2) (1) [32-bit swap + 16-bit swap + 8-bit swap]
#else /* LCDMODE_LTDC_L8 */
	const unsigned grx_format_MAIN = 0x00;	// GRx_FORMAT 0: RGB565
	const unsigned grx_rdswa_MAIN = 0x06;	// GRx_RDSWA 110: (7) (8) (5) (6) (3) (4) (1) (2) [32-bit swap + 16-bit swap]
#endif /* LCDMODE_LTDC_L8 */
	const unsigned grx_format_PIP = 0x00;	// GRx_FORMAT 0: RGB565
	const unsigned grx_rdswa_PIP = 0x06;	// GRx_RDSWA 110: (7) (8) (5) (6) (3) (4) (1) (2) [32-bit swap + 16-bit swap]

	////////////////////////////////////////////////////////////////
	// GR0
	SETREG32_CK(& vdc->GR0_FLM_RD, 1, 0, 0);	// GR0_R_ENB Frame Buffer Read Enable
	SETREG32_CK(& vdc->GR0_FLM1, 2, 8, 0x01);	// GR0_FLM_SEL 1: Selects GR0_FLM_NUM.
	SETREG32_CK(& vdc->GR0_FLM2, 32, 0, (uintptr_t) & framebuff);	// GR0_BASE
	SETREG32_CK(& vdc->GR0_FLM3, 15, 16, ROWSIZE);	// GR0_LN_OFF
	SETREG32_CK(& vdc->GR0_FLM3, 10, 0, 0x00);	// GR0_FLM_NUM
	SETREG32_CK(& vdc->GR0_FLM4, 23, 0, ROWSIZE * HEIGHT);	// GR0_FLM_OFF
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

	SETREG32_CK(& vdc->GR2_FLM_RD, 1, 0, 1);	// GR2_R_ENB Frame Buffer Read Enable
	SETREG32_CK(& vdc->GR2_FLM1, 2, 8, 0x01);	// GR2_FLM_SEL 1: Selects GR2_FLM_NUM.
	SETREG32_CK(& vdc->GR2_FLM2, 32, 0, (uintptr_t) & framebuff);	// GR2_BASE
	SETREG32_CK(& vdc->GR2_FLM3, 15, 16, ROWSIZE);	// GR2_LN_OFF
	SETREG32_CK(& vdc->GR2_FLM3, 10, 0, 0x00);	// GR0_FLM_NUM
	SETREG32_CK(& vdc->GR2_FLM4, 23, 0, ROWSIZE * HEIGHT);	// GR2_FLM_OFF
	SETREG32_CK(& vdc->GR2_FLM5, 11, 16, HEIGHT - 1);	// GR2_FLM_LNUM Sets the number of lines in a frame
	SETREG32_CK(& vdc->GR2_FLM5, 11, 0, HEIGHT - 1);	// GR2_FLM_LOOP Sets the number of lines in a frame
	SETREG32_CK(& vdc->GR2_FLM6, 11, 16, WIDTH - 1);	// GR2_HW Sets the width of the horizontal valid period.
	SETREG32_CK(& vdc->GR2_FLM6, 4, 28, grx_format_MAIN);	// GR2_FORMAT 0: RGB565
	SETREG32_CK(& vdc->GR2_FLM6, 3, 10, grx_rdswa_MAIN);	// GR2_RDSWA 110: (7) (8) (5) (6) (3) (4) (1) (2) [32-bit swap + 16-bit swap]
	SETREG32_CK(& vdc->GR2_AB1, 2, 0,	0x02);			// GR2_DISP_SEL 2: Current graphics display
	SETREG32_CK(& vdc->GR2_BASE, 24, 0, 0x0000FF00);	// BLUE GR2_BASE GBR Background Color B,Gb & R Signal
	SETREG32_CK(& vdc->GR2_AB2, 11, 16, TOPMARGIN);	// GR2_GRC_VS
	SETREG32_CK(& vdc->GR2_AB2, 11, 0, HEIGHT);		// GR2_GRC_VW
	SETREG32_CK(& vdc->GR2_AB3, 11, 16, LEFTMARGIN);	// GR2_GRC_HS
	SETREG32_CK(& vdc->GR2_AB3, 11, 0, WIDTH);			// GR2_GRC_HW

#if LCDMODE_LTDC_L8
	#define     VDC5_CH0_GR0_CLUT_TBL           (*(volatile uint32_t*)0xFCFF6000)
	//#define     VDC5_CH0_GR1_CLUT_TBL           (*(volatile uint32_t*)0xFCFF6400)
	#define     VDC5_CH0_GR2_CLUT_TBL           (*(volatile uint32_t*)0xFCFF6800)
	#define     VDC5_CH0_GR3_CLUT_TBL           (*(volatile uint32_t*)0xFCFF6C00)

	SETREG32_CK(& vdc->GR2_CLUT, 1, 16, 0x00);			// GR2_CLT_SEL
	VDC5_fillLUT_L8(& VDC5_CH0_GR2_CLUT_TBL);
	SETREG32_CK(& vdc->GR2_CLUT, 1, 16, 0x01);			// GR2_CLT_SEL
	//vdc->GR2_CLUT ^= (1uL << 16);	// GR2_CLT_SEL Switch to filled table
#endif /* LCDMODE_LTDC_L8 */

	////////////////////////////////////////////////////////////////
	// GR3 - PIP screen

	SETREG32_CK(& vdc->GR3_FLM_RD, 1, 0, 0);			// GR3_R_ENB Frame Buffer Read Enable
	SETREG32_CK(& vdc->GR3_FLM1, 2, 8, 0x01);			// GR3_FLM_SEL 1: Selects GR3_FLM_NUM.
	SETREG32_CK(& vdc->GR3_FLM2, 32, 0, (uintptr_t) & framebuff);	// GR3_BASE
	SETREG32_CK(& vdc->GR3_FLM3, 15, 16, ROWSIZE);		// GR3_LN_OFF
	SETREG32_CK(& vdc->GR3_FLM3, 10, 0, 0x00);			// GR3_FLM_NUM
	SETREG32_CK(& vdc->GR3_FLM4, 23, 0, ROWSIZE * HEIGHT);	// GR0_FLM_OFF
	SETREG32_CK(& vdc->GR3_FLM5, 11, 16, HEIGHT - 1);	// GR3_FLM_LNUM Sets the number of lines in a frame
	SETREG32_CK(& vdc->GR3_FLM5, 11, 0, HEIGHT - 1);	// GR3_FLM_LOOP Sets the number of lines in a frame
	SETREG32_CK(& vdc->GR3_FLM6, 11, 16, WIDTH - 1);	// GR3_HW Sets the width of the horizontal valid period.
	SETREG32_CK(& vdc->GR3_FLM6, 4, 28, grx_format_PIP);	// GR3_FORMAT 0: RGB565
	SETREG32_CK(& vdc->GR3_FLM6, 3, 10, grx_rdswa_PIP);	// GR3_RDSWA 110: (7) (8) (5) (6) (3) (4) (1) (2) [32-bit swap + 16-bit swap]
	SETREG32_CK(& vdc->GR3_AB1, 2, 0,	0x01);			// GR3_DISP_SEL 1: Lower-layer graphics display
	SETREG32_CK(& vdc->GR3_BASE, 24, 0, 0x000000FF);	// RED GR3_BASE GBR Background Color B,Gb & R Signal
	SETREG32_CK(& vdc->GR3_AB2, 11, 16, TOPMARGIN);	// GR3_GRC_VS
	SETREG32_CK(& vdc->GR3_AB2, 11, 0, HEIGHT);		// GR3_GRC_VW
	SETREG32_CK(& vdc->GR3_AB3, 11, 16, LEFTMARGIN);	// GR3_GRC_HS
	SETREG32_CK(& vdc->GR3_AB3, 11, 0, WIDTH);			// GR3_GRC_HW

#if LCDMODE_LTDC_PIP16

	/* Adjust GR3 parameters for PIP mode (GR2 - mani window, GR3 - PIP) */

	//pipparams_t mainwnd = { 0, 0, DIM_SECOND, DIM_FIRST, (uintptr_t) & framebuff };
	pipparams_t pipwnd;
	display2_getpipparams(& pipwnd);

	SETREG32_CK(& vdc->GR3_FLM_RD, 1, 0, 0);			// GR3_R_ENB Frame Buffer Read Enable
	SETREG32_CK(& vdc->GR3_AB1, 2, 0,	0x02);			// GR2_DISP_SEL 2: Current graphics display
	SETREG32_CK(& vdc->GR3_FLM3, 15, 16, pipwnd.w * sizeof (PACKEDCOLOR565_T));		// GR3_LN_OFF
	SETREG32_CK(& vdc->GR3_FLM3, 10, 0, 0x00);			// GR3_FLM_NUM
	SETREG32_CK(& vdc->GR3_FLM4, 23, 0, pipwnd.w * pipwnd.h * sizeof (PACKEDCOLOR565_T));	// GR3_FLM_OFF
	SETREG32_CK(& vdc->GR3_FLM5, 11, 16, pipwnd.h - 1);	// GR3_FLM_LNUM Sets the number of lines in a frame
	SETREG32_CK(& vdc->GR3_FLM5, 11, 0, pipwnd.h - 1);	// GR3_FLM_LOOP Sets the number of lines in a frame
	SETREG32_CK(& vdc->GR3_FLM6, 11, 16, pipwnd.w - 1);	// GR3_HW Sets the width of the horizontal valid period.
	SETREG32_CK(& vdc->GR3_AB2, 11, 16, TOPMARGIN + pipwnd.y);	// GR3_GRC_VS
	SETREG32_CK(& vdc->GR3_AB2, 11, 0, pipwnd.h);		// GR3_GRC_VW
	SETREG32_CK(& vdc->GR3_AB3, 11, 16, LEFTMARGIN + pipwnd.x);	// GR3_GRC_HS
	SETREG32_CK(& vdc->GR3_AB3, 11, 0, pipwnd.w);			// GR3_GRC_HW

#endif /* LCDMODE_LTDC_PIP16 */

#if 0
	struct fb_videomode *mode = priv->videomode;
	uint32_t tmp;
	struct vdc5fb_layer *layer;
	uint32_t update_addr[4];
	int i;

	/* Need at least 1 graphic layer for /dev/fb0 */
	for (i=0;i<4;i++)
		if( priv->pdata->layers[i].xres )
			break;
	if( i == 4 )
	{
		printk("\n\n\n%s: You need to define at least 1 'layer' to be used as /dev/fb0\n\n\n",__func__);
		return -1;
	}

	update_addr[0] = (uint32_t)priv->base + vdc5fb_offsets[GR0_UPDATE];
	update_addr[1] = (uint32_t)priv->base + vdc5fb_offsets[GR1_UPDATE];
	update_addr[2] = (uint32_t)priv->base + vdc5fb_offsets[GR2_UPDATE];
	update_addr[3] = (uint32_t)priv->base + vdc5fb_offsets[GR3_UPDATE];

	for (i=0;i<4;i++) {
#ifdef DEBUG
		/* Set Background color (really for debugging only) */
		switch (i) {
			case 0:	tmp = 0x00800000;	// GR0 = Green
				break;
			case 1:	tmp = 0x00000080;	// GR1 = red
				break;
			case 2:	tmp = 0x00008000;	// GR2 = Blue
				break;
			case 3:	tmp = 0x00008080;	// GR3 = purple
		}
		vdc5fb_iowrite32(tmp, update_addr[i] + GR_BASE_OFFSET);	/* Background color (0-G-B-R) */
#endif
		layer = &priv->pdata->layers[i];
		if( layer->xres == 0 ) {
			/* not used */
			vdc5fb_iowrite32(0, update_addr[i] + GR_FLM_RD_OFFSET);
			if( i == 0 )
				vdc5fb_iowrite32(0, update_addr[i] + GR_AB1_OFFSET);	/* background graphics display */
			else
				vdc5fb_iowrite32(1, update_addr[i] + GR_AB1_OFFSET);	/* Lower-layer graphics display */
			continue;
		}

		vdc5fb_iowrite32(GR_R_ENB, update_addr[i] + GR_FLM_RD_OFFSET);
		vdc5fb_iowrite32(GR_FLM_SEL(1), update_addr[i] + GR_FLM1_OFFSET); /* scalers MUST use FLM_SEL */
		if( layer->base == 0 )
			layer->base = priv->dma_handle;	/* Allocated during probe */
		if( layer->base >= 0xC0000000 )
			layer->base = virt_to_phys((void *)layer->base);	/* Convert to physical address */

		printk("vdc5fb: Layer %u Enabled (%ux%u @ 0x%08x)\n",i,layer->xres,layer->yres, layer->base);

		vdc5fb_iowrite32(layer->base, update_addr[i] + GR_FLM2_OFFSET);	/* frame buffer address*/
		tmp = GR_LN_OFF(layer->xres * (layer->bpp / 8));	/* length of each line (and Frame Number=0)*/
		vdc5fb_iowrite32(tmp, update_addr[i] + GR_FLM3_OFFSET);
		tmp = GR_FLM_LOOP(layer->yres - 1);
		tmp |= GR_FLM_LNUM(layer->yres - 1);
		vdc5fb_iowrite32(tmp, update_addr[i] + GR_FLM5_OFFSET);		/* lines per frame */
		tmp = layer->format;
		tmp |= GR_HW(layer->xres - 1);
		vdc5fb_iowrite32(tmp, update_addr[i] + GR_FLM6_OFFSET);	/* frame format */

		tmp = 0;
		if( layer->blend )
			tmp |= GR_DISP_SEL(3);		/* Blended display of lower-layer graphics and current graphics */
		else
			tmp |= GR_DISP_SEL(2);		/* Current graphics display */
		vdc5fb_iowrite32(tmp, update_addr[i] + GR_AB1_OFFSET);

		tmp = GR_GRC_VW(layer->yres);
		tmp |= GR_GRC_VS(mode->vsync_len + mode->upper_margin + layer->y_offset);
		vdc5fb_iowrite32(tmp, update_addr[i] + GR_AB2_OFFSET);

		tmp = GR_GRC_HW(layer->xres);
		tmp |= GR_GRC_HS(mode->hsync_len + mode->left_margin + layer->x_offset);
		vdc5fb_iowrite32(tmp, update_addr[i] + GR_AB3_OFFSET);
	}

	/* Graphics VIN (Image Synthsizer) */
	/* Scaler 0 and Scaler 1 are blended together using this */
	/* GR0 = lower */
	/* GR1 = current */
	tmp = vdc5fb_read(priv, GR_VIN_AB1);
	tmp &= GR_AB1_MASK;
	if ( priv->pdata->layers[0].xres != 0 ) {
		if ( priv->pdata->layers[1].xres == 0 )
			// GR0 used, GR1 not used
			tmp |= GR_DISP_SEL(1);		/* lower only*/
		else
			// GR0 used, GR1 used
			tmp |= GR_DISP_SEL(3);		/* blend */
	}
	else if ( priv->pdata->layers[1].xres != 0 )
	{
			// GR0 not used, GR1 used
			tmp |= GR_DISP_SEL(2);		/* current only */
	}
	vdc5fb_write(priv, GR_VIN_AB1, tmp);
	vdc5fb_write(priv, GR_VIN_BASE, 0x00FF00);	/* Background color (0-G-B-R) */

	/* Set the LCD margins, other wise the pixels will be cliped
	  (and background color will show through instead */
	tmp = GR_GRC_VW(priv->panel_pixel_yres);
	tmp |= GR_GRC_VS(mode->vsync_len + mode->upper_margin);
	vdc5fb_write(priv, GR_VIN_AB2, tmp);
	tmp = GR_GRC_HW(priv->panel_pixel_xres);
	tmp |= GR_GRC_HS(mode->hsync_len + mode->left_margin);
	vdc5fb_write(priv, GR_VIN_AB3, tmp);

	/* Graphics OIR */
#ifdef OUTPUT_IMAGE_GENERATOR
	vdc5fb_write(priv, GR_OIR_FLM_RD, GR_R_ENB);
	vdc5fb_write(priv, GR_OIR_FLM1, GR_FLM_SEL(1));
	vdc5fb_write(priv, GR_OIR_FLM2, priv->dma_handle);
	tmp = GR_FLM_NUM(priv->flm_num);
	tmp |= GR_LN_OFF(mode->xres * (priv->info->var.bits_per_pixel / 8));
	vdc5fb_write(priv, GR_OIR_FLM3, tmp);
	tmp = GR_FLM_OFF(priv->flm_off);
	vdc5fb_write(priv, GR_OIR_FLM4, tmp);
	tmp = GR_FLM_LOOP(mode->yres - 1);
	tmp |= GR_FLM_LNUM(mode->yres - 1);
	vdc5fb_write(priv, GR_OIR_FLM5, tmp);
	if (priv->info->var.bits_per_pixel == 16)
		tmp = D_GR_FLM6_RGB565;		/* RGB565 LE, 78563412 */
	else
		tmp = D_GR_FLM6_ARGB8888;	/* ARGB8888 LE, 56781234 */
	tmp |= GR_HW(mode->xres - 1);
	vdc5fb_write(priv, GR_OIR_FLM6, tmp);

	tmp = vdc5fb_read(priv, GR_OIR_AB1);
	tmp &= GR_AB1_MASK;
	tmp |= GR_DISP_SEL(2);		/* current graphics */
	vdc5fb_write(priv, GR_OIR_AB1, tmp);

	tmp = GR_GRC_VW(mode->yres);
	tmp |= GR_GRC_VS(mode->vsync_len + mode->upper_margin);
	vdc5fb_write(priv, GR_OIR_AB2, tmp);

	tmp = GR_GRC_HW(mode->xres);
	tmp |= GR_GRC_HS(mode->hsync_len + mode->left_margin);
	vdc5fb_write(priv, GR_OIR_AB3, tmp);

	vdc5fb_write(priv, GR_OIR_AB7, 0);
	vdc5fb_write(priv, GR_OIR_AB8, D_GR_AB8);
	vdc5fb_write(priv, GR_OIR_AB9, D_GR_AB9);
	vdc5fb_write(priv, GR_OIR_AB10, D_GR_AB10);
	vdc5fb_write(priv, GR_OIR_AB11, D_GR_AB11);

	vdc5fb_write(priv, GR_OIR_BASE, D_GR_BASE);
#endif

#endif
}

static void vdc5fb_init_outcnt(struct st_vdc5 * const vdc)
{
	////////////////////////////////////////////////////////////////
	// OUT
	SETREG32_CK(& vdc->OUT_CLK_PHASE, 1, 8, 0x00);	// OUTCNT_LCD_EDGE 0: Output changed at the rising edge of LCD_CLK pin, data latched at falling edge
	SETREG32_CK(& vdc->OUT_SET, 2, 8, 0x00);	// OUT_FRQ_SEL Clock Frequency Control 0: 100% speed — (parallel RGB)
	SETREG32_CK(& vdc->OUT_SET, 2, 12, 0x02);	// OUT_FORMAT Output Format Select 2: RGB565

#if 0
	struct vdc5fb_pdata *pdata = priv_to_pdata(priv);
	uint32_t tmp;

	vdc5fb_write(priv, OUT_CLK_PHASE, D_OUT_CLK_PHASE);
	vdc5fb_write(priv, OUT_BRIGHT1, PBRT_G(512));
	vdc5fb_write(priv, OUT_BRIGHT2, (PBRT_B(512) | PBRT_R(512)));
	tmp = (CONT_G(128) | CONT_B(128) | CONT_R(128));
	vdc5fb_write(priv, OUT_CONTRAST, tmp);

	vdc5fb_write(priv, GAM_SW, 0);

	tmp = D_OUT_PDTHA;
	tmp |= PDTHA_FORMAT(0);
	vdc5fb_write(priv, OUT_PDTHA, tmp);

	tmp = D_OUT_SET;
	tmp |= OUT_FORMAT(pdata->out_format);
	vdc5fb_write(priv, OUT_SET, tmp);

#endif
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
	SETREG32_CK(& vdc->TCON_TIM, 11, 0, 0);			// TCON_OFFSET

	//SETREG32_CK(& vdc->TCON_TIM_POLA2, 2, 12, 0x00);	// TCON_POLA_MD
	//SETREG32_CK(& vdc->TCON_TIM_POLB2, 2, 12, 0x00);	// TCON_POLB_MD

	// HSYNC signal
	SETREG32_CK(& vdc->TCON_TIM_STH1, 11, 16,	0);		// TCON_STH_HS
	SETREG32_CK(& vdc->TCON_TIM_STH1, 11, 0, HSYNC);	// TCON_STH_HW
	// Source strobe signal
	SETREG32_CK(& vdc->TCON_TIM_STB1, 11, 16, 0);		// TCON_STB_HS
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

	debug_printf_P(PSTR("arm_hardware_ltdc_initialize start, WIDTH=%d, HEIGHT=%d\n"), WIDTH, HEIGHT);
	const unsigned ROWSIZE = sizeof framebuff [0];	// размер одной строки в байтах


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

#if LCDMODE_LTDC_PIP16
	arm_hardware_ltdc_pip_off();
#endif /* LCDMODE_LTDC_PIP16 */

	debug_printf_P(PSTR("arm_hardware_ltdc_initialize done\n"));
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
            

  PACKEDCOLOR_T LTDC_BackgroundColor;         /*!< configures the background  */

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

  PACKEDCOLOR_T LTDC_DefaultColor;           /*!< Configures the default color value.. */


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

#if LCDMODE_LTDC_L24


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

#elif LCDMODE_LTDC_L8

static void
fillLUT_L8(
	LTDC_Layer_TypeDef* LTDC_Layerx
	)
{
	unsigned color;

	for (color = 0; color < 256; ++ color)
	{
#define XRGB(zr,zg,zb) do { r = (zr), g = (zg), b = (zb); } while (0)
		uint_fast8_t r, g, b;

		switch (color)
		{
		case TFTRGB(0, 0, 0)			/*COLOR_BLACK*/:		XRGB(0, 0, 0);			break;	// 0x00 черный
		case TFTRGB(255, 0, 0)			/*COLOR_RED*/:			XRGB(255, 0, 0);		break; 	// 0xE0 красный
		case TFTRGB(0, 255, 0)			/*COLOR_GREEN*/:		XRGB(0, 255, 0);		break; 	// 0x1C зеленый
		case TFTRGB(0, 0, 255)			/*COLOR_BLUE*/:			XRGB(0, 0, 255);		break; 	// 0x03 синий
		case TFTRGB(128, 0, 0)			/*COLOR_DARKRED*/:		XRGB(128, 0, 0);		break; 	// 
		case TFTRGB(0, 128, 0)			/*COLOR_DARKGREEN*/:	XRGB(0, 128, 0);		break; 	// 
		case TFTRGB(0, 0, 128)			/*COLOR_DARKBLUE*/:		XRGB(0, 0, 128);		break; 	// 
		case TFTRGB(255, 255, 0)		/*COLOR_YELLOW*/:		XRGB(255, 255, 0);		break; 	// 0xFC желтый
		case TFTRGB(255, 0, 255)		/*COLOR_MAGENTA*/:		XRGB(255, 0, 255);		break; 	// 0x83 пурпурный
		case TFTRGB(0, 255, 255)		/*COLOR_CYAN*/:			XRGB(0, 255, 255);		break; 	// 0x1F голубой
		case TFTRGB(255, 255, 255)		/*COLOR_WHITE*/:		XRGB(255, 255, 255);	break;  // 0xff	белый
		case TFTRGB(128, 128, 128)		/*COLOR_GRAY*/:			XRGB(128, 128, 128);	break; 	// серый
		case TFTRGB(0xa5, 0x2a, 0x2a)	/*COLOR_BROWN*/:		XRGB(0xa5, 0x2a, 0x2a);	break; 	// 0x64 коричневый
		case TFTRGB(0xff, 0xd7, 0x00)	/*COLOR_GOLD*/:			XRGB(0xff, 0xd7, 0x00);	break; 	// 0xF4 золото
		case TFTRGB(0xd1, 0xe2, 0x31)	/*COLOR_PEAR*/:			XRGB(0xd1, 0xe2, 0x31);	break; 	// 0xDC грушевый
#undef XRGB
		default:
			r = ((color & 0xe0) << 0) | ((color & 0x80) ? 0x1f : 0);	// red
			g = ((color & 0x1c) << 3) | ((color & 0x10) ? 0x1f : 0);	// green
			b = ((color & 0x03) << 6) | ((color & 0x02) ? 0x3f : 0);	// blue
			break;
		}
		/* запись значений в регистры палитры */
		LTDC_Layerx->CLUTWR = 
			((color << LTDC_LxCLUTWR_CLUTADD_Pos) & LTDC_LxCLUTWR_CLUTADD) |
			((r << LTDC_LxCLUTWR_RED_Pos) & LTDC_LxCLUTWR_RED) |
			((g << LTDC_LxCLUTWR_GREEN_Pos) & LTDC_LxCLUTWR_GREEN) |
			((b << LTDC_LxCLUTWR_BLUE_Pos) & LTDC_LxCLUTWR_BLUE) |
			0;
	}

	LTDC_Layerx->CR |= LTDC_LxCR_CLUTEN;
}
#else

#endif /* LCDMODE_LTDC_L8 */


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
	LTDC_Layerx->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
	LTDC_Layerx->CFBAR |= (LTDC_Layer_InitStruct->LTDC_CFBStartAdress);

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
	const unsigned ROWSIZE = (pixelsize * wnd->w);	// размер одной строки в байтах

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
	LTDC_Layer_InitStruct.LTDC_CFBPitch = ROWSIZE; // (DIM_SECOND * 2);

	/* Configure the number of lines */  
	LTDC_Layer_InitStruct.LTDC_CFBLineNumber = wnd->h;

	/* Start Address configuration : the LCD Frame buffer is defined on SDRAM */    
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = wnd->frame;
	//LTDC_Layer1->CFBAR = (uint32_t) & framebuff;

	/* Initialize LTDC layer 1 */
	LTDC_LayerInit(LTDC_Layerx, & LTDC_Layer_InitStruct);

	/* Enable foreground & background Layers */
	//LTDC_LayerCmd(LTDC_Layer1, ENABLE); 
	//LTDC_LayerCmd(LTDC_Layer2, ENABLE);
	/* Enable LTDC_Layer by setting LEN bit */
	LTDC_Layerx->CR |= LTDC_LxCR_LEN;

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
	const unsigned long key = COLOR_KEY;
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
	debug_printf_P(PSTR("arm_hardware_ltdc_initialize start, WIDTH=%d, HEIGHT=%d\n"), WIDTH, HEIGHT);

	//const unsigned ROWSIZE = sizeof framebuff [0];	// размер одной строки в байтах
	//const unsigned rowsize2 = (sizeof (PACKEDCOLOR_T) * DIM_SECOND);
	//ASSERT(ROWSIZE == rowsize2);
	debug_printf_P(PSTR("arm_hardware_ltdc_initialize: framebuff=%p\n"), framebuff);

	/* Initialize the LCD */

	hardware_set_dotclock(LTDC_DOTCLK);

#if CPUSTYLE_STM32H7XX
	/* Enable the LTDC Clock */
	RCC->APB3ENR |= RCC_APB3ENR_LTDCEN;	/* LTDC clock enable */
	(void) RCC->APB3ENR;

#elif CPUSTYLE_STM32MP1

	/* SYSCFG clock enable */
	RCC->MP_APB3ENSETR = RCC_MC_APB3ENSETR_SYSCFGEN;
	(void) RCC->MP_APB3ENSETR;
	/*
	 * Interconnect update : select master using the port 1.
	 * LTDC = AXI_M9.
	 */
	SYSCFG->ICNR |= SYSCFG_ICNR_AXI_M9;
	(void) SYSCFG->ICNR;

	/* Enable the LTDC Clock */
	RCC->MP_APB4ENSETR = RCC_MP_APB4ENSETR_LTDCEN;	/* LTDC clock enable */
	/* Enable the LTDC Clock in low-power mode */
	(void) RCC->MP_APB4ENSETR;
	RCC->MP_APB4LPENSETR = RCC_MP_APB4LPENSETR_LTDCLPEN;	/* LTDC clock enable */
	(void) RCC->MP_APB4LPENSETR;

#else /* CPUSTYLE_STM32H7XX */
	/* Enable the LTDC Clock */
	RCC->APB2ENR |= RCC_APB2ENR_LTDCEN;	/* LTDC clock enable */
	(void) RCC->APB2ENR;
#endif /* CPUSTYLE_STM32H7XX */

	/* Configure the LCD Control pins */
	HARDWARE_LTDC_INITIALIZE(BOARD_DEMODE);	// подключение к выводам процессора сигналов периферийного контроллера

	/* LTDC Initialization -------------------------------------------------------*/
	LTDC_InitTypeDef LTDC_InitStruct;

	pipparams_t mainwnd = { 0, 0, DIM_SECOND, DIM_FIRST, (uintptr_t) & framebuff };
#if LCDMODE_LTDC_PIP16
	pipparams_t pipwnd;
	display2_getpipparams(& pipwnd);

	debug_printf_P(PSTR("arm_hardware_ltdc_initialize: pip: x/y=%u/%u, w/h=%u/%u\n"), pipwnd.x, pipwnd.y, pipwnd.w, pipwnd.h);
#endif /* LCDMODE_LTDC_PIP16 */

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
#if LCDMODE_LTDC_L24

	LCD_LayerInit(LAYER_MAIN, LEFTMARGIN, TOPMARGIN, & mainwnd, LTDC_Pixelformat_L8, 3, sizeof (PACKEDCOLOR_T));

#elif LCDMODE_LTDC_L8

	LCD_LayerInit(LAYER_MAIN, LEFTMARGIN, TOPMARGIN, & mainwnd, LTDC_Pixelformat_L8, 1, sizeof (PACKEDCOLOR_T));

#else
	/* Без палитры */
	LCD_LayerInit(LAYER_MAIN, LEFTMARGIN, TOPMARGIN, & mainwnd, LTDC_Pixelformat_RGB565, 1, sizeof (PACKEDCOLOR_T));

#endif /* LCDMODE_LTDC_L8 */

#if LCDMODE_LTDC_PIP16

	LCD_LayerInitMain(LAYER_MAIN);	// довести инициализацию

	// Bottom layer
	LCD_LayerInit(LAYER_PIP, LEFTMARGIN, TOPMARGIN, & pipwnd, LTDC_Pixelformat_RGB565, 1, sizeof (uint16_t));
	LCD_LayerInitPIP(LAYER_PIP);	// довести инициализацию

#endif /* LCDMODE_LTDC_PIP16 */

	LTDC->SRCR = LTDC_SRCR_IMR;	/*!< Immediately Reload. */

	/* Enable the LTDC */
	LTDC->GCR |= LTDC_GCR_LTDCEN;

#if LCDMODE_LTDC_L24
	fillLUT_L24(LAYER_MAIN);	// прямая трансляция всех ьайтов из памяти на выход. загрузка палитры - имеет смысл до Reload
#elif LCDMODE_LTDC_L8
	fillLUT_L8(LAYER_MAIN);	// загрузка палитры - имеет смысл до Reload
#endif /* LCDMODE_LTDC_L8 */

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

#if LCDMODE_LTDC_PIP16
	arm_hardware_ltdc_pip_off();
#endif /* LCDMODE_LTDC_PIP16 */
	debug_printf_P(PSTR("arm_hardware_ltdc_initialize done\n"));
}

#if LCDMODE_LTDC_PIP16

/* set bottom buffer start */
void arm_hardware_ltdc_pip_set(uintptr_t p)
{
	LAYER_PIP->CFBAR = p;
	LAYER_PIP->CR |= LTDC_LxCR_LEN;
	LTDC->SRCR = LTDC_SRCR_VBR;	/* Vertical Blanking Reload. */
}

void arm_hardware_ltdc_pip_off(void)	// set PIP framebuffer address
{
	LAYER_PIP->CR &= ~ LTDC_LxCR_LEN;
	LTDC->SRCR = LTDC_SRCR_VBR;	/* Vertical Blanking Reload. */
}

#endif /* LCDMODE_LTDC_PIP16 */

#endif /* CPUSTYLE_STM32F */

#endif /* WITHLTDCHW */
