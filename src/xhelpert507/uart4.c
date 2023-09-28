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

#define UARTSPEED 9600

static u8queue_t txq;
static u8queue_t rxq;


static const uint_fast32_t starttxmin = NTICKS(10);
static const uint_fast32_t starttxamx = NTICKS(100);
static volatile uint_fast32_t lasttxticks;

enum answtype
{
	ANSW_NONE,
	ANSW_BINSF,	// Запрос float/double
	ANSW_BINSI,	// запрос integer
	ANSW_BINSSETF,	// установка float/double
	ANSW_BINSSETI,	// установка запрос integer
	ANSW_BINSGROUP,
	ANSW_MAGN,
	ANSW_RUDDER
};

static enum answtype answerrequest = ANSW_NONE;
static unsigned binsreg;
static unsigned binsargcount;

static enum answtype answerrequested(void)
{
	int v;
	uint_fast32_t t;

	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
	t = lasttxticks;
	/* Проверка попадания во временное окно начала ответа мастеру */
	v = (t >= starttxmin && t < starttxamx);

	LowerIrql(oldIrql);

	if (v)
	{
		int f;
		f = answerrequest;
		answerrequest = ANSW_NONE;
		return f;
	}
	return ANSW_NONE;
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
	NMEA_PARAMS = 15
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
		strcmp(nmea_buff [chn] [PARAM_NAMETAG], "TLR0") == 0 &&
		1)
	{
		// БИНС
		// получение float/double
		binsreg = strtoul(nmea_buff [chn][PARAM_NAMETAG + 1], NULL, 10);
		binsargcount = strtoul(nmea_buff [chn][PARAM_NAMETAG + 2], NULL, 10);
		if (binsreg < 255 && binsargcount >= 1 && binsargcount <= 10)
			answerrequest = ANSW_BINSF;
	}
	else if (
		//nmea_param [chn] >= NMEA_PARAMS &&
		strcmp(nmea_buff [chn] [PARAM_NAMETAG], "TLR1") == 0 &&
		1)
	{
		// БИНС
		// получение integer
		binsreg = strtoul(nmea_buff [chn][PARAM_NAMETAG + 1], NULL, 10);
		binsargcount = strtoul(nmea_buff [chn][PARAM_NAMETAG + 2], NULL, 10);
		if (binsreg < 255 && binsargcount >= 1 && binsargcount <= 10)
			answerrequest = ANSW_BINSI;
	}
	else if (
		//nmea_param [chn] >= NMEA_PARAMS &&
		strcmp(nmea_buff [chn] [PARAM_NAMETAG], "TLR2") == 0 &&
		1)
	{
		// БИНС
		// установка float/double
		binsreg = strtoul(nmea_buff [chn][PARAM_NAMETAG + 1], NULL, 10);
		binsargcount = strtoul(nmea_buff [chn][PARAM_NAMETAG + 2], NULL, 10);
		if (binsreg < 255 && binsargcount >= 1 && binsargcount <= MAXPACKREGS)
		{
			answerrequest = ANSW_BINSSETF;
			static double setvalF [MAXPACKREGS];		// значения для установки
			unsigned i;
			for (i = 0; i < binsargcount; ++ i)
			{
				setvalF [i] = strtod(nmea_buff [chn][PARAM_NAMETAG + 3 + i], NULL);
			}
			xbsetregF(binsreg, binsargcount, setvalF);	// установить группу регистров
		}
	}
	else if (
		//nmea_param [chn] >= NMEA_PARAMS &&
		strcmp(nmea_buff [chn] [PARAM_NAMETAG], "TLR3") == 0 &&
		1)
	{
		// БИНС
		// установка integer
		binsreg = strtoul(nmea_buff [chn][PARAM_NAMETAG + 1], NULL, 10);
		binsargcount = strtoul(nmea_buff [chn][PARAM_NAMETAG + 2], NULL, 10);
		if (binsreg < 255 && binsargcount >= 1 && binsargcount <= MAXPACKREGS)
		{
			answerrequest = ANSW_BINSSETI;
			static long setvalI [MAXPACKREGS];		// значения для установки
			unsigned i;
			for (i = 0; i < binsargcount; ++ i)
			{
				setvalI [i] = strtoul(nmea_buff [chn][PARAM_NAMETAG + 3 + i], NULL, 10);
			}
			xbsetregI(binsreg, binsargcount, setvalI);	// установить группу регистров
		}
	}
	else if (
		//nmea_param [chn] >= NMEA_PARAMS &&
		strcmp(nmea_buff [chn] [PARAM_NAMETAG], "TLR4") == 0 &&
		1)
	{
		// БИНС
		// получение группы параметров
		answerrequest = ANSW_BINSGROUP;
	}
	else if (
		nmea_param [chn] >= (MAXPACKREGSEXT + 1) &&
		strcmp(nmea_buff [chn] [PARAM_NAMETAG], "TLR5") == 0 &&
		1)
	{
		// БИНС
		// установка float/double параметров РЕГИСТРЫ ВНЕШНЕЙ КОРРЕКЦИИ

		answerrequest = MAXPACKREGSEXT;
		static double setvalF [MAXPACKREGSEXT];		// значения для установки
		unsigned i;
		for (i = 0; i < MAXPACKREGSEXT; ++ i)
		{
			setvalF [i] = strtod(nmea_buff [chn][PARAM_NAMETAG + 1 + i], NULL);
		}
		xbsetregEXTF(setvalF);	// установить группу регистров РЕГИСТРЫ ВНЕШНЕЙ КОРРЕКЦИИ
		answerrequest = ANSW_BINSGROUP;
	}
	else if (
		//nmea_param [chn] >= NMEA_PARAMS &&
		strcmp(nmea_buff [chn] [PARAM_NAMETAG], "TLMQ") == 0 &&
		1)
	{
		// компас
		answerrequest = ANSW_MAGN;
	}
	else if (
		//nmea_param [chn] >= NMEA_PARAMS &&
		strcmp(nmea_buff [chn] [PARAM_NAMETAG], "TLPS") == 0 &&
		1)
	{
		// разбор управляющих паарметров положения рулей
		long pos1 = strtol(nmea_buff [chn][PARAM_NAMETAG + 1], NULL, 10);
		long pos2 = strtol(nmea_buff [chn][PARAM_NAMETAG + 2], NULL, 10);

		if (pos1 > 1023)
			pos1 = 1023;
		else if (pos1 < - 1023)
			pos1 = - 1023;

		if (pos2 > 1023)
			pos2 = 1023;
		else if (pos2 < - 1023)
			pos2 = - 1023;

		xbsave_setpos(1, pos1);
		xbsave_setpos(2, pos2);
		answerrequest = ANSW_RUDDER;
	}
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
static double binsmirrF [256];
static int binsmirrI [256];

