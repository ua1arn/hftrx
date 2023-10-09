/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints

//#define WITHGNSS	1	/* В конфигурации трбуется разбирать поток NMEA от навигационного модуля */
//#define WITHGNSS_1PPS	1 	/* обрабатываются прерывания от аппаратного входа 1PPS	*/

//#define WITHGNSS_NMEA	1

#if WITHGNSS

#include "board.h"
#include <string.h>
#include <ctype.h>

// ****************
// NMEA parser

enum nmea_states
{
	NMEAST_INITIALIZED,
	NMEAST_OPENED,	// встретился символ '$'
	NMEAST_CHSHI,	// прём старшего символа контрольной суммы
	NMEAST_CHSLO,	// приём младшего символа контрольной суммы
	//
	NMEAST_COUNTSTATES

};


typedef struct timeholder
{
	uint_fast8_t ms;
	uint_fast8_t seconds;
	uint_fast8_t minutes;
	uint_fast8_t hours;
#if defined (RTC1_TYPE)
	uint_fast8_t day;
	uint_fast8_t month;
	uint_fast8_t year;
#endif /* defined (RTC1_TYPE) */
	uint_fast8_t valid;
} timeholder_t;

#if WITHLFM

// исправить время на следующую секунду
void time_next(
	volatile timeholder_t * t
	)
{
	const uint_fast8_t a = 1;
	if ((t->seconds += a) >= 60)
	{
		t->seconds -= 60;
		if ((t->minutes += 1) >= 60)
		{
			t->minutes -= 60;
			if ((t->hours += 1) >= 24)
			{
				t->hours -= 24;
			}
		}
	}
}
// исправить время на предидущую секунду
void time_prev(
	timeholder_t * t
	)
{
	const uint_fast8_t a = 1;
	if ((t->seconds -= a) >= 60)
	{
		t->seconds += 60;
		if ((t->minutes -= 1) >= 60)
		{
			t->minutes += 60;
			if ((t->hours -= 1) >= 24)
			{
				t->hours += 24;
			}
		}
	}
}
#endif /* WITHLFM */

static uint_fast8_t nmea_state = NMEAST_INITIALIZED;
static uint_fast8_t nmea_checksum;
static uint_fast8_t nmea_chsval;
static uint_fast8_t nmea_param;		// номер принимаемого параметра в строке
static uint_fast8_t nmea_chars;		// количество символов, помещённых в буфер

#define NMEA_PARAMS 20
#define NMEA_CHARS 16	// really need 11
static char nmea_buff [NMEA_PARAMS] [NMEA_CHARS];
static volatile timeholder_t nmea_time;
//static timeholder_t th;
static volatile uint_fast8_t secondticks;
static uint_fast8_t rtc_nmea_updated = 0;


static uint_fast8_t local_isdigit(char c)
{
	//return isdigit((unsigned char) c) != 0;
	return c >= '0' && c <= '9';
}

static unsigned char hex2int(uint_fast8_t c)
{
	if (local_isdigit((unsigned char) c))
		return c - '0';
	if (isupper((unsigned char) c))
		return c - 'A' + 10;
	if (islower((unsigned char) c))
		return c - 'a' + 10;
	return 0;
}

void update_rtc_by_nmea_time(void)
{
#if defined (RTC1_TYPE)
	if (! rtc_nmea_updated && nmea_time.valid)
	{
		rtc_nmea_updated = 1;
		// todo: добавить в меню выбор часового пояса
		board_rtc_setdatetime(nmea_time.year, nmea_time.month, nmea_time.day, nmea_time.hours + 3, nmea_time.minutes, nmea_time.seconds);
	}
#endif /* defined (RTC1_TYPE) */
}

