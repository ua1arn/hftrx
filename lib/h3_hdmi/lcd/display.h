#ifndef _DISPLAY_H
#define _DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct virt_mode_t {
  int fb_width, fb_height, fb_bytes;
  int x, y, ovx, ovy;
};

#include "display_timing.h"
extern struct display_timing default_timing;

extern int display_is_digital;
extern int display_is_pal;

// HDMI controller output resolution
// NB: Any change in resolution requires additional changes in the HDMI
// controller register settings below.
#define DISPLAY_PHYS_RES_X	(display_is_digital ? default_timing.hactive.typ : 720)
#define DISPLAY_PHYS_RES_Y	(display_is_digital ? default_timing.vactive.typ : (display_is_pal ? 576 : 480))

#define VIDEO_RAM_BYTES 0x180000

// The HDMI registers base address.
#define HDMI_BASE     0x01EE0000
#define HDMI_PHY_BASE (HDMI_BASE + 0x10000)

#define HDMI_REG8(off)  *(volatile uint8_t *)(HDMI_BASE + (off))
#define HDMI_REG32(off) *(volatile uint32_t *)(HDMI_BASE + (off))
#define CEC          *(volatile uint32_t*)(HDMI_BASE + 0x10FF8)
#define VERSION      *(volatile uint32_t*)(HDMI_BASE + 0x10FFC)
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
#define HDMI_MC_SWRSTZREQ     HDMI_REG8(0x4002)

#define HDMI_VP_STUFF         *(volatile uint8_t*)(HDMI_BASE + 0x0802)
#define HDMI_VP_CONF          *(volatile uint8_t*)(HDMI_BASE + 0x0804)

#define HDMI_TX_INVID0        *(volatile uint8_t*)(HDMI_BASE + 0x0200)
#define HDMI_TX_INSTUFFING    *(volatile uint8_t*)(HDMI_BASE + 0x0201)

// Audio register names from
// http://rockchip.fr/Rockchip%20RK3399%20TRM%20V1.3%20Part2.pdf
// (Different revisions of the this HDMI IP are used in various SoCs;
// some vendors provide documentation, and some don't.)

#define HDMI_FC_AUDIOCONF0	HDMI_REG8(0x1025)
#define HDMI_FC_AUDIOCONF1	HDMI_REG8(0x1026)
#define HDMI_FC_AUDIOCONF2	HDMI_REG8(0x1027)
#define HDMI_FC_AUDIOCONF3	HDMI_REG8(0x1028)

#define HDMI_FC_AUDSCONF	HDMI_REG8(0x1063)
#define HDMI_FC_AUDSV		HDMI_REG8(0x1065)
#define HDMI_FC_AUDSU		HDMI_REG8(0x1066)
#define HDMI_FC_AUDSCHNL0	HDMI_REG8(0x1067)
#define HDMI_FC_AUDSCHNL1	HDMI_REG8(0x1068)
#define HDMI_FC_AUDSCHNL2	HDMI_REG8(0x1069)
#define HDMI_FC_AUDSCHNL3	HDMI_REG8(0x106a)
#define HDMI_FC_AUDSCHNL4	HDMI_REG8(0x106b)
#define HDMI_FC_AUDSCHNL5	HDMI_REG8(0x106c)
#define HDMI_FC_AUDSCHNL6	HDMI_REG8(0x106d)
#define HDMI_FC_AUDSCHNL7	HDMI_REG8(0x106e)
#define HDMI_FC_AUDSCHNL8	HDMI_REG8(0x106f)

#define HDMI_AUD_CONF0		HDMI_REG8(0x3100)
#define HDMI_AUD_CONF1		HDMI_REG8(0x3101)
#define HDMI_AUD_INT		HDMI_REG8(0x3102)
#define HDMI_AUD_CONF2		HDMI_REG8(0x3103)

#define HDMI_AUD_N1		HDMI_REG8(0x3200)
#define HDMI_AUD_N2		HDMI_REG8(0x3201)
#define HDMI_AUD_N3		HDMI_REG8(0x3102)
#define HDMI_AUD_CTS1		HDMI_REG8(0x3203)
#define HDMI_AUD_CTS2		HDMI_REG8(0x3204)
#define HDMI_AUD_CTS3		HDMI_REG8(0x3205)
#define HDMI_AUD_INPUTCLKFS	HDMI_REG8(0x3206)


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

#define LCD1_TCON1_PS_SYNC    *(volatile uint32_t*)(LCD1_BASE + 0x0B0)