void xbsavebins_float32(unsigned reg, float v)
{
	if (reg < ARRAY_SIZE(binsmirrF))
		binsmirrF [reg] = v;
}

void xbsavebins_float64(unsigned reg, double v)
{
	if (reg < ARRAY_SIZE(binsmirrF))
		binsmirrF [reg] = v;
}


void xbsavebins_int32(unsigned reg, int32_t v)
{
	if (reg < ARRAY_SIZE(binsmirrI))
		binsmirrI [reg] = v;
}

static double groll, gpitch, gjaw;

void xbsavemagn(double roll, double pitch, double jaw)
{
	groll = roll;
	gpitch = pitch;
	gjaw = jaw;
	PRINTF("roll=%g,pitch=%g,jav=%g\n", roll, pitch, jav);
}

static char state [1024];

static void answerbinsI(void)
{
	size_t len;
	switch (binsargcount)
	{
	case 1:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARI,%u,%u,%u"
				"*",
				binsreg, binsargcount,
				binsmirrI [binsreg + 0]
			);
		nmea_send(state, len);
		break;
	case 2:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARI,%u,%u,%u,%u"
				"*",
				binsreg, binsargcount,
				binsmirrI [binsreg + 0],
				binsmirrI [binsreg + 1]
			);
		nmea_send(state, len);
		break;
	case 3:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARI,%u,%u,%u,%u,%u"
				"*",
				binsreg, binsargcount,
				binsmirrI [binsreg + 0],
				binsmirrI [binsreg + 1],
				binsmirrI [binsreg + 2]
			);
		nmea_send(state, len);
		break;
	case 4:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARI,%u,%u,%u,%u,%u,%u"
				"*",
				binsreg, binsargcount,
				binsmirrI [binsreg + 0],
				binsmirrI [binsreg + 1],
				binsmirrI [binsreg + 2],
				binsmirrI [binsreg + 3]
			);
		nmea_send(state, len);
		break;
	case 5:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARI,%u,%u,%u,%u,%u,%u,%u"
				"*",
				binsreg, binsargcount,
				binsmirrI [binsreg + 0],
				binsmirrI [binsreg + 1],
				binsmirrI [binsreg + 2],
				binsmirrI [binsreg + 3],
				binsmirrI [binsreg + 4]
			);
		nmea_send(state, len);
		break;
	case 6:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARI,%u,%u,%u,%u,%u,%u,%u,%u"
				"*",
				binsreg, binsargcount,
				binsmirrI [binsreg + 0],
				binsmirrI [binsreg + 1],
				binsmirrI [binsreg + 2],
				binsmirrI [binsreg + 3],
				binsmirrI [binsreg + 4],
				binsmirrI [binsreg + 5]
			);
		nmea_send(state, len);
		break;
	case 7:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARI,%u,%u,%u,%u,%u,%u,%u,%u,%u"
				"*",
				binsreg, binsargcount,
				binsmirrI [binsreg + 0],
				binsmirrI [binsreg + 1],
				binsmirrI [binsreg + 2],
				binsmirrI [binsreg + 3],
				binsmirrI [binsreg + 4],
				binsmirrI [binsreg + 5],
				binsmirrI [binsreg + 6]
			);
		nmea_send(state, len);
		break;
	case 8:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARI,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u"
				"*",
				binsreg, binsargcount,
				binsmirrI [binsreg + 0],
				binsmirrI [binsreg + 1],
				binsmirrI [binsreg + 2],
				binsmirrI [binsreg + 3],
				binsmirrI [binsreg + 4],
				binsmirrI [binsreg + 5],
				binsmirrI [binsreg + 6],
				binsmirrI [binsreg + 7]
			);
		nmea_send(state, len);
		break;
	case 9:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARI,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u"
				"*",
				binsreg, binsargcount,
				binsmirrI [binsreg + 0],
				binsmirrI [binsreg + 1],
				binsmirrI [binsreg + 2],
				binsmirrI [binsreg + 3],
				binsmirrI [binsreg + 4],
				binsmirrI [binsreg + 5],
				binsmirrI [binsreg + 6],
				binsmirrI [binsreg + 7],
				binsmirrI [binsreg + 8]
			);
		nmea_send(state, len);
		break;
	case 10:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARI,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u"
				"*",
				binsreg, binsargcount,
				binsmirrI [binsreg + 0],
				binsmirrI [binsreg + 1],
				binsmirrI [binsreg + 2],
				binsmirrI [binsreg + 3],
				binsmirrI [binsreg + 4],
				binsmirrI [binsreg + 5],
				binsmirrI [binsreg + 6],
				binsmirrI [binsreg + 7],
				binsmirrI [binsreg + 8],
				binsmirrI [binsreg + 9]
			);
		nmea_send(state, len);
		break;
	}

}

