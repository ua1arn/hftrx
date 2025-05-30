/*
 * drivers/media/platform/sunxi-tvd/bsp_tvd/bsp_tvd.c
 *
 * Copyright (c) 2007-2018 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * tvd lowlevel
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
#include "hardware.h"
#include "formats.h"

#if WITHTVDHW

#include "bsp_tvd.h"
#include "clocks.h"

static volatile __tvd_top_dev_t *tvd_top_dev;
static volatile __tvd_dev_t 	 *tvd_device[4];

static volatile __tvd_dev_t * tvd_devicep(unsigned sel)
{
	return tvd_device[sel];
}

int32_t tvd_top_set_reg_base(uintptr_t base)
{
	tvd_top_dev = (__tvd_top_dev_t *)base;
	return 0;
}

int32_t tvd_set_reg_base(unsigned sel, uintptr_t base)
{
	if (sel >= ARRAY_SIZE(tvd_device))
		return -1;

	tvd_device [sel] = (__tvd_dev_t *)base;
	return 0;
}

void tvd_3d_mode(unsigned _3d_sel, uint32_t _3d_en, uintptr_t _3d_addr)
{
	tvd_top_dev->tvd_3d_ctl3.bits.comb_3d_addr0 = _3d_addr;
	tvd_top_dev->tvd_3d_ctl4.bits.comb_3d_addr1 = _3d_addr+0x200000;
	tvd_top_dev->tvd_3d_ctl5.bits.comb_3d_size = 0x200000;
	tvd_top_dev->tvd_3d_ctl1.bits.comb_3d_sel = _3d_sel;
	tvd_top_dev->tvd_3d_ctl1.bits.comb_3d_en = _3d_en;
	tvd_top_dev->tvd_3d_ctl1.bits.tvd_en_3d_dma = _3d_en;

	tvd_devicep(_3d_sel)->tvd_yc_sep1.bits._3d_comb_filter_dis =
								_3d_en ? 0 : 1;
}

void tvd_enable_chanel(unsigned sel, uint32_t en)
{
	tvd_devicep(sel)->tvd_en.bits.tvd_en_ch = en;
}

void tvd_input_sel(uint32_t input)
{
	tvd_top_dev->tvd_top_map.bits.tvin_sel = input;
}

int32_t tvd_init(unsigned sel, uint32_t interface)
{
	if (sel == 0 && interface != CVBS_INTERFACE) {
		tvd_adc_config(0, 1);
		tvd_adc_config(1, 1);
		tvd_adc_config(2, 1);
	} else if (interface == CVBS_INTERFACE || sel == 3) {
		tvd_adc_config(sel, 1);
	}

	if (tvd_top_dev->tvd_top_map.bits.tvd_adc_map != 2) {
		tvd_top_dev->tvd_top_map.bits.tvd_adc_map = (interface ? 2 : 1);
		/*
		(sel == 0) ? (interface ? 2 : 1) :
		(sel == 1) ? (interface ? 0 : 1) :
		(sel == 2) ? (interface ? 0 : 1) :
		(interface ? 0 : 1);
		*/
	}
	/*
		if(sel==0)
		{
			tvd_top_dev->tvd_top_map.bits.tvd_adc_map = interface ?
	   2 : 1;
		}
	*/
	tvd_enable_chanel(sel, 1);
	return 0;
}

int32_t tvd_deinit(unsigned sel, uint32_t interface)
{
	tvd_enable_chanel(sel, 0);
	if (sel == 0 && interface != CVBS_INTERFACE) {
		tvd_adc_config(0, 0);
		tvd_adc_config(1, 0);
		tvd_adc_config(2, 0);
	} else if (interface == CVBS_INTERFACE || sel == 3) {
		tvd_adc_config(sel, 0);
	}

	return 0;
}

int32_t tvd_get_lock(unsigned sel)
{
	uint32_t reg_val;
	uint32_t lock;
	uint32_t irq_status;

	tvd_irq_status_get(sel, TVD_IRQ_UNLOCK, &irq_status);
	tvd_irq_status_clear(sel, TVD_IRQ_UNLOCK);
	reg_val = tvd_devicep(sel)->tvd_status4.dwval;
	if (0xe != (reg_val & 0xf) || (0x1 == irq_status))
		lock = 0;
	else
		lock = 1;
	return lock;
}

void tvd_reset(unsigned sel)
{
	volatile uint32_t delay = 100;
	tvd_devicep(sel)->tvd_en.bits.tvd_en_ch = 0;
	while (delay--)
		;
	tvd_devicep(sel)->tvd_en.bits.tvd_en_ch = 1;
}

void tvd_blue_display_mode(unsigned sel, uint32_t mode)
{
	tvd_devicep(sel)->tvd_mode.bits.blue_display_mode = mode;
}

int32_t tvd_get_status(unsigned sel, uint32_t *locked, uint32_t *system)
{
	if (tvd_devicep(sel)->tvd_status4.bits.no_signal_detected == 0 &&
	    tvd_devicep(sel)->tvd_status4.bits.h_locked == 1 &&
	    tvd_devicep(sel)->tvd_status4.bits.v_locked == 1)
		*locked = 1;
	else
		*locked = 0;

	if (tvd_devicep(sel)->tvd_status4.bits._625lines_detected == 1)
		*system = 1;
	else
		*system = 0;

	return 0;
}

