/*
 * uart2.c
 *
 *	Работа с ГИРОНАВ БИНС Н1 - РЕГИСТРЫ ВНЕШНЕЙ КОРРЕКЦИИ
 */

#include "hardware.h"
#include "formats.h"
#include "bootloader.h"
#include "board.h"
#include "gpio.h"

#include "xhelper507.h"

#if WITHCTRLBOARDT507

// БИНС поправки
// RS-485 38400 8N1


#define STARTCRCVAL 0xFF	// начальное значение аккумулятора CRC протокола обмена

// Быстрый расчет следующего значения CRC
static unsigned calculateCRC8(unsigned v, unsigned wCRCWord)
{

    static const uint8_t wCRCTable[] =
    {
        0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97, 0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E,
        0x43, 0x72, 0x21, 0x10, 0x87, 0xB6, 0xE5, 0xD4, 0xFA, 0xCB, 0x98, 0xA9, 0x3E, 0x0F, 0x5C, 0x6D,
        0x86, 0xB7, 0xE4, 0xD5, 0x42, 0x73, 0x20, 0x11, 0x3F, 0x0E, 0x5D, 0x6C, 0xFB, 0xCA, 0x99, 0xA8,
        0xC5, 0xF4, 0xA7, 0x96, 0x01, 0x30, 0x63, 0x52, 0x7C, 0x4D, 0x1E, 0x2F, 0xB8, 0x89, 0xDA, 0xEB,
        0x3D, 0x0C, 0x5F, 0x6E, 0xF9, 0xC8, 0x9B, 0xAA, 0x84, 0xB5, 0xE6, 0xD7, 0x40, 0x71, 0x22, 0x13,
        0x7E, 0x4F, 0x1C, 0x2D, 0xBA, 0x8B, 0xD8, 0xE9, 0xC7, 0xF6, 0xA5, 0x94, 0x03, 0x32, 0x61, 0x50,
        0xBB, 0x8A, 0xD9, 0xE8, 0x7F, 0x4E, 0x1D, 0x2C, 0x02, 0x33, 0x60, 0x51, 0xC6, 0xF7, 0xA4, 0x95,
        0xF8, 0xC9, 0x9A, 0xAB, 0x3C, 0x0D, 0x5E, 0x6F, 0x41, 0x70, 0x23, 0x12, 0x85, 0xB4, 0xE7, 0xD6,
        0x7A, 0x4B, 0x18, 0x29, 0xBE, 0x8F, 0xDC, 0xED, 0xC3, 0xF2, 0xA1, 0x90, 0x07, 0x36, 0x65, 0x54,
        0x39, 0x08, 0x5B, 0x6A, 0xFD, 0xCC, 0x9F, 0xAE, 0x80, 0xB1, 0xE2, 0xD3, 0x44, 0x75, 0x26, 0x17,
        0xFC, 0xCD, 0x9E, 0xAF, 0x38, 0x09, 0x5A, 0x6B, 0x45, 0x74, 0x27, 0x16, 0x81, 0xB0, 0xE3, 0xD2,
        0xBF, 0x8E, 0xDD, 0xEC, 0x7B, 0x4A, 0x19, 0x28, 0x06, 0x37, 0x64, 0x55, 0xC2, 0xF3, 0xA0, 0x91,
        0x47, 0x76, 0x25, 0x14, 0x83, 0xB2, 0xE1, 0xD0, 0xFE, 0xCF, 0x9C, 0xAD, 0x3A, 0x0B, 0x58, 0x69,
        0x04, 0x35, 0x66, 0x57, 0xC0, 0xF1, 0xA2, 0x93, 0xBD, 0x8C, 0xDF, 0xEE, 0x79, 0x48, 0x1B, 0x2A,
        0xC1, 0xF0, 0xA3, 0x92, 0x05, 0x34, 0x67, 0x56, 0x78, 0x49, 0x1A, 0x2B, 0xBC, 0x8D, 0xDE, 0xEF,
        0x82, 0xB3, 0xE0, 0xD1, 0x46, 0x77, 0x24, 0x15, 0x3B, 0x0A, 0x59, 0x68, 0xFF, 0xCE, 0x9D, 0xAC,
    };

    return wCRCTable [(v ^ wCRCWord) & 0xFF];
}

// Очереди символов для обмена

// Очередь символов для передачи в канал обмена
static u8queue_t txq;

