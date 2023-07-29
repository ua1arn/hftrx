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

#if CPUSTYLE_H616
	#define ENHTEST 1
#else
	#define ENHTEST 0
#endif

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

static void dbp(void)
{
	printhex32(SUNXI_DRAM_PHY0_BASE, (void *) SUNXI_DRAM_PHY0_BASE, 32);
	printhex32(SUNXI_DRAM_CTL0_BASE, (void *) SUNXI_DRAM_CTL0_BASE, 32);

}

#define SUNXI_PRCM_BASE			0x07010000

#define SUNXI_CCM_BASE CCU_BASE

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

struct xdram_para {
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

void mctl_set_timing_params(struct xdram_para *para);


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

void mctl_set_timing_params(struct xdram_para *para)
{
	struct sunxi_mctl_ctl_reg * const mctl_ctl =
			(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;

	uint8_t tccd		= 2;			/* JEDEC: 4nCK */
	uint8_t tfaw		= ns_to_t(50);		/* JEDEC: 30 ns w/ 1K pages */
	uint8_t trrd		= max(ns_to_t(6), 4);	/* JEDEC: max(6 ns, 4nCK) */
	uint8_t trcd		= ns_to_t(15);		/* JEDEC: 13.5 ns */
	uint8_t trc		= ns_to_t(53);		/* JEDEC: 49.5 ns */
	uint8_t txp		= max(ns_to_t(6), 3);	/* JEDEC: max(6 ns, 3nCK) */
	uint8_t trtp		= max(ns_to_t(8), 2);	/* JEDEC: max(7.5 ns, 4nCK) */
	uint8_t trp		= ns_to_t(15);		/* JEDEC: >= 13.75 ns */
	uint8_t tras		= ns_to_t(38);		/* JEDEC >= 36 ns, <= 9*trefi */
	uint16_t trefi	= ns_to_t(7800) / 32;	/* JEDEC: 7.8us@Tcase <= 85C */
	uint16_t trfc	= ns_to_t(350);		/* JEDEC: 160 ns for 2Gb */
	uint16_t txsr	= 4;			/* ? */

	uint8_t tmrw		= 0;			/* ? */
	uint8_t tmrd		= 4;			/* JEDEC: 4nCK */
	uint8_t tmod		= max(ns_to_t(15), 12);	/* JEDEC: max(15 ns, 12nCK) */
	uint8_t tcke		= max(ns_to_t(6), 3);	/* JEDEC: max(5.625 ns, 3nCK) */
	uint8_t tcksrx	= max(ns_to_t(10), 4);	/* JEDEC: max(10 ns, 5nCK) */
	uint8_t tcksre	= max(ns_to_t(10), 4);	/* JEDEC: max(10 ns, 5nCK) */
	uint8_t tckesr	= tcke + 1;		/* JEDEC: tCKE(min) + 1nCK */
	uint8_t trasmax	= (para->clk / 2) / 15;	/* JEDEC: tREFI * 9 */
	uint8_t txs		= ns_to_t(360) / 32;	/* JEDEC: max(5nCK,tRFC+10ns) */
	uint8_t txsdll	= 16;			/* JEDEC: 512 nCK */
	uint8_t txsabort	= 4;			/* ? */
	uint8_t txsfast	= 4;			/* ? */
	uint8_t tcl		= 7;			/* JEDEC: CL / 2 => 6 */
	uint8_t tcwl		= 5;			/* JEDEC: 8 */
	uint8_t t_rdata_en	= 9;			/* ? */

	uint8_t twtp		= 14;			/* (WL + BL / 2 + tWR) / 2 */
	uint8_t twr2rd	= trtp + 7;		/* (WL + BL / 2 + tWTR) / 2 */
	uint8_t trd2wr	= 5;			/* (RL + BL / 2 + 2 - WL) / 2 */

	/* set DRAM timing */
	writel(0x180f0c10,&mctl_ctl->dramtmg[0]); //100
	writel(0x00030418, &mctl_ctl->dramtmg[1]); //104
	writel(0x050a1212, &mctl_ctl->dramtmg[2]); //108
	writel(0x0060600c, &mctl_ctl->dramtmg[3]); //10c
	writel(0x07040408, &mctl_ctl->dramtmg[4]); //110
	writel(0x02020606, &mctl_ctl->dramtmg[5]); //114
	/* Value suggested by ZynqMP manual and used by libdram */
	writel(0x02020005, &mctl_ctl->dramtmg[6]); //118
	writel(0x04041004, &mctl_ctl->dramtmg[8]); //120
	writel(0x00020208, &mctl_ctl->dramtmg[9]); //124
	writel(0xE0C05, &mctl_ctl->dramtmg[10]);   //128
	writel(0x440C021C, &mctl_ctl->dramtmg[11]); //12c
	writel(8, &mctl_ctl->dramtmg[12]);         //130
	writel(0xA100002, &mctl_ctl->dramtmg[13]); //134
	writel(0x45, &mctl_ctl->dramtmg[14]);      //138

	writel(0x203f0, &mctl_ctl->init[0]);        //0do
	writel(0x1f20000, &mctl_ctl->init[1]);      //0d4
	writel(0xd05, &mctl_ctl->init[2]);          //0d8
	writel(0x0034001b, &mctl_ctl->init[3]);     //0dc
	writel(0x00330000, &mctl_ctl->init[4]);     //0e0
	writel(0x00100004, &mctl_ctl->init[5]);     //0e4
	writel(0x00040072, &mctl_ctl->init[6]);     //0e8
	writel(0x00240009, &mctl_ctl->init[7]);     //0ec

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

inline void mbus_configure_port(uint8_t port,
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

	//PRINTF("MBUS port %d cfg0 %08x cfg1 %08x\n", port, (unsigned) cfg0, (unsigned) cfg1);
	writel_relaxed(cfg0, &mctl_com->master[port].cfg0);
	writel_relaxed(cfg1, &mctl_com->master[port].cfg1);
}

#define MBUS_CONF(port, bwlimit, qos, acs, bwl0, bwl1, bwl2)	\
	mbus_configure_port(port, bwlimit, 0/*0*/, \
			    MBUS_QOS_ ## qos, 0, acs, bwl0, bwl1, bwl2)

static void mctl_set_master_priority(void)
{
	struct sunxi_mctl_com_reg * const mctl_com =
			(struct sunxi_mctl_com_reg *)SUNXI_DRAM_COM_BASE;

	/* enable bandwidth limit windows and set windows size 1us */
	writel(399, &mctl_com->tmr);
	writel(BIT_U32(16), &mctl_com->bwcr);
	writel(0x1000009, SUNXI_DRAM_COM_BASE + 0x370);
	writel(0x640080, SUNXI_DRAM_COM_BASE + 0x374);

	MBUS_CONF( 0, 1/*1*/, HIGHEST, 0,  256,  128,  100);
	MBUS_CONF( 1, 1/*1*/,    HIGH, 0, 1536, 1400,  256);
	MBUS_CONF( 2, 1/*1*/, HIGHEST, 0,  512,  256,   96);
	MBUS_CONF( 3, 1/*1*/,    HIGH, 0,  256,  100,   80);
	MBUS_CONF( 4, 1/*1*/,    HIGH, 2, 8192, 5500, 5000);
	MBUS_CONF( 5, 1/*1*/,    HIGH, 2,  100,   64,   32);
	MBUS_CONF( 6, 1/*1*/,    HIGH, 2,  100,   64,   32);
	MBUS_CONF( 8, 1/*1*/,    HIGH, 0,  256,  128,   64);
	MBUS_CONF(11, 1/*1*/,    HIGH, 0,  256,  128,  100);
	MBUS_CONF(14, 1/*1*/,    HIGH, 0, 1024,  256,   64);
	MBUS_CONF(16, 1/*1*/, HIGHEST, 6, 8192, 2800, 2400);
	MBUS_CONF(21, 1/*1*/, HIGHEST, 6, 2048,  768,  512);
	MBUS_CONF(25, 1/*1*/, HIGHEST, 0,  100,   64,   32);
	MBUS_CONF(26, 1/*1*/,    HIGH, 2, 8192, 5500, 5000);
	MBUS_CONF(37, 1/*1*/,    HIGH, 0,  256,  128,   64);
	MBUS_CONF(38, 1/*1*/,    HIGH, 2,  100,   64,   32);
	MBUS_CONF(39, 1/*1*/,    HIGH, 2, 8192, 5500, 5000);
	MBUS_CONF(40, 1/*1*/,    HIGH, 2,  100,   64,   32);
	writel(0x64000f, SUNXI_DRAM_COM_BASE + 0x3a0);
	__DMB();
}

static void mctl_sys_init(struct xdram_para *para)
{
	const unsigned reffreq = allwnrt113_get_hosc_freq() / 1000 / 1000;
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
	       CCM_PLL5_CTRL_N(para->clk * 2 / reffreq), &ccm->pll5_cfg);
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

static void mctl_set_addrmap(struct xdram_para *para)
{
	struct sunxi_mctl_ctl_reg * const mctl_ctl =
			(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;
	uint8_t cols = para->cols;
	uint8_t rows = para->rows;
	uint8_t ranks = para->ranks;

	if (!para->bus_full_width)
		cols -= 1;

	/* Ranks */
	if (ranks == 2)
		mctl_ctl->addrmap[0] = rows + cols - 3;
	else
		mctl_ctl->addrmap[0] = 0x1F;

	/* Banks, hardcoded to 8 banks now */
	mctl_ctl->addrmap[1] = 0x00080808;

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
	mctl_ctl->addrmap[5] = 0x07070707;
	switch (rows) {
	case 13:
		mctl_ctl->addrmap[6] = 0x0f070707;
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
	0x03, 0x00, 0x17, 0x05, 0x02, 0x19, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x01,
	0x18, 0x04, 0x1a
};

static void mctl_phy_configure_odt(void)
{
	writel_relaxed(0xe, SUNXI_DRAM_PHY0_BASE + 0x388);
	writel_relaxed(0x4, SUNXI_DRAM_PHY0_BASE + 0x38c);

	writel_relaxed(0xe, SUNXI_DRAM_PHY0_BASE + 0x3c8);
	writel_relaxed(0x4, SUNXI_DRAM_PHY0_BASE + 0x3cc);

	writel_relaxed(0xe, SUNXI_DRAM_PHY0_BASE + 0x408);
	writel_relaxed(0x4, SUNXI_DRAM_PHY0_BASE + 0x40c);

	writel_relaxed(0xe, SUNXI_DRAM_PHY0_BASE + 0x448);
	writel_relaxed(0x4, SUNXI_DRAM_PHY0_BASE + 0x44c);

	writel_relaxed(0xe, SUNXI_DRAM_PHY0_BASE + 0x340);
	writel_relaxed(0xe, SUNXI_DRAM_PHY0_BASE + 0x344);

	writel_relaxed(0xe, SUNXI_DRAM_PHY0_BASE + 0x348);
	writel_relaxed(0xe, SUNXI_DRAM_PHY0_BASE + 0x34c);
	//??
	writel_relaxed(0xc, SUNXI_DRAM_PHY0_BASE + 0x380);
	writel_relaxed(0x0, SUNXI_DRAM_PHY0_BASE + 0x384);
	//??
	writel_relaxed(0xc, SUNXI_DRAM_PHY0_BASE + 0x3c0);
	writel_relaxed(0x0, SUNXI_DRAM_PHY0_BASE + 0x3c4);
	//??
	writel_relaxed(0xc, SUNXI_DRAM_PHY0_BASE + 0x400);
	writel_relaxed(0x0, SUNXI_DRAM_PHY0_BASE + 0x404);
	//??
	writel_relaxed(0xc, SUNXI_DRAM_PHY0_BASE + 0x440);
	writel_relaxed(0x0, SUNXI_DRAM_PHY0_BASE + 0x444);

	__DMB();
}

static int mctl_phy_write_leveling(struct xdram_para *para)
{
	int result = 1/*1*/;
	uint32_t val;

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0xc0, 0x80);
	writel(4, SUNXI_DRAM_PHY0_BASE + 0xc);
	writel(0x40, SUNXI_DRAM_PHY0_BASE + 0x10);

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 4);

	if (para->bus_full_width)
		val = 0xf;
	else
		val = 3;

	mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x188), val, val);

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 4);

	val = readl(SUNXI_DRAM_PHY0_BASE + 0x258);
	if (val == 0 || val == 0x3f)
		result = 0/*0*/;
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x25c);
	if (val == 0 || val == 0x3f)
		result = 0/*0*/;
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x318);
	if (val == 0 || val == 0x3f)
		result = 0/*0*/;
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x31c);
	if (val == 0 || val == 0x3f)
		result = 0/*0*/;

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0xc0);

	if (para->ranks == 2) {
		clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0xc0, 0x40);

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 4);

		if (para->bus_full_width)
			val = 0xf;
		else
			val = 3;

		mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x188), val, val);

		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 4);
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0xc0);

	return result;
}

