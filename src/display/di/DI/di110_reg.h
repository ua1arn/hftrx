/*
 * Copyright (c) 2020-2031 Allwinnertech Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
#ifndef _DI110_REG_H_
#define _DI110_REG_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../TVD/tvd.h"


#define __iomem /* volatile */

#if 0

#define DI_INFO printf
#define DI_DEBUG printf
#define TAG

#else

#define DI_INFO(...)
#define DI_DEBUG(...)

#endif

#define DI_ALIGN(value, align) ((align == 0) ? \
				value : \
				(((value) + ((align) - 1)) & ~((align) - 1)))

#define dma_addr_t unsigned int

struct di_mapped_buf {
	uint32_t size_request;
	uint32_t size_alloced;
	void *vir_addr;
	dma_addr_t dma_addr;
};

//---

union di_ctrl_reg {
	uint32_t dwval;
	struct {
		uint32_t start:1;
		uint32_t reserve0:3;
		uint32_t auto_clk_gate_en:1;
		uint32_t reserve1:26;
		uint32_t reset:1;
	} bits;
};

union di_int_ctrl_reg {
	uint32_t dwval;
	struct {
		uint32_t finish_int_en:1;
		uint32_t reserve0:31;
	} bits;
};

union di_status_reg {
	uint32_t dwval;
	struct {
		uint32_t finish_flag:1;
		uint32_t reserve0:3;
		uint32_t busy:1;
		uint32_t reserve1:7;
		uint32_t cut_blk:4;
		uint32_t cur_line:10;
		uint32_t reserve2:2;
		uint32_t cur_chl:2;
		uint32_t reserve3:2;
	} bits;
};

union di_version_reg {
	uint32_t dwval;
	struct {
		uint32_t ip_version:12;
		uint32_t reserve0:20;
	} bits;
};

union di_size_reg {
	uint32_t dwval;
	struct {
		uint32_t width:11;
		uint32_t reserve0:5;
		uint32_t height:11;
		uint32_t reserve1:5;
	} bits;
};

union di_format_reg {
	uint32_t dwval;
	struct {
		uint32_t in_format:3;
		uint32_t reserve0:1;
		uint32_t field_order:1;
		uint32_t reserve1:27;
	} bits;
};

union di_model_para_reg {
	uint32_t dwval;
	struct {
		uint32_t di_mode:1;
		uint32_t reserve0:3;
		uint32_t output_mode:1;
		uint32_t reserve1:3;
		uint32_t feather_detection_en:1;
		uint32_t reserve2:23;
	} bits;
};

/* for pitch of plane0(Y Component)
 * plane0 of in_fb0
 * plane0 of in_fb1
 */
union di_in_pitch0_reg {
	uint32_t dwval;
	struct {
		uint32_t fb0_plane0:12;
		uint32_t reserve0:4;
		uint32_t fb1_plane0:12;
		uint32_t reserve1:4;
	} bits;
};

/* for pitch of plane1(U(or V) Component)
 * plane1 of in_fb0
 * plane1 of in_fb1
 */
union di_in_pitch1_reg {
	uint32_t dwval;
	struct {
		uint32_t fb0_plane1:12;
		uint32_t reserve0:4;
		uint32_t fb1_plane1:12;
		uint32_t reserve1:4;
	} bits;
};

/* for pitch of plane2(V(or U) Component)
 * plane2 of in_fb0
 * plane2 of in_fb1
 */
union di_in_pitch2_reg {
	uint32_t dwval;
	struct {
		uint32_t fb0_plane2:12;
		uint32_t reserve0:4;
		uint32_t fb1_plane2:12;
		uint32_t reserve1:4;
	} bits;
};

/* for pitch of plane0(Y Component)
 * plane0 of out_fb
 */
union di_out_pitch0_reg {
	uint32_t dwval;
	struct {
		uint32_t plane0:12;
		uint32_t reserve0:20;
	} bits;
};

