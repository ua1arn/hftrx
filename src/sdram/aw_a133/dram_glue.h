/*
 * dram_glue.h
 *
 *  Created on: Jan 17, 2026
 *      Author: Gena
 */

#ifndef SRC_SDRAM_AW_H616_DRAM_GLUE_H_
#define SRC_SDRAM_AW_H616_DRAM_GLUE_H_

#include "hardware.h"

#if WITHSDRAMHW
#include "formats.h"
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint32_t ulong;
//typedef int bool;

#define max(a,b) ((a) > (b) ? (a) : (b))

#define DIV_ROUND_UP(n, d)	(((n) + (d) - 1) / (d))
#define REPEAT_BYTE(x)		((~0ul / 0xff) * (x))

#define IS_ENABLED(v) (1)

#define BIT(pos) (UINT32_C(1) << (pos))

typedef uintptr_t virtual_addr_t;

static uint32_t read32(uintptr_t addr)
{
	//__DSB();
	return * (volatile uint32_t *) addr;
}

static void write32(uintptr_t addr, uint32_t value)
{
	* (volatile uint32_t *) addr = value;
	//__DSB();
}

static void write32ptr(volatile void * addr, uint32_t value)
{
	* (volatile uint32_t *) addr = value;
	//__DSB();
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

#define clrbits_le32(addr, clear) \
	write32(((virtual_addr_t)(addr)), read32(((virtual_addr_t)(addr))) & ~(clear))

#define setbits_le32(addr, set) \
	write32(((virtual_addr_t)(addr)), read32(((virtual_addr_t)(addr))) | (set))

#define clrsetbits_le32(addr, clear, set) \
	write32(((virtual_addr_t)(addr)), (read32(((virtual_addr_t)(addr))) & ~(clear)) | (set))

#define writel(data, addr) do { write32((uintptr_t)(addr), (data)); } while (0)
#define readl(addr) (read32((uintptr_t)(addr)))

#define writel_relaxed(data, addr) do { writel((data), (addr)); } while (0)
#define readl_relaxed(addr) (readl((addr)))



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
	u32 mstr; 			/* 0x000 */
	u32 statr; 			/* 0x004 unused */
	u32 mstr1; 			/* 0x008 unused */
	u32 clken; 			/* 0x00c */
	u32 mrctrl0; 			/* 0x010 unused */
	u32 mrctrl1; 			/* 0x014 unused */
	u32 mrstatr; 			/* 0x018 unused */
	u32 mrctrl2; 			/* 0x01c unused */
	u32 derateen; 			/* 0x020 unused */
	u32 derateint; 			/* 0x024 unused */
	u8 reserved_0x028[8]; 		/* 0x028 */
	u32 pwrctl; 			/* 0x030 unused */
	u32 pwrtmg; 			/* 0x034 unused */
	u32 hwlpctl; 			/* 0x038 unused */
	u8 reserved_0x03c[20];		/* 0x03c */
	u32 rfshctl0;			/* 0x050 unused */
	u32 rfshctl1;			/* 0x054 unused */
	u8 reserved_0x058[8];		/* 0x05c */
	u32 rfshctl3;			/* 0x060 */
	u32 rfshtmg;			/* 0x064 */
	u8 reserved_0x068[104];		/* 0x068 */
	u32 init[8];			/* 0x0d0 */
	u32 dimmctl;			/* 0x0f0 unused */
	u32 rankctl;			/* 0x0f4 */
	u8 reserved_0x0f8[8];		/* 0x0f8 */
	u32 dramtmg[17];		/* 0x100 */
	u8 reserved_0x144[60];		/* 0x144 */
	u32 zqctl[3];			/* 0x180 */
	u32 zqstat;			/* 0x18c unused */
	u32 dfitmg0;			/* 0x190 */
	u32 dfitmg1;			/* 0x194 */
	u32 dfilpcfg[2];		/* 0x198 unused */
	u32 dfiupd[3];			/* 0x1a0 */
	u32 reserved_0x1ac;		/* 0x1ac */
	u32 dfimisc;			/* 0x1b0 */
	u32 dfitmg2;			/* 0x1b4 unused */
	u32 dfitmg3;			/* 0x1b8 unused */
	u32 dfistat;			/* 0x1bc */
	u32 dbictl;			/* 0x1c0 */
	u8 reserved_0x1c4[60];		/* 0x1c4 */
	u32 addrmap[12];		/* 0x200 */
	u8 reserved_0x230[16];		/* 0x230 */
	u32 odtcfg;			/* 0x240 */
	u32 odtmap;			/* 0x244 */
	u8 reserved_0x248[8];		/* 0x248 */
	u32 sched[2];			/* 0x250 */
	u8 reserved_0x258[180]; 	/* 0x258 */
	u32 dbgcmd;			/* 0x30c unused */
	u32 dbgstat;			/* 0x310 unused */
	u8 reserved_0x314[12];		/* 0x314 */
	u32 swctl;			/* 0x320 */
	u32 swstat;			/* 0x324 */
	u8 reserved_0x328[7768];	/* 0x328 */
	u32 unk_0x2180;			/* 0x2180 */
	u8 reserved_0x2184[188];	/* 0x2184 */
	u32 unk_0x2240;			/* 0x2240 */
	u8 reserved_0x2244[3900];	/* 0x2244 */
	u32 unk_0x3180;			/* 0x3180 */
	u8 reserved_0x3184[188];	/* 0x3184 */
	u32 unk_0x3240;			/* 0x3240 */
	u8 reserved_0x3244[3900];	/* 0x3244 */
	u32 unk_0x4180;			/* 0x4180 */
	u8 reserved_0x4184[188];	/* 0x4184 */
	u32 unk_0x4240;			/* 0x4240 */
};