static int mctl_phy_read_calibration(struct xdram_para *para)
{
	int result = 1/*1*/;
	uint32_t val, tmp;

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30, 0x20);

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);

	if (para->bus_full_width)
		val = 0xf;
	else
		val = 3;

	while ((readl(SUNXI_DRAM_PHY0_BASE + 0x184) & val) != val) {
		if (readl(SUNXI_DRAM_PHY0_BASE + 0x184) & 0x20) {
			result = 0/*0*/;
			break;
		}
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30);

	if (para->ranks == 2) {
		clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30, 0x10);

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);

		while ((readl(SUNXI_DRAM_PHY0_BASE + 0x184) & val) != val) {
			if (readl(SUNXI_DRAM_PHY0_BASE + 0x184) & 0x20) {
				result = 0/*0*/;
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

static int mctl_phy_read_training(struct xdram_para *para)
{
	uint32_t val1, val2, *ptr1, *ptr2;
	int result = 1/*1*/;
	int i;

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 3, 2);
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x804, 0x3f, 0xf);
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x808, 0x3f, 0xf);
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0xa04, 0x3f, 0xf);
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0xa08, 0x3f, 0xf);

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 6);
	setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 1);

	mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x840), 0xc, 0xc);
	if (readl(SUNXI_DRAM_PHY0_BASE + 0x840) & 3)
		result = 0/*0*/;

	if (para->bus_full_width) {
		mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xa40), 0xc, 0xc);
		if (readl(SUNXI_DRAM_PHY0_BASE + 0xa40) & 3)
			result = 0/*0*/;
	}

	ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x898);
	ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x850);
	for (i = 0; i < 9; i++) {
		val1 = readl(&ptr1[i]);
		val2 = readl(&ptr2[i]);
		if (val1 - val2 <= 6)
			result = 0/*0*/;
	}
	ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x8bc);
	ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x874);
	for (i = 0; i < 9; i++) {
		val1 = readl(&ptr1[i]);
		val2 = readl(&ptr2[i]);
		if (val1 - val2 <= 6)
			result = 0/*0*/;
	}

	if (para->bus_full_width) {
		ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xa98);
		ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xa50);
		for (i = 0; i < 9; i++) {
			val1 = readl(&ptr1[i]);
			val2 = readl(&ptr2[i]);
			if (val1 - val2 <= 6)
				result = 0/*0*/;
		}

		ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xabc);
		ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xa74);
		for (i = 0; i < 9; i++) {
			val1 = readl(&ptr1[i]);
			val2 = readl(&ptr2[i]);
			if (val1 - val2 <= 6)
				result = 0/*0*/;
		}
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 3);

	if (para->ranks == 2) {
		/* maybe last parameter should be 1? */
		clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 3, 2);

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 6);
		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 1);

		mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x840), 0xc, 0xc);
		if (readl(SUNXI_DRAM_PHY0_BASE + 0x840) & 3)
			result = 0/*0*/;

		if (para->bus_full_width) {
			mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xa40), 0xc, 0xc);
			if (readl(SUNXI_DRAM_PHY0_BASE + 0xa40) & 3)
				result = 0/*0*/;
		}

		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 3);
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 3);

	return result;
}

static int mctl_phy_write_training(struct xdram_para *para)
{
	uint32_t val1, val2, *ptr1, *ptr2;
	int result = 1/*1*/;
	int i;

	writel(0, SUNXI_DRAM_PHY0_BASE + 0x134);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x138);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x19c);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x1a0);

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 0xc, 8);

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x10);
	setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x20);

	mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x8e0), 3, 3);
	if (readl(SUNXI_DRAM_PHY0_BASE + 0x8e0) & 0xc)
		result = 0/*0*/;

	if (para->bus_full_width) {
		mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xae0), 3, 3);
		if (readl(SUNXI_DRAM_PHY0_BASE + 0xae0) & 0xc)
			result = 0/*0*/;
	}

	ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x938);
	ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x8f0);
	for (i = 0; i < 9; i++) {
		val1 = readl(&ptr1[i]);
		val2 = readl(&ptr2[i]);
		if (val1 - val2 <= 6)
			result = 0/*0*/;
	}
	ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x95c);
	ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x914);
	for (i = 0; i < 9; i++) {
		val1 = readl(&ptr1[i]);
		val2 = readl(&ptr2[i]);
		if (val1 - val2 <= 6)
			result = 0/*0*/;
	}

	if (para->bus_full_width) {
		ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xb38);
		ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xaf0);
		for (i = 0; i < 9; i++) {
			val1 = readl(&ptr1[i]);
			val2 = readl(&ptr2[i]);
			if (val1 - val2 <= 6)
				result = 0/*0*/;
		}
		ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xb5c);
		ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xb14);
		for (i = 0; i < 9; i++) {
			val1 = readl(&ptr1[i]);
			val2 = readl(&ptr2[i]);
			if (val1 - val2 <= 6)
				result = 0/*0*/;
		}
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x60);

	if (para->ranks == 2) {
		clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 0xc, 4);

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x10);
		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x20);

		mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x8e0), 3, 3);
		if (readl(SUNXI_DRAM_PHY0_BASE + 0x8e0) & 0xc)
			result = 0/*0*/;

		if (para->bus_full_width) {
			mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xae0), 3, 3);
			if (readl(SUNXI_DRAM_PHY0_BASE + 0xae0) & 0xc)
				result = 0/*0*/;
		}

		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x60);
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 0xc);

	return result;
}

static int mctl_phy_bit_delay_compensation(struct xdram_para *para)
{
	uint32_t *ptr;
	int i;

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, 1);
	setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 8);
	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x10);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x484);
	for (i = 0; i < 9; i++) {
		writel_relaxed(0x23, ptr);
		writel_relaxed(0x23, ptr + 0x30);
		ptr += 2;
	}
	writel_relaxed(0x0e, SUNXI_DRAM_PHY0_BASE + 0x4d0);
	writel_relaxed(0x0e, SUNXI_DRAM_PHY0_BASE + 0x590);
	writel_relaxed(0x0e, SUNXI_DRAM_PHY0_BASE + 0x4cc);
	writel_relaxed(0x0e, SUNXI_DRAM_PHY0_BASE + 0x58c);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x4d8);
	for (i = 0; i < 9; i++) {
		writel_relaxed(0x27, ptr);
		writel_relaxed(0x27, ptr + 0x30);
		ptr += 2;
	}
	writel_relaxed(0x10, SUNXI_DRAM_PHY0_BASE + 0x524);
	writel_relaxed(0x10, SUNXI_DRAM_PHY0_BASE + 0x5e4);
	writel_relaxed(0x10, SUNXI_DRAM_PHY0_BASE + 0x520);
	writel_relaxed(0x10, SUNXI_DRAM_PHY0_BASE + 0x5e0);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x604);
	for (i = 0; i < 9; i++) {
		writel_relaxed(0x27, ptr);
		writel_relaxed(0x27, ptr + 0x30);
		ptr += 2;
	}
	writel_relaxed(0x10, SUNXI_DRAM_PHY0_BASE + 0x650);
	writel_relaxed(0x10, SUNXI_DRAM_PHY0_BASE + 0x710);
	writel_relaxed(0x10, SUNXI_DRAM_PHY0_BASE + 0x64c);
	writel_relaxed(0x10, SUNXI_DRAM_PHY0_BASE + 0x70c);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x658);
	for (i = 0; i < 9; i++) {
		writel_relaxed(0x22, ptr);
		writel_relaxed(0x22, ptr + 0x30);
		ptr += 2;
	}
	writel_relaxed(0x0e, SUNXI_DRAM_PHY0_BASE + 0x6a4);
	writel_relaxed(0x0e, SUNXI_DRAM_PHY0_BASE + 0x764);
	writel_relaxed(0x0e, SUNXI_DRAM_PHY0_BASE + 0x6a0);
	writel_relaxed(0x0e, SUNXI_DRAM_PHY0_BASE + 0x760);

	__DMB();

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, 1);

	/* second part */
	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x54, 0x80);
	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 4);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x480);
	for (i = 0; i < 9; i++) {
		writel_relaxed(0x08, ptr);
		writel_relaxed(0x08, ptr + 0x30);
		ptr += 2;
	}
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x528);
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x5e8);
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x4c8);
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x588);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x4d4);
	for (i = 0; i < 9; i++) {
		writel_relaxed(0x07, ptr);
		writel_relaxed(0x07, ptr + 0x30);
		ptr += 2;
	}
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x52c);
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x5ec);
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x51c);
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x5dc);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x600);
	for (i = 0; i < 9; i++) {
		writel_relaxed(0x06, ptr);
		writel_relaxed(0x06, ptr + 0x30);
		ptr += 2;
	}
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x6a8);
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x768);
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x648);
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x708);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x654);
	for (i = 0; i < 9; i++) {
		writel_relaxed(0x06, ptr);
		writel_relaxed(0x06, ptr + 0x30);
		ptr += 2;
	}
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x6ac);
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x76c);
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x69c);
	writel_relaxed(0x16, SUNXI_DRAM_PHY0_BASE + 0x75c);

	__DMB();

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x54, 0x80);

	return 1/*1*/;
}

static int mctl_phy_init(struct xdram_para *para)
{
	struct sunxi_mctl_com_reg * const mctl_com =
			(struct sunxi_mctl_com_reg *)SUNXI_DRAM_COM_BASE;
	struct sunxi_mctl_ctl_reg * const mctl_ctl =
			(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;
	uint32_t val, *ptr;
	int i;

	//LPDDR4 only
	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x4,0x80);

	if (para->bus_full_width)
		val = 0xf;
	else
		val = 3;
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x3c, 0xf, val);

	writel(0x14, SUNXI_DRAM_PHY0_BASE + 0x14);
	writel(0x14, SUNXI_DRAM_PHY0_BASE + 0x35c);
	writel(0x14, SUNXI_DRAM_PHY0_BASE + 0x368);
	writel(0x14, SUNXI_DRAM_PHY0_BASE + 0x374);

	writel(0, SUNXI_DRAM_PHY0_BASE + 0x18);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x360);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x36c);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x378);

	writel(0xa, SUNXI_DRAM_PHY0_BASE + 0x1c);
	writel(0xa, SUNXI_DRAM_PHY0_BASE + 0x364);
	writel(0xa, SUNXI_DRAM_PHY0_BASE + 0x370);
	writel(0xa, SUNXI_DRAM_PHY0_BASE + 0x37c);
	//
	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xc0);
	for (i = 0; i < ARRAY_SIZE(phy_init); i++)
		writel(phy_init[i], &ptr[i]);

	if (IS_ENABLED(CONFIG_DRAM_SUN50I_H616_UNKNOWN_FEATURE)) {
		ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x780);
		for (i = 0; i < 31; i++)
			writel(0x06, &ptr[i]);
		writel(0xc, SUNXI_DRAM_PHY0_BASE + 0x788);
		writel(0x6, SUNXI_DRAM_PHY0_BASE + 0x78c);
		writel(0x6, SUNXI_DRAM_PHY0_BASE + 0x7a4);
		writel(0x6, SUNXI_DRAM_PHY0_BASE + 0x7b8);
		writel(0x6, SUNXI_DRAM_PHY0_BASE + 0x7d4);
		writel(0x6, SUNXI_DRAM_PHY0_BASE + 0x7dc);
		writel(0x6, SUNXI_DRAM_PHY0_BASE + 0x7e0);
		writel(0x6, SUNXI_DRAM_PHY0_BASE + 0x7fc);
	}

	writel(0x37, SUNXI_DRAM_PHY0_BASE + 0x3dc);
	writel(0x37, SUNXI_DRAM_PHY0_BASE + 0x45c);

	//if (IS_ENABLED(CONFIG_DRAM_ODT_EN))
		mctl_phy_configure_odt();

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 4, 7, 0xd);

	if (para->clk <= 672)
		writel(0xf, SUNXI_DRAM_PHY0_BASE + 0x20);
	if (para->clk > 500) {
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x144, BIT_U32(7));
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x14c, 0xe0);
	} else {
		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x144, BIT_U32(7));
		clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x14c, 0xe0, 0x20);
	}
//
	//clrbits_le32(&mctl_com->unk_0x500, 0x200);
	//udelay(1);
	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x14c, 8);

	mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x180), 4, 4);

	writel(0x37, SUNXI_DRAM_PHY0_BASE + 0x58);
	clrbits_le32(&mctl_com->unk_0x500, 0x200);
//?? mctl_dfi_init
	writel(0, &mctl_ctl->swctl);
	setbits_le32(&mctl_ctl->dfimisc, 1);

	/* start DFI init */
	setbits_le32(&mctl_ctl->dfimisc, 0x20);
	writel(1, &mctl_ctl->swctl);
	TP();
	mctl_await_completion(&mctl_ctl->swstat, 1, 1);
	TP();
	/* poll DFI init complete */
	mctl_await_completion(&mctl_ctl->dfistat, 1, 1);
	TP();
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

	writel(0, SUNXI_DRAM_PHY0_BASE + 0x54);
//end
	writel(0, &mctl_ctl->swctl);
	clrbits_le32(&mctl_ctl->rfshctl3, 1);
	writel(1, &mctl_ctl->swctl);
	//phy_zq_calibration

	// if (IS_ENABLED(CONFIG_DRAM_SUN50I_H616_WRITE_LEVELING)) {
	// 	for (i = 0; i < 5; i++)
	// 		if (mctl_phy_write_leveling(para))
	// 			break;
	// 	if (i == 5) {
	// 		PRINTF("write leveling failed!\n");
	// 		return 0/*0*/;
	// 	}
	// }

	if (IS_ENABLED(CONFIG_DRAM_SUN50I_H616_READ_CALIBRATION)) {
		for (i = 0; i < 5; i++)
			if (mctl_phy_read_calibration(para))
				break;
		if (i == 5) {
			PRINTF("read calibration failed!\n");
			return 0/*0*/;
		}
	}

	if (0 && IS_ENABLED(CONFIG_DRAM_SUN50I_H616_READ_TRAINING)) {
		for (i = 0; i < 5; i++)
			if (mctl_phy_read_training(para))
				break;
		if (i == 5) {
			PRINTF("read training failed!\n");
			return 0/*0*/;
		}
	}

	if (IS_ENABLED(CONFIG_DRAM_SUN50I_H616_WRITE_TRAINING)) {
		for (i = 0; i < 5; i++)
			if (mctl_phy_write_training(para))
				break;
		if (i == 5) {
			PRINTF("write training failed!\n");
			return 0/*0*/;
		}
	}
	//??
	if (IS_ENABLED(CONFIG_DRAM_SUN50I_H616_BIT_DELAY_COMPENSATION))
		mctl_phy_bit_delay_compensation(para);

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, 4);

	return 1/*1*/;
}