#define LCD1_TCON1_IO_POL     *(volatile uint32_t*)(LCD1_BASE + 0x0F0)
#define LCD1_TCON1_IO_TRI     *(volatile uint32_t*)(LCD1_BASE + 0x0F4)

#define LCD1_TCON_CEU_CTL          *(volatile uint32_t*)(LCD1_BASE + 0x100)
#define LCD1_TCON_CEU_COEF_MUL(n)  *(volatile uint32_t*)(LCD1_BASE + 0x110 + (n) * 4)
#define LCD1_TCON_CEU_COEF_RANG(n) *(volatile uint32_t*)(LCD1_BASE + 0x140 + (n) * 4)

#define LCD1_TCON1_GAMMA_TABLE(n)  *(volatile uint32_t*)(LCD1_BASE + 0x400 + (n) * 4)

// DE2
#define DE_BASE 0x01000000
#define DE_SCLK_GATE                  *(volatile uint32_t*)(DE_BASE + 0x000)
#define DE_HCLK_GATE                  *(volatile uint32_t*)(DE_BASE + 0x004)
#define DE_AHB_RESET                  *(volatile uint32_t*)(DE_BASE + 0x008)
#define DE_SCLK_DIV                   *(volatile uint32_t*)(DE_BASE + 0x00C)
#define DE_DE2TCON_MUX                *(volatile uint32_t*)(DE_BASE + 0x010)
#define DE_CMD_CTL                    *(volatile uint32_t*)(DE_BASE + 0x014)

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

// Mixer 1
#define DE_MIXER1                     (DE_BASE + 0x200000)
#define DE_MIXER1_GLB                 (DE_MIXER1 + 0x0)
#define DE_MIXER1_GLB_CTL             *(volatile uint32_t*)(DE_MIXER1_GLB + 0x000)
#define DE_MIXER1_GLB_STS             *(volatile uint32_t*)(DE_MIXER1_GLB + 0x004)
#define DE_MIXER1_GLB_DBUFFER         *(volatile uint32_t*)(DE_MIXER1_GLB + 0x008)
#define DE_MIXER1_GLB_SIZE            *(volatile uint32_t*)(DE_MIXER1_GLB + 0x00C)

#define DE_MIXER1_BLD                 (DE_MIXER1 + 0x1000)
#define DE_MIXER1_BLD_FILL_COLOR_CTL  *(volatile uint32_t*)(DE_MIXER1_BLD + 0x000)
#define DE_MIXER1_BLD_FILL_COLOR(x)   *(volatile uint32_t*)(DE_MIXER1_BLD + 0x004 + x * 0x10)
#define DE_MIXER1_BLD_CH_ISIZE(x)     *(volatile uint32_t*)(DE_MIXER1_BLD + 0x008 + x * 0x10)
#define DE_MIXER1_BLD_CH_OFFSET(x)    *(volatile uint32_t*)(DE_MIXER1_BLD + 0x00C + x * 0x10)
#define DE_MIXER1_BLD_CH_RTCTL        *(volatile uint32_t*)(DE_MIXER1_BLD + 0x080)
#define DE_MIXER1_BLD_PREMUL_CTL      *(volatile uint32_t*)(DE_MIXER1_BLD + 0x084)
#define DE_MIXER1_BLD_BK_COLOR        *(volatile uint32_t*)(DE_MIXER1_BLD + 0x088)
#define DE_MIXER1_BLD_SIZE            *(volatile uint32_t*)(DE_MIXER1_BLD + 0x08C)
#define DE_MIXER1_BLD_CTL(x)          *(volatile uint32_t*)(DE_MIXER1_BLD + 0x090 + x * 0x4)
#define DE_MIXER1_BLD_KEY_CTL         *(volatile uint32_t*)(DE_MIXER1_BLD + 0x0B0)
#define DE_MIXER1_BLD_KEY_CON         *(volatile uint32_t*)(DE_MIXER1_BLD + 0x0B4)
#define DE_MIXER1_BLD_KEY_MAX(x)      *(volatile uint32_t*)(DE_MIXER1_BLD + 0x0C0 + x * 0x4)
#define DE_MIXER1_BLD_KEY_MIN(x)      *(volatile uint32_t*)(DE_MIXER1_BLD + 0x0E0 + x * 0x4)
#define DE_MIXER1_BLD_OUT_COLOR       *(volatile uint32_t*)(DE_MIXER1_BLD + 0x0FC)

