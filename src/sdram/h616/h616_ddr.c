/*
 * a64_ddr3.c
 *
 *  Created on: 15 февр. 2023 г.
 *      Author: User
 */

// https://github.com/apritzel/u-boot/blob/3aaabfe9ff4bbcd11096513b1b28d1fb0a40800f/arch/arm/mach-sunxi/dram_sun50i_h616.c#L3

#include "hardware.h"

#if CPUSTYLE_T507

#include "formats.h"

#define CONFIG_SYS_SDRAM_BASE 0x40000000

#define CONFIG_DRAM_CLK 533

#define CONFIG_MACH_SUN50I_H616 1

#ifdef CONFIG_MACH_SUN50I_H6
	#define SUNXI_DRAM_COM_BASE		0x04002000
	#define SUNXI_DRAM_CTL0_BASE		0x04003000
	#define SUNXI_DRAM_PHY0_BASE		0x04005000
#endif
#define SUNXI_NFC_BASE			0x04011000
#define SUNXI_MMC0_BASE			0x04020000
#define SUNXI_MMC1_BASE			0x04021000
#define SUNXI_MMC2_BASE			0x04022000
#ifdef CONFIG_MACH_SUN50I_H616
	#define SUNXI_DRAM_COM_BASE		0x047FA000
	#define SUNXI_DRAM_CTL0_BASE		0x047FB000
	#define SUNXI_DRAM_PHY0_BASE		0x04800000
#endif

#define SUNXI_CCM_BASE CCU_BASE

#define IS_ENABLED(v) (0)

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
#define MSTR_DEVICETYPE_MASK	GENMASK(5, 0)
#define MSTR_2TMODE		BIT_U32(10)
#define MSTR_BUSWIDTH_FULL	(0 << 12)
#define MSTR_BUSWIDTH_HALF	(1 << 12)
#define MSTR_ACTIVE_RANKS(x)	(((x == 2) ? 3 : 1) << 24)
#define MSTR_BURST_LENGTH(x)	(((x) >> 1) << 16)

struct dram_para {
	uint32_t clk;
	enum sunxi_dram_type type;
	uint8_t cols;
	uint8_t rows;
	uint8_t ranks;
	uint8_t bus_full_width;
};


static inline int ns_to_t(int nanoseconds)
{
	const unsigned int ctrl_freq = CONFIG_DRAM_CLK / 2;

	return DIV_ROUND_UP(ctrl_freq * nanoseconds, 1000);
}

void mctl_set_timing_params(struct dram_para *para);


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
//			panic("Timeout initialising DRAM\n");
	}
}

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

	PRINTF("MBUS port %d cfg0 %08x cfg1 %08x\n", port, (unsigned) cfg0, (unsigned) cfg1);
	writel(cfg0, &mctl_com->master[port].cfg0);
	writel(cfg1, &mctl_com->master[port].cfg1);
}

#define MBUS_CONF(port, bwlimit, qos, acs, bwl0, bwl1, bwl2)	\
	mbus_configure_port(port, bwlimit, 0 /*fasle*/, \
			    MBUS_QOS_ ## qos, 0, acs, bwl0, bwl1, bwl2)

static void mctl_set_master_priority(void)
{
	struct sunxi_mctl_com_reg * const mctl_com =
			(struct sunxi_mctl_com_reg *)SUNXI_DRAM_COM_BASE;

	/* enable bandwidth limit windows and set windows size 1us */
	writel(399, &mctl_com->tmr);
	writel(BIT_U32(16), &mctl_com->bwcr);

	MBUS_CONF( 0, 1 /*true*/, HIGHEST, 0,  256,  128,  100);
	MBUS_CONF( 1, 1 /*true*/,    HIGH, 0, 1536, 1400,  256);
	MBUS_CONF( 2, 1 /*true*/, HIGHEST, 0,  512,  256,   96);
	MBUS_CONF( 3, 1 /*true*/,    HIGH, 0,  256,  100,   80);
	MBUS_CONF( 4, 1 /*true*/,    HIGH, 2, 8192, 5500, 5000);
	MBUS_CONF( 5, 1 /*true*/,    HIGH, 2,  100,   64,   32);
	MBUS_CONF( 6, 1 /*true*/,    HIGH, 2,  100,   64,   32);
	MBUS_CONF( 8, 1 /*true*/,    HIGH, 0,  256,  128,   64);
	MBUS_CONF(11, 1 /*true*/,    HIGH, 0,  256,  128,  100);
	MBUS_CONF(14, 1 /*true*/,    HIGH, 0, 1024,  256,   64);
	MBUS_CONF(16, 1 /*true*/, HIGHEST, 6, 8192, 2800, 2400);
	MBUS_CONF(21, 1 /*true*/, HIGHEST, 6, 2048,  768,  512);
	MBUS_CONF(25, 1 /*true*/, HIGHEST, 0,  100,   64,   32);
	MBUS_CONF(26, 1 /*true*/,    HIGH, 2, 8192, 5500, 5000);
	MBUS_CONF(37, 1 /*true*/,    HIGH, 0,  256,  128,   64);
	MBUS_CONF(38, 1 /*true*/,    HIGH, 2,  100,   64,   32);
	MBUS_CONF(39, 1 /*true*/,    HIGH, 2, 8192, 5500, 5000);
	MBUS_CONF(40, 1 /*true*/,    HIGH, 2,  100,   64,   32);

	__DMB();
}

static void mctl_sys_init(struct dram_para *para)
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
	       CCM_PLL5_CTRL_N(para->clk * 2 / 24), &ccm->pll5_cfg);
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

