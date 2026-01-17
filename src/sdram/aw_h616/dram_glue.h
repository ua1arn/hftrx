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

#define BIT BIT_U32
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
	const unsigned int ctrl_freq = CONFIG_DRAM_CLK / 2;

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


struct sunxi_ccm_reg {
	uint32_t pll1_cfg;		/* 0x000 pll1 (cpux) control */
	uint8_t reserved_0x004[12];
	uint32_t pll5_cfg;		/* 0x010 pll5 (ddr) control */
	uint8_t reserved_0x014[12];
	uint32_t pll6_cfg;		/* 0x020 pll6 (periph0) control */
	uint8_t reserved_0x020[4];
	uint32_t pll_periph1_cfg;	/* 0x028 pll periph1 control */
	uint8_t reserved_0x028[4];
	uint32_t pll7_cfg;		/* 0x030 pll7 (gpu) control */
	uint8_t reserved_0x034[12];
	uint32_t pll3_cfg;		/* 0x040 pll3 (video0) control */
	uint8_t reserved_0x044[4];
	uint32_t pll_video1_cfg;	/* 0x048 pll video1 control */
	uint8_t reserved_0x04c[12];
	uint32_t pll4_cfg;		/* 0x058 pll4 (ve) control */
	uint8_t reserved_0x05c[4];
	uint32_t pll10_cfg;		/* 0x060 pll10 (de) control */
	uint8_t reserved_0x064[12];
	uint32_t pll9_cfg;		/* 0x070 pll9 (hsic) control */
	uint8_t reserved_0x074[4];
	uint32_t pll2_cfg;		/* 0x078 pll2 (audio) control */
	uint8_t reserved_0x07c[148];
	uint32_t pll5_pat;		/* 0x110 pll5 (ddr) pattern */
	uint8_t reserved_0x114[20];
	uint32_t pll_periph1_pat0;	/* 0x128 pll periph1 pattern0 */
	uint32_t pll_periph1_pat1;	/* 0x12c pll periph1 pattern1 */
	uint32_t pll7_pat0;		/* 0x130 pll7 (gpu) pattern0 */
	uint32_t pll7_pat1;		/* 0x134 pll7 (gpu) pattern1 */
	uint8_t reserved_0x138[8];
	uint32_t pll3_pat0;		/* 0x140 pll3 (video0) pattern0 */
	uint32_t pll3_pat1;		/* 0x144 pll3 (video0) pattern1 */
	uint32_t pll_video1_pat0;	/* 0x148 pll video1 pattern0 */
	uint32_t pll_video1_pat1;	/* 0x14c pll video1 pattern1 */
	uint8_t reserved_0x150[8];
	uint32_t pll4_pat0;		/* 0x158 pll4 (ve) pattern0 */
	uint32_t pll4_pat1;		/* 0x15c pll4 (ve) pattern1 */
	uint32_t pll10_pat0;		/* 0x160 pll10 (de) pattern0 */
	uint32_t pll10_pat1;		/* 0x164 pll10 (de) pattern1 */
	uint8_t reserved_0x168[8];
	uint32_t pll9_pat0;		/* 0x170 pll9 (hsic) pattern0 */
	uint32_t pll9_pat1;		/* 0x174 pll9 (hsic) pattern1 */
	uint32_t pll2_pat0;		/* 0x178 pll2 (audio) pattern0 */
	uint32_t pll2_pat1;		/* 0x17c pll2 (audio) pattern1 */
	uint8_t reserved_0x180[384];
	uint32_t pll1_bias;		/* 0x300 pll1 (cpux) bias */
	uint8_t reserved_0x304[12];
	uint32_t pll5_bias;		/* 0x310 pll5 (ddr) bias */
	uint8_t reserved_0x314[12];
	uint32_t pll6_bias;		/* 0x320 pll6 (periph0) bias */
	uint8_t reserved_0x324[4];
	uint32_t pll_periph1_bias;	/* 0x328 pll periph1 bias */
	uint8_t reserved_0x32c[4];
	uint32_t pll7_bias;		/* 0x330 pll7 (gpu) bias */
	uint8_t reserved_0x334[12];
	uint32_t pll3_bias;		/* 0x340 pll3 (video0) bias */
	uint8_t reserved_0x344[4];
	uint32_t pll_video1_bias;	/* 0x348 pll video1 bias */
	uint8_t reserved_0x34c[12];
	uint32_t pll4_bias;		/* 0x358 pll4 (ve) bias */
	uint8_t reserved_0x35c[4];
	uint32_t pll10_bias;		/* 0x360 pll10 (de) bias */
	uint8_t reserved_0x364[12];
	uint32_t pll9_bias;		/* 0x370 pll9 (hsic) bias */
	uint8_t reserved_0x374[4];
	uint32_t pll2_bias;		/* 0x378 pll2 (audio) bias */
	uint8_t reserved_0x37c[132];
	uint32_t pll1_tun;		/* 0x400 pll1 (cpux) tunning */
	uint8_t reserved_0x404[252];
	uint32_t cpu_axi_cfg;	/* 0x500 CPUX/AXI clock control*/
	uint8_t reserved_0x504[12];
	uint32_t psi_ahb1_ahb2_cfg;	/* 0x510 PSI/AHB1/AHB2 clock control */
	uint8_t reserved_0x514[8];
	uint32_t ahb3_cfg;		/* 0x51c AHB3 clock control */
	uint32_t apb1_cfg;		/* 0x520 APB1 clock control */
	uint32_t apb2_cfg;		/* 0x524 APB2 clock control */
	uint8_t reserved_0x528[24];
	uint32_t mbus_cfg;		/* 0x540 MBUS clock control */
	uint8_t reserved_0x544[188];
	uint32_t de_clk_cfg;		/* 0x600 DE clock control */
	uint8_t reserved_0x604[8];
	uint32_t de_gate_reset;	/* 0x60c DE gate/reset control */
	uint8_t reserved_0x610[16];
	uint32_t di_clk_cfg;		/* 0x620 DI clock control */
	uint8_t reserved_0x024[8];
	uint32_t di_gate_reset;	/* 0x62c DI gate/reset control */
	uint8_t reserved_0x630[64];
	uint32_t gpu_clk_cfg;	/* 0x670 GPU clock control */
	uint8_t reserved_0x674[8];
	uint32_t gpu_gate_reset;	/* 0x67c GPU gate/reset control */
	uint32_t ce_clk_cfg;		/* 0x680 CE clock control */
	uint8_t reserved_0x684[8];
	uint32_t ce_gate_reset;	/* 0x68c CE gate/reset control */
	uint32_t ve_clk_cfg;		/* 0x690 VE clock control */
	uint8_t reserved_0x694[8];
	uint32_t ve_gate_reset;	/* 0x69c VE gate/reset control */
	uint8_t reserved_0x6a0[16];
	uint32_t emce_clk_cfg;	/* 0x6b0 EMCE clock control */
	uint8_t reserved_0x6b4[8];
	uint32_t emce_gate_reset;	/* 0x6bc EMCE gate/reset control */
	uint32_t vp9_clk_cfg;	/* 0x6c0 VP9 clock control */
	uint8_t reserved_0x6c4[8];
	uint32_t vp9_gate_reset;	/* 0x6cc VP9 gate/reset control */
	uint8_t reserved_0x6d0[60];
	uint32_t dma_gate_reset;	/* 0x70c DMA gate/reset control */
	uint8_t reserved_0x710[12];
	uint32_t msgbox_gate_reset;	/* 0x71c Message Box gate/reset control */
	uint8_t reserved_0x720[12];
	uint32_t spinlock_gate_reset;/* 0x72c Spinlock gate/reset control */
	uint8_t reserved_0x730[12];
	uint32_t hstimer_gate_reset;	/* 0x73c HS Timer gate/reset control */
	uint32_t avs_gate_reset;	/* 0x740 AVS gate/reset control */
	uint8_t reserved_0x744[72];
	uint32_t dbgsys_gate_reset;	/* 0x78c Debugging system gate/reset control */
	uint8_t reserved_0x790[12];
	uint32_t psi_gate_reset;	/* 0x79c PSI gate/reset control */
	uint8_t reserved_0x7a0[12];
	uint32_t pwm_gate_reset;	/* 0x7ac PWM gate/reset control */
	uint8_t reserved_0x7b0[12];
	uint32_t iommu_gate_reset;	/* 0x7bc IOMMU gate/reset control */
	uint8_t reserved_0x7c0[64];
	uint32_t dram_clk_cfg;		/* 0x800 DRAM clock control */
	uint32_t mbus_gate;		/* 0x804 MBUS gate control */
	uint8_t reserved_0x808[4];
	uint32_t dram_gate_reset;	/* 0x80c DRAM gate/reset control */
	uint32_t nand0_clk_cfg;	/* 0x810 NAND0 clock control */
	uint32_t nand1_clk_cfg;	/* 0x814 NAND1 clock control */
	uint8_t reserved_0x818[20];
	uint32_t nand_gate_reset;	/* 0x82c NAND gate/reset control */
	uint32_t sd0_clk_cfg;	/* 0x830 MMC0 clock control */
	uint32_t sd1_clk_cfg;	/* 0x834 MMC1 clock control */
	uint32_t sd2_clk_cfg;	/* 0x838 MMC2 clock control */
	uint8_t reserved_0x83c[16];
	uint32_t sd_gate_reset;	/* 0x84c MMC gate/reset control */
	uint8_t reserved_0x850[188];
	uint32_t uart_gate_reset;	/* 0x90c UART gate/reset control */
	uint8_t reserved_0x910[12];
	uint32_t twi_gate_reset;	/* 0x91c I2C gate/reset control */
	uint8_t reserved_0x920[28];
	uint32_t scr_gate_reset;	/* 0x93c SCR gate/reset control */
	uint32_t spi0_clk_cfg;	/* 0x940 SPI0 clock control */
	uint32_t spi1_clk_cfg;	/* 0x944 SPI1 clock control */
	uint8_t reserved_0x948[36];
	uint32_t spi_gate_reset;	/* 0x96c SPI gate/reset control */
	uint8_t reserved_0x970[12];
	uint32_t emac_gate_reset;	/* 0x97c EMAC gate/reset control */
	uint8_t reserved_0x980[48];
	uint32_t ts_clk_cfg;		/* 0x9b0 TS clock control */
	uint8_t reserved_0x9b4[8];
	uint32_t ts_gate_reset;	/* 0x9bc TS gate/reset control */
	uint32_t irtx_clk_cfg;	/* 0x9c0 IR TX clock control */
	uint8_t reserved_0x9c4[8];
	uint32_t irtx_gate_reset;	/* 0x9cc IR TX gate/reset control */
	uint8_t reserved_0x9d0[44];
	uint32_t ths_gate_reset;	/* 0x9fc THS gate/reset control */
	uint8_t reserved_0xa00[12];
	uint32_t i2s3_clk_cfg;	/* 0xa0c I2S3 clock control */
	uint32_t i2s0_clk_cfg;	/* 0xa10 I2S0 clock control */
	uint32_t i2s1_clk_cfg;	/* 0xa14 I2S1 clock control */
	uint32_t i2s2_clk_cfg;	/* 0xa18 I2S2 clock control */
	uint32_t i2s_gate_reset;	/* 0xa1c I2S gate/reset control */
	uint32_t spdif_clk_cfg;	/* 0xa20 SPDIF clock control */
	uint8_t reserved_0xa24[8];
	uint32_t spdif_gate_reset;	/* 0xa2c SPDIF gate/reset control */
	uint8_t reserved_0xa30[16];
	uint32_t dmic_clk_cfg;	/* 0xa40 DMIC clock control */
	uint8_t reserved_0xa44[8];
	uint32_t dmic_gate_reset;	/* 0xa4c DMIC gate/reset control */
	uint8_t reserved_0xa50[16];
	uint32_t ahub_clk_cfg;	/* 0xa60 Audio HUB clock control */
	uint8_t reserved_0xa64[8];
	uint32_t ahub_gate_reset;	/* 0xa6c Audio HUB gate/reset control */
	uint32_t usb0_clk_cfg;	/* 0xa70 USB0(OTG) clock control */
	uint32_t usb1_clk_cfg;	/* 0xa74 USB1(XHCI) clock control */
	uint8_t reserved_0xa78[4];
	uint32_t usb3_clk_cfg;	/* 0xa78 USB3 clock control */
	uint8_t reserved_0xa80[12];
	uint32_t usb_gate_reset;	/* 0xa8c USB gate/reset control */
	uint8_t reserved_0xa90[32];
	uint32_t pcie_ref_clk_cfg;	/* 0xab0 PCIE REF clock control */
	uint32_t pcie_axi_clk_cfg;	/* 0xab4 PCIE AXI clock control */
	uint32_t pcie_aux_clk_cfg;	/* 0xab8 PCIE AUX clock control */
	uint32_t pcie_gate_reset;	/* 0xabc PCIE gate/reset control */
	uint8_t reserved_0xac0[64];
	uint32_t hdmi_clk_cfg;	/* 0xb00 HDMI clock control */
	uint32_t hdmi_slow_clk_cfg;	/* 0xb04 HDMI slow clock control */
	uint8_t reserved_0xb08[8];
	uint32_t hdmi_cec_clk_cfg;	/* 0xb10 HDMI CEC clock control */
	uint8_t reserved_0xb14[8];
	uint32_t hdmi_gate_reset;	/* 0xb1c HDMI gate/reset control */
	uint8_t reserved_0xb20[60];
	uint32_t tcon_top_gate_reset;/* 0xb5c TCON TOP gate/reset control */
	uint32_t tcon_lcd0_clk_cfg;	/* 0xb60 TCON LCD0 clock control */
	uint8_t reserved_0xb64[24];
	uint32_t tcon_lcd_gate_reset;/* 0xb7c TCON LCD gate/reset control */
	uint32_t tcon_tv0_clk_cfg;	/* 0xb80 TCON TV0 clock control */
	uint8_t reserved_0xb84[24];
	uint32_t tcon_tv_gate_reset;	/* 0xb9c TCON TV gate/reset control */
	uint8_t reserved_0xba0[96];
	uint32_t csi_misc_clk_cfg;	/* 0xc00 CSI MISC clock control */
	uint32_t csi_top_clk_cfg;	/* 0xc04 CSI TOP clock control */
	uint32_t csi_mclk_cfg;	/* 0xc08 CSI Master clock control */
	uint8_t reserved_0xc0c[32];
	uint32_t csi_gate_reset;	/* 0xc2c CSI gate/reset control */
	uint8_t reserved_0xc30[16];
	uint32_t hdcp_clk_cfg;	/* 0xc40 HDCP clock control */
	uint8_t reserved_0xc44[8];
	uint32_t hdcp_gate_reset;	/* 0xc4c HDCP gate/reset control */
	uint8_t reserved_0xc50[688];
	uint32_t ccu_sec_switch;	/* 0xf00 CCU security switch */
	uint32_t pll_lock_dbg_ctrl;	/* 0xf04 PLL lock debugging control */
};