#define DE_MIXER1_OVL_V               (DE_MIXER1 + 0x2000)
#define DE_MIXER1_OVL_V_ATTCTL(x)     *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0x00 + x * 0x30)
#define DE_MIXER1_OVL_V_MBSIZE(x)     *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0x04 + x * 0x30)
#define DE_MIXER1_OVL_V_COOR(x)       *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0x08 + x * 0x30)
#define DE_MIXER1_OVL_V_PITCH0(x)     *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0x0C + x * 0x30)
#define DE_MIXER1_OVL_V_PITCH1(x)     *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0x10 + x * 0x30)
#define DE_MIXER1_OVL_V_PITCH2(x)     *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0x14 + x * 0x30)
#define DE_MIXER1_OVL_V_TOP_LADD0(x)  *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0x18 + x * 0x30)
#define DE_MIXER1_OVL_V_TOP_LADD1(x)  *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0x1C + x * 0x30)
#define DE_MIXER1_OVL_V_TOP_LADD2(x)  *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0x20 + x * 0x30)
#define DE_MIXER1_OVL_V_BOT_LADD0(x)  *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0x24 + x * 0x30)
#define DE_MIXER1_OVL_V_BOT_LADD1(x)  *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0x28 + x * 0x30)
#define DE_MIXER1_OVL_V_BOT_LADD2(x)  *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0x2C + x * 0x30)
#define DE_MIXER1_OVL_V_FILL_COLOR(x) *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0xC0 + x * 0x4)
#define DE_MIXER1_OVL_V_TOP_HADD0     *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0xD0)
#define DE_MIXER1_OVL_V_TOP_HADD1     *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0xD4)
#define DE_MIXER1_OVL_V_TOP_HADD2     *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0xD8)
#define DE_MIXER1_OVL_V_BOT_HADD0     *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0xDC)
#define DE_MIXER1_OVL_V_BOT_HADD1     *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0xE0)
#define DE_MIXER1_OVL_V_BOT_HADD2     *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0xE4)
#define DE_MIXER1_OVL_V_SIZE          *(volatile uint32_t*)(DE_MIXER1_OVL_V + 0xE8)

#define DE_MIXER1_OVL_UI               (DE_MIXER1 + 0x3000)
#define DE_MIXER1_OVL_UI_ATTR_CTL(x)   *(volatile uint32_t *)(DE_MIXER1_OVL_UI + 0x00 + x * 0x20)
#define DE_MIXER1_OVL_UI_MBSIZE(x)     *(volatile uint32_t *)(DE_MIXER1_OVL_UI + 0x04 + x * 0x20)
#define DE_MIXER1_OVL_UI_COOR(x)       *(volatile uint32_t *)(DE_MIXER1_OVL_UI + 0x08 + x * 0x20)
#define DE_MIXER1_OVL_UI_PITCH(x)      *(volatile uint32_t *)(DE_MIXER1_OVL_UI + 0x0C + x * 0x20)
#define DE_MIXER1_OVL_UI_TOP_LADD(x)   *(volatile uint32_t *)(DE_MIXER1_OVL_UI + 0x10 + x * 0x20)
#define DE_MIXER1_OVL_UI_BOT_LADD(x)   *(volatile uint32_t *)(DE_MIXER1_OVL_UI + 0x14 + x * 0x20)
#define DE_MIXER1_OVL_UI_FILL_COLOR(x) *(volatile uint32_t *)(DE_MIXER1_OVL_UI + 0x18 + x * 0x4)
#define DE_MIXER1_OVL_UI_TOP_HADD      *(volatile uint32_t *)(DE_MIXER1_OVL_UI + 0x80)
#define DE_MIXER1_OVL_UI_BOT_HADD      *(volatile uint32_t *)(DE_MIXER1_OVL_UI + 0x84)
#define DE_MIXER1_OVL_UI_SIZE          *(volatile uint32_t *)(DE_MIXER1_OVL_UI + 0x88)

