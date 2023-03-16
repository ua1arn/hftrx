/*
 * elvees_vm14.h
 *
 *  Created on: 16 мар. 2023 г.
 *      Author: User
 */

#ifndef ARCH_VM14_ELVEES_VM14_H_
#define ARCH_VM14_ELVEES_VM14_H_

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

  /* Private Peripheral Interrupts */
  //VirtualMaintenanceInterrupt_IRQn = 25,     /*!< Virtual Maintenance Interrupt */
 // HypervisorTimer_IRQn             = 26,     /*!< Hypervisor Timer Interrupt */
  GlobalTimer_IRQn                 = 27,     /*!< Global Timer Interrupt */
  Legacy_nFIQ_IRQn                 = 28,     /*!< Legacy nFIQ Interrupt */
  PrivTimer_IRQn        	   	   = 29,     /*!< Private Timer Interrupt */
  AwdtTimer_IRQn      			   = 30,     /*!< Private watchdog timer for each CPU Interrupt */
  Legacy_nIRQ_IRQn                 = 31,     /*!< Legacy nIRQ Interrupt */

  /******  VM14  specific Interrupt Numbers ****************************************************************************/

	/* interrupts */

	Force_IRQn_enum_size             = 1048    /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */

} IRQn_Type;


/* configuration for the PL310 L2 cache controller */
#define PL310_BASE L2CACHE_BASE
#define PL310_TAG_RAM_LATENCY ((1uL << 8) | (1uL << 4) | (1uL << 0))
#define PL310_DATA_RAM_LATENCY ((1uL << 8) | (2uL << 4) | (1uL << 0))

#define GIC_DISTRIBUTOR_BASE         0x39001000                        /*!< (GIC DIST  ) Base Address */
#define GIC_INTERFACE_BASE           0x39000100                        /*!< (GIC CPU IF) Base Address */
#define L2C_310_BASE                 L2CACHE_BASE                        /*!< (PL310     ) Base Address */
#define TIMER_BASE				PRIV_TIMER_BASE


/* --------  Configuration of the Cortex-A9 Processor and Core Peripherals  ------- */
#define __CA_REV         		    0x0000    /*!< Core revision r0p0 */

#define __CORTEX_A                    9U      /*!< Cortex-A# Core */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present */
#define __TIM_PRESENT                 0U//1U      /*!< Set to 1 if TIM is present */
#define __L2C_PRESENT                 0U//1U      /*!< Set to 1 if L2C is present */

#include "core_ca.h"
#include <arch/vm14/system_vm14.h>




#endif /* ARCH_VM14_ELVEES_VM14_H_ */
