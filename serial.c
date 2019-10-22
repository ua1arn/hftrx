/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "keyboard.h"

#include <string.h>
#include <math.h>

#include "board.h"
#include "audio.h"

#include "formats.h"	// for debug prints
#include "inc/spi.h"

#if WITHUART1HW 

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	void RAMFUNC_NONILINE USART0_Handler(void) 
	{
		const uint_fast32_t csr = USART0->US_CSR;

		if (csr & US_CSR_RXRDY)
			HARDWARE_UART1_ONRXCHAR(USART0->US_RHR);
		if (csr & US_CSR_TXRDY)
			HARDWARE_UART1_ONTXCHAR(USART0);
	}

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

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

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX

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

	#if HARDWARE_ARM_USEUSART0
		static RAMFUNC_NONILINE void AT91F_US0Handler(void) 
		{
			const uint_fast32_t csr = AT91C_BASE_US0->US_CSR;

			if (csr & AT91C_US_RXRDY)
				HARDWARE_UART1_ONRXCHAR(AT91C_BASE_US0->US_RHR);
			if (csr & AT91C_US_TXRDY)
				HARDWARE_UART1_ONTXCHAR(AT91C_BASE_US0);
		}
	#elif HARDWARE_ARM_USEUSART1
		static RAMFUNC_NONILINE void AT91F_US1Handler(void) 
		{
			const uint_fast32_t csr = AT91C_BASE_US1->US_CSR;

			if (csr & AT91C_US_RXRDY)
				HARDWARE_UART1_ONRXCHAR(AT91C_BASE_US1->US_RHR);
			if (csr & AT91C_US_TXRDY)
				HARDWARE_UART1_ONTXCHAR(AT91C_BASE_US1);
		}
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif		/* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_ATMEGA328

	ISR(USART_RX_vect)
	{
		HARDWARE_UART1_ONRXCHAR(UDR0);
	}

	ISR(USART_TX_vect)
	{
		HARDWARE_UART1_ONTXCHAR(NULL);
	}

#elif CPUSTYLE_ATMEGA_XXX4

	ISR(USART0_RX_vect)
	{
		HARDWARE_UART1_ONRXCHAR(UDR0);
	}

	ISR(USART0_TX_vect)
	{
		HARDWARE_UART1_ONTXCHAR(NULL);
	}

#elif CPUSTYLE_ATMEGA32

	ISR(USART_RXC_vect)
	{
		HARDWARE_UART1_ONRXCHAR(UDR);
	}

	ISR(USART_TXC_vect)
	{
		HARDWARE_UART1_ONTXCHAR(NULL);
	}

#elif CPUSTYLE_ATMEGA128

	ISR(USART0_RX_vect)
	{
		HARDWARE_UART1_ONRXCHAR(UDR0);
	}

	ISR(USART0_TX_vect)
	{
		HARDWARE_UART1_ONTXCHAR(NULL);
	}

#elif CPUSTYLE_ATXMEGAXXXA4
	
	ISR(USARTE0_RXC_vect)
	{
		HARDWARE_UART1_ONRXCHAR(USARTE0.DATA);
	}

	ISR(USARTE0_DRE_vect)
	{
		HARDWARE_UART1_ONTXCHAR(& USARTE0);
	}

#elif CPUSTYLE_R7S721

	// Приём символа он последовательного порта
	static RAMFUNC_NONILINE void r7s721_scifrxi0_interrupt(void)
	{
		(void) SCIF0.SCFSR;						// Перед сбросом бита RDF должно произойти его чтение в ненулевом состоянии
		SCIF0.SCFSR = (uint16_t) ~ SCIF0_SCFSR_RDF;	// RDF=0 читать незачем (в примерах странное - сбрасывабтся и другие биты)
		uint_fast8_t n = (SCIF0.SCFDR & SCIF0_SCFDR_R) >> SCIF0_SCFDR_R_SHIFT;
		while (n --)
			HARDWARE_UART1_ONRXCHAR(SCIF0.SCFRDR & SCIF0_SCFRDR_D);
	}

	// Передача символа в последовательный порт
	static RAMFUNC_NONILINE void r7s721_sciftxi0_interrupt(void)
	{
		HARDWARE_UART1_ONTXCHAR(& SCIF0);
	}

#else

	#error Undefined CPUSTYLE_XXX

#endif	/* CPUSTYLE_ATMEGA_XXX4 */
	

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/*
		disableIRQ();
		enableIRQ();
*/
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uart1_enabletx(uint_fast8_t state)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0

		if (state)
			USART0->US_IER = US_IER_TXRDY;
		else
			USART0->US_IDR = US_IDR_TXRDY;

	#elif HARDWARE_ARM_USEUSART1

		if (state)
			USART1->US_IER = US_IER_TXRDY;
		else
			USART1->US_IDR = US_IDR_TXRDY;

	#elif HARDWARE_ARM_USEUART0

		if (state)
			UART0->UART_IER = UART_IER_TXRDY;
		else
			UART0->UART_IDR = UART_IDR_TXRDY;

	#elif HARDWARE_ARM_USEUART1

		if (state)
			UART1->UART_IER = UART_IER_TXRDY;
		else
			UART1->UART_IDR = UART_IDR_TXRDY;

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_STM32

	if (state)
		USART1->CR1 |= USART_CR1_TXEIE;
	else
		USART1->CR1 &= ~ USART_CR1_TXEIE;

