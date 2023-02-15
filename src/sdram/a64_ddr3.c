/*
 * a64_ddr3.c
 *
 *  Created on: 15 февр. 2023 г.
 *      Author: User
 */

#include "hardware.h"

#if CPUSTYLE_A64

#include "formats.h"

//#define PHYS_SDRAM_0 0x40000000
#define PHYS_SDRAM_0_SIZE (2u * 1024 * 1024 * 1024)

//#define SUNXI_SRAMC_BASE		0x01c00000
#define SUNXI_DRAMC_BASE	DRAMC_BASE//	0x01c01000
//#define SUNXI_DMA_BASE			0x01c02000
//#define SUNXI_NFC_BASE			0x01c03000

typedef uintptr_t virtual_addr_t;

// SPDX-License-Identifier: GPL-2.0+

static void udelay(unsigned us)
{
	local_delay_us(us * 1);
}

static uint32_t read32(uintptr_t addr)
{
	return * (volatile uint32_t *) addr;
}

static void write32(uintptr_t addr, uint32_t value)
{
	* (volatile uint32_t *) addr = value;
}

static void write32ptr(void * addr, uint32_t value)
{
	write32((uintptr_t) addr, value);
}

static uint32_t read32ptr(void * addr)
{
	return read32((uintptr_t) addr);
}

#define clrbits_le32(addr, clear) \
		write32ptr((addr), read32ptr(addr) & ~(clear))

#define setbits_le32(addr, set) \
		write32ptr((addr), read32ptr(addr) | (set))

#define clrsetbits_le32(addr, clear, set) \
		write32ptr((addr), (read32ptr(addr) & ~(clear)) | (set))

#define writel(data, addr) do { write32ptr((addr), (data)); } while (0)
#define readl(addr) (read32ptr(addr))
#define BIT_U32(pos) ((uint32_t) 1 << (pos))

// https://github.com/BPI-SINOVOIP/BPI-M64-bsp-4.4/blob/66bef0f2f30b367eb93b1cbad21ce85e0361f7ae/u-boot-sunxi/arch/arm/cpu/armv7/sunxi/dram.c

/*
 * (C) Copyright 2007-2015
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

#ifndef __PLATFORM_H
#define __PLATFORM_H
//
//#define SUNXI_SRAM_D_BASE                (0x00010000L)
//#define SUNXI_SRAM_A2_BASE               (0x00040000L)
///* base address of modules */
//#define SUNXI_DE_BASE                    (0x01000000L)
//#define SUNXI_CORESIGHT_DEBUG_BASE       (0x01400000L)
//#define SUNXI_CPU_MBIST_BASE             (0x01502000L)
//#define SUNXI_CPUX_CFG_BASE              (0x01700000L)
//
//
//#define SUNXI_SYSCRL_BASE			 (0x01c00000L)
//
//#define SUNXI_DMA_BASE           		 (0x01c02000L)
//#define SUNXI_NFC_BASE			         (0x01c03000L)
//#define SUNXI_TSC_BASE			         (0x01c06000L)
//
//#define SUNXI_TCON0_BASE			     (0x01c0c000L)
//#define SUNXI_TCON1_BASE			     (0x01c0d000L)
//#define SUNXI_VE_BASE				     (0x01c0e000L)
//
//#define SUNXI_SMHC0_BASE			     (0x01c0f000L)
//#define SUNXI_SMHC1_BASE			     (0x01c10000L)
//#define SUNXI_SMHC2_BASE			     (0x01c11000L)
//
//#define SUNXI_SID_BASE			         (0x01c14000L)
//#define SUNXI_SID_SRAM_BASE		         (0x01c14200L)
//#define SUNXI_SS_BASE				 (0x01c15000L)
//
//#define SUNXI_MSGBOX_BASE			 (0x01c17000L)
//#define SUNXI_SPINLOCK_BASE		         (0x01c18000L)
//
//#define SUNXI_USBOTG_BASE			     (0x01c19000L)
//#define SUNXI_EHCI0_BASE			     (0x01c1a000L)
//#define SUNXI_EHCI1_BASE			     (0x01c1b000L)
//
//#define SUNXI_SMC_BASE			         (0x01c1e000L)
//
//#define SUNXI_CCM_BASE			         (0x01c20000L)
//#define SUNXI_PIO_BASE			         (0x01c20800L)
//#define SUNXI_TIMER_BASE			     (0x01c20c00L)
//#define SUNXI_SPDIF_BASE			     (0x01c21000L)
//#define SUNXI_PWM03_BASE			     (0x01c21400L)
//
//#define SUNXI_KEYADC_BASE			     (0x01c21800L)
//#define SUNXI_DAUDIO0_BASE			     (0x01c22000L)
//#define SUNXI_DAUDIO1_BASE			     (0x01c24000L)
//#define SUNXI_DAUDIO2_BASE			     (0x01c28000L)
//
//#define SUNXI_AC_BASE			         (0x01c22c00L)
//#define SUNXI_SPC_BASE			         (0x01c23400L)
//#define SUNXI_THC_BASE			         (0x01c25000L)
//
//#define SUNXI_UART0_BASE			     (0x01c28000L)
//#define SUNXI_UART1_BASE			     (0x01c28400L)
//#define SUNXI_UART2_BASE			     (0x01c28800L)
//#define SUNXI_UART3_BASE			     (0x01c28c00L)
//#define SUNXI_UART4_BASE			     (0x01c29000L)
//
//#define SUNXI_TWI0_BASE			         (0x01c2ac00L)
//#define SUNXI_TWI1_BASE			         (0x01c2b000L)
//#define SUNXI_TWI2_BASE			         (0x01c2b400L)
//#define SUNXI_SCR_BASE			         (0x01c2c400L)
//
//#define SUNXI_EMAC_BASE			         (0x01c30000L)
//#define SUNXI_GPU_BASE			         (0x01c40000L)
//#define SUNXI_HSTMR_BASE			 (0x01c60000L)
//
//#define SUNXI_DRAMCOM_BASE		         (0x01c62000L)
//#define SUNXI_DRAMCTL0_BASE		         (0x01c63000L)
//#define SUNXI_DRAMPHY0_BASE		         (0x01c65000L)
//
//#define SUNXI_SPI0_BASE			         (0x01c68000L)
//#define SUNXI_SPI1_BASE			         (0x01c69000L)
//
//#define ARMA9_SCU_BASE		             (0x01c80000L)
//#define ARMA9_GIC_BASE		             (0x01c81000L)
//#define ARMA9_CPUIF_BASE	             (0x01c82000L)
//
//#define SUNXI_MIPI_DSI0_BASE		     (0x01ca0000L)
//#define SUNXI_MIPI_DSIPHY_BASE		     (0x01ca1000L)
//
//#define SUNXI_CSI0_BASE			         (0x01cb0000L)
//#define SUNXI_DE_INTERLACED_BASE		 (0x01e00000L)
//#define SUNXI_HDMI_BASE			         (0x01ee0000L)
////#define HDMI_BASE                                SUNXI_HDMI_BASE
//
//#define SUNXI_RTC_BASE			         (0x01f00000L)
//#define SUNXI_RTMR01_BASE		         (0x01f00800L)
//#define SUNXI_RINTC_BASE			 (0x01f00C00L)
//#define SUNXI_RWDOG_BASE			 (0x01f01000L)
//#define SUNXI_RPRCM_BASE			 (0x01f01400L)
//#define SUNXI_RTWD_BASE			         (0x01f01800L)
//#define SUNXI_RCPUCFG_BASE		         (0x01f01C00L)
//#define SUNXI_RCIR_BASE			         (0x01f02000L)
//#define SUNXI_RTWI_BASE			         (0x01f02400L)
//#define SUNXI_RUART_BASE			 (0x01f02800L)
//#define SUNXI_RPIO_BASE			         (0x01f02c00L)
//#define SUNXI_RRSB_BASE			         (0x01f03400L)
//#define SUNXI_RPWM_BASE			         (0x01f03800L)
//
//
//#define SUNXI_CPUX_CFG_BASE_A32           (0x01700000)
//#define RVBARADDR0_L		             (SUNXI_CPUX_CFG_BASE_A32+0xA0)
//#define RVBARADDR0_H		             (SUNXI_CPUX_CFG_BASE_A32+0xA4)
//#define RVBARADDR1_L		             (SUNXI_CPUX_CFG_BASE_A32+0xA8)
//#define RVBARADDR1_H		             (SUNXI_CPUX_CFG_BASE_A32+0xAC)
//#define RVBARADDR2_L		             (SUNXI_CPUX_CFG_BASE_A32+0xB0)
//#define RVBARADDR2_H		             (SUNXI_CPUX_CFG_BASE_A32+0xB4)
//#define RVBARADDR3_L		             (SUNXI_CPUX_CFG_BASE_A32+0xB8)
//#define RVBARADDR3_H		             (SUNXI_CPUX_CFG_BASE_A32+0xBC)

