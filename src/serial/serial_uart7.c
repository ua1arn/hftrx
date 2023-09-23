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
				HARDWARE_UART7_ONRXCHAR(UART7->RDR);
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
		(void) SCIF7.SCFSR;						// Перед сбросом бита RDF должно произойти его чтение в ненулевом состоянии
		SCIF7.SCFSR = (uint16_t) ~ SCIF7_SCFSR_RDF;	// RDF=0 читать незачем (в примерах странное - сбрасывабтся и другие биты)
		uint_fast8_t n = (SCIF7.SCFDR & SCIF7_SCFDR_R) >> SCIF7_SCFDR_R_SHIFT;
		while (n --)
			HARDWARE_UART7_ONRXCHAR(SCIF7.SCFRDR & SCIF7_SCFRDR_D);
	}

	// Передача символа в последовательный порт
	static void SCIFTXI7_IRQHandler(void)
	{
		HARDWARE_UART7_ONTXCHAR(& SCIF7);
	}

#else
	#error Undefined CPUSTYLE_XXX
#endif	/* CPUSTYLE_ATMEGA_XXX4 */


/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uart7_enabletx(uint_fast8_t state)
{
#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

	if (state)
		UART7->CR1 |= USART_CR1_TXEIE;
	else
		UART7->CR1 &= ~ USART_CR1_TXEIE;

#elif CPUSTYLE_ATXMEGAXXXA4

	if (state)
		USARTE7.CTRLA = (USARTE7.CTRLA & ~ USART_DREINTLVL_gm) | USART_DREINTLVL_LO_gc;
	else
		USARTE7.CTRLA = (USARTE7.CTRLA & ~ USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;

#elif CPUSTYLE_TMS320F2833X

	if (state)
		SCIBCTL2 |= (1U << 0);	// TX INT ENA
	else
		SCIBCTL2 &= ~ (1U << 0); // TX INT ENA

#elif CPUSTYLE_R7S721

	if (state)
		SCIF7.SCSCR |= (1U << 7);	// TIE Transmit Interrupt Enable
	else
		SCIF7.SCSCR &= ~ (1U << 7);	// TIE Transmit Interrupt Enable

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uart7_enablerx(uint_fast8_t state)
{
#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

	if (state)
		UART7->CR1 |= USART_CR1_RXNEIE;
	else
		UART7->CR1 &= ~ USART_CR1_RXNEIE;

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

#else
	#error Undefined CPUSTYLE_XXX
#endif
}


/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uart7_tx(void * ctx, uint_fast8_t c)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	UART7->DR = c;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32MP1

	UART7->TDR = c;

#elif CPUSTYLE_TMS320F2833X

	SCIBTXBUF = c;

#elif CPUSTYLE_R7S721

	(void) SCIF7.SCFSR;			// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
	SCIF7.SCFTDR = c;
	SCIF7.SCFSR = (uint16_t) ~ (1U << SCIF7_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

/* дождаться, когда буде все передано */
void hardware_uart7_flush(void)
{

}

/* приём символа, если готов порт */
uint_fast8_t
hardware_uart7_getchar(char * cp)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((UART7->SR & (USART_SR_RXNE | USART_SR_ORE | USART_SR_FE | USART_SR_NE)) == 0)
		return 0;
	* cp = UART7->DR;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	const uint_fast32_t isr = UART7->ISR;
	if (isr & USART_ISR_ORE)
		UART7->ICR = USART_ICR_ORECF;
	if (isr & USART_ISR_FE)
		UART7->ICR = USART_ICR_FECF;
	if ((isr & USART_ISR_RXNE_RXFNE) == 0)
		return 0;
	* cp = UART7->RDR;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	const uint_fast32_t isr = UART7->ISR;
	if (isr & USART_ISR_ORE)
		UART7->ICR = USART_ICR_ORECF;
	if (isr & USART_ISR_FE)
		UART7->ICR = USART_ICR_FECF;
	if ((isr & USART_ISR_RXNE) == 0)
		return 0;
	* cp = UART7->RDR;

#elif CPUSTYLE_TMS320F2833X

	if ((SCIBRXST & (1U << 6)) == 0)	// Wait for RXRDY bit
		return 0;
	* cp = SCIBRXBUF;

#elif CPUSTYLE_R7S721

	if ((SCIF7.SCFSR & (1U << 1)) == 0)	// RDF
		return 0;
	* cp = SCIF7.SCFRDR;
	SCIF7.SCFSR = (uint16_t) ~ (1U << 1);	// RDF=0 читать незачем (в примерах странное)

#else
	#error Undefined CPUSTYLE_XXX
#endif

	return 1;
}

/* передача символа если готов порт */
uint_fast8_t
hardware_uart7_putchar(uint_fast8_t c)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	if ((UART7->SR & USART_SR_TXE) == 0)
		return 0;
	UART7->DR = c;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	if ((UART7->ISR & USART_ISR_TXE_TXFNF) == 0)
		return 0;
	UART7->TDR = c;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	if ((UART7->ISR & USART_ISR_TXE) == 0)
		return 0;
	UART7->TDR = c;

#elif CPUSTYLE_TMS320F2833X

	if ((SCIBCTL2 & (1U << 7)) == 0)	// wait for TXRDY bit
		return 0;
	SCIBTXBUF = c;

#elif CPUSTYLE_R7S721

	if ((SCIF7.SCFSR & (1U << SCIF7_SCFSR_TDFE_SHIFT)) == 0)	// Перед сбросом бита TDFE должно произойти его чтение в ненулевом состоянии
		return 0;
	SCIF7.SCFTDR = c;
	SCIF7.SCFSR = (uint16_t) ~ (1U << SCIF7_SCFSR_TDFE_SHIFT);	// TDFE=0 читать незачем (в примерах странное)

#else
	#error Undefined CPUSTYLE_XXX
#endif

	return 1;
}

void hardware_uart7_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate)
{
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

#if WITHUARTFIFO
	UART7->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUARTFIFO */
	UART7->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUARTFIFO */

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

	UART7->CR1 = 0;

#if WITHUARTFIFO
	UART7->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUARTFIFO */
	UART7->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUARTFIFO */

	UART7->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART7_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(UART7_IRQn, UART7_IRQHandler);
	}

	UART7->CR1 |= USART_CR1_UE; // Включение USART1.

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

