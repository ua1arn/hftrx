/*
 * allwnr_t13s3.h
 *
 *  Created on: 31 мая 2022 г.
 *      Author: User
 */

#ifndef ARCH_ALLWNR_T13S3_ALLWNR_T13S3_H_
#define ARCH_ALLWNR_T13S3_ALLWNR_T13S3_H_

/**
 * @brief STM32MP1XX Interrupt Number Definition, according to the selected device
 *        in @ref Library_configuration_section
 */
 typedef enum IRQn
 {
   /******  Cortex-A Processor Specific Interrupt Numbers ***************************************************************/
   /* Software Generated Interrupts                                                                                     */
   SGI0_IRQn                        =  0,     /*!< Software Generated Interrupt  0                                      */
   SGI1_IRQn                        =  1,     /*!< Software Generated Interrupt  1                                      */
   SGI2_IRQn                        =  2,     /*!< Software Generated Interrupt  2                                      */
   SGI3_IRQn                        =  3,     /*!< Software Generated Interrupt  3                                      */
   SGI4_IRQn                        =  4,     /*!< Software Generated Interrupt  4                                      */
   SGI5_IRQn                        =  5,     /*!< Software Generated Interrupt  5                                      */
   SGI6_IRQn                        =  6,     /*!< Software Generated Interrupt  6                                      */
   SGI7_IRQn                        =  7,     /*!< Software Generated Interrupt  7                                      */
   SGI8_IRQn                        =  8,     /*!< Software Generated Interrupt  8                                      */
   SGI9_IRQn                        =  9,     /*!< Software Generated Interrupt  9                                      */
   SGI10_IRQn                       = 10,     /*!< Software Generated Interrupt 10                                      */
   SGI11_IRQn                       = 11,     /*!< Software Generated Interrupt 11                                      */
   SGI12_IRQn                       = 12,     /*!< Software Generated Interrupt 12                                      */
   SGI13_IRQn                       = 13,     /*!< Software Generated Interrupt 13                                      */
   SGI14_IRQn                       = 14,     /*!< Software Generated Interrupt 14                                      */
   SGI15_IRQn                       = 15,     /*!< Software Generated Interrupt 15                                      */
   /* Private Peripheral Interrupts                                                                                     */
   VirtualMaintenanceInterrupt_IRQn = 25,     /*!< Virtual Maintenance Interrupt                                        */
   HypervisorTimer_IRQn             = 26,     /*!< Hypervisor Timer Interrupt                                           */
   VirtualTimer_IRQn                = 27,     /*!< Virtual Timer Interrupt                                              */
   Legacy_nFIQ_IRQn                 = 28,     /*!< Legacy nFIQ Interrupt                                                */
   SecurePhysicalTimer_IRQn         = 29,     /*!< Secure Physical Timer Interrupt                                      */
   NonSecurePhysicalTimer_IRQn      = 30,     /*!< Non-Secure Physical Timer Interrupt                                  */
   Legacy_nIRQ_IRQn                 = 31,     /*!< Legacy nIRQ Interrupt                                                */
   /******  STM32 specific Interrupt Numbers ****************************************************************************/
   ////WWDG1_IRQn                       = 32,     /*!< Window WatchDog Interrupt                                            */
   ////PVD_AVD_IRQn                     = 33,     /*!< PVD & AVD detector through EXTI                                      */
   ////TAMP_IRQn                        = 34,     /*!< Tamper interrupts through the EXTI line                              */
   ///
   MAX_IRQ_n,
   Force_IRQn_enum_size             = 1048    /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
 } IRQn_Type;

/* =========================================================================================================================== */
/* ================                           Processor and Core Peripheral Section                           ================ */
/* =========================================================================================================================== */

/* ===========================  Configuration of the ARM Cortex-A Processor and Core Peripherals  ============================ */
#define __CORTEX_A                    7U      /*!< Cortex-A# Core                              */
#define __CA_REV                 0x0005U      /*!< Core revision r0p0                          */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present                  */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present                  */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present                  */
#define __L2C_PRESENT                 0U      /*!< Set to 1 if L2C is present                  */

#define GIC_BASE             0x03020000
#define GIC_DISTRIBUTOR_BASE GIC_BASE
#define GIC_INTERFACE_BASE   (GIC_BASE+0x1000)

#include "core_ca.h"
//#include "system_stm32mp1xx_A7.h"




#endif /* ARCH_ALLWNR_T13S3_ALLWNR_T13S3_H_ */
