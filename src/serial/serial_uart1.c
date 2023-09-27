/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if WITHUART1HW

#include "serial.h"
#include "formats.h"	// for debug prints
#include "board.h"
#include "gpio.h"
#include <string.h>
#include <math.h>

#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	void RAMFUNC_NONILINE USART1_IRQHandler(void)
	{
		const uint_fast32_t sr = USART1->SR;

		if (sr & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE))
			HARDWARE_UART1_ONRXCHAR(USART1->DR);
		if (sr & (USART_SR_ORE | USART_SR_FE | USART_SR_NE))
			HARDWARE_UART1_ONOVERFLOW();
		if (sr & USART_SR_TXE)
			HARDWARE_UART1_ONTXCHAR(USART1);
	}

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32MP1

	void RAMFUNC_NONILINE USART1_IRQHandler(void)
	{
		const uint_fast32_t isr = USART1->ISR;

		if (isr & USART_ISR_RXNE)
			HARDWARE_UART1_ONRXCHAR(USART1->RDR);
		if (isr & USART_ISR_ORE)
		{
			USART1->ICR = USART_ICR_ORECF;
			HARDWARE_UART1_ONOVERFLOW();
		}
		if (isr & USART_ISR_FE)
			USART1->ICR = USART_ICR_FECF;
		if (isr & USART_ISR_TXE)
			HARDWARE_UART1_ONTXCHAR(USART1);
	}

#elif CPUSTYLE_AT91SAM7S

	static RAMFUNC_NONILINE void AT91F_US1Handler(void)
	{
		const uint_fast32_t csr = AT91C_BASE_US1->US_CSR;

		if (csr & AT91C_US_RXRDY)
			HARDWARE_UART1_ONRXCHAR(AT91C_BASE_US1->US_RHR);
		if (csr & AT91C_US_TXRDY)
			HARDWARE_UART1_ONTXCHAR(AT91C_BASE_US1);
	}

#elif CPUSTYLE_ATMEGA328

	ISR(USART_RX_vect)
	{
		HARDWARE_UART1_ONRXCHAR(UDR1);
	}

	ISR(USART_TX_vect)
	{
		HARDWARE_UART1_ONTXCHAR(NULL);
	}

#elif CPUSTYLE_ATMEGA_XXX4

	ISR(USART1_RX_vect)
	{
		HARDWARE_UART1_ONRXCHAR(UDR1);
	}

	ISR(USART1_TX_vect)
	{
		HARDWARE_UART1_ONTXCHAR(NULL);
	}

#elif CPUSTYLE_ATMEGA128

	ISR(USART1_RX_vect)
	{
		HARDWARE_UART1_ONRXCHAR(UDR0);
	}

	ISR(USART1_TX_vect)
	{
		HARDWARE_UART1_ONTXCHAR(NULL);
	}

#elif CPUSTYLE_ATXMEGAXXXA4

	ISR(USARTE1_RXC_vect)
	{
		HARDWARE_UART1_ONRXCHAR(USARTE1.DATA);
	}

	ISR(USARTE1_DRE_vect)
	{
		HARDWARE_UART1_ONTXCHAR(& USARTE1);
	}

#elif CPUSTYLE_R7S721

	// Приём символа он последовательного порта
	static RAMFUNC_NONILINE void SCIFRXI1_IRQHandler(void)
	{
		(void) SCIF1.SCFSR;						// Перед сбросом бита RDF должно произойти его чтение в ненулевом состоянии
		SCIF1.SCFSR = (uint16_t) ~ SCIF1_SCFSR_RDF;	// RDF=0 читать незачем (в примерах странное - сбрасывабтся и другие биты)
		uint_fast8_t n = (SCIF1.SCFDR & SCIF1_SCFDR_R) >> SCIF1_SCFDR_R_SHIFT;
		while (n --)
			HARDWARE_UART1_ONRXCHAR(SCIF1.SCFRDR & SCIF1_SCFRDR_D);
	}

	// Передача символа в последовательный порт
	static RAMFUNC_NONILINE void SCIFTXI1_IRQHandler(void)
	{
		HARDWARE_UART1_ONTXCHAR(& SCIF1);
	}

#elif CPUSTYLE_XC7Z

	static void UART1_IRQHandler(void)
	{
		char c;
		UART1->ISR = UART1->IMR;	// clear interrupt status

		while (hardware_uart1_getchar(& c))
		{
			HARDWARE_UART1_ONRXCHAR(c);
		}
	}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	static RAMFUNC_NONILINE void UART1_IRQHandler(void)
	{
		const uint_fast32_t ier = UART1->UART_DLH_IER;
		const uint_fast32_t usr = UART1->UART_USR;

		if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
		{
			if (usr & (1u << 3))	// RX FIFO Not Empty
				HARDWARE_UART1_ONRXCHAR(UART1->UART_RBR_THR_DLL);
		}
		if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
		{
			if (usr & (1u << 1))	// TX FIFO Not Full
				HARDWARE_UART1_ONTXCHAR(UART1);
		}
	}