#define DE_MIXER1_VS_BASE             (DE_MIXER1 + 0x20000)
#define DE_MIXER1_VS_CTRL             *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x00)
#define DE_MIXER1_VS_STATUS           *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x08)
#define DE_MIXER1_VS_FIELD_CTRL       *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x0C)
#define DE_MIXER1_VS_OUT_SIZE         *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x40)
#define DE_MIXER1_VS_Y_SIZE           *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x80)
#define DE_MIXER1_VS_Y_HSTEP          *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x88)
#define DE_MIXER1_VS_Y_VSTEP          *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x8C)
#define DE_MIXER1_VS_Y_HPHASE         *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x90)
#define DE_MIXER1_VS_Y_VPHASE0        *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x98)
#define DE_MIXER1_VS_Y_VPHASE1        *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x9C)
#define DE_MIXER1_VS_C_SIZE           *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0xC0)
#define DE_MIXER1_VS_C_HSTEP          *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0xC8)
#define DE_MIXER1_VS_C_VSTEP          *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0xCC)
#define DE_MIXER1_VS_C_HPHASE         *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0xD0)
#define DE_MIXER1_VS_C_VPHASE0        *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0xD8)
#define DE_MIXER1_VS_C_VPHASE1        *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0xDC)
#define DE_MIXER1_VS_Y_HCOEF0(x)      *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x200 + x * 4)
#define DE_MIXER1_VS_Y_HCOEF1(x)      *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x300 + x * 4)
#define DE_MIXER1_VS_Y_VCOEF(x)       *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x400 + x * 4)
#define DE_MIXER1_VS_C_HCOEF0(x)      *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x600 + x * 4)
#define DE_MIXER1_VS_C_HCOEF1(x)      *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x700 + x * 4)
#define DE_MIXER1_VS_C_VCOEF(x)       *(volatile uint32_t*)(DE_MIXER1_VS_BASE + 0x800 + x * 4)

#define DE_MIXER1_UIS_BASE(n)         (DE_MIXER1 + 0x40000 + 0x10000 * (n))
#define DE_MIXER1_UIS_CTRL(n)         *(volatile uint32_t*)(DE_MIXER1_UIS_BASE(n) + 0x00)
#define DE_MIXER1_UIS_STATUS(n)       *(volatile uint32_t*)(DE_MIXER1_UIS_BASE(n) + 0x08)
#define DE_MIXER1_UIS_FIELD_CTRL(n)   *(volatile uint32_t*)(DE_MIXER1_UIS_BASE(n) + 0x0C)
#define DE_MIXER1_UIS_OUT_SIZE(n)     *(volatile uint32_t*)(DE_MIXER1_UIS_BASE(n) + 0x40)
#define DE_MIXER1_UIS_IN_SIZE(n)      *(volatile uint32_t*)(DE_MIXER1_UIS_BASE(n) + 0x80)
#define DE_MIXER1_UIS_HSTEP(n)        *(volatile uint32_t*)(DE_MIXER1_UIS_BASE(n) + 0x88)
#define DE_MIXER1_UIS_VSTEP(n)        *(volatile uint32_t*)(DE_MIXER1_UIS_BASE(n) + 0x8C)
#define DE_MIXER1_UIS_HPHASE(n)       *(volatile uint32_t*)(DE_MIXER1_UIS_BASE(n) + 0x90)
#define DE_MIXER1_UIS_VPHASE0(n)      *(volatile uint32_t*)(DE_MIXER1_UIS_BASE(n) + 0x98)
#define DE_MIXER1_UIS_VPHASE1(n)      *(volatile uint32_t*)(DE_MIXER1_UIS_BASE(n) + 0x9C)
#define DE_MIXER1_UIS_HCOEF(n, x)     *(volatile uint32_t*)(DE_MIXER1_UIS_BASE(n) + 0x200 + x * 4)

#define DE_SIZE(x, y) ((((y)-1) << 16) | ((x)-1))
#define DE_SIZE_PHYS  DE_SIZE(DISPLAY_PHYS_RES_X, DISPLAY_PHYS_RES_Y)

void display_set_mode(int x, int y, int ovx, int ovy);
void display_swap_buffers(void);
void display_clear_active_buffer(void);
void display_enable_filter(int onoff);

extern int display_single_buffer;

extern volatile uint32_t *display_active_buffer;
extern volatile uint32_t *display_visible_buffer;

void display_scaler_set_coeff(uint32_t hstep, int sub);
void display_scaler_nearest_neighbour(void);

struct display_phys_mode_t {
  int pixclk;

  int hactive;
  int hfront_porch;
  int hsync_width;
  int hback_porch;
  int hsync_pol;

  int vactive;
  int vfront_porch;
  int vsync_width;
  int vback_porch;
  int vsync_pol;

  int hdmi;
};

void hook_display_vblank(void);

int display_init(const struct display_phys_mode_t *mode);
#ifdef __cplusplus
}
#endif

#endif
