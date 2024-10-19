/*
 * a64_ddr3.c
 *
 *  Created on: 15 февр. 2023 г.
 *      Author: User
 */

// https://github.com/apritzel/u-boot/blob/3aaabfe9ff4bbcd11096513b1b28d1fb0a40800f/arch/arm/mach-sunxi/dram_sun50i_h616.c#L3
// https://github.com/iuncuim/u-boot/blob/t507-lpddr4/arch/arm/mach-sunxi/dram_sun50i_h616.c

#include "hardware.h"

#if WITHSDRAMHW && CPUSTYLE_T507 && ! CPUSTYLE_H616

#include "formats.h"
#include <string.h>

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

#define CONFIG_MACH_SUN50I_H616 1

#ifdef CONFIG_MACH_SUN50I_H6
	#define SUNXI_DRAM_COM_BASE		0x04002000
	#define SUNXI_DRAM_CTL0_BASE		0x04003000
	#define SUNXI_DRAM_PHY0_BASE		0x04005000
#endif
//#define SUNXI_NFC_BASE			0x04011000
//#define SUNXI_MMC0_BASE			0x04020000
//#define SUNXI_MMC1_BASE			0x04021000
//#define SUNXI_MMC2_BASE			0x04022000
#ifdef CONFIG_MACH_SUN50I_H616
	#define SUNXI_DRAM_COM_BASE		0x047FA000
	#define SUNXI_DRAM_CTL0_BASE		0x047FB000
	#define SUNXI_DRAM_PHY0_BASE		0x04800000
#endif

#define DRAM_CLK_ENABLE			BIT_U32(31)

#define SUNXI_PRCM_BASE			0x07010000

#define SUNXI_CCM_BASE CCU_BASE


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

uint32_t read32(uintptr_t addr)
{
	__DSB();
	return * (volatile uint32_t *) addr;
}

void write32(uintptr_t addr, uint32_t value)
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
	//local_delay_us(1 * loops);
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

#define DIV_ROUND_UP(n, d)	(((n) + (d) - 1) / (d))
#define REPEAT_BYTE(x)		((~0ul / 0xff) * (x))

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


static inline int ns_to_t(int nanoseconds)
{
	const unsigned int ctrl_freq = BOARD_CONFIG_DRAM_CLK / 2;

	return DIV_ROUND_UP(ctrl_freq * (uint_fast64_t) nanoseconds, 1000);
}


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
#elif CONFIG_MACH_SUN50I_H616
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


/*
 * Wait up to 1s for value to be set in given part of reg.
 */
void mctl_await_completion(uint32_t *reg, uint32_t mask, uint32_t val)
{
	//unsigned long tmo = timer_get_us() + 1000000;

	while ((readl(reg) & mask) != val) {
//		if (timer_get_us() > tmo)
//			PRINTF("Timeout initialising DRAM\n");
	}
}


#if 1//CPUSTYLE_H616

// https://github.com/iuncuim/u-boot/blob/t507-lpddr4/arch/arm/mach-sunxi/dram_timings/h616_ddr3_1333.c

#define max(a,b) ((a) > (b) ? (a) : (b))

