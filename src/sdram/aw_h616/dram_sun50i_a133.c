// SPDX-License-Identifier: GPL-2.0+
/*
 * sun50i A133 platform dram controller driver
 *
 * Controller and PHY appear to be quite similar to that of the H616;
 * however certain offsets, timings, and other details are different enough that
 * the original code does not work as expected. Some device flags and
 * calibrations are not yet implemented, and configuration aside from DDR4
 * have not been tested.
 *
 * (C) Copyright 2024 MasterR3C0RD <masterr3c0rd@epochal.quest>
 *
 * Uses code from H616 driver, which is
 * (C) Copyright 2020 Jernej Skrabec <jernej.skrabec@siol.net>
 *
 */
#include "hardware.h"
#if WITHSDRAMHW && (CONFIG_SUNXI_DRAM_A133_LPDDR4) && 1
#include "formats.h"
#include "clocks.h"
#include <string.h>

#define max(a,b) (ulmax16((a), (b)))

#if 1
//https://github.com/u-boot/u-boot/blob/9b45d574af1b64fcd997f08fe702a2d0ecfccbe4/configs/liontron-h-a133l_defconfig#L5
#define CONFIG_DRAM_SUNXI_DX_ODT 0x7070707
#define CONFIG_DRAM_SUNXI_DX_DRI 0xd0d0d0d
#define CONFIG_DRAM_SUNXI_CA_DRI 0xe0e
#define CONFIG_DRAM_SUNXI_PARA0 0xd0a050c
#define CONFIG_DRAM_SUNXI_MR11 0x4
#define CONFIG_DRAM_SUNXI_MR12 0x72
#define CONFIG_DRAM_SUNXI_MR14 0x7
#define CONFIG_DRAM_SUNXI_TPR1 0x26
#define CONFIG_DRAM_SUNXI_TPR2 0x6060606
#define CONFIG_DRAM_SUNXI_TPR3 0x84040404
#define CONFIG_DRAM_SUNXI_TPR6 0x48000000
#define CONFIG_DRAM_SUNXI_TPR10 0x273333
#define CONFIG_DRAM_SUNXI_TPR11 0x231d151c
#define CONFIG_DRAM_SUNXI_TPR12 0x1212110e
#define CONFIG_DRAM_SUNXI_TPR13 0x7521
#define CONFIG_DRAM_SUNXI_TPR14 0x2023211f
//#define CONFIG_MACH_SUN50I_A133 y
//#define CONFIG_SUNXI_DRAM_A133_LPDDR4 y
//#define CONFIG_DRAM_CLK 792

#endif
#define CONFIG_DRAM_SUNXI_MR13 0	// во многих конфигурациях так

#define CFG_SYS_SDRAM_BASE 0x40000000

#define BIT_U32(pos) (UINT32_C(1) << (pos))

typedef uintptr_t virtual_addr_t;

static uint32_t read32(uintptr_t addr)
{
	__DSB();
	return * (volatile uint32_t *) addr;
}

static void write32(uintptr_t addr, uint32_t value)
{
	* (volatile uint32_t *) addr = value;
	__DSB();
}

static void write32ptr(volatile void * addr, uint32_t value)
{
	* (volatile uint32_t *) addr = value;
	__DSB();
}

static /*static inline*/ void sdelay(int loops)
{
//	__asm__ __volatile__ ("1:\n" "subs %0, %1, #1\n"
//		"bne 1b":"=r" (loops):"0"(loops));
	local_delay_us(1 * loops);
	while (loops --)
		__NOP();

}

static /*static inline*/ void udelay(int loops)
{
//	__asm__ __volatile__ ("1:\n" "subs %0, %1, #1\n"
//		"bne 1b":"=r" (loops):"0"(loops));
	local_delay_us(1 * loops);
//	while (loops --)
//		__NOP();

}

static uint32_t readl(void * addr)
{
	__DSB();
	return * (volatile uint32_t *) addr;
}

static void writel(uint32_t value, void * addr)
{
	* (volatile uint32_t *) addr = value;
	__DSB();
}
#define writel_relaxed(v, a) do { writel((v), (void *) (a)); } while (0)
#define readl_relaxed(a) (readl((void *) (a)))

#define panic PRINTF

#define clrbits_le32(addr, clear) \
	write32(((virtual_addr_t)(addr)), read32(((virtual_addr_t)(addr))) & ~(clear))

#define setbits_le32(addr, set) \
	write32(((virtual_addr_t)(addr)), read32(((virtual_addr_t)(addr))) | (set))

#define clrsetbits_le32(addr, clear, set) \
	write32(((virtual_addr_t)(addr)), (read32(((virtual_addr_t)(addr))) & ~(clear)) | (set))

#define SUNXI_CCM_BASE CCU_BASE

//Main CCU register offsets */
#define CCU_H6_PLL1_CFG			0x000
#define CCU_H6_PLL5_CFG			0x010
#define CCU_H6_PLL6_CFG			0x020
#define CCU_H6_CPU_AXI_CFG		0x500
#define CCU_H6_PSI_AHB1_AHB2_CFG	0x510
#define CCU_H6_AHB3_CFG			0x51c
#define CCU_H6_APB1_CFG			0x520
#define CCU_H6_APB2_CFG			0x524
#define CCU_H6_MBUS_CFG			0x540
#define CCU_H6_DRAM_CLK_CFG		0x800
#define CCU_H6_DRAM_GATE_RESET		0x80c
#define CCU_MMC0_CLK_CFG		0x830
#define CCU_MMC1_CLK_CFG		0x834
#define CCU_MMC2_CLK_CFG		0x838
#define CCU_H6_MMC_GATE_RESET		0x84c
#define CCU_H6_UART_GATE_RESET		0x90c
#define CCU_H6_I2C_GATE_RESET		0x91c

/* A523 CPU PLL offsets */
#define CPC_CPUA_PLL_CTRL		0x04
#define CPC_DSU_PLL_CTRL		0x08
#define CPC_CPUB_PLL_CTRL		0x0c
#define CPC_CPUA_CLK_REG		0x60
#define CPC_CPUB_CLK_REG		0x64
#define CPC_DSU_CLK_REG			0x6c

/* PLL bit fields */
#define CCM_PLL_CTRL_EN			BIT_U32(31)
#define CCM_PLL_LDO_EN			BIT_U32(30)
#define CCM_PLL_LOCK_EN			BIT_U32(29)
#define CCM_PLL_LOCK			BIT_U32(28)
#define CCM_PLL_OUT_EN			BIT_U32(27)
#define CCM_PLL1_UPDATE			BIT_U32(26)
#define CCM_PLL1_CTRL_P(p)		((p) << 16)
#define CCM_PLL1_CTRL_N_MASK		GENMASK(15, 8)
#define CCM_PLL1_CTRL_N(n)		(((n) - 1) << 8)

/* A523 CPU clock fields */
#define CPU_CLK_SRC_HOSC		(0 << 24)
#define CPU_CLK_SRC_CPUPLL		(3 << 24)
#define CPU_CLK_CTRL_P(p)		((p) << 16)
#define CPU_CLK_APB_DIV(n)		(((n) - 1) << 8)
#define CPU_CLK_PERI_DIV(m1)		(((m1) - 1) << 2)
#define CPU_CLK_AXI_DIV(m)		(((m) - 1) << 0)

/* pll5 bit field */
#define CCM_PLL5_CTRL_N(n)		(((n) - 1) << 8)
#define CCM_PLL5_CTRL_DIV1(div1)	((div1) << 0)
#define CCM_PLL5_CTRL_DIV2(div0)	((div0) << 1)

/* pll6 bit field */
#define CCM_PLL6_CTRL_P0_SHIFT		16
#define CCM_PLL6_CTRL_P0_MASK		(0x7 << CCM_PLL6_CTRL_P0_SHIFT)
#define CCM_PLL6_CTRL_N_SHIFT		8
#define CCM_PLL6_CTRL_N_MASK		(0xff << CCM_PLL6_CTRL_N_SHIFT)
#define CCM_PLL6_CTRL_DIV1_SHIFT	0
#define CCM_PLL6_CTRL_DIV1_MASK		(0x1 << CCM_PLL6_CTRL_DIV1_SHIFT)
#define CCM_PLL6_CTRL_DIV2_SHIFT	1
#define CCM_PLL6_CTRL_DIV2_MASK		(0x1 << CCM_PLL6_CTRL_DIV2_SHIFT)

/* cpu_axi bit field*/
#define CCM_CPU_AXI_MUX_MASK		(0x3 << 24)
#define CCM_CPU_AXI_MUX_OSC24M		(0x0 << 24)
#define CCM_CPU_AXI_MUX_PLL_CPUX	(0x3 << 24)
#define CCM_CPU_AXI_APB_MASK		0x300
#define CCM_CPU_AXI_AXI_MASK		0x3
#define CCM_CPU_AXI_DEFAULT_FACTORS	0x301

#ifdef CONFIG_MACH_SUN50I_H6				/* H6 */

#define CCM_PLL6_DEFAULT		0xa0006300
#define CCM_PSI_AHB1_AHB2_DEFAULT	0x03000102
#define CCM_AHB3_DEFAULT		0x03000002
#define CCM_APB1_DEFAULT		0x03000102

#elif CONFIG_MACH_SUN50I_H616				/* H616 */

#define CCM_PLL6_DEFAULT		0xa8003100
#define CCM_PSI_AHB1_AHB2_DEFAULT	0x03000002
#define CCM_AHB3_DEFAULT		0x03000002
#define CCM_APB1_DEFAULT		0x03000102

#elif CONFIG_MACH_SUN8I_R528				/* R528 */

#define CCM_PLL6_DEFAULT		0xe8216300
#define CCM_PSI_AHB1_AHB2_DEFAULT	0x03000002
#define CCM_APB1_DEFAULT		0x03000102

#elif CONFIG_MACH_SUN50I_A133				/* A133 */

#define CCM_PLL6_DEFAULT		0xb8003100
#define CCM_PSI_AHB1_AHB2_DEFAULT	0x03000002
#define CCM_AHB3_DEFAULT		0x03000002
#define CCM_APB1_DEFAULT		0x03000102