/*dram_para_offset is the numbers of u32 before dram data sturcture(dram_para) in struct arisc_para*/
#define SCP_DRAM_PARA_OFFSET                 (sizeof(u32) * 13 + sizeof(u32) + sizeof(u32) * 2 * 64)
#define SCP_DARM_PARA_NUM	             (24)

#endif

/*
 * sun4i, sun5i and sun7i clock register definitions
 *
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _SUNXI_CLOCK_SUN4I_H
#define _SUNXI_CLOCK_SUN4I_H

struct sunxi_ccm_reg {
	uint32_t pll1_cfg;		/* 0x00 pll1 control */
	uint32_t pll1_tun;		/* 0x04 pll1 tuning */
	uint32_t pll2_cfg;		/* 0x08 pll2 control */
	uint32_t pll2_tun;		/* 0x0c pll2 tuning */
	uint32_t pll3_cfg;		/* 0x10 pll3 control */
	uint8_t res0[0x4];
	uint32_t pll4_cfg;		/* 0x18 pll4 control */
	uint8_t res1[0x4];
	uint32_t pll5_cfg;		/* 0x20 pll5 control */
	uint32_t pll5_tun;		/* 0x24 pll5 tuning */
	uint32_t pll6_cfg;		/* 0x28 pll6 control */
	uint32_t pll6_tun;		/* 0x2c pll6 tuning */
	uint32_t pll7_cfg;		/* 0x30 pll7 control */
	uint32_t pll1_tun2;		/* 0x34 pll5 tuning2 */
	uint8_t res2[0x4];
	uint32_t pll5_tun2;		/* 0x3c pll5 tuning2 */
	uint8_t res3[0xc];
	uint32_t pll_lock_dbg;	/* 0x4c pll lock time debug */
	uint32_t osc24m_cfg;		/* 0x50 osc24m control */
	uint32_t cpu_ahb_apb0_cfg;	/* 0x54 cpu,ahb and apb0 divide ratio */
	uint32_t apb1_clk_div_cfg;	/* 0x58 apb1 clock dividor */
	uint32_t axi_gate;		/* 0x5c axi module clock gating */
	uint32_t ahb_gate0;		/* 0x60 ahb module clock gating 0 */
	uint32_t ahb_gate1;		/* 0x64 ahb module clock gating 1 */
	uint32_t apb0_gate;		/* 0x68 apb0 module clock gating */
	uint32_t apb1_gate;		/* 0x6c apb1 module clock gating */
	uint8_t res4[0x10];
	uint32_t nand_sclk_cfg;	/* 0x80 nand sub clock control */
	uint32_t ms_sclk_cfg;	/* 0x84 memory stick sub clock control */
	uint32_t sd0_clk_cfg;	/* 0x88 sd0 clock control */
	uint32_t sd1_clk_cfg;	/* 0x8c sd1 clock control */
	uint32_t sd2_clk_cfg;	/* 0x90 sd2 clock control */
	uint32_t sd3_clk_cfg;	/* 0x94 sd3 clock control */
	uint32_t ts_clk_cfg;		/* 0x98 transport stream clock control */
	uint32_t ss_clk_cfg;		/* 0x9c */
	uint32_t spi0_clk_cfg;	/* 0xa0 */
	uint32_t spi1_clk_cfg;	/* 0xa4 */
	uint32_t spi2_clk_cfg;	/* 0xa8 */
	uint32_t pata_clk_cfg;	/* 0xac */
	uint32_t ir0_clk_cfg;	/* 0xb0 */
	uint32_t ir1_clk_cfg;	/* 0xb4 */
	uint32_t iis_clk_cfg;	/* 0xb8 */
	uint32_t ac97_clk_cfg;	/* 0xbc */
	uint32_t spdif_clk_cfg;	/* 0xc0 */
	uint32_t keypad_clk_cfg;	/* 0xc4 */
	uint32_t sata_clk_cfg;	/* 0xc8 */
	uint32_t usb_clk_cfg;	/* 0xcc */
	uint32_t gps_clk_cfg;	/* 0xd0 */
	uint32_t spi3_clk_cfg;	/* 0xd4 */
	uint8_t res5[0x28];
	uint32_t dram_clk_cfg;	/* 0x100 */
	uint32_t be0_clk_cfg;	/* 0x104 */
	uint32_t be1_clk_cfg;	/* 0x108 */
	uint32_t fe0_clk_cfg;	/* 0x10c */
	uint32_t fe1_clk_cfg;	/* 0x110 */
	uint32_t mp_clk_cfg;		/* 0x114 */
	uint32_t lcd0_ch0_clk_cfg;	/* 0x118 */
	uint32_t lcd1_ch0_clk_cfg;	/* 0x11c */
	uint32_t csi_isp_clk_cfg;	/* 0x120 */
	uint8_t res6[0x4];
	uint32_t tvd_clk_reg;	/* 0x128 */
	uint32_t lcd0_ch1_clk_cfg;	/* 0x12c */
	uint32_t lcd1_ch1_clk_cfg;	/* 0x130 */
	uint32_t csi0_clk_cfg;	/* 0x134 */
	uint32_t csi1_clk_cfg;	/* 0x138 */
	uint32_t ve_clk_cfg;		/* 0x13c */
	uint32_t audio_codec_clk_cfg;	/* 0x140 */
	uint32_t avs_clk_cfg;	/* 0x144 */
	uint32_t ace_clk_cfg;	/* 0x148 */
	uint32_t lvds_clk_cfg;	/* 0x14c */
	uint32_t hdmi_clk_cfg;	/* 0x150 */
	uint32_t mali_clk_cfg;	/* 0x154 */
	uint8_t res7[0x4];
	uint32_t mbus_clk_cfg;	/* 0x15c */
	uint8_t res8[0x4];
	uint32_t gmac_clk_cfg;	/* 0x164 */
};

