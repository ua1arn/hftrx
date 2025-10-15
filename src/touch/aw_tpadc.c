/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Allwinner F133/t113-s3 resistive touch screen controller

#include "hardware.h"

#if TSC1_TYPE == TSC_TYPE_AWTPADC

#include "formats.h"
#include "clocks.h"
#include "touch.h"

// https://github.com/RT-Thread/rt-thread/blob/master/bsp/allwinner/libraries/sunxi-hal/hal/source/tpadc/hal_tpadc.c

/* вызывается при разрешённых прерываниях. */
void board_tsc_initialize(void)
{
	CCU->TPADC_CLK_REG = 0x00 * (UINT32_C(1) << 31);	// 000: HOSC
	CCU->TPADC_CLK_REG = (UINT32_C(1) << 31);	// TPADC_CLK_GATING

	CCU->TPADC_BGR_REG |= (UINT32_C(1) << 0);	// Gating clock to TPADC
	CCU->TPADC_BGR_REG &= ~ (UINT32_C(1) << 16);	// Assert TPADC RESET
	CCU->TPADC_BGR_REG |= (UINT32_C(1) << 16);	// De-assert TPADC RESET
	(void) CCU->TPADC_BGR_REG;

	PRINTF("board_tsc_initialize: allwnr_t113_get_tpadc_freq()=%u Hz\n", (unsigned) allwnr_t113_get_tpadc_freq());
	PRINTF("TPADC->TP_CTRL_REG0=%08X\n", (unsigned) TPADC->TP_CTRL_REG0);
	TPADC->TP_CTRL_REG0 =
		0x0F * (UINT32_C(1) << 24) |
		1 * (UINT32_C(1) << 23) |	// ADC_FIRST_DLY_MODE
		0x0FFF * (UINT32_C(1) << 0) |
		0;
	PRINTF("TPADC->TP_CTRL_REG0=%08X\n", (unsigned) TPADC->TP_CTRL_REG0);

	TPADC->TP_INT_FIFO_CTRL_REG =
		(2 - 1) * (UINT32_C(1) << 8) |
		0;
	TPADC->TP_CTRL_REG1 =
		0 * (UINT32_C(1) << 4) | // TP_MODE_SELECT 0: TP
		1 * (UINT32_C(1) << 5) | // TP_EN
		//0x0F * (UINT32_C(1) << 0) | // ADC_CHAN3_SELECT..ADC_CHAN0_SELECT
		0;
//	TPADC->TP_CTRL_REG2 =
//		8 * (UINT32_C(1) << 28) |
//		0;

	TPADC->TP_CTRL_REG3 =
		//1 * (UINT32_C(1) << 2) |	// FILTER_EN
		0x00 * (UINT32_C(1) << 0) |
		0;

	TPADC->TP_CTRL_REG1 |= (UINT32_C(1) << 7); 	// TOUCH_PAN_CALI_EN
	while ((TPADC->TP_CTRL_REG1 & (UINT32_C(1) << 7)) != 0)
		;
	TPADC->TP_CTRL_REG1 |= (UINT32_C(1) << 5); 	// TP_EN

	board_tsc_calibration();	// использовать результаты калибровки
}

uint_fast8_t
board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr, uint_fast16_t * zr)
{
	* zr = 0;	// stub
	for (;;)
	{
		const uint_fast32_t fifo_stat = TPADC->TP_INT_FIFO_STAT_REG;
		if ((fifo_stat & (UINT32_C(1) << 16)) != 0 && ((fifo_stat >> 8) & 0x3F) >= 2)
		{
			const uint_fast32_t v1 = TPADC->TP_DATA_REG & 0xFFF;
			const uint_fast32_t v2 = TPADC->TP_DATA_REG & 0xFFF;
			PRINTF("fifo_stat=%08X: %03X %03X\n", (unsigned) fifo_stat, (unsigned) v1, (unsigned) v2);
			* xr = v1;
			* yr = v2;
			return 1;
		}
	}
	return 0;
}

// результат калибровки
#if (DIM_X == 800) && (DIM_Y == 480)
static tPoint calpoints [TSCCALIBPOINTS] =
{
	{ 1694, 1011, }, /* point 0 */
	{ 3811, 891, }, /* point 1 */
	{ 699, 3363, }, /* point 2 */
	{ 3440, 3340, }, /* point 3 */
	{ 1963, 2236, }, /* point 4 */
};
#else
#error Provide calibration data
#endif

tPoint *
board_tsc_getcalpoints(void)
{
	return calpoints;
}

#endif /* TSC1_TYPE == TSC_TYPE_AWTPADC */
