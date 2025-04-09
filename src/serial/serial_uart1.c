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
#include "clocks.h"

#define thisPORT 1

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
	const uint_fast32_t cr1 = USART1->CR1;

	if (cr1 & USART_CR1_RXNEIE)
	{
		if (isr & USART_ISR_RXNE_RXFNE)
		{
			const uint_fast8_t c = USART1->RDR;
			HARDWARE_UART1_ONRXCHAR(c);
		}
		if (isr & USART_ISR_ORE)
		{
			USART1->ICR = USART_ICR_ORECF;
			HARDWARE_UART1_ONOVERFLOW();
		}
		if (isr & USART_ISR_FE)
			USART1->ICR = USART_ICR_FECF;
	}
	if (cr1 & USART_CR1_TXEIE)
	{
		if (isr & USART_ISR_TXE_TXFNF)
			HARDWARE_UART1_ONTXCHAR(USART1);
	}
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
	const uint_fast32_t sts = UART1->ISR & UART1->IMR;
	if (sts & (1u << 5))	// RXOVR
	{
		UART1->ISR = (1u << 5);	// RXOVR
		HARDWARE_UART1_ONOVERFLOW();
	}
	if (sts & (1u << 7))	// PARE
	{
		UART1->ISR = (1u << 7);	// PARE
	}
	if (sts & (1u << 6))	// FRAME
	{
		UART1->ISR = (1u << 6);	// FRAME
	}
	if (sts & (1u << 3))	// TEMPTY
	{
		HARDWARE_UART1_ONTXCHAR(UART1);
	}
	while (hardware_uart1_getchar(& c))
	{
		HARDWARE_UART1_ONRXCHAR(c);
	}
}

#elif CPUSTYLE_ALLWINNER

