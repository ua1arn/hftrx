/**
  ******************************************************************************
  * @file    usbh_conf_template.h
  * @author  MCD Application Team
  * @brief   Header file for usbh_conf_template.c
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBH_CONF_TEMPLATE_H
#define __USBH_CONF_TEMPLATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hardware.h"
#include "formats.h"

/* Includes ------------------------------------------------------------------*/


#if CPUSTYLE_R7S721
	#include "rza1xx_hal.h"

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @addtogroup STM32_USB_HOST_LIBRARY
  * @{
  */

/** @defgroup USBH_CONF
  * @brief usb host low level driver configuration file
  * @{
  */

/** @defgroup USBH_CONF_Exported_Defines
  * @{
  */

#define USBH_MAX_NUM_ENDPOINTS                16U
#define USBH_MAX_NUM_INTERFACES               16U
#define USBH_MAX_NUM_CONFIGURATION            2U
#define USBH_KEEP_CFG_DESCRIPTOR              1U
#define USBH_MAX_NUM_SUPPORTED_CLASS          16U
#define USBH_MAX_SIZE_CONFIGURATION           0x800U
#define USBH_MAX_DATA_BUFFER                  0x200U
#define USBH_DEBUG_LEVEL                      0U
#define USBH_USE_OS                           0U

/** @defgroup USBH_Exported_Macros
  * @{
  */

/* Memory management macros */
//#define USBH_malloc               malloc
//#define USBH_free                 free
#define USBH_memset               memset
#define USBH_memcpy               memcpy

/* DEBUG macros */
#if (USBH_DEBUG_LEVEL > 0U)
#define  USBH_UsrLog(...)   do { \
                                 PRINTF(__VA_ARGS__); \
                                 PRINTF("\n"); \
                               } while (0)
#else
#define USBH_UsrLog(...) do {} while (0)
#endif

#if (USBH_DEBUG_LEVEL > 1U)

#define  USBH_ErrLog(...) do { \
                               PRINTF("ERROR: ") ; \
                               PRINTF(__VA_ARGS__); \
                               PRINTF("\n"); \
                             } while (0)
#else
#define USBH_ErrLog(...) do {} while (0)
#endif

#if (USBH_DEBUG_LEVEL > 2U)
#define  USBH_DbgLog(...)   do { \
                                 PRINTF("DEBUG : ") ; \
                                 PRINTF(__VA_ARGS__); \
                                 PRINTF("\n"); \
                               } while (0)
#else
#define USBH_DbgLog(...) do {} while (0)
#endif

/**
  * @}
  */



/**
  * @}
  */


/** @defgroup USBH_CONF_Exported_Types
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_CONF_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_CONF_Exported_Variables
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_CONF_Exported_FunctionsPrototype
  * @{
  */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBH_CONF_TEMPLATE_H */


/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