#elif CONFIG_MACH_SUN55I_A523				/* A523 */

#define CCM_PLL6_DEFAULT		0xe8216310	    /* 1200 MHz */
#define CCM_PSI_AHB1_AHB2_DEFAULT	0x03000002	    /* 200 MHz */
#define CCM_APB1_DEFAULT		0x03000005	    /* APB0 really */
#define CCM_APB2_DEFAULT		0x03000005	    /* APB1 really */
#endif


/* apb2 bit field */
#define APB2_CLK_SRC_OSC24M		(0x0 << 24)
#define APB2_CLK_SRC_OSC32K		(0x1 << 24)
#define APB2_CLK_SRC_PSI		(0x2 << 24)
#define APB2_CLK_SRC_PLL6		(0x3 << 24)
#define APB2_CLK_SRC_MASK		(0x3 << 24)
#define APB2_CLK_RATE_N_1		(0x0 << 8)
#define APB2_CLK_RATE_N_2		(0x1 << 8)
#define APB2_CLK_RATE_N_4		(0x2 << 8)
#define APB2_CLK_RATE_N_8		(0x3 << 8)
#define APB2_CLK_RATE_N_MASK		(3 << 8)
#define APB2_CLK_RATE_M(m)		(((m)-1) << 0)
#define APB2_CLK_RATE_M_MASK            (3 << 0)

/* MBUS clock bit field */
#define MBUS_ENABLE			BIT_U32(31)
#define MBUS_RESET			BIT_U32(30)
#define MBUS_UPDATE			BIT_U32(27)
#define MBUS_CLK_SRC_MASK		GENMASK(25, 24)
#define MBUS_CLK_SRC_OSCM24		(0 << 24)
#define MBUS_CLK_SRC_PLL6X2		(1 << 24)
#define MBUS_CLK_SRC_PLL5		(2 << 24)
#define MBUS_CLK_SRC_PLL6X4		(3 << 24)
#define MBUS_CLK_M(m)			(((m)-1) << 0)

/* Module gate/reset shift*/
#define RESET_SHIFT			(16)
#define GATE_SHIFT			(0)

/* DRAM clock bit field */
#define DRAM_CLK_ENABLE			BIT_U32(31)
#define DRAM_MOD_RESET			BIT_U32(30)
#define DRAM_CLK_UPDATE			BIT_U32(27)
#define DRAM_CLK_SRC_MASK		GENMASK(25, 24)
#define DRAM_CLK_SRC_PLL5		(0 << 24)
#define DRAM_CLK_M_MASK			(0x1f)
#define DRAM_CLK_M(m)			(((m)-1) << 0)

/* MMC clock bit field */
#define CCM_MMC_CTRL_M(x)		((x) - 1)
#define CCM_MMC_CTRL_N(x)		((x) << 8)
#define CCM_MMC_CTRL_OSCM24		(0x0 << 24)
#define CCM_MMC_CTRL_PLL6		(0x1 << 24)
#define CCM_MMC_CTRL_PLL_PERIPH2X2	(0x2 << 24)
#define CCM_MMC_CTRL_ENABLE		(0x1 << 31)
/* H6 doesn't have these delays */
#define CCM_MMC_CTRL_OCLK_DLY(a)	((void) (a), 0)
#define CCM_MMC_CTRL_SCLK_DLY(a)	((void) (a), 0)



#if CONFIG_MACH_SUN50I_H6
	#define SUNXI_DRAM_COM_BASE		0x04002000
	#define SUNXI_DRAM_CTL0_BASE		0x04003000
	#define SUNXI_DRAM_PHY0_BASE		0x04005000
#endif
//#define SUNXI_NFC_BASE			0x04011000
//#define SUNXI_MMC0_BASE			0x04020000
//#define SUNXI_MMC1_BASE			0x04021000
//#define SUNXI_MMC2_BASE			0x04022000
#if CONFIG_MACH_SUN50I_T507 || CONFIG_MACH_SUN50I_H616
	#define SUNXI_DRAM_COM_BASE			0x047FA000
	#define SUNXI_DRAM_CTL0_BASE		0x047FB000
	#define SUNXI_DRAM_PHY0_BASE		0x04800000
#endif

#if CONFIG_MACH_SUN50I_A133
	// https://github.com/u-boot/u-boot/blob/d33b21b7e261691e8d6613a24cc9b0ececba3b01/arch/arm/include/asm/arch-sunxi/cpu_sun50i_h6.h#L33
	#define SUNXI_DRAM_COM_BASE			0x04810000
	#define SUNXI_DRAM_CTL0_BASE		0x04820000
	#define SUNXI_DRAM_PHY0_BASE		0x04830000
#endif

#define DRAM_CLK_ENABLE			BIT_U32(31)

#define SUNXI_PRCM_BASE			0x07010000

#define SUNXI_CCM_BASE CCU_BASE

#define DIV_ROUND_UP(n, d)	(((n) + (d) - 1) / (d))
#define REPEAT_BYTE(x)		((~0ul / 0xff) * (x))

/*
 * Wait up to 1s for value to be set in given part of reg.
 */
static void mctl_await_completion(uint32_t *reg, uint32_t mask, uint32_t val)
{
	//unsigned long tmo = timer_get_us() + 1000000;

	while ((read32((uintptr_t) reg) & mask) != val) {
//		if (timer_get_us() > tmo)
//			panic("Timeout initialising DRAM\n");
	}
}

// SPDX-License-Identifier:	GPL-2.0+
/*
 * A133 dram controller register and constant defines
 *
 * (C) Copyright 2024  MasterR3C0RD <masterr3c0rd@epochal.quest>
 */

#ifndef _SUNXI_DRAM_SUN50I_A133_H
#define _SUNXI_DRAM_SUN50I_A133_H

//#include <linux/bitops.h>

enum sunxi_dram_type {
	SUNXI_DRAM_TYPE_DDR3 = 3,
	SUNXI_DRAM_TYPE_DDR4,
	SUNXI_DRAM_TYPE_LPDDR3 = 7,
	SUNXI_DRAM_TYPE_LPDDR4
};

static inline int ns_to_t(int nanoseconds)
{
	const unsigned int ctrl_freq = CONFIG_DRAM_CLK / 2;

	return DIV_ROUND_UP(ctrl_freq * nanoseconds, 1000);
}

/* MBUS part is largely the same as in H6, except for one special register */
#define MCTL_COM_UNK_008	0x008
/* NOTE: This register has the same importance as mctl_ctl->clken in H616 */
#define MCTL_COM_MAER0		0x020

/*
 * Controller registers seems to be the same or at least very similar
 * to those in H6.
 */
struct sunxi_mctl_ctl_reg {
	uint32_t mstr; 			/* 0x000 */
	uint32_t statr; 			/* 0x004 unused */
	uint32_t mstr1; 			/* 0x008 unused */
	uint32_t clken; 			/* 0x00c */
	uint32_t mrctrl0; 			/* 0x010 unused */
	uint32_t mrctrl1; 			/* 0x014 unused */
	uint32_t mrstatr; 			/* 0x018 unused */
	uint32_t mrctrl2; 			/* 0x01c unused */
	uint32_t derateen; 			/* 0x020 unused */
	uint32_t derateint; 			/* 0x024 unused */
	uint8_t reserved_0x028[8]; 		/* 0x028 */
	uint32_t pwrctl; 			/* 0x030 unused */
	uint32_t pwrtmg; 			/* 0x034 unused */
	uint32_t hwlpctl; 			/* 0x038 unused */
	uint8_t reserved_0x03c[20];		/* 0x03c */
	uint32_t rfshctl0;			/* 0x050 unused */
	uint32_t rfshctl1;			/* 0x054 unused */
	uint8_t reserved_0x058[8];		/* 0x05c */
	uint32_t rfshctl3;			/* 0x060 */
	uint32_t rfshtmg;			/* 0x064 */
	uint8_t reserved_0x068[104];		/* 0x068 */
	uint32_t init[8];			/* 0x0d0 */
	uint32_t dimmctl;			/* 0x0f0 unused */
	uint32_t rankctl;			/* 0x0f4 */
	uint8_t reserved_0x0f8[8];		/* 0x0f8 */
	uint32_t dramtmg[17];		/* 0x100 */
	uint8_t reserved_0x144[60];		/* 0x144 */
	uint32_t zqctl[3];			/* 0x180 */
	uint32_t zqstat;			/* 0x18c unused */
	uint32_t dfitmg0;			/* 0x190 */
	uint32_t dfitmg1;			/* 0x194 */
	uint32_t dfilpcfg[2];		/* 0x198 unused */
	uint32_t dfiupd[3];			/* 0x1a0 */
	uint32_t reserved_0x1ac;		/* 0x1ac */
	uint32_t dfimisc;			/* 0x1b0 */
	uint32_t dfitmg2;			/* 0x1b4 unused */
	uint32_t dfitmg3;			/* 0x1b8 unused */
	uint32_t dfistat;			/* 0x1bc */
	uint32_t dbictl;			/* 0x1c0 */
	uint8_t reserved_0x1c4[60];		/* 0x1c4 */
	uint32_t addrmap[12];		/* 0x200 */
	uint8_t reserved_0x230[16];		/* 0x230 */
	uint32_t odtcfg;			/* 0x240 */
	uint32_t odtmap;			/* 0x244 */
	uint8_t reserved_0x248[8];		/* 0x248 */
	uint32_t sched[2];			/* 0x250 */
	uint8_t reserved_0x258[180]; 	/* 0x258 */
	uint32_t dbgcmd;			/* 0x30c unused */
	uint32_t dbgstat;			/* 0x310 unused */
	uint8_t reserved_0x314[12];		/* 0x314 */
	uint32_t swctl;			/* 0x320 */
	uint32_t swstat;			/* 0x324 */
	uint8_t reserved_0x328[7768];	/* 0x328 */
	uint32_t unk_0x2180;			/* 0x2180 */
	uint8_t reserved_0x2184[188];	/* 0x2184 */
	uint32_t unk_0x2240;			/* 0x2240 */
	uint8_t reserved_0x2244[3900];	/* 0x2244 */
	uint32_t unk_0x3180;			/* 0x3180 */
	uint8_t reserved_0x3184[188];	/* 0x3184 */
	uint32_t unk_0x3240;			/* 0x3240 */
	uint8_t reserved_0x3244[3900];	/* 0x3244 */
	uint32_t unk_0x4180;			/* 0x4180 */
	uint8_t reserved_0x4184[188];	/* 0x4184 */
	uint32_t unk_0x4240;			/* 0x4240 */
};

