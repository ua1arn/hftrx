/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "serial.h"
#include "formats.h"	// for debug prints
#include "board.h"
#include "gpio.h"
#include <string.h>
#include <math.h>

// Set interrupt vector wrapper
void serial_set_handler(uint_fast16_t int_id, void (* handler)(void))
{
#if WITHNMEAOVERREALTIME
		arm_hardware_set_handler_overrealtime(int_id, handler);
#else /* WITHNMEAOVERREALTIME */
		arm_hardware_set_handler_system(int_id, handler);
#endif /* WITHNMEAOVERREALTIME */
}


#if WITHNMEA && ! LINUX_SUBSYSTEM

//static void UART0_IRQHandler(void);

static void UART0_IRQHandler(void)
{
	char c;
	UART0->ISR = UART0->IMR;	// clear interrupt status

	while (hardware_uart0_getchar(& c))
	{
		HARDWARE_UART0_ONRXCHAR(c);
	}
}

// Очереди символов для обмена с согласующим устройством
enum { qSZ = 512 };
static uint8_t queue [qSZ];
static volatile unsigned qp, qg;

// Передать символ в host
static uint_fast8_t	qput(uint_fast8_t c)
{
	unsigned qpt = qp;
	const unsigned next = (qpt + 1) % qSZ;
	if (next != qg)
	{
		queue [qpt] = c;
		qp = next;
		HARDWARE_NMEA_ENABLETX(1);
		return 1;
	}
	return 0;
}

// Получить символ в host
static uint_fast8_t qget(uint_fast8_t * pc)
{
	if (qp != qg)
	{
		* pc = queue [qg];
		qg = (qg + 1) % qSZ;
		return 1;
	}
	return 0;
}

// получить состояние очереди передачи
static uint_fast8_t qempty(void)
{
	return qp == qg;
}

// Передать массив символов
static void qputs(const char * s, int n)
{
	while (n --)
		qput(* s ++);
}


/* вызывается из обработчика прерываний */
// компорт готов передавать
void nmea_sendchar(void * ctx)
{
	uint_fast8_t c;
	if (qget(& c))
	{
		HARDWARE_NMEA_TX(ctx, c);
		if (qempty())
			HARDWARE_NMEA_ENABLETX(0);
	}
	else
	{
		HARDWARE_NMEA_ENABLETX(0);
	}
}

int nmea_putc(int c)
{
#if WITHNMEAOVERREALTIME
	IRQL_t oldIrql;
	RiseIrql(IRQL_OVERREALTIME, & oldIrql);
	qput(c);
	LowerIrql(oldIrql;);
#else /* WITHNMEAOVERREALTIME */
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
    qput(c);
	LowerIrql(oldIrql);
#endif /* WITHNMEAOVERREALTIME */
	return c;
}


void nmea_format(const char * format, ...)
{
	char b [256];
	int n, i;
	va_list	ap;
	va_start(ap, format);

	n = vsnprintf(b, sizeof b / sizeof b [0], format, ap);

	for (i = 0; i < n; ++ i)
		nmea_putc(b [i]);

	va_end(ap);
}

/* вызывается из обработчика прерываний */
// произошла потеря символа (символов) при получении данных с CAT компорта
void nmea_rxoverflow(void)
{
}
/* вызывается из обработчика прерываний */
void nmea_disconnect(void)
{

}

void nmea_parser_init(void)
{
#if WITHUART0HW && WITHNMEA_USART0
	NMEA_INITIALIZE();
	serial_set_handler(UART0_IRQn, UART0_IRQHandler);
#endif /* WITHUART0HW && WITHNMEA_USART0 */
#if WITHUART1HW && WITHNMEA_USART1
	NMEA_INITIALIZE();
	serial_set_handler(UART1_IRQn, UART1_IRQHandler);
#endif /* WITHUART1HW && WITHNMEA_USART1 */
}

#endif /* WITHNMEA && ! LINUX_SUBSYSTEM */

#if CPUSTYLE_R7S721

// scemr:
// b0=1: 1: Base clock frequency is 8 times the bit rate,
// b0=0: 0: Base clock frequency is 16 times the bit rate
// scmsr:
// b1..b0: 0: /1, 1: /4, 2: /16, 3: /64
enum
{
	SCEMR_x16 = 0x00,	// ABCS=0
	SCEMR_x8 = 0x01,	// ABCS=1
	SCSMR_DIV1 = 0x00,
	SCSMR_DIV4 = 0x01,
	SCSMR_DIV16 = 0x02,
	SCSMR_DIV64 = 0x03,
};

const FLASHMEM struct spcr_spsr_tag scemr_scsmr [] =
{
	{ SCEMR_x8, 	SCSMR_DIV1, },		/* /8 = 8 * 1 */
	{ SCEMR_x16, 	SCSMR_DIV1, }, 		/* /16 = 16 * 1 */
	{ SCEMR_x8, 	SCSMR_DIV4, },		/* /32 = 8 * 4 */
	{ SCEMR_x16, 	SCSMR_DIV4, },		/* /64 = 16 * 4 */
	{ SCEMR_x8, 	SCSMR_DIV16, }, 	/* /128 = 8 * 16 */
	{ SCEMR_x16, 	SCSMR_DIV16, }, 	/* /256 = 16 * 16 */
	{ SCEMR_x8, 	SCSMR_DIV64, },  	/* /512 = 8 * 64 */
	{ SCEMR_x16, 	SCSMR_DIV64, }, 	/* /1024 = 16 * 64 */
};

#endif /* CPUSTYLE_R7S721 */



