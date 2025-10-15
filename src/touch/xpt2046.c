/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN

// XPT2046 Resistive touch screen controller SHENZHEN XPTEK TECHNOLOGY CO.,LTD http://www.xptek.com.cn
// TI TSC2046
// HR2046
// ADS7843
// SPI interface used

#include "hardware.h"
#include "board.h"
#include "formats.h"

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_XPT2046)

#include "spi.h"
#include "touch.h"

// При необходимости разместить в файле конфигурации платы.
//#define BOARD_TSC1_XMIRROR 1	// Зеркалируем тачскрин по горизонтали.
//#define BOARD_TSC1_YMIRROR 1	// Зеркалируем тачскрин по вертикали.

#define tscspeed SPIC_SPEED100k
#define tscmode SPIC_MODE0

#define XPT2046_SPOOLMS	20	// период опроса

#define XPT2046_DFR_MODE 	0x00
#define XPT2046_SER_MODE 	0x04
#define XPT2046_CONTROL  	0x80
#define XPT2046_A0			0x10
#define XPT2046_8BIT_MODE	0x08
#define XPT2046_PD0  		0x01	// full-power (PD0 = 1), not go into power-down (PD0 = 1) ???
#define XPT2046_PD1  		0x02
#define XPT2046_NOP  		0x00

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

// See https://github.com/ikeji/Ender3Firmware/blob/ef1f9d25eb2cd084ce929e1ad4163ef0a3e88142/Marlin/src/feature/touch/xpt2046.cpp
// https://github.com/Bodmer/TFT_Touch/blob/master/TFT_Touch.cpp
// https://github.com/MarlinFirmware/Marlin/blob/2.0.x/Marlin/src/lcd/touch/touch_buttons.cpp
// MKS Robin Mini/firmware/Marlin2.0-MKS-Robin_mini/Marlin/src/HAL/HAL_STM32F1/xpt2046.h
// https://github.com/d-qoi/TSC2046_kernel_driver/blob/master/TSC2046_driver.c
// https://github.com/dmquirozc/XPT2046_driver_STM32/tree/main
// https://github.com/vadrov/stm32-xpt2046-ili9341-dma-irq-spi-temperature-voltage

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
	enum { AVERAGE = 4 };
	enum { PDx = 1*XPT2046_PD1 | 1*XPT2046_PD0 };	// оба бита "1" - прерывания не формируются
	static const uint8_t txbuf [] =
	{
		XPT2046_CONTROL | PDx | XPT2046_Y, 0x00,	// ignored
		XPT2046_CONTROL | PDx | XPT2046_Y, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_Y, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_Y, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_Y, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_X, 0x00,	// ignored
		XPT2046_CONTROL | PDx | XPT2046_X, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_X, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_X, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_X, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_Z1, 0x00,	// ignored
		XPT2046_CONTROL | PDx | XPT2046_Z1, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_Z1, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_Z1, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_Z1, 0x00,
#if 0
		XPT2046_CONTROL | PDx | XPT2046_Z2, 0x00,	// ignored
		XPT2046_CONTROL | PDx | XPT2046_Z2, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_Z2, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_Z2, 0x00,
		XPT2046_CONTROL | PDx | XPT2046_Z2, 0x00,
#endif
		XPT2046_NOP,
	};
	uint8_t rxbuf [ARRAY_SIZE(txbuf)];

	prog_spi_exchange(target, tscspeed, tscmode, txbuf, rxbuf, ARRAY_SIZE(txbuf));
	//printhex((uintptr_t) 0, rxbuf, sizeof rxbuf);

	uint_fast16_t xv = 0, yv = 0, z1v = 0, z2v = 0;
	unsigned i, offs;
	offs = 1;

	offs += 2;	// skip dummy read
	for (i = 0; i < AVERAGE; ++ i, offs += 2)
	{
		yv += USBD_peek_u16_BE(rxbuf + offs) / 8;
	}
	offs += 2;	// skip dummy read
	for (i = 0; i < AVERAGE; ++ i, offs += 2)
	{
		xv += USBD_peek_u16_BE(rxbuf + offs) / 8;
	}
	offs += 2;	// skip dummy read
	for (i = 0; i < AVERAGE; ++ i, offs += 2)
	{
		z1v += USBD_peek_u16_BE(rxbuf + offs) / 8;
	}
#if 0
	offs += 2;	// skip dummy read
	for (i = 0; i < AVERAGE; ++ i, offs += 2)
	{
		z2v += USBD_peek_u16_BE(rxbuf + offs) / 8;
	}
