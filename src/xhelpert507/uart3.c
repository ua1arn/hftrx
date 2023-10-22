/*
 * uart3.c
 *
 *	Работа с ГИРОНАВ БИНС Н1 - ИНФОРМАЦИОННЫЙ ПРОТОКОЛ
 */

#include "hardware.h"
#include "formats.h"
#include "bootloader.h"
#include "board.h"
#include "gpio.h"

#include "xhelper507.h"

#if WITHCTRLBOARDT507

// БИНС основной
// RS-422, 921600 8N1

#define PERIODSPOOL 200	// период опроса, мс
#define RXTOUT 50		// конец пакета на приеме подразумевается по интервалу, мс

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
		hardware_uart3_enabletx(1);
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

// Элемент очереди принятых пакетов
typedef struct rxlist
{
	LIST_ENTRY item;
	uint8_t buff [128];
	unsigned count;
	unsigned crc;
} rxlist_t;

static LIST_ENTRY rxlistfree;	// свободные буферы принятых пакетов
static LIST_ENTRY rxlistready;	// заполненные буферы принятых пакетов - для обработки
static rxlist_t * rxp = NULL;	// буфер принимаемого в данный момент пакета

static void uartX_rxlist_initilize(void)
{
	unsigned i;
	static rxlist_t lists [3];

	InitializeListHead(& rxlistfree);
	InitializeListHead(& rxlistready);
	for (i = 0; i < ARRAY_SIZE(lists); ++ i)
	{
		rxlist_t * const p = & lists [i];
		InsertTailList(& rxlistfree, & p->item);
	}
}

// установить текущим буфером приема следующий свободный.
// Если свободных нет - самый старый из принятых
static void nextlist(void)
{
	if (! IsListEmpty(& rxlistfree))
	{
		LIST_ENTRY * v = RemoveHeadList(& rxlistfree);
		rxlist_t * const p = CONTAINING_RECORD(v, rxlist_t, item);
		p->crc = 0xFFFF;
		p->count = 0;
		//
		rxp = p;
	}
	else if (! IsListEmpty(& rxlistready))
	{
		LIST_ENTRY * v = RemoveHeadList(& rxlistready);
		rxlist_t * const p = CONTAINING_RECORD(v, rxlist_t, item);
		p->crc = STARTCRCVAL;
		p->count = 0;
		//
		rxp = p;
	}
	else
	{
		rxp = NULL;
	}
}


// callback по готовности последовательного порта к пердаче
void user_uart3_ontxchar(void * ctx)
{
	uint_fast8_t c;
	if (uint8_queue_get(& txq, & c))
	{
		hardware_uart3_tx(ctx, c);
		if (uint8_queue_empty(& txq))
			hardware_uart3_enabletx(0);
	}
	else
	{
		hardware_uart3_enabletx(0);
	}
}

static unsigned package_tout;

// callback по принятому символу. Разбор и пересчет CRC
void user_uart3_onrxchar(uint_fast8_t c)
{
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);

	package_tout = 0;
	if (rxp != NULL)
	{
		if (rxp->count < ARRAY_SIZE(rxp->buff))
		{
			rxp->buff [rxp->count ++] = c;
			rxp->crc = calculateCRC8(c, rxp->crc);
		}
	}
	LowerIrql(oldIrql);
}

