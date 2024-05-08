/*
 * tc358768.c
 *
 *  Created on: Apr 30, 2021
 *      Author: gena
 */


/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Доработки для LS020 Василий Линывый, livas60@mail.ru
//

#include "hardware.h"



#if LCDMODETX_TC358778XBG


#include "display.h"
#include <stdint.h>
#include <string.h>
#include <limits.h>

#include "formats.h"	// for debug prints
#include "gpio.h"
#include "src/touch/touch.h"


#include "mipi_dsi.h"

// See:
//	http://www.staroceans.org/projects/beagleboard/drivers/gpu/drm/omapdrm/displays/encoder-tc358768.c

/* Global (16-bit addressable) */
#define TC358768_CHIPID			0x0000
#define TC358768_SYSCTL			0x0002
#define TC358768_CONFCTL		0x0004	// Input Control Register
#define TC358768_VSDLY			0x0006
#define TC358768_DATAFMT		0x0008
#define TC358768_GPIOEN			0x000E
#define TC358768_GPIODIR		0x0010
#define TC358768_GPIOIN			0x0012
#define TC358768_GPIOOUT		0x0014
#define TC358768_PLLCTL0		0x0016
#define TC358768_PLLCTL1		0x0018
#define TC358768_CMDBYTE		0x0022
#define TC358768_PP_MISC		0x0032
#define TC358768_DSITX_DT		0x0050
#define TC358768_FIFOSTATUS		0x00F8

/* Debug (16-bit addressable) */
#define TC358768_VBUFCTRL		0x00E0
#define TC358768_DBG_WIDTH		0x00E2
#define TC358768_DBG_VBLANK		0x00E4
#define TC358768_DBG_DATA		0x00E8

/* TX PHY (32-bit addressable) */
#define TC358768_CLW_DPHYCONTTX		0x0100
#define TC358768_D0W_DPHYCONTTX		0x0104
#define TC358768_D1W_DPHYCONTTX		0x0108
#define TC358768_D2W_DPHYCONTTX		0x010C
#define TC358768_D3W_DPHYCONTTX		0x0110
#define TC358768_CLW_CNTRL		0x0140
#define TC358768_D0W_CNTRL		0x0144
#define TC358768_D1W_CNTRL		0x0148
#define TC358768_D2W_CNTRL		0x014C
#define TC358768_D3W_CNTRL		0x0150

/* TX PPI (32-bit addressable) */
#define TC358768_STARTCNTRL		0x0204
#define TC358768_DSITXSTATUS		0x0208
#define TC358768_LINEINITCNT		0x0210
#define TC358768_LPTXTIMECNT		0x0214
#define TC358768_TCLK_HEADERCNT		0x0218
#define TC358768_TCLK_TRAILCNT		0x021C
#define TC358768_THS_HEADERCNT		0x0220
#define TC358768_TWAKEUP		0x0224
#define TC358768_TCLK_POSTCNT		0x0228
#define TC358768_THS_TRAILCNT		0x022C
#define TC358768_HSTXVREGCNT		0x0230
#define TC358768_HSTXVREGEN		0x0234
#define TC358768_TXOPTIONCNTRL		0x0238
#define TC358768_BTACNTRL1		0x023C

/* TX CTRL (32-bit addressable) */
#define TC358768_DSI_STATUS		0x0410
#define TC358768_DSI_INT		0x0414
#define TC358768_DSICMD_RXFIFO		0x0430
#define TC358768_DSI_ACKERR		0x0434
#define TC358768_DSI_RXERR		0x0440
#define TC358768_DSI_ERR		0x044C
#define TC358768_DSI_CONFW		0x0500
#define TC358768_DSI_RESET		0x0504
#define TC358768_DSI_INT_CLR		0x050C
#define TC358768_DSI_START		0x0518

/* DSITX CTRL (16-bit addressable) */
#define TC358768_DSICMD_TX		0x0600
#define TC358768_DSICMD_TYPE		0x0602
#define TC358768_DSICMD_WC		0x0604
#define TC358768_DSICMD_WD0		0x0610
#define TC358768_DSICMD_WD1		0x0612
#define TC358768_DSICMD_WD2		0x0614
#define TC358768_DSICMD_WD3		0x0616
#define TC358768_DSI_EVENT		0x0620
#define TC358768_DSI_VSW		0x0622
#define TC358768_DSI_VBPR		0x0624
#define TC358768_DSI_VACT		0x0626
#define TC358768_DSI_HSW		0x0628
#define TC358768_DSI_HBPR		0x062A
#define TC358768_DSI_HACT		0x062C


/* MIPI DSI Processor-to-Peripheral transaction types */
enum {
	MIPI_DSI_V_SYNC_START				= 0x01,
	MIPI_DSI_V_SYNC_END				= 0x11,
	MIPI_DSI_H_SYNC_START				= 0x21,
	MIPI_DSI_H_SYNC_END				= 0x31,

	MIPI_DSI_COLOR_MODE_OFF				= 0x02,
	MIPI_DSI_COLOR_MODE_ON				= 0x12,
	MIPI_DSI_SHUTDOWN_PERIPHERAL			= 0x22,
	MIPI_DSI_TURN_ON_PERIPHERAL			= 0x32,

	MIPI_DSI_GENERIC_SHORT_WRITE_0_PARAM		= 0x03,
	MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM		= 0x13,
	MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM		= 0x23,

	MIPI_DSI_GENERIC_READ_REQUEST_0_PARAM		= 0x04,
	MIPI_DSI_GENERIC_READ_REQUEST_1_PARAM		= 0x14,
	MIPI_DSI_GENERIC_READ_REQUEST_2_PARAM		= 0x24,

	MIPI_DSI_DCS_SHORT_WRITE			= 0x05,
	MIPI_DSI_DCS_SHORT_WRITE_PARAM			= 0x15,

	MIPI_DSI_DCS_READ				= 0x06,

	MIPI_DSI_DCS_COMPRESSION_MODE                   = 0x07,
	MIPI_DSI_PPS_LONG_WRITE                         = 0x0A,

	MIPI_DSI_SET_MAXIMUM_RETURN_PACKET_SIZE		= 0x37,

	MIPI_DSI_END_OF_TRANSMISSION			= 0x08,

	MIPI_DSI_NULL_PACKET				= 0x09,
	MIPI_DSI_BLANKING_PACKET			= 0x19,
	MIPI_DSI_GENERIC_LONG_WRITE			= 0x29,
	MIPI_DSI_DCS_LONG_WRITE				= 0x39,

	MIPI_DSI_LOOSELY_PACKED_PIXEL_STREAM_YCBCR20	= 0x0c,
	MIPI_DSI_PACKED_PIXEL_STREAM_YCBCR24		= 0x1c,
	MIPI_DSI_PACKED_PIXEL_STREAM_YCBCR16		= 0x2c,

	MIPI_DSI_PACKED_PIXEL_STREAM_30			= 0x0d,
	MIPI_DSI_PACKED_PIXEL_STREAM_36			= 0x1d,
	MIPI_DSI_PACKED_PIXEL_STREAM_YCBCR12		= 0x3d,

	MIPI_DSI_PACKED_PIXEL_STREAM_16			= 0x0e,
	MIPI_DSI_PACKED_PIXEL_STREAM_18			= 0x1e,
	MIPI_DSI_PIXEL_STREAM_3BYTE_18			= 0x2e,
	MIPI_DSI_PACKED_PIXEL_STREAM_24			= 0x3e,
};

/* MIPI DSI Peripheral-to-Processor transaction types */
enum {
	MIPI_DSI_RX_ACKNOWLEDGE_AND_ERROR_REPORT	= 0x02,
	MIPI_DSI_RX_END_OF_TRANSMISSION			= 0x08,
	MIPI_DSI_RX_GENERIC_SHORT_READ_RESPONSE_1BYTE	= 0x11,
	MIPI_DSI_RX_GENERIC_SHORT_READ_RESPONSE_2BYTE	= 0x12,
	MIPI_DSI_RX_GENERIC_LONG_READ_RESPONSE		= 0x1a,
	MIPI_DSI_RX_DCS_LONG_READ_RESPONSE		= 0x1c,
	MIPI_DSI_RX_DCS_SHORT_READ_RESPONSE_1BYTE	= 0x21,
	MIPI_DSI_RX_DCS_SHORT_READ_RESPONSE_2BYTE	= 0x22,
};

/* MIPI DCS commands */
enum {
	MIPI_DCS_NOP			= 0x00,
	MIPI_DCS_SOFT_RESET		= 0x01,
	MIPI_DCS_GET_DISPLAY_ID		= 0x04,
	MIPI_DCS_GET_RED_CHANNEL	= 0x06,
	MIPI_DCS_GET_GREEN_CHANNEL	= 0x07,
	MIPI_DCS_GET_BLUE_CHANNEL	= 0x08,
	MIPI_DCS_GET_DISPLAY_STATUS	= 0x09,
	MIPI_DCS_GET_POWER_MODE		= 0x0A,
	MIPI_DCS_GET_ADDRESS_MODE	= 0x0B,
	MIPI_DCS_GET_PIXEL_FORMAT	= 0x0C,
	MIPI_DCS_GET_DISPLAY_MODE	= 0x0D,
	MIPI_DCS_GET_SIGNAL_MODE	= 0x0E,
	MIPI_DCS_GET_DIAGNOSTIC_RESULT	= 0x0F,
	MIPI_DCS_ENTER_SLEEP_MODE	= 0x10,
	MIPI_DCS_EXIT_SLEEP_MODE	= 0x11,
	MIPI_DCS_ENTER_PARTIAL_MODE	= 0x12,
	MIPI_DCS_ENTER_NORMAL_MODE	= 0x13,
	MIPI_DCS_EXIT_INVERT_MODE	= 0x20,
	MIPI_DCS_ENTER_INVERT_MODE	= 0x21,
	MIPI_DCS_SET_GAMMA_CURVE	= 0x26,
	MIPI_DCS_SET_DISPLAY_OFF	= 0x28,
	MIPI_DCS_SET_DISPLAY_ON		= 0x29,
	MIPI_DCS_SET_COLUMN_ADDRESS	= 0x2A,
	MIPI_DCS_SET_PAGE_ADDRESS	= 0x2B,
	MIPI_DCS_WRITE_MEMORY_START	= 0x2C,
	MIPI_DCS_WRITE_LUT		= 0x2D,
	MIPI_DCS_READ_MEMORY_START	= 0x2E,
	MIPI_DCS_SET_PARTIAL_AREA	= 0x30,
	MIPI_DCS_SET_SCROLL_AREA	= 0x33,
	MIPI_DCS_SET_TEAR_OFF		= 0x34,
	MIPI_DCS_SET_TEAR_ON		= 0x35,
	MIPI_DCS_SET_ADDRESS_MODE	= 0x36,
	MIPI_DCS_SET_SCROLL_START	= 0x37,
	MIPI_DCS_EXIT_IDLE_MODE		= 0x38,
	MIPI_DCS_ENTER_IDLE_MODE	= 0x39,
	MIPI_DCS_SET_PIXEL_FORMAT	= 0x3A,
	MIPI_DCS_WRITE_MEMORY_CONTINUE	= 0x3C,
	MIPI_DCS_READ_MEMORY_CONTINUE	= 0x3E,
	MIPI_DCS_SET_TEAR_SCANLINE	= 0x44,
	MIPI_DCS_GET_SCANLINE		= 0x45,
	MIPI_DCS_SET_DISPLAY_BRIGHTNESS = 0x51,		/* MIPI DCS 1.3 */
	MIPI_DCS_GET_DISPLAY_BRIGHTNESS = 0x52,		/* MIPI DCS 1.3 */
	MIPI_DCS_WRITE_CONTROL_DISPLAY  = 0x53,		/* MIPI DCS 1.3 */
	MIPI_DCS_GET_CONTROL_DISPLAY	= 0x54,		/* MIPI DCS 1.3 */
	MIPI_DCS_WRITE_POWER_SAVE	= 0x55,		/* MIPI DCS 1.3 */
	MIPI_DCS_GET_POWER_SAVE		= 0x56,		/* MIPI DCS 1.3 */
	MIPI_DCS_SET_CABC_MIN_BRIGHTNESS = 0x5E,	/* MIPI DCS 1.3 */
	MIPI_DCS_GET_CABC_MIN_BRIGHTNESS = 0x5F,	/* MIPI DCS 1.3 */
	MIPI_DCS_READ_DDB_START		= 0xA1,
	MIPI_DCS_READ_DDB_CONTINUE	= 0xA8,
};

/* MIPI DCS pixel formats */
#define MIPI_DCS_PIXEL_FMT_24BIT	7
#define MIPI_DCS_PIXEL_FMT_18BIT	6
#define MIPI_DCS_PIXEL_FMT_16BIT	5
#define MIPI_DCS_PIXEL_FMT_12BIT	3
#define MIPI_DCS_PIXEL_FMT_8BIT		2
#define MIPI_DCS_PIXEL_FMT_3BIT		1



struct tc358768_drv_data
{
	int dev;
	unsigned fbd, prd, frs;
	unsigned bitclk;

	unsigned dsi_lanes;	// 4
	unsigned pd_lines;	// 24
	unsigned long refclk;
};

#define TC358768_I2C_ADDR (0x0E * 2)

