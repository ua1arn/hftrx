/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "src/display/display.h"
#include "display2.h"
#include "formats.h"
#include "touch.h"
#include "gpio.h"

//#define WITHTSC5PCALIBRATE 1	/* Калибровка по пяти точкам */

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

#if WITHTSC5PCALIBRATE

// функции калибровки взяты тут:
// https://github.com/vadrov/stm32-xpt2046-ili9341-dma-irq-spi-temperature-voltage
/*
 * Copyright (C) 2019, VadRov, all right reserved.
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */

/*
 * Расчет коэффициентов для преобразования координат тачскрина в дисплейные координаты
 */
static void CoefCalc(const tPoint *p_d, const tPoint *p_t, tCoef *coef, int all_points)
{
	int i;
	int_fast64_t a = 0, b = 0, c = 0, d = 0, e = 0;
	int_fast64_t X1 = 0, X2 = 0, X3 = 0, Y1 = 0, Y2 = 0, Y3 = 0;
	for (i = 0; i < all_points; i ++)
	{
		a += p_t[i].x * p_t[i].x;
		b += p_t[i].y * p_t[i].y;
		c += p_t[i].x * p_t[i].y;
		d += p_t[i].x;
		e += p_t[i].y;
		X1 += p_t[i].x * p_d[i].x;
		X2 += p_t[i].y * p_d[i].x;
		X3 += p_d[i].x;
		Y1 += p_t[i].x * p_d[i].y;
		Y2 += p_t[i].y * p_d[i].y;
		Y3 += p_d[i].y;
	}
	coef->D = all_points * (a * b - c * c) + 2 * c *  d * e - a * e * e - b * d * d;
	coef->Dx1 = all_points * (X1 * b - X2 * c) + e * (X2 * d - X1 * e) + X3 * (c * e - b * d);
	coef->Dx2 = all_points * (X2 * a - X1 * c) + d * (X1 * e - X2 * d) + X3 * (c * d - a * e);
	coef->Dx3 = X3 * (a * b - c * c) + X1 * (c * e - b * d) + X2 * (c * d - a * e);
	coef->Dy1 = all_points * (Y1 * b - Y2 * c) + e * (Y2 * d - Y1 * e) + Y3 * (c * e - b * d);
	coef->Dy2 = all_points * (Y2 * a - Y1 * c) + d * (Y1 * e - Y2 * d) + Y3 * (c * d - a * e);
	coef->Dy3 = Y3 * (a * b - c * c) + Y1 * (c * e - b * d) + Y2 * (c * d -a * e);
}

static tCoef tsccoef;

/* поддержка калибровки */
const void * board_tsc_normparams(void)
{
	return & tsccoef;
}

/*
 * Преобразование координат тачскрина в дисплейные/экранные координаты:
 * - в переменной p_t (тип tPoint) принимает координаты тачскрина;
 * - в переменной coef (тип tCoef) принимает коэффициенты преобразования;
 * - в переменной p_d (тип tPoint) возвращает дисплейные координаты.
 */
static void tsc_ConvertPoint(tPoint *p_d, const tPoint *p_t, const tCoef *coef)
{
	const int_fast32_t x = (int_fast32_t) ((p_t->x * coef->Dx1 + p_t->y * coef->Dx2 + coef->Dx3) / coef->D);
	const int_fast32_t y = (int_fast32_t) ((p_t->x * coef->Dy1 + p_t->y * coef->Dy2 + coef->Dy3) / coef->D);
	p_d->x = (x < 0) ? 0 : (x >= DIM_X) ? (DIM_X - 1) : x;
	p_d->y = (y < 0) ? 0 : (y >= DIM_Y) ? (DIM_Y - 1) : y;
}

/* получение координаты нажатия в пределах 0..DIM_X-1 */
uint_fast16_t board_tsc_normalize_x(uint_fast16_t tx, uint_fast16_t ty, const void * params)
{
	const tCoef * const coef = (const tCoef *) params;
	const int_fast32_t x = (int_fast32_t) ((tx * coef->Dx1 + ty * coef->Dx2 + coef->Dx3) / coef->D);
	return (x < 0) ? 0 : (x >= DIM_X) ? (DIM_X - 1) : x;
}

