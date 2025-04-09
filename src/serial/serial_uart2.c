/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if WITHUART2HW

#include "serial.h"
#include "formats.h"	// for debug prints
#include "board.h"
#include "gpio.h"
#include <string.h>
#include <math.h>
#include "clocks.h"

#define thisPORT 2

#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

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

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

void RAMFUNC_NONILINE USART2_IRQHandler(void)
{
	const uint_fast32_t isr = USART2->ISR;
	const uint_fast32_t cr1 = USART2->CR1;

	if (cr1 & USART_CR1_RXNEIE)
	{
		if (isr & USART_ISR_RXNE_RXFNE)
		{
			const uint_fast8_t c = USART2->RDR;
			HARDWARE_UART2_ONRXCHAR(c);
		}
		if (isr & USART_ISR_ORE)
		{
			USART2->ICR = USART_ICR_ORECF;
			HARDWARE_UART2_ONOVERFLOW();
		}
		if (isr & USART_ISR_FE)
			USART2->ICR = USART_ICR_FECF;
	}
	if (cr1 & USART_CR1_TXEIE)
	{
		if (isr & USART_ISR_TXE_TXFNF)
			HARDWARE_UART2_ONTXCHAR(USART2);
	}
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

#elif CPUSTYLE_R7S721

// Приём символа он последовательного порта
static void SCIFRXI2_IRQHandler(void)
{
	(void) SCIF2.SCFSR;						// Перед сбросом бита RDF должно произойти его чтение в ненулевом состоянии
	SCIF2.SCFSR = (uint16_t) ~ SCIF4_SCFSR_RDF;	// RDF=0 читать незачем (в примерах странное - сбрасывабтся и другие биты)
	uint_fast8_t n = (SCIF2.SCFDR & SCIF2_SCFDR_R) >> SCIF2_SCFDR_R_SHIFT;
	while (n --)
		HARDWARE_UART2_ONRXCHAR(SCIF2.SCFRDR & SCIF2_SCFRDR_D);
}

// Передача символа в последовательный порт
static void SCIFTXI2_IRQHandler(void)
{
	HARDWARE_UART2_ONTXCHAR(& SCIF2);
}

#elif CPUSTYLE_ALLWINNER

static RAMFUNC_NONILINE void UART2_IRQHandler(void)
{
	const uint_fast32_t ier = UART2->UART_DLH_IER;
	const uint_fast32_t usr = UART2->UART_USR;

	if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
	{
		if (usr & (1u << 3))	// RX FIFO Not Empty
			HARDWARE_UART2_ONRXCHAR(UART2->UART_RBR_THR_DLL);
	}
	if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
	{
		if (usr & (1u << 1))	// TX FIFO Not Full
			HARDWARE_UART2_ONTXCHAR(UART2);
	}
}

#elif CPUSTYLE_ROCKCHIP

static RAMFUNC_NONILINE void UART2_IRQHandler(void)
{
	const uint_fast32_t ier = UART2->UART_DLH_IER;
	const uint_fast32_t usr = UART2->UART_USR;

	if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
	{
		if (usr & (1u << 3))	// RX FIFO Not Empty
			HARDWARE_UART2_ONRXCHAR(UART2->UART_RBR_THR_DLL);
	}
	if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
	{
		if (usr & (1u << 1))	// TX FIFO Not Full
			HARDWARE_UART2_ONTXCHAR(UART2);
	}
}

#else
	#error Undefined CPUSTYLE_XXX
#endif	/* CPUSTYLE_ATMEGA_XXX4 */

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uart2_enabletx(uint_fast8_t state)
{
	hardware_uartx_enabletx(UARTBASENAME(thisPORT), state);
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uart2_enablerx(uint_fast8_t state)
{
	hardware_uartx_enablerx(UARTBASENAME(thisPORT), state);
}

/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uart2_tx(void * ctx, uint_fast8_t c)
{
	hardware_uartx_tx(UARTBASENAME(thisPORT), c);
}

/* дождаться, когда буде все передано */
void hardware_uart2_flush(void)
{
	hardware_uartx_flush(UARTBASENAME(thisPORT));
}

/* приём символа, если готов порт */
uint_fast8_t
hardware_uart2_getchar(char * cp)
{
	return hardware_uartx_getchar(UARTBASENAME(thisPORT), cp);
}

/* передача символа если готов порт */
uint_fast8_t
hardware_uart2_putchar(uint_fast8_t c)
{
	return hardware_uartx_putchar(UARTBASENAME(thisPORT), c);
}


void hardware_uart2_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd)
{
	int fifo = 1;
#if CPUSTYLE_STM32F1XX

	RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Включение тактирования USART2.
	(void) RCC->APB1ENR;

	USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables


	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     //включить тактирование альтернативных функций
	(void) RCC->APB2ENR;

	HARDWARE_UART2_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(USART2_IRQn, & USART2_IRQHandler);
	}

	USART2->CR1 |= USART_CR1_UE; // Включение USART2.

#elif CPUSTYLE_STM32H7XX

	RCC->APB1LENR |= RCC_APB1LENR_USART2EN; // Включение тактирования USART2.
	(void) RCC->APB1LENR;

	USART2->CR1 = 0;

#if WITHUART2HW_FIFO
	USART2->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUART2HW_FIFO */
	USART2->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUART2HW_FIFO */

	USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART2_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(USART2_IRQn, & USART2_IRQHandler);
	}

	USART2->CR1 |= USART_CR1_UE; // Включение USART2.

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX

	RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Включение тактирования USART2.
	(void) RCC->APB1ENR;

	USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART2_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(USART2_IRQn, & USART2_IRQHandler);
	}

	USART2->CR1 |= USART_CR1_UE; // Включение USART2.

