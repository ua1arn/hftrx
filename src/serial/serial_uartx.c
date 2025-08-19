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
#include "clocks.h"

#if ! LINUX_SUBSYSTEM

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uartx_enabletx(UART_t * uart, uint_fast8_t state)
{
#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

	if (state)
		uart->CR1 |= USART_CR1_TXEIE;
	else
		uart->CR1 &= ~ USART_CR1_TXEIE;

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	if (state)
		uart->UART_IER = UART_IER_TXRDY;
	else
		uart->UART_IDR = UART_IDR_TXRDY;

#elif CPUSTYLE_AT91SAM7S

	if (state)
		uart->US_IER = AT91C_US_TXRDY;
	else
		uart->US_IDR = AT91C_US_TXRDY;

#elif CPUSTYLE_R7S721

	if (state)
		uart->SCSCR |= SCIF0_SCSCR_TIE;	// TIE Transmit Interrupt Enable
	else
		uart->SCSCR &= ~ SCIF0_SCSCR_TIE;	// TIE Transmit Interrupt Enable

#elif CPUSTYLE_XC7Z

	const uint32_t mask = (UINT32_C(1) << 3);	// TEMPTY Enable Transmit Holding Register Empty Interrupt
	if (state)
	{
		 uart->IER = mask;
	}
	else
	{
		 uart->IDR = mask;
	}

#elif CPUSTYLE_ALLWINNER

	if (state)
		 uart->UART_DLH_IER |= (1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt
	else
		 uart->UART_DLH_IER &= ~ (1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt

#elif CPUSTYLE_VM14

	if (state)
		 uart->UART_DLH_IER |= 0*(1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt
	else
		 uart->UART_DLH_IER &= ~ 0*(1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt

#elif CPUSTYLE_ROCKCHIP

	if (state)
		 uart->UART_DLH_IER |= (1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt
	else
		 uart->UART_DLH_IER &= ~ (1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uartx_enablerx(UART_t * uart, uint_fast8_t state)
{
#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

	if (state)
		uart->CR1 |= USART_CR1_RXNEIE;
	else
		uart->CR1 &= ~ USART_CR1_RXNEIE;

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	if (state)
		uart->UART_IER = UART_IER_RXRDY;
	else
		uart->UART_IDR = UART_IDR_RXRDY;

#elif CPUSTYLE_AT91SAM7S

	if (state)
		uart->US_IER = AT91C_US_RXRDY;
	else
		uart->US_IDR = AT91C_US_RXRDY;


#elif CPUSTYLE_R7S721

	if (state)
		uart->SCSCR |= SCIF0_SCSCR_RIE;	// RIE Receive Interrupt Enable
	else
		uart->SCSCR &= ~ SCIF0_SCSCR_RIE;	// RIE Receive Interrupt Enable

#elif CPUSTYLE_XC7Z

	uart->RXWM = 16; 							/* set RX FIFO Trigger Level */
	const uint32_t mask = (UINT32_C(1) << 8) | (UINT32_C(1) << 5) | (UINT32_C(1) << 0);	/* TIMEOUT, RX FIFO trigger interrupt */
	if (state)
	{
		 uart->IER = mask;
	}
	else
	{
		 uart->IDR = mask;
	}

#elif CPUSTYLE_ALLWINNER

	if (state)
		 uart->UART_DLH_IER |= (1u << 0);	// ERBFI Enable Received Data Available Interrupt
	else
		 uart->UART_DLH_IER &= ~ (1u << 0);	// ERBFI Enable Received Data Available Interrupt

#elif CPUSTYLE_VM14

	if (state)
		 uart->UART_DLH_IER |= (1u << 0);	// ERBFI Enable Received Data Available Interrupt
	else
		 uart->UART_DLH_IER &= ~ (1u << 0);	// ERBFI Enable Received Data Available Interrupt

#elif CPUSTYLE_ROCKCHIP

	if (state)
		 uart->UART_DLH_IER |= (1u << 0);	// ERBFI Enable Received Data Available Interrupt
	else
		 uart->UART_DLH_IER &= ~ (1u << 0);	// ERBFI Enable Received Data Available Interrupt

#else
	#error Undefined CPUSTYLE_XXX
#endif
}


/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uartx_tx(UART_t * uart, uint_fast8_t c)
{
#if CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32MP1

	uart->TDR = c;

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		uart->US_THR = c;
	#elif HARDWARE_ARM_USEUART0
		uart->UART_THR = c;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	uart->US_THR = c;

#elif CPUSTYLE_R7S721

	(void) uart->SCFSR;			// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
	uart->SCFTDR = c;
	uart->SCFSR = (uint16_t) ~ (1U << SCIF0_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#elif CPUSTYLE_STM32MP1
	#warning Insert code for CPUSTYLE_STM32MP1

#elif CPUSTYLE_XC7Z

	uart->FIFO = c;

#elif CPUSTYLE_ALLWINNER

	uart->UART_RBR_THR_DLL = c;

#elif CPUSTYLE_VM14

	uart->UART_RBR_THR_DLL = c;

#elif CPUSTYLE_ROCKCHIP

	uart->UART_RBR_THR_DLL = c;

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* дождаться, когда буде все передано */
void hardware_uartx_flush(UART_t * uart)
{
#if CPUSTYLE_ALLWINNER

	while ((uart->UART_USR & (1u << 2)) == 0)	// TFE Transmit FIFO Empty
		;

#elif CPUSTYLE_VM14

	for (;;)
	{
		if ((uart->UART_USR & (1u << 1)) == 0)	// TFNF TX FIFO Not Full
			continue;
		if ((uart->UART_USR & (1u << 2)) == 0)	// TFE TX FIFO empty
			continue;
		if ((uart->UART_TFL & 0xFF) != 0)
			continue;
	}

#else
	//#error Undefined CPUSTYLE_XXX
#endif
}

/* приём символа, если готов порт */
uint_fast8_t
hardware_uartx_getchar(UART_t * uart, char * cp)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((uart->SR & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE)) == 0)
		return 0;
	* cp = uart->DR;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	const uint_fast32_t isr = uart->ISR;
	if (isr & USART_ISR_ORE)
		uart->ICR = USART_ICR_ORECF;
	if (isr & USART_ISR_FE)
		uart->ICR = USART_ICR_FECF;
	if ((isr & USART_ISR_RXNE_RXFNE) == 0)
		return 0;
	* cp = uart->RDR;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	const uint_fast32_t isr = uart->ISR;
	if (isr & USART_ISR_ORE)
		uart->ICR = USART_ICR_ORECF;
	if (isr & USART_ISR_FE)
		uart->ICR = USART_ICR_FECF;
	if ((isr & USART_ISR_RXNE) == 0)
		return 0;
	* cp = uart->RDR;

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	if ((uart->US_CSR & US_CSR_RXRDY) == 0)
		return 0;
	* cp = uart->US_RHR;

#elif CPUSTYLE_AT91SAM7S

	if ((uart->US_CSR & AT91C_US_RXRDY) == 0)
		return 0;
	* cp = uart->US_RHR;

#elif CPUSTYLE_R7S721

	if ((uart->SCFSR & (1U << 1)) == 0)	// RDF
		return 0;
	* cp = uart->SCFRDR;
	uart->SCFSR = (uint16_t) ~ (1U << 1);	// RDF=0 читать незачем (в примерах странное)

#elif CPUSTYLE_XC7Z

	if ((uart->SR & XUARTPS_SR_RXEMPTY) != 0)
		return 0;
	* cp = uart->FIFO;

#elif CPUSTYLE_ALLWINNER

	if ((uart->UART_USR & (1u << 3)) == 0)	// RX FIFO Not Empty
		return 0;
	* cp = uart->UART_RBR_THR_DLL;

#elif CPUSTYLE_VM14

	if ((uart->UART_USR & (1u << 3)) == 0)	// RFNE - RX FIFO Not Empty
		return 0;
	* cp = uart->UART_RBR_THR_DLL;

#elif CPUSTYLE_ROCKCHIP

	if ((uart->UART_USR & (1u << 3)) == 0)	// RX FIFO Not Empty
		return 0;
	* cp = uart->UART_RBR_THR_DLL;

#else
	#error Undefined CPUSTYLE_XXX
#endif

	return 1;
}

/* передача символа если готов порт */
uint_fast8_t
hardware_uartx_putchar(UART_t * uart, uint_fast8_t c)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((uart->SR & USART_SR_TXE) == 0)
		return 0;
	hardware_uartx_tx(uart, c);

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	if ((uart->ISR & USART_ISR_TXE_TXFNF) == 0)
		return 0;
	hardware_uartx_tx(uart, c);

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	if ((uart->ISR & USART_ISR_TXE) == 0)
		return 0;
	hardware_uartx_tx(uart, c);

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		if ((uart->US_CSR & US_CSR_TXRDY) == 0)
			return 0;
		hardware_uartx_tx(uart, c);
	#elif HARDWARE_ARM_USEUART0
		if ((uart->UART_SR & UART_SR_TXRDY) == 0)
			return 0;
		hardware_uartx_tx(uart, c);
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	if ((uart->US_CSR & AT91C_US_TXRDY) == 0)
		return 0;
	hardware_uartx_tx(uart, c);

#elif CPUSTYLE_R7S721

	if ((uart->SCFSR & (1U << SCIF0_SCFSR_TDFE_SHIFT)) == 0)	// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
		return 0;
	hardware_uartx_tx(uart, c);

#elif CPUSTYLE_XC7Z

	if ((uart->SR & XUARTPS_SR_TNFUL) != 0)
		return 0;
	hardware_uartx_tx(uart, c);

#elif CPUSTYLE_ALLWINNER

	if ((uart->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		return 0;
	hardware_uartx_tx(uart, c);

#elif CPUSTYLE_VM14

	if ((uart->UART_USR & (1u << 1)) == 0)	// TFNF TX FIFO Not Full
		return 0;
	hardware_uartx_tx(uart, c);

#elif CPUSTYLE_ROCKCHIP

	if ((uart->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		return 0;
	hardware_uartx_tx(uart, c);

#else
	#error Undefined CPUSTYLE_XXX
#endif

	return 1;
}

void hardware_uartx_initialize(UART_t * uart, uint_fast32_t busfreq, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd, uint_fast8_t fifo)
{
#if CPUSTYLE_STM32MP1

	uart->CR1 = 0;
	if (fifo)
	{
		uart->CR1 |= USART_CR1_FIFOEN_Msk;
	}
	else
	{
		uart->CR1 &= ~ USART_CR1_FIFOEN_Msk;
	}
	uart->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables
	uart->CR1 |= USART_CR1_UE; // Включение USART.

#elif CPUSTYLE_STM32F1XX

	uart->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables
	uart->CR1 |= USART_CR1_UE; // Включение USART.

#elif CPUSTYLE_STM32H7XX

	if (fifo)
	{
		uart->CR1 |= USART_CR1_FIFOEN_Msk;
	}
	else
	{
		uart->CR1 &= ~ USART_CR1_FIFOEN_Msk;
	}
	uart->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables
	uart->CR1 |= USART_CR1_UE; // Включение USART2.

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	uart->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables
	uart->CR1 |= USART_CR1_UE; // Включение USART2.

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// reset the UART
	uart->US_CR = US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS;
	// set serial line mode
	uart->US_MR =
		US_MR_CHMODE_NORMAL |// Normal Mode
		US_MR_USCLKS_MCK |   // Clock = MCK
		US_MR_CHRL_8_BIT |
		US_MR_PAR_NO |
		US_MR_NBSTOP_1_BIT;

	uart->US_IDR = (US_IDR_RXRDY | US_IDR_TXRDY);
	uart->US_CR = US_CR_RXEN | US_CR_TXEN;	// разрешаем приёмник и передатчик.

#elif CPUSTYLE_AT91SAM7S

	// disable I/O pullup
	// reset the UART
	uart->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS;
	// set serial line mode
	uart->US_MR =
		AT91C_US_OVER |
		AT91C_US_USMODE_NORMAL |// Normal Mode
		AT91C_US_CLKS_CLOCK |   // Clock = MCK
		AT91C_US_CHRL_8_BITS |
		AT91C_US_PAR_NONE |
		AT91C_US_NBSTOP_1_BIT;
	uart->US_IDR = (AT91C_US_RXRDY | AT91C_US_TXRDY);
	uart->US_CR = AT91C_US_RXEN  | AT91C_US_TXEN;	// разрешаем приёмник и передатчик.

#elif CPUSTYLE_R7S721

	uart->SCSCR = 0x0000;	/* SCIF transmitting and receiving operations stop, internal clock */

	uart->SCSCR = (uart->SCSCR & ~ 0x03) |
		0x00 |						// internal clock
		0;

	/* ---- Serial status register(SCFSR2) setting ---- */
	/* ER,BRK,DR bit clear */
	(void) uart->SCFSR;						// Перед сбросом бита xxx должно произойти его чтение в ненулевом состоянии
	uart->SCFSR = ~ 0x0091;	// 0xFF6E;

	/* ---- Line status register (SCLSR2) setting ---- */
	/* ORER bit clear */
	//uart->SCLSR.BIT.ORER  = 0;
	uart->SCLSR &= ~ 0x0001;

	uart->SCSMR =
		0x00 |	/* 8-N-1 format */
		0;

	/* ---- FIFO control register (SCFCR2) setting ---- */
	/*  RTS output active trigger        :Initial value	*/
	/*  Receive FIFO data trigger        :1-data		*/
	/*  Transmit FIFO data trigger       :0-data		*/
	/*  Modem control enable             :Disabled		*/
	/*  Receive FIFO data register reset :Disabled		*/
	/*  Loop-back test                   :Disabled 		*/
	uart->SCFCR = 0x0030;

	/* ---- Serial port register (SCSPTR2) setting ---- */
	/* Serial port  break output(SPB2IO)  1: Enabled */
	/* Serial port break data(SPB2DT)  1: High-level */
	//uart->SCSPTR |= 0x0003;

	uart->SCSCR |= 0x0030;	// TE RE - SCIF0 transmitting and receiving operations are enabled */

#elif CPUSTYLE_XC7Z

	uint32_t r; // Temporary value variable
	r = uart->CR;
	r &= ~(XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN); // Clear Tx & Rx Enable
	r |= XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS; // Tx & Rx Disable
	uart->CR = r;

	uart->MR = 0;
	uart->MR &= ~XUARTPS_MR_CLKSEL; // Clear "Input clock selection" - 0: clock source is uart_ref_clk
	uart->MR |= XUARTPS_MR_CHARLEN_8_BIT; 	// Set "8 bits data"
	uart->MR |= XUARTPS_MR_PARITY_NONE; 	// Set "No parity mode"
	uart->MR |= XUARTPS_MR_STOPMODE_1_BIT; // Set "1 stop bit"
	uart->MR |= XUARTPS_MR_CHMODE_NORM; 	// Set "Normal mode"

	uart->CR |= (XUARTPS_CR_TXRST | XUARTPS_CR_RXRST); // TX & RX logic reset

	r = uart->CR;
	r |= XUARTPS_CR_RX_EN | XUARTPS_CR_TX_EN; // Set TX & RX enabled
	r &= ~(XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS); // Clear TX & RX disabled
	uart->CR = r;

	r = (UINT32_C(1) << 3);	// TEMPTY Enable Transmit Holding Register Empty Interrupt
	uart->IDR = r;
	uart->RXWM = 16; 							/* set RX FIFO Trigger Level */
	r = (UINT32_C(1) << 8) | (UINT32_C(1) << 5) | (UINT32_C(1) << 0);	/* TIMEOUT, RX FIFO trigger interrupt */
	uart->IDR = r;

#elif CPUSTYLE_ALLWINNER

	uint32_t divisor = calcdivround2(busfreq, defbaudrate * 16);

	uart->UART_DLH_IER = 0;
	uart->UART_IIR_FCR = 0xf7;
	uart->UART_MCR = 0x00;

	uart->UART_LCR |= (UINT32_C(1) << 7);	// Divisor Latch Access Bit
	uart->UART_RBR_THR_DLL = divisor & 0xff;
	uart->UART_DLH_IER = (divisor >> 8) & 0xff;
	uart->UART_LCR &= ~ (UINT32_C(1) << 7);	// Divisor Latch Access Bit
	//
	uart->UART_LCR = (uart->UART_LCR & ~ UINT32_C(0x3F)) |
			((UINT32_C(0x03) & (bits - 5)) << 0) | (0 << 2) | // DAT_LEN_8_BITS ONE_STOP_BIT
			(!! odd << 4) |	// bit5:bit4 0 - even, 1 - odd
			(!! parity << 3) |	// bit3 1: parity enable
			0;

#elif CPUSTYLE_VM14

	/* Config uart0 to 115200-8-1-0 */
	uint32_t divisor = busfreq / ((defbaudrate) * 16);

	uart->UART_DLH_IER = 0;
	uart->UART_IIR_FCR = 0xf7;
	uart->UART_MCR = 0x00;

	uart->UART_LCR |= (1 << 7);	// Divisor Latch Access Bit
	uart->UART_RBR_THR_DLL = divisor & 0xff;
	uart->UART_DLH_IER = (divisor >> 8) & 0xff;
	uart->UART_LCR &= ~ (1 << 7);	// Divisor Latch Access Bit
	//
	uart->UART_LCR &= ~ 0x3f;
	uart->UART_LCR |=
			((0x03 & (bits - 5)) << 0) | (0 << 2) | // DAT_LEN_8_BITS ONE_STOP_BIT
			(! odd << 4) |	// bit4 0 – нечетность,
			(!! parity << 3) |	// bit3 1: parity enable
			0;

	(void) uart->UART_LCR;

#elif CPUSTYLE_ROCKCHIP

	uint32_t divisor = busfreq / ((defbaudrate) * 16);

	uart->UART_DLH_IER = 0;
	uart->UART_IIR_FCR = 0xf7;
	uart->UART_MCR = 0x00;

	uart->UART_LCR |= (UINT32_C(1) << 7);	// Divisor Latch Access Bit
	uart->UART_RBR_THR_DLL = divisor & 0xff;
	uart->UART_DLH_IER = (divisor >> 8) & 0xff;
	uart->UART_LCR &= ~ (UINT32_C(1) << 7);	// Divisor Latch Access Bit
	//
	uart->UART_LCR = (uart->UART_LCR & ~ UINT32_C(0x3F)) |
			((UINT32_C(0x03) & (bits - 5)) << 0) | (0 << 2) | // DAT_LEN_8_BITS ONE_STOP_BIT
			(!! odd << 4) |	// bit5:bit4 0 - even, 1 - odd
			(!! parity << 3) |	// bit3 1: parity enable
			0;

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

void
hardware_uartx_set_speed(UART_t * uart, uint_fast32_t busfreq, uint_fast32_t baudrate)
{
#if CPUSTYLE_STM32MP1

	uart->BRR = calcdivround2(busfreq, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_STM32F

	uart->BRR = calcdivround2(busfreq, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(busfreq, baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
	uart->US_BRGR = value;
	if (prei == 0)
	{
		uart->US_MR |= US_MR_OVER;
	}
	else
	{
		uart->US_MR &= ~ US_MR_OVER;
	}

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(busfreq, baudrate), AT91SAM7_USART_BRGR_WIDTH, AT91SAM7_USART_BRGR_TAPS, & value, 0);

	uart->US_BRGR = value;
	if (prei == 0)
	{
		uart->US_MR |= AT91C_US_OVER;
	}
	else
	{
		uart->US_MR &= ~ AT91C_US_OVER;
	}


#elif CPUSTYLE_R7S721

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(busfreq, baudrate), R7S721_SCIF_SCBRR_WIDTH, R7S721_SCIF_SCBRR_TAPS, & value, 1);

	uart->SCSMR = (uart->SCSMR & ~ 0x03) |
		scemr_scsmr [prei].scsmr |	// prescaler: 0: /1, 1: /4, 2: /16, 3: /64
		0;
	uart->SCEMR = (uart->SCEMR & ~ (0x80 | 0x01)) |
		0 * 0x80 |						// BGDM
		scemr_scsmr [prei].scemr |	// ABCS = 8/16 clocks per bit
		0;
	uart->SCBRR = value;	/* Bit rate register */

#elif CPUSTYLE_XC7Z

	  uint32_t r; // Temporary value variable
	  r = uart->CR;
	  r &= ~(XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN); // Clear Tx & Rx Enable
	  r |= XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS; // Tx & Rx Disable
	  uart->CR = r;
	  const unsigned long sel_clk = busfreq;
	  const unsigned long bdiv = 8;
	  // baud_rate = sel_clk / (CD * (BDIV + 1) (ref: UG585 - TRM - Ch. 19 UART)
	  uart->BAUDDIV = bdiv - 1; // ("BDIV")
	  uart->BAUDGEN = calcdivround2(sel_clk, baudrate * bdiv); // ("CD")
	  // Baud Rate = 100Mhz / (124 * (6 + 1)) = 115200 bps
	  uart->CR |= (XUARTPS_CR_TXRST | XUARTPS_CR_RXRST); // TX & RX logic reset

	  r = uart->CR;
	  r |= XUARTPS_CR_RX_EN | XUARTPS_CR_TX_EN; // Set TX & RX enabled
	  r &= ~(XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS); // Clear TX & RX disabled
	  uart->CR = r;

#elif CPUSTYLE_ALLWINNER

	const unsigned divisor = calcdivround2(busfreq, baudrate * 16);

	uart->UART_LCR |= (1 << 7);
	uart->UART_RBR_THR_DLL = divisor & 0xff;
	uart->UART_DLH_IER = (divisor >> 8) & 0xff;
	uart->UART_LCR &= ~ (1 << 7);

#elif CPUSTYLE_VM14

	const unsigned divisor = calcdivround2(busfreq, baudrate * 16);

	while ((uart->UART_USR & (1u << 2)) == 0)	/* TFE - FIFO передатчика пуст. */
		;
	while ((uart->UART_USR & (1u << 0)) != 0)	/* BUSY - UART занят. */
	{
		/* todo: решить проблему с принимаемыми символами */
	}
	uart->UART_LCR |= (1 << 7);
	uart->UART_RBR_THR_DLL = divisor & 0xff;
	uart->UART_DLH_IER = (divisor >> 8) & 0xff;
	uart->UART_LCR &= ~ (1 << 7);
	(void) uart->UART_LCR;

#elif CPUSTYLE_ROCKCHIP

	const unsigned divisor = calcdivround2(busfreq, baudrate * 16);

	uart->UART_LCR |= (1 << 7);
	uart->UART_RBR_THR_DLL = divisor & 0xff;
	uart->UART_DLH_IER = (divisor >> 8) & 0xff;
	uart->UART_LCR &= ~ (1 << 7);

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

#endif /* ! LINUX_SUBSYSTEM */