void
hardware_uart7_set_speed(uint_fast32_t baudrate)
{
#if CPUSTYLE_ATMEGA_XXX4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR7A |= (1U << U2X1);
	else
		UCSR7A &= ~ (1U << U2X1);

	UBRR7 = value;	/* Значение получено уже уменьшенное на 1 */


#elif CPUSTYLE_ATMEGA128

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR7A |= (1U << U2X1);
	else
		UCSR7A &= ~ (1U << U2X1);

	UBRR7H = (value >> 8) & 0xff;	/* Значение получено уже уменьшенное на 1 */
	UBRR7L = value & 0xff;

#elif CPUSTYLE_ATXMEGAXXXA4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATXMEGA_UBR_WIDTH, ATXMEGA_UBR_TAPS, & value, 1);
	if (prei == 0)
		USARTE7.CTRLB |= USART_CLK2X_bm;
	else
		USARTE7.CTRLB &= ~USART_CLK2X_bm;
	// todo: проверить требование к порядку обращения к портам
	USARTE7.BAUDCTRLA = (value & 0xff);	/* Значение получено уже уменьшенное на 1 */
	USARTE7.BAUDCTRLB = (ATXMEGA_UBR_BSEL << 4) | ((value >> 8) & 0x0f);

#elif CPUSTYLE_STM32MP1

	// uart7
	UART7->BRR = calcdivround2(BOARD_UART7_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_STM32F

	// uart7 on apb1
	USART7->BRR = calcdivround2(BOARD_UART7_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_TMS320F2833X

	const unsigned long lspclk = CPU_FREQ / 4;
	const unsigned long brr = (lspclk / 8) / baudrate;	// @ CPU_FREQ = 100 MHz, 9600 can not be programmed

	SCIBHBAUD = (brr - 1) >> 8;		// write 8 bits, not 16
	SCIBLBAUD = (brr - 1) >> 0;

#elif CPUSTYLE_R7S721

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(baudrate), R7S721_SCIF_SCBRR_WIDTH, R7S721_SCIF_SCBRR_TAPS, & value, 1);

	SCIF7.SCSMR = (SCIF7.SCSMR & ~ 0x03) |
		scemr_scsmr [prei].scsmr |	// prescaler: 0: /1, 1: /4, 2: /16, 3: /64
		0;
	SCIF7.SCEMR = (SCIF7.SCEMR & ~ (0x80 | 0x01)) |
		0 * 0x80 |						// BGDM
		scemr_scsmr [prei].scemr |	// ABCS = 8/16 clocks per bit
		0;
	SCIF7.SCBRR = value;	/* Bit rate register */

#else
	#warning Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART7HW */
