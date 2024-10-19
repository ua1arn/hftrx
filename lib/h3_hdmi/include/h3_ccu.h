/**
 * @file h3_ccu.h
 *
 */
/* Copyright (C) 2018-2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef H3_CCU_H_
#define H3_CCU_H_

#include <stdint.h>

typedef enum H3_CCU_PLL {
	CCU_PLL_CPUX,
	CCU_PLL_AUDIO,
	CCU_PLL_VIDEO,
	CCU_PLL_VE,
	CCU_PLL_DDR,
	CCU_PLL_PERIPH0,
	CCU_PLL_GPU,
	CCU_PLL_PERIPH1,
	CCU_PLL_DE
} ccu_pll_t;

#define CCU_BUS_CLK_GATING0_DMA				(1U << 6)
#define CCU_BUS_CLK_GATING0_HSTMR			(1U << 19)
#define CCU_BUS_CLK_GATING0_SPI0			(1U << 20)
#define CCU_BUS_CLK_GATING0_SPI1			(1U << 21)
#define CCU_BUS_CLK_GATING0_USB_OTG			(1U << 23)
#define CCU_BUS_CLK_GATING0_USB_OTG_EHCI0	(1U << 24)
#define CCU_BUS_CLK_GATING0_USB_EHCI1		(1U << 25)
#define CCU_BUS_CLK_GATING0_USB_EHCI2		(1U << 26)
#define CCU_BUS_CLK_GATING0_USB_EHCI3		(1U << 27)
#define CCU_BUS_CLK_GATING0_USB_OTG_OHCI0	(1U << 28)
#define CCU_BUS_CLK_GATING0_USB_OHCI1		(1U << 29)
#define CCU_BUS_CLK_GATING0_USB_OHCI2		(1U << 30)
#define CCU_BUS_CLK_GATING0_USB_OHCI3		(1U << 31)

#define CCU_BUS_CLK_GATING1_TCON0			(1U << 3)
#define CCU_BUS_CLK_GATING1_TCON1			(1U << 4)
#define CCU_BUS_CLK_GATING1_HDMI			(1U << 11)
#define CCU_BUS_CLK_GATING1_DE				(1U << 12)
#define CCU_BUS_CLK_GATING1_SPINLOCK		(1U << 22)

#define CCU_BUS_CLK_GATING2_AC_DIG			(1U << 0)
#define CCU_BUS_CLK_GATING2_THS				(1U << 8)

#define CCU_BUS_CLK_GATING3_TWI0			(1U << 0)
#define CCU_BUS_CLK_GATING3_TWI1			(1U << 1)
#define CCU_BUS_CLK_GATING3_UART0			(1U << 16)
#define CCU_BUS_CLK_GATING3_UART1			(1U << 17)
#define CCU_BUS_CLK_GATING3_UART2			(1U << 18)
#define CCU_BUS_CLK_GATING3_UART3			(1U << 19)

#define CCU_BUS_SOFT_RESET0_DMA				(1U << 6)
#define CCU_BUS_SOFT_RESET0_HSTMR			(1U << 19)
#define CCU_BUS_SOFT_RESET0_SPI0			(1U << 20)
#define CCU_BUS_SOFT_RESET0_SPI1			(1U << 21)
#define CCU_BUS_SOFT_RESET0_USB_OTG			(1U << 23)
#define CCU_BUS_SOFT_RESET0_USB_OTG_EHCI0	(1U << 24)
#define CCU_BUS_SOFT_RESET0_USB_EHCI1		(1U << 25)
#define CCU_BUS_SOFT_RESET0_USB_EHCI2		(1U << 26)
#define CCU_BUS_SOFT_RESET0_USB_EHCI3		(1U << 27)
#define CCU_BUS_SOFT_RESET0_USB_OTG_OHCI0	(1U << 28)
#define CCU_BUS_SOFT_RESET0_USB_OHCI1		(1U << 29)
#define CCU_BUS_SOFT_RESET0_USB_OHCI2		(1U << 30)
#define CCU_BUS_SOFT_RESET0_USB_OHCI3		(1U << 31)

#define CCU_BUS_SOFT_RESET1_TCON0			(1U << 3)
#define CCU_BUS_SOFT_RESET1_TCON1			(1U << 4)
#define CCU_BUS_SOFT_RESET1_HDMI2			(1U << 10)
#define CCU_BUS_SOFT_RESET1_HDMI			(1U << 11)
#define CCU_BUS_SOFT_RESET1_DE				(1U << 12)
#define CCU_BUS_SOFT_RESET1_SPINLOCK		(1U << 22)

#define CCU_BUS_SOFT_RESET3_AC				(1U << 0)
#define CCU_BUS_SOFT_RESET3_THS				(1U << 8)

#define CCU_BUS_SOFT_RESET4_TWI0			(1U << 0)
#define CCU_BUS_SOFT_RESET4_TWI1			(1U << 1)
#define CCU_BUS_SOFT_RESET4_UART0			(1U << 16)
#define CCU_BUS_SOFT_RESET4_UART1			(1U << 17)
#define CCU_BUS_SOFT_RESET4_UART2			(1U << 18)
#define CCU_BUS_SOFT_RESET4_UART3			(1U << 19)

#define CCU_PERIPH0_CLOCK_HZ		600000000		///< 600MHz

#define CCU_PLL_CPUX_MIN_CLOCK_HZ	200000000UL		///< 200MHz
#define CCU_PLL_CPUX_MAX_CLOCK_HZ 	2600000000UL	///< 2.6GHz

///< 4.3.5.19 THS Clock Register (THS_CLK_REG)
#define CCU_THS_CLK_SCLK_GATING		(1U << 31)

#define CCU_THS_CLK_SRC_OSC24M		0
	#define CCU_THS_CLK_SRC_SHIFT		(24)
	#define CCU_THS_CLK_SRC_MASK		(0xFU << 24)

#define CCU_THS_CLK_DIV_RATIO_1		(0)	///< /1
#define CCU_THS_CLK_DIV_RATIO_2		(1)	///< /2
#define CCU_THS_CLK_DIV_RATIO_4		(2)	///< /4
#define CCU_THS_CLK_DIV_RATIO_6		(3)	///< /6
	#define CCU_THS_CLK_DIV_RATIO_SHIFT	(0)
	#define CCU_THS_CLK_DIV_RATIO_MASK	(0xFU << 0)

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t h3_ccu_get_pll_rate(ccu_pll_t);

#ifdef __cplusplus
}
#endif

#endif /* H3_CCU_H_ */
