#include "hardware.h"

#if CPUSTYLE_H3

#include <string.h>
#include "formats.h"
#include "display.h"

#define APPDPIXELWIDTH 4

static void h3_hdmi_dump(void);

static void h3_display_clocks_init(const videomode_t * vdmode)
{
	PRINTF("dot clock = %u\n", display_getdotclock(vdmode));
	// Set up shared and dedicated clocks for HDMI, LCD/TCON and DE2
	CCU->PLL_DE_CTRL_REG = (UINT32_C(1) << 31) | (UINT32_C(1) << 24) | ((18 - 1) * (UINT32_C(1) << 8)) | ((1 - 1) * (UINT32_C(1) << 0)); // 432MHz
	CCU->PLL_VIDEO_CTRL_REG = (UINT32_C(1) << 31) | (UINT32_C(1) << 25) | (UINT32_C(1) << 24) | ((99 - 1) * (UINT32_C(1) << 8)) | ((8 - 1) * (UINT32_C(1) << 0)); // 297MHz
	local_delay_ms(50);

	CCU->BUS_CLK_GATING_REG1 |= (UINT32_C(1) << 12) | (UINT32_C(1) << 11) | (UINT32_C(1) << 3); // Enable DE, HDMI, TCON0
	CCU->BUS_SOFT_RST_REG1 |= (UINT32_C(1) << 12) | ( UINT32_C(1) << 11) | ( UINT32_C(1) << 10) | (UINT32_C(1) << 3); // De-assert reset of DE, HDMI0/1, TCON0
	CCU->DE_CLK_REG = (UINT32_C(1) << 31) | (UINT32_C(1) << 24); // Enable DE clock, set source to PLL_DE
	CCU->HDMI_CLK_REG = (UINT32_C(1) << 31); // Enable HDMI clk (use PLL3)
	CCU->HDMI_SLOW_CLK_REG = (UINT32_C(1) << 31); // Enable HDMI slow clk
	CCU->TCON0_CLK_REG = (UINT32_C(1) << 31) | 1; // 1-1980,2-2080 3-3080,3 Enable TCON0 clk, divide by 4
}

