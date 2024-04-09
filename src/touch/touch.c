/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "src/display/display.h"
#include "formats.h"
#include "touch.h"
#include "gpio.h"


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
board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr)
{
	static uint_fast16_t x = 0, y = 0;
	uint_fast8_t z;
	if (board_tsc_is_pressed())
	{
		if (stmpe811_TS_GetXYZ(& x, & y, & z))
		{
			* xr = x;
			* yr = y;
			return 1;
		}
		* xr = x;
		* yr = y;
		return 1;
	}
	* xr = x;
	* yr = y;
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
board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr)
{
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
board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr)
{
	static uint_fast16_t x = 0, y = 0;

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

#if 1
	static uint_fast16_t xrawmin = 180;//330;
	static uint_fast16_t xrawmid = 2100;//330;
	static uint_fast16_t xrawmax = 3850;//3610;

	static uint_fast16_t yrawmin = 380;//510;
	static uint_fast16_t yrawmid = 2000;//510;
	static uint_fast16_t yrawmax = 3750;//3640;

#else
	/* top left raw data values */
	static uint_fast16_t xrawmin = 850;//330;
	static uint_fast16_t yrawmin = 420;//510;

	/* bottom right raw data values */
	static uint_fast16_t xrawmax = 3990;//3610;
	static uint_fast16_t yrawmax = 3890;//3640;
#endif

/* получение координаты нажатия в пределах 0..DIM_X-1 */
uint_fast16_t board_tsc_normalize_x(uint_fast16_t x, uint_fast16_t y, const void * params)
{

#if BOARD_TSC1_XMIRROR
	return tcsnormalize(x, xrawmax, xrawmin, DIM_X - 1);
#else /* BOARD_TSC1_XMIRROR */
	if (x < xrawmid)
		return tcsnormalize(x, xrawmin, xrawmid - 1, DIM_X / 2 - 1);
	else
		return tcsnormalize(x, xrawmid, xrawmax, DIM_X / 2 - 1) + DIM_X / 2;
#endif /* BOARD_TSC1_XMIRROR */
}

/* получение координаты нажатия в пределах 0..DIM_Y-1 */
uint_fast16_t board_tsc_normalize_y(uint_fast16_t x, uint_fast16_t y, const void * params)
{
#if BOARD_TSC1_YMIRROR
	return tcsnormalize(y, yrawmax, yrawmin, DIM_Y - 1);
#else /* BOARD_TSC1_YMIRROR */
	if (y < yrawmid)
		return tcsnormalize(y, yrawmin, yrawmid - 1, DIM_Y / 2 - 1);
	else
		return tcsnormalize(y, yrawmid, yrawmax, DIM_Y / 2 - 1) + DIM_Y / 2;
#endif /* BOARD_TSC1_YMIRROR */
}

uint_fast8_t
board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr)
{
	return xpt2046_getxy(xr, yr);
}

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
board_tsc_getraw(uint_fast16_t * px, uint_fast16_t * py)
{
	const unsigned i2caddr = TSC_I2C_ADDR;


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
		* px = v1 + v2 * 256;
		* py = v3 + v4 * 256;
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
board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr)
{
	uint8_t command = REG_TOUCHDATA;
	uint8_t read_buf[9];

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
board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr)
{
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

#if TSC1_TYPE == TSC_TYPE_EVDEV
	evdev_initialize();
#endif /* TSC1_TYPE == TSC_TYPE_EVDEV */

	/* Тест - печать ненормализованных значений */
#if WITHDEBUG && 0
	for (;;)
	{
		uint_fast16_t x, y;
		if (board_tsc_getraw(& x, & y))
		{
			uint_fast16_t xc = board_tsc_normalize_x(x, y, NULL);
			uint_fast16_t yc = board_tsc_normalize_y(x, y, NULL);
			PRINTF("board_tsc_getraw: x=%-5u, y=%-5u xc=%-5u, yc=%-5u\n", x, y, xc, yc);
		}
	}
#endif

}

#if ! LINUX_SUBSYSTEM && TSC1_TYPE != TSC_TYPE_EVDEV

uint_fast8_t
board_tsc_getxy(uint_fast16_t * xr, uint_fast16_t * yr)
{
	uint_fast16_t x, y;
	if (board_tsc_getraw(& x, & y))
	{
		* xr = board_tsc_normalize_x(x, y, NULL);
		* yr = board_tsc_normalize_y(x, y, NULL);
		return 1;
	}
	* xr = 0;	/* зачем ? */
	* yr = 0;	/* зачем ? */
	return 0;
}

#endif /* ! LINUX_SUBSYSTEM && TSC1_TYPE != TSC_TYPE_EVDEV */

#if WITHLVGL

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
