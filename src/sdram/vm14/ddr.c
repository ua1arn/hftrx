#include "hardware.h"

#if WITHSDRAMHW
#if CPUSTYLE_VM14

#include "formats.h"
#include "clocks.h"

////////////////////////
///
#include "ddr.h"
#include "ddr_reg_fields.h"
#include "ddr_regs.h"

#define BOOTROM_LOADED_ADDRESS 0x00000000 // May be 0x30000000 or 0x00000000
#define _AOPI_DDR_INIT(a, b, c)                                                \
    (((int (*)(unsigned int, void *, void *))(BOOTROM_LOADED_ADDRESS + 0x60))( \
        a, b, c)) /*offset 0x60*/

#if !(ROM_DDR_INIT)
void lpddr2_pub_load(lpddr2_t *lpddr2_mem, ddrphy_t *pub_regs);
void ddr3_pub_load(ddr3_t *ddr3_mem, ddrphy_t *pub_regs);
void umctl2_load(ddr_common_t *ddr_mem_cfg, dwc_umctl2_regs_t *umctl2_regs);
void init_start(dwc_umctl2_regs_t *umctl2_regs, ddrphy_t *pub_regs,
                unsigned int base_addr);
int pub_init_cmpl_wait(ddrphy_t *pub_regs, unsigned reset_type, unsigned check);
void umctl2_norm_wait(dwc_umctl2_regs_t *umctl2_regs);
void ddr_self_refresh_exit(dwc_umctl2_regs_t *umctl2_regs);
void ddr_self_refresh_enter(dwc_umctl2_regs_t *umctl2_regs);
void phy_train_save(ddrphy_t *pub_regs, ddr_common_t *ddr_mem_cfg);
unsigned int check_both_ctl(unsigned int *ctl_enabled);
int umctl2_init_common(ddr_common_t **mem_cfg, dwc_umctl2_regs_t **umctl2_regs,
                       ddrphy_t **pub_regs, sys_t *sys,
                       const unsigned reset_type);
int testmem_ddr(unsigned int base_addr);
int DDR_INIT(unsigned int reset_type, void *mem_cfg_0, void *mem_cfg_1);
#endif

#define MHZ_TO_SEL(MHz) (((MHz) / 24) - 1)

#define DDR0_BA (0x40000000)
#define DDR1_BA (0xA0000000)

#define DDRMC0_BASE 0x37204000
#define DDRMC0(a) (*(volatile uint32_t *)(DDRMC0_BASE + (a)))
#define DDRMC1_BASE 0x37206000
#define DDRMC1(a) (*(volatile uint32_t *)(DDRMC1_BASE + (a)))
#define DDRMC0_PHY_BASE 0x37205000
#define DDRMC0_PHY(a) (*(volatile uint32_t *)(DDRMC0_PHY_BASE + (a)))
#define DDRMC1_PHY_BASE 0x37207000
#define DDRMC1_PHY(a) (*(volatile uint32_t *)(DDRMC1_PHY_BASE + (a)))

//#define CMCTR_BASE 0x38094000
#define DIV_DDR0_CTR_REG (CMCTR->DIV_DDR0_CTR) //(*(volatile unsigned int *)(CMCTR_BASE + 0x030))
#define DIV_DDR1_CTR_REG (CMCTR->DIV_DDR1_CTR) //(*(volatile unsigned int *)(CMCTR_BASE + 0x034))
#define GATE_CORE_CTR_REG (CMCTR->GATE_CORE_CTR) //(*(volatile unsigned int *)(CMCTR_BASE + 0x048))
#define GPU_EN (UINT32_C(1) << 6)
#define VPU_EN (UINT32_C(1) << 5)
#define VPOUT_EN (UINT32_C(1) << 4)
#define VPIN_EN (UINT32_C(1) << 3)
#define DDR1_EN (UINT32_C(1) << 2)
#define DDR0_EN (UINT32_C(1) << 1)
#define L0_EN (UINT32_C(1) << 0)
//#define SEL_CPLL_REG (*(volatile unsigned int *)(CMCTR_BASE + 0x104))
#define PLL_LOCK_BIT (UINT32_C(1) << 31)

//#define PMCTR_BASE 0x38095000
//#define SMCTR_BASE 0x38096000

#define ddr_max(x, y)                                                              \
    ({                                                                         \
        typeof(x) _max1 = (x);                                                 \
        typeof(y) _max2 = (y);                                                 \
        (void)(&_max1 == &_max2);                                              \
        _max1 > _max2 ? _max1 : _max2;                                         \
    })


