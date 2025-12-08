/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints
#include "utils.h"	// peek/poke

#include <limits.h>

//#define MMUUSE4KPAGES (1 && defined (__ARM_ARCH) && ! defined (__aarch64__))
//#define MMUUSE16MPAGES (1 && defined (__ARM_ARCH) && ! defined (__aarch64__))
#define MMUUSE2MPAGES (1 && defined (__ARM_ARCH) && defined (__aarch64__))
#define MMUUSE1MPAGES (1 && defined (__ARM_ARCH) && ! defined (__aarch64__))

#if ! LINUX_SUBSYSTEM

#if ! defined (HARDWARE_ADDRSPACE_GB)
	#define HARDWARE_ADDRSPACE_GB 4		// Размер адресного пространства (для 4 ГБ памяти надо 8 ГБ - базовый адрес ОЗУ 0x40000000).
#endif /* ! defined (HARDWARE_ADDRSPACE_GB) */

#if (__CORTEX_A != 0) || CPUSTYLE_ARM9 || CPUSTYLE_RISCV

#if WITHGPUHW
// GPU MMU

static unsigned gpu_mali400_4k_mcached(uint8_t * b, uint_fast64_t addr, int ro, int xn)
{
	return USBD_poke_u32(b, UINT64_C(0));//TTB_SMALLSECTION_AARCH32_4K_CACHED(addr, ro, xn);
}
static unsigned gpu_mali400_4k_mncached(uint8_t * b, uint_fast64_t addr, int ro, int xn)
{
	return USBD_poke_u32(b, UINT64_C(0));//TTB_SMALLSECTION_AARCH32_4K_NCACHED(addr, ro, xn);
}
static unsigned gpu_mali400_4k_mdevice(uint8_t * b, uint_fast64_t addr)
{
	return USBD_poke_u32(b, UINT64_C(0));//TTB_SMALLSECTION_AARCH32_4K_DEVICE(addr);
}
static unsigned gpu_mali400_4k_mnoaccess(uint8_t * b, uint_fast64_t addr)
{
	return USBD_poke_u32(b, UINT32_C(0));
}
// Next level table
static unsigned gpu_mali400_4k_mtable(uint8_t * b, uint_fast64_t addr, int level)
{
	// 1KB granulation address
	return USBD_poke_u32(b, UINT64_C(0));//TTB_AARCH32_PAGETABLE(addr);	// First-level table entry - Page table
}

static const getmmudesc_t gpu_mali400_table4k =
{
	.mcached = gpu_mali400_4k_mcached,
	.mncached = gpu_mali400_4k_mncached,
	.mdevice = gpu_mali400_4k_mdevice,
	.mnoaccess = gpu_mali400_4k_mnoaccess,
	.mtable = gpu_mali400_4k_mtable
};

#endif /* WITHGPUHW */

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

#elif (__CORTEX_A != 0)
// 13.3 Memory attributes

// Also see TCR_EL3 parameter
#define AARCH64_CACHEATTR_NOCACHE 0x00		// Non-cacheable
#define AARCH64_CACHEATTR_WB_WA_CACHE 0x01	// Write-Back Write-Allocate Cacheable
//#define AARCH64_CACHEATTR_WT_NWA_CACHE 0x02	// Write-Through Cacheable
//#define AARCH64_CACHEATTR_WB_NWA_CACHE 0x03	// Write-Back no Write-Allocate Cacheable

// G5.7 Memory region attributes
// Table G5-16 SH[1:0] field encoding for Normal memory, Long-descriptor format
#define AARCH64_SH_MEMORY 0x03		// 0x03 - inner shareable
#define AARCH64_SH_DEVICE 0x02		// 0x02 - Outer shareable

// Lower attributes
#define AARCH64_LOWER_ATTR(SHattr, AttrIndx) ( \
	0x01 * (UINT64_C(1) << 10) |	/*  AF */ \
	(SHattr) * (UINT64_C(1) << 8) |		/* SH[1:0] */ \
	0x01 * (UINT64_C(1) << 6) |		/* AP[2:1] (AP = 0x02) */ \
	0x00 * (UINT64_C(1) << 5) |		/* NS */ \
	(AttrIndx & 0x07) * (UINT64_C(1) << 2) | /* AttrIndx[2:0] */ \
	0)
#define AARCH64_UPPER_ATTR (UINT64_C(0))

enum aarch64_attrindex
{
	AARCH64_ATTR_INDEX_CACHED = 0,
	AARCH64_ATTR_INDEX_NCACHED,
	AARCH64_ATTR_INDEX_DEVICE

};

//#elif (__CORTEX_A != 0)

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

#define AARCH32_APRWval 		0x03	/* Full access */
#define AARCH32_APROval 		0x06	/* All write accesses generate Permission faults */
#define AARCH32_DOMAINval		0//0x0F
#define AARCH32_PXNval			0x01	/* PXN  */

/* Table B3-10 TEX, C, and B encodings when TRE == 0 */

/* Outer and Inner Write-Back, Write-Allocate */
// Cacheable memory attributes, without TEX remap
// DDI0406C_d_armv7ar_arm.pdf
// Table B3-11 Inner and Outer cache attribute encoding

#define AARCH32_MKATTR_TEXval(cacheattr) (0x04u | ((cacheattr) & 0x03u))
#define AARCH32_MKATTR_Cval(cacheattr) (!! ((cacheattr) & 0x02u))
#define AARCH32_MKATTR_Bval(cacheattr) (!! ((cacheattr) & 0x01u))

// Also see __set_TTBR0 parameter
#define AARCH32_CACHEATTR_NOCACHE 0x00		// Non-cacheable
#define AARCH32_CACHEATTR_WB_WA_CACHE 0x01	// Write-Back, Write-Allocate
//#define CACHEATTR_WT_NWA_CACHE 0x02	// Write-Through, no Write-Allocate
//#define CACHEATTR_WB_NWA_CACHE 0x03	// Write-Back, no Write-Allocate

/* атрибуты для разных областей памяти (при TEX[2]=1 способе задания) */
#define AARCH32_RAM_ATTRS AARCH32_CACHEATTR_WB_WA_CACHE
//#define AARCH32_RAM_ATTRS AARCH32_CACHEATTR_WB_NWA_CACHE
#define AARCH32_DEVICE_ATTRS AARCH32_CACHEATTR_NOCACHE
#define AARCH32_NCRAM_ATTRS AARCH32_CACHEATTR_NOCACHE

#define AARCH32_TEXval_RAM		AARCH32_MKATTR_TEXval(AARCH32_RAM_ATTRS)	// Define the Outer cache attribute
#define AARCH32_Cval_RAM		AARCH32_MKATTR_Cval(AARCH32_RAM_ATTRS)		// Define the Inner cache attribute
#define AARCH32_Bval_RAM		AARCH32_MKATTR_Bval(AARCH32_RAM_ATTRS)		// Define the Inner cache attribute

#if WITHSMPSYSTEM
	#define AARCH32_SHAREDval_RAM 1		// required for ldrex.. and strex.. functionality
#else /* WITHSMPSYSTEM */
	#define AARCH32_SHAREDval_RAM 0		// If non-zero, Renesas Cortex-A9 hung by buffers
#endif /* WITHSMPSYSTEM */

#define AARCH32_TEXval_NCRAM	AARCH32_MKATTR_TEXval(AARCH32_NCRAM_ATTRS)	// Define the Outer cache attribute
#define AARCH32_Cval_NCRAM		AARCH32_MKATTR_Cval(AARCH32_NCRAM_ATTRS)	// Define the Inner cache attribute
#define AARCH32_Bval_NCRAM		AARCH32_MKATTR_Bval(AARCH32_NCRAM_ATTRS)	// Define the Inner cache attribute

#if WITHSMPSYSTEM
	#define AARCH32_SHAREDval_NCRAM 1		// required for ldrex.. and strex.. functionality
#else /* WITHSMPSYSTEM */
	#define AARCH32_SHAREDval_NCRAM 0		// If non-zero, Renesas Cortex-A9 hung by buffers
#endif /* WITHSMPSYSTEM */

#if 1
	/* Shareable Device */
	#define AARCH32_TEXval_DEVICE       0x00
	#define AARCH32_Cval_DEVICE         0
	#define AARCH32_Bval_DEVICE         1
	#define AARCH32_SHAREDval_DEVICE 	0
#else
	/* Shareable Device */
	#define AARCH32_TEXval_DEVICE	AARCH32_MKATTR_TEXval(AARCH32_DEVICE_ATTRS)	// Define the Outer cache attribute
	#define AARCH32_Cval_DEVICE		AARCH32_MKATTR_Cval(AARCH32_DEVICE_ATTRS)	// Define the Inner cache attribute
	#define AARCH32_Bval_DEVICE		AARCH32_MKATTR_Bval(AARCH32_DEVICE_ATTRS)	// Define the Inner cache attribute

	#if WITHSMPSYSTEM
		#define AARCH32_SHAREDval_DEVICE 1		// required for ldrex.. and strex.. functionality
	#else /* WITHSMPSYSTEM */
		#define AARCH32_SHAREDval_DEVICE 0		// If non-zero, Renesas Cortex-A9 hung by buffers
	#endif /* WITHSMPSYSTEM */
#endif

