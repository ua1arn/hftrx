/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_conf.h
  * @version        : v1.0_Cube
  * @brief          : Header for usbd_conf.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hardware.h"
#include "formats.h"	// debug PRINTF, TP and other

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if CPUSTYLE_R7S721
	#include "rza1xx_hal.h"
	#include "rza1xx_hal_usb.h"

#elif CPUSTYLE_STM32MP1
	#include "stm32mp1xx.h"
	#include "stm32mp1xx_hal.h"

#elif CPUSTYLE_STM32H7XX
	#include "stm32h7xx.h"
	#include "stm32h7xx_hal.h"

#elif CPUSTYLE_STM32F7XX
	#include "stm32f7xx.h"
	#include "stm32f7xx_hal.h"

#elif CPUSTYLE_STM32F4XX
	#include "stm32f4xx.h"
	#include "stm32f4xx_hal.h"

#elif CPUSTYLE_XC7Z

	#include "zynq7000_hal.h"

#elif CPUSTYLE_ALLWINNER

	#include "t113s3_hal.h"

#endif

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/** @addtogroup USBD_OTG_DRIVER
  * @brief Driver for Usb device.
  * @{
  */

/** @defgroup USBD_CONF USBD_CONF
  * @brief Configuration file for Usb otg low level driver.
  * @{
  */

/** @defgroup USBD_CONF_Exported_Variables USBD_CONF_Exported_Variables
  * @brief Public variables.
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CONF_Exported_Defines USBD_CONF_Exported_Defines
  * @brief Defines for configuration of the Usb device.
  * @{
  */

/*---------- -----------*/
#define USBD_MAX_NUM_INTERFACES     32U
/*---------- -----------*/
#define USBD_MAX_NUM_CONFIGURATION     1U
/*---------- -----------*/
#define USBD_MAX_STR_DESC_SIZ     512U
/*---------- -----------*/
#define USBD_DEBUG_LEVEL     0U
/*---------- -----------*/
#define USBD_LPM_ENABLED     0U
/*---------- -----------*/
#define USBD_SELF_POWERED     1U

/****************************************/
/* #define for FS and HS identification */
#define DEVICE_FS 		0
#define DEVICE_HS 		1

/**
  * @}
  */

/** @defgroup USBD_CONF_Exported_Macros USBD_CONF_Exported_Macros
  * @brief Aliases.
  * @{
  */

/* Memory management macros make sure to use static memory allocation */
/** Alias for memory allocation. */
//#define USBD_malloc         (void *)USBD_static_malloc

/** Alias for memory release. */
//#define USBD_free           USBD_static_free

/** Alias for memory set. */
#define USBD_memset         memset

/** Alias for memory copy. */
#define USBD_memcpy         memcpy

/** Alias for delay. */
#define USBD_Delay          HAL_Delay

/* DEBUG macros */

#if (USBD_DEBUG_LEVEL > 0)
#define USBD_UsrLog(...)    do { \
							PRINTF(__VA_ARGS__);\
							PRINTF("\n"); \
							} while (0)
#else
#define USBD_UsrLog(...) do {} while (0)
#endif

#if (USBD_DEBUG_LEVEL > 1)

#define USBD_ErrLog(...)    do { \
							PRINTF("ERROR: ") ;\
							PRINTF(__VA_ARGS__);\
							PRINTF("\n"); \
 	 	 	 	 	 	 	 } while (0)
#else
#define USBD_ErrLog(...) do {} while (0)
#endif

#if (USBD_DEBUG_LEVEL > 2)
#define USBD_DbgLog(...)    do { \
								PRINTF("DEBUG : ") ;\
								PRINTF(__VA_ARGS__);\
								PRINTF("\n"); \
							} while (0)
#else
#define USBD_DbgLog(...) do {} while (0)
#endif

/**
  * @}
  */

/** @defgroup USBD_CONF_Exported_Types USBD_CONF_Exported_Types
  * @brief Types.
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CONF_Exported_FunctionsPrototype USBD_CONF_Exported_FunctionsPrototype
  * @brief Declaration of public functions for Usb device.
  * @{
  */

/* Exported functions -------------------------------------------------------*/
void *USBD_static_malloc(uint32_t size);
void USBD_static_free(void *p);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* __USBD_CONF__H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