void mctl_set_timing_params(const struct dram_para *para)
{
	struct sunxi_mctl_ctl_reg * const mctl_ctl =
			(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;

	uint8_t tccd		= 4;			/* JEDEC: 4nCK */
	uint8_t tfaw		= ns_to_t(36);		/* JEDEC: 30 ns w/ 1K pages */
	uint8_t trrd		= max(ns_to_t(6), 4);	/* JEDEC: max(6 ns, 4nCK) */
	uint8_t trcd		= ns_to_t(17);		/* JEDEC: 13.5 ns */
	uint8_t trc		= ns_to_t(60);		/* JEDEC: 49.5 ns */
	uint8_t txp		= max(ns_to_t(6), 3);	/* JEDEC: max(6 ns, 3nCK) */
	uint8_t trtp		= max(ns_to_t(8), 4);	/* JEDEC: max(7.5 ns, 4nCK) */
	uint8_t trp		= ns_to_t(20);		/* JEDEC: >= 13.75 ns */
	uint8_t tras		= ns_to_t(40);		/* JEDEC >= 36 ns, <= 9*trefi */
	uint16_t trefi	= ns_to_t(7800) / 32;	/* JEDEC: 7.8us@Tcase <= 85C */
	uint16_t trfc	= ns_to_t(350);		/* JEDEC: 160 ns for 2Gb */
	uint16_t txsr	= 69;			/* ? */

	uint8_t tmrw		= 6;			/* ? */
	uint8_t tmrd		= 6;			/* JEDEC: 4nCK */
	uint8_t tmod		= max(ns_to_t(15), 12);	/* JEDEC: max(15 ns, 12nCK) */
	uint8_t tcke		= max(ns_to_t(6), 6);	/* JEDEC: max(5.625 ns, 3nCK) */
	uint8_t tcksrx	= max(ns_to_t(5), 2);	/* JEDEC: max(10 ns, 5nCK) */
	uint8_t tcksre	= max(ns_to_t(5), 2);	/* JEDEC: max(10 ns, 5nCK) */
	uint8_t tckesr	= tcke;		/* JEDEC: tCKE(min) + 1nCK */
	uint8_t trasmax	= (para->clk / 2) / 32;	/* JEDEC: tREFI * 9 */
	uint8_t txs		= ns_to_t(380) / 32;	/* JEDEC: max(5nCK,tRFC+10ns) */
	uint8_t txsdll	= 16;			/* JEDEC: 512 nCK */
	uint8_t txsabort	= 4;			/* ? */
	uint8_t txsfast	= 4;			/* ? */
	uint8_t tcl		= 10;			/* JEDEC: CL / 2 => 6 */
	uint8_t tcwl		= 5;			/* JEDEC: 8 */
	uint8_t t_rdata_en	= 9;			/* ? */

	uint8_t twtp		= 24;			/* (WL + BL / 2 + tWR) / 2 */
	uint8_t twr2rd	= trtp + 14;		/* (WL + BL / 2 + tWTR) / 2 */
	uint8_t trd2wr	= 18;			/* (RL + BL / 2 + 2 - WL) / 2 */

	/* set DRAM timing */
	writel((twtp << 24) | (tfaw << 16) | (trasmax << 8) | tras,
	       &mctl_ctl->dramtmg[0]);
	writel((txp << 16) | (trtp << 8) | trc, &mctl_ctl->dramtmg[1]);
	writel((tcwl << 24) | (tcl << 16) | (trd2wr << 8) | twr2rd,
	       &mctl_ctl->dramtmg[2]);
	writel((tmrw << 20) | (tmrd << 12) | tmod, &mctl_ctl->dramtmg[3]);
	writel((trcd << 24) | (tccd << 16) | (trrd << 8) | trp,
	       &mctl_ctl->dramtmg[4]);
	writel((tcksrx << 24) | (tcksre << 16) | (tckesr << 8) | tcke,
	       &mctl_ctl->dramtmg[5]);
	/* Value suggested by ZynqMP manual and used by libdram */
	writel((txp + 2) | 0x02020000, &mctl_ctl->dramtmg[6]);
	writel((txsfast << 24) | (txsabort << 16) | (txsdll << 8) | txs,
	       &mctl_ctl->dramtmg[8]);
	writel(0x00020208, &mctl_ctl->dramtmg[9]);
	writel(0xE0C05, &mctl_ctl->dramtmg[10]);
	writel(0x440C021C, &mctl_ctl->dramtmg[11]);
	writel(8, &mctl_ctl->dramtmg[12]);
	writel(0xA100002, &mctl_ctl->dramtmg[13]);
	writel(txsr, &mctl_ctl->dramtmg[14]);

	writel(0x203f0, &mctl_ctl->init[0]);
	writel(0x1f20000, &mctl_ctl->init[1]);
	writel(0xd05, &mctl_ctl->init[2]);
	writel(0x0034001b, &mctl_ctl->init[3]);
	writel(0x00330000, &mctl_ctl->init[4]);
	writel(0x00100004, &mctl_ctl->init[5]);
	writel(0x00040072, &mctl_ctl->init[6]);
	writel(0x00240009, &mctl_ctl->init[7]);

	writel(0, &mctl_ctl->dfimisc);
	clrsetbits_le32(&mctl_ctl->rankctl, 0xff0, 0x660);

	/* Configure DFI timing */
	writel(0x02918005, &mctl_ctl->dfitmg0);
	writel(0x100202, &mctl_ctl->dfitmg1);

	/* set refresh timing */
	writel(0x002b0041, &mctl_ctl->rfshtmg);
}
#endif
/*
 * Test if memory at offset offset matches memory at begin of DRAM
 *
 * Note: dsb() is not available on ARMv5 in Thumb mode
 */
#ifndef CONFIG_MACH_SUNIV
int mctl_mem_matches(uint32_t offset)
{
	/* Try to write different values to RAM at two addresses */
	writel(0, CONFIG_SYS_SDRAM_BASE);
	writel(0xaa55aa55, (unsigned long)CONFIG_SYS_SDRAM_BASE + offset);
	__DSB();
	/* Check if the same value is actually observed when reading back */
	return readl(CONFIG_SYS_SDRAM_BASE) ==
	       readl((unsigned long)CONFIG_SYS_SDRAM_BASE + offset);
}
#endif
// SPDX-License-Identifier: GPL-2.0+
/*
 * sun50i H616 platform dram controller driver
 *
 * While controller is very similar to that in H6, PHY is completely
 * unknown. That's why this driver has plenty of magic numbers. Some
 * meaning was nevertheless deduced from strings found in boot0 and
 * known meaning of some dram parameters.
 * This driver only supports DDR3 memory and omits logic for all
 * other supported types supported by hardware.
 *
 * (C) Copyright 2020 Jernej Skrabec <jernej.skrabec@siol.net>
 *
 */
//#include <common.h>
//#include <init.h>
//#include <log.h>
//#include <asm/io.h>
//#include <asm/arch/clock.h>
//#include <asm/arch/dram.h>
//#include <asm/arch/cpu.h>
//#include <asm/arch/prcm.h>
//#include <linux/bitops.h>
//#include <linux/delay.h>
//#include <linux/kconfig.h>


enum {
	MBUS_QOS_LOWEST = 0,
	MBUS_QOS_LOW,
	MBUS_QOS_HIGH,
	MBUS_QOS_HIGHEST
};

static void mbus_configure_port(uint8_t port,
				int bwlimit,
				int priority,
				uint8_t qos,
				uint8_t waittime,
				uint8_t acs,
				uint16_t bwl0,
				uint16_t bwl1,
				uint16_t bwl2)
{
	struct sunxi_mctl_com_reg * const mctl_com =
			(struct sunxi_mctl_com_reg *)SUNXI_DRAM_COM_BASE;

	const uint32_t cfg0 = ( (bwlimit ? (1 << 0) : 0)
			   | (priority ? (1 << 1) : 0)
			   | ((qos & 0x3) << 2)
			   | ((waittime & 0xf) << 4)
			   | ((acs & 0xff) << 8)
			   | (bwl0 << 16) );
	const uint32_t cfg1 = ((uint32_t)bwl2 << 16) | (bwl1 & 0xffff);

	PRINTF("MBUS port %d cfg0 %08x cfg1 %08x\n", (int) port, (unsigned) cfg0, (unsigned) cfg1);
	writel_relaxed(cfg0, &mctl_com->master[port].cfg0);
	writel_relaxed(cfg1, &mctl_com->master[port].cfg1);
}

#define MBUS_CONF(port, bwlimit, qos, acs, bwl0, bwl1, bwl2)	\
	mbus_configure_port(port, bwlimit, (1 == 0), \
			    MBUS_QOS_ ## qos, 0, acs, bwl0, bwl1, bwl2)

static void mctl_set_master_priority(void)
{
	struct sunxi_mctl_com_reg * const mctl_com =
			(struct sunxi_mctl_com_reg *)SUNXI_DRAM_COM_BASE;

	/* enable bandwidth limit windows and set windows size 1us */
	writel(399, &mctl_com->tmr);
	writel(BIT_U32(16), &mctl_com->bwcr);

	MBUS_CONF( 0, (1 == 1), HIGHEST, 0,  256,  128,  100);
	MBUS_CONF( 1, (1 == 1),    HIGH, 0, 1536, 1400,  256);
	MBUS_CONF( 2, (1 == 1), HIGHEST, 0,  512,  256,   96);
	MBUS_CONF( 3, (1 == 1),    HIGH, 0,  256,  100,   80);
	MBUS_CONF( 4, (1 == 1),    HIGH, 2, 8192, 5500, 5000);
	MBUS_CONF( 5, (1 == 1),    HIGH, 2,  100,   64,   32);
	MBUS_CONF( 6, (1 == 1),    HIGH, 2,  100,   64,   32);
	MBUS_CONF( 8, (1 == 1),    HIGH, 0,  256,  128,   64);
	MBUS_CONF(11, (1 == 1),    HIGH, 0,  256,  128,  100);
	MBUS_CONF(14, (1 == 1),    HIGH, 0, 1024,  256,   64);
	MBUS_CONF(16, (1 == 1), HIGHEST, 6, 8192, 2800, 2400);
	MBUS_CONF(21, (1 == 1), HIGHEST, 6, 2048,  768,  512);
	MBUS_CONF(25, (1 == 1), HIGHEST, 0,  100,   64,   32);
	MBUS_CONF(26, (1 == 1),    HIGH, 2, 8192, 5500, 5000);
	MBUS_CONF(37, (1 == 1),    HIGH, 0,  256,  128,   64);
	MBUS_CONF(38, (1 == 1),    HIGH, 2,  100,   64,   32);
	MBUS_CONF(39, (1 == 1),    HIGH, 2, 8192, 5500, 5000);
	MBUS_CONF(40, (1 == 1),    HIGH, 2,  100,   64,   32);

	__DMB();
}

static void mctl_sys_init(uint32_t clk_rate)
{
	struct sunxi_ccm_reg * const ccm =
			(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	struct sunxi_mctl_com_reg * const mctl_com =
			(struct sunxi_mctl_com_reg *)SUNXI_DRAM_COM_BASE;
	struct sunxi_mctl_ctl_reg * const mctl_ctl =
			(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;

	/* Put all DRAM-related blocks to reset state */
	clrbits_le32(&ccm->mbus_cfg, MBUS_ENABLE);
	clrbits_le32(&ccm->mbus_cfg, MBUS_RESET);
	clrbits_le32(&ccm->dram_gate_reset, BIT_U32(GATE_SHIFT));
	udelay(5);
	clrbits_le32(&ccm->dram_gate_reset, BIT_U32(RESET_SHIFT));
	clrbits_le32(&ccm->pll5_cfg, CCM_PLL5_CTRL_EN);
	clrbits_le32(&ccm->dram_clk_cfg, DRAM_MOD_RESET);

	udelay(5);

	/* Set PLL5 rate to doubled DRAM clock rate */
	writel(CCM_PLL5_CTRL_EN | CCM_PLL5_LOCK_EN | CCM_PLL5_OUT_EN |
	       CCM_PLL5_CTRL_N(clk_rate * 2 / 24), &ccm->pll5_cfg);
	mctl_await_completion(&ccm->pll5_cfg, CCM_PLL5_LOCK, CCM_PLL5_LOCK);

	/* Configure DRAM mod clock */
	writel(DRAM_CLK_SRC_PLL5, &ccm->dram_clk_cfg);
	writel(BIT_U32(RESET_SHIFT), &ccm->dram_gate_reset);
	udelay(5);
	setbits_le32(&ccm->dram_gate_reset, BIT_U32(GATE_SHIFT));

	/* Disable all channels */
	writel(0, &mctl_com->maer0);
	writel(0, &mctl_com->maer1);
	writel(0, &mctl_com->maer2);

	/* Configure MBUS and enable DRAM mod reset */
	setbits_le32(&ccm->mbus_cfg, MBUS_RESET);
	setbits_le32(&ccm->mbus_cfg, MBUS_ENABLE);

	clrbits_le32(&mctl_com->unk_0x500, BIT_U32(25));

	setbits_le32(&ccm->dram_clk_cfg, DRAM_MOD_RESET);
	udelay(5);

	/* Unknown hack, which enables access of mctl_ctl regs */
	writel(0x8000, &mctl_ctl->clken);
}

static void mctl_set_addrmap(const struct dram_config *config)
{
	struct sunxi_mctl_ctl_reg * const mctl_ctl =
			(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;
	uint8_t cols = config->cols;
	uint8_t rows = config->rows;
	uint8_t ranks = config->ranks;

	if (!config->bus_full_width)
		cols -= 1;

	/* Ranks */
	if (ranks == 2)
		mctl_ctl->addrmap[0] = rows + cols - 3;
	else
		mctl_ctl->addrmap[0] = 0x1F;

	/* Banks, hardcoded to 8 banks now */
	mctl_ctl->addrmap[1] = (cols - 2) | (cols - 2) << 8 | (cols - 2) << 16;

	/* Columns */
	mctl_ctl->addrmap[2] = 0;
	switch (cols) {
	case 7:
		mctl_ctl->addrmap[3] = 0x1F1F1F00;
		mctl_ctl->addrmap[4] = 0x1F1F;
		break;
	case 8:
		mctl_ctl->addrmap[3] = 0x1F1F0000;
		mctl_ctl->addrmap[4] = 0x1F1F;
		break;
	case 9:
		mctl_ctl->addrmap[3] = 0x1F000000;
		mctl_ctl->addrmap[4] = 0x1F1F;
		break;
	case 10:
		mctl_ctl->addrmap[3] = 0;
		mctl_ctl->addrmap[4] = 0x1F1F;
		break;
	case 11:
		mctl_ctl->addrmap[3] = 0;
		mctl_ctl->addrmap[4] = 0x1F00;
		break;
	case 12:
		mctl_ctl->addrmap[3] = 0;
		mctl_ctl->addrmap[4] = 0;
		break;
	default:
		PRINTF("Unsupported DRAM configuration: column number invalid\n");
		for (;;)
			;
	}

	/* Rows */
	mctl_ctl->addrmap[5] = (cols - 3) | ((cols - 3) << 8) | ((cols - 3) << 16) | ((cols - 3) << 24);
	switch (rows) {
	case 13:
		mctl_ctl->addrmap[6] = (cols - 3) | 0x0F0F0F00;
		mctl_ctl->addrmap[7] = 0x0F0F;
		break;
	case 14:
		mctl_ctl->addrmap[6] = (cols - 3) | ((cols - 3) << 8) | 0x0F0F0000;
		mctl_ctl->addrmap[7] = 0x0F0F;
		break;
	case 15:
		mctl_ctl->addrmap[6] = (cols - 3) | ((cols - 3) << 8) | ((cols - 3) << 16) | 0x0F000000;
		mctl_ctl->addrmap[7] = 0x0F0F;
		break;
	case 16:
		mctl_ctl->addrmap[6] = (cols - 3) | ((cols - 3) << 8) | ((cols - 3) << 16) | ((cols - 3) << 24);
		mctl_ctl->addrmap[7] = 0x0F0F;
		break;
	case 17:
		mctl_ctl->addrmap[6] = (cols - 3) | ((cols - 3) << 8) | ((cols - 3) << 16) | ((cols - 3) << 24);
		mctl_ctl->addrmap[7] = (cols - 3) | 0x0F00;
		break;
	case 18:
		mctl_ctl->addrmap[6] = (cols - 3) | ((cols - 3) << 8) | ((cols - 3) << 16) | ((cols - 3) << 24);
		mctl_ctl->addrmap[7] = (cols - 3) | ((cols - 3) << 8);
		break;
	default:
		PRINTF("Unsupported DRAM configuration: row number invalid\n");
		for (;;)
			;
	}

	/* Bank groups, DDR4 only */
	mctl_ctl->addrmap[8] = 0x3F3F;
}

static const uint8_t phy_init[] = {
#ifdef CONFIG_SUNXI_DRAM_H616_DDR3_1333
	0x07, 0x0b, 0x02, 0x16, 0x0d, 0x0e, 0x14, 0x19,
	0x0a, 0x15, 0x03, 0x13, 0x04, 0x0c, 0x10, 0x06,
	0x0f, 0x11, 0x1a, 0x01, 0x12, 0x17, 0x00, 0x08,
	0x09, 0x05, 0x18
#elif defined(CONFIG_SUNXI_DRAM_H616_LPDDR3)
	0x18, 0x06, 0x00, 0x05, 0x04, 0x03, 0x09, 0x02,
	0x08, 0x01, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x07,
	0x17, 0x19, 0x1a
#elif defined(CONFIG_SUNXI_DRAM_H616_LPDDR4)
	0x03, 0x00, 0x17, 0x05, 0x02, 0x19, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x01,
	0x18, 0x04, 0x1a
#endif
};

static void mctl_phy_configure_odt(const struct dram_para *para)
{
	unsigned int val;

	val = para->dx_dri & 0x1f;
	writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x388);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
		writel_relaxed(4, SUNXI_DRAM_PHY0_BASE + 0x38c);
	else
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x38c);

	val = (para->dx_dri >> 8) & 0x1f;
	writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x3c8);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
		writel_relaxed(4, SUNXI_DRAM_PHY0_BASE + 0x3cc);
	else
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x3cc);

	val = (para->dx_dri >> 16) & 0x1f;
	writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x408);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
		writel_relaxed(4, SUNXI_DRAM_PHY0_BASE + 0x40c);
	else
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x40c);

	val = (para->dx_dri >> 24) & 0x1f;
	writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x448);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
		writel_relaxed(4, SUNXI_DRAM_PHY0_BASE + 0x44c);
	else
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x44c);

	val = para->ca_dri & 0x1f;
	writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x340);
	writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x344);

	val = (para->ca_dri >> 8) & 0x1f;
	writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x348);
	writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x34c);

	val = para->dx_odt & 0x1f;
	if (para->type == SUNXI_DRAM_TYPE_LPDDR3)
		writel_relaxed(0, SUNXI_DRAM_PHY0_BASE + 0x380);
	else
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x380);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
		writel_relaxed(0, SUNXI_DRAM_PHY0_BASE + 0x384);
	else
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x384);

	val = (para->dx_odt >> 8) & 0x1f;
	if (para->type == SUNXI_DRAM_TYPE_LPDDR3)
		writel_relaxed(0, SUNXI_DRAM_PHY0_BASE + 0x3c0);
	else
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x3c0);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
		writel_relaxed(0, SUNXI_DRAM_PHY0_BASE + 0x3c4);
	else
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x3c4);

	val = (para->dx_odt >> 16) & 0x1f;
	if (para->type == SUNXI_DRAM_TYPE_LPDDR3)
		writel_relaxed(0, SUNXI_DRAM_PHY0_BASE + 0x400);
	else
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x400);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
		writel_relaxed(0, SUNXI_DRAM_PHY0_BASE + 0x404);
	else
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x404);

	val = (para->dx_odt >> 24) & 0x1f;
	if (para->type == SUNXI_DRAM_TYPE_LPDDR3)
		writel_relaxed(0, SUNXI_DRAM_PHY0_BASE + 0x440);
	else
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x440);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
		writel_relaxed(0, SUNXI_DRAM_PHY0_BASE + 0x444);
	else
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x444);

	__DMB();
}

