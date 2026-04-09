/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

/* Capasitive touch screen controller S3402 (on panel H497TLB01.4) */

#include "hardware.h"
#include "formats.h"

#if defined(TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_S3402)

#include "gpio.h"
#include "touch.h"

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

/* вызывается при разрешённых прерываниях. */
void board_tsc_initialize(void)
{
	s3402_initialize();
	s3402_get_id();	// test
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