static void h3_hdmi_init(const videomode_t * vdmode)
{
	/* Accumulated parameters for this display */
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
	PRINTF("HTOTAL=%u\n", HTOTAL);
	PRINTF("VTOTAL=%u\n", VTOTAL);
	PRINTF("HBLANKING=%u\n", HBLANKING);
	PRINTF("VBLANKING=%u\n", VBLANKING);
	PRINTF("HSYNC=%u\n", HSYNC);
	PRINTF("VSYNC=%u\n", VSYNC);
	PRINTF("HFP=%u\n", HFP);
	PRINTF("VFP=%u\n", VFP);

	PRINTF("dot clock = %u\n", display_getdotclock(vdmode));
	// HDMI PHY init, the following black magic is based on the procedure documented at:
	// http://linux-sunxi.org/images/3/38/AW_HDMI_TX_PHY_S40_Spec_V0.1.pdf
	HDMI_PHY->HDMI_PHY_CFG1 = 0;
	HDMI_PHY->HDMI_PHY_CFG1 = 1;
	local_delay_us(5);
	HDMI_PHY->HDMI_PHY_CFG1 |= (UINT32_C(1) << 16);
	HDMI_PHY->HDMI_PHY_CFG1 |= (UINT32_C(1) << 1);
	local_delay_us(10);
	HDMI_PHY->HDMI_PHY_CFG1 |= (UINT32_C(1) << 2);
	local_delay_us(5);
	HDMI_PHY->HDMI_PHY_CFG1 |= (UINT32_C(1) << 3);
	local_delay_us(40);
	HDMI_PHY->HDMI_PHY_CFG1 |= (UINT32_C(1) << 19);
	local_delay_us(100);
	HDMI_PHY->HDMI_PHY_CFG1 |= (UINT32_C(1) << 18);
	HDMI_PHY->HDMI_PHY_CFG1 |= (7 << 4);

	while ((HDMI_PHY->HDMI_PHY_STS & 0x80) == 0)
		;

	HDMI_PHY->HDMI_PHY_CFG1 |= (0xf << 4);
	HDMI_PHY->HDMI_PHY_CFG1 |= (0xf << 8);
	HDMI_PHY->HDMI_PHY_CFG3 |= (UINT32_C(1) << 0) | (UINT32_C(1) << 2);

	HDMI_PHY->HDMI_PHY_PLL1 &= ~ (UINT32_C(1) << 26);
	HDMI_PHY->HDMI_PHY_CEC = 0;

	HDMI_PHY->HDMI_PHY_PLL1 = 0x39dc5040;
	HDMI_PHY->HDMI_PHY_PLL2 = 0x80084381;
	local_delay_us(10000);
	HDMI_PHY->HDMI_PHY_PLL3 = 1;
	HDMI_PHY->HDMI_PHY_PLL1 |= (UINT32_C(1) << 25);
	local_delay_us(10000);
	uint32_t tmp = (HDMI_PHY->HDMI_PHY_STS & 0x1f800) >> 11;
	HDMI_PHY->HDMI_PHY_PLL1 |= (UINT32_C(1) << 31) | (UINT32_C(1) << 30) | tmp;

	HDMI_PHY->HDMI_PHY_CFG1 = 0x01FFFF7F;
	HDMI_PHY->HDMI_PHY_CFG2 = 0x8063A800;
	HDMI_PHY->HDMI_PHY_CFG3 = 0x0F81C485;

	/* enable read access to HDMI controller */
	HDMI_PHY->HDMI_PHY_READ_EN = 0x54524545;
	/* descramble register offsets */
	HDMI_PHY->HDMI_PHY_UNSCRAMBLE = 0x42494E47;
	// HDMI Config, based on the documentation at:
	// https://people.freebsd.org/~gonzo/arm/iMX6-HDMI.pdf
	HDMI_TX0->HDMI_FC_INVIDCONF = (UINT32_C(1) << 6) | (UINT32_C(1) << 5) | (UINT32_C(1) << 4) | (UINT32_C(1) << 3); // Polarity etc
	HDMI_TX0->HDMI_FC_INHACTV0 = (WIDTH & 0xff);    // Horizontal pixels
	HDMI_TX0->HDMI_FC_INHACTV1 = (WIDTH >> 8);      // Horizontal pixels
	HDMI_TX0->HDMI_FC_INHBLANK0 = (HBLANKING & 0xff);     // Horizontal blanking
	HDMI_TX0->HDMI_FC_INHBLANK1 = (HBLANKING >> 8);       // Horizontal blanking

	HDMI_TX0->HDMI_FC_INVACTV0 = (HEIGHT & 0xff);    // Vertical pixels
	HDMI_TX0->HDMI_FC_INVACTV1 = (HEIGHT >> 8);      // Vertical pixels
	HDMI_TX0->HDMI_FC_INVBLANK = VBLANKING;               // Vertical blanking

	HDMI_TX0->HDMI_FC_HSYNCINDELAY0 = (HFP & 0xff);
	HDMI_TX0->HDMI_FC_HSYNCINDELAY1 = (HFP >> 8);    // Horizontal Front porch
	HDMI_TX0->HDMI_FC_HSYNCINWIDTH0 = (HSYNC & 0xff);  // Horizontal sync pulse
	HDMI_TX0->HDMI_FC_HSYNCINWIDTH1 = (HSYNC >> 8);    // Horizontal sync pulse

	HDMI_TX0->HDMI_FC_VSYNCINDELAY = VFP;
	HDMI_TX0->HDMI_FC_VSYNCINWIDTH = VSYNC;            // Vertical sync pulse

	HDMI_TX0->HDMI_FC_CTRLDUR = 12;   // Frame Composer Control Period Duration
	HDMI_TX0->HDMI_FC_EXCTRLDUR = 32; // Frame Composer Extended Control Period Duration
	HDMI_TX0->HDMI_FC_EXCTRLSPAC = 1; // Frame Composer Extended Control Period Maximum Spacing
	HDMI_TX0->HDMI_FC_CH0PREAM = 0x0b; // Frame Composer Channel 0 Non-Preamble Data
	HDMI_TX0->HDMI_FC_CH1PREAM = 0x16; // Frame Composer Channel 1 Non-Preamble Data
	HDMI_TX0->HDMI_FC_CH2PREAM = 0x21; // Frame Composer Channel 2 Non-Preamble Data
	HDMI_TX0->HDMI_MC_FLOWCTRL = 0;    // Main Controller Feed Through Control
	HDMI_TX0->HDMI_MC_CLKDIS = 0x74; // Main Controller Synchronous Clock Domain Disable

#if 0
	HDMI_TX0->HDMI_FC_INVIDCONF = (UINT32_C(1) << 6) | (UINT32_C(1) << 5) | (UINT32_C(1) << 4) | (UINT32_C(1) << 3); // Polarity etc

	HDMI_TX0->HDMI_FC_INHACTIV0 = (800 & 0xff);    // Horizontal pixels
	HDMI_TX0->HDMI_FC_INHACTIV1 = (800 >> 8);      // Horizontal pixels
	HDMI_TX0->HDMI_FC_INHBLANK0 = (256 & 0xff);     // Horizontal blanking
	HDMI_TX0->HDMI_FC_INHBLANK1 = (256 >> 8);       // Horizontal blanking

	HDMI_TX0->HDMI_FC_INVACTIV0 = (480 & 0xff);    // Vertical pixels
	HDMI_TX0->HDMI_FC_INVACTIV1 = (480 >> 8);      // Vertical pixels
	HDMI_TX0->HDMI_FC_INVBLANK  = 28;               // Vertical blanking

	HDMI_TX0->HDMI_FC_HSYNCINDELAY0 = (40 & 0xff);  // Horizontal Front porch
	HDMI_TX0->HDMI_FC_HSYNCINDELAY1 = (40 >> 8);    // Horizontal Front porch
	HDMI_TX0->HDMI_FC_VSYNCINDELAY  = 1;            // Vertical front porch
	HDMI_TX0->HDMI_FC_HSYNCINWIDTH0 = (128 & 0xff);  // Horizontal sync pulse
	HDMI_TX0->HDMI_FC_HSYNCINWIDTH1 = (128 >> 8);    // Horizontal sync pulse
	HDMI_TX0->HDMI_FC_VSYNCINWIDTH  = 4;            // Vertical sync pulse

	HDMI_TX0->HDMI_FC_CTRLDUR    = 12;   // Frame Composer Control Period Duration
	HDMI_TX0->HDMI_FC_EXCTRLDUR  = 32;   // Frame Composer Extended Control Period Duration
	HDMI_TX0->HDMI_FC_EXCTRLSPAC = 1;    // Frame Composer Extended Control Period Maximum Spacing
	HDMI_TX0->HDMI_FC_CH0PREAM   = 0x0b; // Frame Composer Channel 0 Non-Preamble Data
	HDMI_TX0->HDMI_FC_CH1PREAM   = 0x16; // Frame Composer Channel 1 Non-Preamble Data
	HDMI_TX0->HDMI_FC_CH2PREAM   = 0x21; // Frame Composer Channel 2 Non-Preamble Data
	HDMI_TX0->HDMI_MC_FLOWCTRL   = 0;    // Main Controller Feed Through Control
	HDMI_TX0->HDMI_MC_CLKDIS     = 0x74; // Main Controller Synchronous Clock Domain Disable
#endif

#if 0
	/*800x480p hfp:40 hs:48 hbp:40 vfp:13 vs:3 vbp:29 pixel clock:32 MHz */

	hltdc.Init.HorizontalSync = 19;
	hltdc.Init.VerticalSync = 2;
	hltdc.Init.AccumulatedHBP = 159;
	hltdc.Init.AccumulatedVBP = 22;
	hltdc.Init.AccumulatedActiveW = 1183;
	hltdc.Init.AccumulatedActiveH = 622;
	hltdc.Init.TotalWidth = 1343;
	hltdc.Init.TotalHeigh = 634;
#endif
}

