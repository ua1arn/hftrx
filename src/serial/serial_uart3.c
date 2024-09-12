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

#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	void RAMFUNC_NONILINE USART3_IRQHandler(void)
	{
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
		(void) SCIF3.SCFSR;						// Перед сбросом бита RDF должно произойти его чтение в ненулевом состоянии
		SCIF3.SCFSR = (uint16_t) ~ SCIF3_SCFSR_RDF;	// RDF=0 читать незачем (в примерах странное - сбрасывабтся и другие биты)
		uint_fast8_t n = (SCIF3.SCFDR & SCIF3_SCFDR_R) >> SCIF3_SCFDR_R_SHIFT;
		while (n --)
			HARDWARE_UART3_ONRXCHAR(SCIF3.SCFRDR & SCIF3_SCFRDR_D);
	}

	// Передача символа в последовательный порт
	static void SCIFTXI3_IRQHandler(void)
	{
		HARDWARE_UART3_ONTXCHAR(& SCIF3);
	}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	static RAMFUNC_NONILINE void UART3_IRQHandler(void)
	{
		const uint_fast32_t ier = UART3->UART_DLH_IER;
		const uint_fast32_t usr = UART3->UART_USR;
		if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
		{
			if (usr & (1u << 3))	// RX FIFO Not Empty
				HARDWARE_UART3_ONRXCHAR(UART3->UART_RBR_THR_DLL);
		}
		if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
		{
			if (usr & (1u << 1))	// TX FIFO Not Full
				HARDWARE_UART3_ONTXCHAR(UART3);
		}
	}
#else
	#error Undefined CPUSTYLE_XXX
#endif	/* CPUSTYLE_ATMEGA_XXX4 */


