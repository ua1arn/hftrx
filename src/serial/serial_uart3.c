/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if WITHUART3HW

#include "serial.h"
#include "formats.h"	// for debug prints
#include "board.h"
#include "gpio.h"
#include <string.h>
#include <math.h>
#include "clocks.h"

#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1 || CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX
	typedef USART_TypeDef UART_t;
	#undef UARTBASENAME
	#define UARTBASENAME(port) UARTBASEconcat(USART, port)
#endif

#define thisPORT 3

#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

void RAMFUNC_NONILINE USART3_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	const uint_fast32_t sr = USART3->SR;

	if (sr & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE))
		HARDWARE_UART3_ONRXCHAR(USART3->DR);
	if (sr & (USART_SR_ORE | USART_SR_FE | USART_SR_NE))
		HARDWARE_UART3_ONOVERFLOW();
	if (sr & USART_SR_TXE)
		HARDWARE_UART3_ONTXCHAR(USART3);
}

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

void RAMFUNC_NONILINE USART3_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	const uint_fast32_t isr = USART3->ISR;
	const uint_fast32_t cr1 = USART3->CR1;

	if (cr1 & USART_CR1_RXNEIE)
	{
		if (isr & USART_ISR_RXNE_RXFNE)
		{
			const uint_fast8_t c = UART3->RDR;
			HARDWARE_UART3_ONRXCHAR(c);
		}
		if (isr & USART_ISR_ORE)
		{
			USART3->ICR = USART_ICR_ORECF;
			HARDWARE_UART3_ONOVERFLOW();
		}
		if (isr & USART_ISR_FE)
			USART3->ICR = USART_ICR_FECF;
	}
	if (cr1 & USART_CR1_TXEIE)
	{
		if (isr & USART_ISR_TXE_TXFNF)
			HARDWARE_UART3_ONTXCHAR(USART3);
	}
}

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

void RAMFUNC_NONILINE USART3_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	const uint_fast32_t isr = USART3->ISR;

	if (isr & USART_ISR_RXNE)
		HARDWARE_UART3_ONRXCHAR(USART3->RDR);
	if (isr & USART_ISR_ORE)
	{
		USART3->ICR = USART_ICR_ORECF;
		HARDWARE_UART3_ONOVERFLOW();
	}
	if (isr & USART_ISR_FE)
		USART3->ICR = USART_ICR_FECF;
	if (isr & USART_ISR_TXE)
		HARDWARE_UART3_ONTXCHAR(USART3);
}

#elif CPUSTYLE_R7S721

// Приём символа он последовательного порта
static void SCIFRXI3_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	(void) uart->SCFSR;						// Перед сбросом бита RDF должно произойти его чтение в ненулевом состоянии
	uart->SCFSR = (uint16_t) ~ SCIF3_SCFSR_RDF;	// RDF=0 читать незачем (в примерах странное - сбрасывабтся и другие биты)
	if (uart->SCSCR & SCIF3_SCSCR_RIE)	// RIE Receive Interrupt Enable
	{
		uint_fast8_t n = (uart->SCFDR & SCIF3_SCFDR_R) >> SCIF3_SCFDR_R_SHIFT;
		while (n --)
			HARDWARE_UART3_ONRXCHAR(uart->SCFRDR & SCIF3_SCFRDR_D);
	}
}

// Передача символа в последовательный порт
static void SCIFTXI3_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	if (uart->SCSCR & SCIF3_SCSCR_TIE)	// TIE Transmit Interrupt Enable
	{
		HARDWARE_UART3_ONTXCHAR(uart);
	}
}

#elif CPUSTYLE_ALLWINNER

static RAMFUNC_NONILINE void UART3_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	const uint_fast32_t ier = uart->UART_DLH_IER;
	const uint_fast32_t usr = uart->UART_USR;

	if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
	{
		if (usr & (1u << 3))	// RX FIFO Not Empty
			HARDWARE_UART3_ONRXCHAR(uart->UART_RBR_THR_DLL);
	}
	if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
	{
		if (usr & (1u << 1))	// TX FIFO Not Full
			HARDWARE_UART3_ONTXCHAR(uart);
	}
}

