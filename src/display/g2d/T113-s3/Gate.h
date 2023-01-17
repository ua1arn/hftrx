#ifndef _CLOCK_H_
#define _CLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Type.h"

#define CCU_BASE 0x02001000

#define SMHC0_CLK_REG (*(IO u32*)(CCU_BASE+0x0830)) /* SMHC0 */
#define SMHC_BGR_REG (*(IO u32*)(CCU_BASE+0x084C))

#define USB0_CLK_REG (*(IO u32*)(CCU_BASE+0x0A70)) /* USB OTG0 */
#define USB_BGR_REG (*(IO u32*)(CCU_BASE+0x0A8C))

#define AVS_CLK_REG (*(IO u32*)(CCU_BASE+0x740)) /* AVS Clock Register */

#define TVD_CLK_REG (*(IO u32*)(CCU_BASE+0x0BC0))
#define TVD_BGR_REG (*(IO u32*)(CCU_BASE+0x0BDC))

#define MBUS_MAT_CLK_GATING_REG (*(IO u32*)(CCU_BASE+0x0804))

#define APB1_CLK_REG (*(IO u32*)(CCU_BASE+0x0524)) //APB1 Clock Register

#define UART_BGR_REG (*(IO u32*)(CCU_BASE+0x090C)) //UART Bus Gating Reset Register

#define DE_CLK_REG (*(IO u32*)(CCU_BASE+0x0600))
#define DE_BGR_REG (*(IO u32*)(CCU_BASE+0x060C))

#define G2D_CLK_REG (*(IO u32*)(CCU_BASE+0x0630))
#define G2D_BGR_REG (*(IO u32*)(CCU_BASE+0x063C))

#define TCONLCD_CLK_REG (*(IO u32*)(CCU_BASE+0x0B60))
#define TCONLCD_BGR_REG (*(IO u32*)(CCU_BASE+0x0B7C))

#define PLL_VIDEO0_CTRL_REG (*(IO u32*)(CCU_BASE+0x0040))

#define MBUS_MAT_CLK_GATING_REG (*(IO u32*)(CCU_BASE+0x0804))

#define LCD_GINT0_REG (*(IO u32*)(T113_TCONLCD_BASE+0x4))

//старьё от V3s (!!! не использовать)

#define PLL2_CFG_REG             (*(IO u32*)(CCU_BASE+0x008)) /* PLL2-Audio         */
#define AUDIO_CODEC_SCLK_CFG_REG (*(IO u32*)(CCU_BASE+0x140)) /* Audio Codec Clock  */

#define BUS_CLK_GATING_REG0 (*(IO u32*)(CCU_BASE+0x0060)) /* Bus Clock Gating Register0     */
#define BUS_CLK_GATING_REG2 (*(IO u32*)(CCU_BASE+0x0068))
#define BUS_CLK_GATING_REG3 (*(IO u32*)(CCU_BASE+0x006C))

#define BUS_SOFT_RST_REG0   (*(IO u32*)(CCU_BASE+0x02C0))
#define BUS_SOFT_RST_REG3   (*(IO u32*)(CCU_BASE+0x02D0))
#define BUS_SOFT_RST_REG4   (*(IO u32*)(CCU_BASE+0x02D8))

#ifdef __cplusplus
}
#endif

#endif