/* получение координаты нажатия в пределах 0..DIM_Y-1 */
uint_fast16_t board_tsc_normalize_y(uint_fast16_t tx, uint_fast16_t ty, const void * params)
{
	const tCoef * const coef = (const tCoef *) params;
	const int_fast32_t y = (int_fast32_t) ((tx * coef->Dy1 + ty * coef->Dy2 + coef->Dy3) / coef->D);
	return (y < 0) ? 0 : (y >= DIM_Y) ? (DIM_Y - 1) : y;
}

#else /* WITHTSC5PCALIBRATE */

/* поддержка калибровки */
const void * board_tsc_normparams(void)
{
	return NULL;
}

#endif /* WITHTSC5PCALIBRATE */

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811)
#include "stmpe811.h"

uint_fast8_t board_tsc_is_pressed(void) /* Return 1 if touch detection */
{
	return stmpe811_is_pressed();
}


/* top left raw data values */
static uint_fast16_t xrawmin = 70;
static uint_fast16_t yrawmin = 3890;
/* bottom right raw data values */
static uint_fast16_t xrawmax = 3990;
static uint_fast16_t yrawmax = 150;

/* получение координаты нажатия в пределах 0..DIM_X-1 */
uint_fast16_t board_tsc_normalize_x(uint_fast16_t x, uint_fast16_t y, const void * params)
{
	return tcsnormalize(x, xrawmin, xrawmax, DIM_X - 1);
}

/* получение координаты нажатия в пределах 0..DIM_Y-1 */
uint_fast16_t board_tsc_normalize_y(uint_fast16_t x, uint_fast16_t y, const void * params)
{
	return tcsnormalize(y, yrawmin, yrawmax, DIM_Y - 1);
}

// On AT070TN90 with touch screen attached Y coordinate increments from bottom to top, X from left to right
uint_fast8_t
board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr, uint_fast16_t * zr)
{
	static uint_fast16_t x = 0, y = 0;
	uint_fast8_t z = 0;
	if (board_tsc_is_pressed())
	{
		if (stmpe811_TS_GetXYZ(& x, & y, & z))
		{
			* xr = x;
			* yr = y;
			* zr = z;
			return 1;
		}
		* xr = x;
		* yr = y;
		* zr = z;
		return 1;
	}
	* xr = x;
	* yr = y;
	* zr = z;
	return 0;
}

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811) */

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911)
#include "gt911.h"


/* получение координаты нажатия в пределах 0..DIM_X-1 */
uint_fast16_t board_tsc_normalize_x(uint_fast16_t x, uint_fast16_t y, const void * params)
{
#if BOARD_TSC1_XMIRROR
	return DIM_X - 1 - x;
#else /* BOARD_TSC1_XMIRROR */
	return x;
#endif /* BOARD_TSC1_XMIRROR */
}

/* получение координаты нажатия в пределах 0..DIM_Y-1 */
uint_fast16_t board_tsc_normalize_y(uint_fast16_t x, uint_fast16_t y, const void * params)
{
#if BOARD_TSC1_YMIRROR
	return DIM_Y - 1 - y;
#else /* BOARD_TSC1_XMIRROR */
	return y;
#endif /* BOARD_TSC1_XMIRROR */
}

/* получение ненормальзованных координат нажатия */
uint_fast8_t
board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr, uint_fast16_t * zr)
{
	* zr = 0;	// stub
#if LINUX_SUBSYSTEM
	static uint32_t oldt = sys_now();
	static uint_fast16_t x = 0, y = 0, p = 0;

	uint32_t t = sys_now();
	if (t - oldt > 20)		// перед чтениями координат нужна задержка минимум на 15 + 5 ms
	{
		oldt = t;
		p = gt911_getXY(& x, & y);
	}

	* xr = x;
	* yr = y;
	return p;
#else
	return gt911_getXY(xr, yr);
#endif /* LINUX_SUBSYSTEM */
}

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911) */

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_FT5336)
#include "ft5336.h"

static TS_StateTypeDef ts_ft5336;

static void
tsc_interrupt_handler(void)
{
	TP();
}

/* получение координаты нажатия в пределах 0..DIM_X-1 */
uint_fast16_t board_tsc_normalize_x(uint_fast16_t x, uint_fast16_t y, const void * params)
{
	return x;
}