static int mctl_ctrl_init(struct xdram_para *para)
{
	struct sunxi_mctl_com_reg * const mctl_com =
			(struct sunxi_mctl_com_reg *)SUNXI_DRAM_COM_BASE;
	struct sunxi_mctl_ctl_reg * const mctl_ctl =
			(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;
	uint32_t reg_val;

	clrsetbits_le32(&mctl_com->unk_0x500, BIT_U32(24), 0x200);
	writel(0x8000, &mctl_ctl->clken);

	setbits_le32(&mctl_com->unk_0x008, 0xff00);
	// for LPDDR4 only
	writel(1, SUNXI_DRAM_COM_BASE + 0x50);
	//end
	//??
	clrsetbits_le32(&mctl_ctl->sched[0], 0xff00, 0x3000);

	writel(0, &mctl_ctl->hwlpctl);
	//
	setbits_le32(&mctl_com->unk_0x008, 0xff00);

	reg_val = MSTR_BURST_LENGTH(8) | MSTR_ACTIVE_RANKS(para->ranks);
	reg_val |= MSTR_DEVICETYPE_LPDDR4;
	if (para->bus_full_width)
		reg_val |= MSTR_BUSWIDTH_FULL;
	else
		reg_val |= MSTR_BUSWIDTH_HALF;
	writel(BIT_U32(31) | BIT_U32(30) | reg_val, &mctl_ctl->mstr);
	if (para->ranks == 2)
		writel(0x0303, &mctl_ctl->odtmap);
	else
		writel(0x0201, &mctl_ctl->odtmap);

	writel(0x04000400, &mctl_ctl->odtcfg);
	writel(0x04000400, &mctl_ctl->unk_0x2240);
	writel(0x04000400, &mctl_ctl->unk_0x3240);
	writel(0x04000400, &mctl_ctl->unk_0x4240);
//
	writel(BIT_U32(31),&mctl_com->cr);

	mctl_set_addrmap(para);

	mctl_set_timing_params(para);

	writel(0, &mctl_ctl->pwrctl);
//
	setbits_le32(&mctl_ctl->dfiupd[0], BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->zqctl[0], BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x2180, BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x3180, BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x4180, BIT_U32(31) | BIT_U32(30));

	//(LP)DDR4 only set_controller_dbi
	setbits_le32(&mctl_ctl->dbictl, 0x1);

	setbits_le32(&mctl_ctl->rfshctl3, BIT_U32(0));
	clrbits_le32(&mctl_ctl->dfimisc, BIT_U32(0));

	writel(0, &mctl_com->maer0);
	writel(0, &mctl_com->maer1);
	writel(0, &mctl_com->maer2);

	writel(0x20, &mctl_ctl->pwrctl);
	//??
	setbits_le32(&mctl_ctl->clken, BIT_U32(8));

	clrsetbits_le32(&mctl_com->unk_0x500, BIT_U32(24), 0x300);
	/* this write seems to enable PHY MMIO region */
	setbits_le32(&mctl_com->unk_0x500, BIT_U32(24));

	if (!mctl_phy_init(para))
		return 0/*0*/;

	writel(0, &mctl_ctl->swctl);
	clrbits_le32(&mctl_ctl->rfshctl3, BIT_U32(0));

	setbits_le32(&mctl_com->unk_0x014, BIT_U32(31));
	writel(0xffffffff, &mctl_com->maer0);
	writel(0x7ff, &mctl_com->maer1);
	writel(0xffff, &mctl_com->maer2);

	writel(1, &mctl_ctl->swctl);
	mctl_await_completion(&mctl_ctl->swstat, 1, 1);

	return 1/*1*/;
}

static int mctl_core_init(struct xdram_para *para)
{
	mctl_sys_init(para);

	return mctl_ctrl_init(para);
}

static void mctl_auto_detect_rank_width(struct xdram_para *para)
{
	/* this is minimum size that it's supported */
	para->cols = 8;
	para->rows = 13;

	/*
	 * Strategy here is to test most demanding combination first and least
	 * demanding last, otherwise HW might not be fully utilized. For
	 * example, half bus width and rank = 1 combination would also work
	 * on HW with full bus width and rank = 2, but only 1/4 RAM would be
	 * visible.
	 */

	PRINTF("testing 32-bit width, rank = 2\n");
	para->bus_full_width = 1;
	para->ranks = 2;
	if (mctl_core_init(para))
		return;

	PRINTF("testing 32-bit width, rank = 1\n");
	para->bus_full_width = 1;
	para->ranks = 1;
	if (mctl_core_init(para))
		return;

	PRINTF("testing 16-bit width, rank = 2\n");
	para->bus_full_width = 0;
	para->ranks = 2;
	if (mctl_core_init(para))
		return;

	PRINTF("testing 16-bit width, rank = 1\n");
	para->bus_full_width = 0;
	para->ranks = 1;
	if (mctl_core_init(para))
		return;

	PRINTF("This DRAM setup is currently not supported.\n");
	for (;;)
		;
}

static void mctl_auto_detect_dram_size(struct xdram_para *para)
{
	/* detect row address bits */
	para->cols = 8;
	para->rows = 18;
	mctl_core_init(para);

	for (para->rows = 13; para->rows < 18; para->rows++) {
		/* 8 banks, 8 bit per byte and 16/32 bit width */
		if (mctl_mem_matches((1 << (para->rows + para->cols +
					    4 + para->bus_full_width))))
			break;
	}

	/* detect column address bits */
	para->cols = 11;
	mctl_core_init(para);

	for (para->cols = 8; para->cols < 11; para->cols++) {
		/* 8 bits per byte and 16/32 bit width */
		if (mctl_mem_matches(1 << (para->cols + 1 +
					   para->bus_full_width)))
			break;
	}
}

static unsigned long mctl_calc_size(struct xdram_para *para)
{
	uint8_t width = para->bus_full_width ? 4 : 2;

	/* 8 banks */
	return (1ULL << (para->cols + para->rows + 3)) * width * para->ranks;
}

static struct xdram_para xpara = {
	.clk = 800, //CONFIG_DRAM_CLK,
	.type = SUNXI_DRAM_TYPE_DDR3,
};

unsigned long sunxi_dram_init(void)
{
	struct sunxi_prcm_reg *const prcm =
		(struct sunxi_prcm_reg *)SUNXI_PRCM_BASE;
	unsigned long size;

	setbits_le32(&prcm->res_cal_ctrl, BIT_U32(8));
	clrbits_le32(&prcm->ohms240, 0x3f);

	mctl_auto_detect_rank_width(&xpara);
	mctl_auto_detect_dram_size(&xpara);

	mctl_core_init(&xpara);

	size = mctl_calc_size(&xpara);

	mctl_set_master_priority();

	return size;
}


void FLASHMEMINITFUNC arm_hardware_sdram_initialize0(void)
{
	PRINTF("arm_hardware_sdram_initialize start, cpux=%u MHz\n", (unsigned) (allwnr_t507_get_cpux_freq() / 1000 / 1000));
	unsigned long v = sunxi_dram_init();
	PRINTF("arm_hardware_sdram_initialize: v=%lu, %lu MB\n", v, v / 1024 / 1024);

	dbp();
	memset((void *) CONFIG_SYS_SDRAM_BASE + 0x00, 0xE5, 0x80);
	memset((void *) CONFIG_SYS_SDRAM_BASE + 0x80, 0xDF, 0x80);
	printhex(CONFIG_SYS_SDRAM_BASE, (void *) CONFIG_SYS_SDRAM_BASE, 2 * 0x80);

	PRINTF("arm_hardware_sdram_initialize done, ddr=%u MHz\n", (unsigned) (allwnr_t507_get_dram_freq() / 1000 / 1000));
}

#if 1


struct dram_para
{
	uint32_t clk;
	enum sunxi_dram_type type;
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
};

struct dram_timing
{
	uint32_t tccd;
	uint32_t tfaw;
	uint32_t trrd;
	uint32_t trcd;
	uint32_t trc;
	uint32_t txp;
	uint32_t trtp;
	uint32_t trp;
	uint32_t tras;
	uint16_t trefi;
	uint16_t trfc;
	uint16_t txsr;

	uint32_t tmrw;
	uint32_t tmrd;
	uint32_t tmod;
	uint32_t tcke;
	uint32_t tcksrx;
	uint32_t tcksre;
	uint32_t tckesr;
	uint32_t trasmax;
	uint32_t txs;
	uint32_t txsdll;
	uint32_t txsabort;
	uint32_t txsfas;
	uint32_t tcl;
	uint32_t tcwl;

	uint32_t twtp;
	uint32_t twr2rd;
	uint32_t trd2wr;

	uint32_t unk_4;
	uint32_t unk_42;
	uint32_t unk_43;
	uint32_t unk_44;
	uint32_t unk_50;
	uint32_t unk_63;
	uint32_t unk_66;
	uint32_t unk_69;
};

static struct sunxi_ccm_reg *const ccm = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
static struct sunxi_prcm_reg *const prcm = (struct sunxi_prcm_reg *)SUNXI_PRCM_BASE;
static struct sunxi_mctl_com_reg *const mctl_com = (struct sunxi_mctl_com_reg *)SUNXI_DRAM_COM_BASE;
static struct sunxi_mctl_ctl_reg *const mctl_ctl = (struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;

static struct dram_timing channel_timing = {
	.trtp = 3,
	.unk_4 = 3,
	.trp = 6,
	.tckesr = 3,
	.trcd = 6,
	.trrd = 3,
	.tmod = 6,
	.unk_42 = 4,
	.txsr = 4,
	.txs = 4,
	.unk_66 = 8,
	.unk_69 = 8,
	.unk_50 = 1,
	.unk_63 = 2,
	.tcksre = 4,
	.tcksrx = 4,
	.trd2wr = 4,
	.trasmax = 27,
	.twr2rd = 8,
	.twtp = 12,
	.trfc = 128,
	.trefi = 98,
	.txp = 10,
	.tfaw = 16,
	.tras = 14,
	.trc = 20,
	.tcke = 2,
	.tmrw = 0,
	.tccd = 2,
	.tmrd = 2,
	.tcwl = 3,
	.tcl = 3,
	.unk_43 = 1,
	.unk_44 = 1,
};

static const unsigned char phy_init_ddr3_a[] = {
	0x0C, 0x08, 0x19, 0x18, 0x10, 0x06, 0x0A, 0x03,
	0x0E, 0x00, 0x0B, 0x05, 0x09, 0x1A, 0x04, 0x13,
	0x16, 0x11, 0x01, 0x15, 0x0D, 0x07, 0x12, 0x17,
	0x14, 0x02, 0x0F};
static const unsigned char phy_init_lpddr3_a[] = {
	0x08, 0x03, 0x02, 0x00, 0x18, 0x19, 0x09, 0x01,
	0x06, 0x17, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x04,
	0x05, 0x07, 0x1A};
static const unsigned char phy_init_ddr4_a[] = {
	0x19, 0x1A, 0x04, 0x12, 0x09, 0x06, 0x08, 0x0A,
	0x16, 0x17, 0x18, 0x0F, 0x0C, 0x13, 0x02, 0x05,
	0x01, 0x11, 0x0E, 0x00, 0x0B, 0x07, 0x03, 0x14,
	0x15, 0x0D, 0x10};
static const unsigned char phy_init_lpddr4_a[] = {
	0x01, 0x05, 0x02, 0x00, 0x19, 0x03, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x04, 0x1A};

static const unsigned char phy_init_ddr3_b[] = {
	0x03, 0x19, 0x18, 0x02, 0x10, 0x15, 0x16, 0x07,
	0x06, 0x0E, 0x05, 0x08, 0x0D, 0x04, 0x17, 0x1A,
	0x13, 0x11, 0x12, 0x14, 0x00, 0x01, 0xC, 0x0A,
	0x09, 0x0B, 0x0F};
static const unsigned char phy_init_lpddr3_b[] = {
	0x05, 0x06, 0x17, 0x02, 0x19, 0x18, 0x04, 0x07,
	0x03, 0x01, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x08,
	0x09, 0x00, 0x1A};
static const unsigned char phy_init_ddr4_b[] = {
	0x13, 0x17, 0xE, 0x01, 0x06, 0x12, 0x14, 0x07,
	0x09, 0x02, 0x0F, 0x00, 0x0D, 0x05, 0x16, 0x0C,
	0x0A, 0x11, 0x04, 0x03, 0x18, 0x15, 0x08, 0x10,
	0x0B, 0x19, 0x1A};
static const unsigned char phy_init_lpddr4_b[] = {
	0x01, 0x03, 0x02, 0x19, 0x17, 0x00, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x04,
	0x18, 0x05, 0x1A};

static void libdram_mctl_await_completion(uint32_t *reg, uint32_t mask, uint32_t val)
{
	PRINTF("libdram_mctl_await_completion %p\n", reg);
	//unsigned long tmo = timer_get_us() + 1000000;

	while ((readl(reg) & mask) != val)
	{
//		if (timer_get_us() > tmo)
//			panic("Timeout initialising DRAM\n");
	}
	PRINTF("libdram_mctl_await_completion done\n");
}

static int libdram_dramc_simple_wr_test(uint32_t dram_size, uint32_t test_range)
{
	volatile uint32_t *dram_memory = (volatile uint32_t *)CONFIG_SYS_SDRAM_BASE;
	uint32_t step = dram_size * 1024 * 1024 / 8;

	for (unsigned i = 0; i < test_range; i++)
	{
		dram_memory[i] = i + 0x1234567;
		dram_memory[i + step] = i - 0x1234568;
	}

	for (unsigned i = 0; i < test_range; i++)
	{
		volatile uint32_t *ptr;
		if (dram_memory[i] != i + 0x1234567)
		{
			ptr = &dram_memory[i];
			goto fail;
		}
		if (dram_memory[i + step] != i - 0x1234568)
		{
			ptr = &dram_memory[i + step];
			goto fail;
		}
		continue;
	fail:
		PRINTF("DRAM simple test FAIL----- at address %p\n", ptr);
		return 1;
	}

	PRINTF("DRAM simple test OK.\n");
	return 0;
}

static uint32_t libdram_DRAMC_get_dram_size(struct dram_para *para)
{
	uint32_t size_bits, size;

	size_bits = (para->para2 & 0xFFFF) >> 12;
	size_bits += (para->para1 & 0xFFFF) >> 14;
	size_bits += (para->para1 >> 4) & 0xFF;
	size_bits += (para->para1 >> 12) & 3;
	size_bits += para->para1 & 0xF;

	if (para->para2 & 0xF)
		size_bits -= 19;
	else
		size_bits -= 18;

	size = 1 << size_bits;
	if (para->tpr13 & 0x70000)
	{
		if (para->para2 >> 30 != 2)
			size = (3 * size) >> 2;
	}

	return size;
}

static void libdram_ccm_set_pll_ddr0_sccg(struct dram_para *para)
{
	switch ((para->tpr13 >> 20) & 7)
	{
	case 0u:
		break;
	case 1u:
		ccm->pll5_pat = 0xE486CCCC;
		break;
	case 2u:
		ccm->pll5_pat = 0xE486CCCC;
		break;
	case 3u:
		ccm->pll5_pat = 0xE486CCCC;
		break;
	case 5u:
		ccm->pll5_pat = 0xE486CCCC;
		break;
	default:
		ccm->pll5_pat = 0xE486CCCC;
		break;
	}
	ccm->pll5_cfg |= 0x1000000u;
}

static void libdram_mctl_sys_init(struct dram_para *para)
{
	return;
	/* Put all DRAM-related blocks to reset state */
	clrbits_le32(&ccm->mbus_cfg, MBUS_ENABLE);
	clrbits_le32(&ccm->mbus_cfg, MBUS_RESET);
	PRINTF("dram_gate_reset: %08X\n", (unsigned) readl(&ccm->dram_gate_reset));
	clrbits_le32(&ccm->dram_gate_reset, BIT_U32(GATE_SHIFT));	// !!!
	PRINTF("dram_gate_reset: %08X\n", (unsigned) readl(&ccm->dram_gate_reset));
	clrbits_le32(&ccm->dram_gate_reset, BIT_U32(RESET_SHIFT));	// !!!
	PRINTF("dram_gate_reset: %08X\n", (unsigned) readl(&ccm->dram_gate_reset));
	clrbits_le32(&ccm->pll5_cfg, CCM_PLL5_CTRL_EN);
	clrbits_le32(&ccm->dram_clk_cfg, DRAM_MOD_RESET);

	udelay(5);
	libdram_ccm_set_pll_ddr0_sccg(para);
	clrsetbits_le32(&ccm->pll5_cfg, 0xff03, CCM_PLL5_CTRL_EN | CCM_PLL5_LOCK_EN | CCM_PLL5_OUT_EN | CCM_PLL5_CTRL_N(para->clk * 2 / 24));
	libdram_mctl_await_completion(&ccm->pll5_cfg, CCM_PLL5_LOCK, CCM_PLL5_LOCK);
	unsigned ctrl_freq = ((((CCU->PLL_DDR0_CTRL_REG >> 8) & 0xFF) + 1) * 24) / 2;

	/* Configure DRAM mod clock */
	clrbits_le32(&ccm->dram_clk_cfg, 0x3000000);
	clrsetbits_le32(&ccm->dram_clk_cfg, 0x800001F, DRAM_CLK_ENABLE | BIT_U32(0) | 0*BIT_U32(1)); // FACTOR_N = 3
	writel(BIT_U32(RESET_SHIFT), &ccm->dram_gate_reset);
	setbits_le32(&ccm->dram_gate_reset, BIT_U32(GATE_SHIFT));
	PRINTF("start: dram_gate_reset: %08X\n", (unsigned) readl(&ccm->dram_gate_reset));

	/* Configure MBUS and enable DRAM mod reset */
	setbits_le32(&ccm->mbus_cfg, MBUS_RESET);
	setbits_le32(&ccm->mbus_cfg, MBUS_ENABLE);
	setbits_le32(&ccm->dram_clk_cfg, DRAM_MOD_RESET);
	udelay(5);
}

static void libdram_mctl_com_set_bus_config(struct dram_para *para)
{
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
	{
		(*((volatile uint32_t *)0x03102ea8)) |= 0x1; // NSI Register ??
	}
	clrsetbits_le32(&mctl_ctl->sched[0], 0xff00, 0x3000);
	if ((para->tpr13 & 0x10000000) != 0)
	{
		clrsetbits_le32(&mctl_ctl->sched[0], 0xf, 0x1);
		PRINTF("MX_SCHED(0x04820250) = %p \n", &mctl_ctl->sched[0]);
	}
}

static void libdram_mctl_com_set_controller_config(struct dram_para *para)
{
	uint32_t val = 0;

	switch (para->type)
	{
	case SUNXI_DRAM_TYPE_DDR3:
		val = MSTR_BURST_LENGTH(8) | MSTR_DEVICETYPE_DDR3;
		break;

	case SUNXI_DRAM_TYPE_DDR4:
		val = MSTR_BURST_LENGTH(8) | MSTR_DEVICETYPE_DDR4;
		break;

	case SUNXI_DRAM_TYPE_LPDDR3:
		val = MSTR_BURST_LENGTH(8) | MSTR_DEVICETYPE_LPDDR3;
		break;

	case SUNXI_DRAM_TYPE_LPDDR4:
		val = MSTR_BURST_LENGTH(16) | MSTR_DEVICETYPE_LPDDR4;
		break;
	}
	val |= (((para->para2 >> 11) & 6) + 1) << 24;
	val |= (para->para2 << 12) & 0x1000;
	writel(BIT_U32(31) | BIT_U32(30) | val, &mctl_ctl->mstr);
}

static void libdram_mctl_com_set_controller_geardown_mode(struct dram_para *para)
{
	if (para->tpr13 & BIT_U32(30))
	{
		setbits_le32(&mctl_ctl->mstr, MSTR_DEVICETYPE_DDR3);
	}
}

static void libdram_mctl_com_set_controller_2T_mode(struct dram_para *para)
{
	if ((mctl_ctl->mstr & 0x800) != 0 || (para->tpr13 & 0x20) != 0)
	{
		clrbits_le32(&mctl_ctl->mstr, MSTR_2TMODE);
	}
	else
	{
		setbits_le32(&mctl_ctl->mstr, MSTR_2TMODE);
	}
}

static void libdram_mctl_com_set_controller_odt(struct dram_para *para)
{
	uint32_t val = 0;

	if ((para->para2 & 0x1000) == 0)
		writel(0x0201, &mctl_ctl->odtmap);
	else
		writel(0x0303, &mctl_ctl->odtmap);

	switch (para->type)
	{
	case SUNXI_DRAM_TYPE_DDR3:
		val = 0x06000400;
		break;

	case SUNXI_DRAM_TYPE_DDR4:
		val = ((para->mr4 << 10) & 0x70000) | 0x400 | ((((para->mr4 >> 12) & 1) + 6) << 24);
		break;

	case SUNXI_DRAM_TYPE_LPDDR3:
		if (para->clk >= 400)
			val = ((7 * para->clk / 2000 + 7) << 24) | 0x400 | ((4 - 7 * para->clk / 2000) << 16);
		else
			val = ((7 * para->clk / 2000 + 7) << 24) | 0x400 | ((3 - 7 * para->clk / 2000) << 16);
		break;

	case SUNXI_DRAM_TYPE_LPDDR4:
		val = 0x04000400;
		break;
	}
	writel(val, &mctl_ctl->odtcfg);
	writel(val, &mctl_ctl->unk_0x2240);
	writel(val, &mctl_ctl->unk_0x3240);
	writel(val, &mctl_ctl->unk_0x4240);
}

static void libdram_mctl_com_set_controller_address_map(struct dram_para *para)
{
	uint8_t cols, rows, ranks;
	uint32_t unk_2, unk_5, unk_16;

	cols = para->para1 & 0xF;
	rows = (para->para1 >> 4) & 0xFF;
	ranks = (para->tpr13 >> 16) & 7;

	unk_2 = (para->para1 >> 12) & 3;
	unk_5 = (para->para1 & 0xFFFF) >> 14;

	if (para->para2 << 28)
		cols -= 1;

	/* Columns */
	mctl_ctl->addrmap[2] = (unk_5 << 8) | (unk_5 << 16) | (unk_5 << 24);
	switch (cols)
	{
	case 8:
		mctl_ctl->addrmap[3] = 0x1F1F0000 | unk_5 | (unk_5 << 8);
		mctl_ctl->addrmap[4] = 0x1F1F;
		break;

	case 9:
		mctl_ctl->addrmap[3] = 0x1F000000 | unk_5 | (unk_5 << 8) | (unk_5 << 16);
		mctl_ctl->addrmap[4] = 0x1F1F;
		break;

	case 10:
		mctl_ctl->addrmap[3] = unk_5 | (unk_5 << 8) | (unk_5 << 16) | (unk_5 << 24);
		mctl_ctl->addrmap[4] = 0x1F1F;
		break;

	case 11:
		mctl_ctl->addrmap[3] = unk_5 | (unk_5 << 8) | (unk_5 << 16) | (unk_5 << 24);
		mctl_ctl->addrmap[4] = 0x1F00 | unk_5;
		break;

	default:
		mctl_ctl->addrmap[3] = unk_5 | (unk_5 << 8) | (unk_5 << 16) | (unk_5 << 24);
		mctl_ctl->addrmap[4] = unk_5 | (unk_5 << 8);
		break;
	}

	/* Bank groups */
	switch (unk_5)
	{
	case 1:
		mctl_ctl->addrmap[8] = 0x3f01;
		break;

	case 2:
		mctl_ctl->addrmap[8] = 0x101;
		break;

	default:
		mctl_ctl->addrmap[8] = 0x3f3f;
		break;
	}

	/* Banks */
	if (unk_2 == 3)
	{
		mctl_ctl->addrmap[1] = (unk_5 - 2 + cols) | ((unk_5 - 2 + cols) << 8) | ((unk_5 - 2 + cols) << 16);
	}
	else
	{
		mctl_ctl->addrmap[1] = (unk_5 - 2 + cols) | ((unk_5 - 2 + cols) << 8) | 0x3F0000;
	}

	/* Rows */
	unk_16 = unk_5 + unk_2 + cols;
	mctl_ctl->addrmap[5] = (unk_16 - 6) | ((unk_16 - 6) << 8) | ((unk_16 - 6) << 16) | ((unk_16 - 6) << 24);
	switch (rows)
	{
	case 14:
		mctl_ctl->addrmap[6] = (unk_16 - 6) | ((unk_16 - 6) << 8) | 0x0F0F0000;
		mctl_ctl->addrmap[7] = 0x0F0F;
		break;

	case 15:
		if ((ranks == 1 && cols == 11) || (ranks == 2 && cols == 10))
		{
			mctl_ctl->addrmap[6] = (unk_16 - 6) | ((unk_16 - 5) << 8) | ((unk_16 - 5) << 16) | 0x0F000000;
			mctl_ctl->addrmap[0] = unk_16 + 7;
		}
		else
		{
			mctl_ctl->addrmap[6] = (unk_16 - 6) | ((unk_16 - 6) << 8) | ((unk_16 - 6) << 16) | 0x0F000000;
		}
		mctl_ctl->addrmap[7] = 0x0F0F;
		break;

	case 16:
		if (ranks == 1 && cols == 10)
		{
			mctl_ctl->addrmap[6] = (unk_16 - 6) | ((unk_16 - 6) << 8) | ((unk_16 - 5) << 16) | ((unk_16 - 5) << 24);
			mctl_ctl->addrmap[0] = unk_16 + 8;
		}
		else
		{
			mctl_ctl->addrmap[6] = (unk_16 - 6) | ((unk_16 - 6) << 8) | ((unk_16 - 6) << 16) | ((unk_16 - 6) << 24);
		}
		mctl_ctl->addrmap[7] = 0x0F0F;
		break;

	case 17:
		mctl_ctl->addrmap[6] = (unk_16 - 6) | ((unk_16 - 6) << 8) | ((unk_16 - 6) << 16) | ((unk_16 - 6) << 24);
		mctl_ctl->addrmap[7] = (unk_16 - 6) | 0x0F00;
		break;

	default:
		mctl_ctl->addrmap[6] = (unk_16 - 6) | ((unk_16 - 6) << 8) | ((unk_16 - 6) << 16) | ((unk_16 - 6) << 24);
		mctl_ctl->addrmap[7] = (unk_16 - 6) | ((unk_16 - 6) << 8);
		break;
	}

	if (para->para2 & 0x1000)
	{
		if (ranks < 2)
		{
			mctl_ctl->addrmap[0] = rows - 6 + unk_16;
		}
	}
	else
	{
		mctl_ctl->addrmap[0] = 0x1F;
	}
}

static uint32_t libdram_auto_cal_timing(int a1, int a2)
{
	unsigned int result;

	result = a2 * a1 / 1000;
	if (a2 * a1 % 1000)
		++result;
	return result;
}

static void libdram_mctl_com_set_channel_timing(struct dram_para *para)
{
	uint32_t ctrl_freq;
	//ctrl_freq = (((*((volatile uint32_t *)0x3001011)) + 1) * 24) >> 2;
	ctrl_freq = ((((CCU->PLL_DDR0_CTRL_REG >> 8) & 0xFF) + 1) * 24) / 2;
	switch (para->type)
	{
	case SUNXI_DRAM_TYPE_DDR3:
		channel_timing.tfaw = libdram_auto_cal_timing(50, ctrl_freq);
		channel_timing.trrd = libdram_auto_cal_timing(10, ctrl_freq);
		if (channel_timing.trrd < 2u)
			channel_timing.trrd = 2;
		channel_timing.trcd = libdram_auto_cal_timing(15, ctrl_freq);
		channel_timing.trc = libdram_auto_cal_timing(53, ctrl_freq);
		channel_timing.trtp = libdram_auto_cal_timing(8, ctrl_freq);
		if (channel_timing.trtp < 2)
			channel_timing.trtp = 2;
		channel_timing.tras = libdram_auto_cal_timing(38, ctrl_freq);
		channel_timing.unk_4 = channel_timing.trtp;
		channel_timing.trefi = libdram_auto_cal_timing(7800, ctrl_freq) >> 5;
		channel_timing.trfc = libdram_auto_cal_timing(350, ctrl_freq);
		channel_timing.trp = channel_timing.trcd;
		channel_timing.txs = libdram_auto_cal_timing(360, ctrl_freq) >> 5;
		channel_timing.txp = channel_timing.trtp;
		channel_timing.tccd = 2;
		channel_timing.tcke = libdram_auto_cal_timing(8, ctrl_freq);
		channel_timing.tcksrx = libdram_auto_cal_timing(10, ctrl_freq);
		if (channel_timing.tcksrx <= 2)
		{
			channel_timing.tcke = 6;
		}
		else
		{
			if (channel_timing.tcke < 2u)
				channel_timing.tcke = 2;
		}
		channel_timing.tckesr = channel_timing.tcke + 1;
		channel_timing.trasmax = ctrl_freq / 15;
		para->mr0 = 0x1F14;
		para->mr2 &= ~0x38;
		para->mr2 |= 0x20;
		para->mr3 = 0;
		if ((int)(channel_timing.trtp + channel_timing.trp) <= 8)
			channel_timing.trtp = 9 - channel_timing.trp;
		channel_timing.twr2rd = channel_timing.unk_4 + 7;
		channel_timing.tcksre = channel_timing.tcksrx;
		channel_timing.trd2wr = 5;
		channel_timing.twtp = 14;
		channel_timing.tmod = 12;
		channel_timing.tmrd = 4;
		channel_timing.tmrw = 0;
		channel_timing.tcwl = 5;
		channel_timing.tcl = 7;
		channel_timing.unk_44 = 6;
		channel_timing.unk_43 = 10;
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		channel_timing.tfaw = libdram_auto_cal_timing(50, ctrl_freq);
		if (channel_timing.tfaw < 4)
			channel_timing.tfaw = 4;
		channel_timing.trrd = libdram_auto_cal_timing(10, ctrl_freq);
		if (channel_timing.trrd < 1)
			channel_timing.trrd = 1;
		channel_timing.trcd = libdram_auto_cal_timing(10, ctrl_freq);
		if (channel_timing.trcd < 1)
			channel_timing.trcd = 1;
		channel_timing.trc = libdram_auto_cal_timing(70, ctrl_freq);
		channel_timing.trtp = libdram_auto_cal_timing(8, ctrl_freq);
		if (channel_timing.trtp < 2)
			channel_timing.trtp = 2;
		channel_timing.trp = libdram_auto_cal_timing(27, ctrl_freq);
		channel_timing.tras = libdram_auto_cal_timing(42, ctrl_freq);
		channel_timing.unk_4 = channel_timing.trtp;
		channel_timing.trefi = libdram_auto_cal_timing(3900, ctrl_freq) >> 5;
		channel_timing.trfc = libdram_auto_cal_timing(210, ctrl_freq);
		channel_timing.txp = channel_timing.trtp;
		channel_timing.txsr = libdram_auto_cal_timing(220, ctrl_freq);
		channel_timing.tccd = 2;
		para->mr0 = 0;
		para->mr1 = 0x83;
		para->mr2 = 0x1c;
		channel_timing.tcke = 3;
		channel_timing.twr2rd = channel_timing.unk_4 + 9;
		channel_timing.tcksre = 5;
		channel_timing.tcksrx = 5;
		channel_timing.tckesr = 5;
		channel_timing.trd2wr = 0xd;
		channel_timing.trasmax = 0x18;
		channel_timing.twtp = 0x10;
		channel_timing.tmod = 0xc;
		channel_timing.tmrd = 5;
		channel_timing.tmrw = 5;
		channel_timing.tcwl = 4;
		channel_timing.tcl = 7;
		channel_timing.unk_44 = 6;
		channel_timing.unk_43 = 0xc;
		break;
	case SUNXI_DRAM_TYPE_DDR4:
		channel_timing.tfaw = libdram_auto_cal_timing(35, ctrl_freq);
		channel_timing.trrd = libdram_auto_cal_timing(8, ctrl_freq);
		if (channel_timing.trrd < 2u)
			channel_timing.trrd = 2;
		channel_timing.txp = libdram_auto_cal_timing(6, ctrl_freq);
		if (channel_timing.txp < 2u)
			channel_timing.txp = 2;
		channel_timing.unk_66 = libdram_auto_cal_timing(10, ctrl_freq);
		if (channel_timing.unk_66 < 8u)
			channel_timing.unk_66 = 8;
		channel_timing.trcd = libdram_auto_cal_timing(15, ctrl_freq);
		channel_timing.trc = libdram_auto_cal_timing(49, ctrl_freq);
		channel_timing.unk_50 = libdram_auto_cal_timing(3, ctrl_freq);
		if (!channel_timing.unk_50)
			channel_timing.unk_50 = 1;
		channel_timing.tras = libdram_auto_cal_timing(34, ctrl_freq);
		channel_timing.trefi = libdram_auto_cal_timing(7800, ctrl_freq) >> 5;
		channel_timing.trfc = libdram_auto_cal_timing(350, ctrl_freq);
		channel_timing.unk_4 = channel_timing.trrd;
		channel_timing.txs = libdram_auto_cal_timing(360, ctrl_freq) >> 5;
		channel_timing.trp = channel_timing.trcd;
		channel_timing.unk_63 = channel_timing.txp;
		channel_timing.tccd = 3;
		channel_timing.tmod = libdram_auto_cal_timing(15, ctrl_freq);
		if (channel_timing.tmod < 12)
			channel_timing.tmod = 12;
		channel_timing.tcke = libdram_auto_cal_timing(5, ctrl_freq);
		if (channel_timing.tcke < 2)
			channel_timing.tcke = 2;
		channel_timing.tckesr = channel_timing.tcke + 1;
		channel_timing.tcksrx = libdram_auto_cal_timing(10, ctrl_freq);
		if (channel_timing.tcksrx < 3u)
			channel_timing.tcksrx = 3;
		channel_timing.unk_42 = libdram_auto_cal_timing(170, ctrl_freq) >> 5;
		channel_timing.trasmax = libdram_auto_cal_timing(70200, ctrl_freq) >> 10;
		if (channel_timing.trp > 4)
			channel_timing.trtp = 4;
		else
			channel_timing.trtp = 9 - channel_timing.trp;
		if (channel_timing.trp <= 4)
			channel_timing.trtp = channel_timing.trtp;
		para->mr2 &= ~0x38;
		para->mr2 |= 8;
		para->mr0 = 1312;
		channel_timing.twr2rd = channel_timing.unk_4 + 7;
		channel_timing.unk_69 = channel_timing.unk_50 + 7;
		channel_timing.tcksre = channel_timing.tcksrx;
		channel_timing.trd2wr = 5;
		channel_timing.twtp = 14;
		channel_timing.tmrd = 4;
		channel_timing.tmrw = 0;
		channel_timing.tcwl = 5;
		channel_timing.tcl = 7;
		channel_timing.unk_44 = 6;
		channel_timing.unk_43 = 10;
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		channel_timing.tfaw = libdram_auto_cal_timing(40, ctrl_freq);
		channel_timing.unk_4 = libdram_auto_cal_timing(10, ctrl_freq);
		channel_timing.trrd = channel_timing.unk_4;
		if (channel_timing.trrd < 2)
			channel_timing.trrd = 2;
		channel_timing.trcd = libdram_auto_cal_timing(18, ctrl_freq);
		if (channel_timing.trcd < 2)
			channel_timing.trcd = 2;
		channel_timing.trc = libdram_auto_cal_timing(65, ctrl_freq);
		channel_timing.trtp = libdram_auto_cal_timing(8, ctrl_freq);
		channel_timing.txp = channel_timing.trtp;
		if (channel_timing.txp < 2)
			channel_timing.txp = 2;
		if (para->tpr13 & 0x10000000)
		{
			channel_timing.unk_4 = libdram_auto_cal_timing(12, ctrl_freq);
			channel_timing.tcl = 11;
			channel_timing.unk_44 = 5;
			channel_timing.unk_43 = 19;
		}
		else
		{
			channel_timing.tcl = 10;
			channel_timing.unk_44 = 5;
			channel_timing.unk_43 = 17;
		}
		if (channel_timing.unk_4 < 4)
			channel_timing.unk_4 = 4;
		if (channel_timing.trtp < 4)
			channel_timing.trtp = 4;
		channel_timing.trp = libdram_auto_cal_timing(21, ctrl_freq);
		channel_timing.tras = libdram_auto_cal_timing(42, ctrl_freq);
		channel_timing.trefi = libdram_auto_cal_timing(3904, ctrl_freq) >> 5;
		channel_timing.trfc = libdram_auto_cal_timing(280, ctrl_freq);
		channel_timing.txsr = libdram_auto_cal_timing(290, ctrl_freq);
		channel_timing.tccd = 4;
		channel_timing.tmrw = libdram_auto_cal_timing(14, ctrl_freq);
		if (channel_timing.tmrw < 5)
			channel_timing.tmrw = 5;
		channel_timing.tcke = libdram_auto_cal_timing(15, ctrl_freq);
		if (channel_timing.tcke < 2)
			channel_timing.tcke = 2;
		channel_timing.tcksrx = libdram_auto_cal_timing(2, ctrl_freq);
		if (channel_timing.tcksrx < 2)
			channel_timing.tcksrx = 2;
		channel_timing.tcksre = libdram_auto_cal_timing(5, ctrl_freq);
		if (channel_timing.tcksre < 2)
			channel_timing.tcksre = 2;
		channel_timing.trasmax = (uint32_t)(9 * channel_timing.trefi) >> 5;
		para->mr1 = 52;
		para->mr2 = 27;
		channel_timing.trd2wr = (libdram_auto_cal_timing(4, ctrl_freq) + 17) - (libdram_auto_cal_timing(1, ctrl_freq));
		channel_timing.tckesr = channel_timing.tcke;
		channel_timing.trtp = 4;
		channel_timing.twr2rd = channel_timing.unk_4 + 14;
		channel_timing.tmrd = channel_timing.tmrw;
		channel_timing.twtp = 24;
		channel_timing.tmod = 12;
		channel_timing.tcwl = 5;
		break;
	}
	writel((channel_timing.twtp << 24) | (channel_timing.tfaw << 16) | (channel_timing.trasmax << 8) | channel_timing.tras, &mctl_ctl->dramtmg[0]);
	writel((channel_timing.txp << 16) | (channel_timing.trtp << 8) | channel_timing.trc, &mctl_ctl->dramtmg[1]);
	writel((channel_timing.tcwl << 24) | (channel_timing.tcl << 16) | (channel_timing.trd2wr << 8) | channel_timing.twr2rd, &mctl_ctl->dramtmg[2]);
	writel((channel_timing.tmrw << 20) | (channel_timing.tmrd << 12) | channel_timing.tmod, &mctl_ctl->dramtmg[3]);
	writel((channel_timing.trcd << 24) | (channel_timing.tccd << 16) | (channel_timing.trrd << 8) | channel_timing.trp, &mctl_ctl->dramtmg[4]);
	writel((channel_timing.tcksrx << 24) | (channel_timing.tcksre << 16) | (channel_timing.tckesr << 8) | channel_timing.tcke, &mctl_ctl->dramtmg[5]);
	writel((channel_timing.txp + 2) | 0x02020000, &mctl_ctl->dramtmg[6]);
	writel((channel_timing.unk_42 << 24) | (channel_timing.unk_42 << 16) | 0x1000 | channel_timing.txs, &mctl_ctl->dramtmg[8]);
	writel(channel_timing.unk_69 | (channel_timing.unk_63 << 8) | 0x20000, &mctl_ctl->dramtmg[9]);
	writel(0xE0C05, &mctl_ctl->dramtmg[10]);
	writel(0x440C021C, &mctl_ctl->dramtmg[11]);
	writel(channel_timing.unk_66, &mctl_ctl->dramtmg[12]);
	writel(0xA100002, &mctl_ctl->dramtmg[13]);
	writel(channel_timing.txsr, &mctl_ctl->dramtmg[14]);

	switch (para->type)
	{
	case SUNXI_DRAM_TYPE_DDR3:
		clrbits_le32(&mctl_ctl->init[0], 0xC0000000);
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		clrsetbits_le32(&mctl_ctl->init[0], 0xC3FF0000 | 0xC0000FFF, 0x4F0000 | 0x112);
		break;
	case SUNXI_DRAM_TYPE_DDR4:
		clrbits_le32(&mctl_ctl->init[0], 0xC0000000);
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		clrsetbits_le32(&mctl_ctl->init[0], 0xC0000FFF, 0x3F0);
		break;
	}

	if (para->tpr13 & 8)
	{
		writel(0x420000, &mctl_ctl->init[1]);
	}
	else
	{
		writel(0x1F20000, &mctl_ctl->init[1]);
	}

	clrsetbits_le32(&mctl_ctl->init[2], 0xFF0F, 0xFF0F);
	writel(0, &mctl_ctl->dfimisc);

	switch (para->type)
	{
	case SUNXI_DRAM_TYPE_DDR4:
		writel(para->mr5 | (para->mr4 << 16), &mctl_ctl->init[6]);
		writel(para->mr6, &mctl_ctl->init[7]);
	case SUNXI_DRAM_TYPE_DDR3:
		writel(para->mr1 | (para->mr0 << 16), &mctl_ctl->init[3]);
		writel(para->mr3 | (para->mr2 << 16), &mctl_ctl->init[4]);
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		writel(para->mr12 | (para->mr11 << 16), &mctl_ctl->init[6]);
		writel(para->mr14 | (para->mr22 << 16), &mctl_ctl->init[7]);
	case SUNXI_DRAM_TYPE_LPDDR3:
		writel(para->mr2 | (para->mr1 << 16), &mctl_ctl->init[3]);
		writel(para->mr3 << 16, &mctl_ctl->init[4]);
		break;
	}

	clrsetbits_le32(&mctl_ctl->rankctl, 0xff0, 0x660);

	if (para->tpr13 & 0x20)
	{
		writel((channel_timing.unk_44) | 0x2000000 | (channel_timing.unk_43 << 16) | 0x808000, &mctl_ctl->dfitmg0);
	}
	else
	{
		writel((channel_timing.unk_44 - 1) | 0x2000000 | ((channel_timing.unk_43 - 1) << 16) | 0x808000, &mctl_ctl->dfitmg0);
	}

	writel(0x100202, &mctl_ctl->dfitmg1);

	writel(channel_timing.trfc | (channel_timing.trefi << 16), &mctl_ctl->rfshtmg);
}

static void libdram_mctl_com_set_controller_update(void)
{
	setbits_le32(&mctl_ctl->dfiupd[0], BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->zqctl[0], BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x2180, BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x3180, BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x4180, BIT_U32(31) | BIT_U32(30));
}

static void libdram_mctl_com_set_controller_dbi(struct dram_para *para)
{
	if ((para->tpr13 & 0x20000000) != 0)
		setbits_le32(&mctl_ctl->dbictl, 4);
}

static void libdram_mctl_com_set_controller_refresh(int val)
{
	clrsetbits_le32(&mctl_ctl->rfshctl3, BIT_U32(0), val & BIT_U32(0));
}

static void libdram_mctl_com_set_controller_before_phy(void)
{
	libdram_mctl_com_set_controller_refresh(1);
	clrbits_le32(&mctl_ctl->dfimisc, 1);
	writel(0x20, &mctl_ctl->pwrctl);
}

static void libdram_mctl_com_init(struct dram_para *para)
{
	libdram_mctl_com_set_controller_config(para);

	if (para->type == SUNXI_DRAM_TYPE_DDR4)
	{
		libdram_mctl_com_set_controller_geardown_mode(para);
	}

	if (para->type == SUNXI_DRAM_TYPE_DDR3 || para->type == SUNXI_DRAM_TYPE_DDR4)
	{
		libdram_mctl_com_set_controller_2T_mode(para);
	}

	libdram_mctl_com_set_controller_odt(para);
	// mctl_com_set_controller_address_map(para);
	// PRINTF("external 2: %x, 3: %x, 4:  %x\n", mctl_ctl->addrmap[2], mctl_ctl->addrmap[3], mctl_ctl->addrmap[4]);
	// PRINTF("external 8: %x, 1: %x, 5:  %x\n", mctl_ctl->addrmap[8], mctl_ctl->addrmap[1], mctl_ctl->addrmap[5]);
	// PRINTF("external 6: %x, 7: %x, 0:  %x\n", mctl_ctl->addrmap[6], mctl_ctl->addrmap[7], mctl_ctl->addrmap[0]);
	libdram_mctl_com_set_controller_address_map(para);
	// PRINTF("internal 2: %x, 3: %x, 4:  %x\n", mctl_ctl->addrmap[2], mctl_ctl->addrmap[3], mctl_ctl->addrmap[4]);
	// PRINTF("internal 8: %x, 1: %x, 5:  %x\n", mctl_ctl->addrmap[8], mctl_ctl->addrmap[1], mctl_ctl->addrmap[5]);
	// PRINTF("internal 6: %x, 7: %x, 0:  %x\n", mctl_ctl->addrmap[6], mctl_ctl->addrmap[7], mctl_ctl->addrmap[0]);

	libdram_mctl_com_set_channel_timing(para);
	// for (int i = 0; i < 17; i++)
	// {
	// 	PRINTF("mctl_ctl->dramtmg[%d]: %x\n", i, mctl_ctl->dramtmg[i]);
	// }

	writel(0, &mctl_ctl->pwrctl);

	libdram_mctl_com_set_controller_update();

	if (para->type == SUNXI_DRAM_TYPE_DDR4 || para->type == SUNXI_DRAM_TYPE_LPDDR4)
	{
		libdram_mctl_com_set_controller_dbi(para);
	}

	libdram_mctl_com_set_controller_before_phy();
}

static void libdram_mctl_com_set_controller_after_phy(void)
{
	writel(0, &mctl_ctl->swctl);
	libdram_mctl_com_set_controller_refresh(0);
	writel(1, &mctl_ctl->swctl);
	libdram_mctl_await_completion(&mctl_ctl->swstat, 1, 1);
}

static void libdram_mctl_phy_cold_reset(void)
{
	clrsetbits_le32(&mctl_com->unk_0x008, 0x1000200, 0x200);
	udelay(1);
	setbits_le32(&mctl_com->unk_0x008, 0x1000000);
}

static void libdram_mctl_phy_set_address_remapping(struct dram_para *para)
{
	int i;
	uint32_t *ptr;
	const uint8_t *phy_init = NULL;

	switch (readl(SUNXI_SID_BASE))
	{
	case 0x800:
	case 0x2400:
		switch (para->type)
		{
		case SUNXI_DRAM_TYPE_DDR3:
			phy_init = phy_init_ddr3_a;
			break;
		case SUNXI_DRAM_TYPE_LPDDR3:
			phy_init = phy_init_lpddr3_a;
			break;
		case SUNXI_DRAM_TYPE_DDR4:
			phy_init = phy_init_ddr4_a;
			break;
		case SUNXI_DRAM_TYPE_LPDDR4:
			phy_init = phy_init_lpddr4_a;
			break;
		}
		break;
	default:
		switch (para->type)
		{
		case SUNXI_DRAM_TYPE_DDR3:
			phy_init = phy_init_ddr3_b;
			break;
		case SUNXI_DRAM_TYPE_LPDDR3:
			phy_init = phy_init_lpddr3_b;
			break;
		case SUNXI_DRAM_TYPE_DDR4:
			phy_init = phy_init_ddr4_b;
			break;
		case SUNXI_DRAM_TYPE_LPDDR4:
			phy_init = phy_init_lpddr4_b;
			break;
		}
		break;
	}

	ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0xc0);
	for (i = 0; i < 27; i++)
	{
		// PRINTF("val: %u, ptr: %p\n", phy_init[i], ptr);
		writel(phy_init[i], ptr++);
	}
}

static void libdram_mctl_phy_vref_config(struct dram_para *para)
{
	uint32_t val = 0;

	switch (para->type)
	{
	case SUNXI_DRAM_TYPE_DDR3:
		val = para->tpr6 & 0xFF;
		if (!val)
			val = 0x80;
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		val = (para->tpr6 >> 16) & 0xFF;
		if (!val)
			val = 0x80;
		break;
	case SUNXI_DRAM_TYPE_DDR4:
		val = (para->tpr6 >> 8) & 0xFF;
		if (!val)
			val = 0x80;
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		val = (para->tpr6 >> 24) & 0xFF;
		if (!val)
			val = 0x33;
		break;
	}

	writel(val, SUNXI_DRAM_PHY0_BASE + 0x3dc);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x45c);
}

