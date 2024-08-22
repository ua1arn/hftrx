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
#include "hardware.h"
#include "formats.h"
#if WIHSTVDHW

#include "src/display/display.h"

#include "buffers.h"
#include "di110_reg.h"
#include "di110.h"

#include "sunxi_di.h"

static struct di_reg *di_reg_base;

static struct di_reg *di_dev_get_reg_base(void)
{
	return di_reg_base;
}

void di_dev_set_reg_base(void __iomem *reg_base)
{
	di_reg_base = (struct di_reg *)reg_base;
}

void di_dev_exit(void)
{
	di_reg_base = 0;
}

static void di_dev_dump_reg_value(void)
{
	//unsigned int *addr;
	unsigned long i;
	struct di_reg *reg = di_dev_get_reg_base();

	//addr = (unsigned int *)reg;

	DI_INFO("GENERAL TOP REG(0x00~0x18):\n");
	for (i = 0x0; i <= 0x18; i += 4) {
		DI_INFO("0x%02lx:0x%08x ", i, *((unsigned int *)((unsigned long)addr + i)));
	}
	DI_INFO("\n");

	DI_INFO("PITCH REG(0x1c~0x34):\n");
	for (i = 0x1c; i <= 0x34; i += 4) {
		DI_INFO("0x%02lx:0x%08x ", i, *((unsigned int *)((unsigned long)addr + i)));
	}
	DI_INFO("\n");

	DI_INFO("ADDRESS REG:(0x38~0x7c)\n");
	for (i = 0x38; i <= 0x7c; i += 4) {
		DI_INFO("0x%02lx:0x%08x ", i, *((unsigned int *)((unsigned long)addr + i)));
	}
	DI_INFO("\n");

	DI_INFO("OTHER REGS(0x80~0xa0):\n");
	for (i = 0x80; i <= 0xa0; i += 4) {
		DI_INFO("0x%02lx:0x%08x ", i, *((unsigned int *)((unsigned long)addr + i)));
	}
	DI_INFO("\n");
}

/*
static bool di_dev_work_mode_is_change(struct di_process_fb_arg *fb_arg)
{
	if ((fb_arg->is_interlace != current_fb_arg.is_interlace)
		|| (fb_arg->field_order != current_fb_arg.field_order)
		|| (fb_arg->pixel_format != current_fb_arg.pixel_format)
		|| (fb_arg->size.width != current_fb_arg.size.width)
		|| (fb_arg->size.height != current_fb_arg.size.height)) {
		DI_INFO("DI device work mode changes!\n ");
		return true;
	}

	return false;
}
*/

/*
* convert format to the format that indecated in DI HW
*/
static uint32_t di_dev_convert_fmt(uint32_t format)
{
return YUV420_NV12; //������
/*
	switch (format) {
	case DRM_FORMAT_YUV420:
	case DRM_FORMAT_YVU420:
		return YUV420_PLANNER;

	case DRM_FORMAT_NV12:
		return YUV420_NV12;

	case DRM_FORMAT_NV21:
		return YUV420_NV21;

	case DRM_FORMAT_YUV422:
	case DRM_FORMAT_YVU422:
		return YUV422_PLANNER;

	case DRM_FORMAT_NV16:
		return YUV422_NV16;

	case DRM_FORMAT_NV61:
		return YUV422_NV61;
	default:
		DI_ERR(TAG"unknown format=%s(0x%x)\n",
			di_format_to_string(format), format);
		return 0;
	}
*/
}

static uint8_t di_get_format_type(uint32_t format)
{
return UVCOMBINE_FORMAT; //������
/*
	switch (format) {
	case DRM_FORMAT_YUV420:
	case DRM_FORMAT_YVU420:
		return PLANNER_FORMAT;

	case DRM_FORMAT_NV12:
	case DRM_FORMAT_NV21:
		return UVCOMBINE_FORMAT;

	case DRM_FORMAT_YUV422:
	case DRM_FORMAT_YVU422:
		return PLANNER_FORMAT;

	case DRM_FORMAT_NV16:
	case DRM_FORMAT_NV61:
		return UVCOMBINE_FORMAT;
	default:
		DI_ERR(TAG"unknown format=%s(0x%x)\n",
			di_format_to_string(format), format);
		return 0;
	}

	return 0;
*/
}