/* apb1 bit field */
#define APB1_CLK_SRC_OSC24M		(0x0 << 24)
#define APB1_CLK_SRC_PLL6		(0x1 << 24)
#define APB1_CLK_SRC_LOSC		(0x2 << 24)
#define APB1_CLK_SRC_MASK		(0x3 << 24)
#define APB1_CLK_RATE_N_1		(0x0 << 16)
#define APB1_CLK_RATE_N_2		(0x1 << 16)
#define APB1_CLK_RATE_N_4		(0x2 << 16)
#define APB1_CLK_RATE_N_8		(0x3 << 16)
#define APB1_CLK_RATE_N_MASK		(3 << 16)
#define APB1_CLK_RATE_M(m)		(((m)-1) << 0)
#define APB1_CLK_RATE_M_MASK            (0x1f << 0)

/* apb1 gate field */
#define APB1_GATE_UART_SHIFT	(16)
#define APB1_GATE_UART_MASK		(0xff << APB1_GATE_UART_SHIFT)
#define APB1_GATE_TWI_SHIFT	(0)
#define APB1_GATE_TWI_MASK		(0xf << APB1_GATE_TWI_SHIFT)

/* clock divide */
#define AXI_DIV_SHIFT		(0)
#define AXI_DIV_1			0
#define AXI_DIV_2			1
#define AXI_DIV_3			2
#define AXI_DIV_4			3
#define AHB_DIV_SHIFT		(4)
#define AHB_DIV_1			0
#define AHB_DIV_2			1
#define AHB_DIV_4			2
#define AHB_DIV_8			3
#define APB0_DIV_SHIFT		(8)
#define APB0_DIV_1			0
#define APB0_DIV_2			1
#define APB0_DIV_4			2
#define APB0_DIV_8			3
#define CPU_CLK_SRC_SHIFT	(16)
#define CPU_CLK_SRC_OSC24M		1
#define CPU_CLK_SRC_PLL1		2

#define CCM_PLL1_CFG_ENABLE_SHIFT		31
#define CCM_PLL1_CFG_VCO_RST_SHIFT		30
#define CCM_PLL1_CFG_VCO_BIAS_SHIFT		26
#define CCM_PLL1_CFG_PLL4_EXCH_SHIFT		25
#define CCM_PLL1_CFG_BIAS_CUR_SHIFT		20
#define CCM_PLL1_CFG_DIVP_SHIFT			16
#define CCM_PLL1_CFG_LCK_TMR_SHIFT		13
#define CCM_PLL1_CFG_FACTOR_N_SHIFT		8
#define CCM_PLL1_CFG_FACTOR_K_SHIFT		4
#define CCM_PLL1_CFG_SIG_DELT_PAT_IN_SHIFT	3
#define CCM_PLL1_CFG_SIG_DELT_PAT_EN_SHIFT	2
#define CCM_PLL1_CFG_FACTOR_M_SHIFT		0

#define PLL1_CFG_DEFAULT	0xa1005000

#define PLL6_CFG_DEFAULT	0xa1009911

/* nand clock */
#define NAND_CLK_SRC_OSC24		0
#define NAND_CLK_DIV_N			0
#define NAND_CLK_DIV_M			0

/* gps clock */
#define GPS_SCLK_GATING_OFF		0
#define GPS_RESET			0

/* ahb clock gate bit offset */
#define AHB_GATE_OFFSET_GPS		26
#define AHB_GATE_OFFSET_SATA		25
#define AHB_GATE_OFFSET_PATA		24
#define AHB_GATE_OFFSET_SPI3		23
#define AHB_GATE_OFFSET_SPI2		22
#define AHB_GATE_OFFSET_SPI1		21
#define AHB_GATE_OFFSET_SPI0		20
#define AHB_GATE_OFFSET_TS0		18
#define AHB_GATE_OFFSET_EMAC		17
#define AHB_GATE_OFFSET_ACE		16
#define AHB_GATE_OFFSET_DLL		15
#define AHB_GATE_OFFSET_SDRAM		14
#define AHB_GATE_OFFSET_NAND		13
#define AHB_GATE_OFFSET_MS		12
#define AHB_GATE_OFFSET_MMC3		11
#define AHB_GATE_OFFSET_MMC2		10
#define AHB_GATE_OFFSET_MMC1		9
#define AHB_GATE_OFFSET_MMC0		8
#define AHB_GATE_OFFSET_MMC(n)		(AHB_GATE_OFFSET_MMC0 + (n))
#define AHB_GATE_OFFSET_BIST		7
#define AHB_GATE_OFFSET_DMA		6
#define AHB_GATE_OFFSET_SS		5
#define AHB_GATE_OFFSET_USB_OHCI1	4
#define AHB_GATE_OFFSET_USB_EHCI1	3
#define AHB_GATE_OFFSET_USB_OHCI0	2
#define AHB_GATE_OFFSET_USB_EHCI0	1
#define AHB_GATE_OFFSET_USB		0

/* ahb clock gate bit offset (second register) */
#define AHB_GATE_OFFSET_GMAC		17

#define CCM_AHB_GATE_GPS (0x1 << 26)
#define CCM_AHB_GATE_SDRAM (0x1 << 14)
#define CCM_AHB_GATE_DLL (0x1 << 15)
#define CCM_AHB_GATE_ACE (0x1 << 16)

#define CCM_PLL5_CTRL_M(n) (((n) & 0x3) << 0)
#define CCM_PLL5_CTRL_M_MASK CCM_PLL5_CTRL_M(0x3)
#define CCM_PLL5_CTRL_M_X(n) ((n) - 1)
#define CCM_PLL5_CTRL_M1(n) (((n) & 0x3) << 2)
#define CCM_PLL5_CTRL_M1_MASK CCM_PLL5_CTRL_M1(0x3)
#define CCM_PLL5_CTRL_M1_X(n) ((n) - 1)
#define CCM_PLL5_CTRL_K(n) (((n) & 0x3) << 4)
#define CCM_PLL5_CTRL_K_MASK CCM_PLL5_CTRL_K(0x3)
#define CCM_PLL5_CTRL_K_X(n) ((n) - 1)
#define CCM_PLL5_CTRL_LDO (0x1 << 7)
#define CCM_PLL5_CTRL_N(n) (((n) & 0x1f) << 8)
#define CCM_PLL5_CTRL_N_MASK CCM_PLL5_CTRL_N(0x1f)
#define CCM_PLL5_CTRL_N_X(n) (n)
#define CCM_PLL5_CTRL_P(n) (((n) & 0x3) << 16)
#define CCM_PLL5_CTRL_P_MASK CCM_PLL5_CTRL_P(0x3)
#define CCM_PLL5_CTRL_P_X(n) ((n) - 1)
#define CCM_PLL5_CTRL_BW (0x1 << 18)
#define CCM_PLL5_CTRL_VCO_GAIN (0x1 << 19)
#define CCM_PLL5_CTRL_BIAS(n) (((n) & 0x1f) << 20)
#define CCM_PLL5_CTRL_BIAS_MASK CCM_PLL5_CTRL_BIAS(0x1f)
#define CCM_PLL5_CTRL_BIAS_X(n) ((n) - 1)
#define CCM_PLL5_CTRL_VCO_BIAS (0x1 << 25)
#define CCM_PLL5_CTRL_DDR_CLK (0x1 << 29)
#define CCM_PLL5_CTRL_BYPASS (0x1 << 30)
#define CCM_PLL5_CTRL_EN (0x1 << 31)

#define CCM_PLL6_CTRL_N_SHIFT	8
#define CCM_PLL6_CTRL_N_MASK	(0x1f << CCM_PLL6_CTRL_N_SHIFT)
#define CCM_PLL6_CTRL_K_SHIFT	4
#define CCM_PLL6_CTRL_K_MASK	(0x3 << CCM_PLL6_CTRL_K_SHIFT)

#define CCM_GPS_CTRL_RESET (0x1 << 0)
#define CCM_GPS_CTRL_GATE (0x1 << 1)