/*system: 1:pal; 0:NTSC*/
int32_t tvd_config(unsigned sel, uint32_t interface, uint32_t system)
{
	tvd_devicep(sel)->tvd_wb1.bits.wb_en = 0;    /*wb dma disable*/
	tvd_devicep(sel)->tvd_en.bits.tvd_en_ch = 0; /*tvd module disable*/

	tvd_devicep(sel)->tvd_en.bits.en_lock_disable_write_back1only_start_wb_when_locked = 0;
	tvd_devicep(sel)->tvd_en.bits.en_lock_disable_write_back2when_unlocked = 0;
	tvd_devicep(sel)->tvd_en.bits.clr_rsmp_fifo = 0;
	tvd_devicep(sel)->tvd_mode.bits.blue_display_mode = 2; /*auto*/
	tvd_devicep(sel)->tvd_mode.bits.blue_color = 1;	/*black*/
	tvd_devicep(sel)->tvd_clamp_agc1.bits.agc_en = 1;
	tvd_devicep(sel)->tvd_clamp_agc1.bits.agc_frequence = 1;
	tvd_devicep(sel)->tvd_clamp_agc1.bits.cagc_en = interface ? 0 : 1;
	tvd_devicep(sel)->tvd_clamp_agc2.bits.agc_gate_width = 64;
	tvd_devicep(sel)->tvd_clamp_agc2.bits.agc_backporch_delay = 100;
	tvd_devicep(sel)->tvd_clamp_agc2.bits.agc_gate_begin = 1666;
	tvd_devicep(sel)->tvd_hlock1.bits.h_sample_step = 0x20000000;
	tvd_devicep(sel)->tvd_hlock2.bits.hsync_filter_gate_start_time = 214;
	tvd_devicep(sel)->tvd_hlock2.bits.hsync_filter_gate_end_time = 78;
	tvd_devicep(sel)->tvd_hlock3.bits.hsync_rising_detect_window_start_time =
	    45;
	tvd_devicep(sel)->tvd_hlock3.bits.hsync_rising_detect_window_end_time =
	    80;
	tvd_devicep(sel)->tvd_hlock3.bits.hsync_tip_detect_window_start_time =
	    233;
	tvd_devicep(sel)->tvd_hlock3.bits.hsync_tip_detect_window_end_time = 15;
	tvd_devicep(sel)
	    ->tvd_hlock4.bits
	    .hsync_detect_window_start_time_for_coarse_detection = 0;
	tvd_devicep(sel)->tvd_hlock4.bits.hsync_detect_window_end_time_for_corase_detect = 128;
	tvd_devicep(sel)->tvd_hlock4.bits.hsync_rising_time_for_fine_detect = 62;
	tvd_devicep(sel)->tvd_hlock4.bits.hsync_fine_to_coarse_offset = 62;

	tvd_devicep(sel)->tvd_mode.bits.ypbpr_mode = interface ? 1 : 0;
	tvd_devicep(sel)->tvd_mode.bits.svideo_mode = 0;
	tvd_devicep(sel)->tvd_mode.bits.progressive_mode =
	    (interface == 2) ? 1 : 0;

	tvd_devicep(sel)->tvd_clamp_agc1.bits.agc_target =  interface ? 0 :
						(system == 0) ? 221 : 220;
	tvd_devicep(sel)->tvd_clamp_agc1.bits.cagc_target = interface ?
							0 : (system == 0) ?
							160 : 144;
	tvd_devicep(sel)->tvd_clamp_agc2.bits.black_level_clamp =
					(interface == 0 || system == 0) ? 1 : 0;
	tvd_devicep(sel)->tvd_hlock2.bits.htol = system;
	tvd_devicep(sel)->tvd_vlock1.bits.vtol = system;
	tvd_devicep(sel)->tvd_hlock5.bits.hactive_start = (system ? 137 : 130) -
							(interface ? 20 : 0);
	tvd_devicep(sel)->tvd_hlock5.bits.hactive_width = 80;
	tvd_devicep(sel)->tvd_vlock1.bits.vactive_start = system?42:34;
	tvd_devicep(sel)->tvd_vlock1.bits.vactive_height = system?193:97;
	tvd_devicep(sel)->tvd_hlock5.bits.backporch_detect_window_start_time = 34;
	tvd_devicep(sel)->tvd_hlock5.bits.backporch_detect_window_end_time = 66;
	tvd_devicep(sel)->tvd_vlock2.bits.hsync_dectector_disable_start_line = 112;
	tvd_devicep(sel)->tvd_vlock2.bits.hsync_detector_disable_end_line = 14;
	tvd_devicep(sel)->tvd_clock1.bits.color_kill_en = 1;
	tvd_devicep(sel)->tvd_clock1.bits.color_std = system;
	tvd_devicep(sel)->tvd_clock1.bits.color_std_ntsc = 0;
	tvd_devicep(sel)->tvd_clock1.bits.burst_gate_start_time = 50;
	tvd_devicep(sel)->tvd_clock1.bits.burst_gate_end_time = 70;
	tvd_devicep(sel)->tvd_clock1.bits.wide_burst_gate = 0;
	tvd_devicep(sel)->tvd_clock1.bits.chroma_lpf = 1;
	tvd_devicep(sel)->tvd_clock2.bits.c_sample_step = interface ? 0x0 :
					system ? 0x2a098acb : 0x21f07c1f;
	tvd_devicep(sel)->tvd_yc_sep1.bits._3d_comb_filter_mode = system ? 4 : 1;
	tvd_devicep(sel)->tvd_yc_sep1.bits._3d_comb_filter_dis = 1;
	tvd_devicep(sel)->tvd_yc_sep1.bits._2d_comb_filter_mode = system?6:0;
	tvd_devicep(sel)->tvd_yc_sep1.bits.secam_notch_wide = 0;
	tvd_devicep(sel)->tvd_yc_sep1.bits.chroma_bandpass_filter_en = 1;
	tvd_devicep(sel)->tvd_yc_sep1.bits.pal_chroma_level = 16;
	tvd_devicep(sel)->tvd_yc_sep1.bits.comb_filter_buffer_clear = 0;
	tvd_devicep(sel)->tvd_yc_sep1.bits.notch_factor = 0;
	tvd_devicep(sel)->tvd_yc_sep1.bits._2d_comb_factor = 2;
	tvd_devicep(sel)->tvd_yc_sep1.bits._3d_comb_factor = system ? 0 : 1;
	tvd_devicep(sel)->tvd_yc_sep1.bits.chroma_coring_enable = 0;
	tvd_devicep(sel)->tvd_yc_sep2.bits.horizontal_luma_filter_gain = 2;
	tvd_devicep(sel)->tvd_yc_sep2.bits.horizontal_chroma_filter_gain = 2;
	tvd_devicep(sel)->tvd_yc_sep2.bits.luma_vertical_filter_gain = 3;
	tvd_devicep(sel)->tvd_yc_sep2.bits.chroma_vertical_filter_gain = 3;
	tvd_devicep(sel)->tvd_yc_sep2.bits.motion_detect_noise_detect_en = 1;
	tvd_devicep(sel)->tvd_yc_sep2.bits.motion_detect_noise_threshold =
	    system ? 32 : 33;
	tvd_devicep(sel)->tvd_yc_sep2.bits.noise_detect_en = system ? 0 : 1;
	/* default value:50 */
	tvd_devicep(sel)->tvd_yc_sep2.bits.noise_threshold = 10;
	tvd_devicep(sel)->tvd_yc_sep2.bits.luma_noise_factor = 3;
	tvd_devicep(sel)->tvd_yc_sep2.bits.chroma_noise_factor = 2;
	tvd_devicep(sel)->tvd_yc_sep2.bits.burst_noise_factor = 0;
	tvd_devicep(sel)->tvd_yc_sep2.bits.vertical_noise_factor = 0;
	tvd_devicep(sel)->tvd_enhance1.bits.yc_delay = 0;

	tvd_devicep(sel)->tvd_enhance1.bits.contrast_gain =
	    tvd_get_contrast(sel);
	tvd_devicep(sel)->tvd_enhance1.bits.bright_offset = tvd_get_luma(sel);
	if (system == 0) {
		tvd_devicep(sel)->tvd_enhance1.bits.sharp_en = 1;
		tvd_devicep(sel)->tvd_enhance1.bits.sharp_coef1 = 5;
		tvd_devicep(sel)->tvd_enhance1.bits.sharp_coef2 = 2;
		tvd_devicep(sel)->tvd_enhance3.bits.cbcr_gain_en = 1;
	} else if (system == 1) {
		tvd_devicep(sel)->tvd_enhance1.bits.sharp_en = 0;
		tvd_devicep(sel)->tvd_enhance1.bits.sharp_coef1 = 2;
		tvd_devicep(sel)->tvd_enhance1.bits.sharp_coef2 = 1;
		tvd_devicep(sel)->tvd_enhance3.bits.cbcr_gain_en = 0;
	}
	tvd_devicep(sel)->tvd_enhance1.bits.sharp_en = 0;
	tvd_devicep(sel)->tvd_enhance1.bits.sharp_coef1 = 2;
	tvd_devicep(sel)->tvd_enhance1.bits.sharp_coef2 = 1;
	tvd_devicep(sel)->tvd_enhance2.bits.saturation_gain =
	    tvd_get_saturation(sel);
	tvd_devicep(sel)->tvd_enhance2.bits.chroma_enhance_en = 1;
	tvd_devicep(sel)->tvd_enhance2.bits.chroma_enhance_strength = 3;
	tvd_devicep(sel)->tvd_enhance3.bits.cb_gain = 2055;
	tvd_devicep(sel)->tvd_enhance3.bits.cr_gain = 1457;

	if (interface == 0) {
		tvd_devicep(sel)->tvd_enhance3.bits.cb_gain = 0x811;
		tvd_devicep(sel)->tvd_enhance3.bits.cr_gain = 0x5b8;
		tvd_devicep(sel)->tvd_enhance3.bits.cbcr_gain_en =
		    system ? 0 : 1;
	} else {
		tvd_devicep(sel)->tvd_enhance3.bits.cb_gain = 1460;
		tvd_devicep(sel)->tvd_enhance3.bits.cr_gain = 1460;
		tvd_devicep(sel)->tvd_enhance3.bits.cbcr_gain_en = 1;
	}

	tvd_devicep(sel)->tvd_debug1.bits.afe_gain_value = 64;
	tvd_devicep(sel)->tvd_debug1.bits.tvin_lock_debug = 0;
	tvd_devicep(sel)->tvd_debug1.bits.tvin_lock_high = 0;
	tvd_devicep(sel)->tvd_debug1.bits.truncation2_reset_gain_enable = 0;
	tvd_devicep(sel)->tvd_debug1.bits.truncation_reset_gain_enable = 0;
	tvd_devicep(sel)->tvd_debug1.bits.unlock_reset_gain_enable = 1;
	tvd_devicep(sel)->tvd_debug1.bits.afe_gain_mode = 0;
	tvd_devicep(sel)->tvd_debug1.bits.clamp_mode = 0;
	tvd_devicep(sel)->tvd_debug1.bits.clamp_up_start = 0;
	tvd_devicep(sel)->tvd_debug1.bits.clamp_dn_start = 0;
	tvd_devicep(sel)->tvd_debug1.bits.clamp_updn_cycles = 0;
	tvd_devicep(sel)->tvd_debug2.bits.agc_gate_thresh = 10;
	tvd_devicep(sel)->tvd_debug2.bits.ccir656_en = 0;
	tvd_devicep(sel)->tvd_debug2.bits.adc_cbcr_pump_swap = 0;
	tvd_devicep(sel)->tvd_debug2.bits.adc_updn_swap = 1;
	tvd_devicep(sel)->tvd_debug2.bits.adc_input_swap = 0;
	tvd_devicep(sel)->tvd_debug2.bits.hv_dely = 0;
	tvd_devicep(sel)->tvd_debug2.bits.cv_inv = 0;
	tvd_devicep(sel)->tvd_debug2.bits.tvd_src = 0;
	tvd_devicep(sel)->tvd_debug2.bits.adc_wb_mode = 0;
	tvd_devicep(sel)->tvd_debug2.bits.hue = 0;
	tvd_devicep(sel)->tvd_debug3.bits.noise_thresh = 50;
	tvd_devicep(sel)->tvd_debug3.bits.ccir656_cbcr_write_back_sequence = 0;
	tvd_devicep(sel)->tvd_debug3.bits.cbcr_swap = 0;
	tvd_devicep(sel)->tvd_debug3.bits.nstd_hysis = 0;
	tvd_devicep(sel)->tvd_debug4.bits.fixed_burstgate = 1;
	tvd_devicep(sel)->tvd_debug4.bits.cautopos = 12;
	tvd_devicep(sel)->tvd_debug4.bits.vnon_std_threshold = 0;
	tvd_devicep(sel)->tvd_debug4.bits.hnon_std_threshold = 6;
	tvd_devicep(sel)->tvd_debug4.bits.user_ckill_mode = 0;
	tvd_devicep(sel)->tvd_debug4.bits.hlock_ckill = 0;
	tvd_devicep(sel)->tvd_debug4.bits.vbi_ckill = 0;
	tvd_devicep(sel)->tvd_debug4.bits.chroma_kill = 7;
	tvd_devicep(sel)->tvd_debug4.bits.agc_gate_vsync_stip = 0;
	tvd_devicep(sel)->tvd_debug4.bits.agc_gate_vsync_coarse = 1;
	tvd_devicep(sel)->tvd_debug4.bits.agc_gate_kill_mode = 0;
	tvd_devicep(sel)->tvd_debug4.bits.agc_peak_en = system ? 1 : 0;
	tvd_devicep(sel)->tvd_debug4.bits.hstate_unlocked = 1;
	tvd_devicep(sel)->tvd_debug4.bits.disable_hfine = 0;
	tvd_devicep(sel)->tvd_debug4.bits.hstate_fixed = 0;
	tvd_devicep(sel)->tvd_debug4.bits.hlock_vsync_mode = 3;
	tvd_devicep(sel)->tvd_debug5.bits.vsync_clamp_mode = 0;
	tvd_devicep(sel)->tvd_debug5.bits.vsync_vbi_lockout_start = 112;
	tvd_devicep(sel)->tvd_debug5.bits.vsync_vbi_max = 14;
	tvd_devicep(sel)->tvd_debug5.bits.vlock_wide_range = 0;
	tvd_devicep(sel)->tvd_debug5.bits.locked_count_clean_max = 4;
	tvd_devicep(sel)->tvd_debug5.bits.locked_count_noisy_max = 7;
	tvd_devicep(sel)->tvd_debug5.bits.hstate_max = (system == 0) ? 3 : 5;
	tvd_devicep(sel)->tvd_debug5.bits.fixed_cstate = 0;
	tvd_devicep(sel)->tvd_debug5.bits.vodd_delayed = 0;
	tvd_devicep(sel)->tvd_debug5.bits.veven_delayed = 0;
	tvd_devicep(sel)->tvd_debug5.bits.field_polarity = 0;
	tvd_devicep(sel)->tvd_debug6.bits.cstate = 5;
	tvd_devicep(sel)->tvd_debug6.bits.lose_chromalock_level = 7;
	tvd_devicep(sel)->tvd_debug6.bits.lose_chromalock_count = 6;
	tvd_devicep(sel)->tvd_debug6.bits.palsw_level = 2;
	tvd_devicep(sel)->tvd_debug6.bits.vsync_thresh = 44;
	tvd_devicep(sel)->tvd_debug6.bits.vsync_cntl = 2;
	tvd_devicep(sel)->tvd_debug6.bits.vloop_tc = 2;
	tvd_devicep(sel)->tvd_debug6.bits.field_detect_mode = 2;
	tvd_devicep(sel)->tvd_debug6.bits.cpump_delay = 185;
	tvd_devicep(sel)->tvd_debug7.bits.hresampler_2up = 1;
	tvd_devicep(sel)->tvd_debug7.bits.cpump_adjust_polarity = 0;
	tvd_devicep(sel)->tvd_debug7.bits.cpump_adjust_delay = 10;
	tvd_devicep(sel)->tvd_debug7.bits.cpump_adjust = 200;
	tvd_devicep(sel)->tvd_debug7.bits.cpump_delay_en = 0;
	tvd_devicep(sel)->tvd_debug7.bits.vf_nstd_en = 1;
	tvd_devicep(sel)->tvd_debug7.bits.vcr_auto_switch_en = 1;
	tvd_devicep(sel)->tvd_debug7.bits.mv_hagc = 1;
	tvd_devicep(sel)->tvd_debug7.bits.dagc_en = 1;
	tvd_devicep(sel)->tvd_debug7.bits.agc_half_en = 0;
	tvd_devicep(sel)->tvd_debug7.bits.dc_clamp_mode = 1;
	tvd_devicep(sel)->tvd_debug7.bits.ldpause_threshold = 0;
	tvd_devicep(sel)->tvd_debug8.bits.chroma_step_ntsc = 0x21f07c1f;
	tvd_devicep(sel)->tvd_debug9.bits.chroma_step_paln = 0x21f69446;
	tvd_devicep(sel)->tvd_debug10.bits.chroma_step_palm = 0x21e6efe3;
	tvd_devicep(sel)->tvd_debug11.bits.chroma_step_pal = 0x2a098acb;
	tvd_devicep(sel)->tvd_debug12.bits.y_wb_protect = 0xffffffff;
	tvd_devicep(sel)->tvd_debug13.bits.uv_wb_protect = 0xffffffff;
	tvd_devicep(sel)->tvd_debug14.bits.agc_peak_nominal = system ? 12 : 10;
	tvd_devicep(sel)->tvd_debug14.bits.agc_peak_cntl = 1;
	tvd_devicep(sel)->tvd_debug14.bits.vsync_agc_lockout_start = 108;
	tvd_devicep(sel)->tvd_debug14.bits.vsync_agc_max = 16;
	tvd_devicep(sel)->tvd_debug14.bits.noise_line = 0;
	tvd_devicep(sel)->tvd_en.bits.tvd_en_ch = 1;
	/*tvd_devicep(sel)->tvd_debug15.bits.adc_lock_interval_period=0x40;*/
	/*tvd_devicep(sel)->tvd_debug15.bits.adc_lock_interval_low=30;*/
	/*tvd_devicep(sel)->tvd_debug15.bits.adc_lock_mode=1;*/

	return 0;
}