unsigned
tc358768_rd_reg_16or32bits(unsigned register_id)
{
	const unsigned i2caddr = TC358768_I2C_ADDR;

	const int v16bit = (register_id < 0x100 || register_id >= 0x600);

	if (v16bit)
	{
#if WITHTWIHW
		const uint8_t bufw [2] = { register_id >> 8, register_id >> 0, };
		uint8_t bufr [2];

		if (i2chw_write(i2caddr | 0x00, bufw, ARRAY_SIZE(bufw)))
			return 1;
		if (i2chw_read(i2caddr | 0x01, bufr, ARRAY_SIZE(bufr)))
			return 1;

		return
				(((unsigned long) bufr [0]) << 8) |
				(((unsigned long) bufr [1]) << 0) |
				0;

#elif WITHTWISW
		uint8_t v1, v2;

		i2c_start(i2caddr | 0x00);
		i2c_write(register_id >> 8);
		i2c_write_withrestart(register_id >> 0);
		i2c_start(i2caddr | 0x01);
		i2c_read(& v1, I2C_READ_ACK_1);	// ||
		i2c_read(& v2, I2C_READ_NACK);	// ||

		return
				(((unsigned long) v1) << 8) |
				(((unsigned long) v2) << 0) |
				0;
#endif
	}
	else
	{
#if WITHTWIHW
		const uint8_t bufw [2] = { register_id >> 8, register_id >> 0, };
		uint8_t bufr [4];

		if (i2chw_write(i2caddr | 0x00, bufw, ARRAY_SIZE(bufw)))
			return 1;
		if (i2chw_read(i2caddr | 0x01, bufr, ARRAY_SIZE(bufr)))
			return 1;

		return
				(((unsigned long) bufr [0]) << 8) |
				(((unsigned long) bufr [1]) << 0) |
				(((unsigned long) bufr [2]) << 24) |
				(((unsigned long) bufr [3]) << 16) |
				0;

#elif WITHTWISW
		uint8_t v1, v2, v3, v4;

		i2c_start(i2caddr | 0x00);
		i2c_write(register_id >> 8);
		i2c_write_withrestart(register_id >> 0);
		i2c_start(i2caddr | 0x01);
		i2c_read(& v1, I2C_READ_ACK_1);	// ||
		i2c_read(& v2, I2C_READ_ACK);	// ||
		i2c_read(& v3, I2C_READ_ACK);	// ||
		i2c_read(& v4, I2C_READ_NACK);	// ||

		return
				(((unsigned long) v1) << 8) |
				(((unsigned long) v2) << 0) |
				(((unsigned long) v3) << 24) |
				(((unsigned long) v4) << 16) |
				0;
#endif
	}
}

int
tc358768_wr_reg_16bits(unsigned long value)
{
	const unsigned i2caddr = TC358768_I2C_ADDR;
	const unsigned register_id = value >> 16;
	const int v16bit = (register_id < 0x100 || register_id >= 0x600);

#if WITHTWIHW
	const uint8_t bufw [] =
	{
		register_id >> 8, register_id >> 0,
		value >> 8, value >> 0,
	};

	return i2chw_write(i2caddr | 0x00, bufw, ARRAY_SIZE(bufw));

#elif WITHTWISW
	i2c_start(i2caddr | 0x00);
	i2c_write(register_id >> 8);		// addres hi
	i2c_write(register_id >> 0);		// addres lo
	i2c_write(value >> 8);		// data hi
	i2c_write(value >> 0);		// data lo
	if (v16bit == 0)
	{
		i2c_write(0x00);
		i2c_write(0x00);
	}
	i2c_waitsend();
    i2c_stop();

    return 0;
#endif
}

static void dumpunereg(unsigned reg, unsigned data)
{
	if (reg < 0x100 || reg >= 0x600)
		PRINTF("tc358768_write(xx, 0x%04X, 0x%04X); /* addr=0x%04X, data=0x%04x */ \n", reg, data, reg, data);
	else
		PRINTF("tc358768_write(xx, 0x%04X, 0x%08X); /* addr=0x%04X, data=0x%08x */ \n", reg, data, reg, data);
}

static int tc358768_write(
	struct tc358768_drv_data *ddata,
	unsigned int reg,
	unsigned long val
	)
{
	//dumpunereg(reg, val);
	const unsigned i2caddr = TC358768_I2C_ADDR;

	if (reg < 0x100 || reg >= 0x600)
	{
		// 16-bit register
#if WITHTWIHW
		const uint8_t bufw [] =
		{
			reg >> 8, reg >> 0,
			val >> 8, val >> 0,
		};

		return i2chw_write(i2caddr | 0x00, bufw, ARRAY_SIZE(bufw));

#elif WITHTWISW
		i2c_start(i2caddr | 0x00);
		i2c_write(reg >> 8);		// addres hi
		i2c_write(reg >> 0);		// addres lo
		i2c_write(val >> 8);		// data 15..8
		i2c_write(val >> 0);		// data 7..0
		i2c_waitsend();
	    i2c_stop();
#endif
	}
	else
	{
		// 32-bit register
#if WITHTWIHW
		const uint8_t bufw [] =
		{
			reg >> 8, reg >> 0,
			val >> 8, val >> 0,
			val >> 24, val >> 16,
		};

		return i2chw_write(i2caddr | 0x00, bufw, ARRAY_SIZE(bufw));

#elif WITHTWISW
		i2c_start(i2caddr | 0x00);
		i2c_write(reg >> 8);		// addres hi
		i2c_write(reg >> 0);		// addres lo
		i2c_write(val >> 8);		// data 15..8
		i2c_write(val >> 0);		// data 7..0
		i2c_write(val >> 24);		// data 31..24
		i2c_write(val >> 16);		// data 23..16
		i2c_waitsend();
	    i2c_stop();
#endif
	}
	return 0;
}

static int tc358768_read(
	struct tc358768_drv_data *ddata,
	unsigned int register_id,
	unsigned long * val
	)
{
	const unsigned i2caddr = TC358768_I2C_ADDR;

	const int v16bit = (register_id < 0x100 || register_id >= 0x600);
	if (v16bit)
	{
		// 16-bit register
#if WITHTWIHW
		const uint8_t bufw [2] = { register_id >> 8, register_id >> 0, };
		uint8_t bufr [2];

		if (i2chw_write(i2caddr | 0x00, bufw, ARRAY_SIZE(bufw)))
			return 1;
		if (i2chw_read(i2caddr | 0x01, bufr, ARRAY_SIZE(bufr)))
			return 1;

		* val =
				(((unsigned long) bufr [0]) << 8) |
				(((unsigned long) bufr [1]) << 0) |
				0;

#elif WITHTWISW
		uint8_t v1, v2;

		i2c_start(i2caddr | 0x00);
		i2c_write(register_id >> 8);
		i2c_write_withrestart(register_id >> 0);
		i2c_start(i2caddr | 0x01);
		i2c_read(& v1, I2C_READ_ACK_1);	// ||
		i2c_read(& v2, I2C_READ_NACK);	// ||

		* val =
				(((unsigned long) v1) << 8) |
				(((unsigned long) v2) << 0) |
				0;
#endif
	}
	else
	{
		// 32-bit register
#if WITHTWIHW
		const uint8_t bufw [2] = { register_id >> 8, register_id >> 0, };
		uint8_t bufr [4];

		if (i2chw_write(i2caddr | 0x00, bufw, ARRAY_SIZE(bufw)))
			return 1;
		if (i2chw_read(i2caddr | 0x01, bufr, ARRAY_SIZE(bufr)))
			return 1;

		* val =
				(((unsigned long) bufr [0]) << 8) |
				(((unsigned long) bufr [1]) << 0) |
				(((unsigned long) bufr [2]) << 24) |
				(((unsigned long) bufr [3]) << 16) |
				0;

#elif WITHTWISW
		uint8_t v1, v2, v3, v4;

		i2c_start(i2caddr | 0x00);
		i2c_write(register_id >> 8);
		i2c_write_withrestart(register_id >> 0);
		i2c_start(i2caddr | 0x01);
		i2c_read(& v1, I2C_READ_ACK_1);	// ||
		i2c_read(& v2, I2C_READ_ACK);	// ||
		i2c_read(& v3, I2C_READ_ACK);	// ||
		i2c_read(& v4, I2C_READ_NACK);	// ||

		* val =
				(((unsigned long) v1) << 8) |
				(((unsigned long) v2) << 0) |
				(((unsigned long) v3) << 24) |
				(((unsigned long) v4) << 16) |
				0;
#endif
	}

	return 0;
}

static int tc358768_update_bits(struct tc358768_drv_data *ddata,
	unsigned int reg, unsigned int mask, unsigned int val)
{
	int ret;
	unsigned long tmp, orig;

	ret = tc358768_read(ddata, reg, &orig);
	if (ret != 0)
		return ret;

	tmp = orig & ~mask;
	tmp |= val & mask;

	////dev_dbg(ddata->dev, "UPD \t%04x\t%08x -> %08x\n", reg, orig, tmp);

	if (tmp != orig)
		ret = tc358768_write(ddata, reg, tmp);

	return ret;
}


static int tc358768_dsi_xfer_short(struct tc358768_drv_data *ddata,
	uint8_t data_id, uint8_t data0, uint8_t data1)
{
	const uint8_t packet_type = 0x10; /* DSI Short Packet */
	const uint8_t word_count = 0;

	tc358768_write(ddata, TC358768_DSICMD_TYPE,
		(packet_type << 8) | data_id);
	tc358768_write(ddata, TC358768_DSICMD_WC, (word_count & 0xf));
	tc358768_write(ddata, TC358768_DSICMD_WD0, (data1 << 8) | data0);
	tc358768_write(ddata, TC358768_DSICMD_TX, 1); /* start transfer */

	return 0;
}

static void tc358768_sw_reset(struct tc358768_drv_data *ddata)
{
	/* Assert Reset */
	tc358768_write(ddata, TC358768_SYSCTL, 1);
	/* Release Reset, Exit Sleep */
	tc358768_write(ddata, TC358768_SYSCTL, 0);
}
//
//#define REFCLK 25000000uL
//#define DSI_NDL 4
//#define DPI_NDL 24


struct tc358768_drv_data dev0 =
{
		.refclk = 25000000uL,
		.pd_lines = 16, //24,
		.dsi_lanes = 4
};

static uint32_t local_min(uint32_t a, uint32_t b) { return a < b ? a : b; }
static uint32_t local_max(uint32_t a, uint32_t b) { return a > b ? a : b; }
static uint64_t div_u64(uint64_t a, uint64_t b) { return a / b; }

//
//static uint32_t tc358768_pll_to_pclk(struct tc358768_drv_data *priv, uint32_t pll_clk)
//{
//	return (uint32_t)div_u64((uint64_t)pll_clk * priv->dsi_lanes, priv->pd_lines) / 2;
//}

static uint32_t tc358768_pclk_to_pll(struct tc358768_drv_data *priv, uint32_t pclk)
{
	return (uint32_t)div_u64((uint64_t)pclk * priv->pd_lines, priv->dsi_lanes) * 2;
}

/*

struct omap_video_timings
{
	unsigned vsw;
	unsigned hsw;
	unsigned vbp;
	unsigned hbp;
	unsigned y_res;
	unsigned x_res;
	unsigned long pixelclock;
};

struct omap_video_timings timings0 = {
		.vsw = VSYNC,
		.hsw = HSYNC,
		.vbp = VBP,
		.hbp = HBP,
		.y_res = HEIGHT,
		.x_res = WIDTH,
		.pixelclock = LTDC_DOTCLK
};
*/

static unsigned long clk_get_rate(unsigned long v) { return v; }

static int tc358768_calc_pll(struct tc358768_drv_data *ddata, const videomode_t * vdmode)
{
	/* Accumulated parameters for this display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned HSYNC = vdmode->hsync;	/*  */
	const unsigned VSYNC = vdmode->vsync;	/*  */
	const unsigned LEFTMARGIN = HSYNC + vdmode->hbp;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = VSYNC + vdmode->vbp;	/* vertical delay before DE start */
	const unsigned HFULL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VFULL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

//	const struct omap_video_timings *t = & timings0;
	static const unsigned long frs_limits[] = {
		1000000000, 500000000, 250000000, 125000000, 62500000
	};
	unsigned fbd, prd, frs;
	uint32_t target_pll;
	unsigned long refclk;
	unsigned i;
	uint32_t max_pll, min_pll;

	uint32_t best_diff, best_pll, best_prd, best_fbd;

	target_pll = tc358768_pclk_to_pll(ddata, hardware_get_dotclock(display_getdotclock(vdmode)));

	/* pll_clk = RefClk * [(FBD + 1)/ (PRD + 1)] * [1 / (2^FRS)] */

	frs = UINT_MAX;

	for (i = 0; i < ARRAY_SIZE(frs_limits) - 1; ++i) {
		if (target_pll < frs_limits[i] && target_pll >= frs_limits[i + 1]) {
			frs = i;
			max_pll = frs_limits[i];
			min_pll = frs_limits[i + 1];
			break;
		}
	}

	if (frs == UINT_MAX)
		return -(1);

	refclk = clk_get_rate(ddata->refclk);

	best_pll = best_prd = best_fbd = 0;
	best_diff = UINT_MAX;

	for (prd = 0; prd < 16; ++prd) {
		uint32_t divisor = (prd + 1) * (1 << frs);

		for (fbd = 0; fbd < 512; ++fbd) {
			uint32_t pll, diff;

			pll = (uint32_t)div_u64((uint64_t)refclk * (fbd + 1), divisor);

			if (pll >= max_pll || pll < min_pll)
				continue;

			diff = local_max(pll, target_pll) - local_min(pll, target_pll);

			if (diff < best_diff) {
				best_diff = diff;
				best_pll = pll;
				best_prd = prd;
				best_fbd = fbd;
			}

			if (best_diff == 0)
				break;
		}

		if (best_diff == 0)
			break;
	}

	if (best_diff == UINT_MAX) {
		PRINTF("could not find suitable PLL setup\n");
		return -(1);
	}

//	best_prd = 17; //9;
//	frs = 3;
//	best_fbd = 200;
//	uint32_t divisor = (best_prd + 1) * (1 << frs);
//	best_pll = (uint32_t)div_u64((uint64_t)refclk * (best_fbd + 1), divisor);
//
	ddata->fbd = best_fbd;
	ddata->prd = best_prd;
	ddata->frs = frs;
	ddata->bitclk = best_pll / 2;

	return 0;
}

