

/*
 * ddr_cfg.h
 *
 *  Created on: Aug 28, 2013
 *      Author: dzagrebin
 */

#ifndef DDR_CFG_H_
#define DDR_CFG_H_

typedef struct {
    union {
        unsigned int full_reg;
        struct {
            unsigned t_ras_min : 6;
            unsigned t_ras_max : 7;
            unsigned t_faw : 6;
            unsigned wr2pre : 6;
            unsigned trc : 6;
        };
    } dram_tmg_0;
    union {
        unsigned int full_reg;
        struct {
            unsigned rd2pre : 5;
            unsigned t_xp : 5;
            unsigned wr2rd : 6;
            unsigned rd2wr : 5;
            unsigned read_latency : 6;
        };
    } dram_tmg_1;
    union {
        unsigned int full_reg;
        struct {
            unsigned write_latency : 6;
            unsigned t_mrd : 6;
            unsigned t_rp : 5;
            unsigned t_rrd : 4;
            unsigned t_ccd : 3;
            unsigned t_rcd : 5;
        };
    } dram_tmg_2;
    union {
        unsigned int full_reg;
        struct {
            unsigned t_cke : 4;
            unsigned t_ckesr : 6;
            unsigned t_cksre : 4;
            unsigned t_cksrx : 4;
            unsigned post_selfref_gap : 7;
        };
    } dram_tmg_3;
    union {
        unsigned int full_reg;
        struct {
            unsigned active_ranks : 2;
            unsigned burst_rdwr : 4;
            unsigned ddr3_type : 1;
            unsigned lpddr2_type : 1;
            unsigned selfref_to_x32 : 8;
            unsigned port_en_0 : 1;
            unsigned port_en_1 : 1;
            unsigned port_en_2 : 1;
        };
    } misc0;
    union {
        unsigned int full_reg;
        struct {
            unsigned refresh_margin : 4;
            unsigned refresh_to_x32 : 5;
            unsigned refresh_burst : 3;
            unsigned per_bank_refresh : 1;
            unsigned t_rfc_nom_x32 : 8;
            unsigned t_rfc_min : 9;
        };
    } refr_cnt;
    union {
        unsigned int full_reg;
        struct {
            unsigned dfi_t_ctrl_delay : 5;
            unsigned dfi_t_rddata_en : 6;
            unsigned dfi_tphy_wrdata : 6;
            unsigned dfi_tphy_wrlat : 6;
            unsigned dfi_t_dram_clk_disable : 4;
            unsigned dfi_t_dram_clk_enable : 4;
        };
    } dfi_tmg;
    union {
        unsigned int full_reg;
        struct {
            unsigned addrmap_bank_b0 : 4;
            unsigned addrmap_bank_b1 : 4;
            unsigned addrmap_bank_b2 : 4;
            unsigned addrmap_col_b2 : 4;
            unsigned addrmap_col_b3 : 4;
            unsigned addrmap_col_b4 : 4;
            unsigned addrmap_col_b5 : 4;
        };
    } addrmap0;
    union {
        unsigned int full_reg;
        struct {
            unsigned addrmap_cs_bit0 : 5;
            unsigned addrmap_col_b6 : 4;
            unsigned addrmap_col_b7 : 4;
            unsigned addrmap_col_b8 : 4;
            unsigned addrmap_col_b9 : 4;
            unsigned addrmap_col_b10 : 4;
            unsigned addrmap_col_b11 : 4;
        };
    } addrmap1;
    union {
        unsigned int full_reg;
        struct {
            unsigned addrmap_row_b0 : 4;
            unsigned addrmap_row_b1 : 4;
            unsigned addrmap_row_b2_10 : 4;
            unsigned addrmap_row_b11 : 4;
            unsigned addrmap_row_b12 : 4;
            unsigned addrmap_row_b13 : 4;
            unsigned addrmap_row_b14 : 4;
            unsigned addrmap_row_b15 : 4;
        };
    } addrmap2;
    union {
        unsigned int full_reg;
        struct {
            unsigned ptr_tdllrst : 6;
            unsigned ptr_tdlllock : 12;
            unsigned ptr_titmrst : 4;
            unsigned ptr_tdinit1 : 8;
        };
    } misc1;
    union {
        unsigned int full_reg;
        struct {
            unsigned ptr_tdinit0 : 19;
            unsigned dcr_ddrmd : 3;
            unsigned dtpr_trtp : 3;
            unsigned dtpr_twtr : 3;
            unsigned dtpr_tccd : 1;
            unsigned dtpr_trtw : 1;
        };
    } misc2;
    union {
        unsigned int full_reg;
        struct {
            unsigned ptr_tdinit2 : 17;
            unsigned dtpr_txs : 10;
            unsigned dtpr_tcke : 4;
        };
    } misc3;
    union {
        unsigned int full_reg;
        struct {
            unsigned tdllk : 10;
            unsigned pgcr_dqscfg : 1;
            unsigned pgcr_dftcmp : 1;
            unsigned pgcr_ranken : 2;
            unsigned pgcr_rfshdt : 2;
            unsigned sel_cpll : 8;
            unsigned ddr_div : 2;
            unsigned ddr_remap : 1;
        };
    } misc4;
    union {
        unsigned int full_reg;
        struct {
            unsigned dfi_upd_int_min : 8;
            unsigned dfi_upd_int_max : 8;
        };
    } misc5;

    unsigned int dump[32]; // Need for bootrom function _api_DDR_INIT()
} ddr_common_t;

