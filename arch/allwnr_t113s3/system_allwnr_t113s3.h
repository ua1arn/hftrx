/**
  ******************************************************************************
  * @file    system_stm32mp1xx_A7.h
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32mp1xx_system
  * @{
  */

/**
  * @brief Define to prevent recursive inclusion
  */
#ifndef __SYSTEM_STM32MP1XX_A7_H
#define __SYSTEM_STM32MP1XX_A7_H

#ifdef __cplusplus
 extern "C" {
#endif


/** @addtogroup STM32MP1xx_System_Includes
  * @{
  */

/**
  * @}
  */


/** @addtogroup STM32MP1xx_System_Exported_types
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

/** @addtogroup STM32MP1xx_System_Exported_Constants
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32MP1xx_System_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32MP1xx_System_Exported_Functions
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

#endif /*__SYSTEM_STM32MP1XX_A7_H */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
