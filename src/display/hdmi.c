/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "formats.h"	// for debug prints
#include "display.h"
#include "audio.h"

#if defined (HDMI_TX0) && WITHHDMITVHW

#include "dw-hdmi.h"


/* hdmi initialization step b.4 */
static void hdmi_enable_video_path(HDMI_TX_TypeDef * const hdmi, int audio)
{
	unsigned clkdis;

	/* control period minimum duration */
	hdmi->HDMI_FC_CTRLDUR = 12;
	hdmi->HDMI_FC_EXCTRLDUR = 32;
	hdmi->HDMI_FC_EXCTRLSPAC = 1;

	/* set to fill tmds data channels */
	hdmi->HDMI_FC_CH0PREAM = 0x0b;
	hdmi->HDMI_FC_CH1PREAM = 0x16;
	hdmi->HDMI_FC_CH2PREAM = 0x21;

	hdmi->HDMI_MC_FLOWCTRL = HDMI_MC_FLOWCTRL_FEED_THROUGH_OFF_CSC_BYPASS;

	/* enable pixel clock and tmds data path */
	clkdis = 0x7f;
	clkdis &= ~HDMI_MC_CLKDIS_PIXELCLK_DISABLE;
	hdmi->HDMI_MC_CLKDIS = clkdis;

	clkdis &= ~HDMI_MC_CLKDIS_TMDSCLK_DISABLE;
	hdmi->HDMI_MC_CLKDIS = clkdis;

	if (audio) {
		clkdis &= ~HDMI_MC_CLKDIS_AUDCLK_DISABLE;
		hdmi->HDMI_MC_CLKDIS = clkdis;
	}
}

static void hdmi_enable_audio_clk(HDMI_TX_TypeDef * const hdmi)
{
	hdmi->HDMI_MC_CLKDIS &= ~ HDMI_MC_CLKDIS_AUDCLK_DISABLE;
}

