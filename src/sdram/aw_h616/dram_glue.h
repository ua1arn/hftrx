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

#define TPR10_CA_BIT_DELAY	BIT_U32(16)
#define TPR10_DX_BIT_DELAY0	BIT_U32(17)
#define TPR10_DX_BIT_DELAY1	BIT_U32(18)
#define TPR10_WRITE_LEVELING	BIT_U32(20)
#define TPR10_READ_CALIBRATION	BIT_U32(21)
#define TPR10_READ_TRAINING	BIT_U32(22)
#define TPR10_WRITE_TRAINING	BIT_U32(23)

#define CONFIG_SYS_SDRAM_BASE 0x40000000

/* SID address space starts at 0x03006000, but e-fuse is at offset 0x200 */
#define SUNXI_SID_BASE   0x03006200


#if CONFIG_MACH_SUN50I_H6
	#define SUNXI_DRAM_COM_BASE			0x04002000
	#define SUNXI_DRAM_CTL0_BASE		0x04003000
	#define SUNXI_DRAM_PHY0_BASE		0x04005000
#endif

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

	#define CCM_PLL6_DEFAULT		0xb8003100
	#define CCM_PSI_AHB1_AHB2_DEFAULT	0x03000002
	#define CCM_AHB3_DEFAULT		0x03000002
	#define CCM_APB1_DEFAULT		0x03000102

#endif

#define DRAM_CLK_ENABLE			BIT_U32(31)

#define SUNXI_PRCM_BASE			0x07010000

#define SUNXI_CCM_BASE CCU_BASE

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define max(a,b) ((a) > (b) ? (a) : (b))

#define DIV_ROUND_UP(n, d)	(((n) + (d) - 1) / (d))
#define REPEAT_BYTE(x)		((~0ul / 0xff) * (x))

static inline int ns_to_t(int nanoseconds)
{
	const unsigned int ctrl_freq = BOARD_CONFIG_DRAM_CLK / 2;

	return DIV_ROUND_UP(ctrl_freq * (uint_fast64_t) nanoseconds, 1000);
}

// https://github.com/engSinteck/A133_Image/blob/125333364cdacc364a5ea855019756a03a3043dd/longan/brandy/arisc/ar100s/include/driver/dram.h#L83

typedef struct dram_para
{
	uint32_t clk;
	uint32_t /*enum sunxi_dram_type */type;
	uint32_t dx_odt;
	uint32_t dx_dri;
	uint32_t ca_dri;
	uint32_t para0;
	uint32_t para1;
	uint32_t para2;
	uint32_t mr0;
	uint32_t mr1;
	uint32_t mr2;
	uint32_t mr3;
	uint32_t mr4;
	uint32_t mr5;
	uint32_t mr6;
	uint32_t mr11;
	uint32_t mr12;
	uint32_t mr13;
	uint32_t mr14;
	uint32_t mr16;
	uint32_t mr17;
	uint32_t mr22;
	uint32_t tpr0;
	uint32_t tpr1;
	uint32_t tpr2;
	uint32_t tpr3;
	uint32_t tpr6;
	uint32_t tpr10;
	uint32_t tpr11;
	uint32_t tpr12;
	uint32_t tpr13;
	uint32_t tpr14;

	uint32_t odt_en;
} dram_para_t;

// CONFIG_DRAM_SUN50I_H616_UNKNOWN_FEATURE
// CONFIG_DRAM_ODT_EN
// CONFIG_DRAM_SUN50I_H616_WRITE_LEVELING
// CONFIG_DRAM_SUN50I_H616_READ_CALIBRATION
// CONFIG_DRAM_SUN50I_H616_READ_TRAINING
// CONFIG_DRAM_SUN50I_H616_WRITE_TRAINING
// CONFIG_DRAM_SUN50I_H616_BIT_DELAY_COMPENSATION

#define IS_ENABLED(v) (1)

#define BIT_U32(pos) (UINT32_C(1) << (pos))

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

#define clrbits_le32(addr, clear) \
	write32(((virtual_addr_t)(addr)), read32(((virtual_addr_t)(addr))) & ~(clear))

#define setbits_le32(addr, set) \
	write32(((virtual_addr_t)(addr)), read32(((virtual_addr_t)(addr))) | (set))

#define clrsetbits_le32(addr, clear, set) \
	write32(((virtual_addr_t)(addr)), (read32(((virtual_addr_t)(addr))) & ~(clear)) | (set))

#define writel(data, addr) do { write32((uintptr_t)(addr), (data)); } while (0)
#define readl(addr) (read32((uintptr_t)(addr)))

#define writel_relaxed(data, addr) do { writel((data), (addr)); } while (0)

