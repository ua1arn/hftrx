/*
 * xpt2046.c
 *
 *  Created on: Jun 18, 2021
 *      Author: gena
 */


#include "hardware.h"
#include "board.h"
#include "formats.h"

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_XPT2046)

#include "spi.h"
#include "xpt2046.h"

// Resistive touch screen controller SHENZHEN XPTEK TECHNOLOGY CO.,LTD http://www.xptek.com.cn
// SPI interface used

// See https://github.com/ikeji/Ender3Firmware/blob/ef1f9d25eb2cd084ce929e1ad4163ef0a3e88142/Marlin/src/feature/touch/xpt2046.cpp
// https://github.com/Bodmer/TFT_Touch/blob/master/TFT_Touch.cpp

#define targettsc1 targetext1	/* PE8 ext1 on front panel */


#if WITHSPIHW || WITHSPISW

// Read XPT2046 ADC
static uint_fast32_t
xpt2046_read(
	spitarget_t target,
	uint_fast8_t cmd
	)
{
	uint_fast16_t v0, v1, v2, v3;
	// сдвинуто, чтобы позиция временной диаграммы,
	// где формируется время выборки, не попадала на паузу между байтами.
	uint_fast32_t rv;

	enum { CMDPOS = 16 };
	enum { DATAPOS = 0 };
	const spi_speeds_t tscspeed = SPIC_SPEED4M;
	const spi_modes_t tscmode = SPIC_MODE0;

	// PD0=1 & PD1=1: Device is always powered. Reference is on and ADC is on.
	cmd |= XPT2046_CONTROL | XPT2046_PD0 | XPT2046_PD1;

#if WITHSPI32BIT

	hardware_spi_connect_b32(tscspeed, tscmode);
	prog_select(target);

	hardware_spi_b32_p1((uint_fast32_t) cmd << CMDPOS);
	rv = hardware_spi_complete_b32();

	prog_unselect(target);
	hardware_spi_disconnect();


#elif WITHSPI16BIT

	hardware_spi_connect_b16(tscspeed, tscmode);
	prog_select(target);

	hardware_spi_b16_p1((uint_fast32_t) cmd1 << CMDPOS >> 16);
	v0 = hardware_spi_complete_b16();
	hardware_spi_b16_p1(0);
	v1 = hardware_spi_complete_b16();

	prog_unselect(target);
	hardware_spi_disconnect();

	rv = ((uint_fast32_t) v0 << 16) | v1;

#else

	spi_select2(target, tscmode, tscspeed);	// for 50 kS/S and 24 bit words

	v0 = spi_read_byte(target, (uint_fast32_t) cmd1 << CMDPOS >> 24);
	v1 = spi_read_byte(target, (uint_fast32_t) cmd1 << CMDPOS >> 16);
	v2 = spi_read_byte(target, (uint_fast32_t) cmd1 << CMDPOS >> 8);
	v3 = spi_read_byte(target, 0x00);

	spi_unselect(target);

	rv = ((uint_fast32_t) v0 << 24) | ((uint_fast32_t) v1 << 16) | ((uint_fast32_t) v2 << 8) | v3;

#endif

	return (rv >> DATAPOS);// & 0x0FFF;	// 12 bit ADC
}
#endif /* WITHSPIHW || WITHSPISW */

static uint_fast16_t
tcsnormalize(
		uint_fast16_t raw,
		uint_fast16_t rawmin,
		uint_fast16_t rawmax,
		uint_fast16_t range
		)
{
	if (rawmin < rawmax)
	{
		// Normal direction
		const uint_fast16_t distance = rawmax - rawmin;
		if (raw < rawmin)
			return 0;
		raw = raw - rawmin;
		if (raw > distance)
			return range;
		return (uint_fast32_t) raw * range / distance;
	}
	else
	{
		// reverse direction
		const uint_fast16_t distance = rawmin - rawmax;
		if (raw >= rawmin)
			return 0;
		raw = rawmin - raw;
		if (raw > distance)
			return range;
		return (uint_fast32_t) raw * range / distance;
	}
}

uint_fast8_t xpt2046_getxy(uint_fast16_t * xr, uint_fast16_t * yr)
{
	/* top left raw data values */
	static uint_fast16_t xrawmin = 70;
	static uint_fast16_t yrawmin = 3890;
	/* bottom right raw data values */
	static uint_fast16_t xrawmax = 3990;
	static uint_fast16_t yrawmax = 150;

	uint_fast16_t x, y;

	x = xpt2046_read(targettsc1, 0x00);
	y = xpt2046_read(targettsc1, 0x00);

	* xr = tcsnormalize(x, xrawmin, xrawmax, DIM_X - 1);
	* yr = tcsnormalize(y, yrawmin, yrawmax, DIM_Y - 1);

	return 0;
}

// MKS Robin Mini/firmware/Marlin2.0-MKS-Robin_mini/Marlin/src/HAL/HAL_STM32F1/xpt2046.h

void xpt2046_initialize(void)
{
	PRINTF("xpt2046_initialize start.\n");
	for (;;)
	{
		unsigned x = xpt2046_read(targettsc1, XPT2046_X | XPT2046_DFR_MODE);
		unsigned y = xpt2046_read(targettsc1, XPT2046_Y | XPT2046_DFR_MODE);
		unsigned z1 = xpt2046_read(targettsc1, XPT2046_Z1 | XPT2046_DFR_MODE);
		//unsigned z2 = xpt2046_read(targettsc1, XPT2046_Z2);
		int st = z1 > XPT2046_Z1_THRESHOLD;
		PRINTF("xpt2046: x=%08X, y=%08X z1=%08X, st=%d\n", x, y, z1, st);
	}
	PRINTF("xpt2046_initialize done.\n");
}

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_XPT2046) */