static RAMFUNC_NONILINE void UART1_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	const uint_fast32_t ier = uart->UART_DLH_IER;
	const uint_fast32_t usr = uart->UART_USR;

	if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
	{
		if (usr & (1u << 3))	// RX FIFO Not Empty
			HARDWARE_UART1_ONRXCHAR(uart->UART_RBR_THR_DLL);
	}
	if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
	{
		if (usr & (1u << 1))	// TX FIFO Not Full
			HARDWARE_UART1_ONTXCHAR(uart);
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

#elif CPUSTYLE_ROCKCHIP

static RAMFUNC_NONILINE void UART1_IRQHandler(void)
{
	UART_t * const uart = UARTBASENAME(thisPORT);
	const uint_fast32_t ier = uart->UART_DLH_IER;
	const uint_fast32_t usr = uart->UART_USR;

	if (ier & (1u << 0))	// ERBFI Enable Received Data Available Interrupt
	{
		if (usr & (1u << 3))	// RX FIFO Not Empty
			HARDWARE_UART1_ONRXCHAR(uart->UART_RBR_THR_DLL);
	}
	if (ier & (1u << 1))	// ETBEI Enable Transmit Holding Register Empty Interrupt
	{
		if (usr & (1u << 1))	// TX FIFO Not Full
			HARDWARE_UART1_ONTXCHAR(uart);
	}
}

#else
	#error Undefined CPUSTYLE_XXX
#endif	/* CPUSTYLE_ATMEGA_XXX4 */

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерывания по передаче символа */
void hardware_uart1_enabletx(uint_fast8_t state)
{
	hardware_uartx_enabletx(UARTBASENAME(thisPORT), state);
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void hardware_uart1_enablerx(uint_fast8_t state)
{
	hardware_uartx_enablerx(UARTBASENAME(thisPORT), state);
}

/* передача символа из обработчика прерывания готовности передатчика */
void hardware_uart1_tx(void * ctx, uint_fast8_t c)
{
	hardware_uartx_tx(UARTBASENAME(thisPORT), c);
}

/* дождаться, когда буде все передано */
void hardware_uart1_flush(void)
{
	hardware_uartx_flush(UARTBASENAME(thisPORT));
}

/* приём символа, если готов порт */
uint_fast8_t
hardware_uart1_getchar(char * cp)
{
	return hardware_uartx_getchar(UARTBASENAME(thisPORT), cp);
}

/* передача символа если готов порт */
uint_fast8_t
hardware_uart1_putchar(uint_fast8_t c)
{
	return hardware_uartx_putchar(UARTBASENAME(thisPORT), c);
}

void hardware_uart1_initialize(uint_fast8_t debug, uint_fast32_t defbaudrate, uint_fast8_t bits, uint_fast8_t parity, uint_fast8_t odd)
{
	int fifo = 1;
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
	#endif /* HARDWARE_ARM_USEUSARTx */


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
#if WITHUART1HW_FIFO
	USART1->CR1 |= USART_CR1_FIFOEN_Msk;
#else /* WITHUART1HW_FIFO */
	USART1->CR1 &= ~ USART_CR1_FIFOEN_Msk;
#endif /* WITHUART1HW_FIFO */
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

	hardware_uartx_initialize(UARTBASENAME(thisPORT), BOARD_USART1_FREQ, defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART1_INITIALIZE();	/* Присоединить периферию к выводам */
	if (debug == 0)
	{
	   serial_set_handler(USART1_IRQn, USART1_IRQHandler);
	}

	USART1->CR1 |= USART_CR1_UE; // Включение USART1.

#elif CPUSTYLE_XC7Z

	const unsigned ix = thisPORT;
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

	HARDWARE_UART1_INITIALIZE();	/* Присоединить периферию к выводам */

	if (debug == 0)
	{
	   serial_set_handler(UART1_IRQn, UART1_IRQHandler);
	}

	r = UART1->CR;
	r |= XUARTPS_CR_RX_EN | XUARTPS_CR_TX_EN; // Set TX & RX enabled
	r &= ~(XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS); // Clear TX & RX disabled
	UART1->CR = r;

#elif CPUSTYLE_A64

	const unsigned ix = thisPORT;

	/* Open the clock gate for uart1 */
	CCU->BUS_CLK_GATING_REG3 |= (1u << (ix + 16));	// UART1_GATING

	/* De-assert uart1 reset */
	CCU-> BUS_SOFT_RST_REG4 |= (1u << (ix + 16));	//  UART1_RST

	hardware_uartx_initialize(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART1_INITIALIZE();
	if (debug == 0)
	{
	   serial_set_handler(UART1_IRQn, UART1_IRQHandler);
	}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616)

	const unsigned ix = thisPORT;

	/* Open the clock gate for uart1 */
	CCU->UART_BGR_REG |= (1u << (ix + 0));

	/* De-assert uart1 reset */
	CCU->UART_BGR_REG |= (1u << (ix + 16));

	hardware_uartx_initialize(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART1_INITIALIZE();

	if (debug == 0)
	{
	   serial_set_handler(UART1_IRQn, UART1_IRQHandler);
	}


#elif CPUSTYLE_V3S

	const unsigned ix = thisPORT;

	/* Open the clock gate for uart1 */
	CCU->BUS_CLK_GATING_REG3 |= (1u << (ix + 16));

	/* De-assert uart1 reset */
	CCU->BUS_SOFT_RST_REG4 |= (1u << (ix + 16));

	hardware_uartx_initialize(UARTBASENAME(thisPORT), HARDWARE_UART_FREQ, defbaudrate, bits, parity, odd, fifo);
	HARDWARE_UART1_INITIALIZE();
	if (debug == 0)
	{
	   serial_set_handler(UART1_IRQn, UART1_IRQHandler);
	}

#elif CPUSTYLE_VM14

	const unsigned ix = thisPORT;	// UART1

	CMCTR->GATE_SYS_CTR |= ((1u << 12) << ix); // UART1_EN Enable CLK

	hardware_uartx_initialize(UARTBASENAME(thisPORT), elveesvm14_get_usart_freq(), defbaudrate, bits, parity, odd, fifo);
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

#elif CPUSTYLE_STM32MP1

	// usart1
	USART1->BRR = calcdivround2(BOARD_USART1_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_STM32F

	// uart1 on apb2 up to 72/36 MHz

	USART1->BRR = calcdivround2(BOARD_USART1_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_R7S721

	hardware_uartx_set_speed(UARTBASENAME(thisPORT), P1CLOCK_FREQ, baudrate);

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

#endif /* WITHUART1HW */
