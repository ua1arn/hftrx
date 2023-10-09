/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Поддержка MAXIM DS1305EN RTC clock chip with SPI interface
// SERMODE pin tied high.
//

#include "hardware.h"
#include "formats.h"
#include "board.h"

#if defined (RTC1_TYPE) && RTC1_TYPE == RTC_TYPE_ZYNQ_MP

#include "xrtcpsu.h"

static XRtcPsu xczu_rtc;

/* возврат не-0 если требуется начальная загрузка значений */
uint_fast8_t board_rtc_chip_initialize(void)
{
	XRtcPsu_Config * rtccfg = XRtcPsu_LookupConfig(XPAR_XRTCPSU_0_DEVICE_ID);
	XRtcPsu_CfgInitialize(& xczu_rtc, rtccfg, rtccfg->BaseAddr);
	u32 Status = XRtcPsu_SelfTest(& xczu_rtc);
	if (Status != XST_SUCCESS) {
		PRINTF("rtcpsu init fail %ld\n", Status);
		ASSERT(0);
	}
	return 0;
}

void board_rtc_getdatetime(
	uint_fast16_t * year,
	uint_fast8_t * month,	// 01-12
	uint_fast8_t * day,
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * seconds
	)
{
	XRtcPsu_DT dt;
	uint32_t time = XRtcPsu_GetCurrentTime(& xczu_rtc);
	XRtcPsu_SecToDateTime(time, & dt);

	* year = dt.Year;
	* month = dt.Month;
	* day = dt.Day;
	* hour = dt.Hour;
	* minute = dt.Min;
	* seconds = dt.Sec;
}

void board_rtc_setdatetime(
	uint_fast16_t year,
	uint_fast8_t month,
	uint_fast8_t dayofmonth,
	uint_fast8_t hours,
	uint_fast8_t minutes,
	uint_fast8_t seconds
	)
{
	XRtcPsu_DT dt;
	dt.Year = year;
	dt.Month = month;
	dt.Day = dayofmonth;
	dt.Hour = hours;
	dt.Min = minutes;
	dt.Sec = seconds;
	XRtcPsu_SetTime(& xczu_rtc, XRtcPsu_DateTimeToSec(& dt));
	board_rtc_chip_initialize();
}

#endif /* defined (RTC1_TYPE) && RTC1_TYPE == RTC_TYPE_ZYNQ_MP */