/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uart3_enabletx(uint_fast8_t state)
{
#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

	if (state)
		USART3->CR1 |= USART_CR1_TXEIE;
	else
		USART3->CR1 &= ~ USART_CR1_TXEIE;

#elif CPUSTYLE_ATMEGA_XXX4

	/* Used USART 1 */
	if (state)
	{
		UCSR3B |= (1U << TXCIE1);
		HARDWARE_UART3_ONTXCHAR(NULL);	// initiate 1-st character sending
	}
	else
	{
		UCSR3B &= ~ (1U << TXCIE1);
	}

#elif CPUSTYLE_R7S721

	if (state)
		SCIF3.SCSCR |= (1U << 7);	// TIE Transmit Interrupt Enable
	else
		SCIF3.SCSCR &= ~ (1U << 7);	// TIE Transmit Interrupt Enable

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	if (state)
		UART3->UART_DLH_IER |= (1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt
	else
		UART3->UART_DLH_IER &= ~ (1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uart3_enablerx(uint_fast8_t state)
{
#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

	if (state)
		USART3->CR1 |= USART_CR1_RXNEIE;
	else
		USART3->CR1 &= ~ USART_CR1_RXNEIE;

#elif CPUSTYLE_TMS320F2833X

	if (state)
		SCIBCTL2 |= (1U << 1);	// RX/BK INT ENA
	else
		SCIBCTL2 &= ~ (1U << 1); // RX/BK INT ENA

#elif CPUSTYLE_R7S721

	if (state)
		SCIF3.SCSCR |= (1U << 6);	// RIE Receive Interrupt Enable
	else
		SCIF3.SCSCR &= ~ (1U << 6);	// RIE Receive Interrupt Enable

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	if (state)
		UART3->UART_DLH_IER |= (1u << 0);	// ERBFI Enable Received Data Available Interrupt
	else
		UART3->UART_DLH_IER &= ~ (1u << 0);	// ERBFI Enable Received Data Available Interrupt

#else
	#error Undefined CPUSTYLE_XXX
#endif
}


/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uart3_tx(void * ctx, uint_fast8_t c)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	USART3->DR = c;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32MP1

	USART3->TDR = c;

#elif CPUSTYLE_TMS320F2833X

	SCIBTXBUF = c;

#elif CPUSTYLE_R7S721

	(void) SCIF3.SCFSR;			// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
	SCIF3.SCFTDR = c;
	SCIF3.SCFSR = (uint16_t) ~ (1U << SCIF3_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	UART3->UART_RBR_THR_DLL = c;

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* дождаться, когда буде все передано */
void hardware_uart3_flush(void)
{
#if (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	while ((UART3->UART_USR & (1u << 2)) == 0)	// TFE Transmit FIFO Empty
		;

#else
	//#error Undefined CPUSTYLE_XXX
#endif


}

/* приём символа, если готов порт */
uint_fast8_t
hardware_uart3_getchar(char * cp)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((USART3->SR & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE)) == 0)
		return 0;
	* cp = USART3->DR;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	const uint_fast32_t isr = USART3->ISR;
	if (isr & USART_ISR_ORE)
		USART3->ICR = USART_ICR_ORECF;
	if (isr & USART_ISR_FE)
		USART3->ICR = USART_ICR_FECF;
	if ((isr & USART_ISR_RXNE_RXFNE) == 0)
		return 0;
	* cp = USART3->RDR;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	const uint_fast32_t isr = USART3->ISR;
	if (isr & USART_ISR_ORE)
		USART3->ICR = USART_ICR_ORECF;
	if (isr & USART_ISR_FE)
		USART3->ICR = USART_ICR_FECF;
	if ((isr & USART_ISR_RXNE) == 0)
		return 0;
	* cp = USART3->RDR;

#elif CPUSTYLE_TMS320F2833X

	if ((SCIBRXST & (1U << 6)) == 0)	// Wait for RXRDY bit
		return 0;
	* cp = SCIBRXBUF;

#elif CPUSTYLE_R7S721

	if ((SCIF3.SCFSR & (1U << 1)) == 0)	// RDF
		return 0;
	* cp = SCIF3.SCFRDR;
	SCIF3.SCFSR = (uint16_t) ~ (1U << 1);	// RDF=0 читать незачем (в примерах странное)

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	if ((UART3->UART_USR & (1u << 3)) == 0)	// RX FIFO Not Empty
		return 0;
	* cp = UART3->UART_RBR_THR_DLL;

#else
	#error Undefined CPUSTYLE_XXX
#endif

	return 1;
}

/* передача символа если готов порт */
uint_fast8_t
hardware_uart3_putchar(uint_fast8_t c)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((USART3->SR & USART_SR_TXE) == 0)
		return 0;
	USART3->DR = c;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	if ((USART3->ISR & USART_ISR_TXE_TXFNF) == 0)
		return 0;
	USART3->TDR = c;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	if ((USART3->ISR & USART_ISR_TXE) == 0)
		return 0;
	USART3->TDR = c;

#elif CPUSTYLE_TMS320F2833X

	if ((SCIBCTL2 & (1U << 7)) == 0)	// wait for TXRDY bit
		return 0;
	SCIBTXBUF = c;

#elif CPUSTYLE_R7S721

	if ((SCIF3.SCFSR & (1U << SCIF3_SCFSR_TDFE_SHIFT)) == 0)	// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
		return 0;
	SCIF3.SCFTDR = c;
	SCIF3.SCFSR = (uint16_t) ~ (1U << SCIF3_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	if ((UART3->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		return 0;
	UART3->UART_RBR_THR_DLL = c;

#else
	#error Undefined CPUSTYLE_XXX
#endif

	return 1;
}

void hardware_uart3_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd)
{
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
		serial_set_handler(USART3_IRQn, USART3_IRQHandler);
	}

	USART3->CR1 |= USART_CR1_UE; // Включение USART1.

#elif CPUSTYLE_A64

	const unsigned ix = 3;

	/* Open the clock gate for uart3 */
	CCU->BUS_CLK_GATING_REG3 |= (1u << (ix + 16));	// UART3_GATING

	/* De-assert uart3 reset */
	CCU-> BUS_SOFT_RST_REG4 |= (1u << (ix + 16));	//  UART3_RST

	/* Config uart0 to 115200-8-1-0 */
	uint32_t divisor = allwnr_t113_get_uart_freq() / ((defbaudrate) * 16);

	UART3->UART_DLH_IER = 0;
	UART3->UART_IIR_FCR = 0xf7;
	UART3->UART_MCR = 0x00;

	UART3->UART_LCR |= (1 << 7);	// Divisor Latch Access Bit
	UART3->UART_RBR_THR_DLL = divisor & 0xff;
	UART3->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART3->UART_LCR &= ~ (1 << 7);	// Divisor Latch Access Bit
	//
	UART3->UART_LCR &= ~ 0x3f;
	UART3->UART_LCR |=
			((0x03 & (bits - 5)) << 0) | (0 << 2) | // DAT_LEN_8_BITS ONE_STOP_BIT
			(!! odd << 4) |	// bit5:bit4 0 - even, 1 - odd
			(!! parity << 3) |	// bit3 1: parity enable
			0;

	HARDWARE_UART3_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART3_IRQn, UART3_IRQHandler);
	}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_T507 || CPUSTYLE_H616)

	const unsigned ix = 3;

	/* Open the clock gate for uart3 */
	CCU->UART_BGR_REG |= (1u << (ix + 0));

	/* De-assert uart3 reset */
	CCU->UART_BGR_REG |= (1u << (ix + 16));

	/* Config uart0 to 115200-8-1-0 */
	uint32_t divisor = allwnr_t113_get_uart_freq() / ((defbaudrate) * 16);

	UART3->UART_DLH_IER = 0;
	UART3->UART_IIR_FCR = 0xf7;
	UART3->UART_MCR = 0x00;

	UART3->UART_LCR |= (1 << 7);	// Divisor Latch Access Bit
	UART3->UART_RBR_THR_DLL = divisor & 0xff;
	UART3->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART3->UART_LCR &= ~ (1 << 7);	// Divisor Latch Access Bit
	//
	UART3->UART_LCR &= ~ 0x3f;
	UART3->UART_LCR |=
			((0x03 & (bits - 5)) << 0) | (0 << 2) | // DAT_LEN_8_BITS ONE_STOP_BIT
			(!! odd << 4) |	// bit5:bit4 0 - even, 1 - odd
			(!! parity << 3) |	// bit3 1: parity enable
			0;

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
#if CPUSTYLE_ATMEGA_XXX4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR3A |= (1U << U2X1);
	else
		UCSR3A &= ~ (1U << U2X1);

	UBRR3 = value;	/* Значение получено уже уменьшенное на 1 */


