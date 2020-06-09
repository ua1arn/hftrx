/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "board.h"

#include <stdlib.h>
//#include <assert.h>

#include "formats.h"	// for debug prints
#include "inc/spi.h"

#if CPUSTYLE_STM32H7XX
#else /* CPUSTYLE_STM32H7XX */
#endif /* CPUSTYLE_STM32H7XX */

#if WITHSPISLAVE

static uint8_t spi3rxbuf16 [DSPCTL_BUFSIZE];

#if CPUSTYLE_STM32F4XX

/* DMA для прёма по SPI3 */
// RX	SPI3	DMA1	Stream 0	Channel 0

static void DMA1_SPI3_RX_initialize(void)
{

#if CPUSTYLE_STM32H7XX
	/* SPI3_RX - Stream0, Channel0 */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;//включил DMA1
	(void) RCC->AHB1ENR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA1_Stream0->PAR = (uint32_t) & SPI3->RXDR;
#elif CPUSTYLE_STM32MP1
	/* SPI3_RX - Stream0, Channel0 */
	RCC->MP_AHB2ENSETR = RCC_MC_AHB2ENSETR_DMA1EN; // включил DMA1
	(void) RCC->MP_AHB2ENSETR;
	RCC->MP_AHB2ENSETR = RCC_MC_AHB2ENSETR_DMAMUXEN; // включил DMAMUX
	(void) RCC->MP_AHB2ENSETR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA1_Stream0->PAR = (uint32_t) & SPI3->RXDR;
#else /* CPUSTYLE_STM32H7XX */
	/* SPI3_RX - Stream0, Channel0 */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;//включил DMA1
	(void) RCC->AHB1ENR;
	const uint_fast8_t ch = 0;
	DMA1_Stream0->PAR = (uint32_t) & SPI3->DR;
#endif /* CPUSTYLE_STM32H7XX */

	DMA1_Stream0->FCR &= ~ DMA_SxFCR_DMDIS;	// use direct mode
	DMA1_Stream0->CR =
		(ch * DMA_SxCR_CHSEL_0) |	//канал
		(0 * DMA_SxCR_MBURST_0) |	// 0: single transfer
		(0 * DMA_SxCR_PBURST_0) |	// 0: single transfer
		(0 * DMA_SxCR_DIR_0) |	// 00: Peripheral-to-memory
		(1 * DMA_SxCR_MINC) |	//инкремент памяти
		(0 * DMA_SxCR_MSIZE_0) | //длина в памяти - 8 bit
		(0 * DMA_SxCR_PSIZE_0) | //длина в DR - 8 bit
		(0 * DMA_SxCR_PL_0) |		// Priority level - low
		(0 * DMA_SxCR_CT) | // M0AR selected
		0;

#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel0->CCR = 61 * DMAMUX_CxCR_DMAREQ_ID_0;	// SPI3_RX
	(void) DMAMUX1_Channel0;
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */

	// Подготовка DMA к следующему приёму с начала буфера
	DMA1_Stream0->M0AR = (uint32_t) spi3rxbuf16;
	//DMA1_Stream0->M1AR = (uint32_t) spi3rxbuf16_1;
	DMA1_Stream0->NDTR = (DMA1_Stream0->NDTR & ~ DMA_SxNDT) |
		(DSPCTL_BUFSIZE * DMA_SxNDT_0);
	DMA1_Stream0->CR |= DMA_SxCR_EN;

}

// todo: разобраться с заполнением буфера, если ожидаем меньше, чем нам передали.
static RAMFUNC void stm32fxxx_pinirq_SPISLAVE(portholder_t pr)
{
	if ((pr & EXTI_PR_PR15) != 0)
	{
		const unsigned len = DSPCTL_BUFSIZE - DMA1_Stream0->NDTR;
		if ((DMA1->LISR & DMA_LISR_TCIF0) != 0)
		{
			// DMA допередавал до конца, возможно есть символы в буфере SPI
			while ((SPI3->SR & SPI_SR_RXNE) != 0)	
				(void) SPI3->DR;
			DMA1->LIFCR = DMA_LIFCR_CTCIF0;		// сбросил флаг - DMA готово начинать с начала
		}
		else
		{
			// DMA ещё может передавать
			// Надо привести в начальное состояние
			DMA1_Stream0->CR &= ~ DMA_SxCR_EN;
			while ((DMA1_Stream0->CR &  DMA_SxCR_EN) != 0)
				;
		}
		DMA1_Stream0->CR |= DMA_SxCR_EN;	// перезапуск DMA
		hardware_spi_slave_callback(spi3rxbuf16, len);
	}
}

