#ifndef LCD_H_
#define LCD_H_

#define VIDEO_RAM_BYTES 0x180000

// The HDMI registers base address.
#define HDMI_BASE 0x01EE0000
#define HDMI_PHY_BASE (HDMI_BASE + 0x10000)

// HDMI register helpers.
#define HDMI_PHY_POL          *(volatile uint32_t*)(HDMI_BASE + 0x10000)
#define HDMI_PHY_READ_EN      *(volatile uint32_t*)(HDMI_BASE + 0x10010)
#define HDMI_PHY_UNSCRAMBLE   *(volatile uint32_t*)(HDMI_BASE + 0x10014)
#define HDMI_PHY_CFG1         *(volatile uint32_t*)(HDMI_BASE + 0x10020)
#define HDMI_PHY_CFG2         *(volatile uint32_t*)(HDMI_BASE + 0x10024)
#define HDMI_PHY_CFG3         *(volatile uint32_t*)(HDMI_BASE + 0x10028)
#define HDMI_PHY_PLL1         *(volatile uint32_t*)(HDMI_BASE + 0x1002C)
#define HDMI_PHY_PLL2         *(volatile uint32_t*)(HDMI_BASE + 0x10030)
#define HDMI_PHY_PLL3         *(volatile uint32_t*)(HDMI_BASE + 0x10034)
#define HDMI_PHY_STS          *(volatile uint32_t*)(HDMI_BASE + 0x10038)
#define HDMI_PHY_CEC          *(volatile uint32_t*)(HDMI_BASE + 0x1003C)

#define HDMI_FC_INVIDCONF     *(volatile uint8_t*)(HDMI_BASE + 0x1000)

#define HDMI_FC_INHACTIV0     *(volatile uint8_t*)(HDMI_BASE + 0x1001)
#define HDMI_FC_INHACTIV1     *(volatile uint8_t*)(HDMI_BASE + 0x1002)
#define HDMI_FC_INHBLANK0     *(volatile uint8_t*)(HDMI_BASE + 0x1003)
#define HDMI_FC_INHBLANK1     *(volatile uint8_t*)(HDMI_BASE + 0x1004)

#define HDMI_FC_INVACTIV0     *(volatile uint8_t*)(HDMI_BASE + 0x1005)
#define HDMI_FC_INVACTIV1     *(volatile uint8_t*)(HDMI_BASE + 0x1006)
#define HDMI_FC_INVBLANK      *(volatile uint8_t*)(HDMI_BASE + 0x1007)

#define HDMI_FC_HSYNCINDELAY0 *(volatile uint8_t*)(HDMI_BASE + 0x1008)
#define HDMI_FC_HSYNCINDELAY1 *(volatile uint8_t*)(HDMI_BASE + 0x1009)
#define HDMI_FC_HSYNCINWIDTH0 *(volatile uint8_t*)(HDMI_BASE + 0x100A)
#define HDMI_FC_HSYNCINWIDTH1 *(volatile uint8_t*)(HDMI_BASE + 0x100B)
#define HDMI_FC_VSYNCINDELAY  *(volatile uint8_t*)(HDMI_BASE + 0x100C)
#define HDMI_FC_VSYNCINWIDTH  *(volatile uint8_t*)(HDMI_BASE + 0x100D)

#define HDMI_FC_CTRLDUR       *(volatile uint8_t*)(HDMI_BASE + 0x1011)
#define HDMI_FC_EXCTRLDUR     *(volatile uint8_t*)(HDMI_BASE + 0x1012)
#define HDMI_FC_EXCTRLSPAC    *(volatile uint8_t*)(HDMI_BASE + 0x1013)
#define HDMI_FC_CH0PREAM      *(volatile uint8_t*)(HDMI_BASE + 0x1014)
#define HDMI_FC_CH1PREAM      *(volatile uint8_t*)(HDMI_BASE + 0x1015)
#define HDMI_FC_CH2PREAM      *(volatile uint8_t*)(HDMI_BASE + 0x1016)
#define HDMI_MC_FLOWCTRL      *(volatile uint8_t*)(HDMI_BASE + 0x4004)
#define HDMI_MC_CLKDIS        *(volatile uint8_t*)(HDMI_BASE + 0x4001)

#define HDMI_VP_STUFF         *(volatile uint8_t*)(HDMI_BASE + 0x0802)
#define HDMI_VP_CONF          *(volatile uint8_t*)(HDMI_BASE + 0x0804)

#define HDMI_TX_INVID0        *(volatile uint8_t*)(HDMI_BASE + 0x0200)
#define HDMI_TX_INSTUFFING    *(volatile uint8_t*)(HDMI_BASE + 0x0201)