#define CPLL_VALUE ((CMCTR->SEL_CPLL) & 0xFF)
#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))
#define tCK DIV_ROUND_UP(1000000, ((CPLL_VALUE + 1) * 24))
#define bl 8                        /* burst length. Units: clock cycles */
#define cl 7                        /* read latency. Units: clocks */
#define cwl 6                       /* write latency. Units: clock cycles */
#define tWR 15000                   /* Units: ps */
#define tFAW 30000                  /* Units: ps */
#define tRAS_min 35000              /* Units: ps */
#define tRAS_max 9 * 7800000        /* Units: ps */
#define tRC 48750                   /* Units: ps */
#define tXPDLL ddr_max(UINT32_C(10) * tCK, UINT32_C(24000)) /* Units: ps */
#define tAL 0
#define tRTP ddr_max(UINT32_C(4) * tCK, UINT32_C(7500)) /* Units: ps */
#define tWTR ddr_max(UINT32_C(4) * tCK, UINT32_C(7500)) /* Units: ps */
#define tRCD 13750              /* Units: ps */
#define tRRD ddr_max(UINT32_C(4) * tCK, UINT32_C(6000)) /* Units: ps */
#define tCCD 4                  /* Units: clock cycles */
#define tRP 13750               /* Units: ps */

static ddr3_t nt5cb256m8fn_di_cfg;

void set_ddr_freq(unsigned int MHz);