static void h3_hdmi_tcon_init(const videomode_t * vdmode)
{
	/* Accumulated parameters for this display */
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
//	PRINTF("HBP=%u\n", HBP);
//	PRINTF("VBP=%u\n", VBP);

	// LCD0 feeds mixer0 to HDMI
	TCON0->LCD_GCTL_REG = 0;
	TCON0->LCD_GINT0_REG = 0;
	TCON0->LCD_CTL_REG =
		(UINT32_C(1) << 31) |	// TCON1_En
		30 * (UINT32_C(1) << 5) |	// Start_Delay - AT bit 4, not 5
		0;

	TCON0->TCON1_BASIC0_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);	// TCON1_XI TCON1_YI
	TCON0->TCON1_BASIC1_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);	// LS_XO LS_YO
	TCON0->TCON1_BASIC2_REG = ((WIDTH - 1) << 16) | (HEIGHT - 1);	// TCON1_XO TCON1_YO
	TCON0->TCON1_BASIC3_REG = ((HTOTAL - 1) << 16) | ((HBP - 1) << 0);	// HT HBP
	TCON0->TCON1_BASIC4_REG = ((VTOTAL * 2) << 16) | ((VBP - 1) << 0);			// VT VBP
	TCON0->TCON1_BASIC5_REG = ((HSYNC - 1) << 16) | ((VSYNC - 1) << 0);			// HSPW VSPW

	TCON0->LCD_GCTL_REG = (UINT32_C(1) << 31);
}