static void libdram_mctl_drive_odt_config(struct dram_para *para)
{
	uint32_t val;

	writel(para->dx_dri & 0x1f, SUNXI_DRAM_PHY0_BASE + 0x388);
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x388);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x38c);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
	{
		if ((para->tpr1 & 0x1f1f1f1f) != 0)
			writel(para->tpr1 & 0x1f, SUNXI_DRAM_PHY0_BASE + 0x38c);
		else
			writel(4, SUNXI_DRAM_PHY0_BASE + 0x38c);
	}

	writel((para->dx_dri >> 8) & 0x1f, SUNXI_DRAM_PHY0_BASE + 0x3c8);
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x3c8);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x3cc);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
	{
		if ((para->tpr1 & 0x1f1f1f1f) != 0)
			writel((para->tpr1 >> 8) & 0x1f, SUNXI_DRAM_PHY0_BASE + 0x3cc);
		else
			writel(4, SUNXI_DRAM_PHY0_BASE + 0x3cc);
	}

	writel((para->dx_dri >> 16) & 0x1f, SUNXI_DRAM_PHY0_BASE + 0x408);
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x408);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x40c);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
	{
		if ((para->tpr1 & 0x1f1f1f1f) != 0)
			writel((para->tpr1 >> 16) & 0x1f, SUNXI_DRAM_PHY0_BASE + 0x40c);
		else
			writel(4, SUNXI_DRAM_PHY0_BASE + 0x40c);
	}

	writel((para->dx_dri >> 24) & 0x1f, SUNXI_DRAM_PHY0_BASE + 0x448);
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x448);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x44c);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
	{
		if ((para->tpr1 & 0x1f1f1f1f) != 0)
			writel((para->tpr1 >> 24) & 0x1f, SUNXI_DRAM_PHY0_BASE + 0x44c);
		else
			writel(4, SUNXI_DRAM_PHY0_BASE + 0x44c);
	}

	writel(para->ca_dri & 0x1f, SUNXI_DRAM_PHY0_BASE + 0x340);
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x340);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x344);

	writel((para->ca_dri >> 8) & 0x1f, SUNXI_DRAM_PHY0_BASE + 0x348);
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x348);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x34c);

	val = para->dx_odt & 0x1f;
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x380);
	if (para->type == SUNXI_DRAM_TYPE_DDR4 || para->type == SUNXI_DRAM_TYPE_LPDDR3)
	{
		writel(0, SUNXI_DRAM_PHY0_BASE + 0x380);
	}
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x384);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
	{
		writel(0, SUNXI_DRAM_PHY0_BASE + 0x384);
	}

	val = (para->dx_odt >> 8) & 0x1f;
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x3c0);
	if (para->type == SUNXI_DRAM_TYPE_DDR4 || para->type == SUNXI_DRAM_TYPE_LPDDR3)
	{
		writel(0, SUNXI_DRAM_PHY0_BASE + 0x3c0);
	}
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x3c4);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
	{
		writel(0, SUNXI_DRAM_PHY0_BASE + 0x3c4);
	}

	val = (para->dx_odt >> 16) & 0x1f;
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x400);
	if (para->type == SUNXI_DRAM_TYPE_DDR4 || para->type == SUNXI_DRAM_TYPE_LPDDR3)
	{
		writel(0, SUNXI_DRAM_PHY0_BASE + 0x400);
	}
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x404);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
	{
		writel(0, SUNXI_DRAM_PHY0_BASE + 0x404);
	}

	val = (para->dx_odt >> 24) & 0x1f;
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x440);
	if (para->type == SUNXI_DRAM_TYPE_DDR4 || para->type == SUNXI_DRAM_TYPE_LPDDR3)
	{
		writel(0, SUNXI_DRAM_PHY0_BASE + 0x440);
	}
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x444);
	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
	{
		writel(0, SUNXI_DRAM_PHY0_BASE + 0x444);
	}
}

