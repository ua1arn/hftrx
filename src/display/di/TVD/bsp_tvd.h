/*
 * drivers/media/platform/sunxi-tvd/bsp_tvd/bsp_tvd.h
 *
 * Copyright (c) 2007-2018 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef BSP_TVD_H_
#define BSP_TVD_H_

#include <stdint.h>
#include <stdlib.h>

#define msleep mdelay

#define kmalloc(a,b) malloc(a)
#define kfree free

#define pr_warn(...)

#define FLITER_NUM 1
#define TVD_3D_COMP_BUFFER_SIZE (0x400000)
#define CVBS_INTERFACE 0
#define YPBPRI_INTERFACE 1
#define YPBPRP_INTERFACE 2
#define NTSC 0
#define PAL 1
#define NONE 2


//
// detail information of registers
//

typedef union {
	uint32_t dwval;
	struct {
		uint32_t tvd_adc_map                                              :  2 ;    // default: 0x0;
		uint32_t res0                                                     : 2 ;
		uint32_t tvin_sel                                                 : 1 ;
		uint32_t res1                                                     : 27 ;
	} bits;
} tvd_top_map_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t tvd_en_3d_dma                                            :  1 ;    // default: 0x0;
		uint32_t comb_3d_en                                               :  1 ;    // default: 0x1;
		uint32_t res0                                                     :  2 ;    // default: ;
		uint32_t comb_3d_sel                                              :  2 ;    // default: 0x1;
		uint32_t res1                                                     : 26 ;    // default: ;
	} bits;
} tvd_3d_ctl1_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t dram_trig                                                     ;    // default: 0x0;
	} bits;
} tvd_3d_ctl2_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t comb_3d_addr0                                                 ;    // default: 0x0;
	} bits;
} tvd_3d_ctl3_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t comb_3d_addr1                                                 ;    // default: 0x0;
	} bits;
} tvd_3d_ctl4_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t comb_3d_size                                                  ;    // default: 0x0;
	} bits;
} tvd_3d_ctl5_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t res0:4;
		uint32_t lpf_dig_en:1;
		uint32_t res1:19;
		uint32_t lpf_dig_sel:1;
		uint32_t res2:7;
	} bits;
} tvd_adc_dig_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t adc_en                                                   :  1 ;    // default: 0x0;
		uint32_t afe_en                                                   :  1 ;    // default: 0x0;
		uint32_t lpf_en                                                   :  1 ;    // default: 0x0;
		uint32_t lpf_sel                                                  :  2 ;    // default: 0x0;
		uint32_t res0                                                     : 27 ;    // default: ;
	} bits;
} tvd_adc_ctl_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t stage1_ibias                                             :  2 ;    // default: 0x0;
		uint32_t stage2_ibias                                             :  2 ;    // default: 0x0;
		uint32_t stage3_ibias                                             :  2 ;    // default: 0x0;
		uint32_t stage4_ibias                                             :  2 ;    // default: 0x0;
		uint32_t stage5_ibias                                             :  2 ;    // default: 0x0;
		uint32_t stage6_ibias                                             :  2 ;    // default: 0x0;
		uint32_t stage7_ibias                                             :  2 ;    // default: 0x0;
		uint32_t stage8_ibias                                             :  2 ;    // default: 0x0;
		uint32_t clp_step                                                 :  3 ;    // default: 0x0;
		uint32_t res0                                                     :  9 ;    // default: ;
		uint32_t data_dly                                                 :  1 ;    // default: 0x0;
		uint32_t res1                                                     :  2 ;    // default: ;
		uint32_t adc_test                                                 :  1 ;    // default: 0x0;
	} bits;
} tvd_adc_cfg_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t adc_wb_length                                            : 23 ;    // default: 0x10000;
		uint32_t res0                                                     :  1 ;    // default: ;
		uint32_t adc_wb_start                                             :  1 ;    // default: 0x0;
		uint32_t res1                                                     :  3 ;    // default: ;
		uint32_t adc_wb_buffer_reset                                      :  1 ;    // default: 0x0;
		uint32_t adc_dump_mode                                            :  1 ;    // default: ;
		uint32_t adc_test_mode                                            :  1 ;    // default: 0x0;
		uint32_t fifo_err                                                 :  1 ;    // default: 0x0;
	} bits;
} tvd_adc_dump_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t tvd_en_ch                                                :  1 ;    // default: 0x0;
		uint32_t res0                                                     : 14 ;    // default: ;
		uint32_t clr_rsmp_fifo                                            :  1 ;    // default: 0x0;
		uint32_t res1                                                     :  9 ;    // default: ;
		uint32_t en_lock_disable_write_back1only_start_wb_when_locked     :  1 ;    // default: 0x0;
		uint32_t en_lock_disable_write_back2when_unlocked                 :  1 ;    // default: 0x0;
		uint32_t res2                                                     :  5 ;    // default: ;
	} bits;
} tvd_en_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t ypbpr_mode                                               :  1 ;    // default: 0x0;
		uint32_t svideo_mode                                              :  1 ;    // default: 0x0;
		uint32_t progressive_mode                                         :  1 ;    // default: 0x0;
		uint32_t res0                                                     :  1 ;    // default: ;
		uint32_t blue_display_mode                                        :  2 ;    // default: 0x2;
		uint32_t res1                                                     :  2 ;    // default: ;
		uint32_t blue_color                                               :  1 ;	   // default: 0x0
		uint32_t res2                                                     : 23 ;    // default: ;
	} bits;
} tvd_mode_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t agc_en                                                   :  1 ;    // default: 0x1;
		uint32_t agc_frequence                                            :  1 ;    // default: 0x0;
		uint32_t res0                                                     :  6 ;    // default: ;
		uint32_t agc_target                                               :  8 ;    // default: 0xdd;
		uint32_t cagc_en                                                  :  1 ;    // default: 0x0;
		uint32_t res1                                                     :  7 ;    // default: ;
		uint32_t cagc_target                                              :  8 ;    // default: 0x8a;
	} bits;
} tvd_clamp_agc1_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t agc_gate_width                                           :  7 ;    // default: 0x40;
		uint32_t res0                                                     :  1 ;    // default: ;
		uint32_t agc_backporch_delay                                      :  8 ;    // default: 0x64;
		uint32_t agc_gate_begin                                           : 13 ;    // default: 0x0;
		uint32_t res1                                                     :  2 ;    // default: ;
		uint32_t black_level_clamp                                        :  1 ;    // default: 0x1;
	} bits;
} tvd_clamp_agc2_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t h_sample_step                                                 ;    // default: 0x20000000;
	} bits;
} tvd_hlock1_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t htol                                                     :  4 ;    // default: 0x0;
		uint32_t res0                                                     : 12 ;    // default: ;
		uint32_t hsync_filter_gate_start_time                             :  8 ;    // default: 0xd6;
		uint32_t hsync_filter_gate_end_time                               :  8 ;    // default: 0x4e;
	} bits;
} tvd_hlock2_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t hsync_rising_detect_window_start_time                    :  8 ;    // default: 0x0;
		uint32_t hsync_rising_detect_window_end_time                      :  8 ;    // default: 0x0;
		uint32_t hsync_tip_detect_window_start_time                       :  8 ;    // default: 0x0;
		uint32_t hsync_tip_detect_window_end_time                         :  8 ;    // default: 0x0;
	} bits;
} tvd_hlock3_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t hsync_detect_window_start_time_for_coarse_detection      :  8 ;    // default: 0x00;
		uint32_t hsync_detect_window_end_time_for_corase_detect           :  8 ;    // default: 0x0;
		uint32_t hsync_rising_time_for_fine_detect                        :  8 ;    // default: 0x0;
		uint32_t hsync_fine_to_coarse_offset                              :  8 ;    // default: 0x0;
	} bits;
} tvd_hlock4_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t hactive_start                                            :  8 ;    // default: 0x0;
		uint32_t hactive_width                                            :  8 ;    // default: 0x0;
		uint32_t backporch_detect_window_start_time                       :  8 ;    // default: 0x0;
		uint32_t backporch_detect_window_end_time                         :  8 ;    // default: 0x0;
	} bits;
} tvd_hlock5_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t vtol                                                     :  3 ;    // default: 0x0;
		uint32_t res0                                                     :  1 ;    // default: ;
		uint32_t vactive_start                                            : 11 ;    // default: 0x22;
		uint32_t res1                                                     :  1 ;    // default: ;
		uint32_t vactive_height                                           : 11 ;    // default: 0;
		uint32_t res2                                                     :  5 ;    // default: ;
	} bits;
} tvd_vlock1_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t hsync_dectector_disable_start_line                       :  7 ;    // default: 0x0;
		uint32_t res0                                                     :  9 ;    // default: ;
		uint32_t hsync_detector_disable_end_line                          :  5 ;    // default: 0x0;
		uint32_t res1                                                     : 11 ;    // default: ;
	} bits;
} tvd_vlock2_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t color_kill_en                                            :  1 ;    // default: 0x1;
		uint32_t color_std                                                :  3 ;    // default: 0x0;
		uint32_t res0                                                     :  4 ;    // default: ;
		uint32_t burst_gate_start_time                                    :  8 ;    // default: 0x0;
		uint32_t burst_gate_end_time                                      :  8 ;    // default: 0x0;
		uint32_t wide_burst_gate                                          :  1 ;    // default: 0x0;
		uint32_t res1                                                     :  1 ;    // default: ;
		uint32_t chroma_lpf                                               :  2 ;    // default: 0x0;
		uint32_t color_std_ntsc                                           :  1 ;    // default: 0x0;
		uint32_t res2                                                     :  3 ;    // default: ;
	} bits;
} tvd_clock1_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t c_sample_step                                                 ;    // default: 0x0;
	} bits;
} tvd_clock2_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t _3d_comb_filter_mode                                  	:  3 ;    // default: 0x1;
		uint32_t _3d_comb_filter_dis                                   	:  1 ;    // default: 0x0;
		uint32_t _2d_comb_filter_mode                                  	:  4 ;    // default: 0x0;
		uint32_t secam_notch_wide                                        :  1 ;    // default: 0x0;
		uint32_t chroma_bandpass_filter_en                               :  1 ;    // default: 0x1;
		uint32_t pal_chroma_level                                        :  6 ;    // default: 0x0;
		uint32_t comb_filter_buffer_clear                                :  1 ;    // default: 0x0;
		uint32_t res0                                                    :  3 ;    // default: ;
		uint32_t notch_factor                                            :  3 ;    // default: 0x0;
		uint32_t _2d_comb_factor                                        	:  3 ;    // default: 0x0;
		uint32_t _3d_comb_factor                                       	:  3 ;    // default: 0x0;
		uint32_t chroma_coring_enable                                    :  1 ;    // default: 0x1;
		uint32_t res1                                                    :  2 ;    // default: ;
	} bits;
} tvd_yc_sep1_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t horizontal_luma_filter_gain                              :  2 ;    // default: 0x3;
		uint32_t horizontal_chroma_filter_gain                            :  2 ;    // default: 0x3;
		uint32_t luma_vertical_filter_gain                                :  2 ;    // default: 0x2;
		uint32_t chroma_vertical_filter_gain                              :  2 ;    // default: 0x2;
		uint32_t motion_detect_noise_detect_en                            :  1 ;    // default: 0x0;
		uint32_t motion_detect_noise_threshold                            :  7 ;    // default: 0x0;
		uint32_t noise_detect_en                                          :  1 ;    // default: 0x0;
		uint32_t noise_threshold                                          :  7 ;    // default: 0x0;
		uint32_t luma_noise_factor                                        :  2 ;    // default: 0x3;
		uint32_t chroma_noise_factor                                      :  2 ;    // default: 0x3;
		uint32_t burst_noise_factor                                       :  2 ;    // default: 0x3;
		uint32_t vertical_noise_factor                                    :  2 ;    // default: 0x3;
	} bits;
} tvd_yc_sep2_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t yc_delay                                                 :  4 ;    // default: 0x0;
		uint32_t res0                                                     :  4 ;    // default: ;
		uint32_t contrast_gain                                            :  8 ;    // default: 0x80;
		uint32_t bright_offset                                            :  8 ;    // default: 0x20;
		uint32_t sharp_en                                                 :  1 ;    // default: 0x0;
		uint32_t sharp_coef1                                              :  3 ;    // default: 0x2;
		uint32_t sharp_coef2                                              :  2 ;    // default: 0x1;
		uint32_t res1                                                     :  2 ;    // default: ;
	} bits;
} tvd_enhance1_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t saturation_gain                                          :  8 ;    // default: 0x80;
		uint32_t chroma_enhance_en                                        :  1 ;    // default: 0x0;
		uint32_t chroma_enhance_strength                                  :  2 ;    // default: 0x3;
		uint32_t res0                                                     : 21 ;    // default: ;
	} bits;
} tvd_enhance2_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t cb_gain                                                  : 12 ;    // default: 0x80;
		uint32_t res0                                                     :  4 ;    // default: ;
		uint32_t cr_gain                                                  : 12 ;    // default: 0x0;
		uint32_t cbcr_gain_en                                             :  1 ;    // default: 0x0;
		uint32_t res1                                                     :  3 ;    // default: ;
	} bits;
} tvd_enhance3_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t wb_en:1;
		uint32_t wb_format:1;
		uint32_t field_sel:1;
		uint32_t hyscale_en:1;
		uint32_t wb_mb_mode:1;
		uint32_t wb_frame_mode:1;
		uint32_t flip_field:1;
		uint32_t res0:1;
		uint32_t wb_addr_valid:1;
		uint32_t res1:7;
		uint32_t hactive_stride:12;
		uint32_t yuv420_fil_en:1;
		uint32_t res2:2;
		/* 0x0->nv12, 0x1->nv21 */
		uint32_t wb_uv_swap:1;
	} bits;
} tvd_wb1_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t hactive_num                                              : 12 ;    // default: 0x0;
		uint32_t res0                                                     :  4 ;    // default: ;
		uint32_t vactive_num                                              : 11 ;    // default: 0x0;
		uint32_t res1                                                     :  5 ;    // default: ;
	} bits;
} tvd_wb2_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t ch1_y_addr                                                    ;    // default: 0x0;
	} bits;
} tvd_wb3_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t ch1_c_addr                                                    ;    // default: 0x0;
	} bits;
} tvd_wb4_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t lock                                                     :  1 ;    // Default: 0x0;
		uint32_t unlock                                                   :  1 ;    // Default: 0x0;
		uint32_t res0                                                     :  2 ;    // Default: ;
		uint32_t fifo_c_o                                                 :  1 ;    // Default: 0x0;
		uint32_t fifo_y_o                                                 :  1 ;    // Default: 0x0;
		uint32_t res1                                                     :  1 ;    // Default: 0x0;
		uint32_t fifo_c_u                                                 :  1 ;    // Default: 0x0;
		uint32_t fifo_y_u                                                 :  1 ;    // Default: 0x0;
		uint32_t res2                                                     :  7 ;    // Default: ;
		uint32_t wb_addr_change_err                                       :  1 ;    // Default: 0x0;
		uint32_t res3                                                     :  7 ;    // Default: ;
		uint32_t frame_end                                                :  1 ;    // Default: 0x0;
		uint32_t res4                                                     :  3 ;    // Default: ;
		uint32_t fifo_3d_rx_u                                             :  1 ;    // Default: 0x0;
		uint32_t fifo_3d_rx_o                                             :  1 ;    // Default: 0x0;
		uint32_t fifo_3d_tx_u                                             :  1 ;    // Default: 0x0;
		uint32_t fifo_3d_tx_o                                             :  1 ;    // Default: 0x0;
	} bits;
} tvd_irq_ctl_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t lock                                                     :  1 ;    // Default: 0x0;
		uint32_t unlock                                                   :  1 ;    // Default: 0x0;
		uint32_t res0                                                     :  2 ;    // Default: ;
		uint32_t fifo_c_o                                                 :  1 ;    // Default: 0x0;
		uint32_t fifo_y_o                                                 :  1 ;    // Default: 0x0;
		uint32_t res1                                                     :  1 ;    // Default: 0x0;
		uint32_t fifo_c_u                                                 :  1 ;    // Default: 0x0;
		uint32_t fifo_y_u                                                 :  1 ;    // Default: 0x0;
		uint32_t res2                                                     :  7 ;    // Default: ;
		uint32_t wb_addr_change_err                                       :  1 ;    // Default: 0x0;
		uint32_t res3                                                     :  7 ;    // Default: ;
		uint32_t frame_end                                                :  1 ;    // Default: 0x0;
		uint32_t res4                                                     :  3 ;    // Default: ;
		uint32_t fifo_3d_rx_u                                             :  1 ;    // Default: 0x0;
		uint32_t fifo_3d_rx_o                                             :  1 ;    // Default: 0x0;
		uint32_t fifo_3d_tx_u                                             :  1 ;    // Default: 0x0;
		uint32_t fifo_3d_tx_o                                             :  1 ;    // Default: 0x0;
	} bits;
} tvd_irq_status_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t res0                                                     :  8 ;    // default: ;
		uint32_t afe_gain_value                                           :  8 ;    // default: 0x0;
		uint32_t tvin_lock_debug                                          :  1 ;    // default: 0x0;
		uint32_t tvin_lock_high                                           :  1 ;    // default: 0x0;
		uint32_t truncation2_reset_gain_enable                            :  1 ;    // default: 0x0;
		uint32_t truncation_reset_gain_enable                             :  1 ;    // default: 0x0;
		uint32_t unlock_reset_gain_enable                                 :  1 ;    // default: 0x0;
		uint32_t afe_gain_mode                                            :  1 ;    // default: 0x0;
		uint32_t clamp_mode                                               :  1 ;    // default: 0;
		uint32_t clamp_up_start                                           :  1 ;    // default: 0x0;
		uint32_t clamp_dn_start                                           :  1 ;    // default: 0x0;
		uint32_t clamp_updn_cycles                                        :  7 ;    // default: 0x0;
	} bits;
} tvd_debug1_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t res0                                                     :  3 ;    // default;
		uint32_t agc_gate_thresh                                          :  5 ;    // default: 0xa;
		uint32_t ccir656_en                                               :  1 ;    // default: 0x0;
		uint32_t adc_cbcr_pump_swap                                       :  1 ;    // default: 0x0;
		uint32_t adc_updn_swap                                            :  1 ;    // default: 0x1;
		uint32_t adc_input_swap                                           :  1 ;    // default: 0x0;
		uint32_t hv_dely                                                  :  1 ;    // default: 0x0;
		uint32_t cv_inv                                                   :  1 ;    // default: 0x0;
		uint32_t tvd_src                                                  :  1 ;    // default: 0x0;
		uint32_t res1                                                     :  3 ;    // default: 0x0;
		uint32_t adc_wb_mode                                              :  2 ;    // default: 0x0;
		uint32_t hue                                                      :  8 ;    // default: 0x0;
		uint32_t res2                                                     :  4 ;    // default;
	} bits;
} tvd_debug2_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t res0                                                     : 20 ;    // default: ;
		uint32_t noise_thresh                                             :  8 ;    // default: 0x32;
		uint32_t ccir656_cbcr_write_back_sequence                         :  1 ;    // default: 0x0;
		uint32_t cbcr_swap                                                :  1 ;    // default: 0x0;
		uint32_t nstd_hysis                                               :  2 ;    // default: 0x0;
	} bits;
} tvd_debug3_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t fixed_burstgate                                          :  1 ;    // default: 0x1;
		uint32_t cautopos                                                 :  5 ;    // default: 0xc;
		uint32_t vnon_std_threshold                                       :  4 ;    // default: 0x0;
		uint32_t hnon_std_threshold                                       :  4 ;    // default: 0x6;
		uint32_t user_ckill_mode                                          :  2 ;    // default: 0x0;
		uint32_t hlock_ckill                                              :  1 ;    // default: 0x0;
		uint32_t vbi_ckill                                                :  1 ;    // default: 0x0;
		uint32_t chroma_kill                                              :  4 ;    // default: 0x7;
		uint32_t agc_gate_vsync_stip                                      :  1 ;    // default: 0x0;
		uint32_t agc_gate_vsync_coarse                                    :  1 ;    // default: 0x1;
		uint32_t agc_gate_kill_mode                                       :  2 ;    // default: 0x3;
		uint32_t agc_peak_en                                              :  1 ;    // default: 0x1;
		uint32_t hstate_unlocked                                          :  1 ;    // default: 0x1;
		uint32_t disable_hfine                                            :  1 ;    // default: 0x0;
		uint32_t hstate_fixed                                             :  1 ;    // default: 0x0;
		uint32_t hlock_vsync_mode                                         :  2 ;    // default: 0x3;
	} bits;
} tvd_debug4_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t vsync_clamp_mode                                         :  2 ;    // default: 0x2;
		uint32_t vsync_vbi_lockout_start                                  :  7 ;    // default: 0x70;
		uint32_t vsync_vbi_max                                            :  7 ;    // default: 0xe;
		uint32_t vlock_wide_range                                         :  1 ;    // default: 0x0;
		uint32_t locked_count_clean_max                                   :  4 ;    // default: 0x4;
		uint32_t locked_count_noisy_max                                   :  4 ;    // default: 0x7;
		uint32_t hstate_max                                               :  3 ;    // default: 0x3;
		uint32_t fixed_cstate                                             :  1 ;    // default: 0x0;
		uint32_t vodd_delayed                                             :  1 ;    // default: 0x0;
		uint32_t veven_delayed                                            :  1 ;    // default: 0x0;
		uint32_t field_polarity                                           :  1 ;    // default: 0x0;
	} bits;
} tvd_debug5_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t cstate                                                   :  3 ;    // default: 0x5;
		uint32_t lose_chromalock_level                                    :  3 ;    // default: 0x7;
		uint32_t lose_chromalock_count                                    :  4 ;    // default: 0x6;
		uint32_t palsw_level                                              :  2 ;    // default: 0x2;
		uint32_t vsync_thresh                                             :  6 ;    // default: 0x0;
		uint32_t vsync_cntl                                               :  2 ;    // default: 0x0;
		uint32_t vloop_tc                                                 :  2 ;    // default: 0x2;
		uint32_t field_detect_mode                                        :  2 ;    // default: 0x2;
		uint32_t cpump_delay                                              :  8 ;    // default: 0xb9;
	} bits;
} tvd_debug6_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t hresampler_2up                                           :  1 ;    // default: 0x1;
		uint32_t cpump_adjust_polarity                                    :  1 ;    // default: 0x0;
		uint32_t cpump_adjust_delay                                       :  6 ;    // default: 0x28;
		uint32_t cpump_adjust                                             :  8 ;    // default: 0xc8;
		uint32_t cpump_delay_en                                           :  1 ;    // default: 0x0;
		uint32_t vf_nstd_en                                               :  1 ;    // default: 0x1;
		uint32_t vcr_auto_switch_en                                       :  1 ;    // default: 0x1;
		uint32_t mv_hagc                                                  :  1 ;    // default: 0x1;
		uint32_t dagc_en                                                  :  1 ;    // default: 0x1;
		uint32_t agc_half_en                                              :  1 ;    // default: 0x1;
		uint32_t dc_clamp_mode                                            :  2 ;    // default: 0x0;
		uint32_t ldpause_threshold                                        :  4 ;    // default: 0x0;
		uint32_t res0                                                     :  4 ;    // default: ;
	} bits;
} tvd_debug7_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t chroma_step_ntsc                                              ;    // default: 0x0;
	} bits;
} tvd_debug8_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t chroma_step_paln                                              ;    // default: 0x0;
	} bits;
} tvd_debug9_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t chroma_step_palm                                              ;    // default: 0x0;
	} bits;
} tvd_debug10_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t chroma_step_pal                                               ;    // default: 0x0;
	} bits;
} tvd_debug11_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t y_wb_protect                                                  ;    // default: 0x0;
	} bits;
} tvd_debug12_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t uv_wb_protect                                                 ;    // default: 0x0;
	} bits;
} tvd_debug13_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t agc_peak_nominal                                         :  7 ;    // default: 0xa;
		uint32_t agc_peak_cntl                                            :  3 ;    // default: 0x1;
		uint32_t vsync_agc_lockout_start                                  :  7 ;    // default: 0x6c;
		uint32_t vsync_agc_max                                            :  6 ;    // default: 0x10;
		uint32_t noise_line                                               :  5 ;    // default: 0x0;
		uint32_t res0                                                     :  4 ;    // default: ;
	} bits;
} tvd_debug14_reg_t;

