#include "hardware.h"
#include "board.h"
#include "formats.h"

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_FT5336)

#include "ft5336.h"

static uint16_t tsXBoundary, tsYBoundary;
static uint8_t  tsOrientation;
static uint8_t  I2cAddress = FT5336_I2C_SLAVE_ADDRESS;

static ft5336_handle_TypeDef ft5336_handle = { FT5336_I2C_NOT_INITIALIZED, 0, 0 };

uint_fast8_t ft5336_read(uint_fast8_t reg)
{
	uint_fast8_t k = 0, data = 0;

	i2c_start(I2cAddress);
	i2c_write(reg);

	i2c_start(I2cAddress | 1);
	i2c_read((uint8_t *) & data, I2C_READ_ACK_NACK);

	return data;
}

void ft5336_Init(uint16_t DeviceAddr)
{
	local_delay_ms(200);
	ft5336_handle.i2cInitialized = FT5336_I2C_INITIALIZED;
}

uint16_t ft5336_ReadID(uint16_t DeviceAddr)
{
  uint8_t ucReadId = 0;
  uint8_t nbReadAttempts = 0;
  uint8_t bFoundDevice = 0; /* Device not found by default */

  for(nbReadAttempts = 0; nbReadAttempts < 3; nbReadAttempts ++)
  {
    /* Read register FT5336_CHIP_ID_REG as DeviceID detection */
	  ucReadId = ft5336_read(FT5336_CHIP_ID_REG);
	  if (ucReadId == FT5336_ID_VALUE)
		  return ucReadId;
  }

  return 0;
}

uint8_t ft5336_TS_DetectTouch(uint16_t DeviceAddr)
{
  uint8_t nbTouch = 0;

  /* Read register FT5336_TD_STAT_REG to check number of touches detection */
  nbTouch = ft5336_read(FT5336_TD_STAT_REG);
  nbTouch &= FT5336_TD_STAT_MASK;

  if(nbTouch > FT5336_MAX_DETECTABLE_TOUCH)
  {
    /* If invalid number of touch detected, set it to zero */
    nbTouch = 0;
  }

  /* Update ft5336 driver internal global : current number of active touches */
  ft5336_handle.currActiveTouchNb = nbTouch;

  /* Reset current active touch index on which to work on */
  ft5336_handle.currActiveTouchIdx = 0;

  return(nbTouch);
}

void ft5336_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y)
{
  uint8_t ucReadData = 0;
  static uint16_t coord;
  uint8_t regAddressXLow = 0;
  uint8_t regAddressXHigh = 0;
  uint8_t regAddressYLow = 0;
  uint8_t regAddressYHigh = 0;

  if(ft5336_handle.currActiveTouchIdx < ft5336_handle.currActiveTouchNb)
  {
    switch(ft5336_handle.currActiveTouchIdx)
    {
    case 0 :
      regAddressXLow  = FT5336_P1_XL_REG;
      regAddressXHigh = FT5336_P1_XH_REG;
      regAddressYLow  = FT5336_P1_YL_REG;
      regAddressYHigh = FT5336_P1_YH_REG;
      break;

    case 1 :
      regAddressXLow  = FT5336_P2_XL_REG;
      regAddressXHigh = FT5336_P2_XH_REG;
      regAddressYLow  = FT5336_P2_YL_REG;
      regAddressYHigh = FT5336_P2_YH_REG;
      break;

    case 2 :
      regAddressXLow  = FT5336_P3_XL_REG;
      regAddressXHigh = FT5336_P3_XH_REG;
      regAddressYLow  = FT5336_P3_YL_REG;
      regAddressYHigh = FT5336_P3_YH_REG;
      break;

    case 3 :
      regAddressXLow  = FT5336_P4_XL_REG;
      regAddressXHigh = FT5336_P4_XH_REG;
      regAddressYLow  = FT5336_P4_YL_REG;
      regAddressYHigh = FT5336_P4_YH_REG;
      break;

    case 4 :
      regAddressXLow  = FT5336_P5_XL_REG;
      regAddressXHigh = FT5336_P5_XH_REG;
      regAddressYLow  = FT5336_P5_YL_REG;
      regAddressYHigh = FT5336_P5_YH_REG;
      break;

    case 5 :
      regAddressXLow  = FT5336_P6_XL_REG;
      regAddressXHigh = FT5336_P6_XH_REG;
      regAddressYLow  = FT5336_P6_YL_REG;
      regAddressYHigh = FT5336_P6_YH_REG;
      break;

    case 6 :
      regAddressXLow  = FT5336_P7_XL_REG;
      regAddressXHigh = FT5336_P7_XH_REG;
      regAddressYLow  = FT5336_P7_YL_REG;
      regAddressYHigh = FT5336_P7_YH_REG;
      break;

    case 7 :
      regAddressXLow  = FT5336_P8_XL_REG;
      regAddressXHigh = FT5336_P8_XH_REG;
      regAddressYLow  = FT5336_P8_YL_REG;
      regAddressYHigh = FT5336_P8_YH_REG;
      break;

    case 8 :
      regAddressXLow  = FT5336_P9_XL_REG;
      regAddressXHigh = FT5336_P9_XH_REG;
      regAddressYLow  = FT5336_P9_YL_REG;
      regAddressYHigh = FT5336_P9_YH_REG;
      break;

    case 9 :
      regAddressXLow  = FT5336_P10_XL_REG;
      regAddressXHigh = FT5336_P10_XH_REG;
      regAddressYLow  = FT5336_P10_YL_REG;
      regAddressYHigh = FT5336_P10_YH_REG;
      break;

    default :
      break;

    } /* end switch(ft5336_handle.currActiveTouchIdx) */

    /* Read low part of X position */
    ucReadData = ft5336_read(regAddressXLow);
    coord = (ucReadData & FT5336_TOUCH_POS_LSB_MASK) >> FT5336_TOUCH_POS_LSB_SHIFT;

    /* Read high part of X position */
    ucReadData = ft5336_read(regAddressXHigh);
    coord |= ((ucReadData & FT5336_TOUCH_POS_MSB_MASK) >> FT5336_TOUCH_POS_MSB_SHIFT) << 8;

    /* Send back ready X position to caller */
    *X = coord;

    /* Read low part of Y position */
    ucReadData = ft5336_read(regAddressYLow);
    coord = (ucReadData & FT5336_TOUCH_POS_LSB_MASK) >> FT5336_TOUCH_POS_LSB_SHIFT;

    /* Read high part of Y position */
    ucReadData = ft5336_read(regAddressYHigh);
    coord |= ((ucReadData & FT5336_TOUCH_POS_MSB_MASK) >> FT5336_TOUCH_POS_MSB_SHIFT) << 8;

    /* Send back ready Y position to caller */
    *Y = coord;

    ft5336_handle.currActiveTouchIdx++; /* next call will work on next touch */

  }
}

