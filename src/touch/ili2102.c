/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Capacitive touch screen controller Ilitek ILI2102

#include "hardware.h"
#include "formats.h"

#if defined(TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_ILI2102)
#include "touch.h"

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

/* вызывается при разрешённых прерываниях. */
void board_tsc_initialize(void)
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
