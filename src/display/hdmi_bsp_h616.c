/*
 * Allwinner SoCs hdmi lowlevel driver.
 *
 * Copyright (C) 2017 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include "hardware.h"
#include "formats.h"	// for PRINTF prints

#if WITHLTDCHW && WITHHDMITVHW && 0

#define SUNXI_HDMI_BASE HDMI_TX0_BASE
#define BIT32(pos) (UINT32_C(1) << (pos))

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

static void writel(unsigned val, void * addr)
{
	write32((uintptr_t) addr, val);
}

static uint8_t readb(void * addr)
{
	__DSB();
	return * (volatile uint8_t *) addr;
}

static uint32_t readl(void * addr)
{
	__DSB();
	return * (volatile uint32_t *) addr;
}

#define clrbits_le32(addr, clear) \
	write32(((uintptr_t)(addr)), read32(((uintptr_t)(addr))) & ~(clear))

#define setbits_le32(addr, set) \
	write32(((uintptr_t)(addr)), read32(((uintptr_t)(addr))) | (set))

#define clrsetbits_le32(addr, clear, set) \
	write32(((uintptr_t)(addr)), (read32(((uintptr_t)(addr))) & ~(clear)) | (set))

struct sunxi_dw_hdmi_priv {
	int xx;
	//struct dw_hdmi hdmi;
};

struct sunxi_hdmi_phy {
	uint32_t pol;
	uint32_t res1[3];
	uint32_t read_en;
	uint32_t unscramble;
	uint32_t res2[2];
	uint32_t ctrl;
	uint32_t unk1;
	uint32_t unk2;
	uint32_t pll;
	uint32_t clk;
	uint32_t unk3;
	uint32_t status;
};

#define HDMI_PHY_OFFS 0x10000

static int sunxi_dw_hdmi_get_divider(unsigned clock)
{
	/*
	 * Due to missing documentaion of HDMI PHY, we know correct
	 * settings only for following four PHY dividers. Select one
	 * based on clock speed.
	 */
	if (clock <= 27000000)
		return 11;
	else if (clock <= 74250000)
		return 4;
	else if (clock <= 148500000)
		return 2;
	else
		return 1;
}

static void sunxi_dw_hdmi_phy_init(void)
{
	struct sunxi_hdmi_phy * const phy =
		(struct sunxi_hdmi_phy *)(SUNXI_HDMI_BASE + HDMI_PHY_OFFS);
	unsigned long tmo;
	uint32_t tmp;

	/*
	 * HDMI PHY settings are taken as-is from Allwinner BSP code.
	 * There is no documentation.
	 */
	writel(0, &phy->ctrl);
	setbits_le32(&phy->ctrl, BIT32(0));
	local_delay_us(5);
	setbits_le32(&phy->ctrl, BIT32(16));
	setbits_le32(&phy->ctrl, BIT32(1));
	local_delay_us(10);
	setbits_le32(&phy->ctrl, BIT32(2));
	local_delay_us(5);
	setbits_le32(&phy->ctrl, BIT32(3));
	local_delay_us(40);
	setbits_le32(&phy->ctrl, BIT32(19));
	local_delay_us(100);
	setbits_le32(&phy->ctrl, BIT32(18));
	setbits_le32(&phy->ctrl, 7 << 4);

	local_delay_ms(250);
	/* Note that Allwinner code doesn't fail in case of timeout */
//	tmo = timer_get_us() + 2000;
//	while ((readl(&phy->status) & 0x80) == 0) {
//		if (timer_get_us() > tmo) {
//			PRINTF("Warning: HDMI PHY init timeout!\n");
//			break;
//		}
//	}

	setbits_le32(&phy->ctrl, 0xf << 8);
	setbits_le32(&phy->ctrl, BIT32(7));

	writel(0x39dc5040, &phy->pll);
	writel(0x80084343, &phy->clk);
	local_delay_us(10000);
	writel(1, &phy->unk3);
	setbits_le32(&phy->pll, BIT32(25));
	local_delay_us(100000);
	tmp = (readl(&phy->status) & 0x1f800) >> 11;
	setbits_le32(&phy->pll, BIT32(31) | BIT32(30));
	setbits_le32(&phy->pll, tmp);
	writel(0x01FF0F7F, &phy->ctrl);
	writel(0x80639000, &phy->unk1);
	writel(0x0F81C405, &phy->unk2);

	/* enable read access to HDMI controller */
	writel(0x54524545, &phy->read_en);
	/* descramble register offsets */
	writel(0x42494E47, &phy->unscramble);
}

