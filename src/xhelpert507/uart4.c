/*
 * uart0.c
 *
 *  Created on: 21 сент. 2023 г.
 *      Author: User
 */

#include "hardware.h"

#if WITHCTRLBOARDT507

#include "formats.h"
#include "bootloader.h"
#include "board.h"
#include "gpio.h"

#include "xhelper507.h"
#include <ctype.h>
#include <string.h>

// управление
// отладка
// RS-485 9600 8N1

#define UARTSPEED 115200

static u8queue_t txq;
static u8queue_t rxq;


static const uint_fast32_t starttxmin = NTICKS(10);
static const uint_fast32_t starttxamx = NTICKS(100);
static volatile uint_fast32_t lasttxticks;

static int answerrequest;

static int answerrequested(void)
{
	int v;
	uint_fast32_t t;

	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
	t = lasttxticks;
	/* Проверка попадания во временное окно начала ответа мастеру */
	v = (t >= starttxmin && t < starttxamx) && answerrequest;

	LowerIrql(oldIrql);

	if (v)
		answerrequest = 0;

	return v;
}

static void serialtouts(void * ctx)
{
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
	uint_fast32_t v = lasttxticks;
	if (v < starttxamx)
	{
		lasttxticks = v + 1;
	}
	LowerIrql(oldIrql);
}

void user_uart4_onrxchar(uint_fast8_t c)
{
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
	uint8_queue_put(& rxq, c);
	lasttxticks = 0;
	LowerIrql(oldIrql);
}

void user_uart4_ontxchar(void * ctx)
{
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
	uint_fast8_t c;
	if (uint8_queue_get(& txq, & c))
	{
		hardware_uart4_tx(ctx, c);
		if (uint8_queue_empty(& txq))
			hardware_uart4_enabletx(0);
	}
	else
	{
		hardware_uart4_enabletx(0);
	}
	LowerIrql(oldIrql);
}

static int nmeaX_putc(int c)
{
	IRQL_t oldIrql;
	uint_fast8_t f;

	do {
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		f = uint8_queue_put(& txq, c);
		hardware_uart4_enabletx(1);
		LowerIrql(oldIrql);
	} while (! f);
	return c;
}

static void uartX_write(const uint8_t * buff, size_t n)
{
	while (n --)
	{
		const uint8_t c = * buff ++;
		nmeaX_putc(c);
	}
}

static float gpressure;

void xbsave_pressure(float v)
{
	gpressure = v;
}

static int positions [2];
void xbsave_position(unsigned id, int pos)
{
	switch (id)
	{
	case 1:
		positions [0] = pos;
		break;
	case 2:
		positions [1] = pos;
		break;
	}
}

static int nmeaX_putchar(int c)
{
	if (c == '\n')
		nmeaX_putchar('\r');

	nmeaX_putc(c);
	return c;
}

void nmeaX_puts_impl(const char * s, size_t len)
{
	while (len --)
	{
		const char c = * s ++;
		nmeaX_putchar(c);
	}
}

static uint_fast8_t calcxorv(
	const char * s,
	size_t len
	)
{
	uint_fast8_t r = '*';
	while (len --)
		r ^= (unsigned char) * s ++;
	return r & 0xff;
}

/* Передача строки без '$' в начале и с завершающим  '*'
 * Ведущий символ '$' и контрольный код формируются тут.
 */
static void nmea_send(const char * body, size_t len)
{
	static const char hex [] = "0123456789ABCDEF";
	unsigned xorv = calcxorv(body, len);

	nmeaX_putchar('$');
	nmeaX_puts_impl(body, len);
	nmeaX_putchar(hex [(xorv >> 4) & 0x0F]);
	nmeaX_putchar(hex [(xorv >> 0) & 0x0F]);
	nmeaX_putchar('\n');
}

#define NMEA_CHN 1
#define NMEA_CHARS 16

enum nmea_states
{
	NMEAST_INITIALIZED = 0,
	NMEAST_OPENED,	// встретился символ '$'
	NMEAST_CHSHI,	// прём старшего символа контрольной суммы
	NMEAST_CHSLO,	// приём младшего символа контрольной суммы
	//
	NMEAST_COUNTSTATES
};


enum
{
	PARAM_NAMETAG,


	//
	NMEA_PARAMS = 5
};

static uint_fast8_t nmea_state [NMEA_CHN]; //= NMEAST_INITIALIZED;
static uint_fast8_t nmea_checksum[NMEA_CHN];
static uint_fast8_t nmea_chsval[NMEA_CHN];
static uint_fast8_t nmea_param[NMEA_CHN];		// номер принимаемого параметра в строке - количество принятых реально
static uint_fast16_t nmea_chars[NMEA_CHN];		// количество символов, помещённых в буфер

static char nmea_buff [NMEA_CHN] [NMEA_PARAMS] [NMEA_CHARS];

