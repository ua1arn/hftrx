/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if WITHUART0HW

#include "serial.h"
#include "formats.h"	// for debug prints
#include "board.h"
#include "gpio.h"
#include <string.h>
#include <math.h>
#include "clocks.h"

#endif /* WITHUART0HW */
#if WITHUART0HW

#define thisPORT 0

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

void RAMFUNC_NONILINE USART0_Handler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	const uint_fast32_t csr = USART0->US_CSR;

	if (csr & US_CSR_RXRDY)
		HARDWARE_UART0_ONRXCHAR(USART0->US_RHR);
	if (csr & US_CSR_TXRDY)
		HARDWARE_UART0_ONTXCHAR(USART0);
}

#elif CPUSTYLE_AT91SAM7S

static RAMFUNC_NONILINE void AT91F_US0Handler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	const uint_fast32_t csr = AT91C_BASE_US0->US_CSR;

	if (csr & AT91C_US_RXRDY)
		HARDWARE_UART0_ONRXCHAR(AT91C_BASE_US0->US_RHR);
	if (csr & AT91C_US_TXRDY)
		HARDWARE_UART0_ONTXCHAR(AT91C_BASE_US0);
}

#elif CPUSTYLE_R7S721

// Приём символа он последовательного порта
static void SCIFRXI0_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	(void) uart->SCFSR;						// Перед сбросом бита RDF должно произойти его чтение в ненулевом состоянии
	uart->SCFSR = (uint16_t) ~ SCIF0_SCFSR_RDF;	// RDF=0 читать незачем (в примерах странное - сбрасывабтся и другие биты)
	if (uart->SCSCR & SCIF0_SCSCR_RIE)	// RIE Receive Interrupt Enable
	{
		uint_fast8_t n = (uart->SCFDR & SCIF0_SCFDR_R) >> SCIF0_SCFDR_R_SHIFT;
		while (n --)
			HARDWARE_UART0_ONRXCHAR(uart->SCFRDR & SCIF0_SCFRDR_D);
	}
}

// Передача символа в последовательный порт
static void SCIFTXI0_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	if (uart->SCSCR & SCIF0_SCSCR_TIE)	// TIE Transmit Interrupt Enable
	{
		HARDWARE_UART0_ONTXCHAR(uart);
	}
}

#elif CPUSTYLE_XC7Z

static void UART0_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	char c;
	const uint_fast32_t sts = UART0->ISR & UART0->IMR;
	if (sts & (1u << 5))	// RXOVR
	{
		UART0->ISR = (1u << 5);	// RXOVR
		HARDWARE_UART0_ONOVERFLOW();
	}
	if (sts & (1u << 7))	// PARE
	{
		UART0->ISR = (1u << 7);	// PARE
	}
	if (sts & (1u << 6))	// FRAME
	{
		UART0->ISR = (1u << 6);	// FRAME
	}
	if (sts & (1u << 3))	// TEMPTY
	{
		HARDWARE_UART0_ONTXCHAR(UART0);
	}
	while (hardware_uart0_getchar(& c))
	{
		HARDWARE_UART0_ONRXCHAR(c);
	}
}

#elif CPUSTYLE_ALLWINNER

static RAMFUNC_NONILINE void UART0_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	const uint_fast32_t ier = uart->UART_DLH_IER;
	const uint_fast32_t usr = uart->UART_USR;

	if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
	{
		if (usr & (1u << 3))	// RX FIFO Not Empty
			HARDWARE_UART0_ONRXCHAR(uart->UART_RBR_THR_DLL);
	}
	if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
	{
		if (usr & (1u << 1))	// TX FIFO Not Full
			HARDWARE_UART0_ONTXCHAR(uart);
	}
}

#elif CPUSTYLE_VM14

static RAMFUNC_NONILINE void UART0_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	const uint_fast32_t ier = UART0->UART_DLH_IER;
	const uint_fast32_t usr = UART0->UART_USR;

	if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
	{
		if (usr & (1u << 3))	// RX FIFO Not Empty
			HARDWARE_UART0_ONRXCHAR(UART0->UART_RBR_THR_DLL);
	}
	if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
	{
		if (usr & (1u << 1))	// TX FIFO Not Full
			HARDWARE_UART0_ONTXCHAR(UART0);
	}

}

#elif CPUSTYLE_ROCKCHIP