// передача символа в канал. Ожидание, если очередь заполнена
static int nmeaX_putc(int c)
{
	IRQL_t oldIrql;
	uint_fast8_t f;

	do {
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		f = uint8_queue_put(& txq, c);
		hardware_uart2_enabletx(1);
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

static void uartX_format(const char * format, ...)
{
	char b [256];
	int n, i;
	va_list	ap;
	va_start(ap, format);

	n = vsnprintf(b, sizeof b / sizeof b [0], format, ap);

	for (i = 0; i < n; ++ i)
		nmeaX_putc(b [i]);

	va_end(ap);
}

// callback по принятому символу. ничего не делаем
void user_uart2_onrxchar(uint_fast8_t c)
{
}

// callback по готовности последовательного порта к пердаче
void user_uart2_ontxchar(void * ctx)
{
	uint_fast8_t c;
	if (uint8_queue_get(& txq, & c))
	{
		hardware_uart2_tx(ctx, c);
		if (uint8_queue_empty(& txq))
			hardware_uart2_enabletx(0);
	}
	else
	{
		hardware_uart2_enabletx(0);
	}
}

// Передать символ с обновлением контрольного кода
static unsigned uartX_putc_crc8(int c, unsigned crc)
{
	nmeaX_putc(c);
	crc = calculateCRC8(c, crc);
	return crc;
}

// Передача массива с дополнением кодла контроля целостности сообщения
static void uartX_write_crc8(const uint8_t * buff, size_t n)
{
	unsigned crc = STARTCRCVAL;
	while (n --)
	{
		crc = uartX_putc_crc8(* buff ++, crc);
	}
	crc = uartX_putc_crc8(crc, crc);
	ASSERT(crc == 0);
}

// Сформировать int8 в передаваемом пакете
static unsigned mbuff_uint8(uint8_t * b, uint_fast8_t v)
{
	b [0] = v;
	return 1;
}

// Сформировать int32 в передаваемом пакете
static unsigned mbuff_uint32_LE(uint8_t * b, uint_fast32_t v)
{
	b [0] = v >> 0;
	b [1] = v >> 8;
	b [2] = v >> 16;
	b [3] = v >> 24;
	return 4;
}

// Сформировать float32 в передаваемом пакете
static unsigned mbuff_float32_LE(uint8_t * b, float v)
{
	union
	{
		float f;
		uint8_t b [sizeof (float)];
	} u;
	u.f = v;
	b [0] = u.b [0];
	b [1] = u.b [1];
	b [2] = u.b [2];
	b [3] = u.b [3];
	return 4;
}

// Сформировать float64 в передаваемом пакете
static unsigned mbuff_float64_LE(uint8_t * b, double v)
{
	union
	{
		double f;
		uint8_t b [sizeof (float)];
	} u;
	u.f = v;
	b [0] = u.b [0];
	b [1] = u.b [1];
	b [2] = u.b [2];
	b [3] = u.b [3];
	b [4] = u.b [4];
	b [5] = u.b [5];
	b [6] = u.b [6];
	b [7] = u.b [7];

	return 8;
}

// Типы регистров БИНС Н1 при обмене
enum regtypes
{
	REGT_UINT32,
	REGT_FLOAT32,
	REGT_FLOAT64,
};

// получить тип регистра по его адресующему коду
static enum regtypes getregtype(unsigned reg)
{
	switch (reg)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		return REGT_FLOAT64;
	default:	// 4..
		return REGT_FLOAT32;
	}
}

// register increment size
static int xbreginc(unsigned reg)
{
	switch (getregtype(reg))
	{
	case REGT_UINT32:
	case REGT_FLOAT32:
		return 1;
	case REGT_FLOAT64:
		return 2;
	}
	return 1;
}

// Получить Количество регистров для обмена мщ начального адреса и количества параметров
static unsigned calctregcount(unsigned reg, unsigned argcount)
{
	unsigned tregcount;

	for (tregcount = 0; argcount --; reg += xbreginc(reg))
		tregcount += xbreginc(reg);

	return tregcount;
}

void xbsetregEXTF(const double * pv)
{
	unsigned reg = 0;
	unsigned argcount = MAXPACKREGSEXT;
	const unsigned tregcount = calctregcount(reg, argcount);
	uint8_t b [4 + 8 * MAXPACKREGSEXT];
	unsigned n = 0;
	unsigned i;

	n += mbuff_uint8(b + n, 0xFB);	// preamble
	n += mbuff_uint8(b + n, GIRONAV_DEVADDR);	// device address
	n += mbuff_uint8(b + n, reg);	// register address
	n += mbuff_uint8(b + n, tregcount | 0x80);	// number of registers write

	for (i = 0; i < argcount && i < MAXPACKREGS; ++ i)
	{
		switch (getregtype(reg))
		{
		case REGT_FLOAT64:
			n += mbuff_float64_LE(b + n, pv [i]);
			break;
		case REGT_FLOAT32:
			n += mbuff_float32_LE(b + n, pv [i]);
			break;
		case REGT_UINT32:
			n += mbuff_uint32_LE(b + n, pv [i]);
			break;
		default:
			break;
		}
	}

	uartX_write_crc8(b, n);
}
//
//static dpcobj_t uart2_dpc_entry;
///* Функционирование USER MODE обработчиков */
//static void uart2_spool(void * ctx)
//{
//}

void user_uart2_initialize(void)
{
	static uint8_t txb [512];
	uint8_queue_init(& txq, txb, ARRAY_SIZE(txb));

	hardware_uart2_initialize(0, 38400, 8, 0, 0);
	hardware_uart2_set_speed(38400);
	hardware_uart2_enablerx(0);
	hardware_uart2_enabletx(0);

//	dpcobj_initialize(& uart2_dpc_entry, uart2_spool, NULL);
//	board_dpc_addentry(& uart2_dpc_entry, board_dpc_coreid());
}

#endif /* WITHCTRLBOARDT507 */