static void tc358768_setup_pll(struct tc358768_drv_data *ddata)
{
	unsigned fbd, prd, frs;

	fbd = ddata->fbd;	// Feedback divider setting
	prd = ddata->prd;	// Input divider setting
	frs = ddata->frs;	// Frequency range setting (post divider)

	PRINTF("PLL: refclk %lu, fbd %u, prd %u, frs %u\n",
			ddata->refclk, fbd, prd, frs);

//	PRINTF("PLL: %u, BitClk %u, ByteClk %u, pclk %u\n",
//		ddata->bitclk * 2, ddata->bitclk, ddata->bitclk / 4,
//		tc358768_pll_to_pclk(ddata, ddata->bitclk * 2));

	/* PRD[15:12] FBD[8:0] */
	tc358768_write(ddata, TC358768_PLLCTL0, (prd << 12) | fbd);

	/* FRS[11:10] LBWS[9:8] CKEN[4] RESETB[1] EN[0] */
	tc358768_write(ddata, TC358768_PLLCTL1,
		(frs << 10) | (0x2 << 8) | (0 << 4) | (1 << 1) | (1 << 0));

	/* wait for lock */
	local_delay_ms(5);

	/* FRS[11:10] LBWS[9:8] CKEN[4] RESETB[1] EN[0] */
	tc358768_write(ddata, TC358768_PLLCTL1,
		(frs << 10) | (0x2 << 8) | (1 << 4) | (1 << 1) | (1 << 0));
}


static int
isnamed(unsigned addr)
{
	switch (addr)
	{
	case TC358768_CHIPID			: // 0x0000
	case TC358768_SYSCTL			: // 0x0002
	case TC358768_CONFCTL		: // 0x0004	// Input Control Register
	case TC358768_VSDLY			: // 0x0006
	case TC358768_DATAFMT		: // 0x0008
	case TC358768_GPIOEN			: // 0x000E
	case TC358768_GPIODIR		: // 0x0010
	case TC358768_GPIOIN			: // 0x0012
	case TC358768_GPIOOUT		: // 0x0014
	case TC358768_PLLCTL0		: // 0x0016
	case TC358768_PLLCTL1		: // 0x0018
	case TC358768_CMDBYTE		: // 0x0022
	case TC358768_PP_MISC		: // 0x0032
	case TC358768_DSITX_DT		: // 0x0050
	case TC358768_FIFOSTATUS		: // 0x00F8

	// Debug (16-bit addressable) */
	case TC358768_VBUFCTRL		: // 0x00E0
	case TC358768_DBG_WIDTH		: // 0x00E2
	case TC358768_DBG_VBLANK		: // 0x00E4
	case TC358768_DBG_DATA		: // 0x00E8

	// TX PHY (32-bit addressable) */
	case TC358768_CLW_DPHYCONTTX		: // 0x0100
	case TC358768_D0W_DPHYCONTTX		: // 0x0104
	case TC358768_D1W_DPHYCONTTX		: // 0x0108
	case TC358768_D2W_DPHYCONTTX		: // 0x010C
	case TC358768_D3W_DPHYCONTTX		: // 0x0110
	case TC358768_CLW_CNTRL		: // 0x0140
	case TC358768_D0W_CNTRL		: // 0x0144
	case TC358768_D1W_CNTRL		: // 0x0148
	case TC358768_D2W_CNTRL		: // 0x014C
	case TC358768_D3W_CNTRL		: // 0x0150

	// TX PPI (32-bit addressable) */
	case TC358768_STARTCNTRL		: // 0x0204
	case TC358768_DSITXSTATUS		: // 0x0208
	case TC358768_LINEINITCNT		: // 0x0210
	case TC358768_LPTXTIMECNT		: // 0x0214
	case TC358768_TCLK_HEADERCNT		: // 0x0218
	case TC358768_TCLK_TRAILCNT		: // 0x021C
	case TC358768_THS_HEADERCNT		: // 0x0220
	case TC358768_TWAKEUP		: // 0x0224
	case TC358768_TCLK_POSTCNT		: // 0x0228
	case TC358768_THS_TRAILCNT		: // 0x022C
	case TC358768_HSTXVREGCNT		: // 0x0230
	case TC358768_HSTXVREGEN		: // 0x0234
	case TC358768_TXOPTIONCNTRL		: // 0x0238
	case TC358768_BTACNTRL1		: // 0x023C

	// TX CTRL (32-bit addressable) */
	case TC358768_DSI_STATUS		: // 0x0410
	case TC358768_DSI_INT		: // 0x0414
	case TC358768_DSICMD_RXFIFO		: // 0x0430
	case TC358768_DSI_ACKERR		: // 0x0434
	case TC358768_DSI_RXERR		: // 0x0440
	case TC358768_DSI_ERR		: // 0x044C
	case TC358768_DSI_CONFW		: // 0x0500
	case TC358768_DSI_RESET		: // 0x0504
	case TC358768_DSI_INT_CLR		: // 0x050C
	case TC358768_DSI_START		: // 0x0518

	// DSITX CTRL (16-bit addressable) */
	//case TC358768_DSICMD_TX		: // 0x0600
	//case TC358768_DSICMD_TYPE		: // 0x0602
	//case TC358768_DSICMD_WC		: // 0x0604
	//case TC358768_DSICMD_WD0		: // 0x0610
	//case TC358768_DSICMD_WD1		: // 0x0612
	//case TC358768_DSICMD_WD2		: // 0x0614
	//case TC358768_DSICMD_WD3		: // 0x0616
	case TC358768_DSI_EVENT		: // 0x0620
	case TC358768_DSI_VSW		: // 0x0622
	case TC358768_DSI_VBPR		: // 0x0624
	case TC358768_DSI_VACT		: // 0x0626
	case TC358768_DSI_HSW		: // 0x0628
	case TC358768_DSI_HBPR		: // 0x062A
	case TC358768_DSI_HACT		: // 0x062C
		return 1;
	default:
		return 0;
	}
}

static void tc358768_dump(struct tc358768_drv_data *ddata)
{
	unsigned addr;
	for (addr = 0; addr < 0x100; ++ addr)
	{
		unsigned long data;
		if (!isnamed(addr))
			continue;
		tc358768_read(ddata, addr, & data);
		PRINTF("tc358768_write(xx, 0x%04x, 0x%04lx); /* addr=0x%04X, data=0x%04lx */ \n", addr, data, addr, data);
	}
	for (addr = 0x100; addr < 0x700; ++ addr)
	{
		unsigned long data;
		if (!isnamed(addr))
			continue;
		tc358768_read(ddata, addr, & data);
		PRINTF("tc358768_write(xx, 0x%04x, 0x%08lx); /* addr=0x%04X, data=0x%08lx */ \n", addr, data, addr, data);
	}
}

static void tc358768_power_on(struct tc358768_drv_data *ddata, const videomode_t * vdmode)
{
	//const struct omap_video_timings *t = & timings0;

	tc358768_sw_reset(ddata);

	PRINTF("TC358778XBG: Chip and Revision ID=%04X\n", tc358768_rd_reg_16or32bits(TC358768_CHIPID));

	tc358768_setup_pll(ddata);

	/* VSDly[9:0] */
	tc358768_write(ddata, TC358768_VSDLY, 1);
	/* PDFormat[7:4] spmode_en[3] rdswap_en[2] dsitx_en[1] txdt_en[0] */
	tc358768_write(ddata, TC358768_DATAFMT, (0x3 << 4) | (1 << 2) | (1 << 1) | (1 << 0));
	/* dsitx_dt[7:0] 3e = Packed Pixel Stream, 24-bit RGB, 8-8-8 Format*/
	tc358768_write(ddata, TC358768_DSITX_DT, MIPI_DSI_PACKED_PIXEL_STREAM_24);
	//tc358768_write(ddata, TC358768_DSITX_DT, MIPI_DSI_PACKED_PIXEL_STREAM_16);

	/* Enable D-PHY (HiZ->LP11) */
	tc358768_write(ddata, TC358768_CLW_CNTRL, 0x0000);
	tc358768_write(ddata, TC358768_D0W_CNTRL, 0x0000);
	tc358768_write(ddata, TC358768_D1W_CNTRL, 0x0000);
	tc358768_write(ddata, TC358768_D2W_CNTRL, 0x0000);
	tc358768_write(ddata, TC358768_D3W_CNTRL, 0x0000);

	/* DSI Timings */
	/* LP11 = 100 us for D-PHY Rx Init */
	tc358768_write(ddata, TC358768_LINEINITCNT,	0x00002c88);
	tc358768_write(ddata, TC358768_LPTXTIMECNT,	0x00000005);
	tc358768_write(ddata, TC358768_TCLK_HEADERCNT,	0x00001f06);
	tc358768_write(ddata, TC358768_TCLK_TRAILCNT,	0x00000003);
	tc358768_write(ddata, TC358768_THS_HEADERCNT,	0x00000606);
	tc358768_write(ddata, TC358768_TWAKEUP,		0x00004a88);
	tc358768_write(ddata, TC358768_TCLK_POSTCNT,	0x0000000b);
	tc358768_write(ddata, TC358768_THS_TRAILCNT,	0x00000004);
	tc358768_write(ddata, TC358768_HSTXVREGEN,	0x0000001f);

	/* CONTCLKMODE[0] */
	tc358768_write(ddata, TC358768_TXOPTIONCNTRL, 0x1);
	/* TXTAGOCNT[26:16] RXTASURECNT[10:0] */
	tc358768_write(ddata, TC358768_BTACNTRL1, (0x5 << 16) | (0x5));
	/* START[0] */
	tc358768_write(ddata, TC358768_STARTCNTRL, 0x1);

	/* DSI Tx Timing Control */

	/* Set event mode */
	tc358768_write(ddata, TC358768_DSI_EVENT, 1);

	/* vsw (+ vbp) */
	tc358768_write(ddata, TC358768_DSI_VSW, vdmode->vsync + vdmode->vbp);
	/* vbp (not used in event mode) */
	tc358768_write(ddata, TC358768_DSI_VBPR, 0);
	/* vact */
	tc358768_write(ddata, TC358768_DSI_VACT, vdmode->height);

	/* (hsw + hbp) * byteclk * ndl / pclk */
//	tc358768_write(ddata, TC358768_DSI_HSW,
//			(uint32_t) div_u64((t->hsw + t->hbp) * ((uint64_t) ddata->bitclk / 4) * ddata->dsi_lanes, t->pixelclock));
//	tc358768_write(ddata, TC358768_DSI_HSW,
//			(uint32_t) div_u64((t->hsw + t->hbp) * ((uint64_t) ddata->bitclk / 8) * ddata->dsi_lanes, t->pixelclock));
//	tc358768_write(ddata, TC358768_DSI_HSW,
//			(uint32_t) div_u64((t->hsw + t->hbp) * ((uint64_t) ddata->bitclk / 16) * ddata->dsi_lanes, t->pixelclock));

//	tc358768_write(ddata, TC358768_DSI_HSW, (t->hsw + t->hbp) * 3);

	tc358768_write(ddata, TC358768_DSI_HSW, 600);

	/* hbp (not used in event mode) */
	tc358768_write(ddata, TC358768_DSI_HBPR, 0);
	/* hact (bytes) */
	tc358768_write(ddata, TC358768_DSI_HACT, vdmode->width * 3);	/* зависит от того, какой входной формат */
	////tc358768_write(ddata, TC358768_DSI_HACT, 3000);

	/* Start DSI Tx */
	tc358768_write(ddata, TC358768_DSI_START, 0x1);

	/* SET, DSI_Control, 0xa7 */
	/* 0xa7 = HS | CONTCLK | 4-datalines | EoTDisable */
	tc358768_write(ddata, TC358768_DSI_CONFW, (5<<29) | (0x3 << 24) | 0xa7);
	/* CLEAR, DSI_Control, 0x8001 */
	/* 0x8001 = DSIMode */
	tc358768_write(ddata, TC358768_DSI_CONFW, (6<<29) | (0x3 << 24) | 0x8000);

	/* clear FrmStop and RstPtr */
	tc358768_update_bits(ddata, TC358768_PP_MISC, 0x3 << 14, 0);	// FrmStop=0, RstPtr=0

	/* set PP_en */
	tc358768_update_bits(ddata, TC358768_CONFCTL, 1 << 6, 1 << 6);
}

static void tc358768_power_off(struct tc358768_drv_data *ddata)
{
	/* set FrmStop */
	tc358768_update_bits(ddata, TC358768_PP_MISC, 1 << 15, 1 << 15);

	/* wait at least for one frame */
	local_delay_ms(50);

	/* clear PP_en */
	tc358768_update_bits(ddata, TC358768_CONFCTL, 1 << 6, 0);

	/* set RstPtr */
	tc358768_update_bits(ddata, TC358768_PP_MISC, 1 << 14, 1 << 14);
}
//
//unsigned getw2(void)
//{
//	struct tc358768_drv_data * ddata = & dev0;
//	unsigned long val;
//	tc358768_read(ddata, TC358768_DSI_HSW, & val);
//
//	return val;
//}
//
//unsigned setw(unsigned w)
//{
//	struct tc358768_drv_data * ddata = & dev0;
//	unsigned long val;
//
//	tc358768_power_off(ddata);
//	tc358768_write(ddata, TC358768_DSI_HSW, w);
//	tc358768_read(ddata, TC358768_DSI_HSW, & val);
//	local_delay_ms(20);
//	tc358768_power_on(ddata);
//
//	return val;
//}
//
//unsigned getq2(void)
//{
//	struct tc358768_drv_data * ddata = & dev0;
//	unsigned long val;
//	tc358768_read(ddata, TC358768_DSI_HACT, & val);
//
//	return val;
//}
//
//unsigned setq(unsigned q)
//{
//	struct tc358768_drv_data * ddata = & dev0;
//	unsigned long val;
//
//	//tc358768_power_off(ddata);
//	//tc358768_write(ddata, TC358768_DSI_HACT, q);
//	tc358768_read(ddata, TC358768_DSI_HACT, & val);
//	//local_delay_ms(20);
//	//tc358768_power_on(ddata);
//
//	return val;
//}

//////////////////////////////////

void tc_print(uint32_t addr) {
	//PRINTF("+++++++++++addr->%04x: %04x\n", addr, tc358768_rd_reg_16or32bits(addr));
}