static int mctl_phy_write_leveling(const struct dram_config *config)
{
	int result = (1 == 1);
	uint32_t val;

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0xc0, 0x80);
	writel(4, SUNXI_DRAM_PHY0_BASE + 0xc);
	writel(0x40, SUNXI_DRAM_PHY0_BASE + 0x10);

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 4);

	if (config->bus_full_width)
		val = 0xf;
	else
		val = 3;

	mctl_await_completion((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x188), val, val);

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 4);

	val = readl(SUNXI_DRAM_PHY0_BASE + 0x258);
	if (val == 0 || val == 0x3f)
		result = (1 == 0);
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x25c);
	if (val == 0 || val == 0x3f)
		result = (1 == 0);
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x318);
	if (val == 0 || val == 0x3f)
		result = (1 == 0);
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x31c);
	if (val == 0 || val == 0x3f)
		result = (1 == 0);

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0xc0);

	if (config->ranks == 2) {
		clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0xc0, 0x40);

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 4);

		if (config->bus_full_width)
			val = 0xf;
		else
			val = 3;

		mctl_await_completion((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x188), val, val);

		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 4);
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0xc0);

	return result;
}

static int mctl_phy_read_calibration(const struct dram_config *config)
{
	int result = (1 == 1);
	uint32_t val, tmp;

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30, 0x20);

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);

	if (config->bus_full_width)
		val = 0xf;
	else
		val = 3;

	while ((readl(SUNXI_DRAM_PHY0_BASE + 0x184) & val) != val) {
		if (readl(SUNXI_DRAM_PHY0_BASE + 0x184) & 0x20) {
			result = (1 == 0);
			break;
		}
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30);

	if (config->ranks == 2) {
		clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30, 0x10);

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);

		while ((readl(SUNXI_DRAM_PHY0_BASE + 0x184) & val) != val) {
			if (readl(SUNXI_DRAM_PHY0_BASE + 0x184) & 0x20) {
				result = (1 == 0);
				break;
			}
		}

		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30);

	val = readl(SUNXI_DRAM_PHY0_BASE + 0x274) & 7;
	tmp = readl(SUNXI_DRAM_PHY0_BASE + 0x26c) & 7;
	if (val < tmp)
		val = tmp;
	tmp = readl(SUNXI_DRAM_PHY0_BASE + 0x32c) & 7;
	if (val < tmp)
		val = tmp;
	tmp = readl(SUNXI_DRAM_PHY0_BASE + 0x334) & 7;
	if (val < tmp)
		val = tmp;
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x38, 0x7, (val + 2) & 7);

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 4, 0x20);

	return result;
}