static void sunxi_dw_hdmi_phy_set(unsigned clock, int phy_div)
{
	struct sunxi_hdmi_phy * const phy =
		(struct sunxi_hdmi_phy *)(SUNXI_HDMI_BASE + HDMI_PHY_OFFS);
//	int divval = sunxi_dw_hdmi_get_divider(clock);
//	PRINTF("sunxi_dw_hdmi_get_divider: divval=%u\n", divval);
	int divval = 1;
	uint32_t tmp;

	/*
	 * Unfortunately, we don't know much about those magic
	 * numbers. They are taken from Allwinner BSP driver.
	 */
	switch (divval) {
	case 1:
		writel(0x30dc5fc0, &phy->pll);
		writel(0x800863C0 | (phy_div - 1), &phy->clk);
		local_delay_ms(10);
		writel(0x00000001, &phy->unk3);
		setbits_le32(&phy->pll, BIT32(25));
		local_delay_ms(200);
		tmp = (readl(&phy->status) & 0x1f800) >> 11;
		setbits_le32(&phy->pll, BIT32(31) | BIT32(30));
		if (tmp < 0x3d)
			setbits_le32(&phy->pll, tmp + 2);
		else
			setbits_le32(&phy->pll, 0x3f);
		local_delay_ms(100);
		writel(0x01FFFF7F, &phy->ctrl);
		writel(0x8063b000, &phy->unk1);
		writel(0x0F8246B5, &phy->unk2);
		break;
	case 2:
		writel(0x39dc5040, &phy->pll);
		writel(0x80084380 | (phy_div - 1), &phy->clk);
		local_delay_ms(10);
		writel(0x00000001, &phy->unk3);
		setbits_le32(&phy->pll, BIT32(25));
		local_delay_ms(100);
		tmp = (readl(&phy->status) & 0x1f800) >> 11;
		setbits_le32(&phy->pll, BIT32(31) | BIT32(30));
		setbits_le32(&phy->pll, tmp);
		writel(0x01FFFF7F, &phy->ctrl);
		writel(0x8063a800, &phy->unk1);
		writel(0x0F81C485, &phy->unk2);
		break;
	case 4:
		writel(0x39dc5040, &phy->pll);
		writel(0x80084340 | (phy_div - 1), &phy->clk);
		local_delay_ms(10);
		writel(0x00000001, &phy->unk3);
		setbits_le32(&phy->pll, BIT32(25));
		local_delay_ms(100);
		tmp = (readl(&phy->status) & 0x1f800) >> 11;
		setbits_le32(&phy->pll, BIT32(31) | BIT32(30));
		setbits_le32(&phy->pll, tmp);
		writel(0x01FFFF7F, &phy->ctrl);
		writel(0x8063b000, &phy->unk1);
		writel(0x0F81C405, &phy->unk2);
		break;
	case 11:
		writel(0x39dc5040, &phy->pll);
		writel(0x80084300 | (phy_div - 1), &phy->clk);
		local_delay_ms(10);
		writel(0x00000001, &phy->unk3);
		setbits_le32(&phy->pll, BIT32(25));
		local_delay_ms(100);
		tmp = (readl(&phy->status) & 0x1f800) >> 11;
		setbits_le32(&phy->pll, BIT32(31) | BIT32(30));
		setbits_le32(&phy->pll, tmp);
		writel(0x01FFFF7F, &phy->ctrl);
		writel(0x8063b000, &phy->unk1);
		writel(0x0F81C405, &phy->unk2);
		break;
	}
}