typedef union {
	uint32_t dwval;
	struct { 	uint32_t adc_lock_mode:1;
		uint32_t res0:3;			   /* default: 0x1; */
		uint32_t adc_lock_interval_period:10; /* default: 0x6c; */
		uint32_t res1:2;			   /* default: 0x10; */
		uint32_t adc_lock_interval_low:10;    /* default: 0x0; */
		uint32_t res2:6;			   /* default: ; */
	} bits;
} tvd_debug15_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t agc_analog_gain_status                                   :  8 ;    // default: 0x4;
		uint32_t agc_digital_gain_status                                  :  8 ;    // default: 0x0;
		uint32_t chroma_magnitude_status                                  :  8 ;    // default: 0x0;
		uint32_t res0                                                     :  8 ;    // default: ;
	} bits;
} tvd_status1_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t chroma_sync_dto_increment_status                              ;    // default: 0x21f07c1f;
	} bits;
} tvd_status2_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t horizontal_sync_dto_increment_status                     : 30 ;    // default: 0x20000000;
		uint32_t res0                                                     :  2 ;    // default: ;
	} bits;
} tvd_status3_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t no_signal_detected                                       :  1 ;    // default: 0x0;
		uint32_t h_locked     		                                     :  1 ;    // default: 0x0;
		uint32_t v_locked                                                 :  1 ;    // default: 0x1;
		uint32_t chroma_pll_locked_to_colour_burst                        :  1 ;    // default: 0x1;
		uint32_t macrovision_vbi_pseudosync_pulses_detection              :  1 ;    // default: 0x0;
		uint32_t macrovision_colour_stripes_detected                      :  3 ;    // default: 0x0;
		uint32_t proscan_detected                                         :  1 ;    // default: 0x0;
		uint32_t hnon_standard                                            :  1 ;    // default: 0x0;
		uint32_t vnon_standard                                            :  1 ;    // default: 0x0;
		uint32_t res0                                                     :  5 ;    // default: 0x0;
		uint32_t pal_detected                                             :  1 ;    // default: 0x0;
		uint32_t secam_detected                                           :  1 ;    // default: 0x0;
		uint32_t _625lines_detected                                       :  1 ;    // default: 0x0;
		uint32_t noisy                                                    :  1 ;    // default: 0x0;
		uint32_t vcr                                                      :  1 ;    // default: 0x0;
		uint32_t vcr_trick                                                :  1 ;    // default: 0x0;
		uint32_t vcr_ff                                                   :  1 ;    // default: 0x0;
		uint32_t vcr_rew                                                  :  1 ;    // default: 0x0;
		uint32_t res1                                                     :  8 ;    // default: ;
	} bits;
} tvd_status4_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t sync_level:11;
		uint32_t res0:5;
		uint32_t blank_level:11;
		uint32_t res1:5;
	} bits;
} tvd_status5_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t auto_detect_finish                                       :  1 ;    // default: 0x0;
		uint32_t tv_standard                                              :  3 ;    // default: 0x0;
		uint32_t auto_detect_en                                           :  1 ;    // default: 0x0;
		uint32_t mask_palm                                                :  1 ;    // default: 0x0;
		uint32_t mask_palcn                                               :  1 ;    // default: 0x0;
		uint32_t mask_pal60                                               :  1 ;    // default: 0x0;
		uint32_t mask_ntsc443                                             :  1 ;    // default: 0x0;
		uint32_t mask_secam                                               :  1 ;    // default: 0x0;
		uint32_t mask_unknown                                             :  1 ;    // default: 0x0;
		uint32_t res0                                                     : 21 ;    // default: ;
	} bits;
} tvd_status6_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t teletext_vbi_frame_code_register1                        :  8 ;    // default: 0x0;
		uint32_t teletext_vbi_frame_code_register2                        :  8 ;    // default: 0x0;
		uint32_t data_high_level_register                                 :  8 ;    // default: 0x0;
		uint32_t vbi_data_type_configuration_register_for_line7           :  8 ;    // default: 0x0;
	} bits;
} tvd_vbi1_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t vbi_data_type_configuration_register_for_line8           :  8 ;    // default: 0x0;
		uint32_t vbi_data_type_configuration_register_for_line9           :  8 ;    // default: 0x0;
		uint32_t vbi_data_type_configuration_register_for_line10          :  8 ;    // default: 0x0;
		uint32_t vbi_data_type_configuration_register_for_line11          :  8 ;    // default: 0x0;
	} bits;
} tvd_vbi2_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t vbi_data_type_configuration_register_for_line12          :  8 ;    // default: 0x0;
		uint32_t vbi_data_type_configuration_register_for_line13          :  8 ;    // default: 0x0;
		uint32_t vbi_data_type_configuration_register_for_line14          :  8 ;    // default: 0x0;
		uint32_t vbi_data_type_configuration_register_for_line15          :  8 ;    // default: 0x0;
	} bits;
} tvd_vbi3_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t vbi_data_type_configuration_register_for_line16          :  8 ;    // default: 0x0;
		uint32_t vbi_data_type_configuration_register_for_line17          :  8 ;    // default: 0x0;
		uint32_t vbi_data_type_configuration_register_for_line18          :  8 ;    // default: 0x0;
		uint32_t vbi_data_type_configuration_register_for_line19          :  8 ;    // default: 0x0;
	} bits;
} tvd_vbi4_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t vbi_data_type_configuration_register_for_line20          :  8 ;    // default: 0x0;
		uint32_t vbi_data_type_configuration_register_for_line21          :  8 ;    // default: 0x0;
		uint32_t vbi_data_type_configuration_register_for_line22          :  8 ;    // default: 0x0;
		uint32_t vbi_data_type_configuration_register_for_line23          :  8 ;    // default: 0x0;
	} bits;
} tvd_vbi5_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t vbi_data_type_configuration_register_for_remaining_lines :  8 ;    // default: 0x0;
		uint32_t vbi_loop_filter_gain                                     :  8 ;    // default: 0x0;
		uint32_t vbi_loop_filter_i_gain                                   :  8 ;    // default: 0x0;
		uint32_t vbi_loop_filter_g_gain                                   :  8 ;    // default: 0x0;
	} bits;
} tvd_vbi6_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t upper_byte_vbi_close_caption_dto                         :  8 ;    // default: 0x0;
		uint32_t lower_byte_vbi_close_caption_dto                         :  8 ;    // default: 0x0;
		uint32_t upper_byte_vbi_teletext_dto                              :  8 ;    // default: 0x0;
		uint32_t lower_byte_vbi_teletext_dto                              :  8 ;    // default: 0x0;
	} bits;
} tvd_vbi7_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t upper_byte_vbi_wss625_dto                                :  8 ;    // default: 0x0;
		uint32_t lower_byte_vbi_wss625_dto                                :  8 ;    // default: 0x0;
		uint32_t vbi_close_caption_data_1_register1                       :  8 ;    // default: 0x0;
		uint32_t vbi_close_caption_data_1_register2                       :  8 ;    // default: 0x0;
	} bits;
} tvd_vbi8_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t vbi_close_caption_data_1_register3                       :  8 ;    // default: 0x0;
		uint32_t vbi_close_caption_data_1_register4                       :  8 ;    // default: 0x0;
		uint32_t vbi_close_caption_data_2_register                        :  8 ;    // default: 0x0;
		uint32_t vbi_wss_data_1_register                                  :  8 ;    // default: 0x0;
	} bits;
} tvd_vbi9_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t vbi_wss_data_2_register                                  :  8 ;    // default: 0x0;
		uint32_t vbi_data_status_register                                 :  8 ;    // default: 0x0;
		uint32_t vbi_caption_start_register                               :  8 ;    // default: 0x0;
		uint32_t vbi_wss625_start_register                                :  8 ;    // default: 0x0;
	} bits;
} tvd_vbi10_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t vbi_teletext_start_register                              :  8 ;    // default: 0x0;
		uint32_t res0                                                     :  8 ;    // default: 0x0;
		uint32_t res1                                                     :  8 ;    // default: 0x0;
		uint32_t res2                                                     :  8 ;    // default: 0x0;
	} bits;
} tvd_vbi11_reg_t;