// For v7 - See B3.5.2 in DDI0406C_C_arm_architecture_reference_manual.pdf
//  B3.5.1  Short-descriptor translation table format descriptors
//  Short-descriptor translation table first-level descriptor formats
// 1MB memory region
#define	TTB_SECTION_AARCH32_1M(pa, TEXv, Bv, Cv, DOMAINv, SHAREDv, APv, XNv) ( \
		(((pa) >> 20) & 0xFFF) * (UINT32_C(1) << 20) |	/* Section base address, PA[31:20] */ \
		0 * (UINT32_C(1) << 19) |	/* NS */ \
		0 * (UINT32_C(1) << 18) |	/* v7: 0, v8: 0 - section, 1 - supersection */ \
		0 * (UINT32_C(1) << 17) |	/* nG */ \
		!! (SHAREDv) * (UINT32_C(1) << 16) |	/* S */ \
		(((APv) >> 2) & 0x01) * (UINT32_C(1) << 15) |	/* AP[2] */ \
		((TEXv) & 0x07) * (UINT32_C(1) << 12) |	/* TEX[2:0] */ \
		(((APv) >> 0) & 0x03) * (UINT32_C(1) << 10) |	/* AP[1..0] */ \
		0 * (UINT32_C(1) << 9) |	/* implementation defined */ \
		(DOMAINv) * (UINT32_C(1) << 5) |	/* DOMAIN */ \
		!! (XNv) * (UINT32_C(1) << 4) |	/* XN The Execute-never bit. */ \
		!! (Cv) * (UINT32_C(1) << 3) |	/* C */ \
		!! (Bv) * (UINT32_C(1) << 2) |	/* B */ \
		1 * (UINT32_C(1) << 1) |	/* 1, Section or Supersection */ \
		!! (AARCH32_PXNval) * (UINT32_C(1) << 0) |	/* PXN */ \
		0 \
	)

//  B3.5.1 Short-descriptor translation table format descriptors
//  Short-descriptor translation table first-level descriptor formats
// 16MB memory region
// Memory described by Supersections is in domain 0
#define	TTB_SUPERSECTION_AARCH32_16M(pa, TEXv, Bv, Cv, SHAREDv, APv, XNv) ( \
		(((pa) >> 24) & 0xFF) * (UINT32_C(1) << 24) |	/* Supersection base address, PA[31:24] */ \
		(((pa) >> 32) & 0x0F) * (UINT32_C(1) << 20) |	/* Extended base address, PA[35:32] */ \
		0 * (UINT32_C(1) << 19) |	/* NS */ \
		1 * (UINT32_C(1) << 18) |	/* 0 - section, 1 - supersection */ \
		0 * (UINT32_C(1) << 17) |	/* nG */ \
		!! (SHAREDv) * (UINT32_C(1) << 16) |	/* S */ \
		(((APv) >> 2) & 0x01) * (UINT32_C(1) << 15) |	/* AP[2] */ \
		((TEXv) & 0x07) * (UINT32_C(1) << 12) |	/* TEX[2:0] */ \
		(((APv) >> 0) & 0x03) * (UINT32_C(1) << 10) |	/* AP[1..0] */ \
		0 * (UINT32_C(1) << 9) |	/* implementation defined */ \
		(((pa) >> 36) & 0x0F) * (UINT32_C(1) << 5) |	/*  Extended base address, PA[39:36] */ \
		!! (XNv) * (UINT32_C(1) << 4) |	/* XN The Execute-never bit. */ \
		!! (Cv) * (UINT32_C(1) << 3) |	/* C */ \
		!! (Bv) * (UINT32_C(1) << 2) |	/* B */ \
		1 * (UINT32_C(1) << 1) |	/* 1, Section or Supersection */ \
		!! (AARCH32_PXNval) * (UINT32_C(1) << 0) |	/* PXN */ \
		0 \
	)

// For v8 - See G5.4 The VMSAv8-32 Short-descriptor translation table format
// in DDI0487_I_a_a-profile_architecture_reference_manual.pdf
// Section (1 MB 1MB memory region)


// B3.5.1 Short-descriptor translation table format descriptors
// Short-descriptor translation table first-level descriptor formats

#define	TTB_AARCH32_PAGETABLE_RAW(pa, TEXv, Bv, Cv, DOMAINv, SHAREDv, APv, XNv) ( \
		(((pa) >> 10) & 0x3FFFFF) * (UINT32_C(1) << 10) |	/* Page table base address, bits[31:10] */ \
		0 * (UINT32_C(1) << 9) |	/* implementation defined */ \
		(DOMAINv) * (UINT32_C(1) << 5) |	/* DOMAIN */ \
		0 * (UINT32_C(1) << 3) |	/* NS */ \
		!! (AARCH32_PXNval) * (UINT32_C(1) << 2) |	/* PXN */ \
		0x01 * (UINT32_C(1) << 0) |	/* bits [1:0]*/ \
		0 \
	)

// B3.5.1 Short-descriptor translation table format descriptors
// Short-descriptor translation table second-level descriptor formats

// Small page (4KB memory page)
#define	TTB_SMALLSECTION_AARCH32_4K(pa, TEXv, Bv, Cv, DOMAINv, SHAREDv, APv, XNv) ( \
		(((pa) >> 12) & 0xFFFFF) * (UINT32_C(1) << 12) |	/* Section base address, PA[31:12] */ \
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

#define	TTB_SUPERSECTION_AARCH32_16M_NCACHED(addr, ro, xn)	TTB_SUPERSECTION_AARCH32_16M((addr), AARCH32_TEXval_NCRAM, AARCH32_Bval_NCRAM, AARCH32_Cval_NCRAM, AARCH32_SHAREDval_NCRAM, (ro) ? AARCH32_APROval : AARCH32_APRWval, (xn) != 0)
#define	TTB_SUPERSECTION_AARCH32_16M_CACHED(addr, ro, xn) 	TTB_SUPERSECTION_AARCH32_16M((addr), AARCH32_TEXval_RAM, AARCH32_Bval_RAM, AARCH32_Cval_RAM, AARCH32_SHAREDval_RAM, (ro) ? AARCH32_APROval : AARCH32_APRWval, (xn) != 0)
#define	TTB_SUPERSECTION_AARCH32_16M_DEVICE(addr) 			TTB_SUPERSECTION_AARCH32_16M((addr), AARCH32_TEXval_DEVICE, AARCH32_Bval_DEVICE, AARCH32_Cval_DEVICE, AARCH32_SHAREDval_DEVICE, AARCH32_APRWval, 1 /* XN=1 */)

#define	TTB_SECTION_AARCH32_1M_NCACHED(addr, ro, xn)	TTB_SECTION_AARCH32_1M((addr), AARCH32_TEXval_NCRAM, AARCH32_Bval_NCRAM, AARCH32_Cval_NCRAM, AARCH32_DOMAINval, AARCH32_SHAREDval_NCRAM, (ro) ? AARCH32_APROval : AARCH32_APRWval, (xn) != 0)
#define	TTB_SECTION_AARCH32_1M_CACHED(addr, ro, xn) 	TTB_SECTION_AARCH32_1M((addr), AARCH32_TEXval_RAM, AARCH32_Bval_RAM, AARCH32_Cval_RAM, AARCH32_DOMAINval, AARCH32_SHAREDval_RAM, (ro) ? AARCH32_APROval : AARCH32_APRWval, (xn) != 0)
#define	TTB_SECTION_AARCH32_1M_DEVICE(addr) 			TTB_SECTION_AARCH32_1M((addr), AARCH32_TEXval_DEVICE, AARCH32_Bval_DEVICE, AARCH32_Cval_DEVICE, AARCH32_DOMAINval, AARCH32_SHAREDval_DEVICE, AARCH32_APRWval, 1 /* XN=1 */)

#define	TTB_SMALLSECTION_AARCH32_4K_NCACHED(addr, ro, xn)	TTB_SMALLSECTION_AARCH32_4K((addr), AARCH32_TEXval_NCRAM, AARCH32_Bval_NCRAM, AARCH32_Cval_NCRAM, AARCH32_DOMAINval, AARCH32_SHAREDval_NCRAM, (ro) ? AARCH32_APROval : AARCH32_APRWval, (xn) != 0)
#define	TTB_SMALLSECTION_AARCH32_4K_CACHED(addr, ro, xn) 	TTB_SMALLSECTION_AARCH32_4K((addr), AARCH32_TEXval_RAM, AARCH32_Bval_RAM, AARCH32_Cval_RAM, AARCH32_DOMAINval, AARCH32_SHAREDval_RAM, (ro) ? AARCH32_APROval : AARCH32_APRWval, (xn) != 0)
#define	TTB_SMALLSECTION_AARCH32_4K_DEVICE(addr) 			TTB_SMALLSECTION_AARCH32_4K((addr), AARCH32_TEXval_DEVICE, AARCH32_Bval_DEVICE, AARCH32_Cval_DEVICE, AARCH32_DOMAINval, AARCH32_SHAREDval_DEVICE, AARCH32_APRWval, 1 /* XN=1 */)
// First-level table entry - Page table
#define	TTB_AARCH32_PAGETABLE(addr) 			TTB_AARCH32_PAGETABLE_RAW((addr), AARCH32_TEXval_DEVICE, AARCH32_Bval_DEVICE, AARCH32_Cval_DEVICE, AARCH32_DOMAINval, AARCH32_SHAREDval_DEVICE, AARCH32_APRWval, 1 /* XN=1 */)