#elif CPUSTYLE_AT91SAM7S

	#if HARDWARE_ARM_USEUSART0

	if (state)
		AT91C_BASE_US0->US_IER = AT91C_US_TXRDY;
	else
		AT91C_BASE_US0->US_IDR = AT91C_US_TXRDY;

	#elif HARDWARE_ARM_USEUSART1

	if (state)
		AT91C_BASE_US1->US_IER = AT91C_US_TXRDY;
	else
		AT91C_BASE_US1->US_IDR = AT91C_US_TXRDY;

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_ATMEGA_XXX4

	/* Used USART 0 */
	if (state)
	{
		UCSR0B |= (1U << TXCIE0);
		HARDWARE_UART1_ONTXCHAR(NULL);	// initiate 1-st character sending
	}
	else
	{
		UCSR0B &= ~ (1U << TXCIE0);
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

	/* Used USART 0 */
	if (state)
	{
		UCSR0B |= (1U << TXCIE0);
		HARDWARE_UART1_ONTXCHAR(NULL);	// initiate 1-st character sending
	}
	else
	{
		UCSR0B &= ~ (1U << TXCIE0);
	}

#elif CPUSTYLE_ATXMEGAXXXA4

	if (state)
		USARTE0.CTRLA = (USARTE0.CTRLA & ~ USART_DREINTLVL_gm) | USART_DREINTLVL_LO_gc;
	else
		USARTE0.CTRLA = (USARTE0.CTRLA & ~ USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;

#elif CPUSTYPE_TMS320F2833X

	if (state)
		SCIACTL2 |= (1U << 0);	// TX INT ENA
	else
		SCIACTL2 &= ~ (1U << 0); // TX INT ENA

#elif CPUSTYLE_R7S721

	if (state)
		SCIF0.SCSCR |= SCIF0_SCSCR_TIE;	// TIE Transmit Interrupt Enable
	else
		SCIF0.SCSCR &= ~ SCIF0_SCSCR_TIE;	// TIE Transmit Interrupt Enable

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uart1_enablerx(uint_fast8_t state)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0

		if (state)
			USART0->US_IER = US_IER_RXRDY;
		else
			USART0->US_IDR = US_IDR_RXRDY;

	#elif HARDWARE_ARM_USEUSART1

		if (state)
			USART1->US_IER = US_IER_RXRDY;
		else
			USART1->US_IDR = US_IDR_RXRDY;

	#elif HARDWARE_ARM_USEUART0

		if (state)
			UART0->UART_IER = UART_IER_RXRDY;
		else
			UART0->UART_IDR = UART_IDR_RXRDY;

	#elif HARDWARE_ARM_USEUART1

		if (state)
			UART1->UART_IER = UART_IER_RXRDY;
		else
			UART1->UART_IDR = UART_IDR_RXRDY;

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_AT91SAM7S

	#if HARDWARE_ARM_USEUSART0

	if (state)
		AT91C_BASE_US0->US_IER = AT91C_US_RXRDY;
	else
		AT91C_BASE_US0->US_IDR = AT91C_US_RXRDY;

	#elif HARDWARE_ARM_USEUSART1

	if (state)
		AT91C_BASE_US1->US_IER = AT91C_US_RXRDY;
	else
		AT91C_BASE_US1->US_IDR = AT91C_US_RXRDY;

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */


#elif CPUSTYLE_ATMEGA_XXX4
	/* Used USART 0 */
	if (state)
		UCSR0B |= (1U << RXCIE0);
	else
		UCSR0B &= ~ (1U << RXCIE0);

#elif CPUSTYLE_ATMEGA32

	if (state)
		UCSRB |= (1U << RXCIE);
	else
		UCSRB &= ~ (1U << RXCIE);

#elif CPUSTYLE_ATMEGA128

	/* Used USART 0 */
	if (state)
		UCSR0B |= (1U << RXCIE0);
	else
		UCSR0B &= ~ (1U << RXCIE0);

#elif CPUSTYLE_ATXMEGAXXXA4

	if (state)
		USARTE0.CTRLA = (USARTE0.CTRLA & ~ USART_RXCINTLVL_gm) | USART_RXCINTLVL_LO_gc;
	else
		USARTE0.CTRLA = (USARTE0.CTRLA & ~ USART_RXCINTLVL_gm) | USART_RXCINTLVL_OFF_gc;

#elif CPUSTYLE_STM32

	if (state)
		USART1->CR1 |= USART_CR1_RXNEIE;
	else
		USART1->CR1 &= ~ USART_CR1_RXNEIE;

#elif CPUSTYPE_TMS320F2833X

	if (state)
		SCIACTL2 |= (1U << 1);	// RX/BK INT ENA
	else
		SCIACTL2 &= ~ (1U << 1); // RX/BK INT ENA

#elif CPUSTYLE_R7S721

	if (state)
		SCIF0.SCSCR |= SCIF0_SCSCR_RIE;	// RIE Receive Interrupt Enable
	else
		SCIF0.SCSCR &= ~ SCIF0_SCSCR_RIE;	// RIE Receive Interrupt Enable

#else
	#error Undefined CPUSTYLE_XXX

#endif
}


/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uart1_tx(void * ctx, uint_fast8_t c)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		USART0->US_THR = c;
	#elif HARDWARE_ARM_USEUART0
		UART0->UART_THR = c;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	#if HARDWARE_ARM_USEUSART0
		AT91C_BASE_US0->US_THR = c;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_ATMEGA128
	UDR0 = c;

#elif CPUSTYLE_ATMEGA_XXX4

	UDR0 = c;

#elif CPUSTYLE_ATMEGA32

	UDR = c;

#elif CPUSTYLE_ATXMEGAXXXA4

	USARTE0.DATA = c;

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	USART1->DR = c;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX

	USART1->TDR = c;

#elif CPUSTYPE_TMS320F2833X

	SCIATXBUF = c;

#elif CPUSTYLE_R7S721

	(void) SCIF0.SCFSR;			// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
	SCIF0.SCFTDR = c;
	SCIF0.SCFSR = (uint16_t) ~ (1U << SCIF0_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#else
	#error Undefined CPUSTYLE_XXX
#endif
}



/* приём символа, если готов порт */
uint_fast8_t 
hardware_usart1_getchar(char * cp)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		if ((USART0->US_CSR & US_CSR_RXRDY) == 0)
			return 0;
		* cp = USART0->US_RHR;
	#elif HARDWARE_ARM_USEUSART1
		if ((USART1->US_CSR & US_CSR_RXRDY) == 0)
			return 0;
		* cp = USART1->US_RHR;
	#elif HARDWARE_ARM_USEUART0
		if ((UART0->UART_SR & UART_SR_RXRDY) == 0)
			return 0;
		* cp = UART0->UART_RHR;
	#elif HARDWARE_ARM_USEUART1
		if ((UART1->UART_SR & UART_SR_RXRDY) == 0)
			return 0;
		* cp = UART1->UART_RHR;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	#if HARDWARE_ARM_USEUSART0
		if ((AT91C_BASE_US0->US_CSR & AT91C_US_RXRDY) == 0)
			return 0;
		* cp = AT91C_BASE_US0->US_RHR;
	#elif HARDWARE_ARM_USEUSART1
		if ((AT91C_BASE_US1->US_CSR & AT91C_US_RXRDY) == 0)
			return 0;
		* cp = AT91C_BASE_US1->US_RHR;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_ATXMEGAXXXA4

	if ((USARTE0.STATUS & (1 << USART_RXCIF_bp)) == 0)
			return 0;
	* cp = USARTE0.DATA;

#elif CPUSTYLE_ATMEGA128

	if ((UCSR0A & (1 << RXC0)) == 0)
			return 0;
	* cp = UDR0;

#elif CPUSTYLE_ATMEGA_XXX4

	if ((UCSR0A & (1 << RXC0)) == 0)
			return 0;
	* cp = UDR0;

#elif CPUSTYLE_ATMEGA32

	if ((UCSRA & (1 << RXC)) == 0)
			return 0;
	* cp = UDR;

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((USART1->SR & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE)) == 0)
		return 0;
	* cp = USART1->DR;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX
	const uint_fast32_t isr = USART1->ISR;
	if (isr & USART_ISR_ORE)
		USART1->ICR = USART_ICR_ORECF;
	if (isr & USART_ISR_FE)
		USART1->ICR = USART_ICR_FECF;
	if ((isr & USART_ISR_RXNE) == 0)
		return 0;
	* cp = USART1->RDR;

#elif CPUSTYPE_TMS320F2833X

	if ((SCIARXST & (1U << 6)) == 0)	// Wait for RXRDY bit
		return 0;
	* cp = SCIARXBUF;

#elif CPUSTYLE_R7S721

	if ((SCIF0.SCFSR & (1U << 1)) == 0)	// RDF
		return 0;
	* cp = SCIF0.SCFRDR;
	SCIF0.SCFSR = (uint16_t) ~ (1U << 1);	// RDF=0 читать незачем (в примерах странное)

#else
	#error Undefined CPUSTYLE_XXX
#endif
	return 1;
}