static void hdmi_phy_gen2_pddq(HDMI_TX_TypeDef *hdmi, unsigned enable)
{
	hdmi->HDMI_PHY_CONF0 = (hdmi->HDMI_PHY_CONF0 & ~ HDMI_PHY_CONF0_GEN2_PDDQ_MASK) |
			!! enable * (1 << HDMI_PHY_CONF0_GEN2_PDDQ_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_GEN2_PDDQ_MASK,
//		 enable << HDMI_PHY_CONF0_GEN2_PDDQ_OFFSET);
}
static inline void hdmi_phy_test_clear(HDMI_TX_TypeDef *hdmi, unsigned bit)
{
	hdmi->HDMI_PHY_TST0 = (hdmi->HDMI_PHY_TST0 & ~ HDMI_PHY_TST0_TSTCLR_MASK) |
			!! bit * (1 << HDMI_PHY_TST0_TSTCLR_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_TST0, HDMI_PHY_TST0_TSTCLR_MASK,
//		 bit << HDMI_PHY_TST0_TSTCLR_OFFSET);
}

static void hdmi_phy_gen2_txpwron(HDMI_TX_TypeDef *hdmi, unsigned enable)
{
	hdmi->HDMI_PHY_CONF0 = (hdmi->HDMI_PHY_CONF0 & ~ HDMI_PHY_CONF0_GEN2_TXPWRON_MASK) |
			!! enable * (1 << HDMI_PHY_CONF0_GEN2_TXPWRON_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_CONF0,
//		 HDMI_PHY_CONF0_GEN2_TXPWRON_MASK,
//		 enable << HDMI_PHY_CONF0_GEN2_TXPWRON_OFFSET);
}

static void hdmi_phy_sel_data_en_pol(HDMI_TX_TypeDef *hdmi, unsigned enable)
{
	hdmi->HDMI_PHY_CONF0 = (hdmi->HDMI_PHY_CONF0 & ~ HDMI_PHY_CONF0_SELDATAENPOL_MASK) |
			!! enable * (1 << HDMI_PHY_CONF0_SELDATAENPOL_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_CONF0,
//		 HDMI_PHY_CONF0_SELDATAENPOL_MASK,
//		 enable << HDMI_PHY_CONF0_SELDATAENPOL_OFFSET);
}

static void hdmi_phy_sel_interface_control(HDMI_TX_TypeDef *hdmi, unsigned enable)
{
	hdmi->HDMI_PHY_CONF0 = (hdmi->HDMI_PHY_CONF0 & ~ HDMI_PHY_CONF0_SELDIPIF_MASK) |
			!! enable * (1 << HDMI_PHY_CONF0_SELDIPIF_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_SELDIPIF_MASK,
//		 enable << HDMI_PHY_CONF0_SELDIPIF_OFFSET);
}

static void hdmi_phy_enable_power(HDMI_TX_TypeDef *hdmi, unsigned enable)
{
	hdmi->HDMI_PHY_CONF0 = (hdmi->HDMI_PHY_CONF0 & ~ HDMI_PHY_CONF0_PDZ_MASK) |
			!! enable * (1 << HDMI_PHY_CONF0_PDZ_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_PDZ_MASK,
//		 enable << HDMI_PHY_CONF0_PDZ_OFFSET);
}

static void hdmi_phy_enable_tmds(HDMI_TX_TypeDef *hdmi, unsigned enable)
{
	hdmi->HDMI_PHY_CONF0 = (hdmi->HDMI_PHY_CONF0 & ~ HDMI_PHY_CONF0_ENTMDS_MASK) |
			!! enable * (1 << HDMI_PHY_CONF0_ENTMDS_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_ENTMDS_MASK,
//		 enable << HDMI_PHY_CONF0_ENTMDS_OFFSET);
}

static void hdmi_phy_enable_spare(HDMI_TX_TypeDef *hdmi, unsigned enable)
{
	hdmi->HDMI_PHY_CONF0 = (hdmi->HDMI_PHY_CONF0 & ~ HDMI_PHY_CONF0_SPARECTRL_MASK) |
			!! enable * (1 << HDMI_PHY_CONF0_SPARECTRL_OFFSET);
//	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_SPARECTRL_MASK,
//		 enable << HDMI_PHY_CONF0_SPARECTRL_OFFSET);
}

static uint_fast32_t hdmi_compute_n(uint_fast32_t freq, uint_fast64_t pixel_clk)
{
	unsigned int n = (128 * freq) / 1000;
	unsigned int mult = 1;

	while (freq > 48000) {
		mult *= 2;
		freq /= 2;
	}

	switch (freq) {
	case 32000:
		if (pixel_clk == 25175000)
			n = 4576;
		else if (pixel_clk == 27027000)
			n = 4096;
		else if (pixel_clk == 74176000 || pixel_clk == 148352000)
			n = 11648;
		else
			n = 4096;
		n *= mult;
		break;

	case 44100:
		if (pixel_clk == 25175000)
			n = 7007;
		else if (pixel_clk == 74176000)
			n = 17836;
		else if (pixel_clk == 148352000)
			n = 8918;
		else
			n = 6272;
		n *= mult;
		break;

	case 48000:
		if (pixel_clk == 25175000)
			n = 6864;
		else if (pixel_clk == 27027000)
			n = 6144;
		else if (pixel_clk == 74176000)
			n = 11648;
		else if (pixel_clk == 148352000)
			n = 5824;
		else
			n = 6144;
		n *= mult;
		break;

	default:
		break;
	}

	return n;
}

static void hdmi_set_cts_n(HDMI_TX_TypeDef * const hdmi, unsigned int cts,
			   unsigned int n)
{
	/* Must be set/cleared first */
	hdmi->HDMI_AUD_CTS3 &= (uint8_t) ~ HDMI_AUD_CTS3_CTS_MANUAL;
//	hdmi_modb(hdmi, 0, HDMI_AUD_CTS3_CTS_MANUAL, HDMI_AUD_CTS3);

	/* nshift factor = 0 */
	hdmi->HDMI_AUD_CTS3 &= (uint8_t) ~ HDMI_AUD_CTS3_N_SHIFT_MASK;
//	hdmi_modb(hdmi, 0, HDMI_AUD_CTS3_N_SHIFT_MASK, HDMI_AUD_CTS3);

	//  HDMI Audio Clock Regenerator CTS calculated value
	hdmi->HDMI_AUD_CTS3 = ((cts >> 16) & HDMI_AUD_CTS3_AUDCTS19_16_MASK) | HDMI_AUD_CTS3_CTS_MANUAL;
//	hdmi_writeb(hdmi, ((cts >> 16) & HDMI_AUD_CTS3_AUDCTS19_16_MASK) |
//		    HDMI_AUD_CTS3_CTS_MANUAL, HDMI_AUD_CTS3);
	hdmi->HDMI_AUD_CTS2 = (cts >> 8) & 0xff;
//	hdmi_writeb(hdmi, (cts >> 8) & 0xff, HDMI_AUD_CTS2);
	hdmi->HDMI_AUD_CTS1 = cts & 0xff;
//	hdmi_writeb(hdmi, cts & 0xff, HDMI_AUD_CTS1);

	// HDMI Audio Clock Regenerator N valu
	hdmi->HDMI_AUD_N3 = (n >> 16) & 0x0f;
//	hdmi_writeb(hdmi, (n >> 16) & 0x0f, HDMI_AUD_N3);
	hdmi->HDMI_AUD_N2 = (n >> 8) & 0xff;
//	hdmi_writeb(hdmi, (n >> 8) & 0xff, HDMI_AUD_N2);
	hdmi->HDMI_AUD_N1 = n & 0xff;
//	hdmi_writeb(hdmi, n & 0xff, HDMI_AUD_N1);
}

static int hdmi_phy_wait_i2c_done(HDMI_TX_TypeDef *hdmi, unsigned msec)
{
	const int ec = local_wait8mask(& hdmi->HDMI_IH_I2CMPHY_STAT0, 0x02, 0x02, msec);
	const uint32_t val = hdmi->HDMI_IH_I2CMPHY_STAT0;
	hdmi->HDMI_IH_I2CMPHY_STAT0 = val & 0x03;
	return ec || (val & 0x01) ? 1 : 0;	// i2cmphyerror check

//	local_delay_ms(msec);
//	return 0;
	for (;;)
	{
		const uint32_t val = hdmi->HDMI_IH_I2CMPHY_STAT0;
		if (val & 0x03)
		{
			 hdmi->HDMI_IH_I2CMPHY_STAT0 = val;
			 return (val & 0x01) ? 1 : 0;	// i2cmphyerror chack
		}
	}
	// timeout
	return 1;
//	ulong start;
//	uint32_t val;
//
//	start = get_timer(0);
//	do {
//		val = hdmi_read(hdmi, HDMI_IH_I2CMPHY_STAT0);
//		if (val & 0x3) {
//			hdmi_write(hdmi, val, HDMI_IH_I2CMPHY_STAT0);
//			return 0;
//		}
//
//		udelay(100);
//	} while (get_timer(start) < msec);
//
	return 1;
}
#define HDMI_PHY_TIMEOUT 100

static int hdmi_phy_i2c_write(HDMI_TX_TypeDef *hdmi, unsigned data, unsigned addr)
{
	// I2C reset
//	hdmi->HDMI_PHY_I2CM_SOFTRSTZ_ADDR = 0x00;
//	while ((hdmi->HDMI_PHY_I2CM_SOFTRSTZ_ADDR & 0x01) == 0)
//		;
	hdmi->HDMI_PHY_I2CM_SLAVE_ADDR = HDMI_PHY_I2CM_SLAVE_ADDR_PHY_GEN2;
	hdmi->HDMI_IH_I2CMPHY_STAT0 = 0xFF;
	hdmi->HDMI_PHY_I2CM_ADDRESS_ADDR = addr;
	hdmi->HDMI_PHY_I2CM_DATAO_1_ADDR = (data >> 8);
	hdmi->HDMI_PHY_I2CM_DATAO_0_ADDR = (data >> 0);
	hdmi->HDMI_PHY_I2CM_OPERATION_ADDR = HDMI_PHY_I2CM_OPERATION_ADDR_WRITE;

//	hdmi_write(hdmi, 0xff, HDMI_IH_I2CMPHY_STAT0);
//	hdmi_write(hdmi, addr, HDMI_PHY_I2CM_ADDRESS_ADDR);
//	hdmi_write(hdmi, (uint8_t)(data >> 8), HDMI_PHY_I2CM_DATAO_1_ADDR);
//	hdmi_write(hdmi, (uint8_t)(data >> 0), HDMI_PHY_I2CM_DATAO_0_ADDR);
//	hdmi_write(hdmi, HDMI_PHY_I2CM_OPERATION_ADDR_WRITE,
//		   HDMI_PHY_I2CM_OPERATION_ADDR);

	int ec = hdmi_phy_wait_i2c_done(hdmi, HDMI_PHY_TIMEOUT);
	hdmi->HDMI_PHY_I2CM_OPERATION_ADDR = 0;
	return ec;
}

static int hdmi_phy_i2c_read(HDMI_TX_TypeDef *hdmi, unsigned * data, unsigned addr)
{
	// I2C reset
//	hdmi->HDMI_PHY_I2CM_SOFTRSTZ_ADDR = 0x00;
//	while ((hdmi->HDMI_PHY_I2CM_SOFTRSTZ_ADDR & 0x01) == 0)
//		;
	hdmi->HDMI_PHY_I2CM_SLAVE_ADDR = HDMI_PHY_I2CM_SLAVE_ADDR_PHY_GEN2;
	hdmi->HDMI_IH_I2CMPHY_STAT0 = 0xFF;
	hdmi->HDMI_PHY_I2CM_ADDRESS_ADDR = addr;
	hdmi->HDMI_PHY_I2CM_OPERATION_ADDR = HDMI_PHY_I2CM_OPERATION_ADDR_READ;

//	hdmi_write(hdmi, 0xff, HDMI_IH_I2CMPHY_STAT0);
//	hdmi_write(hdmi, addr, HDMI_PHY_I2CM_ADDRESS_ADDR);
//	hdmi_write(hdmi, (uint8_t)(data >> 8), HDMI_PHY_I2CM_DATAO_1_ADDR);
//	hdmi_write(hdmi, (uint8_t)(data >> 0), HDMI_PHY_I2CM_DATAO_0_ADDR);
//	hdmi_write(hdmi, HDMI_PHY_I2CM_OPERATION_ADDR_WRITE,
//		   HDMI_PHY_I2CM_OPERATION_ADDR);

	* data = 0xDEADBEEF;
	int ec = hdmi_phy_wait_i2c_done(hdmi, HDMI_PHY_TIMEOUT);
	if (0 == ec)
	{
		* data = hdmi->HDMI_PHY_I2CM_DATAI_1_ADDR * 256 + hdmi->HDMI_PHY_I2CM_DATAI_0_ADDR;
	}
	hdmi->HDMI_PHY_I2CM_OPERATION_ADDR = 0;
	return ec;
}

enum {
	DW_HDMI_RES_8,
	DW_HDMI_RES_10,
	DW_HDMI_RES_12,
	DW_HDMI_RES_MAX,
};

struct dw_hdmi_mpll_config {
	unsigned long mpixelclock;
	struct {
		uint16_t cpce;
		uint16_t gmp;
	} res[DW_HDMI_RES_MAX];
};

struct dw_hdmi_curr_ctrl {
	unsigned long mpixelclock;
	uint16_t curr[DW_HDMI_RES_MAX];
};

struct dw_hdmi_phy_config {
	uint64_t mpixelclock;
	uint32_t sym_ctr;    /* clock symbol and transmitter control */
	uint32_t term;       /* transmission termination value */
	uint32_t vlev_ctr;   /* voltage level control */
};

// https://github.com/EchoHeim/Allwinner-H616/blob/c499413803e4128439cadf2f56972e207721abe4/kernel/drivers/gpu/drm/sun4i/sun8i_hdmi_phy.c#L18
// https://github.com/EchoHeim/Allwinner-H616/blob/master/kernel/drivers/gpu/drm/sun4i/sun8i_hdmi_phy.c

#if CPUSTYLE_A733 && 0
// https://github.com/radxa/allwinner-bsp/blob/cubie-aiot-v1.4.6/drivers/drm/sunxi_device/sunxi_hdmi.c
// https://github.com/radxa/allwinner-bsp/blob/bf1d47a3b42d906cdfbfd7937f316a97b9919ae5/drivers/drm/sunxi_device/sunxi_hdmi.c#L64

#elif CPUSTYLE_T507 || CPUSTYLE_A733 || 1

static const struct dw_hdmi_mpll_config sun50i_h616_mpll_cfg[] = {
    {
        27000000,
        {
            {0x00b3, 0x0003},
            {0x2153, 0x0003},
            {0x40f3, 0x0003},
        },
    },
    {
        74250000,
        {
            {0x0072, 0x0003},
            {0x2145, 0x0003},
            {0x4061, 0x0003},
        },
    },
    {
        148500000,
        {
            {0x0051, 0x0003},
            {0x214c, 0x0003},
            {0x4064, 0x0003},
        },
    },
    {
        297000000,
        {
            {0x0040, 0x0003},
            {0x3b4c, 0x0003},
            {0x5a64, 0x0003},
        },
    },
    {
        594000000,
        {
            {0x1a40, 0x0003},
            {0x3b4c, 0x0003},
            {0x5a64, 0x0003},
        },
    },
    {
        ~0UL,
        {
            {0x0000, 0x0000},
            {0x0000, 0x0000},
            {0x0000, 0x0000},
        },
    }};

static const struct dw_hdmi_curr_ctrl sun50i_h616_cur_ctr[] = {
    /* pixelclk    bpp8    bpp10   bpp12 */
    {
        27000000,
        {0x0012, 0x0000, 0x0000},
    },
    {
        74250000,
        {0x0013, 0x0013, 0x0013},
    },
    {
        148500000,
        {0x0019, 0x0019, 0x0019},
    },
    {
        297000000,
        {0x0019, 0x001b, 0x0019},
    },
    {
        594000000,
        {0x0010, 0x0010, 0x0010},
    },
    {
        ~0UL,
        {0x0000, 0x0000, 0x0000},
    }};

static const struct dw_hdmi_phy_config sun50i_h616_phy_config[] = {
    /*pixelclk   symbol   term   vlev*/
    {27000000, 0x8009, 0x0007, 0x02b0},
    {74250000, 0x8019, 0x0004, 0x0290},
    {148500000, 0x8019, 0x0004, 0x0290},
    {297000000, 0x8039, 0x0004, 0x022b},
    {594000000, 0x8029, 0x0000, 0x008a},
    {~0UL, 0x0000, 0x0000, 0x0000}};
#else
	#warning Undefined CPUSTYLE_xxx

#endif

// See also https://github.com/MYIR-ALLWINNER/myir-t5-kernel/blob/a7089355dd727f5aaedade642f5fbc5b354b215a/drivers/gpu/drm/bridge/dw-hdmi.c#L733

static int hdmi_phy_configure(HDMI_TX_TypeDef * const hdmi, uint_fast32_t mpixelclock, unsigned res, int cscon)
{

	PRINTF("hdmi_phy_configure: hdmi->mpixelclock=%u\n", (unsigned) mpixelclock);
	//PRINTF("hdmi_phy_configure: hdmi->HDMI_PHY_STAT0=%08X\n", (unsigned) hdmi->HDMI_PHY_STAT0);

	/* Enable csc path */
	HDMI_TX0->HDMI_MC_FLOWCTRL = cscon ? HDMI_MC_FLOWCTRL_FEED_THROUGH_OFF_CSC_IN_PATH : HDMI_MC_FLOWCTRL_FEED_THROUGH_OFF_CSC_BYPASS;

	/* gen2 tx power off */
	hdmi_phy_gen2_txpwron(hdmi, 0);
//
//	/* gen2 pddq */
	hdmi_phy_gen2_pddq(hdmi, 1);

	/* phy reset */
	HDMI_TX0->HDMI_MC_PHYRSTZ = HDMI_MC_PHYRSTZ_DEASSERT;
	HDMI_TX0->HDMI_MC_PHYRSTZ = HDMI_MC_PHYRSTZ_ASSERT;
	HDMI_TX0->HDMI_MC_HEACPHY_RST = HDMI_MC_HEACPHY_RST_ASSERT;

//	HDMI_TX0->HDMI_MC_PHYRSTZ = HDMI_MC_PHYRSTZ_DEASSERT;
//	HDMI_TX0->HDMI_MC_HEACPHY_RST = HDMI_MC_HEACPHY_RST_DEASSERT;

	hdmi_phy_test_clear(hdmi, 1);
	hdmi->HDMI_PHY_I2CM_SLAVE_ADDR = HDMI_PHY_I2CM_SLAVE_ADDR_PHY_GEN2;
	hdmi_phy_test_clear(hdmi, 0);

	const struct dw_hdmi_mpll_config *mpll_config = sun50i_h616_mpll_cfg;
	const struct dw_hdmi_curr_ctrl *curr_ctrl = sun50i_h616_cur_ctr;
	const struct dw_hdmi_phy_config *phy_config = sun50i_h616_phy_config;

	/* PLL/MPLL Cfg - always match on final entry */
	for (; mpll_config->mpixelclock != ~0UL; mpll_config++)
		if (mpixelclock <=
		    mpll_config->mpixelclock)
			break;

	for (; curr_ctrl->mpixelclock != ~0UL; curr_ctrl++)
		if (mpixelclock <=
		    curr_ctrl->mpixelclock)
			break;

	for (; phy_config->mpixelclock != ~0UL; phy_config++)
		if (mpixelclock <=
		    phy_config->mpixelclock)
			break;

	if (mpll_config->mpixelclock == ~0UL ||
	    curr_ctrl->mpixelclock == ~0UL ||
	    phy_config->mpixelclock == ~0UL) {
		PRINTF("Pixel clock %u - unsupported by HDMI\n",
			(unsigned) mpixelclock);
		return -1;
	}

	unsigned datav;	// debug variable

	const int resix = DW_HDMI_RES_8;
	hdmi_phy_i2c_write(hdmi, mpll_config->res[resix].cpce, PHY_OPMODE_PLLCFG);
	hdmi_phy_i2c_read(hdmi, & datav, PHY_OPMODE_PLLCFG);
	ASSERT(datav == mpll_config->res[resix].cpce);

#if CPUSTYLE_A733
	enum { PHY_PLLGMPCTRL_11 = 0x11 };
	hdmi_phy_i2c_write(hdmi, mpll_config->res[resix].gmp, PHY_PLLGMPCTRL_11);
	hdmi_phy_i2c_read(hdmi, & datav, PHY_PLLGMPCTRL_11);
	ASSERT(datav == mpll_config->res[resix].gmp);
#else
	hdmi_phy_i2c_write(hdmi, mpll_config->res[resix].gmp, PHY_PLLGMPCTRL);
	hdmi_phy_i2c_read(hdmi, & datav, PHY_PLLGMPCTRL);
	ASSERT(datav == mpll_config->res[resix].gmp);
#endif

	hdmi_phy_i2c_write(hdmi, curr_ctrl->curr[resix], PHY_PLLCURRCTRL);
	hdmi_phy_i2c_read(hdmi, & datav, PHY_PLLCURRCTRL);
	ASSERT(datav == curr_ctrl->curr[resix]);

	hdmi_phy_i2c_write(hdmi, 0x0000, PHY_PLLPHBYCTRL);
	hdmi_phy_i2c_read(hdmi, & datav, PHY_PLLPHBYCTRL);
	ASSERT(datav == 0x0000);

	hdmi_phy_i2c_write(hdmi, 0x0006, PHY_PLLCLKBISTPHASE);
	hdmi_phy_i2c_read(hdmi, & datav, PHY_PLLCLKBISTPHASE);
	ASSERT(datav == 0x0006);

//	for (i = 0; hdmi->phy_cfg[i].mpixelclock != (~0ul); i++)
//		if (mpixelclock <= hdmi->phy_cfg[i].mpixelclock)
//			break;

	/*
	 * resistance term 133ohm cfg
	 * preemp cgf 0.00
	 * tx/ck lvl 10
	 */
	hdmi_phy_i2c_write(hdmi, phy_config->term, PHY_TXTERM);
	hdmi_phy_i2c_read(hdmi, & datav, PHY_TXTERM);
	ASSERT(datav == phy_config->term);

	hdmi_phy_i2c_write(hdmi, phy_config->sym_ctr, PHY_CKSYMTXCTRL);
	hdmi_phy_i2c_read(hdmi, & datav, PHY_CKSYMTXCTRL);
	ASSERT(datav == phy_config->sym_ctr);

	hdmi_phy_i2c_write(hdmi, phy_config->vlev_ctr, PHY_VLEVCTRL);
	hdmi_phy_i2c_read(hdmi, & datav, PHY_VLEVCTRL);
	//PRINTF("datav=0x%04X, expected=0x%04X\n", datav, phy_config->vlev_ctr);
	//ASSERT(datav == phy_config->vlev_ctr);


	/* remove clk term */
	hdmi_phy_i2c_write(hdmi, 0x8000, PHY_CKCALCTRL);
	hdmi_phy_i2c_read(hdmi, & datav, PHY_CKCALCTRL);
	//PRINTF("datav=0x%04X, expected=0x%04X\n", datav, 0x8000);
	//ASSERT(datav == 0x8000);

	//PRINTF("hdmi->HDMI_PHY_STAT0=%08X\n", (unsigned) hdmi->HDMI_PHY_STAT0);

	hdmi_phy_enable_power(hdmi, 1);

	/* toggle tmds enable */
	hdmi_phy_enable_tmds(hdmi, 0);
	hdmi_phy_enable_tmds(hdmi, 1);

	/* gen2 tx power on */
	hdmi_phy_gen2_txpwron(hdmi, 1);
	hdmi_phy_gen2_pddq(hdmi, 0);

	hdmi_phy_enable_spare(hdmi, 1);

	/* wait for phy pll lock */
	if (local_wait8mask(& hdmi->HDMI_PHY_STAT0, HDMI_PHY_TX_PHY_LOCK, HDMI_PHY_TX_PHY_LOCK, 100))
	{
		PRINTF("HDMI PLL not statred: hdmi->HDMI_PHY_STAT0=0x%02X\n", (unsigned) hdmi->HDMI_PHY_STAT0);
	}
	else
	{
		//PRINTF("HDMI PLL statred: hdmi->HDMI_PHY_STAT0=0x%02X\n", (unsigned) hdmi->HDMI_PHY_STAT0);
	}

//	start = get_timer(0);
//	do {
//		val = hdmi_read(hdmi, HDMI_PHY_STAT0);
//		if (!(val & HDMI_PHY_TX_PHY_LOCK))
//			return 0;
//
//		udelay(100);
//	} while (get_timer(start) < 5);
	return 0;	// OK
}

static void hdmi_tx_hdcp_config(HDMI_TX_TypeDef * const hdmi)
{
	uint8_t de;

	if (1 /*hdmi->hdmi_data.video_mode.mdataenablepolarity */)
		de = HDMI_A_VIDPOLCFG_DATAENPOL_ACTIVE_HIGH;
	else
		de = HDMI_A_VIDPOLCFG_DATAENPOL_ACTIVE_LOW;

	/* disable rx detect */
	hdmi->HDMI_A_HDCPCFG0 = (hdmi->HDMI_A_HDCPCFG0 & ~ HDMI_A_HDCPCFG0_RXDETECT_MASK) |
			HDMI_A_HDCPCFG0_RXDETECT_DISABLE;
//	hdmi_modb(hdmi, HDMI_A_HDCPCFG0_RXDETECT_DISABLE,
//		  HDMI_A_HDCPCFG0_RXDETECT_MASK, HDMI_A_HDCPCFG0);

	hdmi->HDMI_A_VIDPOLCFG = (hdmi->HDMI_A_VIDPOLCFG & ~ HDMI_A_VIDPOLCFG_DATAENPOL_MASK) |
			de;
//	hdmi_modb(hdmi, de, HDMI_A_VIDPOLCFG_DATAENPOL_MASK, HDMI_A_VIDPOLCFG);

	hdmi->HDMI_A_HDCPCFG1 = (hdmi->HDMI_A_HDCPCFG0 & ~ HDMI_A_HDCPCFG1_ENCRYPTIONDISABLE_MASK) |
			HDMI_A_HDCPCFG1_ENCRYPTIONDISABLE_DISABLE;
//	hdmi_modb(hdmi, HDMI_A_HDCPCFG1_ENCRYPTIONDISABLE_DISABLE,
//		  HDMI_A_HDCPCFG1_ENCRYPTIONDISABLE_MASK, HDMI_A_HDCPCFG1);
}

static void hdmi_video_sample(HDMI_TX_TypeDef * const hdmi)
{
	uint32_t val;
	// https://github.com/MYIR-ALLWINNER/myir-t5-kernel/blob/a7089355dd727f5aaedade642f5fbc5b354b215a/drivers/gpu/drm/bridge/dw-hdmi.c#L366
	unsigned color_format = 0x01;	// RGB, 8 bit

	val = HDMI_TX_INVID0_INTERNAL_DE_GENERATOR_DISABLE |
		((color_format << HDMI_TX_INVID0_VIDEO_MAPPING_OFFSET) &
		HDMI_TX_INVID0_VIDEO_MAPPING_MASK);
	hdmi->HDMI_TX_INVID0 = val;

	/* Enable TX stuffing: When DE is inactive, fix the output data to 0 */
	val = HDMI_TX_INSTUFFING_BDBDATA_STUFFING_ENABLE |
		HDMI_TX_INSTUFFING_RCRDATA_STUFFING_ENABLE |
		HDMI_TX_INSTUFFING_GYDATA_STUFFING_ENABLE;
	hdmi->HDMI_TX_INSTUFFING = val;
	hdmi->HDMI_TX_GYDATA0 = 0x00;
	hdmi->HDMI_TX_GYDATA1 = 0x00;
	hdmi->HDMI_TX_RCRDATA0 = 0x00;
	hdmi->HDMI_TX_RCRDATA1 = 0x00;
	hdmi->HDMI_TX_BCBDATA0 = 0x00;
	hdmi->HDMI_TX_BCBDATA1 = 0x00;
}

static void dw_hdmi_i2s_setup(HDMI_TX_TypeDef * const hdmi)
{
	/* Reset the FIFOs before applying new params */
	hdmi->HDMI_AUD_CONF0 = HDMI_AUD_CONF0_SW_RESET;
	hdmi->HDMI_MC_SWRSTZ = ~HDMI_MC_SWRSTZ_I2SSWRST_REQ;

	unsigned int inputclkfs	= HDMI_AUD_INPUTCLKFS_64FS;
	unsigned int conf0 = (HDMI_AUD_CONF0_I2S_SELECT | HDMI_AUD_CONF0_I2S_EN0);
	unsigned int conf1 = 0;

	//	switch (hparms->channels) {
	//	case 7 ... 8:
	//		conf0 |= HDMI_AUD_CONF0_I2S_EN3;
	//		fallthrough;
	//	case 5 ... 6:
	//		conf0 |= HDMI_AUD_CONF0_I2S_EN2;
	//		fallthrough;
	//	case 3 ... 4:
	//		conf0 |= HDMI_AUD_CONF0_I2S_EN1;
	//		/* Fall-thru */
	//	}

	//	switch (hparms->sample_width) {
	//	case 16:
	//		conf1 = HDMI_AUD_CONF1_WIDTH_16;
	//		break;
	//	case 24:
	//	case 32:
	//		conf1 = HDMI_AUD_CONF1_WIDTH_24;
	//		break;
	//	}

	//	switch (fmt->fmt) {
	//	case HDMI_I2S:
	//		conf1 |= HDMI_AUD_CONF1_MODE_I2S;
	//		break;
	//	case HDMI_RIGHT_J:
	//		conf1 |= HDMI_AUD_CONF1_MODE_RIGHT_J;
	//		break;
	//	case HDMI_LEFT_J:
	//		conf1 |= HDMI_AUD_CONF1_MODE_LEFT_J;
	//		break;
	//	case HDMI_DSP_A:
	//		conf1 |= HDMI_AUD_CONF1_MODE_BURST_1;
	//		break;
	//	case HDMI_DSP_B:
	//		conf1 |= HDMI_AUD_CONF1_MODE_BURST_2;
	//		break;
	//	default:
	//		dev_err(dev, "unsupported format\n");
	//		return -EINVAL;
	//	}

	//	dw_hdmi_set_sample_rate(hdmi, hparms->sample_rate);
	//	dw_hdmi_set_channel_status(hdmi, hparms->iec.status);
	//	dw_hdmi_set_channel_count(hdmi, hparms->channels);
	//	dw_hdmi_set_channel_allocation(hdmi, hparms->cea.channel_allocation);
	//


	conf1 = WITHADAPTERHDMIWIDTH == 16 ? HDMI_AUD_CONF1_WIDTH_16 : HDMI_AUD_CONF1_WIDTH_24;
	conf1 |= HDMI_AUD_CONF1_MODE_I2S;

	hdmi->HDMI_AUD_INPUTCLKFS = inputclkfs;
	hdmi->HDMI_AUD_CONF0 = conf0;
	hdmi->HDMI_AUD_CONF1 = conf1;
	//	hdmi_write(audio, inputclkfs, HDMI_AUD_INPUTCLKFS);
	//	hdmi_write(audio, conf0, HDMI_AUD_CONF0);
	//	hdmi_write(audio, conf1, HDMI_AUD_CONF1);

	//	PRINTF("hdmi->HDMI_AUD_INPUTCLKFS=%08X\n", (unsigned) hdmi->HDMI_AUD_INPUTCLKFS);
	//	PRINTF("hdmi->HDMI_AUD_CONF0=%08X\n", (unsigned) hdmi->HDMI_AUD_CONF0);
	//	PRINTF("hdmi->HDMI_AUD_CONF1=%08X\n", (unsigned) hdmi->HDMI_AUD_CONF1);
}

static void dw_hdmi_clear_overflow(HDMI_TX_TypeDef * const hdmi)
{
	int count;
	uint8_t val;

	/* TMDS software reset */
	hdmi->HDMI_MC_SWRSTZ = (uint8_t) ~ HDMI_MC_SWRSTZ_TMDSSWRST_REQ;
	if (local_wait8mask(& hdmi->HDMI_MC_SWRSTZ, HDMI_MC_SWRSTZ_TMDSSWRST_REQ, 1 * HDMI_MC_SWRSTZ_TMDSSWRST_REQ, 100))
	{
		TP();
	}
//	while ((hdmi->HDMI_MC_SWRSTZ & HDMI_MC_SWRSTZ_TMDSSWRST_REQ) == 0)
//		;

	val = hdmi->HDMI_FC_INVIDCONF;
//	if (hdmi->dev_type == IMX6DL_HDMI) {
//		hdmi->HDMI_FC_INVIDCONF = val;
//		return;
//	}

	for (count = 0; count < 4; count++)
		hdmi->HDMI_FC_INVIDCONF = val;
}

//static void sun8i_hdmi_phy_unlock(struct sun8i_hdmi_phy *phy)
//{
//    /* enable read access to HDMI controller */
//    regmap_vsu_write(phy->regs, SUN8I_HDMI_PHY_READ_EN_REG,
//                 SUN8I_HDMI_PHY_READ_EN_MAGIC);
//
//    /* unscramble register offsets */
//    regmap_vsu_write(phy->regs, SUN8I_HDMI_PHY_UNSCRAMBLE_REG,
//                 SUN8I_HDMI_PHY_UNSCRAMBLE_MAGIC);
//}
//
//static void sun50i_hdmi_phy_init_h616(struct sun8i_hdmi_phy *phy)
//{
//    regmap_update_bits(phy->regs, SUN8I_HDMI_PHY_REXT_CTRL_REG,
//                       SUN8I_HDMI_PHY_REXT_CTRL_REXT_EN,
//                       SUN8I_HDMI_PHY_REXT_CTRL_REXT_EN);
//
//    regmap_update_bits(phy->regs, SUN8I_HDMI_PHY_REXT_CTRL_REG,
//                       0xffff0000, 0x80c00000);
//}
//##	06010000: 00000000 FFFF0000 00184210 00000002 00000000 000F990B 00000004 00000000
//##	06010020: 00000F80 0C0040D8 02700203 000C6023 00000000 00000000 00000000 00000000
//##	06010040: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
//##	06010060: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
//##	06010080: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
//##	060100A0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
//##	060100C0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
//##	060100E0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

#if defined (HDMI_PHY)
static void h3_hdmi_phy_init(uint_fast32_t dotclock)
{
	HDMI_PHY_TypeDef * const phy = HDMI_PHY;

	/* enable read access to HDMI controller */
	phy->HDMI_PHY_READ_EN = 0x54524545;
	/* descramble register offsets */
	phy->HDMI_PHY_UNSCRAMBLE = 0x42494E47;

	//PRINTF("phy->HDMI_PHY_STS=%08X\n", (unsigned) phy->HDMI_PHY_STS);
	// HDMI PHY init, the following black magic is based on the procedure documented at:
	// http://linux-sunxi.org/images/3/38/AW_HDMI_TX_PHY_S40_Spec_V0.1.pdf
	phy->HDMI_PHY_CFG1 = 0;
	phy->HDMI_PHY_CFG1 = 1;
	local_delay_us(5);
	phy->HDMI_PHY_CFG1 |= (UINT32_C(1) << 16);
	phy->HDMI_PHY_CFG1 |= (UINT32_C(1) << 1);
	local_delay_us(10);
	phy->HDMI_PHY_CFG1 |= (UINT32_C(1) << 2);
	local_delay_us(5);
	phy->HDMI_PHY_CFG1 |= (UINT32_C(1) << 3);
	local_delay_us(40);
	phy->HDMI_PHY_CFG1 |= (UINT32_C(1) << 19);
	local_delay_us(100);
	phy->HDMI_PHY_CFG1 |= (UINT32_C(1) << 18);
	phy->HDMI_PHY_CFG1 |= (7 << 4);
	if (local_wait32mask(& phy->HDMI_PHY_STS, 0x80, 0x80, 100))
	{
		PRINTF("h3_hdmi_phy_init: timeout error\n");
	}
//	int z = 0;
//	while (z ++ < 10 && (phy->HDMI_PHY_STS & 0x80) == 0)
//		local_delay_ms(10);
	//PRINTF("phy->HDMI_PHY_STS=%08X\n", (unsigned) phy->HDMI_PHY_STS);

	phy->HDMI_PHY_CFG1 |= (0xf << 4);
	phy->HDMI_PHY_CFG1 |= (0xf << 8);
	phy->HDMI_PHY_CFG3 |= (UINT32_C(1) << 0) | (UINT32_C(1) << 2);

	phy->HDMI_PHY_PLL1 &= ~ (UINT32_C(1) << 26);
	phy->HDMI_PHY_CEC = 0;


	// 148.5 MHz
	uint_fast32_t PHY_PLL1_VAL = (0x3ddc5040 & ~ ((UINT32_C(1) << 25)) & ~ ((UINT32_C(1) << 26)));
	uint_fast32_t PHY_PLL2_VAL = 0x80084381;
	uint_fast32_t PHY_PLL3_VAL = 0x00000001;

	uint_fast32_t PHY_CFG1_VAL = 0x01FFFF7F;
	uint_fast32_t PHY_CFG2_VAL = 0x8063A800;
	uint_fast32_t PHY_CFG3_VAL = 0x0F81C485;

	uint_fast32_t tmp_rcal_200 = 0;

	switch (dotclock)
	{
	default:
	case 148500000:
		// 148.5 MHz
		PHY_PLL1_VAL = (0x3ddc5040 & ~ ((UINT32_C(1) << 25)) & ~ ((UINT32_C(1) << 26)));
		PHY_PLL2_VAL = 0x80084381;
		PHY_PLL3_VAL = 0x00000001;

		PHY_CFG1_VAL = 0x01FFFF7F;
		PHY_CFG2_VAL = 0x8063A800;
		PHY_CFG3_VAL = 0x0F81C485;
		break;

	case 74250000:
		// 74.25 MHz
		PHY_PLL1_VAL = 0x3ddc5040;
		PHY_PLL2_VAL = 0x80084343;
		PHY_PLL3_VAL = 0x00000001;

		tmp_rcal_200 = 0;
		PHY_CFG1_VAL = 0x01FFFF7F;
		PHY_CFG2_VAL = 0x80623000 | tmp_rcal_200;
		PHY_CFG3_VAL = 0x0F814385;
		break;
	}

	switch (dotclock)
	{
	default:
		PRINTF("Unspecified dot clock %u Hz\n", (unsigned) dotclock);
		//break;
	case 74250000:
	case 148500000:
		phy->HDMI_PHY_PLL1 = PHY_PLL1_VAL;
		phy->HDMI_PHY_PLL2 = PHY_PLL2_VAL;
		local_delay_us(10000);
		phy->HDMI_PHY_PLL3 = PHY_PLL3_VAL;
		phy->HDMI_PHY_PLL1 |= (UINT32_C(1) << 25);

		local_delay_us(10000);
		{
			uint32_t tmp = (phy->HDMI_PHY_STS & 0x1F800) >> 11;
			phy->HDMI_PHY_PLL1 |= (UINT32_C(1) << 31) | (UINT32_C(1) << 30) | tmp;
		}

		phy->HDMI_PHY_CFG1 = PHY_CFG1_VAL;
		phy->HDMI_PHY_CFG2 = PHY_CFG2_VAL;
		phy->HDMI_PHY_CFG3 = PHY_CFG3_VAL;
		break;

//	case 297000000:
//
//		phy->HDMI_PHY_PLL1 = 0x3ddc5040;
//		phy->HDMI_PHY_PLL2 = 0x80084381;
//		phy->HDMI_PHY_PLL3 = 1;
//
//		phy->HDMI_PHY_CFG1 = 0x01FFFF7F;
//		phy->HDMI_PHY_CFG2 = 0x8063A800;
//		phy->HDMI_PHY_CFG3 = 0x0F81C485;
//		break;
//
//
//	case 27000000:
//		break;

	}
}
#endif /* defined (HDMI_PHY) */

static void t507_hdmi_phy_init(uint_fast32_t dotclock)
{
	HDMI_TX_TypeDef * const hdmi = HDMI_TX0;
	/* PHY_I2CM_SLAVE_ADDR field values */
//		HDMI_PHY_I2CM_SLAVE_ADDR_PHY_GEN2 = 0x69,
//		HDMI_PHY_I2CM_SLAVE_ADDR_HEAC_PHY = 0x49,
	int cscon = 0;//hdmi->sink_is_hdmi && is_color_space_conversion(hdmi);

	/* hdmi phy spec says to do the phy initialization sequence twice */
	int i;
	for (i = 0; i < 2; i++) {
		hdmi_phy_sel_data_en_pol(hdmi, 1);
		hdmi_phy_sel_interface_control(hdmi, 0);
		hdmi_phy_enable_tmds(hdmi, 0);
		hdmi_phy_enable_power(hdmi, 0);

		int ret = hdmi_phy_configure(hdmi, dotclock, 8, cscon);
//		if (ret) {
//			debug("hdmi phy config failure %d\n", ret);
//			return ret;
//		}
	}

}

static void h3_hdmi_init(const videomode_t * vdmode)
{
	HDMI_TX_TypeDef * const hdmi = HDMI_TX0;

	/* Accuhdmithis display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned HSYNC = vdmode->hsync;	/*  */
	const unsigned VSYNC = vdmode->vsync;	/*  */
	const unsigned HFP = vdmode->hfp;	/* horizontal front porch */
	const unsigned VFP = vdmode->vfp;	/* vertical front porch */
	const unsigned HBP = vdmode->hbp;	/* horizontal back porch */
	const unsigned VBP = vdmode->vbp;	/* vertical back porch */
	const unsigned LEFTMARGIN = HSYNC + HBP;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = VSYNC + VBP;	/* vertical delay before DE start */
	const unsigned HBLANKING = HBP + HSYNC +  HFP;	/* Horizontal Blanking = hsync + hbp + hfp */
	const unsigned VBLANKING = VBP + VSYNC +  VFP;	/* Vertical Blanking = vsync + vbp + vfp */
	const unsigned HTOTAL = HBLANKING + WIDTH;	/* horizontal full period */
	const unsigned VTOTAL = VBLANKING + HEIGHT;	/* vertical full period */

//	PRINTF("LEFTMARGIN=%u\n", LEFTMARGIN);
//	PRINTF("TOPMARGIN=%u\n", TOPMARGIN);
//	PRINTF("HTOTAL=%u\n", HTOTAL);
//	PRINTF("VTOTAL=%u\n", VTOTAL);
//	PRINTF("HBLANKING=%u\n", HBLANKING);
//	PRINTF("VBLANKING=%u\n", VBLANKING);
//	PRINTF("HSYNC=%u\n", HSYNC);
//	PRINTF("VSYNC=%u\n", VSYNC);
//	PRINTF("HFP=%u\n", HFP);
//	PRINTF("VFP=%u\n", VFP);
//
//	PRINTF("dot clock = %u\n", display_getdotclock(vdmode));
	// HDMI Config, based on the documentation at:
	// https://people.freebsd.org/~gonzo/arm/iMX6-HDMI.pdf
	hdmi->HDMI_FC_INVIDCONF = (UINT32_C(1) << 6) | (UINT32_C(1) << 5) | (UINT32_C(1) << 4) | (UINT32_C(1) << 3); // Polarity etc
	hdmi->HDMI_FC_INHACTV0 = (WIDTH & 0xff);    // Horizontal pixels
	hdmi->HDMI_FC_INHACTV1 = (WIDTH >> 8);      // Horizontal pixels
	hdmi->HDMI_FC_INHBLANK0 = (HBLANKING & 0xff);     // Horizontal blanking
	hdmi->HDMI_FC_INHBLANK1 = (HBLANKING >> 8);       // Horizontal blanking

	hdmi->HDMI_FC_INVACTV0 = (HEIGHT & 0xff);    // Vertical pixels
	hdmi->HDMI_FC_INVACTV1 = (HEIGHT >> 8);      // Vertical pixels
	hdmi->HDMI_FC_INVBLANK = VBLANKING;               // Vertical blanking

	hdmi->HDMI_FC_HSYNCINDELAY0 = (HFP & 0xff);
	hdmi->HDMI_FC_HSYNCINDELAY1 = (HFP >> 8);    // Horizontal Front porch
	hdmi->HDMI_FC_HSYNCINWIDTH0 = (HSYNC & 0xff);  // Horizontal sync pulse
	hdmi->HDMI_FC_HSYNCINWIDTH1 = (HSYNC >> 8);    // Horizontal sync pulse

	hdmi->HDMI_FC_VSYNCINDELAY = VFP;
	hdmi->HDMI_FC_VSYNCINWIDTH = VSYNC;            // Vertical sync pulse

//	hdmi->HDMI_FC_CTRLDUR = 12;   // Frame Composer Control Period Duration
//	hdmi->HDMI_FC_EXCTRLDUR = 32; // Frame Composer Extended Control Period Duration
//	hdmi->HDMI_FC_EXCTRLSPAC = 1; // Frame Composer Extended Control Period Maximum Spacing
//	hdmi->HDMI_FC_CH0PREAM = 0x0b; // Frame Composer Channel 0 Non-Preamble Data
//	hdmi->HDMI_FC_CH1PREAM = 0x16; // Frame Composer Channel 1 Non-Preamble Data
//	hdmi->HDMI_FC_CH2PREAM = 0x21; // Frame Composer Channel 2 Non-Preamble Data
//	hdmi->HDMI_MC_FLOWCTRL = HDMI_MC_FLOWCTRL_FEED_THROUGH_OFF_CSC_BYPASS;    // Main Controller Feed Through Control
//	hdmi->HDMI_MC_CLKDIS = 0x74; // Main Controller Synchronous Clock Domain Disable

}

enum dw_hdmi_phy_type {
	DW_HDMI_PHY_DWC_HDMI_TX_PHY = 0x00,
	DW_HDMI_PHY_DWC_MHL_PHY_HEAC = 0xb2,
	DW_HDMI_PHY_DWC_MHL_PHY = 0xc2,
	DW_HDMI_PHY_DWC_HDMI_3D_TX_PHY_HEAC = 0xe2,
	DW_HDMI_PHY_DWC_HDMI_3D_TX_PHY = 0xf2,
	DW_HDMI_PHY_DWC_HDMI20_TX_PHY = 0xf3,
	DW_HDMI_PHY_VENDOR_PHY = 0xfe,
};

static void hdmi_version(void)
{
	HDMI_TX_TypeDef * const hdmi = HDMI_TX0;

	// A733
	//	Detected HDMI controller 0x21:0x2a:0xa0:0xc1
	//	 Config 0x9f:0x62:0xfe:0x0
	//
	// T507-H
	//	Detected HDMI controller 0x21:0x2a:0xa0:0xc1
	//	 Config 0x9f:0x62:0xf3:0x0
	//
	// A64
	//	Detected HDMI controller 0x13:0x2a:0xa0:0xc1
	//	 Config 0xbf:0x2:0xfe:0x0

	// iMX6-HDMI (new).pdf
	// HDMI_CONFIG2_ID field descriptions
	//	Indicates the type of PHY interface selected:
	//	00h Legacy PHY (HDMI TX PHY)
	//	F2h PHY_Gen2 (HDMI 3D TX PHY)
	//	E2h PHY_Gen2 (HDMI 3D TX PHY) + HEAC PHY


	PRINTF("Detected HDMI controller 0x%x:0x%x:0x%x:0x%x\n",
			hdmi->HDMI_DESIGN_ID,
			hdmi->HDMI_REVISION_ID,
			hdmi->HDMI_PRODUCT_ID0,
			hdmi->HDMI_PRODUCT_ID1
			);

	PRINTF(" Config 0x%x:0x%x:0x%x:0x%x\n",
			hdmi->HDMI_CONFIG0_ID,
			hdmi->HDMI_CONFIG1_ID,
			hdmi->HDMI_CONFIG2_ID,
			hdmi->HDMI_CONFIG3_ID
			);

}
#endif /* defined (HDMI_TX0) && WITHHDMITVHW */

#if WITHHDMITVHW



void awxxx_hdmi_init(const videomode_t * vdmode, uint_fast32_t dotclock)
{
	HDMI_TX_TypeDef * const hdmi = HDMI_TX0;

	hdmi_version();

//	switch (hdmi->HDMI_CONFIG2_ID)
//	{
//	default:
//		break;
//	}

#if CPUSTYLE_T507 || CPUSTYLE_A733
	t507_hdmi_phy_init(dotclock);
#else
	h3_hdmi_phy_init(dotclock);
#endif


	static const unsigned int n_table[21] = {
		32000, 3072, 4096, 44100, 4704, 6272, 88200, 4704 * 2, 6272 * 2,
		176400, 4704 * 4, 6272 * 4, 48000, 5120, 6144, 96000, 5120 * 2,
		6144 * 2, 192000, 5120 * 4, 6144 * 4,
	};

//	PRINTF("hdmi->HDMI_CONFIG3_ID=%02X, HDMI_CONFIG3_AHBAUDDMA=%02X\n", hdmi->HDMI_CONFIG3_ID, HDMI_CONFIG3_AHBAUDDMA);
//	PRINTF("hdmi->HDMI_CONFIG0_ID=%02X, HDMI_CONFIG0_I2S=%02X\n", hdmi->HDMI_CONFIG0_ID, HDMI_CONFIG0_I2S);

//	hdmi_write(0xE04B, 0x00 | (audio->sample_bit == 16)
//			? 0x02 : ((audio->sample_bit == 24) ? 0xb : 0x0));
//	hdmi_write(0x0251, audio->sample_bit);
//	n = 6272;
//
//	for (i = 0; i < 21; i += 3) {
//		if (audio->sample_rate == n_table[i]) {
//			if (ptbl[id].para[1] == 1)
//				n = n_table[i + 1];
//			else
//				n = n_table[i + 2];
///* cts = (n / 128) * (glb_video.tmds_clk / 100) / (audio->sample_rate / 100); */
//			    break;
//		}
//	}
	/* cts = (n / 128) * (glb_video.tmds_clk / 100) / (audio->sample_rate / 100); */
	unsigned audio_cts = 10;
	const int_fast32_t samplerate = dsp_get_sampleraterx();
	unsigned audio_n = hdmi_compute_n(samplerate, dotclock);
	/*
	 * Compute the CTS value from the N value.  Note that CTS and N
	 * can be up to 20 bits in total, so we need 64-bit math.  Also
	 * note that our TDMS clock is not fully accurate; it is accurate
	 * to kHz.  This can introduce an unnecessary remainder in the
	 * calculation below, so we don't try to warn about that.
	 */
	uint_fast64_t tmp = (uint_fast64_t)dotclock /*ftdms*/ * audio_n;
	tmp /= 128 * samplerate;
	audio_cts = tmp;
	// hdmi audio CTS=74250, N=6144
	//PRINTF("hdmi audio CTS=%u, N=%u\n", audio_cts, audio_n);

	h3_hdmi_init(vdmode);
	hdmi_enable_video_path(hdmi, 1);
	hdmi_video_sample(hdmi);
	hdmi_tx_hdcp_config(hdmi);
	// audio enable
	hdmi_set_cts_n(hdmi, audio_cts, audio_n);
	hdmi_enable_audio_clk(hdmi);
	dw_hdmi_i2s_setup(hdmi);
	dw_hdmi_clear_overflow(hdmi);

	hdmi_version();

	//	t507_hdmi_edid_test();

}
#endif /* WITHHDMITVHW */
