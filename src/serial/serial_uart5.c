/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if WITHUART5HW

#include "serial.h"
#include "formats.h"	// for debug prints
#include "board.h"
#include "gpio.h"
#include <string.h>
#include <math.h>
#include "clocks.h"

#define thisPORT 5

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

void RAMFUNC_NONILINE USART5_Handler(void)
{
	const uint_fast32_t csr = USART5->US_CSR;

	if (csr & US_CSR_RXRDY)
		HARDWARE_UART5_ONRXCHAR(USART5->US_RHR);
	if (csr & US_CSR_TXRDY)
		HARDWARE_UART5_ONTXCHAR(USART5);
}

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

void RAMFUNC_NONILINE UART5_IRQHandler(void)
{
	const uint_fast32_t sr = UART5->SR;

	if (sr & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE))
		HARDWARE_UART5_ONRXCHAR(UART5->DR);
	if (sr & (USART_SR_ORE | USART_SR_FE | USART_SR_NE))
		HARDWARE_UART5_ONOVERFLOW();
	if (sr & USART_SR_TXE)
		HARDWARE_UART5_ONTXCHAR(UART5);
}

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

void RAMFUNC_NONILINE UART5_IRQHandler(void)
{
	const uint_fast32_t isr = UART5->ISR;
	const uint_fast32_t cr1 = UART5->CR1;

	if (cr1 & USART_CR1_RXNEIE)
	{
		if (isr & USART_ISR_RXNE_RXFNE)
		{
			const uint_fast8_t c = UART5->RDR;
			HARDWARE_UART5_ONRXCHAR(c);
		}
		if (isr & USART_ISR_ORE)
		{
			UART5->ICR = USART_ICR_ORECF;
			HARDWARE_UART5_ONOVERFLOW();
		}
		if (isr & USART_ISR_FE)
			UART5->ICR = USART_ICR_FECF;
	}
	if (cr1 & USART_CR1_TXEIE)
	{
		if (isr & USART_ISR_TXE_TXFNF)
			HARDWARE_UART5_ONTXCHAR(UART5);
	}
}

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

void RAMFUNC_NONILINE UART5_IRQHandler(void)
{
	const uint_fast32_t isr = UART5->ISR;

	if (isr & USART_ISR_RXNE)
		HARDWARE_UART5_ONRXCHAR(UART5->RDR);
	if (isr & USART_ISR_ORE)
	{
		UART5->ICR = USART_ICR_ORECF;
		HARDWARE_UART5_ONOVERFLOW();
	}
	if (isr & USART_ISR_FE)
		UART5->ICR = USART_ICR_FECF;
	if (isr & USART_ISR_TXE)
		HARDWARE_UART5_ONTXCHAR(UART5);
}

#elif CPUSTYLE_R7S721

// Приём символа он последовательного порта
static void SCIFRXI5_IRQHandler(void)
{
	(void) SCIF5.SCFSR;						// Перед сбросом бита RDF должно произойти его чтение в ненулевом состоянии
	SCIF5.SCFSR = (uint16_t) ~ SCIF5_SCFSR_RDF;	// RDF=0 читать незачем (в примерах странное - сбрасывабтся и другие биты)
	uint_fast8_t n = (SCIF5.SCFDR & SCIF5_SCFDR_R) >> SCIF5_SCFDR_R_SHIFT;
	while (n --)
		HARDWARE_UART5_ONRXCHAR(SCIF5.SCFRDR & SCIF5_SCFRDR_D);
}

// Передача символа в последовательный порт
static void SCIFTXI5_IRQHandler(void)
{
	HARDWARE_UART5_ONTXCHAR(& SCIF5);
}

#elif CPUSTYLE_ALLWINNER

