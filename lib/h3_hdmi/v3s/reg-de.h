#ifndef __REG_DE_H__
#define __REG_DE_H__

#include "main.h"

#define V3S_DE_BASE			(0x01000000)


///#define V3S_DE_MUX0         (0x01100000)  ///1M


#define V3S_DE_MUX_GLB		(0x00100000 + 0x00000)  ///4k
#define V3S_DE_MUX_BLD		(0x00100000 + 0x01000)  ///4k
#define V3S_DE_MUX_CHAN		(0x00100000 + 0x02000)  ///4k
#define V3S_DE_MUX_VSU		(0x00100000 + 0x20000)  ///128k
#define V3S_DE_MUX_GSU1		(0x00100000 + 0x30000)
#define V3S_DE_MUX_GSU2		(0x00100000 + 0x40000)
#define V3S_DE_MUX_GSU3		(0x00100000 + 0x50000)
#define V3S_DE_MUX_FCE		(0x00100000 + 0xa0000)
#define V3S_DE_MUX_BWS		(0x00100000 + 0xa2000)
#define V3S_DE_MUX_LTI		(0x00100000 + 0xa4000)
#define V3S_DE_MUX_PEAK		(0x00100000 + 0xa6000)
#define V3S_DE_MUX_ASE		(0x00100000 + 0xa8000)
#define V3S_DE_MUX_FCC		(0x00100000 + 0xaa000)
#define V3S_DE_MUX_DCSC		(0x00100000 + 0xb0000)

#define ul32_t uint32_t

struct de_clk_t {
	ul32_t gate_cfg;
	ul32_t bus_cfg;
	ul32_t rst_cfg;
	ul32_t div_cfg;
	ul32_t sel_cfg;
};

struct de_glb_t {
	ul32_t ctl;    ///0x000
	ul32_t status; ///0x004
	ul32_t dbuff;  ///0x008
	ul32_t size;   ///0x00c
};

struct de_bld_t {
	ul32_t fcolor_ctl;
	struct {
		ul32_t fcolor;
		ul32_t insize;
		ul32_t offset;
		ul32_t dum;
	} attr[4];
	ul32_t dum0[15];
	ul32_t route;
	ul32_t premultiply;
	ul32_t bkcolor;
	ul32_t output_size;
	ul32_t bld_mode[4];
	ul32_t dum1[4];
	ul32_t ck_ctl;
	ul32_t ck_cfg;
	ul32_t dum2[2];
	ul32_t ck_max[4];
	ul32_t dum3[4];
	ul32_t ck_min[4];
	ul32_t dum4[3];
	ul32_t out_ctl;
};

struct de_vi_t {
	struct {
		ul32_t attr;
		ul32_t size;
		ul32_t coord;
		ul32_t pitch[3];
		ul32_t top_laddr[3];
		ul32_t bot_laddr[3];
	} cfg[4];
	ul32_t fcolor[4];
	ul32_t top_haddr[3];
	ul32_t bot_haddr[3];
	ul32_t ovl_size[2];
	ul32_t hori[2];
	ul32_t vert[2];
};

struct de_ui_t {
	struct {
		ul32_t attr;
		ul32_t size;
		ul32_t coord;
		ul32_t pitch;
		ul32_t top_laddr;
		ul32_t bot_laddr;
		ul32_t fcolor;
		ul32_t dum;
	} cfg[4];
	ul32_t top_haddr;
	ul32_t bot_haddr;
	ul32_t ovl_size;
};

#endif /* __V3S_REG_DE_H__ */
