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
#include "gpio.h"

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811)
#include "stmpe811.h"

uint_fast8_t board_tsc_is_pressed(void) /* Return 1 if touch detection */
{
	return stmpe811_is_pressed();
}

// On AT070TN90 with touch screen attached Y coordinate increments from bottom to top, X from left to right
uint_fast8_t
board_tsc_getxy(uint_fast16_t * xr, uint_fast16_t * yr)
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

static void
stmpe811_handler(void)
{
	TP();
}

/* Назначить обработчик прерывания по единичному уровню для выхода прерываний от ST STMPE811 */
static void
stmpe811_sethandler(void)
{
	const portholder_t INMASK = 1uL << 1;	// JP0_1
	arm_hardware_jpio0_inputs(INMASK);
	arm_hardware_piojp0_onchangeinterrupt(INMASK, 0, ARM_SYSTEM_PRIORITY, stmpe811_handler);	// JP0_1 interrupt, level-sensitive
}

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_STMPE811) */

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911)
#include "gt911.h"

uint_fast8_t
board_tsc_getxy(uint_fast16_t * xr, uint_fast16_t * yr)
{
	static uint_fast16_t x = 0, y = 0;

	if (gt911_getXY(& x, & y))
	{
		* xr = x;
		* yr = y;
		return 1;
	}
	* xr = x;
	* yr = y;
	return 0;
}
#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911) */

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_FT5336)
#include "ft5336.h"

static TS_StateTypeDef ts_ft5336;

uint_fast8_t
board_tsc_getxy(uint_fast16_t * xr, uint_fast16_t * yr)
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

#if defined (TSC1_TYPE)

void board_tsc_initialize(void)
{
#if TSC1_TYPE == TSC_TYPE_GT911
	if (gt911_initialize(GOODIX_I2C_ADDR_BA))
		PRINTF("gt911 initialization succefful\n");
	else
		PRINTF("gt911 initialization error\n");
#endif /* TSC1_TYPE == TSC_TYPE_GT911 */

#if TSC1_TYPE == TSC_TYPE_STMPE811
	stmpe811_initialize();
#endif /* TSC1_TYPE == TSC_TYPE_STMPE811 */

#if TSC1_TYPE == TSC_TYPE_FT5336
	if (ft5336_Initialize(DIM_X, DIM_Y) == FT5336_I2C_INITIALIZED)
		PRINTF("ft5336 initialization succefful\n");
	else
	{
		PRINTF("ft5336 initialization error\n");
		ASSERT(0);
	}
#endif /* TSC1_TYPE == TSC_TYPE_FT5336 */
}
#endif /* defined (TSC1_TYPE) */