#elif CPUSTYLE_ATMEGA_XXX4

	// USART initialization
	UCSR2B = (1U << RXEN1) | (1U << TXEN1) /* | (1U << UCSZ12) */;
	UCSR2C = (1U << UCSZ11) | (1U << UCSZ10);	// asynchronious mode, 8 bit.
	// enable pull-up registers for RXD and TXD pins: then rx or tx disabled, these pins disconnected fron UART
	PORTD |= ((1U << PD2) | (1U << PD3));
#if defined (DDRD2) && defined (DDRD3)
	DDRD &= ~ ((1U << DDRD2) | (1U << DDRD3));
#else
	DDRD &= ~ ((1U << DDD2) | (1U << DDD3));
#endif

#elif CPUSTYLE_ATMEGA128

	// USART initialization
	UCSR2B = (1U << RXEN1) | (1U << TXEN1) /* | (1U << UCSZ12) */;
	UCSR2C = (1U << UCSZ11) | (1U << UCSZ10);	// asynchronious mode, 8 bit.
	// enable pull-up registers for RXD and TXD pins: then rx or tx disabled, these pins disconnected fron UART
	//PORTE |= ((1U << PE0) | (1U << PE1));

#elif CPUSTYLE_ATXMEGAXXXA4

xxxx!;
	PORTE.DIRSET = PIN3_bm; // PE3 (TXD0) as output
	PORTE.DIRCLR = PIN2_bm; // PE2 (RXD0) as input
	PORTE_PIN2CTRL = (PORTE_PIN2CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc;							// pin is pulled high

	USARTE1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;
	USARTE1.CTRLB = USART_RXEN_bm | USART_TXEN_bm;

#elif CPUSTYLE_R7S721

    /* ---- Supply clock to the SCIF(channel 2) ---- */
	CPG.STBCR4 &= ~ CPG_STBCR4_BIT_MSTP45;	// Module Stop 45 - SCIF2
	(void) CPG.STBCR4;			/* Dummy read */

	SCIF2.SCSCR = 0x0000;	/* SCIF transmitting and receiving operations stop, internal clock */

	SCIF2.SCSCR = (SCIF2.SCSCR & ~ 0x03) |
		0x00 |						// internal clock
		0;

	/* ---- Serial status register(SCFSR2) setting ---- */
	/* ER,BRK,DR bit clear */
	(void) SCIF2.SCFSR;						// Перед сбросом бита xxx должно произойти его чтение в ненулевом состоянии
	SCIF2.SCFSR = ~ 0x0091;	// 0xFF6E;

	/* ---- Line status register (SCLSR2) setting ---- */
	/* ORER bit clear */
	//SCIF2.SCLSR.BIT.ORER  = 0;
	SCIF2.SCLSR &= ~ 0x0001;

	SCIF2.SCSMR =
		0x00 |	/* 8-N-1 format */
		0;

	/* ---- FIFO control register (SCFCR2) setting ---- */
	/*  RTS output active trigger        :Initial value	*/
	/*  Receive FIFO data trigger        :1-data		*/
	/*  Transmit FIFO data trigger       :0-data		*/
	/*  Modem control enable             :Disabled		*/
	/*  Receive FIFO data register reset :Disabled		*/
	/*  Loop-back test                   :Disabled 		*/
	SCIF2.SCFCR = 0x0030;

	/* ---- Serial port register (SCSPTR2) setting ---- */
	/* Serial port  break output(SPB2IO)  1: Enabled */
	/* Serial port break data(SPB2DT)  1: High-level */
	//SCIF2.SCSPTR |= 0x0003;

	if (debug == 0)
	{
	   serial_set_handler(SCIFRXI2_IRQn, SCIFRXI2_IRQHandler);
	   serial_set_handler(SCIFTXI2_IRQn, SCIFTXI2_IRQHandler);
	}
	HARDWARE_UART2_INITIALIZE();	/* Присоединить периферию к выводам */

	SCIF2.SCSCR |= 0x0030;	// TE RE - SCIF2 transmitting and receiving operations are enabled */

#elif CPUSTYLE_STM32MP1

	RCC->MP_APB1ENSETR = RCC_MP_APB1ENSETR_USART2EN; // Включение тактирования USART2.
	(void) RCC->MP_APB1ENSETR;
	RCC->MP_APB1LPENSETR = RCC_MP_APB1LPENSETR_USART2LPEN; // Включение тактирования USART2.
	(void) RCC->MP_APB1LPENSETR;
	RCC->APB1RSTSETR = RCC_APB1RSTSETR_USART2RST; // Установить сброс USART2.
	(void) RCC->APB1RSTSETR;
	RCC->APB1RSTCLRR = RCC_APB1RSTCLRR_USART2RST; // Снять брос USART2.
	(void) RCC->APB1RSTCLRR;

	USART2->CR1 = 0;

#if WITHUART2HW_FIFO
	USART2->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUART2HW_FIFO */
	USART2->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUART2HW_FIFO */

	USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE); // Transmitter Enable & Receiver Enables

	HARDWARE_UART2_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
		serial_set_handler(USART2_IRQn, USART2_IRQHandler);
	}

	USART2->CR1 |= USART_CR1_UE; // Включение USART2.

