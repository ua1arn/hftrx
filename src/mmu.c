/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints
#include "utils.h"	// peek/poke

//#define MMUUSE4KPAGES 1

#if ! LINUX_SUBSYSTEM

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

#if defined (__CORTEX_M)
	#if CPUSTYLE_STM32H7XX

	// MPU initialize
	static void lowlevel_stm32h7xx_mpu_initialize(void)
	{
		/* Disables the MPU */
		MPU->CTRL = (MPU->CTRL & ~ (MPU_CTRL_ENABLE_Msk)) |
			0 * MPU_CTRL_ENABLE_Msk |
			0;



	#define INNER_NORMAL_WB_RWA_TYPE(x)   (( 0x04 << MPU_RASR_TEX_Pos ) | ( DISABLE  << MPU_RASR_C_Pos ) | ( ENABLE  << MPU_RASR_B_Pos )  | ( x << MPU_RASR_S_Pos ))
	#define INNER_NORMAL_WB_NWA_TYPE(x)   (( 0x04 << MPU_RASR_TEX_Pos ) | ( ENABLE  << MPU_RASR_C_Pos )  | ( ENABLE  << MPU_RASR_B_Pos )  | ( x << MPU_RASR_S_Pos ))
	#define STRONGLY_ORDERED_SHAREABLE_TYPE      (( 0x00 << MPU_RASR_TEX_Pos ) | ( DISABLE << MPU_RASR_C_Pos ) | ( DISABLE << MPU_RASR_B_Pos ))     // DO not care //
	#define SHAREABLE_DEVICE_TYPE                (( 0x00 << MPU_RASR_TEX_Pos ) | ( DISABLE << MPU_RASR_C_Pos ) | ( ENABLE  << MPU_RASR_B_Pos ))     // DO not care //


		// SRAM
		/* Set the Region base address and region number */
		MPU->RBAR = D1_AXISRAM_BASE | MPU_RBAR_VALID_Msk | 0x00;
		MPU->RASR =
			(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
			(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
			(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
			(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
			(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
			(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
			(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
			(0x12 << MPU_RASR_SIZE_Pos) |	// Size 512 kB
			(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
			0;
		// ITCM
		/* Set the Region base address and region number */
		MPU->RBAR = D1_ITCMRAM_BASE | MPU_RBAR_VALID_Msk | 0x01;
		MPU->RASR =
			(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
			(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
			(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
			(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
			(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
			(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
			(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
			(0x0F << MPU_RASR_SIZE_Pos) |	// Size 64 kB
			(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
			0;
		// DTCM
		/* Set the Region base address and region number */
		MPU->RBAR = D1_DTCMRAM_BASE | MPU_RBAR_VALID_Msk | 0x02;
		MPU->RASR =
			(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
			(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
			(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
			(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
			(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
			(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
			(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
			(0x10 << MPU_RASR_SIZE_Pos) |	// Size 128 kB
			(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
			0;
		// FLASH
		/* Set the Region base address and region number */
		MPU->RBAR = D1_AXIFLASH_BASE | MPU_RBAR_VALID_Msk | 0x03;
		MPU->RASR =
			(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
			(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
			(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
			(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
			(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
			(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
			(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
			(0x14 << MPU_RASR_SIZE_Pos) |	// Size 2 MB
			(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
			0;
		// DEVICE
		/* Set the Region base address and region number */
		MPU->RBAR = PERIPH_BASE | MPU_RBAR_VALID_Msk | 0x04;
		MPU->RASR =
			(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
			(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
			(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
			(0x00 << MPU_RASR_C_Pos)    |	// IsCacheable
			(0x00 << MPU_RASR_B_Pos)    |	// IsBufferable
			(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
			(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
			(0x1B << MPU_RASR_SIZE_Pos) |	// Size 256 MB
			(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
			0;
		/* Enables the MPU */
		MPU->CTRL = (MPU->CTRL & ~ (MPU_CTRL_ENABLE_Msk)) |
			1 * MPU_CTRL_ENABLE_Msk |
			0;
	}

	#endif /* CPUSTYLE_STM32H7XX */

#elif defined (__aarch64__)
// 13.3 Memory attributes

// Also see TCR_EL3 parameter
#define CACHEATTR_NOCACHE 0x00		// Non-cacheable
#define CACHEATTR_WB_WA_CACHE 0x01	// Write-Back Write-Allocate Cacheable
//#define CACHEATTR_WT_NWA_CACHE 0x02	// Write-Through Cacheable
//#define CACHEATTR_WB_NWA_CACHE 0x03	// Write-Back no Write-Allocate Cacheable

// Lower attributes
#define AARCH64_LOWER_ATTR(AttrIndx) ( \
	0x01 * (UINT32_C(1) << 10) |	/*  AF */ \
	0x03 * (UINT32_C(1) << 8) |		/* SH[1:0] */ \
	0x01 * (UINT32_C(1) << 6) |		/* AP[2:1] (AP = 0x02) */ \
	0x00 * (UINT32_C(1) << 5) |		/* NS */ \
	(AttrIndx & 0x07) * (UINT32_C(1) << 2) | /* AttrIndx[2:0] */ \
	0)
#define AARCH64_UPPER_ATTR 0

enum aarch64_attrindex
{
	AARCH64_ATTR_INDEX_CACHED = 0,
	AARCH64_ATTR_INDEX_NCACHED,
	AARCH64_ATTR_INDEX_DEVICE

};

static uint_fast64_t arch64_mcached(uint_fast64_t addr, int ro, int xn)
{
	return AARCH64_UPPER_ATTR |
			(addr & ~ UINT64_C(0x0FFFFF)) |
			AARCH64_LOWER_ATTR(AARCH64_ATTR_INDEX_CACHED) |
			0x01;
}
static uint_fast64_t arch64_mncached(uint_fast64_t addr, int ro, int xn)
{
	return AARCH64_UPPER_ATTR |
			(addr & ~ UINT64_C(0x0FFFFF)) |
			AARCH64_LOWER_ATTR(AARCH64_ATTR_INDEX_NCACHED) |
			0x01;
}
static uint64_t arch64_mdevice(uint_fast64_t addr)
{
	return AARCH64_UPPER_ATTR
			| (addr & ~ UINT64_C(0x0FFFFF)) |
			AARCH64_LOWER_ATTR(AARCH64_ATTR_INDEX_DEVICE) |
			0x01;
}
// Next level table
// DDI0487_I_a_a-profile_architecture_reference_manual.pdf
// D8.3.1 Table Descriptor format
static uint_fast64_t arch64_mtable(uint_fast64_t addr)
{
	return (addr & ~ UINT64_C(0x0FFF)) |
			0x03;
}
static uint_fast64_t arch64_mnoaccess(uint_fast64_t addr)
{
	return 0;
}

static const getmmudesc_t arch64_table_2M =
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

static const getmmudesc_t arch32_table_1M =
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

static const getmmudesc_t arch32_table_4k =
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


static const getmmudesc_t rv64_table_4k =
{
	.mcached = rv64_4k_mcached,
	.mncached = rv64_4k_mncached,
	.mdevice = rv64_4k_mdevice,
	.mnoaccess = rv64_4k_mnoaccess,
	.mtable = rv64_4k_mtable
};


#if 0//CPUSTYLE_RISCV

static void ttb_level1_xk_initialize(const getmmudesc_t * arch, uint_fast64_t (* accessbits)(const getmmudesc_t * arch, uint_fast64_t a, int ro, int xn), const uint_fast32_t pagesize)
{
	// When the page table size is set to 4 KB, 2 MB, or 1 GB, the page table is indexed by 3, 2, or 1 times, respectively.
	uintptr_t address = 0;
	uintptr_t addrstep = UINT64_C(1) << 21;	// 2 MB
	unsigned i;
	for (i = 0; i < ARRAY_SIZE(xlevel1_pagetable_u64); ++ i)
	{
		level1_pagetable_u64 [i] =
				//((address >> 12) & 0x1FF) * (UINT64_C(1) << 10) |	// 9 bits PPN [0], 4 KB granulation
				((address >> 21) & 0x1FF) * (UINT64_C(1) << 19) |	// 9 bits PPN [1]
				//((address >> 36) & 0x7FF) * (UINT64_C(1) << 28) |	// 11 bits PPN [2]
				NCRAM_ATTRS |
				0;
		address += addrstep;
	}
}

static void ttb_level0_xk_initialize(const getmmudesc_t * arch, uint_fast64_t (* accessbits)(const getmmudesc_t * arch, uint_fast64_t a, int ro, int xn), const uint_fast32_t pagesize, uint_fast64_t nextlevel)
{
	// Pointe to 1 GB pages
	unsigned i;
	for (i = 0; i < ARRAY_SIZE(ttb0_base_u64); ++ i)
	{
		uintptr_t address = (uintptr_t) (xlevel1_pagetable_u64 + 512 * i) | 0x03;
		//uintptr_t address = 1 * (UINT64_C(1) << 30) * i;
		xttb0_base_u64 [i] =
			((address >> 12) & 0x1FF) * (UINT64_C(1) << 10) |	// 9 bits PPN [0], 4 KB granulation
			//((address >> 24) & 0x1FF) * (UINT64_C(1) << 19) |	// 9 bits PPN [1]
			//((address >> 36) & 0x7FF) * (UINT64_C(1) << 28) |	// 11 bits PPN [2]
			TABLE_ATTRS |
			0;
	}
}

#endif /* CPUSTYLE_RISCV */

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

#endif /* __CORTEX_A */

typedef struct mmulayout_tag
{
	const getmmudesc_t * arch;
	uintptr_t phyaddr;	// Начальное значение
	uint32_t pagesize;	// размер страниц
	uint32_t pagecount;
	uint8_t * table;
	unsigned (* poke)(uint8_t * b, uint_fast64_t v);
	unsigned flag;	// 0 - дескрипторы памяти, 1 - таблицы (mtable)
} mmulayout_t;

static void fillmmu(const mmulayout_t * p, unsigned n, uint_fast64_t (* accessbits)(const getmmudesc_t * arch, uint_fast64_t a, int ro, int xn))
{
	while (n --)
	{
		unsigned pages = p->pagecount;
		uint8_t * tb = p->table;	// table base
		uint_fast64_t phyaddr = p->phyaddr;
		for (; pages --; phyaddr += p->pagesize)
		{
			tb += p->poke(tb, p->flag ? p->arch->mtable(phyaddr) : accessbits(p->arch, phyaddr, 0, 0));
		}
		const ptrdiff_t cachesize = tb - p->table;
		dcache_clean_invalidate((uintptr_t) p->table, cachesize);
		++ p;
	}
//
//	if (n --)
//	{
//		unsigned pages = p->pagecount;
//		uint8_t * tb = p->table;	// table base
//		uint_fast64_t phyaddr = p->phyaddr;
//		for (; pages --; phyaddr += p->pagesize)
//		{
//			tb += p->poke(tb, arch->mtable(phyaddr));
//		}
//		const ptrdiff_t cachesize = tb - p->table;
//		dcache_clean_invalidate((uintptr_t) p->table, cachesize);
//		++ p;
//	}
}

static unsigned mmulayout_poke_u64_le(uint8_t * b, uint_fast64_t v)
{
	return USBD_poke_u64(b, v);
}

static unsigned mmulayout_poke_u32_le(uint8_t * b, uint_fast64_t v)
{
	return USBD_poke_u32(b, v);
}

#if defined (__CORTEX_M)

#elif CPUSTYLE_RISCV

	static RAMFRAMEBUFF __ALIGNED(4 * 1024) uint64_t xlevel1_pagetable_u64 [512 * 4];	// Used as PPN in SATP register
	static RAMFRAMEBUFF __ALIGNED(4 * 1024) uint64_t xttb0_base_u64 [512];	// Used as PPN in SATP register

	static const mmulayout_t mmuinfo [] =
	{
		{
			.arch = rv64_table_4k,
			.phyaddr = 0x00000000,	/* Начало физической памяти */
			.pagesize = (UINT32_C(1) << 21),	// 2M
			.pagecount = 512 * 4,
			.table = (uint8_t *) xlevel1_pagetable_u64,
			.poke = mmulayout_poke_u64_le,
			.flag = 0
		},
		{
			.arch = rv64_table_4k,
			.phyaddr = (uintptr_t) level1_pagetable_u64,
			.pagesize = (UINT32_C(1) << 12),	// 4k
			.pagecount = ARRAY_SIZE(xttb0_base_u64),
			.table = (uint8_t *) xttb0_base_u64,
			.poke = mmulayout_poke_u64_le,
			.flag = 1
		},
	};

#elif defined(__aarch64__)

	// Last x4 - for 34 bit address (16 GB address space)
	// Check TCR_EL3 setup
	// pages of 2 MB
	#define AARCH64_LEVEL0_SIZE (4 * 4)
	#define AARCH64_LEVEL1_SIZE (AARCH64_LEVEL0_SIZE * 512)
	static RAMFRAMEBUFF __ALIGNED(4 * 1024) uint8_t ttb0_base_u64 [AARCH64_LEVEL0_SIZE * 8];	// ttb0_base must be a 4KB-aligned address.
	static RAMFRAMEBUFF __ALIGNED(4 * 1024) uint8_t xxlevel1_pagetable_u64 [AARCH64_LEVEL1_SIZE * 8];	// ttb0_base must be a 4KB-aligned address.

	static const mmulayout_t mmuinfo [] =
	{
		{
			.arch = & arch64_table_2M,
			.phyaddr = 0x00000000,	/* Начало физической памяти */
			.pagesize = (UINT32_C(1) << 21),	// 2M
			.pagecount = AARCH64_LEVEL1_SIZE,
			.table = xxlevel1_pagetable_u64,
			.poke = mmulayout_poke_u64_le,
			.flag = 0
		},
		{
			.arch = & arch64_table_2M,
			.phyaddr = (uintptr_t) xxlevel1_pagetable_u64,
			.pagesize = (UINT32_C(1) << 12),	// 4k
			.pagecount = AARCH64_LEVEL0_SIZE,
			.table = ttb0_base_u64,
			.poke = mmulayout_poke_u64_le,
			.flag = 1
		},
	};
#else /* defined(__aarch64__) */

	#if MMUUSE4KPAGES

		// pages of 4 k
		#define AARCH32_4K_LEVEL0_SIZE (4096)
		#define AARCH32_4K_LEVEL1_SIZE (AARCH32_4K_LEVEL0_SIZE * 256)
		static RAMFRAMEBUFF __ALIGNED(16 * 1024) uint8_t ttb0_base_u32 [AARCH32_4K_LEVEL0_SIZE * 4];
		static RAMFRAMEBUFF __ALIGNED(1 * 1024) uint8_t level1_pagetable_u32 [AARCH32_4K_LEVEL1_SIZE * 4];	// вся физическая память страницами по 4 килобайта
		static const mmulayout_t mmuinfo [] =
		{
			{
				.arch = & arch32_table_4k,
				.phyaddr = 0x00000000,	/* Начало физической памяти */
				.pagesize = (UINT32_C(1) << 12),	// 4k
				.pagecount = AARCH32_4K_LEVEL1_SIZE,
				.table = level1_pagetable_u32,
				.poke = mmulayout_poke_u32_le,
				.flag = 0
			},
			{
				.arch = & arch32_table_4k,
				.phyaddr = (uintptr_t) level1_pagetable_u32,
				.pagesize = (UINT32_C(1) << 10),	// 1k
				.pagecount = AARCH32_4K_LEVEL0_SIZE,
				.table = ttb0_base_u32,
				.poke = mmulayout_poke_u32_le,
				.flag = 1
			},
		};

	#else /* MMUUSE4KPAGES */

		// pages of 1 MB
		#define AARCH32_1MB_LEVEL0_SIZE (4096)
		static RAMFRAMEBUFF __ALIGNED(16 * 1024) uint8_t ttb0_base_u32 [AARCH32_1MB_LEVEL0_SIZE * 4];	// вся физическая память страницами по 1 мегабайт
		static const mmulayout_t mmuinfo [] =
		{
			{
				.arch = & arch32_table_1M,
				.phyaddr = 0x00000000,	/* Начало физической памяти */
				.pagesize = (UINT32_C(1) << 20),	// 1M
				.pagecount = AARCH32_1MB_LEVEL0_SIZE,
				.table = ttb0_base_u32,
				.poke = mmulayout_poke_u32_le,
				.flag = 0
			},
		};

	#endif /* MMUUSE4KPAGES */

#endif /* defined(__aarch64__) */

#endif /* (__CORTEX_A != 0) || CPUSTYLE_ARM9 || CPUSTYLE_RISCV */

/* Один раз - инициализация таблиц в памяти */
void
sysinit_mmu_tables(void)
{
	//PRINTF("sysinit_mmu_tables\n");

#if (__CORTEX_A != 0) || CPUSTYLE_ARM9
	// MMU iniitialize
	fillmmu(mmuinfo, ARRAY_SIZE(mmuinfo), ttb_mempage_accessbits);

#elif CPUSTYLE_RISCV
	#warning To be implemented
	// RISC-V MMU initialize
	fillmmu(mmuinfo, ARRAY_SIZE(mmuinfo), ttb_mempage_accessbits);

#elif defined (__CORTEX_M)

#endif

	//PRINTF("sysinit_mmu_tables done.\n");
}

/* На каждом процессоре - Загрузка TTBR, инвалидация кеш памяти и включение MMU */
void
sysinit_ttbr_initialize(void)
{
	//PRINTF("sysinit_ttbr_initialize.\n");

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
#endif

#if defined(__aarch64__)

	ASSERT(((uintptr_t) ttb0_base_u64 & 0x0FFF) == 0); // 4 KB

	//__set_TTBR0_EL1((uintptr_t) ttb0_base);
	//__set_TTBR0_EL2((uintptr_t) ttb0_base);
	__set_TTBR0_EL3((uintptr_t) ttb0_base_u64);

	// DDI0500J_cortex_a53_r0p4_trm.pdf
	// 4.3.53 Translation Control Register, EL3
	const uint_fast32_t IRGN_attr = CACHEATTR_WB_WA_CACHE;	// Normal memory, Inner Write-Back Write-Allocate Cacheable.
	const uint_fast32_t RGN_attr = CACHEATTR_WB_WA_CACHE;	// Normal memory, Outer Write-Back Write-Allocate Cacheable.
	const unsigned aspacebits = 21 + __log2_up(AARCH64_LEVEL1_SIZE);	// pages of 2 MB
	uint_fast32_t tcrv =
			0x00 * (UINT32_C(1) << 14) | 	// TG0 TTBR0_EL3 granule size 0b00 4 KB
			0x03 * (UINT32_C(1) << 12) |	// 0x03 - Inner shareable
			RGN_attr * (UINT32_C(1) << 10) |	// Outer cacheability attribute
			IRGN_attr * (UINT32_C(1) << 8) |	// Inner cacheability attribute
			(64 - aspacebits) * (UINT32_C(1) << 0) |		// n=0..63. T0SZ=2^(64-n): n=28: 64GB, n=30: 16GB, n=32: 4GB, n=43: 2MB
			0;
	__set_TCR_EL3(tcrv);

	const uint_fast32_t mairv =
			0xFF * (UINT32_C(1) << (AARCH64_ATTR_INDEX_CACHED * 8)) |		// Normal Memory, Inner/Outer Write-back non-transient
			0x44 * (UINT32_C(1) << (AARCH64_ATTR_INDEX_NCACHED * 8)) |	// Normal memory, Inner/Outer Non-Cacheable
			0x00 * (UINT32_C(1) << (AARCH64_ATTR_INDEX_DEVICE * 8)) | 	// Device-nGnRnE memory
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

	ASSERT(((uintptr_t) ttb0_base_u32 & 0x3FFF) == 0);

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
			(uintptr_t) ttb0_base_u32 |	/* Translation table base 0 address, bits[31:x]. */
			((uint_fast32_t) !! (IRGN_attr & 0x01) << 6) |	// IRGN[0]
			((uint_fast32_t) !! (IRGN_attr & 0x02) << 0) |	// IRGN[1]
			(RGN_attr << 3) |	// RGN
			!1*(UINT32_C(1) << 5) |	// NOS - Not Outer Shareable bit - TEST for RAMNC
			1*(UINT32_C(1) << 1) |	// S - Shareable bit. Indicates the Shareable attribute for the memory associated with the translation table
			0);
#else /* WITHSMPSYSTEM */
	// TTBR0
	__set_TTBR0(
			(uintptr_t) ttb0_base_u32 |	/* Translation table base 0 address, bits[31:x]. */
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

	ASSERT(((uintptr_t) ttb0_base_u64 & 0x0FFF) == 0);
	mmu_flush_cache();
	const unsigned asid = 0;
	// 5.2.1.1 MMU address translation register (SATP)
	// When Mode is 0, the MMU is disabled. C906 supports only the MMU disabled and Sv39 modes
	const uint_fast64_t satp =
			//CSR_SATP_MODE_PHYS * (UINT64_C(1) << 60) | // MODE
			CSR_SATP_MODE_SV39 * (UINT64_C(1) << 60) | // MODE
			(asid  & UINT64_C(0xFFFF))* (UINT64_C(1) << 44) | // ASID
			(((uintptr_t) xttb0_base_u64 >> 12) & UINT64_C(0x0FFFFFFF)) * (UINT64_C(1) << 0) |	// PPN - 28 bit
			0;
	PRINTF("1 ttb0_base=%p" "\n", ttb0_base_u64);
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

#elif defined (__CORTEX_M)

#endif
	//PRINTF("sysinit_ttbr_initialize done.\n");
}

#endif /* ! LINUX_SUBSYSTEM */