int32_t tvd_set_wb_width(unsigned sel, uint32_t width)
{
	tvd_devicep(sel)->tvd_wb2.bits.hactive_num = width;
	return 0;
}

int32_t tvd_set_wb_width_jump(unsigned sel, uint32_t width_jump)
{
	tvd_devicep(sel)->tvd_wb1.bits.hactive_stride = width_jump;
	return 0;
}

int32_t tvd_set_wb_height(unsigned sel, uint32_t height)
{
	tvd_devicep(sel)->tvd_wb2.bits.vactive_num = height;
	return 0;
}

/**
 * @name       tvd_set_wb_addr
 * @brief      set dma y and c(uv) channel addre
 * @param[IN]
 * @param[OUT]
 * @return
 */
//int32_t tvd_set_wb_addr(unsigned sel, uintptr_t addr_y, uintptr_t addr_c)
//{
//	//make sure 1 frame change 1 buffer,no need to detect this bit
//	//while(tvd_devicep(sel)->tvd_wb1.bits.wb_addr_valid);
//	tvd_devicep(sel)->tvd_wb3.bits.ch1_y_addr = addr_y;
//	tvd_devicep(sel)->tvd_wb4.bits.ch1_c_addr = addr_c;
//	tvd_devicep(sel)->tvd_wb1.bits.wb_addr_valid = 1;
//	return 0;
//}