#define tc358768_wr_regs_32bits(reg_array)  _tc358768_wr_regs_32bits(reg_array, ARRAY_SIZE(reg_array))
int _tc358768_wr_regs_32bits(unsigned int reg_array[], uint32_t n) {

	int i = 0;
	//PRINTF("%s:%d\n", __func__, n);
	for(i = 0; i < n; i++) {
		if(reg_array[i] < 0x00020000) {
		    if(reg_array[i] < 20000)
		    	local_delay_us(2*reg_array[i]);
		    else {
		    	local_delay_ms(2*reg_array[i]/1000);
		    }
		} else {
			tc358768_wr_reg_16bits(reg_array[i]);
		}
	}
	return 0;
}

int tc358768_command_tx_less8bytes(unsigned char type, const unsigned char *regs, uint32_t n) {
	int i = 0;
	unsigned int command[] = {
			0x06020000,
			0x06040000,
			0x06100000,
			0x06120000,
			0x06140000,
			0x06160000,
	};

	if(n <= 2)
		command[0] |= 0x1000;   //short packet
	else {
		command[0] |= 0x4000;   //long packet
		command[1] |= n;		//word count byte
	}
	command[0] |= type;         //data type

	//PRINTF("*cmd:\n");
	//PRINTF("0x%08x\n", command[0]);
	//PRINTF("0x%08x\n", command[1]);

	for(i = 0; i < (n + 1)/2; i++) {
		command[i+2] |= regs[i*2];
		if((i*2 + 1) < n)
			command[i+2] |= regs[i*2 + 1] << 8;
		//PRINTF("0x%08x\n", command[i+2]);
	}

	_tc358768_wr_regs_32bits(command, (n + 1)/2 + 2);
	tc358768_wr_reg_16bits(0x06000001);   //Packet Transfer
	//wait until packet is out
	i = 1000;
	while (tc358768_rd_reg_16or32bits(TC358768_DSICMD_TX) & 0x01) {
		if(i-- == 0)
			break;
		tc_print(TC358768_DSICMD_TX);
	}
	//local_delay_us(50);
	return 0;
}

int tc358768_command_tx_more8bytes_hs(unsigned char type, unsigned char regs[], uint32_t n) {

	int i = 0;
	unsigned int dbg_data = 0x00E80000, temp = 0;
	unsigned int command[] = {
			0x05000080,    //HS data 4 lane, EOT is added
			0x0502A300,
			0x00080001,
			0x00500000,    //Data ID setting
			0x00220000,    //Transmission byte count= byte
			0x00E08000,	   //Enable I2C/SPI write to VB
			0x00E20048,    //Total word count = 0x48 (local_max 0xFFF). This value should be adjusted considering trade off between transmission time and transmission start/stop time delay
			0x00E4007F,    //Vertical blank line = 0x7F
	};


	command[3] |= type;        //data type
	command[4] |= n & 0xffff;           //Transmission byte count

	tc358768_wr_regs_32bits(command);

	for(i = 0; i < (n + 1)/2; i++) {
		temp = dbg_data | regs[i*2];
		if((i*2 + 1) < n)
			temp |= (regs[i*2 + 1] << 8);
		//PRINTF("0x%08x\n", temp);
		tc358768_wr_reg_16bits(temp);
	}
	if((n % 4 == 1) ||  (n % 4 == 2))     //4 bytes align
		tc358768_wr_reg_16bits(dbg_data);

	tc358768_wr_reg_16bits(0x00E0C000);     //Start command transmisison
	tc358768_wr_reg_16bits(0x00E00000);	 //Stop command transmission. This setting should be done just after above setting to prevent multiple output
	local_delay_us(2*200);
	//Re-Initialize
	//tc358768_wr_regs_32bits(re_initialize);
	return 0;
}

//low power mode only for tc358768a
int tc358768_command_tx_more8bytes_lp(unsigned char type, const unsigned char regs[], uint32_t n) {

	int i = 0;
	unsigned int dbg_data = 0x00E80000, temp = 0;
	unsigned int command[] = {
			0x00080001,
			0x00500000,    //Data ID setting
			0x00220000,    //Transmission byte count= byte
			0x00E08000,	   //Enable I2C/SPI write to VB
	};

	command[1] |= type;        //data type
	command[2] |= n & 0xffff;           //Transmission byte count

	tc358768_wr_regs_32bits(command);

	for(i = 0; i < (n + 1)/2; i++) {
		temp = dbg_data | regs[i*2];
		if((i*2 + 1) < n)
			temp |= (regs[i*2 + 1] << 8);
		//PRINTF("0x%08x\n", temp);
		tc358768_wr_reg_16bits(temp);

	}
	if((n % 4 == 1) ||  (n % 4 == 2))     //4 bytes align
		tc358768_wr_reg_16bits(dbg_data);

	tc358768_wr_reg_16bits(0x00E0E000);     //Start command transmisison
	local_delay_us(2*1000);
	tc358768_wr_reg_16bits(0x00E02000);	 //Keep Mask High to prevent short packets send out
	tc358768_wr_reg_16bits(0x00E00000);	 //Stop command transmission. This setting should be done just after above setting to prevent multiple output
	local_delay_us(2*10);
	return 0;
}

int _tc358768_send_packet(unsigned char type, const unsigned char regs[], uint32_t n) {

	if(n <= 8) {
		tc358768_command_tx_less8bytes(type, regs, n);
	} else {
		//tc358768_command_tx_more8bytes_hs(type, regs, n);
////		tc358768_command_tx_more8bytes_lp(type, regs, n);
	}
	return 0;
}

int tc358768_send_packet(unsigned char type, unsigned char regs[], uint32_t n) {
	return _tc358768_send_packet(type, regs, n);
}


/*
The DCS is separated into two functional areas: the User Command Set and the Manufacturer Command
Set. Each command is an eight-bit code with 00h to AFh assigned to the User Command Set and all other
codes assigned to the Manufacturer Command Set.
*/
int _mipi_dsi_send_dcs_packet(const unsigned char regs[], uint32_t n) {

	unsigned char type = 0;
	if(n == 1) {
		type = DTYPE_DCS_SWRITE_0P;
	} else if (n == 2) {
		type = DTYPE_DCS_SWRITE_1P;
	} else if (n > 2) {
		type = DTYPE_DCS_LWRITE;
	}
	_tc358768_send_packet(type, regs, n);
	return 0;
}

int mipi_dsi_send_dcs_packet(const unsigned char regs[], uint32_t n) {
	return _mipi_dsi_send_dcs_packet(regs, n);
}


int _tc358768_rd_lcd_regs(unsigned char type, char comd, int size, unsigned char* buf) {

	unsigned char regs[8];
	uint32_t count = 0, data30, data32;
	regs[0] = size;
	regs[1] = 0;
	tc358768_command_tx_less8bytes(0x37, regs, 2);
	tc358768_wr_reg_16bits(0x05040010);
	tc358768_wr_reg_16bits(0x05060000);
	regs[0] = comd;
	tc358768_command_tx_less8bytes(type, regs, 1);

	while (!(tc358768_rd_reg_16or32bits(TC358768_DSI_STATUS) & 0x20)){
		PRINTF("error 0x0410:%04x\n", tc358768_rd_reg_16or32bits(TC358768_DSI_STATUS));
		local_delay_ms(2*1);
		if(count++ > 10) {
			break;
		}
	}

	data30 = tc358768_rd_reg_16or32bits(0x0430);	  //data id , word count[0:7]
	//PRINTF("0x0430:%04x\n", data30);
	data32 = tc358768_rd_reg_16or32bits(0x0432);	  //word count[8:15]  ECC
	//PRINTF("0x0432:%04x\n", data32);

	while(size > 0) {
		data30 = tc358768_rd_reg_16or32bits(0x0430);
		//PRINTF("0x0430:%04x\n", data30);
		data32 = tc358768_rd_reg_16or32bits(0x0432);
		//PRINTF("0x0432:%04x\n", data32);

		if(size-- > 0)
			*buf++ = (uint8_t)data30;
		else
			break;
		if(size-- > 0)
			*buf++ = (uint8_t) (data30 >> 8);
		else
			break;
		if(size-- > 0) {
			*buf++ = (uint8_t)data32;
			if(size-- > 0)
				*buf++ = (uint8_t) (data32 >> 8);
		}
	}

	data30 = tc358768_rd_reg_16or32bits(0x0430);
	//PRINTF("0x0430:%04x\n", data30);
	data32 = tc358768_rd_reg_16or32bits(0x0432);
	//PRINTF("0x0432:%04x\n", data32);
	return 0;
}

int mipi_dsi_read_dcs_packet(unsigned char *data, uint32_t n) {
	//DCS READ
	_tc358768_rd_lcd_regs(0x06, *data, n, data);
	return 0;
}

int tc358768_get_id(void) {

	int id = -1;

	//tc358768_power_up();
	id = tc358768_rd_reg_16or32bits(0);
	return id;
}


/*
	panel {
		compatible = "samsung,s6e8aa0";
		reg = <0>;
		vdd3-supply = <&vcclcd_reg>;
		vci-supply = <&vlcd_reg>;
		reset-gpios = <&gpy4 5 0>;
		power-on-delay= <50>;
		reset-delay = <100>;
		init-delay = <100>;
		panel-width-mm = <58>;
		panel-height-mm = <103>;
		flip-horizontal;
		flip-vertical;
		display-timings {
			timing0: timing-0 {
				clock-frequency = <57153600>;
				hactive = <720>;
				vactive = <1280>;
				hfront-porch = <5>;
				hback-porch = <5>;
				hsync-len = <5>;
				vfront-porch = <16>;
				vback-porch = <11>;
				vsync-len = <5>;
			};
		};
	};
*/

/*
 * MIPI CMD
 */
enum {
	MIPI_GEN_CMD = 0,
	MIPI_DCS_CMD,
};

enum {
	GEN_SW_0P_TX = 1,
	GEN_SW_1P_TX,
	GEN_SW_2P_TX,
	GEN_SR_0P_TX,
	GEN_SR_1P_TX,
	GEN_SR_2P_TX,
	DCS_SW_0P_TX,
	DCS_SW_1P_TX,
	DCS_SR_0P_TX,
	MAX_RD_PKT_SIZE,
	GEN_LW_TX,
	DCS_LW_TX,
};

struct comipfb_info ;

extern int mipi_dsih_gen_wr_cmd(struct comipfb_info *fbi, uint8_t vc, uint8_t* params, uint16_t param_length);
extern int mipi_dsih_dcs_wr_cmd(struct comipfb_info *fbi, uint8_t vc, uint8_t* params, uint16_t param_length);
extern uint16_t mipi_dsih_dcs_rd_cmd(struct comipfb_info *fbi, uint8_t vc, uint8_t command, uint8_t bytes_to_read, uint8_t* read_buffer);
extern uint16_t mipi_dsih_gen_rd_cmd(struct comipfb_info *fbi, uint8_t vc, uint8_t* params, uint16_t param_length, uint8_t bytes_to_read, uint8_t* read_buffer);
extern int mipi_dsih_gen_wr_packet(struct comipfb_info *fbi, uint8_t vc, uint8_t data_type, uint8_t* params, uint16_t param_length);

static struct comipfb_info *fs_fbi;

static uint8_t mipi_read_buff[30];


#define ROW_LINE 64

#define COMIPFB_HSYNC_HIGH_ACT		(0x03)
#define COMIPFB_VSYNC_HIGH_ACT		(0x04)

#define MIPI_VIDEO_MODE 		(0x05)
#define MIPI_COMMAND_MODE 		(0x06)

#define ARRAY_AND_SIZE(x)		(uint8_t *)(x), ARRAY_SIZE(x)

/*display prefer and ce*/
#define PREFER_INIT			(0)
#define PREFER_WARM			(1)
#define PREFER_NATURE			(2)
#define PREFER_COOL			(3)

#define CE_BRIGHT 			(12)
#define CE_VELVIA			(11)
#define CE_STANDARD			(10)


enum {
	DCS_CMD = 2,
	GEN_CMD,
	SW_PACK0,
	SW_PACK1,
	SW_PACK2,
	LW_PACK,
	SHUTDOWN_SW_PACK,
};

/*Device flags*/
#define	PREFER_CMD_SEND_MONOLITHIC	(0x00000001)
#define	CE_CMD_SEND_MONOLITHIC		(0x00000002)

#define RESUME_WITH_PREFER		(0x00000010)
#define RESUME_WITH_CE			(0x00000020)


/**
 * Video stream type
 */
typedef enum {
	VIDEO_NON_BURST_WITH_SYNC_PULSES = 0,
	VIDEO_NON_BURST_WITH_SYNC_EVENTS,	// hs_freq and pck should be multipe
	VIDEO_BURST_WITH_SYNC_PULSES,
}dsih_video_mode_t;

#ifdef CONFIG_FBCON_DRAW_PANIC_TEXT
extern int kpanic_in_progress;
#endif

struct comipfb_dev_cmds {
	unsigned char *cmds;
	unsigned short n_pack;
	unsigned short n_cmds;
};

struct bl_cmds {
	struct comipfb_dev_cmds bl_cmd;
	unsigned int brightness_bit;
};

struct mipi_color_bits {
	unsigned int color_bits; // must be set !!
	unsigned int is_18bit_loosely; // optional
};

struct rw_timeout {
	unsigned int hs_rd_to_cnt;
	unsigned int lp_rd_to_cnt;
	unsigned int hs_wr_to_cnt;
	unsigned int lp_wr_to_cnt;
	unsigned int bta_to_cnt;
};

struct video_mode_info {
	unsigned int hsync;	/* Horizontal Synchronization, unit : pclk. */
	unsigned int hbp;	/* Horizontal Back Porch, unit : pclk. */
	unsigned int hfp;	/* Horizontal Front Porch, unit : pclk. */
	unsigned int vsync;	/* Vertical Synchronization, unit : line. */
	unsigned int vbp;	/* Vertical Back Porch, unit : line. */
	unsigned int vfp;	/* Vertical Front Porch, unit : line. */
	unsigned int sync_pol;
	unsigned int lp_cmd_en:1;
	unsigned int frame_bta_ack:1;
	unsigned int lp_hfp_en:1; // default should be 1
	unsigned int lp_hbp_en:1;
	unsigned int lp_vact_en:1;
	unsigned int lp_vfp_en:1;
	unsigned int lp_vbp_en:1;
	unsigned int lp_vsa_en:1;
	dsih_video_mode_t mipi_trans_type; /* burst or no burst*/
};