/* Завершён прием строки и проверена контрорльная сумма. начинаем разбирать. */
static void nmea_done(unsigned chn)
{
	if (0)
	{

	}
	else if (
		//nmea_param [chn] >= NMEA_PARAMS &&
		strcmp(nmea_buff [chn] [PARAM_NAMETAG], "TLREQ") == 0 &&
		1)
	{
		// разбор управляющих паарметров маяка и цифровых выходов
	}
	else if (
		//nmea_param [chn] >= NMEA_PARAMS &&
		strcmp(nmea_buff [chn] [PARAM_NAMETAG], "TLPOS") == 0 &&
		1)
	{
		// разбор управляющих паарметров маяка и цифровых выходов
		long id = strtol(nmea_buff [chn][PARAM_NAMETAG + 1], NULL, 10);
		long pos = strtol(nmea_buff [chn][PARAM_NAMETAG + 2], NULL, 10);
		if (pos > 1023)
			pos = 1023;
		else if (pos < - 1023)
			pos = - 1023;
		xbsave_setpos(id, pos);
	}
	answerrequest = 1;
}

static unsigned char hex2int(uint_fast8_t c)
{
	if (isdigit((unsigned char) c))
		return c - '0';
	if (isupper((unsigned char) c))
		return c - 'A' + 10;
	if (islower((unsigned char) c))
		return c - 'a' + 10;
	return 255;
}

static void nmeaX_parsechar(uint_fast8_t c, unsigned chn)
{
	if (c == '$')
	{
		nmea_checksum [chn] = '*';
		nmea_state [chn] = NMEAST_OPENED;
		nmea_param [chn] = 0;		// номер принимаемого параметра в строке
		nmea_chars [chn] = 0;		// количество символов, помещённых в буфер
		return;
	}
	switch (nmea_state [chn])
	{
	case NMEAST_INITIALIZED:
		break;

	case NMEAST_OPENED:
		nmea_checksum [chn] ^= c;
		if (c == ',')
		{
			// закрываем буфер параметра, переходим к следующему параметру
			nmea_buff [chn] [nmea_param [chn]][nmea_chars [chn]] = '\0';
			nmea_param [chn] += 1;
			nmea_chars [chn] = 0;
		}
		else if (c == '*')
		{
			// закрываем буфер параметра, переходим к следующему параметру
			nmea_buff [chn] [nmea_param [chn]][nmea_chars [chn]] = '\0';
			nmea_param [chn] += 1;
			// переходим к приёму контрольной суммы
			nmea_state [chn] = NMEAST_CHSHI;
		}
		else if (nmea_param [chn] < NMEA_PARAMS && nmea_chars [chn] < (NMEA_CHARS - 1))
		{
			nmea_buff [chn] [nmea_param [chn]][nmea_chars [chn]] = c;
			nmea_chars [chn] += 1;
			//stat_l1 = stat_l1 > nmea_chars ? stat_l1 : nmea_chars;
		}
		else
			nmea_state [chn] = NMEAST_INITIALIZED;	// при ошибках формата строки
		break;

	case NMEAST_CHSHI:
		nmea_chsval [chn] = hex2int(c) * 16;
		nmea_state [chn] = NMEAST_CHSLO;
		break;

	case NMEAST_CHSLO:
		nmea_state [chn] = NMEAST_INITIALIZED;
		if (
			//nmea_checksum [chn] == (nmea_chsval [chn] + hex2int(c)) &&	// Эта строка комментируется для отключения проверки контрольной суммы команды
			1)
		{
			nmea_done(chn);	/* Завершён прием строки и проверена контрорльная сумма. начинаем разбирать. */
		}
		break;
	}
}

void uart4_spool(void)
{
#if ! (WITHDEBUG && WITHDEBUG_USART4)
	uint_fast8_t c;
	uint_fast8_t f;
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
	f = uint8_queue_get(& rxq, & c);
	LowerIrql(oldIrql);

	if (f)
	{
		nmeaX_parsechar(c & 0xFF, 0);
	}

	if (answerrequested())
	{
		answerrequest = 0;
		static char state [512];
		size_t len = local_snprintf_P(state, sizeof state / sizeof state [0],
				"TLSTS,%f,%d,%d,%d"
				"*",
				gpressure,
				positions [0],
				positions [1],
				//
				1	// active flag
			);
		nmea_send(state, len);

	}
#endif /* ! (WITHDEBUG && WITHDEBUG_USART4) */
}


static ticker_t uart3_pkg_ticker;

void user_uart4_initialize(void)
{
#if ! (WITHDEBUG && WITHDEBUG_USART4)
	uint8_queue_init(& rxq);
	uint8_queue_init(& txq);

	hardware_uart4_initialize(0, UARTSPEED, 8, 0, 0);
	hardware_uart4_set_speed(UARTSPEED);
	hardware_uart4_enablerx(1);
	hardware_uart4_enabletx(0);

	ticker_initialize(& uart3_pkg_ticker, 1, serialtouts, NULL);
	ticker_add(& uart3_pkg_ticker);

#endif /* ! (WITHDEBUG && WITHDEBUG_USART4) */
}

#endif /* WITHCTRLBOARDT507 */