uintptr_t tvd_set_wb_addr2(unsigned sel, uintptr_t addr_y, uintptr_t addr_c)
{
	uintptr_t old = tvd_devicep(sel)->tvd_wb3.bits.ch1_y_addr;
	//make sure 1 frame change 1 buffer,no need to detect this bit
	//while(tvd_devicep(sel)->tvd_wb1.bits.wb_addr_valid);
	tvd_devicep(sel)->tvd_wb3.bits.ch1_y_addr = addr_y;
	tvd_devicep(sel)->tvd_wb4.bits.ch1_c_addr = addr_c;
	tvd_devicep(sel)->tvd_wb1.bits.wb_addr_valid = 1;
	return old;
}

int32_t tvd_set_wb_fmt(unsigned sel, TVD_FMT_T fmt)
{
	tvd_devicep(sel)->tvd_wb1.bits.wb_mb_mode =
						(fmt == TVD_MB_YUV420) ? 1 : 0;
	tvd_devicep(sel)->tvd_wb1.bits.wb_format  =
						(fmt == TVD_PL_YUV422) ? 1 : 0;

	tvd_devicep(sel)->tvd_wb1.bits.yuv420_fil_en =
	    (fmt == TVD_PL_YUV422) ? 0 : 1;
	return 0;
}