#define CCM_DRAM_CTRL_DCLK_OUT (0x1 << 15)

#define CCM_MBUS_CTRL_M(n) (((n) & 0xf) << 0)
#define CCM_MBUS_CTRL_M_MASK CCM_MBUS_CTRL_M(0xf)
#define CCM_MBUS_CTRL_M_X(n) ((n) - 1)
#define CCM_MBUS_CTRL_N(n) (((n) & 0xf) << 16)
#define CCM_MBUS_CTRL_N_MASK CCM_MBUS_CTRL_N(0xf)
#define CCM_MBUS_CTRL_N_X(n) (((n) >> 3) ? 3 : (((n) >> 2) ? 2 : (((n) >> 1) ? 1 : 0)))
#define CCM_MBUS_CTRL_CLK_SRC(n) (((n) & 0x3) << 24)
#define CCM_MBUS_CTRL_CLK_SRC_MASK CCM_MBUS_CTRL_CLK_SRC(0x3)
#define CCM_MBUS_CTRL_CLK_SRC_HOSC 0x0
#define CCM_MBUS_CTRL_CLK_SRC_PLL6 0x1
#define CCM_MBUS_CTRL_CLK_SRC_PLL5 0x2
#define CCM_MBUS_CTRL_GATE (0x1 << 31)

#define CCM_MMC_CTRL_OSCM24 (0x0 << 24)
#define CCM_MMC_CTRL_PLL6   (0x1 << 24)
#define CCM_MMC_CTRL_PLL5   (0x2 << 24)

#define CCM_MMC_CTRL_ENABLE (0x1 << 31)

#define CCM_GMAC_CTRL_TX_CLK_SRC_MII 0x0
#define CCM_GMAC_CTRL_TX_CLK_SRC_EXT_RGMII 0x1
#define CCM_GMAC_CTRL_TX_CLK_SRC_INT_RGMII 0x2
#define CCM_GMAC_CTRL_GPIT_MII (0x0 << 2)
#define CCM_GMAC_CTRL_GPIT_RGMII (0x1 << 2)

#endif /* _SUNXI_CLOCK_SUN4I_H */
/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Berg Xing <bergxing@allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Sunxi platform dram register definition.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _SUNXI_DRAM_H
#define _SUNXI_DRAM_H

//#include <linux/types.h>

struct sunxi_dram_reg {
	uint32_t ccr;		/* 0x00 controller configuration register */
	uint32_t dcr;		/* 0x04 dram configuration register */
	uint32_t iocr;		/* 0x08 i/o configuration register */
	uint32_t csr;		/* 0x0c controller status register */
	uint32_t drr;		/* 0x10 dram refresh register */
	uint32_t tpr0;		/* 0x14 dram timing parameters register 0 */
	uint32_t tpr1;		/* 0x18 dram timing parameters register 1 */
	uint32_t tpr2;		/* 0x1c dram timing parameters register 2 */
	uint32_t gdllcr;		/* 0x20 global dll control register */
	uint8_t res0[0x28];
	uint32_t rslr0;		/* 0x4c rank system latency register */
	uint32_t rslr1;		/* 0x50 rank system latency register */
	uint8_t res1[0x8];
	uint32_t rdgr0;		/* 0x5c rank dqs gating register */
	uint32_t rdgr1;		/* 0x60 rank dqs gating register */
	uint8_t res2[0x34];
	uint32_t odtcr;		/* 0x98 odt configuration register */
	uint32_t dtr0;		/* 0x9c data training register 0 */
	uint32_t dtr1;		/* 0xa0 data training register 1 */
	uint32_t dtar;		/* 0xa4 data training address register */
	uint32_t zqcr0;		/* 0xa8 zq control register 0 */
	uint32_t zqcr1;		/* 0xac zq control register 1 */
	uint32_t zqsr;		/* 0xb0 zq status register */
	uint32_t idcr;		/* 0xb4 initializaton delay configure reg */
	uint8_t res3[0x138];
	uint32_t mr;			/* 0x1f0 mode register */
	uint32_t emr;		/* 0x1f4 extended mode register */
	uint32_t emr2;		/* 0x1f8 extended mode register */
	uint32_t emr3;		/* 0x1fc extended mode register */
	uint32_t dllctr;		/* 0x200 dll control register */
	uint32_t dllcr[5];		/* 0x204 dll control register 0(byte 0) */
	/* 0x208 dll control register 1(byte 1) */
	/* 0x20c dll control register 2(byte 2) */
	/* 0x210 dll control register 3(byte 3) */
	/* 0x214 dll control register 4(byte 4) */
	uint32_t dqtr0;		/* 0x218 dq timing register */
	uint32_t dqtr1;		/* 0x21c dq timing register */
	uint32_t dqtr2;		/* 0x220 dq timing register */
	uint32_t dqtr3;		/* 0x224 dq timing register */
	uint32_t dqstr;		/* 0x228 dqs timing register */
	uint32_t dqsbtr;		/* 0x22c dqsb timing register */
	uint32_t mcr;		/* 0x230 mode configure register */
	uint8_t res[0x8];
	uint32_t ppwrsctl;		/* 0x23c pad power save control */
	uint32_t apr;		/* 0x240 arbiter period register */
	uint32_t pldtr;		/* 0x244 priority level data threshold reg */
	uint8_t res5[0x8];
	uint32_t hpcr[32];		/* 0x250 host port configure register */
	uint8_t res6[0x10];
	uint32_t csel;		/* 0x2e0 controller select register */
};

struct dram_para {
	uint32_t clock;
	uint32_t type;
	uint32_t rank_num;
	uint32_t density;
	uint32_t io_width;
	uint32_t bus_width;
	uint32_t cas;
	uint32_t zq;
	uint32_t odt_en;
	uint32_t size;
	uint32_t tpr0;
	uint32_t tpr1;
	uint32_t tpr2;
	uint32_t tpr3;
	uint32_t tpr4;
	uint32_t tpr5;
	uint32_t emr1;
	uint32_t emr2;
	uint32_t emr3;
};

#define DRAM_CCR_COMMAND_RATE_1T (0x1 << 5)
#define DRAM_CCR_DQS_GATE (0x1 << 14)
#define DRAM_CCR_DQS_DRIFT_COMP (0x1 << 17)
#define DRAM_CCR_ITM_OFF (0x1 << 28)
#define DRAM_CCR_DATA_TRAINING (0x1 << 30)
#define DRAM_CCR_INIT (0x1 << 31)