#elif CPUSTYLE_ROCKCHIP

static RAMFUNC_NONILINE void UART3_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	const uint_fast32_t ier = uart->UART_DLH_IER;
	const uint_fast32_t usr = uart->UART_USR;

	if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
	{
		if (usr & (1u << 3))	// RX FIFO Not Empty
			HARDWARE_UART3_ONRXCHAR(uart->UART_RBR_THR_DLL);
	}
	if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
	{
		if (usr & (1u << 1))	// TX FIFO Not Full
			HARDWARE_UART3_ONTXCHAR(uart);
	}
}

#else
	#error Undefined CPUSTYLE_XXX
#endif	/* CPUSTYLE_ATMEGA_XXX4 */

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uart3_enabletx(uint_fast8_t state)
{
	hardware_uartx_enabletx(UARTBASENAME(thisPORT), state);
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uart3_enablerx(uint_fast8_t state)
{
	hardware_uartx_enablerx(UARTBASENAME(thisPORT), state);
}

/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uart3_tx(void * ctx, uint_fast8_t c)
{
	hardware_uartx_tx(UARTBASENAME(thisPORT), c);
}

/* дождаться, когда буде все передано */
void hardware_uart3_flush(void)
{
	hardware_uartx_flush(UARTBASENAME(thisPORT));
}

/* приём символа, если готов порт */
uint_fast8_t
hardware_uart3_getchar(char * cp)
{
	return hardware_uartx_getchar(UARTBASENAME(thisPORT), cp);
}

/* передача символа если готов порт */
uint_fast8_t
hardware_uart3_putchar(uint_fast8_t c)
{
	return hardware_uartx_putchar(UARTBASENAME(thisPORT), c);
}

void hardware_uart3_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd)
{
	int fifo = 1;
#if CPUSTYLE_STM32F1XX

	RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // Включение тактирования USART3.
	(void) RCC->APB1ENR;

	USART3->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables


	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     //включить тактирование альтернативных функций
	(void) RCC->APB2ENR;

	HARDWARE_UART3_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(USART3_IRQn, & USART3_IRQHandler);
	}

	USART3->CR1 |= USART_CR1_UE; // Включение USART3.

#elif CPUSTYLE_STM32H7XX

	RCC->APB1LENR |= RCC_APB1LENR_USART3EN; // Включение тактирования USART3.
	(void) RCC->APB1LENR;

	USART3->CR1 = 0;

#if WITHUART3HW_FIFO
	USART3->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUART3HW_FIFO */
	USART3->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUART3HW_FIFO */

	USART3->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART3_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(USART3_IRQn, & USART3_IRQHandler);
	}

	USART3->CR1 |= USART_CR1_UE; // Включение USART3.

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	RCC->APB1ENR |= RCC_APB1ENR_USART3EN_Msk; // Включение тактирования USART3.
	(void) RCC->APB1ENR;

	USART3->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART3_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(USART3_IRQn, & USART3_IRQHandler);
	}

	USART3->CR1 |= USART_CR1_UE; // Включение USART3.

