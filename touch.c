/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "audio.h"
#include "synthcalcs.h"
#include "keyboard.h"

#include "display/display.h"
#include "formats.h"
#include "inc/gpio.h"
#include "inc/spi.h"

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811)

static uint_fast8_t tscpresetnt;

#if TSC_TYPE_STMPE811_USE_SPI
#else /* TSC_TYPE_STMPE811_USE_SPI */
#define BOARD_I2C_STMPE811	(0x82)           /* STMPE811 Controller A0=0: 0x82, A0=1: 0x88 */
#endif /* TSC_TYPE_STMPE811_USE_SPI */
/* chip IDs */
#define STMPE811_ID                     0x0811

/* Identification registers & System Control */
#define STMPE811_REG_CHP_ID         	0x00
#define STMPE811_REG_ID_VER             0x02

/* Global interrupt Enable bit */
#define STMPE811_GIT_EN                 0x01

/* IO expander functionalities */
#define STMPE811_ADC_FCT                0x01
#define STMPE811_TS_FCT                 0x02
#define STMPE811_IO_FCT                 0x04
#define STMPE811_TEMPSENS_FCT           0x08

/* Global Interrupts definitions */
#define STMPE811_GIT_IO                 0x80  /* IO interrupt                   */
#define STMPE811_GIT_ADC                0x40  /* ADC interrupt                  */
#define STMPE811_GIT_TEMP               0x20  /* Not implemented                */
#define STMPE811_GIT_FE                 0x10  /* FIFO empty interrupt           */
#define STMPE811_GIT_FF                 0x08  /* FIFO full interrupt            */
#define STMPE811_GIT_FOV                0x04  /* FIFO overflowed interrupt      */
#define STMPE811_GIT_FTH                0x02  /* FIFO above threshold interrupt */
#define STMPE811_GIT_TOUCH              0x01  /* Touch is detected interrupt    */
#define STMPE811_ALL_GIT                0x1F  /* All global interrupts          */
#define STMPE811_TS_IT                  (STMPE811_GIT_TOUCH | STMPE811_GIT_FTH |  STMPE811_GIT_FOV | STMPE811_GIT_FF | STMPE811_GIT_FE) /* Touch screen interrupts */

/* General Control Registers */
#define STMPE811_REG_SYS_CTRL1          0x03
#define STMPE811_REG_SYS_CTRL2          0x04
#define STMPE811_REG_SPI_CFG            0x08

/* Interrupt system registers */
#define STMPE811_REG_INT_CTRL           0x09
#define STMPE811_REG_INT_EN             0x0A
#define STMPE811_REG_INT_STA            0x0B
#define STMPE811_REG_IO_INT_EN          0x0C
#define STMPE811_REG_IO_INT_STA         0x0D

/* IO Registers */
#define STMPE811_REG_IO_SET_PIN         0x10
#define STMPE811_REG_IO_CLR_PIN         0x11
#define STMPE811_REG_IO_MP_STA          0x12
#define STMPE811_REG_IO_DIR             0x13
#define STMPE811_REG_IO_ED              0x14
#define STMPE811_REG_IO_RE              0x15
#define STMPE811_REG_IO_FE              0x16
#define STMPE811_REG_IO_AF              0x17

/* ADC Registers */
#define STMPE811_REG_ADC_INT_EN         0x0E
#define STMPE811_REG_ADC_INT_STA        0x0F
#define STMPE811_REG_ADC_CTRL1          0x20
#define STMPE811_REG_ADC_CTRL2          0x21
#define STMPE811_REG_ADC_CAPT           0x22
#define STMPE811_REG_ADC_DATA_CH0       0x30
#define STMPE811_REG_ADC_DATA_CH1       0x32
#define STMPE811_REG_ADC_DATA_CH2       0x34
#define STMPE811_REG_ADC_DATA_CH3       0x36
#define STMPE811_REG_ADC_DATA_CH4       0x38
#define STMPE811_REG_ADC_DATA_CH5       0x3A
#define STMPE811_REG_ADC_DATA_CH6       0x3B
#define STMPE811_REG_ADC_DATA_CH7       0x3C

/* Touch Screen Registers */
#define STMPE811_REG_TSC_CTRL           0x40
#define STMPE811_REG_TSC_CFG            0x41
#define STMPE811_REG_WDM_TR_X           0x42
#define STMPE811_REG_WDM_TR_Y           0x44
#define STMPE811_REG_WDM_BL_X           0x46
#define STMPE811_REG_WDM_BL_Y           0x48
#define STMPE811_REG_FIFO_TH            0x4A
#define STMPE811_REG_FIFO_STA           0x4B
#define STMPE811_REG_FIFO_SIZE          0x4C
#define STMPE811_REG_TSC_DATA_X         0x4D
#define STMPE811_REG_TSC_DATA_Y         0x4F
#define STMPE811_REG_TSC_DATA_Z         0x51
#define STMPE811_REG_TSC_DATA_XYZ       0x52
#define STMPE811_REG_TSC_FRACT_XYZ      0x56
#define STMPE811_REG_TSC_DATA_INC       0x57
#define STMPE811_REG_TSC_DATA_NON_INC   0xD7
#define STMPE811_REG_TSC_I_DRIVE        0x58
#define STMPE811_REG_TSC_SHIELD         0x59