static void set_nt5cb256m8fn_di_cfg(void)
{
    nt5cb256m8fn_di_cfg.misc0.full_reg = 0;
    nt5cb256m8fn_di_cfg.misc0.burst_rdwr = 4;
    nt5cb256m8fn_di_cfg.misc0.ddr3_type = 1;
    nt5cb256m8fn_di_cfg.misc0.lpddr2_type = 0;
    nt5cb256m8fn_di_cfg.misc0.active_ranks = 1;
    nt5cb256m8fn_di_cfg.misc0.port_en_0 = 1;
    nt5cb256m8fn_di_cfg.misc0.port_en_1 = 1;
    nt5cb256m8fn_di_cfg.misc0.port_en_2 = 1;

    nt5cb256m8fn_di_cfg.dram_tmg_0.full_reg = 0;
    nt5cb256m8fn_di_cfg.dram_tmg_0.wr2pre =
        cwl + (bl / 2) + DIV_ROUND_UP(tWR, tCK);
    nt5cb256m8fn_di_cfg.dram_tmg_0.t_faw = DIV_ROUND_UP(tFAW, tCK);
    nt5cb256m8fn_di_cfg.dram_tmg_0.t_ras_max = tRAS_max / (tCK * 1024);
    nt5cb256m8fn_di_cfg.dram_tmg_0.t_ras_min = DIV_ROUND_UP(tRAS_min, tCK);
    nt5cb256m8fn_di_cfg.dram_tmg_0.trc = DIV_ROUND_UP(tRC, tCK);

    nt5cb256m8fn_di_cfg.dram_tmg_1.full_reg = 0;
    nt5cb256m8fn_di_cfg.dram_tmg_1.t_xp = DIV_ROUND_UP(tXPDLL, tCK);
    nt5cb256m8fn_di_cfg.dram_tmg_1.rd2pre = tAL + DIV_ROUND_UP(tRTP, tCK);
    nt5cb256m8fn_di_cfg.dram_tmg_1.read_latency = cl;
    nt5cb256m8fn_di_cfg.dram_tmg_1.rd2wr = cl + (bl / 2) + 2 - cwl;
    nt5cb256m8fn_di_cfg.dram_tmg_1.wr2rd =
        cwl + (bl / 2) + DIV_ROUND_UP(tWTR, tCK);

    nt5cb256m8fn_di_cfg.dram_tmg_2.full_reg = 0;
    nt5cb256m8fn_di_cfg.dram_tmg_2.write_latency = cwl;
    nt5cb256m8fn_di_cfg.dram_tmg_2.t_rcd = DIV_ROUND_UP(tRCD, tCK) - tAL;
    nt5cb256m8fn_di_cfg.dram_tmg_2.t_rrd = DIV_ROUND_UP(tRRD, tCK);
    nt5cb256m8fn_di_cfg.dram_tmg_2.t_ccd = tCCD;
    nt5cb256m8fn_di_cfg.dram_tmg_2.t_rp = DIV_ROUND_UP(tRP, tCK);

    nt5cb256m8fn_di_cfg.refr_cnt.full_reg = 0;
    nt5cb256m8fn_di_cfg.refr_cnt.t_rfc_min = DIV_ROUND_UP(160000, tCK);

    nt5cb256m8fn_di_cfg.dfi_tmg.full_reg = 0;
    nt5cb256m8fn_di_cfg.dfi_tmg.dfi_t_ctrl_delay = 2;
    nt5cb256m8fn_di_cfg.dfi_tmg.dfi_tphy_wrlat = cwl - 1;
    nt5cb256m8fn_di_cfg.dfi_tmg.dfi_t_rddata_en = cl - 2;
    nt5cb256m8fn_di_cfg.dfi_tmg.dfi_tphy_wrdata = 1;

    nt5cb256m8fn_di_cfg.addrmap0.full_reg = 0;
    nt5cb256m8fn_di_cfg.addrmap0.addrmap_bank_b0 = 7;
    nt5cb256m8fn_di_cfg.addrmap0.addrmap_bank_b1 = 7;
    nt5cb256m8fn_di_cfg.addrmap0.addrmap_bank_b2 = 7;
    nt5cb256m8fn_di_cfg.addrmap0.addrmap_col_b2 = 0;
    nt5cb256m8fn_di_cfg.addrmap0.addrmap_col_b3 = 0;
    nt5cb256m8fn_di_cfg.addrmap0.addrmap_col_b4 = 0;
    nt5cb256m8fn_di_cfg.addrmap0.addrmap_col_b5 = 0;

    nt5cb256m8fn_di_cfg.addrmap1.full_reg = 0;
    nt5cb256m8fn_di_cfg.addrmap1.addrmap_col_b6 = 0;
    nt5cb256m8fn_di_cfg.addrmap1.addrmap_cs_bit0 = 31;
    nt5cb256m8fn_di_cfg.addrmap1.addrmap_col_b7 = 0;
    nt5cb256m8fn_di_cfg.addrmap1.addrmap_col_b8 = 0;
    nt5cb256m8fn_di_cfg.addrmap1.addrmap_col_b9 = 15;
    nt5cb256m8fn_di_cfg.addrmap1.addrmap_col_b10 = 15;
    nt5cb256m8fn_di_cfg.addrmap1.addrmap_col_b11 = 15;

    nt5cb256m8fn_di_cfg.addrmap2.full_reg = 0;
    nt5cb256m8fn_di_cfg.addrmap2.addrmap_row_b0 = 6;
    nt5cb256m8fn_di_cfg.addrmap2.addrmap_row_b1 = 6;
    nt5cb256m8fn_di_cfg.addrmap2.addrmap_row_b2_10 = 6;
    nt5cb256m8fn_di_cfg.addrmap2.addrmap_row_b11 = 6;
    nt5cb256m8fn_di_cfg.addrmap2.addrmap_row_b12 = 6;
    nt5cb256m8fn_di_cfg.addrmap2.addrmap_row_b13 = 6;
    nt5cb256m8fn_di_cfg.addrmap2.addrmap_row_b14 = 6;
    nt5cb256m8fn_di_cfg.addrmap2.addrmap_row_b15 = 15;

    nt5cb256m8fn_di_cfg.misc1.full_reg = 0;

    nt5cb256m8fn_di_cfg.misc2.full_reg = 0;
    nt5cb256m8fn_di_cfg.misc2.dtpr_twtr = DIV_ROUND_UP(tWTR, tCK);
    nt5cb256m8fn_di_cfg.misc2.dtpr_trtp = DIV_ROUND_UP(tRTP, tCK);
    nt5cb256m8fn_di_cfg.misc2.dtpr_trtw = 0;

    nt5cb256m8fn_di_cfg.misc3.full_reg = 0;

    nt5cb256m8fn_di_cfg.misc4.full_reg = 0;
    nt5cb256m8fn_di_cfg.misc4.sel_cpll = CPLL_VALUE;
    nt5cb256m8fn_di_cfg.misc4.ddr_div = 0;
    nt5cb256m8fn_di_cfg.misc4.pgcr_ranken = 1;
    nt5cb256m8fn_di_cfg.misc4.pgcr_rfshdt = 1;
    nt5cb256m8fn_di_cfg.misc4.ddr_remap = 0;

    nt5cb256m8fn_di_cfg.misc5.full_reg = 0;
    nt5cb256m8fn_di_cfg.misc5.dfi_upd_int_min = 12;
    nt5cb256m8fn_di_cfg.misc5.dfi_upd_int_max = 64;

    nt5cb256m8fn_di_cfg.config_0.full_reg = 0;
    nt5cb256m8fn_di_cfg.config_0.phy_mr_cl = 3;
    nt5cb256m8fn_di_cfg.config_0.phy_mr_bl = 0;
    nt5cb256m8fn_di_cfg.config_0.phy_mr_wr = 4;
    nt5cb256m8fn_di_cfg.config_0.phy_mr_cwl = 1;

    nt5cb256m8fn_di_cfg.config_1.full_reg = 0;
    nt5cb256m8fn_di_cfg.config_1.dtpr_tmrd = 0;
    nt5cb256m8fn_di_cfg.config_1.dtpr_tmod = 0;
}
void set_ddr_freq(unsigned int MHz)
{
    // Set divisors
    DIV_DDR0_CTR_REG = 0;
    DIV_DDR1_CTR_REG = 0;

    // Enable DDR CLK
    GATE_CORE_CTR_REG |= DDR1_EN | DDR0_EN;

    // Set CPLL
    (CMCTR->SEL_CPLL) = MHZ_TO_SEL(MHz);
    while (!((CMCTR->SEL_CPLL) & PLL_LOCK_BIT))
        ;
}
int init_ddr3(unsigned int MHz, int bDDRMC0, int bDDRMC1)
{
    int i;
    ddrmc_t *DDRMC[2] = {(ddrmc_t *)DDRMC0_BASE, (ddrmc_t *)DDRMC1_BASE};
    ddrphy_t *DDRPHY[2] = {(ddrphy_t *)DDRMC0_PHY_BASE,
                           (ddrphy_t *)DDRMC1_PHY_BASE};

    set_ddr_freq(MHz);
    set_nt5cb256m8fn_di_cfg();

    for (i = 0; i < 2; i++) {
        DDRMC[i]->RFSHTMG = (DDRMC[i]->RFSHTMG & ~0x1ff) |
                            nt5cb256m8fn_di_cfg.refr_cnt.t_rfc_min;

        DDRPHY[i]->DTPR0 = (DDRPHY[i]->DTPR0 & ~0x1f0000) |
                           (nt5cb256m8fn_di_cfg.dram_tmg_0.t_ras_min << 16);
    }
#if ROM_DDR_INIT
    return _api_DDR_INIT(1, bDDRMC0 ? &nt5cb256m8fn_di_cfg : 0,
                         bDDRMC1 ? &nt5cb256m8fn_di_cfg : 0);
#else
    return DDR_INIT(1, bDDRMC0 ? &nt5cb256m8fn_di_cfg : 0,
                    bDDRMC1 ? &nt5cb256m8fn_di_cfg : 0);
#endif
}
#if !(ROM_DDR_INIT)

