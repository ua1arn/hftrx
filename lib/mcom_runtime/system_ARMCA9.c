/******************************************************************************
 * @file     system_ARMCA9.c
 * @brief    CMSIS Device System Source File for Arm Cortex-A9 Device Series
 * @version  V1.0.1
 * @date     13. February 2019
 *
 * @note
 *
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mcom02.h"
#include "1892vm14ya.h"
#include "ArmV7Support.h"
#include "ARMCA9.h"
#include "cmsis_cp15.h"
#include "core_ca.h"

#define SCU_CONTROL_BASE 0x39000000
#define L2C_310 		 0x39004000

// Short-descriptor format memory region attributes, without TEX remap
// When using the Short-descriptor translation table formats, TEX remap is disabled when SCTLR.TRE is set to 0.

// For TRE - see
// B4.1.127 PRRR, Primary Region Remap Register, VMSA

#define APRWval 		0x03	/* Full access */
#define APROval 		0x06	/* All write accesses generate Permission faults */
#define DOMAINval		0x0F
#define SECTIONval		0x02	/* 0b10, Section or Supersection */

/* Table B3-10 TEX, C, and B encodings when TRE == 0 */

/* Outer and Inner Write-Back, Write-Allocate */
// Cacheable memory attributes, without TEX remap
// DDI0406C_d_armv7ar_arm.pdf
// Table B3-11 Inner and Outer cache attribute encoding
#define ATTR_AA_WBCACHE 0x01	// Inner attribute - Write-Back, Write-Allocate
#define ATTR_BB_WBCACHE 0x01	// Outer attribute - Write-Back, Write-Allocate
#define TEXval_WBCACHE		(0x04 | (ATTR_BB_WBCACHE))
#define Cval_WBCACHE		(((ATTR_AA_WBCACHE) & 0x02) != 0)
#define Bval_WBCACHE		(((ATTR_AA_WBCACHE) & 0x01) != 0)
#if WITHSMPSYSTEM
	#define SHAREDval_WBCACHE 1		// required for ldrex.. and strex.. functionality
#else /* WITHSMPSYSTEM */
	#define SHAREDval_WBCACHE 0		// If non-zero, Renesas Cortex-A9 hung by buffers
#endif /* WITHSMPSYSTEM */

/* Shareable Device */
#define TEXval_DEVICE       0x00
#define Cval_DEVICE         0
#define Bval_DEVICE         1
#define SHAREDval_DEVICE 	0

// See B3.5.2 in DDI0406C_C_arm_architecture_reference_manual.pdf

#define	TTB_PARA(TEXv, Bv, Cv, DOMAINv, SHAREDv, APv, XNv) ( \
		(SECTIONval) * (1u << 0) |	/* 0b10, Section or Supersection */ \
		!! (Bv) * (1u << 2) |	/* B */ \
		!! (Cv) * (1u << 3) |	/* C */ \
		!! (XNv) * (1u << 4) |	/* XN The Execute-never bit. */ \
		(DOMAINv) * (1u << 5) |	/* DOMAIN */ \
		0 * (1u << 9) |	/* implementation defined */ \
		(((APv) >> 0) & 0x03) * (1u << 10) |	/* AP [1..0] */ \
		((TEXv) & 0x07) * (1u << 12) |	/* TEX */ \
		(((APv) >> 2) & 0x01) * (1u << 15) |	/* AP[2] */ \
		!! (SHAREDv) * (1u << 16) |	/* S */ \
		0 * (1u << 17) |	/* nG */ \
		0 * (1u << 18) |	/* 0 */ \
		0 * (1u << 19) |	/* NS */ \
		0 \
	)

#define	TTB_PARA_CACHED(ro, xn) TTB_PARA(TEXval_WBCACHE, Bval_WBCACHE, Cval_WBCACHE, DOMAINval, SHAREDval_WBCACHE, (ro) ? APROval : APRWval, (xn) != 0)
#define	TTB_PARA_DEVICE 		TTB_PARA(TEXval_DEVICE, Bval_DEVICE, Cval_DEVICE, DOMAINval, SHAREDval_DEVICE, APRWval, 1 /* XN=1 */)
#define	TTB_PARA_NO_ACCESS 		0




static uint32_t ttb_1MB_accessbits(uintptr_t a, int ro, int xn)
{
	const uint32_t addrbase = a & 0xFFF00000;

	if (a < 0x00400000)
		return addrbase | TTB_PARA_CACHED(ro, 0);

	if (a >= 0x40000000 && a < 0xC0000000)			//  DDR3 - 2 GB
		return addrbase | TTB_PARA_CACHED(ro, 0);
//	if (a >= 0x000020000 && a < 0x000038000)			//  SYSRAM - 64 kB
//		return addrbase | TTB_PARA_CACHED(ro, 0);

	return addrbase | TTB_PARA_DEVICE;

}