typedef union {
	uint32_t dwval;
	struct {
		uint32_t res0                       	  ;    // default: ;
	} bits;
} tvd_reservd_reg_t;

//device define
typedef struct {
	tvd_top_map_reg_t 				tvd_top_map;				//0x000
	tvd_reservd_reg_t				tvd_top_reg004;				//0x004
	tvd_3d_ctl1_reg_t 				tvd_3d_ctl1;				//0x008
	tvd_3d_ctl2_reg_t 				tvd_3d_ctl2;				//0x00c
	tvd_3d_ctl3_reg_t 				tvd_3d_ctl3;				//0x010
	tvd_3d_ctl4_reg_t 				tvd_3d_ctl4;				//0x014
	tvd_3d_ctl5_reg_t 				tvd_3d_ctl5;				//0x018
	tvd_reservd_reg_t				tvd_top_reg01c[2];			//0x01c~0x020
	tvd_adc_dig_reg_t				tvd_adc0_dig;				//0x024
	tvd_adc_ctl_reg_t 				tvd_adc0_ctl;				//0x028
	tvd_adc_cfg_reg_t 				tvd_adc0_cfg;				//0x02c
	tvd_reservd_reg_t				tvd_top_reg030[5];			//0x030~0x040
	tvd_adc_dig_reg_t				tvd_adc1_dig;				//0x044
	tvd_adc_ctl_reg_t 				tvd_adc1_ctl;				//0x048
	tvd_adc_cfg_reg_t 				tvd_adc1_cfg;				//0x04c
	tvd_reservd_reg_t				tvd_top_reg050[5];			//0x050~0x060
	tvd_adc_dig_reg_t				tvd_adc2_dig;				//0x064
	tvd_adc_ctl_reg_t 				tvd_adc2_ctl;				//0x068
	tvd_adc_cfg_reg_t 				tvd_adc2_cfg;				//0x06c
	tvd_reservd_reg_t				tvd_top_reg060[5];			//0x070~0x080
	tvd_adc_dig_reg_t				tvd_adc3_dig;				//0x084
	tvd_adc_ctl_reg_t 				tvd_adc3_ctl;				//0x088
	tvd_adc_cfg_reg_t 				tvd_adc3_cfg;				//0x08c
	tvd_reservd_reg_t				tvd_top_reg090[24];			//0x070~0x0ec
	tvd_adc_dump_reg_t				tvd_adc_dump;				//0x0f0
} __tvd_top_dev_t;