/* передача символа если готов порт */
uint_fast8_t 
hardware_usart1_putchar(uint_fast8_t c)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		if ((USART0->US_CSR & US_CSR_TXRDY) == 0)
			return 0;
		USART0->US_THR = c;
	#elif HARDWARE_ARM_USEUSART1
		if ((USART1->US_CSR & US_CSR_TXRDY) == 0)
			return 0;
		USART1->US_THR = c;
	#elif HARDWARE_ARM_USEUART0
		if ((UART0->UART_SR & UART_SR_TXRDY) == 0)
			return 0;
		UART0->UART_THR = c;
	#elif HARDWARE_ARM_USEUART1
		if ((UART1->UART_SR & UART_SR_TXRDY) == 0)
			return 0;
		UART1->UART_THR = c;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	#if HARDWARE_ARM_USEUSART0
		if ((AT91C_BASE_US0->US_CSR & AT91C_US_TXRDY) == 0)
			return 0;
		AT91C_BASE_US0->US_THR = c;
	#elif HARDWARE_ARM_USEUSART1
		if ((AT91C_BASE_US1->US_CSR & AT91C_US_TXRDY) == 0)
			return 0;
		AT91C_BASE_US1->US_THR = c;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_ATXMEGAXXXA4

	if ((USARTE0.STATUS & USART_DREIF_bm) == 0)
		return 0;
	USARTE0.DATA = c;

#elif CPUSTYLE_ATMEGA_XXX4

	if ((UCSR0A & (1 << UDRE0)) == 0)
		return 0;
	UDR0 = c;

#elif CPUSTYLE_ATMEGA128

	if ((UCSR0A & (1 << UDRE0)) == 0)
		return 0;
	UDR0 = c;

#elif CPUSTYLE_ATMEGA32

	if ((UCSRA & (1 << UDRE)) == 0)
		return 0;
	UDR = c;

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((USART1->SR & USART_SR_TXE) == 0)
		return 0;
	USART1->DR = c;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX

	if ((USART1->ISR & USART_ISR_TXE) == 0)
		return 0;
	USART1->TDR = c;

#elif CPUSTYPE_TMS320F2833X

	if ((SCIACTL2 & (1U << 7)) == 0)	// wait for TXRDY bit
		return 0;
	SCIATXBUF = c;

#elif CPUSTYLE_R7S721

	if ((SCIF0.SCFSR & (1U << SCIF0_SCFSR_TDFE_SHIFT)) == 0)	// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
		return 0;
	SCIF0.SCFTDR = c;
	SCIF0.SCFSR = (uint16_t) ~ (1U << SCIF0_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#else
	#error Undefined CPUSTYLE_XXX
#endif
	return 1;
}

