/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if WITHUART4HW

#include "serial.h"
#include "formats.h"	// for debug prints
#include "board.h"
#include "gpio.h"
#include <string.h>
#include <math.h>

#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	void RAMFUNC_NONILINE UART4_IRQHandler(void)
	{
		const uint_fast32_t sr = UART4->SR;

		if (sr & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE))
			HARDWARE_UART4_ONRXCHAR(UART4->DR);
		if (sr & (USART_SR_ORE | USART_SR_FE | USART_SR_NE))
			HARDWARE_UART4_ONOVERFLOW();
		if (sr & USART_SR_TXE)
			HARDWARE_UART4_ONTXCHAR(UART4);
	}

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	void RAMFUNC_NONILINE UART4_IRQHandler(void)
	{
		const uint_fast32_t isr = UART4->ISR;
		const uint_fast32_t cr1 = UART4->CR1;

		if (cr1 & USART_CR1_RXNEIE)
		{
			if (isr & USART_ISR_RXNE_RXFNE)
				HARDWARE_UART4_ONRXCHAR(UART4->RDR);
			if (isr & USART_ISR_ORE)
			{
				UART4->ICR = USART_ICR_ORECF;
				HARDWARE_UART4_ONOVERFLOW();
			}
			if (isr & USART_ISR_FE)
				UART4->ICR = USART_ICR_FECF;
		}
		if (cr1 & USART_CR1_TXEIE)
		{
			if (isr & USART_ISR_TXE_TXFNF)
				HARDWARE_UART4_ONTXCHAR(UART4);
		}
	}

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	void RAMFUNC_NONILINE UART4_IRQHandler(void)
	{
		const uint_fast32_t isr = UART4->ISR;

		if (isr & USART_ISR_RXNE)
			HARDWARE_UART4_ONRXCHAR(UART4->RDR);
		if (isr & USART_ISR_ORE)
		{
			UART4->ICR = USART_ICR_ORECF;
			HARDWARE_UART4_ONOVERFLOW();
		}
		if (isr & USART_ISR_FE)
			UART4->ICR = USART_ICR_FECF;
		if (isr & USART_ISR_TXE)
			HARDWARE_UART4_ONTXCHAR(UART4);
	}

#elif CPUSTYLE_ATMEGA_XXX4

	ISR(USART4_RX_vect)
	{
		HARDWARE_UART4_ONRXCHAR(UDR0);
	}

	ISR(USART4_TX_vect)
	{
		HARDWARE_UART4_ONTXCHAR(NULL);
	}

#elif CPUSTYLE_ATMEGA128

	ISR(USART4_RX_vect)
	{
		HARDWARE_UART4_ONRXCHAR(UDR1);
	}

	ISR(USART4_TX_vect)
	{
		HARDWARE_UART4_ONTXCHAR(NULL);
	}

#elif CPUSTYLE_ATXMEGAXXXA4

	ISR(USARTE4_RXC_vect)
	{
		HARDWARE_UART4_ONRXCHAR(USARTE1.DATA);
	}

	ISR(USARTE4_DRE_vect)
	{
		HARDWARE_UART4_ONTXCHAR(& USARTE1);
	}

#elif CPUSTYLE_R7S721

	// Приём символа он последовательного порта
	static void SCIFRXI4_IRQHandler(void)
	{
		(void) SCIF4.SCFSR;						// Перед сбросом бита RDF должно произойти его чтение в ненулевом состоянии
		SCIF4.SCFSR = (uint16_t) ~ SCIF4_SCFSR_RDF;	// RDF=0 читать незачем (в примерах странное - сбрасывабтся и другие биты)
		uint_fast8_t n = (SCIF4.SCFDR & SCIF4_SCFDR_R) >> SCIF4_SCFDR_R_SHIFT;
		while (n --)
			HARDWARE_UART4_ONRXCHAR(SCIF4.SCFRDR & SCIF4_SCFRDR_D);
	}

	// Передача символа в последовательный порт
	static void SCIFTXI4_IRQHandler(void)
	{
		HARDWARE_UART4_ONTXCHAR(& SCIF4);
	}


#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507)

	static RAMFUNC_NONILINE void UART4_IRQHandler(void)
	{
		const uint_fast32_t ier = UART4->UART_DLH_IER;
		const uint_fast32_t usr = UART4->UART_USR;
		if ((UART4->UART_USR & (1u << 3)) == 0)	// RX FIFO Not Empty

		if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
		{
			if (usr & (1u << 3))	// RX FIFO Not Empty
				HARDWARE_UART4_ONRXCHAR(UART4->UART_RBR_THR_DLL);
		}
		if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
		{
			if (usr & (1u << 1))	// TX FIFO Not Full
				HARDWARE_UART4_ONTXCHAR(UART4);
		}
	}