int DDR_INIT(unsigned int reset_type, void *mem_cfg_0, void *mem_cfg_1)
{

    ddr_common_t *mem_cfg[2];
    mem_cfg[0] = (ddr_common_t *)(mem_cfg_0);
    mem_cfg[1] = (ddr_common_t *)(mem_cfg_1);

    dwc_umctl2_regs_t *umctl2_regs[2];
    umctl2_regs[0] = (dwc_umctl2_regs_t *)(DDRMC0_BASE);
    umctl2_regs[1] = (dwc_umctl2_regs_t *)(DDRMC1_BASE);

    ddrphy_t *pub_regs[2];
    pub_regs[0] = (ddrphy_t *)(DDRMC0_PHY_BASE);
    pub_regs[1] = (ddrphy_t *)(DDRMC1_PHY_BASE);

    sys_t sys;

    sys.DDR_REMAP = (unsigned int *)(SMCTR_BASE + 0xc);
    sys.BASE_ADR[0] = DDR0_BA;
    sys.BASE_ADR[1] = DDR1_BA;

    while ((PMCTR->CORE_PWR_STATUS & 1)) {
    }

    return umctl2_init_common(&mem_cfg[0], &umctl2_regs[0], &pub_regs[0], &sys,
                              reset_type);
}

int umctl2_init_common(ddr_common_t **mem_cfg, dwc_umctl2_regs_t **umctl2_regs,
                       ddrphy_t **pub_regs, sys_t *sys,
                       const unsigned reset_type)
{

    unsigned int j, postcode, ddr_ret;
    unsigned int ctl_enabled[2] = {(mem_cfg[0]) ? 1 : 0, (mem_cfg[1]) ? 1 : 0};

    if (!check_both_ctl(&ctl_enabled[0]))
        return (POST_DDR_CFG_POINTER_NULL) | (POST_DDR_CFG_POINTER_NULL << 16);

    postcode = 0;

    ddr_ret = PMCTR->DDR_PIN_RET;

    if (ctl_enabled[0])
        ddr_ret = ddr_ret & (2);
    if (ctl_enabled[1])
        ddr_ret = ddr_ret & (1);
    PMCTR->DDR_PIN_RET = ddr_ret;

    for (j = 0; j < 2; j++) {
        if (!ctl_enabled[j]) {
            // postcode = POST_DDR_CFG_POINTER_NULL << (j*16);
            continue;
        }

        CMCTR->GATE_CORE_CTR = CMCTR->GATE_CORE_CTR | (UINT32_C(1) << (j + 1));

        umctl2_regs[j]->DFIMISC = SET_SNPS_DDR_DFIMISC_DFI_INIT_COMPLETE_EN(
            umctl2_regs[j]->DFIMISC, 0);
        umctl2_load(mem_cfg[j], umctl2_regs[j]);
    }

    PMCTR->DDR_INIT_END = 0x1;

    for (j = 0; j < 2; j++) {
        if (ctl_enabled[j]) {
            if (mem_cfg[j]->misc0.ddr3_type)
                ddr3_pub_load((ddr3_t *)(mem_cfg[j]), pub_regs[j]);
            if (mem_cfg[j]->misc0.lpddr2_type)
                lpddr2_pub_load((lpddr2_t *)(mem_cfg[j]), pub_regs[j]);
            init_start(umctl2_regs[j], pub_regs[j], sys->BASE_ADR[j]);
        }
    }

    for (j = 0; j < 2; j++) {
        if (ctl_enabled[j]) {

            if (pub_init_cmpl_wait(pub_regs[j], reset_type, 0)) {
                ctl_enabled[j] = 0;
                postcode = postcode | (POST_DDR_PHY_INIT_FAILED << (j * 16));
            } else {
                umctl2_regs[j]->INIT0 = SET_SNPS_DDR_INIT0_SKIP_DRAM_INIT(
                    umctl2_regs[j]->INIT0, 0x1);
                umctl2_regs[j]->DFIMISC =
                    SNPS_DDR_DFIMISC_DFI_INIT_COMPLETE_EN(1);
                umctl2_norm_wait(umctl2_regs[j]);
            }
        }
    }

    if (!check_both_ctl(&ctl_enabled[0]))
        return postcode;

    for (j = 0; j < 2; j++) {
        if (ctl_enabled[j])
            phy_train_save(pub_regs[j], mem_cfg[j]);
    }

    for (j = 0; j < 2; j++) {
        if (ctl_enabled[j]) {
            umctl2_regs[j]->PCTRL_0 = mem_cfg[j]->misc0.port_en_0;
            umctl2_regs[j]->PCTRL1 = mem_cfg[j]->misc0.port_en_1;
            umctl2_regs[j]->PCTRL2 = mem_cfg[j]->misc0.port_en_2;
        }
    }

    for (j = 0; j < 2; j++) {
        if (ctl_enabled[j]) {
            if (testmem_ddr(sys->BASE_ADR[j])) {
                postcode = postcode | (POST_DDR_TESTMEM_FAILED << (j * 16));
            }
        }
    }

    return postcode;
}