static void libdram_mctl_phy_ca_bit_delay_compensation(struct dram_para *para)
{
	uint32_t *ptr, tpr0;
	uint32_t i, a, b, c, d;

	if (para->tpr10 >= 0)
	{ // Sorry for direct copy from decompiler
		tpr0 = ((32 * para->tpr10) & 0x1E00) | ((para->tpr10 << 9) & 0x1E0000) | ((2 * para->tpr10) & 0x1E) | ((para->tpr10 << 13) & 0x1E000000);
		if (para->tpr10 >> 29)
			tpr0 *= 2;
	}
	else
	{
		tpr0 = para->tpr0;
	}

	ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x780);
	for (i = 0; i < 32; i++)
	{
		// PRINTF("val: %u, ptr: %p\n", (tpr0 >> 8) & 0x3F, ptr);
		writel((tpr0 >> 8) & 0x3f, ptr++);
	}

	a = tpr0 & 0x3f;
	b = tpr0 & 0x3f;
	c = (tpr0 >> 16) & 0x3f;
	d = (tpr0 >> 24) & 0x3f;

	switch (readl(SUNXI_SID_BASE))
	{ // Seems like allwinner fab factory change
	case 0x800:
	case 0x2400:
		switch (para->type)
		{
		case SUNXI_DRAM_TYPE_DDR3:
			writel(a, SUNXI_DRAM_PHY0_BASE + 0x7dc);
			writel(b, SUNXI_DRAM_PHY0_BASE + 0x7e0);
			writel(c, SUNXI_DRAM_PHY0_BASE + 0x7e4);
			writel(d, SUNXI_DRAM_PHY0_BASE + 0x2388); // ??? WTF
			break;
		case SUNXI_DRAM_TYPE_LPDDR4:
			writel(a, SUNXI_DRAM_PHY0_BASE + 0x7dc);
			writel(b, SUNXI_DRAM_PHY0_BASE + 0x7e0);
			writel(c, SUNXI_DRAM_PHY0_BASE + 0x7e4);
			writel(d, SUNXI_DRAM_PHY0_BASE + 0x790);
			break;
		default:
			break;
		}
		break;
	default:
		switch (para->type)
		{
		case SUNXI_DRAM_TYPE_DDR3:
			writel(a, SUNXI_DRAM_PHY0_BASE + 0x7dc);
			writel(b, SUNXI_DRAM_PHY0_BASE + 0x7e0);
			writel(c, SUNXI_DRAM_PHY0_BASE + 0x7b8);
			writel(d, SUNXI_DRAM_PHY0_BASE + 0x784);
			break;
		case SUNXI_DRAM_TYPE_LPDDR3:
			writel(a, SUNXI_DRAM_PHY0_BASE + 0x7dc);
			writel(b, SUNXI_DRAM_PHY0_BASE + 0x7e0);
			writel(c, SUNXI_DRAM_PHY0_BASE + 0x788);
			writel(d, SUNXI_DRAM_PHY0_BASE + 0x790);
			break;
		case SUNXI_DRAM_TYPE_DDR4:
			writel(a, SUNXI_DRAM_PHY0_BASE + 0x7dc);
			writel(b, SUNXI_DRAM_PHY0_BASE + 0x7e0);
			writel(c, SUNXI_DRAM_PHY0_BASE + 0x784);
			break;
		case SUNXI_DRAM_TYPE_LPDDR4:
			writel(a, SUNXI_DRAM_PHY0_BASE + 0x7dc);
			writel(b, SUNXI_DRAM_PHY0_BASE + 0x7e0);
			writel(c, SUNXI_DRAM_PHY0_BASE + 0x790);
			writel(d, SUNXI_DRAM_PHY0_BASE + 0x78c);
			break;
		}
		break;
	}
}