static void mctl_set_addrmap(struct dram_para *para)
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
	0x07, 0x0b, 0x02, 0x16, 0x0d, 0x0e, 0x14, 0x19,
	0x0a, 0x15, 0x03, 0x13, 0x04, 0x0c, 0x10, 0x06,
	0x0f, 0x11, 0x1a, 0x01, 0x12, 0x17, 0x00, 0x08,
	0x09, 0x05, 0x18
};

static void mctl_phy_configure_odt(void)
{
	writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x388);
	writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x38c);

	writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x3c8);
	writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x3cc);

	writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x408);
	writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x40c);

	writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x448);
	writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x44c);

	writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x340);
	writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x344);

	writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x348);
	writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x34c);

	writel(0x8, SUNXI_DRAM_PHY0_BASE + 0x380);
	writel(0x8, SUNXI_DRAM_PHY0_BASE + 0x384);

	writel(0x8, SUNXI_DRAM_PHY0_BASE + 0x3c0);
	writel(0x8, SUNXI_DRAM_PHY0_BASE + 0x3c4);

	writel(0x8, SUNXI_DRAM_PHY0_BASE + 0x400);
	writel(0x8, SUNXI_DRAM_PHY0_BASE + 0x404);

	writel(0x8, SUNXI_DRAM_PHY0_BASE + 0x440);
	writel(0x8, SUNXI_DRAM_PHY0_BASE + 0x444);

	__DMB();
}

static int mctl_phy_write_leveling(struct dram_para *para)
{
	int result = 1 /*true*/;
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
		result = 0 /*fasle*/;
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x25c);
	if (val == 0 || val == 0x3f)
		result = 0 /*fasle*/;
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x318);
	if (val == 0 || val == 0x3f)
		result = 0 /*fasle*/;
	val = readl(SUNXI_DRAM_PHY0_BASE + 0x31c);
	if (val == 0 || val == 0x3f)
		result = 0 /*fasle*/;

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

