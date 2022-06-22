/**
  ******************************************************************************
  * @file    system_allwnr_t113s3.h
  ******************************************************************************
  */
#ifndef __SYSTEM_ALLWNR_T113S3_H
#define __SYSTEM_ALLWNR_T113S3_H

#ifdef __cplusplus
 extern "C" {
#endif

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

extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_ALLWNR_T113S3_H */