#define DRAM_MEMORY_TYPE_DDR1 1
#define DRAM_MEMORY_TYPE_DDR2 2
#define DRAM_MEMORY_TYPE_DDR3 3
#define DRAM_MEMORY_TYPE_LPDDR2 4
#define DRAM_MEMORY_TYPE_LPDDR 5
#define DRAM_DCR_TYPE (0x1 << 0)
#define DRAM_DCR_TYPE_DDR2 0x0
#define DRAM_DCR_TYPE_DDR3 0x1
#define DRAM_DCR_IO_WIDTH(n) (((n) & 0x3) << 1)
#define DRAM_DCR_IO_WIDTH_MASK DRAM_DCR_IO_WIDTH(0x3)
#define DRAM_DCR_IO_WIDTH_8BIT 0x0
#define DRAM_DCR_IO_WIDTH_16BIT 0x1
#define DRAM_DCR_CHIP_DENSITY(n) (((n) & 0x7) << 3)
#define DRAM_DCR_CHIP_DENSITY_MASK DRAM_DCR_CHIP_DENSITY(0x7)
#define DRAM_DCR_CHIP_DENSITY_256M 0x0
#define DRAM_DCR_CHIP_DENSITY_512M 0x1
#define DRAM_DCR_CHIP_DENSITY_1024M 0x2
#define DRAM_DCR_CHIP_DENSITY_2048M 0x3
#define DRAM_DCR_CHIP_DENSITY_4096M 0x4
#define DRAM_DCR_CHIP_DENSITY_8192M 0x5
#define DRAM_DCR_BUS_WIDTH(n) (((n) & 0x7) << 6)
#define DRAM_DCR_BUS_WIDTH_MASK DRAM_DCR_BUS_WIDTH(0x7)
#define DRAM_DCR_BUS_WIDTH_32BIT 0x3
#define DRAM_DCR_BUS_WIDTH_16BIT 0x1
#define DRAM_DCR_BUS_WIDTH_8BIT 0x0
#define DRAM_DCR_NR_DLLCR_32BIT 5
#define DRAM_DCR_NR_DLLCR_16BIT 3
#define DRAM_DCR_NR_DLLCR_8BIT 2
#define DRAM_DCR_RANK_SEL(n) (((n) & 0x3) << 10)
#define DRAM_DCR_RANK_SEL_MASK DRAM_DCR_CMD_RANK(0x3)
#define DRAM_DCR_CMD_RANK_ALL (0x1 << 12)
#define DRAM_DCR_MODE(n) (((n) & 0x3) << 13)
#define DRAM_DCR_MODE_MASK DRAM_DCR_MODE(0x3)
#define DRAM_DCR_MODE_SEQ 0x0
#define DRAM_DCR_MODE_INTERLEAVE 0x1

#define DRAM_CSR_FAILED (0x1 << 20)

#define DRAM_DRR_TRFC(n) ((n) & 0xff)
#define DRAM_DRR_TREFI(n) (((n) & 0xffff) << 8)
#define DRAM_DRR_BURST(n) ((((n) - 1) & 0xf) << 24)

#define DRAM_MCR_MODE_NORM(n) (((n) & 0x3) << 0)
#define DRAM_MCR_MODE_NORM_MASK DRAM_MCR_MOD_NORM(0x3)
#define DRAM_MCR_MODE_DQ_OUT(n) (((n) & 0x3) << 2)
#define DRAM_MCR_MODE_DQ_OUT_MASK DRAM_MCR_MODE_DQ_OUT(0x3)
#define DRAM_MCR_MODE_ADDR_OUT(n) (((n) & 0x3) << 4)
#define DRAM_MCR_MODE_ADDR_OUT_MASK DRAM_MCR_MODE_ADDR_OUT(0x3)
#define DRAM_MCR_MODE_DQ_IN_OUT(n) (((n) & 0x3) << 6)
#define DRAM_MCR_MODE_DQ_IN_OUT_MASK DRAM_MCR_MODE_DQ_IN_OUT(0x3)
#define DRAM_MCR_MODE_DQ_TURNON_DELAY(n) (((n) & 0x7) << 8)
#define DRAM_MCR_MODE_DQ_TURNON_DELAY_MASK DRAM_MCR_MODE_DQ_TURNON_DELAY(0x7)
#define DRAM_MCR_MODE_ADDR_IN (0x1 << 11)
#define DRAM_MCR_RESET (0x1 << 12)
#define DRAM_MCR_MODE_EN(n) (((n) & 0x3) << 13)
#define DRAM_MCR_MODE_EN_MASK DRAM_MCR_MOD_EN(0x3)
#define DRAM_MCR_DCLK_OUT (0x1 << 16)

#define DRAM_DLLCR_NRESET (0x1 << 30)
#define DRAM_DLLCR_DISABLE (0x1 << 31)

#define DRAM_ZQCR0_IMP_DIV(n) (((n) & 0xff) << 20)
#define DRAM_ZQCR0_IMP_DIV_MASK DRAM_ZQCR0_IMP_DIV(0xff)

#define DRAM_IOCR_ODT_EN(n) ((((n) & 0x3) << 30) | ((n) & 0x3) << 0)
#define DRAM_IOCR_ODT_EN_MASK DRAM_IOCR_ODT_EN(0x3)

#define DRAM_MR_BURST_LENGTH(n) (((n) & 0x7) << 0)
#define DRAM_MR_BURST_LENGTH_MASK DRAM_MR_BURST_LENGTH(0x7)
#define DRAM_MR_CAS_LAT(n) (((n) & 0x7) << 4)
#define DRAM_MR_CAS_LAT_MASK DRAM_MR_CAS_LAT(0x7)
#define DRAM_MR_WRITE_RECOVERY(n) (((n) & 0x7) << 9)
#define DRAM_MR_WRITE_RECOVERY_MASK DRAM_MR_WRITE_RECOVERY(0x7)
#define DRAM_MR_POWER_DOWN (0x1 << 12)

#define DRAM_CSEL_MAGIC 0x16237495

unsigned long sunxi_dram_init(void);
unsigned long dramc_init(struct dram_para *para);

#endif /* _SUNXI_DRAM_H */
/*
 * sunxi DRAM controller initialization
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 * (C) Copyright 2013 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
 *
 * Based on sun4i Linux kernel sources mach-sunxi/pm/standby/dram*.c
 * and earlier U-Boot Allwiner A10 SPL work
 *
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Berg Xing <bergxing@allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Unfortunately the only documentation we have on the sun7i DRAM
 * controller is Allwinner boot0 + boot1 code, and that code uses
 * magic numbers & shifts with no explanations. Hence this code is
 * rather undocumented and full of magic.
 */
//
//#include <common.h>
//#include <asm/io.h>
//#include <asm/arch/clock.h>
//#include <asm/arch/dram.h>
//#include <asm/arch/timer.h>
//#include <asm/arch/sys_proto.h>

#define CPU_CFG_CHIP_VER(n) ((n) << 6)
#define CPU_CFG_CHIP_VER_MASK CPU_CFG_CHIP_VER(0x3)
#define CPU_CFG_CHIP_REV_A 0x0
#define CPU_CFG_CHIP_REV_C1 0x1
#define CPU_CFG_CHIP_REV_C2 0x2
#define CPU_CFG_CHIP_REV_B 0x3

static void panic(const char * s)
{
	PRINTF("%s", s);
	for (;;)
		;
}
/*
 * Wait up to 1s for mask to be clear in given reg.
 */
static void await_completion(uint32_t *reg, uint32_t mask)
{
	//unsigned long tmo = timer_get_us() + 1000000;

	while (read32ptr(reg) & mask) {
		//
		//if (timer_get_us() > tmo)
		//	panic("Timeout initialising DRAM\n");
	}
}

static void mctl_ddr3_reset(void)
{
	struct sunxi_dram_reg *dram =
			(struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	clrbits_le32(&dram->mcr, DRAM_MCR_RESET);
	udelay(2);
	setbits_le32(&dram->mcr, DRAM_MCR_RESET);
}

static void mctl_set_drive(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	clrsetbits_le32(&dram->mcr, DRAM_MCR_MODE_NORM(0x3) | (0x3 << 28),
			DRAM_MCR_MODE_EN(0x3) |
			0xffc);
}

static void mctl_itm_disable(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	clrsetbits_le32(&dram->ccr, DRAM_CCR_INIT, DRAM_CCR_ITM_OFF);
}

static void mctl_itm_enable(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	clrbits_le32(&dram->ccr, DRAM_CCR_ITM_OFF);
}

static void mctl_enable_dll0(uint32_t phase)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	clrsetbits_le32(&dram->dllcr[0], 0x3f << 6,
			((phase >> 16) & 0x3f) << 6);
	clrsetbits_le32(&dram->dllcr[0], DRAM_DLLCR_NRESET, DRAM_DLLCR_DISABLE);
	udelay(2);

	clrbits_le32(&dram->dllcr[0], DRAM_DLLCR_NRESET | DRAM_DLLCR_DISABLE);
	udelay(22);

	clrsetbits_le32(&dram->dllcr[0], DRAM_DLLCR_DISABLE, DRAM_DLLCR_NRESET);
	udelay(22);
}

