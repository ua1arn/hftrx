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


// руль машинка
// RS-485

static u8queue_t txq;
static u8queue_t rxq;



/* вызывается из обработчика прерываний */
// компорт готов передавать
void user_uart1_ontxchar(void * ctx)
{
	uint_fast8_t c;
	if (uint8_queue_get(& txq, & c))
	{
		hardware_uart1_tx(ctx, c);
		if (uint8_queue_empty(& txq))
			hardware_uart1_enabletx(0);
	}
	else
	{
		hardware_uart1_enabletx(0);
	}
}

void user_uart1_onrxchar(uint_fast8_t c)
{
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
	uint8_queue_put(& rxq, c);
	LowerIrql(oldIrql);
}

static int nmeaX_putc(int c)
{
	IRQL_t oldIrql;
	uint_fast8_t f;

	do {
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		f = uint8_queue_put(& txq, c);
		hardware_uart1_enabletx(1);
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

static uint16_t crc16(unsigned v, unsigned crc)
 {
	crc = (uint16_t) ((v << 8u) ^ crc);
	for (uint8_t k = 0u; k < 8u; k++) {
		if (crc & 0x8000u)
			crc = (uint16_t) ((crc << 1u) ^ 0x8005u);
		else
			crc = (uint16_t) (crc << 1u);
	}
	return crc;
}



static unsigned culateCRC16(unsigned v, unsigned wCRCWord)
{
    static const uint16_t wCRCTable[] =
    {
		0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011, 0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
		0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072, 0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
		0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2, 0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
		0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1, 0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
		0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192, 0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
		0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1, 0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
		0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151, 0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
		0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132, 0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
		0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312, 0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
		0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371, 0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
		0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1, 0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
		0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2, 0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
		0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291, 0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
		0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2, 0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
		0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252, 0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
		0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231, 0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202,
    };

    return wCRCTable [(v ^ wCRCWord) & 0xFF];
}

static unsigned uartX_putc_crc16(int c, unsigned crc)
{
	nmeaX_putc(c);
	//PRINTF("tx:%02X ", c & 0xFF);
	crc = crc16(c, crc);
	return crc;
}

static void uartX_write_crc16(const uint8_t * buff, size_t n)
{
	unsigned crc = 0xFFFF;
	while (n --)
	{
		crc = uartX_putc_crc16(* buff ++, crc);
	}
	unsigned crc2 = crc;
	crc = uartX_putc_crc16(crc2 >> 8, crc);
	crc = uartX_putc_crc16(crc2 >> 0, crc);
	ASSERT(crc == 0);
}

static void uart1_req(int targetId)
{
	uint8_t b [32];
	unsigned arg1 = 0;
	b [0] = 0x69;	// command code - readout actual position
	b [1] = targetId; //0x01;	// target id
	b [2] = arg1 >> 8;	// arg1 high byte
	b [3] = arg1 >> 0;	// arg1 low byte

	uartX_write_crc16(b, 4);
}

static int phase;
static void uart1_dpc_spool(void * ctx)
{
//	spooltable [spoolcode]();
//	if (++ spoolcode >= ARRAY_SIZE(spooltable))
//		spoolcode = 0;
	phase = ! phase;
	uart1_req(phase ? 1 : 2);
	//TP();
}


static void uart1_dpc_pkg_spool(void * ctx)
{
}

static ticker_t uart1_ticker;
static ticker_t uart1_pkg_ticker;
static dpclock_t uart1_dpc_lock;

/* system-mode function */
static void uart1_timer_event(void * ctx)
{
	(void) ctx;	// приходит NULL

	board_dpc(& uart1_dpc_lock, uart1_dpc_spool, NULL);
}

static unsigned package_tout;

/* system-mode function */
static void uart1_timer_pkg_event(void * ctx)
{
	(void) ctx;	// приходит NULL
	if (++ package_tout)
	{
		board_dpc(& uart1_dpc_lock, uart1_dpc_pkg_spool, NULL);
	}
}

void user_uart1_initialize(void)
{
	hardware_uart1_initialize(0, 115200);
	hardware_uart1_set_speed(115200);


	hardware_uart1_enablerx(1);
	hardware_uart1_enabletx(0);

//	for (;;)
//		nmeaX_putc(0xF0);

	dpclock_initialize(& uart1_dpc_lock);
	ticker_initialize(& uart1_ticker, NTICKS(750), uart1_timer_event, NULL);
	ticker_add(& uart1_ticker);
//	ticker_initialize(& uart1_pkg_ticker, 1, uart1_timer_pkg_event, NULL);
//	ticker_add(& uart1_pkg_ticker);
}

// 3.1 Set Point Command
// http://www.sunshine2k.de/coding/javascript/crc/crc_js.html

void uart1_spool(void)
{
	uint_fast8_t c;
	uint_fast8_t f;
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
    f = uint8_queue_get(& rxq, & c);
	LowerIrql(oldIrql);

	if (f)
	{
		PRINTF("rx1:%02X ", c & 0xFF);
	}
}

#endif /* WITHCTRLBOARDT507 */