// This function configured DE2 as follows:
// MIXER0 -> WB -> MIXER1 -> HDMI

void de2_top_init(void)
{
	DE_TOP->AHB_RESET |= (UINT32_C(1) << 0) | (UINT32_C(1) << 1);  ///core 0,1
	DE_TOP->SCLK_GATE |= (UINT32_C(1) << 0) | (UINT32_C(1) << 1);  ///core 0,1
	DE_TOP->HCLK_GATE |= (UINT32_C(1) << 0) | (UINT32_C(1) << 1);  ///core 0,1

	DE_TOP->DE2TCON_MUX &= ~ (UINT32_C(1) << 0);
}

void de2_bld_init(const videomode_t * vdmode)
{
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */

	// Erase the whole of MIXER0. This contains uninitialized data.
	for (uint32_t addr = DE_MIXER0_GLB_BASE + 0x0000; addr < DE_MIXER0_GLB_BASE + 0xC000; addr += 4)
	{
		* (volatile uint32_t*) (addr) = 0;
	}

	DE_MIXER0_GLB->GLB_CTL = 1;
	DE_MIXER0_GLB->GLB_SIZE = ((HEIGHT - 1) << 16) | (WIDTH - 1);
	//DE_MIXER0_GLB_SIZE = ((480 - 1) << 16) | (800 - 1);

	DE_MIXER0_BLD->BLD_EN_COLOR_CTL = 0x100;
	DE_MIXER0_BLD->ROUTE = 0;
	DE_MIXER0_BLD->OUTPUT_SIZE = ((HEIGHT - 1) << 16) | (WIDTH - 1);
	DE_MIXER0_BLD->CH [0].BLD_CH_ISIZE = ((HEIGHT - 1) << 16) | (WIDTH - 1);
	//DE_MIXER0_BLD->BLD_SIZE = ((480 - 1) << 16) | (800 - 1);
	//DE_MIXER0_BLD->CH[0].BLD_CH_ISIZE = (489 << 16) | (800 - 1);
}