#elif CPUSTYLE_VM14

	static RAMFUNC_NONILINE void UART1_IRQHandler(void)
	{
		const uint_fast32_t ier = UART1->UART_DLH_IER;
		const uint_fast32_t usr = UART1->UART_USR;

		if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
		{
			if (usr & (1u << 3))	// RX FIFO Not Empty
				HARDWARE_UART1_ONRXCHAR(UART1->UART_RBR_THR_DLL);
		}
		if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
		{
			if (usr & (1u << 1))	// TX FIFO Not Full
				HARDWARE_UART1_ONTXCHAR(UART1);
		}

	}

#else
	#error Undefined CPUSTYLE_XXX
#endif	/* CPUSTYLE_ATMEGA_XXX4 */


/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uart1_enabletx(uint_fast8_t state)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART1

		if (state)
			USART1->US_IER = US_IER_TXRDY;
		else
			USART1->US_IDR = US_IDR_TXRDY;

	#elif HARDWARE_ARM_USEUART1

		if (state)
			UART1->UART_IER = UART_IER_TXRDY;
		else
			UART1->UART_IDR = UART_IDR_TXRDY;

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_AT91SAM7S

	if (state)
		AT91C_BASE_US1->US_IER = AT91C_US_TXRDY;
	else
		AT91C_BASE_US1->US_IDR = AT91C_US_TXRDY;

#elif CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

	if (state)
		USART1->CR1 |= USART_CR1_TXEIE;
	else
		USART1->CR1 &= ~ USART_CR1_TXEIE;

#elif CPUSTYLE_ATMEGA_XXX4

	/* Used USART 1 */
	if (state)
	{
		UCSR1B |= (1U << TXCIE0);
		HARDWARE_UART1_ONTXCHAR(NULL);	// initiate 1-st character sending
	}
	else
	{
		UCSR1B &= ~ (1U << TXCIE0);
	}

#elif CPUSTYLE_ATMEGA32

	if (state)
	{
		UCSRB |= (1U << TXCIE);
		HARDWARE_UART1_ONTXCHAR(NULL);	// initiate 1-st character sending
	}
	else
	{
		UCSRB &= ~ (1U << TXCIE);
	}

#elif CPUSTYLE_ATMEGA128

	/* Used USART 1 */
	if (state)
	{
		UCSR1B |= (1U << TXCIE0);
		HARDWARE_UART1_ONTXCHAR(NULL);	// initiate 1-st character sending
	}
	else
	{
		UCSR1B &= ~ (1U << TXCIE0);
	}