/* pll1 bit field */
#define CCM_PLL1_CTRL_EN		BIT_U32(31)
#define CCM_PLL1_LOCK_EN		BIT_U32(29)
#define CCM_PLL1_LOCK			BIT_U32(28)
#define CCM_PLL1_OUT_EN			BIT_U32(27)
#define CCM_PLL1_CLOCK_TIME_2		(2 << 24)
#define CCM_PLL1_CTRL_P(p)		((p) << 16)
#define CCM_PLL1_CTRL_N(n)		(((n) - 1) << 8)

/* pll5 bit field */
#define CCM_PLL5_CTRL_EN		BIT_U32(31)
#define CCM_PLL5_LOCK_EN		BIT_U32(29)
#define CCM_PLL5_LOCK			BIT_U32(28)
#define CCM_PLL5_OUT_EN			BIT_U32(27)
#define CCM_PLL5_CTRL_N(n)		(((n) - 1) << 8)
#define CCM_PLL5_CTRL_DIV1(div1)	((div1) << 0)
#define CCM_PLL5_CTRL_DIV2(div0)	((div0) << 1)

/* pll6 bit field */
#define CCM_PLL6_CTRL_EN		BIT_U32(31)
#define CCM_PLL6_LOCK_EN		BIT_U32(29)
#define CCM_PLL6_LOCK			BIT_U32(28)
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

