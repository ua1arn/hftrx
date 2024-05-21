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

// При необходимости разместить в файле конфигурации платы.
//#define BOARD_TSC1_XMIRROR 1	// Зеркалируем тачскрин по горизонтали.
//#define BOARD_TSC1_YMIRROR 1	// Зеркалируем тачскрин по вертикали.

#define tscspeed SPIC_SPEED1M
#define tscmode SPIC_MODE0

#define XPT2046_DFR_MODE 	0x00
#define XPT2046_SER_MODE 	0x04
#define XPT2046_CONTROL  	0x80
#define XPT2046_A0			0x10
#define XPT2046_8BIT_MODE	0x08
#define XPT2046_PD0  		0x01	// full-power (PD0 = 1), not go into power-down (PD0 = 1) ???
#define XPT2046_PD1  		0x02

enum XPTCoordinate
{
	//XPT2046_X  = 1 * XPT2046_A0 | XPT2046_DFR_MODE,	// Длинная сторона на 320x240
	//XPT2046_Y  = 5 * XPT2046_A0 | XPT2046_DFR_MODE,	// Короткая сторона на 320x240
	XPT2046_Y  = 1 * XPT2046_A0 | XPT2046_DFR_MODE,
	XPT2046_X  = 5 * XPT2046_A0 | XPT2046_DFR_MODE,
	XPT2046_Z1 = 3 * XPT2046_A0 | XPT2046_DFR_MODE,
	XPT2046_Z2 = 4 * XPT2046_A0 | XPT2046_DFR_MODE,
	XPT2046_TEMP = 0 * XPT2046_A0 | XPT2046_SER_MODE,		// Термодачик
	XPT2046_TEMP_1 = 6 * XPT2046_A0 | XPT2046_SER_MODE,		// Термодачик (The second mode)
	XPT2046_TEMP_2 = 7 * XPT2046_A0 | XPT2046_SER_MODE		// Термодачик (The second mode)
};

#define XPT2046_Z1_THRESHOLD 100	// чувствительность к силе нажатия
#define XPT2046_Z2_THRESHOLD 4090
#define XPT2046_X_THRESHOLD 3900
#define XPT2046_Y_THRESHOLD 3900

// See https://github.com/ikeji/Ender3Firmware/blob/ef1f9d25eb2cd084ce929e1ad4163ef0a3e88142/Marlin/src/feature/touch/xpt2046.cpp
// https://github.com/Bodmer/TFT_Touch/blob/master/TFT_Touch.cpp
// https://github.com/MarlinFirmware/Marlin/blob/2.0.x/Marlin/src/lcd/touch/touch_buttons.cpp
// MKS Robin Mini/firmware/Marlin2.0-MKS-Robin_mini/Marlin/src/HAL/HAL_STM32F1/xpt2046.h

#if WITHSPIHW || WITHSPISW

// Read XPT2046 ADC
static uint_fast16_t
xpt2046_read(
	spitarget_t target,
	uint_fast8_t cmd
	)
{
	uint_fast16_t v0, v1, v2, v3;
	// сдвинуто, чтобы позиция временной диаграммы,
	// где формируется время выборки, не попадала на паузу между байтами.
	uint_fast32_t rv;

	enum { CMDPOS = 13 };
	enum { DATAPOS = 0 };

	cmd |= XPT2046_CONTROL;
	cmd |= XPT2046_PD0 | XPT2046_PD1;	// PD0=1 & PD1=1: Device is always powered. Reference is on and ADC is on.

#if WITHSPILOWSUPPORTT || 1
	// Работа совместно с фоновым обменом SPI по прерываниям

	uint8_t txbuf [4];
	uint8_t rxbuf [ARRAY_SIZE(txbuf)];

	USBD_poke_u32_BE(txbuf, (uint_fast32_t) cmd << CMDPOS);

	prog_spi_exchange(target, tscspeed, tscmode, txbuf, rxbuf, ARRAY_SIZE(txbuf));

	rv = USBD_peek_u32_BE(rxbuf);

#elif WITHSPI32BIT

	hardware_spi_connect_b32(tscspeed, tscmode);
	prog_select(target);

	hardware_spi_b32_p1((uint_fast32_t) cmd << CMDPOS);
	rv = hardware_spi_complete_b32();

	prog_unselect(target);
	hardware_spi_disconnect();


#elif WITHSPI16BIT

	hardware_spi_connect_b16(tscspeed, tscmode);
	prog_select(target);

	hardware_spi_b16_p1((uint_fast32_t) cmd << CMDPOS >> 16);
	v0 = hardware_spi_complete_b16();
	hardware_spi_b16_p1((uint_fast32_t) cmd << CMDPOS >> 0);
	v1 = hardware_spi_complete_b16();

	prog_unselect(target);
	hardware_spi_disconnect();

	rv = ((uint_fast32_t) v0 << 16) | v1;

#else

	spi_select2(target, tscmode, tscspeed);	// for 50 kS/S and 24 bit words

	v0 = spi_read_byte(target, (uint_fast32_t) cmd << CMDPOS >> 24);
	v1 = spi_read_byte(target, (uint_fast32_t) cmd << CMDPOS >> 16);
	v2 = spi_read_byte(target, (uint_fast32_t) cmd << CMDPOS >> 8);
	v3 = spi_read_byte(target, 0x00);

	spi_unselect(target);

	rv = ((uint_fast32_t) v0 << 24) | ((uint_fast32_t) v1 << 16) | ((uint_fast32_t) v2 << 8) | v3;

#endif

	return (rv >> DATAPOS) & 0x0FFF;	// 12 bit ADC
}

