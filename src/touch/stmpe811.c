#include "hardware.h"
#include "board.h"
#include "formats.h"
#include "gpio.h"

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811)
#include "stmpe811.h"

static uint_fast8_t tscpresent;
static volatile uint_fast8_t tsc_int = 0;

static i2cp_t tp_i2cp;	/* параметры для обмена по I2C. */

static int i2cperiph_readN(uint_fast8_t d_adr, uint_fast8_t r_adr, uint32_t r_byte, uint8_t * r_buffer)
{
	const i2cp_t * const p1 = & tp_i2cp;
#if WITHTWISW

	i2cp_start(p1, d_adr);
	i2cp_write_withrestart(p1, r_adr);		// Register 135
	i2cp_start(p1, d_adr | 1);
	if (r_byte == 1)
	{
		uint_fast8_t v;
		i2cp_read(p1, r_buffer, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
		return 0;
	}
	else if (r_byte == 2)
	{
		i2cp_read(p1, r_buffer ++, I2C_READ_ACK_1);	/* чтение первого байта ответа */
		i2cp_read(p1, r_buffer ++, I2C_READ_NACK);	/* чтение последнего байта ответа */
		return 0;
	}
	else
	{
		i2cp_read(p1, r_buffer ++, I2C_READ_ACK_1);	/* чтение первого байта ответа */
		while (r_byte -- > 2)
		{
			i2cp_read(p1, r_buffer ++, I2C_READ_ACK);	/* чтение промежуточного байта ответа */
		}
		i2cp_read(p1, r_buffer ++, I2C_READ_NACK);	/* чтение последнего байта ответа */
		return 0;
	}

#elif WITHTWIHW

	uint8_t bufw = r_adr;
	i2chw_write(d_adr, & bufw, 1);
	return i2chw_read(d_adr, r_buffer, r_byte);

#endif
}

static void i2cperiph_write8(uint_fast8_t DeviceAddr, uint_fast8_t reg, uint_fast8_t val)
{
	const i2cp_t * const p1 = & tp_i2cp;
#if WITHTWISW
	i2cp_start(p1, DeviceAddr);
	i2cp_write(p1, reg);		// Register 135
	i2cp_write(p1, val);
	i2cp_waitsend(p1);
	i2cp_stop(p1);
#elif WITHTWIHW
	uint8_t bufw[2] = { reg, val, };
	i2chw_write(DeviceAddr, bufw, 2);
#endif
}

static uint_fast16_t i2cperiph_read8(uint_fast8_t DeviceAddr, uint_fast8_t reg)
{
	const i2cp_t * const p1 = & tp_i2cp;
	uint8_t v = 0xFF;
#if WITHTWISW
	i2cp_start(p1, DeviceAddr);
	i2cp_write_withrestart(p1, reg);		// Register 135
	i2cp_start(p1, DeviceAddr | 1);
	i2cp_read(p1, & v, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
#elif WITHTWIHW
	i2cperiph_readN(DeviceAddr, reg, 1, & v);
#endif
	return v;
}

static uint_fast16_t i2cperiph_read16(uint_fast8_t DeviceAddr, uint_fast8_t reg)
{
	const i2cp_t * const p1 = & tp_i2cp;
	uint8_t v [2] = { 0xFF, 0xFF };
#if WITHTWISW
	i2cp_start(p1, DeviceAddr);
	i2cp_write_withrestart(p1, reg);		// Register 135
	i2cp_start(p1, DeviceAddr | 1);
	i2cp_read(p1, & v [0], I2C_READ_ACK_1);
	i2cp_read(p1, & v [1], I2C_READ_NACK);
#elif WITHTWIHW
	i2cperiph_readN(DeviceAddr, reg, 2, v);
#endif
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
static void stmpe811_IO_DisableAF(uint_fast8_t DeviceAddr, uint_fast16_t IO_Pin)
{
  uint8_t tmp = 0;

  /* Get the current register value */
  tmp = i2cperiph_read8(DeviceAddr, STMPE811_REG_IO_AF);

  /* Disable the selected pins alternate function */
  tmp &= ~ (uint8_t) IO_Pin;

  /* Write back the new register value */
  i2cperiph_write8(DeviceAddr, STMPE811_REG_IO_AF, tmp);

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

#if WITH_STMPE811_INTERRUPTS
	if (! tscpresent || ! tsc_int)
		return 0;

	tsc_int = 0;
#else /* WITH_STMPE811_INTERRUPTS */
	if (! tscpresent)
		return 0;
#endif /* WITH_STMPE811_INTERRUPTS */

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

	* X = xx;// tcsnormalize(xx, xrawmin, xrawmax, DIM_X - 1);
	* Y = yy;//tcsnormalize(yy, yrawmin, yrawmax, DIM_Y - 1);
	* Z = dataXYZ [3];
#if 1
	/* Reset FIFO */
	i2cperiph_write8(DeviceAddr, STMPE811_REG_FIFO_STA, 0x01);
	/* Enable the FIFO again */
	i2cperiph_write8(DeviceAddr, STMPE811_REG_FIFO_STA, 0x00);
#endif
	i2cperiph_write8(DeviceAddr, STMPE811_REG_INT_STA, 0xFF);
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

#if WITH_STMPE811_INTERRUPTS
  i2cperiph_write8(DeviceAddr, STMPE811_REG_INT_CTRL, 0x07);
  i2cperiph_write8(DeviceAddr, STMPE811_REG_INT_EN, STMPE811_GIT_TOUCH);
#endif /* WITH_STMPE811_INTERRUPTS */

  /*  Clear all the status pending bits if any */
  i2cperiph_write8(DeviceAddr, STMPE811_REG_INT_STA, 0xFF);

  /* Wait for 2 ms delay */
  local_delay_ms(2);
}

void
stmpe811_interrupt_handler(void * ctx)
{
	tsc_int = 1;
}

void stmpe811_initialize(void)
{
	unsigned chip_id;
	unsigned ver;

#if (WITHTWISW) && ! LINUX_SUBSYSTEM
	i2cp_intiialize(& tp_i2cp, I2CP_I2C1, 400000);
#endif /* (WITHTWISW) && ! LINUX_SUBSYSTEM */
	/* Soft reset */
	i2cperiph_write8(BOARD_I2C_STMPE811, STMPE811_REG_SYS_CTRL1, 0x02);

	chip_id = i2cperiph_read16(BOARD_I2C_STMPE811, STMPE811_REG_CHP_ID);
	ver = i2cperiph_read8(BOARD_I2C_STMPE811, STMPE811_REG_ID_VER);
	PRINTF(PSTR("stmpe811_initialize: chip_id=%04X, expected %04X, ver=%02x\n"), chip_id, STMPE811_ID, ver);

	tscpresent = chip_id == STMPE811_ID;

	if (tscpresent != 0 && ver == 0x03)
	{
		stmpe811_TS_Start(BOARD_I2C_STMPE811);

	#if WITH_STMPE811_INTERRUPTS
		BOARD_STMPE811_INT_CONNECT();
	#endif /* WITH_STMPE811_INTERRUPTS */
	}
}

uint_fast8_t stmpe811_is_pressed(void) /* Return 1 if touch detection */
{
	return tscpresent &&
			((i2cperiph_read8(BOARD_I2C_STMPE811, STMPE811_REG_TSC_CTRL) & STMPE811_TS_CTRL_STATUS) >> STMPE811_TS_CTRL_STATUS_POS) != 0;
}

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811) */