void hardware_uart1_initialize(uint_fast8_t debug)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		// enable the clock of USART0
		PMC->PMC_PCER0 = 1UL << ID_USART0;

		HARDWARE_USART1_INITIALIZE();	/* Присоединить периферию к выводам */
		
		// reset the UART
		USART0->US_CR = US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS;
		// set serial line mode
		USART0->US_MR = 
			US_MR_CHMODE_NORMAL |// Normal Mode
			US_MR_USCLKS_MCK |   // Clock = MCK
			US_MR_CHRL_8_BIT |
			US_MR_PAR_NO |
			US_MR_NBSTOP_1_BIT;                   

		USART0->US_IDR = (US_IDR_RXRDY | US_IDR_TXRDY);

		if (debug == 0)
		{
			NVIC_SetVector(USART0_IRQn, (uintptr_t) & USART0_IRQHandler);
			NVIC_SetPriority(USART0_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(USART0_IRQn);		// enable USART0_Handler();
		}

		USART0->US_CR = US_CR_RXEN | US_CR_TXEN;	// разрешаем приёмник и передатчик.

	#elif HARDWARE_ARM_USEUSART1
		// enable the clock of USART1
		PMC->PMC_PCER0 = 1UL << ID_USART1;

		HARDWARE_USART1_INITIALIZE();	/* Присоединить периферию к выводам */

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
			NVIC_SetVector(USART1_IRQn, (uintptr_t) & USART1_IRQHandler);
			NVIC_SetPriority(USART1_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(USART1_IRQn);		// enable USART1_Handler();
		}

		USART1->US_CR = US_CR_RXEN | US_CR_TXEN;	// разрешаем приёмник и передатчик.

	#elif HARDWARE_ARM_USEUART0
		// enable the clock of UART0
		PMC->PMC_PCER0 = 1UL << ID_UART0;

		HARDWARE_USART1_INITIALIZE();	/* Присоединить периферию к выводам */
		
		// reset the UART
		UART0->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;
		// set serial line mode
		UART0->UART_MR = 
			UART_MR_CHMODE_NORMAL |// Normal Mode
			//UART_MR_USCLKS_MCK |   // Clock = MCK
			//UART_MR_CHRL_8_BIT |
			UART_MR_PAR_NO |
			//UART_MR_NBSTOP_1_BIT |
			0;
									
		UART0->UART_IDR = (UART_IDR_RXRDY | UART_IDR_TXRDY);

		if (debug == 0)
		{
			NVIC_SetVector(UART0_IRQn, (uintptr_t) & UART0_IRQHandler);
			NVIC_SetPriority(UART0_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(UART0_IRQn);		// enable UART0_Handler();
		}

		UART0->UART_CR = UART_CR_RXEN | UART_CR_TXEN;	// разрешаем приёмник и передатчик.

	#elif HARDWARE_ARM_USEUART1
		// enable the clock of UART1
		PMC->PMC_PCER0 = 1UL << ID_UART1;

		HARDWARE_USART1_INITIALIZE();	/* Присоединить периферию к выводам */

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
			NVIC_SetVector(UART1_IRQn, (uintptr_t) & UART1_IRQHandler);
			NVIC_SetPriority(UART1_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(UART1_IRQn);		// enable UART1_Handler();
		}

		UART1->UART_CR = UART_CR_RXEN | UART_CR_TXEN;	// разрешаем приёмник и передатчик.

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */


#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Включение тактирования USART1.
	__DSB();

	USART1->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables


	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     //включить тактирование альтернативных функций
	__DSB();

	HARDWARE_USART1_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		NVIC_SetVector(USART1_IRQn, (uintptr_t) & USART1_IRQHandler);
		NVIC_SetPriority(USART1_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(USART1_IRQn);		// enable USART1_IRQHandler();
	}

	USART1->CR1 |= USART_CR1_UE; // Включение USART1.

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Включение тактирования USART1.
	__DSB();

	USART1->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_USART1_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		NVIC_SetVector(USART1_IRQn, (uintptr_t) & USART1_IRQHandler);
		NVIC_SetPriority(USART1_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(USART1_IRQn);		// enable USART1_IRQHandler();
	}

	USART1->CR1 |= USART_CR1_UE; // Включение USART1.

#elif CPUSTYLE_AT91SAM7S

	#if HARDWARE_ARM_USEUSART0
		// enable the clock of USART0
		AT91C_BASE_PMC->PMC_PCER = 1UL << AT91C_ID_US0;

		HARDWARE_USART1_INITIALIZE();	/* Присоединить периферию к выводам */

		// disable I/O pullup
		// reset the UART
		AT91C_BASE_US0->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS;
		// set serial line mode
		AT91C_BASE_US0->US_MR = 
							AT91C_US_OVER |
							AT91C_US_USMODE_NORMAL |// Normal Mode
						   AT91C_US_CLKS_CLOCK |   // Clock = MCK
						   AT91C_US_CHRL_8_BITS |
						   AT91C_US_PAR_NONE |
						   AT91C_US_NBSTOP_1_BIT;                   
		AT91C_BASE_US0->US_IDR = (AT91C_US_RXRDY | AT91C_US_TXRDY);

		{
			enum { irqID = AT91C_ID_US0 };
			// programming interrupts from ADC
			AT91C_BASE_AIC->AIC_IDCR = (1UL << irqID);
			AT91C_BASE_AIC->AIC_SVR [irqID] = (AT91_REG) AT91F_US0Handler;
			AT91C_BASE_AIC->AIC_SMR [irqID] = 
				(AT91C_AIC_SRCTYPE & AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL) |
				(AT91C_AIC_PRIOR & AT91C_AIC_PRIOR_LOWEST);
			AT91C_BASE_AIC->AIC_ICCR = (1UL << irqID);		// clear pending interrupt
			AT91C_BASE_AIC->AIC_IECR = (1UL << irqID);	// enable inerrupt

		}

		AT91C_BASE_US0->US_CR = AT91C_US_RXEN  | AT91C_US_TXEN;	// разрешаем приёмник и передатчик.

	#elif HARDWARE_ARM_USEUSART1
		// enable the clock of USART1
		AT91C_BASE_PMC->PMC_PCER = 1UL << AT91C_ID_US1;

		HARDWARE_USART1_INITIALIZE();	/* Присоединить периферию к выводам */

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

		{
			enum { irqID = AT91C_ID_US1 };
			// programming interrupts from ADC
			AT91C_BASE_AIC->AIC_IDCR = (1UL << irqID);
			AT91C_BASE_AIC->AIC_SVR [irqID] = (AT91_REG) AT91F_US1Handler;
			AT91C_BASE_AIC->AIC_SMR [irqID] = 
				(AT91C_AIC_SRCTYPE & AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL) |
				(AT91C_AIC_PRIOR & AT91C_AIC_PRIOR_LOWEST);
			AT91C_BASE_AIC->AIC_ICCR = (1UL << irqID);		// clear pending interrupt
			AT91C_BASE_AIC->AIC_IECR = (1UL << irqID);	// enable inerrupt
		}

		AT91C_BASE_US1->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;	// разрешаем приёмник и передатчик.

	#else	/* HARDWARE_ARM_USExxx */

		#error Wrong HARDWARE_ARM_USExxx value

	#endif /* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_ATMEGA_XXX4

	// USART initialization
	UCSR0B = (1U << RXEN0) | (1U << TXEN0) /* | (1U << UCSZ02) */;
	UCSR0C = (1U << UCSZ01) | (1U << UCSZ00);	// asynchronious mode, 8 bit.
	// enable pull-up registers for RXD and TXD pins: then rx or tx disabled, these pins disconnected fron UART
	PORTD |= ((1U << PD0) | (1U << PD1));
#if defined (DDRD0) && defined (DDRD1)
	DDRD &= ~ ((1U << DDRD0) | (1U << DDRD1));
#else
	DDRD &= ~ ((1U << DDD0) | (1U << DDD1));
#endif

#elif CPUSTYLE_ATMEGA128

	// USART initialization
	UCSR0B = (1U << RXEN0) | (1U << TXEN0) /* | (1U << UCSZ02) */;
	UCSR0C = (1U << UCSZ01) | (1U << UCSZ00);	// asynchronious mode, 8 bit.
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

	USARTE0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;
	USARTE0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;

#elif CPUSTYPE_TMS320F2833X

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
			(1UL << (35 % 32)) |	// SCITXDA
			(1UL << (36 % 32)),	// SCIRXDA
			1	// mux = 1
			);

	SCIACTL1 |= (1U << 0) |	// RX enable
				(1U << 1);	// TX enable

#elif CPUSTYLE_R7S721

    /* ---- Supply clock to the SCIF(channel 0) ---- */
	CPG.STBCR4 &= ~ CPG_STBCR4_BIT_MSTP47;	// Module Stop 47 SCIF0
	(void) CPG.STBCR4;			/* Dummy read */

	SCIF0.SCSCR = 0x0000;	/* SCIF transmitting and receiving operations stop, internal clock */

	SCIF0.SCSCR = (SCIF0.SCSCR & ~ 0x03) |	
		0x00 |						// internal clock
		0;

	/* ---- Serial status register(SCFSR2) setting ---- */
	/* ER,BRK,DR bit clear */
	(void) SCIF0.SCFSR;						// Перед сбросом бита xxx должно произойти его чтение в ненулевом состоянии
	SCIF0.SCFSR = ~ 0x0091;	// 0xFF6E;

	/* ---- Line status register (SCLSR2) setting ---- */
	/* ORER bit clear */
	//SCIF0.SCLSR.BIT.ORER  = 0;
	SCIF0.SCLSR &= ~ 0x0001;

	SCIF0.SCSMR = 
		0x00 |	/* 8-N-1 format */
		0;

	/* ---- FIFO control register (SCFCR2) setting ---- */
	/*  RTS output active trigger        :Initial value	*/
	/*  Receive FIFO data trigger        :1-data		*/
	/*  Transmit FIFO data trigger       :0-data		*/
	/*  Modem control enable             :Disabled		*/
	/*  Receive FIFO data register reset :Disabled		*/
	/*  Loop-back test                   :Disabled 		*/
	SCIF0.SCFCR = 0x0030;

	/* ---- Serial port register (SCSPTR2) setting ---- */
	/* Serial port  break output(SPB2IO)  1: Enabled */
	/* Serial port break data(SPB2DT)  1: High-level */
	//SCIF0.SCSPTR |= 0x0003;

	{
		const IRQn_ID_t int_id = SCIFRXI0_IRQn;
		IRQ_SetHandler(int_id, r7s721_scifrxi0_interrupt);
		IRQ_SetPriority(int_id, ARM_SYSTEM_PRIORITY);
		IRQ_Enable(int_id);
	}
	{
		const IRQn_ID_t int_id = SCIFTXI0_IRQn;
		IRQ_SetHandler(int_id, r7s721_sciftxi0_interrupt);
		IRQ_SetPriority(int_id, ARM_SYSTEM_PRIORITY);
		IRQ_Enable(int_id);
	}

	HARDWARE_USART1_INITIALIZE();	/* Присоединить периферию к выводам */

	SCIF0.SCSCR |= 0x0030;	// TE RE - SCIF0 transmitting and receiving operations are enabled */

#else

	#error Undefined CPUSTYLE_XXX

#endif

}

#endif /* WITHUART1HW */

#if WITHUART2HW 

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	void RAMFUNC_NONILINE USART1_Handler(void) 
	{
		const uint_fast32_t csr = USART1->US_CSR;

		if (csr & US_CSR_RXRDY)
			HARDWARE_UART2_ONRXCHAR(USART1->US_RHR);
		if (csr & US_CSR_TXRDY)
			HARDWARE_UART2_ONTXCHAR(USART1);
	}

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	void RAMFUNC_NONILINE USART2_IRQHandler(void)
	{
		const uint_fast32_t sr = USART2->SR;

		if (sr & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE))
			HARDWARE_UART2_ONRXCHAR(USART2->DR);
		if (sr & (USART_SR_ORE | USART_SR_FE | USART_SR_NE))
			HARDWARE_UART2_ONOVERFLOW();
		if (sr & USART_SR_TXE)
			HARDWARE_UART2_ONTXCHAR(USART2);
	}