#ifdef CONFIG_MACH_SUN50I_H6
#define CCM_PLL6_DEFAULT		0xa0006300

/* psi_ahb1_ahb2 bit field */
#define CCM_PSI_AHB1_AHB2_DEFAULT	0x03000102

/* ahb3 bit field */
#define CCM_AHB3_DEFAULT		0x03000002

/* apb1 bit field */
#define CCM_APB1_DEFAULT		0x03000102
#elif CONFIG_MACH_SUN50I_T507 || CONFIG_MACH_SUN50I_H616 || CONFIG_MACH_SUN50I_H618
#define CCM_PLL6_DEFAULT		0xa8003100

/* psi_ahb1_ahb2 bit field */
#define CCM_PSI_AHB1_AHB2_DEFAULT	0x03000002

/* ahb3 bit field */
#define CCM_AHB3_DEFAULT		0x03000002

/* apb1 bit field */
#define CCM_APB1_DEFAULT		0x03000102
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
#define DRAM_MOD_RESET			BIT_U32(30)
#define DRAM_CLK_UPDATE			BIT_U32(27)
#define DRAM_CLK_SRC_MASK		GENMASK(25, 24)
#define DRAM_CLK_SRC_PLL5		(0 << 24)
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

struct dram_config {
	uint32_t clk;
	enum sunxi_dram_type type;
	uint32_t cols;
	uint32_t rows;
	uint32_t ranks;
	uint32_t bus_full_width;
};