/* Touch Screen Pins definition */
//#define STMPE811_TOUCH_YD               IO_PIN_1
//#define STMPE811_TOUCH_XD               IO_PIN_2
//#define STMPE811_TOUCH_YU               IO_PIN_3
//#define STMPE811_TOUCH_XU               IO_PIN_4
#define STMPE811_TOUCH_IO_ALL           (uint32_t)(STMPE811_PIN_1 | STMPE811_PIN_2 | STMPE811_PIN_3 | STMPE811_PIN_4)

/* IO Pins definition */
#define STMPE811_PIN_0                  0x01
#define STMPE811_PIN_1                  0x02
#define STMPE811_PIN_2                  0x04
#define STMPE811_PIN_3                  0x08
#define STMPE811_PIN_4                  0x10
#define STMPE811_PIN_5                  0x20
#define STMPE811_PIN_6                  0x40
#define STMPE811_PIN_7                  0x80
#define STMPE811_PIN_ALL                0xFF

/* IO Pins directions */
#define STMPE811_DIRECTION_IN           0x00
#define STMPE811_DIRECTION_OUT          0x01

/* IO IT types */
#define STMPE811_TYPE_LEVEL             0x00
#define STMPE811_TYPE_EDGE              0x02

/* IO IT polarity */
#define STMPE811_POLARITY_LOW           0x00
#define STMPE811_POLARITY_HIGH          0x04

/* IO Pin IT edge modes */
#define STMPE811_EDGE_FALLING           0x01
#define STMPE811_EDGE_RISING            0x02

/* TS registers masks */
#define STMPE811_TS_CTRL_ENABLE         0x01
#define STMPE811_TS_CTRL_STATUS         0x80
#define STMPE811_TS_CTRL_STATUS_POS     0x07

int32_t i2cperiph_readN(uint_fast8_t d_adr, uint_fast8_t r_adr, uint32_t r_byte, uint_fast8_t * r_buffer)
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
	uint_fast8_t v;

	i2c_start(DeviceAddr);
	i2c_write_withrestart(reg);		// Register 135
	i2c_start(DeviceAddr | 1);
	i2c_read(& v, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
	return v;
}

uint_fast16_t i2cperiph_read16(uint_fast8_t DeviceAddr, uint_fast8_t reg)
{
	uint_fast8_t v [2];

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

static uint_fast8_t stmpe811_TS_GetXYZ(
	uint_fast16_t * X,
	uint_fast16_t * Y,
	uint_fast8_t * Z
	)
{
	uint_fast8_t DeviceAddr = BOARD_I2C_STMPE811;
	uint_fast8_t dataXYZ [4];
	uint_fast32_t vdataXY;
	uint_fast8_t sta = i2cperiph_read8(DeviceAddr, STMPE811_REG_FIFO_STA);
	if ((sta & 0x10) == 0)
		return 0;
	i2cperiph_readN(DeviceAddr, STMPE811_REG_TSC_DATA_NON_INC, sizeof dataXYZ / sizeof dataXYZ [0], dataXYZ);

	/* Calculate positions values */
	vdataXY =
			((uint_fast32_t) dataXYZ [0] << 16) |
			((uint_fast32_t) dataXYZ [1] << 8) |
			((uint_fast32_t) dataXYZ [2] << 0);
	* X = (vdataXY >> 12) & 0x00000FFF;
	* Y = (vdataXY >> 0) & 0x00000FFF;
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

/* top left raw data values */
static uint_fast16_t xrawmin = 70;
static uint_fast16_t yrawmin = 3890;
/* bottom right raw data values */
static uint_fast16_t xrawmax = 3990;
static uint_fast16_t yrawmax = 150;

// On AT070TN90 with touch screen attached Y coordinate increments from bottom to top, X from left to right
uint_fast8_t board_tsc_getxy(uint_fast16_t * xr, uint_fast16_t * yr)
{
	uint_fast16_t x, y;
	uint_fast8_t z;
	if (tscpresetnt && stmpe811_TS_GetXYZ(& x, & y, & z))
	{
		* xr = tcsnormalize(x, xrawmin, xrawmax, DIM_X - 1);
		* yr = tcsnormalize(y, yrawmin, yrawmax, DIM_Y - 1);
		return 1;
	}
	return 0;
}

void stmpe811_initialize(void)
{
	unsigned chip_id;
	unsigned ver;

	chip_id = i2cperiph_read16(BOARD_I2C_STMPE811, STMPE811_REG_CHP_ID);
	ver = i2cperiph_read8(BOARD_I2C_STMPE811, STMPE811_REG_ID_VER);
	debug_printf_P(PSTR("stmpe811_initialize: chip_id=%04X, expected %04X, ver=%02x\r\n"), chip_id, STMPE811_ID, ver);

	tscpresetnt = chip_id == STMPE811_ID;

	if (tscpresetnt != 0 && ver == 0x03)
	{
		stmpe811_TS_Start(BOARD_I2C_STMPE811);
	}
}

uint_fast8_t board_tsc_is_pressed (void) /* Return 1 if touch detection */
{
	return (i2cperiph_read8(BOARD_I2C_STMPE811, STMPE811_REG_TSC_CTRL) & STMPE811_TS_CTRL_STATUS) >> STMPE811_TS_CTRL_STATUS_POS;
}

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811) */

#if defined (TSC1_TYPE)

void board_tsc_initialize(void)
{
#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811)
	stmpe811_initialize();
#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811) */
}
#endif /* defined (TSC1_TYPE) */