static void libdram_phy_para_config(struct dram_para *para)
{
	uint32_t val;

	clrbits_le32(&prcm->sys_pwroff_gating, 0x10);

	if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x4, 0x08);

	if ((para->para2 & 1) != 0)
		val = 3;
	else
		val = 0xf;
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x3c, 0xf, val);

	switch (para->type)
	{
	case SUNXI_DRAM_TYPE_DDR3:
		val = 13;
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		val = 14;
		break;
	case SUNXI_DRAM_TYPE_DDR4:
		val = 13;
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		val = 20;
		if (para->tpr13 & BIT_U32(28))
			val = 22;
		break;
	}

	writel(val, SUNXI_DRAM_PHY0_BASE + 0x14);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x35c);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x368);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x374);

	writel(0, SUNXI_DRAM_PHY0_BASE + 0x18);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x360);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x36c);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x378);

	switch (para->type)
	{
	case SUNXI_DRAM_TYPE_DDR3:
		val = 9;
		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		val = 8;
		break;
	case SUNXI_DRAM_TYPE_DDR4:
		val = 10;
		break;
	case SUNXI_DRAM_TYPE_LPDDR4:
		val = 10;
		break;
	}

	writel(val, SUNXI_DRAM_PHY0_BASE + 0x1c);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x364);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x370);
	writel(val, SUNXI_DRAM_PHY0_BASE + 0x37c);

	libdram_mctl_phy_set_address_remapping(para);
	libdram_mctl_phy_vref_config(para);
	libdram_mctl_drive_odt_config(para);

	if (para->tpr10 >> 16)
		libdram_mctl_phy_ca_bit_delay_compensation(para);

	switch (para->type)
	{
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
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 4, 0x7, val | 8);

	if (para->clk <= 672)
		writel(0xf, SUNXI_DRAM_PHY0_BASE + 0x20);
	if (para->clk > 500)
	{
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x144, 0x80);
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x14c, 0xe0);
	}
	else
	{
		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x144, 0x80);
		clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x14c, 0xe0, 0x20);
	}

	clrbits_le32(&mctl_com->unk_0x008, 0x200);
	udelay(1);
	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x14c, 8);
	TP();
	dbp();
	libdram_mctl_await_completion((uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x180), 4, 4);
	TP();

	if ((para->tpr13 & 0x10) == 0)
		udelay(1000);

	writel(0x37, SUNXI_DRAM_PHY0_BASE + 0x58);
	setbits_le32(&prcm->sys_pwroff_gating, 0x10);
}

