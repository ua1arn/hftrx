/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//


#include "hardware.h"


#if LCDMODEX_SII9022A
/* sii9022AA Lattice Semiconductor Corp. HDMI Transmitter */

#include "board.h"
#include "display.h"
#include "formats.h"
#include "spi.h"	// hardware_spi_master_send_frame
#include "display2.h"
#include <string.h>

/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

//#include <linux/i2c.h>
//#include <linux/delay.h>
//#include "msm_fb.h"


// Transmitter Programming Interface (TPI) device address
// CI2CA = LOW, 49-Ball / 81-Ball / 72-Pin
enum { SII9022_ADDR_W = 0x72, SII9022_ADDR_R = SII9022_ADDR_W | 0x01 };

// See
//	SiI9020 HDMI PanelLink Transmitter Programmer's Reference
//	SiI-PR-1032-0.74
//	https://github.com/facchinm/kernel-brain/tree/master/drivers/hdmi

#define SII9022_ID_902xA		0xb0

#define HDMI_I2C_MONITOR_ADDRESS	0x50

#define SII9022_VIDEO_DATA_BASE_REG	0x00
#define SII9022_PIXEL_CLK_LSB_REG	(SII9022_VIDEO_DATA_BASE_REG + 0x00)
#define SII9022_PIXEL_CLK_MSB_REG	(SII9022_VIDEO_DATA_BASE_REG + 0x01)
#define SII9022_VFREQ_LSB_REG		(SII9022_VIDEO_DATA_BASE_REG + 0x02)
#define SII9022_VFREQ_MSB_REG		(SII9022_VIDEO_DATA_BASE_REG + 0x03)
#define SII9022_PIXELS_LSB_REG		(SII9022_VIDEO_DATA_BASE_REG + 0x04)
#define SII9022_PIXELS_MSB_REG		(SII9022_VIDEO_DATA_BASE_REG + 0x05)
#define SII9022_LINES_LSB_REG		(SII9022_VIDEO_DATA_BASE_REG + 0x06)
#define SII9022_LINES_MSB_REG		(SII9022_VIDEO_DATA_BASE_REG + 0x07)

#define SII9022_PIXEL_REPETITION_REG	0x08

#define SII9022_AVI_IN_FORMAT_REG	0x09
#define SII9022_AVI_OUT_FORMAT_REG	0x0a
#define SII9022_AVI_INFOFRAME_BASE_REG	0x0c
#define SII9022_AVI_INFOFRAME_LEN	14

#define SII9022_SYS_CTRL_DATA_REG	0x1a
#define SII9022_DEVICE_ID_REG		0x1b
#define SII9022_DEVICE_REV_ID_REG	0x1c
#define SII9022_DEVICE_TPI_ID_REG	0x1d
#define SII9022_DEVICE_HDCP_REV_REG	0x30

#define SII9022_POWER_STATE_CTRL_REG	0x1e


#define SII9022_IRQ_ENABLE_REG		0x3c
#define SII9022_IRQ_STATUS_REG		0x3d

#define SII9022_TPI_RQB_REG		0xc7

#define SII9022_HPD_DELAY_DEBOUNCE	0x7c
#define SII9022_TMDS_CONT_REG		0x82

/* Indirect internal register access */
#define HDMI_IND_SET_PAGE		0xbc
#define HDMI_IND_OFFSET			0xbd
#define HDMI_IND_VALUE			0xbe

/* AVI InfoFrame */
#define HDMI_CPI_MISC_IF_SELECT_REG     0xbf
#define HDMI_CPI_MISC_IF_OFFSET         0xC0

/* Audio  */
#define HDMI_TPI_I2S_ENABLE_MAPPING_REG 0x1f
#define HDMI_TPI_I2S_INPUT_CONFIG_REG   0x20
#define HDMI_TPI_I2S_STRM_HDR_BASE      0x21
#define HDMI_TPI_I2S_STRM_HDR_0_REG     (HDMI_TPI_I2S_STRM_HDR_BASE + 0)
#define HDMI_TPI_I2S_STRM_HDR_1_REG     (HDMI_TPI_I2S_STRM_HDR_BASE + 1)
#define HDMI_TPI_I2S_STRM_HDR_2_REG     (HDMI_TPI_I2S_STRM_HDR_BASE + 2)
#define HDMI_TPI_I2S_STRM_HDR_3_REG     (HDMI_TPI_I2S_STRM_HDR_BASE + 3)
#define HDMI_TPI_I2S_STRM_HDR_4_REG     (HDMI_TPI_I2S_STRM_HDR_BASE + 4)
#define HDMI_TPI_AUDIO_CONFIG_BYTE2_REG	0x26
#define HDMI_TPI_AUDIO_CONFIG_BYTE3_REG	0x27
#define HDMI_TPI_AUDIO_CONFIG_BYTE4_REG	0x28

/* SII9022_SYS_CTRL_DATA_REG */
#define SII9022_SYS_CTRL_DDC_BUS_GRANTED	(1u << 1)
#define SII9022_SYS_CTRL_DDC_BUS_REQUEST	(1u << 2)

/* HDMI_TPI_AUDIO_CONFIG_BYTE2_REG  */
#define TPI_AUDIO_CODING_STREAM_HEADER		(0 << 0)
#define TPI_AUDIO_CODING_PCM			(1 << 0)
#define TPI_AUDIO_CODING_AC3			(2 << 0)
#define TPI_AUDIO_CODING_MPEG1			(3 << 0)
#define TPI_AUDIO_CODING_MP3			(4 << 0)
#define TPI_AUDIO_CODING_MPEG2			(5 << 0)
#define TPI_AUDIO_CODING_AAC			(6 << 0)
#define TPI_AUDIO_CODING_DTS			(7 << 0)
#define TPI_AUDIO_CODING_ATRAC			(8 << 0)
#define TPI_AUDIO_MUTE_DISABLE			(0 << 4)
#define TPI_AUDIO_MUTE_ENABLE			(1 << 4)
#define TPI_AUDIO_LAYOUT_2_CHANNELS		(0 << 5)
#define TPI_AUDIO_LAYOUT_8_CHANNELS		(1 << 5)
#define TPI_AUDIO_INTERFACE_DISABLE		(0 << 6)
#define TPI_AUDIO_INTERFACE_SPDIF		(1 << 6)
#define TPI_AUDIO_INTERFACE_I2S			(2 << 6)

