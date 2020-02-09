/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
/*
 * ALSA SoC TLV320AIC23 codec driver
 *
 * Author:      Arun KS, <arunks@mistralsolutions.com>
 * Copyright:   (C) 2008 Mistral Solutions Pvt Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef TLV320AIC23_H_INCLUDED
#define TLV320AIC23_H_INCLUDED

/* Codec TLV320AIC23 */
#define TLV320AIC23_LINVOL              0x00	/* Left line input volume control register */
#define TLV320AIC23_RINVOL              0x01	/* Right line input volume control register */
#define TLV320AIC23_LCHNVOL             0x02	/* Left channel headphone volume control register */
#define TLV320AIC23_RCHNVOL             0x03	/* Right channel headphone volume control register */
#define TLV320AIC23_ANLG                0x04	/* Analog audio path control */
#define TLV320AIC23_DIGT                0x05	/* Digital audio path control */
#define TLV320AIC23_PWR                 0x06	/* Power down control */
#define TLV320AIC23_DIGT_FMT            0x07	/* Digital audio interface format */
#define TLV320AIC23_SRATE               0x08	/* Sample rate control */
#define TLV320AIC23_ACTIVE              0x09	/* Digital interface activation */
#define TLV320AIC23_RESET               0x0F	/* Reset register */

/* Left (right) line input volume control register */
#define TLV320AIC23_LRS_ENABLED         0x0100
#define TLV320AIC23_LIM_MUTED           0x0080
#define TLV320AIC23_LIV_DEFAULT         0x0017
#define TLV320AIC23_LIV_MAX             0x001f
#define TLV320AIC23_LIV_MIN             0x0000

/* Left (right) channel headphone volume control register */
#define TLV320AIC23_LZC_ON              0x0080
//#define TLV320AIC23_LHV_DEFAULT         0x0079
#define TLV320AIC23_LHV_MAX             0x0079
#define TLV320AIC23_LHV_MAX_overload    0x007f
#define TLV320AIC23_LHV_MIN             0x0010

/* Analog audio path control register */
#define TLV320AIC23_STA_REG(x)          ((x)<<6)
#define TLV320AIC23_STE_ENABLED         0x0020
#define TLV320AIC23_DAC_SELECTED        0x0010
#define TLV320AIC23_BYPASS_ON           0x0008
#define TLV320AIC23_INSEL_MIC           0x0004
#define TLV320AIC23_MICM_MUTED          0x0002
#define TLV320AIC23_MICB_20DB           0x0001

/* Digital audio path control register */
#define TLV320AIC23_DACM_MUTE           0x0008
#define TLV320AIC23_DEEMP_32K           0x0002
#define TLV320AIC23_DEEMP_44K           0x0004
#define TLV320AIC23_DEEMP_48K           0x0006
#define TLV320AIC23_ADCHP_ONFF          0x0001	/* ПРОВЕРЕНО: ФВЧ перед АЦП - наличие бита означает ВЫКЛЮЧИТЬ */

/* Power control down register */
#define TLV320AIC23_DEVICE_PWR_OFF      0x0080
#define TLV320AIC23_CLK_OFF             0x0040
#define TLV320AIC23_OSC_OFF             0x0020
#define TLV320AIC23_OUT_OFF             0x0010
#define TLV320AIC23_DAC_OFF             0x0008
#define TLV320AIC23_ADC_OFF             0x0004
#define TLV320AIC23_MIC_OFF             0x0002
#define TLV320AIC23_LINE_OFF            0x0001

/* Digital audio interface register */
#define TLV320AIC23_MS_MASTER           0x0040
#define TLV320AIC23_LRSWAP_ON           0x0020
#define TLV320AIC23_LRP_ON              0x0010
#define TLV320AIC23_IWL_16              0x0000
#define TLV320AIC23_IWL_20              0x0004
#define TLV320AIC23_IWL_24              0x0008
#define TLV320AIC23_IWL_32              0x000C
#define TLV320AIC23_FOR_I2S             0x0002
#define TLV320AIC23_FOR_DSP             0x0003
#define TLV320AIC23_FOR_LJUST           0x0001

/* Sample rate control register */
#define TLV320AIC23_CLKOUT_HALF         0x0080
#define TLV320AIC23_CLKIN_HALF          0x0040
#define TLV320AIC23_BOSR_384fs          0x0002  /* BOSR_272fs in USB mode */
#define TLV320AIC23_USB_CLK_ON          0x0001
#define TLV320AIC23_SR_MASK             0xf
#define TLV320AIC23_CLKOUT_SHIFT        7
#define TLV320AIC23_CLKIN_SHIFT         6
#define TLV320AIC23_SR_SHIFT            2
#define TLV320AIC23_BOSR_SHIFT          1

/* Digital interface register */
#define TLV320AIC23_ACT_ON              0x0001

/*
 * AUDIO related MACROS
 */

#define TLV320AIC23_OUT_VOL_MIN         TLV320AIC23_LHV_MIN
#define TLV320AIC23_OUT_VOL_MAX         TLV320AIC23_LHV_MAX
#define TLV320AIC23_OUT_VOL_MASK        0x7f

#define TLV320AIC23_IN_VOL_MIN          TLV320AIC23_LIV_MIN
#define TLV320AIC23_IN_VOL_MAX          TLV320AIC23_LIV_MAX
#define TLV320AIC23_IN_VOL_MASK         0x7f



#if defined (CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_TLV320AIC23B)
	//#define WITHAFCODEC1HAVEPROC	1	/* кодек имеет управление обработкой микрофонного сигнала */
	#define WITHAFCODEC1HAVELINEINLEVEL	1	/* кодек имеет управление усилением с линейного входа */
	#define	HARDWARE_CODEC1_NPROCPARAMS	5
#endif /* defined (CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_TLV320AIC23B) */

#endif /* TLV320AIC23_H_INCLUDED */