#endif
	* x = xv / AVERAGE;
	* y = yv / AVERAGE;
	* z1 = z1v / AVERAGE;
	* z2 = z2v / AVERAGE;
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
static uint_fast8_t xpt2046_getxy_spi(uint_fast16_t * xr, uint_fast16_t * yr, uint_fast16_t * zr)
{
	const spitarget_t target = targettsc1;
	uint_fast16_t x, y, z1, z2;
	xpt2046_read4(target, & x, & y, & z1, & z2);

	* xr = x;
	* yr = y;
	//xpt2046_pressure(x, y, z1, z2);
	* zr = z1;
	//PRINTF("xpt2046 xpt2046_getxy_spi: x=%5u, y=%5u z1=%5u, z2=%5u\n", x, y, z1, z2);
	return !! x && !! y && (z1 > XPT2046_Z1_THRESHOLD);
}

void
xpt2406_interrupt_handler(void * ctx)
{
	const spitarget_t target = targettsc1;
	uint_fast16_t x, y, z1, z2;
	(void) ctx;
	xpt2046_read4(target, & x, & y, & z1, & z2);
	PRINTF("xpt2046 interrupt: x=%5u, y=%5u z1=%5u, z2=%5u\n", x, y, z1, z2);
}

static IRQLSPINLOCK_t tsclock = IRQLSPINLOCK_INIT;

static volatile uint_fast16_t gx, gy, gz;
static volatile uint_fast8_t gready;
static volatile unsigned press;


/* получение ненормальзованных координат нажатия */
uint_fast8_t board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr, uint_fast16_t * zr)
{
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& tsclock, & oldIrql, IRQL_SYSTEM);
	const uint_fast8_t f = gready;
	* xr = gx;
	* yr = gy;
	* zr = gz;
	gready = 0;
	IRQLSPIN_UNLOCK(& tsclock, oldIrql);
	return f;
}

static void xpt204_spool(void * ctx)
{
	enum { PRESSDELAY = 3 };
	uint_fast16_t x, y, z1;
	(void) ctx;
	const uint_fast8_t f = xpt2046_getxy_spi(& x, & y, & z1);
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& tsclock, & oldIrql, IRQL_SYSTEM);
	//PRINTF("%d: p=%u, r=%u\n", f, press, release);
	if (f)
	{
		if (press < PRESSDELAY)
			++ press;
		if (press == PRESSDELAY)
		{
			//TP();
			gx = x;
			gy = y;
			gz = z1;
			gready = 1;
			press = PRESSDELAY + 1;
		}
	}
	else
	{
		press = 0;
	}
	IRQLSPIN_UNLOCK(& tsclock, oldIrql);
}

void board_tsc_initialize(void)
{
	const spitarget_t target = targettsc1;
	IRQLSPINLOCK_INITIALIZE(& tsclock);
	//PRINTF("xpt2046_initialize:\n");
	{
		uint_fast16_t x, y, z1, z2;
		xpt2046_read4(target, & x, & y, & z1, & z2);
	}

#if 0
	for (;;)
	{
		uint_fast16_t x, y, z1, z2;
		xpt2046_read4(target, & x, & y, & z1, & z2);
		PRINTF("xpt2046: x=%5u, y=%5u z1=%5u, z2=%5u\n", x, y, z1, z2);
	}
#endif

	static ticker_t xpt204_ticker;
	ticker_initialize(& xpt204_ticker, NTICKS(XPT2046_SPOOLMS), xpt204_spool, NULL);
	ticker_add(& xpt204_ticker);

	//BOARD_XPT2046_INT_CONNECT();
	//PRINTF("xpt2046_initialize done.\n");
	board_tsc_calibration();	// использовать результаты калибровки
}

#if WITHTSC5PCALIBRATE
// результат калибровки
#if (DIM_X == 800) && (DIM_Y == 480)
static tPoint calpoints [TSCCALIBPOINTS] =
{
	{ 770, 878, }, /* point 0 */
	{ 3350, 881, }, /* point 1 */
	{ 771, 3407, }, /* point 2 */
	{ 3336, 3416, }, /* point 3 */
	{ 2058, 2185, }, /* point 4 */
};
#else
#error Provide calibration data
#endif
#endif /* WITHTSC5PCALIBRATE */

tPoint *
board_tsc_getcalpoints(void)
{
	return calpoints;
}

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_XPT2046) */