/* HDMI_TPI_AUDIO_CONFIG_BYTE3_REG  */
#define TPI_AUDIO_CHANNEL_STREAM		(0 << 0)
#define TPI_AUDIO_2_CHANNEL			(1 << 0)
#define TPI_AUDIO_8_CHANNEL			(7 << 0)
#define TPI_AUDIO_FREQ_STREAM			(0 << 3)
#define TPI_AUDIO_FREQ_32KHZ			(1 << 3)
#define TPI_AUDIO_FREQ_44KHZ			(2 << 3)
#define TPI_AUDIO_FREQ_48KHZ			(3 << 3)
#define TPI_AUDIO_FREQ_88KHZ			(4 << 3)
#define TPI_AUDIO_FREQ_96KHZ			(5 << 3)
#define TPI_AUDIO_FREQ_176KHZ			(6 << 3)
#define TPI_AUDIO_FREQ_192KHZ			(7 << 3)
#define TPI_AUDIO_SAMPLE_SIZE_STREAM		(0 << 6)
#define TPI_AUDIO_SAMPLE_SIZE_16		(1 << 6)
#define TPI_AUDIO_SAMPLE_SIZE_20		(2 << 6)
#define TPI_AUDIO_SAMPLE_SIZE_24		(3 << 6)

/* HDMI_TPI_I2S_ENABLE_MAPPING_REG  */
#define TPI_I2S_CONFIG_FIFO0			(0 << 0)
#define TPI_I2S_CONFIG_FIFO1			(1 << 0)
#define TPI_I2S_CONFIG_FIFO2			(2 << 0)
#define TPI_I2S_CONFIG_FIFO3			(3 << 0)
#define TPI_I2S_LEFT_RIGHT_SWAP			(1 << 2)
#define TPI_I2S_AUTO_DOWNSAMPLE			(1 << 3)
#define TPI_I2S_SELECT_SD0			(0 << 4)
#define TPI_I2S_SELECT_SD1			(1 << 4)
#define TPI_I2S_SELECT_SD2			(2 << 4)
#define TPI_I2S_SELECT_SD3			(3 << 4)
#define TPI_I2S_FIFO_ENABLE			(1 << 7)

/* HDMI_TPI_I2S_INPUT_CONFIG_REG  */
#define TPI_I2S_FIRST_BIT_SHIFT_YES		(0 << 0)
#define TPI_I2S_FIRST_BIT_SHIFT_NO		(1 << 0)
#define TPI_I2S_SD_DIRECTION_MSB_FIRST		(0 << 1)
#define TPI_I2S_SD_DIRECTION_LSB_FIRST		(1 << 1)
#define TPI_I2S_SD_JUSTIFY_LEFT			(0 << 2)
#define TPI_I2S_SD_JUSTIFY_RIGHT		(1 << 2)
#define TPI_I2S_WS_POLARITY_LOW			(0 << 3)
#define TPI_I2S_WS_POLARITY_HIGH		(1 << 3)
#define TPI_I2S_MCLK_MULTIPLIER_128		(0 << 4)
#define TPI_I2S_MCLK_MULTIPLIER_256		(1 << 4)
#define TPI_I2S_MCLK_MULTIPLIER_384		(2 << 4)
#define TPI_I2S_MCLK_MULTIPLIER_512		(3 << 4)
#define TPI_I2S_MCLK_MULTIPLIER_768		(4 << 4)
#define TPI_I2S_MCLK_MULTIPLIER_1024		(5 << 4)
#define TPI_I2S_MCLK_MULTIPLIER_1152		(6 << 4)
#define TPI_I2S_MCLK_MULTIPLIER_192		(7 << 4)
#define TPI_I2S_SCK_EDGE_FALLING		(0 << 7)
#define TPI_I2S_SCK_EDGE_RISING			(1 << 7)

#define SII9022_EDID_LEN			256
#define ONE_BLOCK_EDID_LEN			128

enum sii9022x_power_state {
	SII9022_POWER_STATE_D0,
	SII9022_POWER_STATE_D2,
	SII9022_POWER_STATE_D3_HOT,
	SII9022_POWER_STATE_D3_COLD,
};

//#define DEVICE_NAME "sii9022"
#define SII9022_DEVICE_ID   0xB0

struct i2c_adapter { int a; };

struct i2c_client { int dev; int irq; struct i2c_adapter * adapter; };

struct device { int a; };

static int i2c_master_send(
	struct i2c_client *client,
	const uint8_t * buff,
	unsigned cnt
	)
{
	unsigned i;
	i2c2_start(SII9022_ADDR_W);
	for (i = 0; i < cnt; ++ i)
		i2c2_write(buff [i]);
	i2c2_waitsend();
	i2c2_stop();
	return cnt;
}

