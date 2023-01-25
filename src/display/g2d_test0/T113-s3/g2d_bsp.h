/* g2d_bsp.h
 *
 * Copyright (c)	2016 Allwinnertech Co., Ltd.
 *					2016 gqs
 *
 * G2D driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 */

#ifndef __G2D_BSP_H
#define __G2D_BSP_H

#include "hardware.h"
#include "formats.h"

/*
#include "linux/kernel.h"
#include "linux/mm.h"
#include <asm/uaccess.h>
#include <asm/memory.h>
#include <asm/unistd.h>
#include "linux/semaphore.h"
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/dma-mapping.h>
#include <linux/fb.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include "asm-generic/int-ll64.h"
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_iommu.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/string.h>
#include <linux/g2d_driver.h>
#include <linux/dma-buf.h>
#include <linux/reset.h>
*/

#include "../../g2d_driver.h"

#include "do_div.h"

#include <stddef.h>

#include "types2.h"

//#include "UART.h"
//#include "LowLevel.h"

#define EINVAL 22 /* Invalid argument */

#if 0

#include <stdio.h>

#define G2D_INFO_MSG PRINTF

#define pr_info PRINTF
#define pr_err  PRINTF
#define pr_warn PRINTF

#else

#define G2D_INFO_MSG(...) do { } while (0)

#define pr_info(...) do { } while (0)
#define pr_err PRINTF
#define pr_warn PRINTF

#endif

/*

extern u32 dbg_info;

#define G2D_INFO_MSG(fmt, args...) \
	do {\
		if (dbg_info)\
		pr_info("[G2D-%s] line:%d: " fmt, __func__, __LINE__, ##args);\
	} while (0)
*/

typedef struct {
	unsigned long g2d_base;
} g2d_init_para;

typedef struct {
	g2d_init_para init_para;
} g2d_dev_t;

typedef enum {
	G2D_RGB2YUV_709,
	G2D_YUV2RGB_709,
	G2D_RGB2YUV_601,
	G2D_YUV2RGB_601,
	G2D_RGB2YUV_2020,
	G2D_YUV2RGB_2020,
} g2d_csc_sel;

typedef enum {
	VSU_FORMAT_YUV422 = 0x00,
	VSU_FORMAT_YUV420 = 0x01,
	VSU_FORMAT_YUV411 = 0x02,
	VSU_FORMAT_RGB = 0x03,
	VSU_FORMAT_BUTT = 0x04,
} vsu_pixel_format;

#define VSU_ZOOM0_SIZE	1
#define VSU_ZOOM1_SIZE	8
#define VSU_ZOOM2_SIZE	4
#define VSU_ZOOM3_SIZE	1
#define VSU_ZOOM4_SIZE	1
#define VSU_ZOOM5_SIZE	1

#define VSU_PHASE_NUM            32
#define VSU_PHASE_FRAC_BITWIDTH  19
#define VSU_PHASE_FRAC_REG_SHIFT 1
#define VSU_FB_FRAC_BITWIDTH     32

#define VI_LAYER_NUMBER 1
#define UI_LAYER_NUMBER 3

int32_t g2d_bsp_open(void);
int32_t g2d_bsp_close(void);
int32_t g2d_bsp_reset(void);
int32_t mixer_irq_query(void);
int32_t rot_irq_query(void);
int32_t g2d_mixer_reset(void);
int32_t g2d_rot_reset(void);
int32_t g2d_bsp_bld(g2d_image_enh *, g2d_image_enh *, uint32_t, g2d_ck *);
int32_t g2d_fillrectangle(g2d_image_enh *dst, uint32_t color_value);
int32_t g2d_bsp_maskblt(g2d_image_enh *src, g2d_image_enh *ptn,
		      g2d_image_enh *mask, g2d_image_enh *dst,
		      uint32_t back_flag, uint32_t fore_flag);
int32_t g2d_bsp_bitblt(g2d_image_enh *src, g2d_image_enh *dst, uint32_t flag);
int32_t g2d_byte_cal(uint32_t format, uint32_t *ycnt, uint32_t *ucnt, uint32_t *vcnt);

extern int g2d_wait_cmd_finish(void);

uint32_t	mixer_reg_init(void);
int32_t	mixer_blt(g2d_blt *para, enum g2d_scan_order scan_order);
int32_t	mixer_fillrectangle(g2d_fillrect *para);
int32_t	mixer_stretchblt(g2d_stretchblt *para, enum g2d_scan_order scan_order);
int32_t	mixer_maskblt(g2d_maskblt *para);
uint32_t	mixer_set_palette(g2d_palette *para);
uint64_t	mixer_get_addr(uint32_t buffer_addr, uint32_t format,
			uint32_t stride, uint32_t x, uint32_t y);
uint32_t	mixer_set_reg_base(unsigned long addr);
uint32_t	mixer_get_irq(void);
uint32_t	mixer_get_irq0(void);
uint32_t	mixer_clear_init(void);
uint32_t	mixer_clear_init0(void);
int32_t	mixer_cmdq(uint32_t addr);
uint32_t	mixer_premultiply_set(uint32_t flag);
uint32_t	mixer_micro_block_set(g2d_blt *para);

#endif	/* __G2D_BSP_H */

