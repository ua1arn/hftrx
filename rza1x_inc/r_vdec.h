/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file         r_vdec.h
* @version      0.05
* @brief        VDCE driver API
******************************************************************************/

#ifndef R_VDEC_H
#define R_VDEC_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdec_user.h"


#ifdef  __cplusplus
extern  "C"
{
#endif  /* __cplusplus */


/******************************************************************************
Macro definitions
******************************************************************************/
    /*! The number of chroma filter TAP coefficient values (F0 to F8) */
#define     VDEC_CHRFIL_TAPCOEF_NUM     (9)


/******************************************************************************
Typedef definitions
******************************************************************************/
/*! Error code */
typedef enum
{
    VDEC_OK = 0,                /*!< Normal completion */
    VDEC_ERR_CHANNEL,           /*!< Channel error */
    VDEC_ERR_PARAM,             /*!< Parameter error */
    VDEC_ERR_NUM                /*!< The number of the error codes */
} vdec_error_t;

/*! Channel */
typedef enum
{
    VDEC_CHANNEL_0 = 0,         /*!< VDEC channel 0 */
    VDEC_CHANNEL_1,             /*!< VDEC channel 1 */
    VDEC_CHANNEL_NUM            /*!< The number of VDEC channels */
} vdec_channel_t;

/*! On/Off */
typedef enum
{
    VDEC_OFF    = 0,            /*!< Off */
    VDEC_ON     = 1             /*!< On */
} vdec_onoff_t;

/***********************    For R_VDEC_Initialize       ***********************/
/*! Input pin control */
typedef enum
{
    VDEC_ADC_VINSEL_VIN1 = 0,       /*!< CVBS input pin 1 */
    VDEC_ADC_VINSEL_VIN2,           /*!< CVBS input pin 2 */
    VDEC_ADC_VINSEL_NUM             /*!< The number of CVBS input pin select settings */
} vdec_adc_vinsel_t;

/***********************    For R_VDEC_ActivePeriod     ***********************/
/*! Active image period */
typedef struct
{
    uint16_t    srcleft;            /*!< Left end of input video signal capturing area */
    uint16_t    srctop;             /*!< Top end of input video signal capturing area */
    uint16_t    srcheight;          /*!< Height of input video signal capturing area */
    uint16_t    srcwidth;           /*!< Width of input video signal capturing area */
} vdec_active_period_t;

/***********************    For R_VDEC_SyncSeparation   ***********************/
/*! Pre-filter cutoff frequency before vertical sync separation */
typedef enum
{
    VDEC_LPF_VSYNC_NONE = 0,        /*!< None */
    VDEC_LPF_VSYNC_0_94,            /*!< 0.94 MHz */
    VDEC_LPF_VSYNC_0_67,            /*!< 0.67 MHz */
    VDEC_LPF_VSYNC_0_54,            /*!< 0.54 MHz */
    VDEC_LPF_VSYNC_0_47,            /*!< 0.47 MHz */
    VDEC_LPF_VSYNC_0_34,            /*!< 0.34 MHz */
    VDEC_LPF_VSYNC_0_27,            /*!< 0.27 MHz */
    VDEC_LPF_VSYNC_0_23,            /*!< 0.23 MHz */
    VDEC_LPF_VSYNC_NUM
} vdec_lpfvsync_t;
/*! Pre-filter cutoff frequency before horizontal sync separation */
typedef enum
{
    VDEC_LPF_HSYNC_NONE = 0,        /*!< None */
    VDEC_LPF_HSYNC_2_15,            /*!< 2.15 MHz */
    VDEC_LPF_HSYNC_1_88,            /*!< 1.88 MHz */
    VDEC_LPF_HSYNC_1_34,            /*!< 1.34 MHz */
    VDEC_LPF_HSYNC_1_07,            /*!< 1.07 MHz */
    VDEC_LPF_HSYNC_0_94,            /*!< 0.94 MHz */
    VDEC_LPF_HSYNC_0_67,            /*!< 0.67 MHz */
    VDEC_LPF_HSYNC_0_54,            /*!< 0.54 MHz */
    VDEC_LPF_HSYNC_NUM
} vdec_lpfhsync_t;
/*! Noise reduction LPF */
typedef struct
{
    vdec_lpfvsync_t lpfvsync;       /*!< LPF cutoff frequency before vertical sync separation */
    vdec_lpfhsync_t lpfhsync;       /*!< LPF cutoff frequency before horizontal sync separation */
} vdec_noise_rd_lpf_t;
/*! Reference level operation speed control for composite sync separation (for Hsync signal) */
typedef enum
{
    VDEC_VELOCITY_SHIFT_1 = 0,      /*!< x1 */
    VDEC_VELOCITY_SHIFT_2,          /*!< x2 */
    VDEC_VELOCITY_SHIFT_4,          /*!< x4 */
    VDEC_VELOCITY_SHIFT_8,          /*!< x8 */
    VDEC_VELOCITY_SHIFT_16,         /*!< x16 */
    VDEC_VELOCITY_SHIFT_32,         /*!< x32 */
    VDEC_VELOCITY_SHIFT_64,         /*!< x64 */
    VDEC_VELOCITY_SHIFT_128,        /*!< x128 */
    VDEC_VELOCITY_SHIFT_256,        /*!< x256 */
    VDEC_VELOCITY_SHIFT_NUM
} vdec_velocityshift_h_t;
/*! Auto-slice level setting for composite sync separator circuit */
typedef enum
{
    VDEC_SLICE_MODE_MANULAL = 0,    /*!< Manual setting */
    VDEC_SLICE_MODE_AUTO_25,        /*!< 25% of sync depth (Auto) */
    VDEC_SLICE_MODE_AUTO_50,        /*!< 50% of sync depth (Auto) */
    VDEC_SLICE_MODE_AUTO_75,        /*!< 75% of sync depth (Auto) */
    VDEC_SLICE_MODE_NUM
} vdec_slicermode_t;
/*! Clipping level */
typedef enum
{
    VDEC_CLIP_LV_512 = 0,           /*!< 512 */
    VDEC_CLIP_LV_546,               /*!< 546 */
    VDEC_CLIP_LV_580,               /*!< 580 */
    VDEC_CLIP_LV_614,               /*!< 614 */
    VDEC_CLIP_LV_648,               /*!< 648 */
    VDEC_CLIP_LV_682,               /*!< 682 */
    VDEC_CLIP_LV_716,               /*!< 716 */
    VDEC_CLIP_LV_750,               /*!< 750 */
    VDEC_CLIP_LV_785,               /*!< 785 */
    VDEC_CLIP_LV_819,               /*!< 819 */
    VDEC_CLIP_LV_853,               /*!< 853 */
    VDEC_CLIP_LV_887,               /*!< 887 */
    VDEC_CLIP_LV_921,               /*!< 921 */
    VDEC_CLIP_LV_955,               /*!< 955 */
    VDEC_CLIP_LV_989,               /*!< 989 */
    VDEC_CLIP_LV_1023,              /*!< 1023 */
    VDEC_CLIP_LV_NUM
} vdec_ssclipsel_t;
/*! Sync slicer */
typedef struct
{
    vdec_velocityshift_h_t  velocityshift_h;    /*!< Reference level operation speed control for
                                                     composite sync separation (for Hsync signal) */
    vdec_slicermode_t       slicermode_h;       /*!< Auto-slice level setting for composite sync separator circuit
                                                     (for Hsync signal) */
    vdec_slicermode_t       slicermode_v;       /*!< Auto-slice level setting for composite sync separation circuit
                                                     (for Vsync signal) */
    uint16_t                syncmaxduty_h;      /*!< Max ratio of horizontal cycle
                                                     to horizontal sync signal pulse width (for Hsync signal) */
    uint16_t                syncminduty_h;      /*!< Min ratio of horizontal cycle
                                                     to horizontal sync signal pulse width (for Hsync signal) */
    vdec_ssclipsel_t        ssclipsel;          /*!< Clipping level */
    uint16_t                csyncslice_h;       /*!< Slice level for composite sync signal separation
                                                     (for Hsync signal) */
    uint16_t                syncmaxduty_v;      /*!< Max ratio of horizontal cycle
                                                     to horizontal sync signal pulse width (for Vsync signal) */
    uint16_t                syncminduty_v;      /*!< Min ratio of horizontal cycle
                                                     to horizontal sync signal pulse width (for Vsync signal) */
    vdec_onoff_t            vsyncdelay;         /*!< Delays the separated vertical sync signal
                                                     for 1/4 horizontal cycle */
    uint16_t                vsyncslice;         /*!< Threshold for vertical sync separation */
    uint16_t                csyncslice_v;       /*!< Slice level for composite sync signal separation
                                                     (for Vsync signal) */
} vdec_sync_slicer_t;
/*! Horizontal AFC VBI period operating mode */
typedef enum
{
    VDEC_HAFCMD_FIX_PHST = 0,               /*!< Loop gain is fixed and phase comparison is stopped
                                                 during VBI period */
    VDEC_HAFCMD_FIX_LGRD,                   /*!< Loop gain is fixed and loop gain is reduced during VBI period */
    VDEC_HAFCMD_AUTO_PHST,                  /*!< Loop gain is automatically controlled and
                                                 phase comparison is stopped during VBI period */
    VDEC_HAFCMD_AUTO_LGRD,                  /*!< Loop gain is automatically controlled and loop gain is reduced
                                                 during VBI period */
    VDEC_HAFCMD_NUM
} vdec_hafcmode_t;
/*! Phase comparator feedback adjust for low sync signal lock stability */
typedef enum
{
    VDEC_PHDET_DIV_1_1 = 0,                 /*!< 1/1 */
    VDEC_PHDET_DIV_1_2,                     /*!< 1/2 */
    VDEC_PHDET_DIV_1_4,                     /*!< 1/4 */
    VDEC_PHDET_DIV_1_8,                     /*!< 1/8 */
    VDEC_PHDET_DIV_1_16,                    /*!< 1/16 */
    VDEC_PHDET_DIV_1_32,                    /*!< 1/32 */
    VDEC_PHDET_DIV_NUM
} vdec_phdet_div_t;
/*! Horizontal AFC */
typedef struct
{
    uint16_t            hafcgain;           /*!< Horizontal AFC loop gain */
    uint16_t            hafctyp;            /*!< Horizontal AFC center oscillation frequency */
    uint16_t            hafcstart;          /*!< Start line of horizontal AFC normal operation */
    vdec_onoff_t        nox2hosc;           /*!< Disable of horizontal AFC double speed detection */
    uint16_t            hafcmax;            /*!< Maximum oscillation frequency of horizontal AFC */
    uint16_t            hafcend;            /*!< End line of horizontal AFC normal operation */
    vdec_hafcmode_t     hafcmode;           /*!< Horizontal AFC VBI period operating mode */
    uint16_t            hafcmin;            /*!< Min oscillation frequency of horizontal AFC */
    vdec_onoff_t        phdet_fix;          /*!< Forcible or LOWGAIN control */
    vdec_phdet_div_t    phdet_div;          /*!< Phase comparator feedback adjust for low sync signal lock stability */
} vdec_horizontal_afc_t;
/*! Vertical countdown center oscillation frequency */
typedef enum
{
    VDEC_VCD_FRQ_AUTO = 0,                  /*!< Auto-detection */
    VDEC_VCD_FRQ_50HZ,                      /*!< 50.00 Hz */
    VDEC_VCD_FRQ_59_94HZ,                   /*!< 59.94 Hz */
    VDEC_VCD_FRQ_60HZ,                      /*!< 60.00 Hz */
    VDEC_VCD_FRQ_NUM
} vdec_vcddefault_t;
/*! Vertical countdown */
typedef struct
{
    vdec_onoff_t        novcd50_;           /*!< Vertical countdown 50-Hz oscillation mode */
    vdec_onoff_t        novcd60_;           /*!< Vertical countdown 60-Hz (59.94-Hz) oscillation mode */
    vdec_vcddefault_t   vcddefault;         /*!< Vertical countdown center oscillation frequency */
    uint16_t            vcdwindow;          /*!< Vertical countdown sync area */
    uint16_t            vcdoffset;          /*!< Vertical countdown minimum oscillation frequency */
} vdec_vcount_down_t;
/*! AGC / PGA */
typedef struct
{
    vdec_onoff_t    agcmode;                /*!< A/D converter AGC ON/OFF control & PGA gain select */
    vdec_onoff_t    doreduce;               /*!< Manual control of sync signal amplitude detection during VBI period */
    vdec_onoff_t    noreduce_;              /*!< Control of sync signal amplitude detection during VBI period */
    uint16_t        agcresponse;            /*!< AGC response speed */
    uint16_t        agclevel;               /*!< Sync signal reference amplitude */
    uint16_t        agcprecis;              /*!< AGC gain adjustment accuracy */
    uint16_t        pga_gain;               /*!< PGA gain */
} vdec_agc_t;
/*! Peak luminance value to operate peak limiter */
typedef enum
{
    VDEC_PEAKLV_LIM_OFF =0,                 /*!< Limiter OFF */
    VDEC_PEAKLV_1008,                       /*!< 1008 LSB */
    VDEC_PEAKLV_992,                        /*!< 992 LSB */
    VDEC_PEAKLV_960,                        /*!< 960 LSB */
    VDEC_PEAKLV_NUM
} vdec_peaklevel_t;
/*! Maximum compression rate of peak limiter */
typedef enum
{
    VDEC_PEAKRATIO_50 = 0,                  /*!< Compressed up to 50% */
    VDEC_PEAKRATIO_25,                      /*!< Compressed up to 25% */
    VDEC_PEAKRATIO_12_5,                    /*!< Compressed up to 12.5% */
    VDEC_PEAKRATIO_0,                       /*!< Compressed up to 0% */
    VDEC_PEAKRATIO_NUM
} vdec_peakratio_t;
/*! Peak limiter control */
typedef struct
{
    vdec_peaklevel_t    peaklevel;          /*!< Peak luminance value to operate peak limiter */
    uint16_t            peakattack;         /*!< Response speed with peak limiter gain decreased */
    uint16_t            peakrelease;        /*!< Response speed with peak limiter gain increased */
    vdec_peakratio_t    peakratio;          /*!< Maximum compression rate of peak limiter */
    uint16_t            maxpeaksamples;     /*!< Allowable number of overflowing pixels */
} vdec_peak_limiter_t;
/*! Sync separation */
typedef struct
{
    vdec_noise_rd_lpf_t   * noise_rd_lpf;       /*!< Noise reduction LPF */
    vdec_sync_slicer_t    * sync_slicer;        /*!< Sync slicer */
    vdec_horizontal_afc_t * horizontal_afc;     /*!< Horizontal AFC */
    vdec_vcount_down_t    * vcount_down;        /*!< Vertical countdown */
    vdec_agc_t            * agc;                /*!< AGC / PGA */
    vdec_peak_limiter_t   * peak_limiter;       /*!< Peak limiter control */
} vdec_sync_separation_t;

/***********************    For R_VDEC_YcSeparation     ***********************/
/*! Over-range control */
typedef struct
{
    uint16_t        radj_o_level0;          /*!< A/D over-threshold level (between levels 0 and 1) */
    uint16_t        radj_u_level0;          /*!< A/D under-threshold level (between levels 2 and 3) */
    uint16_t        radj_o_level1;          /*!< A/D over-threshold level (between levels 1 and 2) */
    uint16_t        radj_u_level1;          /*!< A/D under-threshold level (between levels 1 and 2) */
    uint16_t        radj_o_level2;          /*!< A/D over-threshold level (between levels 2 and 3) */
    uint16_t        radj_u_level2;          /*!< A/D under-threshold level (between levels 0 and 1) */
    vdec_onoff_t    ucmp_sw;                /*!< Over-range detection */
    vdec_onoff_t    dcmp_sw;                /*!< Under-range detection */
    vdec_onoff_t    hwide_sw;               /*!< Horizontal enlargement of over/under-range level */
} vdec_over_range_t;
/*! Latter-stage BPF select */
typedef enum
{
    VDEC_LSTG_BPFSEL_BYPASS     = 0,        /*!< Bypass */
    VDEC_LSTG_BPFSEL_17TAP      = 1         /*!< 17 TAP */
} vdec_lstg_bpfsel_t;
/*! Former-stage BPF select */
typedef enum
{
    VDEC_FSTG_BPFSEL_17TAP      = 0,        /*!< 17 TAP */
    VDEC_FSTG_BPFSEL_9TAP       = 1         /*!< 9 TAP */
} vdec_fstg_bpfsel_t;
/*! Horizontal filter and horizontal/vertical filter bandwidth switch signal */
typedef enum
{
    VDEC_HFIL_TAP_SEL_17TAP     = 0,        /*!< 17 TAP */
    VDEC_HFIL_TAP_SEL_9TAP      = 1         /*!< 9 TAP */
} vdec_hfil_tap_sel_t;
/*! Mixing ratio of signals after passing filters */
typedef enum
{
    VDEC_FILMIX_RATIO_0 = 0,            /*!< 0% */
    VDEC_FILMIX_RATIO_12_5,             /*!< 12.5% */
    VDEC_FILMIX_RATIO_25,               /*!< 25% */
    VDEC_FILMIX_RATIO_37_5,             /*!< 37.5% */
    VDEC_FILMIX_RATIO_50,               /*!< 50% */
    VDEC_FILMIX_RATIO_62_5,             /*!< 62.5% */
    VDEC_FILMIX_RATIO_75,               /*!< 75% */
    VDEC_FILMIX_RATIO_87_5,             /*!< 87.5% */
    VDEC_FILMIX_RATIO_100,              /*!< 100% */
    VDEC_FILMIX_RATIO_NUM
} vdec_filmix_ratio_t;
/*! Two-dimensional cascade/TAKE-OFF filter mode select */
typedef enum
{
    VDEC_2DFIL_MDSEL_BYPASS = 0,        /*!< Bypass */
    VDEC_2DFIL_MDSEL_CASCADE,           /*!< Cascade filter */
    VDEC_2DFIL_MDSEL_TAKEOFF,           /*!< TAKE-OFF filter */
    VDEC_2DFIL_MDSEL_NUM
} vdec_2dfil_mdsel_t;
/*! Two-dimensional cascade filter select */
typedef enum
{
    VDEC_2D_FIL_SEL_BYPASS      = 0,        /*!< Bypass */
    VDEC_2D_FIL_SEL_17TAP       = 1         /*!< 17 TAP */
} vdec_2d_fil_sel_t;
/*! Y/C separation control */
typedef struct
{
    uint16_t            k15;            /*!< Two-dimensional Y/C separation filter select coefficient, K15 */
    uint16_t            k13;            /*!< Two-dimensional Y/C separation filter select coefficient, K13 */
    uint16_t            k11;            /*!< Two-dimensional Y/C separation filter select coefficient, K11 */
    uint16_t            k16;            /*!< Two-dimensional Y/C separation filter select coefficient, K16 */
    uint16_t            k14;            /*!< Two-dimensional Y/C separation filter select coefficient, K14 */
    uint16_t            k12;            /*!< Two-dimensional Y/C separation filter select coefficient, K12 */
    uint16_t            k22a;           /*!< Two-dimensional Y/C separation filter select coefficient, K22A */
    uint16_t            k21a;           /*!< Two-dimensional Y/C separation filter select coefficient, K21A */
    uint16_t            k22b;           /*!< Two-dimensional Y/C separation filter select coefficient, K22B */
    uint16_t            k21b;           /*!< Two-dimensional Y/C separation filter select coefficient, K21B */
    uint16_t            k23b;           /*!< Two-dimensional Y/C separation filter select coefficient, K23B */
    uint16_t            k23a;           /*!< Two-dimensional Y/C separation filter select coefficient, K23A */
    uint16_t            k24;            /*!< Two-dimensional Y/C separation filter select coefficient, K24 */
    vdec_lstg_bpfsel_t  hbpf_narrow;    /*!< Latter-stage horizontal BPF select */
    vdec_lstg_bpfsel_t  hvbpf_narrow;   /*!< Latter-stage horizontal/vertical BPF select */
    vdec_fstg_bpfsel_t  hbpf1_9tap_on;  /*!< Former-stage horizontal BPF select */
    vdec_fstg_bpfsel_t  hvbpf1_9tap_on; /*!< Former-stage horizontal/vertical BPF select */
    vdec_hfil_tap_sel_t hfil_tap_sel;   /*!< Horizontal filter and horizontal/vertical filter
                                             bandwidth switch signal */
    vdec_onoff_t        det2_on;        /*!< Two-dimensional filter mixing select */
    vdec_filmix_ratio_t hsel_mix_y;     /*!< Mixing ratio of signal after passing horizontal filter
                                             to signal after passing former-stage horizontal filter */
    vdec_filmix_ratio_t vsel_mix_y;     /*!< Mixing ratio of signal after passing vertical filter
                                             to signal after passing former-stage horizontal/vertical filter */
    vdec_filmix_ratio_t hvsel_mix_y;    /*!< Mixing ratio of signal after passing horizontal/vertical filter
                                             to signal after passing former-stage horizontal/vertical filter */
    uint16_t            v_y_level;      /*!< Vertical luminance detection level for correlation detection filter */
    vdec_filmix_ratio_t det2_mix_c;     /*!< Mixing ratio of c signal after passing horizontal/vertical adaptive filter
                                             to signal after passing correlation detection filter */
    vdec_filmix_ratio_t det2_mix_y;     /*!< Mixing ratio of c signal for y generation after passing
                                             horizontal/vertical adaptive filter to signal after passing correlation */
    vdec_2dfil_mdsel_t  fil2_mode_2d;   /*!< Two-dimensional cascade/TAKE-OFF filter mode select */
    vdec_2d_fil_sel_t   fil2_narrow_2d; /*!< Two-dimensional cascade filter select */
} vdec_yc_sep_ctrl_t;
/*! Chroma filter TAP coefficient */
typedef struct
{
    uint16_t    fil2_2d_f[VDEC_CHRFIL_TAPCOEF_NUM];     /*!< Chroma filter TAP coefficient 0 to 8 */
} vdec_chrfil_tap_t;
/*! Y/C separation */
typedef struct
{
    vdec_over_range_t         * over_range;     /*!< Over-range control */
    vdec_yc_sep_ctrl_t        * yc_sep_ctrl;    /*!< Y/C separation control */
    const vdec_chrfil_tap_t   * fil2_2d_wa;     /*!< Two-dimensional cascade broadband (3.58/4.43/SECAM-DR)/TAKE-OFF
                                                     filter TAP coefficient */
    const vdec_chrfil_tap_t * fil2_2d_wb;       /*!< Two-dimensional cascade broadband (SECAM-DB) filter
                                                     TAP coefficient */
    const vdec_chrfil_tap_t * fil2_2d_na;       /*!< Two-dimensional cascade narrowband (3.58/4.43/SECAM-DR) filter
                                                     TAP coefficient */
    const vdec_chrfil_tap_t * fil2_2d_nb;       /*!< Two-dimensional cascade narrowband (SECAMDB) filter
                                                     TAP coefficient */
} vdec_yc_separation_t;

/***********************    For R_VDEC_ChromaDecoding   ***********************/
/*! Color system */
typedef enum
{
    VDEC_COL_SYS_NTSC = 0,                  /*!< NTSC */
    VDEC_COL_SYS_PAL,                       /*!< PAL */
    VDEC_COL_SYS_SECAM,                     /*!< SECAM */
    VDEC_COL_SYS_NON,                       /*!< Not specified/undetectable */
    VDEC_COL_SYS_NUM                        /*!< The number of color system settings */
} vdec_color_sys_t;
/*! Averaging processing for pre-demodulated line */
typedef enum
{
    VDEC_DEMOD_MD_NO    = 0,                /*!< No processing */
    VDEC_DEMOD_MD_PAL   = 2                 /*!< For PAL */
} vdec_demodmode_t;
/*! Chroma decoding control */
typedef struct
{
    vdec_color_sys_t    defaultsys;         /*!< Default color system */
    vdec_onoff_t        nontsc358_;         /*!< NTSC-M detection control */
    vdec_onoff_t        nontsc443_;         /*!< NTSC-4.43 detection control */
    vdec_onoff_t        nopalm_;            /*!< PAL-M detection control */
    vdec_onoff_t        nopaln_;            /*!< PAL-N detection control */
    vdec_onoff_t        nopal443_;          /*!< PAL-B, G, H, I, D detection control */
    vdec_onoff_t        nosecam_;           /*!< SECAM detection control */
    uint16_t            lumadelay;          /*!< Luminance signal delay adjustment */
    vdec_onoff_t        chromalpf;          /*!< LPF for demodulated chroma */
    vdec_demodmode_t    demodmode;          /*!< Averaging processing for pre-demodulated line */
} vdec_chrmdec_ctrl_t;
/*! Burst lock PLL lock range */
typedef enum
{
    VDEC_LOCK_RANGE_400HZ = 0,              /*!< +-400 Hz */
    VDEC_LOCK_RANGE_800HZ,                  /*!< +-800 Hz */
    VDEC_LOCK_RANGE_1200HZ,                 /*!< +-1200 Hz */
    VDEC_LOCK_RANGE_1600HZ,                 /*!< +-1600 Hz */
    VDEC_LOCK_RANGE_NUM
} vdec_lockrange_t;
/*! Burst lock */
typedef struct
{
    vdec_lockrange_t    lockrange;          /*!< Burst lock PLL lock range */
    uint16_t            loopgain;           /*!< Burst lock PLL loop gain */
    uint16_t            locklimit;          /*!< Level for burst lock PLL to re-search free-run frequency */
    uint16_t            bgpcheck;           /*!< burst gate pulse position check */
    uint16_t            bgpwidth;           /*!< burst gate pulse width */
    uint16_t            bgpstart;           /*!< burst gate pulse start position */
} vdec_burst_lock_t;
/*! ACC operating mode */
typedef enum
{
    VDEC_ACC_MD_AUTO    = 0,                /*!< Auto gain */
    VDEC_ACC_MD_MANUAL  = 1                 /*!< Manual gain */
} vdec_accmode_t;
/*! Maximum ACC gain */
typedef enum
{
    VDEC_ACC_MAX_GAIN_6 = 0,                /*!< 6 times */
    VDEC_ACC_MAX_GAIN_8,                    /*!< 8 times */
    VDEC_ACC_MAX_GAIN_12,                   /*!< 12 times */
    VDEC_ACC_MAX_GAIN_16,                   /*!< 16 times */
    VDEC_ACC_MAX_GAIN_NUM
} vdec_accmaxgain_t;
/*! Chroma manual gain (sub) */
typedef enum
{
    VDEC_CHRM_SB_GAIN_1 = 0,                /*!< 1 time */
    VDEC_CHRM_SB_GAIN_2,                    /*!< 2 times */
    VDEC_CHRM_SB_GAIN_4,                    /*!< 4 times */
    VDEC_CHRM_SB_GAIN_8,                    /*!< 8 times */
    VDEC_CHRM_SB_GAIN_NUM
} vdec_chrm_subgain_t;
/*! ACC / color killer */
typedef struct
{
    vdec_accmode_t      accmode;            /*!< ACC operating mode */
    vdec_accmaxgain_t   accmaxgain;         /*!< Maximum ACC Gain */
    uint16_t            acclevel;           /*!< ACC reference color burst amplitude */
    vdec_chrm_subgain_t chromasubgain;      /*!< Chroma manual gain (sub) */
    uint16_t            chromamaingain;     /*!< Chroma manual gain (main) */
    uint16_t            accresponse;        /*!< ACC response speed */
    uint16_t            accprecis;          /*!< ACC gain adjustment accuracy */
    vdec_onoff_t        killermode;         /*!< Forced color killer mode ON/OFF */
    uint16_t            killerlevel;        /*!< Color killer operation start point */
    uint16_t            killeroffset;       /*!< The offset level to turn off the color killer */
} vdec_acc_t;
/*! TINT/R-Y axis correction */
typedef struct
{
    uint16_t    tintsub;                    /*!< Fine adjustment of r-y demodulation axis */
    uint16_t    tintmain;                   /*!< Hue adjustment level */
} vdec_tint_ry_t;
/*! Chroma decoding */
typedef struct
{
    vdec_chrmdec_ctrl_t   * chrmdec_ctrl;   /*!< Chroma decoding control */
    vdec_burst_lock_t     * burst_lock;     /*!< Burst lock */
    vdec_acc_t            * acc;            /*!< ACC/color killer */
    vdec_tint_ry_t        * tint_ry;        /*!< TINT/R-Y axis correction (only valid for NTSC/PAL) */
} vdec_chroma_decoding_t;

/***********************    For R_VDEC_DigitalClamp     ***********************/
/*! Clamp level setting mode */
typedef enum
{
    VDEC_DCPMODE_MANUAL = 0,                /*!< Manual clamp level setting */
    VDEC_DCPMODE_AUTO   = 1                 /*!< Auto clamp level setting */
} vdec_dcpmode_t;
/*! Pedestal clamp */
typedef struct
{
    vdec_dcpmode_t      dcpmode_y;          /*!< Clamp level setting mode */
    uint16_t            blanklevel_y;       /*!< Clamp offset level (Y signal) */
    uint16_t            dcppos_y;           /*!< Digital clamp pulse horizontal start position (Y signal) */
} vdec_pedestal_clamp_t;
/*! Center clamp */
typedef struct
{
    vdec_dcpmode_t      dcpmode_c;          /*!< Clamp level setting mode */
    uint16_t            blanklevel_cb;      /*!< Clamp offset level (Cb signal) */
    uint16_t            blanklevel_cr;      /*!< Clamp offset level (Cr signal) */
    uint16_t            dcppos_c;           /*!< Digital clamp pulse horizontal start position (Cb/Cr signal) */
} vdec_center_clamp_t;
/*! Video signal for autocorrelation function */
typedef enum
{
    VDEC_ACFINPUT_Y = 0,                    /*!< Y signal */
    VDEC_ACFINPUT_CB,                       /*!< Cb signal */
    VDEC_ACFINPUT_CR,                       /*!< Cr signal */
    VDEC_ACFINPUT_NUM
} vdec_acfinput_t;
/*! Noise detection */
typedef struct
{
    vdec_acfinput_t     acfinput;           /*!< Video signal for autocorrelation function */
    uint16_t            acflagtime;         /*!< Delay time for autocorrelation function calculation */
    uint16_t            acffilter;          /*!< Smoothing parameter of autocorrelation function data */
} vdec_noise_det_t;
/*! Digital clamp */
typedef struct
{
    vdec_onoff_t            dcpcheck;       /*!< Digital clamp pulse position check */
    uint16_t                dcpresponse;    /*!< Digital clamp response speed */
    uint16_t                dcpstart;       /*!< Digital clamp start line (in 1-line units) */
    uint16_t                dcpend;         /*!< Digital clamp end line (in 1-line units) */
    uint16_t                dcpwidth;       /*!< Digital clamp pulse width */
    vdec_pedestal_clamp_t * pedestal_clamp; /*!< Pedestal clamp */
    vdec_center_clamp_t   * center_clamp;   /*!< Center clamp */
    vdec_noise_det_t      * noise_det;      /*!< Noise detection */
} vdec_degital_clamp_t;

/***********************    For R_VDEC_Output           ***********************/
/*! Output adjustment */
typedef struct
{
    uint16_t    y_gain2;                /*!< Y signal gain coefficient */
    uint16_t    cb_gain2;               /*!< Cb signal gain coefficient */
    uint16_t    cr_gain2;               /*!< Cr signal gain coefficient */
} vdec_output_t;

/***********************    For R_VDEC_Query            ***********************/
/*! Answer */
typedef enum
{
    VDEC_NO         = 0,            /*!< No */
    VDEC_YES        = 1             /*!< Yes */
} vdec_answer_t;
/*! Lock state */
typedef enum
{
    VDEC_UNLOCK     = 0,            /*!< Unlocked */
    VDEC_LOCK       = 1             /*!< Locked */
} vdec_lock_t;
/*! Speed detection result */
typedef enum
{
    VDEC_FHMD_NORMAL        = 0,    /*!< Normal speed */
    VDEC_FHMD_MULTIPLIED    = 1     /*!< Multiplied speed */
} vdec_fhmode_t;
/*! Vertical countdown oscillation mode */
typedef enum
{
    VDEC_FVMD_50HZ  = 0,            /*!< 50 Hz */
    VDEC_FVMD_60HZ  = 1             /*!< 60 Hz */
} vdec_fvmode_t;
/*! Color sub-carrier frequency detection result */
typedef enum
{
    VDEC_FSCMD_3_58     = 0,        /*!< 3.58 MHz */
    VDEC_FSCMD_4_43     = 1         /*!< 4.43 MHz */
} vdec_fscmode_t;
/*! Sync separation (for query) */
typedef struct
{
    vdec_lock_t     fhlock;         /*!< Horizontal AFC lock detection result */
    vdec_answer_t   isnoisy;        /*!< Detection result of low S/N signal by sync separation */
    vdec_fhmode_t   fhmode;         /*!< Speed detection result */
    vdec_answer_t   nosignal_;      /*!< No-signal detection result */
    vdec_lock_t     fvlock;         /*!< Vertical countdown lock detection result */
    vdec_fvmode_t   fvmode;         /*!< Vertical countdown oscillation mode */
    vdec_answer_t   interlaced;     /*!< Interlace detection result */
    uint16_t        fvcount;        /*!< Vertical cycle measurement result */
    uint32_t        fhcount;        /*!< Horizontal AFC oscillation cycle */
    vdec_answer_t   isreduced;      /*!< Sync amplitude detection result during VBI period */
    uint16_t        syncdepth;      /*!< Sync pulse amplitude detection result */
} vdec_q_sync_sep_t;
/*! AGC (for query) */
typedef struct
{
    uint16_t    highsamples;        /*!< Number of pixels which have larger luminance value than peak luminance */
    uint16_t    peaksamples;        /*!< Number of overflowing pixels */
    uint16_t    agcconverge;        /*!< AGC convergence detection result */
    uint16_t    agcgain;            /*!< Current AGC gain value */
} vdec_q_agc_t;
/*! Chroma decoding (for query) */
typedef struct
{
    vdec_color_sys_t    colorsys;   /*!< Color system detection result */
    vdec_fscmode_t      fscmode;    /*!< Color sub-carrier frequency detection result */
    vdec_lock_t         fsclock;    /*!< Burst lock PLL lock state detection result */
    vdec_answer_t       noburst_;   /*!< Color burst detection result */
    vdec_chrm_subgain_t accsubgain; /*!< Current ACC gain value (sub) */
    uint16_t            accmaingain;/*!< Current ACC gain value (main) */
    vdec_answer_t       issecam;    /*!< SECAM detection result */
    vdec_answer_t       ispal;      /*!< PAL detection result */
    vdec_answer_t       isntsc;     /*!< NTSC detection result */
    uint16_t            locklevel;  /*!< Low S/N signal detection result by burst lock PLL */
} vdec_q_chroma_dec_t;
/*! Digital clamp (for query) */
typedef struct
{
    uint16_t    clamplevel_cb;      /*!< Digital clamp subtraction value (Cb) */
    uint16_t    clamplevel_y;       /*!< Digital clamp subtraction value (Y) */
    uint16_t    clamplevel_cr;      /*!< Digital clamp subtraction value (Cr) */
    uint16_t    acfstrength;        /*!< Noise autocorrelation strength at digital clamp pulse position */
} vdec_q_digital_clamp_t;


/******************************************************************************
Functions Prototypes
******************************************************************************/
vdec_error_t R_VDEC_Initialize(
    const vdec_channel_t    ch,
    const vdec_adc_vinsel_t vinsel,
    void           (* const init_func)(uint32_t),
    const uint32_t          user_num);
vdec_error_t R_VDEC_Terminate(const vdec_channel_t ch, void (* const quit_func)(uint32_t), const uint32_t user_num);
vdec_error_t R_VDEC_ActivePeriod(const vdec_channel_t ch, const vdec_active_period_t * const param);
vdec_error_t R_VDEC_SyncSeparation(const vdec_channel_t ch, const vdec_sync_separation_t * const param);
vdec_error_t R_VDEC_YcSeparation(const vdec_channel_t ch, const vdec_yc_separation_t * const param);
vdec_error_t R_VDEC_ChromaDecoding(const vdec_channel_t ch, const vdec_chroma_decoding_t * const param);
vdec_error_t R_VDEC_DigitalClamp(const vdec_channel_t ch, const vdec_degital_clamp_t * const param);
vdec_error_t R_VDEC_Output(const vdec_channel_t ch, const vdec_output_t * const param);
vdec_error_t R_VDEC_Query(
    const vdec_channel_t            ch,
    vdec_q_sync_sep_t       * const q_sync_sep,
    vdec_q_agc_t            * const q_agc,
    vdec_q_chroma_dec_t     * const q_chroma_dec,
    vdec_q_digital_clamp_t  * const q_digital_clamp);


#ifdef  __cplusplus
}
#endif  /* __cplusplus */


#endif  /* R_VDEC_H */