static RAMFUNC_NONILINE void UART5_IRQHandler(void)
{
	const uint_fast32_t ier = UART5->UART_DLH_IER;
	const uint_fast32_t usr = UART5->UART_USR;

	if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
	{
		if (usr & (1u << 3))	// RX FIFO Not Empty
			HARDWARE_UART5_ONRXCHAR(UART5->UART_RBR_THR_DLL);
	}
	if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
	{
		if (usr & (1u << 1))	// TX FIFO Not Full
			HARDWARE_UART5_ONTXCHAR(UART5);
	}
}

#elif CPUSTYLE_ROCKCHIP
	#warning Unimplemented CPUSTYLE_ROCKCHIP

#else
	#error Undefined CPUSTYLE_XXX
#endif	/* CPUSTYLE_ATMEGA_XXX4 */


/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uart5_enabletx(uint_fast8_t state)
{
	hardware_uartx_enabletx(UARTBASENAME(thisPORT), state);
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uart5_enablerx(uint_fast8_t state)
{
	hardware_uartx_enablerx(UARTBASENAME(thisPORT), state);
}

/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uart5_tx(void * ctx, uint_fast8_t c)
{
	hardware_uartx_tx(UARTBASENAME(thisPORT), c);
}

/* дождаться, когда буде все передано */
void hardware_uart5_flush(void)
{
	hardware_uartx_flush(UARTBASENAME(thisPORT));
}

/* приём символа, если готов порт */
uint_fast8_t
hardware_uart5_getchar(char * cp)
{
	return hardware_uartx_getchar(UARTBASENAME(thisPORT), cp);
}

/* передача символа если готов порт */
uint_fast8_t
hardware_uart5_putchar(uint_fast8_t c)
{
	return hardware_uartx_putchar(UARTBASENAME(thisPORT), c);
}

void hardware_uart5_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd)
{
	int fifo = 1;
#if CPUSTYLE_STM32F1XX

	RCC->APB1ENR |= RCC_APB1ENR_UART5EN; // Включение тактирования UART5.
	(void) RCC->APB1ENR;

	UART5->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables


	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     //включить тактирование альтернативных функций
	(void) RCC->APB2ENR;

	HARDWARE_UART5_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(UART5_IRQn, & UART5_IRQHandler);
	}

	UART5->CR1 |= USART_CR1_UE; // Включение UART5.

#elif CPUSTYLE_STM32H7XX

	RCC->APB1LENR |= RCC_APB1LENR_UART5EN; // Включение тактирования UART5.
	(void) RCC->APB1LENR;

	UART5->CR1 = 0;

#if WITHUART5HW_FIFO
	UART5->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUART5HW_FIFO */
	UART5->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUART5HW_FIFO */

	UART5->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART5_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(UART5_IRQn, & UART5_IRQHandler);
	}

	UART5->CR1 |= USART_CR1_UE; // Включение UART5.

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	RCC->APB1ENR |= RCC_APB1ENR_UART5EN; // Включение тактирования UART5.
	(void) RCC->APB1ENR;

	UART5->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART5_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(UART5_IRQn, & UART5_IRQHandler);
	}

	UART5->CR1 |= USART_CR1_UE; // Включение UART5.