// LCD/TCON
#define LCD0_BASE 0x01C0C000
#define LCD0_GCTL             *(volatile uint32_t*)(LCD0_BASE + 0x000)
#define LCD0_GINT0            *(volatile uint32_t*)(LCD0_BASE + 0x004)
#define LCD0_GINT1            *(volatile uint32_t*)(LCD0_BASE + 0x008)
#define LCD0_TCON1_CTL        *(volatile uint32_t*)(LCD0_BASE + 0x090)
#define LCD0_TCON1_BASIC0     *(volatile uint32_t*)(LCD0_BASE + 0x094)
#define LCD0_TCON1_BASIC1     *(volatile uint32_t*)(LCD0_BASE + 0x098)
#define LCD0_TCON1_BASIC2     *(volatile uint32_t*)(LCD0_BASE + 0x09C)
#define LCD0_TCON1_BASIC3     *(volatile uint32_t*)(LCD0_BASE + 0x0A0)
#define LCD0_TCON1_BASIC4     *(volatile uint32_t*)(LCD0_BASE + 0x0A4)
#define LCD0_TCON1_BASIC5     *(volatile uint32_t*)(LCD0_BASE + 0x0A8)

#define LCD1_BASE 0x01C0D000
#define LCD1_GCTL             *(volatile uint32_t*)(LCD1_BASE + 0x000)
#define LCD1_GINT0            *(volatile uint32_t*)(LCD1_BASE + 0x004)
#define LCD1_GINT1            *(volatile uint32_t*)(LCD1_BASE + 0x008)
#define LCD1_TCON1_CTL        *(volatile uint32_t*)(LCD1_BASE + 0x090)
#define LCD1_TCON1_BASIC0     *(volatile uint32_t*)(LCD1_BASE + 0x094)
#define LCD1_TCON1_BASIC1     *(volatile uint32_t*)(LCD1_BASE + 0x098)
#define LCD1_TCON1_BASIC2     *(volatile uint32_t*)(LCD1_BASE + 0x09C)
#define LCD1_TCON1_BASIC3     *(volatile uint32_t*)(LCD1_BASE + 0x0A0)
#define LCD1_TCON1_BASIC4     *(volatile uint32_t*)(LCD1_BASE + 0x0A4)
#define LCD1_TCON1_BASIC5     *(volatile uint32_t*)(LCD1_BASE + 0x0A8)

// DE2
#define DE_BASE 0x01000000
#define DE_SCLK_GATE                  *(volatile uint32_t*)(DE_BASE + 0x000)
#define DE_HCLK_GATE                  *(volatile uint32_t*)(DE_BASE + 0x004)
#define DE_AHB_RESET                  *(volatile uint32_t*)(DE_BASE + 0x008)
#define DE_SCLK_DIV                   *(volatile uint32_t*)(DE_BASE + 0x00C)
#define DE_DE2TCON_MUX                *(volatile uint32_t*)(DE_BASE + 0x010)

// Mixer 0
#define DE_MIXER0                     (DE_BASE + 0x100000)
#define DE_MIXER0_GLB                 (DE_MIXER0 + 0x0)
#define DE_MIXER0_GLB_CTL             *(volatile uint32_t*)(DE_MIXER0_GLB + 0x000)
#define DE_MIXER0_GLB_STS             *(volatile uint32_t*)(DE_MIXER0_GLB + 0x004)
#define DE_MIXER0_GLB_DBUFFER         *(volatile uint32_t*)(DE_MIXER0_GLB + 0x008)
#define DE_MIXER0_GLB_SIZE            *(volatile uint32_t*)(DE_MIXER0_GLB + 0x00C)

#define DE_MIXER0_BLD                 (DE_MIXER0 + 0x1000)
#define DE_MIXER0_BLD_FILL_COLOR_CTL  *(volatile uint32_t*)(DE_MIXER0_BLD + 0x000)
#define DE_MIXER0_BLD_FILL_COLOR(x)   *(volatile uint32_t*)(DE_MIXER0_BLD + 0x004 + x * 0x10)
#define DE_MIXER0_BLD_CH_ISIZE(x)     *(volatile uint32_t*)(DE_MIXER0_BLD + 0x008 + x * 0x10)
#define DE_MIXER0_BLD_CH_OFFSET(x)    *(volatile uint32_t*)(DE_MIXER0_BLD + 0x00C + x * 0x10)
#define DE_MIXER0_BLD_CH_RTCTL        *(volatile uint32_t*)(DE_MIXER0_BLD + 0x080)
#define DE_MIXER0_BLD_PREMUL_CTL      *(volatile uint32_t*)(DE_MIXER0_BLD + 0x084)
#define DE_MIXER0_BLD_BK_COLOR        *(volatile uint32_t*)(DE_MIXER0_BLD + 0x088)
#define DE_MIXER0_BLD_SIZE            *(volatile uint32_t*)(DE_MIXER0_BLD + 0x08C)
#define DE_MIXER0_BLD_CTL(x)          *(volatile uint32_t*)(DE_MIXER0_BLD + 0x090 + x * 0x4)
#define DE_MIXER0_BLD_KEY_CTL         *(volatile uint32_t*)(DE_MIXER0_BLD + 0x0B0)
#define DE_MIXER0_BLD_KEY_CON         *(volatile uint32_t*)(DE_MIXER0_BLD + 0x0B4)
#define DE_MIXER0_BLD_KEY_MAX(x)      *(volatile uint32_t*)(DE_MIXER0_BLD + 0x0C0 + x * 0x4)
#define DE_MIXER0_BLD_KEY_MIN(x)      *(volatile uint32_t*)(DE_MIXER0_BLD + 0x0E0 + x * 0x4)
#define DE_MIXER0_BLD_OUT_COLOR       *(volatile uint32_t*)(DE_MIXER0_BLD + 0x0FC)

