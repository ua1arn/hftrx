#ifndef __T113_REG_TCONLCD_H__
#define __T113_REG_TCONLCD_H__

#include "types.h"

#define T113_TCONLCD_BASE		(0x05461000)

struct t113_tconlcd_reg_t {
	uint32_t gctrl;				/* 0x00 */
	uint32_t gint0;				/* 0x04 */
	uint32_t gint1;				/* 0x08 */
	uint32_t res_0c;
	uint32_t frm_ctrl;				/* 0x10 */
	uint32_t frm_seed[6];			/* 0x14 */
	uint32_t frm_table[4];			/* 0x2c */
	uint32_t fifo_3d;				/* 0x3c */
	uint32_t ctrl;					/* 0x40 */
	uint32_t dclk;					/* 0x44 */
	uint32_t timing0;				/* 0x48 */
	uint32_t timing1;				/* 0x4c */
	uint32_t timing2;				/* 0x50 */
	uint32_t timing3;				/* 0x54 */
	uint32_t hv_intf;				/* 0x58 */
	uint32_t res_5c;
	uint32_t cpu_intf;				/* 0x60 */
	uint32_t cpu_wr;				/* 0x64 */
	uint32_t cpu_rd0;				/* 0x68 */
	uint32_t cpu_rd1;				/* 0x6c */
	uint32_t res_70_80[5];			/* 0x70 */
	uint32_t lvds_intf;			/* 0x84 */
	uint32_t io_polarity;			/* 0x88 */
	uint32_t io_tristate;			/* 0x8c */
	uint32_t res_90_f8[27];
	uint32_t debug;				/* 0xfc */
	uint32_t ceu_ctl;				/* 0x100 */
	uint32_t res_104_10c[3];
	uint32_t ceu_coef[20];			/* 0x110 */
	uint32_t cpu_tri0;				/* 0x160 */
	uint32_t cpu_tri1;				/* 0x164 */
	uint32_t cpu_tri2;				/* 0x168 */
	uint32_t cpu_tri3;				/* 0x16c */
	uint32_t cpu_tri4;				/* 0x170 */
	uint32_t cpu_tri5;				/* 0x174 */
	uint32_t res_178_17c[2];
	uint32_t cmap_ctl;				/* 0x180 */
	uint32_t res_184_18c[3];
	uint32_t cmap_odd0;			/* 0x190 */
	uint32_t cmap_odd1;			/* 0x194 */
	uint32_t cmap_even0;			/* 0x198 */
	uint32_t cmap_even1;			/* 0x19c */
	uint32_t res_1a0_1ec[20];
	uint32_t safe_period;			/* 0x1f0 */
	uint32_t res_1f4_21c[11];
	uint32_t lvds_ana0;			/* 0x220 */
	uint32_t lvds_ana1;			/* 0x224 */
	uint32_t res_228_22c[2];
	uint32_t sync_ctl;				/* 0x230 */
	uint32_t sync_pos;				/* 0x234 */
	uint32_t slave_stop_pos;		/* 0x238 */
	uint32_t res_23c_3fc[113];
	uint32_t gamma_table[256];		/* 0x400 */
};

#endif /* __T113_REG_TCONLCD_H__ */
