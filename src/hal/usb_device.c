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
#include "board.h"
#include "formats.h"
#include "gpio.h"

#if WITHUSBHW

#if WITHTINYUSB
#include "tusb.h"
#endif
#include "usb_device.h"

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void Error_Handler(void);

/* USER CODE END PFP */

#if defined (WITHUSBHW_DEVICE)
	/* USB Device Core handle declaration. */
	__ALIGN_BEGIN USBD_HandleTypeDef hUsbDeviceHS __ALIGN_END;
#endif /* defined (WITHUSBHW_DEVICE) */

#if ! WITHTINYUSB && (defined (WITHUSBHW_HOST) || defined (WITHUSBHW_OHCI) || defined (WITHUSBHW_EHCI))
	/* USB Host Core handle declaration. */
	__ALIGN_BEGIN USBH_HandleTypeDef hUsbHostHS __ALIGN_END;

	// MORI
//	USBH_HandleTypeDef hUSBHost[5];
//	HCD_HandleTypeDef _hHCD[2];

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
#if WITHTINYUSB && CFG_TUD_ENABLED
	usbdevice_clk_init();
	tud_init(BOARD_TUD_RHPORT);
#else /* WITHTINYUSB && CFG_TUD_ENABLED */
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
#if WITHUSBDMTP
	USBD_AddClass(& hUsbDeviceHS, & USBD_CLASS_MTP);
#endif /* WITHUSBDMTP */
#if WITHUSBDMSC
	USBD_AddClass(& hUsbDeviceHS, & USBD_CLASS_MSC);
#endif /* WITHUSBDMSC */
#endif /* WITHTINYUSB && CFG_TUD_ENABLED */
}

void MX_USB_DEVICE_DeInit(void)
{
#if WITHTINYUSB && CFG_TUD_ENABLED

#else /* WITHTINYUSB && CFG_TUD_ENABLED */
	if (USBD_Stop(&hUsbDeviceHS) != USBD_OK)
	{
		Error_Handler();
	}
#endif /* WITHTINYUSB && CFG_TUD_ENABLED */
}

#endif /* defined (WITHUSBHW_DEVICE) */

#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) || defined (WITHUSBHW_OHCI)

/* User-mode function */
void MX_USB_HOST_Init(void)
{
#if WITHTINYUSB && CFG_TUH_ENABLED
	board_set_usbhostvbuson(1);
	local_delay_ms(100);
	ohciehci_clk_init();
#if CFG_TUH_HID
	tuh_hid_set_default_protocol(HID_PROTOCOL_REPORT);
#endif /* CFG_TUH_HID */
	tuh_init(BOARD_TUH_RHPORT);
#else /* WITHTINYUSB && CFG_TUH_ENABLED */
	/* Init Host Library, Add Supported Class and Start the library*/
	USBH_Init(& hUsbHostHS, USBH_UserProcess, 0);

#if WITHUSEUSBBT
	USBH_RegisterClass(& hUsbHostHS, USBH_BLUETOOTH_CLASS);
#endif /* WITHUSEUSBBT */
#if WITHUSEUSBFLASH
	USBH_RegisterClass(& hUsbHostHS, & USBH_msc);
#endif /* WITHUSEUSBFLASH */
#if 1
	USBH_RegisterClass(& hUsbHostHS, & HUB_Class);
	USBH_RegisterClass(& hUsbHostHS, & HID_Class);
#endif /* WITHUSEUSBFLASH */
#endif /* WITHTINYUSB && CFG_TUH_ENABLED */
}

/* User-mode function */
void MX_USB_HOST_DeInit(void)
{
#if WITHTINYUSB && CFG_TUH_ENABLED
//	tuh_deinit(BOARD_TUH_RHPORT);
//	ohciehci_clk_deinit();
#else /* WITHTINYUSB */
	USBH_DeInit(& hUsbHostHS);
#endif
}

#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) || defined (WITHUSBHW_OHCI) */