void umctl2_load(ddr_common_t *ddr_mem_cfg, dwc_umctl2_regs_t *umctl2_regs)
{

    umctl2_regs->MSTR =
        (SNPS_DDR_MSTR_BURST_RDWR(ddr_mem_cfg->misc0.burst_rdwr) |
         SNPS_DDR_MSTR_DDR3(ddr_mem_cfg->misc0.ddr3_type) |
         SNPS_DDR_MSTR_LPDDR2(ddr_mem_cfg->misc0.lpddr2_type) |
         SNPS_DDR_MSTR_ACTIVE_RANKS(ddr_mem_cfg->misc0.active_ranks));

    umctl2_regs->DRAMTMG0 =
        (SNPS_DDR_DRAMTMG0_WR2PRE(ddr_mem_cfg->dram_tmg_0.wr2pre) |
         SNPS_DDR_DRAMTMG0_T_FAW(ddr_mem_cfg->dram_tmg_0.t_faw) |
         SNPS_DDR_DRAMTMG0_T_RAS_MAX(ddr_mem_cfg->dram_tmg_0.t_ras_max) |
         SNPS_DDR_DRAMTMG0_T_RAS_MIN(ddr_mem_cfg->dram_tmg_0.t_ras_min));

    umctl2_regs->DRAMTMG1 =
        (SNPS_DDR_DRAMTMG1_T_XP(ddr_mem_cfg->dram_tmg_1.t_xp) |
         SNPS_DDR_DRAMTMG1_RD2PRE(ddr_mem_cfg->dram_tmg_1.rd2pre) |
         SNPS_DDR_DRAMTMG1_T_RC(ddr_mem_cfg->dram_tmg_0.trc));

    umctl2_regs->DRAMTMG2 =
        (SNPS_DDR_DRAMTMG2_WRITE_LATENCY(
             ddr_mem_cfg->dram_tmg_2.write_latency) |
         SNPS_DDR_DRAMTMG2_READ_LATENCY(ddr_mem_cfg->dram_tmg_1.read_latency) |
         SNPS_DDR_DRAMTMG2_RD2WR(ddr_mem_cfg->dram_tmg_1.rd2wr) |
         SNPS_DDR_DRAMTMG2_WR2RD(ddr_mem_cfg->dram_tmg_1.wr2rd));

    umctl2_regs->DRAMTMG4 =
        (SNPS_DDR_DRAMTMG4_T_RCD(ddr_mem_cfg->dram_tmg_2.t_rcd) |
         SNPS_DDR_DRAMTMG4_T_RRD(ddr_mem_cfg->dram_tmg_2.t_rrd) |
         SNPS_DDR_DRAMTMG4_T_CCD(ddr_mem_cfg->dram_tmg_2.t_ccd) |
         SNPS_DDR_DRAMTMG4_T_RP(ddr_mem_cfg->dram_tmg_2.t_rp));

    umctl2_regs->DFITMG0 =
        (SNPS_DDR_DFITMG0_DFI_T_CTRL_DELAY(
             ddr_mem_cfg->dfi_tmg.dfi_t_ctrl_delay) |
         SNPS_DDR_DFITMG0_DFI_TPHY_WRLAT(ddr_mem_cfg->dfi_tmg.dfi_tphy_wrlat) |
         SNPS_DDR_DFITMG0_DFI_T_RDDATA_EN(
             ddr_mem_cfg->dfi_tmg.dfi_t_rddata_en) |
         SNPS_DDR_DFITMG0_DFI_TPHY_WRDATA(
             ddr_mem_cfg->dfi_tmg.dfi_tphy_wrdata));

    umctl2_regs->ADDRMAP0 = SET_SNPS_DDR_ADDRMAP0_ADDRMAP_CS_BIT0(
        umctl2_regs->ADDRMAP0, ddr_mem_cfg->addrmap1.addrmap_cs_bit0);

    umctl2_regs->ADDRMAP1 = (SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B0(
                                 ddr_mem_cfg->addrmap0.addrmap_bank_b0) |
                             SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B1(
                                 ddr_mem_cfg->addrmap0.addrmap_bank_b1) |
                             SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B2(
                                 ddr_mem_cfg->addrmap0.addrmap_bank_b2));

    umctl2_regs->ADDRMAP2 = (SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B2(
                                 ddr_mem_cfg->addrmap0.addrmap_col_b2) |
                             SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B3(
                                 ddr_mem_cfg->addrmap0.addrmap_col_b3) |
                             SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B4(
                                 ddr_mem_cfg->addrmap0.addrmap_col_b4) |
                             SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B5(
                                 ddr_mem_cfg->addrmap0.addrmap_col_b5));

    umctl2_regs->ADDRMAP3 = (SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B6(
                                 ddr_mem_cfg->addrmap1.addrmap_col_b6) |
                             SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B7(
                                 ddr_mem_cfg->addrmap1.addrmap_col_b7) |
                             SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B8(
                                 ddr_mem_cfg->addrmap1.addrmap_col_b8) |
                             SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B9(
                                 ddr_mem_cfg->addrmap1.addrmap_col_b9));

    umctl2_regs->ADDRMAP4 = (SNPS_DDR_ADDRMAP4_ADDRMAP_COL_B10(
                                 ddr_mem_cfg->addrmap1.addrmap_col_b10) |
                             SNPS_DDR_ADDRMAP4_ADDRMAP_COL_B11(
                                 ddr_mem_cfg->addrmap1.addrmap_col_b11));

    umctl2_regs->ADDRMAP5 = (SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B0(
                                 ddr_mem_cfg->addrmap2.addrmap_row_b0) |
                             SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B1(
                                 ddr_mem_cfg->addrmap2.addrmap_row_b1) |
                             SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B2_10(
                                 ddr_mem_cfg->addrmap2.addrmap_row_b2_10) |
                             SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B11(
                                 ddr_mem_cfg->addrmap2.addrmap_row_b11));

    umctl2_regs->ADDRMAP6 = (SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B12(
                                 ddr_mem_cfg->addrmap2.addrmap_row_b12) |
                             SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B13(
                                 ddr_mem_cfg->addrmap2.addrmap_row_b13) |
                             SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B14(
                                 ddr_mem_cfg->addrmap2.addrmap_row_b14) |
                             SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B15(
                                 ddr_mem_cfg->addrmap2.addrmap_row_b15));

    umctl2_regs->DFIUPD1 = (SNPS_DDR_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MIN(
                                ddr_mem_cfg->misc5.dfi_upd_int_min) |
                            SNPS_DDR_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MAX(
                                ddr_mem_cfg->misc5.dfi_upd_int_max));
}