/* получение координаты нажатия в пределах 0..DIM_Y-1 */
uint_fast16_t board_tsc_normalize_y(uint_fast16_t x, uint_fast16_t y, const void * params)
{
	return y;
}

/* получение ненормальзованных координат нажатия */
uint_fast8_t
board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr, uint_fast16_t * zr)
{
	static uint_fast16_t x = 0, y = 0;

	* zr = 0;	// stub
	ft5336_GetState(& ts_ft5336);

	if (ts_ft5336.touchDetected)
	{
		* xr = ts_ft5336.touchX[0];
		* yr = ts_ft5336.touchY[0];
		return 1;
	}
	* xr = x;
	* yr = y;
	return 0;
}

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_FT5336) */

#if defined (TSC1_TYPE) && TSC1_TYPE == TSC_TYPE_XPT2046

#include "xpt2046.h"

#endif /* defined (TSC1_TYPE) && TSC1_TYPE == TSC_TYPE_XPT2046 */

#if defined(TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_S3402)

#define TSC_I2C_ADDR (0x20 * 2)

static void s3402_initialize(void)
{
	// BOARD_TP_RESX - active low
	//	x-gpios = <&gpiog 0 GPIO_ACTIVE_HIGH>; /* TP_RESX_18 */
	const portholder_t BOARD_TP_RESX = (UINT32_C(1) << 0);	// PG0 - TP_RESX_18 - pin 03
	arm_hardware_piog_outputs(BOARD_TP_RESX, 0 * BOARD_TP_RESX);
	local_delay_ms(5);
	arm_hardware_piog_outputs(BOARD_TP_RESX, 1 * BOARD_TP_RESX);
	local_delay_ms(50);

	const unsigned i2caddr = TSC_I2C_ADDR;


	i2c_start(i2caddr | 0x00);
	i2c_write(0xFF);		// set page addr
	i2c_write(0x00);		// page #0
	i2c_waitsend();
    i2c_stop();
}

int s3402_get_id(void)
{
	const unsigned i2caddr = TSC_I2C_ADDR;

	uint8_t v0;

	i2c_start(i2caddr | 0x00);
	i2c_write_withrestart(0xE1);	//  Manufacturer ID register
	i2c_start(i2caddr | 0x01);
	i2c_read(& v0, I2C_READ_ACK_NACK);	// ||	The Manufacturer ID register always returns data $01.

	PRINTF("tsc id=%08lX (expected 0x01)\n", v0);

	return v0;
}

/* получение координаты нажатия в пределах 0..DIM_X-1 */
uint_fast16_t board_tsc_normalize_x(uint_fast16_t x, uint_fast16_t y, const void * params)
{
	return x;
}

/* получение координаты нажатия в пределах 0..DIM_Y-1 */
uint_fast16_t board_tsc_normalize_y(uint_fast16_t x, uint_fast16_t y, const void * params)
{
	return y;
}

uint_fast8_t
board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr, uint_fast16_t * zr)
{
	const unsigned i2caddr = TSC_I2C_ADDR;


	* zr = 0;	// stub
	uint8_t v0, v1, v2, v3, v4, v5, v6, v7;

	i2c_start(i2caddr | 0x00);
	i2c_write_withrestart(0x06);	// Address=0x0006 is used to read coordinate.
	i2c_start(i2caddr | 0x01);
	i2c_read(& v0, I2C_READ_ACK_1);	// ||
	i2c_read(& v1, I2C_READ_ACK);	// ||
	i2c_read(& v2, I2C_READ_ACK);	// ||
	i2c_read(& v3, I2C_READ_ACK);	// ||
	i2c_read(& v4, I2C_READ_ACK);	// ||
	i2c_read(& v5, I2C_READ_ACK);	// ||
	i2c_read(& v6, I2C_READ_ACK);	// ||
	i2c_read(& v7, I2C_READ_NACK);	// ||

	if (v0 != 0)
	{
		* xr = v1 + v2 * 256;
		* yr = v3 + v4 * 256;
		return 1;
	}
	return 0;
}
#endif /* defined(TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_S3402) */

#if defined(TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_ILI2102)

#define TSC_ILI2102_ADDR		0x82

#define REG_TOUCHDATA         	0x10
#define REG_PANEL_INFO        	0x20
#define REG_FIRMWARE_VERSION  	0x40
#define REG_CALIBRATE         	0xCC

