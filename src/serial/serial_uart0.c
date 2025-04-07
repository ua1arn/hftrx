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

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	void RAMFUNC_NONILINE USART0_Handler(void)
	{
		const uint_fast32_t csr = USART0->US_CSR;

		if (csr & US_CSR_RXRDY)
			HARDWARE_UART0_ONRXCHAR(USART0->US_RHR);
		if (csr & US_CSR_TXRDY)
			HARDWARE_UART0_ONTXCHAR(USART0);
	}

#elif CPUSTYLE_AT91SAM7S

	static RAMFUNC_NONILINE void AT91F_US0Handler(void)
	{
		const uint_fast32_t csr = AT91C_BASE_US0->US_CSR;

		if (csr & AT91C_US_RXRDY)
			HARDWARE_UART0_ONRXCHAR(AT91C_BASE_US0->US_RHR);
		if (csr & AT91C_US_TXRDY)
			HARDWARE_UART0_ONTXCHAR(AT91C_BASE_US0);
	}

#elif CPUSTYLE_R7S721

	// Приём символа он последовательного порта
	static RAMFUNC_NONILINE void SCIFRXI0_IRQHandler(void)
	{
		(void) SCIF0.SCFSR;						// Перед сбросом бита RDF должно произойти его чтение в ненулевом состоянии
		SCIF0.SCFSR = (uint16_t) ~ SCIF0_SCFSR_RDF;	// RDF=0 читать незачем (в примерах странное - сбрасывабтся и другие биты)
		uint_fast8_t n = (SCIF0.SCFDR & SCIF0_SCFDR_R) >> SCIF0_SCFDR_R_SHIFT;
		while (n --)
			HARDWARE_UART0_ONRXCHAR(SCIF0.SCFRDR & SCIF0_SCFRDR_D);
	}

	// Передача символа в последовательный порт
	static RAMFUNC_NONILINE void SCIFTXI0_IRQHandler(void)
	{
		HARDWARE_UART0_ONTXCHAR(& SCIF0);
	}