#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)
/* User-mode function */
void MX_USB_HOST_Process(void)
{
#if WITHTINYUSB && CFG_TUH_ENABLED
	tuh_task();
#else
	USBH_Process(& hUsbHostHS);
#endif
#if WITHEHCIHWSOFTSPOLL
	EHCI_HandleTypeDef * const hehci = (EHCI_HandleTypeDef*) hUsbHostHS.pData;
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	LCLSPIN_LOCK(& hehci->asynclock);
	HAL_EHCI_IRQHandler(& hehci_USB);
	HAL_OHCI_IRQHandler(& hehci_USB);
	LCLSPIN_UNLOCK(& hehci->asynclock);
	LowerIrql(oldIrql);
#endif
}

#endif /* defined (WITHUSBHW_HOST) */

#if defined (WITHUSBHW_DEVICE)

/* User-mode function */
void MX_USB_DEVICE_Process(void)
{
#if (CPUSTYLE_ALLWINNER) && WITHUSBHW && defined (WITHUSBHW_DEVICE) && ! (WITHTINYUSB && CFG_TUD_ENABLED)
    usb_device_function0(&hUsbDeviceHS);
#endif /* CPUSTYLE_ALLWINNER */
}
#endif /* defined (WITHUSBHW_DEVICE) */
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

static void board_usb_dpc(void * ctx)
{
#if WITHUSBHW
#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)
#if WITHTINYUSB
//#if WITHEHCIHWSOFTSPOLL
//	hcd_int_handler(BOARD_TUH_RHPORT, 0);
//#endif
#if CFG_TUH_ENABLED
    tuh_task();
#endif
#if CFG_TUD_ENABLED
    tud_task();
#endif
 #else /* WITHTINYUSB */
	MX_USB_HOST_Process();
#endif
#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) */
#if defined (WITHUSBHW_DEVICE)
	MX_USB_DEVICE_Process();
#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) */
#endif /* WITHUSBHW */
}

static dpcobj_t usb_dpc_entry;

void board_usb_initialize(void)
{
#if WITHUSBHW
	//PRINTF("board_usb_initialize [%p]\n", board_usb_initialize);

#if defined (WITHUSBHW_DEVICE)
	MX_USB_DEVICE_Init();
#endif /* defined (WITHUSBHW_DEVICE) */
#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)
	MX_USB_HOST_Init();
#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) */
	dpcobj_initialize(& usb_dpc_entry, board_usb_dpc, NULL);
	board_dpc_addentry(& usb_dpc_entry, board_dpc_coreid());
	//PRINTF("board_usb_initialize done\n");
#endif /* WITHUSBHW */
}

void board_usb_deinitialize(void)
{
#if WITHUSBHW
	//PRINTF("board_usb_deinitialize [%p]\n", board_usb_deinitialize);
	board_dpc_delentry(& usb_dpc_entry);
#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)
	MX_USB_HOST_DeInit();
#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) */
#if defined (WITHUSBHW_DEVICE)
	MX_USB_DEVICE_DeInit();
#endif /* defined (WITHUSBHW_DEVICE) */
#if (defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) || defined (WITHUSBHW_DEVICE)) && defined (USBPHYC)
	USB_HS_PHYCDeInit();
#endif /* (defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) || defined (WITHUSBHW_DEVICE)) && defined (USBPHYC) */
	//PRINTF("board_usb_deinitialize done\n");
#endif /* WITHUSBHW */
}

/* вызывается при разрешённых прерываниях. */
void board_usb_activate(void)
{
#if WITHUSBHW
	//PRINTF("board_usb_activate [%p]\n", board_usb_activate);
#if defined (WITHUSBHW_DEVICE)
#if WITHUSBDEV_HSDESC
	usbd_descriptors_initialize(1, hamradio_get_ft8cn());

#else /* WITHUSBDEV_HSDESC */
	usbd_descriptors_initialize(0, hamradio_get_ft8cn());

#endif /* WITHUSBDEV_HSDESC */
	//PRINTF("board_usb_activate\n");

#if WITHTINYUSB && CFG_TUD_ENABLED
#else /* WITHTINYUSB && CFG_TUD_ENABLED */

	if (USBD_Start(& hUsbDeviceHS) != USBD_OK)
	{
		Error_Handler();
	}
#endif /* WITHTINYUSB && CFG_TUD_ENABLED */

#endif /* defined (WITHUSBHW_DEVICE) */
#if ! WITHTINYUSB && (defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI))
	if (USBH_Start(& hUsbHostHS) != USBH_OK)
	{
		Error_Handler();
	}
#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) */
	//PRINTF("board_usb_activate done\n");
#endif /* WITHUSBHW */
}