#define MAX_TOUCHES    			2

static uint8_t tsc_ili2102_present = 0;

/* получение координаты нажатия в пределах 0..DIM_X-1 */
uint_fast16_t board_tsc_normalize_x(uint_fast16_t x, uint_fast16_t y, const void * params)
{
	return x;
}

/* получение координаты нажатия в пределах 0..DIM_Y-1 */
uint_fast16_t board_tsc_normalize_y(uint_fast16_t x, uint_fast16_t y, const void * params)
{
	return y;
}

uint_fast8_t
board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr, uint_fast16_t * zr)
{
	uint8_t command = REG_TOUCHDATA;
	uint8_t read_buf[9];

	* zr = 0;	// stub
	if (! tsc_ili2102_present)
	{
		return 0;
	}

	i2chw_write(TSC_ILI2102_ADDR, & command, 1);
	i2chw_read(TSC_ILI2102_ADDR, read_buf, sizeof(read_buf));

	if (read_buf[0])
	{
		* xr = read_buf[1] | read_buf[2] << 8;
		* yr = read_buf[3] | read_buf[4] << 8;
		return 1;
	}
	return 0;
}

void ili2102_initialize(void)
{
	BOARD_GT911_RESET_INITIO_1();
	BOARD_GT911_RESET_SET(1);
	local_delay_us(100);
	BOARD_GT911_RESET_SET(0);
	local_delay_us(100);
	BOARD_GT911_RESET_SET(1);
	local_delay_us(500);

	uint8_t command = REG_FIRMWARE_VERSION;
	uint8_t read_buf[3], status;

	status = i2chw_write(TSC_ILI2102_ADDR, & command, 1);
	i2chw_read(TSC_ILI2102_ADDR, read_buf, sizeof(read_buf));

	if (status)
	{
		tsc_ili2102_present = 0;
		PRINTF("ili2102 initialize error\n");
		return;
	}

	tsc_ili2102_present = 1;
	PRINTF("ili2102 initialize successful\n");
}

#endif /*defined(TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_ILI2102) */

#if TSC1_TYPE == TSC_TYPE_AWTPADC

// https://github.com/RT-Thread/rt-thread/blob/master/bsp/allwinner/libraries/sunxi-hal/hal/source/tpadc/hal_tpadc.c

void awgpadc_initialize(void)
{
	CCU->TPADC_CLK_REG = 0x00 * (UINT32_C(1) << 31);	// 000: HOSC
	CCU->TPADC_CLK_REG = (UINT32_C(1) << 31);	// TPADC_CLK_GATING

	CCU->TPADC_BGR_REG |= (UINT32_C(1) << 0);	// Gating clock to TPADC
	CCU->TPADC_BGR_REG &= ~ (UINT32_C(1) << 16);	// Assert TPADC RESET
	CCU->TPADC_BGR_REG |= (UINT32_C(1) << 16);	// De-assert TPADC RESET

	TPADC->TP_CTRL_REG1 =
		0 * (UINT32_C(1) << 4) | // TP_MODE_SELECT
		0x0F * (UINT32_C(1) << 0) | // ADC_CHAN3_SELECT..ADC_CHAN0_SELECT
		0;
	TPADC->TP_CTRL_REG1 |= (UINT32_C(1) << 7); 	// TOUCH_PAN_CALI_EN
	while ((TPADC->TP_CTRL_REG1 & (UINT32_C(1) << 7)) != 0)
		;
	TPADC->TP_CTRL_REG1 |= (UINT32_C(1) << 5); 	// TP_EN
}

uint_fast8_t
board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr, uint_fast16_t * zr)
{
	* zr = 0;	// stub
	if ((TPADC->TP_INT_FIFO_STAT_REG & (UINT32_C(1) << 16)) != 0)
	{
		const uint_fast32_t v = TPADC->TP_DATA_REG & 0xFFF;
		TPADC->TP_INT_FIFO_STAT_REG = (UINT32_C(1) << 16); // Clear FIFO data pending flag
		* xr = 0;
		* yr = 0;
		return 0 * 1;
	}
	return 0;
}


/* получение координаты нажатия в пределах 0..DIM_X-1 */
uint_fast16_t board_tsc_normalize_x(uint_fast16_t x, uint_fast16_t y, const void * params)
{
	return x;
}

