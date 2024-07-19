/*
 * allwnr_a64.h
 *
 *  Created on: 31 мая 2022 г.
 *      Author: User
 *
 *       For details of A7 CPUX signals, refer to the technical
 *       reference manual of DDI0464F_cortex_A7_mpcore_r0p5_trm.pdf.
 */

#ifndef ARCH_ALLWNR_A64_H_
#define ARCH_ALLWNR_A64_H_

/**
 * @brief ARM Cortex-A53 Quad-Core (r0p4, revidr=0x80)
 *
 * sun50iw1
 */

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
#define DRAM_SPACE_BASE 		((uintptr_t) 0x40000000)			/*!< (DRAM        ) Base Address - 2GB */

// CPUX related
// Address (It is for Cluster CPU)
//#define CPU_SUBSYS_CTRL_BASE	((uintptr_t) 0x08100000)
#define TimeStamp_STA_BASE		((uintptr_t) 0x08110000)
#define TimeStamp_CTRL_BASE		((uintptr_t) 0x08120000)
#define IDC_BASE				((uintptr_t) 0x08130000)
//#define C0_CPUX_CFG_BASE		((uintptr_t) 0x09010000)
#define C0_CPUX_MBIST_BASE		((uintptr_t) 0x09020000)	// Memory Built In Self Test (MBIST) controller - DDI0414I_cortex_a9_mbist_controller_r4p1_trm.pdf

/* ===========================  Configuration of the ARM Cortex-A Processor and Core Peripherals  ============================ */
#define __CORTEX_A                   53U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0004U      /*!< Core revision r0p4 [15:8] rev [7:0] patch */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

#include "device.h"
//
//#define __IO volatile
//
///* CMSIS compiler specific defines */
//#ifndef   __ASM
//  #define __ASM                                  __asm
//#endif
//#ifndef   __INLINE
//  #define __INLINE                               inline
//#endif
//#ifndef   __FORCEINLINE
//  #define __FORCEINLINE                          __attribute__((always_inline))
//#endif
//#ifndef   __STATIC_INLINE
//  #define __STATIC_INLINE                        static inline
//#endif
//#ifndef   __STATIC_FORCEINLINE
//  #define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static inline
//#endif
//#ifndef   __NO_RETURN
//  #define __NO_RETURN                            __attribute__((__noreturn__))
//#endif
//#ifndef   CMSIS_DEPRECATED
//  #define CMSIS_DEPRECATED                       __attribute__((deprecated))
//#endif
//#ifndef   __USED
//  #define __USED                                 __attribute__((used))
//#endif
//#ifndef   __WEAK
//  #define __WEAK                                 __attribute__((weak))
//#endif
//#ifndef   __PACKED
//  #define __PACKED                               __attribute__((packed, aligned(1)))
//#endif
//#ifndef   __PACKED_STRUCT
//  #define __PACKED_STRUCT                        struct __attribute__((packed, aligned(1)))
//#endif
//#ifndef   __UNALIGNED_UINT16_WRITE
//  #pragma GCC diagnostic push
//  #pragma GCC diagnostic ignored "-Wpacked"
//  #pragma GCC diagnostic ignored "-Wattributes"
//  __PACKED_STRUCT T_UINT16_WRITE { uint16_t v; };
//  #pragma GCC diagnostic pop
//  #define __UNALIGNED_UINT16_WRITE(addr, val)    (void)((((struct T_UINT16_WRITE *)(void *)(addr))->v) = (val))
//#endif
//#ifndef   __UNALIGNED_UINT16_READ
//  #pragma GCC diagnostic push
//  #pragma GCC diagnostic ignored "-Wpacked"
//  #pragma GCC diagnostic ignored "-Wattributes"
//  __PACKED_STRUCT T_UINT16_READ { uint16_t v; };
//  #pragma GCC diagnostic pop
//  #define __UNALIGNED_UINT16_READ(addr)          (((const struct T_UINT16_READ *)(const void *)(addr))->v)
//#endif
//#ifndef   __UNALIGNED_UINT32_WRITE
//  #pragma GCC diagnostic push
//  #pragma GCC diagnostic ignored "-Wpacked"
//  #pragma GCC diagnostic ignored "-Wattributes"
//  __PACKED_STRUCT T_UINT32_WRITE { uint32_t v; };
//  #pragma GCC diagnostic pop
//  #define __UNALIGNED_UINT32_WRITE(addr, val)    (void)((((struct T_UINT32_WRITE *)(void *)(addr))->v) = (val))
//#endif
//#ifndef   __UNALIGNED_UINT32_READ
//  #pragma GCC diagnostic push
//  #pragma GCC diagnostic ignored "-Wpacked"
//  #pragma GCC diagnostic ignored "-Wattributes"
//  __PACKED_STRUCT T_UINT32_READ { uint32_t v; };
//  #pragma GCC diagnostic pop
//  #define __UNALIGNED_UINT32_READ(addr)          (((const struct T_UINT32_READ *)(const void *)(addr))->v)
//#endif
//#ifndef   __ALIGNED
//  #define __ALIGNED(x)                           __attribute__((aligned(x)))
//#endif
//#ifndef   __RESTRICT
//  #define __RESTRICT                             __restrict
//#endif
//#ifndef   __COMPILER_BARRIER
//  #define __COMPILER_BARRIER()                   __ASM volatile("":::"memory")
//#endif

