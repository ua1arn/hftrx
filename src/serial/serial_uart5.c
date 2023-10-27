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
				HARDWARE_UART5_ONRXCHAR(UART5->RDR);
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

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

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

#else
	#error Undefined CPUSTYLE_XXX
#endif	/* CPUSTYLE_ATMEGA_XXX4 */


/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uart5_enabletx(uint_fast8_t state)
{
#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

	if (state)
		UART5->CR1 |= USART_CR1_TXEIE;
	else
		UART5->CR1 &= ~ USART_CR1_TXEIE;

#elif CPUSTYLE_ATXMEGAXXXA4

	if (state)
		USARTE5.CTRLA = (USARTE5.CTRLA & ~ USART_DREINTLVL_gm) | USART_DREINTLVL_LO_gc;
	else
		USARTE5.CTRLA = (USARTE5.CTRLA & ~ USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;

#elif CPUSTYLE_TMS320F2833X

	if (state)
		SCIBCTL2 |= (1U << 0);	// TX INT ENA
	else
		SCIBCTL2 &= ~ (1U << 0); // TX INT ENA

#elif CPUSTYLE_R7S721

	if (state)
		SCIF5.SCSCR |= (1U << 7);	// TIE Transmit Interrupt Enable
	else
		SCIF5.SCSCR &= ~ (1U << 7);	// TIE Transmit Interrupt Enable

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	if (state)
		UART5->UART_DLH_IER |= (1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt
	else
		UART5->UART_DLH_IER &= ~ (1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uart5_enablerx(uint_fast8_t state)
{
#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

	if (state)
		UART5->CR1 |= USART_CR1_RXNEIE;
	else
		UART5->CR1 &= ~ USART_CR1_RXNEIE;

#elif CPUSTYLE_TMS320F2833X

	if (state)
		SCIBCTL2 |= (1U << 1);	// RX/BK INT ENA
	else
		SCIBCTL2 &= ~ (1U << 1); // RX/BK INT ENA

#elif CPUSTYLE_R7S721

	if (state)
		SCIF5.SCSCR |= (1U << 6);	// RIE Receive Interrupt Enable
	else
		SCIF5.SCSCR &= ~ (1U << 6);	// RIE Receive Interrupt Enable

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	if (state)
		UART5->UART_DLH_IER |= (1u << 0);	// ERBFI Enable Received Data Available Interrupt
	else
		UART5->UART_DLH_IER &= ~ (1u << 0);	// ERBFI Enable Received Data Available Interrupt

#else
	#error Undefined CPUSTYLE_XXX
#endif
}


/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uart5_tx(void * ctx, uint_fast8_t c)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	UART5->DR = c;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32MP1

	UART5->TDR = c;

#elif CPUSTYLE_TMS320F2833X

	SCIBTXBUF = c;

#elif CPUSTYLE_R7S721

	(void) SCIF5.SCFSR;			// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
	SCIF5.SCFTDR = c;
	SCIF5.SCFSR = (uint16_t) ~ (1U << SCIF5_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	UART5->UART_RBR_THR_DLL = c;

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* дождаться, когда буде все передано */
void hardware_uart5_flush(void)
{
#if (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	while ((UART5->UART_USR & (1u << 2)) == 0)	// TFE Transmit FIFO Empty
		;

#else
	//#error Undefined CPUSTYLE_XXX
#endif


}

/* приём символа, если готов порт */
uint_fast8_t
hardware_uart5_getchar(char * cp)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((UART5->SR & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE)) == 0)
		return 0;
	* cp = UART5->DR;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	const uint_fast32_t isr = UART5->ISR;
	if (isr & USART_ISR_ORE)
		UART5->ICR = USART_ICR_ORECF;
	if (isr & USART_ISR_FE)
		UART5->ICR = USART_ICR_FECF;
	if ((isr & USART_ISR_RXNE_RXFNE) == 0)
		return 0;
	* cp = UART5->RDR;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	const uint_fast32_t isr = UART5->ISR;
	if (isr & USART_ISR_ORE)
		UART5->ICR = USART_ICR_ORECF;
	if (isr & USART_ISR_FE)
		UART5->ICR = USART_ICR_FECF;
	if ((isr & USART_ISR_RXNE) == 0)
		return 0;
	* cp = UART5->RDR;

#elif CPUSTYLE_TMS320F2833X

	if ((SCIBRXST & (1U << 6)) == 0)	// Wait for RXRDY bit
		return 0;
	* cp = SCIBRXBUF;

#elif CPUSTYLE_R7S721

	if ((SCIF5.SCFSR & (1U << 1)) == 0)	// RDF
		return 0;
	* cp = SCIF5.SCFRDR;
	SCIF5.SCFSR = (uint16_t) ~ (1U << 1);	// RDF=0 читать незачем (в примерах странное)

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	if ((UART5->UART_USR & (1u << 3)) == 0)	// RX FIFO Not Empty
		return 0;
	* cp = UART5->UART_RBR_THR_DLL;

#else
	#error Undefined CPUSTYLE_XXX
#endif

	return 1;
}

/* передача символа если готов порт */
uint_fast8_t
hardware_uart5_putchar(uint_fast8_t c)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((UART5->SR & USART_SR_TXE) == 0)
		return 0;
	UART5->DR = c;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	if ((UART5->ISR & USART_ISR_TXE_TXFNF) == 0)
		return 0;
	UART5->TDR = c;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	if ((UART5->ISR & USART_ISR_TXE) == 0)
		return 0;
	UART5->TDR = c;

#elif CPUSTYLE_TMS320F2833X

	if ((SCIBCTL2 & (1U << 7)) == 0)	// wait for TXRDY bit
		return 0;
	SCIBTXBUF = c;

#elif CPUSTYLE_R7S721

	if ((SCIF5.SCFSR & (1U << SCIF5_SCFSR_TDFE_SHIFT)) == 0)	// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
		return 0;
	SCIF5.SCFTDR = c;
	SCIF5.SCFSR = (uint16_t) ~ (1U << SCIF5_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	if ((UART5->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		return 0;
	UART5->UART_RBR_THR_DLL = c;

#else
	#error Undefined CPUSTYLE_XXX
#endif

	return 1;
}

void hardware_uart5_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd)
{
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

#if WITHUARTFIFO
	UART5->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUARTFIFO */
	UART5->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUARTFIFO */

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

#elif CPUSTYLE_TMS320F2833X
xxxx!;
	// Enable SCI-B clock
	PCLKCR0 |= (1U << 10);	// SCIBENCLK

	//SCIBCTL1 &= ~ (1U << 5);	// SW RESET on
	SCIBCTL1 |= (1U << 5);	// SW RESET off

	SCIBCCR =
			(7U << 0) |	// Data length = 8 bit
			(0U << 5) | // Parity enable
			(0U << 6) | // Evan/Odd parity
			(0U << 7);	// 0 - one stop bit, 1 - to stop bits


	tms320_hardware_piob_periph(
			(1u << (35 % 32)) |	// SCITXDA
			(1u << (36 % 32)),	// SCIRXDA
			1	// mux = 1
			);

	SCIBCTL1 |= (1U << 0) |	// RX enable
				(1U << 1);	// TX enable

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

#if WITHUARTFIFO
	UART5->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUARTFIFO */
	UART5->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUARTFIFO */

	UART5->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART5_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(UART5_IRQn, UART5_IRQHandler);
	}

	UART5->CR1 |= USART_CR1_UE; // Включение USART1.


#elif CPUSTYLE_A64

	const unsigned ix = 5;

	/* Open the clock gate for uart0 */
	CCU->BUS_CLK_GATING_REG3 |= (1u << (ix + 16));	// UART5_GATING

	/* De-assert uart0 reset */
	CCU-> BUS_SOFT_RST_REG4 |= (1u << (ix + 16));	//  UART5_RST

	/* Config uart5 to 115200-8-1-0 */
	uint32_t divisor = allwnrt113_get_uart_freq() / ((defbaudrate) * 16);

	UART5->UART_DLH_IER = 0;
	UART5->UART_IIR_FCR = 0xf7;
	UART5->UART_MCR = 0x00;

	UART5->UART_LCR |= (1 << 7);	// Divisor Latch Access Bit
	UART5->UART_RBR_THR_DLL = divisor & 0xff;
	UART5->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART5->UART_LCR &= ~ (1 << 7);	// Divisor Latch Access Bit
	//
	UART5->UART_LCR &= ~ 0x3f;
	UART5->UART_LCR |=
			((0x03 & (bits - 5)) << 0) | (0 << 2) | // DAT_LEN_8_BITS ONE_STOP_BIT
			(!! odd << 4) |	// bit5:bit4 0 - even, 1 - odd
			(!! parity << 3) |	// bit3 1: parity enable
			0;

	HARDWARE_UART5_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
	   serial_set_handler(UART5_IRQn, UART5_IRQHandler);
	}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_T507 || CPUSTYLE_H616)

	const unsigned ix = 5;

	/* Open the clock gate for uart5 */
	CCU->UART_BGR_REG |= (1u << (ix + 0));

	/* De-assert uart5 reset */
	CCU->UART_BGR_REG |= (1u << (ix + 16));

	/* Config uart0 to 115200-8-1-0 */
	uint32_t divisor = allwnrt113_get_uart_freq() / ((defbaudrate) * 16);

	UART5->UART_DLH_IER = 0;
	UART5->UART_IIR_FCR = 0xf7;
	UART5->UART_MCR = 0x00;

	UART5->UART_LCR |= (1 << 7);	// Divisor Latch Access Bit
	UART5->UART_RBR_THR_DLL = divisor & 0xff;
	UART5->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART5->UART_LCR &= ~ (1 << 7);	// Divisor Latch Access Bit
	//
	UART5->UART_LCR &= ~ 0x3f;
	UART5->UART_LCR |=
			((0x03 & (bits - 5)) << 0) | (0 << 2) | // DAT_LEN_8_BITS ONE_STOP_BIT
			(!! odd << 4) |	// bit5:bit4 0 - even, 1 - odd
			(!! parity << 3) |	// bit3 1: parity enable
			0;

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

#elif CPUSTYLE_TMS320F2833X

	const unsigned long lspclk = CPU_FREQ / 4;
	const unsigned long brr = (lspclk / 8) / baudrate;	// @ CPU_FREQ = 100 MHz, 9600 can not be programmed

	SCIBHBAUD = (brr - 1) >> 8;		// write 8 bits, not 16
	SCIBLBAUD = (brr - 1) >> 0;

#elif CPUSTYLE_R7S721

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(baudrate), R7S721_SCIF_SCBRR_WIDTH, R7S721_SCIF_SCBRR_TAPS, & value, 1);

	SCIF5.SCSMR = (SCIF5.SCSMR & ~ 0x03) |
		scemr_scsmr [prei].scsmr |	// prescaler: 0: /1, 1: /4, 2: /16, 3: /64
		0;
	SCIF5.SCEMR = (SCIF5.SCEMR & ~ (0x80 | 0x01)) |
		0 * 0x80 |						// BGDM
		scemr_scsmr [prei].scemr |	// ABCS = 8/16 clocks per bit
		0;
	SCIF5.SCBRR = value;	/* Bit rate register */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	unsigned divisor = calcdivround2(BOARD_USART_FREQ, baudrate * 16);

	UART5->UART_LCR |= (1 << 7);
	UART5->UART_RBR_THR_DLL = divisor & 0xff;
	UART5->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART5->UART_LCR &= ~ (1 << 7);

#else
	#warning Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART5HW */
