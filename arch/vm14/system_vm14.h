/*
 * system_v14.h
 *
 *  Created on: 16 мар. 2023 г.
 *      Author: User
 */

#ifndef ARCH_VM14_SYSTEM_V14_H_
#define ARCH_VM14_SYSTEM_V14_H_



#ifdef __cplusplus
 extern "C" {
#endif


/** @addtogroup VM14_System_Includes
  * @{
  */

/**
  * @}
  */


/** @addtogroup VM14_System_Exported_types
  * @{
  */
  /* This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
extern uint32_t SystemCoreClock;          /*!< System Core1 Clock Frequency  */
extern uint32_t SystemCore1Clock;         /*!< System Core1 Clock Frequency  */
extern uint32_t SystemCore2Clock;         /*!< System Core2 Clock Frequency  */

/**
  * @}
  */

/** @addtogroup VM14_System_Exported_Constants
  * @{
  */

/**
  * @}
  */

/** @addtogroup VM14_System_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup VM14_System_Exported_Functions
  * @{
  */

extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif



#endif /* ARCH_VM14_SYSTEM_V14_H_ */
