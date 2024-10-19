
#include <stdint.h>
#include "ccu.h"
//#include "system.h"
#include "lcd.h"
#include "main.h"

RAMNC uint8_t xxfb1 [512 * 512 * 4];
RAMNC uint8_t xxfb2 [512 * 512 * 4];
RAMNC uint8_t xxfb3 [512 * 512 * 4];


void DrawCircleCorner(int16_t x0, int16_t y0, int16_t r, uint8_t corner, uint32_t color);
void FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta,  uint32_t color);
void hdmi_dump(void);

void display_clocks_init(void) {
  // Set up shared and dedicated clocks for HDMI, LCD/TCON and DE2
  PLL_DE_CTRL      = (1<<31) | (1<<24) | (17<<8) | (0<<0); // 432MHz
  PLL_VIDEO_CTRL   = (1<<31) | (1<<25) | (1<<24) | (98<<8) | (7<<0); // 297MHz
  BUS_CLK_GATING1 |= (1<<12) | (1<<11) | (1<<3); // Enable DE, HDMI, TCON0
  BUS_SOFT_RST1   |= (1<<12) | (3<<10) | (1<<3); // De-assert reset of DE, HDMI0/1, TCON0
  DE_CLK           = (1<<31) | (1<<24); // Enable DE clock, set source to PLL_DE
  HDMI_CLK         = (1<<31); // Enable HDMI clk (use PLL3)
  HDMI_SLOW_CLK    = (1<<31); // Enable HDMI slow clk
  TCON0_CLK        = (1<<31) | 1; // 1-1980,2-2080 3-3080,3 Enable TCON0 clk, divide by 4
}