static void sysinit_ttbr_initialize(void)
{
	extern volatile uint32_t __TTB_BASE;		// �������� �� ������� �������
	volatile uint32_t * const tlbbase = & __TTB_BASE;
	//ASSERT(((uintptr_t) tlbbase & 0x3F00) == 0);

#if 0
	/* Set location of level 1 page table
	; 31:14 - Translation table base addr (31:14-TTBCR.N, TTBCR.N is 0 out of reset)
	; 13:7  - 0x0
	; 6     - IRGN[0] 0x1  (Inner WB WA)
	; 5     - NOS     0x0  (Non-shared)
	; 4:3   - RGN     0x01 (Outer WB WA)
	; 2     - IMP     0x0  (Implementation Defined)
	; 1     - S       0x0  (Non-shared)
	; 0     - IRGN[1] 0x0  (Inner WB WA) */
	__set_TTBR0(((uint32_t)&Image$$TTB$$ZI$$Base) | 0x48);
	__ISB();

	/* Set up domain access control register
	; We set domain 0 to Client and all other domains to No Access.
	; All translation table entries specify domain 0 */
	__set_DACR(1);
	__ISB();
#else
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
	__set_TTBR0((uintptr_t) tlbbase | 0x48);	// TTBR0
	//CP15_writeTTB1((unsigned int) tlbbase | 0x48);	// TTBR1
	  __ISB();

	// Program the domain access register
	//__set_DACR(0x55555555); // domain 15: access are not checked
	__set_DACR(0xFFFFFFFF); // domain 15: access are not checked
#endif

	MMU_InvalidateTLB();

	// ������������ ���������� ��������� RESE�
	L1C_InvalidateDCacheAll();
	L1C_InvalidateICacheAll();
	L1C_InvalidateBTAC();
	L2C_InvAllByWay();

	MMU_Enable();

}

static void ttb_1MB_initialize(uint32_t (* accessbits)(uintptr_t a, int ro, int xn), uintptr_t textstart, uint_fast32_t textsize)
{
	extern volatile uint32_t __TTB_BASE;		// �������� �� ������� �������
	volatile uint32_t * const tlbbase = & __TTB_BASE;
	unsigned i;
	const uint_fast32_t pagesize = (1uL << 20);

	for (i = 0; i < 4096; ++ i)
	{
		const uintptr_t address = (uintptr_t) i << 20;
		tlbbase [i] =  accessbits(address, 0, 0);
	}
	/* ���������� R/O �������� ��� ��������� ������� */
	while (textsize >= pagesize)
	{
		tlbbase [textstart / pagesize] =  accessbits(textstart, 0 * 1, 0);
		textsize -= pagesize;
		textstart += pagesize;
	}
}


static void sysinit_mmu_initialize(void)
{
	// MMU iniitialize

	{
		// SCU inut
		// SCU Control Register
		((volatile uint32_t *) SCU_CONTROL_BASE) [0] &= ~ 0x01;
		((volatile uint32_t *) SCU_CONTROL_BASE) [0x3] = 0;		// SCU Invalidate All Registers in Secure State
		((volatile uint32_t *) SCU_CONTROL_BASE) [0] |= 0x01;	// SCU Control Register
	}

	//enable_SCU();
	// MMU iniitialize
	ttb_1MB_initialize(ttb_1MB_accessbits, 0, 0);
	sysinit_ttbr_initialize();	/* �������� TTBR, ����������� ��� ������ � ��������� MMU */

}

// ����� ��� ���� ����������� �������������
static void sysinit_cache_initialize(void)
{

		L1C_InvalidateDCacheAll();
		L1C_InvalidateICacheAll();
		L1C_InvalidateBTAC();
		L1C_EnableCaches();
		L1C_EnableBTAC();
		//#if (__CORTEX_A == 9U)
			// not set the ACTLR.SMP
			// 0x02: L2 Prefetch hint enable
			__set_ACTLR(__get_ACTLR() | ACTLR_L1PE_Msk | ACTLR_FW_Msk | 0x02);
			__ISB();
			__DSB();
		//#elif (__CORTEX_A == 7U)
			// set the ACTLR.SMP
		//	__set_ACTLR(__get_ACTLR() | ACTLR_SMP_Msk);
		//	__ISB();
		//	__DSB();
		//#endif /* (__CORTEX_A == 9U) */

}

static void sysinit_cache_L2_cpu0_initialize(void)
{
			L2C_Disable();
			* (volatile uint32_t *) ((uintptr_t) L2C_310 + 0x010C) = 0x00000222;	// reg1_data_ram_control
			* (volatile uint32_t *) ((uintptr_t) L2C_310 + 0x0108) = 0x00000211;	// reg1_tag_ram_control
			// Enable Level 2 Cache
			L2C_InvAllByWay();
			L2C_Enable();
}



/*----------------------------------------------------------------------------
  System Initialization
 *----------------------------------------------------------------------------*/
void SystemInit (void)
{
/* do not use global variables because this function is called before
   reaching pre-main. RW section may be overwritten afterwards.          */
  /*
  // Invalidate entire Unified TLB
  __set_TLBIALL(0);

  // Invalidate entire branch predictor array
  __set_BPIALL(0);
  __DSB();
  __ISB();

  //  Invalidate instruction cache and flush branch target cache
  __set_ICIALLU(0);
  __DSB();
  __ISB();

  //  Invalidate data cache
  L1C_InvalidateDCacheAll();

#if (__FPU_PRESENT == 1)
  // Enable FPU
  __FPU_Enable();
#endif

  // Create Translation Table
  MMU_CreateTranslationTable();

  // Enable MMU
  MMU_Enable();

  // Enable Caches
  L1C_EnableCaches();
  L1C_EnableBTAC();

#if (__L2C_PRESENT == 1) 
  // Enable GIC
  L2C_Enable();
#endif

  // IRQ Initialize
  IRQ_Initialize();
  */


    ArmEnableNEON();
	//ArmEnableSWPInstruction();
	sysinit_mmu_initialize();
	sysinit_cache_initialize();	// caches iniitialize
	sysinit_cache_L2_cpu0_initialize();	// L2 cache, SCU initialize


}