#elif CPUSTYLE_ATXMEGAXXXA4

	if (state)
		USARTE1.CTRLA = (USARTE1.CTRLA & ~ USART_DREINTLVL_gm) | USART_DREINTLVL_LO_gc;
	else
		USARTE1.CTRLA = (USARTE1.CTRLA & ~ USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;

#elif CPUSTYLE_TMS320F2833X

	if (state)
		SCIACTL2 |= (1U << 0);	// TX INT ENA
	else
		SCIACTL2 &= ~ (1U << 0); // TX INT ENA

#elif CPUSTYLE_R7S721

	if (state)
		SCIF1.SCSCR |= SCIF1_SCSCR_TIE;	// TIE Transmit Interrupt Enable
	else
		SCIF1.SCSCR &= ~ SCIF1_SCSCR_TIE;	// TIE Transmit Interrupt Enable

#elif CPUSTYLE_XC7Z

	if (state)
		 UART1->IER |= 0*(1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt
	else
		 UART1->IER &= ~ 0*(1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	if (state)
		UART1->UART_DLH_IER |= (1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt
	else
		UART1->UART_DLH_IER &= ~ (1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt

#elif CPUSTYLE_VM14

	if (state)
		 UART1->UART_DLH_IER |= 0*(1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt
	else
		 UART1->UART_DLH_IER &= ~ 0*(1u << 1);	// ETBEI Enable Transmit Holding Register Empty Interrupt

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uart1_enablerx(uint_fast8_t state)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART1

		if (state)
			USART1->US_IER = US_IER_RXRDY;
		else
			USART1->US_IDR = US_IDR_RXRDY;

	#elif HARDWARE_ARM_USEUART1

		if (state)
			UART1->UART_IER = UART_IER_RXRDY;
		else
			UART1->UART_IDR = UART_IDR_RXRDY;

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_AT91SAM7S

	if (state)
		AT91C_BASE_US1->US_IER = AT91C_US_RXRDY;
	else
		AT91C_BASE_US1->US_IDR = AT91C_US_RXRDY;


#elif CPUSTYLE_ATMEGA_XXX4
	/* Used USART 0 */
	if (state)
		UCSR1B |= (1U << RXCIE0);
	else
		UCSR1B &= ~ (1U << RXCIE0);

#elif CPUSTYLE_ATMEGA32

	if (state)
		UCSRB |= (1U << RXCIE);
	else
		UCSRB &= ~ (1U << RXCIE);

#elif CPUSTYLE_ATMEGA128

	/* Used USART 1 */
	if (state)
		UCSR1B |= (1U << RXCIE0);
	else
		UCSR1B &= ~ (1U << RXCIE0);

#elif CPUSTYLE_ATXMEGAXXXA4

	if (state)
		USARTE1.CTRLA = (USARTE1.CTRLA & ~ USART_RXCINTLVL_gm) | USART_RXCINTLVL_LO_gc;
	else
		USARTE1.CTRLA = (USARTE1.CTRLA & ~ USART_RXCINTLVL_gm) | USART_RXCINTLVL_OFF_gc;

#elif CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

	if (state)
		USART1->CR1 |= USART_CR1_RXNEIE;
	else
		USART1->CR1 &= ~ USART_CR1_RXNEIE;

#elif CPUSTYLE_TMS320F2833X

	if (state)
		SCIACTL2 |= (1U << 1);	// RX/BK INT ENA
	else
		SCIACTL2 &= ~ (1U << 1); // RX/BK INT ENA

#elif CPUSTYLE_R7S721

	if (state)
		SCIF1.SCSCR |= SCIF1_SCSCR_RIE;	// RIE Receive Interrupt Enable
	else
		SCIF1.SCSCR &= ~ SCIF1_SCSCR_RIE;	// RIE Receive Interrupt Enable

#elif CPUSTYLE_XC7Z

	uint32_t mask = state ? 1 : 0; 			/* RX FIFO trigger interrupt */
	UART1->RXWM = 1; 						/* set RX FIFO Trigger Level */
	UART1->IER = mask;
	UART1->IDR = ~ mask;

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	if (state)
		 UART1->UART_DLH_IER |= (1u << 0);	// ERBFI Enable Received Data Available Interrupt
	else
		 UART1->UART_DLH_IER &= ~ (1u << 0);	// ERBFI Enable Received Data Available Interrupt

#elif CPUSTYLE_VM14

	if (state)
		 UART1->UART_DLH_IER |= (1u << 0);	// ERBFI Enable Received Data Available Interrupt
	else
		 UART1->UART_DLH_IER &= ~ (1u << 0);	// ERBFI Enable Received Data Available Interrupt

#else
	#error Undefined CPUSTYLE_XXX
#endif
}


/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uart1_tx(void * ctx, uint_fast8_t c)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART1
		USART1->US_THR = c;
	#elif HARDWARE_ARM_USEUART1
		UART1->UART_THR = c;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	AT91C_BASE_US1->US_THR = c;

#elif CPUSTYLE_ATMEGA128

	UDR1 = c;

#elif CPUSTYLE_ATMEGA_XXX4

	UDR1 = c;

#elif CPUSTYLE_ATMEGA32

	UDR = c;

#elif CPUSTYLE_ATXMEGAXXXA4

	USARTE1.DATA = c;

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	USART1->DR = c;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32MP1

	USART1->TDR = c;

#elif CPUSTYLE_TMS320F2833X

	SCIATXBUF = c;

#elif CPUSTYLE_R7S721

	(void) SCIF1.SCFSR;			// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
	SCIF1.SCFTDR = c;
	SCIF1.SCFSR = (uint16_t) ~ (1U << SCIF1_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#elif CPUSTYLE_STM32MP1
	#warning Insert code for CPUSTYLE_STM32MP1

#elif CPUSTYLE_XC7Z

	UART1->FIFO = c;

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	UART1->UART_RBR_THR_DLL = c;

#elif CPUSTYLE_VM14

	UART1->UART_RBR_THR_DLL = c;

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* дождаться, когда буде все передано */
void hardware_uart1_flush(void)
{

}


/* приём символа, если готов порт */
uint_fast8_t
hardware_uart1_getchar(char * cp)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART1
		if ((USART1->US_CSR & US_CSR_RXRDY) == 0)
			return 0;
		* cp = USART1->US_RHR;
	#elif HARDWARE_ARM_USEUART1
		if ((UART1->UART_SR & UART_SR_RXRDY) == 0)
			return 0;
		* cp = UART1->UART_RHR;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	if ((AT91C_BASE_US1->US_CSR & AT91C_US_RXRDY) == 0)
		return 0;
	* cp = AT91C_BASE_US1->US_RHR;

#elif CPUSTYLE_ATXMEGAXXXA4

	if ((USARTE1.STATUS & (1 << USART_RXCIF_bp)) == 0)
			return 0;
	* cp = USARTE1.DATA;

#elif CPUSTYLE_ATMEGA128

	if ((UCSR1A & (1 << RXC0)) == 0)
			return 0;
	* cp = UDR0;

#elif CPUSTYLE_ATMEGA_XXX4

	if ((UCSR1A & (1 << RXC0)) == 0)
			return 0;
	* cp = UDR1;

#elif CPUSTYLE_ATMEGA32

	if ((UCSRA & (1 << RXC)) == 0)
			return 0;
	* cp = UDR;

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((USART1->SR & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE)) == 0)
		return 0;
	* cp = USART1->DR;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32MP1
	const uint_fast32_t isr = USART1->ISR;
	if (isr & USART_ISR_ORE)
		USART1->ICR = USART_ICR_ORECF;
	if (isr & USART_ISR_FE)
		USART1->ICR = USART_ICR_FECF;
	if ((isr & USART_ISR_RXNE) == 0)
		return 0;
	* cp = USART1->RDR;

#elif CPUSTYLE_TMS320F2833X

	if ((SCIARXST & (1U << 6)) == 0)	// Wait for RXRDY bit
		return 0;
	* cp = SCIARXBUF;

#elif CPUSTYLE_R7S721

	if ((SCIF1.SCFSR & (1U << 1)) == 0)	// RDF
		return 0;
	* cp = SCIF1.SCFRDR;
	SCIF1.SCFSR = (uint16_t) ~ (1U << 1);	// RDF=0 читать незачем (в примерах странное)

#elif CPUSTYLE_XC7Z

	if ((UART1->SR & XUARTPS_SR_RXEMPTY) != 0)
		return 0;
	* cp = UART1->FIFO;

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	if ((UART1->UART_USR & (1u << 3)) == 0)	// RX FIFO Not Empty
		return 0;
	* cp = UART1->UART_RBR_THR_DLL;

#elif CPUSTYLE_VM14

	if ((UART1->UART_USR & (1u << 3)) == 0)	// RFNE - RX FIFO Not Empty
		return 0;
	* cp = UART1->UART_RBR_THR_DLL;

#else
	#error Undefined CPUSTYLE_XXX
#endif

	return 1;
}

/* передача символа если готов порт */
uint_fast8_t
hardware_uart1_putchar(uint_fast8_t c)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART1
		if ((USART1->US_CSR & US_CSR_TXRDY) == 0)
			return 0;
		USART1->US_THR = c;
	#elif HARDWARE_ARM_USEUART1
		if ((UART1->UART_SR & UART_SR_TXRDY) == 0)
			return 0;
		UART1->UART_THR = c;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	if ((AT91C_BASE_US1->US_CSR & AT91C_US_TXRDY) == 0)
		return 0;
	AT91C_BASE_US1->US_THR = c;

#elif CPUSTYLE_ATXMEGAXXXA4

	if ((USARTE1.STATUS & USART_DREIF_bm) == 0)
		return 0;
	USARTE1.DATA = c;

#elif CPUSTYLE_ATMEGA_XXX4

	if ((UCSR1A & (1 << UDRE0)) == 0)
		return 0;
	UDR1 = c;

#elif CPUSTYLE_ATMEGA128

	if ((UCSR1A & (1 << UDRE0)) == 0)
		return 0;
	UDR1 = c;

#elif CPUSTYLE_ATMEGA32

	if ((UCSRA & (1 << UDRE)) == 0)
		return 0;
	UDR = c;

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((USART1->SR & USART_SR_TXE) == 0)
		return 0;
	USART1->DR = c;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32MP1

	if ((USART1->ISR & USART_ISR_TXE) == 0)
		return 0;
	USART1->TDR = c;

#elif CPUSTYLE_TMS320F2833X

	if ((SCIACTL2 & (1U << 7)) == 0)	// wait for TXRDY bit
		return 0;
	SCIATXBUF = c;

#elif CPUSTYLE_R7S721

	if ((SCIF1.SCFSR & (1U << SCIF1_SCFSR_TDFE_SHIFT)) == 0)	// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
		return 0;
	SCIF1.SCFTDR = c;
	SCIF1.SCFSR = (uint16_t) ~ (1U << SCIF1_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#elif CPUSTYLE_XC7Z

	if ((UART1->SR & XUARTPS_SR_TNFUL) != 0)
		return 0;
	UART1->FIFO = c;

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	if ((UART1->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		return 0;
	UART1->UART_RBR_THR_DLL = c;

#elif CPUSTYLE_VM14

	if ((UART1->UART_USR & (1u << 1)) == 0)	// TFNF TX FIFO Not Full
		return 0;
	UART1->UART_RBR_THR_DLL = c;

#else
	#error Undefined CPUSTYLE_XXX
#endif

	return 1;
}

void hardware_uart1_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART1
		// enable the clock of USART1
		PMC->PMC_PCER0 = 1u << ID_USART1;

		HARDWARE_UART1_INITIALIZE();	/* Присоединить периферию к выводам */

		// reset the UART
		USART1->US_CR = US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS;
		// set serial line mode
		USART1->US_MR =
			US_MR_CHMODE_NORMAL |// Normal Mode
			US_MR_USCLKS_MCK |   // Clock = MCK
			US_MR_CHRL_8_BIT |
			US_MR_PAR_NO |
			US_MR_NBSTOP_1_BIT;

		USART1->US_IDR = (US_IDR_RXRDY | US_IDR_TXRDY);

		if (debug == 0)
		{
			serial_set_handler(USART1_IRQn, & USART1_IRQHandler);
		}

		USART1->US_CR = US_CR_RXEN | US_CR_TXEN;	// разрешаем приёмник и передатчик.

	#elif HARDWARE_ARM_USEUART1
		// enable the clock of UART1
		PMC->PMC_PCER0 = 1u << ID_UART1;

		HARDWARE_UART1_INITIALIZE();	/* Присоединить периферию к выводам */

		// reset the UART
		UART1->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;
		// set serial line mode
		UART1->UART_MR =
			UART_MR_CHMODE_NORMAL |// Normal Mode
			//UART_MR_USCLKS_MCK |   // Clock = MCK
			//UART_MR_CHRL_8_BIT |
			UART_MR_PAR_NO |
			//UART_MR_NBSTOP_1_BIT |
			0;

		UART1->UART_IDR = (UART_IDR_RXRDY | UART_IDR_TXRDY);

		if (debug == 0)
		{
			serial_set_handler(UART1_IRQn, & UART1_IRQHandler);
		}

		UART1->UART_CR = UART_CR_RXEN | UART_CR_TXEN;	// разрешаем приёмник и передатчик.

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */


#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Включение тактирования USART1.
	(void) RCC->APB2ENR;

	USART1->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables


	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     //включить тактирование альтернативных функций
	(void) RCC->APB2ENR;

	HARDWARE_UART1_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(USART1_IRQn, & USART1_IRQHandler);
	}

	USART1->CR1 |= USART_CR1_UE; // Включение USART1.

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Включение тактирования USART1.
	(void) RCC->APB2ENR;

	USART1->CR1 = 0;

#if ! defined (__CORTEX_M)
#if WITHUARTFIFO
	USART1->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUARTFIFO */
	USART1->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUARTFIFO */
#endif

	USART1->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART1_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(USART1_IRQn, & USART1_IRQHandler);
	}

	USART1->CR1 |= USART_CR1_UE; // Включение USART1.

#elif CPUSTYLE_AT91SAM7S

	// enable the clock of USART1
	AT91C_BASE_PMC->PMC_PCER = 1u << AT91C_ID_US1;

	HARDWARE_UART1_INITIALIZE();	/* Присоединить периферию к выводам */

	// enable uart pins on PIO
	AT91C_BASE_PIOA->PIO_ASR = AT91C_PA21_RXD1 | AT91C_PA22_TXD1; // assigns the 2 I/O lines to peripheral A function
	AT91C_BASE_PIOA->PIO_PDR = AT91C_PA21_RXD1 | AT91C_PA22_TXD1;	// enable peripheral control of PA21,PA22 (RXD1 and TXD1)
	// reset the UART
	AT91C_BASE_US1->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS;
	// set serial line mode
	AT91C_BASE_US1->US_MR =
						AT91C_US_OVER |
						AT91C_US_USMODE_NORMAL |// Normal Mode
					   AT91C_US_CLKS_CLOCK |   // Clock = MCK
					   AT91C_US_CHRL_8_BITS |
					   AT91C_US_PAR_NONE |
					   AT91C_US_NBSTOP_1_BIT;

	AT91C_BASE_US1->US_IDR = (AT91C_US_RXRDY | AT91C_US_TXRDY);

	if (debug == 0)
	{
		serial_set_handler(AT91C_ID_US1, AT91F_US1Handler);
	}

	AT91C_BASE_US1->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;	// разрешаем приёмник и передатчик.

#elif CPUSTYLE_ATMEGA_XXX4

	// USART initialization
	UCSR1B = (1U << RXEN1) | (1U << TXEN1) /* | (1U << UCSZ02) */;
	UCSR1C = (1U << UCSZ01) | (1U << UCSZ00);	// asynchronious mode, 8 bit.
	// enable pull-up registers for RXD and TXD pins: then rx or tx disabled, these pins disconnected fron UART
	PORTD |= ((1U << PD0) | (1U << PD1));
#if defined (DDRD0) && defined (DDRD1)
	DDRD &= ~ ((1U << DDRD0) | (1U << DDRD1));
#else
	DDRD &= ~ ((1U << DDD0) | (1U << DDD1));
#endif

#elif CPUSTYLE_ATMEGA128

	// USART initialization
	UCSR1B = (1U << RXEN1) | (1U << TXEN1) /* | (1U << UCSZ02) */;
	UCSR1C = (1U << UCSZ01) | (1U << UCSZ00);	// asynchronious mode, 8 bit.
	// enable pull-up registers for RXD and TXD pins: then rx or tx disabled, these pins disconnected fron UART
	//PORTE |= ((1U << PE0) | (1U << PE1));

#elif CPUSTYLE_ATMEGA

	// USART initialization
	UCSRB = (1U << RXEN) | (1U << TXEN) /* | (1U << UCSZ2) */;
	UCSRC = (1U << URSEL) | (1U << UCSZ1) | (1U << UCSZ0);	// asynchronious mode, 8 bit

	// enable pull-up registers for RXD and TXD pins: then rx or tx disabled, these pins disconnected fron UART
	PORTD |= ((1U << PD0) | (1U << PD1));

#if defined (DDRD0) && defined (DDRD1)
	DDRD &= ~ ((1U << DDRD0) | (1U << DDRD1));
#else
	DDRD &= ~ ((1U << DDD0) | (1U << DDD1));
#endif

#elif CPUSTYLE_ATXMEGAXXXA4

	PORTE.DIRSET = PIN3_bm; // PE3 (TXD0) as output
	PORTE.DIRCLR = PIN2_bm; // PE2 (RXD0) as input
	PORTE_PIN2CTRL = (PORTE_PIN2CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc;							// pin is pulled high

	USARTE1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;
	USARTE1.CTRLB = USART_RXEN_bm | USART_TXEN_bm;

#elif CPUSTYLE_TMS320F2833X

	// Enable SCI-A clock
	PCLKCR0 |= (1U << 10);	// SCIAENCLK

	//SCIACTL1 &= ~ (1U << 5);	// SW RESET on
	SCIACTL1 |= (1U << 5);	// SW RESET off

	SCIACCR =
			(7U << 0) |	// Data length = 8 bit
			(0U << 5) | // Parity enable
			(0U << 6) | // Evan/Odd parity
			(0U << 7);	// 0 - one stop bit, 1 - to stop bits


	tms320_hardware_piob_periph(
			(1u << (35 % 32)) |	// SCITXDA
			(1u << (36 % 32)),	// SCIRXDA
			1	// mux = 1
			);

	SCIACTL1 |= (1U << 0) |	// RX enable
				(1U << 1);	// TX enable

#elif CPUSTYLE_R7S721

    /* ---- Supply clock to the SCIF(channel 1) ---- */
	CPG.STBCR4 &= ~ CPG_STBCR4_BIT_MSTP46;	// Module Stop 46 SCIF1
	(void) CPG.STBCR4;			/* Dummy read */

	SCIF1.SCSCR = 0x0000;	/* SCIF transmitting and receiving operations stop, internal clock */

	SCIF1.SCSCR = (SCIF0.SCSCR & ~ 0x03) |
		0x00 |						// internal clock
		0;

	/* ---- Serial status register(SCFSR2) setting ---- */
	/* ER,BRK,DR bit clear */
	(void) SCIF1.SCFSR;						// Перед сбросом бита xxx должно произойти его чтение в ненулевом состоянии
	SCIF1.SCFSR = ~ 0x0091;	// 0xFF6E;

	/* ---- Line status register (SCLSR2) setting ---- */
	/* ORER bit clear */
	//SCIF0.SCLSR.BIT.ORER  = 0;
	SCIF1.SCLSR &= ~ 0x0001;

	SCIF1.SCSMR =
		0x00 |	/* 8-N-1 format */
		0;

	/* ---- FIFO control register (SCFCR2) setting ---- */
	/*  RTS output active trigger        :Initial value	*/
	/*  Receive FIFO data trigger        :1-data		*/
	/*  Transmit FIFO data trigger       :0-data		*/
	/*  Modem control enable             :Disabled		*/
	/*  Receive FIFO data register reset :Disabled		*/
	/*  Loop-back test                   :Disabled 		*/
	SCIF1.SCFCR = 0x0030;

	/* ---- Serial port register (SCSPTR2) setting ---- */
	/* Serial port  break output(SPB2IO)  1: Enabled */
	/* Serial port break data(SPB2DT)  1: High-level */
	//SCIF0.SCSPTR |= 0x0003;

	if (debug == 0)
	{
	   serial_set_handler(SCIFRXI1_IRQn, SCIFRXI1_IRQHandler);
	   serial_set_handler(SCIFTXI1_IRQn, SCIFTXI1_IRQHandler);
	}
	HARDWARE_UART1_INITIALIZE();	/* Присоединить периферию к выводам */

	SCIF1.SCSCR |= 0x0030;	// TE RE - SCIF1 transmitting and receiving operations are enabled */

#elif CPUSTYLE_STM32MP1

	RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_USART1EN; // Включение тактирования USART1.
	(void) RCC->MP_APB5ENSETR;
	RCC->MP_APB5LPENSETR = RCC_MP_APB5LPENSETR_USART1LPEN; // Включение тактирования USART1.
	(void) RCC->MP_APB5LPENSETR;
	RCC->APB5RSTSETR = RCC_APB5RSTSETR_USART1RST; // Установить сброс USART1.
	(void) RCC->APB5RSTSETR;
	RCC->APB5RSTCLRR = RCC_APB5RSTCLRR_USART1RST; // Снять брос USART1.
	(void) RCC->APB5RSTCLRR;

	USART1->CR1 = 0;

#if WITHUARTFIFO
	USART1->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUARTFIFO */
	USART1->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUARTFIFO */

	USART1->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART1_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
	   serial_set_handler(USART1_IRQn, USART1_IRQHandler);
	}

	USART1->CR1 |= USART_CR1_UE; // Включение USART1.

#elif CPUSTYLE_XC7Z

	const unsigned ix = 1;
	SCLR->SLCR_UNLOCK = 0x0000DF0DU;
    SCLR->APER_CLK_CTRL |= (1u << (20 + ix));    // APER_CLK_CTRL.UART1_CPU_1XCLKACT
    //EMIT_MASKWRITE(0XF8000154, 0x00003F33U ,0x00001002U),	// UART_CLK_CTRL
	SCLR->UART_CLK_CTRL = (SCLR->UART_CLK_CTRL & ~ (0x00003F30U)) |
			((uint_fast32_t) SCLR_UART_CLK_CTRL_DIVISOR_VALUE << 8) | // DIVISOR
			(0x00u << 4) |	// SRCSEL - 0x: IO PLL
			(0x01 << ix) |	// CLKACT1 - UART 1 reference clock active
			0;

	uint32_t r; // Temporary value variable
	r = UART1->CR;
	r &= ~(XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN); // Clear Tx & Rx Enable
	r |= XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS; // Tx & Rx Disable
	UART1->CR = r;

	UART1->MR = 0;
	UART1->MR &= ~XUARTPS_MR_CLKSEL; // Clear "Input clock selection" - 0: clock source is uart_ref_clk
	UART1->MR |= XUARTPS_MR_CHARLEN_8_BIT; 	// Set "8 bits data"
	UART1->MR |= XUARTPS_MR_PARITY_NONE; 	// Set "No parity mode"
	UART1->MR |= XUARTPS_MR_STOPMODE_1_BIT; // Set "1 stop bit"
	UART1->MR |= XUARTPS_MR_CHMODE_NORM; 	// Set "Normal mode"

	UART1->CR |= (XUARTPS_CR_TXRST | XUARTPS_CR_RXRST); // TX & RX logic reset

	r = UART1->CR;
	r |= XUARTPS_CR_RX_EN | XUARTPS_CR_TX_EN; // Set TX & RX enabled
	r &= ~(XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS); // Clear TX & RX disabled
	UART1->CR = r;

	HARDWARE_UART1_INITIALIZE();	/* Присоединить периферию к выводам */

#elif CPUSTYLE_A64

	const unsigned ix = 1;

	/* Open the clock gate for uart0 */
	CCU->BUS_CLK_GATING_REG3 |= (1u << (ix + 16));	// UART1_GATING

	/* De-assert uart0 reset */
	CCU-> BUS_SOFT_RST_REG4 |= (1u << (ix + 16));	//  UART1_RST

	/* Config uart0 to 115200-8-1-0 */
	uint32_t divisor = allwnrt113_get_uart_freq() / ((defbaudrate) * 16);

	UART1->UART_DLH_IER = 0;
	UART1->UART_IIR_FCR = 0xf7;
	UART1->UART_MCR = 0x00;

	UART1->UART_LCR |= (1 << 7);	// Divisor Latch Access Bit
	UART1->UART_RBR_THR_DLL = divisor & 0xff;
	UART1->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART1->UART_LCR &= ~ (1 << 7);	// Divisor Latch Access Bit
	//
	UART1->UART_LCR &= ~ 0x1f;
	UART1->UART_LCR |= (0x3 << 0) | (0 << 2) | (0x0 << 3);	//DAT_LEN_8_BITS ONE_STOP_BIT NO_PARITY

	HARDWARE_UART1_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART1_IRQn, UART1_IRQHandler);
	}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	const unsigned ix = 1;

	/* Open the clock gate for uart1 */
	CCU->UART_BGR_REG |= (1u << (ix + 0));

	/* De-assert uart1 reset */
	CCU->UART_BGR_REG |= (1u << (ix + 16));

	/* Config uart0 to 115200-8-1-0 */
	uint32_t divisor = allwnrt113_get_uart_freq() / ((defbaudrate) * 16);

	UART1->UART_DLH_IER = 0;
	UART1->UART_IIR_FCR = 0xf7;
	UART1->UART_MCR = 0x00;

	UART1->UART_LCR |= (1 << 7);	// Divisor Latch Access Bit
	UART1->UART_RBR_THR_DLL = divisor & 0xff;
	UART1->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART1->UART_LCR &= ~ (1 << 7);	// Divisor Latch Access Bit
	//
	UART1->UART_LCR &= ~ 0x1f;
	UART1->UART_LCR |= (0x3 << 0) | (0 << 2) | (0x0 << 3);	//DAT_LEN_8_BITS ONE_STOP_BIT NO_PARITY

	HARDWARE_UART1_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART1_IRQn, UART1_IRQHandler);
	}

