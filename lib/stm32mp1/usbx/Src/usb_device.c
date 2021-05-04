/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_device.c
  * @version        : v1.0_Cube
  * @brief          : This file implements the USB Device
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "hardware.h"
#include "formats.h"

#include "src/usb/usbx_core.h"

#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
//#include "usbd_cdc.h"
//#include "usbd_cdc_if.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceHS;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_DEVICE_Init(void)
{
  /* USER CODE BEGIN USB_DEVICE_Init_PreTreatment */
  /* USER CODE END USB_DEVICE_Init_PreTreatment */

//  /* Init Device Library, add supported class and start the library. */
//  if (USBD_Init(&hUsbDeviceHS, &HS_Desc, DEVICE_HS) != USBD_OK)
//  {
//    Error_Handler();
//  }
//  if (USBD_RegisterClass(&hUsbDeviceHS, &USBD_CDC) != USBD_OK)
//  {
//    Error_Handler();
//  }
//  if (USBD_CDC_RegisterInterface(&hUsbDeviceHS, &USBD_Interface_fops_HS) != USBD_OK)
//  {
//    Error_Handler();
//  }
//  if (USBD_Start(&hUsbDeviceHS) != USBD_OK)
//  {
//    Error_Handler();
//  }

	USBD_Init2(& hUsbDeviceHS);

	// поддержка работы бутлоадера на платах, где есть подпитка VBUS от DP через защитные диоды
	//device_vbusbefore = hardware_usbd_get_vbusnow();
	//PRINTF(PSTR("hardware_usbd_initialize: device_vbusbefore=%d\n"), (int) device_vbusbefore);

#if WITHUSBUAC
	USBD_AddClass(& hUsbDeviceHS, & USBD_CLASS_UAC);
#endif /* WITHUSBUAC */
#if WITHUSBCDCACM
	USBD_AddClass(& hUsbDeviceHS, & USBD_CLASS_CDCACM);
#endif /* WITHUSBCDCACM */
#if WITHUSBDFU
	USBD_AddClass(& hUsbDeviceHS, & USBD_CLASS_DFU);
#endif /* WITHUSBDFU */
#if WITHUSBCDCEEM
	USBD_AddClass(& hUsbDeviceHS, & USBD_CLASS_CDC_EEM);
#endif /* WITHUSBCDCEEM */
#if WITHUSBRNDIS
	USBD_AddClass(& hUsbDeviceHS, & USBD_CLASS_RNDIS);
#endif /* WITHUSBRNDIS */

	if (USBD_Start(&hUsbDeviceHS) != USBD_OK)
	{
		Error_Handler();
	}
 /* USER CODE BEGIN USB_DEVICE_Init_PostTreatment */
  
  /* USER CODE END USB_DEVICE_Init_PostTreatment */
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
