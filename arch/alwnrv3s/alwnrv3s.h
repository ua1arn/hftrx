/*
 * alwnrv3s.h
 *
 *      Author: gena
 */

#ifndef ARMCPU_ZYNQ7000_H_
#define ARMCPU_ZYNQ7000_H_

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
/******  SGI Interrupts Numbers                 ****************************************/
  SGI0_IRQn           =  0,                                       //!< SGI0_IRQn
  SGI1_IRQn           =  1,                                       //!< SGI1_IRQn
  SGI2_IRQn           =  2,                                       //!< SGI2_IRQn
  SGI3_IRQn           =  3,                                       //!< SGI3_IRQn
  SGI4_IRQn           =  4,                                       //!< SGI4_IRQn
  SGI5_IRQn           =  5,                                       //!< SGI5_IRQn
  SGI6_IRQn           =  6,                                       //!< SGI6_IRQn
  SGI7_IRQn           =  7,                                       //!< SGI7_IRQn
  SGI8_IRQn           =  8,                                       //!< SGI8_IRQn
  SGI9_IRQn           =  9,                                       //!< SGI9_IRQn
  SGI10_IRQn          = 10,                                       //!< SGI10_IRQn
  SGI11_IRQn          = 11,                                       //!< SGI11_IRQn
  SGI12_IRQn          = 12,                                       //!< SGI12_IRQn
  SGI13_IRQn          = 13,                                       //!< SGI13_IRQn
  SGI14_IRQn          = 14,                                       //!< SGI14_IRQn
  SGI15_IRQn          = 15,                                       //!< SGI15_IRQn

  /* Private Peripheral Interrupts                                                                                     */
  //VirtualMaintenanceInterrupt_IRQn = 25,     /*!< Virtual Maintenance Interrupt                                        */
 // HypervisorTimer_IRQn             = 26,     /*!< Hypervisor Timer Interrupt                                           */
  GlobalTimer_IRQn                 = 27,     /*!< Global Timer Interrupt                                              */
  Legacy_nFIQ_IRQn                 = 28,     /*!< Legacy nFIQ Interrupt                                                */
  PrivTimer_IRQn        	   	   = 29,     /*!< Private Timer Interrupt                                      */
  AwdtTimer_IRQn      			   = 30,     /*!< Private watchdog timer for each CPU Interrupt                                  */
  Legacy_nIRQ_IRQn                 = 31,     /*!< Legacy nIRQ Interrupt                                                */

  /******  V3s  specific Interrupt Numbers ****************************************************************************/

	Force_IRQn_enum_size             = 1048    /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */

} IRQn_Type;

// Allwinner V3s is SoC with build-in ARM Cortex A7 CPU and integrated 64MB DDR2 RAM
//
//	ARM Cortex TM -A7 MP1 Processor
//	Thumb-2 Technology
//	Support NEON Advanced SIMD(Single Instruction Multiple Data)instruction for acceleration of media and signal processing functions
//	Support Large Physical Address Extensions(LPAE)
//	VFPv4 Floating Point Unit
//	32KB L1 Instruction cache and 32KB L1 Data cache
//	128KB L2 cache

/******************************************************************************/
/*                Device Specific Peripheral Section                          */
/******************************************************************************/

//#define CPUPRIV_BASE      (0xF8F00000uL)
#define SCU_CONTROL_BASE  (0x01C80000)
#define GIC_PROC_BASE     (SCU_CONTROL_BASE + 0x2000)
//#define GLOBAL_TIMER_BASE (CPUPRIV_BASE + 0x0200uL)
//#define PRIV_TIMER_BASE   (CPUPRIV_BASE + 0x0600uL)
#define GIC_DISTRIB_BASE  (SCU_CONTROL_BASE + 0x1000)

#define __CORTEX_A                    7U      /*!< Cortex-A# Core                              */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 0U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

#define GIC_DISTRIBUTOR_BASE         GIC_DISTRIB_BASE                        /*!< (GIC DIST  ) Base Address */
#define GIC_INTERFACE_BASE           GIC_PROC_BASE                        /*!< (GIC CPU IF) Base Address */
//#define L2C_310_BASE                 L2CACHE_BASE                        /*!< (PL310     ) Base Address */
//#define TIMER_BASE				PRIV_TIMER_BASE

/* --------  Configuration of the Cortex-A9 Processor and Core Peripherals  ------- */
#define __CA_REV         		    0x0000    /*!< Core revision r0p0       */

#include "core_ca.h"
#include "system_alwnrv3s.h"

// todo: registers here

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


/**
  * @}
  */

#ifdef USE_HAL_DRIVER
	#include "alwnrv3s_hal.h"
#endif /* USE_HAL_DRIVER */


#ifdef __cplusplus
}
#endif


#endif /* ARMCPU_ZYNQ7000_H_ */
