/*
 * hdmi_glue.h
 *
 *  Created on: Mar 1, 2026
 *      Author: Gena
 */

#ifndef INC_HDMI_GLUE_H_
#define INC_HDMI_GLUE_H_


#include "hardware.h"
#include "formats.h"
#include <string.h>
#include <stdio.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uintptr_t dma_addr_t;

typedef uint16_t __le16;


#define DRM_INFO PRINTF
#define hdmi_trace PRINTF

#define udelay(us) do { local_delay_us((us)); } while (0)
#define mdelay(ms) do { local_delay_ms((ms)); } while (0)

enum hdmi_3d_structure {
	HDMI_3D_STRUCTURE_INVALID = -1,
	HDMI_3D_STRUCTURE_FRAME_PACKING = 0,
	HDMI_3D_STRUCTURE_FIELD_ALTERNATIVE,
	HDMI_3D_STRUCTURE_LINE_ALTERNATIVE,
	HDMI_3D_STRUCTURE_SIDE_BY_SIDE_FULL,
	HDMI_3D_STRUCTURE_L_DEPTH,
	HDMI_3D_STRUCTURE_L_DEPTH_GFX_GFX_DEPTH,
	HDMI_3D_STRUCTURE_TOP_AND_BOTTOM,
	HDMI_3D_STRUCTURE_SIDE_BY_SIDE_HALF = 8,
};


#define HDMI_IEEE_OUI 0x000c03
#define HDMI_FORUM_IEEE_OUI 0xc45dd8
#define HDMI_INFOFRAME_HEADER_SIZE  4
#define HDMI_AVI_INFOFRAME_SIZE    13
#define HDMI_SPD_INFOFRAME_SIZE    25
#define HDMI_AUDIO_INFOFRAME_SIZE  10

#ifdef BIT
#undef BIT
#define BIT(n) (1UL << (n))
#else
#define BIT(n) (1UL << (n))
#endif

struct detailed_pixel_timing {
	u8 hactive_lo;
	u8 hblank_lo;
	u8 hactive_hblank_hi;
	u8 vactive_lo;
	u8 vblank_lo;
	u8 vactive_vblank_hi;
	u8 hsync_offset_lo;
	u8 hsync_pulse_width_lo;
	u8 vsync_offset_pulse_width_lo;
	u8 hsync_vsync_offset_pulse_width_hi;
	u8 width_mm_lo;
	u8 height_mm_lo;
	u8 width_height_mm_hi;
	u8 hborder;
	u8 vborder;
	u8 misc;
} __attribute__((packed));


struct detailed_data_string {
	u8 str[13];
} __attribute__((packed));

struct detailed_data_monitor_range {
	u8 min_vfreq;
	u8 max_vfreq;
	u8 min_hfreq_khz;
	u8 max_hfreq_khz;
	u8 pixel_clock_mhz;
	u8 flags;
	union {
		struct {
			u8 reserved;
			u8 hfreq_start_khz;
			u8 c;
			__le16 m;
			u8 k;
			u8 j;
		} __attribute__((packed)) gtf2;
		struct {
			u8 version;
			u8 data1;
			u8 data2;
			u8 supported_aspects;
			u8 flags;
			u8 supported_scalings;
			u8 preferred_refresh;
		} __attribute__((packed)) cvt;
	} formula;
} __attribute__((packed));

struct detailed_data_wpindex {
	u8 white_yx_lo;
	u8 white_x_hi;
	u8 white_y_hi;
	u8 gamma;
} __attribute__((packed));

struct detailed_data_color_point {
	u8 windex1;
	u8 wpindex1[3];
	u8 windex2;
	u8 wpindex2[3];
} __attribute__((packed));

struct cvt_timing {
	u8 code[3];
} __attribute__((packed));

struct detailed_non_pixel {
	u8 pad1;
	u8 type;
	u8 pad2;
	union {
		struct detailed_data_string str;
		struct detailed_data_monitor_range range;
		struct detailed_data_wpindex color;
		struct std_timing timings[6];
		struct cvt_timing cvt[4];
	} data;
} __attribute__((packed));

struct edid {
	u8 header[8];

	u8 mfg_id[2];
	u8 prod_code[2];
	u32 serial;
	u8 mfg_week;
	u8 mfg_year;

	u8 version;
	u8 revision;

	u8 input;
	u8 width_cm;
	u8 height_cm;
	u8 gamma;
	u8 features;

	u8 red_green_lo;
	u8 black_white_lo;
	u8 red_x;
	u8 red_y;
	u8 green_x;
	u8 green_y;
	u8 blue_x;
	u8 blue_y;
	u8 white_x;
	u8 white_y;

	struct est_timings established_timings;

	struct std_timing standard_timings[8];

	struct detailed_timing detailed_timings[4];

	u8 extensions;

	u8 checksum;
} __attribute__((packed));


#endif /* INC_HDMI_GLUE_H_ */