// ********************************************************************************************************************

uint8_t ft5336_Initialize(uint16_t ts_SizeX, uint16_t ts_SizeY)
{
  uint8_t status;
  tsXBoundary = ts_SizeX;
  tsYBoundary = ts_SizeY;

  /* Read ID and verify if the touch screen driver is ready */
  ft5336_Init(I2cAddress);
  if(ft5336_ReadID(I2cAddress) == FT5336_ID_VALUE)
  {
    /* Initialize the TS driver structure */
	status = FT5336_I2C_INITIALIZED;
    tsOrientation = TS_SWAP_XY;
  }
  else
  {
    status = FT5336_I2C_NOT_INITIALIZED;
  }

  return status;
}

uint8_t ft5336_GetState(TS_StateTypeDef *TS_State)
{
  static uint32_t _x[FT5336_MAX_DETECTABLE_TOUCH] = {0, 0};
  static uint32_t _y[FT5336_MAX_DETECTABLE_TOUCH] = {0, 0};
  uint8_t ts_status = FT5336_STATUS_OK;
  uint16_t x[FT5336_MAX_DETECTABLE_TOUCH];
  uint16_t y[FT5336_MAX_DETECTABLE_TOUCH];
  uint16_t brute_x[FT5336_MAX_DETECTABLE_TOUCH];
  uint16_t brute_y[FT5336_MAX_DETECTABLE_TOUCH];
  uint16_t x_diff;
  uint16_t y_diff;
  uint32_t index;

  /* Check and update the number of touches active detected */
  TS_State->touchDetected = ft5336_TS_DetectTouch(I2cAddress);

  if(TS_State->touchDetected)
  {
    for(index=0; index < TS_State->touchDetected; index++)
    {
      /* Get each touch coordinates */
    	ft5336_TS_GetXY(I2cAddress, &(brute_x[index]), &(brute_y[index]));

      if(tsOrientation == TS_SWAP_NONE)
      {
        x[index] = brute_x[index];
        y[index] = brute_y[index];
      }

      if(tsOrientation & TS_SWAP_X)
      {
        x[index] = 4096 - brute_x[index];
      }

      if(tsOrientation & TS_SWAP_Y)
      {
        y[index] = 4096 - brute_y[index];
      }

      if(tsOrientation & TS_SWAP_XY)
      {
        y[index] = brute_x[index];
        x[index] = brute_y[index];
      }

      x_diff = x[index] > _x[index]? (x[index] - _x[index]): (_x[index] - x[index]);
      y_diff = y[index] > _y[index]? (y[index] - _y[index]): (_y[index] - y[index]);

      if ((x_diff + y_diff) > 5)
      {
        _x[index] = x[index];
        _y[index] = y[index];
      }

      if(I2cAddress == FT5336_I2C_SLAVE_ADDRESS)
      {
        TS_State->touchX[index] = x[index];
        TS_State->touchY[index] = y[index];
      }
      else
      {
        /* 2^12 = 4096 : indexes are expressed on a dynamic of 4096 */
        TS_State->touchX[index] = (tsXBoundary * _x[index]) >> 12;
        TS_State->touchY[index] = (tsYBoundary * _y[index]) >> 12;
      }
    }
  }

  return (ts_status);
}

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_FT5336) */