typedef struct {
    union {
        unsigned int full_reg;
        struct {
            unsigned t_ras_min : 6;
            unsigned t_ras_max : 7;
            unsigned t_faw : 6;
            unsigned wr2pre : 6;
            unsigned trc : 6;
        };
    } dram_tmg_0;
    union {
        unsigned int full_reg;
        struct {
            unsigned rd2pre : 5;
            unsigned t_xp : 5;
            unsigned wr2rd : 6;
            unsigned rd2wr : 5;
            unsigned read_latency : 6;
        };
    } dram_tmg_1;
    union {
        unsigned int full_reg;
        struct {
            unsigned write_latency : 6;
            unsigned t_mrd : 6;
            unsigned t_rp : 5;
            unsigned t_rrd : 4;
            unsigned t_ccd : 3;
            unsigned t_rcd : 5;
        };
    } dram_tmg_2;
    union {
        unsigned int full_reg;
        struct {
            unsigned t_cke : 4;
            unsigned t_ckesr : 6;
            unsigned t_cksre : 4;
            unsigned t_cksrx : 4;
            unsigned post_selfref_gap : 7;
        };
    } dram_tmg_3;
    union {
        unsigned int full_reg;
        struct {
            unsigned active_ranks : 2;
            unsigned burst_rdwr : 4;
            unsigned ddr3_type : 1;
            unsigned lpddr2_type : 1;
            unsigned selfref_to_x32 : 8;
            unsigned port_en_0 : 1;
            unsigned port_en_1 : 1;
            unsigned port_en_2 : 1;
        };
    } misc0;
    union {
        unsigned int full_reg;
        struct {
            unsigned refresh_margin : 4;
            unsigned refresh_to_x32 : 5;
            unsigned refresh_burst : 3;
            unsigned per_bank_refresh : 1;
            unsigned t_rfc_nom_x32 : 8;
            unsigned t_rfc_min : 9;
        };
    } refr_cnt;
    union {
        unsigned int full_reg;
        struct {
            unsigned dfi_t_ctrl_delay : 5;
            unsigned dfi_t_rddata_en : 6;
            unsigned dfi_tphy_wrdata : 6;
            unsigned dfi_tphy_wrlat : 6;
            unsigned dfi_t_dram_clk_disable : 4;
            unsigned dfi_t_dram_clk_enable : 4;
        };
    } dfi_tmg;
    union {
        unsigned int full_reg;
        struct {
            unsigned addrmap_bank_b0 : 4;
            unsigned addrmap_bank_b1 : 4;
            unsigned addrmap_bank_b2 : 4;
            unsigned addrmap_col_b2 : 4;
            unsigned addrmap_col_b3 : 4;
            unsigned addrmap_col_b4 : 4;
            unsigned addrmap_col_b5 : 4;
        };
    } addrmap0;
    union {
        unsigned int full_reg;
        struct {
            unsigned addrmap_cs_bit0 : 5;
            unsigned addrmap_col_b6 : 4;
            unsigned addrmap_col_b7 : 4;
            unsigned addrmap_col_b8 : 4;
            unsigned addrmap_col_b9 : 4;
            unsigned addrmap_col_b10 : 4;
            unsigned addrmap_col_b11 : 4;
        };
    } addrmap1;
    union {
        unsigned int full_reg;
        struct {
            unsigned addrmap_row_b0 : 4;
            unsigned addrmap_row_b1 : 4;
            unsigned addrmap_row_b2_10 : 4;
            unsigned addrmap_row_b11 : 4;
            unsigned addrmap_row_b12 : 4;
            unsigned addrmap_row_b13 : 4;
            unsigned addrmap_row_b14 : 4;
            unsigned addrmap_row_b15 : 4;
        };
    } addrmap2;
    union {
        unsigned int full_reg;
        struct {
            unsigned ptr_tdllrst : 6;
            unsigned ptr_tdlllock : 12;
            unsigned ptr_titmrst : 4;
            unsigned ptr_tdinit1 : 8;
        };
    } misc1;
    union {
        unsigned int full_reg;
        struct {
            unsigned ptr_tdinit0 : 19;
            unsigned dcr_ddrmd : 3;
            unsigned dtpr_trtp : 3;
            unsigned dtpr_twtr : 3;
            unsigned dtpr_tccd : 1;
            unsigned dtpr_trtw : 1;
        };
    } misc2;
    union {
        unsigned int full_reg;
        struct {
            unsigned ptr_tdinit2 : 17;
            unsigned dtpr_txs : 10;
            unsigned dtpr_tcke : 4;
        };
    } misc3;
    union {
        unsigned int full_reg;
        struct {
            unsigned tdllk : 10;
            unsigned pgcr_dqscfg : 1;
            unsigned pgcr_dftcmp : 1;
            unsigned pgcr_ranken : 2;
            unsigned pgcr_rfshdt : 2;
            unsigned sel_cpll : 8;
            unsigned ddr_div : 2;
            unsigned ddr_remap : 1;
        };
    } misc4;
    union {
        unsigned int full_reg;
        struct {
            unsigned dfi_upd_int_min : 8;
            unsigned dfi_upd_int_max : 8;
        };
    } misc5;

    unsigned int dump[32]; // Need for bootrom function _api_DDR_INIT()

    union {
        unsigned int full_reg;
        struct {
            unsigned phy_mr_bl : 2;
            unsigned phy_mr_cl : 3;
            unsigned phy_mr_dr : 1;
            unsigned phy_mr_wr : 3;
            unsigned phy_mr_cwl : 3;
            unsigned phy_mr_asr : 1;
        };
    } config_0;
    union {
        unsigned int full_reg;
        struct {
            unsigned t_mod : 10;
            unsigned dtpr_tmrd : 2;
            unsigned dtpr_tmod : 2;
            unsigned pir_dram_rst : 1;
        };
    } config_1;

} ddr3_t;