static int mctl_phy_read_calibration(struct dram_para *para)
{
	int result = 1 /*true*/;
	uint32_t val, tmp;

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 0x30, 0x20);

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 1);

	if (para->bus_full_width)
		val = 0xf;
	else
		val = 3;

	while ((readl(SUNXI_DRAM_PHY0_BASE + 0x184) & val) != val) {
		if (readl(SUNXI_DRAM_PHY0_BASE + 0x184) & 0x20) {
			result = 0 /*fasle*/;
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
				result = 0 /*fasle*/;
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

static int mctl_phy_read_training(struct dram_para *para)
{
	uint32_t val1, val2, *ptr1, *ptr2;
	int result = 1 /*true*/;
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
		result = 0 /*fasle*/;

	if (para->bus_full_width) {
		mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xa40), 0xc, 0xc);
		if (readl(SUNXI_DRAM_PHY0_BASE + 0xa40) & 3)
			result = 0 /*fasle*/;
	}

	ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x898);
	ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x850);
	for (i = 0; i < 9; i++) {
		val1 = readl(&ptr1[i]);
		val2 = readl(&ptr2[i]);
		if (val1 - val2 <= 6)
			result = 0 /*fasle*/;
	}
	ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x8bc);
	ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x874);
	for (i = 0; i < 9; i++) {
		val1 = readl(&ptr1[i]);
		val2 = readl(&ptr2[i]);
		if (val1 - val2 <= 6)
			result = 0 /*fasle*/;
	}

	if (para->bus_full_width) {
		ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xa98);
		ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xa50);
		for (i = 0; i < 9; i++) {
			val1 = readl(&ptr1[i]);
			val2 = readl(&ptr2[i]);
			if (val1 - val2 <= 6)
				result = 0 /*fasle*/;
		}

		ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xabc);
		ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xa74);
		for (i = 0; i < 9; i++) {
			val1 = readl(&ptr1[i]);
			val2 = readl(&ptr2[i]);
			if (val1 - val2 <= 6)
				result = 0 /*fasle*/;
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
			result = 0 /*fasle*/;

		if (para->bus_full_width) {
			mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xa40), 0xc, 0xc);
			if (readl(SUNXI_DRAM_PHY0_BASE + 0xa40) & 3)
				result = 0 /*fasle*/;
		}

		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 3);
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 3);

	return result;
}

static int mctl_phy_write_training(struct dram_para *para)
{
	uint32_t val1, val2, *ptr1, *ptr2;
	int result = 1 /*true*/;
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
		result = 0 /*fasle*/;

	if (para->bus_full_width) {
		mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xae0), 3, 3);
		if (readl(SUNXI_DRAM_PHY0_BASE + 0xae0) & 0xc)
			result = 0 /*fasle*/;
	}

	ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x938);
	ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x8f0);
	for (i = 0; i < 9; i++) {
		val1 = readl(&ptr1[i]);
		val2 = readl(&ptr2[i]);
		if (val1 - val2 <= 6)
			result = 0 /*fasle*/;
	}
	ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x95c);
	ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x914);
	for (i = 0; i < 9; i++) {
		val1 = readl(&ptr1[i]);
		val2 = readl(&ptr2[i]);
		if (val1 - val2 <= 6)
			result = 0 /*fasle*/;
	}

	if (para->bus_full_width) {
		ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xb38);
		ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xaf0);
		for (i = 0; i < 9; i++) {
			val1 = readl(&ptr1[i]);
			val2 = readl(&ptr2[i]);
			if (val1 - val2 <= 6)
				result = 0 /*fasle*/;
		}
		ptr1 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xb5c);
		ptr2 = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xb14);
		for (i = 0; i < 9; i++) {
			val1 = readl(&ptr1[i]);
			val2 = readl(&ptr2[i]);
			if (val1 - val2 <= 6)
				result = 0 /*fasle*/;
		}
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x60);

	if (para->ranks == 2) {
		clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 0xc, 4);

		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x10);
		setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x20);

		mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x8e0), 3, 3);
		if (readl(SUNXI_DRAM_PHY0_BASE + 0x8e0) & 0xc)
			result = 0 /*fasle*/;

		if (para->bus_full_width) {
			mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xae0), 3, 3);
			if (readl(SUNXI_DRAM_PHY0_BASE + 0xae0) & 0xc)
				result = 0 /*fasle*/;
		}

		clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x60);
	}

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x198, 0xc);

	return result;
}