void hdmi_init(void) {
	TP();
  // HDMI PHY init, the following black magic is based on the procedure documented at:
  // http://linux-sunxi.org/images/3/38/AW_HDMI_TX_PHY_S40_Spec_V0.1.pdf
  HDMI_PHY_CFG1 = 0;
  HDMI_PHY_CFG1 = 1;
  local_delay_us(5);
  HDMI_PHY_CFG1 |= (1<<16);
  HDMI_PHY_CFG1 |= (1<<1);
  local_delay_us(10);
  HDMI_PHY_CFG1 |= (1<<2);
  local_delay_us(5);
  HDMI_PHY_CFG1 |= (1<<3);
  local_delay_us(40);
  HDMI_PHY_CFG1 |= (1<<19);
  local_delay_us(100);
  HDMI_PHY_CFG1 |= (1<<18);
  HDMI_PHY_CFG1 |= (7<<4);
	TP();
 while((HDMI_PHY_STS & 0x80) == 0)
	 ;
	TP();

  HDMI_PHY_CFG1 |= (0xf<<4);
  HDMI_PHY_CFG1 |= (0xf<<8);
  HDMI_PHY_CFG3 |= (1<<0) | (1<<2);

  HDMI_PHY_PLL1 &= ~(1<<26);
  HDMI_PHY_CEC = 0;

  HDMI_PHY_PLL1 = 0x39dc5040;
  HDMI_PHY_PLL2 = 0x80084381;
  local_delay_us(10000);
  HDMI_PHY_PLL3 = 1;
  HDMI_PHY_PLL1 |= (1<<25);
  local_delay_us(10000);
  uint32_t tmp = (HDMI_PHY_STS & 0x1f800) >> 11;
  HDMI_PHY_PLL1 |= (1<<31) | (1<<30) | tmp;

  HDMI_PHY_CFG1 = 0x01FFFF7F;
  HDMI_PHY_CFG2 = 0x8063A800;
  HDMI_PHY_CFG3 = 0x0F81C485;

  /* enable read access to HDMI controller */
  HDMI_PHY_READ_EN = 0x54524545;
  /* descramble register offsets */
  HDMI_PHY_UNSCRAMBLE = 0x42494E47;

  // HDMI Config, based on the documentation at:
  // https://people.freebsd.org/~gonzo/arm/iMX6-HDMI.pdf
  HDMI_FC_INVIDCONF = (1<<6) | (1<<5) | (1<<4) | (1<<3); // Polarity etc
  HDMI_FC_INHACTIV0 = (1920 & 0xff);    // Horizontal pixels
  HDMI_FC_INHACTIV1 = (1920 >> 8);      // Horizontal pixels
  HDMI_FC_INHBLANK0 = (280 & 0xff);     // Horizontal blanking
  HDMI_FC_INHBLANK1 = (280 >> 8);       // Horizontal blanking

  HDMI_FC_INVACTIV0 = (1080 & 0xff);    // Vertical pixels
  HDMI_FC_INVACTIV1 = (1080 >> 8);      // Vertical pixels
  HDMI_FC_INVBLANK  = 45;               // Vertical blanking

  HDMI_FC_HSYNCINDELAY0 = (88 & 0xff);  // Horizontal Front porch
  HDMI_FC_HSYNCINDELAY1 = (88 >> 8);    // Horizontal Front porch
  HDMI_FC_VSYNCINDELAY  = 4;            // Vertical front porch
  HDMI_FC_HSYNCINWIDTH0 = (44 & 0xff);  // Horizontal sync pulse
  HDMI_FC_HSYNCINWIDTH1 = (44 >> 8);    // Horizontal sync pulse
  HDMI_FC_VSYNCINWIDTH  = 5;            // Vertical sync pulse

  HDMI_FC_CTRLDUR    = 12;   // Frame Composer Control Period Duration
  HDMI_FC_EXCTRLDUR  = 32;   // Frame Composer Extended Control Period Duration
  HDMI_FC_EXCTRLSPAC = 1;    // Frame Composer Extended Control Period Maximum Spacing
  HDMI_FC_CH0PREAM   = 0x0b; // Frame Composer Channel 0 Non-Preamble Data
  HDMI_FC_CH1PREAM   = 0x16; // Frame Composer Channel 1 Non-Preamble Data
  HDMI_FC_CH2PREAM   = 0x21; // Frame Composer Channel 2 Non-Preamble Data
  HDMI_MC_FLOWCTRL   = 0;    // Main Controller Feed Through Control
  HDMI_MC_CLKDIS     = 0x74; // Main Controller Synchronous Clock Domain Disable


/*  HDMI_FC_INVIDCONF = (1<<6) | (1<<5) | (1<<4) | (1<<3); // Polarity etc
  HDMI_FC_INHACTIV0 = (800 & 0xff);    // Horizontal pixels
  HDMI_FC_INHACTIV1 = (800 >> 8);      // Horizontal pixels
  HDMI_FC_INHBLANK0 = (256 & 0xff);     // Horizontal blanking
  HDMI_FC_INHBLANK1 = (256 >> 8);       // Horizontal blanking

  HDMI_FC_INVACTIV0 = (480 & 0xff);    // Vertical pixels
  HDMI_FC_INVACTIV1 = (480 >> 8);      // Vertical pixels
  HDMI_FC_INVBLANK  = 28;               // Vertical blanking

  HDMI_FC_HSYNCINDELAY0 = (40 & 0xff);  // Horizontal Front porch
  HDMI_FC_HSYNCINDELAY1 = (40 >> 8);    // Horizontal Front porch
  HDMI_FC_VSYNCINDELAY  = 1;            // Vertical front porch
  HDMI_FC_HSYNCINWIDTH0 = (128 & 0xff);  // Horizontal sync pulse
  HDMI_FC_HSYNCINWIDTH1 = (128 >> 8);    // Horizontal sync pulse
  HDMI_FC_VSYNCINWIDTH  = 4;            // Vertical sync pulse

  HDMI_FC_CTRLDUR    = 12;   // Frame Composer Control Period Duration
  HDMI_FC_EXCTRLDUR  = 32;   // Frame Composer Extended Control Period Duration
  HDMI_FC_EXCTRLSPAC = 1;    // Frame Composer Extended Control Period Maximum Spacing
  HDMI_FC_CH0PREAM   = 0x0b; // Frame Composer Channel 0 Non-Preamble Data
  HDMI_FC_CH1PREAM   = 0x16; // Frame Composer Channel 1 Non-Preamble Data
  HDMI_FC_CH2PREAM   = 0x21; // Frame Composer Channel 2 Non-Preamble Data
  HDMI_MC_FLOWCTRL   = 0;    // Main Controller Feed Through Control
  HDMI_MC_CLKDIS     = 0x74; // Main Controller Synchronous Clock Domain Disable
*/
  /*800x480p hfp:40 hs:48 hbp:40 vfp:13 vs:3 vbp:29 pixel clock:32 MHz

      hltdc.Init.HorizontalSync = 19;
    hltdc.Init.VerticalSync = 2;
    hltdc.Init.AccumulatedHBP = 159;
    hltdc.Init.AccumulatedVBP = 22;
    hltdc.Init.AccumulatedActiveW = 1183;
    hltdc.Init.AccumulatedActiveH = 622;
    hltdc.Init.TotalWidth = 1343;
    hltdc.Init.TotalHeigh = 634;
    */

}
void lcd_init(void) {
  // LCD0 feeds mixer0 to HDMI
  LCD0_GCTL         = (1<<31);
  LCD0_GINT0        = 0;
  LCD0_TCON1_CTL    = (1<<31) | (30<<4);
  LCD0_TCON1_BASIC0 = (1919<<16) | 1079;
  LCD0_TCON1_BASIC1 = (1919<<16) | 1079;
  LCD0_TCON1_BASIC2 = (1919<<16) | 1079;
  LCD0_TCON1_BASIC3 = (2199<<16) | 191;
  LCD0_TCON1_BASIC4 = (2250<<16) | 40;
  LCD0_TCON1_BASIC5 = (43<<16) | 4;
 /* LCD0_TCON1_BASIC0 = (799<<16) | 479;
  LCD0_TCON1_BASIC1 = (799<<16) | 479;
  LCD0_TCON1_BASIC2 = (799<<16) | 479;
  LCD0_TCON1_BASIC3 = (1055<<16) | 191;
  LCD0_TCON1_BASIC4 = (850<<16) | 140;
  LCD0_TCON1_BASIC5 = (127<<16) | 40;*/

  LCD0_GINT1 = 1;
  LCD0_GINT0 = (1<<30);//28
  ///irq_enable(118);  // LCD0
}

