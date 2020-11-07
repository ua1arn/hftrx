#include "hardware.h"
#include "board.h"
#include "formats.h"
#include "gpio.h"

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911)

#include "gt911.h"

static uint_fast8_t gt911_addr = 0;
static uint_fast8_t tscpresetnt;
static uint_fast8_t tsc_int = 0;

void gt911_set_reg(uint_fast16_t reg)
{
	i2c_start(gt911_addr);
	i2c_write(reg >> 8);
	i2c_write(reg & 0xFF);
}

void gt911_read(uint_fast16_t reg, uint8_t * buf, size_t len)
{
	uint_fast8_t k = 0;

	gt911_set_reg(reg);

	i2c_start(gt911_addr | 1);

	if (len == 1)
		i2c_read(buf, I2C_READ_ACK_NACK);
	else if (len == 2)
	{
		i2c_read(buf ++, I2C_READ_ACK_1);	/* чтение первого байта ответа */
		i2c_read(buf ++, I2C_READ_NACK);	/* чтение последнего байта ответа */
	}
	else
	{
		i2c_read(buf ++, I2C_READ_ACK_1);	/* чтение первого байта ответа */
		while (++ k <= len - 2)
		{
			i2c_read(buf ++, I2C_READ_ACK);	/* чтение промежуточного байта ответа */
		}
		i2c_read(buf ++, I2C_READ_NACK);	/* чтение последнего байта ответа */
	}
}

uint16_t gt911_readInput(GTPoint * point)
{
	uint_fast8_t touch_num, buf_state;
	uint8_t buf [6];

	gt911_read(GOODIX_READ_COORD_ADDR, buf, 6);
	buf_state = buf [0] >> 7;

	if (! buf_state)
		return 0;

	touch_num = buf [0] & 0x7f;
	if (touch_num > 0)
	{
		/* получение координат первой точки касания */
		point->x = (buf [2] << 0) | (buf [3] << 8);
		point->y = (buf [4] << 0) | (buf [5] << 8);
	}
	gt911_set_reg(GOODIX_READ_COORD_ADDR);
	i2c_write(0);

	return touch_num;
}

uint_fast8_t gt911_calcChecksum(uint8_t * buf, uint_fast8_t len)
{
	uint_fast8_t ccsum = 0;
	for (uint_fast8_t i = 0; i < len; i++) {
		ccsum += buf [i];
	}
	//ccsum %= 256;
	ccsum = (~ ccsum) + 1;
	return ccsum & 0xFF;
}

uint_fast8_t gt911_readChecksum(void)
{
	uint_fast16_t aStart = GT_REG_CFG;
	uint_fast16_t aStop = 0x80FE;
	uint_fast8_t len = aStop - aStart + 1;
	uint8_t buf [len];

	gt911_read(aStart, buf, len);
	return gt911_calcChecksum(buf, len);
}

void gt911_readConfig(uint8_t * config)
{
	gt911_read(GT_REG_CFG, config, GOODIX_CONFIG_911_LENGTH);
}

void gt911_fwResolution(uint_fast16_t maxX, uint_fast16_t maxY)
{
	uint_fast8_t len = GOODIX_CONFIG_911_LENGTH;
	uint16_t pos = 0;
	uint8_t cfg [len];
	gt911_readConfig(cfg);

	cfg [1] = (maxX & 0xff);
	cfg [2] = (maxX >> 8);
	cfg [3] = (maxY & 0xff);
	cfg [4] = (maxY >> 8);
	cfg [15] = 0xf;			// период опроса 15 + 5 мс
	cfg [len - 2] = gt911_calcChecksum(cfg, len - 2);
	cfg [len - 1] = 1;

	while (pos < len) {
		gt911_set_reg(GT_REG_CFG + pos);
		i2c_write(cfg [pos]);
		pos ++;
	}
}

uint_fast16_t gt911_productID(void) {
	uint_fast8_t res;
	uint8_t buf [4];

	gt911_read(GOODIX_REG_ID, buf, 4);
	res = buf [3] | (buf [2] << 8) | (buf [1] << 16) | (buf [0] << 24);
	return res;
}

uint_fast8_t gt911_getXY(uint_fast16_t * xt, uint_fast16_t * yt)
{
	if (! tscpresetnt || ! tsc_int)
		return 0;

	GTPoint points[5]; //points buffer
	int8_t contacts;
	contacts = gt911_readInput(points);
	tsc_int = 0;

	if (contacts == 0)
		return 0;

	* xt = points [0].x;
	* yt = points [0].y;
	return 1;
}

static void
gt911_interrupt_handler(void)
{
	tsc_int = 1;
}

uint_fast8_t gt911_initialize(void)
{
	const portholder_t int_pin = 1uL << 3;		/* P5_3 */
	const portholder_t reset_pin = 1uL << 15;	/* P5_15 */

	// reset and i2c address select sequence
	arm_hardware_pio5_outputs(reset_pin, 1 * reset_pin);
	local_delay_us(100);
	arm_hardware_pio5_outputs(reset_pin, 0 * reset_pin);
	local_delay_us(100);
	arm_hardware_pio5_outputs(int_pin, 0 * int_pin);		// 0xBA address select
	local_delay_us(100);
	arm_hardware_pio5_outputs(reset_pin, 1 * reset_pin);
	local_delay_ms(100);
	arm_hardware_pio5_inputs(int_pin);

	gt911_addr = GOODIX_I2C_ADDR_BA;
	tscpresetnt = 0;
	uint_fast16_t id = gt911_productID();
	if (id != GT911_ID)
		return 0;

	gt911_fwResolution(DIM_X, DIM_Y);
	gt911_set_reg(GOODIX_READ_COORD_ADDR);
	i2c_write(0);
	tscpresetnt = 1;

	arm_hardware_pio5_onchangeinterrupt(int_pin, 1, ARM_SYSTEM_PRIORITY, gt911_interrupt_handler);	// P5_3 interrupt, rising edge sensitive

	return 1;
}
#endif
