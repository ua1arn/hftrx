
#include "hardware.h"

#if CPUSTYLE_H3

#include "formats.h"

#include "h3_hdmi.h"

static RAMNC uint8_t xxfb1 [512 * 512 * 4];
static RAMNC uint8_t xxfb2 [512 * 512 * 4];

#define  framebuffer1 ((uintptr_t) xxfb1) // 0x43000000
#define  framebuffer2 ((uintptr_t) xxfb2) // (0x45000000+(512*512))

void DrawCircleCorner(int16_t x0, int16_t y0, int16_t r, uint8_t corner, uint32_t color);
void FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta,  uint32_t color);
void hdmi_dump(void);

void display_clocks_init(void) {
  // Set up shared and dedicated clocks for HDMI, LCD/TCON and DE2
  CCU->PLL_DE_CTRL_REG      = (UINT32_C(1) << 31) | (UINT32_C(1) << 24) | ((18-1) * (UINT32_C(1) << 8)) | ((1-1) * (UINT32_C(1) << 0)); // 432MHz
  CCU->PLL_VIDEO_CTRL_REG   = (UINT32_C(1) << 31) | (UINT32_C(1) << 25) | (UINT32_C(1) << 24) | ((99-1) * (UINT32_C(1) << 8)) | ((8-1) * (UINT32_C(1) << 0)); // 297MHz
  CCU->BUS_CLK_GATING_REG1 |= (UINT32_C(1) << 12) | (UINT32_C(1) << 11) | (UINT32_C(1) << 3); // Enable DE, HDMI, TCON0
  CCU->BUS_SOFT_RST_REG1   |= (UINT32_C(1) << 12) | ( UINT32_C(1) << 11) | ( UINT32_C(1) << 10) | (UINT32_C(1) << 3); // De-assert reset of DE, HDMI0/1, TCON0
  CCU->DE_CLK_REG           = (UINT32_C(1) << 31) | (UINT32_C(1) << 24); // Enable DE clock, set source to PLL_DE
  CCU->HDMI_CLK_REG         = (UINT32_C(1) << 31); // Enable HDMI clk (use PLL3)
  CCU->HDMI_SLOW_CLK_REG    = (UINT32_C(1) << 31); // Enable HDMI slow clk
  CCU->TCON0_CLK_REG        = (UINT32_C(1) << 31) | 1; // 1-1980,2-2080 3-3080,3 Enable TCON0 clk, divide by 4
  local_delay_ms(50);
}

