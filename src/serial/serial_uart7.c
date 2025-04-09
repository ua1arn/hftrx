/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if WITHUART7HW

#include "serial.h"
#include "formats.h"	// for debug prints
#include "board.h"
#include "gpio.h"
#include <string.h>
#include <math.h>
#include "clocks.h"

#define thisPORT 7

#if CPUSTYLE_STM32MP1
	#undef UARTBASENAME
	#define UARTBASENAME(port) UARTBASEconcat(UART, port)
#endif

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

void RAMFUNC_NONILINE USART7_Handler(void)
{
	const uint_fast32_t csr = USART7->US_CSR;

	if (csr & US_CSR_RXRDY)
		HARDWARE_UART7_ONRXCHAR(USART7->US_RHR);
	if (csr & US_CSR_TXRDY)
		HARDWARE_UART7_ONTXCHAR(USART7);
}

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

void RAMFUNC_NONILINE UART7_IRQHandler(void)
{
	const uint_fast32_t sr = UART7->SR;

	if (sr & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE))
		HARDWARE_UART7_ONRXCHAR(UART7->DR);
	if (sr & (USART_SR_ORE | USART_SR_FE | USART_SR_NE))
		HARDWARE_UART7_ONOVERFLOW();
	if (sr & USART_SR_TXE)
		HARDWARE_UART7_ONTXCHAR(UART7);
}

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

void RAMFUNC_NONILINE UART7_IRQHandler(void)
{
	const uint_fast32_t isr = UART7->ISR;
	const uint_fast32_t cr1 = UART7->CR1;

	if (cr1 & USART_CR1_RXNEIE)
	{
		if (isr & USART_ISR_RXNE_RXFNE)
		{
			const uint_fast8_t c = UART7->RDR;
			HARDWARE_UART7_ONRXCHAR(c);
		}
		if (isr & USART_ISR_ORE)
		{
			UART7->ICR = USART_ICR_ORECF;
			HARDWARE_UART7_ONOVERFLOW();
		}
		if (isr & USART_ISR_FE)
			UART7->ICR = USART_ICR_FECF;
	}
	if (cr1 & USART_CR1_TXEIE)
	{
		if (isr & USART_ISR_TXE_TXFNF)
			HARDWARE_UART7_ONTXCHAR(UART7);
	}
}

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

void RAMFUNC_NONILINE UART7_IRQHandler(void)
{
	const uint_fast32_t isr = UART7->ISR;

	if (isr & USART_ISR_RXNE)
		HARDWARE_UART7_ONRXCHAR(UART7->RDR);
	if (isr & USART_ISR_ORE)
	{
		UART7->ICR = USART_ICR_ORECF;
		HARDWARE_UART7_ONOVERFLOW();
	}
	if (isr & USART_ISR_FE)
		UART7->ICR = USART_ICR_FECF;
	if (isr & USART_ISR_TXE)
		HARDWARE_UART7_ONTXCHAR(UART7);
}

#elif CPUSTYLE_R7S721

// Приём символа он последовательного порта
static void SCIFRXI7_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	(void) uart->SCFSR;						// Перед сбросом бита RDF должно произойти его чтение в ненулевом состоянии
	uart->SCFSR = (uint16_t) ~ SCIF7_SCFSR_RDF;	// RDF=0 читать незачем (в примерах странное - сбрасывабтся и другие биты)
	if (uart->SCSCR & SCIF7_SCSCR_RIE)	// RIE Receive Interrupt Enable
	{
		uint_fast8_t n = (uart->SCFDR & SCIF7_SCFDR_R) >> SCIF7_SCFDR_R_SHIFT;
		while (n --)
			HARDWARE_UART7_ONRXCHAR(uart->SCFRDR & SCIF7_SCFRDR_D);
	}
}

// Передача символа в последовательный порт
static void SCIFTXI7_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	if (uart->SCSCR & SCIF7_SCSCR_TIE)	// TIE Transmit Interrupt Enable
	{
		HARDWARE_UART7_ONTXCHAR(uart);
	}
}