#if CPUSTYLE_T507

#define CONFIG_DRAM_SUN50I_H616_TPR0 0x0
#define CONFIG_DRAM_SUN50I_H616_TPR2 0x0

/* 1GB LPDDR4 @ HelperBoard507 */
//#define CONFIG_DRAM_SUN50I_H616_DX_ODT 0x0c0c0c0c
//#define CONFIG_DRAM_SUN50I_H616_DX_DRI 0x0e0e0e0e
//#define CONFIG_DRAM_SUN50I_H616_CA_DRI 0x0e0e
//#define CONFIG_DRAM_SUN50I_H616_ODT_EN 0x7887bbbb
//#define CONFIG_DRAM_SUN50I_H616_TPR10 0x402e0000
//#define CONFIG_DRAM_SUN50I_H616_TPR11 0x22262622
//#define CONFIG_DRAM_SUN50I_H616_TPR12 0x0b0c0d0b

// https://github.com/wirenboard/u-boot-upstream/blob/4eb7bc820e8ed06ba2e2a1e6fd870060906393be/configs/sun50i_wirenboard8_defconfig#L8C1-L16C1

#define CONFIG_DRAM_SUN50I_H616_DX_ODT 0x07070707
#define CONFIG_DRAM_SUN50I_H616_DX_DRI 0x0e0e0e0e
#define CONFIG_DRAM_SUN50I_H616_CA_DRI 0x0e0e
#define CONFIG_DRAM_SUN50I_H616_ODT_EN 0xaaaaeeee
#define CONFIG_DRAM_SUN50I_H616_TPR6 0x44000000
#define CONFIG_DRAM_SUN50I_H616_TPR10 0x402f6633
#define CONFIG_DRAM_SUN50I_H616_TPR11 0x24242624
#define CONFIG_DRAM_SUN50I_H616_TPR12 0x0f0f100f