#else
	#error Undefined CPUSTYLE_XXX
#endif	/* CPUSTYLE_ATMEGA_XXX4 */


/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uart4_enabletx(uint_fast8_t state)
{
#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

	if (state)
		UART4->CR1 |= USART_CR1_TXEIE;
	else
		UART4->CR1 &= ~ USART_CR1_TXEIE;

#elif CPUSTYLE_ATMEGA_XXX4

	/* Used USART 1 */
	if (state)
	{
		UCSR4B |= (1U << TXCIE1);
		HARDWARE_UART4_ONTXCHAR(NULL);	// initiate 1-st character sending
	}
	else
	{
		UCSR4B &= ~ (1U << TXCIE1);
	}

#elif CPUSTYLE_ATMEGA128

	/* Used USART 1 */
	if (state)
	{
		UCSR4B |= (1U << TXCIE1);
		HARDWARE_UART4_ONTXCHAR(NULL);	// initiate 1-st character sending
	}
	else
	{
		UCSR4B &= ~ (1U << TXCIE1);
	}

#elif CPUSTYLE_ATXMEGAXXXA4

	if (state)
		USARTE4.CTRLA = (USARTE4.CTRLA & ~ USART_DREINTLVL_gm) | USART_DREINTLVL_LO_gc;
	else
		USARTE4.CTRLA = (USARTE4.CTRLA & ~ USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;

#elif CPUSTYLE_TMS320F2833X

	if (state)
		SCIBCTL2 |= (1U << 0);	// TX INT ENA
	else
		SCIBCTL2 &= ~ (1U << 0); // TX INT ENA

#elif CPUSTYLE_R7S721

	if (state)
		SCIF4.SCSCR |= (1U << 7);	// TIE Transmit Interrupt Enable
	else
		SCIF4.SCSCR &= ~ (1U << 7);	// TIE Transmit Interrupt Enable

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507)

	if (state)
		 UART4->UART_DLH_IER |= (1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt
	else
		 UART4->UART_DLH_IER &= ~ (1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uart4_enablerx(uint_fast8_t state)
{
#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

	if (state)
		UART4->CR1 |= USART_CR1_RXNEIE;
	else
		UART4->CR1 &= ~ USART_CR1_RXNEIE;

#elif CPUSTYLE_TMS320F2833X

	if (state)
		SCIBCTL2 |= (1U << 1);	// RX/BK INT ENA
	else
		SCIBCTL2 &= ~ (1U << 1); // RX/BK INT ENA

#elif CPUSTYLE_R7S721

	if (state)
		SCIF4.SCSCR |= (1U << 6);	// RIE Receive Interrupt Enable
	else
		SCIF4.SCSCR &= ~ (1U << 6);	// RIE Receive Interrupt Enable

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507)

	if (state)
		 UART4->UART_DLH_IER |= (1u << 0);	// ERBFI Enable Received Data Available Interrupt
	else
		 UART4->UART_DLH_IER &= ~ (1u << 0);	// ERBFI Enable Received Data Available Interrupt

#else
	#error Undefined CPUSTYLE_XXX
#endif
}


/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uart4_tx(void * ctx, uint_fast8_t c)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	UART4->DR = c;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32MP1

	UART4->TDR = c;

#elif CPUSTYLE_TMS320F2833X

	SCIBTXBUF = c;

#elif CPUSTYLE_R7S721

	(void) SCIF4.SCFSR;			// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
	SCIF4.SCFTDR = c;
	SCIF4.SCFSR = (uint16_t) ~ (1U << SCIF4_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507)

	UART4->UART_RBR_THR_DLL = c;

#else
	#error Undefined CPUSTYLE_XXX
#endif
}



/* приём символа, если готов порт */
uint_fast8_t
hardware_uart4_getchar(char * cp)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((UART4->SR & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE)) == 0)
		return 0;
	* cp = UART4->DR;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	const uint_fast32_t isr = UART4->ISR;
	if (isr & USART_ISR_ORE)
		UART4->ICR = USART_ICR_ORECF;
	if (isr & USART_ISR_FE)
		UART4->ICR = USART_ICR_FECF;
	if ((isr & USART_ISR_RXNE_RXFNE) == 0)
		return 0;
	* cp = UART4->RDR;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	const uint_fast32_t isr = UART4->ISR;
	if (isr & USART_ISR_ORE)
		UART4->ICR = USART_ICR_ORECF;
	if (isr & USART_ISR_FE)
		UART4->ICR = USART_ICR_FECF;
	if ((isr & USART_ISR_RXNE) == 0)
		return 0;
	* cp = UART4->RDR;