// Read XPT2046 ADC
static void
xpt2046_read4(
	spitarget_t target,
	uint_fast16_t * x,
	uint_fast16_t * y,
	uint_fast16_t * z
	)
{
	static const uint8_t txbuf [] =
	{
		XPT2046_CONTROL | XPT2046_Y, 0x00, 0x00,
		XPT2046_CONTROL | XPT2046_X, 0x00, 0x00,
		XPT2046_CONTROL | XPT2046_Z1, 0x00, 0x00,
	};
	uint8_t rxbuf [ARRAY_SIZE(txbuf)];

	prog_spi_exchange(target, tscspeed, tscmode, txbuf, rxbuf, ARRAY_SIZE(txbuf));
	//printhex((uintptr_t) rxbuf, rxbuf, sizeof rxbuf);

	* y = USBD_peek_u16_BE(rxbuf + 0);
	* x = USBD_peek_u16_BE(rxbuf + 3);
	* z = USBD_peek_u16_BE(rxbuf + 6);
}

#endif /* WITHSPIHW || WITHSPISW */

/* получение ненормальзованных координат нажатия */
uint_fast8_t xpt2046_getxy(uint_fast16_t * xr, uint_fast16_t * yr)
{
	const spitarget_t target = targettsc1;

	//uint_fast16_t x0 = xpt2046_read(target, XPT2046_X);
	uint_fast16_t x = xpt2046_read(target, XPT2046_X);
	//uint_fast16_t y0 = xpt2046_read(target, XPT2046_Y);
	uint_fast16_t y = xpt2046_read(target, XPT2046_Y);
	//uint_fast16_t z10 = xpt2046_read(target, XPT2046_Z1);
	uint_fast16_t z1 = xpt2046_read(target, XPT2046_Z1);
	//uint_fast16_t z20 = xpt2046_read(target, XPT2046_Z2);
	uint_fast16_t z2 = xpt2046_read(target, XPT2046_Z2);

	* xr = x;
	* yr = y;

	return x < XPT2046_X_THRESHOLD && y < XPT2046_Y_THRESHOLD && z1 > XPT2046_Z1_THRESHOLD && z2 < XPT2046_Z2_THRESHOLD;
}

void xpt2046_initialize(void)
{
	BOARD_XPT2046_INT_CONNECT();
	const spitarget_t target = targettsc1;
	const unsigned t = xpt2046_read(target, XPT2046_TEMP);
	PRINTF("xpt2046_initialize: t=%u\n", t);
#if 0
	for (;;)
	{
//		const unsigned t = xpt2046_read(target, XPT2046_TEMP);
//		const unsigned x = xpt2046_read(target, XPT2046_X);
//		const unsigned y = xpt2046_read(target, XPT2046_Y);
//		const unsigned z1 = xpt2046_read(target, XPT2046_Z1);

		unsigned x, y, z1;
		xpt2046_read4( & x, & y, & z1);
		//unsigned z2 = xpt2046_read(target, XPT2046_Z2);
		const int st = z1 > XPT2046_Z1_THRESHOLD;
		PRINTF("xpt2046_initialize: t=%-5u, x=%-5u, y=%-5u z1=%-5u, st=%d (int=%d)\n", t, x, y, z1, st, BOARD_XPT2046_INT_GET());
	}
#endif
	//PRINTF("xpt2046_initialize done.\n");
}

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_XPT2046) */