/*
 * Note: This differs from pm/standby in that it checks the bus width
 */
static void mctl_enable_dllx(uint32_t phase)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	uint32_t i, n, bus_width;

	bus_width = readl(&dram->dcr);

	if ((bus_width & DRAM_DCR_BUS_WIDTH_MASK) ==
	    DRAM_DCR_BUS_WIDTH(DRAM_DCR_BUS_WIDTH_32BIT))
		n = DRAM_DCR_NR_DLLCR_32BIT;
	else
		n = DRAM_DCR_NR_DLLCR_16BIT;

	for (i = 1; i < n; i++) {
		clrsetbits_le32(&dram->dllcr[i], 0xf << 14,
				(phase & 0xf) << 14);
		clrsetbits_le32(&dram->dllcr[i], DRAM_DLLCR_NRESET,
				DRAM_DLLCR_DISABLE);
		phase >>= 4;
	}
	udelay(2);

	for (i = 1; i < n; i++)
		clrbits_le32(&dram->dllcr[i], DRAM_DLLCR_NRESET |
			     DRAM_DLLCR_DISABLE);
	udelay(22);

	for (i = 1; i < n; i++)
		clrsetbits_le32(&dram->dllcr[i], DRAM_DLLCR_DISABLE,
				DRAM_DLLCR_NRESET);
	udelay(22);
}

static uint32_t hpcr_value[32] = {
#ifdef CONFIG_SUN7I
	0x0301, 0x0301, 0x0301, 0x0301,
	0x0301, 0x0301, 0x0301, 0x0301,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0x1031, 0x1031, 0x0735, 0x1035,
	0x1035, 0x0731, 0x1031, 0x0735,
	0x1035, 0x1031, 0x0731, 0x1035,
	0x0001, 0x1031, 0, 0x1031
	/* last row differs from boot0 source table
	 * 0x1031, 0x0301, 0x0301, 0x0731
	 * but boot0 code skips #28 and #30, and sets #29 and #31 to the
	 * value from #28 entry (0x1031)
	 */
#endif
};

static void mctl_configure_hostport(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	uint32_t i;

	for (i = 0; i < 32; i++)
		writel(hpcr_value[i], &dram->hpcr[i]);
}

static void mctl_setup_dram_clock(uint32_t clk)
{
	uint32_t reg_val;
	struct sunxi_ccm_reg *ccm = (struct sunxi_ccm_reg *) CCU_BASE; //SUNXI_CCM_BASE;

	/* setup DRAM PLL */
	reg_val = readl(&ccm->pll5_cfg);
	reg_val &= ~CCM_PLL5_CTRL_M_MASK;		/* set M to 0 (x1) */
	reg_val &= ~CCM_PLL5_CTRL_K_MASK;		/* set K to 0 (x1) */
	reg_val &= ~CCM_PLL5_CTRL_N_MASK;		/* set N to 0 (x0) */
	reg_val &= ~CCM_PLL5_CTRL_P_MASK;		/* set P to 0 (x1) */
	if (clk >= 540 && clk < 552) {
		/* dram = 540MHz, pll5p = 540MHz */
		reg_val |= CCM_PLL5_CTRL_M(CCM_PLL5_CTRL_M_X(2));
		reg_val |= CCM_PLL5_CTRL_K(CCM_PLL5_CTRL_K_X(3));
		reg_val |= CCM_PLL5_CTRL_N(CCM_PLL5_CTRL_N_X(15));
		reg_val |= CCM_PLL5_CTRL_P(1);
	} else if (clk >= 512 && clk < 528) {
		/* dram = 512MHz, pll5p = 384MHz */
		reg_val |= CCM_PLL5_CTRL_M(CCM_PLL5_CTRL_M_X(3));
		reg_val |= CCM_PLL5_CTRL_K(CCM_PLL5_CTRL_K_X(4));
		reg_val |= CCM_PLL5_CTRL_N(CCM_PLL5_CTRL_N_X(16));
		reg_val |= CCM_PLL5_CTRL_P(2);
	} else if (clk >= 496 && clk < 504) {
		/* dram = 496MHz, pll5p = 372MHz */
		reg_val |= CCM_PLL5_CTRL_M(CCM_PLL5_CTRL_M_X(3));
		reg_val |= CCM_PLL5_CTRL_K(CCM_PLL5_CTRL_K_X(2));
		reg_val |= CCM_PLL5_CTRL_N(CCM_PLL5_CTRL_N_X(31));
		reg_val |= CCM_PLL5_CTRL_P(2);
	} else if (clk >= 468 && clk < 480) {
		/* dram = 468MHz, pll5p = 468MHz */
		reg_val |= CCM_PLL5_CTRL_M(CCM_PLL5_CTRL_M_X(2));
		reg_val |= CCM_PLL5_CTRL_K(CCM_PLL5_CTRL_K_X(3));
		reg_val |= CCM_PLL5_CTRL_N(CCM_PLL5_CTRL_N_X(13));
		reg_val |= CCM_PLL5_CTRL_P(1);
	} else if (clk >= 396 && clk < 408) {
		/* dram = 396MHz, pll5p = 396MHz */
		reg_val |= CCM_PLL5_CTRL_M(CCM_PLL5_CTRL_M_X(2));
		reg_val |= CCM_PLL5_CTRL_K(CCM_PLL5_CTRL_K_X(3));
		reg_val |= CCM_PLL5_CTRL_N(CCM_PLL5_CTRL_N_X(11));
		reg_val |= CCM_PLL5_CTRL_P(1);
	} else 	{
		/* any other frequency that is a multiple of 24 */
		reg_val |= CCM_PLL5_CTRL_M(CCM_PLL5_CTRL_M_X(2));
		reg_val |= CCM_PLL5_CTRL_K(CCM_PLL5_CTRL_K_X(2));
		reg_val |= CCM_PLL5_CTRL_N(CCM_PLL5_CTRL_N_X(clk / 24));
		reg_val |= CCM_PLL5_CTRL_P(CCM_PLL5_CTRL_P_X(2));
	}
	reg_val &= ~CCM_PLL5_CTRL_VCO_GAIN;		/* PLL VCO Gain off */
	reg_val |= CCM_PLL5_CTRL_EN;			/* PLL On */
	writel(reg_val, &ccm->pll5_cfg);
	udelay(5500);

	setbits_le32(&ccm->pll5_cfg, CCM_PLL5_CTRL_DDR_CLK);

#if defined(CONFIG_SUN4I) || defined(CONFIG_SUN7I)
	/* reset GPS */
	clrbits_le32(&ccm->gps_clk_cfg, CCM_GPS_CTRL_RESET | CCM_GPS_CTRL_GATE);
	setbits_le32(&ccm->ahb_gate0, CCM_AHB_GATE_GPS);
	udelay(1);
	clrbits_le32(&ccm->ahb_gate0, CCM_AHB_GATE_GPS);
#endif

	/* setup MBUS clock */
	reg_val = CCM_MBUS_CTRL_GATE |
		  CCM_MBUS_CTRL_CLK_SRC(CCM_MBUS_CTRL_CLK_SRC_PLL6) |
		  CCM_MBUS_CTRL_N(CCM_MBUS_CTRL_N_X(2)) |
		  CCM_MBUS_CTRL_M(CCM_MBUS_CTRL_M_X(2));
	writel(reg_val, &ccm->mbus_clk_cfg);

	/*
	 * open DRAMC AHB & DLL register clock
	 * close it first
	 */
	clrbits_le32(&ccm->ahb_gate0, CCM_AHB_GATE_SDRAM | CCM_AHB_GATE_DLL);
	udelay(22);

	/* then open it */
	setbits_le32(&ccm->ahb_gate0, CCM_AHB_GATE_SDRAM | CCM_AHB_GATE_DLL);
	udelay(22);
}