/* получение координаты нажатия в пределах 0..DIM_Y-1 */
uint_fast16_t board_tsc_normalize_y(uint_fast16_t x, uint_fast16_t y, const void * params)
{
	return y;
}

#endif /* TSC1_TYPE == TSC_TYPE_AWTPADC */

#if defined (TSC1_TYPE)

/* вызывается при разрешённых прерываниях. */
void board_tsc_initialize(void)
{
#if TSC1_TYPE == TSC_TYPE_GT911
	if (gt911_initialize())
		PRINTF("gt911 initialization successful\n");
	else
		PRINTF("gt911 initialization error\n");
#endif /* TSC1_TYPE == TSC_TYPE_GT911 */

#if TSC1_TYPE == TSC_TYPE_STMPE811
	stmpe811_initialize();
#endif /* TSC1_TYPE == TSC_TYPE_STMPE811 */

#if TSC1_TYPE == TSC_TYPE_FT5336
	if (ft5336_Initialize(DIM_X, DIM_Y) == FT5336_I2C_INITIALIZED)
		PRINTF("ft5336 initialization successful\n");
	else
	{
		PRINTF("ft5336 initialization error\n");
	}
#endif /* TSC1_TYPE == TSC_TYPE_FT5336 */

#if TSC1_TYPE == TSC_TYPE_XPT2046
	xpt2046_initialize();
#endif /* TSC1_TYPE == TSC_TYPE_XPT2046 */

#if TSC1_TYPE == TSC_TYPE_S3402
	s3402_initialize();
	s3402_get_id();	// test
#endif /* TSC1_TYPE == TSC_TYPE_XPT2046 */

#if TSC1_TYPE == TSC_TYPE_ILI2102
	ili2102_initialize();
#endif /* TSC1_TYPE == TSC_TYPE_ILI2102 */

#if TSC1_TYPE == TSC_TYPE_AWTPADC
	awgpadc_initialize();
#endif /* TSC1_TYPE == TSC_TYPE_AWTPADC */


#if WITHTSC5PCALIBRATE

	const uint_fast16_t xstep = DIM_X / 6;
	const uint_fast16_t ystep = DIM_Y / 6;
	tPoint p_display [TSCCALIBPOINTS];
	tPoint * const p_touch = board_tsc_getcalpoints();

	p_display [0].x = xstep * 1;	// левый верхний
	p_display [0].y = ystep * 1;
	p_display [1].x = xstep * 5;	// правый верхний
	p_display [1].y = ystep * 1;
	p_display [2].x = xstep * 1;	// левый нижний
	p_display [2].y = ystep * 5;
	p_display [3].x = xstep * 5;	// правый нижний
	p_display [3].y = ystep * 5;
	p_display [4].x = xstep * 3;	// центр экрана
	p_display [4].y = ystep * 3;

	// Выполнение калибровки тач сенсора
	if (0)
	{
		enum { r0 = 15 };
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		gxdrawb_t dbv;	// framebuffer для выдачи диагностических сообщений

		if (1)
		{
			uint_fast8_t tg;	// получение калибровочных значений для данной точки
			for (tg = 0; tg < TSCCALIBPOINTS; ++ tg)
			{
				//PRINTF("tsc: calibrate target %u\n", (unsigned) tg);

				gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);
				// стереть фон
				colpip_fillrect(& dbv, 0, 0, DIM_X, DIM_Y, COLOR_BLACK);
				// нарисовать мишени для калибровки
				uint_fast8_t i;
				for (i = 0; i < TSCCALIBPOINTS; ++ i)
				{
					const uint_fast16_t xg = DIM_X / 32;
					const uint_fast16_t yg = DIM_Y / 20;
					colpip_line(& dbv, p_display [i].x - xg, p_display [i].y - 0, p_display [i].x + xg, p_display [i].y + 0, COLOR_WHITE, 0);
					colpip_line(& dbv, p_display [i].x - 0, p_display [i].y - yg, p_display [i].x + 0, p_display [i].y + yg, COLOR_WHITE, 0);
					if (i == tg)
					{
						colpip_segm(& dbv, p_display [i].x, p_display [i].y, 0, 360, 15, r0, COLOR_WHITE, 0, 0);
					}
				}
				colpip_text(& dbv, xstep * 2, ystep * 5, COLOR_WHITE, "CALIBRATE", 9);
				colmain_nextfb();
				// wait answer
				unsigned as;
				for (as = 0; as < 5000; /*++ as */)
				{
					uint_fast16_t x, y, z;
					if (board_tsc_getraw(& x, & y, & z))
					{
						p_touch [tg].x = x;
						p_touch [tg].y = y;
						//PRINTF("tsc: calibrate target %u: x=%-5u, y=%-5u , z=%-5u\n", tg, x, y, z);
						PRINTF("{ %u, %u, }, /* point %u */\n", x, y, tg);
						break;
					}
					//local_delay_ms(1);
				}
				gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);
				// стереть фон
				colpip_fillrect(& dbv, 0, 0, DIM_X, DIM_Y, COLOR_BLACK);
				colpip_text(& dbv, xstep * 2, ystep * 5, COLOR_WHITE, "CALIBRATE DONE", 14);
				colmain_nextfb();
				//PRINTF("tsc: calibrate target %u done\n", (unsigned) tg);
			}
		}

	}

	//Раcсчитываем коэффициенты для перехода от координат тачскрина в дисплейные координаты.
	CoefCalc(p_display, p_touch, & tsccoef, TSCCALIBPOINTS);