int32_t tvd_set_wb_uv_swap(unsigned sel, uint32_t swap)
{
	tvd_devicep(sel)->tvd_wb1.bits.wb_uv_swap = swap ? 1 : 0;

	return 0;
}

int32_t tvd_set_wb_field(unsigned sel, uint32_t is_field_mode, uint32_t is_field_even)
{
	tvd_devicep(sel)->tvd_wb1.bits.wb_frame_mode = is_field_mode ? 0 : 1 ;
	tvd_devicep(sel)->tvd_wb1.bits.hyscale_en =    is_field_mode ? 1 : 0 ;
	return 0;
}

int32_t tvd_capture_on(unsigned sel)
{
	tvd_devicep(sel)->tvd_wb1.bits.wb_en = 1;
	return 0;
}

int32_t tvd_capture_off(unsigned sel)
{
	tvd_devicep(sel)->tvd_wb1.bits.wb_en = 0;
	return 0;
}

int32_t tvd_irq_enable(unsigned sel, TVD_IRQ_T irq_id)
{
	tvd_devicep(sel)->tvd_irq_ctl.dwval |= (1<<irq_id);
	return 0;
}

int32_t tvd_irq_disable(unsigned sel, TVD_IRQ_T irq_id)
{
	tvd_devicep(sel)->tvd_irq_ctl.dwval &= ~(1<<irq_id);
	return 0;
}

