/*
 * platform_def.h
 *
 *  Created on: May 9, 2023
 *      Author: Gena
 */

#ifndef SRC_SDRAM_STM32MP1_PLATFORM_DEF_H_
#define SRC_SDRAM_STM32MP1_PLATFORM_DEF_H_

#include "formats.h"

#include BOARD_DDR_CONFIG


#define VERBOSE PRINTF
//#define VERBOSE(...) // PRINTF
#define ERROR PRINTF
#define INFO PRINTF
void panic(void);

/* DDR power initializations */
enum ddr_type {
	STM32MP_DDR3,
	STM32MP_LPDDR2,
	STM32MP_LPDDR3
};


static inline void
mmio_write_32(uintptr_t addr, uint32_t value)
{
	volatile uint32_t * const reg = (volatile uint32_t * const) addr;
	* reg = value;
	//(void) * reg;
}

static inline uint32_t
mmio_read_32(uintptr_t addr)
{
	volatile uint32_t * const reg = (volatile uint32_t * const) addr;
	return * reg;
}

static void inline
mmio_clrbits_32(uintptr_t addr, uint32_t mask)
{
	volatile uint32_t * const reg = (volatile uint32_t * const) addr;
	* reg &= ~ mask;
	//(void) * reg;
}

static inline void
mmio_setbits_32(uintptr_t addr, uint32_t mask)
{
	volatile uint32_t * const reg = (volatile uint32_t * const) addr;
	* reg |= mask;
	//(void) * reg;
}


static inline void
mmio_clrsetbits_32(uintptr_t addr, uint32_t cmask, uint32_t smask)
{
	volatile uint32_t * const reg = (volatile uint32_t * const) addr;
	* reg = (* reg & ~ cmask) | smask;
	//(void) * reg;
}


/*
#define INT8_C(x)  x
#define INT16_C(x) x
#define INT32_C(x) x
#define INT64_C(x) x ## LL

#define UINT8_C(x)  x
#define UINT16_C(x) x
#define UINT32_C(x) x ## U
#define UINT64_C(x) x ## ULL

#define INTMAX_C(x)  x ## LL
#define UINTMAX_C(x) x ## ULL
*/

#define IS_POWER_OF_TWO(x)			\
	(((x) & ((x) - 1)) == 0)

#define SIZE_FROM_LOG2_WORDS(n)		(4 << (n))

#define BIT_32(nr)			(U(1) << (nr))
#define BIT_64(nr)			(ULL(1) << (nr))

#ifdef __aarch64__
#define BIT				BIT_64
#else
#define BIT				BIT_32
#endif

#define ULL(v) ((unsigned long long) (v))
#define UL(v) ((unsigned long) (v))
#define U(v) ((unsigned) (v))

/* DDR configuration */
#define STM32MP_DDR_BASE		U(0xC0000000)
#define STM32MP_DDR_MAX_SIZE		U(0x40000000)	/* Max 1GB */
#ifdef AARCH32_SP_OPTEE
#define STM32MP_DDR_S_SIZE		U(0x01E00000)	/* 30 MB */
#define STM32MP_DDR_SHMEM_SIZE		U(0x00200000)	/* 2 MB */
#endif


/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h. For example
 * GENMASK_64(39, 21) gives us the 64bit vector 0x000000ffffe00000.
 */
#if defined(__LINKER__) || defined(__ASSEMBLER__)
#define GENMASK_32(h, l) \
	(((0xFFFFFFFF) << (l)) & (0xFFFFFFFF >> (32 - 1 - (h))))

#define GENMASK_64(h, l) \
	((~0 << (l)) & (~0 >> (64 - 1 - (h))))
#else
#define GENMASK_32(h, l) \
	(((~UINT32_C(0)) << (l)) & (~UINT32_C(0) >> (32 - 1 - (h))))

#define GENMASK_64(h, l) \
	(((~UINT64_C(0)) << (l)) & (~UINT64_C(0) >> (64 - 1 - (h))))
#endif

#ifdef __aarch64__
#define GENMASK				GENMASK_64
#else
#define GENMASK				GENMASK_32
#endif

#define EINVAL 22


/********************************************************************
 * A simple timer driver providing synchronous delay functionality.
 * The driver must be initialized with a structure that provides a
 * function pointer to return the timer value and a clock
 * multiplier/divider. The ratio of the multiplier and the divider is
 * the clock period in microseconds.
 ********************************************************************/

typedef struct timer_ops {
	uint32_t (*get_timer_value)(void);
	uint32_t clk_mult;
	uint32_t clk_div;
} timer_ops_t;

//static inline uint64_t timeout_cnt_us2cnt(uint32_t us)
//{
//	return ((uint64_t)us * (uint64_t)read_cntfrq_el0()) / 1000000ULL;
//}

static inline uint64_t timeout_init_us(uint32_t us)
{
//	uint64_t cnt = timeout_cnt_us2cnt(us);
//
//	cnt += read_cntpct_el0();
//
//	return cnt;
	return 0;
}

static inline int timeout_elapsed(uint64_t expire_cnt)
{
	return 0;//read_cntpct_el0() > expire_cnt;
}

#define mdelay(msec) do { local_delay_ms(msec); } while (0)
#define udelay(usec) do { local_delay_ms(usec); } while (0)
void timer_init(const timer_ops_t *ops_ptr);

#endif /* SRC_SDRAM_STM32MP1_PLATFORM_DEF_H_ */