#elif CPUSTYLE_ATMEGA128

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR3A |= (1U << U2X1);
	else
		UCSR3A &= ~ (1U << U2X1);

	UBRR3H = (value >> 8) & 0xff;	/* Значение получено уже уменьшенное на 1 */
	UBRR3L = value & 0xff;

#elif CPUSTYLE_ATXMEGAXXXA4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATXMEGA_UBR_WIDTH, ATXMEGA_UBR_TAPS, & value, 1);
	if (prei == 0)
		USARTE3.CTRLB |= USART_CLK2X_bm;
	else
		USARTE3.CTRLB &= ~ USART_CLK2X_bm;
	// todo: проверить требование к порядку обращения к портам
	USARTE3.BAUDCTRLA = (value & 0xff);	/* Значение получено уже уменьшенное на 1 */
	USARTE3.BAUDCTRLB = (ATXMEGA_UBR_BSEL << 4) | ((value >> 8) & 0x0f);

#elif CPUSTYLE_STM32MP1

	// uart3
	UART3->BRR = calcdivround2(stm32mp1_uart2_4_get_freq(), baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_STM32F

	// uart3 on apb1

	USART3->BRR = calcdivround2(BOARD_USART3_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_TMS320F2833X

	const unsigned long lspclk = CPU_FREQ / 4;
	const unsigned long brr = (lspclk / 8) / baudrate;	// @ CPU_FREQ = 100 MHz, 9600 can not be programmed

	SCIBHBAUD = (brr - 1) >> 8;		// write 8 bits, not 16
	SCIBLBAUD = (brr - 1) >> 0;

#elif CPUSTYLE_R7S721

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(baudrate), R7S721_SCIF_SCBRR_WIDTH, R7S721_SCIF_SCBRR_TAPS, & value, 1);

	SCIF3.SCSMR = (SCIF3.SCSMR & ~ 0x03) |
		scemr_scsmr [prei].scsmr |	// prescaler: 0: /1, 1: /4, 2: /16, 3: /64
		0;
	SCIF3.SCEMR = (SCIF4.SCEMR & ~ (0x80 | 0x01)) |
		0 * 0x80 |						// BGDM
		scemr_scsmr [prei].scemr |	// ABCS = 8/16 clocks per bit
		0;
	SCIF3.SCBRR = value;	/* Bit rate register */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	unsigned divisor = calcdivround2(BOARD_USART_FREQ, baudrate * 16);

	UART3->UART_LCR |= (1 << 7);
	UART3->UART_RBR_THR_DLL = divisor & 0xff;
	UART3->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART3->UART_LCR &= ~ (1 << 7);

#else
	#warning Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART3HW */