#elif CPUSTYLE_R7S721

    /* ---- Supply clock to the SCIF(channel 5) ---- */
	CPG.STBCR4 &= ~ CPG_STBCR4_BIT_MSTP42;	// Module Stop 42 - SCIF5
	(void) CPG.STBCR4;			/* Dummy read */

	SCIF5.SCSCR = 0x0000;	/* SCIF transmitting and receiving operations stop, internal clock */

	SCIF5.SCSCR = (SCIF5.SCSCR & ~ 0x03) |
		0x00 |						// internal clock
		0;

	/* ---- Serial status register(SCFSR2) setting ---- */
	/* ER,BRK,DR bit clear */
	(void) SCIF5.SCFSR;						// Перед сбросом бита xxx должно произойти его чтение в ненулевом состоянии
	SCIF5.SCFSR = ~ 0x0091;	// 0xFF6E;

	/* ---- Line status register (SCLSR2) setting ---- */
	/* ORER bit clear */
	//SCIF5.SCLSR.BIT.ORER  = 0;
	SCIF5.SCLSR &= ~ 0x0001;

	SCIF5.SCSMR =
		0x00 |	/* 8-N-1 format */
		0;

	/* ---- FIFO control register (SCFCR2) setting ---- */
	/*  RTS output active trigger        :Initial value	*/
	/*  Receive FIFO data trigger        :1-data		*/
	/*  Transmit FIFO data trigger       :0-data		*/
	/*  Modem control enable             :Disabled		*/
	/*  Receive FIFO data register reset :Disabled		*/
	/*  Loop-back test                   :Disabled 		*/
	SCIF5.SCFCR = 0x0030;

	/* ---- Serial port register (SCSPTR2) setting ---- */
	/* Serial port  break output(SPB2IO)  1: Enabled */
	/* Serial port break data(SPB2DT)  1: High-level */
	//SCIF5.SCSPTR |= 0x0003;

	if (debug == 0)
	{
	   serial_set_handler(SCIFRXI5_IRQn, SCIFRXI5_IRQHandler);
	   serial_set_handler(SCIFTXI5_IRQn, SCIFTXI5_IRQHandler);
	}
	HARDWARE_UART5_INITIALIZE();	/* Присоединить периферию к выводам */

	SCIF5.SCSCR |= 0x0030;	// TE RE - SCIF5 transmitting and receiving operations are enabled */

#elif CPUSTYLE_STM32MP1

	RCC->MP_APB1ENSETR = RCC_MP_APB1ENSETR_UART5EN; // Включение тактирования UART5.
	(void) RCC->MP_APB1ENSETR;
	RCC->MP_APB1LPENSETR = RCC_MP_APB1LPENSETR_UART5LPEN; // Включение тактирования UART5.
	(void) RCC->MP_APB1LPENSETR;
	RCC->APB1RSTSETR = RCC_APB1RSTSETR_UART5RST; // Установить сброс UART5.
	(void) RCC->APB1RSTSETR;
	RCC->APB1RSTCLRR = RCC_APB1RSTCLRR_UART5RST; // Снять брос UART5.
	(void) RCC->APB1RSTCLRR;

	UART5->CR1 = 0;

#if WITHUART5HW_FIFO
	UART5->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUART5HW_FIFO */
	UART5->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUART5HW_FIFO */

	UART5->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART5_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(UART5_IRQn, UART5_IRQHandler);
	}

	UART5->CR1 |= USART_CR1_UE; // Включение USART1.


#elif CPUSTYLE_A64

	const unsigned ix = thisPORT;

	/* Open the clock gate for uart0 */
	CCU->BUS_CLK_GATING_REG3 |= (1u << (ix + 16));	// UART5_GATING

	/* De-assert uart0 reset */
	CCU-> BUS_SOFT_RST_REG4 |= (1u << (ix + 16));	//  UART5_RST

	hardware_uartx_initialize(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART5_INITIALIZE();	/* Присоединить периферию к выводам */
	if (debug == 0)
	{
	   serial_set_handler(UART5_IRQn, UART5_IRQHandler);
	}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_T507 || CPUSTYLE_H616)

	const unsigned ix = thisPORT;

	/* Open the clock gate for uart5 */
	CCU->UART_BGR_REG |= (1u << (ix + 0));

	/* De-assert uart5 reset */
	CCU->UART_BGR_REG |= (1u << (ix + 16));

	hardware_uartx_initialize(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART5_INITIALIZE();	/* Присоединить периферию к выводам */
	if (debug == 0)
	{
	   serial_set_handler(UART5_IRQn, UART5_IRQHandler);
	}

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

void
hardware_uart5_set_speed(uint_fast32_t baudrate)
{
#if CPUSTYLE_STM32MP1

	// uart5
	UART5->BRR = calcdivround2(BOARD_UART5_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_STM32F

	// uart5 on apb1
	USART5->BRR = calcdivround2(BOARD_UART5_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

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

#endif /* WITHUART5HW */