#elif CPUSTYLE_A64

	const unsigned ix = thisPORT;

	/* Open the clock gate for uart1 */
	CCU->BUS_CLK_GATING_REG3 |= (1u << (ix + 16));	// UART2_GATING

	/* De-assert uart2 reset */
	CCU-> BUS_SOFT_RST_REG4 |= (1u << (ix + 16));	//  UART0_RST

	/* Config uart2 to 115200-8-1-0 */
	uint32_t divisor = HARDWARE_UART_FREQ / ((defbaudrate) * 16);

	UART2->UART_DLH_IER = 0;
	UART2->UART_IIR_FCR = 0xf7;
	UART2->UART_MCR = 0x00;

	UART2->UART_LCR |= (1 << 7);	// Divisor Latch Access Bit
	UART2->UART_RBR_THR_DLL = divisor & 0xff;
	UART2->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART2->UART_LCR &= ~ (1 << 7);	// Divisor Latch Access Bit
	//
	UART2->UART_LCR &= ~ 0x3f;
	UART2->UART_LCR |=
			((0x03 & (bits - 5)) << 0) | (0 << 2) | // DAT_LEN_8_BITS ONE_STOP_BIT
			(!! odd << 4) |	// bit5:bit4 0 - even, 1 - odd
			(!! parity << 3) |	// bit3 1: parity enable
			0;

	HARDWARE_UART2_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART2_IRQn, UART2_IRQHandler);
	}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_T507 || CPUSTYLE_H616)

	const unsigned ix = thisPORT;

	/* Open the clock gate for uart2 */
	CCU->UART_BGR_REG |= (1u << (ix + 0));

	/* De-assert uart2 reset */
	CCU->UART_BGR_REG |= (1u << (ix + 16));

	/* Config uart2 to 115200-8-1-0 */
	uint32_t divisor = HARDWARE_UART_FREQ / ((defbaudrate) * 16);

	UART2->UART_DLH_IER = 0;
	UART2->UART_IIR_FCR = 0xf7;
	UART2->UART_MCR = 0x00;

	UART2->UART_LCR |= (1 << 7);	// Divisor Latch Access Bit
	UART2->UART_RBR_THR_DLL = divisor & 0xff;
	UART2->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART2->UART_LCR &= ~ (1 << 7);	// Divisor Latch Access Bit
	//
	UART2->UART_LCR &= ~ 0x3f;
	UART2->UART_LCR |=
			((0x03 & (bits - 5)) << 0) | (0 << 2) | // DAT_LEN_8_BITS ONE_STOP_BIT
			(!! odd << 4) |	// bit5:bit4 0 - even, 1 - odd
			(!! parity << 3) |	// bit3 1: parity enable
			0;

	HARDWARE_UART2_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART2_IRQn, UART2_IRQHandler);
	}


