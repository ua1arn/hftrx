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

#if WITHUSBHW

#include "formats.h"

#include "usb_device.h"

#if WITHUSEUSBFLASH
#include "../../Class/MSC/Inc/usbh_msc.h"
#endif /* WITHUSEUSBFLASH */
#include "../../Class/HID/Inc/usbh_hid.h"
#include "../../Class/HUB/Inc/usbh_hub.h"

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void Error_Handler(void);

/* USER CODE END PFP */

#if defined (WITHUSBHW_DEVICE)
	/* USB Device Core handle declaration. */
	RAMBIGDTCM __ALIGN_BEGIN USBD_HandleTypeDef hUsbDeviceHS __ALIGN_END;
#endif /* defined (WITHUSBHW_DEVICE) */

#if defined (WITHUSBHW_HOST)
	/* USB Host Core handle declaration. */
	RAMBIGDTCM __ALIGN_BEGIN USBH_HandleTypeDef hUsbHostHS __ALIGN_END;

	// MORI
//	USBH_HandleTypeDef hUSBHost[5];
//	HCD_HandleTypeDef _hHCD[2];

	static ApplicationTypeDef Appli_state = APPLICATION_IDLE;

#endif /* defined (WITHUSBHW_HOST) */
/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

#if defined (WITHUSBHW_DEVICE)
// BOOTLOADER support
static uint_fast8_t device_vbusbefore;

uint_fast8_t hardware_usbd_get_vbusbefore(void)
{
	return device_vbusbefore;
}

static uint_fast8_t hardware_usbd_get_vbusnow0(void)
{
#if CPUSTYLE_R7S721
	return (WITHUSBHW_DEVICE->INTSTS0 & USB_INTSTS0_VBSTS) != 0;

#elif (CPUSTYLE_STM32F || CPUSTYLE_STM32MP1) && defined (USB_OTG_GOTGCTL_BSESVLD_Msk) && WITHUSBDEV_VBUSSENSE
	return (WITHUSBHW_DEVICE->GOTGCTL & USB_OTG_GOTGCTL_BSESVLD_Msk) != 0;

#else /* CPUSTYLE_R7S721 */
	return 0;

#endif /* CPUSTYLE_R7S721 */
}


uint_fast8_t hardware_usbd_get_vbusnow(void)
{
	uint_fast8_t st0;
	uint_fast8_t st = hardware_usbd_get_vbusnow0();

	do
	{
		st0 = st;
		st = hardware_usbd_get_vbusnow0();
	} while (st0 != st);
	return st;
}

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
	device_vbusbefore = hardware_usbd_get_vbusnow();
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
#if WITHUSBHID
	USBD_AddClass(& hUsbDeviceHS, & USBD_CLASS_HID);
#endif /* WITHUSBHID */

 /* USER CODE BEGIN USB_DEVICE_Init_PostTreatment */
  
  /* USER CODE END USB_DEVICE_Init_PostTreatment */
}

void MX_USB_DEVICE_DeInit(void)
{
	if (USBD_Stop(&hUsbDeviceHS) != USBD_OK)
	{
		Error_Handler();
	}
}

#endif /* defined (WITHUSBHW_DEVICE) */

#if defined (WITHUSBHW_HOST)


/*
 * user callback definition
*/
void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{

	/* USER CODE BEGIN CALL_BACK_1 */
	switch(id)
	{
	case HOST_USER_SELECT_CONFIGURATION:
		break;

	case HOST_USER_DISCONNECTION:
		Appli_state = APPLICATION_DISCONNECT;
		break;

	case HOST_USER_CLASS_ACTIVE:
		Appli_state = APPLICATION_READY;
		break;

	case HOST_USER_CONNECTION:
		Appli_state = APPLICATION_START;
		break;

	default:
		break;
	}
	/* USER CODE END CALL_BACK_1 */
}
// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
static void
board_usb_tspool(void * ctx)
{
#if defined (WITHUSBHW_HOST)
	USBH_Process(& hUsbHostHS);

#endif /* defined (WITHUSBHW_HOST) */
}

void MX_USB_HOST_Init(void)
{
	static ticker_t usbticker;
	/* Init Host Library,Add Supported Class and Start the library*/
	USBH_Init(& hUsbHostHS, USBH_UserProcess, 0);

#if WITHUSEUSBFLASH
	USBH_RegisterClass(& hUsbHostHS, & USBH_msc);
#endif /* WITHUSEUSBFLASH */
#if 1
	USBH_RegisterClass(& hUsbHostHS, & HUB_Class);
	USBH_RegisterClass(& hUsbHostHS, & HID_Class);
#endif /* WITHUSEUSBFLASH */
	//ticker_initialize(& usbticker, 1, board_usb_tspool, NULL);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
	//ticker_add(& usbticker);

}