#elif CPUSTYLE_XC7Z

	static void UART0_IRQHandler(void)
	{
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

#elif CPUSTYLE_VM14

	static RAMFUNC_NONILINE void UART0_IRQHandler(void)
	{
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

#else
	#error Undefined CPUSTYLE_XXX
#endif	/* CPUSTYLE_ATMEGA_XXX4 */


/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uart0_enabletx(uint_fast8_t state)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		hardware_uartx_enabletx(USART0, state);
	#elif HARDWARE_ARM_USEUART0
		hardware_uartx_enabletx(UART0, state);
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_AT91SAM7S

	hardware_uartx_enabletx(AT91C_BASE_US0, state);

#elif CPUSTYLE_R7S721

	hardware_uartx_enabletx(& SCIF0, state);

#elif CPUSTYLE_XC7Z

	hardware_uartx_enabletx(UART0, state);

#elif CPUSTYLE_ALLWINNER

	hardware_uartx_enabletx(UART0, state);

#elif CPUSTYLE_VM14

	hardware_uartx_enabletx(UART0, state);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uart0_enablerx(uint_fast8_t state)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		hardware_uartx_enablerx(USART0, state);
	#elif HARDWARE_ARM_USEUART0
		hardware_uartx_enablerx(UART0, state);
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_AT91SAM7S

	hardware_uartx_enablerx(AT91C_BASE_US0, state);

#elif CPUSTYLE_R7S721

	hardware_uartx_enablerx(& SCIF0, state);

#elif CPUSTYLE_XC7Z

	hardware_uartx_enablerx(UART0, state);

#elif CPUSTYLE_ALLWINNER

	hardware_uartx_enablerx(UART0, state);

#elif CPUSTYLE_VM14

	hardware_uartx_enablerx(UART0, state);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uart0_tx(void * ctx, uint_fast8_t c)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		hardware_uartx_tx(USART0, c);
	#elif HARDWARE_ARM_USEUART0
		hardware_uartx_tx(UART0, c);
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	hardware_uartx_tx(AT91C_BASE_US0, c);

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	hardware_uartx_tx(USART0, c);

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32MP1

	hardware_uartx_tx(UART0, c);

#elif CPUSTYLE_R7S721

	hardware_uartx_tx(& SCIF0, c);

#elif CPUSTYLE_STM32MP1

	hardware_uartx_tx(UART0, c);

#elif CPUSTYLE_XC7Z

	hardware_uartx_tx(UART0, c);

#elif CPUSTYLE_ALLWINNER

	hardware_uartx_tx(UART0, c);

#elif CPUSTYLE_VM14

	hardware_uartx_tx(UART0, c);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* дождаться, когда буде все передано */
void hardware_uart0_flush(void)
{
#if CPUSTYLE_ALLWINNER

	hardware_uartx_flush(UART0);

#elif CPUSTYLE_VM14

	hardware_uartx_flush(UART0);

#else
	//#error Undefined CPUSTYLE_XXX
#endif
}

/* приём символа, если готов порт */
uint_fast8_t
hardware_uart0_getchar(char * cp)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		hardware_uartx_getchar(USART0, cp);
	#elif HARDWARE_ARM_USEUART0
		hardware_uartx_getchar(UART0, cp);
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	return hardware_uartx_getchar(AT91C_BASE_US0, cp);

#elif CPUSTYLE_R7S721

	return hardware_uartx_getchar(SCIF0, cp);

#elif CPUSTYLE_XC7Z

	return hardware_uartx_getchar(UART0, cp);

#elif CPUSTYLE_ALLWINNER

	return hardware_uartx_getchar(UART0, cp);

#elif CPUSTYLE_VM14

	return hardware_uartx_getchar(UART0, cp);

#else
	#error Undefined CPUSTYLE_XXX
#endif

	return 1;
}

/* передача символа если готов порт */
uint_fast8_t
hardware_uart0_putchar(uint_fast8_t c)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		return hardware_uartx_putchar(USART0, c);
	#elif HARDWARE_ARM_USEUART0
		return hardware_uartx_putchar(UART0, c);
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	return hardware_uartx_putchar(AT91C_BASE_US0, c);

#elif CPUSTYLE_R7S721

	return hardware_uartx_putchar(& SCIF0, c);

#elif CPUSTYLE_XC7Z

	return hardware_uartx_putchar(UART0, c);

#elif CPUSTYLE_ALLWINNER

	return hardware_uartx_putchar(UART0, c);

#elif CPUSTYLE_VM14

	return hardware_uartx_putchar(UART0, c);

#else
	#error Undefined CPUSTYLE_XXX
#endif

	return 1;
}

void hardware_uart0_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd)
{
	int fifo = 1;
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		// enable the clock of USART0
		PMC->PMC_PCER0 = 1u << ID_USART0;

		HARDWARE_UART0_INITIALIZE();	/* Присоединить периферию к выводам */

		hardware_uartx_initialize(USART0, busfreq, defbaudrate, bits, parity, odd, fifo);

		if (debug == 0)
		{
			serial_set_handler(USART0_IRQn, & USART0_IRQHandler);
		}

	#elif HARDWARE_ARM_USEUART0
		// enable the clock of UART0
		PMC->PMC_PCER0 = 1u << ID_UART0;

		HARDWARE_UART0_INITIALIZE();	/* Присоединить периферию к выводам */

		hardware_uartx_initialize(UART0, busfreq, defbaudrate, bits, parity, odd, fifo);

		if (debug == 0)
		{
			serial_set_handler(UART0_IRQn, & UART0_IRQHandler);
		}

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_AT91SAM7S

	// enable the clock of USART0
	AT91C_BASE_PMC->PMC_PCER = 1u << AT91C_ID_US0;

	HARDWARE_UART0_INITIALIZE();	/* Присоединить периферию к выводам */

	hardware_uartx_initialize(AT91C_BASE_US0, busfreq, defbaudrate, bits, parity, odd, fifo);

	if (debug == 0)
	{
		serial_set_handler(AT91C_ID_US0, AT91F_US0Handler);
	}


#elif CPUSTYLE_R7S721

    /* ---- Supply clock to the SCIF(channel 0) ---- */
	CPG.STBCR4 &= ~ CPG_STBCR4_BIT_MSTP47;	// Module Stop 47 SCIF0
	(void) CPG.STBCR4;			/* Dummy read */

	hardware_uartx_initialize(UART0, busfreq, defbaudrate, bits, parity, odd, fifo);

	if (debug == 0)
	{
	   serial_set_handler(SCIFRXI0_IRQn, SCIFRXI0_IRQHandler);
	   serial_set_handler(SCIFTXI0_IRQn, SCIFTXI0_IRQHandler);
	}
	HARDWARE_UART0_INITIALIZE();	/* Присоединить периферию к выводам */

#elif CPUSTYLE_XC7Z

	const unsigned ix = 0;
	SCLR->SLCR_UNLOCK = 0x0000DF0DU;
	SCLR->APER_CLK_CTRL |= (1u << (20 + ix));	// APER_CLK_CTRL.UART1_CPU_1XCLKACT
	//EMIT_MASKWRITE(0XF8000154, 0x00003F33U ,0x00001002U),	// UART_CLK_CTRL
	SCLR->UART_CLK_CTRL = (SCLR->UART_CLK_CTRL & ~ (0x00003F30U)) |
			((uint_fast32_t) SCLR_UART_CLK_CTRL_DIVISOR_VALUE << 8) | // DIVISOR
			(0x00u << 4) |	// SRCSEL - 0x: IO PLL
			(0x01 << ix) |	// CLKACT0 - UART 0 reference clock active
			0;

	hardware_uartx_initialize(UART0, busfreq, defbaudrate, bits, parity, odd, fifo);

	HARDWARE_UART0_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
	   serial_set_handler(UART0_IRQn, UART0_IRQHandler);
	}