void de2_vsu_init(const videomode_t * vdmode)
{
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const uint32_t APPDIMS_SIZE = ((DIM_Y - 1) << 16) | (DIM_X - 1);

	const uint32_t HSTEP = 0x40000;
	const uint32_t VSTEP = 0x40000;

	DE_MIXER0_VSU1->VSU_CTRL_REG = (UINT32_C(1) << 0);
	DE_MIXER0_VSU1->VSU_OUT_SIZE_REG = ((HEIGHT - 1) << 16) | (WIDTH - 1);
	DE_MIXER0_VSU1->VSU_Y_SIZE_REG = APPDIMS_SIZE;
	DE_MIXER0_VSU1->VSU_Y_HSTEP_REG = HSTEP;
	DE_MIXER0_VSU1->VSU_Y_VSTEP_REG = VSTEP;
	DE_MIXER0_VSU1->VSU_C_SIZE_REG = APPDIMS_SIZE;
	DE_MIXER0_VSU1->VSU_C_HSTEP_REG = HSTEP;
	DE_MIXER0_VSU1->VSU_C_VSTEP_REG = VSTEP;

	for (int n = 0; n < 32; n ++)
	{
		DE_MIXER0_VSU1->VSU_Y_HCOEF0_REGN [n] = 0x40000000;	// 0x200
		DE_MIXER0_VSU1->VSU_Y_HCOEF1_REGN [n] = 0;			// 0x300
		DE_MIXER0_VSU1->VSU_Y_VCOEF_REGN [n] = 0x00004000;	// 0x400
		DE_MIXER0_VSU1->VSU_C_HCOEF0_REGN [n] = 0x40000000;	// 0x600
		DE_MIXER0_VSU1->VSU_C_HCOEF1_REGN [n] = 0;			// 0x700
		DE_MIXER0_VSU1->VSU_C_VCOEF_REGN [n] = 0x00004000;	// 0x800
	}

	DE_MIXER0_VSU1->VSU_CTRL_REG = (UINT32_C(1) << 0) | (UINT32_C(1) << 4);

	DE_MIXER0_GLB->GLB_DBUFFER = (UINT32_C(1) << 0);
}

void de2_vi_init(const videomode_t * vdmode, uintptr_t fb)
{
	const uint32_t APPDIMS_SIZE = ((DIM_Y - 1) << 16) | (DIM_X - 1);

	// The output takes a 480x270 area from a total 512x302
	// buffer leaving a 16px overscan on all 4 sides.
	DE_MIXER0_VI1->CFG [0].ATTR = (UINT32_C(1) << 15) | (UINT32_C(1) << 0);
	DE_MIXER0_VI1->CFG [0].SIZE = APPDIMS_SIZE;

	DE_MIXER0_VI1->CFG [0].COORD = 0;
	DE_MIXER0_VI1->CFG [0].PITCH [0] = DIM_X * APPDPIXELWIDTH; // Scan line in bytes including overscan
	DE_MIXER0_VI1->CFG [0].TOP_LADDR [0] = fb;

	DE_MIXER0_VI1->OVL_SIZE [0] = APPDIMS_SIZE;	// OVL_V_SIZE
}


static RAMNC uint8_t xxfb1 [DIM_Y * DIM_X * APPDPIXELWIDTH];

void h3_de2_init(const videomode_t * vdmode)
{
	de2_top_init();
	de2_bld_init(vdmode);
	de2_vi_init(vdmode, (uintptr_t) xxfb1);
	de2_vsu_init(vdmode);
}