#elif CPUSTYLE_ALLWINNER

static RAMFUNC_NONILINE void UART7_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	const uint_fast32_t ier = uart->UART_DLH_IER;
	const uint_fast32_t usr = uart->UART_USR;

	if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
	{
		if (usr & (1u << 3))	// RX FIFO Not Empty
			HARDWARE_UART7_ONRXCHAR(uart->UART_RBR_THR_DLL);
	}
	if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
	{
		if (usr & (1u << 1))	// TX FIFO Not Full
			HARDWARE_UART7_ONTXCHAR(uart);
	}
}

#elif CPUSTYLE_ROCKCHIP

static RAMFUNC_NONILINE void UART7_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	const uint_fast32_t ier = uart->UART_DLH_IER;
	const uint_fast32_t usr = uart->UART_USR;

	if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
	{
		if (usr & (1u << 3))	// RX FIFO Not Empty
			HARDWARE_UART7_ONRXCHAR(uart->UART_RBR_THR_DLL);
	}
	if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
	{
		if (usr & (1u << 1))	// TX FIFO Not Full
			HARDWARE_UART7_ONTXCHAR(uart);
	}
}

#else
	#error Undefined CPUSTYLE_XXX
#endif	/* CPUSTYLE_ATMEGA_XXX4 */


/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uart7_enabletx(uint_fast8_t state)
{
	hardware_uartx_enabletx(UARTBASENAME(thisPORT), state);
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uart7_enablerx(uint_fast8_t state)
{
	hardware_uartx_enablerx(UARTBASENAME(thisPORT), state);
}

/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uart7_tx(void * ctx, uint_fast8_t c)
{
	hardware_uartx_tx(UARTBASENAME(thisPORT), c);
}

/* дождаться, когда буде все передано */
void hardware_uart7_flush(void)
{
	hardware_uartx_flush(UARTBASENAME(thisPORT));
}

/* приём символа, если готов порт */
uint_fast8_t
hardware_uart7_getchar(char * cp)
{
	return hardware_uartx_getchar(UARTBASENAME(thisPORT), cp);
}

/* передача символа если готов порт */
uint_fast8_t
hardware_uart7_putchar(uint_fast8_t c)
{
	return hardware_uartx_putchar(UARTBASENAME(thisPORT), c);
}

void hardware_uart7_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate)
{
	int fifo = 1;
#if CPUSTYLE_STM32F1XX

	RCC->APB1ENR |= RCC_APB1ENR_UART7EN; // Включение тактирования UART7.
	(void) RCC->APB1ENR;

	UART7->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables


	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     //включить тактирование альтернативных функций
	(void) RCC->APB2ENR;

	HARDWARE_UART7_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(UART7_IRQn, & UART7_IRQHandler);
	}

	UART7->CR1 |= USART_CR1_UE; // Включение UART4.

#elif CPUSTYLE_STM32H7XX

	RCC->APB1LENR |= RCC_APB1LENR_UART7EN; // Включение тактирования UART4.
	(void) RCC->APB1LENR;

	UART7->CR1 = 0;

#if WITHUART7HW_FIFO
	UART7->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUART7HW_FIFO */
	UART7->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUART7HW_FIFO */

	UART7->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART7_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(UART7_IRQn, & UART7_IRQHandler);
	}

	UART7->CR1 |= USART_CR1_UE; // Включение UART4.

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	RCC->APB1ENR |= RCC_APB1ENR_UART7EN; // Включение тактирования UART7.
	(void) RCC->APB1ENR;

	UART7->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART7_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(UART7_IRQn, & UART7_IRQHandler);
	}

	UART7->CR1 |= USART_CR1_UE; // Включение UART4.