/*set output address
* set out put fb y/cb/cr address
*/
static void di_dev_set_addr(struct di_reg *reg,
		struct di_process_fb_arg *fb_arg)
{
//set in_fb0
	reg->in_frame0_addr0.dwval = fb_arg->in_fb0.buf.addr.y_addr & 0xffffffff;
	reg->in_frame0_addr1.dwval = fb_arg->in_fb0.buf.addr.cb_addr & 0xffffffff;
	reg->in_frame0_addr2.dwval = fb_arg->in_fb0.buf.addr.cr_addr & 0xffffffff;
	reg->in_frame0_add_ext.bits.plane0_h
				= fb_arg->in_fb0.buf.addr.y_addr >> 32 & 0xff;
	reg->in_frame0_add_ext.bits.plane1_h
				= fb_arg->in_fb0.buf.addr.cb_addr >> 32 & 0xff;
	reg->in_frame0_add_ext.bits.plane2_h
				= fb_arg->in_fb0.buf.addr.cr_addr >> 32 & 0xff;
//set in_fb1
	reg->in_frame1_addr0.dwval = fb_arg->in_fb1.buf.addr.y_addr & 0xffffffff;
	reg->in_frame1_addr1.dwval = fb_arg->in_fb1.buf.addr.cb_addr & 0xffffffff;
	reg->in_frame1_addr2.dwval = fb_arg->in_fb1.buf.addr.cr_addr & 0xffffffff;
	reg->in_frame1_add_ext.bits.plane0_h
				= fb_arg->in_fb1.buf.addr.y_addr >> 32 & 0xff;
	reg->in_frame1_add_ext.bits.plane1_h
				= fb_arg->in_fb1.buf.addr.cb_addr >> 32 & 0xff;
	reg->in_frame1_add_ext.bits.plane2_h
				= fb_arg->in_fb1.buf.addr.cr_addr >> 32 & 0xff;

//set out_fb0
	reg->out_frame0_addr0.dwval = fb_arg->out_fb0.buf.addr.y_addr & 0xffffffff;
	reg->out_frame0_addr1.dwval = fb_arg->out_fb0.buf.addr.cb_addr & 0xffffffff;
	reg->out_frame0_addr2.dwval = fb_arg->out_fb0.buf.addr.cr_addr & 0xffffffff;
	reg->out_frame0_add_ext.bits.plane0_h
				= fb_arg->out_fb0.buf.addr.y_addr >> 32 & 0xff;
	reg->out_frame0_add_ext.bits.plane1_h
				= fb_arg->out_fb0.buf.addr.cb_addr >> 32 & 0xff;
	reg->out_frame0_add_ext.bits.plane2_h
				= fb_arg->out_fb0.buf.addr.cr_addr >> 32 & 0xff;

//set out_fb1
	reg->out_frame1_addr0.dwval = fb_arg->out_fb1.buf.addr.y_addr & 0xffffffff;
	reg->out_frame1_addr1.dwval = fb_arg->out_fb1.buf.addr.cb_addr & 0xffffffff;
	reg->out_frame1_addr2.dwval = fb_arg->out_fb1.buf.addr.cr_addr & 0xffffffff;
	reg->out_frame1_add_ext.bits.plane0_h
				= fb_arg->out_fb1.buf.addr.y_addr >> 32 & 0xff;
	reg->out_frame1_add_ext.bits.plane1_h
				= fb_arg->out_fb1.buf.addr.cb_addr >> 32 & 0xff;
	reg->out_frame1_add_ext.bits.plane2_h
				= fb_arg->out_fb1.buf.addr.cr_addr >> 32 & 0xff;
}

static void di_dev_set_pitches(struct di_reg *reg,
		struct di_process_fb_arg *fb_arg)
{
//in plane0(Y)
	//reg->in_pitch0.bits.fb0_plane0 = DI_ALIGN(fb_arg->in_fb0.size.width, 2);
	//reg->in_pitch0.bits.fb1_plane0 = DI_ALIGN(fb_arg->in_fb1.size.width, 2);
	reg->in_pitch0.dwval = (DI_ALIGN(fb_arg->in_fb1.size.width, 2) << 16)
				| DI_ALIGN(fb_arg->in_fb0.size.width, 2);
//out plane0(Y)
	reg->out_pitch0.bits.plane0 = DI_ALIGN(fb_arg->out_fb0.size.width, 2);