void st_clock(void)
{
	// Set up shared and dedicated clocks for HDMI, LCD/TCON and DE2
	// PLL_DE_CTRL      = (UINT32_C(1) << 31) | (UINT32_C(1) << 24) | (17 << 8) | (0 << 0); // 432MHz
	/// PLL_VIDEO_CTRL   = (UINT32_C(1) << 31) | (UINT32_C(1) << 25) | (UINT32_C(1) << 24) | (98 << 8) | (7 << 0); // 297MHz
	CCU->BUS_CLK_GATING_REG1 |=
	/*(UINT32_C(1) << 12) | */(UINT32_C(1) << 11) /*| (UINT32_C(1) << 3)*/; // Enable DE, HDMI, TCON0
	CCU->BUS_SOFT_RST_REG1 |=
	/*(UINT32_C(1) << 12) | */(3 << 10)/* | (UINT32_C(1) << 3)*/; // De-assert reset of DE, HDMI0/1, TCON0
	//DE_CLK           = (UINT32_C(1) << 31) | (UINT32_C(1) << 24); // Enable DE clock, set source to PLL_DE
	// HDMI_CLK         = (UINT32_C(1) << 31); // Enable HDMI clk (use PLL3)
	CCU->HDMI_SLOW_CLK_REG = (UINT32_C(1) << 31); // Enable HDMI slow clk
	// TCON0_CLK        = (UINT32_C(1) << 31) | 1; // 1-1980,2-2080 3-3080,3 Enable TCON0 clk, divide by 4
}


static void h3_hdmi_dump(void)
{
	/* enable read access to HDMI controller */
	HDMI_PHY->HDMI_PHY_READ_EN = 0x54524545;
	/* descramble register offsets */
	HDMI_PHY->HDMI_PHY_UNSCRAMBLE = 0x42494E47;

	/*PRINTF("HDMI_PHY\n");
	 PRINTF(" POL        %08X\n", H3_HDMI_PHY->POL);
	 PRINTF(" READ_EN    %08X\n", H3_HDMI_PHY->READ_EN);
	 PRINTF(" UNSCRAMBLE %08X\n", H3_HDMI_PHY->UNSCRAMBLE);
	 PRINTF(" CTRL       %08X\n", H3_HDMI_PHY->CTRL);
	 PRINTF(" UNK1       %08X\n", H3_HDMI_PHY->UNK1);
	 PRINTF(" UNK2       %08X\n", H3_HDMI_PHY->UNK2);
	 PRINTF(" PLL        %08X\n", HHDMI_WT_2200Y->PLL);
	 PRINTF(" CLK        %08X\n", H3_HDMI_PHY->CLK);
	 PRINTF(" UNK3       %08X\n", H3_HDMI_PHY->UNK3);*/

	PRINTF(" STATUS  %08X\n", (unsigned) HDMI_PHY->HDMI_PHY_STS);

	PRINTF("HDMI_PHY->CEC_VERSION=%08X\n", (unsigned) HDMI_PHY->CEC_VERSION);
	PRINTF("HDMI_PHY->VERSION=%08X\n", (unsigned) HDMI_PHY->VERSION);
}

// This function initializes the HDMI port and TCON.
// Almost everything here is resolution specific and
// currently hardcoded to 1920x1080@60Hz.
static void h3_display_init_ex(void)
{
	const videomode_t * vdmode_HDMI = get_videomode_HDMI();
	//active_buffer = framebuffer1;
	h3_display_clocks_init(vdmode_HDMI);
	///st_clock();
	h3_hdmi_init(vdmode_HDMI);
	h3_hdmi_dump();
	h3_hdmi_tcon_init(vdmode_HDMI);
	h3_de2_init(vdmode_HDMI);
}

void h3_hdmi_test(void)
{
	h3_display_init_ex();
}

void h3_stamp_fb(uintptr_t fb)
{
	memcpy(xxfb1, (void *) fb, sizeof xxfb1);
	dcache_clean((uintptr_t) xxfb1, sizeof xxfb1);
}

#endif /* CPUSTYLE_H3 */
