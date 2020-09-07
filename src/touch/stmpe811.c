#include "hardware.h"
#include "board.h"
#include "formats.h"

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811)
#include "stmpe811.h"

static uint_fast8_t tscpresetnt;

int32_t i2cperiph_readN(uint_fast8_t d_adr, uint_fast8_t r_adr, uint32_t r_byte, uint8_t * r_buffer)
{
	i2c_start(d_adr);
	i2c_write_withrestart(r_adr);		// Register 135
	i2c_start(d_adr | 1);
	if (r_byte == 1)
	{
		uint_fast8_t v;
		i2c_read(r_buffer, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
		return 0;
	}
	else if (r_byte == 2)
	{
		i2c_read(r_buffer ++, I2C_READ_ACK_1);	/* чтение первого байта ответа */
		i2c_read(r_buffer ++, I2C_READ_NACK);	/* чтение последнего байта ответа */
		return 0;
	}
	else
	{
		i2c_read(r_buffer ++, I2C_READ_ACK_1);	/* чтение первого байта ответа */
		while (r_byte -- > 2)
		{
			i2c_read(r_buffer ++, I2C_READ_ACK);	/* чтение промежуточного байта ответа */
		}
		i2c_read(r_buffer ++, I2C_READ_NACK);	/* чтение последнего байта ответа */
		return 0;
	}
}

void i2cperiph_write8(uint_fast8_t DeviceAddr, uint_fast8_t reg, uint_fast8_t val)
{
	i2c_start(DeviceAddr);
	i2c_write(reg);		// Register 135
	i2c_write(val);
	i2c_waitsend();
	i2c_stop();
}

uint_fast16_t i2cperiph_read8(uint_fast8_t DeviceAddr, uint_fast8_t reg)
{
	uint8_t v;

	i2c_start(DeviceAddr);
	i2c_write_withrestart(reg);		// Register 135
	i2c_start(DeviceAddr | 1);
	i2c_read(& v, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
	return v;
}

uint_fast16_t i2cperiph_read16(uint_fast8_t DeviceAddr, uint_fast8_t reg)
{
	uint8_t v [2];

	i2c_start(DeviceAddr);
	i2c_write_withrestart(reg);		// Register 135
	i2c_start(DeviceAddr | 1);
	i2c_read(& v [0], I2C_READ_ACK_1);
	i2c_read(& v [1], I2C_READ_NACK);
	return v [0] * 256 + v [1];
}


/**
  * @brief  Disable the AF for the selected IO pin(s).
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  IO_Pin: The IO pin to be configured. This parameter could be any
  *         combination of the following values:
  *   @arg  STMPE811_PIN_x: Where x can be from 0 to 7.
  * @retval None
  */
void stmpe811_IO_DisableAF(uint_fast8_t DeviceAddr, uint_fast16_t IO_Pin)
{
  uint8_t tmp = 0;

  /* Get the current register value */
  tmp = i2cperiph_read8(DeviceAddr, STMPE811_REG_IO_AF);

  /* Disable the selected pins alternate function */
  tmp &= ~ (uint8_t) IO_Pin;

  /* Write back the new register value */
  i2cperiph_write8(DeviceAddr, STMPE811_REG_IO_AF, tmp);

}

/* top left raw data values */
static uint_fast16_t xrawmin = 70;
static uint_fast16_t yrawmin = 3890;
/* bottom right raw data values */
static uint_fast16_t xrawmax = 3990;
static uint_fast16_t yrawmax = 150;

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

uint_fast8_t stmpe811_TS_GetXYZ(
	uint_fast16_t * X,
	uint_fast16_t * Y,
	uint_fast8_t * Z
	)
{
	uint_fast8_t DeviceAddr = BOARD_I2C_STMPE811;
	uint8_t dataXYZ [4];
	uint_fast32_t vdataXY;
	uint_fast16_t xx, yy;
	uint_fast8_t sta = i2cperiph_read8(DeviceAddr, STMPE811_REG_FIFO_STA);
	if ((sta & 0x10) == 0)
		return 0;
	i2cperiph_readN(DeviceAddr, STMPE811_REG_TSC_DATA_NON_INC, sizeof dataXYZ / sizeof dataXYZ [0], dataXYZ);

	/* Calculate positions values */
	vdataXY =
			((uint_fast32_t) dataXYZ [0] << 16) |
			((uint_fast32_t) dataXYZ [1] << 8) |
			((uint_fast32_t) dataXYZ [2] << 0);
	xx = (vdataXY >> 12) & 0x00000FFF;
	yy = (vdataXY >> 0) & 0x00000FFF;

	* X = tcsnormalize(xx, xrawmin, xrawmax, DIM_X - 1);
	* Y = tcsnormalize(yy, yrawmin, yrawmax, DIM_Y - 1);
	* Z = dataXYZ [3];
#if 1
	/* Reset FIFO */
	i2cperiph_write8(DeviceAddr, STMPE811_REG_FIFO_STA, 0x01);
	/* Enable the FIFO again */
	i2cperiph_write8(DeviceAddr, STMPE811_REG_FIFO_STA, 0x00);
#endif
	return 1;
}

/**
  * @brief  Configures the touch Screen Controller (Single point detection)
  * @param  DeviceAddr: Device address on communication Bus.
  * @retval None.
  */
static void stmpe811_TS_Start(uint_fast8_t DeviceAddr)
{
	uint_fast8_t mode;

  /* Get the current register value */
  mode = i2cperiph_read8(DeviceAddr, STMPE811_REG_SYS_CTRL2);

   /* Set the Functionalities to be Enabled */
  mode &= ~ (STMPE811_TS_FCT | STMPE811_ADC_FCT);

  /* Set the new register value */
  i2cperiph_write8(DeviceAddr, STMPE811_REG_SYS_CTRL2, mode);


  /* Select Sample Time, bit number and ADC Reference */
  i2cperiph_write8(DeviceAddr, STMPE811_REG_ADC_CTRL1, 0x49);

  /* Wait for 2 ms */
  local_delay_ms(2);

  /* Select the ADC clock speed: 3.25 MHz */
  i2cperiph_write8(DeviceAddr, STMPE811_REG_ADC_CTRL2, 0x01);

  /* Select TSC pins in non default mode */
  stmpe811_IO_DisableAF(DeviceAddr, STMPE811_TOUCH_IO_ALL);

  /* Select 2 nF filter capacitor */
  /* Configuration:
     - Touch average control    : 4 samples
     - Touch delay time         : 500 uS
     - Panel driver setting time: 500 uS
  */
  i2cperiph_write8(DeviceAddr, STMPE811_REG_TSC_CFG, 0x9A);

  /* Configure the Touch FIFO threshold: single point reading */
  i2cperiph_write8(DeviceAddr, STMPE811_REG_FIFO_TH, 0x01);

  /* Clear the FIFO memory content. */
  i2cperiph_write8(DeviceAddr, STMPE811_REG_FIFO_STA, 0x01);

  /* Put the FIFO back into operation mode  */
  i2cperiph_write8(DeviceAddr, STMPE811_REG_FIFO_STA, 0x00);

  /* Set the range and accuracy pf the pressure measurement (Z) :
     - Fractional part :7
     - Whole part      :1
  */
  i2cperiph_write8(DeviceAddr, STMPE811_REG_TSC_FRACT_XYZ, 0x01);

  /* Set the driving capability (limit) of the device for TSC pins: 50mA */
  i2cperiph_write8(DeviceAddr, STMPE811_REG_TSC_I_DRIVE, 0x01);

  /* Touch screen control configuration (enable TSC):
     - No window tracking index
     - XYZ acquisition mode
   */
  i2cperiph_write8(DeviceAddr, STMPE811_REG_TSC_CTRL,
		  (0x05 << 4)	 | /* Tracking index */
		  (0x00 << 1)	 | /* 000: X, Y, Z acquisition */
		  0x01			 | /* EN: Enable TSC */
		  0
		  );

  /*  Clear all the status pending bits if any */
  i2cperiph_write8(DeviceAddr, STMPE811_REG_INT_STA, 0xFF);

  /* Wait for 2 ms delay */
  local_delay_ms(2);
}

void stmpe811_initialize(void)
{
	unsigned chip_id;
	unsigned ver;

	chip_id = i2cperiph_read16(BOARD_I2C_STMPE811, STMPE811_REG_CHP_ID);
	ver = i2cperiph_read8(BOARD_I2C_STMPE811, STMPE811_REG_ID_VER);
	PRINTF(PSTR("stmpe811_initialize: chip_id=%04X, expected %04X, ver=%02x\n"), chip_id, STMPE811_ID, ver);

	tscpresetnt = chip_id == STMPE811_ID;

	if (tscpresetnt != 0 && ver == 0x03)
	{
		stmpe811_TS_Start(BOARD_I2C_STMPE811);
	}
}

uint_fast8_t stmpe811_is_pressed(void) /* Return 1 if touch detection */
{
	return tscpresetnt &&
			((i2cperiph_read8(BOARD_I2C_STMPE811, STMPE811_REG_TSC_CTRL) & STMPE811_TS_CTRL_STATUS) >> STMPE811_TS_CTRL_STATUS_POS) != 0;
}

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811) */