void hdmi_init(void) {
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

	while((HDMI_PHY->HDMI_PHY_STS & 0x80) == 0)
		;

	HDMI_PHY->HDMI_PHY_CFG1 |= (0xf << 4);
	HDMI_PHY->HDMI_PHY_CFG1 |= (0xf << 8);
	HDMI_PHY->HDMI_PHY_CFG3 |= (UINT32_C(1) << 0) | (UINT32_C(1) << 2);

	HDMI_PHY->HDMI_PHY_PLL1 &= ~(UINT32_C(1) << 26);
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
	HDMI_TX0->HDMI_FC_INHACTV0 = (1920 & 0xff);    // Horizontal pixels
	HDMI_TX0->HDMI_FC_INHACTV1 = (1920 >> 8);      // Horizontal pixels
	HDMI_TX0->HDMI_FC_INHBLANK0 = (280 & 0xff);     // Horizontal blanking
	HDMI_TX0->HDMI_FC_INHBLANK1 = (280 >> 8);       // Horizontal blanking

	HDMI_TX0->HDMI_FC_INVACTV0 = (1080 & 0xff);    // Vertical pixels
	HDMI_TX0->HDMI_FC_INVACTV1 = (1080 >> 8);      // Vertical pixels
	HDMI_TX0->HDMI_FC_INVBLANK  = 45;               // Vertical blanking

	HDMI_TX0->HDMI_FC_HSYNCINDELAY0 = (88 & 0xff);  // Horizontal Front porch
	HDMI_TX0->HDMI_FC_HSYNCINDELAY1 = (88 >> 8);    // Horizontal Front porch
	HDMI_TX0->HDMI_FC_VSYNCINDELAY  = 4;            // Vertical front porch
	HDMI_TX0->HDMI_FC_HSYNCINWIDTH0 = (44 & 0xff);  // Horizontal sync pulse
	HDMI_TX0->HDMI_FC_HSYNCINWIDTH1 = (44 >> 8);    // Horizontal sync pulse
	HDMI_TX0->HDMI_FC_VSYNCINWIDTH  = 5;            // Vertical sync pulse

	HDMI_TX0->HDMI_FC_CTRLDUR    = 12;   // Frame Composer Control Period Duration
	HDMI_TX0->HDMI_FC_EXCTRLDUR  = 32;   // Frame Composer Extended Control Period Duration
	HDMI_TX0->HDMI_FC_EXCTRLSPAC = 1;    // Frame Composer Extended Control Period Maximum Spacing
	HDMI_TX0->HDMI_FC_CH0PREAM   = 0x0b; // Frame Composer Channel 0 Non-Preamble Data
	HDMI_TX0->HDMI_FC_CH1PREAM   = 0x16; // Frame Composer Channel 1 Non-Preamble Data
	HDMI_TX0->HDMI_FC_CH2PREAM   = 0x21; // Frame Composer Channel 2 Non-Preamble Data
	HDMI_TX0->HDMI_MC_FLOWCTRL   = 0;    // Main Controller Feed Through Control
	HDMI_TX0->HDMI_MC_CLKDIS     = 0x74; // Main Controller Synchronous Clock Domain Disable


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
void lcd_init(void) {
  // LCD0 feeds mixer0 to HDMI
	TCON0->TCON_GCTL_REG         = (UINT32_C(1) << 31);
	TCON0->TCON_GINT0_REG        = 0;
	TCON0->TCON1_CTL_REG    = (UINT32_C(1) << 31) | (30 << 4);
	TCON0->TCON1_BASIC0_REG = (1919 << 16) | 1079;
	TCON0->TCON1_BASIC1_REG = (1919 << 16) | 1079;
	TCON0->TCON1_BASIC2_REG = (1919 << 16) | 1079;
	TCON0->TCON1_BASIC3_REG = (2199 << 16) | 191;
	TCON0->TCON1_BASIC4_REG = (2250 << 16) | 40;
	TCON0->TCON1_BASIC5_REG = (43 << 16) | 4;
#if 0
	TCON0->TCON1_BASIC0_REG = (799 << 16) | 479;
	TCON0->TCON1_BASIC1_REG = (799 << 16) | 479;
	TCON0->TCON1_BASIC2_REG = (799 << 16) | 479;
	TCON0->TCON1_BASIC3_REG = (1055 << 16) | 191;
	TCON0->TCON1_BASIC4_REG = (850 << 16) | 140;
	TCON0->TCON1_BASIC5_REG = (127 << 16) | 40;
#endif

	TCON0->TCON_GINT1_REG = 1;
	TCON0->TCON_GINT0_REG = (UINT32_C(1) << 30);//28
  ///irq_enable(118);  // LCD0
}

// This function configured DE2 as follows:
// MIXER0 -> WB -> MIXER1 -> HDMI
uint32_t xsize,ysize;

void de2_init(void)
 {
  xsize=480-1;
  ysize=270-1;

  DE_AHB_RESET |= (UINT32_C(1) << 0)|(UINT32_C(1) << 1);///core 0,1
  DE_SCLK_GATE |= (UINT32_C(1) << 0)|(UINT32_C(1) << 1);///core 0,1
  DE_HCLK_GATE |= (UINT32_C(1) << 0)|(UINT32_C(1) << 1);///core 0,1

  DE_DE2TCON_MUX &= ~(UINT32_C(1) << 0);

  // Erase the whole of MIXER0. This contains uninitialized data.
  for(uint32_t addr = DE_MIXER0 + 0x0000; addr < DE_MIXER0 + 0xC000; addr += 4)
  {
	   *(volatile uint32_t*)(addr) = 0;
  }

  DE_MIXER0_GLB_CTL = 1;
  DE_MIXER0_GLB_SIZE = (1079 << 16) | 1919;
///DE_MIXER0_GLB_SIZE = (479 << 16) | 799;

  DE_MIXER0_BLD_FILL_COLOR_CTL = 0x100;
  DE_MIXER0_BLD_CH_RTCTL = 0;
  DE_MIXER0_BLD_SIZE = (1079 << 16) | 1919;
  DE_MIXER0_BLD_CH_ISIZE(0) = (1079 << 16) | 1919;
 /// DE_MIXER0_BLD_SIZE = (479 << 16) | 799;
 /// DE_MIXER0_BLD_CH_ISIZE(0) = (489 << 16) | 799;
  // The output takes a 480x270 area from a total 512x302
  // buffer leaving a 16px overscan on all 4 sides.
  DE_MIXER0_OVL_V_ATTCTL(0) = (UINT32_C(1) << 15) | (UINT32_C(1) << 0);
  DE_MIXER0_OVL_V_MBSIZE(0) = (ysize << 16) | xsize;

  DE_MIXER0_OVL_V_COOR(0) = 0;
  DE_MIXER0_OVL_V_PITCH0(0) = 512*4; // Scan line in bytes including overscan
 /// DE_MIXER0_OVL_V_PITCH0(0) = 512*2; // Scan line in bytes including overscan
  DE_MIXER0_OVL_V_TOP_LADD0(0) = (uint32_t)(framebuffer1+ 512*16+16);///[512*16+16]; // Start at y=16

  DE_MIXER0_OVL_V_SIZE = (ysize << 16) | xsize;

  DE_MIXER0_VS_CTRL = 1;
  DE_MIXER0_VS_OUT_SIZE = (1079 << 16) | 1919;
  DE_MIXER0_VS_Y_SIZE = (ysize << 16) | xsize;
  DE_MIXER0_VS_Y_HSTEP = 0x40000;
  DE_MIXER0_VS_Y_VSTEP = 0x40000;
  DE_MIXER0_VS_C_SIZE = (ysize << 16) | xsize;
  DE_MIXER0_VS_C_HSTEP = 0x40000;
  DE_MIXER0_VS_C_VSTEP = 0x40000;

  for(int n=0;n<32;n++)
  {
    DE_MIXER0_VS_Y_HCOEF0(n) = 0x40000000;
    DE_MIXER0_VS_Y_HCOEF1(n) = 0;
    DE_MIXER0_VS_Y_VCOEF(n)  = 0x00004000;
    DE_MIXER0_VS_C_HCOEF0(n) = 0x40000000;
    DE_MIXER0_VS_C_HCOEF1(n) = 0;
    DE_MIXER0_VS_C_VCOEF(n)  = 0x00004000;
  }


  DE_MIXER0_VS_CTRL = 1 | (UINT32_C(1) << 4);
  DE_MIXER0_GLB_DBUFFER = 1;
}

void st_clock(void) {
  // Set up shared and dedicated clocks for HDMI, LCD/TCON and DE2
 // PLL_DE_CTRL      = (UINT32_C(1) << 31) | (UINT32_C(1) << 24) | (17 << 8) | (0 << 0); // 432MHz
 /// PLL_VIDEO_CTRL   = (UINT32_C(1) << 31) | (UINT32_C(1) << 25) | (UINT32_C(1) << 24) | (98 << 8) | (7 << 0); // 297MHz
  CCU->BUS_CLK_GATING_REG1 |= /*(UINT32_C(1) << 12) | */(UINT32_C(1) << 11) /*| (UINT32_C(1) << 3)*/; // Enable DE, HDMI, TCON0
  CCU->BUS_SOFT_RST_REG1   |= /*(UINT32_C(1) << 12) | */(3 << 10)/* | (UINT32_C(1) << 3)*/; // De-assert reset of DE, HDMI0/1, TCON0
  //DE_CLK           = (UINT32_C(1) << 31) | (UINT32_C(1) << 24); // Enable DE clock, set source to PLL_DE
 // HDMI_CLK         = (UINT32_C(1) << 31); // Enable HDMI clk (use PLL3)
  CCU->HDMI_SLOW_CLK_REG    = (UINT32_C(1) << 31); // Enable HDMI slow clk
 // TCON0_CLK        = (UINT32_C(1) << 31) | 1; // 1-1980,2-2080 3-3080,3 Enable TCON0 clk, divide by 4
}
// This function initializes the HDMI port and TCON.
// Almost everything here is resolution specific and
// currently hardcoded to 1920x1080@60Hz.
void display_init_ex(void)
{
  //active_buffer = framebuffer1;
  display_clocks_init();
  ///st_clock();
  hdmi_init();
  hdmi_dump();
  lcd_init();
  de2_init();
}

void UB_LCD_FillLayer(uint32_t color)
{
    uint32_t index = 0;

    // Bildschirm loeschen
    for (index = 0x00; index < 4*512*270; index+=4)
    {
        *(volatile uint32_t*)(framebuffer1 +512*16+16+ index) = color;
    }


}
///-------------

void hdmi_dump(void) {
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
	PRINTF(" PLL        %08X\n", H3_HDMI_PHY->PLL);
	PRINTF(" CLK        %08X\n", H3_HDMI_PHY->CLK);
	PRINTF(" UNK3       %08X\n", H3_HDMI_PHY->UNK3);*/

	PRINTF(" STATUS  %08X\n",(unsigned) HDMI_PHY->HDMI_PHY_STS);


	PRINTF("HDMI_PHY->CEC_VERSION=%08X\n", (unsigned) HDMI_PHY->CEC_VERSION);
	PRINTF("HDMI_PHY->VERSION=%08X\n", (unsigned) HDMI_PHY->VERSION);
}


void h3_hdmi_test(void)
{
	display_init_ex();
	UB_LCD_FillLayer(0x00FF00);	// GREEN
}

#endif /* CPUSTYLE_H3 */