#elif CPUSTYLE_TMS320F2833X

	if ((SCIBRXST & (1U << 6)) == 0)	// Wait for RXRDY bit
		return 0;
	* cp = SCIBRXBUF;

#elif CPUSTYLE_R7S721

	if ((SCIF4.SCFSR & (1U << 1)) == 0)	// RDF
		return 0;
	* cp = SCIF4.SCFRDR;
	SCIF4.SCFSR = (uint16_t) ~ (1U << 1);	// RDF=0 читать незачем (в примерах странное)

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507)

	if ((UART4->UART_USR & (1u << 3)) == 0)	// RX FIFO Not Empty
		return 0;
	* cp = UART4->UART_RBR_THR_DLL;

#else
	#error Undefined CPUSTYLE_XXX
#endif

	return 1;
}

/* передача символа если готов порт */
uint_fast8_t
hardware_uart4_putchar(uint_fast8_t c)
{
#if CPUSTYLE_ATXMEGAXXXA4

	if ((USARTE4.STATUS & USART_DREIF_bm) == 0)
		return 0;
	USARTE4.DATA = c;

#elif CPUSTYLE_ATMEGA_XXX4

	if ((UCSR4A & (1 << UDRE4)) == 0)
		return 0;
	UDR4 = c;

#elif CPUSTYLE_ATMEGA128

	if ((UCSR0A & (1 << UDRE0)) == 0)
		return 0;
	UDR4 = c;

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((UART4->SR & USART_SR_TXE) == 0)
		return 0;
	UART4->DR = c;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	if ((UART4->ISR & USART_ISR_TXE_TXFNF) == 0)
		return 0;
	UART4->TDR = c;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	if ((UART4->ISR & USART_ISR_TXE) == 0)
		return 0;
	UART4->TDR = c;

#elif CPUSTYLE_TMS320F2833X

	if ((SCIBCTL2 & (1U << 7)) == 0)	// wait for TXRDY bit
		return 0;
	SCIBTXBUF = c;

#elif CPUSTYLE_R7S721

	if ((SCIF4.SCFSR & (1U << SCIF4_SCFSR_TDFE_SHIFT)) == 0)	// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
		return 0;
	SCIF4.SCFTDR = c;
	SCIF4.SCFSR = (uint16_t) ~ (1U << SCIF4_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507)

	if ((UART4->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		return 0;
	UART4->UART_RBR_THR_DLL = c;

#else
	#error Undefined CPUSTYLE_XXX
#endif

	return 1;
}

void hardware_uart4_initialize(uint_fast8_t debug)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB1ENR |= RCC_APB1ENR_UART4EN; // Включение тактирования UART4.
	(void) RCC->APB1ENR;

	UART4->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables


	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     //включить тактирование альтернативных функций
	(void) RCC->APB2ENR;

	HARDWARE_UART4_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(UART4_IRQn, & UART4_IRQHandler);
	}

	UART4->CR1 |= USART_CR1_UE; // Включение UART4.

#elif CPUSTYLE_STM32H7XX

	RCC->APB1LENR |= RCC_APB1LENR_UART4EN; // Включение тактирования UART4.
	(void) RCC->APB1LENR;

	UART4->CR1 = 0;

#if WITHUARTFIFO
	UART4->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUARTFIFO */
	UART4->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUARTFIFO */

	UART4->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART4_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(UART4_IRQn, & UART4_IRQHandler);
	}

	UART4->CR1 |= USART_CR1_UE; // Включение UART4.

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	RCC->APB1ENR |= RCC_APB1ENR_UART4EN; // Включение тактирования UART4.
	(void) RCC->APB1ENR;

	UART4->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART4_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(UART4_IRQn, & UART4_IRQHandler);
	}

	UART4->CR1 |= USART_CR1_UE; // Включение UART4.

#elif CPUSTYLE_ATMEGA128

	// USART initialization
	UCSR4B = (1U << RXEN1) | (1U << TXEN1) /* | (1U << UCSZ12) */;
	UCSR4C = (1U << UCSZ11) | (1U << UCSZ10);	// asynchronious mode, 8 bit.
	// enable pull-up registers for RXD and TXD pins: then rx or tx disabled, these pins disconnected fron UART
	//PORTE |= ((1U << PE0) | (1U << PE1));

#elif CPUSTYLE_ATXMEGAXXXA4