//static void sunxi_dw_hdmi_pll_set(unsigned clk_khz, int *phy_div)
//{
//	int value, n, m, divval, diff;
//	int best_n = 0, best_m = 0, best_div = 0, best_diff = 0x0FFFFFFF;
//
//	/*
//	 * Find the lowest divider resulting in a matching clock. If there
//	 * is no match, pick the closest lower clock, as monitors tend to
//	 * not sync to higher frequencies.
//	 */
//	for (divval = 1; divval <= 16; divval++) {
//		int target = clk_khz * divval;
//
//		if (target < 192000)
//			continue;
//		if (target > 912000)
//			continue;
//
//		for (m = 1; m <= 16; m++) {
//			n = (m * target) / 24000;
//
//			if (n >= 1 && n <= 128) {
//				value = (24000 * n) / m / divval;
//				diff = clk_khz - value;
//				if (diff < best_diff) {
//					best_diff = diff;
//					best_m = m;
//					best_n = n;
//					best_div = divval;
//				}
//			}
//		}
//	}
//
//	*phy_div = best_div;
//
//	clock_set_pll3_factors(best_m, best_n);
//	PRINTF("dotclock: %dkHz = %dkHz: (24MHz * %d) / %d / %d\n",
//	      clk_khz, (clock_get_pll3() / 1000) / best_div,
//	      best_n, best_m, best_div);
//}

//static void sunxi_dw_hdmi_lcdc_init(int mux, const struct display_timing *edid,
//				    int bpp)
//{
//	struct sunxi_ccm_reg * const ccm =
//		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
//	int divval = DIV_ROUND_UP(clock_get_pll3(), edid->pixelclock.typ);
//	struct sunxi_lcdc_reg *lcdc;
//
//	if (mux == 0) {
//		lcdc = (struct sunxi_lcdc_reg *)SUNXI_LCD0_BASE;
//
//		/* Reset off */
//		setbits_le32(&ccm->ahb_reset1_cfg, 1 << AHB_RESET_OFFSET_LCD0);
//
//		/* Clock on */
//		setbits_le32(&ccm->ahb_gate1, 1 << AHB_GATE_OFFSET_LCD0);
//		writel(CCM_LCD0_CTRL_GATE | CCM_LCD0_CTRL_M(divval),
//		       &ccm->lcd0_clk_cfg);
//	} else {
//		lcdc = (struct sunxi_lcdc_reg *)SUNXI_LCD1_BASE;
//
//		/* Reset off */
//		setbits_le32(&ccm->ahb_reset1_cfg, 1 << AHB_RESET_OFFSET_LCD1);
//
//		/* Clock on */
//		setbits_le32(&ccm->ahb_gate1, 1 << AHB_GATE_OFFSET_LCD1);
//		writel(CCM_LCD1_CTRL_GATE | CCM_LCD1_CTRL_M(divval),
//		       &ccm->lcd1_clk_cfg);
//	}
//
//	lcdc_init(lcdc);
//	lcdc_tcon1_mode_set(lcdc, edid, 0, 0);
//	lcdc_enable(lcdc, bpp);
//}

//static int sunxi_dw_hdmi_phy_cfg(struct dw_hdmi *hdmi, unsigned mpixelclock)
//{
//	int phy_div;
//
//	sunxi_dw_hdmi_pll_set(mpixelclock / 1000, &phy_div);
//	sunxi_dw_hdmi_phy_set(mpixelclock, phy_div);
//
//	return 0;
//}
//
//static int sunxi_dw_hdmi_read_edid(struct udevice *dev, u8 *buf, int buf_size)
//{
//	struct sunxi_dw_hdmi_priv *priv = dev_get_priv(dev);
//
//	return dw_hdmi_read_edid(&priv->hdmi, buf, buf_size);
//}
//
//static int sunxi_dw_hdmi_mode_valid(struct udevice *dev,
//				     const struct display_timing *timing)
//{
//	return timing->pixelclock.typ <= 297000000;
//}