// DDI0487_I_a_a-profile_architecture_reference_manual.pdf
// D8.3.2  Block descriptor and Page descriptor formats

// Granule size for the TTBR0_EL3. 0x00 4KB, 0x01 64KB, 0x02 16KB
// Обратить внимание на выравнивание ttb0_base
//	A translation table is required to be aligned to the size of the table. If a table contains fewer than
//	eight entries, it must be aligned on a 64 byte address boundary.

// aarch64: D17.2.133  TCR_EL3, Translation Control Register (EL3)
// aarch32: G8.2.164 TTBCR, Translation Table Base Control Register
static const unsigned vTG0 = 0x00;	// TCR_EL3.TG0

// Figure D8-14 Block descriptor formats
// 4KB, 16KB, and 64KB granules, 48-bit OA
//	For the 4KB granule size, the level 1 descriptor n is 30, and the level 2 descriptor n is 21.
//	For the 16KB granule size, the level 2 descriptor n is 25.
//	For the 64KB granule size, the level 2 descriptor n is 29

static uint_fast64_t aarch64_2M_addrmaskmem(uint_fast64_t addr)
{
	const uint_fast64_t mask48 = UINT64_C(0xFFFFFFFFFFFF);	// bits 47..0
	switch (vTG0)
	{
	case 0x00:	// 4KB
		return (addr >> 21 << 21) & mask48;
	case 0x02:	// 16KB
		return (addr >> 25 << 25) & mask48;
	case 0x01:	// 64KB
		return (addr >> 29 << 29) & mask48;
	}
}

// Figure D8-15 Page descriptor formats
static uint_fast64_t aarch64_2M_addrmasktable(uint_fast64_t addr)
{
	const uint_fast64_t mask48 = UINT64_C(0xFFFFFFFFFFFF);	// bits 47..0
	switch (vTG0)
	{
	case 0x00:	// 4KB
		return (addr >> 12 << 12) & mask48;
	case 0x02:	// 16KB
		return (addr >> 14 << 14) & mask48;
	case 0x01:	// 64KB
		return (addr >> 16 << 16) & mask48;
	}
}
static unsigned aarch64_2M_mcached(uint8_t * b, uint_fast64_t addr, int ro, int xn)
{
	return USBD_poke_u64(b,
			AARCH64_UPPER_ATTR |
			aarch64_2M_addrmaskmem(addr) |
			AARCH64_LOWER_ATTR(AARCH64_SH_MEMORY, AARCH64_ATTR_INDEX_CACHED) |
			0x01);
}
static unsigned aarch64_2M_mncached(uint8_t * b, uint_fast64_t addr, int ro, int xn)
{
	return USBD_poke_u64(b,
			AARCH64_UPPER_ATTR |
			aarch64_2M_addrmaskmem(addr) |
			AARCH64_LOWER_ATTR(AARCH64_SH_MEMORY, AARCH64_ATTR_INDEX_NCACHED) |
			0x01);
}
static unsigned aarch64_2M_mdevice(uint8_t * b, uint_fast64_t addr)
{
	return USBD_poke_u64(b,
			AARCH64_UPPER_ATTR |
			aarch64_2M_addrmaskmem(addr) |
			AARCH64_LOWER_ATTR(AARCH64_SH_DEVICE, AARCH64_ATTR_INDEX_DEVICE) |
			0x01);
}
// Next level table
// DDI0487_I_a_a-profile_architecture_reference_manual.pdf
// D8.3.1 Table Descriptor format

static unsigned aarch64_2M_mtable(uint8_t * b, uint_fast64_t addr, int level)
{
	return USBD_poke_u64(b,
			aarch64_2M_addrmasktable(addr) |
			0x03);
}
static unsigned aarch64_2M_mnoaccess(uint8_t * b, uint_fast64_t addr)
{
	return USBD_poke_u64(b, UINT64_C(0));
}

static const getmmudesc_t aarch64_table_2M =
{
	.mcached = aarch64_2M_mcached,
	.mncached = aarch64_2M_mncached,
	.mdevice = aarch64_2M_mdevice,
	.mnoaccess = aarch64_2M_mnoaccess,
	.mtable = aarch64_2M_mtable
};

///////////////
///
static unsigned aarch32_v7_1M_mcached(uint8_t * b, uint_fast64_t addr, int ro, int xn)
{
	return USBD_poke_u32(b, TTB_SECTION_AARCH32_1M_CACHED(addr, ro, xn));
}
static unsigned aarch32_v7_1M_mncached(uint8_t * b, uint_fast64_t addr, int ro, int xn)
{
	return USBD_poke_u32(b, TTB_SECTION_AARCH32_1M_NCACHED(addr, ro, xn));
}
static unsigned aarch32_v7_1M_mdevice(uint8_t * b, uint_fast64_t addr)
{
	return USBD_poke_u32(b, TTB_SECTION_AARCH32_1M_DEVICE(addr));
}
static unsigned aarch32_v7_1M_mnoaccess(uint8_t * b, uint_fast64_t addr)
{
	return USBD_poke_u32(b, UINT64_C(0));
}
static unsigned aarch32_v7_1M_mtable(uint8_t * b, uint_fast64_t addr, int level)
{
	// Next level table - dummy
	ASSERT(0);
	return USBD_poke_u32(b, UINT64_C(0));
}

static const getmmudesc_t aarch32_table_1M =
{
	.mcached = aarch32_v7_1M_mcached,
	.mncached = aarch32_v7_1M_mncached,
	.mdevice = aarch32_v7_1M_mdevice,
	.mnoaccess = aarch32_v7_1M_mnoaccess,
	.mtable = aarch32_v7_1M_mtable
};

///////////////
///
static unsigned aarch32_v7_16M_mcached(uint8_t * b, uint_fast64_t phyaddr, int ro, int xn)
{
	return USBD_poke_u32(b, TTB_SUPERSECTION_AARCH32_16M_CACHED(phyaddr, ro, xn));
}
static unsigned aarch32_v7_16M_mncached(uint8_t * b, uint_fast64_t phyaddr, int ro, int xn)
{
	return USBD_poke_u32(b, TTB_SUPERSECTION_AARCH32_16M_NCACHED(phyaddr, ro, xn));
}
static unsigned aarch32_v7_16M_mdevice(uint8_t * b, uint_fast64_t phyaddr)
{
	return USBD_poke_u32(b, TTB_SUPERSECTION_AARCH32_16M_DEVICE(phyaddr));
}
static unsigned aarch32_v7_16M_mnoaccess(uint8_t * b, uint_fast64_t phyaddr)
{
	return USBD_poke_u32(b, UINT64_C(0));
}
static unsigned aarch32_v7_16M_mtable(uint8_t * b, uint_fast64_t phyaddr, int level)
{
	// Next level table - dummy
	ASSERT(0);
	return USBD_poke_u32(b, UINT64_C(0));
}

static const getmmudesc_t aarch32_table_16M =
{
	.mcached = aarch32_v7_16M_mcached,
	.mncached = aarch32_v7_16M_mncached,
	.mdevice = aarch32_v7_16M_mdevice,
	.mnoaccess = aarch32_v7_16M_mnoaccess,
	.mtable = aarch32_v7_16M_mtable
};

static unsigned arch32_4k_mcached(uint8_t * b, uint_fast64_t addr, int ro, int xn)
{
	return USBD_poke_u32(b, TTB_SMALLSECTION_AARCH32_4K_CACHED(addr, ro, xn));
}
static unsigned arch32_4k_mncached(uint8_t * b, uint_fast64_t addr, int ro, int xn)
{
	return USBD_poke_u32(b, TTB_SMALLSECTION_AARCH32_4K_NCACHED(addr, ro, xn));
}
static unsigned arch32_4k_mdevice(uint8_t * b, uint_fast64_t addr)
{
	return USBD_poke_u32(b, TTB_SMALLSECTION_AARCH32_4K_DEVICE(addr));
}
static unsigned arch32_4k_mnoaccess(uint8_t * b, uint_fast64_t addr)
{
	return USBD_poke_u32(b, UINT32_C(0));
}
// Next level table
static unsigned arch32_4k_mtable(uint8_t * b, uint_fast64_t addr, int level)
{
	// 1KB granulation address
	return USBD_poke_u32(b, TTB_AARCH32_PAGETABLE(addr));	// First-level table entry - Page table
}

#if defined (__ARM_ARCH) && (__ARM_ARCH == 8)
	// Use DDI0487_I_a_a-profile_architecture_reference_manual.pdf
	//#warning __aarch32 v8
#endif

static const getmmudesc_t aarch32_v7_table_4k =
{
	.mcached = arch32_4k_mcached,
	.mncached = arch32_4k_mncached,
	.mdevice = arch32_4k_mdevice,
	.mnoaccess = arch32_4k_mnoaccess,
	.mtable = arch32_4k_mtable
};

#if ! defined(__aarch64__)
/** \brief  Set TTBCR

    This function assigns the given value to the Translation Table Base Register 0.

    \param [in]    ttbcr  Translation Table Base Register 0 value to set
 */
__STATIC_FORCEINLINE void __set_TTBCR(uint32_t ttbcr)
{
  __set_CP(15, 0, ttbcr, 2, 0, 2);
}