struct command_mode_info {
	unsigned int tear_fx_en:1;
	unsigned int ack_rqst_en:1;
	unsigned int gen_sw_0p_tx:1;	// default should be 1
	unsigned int gen_sw_1p_tx:1;	// default should be 1
	unsigned int gen_sw_2p_tx:1;	// default should be 1
	unsigned int gen_sr_0p_tx:1;	// default should be 1
	unsigned int gen_sr_1p_tx:1;	// default should be 1
	unsigned int gen_sr_2p_tx:1;	// default should be 1
	unsigned int gen_lw_tx:1;		// default should be 1
	unsigned int dcs_sw_0p_tx:1;	// default should be 1
	unsigned int dcs_sw_1p_tx:1;	// default should be 1
	unsigned int dcs_sr_0p_tx:1;	// default should be 1
	unsigned int dcs_lw_tx:1;		// default should be 1
	unsigned int max_rd_pkt_size:1;	// default should be 1
	struct rw_timeout timeout;
};

struct external_info {
	unsigned char crc_rx_en:1;
	unsigned char ecc_rx_en:1;
	unsigned char eotp_rx_en:1;
	unsigned char eotp_tx_en:1;
	unsigned int dev_read_time;	//HSBYTECLK is danwe
};

struct phy_time_info {
	unsigned char lpx;
	//unsigned char clk_lpx;
	unsigned char clk_tprepare;
	unsigned char clk_hs_zero;
	unsigned char clk_hs_trail;
	unsigned char clk_hs_exit;
	unsigned char clk_hs_post;

	//unsigned char data_lpx;
	unsigned char data_tprepare;
	unsigned char data_hs_zero;
	unsigned char data_hs_trail;
	unsigned char data_hs_exit;
	unsigned char data_hs_post;
};

struct te_info {
	unsigned int te_source;
	unsigned int te_trigger_mode;
	unsigned int te_en;
	unsigned int te_sync_en; // In command mode should set 1,  video should set 0
	unsigned int te_cps;	// te count per second
};

struct comipfb_dev_timing_mipi {
	unsigned int hs_freq; /*PHY output freq, bytes KHZ*/
	unsigned int lp_freq; /*default is 10MHZ*/
	unsigned int no_lanes; /*lane numbers*/
	unsigned int display_mode; //video mode or command mode.
	unsigned int auto_stop_clklane_en;
	unsigned int im_pin_val; /*IM PIN val, if use gpio_im, default config is 1  ?? */
	struct mipi_color_bits color_mode; /*color bits*/
	struct video_mode_info videomode_info;
	struct command_mode_info commandmode_info;
	struct phy_time_info phytime_info;
	struct te_info teinfo;
	struct external_info ext_info;
};

struct common_id_info {
	unsigned char pack_type;
	unsigned char id[6];
	unsigned char id_count;
	unsigned char cmd;
};

struct comipfb_id_info {
	unsigned char num_id_info;
	struct common_id_info *id_info;
	struct comipfb_dev_cmds prepare_cmd;
};

struct prefer_ce_info {
	int type;
	struct comipfb_dev_cmds cmds;
};

struct comipfb_prefer_ce {
	int types;
	struct prefer_ce_info *info;
};

struct comipfb_dev {
	const char* name;	/* Device name. */
	unsigned int interface_info;//interface infomation  MIPI or RGB
	unsigned int lcd_id;
	unsigned int refresh_en;	/* Refresh enable. */
	unsigned int pclk;		/* Pixel clock in HZ. */
	unsigned int bpp;		/* Bits per pixel. */
	unsigned int xres;		/* Device resolution. */
	unsigned int yres;
	unsigned int width;		/* Width of device in mm. */
	unsigned int height;		/* Height of device in mm. */
	unsigned int flags;		/* Device flags. */
	unsigned int auto_fps;		/* auto adjust frame rate flag*/
	union {
		//struct comipfb_dev_timing_rgb rgb;
		struct comipfb_dev_timing_mipi mipi;
	} timing;

	struct comipfb_id_info panel_id_info;
	struct comipfb_id_info esd_id_info;
	struct comipfb_dev_cmds cmds_init;
	struct comipfb_dev_cmds cmds_suspend;
	struct comipfb_dev_cmds cmds_resume;
	struct comipfb_dev_cmds cmds_pre_suspend;
	struct bl_cmds backlight_info;

	struct comipfb_prefer_ce resume_prefer_info;
	struct comipfb_prefer_ce display_prefer_info;
	struct comipfb_prefer_ce display_ce_info;
	int init_last;		/*when resume, send gamma/ce cmd before init cmd*/

	int (*power)(struct comipfb_info *fbi, int onoff);
	int (*reset)(struct comipfb_info *fbi);
	int (*suspend)(struct comipfb_info *fbi);
	int (*resume)(struct comipfb_info *fbi);
};


#define ENTER_ULPS	0x01
#define EXIT_ULPS	0x02

struct comipfb_info;
struct comipfb_dev_cmds;

struct comipfb_if {
	int (*init)(struct comipfb_info *fbi);
	int (*exit)(struct comipfb_info *fbi);
	int (*suspend)(struct comipfb_info *fbi);
	int (*resume)(struct comipfb_info *fbi);
	int (*dev_cmd)(struct comipfb_info *fbi, struct comipfb_dev_cmds *cmds);
	void (*bl_change)(struct comipfb_info *fbi, int val);
};
extern struct comipfb_if comipfb_if_mipi;
extern struct comipfb_if* comipfb_if_get(struct comipfb_info *fbi);
extern int comipfb_if_mipi_dev_cmds(struct comipfb_info *fbi, struct comipfb_dev_cmds *cmds);
//extern int comipfb_if_mipi_init(struct comipfb_info *fbi);

struct device_attribute;
struct device;

extern int comipfb_dev_register(struct comipfb_dev* dev);
extern int comipfb_dev_unregister(struct comipfb_dev* dev);
extern struct comipfb_dev* comipfb_dev_get(struct comipfb_info *fbi);

#if 0

static ssize_t mipi_read_store(struct device *dev, struct device_attribute *attr,
								const char *buf, size_t count)
{
	int cmd_type, read_cmd, read_count;

	sscanf(buf,"%x%x%x", &cmd_type, &read_cmd, &read_count);

	memset(&mipi_read_buff, 0x0, ARRAY_SIZE(mipi_read_buff));

	mipi_dsih_gen_wr_packet(fs_fbi, 0, 0x37, (uint8_t *)&read_count, 1);
	if (cmd_type == 1)
		mipi_dsih_dcs_rd_cmd(fs_fbi, 0, (uint8_t)read_cmd, (uint8_t)read_count,
			(uint8_t *)&mipi_read_buff);
	else if (cmd_type == 0)
		mipi_dsih_gen_rd_cmd(fs_fbi, 0, (uint8_t *)&read_cmd, 1, (uint8_t)read_count,
			(uint8_t *)&mipi_read_buff);

	return count;
}
//
//static ssize_t mipi_read_show(struct device *dev, struct device_attribute *attr, char *buf)
//{
//	return snprintf(buf,
//			PAGE_SIZE,
//			"buf[0]=0x%x, buf[1]=0x%x, buf[2]=0x%x, buf[3]=0x%x, buf[4]=0x%x\n",
//			mipi_read_buff[0],
//			mipi_read_buff[1],
//			mipi_read_buff[2],
//			mipi_read_buff[3],
//			mipi_read_buff[4]);
//}

//DEVICE_ATTR(mipi_read, S_IRUGO | S_IWUSR, mipi_read_show, mipi_read_store);
//
//void comipfb_sysfs_add_read(struct device *dev)
//{
//	device_create_file(dev, &dev_attr_mipi_read);
//}
//
//void comipfb_sysfs_remove_read(struct device *dev)
//{
//	device_remove_file(dev, &dev_attr_mipi_read);
//}

#define KERN_ERR "panel: "
//EXPORT_SYMBOL(comipfb_sysfs_remove_read);
//EXPORT_SYMBOL(comipfb_sysfs_add_read);

/*
 *
 * MIPI interface
 */
static int comipfb_mipi_mode_change(struct comipfb_info *fbi)
{
//	int gpio_im, gpio_val;
//	struct comipfb_dev_timing_mipi *mipi;
//
//	mipi = &(fbi->cdev->timing.mipi);
//
//	if (fbi != NULL) {
//		gpio_im = fbi->pdata->gpio_im;
//		gpio_val = mipi->im_pin_val;
//		if (gpio_im >= 0) {
//			gpio_request(gpio_im, "LCD IM");
//			gpio_direction_output(gpio_im, gpio_val);
//		}
//	}
	return 0;
}

int comipfb_if_mipi_dev_cmds(struct comipfb_info *fbi, struct comipfb_dev_cmds *cmds)
{
	int ret = 0;
	int loop = 0;
	unsigned char *p;

	if (!cmds) {
		PRINTF("cmds null\n");
		return -(1);
	}
	if (!cmds->n_pack || !cmds->cmds) {
		PRINTF("cmds null\n");
		return -(1);
	}
	for (loop = 0; loop < cmds->n_pack; loop++) {
		p = cmds->cmds + loop * ROW_LINE;
		if (p[1] == DCS_CMD)
			ret = mipi_dsih_dcs_wr_cmd(fbi, 0, &(p[2]), (uint16_t)p[3]);
		else if (p[1] == GEN_CMD)
			ret = mipi_dsih_gen_wr_cmd(fbi, 0, &(p[2]), (uint16_t)p[3]);
		if (ret < 0) {
			PRINTF("*MIPI send command failed !!*\n");
			return ret;
		}
		if (p[0] > 0) {
#ifdef CONFIG_FBCON_DRAW_PANIC_TEXT
			if (unlikely(kpanic_in_progress == 1)) {
				if (p[0] == 0xff)
					local_delay_ms(200);
				else
					local_delay_ms(p[0]);
			}
			else {

				if (p[0] == 0xff)
					local_delay_ms(200);
				else
					local_delay_ms(p[0]);
			}
#else
			if (p[0] == 0xff)
				local_delay_ms(200);
			else
				local_delay_ms(p[0]);
#endif

		}
	}
	return ret;
}

void comipfb_if_mipi_reset(struct comipfb_info *fbi)
{
	struct comipfb_dev_timing_mipi *mipi;
	int stop_status = 0;
	int check_times = 3;
	int i = 0;

	//mipi = &(fbi->cdev->timing.mipi);

	switch (4) //(mipi->no_lanes) {
		case 1:
			stop_status = 0x10;
			break;
		case 2:
			stop_status = 0x90;
			break;
		case 3:
			stop_status = 0x290;
			break;
		case 4:
			stop_status = 0xa90;
			break;
		default:
			break;
	}

	for (i = 0; i < check_times; i++) {
		if ((mipi_dsih_dphy_get_status(fbi) & stop_status) == stop_status)
			break;
		local_delay_us(2*5);
	}

//	mipi_dsih_hal_power(fbi, 0);
//	mipi_dsih_hal_power(fbi, 1);
}

static int comipfb_if_mipi_init(struct comipfb_info *fbi)
{
	int ret = 0;
	struct comipfb_dev_timing_mipi *mipi;
	unsigned int dev_flags;

	fs_fbi = fbi;

	mipi = &(fbi->cdev->timing.mipi);

	comipfb_mipi_mode_change(fbi);

	mipi_dsih_dphy_enable_hs_clk(fbi, 1);
	local_delay_ms(5);

	if (mipi->display_mode == MIPI_VIDEO_MODE)
		mipi_dsih_hal_mode_config(fbi, 1);

	mipi_dsih_dphy_enable_hs_clk(fbi, 0);
	/* Reset device. */
	if (!(fbi->cdev->power)) {
		if (fbi->cdev->reset)
			fbi->cdev->reset(fbi);
	}

	if (!fbi->cdev->init_last)
		ret = comipfb_if_mipi_dev_cmds(fbi, &fbi->cdev->cmds_init);

	dev_flags = fbi->cdev->flags;
	if ((fbi->display_prefer != 0) && (dev_flags & RESUME_WITH_PREFER)) {
		ret = comipfb_display_prefer_set(fbi, fbi->display_prefer);
		if (ret)
			dev_warn(fbi->dev, "set prefer within resume failed\n");
	}
	if ((fbi->display_ce != 0) && (dev_flags & RESUME_WITH_CE)) {
		ret = comipfb_display_ce_set(fbi, fbi->display_ce);
		if (ret)
			dev_warn(fbi->dev, "set ce within resume failed\n");
	}
	if (fbi->cdev->init_last)
		ret = comipfb_if_mipi_dev_cmds(fbi, &fbi->cdev->cmds_init);
	local_delay_ms(10);

	if (mipi->display_mode == MIPI_VIDEO_MODE) {
		mipi_dsih_hal_mode_config(fbi, 0);
	}else
		mipi_dsih_hal_dcs_wr_tx_type(fbi, 3, 0);

	local_delay_ms(5);
	mipi_dsih_dphy_enable_hs_clk(fbi, 1);

	return ret;
}

static int comipfb_if_mipi_exit(struct comipfb_info *fbi)
{
//	int gpio_im = fbi->pdata->gpio_im;
//	if (gpio_im >= 0)
//		gpio_free(gpio_im);
//
	return 0;
}

static int comipfb_if_mipi_suspend(struct comipfb_info *fbi)
{
	int ret = 0;
//
//	if(fbi->cdev->suspend)
//		ret = fbi->cdev->suspend(fbi);
//
	return ret;
}

static int comipfb_if_mipi_resume(struct comipfb_info *fbi)
{
	int ret = 0;
//
//	if(fbi->cdev->resume)
//		ret = fbi->cdev->resume(fbi);
//
	return ret;
}