//check_member(sunxi_mctl_ctl_reg, swstat, 0x324);
//check_member(sunxi_mctl_ctl_reg, unk_0x4240, 0x4240);

#define MSTR_DEVICETYPE_DDR3	BIT_U32(0)
#define MSTR_DEVICETYPE_LPDDR2	BIT_U32(2)
#define MSTR_DEVICETYPE_LPDDR3	BIT_U32(3)
#define MSTR_DEVICETYPE_DDR4	BIT_U32(4)
#define MSTR_DEVICETYPE_LPDDR4	BIT_U32(5)
#define MSTR_DEVICETYPE_MASK	GENMASK(5, 0)
#define MSTR_GEARDOWNMODE	BIT_U32(0)		/* Same as MSTR_DEVICETYPE_DDR3, only used for DDR4 */
#define MSTR_2TMODE		BIT_U32(10)
#define MSTR_BUSWIDTH_FULL	(0 << 12)
#define MSTR_BUSWIDTH_HALF	(1 << 12)
#define MSTR_ACTIVE_RANKS(x)	(((x == 1) ? 3 : 1) << 24)
#define MSTR_BURST_LENGTH(x)	(((x) >> 1) << 16)
#define MSTR_DEVICECONFIG_X32	(3 << 30)

#define TPR10_CA_BIT_DELAY	BIT_U32(16)
#define TPR10_DX_BIT_DELAY0	BIT_U32(17)
#define TPR10_DX_BIT_DELAY1	BIT_U32(18)
#define TPR10_WRITE_LEVELING	BIT_U32(20)
#define TPR10_READ_CALIBRATION	BIT_U32(21)
#define TPR10_READ_TRAINING	BIT_U32(22)
#define TPR10_WRITE_TRAINING	BIT_U32(23)

/* MRCTRL constants */
#define MRCTRL0_MR_RANKS_ALL	(3 << 4)
#define MRCTRL0_MR_ADDR(x)	(x << 12)
#define MRCTRL0_MR_WR		BIT_U32(31)

#define MRCTRL1_MR_ADDR(x)	(x << 8)
#define MRCTRL1_MR_DATA(x)	(x)

/* ADDRMAP constants */
#define ADDRMAP_DISABLED_3F_B(b)	(0x3f + b)
#define ADDRMAP_DISABLED_1F_B(b)	(0x1f + b)
#define ADDRMAP_DISABLED_0F_B(b)	(0x0f + b)

#define _ADDRMAP_VALUE(a,x,b)		(((a) - b) << (x * 8))

/*
 * Bx = internal base
 * The selected HIF address bit for each address bit is determined
 * by adding the internal base to the value of each field
 * */

#define ADDRMAP0_CS0_B6(v)	_ADDRMAP_VALUE(v, 0, 6)

#define ADDRMAP1_BANK0_B2(v) 	_ADDRMAP_VALUE(v, 0, 2)
#define ADDRMAP1_BANK1_B3(v)	_ADDRMAP_VALUE(v, 1, 3)
#define ADDRMAP1_BANK2_B4(v)	_ADDRMAP_VALUE(v, 2, 4)

#define ADDRMAP2_COL2_B2(v)	_ADDRMAP_VALUE(v, 0, 2)
#define ADDRMAP2_COL3_B3(v)	_ADDRMAP_VALUE(v, 1, 3)
#define ADDRMAP2_COL4_B4(v)	_ADDRMAP_VALUE(v, 2, 4)
#define ADDRMAP2_COL5_B5(v)	_ADDRMAP_VALUE(v, 3, 5)

#define ADDRMAP3_COL6_B6(v)	_ADDRMAP_VALUE(v, 0, 6)
#define ADDRMAP3_COL7_B7(v)	_ADDRMAP_VALUE(v, 1, 7)
#define ADDRMAP3_COL8_B8(v)	_ADDRMAP_VALUE(v, 2, 8)
#define ADDRMAP3_COL9_B9(v)	_ADDRMAP_VALUE(v, 3, 9)

#define ADDRMAP4_COL10_B10(v)	_ADDRMAP_VALUE(v, 0, 10)
#define ADDRMAP4_COL11_B11(v)	_ADDRMAP_VALUE(v, 1, 11)

#define ADDRMAP5_ROW0_B6(v)	_ADDRMAP_VALUE(v, 0, 6)
#define ADDRMAP5_ROW1_B7(v)	_ADDRMAP_VALUE(v, 1, 7)
#define ADDRMAP5_ROW2_10_B8(v)	_ADDRMAP_VALUE(v, 2, 8)
#define ADDRMAP5_ROW11_B17(v)	_ADDRMAP_VALUE(v, 3, 17)

#define ADDRMAP6_ROW12_B18(v)	_ADDRMAP_VALUE(v, 0, 18)
#define ADDRMAP6_ROW13_B19(v)	_ADDRMAP_VALUE(v, 1, 19)
#define ADDRMAP6_ROW14_B20(v)	_ADDRMAP_VALUE(v, 2, 20)
#define ADDRMAP6_ROW15_B21(v)	_ADDRMAP_VALUE(v, 3, 21)

#define ADDRMAP7_ROW16_B22(v)	_ADDRMAP_VALUE(v, 0, 22)
#define ADDRMAP7_ROW17_B23(v)	_ADDRMAP_VALUE(v, 1, 23)

#define ADDRMAP8_BG0_B2(v)	_ADDRMAP_VALUE(v, 0, 2)
#define ADDRMAP8_BG1_B3(v)	_ADDRMAP_VALUE(v, 1, 3)

/* These are only used if ADDRMAP5_ROW_BITS_2_10 = ADDRMAP_DISABLED_0F */
#define ADDRMAP9_ROW2_B8(v)	_ADDRMAP_VALUE(v, 0, 8)
#define ADDRMAP9_ROW3_B9(v)	_ADDRMAP_VALUE(v, 1, 9)
#define ADDRMAP9_ROW4_B10(v)	_ADDRMAP_VALUE(v, 2, 10)
#define ADDRMAP9_ROW5_B11(v)	_ADDRMAP_VALUE(v, 3, 11)

#define ADDRMAP10_ROW6_B12(v)	_ADDRMAP_VALUE(v, 0, 12)
#define ADDRMAP10_ROW7_B13(v)	_ADDRMAP_VALUE(v, 1, 13)
#define ADDRMAP10_ROW8_B14(v)	_ADDRMAP_VALUE(v, 2, 14)
#define ADDRMAP10_ROW9_B15(v)	_ADDRMAP_VALUE(v, 3, 15)

#define ADDRMAP11_ROW10_B16(v)	_ADDRMAP_VALUE(v, 0, 16)

struct dram_para {
	uint32_t clk;
	enum sunxi_dram_type type;
	uint32_t dx_odt;
	uint32_t dx_dri;
	uint32_t ca_dri;
	uint32_t para0;
	uint32_t mr11;
	uint32_t mr12;
	uint32_t mr13;
	uint32_t mr14;
	uint32_t tpr1;
	uint32_t tpr2;
	uint32_t tpr3;
	uint32_t tpr6;
	uint32_t tpr10;
	uint32_t tpr11;
	uint32_t tpr12;
	uint32_t tpr13;
	uint32_t tpr14;
};

void mctl_set_timing_params(const struct dram_para *para);

struct dram_config {
	uint8_t cols;		/* Column bits */
	uint8_t rows;		/* Row bits */
	uint8_t ranks;		/* Rank bits (different from H616!) */
	uint8_t banks;		/* Bank bits */
	uint8_t bankgrps;		/* Bank group bits */
	uint8_t bus_full_width;	/* 1 = x32, 0 = x16 */
};

#endif /* _SUNXI_DRAM_SUN50I_A133_H */


#define CCU_PRCM_I2C_GATE_RESET		0x19c
#define CCU_PRCM_PLL_LDO_CFG		0x244
#define CCU_PRCM_SYS_PWROFF_GATING	0x250
#define CCU_PRCM_RES_CAL_CTRL		0x310
#define CCU_PRCM_OHMS240		0x318

#define PRCM_TWI_GATE		(1 << 0)
#define PRCM_TWI_RESET		(1 << 16)



//#define DEBUG

//#include <asm/arch/clock.h>
//#include <asm/arch/cpu.h>
//#include <asm/arch/dram.h>
//#include <asm/arch/prcm.h>
//#include <asm/io.h>
//#include <init.h>
//#include <linux/bitops.h>
//#include <linux/delay.h>
//#include <log.h>