static int mctl_phy_bit_delay_compensation(struct dram_para *para)
{
	uint32_t *ptr;
	int i;

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, 1);
	setbits_le32(SUNXI_DRAM_PHY0_BASE + 8, 8);
	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 0x10);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x484);
	for (i = 0; i < 9; i++) {
		writel(0x16, ptr);
		writel(0x16, ptr + 0x30);
		ptr += 2;
	}
	writel(0x1c, SUNXI_DRAM_PHY0_BASE + 0x4d0);
	writel(0x1c, SUNXI_DRAM_PHY0_BASE + 0x590);
	writel(0x1c, SUNXI_DRAM_PHY0_BASE + 0x4cc);
	writel(0x1c, SUNXI_DRAM_PHY0_BASE + 0x58c);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x4d8);
	for (i = 0; i < 9; i++) {
		writel(0x1a, ptr);
		writel(0x1a, ptr + 0x30);
		ptr += 2;
	}
	writel(0x1e, SUNXI_DRAM_PHY0_BASE + 0x524);
	writel(0x1e, SUNXI_DRAM_PHY0_BASE + 0x5e4);
	writel(0x1e, SUNXI_DRAM_PHY0_BASE + 0x520);
	writel(0x1e, SUNXI_DRAM_PHY0_BASE + 0x5e0);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x604);
	for (i = 0; i < 9; i++) {
		writel(0x1a, ptr);
		writel(0x1a, ptr + 0x30);
		ptr += 2;
	}
	writel(0x1e, SUNXI_DRAM_PHY0_BASE + 0x650);
	writel(0x1e, SUNXI_DRAM_PHY0_BASE + 0x710);
	writel(0x1e, SUNXI_DRAM_PHY0_BASE + 0x64c);
	writel(0x1e, SUNXI_DRAM_PHY0_BASE + 0x70c);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x658);
	for (i = 0; i < 9; i++) {
		writel(0x1a, ptr);
		writel(0x1a, ptr + 0x30);
		ptr += 2;
	}
	writel(0x1e, SUNXI_DRAM_PHY0_BASE + 0x6a4);
	writel(0x1e, SUNXI_DRAM_PHY0_BASE + 0x764);
	writel(0x1e, SUNXI_DRAM_PHY0_BASE + 0x6a0);
	writel(0x1e, SUNXI_DRAM_PHY0_BASE + 0x760);

	__DMB();

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, 1);

	/* second part */
	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x54, 0x80);
	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x190, 4);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x480);
	for (i = 0; i < 9; i++) {
		writel(0x10, ptr);
		writel(0x10, ptr + 0x30);
		ptr += 2;
	}
	writel(0x18, SUNXI_DRAM_PHY0_BASE + 0x528);
	writel(0x18, SUNXI_DRAM_PHY0_BASE + 0x5e8);
	writel(0x18, SUNXI_DRAM_PHY0_BASE + 0x4c8);
	writel(0x18, SUNXI_DRAM_PHY0_BASE + 0x588);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x4d4);
	for (i = 0; i < 9; i++) {
		writel(0x12, ptr);
		writel(0x12, ptr + 0x30);
		ptr += 2;
	}
	writel(0x1a, SUNXI_DRAM_PHY0_BASE + 0x52c);
	writel(0x1a, SUNXI_DRAM_PHY0_BASE + 0x5ec);
	writel(0x1a, SUNXI_DRAM_PHY0_BASE + 0x51c);
	writel(0x1a, SUNXI_DRAM_PHY0_BASE + 0x5dc);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x600);
	for (i = 0; i < 9; i++) {
		writel(0x12, ptr);
		writel(0x12, ptr + 0x30);
		ptr += 2;
	}
	writel(0x1a, SUNXI_DRAM_PHY0_BASE + 0x6a8);
	writel(0x1a, SUNXI_DRAM_PHY0_BASE + 0x768);
	writel(0x1a, SUNXI_DRAM_PHY0_BASE + 0x648);
	writel(0x1a, SUNXI_DRAM_PHY0_BASE + 0x708);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x654);
	for (i = 0; i < 9; i++) {
		writel(0x14, ptr);
		writel(0x14, ptr + 0x30);
		ptr += 2;
	}
	writel(0x1c, SUNXI_DRAM_PHY0_BASE + 0x6ac);
	writel(0x1c, SUNXI_DRAM_PHY0_BASE + 0x76c);
	writel(0x1c, SUNXI_DRAM_PHY0_BASE + 0x69c);
	writel(0x1c, SUNXI_DRAM_PHY0_BASE + 0x75c);

	__DMB();

	setbits_le32(SUNXI_DRAM_PHY0_BASE + 0x54, 0x80);

	return 1 /*true*/;
}