void ddr3_pub_load(ddr3_t *ddr3_mem, ddrphy_t *pub_regs)
{
    pub_regs->MR0 =
        SET_SNPS_DDR_PUB_MR0_CL(pub_regs->MR0, ddr3_mem->config_0.phy_mr_cl);
    pub_regs->MR0 =
        SET_SNPS_DDR_PUB_MR0_BL(pub_regs->MR0, ddr3_mem->config_0.phy_mr_bl);
    pub_regs->MR0 =
        SET_SNPS_DDR_PUB_MR0_WR(pub_regs->MR0, ddr3_mem->config_0.phy_mr_wr);

    pub_regs->MR2 =
        SET_SNPS_DDR_PUB_MR2_CWL(pub_regs->MR2, ddr3_mem->config_0.phy_mr_cwl);

    pub_regs->DTPR0 = SET_SNPS_DDR_PUB_DTPR0_TRCD(pub_regs->DTPR0,
                                                  ddr3_mem->dram_tmg_2.t_rcd);
    pub_regs->DTPR0 =
        SET_SNPS_DDR_PUB_DTPR0_TWTR(pub_regs->DTPR0, ddr3_mem->misc2.dtpr_twtr);
    pub_regs->DTPR0 = SET_SNPS_DDR_PUB_DTPR0_TMRD(pub_regs->DTPR0,
                                                  ddr3_mem->config_1.dtpr_tmrd);
    pub_regs->DTPR0 =
        SET_SNPS_DDR_PUB_DTPR0_TRTP(pub_regs->DTPR0, ddr3_mem->misc2.dtpr_trtp);
    pub_regs->DTPR0 =
        SET_SNPS_DDR_PUB_DTPR0_TRC(pub_regs->DTPR0, ddr3_mem->dram_tmg_0.trc);
    pub_regs->DTPR0 = SET_SNPS_DDR_PUB_DTPR0_TRRD(pub_regs->DTPR0,
                                                  ddr3_mem->dram_tmg_2.t_rrd);
    pub_regs->DTPR0 =
        SET_SNPS_DDR_PUB_DTPR0_TRP(pub_regs->DTPR0, ddr3_mem->dram_tmg_2.t_rp);

    pub_regs->DTPR1 = SET_SNPS_DDR_PUB_DTPR1_TFAW(pub_regs->DTPR1,
                                                  ddr3_mem->dram_tmg_0.t_faw);
    pub_regs->DTPR1 =
        SET_SNPS_DDR_PUB_DTPR1_TRTW(pub_regs->DTPR1, ddr3_mem->misc2.dtpr_trtw);
    pub_regs->DTPR1 = SET_SNPS_DDR_PUB_DTPR1_TMOD(pub_regs->DTPR1,
                                                  ddr3_mem->config_1.dtpr_tmod);
    pub_regs->DTPR1 = SET_SNPS_DDR_PUB_DTPR1_TRFC(pub_regs->DTPR1,
                                                  ddr3_mem->refr_cnt.t_rfc_min);

    pub_regs->PGCR = SET_SNPS_DDR_PUB_PGCR_DQSCFG(pub_regs->PGCR, 1);
    pub_regs->PGCR = SET_SNPS_DDR_PUB_PGCR_RFSHDT(pub_regs->PGCR,
                                                  ddr3_mem->misc4.pgcr_rfshdt);
    pub_regs->PGCR = SET_SNPS_DDR_PUB_PGCR_RANKEN(pub_regs->PGCR,
                                                  ddr3_mem->misc4.pgcr_ranken);

    pub_regs->DXCCR = SET_SNPS_DDR_PUB_DXCCR_AWDT(pub_regs->DXCCR, 0x0);
}