#ifdef CONFIG_DRAM_SUNXI_PHY_ADDR_MAP_1
static const uint8_t phy_init[] = {
#ifdef CONFIG_SUNXI_DRAM_DDR3
	0x0c, 0x08, 0x19, 0x18, 0x10, 0x06, 0x0a, 0x03, 0x0e,
	0x00, 0x0b, 0x05, 0x09, 0x1a, 0x04, 0x13, 0x16, 0x11,
	0x01, 0x15, 0x0d, 0x07, 0x12, 0x17, 0x14, 0x02, 0x0f
#elif CONFIG_SUNXI_DRAM_DDR4
	0x19, 0x1a, 0x04, 0x12, 0x09, 0x06, 0x08, 0x0a, 0x16,
	0x17, 0x18, 0x0f, 0x0c, 0x13, 0x02, 0x05, 0x01, 0x11,
	0x0e, 0x00, 0x0b, 0x07, 0x03, 0x14, 0x15, 0x0d, 0x10
#elif CONFIG_SUNXI_DRAM_LPDDR3
	0x08, 0x03, 0x02, 0x00, 0x18, 0x19, 0x09, 0x01, 0x06,
	0x17, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
	0x12, 0x13, 0x14, 0x15, 0x16, 0x04, 0x05, 0x07, 0x1a
#elif CONFIG_SUNXI_DRAM_LPDDR4
	0x01, 0x05, 0x02, 0x00, 0x19, 0x03, 0x06, 0x07, 0x08,
	0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
	0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x04, 0x1a
#endif
};
#else
static const uint8_t phy_init[] = {
#ifdef CONFIG_SUNXI_DRAM_DDR3
	0x03, 0x19, 0x18, 0x02, 0x10, 0x15, 0x16, 0x07, 0x06,
	0x0e, 0x05, 0x08, 0x0d, 0x04, 0x17, 0x1a, 0x13, 0x11,
	0x12, 0x14, 0x00, 0x01, 0x0c, 0x0a, 0x09, 0x0b, 0x0f
#elif CONFIG_SUNXI_DRAM_DDR4
	0x13, 0x17, 0x0e, 0x01, 0x06, 0x12, 0x14, 0x07, 0x09,
	0x02, 0x0f, 0x00, 0x0d, 0x05, 0x16, 0x0c, 0x0a, 0x11,
	0x04, 0x03, 0x18, 0x15, 0x08, 0x10, 0x0b, 0x19, 0x1a
#elif CONFIG_SUNXI_DRAM_LPDDR3
	0x05, 0x06, 0x17, 0x02, 0x19, 0x18, 0x04, 0x07, 0x03,
	0x01, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
	0x12, 0x13, 0x14, 0x15, 0x16, 0x08, 0x09, 0x00, 0x1a
#elif CONFIG_SUNXI_DRAM_LPDDR4
	0x01, 0x03, 0x02, 0x19, 0x17, 0x00, 0x06, 0x07, 0x08,
	0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
	0x12, 0x13, 0x14, 0x15, 0x16, 0x04, 0x18, 0x05, 0x1a
#endif
};
#endif

static void mctl_clk_init(uint32_t clk)
{
	void * const ccm = (void *)SUNXI_CCM_BASE;

	/* Place all DRAM blocks into reset */
	clrbits_le32(ccm + CCU_H6_MBUS_CFG, MBUS_ENABLE);
	clrbits_le32(ccm + CCU_H6_MBUS_CFG, MBUS_RESET);
	clrbits_le32(ccm + CCU_H6_DRAM_GATE_RESET, BIT_U32(GATE_SHIFT));
	clrbits_le32(ccm + CCU_H6_DRAM_GATE_RESET, BIT_U32(RESET_SHIFT));
	clrbits_le32(ccm + CCU_H6_PLL5_CFG, CCM_PLL_CTRL_EN);
	clrbits_le32(ccm + CCU_H6_DRAM_CLK_CFG, DRAM_MOD_RESET);
	udelay(5);

	/* Set up PLL5 clock, used for DRAM */
	clrsetbits_le32(ccm + CCU_H6_PLL5_CFG, 0xff03,
			CCM_PLL5_CTRL_N((clk * 2) / 24) | CCM_PLL_CTRL_EN);
	setbits_le32(ccm + CCU_H6_PLL5_CFG, BIT_U32(24));
	clrsetbits_le32(ccm + CCU_H6_PLL5_CFG, 0x3,
			CCM_PLL_LOCK_EN | CCM_PLL_CTRL_EN | CCM_PLL_LDO_EN);
	clrbits_le32(ccm + CCU_H6_PLL5_CFG, 0x3 | CCM_PLL_LDO_EN);
	mctl_await_completion(ccm + CCU_H6_PLL5_CFG,
			      CCM_PLL_LOCK, CCM_PLL_LOCK);

	/* Enable DRAM clock and gate*/
	clrbits_le32(ccm + CCU_H6_DRAM_CLK_CFG, BIT_U32(24) | BIT_U32(25));
	clrsetbits_le32(ccm + CCU_H6_DRAM_CLK_CFG, 0x1f, BIT_U32(1) | BIT_U32(0));
	setbits_le32(ccm + CCU_H6_DRAM_CLK_CFG, DRAM_CLK_UPDATE);
	setbits_le32(ccm + CCU_H6_DRAM_GATE_RESET, BIT_U32(RESET_SHIFT));
	setbits_le32(ccm + CCU_H6_DRAM_GATE_RESET, BIT_U32(GATE_SHIFT));

	/* Re-enable MBUS and reset the DRAM module */
	setbits_le32(ccm + CCU_H6_MBUS_CFG, MBUS_RESET);
	setbits_le32(ccm + CCU_H6_MBUS_CFG, MBUS_ENABLE);
	setbits_le32(ccm + CCU_H6_DRAM_CLK_CFG, DRAM_MOD_RESET);
	udelay(5);
}