#elif CPUSTYLE_V3S

	const unsigned ix = thisPORT;

	/* Open the clock gate for uart2 */
	CCU->BUS_CLK_GATING_REG3 |= (1u << (ix + 16));

	/* De-assert uart2 reset */
	CCU->BUS_SOFT_RST_REG4 |= (1u << (ix + 16));

	/* Config uart2 to 115200-8-1-0 */
	uint32_t divisor = HARDWARE_UART_FREQ / ((defbaudrate) * 16);

	UART2->UART_DLH_IER = 0;
	UART2->UART_IIR_FCR = 0xf7;
	UART2->UART_MCR = 0x00;

	UART2->UART_LCR |= (1 << 7);	// Divisor Latch Access Bit
	UART2->UART_RBR_THR_DLL = divisor & 0xff;
	UART2->UART_DLH_IER = (divisor >> 8) & 0xff;
	UART2->UART_LCR &= ~ (1 << 7);	// Divisor Latch Access Bit
	//
	UART2->UART_LCR &= ~ 0x3f;
	UART2->UART_LCR |=
			((0x03 & (bits - 5)) << 0) | (0 << 2) | // DAT_LEN_8_BITS ONE_STOP_BIT
			(!! odd << 4) |	// bit5:bit4 0 - even, 1 - odd
			(!! parity << 3) |	// bit3 1: parity enable
			0;

	HARDWARE_UART2_INITIALIZE();
	if (debug == 0)
	{
	   serial_set_handler(UART2_IRQn, UART2_IRQHandler);
	}

#elif CPUSTYLE_ROCKCHIP
	#warning Unimplemented CPUSTYLE_ROCKCHIP

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

void
hardware_uart2_set_speed(uint_fast32_t baudrate)
{
#if CPUSTYLE_STM32MP1

	// uart2
	hardware_uartx_set_speed(UARTBASENAME(thisPORT), stm32mp1_uart2_4_get_freq(), baudrate);

#elif CPUSTYLE_STM32F

	// uart2 on apb1
	hardware_uartx_set_speed(UARTBASENAME(thisPORT), BOARD_USART2_FREQ, baudrate);

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

#endif /* WITHUART2HW */