void lpddr2_pub_load(lpddr2_t *lpddr2_mem, ddrphy_t *pub_regs)
{
    pub_regs->PTR0 =
        (SNPS_DDR_PUB_PTR0_TDLLSRST(lpddr2_mem->misc1.ptr_tdllrst) |
         SNPS_DDR_PUB_PTR0_TDLLLOCK(lpddr2_mem->misc1.ptr_tdlllock) |
         SNPS_DDR_PUB_PTR0_TITMSRST(8));

    pub_regs->PTR1 = (SNPS_DDR_PUB_PTR1_TDINIT0(lpddr2_mem->misc2.ptr_tdinit0) |
                      SNPS_DDR_PUB_PTR1_TDINIT1(lpddr2_mem->misc1.ptr_tdinit1));

    pub_regs->PTR2 =
        (SNPS_DDR_PUB_PTR2_TDINIT2(lpddr2_mem->misc3.ptr_tdinit2) |
         SNPS_DDR_PUB_PTR2_TDINIT3(lpddr2_mem->config_1.phy_tdinit3));

    pub_regs->DCR =
        (SNPS_DDR_PUB_DCR_DDRMD(lpddr2_mem->misc2.dcr_ddrmd) |
         SNPS_DDR_PUB_DCR_DDRTYPE(lpddr2_mem->config_1.dcr_ddr_type) |
         SNPS_DDR_PUB_DCR_DDR8BNK(1));

    pub_regs->MR1_LPDDR2 =
        (SNPS_DDR_PUB_MR1_LPDDR2_BL(lpddr2_mem->config_0.phy_mr_bl) |
         SNPS_DDR_PUB_MR1_LPDDR2_NWR(lpddr2_mem->config_0.phy_mr_nwr));

    pub_regs->MR2 = lpddr2_mem->config_0.phy_mr_rl_wr;
    pub_regs->MR3 = lpddr2_mem->config_0.phy_mr_ds;

    pub_regs->DTPR0 =
        (SNPS_DDR_PUB_DTPR0_TMRD(2) |
         SNPS_DDR_PUB_DTPR0_TRTP(lpddr2_mem->misc2.dtpr_trtp) |
         SNPS_DDR_PUB_DTPR0_TWTR(lpddr2_mem->misc2.dtpr_twtr) |
         SNPS_DDR_PUB_DTPR0_TRP(6) |
         SNPS_DDR_PUB_DTPR0_TRCD(lpddr2_mem->dram_tmg_2.t_rcd) |
         SNPS_DDR_PUB_DTPR0_TRAS(lpddr2_mem->dram_tmg_0.t_ras_min) |
         SNPS_DDR_PUB_DTPR0_TRRD(lpddr2_mem->dram_tmg_2.t_rrd) |
         SNPS_DDR_PUB_DTPR0_TRC(lpddr2_mem->dram_tmg_0.trc) |
         SNPS_DDR_PUB_DTPR0_TCCD(lpddr2_mem->dram_tmg_2.t_ccd));

    pub_regs->DTPR1 =
        (SNPS_DDR_PUB_DTPR1_TFAW(lpddr2_mem->dram_tmg_0.t_faw) |
         SNPS_DDR_PUB_DTPR1_TRFC(131) |
         SNPS_DDR_PUB_DTPR1_TDQSCK(lpddr2_mem->config_1.dtpr_tdqsck_min) |
         SNPS_DDR_PUB_DTPR1_TDQSCKMAX(lpddr2_mem->config_1.dtpr_tdqsck_max));

    pub_regs->DTPR2 =
        SET_SNPS_DDR_PUB_DTPR2_TXS(pub_regs->DTPR2, lpddr2_mem->misc3.dtpr_txs);
    pub_regs->DTPR2 = SET_SNPS_DDR_PUB_DTPR2_TXP(pub_regs->DTPR2,
                                                 lpddr2_mem->dram_tmg_1.t_xp);

    pub_regs->PGCR = SET_SNPS_DDR_PUB_PGCR_DQSCFG(
        pub_regs->PGCR, lpddr2_mem->misc4.pgcr_dqscfg);
    pub_regs->PGCR = SET_SNPS_DDR_PUB_PGCR_DFTCMP(
        pub_regs->PGCR, lpddr2_mem->misc4.pgcr_dftcmp);
    pub_regs->PGCR = SET_SNPS_DDR_PUB_PGCR_RANKEN(
        pub_regs->PGCR, lpddr2_mem->misc4.pgcr_ranken);
    pub_regs->PGCR = SET_SNPS_DDR_PUB_PGCR_RFSHDT(
        pub_regs->PGCR, lpddr2_mem->misc4.pgcr_rfshdt);
}