//device define
typedef struct {
	tvd_en_reg_t					tvd_en;						//0x000
	tvd_mode_reg_t					tvd_mode;					//0x004
	tvd_clamp_agc1_reg_t			tvd_clamp_agc1;				//0x008
	tvd_clamp_agc2_reg_t			tvd_clamp_agc2;				//0x00c
	tvd_hlock1_reg_t				tvd_hlock1;					//0x010
	tvd_hlock2_reg_t				tvd_hlock2;					//0x014
	tvd_hlock3_reg_t				tvd_hlock3;					//0x018
	tvd_hlock4_reg_t				tvd_hlock4;					//0x01c
	tvd_hlock5_reg_t				tvd_hlock5;					//0x020
	tvd_vlock1_reg_t				tvd_vlock1;					//0x024
	tvd_vlock2_reg_t				tvd_vlock2;					//0x028
	tvd_reservd_reg_t				tvd_reg02c;				    //0x02c
	tvd_clock1_reg_t				tvd_clock1;					//0x030
	tvd_clock2_reg_t				tvd_clock2;					//0x034
	tvd_reservd_reg_t				tvd_reg038[2];				//0x038~0x03c
	tvd_yc_sep1_reg_t				tvd_yc_sep1;				//0x040
	tvd_yc_sep2_reg_t				tvd_yc_sep2;				//0x044
	tvd_reservd_reg_t				tvd_reg048[2];				//0x048~0x04c
	tvd_enhance1_reg_t				tvd_enhance1;				//0x050
	tvd_enhance2_reg_t				tvd_enhance2;				//0x054
	tvd_enhance3_reg_t				tvd_enhance3;				//0x058
	tvd_reservd_reg_t				tvd_reg05c;					//0x05c
	tvd_wb1_reg_t					tvd_wb1;					//0x060
	tvd_wb2_reg_t					tvd_wb2;					//0x064
	tvd_wb3_reg_t					tvd_wb3;					//0x068
	tvd_wb4_reg_t					tvd_wb4;					//0x06c
	tvd_reservd_reg_t				tvd_reg070[4];				//0x070~0x07c
	tvd_irq_ctl_reg_t				tvd_irq_ctl;				//0x080
	tvd_reservd_reg_t				tvd_reg084[3];				//0x084~0x08c
	tvd_irq_status_reg_t			tvd_irq_status;				//0x090
	tvd_reservd_reg_t				tvd_reg094[27];				//0x094~0x0fc
	tvd_debug1_reg_t				tvd_debug1;					//0x100
	tvd_debug2_reg_t				tvd_debug2;					//0x104
	tvd_debug3_reg_t				tvd_debug3;					//0x108
	tvd_debug4_reg_t				tvd_debug4;					//0x10c
	tvd_debug5_reg_t				tvd_debug5;					//0x110
	tvd_debug6_reg_t				tvd_debug6;					//0x114
	tvd_debug7_reg_t				tvd_debug7;					//0x118
	tvd_debug8_reg_t				tvd_debug8;					//0x11c
	tvd_debug9_reg_t				tvd_debug9;					//0x120
	tvd_debug10_reg_t				tvd_debug10;				//0x124
	tvd_debug11_reg_t				tvd_debug11;				//0x128
	tvd_debug12_reg_t				tvd_debug12;				//0x12c
	tvd_debug13_reg_t				tvd_debug13;				//0x130
	tvd_debug14_reg_t				tvd_debug14;				//0x134
	tvd_debug15_reg_t				tvd_debug15;/*0x138 */
	tvd_reservd_reg_t				tvd_reg13c[17];/*0x13c~0x17c */
	tvd_status1_reg_t				tvd_status1;				//0x180
	tvd_status2_reg_t				tvd_status2;				//0x184
	tvd_status3_reg_t				tvd_status3;				//0x188
	tvd_status4_reg_t				tvd_status4;				//0x18c
	tvd_status5_reg_t				tvd_status5;				//0x190
	tvd_status6_reg_t				tvd_status6;				//0x194
	tvd_reservd_reg_t				tvd_reg198[906];			//0x198~0xfbc
	tvd_vbi1_reg_t					tvd_vbi1;					//0xfc0
	tvd_vbi2_reg_t					tvd_vbi2;					//0xfc4
	tvd_vbi3_reg_t					tvd_vbi3;					//0xfc8
	tvd_vbi4_reg_t					tvd_vbi4;					//0xfcc
	tvd_vbi5_reg_t					tvd_vbi5;					//0xfd0
	tvd_vbi6_reg_t					tvd_vbi6;					//0xfd4
	tvd_vbi7_reg_t					tvd_vbi7;					//0xfd8
	tvd_vbi8_reg_t					tvd_vbi8;					//0xfdc
	tvd_vbi9_reg_t					tvd_vbi9;					//0xfe0
	tvd_vbi10_reg_t               	tvd_vbi10;					//0xfe4
	tvd_vbi11_reg_t                	tvd_vbi11;					//0xfe8
} __tvd_dev_t;