static void libdram_mctl_dfi_init(struct dram_para *para)
{
	setbits_le32(&mctl_com->maer0, 0x100);

	/*
	LDR             R5, =0x4820320
	ORR.W           R3, R3, #0x100
	STR             R3, [R2]
	MOVS            R3, #0
	STR             R3, [R5]
	*/
	writel(0, &mctl_ctl->swctl);

	setbits_le32(&mctl_ctl->dfimisc, 1);
	setbits_le32(&mctl_ctl->dfimisc, 0x20);
	writel(1, &mctl_ctl->swctl);
	libdram_mctl_await_completion(&mctl_ctl->swstat, 1, 1);

	clrbits_le32(&mctl_ctl->dfimisc, 0x20);
	writel(1, &mctl_ctl->swctl);
	libdram_mctl_await_completion(&mctl_ctl->swstat, 1, 1);
	TP();
	libdram_mctl_await_completion(&mctl_ctl->dfistat, 1, 1);
	TP();

	clrbits_le32(&mctl_ctl->pwrctl, 0x20);
	writel(1, &mctl_ctl->swctl);
	libdram_mctl_await_completion(&mctl_ctl->swstat, 1, 1);
	libdram_mctl_await_completion(&mctl_ctl->statr, 3, 1);

	if ((para->tpr13 & 0x10) == 0)
	{
		udelay(200);
	}

	clrbits_le32(&mctl_ctl->dfimisc, 1);

	writel(1, &mctl_ctl->swctl);
	libdram_mctl_await_completion(&mctl_ctl->swstat, 1, 1);

	switch (para->type)
	{
	case SUNXI_DRAM_TYPE_DDR3:
		writel(para->mr0, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr1, &mctl_ctl->mrctrl1);
		writel(0x80001030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr2, &mctl_ctl->mrctrl1);
		writel(0x80002030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr3, &mctl_ctl->mrctrl1);
		writel(0x80003030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);
		break;

	case SUNXI_DRAM_TYPE_LPDDR3:
		writel(para->mr1 | 0x100, &mctl_ctl->mrctrl1);
		writel(0x800000f0, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr2 | 0x200, &mctl_ctl->mrctrl1);
		writel(0x800000f0, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr3 | 0x300, &mctl_ctl->mrctrl1);
		writel(0x800000f0, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr11 | 0xb00, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);
		break;

	case SUNXI_DRAM_TYPE_DDR4:
		writel(para->mr0, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr1, &mctl_ctl->mrctrl1);
		writel(0x80001030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr2, &mctl_ctl->mrctrl1);
		writel(0x80002030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr3, &mctl_ctl->mrctrl1);
		writel(0x80003030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr4, &mctl_ctl->mrctrl1);
		writel(0x80004030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr5, &mctl_ctl->mrctrl1);
		writel(0x80005030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr6 | 0x80, &mctl_ctl->mrctrl1);
		writel(0x80006030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr6 | 0x80, &mctl_ctl->mrctrl1);
		writel(0x80006030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr6 | 0x80, &mctl_ctl->mrctrl1);
		writel(0x80006030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);
		break;

	case SUNXI_DRAM_TYPE_LPDDR4:
		writel(para->mr0, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr1 | 0x100, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr2 | 0x200, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr3 | 0x300, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr4 | 0x400, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr11 | 0xb00, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr12 | 0xc00, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr13 | 0xd00, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr14 | 0xe00, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

		writel(para->mr22 | 0x1600, &mctl_ctl->mrctrl1);
		writel(0x80000030, &mctl_ctl->mrctrl0);
		libdram_mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);
		break;
	}

	writel(0, SUNXI_DRAM_PHY0_BASE + 0x54);
}

static int libdram_phy_write_leveling(struct dram_para *para)
{
	PRINTF("!!!WARNING!!! libdram_phy_write_leveling: unimplemented\n");
	return 1;
}

static int libdram_phy_read_calibration(struct dram_para *para)
{
	uint32_t val;

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30, 0x20);

	if (para->type == SUNXI_DRAM_TYPE_DDR4)
	{
		setbits_le32(SUNXI_DRAM_PHY0_BASE + 54, 0x2);
	}

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);

	if (para->para2 & 1)
		val = 3;
	else
		val = 0xf;

	while ((readl(SUNXI_DRAM_PHY0_BASE + 0x184) & val) != val)
	{
		if (readl(SUNXI_DRAM_PHY0_BASE + 0x184) & 0x20)
		{
			return 0;
		}
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30);

	if ((para->para2 & 0x1000) != 0)
	{
		clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30, 0x10);

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);

		while ((readl(SUNXI_DRAM_PHY0_BASE + 0x184) & val) != val)
		{
			if (readl(SUNXI_DRAM_PHY0_BASE + 0x184) & 0x20)
			{
				return 0;
			}
		}

		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30);

	return 1;
}

static int libdram_phy_read_training(struct dram_para *para)
{
	PRINTF("!!!WARNING!!! libdram_phy_read_training: unimplemented\n");
	return 1;
}

static int libdram_phy_write_training(struct dram_para *para)
{
	PRINTF("!!!WARNING!!! libdram_phy_write_training: unimplemented\n");
	return 1;
}

static int libdram_mctl_phy_dfs(struct dram_para *para, int clk)
{
	PRINTF("!!!WARNING!!! libdram_mctl_phy_dfs: unimplemented\n");
	return 1;
}

static void libdram_mctl_phy_dx_bit_delay_compensation(struct dram_para *para)
{
	int i;
	uint32_t val, *ptr;

	if (para->tpr10 & 0x40000)
	{
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, 1);
		setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 8);
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x10);

		if (para->type == SUNXI_DRAM_TYPE_LPDDR4)
			clrbits_le32(SUNXI_DRAM_PHY0_BASE + 4, 0x80);

		val = para->tpr11 & 0x3F;
		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x484);
		for (i = 0; i < 9; i++)
		{
			writel(val, ptr);
			writel(val, ptr + 0x30);
			ptr += 2;
		}
		val = para->para0 & 0x3F;
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x4d0);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x590);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x4cc);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x58c);

		val = (para->tpr11 >> 8) & 0x3F;
		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x4d8);
		for (i = 0; i < 9; i++)
		{
			writel(val, ptr);
			writel(val, ptr + 0x30);
			ptr += 2;
		}
		val = (para->para0 >> 8) & 0x3F;
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x524);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x5e4);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x520);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x5e0);

		val = (para->tpr11 >> 16) & 0x3F;
		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x604);
		for (i = 0; i < 9; i++)
		{
			writel(val, ptr);
			writel(val, ptr + 0x30);
			ptr += 2;
		}
		val = (para->para0 >> 16) & 0x3F;
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x650);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x710);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x64c);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x70c);

		val = (para->tpr11 >> 24) & 0x3F;
		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x658);
		for (i = 0; i < 9; i++)
		{
			writel(val, ptr);
			writel(val, ptr + 0x30);
			ptr += 2;
		}
		val = (para->para0 >> 24) & 0x3F;
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x6a4);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x764);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x6a0);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x760);
		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, 1);
	}

	if (para->tpr10 & 0x20000)
	{
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x54, 0x80);
		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 4);

		val = para->tpr12 & 0x3F;
		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x480);
		for (i = 0; i < 9; i++)
		{
			writel(val, ptr);
			writel(val, ptr + 0x30);
			ptr += 2;
		}
		val = para->tpr14 & 0x3F;
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x528);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x5e8);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x4c8);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x588);

		val = (para->tpr12 >> 8) & 0x3F;
		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x4d4);
		for (i = 0; i < 9; i++)
		{
			writel(val, ptr);
			writel(val, ptr + 0x30);
			ptr += 2;
		}
		val = (para->tpr14 >> 8) & 0x3F;
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x52c);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x5ec);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x51c);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x5dc);

		val = (para->tpr12 >> 16) & 0x3F;
		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x600);
		for (i = 0; i < 9; i++)
		{
			writel(val, ptr);
			writel(val, ptr + 0x30);
			ptr += 2;
		}
		val = (para->tpr14 >> 16) & 0x3F;
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x6a8);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x768);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x648);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x708);

		val = (para->tpr12 >> 24) & 0x3F;
		ptr = (uint32_t *)(SUNXI_DRAM_PHY0_BASE + 0x654);
		for (i = 0; i < 9; i++)
		{
			writel(val, ptr);
			writel(val, ptr + 0x30);
			ptr += 2;
		}
		val = (para->tpr14 >> 24) & 0x3F;
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x6ac);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x76c);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x69c);
		writel(val, SUNXI_DRAM_PHY0_BASE + 0x75c);
	}

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x54, 0x80);
}