// This function configured DE2 as follows:
// MIXER0 -> WB -> MIXER1 -> HDMI
uint32_t xsize,ysize;

void de2_init(void)
 {
  xsize=480-1;
  ysize=270-1;

  DE_AHB_RESET |= (1<<0)|(1<<1);///core 0,1
  DE_SCLK_GATE |= (1<<0)|(1<<1);///core 0,1
  DE_HCLK_GATE |= (1<<0)|(1<<1);///core 0,1
  DE_DE2TCON_MUX &= ~(1<<0);

  // Erase the whole of MIXER0. This contains uninitialized data.
  for(uint32_t addr = DE_MIXER0 + 0x0000; addr < DE_MIXER0 + 0xC000; addr += 4)
   *(volatile uint32_t*)(addr) = 0;

  DE_MIXER0_GLB_CTL = 1;
  DE_MIXER0_GLB_SIZE = (1079<<16) | 1919;
///DE_MIXER0_GLB_SIZE = (479<<16) | 799;

  DE_MIXER0_BLD_FILL_COLOR_CTL = 0x100;
  DE_MIXER0_BLD_CH_RTCTL = 0;
  DE_MIXER0_BLD_SIZE = (1079<<16) | 1919;
  DE_MIXER0_BLD_CH_ISIZE(0) = (1079<<16) | 1919;
 /// DE_MIXER0_BLD_SIZE = (479<<16) | 799;
 /// DE_MIXER0_BLD_CH_ISIZE(0) = (489<<16) | 799;
  // The output takes a 480x270 area from a total 512x302
  // buffer leaving a 16px overscan on all 4 sides.
  DE_MIXER0_OVL_V_ATTCTL(0) = (1<<15) | (1<<0);
  DE_MIXER0_OVL_V_MBSIZE(0) = (ysize<<16) | xsize;

  DE_MIXER0_OVL_V_COOR(0) = 0;
  DE_MIXER0_OVL_V_PITCH0(0) = 512*4; // Scan line in bytes including overscan
 /// DE_MIXER0_OVL_V_PITCH0(0) = 512*2; // Scan line in bytes including overscan
  DE_MIXER0_OVL_V_TOP_LADD0(0) = (uint32_t)(framebuffer1+ 512*16+16);///[512*16+16]; // Start at y=16

  DE_MIXER0_OVL_V_SIZE = (ysize<<16) | xsize;

  DE_MIXER0_VS_CTRL = 1;
  DE_MIXER0_VS_OUT_SIZE = (1079<<16) | 1919;
  DE_MIXER0_VS_Y_SIZE = (ysize<<16) | xsize;
  DE_MIXER0_VS_Y_HSTEP = 0x40000;
  DE_MIXER0_VS_Y_VSTEP = 0x40000;
  DE_MIXER0_VS_C_SIZE = (ysize<<16) | xsize;
  DE_MIXER0_VS_C_HSTEP = 0x40000;
  DE_MIXER0_VS_C_VSTEP = 0x40000;
  for(int n=0;n<32;n++) {
    DE_MIXER0_VS_Y_HCOEF0(n) = 0x40000000;
    DE_MIXER0_VS_Y_HCOEF1(n) = 0;
    DE_MIXER0_VS_Y_VCOEF(n)  = 0x00004000;
    DE_MIXER0_VS_C_HCOEF0(n) = 0x40000000;
    DE_MIXER0_VS_C_HCOEF1(n) = 0;
    DE_MIXER0_VS_C_VCOEF(n)  = 0x00004000;
  }


  DE_MIXER0_VS_CTRL = 1 | (1<<4);
  DE_MIXER0_GLB_DBUFFER = 1;
}