/* for pitch of plane1(U(or V) Component)
 * plane1 of out_fb
 */
union di_out_pitch1_reg {
	uint32_t dwval;
	struct {
		uint32_t plane1:12;
		uint32_t reserve0:20;
	} bits;
};

/* for pitch of plane2(V(or U) Component)
 * plane2 of out_fb
 */
union di_out_pitch2_reg {
	uint32_t dwval;
	struct {
		uint32_t plane2:12;
		uint32_t reserve0:20;
	} bits;
};

/* for pitch of motion flag*/
union di_mdflag_pitch_reg {
	uint32_t dwval;
	struct {
		uint32_t flag_pitch:12;
		uint32_t reserve0:20;
	} bits;
};

/* plane0(Y Component) address of in_fb0*/
union di_in_frame0_add0_reg {
	uint32_t dwval;
	struct {
		uint32_t plane0:32;
	} bits;
};

/* plane1(U(or V) Component) address of in_fb0*/
union di_in_frame0_add1_reg {
	uint32_t dwval;
	struct {
		uint32_t plane1:32;
	} bits;
};

/* plane2(V(or U) Component) address of in_fb0*/
union di_in_frame0_add2_reg {
	uint32_t dwval;
	struct {
		uint32_t plane2:32;
	} bits;
};

/*extension address of in_fb0*/
union di_in_frame0_add_ext_reg {
	uint32_t dwval;
	struct {
		uint32_t plane0_h:8;
		uint32_t plane1_h:8;
		uint32_t plane2_h:8;
		uint32_t reserve0:8;
	} bits;
};

/* plane0(Y Component) address of in_fb1*/
union di_in_frame1_add0_reg {
	uint32_t dwval;
	struct {
		uint32_t plane0:32;
	} bits;
};

/* plane1(U(or V) Component) address of in_fb1*/
union di_in_frame1_add1_reg {
	uint32_t dwval;
	struct {
		uint32_t plane1:32;
	} bits;
};

/* plane2(V(or U) Component) address of in_fb1*/
union di_in_frame1_add2_reg {
	uint32_t dwval;
	struct {
		uint32_t plane2:32;
	} bits;
};

/*extension address of in_fb1*/
union di_in_frame1_add_ext_reg {
	uint32_t dwval;
	struct {
		uint32_t plane0_h:8;
		uint32_t plane1_h:8;
		uint32_t plane2_h:8;
		uint32_t reserve0:8;
	} bits;
};

/* plane0(Y Component) address of out_fb0*/
union di_out_frame0_add0_reg {
	uint32_t dwval;
	struct {
		uint32_t plane0:32;
	} bits;
};

/* plane1(U(or V) Component) address of out_fb0*/
union di_out_frame0_add1_reg {
	uint32_t dwval;
	struct {
		uint32_t plane1:32;
	} bits;
};

/* plane2(V(or U) Component) address of out_fb0*/
union di_out_frame0_add2_reg {
	uint32_t dwval;
	struct {
		uint32_t plane2:32;
	} bits;
};

/*extension address of out_fb0*/
union di_out_frame0_add_ext_reg {
	uint32_t dwval;
	struct {
		uint32_t plane0_h:8;
		uint32_t plane1_h:8;
		uint32_t plane2_h:8;
		uint32_t reserve0:8;
	} bits;
};

/* plane0(Y Component) address of out_fb1*/
union di_out_frame1_add0_reg {
	uint32_t dwval;
	struct {
		uint32_t plane0:32;
	} bits;
};

/* plane1(U(or V) Component) address of out_fb1*/
union di_out_frame1_add1_reg {
	uint32_t dwval;
	struct {
		uint32_t plane1:32;
	} bits;
};

/* plane2(V(or U) Component) address of out_fb1*/
union di_out_frame1_add2_reg {
	uint32_t dwval;
	struct {
		uint32_t plane2:32;
	} bits;
};

