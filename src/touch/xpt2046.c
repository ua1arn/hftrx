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

#define XPT2046_Z1_THRESHOLD 200	// чувствительность к силе нажатия
#define XPT2046_Z1_THRESHOLD2 1000	// чувствительность к силе нажатия
#define XPT2046_Z2_THRESHOLD 200	// чувствительность к силе нажатия

// See https://github.com/ikeji/Ender3Firmware/blob/ef1f9d25eb2cd084ce929e1ad4163ef0a3e88142/Marlin/src/feature/touch/xpt2046.cpp
// https://github.com/Bodmer/TFT_Touch/blob/master/TFT_Touch.cpp
// https://github.com/MarlinFirmware/Marlin/blob/2.0.x/Marlin/src/lcd/touch/touch_buttons.cpp
// MKS Robin Mini/firmware/Marlin2.0-MKS-Robin_mini/Marlin/src/HAL/HAL_STM32F1/xpt2046.h

#if WITHSPIHW || WITHSPISW

// Read XPT2046 ADC
static void
xpt2046_read4(
	spitarget_t target,
	uint_fast16_t * x,
	uint_fast16_t * y,
	uint_fast16_t * z1,
	uint_fast16_t * z2
	)
{
	enum { PDx = XPT2046_PD0 };	// Reference is off and ADC is on.
	static const uint8_t txbuf [] =
	{
		XPT2046_CONTROL | PDx | XPT2046_Y, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_Y, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_X, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_Z1, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_Z2, 0x00,

		0x00,
	};
	uint8_t rxbuf [ARRAY_SIZE(txbuf)];

	prog_spi_exchange(target, tscspeed, tscmode, txbuf, rxbuf, ARRAY_SIZE(txbuf));
	//printhex((uintptr_t) rxbuf, rxbuf, sizeof rxbuf);
	unsigned i;
	unsigned yv = 0;
	unsigned xv = 0;
	unsigned z1v = 0;
	unsigned z2v = 0;
	for (i = 0; i < 1; ++ i)
	{
		yv += USBD_peek_u16_BE(rxbuf + (i * 8) + 3) / 8;
		xv += USBD_peek_u16_BE(rxbuf + (i * 8) + 5) / 8;
		z1v += USBD_peek_u16_BE(rxbuf + (i * 8) + 7) / 8;
		z2v += USBD_peek_u16_BE(rxbuf + (i * 8) + 9) / 8;
	}
	* y = yv / i;
	* x = xv / i;
	* z1 = z1v / i;
	* z2 = z2v / i;
}

#endif /* WITHSPIHW || WITHSPISW */

static unsigned
xpt2046_pressure(
	uint_fast16_t x,
	uint_fast16_t y,
	uint_fast16_t z1,
	uint_fast16_t z2
	)
{
	return z1;
}

/* получение ненормальзованных координат нажатия */
uint_fast8_t xpt2046_getxy(uint_fast16_t * xr, uint_fast16_t * yr)
{
	const spitarget_t target = targettsc1;
	uint_fast16_t x, y, z1, z2;
	xpt2046_read4(target, & x, & y, & z1, & z2);

	* xr = x;
	* yr = y;
	xpt2046_pressure(x, y, z1, z2);
	return (z1 > XPT2046_Z1_THRESHOLD2) || (z1 > XPT2046_Z1_THRESHOLD && (4095 - z2) > XPT2046_Z2_THRESHOLD);
}

void xpt2046_initialize(void)
{
	//BOARD_XPT2046_INT_CONNECT();
	const spitarget_t target = targettsc1;
	PRINTF("xpt2046_initialize:n");
#if 0
	for (;;)
	{
		unsigned x, y, z1, z2;
		xpt2046_read4(target, & x, & y, & z1, & z2);
		PRINTF("xpt2046: x=%5u, y=%5u z1=%5u, z2=%5u\n", x, y, z1, z2);
	}
#endif
	//PRINTF("xpt2046_initialize done.\n");
}

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_XPT2046) */
