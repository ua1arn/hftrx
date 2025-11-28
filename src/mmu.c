/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints

//#define MMUUSE4KPAGES 1

#if CPUSTYLE_ARM_CM7

// Сейчас в эту память будем читать по DMA
// Убрать копию этой области из кэша
// Используется только в startup
void dcache_invalidate(uintptr_t base, int_fast32_t dsize)
{
	//ASSERT((base % 32) == 0);		// при работе с BACKUP SRAM невыровненно
	SCB_InvalidateDCache_by_Addr((void *) base, dsize);	// DCIMVAC register used.
}

// Сейчас эта память будет записываться по DMA куда-то
// Записать содержимое кэша данных в память
void dcache_clean(uintptr_t base, int_fast32_t dsize)
{
	//ASSERT((base % 32) == 0);		// при работе с BACKUP SRAM невыровненно
	SCB_CleanDCache_by_Addr((void *) base, dsize);	// DCCMVAC register used.
}

// Записать содержимое кэша данных в память
// применяется после начальной инициализации среды выполнния
void dcache_clean_all(void)
{
	SCB_CleanDCache();	// DCCMVAC register used.
}

// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
// Записать содержимое кэша данных в память
// Убрать копию этой области из кэша
void dcache_clean_invalidate(uintptr_t base, int_fast32_t dsize)
{
	//ASSERT((base % 32) == 0);		// при работе с BACKUP SRAM невыровненно
	SCB_CleanInvalidateDCache_by_Addr((void *) base, dsize);	// DCCIMVAC register used.
}

int_fast32_t dcache_rowsize(void)
{
	return DCACHEROWSIZE;
}


int_fast32_t icache_rowsize(void)
{
	return ICACHEROWSIZE;
}

#elif ((__CORTEX_A != 0) || CPUSTYLE_ARM9)

//	MVA
//	For more information about the possible meaning when the table shows that an MVA is required
// 	see Terms used in describing the maintenance operations on page B2-1272.
// 	When the data is stated to be an MVA, it does not have to be cache line aligned.