static void mctl_set_odtmap(const struct dram_para *para,
			    const struct dram_config *config)
{
	struct sunxi_mctl_ctl_reg *mctl_ctl =
		(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;

	uint32_t val, temp1, temp2;

	/* Set ODT/rank mappings*/
	if (config->bus_full_width)
		writel_relaxed(0x0201, &mctl_ctl->odtmap);
	else
		writel_relaxed(0x0303, &mctl_ctl->odtmap);

	switch (para->type) {
	default:
	case SUNXI_DRAM_TYPE_DDR3:
		val = 0x06000400;
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		/* TODO: What's the purpose of these values? */
		temp1 = para->clk * 7 / 2000;
		if (para->clk < 400)
			temp2 = 0x3;
		else
			temp2 = 0x4;

		val = 0x400 | (temp2 - temp1) << 16 | temp1 << 24;
		break;
	case SUNXI_DRAM_TYPE_DDR4:
		/* MR4: CS to CMD / ADDR Latency   and  write preamble */
		val = 0x400 | (0x000 << 10 & 0x70000) |
		      (((0x0000 >> 12) & 1) + 6) << 24;
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		val = 0x4000400;
		break;
	}

	writel_relaxed(val, &mctl_ctl->odtcfg);
	/* Documented as ODTCFG_SHADOW */
	writel_relaxed(val, &mctl_ctl->unk_0x2240);
	/* Offset's interesting; additional undocumented shadows? */
	writel_relaxed(val, &mctl_ctl->unk_0x3240);
	writel_relaxed(val, &mctl_ctl->unk_0x4240);
}

/*
 * This function produces address mapping parameters, used internally by the
 * controller to map address lines to HIF addresses. HIF addresses are word
 * addresses, not byte addresses;
 * In other words, DDR address 0x400 maps to HIF address 0x100.
 *
 * This implementation sets up a reasonable mapping where HIF address
 * ordering (LSB->MSB) is as such:
 * - Bank Groups
 * - Columns
 * - Banks
 * - Rows
 * - Ranks
 *
 * TODO: Handle 1.5GB + 3GB configurations. Info about these is stored in
 * upper bits of TPR13 after autoscan in boot0, and then some extra logic
 * happens in the address mapping
 */
#define INITIAL_HIF_OFFSET 3

static void mctl_set_addrmap(const struct dram_config *config)
{
	struct sunxi_mctl_ctl_reg *mctl_ctl =
		(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;

	uint8_t bankgrp_bits = config->bankgrps;
	uint8_t col_bits = config->cols;
	uint8_t bank_bits = config->banks;
	uint8_t row_bits = config->rows;
	uint8_t rank_bits = config->ranks;

	unsigned int i, hif_offset, hif_bits[6];

	/*
	 * When the bus is half width, we need to adjust address mapping,
	 * as COL[0] will be reallocated as part of the byte address,
	 * offsetting the column address mapping values by 1
	 */
	if (!config->bus_full_width)
		col_bits--;

	/* Match boot0's DRAM requirements */
	if (bankgrp_bits > 2)
		panic("invalid dram configuration (bankgrps_bits = %d)",
		      bankgrp_bits);
	if (col_bits < 8 || col_bits > 12)
		panic("invalid dram configuration (col_bits = %d)", col_bits);

	if (bank_bits < 2 || bank_bits > 3)
		panic("invalid dram configuration (bank_bits = %d)", bank_bits);

	if (row_bits < 14 || row_bits > 18)
		panic("invalid dram configuration (row_bits = %d)", row_bits);

	if (rank_bits > 1)
		panic("invalid dram configuration (rank_bits = %d)", rank_bits);

	/*
	 * Col[0:1] + HIF[0:1] (hardwired), Col[2] = HIF[2] (required)
	 * Thus, we start allocating from HIF[3] onwards
	 */
	hif_offset = INITIAL_HIF_OFFSET;

	/* BG[bankgrp_bits:0] = HIF[3 + bankgrp_bits:3]*/
	switch (bankgrp_bits) {
	case 0:
		writel_relaxed(ADDRMAP8_BG0_B2(ADDRMAP_DISABLED_1F_B(2)) |
			       ADDRMAP8_BG1_B3(ADDRMAP_DISABLED_1F_B(3)),
			&mctl_ctl->addrmap[8]);
		break;
	case 1:
		writel_relaxed(ADDRMAP8_BG0_B2(hif_offset) |
			       ADDRMAP8_BG1_B3(ADDRMAP_DISABLED_1F_B(3)),
			&mctl_ctl->addrmap[8]);
		break;
	case 2:
		writel_relaxed(ADDRMAP8_BG0_B2(hif_offset) |
			       ADDRMAP8_BG1_B3(hif_offset + 1),
			       &mctl_ctl->addrmap[8]);
		break;
	default:
		panic("invalid dram configuration (bankgrp_bits = %d)",
		      bankgrp_bits);
	}

	hif_offset += bankgrp_bits;

	/* Col[2] = HIF[2], Col[5:3] = HIF[offset + 2:offset] */
	writel_relaxed(ADDRMAP2_COL2_B2(2) | ADDRMAP2_COL3_B3(hif_offset) |
		       ADDRMAP2_COL4_B4(hif_offset + 1) |
		       ADDRMAP2_COL5_B5(hif_offset + 2),
		       &mctl_ctl->addrmap[2]);

	/* Col[col_bits:6] = HIF[col_bits + offset - 3:offset - 3] */
	for (i = 6; i < 12; i++) {
		if (i < col_bits)
			hif_bits[i - 6] = hif_offset + (i - INITIAL_HIF_OFFSET);
		else
			hif_bits[i - 6] = ADDRMAP_DISABLED_1F_B(i);
	}

	writel_relaxed(ADDRMAP3_COL6_B6(hif_bits[0]) |
		       ADDRMAP3_COL7_B7(hif_bits[1]) |
		       ADDRMAP3_COL8_B8(hif_bits[2]) |
		       ADDRMAP3_COL9_B9(hif_bits[3]),
		       &mctl_ctl->addrmap[3]);

	writel_relaxed(ADDRMAP4_COL10_B10(hif_bits[4]) |
		       ADDRMAP4_COL11_B11(hif_bits[5]),
		       &mctl_ctl->addrmap[4]);

	hif_offset = bankgrp_bits + col_bits;

	/* Bank[bank_bits:0] = HIF[bank_bits + offset:offset] */
	if (bank_bits == 3)
		writel_relaxed(ADDRMAP1_BANK0_B2(hif_offset) |
			       ADDRMAP1_BANK1_B3(hif_offset + 1) |
			       ADDRMAP1_BANK2_B4(hif_offset + 2),
			       &mctl_ctl->addrmap[1]);
	else
		writel_relaxed(ADDRMAP1_BANK0_B2(hif_offset) |
			       ADDRMAP1_BANK1_B3(hif_offset + 1) |
			       ADDRMAP1_BANK2_B4(ADDRMAP_DISABLED_1F_B(4)),
			       &mctl_ctl->addrmap[1]);

	hif_offset += bank_bits;

	/* Row[11:0] = HIF[11 + offset:offset] */
	writel_relaxed(ADDRMAP5_ROW0_B6(hif_offset) |
		       ADDRMAP5_ROW1_B7(hif_offset + 1) |
		       ADDRMAP5_ROW2_10_B8(hif_offset + 2) |
		       ADDRMAP5_ROW11_B17(hif_offset + 11),
		       &mctl_ctl->addrmap[5]);

	/*
	 * There's some complexity here because of a special case
	 * in boot0 code that appears to work around a hardware bug.
	 * For (col_bits, row_bits, rank_bits) = (10, 16, 1), we have to
	 * place CS[0] in the position we would normally place ROW[14],
	 * and shift ROW[14] and ROW[15] over by one. Using the bit following
	 * ROW[15], as would be standard here, seems to cause nonsensical
	 * aliasing patterns.
	 *
	 * Aside from this case, mapping is simple:
	 * Row[row_bits:12] = HIF[offset + row_bits:offset + 12]
	 */
	for (i = 12; i < 18; i++) {
		if (i >= row_bits)
			hif_bits[i - 12] = ADDRMAP_DISABLED_0F_B(6 + i);
		else if (rank_bits != 1 || col_bits != 10 || row_bits != 16 ||
			 i < 14)
			hif_bits[i - 12] = hif_offset + i;
		else
			hif_bits[i - 12] = hif_offset + i + 1;
	}

	writel_relaxed(ADDRMAP6_ROW12_B18(hif_bits[0]) |
		       ADDRMAP6_ROW13_B19(hif_bits[1]) |
		       ADDRMAP6_ROW14_B20(hif_bits[2]) |
		       ADDRMAP6_ROW15_B21(hif_bits[3]),
		       &mctl_ctl->addrmap[6]);

	writel_relaxed(ADDRMAP7_ROW16_B22(hif_bits[4]) |
		       ADDRMAP7_ROW17_B23(hif_bits[5]),
		       &mctl_ctl->addrmap[7]);

	hif_offset += row_bits;

	/*
	 * Ranks
	 * Most cases: CS[0] = HIF[offset]
	 * Special case (see above): CS[0] = HIF[offset - 2]
	 */
	if (rank_bits == 0)
		writel_relaxed(ADDRMAP0_CS0_B6(ADDRMAP_DISABLED_1F_B(6)),
			       &mctl_ctl->addrmap[0]);
	else if (col_bits == 10 && row_bits == 16)
		writel_relaxed(ADDRMAP0_CS0_B6(hif_offset - 2),
			       &mctl_ctl->addrmap[0]);
	else
		writel_relaxed(ADDRMAP0_CS0_B6(hif_offset),
			       &mctl_ctl->addrmap[0]);
}

static void mctl_com_init(const struct dram_para *para,
			  const struct dram_config *config)
{
	void *const mctl_com = (void *)SUNXI_DRAM_COM_BASE;
	struct sunxi_mctl_ctl_reg *mctl_ctl =
		(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;

	/* Might control power/reset of DDR-related blocks */
	clrsetbits_le32(mctl_com + MCTL_COM_UNK_008, BIT_U32(24), BIT_U32(25) | BIT_U32(9));

	/* Unlock mctl_ctl registers */
	setbits_le32(mctl_com + MCTL_COM_MAER0, BIT_U32(15));

	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
		setbits_le32(0x03102ea8, BIT_U32(0));

	clrsetbits_le32(&mctl_ctl->sched[0], 0xff << 8, 0x30 << 8);
	if (!(para->tpr13 & BIT_U32(28)))
		clrsetbits_le32(&mctl_ctl->sched[0], 0xf, BIT_U32(0));

	writel_relaxed(0, &mctl_ctl->hwlpctl);

	/* Master settings */
	uint32_t mstr_value = MSTR_DEVICECONFIG_X32 |
			 MSTR_ACTIVE_RANKS(config->ranks);

	if (config->bus_full_width)
		mstr_value |= MSTR_BUSWIDTH_FULL;
	else
		mstr_value |= MSTR_BUSWIDTH_HALF;

	/*
	 * Geardown and 2T mode are always enabled here, but is controlled by a flag in boot0;
	 * it has not been a problem so far, but may be suspect if a particular board isn't booting.
	 */
	switch (para->type) {
	case SUNXI_DRAM_TYPE_DDR3:
		mstr_value |= MSTR_DEVICETYPE_DDR3 | MSTR_BURST_LENGTH(8) |
			      MSTR_2TMODE;
		break;
	case SUNXI_DRAM_TYPE_DDR4:
		mstr_value |= MSTR_DEVICETYPE_DDR4 | MSTR_BURST_LENGTH(8) |
			      MSTR_GEARDOWNMODE | MSTR_2TMODE;
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		mstr_value |= MSTR_DEVICETYPE_LPDDR3 | MSTR_BURST_LENGTH(8);
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		mstr_value |= MSTR_DEVICETYPE_LPDDR4 | MSTR_BURST_LENGTH(16);
		break;
	}

	writel_relaxed(mstr_value, &mctl_ctl->mstr);

	mctl_set_odtmap(para, config);
	mctl_set_addrmap(config);
	mctl_set_timing_params(para);

	__DSB();
	writel(0, &mctl_ctl->pwrctl);

	/* Disable automatic controller updates + automatic controller update requests */
	setbits_le32(&mctl_ctl->dfiupd[0], BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->zqctl[0], BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x2180, BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x3180, BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x4180, BIT_U32(31) | BIT_U32(30));

	/*
	 * Data bus inversion
	 * Controlled by a flag in boot0, enabled by default here.
	 */
	if (para->type == SUNXI_DRAM_TYPE_DDR4 ||
	    para->type == SUNXI_DRAM_TYPE_LPDDR4)
		setbits_le32(&mctl_ctl->dbictl, BIT_U32(2));
}

static void mctl_drive_odt_config(const struct dram_para *para)
{
	uint32_t val;
	uintptr_t base;	// was: ulong
	uint32_t i;

	/* DX drive */
	for (i = 0; i < 4; i++) {
		base = SUNXI_DRAM_PHY0_BASE + 0x388 + 0x40 * i;
		val = (para->dx_dri >> (i * 8)) & 0x1f;

		writel_relaxed(val, base);
		if (para->type == SUNXI_DRAM_TYPE_LPDDR4) {
			if (para->tpr3 & 0x1f1f1f1f)
				val = (para->tpr3 >> (i * 8)) & 0x1f;
			else
				val = 4;
		}
		writel_relaxed(val, base + 4);
	}

	/* CA drive */
	for (i = 0; i < 2; i++) {
		base = SUNXI_DRAM_PHY0_BASE + 0x340 + 0x8 * i;
		val = (para->ca_dri >> (i * 8)) & 0x1f;

		writel_relaxed(val, base);
		writel_relaxed(val, base + 4);
	}

	/* DX ODT */
	for (i = 0; i < 4; i++) {
		base = SUNXI_DRAM_PHY0_BASE + 0x380 + 0x40 * i;
		val = (para->dx_odt >> (i * 8)) & 0x1f;

		if (para->type == SUNXI_DRAM_TYPE_DDR4 ||
		    para->type == SUNXI_DRAM_TYPE_LPDDR3)
			writel_relaxed(0, base);
		else
			writel_relaxed(val, base);

		if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
			writel_relaxed(0, base + 4);
		else
			writel_relaxed(val, base + 4);
	}
	__DSB();
}

static void mctl_phy_ca_bit_delay_compensation(const struct dram_para *para)
{
	uint32_t val, i;
	uint32_t *ptr;

	if (para->tpr10 & BIT_U32(31)) {
		val = para->tpr2;
	} else {
		val = ((para->tpr10 << 1) & 0x1e) |
		      ((para->tpr10 << 5) & 0x1e00) |
		      ((para->tpr10 << 9) & 0x1e0000) |
		      ((para->tpr10 << 13) & 0x1e000000);

		if (para->tpr10 >> 29 != 0)
			val <<= 1;
	}

	ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x780);
	for (i = 0; i < 32; i++)
		writel_relaxed((val >> 8) & 0x3f, &ptr[i]);

	writel_relaxed(val & 0x3f, SUNXI_DRAM_PHY0_BASE + 0x7dc);
	writel_relaxed(val & 0x3f, SUNXI_DRAM_PHY0_BASE + 0x7e0);

	switch (para->type) {
	case SUNXI_DRAM_TYPE_DDR3:
		writel_relaxed((val >> 16) & 0x3f,
			       SUNXI_DRAM_PHY0_BASE + 0x7b8);
		writel_relaxed((val >> 24) & 0x3f,
			       SUNXI_DRAM_PHY0_BASE + 0x784);
		break;
	case SUNXI_DRAM_TYPE_DDR4:
		writel_relaxed((val >> 16) & 0x3f,
			       SUNXI_DRAM_PHY0_BASE + 0x784);
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		writel_relaxed((val >> 16) & 0x3f,
			       SUNXI_DRAM_PHY0_BASE + 0x788);
		writel_relaxed((val >> 24) & 0x3f,
			       SUNXI_DRAM_PHY0_BASE + 0x790);
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		writel_relaxed((val >> 16) & 0x3f,
			       SUNXI_DRAM_PHY0_BASE + 0x790);
		writel_relaxed((val >> 24) & 0x3f,
			       SUNXI_DRAM_PHY0_BASE + 0x78c);
		break;
	}

	__DSB();
}

static void mctl_phy_init(const struct dram_para *para,
			  const struct dram_config *config)
{
	struct sunxi_mctl_ctl_reg *mctl_ctl =
		(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;
	void *const prcm = (void *)SUNXI_PRCM_BASE;
	void *const mctl_com = (void *)SUNXI_DRAM_COM_BASE;

	uint32_t val, val2, i;
	uint32_t *ptr;

	/* Disable auto refresh. */
	setbits_le32(&mctl_ctl->rfshctl3, BIT_U32(0));

	/* Set "phy_dbi_mode" to mark the DFI as implementing DBI functionality */
	writel_relaxed(0, &mctl_ctl->pwrctl);
	clrbits_le32(&mctl_ctl->dfimisc, 1);
	writel_relaxed(0x20, &mctl_ctl->pwrctl);

	/* PHY cold reset */
	clrsetbits_le32(mctl_com + MCTL_COM_UNK_008, BIT_U32(24), BIT_U32(9));
	udelay(1);
	setbits_le32(mctl_com + MCTL_COM_UNK_008, BIT_U32(24));

	/* Not sure what this gates the power of. */
	clrbits_le32(prcm + CCU_PRCM_SYS_PWROFF_GATING, BIT_U32(4));

	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x4, BIT_U32(7));

	/* Note: Similar enumeration of values is used during read training */
	if (config->bus_full_width)
		val = 0xf;
	else
		val = 0x3;

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x3c, 0xf, val);

	switch (para->type) {
	default:
	case SUNXI_DRAM_TYPE_DDR3:
		val = 13;
		val2 = 9;
		break;
	case SUNXI_DRAM_TYPE_DDR4:
		val = 13;
		val2 = 10;
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		val = 14;
		val2 = 8;
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		if (para->tpr13 & BIT_U32(28))
			val = 22;
		else
			val = 20;

		val2 = 10;
		break;
	}

	writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x14);
	writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x35c);
	writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x368);
	writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x374);
	writel_relaxed(0, SUNXI_DRAM_PHY0_BASE + 0x18);
	writel_relaxed(0, SUNXI_DRAM_PHY0_BASE + 0x360);
	writel_relaxed(0, SUNXI_DRAM_PHY0_BASE + 0x36c);
	writel_relaxed(0, SUNXI_DRAM_PHY0_BASE + 0x378);
	writel_relaxed(val2, SUNXI_DRAM_PHY0_BASE + 0x1c);
	writel_relaxed(val2, SUNXI_DRAM_PHY0_BASE + 0x364);
	writel_relaxed(val2, SUNXI_DRAM_PHY0_BASE + 0x370);
	writel_relaxed(val2, SUNXI_DRAM_PHY0_BASE + 0x37c);

	/* Set up SDQ swizzle */
	ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xc0);
	for (i = 0; i < ARRAY_SIZE(phy_init); i++)
		writel_relaxed(phy_init[i], &ptr[i]);

	/* Set VREF */
	val = 0;
	switch (para->type) {
	case SUNXI_DRAM_TYPE_DDR3:
		val = para->tpr6 & 0xff;
		if (val == 0)
			val = 0x80;
		break;
	case SUNXI_DRAM_TYPE_DDR4:
		val = (para->tpr6 >> 8) & 0xff;
		if (val == 0)
			val = 0x80;
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		val = (para->tpr6 >> 16) & 0xff;
		if (val == 0)
			val = 0x80;
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		val = (para->tpr6 >> 24) & 0xff;
		if (val == 0)
			val = 0x33;
		break;
	}
	writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x3dc);
	writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x45c);

	mctl_drive_odt_config(para);

	if (para->tpr10 & TPR10_CA_BIT_DELAY)
		mctl_phy_ca_bit_delay_compensation(para);

	switch (para->type) {
	case SUNXI_DRAM_TYPE_DDR3:
		val = 2;
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		val = 3;
		break;
	case SUNXI_DRAM_TYPE_DDR4:
		val = 4;
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		val = 5;
		break;
	}

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x4, 0x7, val | 8);

	if (para->clk <= 672)
		writel_relaxed(0xf, SUNXI_DRAM_PHY0_BASE + 0x20);

	if (para->clk > 500) {
		val = 0;
		val2 = 0;
	} else {
		val = 0x80;
		val2 = 0x20;
	}

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x144, 0x80, val);
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x14c, 0xe0, val2);

	__DSB();
	clrbits_le32(mctl_com + MCTL_COM_UNK_008, BIT_U32(9));
	udelay(1);
	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x14c, BIT_U32(3));

	mctl_await_completion((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x180), BIT_U32(2),
			      BIT_U32(2));

	/*
	 * This delay is controlled by a tpr13 flag in boot0; doesn't hurt
	 * to always do it though.
	 */
	udelay(1000);
	writel(0x37, (void *) (SUNXI_DRAM_PHY0_BASE + 0x58));

	setbits_le32(prcm + CCU_PRCM_SYS_PWROFF_GATING, BIT_U32(4));
}