static int mctl_phy_read_training(const struct dram_config *config)
{
	uint32_t val1, val2, *ptr1, *ptr2;
	int result = (1 == 1);
	int i;

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 3, 2);
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x804, 0x3f, 0xf);
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x808, 0x3f, 0xf);
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0xa04, 0x3f, 0xf);
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0xa08, 0x3f, 0xf);

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 6);
	setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 1);

	mctl_await_completion((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x840), 0xc, 0xc);
	if (readl(SUNXI_DRAM_PHY0_BASE + 0x840) & 3)
		result = (1 == 0);

	if (config->bus_full_width) {
		mctl_await_completion((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xa40), 0xc, 0xc);
		if (readl(SUNXI_DRAM_PHY0_BASE + 0xa40) & 3)
			result = (1 == 0);
	}

	ptr1 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x898);
	ptr2 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x850);
	for (i = 0; i < 9; i++) {
		val1 = readl(&ptr1[i]);
		val2 = readl(&ptr2[i]);
		if (val1 - val2 <= 6)
			result = (1 == 0);
	}
	ptr1 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x8bc);
	ptr2 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x874);
	for (i = 0; i < 9; i++) {
		val1 = readl(&ptr1[i]);
		val2 = readl(&ptr2[i]);
		if (val1 - val2 <= 6)
			result = (1 == 0);
	}

	if (config->bus_full_width) {
		ptr1 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xa98);
		ptr2 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xa50);
		for (i = 0; i < 9; i++) {
			val1 = readl(&ptr1[i]);
			val2 = readl(&ptr2[i]);
			if (val1 - val2 <= 6)
				result = (1 == 0);
		}

		ptr1 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xabc);
		ptr2 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xa74);
		for (i = 0; i < 9; i++) {
			val1 = readl(&ptr1[i]);
			val2 = readl(&ptr2[i]);
			if (val1 - val2 <= 6)
				result = (1 == 0);
		}
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 3);

	if (config->ranks == 2) {
		/* maybe last parameter should be 1? */
		clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 3, 2);

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 6);
		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 1);

		mctl_await_completion((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x840), 0xc, 0xc);
		if (readl(SUNXI_DRAM_PHY0_BASE + 0x840) & 3)
			result = (1 == 0);

		if (config->bus_full_width) {
			mctl_await_completion((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xa40), 0xc, 0xc);
			if (readl(SUNXI_DRAM_PHY0_BASE + 0xa40) & 3)
				result = (1 == 0);
		}

		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 3);
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 3);

	return result;
}

static int mctl_phy_write_training(const struct dram_config *config)
{
	uint32_t val1, val2, *ptr1, *ptr2;
	int result = (1 == 1);
	int i;

	writel(0, SUNXI_DRAM_PHY0_BASE + 0x134);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x138);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x19c);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x1a0);

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 0xc, 8);

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x10);
	setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x20);

	mctl_await_completion((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x8e0), 3, 3);
	if (readl(SUNXI_DRAM_PHY0_BASE + 0x8e0) & 0xc)
		result = (1 == 0);

	if (config->bus_full_width) {
		mctl_await_completion((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xae0), 3, 3);
		if (readl(SUNXI_DRAM_PHY0_BASE + 0xae0) & 0xc)
			result = (1 == 0);
	}

	ptr1 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x938);
	ptr2 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x8f0);
	for (i = 0; i < 9; i++) {
		val1 = readl(&ptr1[i]);
		val2 = readl(&ptr2[i]);
		if (val1 - val2 <= 6)
			result = (1 == 0);
	}
	ptr1 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x95c);
	ptr2 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x914);
	for (i = 0; i < 9; i++) {
		val1 = readl(&ptr1[i]);
		val2 = readl(&ptr2[i]);
		if (val1 - val2 <= 6)
			result = (1 == 0);
	}

	if (config->bus_full_width) {
		ptr1 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xb38);
		ptr2 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xaf0);
		for (i = 0; i < 9; i++) {
			val1 = readl(&ptr1[i]);
			val2 = readl(&ptr2[i]);
			if (val1 - val2 <= 6)
				result = (1 == 0);
		}
		ptr1 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xb5c);
		ptr2 = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xb14);
		for (i = 0; i < 9; i++) {
			val1 = readl(&ptr1[i]);
			val2 = readl(&ptr2[i]);
			if (val1 - val2 <= 6)
				result = (1 == 0);
		}
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x60);

	if (config->ranks == 2) {
		clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 0xc, 4);

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x10);
		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x20);

		mctl_await_completion((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x8e0), 3, 3);
		if (readl(SUNXI_DRAM_PHY0_BASE + 0x8e0) & 0xc)
			result = (1 == 0);

		if (config->bus_full_width) {
			mctl_await_completion((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xae0), 3, 3);
			if (readl(SUNXI_DRAM_PHY0_BASE + 0xae0) & 0xc)
				result = (1 == 0);
		}

		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x60);
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 0xc);

	return result;
}