/*extension address of out_fb1*/
union di_out_frame1_add_ext_reg {
	uint32_t dwval;
	struct {
		uint32_t plane0_h:8;
		uint32_t plane1_h:8;
		uint32_t plane2_h:8;
		uint32_t reserve0:8;
	} bits;
};

union di_mdflag_add_reg {
	uint32_t dwval;
	struct {
		uint32_t flag_add:32;
	} bits;
};

union di_mdflag_add_ext_reg {
	uint32_t dwval;
	struct {
		uint32_t flag_add_h:32;
	} bits;
};

union di_luma_para_reg {
	uint32_t dwval;
	struct {
		uint32_t min_luma_th:4;
		uint32_t max_luma_th:4;
		uint32_t luma_th_shifter:4;
		uint32_t avg_luma_shifter:4;
		uint32_t angle_const_th:4;
		uint32_t max_angle:4;
		uint32_t slow_motion_fac:4;
		uint32_t reserve0:4;
	} bits;
};

union di_chroma_para_reg {
	uint32_t dwval;
	struct {
		uint32_t chroma_diff_th:4;
		uint32_t chroma_spatial_th:4;
		uint32_t reserve0:24;
	} bits;
};

union di_process_time_reg {
	uint32_t dwval;
	struct {
		uint32_t time:32;
	} bits;
};

/*register of di110*/
struct di_reg {
	union di_ctrl_reg ctrl;					//0x000
	union di_int_ctrl_reg int_ctrl;				//0x004
	union di_status_reg status;				//0x008
	union di_version_reg version;				//0x00c
	union di_size_reg size;					//0x010
	union di_format_reg format;				//0x014
	union di_model_para_reg model_para;			//0x018

//pitch
	union di_in_pitch0_reg in_pitch0;			//0x01c
	union di_in_pitch1_reg in_pitch1;			//0x020
	union di_in_pitch2_reg in_pitch2;			//0x024

	union di_out_pitch0_reg out_pitch0;			//0x028
	union di_out_pitch1_reg out_pitch1;			//0x02c
	union di_out_pitch2_reg out_pitch2;			//0x030

	union di_mdflag_pitch_reg mdflag_pitch;			//0x034

//address
	union di_in_frame0_add0_reg in_frame0_addr0;		//0x038
	union di_in_frame0_add1_reg in_frame0_addr1;		//0x03c
	union di_in_frame0_add2_reg in_frame0_addr2;		//0x040
	union di_in_frame0_add_ext_reg in_frame0_add_ext;	//0x044

	union di_in_frame1_add0_reg in_frame1_addr0;		//0x048
	union di_in_frame1_add1_reg in_frame1_addr1;		//0x04c
	union di_in_frame1_add2_reg in_frame1_addr2;		//0x050
	union di_in_frame1_add_ext_reg in_frame1_add_ext;	//0x054

	union di_out_frame0_add0_reg out_frame0_addr0;		//0x058
	union di_out_frame0_add1_reg out_frame0_addr1;		//0x05c
	union di_out_frame0_add2_reg out_frame0_addr2;		//0x060
	union di_out_frame0_add_ext_reg out_frame0_add_ext;	//0x064

	union di_out_frame1_add0_reg out_frame1_addr0;		//0x068
	union di_out_frame1_add1_reg out_frame1_addr1;		//0x06c
	union di_out_frame1_add2_reg out_frame1_addr2;		//0x070
	union di_out_frame1_add_ext_reg out_frame1_add_ext;	//0x074

	union di_mdflag_add_reg mdflag_add;			//0x078
	union di_mdflag_add_ext_reg mdflag_add_ext;		//0x7c

	union di_luma_para_reg luma_para;			//0x80
	union di_chroma_para_reg chroma_para;			//0x84

	uint32_t res[6];						//0x88~0x9c

	union di_process_time_reg process_time;			//0x0a0
};
#endif