static void comipfb_if_mipi_bl_change(struct comipfb_info *fbi, int val)
{
	unsigned int bit;
	struct comipfb_dev_cmds *lcd_backlight_cmd;

	if (fbi == NULL) {
		PRINTF(KERN_ERR "%s ,fbi is NULL",__func__);
		return ;
	}

	bit = fbi->cdev->backlight_info.brightness_bit;
 	lcd_backlight_cmd = &(fbi->cdev->backlight_info.bl_cmd);
	lcd_backlight_cmd->cmds[bit] = (unsigned char)val;
	comipfb_if_mipi_dev_cmds(fbi, lcd_backlight_cmd);
}

static unsigned char te_cmds[][10] = {
/****TE command***/
        {0x00, DCS_CMD, SW_PACK2, 0x02, 0x35, 0x00},
};

static void comipfb_if_mipi_te_trigger(struct comipfb_info *fbi)
{
	struct comipfb_dev_cmds te_pkt;

	te_pkt.cmds = (unsigned char *)te_cmds;
	te_pkt.n_pack = 1;

	comipfb_if_mipi_dev_cmds(fbi, &te_pkt);
}


static int comipfb_if_mipi_read_lcm_id(struct comipfb_info *fbi , struct comipfb_id_info *cur_id_info)
{
	unsigned char rd_cnt=0,lp_cnt=0;
	unsigned char cmd;
	unsigned char *id_val;
	uint8_t lcm_id [8] = {0};
	int i, ret = 0;

	/*ready to read id*/
	if(cur_id_info->prepare_cmd.cmds)
		comipfb_if_mipi_dev_cmds(fbi, &(cur_id_info->prepare_cmd));

	while (lp_cnt < cur_id_info->num_id_info) {
		cmd = cur_id_info->id_info[lp_cnt].cmd;
		rd_cnt = cur_id_info->id_info[lp_cnt].id_count;
		id_val = cur_id_info->id_info[lp_cnt].id;

		mipi_dsih_gen_wr_packet(fbi, 0, 0x37, &rd_cnt, 1);
		if (cur_id_info->id_info[lp_cnt].pack_type == DCS_CMD) {
			ret = mipi_dsih_dcs_rd_cmd(fbi, 0, cmd, rd_cnt, lcm_id);
		} else if (cur_id_info->id_info[lp_cnt].pack_type == GEN_CMD) {
			ret = mipi_dsih_gen_rd_cmd(fbi, 0, &cmd, 1, rd_cnt, lcm_id);
		}
		ret = strncmp((void *) lcm_id, (void *) id_val, rd_cnt);
		if (ret) {
			PRINTF("request:");
			for (i = 0; i< rd_cnt; i++)
				PRINTF("0x%x,", id_val[i]);
			PRINTF(" read:");
			for (i = 0; i< rd_cnt; i++)
				PRINTF("0x%x,", lcm_id[i]);
			return -1;
		}
		lp_cnt++;
	}
	return 0;
}

int comipfb_if_mipi_esd_read_ic(struct comipfb_info *fbi )
{
	int check_result;

	if (!fbi)
		return 0;

	check_result = comipfb_if_mipi_read_lcm_id( fbi , &(fbi->cdev->esd_id_info));

	if (check_result) {
		PRINTF("lcd esd read id failed \n");
		return -1;
	} else {
		return 0;
	}
}

int comipfb_read_lcm_id(struct comipfb_info *fbi, void *dev)
{
	static int check_result, common_pwup = 0;
		struct comipfb_dev *lcm_dev = (struct comipfb_dev *)dev;

	fbi->cdev = lcm_dev;

	/*power on LCM and reset*/
	if (fbi->cdev->power)
		fbi->cdev->power(fbi, 1);
	else {
		if ((!common_pwup) && fbi->pdata->power) {
			fbi->pdata->power(1);
			common_pwup = 1;
		}
		if (fbi->cdev->reset)
			fbi->cdev->reset(fbi);
	}
	check_result = comipfb_if_mipi_read_lcm_id(fbi, &(lcm_dev->panel_id_info));

	if (fbi->cdev->power)
		fbi->cdev->power(fbi, 0);
	if ( check_result ) {
		PRINTF("read lcm id failed \n");
		return -1;
	} else {
		return 0;
	}
}
//static DEFINE_SPINLOCK(cmd_lock);
int comipfb_display_prefer_ce_set(struct comipfb_info *fbi, struct comipfb_prefer_ce *prefer_ce, int mode)
{
	struct prefer_ce_info *info = prefer_ce->info;
	int cnt = 0;
	int ret = 0;

	while (cnt < prefer_ce->types) {
		if (info->type == mode) {
			break;
		}
		info++;
		cnt++;
	}

	if (cnt == prefer_ce->types) {
		PRINTF("mode %d not supported\n", mode);
		return -(1);
	}

	//dev_info(fbi->dev, "sending cmds : type = %d\n", info->type);//for debug
	ret = comipfb_if_mipi_dev_cmds(fbi, &info->cmds);

	return ret;

}

int comipfb_display_prefer_set(struct comipfb_info *fbi, int mode)
{
	struct comipfb_prefer_ce *prefer = &fbi->cdev->display_prefer_info;
	unsigned int dev_flags;
	unsigned long flags;
	int ret = 0;

	if (prefer->types == 0) {
		dev_info(fbi->dev, "%s do not support prefer set\n", fbi->cdev->name);
		return -(1);
	}

	dev_flags = fbi->cdev->flags;
	/*cmds must be send continuous and cannot be interrupted*/
	if (dev_flags & PREFER_CMD_SEND_MONOLITHIC)
		;//spin_lock_irqsave(&cmd_lock, flags);
	ret = comipfb_display_prefer_ce_set(fbi, prefer, mode);
	if (dev_flags & PREFER_CMD_SEND_MONOLITHIC)
		;//spin_unlock_irqrestore(&cmd_lock, flags);

	if (ret)
		PRINTF("set display prefer %d failed\n", mode);

	/*update current prefer mode, when resume from suspend, should check and send cmd if need*/
	fbi->display_prefer = mode;

	return ret;
}

int comipfb_display_ce_set(struct comipfb_info *fbi, int mode)
{
	struct comipfb_prefer_ce *ce = &fbi->cdev->display_ce_info;
	unsigned int dev_flags;
	unsigned long flags;
	int ret = 0;

	if (ce->types == 0) {
		dev_info(fbi->dev, "%s do not support ce set\n", fbi->cdev->name);
		return -(1);
	}

	dev_flags = fbi->cdev->flags;
	/*cmds must be send continuous and cannot be interrupted*/
	if (dev_flags & CE_CMD_SEND_MONOLITHIC)
		;//spin_lock_irqsave(&cmd_lock, flags);
	ret = comipfb_display_prefer_ce_set(fbi, ce, mode);
	if (dev_flags & CE_CMD_SEND_MONOLITHIC)
		;//spin_unlock_irqrestore(&cmd_lock, flags);

	if (ret)
		PRINTF("set display ce %d failed\n", mode);

	/*update current ce mode, when resume from suspend, should check and send cmd if need*/
	fbi->display_ce = mode;

	return ret;
}

struct comipfb_if comipfb_if_mipi = {
        .init           = comipfb_if_mipi_init,
        .exit           = comipfb_if_mipi_exit,
        .suspend        = comipfb_if_mipi_suspend,
        .resume         = comipfb_if_mipi_resume,
        .dev_cmd        = comipfb_if_mipi_dev_cmds,
	.bl_change	= comipfb_if_mipi_bl_change,
	.te_trigger = comipfb_if_mipi_te_trigger,
};

struct comipfb_if* comipfb_if_get(struct comipfb_info *fbi)
{

	switch (fbi->cdev->interface_info) {
		case COMIPFB_MIPI_IF:
			return &comipfb_if_mipi;
		default:
			return NULL;
	}
}


#define MIPI_TIME_X1000000 1000000

#define DSIH_PIXEL_TOLERANCE	2
#define DSIH_FIFO_ACTIVE_WAIT	2000	/*20us * 2000 = 40ms, cross two frame time*/
#define PRECISION_FACTOR 		1000
#define NULL_PACKET_OVERHEAD 	6
#define MAX_NULL_SIZE			1023
#define MAX_RD_COUNT			1000	/*prevent dead loop*/

#if defined(CONFIG_CPU_LC1813)
/* Reference clock frequency divided by Input Frequency Division Ratio LIMITS */
#define DPHY_DIV_UPPER_LIMIT	800000
#define DPHY_DIV_LOWER_LIMIT	1000
#define MIN_OUTPUT_FREQ			80
#elif defined(CONFIG_CPU_LC1860)
/* Reference clock frequency divided by Input Frequency Division Ratio LIMITS */
#define DPHY_DIV_UPPER_LIMIT	40000
#define DPHY_DIV_LOWER_LIMIT	5000
#define MIN_OUTPUT_FREQ			80
#endif
/**
 * Color coding type (depth and pixel configuration)
 */
typedef enum {
	COLOR_CODE_16BIT_CONFIG1 = 0,	//PACKET RGB565
	COLOR_CODE_16BIT_CONFIG2,		//UNPACKET RGB565
	COLOR_CODE_16BIT_CONFIG3,		//UNPACKET RGB565
	COLOR_CODE_18BIT_CONFIG1,		//PACKET RGB666
	COLOR_CODE_18BIT_CONFIG2,		//UNPACKET RGB666
	COLOR_CODE_24BIT,				//PACKET RGB888
	COLOR_CODE_MAX,
}dsih_color_coding_t;

#endif

static uint8_t oled_cmds_init[][ROW_LINE] = {
	{0x00, DCS_CMD, LW_PACK, 0x08, 0x06, 0x00, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x00}
	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb0, 0x00, 0x10, 0x10}
	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xba, 0x60}
	,{0x00, DCS_CMD, LW_PACK, 0x0a, 0x08, 0x00, 0xbb, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77}
	,{0x00, DCS_CMD, LW_PACK, 0x08, 0x06, 0x00, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x02}
	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xca, 0x04}
	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xe1, 0x00}
	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xe2, 0x0a}
	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xe3, 0x40}
	,{0x00, DCS_CMD, LW_PACK, 0x07, 0x05, 0x00, 0xe7, 0x00, 0x00, 0x00, 0x00}
	,{0x00, DCS_CMD, LW_PACK, 0x0b, 0x09, 0x00, 0xed, 0x48, 0x00, 0xe0, 0x13, 0x08, 0x00, 0x91, 0x08}
	,{0x00, DCS_CMD, LW_PACK, 0x09, 0x07, 0x00, 0xfd, 0x00, 0x08, 0x1c, 0x00, 0x00, 0x01}
	,{0x00, DCS_CMD, LW_PACK, 0x0e, 0x0c, 0x00, 0xc3, 0x11, 0x24, 0x04, 0x0a, 0x02, 0x04, 0x00, 0x1c, 0x10, 0xf0, 0x00}
	,{0x00, DCS_CMD, LW_PACK, 0x08, 0x06, 0x00, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x03}
	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xe0, 0x00}
	,{0x00, DCS_CMD, LW_PACK, 0x09, 0x07, 0x00, 0xf1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xf6, 0x08}

	,{0x00, DCS_CMD, LW_PACK, 0x08, 0x06, 0x00, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x05}
	,{0x00, DCS_CMD, LW_PACK, 0x05, 0x03, 0x00, 0xc4, 0x00, 0x14}
	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xc9, 0x04}

	,{0x00, DCS_CMD, LW_PACK, 0x08, 0x06, 0x00, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x01}
	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb0, 0x06, 0x06, 0x06}
	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb1, 0x14, 0x14, 0x14}
	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb2, 0x00, 0x00, 0x00}
	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb4, 0x66, 0x66, 0x66}
	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb5, 0x46, 0x46, 0x46}
	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb6, 0x54, 0x54, 0x54}
	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb7, 0x24, 0x24, 0x24}
	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb9, 0x04, 0x04, 0x04}
	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xba, 0x14, 0x14, 0x14}
	,{0x80, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xbe, 0x22, 0x38, 0x78}
	,{0x80, DCS_CMD, SW_PACK2, 0x02, 0x35, 0x00}

	//add by yuanzheng
	,{0x80, DCS_CMD, SW_PACK1, 0x01, 0x11}
	,{0x80, DCS_CMD, SW_PACK1, 0x01, 0x29}
};

static uint8_t oled_cmds_suspend[][ROW_LINE] = {
	{0x0A, DCS_CMD, SW_PACK1, 0x01, 0x28},
	{0x78, DCS_CMD, SW_PACK1, 0x01, 0x10},
};

static uint8_t oled_cmds_resume[][ROW_LINE] = {
//	{0x00, DCS_CMD, LW_PACK, 0x08, 0x06, 0x00, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x00}
//	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb0, 0x00, 0x10, 0x10}
//	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xba, 0x60}
//	,{0x00, DCS_CMD, LW_PACK, 0x0a, 0x08, 0x00, 0xbb, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77}
//	,{0x00, DCS_CMD, LW_PACK, 0x08, 0x06, 0x00, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x02}
//	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xca, 0x04}
//	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xe1, 0x00}
//	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xe2, 0x0a}
//	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xe3, 0x40}
//	,{0x00, DCS_CMD, LW_PACK, 0x07, 0x05, 0x00, 0xe7, 0x00, 0x00, 0x00, 0x00}
//	,{0x00, DCS_CMD, LW_PACK, 0x0b, 0x09, 0x00, 0xed, 0x48, 0x00, 0xe0, 0x13, 0x08, 0x00, 0x91, 0x08}
//	,{0x00, DCS_CMD, LW_PACK, 0x09, 0x07, 0x00, 0xfd, 0x00, 0x08, 0x1c, 0x00, 0x00, 0x01}
//	,{0x00, DCS_CMD, LW_PACK, 0x0e, 0x0c, 0x00, 0xc3, 0x11, 0x24, 0x04, 0x0a, 0x02, 0x04, 0x00, 0x1c, 0x10, 0xf0, 0x00}
//	,{0x00, DCS_CMD, LW_PACK, 0x08, 0x06, 0x00, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x03}
//	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xe0, 0x00}
//	,{0x00, DCS_CMD, LW_PACK, 0x09, 0x07, 0x00, 0xf1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
//	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xf6, 0x08}
//
//	,{0x00, DCS_CMD, LW_PACK, 0x08, 0x06, 0x00, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x05}
//	,{0x00, DCS_CMD, LW_PACK, 0x05, 0x03, 0x00, 0xc4, 0x00, 0x14}
//	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0xc9, 0x04}
//
//	,{0x00, DCS_CMD, LW_PACK, 0x08, 0x06, 0x00, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x01}
//	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb0, 0x06, 0x06, 0x06}
//	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb1, 0x14, 0x14, 0x14}
//	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb2, 0x00, 0x00, 0x00}
//	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb4, 0x66, 0x66, 0x66}
//	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb5, 0x46, 0x46, 0x46}
//	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb6, 0x54, 0x54, 0x54}
//	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb7, 0x24, 0x24, 0x24}
//	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xb9, 0x04, 0x04, 0x04}
//	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xba, 0x14, 0x14, 0x14}
//	,{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xbe, 0x22, 0x38, 0x78}
//	,{0x00, DCS_CMD, SW_PACK2, 0x02, 0x35, 0x00}

	//add by yuanzheng
	{0x80, DCS_CMD, SW_PACK1, 0x01, 0x11}
	,{0x00, DCS_CMD, SW_PACK1, 0x01, 0x29}
};

