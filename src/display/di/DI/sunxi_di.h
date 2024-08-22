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

#ifndef _UAPI_SUNXI_DI_H_
#define _UAPI_SUNXI_DI_H_

#include <stdbool.h>

enum {
	DI_INTP_MODE_INVALID = 0x0,
	DI_INTP_MODE_BOB,
	DI_INTP_MODE_MOTION,
};

enum {
	DI_OUT_0FRAME = 0x0,
	DI_OUT_1FRAME,
	DI_OUT_2FRAME,
};

enum {
	DI_TNR_MODE_INVALID = 0,
	DI_TNR_MODE_ADAPTIVE,
	DI_TNR_MODE_FIX,
};

enum {
	DI_TNR_LEVEL_HIGH = 0,
	DI_TNR_LEVEL_MIDDLE,
	DI_TNR_LEVEL_LOW,
};

enum {
	DI_BOTTOM_FIELD_FIRST = 0,
	DI_TOP_FIELD_FIRST = 1,
};

struct di_version {
	uint32_t version_major;
	uint32_t version_minor;
	uint32_t version_patchlevel;

	uint32_t ip_version;
};

struct di_timeout_ns {
	uint64_t wait4start;
	uint64_t wait4finish;
};

/*
 * @mode:
 * @level:
 */
struct di_tnr_mode {
	uint32_t mode;
	uint32_t level;
};

struct di_size {
	uint32_t width;
	uint32_t height;
};

struct di_rect {
	uint32_t left;
	uint32_t top;
	uint32_t right;
	uint32_t bottom;
};

struct di_addr {
	uint64_t y_addr;
	uint64_t cb_addr;
	uint64_t cr_addr;
};

struct di_offset {
	uint64_t y_offset;
	uint64_t cb_offset;
	uint64_t cr_offset;
};

/*
 * support dma_buf method or phy_addr_buf method.
 * 1.On dma_buf method: use di_offset
 * 2.On phy_addr_buf method: use di_addr
 */
union di_buf {
	struct di_addr addr;
	struct di_offset offset;
};

/*
 * @format: see DRM_FORMAT_XXX in drm_fourcc.h.
 * @dma_buf_fd: dma buf fd that from userspace.
 *    @dma_buf_fd must be invalid(<0) on phy_addr_buf method.
 * @size.width,@size.height: size of pixel datas of image. unit: pixel.
 */
struct di_fb {
	int32_t dma_buf_fd;
	union di_buf buf;
	struct di_size size;
};

struct di_process_fb_arg {
	uint8_t is_interlace;
	uint8_t field_order; /*1:top field first 0:bottom field first*/
	uint32_t pixel_format;
	struct di_size size; /*size of source interlace picture*/

	uint8_t output_mode; /*0: 1-frame 1: 2-frame*/
	uint8_t di_mode; /*0:motion adaptive mode  1:inter field interpolation mode*/

	struct di_tnr_mode tnr_mode;

	struct di_fb in_fb0;
	struct di_fb in_fb1;
	struct di_fb in_fb2;

	struct di_fb out_fb0;
	struct di_fb out_fb1;
	struct di_fb out_tnr_fb;

};

struct di_mem_arg {
	unsigned int size;
	unsigned int handle;
	uint64_t phys_addr;
};

#define DI_IOC_MAGIC 'D'
#define DI_IO(nr)          _IO(DI_IOC_MAGIC, nr)
#define DI_IOR(nr, size)   _IOR(DI_IOC_MAGIC, nr, size)
#define DI_IOW(nr, size)   _IOW(DI_IOC_MAGIC, nr, size)
#define DI_IOWR(nr, size)  _IOWR(DI_IOC_MAGIC, nr, size)
#define DI_IOCTL_NR(n)     _IOC_NR(n)

#define DI_IOC_GET_VERSION    DI_IOR(0x0, struct di_version)
/*
#define DI_IOC_RESET          DI_IO(0x1)
#define DI_IOC_CHECK_PARA     DI_IO(0x2)
*/
#define DI_IOC_SET_TIMEOUT    DI_IOW(0x3, struct di_timeout_ns)
/*
#define DI_IOC_SET_VIDEO_SIZE DI_IOW(0x4, struct di_size)
#define DI_IOC_SET_DIT_MODE   DI_IOW(0x5, struct di_dit_mode)
#define DI_IOC_SET_TNR_MODE   DI_IOW(0x6, struct di_tnr_mode)
#define DI_IOC_SET_FMD_ENABLE DI_IOW(0x7, struct di_fmd_enable)
*/
#define DI_IOC_PROCESS_FB     DI_IOW(0x8, struct di_process_fb_arg)
//#define DI_IOC_SET_VIDEO_CROP DI_IOW(0x9, struct di_rect)
#define DI_IOC_MEM_REQUEST    DI_IOWR(0x10, struct di_mem_arg)
#define DI_IOC_MEM_RELEASE    DI_IOWR(0x11, struct di_mem_arg)
//#define DI_IOC_SET_DEMO_CROP  DI_IOW(0x12, struct di_demo_crop_arg)

extern unsigned int di_device_get_debug_mode(void);

#define DI_INT_NUMBER 120

void DI_INIT(void);
int32_t di_dev_apply(uint8_t n,uint8_t m);
bool di_dev_is_busy(void);
uint32_t di_dev_query_state_with_clear(void);
int32_t di_dev_apply2(uintptr_t vram1, uintptr_t vram2);

#endif /* #ifndef _UAPI_SUNXI_DI_H_ */