void st_clock(void) {
  // Set up shared and dedicated clocks for HDMI, LCD/TCON and DE2
 // PLL_DE_CTRL      = (1<<31) | (1<<24) | (17<<8) | (0<<0); // 432MHz
 /// PLL_VIDEO_CTRL   = (1<<31) | (1<<25) | (1<<24) | (98<<8) | (7<<0); // 297MHz
  BUS_CLK_GATING1 |= /*(1<<12) | */(1<<11) /*| (1<<3)*/; // Enable DE, HDMI, TCON0
  BUS_SOFT_RST1   |= /*(1<<12) | */(3<<10)/* | (1<<3)*/; // De-assert reset of DE, HDMI0/1, TCON0
  //DE_CLK           = (1<<31) | (1<<24); // Enable DE clock, set source to PLL_DE
 // HDMI_CLK         = (1<<31); // Enable HDMI clk (use PLL3)
  HDMI_SLOW_CLK    = (1<<31); // Enable HDMI slow clk
 // TCON0_CLK        = (1<<31) | 1; // 1-1980,2-2080 3-3080,3 Enable TCON0 clk, divide by 4
}
// This function initializes the HDMI port and TCON.
// Almost everything here is resolution specific and
// currently hardcoded to 1920x1080@60Hz.
void display_init_ex(void) {
  //active_buffer = framebuffer1;
  display_clocks_init();
  ///st_clock();
  hdmi_init();
  hdmi_dump();
  lcd_init();
  de2_init();
}