/** \brief  Set MAIR

    MAIR0 and MAIR1, Memory Attribute Indirection Registers 0 and 1, VMSA

    \param [in]    mair
 */
__STATIC_FORCEINLINE void __set_MAIR(uint32_t mair)
{
  __set_CP(15, 0, mair, 10, 2, 1);
}

__STATIC_FORCEINLINE void __set_TTBR0_64(uint64_t ttbr0)
{
	__set_CP64(15, 0, ttbr0, 2);
}

/** \brief  Get ID_MMFR3

    This function returns the value of theMemory Model Feature Register 3

    \return               Memory Model Feature Register 3 value
 */
__STATIC_FORCEINLINE uint32_t __get_ID_MMFR3(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 0, 1, 7);
  return result;
}

#endif /* ! defined(__aarch64__) */

#elif CPUSTYLE_RISCV

#if 1
// from rt-thread


#define SV39_VPN2(va) (((va) >> (9 + 9 + 12)) & 0x1FF)
#define SV39_VPN1(va) (((va) >> (0 + 9 + 12)) & 0x1FF)
#define SV39_VPN0(va) (((va) >> (0 + 0 + 12)) & 0x1FF)

#define SV39_PA2PPN(pa)   (((pa) >> (0 + 0 + 12)) & 0xFFFFFFFFFF)
#define SV39_PTE2PPN(pte) (((pte) >> (10)) & 0xFFFFFFF)
#define SV39_PTE2PA(pte)  ((uint64_t)(SV39_PTE2PPN(pte) << 12))

#define SV39_JTLB_ENTRY_CNT (256)

/* SMCIR */
#define SMCIR_ASID_OFFSET (0)
#define SMCIR_ASID_LEN    (16)

#define SMCIR_TLBII_OFFSET (25)
#define SMCIR_TLBII_LEN    (1)
#define SMCIR_TLBII        (1 << 25)

#define SMCIR_TLBIALL_OFFSET (26)
#define SMCIR_TLBIALL_LEN    (1)
#define SMCIR_TLBIALL        (1 << 26)

#define SMCIR_TLBIASID_OFFSET (27)
#define SMCIR_TLBIASID_LEN    (1)
#define SMCIR_TLBIASID        (1 << 27)

#define SMCIR_TLBWR_OFFSET (28)
#define SMCIR_TLBWR_LEN    (1)
#define SMCIR_TLBWR        (1 << 28)

#define SMCIR_TLBWI_OFFSET (29)
#define SMCIR_TLBWI_LEN    (1)
#define SMCIR_TLBWI        (1 << 29)

#define SMCIR_TLBR_OFFSET (30)
#define SMCIR_TLBR_LEN    (1)
#define SMCIR_TLBR        (1 << 30)

#define SMCIR_TLBP_OFFSET (31)
#define SMCIR_TLBP_LEN    (1)
#define SMCIR_TLBP        (1 << 31)

/* SMIR */
#define SMIR_INDEX_OFFSET (0)
#define SMIR_INDEX_LEN    (9)

#define SMCIR_T_OFFSET (30)
#define SMCIR_TF_LEN   (1)
#define SMCIR_TF       (1 << 30)

#define SMCIR_P_OFFSET (31)
#define SMCIR_PF_LEN   (1)
#define SMCIR_PF       (1 << 31)

/* SMEH */
#define SMEH_ASID_OFFSET (0)
#define SMEH_ASID_LEN    (16)

#define SMEH_PAGESIZE_OFFSET (16)
#define SMEH_PAGESIZE_LEN    (3)
#define SMEH_PAGESIZE_4KB    (1 << 16)
#define SMEH_PAGESIZE_2MB    (2 << 16)
#define SMEH_PAGESIZE_1GB    (4 << 16)

#define SMEH_VPN_OFFSET (19)
#define SMEH_VPN_LEN    (27)

#undef PTE_V
#undef PTE_U
#undef PTE_G
#undef PTE_A
#undef PTE_D

/* SMEL & PTE */
#define PTE_V_OFFSET (0)
#define PTE_V_LEN    (1)
#define PTE_V        (1 << 0)

#define PTE_R_OFFSET (1)
#define PTE_R_LEN    (1)

#define PTE_W_OFFSET (2)
#define PTE_W_LEN    (1)

#define PTE_X_OFFSET (3)
#define PTE_X_LEN    (1)

#define PTE_XWR_MASK     (~(0xE))
#define PTE_XWR_NEXT     (0 << 1)
#define PTE_XWR_RO_PAGE  (1 << 1)
#define PTE_XWR_WR_PAGE  (3 << 1)
#define PTE_XWR_XO_PAGE  (4 << 1)
#define PTE_XWR_XR_PAGE  (5 << 1)
#define PTE_XWR_XWR_PAGE (7 << 1)

#define PTE_U_OFFSET (4)
#define PTE_U_LEN    (1)
#define PTE_U        (1 << 4)

#define PTE_G_OFFSET (5)
#define PTE_G_LEN    (1)
#define PTE_G        (1 << 5)

#define PTE_A_OFFSET (6)
#define PTE_A_LEN    (1)
#define PTE_A        (1 << 6)

#define PTE_D_OFFSET (7)
#define PTE_D_LEN    (1)
#define PTE_D        (1 << 7)

#define SMEL_PPN_OFFSET (8)
#define PTE_PPN_OFFSET  (10)
#define PTE_PPN_LEN     (28)

#define PTE_SEC_OFFSET (59)
#define PTE_SEC_LEN    (1)

#define PTE_B_OFFSET (61)
#define PTE_B_LEN    (1)
#define PTE_B        ((uintptr_t)1 << 61)

#define PTE_C_OFFSET (62)
#define PTE_C_LEN    (1)
#define PTE_C        ((uintptr_t)1 << 62)

#define PTE_SO_OFFSET (63)
#define PTE_SO_LEN    (1)
#define PTE_SO        ((uintptr_t)1 << 63)

/* SATP */
#define SATP_PPN_OFFSET (0)
#define SATP_PPN_LEN    (28)

#define SATP_ASID_OFFSET (44)
#define SATP_ASID_LEN    (16)

#define SATP_MODE_OFFSET (60)
#define SATP_MODE_LEN    (4)
#define SATP_MODE_SV39   ((uintptr_t)8 << 60)

typedef enum {
    Sv39_PAGESIZE_4KB = SMEH_PAGESIZE_4KB,
    Sv39_PAGESIZE_2MB = SMEH_PAGESIZE_2MB,
    Sv39_PAGESIZE_1GB = SMEH_PAGESIZE_1GB,
    Sv39_PAGESIZE_NUM,
    Sv39_PAGESIZE_MAX = 0x7FFFFFFFFFFFFFFF
} Sv39_PAGESIZE_Type;

typedef enum {
    Sv39_PTE_LOC_JTLB = 0,
    Sv39_PTE_LOC_PAGETABLE,
    Sv39_PTE_LOC_NUM,
    Sv39_PTE_LOC_MAX = 0x7FFFFFFFFFFFFFFF
} Sv39_PTE_LOC_Type;

typedef struct _Sv39_PTE_cfg {
    uintptr_t va;
    uintptr_t pa;
    Sv39_PAGESIZE_Type size;
    uintptr_t flags;
    uintptr_t asid;
    Sv39_PTE_LOC_Type where;
    uintptr_t tlb_entry;
} Sv39_PTE_cfg_t;

//__STATIC_FORCEINLINE void RV_Sv39_INVALID_VMA(void)
//{
//    __asm volatile("fence"
//                   :
//                   :
//                   : "memory");
//    __asm volatile("sfence.vma x0,x0"
//                   :
//                   :
//                   : "memory");
//    __asm volatile("fence"
//                   :
//                   :
//                   : "memory");
//}

/* cache config into jTLB, return tlb index if success */
int RV_Sv39_Create_PageMapping(Sv39_PTE_cfg_t *cfg, uintptr_t *tlb_index, volatile uintptr_t *ttb0);

/* allocate 4KB page table buffer */
__attribute__((weak))
uint64_t *
RV_Sv39_alloc_PageTable_buf(void)
{
    return NULL;
}

/** @defgroup  SV39_Public_Functions
 *  @{
 */

/******************************************************************************
 * @brief  OSD blend layer configuration
 *
 * @param  cfg: point to the Page Table Entry.
 * @param  tlb_index: return the TLB index in use if MAP is created directly in TLB.
 * @param  ttb0: the root translation table in memory.
 *
 * @return Result of the operation.
 *
*******************************************************************************/

