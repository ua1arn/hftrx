/* $Id$ */
#ifndef TAILDEFS_H_INCLUDED
#define TAILDEFS_H_INCLUDED

// В зависимости от типа платы - какие микросхемы применены в синтезаторе

#ifndef HARDWARE_H_INCLUDED
	#error PLEASE, DO NOT USE THIS FILE DIRECTLY. USE FILE "hardware.h" INSTEAD.
#endif

#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

typedef enum
{
#if (SPISPEED400k) || defined (SPISPEED100k)
	SPIC_SPEED100k,
	SPIC_SPEED400k,
	SPIC_SPEED4M,	/* 4 MHz для CS4272 */
	SPIC_SPEED10M,	/* 10 MHz для ILI9341 */
	SPIC_SPEED25M,	/* 25 MHz  */
#endif /* (SPISPEED400k) || defined (SPISPEED100k) */
	SPIC_SPEEDFAST,
	SPIC_SPEEDUFAST,	// Скорость для загрузки FPGA
#if WITHUSESDCARD
	SPIC_SPEEDSDCARD,	// Переключаемая скоростть - 400 кГц или требуемая для SD карты
#endif /* WITHUSESDCARD */
	//
	SPIC_SPEEDS_COUNT
} spi_speeds_t;

#define MULTIVFO ((HYBRID_NVFOS > 1) && (LO1MODE_HYBRID || LO1MODE_FIXSCALE))

#define LO1DIVIDEVCO (HYBRID_OCTAVE_80_160 || HYBRID_OCTAVE_128_256 || FIXSCALE_48M0_X1_DIV256)

#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
	#define LO1MODE_DIRECT	1
	#define SYNTH_R1 1
	#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	#undef DDS1_CLK_MUL
	#define DDS1_CLK_MUL	1
	#undef DDS1_CLK_DIV
	#define DDS1_CLK_DIV	1
#endif

#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI5351A)
	#define LO1MODE_DIRECT	1
	#define SYNTH_R1 1
	#define PLL1_FRACTIONAL_LENGTH	10	/* Si5351a: lower 10 bits is a fractional part */
	#undef DDS1_CLK_MUL
	#define DDS1_CLK_MUL	1
	#undef DDS1_CLK_DIV
	#define DDS1_CLK_DIV	1
	#undef DDS2_CLK_MUL
	#define DDS2_CLK_MUL	1
	#undef DDS2_CLK_DIV
	#define DDS2_CLK_DIV	1
#endif

#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_NONE)
	#define	PLL1_FRACTIONAL_LENGTH 0
	#define SYNTH_R1	1
	#undef DDS1_CLK_MUL
	#define DDS1_CLK_MUL	1
	#undef DDS1_CLK_DIV
	#define DDS1_CLK_DIV	1
#endif

#if defined (SPI_A2) && defined (SPI_A1) && defined (SPI_A0)
	// Три вывода формируют адрес
	// значения параметра target для выбора получателей на шите SPI
	#define SPI_CSEL0 (0 * SPI_A2 | 0 * SPI_A1 | 0 * SPI_A0)
	#define SPI_CSEL1 (0 * SPI_A2 | 0 * SPI_A1 | 1 * SPI_A0)
	#define SPI_CSEL2 (0 * SPI_A2 | 1 * SPI_A1 | 0 * SPI_A0)
	#define SPI_CSEL3 (0 * SPI_A2 | 1 * SPI_A1 | 1 * SPI_A0)
	#define SPI_CSEL4 (1 * SPI_A2 | 0 * SPI_A1 | 0 * SPI_A0)
	#define SPI_CSEL5 (1 * SPI_A2 | 0 * SPI_A1 | 1 * SPI_A0)
	#define SPI_CSEL6 (1 * SPI_A2 | 1 * SPI_A1 | 0 * SPI_A0)
	#define SPI_CSEL7 (1 * SPI_A2 | 1 * SPI_A1 | 1 * SPI_A0)

	#define SPI_ADDRESS_BITS (SPI_A2 | SPI_A1 | SPI_A0)	// маска, включающая все адресные биты

#elif defined (SPI_A1) && defined (SPI_A0)
	// Два вывода формируют адрес
	// значения параметра target для выбора получателей на шите SPI
	#define SPI_CSEL0 (0 * SPI_A1 | 0 * SPI_A0)
	#define SPI_CSEL1 (0 * SPI_A1 | 1 * SPI_A0)
	#define SPI_CSEL2 (1 * SPI_A1 | 0 * SPI_A0)
	#define SPI_CSEL3 (1 * SPI_A1 | 1 * SPI_A0)

	#define SPI_ADDRESS_BITS (SPI_A1 | SPI_A0)	// маска, включающая все адресные биты


#elif defined (SPI_A0)
	// Один вывод формируют адрес
	// значения параметра target для выбора получателей на шите SPI
	#define SPI_CSEL0 (0 * SPI_A0)
	#define SPI_CSEL1 (1 * SPI_A0)

	#define SPI_ADDRESS_BITS (SPI_A1 | SPI_A0)	// маска, включающая все адресные биты

