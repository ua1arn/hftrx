/*
 * allwnr_t13s3.h
 *
 *  Created on: 31 мая 2022 г.
 *      Author: User
 *
 *       For details of A7 CPUX signals, refer to the technical
 *       reference manual of DDI0464F_cortex_A7_mpcore_r0p5_trm.pdf.
 */

#ifndef ARCH_ALLWNR_T113S3_ALLWNR_T13S3_H_
#define ARCH_ALLWNR_T113S3_ALLWNR_T13S3_H_

/**
 * @brief ARM Cortex-A53 Quad-Core (r0p4, revidr=0x80)
 */

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/

// CPUX related
// Address (It is for Cluster CPU)
//#define CPU_SUBSYS_CTRL_BASE	((uintptr_t) 0x08100000)
#define TimeStamp_STA_BASE		((uintptr_t) 0x08110000)
#define TimeStamp_CTRL_BASE		((uintptr_t) 0x08120000)
#define IDC_BASE				((uintptr_t) 0x08130000)
//#define C0_CPUX_CFG_BASE		((uintptr_t) 0x09010000)
#define C0_CPUX_MBIST_BASE		((uintptr_t) 0x09020000)	// Memory Built In Self Test (MBIST) controller - DDI0414I_cortex_a9_mbist_controller_r4p1_trm.pdf

// DRAM Space (SYS domain)
#define DRAM_SPACE_SIZE			0x08000000u			/* 128 MB */
#define DRAM_SPACE_BASE 		((uintptr_t) 0x40000000)			/*!< (DRAM        ) Base Address - 2GB */
#define DSP0_IRAM_BASE 			((uintptr_t) 0x00028000)			/* 32KB */
#define DSP0_DRAM_BASE 			((uintptr_t) 0x00030000)			/* 32KB */

//#define G2D_TOP_BASE        (0x00000 + G2D_BASE)
//#define G2D_MIXER_BASE      (0x00100 + G2D_BASE)
//#define G2D_BLD_BASE        (0x00400 + G2D_BASE)
//#define G2D_V0_BASE         (0x00800 + G2D_BASE)
//#define G2D_UI0_BASE        (0x01000 + G2D_BASE)
//#define G2D_UI1_BASE        (0x01800 + G2D_BASE)
//#define G2D_UI2_BASE        (0x02000 + G2D_BASE)
//#define G2D_WB_BASE         (0x03000 + G2D_BASE)
//#define G2D_VSU_BASE        (0x08000 + G2D_BASE)
//#define G2D_ROT_BASE        (0x28000 + G2D_BASE)
//#define G2D_GSU_BASE        (0x30000 + G2D_BASE)

//#define	GIC_DISTRIBUTOR_BASE	 ((uintptr_t) 0x03021000)
//#define	GIC_INTERFACE_BASE	 ((uintptr_t) 0x03022000)

/* ===========================  Configuration of the ARM Cortex-A Processor and Core Peripherals  ============================ */
#define __CORTEX_A                   53U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0000U      /*!< Core revision r0p0                          */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

#include "cmsis_t507.h"
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

#include "core_ca.h"
#include "system_allwnr_t507.h"

typedef USB_EHCI_Capability_TypeDef USB_EHCI_CapabilityTypeDef;		/* For ST Middleware compatibility */

enum DMAC_SrcReqType
{
	DMAC_SrcReqSRAM = 0,
	DMAC_SrcReqDRAM = 1,
	DMAC_SrcReqI2S0_RX = 3,
	DMAC_SrcReqI2S1_RX = 4,
	DMAC_SrcReqNAND = 5,
	DMAC_SrcReqUART0_RX = 6,
	DMAC_SrcReqUART1_RX = 7,
	DMAC_SrcReqUART2_RX = 8,
	DMAC_SrcReqUART3_RX = 9,
	DMAC_SrcReqUART4_RX = 10,
	DMAC_SrcReqAudioCodec = 15,
	DMAC_SrcReqUSB0_EP1 = 17,
	DMAC_SrcReqUSB0_EP2 = 18,
	DMAC_SrcReqUSB0_EP3 = 19,
	DMAC_SrcReqUSB0_EP4 = 20,
	DMAC_SrcReqUSB0_EP5 = 21,
	DMAC_SrcReqSPI0_RX = 23,
	DMAC_SrcReqSPI1_RX = 24
};

enum DMAC_DstReqType
{
	DMAC_DstReqSRAM = 0,
	DMAC_DstReqDRAM = 1,
	DMAC_DstReqOWA = 2,
	DMAC_DstReqI2S0_TX = 3,
	DMAC_DstReqI2S1_TX = 4,
	DMAC_DstReqI2S2_TX = 27,
	DMAC_DstReqNAND = 5,
	DMAC_DstReqUART0_TX = 6,
	DMAC_DstReqUART1_TX = 7,
	DMAC_DstReqUART2_TX = 8,
	DMAC_DstReqUART3_TX = 9,
	DMAC_DstReqUART4_TX = 10,
	DMAC_DstReqAudioCodec = 15,
	DMAC_DstReqUSB0_EP1 = 17,
	DMAC_DstReqUSB0_EP2 = 18,
	DMAC_DstReqUSB0_EP3 = 19,
	DMAC_DstReqUSB0_EP4 = 20,
	DMAC_DstReqUSB0_EP5 = 21,
	DMAC_DstReqSPI0_TX = 23,
	DMAC_DstReqSPI1_TX = 24
};

/** @addtogroup Exported_types
  * @{
  */
typedef enum
{
  RESET = 0,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0,
  ENABLE = !DISABLE
} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum
{
  ERROR = 0,
  SUCCESS = !ERROR
} ErrorStatus;

/**
  * @}
  */


/** @addtogroup Exported_macros
  * @{
  */
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))

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

#endif /* ARCH_ALLWNR_T113S3_ALLWNR_T13S3_H_ */