static void mctl_phy_bit_delay_compensation(const struct dram_para *para)
{
	uint32_t *ptr, val;
	int i;

	if (para->tpr10 & TPR10_DX_BIT_DELAY1) {
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, 1);
		setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 8);
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x10);

		if (para->tpr10 & BIT_U32(30))
			val = para->tpr11 & 0x3f;
		else
			val = (para->tpr11 & 0xf) << 1;

		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x484);
		for (i = 0; i < 9; i++) {
			writel_relaxed(val, ptr);
			writel_relaxed(val, ptr + 0x30);
			ptr += 2;
		}

		if (para->tpr10 & BIT_U32(30))
			val = (para->odt_en >> 15) & 0x1e;
		else
			val = (para->tpr11 >> 15) & 0x1e;

		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x4d0);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x590);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x4cc);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x58c);

		if (para->tpr10 & BIT_U32(30))
			val = (para->tpr11 >> 8) & 0x3f;
		else
			val = (para->tpr11 >> 3) & 0x1e;

		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x4d8);
		for (i = 0; i < 9; i++) {
			writel_relaxed(val, ptr);
			writel_relaxed(val, ptr + 0x30);
			ptr += 2;
		}

		if (para->tpr10 & BIT_U32(30))
			val = (para->odt_en >> 19) & 0x1e;
		else
			val = (para->tpr11 >> 19) & 0x1e;

		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x524);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x5e4);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x520);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x5e0);

		if (para->tpr10 & BIT_U32(30))
			val = (para->tpr11 >> 16) & 0x3f;
		else
			val = (para->tpr11 >> 7) & 0x1e;

		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x604);
		for (i = 0; i < 9; i++) {
			writel_relaxed(val, ptr);
			writel_relaxed(val, ptr + 0x30);
			ptr += 2;
		}

		if (para->tpr10 & BIT_U32(30))
			val = (para->odt_en >> 23) & 0x1e;
		else
			val = (para->tpr11 >> 23) & 0x1e;

		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x650);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x710);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x64c);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x70c);

		if (para->tpr10 & BIT_U32(30))
			val = (para->tpr11 >> 24) & 0x3f;
		else
			val = (para->tpr11 >> 11) & 0x1e;

		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x658);
		for (i = 0; i < 9; i++) {
			writel_relaxed(val, ptr);
			writel_relaxed(val, ptr + 0x30);
			ptr += 2;
		}

		if (para->tpr10 & BIT_U32(30))
			val = (para->odt_en >> 27) & 0x1e;
		else
			val = (para->tpr11 >> 27) & 0x1e;

		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x6a4);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x764);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x6a0);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x760);

		__DMB();

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, 1);
	}

	if (para->tpr10 & TPR10_DX_BIT_DELAY0) {
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x54, 0x80);
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 4);

		if (para->tpr10 & BIT_U32(30))
			val = para->tpr12 & 0x3f;
		else
			val = (para->tpr12 & 0xf) << 1;

		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x480);
		for (i = 0; i < 9; i++) {
			writel_relaxed(val, ptr);
			writel_relaxed(val, ptr + 0x30);
			ptr += 2;
		}

		if (para->tpr10 & BIT_U32(30))
			val = (para->odt_en << 1) & 0x1e;
		else
			val = (para->tpr12 >> 15) & 0x1e;

		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x528);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x5e8);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x4c8);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x588);

		if (para->tpr10 & BIT_U32(30))
			val = (para->tpr12 >> 8) & 0x3f;
		else
			val = (para->tpr12 >> 3) & 0x1e;

		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x4d4);
		for (i = 0; i < 9; i++) {
			writel_relaxed(val, ptr);
			writel_relaxed(val, ptr + 0x30);
			ptr += 2;
		}

		if (para->tpr10 & BIT_U32(30))
			val = (para->odt_en >> 3) & 0x1e;
		else
			val = (para->tpr12 >> 19) & 0x1e;

		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x52c);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x5ec);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x51c);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x5dc);

		if (para->tpr10 & BIT_U32(30))
			val = (para->tpr12 >> 16) & 0x3f;
		else
			val = (para->tpr12 >> 7) & 0x1e;

		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x600);
		for (i = 0; i < 9; i++) {
			writel_relaxed(val, ptr);
			writel_relaxed(val, ptr + 0x30);
			ptr += 2;
		}

		if (para->tpr10 & BIT_U32(30))
			val = (para->odt_en >> 7) & 0x1e;
		else
			val = (para->tpr12 >> 23) & 0x1e;

		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x6a8);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x768);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x648);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x708);

		if (para->tpr10 & BIT_U32(30))
			val = (para->tpr12 >> 24) & 0x3f;
		else
			val = (para->tpr12 >> 11) & 0x1e;

		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x654);
		for (i = 0; i < 9; i++) {
			writel_relaxed(val, ptr);
			writel_relaxed(val, ptr + 0x30);
			ptr += 2;
		}

		if (para->tpr10 & BIT_U32(30))
			val = (para->odt_en >> 11) & 0x1e;
		else
			val = (para->tpr12 >> 27) & 0x1e;

		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x6ac);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x76c);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x69c);
		writel_relaxed(val, SUNXI_DRAM_PHY0_BASE + 0x75c);

		__DMB();

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x54, 0x80);
	}
}

static void mctl_phy_ca_bit_delay_compensation(const struct dram_para *para,
					       const struct dram_config *config)
{
	uint32_t val, *ptr;
	int i;

	if (para->tpr0 & BIT_U32(30))
		val = (para->tpr0 >> 7) & 0x3e;
	else
		val = (para->tpr10 >> 3) & 0x1e;

	ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x780);
	for (i = 0; i < 32; i++)
		writel(val, &ptr[i]);

	val = (para->tpr10 << 1) & 0x1e;
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x7d8);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x7dc);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x7e0);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x7f4);

	val = (para->tpr10 >> 7) & 0x1e;
	switch (para->type) {
	case SUNXI_DRAM_TYPE_DDR3:
		if (para->tpr2 & 1) {
			writel(val, SUNXI_DRAM_PHY0_BASE + 0x794);
			if (config->ranks == 2) {
				val = (para->tpr10 >> 11) & 0x1e;
				writel(val, SUNXI_DRAM_PHY0_BASE + 0x7e4);
			}
			if (para->tpr0 & BIT_U32(31)) {
				val = (para->tpr0 << 1) & 0x3e;
				writel(val, SUNXI_DRAM_PHY0_BASE + 0x790);
				writel(val, SUNXI_DRAM_PHY0_BASE + 0x7b8);
				writel(val, SUNXI_DRAM_PHY0_BASE + 0x7cc);
			}
		} else {
			writel(val, SUNXI_DRAM_PHY0_BASE + 0x7d4);
			if (config->ranks == 2) {
				val = (para->tpr10 >> 11) & 0x1e;
				writel(val, SUNXI_DRAM_PHY0_BASE + 0x79c);
			}
			if (para->tpr0 & BIT_U32(31)) {
				val = (para->tpr0 << 1) & 0x3e;
				writel(val, SUNXI_DRAM_PHY0_BASE + 0x78c);
				writel(val, SUNXI_DRAM_PHY0_BASE + 0x7a4);
				writel(val, SUNXI_DRAM_PHY0_BASE + 0x7b8);
			}
		}
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		if (para->tpr2 & 1) {
			writel(val, SUNXI_DRAM_PHY0_BASE + 0x7a0);
			if (config->ranks == 2) {
				val = (para->tpr10 >> 11) & 0x1e;
				writel(val, SUNXI_DRAM_PHY0_BASE + 0x79c);
			}
		} else {
			writel(val, SUNXI_DRAM_PHY0_BASE + 0x7e8);
			if (config->ranks == 2) {
				val = (para->tpr10 >> 11) & 0x1e;
				writel(val, SUNXI_DRAM_PHY0_BASE + 0x7f8);
			}
		}
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		if (para->tpr2 & 1) {
			writel(val, SUNXI_DRAM_PHY0_BASE + 0x788);
		} else {
			writel(val, SUNXI_DRAM_PHY0_BASE + 0x794);
		};
		break;
	};
}