static uint8_t oled_pre_read_id[][ROW_LINE] = {
	//{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xB9, 0xFF, 0x83, 0x94},
	//{0x00, DCS_CMD, LW_PACK, 0x10, 0x0E, 0x00, 0xBA, 0x33, 0x83, 0xA8, 0xAD, 0xB6, 0x00, 0x00, 0x40, 0x10, 0xFF, 0x0F, 0x00, 0x80},
	{0x00, DCS_CMD, LW_PACK, 0x06, 0x04, 0x00, 0xB9, 0xFF, 0x83, 0x94},
	{0x00, DCS_CMD, SW_PACK2, 0x02, 0xFE, 0xF4},
	{0x00, DCS_CMD, SW_PACK1, 0x01, 0xFF},
};


#if 0


static struct common_id_info oled_common_id_info[] = {
	{DCS_CMD, {0x83, 0x94, 0x1A}, 3, 0x04},
};
static struct common_id_info oled_common_esd_info[] = {
	{DCS_CMD, {0x1C}, 1, 0x0A},
};

static int oled_auo_rm69052_power(struct comipfb_info *fbi, int onoff)
{
//	int gpio_rst = fbi->pdata->gpio_rst;
//
//	if (gpio_rst < 0) {
//		pr_err("no reset pin found\n");
//		return -ENXIO;
//	}
//
//	gpio_request(gpio_rst, "OLED Reset");

//	if (onoff) {
//		//change init code delay
//		gpio_direction_output(gpio_rst, 0);
//		pmic_voltage_set(PMIC_POWER_LCD_CORE, 0, PMIC_POWER_VOLTAGE_ENABLE);
//		local_delay_ms(50);
//		pmic_voltage_set(PMIC_POWER_LCD_IO, 0, PMIC_POWER_VOLTAGE_ENABLE);
//		local_delay_ms(100);
//		gpio_direction_output(gpio_rst, 1);
//		local_delay_ms(180);
//	} else {
//		gpio_direction_output(gpio_rst, 0);
//		pmic_voltage_set(PMIC_POWER_LCD_CORE, 0, PMIC_POWER_VOLTAGE_DISABLE);
//		pmic_voltage_set(PMIC_POWER_LCD_IO, 0, PMIC_POWER_VOLTAGE_DISABLE);
//		local_delay_ms(10);
//	}
//
//	gpio_free(gpio_rst);

	return 0;
}

static int oled_auo_rm69052_reset(struct comipfb_info *fbi)
{
//	int gpio_rst = fbi->pdata->gpio_rst;
//
//	if (gpio_rst >= 0) {
//		gpio_request(gpio_rst, "OLED Reset");
//		gpio_direction_output(gpio_rst, 1);
//		local_delay_ms(10);
//		gpio_direction_output(gpio_rst, 0);
//		local_delay_ms(10);
//		gpio_direction_output(gpio_rst, 1);
//		local_delay_ms(180);
//		gpio_free(gpio_rst);
//	}
	return 0;
}

static int oled_auo_rm69052_suspend(struct comipfb_info *fbi)
{
	int ret=0;
//	struct comipfb_dev_timing_mipi *mipi;
//
//	mipi = &(fbi->cdev->timing.mipi);
//
//	if (mipi->display_mode == MIPI_VIDEO_MODE) {
//		mipi_dsih_hal_mode_config(fbi, 1);
//	}
//
//	comipfb_if_mipi_dev_cmds(fbi, &fbi->cdev->cmds_suspend);
//
//	local_delay_ms(20);
//	mipi_dsih_dphy_enable_hs_clk(fbi, 0);
//
//	mipi_dsih_dphy_reset(fbi, 0);
//	mipi_dsih_dphy_shutdown(fbi, 0);

	return ret;
}

static int oled_auo_rm69052_resume(struct comipfb_info *fbi)
{
	int ret=0;
//	struct comipfb_dev_timing_mipi *mipi;
//
//	mipi = &(fbi->cdev->timing.mipi);
//
//	mipi_dsih_dphy_shutdown(fbi, 1);
//	mipi_dsih_dphy_reset(fbi, 1);
//
//	//if (fbi->cdev->reset)
//	//	fbi->cdev->reset(fbi);
//
//	if (!(fbi->pdata->flags & COMIPFB_SLEEP_POWEROFF))
//		ret = comipfb_if_mipi_dev_cmds(fbi, &fbi->cdev->cmds_resume);
//	else
//		ret = comipfb_if_mipi_dev_cmds(fbi, &fbi->cdev->cmds_init);
//
//	local_delay_ms(20);
//	if (mipi->display_mode == MIPI_VIDEO_MODE) {
//		mipi_dsih_hal_mode_config(fbi, 0);
//	}
//	mipi_dsih_dphy_enable_hs_clk(fbi, 1);

	return ret;
}

/*frame rate 57*/
struct comipfb_dev oled_auo_rm69052_dev = {
	.name = "oled_auo_rm69052",
	.interface_info = COMIPFB_MIPI_IF,
	.lcd_id = OLED_ID_AUO_RM69052,	//OLED_ID_AUO_RM69052
	.refresh_en = 1,
	.bpp = 32,
	.xres = 720,
	.yres = 1280,
	.flags = 0,
	.pclk = 60000000,
	.timing = {
		.mipi = {
			.hs_freq = 52000,
			.lp_freq = 13000,		//KHZ
			.no_lanes = 4,
			.display_mode = MIPI_VIDEO_MODE,
			.im_pin_val = 1,
			.color_mode = {
				.color_bits = COLOR_CODE_24BIT,
			},
			.videomode_info = {
				.hsync = 5,
				.hbp = 11,
				.hfp = 16,
				.vsync = 5,
				.vbp = 11,
				.vfp = 16,
				.sync_pol = COMIPFB_VSYNC_HIGH_ACT,
				.lp_cmd_en = 1,
				.lp_hfp_en = 1,
				.lp_hbp_en = 1,
				.lp_vact_en = 1,
				.lp_vfp_en = 1,
				.lp_vbp_en = 1,
				.lp_vsa_en = 1,
				.mipi_trans_type = VIDEO_BURST_WITH_SYNC_PULSES,	//VIDEO_BURST_WITH_SYNC_PULSES,VIDEO_NON_BURST_WITH_SYNC_EVENTS,VIDEO_NON_BURST_WITH_SYNC_PULSES
			},
			.phytime_info = {
				.clk_tprepare = 3, //HSBYTECLK
			},
			.teinfo = {
				.te_source = 1, //external signal
				.te_trigger_mode = 0,
				.te_en = 0,
				.te_sync_en = 0,
			},
			.ext_info = {
				.eotp_tx_en = 0,
			},
		},
	},
//	.panel_id_info = {
//		.id_info = oled_common_id_info,
//		.num_id_info = 1,
//		.prepare_cmd = {ARRAY_AND_SIZE(oled_pre_read_id)},
//	},
	.cmds_init = {ARRAY_AND_SIZE(oled_cmds_init)},
	.cmds_suspend = {ARRAY_AND_SIZE(oled_cmds_suspend)},
	.cmds_resume = {ARRAY_AND_SIZE(oled_cmds_resume)},
	.power = oled_auo_rm69052_power,
	.reset = oled_auo_rm69052_reset,
	.suspend = oled_auo_rm69052_suspend,
	.resume = oled_auo_rm69052_resume,
//#ifdef CONFIG_FB_COMIP_ESD
//	.esd_id_info = {
//		.id_info = oled_common_esd_info,
//		.num_id_info = 1,
//	},
//#endif
};
//
//static int __init oled_auo_rm69052_init(void)
//{
//	return comipfb_dev_register(&oled_auo_rm69052_dev);
//}

#endif

void tc358768_deinitialize(void)
{
	struct tc358768_drv_data * ddata = & dev0;

	tc358768_power_off(ddata);
	tc358768_sw_reset(ddata);
	HARDWARE_VODEO_DEINIT();
	local_delay_ms(5);
}

void tc358768_refclk(struct tc358768_drv_data * ddata, const videomode_t * vdmode)
{
	//dev0.refclk = hardware_get_dotclock(display_getdotclock(vdmode)) / 4;
	dev0.refclk = 25000000uL;

	//timings0.pixelclock = hardware_get_dotclock(LTDC_DOTCLK);

}

static void tpteinit(void)
{
	const portholder_t TE = (1uL << 7);	// PC7 (TE) - panel pin 29 Sync signal from driver IC
	const portholder_t OTP_PWR = (1uL << 7);	// PD7 (CTRL - OTP_PWR) - panel pin 30

//#if LCDMODE_H497TLB01P4
//	arm_hardware_pioa_inputs(TSC_INT);
//	arm_hardware_pioa_onchangeinterrupt(TSC_INT, TSC_INT, TSC_INT, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM);
//	arm_hardware_pioc_inputs(TE);
//	arm_hardware_piod_outputs(OTP_PWR, 1 * OTP_PWR);
//#elif LCDMODE_TV101WXM && 0
//
//	tim3_initialize(1000);
//
//	// PC7: TIM3_CH2, AF2
//	arm_hardware_pioc_altfn2(TE, 2);
//	//arm_hardware_pioc_outputs(TE, 0 * TE);	// Brightness: 0 - full, 1: off
//	arm_hardware_piod_outputs(OTP_PWR, 0 * OTP_PWR);	// 0: panel power ON
//#elif LCDMODE_TV101WXM
//	arm_hardware_pioc_outputs(TE, 0 * TE);	// Brightness: 0 - full, 1: off
//	arm_hardware_piod_outputs(OTP_PWR, 0 * OTP_PWR);	// 0: panel power ON
//#endif
}

static void tptepoweron(int state)
{
	const portholder_t TE = (1uL << 7);	// PC7 (TE) - panel pin 29 Sync signal from driver IC
	const portholder_t OTP_PWR = (1uL << 7);	// PD7 (CTRL - OTP_PWR) - panel pin 30
//#if LCDMODE_TV101WXM && 0
//	if (state)
//	{
//		// PC7: TIM3_CH2, AF2
//		arm_hardware_pioc_altfn2(TE, 2);
//	}
//	else
//	{
//		arm_hardware_pioc_outputs(TE, ! state * TE);	// Brightness: 0 - full, 1: off
//	}
//	arm_hardware_piod_outputs(OTP_PWR, ! state * OTP_PWR);	// 0: panel power ON
//	local_delay_ms(50);
//#elif LCDMODE_TV101WXM
//	arm_hardware_pioc_outputs(TE, ! state * TE);	// Brightness: 0 - full, 1: off
//	arm_hardware_piod_outputs(OTP_PWR, ! state * OTP_PWR);	// 0: panel power ON
//	local_delay_ms(50);
//#endif
}