int RV_Sv39_Create_PageMapping(Sv39_PTE_cfg_t *cfg, uintptr_t *tlb_index, volatile uintptr_t *ttb0)
{
    uintptr_t smeh, smel, smcir;
    uintptr_t t0;
    volatile uint64_t *ttb1 = NULL;
    volatile uint64_t *ttb2 = NULL;

    if (Sv39_PTE_LOC_JTLB == cfg->where) {
        t0 = cfg->va;
        smeh = ((t0 >> 12) << SMEH_VPN_OFFSET) | cfg->size | cfg->asid;
        csr_write_smeh(smeh);

        t0 = cfg->pa;
        smel = cfg->flags | ((t0 >> 12) << PTE_PPN_OFFSET);
        csr_write_smel(smel);

        if (cfg->tlb_entry >= SV39_JTLB_ENTRY_CNT) {
            /* random write */
            smcir = SMCIR_TLBWR;
        } else {
        	csr_write_smir(cfg->tlb_entry);
            smcir = SMCIR_TLBWI;
        }

        smcir |= cfg->asid;
        csr_write_smcir(smcir);

        csr_write_smcir(SMCIR_TLBP);
        t0 = csr_read_smir();

        *tlb_index = t0;

        return (t0 & (SMCIR_PF | SMCIR_TF)) ? -1 : 0;
    } else {
        /* top level: 1GB */
        t0 = SV39_VPN2(cfg->va);

        if (!ttb0[t0]) {
            /* no translation for this 1GB range */
            if (Sv39_PAGESIZE_1GB == cfg->size) {
                /* finish map for top level */
                ttb0[t0] = (SV39_PA2PPN(cfg->pa) << PTE_PPN_OFFSET) |
                           (cfg->flags);
                return 0;
            } else {
                ttb1 = RV_Sv39_alloc_PageTable_buf();

                if (NULL == ttb1) {
                    return -1;
                }

                __COMPILER_BARRIER();
                ttb0[t0] = (SV39_PA2PPN((uint64_t)ttb1) << PTE_PPN_OFFSET) |
                           PTE_D | PTE_A |
                           PTE_XWR_NEXT | PTE_V;
            }
        } else {
            /* already mapped, get the next level table pointer */
            ttb1 = (volatile uint64_t *)SV39_PTE2PA(ttb0[t0]);
        }

        /* second level: 2MB */
        t0 = SV39_VPN1(cfg->va);

        if (!ttb1[t0]) {
            /* no translation for this 2MB range */
            if (Sv39_PAGESIZE_2MB == cfg->size) {
                ttb1[t0] = (SV39_PA2PPN(cfg->pa) << PTE_PPN_OFFSET) |
                           (cfg->flags);
                return 0;
            } else {
                ttb2 = RV_Sv39_alloc_PageTable_buf();

                if (NULL == ttb2) {
                    return -1;
                }

                ttb1[t0] = (SV39_PA2PPN((uint64_t)ttb2) << PTE_PPN_OFFSET) |
                           PTE_D | PTE_A |
                           PTE_XWR_NEXT | PTE_V;
            }
        } else {
            ttb2 = (volatile uint64_t *)SV39_PTE2PA(ttb1[t0]);
        }

        /* third level: 4KB */
        t0 = SV39_VPN0(cfg->va);
        __COMPILER_BARRIER();
        ttb2[t0] = (SV39_PA2PPN(cfg->pa) << PTE_PPN_OFFSET) |
                   (cfg->flags);
    }

    /* flush the D cache */

    return 0;
}
#endif

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
//#define RAM_ATTRS 		((UINT64_C(0) << 63) | (UINT64_C(1) << 62) | (UINT64_C(1) << 61) | (UINT64_C(0x07) << 1) | 1)	// Cacheable memory
//#define NCRAM_ATTRS 	((UINT64_C(0) << 63) | (UINT64_C(0) << 62) | (UINT64_C(0) << 61) | (UINT64_C(0x07) << 1) | 1)	// Non-cacheable memory
//#define DEVICE_ATTRS 	((UINT64_C(1) << 63) | (UINT64_C(0) << 62) | (UINT64_C(0) << 61) | (UINT64_C(0x07) << 1) | 1)	// Non-bufferable device
//#define TABLE_ATTRS		((UINT64_C(0) << 63) | 1) // Pointer to next level of page table

// See Table 4.2: Encoding of PTE Type field.

#define RV64_SV39_PTE(pbmt, ppn2, ppn1, ppn0, rsw, d, a, g, u, x, w, r) ( \
		((pbmt) & 0x03) * (UINT64_C(1) << 61) | /* PBMT */ \
		((ppn2) & 0x03FFFFFF) * (UINT64_C(1) << 28) | /* PPN[2] */ \
		((ppn1) & 0x1FF) * (UINT64_C(1) << 19) | /* PPN[1] */ \
		((ppn0) & 0x1FF) * (UINT64_C(1) << 10) | /* PPN[0] */ \
		((rsw) & 0x03) * (UINT64_C(1) << 8) | /* RSW */ \
		!! (d) * (UINT64_C(1) << 7) | /* D: it indicates whether the virtual page has been written since the last time the D bit was cleared. */ \
		!! (a) * (UINT64_C(1) << 6) | /* A: it indicates whether the virtual page has been read written, or fetched from since the last time the A bit was cleared. */ \
		!! (g) * (UINT64_C(1) << 5) | /* G: this bit designates Global mapping. Global mappings are those that exist in all address space. */ \
		!! (u) * (UINT64_C(1) << 4) | /* U: it indicates whether the page is accessible to user mode or not. U-mode software may only access the page when U=1 if the SUM bit in the sstatus registeris set, supervisor mode software may also access pages with U=1 */ \
		!! (x) * (UINT64_C(1) << 3) | /* it indicate if the PTE is executable or not */ \
		!! (w) * (UINT64_C(1) << 2) | /* W: it indicate if the PTE is writeable or not */ \
		!! (r) * (UINT64_C(1) << 1) | /* R: it indicates if the PTE is readable or not */ \
		(1) * (UINT64_C(1) << 0) | /* V: it indicates whether the PTE is valid or not */ \
	0)

//	for Sv39 PTESIZE=8 and LEVELS=3
//	for Sv48 PTESIZE=8 and LEVELS=4
//	for Sv57 PTESIZE=8 and LEVELS=5
// 	Any level of PTE may be a leaf PTE, so
//		in addition to 4 KiB pages, Sv39 supports 2 MiB megapages and 1 GiB gigapages

#define RV64_SV39_VA_PPN2(va) ((va >> 30) & UINT64_C(0x1FF))	// get PPN[2] from virtual address
#define RV64_SV39_VA_PPN1(va) ((va >> 21) & UINT64_C(0x1FF))	// get PPN[1] from virtual address
#define RV64_SV39_VA_PPN0(va) ((va >> 12) & UINT64_C(0x1FF))	// get PPN[0] from virtual address

#define vASID 0x0000	// ASID: the current address space identifier (ASID)

// Bit 63 - Strong order
// Bit 62 - Cacheable
// Bit 61 - Buffer
#define vRAM_PBMT 0x03		// Cacheable memory
#define vNCRAM_PBMT 0x00	// Non-cacheable memory
#define vDEVICE_PBMT 0x04	// Non-bufferable device

#define vTABLE_PBMT 0x00	// Pointer to next level of page table

#define vRSW 0x00	// A bit reserved for software to implement custom page table features. The default value is 2’b0
#define vD 0x01		// Dirty
#define vA 0x01		// Accessed
#define vG 0x01		// 1’b0: indicates that the page is non-shareable and that the ASID is exclusive,  1’b1: indicates that the page is shareable
#define vU 0x01		// User
#define vX 0x01		//  X: executable, W: writable, R: readable
#define vW 0x01
#define vR 0x01

// 4 KB pages
static unsigned rv64_sv39_mcached(uint8_t * b, uint_fast64_t addr, int ro, int xn)
{
	const uint_fast64_t ppn2 = RV64_SV39_VA_PPN2(addr);
	const uint_fast64_t ppn1 = RV64_SV39_VA_PPN1(addr);
	const uint_fast64_t ppn0 = RV64_SV39_VA_PPN0(addr);
	return USBD_poke_u64(b, RV64_SV39_PTE(vRAM_PBMT, ppn2, ppn1, ppn0, vRSW, vD, vA, vG, vU, vX, vW, vR));
}
// 4 KB pages
static unsigned rv64_sv39_mncached(uint8_t * b, uint_fast64_t addr, int ro, int xn)
{
	const uint_fast64_t ppn2 = RV64_SV39_VA_PPN2(addr);
	const uint_fast64_t ppn1 = RV64_SV39_VA_PPN1(addr);
	const uint_fast64_t ppn0 = RV64_SV39_VA_PPN0(addr);
	return USBD_poke_u64(b, RV64_SV39_PTE(vNCRAM_PBMT, ppn2, ppn1, ppn0, vRSW, vD, vA, vG, vU, vX, vW, vR));
}
// 4 KB pages
static unsigned rv64_sv39_mdevice(uint8_t * b, uint_fast64_t addr)
{
	const uint_fast64_t ppn2 = RV64_SV39_VA_PPN2(addr);
	const uint_fast64_t ppn1 = RV64_SV39_VA_PPN1(addr);
	const uint_fast64_t ppn0 = RV64_SV39_VA_PPN0(addr);
	return USBD_poke_u64(b, RV64_SV39_PTE(vDEVICE_PBMT, ppn2, ppn1, ppn0, vRSW, vD, vA, vG, vU, vX, vW, vR));
}
static unsigned rv64_sv39_mtable(uint8_t * b, uint_fast64_t addr, int level)
{
	const uint_fast64_t ppn2 = RV64_SV39_VA_PPN2(addr);
	const uint_fast64_t ppn1 = RV64_SV39_VA_PPN1(addr);
	const uint_fast64_t ppn0 = RV64_SV39_VA_PPN0(addr);
	switch (level)
	{
	default:
	case 0:	// 1 GB pages
		return USBD_poke_u64(b, RV64_SV39_PTE(vTABLE_PBMT, ppn2, ppn1, ppn0, vRSW, vD, vA, vG, vU, 0*vX, 0*vW, 0*vR));
	case 1:	// 2 MB pages
		return USBD_poke_u64(b, RV64_SV39_PTE(vTABLE_PBMT, ppn2, ppn1, ppn0, vRSW, vD, vA, vG, vU, 0*vX, 0*vW, 0*vR));
	}
}
static unsigned rv64_mnoaccess(uint8_t * b, uint_fast64_t addr)
{
	return USBD_poke_u64(b, UINT64_C(0));
}

