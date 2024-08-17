/*
 * Allwinner SoCs tv driver.
 *
 * Copyright (C) 2016 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#ifndef __DE_TVE_H__
#define __DE_TVE_H__

#define TVE_DEVICE_NUM 1
#define TVE_TOP_DEVIVE_NUM 1
#define TVE_DAC_NUM 1

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

//#include "Type.h"

#define __iomem /*volatile*/

enum disp_tv_mode
{
	DISP_TV_MOD_480I                = 0,
	DISP_TV_MOD_576I                = 1,
	DISP_TV_MOD_480P                = 2,
	DISP_TV_MOD_576P                = 3,
	DISP_TV_MOD_720P_50HZ           = 4,
	DISP_TV_MOD_720P_60HZ           = 5,
	DISP_TV_MOD_1080I_50HZ          = 6,
	DISP_TV_MOD_1080I_60HZ          = 7,
	DISP_TV_MOD_1080P_24HZ          = 8,
	DISP_TV_MOD_1080P_50HZ          = 9,
	DISP_TV_MOD_1080P_60HZ          = 0xa,
	DISP_TV_MOD_1080P_24HZ_3D_FP    = 0x17,
	DISP_TV_MOD_720P_50HZ_3D_FP     = 0x18,
	DISP_TV_MOD_720P_60HZ_3D_FP     = 0x19,
	DISP_TV_MOD_1080P_25HZ          = 0x1a,
	DISP_TV_MOD_1080P_30HZ          = 0x1b,
	DISP_TV_MOD_PAL                 = 0xb,
	DISP_TV_MOD_PAL_SVIDEO          = 0xc,
	DISP_TV_MOD_NTSC                = 0xe,
	DISP_TV_MOD_NTSC_SVIDEO         = 0xf,
	DISP_TV_MOD_PAL_M               = 0x11,
	DISP_TV_MOD_PAL_M_SVIDEO        = 0x12,
	DISP_TV_MOD_PAL_NC              = 0x14,
	DISP_TV_MOD_PAL_NC_SVIDEO       = 0x15,
	DISP_TV_MOD_3840_2160P_30HZ     = 0x1c,
	DISP_TV_MOD_3840_2160P_25HZ     = 0x1d,
	DISP_TV_MOD_3840_2160P_24HZ     = 0x1e,
	DISP_TV_MOD_4096_2160P_24HZ     = 0x1f,
	DISP_TV_MOD_4096_2160P_25HZ     = 0x20,
	DISP_TV_MOD_4096_2160P_30HZ     = 0x21,
	DISP_TV_MOD_3840_2160P_60HZ     = 0x22,
	DISP_TV_MOD_4096_2160P_60HZ     = 0x23,
	DISP_TV_MOD_3840_2160P_50HZ     = 0x24,
	DISP_TV_MOD_4096_2160P_50HZ     = 0x25,
	DISP_TV_MOD_2560_1440P_60HZ     = 0x26,
	DISP_TV_MOD_1440_2560P_70HZ     = 0x27,
	DISP_TV_MOD_1080_1920P_60HZ	= 0x28,
	DISP_TV_MOD_1280_1024P_60HZ     = 0x41,
	DISP_TV_MOD_1024_768P_60HZ      = 0x42,
	DISP_TV_MOD_900_540P_60HZ       = 0x43,
	DISP_TV_MOD_1920_720P_60HZ      = 0x44,

	/*Just for the solution of hdmi edid detailed timiming block*/
	DISP_HDMI_MOD_DT0                = 0x4a,
	DISP_HDMI_MOD_DT1                = 0x4b,
	DISP_HDMI_MOD_DT2                = 0x4c,
	DISP_HDMI_MOD_DT3                = 0x4d,
	/*
	 * vga
	 * NOTE:macro'value of new solution must between
	 * DISP_VGA_MOD_640_480P_60 and DISP_VGA_MOD_MAX_NUM
	 * or you have to modify is_vag_mode function in drv_tv.h
	 */
	DISP_VGA_MOD_640_480P_60         = 0x50,
	DISP_VGA_MOD_800_600P_60         = 0x51,
	DISP_VGA_MOD_1024_768P_60        = 0x52,
	DISP_VGA_MOD_1280_768P_60        = 0x53,
	DISP_VGA_MOD_1280_800P_60        = 0x54,
	DISP_VGA_MOD_1366_768P_60        = 0x55,
	DISP_VGA_MOD_1440_900P_60        = 0x56,
	DISP_VGA_MOD_1920_1080P_60       = 0x57,
	DISP_VGA_MOD_1920_1200P_60 = 0x58,
	DISP_TV_MOD_3840_1080P_30 = 0x59,
	DISP_VGA_MOD_1280_720P_60        = 0x5a,
	DISP_VGA_MOD_1600_900P_60        = 0x5b,
	DISP_VGA_MOD_MAX_NUM             = 0x5c,

	DISP_TV_MODE_NUM
};