static int dramc_scan_readpipe(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	uint32_t reg_val;

	/* data training trigger */
#ifdef CONFIG_SUN7I
	clrbits_le32(&dram->csr, DRAM_CSR_FAILED);
#endif
	setbits_le32(&dram->ccr, DRAM_CCR_DATA_TRAINING);

	/* check whether data training process has completed */
	await_completion(&dram->ccr, DRAM_CCR_DATA_TRAINING);

	/* check data training result */
	reg_val = readl(&dram->csr);
	if (reg_val & DRAM_CSR_FAILED)
		return -1;

	return 0;
}

static int dramc_scan_dll_para(void)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	static const uint32_t dqs_dly[7] = {0x3, 0x2, 0x1, 0x0, 0xe, 0xd, 0xc};
	static const uint32_t clk_dly[15] = {0x07, 0x06, 0x05, 0x04, 0x03,
				 0x02, 0x01, 0x00, 0x08, 0x10,
				 0x18, 0x20, 0x28, 0x30, 0x38};
	uint32_t clk_dqs_count[15];
	uint32_t dqs_i, clk_i, cr_i;
	uint32_t max_val, min_val;
	uint32_t dqs_index, clk_index;

	/* Find DQS_DLY Pass Count for every CLK_DLY */
	for (clk_i = 0; clk_i < 15; clk_i++) {
		clk_dqs_count[clk_i] = 0;
		clrsetbits_le32(&dram->dllcr[0], 0x3f << 6,
				(clk_dly[clk_i] & 0x3f) << 6);
		for (dqs_i = 0; dqs_i < 7; dqs_i++) {
			for (cr_i = 1; cr_i < 5; cr_i++) {
				clrsetbits_le32(&dram->dllcr[cr_i],
						0x4f << 14,
						(dqs_dly[dqs_i] & 0x4f) << 14);
			}
			udelay(2);
			if (dramc_scan_readpipe() == 0)
				clk_dqs_count[clk_i]++;
		}
	}
	/* Test DQS_DLY Pass Count for every CLK_DLY from up to down */
	for (dqs_i = 15; dqs_i > 0; dqs_i--) {
		max_val = 15;
		min_val = 15;
		for (clk_i = 0; clk_i < 15; clk_i++) {
			if (clk_dqs_count[clk_i] == dqs_i) {
				max_val = clk_i;
				if (min_val == 15)
					min_val = clk_i;
			}
		}
		if (max_val < 15)
			break;
	}

	/* Check if Find a CLK_DLY failed */
	if (!dqs_i)
		goto fail;

	/* Find the middle index of CLK_DLY */
	clk_index = (max_val + min_val) >> 1;
	if ((max_val == (15 - 1)) && (min_val > 0))
		/* if CLK_DLY[MCTL_CLK_DLY_COUNT] is very good, then the middle
		 * value can be more close to the max_val
		 */
		clk_index = (15 + clk_index) >> 1;
	else if ((max_val < (15 - 1)) && (min_val == 0))
		/* if CLK_DLY[0] is very good, then the middle value can be more
		 * close to the min_val
		 */
		clk_index >>= 1;
	if (clk_dqs_count[clk_index] < dqs_i)
		clk_index = min_val;

	/* Find the middle index of DQS_DLY for the CLK_DLY got above, and Scan
	 * read pipe again
	 */
	clrsetbits_le32(&dram->dllcr[0], 0x3f << 6,
			(clk_dly[clk_index] & 0x3f) << 6);
	max_val = 7;
	min_val = 7;
	for (dqs_i = 0; dqs_i < 7; dqs_i++) {
		clk_dqs_count[dqs_i] = 0;
		for (cr_i = 1; cr_i < 5; cr_i++) {
			clrsetbits_le32(&dram->dllcr[cr_i],
					0x4f << 14,
					(dqs_dly[dqs_i] & 0x4f) << 14);
		}
		udelay(2);
		if (dramc_scan_readpipe() == 0) {
			clk_dqs_count[dqs_i] = 1;
			max_val = dqs_i;
			if (min_val == 7)
				min_val = dqs_i;
		}
	}

	if (max_val < 7) {
		dqs_index = (max_val + min_val) >> 1;
		if ((max_val == (7-1)) && (min_val > 0))
			dqs_index = (7 + dqs_index) >> 1;
		else if ((max_val < (7-1)) && (min_val == 0))
			dqs_index >>= 1;
		if (!clk_dqs_count[dqs_index])
			dqs_index = min_val;
		for (cr_i = 1; cr_i < 5; cr_i++) {
			clrsetbits_le32(&dram->dllcr[cr_i],
					0x4f << 14,
					(dqs_dly[dqs_index] & 0x4f) << 14);
		}
		udelay(2);
		return dramc_scan_readpipe();
	}

fail:
	clrbits_le32(&dram->dllcr[0], 0x3f << 6);
	for (cr_i = 1; cr_i < 5; cr_i++)
		clrbits_le32(&dram->dllcr[cr_i], 0x4f << 14);
	udelay(2);

	return dramc_scan_readpipe();
}

static void dramc_clock_output_en(uint32_t on)
{
#if defined(CONFIG_SUN5I) || defined(CONFIG_SUN7I)
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;

	if (on)
		setbits_le32(&dram->mcr, DRAM_MCR_DCLK_OUT);
	else
		clrbits_le32(&dram->mcr, DRAM_MCR_DCLK_OUT);
#endif
}

static const uint16_t tRFC_table[2][6] = {
	/*       256Mb    512Mb    1Gb      2Gb      4Gb      8Gb      */
	/* DDR2  75ns     105ns    127.5ns  195ns    327.5ns  invalid  */
	{        77,      108,     131,     200,     336,     336 },
	/* DDR3  invalid  90ns     110ns    160ns    300ns    350ns    */
	{        93,      93,      113,     164,     308,     359 }
};

static void dramc_set_autorefresh_cycle(uint32_t clk, uint32_t type, uint32_t density)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	uint32_t tRFC, tREFI;

	tRFC = (tRFC_table[type][density] * clk + 1023) >> 10;
	tREFI = (7987 * clk) >> 10;	/* <= 7.8us */

	writel(DRAM_DRR_TREFI(tREFI) | DRAM_DRR_TRFC(tRFC), &dram->drr);
}

