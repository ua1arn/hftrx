/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints
#include "utils.h"	// peek/poke


#if ! LINUX_SUBSYSTEM

// ОБщая для всех процессоров инициализация
void
sysinit_cache_initialize(void)
{
#if ! LINUX_SUBSYSTEM
	//PRINTF("dcache_rowsize=%u, icache_rowsize=%u\n", dcache_rowsize(), icache_rowsize());
	ASSERT(DCACHEROWSIZE == dcache_rowsize());
	ASSERT(ICACHEROWSIZE == icache_rowsize());
#if defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U)
//	PRINTF("GIC_BINARY_POINT=%u\n", GIC_BINARY_POINT);
//	PRINTF("GIC_GetBinaryPoint()=%u\n", (unsigned) GIC_GetBinaryPoint());
	ASSERT(GIC_BINARY_POINT == GIC_GetBinaryPoint());
#endif /* defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U) */
#endif /* ! LINUX_SUBSYSTEM */

#if defined (__CORTEX_M)
	#if __ICACHE_PRESENT

		SCB_InvalidateICache();
		SCB_EnableICache();

	#endif /* __ICACHE_PRESENT */
	#if __DCACHE_PRESENT

		SCB_InvalidateDCache();
		SCB_EnableDCache();

	#endif /* __DCACHE_PRESENT */

	#if 0
		SCB_InvalidateBTAC();
		SCB_EnableBTAC();
	#endif

#elif (__CORTEX_A != 0) || CPUSTYLE_ARM9
	L1C_InvalidateDCacheAll();
	L1C_InvalidateICacheAll();
	L1C_InvalidateBTAC();
	L1C_EnableCaches();
	L1C_EnableBTAC();

#elif CPUSTYLE_F133

	// RISC-V cache initialize
	// https://riscv.org/wp-content/uploads/2016/07/riscv-privileged-v1.9-1.pdf#page=49


	//	7.3.1 L1 Cache Extension Register
	//	C906 L1 cache related extended registers are mainly divided into:
	//	• Cache Enable and Mode Configuration: The Machine Mode Hardware Configuration Register (mhcr) enables switching of instruction and data caches as well as write allocation and
	//	Configuration for writeback mode. The supervisor mode hardware configuration register (shcr) is a map of mhcr and is a read-only register.
	//	• Dirty entry cleanup and invalidation operations: The Machine Mode Cache Operation Register (mcor) can dirty and invalidate instruction and data caches
	//	operation.
	//	• Cache Reads: Machine Mode Cache Access Instruction Registers (mcins), Cache Access Index Registers (mcindex) and Cache
	//	Access to data register 0/1 (mcdata0/1), through which the read operation of the instruction and data caches can be realized.
	//	The specific control register description can refer to the machine mode processor control and status extension register group.


	//	IE-Icache enable bit:
	//	• When IE=0, Icache is closed;
	//	• When IE=1, Icache is turned on.
	//	This bit will be reset to 1'b0.
	//	DE-Dcache enable bit:
	//	• When DE=0, Dcache is closed;
	//	• When DE=1, Dcache is on.
	//	This bit will be reset to 1'b0.
	//	WA - Cache Write Allocation Set Bits:
	//	• When WA=0, the data cache is in write non-allocate mode;
	//	• When WA=1, the data cache is in write allocate mode.
	//	This bit will be reset to 1'b0.
	//	WB - Cache Write Back Set Bits:
	//	• When WB=0, the data cache is in write through mode.
	//	• When WB=1, the data cache is in write back mode.
	//	C906 only supports write back mode, and WB is fixed to 1.
	//	RS-Address Return Stack Set Bits:
	//	• When RS=0, the return stack is closed;
	//	• When RS=1, the return stack is turned on.
	//	This bit will be reset to 1'b0.
	//	BPE - Allow Predictive Jump Set bit:
	//	• When BPE=0, predictive jumps are turned off;
	//	• When BPE=1, predictive jumps are turned on.
	//	This bit will be reset to 1'b0.
	//	BTB-Branch Target Prediction Enable Bit:
	//	• When BTB=0, branch target prediction is turned off.
	//	• When BTB=1, branch target prediction is on.
	//	This bit will be reset to 1'b0.
	//	WBR - Write Burst Enable Bit:
	//	• When WBR=0, write burst transfers are not supported.
	//	• When WBR=1, write burst transfers are supported.
	//	WBR is fixed to 1 in C906.


	//	https://github.com/DongshanPI/eLinuxCore_dongshannezhastu/blob/master/spl/arch/riscv/cpu/riscv64/mmu.c
	//	/*
	//	(0:1) When CACHE_SEL=2'b11, select instruction and data cache
	//	(4) When INV=1, the cache is invalidated
	//	(16) When BHT_INV=1, the data in the branch history table is invalidated
	//	(17) When TB_INV=1, the data in the branch target buffer is invalidated
	//	*/
	//	csr_write(CSR_MCOR, 0x70013);
	csr_write_mcor(0x70013);

	//
	//	/*
	//	(0) Icache is turned on when IE=1
	//	(1) Dcache is turned on when DE=1
	//	(2) When WA=1, the data cache is in write allocate mode (c906 is not supported)
	//	(3) When WB=1, the data cache is in writeback mode (c906 is fixed to 1)
	//	(4) When RS=1, return to the stack to open
	//	(5) When BPE=1, the prediction jump is turned on
	//	(6) When BTB=1, branch target prediction is enabled
	//	(8) Support write burst transmission write when WBR=1 (c906 fixed to 1)
	//	(12) When L0BTB=1, the prediction of the target of the first level branch is enabled
	//	*/
	//	csr_write(CSR_MHCR, 0x11ff);
	csr_write_mhcr(0x011FF);
	csr_write_mhcr(0x011FF & ~ 0x02);	// D-cache disable for tests
#warning D-cache disabled

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
	csr_set_bits_mxstatus(
			1 * (UINT32_C(1) << 15) |
			1 * (UINT32_C(1) << 16) |
			1 * (UINT32_C(1) << 17) |
			//1 * (UINT32_C(1) << 21) |
			1 * (UINT32_C(1) << 22) |
			0
			//0x638000
			);

	//
	//
	//	/*
	//	(2) DPLD=1, dcache prefetching is enabled
	//	(3,4,5,6,7) AMR=1, when a storage operation of three consecutive cache rows occurs, the storage operation of subsequent consecutive addresses is no longer written to L1Cache
	//	(8) IPLD=1ICACHE prefetch is enabled
	//	(9) LPE=1 cycle acceleration on
	//	(13,14) When DPLD is 2, 8 cache rows are prefetched
	//	*/
	//	csr_write(CSR_MHINT, 0x16e30c);
	csr_write_mhint(0x16E30C);

//	/*C906 will invalid all I-cache automatically when reset*/
//	/*you can invalid I-cache by yourself if necessarily*/
//	/*invalid I-cache*/
//	//	li x3, 0x33
//	//	csrc mcor, x3
//	//	li x3, 0x11
//	//	csrs mcor, x3
//
//	csr_clr_bits_mcor(0x0033);
//	csr_set_bits_mcor(0x0011);
//
//	// Allow Predictive Jump (C906-specific)
//	csr_set_bits_mhcr((uint_xlen_t) 1u << 5);	// BPE
//	// Branch Target Prediction Enable (C906-specific)
//	csr_set_bits_mhcr((uint_xlen_t) 1u << 6);	// BTB
//	// RS-Address Return Stack (C906-specific)
//	csr_set_bits_mhcr((uint_xlen_t) 1u << 4);	// RS
//	// enable I-cache (C906-specific)
//	csr_set_bits_mhcr((uint_xlen_t) 1u << 0);	// IE
//
//
//	/*C906 will invalid all D-cache automatically when reset*/
//	/*you can invalid D-cache by yourself if necessarily*/
//	/*invalid D-cache*/
////	li x3, 0x33
////	csrc mcor, x3
////	li x3, 0x12
////	csrs mcor, x3
//	csr_clr_bits_mcor(0x0033);
//	csr_set_bits_mcor(0x0012);
//
	// enable D-cache Write-allocate (C906-specific)
//	csr_set_bits_mhcr((uint_xlen_t) 1u << 2);	// WA
//	// enable D-cache Write-back (C906-specific)
//	csr_set_bits_mhcr((uint_xlen_t) 1u << 3);	// WB
//	// enable D-cache (C906-specific)
//	csr_set_bits_mhcr((uint_xlen_t) 1u << 1);	// DE
//
//	// Disable D-cache (C906-specific)
//	csr_clr_bits_mhcr((uint_xlen_t) 1u << 1);	// DE
	// 0x0108:
//	PRINTF("MHCR=%08X\n", (unsigned) csr_read_mhcr());

#endif /* CPUSTYLE_RISCV */
}

/* инициадизации кеш-памяти, специфические для CORE0 */
void
sysinit_cache_L2_initialize(void)
{
#if (__L2C_PRESENT == 1) && defined (PL310_DATA_RAM_LATENCY)
	L2C_Disable();
	* (volatile uint32_t *) (L2C_310_BASE + 0x010C) = PL310_DATA_RAM_LATENCY;	// reg1_data_ram_control
	* (volatile uint32_t *) (L2C_310_BASE + 0x0108) = PL310_TAG_RAM_LATENCY;	// reg1_tag_ram_control
#endif /* (__L2C_PRESENT == 1) */
#if (__L2C_PRESENT == 1)
	// Enable Level 2 Cache
	L2C_InvAllByWay();
	L2C_Enable();
#endif
}


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

#else /* ! LINUX_SUBSYSTEM */


#endif /* ! LINUX_SUBSYSTEM */