struct sunxi_prcm_reg {
	uint32_t cpus_cfg;		/* 0x000 */
	uint8_t res0[0x8];		/* 0x004 */
	uint32_t apbs1_cfg;		/* 0x00c */
	uint32_t apbs2_cfg;		/* 0x010 */
	uint8_t res1[0x108];		/* 0x014 */
	uint32_t tmr_gate_reset;	/* 0x11c */
	uint8_t res2[0xc];		/* 0x120 */
	uint32_t twd_gate_reset;	/* 0x12c */
	uint8_t res3[0xc];		/* 0x130 */
	uint32_t pwm_gate_reset;	/* 0x13c */
	uint8_t res4[0x4c];		/* 0x140 */
	uint32_t uart_gate_reset;	/* 0x18c */
	uint8_t res5[0xc];		/* 0x190 */
	uint32_t twi_gate_reset;	/* 0x19c */
	uint8_t res6[0x1c];		/* 0x1a0 */
	uint32_t rsb_gate_reset;	/* 0x1bc */
	uint32_t cir_cfg;		/* 0x1c0 */
	uint8_t res7[0x8];		/* 0x1c4 */
	uint32_t cir_gate_reset;	/* 0x1cc */
	uint8_t res8[0x10];		/* 0x1d0 */
	uint32_t w1_cfg;		/* 0x1e0 */
	uint8_t res9[0x8];		/* 0x1e4 */
	uint32_t w1_gate_reset;	/* 0x1ec */
	uint8_t res10[0x1c];		/* 0x1f0 */
	uint32_t rtc_gate_reset;	/* 0x20c */
	uint8_t res11[0x34];		/* 0x210 */
	uint32_t pll_ldo_cfg;	/* 0x244 */
	uint8_t res12[0x8];		/* 0x248 */
	uint32_t sys_pwroff_gating;	/* 0x250 */
	uint8_t res13[0xbc];		/* 0x254 */
	uint32_t res_cal_ctrl;	/* 0x310 */
	uint32_t ohms200;		/* 0x314 */
	uint32_t ohms240;		/* 0x318 */
	uint32_t res_cal_status;	/* 0x31c */
};
//check_member(sunxi_prcm_reg, rtc_gate_reset, 0x20c);
//check_member(sunxi_prcm_reg, res_cal_status, 0x31c);

#define PRCM_TWI_GATE		(1 << 0)
#define PRCM_TWI_RESET		(1 << 16)

// https://github.com/apritzel/u-boot/blob/master/arch/arm/include/asm/arch-sunxi/dram_sun50i_h616.h

enum sunxi_dram_type {
	SUNXI_DRAM_TYPE_DDR3 = 3,
	SUNXI_DRAM_TYPE_DDR4,
	SUNXI_DRAM_TYPE_LPDDR3 = 7,
	SUNXI_DRAM_TYPE_LPDDR4
};

/* MBUS part is largely the same as in H6, except for one special register */
struct sunxi_mctl_com_reg {
	uint32_t cr;			/* 0x000 control register */
	uint8_t reserved_0x004[4];	/* 0x004 */
	uint32_t unk_0x008;		/* 0x008 */
	uint32_t tmr;		/* 0x00c timer register */
	uint8_t reserved_0x010[4];	/* 0x010 */
	uint32_t unk_0x014;		/* 0x014 */
	uint8_t reserved_0x018[8];	/* 0x018 */
	uint32_t maer0;		/* 0x020 master enable register 0 */
	uint32_t maer1;		/* 0x024 master enable register 1 */
	uint32_t maer2;		/* 0x028 master enable register 2 */
	uint8_t reserved_0x02c[468];	/* 0x02c */
	uint32_t bwcr;		/* 0x200 bandwidth control register */
	uint8_t reserved_0x204[12];	/* 0x204 */
	/*
	 * The last master configured by BSP libdram is at 0x49x, so the
	 * size of this struct array is set to 41 (0x29) now.
	 */
	struct {
		uint32_t cfg0;		/* 0x0 */
		uint32_t cfg1;		/* 0x4 */
		uint8_t reserved_0x8[8];	/* 0x8 */
	} master[41];		/* 0x210 + index * 0x10 */
	uint8_t reserved_0x4a0[96];	/* 0x4a0 */
	uint32_t unk_0x500;		/* 0x500 */
};
//check_member(sunxi_mctl_com_reg, unk_0x500, 0x500);

/*
 * Controller registers seems to be the same or at least very similar
 * to those in H6.
 */