static int mctl_phy_init(struct dram_para *para)
{
	struct sunxi_mctl_com_reg * const mctl_com =
			(struct sunxi_mctl_com_reg *)SUNXI_DRAM_COM_BASE;
	struct sunxi_mctl_ctl_reg * const mctl_ctl =
			(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;
	uint32_t val, *ptr;
	int i;

	if (para->bus_full_width)
		val = 0xf;
	else
		val = 3;
	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 0x3c, 0xf, val);

	writel(0xd, SUNXI_DRAM_PHY0_BASE + 0x14);
	writel(0xd, SUNXI_DRAM_PHY0_BASE + 0x35c);
	writel(0xd, SUNXI_DRAM_PHY0_BASE + 0x368);
	writel(0xd, SUNXI_DRAM_PHY0_BASE + 0x374);

	writel(0, SUNXI_DRAM_PHY0_BASE + 0x18);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x360);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x36c);
	writel(0, SUNXI_DRAM_PHY0_BASE + 0x378);

	writel(9, SUNXI_DRAM_PHY0_BASE + 0x1c);
	writel(9, SUNXI_DRAM_PHY0_BASE + 0x364);
	writel(9, SUNXI_DRAM_PHY0_BASE + 0x370);
	writel(9, SUNXI_DRAM_PHY0_BASE + 0x37c);

	ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0xc0);
	for (i = 0; i < ARRAY_SIZE(phy_init); i++)
		writel(phy_init[i], &ptr[i]);

	if (IS_ENABLED(CONFIG_DRAM_SUN50I_H616_UNKNOWN_FEATURE)) {
		ptr = (uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x780);
		for (i = 0; i < 32; i++)
			writel(0x16, &ptr[i]);
		writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x78c);
		writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x7a4);
		writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x7b8);
		writel(0x8, SUNXI_DRAM_PHY0_BASE + 0x7d4);
		writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x7dc);
		writel(0xe, SUNXI_DRAM_PHY0_BASE + 0x7e0);
	}

	writel(0x80, SUNXI_DRAM_PHY0_BASE + 0x3dc);
	writel(0x80, SUNXI_DRAM_PHY0_BASE + 0x45c);

	if (IS_ENABLED(CONFIG_DRAM_ODT_EN))
		mctl_phy_configure_odt();

	clrsetbits_le32(SUNXI_DRAM_PHY0_BASE + 4, 7, 0xa);

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

	mctl_await_completion((uint32_t*)(SUNXI_DRAM_PHY0_BASE + 0x180), 4, 4);

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

	writel(0x1f14, &mctl_ctl->mrctrl1);
	writel(0x80000030, &mctl_ctl->mrctrl0);
	mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

	writel(4, &mctl_ctl->mrctrl1);
	writel(0x80001030, &mctl_ctl->mrctrl0);
	mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

	writel(0x20, &mctl_ctl->mrctrl1);
	writel(0x80002030, &mctl_ctl->mrctrl0);
	mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

	writel(0, &mctl_ctl->mrctrl1);
	writel(0x80003030, &mctl_ctl->mrctrl0);
	mctl_await_completion(&mctl_ctl->mrctrl0, BIT_U32(31), 0);

	writel(0, SUNXI_DRAM_PHY0_BASE + 0x54);

	writel(0, &mctl_ctl->swctl);
	clrbits_le32(&mctl_ctl->rfshctl3, 1);
	writel(1, &mctl_ctl->swctl);

	if (IS_ENABLED(CONFIG_DRAM_SUN50I_H616_WRITE_LEVELING)) {
		for (i = 0; i < 5; i++)
			if (mctl_phy_write_leveling(para))
				break;
		if (i == 5) {
			PRINTF("write leveling failed!\n");
			return 0 /*fasle*/;
		}
	}

	if (IS_ENABLED(CONFIG_DRAM_SUN50I_H616_READ_CALIBRATION)) {
		for (i = 0; i < 5; i++)
			if (mctl_phy_read_calibration(para))
				break;
		if (i == 5) {
			PRINTF("read calibration failed!\n");
			return 0 /*fasle*/;
		}
	}

	if (IS_ENABLED(CONFIG_DRAM_SUN50I_H616_READ_TRAINING)) {
		for (i = 0; i < 5; i++)
			if (mctl_phy_read_training(para))
				break;
		if (i == 5) {
			PRINTF("read training failed!\n");
			return 0 /*fasle*/;
		}
	}

	if (IS_ENABLED(CONFIG_DRAM_SUN50I_H616_WRITE_TRAINING)) {
		for (i = 0; i < 5; i++)
			if (mctl_phy_write_training(para))
				break;
		if (i == 5) {
			PRINTF("write training failed!\n");
			return 0 /*fasle*/;
		}
	}

	if (IS_ENABLED(CONFIG_DRAM_SUN50I_H616_BIT_DELAY_COMPENSATION))
		mctl_phy_bit_delay_compensation(para);

	clrbits_le32(SUNXI_DRAM_PHY0_BASE + 0x60, 4);

	return 1 /*true*/;
}