static const getmmudesc_t rv64_sv39_table =
{
	.mcached = rv64_sv39_mcached,
	.mncached = rv64_sv39_mncached,
	.mdevice = rv64_sv39_mdevice,
	.mnoaccess = rv64_mnoaccess,
	.mtable = rv64_sv39_mtable
};

static void rv64_xmret2(void)
{
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	TP();
	for (;;)
		;
}
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
static void fillmmu(const mmulayout_t * p, unsigned n, unsigned (* accessbits)(const mmulayout_t * layout, const getmmudesc_t * arch, uint8_t * b, uint_fast64_t phyaddr, int ro, int xn))

{
	while (n --)
	{
		const uint_fast32_t pagesize = UINT32_C(1) << p->phypageszlog2;
		unsigned pages = p->pagecount;
		uint8_t * tb = p->table;	// table base
		uint_fast64_t phyaddr = p->phybytes ? (uintptr_t) p->phybytes : p->phyaddr;
		for (; pages --; phyaddr += pagesize)
		{
			tb += p->level != INT_MAX ?
					p->arch->mtable(tb, phyaddr, p->level) :
					accessbits(p, p->arch, tb, phyaddr, p->ro, p->xn);
		}
		const ptrdiff_t cachesize = tb - p->table;
		dcache_clean_invalidate((uintptr_t) p->table, cachesize);
		++ p;
	}
}

#if defined (__CORTEX_M)

#elif CPUSTYLE_RISCV

	// SV39 model - with 4KB size pages of phy memory
	/* top level: 1GB */
	/* second level: 2MB */
	/* third level: 4KB */
	#define RV64_LEVEL0_SIZE (HARDWARE_ADDRSPACE_GB)
	#define RV64_LEVEL1_SIZE (512 * HARDWARE_ADDRSPACE_GB)
	#define RV64_LEVEL2_SIZE (512 * 512 * HARDWARE_ADDRSPACE_GB)
	static RAMFRAMEBUFF __ALIGNED(4 * 1024) uint8_t xlevel2_pagetable_u64 [RV64_LEVEL2_SIZE * sizeof (uint64_t)];	// Used as PPN in SATP register
	static RAMFRAMEBUFF __ALIGNED(4 * 1024) uint8_t xlevel1_pagetable_u64 [RV64_LEVEL1_SIZE * sizeof (uint64_t)];	// Used as PPN in SATP register
	static RAMFRAMEBUFF __ALIGNED(4 * 1024) uint8_t xlevel0_pagetable_u64 [RV64_LEVEL0_SIZE * sizeof (uint64_t)];	// Used as PPN in SATP register

	static const mmulayout_t mmuinfo [] =
	{
		/* third level: 4KB */
		{
			.arch = & rv64_sv39_table,
			.phyaddr = 0x00000000,	/* Начало физической памяти */
			.phybytes = NULL,
			.phypageszlog2 = 12,	// 4KB
			.pagecount = RV64_LEVEL2_SIZE,
			.table = xlevel2_pagetable_u64,
			.level = INT_MAX,	// memory pages with access bits
			.ro = 0, .xn = 0	// page attributes (pass to mcached/mncached)
		},
		/* second level: 2MB */
		{
			.arch = & rv64_sv39_table,
			.phybytes = xlevel2_pagetable_u64,
			.phypageszlog2 = (9 + 3),	// 512 items by 8 bytes each in xlevel2_pagetable_u64
			.pagecount = RV64_LEVEL1_SIZE,
			.table = xlevel1_pagetable_u64,
			.level = 1,
			.ro = 0, .xn = 0	// page attributes (pass to mcached/mncached)
		},
		/* top level: 1GB */
		{
			.arch = & rv64_sv39_table,
			.phybytes = xlevel1_pagetable_u64,
			.phypageszlog2 = (9 + 3),	// 512 items by 8 bytes each in xlevel1_pagetable_u64
			.pagecount = RV64_LEVEL0_SIZE,
			.table = xlevel0_pagetable_u64,
			.level = 0, // page table level (pass to mtable)
			.ro = 0, .xn = 0	// page attributes (pass to mcached/mncached)
		},
	};
	static const int glongdesc = 1;

#elif MMUUSE2MPAGES

	// pages of 2 MB
	#define AARCH64_LEVEL0_SIZE (HARDWARE_ADDRSPACE_GB)
	#define AARCH64_LEVEL1_SIZE (HARDWARE_ADDRSPACE_GB * 512)		// pages of 2 MB
	//	A translation table is required to be aligned to the size of the table. If a table contains fewer than
	//	eight entries, it must be aligned on a 64 byte address boundary.
	static RAMFRAMEBUFF __ALIGNED(AARCH64_LEVEL0_SIZE * sizeof (uint64_t)) uint8_t ttb0_base [AARCH64_LEVEL0_SIZE * sizeof (uint64_t)];	// ttb0_base must be a 4KB-aligned address.
	static RAMFRAMEBUFF __ALIGNED(4 * 1024) uint8_t xxlevel1_pagetable_u64 [AARCH64_LEVEL1_SIZE * sizeof (uint64_t)];	// ttb0_base must be a 4KB-aligned address.

	static const mmulayout_t mmuinfo [] =
	{
		{
			.arch = & aarch64_table_2M,
			.phyaddr = 0x00000000,	/* Начало физической памяти */
			.phybytes = NULL,
			.phypageszlog2 = 21,	// 2MB
			.pagecount = AARCH64_LEVEL1_SIZE,
			.table = xxlevel1_pagetable_u64,
			.level = INT_MAX,	// memory pages with access bits
			.ro = 0, .xn = 0	// page attributes (pass to mcached/mncached)
		},
		{
			.arch = & aarch64_table_2M,
			.phybytes = xxlevel1_pagetable_u64,
			.phypageszlog2 = 9 + 3,	// 512 elements by 8 bytes in each page of xxlevel1_pagetable_u64
			.pagecount = AARCH64_LEVEL0_SIZE,
			.table = ttb0_base,
			.level = 0, // page table level (pass to mtable)
			.ro = 0, .xn = 0	// page attributes (pass to mcached/mncached)
		},
	};
	static const int glongdesc = 1;
#elif MMUUSE16MPAGES
	// AARCH32
	// pages of 16 MB (supersections)
	#define AARCH32_16MB_LEVEL0_SIZE (HARDWARE_ADDRSPACE_GB * 1024)
	static RAMFRAMEBUFF __ALIGNED(16 * 1024) uint8_t ttb0_base [AARCH32_16MB_LEVEL0_SIZE * sizeof (uint32_t)];	// вся физическая память страницами по 1 мегабайт
	static const mmulayout_t mmuinfo [] =
	{
		{
			.arch = & aarch32_table_16M,
			.phyaddr = 0x00000000,	/* Начало физической памяти */
			.phybytes = NULL,
			.phypageszlog2 = 20,	// каждые 16 ячеек заполняются одинаковой иформацией
			.pagecount = AARCH32_16MB_LEVEL0_SIZE,
			.table = ttb0_base,
			.level = INT_MAX,	// memory pages with access bits
			.ro = 0, .xn = 0	// page attributes (pass to mcached/mncached)
		},
	};
	static const int glongdesc = 0;
#elif MMUUSE4KPAGES

	#define vHARDWARE_ADDRSPACE_GB 4
	// AARCH32
	// pages of 4 k
	#define AARCH32_4K_LEVEL1_SIZE (vHARDWARE_ADDRSPACE_GB * 256 * 1024)	// физическая память - страницы по 4 KB
	#define AARCH32_4K_LEVEL0_SIZE (AARCH32_4K_LEVEL1_SIZE / 256)
	static RAMFRAMEBUFF __ALIGNED(16 * 1024) uint8_t ttb0_base [AARCH32_4K_LEVEL0_SIZE * sizeof (uint32_t)];
	static RAMFRAMEBUFF __ALIGNED(1 * 1024) uint8_t level1_pagetable_u32 [AARCH32_4K_LEVEL1_SIZE * sizeof (uint32_t)];	// вся физическая память страницами по 4 килобайта
	static const mmulayout_t mmuinfo [] =
	{
		{
			.arch = & aarch32_v7_table_4k,
			.phyaddr = 0x00000000,	/* Начало физической памяти */
			.phybytes = NULL,
			.phypageszlog2 = 12,	// 4KB
			.pagecount = AARCH32_4K_LEVEL1_SIZE,
			.table = level1_pagetable_u32,
			.level = INT_MAX,	// memory pages with access bits
			.ro = 0, .xn = 0	// page attributes (pass to mcached/mncached)
		},
		{
			.arch = & aarch32_v7_table_4k,
			.phybytes = level1_pagetable_u32,
			.phypageszlog2 = 10,	// 1KB
			.pagecount = AARCH32_4K_LEVEL0_SIZE,
			.table = ttb0_base,
			.level = 0, // page table level (pass to mtable)
			.ro = 0, .xn = 0	// page attributes (pass to mcached/mncached)
		},
	};
	static const int glongdesc = 0;