void buffer_swap(void) {
  DE_MIXER0_OVL_V_TOP_LADD0(0) = (uint32_t)(active_buffer + 512*16+16);
  if(active_buffer == framebuffer1) {
      active_buffer = framebuffer2;
  } else if(active_buffer == framebuffer2) {
      active_buffer = framebuffer3;
  } else {
      active_buffer = framebuffer1;
  }
  // Blank visible area
  for(int n=512*16; n<512*(270+16); n++)
   /// active_buffer[n] = 0;
  DE_MIXER0_GLB_DBUFFER = 1;
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

///------------------
///--------------GRAF------

int16_t P_Graphic2D_sgn(int16_t x)
{
    return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}
//--------------------------------------------------------------
// convertiert RGB565 in ARGB8888
// fР“С�r RGB565 BUG
//--------------------------------------------------------------
uint32_t P_convert565to8888(uint16_t c)
{
    uint32_t ret_wert;
    uint32_t Red_Value = 0, Green_Value = 0, Blue_Value = 0;

    Red_Value = (0xF800 & c) << 8;
    Green_Value = (0x07E0 & c) << 5;
    Blue_Value = (0x001F & c) << 3;

    ret_wert=0xFF000000;///alpha
    ret_wert|=Red_Value;
    ret_wert|=Green_Value;
    ret_wert|=Blue_Value;

    return ret_wert;
}

uint16_t P_convert8888to565(uint32_t c)
{
    uint16_t ret_wert;

    ret_wert=(c>>8&0xf800)|(c>>5&0x07e0)|(c>>3&0x001f);

    return ret_wert;
}
void DrawPixelNormal(uint16_t xp, uint16_t yp, uint32_t c)
{
    //if(xp>=LCDX) return;
    //if(yp>=LCDY) return;

*(volatile uint32_t*)(framebuffer1 +512*16+16+ (4*xp+(LCDX*4*yp))) = c;

}

void DrawLineNormal(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
    int16_t x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, es, el, err;

    // check auf Limit
    if(x1>=LCDX) x1=LCDX-1;
    if(y1>=LCDY) y1=LCDY-1;

    // check auf Limit
    if(x2>=LCDX) x2=LCDX-1;
    if(y2>=LCDY) y2=LCDY-1;

    // Entfernung in beiden Dimensionen berechnen
    dx = x2 - x1;
    dy = y2 - y1;

    // Vorzeichen des Inkrements bestimmen
    incx = P_Graphic2D_sgn(dx);
    incy = P_Graphic2D_sgn(dy);
    if(dx<0) dx = -dx;
    if(dy<0) dy = -dy;

    // feststellen, welche Entfernung grР В РІР‚СљР вЂ™Р’В¶Р В РІР‚СљР РЋРЎСџer ist
    if (dx>dy)
    {
        // x ist schnelle Richtung
        pdx=incx;
        pdy=0;    // pd. ist Parallelschritt
        ddx=incx;
        ddy=incy; // dd. ist Diagonalschritt
        es =dy;
        el =dx;   // Fehlerschritte schnell, langsam
    }
    else
    {
        // y ist schnelle Richtung
        pdx=0;
        pdy=incy; // pd. ist Parallelschritt
        ddx=incx;
        ddy=incy; // dd. ist Diagonalschritt
        es =dx;
        el =dy;   // Fehlerschritte schnell, langsam
    }

    // Initialisierungen vor Schleifenbeginn
    x = x1;
    y = y1;
    err = (el>>1);
    DrawPixelNormal(x,y,color);

    // Pixel berechnen
    for(t=0; t<el; ++t) // t zaehlt die Pixel, el ist auch Anzahl
    {
        // Aktualisierung Fehlerterm
        err -= es;
        if(err<0)
        {
            // Fehlerterm wieder positiv (>=0) machen
            err += el;
            // Schritt in langsame Richtung, Diagonalschritt
            x += ddx;
            y += ddy;
        }
        else
        {
            // Schritt in schnelle Richtung, Parallelschritt
            x += pdx;
            y += pdy;
        }
        DrawPixelNormal(x,y,color);
    }
}

void DrawRect(uint16_t xp, uint16_t yp, uint16_t w, uint16_t h, uint32_t c)
{

    // check auf Limit
    if(xp>=LCDX) xp=LCDX-1;
    if(yp>=LCDY) yp=LCDY-1;
    if(w==0) return;
    if(h==0) return;


    // check auf Limit
    if((xp+w)>LCDX) w=LCDX-xp;
    if((yp+h)>LCDY) h=LCDY-yp;


    DrawLineNormal(xp, yp, xp+w,yp, c);
    DrawLineNormal(xp, yp+h, xp+w, yp+h, c);
    DrawLineNormal(xp, yp, xp, yp+h,c);
    DrawLineNormal(xp+w, yp, xp+w, yp+h, c);

}

void FillRectangle(uint16_t xp, uint16_t yp, uint16_t w, uint16_t h, uint32_t c)
{
     // check auf Limit
    if(xp>=LCDX) xp=LCDX-1;
    if(yp>=LCDY) yp=LCDY-1;
    if(w==0) return;
    if(h==0) return;

 uint16_t xc;

 for(xc=xp;xc<=(xp+w);xc++)
 {
   DrawLineNormal(xc, yp, xc, yp+h, c);
 }

}
void DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint32_t color)
{
    // smarter version
    ///DMA_DrawStraightDMA565(x + r  , y    , w - r - r,LCD_DIR_HORIZONTAL, color); // Top
    DrawLineNormal(x + r  , y    ,x + r+w - r - r,y,color);
    ///DMA_DrawStraightDMA565(x + r  , y + h - 1, w - r - r,LCD_DIR_HORIZONTAL,color); // Bottom
    DrawLineNormal(x + r  , y + h - 1   ,x + r + w - r - r,y + h - 1 ,color);
    ///DMA_DrawStraightDMA565(x    , y + r  , h - r - r,LCD_DIR_VERTICAL, color); // Left
    DrawLineNormal(x  , y + r   ,x ,y + r + h - r - r,color);
    ///DMA_DrawStraightDMA565(x + w - 1, y + r  , h - r - r,LCD_DIR_VERTICAL,color); // Right
    DrawLineNormal(x + w - 1 , y + r   ,x + w - 1,y + r + h - r - r,color);
    // draw four corners
    DrawCircleCorner(x + r    , y + r    , r, 1,color);
    DrawCircleCorner(x + w - r - 1, y + r    , r, 2,color);
    DrawCircleCorner(x + w - r - 1, y + h - r - 1, r, 4, color);
    DrawCircleCorner(x + r    , y + h - r - 1, r, 8,color);
}

void FillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,uint32_t color)
{
    // smarter version
    FillRectangle(x + r, y, w - r - r, h,color);

    // draw four corners
    FillCircleHelper(x + w - r - 1, y + r, r, 1, h - r - r - 1, color);
    FillCircleHelper(x + r    , y + r, r, 2, h - r - r - 1, color);

}
void DrawCircle(uint16_t xp, uint16_t yp, uint16_t r, uint32_t c)
{
    int16_t f=1-r, ddF_x=0, ddF_y=-2*r, x=0, y=r;

    // check auf Limit
    if(xp>=LCDX) xp=LCDX-1;
    if(yp>=LCDY) yp=LCDY-1;

    if(r==0) return;

    DrawPixelNormal(xp, yp + r, c);
    DrawPixelNormal(xp, yp - r, c);
    DrawPixelNormal(xp + r, yp,c);
    DrawPixelNormal(xp - r, yp, c);

    while(x < y)
    {
        if(f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;

        DrawPixelNormal(xp + x, yp + y, c);
        DrawPixelNormal(xp - x, yp + y, c);
        DrawPixelNormal(xp + x, yp - y, c);
        DrawPixelNormal(xp - x, yp - y, c);
        DrawPixelNormal(xp + y, yp + x, c);
        DrawPixelNormal(xp - y, yp + x, c);
        DrawPixelNormal(xp + y, yp - x, c);
        DrawPixelNormal(xp - y, yp - x, c);
    }
}

void DrawFullCircle(uint16_t xp, uint16_t yp, uint16_t r, uint32_t c)
{
    int32_t  D;
    uint32_t  CurX;
    uint32_t  CurY;
    int16_t x,y,l;

    // check auf Limit
    if(xp>=LCDX) xp=LCDX-1;
    if(yp>=LCDY) yp=LCDY-1;
    if(r==0) return;

    D = 3 - (r << 1);

    CurX = 0;
    CurY = r;

    while (CurX <= CurY)
    {
        if(CurY > 0)
        {
            x=xp - CurX;
            y=yp - CurY;
            l=2*CurY;
            if(y<0)
            {
                l+=y;
                y=0;
            }
            DrawLineNormal(x, y, x,y+l,  c);
            x=xp + CurX;
            y=yp - CurY;
            l=2*CurY;
            if(y<0)
            {
                l+=y;
                y=0;
            }
            DrawLineNormal(x, y, x,y+l,  c);
        }

        if(CurX > 0)
        {
            x=xp - CurY;
            y=yp - CurX;
            l=2*CurX;
            if(y<0)
            {
                l+=y;
                y=0;
            }
            DrawLineNormal(x, y, x,y+l,  c);
            x=xp + CurY;
            y=yp - CurX;
            l=2*CurX;
            if(y<0)
            {
                l+=y;
                y=0;
            }
            DrawLineNormal(x, y, x,y+l,  c);
        }

        if (D < 0)
        {
            D += (CurX << 2) + 6;
        }
        else
        {
            D += ((CurX - CurY) << 2) + 10;
            CurY--;
        }
        CurX++;
    }

    DrawCircle(xp, yp, r, c);
}

void DrawCircleCorner(int16_t x0, int16_t y0, int16_t r, uint8_t corner, uint32_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (corner & 0x01)  /* Top left */
        {
            DrawPixelNormal(x0 - y, y0 - x, color);
            DrawPixelNormal(x0 - x, y0 - y, color);
        }

        if (corner & 0x02)  /* Top right */
        {
            DrawPixelNormal(x0 + x, y0 - y, color);
            DrawPixelNormal(x0 + y, y0 - x, color);
        }

        if (corner & 0x04)  /* Bottom right */
        {
            DrawPixelNormal(x0 + x, y0 + y, color);
            DrawPixelNormal(x0 + y, y0 + x, color);
        }

        if (corner & 0x08)  /* Bottom left */
        {
            DrawPixelNormal(x0 - x, y0 + y, color);
            DrawPixelNormal(x0 - y, y0 + x, color);
        }
    }
}

void FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta,  uint32_t color)
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -r - r;
    int16_t x     = 0;

    delta++;
    while (x < r)
    {
        if (f >= 0)
        {
            r--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;

        if (cornername & 0x1)
        {
            DrawLineNormal(x0 + x, y0 - r,x0 + x, y0  + r + delta,  color);
            DrawLineNormal(x0 + r, y0 - x,x0 + r,y0  + x + delta, color);
        }
        if (cornername & 0x2)
        {
            DrawLineNormal(x0 - x, y0 - r,x0 - x, y0  + r + delta, color);
            DrawLineNormal(x0 - r, y0 - x,x0 - r, y0  + x + delta, color);
        }
    }
}


void CopyImg_slow(UB_Image *img, DMA2D_Koord koord)
{

  int r,s;

  if(koord.source_w==0) return;
  if(koord.source_h==0) return;

for ( s=0; s<koord.source_h; s++)
{
	for ( r=0; r<koord.source_w; r++)
		{
		 DrawPixelNormal(r+koord.source_xp,s+koord.source_yp,P_convert565to8888(img->table[r+(s*koord.source_w)]));
		}

    }

}

void draw_smetr_slow(void)
{
 DMA2D_Koord koord;

  koord.source_xp=0;
  koord.source_yp=0;
  koord.source_w=250;
  koord.source_h=120;

  CopyImg_slow(&eesm,koord);

}



///-------------
char bufdb[100];

void hdmi_dump(void) {
	/* enable read access to HDMI controller */
	HDMI_PHY_READ_EN = 0x54524545;
	/* descramble register offsets */
	HDMI_PHY_UNSCRAMBLE = 0x42494E47;

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

	sprintf(bufdb," STATUS  %08X\n",(unsigned) HDMI_PHY_STS);
    PRINTF(bufdb);

	sprintf(bufdb," CEC  %08X\n",(unsigned) CEC);
    PRINTF(bufdb);
    sprintf(bufdb," VER  %08X\n",(unsigned) VERSION);
    PRINTF(bufdb);
}


void h3_hdmi_test(void)
{
	TP();
	//fb_init();
	TP();
	display_init_ex();
	TP();

	UB_LCD_FillLayer(GREEN);
	TP();
}

