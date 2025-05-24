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

#if WITHSPIDEV && LINUX_SUBSYSTEM

#include <linux/spi/spidev.h>

typedef enum
{
	SPIC_MODE0 = SPI_MODE_0,
	SPIC_MODE1 = SPI_MODE_1,
	SPIC_MODE2 = SPI_MODE_2,
	SPIC_MODE3 = SPI_MODE_3,
	//
	SPIC_MODES_COUNT = 4
} spi_modes_t;

typedef enum
{
	SPIC_SPEED400k 	= 400000,	/* 400 kHz для MCP3208, DS1305 */
	SPIC_SPEED1M 	= 1000000,	/* 1 MHz для XPT2046 */
	SPIC_SPEED4M 	= 4000000,	/* 4 MHz для CS4272 */
	SPIC_SPEED10M 	= 10000000,	/* 10 MHz для ILI9341 */
	SPIC_SPEEDFAST 	= 12000000,
	SPIC_SPEED25M 	= 25000000,	/* 25 MHz  */
	SPIC_SPEEDUFAST = 10000000,
} spi_speeds_t;

#include <src/linux/linux_subsystem.h>

#else /* WITHSPIDEV */

typedef enum
{
	SPIC_MODE0,
	SPIC_MODE1,
	SPIC_MODE2,
	SPIC_MODE3,
	//
	SPIC_MODES_COUNT
} spi_modes_t;

#define	SPISPEED400k	400000u	/* 400 kHz для низкоскоростных микросхем */

