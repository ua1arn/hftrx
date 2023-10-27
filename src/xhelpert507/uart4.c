/*
 * uart4.c
 *
 *  Работа с каналом внешнего управления
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

// Очередь символов для передачи в канал обмена
static u8queue_t txq;
// Очередь принятых симвоов из канала обменна
static u8queue_t rxq;

/* параметры окна времени начала ответа по каналу управления */
static const uint_fast32_t starttxmin = NTICKS(40);
static const uint_fast32_t starttxamx = NTICKS(200);
static volatile uint_fast32_t lasttxticks;	/* таймер после последнего принятого символа - окно ответа */

/* Коды запроошенных ответов NMEA */
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

/* Получение и сброс запрошенного кода ответа. Признак возвращается одтин раз в интервале не менее starttxmin и не более starttxamx
 *
 */
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

// обработка таймера окна ответа
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

// callback по принятому символу. сохранить в очередь для обработки в user level
// сброс таймера окна ответа
void user_uart4_onrxchar(uint_fast8_t c)
{
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
	uint8_queue_put(& rxq, c);
	lasttxticks = 0;	/* сброс таймера окна ответа */
	LowerIrql(oldIrql);
}

// callback по готовности последовательного порта к пердаче
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

// передача символа в канал. Ожидание, если очередь заполнена
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

// Передача в канал указанного массива. Ожидание, если очередь заполнена
static void uartX_write(const uint8_t * buff, size_t n)
{
	while (n --)
	{
		const uint8_t c = * buff ++;
		nmeaX_putc(c);
	}
}

static float gpressure;

// сохранить измеренное давление
void xbsave_pressure(float v)
{
	//PRINTF("Pressure=%f, depth=%f\n", v, v * 101.97162005);
	gpressure = v;
}

// Желаемая позиция для каждой рулевой машинки (-2047..+2047)
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

// передача символа в канал. Ожидание, если очередь заполнена
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

#define NMEA_CHN 1		// колчиество каналов управления
#define NMEA_CHARS 16	// максимальный размер поля (включая завершающий символ '\0').

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
	PARAM_NAMETAG,	// номер поля с кодом сообщения (первое после символа $).


	//
	NMEA_PARAMS = 15	// максимальное количество полей в запросах
};

static uint_fast8_t nmea_state [NMEA_CHN]; //= NMEAST_INITIALIZED;
static uint_fast8_t nmea_checksum[NMEA_CHN];	// текущее значение накопленной контрольной суммы
static uint_fast8_t nmea_chsval[NMEA_CHN];
static uint_fast8_t nmea_param[NMEA_CHN];		// номер принимаемого поля в строке - количество принятых символов
static uint_fast16_t nmea_chars[NMEA_CHN];		// количество символов, помещённых в буфер данного поля

static char nmea_buff [NMEA_CHN] [NMEA_PARAMS] [NMEA_CHARS];	// буфер разбираемого в данный момент сообщения

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
		xbsetregEXTF(setvalF);	// установить группу регистров БИНС Н1 по каналу РЕГИСТРЫ ВНЕШНЕЙ КОРРЕКЦИИ
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
		// разбор управляющих парметров положения рулей
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


// кеш считанных из БИНС Н1 значений регистров
static double binsmirrF [256];	// для float/double регистров
static int binsmirrI [256];		// для int32 регистров

// Сохранение значения регистра - callback из разборщика пакетов на проиёме от БИНС Н1
void xbsavebins_float32(unsigned reg, float v)
{
	if (reg < ARRAY_SIZE(binsmirrF))
		binsmirrF [reg] = v;
}

// Сохранение значения регистра - callback из разборщика пакетов на проиёме от БИНС Н1
void xbsavebins_float64(unsigned reg, double v)
{
	if (reg < ARRAY_SIZE(binsmirrF))
		binsmirrF [reg] = v;
}

// Сохранение значения регистра - callback из разборщика пакетов на проиёме от БИНС Н1
void xbsavebins_int32(unsigned reg, int32_t v)
{
	if (reg < ARRAY_SIZE(binsmirrI))
		binsmirrI [reg] = v;
}

// Кеш параметрв полученных от магнитного компаса - callback из разборщика пакетов на проиёме
static double groll, gpitch, gjaw;

// callback из разборщика пакетов на проиёме от магнитного компаса
void xbsavemagn(double roll, double pitch, double jaw)
{
	groll = roll;
	gpitch = pitch;
	gjaw = jaw;
	//PRINTF("roll=%g,pitch=%g,jav=%g\n", roll, pitch, jaw);
}

// Буфер для формирования ответа в канал управления
static char state [1024];

// Формирование ответа - чтение int32 параметров из БИНС Н1 (используются значения из кеша)
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

// Формирование ответа - чтение float/double параметров из БИНС Н1 (используются значения из кеша)
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

// Формирование ответа - чтение предопределенной шруппы параметров из БИНС Н1 (используются значения из кеша)
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

static dpcobj_t uart4_dpc_entry;
/* Функционирование USER MODE обработчиков */
static void uart4_spool(void * ctx)
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

#if ! (WITHDEBUG && WITHDEBUG_UART4)

	hardware_uart4_initialize(0, UARTSPEED, 8, 0, 0);
	hardware_uart4_set_speed(UARTSPEED);
	hardware_uart4_enablerx(1);
	hardware_uart4_enabletx(0);

	ticker_initialize(& uart3_pkg_ticker, 1, serialtouts, NULL);
	ticker_add(& uart3_pkg_ticker);

	static const char msg [] = "Hello!\n";
	nmeaX_puts_impl(msg, ARRAY_SIZE(msg));

#endif /* ! (WITHDEBUG && WITHDEBUG_UART4) */

	dpcobj_initialize(& uart4_dpc_entry, uart4_spool, NULL);
	board_dpc_addentry(& uart4_dpc_entry);
}

#endif /* WITHCTRLBOARDT507 */