#elif CPUSTYLE_STM32H7XX

	void RAMFUNC_NONILINE USART2_IRQHandler(void)
	{
		const uint_fast32_t isr = USART2->ISR;

		if (isr & USART_ISR_RXNE_RXFNE)
			HARDWARE_UART2_ONRXCHAR(USART2->RDR);
		if (isr & USART_ISR_ORE)
		{
			USART2->ICR = USART_ICR_ORECF;
			HARDWARE_UART2_ONOVERFLOW();
		}
		if (isr & USART_ISR_FE)
			USART2->ICR = USART_ICR_FECF;
		if (isr & USART_ISR_TXE_TXFNF)
			HARDWARE_UART2_ONTXCHAR(USART2);
	}

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	void RAMFUNC_NONILINE USART2_IRQHandler(void)
	{
		const uint_fast32_t isr = USART2->ISR;

		if (isr & USART_ISR_RXNE)
			HARDWARE_UART2_ONRXCHAR(USART2->RDR);
		if (isr & USART_ISR_ORE)
		{
			USART2->ICR = USART_ICR_ORECF;
			HARDWARE_UART2_ONOVERFLOW();
		}
		if (isr & USART_ISR_FE)
			USART2->ICR = USART_ICR_FECF;
		if (isr & USART_ISR_TXE)
			HARDWARE_UART2_ONTXCHAR(USART2);
	}

#elif CPUSTYLE_AT91SAM7S

	#if HARDWARE_ARM_USEUSART0
		static RAMFUNC_NONILINE void AT91F_US0Handler(void) 
		{
			const uint_fast32_t csr = AT91C_BASE_US0->US_CSR;

			if (csr & AT91C_US_RXRDY)
				HARDWARE_UART2_ONRXCHAR(AT91C_BASE_US0->US_RHR);
			if (csr & AT91C_US_TXRDY)
				HARDWARE_UART2_ONTXCHAR(AT91C_BASE_US0);
		}
	#elif HARDWARE_ARM_USEUSART1
		static RAMFUNC_NONILINE void AT91F_US1Handler(void) 
		{
			const uint_fast32_t csr = AT91C_BASE_US1->US_CSR;

			if (csr & AT91C_US_RXRDY)
				HARDWARE_UART2_ONRXCHAR(AT91C_BASE_US1->US_RHR);
			if (csr & AT91C_US_TXRDY)
				HARDWARE_UART2_ONTXCHAR(AT91C_BASE_US1);
		}
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif		/* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_ATMEGA328

	ISR(USART_RX_vect)
	{
		HARDWARE_UART2_ONRXCHAR(UDR0);
	}

	ISR(USART_TX_vect)
	{
		HARDWARE_UART2_ONTXCHAR(NULL);
	}

#elif CPUSTYLE_ATMEGA_XXX4

	ISR(USART1_RX_vect)
	{
		HARDWARE_UART2_ONRXCHAR(UDR0);
	}

	ISR(USART1_TX_vect)
	{
		HARDWARE_UART2_ONTXCHAR(NULL);
	}

#elif CPUSTYLE_ATMEGA32

	ISR(USART_RXC_vect)
	{
		HARDWARE_UART2_ONRXCHAR(UDR);
	}

	ISR(USART_TXC_vect)
	{
		HARDWARE_UART2_ONTXCHAR(NULL);
	}

#elif CPUSTYLE_ATMEGA128

	ISR(USART1_RX_vect)
	{
		HARDWARE_UART2_ONRXCHAR(UDR1);
	}

	ISR(USART1_TX_vect)
	{
		HARDWARE_UART2_ONTXCHAR(NULL);
	}

#elif CPUSTYLE_ATXMEGAXXXA4
	
	ISR(USARTE1_RXC_vect)
	{
		HARDWARE_UART2_ONRXCHAR(USARTE1.DATA);
	}

	ISR(USARTE1_DRE_vect)
	{
		HARDWARE_UART2_ONTXCHAR(& USARTE1);
	}

#elif CPUSTYLE_R7S721

	// Приём символа он последовательного порта
	static void r7s721_scifrxi3_interrupt(void)
	{
		(void) SCIF3.SCFSR;						// Перед сбросом бита RDF должно произойти его чтение в ненулевом состоянии
		SCIF3.SCFSR = (uint16_t) ~ SCIF3_SCFSR_RDF;	// RDF=0 читать незачем (в примерах странное - сбрасывабтся и другие биты)
		uint_fast8_t n = (SCIF3.SCFDR & SCIF3_SCFDR_R) >> SCIF3_SCFDR_R_SHIFT;
		while (n --)
			HARDWARE_UART2_ONRXCHAR(SCIF3.SCFRDR & SCIF3_SCFRDR_D);
	}

	// Передача символа в последовательный порт
	static void r7s721_sciftxi3_interrupt(void)
	{
		HARDWARE_UART2_ONTXCHAR(& SCIF3);
	}

#elif CPUSTYLE_STM32MP157A
#else

	#error Undefined CPUSTYLE_XXX

#endif	/* CPUSTYLE_ATMEGA_XXX4 */
	

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/*
		disableIRQ();
		enableIRQ();
*/
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uart2_enabletx(uint_fast8_t state)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0

		if (state)
			USART0->US_IER = US_IER_TXRDY;
		else
			USART0->US_IDR = US_IDR_TXRDY;

	#elif HARDWARE_ARM_USEUSART1

		if (state)
			USART1->US_IER = US_IER_TXRDY;
		else
			USART1->US_IDR = US_IDR_TXRDY;

	#elif HARDWARE_ARM_USEUART0

		if (state)
			UART0->UART_IER = UART_IER_TXRDY;
		else
			UART0->UART_IDR = UART_IDR_TXRDY;

	#elif HARDWARE_ARM_USEUART1

		if (state)
			UART1->UART_IER = UART_IER_TXRDY;
		else
			UART1->UART_IDR = UART_IDR_TXRDY;

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_AT91SAM7S

	#if HARDWARE_ARM_USEUSART0

	if (state)
		AT91C_BASE_US0->US_IER = AT91C_US_TXRDY;
	else
		AT91C_BASE_US0->US_IDR = AT91C_US_TXRDY;

	#elif HARDWARE_ARM_USEUSART1

	if (state)
		AT91C_BASE_US1->US_IER = AT91C_US_TXRDY;
	else
		AT91C_BASE_US1->US_IDR = AT91C_US_TXRDY;

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_STM32

	if (state)
		USART2->CR1 |= USART_CR1_TXEIE;
	else
		USART2->CR1 &= ~ USART_CR1_TXEIE;

#elif CPUSTYLE_ATMEGA_XXX4

	/* Used USART 1 */
	if (state)
	{
		UCSR1B |= (1U << TXCIE1);
		HARDWARE_UART2_ONTXCHAR(NULL);	// initiate 1-st character sending
	}
	else
	{
		UCSR1B &= ~ (1U << TXCIE1);
	}

#elif CPUSTYLE_ATMEGA32

	#error WITHUART2HW with CPUSTYLE_ATMEGA32 not supported
	if (state)
	{
		UCSRB |= (1U << TXCIE);
		HARDWARE_UART2_ONTXCHAR(NULL);	// initiate 1-st character sending
	}
	else
	{
		UCSRB &= ~ (1U << TXCIE);
	}

