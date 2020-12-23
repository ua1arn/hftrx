/*
 * zynq7000.h
 *
 *  Created on: Dec 23, 2020
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
  VirtualMaintenanceInterrupt_IRQn = 25,     /*!< Virtual Maintenance Interrupt                                        */
  HypervisorTimer_IRQn             = 26,     /*!< Hypervisor Timer Interrupt                                           */
  VirtualTimer_IRQn                = 27,     /*!< Virtual Timer Interrupt                                              */
  Legacy_nFIQ_IRQn                 = 28,     /*!< Legacy nFIQ Interrupt                                                */
  SecurePhysicalTimer_IRQn         = 29,     /*!< Secure Physical Timer Interrupt                                      */
  NonSecurePhysicalTimer_IRQn      = 30,     /*!< Non-Secure Physical Timer Interrupt                                  */
  Legacy_nIRQ_IRQn                 = 31,     /*!< Legacy nIRQ Interrupt                                                */
  /******  ZYNQ  specific Interrupt Numbers ****************************************************************************/

  Force_IRQn_enum_size             = 1048    /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */

} IRQn_Type;


/******************************************************************************/
/*                Device Specific Peripheral Section                          */
/******************************************************************************/


#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

//#include "pl310.h"
//#include "gic.h"

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

/*@}*/ /* end of group Zynq7000_Peripherals */


/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/** @addtogroup Zynq7000_MemoryMap Zynq7000 Memory Mapping
  @{
*/
#define Zynq7000_SDRAM_BASE0                  (0x0C000000UL)                        /*!< (SDRAM0    ) Base Address */
#define Zynq7000_USER_AREA0                   (0x10000000UL)                        /*!< (USER0     ) Base Address */
#define Zynq7000_USER_AREA1                   (0x14000000UL)                        /*!< (USER1     ) Base Address */
#define Zynq7000_SPI_IO0                      (0x18000000UL)                        /*!< (SPI_IO0   ) Base Address */
#define Zynq7000_SPI_IO1                      (0x1C000000UL)                        /*!< (SPI_IO1   ) Base Address */
#define Zynq7000_ONCHIP_SRAM_BASE             (0x20000000UL)                        /*!< (SRAM_OC   ) Base Address */
#define Zynq7000_SPI_MIO_BASE                 (0x3fe00000UL)                        /*!< (SPI_MIO   ) Base Address */
#define Zynq7000_BSC_BASE                     (0x3ff00000UL)                        /*!< (BSC       ) Base Address */
#define Zynq7000_PERIPH_BASE0                 (0xe8000000UL)                        /*!< (PERIPH0   ) Base Address */
#define Zynq7000_PERIPH_BASE1                 (0xfcf00000UL)                        /*!< (PERIPH1   ) Base Address */
#define Zynq7000_GIC_DISTRIBUTOR_BASE         (0xe8201000UL)                        /*!< (GIC DIST  ) Base Address */
#define Zynq7000_GIC_INTERFACE_BASE           (0xe8202000UL)                        /*!< (GIC CPU IF) Base Address */

#define __CORTEX_A                    9U      /*!< Cortex-A# Core                              */
//#define __CA_REV                 0x0005U      /*!< Core revision r0p0                          */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 0U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

#define GIC_DISTRIBUTOR_BASE         Zynq7000_GIC_DISTRIBUTOR_BASE                        /*!< (GIC DIST  ) Base Address */
#define GIC_INTERFACE_BASE           Zynq7000_GIC_INTERFACE_BASE                        /*!< (GIC CPU IF) Base Address */


/* --------  Configuration of the Cortex-A9 Processor and Core Peripherals  ------- */
#define __CA_REV         0x0000    /*!< Core revision r0       */

#include "core_ca.h"
#include "system_zynq7000.h"


#ifdef __cplusplus
}
#endif


#endif /* ARMCPU_ZYNQ7000_H_ */
