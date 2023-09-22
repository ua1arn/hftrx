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

// Очереди символов для обмена
enum { txqSZ = 512 };
static uint8_t txqueue [txqSZ];
static volatile unsigned txqp, txqg;

// Передать символ в host
static uint_fast8_t	txqput(uint_fast8_t c)
{
	unsigned qpt = txqp;
	const unsigned next = (qpt + 1) % txqSZ;
	if (next != txqg)
	{
		txqueue [qpt] = c;
		txqp = next;
		hardware_uart3_enabletx(1);
		return 1;
	}
	return 0;
}

// Получить символ в host
static uint_fast8_t txqget(uint_fast8_t * pc)
{
	if (txqp != txqg)
	{
		* pc = txqueue [txqg];
		txqg = (txqg + 1) % txqSZ;
		return 1;
	}
	return 0;
}

// получить состояние очереди передачи
static uint_fast8_t txqempty(void)
{
	return txqp == txqg;
}

static int nmeaX_putc(int c)
{
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
    txqput(c);
	LowerIrql(oldIrql);

	return c;
}

static void uartX_write(const uint8_t * buff, size_t n)
{
	while (n --)
		nmeaX_putc(* buff ++);
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

////////////////
///
// Очереди символов для обмена
enum { rxqSZ = 512 };
static uint8_t rxqueue [rxqSZ];
static volatile unsigned rxqp, rxqg;

// Передать символ в host
static uint_fast8_t	rxqput(uint_fast8_t c)
{
	unsigned qpt = rxqp;
	const unsigned next = (qpt + 1) % rxqSZ;
	if (next != rxqg)
	{
		rxqueue [qpt] = c;
		rxqp = next;
		return 1;
	}
	return 0;
}

// Получить символ в host
static uint_fast8_t rxqget(uint_fast8_t * pc)
{
	if (rxqp != rxqg)
	{
		* pc = rxqueue [rxqg];
		rxqg = (rxqg + 1) % rxqSZ;
		return 1;
	}
	return 0;
}

// получить состояние очереди передачи
static uint_fast8_t rxqempty(void)
{
	return rxqp == rxqg;
}

////////////////
///


/* вызывается из обработчика прерываний */
// компорт готов передавать
void user_uart3_ontxchar(void * ctx)
{
	uint_fast8_t c;
	if (txqget(& c))
	{
		hardware_uart3_tx(ctx, c);
		if (txqempty())
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
    rxqput(c);
	LowerIrql(oldIrql);
}

void user_uart3_initialize(void)
{
	hardware_uart3_initialize(0);
	hardware_uart3_set_speed(921600);
	hardware_uart3_enablerx(1);
	hardware_uart3_enabletx(0);
}

void uart3_spool(void)
{
	uint_fast8_t c;
	uint_fast8_t f;
	IRQL_t oldIrql;

	RiseIrql(IRQL_SYSTEM, & oldIrql);
    f = rxqget(& c);
	LowerIrql(oldIrql);

	if (f)
		PRINTF("%02X ", c);
}

void uart3_req(void)
{
	static const req [] =
	{
			0xFB, 0x01,  0x00,  0x01, 0x79,
	};

	uartX_write(req, sizeof req);
}


#endif /* WITHCTRLBOARDT507 */