	if (di_get_format_type(fb_arg->pixel_format) == PLANNER_FORMAT) {
//in plane1(U or V)
		reg->in_pitch1.dwval = (DI_ALIGN(fb_arg->in_fb1.size.width / 2, 2) << 16)
				| DI_ALIGN(fb_arg->in_fb0.size.width / 2, 2);

//in plane2(V or U)
		reg->in_pitch2.dwval = (DI_ALIGN(fb_arg->in_fb1.size.width / 2, 2) << 16)
				| DI_ALIGN(fb_arg->in_fb0.size.width / 2, 2);

//out plane1(U or V)
		reg->out_pitch1.dwval
				= DI_ALIGN(fb_arg->out_fb0.size.width / 2, 2);
//out plane2(V or U)
		reg->out_pitch2.dwval
				= DI_ALIGN(fb_arg->out_fb0.size.width / 2, 2);

	} else if (di_get_format_type(fb_arg->pixel_format) == UVCOMBINE_FORMAT) {
//in plane1(U or V)
		reg->in_pitch1.dwval = (DI_ALIGN(fb_arg->in_fb1.size.width, 2) << 16)
					| DI_ALIGN(fb_arg->in_fb0.size.width, 2);

//in plane2(V or U)
		reg->in_pitch2.dwval = 0;

//out plane1(U or V)
		reg->out_pitch1.dwval = DI_ALIGN(fb_arg->out_fb0.size.width, 2);
//out plane2(V or U)
		reg->out_pitch2.dwval = 0;
	}
}

/*set fb
* 1.set pitch,phy address of in/out_fb to reg
*/
static void di_dev_set_fb(struct di_process_fb_arg *fb_arg)
{
	struct di_reg *reg = di_dev_get_reg_base();

	di_dev_set_pitches(reg, fb_arg);
	di_dev_set_addr(reg, fb_arg);
}

/*static*/ bool di_dev_is_busy(void)
{
	struct di_reg *reg = di_dev_get_reg_base();

	return reg->status.bits.busy ? true : false;
}

static void di_dev_reset(void)
{
	struct di_reg *reg = di_dev_get_reg_base();

	reg->ctrl.bits.reset = 1;
	local_delay_us(1);
	reg->ctrl.bits.reset = 0;
}

/*static*/ void di_dev_start(void)
{
	struct di_reg *reg = di_dev_get_reg_base();

	reg->ctrl.bits.start = 1;
}

static void di_dev_apply_fixed_para(void)
{
	struct di_reg *reg = di_dev_get_reg_base();

	reg->ctrl.bits.auto_clk_gate_en = 1;

	//reg->model_para.bits.feather_detection_en = 1;
	reg->model_para.dwval = reg->model_para.dwval | (1  << 8);

	//reg->luma_para.bits.min_luma_th = 0x4;
	//reg->luma_para.bits.max_luma_th = 0xc;
	//reg->luma_para.bits.luma_th_shifter = 0x1;
	//reg->luma_para.bits.avg_luma_shifter = 0x6;
	//reg->luma_para.bits.angle_const_th = 0x5;
	//reg->luma_para.bits.max_angle = 0x6;
	//reg->luma_para.bits.slow_motion_fac = 0x2;
	reg->luma_para.dwval = 0x026561c4;

	reg->chroma_para.bits.chroma_diff_th = 0x4;
	reg->chroma_para.bits.chroma_spatial_th = 0x5;
	reg->chroma_para.dwval = 0x00000054;
}

uint32_t di_dev_get_ip_version(void)
{
	struct di_reg *reg = di_dev_get_reg_base();

	return reg->version.dwval;
}

static void di_dev_enable_irq(uint32_t en)
{
	struct di_reg *reg = di_dev_get_reg_base();

	reg->int_ctrl.bits.finish_int_en = en;
}

uint32_t di_dev_query_state_with_clear(void)
{
	struct di_reg *reg = di_dev_get_reg_base();

	uint32_t val = reg->status.bits.finish_flag;
	reg->status.bits.finish_flag = 1;

	return val;
}

static void di_dev_apply_top_para(struct di_process_fb_arg *fb_arg)
{
	struct di_reg *reg = di_dev_get_reg_base();

	//reg->size.bits.width = DI_ALIGN(fb_arg->size.width, 2) - 1;
	//reg->size.bits.height = DI_ALIGN(fb_arg->size.height, 4) - 1;

	reg->size.dwval = ((DI_ALIGN(fb_arg->size.height, 4) - 1) << 16)
			| (DI_ALIGN(fb_arg->size.width, 2) - 1);

	reg->format.bits.in_format = di_dev_convert_fmt(fb_arg->pixel_format);
	reg->format.bits.field_order = fb_arg->field_order;

	//reg->model_para.bits.di_mode = fb_arg->di_mode;
	//reg->model_para.bits.output_mode = fb_arg->output_mode;
	//reg->model_para.dwval = reg->model_para.dwval | (1  << 8);
	reg->model_para.dwval = (1 << 8) | (fb_arg->output_mode << 4)
					| fb_arg->di_mode;
}