static RAMFUNC_NONILINE void UART0_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	const uint_fast32_t ier = uart->UART_DLH_IER;
	const uint_fast32_t usr = uart->UART_USR;

	if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
	{
		if (usr & (1u << 3))	// RX FIFO Not Empty
			HARDWARE_UART0_ONRXCHAR(uart->UART_RBR_THR_DLL);
	}
	if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
	{
		if (usr & (1u << 1))	// TX FIFO Not Full
			HARDWARE_UART0_ONTXCHAR(uart);
	}
}

#else
	#error Undefined CPUSTYLE_XXX
#endif	/* CPUSTYLE_ATMEGA_XXX4 */


/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uart0_enabletx(uint_fast8_t state)
{
	hardware_uartx_enabletx(UARTBASENAME(thisPORT), state);
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uart0_enablerx(uint_fast8_t state)
{
	hardware_uartx_enablerx(UARTBASENAME(thisPORT), state);
}

/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uart0_tx(void * ctx, uint_fast8_t c)
{
	hardware_uartx_tx(UARTBASENAME(thisPORT), c);
}

/* дождаться, когда буде все передано */
void hardware_uart0_flush(void)
{
	hardware_uartx_flush(UARTBASENAME(thisPORT));
}

/* приём символа, если готов порт */
uint_fast8_t
hardware_uart0_getchar(char * cp)
{
	return hardware_uartx_getchar(UARTBASENAME(thisPORT), cp);
}

/* передача символа если готов порт */
uint_fast8_t
hardware_uart0_putchar(uint_fast8_t c)
{
	return hardware_uartx_putchar(UARTBASENAME(thisPORT), c);
}

void hardware_uart0_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd)
{
	int fifo = 1;
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		// enable the clock of USART0
		PMC->PMC_PCER0 = 1u << ID_USART0;

		HARDWARE_UART0_INITIALIZE();	/* Присоединить периферию к выводам */

		hardware_uartx_initialize(UARTBASENAME(thisPORT), busfreq, defbaudrate, bits, parity, odd, fifo);

		if (debug == 0)
		{
			serial_set_handler(USART0_IRQn, & USART0_IRQHandler);
		}

	#elif HARDWARE_ARM_USEUART0
		// enable the clock of UART0
		PMC->PMC_PCER0 = 1u << ID_UART0;

		HARDWARE_UART0_INITIALIZE();	/* Присоединить периферию к выводам */

		hardware_uartx_initialize(UARTBASENAME(thisPORT), busfreq, defbaudrate, bits, parity, odd, fifo);

		if (debug == 0)
		{
			serial_set_handler(UART0_IRQn, & UART0_IRQHandler);
		}

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSARTx */

#elif CPUSTYLE_AT91SAM7S

	// enable the clock of USART0
	AT91C_BASE_PMC->PMC_PCER = 1u << AT91C_ID_US0;

	HARDWARE_UART0_INITIALIZE();	/* Присоединить периферию к выводам */
	hardware_uartx_initialize(UARTBASENAME(thisPORT), busfreq, defbaudrate, bits, parity, odd, fifo);
	if (debug == 0)
	{
		serial_set_handler(AT91C_ID_US0, AT91F_US0Handler);
	}

#elif CPUSTYLE_R7S721

    /* ---- Supply clock to the SCIF(channel 0) ---- */
	CPG.STBCR4 &= ~ CPG_STBCR4_BIT_MSTP47;	// Module Stop 47 SCIF0
	(void) CPG.STBCR4;			/* Dummy read */

	hardware_uartx_initialize(UARTBASENAME(thisPORT), busfreq, defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART0_INITIALIZE();	/* Присоединить периферию к выводам */
	if (debug == 0)
	{
	   serial_set_handler(SCIFRXI0_IRQn, SCIFRXI0_IRQHandler);
	   serial_set_handler(SCIFTXI0_IRQn, SCIFTXI0_IRQHandler);
	}

#elif CPUSTYLE_XC7Z

	const unsigned ix = thisPORT;
	SCLR->SLCR_UNLOCK = 0x0000DF0DU;
	SCLR->APER_CLK_CTRL |= (UINT32_C(1) << (20 + ix));	// APER_CLK_CTRL.UART1_CPU_1XCLKACT
	//EMIT_MASKWRITE(0XF8000154, 0x00003F33U ,0x00001002U),	// UART_CLK_CTRL
	SCLR->UART_CLK_CTRL = (SCLR->UART_CLK_CTRL & ~ UINT32_C(0x00003F30)) |
			((uint_fast32_t) SCLR_UART_CLK_CTRL_DIVISOR_VALUE << 8) | // DIVISOR
			(0x00u << 4) |	// SRCSEL - 0x: IO PLL
			(0x01 << ix) |	// CLKACT0 - UART 0 reference clock active
			0;

	hardware_uartx_initialize(UARTBASENAME(thisPORT), xc7z_get_uart_freq(), defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART0_INITIALIZE();	/* Присоединить периферию к выводам */
	if (debug == 0)
	{
	   serial_set_handler(UART0_IRQn, UART0_IRQHandler);
	}

#elif CPUSTYLE_A64 || CPUSTYLE_H3

	const unsigned ix = thisPORT;

	/* Open the clock gate for uart0 */
	CCU->BUS_CLK_GATING_REG3 |= (1u << (ix + 16));	// UART0_GATING

	/* De-assert uart0 reset */
	CCU-> BUS_SOFT_RST_REG4 |= (1u << (ix + 16));	//  UART0_RST

	hardware_uartx_initialize(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART0_INITIALIZE();
	if (debug == 0)
	{
	   serial_set_handler(UART0_IRQn, UART0_IRQHandler);
	}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_T507 || CPUSTYLE_H616)

	const unsigned ix = thisPORT;

	/* Open the clock gate for uart0 */
	CCU->UART_BGR_REG |= (1u << (ix + 0));

	/* De-assert uart0 reset */
	CCU->UART_BGR_REG |= (1u << (ix + 16));

	hardware_uartx_initialize(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART0_INITIALIZE();
	if (debug == 0)
	{
	   serial_set_handler(UART0_IRQn, UART0_IRQHandler);
	}

#elif (CPUSTYLE_A133 || CPUSTYLE_R828)

	const unsigned ix = thisPORT;

	/* Open the clock gate for uart0 */
	CCU->UART_BGR_REG |= (1u << (ix + 0));

	/* De-assert uart0 reset */
	CCU->UART_BGR_REG |= (1u << (ix + 16));

	hardware_uartx_initialize(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART0_INITIALIZE();
	if (debug == 0)
	{
	   serial_set_handler(UART0_IRQn, UART0_IRQHandler);
	}

#elif CPUSTYLE_V3S

	const unsigned ix = thisPORT;

	/* Open the clock gate for uart0 */
	CCU->BUS_CLK_GATING_REG3 |= (1u << (ix + 16));

	/* De-assert uart0 reset */
	CCU->BUS_SOFT_RST_REG4 |= (1u << (ix + 16));

	hardware_uartx_initialize(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART0_INITIALIZE();
	if (debug == 0)
	{
	   serial_set_handler(UART0_IRQn, UART0_IRQHandler);
	}

#elif CPUSTYLE_VM14

	const unsigned ix = thisPORT;	// UART0

	CMCTR->GATE_SYS_CTR |= ((1u << 12) << ix); // UART0_EN Enable CLK

	hardware_uartx_initialize(UARTBASENAME(thisPORT), elveesvm14_get_usart_freq(), defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART0_INITIALIZE();
	if (debug == 0)
	{
	   serial_set_handler(UART0_IRQn, UART0_IRQHandler);
	}

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART0HW */

#if WITHUART0HW

void
hardware_uart0_set_speed(uint_fast32_t baudrate)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		// Использование автоматического расчёта предделителя
		unsigned value;
		const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
		USART0->US_BRGR = value;
		if (prei == 0)
		{
			USART0->US_MR |= US_MR_OVER;
		}
		else
		{
			USART0->US_MR &= ~ US_MR_OVER;
		}

	#elif HARDWARE_ARM_USEUART0
		// Использование автоматического расчёта предделителя
		unsigned value;
		calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART0->UART_BRGR = value;

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), AT91SAM7_USART_BRGR_WIDTH, AT91SAM7_USART_BRGR_TAPS, & value, 0);

	AT91C_BASE_US0->US_BRGR = value;
	if (prei == 0)
	{
		AT91C_BASE_US0->US_MR |= AT91C_US_OVER;
	}
	else
	{
		AT91C_BASE_US0->US_MR &= ~ AT91C_US_OVER;
	}

#elif CPUSTYLE_R7S721

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), P1CLOCK_FREQ, baudrate);

#elif CPUSTYLE_XC7Z

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), xc7z_get_uart_freq(), baudrate);

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616 || CPUSTYLE_V3S || CPUSTYLE_H3 || CPUSTYLE_A133 || CPUSTYLE_R818)

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, baudrate);

#elif CPUSTYLE_VM14

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), elveesvm14_get_usart_freq(), baudrate);

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART0HW */