static void answerbinsF(void)
{
	size_t len;
	switch (binsargcount)
	{
	case 1:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARF,%u,%u,%g"
				"*",
				binsreg, binsargcount,
				binsmirrF [binsreg + 0]
			);
		nmea_send(state, len);
		break;
	case 2:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARF,%u,%u,%g,%g"
				"*",
				binsreg, binsargcount,
				binsmirrF [binsreg + 0],
				binsmirrF [binsreg + 1]
			);
		nmea_send(state, len);
		break;
	case 3:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARF,%u,%u,%g,%g,%g"
				"*",
				binsreg, binsargcount,
				binsmirrF [binsreg + 0],
				binsmirrF [binsreg + 1],
				binsmirrF [binsreg + 2]
			);
		nmea_send(state, len);
		break;
	case 4:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARF,%u,%u,%g,%g,%g,%g"
				"*",
				binsreg, binsargcount,
				binsmirrF [binsreg + 0],
				binsmirrF [binsreg + 1],
				binsmirrF [binsreg + 2],
				binsmirrF [binsreg + 3]
			);
		nmea_send(state, len);
		break;
	case 5:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARF,%u,%u,%g,%g,%g,%g,%g"
				"*",
				binsreg, binsargcount,
				binsmirrF [binsreg + 0],
				binsmirrF [binsreg + 1],
				binsmirrF [binsreg + 2],
				binsmirrF [binsreg + 3],
				binsmirrF [binsreg + 4]
			);
		nmea_send(state, len);
		break;
	case 6:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARF,%u,%u,%g,%g,%g,%g,%g,%g"
				"*",
				binsreg, binsargcount,
				binsmirrF [binsreg + 0],
				binsmirrF [binsreg + 1],
				binsmirrF [binsreg + 2],
				binsmirrF [binsreg + 3],
				binsmirrF [binsreg + 4],
				binsmirrF [binsreg + 5]
			);
		nmea_send(state, len);
		break;
	case 7:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARF,%u,%u,%g,%g,%g,%g,%g,%g,%g"
				"*",
				binsreg, binsargcount,
				binsmirrF [binsreg + 0],
				binsmirrF [binsreg + 1],
				binsmirrF [binsreg + 2],
				binsmirrF [binsreg + 3],
				binsmirrF [binsreg + 4],
				binsmirrF [binsreg + 5],
				binsmirrF [binsreg + 6]
			);
		nmea_send(state, len);
		break;
	case 8:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARF,%u,%u,%g,%g,%g,%g,%g,%g,%g,%g"
				"*",
				binsreg, binsargcount,
				binsmirrF [binsreg + 0],
				binsmirrF [binsreg + 1],
				binsmirrF [binsreg + 2],
				binsmirrF [binsreg + 3],
				binsmirrF [binsreg + 4],
				binsmirrF [binsreg + 5],
				binsmirrF [binsreg + 6],
				binsmirrF [binsreg + 7]
			);
		nmea_send(state, len);
		break;
	case 9:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARF,%u,%u,%g,%g,%g,%g,%g,%g,%g,%g,%g"
				"*",
				binsreg, binsargcount,
				binsmirrF [binsreg + 0],
				binsmirrF [binsreg + 1],
				binsmirrF [binsreg + 2],
				binsmirrF [binsreg + 3],
				binsmirrF [binsreg + 4],
				binsmirrF [binsreg + 5],
				binsmirrF [binsreg + 6],
				binsmirrF [binsreg + 7],
				binsmirrF [binsreg + 8]
			);
		nmea_send(state, len);
		break;
	case 10:
		len = local_snprintf_P(state, ARRAY_SIZE(state),
				"TARF,%u,%u,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g"
				"*",
				binsreg, binsargcount,
				binsmirrF [binsreg + 0],
				binsmirrF [binsreg + 1],
				binsmirrF [binsreg + 2],
				binsmirrF [binsreg + 3],
				binsmirrF [binsreg + 4],
				binsmirrF [binsreg + 5],
				binsmirrF [binsreg + 6],
				binsmirrF [binsreg + 7],
				binsmirrF [binsreg + 8],
				binsmirrF [binsreg + 9]
			);
		nmea_send(state, len);
		break;
	}
}