int32_t tvd_irq_status_get(unsigned sel, TVD_IRQ_T irq_id, uint32_t *irq_status)
{
	*irq_status = (tvd_devicep(sel)->tvd_irq_status.dwval >> irq_id) & 0x1;
	return 0;
}

int32_t tvd_irq_status_clear(unsigned sel, TVD_IRQ_T irq_id)
{
	tvd_devicep(sel)->tvd_irq_status.dwval = (1<<irq_id);
	return 0;
}

int32_t tvd_dma_irq_status_get(unsigned sel, uint32_t *irq_status)
{
	*irq_status = tvd_devicep(sel)->tvd_irq_status.dwval;
	return 0;
}

int32_t tvd_dma_irq_status_clear_err_flag(unsigned sel, uint32_t irq_status)
{
	tvd_devicep(sel)->tvd_irq_status.dwval = irq_status;
	return 0;
}

int32_t tvd_set_saturation(unsigned sel, uint32_t saturation)
{
	tvd_devicep(sel)->tvd_enhance2.bits.saturation_gain = saturation;
	return 0;
}

int32_t tvd_set_luma(unsigned sel, uint32_t luma)
{
	tvd_devicep(sel)->tvd_enhance1.bits.bright_offset = luma;
	return 0;
}
int32_t tvd_set_contrast(unsigned sel, uint32_t contrast)
{
	tvd_devicep(sel)->tvd_enhance1.bits.contrast_gain = contrast;
	return 0;
}

uint32_t tvd_get_saturation(unsigned sel)
{
	return tvd_devicep(sel)->tvd_enhance2.bits.saturation_gain;
}

uint32_t tvd_get_luma(unsigned sel)
{
	return tvd_devicep(sel)->tvd_enhance1.bits.bright_offset;
}
uint32_t tvd_get_contrast(unsigned sel)
{
	return tvd_devicep(sel)->tvd_enhance1.bits.contrast_gain;
}

/**
 * @name       tvd_adc_config
 * @brief      config tvd adc and enable it
 * @param[IN]  adc: the index of tvd module
 * @param[IN]  en: 1: enable; 0: disable
 * @return    0 if success
 */
int32_t tvd_adc_config(uint32_t adc, uint32_t en)
{
	volatile tvd_adc_cfg_reg_t *adc_cfg_p;
	volatile tvd_adc_ctl_reg_t *adc_ctl_p;
	volatile tvd_adc_dig_reg_t *adc_dig_p;

	if (adc > 3 || en > 1)
		return -1;

	adc_cfg_p = (adc == 0) ? &tvd_top_dev->tvd_adc0_cfg :
				(adc == 1) ? &tvd_top_dev->tvd_adc1_cfg :
				(adc == 2) ? &tvd_top_dev->tvd_adc2_cfg :
						&tvd_top_dev->tvd_adc3_cfg;

	adc_ctl_p = (adc == 0) ? &tvd_top_dev->tvd_adc0_ctl :
				(adc == 1) ? &tvd_top_dev->tvd_adc1_ctl :
				(adc == 2) ? &tvd_top_dev->tvd_adc2_ctl :
						&tvd_top_dev->tvd_adc3_ctl;

	adc_dig_p = (adc == 0) ? &tvd_top_dev->tvd_adc0_dig :
				(adc == 1) ? &tvd_top_dev->tvd_adc1_dig :
				(adc == 2) ? &tvd_top_dev->tvd_adc2_dig :
						&tvd_top_dev->tvd_adc3_dig;
	adc_cfg_p->bits.stage1_ibias = 2;
	adc_cfg_p->bits.stage2_ibias = 2;
	adc_cfg_p->bits.stage3_ibias = 2;
	adc_cfg_p->bits.stage4_ibias = 2;
	adc_cfg_p->bits.stage5_ibias = 2;
	adc_cfg_p->bits.stage6_ibias = 2;
	adc_cfg_p->bits.stage7_ibias = 2;
	adc_cfg_p->bits.stage8_ibias = 2;
	adc_cfg_p->bits.clp_step = 7;
	adc_cfg_p->bits.data_dly = 1;
	adc_dig_p->bits.lpf_dig_sel = 0;
	adc_dig_p->bits.lpf_dig_en = en;
	adc_ctl_p->bits.afe_en = en;
	adc_ctl_p->bits.adc_en = en;
	return 0;
}

/**
 * tvd_dbgmode_dump_data
 * Enable tvd debug mode, and dump a specified size section of memory,
 *  and then work back on normal state.
 * @chan_sel     : the index of tvd
 * @mode         : ADC dump mode, 0: dump inmediately,
 *                  1: dump start from the next vsync
 * @dump_dst_addr: the address of the section memory
 * @data_length  : the length of memory,unit half word, must be divisible by 4.
 *
 * Return: 0
 * author : liangyd
 * date : 2016-02-03
 * time:
 * version :
 * history :
 */