struct sunxi_mctl_ctl_reg {
	uint32_t mstr;		/* 0x000 */
	uint32_t statr;		/* 0x004 unused */
	uint32_t mstr1;		/* 0x008 unused */
	uint32_t clken;		/* 0x00c */
	uint32_t mrctrl0;		/* 0x010 unused */
	uint32_t mrctrl1;		/* 0x014 unused */
	uint32_t mrstatr;		/* 0x018 unused */
	uint32_t mrctrl2;		/* 0x01c unused */
	uint32_t derateen;		/* 0x020 unused */
	uint32_t derateint;		/* 0x024 unused */
	uint8_t reserved_0x028[8];	/* 0x028 */
	uint32_t pwrctl;		/* 0x030 unused */
	uint32_t pwrtmg;		/* 0x034 unused */
	uint32_t hwlpctl;		/* 0x038 unused */
	uint8_t reserved_0x03c[20];	/* 0x03c */
	uint32_t rfshctl0;		/* 0x050 unused */
	uint32_t rfshctl1;		/* 0x054 unused */
	uint8_t reserved_0x058[8];	/* 0x05c */
	uint32_t rfshctl3;		/* 0x060 */
	uint32_t rfshtmg;		/* 0x064 */
	uint8_t reserved_0x068[104];	/* 0x068 */
	uint32_t init[8];		/* 0x0d0 */
	uint32_t dimmctl;		/* 0x0f0 unused */
	uint32_t rankctl;		/* 0x0f4 */
	uint8_t reserved_0x0f8[8];	/* 0x0f8 */
	uint32_t dramtmg[17];	/* 0x100 */
	uint8_t reserved_0x144[60];	/* 0x144 */
	uint32_t zqctl[3];		/* 0x180 */
	uint32_t zqstat;		/* 0x18c unused */
	uint32_t dfitmg0;		/* 0x190 */
	uint32_t dfitmg1;		/* 0x194 */
	uint32_t dfilpcfg[2];	/* 0x198 unused */
	uint32_t dfiupd[3];		/* 0x1a0 */
	uint32_t reserved_0x1ac;	/* 0x1ac */
	uint32_t dfimisc;		/* 0x1b0 */
	uint32_t dfitmg2;		/* 0x1b4 unused */
	uint32_t dfitmg3;		/* 0x1b8 unused */
	uint32_t dfistat;		/* 0x1bc */
	uint32_t dbictl;		/* 0x1c0 */
	uint8_t reserved_0x1c4[60];	/* 0x1c4 */
	uint32_t addrmap[12];	/* 0x200 */
	uint8_t reserved_0x230[16];	/* 0x230 */
	uint32_t odtcfg;		/* 0x240 */
	uint32_t odtmap;		/* 0x244 */
	uint8_t reserved_0x248[8];	/* 0x248 */
	uint32_t sched[2];		/* 0x250 */
	uint8_t reserved_0x258[180];	/* 0x258 */
	uint32_t dbgcmd;		/* 0x30c unused */
	uint32_t dbgstat;		/* 0x310 unused */
	uint8_t reserved_0x314[12];	/* 0x314 */
	uint32_t swctl;		/* 0x320 */
	uint32_t swstat;		/* 0x324 */
	uint8_t reserved_0x328[7768];/* 0x328 */
	uint32_t unk_0x2180;		/* 0x2180 */
	uint8_t reserved_0x2184[188];/* 0x2184 */
	uint32_t unk_0x2240;		/* 0x2240 */
	uint8_t reserved_0x2244[3900];/* 0x2244 */
	uint32_t unk_0x3180;		/* 0x3180 */
	uint8_t reserved_0x3184[188];/* 0x3184 */
	uint32_t unk_0x3240;		/* 0x3240 */
	uint8_t reserved_0x3244[3900];/* 0x3244 */
	uint32_t unk_0x4180;		/* 0x4180 */
	uint8_t reserved_0x4184[188];/* 0x4184 */
	uint32_t unk_0x4240;		/* 0x4240 */
};
//check_member(sunxi_mctl_ctl_reg, swstat, 0x324);
//check_member(sunxi_mctl_ctl_reg, unk_0x4240, 0x4240);

#define MSTR_DEVICETYPE_DDR3	BIT_U32(0)
#define MSTR_DEVICETYPE_LPDDR2	BIT_U32(2)
#define MSTR_DEVICETYPE_LPDDR3	BIT_U32(3)
#define MSTR_DEVICETYPE_DDR4	BIT_U32(4)
#define MSTR_DEVICETYPE_LPDDR4	BIT_U32(5)
#define MSTR_DEVICETYPE_MASK	GENMASK(5, 0)
#define MSTR_2TMODE		BIT_U32(10)
#define MSTR_BUSWIDTH_FULL	(0 << 12)
#define MSTR_BUSWIDTH_HALF	(1 << 12)
#define MSTR_ACTIVE_RANKS(x)	(((x == 2) ? 3 : 1) << 24)
#define MSTR_BURST_LENGTH(x)	(((x) >> 1) << 16)

struct dram_config {
	uint32_t clk;
	enum sunxi_dram_type type;
	uint32_t cols;
	uint32_t rows;
	uint32_t ranks;
	uint32_t bus_full_width;
};



unsigned long sunxi_dram_init(void);
void mctl_set_timing_params(const struct dram_para *para);

#endif /* WITHSDRAMHW */
#endif /* SRC_SDRAM_AW_H616_DRAM_GLUE_H_ */