#elif CPUSTYLE_ATMEGA128

	/* Used USART 1 */
	if (state)
	{
		UCSR1B |= (1U << TXCIE1);
		HARDWARE_UART2_ONTXCHAR(NULL);	// initiate 1-st character sending
	}
	else
	{
		UCSR1B &= ~ (1U << TXCIE1);
	}

#elif CPUSTYLE_ATXMEGAXXXA4

	if (state)
		USARTE1.CTRLA = (USARTE1.CTRLA & ~ USART_DREINTLVL_gm) | USART_DREINTLVL_LO_gc;
	else
		USARTE1.CTRLA = (USARTE1.CTRLA & ~ USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;

#elif CPUSTYPE_TMS320F2833X

	if (state)
		SCIBCTL2 |= (1U << 0);	// TX INT ENA
	else
		SCIBCTL2 &= ~ (1U << 0); // TX INT ENA

#elif CPUSTYLE_R7S721

	if (state)
		SCIF3.SCSCR |= (1U << 7);	// TIE Transmit Interrupt Enable
	else
		SCIF3.SCSCR &= ~ (1U << 7);	// TIE Transmit Interrupt Enable

#elif CPUSTYLE_STM32MP157A

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uart2_enablerx(uint_fast8_t state)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0

		if (state)
			USART0->US_IER = US_IER_RXRDY;
		else
			USART0->US_IDR = US_IDR_RXRDY;

	#elif HARDWARE_ARM_USEUSART1

		if (state)
			USART1->US_IER = US_IER_RXRDY;
		else
			USART1->US_IDR = US_IDR_RXRDY;

	#elif HARDWARE_ARM_USEUART0

		if (state)
			UART0->UART_IER = UART_IER_RXRDY;
		else
			UART0->UART_IDR = UART_IDR_RXRDY;

	#elif HARDWARE_ARM_USEUART1

		if (state)
			UART1->UART_IER = UART_IER_RXRDY;
		else
			UART1->UART_IDR = UART_IDR_RXRDY;

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_AT91SAM7S

	#if HARDWARE_ARM_USEUSART1

	if (state)
		AT91C_BASE_US1->US_IER = AT91C_US_RXRDY;
	else
		AT91C_BASE_US1->US_IDR = AT91C_US_RXRDY;

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */


#elif CPUSTYLE_ATMEGA_XXX4
	/* Used USART 1 */
	if (state)
		UCSR1B |= (1U << RXCIE1);
	else
		UCSR1B &= ~ (1U << RXCIE1);

#elif CPUSTYLE_ATMEGA32

	#error WITHUART2HW with CPUSTYLE_ATMEGA32 not supported
	if (state)
		UCSRB |= (1U << RXCIE);
	else
		UCSRB &= ~ (1U << RXCIE);

#elif CPUSTYLE_ATMEGA128

	/* Used USART 0 */
	if (state)
		UCSR1B |= (1U << RXCIE1);
	else
		UCSR1B &= ~ (1U << RXCIE1);

#elif CPUSTYLE_ATXMEGAXXXA4

	if (state)
		USARTE1.CTRLA = (USARTE1.CTRLA & ~ USART_RXCINTLVL_gm) | USART_RXCINTLVL_LO_gc;
	else
		USARTE1.CTRLA = (USARTE1.CTRLA & ~ USART_RXCINTLVL_gm) | USART_RXCINTLVL_OFF_gc;

#elif CPUSTYLE_STM32

	if (state)
		USART2->CR1 |= USART_CR1_RXNEIE;
	else
		USART2->CR1 &= ~ USART_CR1_RXNEIE;

#elif CPUSTYPE_TMS320F2833X

	if (state)
		SCIBCTL2 |= (1U << 1);	// RX/BK INT ENA
	else
		SCIBCTL2 &= ~ (1U << 1); // RX/BK INT ENA

#elif CPUSTYLE_R7S721

	if (state)
		SCIF3.SCSCR |= (1U << 6);	// RIE Receive Interrupt Enable
	else
		SCIF3.SCSCR &= ~ (1U << 6);	// RIE Receive Interrupt Enable

#elif CPUSTYLE_STM32MP157A

#else
	#error Undefined CPUSTYLE_XXX

#endif
}


/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uart2_tx(void * ctx, uint_fast8_t c)
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

	#if HARDWARE_ARM_USEUSART1
		AT91C_BASE_US1->US_THR = c;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_ATMEGA128
	UDR1 = c;

#elif CPUSTYLE_ATMEGA_XXX4

	UDR1 = c;

#elif CPUSTYLE_ATMEGA32

	#error WITHUART2HW with CPUSTYLE_ATMEGA32 not supported
	UDR = c;

#elif CPUSTYLE_ATXMEGAXXXA4

	USARTE1.DATA = c;

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	USART2->DR = c;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX

	USART2->TDR = c;

#elif CPUSTYPE_TMS320F2833X

	SCIBTXBUF = c;

#elif CPUSTYLE_R7S721

	(void) SCIF3.SCFSR;			// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
	SCIF3.SCFTDR = c;
	SCIF3.SCFSR = (uint16_t) ~ (1U << SCIF3_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#elif CPUSTYLE_STM32MP157A

#else
	#error Undefined CPUSTYLE_XXX
#endif
}



/* приём символа, если готов порт */
uint_fast8_t 
hardware_usart2_getchar(char * cp)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		if ((USART0->US_CSR & US_CSR_RXRDY) == 0)
			return 0;
		* cp = USART0->US_RHR;
	#elif HARDWARE_ARM_USEUSART1
		if ((USART1->US_CSR & US_CSR_RXRDY) == 0)
			return 0;
		* cp = USART1->US_RHR;
	#elif HARDWARE_ARM_USEUART0
		if ((UART0->UART_SR & UART_SR_RXRDY) == 0)
			return 0;
		* cp = UART0->UART_RHR;
	#elif HARDWARE_ARM_USEUART1
		if ((UART1->UART_SR & UART_SR_RXRDY) == 0)
			return 0;
		* cp = UART1->UART_RHR;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	#if HARDWARE_ARM_USEUSART1
		if ((AT91C_BASE_US1->US_CSR & AT91C_US_RXRDY) == 0)
			return 0;
		* cp = AT91C_BASE_US1->US_RHR;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_ATXMEGAXXXA4

	if ((USARTE1.STATUS & (1 << USART_RXCIF_bp)) == 0)
			return 0;
	* cp = USARTE1.DATA;

#elif CPUSTYLE_ATMEGA128

	if ((UCSR1A & (1 << RXC1)) == 0)
			return 0;
	* cp = UDR1;

#elif CPUSTYLE_ATMEGA_XXX4

	if ((UCSR1A & (1 << RXC1)) == 0)
			return 0;
	* cp = UDR1;

#elif CPUSTYLE_ATMEGA32

	#error WITHUART2HW with CPUSTYLE_ATMEGA32 not supported

	if ((UCSRA & (1 << RXC)) == 0)
			return 0;
	* cp = UDR;

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((USART2->SR & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE)) == 0)
		return 0;
	* cp = USART2->DR;

#elif CPUSTYLE_STM32H7XX

	const uint_fast32_t isr = USART2->ISR;
	if (isr & USART_ISR_ORE)
		USART2->ICR = USART_ICR_ORECF;
	if (isr & USART_ISR_FE)
		USART2->ICR = USART_ICR_FECF;
	if ((isr & USART_ISR_RXNE_RXFNE) == 0)
		return 0;
	* cp = USART2->RDR;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	const uint_fast32_t isr = USART2->ISR;
	if (isr & USART_ISR_ORE)
		USART2->ICR = USART_ICR_ORECF;
	if (isr & USART_ISR_FE)
		USART2->ICR = USART_ICR_FECF;
	if ((isr & USART_ISR_RXNE) == 0)
		return 0;
	* cp = USART2->RDR;