#if 0
uint32_t tvd_dbgmode_dump_data(uint32_t chan_sel, uint32_t mode, uintptr_t dump_dst_addr,
			  uint32_t data_length)
{
	uint32_t i = 0;
	/* tvd configuration backup */
	__tvd_dev_t *tvd_dev_bak = kmalloc(sizeof(__tvd_dev_t),
	    GFP_KERNEL | __GFP_ZERO);
	__tvd_top_dev_t *tvd_top_bak = kmalloc(sizeof(__tvd_top_dev_t),
	    GFP_KERNEL | __GFP_ZERO);
	if (!tvd_dev_bak || !tvd_top_bak) {
		pr_warn("%s(), malloc memroy fail\n", __func__);
		kfree(tvd_top_bak);
		kfree(tvd_dev_bak);
		return -1;
	}

	tvd_dev_bak->tvd_debug7.bits.agc_half_en =
	    tvd_device[chan_sel]->tvd_debug7.bits.agc_half_en;
	tvd_dev_bak->tvd_debug4.bits.agc_peak_en =
	    tvd_device[chan_sel]->tvd_debug4.bits.agc_peak_en;
	tvd_dev_bak->tvd_debug7.bits.dagc_en =
	    tvd_device[chan_sel]->tvd_debug7.bits.dagc_en;
	tvd_dev_bak->tvd_clamp_agc1.bits.agc_en =
	    tvd_device[chan_sel]->tvd_clamp_agc1.bits.agc_en;
	tvd_dev_bak->tvd_debug1.bits.afe_gain_value =
	    tvd_device[chan_sel]->tvd_debug1.bits.afe_gain_value;
	tvd_dev_bak->tvd_debug1.bits.afe_gain_mode =
	    tvd_device[chan_sel]->tvd_debug1.bits.afe_gain_mode;
	tvd_dev_bak->tvd_debug1.bits.clamp_mode =
	    tvd_device[chan_sel]->tvd_debug1.bits.clamp_mode;
	tvd_dev_bak->tvd_debug1.bits.tvin_lock_debug =
	    tvd_device[chan_sel]->tvd_debug1.bits.tvin_lock_debug;
	tvd_dev_bak->tvd_debug1.bits.tvin_lock_high =
	    tvd_device[chan_sel]->tvd_debug1.bits.tvin_lock_high;
	tvd_dev_bak->tvd_en.bits.tvd_en_ch =
	    tvd_device[chan_sel]->tvd_en.bits.tvd_en_ch;

	tvd_top_bak->tvd_adc_dump.bits.adc_test_mode =
	    tvd_top_dev->tvd_adc_dump.bits.adc_test_mode;
	tvd_top_bak->tvd_3d_ctl1.bits.tvd_en_3d_dma =
	    tvd_top_dev->tvd_3d_ctl1.bits.tvd_en_3d_dma;
	tvd_top_bak->tvd_3d_ctl1.bits.comb_3d_en =
	    tvd_top_dev->tvd_3d_ctl1.bits.comb_3d_en;
	tvd_top_bak->tvd_3d_ctl4.bits.comb_3d_addr1 =
	    tvd_top_dev->tvd_3d_ctl4.bits.comb_3d_addr1;
	tvd_top_bak->tvd_3d_ctl5.bits.comb_3d_size =
	    tvd_top_dev->tvd_3d_ctl5.bits.comb_3d_size;
	tvd_top_bak->tvd_3d_ctl2.bits.dram_trig =
	    tvd_top_dev->tvd_3d_ctl2.bits.dram_trig;
	tvd_top_bak->tvd_3d_ctl1.bits.comb_3d_sel =
	    tvd_top_dev->tvd_3d_ctl1.bits.comb_3d_sel;
	tvd_top_bak->tvd_3d_ctl1.bits.comb_3d_sel =
	    tvd_device[chan_sel]->tvd_en.bits.tvd_en_ch;
	tvd_top_bak->tvd_adc_dump.bits.adc_dump_mode =
	    tvd_top_dev->tvd_adc_dump.bits.adc_dump_mode;
	tvd_top_bak->tvd_adc_dump.bits.adc_wb_length =
	    tvd_top_dev->tvd_adc_dump.bits.adc_wb_length;

	/* tvd device agc configuration */
	tvd_device[chan_sel]->tvd_debug7.bits.agc_half_en = 0;
	tvd_device[chan_sel]->tvd_debug4.bits.agc_peak_en = 0;
	tvd_device[chan_sel]->tvd_debug7.bits.dagc_en = 0;
	tvd_device[chan_sel]->tvd_clamp_agc1.bits.agc_en = 0;
	tvd_device[chan_sel]->tvd_debug1.bits.afe_gain_value = 64;
	tvd_device[chan_sel]->tvd_debug1.bits.afe_gain_mode = 1;

	/* tvd clamp mode configuration */
	tvd_device[chan_sel]->tvd_debug1.bits.clamp_mode = 1;

	/* tvd lock configuration */
	tvd_device[chan_sel]->tvd_debug1.bits.tvin_lock_debug = 1;
	tvd_device[chan_sel]->tvd_debug1.bits.tvin_lock_high = 0;

	/* delay for lock finish */
	local_delay_ms(50);

	while (tvd_top_dev->tvd_adc_dump.bits.adc_wb_start)
		;

	tvd_top_dev->tvd_adc_dump.bits.adc_test_mode = 1;

	tvd_top_dev->tvd_3d_ctl1.bits.tvd_en_3d_dma = 0;
	tvd_top_dev->tvd_3d_ctl1.bits.comb_3d_en = 0;

	tvd_top_dev->tvd_3d_ctl4.bits.comb_3d_addr1 = dump_dst_addr;
	tvd_top_dev->tvd_3d_ctl5.bits.comb_3d_size = 0x800000 * 2;
	tvd_top_dev->tvd_3d_ctl2.bits.dram_trig = 0x1000;
	tvd_top_dev->tvd_3d_ctl1.bits.comb_3d_sel = chan_sel;

	tvd_top_dev->tvd_3d_ctl1.bits.tvd_en_3d_dma = 1;
	tvd_top_dev->tvd_3d_ctl1.bits.comb_3d_en = 1;

	tvd_device[chan_sel]->tvd_en.bits.tvd_en_ch = 1;

	tvd_top_dev->tvd_adc_dump.bits.adc_dump_mode = mode;
	tvd_top_dev->tvd_adc_dump.bits.adc_wb_length = data_length & (~0xff);
	tvd_top_dev->tvd_adc_dump.bits.adc_wb_buffer_reset = 1;
	/* delay for buffer reset */
	for (i = 0; i < 20; i++)
		;
	tvd_top_dev->tvd_adc_dump.bits.adc_wb_buffer_reset = 0;

	tvd_top_dev->tvd_adc_dump.bits.adc_wb_start = 1;
	/* wait for dump finish */
	while (tvd_top_dev->tvd_adc_dump.bits.adc_wb_start)
		;

	/* reset tvd configuration */
	tvd_device[chan_sel]->tvd_debug7.bits.agc_half_en =
	    tvd_dev_bak->tvd_debug7.bits.agc_half_en;
	tvd_device[chan_sel]->tvd_debug4.bits.agc_peak_en =
	    tvd_dev_bak->tvd_debug4.bits.agc_peak_en;
	tvd_device[chan_sel]->tvd_debug7.bits.dagc_en =
	    tvd_dev_bak->tvd_debug7.bits.dagc_en;
	tvd_device[chan_sel]->tvd_clamp_agc1.bits.agc_en =
	    tvd_dev_bak->tvd_clamp_agc1.bits.agc_en;
	tvd_device[chan_sel]->tvd_debug1.bits.afe_gain_value =
	    tvd_dev_bak->tvd_debug1.bits.afe_gain_value;
	tvd_device[chan_sel]->tvd_debug1.bits.afe_gain_mode =
	    tvd_dev_bak->tvd_debug1.bits.afe_gain_mode;
	tvd_device[chan_sel]->tvd_debug1.bits.clamp_mode =
	    tvd_dev_bak->tvd_debug1.bits.clamp_mode;
	tvd_device[chan_sel]->tvd_debug1.bits.tvin_lock_debug =
	    tvd_dev_bak->tvd_debug1.bits.tvin_lock_debug;
	tvd_device[chan_sel]->tvd_debug1.bits.tvin_lock_high =
	    tvd_dev_bak->tvd_debug1.bits.tvin_lock_high;
	tvd_device[chan_sel]->tvd_en.bits.tvd_en_ch =
	    tvd_dev_bak->tvd_en.bits.tvd_en_ch;

	tvd_top_dev->tvd_adc_dump.bits.adc_test_mode =
	    tvd_top_bak->tvd_adc_dump.bits.adc_test_mode;
	tvd_top_dev->tvd_3d_ctl4.bits.comb_3d_addr1 =
	    tvd_top_bak->tvd_3d_ctl4.bits.comb_3d_addr1;
	tvd_top_dev->tvd_3d_ctl5.bits.comb_3d_size =
	    tvd_top_bak->tvd_3d_ctl5.bits.comb_3d_size;
	tvd_top_dev->tvd_3d_ctl2.bits.dram_trig =
	    tvd_top_bak->tvd_3d_ctl2.bits.dram_trig;
	tvd_top_dev->tvd_3d_ctl1.bits.comb_3d_sel =
	    tvd_top_bak->tvd_3d_ctl1.bits.comb_3d_sel;
	tvd_top_dev->tvd_3d_ctl1.bits.comb_3d_sel =
	    tvd_dev_bak->tvd_en.bits.tvd_en_ch;
	tvd_top_dev->tvd_adc_dump.bits.adc_dump_mode =
	    tvd_top_bak->tvd_adc_dump.bits.adc_dump_mode;
	tvd_top_dev->tvd_adc_dump.bits.adc_wb_length =
	    tvd_top_bak->tvd_adc_dump.bits.adc_wb_length;
	tvd_top_dev->tvd_3d_ctl1.bits.tvd_en_3d_dma =
	    tvd_top_bak->tvd_3d_ctl1.bits.tvd_en_3d_dma;
	tvd_top_dev->tvd_3d_ctl1.bits.comb_3d_en =
	    tvd_top_bak->tvd_3d_ctl1.bits.comb_3d_en;

	kfree(tvd_top_bak);
	kfree(tvd_dev_bak);

	return 0;
}
#endif