static int mctl_ctrl_init(struct dram_para *para)
{
	struct sunxi_mctl_com_reg * const mctl_com =
			(struct sunxi_mctl_com_reg *)SUNXI_DRAM_COM_BASE;
	struct sunxi_mctl_ctl_reg * const mctl_ctl =
			(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;
	uint32_t reg_val;

	clrsetbits_le32(&mctl_com->unk_0x500, BIT_U32(24), 0x200);
	writel(0x8000, &mctl_ctl->clken);

	setbits_le32(&mctl_com->unk_0x008, 0xff00);

	clrsetbits_le32(&mctl_ctl->sched[0], 0xff00, 0x3000);

	writel(0, &mctl_ctl->hwlpctl);

	setbits_le32(&mctl_com->unk_0x008, 0xff00);

	reg_val = MSTR_BURST_LENGTH(8) | MSTR_ACTIVE_RANKS(para->ranks);
	reg_val |= MSTR_DEVICETYPE_DDR3 | MSTR_2TMODE;
	if (para->bus_full_width)
		reg_val |= MSTR_BUSWIDTH_FULL;
	else
		reg_val |= MSTR_BUSWIDTH_HALF;
	writel(BIT_U32(31) | BIT_U32(30) | reg_val, &mctl_ctl->mstr);

	if (para->ranks == 2)
		writel(0x0303, &mctl_ctl->odtmap);
	else
		writel(0x0201, &mctl_ctl->odtmap);

	writel(0x06000400, &mctl_ctl->odtcfg);
	writel(0x06000400, &mctl_ctl->unk_0x2240);
	writel(0x06000400, &mctl_ctl->unk_0x3240);
	writel(0x06000400, &mctl_ctl->unk_0x4240);

	setbits_le32(&mctl_com->cr, BIT_U32(31));

	mctl_set_addrmap(para);

	mctl_set_timing_params(para);

	writel(0, &mctl_ctl->pwrctl);

	setbits_le32(&mctl_ctl->dfiupd[0], BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->zqctl[0], BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x2180, BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x3180, BIT_U32(31) | BIT_U32(30));
	setbits_le32(&mctl_ctl->unk_0x4180, BIT_U32(31) | BIT_U32(30));

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

	if (!mctl_phy_init(para))
		return 0 /*fasle*/;

	writel(0, &mctl_ctl->swctl);
	clrbits_le32(&mctl_ctl->rfshctl3, BIT_U32(0));

	setbits_le32(&mctl_com->unk_0x014, BIT_U32(31));
	writel(0xffffffff, &mctl_com->maer0);
	writel(0x7ff, &mctl_com->maer1);
	writel(0xffff, &mctl_com->maer2);

	writel(1, &mctl_ctl->swctl);
	mctl_await_completion(&mctl_ctl->swstat, 1, 1);

	return 1 /*true*/;
}

static int mctl_core_init(struct dram_para *para)
{
	mctl_sys_init(para);

	return mctl_ctrl_init(para);
}

static void mctl_auto_detect_rank_width(struct dram_para *para)
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

static void mctl_auto_detect_dram_size(struct dram_para *para)
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

static unsigned long mctl_calc_size(struct dram_para *para)
{
	uint8_t width = para->bus_full_width ? 4 : 2;

	/* 8 banks */
	return (1ULL << (para->cols + para->rows + 3)) * width * para->ranks;
}

unsigned long sunxi_dram_init(void)
{
	struct dram_para para = {
		.clk = CONFIG_DRAM_CLK,
		.type = SUNXI_DRAM_TYPE_DDR3,
	};
	unsigned long size;

	setbits_le32(0x7010310, BIT_U32(8));
	clrbits_le32(0x7010318, 0x3f);

	mctl_auto_detect_rank_width(&para);
	mctl_auto_detect_dram_size(&para);

	mctl_core_init(&para);

	size = mctl_calc_size(&para);

	mctl_set_master_priority();

	return size;
};

#endif /* CPUSTYLE_T507 */