#elif CPUSTYPE_TMS320F2833X

	if ((SCIBRXST & (1U << 6)) == 0)	// Wait for RXRDY bit
		return 0;
	* cp = SCIBRXBUF;

#elif CPUSTYLE_R7S721

	if ((SCIF3.SCFSR & (1U << 1)) == 0)	// RDF
		return 0;
	* cp = SCIF3.SCFRDR;
	SCIF3.SCFSR = (uint16_t) ~ (1U << 1);	// RDF=0 читать незачем (в примерах странное)

#elif CPUSTYLE_STM32MP157A

#else
	#error Undefined CPUSTYLE_XXX
#endif
	return 1;
}

/* передача символа если готов порт */
uint_fast8_t 
hardware_usart2_putchar(uint_fast8_t c)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		if ((USART0->US_CSR & US_CSR_TXRDY) == 0)
			return 0;
		USART0->US_THR = c;
	#elif HARDWARE_ARM_USEUSART1
		if ((USART1->US_CSR & US_CSR_TXRDY) == 0)
			return 0;
		USART1->US_THR = c;
	#elif HARDWARE_ARM_USEUART0
		if ((UART0->UART_SR & UART_SR_TXRDY) == 0)
			return 0;
		UART0->UART_THR = c;
	#elif HARDWARE_ARM_USEUART1
		if ((UART1->UART_SR & UART_SR_TXRDY) == 0)
			return 0;
		UART1->UART_THR = c;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	#if HARDWARE_ARM_USEUSART0
		if ((AT91C_BASE_US0->US_CSR & AT91C_US_TXRDY) == 0)
			return 0;
		AT91C_BASE_US0->US_THR = c;
	#elif HARDWARE_ARM_USEUSART1
		if ((AT91C_BASE_US1->US_CSR & AT91C_US_TXRDY) == 0)
			return 0;
		AT91C_BASE_US1->US_THR = c;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_ATXMEGAXXXA4

	if ((USARTE0.STATUS & USART_DREIF_bm) == 0)
		return 0;
	USARTE0.DATA = c;

#elif CPUSTYLE_ATMEGA_XXX4

	if ((UCSR0A & (1 << UDRE0)) == 0)
		return 0;
	UDR0 = c;

#elif CPUSTYLE_ATMEGA128

	if ((UCSR0A & (1 << UDRE0)) == 0)
		return 0;
	UDR0 = c;

#elif CPUSTYLE_ATMEGA32

	#error WITHUART2HW with CPUSTYLE_ATMEGA32 not supported

	if ((UCSRA & (1 << UDRE)) == 0)
		return 0;
	UDR = c;

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((USART2->SR & USART_SR_TXE) == 0)
		return 0;
	USART2->DR = c;

#elif CPUSTYLE_STM32H7XX

	if ((USART2->ISR & USART_ISR_TXE_TXFNF) == 0)
		return 0;
	USART2->TDR = c;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	if ((USART2->ISR & USART_ISR_TXE) == 0)
		return 0;
	USART2->TDR = c;

#elif CPUSTYPE_TMS320F2833X

	if ((SCIBCTL2 & (1U << 7)) == 0)	// wait for TXRDY bit
		return 0;
	SCIBTXBUF = c;

#elif CPUSTYLE_R7S721

	if ((SCIF3.SCFSR & (1U << SCIF3_SCFSR_TDFE_SHIFT)) == 0)	// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
		return 0;
	SCIF3.SCFTDR = c;
	SCIF3.SCFSR = (uint16_t) ~ (1U << SCIF3_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#elif CPUSTYLE_STM32MP157A

#else
	#error Undefined CPUSTYLE_XXX
#endif
	return 1;
}