static int libdram_ddrphy_phyinit_C_initPhyConfig(struct dram_para *para)
{
	TP();
	int i, max_retry, ret;

	libdram_phy_para_config(para);
	TP();
	libdram_mctl_dfi_init(para);
	TP();
	writel(0, &mctl_ctl->swctl);
	libdram_mctl_com_set_controller_refresh(0);
	TP();
	writel(1, &mctl_ctl->swctl);

	if (para->tpr10 & 0x80000)
		max_retry = 5;
	else
		max_retry = 1;

	if (para->tpr10 & 0x100000)
	{
		for (i = 0; i < max_retry; i++)
			if (libdram_phy_write_leveling(para))
				break;

		if (i == max_retry)
		{
			PRINTF("phy_write_leveling failed!\n");
			return 0;
		}
	}
	TP();

	if (para->tpr10 & 0x200000)
	{
		for (i = 0; i < max_retry; i++)
			if (libdram_phy_read_calibration(para))
				break;

		if (i == max_retry)
		{
			PRINTF("phy_read_calibration failed!\n");
			return 0;
		}
	}
	TP();

	if (para->tpr10 & 0x400000)
	{
		for (i = 0; i < max_retry; i++)
			if (libdram_phy_read_training(para))
				break;

		if (i == max_retry)
		{
			PRINTF("phy_read_training failed!\n");
			return 0;
		}
	}
	TP();

	if (para->tpr10 & 0x800000)
	{
		for (i = 0; i < max_retry; i++)
			if (libdram_phy_write_training(para))
				break;

		if (i == max_retry)
		{
			PRINTF("phy_write_training failed!\n");
			return 0;
		}
	}
	TP();

	libdram_mctl_phy_dx_bit_delay_compensation(para);
	TP();

	ret = 1;
	if ((para->tpr13 & 0x805) == 5)
	{
		ret &= libdram_mctl_phy_dfs(para, 1);
		ret &= libdram_mctl_phy_dfs(para, 2);
		ret &= libdram_mctl_phy_dfs(para, 3);
		ret &= libdram_mctl_phy_dfs(para, 0);
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, 1);
	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x54, 7);

	return ret;
}

static int libdram_mctl_phy_init(struct dram_para *para)
{
	TP();
	libdram_mctl_phy_cold_reset();
	TP();
	return libdram_ddrphy_phyinit_C_initPhyConfig(para);
}

static int libdram_mctl_channel_init(struct dram_para *para)
{
	int ret;

	clrsetbits_le32(&mctl_com->unk_0x008, 0x3000200, 0x2000200); // CLEAR AND SET SAME BIT_U32 ???
	setbits_le32(&mctl_com->maer0, 0x8000);

	TP();
	dbp();
	libdram_mctl_com_set_bus_config(para);
	TP();
	dbp();

	writel(0, &mctl_ctl->hwlpctl);

	libdram_mctl_com_init(para);
	TP();
	dbp();
	ret = libdram_mctl_phy_init(para);
	TP();
	dbp();
	libdram_mctl_com_set_controller_after_phy();
	TP();
	dbp();

	return ret;
}

static int libdram_mctl_core_init(struct dram_para *para)
{
	TP();
	dbp();
	libdram_mctl_sys_init(para);	// FIXME: после этого дампы нулевые...
	TP();
	dbp();
	return libdram_mctl_channel_init(para);
}

static int libdram_auto_scan_dram_rank_width(struct dram_para *para)
{
	PRINTF("!!!WARNING!!! libdram_auto_scan_dram_rank_width: unimplemented\n");
	// return auto_scan_dram_rank_width(para);
	return 1;
}

static int libdram_auto_scan_dram_size(struct dram_para *para)
{
	PRINTF("!!!WARNING!!! libdram_auto_scan_dram_size: unimplemented\n");
	// return auto_scan_dram_size(para);
	return 1;
}

static int libdram_auto_scan_dram_config(struct dram_para *para)
{
	uint32_t clk;
	uint32_t para0;
	uint32_t tpr11, tpr12, tpr14;
	uint32_t dram_size;

	TP();
	dbp();
	clk = para->clk;

	if ((para->tpr13 & 0x1000) && (para->clk > 360))
		para->clk = 360;

	para0 = para->tpr13 & 0x2000000;

	if (para->tpr13 & 0x2000000)
	{
		para0 = para->para0;
		para->para0 = 0x14151A1C;
		tpr12 = para->tpr12;
		tpr11 = para->tpr11;
		tpr14 = para->tpr14;
		para->tpr11 = 0xE131619;
		para->tpr12 = 0x18171817;
		para->tpr14 = 0x2A28282B;
	}
	else
	{
		tpr14 = 0;
		tpr12 = 0;
		tpr11 = 0;
	}

	if (!(para->tpr13 & 0x4000))
	{
		if (!libdram_auto_scan_dram_rank_width(para) || !libdram_auto_scan_dram_size(para))
		{
			return 0;
		}
	}
	if (!(para->tpr13 & 0x8000))
		para->tpr13 |= 0x6001;

	TP();
	dbp();
	if (para->tpr13 & 0x80000)
	{
		uint32_t *ptr;

		TP();
		if (!libdram_mctl_core_init(para))
			return 0;

		dram_size = libdram_DRAMC_get_dram_size(para);
		para->tpr13 &= ~0x80000u;

		switch (dram_size)
		{
		case 4096:
			ptr = (uint32_t *)(CONFIG_SYS_SDRAM_BASE + 0x60000000u);
			writel(0xa0a0a0a0, ptr);
			if (readl(ptr) != 0xa0a0a0a0)
			{
				para->tpr13 |= 0x10000;
				PRINTF("[AUTO DEBUG]3GB autoscan enable,dram_tpr13 = %x\n", (unsigned) para->tpr13);
			}
			break;
		case 2048:
			ptr = (uint32_t *)(CONFIG_SYS_SDRAM_BASE + 0x30000000u);
			writel(0x70707070, ptr);
			if (readl(ptr) == 0x70707070)
			{
				ptr = (uint32_t *)(CONFIG_SYS_SDRAM_BASE + 0x60000000u);
				writel(0xa0a0a0a0, ptr);
				udelay(1);
				if (readl(ptr) != 0xa0a0a0a0)
					para->tpr13 |= 0x50000;
			}
			else
			{
				para->tpr13 |= 0x20000;
			}
			PRINTF("[AUTO DEBUG]1.5GB autoscan enable,dram_tpr13 = %x\n", (unsigned) para->tpr13);
			break;
		default:
			break;
		}
	}
	TP();
	dbp();

	if (para->tpr13 & 0x2000000)
	{
		if (para->para2 & 0x1000)
		{
			para->para0 = para0;
			para->tpr11 = tpr11;
			para->tpr12 = tpr12;
			para->tpr14 = tpr14;
		}
		else
		{
			para->para0 = para->mr17;
			para->tpr11 = para->tpr1;
			para->tpr12 = para->tpr2;
			para->tpr14 = para->mr22;
		}
	}

	para->clk = clk;

	return 1;
}

static int libdram_dram_software_training(struct dram_para *para)
{
	PRINTF("!!!WARNING!!! libdram_dram_software_training: unimplemented\n");
	// return dram_software_training(para);
	return 1;
}

static uint32_t libdram_init_DRAM(struct dram_para *para)
{
	int tmp_tpr11, tmp_tpr12;
	uint32_t dram_size, actual_dram_size;

	tmp_tpr11 = para->tpr13 & 0x800000;
	tmp_tpr12 = 0;
	if (tmp_tpr11)
	{
		tmp_tpr11 = para->tpr11;
		tmp_tpr12 = para->tpr12;
	}
	PRINTF("DRAM BOOT DRIVE INFO: %s\n", "V0.696");

	TP();
	dbp();
	(*((volatile uint32_t *)0x3000160)) |= 0x100;
	(*((volatile uint32_t *)0x3000168)) &= 0xffffffc0;
	TP();
	dbp();

	if ((para->tpr13 & 1) == 0 && !libdram_auto_scan_dram_config(para))
	{
		PRINTF("auto_scan_dram_config: failed\n");
		return 0;
	}
	TP();
	dbp();
	if ((para->tpr13 & 0x800) != 0 && !libdram_dram_software_training(para))
	{
		PRINTF("dram_software_training: failed\n");
		return 0;
	}

	PRINTF("DRAM CLK = %d MHZ\n", (int) para->clk);
	PRINTF("DRAM Type = %d (3:DDR3,4:DDR4,7:LPDDR3,8:LPDDR4)\n", para->type);
	TP();
	dbp();

	if (!libdram_mctl_core_init(para))
	{
		PRINTF("DRAM initial error : 0 !\n");
		return 0;
	}
	TP();
	dbp();

	dram_size = libdram_DRAMC_get_dram_size(para);
	actual_dram_size = (para->para2 >> 16) & 0x3FFF;
	TP();
	dbp();

	switch (para->para2 >> 30)
	{
	case 3:
		if (actual_dram_size != dram_size)
		{
			PRINTF("DRAM SIZE error! auto_scan_dram_size = %d, actual_dram_size = %d\n", (int) dram_size, (int) actual_dram_size);
			return 0;
		}
		break;
	case 2:
		dram_size = actual_dram_size;
		break;
	default:
		para->para2 &= 0xFFFF;
		para->para2 |= dram_size << 16;
		break;
	}
	PRINTF("DRAM SIZE = %d MBytes, para1 = %x, para2 = %x, tpr13 = %x\n", (int) dram_size, (unsigned) para->para1, (unsigned) para->para2, (unsigned) para->tpr13);

	if ((para->tpr13 & 0x1000000) != 0)
		mctl_ctl->pwrctl |= 9;

	if ((para->tpr13 & 0x800000) != 0)
	{
		para->tpr11 = tmp_tpr11;
		para->tpr12 = tmp_tpr12;
	}
	TP();
	dbp();

	if (libdram_dramc_simple_wr_test(dram_size, 4096))
	{
		if ((para->tpr13 & 0x40) != 0)
			return 0;
		if (!libdram_mctl_core_init(para))
		{
			PRINTF("DRAM initial error : 1 !\n");
			return 0;
		}
		if (libdram_dramc_simple_wr_test(dram_size, 4096))
			return 0;
	}
	TP();
	dbp();

	return dram_size;
};

//unsigned long sunxi_dram_init(void)
//{
//	return libdram_init_DRAM(&para) * 1024 * 1024;
//};
static struct dram_para lpddr4 =
{
		.clk       = 792,
		.type      = SUNXI_DRAM_TYPE_LPDDR4,
		.dx_odt    = 0x07070707,
		.dx_dri    = 0x0d0d0d0d,
		.ca_dri    = 0x0e0e,
		.para0     = 0x0d0a050c,
		.para1     = 0x30ea,
		.para2     = 0x1000,
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
		.mr14      = 0x7,
		.mr16      = 0x0,
		.mr17      = 0x0,
		.mr22      = 0x26,
		.tpr0      = 0x06060606,
		.tpr1      = 0x04040404,
		.tpr2      = 0x0,
		.tpr3      = 0x0,
		.tpr6      = 0x48000000,
		.tpr10     = 0x00273333,
		.tpr11     = 0x241f1923,
		.tpr12     = 0x14151313,
		.tpr13     = 0x81d20,
		.tpr14     = 0x2023211f,
};

void xmctl_set_timing_params(struct xdram_para *para)
{
	libdram_mctl_com_set_channel_timing(& lpddr4);
}

void FLASHMEMINITFUNC arm_hardware_sdram_initialize(void)
{
	long int memsize;
	PRINTF("arm_hardware_sdram_initialize start, cpux=%u MHz\n", (unsigned) (allwnr_t507_get_cpux_freq() / 1000 / 1000));

	//arm_hardware_sdram_initialize0();

	TP();
	dbp();

	memsize =   libdram_init_DRAM(& lpddr4) * 1024 * 1024;
	//memsize =  init_DRAM(0, (void *) & lpddr4);

	PRINTF("arm_hardware_sdram_initialize: v=%lu, %lu MB\n", memsize, memsize / 1024 / 1024);

	memset((void *) CONFIG_SYS_SDRAM_BASE, 0, 128u << 20);
//	memset((void *) CONFIG_SYS_SDRAM_BASE + 0x00, 0xE5, 0x80);
//	memset((void *) CONFIG_SYS_SDRAM_BASE + 0x80, 0xDF, 0x80);
	printhex(CONFIG_SYS_SDRAM_BASE, (void *) CONFIG_SYS_SDRAM_BASE, 2 * 0x80);

	PRINTF("arm_hardware_sdram_initialize done, ddr=%u MHz\n", (unsigned) (allwnr_t507_get_dram_freq() / 1000 / 1000));
}
#endif

#endif /* WITHSDRAMHW && CPUSTYLE_T507 && ! CPUSTYLE_H616 */