typedef struct {
    union {
        unsigned int full_reg;
        struct {
            unsigned t_ras_min : 6;
            unsigned t_ras_max : 7;
            unsigned t_faw : 6;
            unsigned wr2pre : 6;
            unsigned trc : 6;
        };
    } dram_tmg_0;
    union {
        unsigned int full_reg;
        struct {
            unsigned rd2pre : 5;
            unsigned t_xp : 5;
            unsigned wr2rd : 6;
            unsigned rd2wr : 5;
            unsigned read_latency : 6;
        };
    } dram_tmg_1;
    union {
        unsigned int full_reg;
        struct {
            unsigned write_latency : 6;
            unsigned t_mrd : 6;
            unsigned t_rp : 5;
            unsigned t_rrd : 4;
            unsigned t_ccd : 3;
            unsigned t_rcd : 5;
        };
    } dram_tmg_2;
    union {
        unsigned int full_reg;
        struct {
            unsigned t_cke : 4;
            unsigned t_ckesr : 6;
            unsigned t_cksre : 4;
            unsigned t_cksrx : 4;
            unsigned post_selfref_gap : 7;
        };
    } dram_tmg_3;
    union {
        unsigned int full_reg;
        struct {
            unsigned active_ranks : 2;
            unsigned burst_rdwr : 4;
            unsigned ddr3_type : 1;
            unsigned lpddr2_type : 1;
            unsigned selfref_to_x32 : 8;
            unsigned port_en_0 : 1;
            unsigned port_en_1 : 1;
            unsigned port_en_2 : 1;
        };
    } misc0;
    union {
        unsigned int full_reg;
        struct {
            unsigned refresh_margin : 4;
            unsigned refresh_to_x32 : 5;
            unsigned refresh_burst : 3;
            unsigned per_bank_refresh : 1;
            unsigned t_rfc_nom_x32 : 8;
            unsigned t_rfc_min : 9;
        };
    } refr_cnt;
    union {
        unsigned int full_reg;
        struct {
            unsigned dfi_t_ctrl_delay : 5;
            unsigned dfi_t_rddata_en : 6;
            unsigned dfi_tphy_wrdata : 6;
            unsigned dfi_tphy_wrlat : 6;
            unsigned dfi_t_dram_clk_disable : 4;
            unsigned dfi_t_dram_clk_enable : 4;
        };
    } dfi_tmg;
    union {
        unsigned int full_reg;
        struct {
            unsigned addrmap_bank_b0 : 4;
            unsigned addrmap_bank_b1 : 4;
            unsigned addrmap_bank_b2 : 4;
            unsigned addrmap_col_b2 : 4;
            unsigned addrmap_col_b3 : 4;
            unsigned addrmap_col_b4 : 4;
            unsigned addrmap_col_b5 : 4;
        };
    } addrmap0;
    union {
        unsigned int full_reg;
        struct {
            unsigned addrmap_cs_bit0 : 5;
            unsigned addrmap_col_b6 : 4;
            unsigned addrmap_col_b7 : 4;
            unsigned addrmap_col_b8 : 4;
            unsigned addrmap_col_b9 : 4;
            unsigned addrmap_col_b10 : 4;
            unsigned addrmap_col_b11 : 4;
        };
    } addrmap1;
    union {
        unsigned int full_reg;
        struct {
            unsigned addrmap_row_b0 : 4;
            unsigned addrmap_row_b1 : 4;
            unsigned addrmap_row_b2_10 : 4;
            unsigned addrmap_row_b11 : 4;
            unsigned addrmap_row_b12 : 4;
            unsigned addrmap_row_b13 : 4;
            unsigned addrmap_row_b14 : 4;
            unsigned addrmap_row_b15 : 4;
        };
    } addrmap2;
    union {
        unsigned int full_reg;
        struct {
            unsigned ptr_tdllrst : 6;
            unsigned ptr_tdlllock : 12;
            unsigned ptr_titmrst : 4;
            unsigned ptr_tdinit1 : 8;
        };
    } misc1;
    union {
        unsigned int full_reg;
        struct {
            unsigned ptr_tdinit0 : 19;
            unsigned dcr_ddrmd : 3;
            unsigned dtpr_trtp : 3;
            unsigned dtpr_twtr : 3;
            unsigned dtpr_tccd : 1;
            unsigned dtpr_trtw : 1;
        };
    } misc2;
    union {
        unsigned int full_reg;
        struct {
            unsigned ptr_tdinit2 : 17;
            unsigned dtpr_txs : 10;
            unsigned dtpr_tcke : 4;
        };
    } misc3;
    union {
        unsigned int full_reg;
        struct {
            unsigned tdllk : 10;
            unsigned pgcr_dqscfg : 1;
            unsigned pgcr_dftcmp : 1;
            unsigned pgcr_ranken : 2;
            unsigned pgcr_rfshdt : 2;
            unsigned sel_cpll : 8;
            unsigned ddr_div : 2;
            unsigned ddr_remap : 1;
        };
    } misc4;
    union {
        unsigned int full_reg;
        struct {
            unsigned dfi_upd_int_min : 8;
            unsigned dfi_upd_int_max : 8;
        };
    } misc5;

    unsigned int dump[32]; // Need for bootrom function _api_DDR_INIT()

    union {
        unsigned int full_reg;
        struct {
            unsigned phy_mr_bl : 3;
            unsigned phy_mr_wc : 1;
            unsigned phy_mr_nwr : 3;
            unsigned phy_mr_rl_wr : 4;
            unsigned phy_mr_ds : 4;
        };
    } config_0;
    union {
        unsigned int full_reg;
        struct {
            unsigned dcr_ddr8bnk : 1;
            unsigned dcr_ddr_type : 2;
            unsigned t_mrw : 10;
            unsigned phy_tdinit3 : 10;
            unsigned dtpr_tdqsck_min : 3;
            unsigned dtpr_tdqsck_max : 3;
        };
    } config_1;

} lpddr2_t;

#endif /* DDR_CFG_H_ */