#elif CPUSTYLE_A133

// https://github.com/u-boot/u-boot/blob/d33b21b7e261691e8d6613a24cc9b0ececba3b01/configs/liontron-h-a133l_defconfig#L1

//CONFIG_DRAM_SUNXI_DX_ODT=0x7070707
//CONFIG_DRAM_SUNXI_DX_DRI=0xd0d0d0d
//CONFIG_DRAM_SUNXI_CA_DRI=0xe0e
//CONFIG_DRAM_SUNXI_PARA0=0xd0a050c
//CONFIG_DRAM_SUNXI_MR11=0x4
//CONFIG_DRAM_SUNXI_MR12=0x72
//CONFIG_DRAM_SUNXI_MR14=0x7
//CONFIG_DRAM_SUNXI_TPR1=0x26
//CONFIG_DRAM_SUNXI_TPR2=0x6060606
//CONFIG_DRAM_SUNXI_TPR3=0x84040404
//CONFIG_DRAM_SUNXI_TPR6=0x48000000
//CONFIG_DRAM_SUNXI_TPR10=0x273333
//CONFIG_DRAM_SUNXI_TPR11=0x231d151c
//CONFIG_DRAM_SUNXI_TPR12=0x1212110e
//CONFIG_DRAM_SUNXI_TPR13=0x7521
//CONFIG_DRAM_SUNXI_TPR14=0x2023211f

#else
	#error Wriong DRAM config

#endif


#define panic PRINTF
#define debug PRINTF
#define dmb() do { __DMB(); } while (0)

unsigned long sunxi_dram_init(void);
void mctl_set_timing_params(const struct dram_para *para);
int mctl_mem_matches(uint64_t offset);

#endif /* WITHSDRAMHW */
#endif /* SRC_SDRAM_AW_H616_DRAM_GLUE_H_ */