/* Helpers for updating mode registers */
static inline void mctl_mr_write(uint32_t mrctrl0, uint32_t mrctrl1)
{
	struct sunxi_mctl_ctl_reg *mctl_ctl =
		(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;

	writel(mrctrl1, &mctl_ctl->mrctrl1);
	writel(mrctrl0 | MRCTRL0_MR_WR | MRCTRL0_MR_RANKS_ALL,
	       &mctl_ctl->mrctrl0);
	mctl_await_completion(&mctl_ctl->mrctrl0, MRCTRL0_MR_WR, 0);
}

static inline void mctl_mr_write_lpddr4(uint8_t addr, uint8_t value)
{
	mctl_mr_write(0, MRCTRL1_MR_ADDR(addr) | MRCTRL1_MR_DATA(value));
}

static inline void mctl_mr_write_lpddr3(uint8_t addr, uint8_t value)
{
	/* Bit [7:6] are set by boot0, but undocumented */
	mctl_mr_write(BIT_U32(6) | BIT_U32(7),
		      MRCTRL1_MR_ADDR(addr) | MRCTRL1_MR_DATA(value));
}

static void mctl_dfi_init(const struct dram_para *para)
{
	void *const mctl_com = (void *)SUNXI_DRAM_COM_BASE;
	struct sunxi_mctl_ctl_reg *mctl_ctl =
		(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;

	/* Unlock DFI registers? */
	setbits_le32(mctl_com + MCTL_COM_MAER0, BIT_U32(8));

	/* Enable dfi_init_complete signal and trigger PHY init start request */
	writel_relaxed(0, &mctl_ctl->swctl);
	setbits_le32(&mctl_ctl->dfimisc, BIT_U32(0));
	setbits_le32(&mctl_ctl->dfimisc, BIT_U32(5));
	writel_relaxed(1, &mctl_ctl->swctl);
	mctl_await_completion(&mctl_ctl->swstat, BIT_U32(0), BIT_U32(0));

	/* Stop sending init request and wait for DFI initialization to complete. */
	writel_relaxed(0, &mctl_ctl->swctl);
	clrbits_le32(&mctl_ctl->dfimisc, BIT_U32(5));
	writel_relaxed(1, &mctl_ctl->swctl);
	mctl_await_completion(&mctl_ctl->swstat, BIT_U32(0), BIT_U32(0));
	mctl_await_completion(&mctl_ctl->dfistat, BIT_U32(0), BIT_U32(0));

	/* Enter Software Exit from Self Refresh */
	writel_relaxed(0, &mctl_ctl->swctl);
	clrbits_le32(&mctl_ctl->pwrctl, BIT_U32(5));
	writel_relaxed(1, &mctl_ctl->swctl);
	mctl_await_completion(&mctl_ctl->swstat, BIT_U32(0), BIT_U32(0));
	mctl_await_completion(&mctl_ctl->statr, 0x3, 1);

	udelay(200);

	/* Disable dfi_init_complete signal */
	writel_relaxed(0, &mctl_ctl->swctl);
	clrbits_le32(&mctl_ctl->dfimisc, BIT_U32(0));
	writel_relaxed(1, &mctl_ctl->swctl);
	mctl_await_completion(&mctl_ctl->swstat, BIT_U32(0), BIT_U32(0));

	/* Write mode registers, fixed in the JEDEC spec */
	switch (para->type) {
	case SUNXI_DRAM_TYPE_DDR3:
		mctl_mr_write(MRCTRL0_MR_ADDR(0), 0x1c70);	/* MR0 */
		/*
		 * outbuf en, TDQs dis, write leveling dis, out drv 40 Ohms,
		 * DLL en, Rtt_nom 120 Ohms
		 */
		mctl_mr_write(MRCTRL0_MR_ADDR(1), 0x40);	/* MR1 */
		/*
		 * full array self-ref, CAS: 8 cyc, SRT w/ norm temp range,
		 * dynamic ODT off
		 */
		mctl_mr_write(MRCTRL0_MR_ADDR(2), 0x18);	/* MR2 */
		/* predef MPR pattern */
		mctl_mr_write(MRCTRL0_MR_ADDR(3), 0);		/* MR3 */
		break;
	case SUNXI_DRAM_TYPE_DDR4:
		mctl_mr_write(MRCTRL0_MR_ADDR(0), 0x840);
		mctl_mr_write(MRCTRL0_MR_ADDR(1), 0x601);
		mctl_mr_write(MRCTRL0_MR_ADDR(2), 0x8);
		mctl_mr_write(MRCTRL0_MR_ADDR(3), 0);
		mctl_mr_write(MRCTRL0_MR_ADDR(4), 0);
		mctl_mr_write(MRCTRL0_MR_ADDR(5), 0x400);

		mctl_mr_write(MRCTRL0_MR_ADDR(6), 0x862 | BIT_U32(7));
		mctl_mr_write(MRCTRL0_MR_ADDR(6), 0x862 | BIT_U32(7));
		mctl_mr_write(MRCTRL0_MR_ADDR(6), 0x862 & (~BIT_U32(7)));
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		mctl_mr_write_lpddr3(1, 0xc3);	/* MR1: nWR=8, BL8 */
		mctl_mr_write_lpddr3(2, 0xa);	/* MR2: RL=12, WL=6 */
		mctl_mr_write_lpddr3(3, 0x2);	/* MR3: 40 0hms PD/PU */
		mctl_mr_write_lpddr3(11, para->mr11);
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		mctl_mr_write_lpddr4(0, 0);	/* MR0 */
		mctl_mr_write_lpddr4(1, 0x34);	/* MR1 */
		mctl_mr_write_lpddr4(2, 0x1b);	/* MR2 */
		mctl_mr_write_lpddr4(3, 0x33);	/* MR3 */
		mctl_mr_write_lpddr4(4, 0x3);	/* MR4 */
		mctl_mr_write_lpddr4(11, para->mr11);
		mctl_mr_write_lpddr4(12, para->mr12);
		mctl_mr_write_lpddr4(13, para->mr13);
		mctl_mr_write_lpddr4(14, para->mr14);
		mctl_mr_write_lpddr4(22, para->tpr1);
		break;
	}

	writel(0, (void *) (SUNXI_DRAM_PHY0_BASE + 0x54));

	/* Re-enable controller refresh */
	writel(0, &mctl_ctl->swctl);
	clrbits_le32(&mctl_ctl->rfshctl3, BIT_U32(0));
	writel(1, &mctl_ctl->swctl);
}

/* Slightly modified from H616 driver */
static int mctl_phy_read_calibration(const struct dram_config *config)
{
	int result = 1;
	uint32_t val, tmp;

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30, 0x20);

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);

	if (config->bus_full_width)
		val = 0xf;
	else
		val = 3;

	while ((readl_relaxed(SUNXI_DRAM_PHY0_BASE + 0x184) & val) != val) {
		if (readl_relaxed(SUNXI_DRAM_PHY0_BASE + 0x184) & 0x20) {
			result = 0;
			break;
		}
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30);

	if (config->ranks == 1) {
		clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30, 0x10);

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);

		while ((readl_relaxed(SUNXI_DRAM_PHY0_BASE + 0x184) & val) !=
		       val) {
			if (readl_relaxed(SUNXI_DRAM_PHY0_BASE + 0x184) &
			    0x20) {
				result = 0;
				break;
			}
		}

		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30);

	val = readl_relaxed(SUNXI_DRAM_PHY0_BASE + 0x274) & 7;
	tmp = readl_relaxed(SUNXI_DRAM_PHY0_BASE + 0x26c) & 7;
	if (val < tmp)
		val = tmp;
	tmp = readl_relaxed(SUNXI_DRAM_PHY0_BASE + 0x32c) & 7;
	if (val < tmp)
		val = tmp;
	tmp = readl_relaxed(SUNXI_DRAM_PHY0_BASE + 0x334) & 7;
	if (val < tmp)
		val = tmp;
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x38, 0x7, (val + 2) & 7);

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 4, 0x20);

	return result;
}