#elif CPUSTYLE_R7S721

    /* ---- Supply clock to the SCIF(channel 1) ---- */
	CPG.STBCR4 &= ~ CPG_STBCR4_BIT_MSTP40;	// Module Stop 40 - SCIF7
	(void) CPG.STBCR4;			/* Dummy read */

	SCIF7.SCSCR = 0x0000;	/* SCIF transmitting and receiving operations stop, internal clock */

	SCIF7.SCSCR = (SCIF7.SCSCR & ~ 0x03) |
		0x00 |						// internal clock
		0;

	/* ---- Serial status register(SCFSR2) setting ---- */
	/* ER,BRK,DR bit clear */
	(void) SCIF7.SCFSR;						// Перед сбросом бита xxx должно произойти его чтение в ненулевом состоянии
	SCIF7.SCFSR = ~ 0x0091;	// 0xFF6E;

	/* ---- Line status register (SCLSR2) setting ---- */
	/* ORER bit clear */
	//SCIF7.SCLSR.BIT.ORER  = 0;
	SCIF7.SCLSR &= ~ 0x0001;

	SCIF7.SCSMR =
		0x00 |	/* 8-N-1 format */
		0;

	/* ---- FIFO control register (SCFCR2) setting ---- */
	/*  RTS output active trigger        :Initial value	*/
	/*  Receive FIFO data trigger        :1-data		*/
	/*  Transmit FIFO data trigger       :0-data		*/
	/*  Modem control enable             :Disabled		*/
	/*  Receive FIFO data register reset :Disabled		*/
	/*  Loop-back test                   :Disabled 		*/
	SCIF7.SCFCR = 0x0030;

	/* ---- Serial port register (SCSPTR2) setting ---- */
	/* Serial port  break output(SPB2IO)  1: Enabled */
	/* Serial port break data(SPB2DT)  1: High-level */
	//SCIF7.SCSPTR |= 0x0003;

	if (debug == 0)
	{
	   serial_set_handler(SCIFRXI3_IRQn, SCIFRXI3_IRQHandler);
	   serial_set_handler(SCIFTXI3_IRQn, SCIFTXI3_IRQHandler);
	}
	HARDWARE_UART7_INITIALIZE();	/* Присоединить периферию к выводам */

	SCIF7.SCSCR |= 0x0030;	// TE RE - SCIF7 transmitting and receiving operations are enabled */

#elif CPUSTYLE_STM32MP1

	RCC->MP_APB1ENSETR = RCC_MP_APB1ENSETR_UART7EN; // Включение тактирования UART7.
	(void) RCC->MP_APB1ENSETR;
	RCC->MP_APB1LPENSETR = RCC_MP_APB1LPENSETR_UART7LPEN; // Включение тактирования UART7.
	(void) RCC->MP_APB1LPENSETR;
	RCC->APB1RSTSETR = RCC_APB1RSTSETR_UART7RST; // Установить сброс UART7.
	(void) RCC->APB1RSTSETR;
	RCC->APB1RSTCLRR = RCC_APB1RSTCLRR_UART7RST; // Снять брос UART7.
	(void) RCC->APB1RSTCLRR;

	hardware_uartx_initialize(UARTBASENAME(thisPORT), stm32mp1_uart7_8_get_freq(), defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART7_INITIALIZE();	/* Присоединить периферию к выводам */
	if (debug == 0)
	{
		serial_set_handler(UART7_IRQn, UART7_IRQHandler);
	}

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

void
hardware_uart7_set_speed(uint_fast32_t baudrate)
{
#if CPUSTYLE_STM32MP1

	// uart7
	hardware_uartx_set_speed(UARTBASENAME(thisPORT), stm32mp1_uart7_8_get_freq(), baudrate);

#elif CPUSTYLE_STM32F

	// uart7 on apb1
	hardware_uartx_set_speed(UARTBASENAME(thisPORT), BOARD_UART7_FREQ, baudrate);

#elif CPUSTYLE_R7S721

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), P1CLOCK_FREQ, baudrate);

#elif CPUSTYLE_ALLWINNER

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, baudrate);

#elif CPUSTYLE_ROCKCHIP

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, baudrate);

#elif CPUSTYLE_VM14

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), elveesvm14_get_usart_freq(), baudrate);

#else
	#warning Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART7HW */