static void answerbinsgroup(void)
{
    //    1 Акселерометр X ! 143
    //    2 Акселерометр Y ! 144
    //    3 Акселерометр Z ! 145
    //    4 Угловая скорость X ! 146
    //    5 Угловая скорость Y ! 147
    //    6 Угловая скорость Z ! 148
    //    7 Дифферент ! 155
    //    8 Крен ! 156
    //    9 Курс ! 154 HEADING_PITCH_ROLL
    //    10 Восточный вектор скорости ! 170 EAST_NORTH_VERTICAL_VELOCITY
    //    11 Северный вектор скорости ! 171
    //    12 Вертикальная скорость ! 172
    //    13 Расчетная лат (широта) ! 173 OUT_LAT
    //    14 Расчетная лон (долгота) ! 175 OUT_LON
    //    15 Расчетная высота ! 177 OUT_HEI
    //    16 Давление в корпусе ! 152 BAR_PRESS
    //    17 Магнитометр X ! 149
    //    18 Магнитометр Y ! 150
    //    19 Магнитометр Z ! 151
    //

    size_t len = local_snprintf_P(state, ARRAY_SIZE(state),
            "TARQ,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g"
            "*",
            binsmirrF [143], //    1 Акселерометр X ! 143
            binsmirrF [144], //    2 Акселерометр Y ! 144
            binsmirrF [145], //    3 Акселерометр Z ! 145
            binsmirrF [146], //    4 Угловая скорость X ! 146
            binsmirrF [147], //    5 Угловая скорость Y ! 147
            binsmirrF [148], //    6 Угловая скорость Z ! 148
            binsmirrF [155], //    7 Дифферент ! 155
            binsmirrF [156], //    8 Крен ! 156
            binsmirrF [154], //    9 Курс ! 154 HEADING_PITCH_ROLL
            binsmirrF [170], //    10 Восточный вектор скорости ! 170 EAST_NORTH_VERTICAL_VELOCITY
            binsmirrF [171], //    11 Северный вектор скорости ! 171
            binsmirrF [172], //    12 Вертикальная скорость ! 172
            binsmirrF [173], //    13 Расчетная лат (широта) ! 173 OUT_LAT
            binsmirrF [175], //    14 Расчетная лон (долгота) ! 175 OUT_LON
            binsmirrF [177], //    15 Расчетная высота ! 177 OUT_HEI
            binsmirrF [152], //    16 Давление в корпусе ! 152 BAR_PRESS
            binsmirrF [149], //    17 Магнитометр X ! 149
            binsmirrF [150], //    18 Магнитометр Y ! 150
            binsmirrF [151] //    19 Магнитометр Z ! 151
            //
        );
    nmea_send(state, len);
}