void tc358768_initialize(const videomode_t * vdmode)
{
	struct tc358768_drv_data * ddata = & dev0;

	HARDWARE_TC358768_POWERON(1);

	tpteinit();
	tptepoweron(1);
	//stpmic1_dump_regulators();
	// See also:
	// https://github.com/bbelos/rk3188-kernel/blob/master/drivers/video/rockchip/transmitter/tc358768.c
	// https://coral.googlesource.com/linux-imx/+/refs/heads/alpha/arch/arm64/boot/dts/freescale/fsl-imx8mq-evk-dcss-rm67191.dts
	// https://developer.toradex.com/knowledge-base/display-output-resolution-and-timings-linux
	// https://code.woboq.org/linux/linux/Documentation/devicetree/bindings/display/panel/samsung,s6e8aa0.txt.html
	// active low
	const portholder_t RESET = (1uL << 1);	// PD1 = RESX_18 - pin  28
	arm_hardware_piod_outputs(RESET, 0 * RESET);
	local_delay_ms(5);
	arm_hardware_piod_outputs(RESET, 1 * RESET);



	// TC358778XBG conrol
	//	x-gpios = <&gpioa 10 GPIO_ACTIVE_HIGH>; /* Video_RST */
	//	x-gpios = <&gpiof 14 GPIO_ACTIVE_HIGH>; /* Video_MODE: 0: test, 1: normal */
	const portholder_t Video_RST = (1uL << 10);	// PA10
	const portholder_t Video_MODE = (1uL << 14);	// PF14: Video_MODE: 0: test, 1: normal

	arm_hardware_piof_outputs(Video_MODE, Video_MODE);
	arm_hardware_pioa_outputs(Video_RST, 0 * Video_RST);
	local_delay_ms(5);
	arm_hardware_pioa_outputs(Video_RST, 1 * Video_RST);
	//PRINTF("TC358778XBG reset off\n");

	PRINTF("TC358778XBG: Chip and Revision ID=0x%04X (expected 0x4401)\n", tc358768_rd_reg_16or32bits(TC358768_CHIPID));

	local_delay_ms(30);

//
	// addr 0E: ID=02000144
	// TC358778XBG
#if 0
	// Reset
	tc358768_write(ddata, TC358768_SYSCTL, 0x001);
	tc358768_write(ddata, TC358768_SYSCTL, 0x000);

	PRINTF("TC358778XBG: Chip and Revision ID=0x%04lX (expected 0x4401)\n", tc358768_rd_reg_16or32bits(TC358768_CHIPID));
	PRINTF("TC358778XBG: System Control Register=%08lX\n", tc358768_rd_reg_16or32bits(TC358768_SYSCTL));
	PRINTF("TC358778XBG: Input Control Register=%08lX\n", tc358768_rd_reg_16or32bits(TC358768_CONFCTL));
	PRINTF("TC358778XBG: Data Format Control Register=%08lX\n", tc358768_rd_reg_16or32bits(TC358768_DATAFMT));
	tc358768_write(ddata, TC358768_DATAFMT, 0x0300);
	local_delay_ms(100);
	PRINTF("TC358778XBG: Data Format Control Register=%08lX\n", tc358768_rd_reg_16or32bits(TC358768_DATAFMT));

	PRINTF("TC358778XBG: PLL Control Register 0=%08lX\n", tc358768_rd_reg_16or32bits(TC358768_PLLCTL0));

	tc358768_write(ddata, TC358768_DSI_VSW, VSYNC);
	tc358768_write(ddata, TC358768_DSI_VBPR, VBP);
	tc358768_write(ddata, TC358768_DSI_VACT, HEIGHT);

	tc358768_write(ddata, TC358768_DSI_HSW, HSYNC);
	tc358768_write(ddata, TC358768_DSI_HBPR, HBP);
	tc358768_write(ddata, TC358768_DSI_HACT, WIDTH);

	PRINTF("TC358778XBG: vact=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_VACT));
	PRINTF("TC358778XBG: hact=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_HACT));
#endif

	tc358768_refclk(ddata, vdmode);
	tc358768_calc_pll(ddata, vdmode);

	tc358768_power_off(ddata);

//	unsigned w = setw(900);
//	unsigned q = setq(3000);

	tc358768_power_on(ddata, vdmode);
//
//	PRINTF("TC358778XBG: Data Format Control Register=%08lX\n", tc358768_rd_reg_16or32bits(TC358768_DATAFMT));
//
//	PRINTF("TC358778XBG: Chip and Revision ID=%04lX\n", tc358768_rd_reg_16or32bits(TC358768_CHIPID));
//
//	PRINTF("TC358778XBG: TC358768_DSI_VSW=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_VSW));
//	PRINTF("TC358778XBG: TC358768_DSI_VBPR=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_VBPR));
//	PRINTF("TC358778XBG: TC358768_DSI_VACT=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_VACT));
//	PRINTF("TC358778XBG: TC358768_DSI_HSW=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_HSW));
//	PRINTF("TC358778XBG: TC358768_DSI_HBPR=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_HBPR));
//	PRINTF("TC358778XBG: TC358768_DSI_HACT=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_HACT));

//	tc358768_dump(ddata);

}

void tc358768_wakeup(const videomode_t * vdmode)
{
	struct tc358768_drv_data * ddata = & dev0;

	HARDWARE_TC358768_POWERON(1);
	//stpmic1_dump_regulators();
	// See also:
	// https://github.com/bbelos/rk3188-kernel/blob/master/drivers/video/rockchip/transmitter/tc358768.c
	// https://coral.googlesource.com/linux-imx/+/refs/heads/alpha/arch/arm64/boot/dts/freescale/fsl-imx8mq-evk-dcss-rm67191.dts
	// https://developer.toradex.com/knowledge-base/display-output-resolution-and-timings-linux
	// https://code.woboq.org/linux/linux/Documentation/devicetree/bindings/display/panel/samsung,s6e8aa0.txt.html

	HARDWARE_TP_INIT();

	HARDWARE_VIDEO_INIT();

	//PRINTF("TC358778XBG reset off\n");

	local_delay_ms(30);

//
	// addr 0E: ID=02000144
	// TC358778XBG
#if 0
	// Reset
	tc358768_write(ddata, TC358768_SYSCTL, 0x001);
	tc358768_write(ddata, TC358768_SYSCTL, 0x000);

	PRINTF("TC358778XBG: Chip and Revision ID=%08lX\n", tc358768_rd_reg_16or32bits(TC358768_CHIPID));
	PRINTF("TC358778XBG: System Control Register=%08lX\n", tc358768_rd_reg_16or32bits(TC358768_SYSCTL));
	PRINTF("TC358778XBG: Input Control Register=%08lX\n", tc358768_rd_reg_16or32bits(TC358768_CONFCTL));
	PRINTF("TC358778XBG: Data Format Control Register=%08lX\n", tc358768_rd_reg_16or32bits(TC358768_DATAFMT));
	tc358768_write(ddata, TC358768_DATAFMT, 0x0300);
	local_delay_ms(100);
	PRINTF("TC358778XBG: Data Format Control Register=%08lX\n", tc358768_rd_reg_16or32bits(TC358768_DATAFMT));

	PRINTF("TC358778XBG: PLL Control Register 0=%08lX\n", tc358768_rd_reg_16or32bits(TC358768_PLLCTL0));

	tc358768_write(ddata, TC358768_DSI_VSW, VSYNC);
	tc358768_write(ddata, TC358768_DSI_VBPR, VBP);
	tc358768_write(ddata, TC358768_DSI_VACT, HEIGHT);

	tc358768_write(ddata, TC358768_DSI_HSW, HSYNC);
	tc358768_write(ddata, TC358768_DSI_HBPR, HBP);
	tc358768_write(ddata, TC358768_DSI_HACT, WIDTH);

	PRINTF("TC358778XBG: vact=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_VACT));
	PRINTF("TC358778XBG: hact=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_HACT));
#endif

	tc358768_refclk(ddata, vdmode);
	tc358768_calc_pll(ddata, vdmode);

	tc358768_power_off(ddata);

//	unsigned w = setw(900);
//	unsigned q = setq(3000);

	tc358768_power_on(ddata, vdmode);
//
//	PRINTF("TC358778XBG: Data Format Control Register=%08lX\n", tc358768_rd_reg_16or32bits(TC358768_DATAFMT));
//
//	PRINTF("TC358778XBG: Chip and Revision ID=%04lX\n", tc358768_rd_reg_16or32bits(TC358768_CHIPID));
//
//	PRINTF("TC358778XBG: TC358768_DSI_VSW=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_VSW));
//	PRINTF("TC358778XBG: TC358768_DSI_VBPR=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_VBPR));
//	PRINTF("TC358778XBG: TC358768_DSI_VACT=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_VACT));
//	PRINTF("TC358778XBG: TC358768_DSI_HSW=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_HSW));
//	PRINTF("TC358778XBG: TC358768_DSI_HBPR=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_HBPR));
//	PRINTF("TC358778XBG: TC358768_DSI_HACT=%ld\n", tc358768_rd_reg_16or32bits(TC358768_DSI_HACT));

}

#if LCDMODE_H497TLB01P4

/*
 *
 *
static uint8_t bigon [] =
1, 5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x00,
2, 3, 0xB0, 0x00, 0x10, 0x10,
3, 1, 0xBA, 0x60,
4, 7, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
5, 8, 0xC0, 0xC0, 0x04, 0x00, 0x20, 0x02, 0xE4, 0xE1, 0xC0,
6, 8, 0xC1, 0xC0, 0x04, 0x00, 0x20, 0x04, 0xE4, 0xE1, 0xC0,
7, 5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x02,
8, 5, 0xEA, 0x7F, 0x20, 0x00,, 0x00, 0x00,
9, 1, 0xCA, 0x04,
10, 1, 0xE1, 0x00,
11, 1, 0xE2, 0x0A,
12, 1, 0xE3, 0x40,
13, 4, 0xE7, 0x00, 0x00, 0x00, 0x00,
14, 8, 0xED, 0x48, 0x00, 0xE0, 0x13, 0x08, 0x00, 0x91, 0x08,
15, 6, 0xFD, 0x00, 0x08, 0x1C, 0x00, 0x00, 0x01,
16, 11, 0xC3, 0x11, 0x24, 0x04, 0x0A, 0x02, 0x04, 0x00, 0x1C, 0x10, 0xF0, 0x00
17, 5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x03,
18, 1, 0xE0, 0x00,
19, 6, 0xF1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15,
20, 1, 0xF6, 0x08,
21, 5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x05,
22, 5, 0xC3, 0x00, 0x10, 0x50, 0x50, 0x50,
23, 2, 0xC4, 0x00, 0x14,
24, 1, 0xC9, 0x04,
25, 5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x01,
26, 3, 0xB0, 0x06, 0x06, 0x06,
27, 3, 0xB1, 0x14, 0x14, 0x14,
28, 3, 0xB2, 0x00, 0x00, 0x00,
29, 3, 0xB4, 0x66, 0x66, 0x66,
30, 3, 0xB5, 0x44, 0x44, 0x44,
31, 3, 0xB6, 0x54, 0x54, 0x54,
32, 3, 0xB7, 0x24, 0x24, 0x24,
33, 3, 0xB9, 0x04, 0x04, 0x04,
34, 3, 0xBA, 0x14, 0x14, 0x14,
35, 3, 0xBE, 0x22, 0x38, 0x78,
36, 1, 0x35, 0x00,

 *
 */
static uint8_t bigon [] =
{
	5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x00,
	3, 0xB0, 0x00, 0x10, 0x10,
	1, 0xBA, 0x60,
	7, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	8, 0xC0, 0xC0, 0x04, 0x00, 0x20, 0x02, 0xE4, 0xE1, 0xC0,
	8, 0xC1, 0xC0, 0x04, 0x00, 0x20, 0x04, 0xE4, 0xE1, 0xC0,
	5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x02,
	5, 0xEA, 0x7F, 0x20, 0x00, 0x00, 0x00,
	1, 0xCA, 0x04,
	1, 0xE1, 0x00,
	1, 0xE2, 0x0A,
	1, 0xE3, 0x40,
	4, 0xE7, 0x00, 0x00, 0x00, 0x00,
	8, 0xED, 0x48, 0x00, 0xE0, 0x13, 0x08, 0x00, 0x91, 0x08,
	6, 0xFD, 0x00, 0x08, 0x1C, 0x00, 0x00, 0x01,
	11, 0xC3, 0x11, 0x24, 0x04, 0x0A, 0x02, 0x04, 0x00, 0x1C, 0x10, 0xF0, 0x00,
	5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x03,
	1, 0xE0, 0x00,
	6, 0xF1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15,
	1, 0xF6, 0x08,
	5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x05,
	5, 0xC3, 0x00, 0x10, 0x50, 0x50, 0x50,
	2, 0xC4, 0x00, 0x14,
	1, 0xC9, 0x04,
	5, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x01,
	3, 0xB0, 0x06, 0x06, 0x06,
	3, 0xB1, 0x14, 0x14, 0x14,
	3, 0xB2, 0x00, 0x00, 0x00,
	3, 0xB4, 0x66, 0x66, 0x66,
	3, 0xB5, 0x44, 0x44, 0x44,
	3, 0xB6, 0x54, 0x54, 0x54,
	3, 0xB7, 0x24, 0x24, 0x24,
	3, 0xB9, 0x04, 0x04, 0x04,
	3, 0xBA, 0x14, 0x14, 0x14,
	3, 0xBE, 0x22, 0x38, 0x78,
	1, 0x35, 0x00,

	0,
};
#endif

	static uint8_t sleepout [] = { 0x11, 0x00, };
	static uint8_t displon [] = { 0x29, 0x00, };
	//static uint8_t readid [] = { 0x29, 0x00, };
	static uint8_t sleep [] = { 0x10, 0x00, };
	static uint8_t disploff [] = { 0x28, 0x00, };

void panel_initialize(const videomode_t * vdmode)
{
	// RM69052 chip
	// also:
	// https://github.com/1667450061/bak/blob/d5c37db8a9254783755b7bfb6823f32474febff8/arch/arm/plat-lc/drivers/video/comipfb2/oled_auo_rm69052.c
	// https://discuss.96boards.org/t/display-problem-on-dsi-panel/1855
	tptepoweron(1);

	local_delay_ms(50);
#if LCDMODE_H497TLB01P4
	const uint8_t * pv = bigon;
	for (;;)
	{
		const uint8_t maxv = * pv;
		if (maxv == 0)
			break;
		mipi_dsi_send_dcs_packet(pv + 1, maxv + 1);
		//local_delay_ms(100);
		pv += maxv + 2;
		//PRINTF("e\n");
	}

#endif
	local_delay_ms(5);
	mipi_dsi_send_dcs_packet(sleepout, ARRAY_SIZE(sleepout));
	local_delay_ms(200);
	mipi_dsi_send_dcs_packet(displon, ARRAY_SIZE(displon));
	PRINTF("display on\n");
}

void panel_wakeup(void)
{
	tptepoweron(1);
	// RM69052 chip
	// also:
	// https://github.com/1667450061/bak/blob/d5c37db8a9254783755b7bfb6823f32474febff8/arch/arm/plat-lc/drivers/video/comipfb2/oled_auo_rm69052.c
	// https://discuss.96boards.org/t/display-problem-on-dsi-panel/1855
#if 0
	local_delay_ms(50);
	const uint8_t * pv = bigon;
	for (;;)
	{
		const uint8_t maxv = * pv;
		if (maxv == 0)
			break;
		mipi_dsi_send_dcs_packet(pv + 1, maxv + 1);
		//local_delay_ms(100);
		pv += maxv + 2;
		//PRINTF("e\n");
	}
#endif
	local_delay_ms(5);
	mipi_dsi_send_dcs_packet(sleepout, ARRAY_SIZE(sleepout));
	local_delay_ms(200);
	mipi_dsi_send_dcs_packet(displon, ARRAY_SIZE(displon));

	PRINTF("display on\n");
}

void panel_deinitialize(void)
{

	mipi_dsi_send_dcs_packet(disploff, ARRAY_SIZE(disploff));
	local_delay_ms(10);
	mipi_dsi_send_dcs_packet(sleep, ARRAY_SIZE(sleep));
	local_delay_ms(20);
	tptepoweron(0);
}


#endif /* LCDMODETX_TC358778XBG */

