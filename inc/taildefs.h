/* $Id$ */
#ifndef TAILDEFS_H_INCLUDED
#define TAILDEFS_H_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// В зависимости от типа платы - какие микросхемы применены в синтезаторе

#ifndef HARDWARE_H_INCLUDED
	#error PLEASE, DO NOT USE THIS FILE DIRECTLY. USE FILE "hardware.h" INSTEAD.
#endif

#if LINUX_SUBSYSTEM
	#include <src/linux/linux_subsystem.h>
#endif /* LINUX_SUBSYSTEM */

#define	SPISPEED400k	400000u	/* 400 kHz для низкоскоростных микросхем */

typedef enum
{
	SPIC_SPEED400k,
	SPIC_SPEED1M,	/* 1 MHz для XPT2046 */
	SPIC_SPEED4M,	/* 4 MHz для CS4272 */
	SPIC_SPEED10M,	/* 10 MHz для ILI9341 */
	SPIC_SPEED25M,	/* 25 MHz  */

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

#if WITHBOTTOMDBVAL
#define WITHBOTTOMDBDEFAULT WITHBOTTOMDBVAL
#else
#define WITHBOTTOMDBDEFAULT 130
#endif /* WITHBOTTOMDBVAL */

#if ! LINUX_SUBSYSTEM

typedef struct lclspinlock_tag {
	USBALIGN_BEGIN volatile uint8_t lock USBALIGN_END;
#if WITHDEBUG
	const char * file;
	int line;
	uint_fast8_t cpuid;
#endif /* WITHDEBUG */
} lclspinlock_t, LCLSPINLOCK_t;

#if WITHDEBUG
	#define LCLSPINLOCK_INIT { 0, "z", 0, 255 }
	#define LCLSPINLOCK_INITIALIZE(p) do { (p)->lock = 0; (p)->file = "n"; (p)->line = 0, (p)->cpuid = 255; } while (0)
#else /* WITHDEBUG */
	#define LCLSPINLOCK_INIT { 0, }
	#define LCLSPINLOCK_INITIALIZE(p) do { (p)->lock = 0; } while (0)
#endif /* WITHDEBUG */

typedef struct irqlspinlock_tag
{
	IRQL_t irql;
	LCLSPINLOCK_t lock;
} IRQLSPINLOCK_t;

//#define IRQLSPINLOCK_INIT(irqlv) { (irqlv), LCLSPINLOCK_INIT }
#define IRQLSPINLOCK_INITIALIZE(p, oldIrqlv) do { LCLSPINLOCK_INITIALIZE(& (p)->lock); (p)->irql = (oldIrqlv); } while (0)
#else /* ! LINUX_SUBSYSTEM */

/* Linux targets: No any hardware IRQ control */

typedef struct irqlspinlock_tag
{
	int irql;
	int lock;
} IRQLSPINLOCK_t;

//#define IRQLSPINLOCK_INIT(irqlv) { (irqlv), LCLSPINLOCK_INIT }
#define IRQLSPINLOCK_INITIALIZE(p, oldIrqlv) do {} while (0)

#endif /* ! LINUX_SUBSYSTEM */

/* newIRQL - уровень приоритета, прерывания с которым и ниже которого требуется запретить */
/* Работа с текущим ядром */
void RiseIrql_DEBUG(IRQL_t newIRQL, IRQL_t * oldIrql, const char * file, int line);
#define RiseIrql(newIRQL, oldIrqlv2) RiseIrql_DEBUG((newIRQL), (oldIrqlv2), __FILE__, __LINE__)

/* Работа с текущим ядром */
void LowerIrql(IRQL_t newIRQL);

#if WITHSMPSYSTEM
	/* Пока привязка процессора обрабатывающего прерывание по приоритету. */
	#define TARGETCPU_SYSTEM (1u << 0)		// CPU #0
	#define TARGETCPU_RT 	(1u << 1)		// CPU #1
	#define TARGETCPU_OVRT 	(1u << 0)		// CPU #0
	#define TARGETCPU_CPU0 (1u << 0)		// CPU #0
	#define TARGETCPU_CPU1 (1u << 1)		// CPU #1

	#define LCLSPIN_LOCK(p) do { lclspin_lock(p, __FILE__, __LINE__); } while (0)
	#define LCLSPIN_UNLOCK(p) do { lclspin_unlock(p); } while (0)

	void lclspin_lock(lclspinlock_t * __restrict lock, const char * file, int line);
	void lclspin_unlock(lclspinlock_t * __restrict lock);

#else /* WITHSMPSYSTEM */
	/* Единственный процесор. */
	#define TARGETCPU_SYSTEM (1u << 0)		// CPU #0
	#define TARGETCPU_RT (1u << 0)			// CPU #0
	#define TARGETCPU_OVRT (1u << 0)		// CPU #0
	#define TARGETCPU_CPU0 (1u << 0)		// CPU #0
	#define TARGETCPU_CPU1 (1u << 0)		// CPU #0

	#define LCLSPIN_LOCK(p) do { (void) p; } while (0)
	#define LCLSPIN_UNLOCK(p) do { (void) p; } while (0)
	#define SPIN_LOCK2(p, f, l) do { (void) p; } while (0)
	#define SPIN_UNLOCK2(p) do { (void) p; } while (0)

#endif /* WITHSMPSYSTEM */

#if ! LINUX_SUBSYSTEM

	/* Захват spinlock с установкой требуемого IRQL и сохранением ранее установленного */
	#define IRQLSPIN_LOCK(p, oldIrql) do { RiseIrql((p)->irql, (oldIrql)); LCLSPIN_LOCK(& (p)->lock); } while (0)
	#define IRQLSPIN_UNLOCK(p, oldIrql) do { LCLSPIN_UNLOCK(& (p)->lock); LowerIrql(oldIrql); } while (0)

#else  /* ! LINUX_SUBSYSTEM */

	/* Linux targets: No any hardware IRQ control */
	#define IRQLSPIN_LOCK(p, oldIrql) do { } while (0)
	#define IRQLSPIN_UNLOCK(p, oldIrql) do { } while (0)

#endif  /* ! LINUX_SUBSYSTEM */


	#define DPC_IRQL	IRQL_REALTIME

	typedef void (* udpcfn_t)(void *);
	typedef struct dpcobj_tag
	{
		LIST_ENTRY item;
		uint8_t flag;
		void (* fn)(void *);
		void * ctx;
	} dpcobj_t;

	void dpcobj_initialize(dpcobj_t * dp, udpcfn_t func, void * arg);
	void board_dpc_processing(void);
	uint_fast8_t board_dpc_addentry(dpcobj_t * dp);	// Запрос периодического вызова user-mode функциии (0 - уже помещён в список)
	uint_fast8_t board_dpc_call(dpcobj_t * dp); // Запрос отложенного вызова user-mode функции (0 - ранее запрошенный вызов еще не выполнился)

	void board_dpc_initialize(void);	/* инициализация списка user-mode опросных функций */

#if LINUX_SUBSYSTEM
	#define RAMFUNC_NONILINE //__attribute__((__section__(".itcm"), noinline))
	#define RAMFUNC			//__attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//__attribute__((section(".framebuff")))	/* память доступная лоя DMA обмена */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM		//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP 		//__attribute__((used, section(".heap"), aligned(64))) // memory used as heap zone
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif CPUSTYLE_R7S721
	#define RAMFUNC_NONILINE // __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))
	#define RAMNOINIT_D1	__attribute__((section(".noinit"))) /* память доступная лоя DMA обмена */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(64))) // memory used as heap zone
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif CPUSTYLE_STM32MP1
	#define RAMFUNC_NONILINE ////__attribute__((__section__(".itcm"), noinline))
	#define RAMFUNC			 ////__attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	__attribute__((section(".framebuff")))	/* память доступная лоя DMA обмена */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			////__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM		////__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA		//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(64))) // memory used as heap zone
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif CPUSTYLE_A64
	#define RAMFUNC_NONILINE ////__attribute__((__section__(".itcm"), noinline))
	#define RAMFUNC			 ////__attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//////__attribute__((section(".framebuff")))	/* память доступная лоя DMA обмена */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			////__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM		////__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA		//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(64))) // memory used as heap zone
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif CPUSTYLE_T507
	#define RAMFUNC_NONILINE ////__attribute__((__section__(".itcm"), noinline))
	#define RAMFUNC			 ////__attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//////__attribute__((section(".framebuff")))	/* память доступная лоя DMA обмена */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			////__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM		////__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA		//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(64))) // memory used as heap zone
	#define RAMNC __attribute__ ((section(".ramnc"), used))

#elif CPUSTYLE_T113
	#define RAMFUNC_NONILINE ////__attribute__((__section__(".itcm"), noinline))
	#define RAMFUNC			__attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//////__attribute__((section(".framebuff")))	/* память доступная лоя DMA обмена */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			////__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM		////__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA		//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(64))) // memory used as heap zone
	#define RAMNC __attribute__ ((section(".ramnc"), used))

#elif CPUSTYLE_VM14
	#define RAMFUNC_NONILINE ////__attribute__((__section__(".itcm"), noinline))
	#define RAMFUNC			 ////__attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//////__attribute__((section(".framebuff")))	/* память доступная лоя DMA обмена */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			////__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM		////__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA		//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(64))) // memory used as heap zone
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif CPUSTYLE_F133
	#define RAMFUNC_NONILINE ////__attribute__((__section__(".itcm"), noinline))
	#define RAMFUNC			 ////__attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//////__attribute__((section(".framebuff")))	/* память доступная лоя DMA обмена */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			////__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM		////__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA		//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(64))) // memory used as heap zone
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif CPUSTYLE_XC7Z
	#define RAMFUNC_NONILINE ////__attribute__((__section__(".itcm"), noinline))
	#define RAMFUNC			 ////__attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	__attribute__((section(".framebuff")))	/* память доступная лоя DMA обмена */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			////__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM		////__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA		//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(64))) // memory used as heap zone
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif (CPUSTYLE_STM32H7XX)
	//	ITCMRAM (rwx) : ORIGIN = 0x00000000, LENGTH = 64K	/* ITCMRAM-RAM */
	//	FLASH (rx)    : ORIGIN = 0x08000000, LENGTH = 2048K	/* FLASH on AXIM interface */
	//	DTCMRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 128K	/* DTCMRAM Data DTCMRAM-RAM */
	//	RAM_D1 (xrw)  : ORIGIN = 0x24000000, LENGTH = 512K	/* AXI SRAM - LTDC frame buffer */
	//	RAM_D2 (xrw)  : ORIGIN = 0x30000000, LENGTH = 288K	/* SRAM1, SRAM2, SRAM3 */
	//	RAM_D3 (xrw)  : ORIGIN = 0x38000000, LENGTH = 64K	/* SRAM4 */
	#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
	#define RAMFUNC_NONILINE __attribute__((noinline,__section__(".itcm")))  // удаление управления noinline добавило 2-3 процента быстродействия __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 __attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 - память доступная лоя DMA обмена */
	#define RAM_D1			__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			__attribute__((section(".ram_d2"))) /* размещение в памяти SRAM_D2 */
	#define RAM_D3			__attribute__((section(".ram_d3"))) /* размещение в памяти SRAM_D3 */
	#define RAMFRAMEBUFF	__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM		__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA		__attribute__((section(".ram_d1"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif (CPUSTYLE_STM32F7XX) && WITHSDRAMHW
//	#pragma name .data .sdramdata
//	#pragma name .bss .sdrambss
	#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
	#define RAMFUNC_NONILINE __attribute__((noinline,__section__(".itcm")))  // удаление управления noinline добавило 2-3 процента быстродействия __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 __attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".sdrambss"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA		//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			__attribute__((section(".sdramdata"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#define RAMLOW	__attribute__((section(".ram")))	// размещение во внутренней мамяти МК
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif (CPUSTYLE_STM32F7XX)
	#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
	#define RAMFUNC_NONILINE __attribute__((noinline,__section__(".itcm")))  // удаление управления noinline добавило 2-3 процента быстродействия __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 __attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".sdram"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA		//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".sdram"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif CPUSTYLE_STM32F4XX && (defined (STM32F429xx) || defined(STM32F407xx))
	#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
	#define RAMFUNC_NONILINE  //__attribute__((__section__(".itcm"), noinline))
	#define RAMFUNC			 // __attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM			__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif CPUSTYLE_STM32F4XX
	#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
	#define RAMFUNC_NONILINE // __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))
	#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif  CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA

	#define FLASHMEM __flash
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP //__attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif CPUSTYLE_TMS320F2833X

	#define FLASHMEM //__flash
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP //__attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#define RAMFUNC_NONILINE // __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif CPUSTYLE_UBLAZE

	#define FLASHMEM //__flash
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP //__attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#define RAMFUNC_NONILINE // __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif CPUSTYLE_STM32L0XX
	#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
	#define RAMFUNC_NONILINE //__attribute__((noinline,__section__(".itcm")))  // удаление управления noinline добавило 2-3 процента быстродействия __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 //__attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#elif CPUSTYLE_ARM && 0

	#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
	#define RAMFUNC_NONILINE __attribute__((noinline,__section__(".itcm")))  // удаление управления noinline добавило 2-3 процента быстродействия __attribute__((__section__(".ramfunc"), noinline))
	#define RAMFUNC			 __attribute__((__section__(".itcm")))
	#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D1			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D2			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D1 */
	#define RAM_D3			//__attribute__((section(".bss"))) /* размещение в памяти SRAM_D2 */
	#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
	#define RAMDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
	#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIGDTCM_MDMA	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
	#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
	#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#define RAMNC //__attribute__ ((section(".ramnc"), used))

#else

	#error Undefined CPUSTYLE_xxxx

#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TAILDEFS_H_INCLUDED */