xxxx!;
	PORTE.DIRSET = PIN3_bm; // PE3 (TXD0) as output
	PORTE.DIRCLR = PIN2_bm; // PE2 (RXD0) as input
	PORTE_PIN2CTRL = (PORTE_PIN2CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc;							// pin is pulled high

	USARTE4.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;
	USARTE4.CTRLB = USART_RXEN_bm | USART_TXEN_bm;


#elif CPUSTYLE_ATMEGA32

	#error WITHUART2HW with CPUSTYLE_ATMEGA not supported

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

    /* ---- Supply clock to the SCIF(channel 4) ---- */
	CPG.STBCR4 &= ~ CPG_STBCR4_BIT_MSTP43;	// Module Stop 43 - SCIF4
	(void) CPG.STBCR4;			/* Dummy read */

	SCIF4.SCSCR = 0x0000;	/* SCIF transmitting and receiving operations stop, internal clock */

	SCIF4.SCSCR = (SCIF4.SCSCR & ~ 0x03) |
		0x00 |						// internal clock
		0;

	/* ---- Serial status register(SCFSR2) setting ---- */
	/* ER,BRK,DR bit clear */
	(void) SCIF4.SCFSR;						// Перед сбросом бита xxx должно произойти его чтение в ненулевом состоянии
	SCIF4.SCFSR = ~ 0x0091;	// 0xFF6E;

	/* ---- Line status register (SCLSR2) setting ---- */
	/* ORER bit clear */
	//SCIF4.SCLSR.BIT.ORER  = 0;
	SCIF4.SCLSR &= ~ 0x0001;

	SCIF4.SCSMR =
		0x00 |	/* 8-N-1 format */
		0;

	/* ---- FIFO control register (SCFCR2) setting ---- */
	/*  RTS output active trigger        :Initial value	*/
	/*  Receive FIFO data trigger        :1-data		*/
	/*  Transmit FIFO data trigger       :0-data		*/
	/*  Modem control enable             :Disabled		*/
	/*  Receive FIFO data register reset :Disabled		*/
	/*  Loop-back test                   :Disabled 		*/
	SCIF4.SCFCR = 0x0030;

	/* ---- Serial port register (SCSPTR2) setting ---- */
	/* Serial port  break output(SPB2IO)  1: Enabled */
	/* Serial port break data(SPB2DT)  1: High-level */
	//SCIF4.SCSPTR |= 0x0003;

	if (debug == 0)
	{
	   serial_set_handler(SCIFRXI4_IRQn, SCIFRXI4_IRQHandler);
	   serial_set_handler(SCIFTXI4_IRQn, SCIFTXI4_IRQHandler);
	}
	HARDWARE_UART4_INITIALIZE();	/* Присоединить периферию к выводам */

	SCIF4.SCSCR |= 0x0030;	// TE RE - SCIF4 transmitting and receiving operations are enabled */

#elif CPUSTYLE_STM32MP1

	RCC->MP_APB1ENSETR = RCC_MP_APB1ENSETR_UART4EN; // Включение тактирования UART4.
	(void) RCC->MP_APB1ENSETR;
	RCC->MP_APB1LPENSETR = RCC_MP_APB1LPENSETR_UART4LPEN; // Включение тактирования UART4.
	(void) RCC->MP_APB1LPENSETR;
	RCC->APB1RSTSETR = RCC_APB1RSTSETR_UART4RST; // Установить сброс UART4.
	(void) RCC->APB1RSTSETR;
	RCC->APB1RSTCLRR = RCC_APB1RSTCLRR_UART4RST; // Снять брос UART4.
	(void) RCC->APB1RSTCLRR;

	UART4->CR1 = 0;

#if WITHUARTFIFO
	UART4->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUARTFIFO */
	UART4->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUARTFIFO */

	UART4->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART4_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(UART4_IRQn, UART4_IRQHandler);
	}

	UART4->CR1 |= USART_CR1_UE; // Включение USART1.

#elif CPUSTYLE_A64

	const unsigned ix = 4;

	/* Open the clock gate for uart0 */
	CCU->BUS_CLK_GATING_REG3 |= (1u << (ix + 16));	// UART4_GATING

	/* De-assert uart0 reset */
	CCU-> BUS_SOFT_RST_REG4 |= (1u << (ix + 16));	//  UART4_RST

	/* Config uart0 to 115200-8-1-0 */
	uint32_t divisor = allwnrt113_get_usart_freq() / ((DEBUGSPEED) * 16);

	UART4->UART_DLH_IER = 0;
	UART4->UART_IIR_FCR = 0xf7;
	UART4->UART_MCR = 0x00;

	UART4->UART_LCR |= (1 << 7);	// Divisor Latch Access Bit
	UART4->UART_RBR_THR_DLL = divisor & 0xff;
	UART4->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART4->UART_LCR &= ~ (1 << 7);	// Divisor Latch Access Bit
	//
	UART4->UART_LCR &= ~ 0x1f;
	UART4->UART_LCR |= (0x3 << 0) | (0 << 2) | (0x0 << 3);	//DAT_LEN_8_BITS ONE_STOP_BIT NO_PARITY

	HARDWARE_UART4_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART4_IRQn, UART4_IRQHandler);
	}