#else
	// нет дешифратора адреса - прямое управление сигналами CS имеющихся SPI устройств.
#endif



typedef struct spinlock_tag {
	volatile uint32_t lock;
#if WITHDEBUG
	const char * file;
	int line;
#endif /* WITHDEBUG */
} spinlock_t;

#define SPINLOCK_t spinlock_t
#define SPINLOCK_INIT { 0 }
#define SPINLOCK_INITIALIZE(p) do { (p)->lock = 0; } while (0)

#if WITHSMPSYSTEM
	/* Пока привязка процессора обрабатывающего прерывание по приоритету. */
	#define TARGETCPU_SYSTEM (1u << 0)		// CPU #0
	#define TARGETCPU_RT 	(1u << 1)		// CPU #1
	#define TARGETCPU_OVRT 	(1u << 0)		// CPU #0

	#define SPIN_LOCK(p) do { spin_lock(p, __FILE__, __LINE__); } while (0)
	#define SPIN_UNLOCK(p) do { spin_unlock(p); } while (0)

//#if 0
//	#define SPIN_LOCK2(p, f, l) do { spin_lock2(p, (f), (l)); } while (0)
//	#define SPIN_UNLOCK2(p) do { spin_unlock(p); } while (0)
//#else
//	#define SPIN_LOCK2(p, f, l) do { } while (0)
//	#define SPIN_UNLOCK2(p) do { } while (0)
//#endif

	void spin_lock(volatile spinlock_t * lock, const char * file, int line);
	void spin_lock2(volatile spinlock_t * lock, const char * file, int line);
	void spin_unlock(volatile spinlock_t * lock);

#else /* WITHSMPSYSTEM */
	/* Единственный процесор. */
	#define TARGETCPU_SYSTEM (1u << 0)		// CPU #0
	#define TARGETCPU_RT (1u << 0)			// CPU #0
	#define TARGETCPU_OVRT (1u << 0)		// CPU #0

	#define SPIN_LOCK(p) do { (void) p; } while (0)
	#define SPIN_UNLOCK(p) do { (void) p; } while (0)
	#define SPIN_LOCK2(p, f, l) do { (void) p; } while (0)
	#define SPIN_UNLOCK2(p) do { (void) p; } while (0)

#endif /* WITHSMPSYSTEM */

#if CPUSTYLE_R7S721
	#define FLASHMEMINIT	__attribute__((section(".initdata"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define FLASHMEMINITFUNC	__attribute__((section(".initfunc"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define RAMFUNC_NONILINE // __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))
	#define RAMNOINIT_D1	__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(64))) // memory used as heap zone
#elif CPUSTYLE_STM32MP1
	#define FLASHMEMINIT	//__attribute__((section(".initdata"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define FLASHMEMINITFUNC//	__attribute__((section(".initfunc"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define RAMFUNC_NONILINE __attribute__((__section__(".itcm"), noinline))
	#define RAMFUNC			 __attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM		__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA		//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(64))) // memory used as heap zone
#elif (CPUSTYLE_STM32H7XX)
	#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
	#define FLASHMEMINIT	__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define FLASHMEMINITFUNC	__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define RAMFUNC_NONILINE __attribute__((noinline,__section__(".itcm")))  // удаление управления noinline добавило 2-3 процента быстродействия __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 __attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM		__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA		__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
#elif (CPUSTYLE_STM32F7XX) && WITHSDRAMHW
//	#pragma name .data .sdramdata
//	#pragma name .bss .sdrambss
	#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
	#define FLASHMEMINIT	__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define FLASHMEMINITFUNC	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define RAMFUNC_NONILINE __attribute__((noinline,__section__(".itcm")))  // удаление управления noinline добавило 2-3 процента быстродействия __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 __attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".sdrabss"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA		//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			__attribute__((section(".sdramdata"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
#elif (CPUSTYLE_STM32F7XX)
	#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
	#define FLASHMEMINIT	__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define FLASHMEMINITFUNC	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define RAMFUNC_NONILINE __attribute__((noinline,__section__(".itcm")))  // удаление управления noinline добавило 2-3 процента быстродействия __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 __attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".sdram"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA		//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".sdram"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
#elif CPUSTYLE_STM32F4XX && (defined (STM32F429xx) || defined(STM32F407xx))
	#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
	#define FLASHMEMINIT	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define FLASHMEMINITFUNC	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define RAMFUNC_NONILINE  //__attribute__((__section__(".itcm"), noinline))
	#define RAMFUNC			 // __attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
#elif CPUSTYLE_STM32F4XX
	#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
	#define FLASHMEMINIT	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define FLASHMEMINITFUNC	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define RAMFUNC_NONILINE // __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))
	#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
#elif CPUSTYLE_ARM
	#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
	#define FLASHMEMINIT	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define FLASHMEMINITFUNC	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define RAMFUNC_NONILINE __attribute__((noinline,__section__(".itcm")))  // удаление управления noinline добавило 2-3 процента быстродействия __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 __attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
#endif

#endif /* TAILDEFS_H_INCLUDED */