#elif CPUSTYLE_A64 || CPUSTYLE_H3

	const unsigned ix = 0;

	/* Open the clock gate for uart0 */
	CCU->BUS_CLK_GATING_REG3 |= (1u << (ix + 16));	// UART0_GATING

	/* De-assert uart0 reset */
	CCU-> BUS_SOFT_RST_REG4 |= (1u << (ix + 16));	//  UART0_RST

	hardware_uartx_initialize(UART0, HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);

	HARDWARE_UART0_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART0_IRQn, UART0_IRQHandler);
	}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_T507 || CPUSTYLE_H616)

	const unsigned ix = 0;

	/* Open the clock gate for uart0 */
	CCU->UART_BGR_REG |= (1u << (ix + 0));

	/* De-assert uart0 reset */
	CCU->UART_BGR_REG |= (1u << (ix + 16));


	hardware_uartx_initialize(UART0, HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);

	HARDWARE_UART0_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART0_IRQn, UART0_IRQHandler);
	}

#elif (CPUSTYLE_A133 || CPUSTYLE_R828)

	const unsigned ix = 0;

	/* Open the clock gate for uart0 */
	CCU->UART_BGR_REG |= (1u << (ix + 0));

	/* De-assert uart0 reset */
	CCU->UART_BGR_REG |= (1u << (ix + 16));


	hardware_uartx_initialize(UART0, HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);

	HARDWARE_UART0_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART0_IRQn, UART0_IRQHandler);
	}

#elif CPUSTYLE_V3S

	const unsigned ix = 0;

	/* Open the clock gate for uart0 */
	CCU->BUS_CLK_GATING_REG3 |= (1u << (ix + 16));

	/* De-assert uart0 reset */
	CCU->BUS_SOFT_RST_REG4 |= (1u << (ix + 16));


	hardware_uartx_initialize(UART0, HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);

	HARDWARE_UART0_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART0_IRQn, UART0_IRQHandler);
	}

#elif CPUSTYLE_VM14

	const unsigned ix = 0;	// UART0

	CMCTR->GATE_SYS_CTR |= ((1u << 12) << ix); // UART0_EN Enable CLK

	hardware_uartx_initialize(UART0, elveesvm14_get_usart_freq(), defbaudrate, bits, parity, odd, fifo);

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

#elif CPUSTYLE_ATMEGA_XXX4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR0A |= (1U << U2X0);
	else
		UCSR0A &= ~ (1U << U2X0);

	UBRR0 = value;	/* Значение получено уже уменьшенное на 1 */