void tvd_agc_auto_config(unsigned sel)
{
	tvd_devicep(sel)->tvd_debug7.bits.agc_half_en = 1;
	tvd_devicep(sel)->tvd_debug4.bits.agc_peak_en = 1;
	tvd_devicep(sel)->tvd_debug7.bits.dagc_en = 1;
	tvd_devicep(sel)->tvd_clamp_agc1.bits.agc_en = 1;
	tvd_devicep(sel)->tvd_debug1.bits.afe_gain_mode = 0;
}

void tvd_agc_manual_config(unsigned sel, uint32_t agc_manual_val)
{
	tvd_devicep(sel)->tvd_debug7.bits.agc_half_en = 0;
	tvd_devicep(sel)->tvd_debug4.bits.agc_peak_en = 0;
	tvd_devicep(sel)->tvd_debug7.bits.dagc_en = 0;
	tvd_devicep(sel)->tvd_clamp_agc1.bits.agc_en = 0;
	tvd_devicep(sel)->tvd_debug1.bits.afe_gain_mode = 1;
	tvd_devicep(sel)->tvd_debug1.bits.afe_gain_value = agc_manual_val;
}

void tvd_cagc_config(unsigned sel, uint32_t enable)
{
	tvd_devicep(sel)->tvd_clamp_agc1.bits.cagc_en = enable;
}

#endif /* WITHTVDHW */