/* tv encoder registers offset */
#define TVE_000    (0x000)
#define TVE_004    (0x004)
#define TVE_008    (0x008)
#define TVE_00C    (0x00c)
#define TVE_010    (0x010)
#define TVE_014    (0x014)
#define TVE_018    (0x018)
#define TVE_01C    (0x01c)
#define TVE_020    (0x020)
#define TVE_024    (0x024)
#define TVE_030    (0X030)
#define TVE_034    (0x034)
#define TVE_038    (0x038)
#define TVE_03C    (0x03c)
#define TVE_040    (0x040)
#define TVE_044    (0x044)
#define TVE_048    (0x048)
#define TVE_04C    (0x04c)
#define TVE_0F8    (0x0f8)
#define TVE_0FC    (0x0fc)
#define TVE_100    (0x100)
#define TVE_104    (0x104)
#define TVE_108    (0x108)
#define TVE_10C    (0x10c)
#define TVE_110    (0x110)
#define TVE_114    (0x114)
#define TVE_118    (0x118)
#define TVE_11C    (0x11c)
#define TVE_120    (0x120)
#define TVE_124    (0x124)
#define TVE_128    (0x128)
#define TVE_12C    (0x12c)
#define TVE_130    (0x130)
#define TVE_134    (0x134)
#define TVE_138    (0x138)
#define TVE_13C    (0x13C)
#define TVE_300    (0x300)
#define TVE_304    (0x304)
#define TVE_308    (0x308)
#define TVE_30C    (0x30c)
#define TVE_380    (0x380)
#define TVE_3A0    (0x3a0)

#define TVE_TOP_020    (0x020)
#define TVE_TOP_024    (0x024)
#define TVE_TOP_028    (0x028)
#define TVE_TOP_02C    (0x02C)
#define TVE_TOP_030    (0x030)
#define TVE_TOP_034    (0x034)
#define TVE_TOP_0F0    (0x0F0)

#define TVE_GET_REG_BASE(sel)                   (TV_Encoder_BASE)
#define TVE_WUINT32(sel,offset,value)           (*((volatile uint32_t *)( TVE_GET_REG_BASE(sel) + (offset) ))=(value))
#define TVE_RUINT32(sel,offset)                 (*((volatile uint32_t *)( TVE_GET_REG_BASE(sel) + (offset) )))
#define TVE_SET_BIT(sel,offset,bit)             (*((volatile uint32_t *)( TVE_GET_REG_BASE(sel) + (offset) )) |= (bit))
#define TVE_CLR_BIT(sel,offset,bit)             (*((volatile uint32_t *)( TVE_GET_REG_BASE(sel) + (offset) )) &= (~(bit)))
#define TVE_INIT_BIT(sel,offset,c,s)            (*((volatile uint32_t *)( TVE_GET_REG_BASE(sel) + (offset) )) = \
                                                (((*(volatile uint32_t *)( TVE_GET_REG_BASE(sel) + (offset) )) & (~(c))) | (s)))

#define TVE_TOP_GET_REG_BASE                    (TVE_TOP_BASE)
#define TVE_TOP_WUINT32(offset,value)           (*((volatile uint32_t *)( TVE_TOP_GET_REG_BASE + (offset) ))=(value))
#define TVE_TOP_RUINT32(offset)                 (*((volatile uint32_t *)( TVE_TOP_GET_REG_BASE + (offset) )))
#define TVE_TOP_SET_BIT(offset,bit)             (*((volatile uint32_t *)( TVE_TOP_GET_REG_BASE + (offset) )) |= (bit))
#define TVE_TOP_CLR_BIT(offset,bit)             (*((volatile uint32_t *)( TVE_TOP_GET_REG_BASE + (offset) )) &= (~(bit)))
#define TVE_TOP_INIT_BIT(offset,c,s)            (*((volatile uint32_t *)( TVE_TOP_GET_REG_BASE + (offset) )) = \
                                               (((*(volatile uint32_t *)( TVE_TOP_GET_REG_BASE + (offset) )) & (~(c))) | (s)))

int32_t tve_low_set_reg_base(uint32_t sel, void __iomem *address);           //
int32_t tve_low_set_top_reg_base(void __iomem *address);                //
int32_t tve_low_set_sid_base(void __iomem *address);
int32_t tve_low_init(uint32_t sel /* , uint32_t *dac_no, uint32_t *cali, int32_t *offset, uint32_t *dac_type, uint32_t num */ );
int32_t tve_low_open(uint32_t sel);                                          //
int32_t tve_low_close(uint32_t sel);
int32_t tve_resync_enable(uint32_t sel);
int32_t tve_resync_disable(uint32_t sel);
int32_t tve_low_set_tv_mode(uint32_t sel, enum disp_tv_mode mode, uint32_t cali); //
int32_t tve_low_get_dac_status(uint32_t sel);
int32_t tve_low_dac_autocheck_enable(uint32_t sel);
int32_t tve_low_dac_autocheck_disable(uint32_t sel);                         //
uint32_t tve_low_get_sid(uint32_t index);
int32_t tve_low_enhance(uint32_t sel, uint32_t mode);
int32_t tve_low_dac_enable(uint32_t sel);                                    //
//extern uint sid_read_key(uint key_index);

#endif