void init_start(dwc_umctl2_regs_t *umctl2_regs, ddrphy_t *pub_regs,
                unsigned int base_addr)
{
    pub_regs->PIR = SNPS_DDR_PUB_PIR_CLRSR(1);
    pub_regs->PIR = (SNPS_DDR_PUB_PIR_ITMSRST(1) | SNPS_DDR_PUB_PIR_DLLSRST(1) |
                     SNPS_DDR_PUB_PIR_DLLLOCK(1) | SNPS_DDR_PUB_PIR_DRAMRST(1) |
                     SNPS_DDR_PUB_PIR_CTLDINIT(1) | SNPS_DDR_PUB_PIR_INIT(1));

    asm volatile("DSB");

    while ((pub_regs->PGSR & 0x3) != 0x3) {
    };

    pub_regs->ZQ0CR0 |= (UINT32_C(1) << 30);
    while ((pub_regs->ZQ0CR0 & (UINT32_C(1) << 30))) {
    };
    while (!(pub_regs->ZQ0SR0 & (UINT32_C(1) << 31)) || pub_regs->ZQ0SR1) {
    };

    pub_regs->PIR = SNPS_DDR_PUB_PIR_CLRSR(1);
    pub_regs->PIR = (SNPS_DDR_PUB_PIR_DRAMRST(1) |
                     SNPS_DDR_PUB_PIR_DRAMINIT(1) | SNPS_DDR_PUB_PIR_QSTRN(1) |
                     SNPS_DDR_PUB_PIR_RVTRN(1) | SNPS_DDR_PUB_PIR_INIT(1));

    asm volatile("DSB");

    while ((pub_regs->PGSR & 0x19) != 0x19) {
    };
}

int testmem_ddr(unsigned int base_addr)
{
    volatile unsigned int *pd = (volatile unsigned int *)base_addr;
    int i = 0;

    for (i = 0; i < 4; i++) {
        pd[i] = (0xc0dec0deUL >> i);
    }
    asm volatile("DSB");
    for (i = 0; i < 4; i++) {
        if (pd[i] != (0xc0dec0deUL >> i)) {
            return 1;
        }
    }
    return 0;
}
int pub_init_cmpl_wait(ddrphy_t *pub_regs, unsigned reset_type, unsigned check)
{

    while ((pub_regs->PGSR & 0x1f) != 0x1f) {
    }

    if (reset_type) {
        if ((pub_regs->PGSR != 0x1f) |
            (GET_SNPS_DDR_PUB_ZQ0SR0_ZERR(pub_regs->ZQ0SR0)))
            return 1;
    } else {
        if (check) {
            if (GET_SNPS_DDR_PUB_ZQ0SR0_ZERR(pub_regs->ZQ0SR0))
                return 1;
        }
    }

    return 0;
}

void umctl2_norm_wait(dwc_umctl2_regs_t *umctl2_regs)
{
    while (!GET_SNPS_DDR_STAT_OPERATING_MODE(umctl2_regs->STAT)) {
    }
}

void phy_train_save(ddrphy_t *pub_regs, ddr_common_t *ddr_mem_cfg)
{
    int j;
    for (j = 0; j < 4; j++) {
        ddr_mem_cfg->dump[j] =
            *((unsigned int *)((unsigned int)(&(pub_regs->DX0GCR)) + j * 0x40));
        ddr_mem_cfg->dump[j + 4] = *(
            (unsigned int *)((unsigned int)(&(pub_regs->DX0DQSTR)) + j * 0x40));
    }
    ddr_mem_cfg->dump[8] = GET_SNPS_DDR_PUB_ZQ0CR0_ZDATA(pub_regs->ZQ0CR0);
}

unsigned int check_both_ctl(unsigned int *ctl_enabled)
{
    return (ctl_enabled[0]) | (ctl_enabled[1]);
}
#endif

void FLASHMEMINITFUNC arm_hardware_sdram_initialize(void)
{
	PRINTF("arm_hardware_sdram_initialize start\n");
	init_ddr3(533, 1, 1);
	PRINTF("arm_hardware_sdram_initialize done\n");
}

#endif /* CPUSTYLE_VM14 */
#endif /* WITHSDRAMHW */