static int i2c_smbus_read_byte_data(
	struct i2c_client *client,
	unsigned reg
	)
{
	uint8_t v;

	i2c2_start(SII9022_ADDR_W);
	i2c2_write_withrestart(reg);
	i2c2_start(SII9022_ADDR_R);
	i2c2_read(& v, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */

	return v;
//	union i2c_smbus_data data;
//	int status;
//
//	status = i2c_smbus_xfer(client->adapter, client->addr, client->flags,
//				I2C_SMBUS_READ, command,
//				I2C_SMBUS_BYTE_DATA, &data);
//	return (status < 0) ? status : data.byte;
}

static int i2c_smbus_write_byte_data(
	struct i2c_client *client,
	unsigned reg,
	unsigned val
	)
{
	i2c2_start(SII9022_ADDR_W);
	i2c2_write(reg);
	i2c2_write(val);
	i2c2_waitsend();
	i2c2_stop();
	return 0;
//	union i2c_smbus_data data;
//	data.byte = value;
//	return i2c_smbus_xfer(client->adapter, client->addr, client->flags,
//			      I2C_SMBUS_WRITE, command,
//			      I2C_SMBUS_BYTE_DATA, &data);
}

static int sii9022x_regmap_write(uint_fast8_t reg, uint_fast8_t data)
{
	i2c2_start(SII9022_ADDR_W);
	i2c2_write(reg);
	i2c2_write(data);
	i2c2_waitsend();
	i2c2_stop();

	return 0;
}

static int sii9022x_regmap_bulk_write(uint_fast8_t reg, const uint8_t * buff, uint_fast8_t count)
{
	i2c2_start(SII9022_ADDR_W);
	i2c2_write(reg);
	while (count --)
		i2c2_write(* buff ++);
	i2c2_waitsend();
	i2c2_stop();

	return 0;
}

static uint_fast8_t sii9022x_regmap_read(uint_fast8_t reg, int * retvalue)
{
	uint8_t v;

	i2c2_start(SII9022_ADDR_W);
	i2c2_write_withrestart(reg);
	i2c2_start(SII9022_ADDR_R);
	i2c2_read(& v, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
	* retvalue = v;
	return 0;

}

static int32_t i2c2periph_writeN(uint_fast8_t d_adr, uint32_t w_byte, const uint8_t * w_buffer)
{
#if WITHTWISW

	i2c2_start(d_adr * 2);	// write access
	while (w_byte --)
		i2c2_write(* w_buffer ++);
	//i2c2_stop();
	return 0;

#elif WITHTWIHW

	uint8_t bufw = r_adr;
	i2c2hw_write(d_adr, & bufw, 1);				// write access
	i2c2hw_write(d_adr, r_buffer, r_byte);					// write access
	return 0;

#endif
}

static int32_t i2c2periph_readN(uint_fast8_t d_adr, uint32_t r_byte, uint8_t * r_buffer)
{
#if WITHTWISW

	i2c2_start(d_adr * 2 | 0x01);	// read access
	if (r_byte == 1)
	{
		uint_fast8_t v;
		i2c2_read(r_buffer, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
		return 0;
	}
	else if (r_byte == 2)
	{
		i2c2_read(r_buffer ++, I2C_READ_ACK_1);	/* чтение первого байта ответа */
		i2c2_read(r_buffer ++, I2C_READ_NACK);	/* чтение последнего байта ответа */
		return 0;
	}
	else
	{
		i2c2_read(r_buffer ++, I2C_READ_ACK_1);	/* чтение первого байта ответа */
		while (r_byte -- > 2)
		{
			i2c2_read(r_buffer ++, I2C_READ_ACK);	/* чтение промежуточного байта ответа */
		}
		i2c2_read(r_buffer ++, I2C_READ_NACK);	/* чтение последнего байта ответа */
		return 0;
	}

#elif WITHTWIHW

	uint8_t bufw = r_adr;
	i2c2hw_write(d_adr, & bufw, 1);				// write access
	i2c2hw_read(d_adr, r_buffer, r_byte);		// read access
	return 0;

#endif
}

struct i2c_msg {
        uint16_t addr; /* slave address            */
        uint16_t flags;
#define I2C_M_TEN       0x0010  /* this is a ten bit chip address */
#define I2C_M_RD        0x0001  /* read data, from slave to master */
#define I2C_M_STOP      0x8000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NOSTART       0x4000  /* if I2C_FUNC_NOSTART */
#define I2C_M_REV_DIR_ADDR  0x2000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK    0x1000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NO_RD_ACK     0x0800  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_RECV_LEN      0x0400  /* length will be first received byte */
        uint16_t len;      /* msg length               */
        uint8_t *buf;      /* pointer to msg data          */
};

// DDC Master access
int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
	unsigned msg;
	for (msg = 0; msg < num; ++ msg)
	{
		if (msgs [msg].flags & I2C_M_RD)
		{
			i2c2periph_readN(msgs [msg].addr, msgs [msg].len, msgs [msg].buf);
		}
		else
		{
			i2c2periph_writeN(msgs [msg].addr, msgs [msg].len, msgs [msg].buf);
		}
	}
	return 0;
	return -1;
}

#if 1
//	https://github.com/facchinm/kernel-brain/blob/master/drivers/hdmi/encoder-sii9022.c

/*
 * Copyright (C) 2014 Atmel
 *		      Bo Shen <voice.shen@atmel.com>
 *
 * Copyright (C) 2010-2011 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//#include "encoder-sii9022.h"

/*
 * Copyright (C) 2012 Avionic Design GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __LINUX_HDMI_H_
#define __LINUX_HDMI_H_

//#include <linux/types.h>

enum hdmi_infoframe_type {
	HDMI_INFOFRAME_TYPE_VENDOR = 0x81,
	HDMI_INFOFRAME_TYPE_AVI = 0x82,
	HDMI_INFOFRAME_TYPE_SPD = 0x83,
	HDMI_INFOFRAME_TYPE_AUDIO = 0x84,
};

#define HDMI_INFOFRAME_HEADER_SIZE  4
#define HDMI_AVI_INFOFRAME_SIZE    13
#define HDMI_SPD_INFOFRAME_SIZE    25
#define HDMI_AUDIO_INFOFRAME_SIZE  10

enum hdmi_colorspace {
	HDMI_COLORSPACE_RGB,
	HDMI_COLORSPACE_YUV422,
	HDMI_COLORSPACE_YUV444,
};

enum hdmi_scan_mode {
	HDMI_SCAN_MODE_NONE,
	HDMI_SCAN_MODE_OVERSCAN,
	HDMI_SCAN_MODE_UNDERSCAN,
};

enum hdmi_colorimetry {
	HDMI_COLORIMETRY_NONE,
	HDMI_COLORIMETRY_ITU_601,
	HDMI_COLORIMETRY_ITU_709,
	HDMI_COLORIMETRY_EXTENDED,
};

enum hdmi_picture_aspect {
	HDMI_PICTURE_ASPECT_NONE,
	HDMI_PICTURE_ASPECT_4_3,
	HDMI_PICTURE_ASPECT_16_9,
};

enum hdmi_active_aspect {
	HDMI_ACTIVE_ASPECT_16_9_TOP = 2,
	HDMI_ACTIVE_ASPECT_14_9_TOP = 3,
	HDMI_ACTIVE_ASPECT_16_9_CENTER = 4,
	HDMI_ACTIVE_ASPECT_PICTURE = 8,
	HDMI_ACTIVE_ASPECT_4_3 = 9,
	HDMI_ACTIVE_ASPECT_16_9 = 10,
	HDMI_ACTIVE_ASPECT_14_9 = 11,
	HDMI_ACTIVE_ASPECT_4_3_SP_14_9 = 13,
	HDMI_ACTIVE_ASPECT_16_9_SP_14_9 = 14,
	HDMI_ACTIVE_ASPECT_16_9_SP_4_3 = 15,
};

enum hdmi_extended_colorimetry {
	HDMI_EXTENDED_COLORIMETRY_XV_YCC_601,
	HDMI_EXTENDED_COLORIMETRY_XV_YCC_709,
	HDMI_EXTENDED_COLORIMETRY_S_YCC_601,
	HDMI_EXTENDED_COLORIMETRY_ADOBE_YCC_601,
	HDMI_EXTENDED_COLORIMETRY_ADOBE_RGB,
};

enum hdmi_quantization_range {
	HDMI_QUANTIZATION_RANGE_DEFAULT,
	HDMI_QUANTIZATION_RANGE_LIMITED,
	HDMI_QUANTIZATION_RANGE_FULL,
};

/* non-uniform picture scaling */
enum hdmi_nups {
	HDMI_NUPS_UNKNOWN,
	HDMI_NUPS_HORIZONTAL,
	HDMI_NUPS_VERTICAL,
	HDMI_NUPS_BOTH,
};

enum hdmi_ycc_quantization_range {
	HDMI_YCC_QUANTIZATION_RANGE_LIMITED,
	HDMI_YCC_QUANTIZATION_RANGE_FULL,
};

enum hdmi_content_type {
	HDMI_CONTENT_TYPE_NONE,
	HDMI_CONTENT_TYPE_PHOTO,
	HDMI_CONTENT_TYPE_CINEMA,
	HDMI_CONTENT_TYPE_GAME,
};

struct hdmi_avi_infoframe {
	enum hdmi_infoframe_type type;
	unsigned char version;
	unsigned char length;
	enum hdmi_colorspace colorspace;
	int active_info_valid;
	int horizontal_bar_valid;
	int vertical_bar_valid;
	enum hdmi_scan_mode scan_mode;
	enum hdmi_colorimetry colorimetry;
	enum hdmi_picture_aspect picture_aspect;
	enum hdmi_active_aspect active_aspect;
	int itc;
	enum hdmi_extended_colorimetry extended_colorimetry;
	enum hdmi_quantization_range quantization_range;
	enum hdmi_nups nups;
	unsigned char video_code;
	enum hdmi_ycc_quantization_range ycc_quantization_range;
	enum hdmi_content_type content_type;
	unsigned char pixel_repeat;
	unsigned short top_bar;
	unsigned short bottom_bar;
	unsigned short left_bar;
	unsigned short right_bar;
};


static void hdmi_infoframe_checksum(void *buffer, size_t size)
{
	uint8_t *ptr = buffer;
	uint8_t csum = 0;
	size_t i;

	/* compute checksum */
	for (i = 0; i < size; i++)
		csum += ptr[i];

	ptr[3] = 256 - csum;
}

/**
 * hdmi_avi_infoframe_init() - initialize an HDMI AVI infoframe
 * @frame: HDMI AVI infoframe
 *
 * Returns 0 on success or a negative error code on failure.
 */
int hdmi_avi_infoframe_init(struct hdmi_avi_infoframe *frame)
{
	memset(frame, 0, sizeof(*frame));

	frame->type = HDMI_INFOFRAME_TYPE_AVI;
	frame->version = 2;
	frame->length = 13;
	return 0;
}

/**
 * hdmi_avi_infoframe_pack() - write HDMI AVI infoframe to binary buffer
 * @frame: HDMI AVI infoframe
 * @buffer: destination buffer
 * @size: size of buffer
 *
 * Packs the information contained in the @frame structure into a binary
 * representation that can be written into the corresponding controller
 * registers. Also computes the checksum as required by section 5.3.5 of
 * the HDMI 1.4 specification.
 *
 * Returns the number of bytes packed into the binary buffer or a negative
 * error code on failure.
 */
ssize_t hdmi_avi_infoframe_pack(struct hdmi_avi_infoframe *frame, void *buffer,
				size_t size)
{
	uint8_t *ptr = buffer;
	size_t length;

	length = HDMI_INFOFRAME_HEADER_SIZE + frame->length;

	if (size < length)
		return -1;

	memset(buffer, 0, length);

	ptr[0] = frame->type;
	ptr[1] = frame->version;
	ptr[2] = frame->length;
	ptr[3] = 0; /* checksum */

	/* start infoframe payload */
	ptr += HDMI_INFOFRAME_HEADER_SIZE;

	ptr[0] = ((frame->colorspace & 0x3) << 5) | (frame->scan_mode & 0x3);

	if (frame->active_info_valid)
		ptr[0] |= B(4);

	if (frame->horizontal_bar_valid)
		ptr[0] |= B(3);

	if (frame->vertical_bar_valid)
		ptr[0] |= B(2);

	ptr[1] = ((frame->colorimetry & 0x3) << 6) |
		 ((frame->picture_aspect & 0x3) << 4) |
		 (frame->active_aspect & 0xf);

	ptr[2] = ((frame->extended_colorimetry & 0x7) << 4) |
		 ((frame->quantization_range & 0x3) << 2) |
		 (frame->nups & 0x3);

	if (frame->itc)
		ptr[2] |= B(7);

	ptr[3] = frame->video_code & 0x7f;

	ptr[4] = ((frame->ycc_quantization_range & 0x3) << 6) |
		 ((frame->content_type & 0x3) << 4) |
		 (frame->pixel_repeat & 0xf);

	ptr[5] = frame->top_bar & 0xff;
	ptr[6] = (frame->top_bar >> 8) & 0xff;
	ptr[7] = frame->bottom_bar & 0xff;
	ptr[8] = (frame->bottom_bar >> 8) & 0xff;
	ptr[9] = frame->left_bar & 0xff;
	ptr[10] = (frame->left_bar >> 8) & 0xff;
	ptr[11] = frame->right_bar & 0xff;
	ptr[12] = (frame->right_bar >> 8) & 0xff;

	hdmi_infoframe_checksum(buffer, length);

	return length;
}

enum hdmi_spd_sdi {
	HDMI_SPD_SDI_UNKNOWN,
	HDMI_SPD_SDI_DSTB,
	HDMI_SPD_SDI_DVDP,
	HDMI_SPD_SDI_DVHS,
	HDMI_SPD_SDI_HDDVR,
	HDMI_SPD_SDI_DVC,
	HDMI_SPD_SDI_DSC,
	HDMI_SPD_SDI_VCD,
	HDMI_SPD_SDI_GAME,
	HDMI_SPD_SDI_PC,
	HDMI_SPD_SDI_BD,
	HDMI_SPD_SDI_SACD,
	HDMI_SPD_SDI_HDDVD,
	HDMI_SPD_SDI_PMP,
};

struct hdmi_spd_infoframe {
	enum hdmi_infoframe_type type;
	unsigned char version;
	unsigned char length;
	char vendor[8];
	char product[16];
	enum hdmi_spd_sdi sdi;
};

int hdmi_spd_infoframe_init(struct hdmi_spd_infoframe *frame,
			    const char *vendor, const char *product);
ssize_t hdmi_spd_infoframe_pack(struct hdmi_spd_infoframe *frame, void *buffer,
				size_t size);

enum hdmi_audio_coding_type {
	HDMI_AUDIO_CODING_TYPE_STREAM,
	HDMI_AUDIO_CODING_TYPE_PCM,
	HDMI_AUDIO_CODING_TYPE_AC3,
	HDMI_AUDIO_CODING_TYPE_MPEG1,
	HDMI_AUDIO_CODING_TYPE_MP3,
	HDMI_AUDIO_CODING_TYPE_MPEG2,
	HDMI_AUDIO_CODING_TYPE_AAC_LC,
	HDMI_AUDIO_CODING_TYPE_DTS,
	HDMI_AUDIO_CODING_TYPE_ATRAC,
	HDMI_AUDIO_CODING_TYPE_DSD,
	HDMI_AUDIO_CODING_TYPE_EAC3,
	HDMI_AUDIO_CODING_TYPE_DTS_HD,
	HDMI_AUDIO_CODING_TYPE_MLP,
	HDMI_AUDIO_CODING_TYPE_DST,
	HDMI_AUDIO_CODING_TYPE_WMA_PRO,
};

enum hdmi_audio_sample_size {
	HDMI_AUDIO_SAMPLE_SIZE_STREAM,
	HDMI_AUDIO_SAMPLE_SIZE_16,
	HDMI_AUDIO_SAMPLE_SIZE_20,
	HDMI_AUDIO_SAMPLE_SIZE_24,
};

enum hdmi_audio_sample_frequency {
	HDMI_AUDIO_SAMPLE_FREQUENCY_STREAM,
	HDMI_AUDIO_SAMPLE_FREQUENCY_32000,
	HDMI_AUDIO_SAMPLE_FREQUENCY_44100,
	HDMI_AUDIO_SAMPLE_FREQUENCY_48000,
	HDMI_AUDIO_SAMPLE_FREQUENCY_88200,
	HDMI_AUDIO_SAMPLE_FREQUENCY_96000,
	HDMI_AUDIO_SAMPLE_FREQUENCY_176400,
	HDMI_AUDIO_SAMPLE_FREQUENCY_192000,
};

enum hdmi_audio_coding_type_ext {
	HDMI_AUDIO_CODING_TYPE_EXT_STREAM,
	HDMI_AUDIO_CODING_TYPE_EXT_HE_AAC,
	HDMI_AUDIO_CODING_TYPE_EXT_HE_AAC_V2,
	HDMI_AUDIO_CODING_TYPE_EXT_MPEG_SURROUND,
};

struct hdmi_audio_infoframe {
	enum hdmi_infoframe_type type;
	unsigned char version;
	unsigned char length;
	unsigned char channels;
	enum hdmi_audio_coding_type coding_type;
	enum hdmi_audio_sample_size sample_size;
	enum hdmi_audio_sample_frequency sample_frequency;
	enum hdmi_audio_coding_type_ext coding_type_ext;
	unsigned char channel_allocation;
	unsigned char level_shift_value;
	int downmix_inhibit;

};

int hdmi_audio_infoframe_init(struct hdmi_audio_infoframe *frame);
ssize_t hdmi_audio_infoframe_pack(struct hdmi_audio_infoframe *frame,
				  void *buffer, size_t size);

struct hdmi_vendor_infoframe {
	enum hdmi_infoframe_type type;
	unsigned char version;
	unsigned char length;
	uint8_t data[27];
};

ssize_t hdmi_vendor_infoframe_pack(struct hdmi_vendor_infoframe *frame,
				   void *buffer, size_t size);

#endif /* _DRM_HDMI_H */

struct work_struct { int a; };

struct sii902x_edid_cfg {
	int cea_underscan;
	int cea_basicaudio;
	int cea_ycbcr444;
	int cea_ycbcr422;
	int hdmi_cap;
	uint8_t video_cap[36];
};

struct sii902x_data {
	struct i2c_client *client;
	struct regmap *regmap;
	struct work_struct work;
	struct sii902x_edid_cfg edid_cfg;
	int reset_pin;
	uint8_t cable_plugin;
	uint8_t edid[SII9022_EDID_LEN];
	int resolution;
	/* For audio */
	unsigned int fmt;
	unsigned int channels;
	uint32_t i2s_fifo_routing[4];
};

//static const struct regmap_config sii9022x_regmap_config = {
//	.reg_bits = 8,
//	.val_bits = 8,
//};

//static ssize_t sii902x_show_state(struct device *dev,
//				  struct device_attribute *attr, char *buf)
//{
//	struct sii902x_data *sii9022x = dev_get_drvdata(dev);
//
//	if (sii9022x->cable_plugin == 0)
//		strcpy(buf, "plugout\n");
//	else
//		strcpy(buf, "plugin\n");
//
//	return strlen(buf);
//}
//static DEVICE_ATTR(cable_state, S_IRUGO, sii902x_show_state, NULL);

//static ssize_t sii902x_show_edid(struct device *dev,
//				 struct device_attribute *attr, char *buf)
//{
//	struct sii902x_data *sii9022x = dev_get_drvdata(dev);
//	int i, j, len = 0;
//
//	for (j = 0; j < SII9022_EDID_LEN / 16; j++) {
//		for (i = 0; i < 16; i++)
//			len += sprintf(buf+len, "0x%02X ",
//				       sii9022x->edid[j * 16 + i]);
//		len += sprintf(buf+len, "\n");
//	}
//
//	return len;
//}
//static DEVICE_ATTR(edid, S_IRUGO, sii902x_show_edid, NULL);

static void sii902x_poweron(struct sii902x_data *sii9022x)
{
	/* Turn on DVI or HDMI */
	if (sii9022x->edid_cfg.hdmi_cap)
		sii9022x_regmap_write(SII9022_SYS_CTRL_DATA_REG, 0x01);
	else
		sii9022x_regmap_write(SII9022_SYS_CTRL_DATA_REG, 0x00);
}

static void sii902x_poweroff(struct sii902x_data *sii9022x)
{
	/* disable tmds before changing resolution */
	if (sii9022x->edid_cfg.hdmi_cap)
		sii9022x_regmap_write(SII9022_SYS_CTRL_DATA_REG, 0x11);
	else
		sii9022x_regmap_write(SII9022_SYS_CTRL_DATA_REG, 0x10);
}

static void sii902x_reset(struct sii902x_data *sii9022x)
{
	BOARD_SII902X_RESET_SET(0);
	local_delay_ms(100);
	BOARD_SII902X_RESET_SET(1);
}

static int sii902x_set_avi_infoframe(struct sii902x_data *sii9022x)
{
	struct i2c_client *client = sii9022x->client;
	struct hdmi_avi_infoframe infoframe;
	uint8_t infoframe_buf[HDMI_INFOFRAME_HEADER_SIZE + HDMI_AVI_INFOFRAME_SIZE];
	int ret;

	hdmi_avi_infoframe_init(&infoframe);

	infoframe.colorspace = HDMI_COLORSPACE_RGB;
	infoframe.active_info_valid  = 1 /* true */;
	infoframe.horizontal_bar_valid = 0 /* false */;
	infoframe.vertical_bar_valid = 0 /* false */;
	infoframe.scan_mode = HDMI_SCAN_MODE_NONE;
	infoframe.colorimetry = HDMI_COLORIMETRY_NONE;
	infoframe.picture_aspect = HDMI_PICTURE_ASPECT_16_9;
	infoframe.active_aspect = HDMI_ACTIVE_ASPECT_PICTURE;
	infoframe.quantization_range = HDMI_QUANTIZATION_RANGE_FULL;

	ret = hdmi_avi_infoframe_pack(&infoframe, infoframe_buf,
				      sizeof(infoframe_buf));
	if (ret < 0) {
		PRINTF("failed to pack avi infoframe\n");
		return ret;
	}

	sii9022x_regmap_bulk_write(SII9022_AVI_INFOFRAME_BASE_REG,
			  &infoframe_buf[3], SII9022_AVI_INFOFRAME_LEN);

	return 0;
}

static void sii902x_setup(struct sii902x_data *sii9022x, const videomode_t * vdmode)
{
	uint16_t data[4];
	uint8_t *tmp;
	int i;

	//dev_dbg(&sii9022x->client->dev, "Sii902x: setup..\n");
	//PRINTF("sii902x_setup:\n");
	/* Power up */
	sii9022x_regmap_write(SII9022_POWER_STATE_CTRL_REG, 0x00);

	/* set TPI video mode */
//	switch (sii9022x->resolution) {

//	case 1080: /* 1080P30 timing */
//		data[0] = 7425;		// pixel clk
//		data[1] = 6000;		// vfreq
//		data[2] = 2200;		// pixels
//		data[3] = 1125;		// lines
//		break;
//	case 900: /* 1440 x 900 timing */
//		data[0] = 8875;
//		data[1] = 6000;
//		data[2] = 1600;
//		data[3] = 926;
//		break;
//	default: /* 720P60 timing */
//		data[0] = 7425;
//		data[1] = 6000;
//		data[2] = 1650;
//		data[3] = 750;
//		break;
//	}

	/* Accumulated parameters for this display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned HSYNC = vdmode->hsync;	/*  */
	const unsigned VSYNC = vdmode->vsync;	/*  */
	const unsigned LEFTMARGIN = HSYNC + vdmode->hbp;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = VSYNC + vdmode->vbp;	/* vertical delay before DE start */
	const unsigned HFULL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VFULL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

	data[0] = display_getdotclock(vdmode) / 1000 / 10;		// pixel clk
	data[1] = vdmode->fps * 100;		// vfreq
	data[2] = HFULL;		// pixels
	data[3] = VFULL;		// lines

	//PRINTF("pixel clk=%u, vfreq=%u, pixels=%u, lines=%u\n", data[0], data[1], data[2], data[3]);

	tmp = (uint8_t *)data;
	for (i = 0; i < 8; i++)
		sii9022x_regmap_write(SII9022_VIDEO_DATA_BASE_REG + i, tmp[i]);

	/* input bus/pixel: full pixel wide (24bit), rising edge */
	sii9022x_regmap_write(SII9022_PIXEL_REPETITION_REG, 0x60);
	/* Set input format to RGB */
	sii9022x_regmap_write(SII9022_AVI_IN_FORMAT_REG, 0x00);
	/* set output format to RGB */
	sii9022x_regmap_write(SII9022_AVI_OUT_FORMAT_REG, 0x10);

	sii902x_set_avi_infoframe(sii9022x);
	//PRINTF("sii902x_setup done.\n");
}

static void sii902x_edid_parse_ext_blk(unsigned char *edid,
				       struct sii902x_edid_cfg *cfg)
{
	unsigned char index = 0x0;
	uint8_t detail_timing_offset, tag_code, data_payload;
	int i;

	if (edid[index++] != 0x2) /* only support cea ext block now */
		return;
	if (edid[index++] != 0x3) /* only support version 3 */
		return;

	detail_timing_offset = edid[index++];

	cfg->cea_underscan = (edid[index] >> 7) & 0x1;
	cfg->cea_basicaudio = (edid[index] >> 6) & 0x1;
	cfg->cea_ycbcr444 = (edid[index] >> 5) & 0x1;
	cfg->cea_ycbcr422 = (edid[index] >> 4) & 0x1;

	/* Parse data block */
	while (++index < detail_timing_offset) {
		tag_code = (edid[index] >> 5) & 0x7;
		data_payload = edid[index] & 0x1f;

		if (tag_code == 0x2) {
			for (i = 0; i < data_payload; i++)
				cfg->video_cap[i] = edid[index + 1 + i];
		}

		/* Find vendor block to check HDMI capable */
		if (tag_code == 0x3) {
			if ((edid[index + 1] == 0x03) &&
			    (edid[index + 2] == 0x0c) &&
			    (edid[index + 3] == 0x00))
				cfg->hdmi_cap = 1 /* true */;
		}

		index += data_payload;
	}
}

static int sii902x_edid_read(struct i2c_adapter *adp, unsigned short addr,
			     unsigned char *edid, struct sii902x_edid_cfg *cfg)
{
	uint8_t buf[2] = {0, 0};
	int dat = 0;
	struct i2c_msg msg[2] = {
		{
			.addr	= addr,		// HDMI_I2C_MONITOR_ADDRESS
			.flags	= 0,
			.len	= 1,
			.buf	= buf,
		}, {
			.addr	= addr,		// HDMI_I2C_MONITOR_ADDRESS
			.flags	= I2C_M_RD,
			.len	= ONE_BLOCK_EDID_LEN,
			.buf	= edid,
		},
	};

	if (adp == NULL)
	{
		TP();
		return -1;
	}

	memset(edid, 0, SII9022_EDID_LEN);
	buf[0] = 0x00;
	dat = i2c_transfer(adp, msg, 2);

	//printhex(0, edid, ONE_BLOCK_EDID_LEN);

	/* need read ext block? Only support one more blk now*/
	if (edid[0x7E]) {
		if (edid[0x7E] > 1) {
			PRINTF("Edid has %d ext block, but now only support 1 ext blk\n",
				 edid[0x7E]);
			return -1;
		}

		/* Add a delay to read extension block */
		local_delay_ms(20);

		buf[0] = ONE_BLOCK_EDID_LEN;
		msg[1].buf = edid + ONE_BLOCK_EDID_LEN;
		dat = i2c_transfer(adp, msg, 2);
		if (dat < 0)
		{
			TP();
			return dat;
		}
		//printhex(ONE_BLOCK_EDID_LEN, edid + ONE_BLOCK_EDID_LEN, ONE_BLOCK_EDID_LEN);

		/* edid ext block parsing */
		sii902x_edid_parse_ext_blk(edid + ONE_BLOCK_EDID_LEN, cfg);
	}

	return 0;
}

//#define SII9022_SYS_CTRL_DDC_BUS_GRANTED	BIT(1)
//#define SII9022_SYS_CTRL_DDC_BUS_REQUEST	BIT(2)

static int sii902x_read_edid(struct sii902x_data *sii9022x)
{
	struct i2c_client *client = sii9022x->client;
	int old, dat, ret, cnt = 100;

	/* Request DDC bus */
	sii9022x_regmap_read(SII9022_SYS_CTRL_DATA_REG, &old);

	sii9022x_regmap_write(SII9022_SYS_CTRL_DATA_REG, old | SII9022_SYS_CTRL_DDC_BUS_REQUEST);
	do {
		cnt--;
		local_delay_ms(20);
		sii9022x_regmap_read(SII9022_SYS_CTRL_DATA_REG, &dat);
	} while ((!(dat & SII9022_SYS_CTRL_DDC_BUS_GRANTED)) && cnt);

	if (!cnt) {
		ret = -1;
		TP();
		goto done;
	}

	sii9022x_regmap_write(SII9022_SYS_CTRL_DATA_REG, old | SII9022_SYS_CTRL_DDC_BUS_GRANTED | SII9022_SYS_CTRL_DDC_BUS_REQUEST);

	/* edid reading */
	ret = sii902x_edid_read(client->adapter, HDMI_I2C_MONITOR_ADDRESS,
				sii9022x->edid, &sii9022x->edid_cfg);
	if (ret) {
		ret = -1;
		TP();
		goto done;
	}

	/* Release DDC bus */
	cnt = 100;
	do {
		cnt--;
		sii9022x_regmap_write(SII9022_SYS_CTRL_DATA_REG,
			     old & ~ (SII9022_SYS_CTRL_DDC_BUS_GRANTED | SII9022_SYS_CTRL_DDC_BUS_REQUEST));
		local_delay_ms(20);
		sii9022x_regmap_read(SII9022_SYS_CTRL_DATA_REG, &dat);
	} while ((dat & (SII9022_SYS_CTRL_DDC_BUS_GRANTED | SII9022_SYS_CTRL_DDC_BUS_REQUEST)) && cnt);

	if (!cnt)
	{
		ret = -1;
		TP();
	}
done:
	sii9022x_regmap_write(SII9022_SYS_CTRL_DATA_REG, old);
	return ret;
}

static void det_worker(struct work_struct *work)
{
//	struct sii902x_data *sii9022x;
//
//	sii9022x = container_of(work, struct sii902x_data, work);
//
//	if (sii902x_read_edid(sii9022x) < 0) {
//		PRINTF(
//			"Sii902x: read edid fail\n");
//	} else {
//		int i;
//
//		for (i = 0; i < sizeof(sii9022x->edid); i++) {
//			if (i % 16 == 0)
//				PRINTF("\n");
//			PRINTF("%02x ", sii9022x->edid[i]);
//		}
//		PRINTF("\n");
//
//		sii902x_setup(sii9022x);
//		sii902x_poweron(sii9022x);
//	}
}

static int sii902x_handle_hpd(struct sii902x_data *sii9022x)
{
	struct i2c_client *client = sii9022x->client;
	int dat, ret;

	ret = sii9022x_regmap_read(SII9022_IRQ_STATUS_REG, &dat);
	if (ret < 0) {
		PRINTF("failed read irq status register\n");
		return -1;
	}

	if (dat & 0x1) {
		/* cable connection changes */
//		if (dat & 0x4) {
//			sii9022x->cable_plugin = 1;
//			schedule_work(&sii9022x->work);
//		} else {
//			sii902x_poweroff(sii9022x);
//			sii9022x->cable_plugin = 0;
//		}
	}

	ret = sii9022x_regmap_write(SII9022_IRQ_STATUS_REG, dat);
	if (ret < 0) {
		PRINTF("failed clean irq status register\n");
		return -1;
	}

	return 0;
}
//
//static irqreturn_t sii902x_detect_handler(int irq, void *data)
//{
//	struct sii902x_data *sii9022x = data;
//	struct i2c_client *client = sii9022x->client;
//	int ret;
//
//	ret = sii902x_handle_hpd(sii9022x);
//	if (ret < 0) {
//		PRINTF("failed deal with irq\n");
//		sii902x_reset(sii9022x);
//	}
//
//	return IRQ_HANDLED;
//}

static int sii902x_detect_version(struct sii902x_data *sii9022x)
{
	struct i2c_client *client = sii9022x->client;
	int product_id, device_id, rev_id, tpi_id, hdcp_rev;
	int dat, ret;

	ret = sii9022x_regmap_write(HDMI_IND_SET_PAGE, 0x01);
	if (ret < 0) {
		PRINTF("can not set page register\n");
		return -1;
	}

	ret = sii9022x_regmap_write(HDMI_IND_OFFSET, 0x03);
	if (ret < 0) {
		PRINTF("can not set offset register\n");
		return -1;
	}

	ret = sii9022x_regmap_read(HDMI_IND_VALUE, &dat);
	if (ret < 0) {
		PRINTF("can not read value register\n");
		return -1;
	}

	product_id = dat << 8;

	ret = sii9022x_regmap_write(HDMI_IND_SET_PAGE, 0x01);
	if (ret < 0) {
		PRINTF("can not set page register\n");
		return -1;
	}

	ret = sii9022x_regmap_write(HDMI_IND_OFFSET, 0x02);
	if (ret < 0) {
		PRINTF("can not set offset register\n");
		return -1;
	}

	ret = sii9022x_regmap_read(HDMI_IND_VALUE, &dat);
	if (ret < 0) {
		PRINTF("can not read value register\n");
		return -1;
	}

	product_id |= dat;
	PRINTF("product id = %x\n", product_id);

	ret = sii9022x_regmap_read(SII9022_DEVICE_ID_REG, &device_id);
	if (ret < 0) {
		PRINTF("can not read device id register\n");
		return -1;
	}

	ret = sii9022x_regmap_read(SII9022_DEVICE_REV_ID_REG, &rev_id);
	if (ret < 0) {
		PRINTF("can not read rev id register\n");
		return -1;
	}

	ret = sii9022x_regmap_read(SII9022_DEVICE_TPI_ID_REG, &tpi_id);
	if (ret < 0) {
		PRINTF("can not read tpi id register\n");
		return -1;
	}

	ret = sii9022x_regmap_read(SII9022_DEVICE_HDCP_REV_REG,
			  &hdcp_rev);
	if (ret < 0) {
		PRINTF("can not read hdcp revision register\n");
		return -1;
	}

	PRINTF("hardware version %02X-%02X-%02X-%02X\n",
		 device_id, rev_id, tpi_id, hdcp_rev);

	return 0;
}

static struct i2c_adapter d0adapter;
static struct i2c_client d0client;
static struct sii902x_data d0;

static int sii902x_probe(struct i2c_client *client)
{
	struct sii902x_data *sii9022x = & d0;
//	struct regmap *regmap;
	int ret;
//
//	sii9022x = devm_kzalloc(&client->dev, sizeof(sii9022x), GFP_KERNEL);
//	if (sii9022x == NULL)
//		return -ENOMEM;
//
//	regmap = devm_regmap_init_i2c(client, &sii9022x_regmap_config);
//	if (IS_ERR(regmap)) {
//		PRINTF("failed to init regmap\n");
//		return PTR_ERR(regmap);
//	}
//
//	sii9022x->regmap = regmap;
	sii9022x->client = client;
//
//	if (client->dev.of_node) {
//		of_property_read_u32(client->dev.of_node, "resolution",
//				     &sii9022x->resolution);
//		sii9022x->reset_pin = of_get_gpio(client->dev.of_node, 0);
//		if (gpio_is_valid(sii9022x->reset_pin)) {
//			ret = gpio_request(sii9022x->reset_pin, "reset");
//			if (ret < 0) {
//				PRINTF(
//					"can not request reset pin\n");
//				return -1;
//			}
//		}
//	}
//
	HARDWARE_SII9022_POWERON(1);	// May be PMIC used here
	/*
	 * The following is the Initialization process
	 * Take reference on SiI9022A PR page 8
	 */

	/* Step 1.1: hardware reset */
	sii902x_reset(sii9022x);

	/* Set termination to default */
	ret = sii9022x_regmap_write(SII9022_TMDS_CONT_REG, 0x25);
	if (ret < 0) {
		PRINTF("failed set termination to default\n");
		return -1;
	}

	/* Set hardware debounce to 64 ms */
	ret = sii9022x_regmap_write(SII9022_HPD_DELAY_DEBOUNCE, 0x14);
	if (ret < 0) {
		PRINTF("failed set hw debounce to 64 ms\n");
		return -1;
	}

	/* Step 1.2: enable TPI mode */
	ret = sii9022x_regmap_write(SII9022_TPI_RQB_REG, 0x00);
	if (ret < 0) {
		PRINTF("can not enable TPI mode\n");
		return -1;
	}

	/* Step 2: detect product id and version */
	ret = sii902x_detect_version(sii9022x);
	if (ret < 0) {
		PRINTF("detect sii902x failed\n");
		return -1;
	}
//
//	INIT_WORK(&(sii9022x->work), det_worker);
//
//	if (client->irq) {
//		ret = devm_request_threaded_irq(&client->dev, client->irq,
//						NULL, sii902x_detect_handler,
//						IRQF_TRIGGER_LOW | IRQF_ONESHOT,
//						"SiI902x_det", sii9022x);
//		if (ret) {
//			PRINTF("failed to request det irq\n");
//		} else {
//			/* Enable cable hot plug irq */
//			sii9022x_regmap_write(SII9022_IRQ_ENABLE_REG,
//				     0x01);
//		}
//
//		ret = device_create_file(&client->dev, &dev_attr_cable_state);
//		if (ret < 0)
//			dev_warn(&client->dev,
//				 "cound not create sys node for cable state\n");
//		ret = device_create_file(&client->dev, &dev_attr_edid);
//		if (ret < 0)
//			dev_warn(&client->dev,
//				 "cound not create sys node for edid\n");
//	}
//
//	i2c_set_clientdata(client, sii9022x);
//
//#ifdef CONFIG_SND_ATMEL_SOC_SII9022
//	sii9022x_hdmi_codec_register(&client->dev);
//#endif
//
	return 0;
}

//static int sii902x_remove(struct i2c_client *client)
//{
//	struct sii902x_data *sii9022x = i2c_get_clientdata(client);
//
//	sii902x_poweroff(sii9022x);
//
//	return 0;
//}
//
//static int sii902x_suspend(struct device *dev)
//{
//	/*TODO*/
//	return 0;
//}
//
//static int sii902x_resume(struct device *dev)
//{
//	/*TODO*/
//	return 0;
//}
//
//SIMPLE_DEV_PM_OPS(sii902x_pm_ops, sii902x_suspend, sii902x_resume);
//
//static const struct i2c_device_id sii902x_id[] = {
//	{ "sii902x", 0 },
//	{},
//};
//MODULE_DEVICE_TABLE(i2c, sii902x_id);
//
//static struct i2c_driver sii902x_i2c_driver = {
//	.driver = {
//		.name = "sii902x",
//		.pm = &sii902x_pm_ops,
//		},
//	.probe = sii902x_probe,
//	.remove = sii902x_remove,
//	.id_table = sii902x_id,
//};
//module_i2c_driver(sii902x_i2c_driver);
//
//MODULE_AUTHOR("Bo Shen <voice.shen@atmel.com>");
//MODULE_DESCRIPTION("SII902x DVI/HDMI driver");
//MODULE_LICENSE("GPL");

#endif
//	sii902x 0-0039: product id = 9022
//	sii902x 0-0039: hardware version B0-02-03-00

void sii9022x_initialize(const videomode_t * vdmode)
{
	struct sii902x_data *sii9022x = & d0;
	//sii902x_reset(NULL);

	d0client.adapter = & d0adapter;
	VERIFY(sii902x_probe(& d0client) == 0);

	if (sii902x_read_edid(sii9022x) < 0) {
		PRINTF(
			"Sii902x: read edid fail\n");
		sii902x_setup(sii9022x, vdmode);
		sii902x_poweron(sii9022x);
	} else {
		PRINTF(
			"Sii902x: read edid okay\n");
		sii902x_setup(sii9022x, vdmode);
		sii902x_poweron(sii9022x);
	}
}

void sii9022x_deinitialize(void)
{

}

void sii9022x_wakeup(const videomode_t * vdmode)
{

}
#endif /* LCDMODEX_SII9022A */