void hardware_uart2_initialize(uint_fast8_t debug)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		// enable the clock of USART0
		PMC->PMC_PCER0 = 1UL << ID_USART0;

		HARDWARE_USART2_INITIALIZE();	/* Присоединить периферию к выводам */
		
		// reset the UART
		USART0->US_CR = US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS;
		// set serial line mode
		USART0->US_MR = 
			US_MR_CHMODE_NORMAL |// Normal Mode
			US_MR_USCLKS_MCK |   // Clock = MCK
			US_MR_CHRL_8_BIT |
			US_MR_PAR_NO |
			US_MR_NBSTOP_1_BIT;                   
									
		USART0->US_IDR = (US_IDR_RXRDY | US_IDR_TXRDY);

		if (debug == 0)
		{
			NVIC_SetVector(USART0_IRQn, (uintptr_t) & USART0_IRQHandler);
			NVIC_SetPriority(USART0_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(USART0_IRQn);		// enable USART0_Handler();
		}

		USART0->US_CR = US_CR_RXEN | US_CR_TXEN;	// разрешаем приёмник и передатчик.

	#elif HARDWARE_ARM_USEUSART1
		// enable the clock of USART1
		PMC->PMC_PCER0 = 1UL << ID_USART1;

		HARDWARE_USART2_INITIALIZE();	/* Присоединить периферию к выводам */

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
			NVIC_SetVector(USART1_IRQn, (uintptr_t) & USART1_IRQHandler);
			NVIC_SetPriority(USART1_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(USART1_IRQn);		// enable USART2_Handler();
		}

		USART1->US_CR = US_CR_RXEN | US_CR_TXEN;	// разрешаем приёмник и передатчик.

	#elif HARDWARE_ARM_USEUART0

		// enable the clock of UART0
		PMC->PMC_PCER0 = 1UL << ID_UART0;

		HARDWARE_USART2_INITIALIZE();	/* Присоединить периферию к выводам */
		
		// reset the UART
		UART0->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;
		// set serial line mode
		UART0->UART_MR = 
			UART_MR_CHMODE_NORMAL |// Normal Mode
			//UART_MR_USCLKS_MCK |   // Clock = MCK
			//UART_MR_CHRL_8_BIT |
			UART_MR_PAR_NO |
			//UART_MR_NBSTOP_1_BIT |
			0;
									
		UART0->UART_IDR = (UART_IDR_RXRDY | UART_IDR_TXRDY);

		if (debug == 0)
		{
			NVIC_SetVector(UART0_IRQn, (uintptr_t) & UART0_IRQHandler);
			NVIC_SetPriority(UART0_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(UART0_IRQn);		// enable UART0_Handler();
		}

		UART0->UART_CR = UART_CR_RXEN | UART_CR_TXEN;	// разрешаем приёмник и передатчик.

	#elif HARDWARE_ARM_USEUART1

		// enable the clock of UART1
		PMC->PMC_PCER0 = 1UL << ID_UART1;

		HARDWARE_USART2_INITIALIZE();	/* Присоединить периферию к выводам */

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
			NVIC_SetVector(UART1_IRQn, (uintptr_t) & UART1_IRQHandler);
			NVIC_SetPriority(UART1_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(UART1_IRQn);		// enable UART1_Handler();
		}

		UART1->UART_CR = UART_CR_RXEN | UART_CR_TXEN;	// разрешаем приёмник и передатчик.

	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */


#elif CPUSTYLE_STM32F1XX

	RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Включение тактирования USART2.
	__DSB();

	USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables


	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     //включить тактирование альтернативных функций
	__DSB();

	HARDWARE_USART2_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		NVIC_SetVector(USART2_IRQn, (uintptr_t) & USART2_IRQHandler);
		NVIC_SetPriority(USART2_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(USART2_IRQn);		// enable USART2_IRQHandler();
	}

	USART2->CR1 |= USART_CR1_UE; // Включение USART2.

#elif CPUSTYLE_STM32H7XX

	RCC->APB1LENR |= RCC_APB1LENR_USART2EN; // Включение тактирования USART2.
	__DSB();

	USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_USART2_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		NVIC_SetVector(USART2_IRQn, (uintptr_t) & USART2_IRQHandler);
		NVIC_SetPriority(USART2_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(USART2_IRQn);		// enable USART2_IRQHandler();
	}

	USART2->CR1 |= USART_CR1_UE; // Включение USART2.

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Включение тактирования USART2.
	__DSB();

	USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_USART2_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		NVIC_SetVector(USART2_IRQn, (uintptr_t) & USART2_IRQHandler);
		NVIC_SetPriority(USART2_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(USART2_IRQn);		// enable USART2_IRQHandler();
	}

	USART2->CR1 |= USART_CR1_UE; // Включение USART2.

#elif CPUSTYLE_AT91SAM7S

	#if HARDWARE_ARM_USEUSART0
		// enable the clock of USART0
		AT91C_BASE_PMC->PMC_PCER = 1UL << AT91C_ID_US0;

		HARDWARE_USART2_INITIALIZE();	/* Присоединить периферию к выводам */

		// disable I/O pullup
		// reset the UART
		AT91C_BASE_US0->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS;
		// set serial line mode
		AT91C_BASE_US0->US_MR = 
							AT91C_US_OVER |
							AT91C_US_USMODE_NORMAL |// Normal Mode
						   AT91C_US_CLKS_CLOCK |   // Clock = MCK
						   AT91C_US_CHRL_8_BITS |
						   AT91C_US_PAR_NONE |
						   AT91C_US_NBSTOP_1_BIT;                   
		AT91C_BASE_US0->US_IDR = (AT91C_US_RXRDY | AT91C_US_TXRDY);

		{
			enum { irqID = AT91C_ID_US0 };
			// programming interrupts from ADC
			AT91C_BASE_AIC->AIC_IDCR = (1UL << irqID);
			AT91C_BASE_AIC->AIC_SVR [irqID] = (AT91_REG) AT91F_US0Handler;
			AT91C_BASE_AIC->AIC_SMR [irqID] = 
				(AT91C_AIC_SRCTYPE & AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL) |
				(AT91C_AIC_PRIOR & AT91C_AIC_PRIOR_LOWEST);
			AT91C_BASE_AIC->AIC_ICCR = (1UL << irqID);		// clear pending interrupt
			AT91C_BASE_AIC->AIC_IECR = (1UL << irqID);	// enable inerrupt

		}

		AT91C_BASE_US0->US_CR = AT91C_US_RXEN  | AT91C_US_TXEN;	// разрешаем приёмник и передатчик.

	#elif HARDWARE_ARM_USEUSART1

		// enable the clock of USART2
		AT91C_BASE_PMC->PMC_PCER = 1UL << AT91C_ID_US1;

		HARDWARE_USART2_INITIALIZE();	/* Присоединить периферию к выводам */

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

		{
			enum { irqID = AT91C_ID_US1 };
			// programming interrupts from ADC
			AT91C_BASE_AIC->AIC_IDCR = (1UL << irqID);
			AT91C_BASE_AIC->AIC_SVR [irqID] = (AT91_REG) AT91F_US1Handler;
			AT91C_BASE_AIC->AIC_SMR [irqID] = 
				(AT91C_AIC_SRCTYPE & AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL) |
				(AT91C_AIC_PRIOR & AT91C_AIC_PRIOR_LOWEST);
			AT91C_BASE_AIC->AIC_ICCR = (1UL << irqID);		// clear pending interrupt
			AT91C_BASE_AIC->AIC_IECR = (1UL << irqID);	// enable inerrupt
		}

		AT91C_BASE_US1->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;	// разрешаем приёмник и передатчик.
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif /* HARDWARE_ARM_USEUSART0 */

#elif CPUSTYLE_ATMEGA_XXX4

	// USART initialization
	UCSR1B = (1U << RXEN1) | (1U << TXEN1) /* | (1U << UCSZ12) */;
	UCSR1C = (1U << UCSZ11) | (1U << UCSZ10);	// asynchronious mode, 8 bit.
	// enable pull-up registers for RXD and TXD pins: then rx or tx disabled, these pins disconnected fron UART
	PORTD |= ((1U << PD2) | (1U << PD3));
#if defined (DDRD2) && defined (DDRD3)
	DDRD &= ~ ((1U << DDRD2) | (1U << DDRD3));
#else
	DDRD &= ~ ((1U << DDD2) | (1U << DDD3));
#endif

#elif CPUSTYLE_ATMEGA128

	// USART initialization
	UCSR1B = (1U << RXEN1) | (1U << TXEN1) /* | (1U << UCSZ12) */;
	UCSR1C = (1U << UCSZ11) | (1U << UCSZ10);	// asynchronious mode, 8 bit.
	// enable pull-up registers for RXD and TXD pins: then rx or tx disabled, these pins disconnected fron UART
	//PORTE |= ((1U << PE0) | (1U << PE1));

#elif CPUSTYLE_ATXMEGAXXXA4

xxxx!;
	PORTE.DIRSET = PIN3_bm; // PE3 (TXD0) as output
	PORTE.DIRCLR = PIN2_bm; // PE2 (RXD0) as input
	PORTE_PIN2CTRL = (PORTE_PIN2CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc;							// pin is pulled high

	USARTE1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;
	USARTE1.CTRLB = USART_RXEN_bm | USART_TXEN_bm;


#elif CPUSTYLE_ATMEGA32

	#error WITHUART2HW with CPUSTYLE_ATMEGA not supported

#elif CPUSTYPE_TMS320F2833X
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
			(1UL << (35 % 32)) |	// SCITXDA
			(1UL << (36 % 32)),	// SCIRXDA
			1	// mux = 1
			);

	SCIBCTL1 |= (1U << 0) |	// RX enable
				(1U << 1);	// TX enable

#elif CPUSTYLE_R7S721

    /* ---- Supply clock to the SCIF(channel 1) ---- */
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

	{
		const IRQn_ID_t int_id = SCIFRXI3_IRQn;
		IRQ_SetHandler(int_id, r7s721_scifrxi3_interrupt);
		IRQ_SetPriority(int_id, ARM_SYSTEM_PRIORITY);
		IRQ_Enable(int_id);
	}
	{
		const IRQn_ID_t int_id = SCIFTXI3_IRQn;
		IRQ_SetHandler(int_id, r7s721_sciftxi3_interrupt);
		IRQ_SetPriority(int_id, ARM_SYSTEM_PRIORITY);
		IRQ_Enable(int_id);
	}

	HARDWARE_USART2_INITIALIZE();	/* Присоединить периферию к выводам */

	SCIF3.SCSCR |= 0x0030;	// TE RE - SCIF3 transmitting and receiving operations are enabled */

#elif CPUSTYLE_STM32MP157A

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART2HW */