/* обработка тиков 5 ms */
/* system-mode function */
static void uart3_timer_pkg_event(void * ctx)
{
	const unsigned n = NTICKS(RXTOUT);	// пауза между приходом символов

	(void) ctx;	// приходит NULL
	if (package_tout < n)
	{
		if (++ package_tout >= n)
		{
			if (rxp->count != 0 && rxp->crc == 0)
			{
				/* приято до паузы, проверка CRC рошла */
				InsertHeadList(& rxlistready, & rxp->item);
				nextlist();
			}
			else
			{
				rxp->count = 0;
				rxp->crc = STARTCRCVAL;
			}
		}
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
	unsigned crc = 0xFF;
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

// Запрос на получение регистров состояния БИНС
void readregisters(unsigned devaddr, unsigned reg, unsigned numregs)
{
	uint8_t b [4];
	unsigned n = 0;

	n += mbuff_uint8(b + n, 0xFB);	// preamble
	n += mbuff_uint8(b + n, devaddr);	// device address
	n += mbuff_uint8(b + n, reg);	// register address
	n += mbuff_uint8(b + n, numregs & 0x7F);	// number of registers read

	uartX_write_crc8(b, n);
}

static unsigned rxpeek_uint32_LE(const uint8_t * b)
{
	unsigned v = 0;

	v = v * 256 + b [3];
	v = v * 256 + b [2];
	v = v * 256 + b [1];
	v = v * 256 + b [0];

	return v;
}

static float rxpeek_float32_LE(const uint8_t * b)
{
	union
	{
		float f;
		uint8_t b [sizeof (float)];
	} u;

	u.b [0] = b [0];
	u.b [1] = b [1];
	u.b [2] = b [2];
	u.b [3] = b [3];

	return u.f;
}

static double rxpeek_float64_LE(const uint8_t * b)
{
	union
	{
		double f;
		uint8_t b [sizeof (double)];
	} u;

	u.b [0] = b [0];
	u.b [1] = b [1];
	u.b [2] = b [2];
	u.b [3] = b [3];
	u.b [4] = b [4];
	u.b [5] = b [5];
	u.b [6] = b [6];
	u.b [7] = b [7];

	return u.f;
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
		return REGT_UINT32;
	case 1:
		return REGT_UINT32;
	case 2:
		return REGT_UINT32;
	case 3:
		return REGT_UINT32;
	case 4:
		return REGT_UINT32;
	case 5: case 6: case 7:
	case 8: case 9: case 10:
	case 11: case 12: case 13:
		return REGT_FLOAT32;
	case 14:
		return REGT_UINT32;
	case 15:
		return REGT_FLOAT32;
	case 16:
		return REGT_FLOAT32;
	case 17:
		return REGT_FLOAT32;
	case 18:
		return REGT_FLOAT32;
	case 19: case 20: case 21:
		return REGT_FLOAT32;
	case 54: case 55: case 56:
		return REGT_FLOAT32;
	case 57: case 58: case 59:
	case 60: case 61: case 62:
	case 63: case 64: case 65:
		return REGT_FLOAT32;
	case 66: case 67: case 68:
		return REGT_FLOAT32;
	case 128:
		return REGT_UINT32;
	case 129: // SYSTEM_TIME
		return REGT_UINT32;
	case 130: case 131: case 132:
		return REGT_UINT32;
	case 133: case 134: case 135:
		return REGT_UINT32;
	case 136: case 137: case 138:
		return REGT_UINT32;
	case 139:
		return REGT_FLOAT32;
	case 140: case 141: case 142:
		return REGT_FLOAT32;
	case 143: case 144: case 145:
		return REGT_FLOAT32;
	case 146: case 147: case 148:
		return REGT_FLOAT32;
	case 149: case 150: case 151:
		return REGT_FLOAT32;
	case 152:
		return REGT_FLOAT32;
	case 153:
		return REGT_FLOAT32;
	case 154: case 155: case 156:
		return REGT_FLOAT32;
	case 157: case 158: case 159: case 160:
		return REGT_FLOAT32;
	case 161: case 162: case 163:
	case 164: case 165: case 166:
	case 167: case 168: case 169:
		return REGT_FLOAT32;
	case 170: case 171: case 172:
		return REGT_FLOAT32;
	case 173:	// float64
		return REGT_FLOAT64;
	case 175:	// float64
		return REGT_FLOAT64;
	case 177:
		return REGT_FLOAT32;
	case 178:	// float64
		return REGT_FLOAT64;
	case 180:
		return REGT_FLOAT64;
	case 182: case 183: case 184: case 185:
		return REGT_FLOAT32;
	case 186: case 187: case 188:
		return REGT_FLOAT32;
	case 189: case 190: case 191:
		return REGT_FLOAT32;
	case 192: // EXT_GNSS_FLAG
		return REGT_UINT32;
	case 193:
		return REGT_UINT32;
	case 194:
		return REGT_UINT32;
	case 195:
		return REGT_UINT32;
	case 196: case 197: case 198:
	case 199: case 200: case 201:
	case 202: case 203: case 204:
		return REGT_UINT32;
	case 205: case 206: case 207:
		return REGT_UINT32;

	default:
		return REGT_UINT32;
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

// Послеодвательный проход по принятому с интерфейса буферу.
// Целостность данных уже проверена
static void parseanswers(const uint8_t * b, unsigned rxregbase, int rxnumregs)
{
	const unsigned lastreg = rxregbase + rxnumregs;
	for (; rxregbase < lastreg; b += 4 * xbreginc(rxregbase), rxregbase += xbreginc(rxregbase))
	{
		switch (rxregbase)
		{
		case 0:
			PRINTF("%-3u: ID_NUMBER=%08X\n", rxregbase, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 1:
			PRINTF("%-3u: FW_VERSION=%08X\n", rxregbase, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 2:
			PRINTF("%-3u: DEVICE SAVE AND RESET=%08X\n", rxregbase, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 3:
			PRINTF("%-3u: CONTROL_0=%08X\n", rxregbase, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 4:
			PRINTF("%-3u: CONTROL_1=%08X\n", rxregbase, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;

		case 5: case 6: case 7:
		case 8: case 9: case 10:
		case 11: case 12: case 13:
			PRINTF("%-3u: ROTATION MATRIX[%u]=%f\n", rxregbase, rxregbase - 5, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 14:
			PRINTF("%-3u: DEVUCE_DATE=%08X\n", rxregbase, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 15:
			PRINTF("%-3u: TYPICAL_LATITUDE=%f\n", rxregbase, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 16:
			PRINTF("%-3u: TYPICAL_LONGITUDE=%f\n", rxregbase, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 17:
			PRINTF("%-3u: TYPICAL_HEIGHT=%f\n", rxregbase, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 18:
			PRINTF("%-3u: INIT_HEADING=%f\n", rxregbase, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 19: case 20: case 21:
			PRINTF("%-3u: ANTENNA_DISTANCE[%u]=%f\n", rxregbase, rxregbase - 19, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 54: case 55: case 56:
			PRINTF("%-3u: GYRO_CALIBRATION_COEFFICIENT[%u]=%f\n", rxregbase, rxregbase - 54, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 57: case 58: case 59:
		case 60: case 61: case 62:
		case 63: case 64: case 65:
			PRINTF("%-3u: MAG_CALIBRATION_MATRIX[%u]=%f\n", rxregbase, rxregbase - 57, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 66: case 67: case 68:
			PRINTF("%-3u: MAG_CALIBRATION_COEFFICIENT[%u]=%f\n", rxregbase, rxregbase - 66, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 128:
			PRINTF("%-3u: DEV_STATUS=%08X\n", rxregbase, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 129: // SYSTEM_TIME
			PRINTF("%-3u: SYSTEM_TIME=%08X\n", rxregbase, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 130: case 131: case 132:
			PRINTF("%-3u: RAW_ACC[%u]=%08X\n", rxregbase, rxregbase - 130, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 133: case 134: case 135:
			PRINTF("%-3u: RAW_GYRO[%u]=%08X\n", rxregbase, rxregbase - 133, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 136: case 137: case 138:
			PRINTF("%-3u: RAW_MAG[%u]=%08X\n", rxregbase, rxregbase - 136, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 139:
			PRINTF("%-3u: ACC_TEMP=%f\n", rxregbase, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 140: case 141: case 142:
			PRINTF("%-3u: GYRO_TEMP[%u]=%f\n", rxregbase, rxregbase - 140, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 143: case 144: case 145:
			PRINTF("%-3u: CALIB_ACC[%u]=%f\n", rxregbase, rxregbase - 143, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 146: case 147: case 148:
			PRINTF("%-3u: CALIB_GYRO[%u]=%f\n", rxregbase, rxregbase - 146, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 149: case 150: case 151:
			PRINTF("%-3u: CALIB_MAG[%u]=%f\n", rxregbase, rxregbase - 149, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 152:
			PRINTF("%-3u: BAR_PRESS=%f\n", rxregbase, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 153:
			PRINTF("%-3u: BAR_ALT=%f\n", rxregbase, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 154: case 155: case 156:
			PRINTF("%-3u: HEADING_PITCH_ROLL[%u]=%f\n", rxregbase, rxregbase - 95, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 157: case 158: case 159: case 160:
			PRINTF("%-3u: QUAT[%u]=%f\n", rxregbase, rxregbase - 157, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 161: case 162: case 163:
		case 164: case 165: case 166:
		case 167: case 168: case 169:
			PRINTF("%-3u: ORIENTATION MATRIX[%u]=%f\n", rxregbase, rxregbase - 161, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 170: case 171: case 172:
			PRINTF("%-3u: EAST_NORTH_VERTICAL_VELOCITY[%u]=%f\n", rxregbase, rxregbase - 170, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 173:	// float64
			PRINTF("%-3u: OUT_LAT=%f\n", rxregbase, rxpeek_float64_LE(b));
			xbsavebins_float64(rxregbase, rxpeek_float64_LE(b));
			break;
		case 175:	// float64
			PRINTF("%-3u: OUT_LON=%f\n", rxregbase, rxpeek_float64_LE(b));
			xbsavebins_float64(rxregbase, rxpeek_float64_LE(b));
			break;
		case 177:
			PRINTF("%-3u: OUT_HEI=%f\n", rxregbase, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 178:	// float64
			PRINTF("%-3u: GPS_LAT=%f\n", rxregbase, rxpeek_float64_LE(b));
			xbsavebins_float64(rxregbase, rxpeek_float64_LE(b));
			break;
		case 180:
			PRINTF("%-3u: GPS_LON=%f\n", rxregbase, rxpeek_float64_LE(b));
			xbsavebins_float64(rxregbase, rxpeek_float64_LE(b));
			break;
		case 182: case 183: case 184: case 185:
			PRINTF("%-3u: GNSS_HEI_SOG_COG_vVEL[%u]=%f\n", rxregbase, rxregbase - 182, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 186: case 187: case 188:
			PRINTF("%-3u: GPS_EAST_NORTH_VERTICAL_VELOCITY[%u]=%f\n", rxregbase, rxregbase - 186, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 189: case 190: case 191:
			PRINTF("%-3u: EXT_GNSS_LAT_LON_HEI_STD[%u]=%f\n", rxregbase, rxregbase - 189, rxpeek_float32_LE(b));
			xbsavebins_float32(rxregbase, rxpeek_float32_LE(b));
			break;
		case 192: // EXT_GNSS_FLAG
			PRINTF("%-3u: EXT_GNSS_FLAG=%08X\n", rxregbase, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 193:
			PRINTF("%-3u: EXT_GNSS_LATENCY=%08X\n", rxregbase, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 194:
			PRINTF("%-3u: EXT_GNSS_TIME=%08X\n", rxregbase, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 195:
			PRINTF("%-3u: MAG_QUALITY,=%08X\n", rxregbase, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 196: case 197: case 198:
		case 199: case 200: case 201:
		case 202: case 203: case 204:
			PRINTF("%-3u: LON_LAT_HEI_Ve_Vn_Vu_HEADING_PITCH_ROLL_SD[%u]=%08X\n", rxregbase, rxregbase - 196, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;
		case 205: case 206: case 207:
			PRINTF("%-3u: TEST_MAG[%u]=%08X\n", rxregbase, rxregbase - 127, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;

		default:
			PRINTF("Undefined register %u %08X\n", rxregbase, rxpeek_uint32_LE(b));
			xbsavebins_int32(rxregbase, rxpeek_uint32_LE(b));
			break;

		}
	}
}

// Получить Количество регистров для обмена мщ начального адреса и количества параметров
static unsigned calctregcount(unsigned reg, unsigned argcount)
{
	unsigned tregcount;

	for (tregcount = 0; argcount --; reg += xbreginc(reg))
		tregcount += xbreginc(reg);

	return tregcount;
}

void xbsetregF(unsigned reg, unsigned argcount, const double * pv)
{
	const unsigned tregcount = calctregcount(reg, argcount);
	uint8_t b [4 + 8 * MAXPACKREGS];
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

void xbsetregI(unsigned reg, unsigned argcount, const long * pv)
{
	const unsigned tregcount = calctregcount(reg, argcount);
	uint8_t b [4 + 8 * MAXPACKREGS];
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


static int parsepacket(const uint8_t * p, unsigned sz)
{
	unsigned rxregbase;
	unsigned rxreg;
	unsigned rxnumregs;

	if (sz < 5)
		return 0;
	if (p [0] != 0xFB)	// preamble
		return 0;
	if (p [1] != GIRONAV_DEVADDR)	// address
		return 0;
	rxregbase = p [2];
	rxnumregs = p [3] & 0x7F;

	if (sz < (5 + rxnumregs * 4))
		return 0;

	parseanswers(p + 4, rxregbase, rxnumregs);
	return 1;
}


static ticker_t uart3_ticker;
static ticker_t uart3_pkg_ticker;
static dpcobj_t uart3_dpc_lock;

/* Функционирование USER MODE обработчиков */
void uart3_spool(void)
{
	rxlist_t * p;
	uint_fast8_t f;
	IRQL_t oldIrql;


	RiseIrql(IRQL_SYSTEM, & oldIrql);
	if (!IsListEmpty(& rxlistready))
	{
		LIST_ENTRY * v = RemoveTailList(& rxlistready);
		p = CONTAINING_RECORD(v, rxlist_t, item);
	}
	else
	{
		p = NULL;
	}
	LowerIrql(oldIrql);

	if (p != NULL)
	{
		/* использование принятого блока */

		//printhex(0, p->buff, p->count);
		parsepacket(p->buff, p->count);

		/* поместить блок в список свободных */
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		InsertHeadList(& rxlistfree, & p->item);
		LowerIrql(oldIrql);
	}
}

static void spool_part0(void)
{
	const unsigned start = 0;	// 0..21 - до USER_PACKAGE
	const unsigned top = 22;
	readregisters(GIRONAV_DEVADDR, start, top - start);
}


static void spool_part1(void)
{
	const unsigned start = 54;	//GYRO_CALIBRATION_COEFFICIENT...
	const unsigned top = 69;
	readregisters(GIRONAV_DEVADDR, start, top - start);
}

static void spool_part2(void)
{
	const unsigned start = 128;	// 0..21 - до USER_PACKAGE
	const unsigned top = 152;
	readregisters(GIRONAV_DEVADDR, start, top - start);
}

static void spool_part3(void)
{
	const unsigned start = 152;	// 0..21 - до USER_PACKAGE
	const unsigned top = 178;
	readregisters(GIRONAV_DEVADDR, start, top - start);
}

static void spool_part4(void)
{
	const unsigned start = 178;	// 0..21 - до USER_PACKAGE
	const unsigned top = 208;
	readregisters(GIRONAV_DEVADDR, start, top - start);
}

// Запросы на получение регистров состояния БИНС несколькими порциями
static int spoolcode;

static void (* spooltable [])(void) =
{
	spool_part0,
	spool_part1,
	spool_part2,
	spool_part3,
	spool_part4,
};

// передача запроса раз в PERIODSPOOL милисекунд
// ВЫполняется из USER LEVEL
static void uart3_dpc_spool(void * ctx)
{
	spooltable [spoolcode]();
	if (++ spoolcode >= ARRAY_SIZE(spooltable))
		spoolcode = 0;
	//TP();
}

static ticker_t uart3_ticker;
static dpcobj_t uart3_dpc_lock;

/* system-mode function */
static void uart3_timer_event(void * ctx)
{
	(void) ctx;	// приходит NULL

	board_dpc_call(& uart3_dpc_lock);	// Запрос отложенногог выполнения USER-MODE функции
}

void user_uart3_initialize(void)
{
	uartX_rxlist_initilize();
	nextlist();
	static uint8_t txb [512];
	uint8_queue_init(& txq, txb, ARRAY_SIZE(txb));

	hardware_uart3_initialize(0, 921600, 8, 0, 0);
	hardware_uart3_set_speed(921600);
	hardware_uart3_enablerx(1);
	hardware_uart3_enabletx(0);

	ticker_initialize(& uart3_pkg_ticker, 1, uart3_timer_pkg_event, NULL);
	ticker_add(& uart3_pkg_ticker);

	dpcobj_initialize(& uart3_dpc_lock, uart3_dpc_spool, NULL);
	ticker_initialize(& uart3_ticker, NTICKS(PERIODSPOOL), uart3_timer_event, NULL);
	ticker_add(& uart3_ticker);

}

#endif /* WITHCTRLBOARDT507 */