// Прерывание по нарастающему фронту на PA15
void EXTI15_10_IRQHandler(void)
{
	const portholder_t pr = EXTI->PR & (EXTI_IMR_MR15 | EXTI_IMR_MR14 | EXTI_IMR_MR14 | EXTI_IMR_MR12 | EXTI_IMR_MR11 | EXTI_IMR_MR10);
	EXTI->PR = pr;		// reset all existing requests
	//(void) EXTI->PR;
	stm32fxxx_pinirq_SPISLAVE(pr);s
}

#endif /* CPUSTYLE_STM32F4XX */


#if CPUSTYLE_AT91SAM7S || CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S
	static portholder_t csr_val [SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
	static portholder_t csr_val16w [SPIC_MODES_COUNT];	/* для spi mode0..mode3 в режиме 16-ти битных слов. */
#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX
	static portholder_t cr1_val [SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
	static portholder_t cr1_val16w [SPIC_MODES_COUNT];	/* для spi mode0..mode3 в режиме 16-ти битных слов. */
#elif CPUSTYLE_STM32F30X
	static portholder_t cr1_val [SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
#elif CPUSTYLE_ATMEGA
	static portholder_t spcr_val [SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
	static portholder_t spsr_val ;
#elif CPUSTYLE_ATXMEGA
	static portholder_t spi_ctl_val [SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
#elif CPUSTYLE_R7S721
	static portholder_t spi_spcmd_val [SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
	static portholder_t spi_spcmd_val16w [SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
#endif /* */

/* инициализация и перевод в состояние "отключено" */
// вызывается при запрещённых прерываниях.
void hardware_spi_slave_initialize(void)		
{
#if CPUSTYLE_STM32F4XX

#if CPUSTYLE_STM32H7XX
	RCC->APB1LENR |= RCC_APB1LENR_SPI3EN; // Подать тактирование
	__DSB();
#else /* CPUSTYLE_STM32H7XX */
	RCC->APB1ENR |= RCC_APB1ENR_SPI3EN; // Подать тактирование
	__DSB();
#endif /* CPUSTYLE_STM32H7XX */

	SPI3->CR1 = 0x0000;             //очистить первый управляющий регистр
	SPI3->CR2 = 0x0000;             //очистить второй управляющий регистр

	DMA1_SPI3_RX_initialize();		// ch=0, DMAMUX=61
#if CPUSTYLE_STM32H7XX
	SPI3->CFG1 = SPI_CFG1_RXDMAEN;
#else /* CPUSTYLE_STM32H7XX */
	SPI3->CR2 = SPI_CR2_RXDMAEN;
#endif /* CPUSTYLE_STM32H7XX */

	// When the SSM bit is set, the NSS pin input is replaced with the value from the SSI bit.
	// This bit has an effect only when the SSM bit is set. The value of this bit is forced onto the NSS pin and the IO value of the NSS pin is ignored.
	const uint_fast32_t cr1bitsslave    = SPI_CR1_SPE;
	const uint_fast32_t cr1bits16wslave = cr1bitsslave | SPI_CR1_DFF;

	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 8-битная передача
	cr1_val [SPIC_MODE0] = cr1bitsslave;					// TODO: not tested
	cr1_val [SPIC_MODE1] = cr1bitsslave | SPI_CR1_CPHA;	// TODO: not tested
	cr1_val [SPIC_MODE2] = cr1bitsslave | SPI_CR1_CPOL;	// CLK leave HIGH	
	cr1_val [SPIC_MODE3] = cr1bitsslave | SPI_CR1_CPOL | SPI_CR1_CPHA; // wrk = CLK leave "HIGH"

	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 16-битная передача
	cr1_val16w [SPIC_MODE0] = cr1bits16wslave;
	cr1_val16w [SPIC_MODE1] = cr1bits16wslave | SPI_CR1_CPHA;
	cr1_val16w [SPIC_MODE2] = cr1bits16wslave | SPI_CR1_CPOL;	// CLK leave HIGH	
	cr1_val16w [SPIC_MODE3] = cr1bits16wslave | SPI_CR1_CPOL | SPI_CR1_CPHA; // wrk = CLK leave "HIGH"

	if (SPI_SLAVE_NSS != EXTI_PR_PR15)
	{
		void wrong_SPI_SLAVE_NSS(void);
		wrong_SPI_SLAVE_NSS();
	}

	HARDWARE_SPISLAVE_INITIALIZE();
#else
	#error Wrong CPUSTYLE macro
#endif
}

// вызывается при разрешённых прерываниях.
void hardware_spi_slave_enable(uint_fast8_t spimode)		
{
#if CPUSTYLE_STM32F4XX
	// Переключение в режим приёма - 8 бит
	SPI3->CR1 = cr1_val [spimode];

#else
	#error Wrong CPUSTYLE macro
#endif
}

#endif /* WITHSPISLAVE */