#elif CPUSTYLE_VM14

	const unsigned ix = 1;	// UART1

	CMCTR->GATE_SYS_CTR |= ((1u << 12) << ix); // UART1_EN Enable CLK

	/* Config uart0 to 115200-8-1-0 */
	uint32_t divisor = elveesvm14_get_usart_freq() / ((defbaudrate) * 16);

	UART1->UART_DLH_IER = 0;
	UART1->UART_IIR_FCR = 0xf7;
	UART1->UART_MCR = 0x00;

	UART1->UART_LCR |= (1 << 7);	// Divisor Latch Access Bit
	UART1->UART_RBR_THR_DLL = divisor & 0xff;
	UART1->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART1->UART_LCR &= ~ (1 << 7);	// Divisor Latch Access Bit
	//
	UART1->UART_LCR &= ~ 0x1f;
	UART1->UART_LCR |= (0x3 << 0) | (0 << 2) | (0x0 << 3);	//DAT_LEN_8_BITS ONE_STOP_BIT NO_PARITY

	(void) UART1->UART_LCR;

	HARDWARE_UART1_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART1_IRQn, UART1_IRQHandler);
	}

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

void
hardware_uart1_set_speed(uint_fast32_t baudrate)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART1
		// Использование автоматического расчёта предделителя
		unsigned value;
		const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
		USART1->US_BRGR = value;
		if (prei == 0)
		{
			USART1->US_MR |= US_MR_OVER;
		}
		else
		{
			USART1->US_MR &= ~ US_MR_OVER;
		}

	#elif HARDWARE_ARM_USEUART1
		// Использование автоматического расчёта предделителя
		unsigned value;
		calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART1->UART_BRGR = value;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), AT91SAM7_USART_BRGR_WIDTH, AT91SAM7_USART_BRGR_TAPS, & value, 0);

	AT91C_BASE_US1->US_BRGR = value;
	if (prei == 0)
	{
		AT91C_BASE_US1->US_MR |= AT91C_US_OVER;
	}
	else
	{
		AT91C_BASE_US1->US_MR &= ~ AT91C_US_OVER;
	}