void board_usb_deactivate(void)
{
#if WITHTINYUSB
#else /* WITHTINYUSB */
#if WITHUSBHW
	//PRINTF(PSTR("board_usb_deactivate start.\n"));
#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)
	USBH_Stop(& hUsbHostHS);
#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) */
#if defined (WITHUSBHW_DEVICE)
	USBD_Stop(& hUsbDeviceHS);
#endif /* defined (WITHUSBHW_DEVICE) */
	//PRINTF(PSTR("board_usb_deactivate done.\n"));
#endif /* WITHUSBHW */
#endif /* WITHTINYUSB */
}

#if ! WITHTINYUSB && (defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI))


/** Status of the application. */
typedef enum {
  APPLICATION_IDLE = 0,
  APPLICATION_START,
  APPLICATION_READY,
  APPLICATION_DISCONNECT
} ApplicationTypeDef;

ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * user callback definition
*/
void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{

	/* USER CODE BEGIN CALL_BACK_1 */
	switch(id)
	{
	case HOST_USER_SELECT_CONFIGURATION:
		TP();
        //phost->device.CfgDesc.bConfigurationValue = 2;
        break;

	case HOST_USER_DISCONNECTION:
		Appli_state = APPLICATION_DISCONNECT;
		switch (USBH_GetActiveClass(phost))
		{
		default:
			/* Дескрипторы уже обнулены... */
			TP();
			PRINTF("Undefined device disconnected.\n");
			break;

#if WITHUSEUSBFLASH
		case USB_MSC_CLASS:
			TP();
			PRINTF("MSC device disconnected.\n");
			break;
#endif /* WITHUSEUSBFLASH */

//		case AC_CLASS:
//			TP();
//		      PRINTF("AUDIO device disconnected.\n");
//			break;
		case USB_HID_CLASS:
			TP();
			PRINTF("HID device disconnected.\n");
			TP();
			break;
		case USB_HUB_CLASS:
			TP();
			PRINTF("HUB device disconnected.\n");
			break;
		}
		break;

	case HOST_USER_CLASS_ACTIVE:
		TP();
		Appli_state = APPLICATION_READY;
	    switch(USBH_GetActiveClass(phost))
	    {
#if WITHUSEUSBFLASH
	    case USB_MSC_CLASS:
	      //Appli_state = APPLICATION_MSC;
	      TP();
	      PRINTF("MSC device active.\n");
	      /* Link the USB disk I/O driver */
	      //FATFS_LinkDriver(&USBH_Driver, USBDISKPath);
	      break;
#endif /* WITHUSEUSBFLASH */

//	    case AC_CLASS:
//	      Appli_state = APPLICATION_AUDIO;
//	      TP();
//	      PRINTF("AUDIO device active.\n");
//	      /* Init SD Storage */
//	      if (SD_StorageInit() == 0)
//	      {
//	        SD_StorageParse();
//	      }
//	      break;

	    case USB_HID_CLASS:
			//Appli_state = APPLICATION_HID;
			TP();
			PRINTF("HID device active.\n");
			break;
		case USB_HUB_CLASS:
			//Appli_state = APPLICATION_HUB;
			TP();
			PRINTF("HUB device active.\n");
			break;
	    }
		break;

	case HOST_USER_CONNECTION:
		TP();
		Appli_state = APPLICATION_START;
		break;

	default:
		break;
	}
	/* USER CODE END CALL_BACK_1 */
}

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

/* так как инициализация USB может происходить при запрещённых
 * прерываниях, работа с systick пока невозможна
 */
void HAL_Delay(uint32_t Delay)
{
	local_delay_ms(Delay);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