static inline void mctl_phy_dx_delay1_inner(uint32_t *base, uint32_t val1, uint32_t val2)
{
	uint32_t *ptr = base;

	for (int i = 0; i < 9; i++) {
		writel_relaxed(val1, ptr);
		writel_relaxed(val1, ptr + 0x30);
		ptr += 2;
	}

	writel_relaxed(val2, ptr + 1);
	writel_relaxed(val2, ptr + 49);
	writel_relaxed(val2, ptr);
	writel_relaxed(val2, ptr + 48);
}

static inline void mctl_phy_dx_delay0_inner(uint32_t *base1, uint32_t *base2, uint32_t val1,
					    uint32_t val2)
{
	uint32_t *ptr = base1;

	for (int i = 0; i < 9; i++) {
		writel_relaxed(val1, ptr);
		writel_relaxed(val1, ptr + 0x30);
		ptr += 2;
	}

	writel_relaxed(val2, base2);
	writel_relaxed(val2, base2 + 48);
	writel_relaxed(val2, ptr);
	writel_relaxed(val2, base2 + 44);
}

/*
 * This might be somewhat transferable to H616; whether or not people like
 * the design is another question
 */
static void mctl_phy_dx_delay_compensation(const struct dram_para *para)
{
	if (para->tpr10 & TPR10_DX_BIT_DELAY1) {
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, 1);
		setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, BIT_U32(3));
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, BIT_U32(4));

		if (para->type == SUNXI_DRAM_TYPE_DDR4)
			clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x4, BIT_U32(7));

		mctl_phy_dx_delay1_inner((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x484),
					 para->tpr11 & 0x3f,
					 para->para0 & 0x3f);
		mctl_phy_dx_delay1_inner((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x4d8),
					 (para->tpr11 >> 8) & 0x3f,
					 (para->para0 >> 8) & 0x3f);
		mctl_phy_dx_delay1_inner((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x604),
					 (para->tpr11 >> 16) & 0x3f,
					 (para->para0 >> 16) & 0x3f);
		mctl_phy_dx_delay1_inner((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x658),
					 (para->tpr11 >> 24) & 0x3f,
					 (para->para0 >> 24) & 0x3f);

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, 1);
	}

	if (para->tpr10 & TPR10_DX_BIT_DELAY0) {
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x54, BIT_U32(7));
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, BIT_U32(2));

		mctl_phy_dx_delay0_inner((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x480),
					 (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x528),
					 para->tpr12 & 0x3f,
					 para->tpr14 & 0x3f);

		mctl_phy_dx_delay0_inner((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x4d4),
					 (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x52c),
					 (para->tpr12 >> 8) & 0x3f,
					 (para->tpr14 >> 8) & 0x3f);

		mctl_phy_dx_delay0_inner((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x600),
					 (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x6a8),
					 (para->tpr12 >> 16) & 0x3f,
					 (para->tpr14 >> 16) & 0x3f);

		mctl_phy_dx_delay0_inner((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x6ac),
					 (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x528),
					 (para->tpr12 >> 24) & 0x3f,
					 (para->tpr14 >> 24) & 0x3f);

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x54, BIT_U32(7));
	}
}

static int mctl_calibrate_phy(const struct dram_para *para,
			       const struct dram_config *config)
{
	struct sunxi_mctl_ctl_reg *mctl_ctl =
		(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;

	int i;

	/* TODO: Implement write levelling */
	if (para->tpr10 & TPR10_READ_CALIBRATION) {
		for (i = 0; i < 5; i++)
			if (mctl_phy_read_calibration(config))
				break;
		if (i == 5) {
			PRINTF("read calibration failed\n");
			return 0;
		}
	}

	/* TODO: Implement read training */
	/* TODO: Implement write training */

	mctl_phy_dx_delay_compensation(para);
	/* TODO: Implement DFS */
	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, BIT_U32(0));
	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x54, 7);

	/* Q: Does self-refresh get disabled by a calibration? */
	writel_relaxed(0, &mctl_ctl->swctl);
	clrbits_le32(&mctl_ctl->rfshctl3, BIT_U32(1));
	writel_relaxed(1, &mctl_ctl->swctl);
	mctl_await_completion(&mctl_ctl->swstat, BIT_U32(0), BIT_U32(0));

	return 1;
}

static int mctl_core_init(const struct dram_para *para,
			   const struct dram_config *config)
{
	mctl_clk_init(para->clk);
	mctl_com_init(para, config);
	mctl_phy_init(para, config);
	mctl_dfi_init(para);

	return mctl_calibrate_phy(para, config);
}

/* Heavily inspired from H616 driver. */
static void auto_detect_ranks(const struct dram_para *para,
			      struct dram_config *config)
{
	int i;

	config->cols = 9;
	config->rows = 14;
	config->banks = 2;
	config->bankgrps = 0;

	/* Test ranks */
	for (i = 1; i >= 0; i--) {
		config->ranks = i;
		config->bus_full_width = 1;
		PRINTF("Testing ranks = %d, 32-bit bus: ", i);
		if (mctl_core_init(para, config)) {
			PRINTF("OK\n");
			break;
		}

		config->bus_full_width = 0;
		PRINTF("Testing ranks = %d, 16-bit bus: ", i);
		if (mctl_core_init(para, config)) {
			PRINTF("OK\n");
			break;
		}
	}

	if (i < 0)
		PRINTF("rank testing failed\n");
}

static void mctl_write_pattern(void)
{
	unsigned int i;
	uint32_t *ptr, val;

	ptr = (uint32_t *)CFG_SYS_SDRAM_BASE;
	for (i = 0; i < 16; ptr++, i++) {
		if (i & 1)
			val = ~(uintptr_t)ptr;
		else
			val = (uintptr_t)ptr;
		writel(val, ptr);
	}
}

static int mctl_check_pattern(uint32_t offset) // was: ulong
{
	unsigned int i;
	uint32_t *ptr, val;

	ptr = (uint32_t *)CFG_SYS_SDRAM_BASE;
	for (i = 0; i < 16; ptr++, i++) {
		if (i & 1)
			val = ~(uint32_t)ptr;
		else
			val = (uint32_t)ptr;
		if (val != *(ptr + offset / 4))
			return 0;
	}

	return 1;
}

static void mctl_auto_detect_dram_size(const struct dram_para *para,
				       struct dram_config *config)
{
	unsigned int shift;
	uint32_t buffer[16];

	/* max config for bankgrps on DDR4, minimum for everything else */
	config->cols = 8;
	config->banks = 2;
	config->rows = 14;