#define DE_MIXER0_OVL_V               (DE_MIXER0 + 0x2000)
#define DE_MIXER0_OVL_V_ATTCTL(x)     *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0x00 + x * 0x30)
#define DE_MIXER0_OVL_V_MBSIZE(x)     *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0x04 + x * 0x30)
#define DE_MIXER0_OVL_V_COOR(x)       *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0x08 + x * 0x30)
#define DE_MIXER0_OVL_V_PITCH0(x)     *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0x0C + x * 0x30)
#define DE_MIXER0_OVL_V_PITCH1(x)     *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0x10 + x * 0x30)
#define DE_MIXER0_OVL_V_PITCH2(x)     *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0x14 + x * 0x30)
#define DE_MIXER0_OVL_V_TOP_LADD0(x)  *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0x18 + x * 0x30)
#define DE_MIXER0_OVL_V_TOP_LADD1(x)  *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0x1C + x * 0x30)
#define DE_MIXER0_OVL_V_TOP_LADD2(x)  *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0x20 + x * 0x30)
#define DE_MIXER0_OVL_V_BOT_LADD0(x)  *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0x24 + x * 0x30)
#define DE_MIXER0_OVL_V_BOT_LADD1(x)  *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0x28 + x * 0x30)
#define DE_MIXER0_OVL_V_BOT_LADD2(x)  *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0x2C + x * 0x30)
#define DE_MIXER0_OVL_V_FILL_COLOR(x) *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0xC0 + x * 0x4)
#define DE_MIXER0_OVL_V_TOP_HADD0     *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0xD0)
#define DE_MIXER0_OVL_V_TOP_HADD1     *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0xD4)
#define DE_MIXER0_OVL_V_TOP_HADD2     *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0xD8)
#define DE_MIXER0_OVL_V_BOT_HADD0     *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0xDC)
#define DE_MIXER0_OVL_V_BOT_HADD1     *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0xE0)
#define DE_MIXER0_OVL_V_BOT_HADD2     *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0xE4)
#define DE_MIXER0_OVL_V_SIZE          *(volatile uint32_t*)(DE_MIXER0_OVL_V + 0xE8)

#define DE_MIXER0_VS_BASE             (DE_MIXER0 + 0x20000)
#define DE_MIXER0_VS_CTRL             *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x00)
#define DE_MIXER0_VS_STATUS           *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x08)
#define DE_MIXER0_VS_FIELD_CTRL       *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x0C)
#define DE_MIXER0_VS_OUT_SIZE         *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x40)
#define DE_MIXER0_VS_Y_SIZE           *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x80)
#define DE_MIXER0_VS_Y_HSTEP          *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x88)
#define DE_MIXER0_VS_Y_VSTEP          *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x8C)
#define DE_MIXER0_VS_Y_HPHASE         *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x90)
#define DE_MIXER0_VS_Y_VPHASE0        *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x98)
#define DE_MIXER0_VS_Y_VPHASE1        *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x9C)
#define DE_MIXER0_VS_C_SIZE           *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0xC0)
#define DE_MIXER0_VS_C_HSTEP          *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0xC8)
#define DE_MIXER0_VS_C_VSTEP          *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0xCC)
#define DE_MIXER0_VS_C_HPHASE         *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0xD0)
#define DE_MIXER0_VS_C_VPHASE0        *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0xD8)
#define DE_MIXER0_VS_C_VPHASE1        *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0xDC)
#define DE_MIXER0_VS_Y_HCOEF0(x)      *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x200 + x * 4)
#define DE_MIXER0_VS_Y_HCOEF1(x)      *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x300 + x * 4)
#define DE_MIXER0_VS_Y_VCOEF(x)       *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x400 + x * 4)
#define DE_MIXER0_VS_C_HCOEF0(x)      *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x600 + x * 4)
#define DE_MIXER0_VS_C_HCOEF1(x)      *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x700 + x * 4)
#define DE_MIXER0_VS_C_VCOEF(x)       *(volatile uint32_t*)(DE_MIXER0_VS_BASE + 0x800 + x * 4)

///void display_init(void);
void buffer_swap(void);

volatile uint32_t active_buffer;

#endif