void nmeagnss_parsechar(uint_fast8_t c)
{
	//dbg_putchar(c);
	switch (nmea_state)
	{
	case NMEAST_INITIALIZED:
		if (c == '$')
		{
			nmea_checksum = '*';
			nmea_state = NMEAST_OPENED;
			nmea_param = 0;		// номер принимаемого параметра в строке
			nmea_chars = 0;		// количество символов, помещённых в буфер
		}
		break;

	case NMEAST_OPENED:
		nmea_checksum ^= c;
		if (c == ',')
		{
			// закрываем буфер параметра, переходим к следующему параметру
			nmea_buff [nmea_param][nmea_chars] = '\0';
			nmea_param += 1;
			nmea_chars = 0;
		}
		else if (c == '*')
		{
			// закрываем буфер параметра, переходим к следующему параметру
			nmea_buff [nmea_param][nmea_chars] = '\0';
			nmea_param += 1;
			// переходим к приёму контрольной суммы
			nmea_state = NMEAST_CHSHI;
		}
		else if (nmea_param < NMEA_PARAMS && nmea_chars < (NMEA_CHARS - 1))
		{
			nmea_buff [nmea_param][nmea_chars] = c;
			nmea_chars += 1;
			//stat_l1 = stat_l1 > nmea_chars ? stat_l1 : nmea_chars;
		}
		else
			nmea_state = NMEAST_INITIALIZED;	// при ошибках формата строки
		break;

	case NMEAST_CHSHI:
		nmea_chsval = hex2int(c) * 16;
		nmea_state = NMEAST_CHSLO;
		break;

	case NMEAST_CHSLO:
		nmea_state = NMEAST_INITIALIZED;
		if (nmea_checksum == (nmea_chsval + hex2int(c)))
		{
			if (nmea_param > 2 &&
				strcmp(nmea_buff [0], "GPRMC") == 0 &&
#if defined (RTC1_TYPE) && (RTC1_TYPE != RTC_TYPE_GPS)				// при наличии выделенного RTC через GPS обновляется время
				strcmp(nmea_buff [2], "A") == 0 &&					// а если в качестве RTC выступает GPS, подводка не требуется
#endif /* defined (RTC1_TYPE) && (RTC1_TYPE != RTC_TYPE_GPS) */
				1)
			{
				// разбор времени
				const char * const s = nmea_buff [1];	// начало буфера, где лежит строка времени.формата 044709.00 или 044709.000
				nmea_time.hours = (s [0] - '0') * 10 + (s [1] - '0');
				nmea_time.minutes = (s [2] - '0') * 10 + (s [3] - '0');
				nmea_time.seconds = (s [4] - '0') * 10 + (s [5] - '0');
				nmea_time.ms = 0; //_strtoul_r(& treent, s + 7, NULL, 10);
#if defined (RTC1_TYPE)
				const char * const d = nmea_buff [9];
				nmea_time.day = (d [0] - '0') * 10 + (d [1] - '0');
				nmea_time.month = (d [2] - '0') * 10 + (d [3] - '0');
				nmea_time.year = (d [4] - '0') * 10 + (d [5] - '0');
#endif /* defined (RTC1_TYPE) */
				nmea_time.valid = 1;

#if WITHLFM
				time_next(& nmea_time);	// какое время надо будет поставить для установки в следующий PPS
				update_rtc_by_nmea_time();
#endif /* WITHLFM */
			}
		}
		break;
	}
}

/* вызывается из обработчика прерываний */
// произошла потеря символа (символов) при получении данных с компорта
void nmeagnss_rxoverflow(void)
{
	nmea_state = NMEAST_INITIALIZED;
}
/* вызывается из обработчика прерываний */
void nmeagnss_disconnect(void)
{
	nmea_state = NMEAST_INITIALIZED;
}

void nmeagnss_initialize(void)
{
	const uint_fast32_t baudrate = UINT32_C(115200);
	nmea_state = NMEAST_INITIALIZED;

#if ! LINUX_SUBSYSTEM

	HARDWARE_NMEA_INITIALIZE(baudrate);
	HARDWARE_NMEA_SET_SPEED(baudrate);
	HARDWARE_NMEA_ENABLERX(1);
	HARDWARE_NMEA_ENABLETX(0);
	nmea_parser0_init();

#endif /*  ! LINUX_SUBSYSTEM */
}

//static timeholder_t th;
// Обработчик вызывается при приходе очередного импульса PPS
void
RAMFUNC_NONILINE
spool_nmeapps(void)
{
	//th = nmea_time;
#if WITHLFM
	if (board_islfmmode() != 0 && nmea_time.valid && islfmstart(nmea_time.minutes * 60 + nmea_time.seconds))
	{
		lfm_run();
	}
#endif /* WITHLFM */
}

#if defined (RTC1_TYPE) && (RTC1_TYPE == RTC_TYPE_GPS)

uint_fast8_t board_rtc_chip_initialize(void)
{
	return 0;
}

void board_rtc_getdatetime(
	uint_fast16_t * year,
	uint_fast8_t * month,	// 01-12
	uint_fast8_t * dayofmonth,
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * seconds
	)
{
	* seconds = nmea_time.seconds;
	* minute = nmea_time.minutes;
	* hour = nmea_time.hours;
	* dayofmonth = nmea_time.day;
	* month = nmea_time.month;
	* year = 2000 + nmea_time.year;
}

void board_rtc_gettime(
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * seconds
	)
{
	* seconds = nmea_time.seconds;
	* minute = nmea_time.minutes;
	* hour = nmea_time.hours;
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
}

#endif /* defined (RTC1_TYPE) && (RTC1_TYPE == RTC_TYPE_GPS) */


//static char nmea_time_str [9];

void gui_get_nmea_time(char * p, size_t sz)
{
	//strcpy(p, nmea_time_str);
#if WITHNMEA && WITHLFM
	local_snprintf_P(p, sz, "%02d:%02d:%02d", nmea_time.hours, nmea_time.minutes, nmea_time.seconds);
#endif /* WITHNMEA && WITHLFM */
}

//void gui_gnssupdate(void)
//{
//#if WITHNMEA && WITHLFM
//	local_snprintf_P(nmea_time_str, ARRAY_SIZE(nmea_time_str), "%02d:%02d:%02d", nmea_time.hours, nmea_time.minutes, nmea_time.seconds);
//#endif /* WITHNMEA && WITHLFM */
//}


/* инициализация парсера GNSS потока NMEA */
void gnss_initialize(void)
{
	// nmeagnss_initialize();
}

#endif /* WITHGNSS */