void L1_CleanDCache_by_Addr(void * addr, int32_t op_size)
{
	if (op_size > 0)
	{
		//int32_t op_size = dsize + (((uintptr_t) addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_mva = (uintptr_t) addr;
		__DSB();
		do
		{
			__set_DCCMVAC(op_mva);	// Clean data cache line by address.
			op_mva += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
		__DMB();     // ensure the ordering of data cache maintenance operations and their effects
	}
}

void L1_CleanInvalidateDCache_by_Addr(void * addr, int32_t op_size)
{
	if (op_size > 0)
	{
		//int32_t op_size = dsize + (((uintptr_t) addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_mva = (uintptr_t) addr;
		__DSB();
		do
		{
			__set_DCCIMVAC(op_mva);	// Clean and Invalidate data cache by address.
			op_mva += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
		__DMB();     // ensure the ordering of data cache maintenance operations and their effects
	}
}

void L1_InvalidateDCache_by_Addr(void * addr, int32_t op_size)
{
	if (op_size > 0)
	{
		//int32_t op_size = dsize + (((uintptr_t) addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_mva = (uintptr_t) addr;
		do
		{
			__set_DCIMVAC(op_mva);	// Invalidate data cache line by address.
			op_mva += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
		// Cache Invalidate operation is not follow by memory-writes
	}
}

#if (! defined(__aarch64__))
/** \brief  Get CTR
\return		Cache Type Register value
*/
uint32_t __get_CTR(void)
{
	uint32_t result;
	__get_CP(15, 0, result, 0, 0, 1);
	return result;
}
#endif

int_fast32_t dcache_rowsize(void)
{
	const uint32_t v = __get_CTR();
	const uint32_t DminLine = (v >> 16) & 0x0F;	// Log2 of the number of words in the smallest cache line of all the data caches and unified caches that are controlled by the processor
	return 4 << DminLine;
}

int_fast32_t icache_rowsize(void)
{
	const uint32_t v = __get_CTR();
	const uint32_t IminLine = (v >> 0) & 0x0F;	// Log2 of the number of words in the smallest cache line of all the instruction caches and unified caches that are controlled by the processor
	return 4 << IminLine;
}

#if (__L2C_PRESENT == 1)

void L2_CleanDCache_by_Addr(void *__restrict addr, int32_t dsize)
{
	if (dsize > 0)
	{
		int32_t op_size = dsize + (((uintptr_t) addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_addr = (uintptr_t) addr /* & ~ (uintptr_t) (DCACHEROWSIZE - 1U) */;
		do
		{
			// Clean cache by physical address
			L2C_310->CLEAN_LINE_PA = op_addr;	// Atomic operation. These operations stall the slave ports until they are complete.
			op_addr += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
	}
}

void L2_CleanInvalidateDCache_by_Addr(void *__restrict addr, int32_t dsize)
{
	if (dsize > 0)
	{
		int32_t op_size = dsize + (((uintptr_t) addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_addr = (uintptr_t) addr /* & ~ (uintptr_t) (DCACHEROWSIZE - 1U) */;
		do
		{
			// Clean and Invalidate cache by physical address
			L2C_310->CLEAN_INV_LINE_PA = op_addr;	// Atomic operation. These operations stall the slave ports until they are complete.
			op_addr += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
	}
}

void L2_InvalidateDCache_by_Addr(void *__restrict addr, int32_t dsize)
{
	if (dsize > 0)
	{
		int32_t op_size = dsize + (((uintptr_t) addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_addr = (uintptr_t) addr /* & ~ (uintptr_t) (DCACHEROWSIZE - 1U) */;
		do
		{
			// Invalidate cache by physical address
			L2C_310->INV_LINE_PA = op_addr;	// Atomic operation. These operations stall the slave ports until they are complete.
			op_addr += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
	}
}
#endif /* (__L2C_PRESENT == 1) */

// Записать содержимое кэша данных в память
// применяется после начальной инициализации среды выполнния
void dcache_clean_all(void)
{
	L1C_CleanInvalidateDCacheAll();
#if (__L2C_PRESENT == 1)
	L2C_CleanInvAllByWay();
#endif
}

// Сейчас в эту память будем читать по DMA
void dcache_invalidate(uintptr_t addr, int_fast32_t dsize)
{
	L1_InvalidateDCache_by_Addr((void *) addr, dsize);
#if (__L2C_PRESENT == 1)
	L2_InvalidateDCache_by_Addr((void *) addr, dsize);
#endif /* (__L2C_PRESENT == 1) */
}

// Сейчас эта память будет записываться по DMA куда-то
void dcache_clean(uintptr_t addr, int_fast32_t dsize)
{
	L1_CleanDCache_by_Addr((void *) addr, dsize);
#if (__L2C_PRESENT == 1)
	L2_CleanDCache_by_Addr((void *) addr, dsize);
#endif /* (__L2C_PRESENT == 1) */
}

// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
void dcache_clean_invalidate(uintptr_t addr, int_fast32_t dsize)
{
	L1_CleanInvalidateDCache_by_Addr((void *) addr, dsize);
#if (__L2C_PRESENT == 1)
	L2_CleanInvalidateDCache_by_Addr((void *) addr, dsize);
#endif /* (__L2C_PRESENT == 1) */
}

#elif CPUSTYLE_F133

// C906 core specific cache operations


//	cache.c/iva means three instructions:
//	 - dcache.cva %0  : writeback     by virtual address cacheline
//	 - dcache.iva %0  : invalid       by virtual address cacheline
//	 - dcache.civa %0 : writeback+inv by virtual address cacheline

//static inline void local_flush_icache_all(void)
//{
//	asm volatile ("fence.i" ::: "memory");
//}


//      __ASM volatile(".4byte 0x0245000b\n":::"memory"); /* dcache.cva a0 */
//      __ASM volatile(".4byte 0x0285000b\n":::"memory"); /* dcache.cpa a0 */
//      __ASM volatile(".4byte 0x0265000b\n":::"memory"); /* dcache.iva a0 */
//      __ASM volatile(".4byte 0x02a5000b\n":::"memory"); /* dcache.ipa a0 */
//      __ASM volatile(".4byte 0x0275000b\n":::"memory"); /* dcache.civa a0 */
//      __ASM volatile(".4byte 0x02b5000b\n":::"memory"); /* dcache.cipa a0 */
//      __ASM volatile(".4byte 0x0010000b\n":::"memory"); /* dcache.call */
//

// Сейчас в эту память будем читать по DMA
void dcache_invalidate(uintptr_t base, int_fast32_t dsize)
{
	if (dsize > 0)
	{
		//base &= ~ (uintptr_t) (DCACHEROWSIZE - 1);
		for(; dsize > 0; dsize -= DCACHEROWSIZE, base += DCACHEROWSIZE)
		{
			__ASM volatile(
					"\t" "mv a0,%0\n"
					//"\t" ".4byte 0x0265000b\n" /* dcache.iva a0 */
					"\t" ".4byte 0x02a5000b\n" /* dcache.ipa a0 */
					:: "r"(base):"a0");
		}
		__ASM volatile(".4byte 0x01b0000b\n":::"memory");		/* sync.is */
	}
}

// Сейчас эта память будет записываться по DMA куда-то
void dcache_clean(uintptr_t base, int_fast32_t dsize)
{
	if (dsize > 0)
	{
		//base &= ~ (uintptr_t) (DCACHEROWSIZE - 1);
		for(; dsize > 0; dsize -= DCACHEROWSIZE, base += DCACHEROWSIZE)
		{
			__ASM volatile(
					"\t" "mv a0,%0\n"
					//"\t" ".4byte 0x0245000b\n" /* dcache.cva a0 */
					"\t" ".4byte 0x0285000b\n" /* dcache.cpa a0 */
					:: "r"(base):"a0");
		}
		__ASM volatile(".4byte 0x01b0000b\n":::"memory");		/* sync.is */
	}
}

// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
void dcache_clean_invalidate(uintptr_t base, int_fast32_t dsize)
{
	if (dsize > 0)
	{
		//base &= ~ (uintptr_t) (DCACHEROWSIZE - 1);
		for(; dsize > 0; dsize -= DCACHEROWSIZE, base += DCACHEROWSIZE)
		{
			__ASM volatile(
					"\t" "mv a0,%0\n"
					//"\t" ".4byte 0x0275000b\n" /* dcache.civa a0 */
					"\t" ".4byte 0x02b5000b\n" /* dcache.cipa a0 */
					:: "r"(base):"a0");
		}
		__ASM volatile(".4byte 0x01b0000b\n":::"memory");		/* sync.is */
	}
}

// Записать содержимое кэша данных в память
// применяется после начальной инициализации среды выполнния
void dcache_clean_all(void)
{
	__ASM volatile(".4byte 0x0010000b\n":::"memory"); /* dcache.call */
}


int_fast32_t dcache_rowsize(void)
{
	return DCACHEROWSIZE;
}


int_fast32_t icache_rowsize(void)
{
	return ICACHEROWSIZE;
}

#else

// Заглушки
// Сейчас в эту память будем читать по DMA
void dcache_invalidate(uintptr_t base, int_fast32_t dsize)
{
}

// Сейчас эта память будет записываться по DMA куда-то
void dcache_clean(uintptr_t base, int_fast32_t dsize)
{
}

// Записать содержимое кэша данных в память
// применяется после начальной инициализации среды выполнния
void dcache_clean_all(void)
{
}

// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
void dcache_clean_invalidate(uintptr_t base, int_fast32_t dsize)
{
}


int_fast32_t dcache_rowsize(void)
{
	return DCACHEROWSIZE;
}


int_fast32_t icache_rowsize(void)
{
	return ICACHEROWSIZE;
}

#endif /* CPUSTYLE_ARM_CM7 */


#if (__CORTEX_A != 0) || CPUSTYLE_ARM9 || CPUSTYLE_RISCV



/* зависящая от процессора карта распределения memory regions */
uint_fast64_t
ttb_mempage_accessbits(const getmmudesc_t * arch, uint_fast64_t phyaddr, int ro, int xn)
{
	//const uintptr_t addrbase = phyaddr & ~ (uintptr_t) UINT32_C(0x0FFFFF);

#if ! CPUSTYLE_R7S721020
	// На Renesas RZA1 недостаточно памяти для выделения выровненной на 1 мегабайт некешируесой области.
	// Все сравнения должны быть не точнее 2 MB

	extern uint32_t __RAMNC_BASE;
	extern uint32_t __RAMNC_TOP;
	const uintptr_t __ramnc_base = (uintptr_t) & __RAMNC_BASE;
	const uintptr_t __ramnc_top = (uintptr_t) & __RAMNC_TOP;
	if (phyaddr >= __ramnc_base && phyaddr < __ramnc_top)			// non-cached DRAM
		return arch->mncached(phyaddr, ro, 1 || xn);
#endif

#if CPUSTYLE_R7S721020

	// Все сравнения должны быть не точнее 1 MB

	if (phyaddr == 0x00000000)
		return TTB_PARA_NO_ACCESS(phyaddr);		// NULL pointers access trap

	if (phyaddr >= 0x18000000 && phyaddr < 0x20000000)			// FIXME: QSPI memory mapped should be R/O, but...
		return arch->mcached(phyaddr, ro || 0, 0);

	if (phyaddr >= 0x00000000 && phyaddr < 0x00A00000)			// up to 10 MB
		return arch->mcached(phyaddr, ro, 0);
	if (phyaddr >= 0x20000000 && phyaddr < 0x20A00000)			// up to 10 MB
		return arch->mcached(phyaddr, ro, 0);

	return arch->mdevice(phyaddr);

#elif CPUSTYLE_STM32MP1

	// Все сравнения должны быть не точнее 1 MB
	if (phyaddr >= 0x20000000 && phyaddr < 0x30000000)			// SYSRAM
		return arch->mcached(phyaddr, ro, 0);
	// 1 GB DDR RAM memory size allowed
	if (phyaddr >= 0xC0000000)							// DDR memory
		return arch->mcached(phyaddr, ro, 0);

	return arch->mdevice(phyaddr);
	return TTB_PARA_NO_ACCESS(phyaddr);

#elif CPUSTYLE_XC7Z

	// Все сравнения должны быть не точнее 1 MB

	if (phyaddr >= 0x00000000 && phyaddr < 0x00100000)			//  OCM (On Chip Memory), DDR3_SCU
		return arch->mcached(phyaddr, ro, 0);

	if (phyaddr >= 0x00100000 && phyaddr < 0x40000000)			//  DDR3 - 255 MB
		return arch->mcached(phyaddr, ro, 0);

	if (phyaddr >= 0xE1000000 && phyaddr < 0xE6000000)			//  SMC (Static Memory Controller)
		return arch->mcached(phyaddr, ro, 0);

	if (phyaddr >= 0x40000000 && phyaddr < 0xFC000000)	// PL, peripherials
		return arch->mdevice(phyaddr);

	if (phyaddr >= 0xFC000000 && phyaddr < 0xFE000000)			//  Quad-SPI linear address for linear mode
		return arch->mcached(phyaddr, ro || 0, 0);

	if (phyaddr >= 0xFFF00000)			// OCM (On Chip Memory) is mapped high
		return arch->mcached(phyaddr, ro, 0);

	return arch->mdevice(phyaddr);

#elif CPUSTYLE_T113

	// Все сравнения должны быть не точнее 1 MB

	if (phyaddr < 0x00400000)
		return arch->mcached(phyaddr, ro, 0);

	if (phyaddr >= 0x40000000)			//  DDR3 - 2 GB
		return arch->mcached(phyaddr, ro, 0);
//	if (phyaddr >= 0x000020000 && phyaddr < 0x000038000)			//  SYSRAM - 64 kB
//		return arch->mcached(phyaddr, ro, 0);

	return arch->mdevice(phyaddr);

#elif CPUSTYLE_F133

	// Все сравнения должны быть не точнее 2 MB

	if (phyaddr < 0x00400000)
		return arch->mcached(phyaddr, ro, 0);

	if (phyaddr >= 0x40000000)			//  DDR3 - 2 GB
		return arch->mcached(phyaddr, ro, 0);
//	if (phyaddr >= 0x000020000 && phyaddr < 0x000038000)			//  SYSRAM - 64 kB
//		return arch->mcached(phyaddr, ro, 0);

	return arch->mdevice(phyaddr);

#elif CPUSTYLE_V3S

	// Все сравнения должны быть не точнее 1 MB

	if (phyaddr < 0x00400000)
		return arch->mcached(phyaddr, ro, 0);

	if (phyaddr >= 0x40000000)			//  DDR3 - 2 GB
		return arch->mcached(phyaddr, ro, 0);
//	if (phyaddr >= 0x000020000 && phyaddr < 0x000038000)			//  SYSRAM - 64 kB
//		return arch->mcached(phyaddr, ro, 0);

	return arch->mdevice(phyaddr);

#elif CPUSTYLE_H3

	// Все сравнения должны быть не точнее 1 MB

	if (phyaddr < 0x01000000)
		return arch->mcached(phyaddr, ro, 0);	// SRAM A1, SRAM A2, SRAM C

	if (phyaddr >= 0xC0000000)
		return arch->mcached(phyaddr, ro, 0);	// N-BROM, S-BROM

	if (phyaddr >= 0x40000000)			//  DDR3 - 2 GB
		return arch->mcached(phyaddr, ro, 0);

	return arch->mdevice(phyaddr);

#elif CPUSTYLE_A64

	// Все сравнения должны быть не точнее 2 MB

	if (phyaddr < 0x01000000)
		return arch->mcached(phyaddr, ro, 0);

	if (phyaddr >= 0x40000000)			//  DDR3 - 2 GB
		return arch->mcached(phyaddr, ro, 0);

	return arch->mdevice(phyaddr);

#elif CPUSTYLE_T507

	// Все сравнения должны быть не точнее 2 MB

	if (phyaddr < 0x01000000)			// BROM, SYSRAM A1, SRAM C
		return arch->mcached(phyaddr, ro, 0);
	// 1 GB DDR RAM memory size allowed
	if (phyaddr >= 0x40000000)			//  DRAM - 2 GB
		return arch->mcached(phyaddr, ro, 0);

	return arch->mdevice(phyaddr);

#elif CPUSTYLE_A133 || CPUSTYLE_R818

	// Все сравнения должны быть не точнее 2 MB

	if (phyaddr < 0x01000000)			// BROM, SYSRAM A1, SRAM C
		return arch->mcached(phyaddr, ro, 0);
	// 1 GB DDR RAM memory size allowed
	if (phyaddr >= 0x40000000)			//  DRAM - 2 GB
		return arch->mcached(phyaddr, ro, 0);

	return arch->mdevice(phyaddr);

#elif CPUSTYLE_VM14

	// 1892ВМ14Я ELVEES multicore.ru
	// Все сравнения должны быть не точнее 1 MB

	if (phyaddr >= 0x20000000 && phyaddr < 0x20100000)			//  SRAM - 64K
		return arch->mcached(phyaddr, ro, 0);

	if (phyaddr >= 0x40000000 && phyaddr < 0xC0000000)			//  DDR - 2 GB
		return arch->mcached(phyaddr, ro, 0);

	return arch->mdevice(phyaddr);

#else

	// Все сравнения должны быть не точнее 1 MB

	#warning ttb_mempage_accessbits: Unhandled CPUSTYLE_xxxx

	return arch->mdevice(phyaddr);

#endif
}

#if 1
// GPU MMU

static uint_fast64_t gpu_mali400_4k_mcached(uint_fast64_t addr, int ro, int xn)
{
	return 0;//TTB_PARA_AARCH32_4k_CACHED(addr, ro, xn);
}
static uint_fast64_t gpu_mali400_4k_mncached(uint_fast64_t addr, int ro, int xn)
{
	return 0;//TTB_PARA_AARCH32_4k_NCACHED(addr, ro, xn);
}
static uint_fast64_t gpu_mali400_4k_mdevice(uint_fast64_t addr)
{
	return 0;//TTB_PARA_AARCH32_4k_DEVICE(addr);
}
static uint_fast64_t gpu_mali400_4k_mnoaccess(uint_fast64_t addr)
{
	return 0;
}
// Next level table
static uint_fast64_t gpu_mali400_4k_mtable(uint_fast64_t addr)
{
	// 1KB granulation address
	return 0;//TTB_PARA_AARCH32_4k_PAGE(addr);	// First-level table entry - Page table
}

static const getmmudesc_t gpu_mali400_table4k =
{
	.mcached = gpu_mali400_4k_mcached,
	.mncached = gpu_mali400_4k_mncached,
	.mdevice = gpu_mali400_4k_mdevice,
	.mnoaccess = gpu_mali400_4k_mnoaccess,
	.mtable = gpu_mali400_4k_mtable
};

#endif

#if defined (__aarch64__)
// 13.3 Memory attributes

// Also see TCR_EL3 parameter
#define CACHEATTR_NOCACHE 0x00		// Non-cacheable
#define CACHEATTR_WB_WA_CACHE 0x01	// Write-Back Write-Allocate Cacheable
//#define CACHEATTR_WT_NWA_CACHE 0x02	// Write-Through Cacheable
//#define CACHEATTR_WB_NWA_CACHE 0x03	// Write-Back no Write-Allocate Cacheable

static const uint32_t aarch64_pageattr =
			0x01 * (UINT32_C(1) << 10) |	// AF
			0x00 * (UINT32_C(1) << 5) |		// NS
			0x01 * (UINT32_C(1) << 6) |		// AP[1:0]
			0x03 * (UINT32_C(1) << 8) |		// SH[1:0]
			0;

//	//PRINTF("aarch64_pageattr=%08X\n", (unsigned) aarch64_pageattr);
//	//ASSERT(0x00000740 == aarch64_pageattr);
//	//uintptr_t ttb_base1_addr = (uintptr_t) ttb_base1 & ~ UINT64_C(0x3FFFFFFF);
//	// 0x740 - BLOCK_1GB
//	// 0x74C - BLOCK_2MB

	enum aarch64_attrindex
	{
		AARCH64_ATTR_CACHED = 0,
		AARCH64_ATTR_NCACHED,
		AARCH64_ATTR_DEVICE

	};

	static const uint32_t pageAttrDEVICE =
			aarch64_pageattr |
			AARCH64_ATTR_DEVICE * (UINT32_C(1) << 2) |
			0
			;
	static const uint32_t pageAttrRAM =
			aarch64_pageattr |
			AARCH64_ATTR_CACHED * (UINT32_C(1) << 2) |
			0
			;
	static const uint32_t pageAttrNCRAM =
			aarch64_pageattr |
			AARCH64_ATTR_NCACHED * (UINT32_C(1) << 2) |
			0
			;
//	PRINTF("pageAttrNCRAM=%08X\n", pageAttrNCRAM);
//	PRINTF("pageAttrRAM=%08X\n", pageAttrRAM);
//	PRINTF("pageAttrDEVICE=%08X\n", pageAttrDEVICE);
//	pageAttrNCRAM=00000744
//	pageAttrRAM=00000740
//	pageAttrDEVICE=00000748

static uint_fast64_t arch64_mcached(uint_fast64_t addr, int ro, int xn)
{
	return (addr & ~ (uint64_t) UINT32_C(0x0FFFFF)) | aarch64_pageattr | pageAttrRAM | 0x01;
}
static uint_fast64_t arch64_mncached(uint_fast64_t addr, int ro, int xn)
{
	return (addr & ~ (uint64_t) UINT32_C(0x0FFFFF)) | aarch64_pageattr | pageAttrNCRAM | 0x01;
}
static uint64_t arch64_mdevice(uint64_t addr)
{
	return (addr & ~ (uint64_t) UINT32_C(0x0FFFFF)) | aarch64_pageattr | pageAttrDEVICE | 0x01;
}
static uint_fast64_t arch64_mnoaccess(uint_fast64_t addr)
{
	return 0;
}
// Next level table
static uint_fast64_t arch64_mtable(uint_fast64_t addr)
{
	return 0;
}

static const getmmudesc_t arch64table2M =
{
	.mcached = arch64_mcached,
	.mncached = arch64_mncached,
	.mdevice = arch64_mdevice,
	.mnoaccess = arch64_mnoaccess,
	.mtable = arch64_mtable
};

#elif (__CORTEX_A != 0)

/*
 * https://community.st.com/s/question/0D73W000000UagD/what-a-type-of-mmu-memory-regions-recommended-for-regions-with-peripheralsstronglyordered-or-device?s1oid=00Db0000000YtG6&s1nid=0DB0X000000DYbd&emkind=chatterCommentNotification&s1uid=0050X000007vtUt&emtm=1599464922440&fromEmail=1&s1ext=0&t=1599470826880
 *
 *
 * PatrickF (ST Employee)

2 hours ago
Hello,

"Device" should be used for all peripherals to get best performance.
There is no rationale to use "Strongly-Ordered" with Cortex-A7
(explanation should be found by deep analysis/understanding of various ARM documents).
 *
 *
*/

// Short-descriptor format memory region attributes, without TEX remap
// When using the Short-descriptor translation table formats, TEX remap is disabled when SCTLR.TRE is set to 0.

// For TRE - see
// B4.1.127 PRRR, Primary Region Remap Register, VMSA

#define APRWval 		0x03	/* Full access */
#define APROval 		0x06	/* All write accesses generate Permission faults */
#define DOMAINval		0x0F
#define SECTIONval		0x02	/* 0b10, Section or Supersection, PXN  */

/* Table B3-10 TEX, C, and B encodings when TRE == 0 */

/* Outer and Inner Write-Back, Write-Allocate */
// Cacheable memory attributes, without TEX remap
// DDI0406C_d_armv7ar_arm.pdf
// Table B3-11 Inner and Outer cache attribute encoding

#define MKATTR_TEXval(cacheattr) (0x04u | ((cacheattr) & 0x03u))
#define MKATTR_Cval(cacheattr) (!! ((cacheattr) & 0x02u))
#define MKATTR_Bval(cacheattr) (!! ((cacheattr) & 0x01u))

// Also see __set_TTBR0 parameter
#define CACHEATTR_NOCACHE 0x00		// Non-cacheable
#define CACHEATTR_WB_WA_CACHE 0x01	// Write-Back, Write-Allocate
//#define CACHEATTR_WT_NWA_CACHE 0x02	// Write-Through, no Write-Allocate
//#define CACHEATTR_WB_NWA_CACHE 0x03	// Write-Back, no Write-Allocate

/* атрибуты для разных областей памяти (при TEX[2]=1 способе задания) */
#define RAM_ATTRS CACHEATTR_WB_WA_CACHE
//#define RAM_ATTRS CACHEATTR_WB_NWA_CACHE
#define DEVICE_ATTRS CACHEATTR_NOCACHE
#define NCRAM_ATTRS CACHEATTR_NOCACHE

#define TEXval_RAM		MKATTR_TEXval(RAM_ATTRS)	// Define the Outer cache attribute
#define Cval_RAM		MKATTR_Cval(RAM_ATTRS)		// Define the Inner cache attribute
#define Bval_RAM		MKATTR_Bval(RAM_ATTRS)		// Define the Inner cache attribute

#if WITHSMPSYSTEM
	#define SHAREDval_RAM 1		// required for ldrex.. and strex.. functionality
#else /* WITHSMPSYSTEM */
	#define SHAREDval_RAM 0		// If non-zero, Renesas Cortex-A9 hung by buffers
#endif /* WITHSMPSYSTEM */

#define TEXval_NCRAM	MKATTR_TEXval(NCRAM_ATTRS)	// Define the Outer cache attribute
#define Cval_NCRAM		MKATTR_Cval(NCRAM_ATTRS)	// Define the Inner cache attribute
#define Bval_NCRAM		MKATTR_Bval(NCRAM_ATTRS)	// Define the Inner cache attribute

#if WITHSMPSYSTEM
	#define SHAREDval_NCRAM 1		// required for ldrex.. and strex.. functionality
#else /* WITHSMPSYSTEM */
	#define SHAREDval_NCRAM 0		// If non-zero, Renesas Cortex-A9 hung by buffers
#endif /* WITHSMPSYSTEM */

#if 1
	/* Shareable Device */
	#define TEXval_DEVICE       0x00
	#define Cval_DEVICE         0
	#define Bval_DEVICE         1
	#define SHAREDval_DEVICE 	0
#else
	/* Shareable Device */
	#define TEXval_DEVICE	MKATTR_TEXval(DEVICE_ATTRS)	// Define the Outer cache attribute
	#define Cval_DEVICE		MKATTR_Cval(DEVICE_ATTRS)	// Define the Inner cache attribute
	#define Bval_DEVICE		MKATTR_Bval(DEVICE_ATTRS)	// Define the Inner cache attribute

	#if WITHSMPSYSTEM
		#define SHAREDval_DEVICE 1		// required for ldrex.. and strex.. functionality
	#else /* WITHSMPSYSTEM */
		#define SHAREDval_DEVICE 0		// If non-zero, Renesas Cortex-A9 hung by buffers
	#endif /* WITHSMPSYSTEM */
#endif

// See B3.5.2 in DDI0406C_C_arm_architecture_reference_manual.pdf

// Large page
#define	TTB_PARA_AARCH32_1M(addr, TEXv, Bv, Cv, DOMAINv, SHAREDv, APv, XNv) ( \
		((addr) & ~ (uint64_t) UINT32_C(0x0FFFFF)) | /* 1M granulation address */ \
		(SECTIONval) * (UINT32_C(1) << 0) |	/* 0b10, Section or Supersection, PXN */ \
		!! (Bv) * (UINT32_C(1) << 2) |	/* B */ \
		!! (Cv) * (UINT32_C(1) << 3) |	/* C */ \
		!! (XNv) * (UINT32_C(1) << 4) |	/* XN The Execute-never bit. */ \
		(DOMAINv) * (UINT32_C(1) << 5) |	/* DOMAIN */ \
		0 * (UINT32_C(1) << 9) |	/* implementation defined */ \
		(((APv) >> 0) & 0x03) * (UINT32_C(1) << 10) |	/* AP [1..0] */ \
		((TEXv) & 0x07) * (UINT32_C(1) << 12) |	/* TEX */ \
		(((APv) >> 2) & 0x01) * (UINT32_C(1) << 15) |	/* AP[2] */ \
		!! (SHAREDv) * (UINT32_C(1) << 16) |	/* S */ \
		0 * (UINT32_C(1) << 17) |	/* nG */ \
		0 * (UINT32_C(1) << 18) |	/* 0 */ \
		0 * (UINT32_C(1) << 19) |	/* NS */ \
		0 \
	)

// B3.5.1 Short-descriptor translation table format descriptors
// Short-descriptor translation table first-level descriptor formats

#define	TTB_PARA_AARCH32_4k_table(addr, TEXv, Bv, Cv, DOMAINv, SHAREDv, APv, XNv) ( \
		((addr) & ~ (uint64_t) UINT32_C(0x03FF)) | /* Page table base address, bits[31:10]. bits 31:10 */ \
		0 * (UINT32_C(1) << 9) |	/* implementation defined */ \
		(DOMAINv) * (UINT32_C(1) << 5) |	/* DOMAIN */ \
		0 * (UINT32_C(1) << 3) |	/* NS */ \
		0x01 * (UINT32_C(1) << 2) |	/* PXN */ \
		0x01 * (UINT32_C(1) << 0) |	/* */ \
		0 \
	)

// B3.5.1 Short-descriptor translation table format descriptors
// Short-descriptor translation table second-level descriptor formats

// Small page (4KB memory page)
#define	TTB_PARA_AARCH32_4k(addr, TEXv, Bv, Cv, DOMAINv, SHAREDv, APv, XNv) ( \
		((addr) & ~ (uint64_t) UINT32_C(0x0FFF)) | /* 4k granulation address */ \
		0 * (UINT32_C(1) << 11) |	/* nG */ \
		!! (SHAREDv) * (UINT32_C(1) << 10) |	/* ! S */ \
		(((APv) >> 2) & 0x01) * (UINT32_C(1) << 9) |	/* AP[2] */ \
		((TEXv) & 0x07) * (UINT32_C(1) << 6) |	/* TEX */ \
		(((APv) >> 0) & 0x03) * (UINT32_C(1) << 4) |	/* AP [1..0] */ \
		!! (Cv) * (UINT32_C(1) << 3) |	/* C */ \
		!! (Bv) * (UINT32_C(1) << 2) |	/* B */ \
		1 * (UINT32_C(1) << 1) |	/* 1 */ \
		!! (XNv) * (UINT32_C(1) << 0) |	/* XN The Execute-never bit. */ \
		0 \
	)

#define	TTB_PARA_AARCH32_1M_NCACHED(addr, ro, xn)	TTB_PARA_AARCH32_1M((addr), TEXval_NCRAM, Bval_NCRAM, Cval_NCRAM, DOMAINval, SHAREDval_NCRAM, (ro) ? APROval : APRWval, (xn) != 0)
#define	TTB_PARA_AARCH32_1M_CACHED(addr, ro, xn) 	TTB_PARA_AARCH32_1M((addr), TEXval_RAM, Bval_RAM, Cval_RAM, DOMAINval, SHAREDval_RAM, (ro) ? APROval : APRWval, (xn) != 0)
#define	TTB_PARA_AARCH32_1M_DEVICE(addr) 			TTB_PARA_AARCH32_1M((addr), TEXval_DEVICE, Bval_DEVICE, Cval_DEVICE, DOMAINval, SHAREDval_DEVICE, APRWval, 1 /* XN=1 */)

// TODO: implementing
#define	TTB_PARA_AARCH32_4k_NCACHED(addr, ro, xn)	TTB_PARA_AARCH32_4k((addr), TEXval_NCRAM, Bval_NCRAM, Cval_NCRAM, DOMAINval, SHAREDval_NCRAM, (ro) ? APROval : APRWval, (xn) != 0)
#define	TTB_PARA_AARCH32_4k_CACHED(addr, ro, xn) 	TTB_PARA_AARCH32_4k((addr), TEXval_RAM, Bval_RAM, Cval_RAM, DOMAINval, SHAREDval_RAM, (ro) ? APROval : APRWval, (xn) != 0)
#define	TTB_PARA_AARCH32_4k_DEVICE(addr) 			TTB_PARA_AARCH32_4k((addr), TEXval_DEVICE, Bval_DEVICE, Cval_DEVICE, DOMAINval, SHAREDval_DEVICE, APRWval, 1 /* XN=1 */)
// First-level table entry - Page table
#define	TTB_PARA_AARCH32_4k_PAGE(addr) 			TTB_PARA_AARCH32_4k_table((addr), TEXval_DEVICE, Bval_DEVICE, Cval_DEVICE, DOMAINval, SHAREDval_DEVICE, APRWval, 1 /* XN=1 */)

static uint_fast64_t arch32_1M_mcached(uint_fast64_t addr, int ro, int xn)
{
	return TTB_PARA_AARCH32_1M_CACHED(addr, ro, xn);
}
static uint_fast64_t arch32_1M_mncached(uint_fast64_t addr, int ro, int xn)
{
	return TTB_PARA_AARCH32_1M_NCACHED(addr, ro, xn);
}
static uint_fast64_t arch32_1M_mdevice(uint_fast64_t addr)
{
	return TTB_PARA_AARCH32_1M_DEVICE(addr);
}
static uint_fast64_t arch32_1M_mnoaccess(uint_fast64_t addr)
{
	return 0;
}
// Next level table
static uint_fast64_t arch32_1M_mtable(uint_fast64_t addr)
{
	return 0;
}

static const getmmudesc_t arch32table1M =
{
	.mcached = arch32_1M_mcached,
	.mncached = arch32_1M_mncached,
	.mdevice = arch32_1M_mdevice,
	.mnoaccess = arch32_1M_mnoaccess,
	.mtable = arch32_1M_mtable
};

static uint_fast64_t arch32_4k_mcached(uint_fast64_t addr, int ro, int xn)
{
	return TTB_PARA_AARCH32_4k_CACHED(addr, ro, xn);
}
static uint_fast64_t arch32_4k_mncached(uint_fast64_t addr, int ro, int xn)
{
	return TTB_PARA_AARCH32_4k_NCACHED(addr, ro, xn);
}
static uint_fast64_t arch32_4k_mdevice(uint_fast64_t addr)
{
	return TTB_PARA_AARCH32_4k_DEVICE(addr);
}
static uint_fast64_t arch32_4k_mnoaccess(uint_fast64_t addr)
{
	return 0;
}
// Next level table
static uint_fast64_t arch32_4k_mtable(uint_fast64_t addr)
{
	// 1KB granulation address
	return TTB_PARA_AARCH32_4k_PAGE(addr);	// First-level table entry - Page table
}

static const getmmudesc_t arch32table4k =
{
	.mcached = arch32_4k_mcached,
	.mncached = arch32_4k_mncached,
	.mdevice = arch32_4k_mdevice,
	.mnoaccess = arch32_4k_mnoaccess,
	.mtable = arch32_4k_mtable
};

#elif CPUSTYLE_RISCV

	//ttb_level0_1MB_initialize(ttb_mempage_accessbits, 0, 0);

	// See https://chromite.readthedocs.io/en/latest/mmu.html

	// XuanTie-Openc906 SYSMAP

	// The C906 is fully compatible with the RV64GC instruction set and supports the standard M/S/U privilege program model.
	// The C906 includes a standard 8-16 region PMP and Sv39 MMU, which is fully compatible with RISC-V Linux.
	// The C906 includes standard CLINT and PLIC interrupt controllers, RV compatible HPM.
	// ? 0xEFFFF000
	// See https://github.com/sophgo/cvi_alios_open/blob/aca2daa48266cd96b142f83bad4e33a6f13d6a24/components/csi/csi2/include/core/core_rv64.h
	// Strong Order, Cacheable, Bufferable, Shareable, Security

	// Bit 63 - Strong order
	// Bit 62 - Cacheable
	// Bit 61 - Buffer
	// Bit 0 - Valid
	#define RAM_ATTRS 		((UINT64_C(0) << 63) | (UINT64_C(1) << 62) | (UINT64_C(1) << 61) | (UINT64_C(0x0E) << 0) | 1)	// Cacheable memory
	#define NCRAM_ATTRS 	((UINT64_C(0) << 63) | (UINT64_C(0) << 62) | (UINT64_C(0) << 61) | (UINT64_C(0x0E) << 0) | 1)	// Non-cacheable memory
	#define DEVICE_ATTRS 	((UINT64_C(1) << 63) | (UINT64_C(0) << 62) | (UINT64_C(0) << 61) | (UINT64_C(0x0E) << 0) | 1)	// Non-bufferable device
	#define TABLE_ATTRS		((UINT64_C(0) << 63) | 1) // Pointer to next level of page table

	// See Table 4.2: Encoding of PTE Type field.

	#define	TTB_PARA_RV64_CACHED(addr, ro, xn) 	((0 * (addr) & ~ UINT64_C(0xFFF)) | (0x00u << 1) | 0x01)
	#define	TTB_PARA_RV64_NCACHED(addr, ro, xn) 	((0 * (addr) & ~ UINT64_C(0xFFF)) | (0x00u << 1) | 0x01)
	#define	TTB_PARA_RV64_DEVICE(addr)			((0 * (addr) & ~ UINT64_C(0xFFF)) | (0x00u << 1) | 0x01)
	#define	TTB_PARA_RV64_NO_ACCESS(addr) 		0

#endif /* __CORTEX_A */

#if defined(__aarch64__)

	/* TTB должна размещаться в памяти, не инициализируемой перед запуском системы */
	// Last x4 - for 34 bit address (16 GB address space)
	// Check TCR_EL3 setup
	// pages of 2 MB
	static RAMFRAMEBUFF __ALIGNED(4 * 1024) volatile uint64_t level2_pagetable [512 * 4 * 4];	// ttb0_base must be a 4KB-aligned address.
	static RAMFRAMEBUFF __ALIGNED(4 * 1024) volatile uint64_t ttb0_base [ARRAY_SIZE(level2_pagetable) / 512];	// ttb0_base must be a 4KB-aligned address.

#elif CPUSTYLE_RISCV

	static RAMFRAMEBUFF __ALIGNED(4 * 1024) volatile uint64_t level2_pagetable [512 * 4];	// Used as PPN in SATP register
	static RAMFRAMEBUFF __ALIGNED(4 * 1024) volatile uint64_t ttb0_base [512];	// Used as PPN in SATP register

	// https://lupyuen.codeberg.page/articles/mmu.html#appendix-flush-the-mmu-cache-for-t-head-c906
	// https://github.com/apache/nuttx/blob/4d63921f0a28aeee89b3a2ae861aaa83d731d28d/arch/risc-v/src/common/riscv_mmu.h#L220
	static inline void mmu_write_satp(uintptr_t reg)
	{
	  __ASM volatile
	    (
	      "csrw satp, %0\n"
	      "sfence.vma x0, x0\n"
	      "fence rw, rw\n"
	      //"fence.i\n"
	      :
	      : "rK" (reg)
	      : "memory"
	    );

	  /* Flush the MMU Cache if needed (T-Head C906) */

//	  if (mmu_flush_cache != NULL)
//	    {
//	      mmu_flush_cache(reg);
//	    }
	}


	// https://lupyuen.codeberg.page/articles/mmu.html#appendix-flush-the-mmu-cache-for-t-head-c906

	// Flush the MMU Cache for T-Head C906.  Called by mmu_write_satp() after
	// updating the MMU SATP Register, when swapping MMU Page Tables.
	// This operation executes RISC-V Instructions that are specific to
	// T-Head C906.
	void mmu_flush_cache(void) {
	  __ASM volatile (
	    // DCACHE.IALL: Invalidate all Page Table Entries in the D-Cache
	    ".4byte 0x0020000b\n"

	    // SYNC.S: Ensure that all Cache Operations are completed
	    ".4byte 0x0190000b\n"
	  );
	}

#else /* defined(__aarch64__) */

	#if MMUUSE4KPAGES
		/* TTB должна размещаться в памяти, не инициализируемой перед запуском системы */
		static RAMFRAMEBUFF __ALIGNED(16 * 1024) volatile uint32_t ttb0_base [4096];	//
		static RAMFRAMEBUFF __ALIGNED(1 * 1024) volatile uint32_t ttb_L1_base [4096 * 256];	// дескрипторы страниц памяти
	#else /* MMUUSE4KPAGES */
		/* TTB должна размещаться в памяти, не инициализируемой перед запуском системы */
		static RAMFRAMEBUFF __ALIGNED(16 * 1024) volatile uint32_t ttb0_base [4096];	//
	#endif /* MMUUSE4KPAGES */

#endif /* defined(__aarch64__) */

#if defined (__aarch64__)

static void
ttb_level2_2MB_initialize(const getmmudesc_t * arch, uint_fast64_t (* accessbits)(const getmmudesc_t * arch, uintptr_t a, int ro, int xn), uintptr_t textstart, uint_fast32_t textsize)
{
	unsigned i;
	const uint_fast64_t pagesize = (UINT32_C(1) << 21);	// 2M step

	for (i = 0; i < ARRAY_SIZE(level2_pagetable); ++ i)
	{
		const uint_fast64_t phyaddr = pagesize * i;
		level2_pagetable [i] =  accessbits(arch, phyaddr, 0, 0);
	}
	/* Установить R/O атрибуты для указанной области */
	while (textsize >= pagesize)
	{
		level2_pagetable [textstart / pagesize] =  accessbits(arch, textstart, 0 * 1, 0);
		textsize -= pagesize;
		textstart += pagesize;
	}
}
#endif

static void
ttb_level0_1MB_initialize(const getmmudesc_t * arch, uint_fast64_t (* accessbits)(const getmmudesc_t * arch, uint64_t a, int ro, int xn))
{
	unsigned i;
	const uint_fast64_t pagesize = (UINT32_C(1) << 20);	// 1M step

	for (i = 0; i <  ARRAY_SIZE(ttb0_base); ++ i)
	{
		const uint_fast64_t phyaddr = pagesize * i;
		ttb0_base [i] =  accessbits(arch, phyaddr, 0, 0);
	}
}

#if MMUUSE4KPAGES && ! defined(__aarch64__) && ! CPUSTYLE_RISCV

static void
ttb_level1_4k_initialize(const getmmudesc_t * arch, uint_fast64_t (* accessbits)(const getmmudesc_t * arch, uint64_t a, int ro, int xn))
{
	unsigned i;
	const uint_fast64_t pagesize = (UINT32_C(1) << 12);	// 4k step

	for (i = 0; i <  ARRAY_SIZE(ttb_L1_base); ++ i)
	{
		const uint_fast64_t phyaddr = pagesize * i;
		ttb_L1_base [i] =  accessbits(arch, phyaddr, 0, 0);
	}
}

// элементы указывают на Second-level table - каждая содержит описание 256 страниц по 4 килобайта - итого 1 мегабайт памяти
// размер одной страницы - 1 килобайт

static void
ttb_level0_4k_initialize(const getmmudesc_t * arch, uint_fast64_t (* accessbits)(const getmmudesc_t * arch, uint_fast64_t a, int ro, int xn))
{
	unsigned i;
	const uint_fast64_t pagesize = (UINT32_C(1) << 10);	// 1k step

	for (i = 0; i <  ARRAY_SIZE(ttb0_base); ++ i)
	{
		const uint_fast64_t phyaddr = (uint_fast64_t) (uintptr_t) ttb_L1_base + (i * pagesize);
		ttb0_base [i] =  arch->mtable(phyaddr);
	}
}
#endif /* MMUUSE4KPAGES && ! defined(__aarch64__) && ! CPUSTYLE_RISCV */

void
sysinit_mmu_tables(void)
{
	PRINTF("sysinit_mmu_tables\n");

#if (__CORTEX_A != 0) || CPUSTYLE_ARM9
	// MMU iniitialize

	#if (__CORTEX_A == 9U) && WITHSMPSYSTEM && defined (SCU_CONTROL_BASE)
		{
			// SCU inut
			// SCU Control Register
			((volatile uint32_t *) SCU_CONTROL_BASE) [0] &= ~ 0x01;
	//
	//
	//		// Filtering Start Address Register
	//		((volatile uint32_t *) SCU_CONTROL_BASE) [0x10] = (((volatile uint32_t *) SCU_CONTROL_BASE) [0x10] & ~ (0xFFFuL << 20)) |
	//				(0x001uL << 20) |
	//				0;
	//		TP();
	//		// Filtering End Address Register
	//		((volatile uint32_t *) SCU_CONTROL_BASE) [0x11] = (((volatile uint32_t *) SCU_CONTROL_BASE) [0x11] & ~ (0xFFFuL << 20)) |
	//				(0xFFEuL << 20) |
	//				0;

			((volatile uint32_t *) SCU_CONTROL_BASE) [0x3] = 0;		// SCU Invalidate All Registers in Secure State
			((volatile uint32_t *) SCU_CONTROL_BASE) [0] |= 0x01;	// SCU Control Register
		}
	#endif /* 1 && (__CORTEX_A == 9U) && WITHSMPSYSTEM && defined (SCU_CONTROL_BASE) */


	#if defined (__aarch64__)
		// MMU iniitialize

		unsigned i;
		for (i = 0; i < ARRAY_SIZE(ttb0_base); ++ i)
		{
			ttb0_base [i] = (uintptr_t) (level2_pagetable + 512 * i) | 0x03;
		}

		ttb_level2_2MB_initialize(& arch64table2M, ttb_mempage_accessbits, 0, 0);

	#else
		// MMU iniitialize

	#if MMUUSE4KPAGES
		ttb_level1_4k_initialize(& arch32table4k, ttb_mempage_accessbits);
		ttb_level0_4k_initialize(& arch32table4k, ttb_mempage_accessbits);
	#else
		ttb_level0_1MB_initialize(& arch32table1M, ttb_mempage_accessbits);
	#endif

	#endif	/* defined (__aarch64__) */


#elif CPUSTYLE_RISCV
	#warning To be implemented
	// RISC-V MMU initialize


	// When the page table size is set to 4 KB, 2 MB, or 1 GB, the page table is indexed by 3, 2, or 1 times, respectively.
	uintptr_t address = 0;
	uintptr_t addrstep = UINT64_C(1) << 21;	// 2 MB
	unsigned i;
	for (i = 0; i < ARRAY_SIZE(level2_pagetable); ++ i)
	{
		level2_pagetable [i] =
				//((address >> 12) & 0x1FF) * (UINT64_C(1) << 10) |	// 9 bits PPN [0], 4 KB granulation
				((address >> 21) & 0x1FF) * (UINT64_C(1) << 19) |	// 9 bits PPN [1]
				//((address >> 36) & 0x7FF) * (UINT64_C(1) << 28) |	// 11 bits PPN [2]
				NCRAM_ATTRS |
				0;
		address += addrstep;
	}
	// Pointe to 1 GB pages
	for (i = 0; i < ARRAY_SIZE(ttb0_base); ++ i)
	{
		uintptr_t address = (uintptr_t) (level2_pagetable + 512 * i) | 0x03;
		//uintptr_t address = 1 * (UINT64_C(1) << 30) * i;
		ttb0_base [i] =
			((address >> 12) & 0x1FF) * (UINT64_C(1) << 10) |	// 9 bits PPN [0], 4 KB granulation
			//((address >> 24) & 0x1FF) * (UINT64_C(1) << 19) |	// 9 bits PPN [1]
			//((address >> 36) & 0x7FF) * (UINT64_C(1) << 28) |	// 11 bits PPN [2]
			TABLE_ATTRS |
			0;
	}

	//ttb_level2_2MB_initialize(& archtable, ttb_mempage_accessbits, 0, 0);

#endif

	PRINTF("sysinit_mmu_tables done.\n");
}

/* Загрузка TTBR, инвалидация кеш памяти и включение MMU */
void
sysinit_ttbr_initialize(void)
{
	PRINTF("sysinit_ttbr_initialize.\n");
#if defined(__aarch64__)

	ASSERT(((uintptr_t) ttb0_base & 0x0FFF) == 0); // 4 KB

	//__set_TTBR0_EL1((uintptr_t) ttb0_base);
	//__set_TTBR0_EL2((uintptr_t) ttb0_base);
	__set_TTBR0_EL3((uintptr_t) ttb0_base);

	// DDI0500J_cortex_a53_r0p4_trm.pdf
	// 4.3.53 Translation Control Register, EL3
	const uint_fast32_t IRGN_attr = CACHEATTR_WB_WA_CACHE;	// Normal memory, Inner Write-Back Write-Allocate Cacheable.
	const uint_fast32_t RGN_attr = CACHEATTR_WB_WA_CACHE;	// Normal memory, Outer Write-Back Write-Allocate Cacheable.
	const unsigned aspacebits = 21 + __log2_up(ARRAY_SIZE(level2_pagetable));	// pages of 2 MB
	uint_fast32_t tcrv =
			0x00 * (UINT32_C(1) << 14) | 	// TG0 TTBR0_EL3 granule size 0b00 4 KB
			0x03 * (UINT32_C(1) << 12) |	// 0x03 - Inner shareable
			RGN_attr * (UINT32_C(1) << 10) |	// Outer cacheability attribute
			IRGN_attr * (UINT32_C(1) << 8) |	// Inner cacheability attribute
			(64 - aspacebits) * (UINT32_C(1) << 0) |		// n=0..63. T0SZ=2^(64-n): n=28: 64GB, n=30: 16GB, n=32: 4GB, n=43: 2MB
			0;
	__set_TCR_EL3(tcrv);

	const uint_fast32_t mairv =
			0xFF * (UINT32_C(1) << (AARCH64_ATTR_CACHED * 8)) |		// Normal Memory, Inner/Outer Write-back non-transient
			0x44 * (UINT32_C(1) << (AARCH64_ATTR_NCACHED * 8)) |	// Normal memory, Inner/Outer Non-Cacheable
			0x00 * (UINT32_C(1) << (AARCH64_ATTR_DEVICE * 8)) | 	// Device-nGnRnE memory
			0;
	// Program the domain access register
	//__set_DACR32_EL2(0xFFFFFFFF); 	// domain 15: access are not checked
	__set_MAIR_EL3(mairv);

	__ISB();

	MMU_InvalidateTLB();

	// Обеспечиваем нормальную обработку RESEТ
	L1C_InvalidateDCacheAll();
	L1C_InvalidateICacheAll();
	L1C_InvalidateBTAC();
#if (__L2C_PRESENT == 1)
	L2C_InvAllByWay();
#endif

#if 0
	PRINTF("aspacebits=%u\n", aspacebits);
	//PRINTF("log2=%u\n", __log2_up(ARRAY_SIZE(level2_pagetable)));
	PRINTF("__get_TCR_EL3=%016" PRIX32 "\n", __get_TCR_EL3());
	PRINTF("__get_MAIR_EL3=%016" PRIX64 "\n", __get_MAIR_EL3());
	uint64_t mair = __get_MAIR_EL3();
	PRINTF("a7=%02X a6=%02X a5=%02X a4=%02X a3=%02X a2=%02X a1=%02X a0=%02X\n",
			(unsigned) (mair >> 56) & 0xFF,
			(unsigned) (mair >> 48) & 0xFF,
			(unsigned) (mair >> 40) & 0xFF,
			(unsigned) (mair >> 32) & 0xFF,
			(unsigned) (mair >> 24) & 0xFF,
			(unsigned) (mair >> 16) & 0xFF,
			(unsigned) (mair >> 8) & 0xFF,
			(unsigned) (mair >> 0) & 0xFF
			);
#endif
	MMU_Enable();

#elif (__CORTEX_A != 0)

	ASSERT(((uintptr_t) ttb0_base & 0x3FFF) == 0);

	//CP15_writeTTBCR(0);
	   /* Set location of level 1 page table
	    ; 31:14 - Translation table base addr (31:14-TTBCR.N, TTBCR.N is 0 out of reset)
	    ; 13:7  - 0x0
	    ; 6     - IRGN[0] 0x1  (Inner WB WA)
	    ; 5     - NOS     0x0  (Non-shared)
	    ; 4:3   - RGN     0x01 (Outer WB WA)
	    ; 2     - IMP     0x0  (Implementation Defined)
	    ; 1     - S       0x0  (Non-shared)
	    ; 0     - IRGN[1] 0x0  (Inner WB WA) */

	// B4.1.154 TTBR0, Translation Table Base Register 0, VMSA
#if WITHSMPSYSTEM
	// TTBR0
	const uint_fast32_t IRGN_attr = CACHEATTR_WB_WA_CACHE;	// Normal memory, Inner Write-Back Write-Allocate Cacheable.
	const uint_fast32_t RGN_attr = CACHEATTR_WB_WA_CACHE;	// Normal memory, Outer Write-Back Write-Allocate Cacheable.
	__set_TTBR0(
			(uintptr_t) ttb0_base |	/* Translation table base 0 address, bits[31:x]. */
			((uint_fast32_t) !! (IRGN_attr & 0x01) << 6) |	// IRGN[0]
			((uint_fast32_t) !! (IRGN_attr & 0x02) << 0) |	// IRGN[1]
			(RGN_attr << 3) |	// RGN
			!1*(UINT32_C(1) << 5) |	// NOS - Not Outer Shareable bit - TEST for RAMNC
			1*(UINT32_C(1) << 1) |	// S - Shareable bit. Indicates the Shareable attribute for the memory associated with the translation table
			0);
#else /* WITHSMPSYSTEM */
	// TTBR0
	__set_TTBR0(
			(uintptr_t) ttb0_base |	/* Translation table base 0 address, bits[31:x]. */
			//(!! (IRGN_attr & 0x02) << 6) | (!! (IRGN_attr & 0x01) << 0) |
			(UINT32_C(1) << 3) |	// RGN
			0*(UINT32_C(1) << 5) |	// NOS
			0*(UINT32_C(1) << 1) |	// S
			0);
#endif /* WITHSMPSYSTEM */
	//CP15_writeTTB1((unsigned int) ttb0_base | 0x48);	// TTBR1
	  __ISB();

	// Program the domain access register
	__set_DACR(0xFFFFFFFF); // domain 15: access are not checked

	MMU_InvalidateTLB();

	// Обеспечиваем нормальную обработку RESEТ
	L1C_InvalidateDCacheAll();
	L1C_InvalidateICacheAll();
	L1C_InvalidateBTAC();
#if (__L2C_PRESENT == 1)
	L2C_InvAllByWay();
#endif

	MMU_Enable();

#elif CPUSTYLE_RISCV

	// See https://github.com/sophgo/cvi_alios_open/blob/aca2daa48266cd96b142f83bad4e33a6f13d6a24/components/csi/csi2/include/core/core_rv64.h
	// Strong Order, Cacheable, Bufferable, Shareable, Security

	#define CSR_SATP_MODE_PHYS   0
	#define CSR_SATP_MODE_SV32   1
	#define CSR_SATP_MODE_SV39   8
	#define CSR_SATP_MODE_SV48   9
	#define CSR_SATP_MODE_SV57   10

	ASSERT(((uintptr_t) ttb0_base & 0x0FFF) == 0);
	mmu_flush_cache();
	const unsigned asid = 0;
	// 5.2.1.1 MMU address translation register (SATP)
	// When Mode is 0, the MMU is disabled. C906 supports only the MMU disabled and Sv39 modes
	const uint_fast64_t satp =
			//CSR_SATP_MODE_PHYS * (UINT64_C(1) << 60) | // MODE
			CSR_SATP_MODE_SV39 * (UINT64_C(1) << 60) | // MODE
			(asid  & UINT64_C(0xFFFF))* (UINT64_C(1) << 44) | // ASID
			(((uintptr_t) ttb0_base >> 12) & UINT64_C(0x0FFFFFFF)) * (UINT64_C(1) << 0) |	// PPN - 28 bit
			0;
	PRINTF("1 ttb0_base=%p" "\n", ttb0_base);
	PRINTF("1 csr_read_satp()=%016" PRIX64 "\n", csr_read_satp());
	//csr_write_satp(satp);
	PRINTF("2 csr_read_satp()=%016" PRIX64 "\n", csr_read_satp());

//	mmu_write_satp(satp);
//	mmu_flush_cache();
//	PRINTF("csr_read_satp()=%016" PRIX64 "\n", csr_read_satp());

	// MAEE in MXSTATUS
	//
	//
	//	/*
	//	(15) When MM is 1, unaligned access is supported, and the hardware handles unaligned access
	//	(16) When UCME is 1, user mode can execute extended cache operation instructions
	//	(17) When CLINTEE is 1, CLINT-initiated superuser software interrupts and timer interrupts can be responded to
	//	(21) When the MAEE is 1, the address attribute bit is extended in the PTE of the MMU, and the user can configure the address attribute of the page
	//	(22) When the THEADISAE is 1, the C906 extended instruction set can be used
	//	*/
	//	csr_set(CSR_MXSTATUS, 0x638000);
//	csr_set_bits_mxstatus(
//			1 * (UINT32_C(1) << 21) |
//			0
//			);

//	//#warning Implement for RISC-C
//	// 4.1.11 Supervisor Page-Table Base Register (sptbr)
//	csr_write_sptbr((uintptr_t) ttb0_base >> 10);

	// https://people.eecs.berkeley.edu/~krste/papers/riscv-priv-spec-1.7.pdf
	// 3.1.6 Virtualization Management Field in mstatus Register
	// Table 3.3: Encoding of virtualization management field VM[4:0]

//	{
//		uint_xlen_t v = csr_read_mstatus();
//		v &= ~ ((uint_xlen_t) 0x1F) << 24;	// VM[4:0]
//		v |= ((uint_xlen_t) 0x08) << 24;	// Set Page-based 32-bit virtual addressing.
//		//csr_write_mstatus(v);
//	}


	// 15.1.2 M-mode exception configuration register group
	// https://riscv.org/wp-content/uploads/2019/08/riscv-privileged-20190608-1.pdf


#endif
	PRINTF("sysinit_ttbr_initialize done.\n");
}

#elif defined (__CORTEX_M)

void
sysinit_ttbr_initialize(void)
{
}

#endif /* CPUSTYLE_R7S721 */