#elif CPUSTYLE_R7S721

    /* ---- Supply clock to the SCIF(channel 3) ---- */
	CPG.STBCR4 &= ~ CPG_STBCR4_BIT_MSTP44;	// Module Stop 44 - SCIF3
	(void) CPG.STBCR4;			/* Dummy read */

	SCIF3.SCSCR = 0x0000;	/* SCIF transmitting and receiving operations stop, internal clock */

	SCIF3.SCSCR = (SCIF3.SCSCR & ~ 0x03) |
		0x00 |						// internal clock
		0;

	/* ---- Serial status register(SCFSR2) setting ---- */
	/* ER,BRK,DR bit clear */
	(void) SCIF3.SCFSR;						// Перед сбросом бита xxx должно произойти его чтение в ненулевом состоянии
	SCIF3.SCFSR = ~ 0x0091;	// 0xFF6E;

	/* ---- Line status register (SCLSR2) setting ---- */
	/* ORER bit clear */
	//SCIF3.SCLSR.BIT.ORER  = 0;
	SCIF3.SCLSR &= ~ 0x0001;

	SCIF3.SCSMR =
		0x00 |	/* 8-N-1 format */
		0;

	/* ---- FIFO control register (SCFCR2) setting ---- */
	/*  RTS output active trigger        :Initial value	*/
	/*  Receive FIFO data trigger        :1-data		*/
	/*  Transmit FIFO data trigger       :0-data		*/
	/*  Modem control enable             :Disabled		*/
	/*  Receive FIFO data register reset :Disabled		*/
	/*  Loop-back test                   :Disabled 		*/
	SCIF3.SCFCR = 0x0030;

	/* ---- Serial port register (SCSPTR2) setting ---- */
	/* Serial port  break output(SPB2IO)  1: Enabled */
	/* Serial port break data(SPB2DT)  1: High-level */
	//SCIF3.SCSPTR |= 0x0003;

	if (debug == 0)
	{
	   serial_set_handler(SCIFRXI3_IRQn, SCIFRXI3_IRQHandler);
	   serial_set_handler(SCIFTXI3_IRQn, SCIFTXI3_IRQHandler);
	}
	HARDWARE_UART3_INITIALIZE();	/* Присоединить периферию к выводам */

	SCIF3.SCSCR |= 0x0030;	// TE RE - SCIF3 transmitting and receiving operations are enabled */

#elif CPUSTYLE_STM32MP1

	RCC->MP_APB1ENSETR = RCC_MP_APB1ENSETR_USART3EN; // Включение тактирования USART3.
	(void) RCC->MP_APB1ENSETR;
	RCC->MP_APB1LPENSETR = RCC_MP_APB1LPENSETR_USART3LPEN; // Включение тактирования USART3.
	(void) RCC->MP_APB1LPENSETR;
	RCC->APB1RSTSETR = RCC_APB1RSTSETR_USART3RST; // Установить сброс USART3.
	(void) RCC->APB1RSTSETR;
	RCC->APB1RSTCLRR = RCC_APB1RSTCLRR_USART3RST; // Снять брос USART3.
	(void) RCC->APB1RSTCLRR;

	hardware_uartx_initialize(UARTBASENAME(thisPORT), stm32mp1_uart3_5_get_freq(), defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART3_INITIALIZE();	/* Присоединить периферию к выводам */
	if (debug == 0)
	{
		serial_set_handler(USART3_IRQn, USART3_IRQHandler);
	}

#elif CPUSTYLE_A64

	const unsigned ix = thisPORT;

	/* Open the clock gate for uart3 */
	CCU->BUS_CLK_GATING_REG3 |= (1u << (ix + 16));	// UART3_GATING

	/* De-assert uart3 reset */
	CCU-> BUS_SOFT_RST_REG4 |= (1u << (ix + 16));	//  UART3_RST

	hardware_uartx_initialize(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART3_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART3_IRQn, UART3_IRQHandler);
	}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_T507)

	const unsigned ix = thisPORT;

	/* Open the clock gate for uart3 */
	CCU->UART_BGR_REG |= (1u << (ix + 0));

	/* De-assert uart3 reset */
	CCU->UART_BGR_REG |= (1u << (ix + 16));

	hardware_uartx_initialize(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART3_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART3_IRQn, UART3_IRQHandler);
	}

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

void
hardware_uart3_set_speed(uint_fast32_t baudrate)
{
#if CPUSTYLE_STM32MP1

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), stm32mp1_uart2_4_get_freq(), baudrate);

#elif CPUSTYLE_STM32F

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), BOARD_USART3_FREQ, baudrate);

#elif CPUSTYLE_R7S721

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), P1CLOCK_FREQ, baudrate);

#elif CPUSTYLE_ALLWINNER

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, baudrate);

#elif CPUSTYLE_ROCKCHIP

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, baudrate);

#elif CPUSTYLE_VM14

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), elveesvm14_get_usart_freq(), baudrate);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* WITHUART3HW */