	shift = 1 + config->bus_full_width;
	if (para->type == SUNXI_DRAM_TYPE_DDR4) {
		config->bankgrps = 2;
		mctl_core_init(para, config);

		/* store content so it can be restored later. */
		memcpy(buffer, (uint32_t *)CFG_SYS_SDRAM_BASE, sizeof(buffer));
		mctl_write_pattern();

		if (mctl_check_pattern(1ULL << (shift + 4)))
			config->bankgrps = 1;

		/* restore data */
		memcpy((uint32_t *)CFG_SYS_SDRAM_BASE, buffer, sizeof(buffer));
	} else {
		/* No bank groups in (LP)DDR3/LPDDR4 */
		config->bankgrps = 0;
	}

	/* reconfigure to make sure all active columns are accessible */
	config->cols = 12;
	mctl_core_init(para, config);

	/* store data again as it might be moved */
	memcpy(buffer, (uint32_t *)CFG_SYS_SDRAM_BASE, sizeof(buffer));
	mctl_write_pattern();

	/*
	 * Detect column address bits. The last number of columns checked
	 * is 11, if that doesn't match, is must be 12, no more checks needed.
	 */
	shift = 1 + config->bus_full_width + config->bankgrps;
	for (config->cols = 8; config->cols < 12; config->cols++) {
		if (mctl_check_pattern(1ULL << (config->cols + shift)))
			break;
	}
	memcpy((uint32_t *)CFG_SYS_SDRAM_BASE, buffer, sizeof(buffer));

	/* reconfigure to make sure that all active banks are accessible */
	config->banks = 3;
	mctl_core_init(para, config);

	memcpy(buffer, (uint32_t *)CFG_SYS_SDRAM_BASE, sizeof(buffer));
	mctl_write_pattern();

	/* detect bank bits */
	shift += config->cols;
	for (config->banks = 2; config->banks < 3; config->banks++) {
		if (mctl_check_pattern(1ULL << (config->banks + shift)))
			break;
	}
	memcpy((uint32_t *)CFG_SYS_SDRAM_BASE, buffer, sizeof(buffer));

	/* reconfigure to make sure that all active rows are accessible */
	config->rows = 18;
	mctl_core_init(para, config);

	memcpy(buffer, (uint32_t *)CFG_SYS_SDRAM_BASE, sizeof(buffer));
	mctl_write_pattern();

	/* detect row address bits */
	shift += config->banks;
	for (config->rows = 14; config->rows < 18; config->rows++) {
		if (mctl_check_pattern(1ULL << (config->rows + shift)))
			break;
	}
	memcpy((uint32_t *)CFG_SYS_SDRAM_BASE, buffer, sizeof(buffer));
}

/* Modified from H616 driver to add banks and bank groups */
static unsigned long calculate_dram_size(const struct dram_config *config)
{
	/* Bootrom only uses x32 or x16 bus widths */
	uint8_t width = config->bus_full_width ? 4 : 2;

	return (1ULL << (config->cols + config->rows + config->banks +
			 config->bankgrps)) *
	       width * (1ULL << config->ranks);
}

static const struct dram_para para = {
	.clk = BOARD_CONFIG_DRAM_CLK, //CONFIG_DRAM_CLK,
	.type = BOARD_CONFIG_DRAM_TYPE,
	/* TODO: Populate from config */
	.dx_odt = CONFIG_DRAM_SUNXI_DX_ODT,
	.dx_dri = CONFIG_DRAM_SUNXI_DX_DRI,
	.ca_dri = CONFIG_DRAM_SUNXI_CA_DRI,
	.para0 = CONFIG_DRAM_SUNXI_PARA0,
	.mr11 = CONFIG_DRAM_SUNXI_MR11,
	.mr12 = CONFIG_DRAM_SUNXI_MR12,
	.mr13 = CONFIG_DRAM_SUNXI_MR13,
	.mr14 = CONFIG_DRAM_SUNXI_MR14,
	.tpr1 = CONFIG_DRAM_SUNXI_TPR1,
	.tpr2 = CONFIG_DRAM_SUNXI_TPR2,
	.tpr3 = CONFIG_DRAM_SUNXI_TPR3,
	.tpr6 = CONFIG_DRAM_SUNXI_TPR6,
	.tpr10 = CONFIG_DRAM_SUNXI_TPR10,
	.tpr11 = CONFIG_DRAM_SUNXI_TPR11,
	.tpr12 = CONFIG_DRAM_SUNXI_TPR12,
	.tpr13 = CONFIG_DRAM_SUNXI_TPR13,
	.tpr14 = CONFIG_DRAM_SUNXI_TPR14,
};

unsigned long sunxi_dram_init(void)
{
	struct dram_config config;

	/* Writing to undocumented SYS_CFG area, according to user manual. */
	setbits_le32(0x03000160, BIT_U32(8));
	clrbits_le32(0x03000168, 0x3f);

	auto_detect_ranks(&para, &config);
	mctl_auto_detect_dram_size(&para, &config);

	if (!mctl_core_init(&para, &config))
		return 0;

	PRINTF("cols = 2^%d, rows = 2^%d, banks = %d, bank groups = %d, ranks = %d, width = %d\n",
	      config.cols, config.rows, 1U << config.banks,
	      1U << config.bankgrps, 1U << config.ranks,
	      16U << config.bus_full_width);

	return calculate_dram_size(&config);
}



void mctl_set_timing_params(const struct dram_para *para)
{
	struct sunxi_mctl_ctl_reg *const mctl_ctl =
		(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;

	uint8_t txsr = 4;
	uint8_t tccd = 3;
	uint8_t rd2wr = 5;
	uint8_t tmrd = 4;
	uint8_t tmrw = 0;
	uint8_t wrlat = 5;
	uint8_t rdlat = 7;
	uint8_t wr2pre = 14;
	uint8_t dfi_tphy_wrlat = 6;
	uint8_t dfi_trddata_en = 10;

	uint8_t tfaw = ns_to_t(35);
	uint8_t trrd = max(ns_to_t(8), 2);
	uint8_t txp = max(ns_to_t(6), 2);
	uint8_t tmrd_pda = max(ns_to_t(10), 8);
	uint8_t trp = ns_to_t(15);
	uint8_t trc = ns_to_t(49);
	uint8_t wr2rd_s = max(ns_to_t(3), 1) + 7;
	uint8_t tras_min = ns_to_t(34);
	uint16_t trefi_x32 = ns_to_t(7800) / 32;
	uint16_t trfc_min = ns_to_t(350);
	uint16_t txs_x32 = ns_to_t(360) / 32;
	uint16_t tmod = max(ns_to_t(15), 12);
	uint8_t tcke = max(ns_to_t(5), 2);
	uint8_t tcksrx = max(ns_to_t(10), 3);
	uint8_t txs_abort_x32 = ns_to_t(170) / 32;
	uint8_t tras_max = ns_to_t(70200) / 1024;

	uint8_t rd2pre = (trp < 5 ? 9 - trp : 4);
	uint8_t wr2rd = trrd + 7;
	uint8_t tckesr = tcke + 1;
	uint8_t trcd = trp;
	uint8_t trrd_s = txp;
	uint8_t tcksre = tcksrx;

	writel(tras_min | tras_max << 8 | tfaw << 16 | wr2pre << 24,
	       &mctl_ctl->dramtmg[0]);
	writel(trc | rd2pre << 8 | txp << 16, &mctl_ctl->dramtmg[1]);
	writel(wr2rd | rd2wr << 8 | rdlat << 16 | wrlat << 24,
	       &mctl_ctl->dramtmg[2]);
	writel(tmod | tmrd << 12 | tmrw << 20, &mctl_ctl->dramtmg[3]);
	writel(trp | trrd << 8 | tccd << 16 | trcd << 24,
	       &mctl_ctl->dramtmg[4]);
	writel(tcke | tckesr << 8 | tcksre << 16 | tcksrx << 24,
	       &mctl_ctl->dramtmg[5]);
	writel((txp + 2) | 0x20 << 16 | 0x20 << 24,
	       &mctl_ctl->dramtmg[6]);
	writel(txs_x32 | 0x10 << 8 | txs_abort_x32 << 16 | txs_abort_x32 << 24,
	       &mctl_ctl->dramtmg[8]);
	writel(wr2rd_s | trrd_s << 8 | 0x2 << 16, &mctl_ctl->dramtmg[9]);
	writel(0xe0c05, &mctl_ctl->dramtmg[10]);
	writel(0x440c021c, &mctl_ctl->dramtmg[11]);
	writel(tmrd_pda, &mctl_ctl->dramtmg[12]);
	writel(0xa100002, &mctl_ctl->dramtmg[13]);
	writel(txsr, &mctl_ctl->dramtmg[14]);

	clrsetbits_le32(&mctl_ctl->init[0], 0xc0000fff, 1008);
	writel(0x1f20000, &mctl_ctl->init[1]);
	clrsetbits_le32(&mctl_ctl->init[2], 0xff0f, 0xd05);
	writel(0, &mctl_ctl->dfimisc);

	writel(0x840 << 16 | 0x601, &mctl_ctl->init[3]);	/* MR0 / MR1 */
	writel(0x8 << 16 | 0x0, &mctl_ctl->init[4]);		/* MR2 / MR3 */
	writel(0x0 << 16 | 0x400, &mctl_ctl->init[6]);		/* MR4 / MR5 */
	writel(0x826, &mctl_ctl->init[7]);			/* MR6 */

	clrsetbits_le32(&mctl_ctl->rankctl, 0xff0, 0x660);
	writel((dfi_tphy_wrlat - 1) | 0x2000000 | (dfi_trddata_en - 1) << 16 |
	       0x808000, &mctl_ctl->dfitmg0);
	writel(0x100202, &mctl_ctl->dfitmg1);
	writel(trfc_min | trefi_x32 << 16, &mctl_ctl->rfshtmg);
}

void arm_hardware_sdram_initialize(void)
{
	TP();
	sunxi_dram_init();
	TP();
}
#endif /* WITHSDRAMHW && (CONFIG_SUNXI_DRAM_A133_LPDDR4) */