static void di_dev_set_mdflag( /* struct di_client *client */uintptr_t dma_addr,uint32_t width)
{
	struct di_reg *reg = di_dev_get_reg_base();

//	struct di_mapped_buf *md_buf = client->md_buf;
//	struct di_process_fb_arg *fb_arg = &client->fb_arg;

	if (dma_addr /*md_buf*/ ) {
		reg->mdflag_add.dwval = dma_addr;                                     //(uint32_t)md_buf->dma_addr;
		reg->mdflag_pitch.dwval = DI_ALIGN(width /*fb_arg->size.width*/ , 1);
	} else {
		reg->mdflag_add.dwval = 0;
		reg->mdflag_pitch.dwval = 0;
	}

	reg->mdflag_add_ext.dwval = 0;
}

struct di_process_fb_arg FB_ARG;

/* invoked in every di process. */
int32_t di_dev_apply(uint8_t n,uint8_t m)
{
#if 0
	// mgs
	FB_ARG.in_fb0.buf.addr.y_addr =TVD_YBUF[(n+0)%3]; //0,1,2,0,1,2,...
	FB_ARG.in_fb0.buf.addr.cb_addr=TVD_CBUF[(n+0)%3];
	FB_ARG.in_fb0.buf.addr.cr_addr=TVD_CBUF[(n+0)%3];

	FB_ARG.in_fb1.buf.addr.y_addr =TVD_YBUF[(n+1)%3];
	FB_ARG.in_fb1.buf.addr.cb_addr=TVD_CBUF[(n+1)%3];
	FB_ARG.in_fb1.buf.addr.cr_addr=TVD_CBUF[(n+1)%3];

	FB_ARG.out_fb0.buf.addr.y_addr =TVD_YBUF[3+(m&1)]; //3,4,3,4,...
	FB_ARG.out_fb0.buf.addr.cb_addr=TVD_CBUF[3+(m&1)];
	FB_ARG.out_fb0.buf.addr.cr_addr=TVD_CBUF[3+(m&1)];
#else
	{
		const uintptr_t vram = allocate_dmabuffercolmain1fb();
		const uintptr_t vramY = vram;
		const uintptr_t vramC = vram + TVD_SIZE;

		FB_ARG.in_fb0.buf.addr.y_addr = vramY;
		FB_ARG.in_fb0.buf.addr.cb_addr= vramC;
		FB_ARG.in_fb0.buf.addr.cr_addr= vramC;
	}

	{
		const uintptr_t vram = allocate_dmabuffercolmain1fb();
		const uintptr_t vramY = vram;
		const uintptr_t vramC = vram + TVD_SIZE;

		FB_ARG.in_fb1.buf.addr.y_addr = vramY;
		FB_ARG.in_fb1.buf.addr.cb_addr= vramC;
		FB_ARG.in_fb1.buf.addr.cr_addr= vramC;
	}
	{
		const uintptr_t vram = allocate_dmabuffercolmain1fb();
		const uintptr_t vramY = vram;
		const uintptr_t vramC = vram + TVD_SIZE;


		FB_ARG.out_fb0.buf.addr.y_addr =vramY; //3,4,3,4,...
		FB_ARG.out_fb0.buf.addr.cb_addr=vramC;
		FB_ARG.out_fb0.buf.addr.cr_addr=vramC;
	}
#endif
	struct di_reg *reg = di_dev_get_reg_base();

	di_dev_set_addr(reg, &FB_ARG);

	di_dev_start();

	return 0;
}