void MX_USB_HOST_DeInit(void)
{

}

void MX_USB_HOST_Process(void)
{
	USBH_Process(& hUsbHostHS);
}

#endif /* defined (WITHUSBHW_HOST) */

/**
  * @}
  */

/**
  * @}
  */

/* Private user code ---------------------------------------------------------*/
/* This variable is updated in three ways:
    1) by calling CMSIS function SystemCoreClockUpdate()
    2) by calling HAL API function HAL_RCC_GetHCLKFreq()
    3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
       Note: If you use this function to configure the system clock; then there
             is no need to call the 2 first functions listed above, since SystemCoreClock
             variable is updated automatically.
*/
//uint32_t SystemCoreClock = 16000000;
//uint32_t HAL_RCC_GetHCLKFreq(void)
//{
//	return SystemCoreClock;
//}



#endif /* WITHUSBHW */

void board_usb_initialize(void)
{
	//PRINTF("board_usb_initialize\n");
#if WITHUSBDEV_HSDESC
	usbd_descriptors_initialize(1);

#else /* WITHUSBDEV_HSDESC */
	usbd_descriptors_initialize(0);

#endif /* WITHUSBDEV_HSDESC */

#if defined (WITHUSBHW_DEVICE)
	MX_USB_DEVICE_Init();
#endif /* defined (WITHUSBHW_DEVICE) */
#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)
	MX_USB_HOST_Init();
#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) */
	//PRINTF("board_usb_initialize done\n");
}

void board_usb_deinitialize(void)
{
#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)
	MX_USB_HOST_DeInit();
#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) */
#if defined (WITHUSBHW_DEVICE)
	MX_USB_DEVICE_DeInit();
#endif /* defined (WITHUSBHW_DEVICE) */
#if (defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) || defined (WITHUSBHW_DEVICE)) && defined (USBPHYC)
	USB_HS_PHYCDeInit();
#endif /* (defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) || defined (WITHUSBHW_DEVICE)) && defined (USBPHYC) */
}

/* вызывается при разрешённых прерываниях. */
void board_usb_activate(void)
{
	//PRINTF("board_usb_activate\n");
#if defined (WITHUSBHW_DEVICE)
	if (USBD_Start(& hUsbDeviceHS) != USBD_OK)
	{
		Error_Handler();
	}
#endif /* defined (WITHUSBHW_DEVICE) */
#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)
	if (USBH_Start(& hUsbHostHS) != USBH_OK)
	{
		Error_Handler();
	}
#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) */
	//PRINTF("board_usb_activate done\n");
}

void board_usb_deactivate(void)
{
	//PRINTF(PSTR("board_usb_deactivate start.\n"));

#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)
	USBH_Stop(& hUsbHostHS);
#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) */

#if defined (WITHUSBHW_DEVICE)
	USBD_Stop(& hUsbDeviceHS);
#endif /* defined (WITHUSBHW_DEVICE) */

	//PRINTF(PSTR("board_usb_deactivate done.\n"));
}

void board_usbh_polling(void)
{
#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)
	MX_USB_HOST_Process();
#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) */
}

uint_fast8_t hamradio_get_usbh_active(void)
{
#if WITHUSBHW && (defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI))
	return hUsbHostHS.device.is_connected != 0 && hUsbHostHS.gState == HOST_CLASS;
	return hUsbHostHS.device.is_connected != 0;
#else
	return  0;
#endif /* WITHUSBHW && (defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)) */
}

#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)
void USBH_HID_EventCallback(USBH_HandleTypeDef *phost)
{
	for (;;)
	{
		//HID_MOUSE_Info_TypeDef * const p = USBH_HID_GetMouseInfo(phost);
		HID_TOUCH_Info_TypeDef * const p = USBH_HID_GetTouchInfo(phost);

		if (p == NULL)
		{
			//TP();
			break;

		}
		if (p->buttons [0])
		{
			PRINTF("USBH_HID_EventCallback: x/y=%4d/%3d, buttons=%d\n", (int) p->x, (int) p->y, (int) p->buttons [0]);
		}
	}
}
#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
