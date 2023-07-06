/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_device.h
  * @version        : v1.0_Cube
  * @brief          : Header for usb_device.c file.
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
#ifndef __USB_DEVICE__H__
#define __USB_DEVICE__H__

#include "hardware.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if CPUSTYLE_R7S721
	#include "rza1xx_hal.h"

#elif CPUSTYLE_STM32MP1
	#include "stm32mp1xx.h"
	#include "stm32mp1xx_hal.h"
	#include "stm32mp1xx_ll_pwr.h"

#elif CPUSTYLE_STM32H7XX
	#include "stm32h7xx.h"
	#include "stm32h7xx_hal.h"
	#include "stm32h7xx_ll_pwr.h"

#elif CPUSTYLE_STM32F7XX
	#include "stm32f7xx.h"
	#include "stm32f7xx_hal.h"
	#include "stm32f7xx_ll_pwr.h"

#elif CPUSTYLE_STM32F4XX
	#include "stm32f4xx.h"
	#include "stm32f4xx_hal.h"
	#include "stm32f4xx_ll_pwr.h"

#elif CPUSTYLE_XC7Z

	#include "zynq7000_hal.h"

#elif CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507

	#include "t113s3_hal.h"

#endif

#include "usbd_def.h"

/* USER CODE BEGIN INCLUDE */
#include "usbd_core.h"
//#include "usbd_desc.h"
//#include "usbd_cdc.h"
//#include "usbd_cdc_if.h"

/* USER CODE BEGIN Includes */
/* USER CODE END Includes */
#include "usbh_core.h"

/* USER CODE END INCLUDE */

/** @addtogroup USBD_OTG_DRIVER
  * @{
  */

/** @defgroup USBD_DEVICE USBD_DEVICE
  * @brief Device file for Usb otg low level driver.
  * @{
  */

/** @defgroup USBD_DEVICE_Exported_Variables USBD_DEVICE_Exported_Variables
  * @brief Public variables.
  * @{
  */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN VARIABLES */

/* USER CODE END VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_DEVICE_Exported_FunctionsPrototype USBD_DEVICE_Exported_FunctionsPrototype
  * @brief Declaration of public functions for Usb device.
  * @{
  */

 /** USB Device initialization function. */
 void MX_USB_DEVICE_Init(void);
 /** USB Device deinitialization function. */
 void MX_USB_DEVICE_DeInit(void);

/*
 * -- Insert functions declaration here --
 */
/* USER CODE BEGIN FD */

extern const USBD_ClassTypeDef USBD_CLASS_UAC;
extern const USBD_ClassTypeDef USBD_CLASS_CDCACM;
extern const USBD_ClassTypeDef USBD_CLASS_DFU;
extern const USBD_ClassTypeDef USBD_CLASS_CDC_EEM;
extern const USBD_ClassTypeDef USBD_CLASS_RNDIS;
extern const USBD_ClassTypeDef USBD_CLASS_HID;
extern const USBD_ClassTypeDef USBD_CLASS_MTP;
extern const USBD_ClassTypeDef USBD_CLASS_MSC;

uint_fast16_t usbd_getuacinrtsmaxpacket(void);
uint_fast16_t usbd_getuacinmaxpacket(void);
uint_fast16_t usbd_getuacoutmaxpacket(void);

void usbd_pipes_initialize(struct _USBD_HandleTypeDef * hpcd);
void usb_device_function0(struct _USBD_HandleTypeDef * hpcd);

/* USER CODE END FD */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
void MX_USB_HOST_Init(void);
void MX_USB_HOST_DeInit(void);

void MX_USB_HOST_Process(void);
void MX_USB_DEVICE_Process(void);

extern USBH_HandleTypeDef hUsbHostHS;

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

#ifdef __cplusplus
}
#endif

#endif /* __USB_DEVICE__H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