#elif CPUSTYLE_ATMEGA128

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR0A |= (1U << U2X0);
	else
		UCSR0A &= ~ (1U << U2X0);

	UBRR0H = (value >> 8) & 0xff;	/* Значение получено уже уменьшенное на 1 */
	UBRR0L = value & 0xff;

#elif CPUSTYLE_ATMEGA

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSRA |= (1U << U2X);
	else
		UCSRA &= ~ (1U << U2X);

	UBRRH = (value >> 8) & 0xff;	/* Значение получено уже уменьшенное на 1 */
	UBRRL = value & 0xff;

#elif CPUSTYLE_ATXMEGAXXXA4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATXMEGA_UBR_WIDTH, ATXMEGA_UBR_TAPS, & value, 1);
	if (prei == 0)
		USARTE0.CTRLB |= USART_CLK2X_bm;
	else
		USARTE0.CTRLB &= ~USART_CLK2X_bm;
	// todo: проверить требование к порядку обращения к портам
	USARTE0.BAUDCTRLA = (value & 0xff);	/* Значение получено уже уменьшенное на 1 */
	USARTE0.BAUDCTRLB = (ATXMEGA_UBR_BSEL << 4) | ((value >> 8) & 0x0f);

#elif CPUSTYLE_R7S721

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(baudrate), R7S721_SCIF_SCBRR_WIDTH, R7S721_SCIF_SCBRR_TAPS, & value, 1);

	SCIF0.SCSMR = (SCIF0.SCSMR & ~ 0x03) |
		scemr_scsmr [prei].scsmr |	// prescaler: 0: /1, 1: /4, 2: /16, 3: /64
		0;
	SCIF0.SCEMR = (SCIF0.SCEMR & ~ (0x80 | 0x01)) |
		0 * 0x80 |						// BGDM
		scemr_scsmr [prei].scemr |	// ABCS = 8/16 clocks per bit
		0;
	SCIF0.SCBRR = value;	/* Bit rate register */

#elif CPUSTYLE_XC7Z

	  uint32_t r; // Temporary value variable
	  r = UART0->CR;
	  r &= ~(XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN); // Clear Tx & Rx Enable
	  r |= XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS; // Tx & Rx Disable
	  UART0->CR = r;
	  const unsigned long sel_clk = xc7z_get_uart_freq();
	  const unsigned long bdiv = 8;
	  // baud_rate = sel_clk / (CD * (BDIV + 1) (ref: UG585 - TRM - Ch. 19 UART)
	  UART0->BAUDDIV = bdiv - 1; // ("BDIV")
	  UART0->BAUDGEN = calcdivround2(sel_clk, baudrate * bdiv); // ("CD")
	  // Baud Rate = 100Mhz / (124 * (6 + 1)) = 115200 bps
	  UART0->CR |= (XUARTPS_CR_TXRST | XUARTPS_CR_RXRST); // TX & RX logic reset

	  r = UART0->CR;
	  r |= XUARTPS_CR_RX_EN | XUARTPS_CR_TX_EN; // Set TX & RX enabled
	  r &= ~(XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS); // Clear TX & RX disabled
	  UART0->CR = r;

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616 || CPUSTYLE_V3S || CPUSTYLE_H3 || CPUSTYLE_A133 || CPUSTYLE_R818)

	unsigned divisor = calcdivround2(HARDWARE_UART_FREQ, baudrate * 16);

	UART0->UART_LCR |= (1 << 7);
	UART0->UART_RBR_THR_DLL = divisor & 0xff;
	UART0->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART0->UART_LCR &= ~ (1 << 7);

#elif CPUSTYLE_VM14

	unsigned divisor = calcdivround2(elveesvm14_get_usart_freq(), baudrate * 16);

	while ((UART0->UART_USR & (1u << 2)) == 0)	/* TFE - FIFO передатчика пуст. */
		;
	while ((UART0->UART_USR & (1u << 0)) != 0)	/* BUSY - UART занят. */
	{
		/* todo: решить проблему с принимаемыми символами */
	}
	UART0->UART_LCR |= (1 << 7);
	UART0->UART_RBR_THR_DLL = divisor & 0xff;
	UART0->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART0->UART_LCR &= ~ (1 << 7);
	(void) UART0->UART_LCR;

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART0HW */