typedef enum {
	TVD_IRQ_LOCK = 0,
	TVD_IRQ_UNLOCK = 1,
	TVD_IRQ_FIFO_C_O = 4,
	TVD_IRQ_FIFO_Y_O = 5,
	TVD_IRQ_FIFO_C_U = 7,
	TVD_IRQ_FIFO_Y_U = 8,
	TVD_IRQ_WB_ADDR_CHANGE_ERR = 16,
	TVD_IRQ_FRAME_END = 24,
	TVD_IRQ_FIFO_3D_RX_U = 28,
	TVD_IRQ_FIFO_3D_RX_O = 29,
	TVD_IRQ_FIFO_3D_TX_U = 30,
	TVD_IRQ_FIFO_3D_TX_O = 31,
} TVD_IRQ_T;

typedef enum {
    TVD_PL_YUV420 					=	0,
    TVD_MB_YUV420					=	1,
    TVD_PL_YUV422					=	2,
} TVD_FMT_T;

int32_t tvd_top_set_reg_base(uintptr_t base);
int32_t tvd_set_reg_base(unsigned sel, uintptr_t base);
int32_t tvd_init(unsigned sel, uint32_t interface);
int32_t tvd_deinit(unsigned sel, uint32_t interface);
int32_t tvd_get_status(unsigned sel, uint32_t *locked, uint32_t *system);
int32_t tvd_config(unsigned sel, uint32_t interface, uint32_t mode);
int32_t tvd_set_wb_width(unsigned sel, uint32_t width);
int32_t tvd_set_wb_width_jump(unsigned sel, uint32_t width_jump);
int32_t tvd_set_wb_height(unsigned sel, uint32_t height);
int32_t tvd_set_wb_addr(unsigned sel, uintptr_t addr_y, uintptr_t addr_c);
uintptr_t tvd_set_wb_addr2(unsigned sel, uintptr_t addr_y, uintptr_t addr_c);
int32_t tvd_set_wb_fmt(unsigned sel, TVD_FMT_T fmt);
int32_t tvd_set_wb_uv_swap(unsigned sel, uint32_t swap);
int32_t tvd_set_wb_field(unsigned sel, uint32_t is_field_mode, uint32_t is_field_even);
int32_t tvd_capture_on(unsigned sel);
int32_t tvd_capture_off(unsigned sel);
int32_t tvd_irq_enable(unsigned sel, TVD_IRQ_T irq_id);
int32_t tvd_irq_disable(unsigned sel, TVD_IRQ_T irq_id);
int32_t tvd_irq_status_get(unsigned sel, TVD_IRQ_T irq_id, uint32_t *irq_status);
int32_t tvd_irq_status_clear(unsigned sel, TVD_IRQ_T irq_id);
int32_t tvd_dma_irq_status_get(unsigned sel, uint32_t *irq_status);
int32_t tvd_dma_irq_status_clear_err_flag(unsigned sel, uint32_t irq_status);

