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

// давление
// RS-485 9600 8O1 (ODD PARITY!)

#define PERIODSPOOL 500
#define RXTOUT 100

static u8queue_t txq;


#define STARTCRCVAL 0xFFFF

static unsigned culateCRC16(unsigned v, unsigned wCRCWord)
{
    static const uint16_t wCRCTable[] =
    {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };

    uint16_t nTemp;

    nTemp = v ^ wCRCWord;
    wCRCWord >>= 8;
    wCRCWord  ^= wCRCTable[nTemp & 0xFF];
    return wCRCWord;
}

typedef struct rxlist
{
	LIST_ENTRY item;
	uint8_t buff [128];
	unsigned count;
	unsigned crc;
} rxlist_t;

static LIST_ENTRY rxlistfree;
static LIST_ENTRY rxlistready;
static rxlist_t * rxp = NULL;

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

static unsigned package_tout;

void user_uart5_onrxchar(uint_fast8_t c)
{
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);

	package_tout = 0;
	if (rxp != NULL)
	{
		if (rxp->count < ARRAY_SIZE(rxp->buff))
		{
			rxp->buff [rxp->count ++] = c;
			rxp->crc = culateCRC16(c, rxp->crc);
		}
	}
	LowerIrql(oldIrql);
}

/* обработка тиков 5 ms */
/* system-mode function */
static void uart5_timer_pkg_event(void * ctx)
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

void user_uart5_ontxchar(void * ctx)
{
	uint_fast8_t c;
	if (uint8_queue_get(& txq, & c))
	{
		hardware_uart5_tx(ctx, c);
		if (uint8_queue_empty(& txq))
			hardware_uart5_enabletx(0);
	}
	else
	{
		hardware_uart5_enabletx(0);
	}
}

static int nmeaX_putc(int c)
{
	IRQL_t oldIrql;
	uint_fast8_t f;

	do {
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		f = uint8_queue_put(& txq, c);
		hardware_uart5_enabletx(1);
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


static float rxpeek_float32_BE(const uint8_t * b)
{
	union
	{
		float f;
		uint8_t b [sizeof (float)];
	} u;

	u.b [3] = b [0];
	u.b [2] = b [1];
	u.b [1] = b [2];
	u.b [0] = b [3];

	return u.f;
}

//	00000000  01 03 04 BA 96 02 0D FF A2                       .........
//	Pressure=0.000000, depth=0.000000

static int parsepacket(const uint8_t * p, unsigned sz)
{
	if (sz < 9)
		return 0;
	if (p [0] != 0x01)
		return 0;
	if (p [1] != 0x03)
		return 0;
	unsigned len = p [2];
	if (len != 4)
		return 0;
	float pr = rxpeek_float32_BE(p + 3);

	PRINTF("Pressure=%f, depth=%f\n", pr, pr * 101.97162005);
	return 1;
}

static ticker_t uart5_ticker;
static ticker_t uart5_pkg_ticker;
static dpclock_t uart5_dpc_lock;

void uart5_spool(void)
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

		/* поместить блок в список своюодных */
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		InsertHeadList(& rxlistfree, & p->item);
		LowerIrql(oldIrql);
	}
}

static unsigned uartX_putc_crc16(int c, unsigned crc)
{
	nmeaX_putc(c);
	crc = culateCRC16(c, crc);
	return crc;
}

static void uartX_write_crc16(const uint8_t * buff, size_t n)
{
	unsigned crc = STARTCRCVAL;
	while (n --)
	{
		crc = uartX_putc_crc16(* buff ++, crc);
	}
	unsigned crc2 = crc;
	crc = uartX_putc_crc16(crc2 >> 0, crc);
	crc = uartX_putc_crc16(crc2 >> 8, crc);
	ASSERT(crc == 0);
}

static void uart5_req(void)
{
	unsigned regaddr = 8;
	unsigned regnum = 2;
	unsigned command = 3;	// read

	uint8_t b [32];
	b [0] = 0x01;	// target id
	b [1] = command; //0x01;	// target id
	b [2] = regaddr >> 8;	// reg high byte
	b [3] = regaddr >> 0;	// reg low byte
	b [4] = regnum >> 8;	// num high byte
	b [5] = regnum >> 0;	// num low byte

	uartX_write_crc16(b, 6);
}

static void uart5_dpc_spool(void * ctx)
{
	uart5_req();
}

static ticker_t uart5_ticker;
static ticker_t uart5_pkg_ticker;
static dpclock_t uart5_dpc_lock;

/* system-mode function */
static void uart5_timer_event(void * ctx)
{
	(void) ctx;	// приходит NULL

	board_dpc(& uart5_dpc_lock, uart5_dpc_spool, NULL);
}

void user_uart5_initialize(void)
{
	uartX_rxlist_initilize();
	nextlist();
	uint8_queue_init(& txq);

	hardware_uart5_initialize(0, 9600, 8, 1, 1);	// 8-O-1
	hardware_uart5_set_speed(9600);
	hardware_uart5_enablerx(1);
	hardware_uart5_enabletx(0);

	ticker_initialize(& uart5_pkg_ticker, 1, uart5_timer_pkg_event, NULL);
	ticker_add(& uart5_pkg_ticker);

	dpclock_initialize(& uart5_dpc_lock);
	ticker_initialize(& uart5_ticker, NTICKS(PERIODSPOOL), uart5_timer_event, NULL);
	ticker_add(& uart5_ticker);
}

#endif /* WITHCTRLBOARDT507 */