#elif CPUSTYLE_ATMEGA_XXX4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR1A |= (1U << U2X0);
	else
		UCSR1A &= ~ (1U << U2X0);

	UBRR1 = value;	/* Значение получено уже уменьшенное на 1 */


#elif CPUSTYLE_ATMEGA128

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR1A |= (1U << U2X0);
	else
		UCSR1A &= ~ (1U << U2X0);

	UBRR1H = (value >> 8) & 0xff;	/* Значение получено уже уменьшенное на 1 */
	UBRR1L = value & 0xff;

#elif CPUSTYLE_ATXMEGAXXXA4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATXMEGA_UBR_WIDTH, ATXMEGA_UBR_TAPS, & value, 1);
	if (prei == 0)
		USARTE1.CTRLB |= USART_CLK2X_bm;
	else
		USARTE1.CTRLB &= ~USART_CLK2X_bm;
	// todo: проверить требование к порядку обращения к портам
	USARTE1.BAUDCTRLA = (value & 0xff);	/* Значение получено уже уменьшенное на 1 */
	USARTE1.BAUDCTRLB = (ATXMEGA_UBR_BSEL << 4) | ((value >> 8) & 0x0f);

#elif CPUSTYLE_STM32MP1

	// usart1
	USART1->BRR = calcdivround2(BOARD_USART1_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_STM32F

	// uart1 on apb2 up to 72/36 MHz

	USART1->BRR = calcdivround2(BOARD_USART1_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_TMS320F2833X

	const unsigned long lspclk = CPU_FREQ / 4;
	const unsigned long brr = (lspclk / 8) / baudrate;	// @ CPU_FREQ = 100 MHz, 9600 can not be programmed

	SCIAHBAUD = (brr - 1) >> 8;		// write 8 bits, not 16
	SCIALBAUD = (brr - 1) >> 0;

#elif CPUSTYLE_R7S721

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(baudrate), R7S721_SCIF_SCBRR_WIDTH, R7S721_SCIF_SCBRR_TAPS, & value, 1);

	SCIF1.SCSMR = (SCIF1.SCSMR & ~ 0x03) |
		scemr_scsmr [prei].scsmr |	// prescaler: 0: /1, 1: /4, 2: /16, 3: /64
		0;
	SCIF1.SCEMR = (SCIF1.SCEMR & ~ (0x80 | 0x01)) |
		0 * 0x80 |						// BGDM
		scemr_scsmr [prei].scemr |	// ABCS = 8/16 clocks per bit
		0;
	SCIF1.SCBRR = value;	/* Bit rate register */

#elif CPUSTYLE_XC7Z

	  uint32_t r; // Temporary value variable
	  r = UART1->CR;
	  r &= ~(XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN); // Clear Tx & Rx Enable
	  r |= XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS; // Tx & Rx Disable
	  UART1->CR = r;
	  const unsigned long sel_clk = xc7z_get_uart_freq();
	  const unsigned long bdiv = 8;
	  // baud_rate = sel_clk / (CD * (BDIV + 1) (ref: UG585 - TRM - Ch. 19 UART)
	  UART1->BAUDDIV = bdiv - 1; // ("BDIV")
	  UART1->BAUDGEN = calcdivround2(sel_clk, baudrate * bdiv); // ("CD")
	  // Baud Rate = 100Mhz / (124 * (6 + 1)) = 115200 bps
	  UART1->CR |= (XUARTPS_CR_TXRST | XUARTPS_CR_RXRST); // TX & RX logic reset

	  r = UART1->CR;
	  r |= XUARTPS_CR_RX_EN | XUARTPS_CR_TX_EN; // Set TX & RX enabled
	  r &= ~(XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS); // Clear TX & RX disabled
	  UART1->CR = r;

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	unsigned divisor = calcdivround2(BOARD_USART_FREQ, baudrate * 16);

	UART1->UART_LCR |= (1 << 7);
	UART1->UART_RBR_THR_DLL = divisor & 0xff;
	UART1->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART1->UART_LCR &= ~ (1 << 7);

#elif CPUSTYLE_VM14

	unsigned divisor = calcdivround2(elveesvm14_get_usart_freq(), baudrate * 16);

	while ((UART1->UART_USR & (1u << 2)) == 0)	/* TFE - FIFO передатчика пуст. */
		;
	while ((UART1->UART_USR & (1u << 0)) != 0)	/* BUSY - UART занят. */
	{
		/* todo: решить проблему с принимаемыми символами */
	}
	UART1->UART_LCR |= (1 << 7);
	UART1->UART_RBR_THR_DLL = divisor & 0xff;
	UART1->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART1->UART_LCR &= ~ (1 << 7);
	(void) UART1->UART_LCR;

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART1HW */