void tvd_enable_chanel(unsigned sel, uint32_t en);           //���������� � tvd_init()

int32_t tvd_adc_config(uint32_t adc, uint32_t en);               //���������� � tvd_init()
int32_t tvd_set_saturation(unsigned sel, uint32_t saturation);
int32_t tvd_set_luma(unsigned sel, uint32_t luma);
int32_t tvd_set_contrast(unsigned sel, uint32_t contrast);
uint32_t tvd_get_saturation(unsigned sel);
uint32_t tvd_get_luma(unsigned sel);
uint32_t tvd_get_contrast(unsigned sel);
void tvd_3d_mode(unsigned _3d_sel, uint32_t _3d_en, uintptr_t _3d_addr);
uint32_t tvd_dbgmode_dump_data(uint32_t chan_sel, uint32_t mode, uintptr_t dump_dst_addr,
			  uint32_t data_length);
void tvd_agc_auto_config(unsigned sel);
void tvd_agc_manual_config(unsigned sel, uint32_t agc_manual_val);
void tvd_cagc_config(unsigned sel, uint32_t enable);
int32_t tvd_get_lock(unsigned sel);
void tvd_blue_display_mode(unsigned sel, uint32_t mode);
void tvd_reset(unsigned sel);
void tvd_input_sel(uint32_t input);
#endif