//check_member(sunxi_mctl_ctl_reg, swstat, 0x324);
//check_member(sunxi_mctl_ctl_reg, unk_0x4240, 0x4240);

#define MSTR_DEVICETYPE_DDR3	BIT(0)
#define MSTR_DEVICETYPE_LPDDR2	BIT(2)
#define MSTR_DEVICETYPE_LPDDR3	BIT(3)
#define MSTR_DEVICETYPE_DDR4	BIT(4)
#define MSTR_DEVICETYPE_LPDDR4	BIT(5)
#define MSTR_DEVICETYPE_MASK	GENMASK(5, 0)
#define MSTR_GEARDOWNMODE	BIT(0)		/* Same as MSTR_DEVICETYPE_DDR3, only used for DDR4 */
#define MSTR_2TMODE		BIT(10)
#define MSTR_BUSWIDTH_FULL	(0 << 12)
#define MSTR_BUSWIDTH_HALF	(1 << 12)
#define MSTR_ACTIVE_RANKS(x)	(((x == 1) ? 3 : 1) << 24)
#define MSTR_BURST_LENGTH(x)	(((x) >> 1) << 16)
#define MSTR_DEVICECONFIG_X32	(3 << 30)

#define TPR10_CA_BIT_DELAY	BIT(16)
#define TPR10_DX_BIT_DELAY0	BIT(17)
#define TPR10_DX_BIT_DELAY1	BIT(18)
#define TPR10_WRITE_LEVELING	BIT(20)
#define TPR10_READ_CALIBRATION	BIT(21)
#define TPR10_READ_TRAINING	BIT(22)
#define TPR10_WRITE_TRAINING	BIT(23)

/* MRCTRL constants */
#define MRCTRL0_MR_RANKS_ALL	(3 << 4)
#define MRCTRL0_MR_ADDR(x)	(x << 12)
#define MRCTRL0_MR_WR		BIT(31)

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
	u8 cols;		/* Column bits */
	u8 rows;		/* Row bits */
	u8 ranks;		/* Rank bits (different from H616!) */
	u8 banks;		/* Bank bits */
	u8 bankgrps;		/* Bank group bits */
	u8 bus_full_width;	/* 1 = x32, 0 = x16 */
};


#if CONFIG_MACH_SUN50I_A133
	// https://github.com/u-boot/u-boot/blob/d33b21b7e261691e8d6613a24cc9b0ececba3b01/arch/arm/include/asm/arch-sunxi/cpu_sun50i_h6.h#L33
	#define SUNXI_DRAM_COM_BASE			0x04810000
	#define SUNXI_DRAM_CTL0_BASE		0x04820000
	#define SUNXI_DRAM_PHY0_BASE		0x04830000

	#define CCM_PLL6_DEFAULT		0xb8003100
	#define CCM_PSI_AHB1_AHB2_DEFAULT	0x03000002
	#define CCM_AHB3_DEFAULT		0x03000002
	#define CCM_APB1_DEFAULT		0x03000102

#endif

#include "cpu_sun50i_h6.h"
/*
#define SUNXI_PRCM_BASE			0x07010000

#define SUNXI_CCM_BASE CCU_BASE
*/

/* Main CCU register offsets */
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
#define CCM_PLL_CTRL_EN			BIT(31)
#define CCM_PLL_LDO_EN			BIT(30)
#define CCM_PLL_LOCK_EN			BIT(29)
#define CCM_PLL_LOCK			BIT(28)
#define CCM_PLL_OUT_EN			BIT(27)
#define CCM_PLL1_UPDATE			BIT(26)
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
#define MBUS_ENABLE			BIT(31)
#define MBUS_RESET			BIT(30)
#define MBUS_UPDATE			BIT(27)
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
#define DRAM_CLK_ENABLE			BIT(31)
#define DRAM_MOD_RESET			BIT(30)
#define DRAM_CLK_UPDATE			BIT(27)
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


#define CCU_PRCM_I2C_GATE_RESET		0x19c
#define CCU_PRCM_PLL_LDO_CFG		0x244
#define CCU_PRCM_SYS_PWROFF_GATING	0x250
#define CCU_PRCM_RES_CAL_CTRL		0x310
#define CCU_PRCM_OHMS240		0x318

#define PRCM_TWI_GATE		(1 << 0)
#define PRCM_TWI_RESET		(1 << 16)

#define CONFIG_SYS_SDRAM_BASE 0x40000000
#define CFG_SYS_SDRAM_BASE	0x40000000

#define panic PRINTF
#define debug PRINTF
#define dmb() do { __DMB(); } while (0)
#define dsb() do { __DSB(); } while (0)

#if CPUSTYLE_A133
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
#define CONFIG_MACH_SUN50I_A133 1
#define CONFIG_SUNXI_DRAM_A133_LPDDR4 1
#endif

#define CONFIG_DRAM_SUNXI_MR13 0	// ???

unsigned long sunxi_dram_init(void);
void mctl_set_timing_params(const struct dram_para *para);
int mctl_mem_matches(uint64_t offset);

#endif /* WITHSDRAMHW */
#endif /* SRC_SDRAM_AW_H616_DRAM_GLUE_H_ */