#elif MMUUSE1MPAGES
	// pages of 1 MB
	#define AARCH32_1MB_LEVEL0_SIZE (HARDWARE_ADDRSPACE_GB * 1024)
	static RAMFRAMEBUFF __ALIGNED(16 * 1024) uint8_t ttb0_base [AARCH32_1MB_LEVEL0_SIZE * sizeof (uint32_t)];	// вся физическая память страницами по 1 мегабайт
	static const mmulayout_t mmuinfo [] =
	{
		{
			.arch = & aarch32_table_1M,
			.phyaddr = 0x00000000,	/* Начало физической памяти */
			.phybytes = NULL,
			.phypageszlog2 = 20,	// 1MB
			.pagecount = AARCH32_1MB_LEVEL0_SIZE,
			.table = ttb0_base,
			.level = INT_MAX,	// memory pages with access bits
			.ro = 0, .xn = 0	// page attributes (pass to mcached/mncached)
		},
	};
	static const int glongdesc = 0;
#endif /* defined(__aarch64__) */

#endif /* (__CORTEX_A != 0) || CPUSTYLE_ARM9 || CPUSTYLE_RISCV */

/* Один раз - инициализация таблиц в памяти */
void
sysinit_mmu_tables(void)
{
	//PRINTF("sysinit_mmu_tables\n");

#if (__CORTEX_A != 0) || CPUSTYLE_ARM9 || CPUSTYLE_RISCV
	// MMU tables iniitialize
	fillmmu(mmuinfo, ARRAY_SIZE(mmuinfo), ttb_mempage_accessbits);

#elif defined (__CORTEX_M)

#endif

	//PRINTF("sysinit_mmu_tables done.\n");
}

#if (__CORTEX_A != 0)

static const uint_fast32_t IRGN_attr = AARCH64_CACHEATTR_WB_WA_CACHE;	// Normal memory, Inner Write-Back Write-Allocate Cacheable.
static const uint_fast32_t ORGN_attr = AARCH64_CACHEATTR_WB_WA_CACHE;	// Normal memory, Outer Write-Back Write-Allocate Cacheable.
//static const uint_fast32_t IRGN_attr = AARCH32_CACHEATTR_WB_WA_CACHE;	// Normal memory, Inner Write-Back Write-Allocate Cacheable.
//const uint_fast32_t ORGN_attr = AARCH32_CACHEATTR_WB_WA_CACHE;	// Normal memory, Outer Write-Back Write-Allocate Cacheable.

// Table G5-16 SH[1:0] field encoding for Normal memory, Long-descriptor format
// 0x02 - outer shareable
// 0x03 - inner shareable
static unsigned SH1_attr = 0x03;
static unsigned SH0_attr = 0x03;

static void progttbcr(int uselongdesc)
{
	// D17.2.146  TTBR0_EL3, Translation Table Base Register 0 (EL3)
	// DDI0500J_cortex_a53_r0p4_trm.pdf
	// 4.3.53 Translation Control Register, EL3
#if defined(__aarch64__)

	// определение размера физической памяти, на который настраиваем MMU
	// __log2_up(AARCH64_LEVEL1_SIZE)=13, mmuinfo [0].pgszlog2=21
	//PRINTF("__log2_up(AARCH64_LEVEL1_SIZE)=%u, mmuinfo [0].pgszlog2=%u\n", (unsigned) __log2_up(AARCH64_LEVEL1_SIZE), mmuinfo [0].phypageszlog2);

	//const unsigned aspacebits = 21 + __log2_up(AARCH64_LEVEL1_SIZE);	// pages of 2 MB
	const unsigned aspacebits = __log2_up(HARDWARE_ADDRSPACE_GB) + 30;
	//ASSERT(aspacebits == aspacebits2);
	const uint_fast32_t tcrv =
		0 * (UINT32_C(1) << 30) |	// TCMA - see FEAT_MTE2
		0 * (UINT32_C(1) << 29) |	// TBID - see FEAT_PAuth (top byte Id)
		0 * (UINT32_C(1) << 28) |	// HWU62 - see FEAT_HPDS2
		0 * (UINT32_C(1) << 27) |	// HWU61 - see FEAT_HPDS2
		0 * (UINT32_C(1) << 26) |	// HWU60 - see FEAT_HPDS2
		0 * (UINT32_C(1) << 25) |	// HWU59 - see FEAT_HPDS2
		0 * (UINT32_C(1) << 22) |	// HD - see FEAT_HAFDBS
		0 * (UINT32_C(1) << 21) |	// HA - see FEAT_HAFDBS
		1 * (UINT32_C(1) << 20) |	// TBI - Top Byte Ignored. Indicates whether the top byte of an address is used for address match for the TTBR0_EL3 region, or ignored and used for tagged addresses.
		0x01 * (UINT32_C(1) << 16) |	// 18:16 PS - Physical Address Size. 36 bits, 64GB
		vTG0 * (UINT32_C(1) << 14) | 	// TG0 Granule size for the TTBR0_EL3. 0x00 4KB, 0x01 64KB, 0x02 16KB
		SH0_attr * (UINT32_C(1) << 12) |	// SH0 0x03 - Inner shareable (Shareability attribute for memory associated with translation table walks using TTBR0_EL3)
		ORGN_attr * (UINT32_C(1) << 10) |	// ORGN0 Outer cacheability attribute
		IRGN_attr * (UINT32_C(1) << 8) |	// IRGN0 Inner cacheability attribute
		(0x3F & (64 - aspacebits)) * (UINT32_C(1) << 0) |		// T0SZ n=0..63. T0SZ=2^(64-n): n=28: 64GB, n=30: 16GB, n=32: 4GB, n=43: 2MB
		0;

	__set_TCR_EL1(tcrv);
	__set_TCR_EL2(tcrv);
	__set_TCR_EL3(tcrv);	// нужно только это
#else /* defined(__aarch64__) */

	// AArch32 System register TTBCR bits [31:0] are architecturally mapped to AArch64 System  register TCR_EL1[31:0].

	const uint_fast32_t ttbcrv_short =
		0 * (UINT32_C(1) << 31) | /* EAE */
		0 * (UINT32_C(1) << 0) |	/* N */
		0;
	const uint_fast32_t ttbcrv_long =
		1 * (UINT32_C(1) << 31) | /* EAE */
		SH1_attr * (UINT32_C(1) << 28) |	// SH1
		IRGN_attr * (UINT32_C(1) << 24) |	// IRGN1
		ORGN_attr * (UINT32_C(1) << 26) |	// ORGN1
		0 * (UINT32_C(1) << 16) | /* T1SZ */
		SH0_attr * (UINT32_C(1) << 12) |	// SH1
		ORGN_attr * (UINT32_C(1) << 10) |	// ORGN0
		IRGN_attr * (UINT32_C(1) << 8) |	// IRGN0
		0 * (UINT32_C(1) << 0) |		// T0SZ Input address range using for TTBR0 and TTBR1
		0;

	__set_TTBCR(uselongdesc ? ttbcrv_long : ttbcrv_short);

#endif /* defined(__aarch64__) */
}

static void progttbr(uintptr_t ttb0, size_t ttb0_size, int uselongdesc)
{
	const unsigned TTBR_ASIDv = 0x00;

#if defined(__aarch64__)

	// 48 bit address
	//	A translation table is required to be aligned to the size of the table. If a table contains fewer than
	//	eight entries, it must be aligned on a 64 byte address boundary.
	const uintptr_t ttb0mask = ((uintptr_t) 1 << __log2_up(ttb0_size)) - 1;
	const uint_fast64_t ttbr0v =
		(ttb0 & ~ ttb0mask) |
		0 * (UINT64_C(1) << 0) |	// When FEAT_TTCNP is implemented: Common not Private
		0;
	//	A translation table is required to be aligned to the size of the table. If a table contains fewer than
	//	eight entries, it must be aligned on a 64 byte address boundary.
	ASSERT(HARDWARE_ADDRSPACE_GB >= 8);
	ASSERT((ttb0 & ttb0mask) == 0);


	__set_TTBR0_EL1(ttbr0v);
	__set_TTBR0_EL2(ttbr0v);
	__set_TTBR0_EL3(ttbr0v);	// нужно только это

#else /* defined(__aarch64__) */

	const uint_fast32_t arch32_ttb0mask = ((uintptr_t) 1 << 14) - 1;
	ASSERT((ttb0 & arch32_ttb0mask) == 0);
	//  G8.2.166 TTBR0, Translation Table Base Register 0
#if WITHSMPSYSTEM
	const uint_fast32_t arch32_ttbr0v =
		(ttb0 & ~ arch32_ttb0mask) |	/* Translation table base 0 address, bits[31:x], where x is 14-(TTBCR.N) */
		!! (IRGN_attr & 0x01) * (UINT32_C(1) << 6) |	// IRGN[0]
		!! (IRGN_attr & 0x02) * (UINT32_C(1) << 0) |	// IRGN[1]
		ORGN_attr * (UINT32_C(1) << 3) |	// RGN
		!1 * (UINT32_C(1) << 5) |	// NOS - Not Outer Shareable bit - TEST for RAMNC
		1 * (UINT32_C(1) << 1) |	// S - Shareable bit. Indicates the Shareable attribute for the memory associated with the translation table
		0;
#else /* WITHSMPSYSTEM */
		const uint_fast32_t arch32_ttbr0v =
		(ttb0 & ~ arch32_ttb0mask) |	/* Translation table base 0 address, bits[31:x], where x is 14-(TTBCR.N) */
		//(!! (IRGN_attr & 0x02) << 6) | (!! (IRGN_attr & 0x01) << 0) |
		1 * (UINT32_C(1) << 3) |	// RGN
		0 * (UINT32_C(1) << 5) |	// NOS
		0 * (UINT32_C(1) << 1) |	// S
		0;
#endif /* WITHSMPSYSTEM */

	const uint_fast64_t arch32_ttbr0v_64 =
		(TTBR_ASIDv & 0xFF) * (UINT64_C(1) << 48) |
		(ttb0 & ~ arch32_ttb0mask) |	/* Translation table base 0 address, bits[31:x], where x is 14-(TTBCR.N) */
		0;
	// B4.1.154 TTBR0, Translation Table Base Register 0, VMSA
	__set_TTBR0(arch32_ttbr0v);
	//__set_TTBR0_64(arch32_ttbr0v_64);	// для TTBCR.EAE == 1 будет другой формат

#endif /* defined(__aarch64__) */
}