typedef enum
{
	//SPIC_SPEED100k,	/* 100 kHz  */
	SPIC_SPEED400k,	/* 400 kHz для MCP3208, DS1305 */
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

#endif /* WITHSPIDEV && LINUX_SUBSYSTEM */

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

#if ! LINUX_SUBSYSTEM

typedef struct lclspinlock_tag {
	ALIGNX_BEGIN volatile uint8_t lock ALIGNX_END;
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
	LCLSPINLOCK_t lock;
} IRQLSPINLOCK_t;

#define IRQLSPINLOCK_INIT { LCLSPINLOCK_INIT }
#define IRQLSPINLOCK_INITIALIZE(p) do { LCLSPINLOCK_INITIALIZE(& (p)->lock); } while (0)

#else /* ! LINUX_SUBSYSTEM */

/* Linux targets: No any hardware IRQ control */

#include <src/linux/linux_subsystem.h>

#define IRQLSPINLOCK_t lclspinlock_t

extern pthread_mutex_t linux_md;	/* added by mgs */
#define IRQLSPINLOCK_INIT PTHREAD_MUTEX_INITIALIZER
#define IRQLSPINLOCK_INITIALIZE(p) do { LCLSPINLOCK_INITIALIZE(p); } while (0)

#endif /* ! LINUX_SUBSYSTEM */

/* newIRQL - уровень приоритета, прерывания с которым и ниже которого требуется запретить */
/* Работа с текущим ядром */
void RiseIrql_DEBUG(IRQL_t newIRQL, IRQL_t * oldIrql, const char * file, int line);
#define RiseIrql(newIRQL, oldIrqlv2) do { RiseIrql_DEBUG((newIRQL), (oldIrqlv2), __FILE__, __LINE__); } while (0)

/* Работа с текущим ядром */
void LowerIrql_DEBUG(IRQL_t newIRQL, const char * file, int line);
#define LowerIrql(newIRQL) do { LowerIrql_DEBUG((newIRQL), __FILE__, __LINE__); } while (0)

void InitializeIrql(IRQL_t newIRQL);

#if WITHSMPSYSTEM
	/* Пока привязка процессора обрабатывающего прерывание по приоритету. */
	#define TARGETCPU_SYSTEM (1u << 0)		// CPU #0
	#define TARGETCPU_RT 	(1u << 1)		// CPU #1
	#define TARGETCPU_RT2	(1u << 2)		// CPU #2
	#define TARGETCPU_RT3 	(1u << 3)		// CPU #3
	#define TARGETCPU_OVRT 	(1u << 0)		// CPU #0
	#define TARGETCPU_CPU0 (1u << 0)		// CPU #0
	#define TARGETCPU_CPU1 (1u << 1)		// CPU #1

	void lclspin_lock(lclspinlock_t * __restrict lock, const char * file, int line);
	void lclspin_unlock(lclspinlock_t * __restrict lock);
	void lclspin_enable(void);	// Allwinner H3 - может работать с блокировками только после включения MMU

	#define LCLSPIN_LOCK(p) do { lclspin_lock(p, __FILE__, __LINE__); } while (0)
	#define LCLSPIN_UNLOCK(p) do { lclspin_unlock(p); } while (0)

#else /* WITHSMPSYSTEM */
	/* Единственный процесор. */
	#define TARGETCPU_SYSTEM (1u << 0)		// CPU #0
	#define TARGETCPU_RT (1u << 0)			// CPU #0
	#define TARGETCPU_RT2 (1u << 0)			// CPU #0
	#define TARGETCPU_RT3 (1u << 0)			// CPU #0
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
	#define IRQLSPIN_LOCK(p, oldIrql, newirql) do { RiseIrql((newirql), (oldIrql)); LCLSPIN_LOCK(& (p)->lock); } while (0)
	#define IRQLSPIN_UNLOCK(p, oldIrql) do { LCLSPIN_UNLOCK(& (p)->lock); LowerIrql(oldIrql); } while (0)

#else  /* ! LINUX_SUBSYSTEM */

	/* Linux targets: No any hardware IRQ control */
	#define IRQLSPIN_LOCK(p, oldIrql, newirql) do { LCLSPIN_LOCK(p); } while (0)
	#define IRQLSPIN_UNLOCK(p, oldIrql) do { LCLSPIN_UNLOCK(p); } while (0)

#endif  /* ! LINUX_SUBSYSTEM */

	#define USBSYS_IRQL IRQL_SYSTEM
	#define CATSYS_IRQL IRQL_SYSTEM
	#define LFMSYS_IRQL IRQL_REALTIME
	#define DPCSYS_IRQL	IRQL_REALTIME
	#define TICKER_IRQL IRQL_SYSTEM
	#define ELKEY_IRQL 	IRQL_OVERREALTIME

	typedef void (* udpcfn_t)(void *);
	typedef struct dpcobj_tag
	{
		void * tag1;
		LIST_ENTRY item;
		uint8_t coreid;	// в какой список включили - 0..HARDWARE_NCORES-1
		uint8_t flag;
		udpcfn_t fn;
		void * ctx;
		uint8_t delflag;	// помечена для удаления
		void * tag2;
	} dpcobj_t;

	void dpcobj_initialize(dpcobj_t * dp, udpcfn_t func, void * arg);
	void board_dpc_processing(void);	// user-mode функция обработки списков запросов dpc на текущем процессоре
	uint_fast8_t board_dpc_coreid(void);	// получить core id текушего потока
	uint_fast8_t board_dpc_addentry(dpcobj_t * dp, uint_fast8_t coreid);	// Запрос периодического вызова user-mode функциии (0 - уже помещён в список)
	uint_fast8_t board_dpc_call(dpcobj_t * dp, uint_fast8_t coreid); // Запрос отложенного вызова user-mode функции (0 - ранее запрошенный вызов еще не выполнился)
	uint_fast8_t board_dpc_delentry(dpcobj_t * dp);	// Удаление периодического вызова

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
	#define RAMNC //__attribute__((section(".ramnc")))

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
	#define RAMNC //__attribute__((section(".ramnc")))

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
	#define RAMNC __attribute__((section(".ramnc")))

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
	#define RAMNC __attribute__((section(".ramnc")))

#elif CPUSTYLE_T507 || CPUSTYLE_H616
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
	#define RAMNC __attribute__((section(".ramnc")))

#elif CPUSTYLE_A133
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
	#define RAMNC __attribute__((section(".ramnc")))

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
	#define RAMNC __attribute__((section(".ramnc")))

#elif CPUSTYLE_H3
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
	#define RAMNC __attribute__((section(".ramnc")))

#elif CPUSTYLE_V3S
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
	#define RAMNC __attribute__((section(".ramnc")))

#elif CPUSTYLE_VM14
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
	#define RAMNC __attribute__((section(".ramnc")))

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
	#define RAMNC __attribute__((section(".ramnc")))

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
	#define RAMNC //__attribute__((section(".ramnc")))

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
	#define RAMNC //__attribute__((section(".ramnc")))

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
	#define RAMNC //__attribute__((section(".ramnc")))

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
	#define RAMNC //__attribute__((section(".ramnc")))

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
	#define RAMNC //__attribute__((section(".ramnc")))

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
	#define RAMNC //__attribute__((section(".ramnc")))

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
	#define RAMNC //__attribute__((section(".ramnc")))

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
	#define RAMNC //__attribute__((section(".ramnc")))

#elif CPUSTYLE_RK356X
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
	#define RAMNC __attribute__((section(".ramnc")))


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
	#define RAMNC //__attribute__((section(".ramnc")))

#else

	#error Undefined CPUSTYLE_xxxx

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TAILDEFS_H_INCLUDED */