static int mctl_phy_init(const struct dram_para *para,
			  const struct dram_config *config)
{
	struct sunxi_mctl_com_reg * const mctl_com =
			(struct sunxi_mctl_com_reg *)SUNXI_DRAM_COM_BASE;
	struct sunxi_mctl_ctl_reg * const mctl_ctl =
			(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;
	uint32_t val, val2 = 0, *ptr, mr0, mr2;
	int i;

	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x4,0x80);

	if (config->bus_full_width)
		val = 0xf;
	else
		val = 3;
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x3c, 0xf, val);

	switch (para->type) {
	case SUNXI_DRAM_TYPE_DDR3:
		if (para->tpr2 & 0x100) {
			val = 9;
			val2 = 7;
		} else {
			val = 13;
			val2 = 9;
		}
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		if (para->tpr2 & 0x100) {
			val = 12;
			val2 = 6;
		} else {
			val = 14;
			val2 = 8;
		}
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		val = 20;
		val2 = 10;
		break;
	};

	writel(val, SUNXI_DRAM_PHY0_BASE + 0x14);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x35c);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x368);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x374);

	writel(0, SUNXI_DRAM_PHY0_BASE + 0x18);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x360);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x36c);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x378);

	writel(val2, SUNXI_DRAM_PHY0_BASE + 0x1c);
	writel(val2, SUNXI_DRAM_PHY0_BASE + 0x364);
	writel(val2, SUNXI_DRAM_PHY0_BASE + 0x370);
	writel(val2, SUNXI_DRAM_PHY0_BASE + 0x37c);

	ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xc0);
	for (i = 0; i < ARRAY_SIZE(phy_init); i++)
		writel(phy_init[i], &ptr[i]);

	if (para->tpr10 & TPR10_CA_BIT_DELAY)
		mctl_phy_ca_bit_delay_compensation(para, config);

	switch (para->type) {
	case SUNXI_DRAM_TYPE_DDR3:
		val = 0x80;
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		val = 0xc0;
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		val = 0x37;
		break;
	};
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x3dc);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x45c);

	mctl_phy_configure_odt(para);

	switch (para->type) {
	case SUNXI_DRAM_TYPE_DDR3:
		val = 0x0a;
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		val = 0x0b;
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		val = 0x0d;
		break;
	};
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 4, 0x7, val);

	if (para->clk <= 672)
		writel(0xf, SUNXI_DRAM_PHY0_BASE + 0x20);
	if (para->clk > 500) {
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x144, BIT_U32(7));
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x14c, 0xe0);
	} else {
		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x144, BIT_U32(7));
		clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x14c, 0xe0, 0x20);
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x14c, 8);

	mctl_await_completion((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x180), 4, 4);

	writel(0x37, SUNXI_DRAM_PHY0_BASE + 0x58);
	clrbits_le32(&mctl_com->unk_0x500, 0x200);

	writel(0, &mctl_ctl->swctl);
	setbits_le32(&mctl_ctl->dfimisc, 1);

	/* start DFI init */
	setbits_le32(&mctl_ctl->dfimisc, 0x20);
	writel(1, &mctl_ctl->swctl);
	mctl_await_completion(&mctl_ctl->swstat, 1, 1);
	/* poll DFI init complete */
	mctl_await_completion(&mctl_ctl->dfistat, 1, 1);
	writel(0, &mctl_ctl->swctl);
	clrbits_le32(&mctl_ctl->dfimisc, 0x20);

	clrbits_le32(&mctl_ctl->pwrctl, 0x20);
	writel(1, &mctl_ctl->swctl);
	mctl_await_completion(&mctl_ctl->swstat, 1, 1);
	mctl_await_completion(&mctl_ctl->statr, 3, 1);

	writel(0, &mctl_ctl->swctl);
	clrbits_le32(&mctl_ctl->dfimisc, 1);

	writel(1, &mctl_ctl->swctl);
	mctl_await_completion(&mctl_ctl->swstat, 1, 1);

	if (para->tpr2 & 0x100) {
		mr0 = 0x1b50;
		mr2 = 0x10;
	} else {
		mr0 = 0x1f14;
		mr2 = 0x20;
	}
	switch (para->type) {
	case SUNXI_DRAM_TYPE_DDR3:
		writel(mr0, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(4, &mctl_ctl->mrctrl1);
		writel(0x80001030, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(mr2, &mctl_ctl->mrctrl1);
		writel(0x80002030, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(0, &mctl_ctl->mrctrl1);
		writel(0x80003030, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		writel(mr0, &mctl_ctl->mrctrl1);
		writel(0x800000f0, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(4, &mctl_ctl->mrctrl1);
		writel(0x800000f0, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(mr2, &mctl_ctl->mrctrl1);
		writel(0x800000f0, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(0x301, &mctl_ctl->mrctrl1);
		writel(0x800000f0, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		writel(0x0, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(0x134, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(0x21b, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(0x333, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(0x403, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(0xb04, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(0xc72, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(0xe09, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(0x1624, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);
		break;
	};

	writel(0, SUNXI_DRAM_PHY0_BASE + 0x54);

	writel(0, &mctl_ctl->swctl);
	clrbits_le32(&mctl_ctl->rfshctl3, 1);
	writel(1, &mctl_ctl->swctl);

	if (para->tpr10 & TPR10_WRITE_LEVELING) {
		for (i = 0; i < 5; i++)
			if (mctl_phy_write_leveling(config))
				break;
		if (i == 5) {
			PRINTF("write leveling failed!\n");
			return (1 == 0);
		}
	}

	if (para->tpr10 & TPR10_READ_CALIBRATION) {
		for (i = 0; i < 5; i++)
			if (mctl_phy_read_calibration(config))
				break;
		if (i == 5) {
			PRINTF("read calibration failed!\n");
			return (1 == 0);
		}
	}

	if (para->tpr10 & TPR10_READ_TRAINING) {
		for (i = 0; i < 5; i++)
			if (mctl_phy_read_training(config))
				break;
		if (i == 5) {
			PRINTF("read training failed!\n");
			return (1 == 0);
		}
	}

	if (para->tpr10 & TPR10_WRITE_TRAINING) {
		for (i = 0; i < 5; i++)
			if (mctl_phy_write_training(config))
				break;
		if (i == 5) {
			PRINTF("write training failed!\n");
			return (1 == 0);
		}
	}

	mctl_phy_bit_delay_compensation(para);

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, 4);

	return (1 == 1);
}

static int mctl_ctrl_init(const struct dram_para *para,
			   const struct dram_config *config)
{
	struct sunxi_mctl_com_reg * const mctl_com =
			(struct sunxi_mctl_com_reg *)SUNXI_DRAM_COM_BASE;
	struct sunxi_mctl_ctl_reg * const mctl_ctl =
			(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;
	uint32_t reg_val;

	clrsetbits_le32(&mctl_com->unk_0x500, BIT_U32(24), 0x200);
	writel(0x8000, &mctl_ctl->clken);

	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
		writel(1, SUNXI_DRAM_COM_BASE + 0x50);

	setbits_le32(&mctl_com->unk_0x008, 0xff00);

	clrsetbits_le32(&mctl_ctl->sched[0], 0xff00, 0x3000);

	writel(0, &mctl_ctl->hwlpctl);

	setbits_le32(&mctl_com->unk_0x008, 0xff00);

	reg_val = MSTR_ACTIVE_RANKS(config->ranks);
	switch (para->type) {
	case SUNXI_DRAM_TYPE_DDR3:
		reg_val |= MSTR_BURST_LENGTH(8) | MSTR_DEVICETYPE_DDR3 | MSTR_2TMODE;
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		reg_val |= MSTR_BURST_LENGTH(8) | MSTR_DEVICETYPE_LPDDR3;
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		reg_val |= MSTR_BURST_LENGTH(16) | MSTR_DEVICETYPE_LPDDR4;
		break;
	};
	if (config->bus_full_width)
		reg_val |= MSTR_BUSWIDTH_FULL;
	else
		reg_val |= MSTR_BUSWIDTH_HALF;
	writel(BIT_U32(31) | BIT_U32(30) | reg_val, &mctl_ctl->mstr);

	if (config->ranks == 2)
		writel(0x0303, &mctl_ctl->odtmap);
	else
		writel(0x0201, &mctl_ctl->odtmap);

	switch (para->type) {
	case SUNXI_DRAM_TYPE_DDR3:
		reg_val = 0x06000400;
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		reg_val = 0x09020400;
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		reg_val = 0x04000400;
		break;
	};
	writel(reg_val, &mctl_ctl->odtcfg);
	writel(reg_val, &mctl_ctl->unk_0x2240);
	writel(reg_val, &mctl_ctl->unk_0x3240);
	writel(reg_val, &mctl_ctl->unk_0x4240);

	writel(BIT_U32(31), &mctl_com->cr);

	mctl_set_addrmap(config);

	mctl_set_timing_params(para);

	writel(0, &mctl_ctl->pwrctl);

	setbits_le32(&mctl_ctl->dfiupd[0], BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->zqctl[0], BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x2180, BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x3180, BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x4180, BIT_U32(31) | BIT_U32(30));

	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
		setbits_le32(&mctl_ctl->dbictl, 0x1);

	setbits_le32(&mctl_ctl->rfshctl3, BIT_U32(0));
	clrbits_le32(&mctl_ctl->dfimisc, BIT_U32(0));

	writel(0, &mctl_com->maer0);
	writel(0, &mctl_com->maer1);
	writel(0, &mctl_com->maer2);

	writel(0x20, &mctl_ctl->pwrctl);
	setbits_le32(&mctl_ctl->clken, BIT_U32(8));

	clrsetbits_le32(&mctl_com->unk_0x500, BIT_U32(24), 0x300);
	/* this write seems to enable PHY MMIO region */
	setbits_le32(&mctl_com->unk_0x500, BIT_U32(24));

	if (!mctl_phy_init(para, config))
		return (1 == 0);

	writel(0, &mctl_ctl->swctl);
	clrbits_le32(&mctl_ctl->rfshctl3, BIT_U32(0));

	setbits_le32(&mctl_com->unk_0x014, BIT_U32(31));
	writel(0xffffffff, &mctl_com->maer0);
	writel(0x7ff, &mctl_com->maer1);
	writel(0xffff, &mctl_com->maer2);

	writel(1, &mctl_ctl->swctl);
	mctl_await_completion(&mctl_ctl->swstat, 1, 1);

	return (1 == 1);
}

static int mctl_core_init(const struct dram_para *para,
			   const struct dram_config *config)
{
	mctl_sys_init(para->clk);

	return mctl_ctrl_init(para, config);
}

static void mctl_auto_detect_rank_width(const struct dram_para *para,
					struct dram_config *config)
{
	/* this is minimum size that it's supported */
	config->cols = 8;
	config->rows = 13;

	/*
	 * Strategy here is to test most demanding combination first and least
	 * demanding last, otherwise HW might not be fully utilized. For
	 * example, half bus width and rank = 1 combination would also work
	 * on HW with full bus width and rank = 2, but only 1/4 RAM would be
	 * visible.
	 */

	PRINTF("testing 32-bit width, rank = 2\n");
	config->bus_full_width = 1;
	config->ranks = 2;
	if (mctl_core_init(para, config))
		return;

	PRINTF("testing 32-bit width, rank = 1\n");
	config->bus_full_width = 1;
	config->ranks = 1;
	if (mctl_core_init(para, config))
		return;

	PRINTF("testing 16-bit width, rank = 2\n");
	config->bus_full_width = 0;
	config->ranks = 2;
	if (mctl_core_init(para, config))
		return;

	PRINTF("testing 16-bit width, rank = 1\n");
	config->bus_full_width = 0;
	config->ranks = 1;
	if (mctl_core_init(para, config))
		return;

	PRINTF("This DRAM setup is currently not supported.\n");
	for (;;)
		;
}

static void mctl_auto_detect_dram_size(const struct dram_para *para,
				       struct dram_config *config)
{
	/* detect row address bits */
	config->cols = 8;
	config->rows = 18;
	mctl_core_init(para, config);

	for (config->rows = 13; config->rows < 18; config->rows++) {
		/* 8 banks, 8 bit per byte and 16/32 bit width */
		if (mctl_mem_matches((1 << (config->rows + config->cols +
					    4 + config->bus_full_width))))
			break;
	}

	/* detect column address bits */
	config->cols = 11;
	mctl_core_init(para, config);

	for (config->cols = 8; config->cols < 11; config->cols++) {
		/* 8 bits per byte and 16/32 bit width */
		if (mctl_mem_matches(1 << (config->cols + 1 +
					   config->bus_full_width)))
			break;
	}
}

static uint64_t mctl_calc_size(const struct dram_config *config)
{
	uint8_t width = config->bus_full_width ? 4 : 2;

	/* 8 banks */
	return (UINT64_C(1) << (config->cols + config->rows + 3)) * width * config->ranks;
}

#if 1

/* 1GB LPDDR4 @ HelperBoard507 */
#define CONFIG_DRAM_SUN50I_H616_DX_ODT 0x0c0c0c0c
#define CONFIG_DRAM_SUN50I_H616_DX_DRI 0x0e0e0e0e
#define CONFIG_DRAM_SUN50I_H616_CA_DRI 0x0e0e
#define CONFIG_DRAM_SUN50I_H616_ODT_EN 0x7887bbbb
#define CONFIG_DRAM_SUN50I_H616_TPR0 0x0
#define CONFIG_DRAM_SUN50I_H616_TPR2 0x0
#define CONFIG_DRAM_SUN50I_H616_TPR10 0x402e0000
#define CONFIG_DRAM_SUN50I_H616_TPR11 0x22262622
#define CONFIG_DRAM_SUN50I_H616_TPR12 0x0b0c0d0b

static const struct dram_para para = {
	.clk = BOARD_CONFIG_DRAM_CLK, //CONFIG_DRAM_CLK,
#ifdef CONFIG_SUNXI_DRAM_H616_DDR3_1333
	.type = SUNXI_DRAM_TYPE_DDR3,
#elif defined(CONFIG_SUNXI_DRAM_H616_LPDDR3)
	.type = SUNXI_DRAM_TYPE_LPDDR3,
#elif defined(CONFIG_SUNXI_DRAM_H616_LPDDR4)
	.type = SUNXI_DRAM_TYPE_LPDDR4,
#endif
	.dx_odt = CONFIG_DRAM_SUN50I_H616_DX_ODT,
	.dx_dri = CONFIG_DRAM_SUN50I_H616_DX_DRI,
	.ca_dri = CONFIG_DRAM_SUN50I_H616_CA_DRI,
	.odt_en = CONFIG_DRAM_SUN50I_H616_ODT_EN,
	.tpr0 = CONFIG_DRAM_SUN50I_H616_TPR0,
	.tpr2 = CONFIG_DRAM_SUN50I_H616_TPR2,
	.tpr10 = CONFIG_DRAM_SUN50I_H616_TPR10,
	.tpr11 = CONFIG_DRAM_SUN50I_H616_TPR11,
	.tpr12 = CONFIG_DRAM_SUN50I_H616_TPR12,
};

#else

static struct dram_para para =
{
		.clk       = 792,
		.type      = SUNXI_DRAM_TYPE_LPDDR4,
		.dx_odt    = 0x08080808,
		.dx_dri    = 0x0e0e0e0e,
		.ca_dri    = 0x0e0e,
		.para0     = 0x7887bbbb,
		.para1     = 0x30fa,
		.para2     = 0x0000,
		.mr0       = 0x0,
		.mr1       = 0x34,
		.mr2       = 0x1b,
		.mr3       = 0x33,
		.mr4       = 0x3,
		.mr5       = 0x0,
		.mr6       = 0x0,
		.mr11      = 0x04,
		.mr12      = 0x72,
		.mr13      = 0x0,
		.mr14      = 0x9,
		.mr16      = 0x0,
		.mr17      = 0x0,
		.mr22      = 0x24,
		.tpr0      = 0x00000000,
		.tpr1      = 0x00000000,
		.tpr2      = 0x0,
		.tpr3      = 0x0,
		.tpr6      = 0x40808080,
		.tpr10     = 0x402f6633,
		.tpr11     = 0x00000000,
		.tpr12     = 0x00000000,
		.tpr13     = 0x02000c60,
		.tpr14     = 0x00000000,
};

#endif

uint64_t sunxi_dram_init(void)
{
	struct sunxi_prcm_reg *const prcm =
		(struct sunxi_prcm_reg *)SUNXI_PRCM_BASE;
	struct dram_config config;
	uint64_t size;

	setbits_le32(&prcm->res_cal_ctrl, BIT_U32(8));
	clrbits_le32(&prcm->ohms240, 0x3f);

	mctl_auto_detect_rank_width(&para, &config);
	mctl_auto_detect_dram_size(&para, &config);

	mctl_core_init(&para, &config);

	size = mctl_calc_size(&config);

	mctl_set_master_priority();

	return size;
};

static struct dram_para lpddr4ssss =
{
		.clk       = 792,
		.type      = SUNXI_DRAM_TYPE_LPDDR4,
		.dx_odt    = 0x08080808,
		.dx_dri    = 0x0e0e0e0e,
		.ca_dri    = 0x0e0e,
		.para0     = 0x7887bbbb,
		.para1     = 0x30fa,
		.para2     = 0x0000,
		.mr0       = 0x0,
		.mr1       = 0x34,
		.mr2       = 0x1b,
		.mr3       = 0x33,
		.mr4       = 0x3,
		.mr5       = 0x0,
		.mr6       = 0x0,
		.mr11      = 0x04,
		.mr12      = 0x72,
		.mr13      = 0x0,
		.mr14      = 0x9,
		.mr16      = 0x0,
		.mr17      = 0x0,
		.mr22      = 0x24,
		.tpr0      = 0x00000000,
		.tpr1      = 0x00000000,
		.tpr2      = 0x0,
		.tpr3      = 0x0,
		.tpr6      = 0x40808080,
		.tpr10     = 0x402f6633,
		.tpr11     = 0x00000000,
		.tpr12     = 0x00000000,
		.tpr13     = 0x02000c60,
		.tpr14     = 0x00000000,
};


int xdramc_simple_wr_test(unsigned int mem_mb, int len)
{
	unsigned int  offs	= (mem_mb >> 1) << 18; // half of memory size
	const uint32_t  patt1 = 0x01234567;
	const uint32_t  patt2 = 0xfedcba98;
	uint32_t *addr, v1, v2, i;

	addr = (uint32_t *)CONFIG_SYS_SDRAM_BASE;
	for (i = 0; i != len; i++, addr++) {
		write32((virtual_addr_t)addr, patt1 + i);
		write32((virtual_addr_t)(addr + offs), patt2 + i);
	}

	addr = (uint32_t *)CONFIG_SYS_SDRAM_BASE;
	for (i = 0; i != len; i++) {
		v1 = read32((virtual_addr_t)(addr + i));
		v2 = patt1 + i;
		if (v1 != v2) {
			PRINTF("DRAM simple test FAIL.\n");
			PRINTF("%x != %x at address %p\n", (unsigned) v1, (unsigned) v2, (void *) (addr + i));
			return 1;
		}
		v1 = read32((virtual_addr_t)(addr + offs + i));
		v2 = patt2 + i;
		if (v1 != v2) {
			PRINTF("DRAM simple test FAIL.\n");
			PRINTF("%x != %x at address %p\n", (unsigned) v1, (unsigned) v2, (void *) (addr + offs + i));
			return 1;
		}
	}
	PRINTF("DRAM simple test OK.\n");
	return 0;
}


static unsigned long rand_val = 123456UL;

static void local_random_init(void)
{
	rand_val = 123456UL;
}

static unsigned long local_random(void)
{


	if (rand_val & 0x80000000UL)
		rand_val = (rand_val << 1);
	else
		rand_val = (rand_val << 1) ^ 0x201051UL;

	return (rand_val);
}

static uint32_t ddr_check_rand(unsigned long sizeee)
{
	typedef uint16_t test_t;
	const uint32_t sizeN = sizeee / sizeof (test_t);
	volatile test_t * const p = (volatile test_t *) CONFIG_SYS_SDRAM_BASE;
	uint32_t i;
	uint32_t uret;

	// fill
	//local_random_init();
	uint32_t seed = rand_val;

	for (i = 0; i < sizeN; ++ i)
	{
		//ddr_check_progress(i);
		p [i] = local_random();
	}
	// compare
	//local_random_init();
	rand_val = seed;
	//p = (volatile uint16_t *) STM32MP_DDR_BASE;
	for (i = 0; i < sizeN; ++ i)
	{
		//ddr_check_progress(i);
		if (p [i] != (test_t) local_random())
			return i * sizeof (test_t);
	}

	return sizeee;	// OK
}


void arm_hardware_sdram_initialize(void)
{
	uint64_t memsizeB;
	unsigned memsizeMB;
	// https://artmemtech.com/
	// artmem atl4b0832
	PRINTF("arm_hardware_sdram_initialize start, cpux=%u MHz\n", (unsigned) (allwnr_t507_get_cpux_freq() / 1000 / 1000));
	PRINTF("arm_hardware_sdram_initialize, ddr=%u MHz\n", (unsigned) (allwnr_t507_get_dram_freq() / 1000 / 1000));

	memsizeB = sunxi_dram_init();
	memsizeMB = memsizeB / 1024 / 1024;
	//memsize =  dram_power_up_process(& lpddr4);
	//dbp();
	PRINTF("arm_hardware_sdram_initialize: result=%u MB\n", memsizeMB);
	(void) memsizeMB;

#if 0
	if (xdramc_simple_wr_test(memsizeMB, 64))
	{
		PRINTF("xdramc_simple_wr_test failed\n");
	}
	else
	{
		PRINTF("xdramc_simple_wr_test passed\n");
	}

	{
		int e;
		for (e = 0; e < 4; ++ e)
		{
			unsigned uret;
			unsigned size = memsize * 1024 * 1024;
			int partfortest = 4;
			uret = ddr_check_rand(size / partfortest);
			if (uret != (size / partfortest)) {
				PRINTF("DDR random test: 0x%08x does not match DT config: 0x%08x\n",
				      uret, size / partfortest);
				for(;;)
					;
			}
			//TP();
		}
	}
#endif
	//	memset((void *) CONFIG_SYS_SDRAM_BASE, 0, 128u << 20);
	//	memset((void *) CONFIG_SYS_SDRAM_BASE + 0x00, 0xE5, 0x80);
	//	memset((void *) CONFIG_SYS_SDRAM_BASE + 0x80, 0xDF, 0x80);
	//printhex32(CONFIG_SYS_SDRAM_BASE, (void *) CONFIG_SYS_SDRAM_BASE, 2 * 0x80);

	PRINTF("arm_hardware_sdram_initialize done, ddr=%u MHz\n", (unsigned) (allwnr_t507_get_dram_freq() / 1000 / 1000));
}

#endif /* WITHSDRAMHW && CPUSTYLE_T507 && ! CPUSTYLE_H616 */