unsigned long dramc_init(struct dram_para *para)
{
	struct sunxi_dram_reg *dram = (struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	uint32_t reg_val;
	uint32_t density;
	int ret_val;

	/* check input dram parameter structure */
	if (!para)
		return 0;

	/* setup DRAM relative clock */
	mctl_setup_dram_clock(para->clock);

	/* reset external DRAM */
	mctl_set_drive();

	/* dram clock off */
	dramc_clock_output_en(0);

	mctl_itm_disable();
	mctl_enable_dll0(para->tpr3);

	/* configure external DRAM */
	reg_val = 0x0;
	if (para->type == DRAM_MEMORY_TYPE_DDR3)
		reg_val |= DRAM_DCR_TYPE_DDR3;
	reg_val |= DRAM_DCR_IO_WIDTH(para->io_width >> 3);

	if (para->density == 256)
		density = DRAM_DCR_CHIP_DENSITY_256M;
	else if (para->density == 512)
		density = DRAM_DCR_CHIP_DENSITY_512M;
	else if (para->density == 1024)
		density = DRAM_DCR_CHIP_DENSITY_1024M;
	else if (para->density == 2048)
		density = DRAM_DCR_CHIP_DENSITY_2048M;
	else if (para->density == 4096)
		density = DRAM_DCR_CHIP_DENSITY_4096M;
	else if (para->density == 8192)
		density = DRAM_DCR_CHIP_DENSITY_8192M;
	else
		density = DRAM_DCR_CHIP_DENSITY_256M;

	reg_val |= DRAM_DCR_CHIP_DENSITY(density);
	reg_val |= DRAM_DCR_BUS_WIDTH((para->bus_width >> 3) - 1);
	reg_val |= DRAM_DCR_RANK_SEL(para->rank_num - 1);
	reg_val |= DRAM_DCR_CMD_RANK_ALL;
	reg_val |= DRAM_DCR_MODE(DRAM_DCR_MODE_INTERLEAVE);
	writel(reg_val, &dram->dcr);

#ifdef CONFIG_SUN7I
	setbits_le32(&dram->zqcr1, (0x1 << 24) | (0x1 << 1));
	if (para->tpr4 & 0x2)
		clrsetbits_le32(&dram->zqcr1, (0x1 << 24), (0x1 << 1));
	dramc_clock_output_en(1);
#endif

#if (defined(CONFIG_SUN5I) || defined(CONFIG_SUN7I))
	/* set odt impendance divide ratio */
	reg_val = ((para->zq) >> 8) & 0xfffff;
	reg_val |= ((para->zq) & 0xff) << 20;
	reg_val |= (para->zq) & 0xf0000000;
	writel(reg_val, &dram->zqcr0);
#endif

#ifdef CONFIG_SUN7I
	/* Set CKE Delay to about 1ms */
	setbits_le32(&dram->idcr, 0x1ffff);
#endif

#ifdef CONFIG_SUN7I
	if ((readl(&dram->ppwrsctl) & 0x1) != 0x1)
		mctl_ddr3_reset();
	else
		setbits_le32(&dram->mcr, DRAM_MCR_RESET);
#endif

	udelay(1);

	await_completion(&dram->ccr, DRAM_CCR_INIT);

	mctl_enable_dllx(para->tpr3);

	/* set refresh period */
	dramc_set_autorefresh_cycle(para->clock, para->type - 2, density);

	/* set timing parameters */
	writel(para->tpr0, &dram->tpr0);
	writel(para->tpr1, &dram->tpr1);
	writel(para->tpr2, &dram->tpr2);

	if (para->type == DRAM_MEMORY_TYPE_DDR3) {
		reg_val = DRAM_MR_BURST_LENGTH(0x0);
#if (defined(CONFIG_SUN5I) || defined(CONFIG_SUN7I))
		reg_val |= DRAM_MR_POWER_DOWN;
#endif
		reg_val |= DRAM_MR_CAS_LAT(para->cas - 4);
		reg_val |= DRAM_MR_WRITE_RECOVERY(0x5);
	} else if (para->type == DRAM_MEMORY_TYPE_DDR2) {
		reg_val = DRAM_MR_BURST_LENGTH(0x2);
		reg_val |= DRAM_MR_CAS_LAT(para->cas);
		reg_val |= DRAM_MR_WRITE_RECOVERY(0x5);
	}
	writel(reg_val, &dram->mr);

	writel(para->emr1, &dram->emr);
	writel(para->emr2, &dram->emr2);
	writel(para->emr3, &dram->emr3);

	/* set DQS window mode */
	clrsetbits_le32(&dram->ccr, DRAM_CCR_DQS_DRIFT_COMP, DRAM_CCR_DQS_GATE);

#ifdef CONFIG_SUN7I
	/* Command rate timing mode 2T & 1T */
	if (para->tpr4 & 0x1)
		setbits_le32(&dram->ccr, DRAM_CCR_COMMAND_RATE_1T);
#endif
	/* reset external DRAM */
	setbits_le32(&dram->ccr, DRAM_CCR_INIT);
	await_completion(&dram->ccr, DRAM_CCR_INIT);

#ifdef CONFIG_SUN7I
	/* setup zq calibration manual */
	reg_val = readl(&dram->ppwrsctl);
	if ((reg_val & 0x1) == 1) {
		/* super_standby_flag = 1 */

		reg_val = readl(0x01c20c00 + 0x120); /* rtc */
		reg_val &= 0x000fffff;
		reg_val |= 0x17b00000;
		writel(reg_val, &dram->zqcr0);

		/* exit self-refresh state */
		clrsetbits_le32(&dram->dcr, 0x1f << 27, 0x12 << 27);
		/* check whether command has been executed */
		await_completion(&dram->dcr, 0x1 << 31);

		udelay(2);

		/* dram pad hold off */
		setbits_le32(&dram->ppwrsctl, 0x16510000);

		await_completion(&dram->ppwrsctl, 0x1);

		/* exit self-refresh state */
		clrsetbits_le32(&dram->dcr, 0x1f << 27, 0x12 << 27);

		/* check whether command has been executed */
		await_completion(&dram->dcr, 0x1 << 31);

		udelay(2);

		/* issue a refresh command */
		clrsetbits_le32(&dram->dcr, 0x1f << 27, 0x13 << 27);
		await_completion(&dram->dcr, 0x1 << 31);

		udelay(2);
	}
#endif

	/* scan read pipe value */
	mctl_itm_enable();
	if (para->tpr3 & (0x1 << 31)) {
		ret_val = dramc_scan_dll_para();
		if (ret_val == 0)
			para->tpr3 =
				(((readl(&dram->dllcr[0]) >> 6) & 0x3f) << 16) |
				(((readl(&dram->dllcr[1]) >> 14) & 0xf) << 0) |
				(((readl(&dram->dllcr[2]) >> 14) & 0xf) << 4) |
				(((readl(&dram->dllcr[3]) >> 14) & 0xf) << 8) |
				(((readl(&dram->dllcr[4]) >> 14) & 0xf) << 12
				);
	} else {
		ret_val = dramc_scan_readpipe();
	}

	if (ret_val < 0)
		return 0;

	/* configure all host port */
	mctl_configure_hostport();

	//return get_ram_size((long *)PHYS_SDRAM_0, PHYS_SDRAM_0_SIZE);
	return PHYS_SDRAM_0_SIZE;
}

void ddrinit(void)
{

	static struct dram_para dram_para = {
		.clock = 432,
		.type = 3,
		.rank_num = 1,
		.density = 4096,
		.io_width = 8,
		.bus_width = 32,
		.cas = 9,
		.zq = 0x7f,
		.odt_en = 0,
		.size = 2048,
		.tpr0 = 0x42d899b7,
		.tpr1 = 0xa090,
		.tpr2 = 0x22a00,
		.tpr3 = 0x0,
		.tpr4 = 0x1,
		.tpr5 = 0x0,
		.emr1 = 0x4,
		.emr2 = 0x10,
		.emr3 = 0x0,
	};

	dramc_init(& dram_para);
}

#endif


