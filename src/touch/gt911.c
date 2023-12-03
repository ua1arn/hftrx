#include "hardware.h"
#include "board.h"
#include "formats.h"
#include "gpio.h"

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911)


// При необходимости разместить в файле конфигурации платы.
//#define BOARD_TSC1_XMIRROR 1	// Зеркалируем тачскрин по горизонтали.
//#define BOARD_TSC1_YMIRROR 1	// Зеркалируем тачскрин по вертикали.

#include "gt911.h"

static i2cp_t tp_i2cp;	/* параметры для обмена по I2C. */

static uint_fast8_t gt911_addr = 0;
static uint_fast8_t tscpresetnt = 0;

static void gt911_io_initialize(void)
{
	BOARD_GT911_RESET_INITIO_1();	// 1-st stage init io (int pin pull up)
	// reset and i2c address select sequence
	BOARD_GT911_RESET_SET(1);
	local_delay_us(100);
	BOARD_GT911_RESET_SET(0);
	local_delay_us(100);
	BOARD_GT911_INT_SET(0);		// 0xBA address select
	local_delay_us(100);
	BOARD_GT911_RESET_SET(1);
	local_delay_ms(100);
	BOARD_GT911_RESET_INITIO_2();	// 2-nd stage init io (int pin pull up)
}

static volatile uint_fast8_t tsc_int = 0;

void
gt911_interrupt_handler(void)
{
	tsc_int = 1;
}

#if WITH_GT911_INTERRUPTS

/* считать признак произошедьшего прерывания */
static uint_fast8_t
gt911_interrupt_get(void)
{
	uint_fast8_t f;
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	f = tsc_int;
	tsc_int = 0;
	LowerIrql(oldIrql);
	return f;
}

static void gt911_intconnect(void)
{
	BOARD_GT911_INT_CONNECT();
}

#else

static uint_fast8_t
gt911_interrupt_get(void)
{
	return 1;
}
static void gt911_intconnect(void)
{
}

#endif

void gt911_set_reg(uint_fast16_t reg)
{
#if WITHTWIHW
	uint8_t buf[2] = { (reg >> 8), (reg & 0xFF), };
	i2chw_write(gt911_addr, buf, 2);
#elif WITHTWISW
	i2c_start(gt911_addr);
	i2c_write(reg >> 8);
	i2c_write(reg & 0xFF);
	i2c_waitsend();
	i2c_stop();
#endif
}

void gt911_read(uint_fast16_t reg, uint8_t * buf, size_t len)
{
#if WITHTWIHW
	gt911_set_reg(reg);
	i2chw_read(gt911_addr, buf, len);
#elif WITHTWISW
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
	i2c_stop();
#endif
}

void gt911_write_reg(uint_fast16_t reg, uint8_t val)
{
#if WITHTWIHW
	uint8_t buf[3] = { (reg >> 8), (reg & 0xFF), val, };
	i2chw_write(gt911_addr, buf, 3);
#elif WITHTWISW
	i2c_start(gt911_addr);
	i2c_write(reg >> 8);
	i2c_write(reg & 0xFF);
	i2c_write(val);
	i2c_waitsend();
	i2c_stop();
#endif
}

uint16_t gt911_readInput(GTPoint * point)
{
	uint_fast8_t touch_num, buf_state;
	uint8_t buf [6];

	gt911_read(GOODIX_READ_COORD_ADDR, buf, 6);
	buf_state = buf [0] >> 7;

	if (! buf_state)
		return 0;

	touch_num = buf [0] & 0x0F;
	if (touch_num > 0)
	{
		/* получение координат первой точки касания */
		point->x = (buf [2] << 0) | (buf [3] << 8);
		point->y = (buf [4] << 0) | (buf [5] << 8);
	}
	gt911_write_reg(GOODIX_READ_COORD_ADDR, 0);

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
	const uint_fast16_t aStart = GT_REG_CFG;
	const uint_fast16_t aStop = 0x80FE;
	const uint_fast8_t len = aStop - aStart + 1;
	uint8_t buf [len];

	gt911_read(aStart, buf, len);
	return gt911_calcChecksum(buf, len);
}

void gt911_fwResolution(uint_fast16_t maxX, uint_fast16_t maxY)
{
	uint_fast8_t len = GOODIX_CONFIG_911_LENGTH;
	uint16_t pos = 0;
	uint8_t cfg [len];
	gt911_read(GT_REG_CFG, cfg, GOODIX_CONFIG_911_LENGTH);

	cfg [1] = (maxX & 0xff);
	cfg [2] = (maxX >> 8);
	cfg [3] = (maxY & 0xff);
	cfg [4] = (maxY >> 8);
	cfg [15] = 0xf;			// период опроса 15 + 5 мс
	cfg [len - 2] = gt911_calcChecksum(cfg, len - 2);
	cfg [len - 1] = 1;

	while (pos < len) {
		gt911_write_reg(GT_REG_CFG + pos, cfg [pos]);
		pos ++;
	}
}

uint32_t gt911_productID(void) {
	uint32_t res;
	uint8_t buf [4];

	gt911_read(GOODIX_REG_ID, buf, 4);
	res = buf [3] | (buf [2] << 8) | (buf [1] << 16) | (buf [0] << 24);
	return res;
}

/* получение ненормальзованных координат нажатия */
uint_fast8_t gt911_getXY(uint_fast16_t * xt, uint_fast16_t * yt)
{
	if (! tscpresetnt || ! gt911_interrupt_get())
		return 0;

	GTPoint points[5]; //points buffer
	uint8_t contacts;
	contacts = gt911_readInput(points);

	if (contacts == 0)
		return 0;

	* xt = points [0].x;
	* yt = points [0].y;

	return 1;
}

uint_fast8_t gt911_initialize(void)
{
#if (WITHTWISW) && ! LINUX_SUBSYSTEM
	i2cp_intiialize(& tp_i2cp, I2CP_I2C1, 400000);
#endif /* (WITHTWISW) && ! LINUX_SUBSYSTEM */

	gt911_io_initialize();

	gt911_addr = GOODIX_I2C_ADDR_BA;
	tscpresetnt = 0;
	uint32_t id = gt911_productID();
	if (id != GT911_ID && id != GT9157_ID)
		return 0;

	gt911_fwResolution(DIM_X, DIM_Y);
	gt911_write_reg(GOODIX_READ_COORD_ADDR, 0);
	tscpresetnt = 1;

	gt911_intconnect();

	return 1;
}
#endif