static void progmair(void)
{
	//  D17.2.99 MAIR_EL3, Memory Attribute Indirection Register (EL3)
	const uint_fast32_t mairv =
		0xFF * (UINT32_C(1) << (AARCH64_ATTR_INDEX_CACHED * 8)) |		// Normal Memory, Inner/Outer Write-back non-transient
		0x44 * (UINT32_C(1) << (AARCH64_ATTR_INDEX_NCACHED * 8)) |	// Normal memory, Inner/Outer Non-Cacheable
		0x00 * (UINT32_C(1) << (AARCH64_ATTR_INDEX_DEVICE * 8)) | 	// Device-nGnRnE memory
		0;

#if defined(__aarch64__)
//	__set_MAIR_EL1(mairv);
//	__set_MAIR_EL2(mairv);
	__set_MAIR_EL3(mairv);	// нужно только это
#else
	__set_MAIR(mairv);
#endif
}
static void progdomain(void)
{
#if defined(__aarch64__)
	//  D17.2.35 DACR32_EL2, Domain Access Control Register
	// Program the domain access register
	const uint_fast32_t dacr32v =
		UINT32_C(0x55555555) * 0x03 |	// domain 15..0: Manager. Accesses are not checked against the permission bits in the translation tables.
		0;
	__set_DACR32_EL2(dacr32v);
#else
	// Program the domain access register
	__set_DACR(0xFFFFFFFF); // domain 15: access are not checked
#endif
}

static void printdebug(void)
{

	// Cortex-A53, aarch32:
	// __get_ID_MMFR3()=0x02102211
	// Cortex-A7
	// __get_ID_MMFR3()=0x02102211

	//PRINTF("__get_ID_MMFR3()=0x%08X\n", (unsigned) __get_ID_MMFR3());
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

}
#endif

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

#if(__CORTEX_A != 0)

	progttbcr(glongdesc);
	progttbr((uintptr_t) ttb0_base, sizeof ttb0_base, glongdesc);
	progmair();
	progdomain();
	printdebug();

	__ISB();

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

	ASSERT(((uintptr_t) xlevel0_pagetable_u64 & 0x0FFF) == 0);
	mmu_flush_cache();

	// 5.2.1.1 MMU address translation register (SATP)
	// When Mode is 0, the MMU is disabled. C906 supports only the MMU disabled and Sv39 modes
	const uint_fast64_t satp =
			//CSR_SATP_MODE_PHYS * (UINT64_C(1) << 60) | // MODE
			CSR_SATP_MODE_SV39 * (UINT64_C(1) << 60) | // MODE
			(vASID  & UINT64_C(0xFFFF)) * (UINT64_C(1) << 44) | // ASID
			(((uintptr_t) xlevel0_pagetable_u64 >> 12) & UINT64_C(0x0FFFFFFF)) * (UINT64_C(1) << 0) |	// PPN - 28 bit
			0;

//	csr_write_smir(UINT64_MAX);
//	csr_write_smel(UINT64_MAX);
//	csr_write_smeh(UINT64_MAX);
//	csr_write_smcir(UINT64_MAX);

//	2 csr_read_smir()=0x00000000000001FF
//	2 csr_read_smel()=0xFF80000FFFFFFFFF
//	2 csr_read_smeh()=0x00003FFFFFFFFFFF
//	2 csr_read_smcir()=0x000000000000FFFF

	PRINTF("2 csr_read_smir()=0x%016" PRIX64 "\n", csr_read_smir());
	PRINTF("2 csr_read_smel()=0x%016" PRIX64 "\n", csr_read_smel());
	PRINTF("2 csr_read_smeh()=0x%016" PRIX64 "\n", csr_read_smeh());
	PRINTF("2 csr_read_smcir()=0x%016" PRIX64 "\n", csr_read_smcir());

//	mmu_write_satp(satp);
//	mmu_flush_cache();
	PRINTF("1 ttb0_base=%p" "\n", xlevel0_pagetable_u64);
	PRINTF("1 satp=0x%016" PRIX64 "\n", satp);
	PRINTF("1 csr_read_satp()=0x%016" PRIX64 "\n", csr_read_satp());
	csr_write_satp(satp);
	PRINTF("2 csr_read_satp()=0x%016" PRIX64 "\n", csr_read_satp());

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
	//csr_write_sasid(vASID);
	//csr_write_sptbr((uintptr_t) xlevel0_pagetable_u64);

	// https://people.eecs.berkeley.edu/~krste/papers/riscv-priv-spec-1.7.pdf
	// 3.1.6 Virtualization Management Field in mstatus Register
	// Table 3.3: Encoding of virtualization management field VM[4:0]

	if (1)
	{
		{
			//	Step 1: Configure the mstatus and mpp registers
			//	Set the MPP (Machine Previous Privilege Mode) field: Write a value of 01 to the MPP field of the mstatus register. This indicates that the previous privilege mode was supervisor mode.
			//	Set the MIE (Machine Interrupt Enable) bit: Set the MIE bit in the mstatus register to enable machine mode interrupts.
			//	Set the SIE (Supervisor Interrupt Enable) bit: Set the SIE bit in the mstatus register to enable supervisor mode interrupts.
			unsigned vMPP = 0x01;	 // When MPP is 2’b01, the CPU is in S-mode before accessing the exception service program
			//unsigned vMPP = 0x03;	 // When MPP is 2’b11, the CPU is in M-mode before entering the exception service program
			unsigned vMIE = 0x01;
			unsigned vSIE = 0x01;
			uint_xlen_t mstatus = csr_read_mstatus();
			mstatus = (mstatus & ~ MSTATUS_MPP_BIT_MASK) | vMPP * (UINT64_C(1) << MSTATUS_MPP_BIT_OFFSET);	// MPP
			mstatus = (mstatus & ~ (UINT64_C(1) << 3)) | vMIE * (UINT64_C(1) << 3);	// MIE
			mstatus = (mstatus & ~ (UINT64_C(1) << 1)) | vSIE * (UINT64_C(1) << 1);	// SIE
			csr_write_mstatus(mstatus);
			csr_set_bits_mstatus(MSTATUS_MIE_BIT_MASK);
			csr_set_bits_mstatus(MSTATUS_SIE_BIT_MASK);
		}

		{
			// Step 2: Configure the sstatus and satp registers
			//	Configure the sstatus.SUM bit: Set the SUM (Supervisor User Memory access) bit to control how user-mode memory accesses are handled in supervisor mode.
			//	Configure the satp register: Write the base address of your page table to the PPN (Physical Page Number) field of the satp register. The MODE field of satp should be set to the appropriate value (e.g., 9 for Sv39, 10 for Sv48) to enable supervisor-mode address translation.
			unsigned vSUM = 0x01;
			uint_xlen_t sstatus = csr_read_sstatus();
			sstatus = (sstatus & ~ (UINT64_C(1) << 18)) | vSUM * (UINT64_C(1) << 18);	// SUM
			csr_write_sstatus(sstatus);

		}

		{
			// Step 3: Execute the mret instruction
			csr_write_sepc((uintptr_t) rv64_xmret2);
			csr_write_mepc((uintptr_t) rv64_xmret2);
			PRINTF("rv64_xmret2=%p\n", rv64_xmret2);
			csr_set_bits_mstatus(MSTATUS_MPIE_BIT_MASK);
			__MRET();
		}
	}


	// 15.1.2 M-mode exception configuration register group
	// https://riscv.org/wp-content/uploads/2019/08/riscv-privileged-20190608-1.pdf

#elif defined (__CORTEX_M)
	#if CPUSTYLE_STM32H7XX
		//lowlevel_stm32h7xx_mpu_initialize();
	#endif /* CPUSTYLE_STM32H7XX */

#endif
	//PRINTF("sysinit_ttbr_initialize done.\n");
}

#endif /* ! LINUX_SUBSYSTEM */