void uart4_spool(void)
{
	size_t len;
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

	switch (answerrequested())
	{
	case ANSW_NONE:
		return;
	case ANSW_RUDDER:
		{
			size_t len = local_snprintf_P(state, ARRAY_SIZE(state),
					"TAPS,%d,%d"
					"*",
					positions [0],
					positions [1]
					//
				);
			nmea_send(state, len);
		}
		break;
	case ANSW_MAGN:
		{
			size_t len = local_snprintf_P(state, ARRAY_SIZE(state),
					"TAMS,%g,%g,%g,%g"
					"*",
					groll,
					gpitch,
					gjaw,
					gpressure
					//
				);
			nmea_send(state, len);
		}
		break;
	case ANSW_BINSF:
	case ANSW_BINSSETF:
		answerbinsF();
		break;
	case ANSW_BINSI:
	case ANSW_BINSSETI:
		answerbinsI();
		break;
    case ANSW_BINSGROUP:
    	answerbinsgroup();
        break;
	}
}


static ticker_t uart3_pkg_ticker;

void user_uart4_initialize(void)
{
	static uint8_t rxb [256];
	uint8_queue_init(& rxq, rxb, ARRAY_SIZE(rxb));
	static uint8_t txb [2048];
	uint8_queue_init(& txq, txb, ARRAY_SIZE(txb));

#if ! (WITHDEBUG && WITHDEBUG_USART4)

	hardware_uart4_initialize(0, UARTSPEED, 8, 0, 0);
	hardware_uart4_set_speed(UARTSPEED);
	hardware_uart4_enablerx(1);
	hardware_uart4_enabletx(0);

	ticker_initialize(& uart3_pkg_ticker, 1, serialtouts, NULL);
	ticker_add(& uart3_pkg_ticker);

	static const char msg [] = "Hello!\n";
	nmeaX_puts_impl(msg, ARRAY_SIZE(msg));

#endif /* ! (WITHDEBUG && WITHDEBUG_USART4) */
}

#endif /* WITHCTRLBOARDT507 */