#elif CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_T507

	const unsigned ix = 4;

	/* Open the clock gate for uart3 */
	CCU->UART_BGR_REG |= (1u << (ix + 0));

	/* De-assert uart3 reset */
	CCU->UART_BGR_REG |= (1u << (ix + 16));

	/* Config uart0 to 115200-8-1-0 */
	uint32_t divisor = allwnrt113_get_usart_freq() / ((DEBUGSPEED) * 16);

	UART4->UART_DLH_IER = 0;
	UART4->UART_IIR_FCR = 0xf7;
	UART4->UART_MCR = 0x00;

	UART4->UART_LCR |= (1 << 7);	// Divisor Latch Access Bit
	UART4->UART_RBR_THR_DLL = divisor & 0xff;
	UART4->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART4->UART_LCR &= ~ (1 << 7);	// Divisor Latch Access Bit
	//
	UART4->UART_LCR &= ~ 0x1f;
	UART4->UART_LCR |= (0x3 << 0) | (0 << 2) | (0x0 << 3);	//DAT_LEN_8_BITS ONE_STOP_BIT NO_PARITY

	HARDWARE_UART4_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART4_IRQn, UART4_IRQHandler);
	}

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

void
hardware_uart4_set_speed(uint_fast32_t baudrate)
{
#if CPUSTYLE_ATMEGA_XXX4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR4A |= (1U << U2X1);
	else
		UCSR4A &= ~ (1U << U2X1);

	UBRR4 = value;	/* Значение получено уже уменьшенное на 1 */

#elif CPUSTYLE_ATMEGA128

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR4A |= (1U << U2X1);
	else
		UCSR4A &= ~ (1U << U2X1);

	UBRR4H = (value >> 8) & 0xff;	/* Значение получено уже уменьшенное на 1 */
	UBRR4L = value & 0xff;

#elif CPUSTYLE_ATXMEGAXXXA4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATXMEGA_UBR_WIDTH, ATXMEGA_UBR_TAPS, & value, 1);
	if (prei == 0)
		USARTE4.CTRLB |= USART_CLK2X_bm;
	else
		USARTE4.CTRLB &= ~USART_CLK2X_bm;
	// todo: проверить требование к порядку обращения к портам
	USARTE4.BAUDCTRLA = (value & 0xff);	/* Значение получено уже уменьшенное на 1 */
	USARTE4.BAUDCTRLB = (ATXMEGA_UBR_BSEL << 4) | ((value >> 8) & 0x0f);

#elif CPUSTYLE_STM32MP1

	// uart4
	UART4->BRR = calcdivround2(stm32mp1_uart2_4_get_freq(), baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_STM32F

	// uart4 on apb1
	UART4->BRR = calcdivround2(BOARD_UART4_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_TMS320F2833X

	const unsigned long lspclk = CPU_FREQ / 4;
	const unsigned long brr = (lspclk / 8) / baudrate;	// @ CPU_FREQ = 100 MHz, 9600 can not be programmed

	SCIBHBAUD = (brr - 1) >> 8;		// write 8 bits, not 16
	SCIBLBAUD = (brr - 1) >> 0;

#elif CPUSTYLE_R7S721

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(baudrate), R7S721_SCIF_SCBRR_WIDTH, R7S721_SCIF_SCBRR_TAPS, & value, 1);

	SCIF4.SCSMR = (SCIF4.SCSMR & ~ 0x03) |
		scemr_scsmr [prei].scsmr |	// prescaler: 0: /1, 1: /4, 2: /16, 3: /64
		0;
	SCIF4.SCEMR = (SCIF4.SCEMR & ~ (0x80 | 0x01)) |
		0 * 0x80 |						// BGDM
		scemr_scsmr [prei].scemr |	// ABCS = 8/16 clocks per bit
		0;
	SCIF4.SCBRR = value;	/* Bit rate register */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507)

	unsigned divisor = calcdivround2(BOARD_USART_FREQ, baudrate * 16);

	UART4->UART_LCR |= (1 << 7);
	UART4->UART_RBR_THR_DLL = divisor & 0xff;
	UART4->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART4->UART_LCR &= ~ (1 << 7);

#else
	#warning Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART4HW */