/* invoked in every di process. */
uintptr_t di_dev_apply2(uintptr_t vram1, uintptr_t vram2, uintptr_t vramOUT)
{
#if 0
	// mgs
	FB_ARG.in_fb0.buf.addr.y_addr =TVD_YBUF[(n+0)%3]; //0,1,2,0,1,2,...
	FB_ARG.in_fb0.buf.addr.cb_addr=TVD_CBUF[(n+0)%3];
	FB_ARG.in_fb0.buf.addr.cr_addr=TVD_CBUF[(n+0)%3];

	FB_ARG.in_fb1.buf.addr.y_addr =TVD_YBUF[(n+1)%3];
	FB_ARG.in_fb1.buf.addr.cb_addr=TVD_CBUF[(n+1)%3];
	FB_ARG.in_fb1.buf.addr.cr_addr=TVD_CBUF[(n+1)%3];

	FB_ARG.out_fb0.buf.addr.y_addr =TVD_YBUF[3+(m&1)]; //3,4,3,4,...
	FB_ARG.out_fb0.buf.addr.cb_addr=TVD_CBUF[3+(m&1)];
	FB_ARG.out_fb0.buf.addr.cr_addr=TVD_CBUF[3+(m&1)];
#else
	uintptr_t old = FB_ARG.out_fb0.buf.addr.y_addr;
	{
		const uintptr_t vramY = vram1;
		const uintptr_t vramC = vram1 + TVD_SIZE;

		FB_ARG.in_fb0.buf.addr.y_addr = vramY;
		FB_ARG.in_fb0.buf.addr.cb_addr= vramC;
		FB_ARG.in_fb0.buf.addr.cr_addr= vramC;
	}

	{
		const uintptr_t vramY = vram2;
		const uintptr_t vramC = vram2 + TVD_SIZE;

		FB_ARG.in_fb1.buf.addr.y_addr = vramY;
		FB_ARG.in_fb1.buf.addr.cb_addr= vramC;
		FB_ARG.in_fb1.buf.addr.cr_addr= vramC;
	}
	{
		const uintptr_t vram = vramOUT;
		const uintptr_t vramY = vram;
		const uintptr_t vramC = vram + TVD_SIZE;


		FB_ARG.out_fb0.buf.addr.y_addr =vramY; //3,4,3,4,...
		FB_ARG.out_fb0.buf.addr.cb_addr=vramC;
		FB_ARG.out_fb0.buf.addr.cr_addr=vramC;
	}
#endif
	struct di_reg *reg = di_dev_get_reg_base();

	di_dev_set_addr(reg, &FB_ARG);

	di_dev_start();

	return old;
}

//---

void DI_Clock(void)
{
 CCU->DI_BGR_REG &= ~ (UINT32_C(1) << 16);       //assert reset

 CCU->DI_CLK_REG =
		 (UINT32_C(1) << 31) |
		 0x00 * (UINT32_C(1) << 24) |
		 (2-1) * (UINT32_C(1) << 0) | //clock on, PLL_PERI(2x), M=2 => 1200/2 = 600 MHz
		 0;

 CCU->DI_BGR_REG |= (UINT32_C(1) << 0);              //pass gate

 CCU->DI_BGR_REG |= (UINT32_C(1) << 16);        //de-assert reset

 PRINTF("DI_Clock: %u MHz\n", (unsigned) (allwnrt113_get_di_freq() / 1000 / 1000));
}

void DI_INIT(void)
{
 DI_Clock();

 di_dev_set_reg_base((void __iomem*)DI_BASE);

 di_dev_reset();

 PRINTF("DI version: 0x%x\n", (unsigned) di_dev_get_ip_version());

 di_dev_apply_fixed_para();

 FB_ARG.size.width=TVD_WIDTH;
 FB_ARG.size.height=TVD_HEIGHT;
 FB_ARG.pixel_format=UVCOMBINE_FORMAT;
 FB_ARG.field_order=1;                 //1: top field first 0:bottom field first
 FB_ARG.output_mode=0;                 //0: 1-frame 1: 2-frame
 FB_ARG.di_mode=0;                     //0: motion adaptive mode  1:inter field interpolation mode

 FB_ARG.in_fb0.size.width=TVD_WIDTH;
 FB_ARG.in_fb1.size.width=TVD_WIDTH;

 FB_ARG.out_fb0.size.width=TVD_WIDTH;
 FB_ARG.out_fb1.size.width=TVD_WIDTH;

 di_dev_apply_top_para(&FB_ARG);
 di_dev_set_fb(&FB_ARG);

#if 1
 // mgs
 //di_dev_set_mdflag(FB_ARG.di_mode?0:TVD_YBUF[5],TVD_WIDTH);
 di_dev_set_mdflag((uintptr_t) 0, TVD_WIDTH);
#endif

 di_dev_enable_irq(1);
}

#endif /* WIHSTVDHW */