#include "system_allwnr_a64.h"

typedef USB_EHCI_Capability_TypeDef USB_EHCI_CapabilityTypeDef;		/* For ST Middleware compatibility */

enum DMAC_SrcReqType
{
	DMAC_SrcReqSRAM = 0,
	DMAC_SrcReqDRAM = 1,

	DMAC_SrcReqI2S0_RX = 3,
	DMAC_SrcReqI2S1_RX,

	DMAC_SrcReqNAND = 5,

	DMAC_SrcReqUART0_RX = 6,
	DMAC_SrcReqUART1_RX,
	DMAC_SrcReqUART2_RX,
	DMAC_SrcReqUART3_RX,
	DMAC_SrcReqUART4_RX,

	DMAC_SrcReqAudioCodec = 15,

	DMAC_SrcReqUSB0_EP1 = 17,
	DMAC_SrcReqUSB0_EP2,
	DMAC_SrcReqUSB0_EP3,
	DMAC_SrcReqUSB0_EP4,
	DMAC_SrcReqUSB0_EP5,

	DMAC_SrcReqSPI0_RX = 23,
	DMAC_SrcReqSPI1_RX
};

enum DMAC_DstReqType
{
	DMAC_DstReqSRAM = 0,
	DMAC_DstReqDRAM = 1,
	DMAC_DstReqOWA = 2,

	DMAC_DstReqI2S0_TX = 3,
	DMAC_DstReqI2S1_TX,
	DMAC_DstReqI2S2_TX = 27,

	DMAC_DstReqNAND = 5,

	DMAC_DstReqUART0_TX = 6,
	DMAC_DstReqUART1_TX,
	DMAC_DstReqUART2_TX,
	DMAC_DstReqUART3_TX,
	DMAC_DstReqUART4_TX,

	DMAC_DstReqAudioCodec = 15,

	DMAC_DstReqUSB0_EP1 = 17,
	DMAC_DstReqUSB0_EP2,
	DMAC_DstReqUSB0_EP3,
	DMAC_DstReqUSB0_EP4,
	DMAC_DstReqUSB0_EP5,

	DMAC_DstReqSPI0_TX = 23,
	DMAC_DstReqSPI1_TX
};

#if defined (USE_HAL_DRIVER)
 #include "t113s3_hal.h"
#endif /* USE_HAL_DRIVER */

#if 0

__STATIC_FORCEINLINE uint32_t __UQADD8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __ASM ("uqadd8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}


__STATIC_FORCEINLINE uint32_t __UQSUB8(uint32_t op1, uint32_t op2)
{
  uint32_t result;

  __ASM ("uqsub8 %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return(result);
}

#endif

#endif /* ARCH_ALLWNR_A64_H_ */