//static int sunxi_dw_hdmi_enable(struct udevice *dev, int panel_bpp,
//				const struct display_timing *edid)
//{
//	struct sunxi_hdmi_phy * const phy =
//		(struct sunxi_hdmi_phy *)(SUNXI_HDMI_BASE + HDMI_PHY_OFFS);
////	struct display_plat *uc_plat = dev_get_uclass_plat(dev);
////	struct sunxi_dw_hdmi_priv *priv = dev_get_priv(dev);
//	int ret;
//
//	ret = dw_hdmi_enable(&priv->hdmi, edid);
//	if (ret)
//		return ret;
//
////	sunxi_dw_hdmi_lcdc_init(uc_plat->source_id, edid, panel_bpp);
//
//	if (edid->flags & DISPLAY_FLAGS_VSYNC_LOW)
//		setbits_le32(&phy->pol, 0x200);
//
//	if (edid->flags & DISPLAY_FLAGS_HSYNC_LOW)
//		setbits_le32(&phy->pol, 0x100);
//
//	setbits_le32(&phy->ctrl, 0xf << 12);
//
//	/*
//	 * This is last hdmi access before boot, so scramble addresses
//	 * again or othwerwise BSP driver won't work. Dummy read is
//	 * needed or otherwise last write doesn't get written correctly.
//	 */
//	(void)readb((void *) SUNXI_HDMI_BASE);
//	writel(0, &phy->unscramble);
//
//	return 0;
//}

//static int sunxi_dw_hdmi_probe(struct udevice *dev)
//{
//	struct sunxi_dw_hdmi_priv *priv = dev_get_priv(dev);
//	struct sunxi_ccm_reg * const ccm =
//		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
//	int ret;
//
//	/* Set pll3 to 297 MHz */
//	clock_set_pll3(297000000);
//
//	/* Set hdmi parent to pll3 */
//	clrsetbits_le32(&ccm->hdmi_clk_cfg, CCM_HDMI_CTRL_PLL_MASK,
//			CCM_HDMI_CTRL_PLL3);
//
//	/* Set ahb gating to pass */
//	setbits_le32(&ccm->ahb_reset1_cfg, 1 << AHB_RESET_OFFSET_HDMI);
//	setbits_le32(&ccm->ahb_reset1_cfg, 1 << AHB_RESET_OFFSET_HDMI2);
//	setbits_le32(&ccm->ahb_gate1, 1 << AHB_GATE_OFFSET_HDMI);
//	setbits_le32(&ccm->hdmi_slow_clk_cfg, CCM_HDMI_SLOW_CTRL_DDC_GATE);
//
//	/* Clock on */
//	setbits_le32(&ccm->hdmi_clk_cfg, CCM_HDMI_CTRL_GATE);
//
//	sunxi_dw_hdmi_phy_init();
//
//	priv->hdmi.ioaddr = SUNXI_HDMI_BASE;
//	priv->hdmi.i2c_clk_high = 0xd8;
//	priv->hdmi.i2c_clk_low = 0xfe;
//	priv->hdmi.reg_io_width = 1;
//	priv->hdmi.phy_set = sunxi_dw_hdmi_phy_cfg;
//
//	ret = dw_hdmi_phy_wait_for_hpd(&priv->hdmi);
//	if (ret < 0) {
//		PRINTF("hdmi can not get hpd signal\n");
//		return -1;
//	}
//
//	dw_hdmi_init(&priv->hdmi);
//
//	return 0;
//}

void t507_hdmi_phy_init(uint_fast32_t dotclock)
{
	struct sunxi_hdmi_phy * const phy =
		(struct sunxi_hdmi_phy *)(SUNXI_HDMI_BASE + HDMI_PHY_OFFS);
	sunxi_dw_hdmi_phy_init();
	sunxi_dw_hdmi_phy_set(dotclock, 1);

//	if (edid->flags & DISPLAY_FLAGS_VSYNC_LOW)
//		setbits_le32(&phy->pol, 0x200);
//
//	if (edid->flags & DISPLAY_FLAGS_HSYNC_LOW)
//		setbits_le32(&phy->pol, 0x100);
//
	if (1)
		setbits_le32(&phy->pol, 0x200);

	if (1)
		setbits_le32(&phy->pol, 0x100);

	setbits_le32(&phy->ctrl, 0xf << 12);
}
#endif