#endif /* WITHTSC5PCALIBRATE */

	/* Тест результата калибровки с рисованием точки касания */
#if WITHDEBUG && 0
	for (;;)
	{
		uint_fast16_t x, y;
		if (board_tsc_getxy(& x, & y))
		{
			enum { r0 = 15 };
			board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
			board_update();
			gxdrawb_t dbv;	// framebuffer для выдачи диагностических сообщений

			PRINTF("board_tsc_getxy: x=%-5u, y=%-5u\n", x, y);

			gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);
			// стереть фон
			colpip_fillrect(& dbv, 0, 0, DIM_X, DIM_Y, COLOR_BLACK);
			enum { RSZ = 5 };
			if (x < DIM_X - RSZ && y < DIM_Y - RSZ)
				colpip_fillrect(& dbv, x, y, RSZ, RSZ, COLOR_WHITE);

			colmain_nextfb();


		}
	}
#endif

	/* Тест - печать ненормализованных значений */
#if WITHDEBUG && 0
	for (;;)
	{
		uint_fast16_t x, y, z;
		if (board_tsc_getraw(& x, & y, & z))
		{
			uint_fast16_t xc = board_tsc_normalize_x(x, y, NULL);
			uint_fast16_t yc = board_tsc_normalize_y(x, y, NULL);
			PRINTF("board_tsc_getraw: x=%-5u, y=%-5u , z=%-5u -> xc=%-5u, yc=%-5u\n", x, y, z, xc, yc);
		}
	}
#endif

}

#if ! LINUX_SUBSYSTEM && defined (TSC1_TYPE) && TSC1_TYPE != TSC_TYPE_EVDEV

uint_fast8_t
board_tsc_getxy(uint_fast16_t * xr, uint_fast16_t * yr)
{
	uint_fast16_t x, y, z;
	if (board_tsc_getraw(& x, & y, & z))
	{
		const void * const param = board_tsc_normparams();	/* поддержка калибровки */

		* xr = board_tsc_normalize_x(x, y, param);
		* yr = board_tsc_normalize_y(x, y, param);
		//PRINTF("board_tsc_getxy: x=%-5u, y=%-5u -> xc=%-5u, yc=%-5u\n", x, y, * xr, * yr);
		return 1;
	}
	* xr = 0;	/* зачем ? */
	* yr = 0;	/* зачем ? */
	return 0;
}

#endif /* ! LINUX_SUBSYSTEM && defined (TSC1_TYPE) && TSC1_TYPE != TSC_TYPE_EVDEV */

#if WITHLVGL && 0

#include "lv_drivers/indev/evdev.h"

void board_tsc_indev_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
	uint_fast16_t x, y;

	if(drv->type == LV_INDEV_TYPE_POINTER)
	{
		if (board_tsc_getxy(& x, & y))
			data->state = LV_INDEV_STATE_PR;
		else
			data->state = LV_INDEV_STATE_REL;

		data->point.x = x;
		data->point.y = y;
	}
}

#endif /* WITHLVGL */

#endif /* defined (TSC1_TYPE) */
