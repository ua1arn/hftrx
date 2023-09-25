/*
 * uart0.c
 *
 *  Created on: 21 сент. 2023 г.
 *      Author: User
 */

#include "hardware.h"
#include "formats.h"
#include "bootloader.h"
#include "board.h"
#include "gpio.h"

#include "xhelper507.h"

#if WITHCTRLBOARDT507

// БИНС основной
// RS-422
#define DEVADDR 1

// Очереди символов для обмена

static u8queue_t txq;
static u8queue_t rxq;

static int nmeaX_putc(int c)
{
	IRQL_t oldIrql;
	uint_fast8_t f;

	do {
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		f = uint8_queue_put(& txq, c);
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



/* вызывается из обработчика прерываний */
// компорт готов передавать
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

void user_uart3_onrxchar(uint_fast8_t c)
{
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
	uint8_queue_put(& rxq, c);
	LowerIrql(oldIrql);
}

static unsigned culateCRC8(unsigned v, unsigned wCRCWord) {

    static const uint8_t wCRCTable[] = {
        0x00 ,0x31 ,0x62 ,0x53 ,0xC4 ,0xF5 ,0xA6 ,0x97 ,0xB9 ,0x88 ,0xDB ,0xEA ,0x7D ,0x4C ,0x1F ,0x2E,
        0x43 ,0x72 ,0x21 ,0x10 ,0x87 ,0xB6 ,0xE5 ,0xD4 ,0xFA ,0xCB ,0x98 ,0xA9 ,0x3E ,0x0F ,0x5C ,0x6D,
        0x86 ,0xB7 ,0xE4 ,0xD5 ,0x42 ,0x73 ,0x20 ,0x11 ,0x3F ,0x0E ,0x5D ,0x6C ,0xFB ,0xCA ,0x99 ,0xA8,
        0xC5 ,0xF4 ,0xA7 ,0x96 ,0x01 ,0x30 ,0x63 ,0x52 ,0x7C ,0x4D ,0x1E ,0x2F ,0xB8 ,0x89 ,0xDA ,0xEB,
        0x3D ,0x0C ,0x5F ,0x6E ,0xF9 ,0xC8 ,0x9B ,0xAA ,0x84 ,0xB5 ,0xE6 ,0xD7 ,0x40 ,0x71 ,0x22 ,0x13,
        0x7E ,0x4F ,0x1C ,0x2D ,0xBA ,0x8B ,0xD8 ,0xE9 ,0xC7 ,0xF6 ,0xA5 ,0x94 ,0x03 ,0x32 ,0x61 ,0x50,
        0xBB ,0x8A ,0xD9 ,0xE8 ,0x7F ,0x4E ,0x1D ,0x2C ,0x02 ,0x33 ,0x60 ,0x51 ,0xC6 ,0xF7 ,0xA4 ,0x95,
        0xF8 ,0xC9 ,0x9A ,0xAB ,0x3C ,0x0D ,0x5E ,0x6F ,0x41 ,0x70 ,0x23 ,0x12 ,0x85 ,0xB4 ,0xE7 ,0xD6,
        0x7A ,0x4B ,0x18 ,0x29 ,0xBE ,0x8F ,0xDC ,0xED ,0xC3 ,0xF2 ,0xA1 ,0x90 ,0x07 ,0x36 ,0x65 ,0x54,
        0x39 ,0x08 ,0x5B ,0x6A ,0xFD ,0xCC ,0x9F ,0xAE ,0x80 ,0xB1 ,0xE2 ,0xD3 ,0x44 ,0x75 ,0x26 ,0x17,
        0xFC ,0xCD ,0x9E ,0xAF ,0x38 ,0x09 ,0x5A ,0x6B ,0x45 ,0x74 ,0x27 ,0x16 ,0x81 ,0xB0 ,0xE3 ,0xD2,
        0xBF ,0x8E ,0xDD ,0xEC ,0x7B ,0x4A ,0x19 ,0x28 ,0x06 ,0x37 ,0x64 ,0x55 ,0xC2 ,0xF3 ,0xA0 ,0x91,
        0x47 ,0x76 ,0x25 ,0x14 ,0x83 ,0xB2 ,0xE1 ,0xD0 ,0xFE ,0xCF ,0x9C ,0xAD ,0x3A ,0x0B ,0x58 ,0x69,
        0x04 ,0x35 ,0x66 ,0x57 ,0xC0 ,0xF1 ,0xA2 ,0x93 ,0xBD ,0x8C ,0xDF ,0xEE ,0x79 ,0x48 ,0x1B ,0x2A,
        0xC1 ,0xF0 ,0xA3 ,0x92 ,0x05 ,0x34 ,0x67 ,0x56 ,0x78 ,0x49 ,0x1A ,0x2B ,0xBC ,0x8D ,0xDE ,0xEF,
        0x82 ,0xB3 ,0xE0 ,0xD1 ,0x46 ,0x77 ,0x24 ,0x15 ,0x3B ,0x0A ,0x59 ,0x68 ,0xFF ,0xCE ,0x9D ,0xAC
    };

    return wCRCTable [(v ^ wCRCWord) & 0xFF];
}

static unsigned uartX_putc_crc8(int c, unsigned crc)
{
	nmeaX_putc(c);
	crc = culateCRC8(c, crc);
	return crc;
}

static void uartX_write_crc8(const uint8_t * buff, size_t n)
{
	unsigned crc = 0xFF;
	while (n --)
	{
		crc = uartX_putc_crc8(* buff ++, crc);
	}
	uartX_putc_crc8(crc, 0);
}

static unsigned mbuff_uint8(uint8_t * b, uint_fast8_t v)
{
	b [0] = v;
	return 1;
}

static unsigned mbuff_uint32(uint8_t * b, uint_fast32_t v)
{
	b [0] = v >> 0;
	b [1] = v >> 8;
	b [2] = v >> 16;
	b [3] = v >> 24;
	return 4;
}

static unsigned mbuff_float32(uint8_t * b, float v)
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

void readregisters(unsigned devaddr, unsigned reg, unsigned numregs)
{
	uint8_t b [32];
	unsigned n = 0;

	n += mbuff_uint8(b + n, 0xFB);	// preamble
	n += mbuff_uint8(b + n, devaddr);	// address
	n += mbuff_uint8(b + n, reg);	// register address
	n += mbuff_uint8(b + n, numregs);	// read one register

	uartX_write_crc8(b, n);
}

// FB 01 01 01 02 00 00 00 7F
void read_HWVersion(void)
{
	readregisters(DEVADDR, 0x01, 1);
}

// CONTROL_0
// FB 01 03 01 01 54 00 01  03
void read_CONTROL_0(void)
{
	readregisters(DEVADDR, 0x03, 1);
}


void uart3_req(void)
{
	//readregisters(DEVADDR, 11, 4);
	//read_HWVersion();
	//read_CONTROL_0();
	readregisters(DEVADDR, 70, 1);
}

static unsigned rxpeek_uint32(const uint8_t * b)
{
	unsigned v = 0;

	v = v * 256 + b [3];
	v = v * 256 + b [2];
	v = v * 256 + b [1];
	v = v * 256 + b [0];

	return v;
}

static float rxpeek_float32(const uint8_t * b)
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

static double rxpeek_float64(const uint8_t * b)
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

static void parseanswers(const uint8_t * b, unsigned rxregbase, unsigned rxnumregs)
{
	for (; rxnumregs --; ++ rxregbase, b += 4)
	{
		switch (rxregbase)
		{
		case 0:
			PRINTF("ID_NUMBER=%08X\n", rxpeek_uint32(b));
			break;
		case 1:
			PRINTF("FW_VERSION=%08X\n", rxpeek_uint32(b));
			break;
		case 2:
			PRINTF("DEVICE SAVE AND RESET=%08X\n", rxpeek_uint32(b));
			break;
		case 3:
			PRINTF("CONTROL_0=%08X\n", rxpeek_uint32(b));
			break;
		case 4:
			PRINTF("CONTROL_1=%08X\n", rxpeek_uint32(b));
			break;

		case 5: case 6: case 7:
		case 8: case 9: case 10:
		case 11: case 12: case 13:
			PRINTF("ROTATION MATRIX[%u]=%f\n", rxregbase - 5, rxpeek_float32(b));
			break;

		case 15:
			PRINTF("TYPICAL_LATITUDE=%f\n", rxpeek_float32(b));
			break;
		case 16:
			PRINTF("TYPICAL_LONGITUDE=%f\n", rxpeek_float32(b));
			break;
		case 17:
			PRINTF("TYPICAL_HEIGHT=%f\n", rxpeek_float32(b));
			break;
		case 18:
			PRINTF("INIT_HEADING=%f\n", rxpeek_float32(b));
			break;

		case 19: case 20: case 21:
			PRINTF("ANTENNA_DISTANCE[%u]=%f\n", rxregbase - 19, rxpeek_float32(b));
			break;

		case 54: case 55: case 56:
			PRINTF("GYRO_CALIBRATION_COEFFICIENT[%u]=%f\n", rxregbase - 54, rxpeek_float32(b));
			break;

		case 57: case 58: case 59:
		case 60: case 61: case 62:
		case 63: case 64: case 65:
			PRINTF("MAG_CALIBRATION_MATRIX[%u]=%f\n", rxregbase - 57, rxpeek_float32(b));
			break;

		case 70: // SYSTEM_TIME
			PRINTF("SYSTEM_TIME=%08X\n", rxpeek_uint32(b));
			break;

		case 74: case 75: case 76:
			PRINTF("RAW_GYRO[%u]=%08X\n", rxregbase - 74, rxpeek_uint32(b));
			break;

		case 77: case 78: case 79:
			PRINTF("RAW_MAG[%u]=%08X\n", rxregbase - 77, rxpeek_uint32(b));
			break;

		case 80:
			PRINTF("ACC_TEMP=%f\n", rxpeek_float32(b));
			break;

		case 81: case 82: case 83:
			PRINTF("GYRO_TEMP[%u]=%f\n", rxregbase - 81, rxpeek_float32(b));
			break;

		case 84: case 85: case 86:
			PRINTF("CALIB_ACC[%u]=%f\n", rxregbase - 84, rxpeek_float32(b));
			break;

		case 87: case 88: case 89:
			PRINTF("CALIB_GYRO[%u]=%f\n", rxregbase - 87, rxpeek_float32(b));
			break;

		case 90: case 91: case 92:
			PRINTF("CALIB_MAG[%u]=%f\n", rxregbase - 90, rxpeek_float32(b));
			break;

		case 93:
			PRINTF("BAR_PRESS=%f\n", rxpeek_float32(b));
			break;

		case 94:
			PRINTF("BAR_ALT=%f\n", rxpeek_float32(b));
			break;

		case 95: case 96: case 97:
			PRINTF("HEADING_PITCH_ROLL[%u]=%f\n", rxregbase - 95, rxpeek_float32(b));
			break;

		case 98: case 99: case 100: case 101:
			PRINTF("QUAT[%u]=%f\n", rxregbase - 98, rxpeek_float32(b));
			break;

		case 102: case 103: case 104:
		case 105: case 106: case 107:
		case 108: case 109: case 110:
			PRINTF("ORIENTATION MATRIX[%u]=%f\n", rxregbase - 102, rxpeek_float32(b));
			break;

		case 111: case 112: case 113:
			PRINTF("EAST_NORTH_VERTICAL_VELOCITY[%u]=%f\n", rxregbase - 111, rxpeek_float32(b));
			break;

		case 114:
			PRINTF("OUT_LAT=%f\n", rxpeek_float64(b));
			b += 4; rxregbase += 1;
			break;

		case 116:
			PRINTF("OUT_LON=%f\n", rxpeek_float64(b));
			b += 4; rxregbase += 1;
			break;

		case 118:
			PRINTF("OUT_HEI=%f\n", rxpeek_float32(b));
			break;

		case 119:
			PRINTF("GPS_LAT=%f\n", rxpeek_float64(b));
			b += 4; rxregbase += 1;
			break;

		case 121:
			PRINTF("GPS_LON=%f\n", rxpeek_float64(b));
			b += 4; rxregbase += 1;
			break;

		case 123: case 124: case 125: case 126:
			PRINTF("GPS_HEI_SOG_COG_vVEL[%u]=%f\n", rxregbase - 123, rxpeek_float32(b));
			break;

		case 127: case 128: case 129:
			PRINTF("TEST_MAG[%u]=%08X\n", rxregbase - 127, rxpeek_uint32(b));
			break;

		default:
			PRINTF("Undefined register %u %08X\n", rxregbase, rxpeek_uint32(b));
			break;

		}
	}
}

enum states
{
	WAITPREAMBLE,
	WAITDEVADDR,
	WAITDEVREG,
	WAITNUMREGS,
	RXREGS,
	WAITCRC

	//
	//allStates
};

enum states st = WAITPREAMBLE;
static unsigned rxcrc;
static unsigned rxregbase;
static unsigned rxreg;
static unsigned rxnumregs;
static uint8_t rxbuff [128];

void uart3_spool(void)
{
	uint_fast8_t c;
	uint_fast8_t f;
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
    f = uint8_queue_get(& rxq, & c);
	LowerIrql(oldIrql);

	if (f)
	{
		//PRINTF("%02X ", c);
		switch (st)
		{
		case WAITPREAMBLE:
			if (c == 0xFB)
			{
				rxcrc = culateCRC8(c, 0xFF);
				st = WAITDEVADDR;
			}
			break;
		case WAITDEVADDR:
			rxcrc = culateCRC8(c, rxcrc);
			if (c == DEVADDR)
				st = WAITDEVREG;
			else
				st = WAITPREAMBLE;
			break;
		case WAITDEVREG:
			rxcrc = culateCRC8(c, rxcrc);
			rxregbase = c;	// start register of device
			st = WAITNUMREGS;
			break;
		case WAITNUMREGS:
			rxcrc = culateCRC8(c, rxcrc);
			rxnumregs = (c & 0x7F);
			rxreg = 0;
			st = RXREGS;
			break;
		case RXREGS:
			rxcrc = culateCRC8(c, rxcrc);
			if (rxreg >= ARRAY_SIZE(rxbuff))
				st = WAITPREAMBLE;
			else
			{
				rxbuff [rxreg ++] = c;
				if (rxreg >= (rxnumregs * 4))
					st = WAITCRC;
			}
			break;
		case WAITCRC:
			rxcrc = culateCRC8(c, rxcrc);
			if (rxcrc == 0)
			{
//				PRINTF("Registers: base=%u, n=%u\n", rxregbase, rxnumregs);
//				printhex(0, rxbuff, rxreg);
				parseanswers(rxbuff, rxregbase, rxnumregs);
			}
			else
			{
				PRINTF("bad CRC\n");
			}
			st = WAITPREAMBLE;
			break;
		}
	}

}

static void uart3_dpc_spool(void * ctx)
{
	readregisters(DEVADDR, 74, 3);	// RAW_GYRO
}

static ticker_t uart3_ticker;
static dpclock_t uart3_dpc_lock;

/* system-mode function */
static void uart3_timer_event(void * ctx)
{
	(void) ctx;	// приходит NULL

	board_dpc(& uart3_dpc_lock, uart3_dpc_spool, NULL);
}

void user_uart3_initialize(void)
{
	uint8_queue_init(& txq);
	uint8_queue_init(& rxq);
	hardware_uart3_initialize(0, 921600);
	hardware_uart3_set_speed(921600);
	hardware_uart3_enablerx(1);
	hardware_uart3_enabletx(0);

	dpclock_initialize(& uart3_dpc_lock);
	ticker_initialize(& uart3_ticker, NTICKS(1500), uart3_timer_event, NULL);
	ticker_add(& uart3_ticker);

}

#endif /* WITHCTRLBOARDT507 */

