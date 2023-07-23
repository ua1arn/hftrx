/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "board.h"

#include "audio.h"

#include "formats.h"	// for debug prints
#include "gpio.h"

typedef struct codechw
{
	void (* initialize_rx)(void);	/* инициализация периферии процессора для приёма данных от кодека */
	void (* initialize_tx)(void);	/* инициализация периферии процессора для передачи данных в кодек */
	void (* initializedma_rx)(void);	/* инициализация DMA процессора для приёма данных от кодека */
	void (* initializedma_tx)(void);	/* инициализация DMA процессора для передачи данных в кодек */
	void (* enable_rx)(uint_fast8_t state);	/* разрешение приёма данных от кодека */
	void (* enable_tx)(uint_fast8_t state);	/* разрешение передачи данных в кодек */
	const char * label;
} codechw_t;

/* DMA word lemgth parameter */
#define FPGAIF_DMA_SxCR_xSIZE		0x02uL	// 10: word (32-bit)
#define FPGARTS_DMA_SxCR_xSIZE		0x02uL	// 10: word (32-bit)

#if CODEC1_FRAMEBITS == 32
	#define CODEC1_DMA_SxCR_xSIZE		0x01uL	// 01: half-word (16-bit)

#else /*  CODEC1_FRAMEBITS == 64 */
	// 2*32, 2*24
	#define CODEC1_DMA_SxCR_xSIZE		0x02uL	// 10: word (32-bit)

#endif /*  CODEC1_FRAMEBITS == 64 */

static void hardware_dummy_initialize(void)
{

}

static void hardware_dummy_enable(uint_fast8_t state)
{

}

#if WITHINTEGRATEDDSP

// Сейчас в эту память будем читать по DMA
static uintptr_t 
dma_invalidate16rx(uintptr_t addr)
{
	ASSERT((addr % DCACHEROWSIZE) == 0);
	ASSERT((buffers_dmabuffer16rxcachesize() % DCACHEROWSIZE) == 0);
	dcache_invalidate(addr, buffers_dmabuffer16rxcachesize());
	return addr;
}

// Сейчас эта память будет записываться по DMA куда-то
// Потом содержимое не требуется
static uintptr_t
dma_flush16tx(uintptr_t addr)
{
	ASSERT((addr % DCACHEROWSIZE) == 0);
	ASSERT((buffers_dmabuffer16txcachesize() % DCACHEROWSIZE) == 0);
	dcache_clean_invalidate(addr, buffers_dmabuffer16txcachesize());
	return addr;
}

// Сейчас в эту память будем читать по DMA
static uintptr_t
dma_invalidate192rts(uintptr_t addr)
{
	ASSERT((addr % DCACHEROWSIZE) == 0);
	ASSERT((buffers_dmabuffer192rtscachesize() % DCACHEROWSIZE) == 0);
	dcache_invalidate(addr,  buffers_dmabuffer192rtscachesize());
	return addr;
}

// Сейчас в эту память будем читать по DMA
uintptr_t
dma_invalidate32rx(uintptr_t addr)
{
	ASSERT((addr % DCACHEROWSIZE) == 0);
	ASSERT((buffers_dmabuffer32rxcachesize() % DCACHEROWSIZE) == 0);
	dcache_invalidate(addr, buffers_dmabuffer32rxcachesize());
	return addr;
}

// Сейчас в эту память будем читать по DMA
uintptr_t
dma_invalidate32rts(uintptr_t addr)
{
	ASSERT((addr % DCACHEROWSIZE) == 0);
	ASSERT((buffers_dmabuffer32rxcachesize() % DCACHEROWSIZE) == 0);
	dcache_invalidate(addr, buffers_dmabuffer32rtscachesize());
	return addr;
}

// Сейчас эта память будет записываться по DMA куда-то
// Потом содержимое не требуется
static uintptr_t dma_flush32tx(uintptr_t addr)
{
	ASSERT((addr % DCACHEROWSIZE) == 0);
	ASSERT((buffers_dmabuffer32txcachesize() % DCACHEROWSIZE) == 0);
	dcache_clean_invalidate(addr,  buffers_dmabuffer32txcachesize());
	return addr;
}

#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

#define DMA_SxCR_PL_VALUE 0u		// STM32xxx DMA Priority level

enum
{
#if defined (WITHFPGAIF_FRAMEBITS)

	FPGAIF_SAI_CR1_DS = 0x07, // 6: data size - 24 bit, 7: 32 bit, 4: 16 bit

	NBSLOT_TX_SAIFPGA = WITHFPGAIF_FRAMEBITS / 32,
	NBSLOT_RX_SAIFPGA = WITHFPGAIF_FRAMEBITS / 32,
	// Количество битов в SLOTEN_TX_xxx и SLOTEN_RX_xxx должно быть равно
	// значениям DMABUFFSTEP32RX и DMABUFFSTEP32TX соответственно.
	#if WITHFPGAIF_FRAMEBITS == 256
		// FPGA версия
		#define DMABUFFSTEP32RX	8
		SLOTEN_RX_SAIFPGA = 0x00FF,

		// На передачу во всех версиях FPGA используется один и тот же блок
		// В каждой половине фрейма используется первый слот (первые 32 бита после переключения WS)
		#define DMABUFFSTEP32TX	8
		SLOTEN_TX_SAIFPGA = 0x00FF,

	#elif WITHFPGAIF_FRAMEBITS == 64
		//#if WITHRTS96 || WITHUSEDUALWATCH
		//	#error WITHRTS96 or WITHUSEDUALWATCH unsupported with 64-bit frame length
		//#endif /* WITHRTS96 || WITHUSEDUALWATCH */
		//#define DMABUFFSTEP32RX	(WITHFPGAIF_FRAMEBITS / 32) //2	// 2 - каждому сэмплу соответствует два числа в DMA буфере	- I/Q
		// Аппаратный кодек
		SLOTEN_RX_SAIFPGA = 0x0003,
		SLOTEN_TX_SAIFPGA = 0x0003,
	#else
		#error Unexpected WITHFPGAIF_FRAMEBITS value
	#endif

#endif /* defined (WITHFPGAIF_FRAMEBITS) */

#if defined (WITHFPGARTS_FRAMEBITS)

	FPGARTS_SAI_CR1_DS = 0x07, // 6: data size - 24 bit, 7: 32 bit, 4: 16 bit

	NBSLOT_SAIRTS = WITHFPGARTS_FRAMEBITS / 32,
	// Количество битов в SLOTEN_TX_xxx и SLOTEN_RX_xxx должно быть равно
	// значениям DMABUFFSTEP32RX и DMABUFFSTEP32TX соответственно.
	#if WITHFPGARTS_FRAMEBITS == 256
		// FPGA версия
		#if WITHUSEDUALWATCH
			SLOTEN_RX_SAIRTS = 0x00FF,
		#else /* WITHUSEDUALWATCH */
			SLOTEN_RX_SAIRTS = 0x00FF,
		#endif /* WITHUSEDUALWATCH */

		// На передачу во всех версиях FPGA используется один и тот же блок
		// В каждой половине фрейма используется первый слот (первые 32 бита после переключения WS)
		SLOTEN_TX_SAIRTS = 0x0011,

	#elif WITHFPGARTS_FRAMEBITS == 64
		// Аудио кодек
		SLOTEN_RX_SAIRTS = 0x0003,
		SLOTEN_TX_SAIRTS = 0x0003,
	#else
		#error Unexpected WITHFPGARTS_FRAMEBITS value
	#endif

#endif /* defined (WITHFPGARTS_FRAMEBITS) */

#if defined (CODEC1_FRAMEBITS)

	#if CODEC1_FRAMEBITS == 64
		SAIAUDIO_SAI_CR1_DS = 0x07, // 6: data size - 24 bit, 7: 32 bit, 4: 16 bit
	#elif CODEC1_FRAMEBITS == 32
		SAIAUDIO_SAI_CR1_DS = 0x04, // 6: data size - 24 bit, 7: 32 bit, 4: 16 bit
	#else /* CODEC1_FRAMEBITS */
		#error Unexpected CODEC1_FRAMEBITS value
	#endif /* CODEC1_FRAMEBITS */
	NBSLOT_RX_SAIAUDIO = 2,	/* DMABUFFSTEP16RX всегда стерео */
	NBSLOT_TX_SAIAUDIO = 2,	/* DMABUFFSTEP16TX всегда стерео */
	// Количество битов в SLOTEN_TX_xxx и SLOTEN_RX_xxx должно быть равно
	// значению DMABUFFSTEP16TX/DMABUFFSTEP16RX соответственно.
	SLOTEN_RX_SAIAUDIO = 0x0003,
	SLOTEN_TX_SAIAUDIO = 0x0003,

#endif /* defined (CODEC1_FRAMEBITS) */

		enum_finish
};


//
#if WITHI2SCLOCKFROMPIN
	static uint_fast32_t 
	NOINLINEAT
	calcdivround_exti2s(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		return (EXTI2S_FREQ + freq / 2) / freq;
	}
#else /* WITHI2SCLOCKFROMPIN */
	static uint_fast32_t 
	NOINLINEAT
	calcdivround_plli2s(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		return (PLLI2S_FREQ_OUT + freq / 2) / freq;
	}
#endif /* WITHI2SCLOCKFROMPIN */



#define DRD(r) ((void) (r))

#if 0

#define DMAERR(dma, dmastream, status, control, errorf, resetf) do { \
		if (((dma)->status & errorf) != 0) \
		{ \
			(dma)->control = resetf; \
			PRINTF("DMAERR " # dmastream " " # errorf " M0AR=%p M1AR=%p" "\n", (dmastream)->M0AR, (dmastream)->M1AR); \
			(dmastream)->CR &= ~ DMA_SxCR_EN; \
			while ((dmastream)->CR & DMA_SxCR_EN) \
				; \
			(dmastream)->CR |= DMA_SxCR_EN; \
			DRD((dmastream)->CR); \
		} \
	} while (0)

// формируется строка вроде такой:
// DMAERR(DMA1, DMA1_Stream3, LISR, LIFCR, DMA_LISR_TEIF3, DMA_LIFCR_CTEIF3);

#define HANDLEERRORS(d, s, hl) do { \
		DMAERR(DMA ## d, DMA ## d ## _Stream ## s, hl ## ISR, hl ## IFCR, DMA_ ## hl ## ISR_TEIF ## s, DMA_ ## hl ## IFCR_CTEIF ## s); /* TE */ \
		DMAERR(DMA ## d, DMA ## d ## _Stream ## s, hl ## ISR, hl ## IFCR, DMA_ ## hl ## ISR_DMEIF ## s, DMA_ ## hl ## IFCR_CDMEIF ## s); /* DME */ \
		DMAERR(DMA ## d, DMA ## d ## _Stream ## s, hl ## ISR, hl ## IFCR, DMA_ ## hl ## ISR_FEIF ## s, DMA_ ## hl ## IFCR_CFEIF ## s); /* FE */ \
	} while (0)

#else

	#define HANDLEERRORS(d, s, hl) /* */
#endif

/* получение битов режима I2S для каналов обммена с кодеком */
static portholder_t stm32xxx_i2scfgr_afcodec(void)
{
#if WITHI2SCLOCKFROMPIN
	const uint_fast32_t i2sdivider = calcdivround_exti2s(ARMI2SMCLK);
#else /* WITHI2SCLOCKFROMPIN */
	const uint_fast32_t i2sdivider = calcdivround_plli2s(ARMI2SMCLK);
#endif /* WITHI2SCLOCKFROMPIN */
	const portholder_t i2soddv = (i2sdivider % 2) != 0;
	const portholder_t i2sdiv = i2sdivider / 2;

	const portholder_t i2scfgr =
		SPI_I2SCFGR_I2SMOD |	// 1: I2S/PCM mode is selected

#if CODEC1_FRAMEBITS == 64
		//(1u << SPI_I2SCFGR_DATFMT_Pos) |	// 1: the data inside the SPI2S_RXDR or SPI2S_TXDR are left aligned.
		//(1u << SPI_I2SCFGR_FIXCH_Pos) |		// 1: the channel length in slave mode is supposed to be 16 or 32 bits (according to CHLEN)
		//(1u << SPI_I2SCFGR_CHLEN_Pos) |		// 1: 32-bit wide audio channel
		(2u << SPI_I2SCFGR_DATLEN_Pos) |	// 00: 16-bit data length, 01: 24-bit data length, 10: 32-bit data length

#elif CODEC1_FRAMEBITS == 32
	#if defined (SPI_I2SCFGR_FIXCH_Pos)
		//(1u << SPI_I2SCFGR_DATFMT_Pos) |	// 1: the data inside the SPI2S_RXDR or SPI2S_TXDR are left aligned.
		(1u << SPI_I2SCFGR_FIXCH_Pos) |		// 1: the channel length in slave mode is supposed to be 16 or 32 bits (according to CHLEN)
	#endif /* defined (SPI_I2SCFGR_FIXCH_Pos) */
		//(0u << SPI_I2SCFGR_CHLEN_Pos) |		// 0: 16-bit wide audio channel
		(0u << SPI_I2SCFGR_DATLEN_Pos) |	// 00: 16-bit data length, 01: 24-bit data length, 10: 32-bit data length

#else /* CODEC1_FRAMEBITS == 64 */
		//(0u << SPI_I2SCFGR_FIXCH_Pos) |		// 0: the channel length in slave mode is different from 16 or 32 bits (CHLEN not taken into account)
		//(0u << SPI_I2SCFGR_CHLEN_Pos) |		// 0: 16-bit wide audio channel
		(0u << SPI_I2SCFGR_DATLEN_Pos) |	// 00: 16-bit data length, 01: 24-bit data length, 10: 32-bit data length

#endif /* CODEC1_FRAMEBITS == 64 */

		(0u << SPI_I2SCFGR_CKPOL_Pos) |

#if CODEC1_FORMATI2S_PHILIPS
		(0u << SPI_I2SCFGR_I2SSTD_Pos) |	// 00: I2S Philips standard

#else /* CODEC1_FORMATI2S_PHILIPS */
		(1u << SPI_I2SCFGR_I2SSTD_Pos) |	// 01: MSB justified standard (left justified)

#endif /* CODEC1_FORMATI2S_PHILIPS */

#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
		((i2sdiv << SPI_I2SCFGR_I2SDIV_Pos) & SPI_I2SCFGR_I2SDIV) |
		(SPI_I2SCFGR_ODD * i2soddv) |
		SPI_I2SCFGR_MCKOE_Msk |
#endif /* CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1 */
		0;

	return i2scfgr;
}

// Обработчик прерывания DMA по приему I2S - I2S2_EXT
void RAMFUNC_NONILINE DMA1_Stream3_IRQHandler_codec1_rx(void)
{
	// проверка условия может потребоваться при добавлении обработчика ошибки
	if ((DMA1->LISR & DMA_LISR_TCIF3) != 0)
	{
		DMA1->LIFCR = DMA_LIFCR_CTCIF3;	// Clear TC interrupt flag
		//__DMB();	//ensure the ordering of data cache maintenance operations and their effects
		//ASSERT((SAI1_Block_B->SR & SAI_xSR_OVRUDR_Msk) == 0);
		const uint_fast8_t b = (DMA1_Stream3->CR & DMA_SxCR_CT) != 0;
		if (b != 0)
		{
			const uintptr_t addr = DMA1_Stream3->M0AR;
			DMA1_Stream3->M0AR = dma_invalidate16rx(allocate_dmabuffer16rx());
			DRD(DMA1_Stream3->M0AR);
			processing_dmabuffer16rx(addr);
		}
		else
		{
			const uintptr_t addr = DMA1_Stream3->M1AR;
			DMA1_Stream3->M1AR = dma_invalidate16rx(allocate_dmabuffer16rx());
			DRD(DMA1_Stream3->M1AR);
			processing_dmabuffer16rx(addr);
		}
	}

	//DMAERR(DMA1, DMA1_Stream3, LISR, LIFCR, DMA_LISR_TEIF3, DMA_LIFCR_CTEIF3);
	HANDLEERRORS(1, 3, L);
}

// Обработчик прерывания DMA по приему I2S - I2S3
void RAMFUNC_NONILINE DMA1_Stream0_IRQHandler_codec1_rx(void)
{
	if ((DMA1->LISR & DMA_LISR_TCIF0) != 0)
	{
		DMA1->LIFCR = DMA_LIFCR_CTCIF0;	// Clear TC interrupt flag
		//__DMB();	//ensure the ordering of data cache maintenance operations and their effects
		//ASSERT((SAI1_Block_B->SR & SAI_xSR_OVRUDR_Msk) == 0);
		const uint_fast8_t b = (DMA1_Stream0->CR & DMA_SxCR_CT) != 0;
		if (b != 0)
		{
			const uintptr_t addr = DMA1_Stream0->M0AR;
			DMA1_Stream0->M0AR = dma_invalidate16rx(allocate_dmabuffer16rx());
			DRD(DMA1_Stream0->M0AR);
			processing_dmabuffer16rx(addr);
		}
		else
		{
			const uintptr_t addr = DMA1_Stream0->M1AR;
			DMA1_Stream0->M1AR = dma_invalidate16rx(allocate_dmabuffer16rx());
			DRD(DMA1_Stream0->M1AR);
			processing_dmabuffer16rx(addr);
		}
	}

	//DMAERR(DMA1, DMA1_Stream0, LISR, LIFCR, DMA_LISR_TEIF0, DMA_LIFCR_CTEIF0);
	HANDLEERRORS(1, 0, L);
}

// Обработчик прерывания DMA по передаче I2S2
// Use dcache_clean
void RAMFUNC_NONILINE DMA1_Stream4_IRQHandler_codec1_tx(void)
{
	if ((DMA1->HISR & DMA_HISR_TCIF4) != 0)
	{
		DMA1->HIFCR = DMA_HIFCR_CTCIF4;	// Clear TC interrupt flag соответствующий stream
		//__DMB();	//ensure the ordering of data cache maintenance operations and their effects
		//ASSERT((SAI1_Block_B->SR & SAI_xSR_OVRUDR_Msk) == 0);
		const uint_fast8_t b = (DMA1_Stream4->CR & DMA_SxCR_CT) != 0;
		if (b != 0)
		{
			release_dmabuffer16tx(DMA1_Stream4->M0AR);
			DMA1_Stream4->M0AR = dma_flush16tx(getfilled_dmabuffer16txphones());
			DRD(DMA1_Stream4->M0AR);
		}
		else
		{
			release_dmabuffer16tx(DMA1_Stream4->M1AR);
			DMA1_Stream4->M1AR = dma_flush16tx(getfilled_dmabuffer16txphones());
			DRD(DMA1_Stream4->M1AR);
		}
	}

	//DMAERR(DMA1, DMA1_Stream4, HISR, HIFCR, DMA_HISR_TEIF4, DMA_HIFCR_CTEIF4);
	HANDLEERRORS(1, 4, H);
}

// Инициализация DMA по передаче I2S2
// Use dcache_clean
static void 
DMA_I2S2_TX_initialize_codec1(void)
{
	/* SPI2_TX - Stream4, Channel0 */ 
	/* DMA для передачи по I2S2*/
#if CPUSTYLE_STM32MP1
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMA1EN; // включил DMA1
	(void) RCC->MP_AHB2ENSETR;
	RCC->MP_AHB2LPENSETR = RCC_MP_AHB2LPENSETR_DMA1LPEN; // включил DMA1
	(void) RCC->MP_AHB2LPENSETR;
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMAMUXEN; // включил DMAMUX
	(void) RCC->MP_AHB2ENSETR;
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA1_Stream4->PAR = (uintptr_t) & SPI2->TXDR;

#elif CPUSTYLE_STM32H7XX
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;	// включил DMA1
	(void) RCC->AHB1ENR;
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA1_Stream4->PAR = (uintptr_t) & SPI2->TXDR;

#else /* others */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;	// включил DMA1
	(void) RCC->AHB1ENR;
	const uint_fast8_t ch = 0;
	DMA1_Stream4->PAR = (uintptr_t) & SPI2->DR;

#endif /* CPUSTYLE_STM32MP1 */

	ASSERT((DMA1_Stream4->CR & DMA_SxCR_EN) == 0);
	DMA1_Stream4->M0AR = dma_flush16tx(allocate_dmabuffer16tx());
    DMA1_Stream4->M1AR = dma_flush16tx(allocate_dmabuffer16tx());
	DMA1_Stream4->NDTR = (DMA1_Stream4->NDTR & ~ DMA_SxNDT) |
		(DMABUFFSIZE16TX * DMA_SxNDT_0);
	DMA1_Stream4->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use direct mode
	DRD(DMA1_Stream4->FCR);
	DMA1_Stream4->CR =
		ch * DMA_SxCR_CHSEL_0 | // канал
		0 * DMA_SxCR_MBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_PBURST_0 |	// 0: single transfer
		1 * DMA_SxCR_DIR_0 |	// направление - память - периферия
		1 * DMA_SxCR_MINC |		// инкремент памяти
		(CODEC1_DMA_SxCR_xSIZE << DMA_SxCR_MSIZE_Pos) |	// длина в памяти - 16b/32b
		(CODEC1_DMA_SxCR_xSIZE << DMA_SxCR_PSIZE_Pos) |	// длина в SPI_DR- 16b/32b
		(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
		0 * DMA_SxCR_CT |		// M0AR selected
		1 * DMA_SxCR_DBM |		// double buffer mode seelcted
		0;
#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel4->CCR = 40 * DMAMUX_CxCR_DMAREQ_ID_0;	// SPI2_TX
	DRD(DMAMUX1_Channel4->CCR);
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */

	DMA1->HIFCR = DMA_HISR_TCIF4;	// Clear TC interrupt flag соответствующий stream
	DMA1_Stream4->CR |= DMA_SxCR_TCIE;	// Разрешаем прерывания от DMA

	arm_hardware_set_handler_realtime(DMA1_Stream4_IRQn, DMA1_Stream4_IRQHandler_codec1_tx);

	DMA1_Stream4->CR |= DMA_SxCR_EN;
}

#if defined (I2S2ext)
/* Инициализация DMA для прёма по I2S2ext*/
static void 
DMA_I2S2ext_rx_init_audio(void)
{
	const uint_fast8_t ch = 3;
	/* I2S2_EXT_RX - Stream3, Channel3 */ 
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;	// включил DMA1
	(void) RCC->AHB1ENR;
	DMA1_Stream3->PAR = (uintptr_t) & I2S2ext->DR;

    DMA1_Stream3->M0AR = dma_invalidate16rx(allocate_dmabuffer16rx());
    DMA1_Stream3->M1AR = dma_invalidate16rx(allocate_dmabuffer16rx());
	DMA1_Stream3->NDTR = (DMA1_Stream3->NDTR & ~ DMA_SxNDT) |
		(DMABUFFSIZE16RX * DMA_SxNDT_0);

	DMA1_Stream3->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use direct mode
	DRD(DMA1_Stream3->FCR);
	DMA1_Stream3->CR =
		ch * DMA_SxCR_CHSEL_0 | // канал
		0 * DMA_SxCR_MBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_PBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_DIR_0 |	// 00: Peripheral-to-memory
		1 * DMA_SxCR_MINC |		//инкремент памяти
		(CODEC1_DMA_SxCR_xSIZE << DMA_SxCR_MSIZE_Pos) |	// длина в памяти - 16b/32b
		(CODEC1_DMA_SxCR_xSIZE << DMA_SxCR_PSIZE_Pos) |	// длина в SPI_DR- 16b/32b
		(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
		0 * DMA_SxCR_CT |		// M0AR selected
		1 * DMA_SxCR_DBM |		// double buffer mode seelcted
		0;

	DMA1->LIFCR = DMA_LISR_TCIF3;	// Clear TC interrupt flag
	DMA1_Stream3->CR |= DMA_SxCR_TCIE;	// Разрешаем прерывания от DMA

	arm_hardware_set_handler_realtime(DMA1_Stream3_IRQn, DMA1_Stream3_IRQHandler_codec1_rx);

	DMA1_Stream3->CR |= DMA_SxCR_EN;
}

#endif /* defined (I2S2ext) */

/* Инициализация DMA для прёма по I2S3 */
static void 
DMA_I2S3_RX_initialize_codec1(void)
{
	/* I2S3_RX - DMA1, Stream0, Channel0 */ 
#if CPUSTYLE_STM32MP1
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMA1EN; // включил DMA1
	(void) RCC->MP_AHB2ENSETR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA1_Stream0->PAR = (uintptr_t) & SPI3->RXDR;

#elif CPUSTYLE_STM32H7XX
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;	// включил DMA1
	(void) RCC->AHB1ENR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA1_Stream0->PAR = (uintptr_t) & SPI3->RXDR;

#else /* others */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;	// включил DMA1
	(void) RCC->AHB1ENR;
	const uint_fast8_t ch = 0;
	DMA1_Stream0->PAR = (uintptr_t) & SPI3->DR;

#endif /* CPUSTYLE_STM32MP1 */

    DMA1_Stream0->M0AR = dma_invalidate16rx(allocate_dmabuffer16rx());
    DMA1_Stream0->M1AR = dma_invalidate16rx(allocate_dmabuffer16rx());
	DMA1_Stream0->NDTR = (DMA1_Stream0->NDTR & ~ DMA_SxNDT) |
		(DMABUFFSIZE16RX * DMA_SxNDT_0);

	DMA1_Stream0->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use direct mode
	DRD(DMA1_Stream0->FCR);
	DMA1_Stream0->CR =
		ch * DMA_SxCR_CHSEL_0 | // канал
		0 * DMA_SxCR_MBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_PBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_DIR_0 |	// 00: Peripheral-to-memory
		1 * DMA_SxCR_MINC |		//инкремент памяти
		(CODEC1_DMA_SxCR_xSIZE << DMA_SxCR_MSIZE_Pos) |	// длина в памяти - 16b/32b
		(CODEC1_DMA_SxCR_xSIZE << DMA_SxCR_PSIZE_Pos) |	// длина в SPI_DR- 16b/32b
		(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
		0 * DMA_SxCR_CT |		// M0AR selected
		1 * DMA_SxCR_DBM |		// double buffer mode seelcted
		0;

#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel0->CCR = 61 * DMAMUX_CxCR_DMAREQ_ID_0;	// SPI3_RX
	DRD(DMAMUX1_Channel0->CCR);
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */

	DMA1->LIFCR = DMA_LISR_TCIF0;	// Clear TC interrupt flag
	DMA1_Stream0->CR |= DMA_SxCR_TCIE;	// Разрешаем прерывания от DMA

	arm_hardware_set_handler_realtime(DMA1_Stream0_IRQn, DMA1_Stream0_IRQHandler_codec1_rx);

	DMA1_Stream0->CR |= DMA_SxCR_EN;
}

/* Инициализация DMA для прёма по I2S2 (дуплекс) */
static void
DMA_I2S2_RX_initialize_codec1(void)
{
	/* I2S2_RX - DMA1, Stream0, Channel0 */
#if CPUSTYLE_STM32MP1
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMA1EN; // включил DMA1
	(void) RCC->MP_AHB2ENSETR;
	RCC->MP_AHB2LPENSETR = RCC_MP_AHB2LPENSETR_DMA1LPEN; // включил DMA1
	(void) RCC->MP_AHB2LPENSETR;
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMAMUXEN; // включил DMAMUX
	(void) RCC->MP_AHB2ENSETR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA1_Stream0->PAR = (uintptr_t) & SPI2->RXDR;

#elif CPUSTYLE_STM32H7XX
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;	// включил DMA1
	(void) RCC->AHB1ENR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA1_Stream0->PAR = (uintptr_t) & SPI2->RXDR;

#else /* others */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;	// включил DMA1
	(void) RCC->AHB1ENR;
	const uint_fast8_t ch = 0;
	DMA1_Stream0->PAR = (uintptr_t) & SPI2->DR;

#endif /* CPUSTYLE_STM32MP1 */

    DMA1_Stream0->M0AR = dma_invalidate16rx(allocate_dmabuffer16rx());
    DMA1_Stream0->M1AR = dma_invalidate16rx(allocate_dmabuffer16rx());
	DMA1_Stream0->NDTR = (DMA1_Stream0->NDTR & ~ DMA_SxNDT) |
		(DMABUFFSIZE16RX * DMA_SxNDT_0);

	DMA1_Stream0->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use direct mode
	DRD(DMA1_Stream0->FCR);
	DMA1_Stream0->CR =
		ch * DMA_SxCR_CHSEL_0 | // канал
		0 * DMA_SxCR_MBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_PBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_DIR_0 |	// 00: Peripheral-to-memory
		1 * DMA_SxCR_MINC |		//инкремент памяти
		(CODEC1_DMA_SxCR_xSIZE << DMA_SxCR_MSIZE_Pos) |	// длина в памяти - 16b/32b
		(CODEC1_DMA_SxCR_xSIZE << DMA_SxCR_PSIZE_Pos) |	// длина в SPI_DR- 16b/32b
		(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
		0 * DMA_SxCR_CT |		// M0AR selected
		1 * DMA_SxCR_DBM |		// double buffer mode seelcted
		0;

#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel0->CCR = 39 * DMAMUX_CxCR_DMAREQ_ID_0;	// SPI2_RX
	DRD(DMAMUX1_Channel0->CCR);
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */


	DMA1->LIFCR = DMA_LISR_TCIF0;	// Clear TC interrupt flag
	DMA1_Stream0->CR |= DMA_SxCR_TCIE;	// Разрешаем прерывания от DMA

	arm_hardware_set_handler_realtime(DMA1_Stream0_IRQn, DMA1_Stream0_IRQHandler_codec1_rx);

	DMA1_Stream0->CR |= DMA_SxCR_EN;
}

#if defined (I2S2ext)
// Интерфейс к НЧ кодеку
static void 
hardware_i2s2_i2s2ext_master_duplex_initialize_codec1(void)		/* инициализация I2S на STM32F429 */
{
	// SPI2 - master transmit
	// I2S2ext - slave receive

#if WITHI2SCLOCKFROMPIN
	// тактовая частота на SPI2 (I2S) подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса
	#if defined (STM32F446xx)
		//RCC->DCKCFGR = (RCC->DCKCFGR & ~ (RCC_DCKCFGR_I2S2SRC)) |
		//	1 * RCC_DCKCFGR_I2S2SRC_0 |	// 01: I2S APB2 clock frequency = I2S_CKIN Alternate function input frequency
		//	0;
		RCC->DCKCFGR = (RCC->DCKCFGR & ~ (RCC_DCKCFGR_I2S1SRC)) |
			1 * RCC_DCKCFGR_I2S1SRC_0 |	// 01: I2S APB1 clock frequency = I2S_CKIN Alternate function input frequency
			0;
	#else /* defined (STM32F446xx) */
		RCC->CFGR |= RCC_CFGR_I2SSRC;	// На STM32F446xx этого бита нет
	#endif /* defined (STM32F446xx) */

	//arm_hardware_pioc_altfn50(1u << 9, AF_SPI2);	// PC9 - MCLK source - I2S_CKIN signal - перенесено в процессорно-зависимый header.
#else /* WITHI2SCLOCKFROMPIN */

	// Возможно использовать только режим с MCLK=256*Fs
	//
	// MCK: Master Clock (mapped separately) is used, when the I2S is configured in master
	// mode (and when the MCKOE bit in the SPI_I2SPR register is set), to output this
	// additional clock generated at a preconfigured frequency rate equal to 256 * FS, where
	// FS is the audio sampling frequency.

	#if defined (STM32F446xx)
		#error Reviev STM32F446xx registers programming
	#else /* defined (STM32F446xx) */
		// Частота сравнения та же самая, что и в основной PLL
		// PLLI2SR (at 28) = output divider of VCO frequency
		RCC->PLLI2SCFGR = (RCC->PLLI2SCFGR & ~ (RCC_PLLI2SCFGR_PLLI2SN | RCC_PLLI2SCFGR_PLLI2SR)) |
			((PLLI2SN_MUL << RCC_PLLI2SCFGR_PLLI2SN_Pos) & RCC_PLLI2SCFGR_PLLI2SN) |	// PLLI2SN bits = multiplier, freq=192..432 MHz, vale = 2..432
			2 * RCC_PLLI2SCFGR_PLLI2SR_0 |		// PLLI2SR bits - output divider, 2..7 - константа в calcdivround_plli2s().
			0;
			
		RCC->CR |= RCC_CR_PLLI2SON;				// Включил PLL
		while ((RCC->CR & RCC_CR_PLLI2SRDY) == 0)	// пока заработает PLL
			;
	#endif /* defined (STM32F446xx) */

#endif /* WITHI2SCLOCKFROMPIN */

	// Теперь настроим модуль SPI.
#if CPUSTYLE_STM32H7XX
	RCC->APB1LENR |= RCC_APB1LENR_SPI2EN; // Подать тактирование
	__DSB();
#else /* CPUSTYLE_STM32H7XX */
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; // Подать тактирование
	__DSB();
#endif /* CPUSTYLE_STM32H7XX */
	        
	const portholder_t i2scfgr = stm32xxx_i2scfgr_afcodec();

	I2S2ext->I2SCFGR = i2scfgr | 1 * SPI_I2SCFGR_I2SCFG_0;	// 10: Master - transmit, 11: Master - receive, 01: Slave - receive
 	SPI2->I2SCFGR = i2scfgr | 2 * SPI_I2SCFGR_I2SCFG_0; // 10: Master - transmit, 11: Master - receive
#if WITHI2SCLOCKFROMPIN
	const portholder_t i2sdivider = calcdivround_exti2s(ARMI2SMCLK);
#else /* WITHI2SCLOCKFROMPIN */
	const portholder_t i2sdivider = calcdivround_plli2s(ARMI2SMCLK);
#endif /* WITHI2SCLOCKFROMPIN */
	const portholder_t i2soddv = (i2sdivider % 2) != 0;
	const portholder_t i2sdiv = i2sdivider / 2;

	// note: SPI_I2SPR_I2SDIV should be >= 2
#if WITHI2SCLOCKFROMPIN
	PRINTF(PSTR("hardware_i2s2_i2s2ext_master_duplex_initialize_codec1: 1 I2S i2sdivider=%lu, ARMI2SMCLK=%lu, EXTI2S_FREQ=%lu\n"), (unsigned long) calcdivround_exti2s(ARMI2SMCLK), (unsigned long) ARMI2SMCLK, (unsigned long) EXTI2S_FREQ);
#else /* WITHI2SCLOCKFROMPIN */
	PRINTF(PSTR("hardware_i2s2_i2s2ext_master_duplex_initialize_codec1: 2 I2S i2sdivider=%lu, ARMI2SMCLK=%lu, PLLI2S_FREQ_OUT=%lu\n"), (unsigned long) calcdivround_plli2s(ARMI2SMCLK), (unsigned long) ARMI2SMCLK, (unsigned long) PLLI2S_FREQ_OUT);
#endif /* WITHI2SCLOCKFROMPIN */

#if CPUSTYLE_STM32MP1
	// Done in stm32xxx_i2scfgr_afcodec

#elif CPUSTYLE_STM32H7XX
	// Done in stm32xxx_i2scfgr_afcodec

#else
	const portholder_t i2spr = 
		((i2sdiv << SPI_I2SPR_I2SDIV_Pos) & SPI_I2SPR_I2SDIV) | 
		(SPI_I2SPR_ODD * i2soddv) | 
		SPI_I2SPR_MCKOE |
		0;
	I2S2ext->I2SPR = i2spr;
	SPI2->I2SPR = i2spr;
#endif

#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	SPI2->CFG2 |= SPI_CFG2_AFCNTR_Msk; // 1: the peripheral keeps always control of all associated GPIOs
#endif /* CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1 */
	// Подключить I2S к выводам процессора
	I2S2HW_I2S2EXTHW_MASTER_INITIALIZE();	// hardware_i2s2_i2s2ext_master_duplex_initialize_codec1
}

#endif /* defined (I2S2ext) */


// Интерфейс к НЧ кодеку
static void 
hardware_i2s2_slave_tx_initialize_codec1(void)		/* инициализация I2S2 */
{
	//PRINTF(PSTR("hardware_i2s2_slave_tx_initialize_codec1\n"));

#if CPUSTYLE_STM32MP1
	RCC->MP_APB1ENSETR = RCC_MP_APB1ENSETR_SPI2EN; // Подать тактирование
	(void) RCC->MP_APB1ENSETR;
	RCC->MP_APB1LPENSETR = RCC_MP_APB1LPENSETR_SPI2LPEN; // Подать тактирование
	(void) RCC->MP_APB1LPENSETR;

#elif CPUSTYLE_STM32H7XX
	RCC->APB1LENR |= RCC_APB1LENR_SPI2EN; // Подать тактирование
	(void) RCC->APB1LENR;

#else /* CPUSTYLE_STM32H7XX */
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; // Подать тактирование
	(void) RCC->APB1ENR;

#endif /* CPUSTYLE_STM32H7XX */
	        
	const portholder_t i2scfgr = stm32xxx_i2scfgr_afcodec();

 	SPI2->I2SCFGR = i2scfgr | 0 * SPI_I2SCFGR_I2SCFG_0; // 00: Slave - transmit

#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	SPI2->CFG2 |= SPI_CFG2_AFCNTR_Msk; // 1: the peripheral keeps always control of all associated GPIOs
#endif /* CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1 */

#if CPUSTYLE_STM32H7XX
	SPI2->CFG2 |= SPI_CFG2_IOSWP;	// оставлено тут и не перенесено в I2S2HW_INITIALIZE потому что есть нсаледованная аппаратура и для STM32H7 и для STM32F7
#endif /* CPUSTYLE_STM32H7XX */

	// Подключить I2S к выводам процессора
	I2S2HW_SLAVE_INITIALIZE();

	//PRINTF(PSTR("hardware_i2s2_slave_tx_initialize_codec1 done\n"));
}


// Интерфейс к НЧ кодеку
static void 
hardware_i2s3_slave_rx_initialize_codec1(void)		/* инициализация I2S3 STM32F4xx */
{
	//PRINTF(PSTR("hardware_i2s3_slave_rx_initialize_codec1\n"));

#if CPUSTYLE_STM32MP1
	RCC->MP_APB1ENSETR = RCC_MP_APB1ENSETR_SPI3EN_Msk; // Подать тактирование
	(void) RCC->MP_APB1ENSETR;
	RCC->MP_APB1LPENSETR = RCC_MP_APB1LPENSETR_SPI3LPEN_Msk; // Подать тактирование
	(void) RCC->MP_APB1LPENSETR;
#elif CPUSTYLE_STM32H7XX
	RCC->APB1LENR |= RCC_APB1LENR_SPI3EN_Msk; // Подать тактирование
	(void) RCC->APB1LENR;
#else /* CPUSTYLE_STM32H7XX */
	RCC->APB1ENR |= RCC_APB1ENR_SPI3EN_Msk; // Подать тактирование
	(void) RCC->APB1ENR;
#endif /* CPUSTYLE_STM32H7XX */
	        
	const portholder_t i2scfgr = stm32xxx_i2scfgr_afcodec();

 	SPI3->I2SCFGR = i2scfgr | 1 * SPI_I2SCFGR_I2SCFG_0;	// 10: Master - transmit, 11: Master - receive, 01: Slave - receive

#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	SPI3->CFG2 |= SPI_CFG2_AFCNTR_Msk; // 1: the peripheral keeps always control of all associated GPIOs
#endif /* CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1 */

#if CPUSTYLE_STM32H7XX
	//SPI3->CFG2 |= SPI_CFG2_IOSWP;
#endif /* CPUSTYLE_STM32H7XX */

	// Подключить I2S к выводам процессора
	I2S3HW_SLAVE_RX_INITIALIZE();	// hardware_i2s3_slave_rx_initialize_codec1

	//PRINTF(PSTR("hardware_i2s3_slave_rx_initialize_codec1 done\n"));
}

static void i2c2mclkout(void)
{
#if WITHI2SCLOCKFROMPIN
	// тактовая частота на SPI2 (I2S) подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса
	#if defined (STM32F446xx)
		//RCC->DCKCFGR = (RCC->DCKCFGR & ~ (RCC_DCKCFGR_I2S2SRC)) |
		//	1 * RCC_DCKCFGR_I2S2SRC_0 |	// 01: I2S APB2 clock frequency = I2S_CKIN Alternate function input frequency
		//	0;
		RCC->DCKCFGR = (RCC->DCKCFGR & ~ (RCC_DCKCFGR_I2S1SRC)) |
			1 * RCC_DCKCFGR_I2S1SRC_0 |	// 01: I2S APB1 clock frequency = I2S_CKIN Alternate function input frequency
			0;
		(void) RCC->DCKCFGR;
	#elif CPUSTYLE_STM32MP1
		// RCC SPI/I2S2,3 kernel clock selection register
		//	0x0: pll4_p_ck clock selected as kernel peripheral clock (default after reset)
		//	0x1: pll3_q_ck clock selected as kernel peripheral clock
		//	0x2: I2S_CKIN clock selected as kernel peripheral clock
		//	0x3: per_ck clock selected as kernel peripheral clock
		//	0x4: pll3_r_ck clock selected as kernel peripheral clock
		//	others: reserved, the kernel clock is disabled
		RCC->SPI2S23CKSELR = (RCC->SPI2S23CKSELR & ~ (RCC_SPI2S23CKSELR_SPI23SRC_Msk)) |
			((0x02uL << RCC_SPI2S23CKSELR_SPI23SRC_Pos) & RCC_SPI2S23CKSELR_SPI23SRC_Msk) |
			0;
		(void) RCC->SPI2S23CKSELR;
	#elif CPUSTYLE_STM32H7XX
		// SPI123SEL[2:0]: SPI/I2S1,2 and 3 kernel clock source selection
		//	000: pll1_q_ck clock selected as SPI/I2S1,2 and 3 kernel clock (default after reset)
		//	001: pll2_p_ck clock selected as SPI/I2S1,2 and 3 kernel clock
		//	010: pll3_p_ck clock selected as SPI/I2S1,2 and 3 kernel clock
		//	011: I2S_CKIN clock selected as SPI/I2S1,2 and 3 kernel clock
		//	100: per_ck clock selected as SPI/I2S1,2 and 3 kernel clock
		//	others: reserved, the kernel clock is disabled
		RCC->D2CCIP1R = (RCC->D2CCIP1R & ~ (RCC_D2CCIP1R_SPI123SEL_Msk)) |
			((0x03uL << RCC_D2CCIP1R_SPI123SEL_Pos) & RCC_D2CCIP1R_SPI123SEL_Msk) |
			0;
		(void) RCC->D2CCIP1R;

	#else /* defined (STM32F446xx) */
		RCC->CFGR |= RCC_CFGR_I2SSRC;
		(void) RCC->CFGR;
	#endif /* defined (STM32F446xx) */

#else /* WITHI2SCLOCKFROMPIN */

	// Возможно использовать только режим с MCLK=256*Fs
	#if defined (STM32F446xx)
		RCC->DCKCFGR = (RCC->DCKCFGR & ~ (RCC_DCKCFGR_I2S2SRC)) |
			0 * RCC_DCKCFGR_I2S2SRC_0 |		 // 00: I2S2 clock frequency = f(PLLI2S_R)
			//1 * RCC_DCKCFGR_I2S2SRC_0 |	 // 01: I2S2 clock frequency = I2S_CKIN Alternate function input frequency
			//2 * RCC_DCKCFGR_I2S2SRC_0 |	 // 10: I2S2 clock frequency = HSI/HSE depends on PLLSRC bit (PLLCFGR[22])
			0;
		// Частота сравнения та же самая, что и в основной PLL
		// PLLI2SR (at 28) = output divider of VCO frequency
		RCC->PLLI2SCFGR = (RCC->PLLI2SCFGR & ~ (RCC_PLLI2SCFGR_PLLI2SN | RCC_PLLI2SCFGR_PLLI2SR)) |
			((PLLI2SN_MUL << RCC_PLLI2SCFGR_PLLI2SN_Pos) & RCC_PLLI2SCFGR_PLLI2SN) |	// PLLI2SN bits = multiplier, freq=192..432 MHz, vale = 2..432
			2 * RCC_PLLI2SCFGR_PLLI2SR_0 |		// PLLI2SR bits - output divider, 2..7 - константа в calcdivround_plli2s().
			0;

		RCC->CR |= RCC_CR_PLLI2SON;				// Включил PLL
		while ((RCC->CR & RCC_CR_PLLI2SRDY) == 0)	// пока заработает PLL
			;
	#else /* defined (STM32F446xx) */
		//
		// MCK: Master Clock (mapped separately) is used, when the I2S is configured in master
		// mode (and when the MCKOE bit in the SPI_I2SPR register is set), to output this
		// additional clock generated at a preconfigured frequency rate equal to 256 * FS, where
		// FS is the audio sampling frequency.


		// Частота сравнения та же самая, что и в основной PLL
		// PLLI2SR (at 28) = output divider of VCO frequency
		RCC->PLLI2SCFGR = (RCC->PLLI2SCFGR & ~ (RCC_PLLI2SCFGR_PLLI2SN | RCC_PLLI2SCFGR_PLLI2SR)) |
			((PLLI2SN_MUL << RCC_PLLI2SCFGR_PLLI2SN_Pos) & RCC_PLLI2SCFGR_PLLI2SN) |	// PLLI2SN bits = multiplier, freq=192..432 MHz, vale = 2..432
			2 * RCC_PLLI2SCFGR_PLLI2SR_0 |		// PLLI2SR bits - output divider, 2..7 - константа в calcdivround_plli2s().
			0;

		RCC->CR |= RCC_CR_PLLI2SON;				// Включил PLL
		while ((RCC->CR & RCC_CR_PLLI2SRDY) == 0)	// пока заработает PLL
			;
	#endif /* defined (STM32F446xx) */

#endif /* WITHI2SCLOCKFROMPIN */
}

// Интерфейс к НЧ кодеку
/* инициализация I2S2 STM32MP1 (и возможно STM32H7xx) */
static void
hardware_i2s2_slave_duplex_initialize_codec1(void)
{
	//PRINTF(PSTR("hardware_i2s2_slave_duplex_initialize_codec1\n"));

#if CPUSTYLE_STM32MP1
	RCC->MP_APB1ENSETR = RCC_MP_APB1ENSETR_SPI2EN_Msk; // Подать тактирование
	(void) RCC->MP_APB1ENSETR;
	RCC->MP_APB1LPENSETR = RCC_MP_APB1LPENSETR_SPI2LPEN_Msk; // Подать тактирование
	(void) RCC->MP_APB1LPENSETR;

#elif CPUSTYLE_STM32H7XX
	RCC->APB1LENR |= RCC_APB1LENR_SPI2EN_Msk; // Подать тактирование
	(void) RCC->APB1LENR;

#else /* CPUSTYLE_STM32H7XX */
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN_Msk; // Подать тактирование
	(void) RCC->APB1ENR;

#endif /* CPUSTYLE_STM32H7XX */

	i2c2mclkout();

	const portholder_t i2scfgr = stm32xxx_i2scfgr_afcodec();

 	SPI2->I2SCFGR = i2scfgr |
 			(4u << SPI_I2SCFGR_I2SCFG_Pos) |	// 100: slave - full duplex
			0;

#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	SPI2->CFG2 |= SPI_CFG2_AFCNTR_Msk; // 1: the peripheral keeps always control of all associated GPIOs
#endif /* CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1 */

	// Подключить I2S к выводам процессора
	I2S2HW_SLAVE_INITIALIZE();	// hardware_i2s2_slave_duplex_initialize_codec1

	//PRINTF(PSTR("hardware_i2s2_slave_duplex_initialize_codec1 done\n"));
}


// Интерфейс к НЧ кодеку
/* инициализация I2S2 STM32MP1 (и возможно STM32H7xx) */
static void
hardware_i2s2_master_duplex_initialize_codec1(void)
{
	//PRINTF(PSTR("hardware_i2s2_master_duplex_initialize_codec1\n"));

#if CPUSTYLE_STM32MP1
	RCC->MP_APB1ENSETR = RCC_MP_APB1ENSETR_SPI2EN_Msk; // Подать тактирование
	(void) RCC->MP_APB1ENSETR;
	RCC->MP_APB1LPENSETR = RCC_MP_APB1LPENSETR_SPI2LPEN_Msk; // Подать тактирование
	(void) RCC->MP_APB1LPENSETR;

#elif CPUSTYLE_STM32H7XX
	RCC->APB1LENR |= RCC_APB1LENR_SPI2EN_Msk; // Подать тактирование
	(void) RCC->APB1LENR;

#else /* CPUSTYLE_STM32H7XX */
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN_Msk; // Подать тактирование
	(void) RCC->APB1ENR;

#endif /* CPUSTYLE_STM32H7XX */

	i2c2mclkout();

	const portholder_t i2scfgr = stm32xxx_i2scfgr_afcodec();

 	SPI2->I2SCFGR = i2scfgr |
 			(5u << SPI_I2SCFGR_I2SCFG_Pos) |	// 101: master - full duplex
			0;

#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	SPI2->CFG2 |= SPI_CFG2_AFCNTR_Msk; // 1: the peripheral keeps always control of all associated GPIOs
#endif /* CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1 */

	// Подключить I2S к выводам процессора
	I2S2HW_MASTER_INITIALIZE();	// hardware_i2s2_master_duplex_initialize_codec1

	//PRINTF(PSTR("hardware_i2s2_master_duplex_initialize_codec1 done\n"));
}

/* разрешение работы I2S  */
// Интерфейс к НЧ кодеку
static void
hardware_i2s2_duplex_enable_codec1(uint_fast8_t state)
{
#if defined (I2S2ext)

	I2S2ext->CR2 |= SPI_CR2_RXDMAEN_Msk; // DMA по приему (slave)
	SPI2->CR2 |= SPI_CR2_TXDMAEN_Msk; // DMA по передаче
	I2S2ext->I2SCFGR |= SPI_I2SCFGR_I2SE_Msk;		// I2S enable (slave enabled first)
	(void) I2S2ext->I2SCFGR;
	SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE_Msk;		// I2S enable
	(void) SPI2->I2SCFGR;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	/* SPI/I2S поддерживает дуплекс (и два канала DMA). */
	SPI2->CFG1 |= SPI_CFG1_TXDMAEN_Msk; // DMA по передаче
	SPI2->CFG1 |= SPI_CFG1_RXDMAEN_Msk; // DMA по приёму
	SPI2->CR1 |= SPI_CR1_SPE_Msk;		// I2S enable
	(void) SPI2->CR1;
	SPI2->CR1 |= SPI_CR1_CSTART_Msk;	// I2S run
	(void) SPI2->CR1;

#else /* CPUSTYLE_STM32H7XX */

	SPI2->CR2 |= SPI_CR2_TXDMAEN_Msk; // DMA по передаче
	SPI2->CR2 |= SPI_CR2_RXDMAEN_Msk; // DMA по передаче
	(void) SPI2->CR2;
	SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE_Msk;		// I2S enable
	(void) SPI2->I2SCFGR;

#endif /* CPUSTYLE_STM32H7XX */
}

/* разрешение работы I2S на STM32F4xx */
// Интерфейс к НЧ кодеку
static void
hardware_i2s2_tx_enable_codec1(uint_fast8_t state)
{
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	SPI2->CFG1 |= SPI_CFG1_TXDMAEN_Msk; // DMA по передаче
	SPI2->CR1 |= SPI_CR1_SPE_Msk;		// I2S enable
	SPI2->CR1 |= SPI_CR1_CSTART_Msk;	// I2S run
	__DSB();
#else /* CPUSTYLE_STM32H7XX */
	SPI2->CR2 |= SPI_CR2_TXDMAEN_Msk; // DMA по передаче
	SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE_Msk;		// I2S enable
	__DSB();
#endif /* CPUSTYLE_STM32H7XX */
}

/* разрешение работы I2S на STM32F4xx */
// Интерфейс к НЧ кодеку
static void 
hardware_i2s3_rx_enable_codec1(uint_fast8_t state)
{
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	SPI3->CFG1 |= SPI_CFG1_RXDMAEN_Msk; // DMA по приёму
	SPI3->CR1 |= SPI_CR1_SPE_Msk;		// I2S enable
	SPI3->CR1 |= SPI_CR1_CSTART_Msk;	// I2S run
	__DSB();

#else /* CPUSTYLE_STM32H7XX */

	SPI3->CR2 |= SPI_CR2_RXDMAEN_Msk; // DMA по приёму
	SPI3->I2SCFGR |= SPI_I2SCFGR_I2SE_Msk;		// I2S enable
	__DSB();

#endif /* CPUSTYLE_STM32H7XX */
}


/* разрешение работы I2S на STM32F4xx */
// Интерфейс к НЧ кодеку
static void
hardware_i2s2_rx_enable(uint_fast8_t state)
{
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	SPI2->CFG1 |= SPI_CFG1_RXDMAEN_Msk; // DMA по приёму
	SPI2->CR1 |= SPI_CR1_SPE_Msk;		// I2S enable
	SPI2->CR1 |= SPI_CR1_CSTART_Msk;	// I2S run
	__DSB();

#else /* CPUSTYLE_STM32H7XX */

	SPI2->CR2 |= SPI_CR2_RXDMAEN_Msk; // DMA по приёму
	SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE_Msk;		// I2S enable
	__DSB();

#endif /* CPUSTYLE_STM32H7XX */
}



#if WITHI2SI2S2EXTFULLDUPLEXHW

// платы, где используются DMA_I2S2 и DMA_I2S2ext
static const codechw_t audiocodechw_i2s2_i2s2ext_duplex_master =
{
	hardware_i2s2_i2s2ext_master_duplex_initialize_codec1,
	hardware_dummy_initialize,
	DMA_I2S2ext_rx_init_audio,			// DMA по приёму канал 3
	DMA_I2S2_TX_initialize_codec1,				// DMA по передаче канал 0
	hardware_i2s2_duplex_enable_codec1,
	hardware_dummy_enable,
	"audiocodechw-i2s2-i2s2ext-duplex-master"
};

#else /* WITHI2SI2S2EXTFULLDUPLEXHW */

// Используется I2S2 в дуплексном режиме
static const codechw_t audiocodechw_i2s2_duplex_slave =
{
	hardware_i2s2_slave_duplex_initialize_codec1,	/* Интерфейс к НЧ кодеку - микрофон и наушники */
	hardware_dummy_initialize,
	DMA_I2S2_RX_initialize_codec1,					// DMA по приёму SPI2_RX
	DMA_I2S2_TX_initialize_codec1,					// DMA по передаче SPI2_TX
	hardware_i2s2_duplex_enable_codec1,
	hardware_dummy_enable,
	"audiocodechw-i2s2-duplex-slave"
};

// Используется I2S2 в дуплексном режиме
static const codechw_t audiocodechw_i2s2_duplex_master =
{
	hardware_i2s2_master_duplex_initialize_codec1,	/* Интерфейс к НЧ кодеку - микрофон и наушники */
	hardware_dummy_initialize,
	DMA_I2S2_RX_initialize_codec1,					// DMA по приёму SPI2_RX
	DMA_I2S2_TX_initialize_codec1,					// DMA по передаче SPI2_TX
	hardware_i2s2_duplex_enable_codec1,
	hardware_dummy_enable,
	"audiocodechw-i2s2-duplex-master"
};

#endif /* WITHI2SI2S2EXTFULLDUPLEXHW */

static const codechw_t audiocodechw_i2s2_tx_slave =
{
	hardware_dummy_initialize,	/* Интерфейс к НЧ кодеку - микрофон */
	hardware_i2s2_slave_tx_initialize_codec1,	/* Интерфейс к НЧ кодеку - наушники */
	hardware_dummy_initialize,					// DMA по приёму SPI3_RX - DMA1, Stream0, Channel0
	DMA_I2S2_TX_initialize_codec1,					// DMA по передаче канал 0
	hardware_dummy_enable,
	hardware_i2s2_tx_enable_codec1,
	"audiocodechw-i2s2-tx-slave"
};

static const codechw_t audiocodechw_i2s3_rx_slave =
{
	hardware_i2s3_slave_rx_initialize_codec1,	/* Интерфейс к НЧ кодеку - микрофон */
	hardware_dummy_initialize,	/* Интерфейс к НЧ кодеку - наушники */
	DMA_I2S3_RX_initialize_codec1,					// DMA по приёму SPI3_RX - DMA1, Stream0, Channel0
	hardware_dummy_initialize,					// DMA по передаче канал 0
	hardware_i2s3_rx_enable_codec1,
	hardware_dummy_enable,
	"audiocodechw-i2s3-rx-slave"
};


#if WITHSAI1HW || WITHSAI2HW || WITHSAI3HW || WITHSAI4HW

	#if WITHSAICLOCKFROMPIN
		static uint_fast32_t 
		NOINLINEAT
		calcdivround_extsai(
			uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
			)
		{
			return (EXTSAI_FREQ + freq / 2) / freq;
		}
	#elif WITHSAICLOCKFROMI2S

	#else
		static uint_fast32_t 
		NOINLINEAT
		calcdivround_pllsai(
			uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
			)
		{
			return (PLLSAI_FREQ_OUT + freq / 2) / freq;
		}
	#endif


	/*
		RM0090, SAI xConfiguration register 1 (SAI_xCR1) (chapter 29.17.1).
		MCKDIV[3:0] value description:

		0000: Divides by 1 the master clock input.
		Otherwise, The Master clock frequency is calculated accordingly to the following formula:
		MCLK_x = SAI_CK_x / (MCKDIV[3:0] * 2)
	*/
	static uint32_t mckdivform(uint32_t divider)
	{
		if (divider == 1)
			return 0;
		return divider / 2;
	}

static void hardware_sai1_sai2_clock_selection(void)
{
	//PRINTF(PSTR("hardware_sai1_sai2_clock_selection\n"));

#if WITHSAICLOCKFROMPIN
	
	// Тактовая частота на SAI1 подается с внешнего генератора, в процессор вводится
	// через MCK сигнал интерфейса.
	// SAI2 синхронизируется от SAI1

	#if defined (STM32F446xx)
		// clock sources:
		// RCC_DCKCFGR_SAI1SRC:
		// 0: SAI1 clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ
		// 1: SAI1 clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
		// 2: SAI1 clock frequency = f(PLL_R)
		// 3: I2S_CKIN Alternate function input frequency
		// RCC_DCKCFGR_SAI2SRC
		// 0: SAI2 clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ
		// 1: SAI2 clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
		// 2: SAI2 clock frequency = f(PLL_R)
		// 3: SAI2 clock frequency = HSI/HSE depends on PLLSRC (PLLCFGR[22])

		// SAI part of DCKCFGR
		RCC->DCKCFGR = (RCC->DCKCFGR & ~ (RCC_DCKCFGR_SAI1SRC | RCC_DCKCFGR_SAI2SRC)) |
			(3 * RCC_DCKCFGR_SAI1SRC_0) |	// I2S APB1 clock source selection
			(3 * RCC_DCKCFGR_SAI2SRC_0) |	// I2S APB2 clock source selection
			0;

	#elif CPUSTYLE_STM32F7XX
		// moved to lowlevel_stm32f7xx_pll_initialize
		//
		/*
		RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_I2SSRC)) |
				1 * RCC_CFGR_I2SSRC |	// 1: External clock mapped on the I2S_CKIN pin used as I2S clock source
				0;
		// SAI part of DCKCFGR1
		RCC->DCKCFGR1 = (RCC->DCKCFGR1 & ~ (RCC_DCKCFGR1_SAI1SEL | RCC_DCKCFGR1_SAI2SEL)) |
			(2 * RCC_DCKCFGR1_SAI1SEL_0) |	// 10: SAI1 clock frequency = Alternate function input frequency
			(2 * RCC_DCKCFGR1_SAI2SEL_0) |	// 10: SAI2 clock frequency = Alternate function input frequency
			0;
		*/
	#elif CPUSTYLE_STM32H7XX
		// clock sources:
		//	000: pll1_q_ck clock selected as SAI1 and DFSDM1 Aclk kernel clock (default after reset)
		//	001: pll2_p_ck clock selected as SAI1 and DFSDM1 Aclk kernel clock
		//	010: pll3_p_ck clock selected as SAI1 and DFSDM1 Aclk kernel clock
		//	011: I2S_CKIN clock selected as SAI1 and DFSDM1 Aclk kernel clock
		//	100: per_ck clock selected as SAI1 and DFSDM1 Aclk kernel clock
		//	others: reserved, the kernel clock is disabled
		//	Note: I2S_CKIN is an external clock taken from a pin.
		RCC->D2CCIP1R = (RCC->D2CCIP1R & ~ (RCC_D2CCIP1R_SAI1SEL_Msk | RCC_D2CCIP1R_SAI23SEL_Msk)) |
			(3u << RCC_D2CCIP1R_SAI1SEL_Pos) |		// SAI1 clock source selection
			(3u << RCC_D2CCIP1R_SAI23SEL_Pos) |	// SAI2, SAI3 clock source selection
			0;
		(void) RCC->D2CCIP1R;

	#elif CPUSTYLE_STM32MP1
		// clock sources:
		//	0x0: pll4_q_ck clock selected as kernel peripheral clock (default after reset)
		//	0x1: pll3_q_ck clock selected as kernel peripheral clock
		//	0x2: I2S_CKIN clock selected as kernel peripheral clock
		//	0x3: per_ck clock selected as kernel peripheral clock
		//	0x4: pll3_r_ck clock selected as kernel peripheral clock
		//	others: reserved, the kernel clock is disabled

		RCC->SAI1CKSELR = (RCC->SAI1CKSELR & ~ (RCC_SAI1CKSELR_SAI1SRC_Msk)) |
				(2u << RCC_SAI1CKSELR_SAI1SRC_Pos) |
				0;
		(void) RCC->SAI1CKSELR;

		//	0x0: pll4_q_ck clock selected as kernel peripheral clock (default after reset)
		//	0x1: pll3_q_ck clock selected as kernel peripheral clock
		//	0x2: I2S_CKIN clock selected as kernel peripheral clock
		//	0x3: per_ck clock selected as kernel peripheral clock
		//	0x4: spdif_ck_symb clock from SPDIFRX selected as kernel peripheral clock
		//	0x5: pll3_r_ck clock selected as kernel peripheral clock
		//	others: reserved, the kernel clock is disabled

		RCC->SAI2CKSELR = (RCC->SAI2CKSELR & ~ (RCC_SAI2CKSELR_SAI2SRC_Msk)) |
				(2u << RCC_SAI2CKSELR_SAI2SRC_Pos) |
				0;
		(void) RCC->SAI2CKSELR;

		//	0x0: pll4_q_ck clock selected as kernel peripheral clock (default after reset)
		//	0x1: pll3_q_ck clock selected as kernel peripheral clock
		//	0x2: I2S_CKIN clock selected as kernel peripheral clock
		//	0x3: per_ck clock selected as kernel peripheral clock
		//	0x4: pll3_r_ck clock selected as kernel peripheral clock
		//	others: reserved, the kernel clock is disabled

		RCC->SAI3CKSELR = (RCC->SAI3CKSELR & ~ (RCC_SAI3CKSELR_SAI3SRC_Msk)) |
				(2u << RCC_SAI3CKSELR_SAI3SRC_Pos) |
				0;
		(void) RCC->SAI3CKSELR;

	#else /* defined (STM32F446xx) */
		// clock sources:
		// 0: SAI1-B clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ
		// 1: SAI1-B clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
		// 2: SAI1-B clock frequency = Alternate function input frequency

		// SAI part of DCKCFGR
		RCC->DCKCFGR = (RCC->DCKCFGR & ~ (RCC_DCKCFGR_SAI1ASRC | RCC_DCKCFGR_SAI1BSRC)) |
			(2 * RCC_DCKCFGR_SAI1ASRC_0) |	// SAI1-A clock source selection
			(2 * RCC_DCKCFGR_SAI1BSRC_0) |	// SAI1-B clock source selection
			0;

	#endif /* defined (STM32F446xx) */

#elif WITHSAICLOCKFROMI2S
		// from I2S PLL
	#if defined (STM32F446xx)
		// clock sources:
		// RCC_DCKCFGR_SAI1SRC:
		// 0: SAI1 clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ
		// 1: SAI1 clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
		// 2: SAI1 clock frequency = f(PLL_R)
		// 3: I2S_CKIN Alternate function input frequency
		// RCC_DCKCFGR_SAI2SRC
		// 0: SAI2 clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ
		// 1: SAI2 clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
		// 2: SAI2 clock frequency = f(PLL_R)
		// 3: SAI2 clock frequency = HSI/HSE depends on PLLSRC (PLLCFGR[22])

		//#error Reviev STM32F446xx registers programming
		// SAI part of DCKCFGR
		RCC->DCKCFGR = (RCC->DCKCFGR & ~ (RCC_DCKCFGR_SAI1SRC | RCC_DCKCFGR_SAI2SRC)) |
			(1 * RCC_DCKCFGR_SAI1SRC_0) |	// I2S APB1 clock source selection
			(1 * RCC_DCKCFGR_SAI2SRC_0) |	// I2S APB2 clock source selection
			0;

	#elif CPUSTYLE_STM32F7XX
		// clock sources:
		// RCC_DCKCFGR_SAI1SRC:
		// 0: SAI1 clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ
		// 1: SAI1 clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
		// 2: SAI1 clock frequency = f(PLL_R)
		// 3: I2S_CKIN Alternate function input frequency
		// RCC_DCKCFGR_SAI2SRC
		// 0: SAI2 clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ
		// 1: SAI2 clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
		// 2: SAI2 clock frequency = f(PLL_R)
		// 3: SAI2 clock frequency = HSI/HSE depends on PLLSRC (PLLCFGR[22])

		//#error Reviev STM32F446xx registers programming
		// SAI part of DCKCFGR
		RCC->DCKCFGR1 = (RCC->DCKCFGR1 & ~ (RCC_DCKCFGR1_SAI1SEL | RCC_DCKCFGR1_SAI2SEL)) |
			(1 * RCC_DCKCFGR1_SAI1SEL_0) |	// SAI1 clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
			(1 * RCC_DCKCFGR1_SAI2SEL_0) |	// SAI2 clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
			0;

	#elif CPUSTYLE_STM32H7XX
		// clock sources:
		//	000: pll1_q_ck clock selected as SAI1 and DFSDM1 Aclk kernel clock (default after reset)
		//	001: pll2_p_ck clock selected as SAI1 and DFSDM1 Aclk kernel clock
		//	010: pll3_p_ck clock selected as SAI1 and DFSDM1 Aclk kernel clock
		//	011: I2S_CKIN clock selected as SAI1 and DFSDM1 Aclk kernel clock
		//	100: per_ck clock selected as SAI1 and DFSDM1 Aclk kernel clock
		//	others: reserved, the kernel clock is disabled
		//	Note: I2S_CKIN is an external clock taken from a pin.
		RCC->D2CCIP1R = (RCC->D2CCIP1R & ~ (RCC_D2CCIP1R_SAI1SEL | RCC_D2CCIP1R_SAI23SEL)) |
			(1u << RCC_D2CCIP1R_SAI1SEL_Pos) |	// PLL2 Q
			(1u << RCC_D2CCIP1R_SAI23SEL_Pos) |
			0;

	#elif CPUSTYLE_STM32MP1
		#warning TODO: implement for CPUSTYLE_STM32MP1
		// clock sources:
		//	0x0: pll4_q_ck clock selected as kernel peripheral clock (default after reset)
		//	0x1: pll3_q_ck clock selected as kernel peripheral clock
		//	0x2: I2S_CKIN clock selected as kernel peripheral clock
		//	0x3: per_ck clock selected as kernel peripheral clock
		//	0x4: pll3_r_ck clock selected as kernel peripheral clock
		//	others: reserved, the kernel clock is disabled

		RCC->SAI1CKSELR = (RCC->SAI1CKSELR & ~ (RCC_SAI1CKSELR_SAI1SRC_Msk)) |
				(XuL << RCC_SAI1CKSELR_SAI1SRC_Pos) |
				0;
		(void) RCC->SAI1CKSELR;

		//	0x0: pll4_q_ck clock selected as kernel peripheral clock (default after reset)
		//	0x1: pll3_q_ck clock selected as kernel peripheral clock
		//	0x2: I2S_CKIN clock selected as kernel peripheral clock
		//	0x3: per_ck clock selected as kernel peripheral clock
		//	0x4: spdif_ck_symb clock from SPDIFRX selected as kernel peripheral clock
		//	0x5: pll3_r_ck clock selected as kernel peripheral clock
		//	others: reserved, the kernel clock is disabled

		RCC->SAI2CKSELR = (RCC->SAI2CKSELR & ~ (RCC_SAI2CKSELR_SAI2SRC_Msk)) |
				(XuL << RCC_SAI2CKSELR_SAI2SRC_Pos) |
				0;
		(void) RCC->SAI2CKSELR;

		//	0x0: pll4_q_ck clock selected as kernel peripheral clock (default after reset)
		//	0x1: pll3_q_ck clock selected as kernel peripheral clock
		//	0x2: I2S_CKIN clock selected as kernel peripheral clock
		//	0x3: per_ck clock selected as kernel peripheral clock
		//	0x4: pll3_r_ck clock selected as kernel peripheral clock
		//	others: reserved, the kernel clock is disabled

		RCC->SAI3CKSELR = (RCC->SAI3CKSELR & ~ (RCC_SAI3CKSELR_SAI3SRC_Msk)) |
				(XuL << RCC_SAI3CKSELR_SAI3SRC_Pos) |
				0;
		(void) RCC->SAI3CKSELR;

	#else /* defined (STM32F446xx) */
		RCC->PLLI2SCFGR = 
			(RCC->PLLI2SCFGR & ~ (RCC_PLLI2SCFGR_PLLI2SQ)) |
			(2 * RCC_PLLI2SCFGR_PLLI2SQ_0) |	// PLLI2SQ: PLLI2S division factor for SAI1 clock - 2..7 - эта константа используется в calcdivround_pllsai().
			0;
		// clock sources:
		// 0: SAI1-B clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ
		// 1: SAI1-B clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
		// 2: SAI1-B clock frequency = Alternate function input frequency
		// SAI part of DCKCFGR
		RCC->DCKCFGR = (RCC->DCKCFGR & ~ (RCC_DCKCFGR_SAI1ASRC | RCC_DCKCFGR_SAI1BSRC)) |
			(1 * RCC_DCKCFGR_SAI1ASRC_0) |	// SAI1-A clock source selection
			(1 * RCC_DCKCFGR_SAI1BSRC_0) |	// SAI1-B clock source selection
			0;
	#endif /* defined (STM32F446xx) */

#else /* WITHSAICLOCKFROMI2S */
	// Для SAI используется отдельная PLL
	// Частота сравнения та же самая, что и в основной PLL
	// PLLSAIDIVQ
	#if defined (STM32F446xx)
		// clock sources:
		// RCC_DCKCFGR_SAI1SRC:
		// 0: SAI1 clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ
		// 1: SAI1 clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
		// 2: SAI1 clock frequency = f(PLL_R)
		// 3: I2S_CKIN Alternate function input frequency
		// RCC_DCKCFGR_SAI2SRC
		// 0: SAI2 clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ
		// 1: SAI2 clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
		// 2: SAI2 clock frequency = f(PLL_R)
		// 3: SAI2 clock frequency = HSI/HSE depends on PLLSRC (PLLCFGR[22])
		//#error Reviev STM32F446xx registers programming

		// SAI part of DCKCFGR
		RCC->DCKCFGR = (RCC->DCKCFGR & ~ (RCC_DCKCFGR_SAI1SRC | RCC_DCKCFGR_SAI2SRC)) |
			(0 * RCC_DCKCFGR_SAI1SRC_0) |	// I2S APB1 clock source selection
			(0 * RCC_DCKCFGR_SAI2SRC_0) |	// I2S APB2 clock source selection
			0;

	#elif CPUSTYLE_STM32F7XX
		// setup PLLSAI
		RCC->PLLSAICFGR = 
			(RCC->PLLSAICFGR & ~ (RCC_PLLSAICFGR_PLLSAIN | RCC_PLLSAICFGR_PLLSAIQ)) |
			((SAIREF1_MUL << RCC_PLLSAICFGR_PLLSAIN_Pos) & RCC_PLLSAICFGR_PLLSAIN) |	// PLLI2SN bits = multiplier, freq=192..432 MHz, vale = 2..432
			((2u << RCC_PLLSAICFGR_PLLSAIQ_Pos) & RCC_PLLSAICFGR_PLLSAIQ) |	// PLLSAI division factor for SAI1 clock: 2..15 - эта константа используется в calcdivround_pllsai().
			0;
		// clock sources:
		// RCC_DCKCFGR_SAI1SRC:
		// 0: SAI1 clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ
		// 1: SAI1 clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
		// 2: SAI1 clock frequency = f(PLL_R)
		// 3: I2S_CKIN Alternate function input frequency
		// RCC_DCKCFGR_SAI2SRC
		// 0: SAI2 clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ
		// 1: SAI2 clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
		// 2: SAI2 clock frequency = f(PLL_R)
		// 3: SAI2 clock frequency = HSI/HSE depends on PLLSRC (PLLCFGR[22])
		//#error Reviev STM32F446xx registers programming

		// SAI part of DCKCFGR
		RCC->DCKCFGR1 = (RCC->DCKCFGR1 & ~ (RCC_DCKCFGR1_SAI1SEL | RCC_DCKCFGR1_SAI2SEL)) |
			(0 * RCC_DCKCFGR1_SAI1SEL_0) |	// SAI1 clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ 
			(0 * RCC_DCKCFGR1_SAI2SEL_0) |	// SAI2 clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ 
			0;

		RCC->CR |= RCC_CR_PLLSAION;				// Включил PLL
		while ((RCC->CR & RCC_CR_PLLSAIRDY) == 0)	// пока заработает PLL
			;

	#else /* defined (STM32F446xx) */
		// setup PLLSAI
		RCC->PLLSAICFGR = 
			(RCC->PLLSAICFGR & ~ (RCC_PLLSAICFGR_PLLSAIN | RCC_PLLSAICFGR_PLLSAIQ)) |
			((SAIREF1_MUL << RCC_PLLSAICFGR_PLLSAIN_Pos) & RCC_PLLSAICFGR_PLLSAIN) |	// PLLI2SN bits = multiplier, freq=192..432 MHz, vale = 2..432
			((2u << RCC_PLLSAICFGR_PLLSAIQ_Pos) & RCC_PLLSAICFGR_PLLSAIQ) |	// PLLSAI division factor for SAI1 clock: 2..15 - эта константа используется в calcdivround_pllsai().
			0;
		// clock sources:
		// 0: SAI1-B clock frequency = f(PLLSAI_Q) / PLLSAIDIVQ
		// 1: SAI1-B clock frequency = f(PLLI2S_Q) / PLLI2SDIVQ
		// 2: SAI1-B clock frequency = Alternate function input frequency
		// SAI part of DCKCFGR
		RCC->DCKCFGR = (RCC->DCKCFGR & ~ (RCC_DCKCFGR_PLLSAIDIVQ | RCC_DCKCFGR_SAI1ASRC | RCC_DCKCFGR_SAI1BSRC)) |
			((0UL << 8) & RCC_DCKCFGR_PLLSAIDIVQ) |	// PLLSAI division factor for SAI1 clock 0..31, dividers = 1..32
			(0 * RCC_DCKCFGR_SAI1ASRC_0) |	// SAI1-A clock source selection
			(0 * RCC_DCKCFGR_SAI1BSRC_0) |	// SAI1-B clock source selection
			0;

		RCC->CR |= RCC_CR_PLLSAION;				// Включил PLL
		while ((RCC->CR & RCC_CR_PLLSAIRDY) == 0)	// пока заработает PLL
			;
	#endif /* defined (STM32F446xx) */
#endif /* WITHSAICLOCKFROMI2S */
	//PRINTF(PSTR("hardware_sai1_sai2_clock_selection done\n"));
}

static void hardware_sai_a_tx_b_rx_initialize_codec1(SAI_Block_TypeDef * sai_A, SAI_Block_TypeDef * sai_B, int master)
{
	sai_A->CR1 &= ~ SAI_xCR1_SAIEN;
	sai_B->CR1 &= ~ SAI_xCR1_SAIEN;

	// extclock = 24.576 MHz
	// if SAI_xCR1_NODIV == 1, ws=384 kHz
	//const uint32_t mckval = 0;	// divider = 1, ws = 96 kHz
	//const uint32_t mckval = 1;	// divider = 2, ws = 48 kHz
	//const uint32_t mckval = 2;	// divider = 4, ws = 24 kHz
	//const uint32_t mckval = 3;	// divider = 6, ws = 16 kHz
	//const uint32_t mckval = 4;	// divider = 8, ws = 12 kHz

	// SAI_xCR1_CKSTR:
	// 0: Signals generated by the SAI change on SCK rising edge,
	//		while signals received by the SAI are sampled on the SCK falling edge.
	// 1: Signals generated by the SAI change on SCK falling edge,
	//		while signals received by the SAI are sampled on the SCK rising edge.
	const uint_fast8_t nodiv = 0;
#if WITHSAICLOCKFROMPIN
	const uint_fast8_t mckdiv = mckdivform(calcdivround_extsai(ARMSAIMCLK));
#elif WITHSAICLOCKFROMI2S
	const uint_fast8_t mckdiv = mckdivform(calcdivround_plli2s(ARMSAIMCLK));
#else
		// from own PLL
	const uint_fast8_t mckdiv = mckdivform(calcdivround_pllsai(ARMSAIMCLK));
#endif
	// CR1 value
	const portholder_t commoncr1 =
		(1 * SAI_xCR1_OUTDRIV) |	//
		//(0 * SAI_xCR1_MONO) |	// stereo MODE - with IQ pairs - see DMABUFFSIZE32
		(1 * SAI_xCR1_DMAEN) |	// 1: DMA enable
		(1 * SAI_xCR1_CKSTR) |	// 0: данные на выходе меняются по нарастающему фронту, 1: по спадающему
		(SAIAUDIO_SAI_CR1_DS * SAI_xCR1_DS_0) |	// 6: data size - 24 bit, 7: 32 bit, 4: 16 bit
		(0 * SAI_xCR1_PRTCFG_0) |	// 0: free protocol
		(nodiv * SAI_xCR1_NODIV) |	// 1: no MCKDIV value
		((mckdiv * SAI_xCR1_MCKDIV_0) & SAI_xCR1_MCKDIV) |	// MCKDIV vale = 4 bits
		0;

#if WITHSAICLOCKFROMPIN
	PRINTF(PSTR("hardware_saiX_a_tx_b_rx_initialize_codec1: 1 SAIx MCKDIV=%lu, ARMSAIMCLK=%lu, EXTSAI_FREQ=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) EXTSAI_FREQ);
#elif WITHSAICLOCKFROMI2S
	PRINTF(PSTR("hardware_saiX_a_tx_b_rx_initialize_codec1: 2 SAIx MCKDIV=%lu, ARMSAIMCLK=%lu, PLLI2S_FREQ_OUT=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) PLLI2S_FREQ_OUT);
#else
	PRINTF(PSTR("hardware_saiX_a_tx_b_rx_initialize_codec1: 3 SAIx MCKDIV=%lu, ARMSAIMCLK=%lu, PLLSAI_FREQ_OUT=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) PLLSAI_FREQ_OUT);
#endif

	sai_A->CR1 =
		commoncr1 |
		(0 * SAI_xCR1_SYNCEN_0) |	// SYNChronization ENable: 0: audio sub-block in asynchronous mode.
		((master ? 0 : 2) * SAI_xCR1_MODE_0) |	// 0: Master transmitter, 1: Master receiver, 2: Slave transmitter, 3: Slave receiver
		0;
	sai_B->CR1 =
		commoncr1 |
		(1 * SAI_xCR1_SYNCEN_0) |	// SYNChronization ENable: audio sub-block is synchronous with the other internal audio sub-block. In this case, the audio sub-block must be configured in slave mode
		((master ? 3 : 3) * SAI_xCR1_MODE_0) |		// 0: Master transmitter, 1: Master receiver, 2: Slave transmitter, 3: Slave receiver
		0;

	// CR2 value
	const portholder_t commoncr2 =
		//(2 * SAI_xCR2_FTH_0) |
		0;
	sai_A->CR2 =
		commoncr2 |
		0;
	sai_B->CR2 =
		commoncr2 |
		0;
	// SLOTR value
	const portholder_t commonslotr =
		(0 * SAI_xSLOTR_SLOTSZ_0) |	// 00: The slot size is equivalent to the data size (specified in DS[3:0] in the SAI_xCR1 register).
		//SAI_xSLOTR_SLOTEN |			// all slots enabled
		//(3UL << 16) |
		//(1 * SAI_xSLOTR_FBOFF_0) | // slot offset - "1" for I2S 24 bit in 32 bit slot
		0;
	sai_A->SLOTR =
		((SLOTEN_TX_SAIAUDIO << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk) |			// TX slots enabled
		((NBSLOT_TX_SAIAUDIO - 1) * SAI_xSLOTR_NBSLOT_0) | // Number of slots in audio Frame
		commonslotr |
		0;
	sai_B->SLOTR =
		((SLOTEN_RX_SAIAUDIO << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk) |			// RX slots enabled
		((NBSLOT_RX_SAIAUDIO - 1) * SAI_xSLOTR_NBSLOT_0) | // Number of slots in audio Frame
		commonslotr |
		0;

	// FRCR value
	const portholder_t comm_frcr =
		((CODEC1_FRAMEBITS - 1) * SAI_xFRCR_FRL_0) |
		(1 * SAI_xFRCR_FSDEF) |		/* FS signal is a start of frame signal + channel side identification - must be set for I2S */
		((CODEC1_FRAMEBITS / 2 - 1) * SAI_xFRCR_FSALL_0) |
#if CODEC1_FORMATI2S_PHILIPS
		(1 * SAI_xFRCR_FSOFF) |		/* 1: FS is asserted one bit before the first bit of the slot 0. Classic I2S form Philips Semiconductors. "0" value for Sony I2S specs. */
		(0 * SAI_xFRCR_FSPO) |	/* 0: канал с индексом 0 передается при "0" на WS - перывм. */
#endif /* CODEC1_FORMATI2S_PHILIPS */
		0;
	// FRAME CONTROL REGISTER
	sai_A->FRCR =
		comm_frcr |
		0;
	sai_B->FRCR =
		comm_frcr |
		0;
}

static void hardware_sai_a_tx_b_rx_initialize_fpga(SAI_Block_TypeDef * sai_A, SAI_Block_TypeDef * sai_B, int master)
{
	sai_A->CR1 &= ~ SAI_xCR1_SAIEN;
	sai_B->CR1 &= ~ SAI_xCR1_SAIEN;

	// extclock = 24.576 MHz
	// if SAI_xCR1_NODIV == 1, ws=384 kHz
	//const uint32_t mckval = 0;	// divider = 1, ws = 96 kHz
	//const uint32_t mckval = 1;	// divider = 2, ws = 48 kHz
	//const uint32_t mckval = 2;	// divider = 4, ws = 24 kHz
	//const uint32_t mckval = 3;	// divider = 6, ws = 16 kHz
	//const uint32_t mckval = 4;	// divider = 8, ws = 12 kHz
	//
	// SAI_xCR1_CKSTR:
	// 0: Signals generated by the SAI change on SCK rising edge,
	//		while signals received by the SAI are sampled on the SCK falling edge.
	// 1: Signals generated by the SAI change on SCK falling edge,
	//		while signals received by the SAI are sampled on the SCK rising edge.
#if WITHSAICLOCKFROMPIN
	const uint_fast8_t nodiv = 0;
	const uint_fast8_t mckdiv = 2; //mckdivform(calcdivround_extsai(ARMSAIMCLK));
#elif WITHSAICLOCKFROMI2S
	//const uint_fast8_t nodiv = 0;
	//const uint_fast8_t mckdiv = mckdivform(calcdivround_plli2s(ARMSAIMCLK));
#else
	// from own PLL
	//const uint_fast8_t nodiv = 0;
	//const uint_fast8_t mckdiv = mckdivform(calcdivround_pllsai(ARMSAIMCLK));
#endif

	// CR1 value
	const portholder_t commoncr1 =
		(1 * SAI_xCR1_OUTDRIV) |	//
		//(0 * SAI_xCR1_MONO) |	// stereo MODE - with IQ pairs - see DMABUFFSIZE32
		(1 * SAI_xCR1_DMAEN) |	// 1: DMA enable
		(1 * SAI_xCR1_CKSTR) |	// 0: данные на выходе меняются по нарастающему фронту, 1: по спадающему
		(FPGAIF_SAI_CR1_DS * SAI_xCR1_DS_0) |	// 6: data size - 24 bit, 7: 32 bit, 4: 16 bit
		(0 * SAI_xCR1_PRTCFG_0) |	// 0: free protocol
		(nodiv * SAI_xCR1_NODIV) |	// 1: no MCKDIV value
		((mckdiv * SAI_xCR1_MCKDIV_0) & SAI_xCR1_MCKDIV) |	// MCKDIV vale = 4 bits
		0;

#if WITHSAICLOCKFROMPIN
	//PRINTF(PSTR("hardware_sai1_slave_duplex_initialize_fpga: 1 SAI2 MCKDIV=%lu, ARMSAIMCLK=%lu, EXTSAI_FREQ=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) EXTSAI_FREQ);
#elif WITHSAICLOCKFROMI2S
	//PRINTF(PSTR("hardware_sai1_slave_duplex_initialize_fpga: 2 SAI2 MCKDIV=%lu, ARMSAIMCLK=%lu, PLLI2S_FREQ_OUT=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) PLLI2S_FREQ_OUT);
#else
	//PRINTF(PSTR("hardware_sai1_slave_duplex_initialize_fpga: 3 SAI2 MCKDIV=%lu, ARMSAIMCLK=%lu, PLLSAI_FREQ_OUT=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) PLLSAI_FREQ_OUT);
#endif

	sai_A->CR1 =
		commoncr1 |
		(0 * SAI_xCR1_SYNCEN_0) |	// SYNChronization ENable: 0: audio sub-block in asynchronous mode.
		((master ? 0 : 2) * SAI_xCR1_MODE_0) |	// 0: Master transmitter, 1: Master receiver, 2: Slave transmitter, 3: Slave receiver
		0;
	sai_B->CR1 =
		commoncr1 |
		(1 * SAI_xCR1_SYNCEN_0) |	// SYNChronization ENable: audio sub-block is synchronous with the other internal audio sub-block. In this case, the audio sub-block must be configured in slave mode
		((master ? 3 : 3) * SAI_xCR1_MODE_0) |		// 0: Master transmitter, 1: Master receiver, 2: Slave transmitter, 3: Slave receiver
		0;

	// CR2 value
	const portholder_t commoncr2 =
		//(2 * SAI_xCR2_FTH_0) |
		0;
	sai_A->CR2 =
		commoncr2 |
		0;
	sai_B->CR2 =
		commoncr2 |
		0;
	// SLOTR value
	const portholder_t commonslotr =
		(0 * SAI_xSLOTR_SLOTSZ_0) |	// 00: The slot size is equivalent to the data size (specified in DS[3:0] in the SAI_xCR1 register).
		//SAI_xSLOTR_SLOTEN |			// all slots enabled
		//(3UL << 16) |
		//(1 * SAI_xSLOTR_FBOFF_0) | // slot offset - "1" for I2S 24 bit in 32 bit slot
		0;
	sai_A->SLOTR =
		((SLOTEN_TX_SAIFPGA << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk) |			// TX slots enabled
		((NBSLOT_TX_SAIFPGA - 1) * SAI_xSLOTR_NBSLOT_0) | // Number of slots in audio Frame
		commonslotr |
		0;
	sai_B->SLOTR =
		((SLOTEN_RX_SAIFPGA << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk) |			// RX slots enabled
		((NBSLOT_RX_SAIFPGA - 1) * SAI_xSLOTR_NBSLOT_0) | // Number of slots in audio Frame
		commonslotr |
		0;

	// FRCR value
	const portholder_t comm_frcr =
		((WITHFPGAIF_FRAMEBITS - 1) * SAI_xFRCR_FRL_0) |
		(1 * SAI_xFRCR_FSDEF) |		/* FS signal is a start of frame signal + channel side identification - must be set for I2S */
		((WITHFPGAIF_FRAMEBITS / 2 - 1) * SAI_xFRCR_FSALL_0) |
#if WITHFPGAIF_FORMATI2S_PHILIPS
		(1 * SAI_xFRCR_FSOFF) |		/* 1: FS is asserted one bit before the first bit of the slot 0. Classic I2S form Philips Semiconductors. "0" value for Sony I2S specs. */
		(0 * SAI_xFRCR_FSPO) |	/* 0: канал с индексом 0 передается при "0" на WS - перывм. */
#endif /* WITHFPGAIF_FORMATI2S_PHILIPS */
		0;
	// FRAME CONTROL REGISTER
	sai_A->FRCR =
		comm_frcr |
		0;
	sai_B->FRCR =
		comm_frcr |
		0;
}

static void hardware_sai1_a_tx_b_rx_master_initialize_codec1(void)		/* инициализация SAI2 на STM32F4xx */
{
#if CPUSTYLE_STM32MP1
	// Теперь настроим модуль SAI.
	RCC->MP_APB2ENSETR = RCC_MP_APB2ENSETR_SAI1EN; //подать тактирование
	(void) RCC->MP_APB2ENSETR;
#else /* CPUSTYLE_STM32MP1 */
	// Теперь настроим модуль SAI.
	RCC->APB2ENR |= RCC_APB2ENR_SAI1EN; //подать тактирование
	(void) RCC->APB2ENR;
#endif /* CPUSTYLE_STM32MP1 */

	hardware_sai1_sai2_clock_selection();
	hardware_sai_a_tx_b_rx_initialize_codec1(SAI1_Block_A, SAI1_Block_B, 1);
	SAI1HW_INITIALIZE();
}

static void hardware_sai1_a_tx_b_rx_slave_initialize_codec1(void)		/* инициализация SAI2 на STM32F4xx */
{
#if CPUSTYLE_STM32MP1
	// Теперь настроим модуль SAI.
	RCC->MP_APB2ENSETR = RCC_MP_APB2ENSETR_SAI1EN; //подать тактирование
	(void) RCC->MP_APB2ENSETR;
#else /* CPUSTYLE_STM32MP1 */
	// Теперь настроим модуль SAI.
	RCC->APB2ENR |= RCC_APB2ENR_SAI1EN; //подать тактирование
	(void) RCC->APB2ENR;
#endif /* CPUSTYLE_STM32MP1 */

	hardware_sai1_sai2_clock_selection();
	hardware_sai_a_tx_b_rx_initialize_codec1(SAI1_Block_A, SAI1_Block_B, 0);
	SAI1HW_INITIALIZE();
}

#if WITHSAI2HW

static void hardware_sai2_a_tx_b_rx_master_initialize_codec1(void)		/* инициализация SAI2 на STM32F4xx */
{
#if CPUSTYLE_STM32MP1
	// Теперь настроим модуль SAI.
	RCC->MP_APB2ENSETR = RCC_MP_APB2ENSETR_SAI2EN; //подать тактирование
	(void) RCC->MP_APB2ENSETR;
#else /* CPUSTYLE_STM32MP1 */
	// Теперь настроим модуль SAI.
	RCC->APB2ENR |= RCC_APB2ENR_SAI2EN; //подать тактирование
	(void) RCC->APB2ENR;
#endif /* CPUSTYLE_STM32MP1 */

	hardware_sai1_sai2_clock_selection();
	hardware_sai_a_tx_b_rx_initialize_codec1(SAI2_Block_A, SAI2_Block_B, 1);
	SAI2HW_INITIALIZE();
}

static void hardware_sai2_a_tx_b_rx_slave_initialize_fpga(void)		/* инициализация SAI1 */
{
#if CPUSTYLE_STM32MP1
	// Теперь настроим модуль SAI.
	RCC->MP_APB2ENSETR = RCC_MP_APB2ENSETR_SAI2EN; //подать тактирование
	(void) RCC->MP_APB2ENSETR;
	RCC->MP_APB2LPENSETR = RCC_MP_APB2LPENSETR_SAI2LPEN; //подать тактирование
	(void) RCC->MP_APB2LPENSETR;
#else /* CPUSTYLE_STM32MP1 */
	// Теперь настроим модуль SAI.
	RCC->APB2ENR |= RCC_APB2ENR_SAI2EN; //подать тактирование
	(void) RCC->APB2ENR;
#endif /* CPUSTYLE_STM32MP1 */

	hardware_sai1_sai2_clock_selection();
	hardware_sai_a_tx_b_rx_initialize_fpga(SAI2_Block_A, SAI2_Block_B, 0);
	SAI2HW_INITIALIZE();
}

static void hardware_sai2_a_tx_b_rx_master_initialize_fpga(void)		/* инициализация SAI2 */
{
#if CPUSTYLE_STM32MP1
	// Теперь настроим модуль SAI.
	RCC->MP_APB2ENSETR = RCC_MP_APB2ENSETR_SAI2EN; //подать тактирование
	(void) RCC->MP_APB2ENSETR;
	RCC->MP_APB2LPENSETR = RCC_MP_APB2LPENSETR_SAI2LPEN; //подать тактирование
	(void) RCC->MP_APB2LPENSETR;
#else /* CPUSTYLE_STM32MP1 */
	// Теперь настроим модуль SAI.
	RCC->APB2ENR |= RCC_APB2ENR_SAI2EN; //подать тактирование
	(void) RCC->APB2ENR;
#endif /* CPUSTYLE_STM32MP1 */

	hardware_sai1_sai2_clock_selection();
	hardware_sai_a_tx_b_rx_initialize_fpga(SAI2_Block_A, SAI2_Block_B, 1);
	SAI2HW_INITIALIZE();
}

static void hardware_sai2_a_enable_codec1(uint_fast8_t state)		/* разрешение работы SAI2 на STM32F4xx */
{
	// при dual watch используется SAI2, но
	// через него не передаются данные.
	// Для работы синхронизации запукаются обе части - и приём и передача - в SAI2

	if (state != 0)
	{
		SAI2_Block_A->CR1 |= SAI_xCR1_SAIEN;
	}
	else
	{
		SAI2_Block_A->CR1 &= ~ SAI_xCR1_SAIEN;
	}
}

static void hardware_sai2_b_enable_codec1(uint_fast8_t state)		/* разрешение работы SAI2 на STM32F4xx */
{
	// при dual watch используется SAI2, но
	// через него не передаются данные.
	// Для работы синхронизации запукаются обе части - и приём и передача - в SAI2

	if (state != 0)
	{
		SAI2_Block_B->CR1 |= SAI_xCR1_SAIEN;
	}
	else
	{
		SAI2_Block_B->CR1 &= ~ SAI_xCR1_SAIEN;
	}
}

#endif /* WITHSAI2HW */

#endif /* WITHSAI1HW || WITHSAI2HW || WITHSAI3HW || WITHSAI4HW */

#if WITHSAI1HW

// DMA по приему SAI1 - обработчик прерывания
// RX	SAI1_B	DMA2	Stream 5	Channel 0
// SAI1_B_RX
void RAMFUNC_NONILINE DMA2_Stream5_IRQHandler_fpga_rx(void)
{
	// проверка условия может потребоваться при добавлении обработчика ошибки
	if ((DMA2->HISR & DMA_HISR_TCIF5) != 0)
	{
		DMA2->HIFCR = DMA_HIFCR_CTCIF5;	// Clear TC interrupt flag соответствующий stream
		//__DMB();	//ensure the ordering of data cache maintenance operations and their effects
		ASSERT((SAI1_Block_B->SR & SAI_xSR_OVRUDR_Msk) == 0);
		const uint_fast8_t b = (DMA2_Stream5->CR & DMA_SxCR_CT) != 0;
		if (b != 0)
		{
			const uintptr_t addr = DMA2_Stream5->M0AR;
			DMA2_Stream5->M0AR = dma_invalidate32rx(allocate_dmabuffer32rx());
			processing_dmabuffer32rts(addr);
			processing_dmabuffer32rx(addr);
			release_dmabuffer32rx(addr);
		}
		else
		{
			const uintptr_t addr = DMA2_Stream5->M1AR;
			DMA2_Stream5->M1AR = dma_invalidate32rx(allocate_dmabuffer32rx());
			processing_dmabuffer32rts(addr);
			processing_dmabuffer32rx(addr);
			release_dmabuffer32rx(addr);
		}
		buffers_resampleuacin(DMABUFFSIZE32RX / DMABUFFSTEP32RX);
	}

	//DMAERR(DMA2, DMA2_Stream5, HISR, HIFCR, DMA_HISR_TEIF5, DMA_HIFCR_CTEIF5);
	HANDLEERRORS(2, 5, H);
}

// DMA по передаче SAI1 - обработчик прерывания
// TX	SAI1_A	DMA2	Stream 1	Channel 0
// Use dcache_clean
void DMA2_Stream1_IRQHandler_fpga_tx(void)
{
	if ((DMA2->LISR & DMA_LISR_TCIF1) != 0)
	{
		DMA2->LIFCR = DMA_LIFCR_CTCIF1;	// Clear TC interrupt flag
		//__DMB();	//ensure the ordering of data cache maintenance operations and their effects
		ASSERT((SAI1_Block_A->SR & SAI_xSR_OVRUDR_Msk) == 0);
		const uint_fast8_t b = (DMA2_Stream1->CR & DMA_SxCR_CT) != 0;
		if (b != 0)
		{
			release_dmabuffer32tx(DMA2_Stream1->M0AR);
			DMA2_Stream1->M0AR = dma_flush32tx(getfilled_dmabuffer32tx_main());
			DRD(DMA2_Stream1->M0AR);
		}
		else
		{
			release_dmabuffer32tx(DMA2_Stream1->M1AR);
			DMA2_Stream1->M1AR = dma_flush32tx(getfilled_dmabuffer32tx_main());
			DRD(DMA2_Stream1->M1AR);
		}
	}

	//DMAERR(DMA2, DMA2_Stream1, LISR, LIFCR, DMA_LISR_TEIF1, DMA_LIFCR_CTEIF1);
	HANDLEERRORS(2, 1, L);
}

// DMA по передаче SAI1 - инициализация
// TX	SAI1_A	DMA2	Stream 1	Channel 0
// Use dcache_clean
static void DMA_SAI1_A_TX_initialize_fpga(void)
{
	/* SAI1_A - Stream1, Channel0 */ 
	/* DMA для передачи по I2S2*/
#if CPUSTYLE_STM32MP1
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMA2EN; // включил DMA2
	(void) RCC->MP_AHB2ENSETR;
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMAMUXEN; // включил DMAMUX
	(void) RCC->MP_AHB2ENSETR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream1->PAR = (uintptr_t) & SAI1_Block_A->DR;

#elif CPUSTYLE_STM32H7XX
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream1->PAR = (uintptr_t) & SAI1_Block_A->DR;

#else /* others */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	const uint_fast8_t ch = 0;
	DMA2_Stream1->PAR = (uintptr_t) & SAI1_Block_A->DR;

#endif /* CPUSTYLE_STM32MP1 */

	DMA2_Stream1->M0AR = dma_flush32tx(allocate_dmabuffer32tx());
	DMA2_Stream1->M1AR = dma_flush32tx(allocate_dmabuffer32tx());
	DMA2_Stream1->NDTR = (DMA2_Stream1->NDTR & ~ DMA_SxNDT) |
		(DMABUFFSIZE32TX * DMA_SxNDT_0);

	DMA2_Stream1->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use direct mode
	DMA2_Stream1->CR =
		ch * DMA_SxCR_CHSEL_0 | //канал
		0 * DMA_SxCR_MBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_PBURST_0 |	// 0: single transfer
		1 * DMA_SxCR_DIR_0 | //направление - память - периферия
		1 * DMA_SxCR_MINC | //инкремент памяти
		2 * DMA_SxCR_MSIZE_0 | //длина в памяти - 32 bit
		2 * DMA_SxCR_PSIZE_0 | //длина в DR - 32 bit
		(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
		0 * DMA_SxCR_CT | // M0AR selected
		1 * DMA_SxCR_DBM | // double buffer mode seelcted
		0;

#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel9->CCR = 87 * DMAMUX_CxCR_DMAREQ_ID_0;	// SAI1_A
	DRD(DMAMUX1_Channel9->CCR);
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */


	DMA2->LIFCR = DMA_LISR_TCIF1 | DMA_LISR_TEIF1;	// Clear TC interrupt flag
	DMA2_Stream1->CR |= DMA_SxCR_TCIE;	// Разрешаем прерывания от DMA

	arm_hardware_set_handler_realtime(DMA2_Stream1_IRQn, DMA2_Stream1_IRQHandler_fpga_tx);

	DMA2_Stream1->CR |= DMA_SxCR_EN;
}

/* DMA для прёма по SAI_1_B  - инициализация */
// RX	SAI1_B	DMA2	Stream 5	Channel 0
static void DMA_SAI1_B_RX_initialize_fpga(void)
{
	/* SAI1_B - Stream5, Channel0 */ 
#if CPUSTYLE_STM32MP1
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMA2EN; // включил DMA2
	(void) RCC->MP_AHB2ENSETR;
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMAMUXEN; // включил DMAMUX
	(void) RCC->MP_AHB2ENSETR;

	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream5->PAR = (uintptr_t) & SAI1_Block_B->DR;

#elif CPUSTYLE_STM32H7XX
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;

	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream5->PAR = (uintptr_t) & SAI1_Block_B->DR;

#else /* others */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	const uint_fast8_t ch = 0;
	DMA2_Stream5->PAR = (uintptr_t) & SAI1_Block_B->DR;

#endif /* CPUSTYLE_STM32MP1 */

	DMA2_Stream5->M0AR = dma_invalidate32rx(allocate_dmabuffer32rx());
	DMA2_Stream5->M1AR = dma_invalidate32rx(allocate_dmabuffer32rx());
	DMA2_Stream5->NDTR = (DMA2_Stream5->NDTR & ~ DMA_SxNDT) |
		(DMABUFFSIZE32RX * DMA_SxNDT_0);

	DMA2_Stream5->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use direct mode
	DRD(DMA2_Stream5->FCR);

	DMA2_Stream5->CR =
		ch * DMA_SxCR_CHSEL_0 | // канал
		0 * DMA_SxCR_MBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_PBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_DIR_0 |	// 00: Peripheral-to-memory
		1 * DMA_SxCR_MINC |		//инкремент памяти
		2 * DMA_SxCR_MSIZE_0 | //длина в памяти - 32 bit
		2 * DMA_SxCR_PSIZE_0 | //длина в DR - 32 bit
		(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
		0 * DMA_SxCR_CT |	// M0AR selected
		1 * DMA_SxCR_DBM |	 // double buffer mode seelcted
		0;
	DRD(DMA2_Stream5->CR);

#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel13->CCR = 88 * DMAMUX_CxCR_DMAREQ_ID_0;	// SAI1_B
	DRD(DMAMUX1_Channel13->CCR);
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */


	DMA2->HIFCR = (DMA_HIFCR_CTCIF5 /* | DMA_HIFCR_CTEIF5 */);	// Clear TC interrupt flag соответствующий stream
	DMA2_Stream5->CR |= DMA_SxCR_TCIE;	// Разрешаем прерывания от DMA

	arm_hardware_set_handler_realtime(DMA2_Stream5_IRQn, DMA2_Stream5_IRQHandler_fpga_rx);

	DMA2_Stream5->CR |= DMA_SxCR_EN;
	DRD(DMA2_Stream5->CR);
}

static void hardware_sai1_master_duplex_initialize_v3d_fpga(void)		/* инициализация SAI1 на STM32F4xx */
{
	hardware_sai1_sai2_clock_selection();

	// Теперь настроим модуль SAI.
#if CPUSTYLE_STM32MP1
	// Теперь настроим модуль SAI.
	RCC->MP_APB2ENSETR = RCC_MP_APB2ENSETR_SAI1EN; //подать тактирование
	(void) RCC->MP_APB2ENSETR;
	RCC->MP_APB2LPENSETR = RCC_MP_APB2LPENSETR_SAI1LPEN; //подать тактирование
	(void) RCC->MP_APB2LPENSETR;
#else /* CPUSTYLE_STM32MP1 */
	// Теперь настроим модуль SAI.
	RCC->APB2ENR |= RCC_APB2ENR_SAI1EN; //подать тактирование
	(void) RCC->APB2ENR;
#endif /* CPUSTYLE_STM32MP1 */
	

	SAI1_Block_A->CR1 &= ~ SAI_xCR1_SAIEN;
	SAI1_Block_B->CR1 &= ~ SAI_xCR1_SAIEN;

	// extclock = 24.576 MHz
	// if SAI_xCR1_NODIV == 1, ws=384 kHz
	//const uint32_t mckval = 0;	// divider = 1, ws = 96 kHz
	//const uint32_t mckval = 1;	// divider = 2, ws = 48 kHz
	//const uint32_t mckval = 2;	// divider = 4, ws = 24 kHz
	//const uint32_t mckval = 3;	// divider = 6, ws = 16 kHz
	//const uint32_t mckval = 4;	// divider = 8, ws = 12 kHz

	// SAI_xCR1_CKSTR:
	// 0: Signals generated by the SAI change on SCK rising edge, 
	//		while signals received by the SAI are sampled on the SCK falling edge.
	// 1: Signals generated by the SAI change on SCK falling edge, 
	//		while signals received by the SAI are sampled on the SCK rising edge.

	const uint_fast8_t nodiv = 0;
#if WITHSAICLOCKFROMPIN
	const uint_fast8_t mckdiv = mckdivform(calcdivround_extsai(ARMSAIMCLK));
#elif WITHSAICLOCKFROMI2S
	const uint_fast8_t mckdiv = mckdivform(calcdivround_plli2s(ARMSAIMCLK));
#else
		// from own PLL
	const uint_fast8_t mckdiv = mckdivform(calcdivround_pllsai(ARMSAIMCLK));
#endif
		
	// CR1 value
	const portholder_t commoncr1 =
		(1 * SAI_xCR1_OUTDRIV) |	//
		//(0 * SAI_xCR1_MONO) |	// stereo MODE - with IQ pairs - see DMABUFFSIZE32
		(1 * SAI_xCR1_DMAEN) |	// 1: DMA enable 
		(1 * SAI_xCR1_CKSTR) |	// 0: данные на выходе меняются по нарастающему фронту, 1: по спадающему
		(7 * SAI_xCR1_DS_0) |	// 6: data size - 24 bit, 7: 32 bit, 4: 16 bit
		(0 * SAI_xCR1_PRTCFG_0) |	// 0: free protocol
		(nodiv * SAI_xCR1_NODIV) |	// 1: no MCKDIV value
		((mckdiv * SAI_xCR1_MCKDIV_0) & SAI_xCR1_MCKDIV) |	// MCKDIV vale = 4 bits
		0;

#if WITHSAICLOCKFROMPIN
	PRINTF(PSTR("hardware_sai1_master_duplex_initialize_fpga: 1 SAI1 MCKDIV=%lu, ARMSAIMCLK=%lu, EXTSAI_FREQ=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) EXTSAI_FREQ);
#elif WITHSAICLOCKFROMI2S
	PRINTF(PSTR("hardware_sai1_master_duplex_initialize_fpga: 2 SAI1 MCKDIV=%lu, ARMSAIMCLK=%lu, PLLI2S_FREQ_OUT=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) PLLI2S_FREQ_OUT);
#else
	PRINTF(PSTR("hardware_sai1_master_duplex_initialize_fpga: 3 SAI1 MCKDIV=%lu, ARMSAIMCLK=%lu, PLLSAI_FREQ_OUT=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) PLLSAI_FREQ_OUT);
#endif

	// v3d specific
	SAI1_Block_A->CR1 = 
		commoncr1 |
		(1 * SAI_xCR1_SYNCEN_0) |	// SYNChronization ENable: 0: audio sub-block in asynchronous mode.
		(0 * SAI_xCR1_MODE_0) |	// 0: Master transmitter, 1: Master receiver, 2: Slave transmitter, 3: Slave receiver
		0;
	SAI1_Block_B->CR1 =
		commoncr1 |
		(0 * SAI_xCR1_SYNCEN_0) |	// SYNChronization ENable: audio sub-block is synchronous with the other internal audio sub-block. In this case, the audio sub-block must be configured in slave mode
		(1 * SAI_xCR1_MODE_0) |		// 0: Master transmitter, 1: Master receiver, 2: Slave transmitter, 3: Slave receiver
		0;

	// CR2 value
	const portholder_t commoncr2 =
		//(2 * SAI_xCR2_FTH_0) |
		0;
	SAI1_Block_A->CR2 =
		commoncr2 |
		0;
	SAI1_Block_B->CR2 =
		commoncr2 |
		0;
	// SLOTR value
	const portholder_t commonslotr =
		(0 * SAI_xSLOTR_SLOTSZ_0) |	// 00: The slot size is equivalent to the data size (specified in DS[3:0] in the SAI_xCR1 register).
		//SAI_xSLOTR_SLOTEN |			// all slots enabled
		//(3UL << 16) |
		//(1 * SAI_xSLOTR_FBOFF_0) | // slot offset - "1" for I2S 24 bit in 32 bit slot
		0;
	SAI1_Block_A->SLOTR =
		((SLOTEN_TX_SAIFPGA << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk) |			// TX slots enabled
		((NBSLOT_TX_SAIFPGA - 1) * SAI_xSLOTR_NBSLOT_0) | // Number of slots in audio Frame
		commonslotr |
		0;
	SAI1_Block_B->SLOTR =
		((SLOTEN_RX_SAIFPGA << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk) |			// RX slots enabled
		((NBSLOT_RX_SAIFPGA - 1) * SAI_xSLOTR_NBSLOT_0) | // Number of slots in audio Frame
		commonslotr |
		0;

	// FRCR value
	const portholder_t comm_frcr =
		((WITHFPGAIF_FRAMEBITS - 1) * SAI_xFRCR_FRL_0) |
		(1 * SAI_xFRCR_FSDEF) |		/* FS signal is a start of frame signal + channel side identification - must be set for I2S */
		((WITHFPGAIF_FRAMEBITS / 2 - 1) * SAI_xFRCR_FSALL_0) |
#if WITHFPGAIF_FORMATI2S_PHILIPS
		(1 * SAI_xFRCR_FSOFF) |		/* 1: FS is asserted one bit before the first bit of the slot 0. Classic I2S form Philips Semiconductors. "0" value for Sony I2S specs. */
		(0 * SAI_xFRCR_FSPO) |	/* 0: канал с индексом 0 передается при "0" на WS - перывм. */
#endif /* WITHFPGAIF_FORMATI2S_PHILIPS */
		0;
	// FRAME CONTROL REGISTER
	SAI1_Block_A->FRCR =
		comm_frcr |
		0;
	SAI1_Block_B->FRCR =
		comm_frcr |
		0;

	SAI1HW_INITIALIZE();
}

static void hardware_sai1_master_duplex_initialize_fpga(void)		/* инициализация SAI1 на STM32F4xx */
{
	hardware_sai1_sai2_clock_selection();

	// Теперь настроим модуль SAI.
#if CPUSTYLE_STM32MP1
	// Теперь настроим модуль SAI.
	RCC->MP_APB2ENSETR = RCC_MP_APB2ENSETR_SAI1EN; //подать тактирование
	(void) RCC->MP_APB2ENSETR;
	RCC->MP_APB2LPENSETR = RCC_MP_APB2LPENSETR_SAI1LPEN; //подать тактирование
	(void) RCC->MP_APB2LPENSETR;
#else /* CPUSTYLE_STM32MP1 */
	// Теперь настроим модуль SAI.
	RCC->APB2ENR |= RCC_APB2ENR_SAI1EN; //подать тактирование
	(void) RCC->APB2ENR;
#endif /* CPUSTYLE_STM32MP1 */


	SAI1_Block_A->CR1 &= ~ SAI_xCR1_SAIEN;
	SAI1_Block_B->CR1 &= ~ SAI_xCR1_SAIEN;

	// extclock = 24.576 MHz
	// if SAI_xCR1_NODIV == 1, ws=384 kHz
	//const uint32_t mckval = 0;	// divider = 1, ws = 96 kHz
	//const uint32_t mckval = 1;	// divider = 2, ws = 48 kHz
	//const uint32_t mckval = 2;	// divider = 4, ws = 24 kHz
	//const uint32_t mckval = 3;	// divider = 6, ws = 16 kHz
	//const uint32_t mckval = 4;	// divider = 8, ws = 12 kHz

	// SAI_xCR1_CKSTR:
	// 0: Signals generated by the SAI change on SCK rising edge,
	//		while signals received by the SAI are sampled on the SCK falling edge.
	// 1: Signals generated by the SAI change on SCK falling edge,
	//		while signals received by the SAI are sampled on the SCK rising edge.

	const uint_fast8_t nodiv = 0;
#if WITHSAICLOCKFROMPIN
	const uint_fast8_t mckdiv = mckdivform(calcdivround_extsai(ARMSAIMCLK));
#elif WITHSAICLOCKFROMI2S
	const uint_fast8_t mckdiv = mckdivform(calcdivround_plli2s(ARMSAIMCLK));
#else
		// from own PLL
	const uint_fast8_t mckdiv = mckdivform(calcdivround_pllsai(ARMSAIMCLK));
#endif

	// CR1 value
	const portholder_t commoncr1 =
		(1 * SAI_xCR1_OUTDRIV) |	//
		//(0 * SAI_xCR1_MONO) |	// stereo MODE - with IQ pairs - see DMABUFFSIZE32
		(1 * SAI_xCR1_DMAEN) |	// 1: DMA enable
		(1 * SAI_xCR1_CKSTR) |	// 0: данные на выходе меняются по нарастающему фронту, 1: по спадающему
		(FPGAIF_SAI_CR1_DS * SAI_xCR1_DS_0) |	// 6: data size - 24 bit, 7: 32 bit, 4: 16 bit
		(0 * SAI_xCR1_PRTCFG_0) |	// 0: free protocol
		(nodiv * SAI_xCR1_NODIV) |	// 1: no MCKDIV value
		((mckdiv * SAI_xCR1_MCKDIV_0) & SAI_xCR1_MCKDIV) |	// MCKDIV vale = 4 bits
		0;

#if WITHSAICLOCKFROMPIN
	PRINTF(PSTR("hardware_sai1_master_duplex_initialize_fpga: 1 SAI1 MCKDIV=%lu, ARMSAIMCLK=%lu, EXTSAI_FREQ=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) EXTSAI_FREQ);
#elif WITHSAICLOCKFROMI2S
	PRINTF(PSTR("hardware_sai1_master_duplex_initialize_fpga: 2 SAI1 MCKDIV=%lu, ARMSAIMCLK=%lu, PLLI2S_FREQ_OUT=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) PLLI2S_FREQ_OUT);
#else
	PRINTF(PSTR("hardware_sai1_master_duplex_initialize_fpga: 3 SAI1 MCKDIV=%lu, ARMSAIMCLK=%lu, PLLSAI_FREQ_OUT=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) PLLSAI_FREQ_OUT);
#endif

	SAI1_Block_A->CR1 =
		commoncr1 |
		(0 * SAI_xCR1_SYNCEN_0) |	// SYNChronization ENable: 0: audio sub-block in asynchronous mode.
		(0 * SAI_xCR1_MODE_0) |	// 0: Master transmitter, 1: Master receiver, 2: Slave transmitter, 3: Slave receiver
		0;
	SAI1_Block_B->CR1 = 
		commoncr1 |
		(1 * SAI_xCR1_SYNCEN_0) |	// SYNChronization ENable: audio sub-block is synchronous with the other internal audio sub-block. In this case, the audio sub-block must be configured in slave mode
		(3 * SAI_xCR1_MODE_0) |		// 0: Master transmitter, 1: Master receiver, 2: Slave transmitter, 3: Slave receiver
		0;

	// CR2 value
	const portholder_t commoncr2 =
		//(2 * SAI_xCR2_FTH_0) |
		0;
	SAI1_Block_A->CR2 = 
		commoncr2 |
		0;
	SAI1_Block_B->CR2 = 
		commoncr2 |
		0;
	// SLOTR value
	const portholder_t commonslotr =
		(0 * SAI_xSLOTR_SLOTSZ_0) |	// 00: The slot size is equivalent to the data size (specified in DS[3:0] in the SAI_xCR1 register).
		//SAI_xSLOTR_SLOTEN |			// all slots enabled
		//(3UL << 16) |
		//(1 * SAI_xSLOTR_FBOFF_0) | // slot offset - "1" for I2S 24 bit in 32 bit slot
		0;
	SAI1_Block_A->SLOTR = 
		((SLOTEN_TX_SAIFPGA << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk) |			// TX slots enabled
		((NBSLOT_TX_SAIFPGA - 1) * SAI_xSLOTR_NBSLOT_0) | // Number of slots in audio Frame
		commonslotr |
		0;
	SAI1_Block_B->SLOTR = 
		((SLOTEN_RX_SAIFPGA << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk) |			// RX slots enabled
		((NBSLOT_RX_SAIFPGA - 1) * SAI_xSLOTR_NBSLOT_0) | // Number of slots in audio Frame
		commonslotr |
		0;

	// FRCR value
	const portholder_t comm_frcr =
		((WITHFPGAIF_FRAMEBITS - 1) * SAI_xFRCR_FRL_0) |
		(1 * SAI_xFRCR_FSDEF) |		/* FS signal is a start of frame signal + channel side identification - must be set for I2S */
		((WITHFPGAIF_FRAMEBITS / 2 - 1) * SAI_xFRCR_FSALL_0) |
#if WITHFPGAIF_FORMATI2S_PHILIPS
		(1 * SAI_xFRCR_FSOFF) |		/* 1: FS is asserted one bit before the first bit of the slot 0. Classic I2S form Philips Semiconductors. "0" value for Sony I2S specs. */
		(0 * SAI_xFRCR_FSPO) |	/* 0: канал с индексом 0 передается при "0" на WS - перывм. */
#endif /* WITHFPGAIF_FORMATI2S_PHILIPS */
		0;
	// FRAME CONTROL REGISTER
	SAI1_Block_A->FRCR =
		comm_frcr |
		0;
	SAI1_Block_B->FRCR =
		comm_frcr |
		0;

	SAI1HW_INITIALIZE();
}

static void hardware_sai1_slave_duplex_initialize_fpga(void)		/* инициализация SAI1 на STM32F4xx */
{
	hardware_sai1_sai2_clock_selection();

#if CPUSTYLE_STM32MP1
	// Теперь настроим модуль SAI.
	RCC->MP_APB2ENSETR = RCC_MP_APB2ENSETR_SAI1EN; //подать тактирование
	(void) RCC->MP_APB2ENSETR;
	RCC->MP_APB2LPENSETR = RCC_MP_APB2LPENSETR_SAI1LPEN; //подать тактирование
	(void) RCC->MP_APB2LPENSETR;
#else /* CPUSTYLE_STM32MP1 */
	// Теперь настроим модуль SAI.
	RCC->APB2ENR |= RCC_APB2ENR_SAI1EN; //подать тактирование
	(void) RCC->APB2ENR;
#endif /* CPUSTYLE_STM32MP1 */
	
	SAI1_Block_A->CR1 &= ~ SAI_xCR1_SAIEN;
	SAI1_Block_B->CR1 &= ~ SAI_xCR1_SAIEN;

	// extclock = 24.576 MHz
	// if SAI_xCR1_NODIV == 1, ws=384 kHz
	//const uint32_t mckval = 0;	// divider = 1, ws = 96 kHz
	//const uint32_t mckval = 1;	// divider = 2, ws = 48 kHz
	//const uint32_t mckval = 2;	// divider = 4, ws = 24 kHz
	//const uint32_t mckval = 3;	// divider = 6, ws = 16 kHz
	//const uint32_t mckval = 4;	// divider = 8, ws = 12 kHz
	//
	// SAI_xCR1_CKSTR:
	// 0: Signals generated by the SAI change on SCK rising edge, 
	//		while signals received by the SAI are sampled on the SCK falling edge.
	// 1: Signals generated by the SAI change on SCK falling edge, 
	//		while signals received by the SAI are sampled on the SCK rising edge.
	//const uint_fast8_t nodiv = 0;
#if WITHSAICLOCKFROMPIN
	//const uint_fast8_t mckdiv = mckdivform(calcdivround_extsai(ARMSAIMCLK));
#elif WITHSAICLOCKFROMI2S
	//const uint_fast8_t mckdiv = mckdivform(calcdivround_plli2s(ARMSAIMCLK));
#else
		// from own PLL
	//const uint_fast8_t mckdiv = mckdivform(calcdivround_pllsai(ARMSAIMCLK));
#endif

	// CR1 value
	const portholder_t commoncr1 =
		(1 * SAI_xCR1_OUTDRIV) |	//
		//(0 * SAI_xCR1_MONO) |	// stereo MODE - with IQ pairs - see DMABUFFSIZE32
		(1 * SAI_xCR1_DMAEN) |	// 1: DMA enable 
		(1 * SAI_xCR1_CKSTR) |	// 0: данные на выходе меняются по нарастающему фронту, 1: по спадающему
		(FPGAIF_SAI_CR1_DS * SAI_xCR1_DS_0) |	// 6: data size - 24 bit, 7: 32 bit, 4: 16 bit
		(0 * SAI_xCR1_PRTCFG_0) |	// 0: free protocol
		//(nodiv * SAI_xCR1_NODIV) |	// 1: no MCKDIV value
		//((mckdiv * SAI_xCR1_MCKDIV_0) & SAI_xCR1_MCKDIV) |	// MCKDIV vale = 4 bits
		0;

#if WITHSAICLOCKFROMPIN
	//PRINTF(PSTR("hardware_sai1_slave_duplex_initialize_fpga: 1 SAI1 MCKDIV=%lu, ARMSAIMCLK=%lu, EXTSAI_FREQ=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) EXTSAI_FREQ);
#elif WITHSAICLOCKFROMI2S
	//PRINTF(PSTR("hardware_sai1_slave_duplex_initialize_fpga: 2 SAI1 MCKDIV=%lu, ARMSAIMCLK=%lu, PLLI2S_FREQ_OUT=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) PLLI2S_FREQ_OUT);
#else
	//PRINTF(PSTR("hardware_sai1_slave_duplex_initialize_fpga: 3 SAI1 MCKDIV=%lu, ARMSAIMCLK=%lu, PLLSAI_FREQ_OUT=%lu\n"), (unsigned long) mckdiv, (unsigned long) ARMSAIMCLK, (unsigned long) PLLSAI_FREQ_OUT);
#endif

	SAI1_Block_A->CR1 = 
		commoncr1 |
		(0 * SAI_xCR1_SYNCEN_0) |	// SYNChronization ENable: 0: audio sub-block in asynchronous mode.
		(2 * SAI_xCR1_MODE_0) |	// 0: Master transmitter, 1: Master receiver, 2: Slave transmitter, 3: Slave receiver
		0;
	SAI1_Block_B->CR1 = 
		commoncr1 |
		(1 * SAI_xCR1_SYNCEN_0) |	// SYNChronization ENable: audio sub-block is synchronous with the other internal audio sub-block. In this case, the audio sub-block must be configured in slave mode
		(3 * SAI_xCR1_MODE_0) |		// 0: Master transmitter, 1: Master receiver, 2: Slave transmitter, 3: Slave receiver
		0;

	// CR2 value
	const portholder_t commoncr2 =
		//(2 * SAI_xCR2_FTH_0) |
		0;
	SAI1_Block_A->CR2 = 
		commoncr2 |
		0;
	SAI1_Block_B->CR2 = 
		commoncr2 |
		0;
	// SLOTR value
	const portholder_t commonslotr =
		(0 * SAI_xSLOTR_SLOTSZ_0) |	// 00: The slot size is equivalent to the data size (specified in DS[3:0] in the SAI_xCR1 register).
		//SAI_xSLOTR_SLOTEN |			// all slots enabled
		//(3UL << 16) |
		//(1 * SAI_xSLOTR_FBOFF_0) | // slot offset - "1" for I2S 24 bit in 32 bit slot
		0;
	SAI1_Block_A->SLOTR = 
		((SLOTEN_TX_SAIFPGA << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk) |			// TX slots enabled
		((NBSLOT_TX_SAIFPGA - 1) * SAI_xSLOTR_NBSLOT_0) | // Number of slots in audio Frame
		commonslotr |
		0;
	SAI1_Block_B->SLOTR = 
		((SLOTEN_RX_SAIFPGA << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk) |			// RX slots enabled
		((NBSLOT_RX_SAIFPGA - 1) * SAI_xSLOTR_NBSLOT_0) | // Number of slots in audio Frame
		commonslotr |
		0;

	// FRCR value
	const portholder_t comm_frcr =
		((WITHFPGAIF_FRAMEBITS - 1) * SAI_xFRCR_FRL_0) |
		(1 * SAI_xFRCR_FSDEF) |		/* FS signal is a start of frame signal + channel side identification - must be set for I2S */
		((WITHFPGAIF_FRAMEBITS / 2 - 1) * SAI_xFRCR_FSALL_0) |
#if WITHFPGAIF_FORMATI2S_PHILIPS
		(1 * SAI_xFRCR_FSOFF) |		/* 1: FS is asserted one bit before the first bit of the slot 0. Classic I2S form Philips Semiconductors. "0" value for Sony I2S specs. */
		(0 * SAI_xFRCR_FSPO) |	/* 0: канал с индексом 0 передается при "0" на WS - перывм. */
#endif /* WITHFPGAIF_FORMATI2S_PHILIPS */
		0;
	// FRAME CONTROL REGISTER
	SAI1_Block_A->FRCR =
		comm_frcr |
		0;
	SAI1_Block_B->FRCR =
		comm_frcr |
		0;

	//PRINTF(PSTR("SAI1_Block_A->CR1=0X%08lX;\n"), SAI1_Block_A->CR1);
	//PRINTF(PSTR("SAI1_Block_A->CR2=0X%08lX;\n"), SAI1_Block_A->CR2);
	//PRINTF(PSTR("SAI1_Block_A->SLOTR=0X%08lX;\n"), SAI1_Block_A->SLOTR);
	//PRINTF(PSTR("SAI1_Block_A->FRCR=0X%08lX;\n"), SAI1_Block_A->FRCR);

	//PRINTF(PSTR("SAI1_Block_B->CR1=0X%08lX;\n"), SAI1_Block_B->CR1);
	//PRINTF(PSTR("SAI1_Block_B->CR2=0X%08lX;\n"), SAI1_Block_B->CR2);
	//PRINTF(PSTR("SAI1_Block_B->SLOTR=0X%08lX;\n"), SAI1_Block_B->SLOTR);
	//PRINTF(PSTR("SAI1_Block_B->FRCR=0X%08lX;\n"), SAI1_Block_B->FRCR);

	SAI1HW_INITIALIZE();
}

static void hardware_sai1_a_enable_fpga(uint_fast8_t state)		/* разрешение работы SAI1 на STM32F4xx */
{
	if (state != 0)
	{
		SAI1_Block_A->CR1 |= SAI_xCR1_SAIEN;
	}
	else
	{
		SAI1_Block_A->CR1 &= ~ SAI_xCR1_SAIEN;
	}
}

static void hardware_sai1_b_enable_fpga(uint_fast8_t state)		/* разрешение работы SAI1 на STM32F4xx */
{
	if (state != 0)
	{
		SAI1_Block_B->CR1 |= SAI_xCR1_SAIEN;
	}
	else
	{
		SAI1_Block_B->CR1 &= ~ SAI_xCR1_SAIEN;
	}
}

#endif /* WITHSAI1HW */

#if WITHSAI2HW

#if WITHFPGARTS_FRAMEBITS == 32

	#define SAI2_RTS_DMA_SxCR_xSIZE		0x01uL	// 01: half-word (16-bit)

#else /*  WITHFPGARTS_FRAMEBITS == 64 */

	// 2*32, 2*24
	#define SAI2_RTS_DMA_SxCR_xSIZE		0x02uL	// 10: word (32-bit)

#endif /*  CODEC1_FRAMEBITS == 64 */

// DMA по приему SAI2 - обработчик прерывания
// RX	SAI2_B	DMA2	Stream7	Channel 3
void RAMFUNC_NONILINE DMA2_Stream7_IRQHandler_codec1_rx(void)
{
	// проверка условия может потребоваться при добавлении обработчика ошибки
	if ((DMA2->HISR & DMA_HISR_TCIF7) != 0)
	{
		DMA2->HIFCR = DMA_HIFCR_CTCIF7;	// Clear TC interrupt flag соответствующий stream
		ASSERT((SAI2_Block_B->SR & SAI_xSR_OVRUDR_Msk) == 0);
		const uint_fast8_t b = (DMA2_Stream7->CR & DMA_SxCR_CT) != 0;
		//__DMB();	//ensure the ordering of data cache maintenance operations and their effects

		if (b != 0)
		{
			const uintptr_t addr = DMA2_Stream7->M0AR;
			DMA2_Stream7->M0AR = dma_invalidate16rx(allocate_dmabuffer16rx());
			processing_dmabuffer16rx(addr);		// посмещается в очередь
		}
		else
		{
			const uintptr_t addr = DMA2_Stream7->M1AR;
			DMA2_Stream7->M1AR = dma_invalidate16rx(allocate_dmabuffer16rx());
			processing_dmabuffer16rx(addr);
		}
	}

	//DMAERR(DMA2, DMA2_Stream7, HISR, HIFCR, DMA_HISR_TEIF7, DMA_HIFCR_CTEIF7);
	HANDLEERRORS(2, 7, H);
}

// DMA по приему SAI2 - обработчик прерывания
// RX	SAI2_B	DMA2	Stream7	Channel 3
void RAMFUNC_NONILINE DMA2_Stream7_IRQHandler_fpga_rx(void)
{
	// проверка условия может потребоваться при добавлении обработчика ошибки
	if ((DMA2->HISR & DMA_HISR_TCIF7) != 0)
	{
		DMA2->HIFCR = DMA_HIFCR_CTCIF7;	// Clear TC interrupt flag соответствующий stream
		ASSERT((SAI2_Block_B->SR & SAI_xSR_OVRUDR_Msk) == 0);
		const uint_fast8_t b = (DMA2_Stream7->CR & DMA_SxCR_CT) != 0;
		if (b != 0)
		{
			const uintptr_t addr = DMA2_Stream7->M0AR;
			DMA2_Stream7->M0AR = dma_invalidate32rx(allocate_dmabuffer32rx());
			processing_dmabuffer32rts(addr);
			processing_dmabuffer32rx(addr);
			release_dmabuffer32rx(addr);
		}
		else
		{
			const uintptr_t addr = DMA2_Stream7->M1AR;
			DMA2_Stream7->M1AR = dma_invalidate32rx(allocate_dmabuffer32rx());
			processing_dmabuffer32rts(addr);
			processing_dmabuffer32rx(addr);
			release_dmabuffer32rx(addr);
		}
		buffers_resampleuacin(DMABUFFSIZE32RX / DMABUFFSTEP32RX);
	}

	//DMAERR(DMA2, DMA2_Stream7, HISR, HIFCR, DMA_HISR_TEIF7, DMA_HIFCR_CTEIF7);
	HANDLEERRORS(2, 7, H);
}

// DMA по приему SAI2 - обработчик прерывания
// RX	SAI2_B	DMA2	Stream7	Channel 3
void RAMFUNC_NONILINE DMA2_Stream7_IRQHandler_fpga_rts192_rx(void)
{
	// проверка условия может потребоваться при добавлении обработчика ошибки
	if ((DMA2->HISR & DMA_HISR_TCIF7) != 0)
	{
		DMA2->HIFCR = DMA_HIFCR_CTCIF7;	// Clear TC interrupt flag соответствующий stream
		ASSERT((SAI2_Block_B->SR & SAI_xSR_OVRUDR_Msk) == 0);
		const uint_fast8_t b = (DMA2_Stream7->CR & DMA_SxCR_CT) != 0;
		if (b != 0)
		{
			const uintptr_t addr = DMA2_Stream7->M0AR;
			DMA2_Stream7->M0AR = dma_invalidate192rts(allocate_dmabuffer192rts());
			processing_dmabuffer32rts192(addr);
		}
		else
		{
			const uintptr_t addr = DMA2_Stream7->M1AR;
			DMA2_Stream7->M1AR = dma_invalidate192rts(allocate_dmabuffer192rts());
			processing_dmabuffer32rts192(addr);
		}
	}

	//DMAERR(DMA2, DMA2_Stream7, HISR, HIFCR, DMA_HISR_TEIF7, DMA_HIFCR_CTEIF7);
	HANDLEERRORS(2, 7, H);
}

// DMA по приему SAI2 - обработчик прерывания
// RX	SAI2_B	DMA2	Stream7	Channel 3
void RAMFUNC_NONILINE DMA2_Stream7_IRQHandler_wfm_rx(void)
{
	// проверка условия может потребоваться при добавлении обработчика ошибки
	if ((DMA2->HISR & DMA_HISR_TCIF7) != 0)
	{
		DMA2->HIFCR = DMA_HIFCR_CTCIF7;	// Clear TC interrupt flag соответствующий stream
		ASSERT((SAI2_Block_B->SR & SAI_xSR_OVRUDR_Msk) == 0);
		const uint_fast8_t b = (DMA2_Stream7->CR & DMA_SxCR_CT) != 0;
		//__DMB();	//ensure the ordering of data cache maintenance operations and their effects

		if (b != 0)
		{
			const uintptr_t addr = DMA2_Stream7->M0AR;
			DMA2_Stream7->M0AR = dma_invalidate32rx(allocate_dmabuffer32rx());
			processing_dmabuffer32wfm(addr);
			release_dmabuffer32rx(addr);
		}
		else
		{
			const uintptr_t addr = DMA2_Stream7->M1AR;
			DMA2_Stream7->M1AR = dma_invalidate32rx(allocate_dmabuffer32rx());
			processing_dmabuffer32wfm(addr);
			release_dmabuffer32rx(addr);
		}
	}

	//DMAERR(DMA2, DMA2_Stream7, HISR, HIFCR, DMA_HISR_TEIF7, DMA_HIFCR_CTEIF7);
	HANDLEERRORS(2, 7, H);
}

// TX	SAI2_A	DMA2	Stream 4	Channel 3
// Use dcache_clean
void DMA2_Stream4_IRQHandler_codec1_tx(void)
{
	// проверка условия может потребоваться при добавлении обработчика ошибки
	if ((DMA2->HISR & DMA_HISR_TCIF4) != 0)
	{
		DMA2->HIFCR = DMA_HIFCR_CTCIF4;	// Clear TC interrupt flag соответствующий stream
		ASSERT((SAI2_Block_A->SR & SAI_xSR_OVRUDR_Msk) == 0);
		const uint_fast8_t b = (DMA2_Stream4->CR & DMA_SxCR_CT) != 0;
		//__DMB();	//ensure the ordering of data cache maintenance operations and their effects

		if (b != 0)
		{
			release_dmabuffer16tx(DMA2_Stream4->M0AR);
			DMA2_Stream4->M0AR = dma_flush16tx(getfilled_dmabuffer16txphones());
			DRD(DMA2_Stream4->M0AR);
		}
		else
		{
			release_dmabuffer16tx(DMA2_Stream4->M1AR);
			DMA2_Stream4->M1AR = dma_flush16tx(getfilled_dmabuffer16txphones());
			DRD(DMA2_Stream4->M1AR);
		}
	}

	//DMAERR(DMA2, DMA2_Stream4, HISR, HIFCR, DMA_HISR_TEIF4, DMA_HIFCR_CTEIF4);
	HANDLEERRORS(2, 4, H);
}

// TX	SAI2_A	DMA2	Stream 4	Channel 3
// Use dcache_clean
void DMA2_Stream4_IRQHandler_fpga_tx(void)
{
	// проверка условия может потребоваться при добавлении обработчика ошибки
	if ((DMA2->HISR & DMA_HISR_TCIF4) != 0)
	{
		DMA2->HIFCR = DMA_HIFCR_CTCIF4;	// Clear TC interrupt flag соответствующий stream
		ASSERT((SAI2_Block_A->SR & SAI_xSR_OVRUDR_Msk) == 0);
		const uint_fast8_t b = (DMA2_Stream4->CR & DMA_SxCR_CT) != 0;
		//__DMB();	//ensure the ordering of data cache maintenance operations and their effects

		if (b != 0)
		{
			release_dmabuffer32tx(DMA2_Stream4->M0AR);
			DMA2_Stream4->M0AR = dma_flush32tx(getfilled_dmabuffer32tx_main());
			DRD(DMA2_Stream4->M0AR);
		}
		else
		{
			release_dmabuffer32tx(DMA2_Stream4->M1AR);
			DMA2_Stream4->M1AR =  dma_flush32tx(getfilled_dmabuffer32tx_main());
			DRD(DMA2_Stream4->M1AR);
		}
	}

	//DMAERR(DMA2, DMA2_Stream4, HISR, HIFCR, DMA_HISR_TEIF4, DMA_HIFCR_CTEIF4);
	HANDLEERRORS(2, 4, H);
}

// TX	SAI2_A	DMA2	Stream 4	Channel 3
// Use dcache_clean
void DMA2_Stream4_IRQHandler_32txsub(void)
{
	// проверка условия может потребоваться при добавлении обработчика ошибки
	if ((DMA2->HISR & DMA_HISR_TCIF4) != 0)
	{
		DMA2->HIFCR = DMA_HIFCR_CTCIF4;	// Clear TC interrupt flag соответствующий stream
		ASSERT((SAI2_Block_A->SR & SAI_xSR_OVRUDR_Msk) == 0);
		const uint_fast8_t b = (DMA2_Stream4->CR & DMA_SxCR_CT) != 0;
		//__DMB();	//ensure the ordering of data cache maintenance operations and their effects

		if (b != 0)
		{
			release_dmabuffer32tx(DMA2_Stream4->M0AR);
			DMA2_Stream4->M0AR = dma_flush32tx(getfilled_dmabuffer32tx_sub());
			DRD(DMA2_Stream4->M0AR);
		}
		else
		{
			release_dmabuffer32tx(DMA2_Stream4->M1AR);
			DMA2_Stream4->M1AR = dma_flush32tx(getfilled_dmabuffer32tx_sub());
			DRD(DMA2_Stream4->M1AR);
		}
	}

	//DMAERR(DMA2, DMA2_Stream4, HISR, HIFCR, DMA_HISR_TEIF4, DMA_HIFCR_CTEIF4);
	HANDLEERRORS(2, 4, H);
}

// TX	SAI2_A	DMA2	Stream 4	Channel 3
// Use dcache_clean
static void DMA_SAI2_A_TX_initialize_32TXSUB(void)
{

#if CPUSTYLE_STM32MP1
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMA2EN; // включил DMA2
	(void) RCC->MP_AHB2ENSETR;
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMAMUXEN; // включил DMAMUX
	(void) RCC->MP_AHB2ENSETR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream4->PAR = (uintptr_t) & SAI2_Block_A->DR;

#elif CPUSTYLE_STM32H7XX
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream4->PAR = (uintptr_t) & SAI2_Block_A->DR;

#else /* others */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	const uint_fast8_t ch = 3;
	DMA2_Stream4->PAR = (uintptr_t) & SAI2_Block_A->DR;

#endif /* CPUSTYLE_STM32MP1 */

	DMA2_Stream4->M0AR = dma_flush32tx(allocate_dmabuffer32tx());
	DMA2_Stream4->M1AR = dma_flush32tx(allocate_dmabuffer32tx());
	DMA2_Stream4->NDTR = (DMA2_Stream4->NDTR & ~ DMA_SxNDT) |
		(DMABUFFSIZE32TX * DMA_SxNDT_0);

	DMA2_Stream4->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use direct mode
	DRD(DMA2_Stream4->FCR);
	DMA2_Stream4->CR =
		ch * DMA_SxCR_CHSEL_0 | //канал
		0 * DMA_SxCR_MBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_PBURST_0 |	// 0: single transfer
		1 * DMA_SxCR_DIR_0 | //направление - память - периферия
		1 * DMA_SxCR_MINC | //инкремент памяти
		2 * DMA_SxCR_MSIZE_0 | //длина в памяти - 32 bit
		2 * DMA_SxCR_PSIZE_0 | //длина в DR - 32 bit
		(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
		0 * DMA_SxCR_CT | // M0AR selected
		1 * DMA_SxCR_DBM | // double buffer mode seelcted
		0;

#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel12->CCR = 89 * DMAMUX_CxCR_DMAREQ_ID_0;	// SAI2_A
	DRD(DMAMUX1_Channel12->CCR);
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */


	DMA2->HIFCR = DMA_HIFCR_CTCIF4;	// Clear TC interrupt flag соответствующий stream
	DMA2_Stream4->CR |= DMA_SxCR_TCIE;	// Разрешаем прерывания от DMA

	arm_hardware_set_handler_realtime(DMA2_Stream4_IRQn, DMA2_Stream4_IRQHandler_32txsub);

	DMA2_Stream4->CR |= DMA_SxCR_EN;
}

// TX	SAI2_A	DMA2	Stream 4	Channel 3
// Use dcache_clean
static void DMA_SAI2_A_TX_initialize_codec1(void)
{
#if CPUSTYLE_STM32MP1
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMA2EN; // включил DMA2
	(void) RCC->MP_AHB2ENSETR;
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMAMUXEN; // включил DMAMUX
	(void) RCC->MP_AHB2ENSETR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream4->PAR = (uintptr_t) & SAI2_Block_A->DR;

#elif CPUSTYLE_STM32H7XX
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream4->PAR = (uintptr_t) & SAI2_Block_A->DR;

#else /* others */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	const uint_fast8_t ch = 3;
	DMA2_Stream4->PAR = (uintptr_t) & SAI2_Block_A->DR;

#endif /* CPUSTYLE_STM32MP1 */

	DMA2_Stream4->M0AR = dma_flush16tx(allocate_dmabuffer16tx());
	DMA2_Stream4->M1AR = dma_flush16tx(allocate_dmabuffer16tx());
	DMA2_Stream4->NDTR = (DMA2_Stream4->NDTR & ~ DMA_SxNDT) |
		(DMABUFFSIZE16TX * DMA_SxNDT_0);

	DMA2_Stream4->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use direct mode
	DRD(DMA2_Stream4->FCR);
	DMA2_Stream4->CR =
		ch * DMA_SxCR_CHSEL_0 | //канал
		0 * DMA_SxCR_MBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_PBURST_0 |	// 0: single transfer
		1 * DMA_SxCR_DIR_0 | //направление - память - периферия
		1 * DMA_SxCR_MINC | //инкремент памяти
		(CODEC1_DMA_SxCR_xSIZE << DMA_SxCR_MSIZE_Pos) | //длина в памяти - 16 bit
		(CODEC1_DMA_SxCR_xSIZE << DMA_SxCR_PSIZE_Pos) | //длина в DR - 16 bit
		(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
		0 * DMA_SxCR_CT | // M0AR selected
		1 * DMA_SxCR_DBM | // double buffer mode seelcted
		0;
	DRD(DMA2_Stream4->CR);

#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel12->CCR = 89 * DMAMUX_CxCR_DMAREQ_ID_0;	// SAI2_A
	DRD(DMAMUX1_Channel12->CCR);
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */


	DMA2->HIFCR = DMA_HIFCR_CTCIF4;	// Clear TC interrupt flag соответствующий stream
	DMA2_Stream4->CR |= DMA_SxCR_TCIE;	// Разрешаем прерывания от DMA

	arm_hardware_set_handler_realtime(DMA2_Stream4_IRQn, DMA2_Stream4_IRQHandler_codec1_tx);

	DMA2_Stream4->CR |= DMA_SxCR_EN;
}

// TX	SAI2_A	DMA2	Stream 4	Channel 3
// Use dcache_clean
static void DMA_SAI2_A_TX_initialize_fpga(void)
{
#if CPUSTYLE_STM32MP1
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMA2EN; // включил DMA2
	(void) RCC->MP_AHB2ENSETR;
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMAMUXEN; // включил DMAMUX
	(void) RCC->MP_AHB2ENSETR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream4->PAR = (uintptr_t) & SAI2_Block_A->DR;

#elif CPUSTYLE_STM32H7XX
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream4->PAR = (uintptr_t) & SAI2_Block_A->DR;

#else /* others */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	const uint_fast8_t ch = 3;
	DMA2_Stream4->PAR = (uintptr_t) & SAI2_Block_A->DR;

#endif /* CPUSTYLE_STM32MP1 */

	DMA2_Stream4->M0AR =  dma_flush32tx(allocate_dmabuffer32tx());
	DMA2_Stream4->M1AR =  dma_flush32tx(allocate_dmabuffer32tx());
	DMA2_Stream4->NDTR = (DMA2_Stream4->NDTR & ~ DMA_SxNDT) |
		(DMABUFFSIZE32TX * DMA_SxNDT_0);

	DMA2_Stream4->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use direct mode
	DRD(DMA2_Stream4->FCR);
	DMA2_Stream4->CR =
		ch * DMA_SxCR_CHSEL_0 | //канал
		0 * DMA_SxCR_MBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_PBURST_0 |	// 0: single transfer
		1 * DMA_SxCR_DIR_0 | //направление - память - периферия
		1 * DMA_SxCR_MINC | //инкремент памяти
		(FPGAIF_DMA_SxCR_xSIZE << DMA_SxCR_MSIZE_Pos) | //длина в памяти - 32 bit
		(FPGAIF_DMA_SxCR_xSIZE << DMA_SxCR_PSIZE_Pos) | //длина в DR - 32 bit
		(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
		0 * DMA_SxCR_CT | // M0AR selected
		1 * DMA_SxCR_DBM | // double buffer mode seelcted
		0;
	DRD(DMA2_Stream4->CR);

#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel12->CCR = 89 * DMAMUX_CxCR_DMAREQ_ID_0;	// SAI2_A
	DRD(DMAMUX1_Channel12->CCR);
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */


	DMA2->HIFCR = DMA_HIFCR_CTCIF4;	// Clear TC interrupt flag соответствующий stream
	DMA2_Stream4->CR |= DMA_SxCR_TCIE;	// Разрешаем прерывания от DMA

	arm_hardware_set_handler_realtime(DMA2_Stream4_IRQn, DMA2_Stream4_IRQHandler_fpga_tx);

	DMA2_Stream4->CR |= DMA_SxCR_EN;
}

/* DMA для прёма по SAI_2_B  - инициализация */
//	RX	SAI2_B	DMA2	Stream7	Channel 0	
static void DMA_SAI2_B_RX_initialize_RTS192(void)
{
#if CPUSTYLE_STM32MP1
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMA2EN; // включил DMA2
	(void) RCC->MP_AHB2ENSETR;
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMAMUXEN; // включил DMAMUX
	(void) RCC->MP_AHB2ENSETR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream7->PAR = (uintptr_t) & SAI2_Block_B->DR;

#elif CPUSTYLE_STM32H7XX
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream7->PAR = (uintptr_t) & SAI2_Block_B->DR;

#else /* others */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	/* DMA2	Stream7	Channel 0 */
	const uint_fast8_t ch = 0;
	DMA2_Stream7->PAR = (uintptr_t) & SAI2_Block_B->DR;

#endif /* CPUSTYLE_STM32MP1 */

	DMA2_Stream7->M0AR = dma_invalidate192rts(allocate_dmabuffer192rts());
	DMA2_Stream7->M1AR = dma_invalidate192rts(allocate_dmabuffer192rts());
	DMA2_Stream7->NDTR = (DMA2_Stream7->NDTR & ~ DMA_SxNDT) |
		(DMABUFFSIZE192RTS * DMA_SxNDT_0);

	DMA2_Stream7->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use direct mode
	DRD(DMA2_Stream7->FCR);
	DMA2_Stream7->CR =
		ch * DMA_SxCR_CHSEL_0 | // канал
		0 * DMA_SxCR_MBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_PBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_DIR_0 |	// 00: Peripheral-to-memory
		1 * DMA_SxCR_MINC |		//инкремент памяти
		2 * DMA_SxCR_MSIZE_0 | //длина в памяти - 32 bit
		2 * DMA_SxCR_PSIZE_0 | //длина в DR - 32 bit
		(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
		0 * DMA_SxCR_CT |	// M0AR selected
		1 * DMA_SxCR_DBM |	 // double buffer mode seelcted
		0;

#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel15->CCR = 90 * DMAMUX_CxCR_DMAREQ_ID_0;	// SAI2_B
	DRD(DMAMUX1_Channel15->CCR);
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */


	DMA2->HIFCR = (DMA_HIFCR_CTCIF7 /*| DMA_HIFCR_CTEIF7 */);	// Clear TC interrupt flag соответствующий stream
	DMA2_Stream7->CR |= DMA_SxCR_TCIE;	// Разрешаем прерывания от DMA

	arm_hardware_set_handler_realtime(DMA2_Stream7_IRQn, DMA2_Stream7_IRQHandler_fpga_rts192_rx);

	DMA2_Stream7->CR |= DMA_SxCR_EN;
}

/* DMA для прёма по SAI_2_B  - инициализация */
//	RX	SAI2_B	DMA2	Stream7	Channel 0	
static void DMA_SAI2_B_RX_initialize_codec1(void)
{
#if CPUSTYLE_STM32MP1
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMA2EN; // включил DMA2
	(void) RCC->MP_AHB2ENSETR;
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMAMUXEN; // включил DMAMUX
	(void) RCC->MP_AHB2ENSETR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream7->PAR = (uintptr_t) & SAI2_Block_B->DR;

#elif CPUSTYLE_STM32H7XX
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream7->PAR = (uintptr_t) & SAI2_Block_B->DR;

#else /* CPUSTYLE_STM32MP1 */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	/* DMA2	Stream7	Channel 0 */
	const uint_fast8_t ch = 0;
	DMA2_Stream7->PAR = (uintptr_t) & SAI2_Block_B->DR;

#endif /* CPUSTYLE_STM32MP1 */

	DMA2_Stream7->M0AR = dma_invalidate16rx(allocate_dmabuffer16rx());
	DMA2_Stream7->M1AR = dma_invalidate16rx(allocate_dmabuffer16rx());
	DMA2_Stream7->NDTR = (DMA2_Stream4->NDTR & ~ DMA_SxNDT) |
		(DMABUFFSIZE16RX * DMA_SxNDT_0);

	DMA2_Stream7->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use direct mode
	DRD(DMA2_Stream7->FCR);
	DMA2_Stream7->CR =
		ch * DMA_SxCR_CHSEL_0 | // канал
		0 * DMA_SxCR_MBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_PBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_DIR_0 |	// 00: Peripheral-to-memory
		1 * DMA_SxCR_MINC |		//инкремент памяти
		(CODEC1_DMA_SxCR_xSIZE << DMA_SxCR_MSIZE_Pos) | //длина в памяти - 16/32 bit
		(CODEC1_DMA_SxCR_xSIZE << DMA_SxCR_PSIZE_Pos)  | //длина в DR - 16/32 bit
		(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
		0 * DMA_SxCR_CT |	// M0AR selected
		1 * DMA_SxCR_DBM |	 // double buffer mode seelcted
		0;

#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel15->CCR = 90 * DMAMUX_CxCR_DMAREQ_ID_0;	// SAI2_B
	DRD(DMAMUX1_Channel15->CCR);
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */


	DMA2->HIFCR = (DMA_HIFCR_CTCIF7 /*| DMA_HIFCR_CTEIF7 */);	// Clear TC interrupt flag соответствующий stream
	DMA2_Stream7->CR |= DMA_SxCR_TCIE;	// Разрешаем прерывания от DMA

	arm_hardware_set_handler_realtime(DMA2_Stream7_IRQn, DMA2_Stream7_IRQHandler_codec1_rx);

	DMA2_Stream7->CR |= DMA_SxCR_EN;
}

/* DMA для прёма по SAI_2_B  - инициализация */
//	RX	SAI2_B	DMA2	Stream7	Channel 0
static void DMA_SAI2_B_RX_initialize_fpga(void)
{
#if CPUSTYLE_STM32MP1
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMA2EN; // включил DMA2
	(void) RCC->MP_AHB2ENSETR;
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMAMUXEN; // включил DMAMUX
	(void) RCC->MP_AHB2ENSETR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream7->PAR = (uintptr_t) & SAI2_Block_B->DR;

#elif CPUSTYLE_STM32H7XX
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream7->PAR = (uintptr_t) & SAI2_Block_B->DR;

#else /* CPUSTYLE_STM32MP1 */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	/* DMA2	Stream7	Channel 0 */
	const uint_fast8_t ch = 0;
	DMA2_Stream7->PAR = (uintptr_t) & SAI2_Block_B->DR;

#endif /* CPUSTYLE_STM32MP1 */

	DMA2_Stream7->M0AR = dma_invalidate32rx(allocate_dmabuffer32rx());
	DMA2_Stream7->M1AR = dma_invalidate32rx(allocate_dmabuffer32rx());
	DMA2_Stream7->NDTR = (DMA2_Stream4->NDTR & ~ DMA_SxNDT) |
		(DMABUFFSIZE32RX * DMA_SxNDT_0);

	DMA2_Stream7->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use direct mode
	DRD(DMA2_Stream7->FCR);
	DMA2_Stream7->CR =
		ch * DMA_SxCR_CHSEL_0 | // канал
		0 * DMA_SxCR_MBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_PBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_DIR_0 |	// 00: Peripheral-to-memory
		1 * DMA_SxCR_MINC |		//инкремент памяти
		(FPGAIF_DMA_SxCR_xSIZE << DMA_SxCR_MSIZE_Pos) | //длина в памяти - 32 bit
		(FPGAIF_DMA_SxCR_xSIZE << DMA_SxCR_PSIZE_Pos)  | //длина в DR - 32 bit
		(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
		0 * DMA_SxCR_CT |	// M0AR selected
		1 * DMA_SxCR_DBM |	 // double buffer mode seelcted
		0;

#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel15->CCR = 90 * DMAMUX_CxCR_DMAREQ_ID_0;	// SAI2_B
	DRD(DMAMUX1_Channel15->CCR);
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */


	DMA2->HIFCR = (DMA_HIFCR_CTCIF7 /*| DMA_HIFCR_CTEIF7 */);	// Clear TC interrupt flag соответствующий stream
	DMA2_Stream7->CR |= DMA_SxCR_TCIE;	// Разрешаем прерывания от DMA

	arm_hardware_set_handler_realtime(DMA2_Stream7_IRQn, DMA2_Stream7_IRQHandler_fpga_rx);

	DMA2_Stream7->CR |= DMA_SxCR_EN;
}

#if WITHFPGARTS_FRAMEBITS
/* инициализация SAI2 */
// Обмен 24-битами в 32-х битном слове (три первых слота в каждой половине фрейма) =
// аудиоданные начиная с младшего байта - для прямой передачи в USB AUDIO
static void hardware_sai2_slave_duplex_initialize_WFM(void)
{
	PRINTF(PSTR("hardware_sai2_slave_duplex_initialize_WFM start\n"));
	hardware_sai1_sai2_clock_selection();

#if CPUSTYLE_STM32MP1
	// Теперь настроим модуль SAI.
	RCC->MP_APB2ENSETR = RCC_MP_APB2ENSETR_SAI2EN; //подать тактирование
	(void) RCC->MP_APB2ENSETR;
	RCC->MP_APB2LPENSETR = RCC_MP_APB2LPENSETR_SAI2LPEN; //подать тактирование
	(void) RCC->MP_APB2LPENSETR;
#else /* CPUSTYLE_STM32MP1 */
	// Теперь настроим модуль SAI.
	RCC->APB2ENR |= RCC_APB2ENR_SAI2EN; //подать тактирование
	(void) RCC->APB2ENR;
#endif /* CPUSTYLE_STM32MP1 */
	

	SAI2_Block_A->CR1 &= ~ SAI_xCR1_SAIEN;
	SAI2_Block_B->CR1 &= ~ SAI_xCR1_SAIEN;

	// SAI_xCR1_CKSTR:
	// 0: Signals generated by the SAI change on SCK rising edge, 
	//		while signals received by the SAI are sampled on the SCK falling edge.
	// 1: Signals generated by the SAI change on SCK falling edge, 
	//		while signals received by the SAI are sampled on the SCK rising edge.

	// CR1 value (синхронизация с SAI1)
	const portholder_t commoncr1 =
		(1 * SAI_xCR1_OUTDRIV) |	//
		//(0 * SAI_xCR1_MONO) |	// stereo MODE - with IQ pairs - see DMABUFFSIZE32
		(1 * SAI_xCR1_DMAEN) |	// 1: DMA enable 
		(1 * SAI_xCR1_CKSTR) |	// 0: данные на выходе меняются по нарастающему фронту, 1: по спадающему
		(FPGARTS_SAI_CR1_DS * SAI_xCR1_DS_0) |	// 6: data size - 24 bit, 7: 32 bit, 4: 16 bit
		(0 * SAI_xCR1_PRTCFG_0) |	// 0: free protocol
		//(1 * SAI_xCR1_NODIV) |	// 1: no MCKDIV value
		0;

	// SAI_xCR1_SYNCEN values:
	// 0: audio sub-block in asynchronous mode.
	// 1: audio sub-block is synchronous with the other internal audio sub-block. In this case, the audio 	sub-block must be configured in slave mode
	// 2: audio sub-block is synchronous with an external SAI embedded peripheral. In this case the audio sub-block should be configured in Slave mode.
	// 3: Reserved

	SAI2_Block_A->CR1 = 
		commoncr1 |
		(2 * SAI_xCR1_SYNCEN_0) |	// SYNChronization ENable: 2: audio sub-block is synchronous with an external SAI embedded peripheral. In this case the audio sub-block should be configured in Slave mode.
		(2 * SAI_xCR1_MODE_0) |	// 0: Master transmitter, 1: Master receiver, 2: Slave transmitter, 3: Slave receiver
		0;
	SAI2_Block_B->CR1 = 
		commoncr1 |
		(1 * SAI_xCR1_SYNCEN_0) |	// SYNChronization ENable: audio sub-block is synchronous with the other internal audio sub-block. In this case, the audio sub-block must be configured in slave mode
		(3 * SAI_xCR1_MODE_0) |		// 0: Master transmitter, 1: Master receiver, 2: Slave transmitter, 3: Slave receiver
		0;

	// CR2 value
	const portholder_t commoncr2 =
		//(2 * SAI_xCR2_FTH_0) |
		0;
	SAI2_Block_A->CR2 = 
		commoncr2 |
		0;
	SAI2_Block_B->CR2 = 
		commoncr2 |
		0;
	// SLOTR value
	const portholder_t commonslotr =
		(0 * SAI_xSLOTR_SLOTSZ_0) |	// 00: The slot size is equivalent to the data size (specified in DS[3:0] in the SAI_xCR1 register).
		((NBSLOT_SAIRTS - 1) * SAI_xSLOTR_NBSLOT_0) | // Number of slots in audio Frame
		//SAI_xSLOTR_SLOTEN |			// all slots enabled
		//(3UL << 16) |
		//(1 * SAI_xSLOTR_FBOFF_0) | // slot offset - "1" for I2S 24 bit in 32 bit slot
		0;
	SAI2_Block_A->SLOTR = 
		((SLOTEN_TX_SAIRTS << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk) |			// TX slots enabled
		commonslotr |
		0;
	SAI2_Block_B->SLOTR = 
		((SLOTEN_RX_SAIRTS << SAI_xSLOTR_SLOTEN_Pos) & SAI_xSLOTR_SLOTEN_Msk) |			// RX slots enabled
		commonslotr |
		0;

	// FRCR value
	const portholder_t comm_frcr =
		((WITHFPGARTS_FRAMEBITS - 1) * SAI_xFRCR_FRL_0) |
		(1 * SAI_xFRCR_FSDEF) |		/* FS signal is a start of frame signal + channel side identification - must be set for I2S */
		((WITHFPGARTS_FRAMEBITS / 2 - 1) * SAI_xFRCR_FSALL_0) |
#if WITHFPGARTS_FORMATI2S_PHILIPS
		(1 * SAI_xFRCR_FSOFF) |		/* 1: FS is asserted one bit before the first bit of the slot 0. Classic I2S form Philips Semiconductors. "0" value for Sony I2S specs. */
		(0 * SAI_xFRCR_FSPO) |	/* 0: канал с индексом 0 передается при "0" на WS - перывм. */
#endif /* WITHFPGARTS_FORMATI2S_PHILIPS */
		0;
	// FRAME CONTROL REGISTER
	SAI2_Block_A->FRCR =
		comm_frcr |
		0;
	SAI2_Block_B->FRCR =
		comm_frcr |
		0;

	// Формируем синхронизацию для SAI2
	// SAI_GCR_SYNCOUT: 1: Block A used for further synchronization for others SAI
	SAI1->GCR = (SAI1->GCR & ~ (SAI_GCR_SYNCIN | SAI_GCR_SYNCOUT)) |
		0 * SAI_GCR_SYNCIN_0 |
		1 * SAI_GCR_SYNCOUT_0 |
		0;

	// Получаем синхронизацию от SAI1
	// SAI2 can select the synchronization from SAI1 by setting SAI2 SYNCIN to 0
	SAI2->GCR = (SAI2->GCR & ~ (SAI_GCR_SYNCIN | SAI_GCR_SYNCOUT)) |
		0 * SAI_GCR_SYNCIN_0 |
		0 * SAI_GCR_SYNCOUT_0 |
		0;

	SAI2HW_INITIALIZE();
	PRINTF(PSTR("hardware_sai2_slave_duplex_initialize_WFM done\n"));
}

static void hardware_sai2_a_enable_WFM(uint_fast8_t state)		/* разрешение работы SAI2 на STM32F4xx */
{
	// при dual watch используется SAI2, но
	// через него не передаются данные.
	// Для работы синхронизации запукаются обе части - и приём и передача - в SAI2

	if (state != 0)
	{
		SAI2_Block_A->CR1 |= SAI_xCR1_SAIEN;
	}
	else
	{
		SAI2_Block_A->CR1 &= ~ SAI_xCR1_SAIEN;
	}
}

static void hardware_sai2_b_enable_WFM(uint_fast8_t state)		/* разрешение работы SAI2 на STM32F4xx */
{
	// при dual watch используется SAI2, но
	// через него не передаются данные.
	// Для работы синхронизации запукаются обе части - и приём и передача - в SAI2

	if (state != 0)
	{
		SAI2_Block_B->CR1 |= SAI_xCR1_SAIEN;
	}
	else
	{
		SAI2_Block_B->CR1 &= ~ SAI_xCR1_SAIEN;
	}
}

#endif /* WITHFPGARTS_FRAMEBITS */


/* DMA для прёма по SAI_2_B  - инициализация */
//	RX	SAI2_B	DMA2	Stream7	Channel 0	
static void DMA_SAI2_B_RX_initializeWFM(void)
{
	PRINTF(PSTR("DMA_SAI2_B_RX_initializeWFM start.\n"));

#if CPUSTYLE_STM32MP1
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMA2EN; // включил DMA2
	(void) RCC->MP_AHB2ENSETR;
	RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_DMAMUXEN; // включил DMAMUX
	(void) RCC->MP_AHB2ENSETR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream7->PAR = (uintptr_t) & SAI2_Block_B->DR;

#elif CPUSTYLE_STM32H7XX
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
	DMA2_Stream7->PAR = (uintptr_t) & SAI2_Block_B->DR;

#else /* others */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2
	(void) RCC->AHB1ENR;
	/* DMA2	Stream7	Channel 0 */
	const uint_fast8_t ch = 0;
	DMA2_Stream7->PAR = (uintptr_t) & SAI2_Block_B->DR;

#endif /* CPUSTYLE_STM32MP1 */

	DMA2_Stream7->M0AR = dma_invalidate32rx(allocate_dmabuffer32rx());
	DMA2_Stream7->M1AR = dma_invalidate32rx(allocate_dmabuffer32rx());
	DMA2_Stream7->NDTR = (DMA2_Stream7->NDTR & ~ DMA_SxNDT) |
		(DMABUFFSIZE32RX * DMA_SxNDT_0);

	DMA2_Stream7->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use direct mode
	DRD(DMA2_Stream7->FCR);
	DMA2_Stream7->CR =
		ch * DMA_SxCR_CHSEL_0 | // канал
		0 * DMA_SxCR_MBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_PBURST_0 |	// 0: single transfer
		0 * DMA_SxCR_DIR_0 |	// 00: Peripheral-to-memory
		1 * DMA_SxCR_MINC |		//инкремент памяти
		2 * DMA_SxCR_MSIZE_0 | //длина в памяти - 32 bit
		2 * DMA_SxCR_PSIZE_0 | //длина в DR - 32 bit
		(DMA_SxCR_PL_VALUE << DMA_SxCR_PL_Pos) |		// Priority level - High
		0 * DMA_SxCR_CT |	// M0AR selected
		1 * DMA_SxCR_DBM |	 // double buffer mode seelcted
		0;

#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel15->CCR = 90 * DMAMUX_CxCR_DMAREQ_ID_0;	// SAI2_B
	DRD(DMAMUX1_Channel15->CCR);
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */


	DMA2->HIFCR = (DMA_HIFCR_CTCIF7 /*| DMA_HIFCR_CTEIF7 */);	// Clear TC interrupt flag соответствующий stream
	DMA2_Stream7->CR |= DMA_SxCR_TCIE;	// Разрешаем прерывания от DMA

	arm_hardware_set_handler_realtime(DMA2_Stream7_IRQn, DMA2_Stream7_IRQHandler_wfm_rx);

	DMA2_Stream7->CR |= DMA_SxCR_EN;
	PRINTF(PSTR("DMA_SAI2_B_RX_initializeWFM done.\n"));
}

static void hardware_sai2_a_enable_fpga(uint_fast8_t state)		/* разрешение работы SAI1 на STM32F4xx */
{
	if (state != 0)
	{
		SAI2_Block_A->CR1 |= SAI_xCR1_SAIEN;
	}
	else
	{
		SAI2_Block_A->CR1 &= ~ SAI_xCR1_SAIEN;
	}
}

static void hardware_sai2_b_enable_fpga(uint_fast8_t state)		/* разрешение работы SAI1 на STM32F4xx */
{
	if (state != 0)
	{
		SAI2_Block_B->CR1 |= SAI_xCR1_SAIEN;
	}
	else
	{
		SAI2_Block_B->CR1 &= ~ SAI_xCR1_SAIEN;
	}
}

static void hardware_sai2_a_enable_wfm(uint_fast8_t state)		/* разрешение работы SAI1 на STM32F4xx */
{
	if (state != 0)
	{
		SAI2_Block_A->CR1 |= SAI_xCR1_SAIEN;
	}
	else
	{
		SAI2_Block_A->CR1 &= ~ SAI_xCR1_SAIEN;
	}
}

static void hardware_sai2_b_enable_wfm(uint_fast8_t state)		/* разрешение работы SAI1 на STM32F4xx */
{
	if (state != 0)
	{
		SAI2_Block_B->CR1 |= SAI_xCR1_SAIEN;
	}
	else
	{
		SAI2_Block_B->CR1 &= ~ SAI_xCR1_SAIEN;
	}
}

#if WITHCODEC1_SAI1_A_TX_B_RX_MASTER
static const codechw_t audiocodechw_sai1_a_tx_b_rx_master =
{
	hardware_sai1_a_tx_b_rx_master_initialize_codec1,	/* Интерфейс к НЧ кодеку - микрофон */
	hardware_dummy_initialize,
	DMA_SAI1_B_RX_initialize_codec1,					// DMA по приёму
	DMA_SAI1_A_TX_initialize_codec1,					// DMA по передаче
	hardware_sai1_b_enable_codec1,
	hardware_sai1_a_enable_codec1,
	"audiocodechw-sai1-a-tx-b-rx-master"
};
#endif /* WITHCODEC1_SAI1_A_TX_B_RX_MASTER */

#if WITHCODEC1_SAI2_A_TX_B_RX_MASTER
static const codechw_t audiocodechw_sai2_a_tx_b_rx_master =
{
	hardware_sai2_a_tx_b_rx_master_initialize_codec1,	/* Интерфейс к НЧ кодеку - микрофон */
	hardware_dummy_initialize,
	DMA_SAI2_B_RX_initialize_codec1,					// DMA по приёму
	DMA_SAI2_A_TX_initialize_codec1,					// DMA по передаче
	hardware_sai2_b_enable_codec1,
	hardware_sai2_a_enable_codec1,
	"audiocodechw-sai1-a-tx-b-rx-master"
};
#endif /* WITHCODEC1_SAI2_A_TX_B_RX_MASTER */

static const codechw_t fpgacodechw_sai2_a_tx_b_rx_slave =
{
	hardware_sai2_a_tx_b_rx_slave_initialize_fpga,
	hardware_dummy_initialize,
	DMA_SAI2_B_RX_initialize_fpga,
	DMA_SAI2_A_TX_initialize_fpga,
	hardware_sai2_b_enable_fpga,
	hardware_sai2_a_enable_fpga,
	"fpgacodechw-sai2-slave"
};

static const codechw_t fpgacodechw_sai2_a_tx_b_rx_master =
{
	hardware_sai2_a_tx_b_rx_master_initialize_fpga,
	hardware_dummy_initialize,
	DMA_SAI2_B_RX_initialize_fpga,
	DMA_SAI2_A_TX_initialize_fpga,
	hardware_sai2_b_enable_fpga,
	hardware_sai2_a_enable_fpga,
	"fpgacodechw-sai2-master"
};

#if WITHFPGARTS_FRAMEBITS
	// other CPUs
	static const codechw_t fpgaspectrumhw_rx_sai2 =
	{
		hardware_sai2_slave_duplex_initialize_WFM,
		hardware_dummy_initialize,
		DMA_SAI2_B_RX_initializeWFM,
		hardware_dummy_initialize,
		hardware_sai2_b_enable_wfm,
		hardware_sai2_a_enable_wfm,
		"sai2-fpga spectrum or WFM"
	};
#endif /* WITHFPGARTS_FRAMEBITS */

#endif /* WITHSAI2HW */

#if WITHSAI1HW

	static const codechw_t fpgacodechw_sai1_a_tx_b_rx_master =
	{
		hardware_sai1_master_duplex_initialize_fpga,
		hardware_dummy_initialize,
		DMA_SAI1_B_RX_initialize_fpga,
		DMA_SAI1_A_TX_initialize_fpga,
		hardware_sai1_b_enable_fpga,
		hardware_sai1_a_enable_fpga,
		"fpgacodechw-sai1-master"
	};

	static const codechw_t fpgacodechw_sai1_a_tx_b_rx_slave =
	{
		hardware_sai1_slave_duplex_initialize_fpga,
		hardware_dummy_initialize,
		DMA_SAI1_B_RX_initialize_fpga,
		DMA_SAI1_A_TX_initialize_fpga,
		hardware_sai1_b_enable_fpga,
		hardware_sai1_a_enable_fpga,
		"fpgacodechw-sai1-slave"
	};

#endif /* WITHSAI1HW */

#elif CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64


#define DMAC_REG0_MASK(ch) ((ch) >= 8 ? 0u : (1u << ((ch) * 4)))
#define DMAC_REG1_MASK(ch) ((ch) < 8 ? 0u : (1u << (((ch) - 8) * 4)))

/* Обработчики прерываний от DMAC в зависимости от номера канала */
static void (* dmac_handlers [16])(unsigned dmach);

/* Обработчик прерывания от DMAC */
static void DMAC_NS_IRQHandler(void)
{
	// 0x04: Queue, 0x02: Pkq, 0x01: half
	const unsigned flag = 0x07;
	unsigned dmach;
#if CPUSTYLE_A64
	const portholder_t reg0 = DMAC->DMAC_IRQ_PEND_REG & DMAC->DMAC_IRQ_EN_REG;

	DMAC->DMAC_IRQ_PEND_REG = reg0;	// Write 1 to clear the pending status.
#else /* CPUSTYLE_A64 */
	const portholder_t reg0 = DMAC->DMAC_IRQ_PEND_REG0 & DMAC->DMAC_IRQ_EN_REG0;
	const portholder_t reg1 = DMAC->DMAC_IRQ_PEND_REG1 & DMAC->DMAC_IRQ_EN_REG1;

	DMAC->DMAC_IRQ_PEND_REG0 = reg0;	// Write 1 to clear the pending status.
	DMAC->DMAC_IRQ_PEND_REG1 = reg1;	// Write 1 to clear the pending status.
#endif /* CPUSTYLE_A64 */

	for (dmach = 0; dmach < 8; ++ dmach)
	{
		const portholder_t maskreg0 = DMAC_REG0_MASK(dmach) * flag;
		if ((reg0 & maskreg0) != 0)
		{
			dmac_handlers [dmach](dmach);
		}
	}
#if ! CPUSTYLE_A64
	for (dmach = 8; dmach < 16; ++ dmach)
	{
		const portholder_t maskreg1 = DMAC_REG1_MASK(dmach) * flag;
		if ((reg1 & maskreg1) != 0)
		{
			dmac_handlers [dmach](dmach);
		}
	}
#endif /* ! CPUSTYLE_A64 */
}


static unsigned ratio2div(unsigned ratio)
{
	enum divs
	{
		CLKD_Div1 = 0x01,
		CLKD_Div2,		// 0x02
		CLKD_Div4,		// 0x03
		CLKD_Div6,		// 0x04
		CLKD_Div8,		// 0x05
		CLKD_Div12,		// 0x06
		CLKD_Div16,		// 0x07
		CLKD_Div24,		// 0x08
		CLKD_Div32,		// 0x09
		CLKD_Div48,		// 0x0A
		CLKD_Div64,		// 0x0B
		CLKD_Div96,		// 0x0C
		CLKD_Div128,	// 0x0D
		CLKD_Div176,	// 0x0E
		CLKD_Div192,	// 0x0F
	};
	switch (ratio)
	{
	case 1:		return CLKD_Div1;
	case 2:		return CLKD_Div2;
	case 4:		return CLKD_Div4;
	case 6:		return CLKD_Div6;
	case 8:		return CLKD_Div8;
	case 12:	return CLKD_Div12;
	case 16:	return CLKD_Div16;
	case 24:	return CLKD_Div24;
	case 32:	return CLKD_Div32;
	case 48:	return CLKD_Div48;
	case 64:	return CLKD_Div64;
	case 96:	return CLKD_Div96;
	case 128:	return CLKD_Div128;
	case 176:	return CLKD_Div176;
	case 192:	return CLKD_Div192;
	default:	return CLKD_Div192;
	}
}

// I2S_PCM_FMT0 Sample Resolution
static unsigned width2fmt(unsigned width)
{
	switch (width)
	{
	default:
	case 8:			return 0x01;
	case 12:		return 0x02;
	case 16:		return 0x03;
	case 20:		return 0x04;
	case 24:		return 0x05;
	case 28:		return 0x06;
	case 32:		return 0x07;
	}
}

/* I2S/PCM RX Channel Mapping Registers initialization */
/* Простое отображение каналов с последовательно увеличивающимся номером */
static void I2S_fill_RXCHMAP(
	I2S_PCM_TypeDef * i2s,
	unsigned rxsdi,
	unsigned NCH
	)
{
#if CPUSTYLE_A64
#elif CPUSTYLE_T113 || CPUSTYLE_F133

	__IO uint32_t * const reg = & i2s->I2S_PCM_RXCHMAP0;
	unsigned chnl;
	for (chnl = 0; chnl < NCH; ++ chnl)
	{
		/* в каждом регистре управления для восьми каналов */
		const portholder_t mask3 = power8((1u << chnl) >> 0);	// биты в I2S_PCM_RXCHMAP3 - каналы 3..0
		const portholder_t mask2 = power8((1u << chnl) >> 4);	// биты в I2S_PCM_RXCHMAP2 - каналы 7..4
		const portholder_t mask1 = power8((1u << chnl) >> 8);	// биты в I2S_PCM_RXCHMAP1 - каналы 11..8
		const portholder_t mask0 = power8((1u << chnl) >> 12);	// биты в I2S_PCM_RXCHMAP0 - каналы 15..12

		const portholder_t ALLMASK = 0x3F;
		const portholder_t field =
			((portholder_t) rxsdi << 4) |	// RX Channel 0 Select (0..3 - SDI0..SDI3)
			((portholder_t) chnl << 0) |	// RX Channel 0 Mapping (0..15 - sample position)
			0;

		reg [0] = (reg [0] & ~ (mask0 * ALLMASK)) | (mask0 * field);
		reg [1] = (reg [1] & ~ (mask1 * ALLMASK)) | (mask1 * field);
		reg [2] = (reg [2] & ~ (mask2 * ALLMASK)) | (mask2 * field);
		reg [3] = (reg [3] & ~ (mask3 * ALLMASK)) | (mask3 * field);
	}
#endif
}

/* I2S/PCM TX0 Channel Mapping Registers initialization */
/* Простое отображение каналов с последовательно увеличивающимся номером */
static void I2S_fill_TXxCHMAP(
	I2S_PCM_TypeDef * i2s,
	unsigned txoffs,	// 0..3 - I2S_PCM_TX0..I2S_PCM_TX3
	unsigned txsdo_UNUSED,		// 0..3 - DOUT0..DOUT3
	unsigned NCH
	)
{
#if CPUSTYLE_A64
#elif CPUSTYLE_T113 || CPUSTYLE_F133

	__IO uint32_t * const reg = & i2s->I2S_PCM_TX0CHMAP0 + txoffs * 2;

	unsigned chnl;
	for (chnl = 0; chnl < NCH; ++ chnl)
	{
		const portholder_t mask1 = power4((1u << chnl) >> 0);	// биты в I2S_PCM_TX0CHMAP1 - каналы 7..0
		const portholder_t mask0 = power4((1u << chnl) >> 8);	// биты в I2S_PCM_TX0CHMAP0 - каналы 15..8

		const portholder_t ALLMASK = 0x0F;
		const portholder_t field = (portholder_t) chnl << 0;

		reg [0] = (reg [0] & ~ ALLMASK) | mask0 * field;
		reg [1] = (reg [1] & ~ ALLMASK) | mask1 * field;
	}
#endif
}

//
//void I2S_PCM1_IrqHandler(void)
//{
//	const uint_fast32_t ista = I2S1->I2S_PCM_ISTA;
//	I2S1->I2S_PCM_ISTA = ista;
//	PRINTF("I2S_PCM1_IrqHandler: ista=%08" PRIXFAST32 "\n", ista);
//	ASSERT(0);
//}
//
//void I2S_PCM2_IrqHandler(void)
//{
//	const uint_fast32_t ista = I2S2->I2S_PCM_ISTA;
//	I2S2->I2S_PCM_ISTA = ista;
//	PRINTF("I2S_PCM2_IrqHandler: ista=%08" PRIXFAST32 "\n", ista);
//	ASSERT(0);
//}

void I2S_PCMx_IrqHandler(void)
{
//	const uint_fast32_t ista = I2S1->I2S_PCM_ISTA;
//	I2S1->I2S_PCM_ISTA = ista;
//	PRINTF("I2S_PCM1_IrqHandler: ista=%08" PRIXFAST32 "\n", ista);
	ASSERT(0);
	for (;;)
		;
}

static void hardware_i2s_initialize(unsigned ix, I2S_PCM_TypeDef * i2s, int master, unsigned NCH, unsigned lrckf, unsigned framebits, unsigned din, unsigned dout)
{

	const unsigned bclkf = lrckf * framebits;
	const unsigned mclkf = lrckf * 256;

#if CPUSTYLE_A64

	const unsigned irq = I2S_PCM0_IRQn + ix;

	volatile uint32_t * const i2s_clk_reg = & CCU->I2S_PCM_0_CLK_REG + ix;

	//	CLK_SRC_SEL.
	//	00: PLL_AUDIO (8X) - 196571428
	//	01: PLL_AUDIO(8X)/2	- 98285714
	//	10: PLL_AUDIO(8X)/4 - 49142857
	//	11: PLL_AUDIO - 24571428
	const unsigned CLK_SRC_SEL = 0x03;

	* i2s_clk_reg =
		(1u << 31) |	// SCLK_GATING.
		(CLK_SRC_SEL << 16) |	//
		0;

	CCU->BUS_CLK_GATING_REG2 |= 1u << (12 + ix);	// Gating Clock For I2S/PCM-2
	CCU->BUS_SOFT_RST_REG3 |= 1u << (12 + ix);	// I2S/PCM x Reset. 1: De-assert.

	uint_fast32_t clk;
	switch (CLK_SRC_SEL)
	{
	default:
	case 0:
		clk = allwnr_a64_get_audiopll8x_freq();
		break;
	case 1:
		clk = allwnr_a64_get_audiopll8x_freq() / 2;
		break;
	case 2:
		clk = allwnr_a64_get_audiopll8x_freq() / 4;
		break;
	case 3:
		clk = allwnr_a64_get_audiopll_freq();
		break;
	}

//	unsigned value;	/* делитель */
//	const uint_fast8_t prei = calcdivider(calcdivround2(clk, mclkf), ALLWNT113_I2Sx_CLK_WIDTH, ALLWNT113_I2Sx_CLK_TAPS, & value, 1);
//	PRINTF("i2s%u: prei=%u, value=%u, mclkf=%u, (clk=%u)\n", ix, prei, value, mclkf, (unsigned) clk);

	PRINTF("i2s%u: mclkf=%u, clk=%u\n", ix, mclkf, (unsigned) clk);
	PRINTF("CCU->PLL_AUDIO_CTRL_REG=%08X\n", (unsigned) CCU->PLL_AUDIO_CTRL_REG);
	PRINTF("CCU->MBUS_CLK_REG=%08X\n", (unsigned) CCU->MBUS_CLK_REG);

#else
	const unsigned irq = I2S_PCM1_IRQn + ix - 1;

	arm_hardware_disable_handler(irq);
	// ARMI2SRATE // I2S sample rate audio codec (human side)
	// ARMI2SMCLK = ARMI2SRATE * 256
	// ARMSAIRATE // SAI sample rate (FPGA/IF CODEC side)
	// ARMSAIMCLK = ARMSAIRATE * 256
	// CODEC1_FRAMEBITS 64

//	PRINTF("allwnrt113_get_audio0pll1x_freq = %lu\n", allwnrt113_get_audio0pll1x_freq());
//	PRINTF("allwnrt113_get_audio0pll4x_freq = %lu\n", allwnrt113_get_audio0pll4x_freq());
//	PRINTF("allwnrt113_get_audio1pll_div2_freq = %lu\n", allwnrt113_get_audio1pll_div2_freq());
//	PRINTF("allwnrt113_get_audio1pll1x_freq = %lu\n", allwnrt113_get_audio1pll1x_freq());
//	PRINTF("allwnrt113_get_audio1pll_div2_freq = %lu\n", allwnrt113_get_audio1pll_div2_freq());
//	PRINTF("allwnrt113_get_audio1pll_div5_freq = %lu\n", allwnrt113_get_audio1pll_div5_freq());

	//	i2s1: prei=2, value=30, mclkf=12288000, (clk=1536000000)
	//	allwnrt113_get_audio0pll1x_freq = 24.571.428
	//	allwnrt113_get_audio0pll4x_freq = 98.28.5714
	//	allwnrt113_get_audio1pll_div2_freq = 1536.000.000
	//	allwnrt113_get_audio1pll1x_freq = 3072.000.000
	//	allwnrt113_get_audio1pll_div2_freq = 1536.000.000
	//	allwnrt113_get_audio1pll_div5_freq = 614.400.000

	// 0x00, 0x01, 0x03: 48000
	// 0x02: ~ 48350
	const unsigned long src = 0x01;
	// CLK_SRC_SEL:
	// 00: PLL_AUDIO0(1X)
	// 01: PLL_AUDIO0(4X)
	// 10: PLL_AUDIO1(DIV2)
	// 11: PLL_AUDIO1(DIV5)
	unsigned long clk;
	switch (src)
	{
	default:
	case 0x00:
		clk = allwnrt113_get_audio0pll1x_freq();
		break;
	case 0x01:
		clk = allwnrt113_get_audio0pll4x_freq();
		break;
	case 0x02:
		clk = allwnrt113_get_audio1pll_div2_freq();
		break;
	case 0x03:
		clk = allwnrt113_get_audio1pll_div5_freq();
		break;
	}
	//TP();
	unsigned value;	/* делитель */
	const uint_fast8_t prei = calcdivider(calcdivround2(clk, mclkf), ALLWNT113_I2Sx_CLK_WIDTH, ALLWNT113_I2Sx_CLK_TAPS, & value, 1);
	//PRINTF("i2s%u: prei=%u, value=%u, mclkf=%u, (clk=%lu)\n", ix, prei, value, mclkf, clk);

	// CLK_SRC_SEL:
	// 00: PLL_AUDIO0(1X)
	// 01: PLL_AUDIO0(4X)
	// 10: PLL_AUDIO1(DIV2)
	// 11: PLL_AUDIO1(DIV5)
	volatile uint32_t * const i2s_clk_reg = & CCU->I2S1_CLK_REG + ix - 1;

	* i2s_clk_reg =
		(1u << 31) |				// I2S/PCM1_CLK_GATING: 1: Clock is ON
		((uint_fast32_t) src << 24) |	// CLK_SRC_SEL
		((uint_fast32_t) prei << 8) |	// Factor N (0..3: /1 /2 /4 /8)
		((uint_fast32_t) value << 0) |	// Factor M (0..31)
		0;

	CCU->I2S_BGR_REG |= (1u << (0 + ix));	// Gating Clock for I2S/PCMx
	CCU->I2S_BGR_REG |= (1u << (16 + ix));	// I2S/PCMx Reset

#endif

	//PRINTF("allwnrt113_get_i2s1_freq = %lu\n", allwnrt113_get_i2s1_freq());

	// Данные на выходе меняются по спадающему фронту (I2S complaint)
	//	BCLK_POLARITY = 0, EDGE_TRANSFER = 0, DIN sample data at positive edge;
	//	BCLK_POLARITY = 0, EDGE_TRANSFER = 1, DIN sample data at negative edge;
	//	BCLK_POLARITY = 1, EDGE_TRANSFER = 0, DIN sample data at negative edge;
	//	BCLK_POLARITY = 1, EDGE_TRANSFER = 1, DIN sample data at positive edge.
	i2s->I2S_PCM_FMT0 =
		0 * (1u << 7) | 						// BCLK_POLARITY 1: Invert mode, DOUT drives data at positive edge
		0 * (1u << 3) | 						// EDGE_TRANSFER 1: Invert mode, DOUT drives data at positive edge
		((framebits / 2) - 1) * (1u << 8) |	// LRCK_PERIOD - for I2S - each channel width
		width2fmt(framebits / NCH) * (1u << 4) |	// SR Sample Resolution . 0x03 - 16 bit, 0x07 - 32 bit
		width2fmt(framebits / NCH) * (1u << 0) |	// SW Slot Width Select . 0x03 - 16 bit, 0x07 - 32 bit
		0;
	i2s->I2S_PCM_FMT1 =
		0;

	if (framebits < 64)
	{
		// Данные берутся/появляются в младшей части регистра FIFO
		// I2S/PCM FIFO Control Register
		i2s->I2S_PCM_FCTL = (i2s->I2S_PCM_FCTL & ~ (0x07uL)) |
			1 * (1u << 2) |	// TXIM Mode 1: TXFIFO[31:0] = {APB_WDATA[19:0], 12’h0}
			3 * (1u << 0) |	// RXOM Mode 3: APB_RDATA[31:0] = {16{RXFIFO[31], RXFIFO[31:16]}
			0;
	}

	// I2S/PCM Channel Configuration Register
	i2s->I2S_PCM_CHCFG =
		(NCH - 1) * (1u << 4) |	// RX_SLOT_NUM 0111: 7 channel or slot 0001: 2 channel or slot
		(NCH - 1) * (1u << 0) |	// TX_SLOT_NUM 0111: 7 channel or slot 0001: 2 channel or slot
		0;

	// Need i2s1: mclkf=12288000, bclkf=3072000, lrckf=48000
	// (pin P2-5) bclk = 3.4 MHz, BCLKDIV=CLKD_Div64
	// (pin P2-6) lrck = 53 khz
	// (pin P2-7) mclk = 13.7 MHz, MCLKDIV=CLKD_Div16
	// BCLK = MCLK / BCLKDIV
	const unsigned ratio = 256 / framebits;
	const unsigned div4 = 1;
	i2s->I2S_PCM_CLKD =
		1 * (1u << 8) |		// MCLKO_EN
		ratio2div(div4) * (1u << 0) |		/* MCLKDIV */
		ratio2div(ratio) * (1u << 4) |		/* BCLKDIV */
		0;

	//PRINTF("I2S%u: MCLKDIV=%u(%u), BCLKDIV=%u(%u)\n", ix, ratio2div(div4), div4, ratio2div(ratio), ratio);

#if CODEC1_FORMATI2S_PHILIPS
	const unsigned txrx_offset = 1;
#else /* CODEC1_FORMATI2S_PHILIPS */
	const unsigned txrx_offset = 0;
#endif /* CODEC1_FORMATI2S_PHILIPS */

	ASSERT(din < 4);
	ASSERT(dout < 4);
	// I2S/PCM Control Register
	i2s->I2S_PCM_CTL = 0;
	i2s->I2S_PCM_CTL =
		(1u << dout) * (1u << 8) |	// DOUT3_EN..DOUT0_EN
		((uint_fast32_t) master << 18) | // BCLK_OUT
		((uint_fast32_t) master << 17) | // LRCK_OUT
		(1u << 4) |	// left mode, need offset=1 for I2S
		0;

	i2s->I2S_PCM_RXCHSEL =
		txrx_offset * (1u << 20) |	// RX_OFFSET (need for I2S mode
		(NCH - 1) * (1u << 16) |	// RX Channel (Slot) Number Select for Input 0111: 8 channel or slot
		0;


	const portholder_t txchsel =
		txrx_offset * (1u << 20) |	// TX3 Offset Tune (TX3 Data offset to LRCK)
		(NCH - 1) * (1u << 16) |	// TX3 Channel (Slot) Number Select for Each Output
		0xFFFF * (1u << 0) |		// TX3 Channel (Slot) Enable
		0;

	i2s->I2S_PCM_TX0CHSEL = txchsel;
	i2s->I2S_PCM_TX1CHSEL = txchsel;
	i2s->I2S_PCM_TX2CHSEL = txchsel;
	i2s->I2S_PCM_TX3CHSEL = txchsel;

	/* Простое отображение каналов с последовательно увеличивающимся номером */
	I2S_fill_RXCHMAP(i2s, din, NCH);
	I2S_fill_TXxCHMAP(i2s, 0, dout, NCH);	// I2S_PCM_TX0CHMAPx
	I2S_fill_TXxCHMAP(i2s, 1, dout, NCH);	// I2S_PCM_TX1CHMAPx
	I2S_fill_TXxCHMAP(i2s, 2, dout, NCH);	// I2S_PCM_TX2CHMAPx
	I2S_fill_TXxCHMAP(i2s, 3, dout, NCH);	// I2S_PCM_TX3CHMAPx

	i2s->I2S_PCM_INT = 0;
	i2s->I2S_PCM_INT |= (1u << 7); // TX_DRQ
	i2s->I2S_PCM_INT |= (1u << 3); // RX_DRQ

	i2s->I2S_PCM_INT |= (1u << 6); // TXUI_EN TXFIFO Underrun Interrupt Enable
	i2s->I2S_PCM_INT |= (1u << 2); // RXUI_EN RXFIFO Overrun Interrupt Enable

	arm_hardware_set_handler_realtime(irq, I2S_PCMx_IrqHandler);

}

#if CPUSTYLE_A64

static void hardware_i2s0_master_duplex_initialize_codec1(void)
{
	hardware_i2s_initialize(0, I2S0, 1, 2, ARMI2SRATE, CODEC1_FRAMEBITS, HARDWARE_I2S1HW_DIN, HARDWARE_I2S1HW_DOUT);
	I2S0HW_INITIALIZE(1);
}

static void hardware_i2s0_slave_duplex_initialize_codec1(void)
{
	hardware_i2s_initialize(0, I2S0, 0, 2, ARMI2SRATE, CODEC1_FRAMEBITS, HARDWARE_I2S1HW_DIN, HARDWARE_I2S1HW_DOUT);
	I2S0HW_INITIALIZE(0);
}

#endif /* CPUSTYLE_A64 */

// Codec initialize
static void hardware_i2s1_master_duplex_initialize_codec1(void)
{
	hardware_i2s_initialize(1, I2S1, 1, 2, ARMI2SRATE, CODEC1_FRAMEBITS, HARDWARE_I2S1HW_DIN, HARDWARE_I2S1HW_DOUT);
	I2S1HW_INITIALIZE(1);
}

static void hardware_i2s1_slave_duplex_initialize_codec1(void)
{
	hardware_i2s_initialize(1, I2S1, 0, 2, ARMI2SRATE, CODEC1_FRAMEBITS, HARDWARE_I2S1HW_DIN, HARDWARE_I2S1HW_DOUT);
	I2S1HW_INITIALIZE(0);
}

static void hardware_i2s2_slave_duplex_initialize_codec1(void)
{
	hardware_i2s_initialize(2, I2S2, 0, 2, ARMI2SRATE, CODEC1_FRAMEBITS, HARDWARE_I2S2HW_DIN, HARDWARE_I2S2HW_DOUT);
	I2S2HW_INITIALIZE(0);
}

// FPGA interface
static void hardware_i2s2_master_duplex_initialize_fpga(void)
{
	hardware_i2s_initialize(2, I2S2, 1, WITHFPGAIF_FRAMEBITS / 32, ARMSAIRATE, WITHFPGAIF_FRAMEBITS, HARDWARE_I2S2HW_DIN, HARDWARE_I2S2HW_DOUT);
	I2S2HW_INITIALIZE(1);
}

static void hardware_i2s1_slave_duplex_initialize_fpga(void)
{
	hardware_i2s_initialize(1, I2S1, 0, WITHFPGAIF_FRAMEBITS / 32, ARMSAIRATE, WITHFPGAIF_FRAMEBITS, HARDWARE_I2S1HW_DIN, HARDWARE_I2S1HW_DOUT);
	I2S1HW_INITIALIZE(0);
}

static void hardware_i2s2_slave_duplex_initialize_fpga(void)
{
	hardware_i2s_initialize(2, I2S2, 0, WITHFPGAIF_FRAMEBITS / 32, ARMSAIRATE, WITHFPGAIF_FRAMEBITS, HARDWARE_I2S2HW_DIN, HARDWARE_I2S2HW_DOUT);
	I2S2HW_INITIALIZE(0);
}

static void hardware_i2s1_enable_codec1(uint_fast8_t state)
{
	I2S_PCM_TypeDef * const i2s = I2S1;
	if (state)
	{
		i2s->I2S_PCM_CTL |=
			(1u << 2) |	// TXEN
			(1u << 1) |	// RXEN
			0;
		i2s->I2S_PCM_CTL |= (1u << 0); // GEN Globe Enable
	}
	else
	{
		i2s->I2S_PCM_CTL &= ~ (1u << 0); // GEN Globe Enable
		i2s->I2S_PCM_CTL &= ~ (1u << 2); // TXEN
		i2s->I2S_PCM_CTL &= ~ (1u << 1); // RXEN
	}
}

static void hardware_i2s2_enable_codec1(uint_fast8_t state)
{
	I2S_PCM_TypeDef * const i2s = I2S2;
	if (state)
	{
		i2s->I2S_PCM_CTL |=
			(1u << 2) |	// TXEN
			(1u << 1) |	// RXEN
			0;
		i2s->I2S_PCM_CTL |= (1u << 0); // GEN Globe Enable
	}
	else
	{
		i2s->I2S_PCM_CTL &= ~ (1u << 0); // GEN Globe Enable
		i2s->I2S_PCM_CTL &= ~ (1u << 2); // TXEN
		i2s->I2S_PCM_CTL &= ~ (1u << 1); // RXEN
	}
}

static void hardware_i2s1_enable_fpga(uint_fast8_t state)
{
	I2S_PCM_TypeDef * const i2s = I2S1;
	if (state)
	{
		i2s->I2S_PCM_CTL |=
			(1u << 2) |	// TXEN
			(1u << 1) |	// RXEN
			0;
		i2s->I2S_PCM_CTL |= (1u << 0); // GEN Globe Enable
	}
	else
	{
		i2s->I2S_PCM_CTL &= ~ (1u << 0); // GEN Globe Enable
		i2s->I2S_PCM_CTL &= ~ (1u << 2); // TXEN
		i2s->I2S_PCM_CTL &= ~ (1u << 1); // RXEN
	}
}

static void hardware_i2s2_enable_fpga(uint_fast8_t state)
{
	I2S_PCM_TypeDef * const i2s = I2S2;
	if (state)
	{
		i2s->I2S_PCM_CTL |=
			(1u << 2) |	// TXEN
			(1u << 1) |	// RXEN
			0;
		i2s->I2S_PCM_CTL |= (1u << 0); // GEN Globe Enable
	}
	else
	{
		i2s->I2S_PCM_CTL &= ~ (1u << 0); // GEN Globe Enable
		i2s->I2S_PCM_CTL &= ~ (1u << 2); // TXEN
		i2s->I2S_PCM_CTL &= ~ (1u << 1); // RXEN
	}
}

#define DMAC_DESC_SRC	1	/* адрес источника */
#define DMAC_DESC_DST	2	/* адрес получателя */
#define DMAC_DESC_PARAM	4	/* Parameter */
#define DMAC_DESC_LINK	5	/* адрес сдедующего дескриптора */

#define DMAC_DESC_SIZE	8	/* Требуется 6 - но для упрощения работы с кеш-памятью сделано 8 */

// DMA Source/Destination Data Width
// 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
static uint_fast32_t dmac_desc_datawidth(unsigned width)
{
	switch (width)
	{
	default:
	case 8: 	return 0x00;
	case 16: 	return 0x01;
	case 32: 	return 0x02;
	case 64: 	return 0x03;
	}
}

static uintptr_t DMA_suspend(unsigned dmach)
{
//	DMAC->CH [dmach].DMAC_PAU_REGN = 1;	// 1: Suspend Transferring
//	while (DMAC->CH [dmach].DMAC_PAU_REGN == 0)
//		;
	return DMAC->CH [dmach].DMAC_DESC_ADDR_REGN;
//	volatile uint32_t * const descraddr = (volatile uint32_t *) DMAC->CH [dmach].DMAC_DESC_ADDR_REGN;
//	return descraddr [DMAC_DESC_LINK];	/* для срабатывания по half packet - получаем неиспользуемый сейчас дескриптор */
}

static void DMA_resume(unsigned dmach, uintptr_t descbase)
{
//    DMAC->CH [dmach].DMAC_PAU_REGN = 0;	// 0: Resume Transferring
}

/* Приём от кодека */
/* от встроенного в процессор или подключенного по I2S */
static void DMA_I2Sx_AudioCodec_RX_Handler_codec1(unsigned dmach)
{
	enum { ix = DMAC_DESC_DST };
	const uintptr_t descbase = DMA_suspend(dmach);

	volatile uint32_t * const descraddr = (volatile uint32_t *) descbase;
	const uintptr_t addr = descraddr [ix];
	descraddr [ix] = dma_invalidate16rx(allocate_dmabuffer16rx());
	dcache_clean(descbase, DMAC_DESC_SIZE * sizeof (uint32_t));

	DMA_resume(dmach, descbase);
//	printhex32(addr, (void *) addr, DMABUFFSTEP16RX * sizeof (aubufv_t));
//	for (;;)
//		;
	/* Работа с только что принятыми данными */
	processing_dmabuffer16rx(addr);
}


/* Передача в кодек */
/* на встроенный в процессор или подключенный по I2S */
static void DMA_I2Sx_AudioCodec_TX_Handler_codec1(unsigned dmach)
{
	enum { ix = DMAC_DESC_SRC };
	const uintptr_t descbase = DMA_suspend(dmach);

	volatile uint32_t * const descraddr = (volatile uint32_t *) descbase;
	const uintptr_t addr = descraddr [ix];
	descraddr [ix] = dma_flush16tx(getfilled_dmabuffer16txphones());			// Source Address
	dcache_clean(descbase, DMAC_DESC_SIZE * sizeof (uint32_t));

	DMA_resume(dmach, descbase);

	/* Работа с только что передаными данными */
	release_dmabuffer16tx(addr);
}

/* Приём от FPGA */
static void DMA_I2Sx_RX_Handler_fpga(unsigned dmach)
{
	enum { ix = DMAC_DESC_DST };
	const uintptr_t descbase = DMA_suspend(dmach);
	volatile uint32_t * const descraddr = (volatile uint32_t *) descbase;
	const uintptr_t addr = descraddr [ix];
	descraddr [ix] = dma_invalidate32rx(allocate_dmabuffer32rx());
	dcache_clean(descbase, DMAC_DESC_SIZE * sizeof (uint32_t));

	DMA_resume(dmach, descbase);

	/* Работа с только что принятыми данными */
	processing_dmabuffer32rts(addr);
	processing_dmabuffer32rx(addr);
	release_dmabuffer32rx(addr);

	buffers_resampleuacin(DMABUFFSIZE32RX / DMABUFFSTEP32RX);
}

/* Передача в FPGA */
static void DMA_I2Sx_TX_Handler_fpga(unsigned dmach)
{
	enum { ix = DMAC_DESC_SRC };
	const uintptr_t descbase = DMA_suspend(dmach);

	volatile uint32_t * const descraddr = (volatile uint32_t *) descbase;
	const uintptr_t addr = descraddr [ix];
	descraddr [ix] = dma_flush32tx(getfilled_dmabuffer32tx_main());
	dcache_clean(descbase, DMAC_DESC_SIZE * sizeof (uint32_t));

	DMA_resume(dmach, descbase);

	/* Работа с только что передаными данными */
	release_dmabuffer32tx(addr);
}


#define DMAC_IRQ_EN_FLAG_VALUE (0x01)	// 0x04: Queue, 0x02: Pkq, 0x01: half

static void DMAC_SetHandler(unsigned dmach, unsigned flag, void (* handler)(unsigned dmach))
{
	ASSERT(dmach < ARRAY_SIZE(dmac_handlers));
	dmac_handlers [dmach] = handler;
#if CPUSTYLE_A64
	arm_hardware_set_handler_realtime(DMAC_IRQn, DMAC_NS_IRQHandler);

	DMAC->DMAC_IRQ_EN_REG = (DMAC->DMAC_IRQ_EN_REG & ~ (DMAC_REG0_MASK(dmach) * 0x07)) | DMAC_REG0_MASK(dmach) * flag;
#else /* CPUSTYLE_A64 */
	arm_hardware_set_handler_realtime(DMAC_NS_IRQn, DMAC_NS_IRQHandler);

	DMAC->DMAC_IRQ_EN_REG0 = (DMAC->DMAC_IRQ_EN_REG0 & ~ (DMAC_REG0_MASK(dmach) * 0x07)) | DMAC_REG0_MASK(dmach) * flag;
	DMAC->DMAC_IRQ_EN_REG1 = (DMAC->DMAC_IRQ_EN_REG1 & ~ (DMAC_REG1_MASK(dmach) * 0x07)) | DMAC_REG1_MASK(dmach) * flag;
#endif /* CPUSTYLE_A64 */
}

static void DMAC_clock_initialize(void)
{
#if CPUSTYLE_A64

	CCU->BUS_CLK_GATING_REG0 |= (1u << 6);	// DMA_GATING
	CCU->BUS_SOFT_RST_REG0 |= (1u << 6);	// DMA_RST

#else /* CPUSTYLE_A64 */
	CCU->MBUS_CLK_REG |= (1u << 30);		// MBUS Reset 1: De-assert reset
	CCU->MBUS_MAT_CLK_GATING_REG |= (1u << 0);	// Gating MBUS Clock For DMA
	CCU->DMA_BGR_REG |= (1u << 16);		// DMA_RST 1: De-assert reset
	CCU->DMA_BGR_REG |= (1u << 0);			// DMA_GATING 1: Pass clock
#endif /* CPUSTYLE_A64 */

	DMAC->DMAC_AUTO_GATE_REG |= (1u << 2);	// DMA_MCLK_CIRCUIT 1: Auto gating disabled
	DMAC->DMAC_AUTO_GATE_REG |= 0x07;
}

enum
{
	DMAC_I2S0_TX_Ch,
	DMAC_I2S0_RX_Ch,
	DMAC_I2S1_TX_Ch,
	DMAC_I2S1_RX_Ch,
	DMAC_AudioCodec_TX_Ch,// = DMAC_I2S1_TX_Ch,
	DMAC_AudioCodec_RX_Ch,// = DMAC_I2S1_RX_Ch,
	DMAC_I2S2_TX_Ch,
	DMAC_I2S2_RX_Ch,
	//
	DMAC_Ch_Total
};

static void DMAC_I2S1_RX_initialize_codec1(void)
{
	const size_t dw = sizeof (aubufv_t);
	static ALIGNX_BEGIN uint32_t descr0 [3] [DMAC_DESC_SIZE] ALIGNX_END;
	const unsigned dmach = DMAC_I2S1_RX_Ch;
	const unsigned sdwt = dmac_desc_datawidth(dw * 8);		// DMA Source Data Width
	const unsigned ddwt = dmac_desc_datawidth(dw * 8);	// DMA Destination Data Width
	const unsigned NBYTES = DMABUFFSIZE16RX * dw;
	const uintptr_t portaddr = (uintptr_t) & I2S1->I2S_PCM_RXFIFO;

	const uint_fast32_t parameterDMAC = 0;
	const uint_fast32_t configDMAC =
		0 * (1u << 30) |	// BMODE_SEL
		ddwt * (1u << 25) |	// DMA Destination Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		0 * (1u << 24) |	// DMA Destination Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 22) |	// DMA Destination Block Size
		DMAC_DstReqDRAM * (1u << 16) |	// DMA Destination DRQ Type
		sdwt * (1u << 9) |	// DMA Source Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		1 * (1u << 8) |	// DMA Source Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 6) |	// DMA Source Block Size
		DMAC_SrcReqI2S1_RX * (1u << 0) |	// DMA Source DRQ Type
		0;

	// Six words of DMAC sescriptor: (Link=0xFFFFF800 for last)
	descr0 [0] [0] = configDMAC;			// Cofigurarion
	descr0 [0] [1] = portaddr;				// Source Address
	descr0 [0] [2] = dma_invalidate16rx(allocate_dmabuffer16rx());				// Destination Address
	descr0 [0] [3] = NBYTES;				// Byte Counter
	descr0 [0] [4] = parameterDMAC;			// Parameter
	descr0 [0] [5] = (uintptr_t) descr0 [1];	// Link to next

	descr0 [1] [0] = configDMAC;			// Cofigurarion
	descr0 [1] [1] = portaddr;				// Source Address
	descr0 [1] [2] = dma_invalidate16rx(allocate_dmabuffer16rx());				// Destination Address
	descr0 [1] [3] = NBYTES;				// Byte Counter
	descr0 [1] [4] = parameterDMAC;				// Parameter
	descr0 [1] [5] = (uintptr_t) descr0 [2];	// Link to previous

	descr0 [2] [0] = configDMAC;			// Cofigurarion
	descr0 [2] [1] = portaddr;				// Source Address
	descr0 [2] [2] = dma_invalidate16rx(allocate_dmabuffer16rx());				// Destination Address
	descr0 [2] [3] = NBYTES;				// Byte Counter
	descr0 [2] [4] = parameterDMAC;				// Parameter
	descr0 [2] [5] = (uintptr_t) descr0 [0];	// Link to previous

	uintptr_t descraddr = (uintptr_t) descr0;
	dcache_clean(descraddr, sizeof descr0);

	DMAC_clock_initialize();

	DMAC->CH [dmach].DMAC_EN_REGN = 0;	// 0: Disabled

	DMAC->CH [dmach].DMAC_DESC_ADDR_REGN = descraddr;
	while (DMAC->CH [dmach].DMAC_DESC_ADDR_REGN != descraddr)
		;

	// 0x04: Queue, 0x02: Pkq, 0x01: half
	DMAC_SetHandler(dmach, DMAC_IRQ_EN_FLAG_VALUE, DMA_I2Sx_AudioCodec_RX_Handler_codec1);

	DMAC->CH [dmach].DMAC_PAU_REGN = 0;	// 0: Resume Transferring
	DMAC->CH [dmach].DMAC_EN_REGN = 1;	// 1: Enabled
}

static void DMAC_I2S1_TX_initialize_codec1(void)
{
	const size_t dw = sizeof (aubufv_t);
	static ALIGNX_BEGIN uint32_t descr0 [3] [DMAC_DESC_SIZE] ALIGNX_END;
	const unsigned dmach = DMAC_I2S1_TX_Ch;
	const unsigned sdwt = dmac_desc_datawidth(dw * 8);	// DMA Source Data Width
	const unsigned ddwt = dmac_desc_datawidth(dw * 8);		// DMA Destination Data Width
	const unsigned NBYTES = DMABUFFSIZE16TX * dw;
	const uintptr_t portaddr = (uintptr_t) & I2S1->I2S_PCM_TXFIFO;

	const uint_fast32_t parameterDMAC = 0;
	const uint_fast32_t configDMAC =
		0 * (1u << 30) |	// BMODE_SEL
		ddwt * (1u << 25) |	// DMA Destination Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		1 * (1u << 24) |	// DMA Destination Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 22) |	// DMA Destination Block Size
		DMAC_DstReqI2S1_TX * (1u << 16) |	// DMA Destination DRQ Type
		sdwt * (1u << 9) |	// DMA Source Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		0 * (1u << 8) |	// DMA Source Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 6) |	// DMA Source Block Size
		DMAC_SrcReqDRAM * (1u << 0) |	// DMA Source DRQ Type
		0;

	// Six words of DMAC sescriptor: (Link=0xFFFFF800 for last)
	descr0 [0] [0] = configDMAC;			// Cofigurarion
	descr0 [0] [1] = dma_flush16tx(getfilled_dmabuffer16txphones());			// Source Address
	descr0 [0] [2] = portaddr;				// Destination Address
	descr0 [0] [3] = NBYTES;				// Byte Counter
	descr0 [0] [4] = parameterDMAC;			// Parameter
	descr0 [0] [5] = (uintptr_t) descr0 [1];	// Link to next

	descr0 [1] [0] = configDMAC;			// Cofigurarion
	descr0 [1] [1] = dma_flush16tx(getfilled_dmabuffer16txphones());			// Source Address
	descr0 [1] [2] = portaddr;				// Destination Address
	descr0 [1] [3] = NBYTES;				// Byte Counter
	descr0 [1] [4] = parameterDMAC;			// Parameter
	descr0 [1] [5] = (uintptr_t) descr0 [2];	// Link to previous

	descr0 [2] [0] = configDMAC;			// Cofigurarion
	descr0 [2] [1] = dma_flush16tx(getfilled_dmabuffer16txphones());			// Source Address
	descr0 [2] [2] = portaddr;				// Destination Address
	descr0 [2] [3] = NBYTES;				// Byte Counter
	descr0 [2] [4] = parameterDMAC;			// Parameter
	descr0 [2] [5] = (uintptr_t) descr0 [0];	// Link to previous

	uintptr_t descraddr = (uintptr_t) descr0;
	dcache_clean(descraddr, sizeof descr0);

	DMAC_clock_initialize();

	DMAC->CH [dmach].DMAC_EN_REGN = 0;	// 0: Disabled

	DMAC->CH [dmach].DMAC_DESC_ADDR_REGN = descraddr;
	while (DMAC->CH [dmach].DMAC_DESC_ADDR_REGN != descraddr)
		;

	// 0x04: Queue, 0x02: Pkq, 0x01: half
	DMAC_SetHandler(dmach, DMAC_IRQ_EN_FLAG_VALUE, DMA_I2Sx_AudioCodec_TX_Handler_codec1);

	DMAC->CH [dmach].DMAC_PAU_REGN = 0;	// 0: Resume Transferring
	DMAC->CH [dmach].DMAC_EN_REGN = 1;	// 1: Enabled
}

static void DMAC_I2S2_TX_initialize_codec1(void)
{
	const size_t dw = sizeof (aubufv_t);
	static ALIGNX_BEGIN uint32_t descr0 [3] [DMAC_DESC_SIZE] ALIGNX_END;
	const unsigned dmach = DMAC_I2S2_TX_Ch;
	const unsigned sdwt = dmac_desc_datawidth(dw * 8);	// DMA Source Data Width
	const unsigned ddwt = dmac_desc_datawidth(dw * 8);		// DMA Destination Data Width
	const unsigned NBYTES = DMABUFFSIZE16TX * dw;
	const uintptr_t portaddr = (uintptr_t) & I2S2->I2S_PCM_TXFIFO;

	const uint_fast32_t parameterDMAC = 0;
	const uint_fast32_t configDMAC =
		0 * (1u << 30) |	// BMODE_SEL
		ddwt * (1u << 25) |	// DMA Destination Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		1 * (1u << 24) |	// DMA Destination Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 22) |	// DMA Destination Block Size
		DMAC_DstReqI2S2_TX * (1u << 16) |	// DMA Destination DRQ Type
		sdwt * (1u << 9) |	// DMA Source Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		0 * (1u << 8) |	// DMA Source Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 6) |	// DMA Source Block Size
		DMAC_SrcReqDRAM * (1u << 0) |	// DMA Source DRQ Type
		0;

	// Six words of DMAC sescriptor: (Link=0xFFFFF800 for last)
	descr0 [0] [0] = configDMAC;			// Cofigurarion
	descr0 [0] [1] = dma_flush16tx(getfilled_dmabuffer16txphones());			// Source Address
	descr0 [0] [2] = portaddr;				// Destination Address
	descr0 [0] [3] = NBYTES;				// Byte Counter
	descr0 [0] [4] = parameterDMAC;			// Parameter
	descr0 [0] [5] = (uintptr_t) descr0 [1];	// Link to next

	descr0 [1] [0] = configDMAC;			// Cofigurarion
	descr0 [1] [1] = dma_flush16tx(getfilled_dmabuffer16txphones());			// Source Address
	descr0 [1] [2] = portaddr;				// Destination Address
	descr0 [1] [3] = NBYTES;				// Byte Counter
	descr0 [1] [4] = parameterDMAC;			// Parameter
	descr0 [1] [5] = (uintptr_t) descr0 [2];	// Link to previous

	descr0 [2] [0] = configDMAC;			// Cofigurarion
	descr0 [2] [1] = dma_flush16tx(getfilled_dmabuffer16txphones());			// Source Address
	descr0 [2] [2] = portaddr;				// Destination Address
	descr0 [2] [3] = NBYTES;				// Byte Counter
	descr0 [2] [4] = parameterDMAC;			// Parameter
	descr0 [2] [5] = (uintptr_t) descr0 [0];	// Link to previous

	uintptr_t descraddr = (uintptr_t) descr0;
	dcache_clean(descraddr, sizeof descr0);

	DMAC_clock_initialize();

	DMAC->CH [dmach].DMAC_EN_REGN = 0;	// 0: Disabled

	DMAC->CH [dmach].DMAC_DESC_ADDR_REGN = descraddr;
	while (DMAC->CH [dmach].DMAC_DESC_ADDR_REGN != descraddr)
		;

	// 0x04: Queue, 0x02: Pkq, 0x01: half
	DMAC_SetHandler(dmach, DMAC_IRQ_EN_FLAG_VALUE, DMA_I2Sx_AudioCodec_TX_Handler_codec1);

	DMAC->CH [dmach].DMAC_PAU_REGN = 0;	// 0: Resume Transferring
	DMAC->CH [dmach].DMAC_EN_REGN = 1;	// 1: Enabled
}

static void DMAC_I2S1_RX_initialize_fpga(void)
{
	const size_t dw = sizeof (IFADCvalue_t);
	static ALIGNX_BEGIN uint32_t descr0 [3] [DMAC_DESC_SIZE] ALIGNX_END;
	const unsigned dmach = DMAC_I2S1_RX_Ch;
	const unsigned sdwt = dmac_desc_datawidth(dw * 8);		// DMA Source Data Width
	const unsigned ddwt = dmac_desc_datawidth(dw * 8);	// DMA Destination Data Width
	const unsigned NBYTES = DMABUFFSIZE32RX * dw;
	const uintptr_t portaddr = (uintptr_t) & I2S1->I2S_PCM_RXFIFO;

	const uint_fast32_t parameterDMAC = 0;
	const uint_fast32_t configDMAC =
		0 * (1u << 30) |	// BMODE_SEL
		ddwt * (1u << 25) |	// DMA Destination Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		0 * (1u << 24) |	// DMA Destination Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 22) |	// DMA Destination Block Size
		DMAC_DstReqDRAM * (1u << 16) |	// DMA Destination DRQ Type
		sdwt * (1u << 9) |	// DMA Source Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		1 * (1u << 8) |	// DMA Source Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 6) |	// DMA Source Block Size
		DMAC_SrcReqI2S1_RX * (1u << 0) |	// DMA Source DRQ Type
		0;

	// Six words of DMAC sescriptor: (Link=0xFFFFF800 for last)
	descr0 [0] [0] = configDMAC;			// Cofigurarion
	descr0 [0] [1] = portaddr;				// Source Address
	descr0 [0] [2] = dma_invalidate32rx(allocate_dmabuffer32rx());		// Destination Address
	descr0 [0] [3] = NBYTES;				// Byte Counter
	descr0 [0] [4] = parameterDMAC;			// Parameter
	descr0 [0] [5] = (uintptr_t) descr0 [1];	// Link to next

	descr0 [1] [0] = configDMAC;			// Cofigurarion
	descr0 [1] [1] = portaddr;				// Source Address
	descr0 [1] [2] = dma_invalidate32rx(allocate_dmabuffer32rx());		// Destination Address
	descr0 [1] [3] = NBYTES;				// Byte Counter
	descr0 [1] [4] = parameterDMAC;			// Parameter
	descr0 [1] [5] = (uintptr_t) descr0 [2];	// Link to previous

	descr0 [2] [0] = configDMAC;			// Cofigurarion
	descr0 [2] [1] = portaddr;				// Source Address
	descr0 [2] [2] = dma_invalidate32rx(allocate_dmabuffer32rx());		// Destination Address
	descr0 [2] [3] = NBYTES;				// Byte Counter
	descr0 [2] [4] = parameterDMAC;			// Parameter
	descr0 [2] [5] = (uintptr_t) descr0 [0];	// Link to previous

	uintptr_t descraddr = (uintptr_t) descr0;
	dcache_clean(descraddr, sizeof descr0);

	DMAC_clock_initialize();

	DMAC->CH [dmach].DMAC_EN_REGN = 0;	// 0: Disabled

	DMAC->CH [dmach].DMAC_DESC_ADDR_REGN = descraddr;
	while (DMAC->CH [dmach].DMAC_DESC_ADDR_REGN != descraddr)
		;

	// 0x04: Queue, 0x02: Pkq, 0x01: half
	DMAC_SetHandler(dmach, DMAC_IRQ_EN_FLAG_VALUE, DMA_I2Sx_RX_Handler_fpga);

	DMAC->CH [dmach].DMAC_PAU_REGN = 0;	// 0: Resume Transferring
	DMAC->CH [dmach].DMAC_EN_REGN = 1;	// 1: Enabled
}

#if defined (I2S2)

// Allwinner A64 not support RX on I2S2
#if ! defined (CPUSTYLE_A64)

static void DMAC_I2S2_RX_initialize_codec1(void)
{
	const size_t dw = sizeof (aubufv_t);
	static ALIGNX_BEGIN uint32_t descr0 [3] [DMAC_DESC_SIZE] ALIGNX_END;
	const unsigned dmach = DMAC_I2S2_RX_Ch;
	const unsigned sdwt = dmac_desc_datawidth(dw * 8);		// DMA Source Data Width
	const unsigned ddwt = dmac_desc_datawidth(dw * 8);	// DMA Destination Data Width
	const unsigned NBYTES = DMABUFFSIZE16RX * dw;
	const uintptr_t portaddr = (uintptr_t) & I2S2->I2S_PCM_RXFIFO;

	const uint_fast32_t parameterDMAC = 0;
	const uint_fast32_t configDMAC =
		0 * (1u << 30) |	// BMODE_SEL
		ddwt * (1u << 25) |	// DMA Destination Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		0 * (1u << 24) |	// DMA Destination Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 22) |	// DMA Destination Block Size
		DMAC_DstReqDRAM * (1u << 16) |	// DMA Destination DRQ Type
		sdwt * (1u << 9) |	// DMA Source Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		1 * (1u << 8) |	// DMA Source Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 6) |	// DMA Source Block Size
		DMAC_SrcReqI2S2_RX * (1u << 0) |	// DMA Source DRQ Type
		0;

	// Six words of DMAC sescriptor: (Link=0xFFFFF800 for last)
	descr0 [0] [0] = configDMAC;			// Cofigurarion
	descr0 [0] [1] = portaddr;				// Source Address
	descr0 [0] [2] = dma_invalidate16rx(allocate_dmabuffer16rx());				// Destination Address
	descr0 [0] [3] = NBYTES;				// Byte Counter
	descr0 [0] [4] = parameterDMAC;			// Parameter
	descr0 [0] [5] = (uintptr_t) descr0 [1];	// Link to next

	descr0 [1] [0] = configDMAC;			// Cofigurarion
	descr0 [1] [1] = portaddr;				// Source Address
	descr0 [1] [2] = dma_invalidate16rx(allocate_dmabuffer16rx());				// Destination Address
	descr0 [1] [3] = NBYTES;				// Byte Counter
	descr0 [1] [4] = parameterDMAC;				// Parameter
	descr0 [1] [5] = (uintptr_t) descr0 [2];	// Link to previous

	descr0 [2] [0] = configDMAC;			// Cofigurarion
	descr0 [2] [1] = portaddr;				// Source Address
	descr0 [2] [2] = dma_invalidate16rx(allocate_dmabuffer16rx());				// Destination Address
	descr0 [2] [3] = NBYTES;				// Byte Counter
	descr0 [2] [4] = parameterDMAC;				// Parameter
	descr0 [2] [5] = (uintptr_t) descr0 [0];	// Link to previous

	uintptr_t descraddr = (uintptr_t) descr0;
	dcache_clean(descraddr, sizeof descr0);

	DMAC_clock_initialize();

	DMAC->CH [dmach].DMAC_EN_REGN = 0;	// 0: Disabled

	DMAC->CH [dmach].DMAC_DESC_ADDR_REGN = descraddr;
	while (DMAC->CH [dmach].DMAC_DESC_ADDR_REGN != descraddr)
		;

	// 0x04: Queue, 0x02: Pkq, 0x01: half
	DMAC_SetHandler(dmach, DMAC_IRQ_EN_FLAG_VALUE, DMA_I2Sx_AudioCodec_RX_Handler_codec1);

	DMAC->CH [dmach].DMAC_PAU_REGN = 0;	// 0: Resume Transferring
	DMAC->CH [dmach].DMAC_EN_REGN = 1;	// 1: Enabled
}

static void DMAC_I2S2_RX_initialize_fpga(void)
{
	const size_t dw = sizeof (IFADCvalue_t);
	static ALIGNX_BEGIN uint32_t descr0 [3] [DMAC_DESC_SIZE] ALIGNX_END;
	const unsigned dmach = DMAC_I2S2_RX_Ch;
	const unsigned sdwt = dmac_desc_datawidth(dw * 8);		// DMA Source Data Width
	const unsigned ddwt = dmac_desc_datawidth(dw * 8);	// DMA Destination Data Width
	const unsigned NBYTES = DMABUFFSIZE32RX * dw;
	const uintptr_t portaddr = (uintptr_t) & I2S2->I2S_PCM_RXFIFO;

	const uint_fast32_t parameterDMAC = 0;
	const uint_fast32_t configDMAC =
		0 * (1u << 30) |	// BMODE_SEL
		ddwt * (1u << 25) |	// DMA Destination Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		0 * (1u << 24) |	// DMA Destination Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 22) |	// DMA Destination Block Size
		DMAC_DstReqDRAM * (1u << 16) |	// DMA Destination DRQ Type
		sdwt * (1u << 9) |	// DMA Source Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		1 * (1u << 8) |	// DMA Source Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 6) |	// DMA Source Block Size
		DMAC_SrcReqI2S2_RX * (1u << 0) |	// DMA Source DRQ Type
		0;

	// Six words of DMAC sescriptor: (Link=0xFFFFF800 for last)
	descr0 [0] [0] = configDMAC;			// Cofigurarion
	descr0 [0] [1] = portaddr;				// Source Address
	descr0 [0] [2] = dma_invalidate32rx(allocate_dmabuffer32rx());		// Destination Address
	descr0 [0] [3] = NBYTES;				// Byte Counter
	descr0 [0] [4] = parameterDMAC;			// Parameter
	descr0 [0] [5] = (uintptr_t) descr0 [1];	// Link to next

	descr0 [1] [0] = configDMAC;			// Cofigurarion
	descr0 [1] [1] = portaddr;				// Source Address
	descr0 [1] [2] = dma_invalidate32rx(allocate_dmabuffer32rx());		// Destination Address
	descr0 [1] [3] = NBYTES;				// Byte Counter
	descr0 [1] [4] = parameterDMAC;			// Parameter
	descr0 [1] [5] = (uintptr_t) descr0 [2];	// Link to previous

	descr0 [2] [0] = configDMAC;			// Cofigurarion
	descr0 [2] [1] = portaddr;				// Source Address
	descr0 [2] [2] = dma_invalidate32rx(allocate_dmabuffer32rx());		// Destination Address
	descr0 [2] [3] = NBYTES;				// Byte Counter
	descr0 [2] [4] = parameterDMAC;			// Parameter
	descr0 [2] [5] = (uintptr_t) descr0 [0];	// Link to previous

	uintptr_t descraddr = (uintptr_t) descr0;
	dcache_clean(descraddr, sizeof descr0);

	DMAC_clock_initialize();

	DMAC->CH [dmach].DMAC_EN_REGN = 0;	// 0: Disabled

	DMAC->CH [dmach].DMAC_DESC_ADDR_REGN = descraddr;
	while (DMAC->CH [dmach].DMAC_DESC_ADDR_REGN != descraddr)
		;

	// 0x04: Queue, 0x02: Pkq, 0x01: half
	DMAC_SetHandler(dmach, DMAC_IRQ_EN_FLAG_VALUE, DMA_I2Sx_RX_Handler_fpga);

	DMAC->CH [dmach].DMAC_PAU_REGN = 0;	// 0: Resume Transferring
	DMAC->CH [dmach].DMAC_EN_REGN = 1;	// 1: Enabled
}

#endif /* ! defined (CPUSTYLE_A64) */

#endif /* defined (I2S2) */

static void DMAC_I2S1_TX_initialize_fpga(void)
{
	const size_t dw = sizeof (IFDACvalue_t);
	static ALIGNX_BEGIN uint32_t descr0 [3] [DMAC_DESC_SIZE] ALIGNX_END;
	const unsigned dmach = DMAC_I2S1_TX_Ch;
	const unsigned sdwt = dmac_desc_datawidth(dw * 8);	// DMA Source Data Width
	const unsigned ddwt = dmac_desc_datawidth(dw * 8);		// DMA Destination Data Width
	const unsigned NBYTES = DMABUFFSIZE32TX * dw;
	const uintptr_t portaddr = (uintptr_t) & I2S1->I2S_PCM_TXFIFO;

	const uint_fast32_t parameterDMAC = 0;
	const uint_fast32_t configDMAC =
		0 * (1u << 30) |	// BMODE_SEL
		ddwt * (1u << 25) |	// DMA Destination Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		1 * (1u << 24) |	// DMA Destination Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 22) |	// DMA Destination Block Size
		DMAC_DstReqI2S1_TX * (1u << 16) |	// DMA Destination DRQ Type
		sdwt * (1u << 9) |	// DMA Source Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		0 * (1u << 8) |	// DMA Source Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 6) |	// DMA Source Block Size
		DMAC_SrcReqDRAM * (1u << 0) |	// DMA Source DRQ Type
		0;

	// Six words of DMAC sescriptor: (Link=0xFFFFF800 for last)
	descr0 [0] [0] = configDMAC;			// Cofigurarion
	descr0 [0] [1] = dma_flush32tx(allocate_dmabuffer32tx());				// Source Address
	descr0 [0] [2] = portaddr;				// Destination Address
	descr0 [0] [3] = NBYTES;				// Byte Counter
	descr0 [0] [4] = parameterDMAC;			// Parameter
	descr0 [0] [5] = (uintptr_t) descr0 [1];	// Link to next

	descr0 [1] [0] = configDMAC;			// Cofigurarion
	descr0 [1] [1] = dma_flush32tx(allocate_dmabuffer32tx());				// Source Address
	descr0 [1] [2] = portaddr;				// Destination Address
	descr0 [1] [3] = NBYTES;				// Byte Counter
	descr0 [1] [4] = parameterDMAC;			// Parameter
	descr0 [1] [5] = (uintptr_t) descr0 [2];	// Link to previous

	descr0 [2] [0] = configDMAC;			// Cofigurarion
	descr0 [2] [1] = dma_flush32tx(allocate_dmabuffer32tx());				// Source Address
	descr0 [2] [2] = portaddr;				// Destination Address
	descr0 [2] [3] = NBYTES;				// Byte Counter
	descr0 [2] [4] = parameterDMAC;			// Parameter
	descr0 [2] [5] = (uintptr_t) descr0 [0];	// Link to previous

	uintptr_t descraddr = (uintptr_t) descr0;
	dcache_clean(descraddr, sizeof descr0);

	DMAC_clock_initialize();

	DMAC->CH [dmach].DMAC_EN_REGN = 0;	// 0: Disabled

	DMAC->CH [dmach].DMAC_DESC_ADDR_REGN = descraddr;
	while (DMAC->CH [dmach].DMAC_DESC_ADDR_REGN != descraddr)
		;

	// 0x04: Queue, 0x02: Pkq, 0x01: half
	DMAC_SetHandler(dmach, DMAC_IRQ_EN_FLAG_VALUE, DMA_I2Sx_TX_Handler_fpga);

	DMAC->CH [dmach].DMAC_PAU_REGN = 0;	// 0: Resume Transferring
	DMAC->CH [dmach].DMAC_EN_REGN = 1;	// 1: Enabled
}

static void DMAC_I2S2_TX_initialize_fpga(void)
{
	const size_t dw = sizeof (IFDACvalue_t);
	static ALIGNX_BEGIN uint32_t descr0 [3] [DMAC_DESC_SIZE] ALIGNX_END;
	const unsigned dmach = DMAC_I2S2_TX_Ch;
	const unsigned sdwt = dmac_desc_datawidth(dw * 8);	// DMA Source Data Width
	const unsigned ddwt = dmac_desc_datawidth(dw * 8);		// DMA Destination Data Width
	const unsigned NBYTES = DMABUFFSIZE32TX * dw;
	const uintptr_t portaddr = (uintptr_t) & I2S2->I2S_PCM_TXFIFO;

	const uint_fast32_t parameterDMAC = 0;
	const uint_fast32_t configDMAC =
		0 * (1u << 30) |	// BMODE_SEL
		ddwt * (1u << 25) |	// DMA Destination Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		1 * (1u << 24) |	// DMA Destination Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 22) |	// DMA Destination Block Size
		DMAC_DstReqI2S2_TX * (1u << 16) |	// DMA Destination DRQ Type
		sdwt * (1u << 9) |	// DMA Source Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		0 * (1u << 8) |	// DMA Source Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 6) |	// DMA Source Block Size
		DMAC_SrcReqDRAM * (1u << 0) |	// DMA Source DRQ Type
		0;

	// Six words of DMAC sescriptor: (Link=0xFFFFF800 for last)
	descr0 [0] [0] = configDMAC;			// Cofigurarion
	descr0 [0] [1] = dma_flush32tx(allocate_dmabuffer32tx());				// Source Address
	descr0 [0] [2] = portaddr;				// Destination Address
	descr0 [0] [3] = NBYTES;				// Byte Counter
	descr0 [0] [4] = parameterDMAC;			// Parameter
	descr0 [0] [5] = (uintptr_t) descr0 [1];	// Link to next

	descr0 [1] [0] = configDMAC;			// Cofigurarion
	descr0 [1] [1] = dma_flush32tx(allocate_dmabuffer32tx());				// Source Address
	descr0 [1] [2] = portaddr;				// Destination Address
	descr0 [1] [3] = NBYTES;				// Byte Counter
	descr0 [1] [4] = parameterDMAC;			// Parameter
	descr0 [1] [5] = (uintptr_t) descr0 [2];	// Link to previous

	descr0 [2] [0] = configDMAC;			// Cofigurarion
	descr0 [2] [1] = dma_flush32tx(allocate_dmabuffer32tx());				// Source Address
	descr0 [2] [2] = portaddr;				// Destination Address
	descr0 [2] [3] = NBYTES;				// Byte Counter
	descr0 [2] [4] = parameterDMAC;			// Parameter
	descr0 [2] [5] = (uintptr_t) descr0 [0];	// Link to previous

	uintptr_t descraddr = (uintptr_t) descr0;
	dcache_clean(descraddr, sizeof descr0);

	DMAC_clock_initialize();

	DMAC->CH [dmach].DMAC_EN_REGN = 0;	// 0: Disabled

	DMAC->CH [dmach].DMAC_DESC_ADDR_REGN = descraddr;
	while (DMAC->CH [dmach].DMAC_DESC_ADDR_REGN != descraddr)
		;

	// 0x04: Queue, 0x02: Pkq, 0x01: half
	DMAC_SetHandler(dmach, DMAC_IRQ_EN_FLAG_VALUE, DMA_I2Sx_TX_Handler_fpga);

	DMAC->CH [dmach].DMAC_PAU_REGN = 0;	// 0: Resume Transferring
	DMAC->CH [dmach].DMAC_EN_REGN = 1;	// 1: Enabled
}

static const codechw_t audiocodechw_i2s1_duplex_master =
{
	hardware_i2s1_master_duplex_initialize_codec1,
	hardware_dummy_initialize,
	DMAC_I2S1_RX_initialize_codec1,
	DMAC_I2S1_TX_initialize_codec1,
	hardware_i2s1_enable_codec1,
	hardware_dummy_enable,
	"audiocodechw-i2s1-duplex-master"
};


#if WITHCODEC1_WHBLOCK_DUPLEX_MASTER

/* встороенный в процессор кодек */
static void hardware_hwblock_master_duplex_initialize_codec1(void)
{
	const unsigned framebits = CODEC1_FRAMEBITS;
	const unsigned lrckf = ARMI2SRATE;

	const unsigned mclkf = lrckf * 512;

#if CPUSTYLE_A64
	#warning Implement for CPUSTYLE_A64

#else

	// 0x00 = 48000 (x 512)
	// 0x01 = ~ 46902 (x 512)
	// 0x02 = 48000 (x 512)
	const unsigned long src = 0x00;
	//	Clock Source Select
	//	00: PLL_AUDIO0(1X)
	//	01: PLL_AUDIO1(DIV2)
	//	10: PLL_AUDIO1(DIV5)
	unsigned long clk;
	switch (src)
	{
	default:
	case 0x00:
		clk = allwnrt113_get_audio0pll1x_freq();
		break;
	case 0x01:
		clk = allwnrt113_get_audio1pll_div2_freq();
		break;
	case 0x02:
		clk = allwnrt113_get_audio1pll_div5_freq();
		break;
	}
	//TP();
	unsigned value;	/* делитель */
	const uint_fast8_t prei = calcdivider(calcdivround2(clk, mclkf), ALLWNT113_AudioCodec_CLK_WIDTH, ALLWNT113_AudioCodec_CLK_TAPS, & value, 1);

	//PRINTF("AudioCodec: prei=%u, value=%u, lrckf=%u, (clk=%lu)\n", prei, value, mclkf, clk);

	// audiocode1x_dac_clk
	// audiocode1x_adc_clk
	//	Clock Source Select
	//	00: PLL_AUDIO0(1X)
	//	01: PLL_AUDIO1(DIV2)
	//	10: PLL_AUDIO1(DIV5)

	const portholder_t clk_reg =
		(1u << 31) |				// AUDIO_CODEC_ADC_CLK_GATING
		((uint_fast32_t) src << 24) |	// CLK_SRC_SEL
		((uint_fast32_t) prei << 8) |	// Factor N (0..3: /1 /2 /4 /8)
		((uint_fast32_t) value << 0) |	// Factor M (0..31)
		0;

	CCU->AUDIO_CODEC_ADC_CLK_REG = clk_reg;
	CCU->AUDIO_CODEC_DAC_CLK_REG = clk_reg;

	CCU->AUDIO_CODEC_BGR_REG |= (1u << 0);	// Gating Clock For AUDIO_CODEC
	CCU->AUDIO_CODEC_BGR_REG |= (1u << 16);	// AUDIO_CODEC Reset
#endif

#if 1
	// anatol

	///AUDIO_CODEC->ADC_DIG_CTRL = (AUDIO_CODEC->ADC_DIG_CTRL & ~ (0x07uL)) |(0x04 | 0x01) << (1u << 0) |	0;// ADC_CHANNEL_EN Bit 2: ADC3 enabled Bit 1: ADC2 enabled Bit 0: ADC1 enabled
//	AUDIO_CODEC->ADC_DIG_CTRL & ~ (1u << 17)|(1u << 16)|(15 << 0);
//	AUDIO_CODEC->ADC_DIG_CTRL |= (1u << 17)|(1u << 16)|(1 << 0)|(1 << 1)/*|(1 << 2)*/;///LINL,LINR IN

	//AUDIO_CODEC->ADC_DIG_CTRL & ~ (1u << 17)|(1u << 16)|(15 << 0);
	AUDIO_CODEC->ADC_DIG_CTRL = (AUDIO_CODEC->ADC_DIG_CTRL & ~ (0x0Fu)) |
			(1u << 17) |	// ADC3_VOL_EN ADC3 Volume Control Enable
			(1u << 16) |	// ADC1_2_VOL_EN ADC1/2 Volume Control Enable
			((0x04 | 0x02 | 0x01) << 0) |	// ADC_CHANNEL_EN Bit 2: ADC3 enabled Bit 1: ADC2 enabled Bit 0: ADC1 enabled
			0;
	ASSERT(DMABUFFSTEP16RX == 3);

	// See WITHADAPTERCODEC1WIDTH and WITHADAPTERCODEC1SHIFT
	AUDIO_CODEC->AC_ADC_FIFOC |= (1u << 16);	// RX_SAMPLE_BITS 1: 20 bits 0: 16 bits
	AUDIO_CODEC->AC_ADC_FIFOC &= ~ (1u << 24);	// RX_FIFO_MODE 0: Expanding ‘0’ at LSB of TX FIFO register
	AUDIO_CODEC->AC_ADC_FIFOC |= (1u << 3);	// ADC_DRQ_EN

	AUDIO_CODEC->AC_DAC_FIFOC |= (1u << 5);	// TX_SAMPLE_BITS 1: 20 bits 0: 16 bits
	AUDIO_CODEC->AC_DAC_FIFOC &= ~ (3u << 24);	// FIFO_MODE 00/10: FIFO_I[19:0] = {TXDATA[31:12]
	AUDIO_CODEC->AC_DAC_FIFOC |= (1u << 4);	// DAC_DRQ_EN


//	AUDIO_CODEC->ADC1_REG |= (1u << 23);  // LINEINL
//	AUDIO_CODEC->ADC2_REG |= (1u << 23);  // LINEINR
//	AUDIO_CODEC->ADC3_REG |= (1u << 30);	// MIC3_PGA_EN

    ///-----LDO-----
	//PRINTF("AUDIO_CODEC->POWER_REG=%08X\n", (unsigned) AUDIO_CODEC->POWER_REG);
//	AUDIO_CODEC->POWER_REG |= (1u << 31);	// ALDO_EN
//	AUDIO_CODEC->POWER_REG |= (1u << 30);	// HPLDO_EN
	//PRINTF("AUDIO_CODEC->POWER_REG=%08X\n", (unsigned) AUDIO_CODEC->POWER_REG);
    ///-------------


	// DAC Analog Control Register
	AUDIO_CODEC->DAC_REG |= (1u << 15) | (1u << 14);	// DACL_EN, DACR_EN


	AUDIO_CODEC->RAMP_REG |=
	   (1u << 15) | // HP_PULL_OUT_EN Heanphone Pullout Enable
	   (1u << 0) | // RD_EN Ramp Digital Enable
	   0;

#if 0
	/* LINEIN use */

	// ADCx Analog Control Register
	//AUDIO_CODEC->ADC1_REG |= (1u << 31);	// LEft ADC1 Channel Enable
	//AUDIO_CODEC->ADC2_REG |= (1u << 31);	// Right ADC1 Channel Enable
	// Left audio
	AUDIO_CODEC->ADC1_REG &= ~ (1u << 27);	// FMINLEN FMINL Disable - R11 - fminL pin 94
	AUDIO_CODEC->ADC1_REG |= (1u << 23);	// LINEINLEN LINEINL Enable

	// ADCx Analog Control Register
	// Right audio
	AUDIO_CODEC->ADC2_REG &= ~ (1u << 27);	// FMINREN FMINR Disable - R10 - fminR pin 93
	AUDIO_CODEC->ADC2_REG |= (1u << 23);	// LINEINREN LINEINR Enable - R6 - lineinR - pin 95

#elif 0
	/* FMIN use */

	// ADCx Analog Control Register
	//AUDIO_CODEC->ADC1_REG |= (1u << 31);	// LEft ADC1 Channel Enable
	//AUDIO_CODEC->ADC2_REG |= (1u << 31);	// Right ADC1 Channel Enable
	// Left audio
	AUDIO_CODEC->ADC1_REG |= (1u << 27);	// FMINLEN FMINL Enable - R11 - fminL pin 94
	AUDIO_CODEC->ADC1_REG &= ~ (1u << 23);	// LINEINLEN LINEINL Disable

	// Right audio
	AUDIO_CODEC->ADC2_REG |= (1u << 27);	// FMINREN FMINR Enable - R10 - fminR pin 93
	AUDIO_CODEC->ADC2_REG &= ~ (1u << 23);	// LINEINREN LINEINR Disable - R6 - lineinR - pin 95

#else

	/* Do not use FM/LINE inputs */

	// ADCx Analog Control Register
	AUDIO_CODEC->ADC1_REG &= ~ (1u << 31);	// LEft ADC1 Channel Disable
	AUDIO_CODEC->ADC2_REG &= ~ (1u << 31);	// Right ADC1 Channel Disable

#endif

	// ADC3 Analog Control Register
	AUDIO_CODEC->ADC3_REG |= (1u << 31);	// MIC3
	// MIC3
	AUDIO_CODEC->ADC3_REG |= (1u << 30);	// MIC3_PGA_EN
	//AUDIO_CODEC->ADC3_REG |= (1u << 28);	// MIC3_SIN_EN MIC3 Single Input Enable
	AUDIO_CODEC->ADC3_REG = (AUDIO_CODEC->ADC3_REG & ~ (0x0Fu << 8)) | (0x0Fu << 8);	// ADC3_PGA_GAIN_CTRL: 36 dB

	// Установка усиления
	AUDIO_CODEC->ADC_VOL_CTRL1 =
		0 * (0xA0u << 0) |	/* ADC1_VOL 0xA0 - middle point */
		0 * (0xA0u << 8) |	/* ADC2_VOL 0xA0 - middle point */
		(0xA0u << 16) |	/* ADC3_VOL 0xA0 - middle point */
		0;
#else

	//PRINTF("AC_ADC_FIFOC=%08lX, AC_DAC_FIFOC=%08lX\n", AUDIO_CODEC->AC_ADC_FIFOC, AUDIO_CODEC->AC_DAC_FIFOC);

	// количество каналов должно соотыетствовать DMABUFFSTEP16RX
	AUDIO_CODEC->ADC_DIG_CTRL = (AUDIO_CODEC->ADC_DIG_CTRL & ~ (0x07uL)) |
			(1u << 17) |	// ADC3_VOL_EN ADC3 Volume Control Enable
			(1u << 16) |	// ADC1_2_VOL_EN ADC1/2 Volume Control Enable
			((0x04 | 0x02) << 0) |	// ADC_CHANNEL_EN Bit 2: ADC3 enabled Bit 1: ADC2 enabled Bit 0: ADC1 enabled
			0;

	// ADCx Analog Control Register
	AUDIO_CODEC->ADC1_REG |= (1u << 31);	// FMINL ADC1 Channel Enable
	AUDIO_CODEC->ADC2_REG |= (1u << 31);	// FMINR
	AUDIO_CODEC->ADC3_REG |= (1u << 31);	// MIC3

	// DAC Analog Control Register
	// количество каналов должно соотыетствовать DMABUFFSTEP16TX
	AUDIO_CODEC->DAC_REG |= (1u << 15) | (1u << 14);	// DACL_EN, DACR_EN

	// See WITHADAPTERCODEC1WIDTH and WITHADAPTERCODEC1SHIFT
	AUDIO_CODEC->AC_ADC_FIFOC |= (1u << 16);	// RX_SAMPLE_BITS 1: 20 bits 0: 16 bits
	AUDIO_CODEC->AC_ADC_FIFOC &= ~ (1u << 24);	// RX_FIFO_MODE 0: Expanding ‘0’ at LSB of TX FIFO register
	AUDIO_CODEC->AC_ADC_FIFOC |= (1u << 3);	// ADC_DRQ_EN

	AUDIO_CODEC->AC_DAC_FIFOC |= (1u << 5);	// TX_SAMPLE_BITS 1: 20 bits 0: 16 bits
	AUDIO_CODEC->AC_DAC_FIFOC &= ~ (3u << 24);	// FIFO_MODE 00/10: FIFO_I[19:0] = {TXDATA[31:12]
	AUDIO_CODEC->AC_DAC_FIFOC |= (1u << 4);	// DAC_DRQ_EN

	// Установка аналоговых параметрв
	AUDIO_CODEC->ADC_VOL_CTRL1 = 0x00FFFFFF;

	AUDIO_CODEC->ADC1_REG |= (1u << 27);	// FMINLEN FMINL Enable
	//AUDIO_CODEC->ADC1_REG |= (1u << 23);	// LINEINLEN LINEINL Enable

	AUDIO_CODEC->ADC2_REG |= (1u << 27);	// FMINREN FMINR Enable
	//AUDIO_CODEC->ADC2_REG |= (1u << 23);	// LINEINREN LINEINR Enable

	AUDIO_CODEC->ADC3_REG |= (1u << 30);	// MIC3_PGA_EN
	//AUDIO_CODEC->ADC3_REG |= (1u << 28);	// MIC3_SIN_EN MIC3 Single Input Enable

#endif
}

/* встороенный в процессор кодек */
static void hardware_hwblock_enable_codec1(uint_fast8_t state)
{
	if (state)
	{
		AUDIO_CODEC->AC_DAC_DPC |= (1u << 31);		// DAC Digital Part Enable
		AUDIO_CODEC->AC_ADC_FIFOC |= (1u << 28);	// ADC Digital Part Enable
	}
	else
	{
		AUDIO_CODEC->AC_ADC_FIFOC &= ~ (1u << 28);	// ADC Digital Part Enable
		AUDIO_CODEC->AC_DAC_DPC &= ~ (1u << 31);	// DAC Digital Part Enable
	}
}

static void DMAC_AudioCodec_RX_initialize_codec1(void)
{
	const size_t dw = sizeof (aubufv_t);
	static ALIGNX_BEGIN uint32_t descr0 [3] [DMAC_DESC_SIZE] ALIGNX_END;
	const unsigned dmach = DMAC_AudioCodec_RX_Ch;
	const unsigned sdwt = dmac_desc_datawidth(dw * 8);		// DMA Source Data Width
	const unsigned ddwt = dmac_desc_datawidth(dw * 8);	// DMA Destination Data Width
	const unsigned NBYTES = DMABUFFSIZE16RX * dw;
	const uintptr_t portaddr = (uintptr_t) & AUDIO_CODEC->AC_ADC_RXDATA;

	const uint_fast32_t parameterDMAC = 0;
	const uint_fast32_t configDMAC =
		0 * (1u << 30) |	// BMODE_SEL
		ddwt * (1u << 25) |	// DMA Destination Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		0 * (1u << 24) |	// DMA Destination Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 22) |	// DMA Destination Block Size
		DMAC_DstReqDRAM * (1u << 16) |	// DMA Destination DRQ Type
		sdwt * (1u << 9) |	// DMA Source Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		1 * (1u << 8) |	// DMA Source Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 6) |	// DMA Source Block Size
		DMAC_SrcReqAudioCodec * (1u << 0) |	// DMA Source DRQ Type
		0;

	// Six words of DMAC sescriptor: (Link=0xFFFFF800 for last)
	descr0 [0] [0] = configDMAC;			// Cofigurarion
	descr0 [0] [1] = portaddr;				// Source Address
	descr0 [0] [2] = dma_invalidate16rx(allocate_dmabuffer16rx());				// Destination Address
	descr0 [0] [3] = NBYTES;				// Byte Counter
	descr0 [0] [4] = parameterDMAC;			// Parameter
	descr0 [0] [5] = (uintptr_t) descr0 [1];	// Link to next

	descr0 [1] [0] = configDMAC;			// Cofigurarion
	descr0 [1] [1] = portaddr;				// Source Address
	descr0 [1] [2] = dma_invalidate16rx(allocate_dmabuffer16rx());				// Destination Address
	descr0 [1] [3] = NBYTES;				// Byte Counter
	descr0 [1] [4] = parameterDMAC;				// Parameter
	descr0 [1] [5] = (uintptr_t) descr0 [2];	// Link to previous

	descr0 [2] [0] = configDMAC;			// Cofigurarion
	descr0 [2] [1] = portaddr;				// Source Address
	descr0 [2] [2] = dma_invalidate16rx(allocate_dmabuffer16rx());				// Destination Address
	descr0 [2] [3] = NBYTES;				// Byte Counter
	descr0 [2] [4] = parameterDMAC;				// Parameter
	descr0 [2] [5] = (uintptr_t) descr0 [0];	// Link to previous

	uintptr_t descraddr = (uintptr_t) descr0;
	dcache_clean(descraddr, sizeof descr0);

	DMAC_clock_initialize();

	DMAC->CH [dmach].DMAC_EN_REGN = 0;	// 0: Disabled

	DMAC->CH [dmach].DMAC_DESC_ADDR_REGN = descraddr;
	while (DMAC->CH [dmach].DMAC_DESC_ADDR_REGN != descraddr)
		;

	// 0x04: Queue, 0x02: Pkq, 0x01: half
	DMAC_SetHandler(dmach, DMAC_IRQ_EN_FLAG_VALUE, DMA_I2Sx_AudioCodec_RX_Handler_codec1);

	DMAC->CH [dmach].DMAC_PAU_REGN = 0;	// 0: Resume Transferring
	DMAC->CH [dmach].DMAC_EN_REGN = 1;	// 1: Enabled
}

static void DMAC_AudioCodec_TX_initialize_codec1(void)
{
	const size_t dw = sizeof (aubufv_t);
	static ALIGNX_BEGIN uint32_t descr0 [3] [DMAC_DESC_SIZE] ALIGNX_END;
	const unsigned dmach = DMAC_AudioCodec_TX_Ch;
	const unsigned sdwt = dmac_desc_datawidth(dw * 8);	// DMA Source Data Width
	const unsigned ddwt = dmac_desc_datawidth(dw * 8);		// DMA Destination Data Width
	const unsigned NBYTES = DMABUFFSIZE16TX * dw;
	const uintptr_t portaddr = (uintptr_t) & AUDIO_CODEC->AC_DAC_TXDATA;

	const uint_fast32_t parameterDMAC = 0;
	const uint_fast32_t configDMAC =
		0 * (1u << 30) |	// BMODE_SEL
		ddwt * (1u << 25) |	// DMA Destination Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		1 * (1u << 24) |	// DMA Destination Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 22) |	// DMA Destination Block Size
		DMAC_DstReqAudioCodec * (1u << 16) |	// DMA Destination DRQ Type
		sdwt * (1u << 9) |	// DMA Source Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		0 * (1u << 8) |	// DMA Source Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 6) |	// DMA Source Block Size
		DMAC_SrcReqDRAM * (1u << 0) |	// DMA Source DRQ Type
		0;

	// Six words of DMAC sescriptor: (Link=0xFFFFF800 for last)
	descr0 [0] [0] = configDMAC;			// Cofigurarion
	descr0 [0] [1] = dma_flush16tx(getfilled_dmabuffer16txphones());			// Source Address
	descr0 [0] [2] = portaddr;				// Destination Address
	descr0 [0] [3] = NBYTES;				// Byte Counter
	descr0 [0] [4] = parameterDMAC;			// Parameter
	descr0 [0] [5] = (uintptr_t) descr0 [1];	// Link to next

	descr0 [1] [0] = configDMAC;			// Cofigurarion
	descr0 [1] [1] = dma_flush16tx(getfilled_dmabuffer16txphones());			// Source Address
	descr0 [1] [2] = portaddr;				// Destination Address
	descr0 [1] [3] = NBYTES;				// Byte Counter
	descr0 [1] [4] = parameterDMAC;			// Parameter
	descr0 [1] [5] = (uintptr_t) descr0 [2];	// Link to previous

	descr0 [2] [0] = configDMAC;			// Cofigurarion
	descr0 [2] [1] = dma_flush16tx(getfilled_dmabuffer16txphones());			// Source Address
	descr0 [2] [2] = portaddr;				// Destination Address
	descr0 [2] [3] = NBYTES;				// Byte Counter
	descr0 [2] [4] = parameterDMAC;			// Parameter
	descr0 [2] [5] = (uintptr_t) descr0 [0];	// Link to previous

	uintptr_t descraddr = (uintptr_t) descr0;
	dcache_clean(descraddr, sizeof descr0);

	DMAC_clock_initialize();

	DMAC->CH [dmach].DMAC_EN_REGN = 0;	// 0: Disabled

	DMAC->CH [dmach].DMAC_DESC_ADDR_REGN = descraddr;
	while (DMAC->CH [dmach].DMAC_DESC_ADDR_REGN != descraddr)
		;

	// 0x04: Queue, 0x02: Pkq, 0x01: half
	DMAC_SetHandler(dmach, DMAC_IRQ_EN_FLAG_VALUE, DMA_I2Sx_AudioCodec_TX_Handler_codec1);

	DMAC->CH [dmach].DMAC_PAU_REGN = 0;	// 0: Resume Transferring
	DMAC->CH [dmach].DMAC_EN_REGN = 1;	// 1: Enabled
}

static const codechw_t audiocodechw_hwblock_duplex_master =
{
	hardware_hwblock_master_duplex_initialize_codec1,
	hardware_dummy_initialize,
	DMAC_AudioCodec_RX_initialize_codec1,
	DMAC_AudioCodec_TX_initialize_codec1,
	hardware_hwblock_enable_codec1,
	hardware_dummy_enable,
	"audiocodechw-hwblock-duplex-master"
};

#endif /* WITHCODEC1_WHBLOCK_DUPLEX_MASTER */

static const codechw_t audiocodechw_i2s1_duplex_slave =
{
	hardware_i2s1_slave_duplex_initialize_codec1,
	hardware_dummy_initialize,
	DMAC_I2S1_RX_initialize_codec1,
	DMAC_I2S1_TX_initialize_codec1,
	hardware_i2s1_enable_codec1,
	hardware_dummy_enable,
	"audiocodechw-i2s1-duplex-slave"
};

#if defined (I2S2)

#if ! defined (CPUSTYLE_A64)

static const codechw_t audiocodechw_i2s2_duplex_slave =
{
	hardware_i2s2_slave_duplex_initialize_codec1,
	hardware_dummy_initialize,
	DMAC_I2S2_RX_initialize_codec1,
	DMAC_I2S2_TX_initialize_codec1,
	hardware_i2s2_enable_codec1,
	hardware_dummy_enable,
	"audiocodechw-i2s2-duplex-slave"
};

static const codechw_t fpgacodechw_i2s2_duplex_master =
{
	hardware_i2s2_master_duplex_initialize_fpga,
	hardware_dummy_initialize,
	DMAC_I2S2_RX_initialize_fpga,
	DMAC_I2S2_TX_initialize_fpga,
	hardware_i2s2_enable_fpga,
	hardware_dummy_enable,
	"fpgacodechw-i2s2-duplex-master"
};

static const codechw_t fpgacodechw_i2s2_duplex_slave =
{
	hardware_i2s2_slave_duplex_initialize_fpga,
	hardware_dummy_initialize,
	DMAC_I2S2_RX_initialize_fpga,
	DMAC_I2S2_TX_initialize_fpga,
	hardware_i2s2_enable_fpga,
	hardware_dummy_enable,
	"fpgacodechw-i2s2-duplex-slave"
};

#endif /* ! defined (CPUSTYLE_A64) */

#endif /* defined (I2S2) */

static const codechw_t fpgacodechw_i2s1_duplex_slave =
{
	hardware_i2s1_slave_duplex_initialize_fpga,
	hardware_dummy_initialize,
	DMAC_I2S1_RX_initialize_fpga,
	DMAC_I2S1_TX_initialize_fpga,
	hardware_i2s1_enable_fpga,
	hardware_dummy_enable,
	"fpgacodechw-i2s1-duplex-slave"
};

#if CPUSTYLE_A64

// FPGA interface
static void hardware_i2s0_master_duplex_initialize_fpga(void)
{
	hardware_i2s_initialize(0, I2S0, 1, WITHFPGAIF_FRAMEBITS / 32, ARMSAIRATE, WITHFPGAIF_FRAMEBITS, HARDWARE_I2S2HW_DIN, HARDWARE_I2S2HW_DOUT);
	I2S0HW_INITIALIZE(1);
}

static void hardware_i2s0_slave_duplex_initialize_fpga(void)
{
	hardware_i2s_initialize(0, I2S0, 0, WITHFPGAIF_FRAMEBITS / 32, ARMSAIRATE, WITHFPGAIF_FRAMEBITS, HARDWARE_I2S1HW_DIN, HARDWARE_I2S1HW_DOUT);
	I2S0HW_INITIALIZE(0);
}


static void hardware_i2s0_enable_fpga(uint_fast8_t state)
{
	state = 0;
	I2S_PCM_TypeDef * const i2s = I2S0;
	if (state)
	{
		i2s->I2S_PCM_CTL |=
			(1u << 2) |	// TXEN
			(1u << 1) |	// RXEN
			0;
		i2s->I2S_PCM_CTL |= (1u << 0); // GEN Globe Enable
	}
	else
	{
		i2s->I2S_PCM_CTL &= ~ (1u << 0); // GEN Globe Enable
		i2s->I2S_PCM_CTL &= ~ (1u << 2); // TXEN
		i2s->I2S_PCM_CTL &= ~ (1u << 1); // RXEN
	}
}

static void DMAC_I2S0_RX_initialize_fpga(void)
{
	const size_t dw = sizeof (IFADCvalue_t);
	static ALIGNX_BEGIN uint32_t descr0 [3] [DMAC_DESC_SIZE] ALIGNX_END;
	const unsigned dmach = DMAC_I2S0_RX_Ch;
	const unsigned sdwt = dmac_desc_datawidth(dw * 8);		// DMA Source Data Width
	const unsigned ddwt = dmac_desc_datawidth(dw * 8);	// DMA Destination Data Width
	const unsigned NBYTES = DMABUFFSIZE32RX * dw;
	const uintptr_t portaddr = (uintptr_t) & I2S0->I2S_PCM_RXFIFO;

	const uint_fast32_t parameterDMAC = 0;
	const uint_fast32_t configDMAC =
		0 * (1u << 30) |	// BMODE_SEL
		ddwt * (1u << 25) |	// DMA Destination Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		0 * (1u << 24) |	// DMA Destination Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 22) |	// DMA Destination Block Size
		DMAC_DstReqDRAM * (1u << 16) |	// DMA Destination DRQ Type
		sdwt * (1u << 9) |	// DMA Source Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		1 * (1u << 8) |	// DMA Source Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 6) |	// DMA Source Block Size
		DMAC_SrcReqI2S0_RX * (1u << 0) |	// DMA Source DRQ Type
		0;

	// Six words of DMAC sescriptor: (Link=0xFFFFF800 for last)
	descr0 [0] [0] = configDMAC;			// Cofigurarion
	descr0 [0] [1] = portaddr;				// Source Address
	descr0 [0] [2] = dma_invalidate32rx(allocate_dmabuffer32rx());		// Destination Address
	descr0 [0] [3] = NBYTES;				// Byte Counter
	descr0 [0] [4] = parameterDMAC;			// Parameter
	descr0 [0] [5] = (uintptr_t) descr0 [1];	// Link to next

	descr0 [1] [0] = configDMAC;			// Cofigurarion
	descr0 [1] [1] = portaddr;				// Source Address
	descr0 [1] [2] = dma_invalidate32rx(allocate_dmabuffer32rx());		// Destination Address
	descr0 [1] [3] = NBYTES;				// Byte Counter
	descr0 [1] [4] = parameterDMAC;			// Parameter
	descr0 [1] [5] = (uintptr_t) descr0 [2];	// Link to previous

	descr0 [2] [0] = configDMAC;			// Cofigurarion
	descr0 [2] [1] = portaddr;				// Source Address
	descr0 [2] [2] = dma_invalidate32rx(allocate_dmabuffer32rx());		// Destination Address
	descr0 [2] [3] = NBYTES;				// Byte Counter
	descr0 [2] [4] = parameterDMAC;			// Parameter
	descr0 [2] [5] = (uintptr_t) descr0 [0];	// Link to previous

	uintptr_t descraddr = (uintptr_t) descr0;
	dcache_clean(descraddr, sizeof descr0);

	DMAC_clock_initialize();

	DMAC->CH [dmach].DMAC_EN_REGN = 0;	// 0: Disabled

	DMAC->CH [dmach].DMAC_DESC_ADDR_REGN = descraddr;
	while (DMAC->CH [dmach].DMAC_DESC_ADDR_REGN != descraddr)
		;

	// 0x04: Queue, 0x02: Pkq, 0x01: half
	DMAC_SetHandler(dmach, DMAC_IRQ_EN_FLAG_VALUE, DMA_I2Sx_RX_Handler_fpga);

	DMAC->CH [dmach].DMAC_PAU_REGN = 0;	// 0: Resume Transferring
	DMAC->CH [dmach].DMAC_EN_REGN = 1;	// 1: Enabled
}

static void DMAC_I2S0_TX_initialize_fpga(void)
{
	const size_t dw = sizeof (IFDACvalue_t);
	static ALIGNX_BEGIN uint32_t descr0 [3] [DMAC_DESC_SIZE] ALIGNX_END;
	const unsigned dmach = DMAC_I2S0_TX_Ch;
	const unsigned sdwt = dmac_desc_datawidth(dw * 8);	// DMA Source Data Width
	const unsigned ddwt = dmac_desc_datawidth(dw * 8);		// DMA Destination Data Width
	const unsigned NBYTES = DMABUFFSIZE32TX * dw;
	const uintptr_t portaddr = (uintptr_t) & I2S0->I2S_PCM_TXFIFO;

	const uint_fast32_t parameterDMAC = 0;
	const uint_fast32_t configDMAC =
		0 * (1u << 30) |	// BMODE_SEL
		ddwt * (1u << 25) |	// DMA Destination Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		1 * (1u << 24) |	// DMA Destination Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 22) |	// DMA Destination Block Size
		DMAC_DstReqI2S0_TX * (1u << 16) |	// DMA Destination DRQ Type
		sdwt * (1u << 9) |	// DMA Source Data Width 00: 8-bit 01: 16-bit 10: 32-bit 11: 64-bit
		0 * (1u << 8) |	// DMA Source Address Mode 0: Linear Mode 1: IO Mode
		0 * (1u << 6) |	// DMA Source Block Size
		DMAC_SrcReqDRAM * (1u << 0) |	// DMA Source DRQ Type
		0;

	// Six words of DMAC sescriptor: (Link=0xFFFFF800 for last)
	descr0 [0] [0] = configDMAC;			// Cofigurarion
	descr0 [0] [1] = dma_flush32tx(allocate_dmabuffer32tx());				// Source Address
	descr0 [0] [2] = portaddr;				// Destination Address
	descr0 [0] [3] = NBYTES;				// Byte Counter
	descr0 [0] [4] = parameterDMAC;			// Parameter
	descr0 [0] [5] = (uintptr_t) descr0 [1];	// Link to next

	descr0 [1] [0] = configDMAC;			// Cofigurarion
	descr0 [1] [1] = dma_flush32tx(allocate_dmabuffer32tx());				// Source Address
	descr0 [1] [2] = portaddr;				// Destination Address
	descr0 [1] [3] = NBYTES;				// Byte Counter
	descr0 [1] [4] = parameterDMAC;			// Parameter
	descr0 [1] [5] = (uintptr_t) descr0 [2];	// Link to previous

	descr0 [2] [0] = configDMAC;			// Cofigurarion
	descr0 [2] [1] = dma_flush32tx(allocate_dmabuffer32tx());				// Source Address
	descr0 [2] [2] = portaddr;				// Destination Address
	descr0 [2] [3] = NBYTES;				// Byte Counter
	descr0 [2] [4] = parameterDMAC;			// Parameter
	descr0 [2] [5] = (uintptr_t) descr0 [0];	// Link to previous

	uintptr_t descraddr = (uintptr_t) descr0;
	dcache_clean(descraddr, sizeof descr0);

	DMAC_clock_initialize();

	DMAC->CH [dmach].DMAC_EN_REGN = 0;	// 0: Disabled

	DMAC->CH [dmach].DMAC_DESC_ADDR_REGN = descraddr;
	while (DMAC->CH [dmach].DMAC_DESC_ADDR_REGN != descraddr)
		;

	// 0x04: Queue, 0x02: Pkq, 0x01: half
	DMAC_SetHandler(dmach, DMAC_IRQ_EN_FLAG_VALUE, DMA_I2Sx_TX_Handler_fpga);

	DMAC->CH [dmach].DMAC_PAU_REGN = 0;	// 0: Resume Transferring
	DMAC->CH [dmach].DMAC_EN_REGN = 1;	// 1: Enabled
}

static const codechw_t fpgacodechw_i2s0_duplex_master =
{
	hardware_i2s0_master_duplex_initialize_fpga,
	hardware_dummy_initialize,
	DMAC_I2S0_RX_initialize_fpga,
	DMAC_I2S0_TX_initialize_fpga,
	hardware_i2s0_enable_fpga,
	hardware_dummy_enable,
	"fpgacodechw-i2s0-duplex-master"
};

static const codechw_t fpgacodechw_i2s0_duplex_slave =
{
	hardware_i2s0_slave_duplex_initialize_fpga,
	hardware_dummy_initialize,
	DMAC_I2S0_RX_initialize_fpga,
	DMAC_I2S0_TX_initialize_fpga,
	hardware_i2s0_enable_fpga,
	hardware_dummy_enable,
	"fpgacodechw-i2s0-duplex-slave"
};

#endif /* CPUSTYLE_A64 */

#elif CPUSTYLE_R7S721
	
enum
{
	R7S721_SSIF_CKDIV1 = 0, // CKDV	0000: AUDIOf/1: 12,288 -> 12,288 (48 kS, 128 bit, stereo)
	R7S721_SSIF_CKDIV2 = 1,
	R7S721_SSIF_CKDIV4 = 2,	// CKDV	0010: AUDIOf/4: 12,288 -> 3.072 (48 kS, 32 bit, stereo)
	R7S721_SSIF_CKDIV6 = 8,
	R7S721_SSIF_CKDIV8 = 3,
	R7S721_SSIF_CKDIV12 = 9,
	R7S721_SSIF_CKDIV16 = 4,
	R7S721_SSIF_CKDIV24 = 10,
	R7S721_SSIF_CKDIV32 = 5,
	R7S721_SSIF_CKDIV48 = 11,
	R7S721_SSIF_CKDIV64 = 6,
	R7S721_SSIF_CKDIV96 = 12,
	R7S721_SSIF_CKDIV128 = 7,
};

#if CODEC1_IFC_MASTER
	#define R7S721_SSIF0_MASTER 0	// AUDIO CODEC I2S INTERFACE
#else /* CODEC1_IFC_MASTER */
	#define R7S721_SSIF0_MASTER 1	// AUDIO CODEC I2S INTERFACE
#endif /* CODEC1_IFC_MASTER */

#define R7S721_SSIF1_MASTER 1	// FGA I2S INTERFACE #1
#define R7S721_SSIF2_MASTER 1	// FGA I2S INTERFACE #2 (spectrum)

#if CODEC1_FRAMEBITS == 64
	#define R7S721_SSIF0_CKDIV_val (R7S721_SSIF_CKDIV4 * (1u << 4))	// 0010: AUDIOц/4: 12,288 -> 3.072 (48 kS, 32 bit, stereo)
	#define R7S721_SSIF0_SWL_val (3 * (1u << 16))	// SWL 3: 32 bit
	#define R7S721_SSIF0_DWL_val (6 * (1u << 19))	// DWL 6: 32 bit

#else /* CODEC1_FRAMEBITS == 64 */
	#define R7S721_SSIF0_CKDIV_val (R7S721_SSIF_CKDIV8 * (1u << 4))	// 0011: AUDIOц/8: 12,288 -> 1.536 (48 kS, 16 bit, stereo)
	#define R7S721_SSIF0_SWL_val (1 * (1u << 16))	// SWL 1: 16 bit
	#define R7S721_SSIF0_DWL_val (1 * (1u << 19))	// DWL 1: 16 bit

#endif /* CODEC1_FRAMEBITS == 64 */

#if WITHI2S2HW

// audio codec
// DMA по приему SSIF0 - обработчик прерывания

static RAMFUNC_NONILINE void r7s721_ssif0_rxdma_audiorx(void)
{
	//__DMB();
	// SR (bt 7)
	// Indicates the register set currently selected in register mode.
	// 0: Next0 Register Set
	// 1: Next1 Register Set
	const uint_fast8_t b = (DMAC0.CHSTAT_n & (1U << DMAC0_CHSTAT_n_SR_SHIFT)) != 0;	// SR
	if (b != 0)
	{
		const uintptr_t addr = DMAC0.N0DA_n;
		DMAC0.N0DA_n = dma_invalidate16rx(allocate_dmabuffer16rx());
		DMAC0.CHCFG_n |= DMAC0_CHCFG_n_REN;	// REN bit
		processing_dmabuffer16rx(addr);
	}
	else
	{
		const uintptr_t addr = DMAC0.N1DA_n;
		DMAC0.N1DA_n = dma_invalidate16rx(allocate_dmabuffer16rx());
		DMAC0.CHCFG_n |= DMAC0_CHCFG_n_REN;	// REN bit
		processing_dmabuffer16rx(addr);
	}
}

// audio codec
// DMA по передаче SSIF0 - обработчик прерывания
// Use dcache_clean

static void r7s721_ssif0_txdma_audio(void)
{
	//__DMB();
	// SR (bt 7)
	// Indicates the register set currently selected in register mode.
	// 0: Next0 Register Set
	// 1: Next1 Register Set
	const uint_fast8_t b = (DMAC1.CHSTAT_n & DMAC1_CHSTAT_n_SR) != 0;	// SR
	if (b != 0)
	{
		const uintptr_t addr = DMAC1.N0SA_n;
		DMAC1.N0SA_n = dma_flush16tx(getfilled_dmabuffer16txphones());
		DMAC1.CHCFG_n |= DMAC1_CHCFG_n_REN;	// REN bit
		release_dmabuffer16tx(addr);
	}
	else
	{
		const uintptr_t addr = DMAC1.N1SA_n;
		DMAC1.N1SA_n = dma_flush16tx(getfilled_dmabuffer16txphones());
		DMAC1.CHCFG_n |= DMAC1_CHCFG_n_REN;	// REN bit
		release_dmabuffer16tx(addr);
	}
}


// audio codec
// DMA по приёму SSIF0

static void r7s721_ssif0_dmarx_initialize_codec1_rx(void)
{
	enum { id = 0 };		// 0: DMAC0
	// DMAC0
   /* Set Source Start Address */
    DMAC0.N0SA_n = (uintptr_t) & SSIF0.SSIFRDR;	// Fixed source address
    DMAC0.N1SA_n = (uintptr_t) & SSIF0.SSIFRDR;	// Fixed source address

    /* Set Destination Start Address */
	DMAC0.N0DA_n = dma_invalidate16rx(allocate_dmabuffer16rx());
	DMAC0.N1DA_n = dma_invalidate16rx(allocate_dmabuffer16rx());

    /* Set Transfer Size */
    DMAC0.N0TB_n = DMABUFFSIZE16RX * sizeof (aubufv_t);	// размер в байтах
    DMAC0.N1TB_n = DMABUFFSIZE16RX * sizeof (aubufv_t);	// размер в байтах

	// Values from Table 9.4 On-Chip Peripheral Module Requests
	// SSIRXI0 (receive data full)
	const uint_fast8_t mid = 0x38;	
	const uint_fast8_t rid = 2;		
	const uint_fast8_t tm = 0;		
	const uint_fast8_t am = 2;		
	const uint_fast8_t lvl = 1;		
	const uint_fast8_t reqd = 0;

	DMAC0.CHCFG_n =
		0 * (1U << 31) |	// DMS	0: Register mode
		1 * (1U << 30) |	// REN	1: Continues DMA transfers.
		1 * (1U << 29) |	// RSW	1: Inverts RSEL automatically after a DMA transaction.
		0 * (1U << 28) |	// RSEL	0: Executes the Next0 Register Set
		0 * (1U << 27) |	// SBE	0: Stops the DMA transfer without sweeping the buffer (initial value).
		0 * (1U << 24) |	// DEM	0: Does not mask the DMA transfer end interrupt - прерывания каждый раз после TC
		tm * (1U << 22) |	// TM	0: Single transfer mode - берётся из Table 9.4
		0 * (1U << 21) |	// DAD	0: Increment destination address
		1 * (1U << 20) |	// SAD	1: Fixed source address
		2 * (1U << 16) |	// DDS	2: 32 bits, 1: 16 bits (Destination Data Size)
		2 * (1U << 12) |	// SDS	2: 32 bits, 1: 16 bits (Source Data Size)
		am * (1U << 8) |	// AM	1: ACK mode: Level mode (active until the transfer request from an on-chip peripheral module
		lvl * (1U << 6) |	// LVL	1: Detects based on the level.
		1 * (1U << 5) |		// HIEN	1: When LVL = 1: Detects a request when the signal is at the High level.
		reqd * (1U << 3) |	// REQD		Request Direction
		(id & 0x07) * (1U << 0) |		// SEL	0: CH0/CH8
		0;

	enum { dmarsshift = (id & 0x01) * 16 };
	DMAC01.DMARS = (DMAC01.DMARS & ~ (0x1FFul << dmarsshift)) |
		mid * (1U << (2 + dmarsshift)) |		// MID
		rid * (1U << (0 + dmarsshift)) |		// RID
		0;

    DMAC07.DCTRL_0_7 = (DMAC07.DCTRL_0_7 & ~ (/*(1U << 1) | */(1U << 0))) |
		//1 * (1U << 1) |		// LVINT	1: Level output
		1 * (1U << 0) |		// PR		1: Round robin mode
		0;

	arm_hardware_set_handler_realtime(DMAINT0_IRQn, r7s721_ssif0_rxdma_audiorx);

	DMAC0.CHCTRL_n = 1 * (1U << 3);		// SWRST
	DMAC0.CHCTRL_n = 1 * (1U << 17);	// CLRINTMSK
	DMAC0.CHCTRL_n = 1 * (1U << 0);		// SETEN
}

// audio codec
// DMA по передаче SSIF0
// Use dcache_clean

static void r7s721_ssif0_dmatx_initialize_codec1_tx(void)
{
	enum { id = 1 };	// 1: DMAC1
	// DMAC1
	/* Set Source Start Address */
	DMAC1.N0SA_n = dma_flush16tx(allocate_dmabuffer16tx());
	DMAC1.N1SA_n = dma_flush16tx(allocate_dmabuffer16tx());

    /* Set Destination Start Address */
    DMAC1.N0DA_n = (uintptr_t) & SSIF0.SSIFTDR;	// Fixed destination address
    DMAC1.N1DA_n = (uintptr_t) & SSIF0.SSIFTDR;	// Fixed destination address

    /* Set Transfer Size */
    DMAC1.N0TB_n = DMABUFFSIZE16TX * sizeof (aubufv_t);	// размер в байтах
    DMAC1.N1TB_n = DMABUFFSIZE16TX * sizeof (aubufv_t);	// размер в байтах

	// Values from Table 9.4 On-Chip Peripheral Module Requests
	// SSITXI0 (transmit data empty)
	const uint_fast8_t mid = 0x38;	
	const uint_fast8_t rid = 1;		
	const uint_fast8_t tm = 0;		
	const uint_fast8_t am = 2;		
	const uint_fast8_t lvl = 1;		
	const uint_fast8_t reqd = 1;	

	DMAC1.CHCFG_n =
		0 * (1U << 31) |	// DMS	0: Register mode
		1 * (1U << 30) |	// REN	1: Continues DMA transfers.
		1 * (1U << 29) |	// RSW	1: Inverts RSEL automatically after a DMA transaction.
		0 * (1U << 28) |	// RSEL	0: Executes the Next0 Register Set
		0 * (1U << 27) |	// SBE	0: Stops the DMA transfer without sweeping the buffer (initial value).
		0 * (1U << 24) |	// DEM	0: Does not mask the DMA transfer end interrupt - прерывания каждый раз после TC
		tm * (1U << 22) |	// TM	0: Single transfer mode - берётся из Table 9.4
		1 * (1U << 21) |	// DAD	1: Fixed destination address
		0 * (1U << 20) |	// SAD	0: Increment source address
		2 * (1U << 16) |	// DDS	2: 32 bits, 1: 16 bits (Destination Data Size)
		2 * (1U << 12) |	// SDS	2: 32 bits, 1: 16 bits (Source Data Size)
		am * (1U << 8) |	// AM	1: ACK mode: Level mode (active until the transfer request from an on-chip peripheral module
		lvl * (1U << 6) |	// LVL	1: Detects based on the level.
		1 * (1U << 5) |		// HIEN	1: When LVL = 1: Detects a request when the signal is at the High level.
		reqd * (1U << 3) |	// REQD		Request Direction
		(id & 0x07) * (1U << 0) |		// SEL	0: CH0/CH8
		0;

	enum { dmarsshift = (id & 0x01) * 16 };
	DMAC01.DMARS = (DMAC01.DMARS & ~ (0x1FFul << dmarsshift)) |
		mid * (1U << (2 + dmarsshift)) |		// MID
		rid * (1U << (0 + dmarsshift)) |		// RID
		0;

    DMAC07.DCTRL_0_7 = (DMAC07.DCTRL_0_7 & ~ (/*(1U << 1) | */(1U << 0))) |
		//1 * (1U << 1) |		// LVINT	1: Level output
		1 * (1U << 0) |		// PR		1: Round robin mode
		0;

	arm_hardware_set_handler_realtime(DMAINT1_IRQn, r7s721_ssif0_txdma_audio);

	DMAC1.CHCTRL_n = 1 * (1U << 3);		// SWRST
	DMAC1.CHCTRL_n = 1 * (1U << 17);	// CLRINTMSK
	DMAC1.CHCTRL_n = 1 * (1U << 0);		// SETEN
}

// Возможно, удастся перейти на master - но надо решить проблему с очередями (FIFO) в FPGA
// Была попытка сделать так: AUDIO_X1 формируется только после появления i2s_ready - не работает
// Правда, и в SLAVE нельзя сказать что работает - около пяти секунд проходит до начала нормальной раболты.

// AUDIO CODEC I2S INTERFACE
static void r7s721_ssif0_duplex_initialize_codec1(void)
{
	const uint_fast8_t master = R7S721_SSIF0_MASTER;
    /* ---- Supply clock to the SSIF(channel 0) ---- */
	CPG.STBCR11 &= ~ (1U << 5);	// Module Stop 115 0: Channel 0 of the serial sound interface runs.
	(void) CPG.STBCR11;			/* Dummy read */

	// I2S compatible
	// SCKP = 0, SWSP = 0, DEL = 0, CHNL = 00
	// System word length = data word length

	// Control Register (SSICR)
	SSIF0.SSICR = 
		R7S721_USE_AUDIO_CLK * (1u << 30) |		// CKS 1: AUDIO_CLK input 0: AUDIO_X1 input
		0 * (1u << 22) |		// CHNL		00: Having one channel per system word (I2S complaint)
		R7S721_SSIF0_DWL_val |		// DWL
		R7S721_SSIF0_SWL_val |		// SWL
		master * (1u << 15) |		// SCKD		1: Serial bit clock is output, master mode.
		master * (1u << 14) |		// SWSD		1: Serial word select is output, master mode.
		0 * (1u << 13) |		// SCKP		0: Данные на выходе меняются по спадающему фронту (I2S complaint)
		0 * (1u << 12) |		// SWSP		0: SSIWS is low for the 1st channel, high for the 2nd channel.  (I2S complaint)
		0 * (1u << 11) |		// SPDP		0: Padding bits are low.
		0 * (1u << 10) |		// SDTA
		0 * (1u << 9) |		// PDTA		1: 16 бит правого канала - биты 31..16 при чтении/записи регистра данных
#if CODEC1_FORMATI2S_PHILIPS
		0 * (1u << 8) |		// DEL	0: 1 clock cycle delay between SSIWS and SSIDATA
#else /* CODEC1_FORMATI2S_PHILIPS */
		1 * (1u << 8) |		// DEL	1: No delay between SSIWS and SSIDATA
#endif /* CODEC1_FORMATI2S_PHILIPS */
		master * R7S721_SSIF0_CKDIV_val |		// CKDV	0011: AUDIOц/8: 12,288 -> 1.536 (48 kS, 16 bit, stereo)
		0;

	// FIFO Control Register (SSIFCR)
	SSIF0.SSIFCR = 
		2 * (1u << 6) |	// TTRG Transmit Data Trigger Number
		2 * (1u << 4) |	// RTRG Receive Data Trigger Number
		1 * (1u << 3) |		// TIE	Transmit Interrupt Enable
		1 * (1u << 2) |		// RIE	Receive Interrupt Enable
		//1 * (1u << 1) |		// TFRST Transmit FIFO Data Register Reset
		//1 * (1u << 0) |		// RFRST Receive FIFO Data Register Reset
		0;

	HARDWARE_SSIF0_INITIALIZE();	// Подключение синалалов периферийного блока к выводам процессора
}

static void r7s721_ssif0_rx_enable_codec1(uint_fast8_t state)
{
	if (state != 0)
	{
		SSIF0.SSICR |= (1u << 0); // REN
	}
	else
	{
		SSIF0.SSICR &= ~ (1u << 0); // REN
	}
}

static void r7s721_ssif0_tx_enable_codec1(uint_fast8_t state)
{
	if (state != 0)
	{
		SSIF0.SSICR |= (1u << 1); // TEN
	}
	else
	{
		SSIF0.SSICR &= ~ (1u << 1); // TEN
	}
}

static const codechw_t audiocodec_ssif0_duplex_master =
{
	r7s721_ssif0_duplex_initialize_codec1,
	hardware_dummy_initialize,
	r7s721_ssif0_dmarx_initialize_codec1_rx,
	r7s721_ssif0_dmatx_initialize_codec1_tx,
	r7s721_ssif0_rx_enable_codec1,
	r7s721_ssif0_tx_enable_codec1,
	"audiocodechw-ssif0-duplex-master"
};
#endif /* WITHI2S2HW */

#if WITHSAI1HW

// FPGA/IF codec
// DMA по передаче SSIF1 - обработчик прерывания
// Use dcache_clean

static void r7s721_ssif1_txdma_fpgatx(void)
{
	//__DMB();
	// SR (bt 7)
	// Indicates the register set currently selected in register mode.
	// 0: Next0 Register Set
	// 1: Next1 Register Set
	const uint_fast8_t b = (DMAC3.CHSTAT_n & (1U << DMAC3_CHSTAT_n_SR_SHIFT)) != 0;	// SR
	if (b != 0)
	{
		const uintptr_t addr = DMAC3.N0SA_n;
		DMAC3.N0SA_n = dma_flush32tx(getfilled_dmabuffer32tx_main());
		DMAC3.CHCFG_n |= DMAC3_CHCFG_n_REN;	// REN bit
		release_dmabuffer32tx(addr);
	}
	else
	{
		const uintptr_t addr = DMAC3.N1SA_n;
		DMAC3.N1SA_n = dma_flush32tx(getfilled_dmabuffer32tx_main());
		DMAC3.CHCFG_n |= DMAC3_CHCFG_n_REN;	// REN bit
		release_dmabuffer32tx(addr);
	}
}

// FPGA/IF codec
// DMA по приему SSIF1 - обработчик прерывания
// Use dma_invalidate32rx

static RAMFUNC_NONILINE void r7s721_ssif1_rxdma_fpgarx(void)
{
	//__DMB();
	// SR (bt 7)
	// Indicates the register set currently selected in register mode.
	// 0: Next0 Register Set
	// 1: Next1 Register Set
	const uint_fast8_t b = (DMAC2.CHSTAT_n & (1U << DMAC2_CHSTAT_n_SR_SHIFT)) != 0;	// SR
	if (b != 0)
	{
		const uintptr_t addr = DMAC2.N0DA_n;
		DMAC2.N0DA_n = dma_invalidate32rx(allocate_dmabuffer32rx());
		DMAC2.CHCFG_n |= DMAC2_CHCFG_n_REN;	// REN bit
		processing_dmabuffer32rts(addr);
		processing_dmabuffer32rx(addr);
		release_dmabuffer32rx(addr);
	}
	else
	{
		const uintptr_t addr = DMAC2.N1DA_n;
		DMAC2.N1DA_n = dma_invalidate32rx(allocate_dmabuffer32rx());
		DMAC2.CHCFG_n |= DMAC2_CHCFG_n_REN;	// REN bit
		processing_dmabuffer32rts(addr);
		processing_dmabuffer32rx(addr);
		release_dmabuffer32rx(addr);
	}
	buffers_resampleuacin(DMABUFFSIZE32RX / DMABUFFSTEP32RX);
}

// FPGA/IF codec
// DMA по приёму SSIF1
// Use dma_invalidate32rx

static void r7s721_ssif1_dmarx_initialize_fpga_rx(void)
{
	enum { id = 2 };	// 2: DMAC2
	// DMAC2
	/* Set Source Start Address */
    DMAC2.N0SA_n = (uintptr_t) & SSIF1.SSIFRDR;	// Fixed source address
    DMAC2.N1SA_n = (uintptr_t) & SSIF1.SSIFRDR;	// Fixed source address

    /* Set Destination Start Address */
	DMAC2.N0DA_n = dma_invalidate32rx(allocate_dmabuffer32rx());
	DMAC2.N1DA_n = dma_invalidate32rx(allocate_dmabuffer32rx());

	/* Set Transfer Size */
	DMAC2.N0TB_n = DMABUFFSIZE32RX * sizeof (IFADCvalue_t);	// размер в байтах
	DMAC2.N1TB_n = DMABUFFSIZE32RX * sizeof (IFADCvalue_t);	// размер в байтах

	// Values from Table 9.4 On-Chip Peripheral Module Requests
	// SSIRXI1 (receive data full)
	const uint_fast8_t mid = 0x39;	
	const uint_fast8_t rid = 2;		
	const uint_fast8_t tm = 0;		
	const uint_fast8_t am = 2;		
	const uint_fast8_t lvl = 1;		
	const uint_fast8_t reqd = 0;

	DMAC2.CHCFG_n =
		0 * (1U << 31) |	// DMS	0: Register mode
		1 * (1U << 30) |	// REN	1: Continues DMA transfers.
		1 * (1U << 29) |	// RSW	1: Inverts RSEL automatically after a DMA transaction.
		0 * (1U << 28) |	// RSEL	0: Executes the Next0 Register Set
		0 * (1U << 27) |	// SBE	0: Stops the DMA transfer without sweeping the buffer (initial value).
		0 * (1U << 24) |	// DEM	0: Does not mask the DMA transfer end interrupt - прерывания каждый раз после TC
		tm * (1U << 22) |	// TM	0: Single transfer mode - берётся из Table 9.4
		0 * (1U << 21) |	// DAD	0: Increment destination address
		1 * (1U << 20) |	// SAD	1: Fixed source address
		2 * (1U << 16) |	// DDS	2: 32 bits (Destination Data Size)
		2 * (1U << 12) |	// SDS	2: 32 bits (Source Data Size)
		am * (1U << 8) |	// AM	1: ACK mode: Level mode (active until the transfer request from an on-chip peripheral module
		lvl * (1U << 6) |	// LVL	1: Detects based on the level.
		1 * (1U << 5) |		// HIEN	1: When LVL = 1: Detects a request when the signal is at the High level.
		reqd * (1U << 3) |	// REQD		Request Direction
		(id & 0x07) * (1U << 0) |		// SEL	0: CH0/CH8
		0;

	enum { dmarsshift = (id & 0x01) * 16 };
	DMAC23.DMARS = (DMAC23.DMARS & ~ (0x1FFul << dmarsshift)) |
		mid * (1U << (2 + dmarsshift)) |		// MID
		rid * (1U << (0 + dmarsshift)) |		// RID
		0;

    DMAC07.DCTRL_0_7 = (DMAC07.DCTRL_0_7 & ~ (/*(1U << 1) | */(1U << 0))) |
		//1 * (1U << 1) |		// LVINT	1: Level output
		1 * (1U << 0) |		// PR		1: Round robin mode
		0;

	arm_hardware_set_handler_realtime(DMAINT2_IRQn, r7s721_ssif1_rxdma_fpgarx);

	DMAC2.CHCTRL_n = 1 * (1U << 3);		// SWRST
	DMAC2.CHCTRL_n = 1 * (1U << 17);	// CLRINTMSK
	DMAC2.CHCTRL_n = 1 * (1U << 0);		// SETEN
}

// FPGA/IF codec
// DMA по передаче SSIF1
// Use dcache_clean

static void r7s721_ssif1_dmatx_initialize_fpga_tx(void)
{
	enum { id = 3 };	// 3: DMAC3
	// DMAC3
   /* Set Source Start Address */
	DMAC3.N0SA_n = dma_flush32tx(allocate_dmabuffer32tx());
	DMAC3.N1SA_n = dma_flush32tx(allocate_dmabuffer32tx());

    /* Set Destination Start Address */
    DMAC3.N0DA_n = (uintptr_t) & SSIF1.SSIFTDR;	// Fixed destination address
    DMAC3.N1DA_n = (uintptr_t) & SSIF1.SSIFTDR;	// Fixed destination address

    /* Set Transfer Size */
    DMAC3.N0TB_n = DMABUFFSIZE32TX * sizeof (IFDACvalue_t);	// размер в байтах
    DMAC3.N1TB_n = DMABUFFSIZE32TX * sizeof (IFDACvalue_t);	// размер в байтах

	// Values from Table 9.4 On-Chip Peripheral Module Requests
	// SSITXI1 (transmit data empty)
	const uint_fast8_t mid = 0x39;	
	const uint_fast8_t rid = 1;		
	const uint_fast8_t tm = 0;		
	const uint_fast8_t am = 2;		
	const uint_fast8_t lvl = 1;		
	const uint_fast8_t reqd = 1;	

	DMAC3.CHCFG_n =
		0 * (1U << 31) |	// DMS	0: Register mode
		1 * (1U << 30) |	// REN	1: Continues DMA transfers.
		1 * (1U << 29) |	// RSW	1: Inverts RSEL automatically after a DMA transaction.
		0 * (1U << 28) |	// RSEL	0: Executes the Next0 Register Set
		0 * (1U << 27) |	// SBE	0: Stops the DMA transfer without sweeping the buffer (initial value).
		0 * (1U << 24) |	// DEM	0: Does not mask the DMA transfer end interrupt - прерывания каждый раз после TC
		tm * (1U << 22) |	// TM	0: Single transfer mode - берётся из Table 9.4
		1 * (1U << 21) |	// DAD	1: Fixed destination address
		0 * (1U << 20) |	// SAD	0: Increment source address
		2 * (1U << 16) |	// DDS	2: 32 bits (Destination Data Size)
		2 * (1U << 12) |	// SDS	2: 32 bits (Source Data Size)
		am * (1U << 8) |	// AM	1: ACK mode: Level mode (active until the transfer request from an on-chip peripheral module
		lvl * (1U << 6) |	// LVL	1: Detects based on the level.
		1 * (1U << 5) |		// HIEN	1: When LVL = 1: Detects a request when the signal is at the High level.
		reqd * (1U << 3) |	// REQD		Request Direction
		(id & 0x07) * (1U << 0) |		// SEL	0: CH0/CH8
		0;

	enum { dmarsshift = (id & 0x01) * 16 };
	DMAC23.DMARS = (DMAC23.DMARS & ~ (0x1FFul << dmarsshift)) |
		mid * (1U << (2 + dmarsshift)) |		// MID
		rid * (1U << (0 + dmarsshift)) |		// RID
		0;

    DMAC07.DCTRL_0_7 = (DMAC07.DCTRL_0_7 & ~ (/*(1U << 1) | */(1U << 0))) |
		//1 * (1U << 1) |		// LVINT	1: Level output
		1 * (1U << 0) |		// PR		1: Round robin mode
		0;

	arm_hardware_set_handler_realtime(DMAINT3_IRQn, r7s721_ssif1_txdma_fpgatx);

	DMAC3.CHCTRL_n = 1 * (1U << 3);		// SWRST
	DMAC3.CHCTRL_n = 1 * (1U << 17);	// CLRINTMSK
	DMAC3.CHCTRL_n = 1 * (1U << 0);		// SETEN
}

// FGA I2S INTERFACE #1
static void r7s721_ssif1_duplex_initialize_fpga(void)
{
	const uint_fast8_t master = R7S721_SSIF1_MASTER;
    /* ---- Supply clock to the SSIF(channel 1) ---- */
	CPG.STBCR11 &= ~ (1U << 4);	// Module Stop 114	- 0: Channel 1 of the serial sound interface runs.
	(void) CPG.STBCR11;			/* Dummy read */

	// I2S compatible
	// SCKP = 0, SWSP = 0, DEL = 0, CHNL = 00
	// System word length = data word length

	// Control Register (SSICR)
	SSIF1.SSICR = 
		R7S721_USE_AUDIO_CLK * (1u << 30) |		// CKS 1: AUDIO_CLK input 0: AUDIO_X1 input
		((WITHFPGAIF_FRAMEBITS / 64) - 1) * (1u << 22) |		// CHNL		00: Having one channel per system word (I2S complaint)
		6 * (1u << 19) |		// DWL 6: 32 bit
#if WITHFPGAIF_FRAMEBITS == 64
		3 * (1u << 16) |		// SWL 3: 32 bit, 6: 128 bit, 7: 256 bit
#elif WITHFPGAIF_FRAMEBITS == 256
		6 * (1u << 16) |		// SWL 3: 32 bit, 6: 128 bit, 7: 256 bit
#endif /*  */
		master * (1u << 15) |		// SCKD	1: Serial bit clock is output, master mode.
		master * (1u << 14) |		// SWSD	1: Serial word select is output, master mode.
		0 * (1u << 13) |		// SCKP	0: Данные на выходе меняются по спадающему фронту (I2S complaint)
		0 * (1u << 12) |		// SWSP	0: SSIWS is low for the 1st channel, high for the 2nd channel.  (I2S complaint)
		0 * (1u << 11) |		// SPDP 0: Padding bits are low.
		0 * (1u << 10) |		// SDTA
		0 * (1u << 9) |		// PDTA
#if WITHFPGAIF_FORMATI2S_PHILIPS
		0 * (1u << 8) |		// DEL	0: 1 clock cycle delay between SSIWS and SSIDATA
#else /* WITHFPGAIF_FORMATI2S_PHILIPS */
		1 * (1u << 8) |		// DEL	1: No delay between SSIWS and SSIDATA
#endif /* WITHFPGAIF_FORMATI2S_PHILIPS */
		master * R7S721_SSIF_CKDIV1 * (1u << 4) |		// CKDV	0000: AUDIOц/4: 12,288 -> 12,288 (48 kS, 128 bit, stereo)
		0;

	// FIFO Control Register (SSIFCR)
	SSIF1.SSIFCR = 
		2 * (1u << 6) |	// TTRG Transmit Data Trigger Number
		2 * (1u << 4) |	// RTRG Receive Data Trigger Number
		1 * (1u << 3) |		// TIE	Transmit Interrupt Enable
		1 * (1u << 2) |		// RIE	Receive Interrupt Enable
		//1 * (1u << 1) |		// TFRST Transmit FIFO Data Register Reset
		//1 * (1u << 0) |		// RFRST Receive FIFO Data Register Reset
		0;


	HARDWARE_SSIF1_INITIALIZE();	// Подключение синалалов периферийного блока к выводам процессора
}


static void r7s721_ssif1_duplex_enable_fpga(uint_fast8_t state)
{
	if (state != 0)
	{
		SSIF1.SSICR |= ((1u << 0) | (1u << 1)); // REN TEN
	}
	else
	{
		SSIF1.SSICR &= ~ ((1u << 0) | (1u << 1)); // REN TEN
	}
}

static const codechw_t fpgacodechw_ssif1_duplex_master =
{
	r7s721_ssif1_duplex_initialize_fpga,
	hardware_dummy_initialize,
	r7s721_ssif1_dmarx_initialize_fpga_rx,
	r7s721_ssif1_dmatx_initialize_fpga_tx,
	r7s721_ssif1_duplex_enable_fpga,
	hardware_dummy_enable,
	"fpgacodechw-ssif1-duplex-master"
};

#endif /* WITHSAI1HW */

#if WITHSAI2HW

// FPGA/spectrum channel
// DMA по приему SSIF2 - обработчик прерывания

static RAMFUNC_NONILINE void r7s721_ssif2_rxdma_WFMrx(void)
{
	//__DMB();
	DMAC4.CHCFG_n |= DMAC4_CHCFG_n_REN;	// REN bit
	// SR (bt 7)
	// Indicates the register set currently selected in register mode.
	// 0: Next0 Register Set
	// 1: Next1 Register Set
	const uint_fast8_t b = (DMAC4.CHSTAT_n & (1U << DMAC4_CHSTAT_n_SR_SHIFT)) != 0;	// SR
	if (b != 0)
	{
		processing_dmabuffer32rts192(DMAC4.N0DA_n);
		DMAC4.N0DA_n = dma_invalidate192rts(allocate_dmabuffer192rts());
	}
	else
	{
		processing_dmabuffer32rts192(DMAC4.N1DA_n);
		DMAC4.N1DA_n = dma_invalidate192rts(allocate_dmabuffer192rts());
	}
}

// FPGA/spectrum channel
// DMA по приёму SSIF2

static void r7s721_ssif2_dmarx_initialize_WFM(void)
{
	enum { id = 4 };	// 4: DMAC4
	// DMAC4
	/* Set Source Start Address */
    DMAC4.N0SA_n = (uintptr_t) & SSIF2.SSIFRDR;	// Fixed source address
    DMAC4.N1SA_n = (uintptr_t) & SSIF2.SSIFRDR;	// Fixed source address

    /* Set Destination Start Address */
	DMAC4.N0DA_n = dma_invalidate192rts(allocate_dmabuffer192rts());
	DMAC4.N1DA_n = dma_invalidate192rts(allocate_dmabuffer192rts());

    /* Set Transfer Size */
    DMAC4.N0TB_n = DMABUFFSIZE192RTS * sizeof (uint8_t);	// размер в байтах
    DMAC4.N1TB_n = DMABUFFSIZE192RTS * sizeof (uint8_t);	// размер в байтах

	// Values from Table 9.4 On-Chip Peripheral Module Requests
	// SSIRTI2 (receive data full)
	const uint_fast8_t mid = 0x3a;	
	const uint_fast8_t rid = 3;		
	const uint_fast8_t tm = 0;		
	const uint_fast8_t am = 2;		
	const uint_fast8_t lvl = 1;		
	const uint_fast8_t reqd = 0;

	DMAC4.CHCFG_n =
		0 * (1U << 31) |	// DMS	0: Register mode
		1 * (1U << 30) |	// REN	1: Continues DMA transfers.
		1 * (1U << 29) |	// RSW	1: Inverts RSEL automatically after a DMA transaction.
		0 * (1U << 28) |	// RSEL	0: Executes the Next0 Register Set
		0 * (1U << 27) |	// SBE	0: Stops the DMA transfer without sweeping the buffer (initial value).
		0 * (1U << 24) |	// DEM	0: Does not mask the DMA transfer end interrupt - прерывания каждый раз после TC
		tm * (1U << 22) |	// TM	0: Single transfer mode - берётся из Table 9.4
		0 * (1U << 21) |	// DAD	0: Increment destination address
		1 * (1U << 20) |	// SAD	1: Fixed source address
		2 * (1U << 16) |	// DDS	2: 32 bits (Destination Data Size)
		2 * (1U << 12) |	// SDS	2: 32 bits (Source Data Size)
		am * (1U << 8) |	// AM	1: ACK mode: Level mode (active until the transfer request from an on-chip peripheral module
		lvl * (1U << 6) |	// LVL	1: Detects based on the level.
		1 * (1U << 5) |		// HIEN	1: When LVL = 1: Detects a request when the signal is at the High level.
		reqd * (1U << 3) |	// REQD		Request Direction
		(id & 0x07) * (1U << 0) |		// SEL	0: CH0/CH8
		0;

	enum { dmarsshift = (id & 0x01) * 16 };
	DMAC45.DMARS = (DMAC45.DMARS & ~ (0x1FFul << dmarsshift)) |
		mid * (1U << (2 + dmarsshift)) |		// MID
		rid * (1U << (0 + dmarsshift)) |		// RID
		0;

    DMAC07.DCTRL_0_7 = (DMAC07.DCTRL_0_7 & ~ (/*(1U << 1) | */(1U << 0))) |
		//1 * (1U << 1) |		// LVINT	1: Level output
		1 * (1U << 0) |		// PR		1: Round robin mode
		0;

	arm_hardware_set_handler_realtime(DMAINT4_IRQn, r7s721_ssif2_rxdma_WFMrx);

	DMAC4.CHCTRL_n = 1 * (1U << 3);		// SWRST
	DMAC4.CHCTRL_n = 1 * (1U << 17);	// CLRINTMSK
	DMAC4.CHCTRL_n = 1 * (1U << 0);		// SETEN
}

// FGA I2S INTERFACE #2
// FPGA/spectrum channel
static void r7s721_ssif2_rx_initialize_WFM(void)
{
	const uint_fast8_t master = R7S721_SSIF2_MASTER;
    /* ---- Supply clock to the SSIF(channel 1) ---- */
	CPG.STBCR11 &= ~ (1U << 3);	// Module Stop 113	- 0: Channel 2 of the serial sound interface runs.
	(void) CPG.STBCR11;			/* Dummy read */

	// I2S compatible
	// SCKP = 0, SWSP = 0, DEL = 0, CHNL = 00
	// System word length = data word length

	// Control Register (SSICR)
	SSIF2.SSICR = 
		R7S721_USE_AUDIO_CLK * (1u << 30) |		// CKS 1: AUDIO_CLK input 0: AUDIO_X1 input
		((WITHFPGARTS_FRAMEBITS / 64) - 1) * (1u << 22) |		// CHNL		00: Having one channel per system word (I2S complaint)
		6 * (1u << 19) |		// DWL 6: 32 bit
#if WITHFPGARTS_FRAMEBITS == 64
		3 * (1u << 16) |		// SWL 3: 32 bit, 6: 128 bit, 7: 256 bit
#elif WITHFPGARTS_FRAMEBITS == 256
		6 * (1u << 16) |		// SWL 3: 32 bit, 6: 128 bit, 7: 256 bit
#endif /*  */
		master * (1u << 15) |		// SCKD	1: Serial bit clock is output, master mode.
		master * (1u << 14) |		// SWSD	1: Serial word select is output, master mode.
		0 * (1u << 13) |		// SCKP	0: Данные на выходе меняются по спадающему фронту (I2S complaint)
		0 * (1u << 12) |		// SWSP	0: SSIWS is low for the 1st channel, high for the 2nd channel.  (I2S complaint)
		0 * (1u << 11) |		// SPDP 0: Padding bits are low.
		0 * (1u << 10) |		// SDTA
		0 * (1u << 9) |		// PDTA
#if WITHFPGARTS_FORMATI2S_PHILIPS
		0 * (1u << 8) |		// DEL	0: 1 clock cycle delay between SSIWS and SSIDATA
#else /* WITHFPGAIF_FORMATI2S_PHILIPS */
		1 * (1u << 8) |		// DEL	1: No delay between SSIWS and SSIDATA
#endif /* WITHFPGAIF_FORMATI2S_PHILIPS */
		master * R7S721_SSIF_CKDIV1 * (1u << 4) |		// CKDV	0000: AUDIOц/4: 12,288 -> 12,288 (48 kS, 128 bit, stereo)
		0;

	// FIFO Control Register (SSIFCR)
	SSIF2.SSIFCR = 
		//2 * (1u << 6) |	// TTRG Transmit Data Trigger Number
		2 * (1u << 4) |	// RTRG Receive Data Trigger Number
		//1 * (1u << 3) |		// TIE	Transmit Interrupt Enable
		1 * (1u << 2) |		// RIE	Receive Interrupt Enable
		//1 * (1u << 1) |		// TFRST Transmit FIFO Data Register Reset
		//1 * (1u << 0) |		// RFRST Receive FIFO Data Register Reset
		0;


	HARDWARE_SSIF2_INITIALIZE();	// Подключение синалалов периферийного блока к выводам процессора
}

// FPGA/spectrum channel
static void r7s721_ssif2_rx_enable_WFM(uint_fast8_t state)
{
	if (state != 0)
	{
		SSIF2.SSICR |= (1u << 0); // REN
	}
	else
	{
		SSIF2.SSICR &= ~ (1u << 0); // REN
	}
}

static const codechw_t fpgaspectrumhw_ssif2_rx_master =
{
	r7s721_ssif2_rx_initialize_WFM,
	hardware_dummy_initialize,
	r7s721_ssif2_dmarx_initialize_WFM,
	hardware_dummy_initialize,
	r7s721_ssif2_rx_enable_WFM,
	hardware_dummy_enable,
	"spectrumhw-ssif2-rx-master"
};

#endif /* WITHSAI2HW */

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	static const codechw_t audiocodechw_xc7z =
	{
		xcz_ah_preinit,				// added...
		hardware_dummy_initialize,
		xcz_audio_rx_init,
		xcz_audio_tx_init,
		xcz_audio_rx_enable,
		xcz_audio_tx_enable,
		"ZYNQ audio codec"
	};

	static const codechw_t ifcodechw_xc7z =
	{
		hardware_dummy_initialize,	// added...
		hardware_dummy_initialize,
		xcz_if_rx_init,
		xcz_if_tx_init,
		xcz_if_rx_enable,
		xcz_if_tx_enable,
		"ZYNQ IF codec"
	};

#elif CPUSTYLE_STM32F || CPUSTYLE_STM32MP1
	// other CPUs
	static const codechw_t fpgaspectrumhw_rx_sai2 =
	{
		hardware_sai2_slave_duplex_initialize_WFM,	// added...
		hardware_dummy_initialize,
		DMA_SAI2_B_RX_initializeWFM,
		hardware_dummy_initialize,
		hardware_sai2_b_enable_WFM,
		hardware_dummy_enable,
		"sai2-fpga spectrum for WFM"
	};

#elif CPUSTYLE_T113 || CPUSTYLE_F133

	#warning Codecs not defined for CPUSTYLE_F133 || CPUSTYLE_F133

#else

	#warning Codecs not defined for this CPUSTYLE_XXX

#endif /* CPUSTYLE_STM32F */

#endif /* WITHINTEGRATEDDSP */

#if WITHCPUDACHW

void hardware_dac_initialize(void)		/* инициализация DAC на STM32F4xx */
{
	PRINTF(PSTR("hardware_dac_initialize start\n"));
#if CPUSTYLE_STM32H7XX

	RCC->APB1LENR |= RCC_APB1LENR_DAC12EN; //подать тактирование
	__DSB();

#elif CPUSTYLE_STM32F

	RCC->APB1ENR |= RCC_APB1ENR_DACEN; //подать тактирование
	__DSB();

#endif
	HARDWARE_DAC_INITIALIZE();	/* включить нужные каналы */
	PRINTF(PSTR("hardware_dac_initialize done\n"));
}
// вывод 12-битного значения на ЦАП - канал 1
void hardware_dac_ch1_setvalue(uint_fast16_t v)
{
	DAC1->DHR12R1 = v;
}

// вывод 12-битного значения на ЦАП - канал 2
void hardware_dac_ch2_setvalue(uint_fast16_t v)
{
	DAC1->DHR12R2 = v;
}

#else

void hardware_dac_initialize(void)		/* инициализация DAC на STM32F4xx */
{
}
// вывод 12-битного значения на ЦАП - канал 1
void hardware_dac_ch1_setvalue(uint_fast16_t v)
{
}
// вывод 12-битного значения на ЦАП - канал 2
void hardware_dac_ch2_setvalue(uint_fast16_t v)
{
}
#endif /* WITHCPUDACHW */

static const codechw_t audiocodechw_dummy =
{
	hardware_dummy_initialize,
	hardware_dummy_initialize,
	hardware_dummy_initialize,
	hardware_dummy_initialize,
	hardware_dummy_enable,
	hardware_dummy_enable,
	"dummy audio codec"
};

static const codechw_t fpgaiqhw_dummy =
{
	hardware_dummy_initialize,
	hardware_dummy_initialize,
	hardware_dummy_initialize,
	hardware_dummy_initialize,
	hardware_dummy_enable,
	hardware_dummy_enable,
	"dummy FPGA I/Q"
};

static const codechw_t fpgaspectrumhw_dummy =
{
	hardware_dummy_initialize,
	hardware_dummy_initialize,
	hardware_dummy_initialize,
	hardware_dummy_initialize,
	hardware_dummy_enable,
	hardware_dummy_enable,
	"dummy FPGA spectrum for WFM"
};

static const codechw_t * const channels [] =
{

#if WITHISBOOTLOADER || ! WITHINTEGRATEDDSP

		& audiocodechw_dummy,		// Интерфейс к НЧ кодеку
		& fpgaiqhw_dummy,			// Интерфейс к IF кодеку/FPGA
		& fpgaspectrumhw_dummy,		// Интерфейс к FPGA - широкополосный канал (WFM)

#elif LINUX_SUBSYSTEM

		& audiocodechw_dummy,		// Интерфейс к НЧ кодеку
		& fpgaiqhw_dummy,			// Интерфейс к IF кодеку/FPGA
		& fpgaspectrumhw_dummy,		// Интерфейс к FPGA - широкополосный канал (WFM)

#elif CPUSTYLE_R7S721

	#if WITHCODEC1_SSIF0_DUPLEX_MASTER
		& audiocodec_ssif0_duplex_master,				// Интерфейс к НЧ кодеку
	#endif /* WITHCODEC1_SSIF0_DUPLEX_MASTER */
	#if WITHFPGAIF_SSIF1_DUPLEX_MASTER
		& fpgacodechw_ssif1_duplex_master,			// Интерфейс к IF кодеку/FPGA
	#endif /* WITHFPGAIF_SSIF1_DUPLEX_MASTER */
	#if WITHFPGARTS_SSIF2_RX_MASTER
		& fpgaspectrumhw_ssif2_rx_master,			// Интерфейс к FPGA - широкополосный канал (WFM)
	#endif /* WITHFPGARTS_SSIF2_RX_MASTER */

#elif CPUSTYLE_STM32F4XX
		& audiocodechw_i2s2_i2s2ext_duplex_master,		// Интерфейс к НЧ кодеку
	#if WITHFPGAIF_SAI1_A_TX_B_RX_SLAVE
		& fpgacodechw_sai1_a_tx_b_rx_slave,				// Интерфейс к IF кодеку/FPGA
	#endif /* WITHFPGAIF_SAI1_A_TX_B_RX_SLAVE */
	#if WITHFPGARTS_SAI2_B_RX_SLAVE
		& fpgaspectrumhw_rx_sai2,					// Интерфейс к FPGA - широкополосный канал (WFM)
	#endif /* WITHFPGARTS_SAI2_B_RX_SLAVE */

#elif WITHINTEGRATEDDSP

	#if CPUSTYLE_XC7Z || CPUSTYLE_XCZU
		& audiocodechw_xc7z,				// Интерфейс к НЧ кодеку
		& ifcodechw_xc7z,					// Интерфейс к IF кодеку/FPGA
		//& fpgaspectrumhw_dummy,				// Интерфейс к FPGA - широкополосный канал (WFM)
	#endif
	#if WITHFPGAIF_SAI1_A_TX_B_RX_SLAVE	// (stm32mp157 9a)
		& fpgacodechw_sai1_a_tx_b_rx_slave,				// Интерфейс к IF кодеку/FPGA
	#endif /* WITHFPGAIF_SAI1_A_TX_B_RX_SLAVE */
	#if WITHFPGAIF_SAI2_A_TX_B_RX_SLAVE
		& fpgacodechw_sai2_a_tx_b_rx_slave,				// Интерфейс к IF кодеку/FPGA
	#endif /* WITHFPGAIF_SAI2_A_TX_B_RX_SLAVE */
	#if WITHFPGAIF_SAI2_A_TX_B_RX_MASTER	// (stm32mp157 9c)
		& fpgacodechw_sai2_a_tx_b_rx_master,				// Интерфейс к IF кодеку/FPGA
	#endif /* WITHFPGAIF_SAI2_A_TX_B_RX_MASTER */
	#if WITHCODEC1_I2S2_TX_SLAVE
		& audiocodechw_i2s2_tx_slave,					// Интерфейс к НЧ кодеку
	#endif /* WITHCODEC1_I2S2_TX_SLAVE */
	#if WITHCODEC1_I2S3_RX_SLAVE
		& audiocodechw_i2s3_rx_slave,					// Интерфейс к НЧ кодеку
	#endif /* WITHCODEC1_I2S3_RX_SLAVE */
	#if WITHFPGAIF_I2S1_DUPLEX_SLAVE
		& fpgacodechw_i2s1_duplex_slave,	// Интерфейс к FPGA
	#endif /* WITHFPGAIF_I2S1_DUPLEX_SLAVE */
	#if WITHCODEC1_I2S2_DUPLEX_SLAVE
		& audiocodechw_i2s2_duplex_slave,	// Интерфейс к НЧ кодеку
	#endif /* WITHCODEC1_I2S2_DUPLEX_SLAVE */
	#if WITHCODEC1_I2S2_DUPLEX_MASTER
		& audiocodechw_i2s2_duplex_master,	// Интерфейс к НЧ кодеку
	#endif /* WITHCODEC1_I2S2_DUPLEX_MASTER */
	#if WITHCODEC1_SAI2_A_TX_B_RX_MASTER
		& audiocodechw_sai2_a_tx_b_rx_master,	// Интерфейс к НЧ кодеку
	#endif /* WITHCODEC1_SAI2_A_TX_B_RX_MASTER */
	#if WITHFPGARTS_SAI2_B_RX_SLAVE
		& fpgaspectrumhw_rx_sai2,					// Интерфейс к FPGA - широкополосный канал (WFM)
	#endif /* WITHFPGARTS_SAI2_B_RX_SLAVE */
	#if WITHCODEC1_SAI1_A_TX_B_RX_MASTER 	/* Обмен с аудиокодеком через SAI1: SAI1_A - TX, SAI1_B - RX */
		& audiocodechw_sai1_a_tx_b_rx_master,	// Интерфейс к НЧ кодеку
	#endif /* WITHCODEC1_SAI1_A_TX_B_RX_MASTER */
	#if WITHCODEC1_SAI2_A_TX_B_RX_MASTER
		& audiocodechw_sai2_a_tx_b_rx_master,	// Интерфейс к НЧ кодеку
	#endif /* WITHCODEC1_SAI2_A_TX_B_RX_MASTER */
	#if WITHFPGAIF_SAI1_A_TX_B_RX_MASTER
		& fpgacodechw_sai1_a_tx_b_rx_master,				// Интерфейс к IF кодеку/FPGA
	#endif /* WITHFPGAIF_SAI1_A_TX_B_RX_MASTER */
	#if WITHCODEC1_I2S2_TX_SLAVE
		& audiocodechw_i2s2_tx_slave,					// Интерфейс к НЧ кодеку
	#endif /* WITHCODEC1_I2S2_TX_SLAVE */
	#if WITHCODEC1_I2S3_RX_SLAVE
		& audiocodechw_i2s3_rx_slave,					// Интерфейс к НЧ кодеку
	#endif /* WITHCODEC1_I2S3_RX_SLAVE */
	#if WITHCODEC1_I2S1_DUPLEX_SLAVE	// allwinner t113-s3 or F133
		& audiocodechw_i2s1_duplex_slave,					// Интерфейс к НЧ кодеку
	#endif /* WITHCODEC1_I2S1_DUPLEX_SLAVE */
	#if WITHFPGAIF_I2S2_DUPLEX_SLAVE	// allwinner t113-s3 or F133
		& fpgacodechw_i2s2_duplex_slave,					// Интерфейс к IF кодеку/FPGA
	#endif /* WITHFPGAIF_I2S2_DUPLEX_SLAVE */
	#if WITHCODEC1_I2S1_DUPLEX_MASTER	// allwinner t113-s3 or F133
		& audiocodechw_i2s1_duplex_master,					// Интерфейс к НЧ кодеку
	#endif /* WITHCODEC1_I2S1_DUPLEX_MASTER */
	#if WITHFPGAIF_I2S2_DUPLEX_MASTER	// allwinner t113-s3 or F133
		& fpgacodechw_i2s2_duplex_master,					// Интерфейс к IF кодеку/FPGA
	#endif /* WITHFPGAIF_I2S2_DUPLEX_MASTER */
	#if WITHFPGAIF_I2S0_DUPLEX_MASTER	// allwinner A64
		& fpgacodechw_i2s0_duplex_master,					// Интерфейс к IF кодеку/FPGA
	#endif /* WITHFPGAIF_I2S0_DUPLEX_MASTER */
	#if WITHFPGAIF_I2S0_DUPLEX_SLAVE	// allwinner A64
		& fpgacodechw_i2s0_duplex_slave,					// Интерфейс к IF кодеку/FPGA
	#endif /* WITHFPGAIF_I2S0_DUPLEX_SLAVE */
	#if WITHCODEC1_WHBLOCK_DUPLEX_MASTER	// allwinner t113-s3 or F133
		& audiocodechw_hwblock_duplex_master,					// Интерфейс к НЧ кодеку (встроенный в процессор)
	#endif /* WITHCODEC1_WHBLOCK_DUPLEX_MASTER */
		//& fpgaspectrumhw_rx_sai2,			// Интерфейс к FPGA - широкополосный канал (WFM)

#else
		& fpgaiqhw_dummy,					// Интерфейс к IF кодеку/FPGA

#endif
};

void hardware_channels_initialize(void)
{
	uint_fast8_t i;
	for (i = 0; i < ARRAY_SIZE(channels); ++ i)
	{
		const codechw_t * const p = channels [i];
		//
		PRINTF(PSTR("hardware_channels_initialize: %s\n"), p->label);
		p->initialize_rx();
		p->initialize_tx();
	}
}

void hardware_channels_enable(void)
{
	uint_fast8_t i;
	for (i = 0; i < ARRAY_SIZE(channels); ++ i)
	{
		const codechw_t * const p = channels [i];
		//
		PRINTF(PSTR("hardware_channels_enable: %s\n"), p->label);
		p->initializedma_rx();
		p->initializedma_tx();
		p->enable_rx(1);
		p->enable_tx(1);
	}
}
