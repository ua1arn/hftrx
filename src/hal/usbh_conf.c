/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : Target/usbh_conf.c
  * @version        : v1.0_Cube
  * @brief          : This file implements the board support package for the USB host library
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

#include "hardware.h"

#if WITHUSBHW && defined (WITHUSBHW_HOST)

#include "board.h"
#include "formats.h"
#include "gpio.h"

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"
//#include "usbh_platform.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */
void Error_Handler(void);

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static USBH_StatusTypeDef USBH_Get_USB_Status(HAL_StatusTypeDef hal_status);

/* USER CODE END PFP */

/* Private functions ---------------------------------------------------------*/

/* USER CODE BEGIN 1 */


/* HCD Handle Structure */
static RAMBIGDTCM __ALIGN_BEGIN HCD_HandleTypeDef hhcd_USB_OTG __ALIGN_END;
/* USB Host Core handle declaration */

void host_OTG_HS_EP1_IN_IRQHandler(void)
{
#if defined (WITHUSBHW_HOST)
	HAL_HCD_IRQHandler(& hhcd_USB_OTG);
#endif /* defined (WITHUSBHW_HOST) */
}

void host_OTG_FS_IRQHandler(void)
{
#if defined (WITHUSBHW_HOST)
	HAL_HCD_IRQHandler(& hhcd_USB_OTG);
#endif /* defined (WITHUSBHW_HOST) */
}

void host_OTG_HS_IRQHandler(void)
{
#if defined (WITHUSBHW_HOST)
	HAL_HCD_IRQHandler(& hhcd_USB_OTG);
#endif /* defined (WITHUSBHW_HOST) */
}

void host_USBI0_IRQHandler(void)
{
#if defined (WITHUSBHW_HOST)
	HAL_HCD_IRQHandler(& hhcd_USB_OTG);
#endif /* defined (WITHUSBHW_HOST) */
}

void host_USBI1_IRQHandler(void)
{
#if defined (WITHUSBHW_HOST)
	HAL_HCD_IRQHandler(& hhcd_USB_OTG);
#endif /* defined (WITHUSBHW_HOST) */
}

/* USER CODE END 1 */

/*******************************************************************************
                       LL Driver Callbacks (HCD -> USB Host Library)
*******************************************************************************/
/* MSP Init */


void HAL_HCD_MspInit(HCD_HandleTypeDef* hpcd)
{
#if CPUSTYLE_R7S721
	if (hpcd->Instance == & USB200)
	{
		arm_hardware_set_handler_system(USBI0_IRQn, host_USBI0_IRQHandler);

		/* ---- Supply clock to the USB20(channel 0) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */

		HARDWARE_USB0_INITIALIZE();
	}
	else if (hpcd->Instance == & USB201)
	{
		arm_hardware_set_handler_system(USBI1_IRQn, host_USBI1_IRQHandler);

		/* ---- Supply clock to the USB20(channel 1) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP70;	// Module Stop 70 0: Channel 1 of the USB 2.0 host/function module runs.
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */

		HARDWARE_USB1_INITIALIZE();
	}
#else
	//PRINTF(PSTR("HAL_HCD_MspInit()\n"));
	if (hpcd->Instance == USB_OTG_FS)
	{
		#if CPUSTYLE_STM32MP1

			USBD_EHCI_INITIALIZE();
			// Set 3.3 volt DETECTOR enable
			PWR->CR3 |= PWR_CR3_USB33DEN_Msk;
			(void) PWR->CR3;
			while ((PWR->CR3 & PWR_CR3_USB33DEN_Msk) == 0)
				;

			// Wait 3.3 volt REGULATOR ready
			while ((PWR->CR3 & PWR_CR3_USB33RDY_Msk) == 0)
				;

			RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_USBOEN;
			(void) RCC->MP_AHB2ENSETR;
			RCC->MP_AHB2LPENSETR = RCC_MP_AHB2LPENSETR_USBOLPEN;
			(void) RCC->MP_AHB2LPENSETR;

			arm_hardware_set_handler_system(OTG_IRQn, host_OTG_HS_IRQHandler);

		#elif CPUSTYLE_STM32H7XX

			//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
			//PRINTF(PSTR("HAL_HCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);
			USBD_FS_INITIALIZE();

			RCC->AHB1ENR |= RCC_AHB1ENR_USB2OTGFSEN_Msk;	/* USB/OTG FS  */
			(void) RCC->AHB1ENR;
			RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN_Msk;	/* USB/OTG FS companion - VBUS? */
			(void) RCC->APB2ENR;

			arm_hardware_set_handler_system(OTG_FS_IRQn, host_OTG_FS_IRQHandler);

		#else
			//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
			//PRINTF(PSTR("HAL_HCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);

			USBD_FS_INITIALIZE();

			RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN_Msk;	/* USB/OTG FS  */
			(void) RCC->AHB2ENR;
			RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN_Msk;	/* USB/OTG FS companion - VBUS? */
			(void) RCC->APB2ENR;

			arm_hardware_set_handler_system(OTG_FS_IRQn, host_OTG_FS_IRQHandler);

		#endif
	}
	else if (hpcd->Instance == USB_OTG_HS)
	{
	#if CPUSTYLE_STM32MP1

		USBD_EHCI_INITIALIZE();
		// Set 3.3 volt DETECTOR enable
		PWR->CR3 |= PWR_CR3_USB33DEN_Msk;
		(void) PWR->CR3;
		while ((PWR->CR3 & PWR_CR3_USB33DEN_Msk) == 0)
			;

		// Wait 3.3 volt REGULATOR ready
		while ((PWR->CR3 & PWR_CR3_USB33RDY_Msk) == 0)
			;

		RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_USBOEN;
		(void) RCC->MP_AHB2ENSETR;
		RCC->MP_AHB2LPENSETR = RCC_MP_AHB2LPENSETR_USBOLPEN;
		(void) RCC->MP_AHB2LPENSETR;

		USB_HS_PHYCInit();
		arm_hardware_set_handler_system(OTG_IRQn, host_OTG_HS_IRQHandler);

	#elif CPUSTYLE_STM32H7XX

		//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
		//PRINTF(PSTR("HAL_HCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);
		USBD_FS_INITIALIZE();

		RCC->AHB1ENR |= RCC_AHB1ENR_USB1OTGHSEN_Msk;	/* USB/OTG HS  */
		(void) RCC->AHB1ENR;
		RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN_Msk;	/* USB/OTG HS companion - VBUS? */
		(void) RCC->APB2ENR;

		arm_hardware_set_handler_system(OTG_HS_IRQn, host_OTG_HS_IRQHandler);

	#else
		//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
		//PRINTF(PSTR("HAL_HCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);

		USBD_FS_INITIALIZE();

		RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN_Msk;	/* USB/OTG HS  */
		(void) RCC->AHB2ENR;
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN_Msk;	/* USB/OTG HS companion - VBUS? */
		(void) RCC->APB2ENR;

		arm_hardware_set_handler_system(OTG_HS_IRQn, host_OTG_HS_IRQHandler);


	#endif
	}
	else
	{
		ASSERT(0);
	}
#endif /*  */
}

void HAL_HCD_MspDeInit(HCD_HandleTypeDef* hpcd)
{
#if CPUSTYLE_R7S721
	if (hpcd->Instance == & USB200)
	{
		const IRQn_ID_t int_id = USBI0_IRQn;
		arm_hardware_disable_handler(int_id);

		/* ---- Supply clock to the USB20(channel 0) ---- */
		//CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		//(void) CPG.STBCR7;			/* Dummy read */

		//HARDWARE_USB0_UNINITIALIZE();

	}
	else if (hpcd->Instance == & USB201)
	{
		const IRQn_ID_t int_id = USBI1_IRQn;
		arm_hardware_disable_handler(int_id);

		/* ---- Supply clock to the USB20(channel 1) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP70;	// Module Stop 70 0: Channel 1 of the USB 2.0 host/function module runs.
		//CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */

		//HARDWARE_USB1_UNINITIALIZE();
	}
	hpcd->Instance->SYSCFG0 &= ~ USB_SYSCFG_USBE;
	hpcd->Instance->INTENB0 = 0;
	hpcd->Instance->INTENB1 = 0;

#elif CPUSTYLE_STM32MP1

	if (hpcd->Instance == USB_OTG_HS)
	{
		/* Peripheral interrupt Deinit*/
		arm_hardware_disable_handler(OTG_IRQn);
	}

#else

	#if defined (USB_OTG_HS)
		if (hpcd->Instance == USB_OTG_HS)
		{
			/* Peripheral interrupt Deinit*/
			arm_hardware_disable_handler(OTG_HS_IRQn);
		}
	#endif /* defined (USB_OTG_HS) */

	#if defined (USB_OTG_FS)
		if (hpcd->Instance == USB_OTG_FS)
		{
			/* Peripheral interrupt Deinit*/
			arm_hardware_disable_handler(OTG_FS_IRQn);
		}
	#endif /* defined (USB_OTG_FS) */

#endif
}


/**
  * @brief  SOF callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_IncTimer(hhcd->pData);
}

/**
  * @brief  Connect callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_Connect(hhcd->pData);
}

/**
  * @brief  Disconnect callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_Disconnect(hhcd->pData);
}

/**
  * @brief  Notify URB state change callback.
  * @param  hhcd: HCD handle
  * @param  chnum: channel number
  * @param  urb_state: state
  * @retval None
  */
void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state)
{
  /* To be used with OS to sync URB state with the global state machine */
#if (USBH_USE_OS == 1)
  USBH_LL_NotifyURBChange(hhcd->pData);
#endif
}

/**
* @brief  Port Port Enabled callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_PortEnabled_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_PortEnabled(hhcd->pData);
}

/**
  * @brief  Port Port Disabled callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_PortDisabled_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_PortDisabled(hhcd->pData);
}

/*******************************************************************************
                       LL Driver Interface (USB Host Library --> HCD)
*******************************************************************************/

/**
  * @brief  Initialize the low level portion of the host driver.
  * @param  phost: Host handle
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_Init(USBH_HandleTypeDef *phost)
{

	/* Init USB_IP */
	/* Link The driver to the stack */
	hhcd_USB_OTG.pData = phost;
	phost->pData = & hhcd_USB_OTG;

	hhcd_USB_OTG.Instance = WITHUSBHW_HOST;

#if CPUSTYLE_R7S721
	hhcd_USB_OTG.Init.Host_channels = 16;
	hhcd_USB_OTG.Init.speed = PCD_SPEED_HIGH;
	hhcd_USB_OTG.Init.dma_enable = DISABLE;
	hhcd_USB_OTG.Init.phy_itface = USB_OTG_EMBEDDED_PHY;

#elif CPUSTYLE_STM32MP1
	hhcd_USB_OTG.Init.Host_channels = 16;
	hhcd_USB_OTG.Init.speed = PCD_SPEED_HIGH;
	#if WITHUSBHOST_DMAENABLE
		hhcd_USB_OTG.Init.dma_enable = ENABLE;	 // xyz HOST
	#else /* WITHUSBHOST_DMAENABLE */
		hhcd_USB_OTG.Init.dma_enable = DISABLE;	 // xyz HOST
	#endif /* WITHUSBHOST_DMAENABLE */
	hhcd_USB_OTG.Init.phy_itface = HCD_PHY_EMBEDDED;
	hhcd_USB_OTG.Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
	#if WITHUSBHOST_HIGHSPEEDULPI
		hhcd_USB_OTG.Init.phy_itface = USB_OTG_ULPI_PHY;
	#elif WITHUSBHOST_HIGHSPEEDPHYC
		hhcd_USB_OTG.Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
	#else /* WITHUSBHOST_HIGHSPEEDULPI */
		hhcd_USB_OTG.Init.phy_itface = USB_OTG_EMBEDDED_PHY;
	#endif /* WITHUSBHOST_HIGHSPEEDULPI */

#else /* CPUSTYLE_R7S721 */
	hhcd_USB_OTG.Init.Host_channels = 16;
	hhcd_USB_OTG.Init.speed = PCD_SPEED_FULL;
	#if WITHUSBHOST_DMAENABLE
		hhcd_USB_OTG.Init.dma_enable = ENABLE;	 // xyz HOST
	#else /* WITHUSBHOST_DMAENABLE */
		hhcd_USB_OTG.Init.dma_enable = DISABLE;	 // xyz HOST
	#endif /* WITHUSBHOST_DMAENABLE */
	hhcd_USB_OTG.Init.phy_itface = HCD_PHY_EMBEDDED;

#endif /* CPUSTYLE_R7S721 */

	hhcd_USB_OTG.Init.Sof_enable = DISABLE;
	if (HAL_HCD_Init(& hhcd_USB_OTG) != HAL_OK)
	{
		ASSERT(0);
	}

	USBH_LL_SetTimer(phost, HAL_HCD_GetCurrentFrame(& hhcd_USB_OTG));
	return USBH_OK;
}

/**
  * @brief  De-Initialize the low level portion of the host driver.
  * @param  phost: Host handle
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_DeInit(USBH_HandleTypeDef *phost)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  hal_status = HAL_HCD_DeInit(phost->pData);

  usb_status = USBH_Get_USB_Status(hal_status);

  phost->pData = NULL;

  return usb_status;
}

/**
  * @brief  Start the low level portion of the host driver.
  * @param  phost: Host handle
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_Start(USBH_HandleTypeDef *phost)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  hal_status = HAL_HCD_Start(phost->pData);

  usb_status = USBH_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Stop the low level portion of the host driver.
  * @param  phost: Host handle
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_Stop(USBH_HandleTypeDef *phost)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  hal_status = HAL_HCD_Stop(phost->pData);

  usb_status = USBH_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Return the USB host speed from the low level driver.
  * @param  phost: Host handle
  * @retval USBH speeds
  */
USBH_SpeedTypeDef USBH_LL_GetSpeed(USBH_HandleTypeDef *phost)
{
  USBH_SpeedTypeDef speed = USBH_SPEED_FULL;

  switch (HAL_HCD_GetCurrentSpeed(phost->pData))
  {
  case 0 :
    speed = USBH_SPEED_HIGH;
    break;

  case 1 :
    speed = USBH_SPEED_FULL;
    break;

  case 2 :
    speed = USBH_SPEED_LOW;
    break;

  default:
   speed = USBH_SPEED_FULL;
    break;
  }
  return  speed;
}

/**
  * @brief  Reset the Host port of the low level driver.
  * @param  phost: Host handle
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_ResetPort(USBH_HandleTypeDef *phost)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  hal_status = HAL_HCD_ResetPort(phost->pData);

  usb_status = USBH_Get_USB_Status(hal_status);

  return usb_status;
}
/**
  * @brief  USBH_LL_ResetPort2
  *         Reset the Host Port of the Low Level Driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_ResetPort2(USBH_HandleTypeDef *phost, unsigned resetIsActive)	/* Without delays */
{
	  HAL_StatusTypeDef hal_status = HAL_OK;
	  USBH_StatusTypeDef usb_status = USBH_OK;

	  hal_status = HAL_HCD_ResetPort2(phost->pData, resetIsActive);

	  usb_status = USBH_Get_USB_Status(hal_status);

	  return usb_status;
}

/**
  * @brief  Return the last transferred packet size.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @retval Packet size
  */
uint32_t USBH_LL_GetLastXferSize(USBH_HandleTypeDef *phost, uint8_t pipe)
{
	uint32_t size2 = HAL_HCD_HC_GetXferCount(phost->pData, pipe);
	uint32_t size;
	do
	{
		size = size2;
		size2 = HAL_HCD_HC_GetXferCount(phost->pData, pipe);
	} while (size != size2);
	return size2;
}

/**
  * @brief  Return the maximum possible transferred packet size.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @param  size: expectes transfer packet size
  * @retval Packet size
  */
uint32_t USBH_LL_GetAdjXferSize(USBH_HandleTypeDef *phost, uint8_t pipe, uint32_t size)
{
	  return ulmin32(size, HAL_HCD_HC_GetMaxPacket(phost->pData, pipe));	// Default implementation
}

/**
  * @brief  Open a pipe of the low level driver.
  * @param  phost: Host handle
  * @param  pipe_num: Pipe index
  * @param  epnum: Endpoint number
  * @param  dev_address: Device USB address
  * @param  speed: Device Speed
  * @param  ep_type: Endpoint type
  * @param  mps: Endpoint max packet size
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_OpenPipe(USBH_HandleTypeDef *phost, uint8_t pipe_num, uint8_t epnum,
								const USBH_TargetTypeDef * dev_target,
								uint8_t ep_type,
								uint16_t mps)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  hal_status = HAL_HCD_HC_Init(phost->pData, pipe_num, epnum,
		  dev_target->dev_address, dev_target->speed, ep_type, mps, dev_target->tt_hubaddr, dev_target->tt_prtaddr);

  usb_status = USBH_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Close a pipe of the low level driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_ClosePipe(USBH_HandleTypeDef *phost, uint8_t pipe)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  hal_status = HAL_HCD_HC_Halt(phost->pData, pipe);

  usb_status = USBH_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Submit a new URB to the low level driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  *         This parameter can be a value from 1 to 15
  * @param  direction : Channel number
  *          This parameter can be one of the these values:
  *           0 : Output
  *           1 : Input
  * @param  ep_type : Endpoint Type
  *          This parameter can be one of the these values:
  *            @arg EP_TYPE_CTRL: Control type
  *            @arg EP_TYPE_ISOC: Isochrounous type
  *            @arg EP_TYPE_BULK: Bulk type
  *            @arg EP_TYPE_INTR: Interrupt type
  * @param  token : Endpoint Type
  *          This parameter can be one of the these values:
  *            @arg 0: PID_SETUP
  *            @arg 1: PID_DATA
  * @param  pbuff : pointer to URB data
  * @param  length : Length of URB data
  * @param  do_ping : activate do ping protocol (for high speed only)
  *          This parameter can be one of the these values:
  *           0 : do ping inactive
  *           1 : do ping active
  * @retval Status
  */
USBH_StatusTypeDef USBH_LL_SubmitURB(USBH_HandleTypeDef *phost, uint8_t pipe, uint8_t direction,
                                     uint8_t ep_type, uint8_t token, uint8_t *pbuff, uint32_t length,
                                     uint8_t do_ping)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  hal_status = HAL_HCD_HC_SubmitRequest(phost->pData, pipe, direction ,
                                        ep_type, token, pbuff, length,
                                        do_ping);
  usb_status =  USBH_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Get a URB state from the low level driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  *         This parameter can be a value from 1 to 15
  * @retval URB state
  *          This parameter can be one of the these values:
  *            @arg URB_IDLE
  *            @arg URB_DONE
  *            @arg URB_NOTREADY
  *            @arg URB_NYET
  *            @arg URB_ERROR
  *            @arg URB_STALL
  */
USBH_URBStateTypeDef USBH_LL_GetURBState(USBH_HandleTypeDef *phost, uint8_t pipe)
{

#if WITHINTEGRATEDDSP
	audioproc_spool_user();		// решение проблем с прерыванием звука при записи файлов
#endif /* WITHINTEGRATEDDSP */

	HCD_URBStateTypeDef state2 = HAL_HCD_HC_GetURBState(phost->pData, pipe);
	HCD_URBStateTypeDef state;
	do
	{
		state = state2;
		state2 = HAL_HCD_HC_GetURBState(phost->pData, pipe);
	} while (state != state2);
	return (USBH_URBStateTypeDef) state2;
}

USBH_SpeedTypeDef USBH_LL_GetPipeSpeed(USBH_HandleTypeDef *phost, uint8_t pipe_num)
{
	HCD_HandleTypeDef *hhcd = phost->pData;

	return hhcd->hc [pipe_num].speed;
}

uint_fast8_t USBH_LL_GetSpeedReady(USBH_HandleTypeDef *phost)
{
	return HAL_HCD_GetCurrentSpeedReady(phost->pData);
}

/**
  * @brief  USBH_LL_DriverVBUS
  *         Drive VBUS.
  * @param  phost: Host handle
  * @param  state : VBUS state
  *          This parameter can be one of the these values:
  *           1 : VBUS Active
  *           0 : VBUS Inactive
  * @retval Status
  */
USBH_StatusTypeDef  USBH_LL_DriverVBUS(USBH_HandleTypeDef *phost, uint8_t state)
{
	//PRINTF(PSTR("USBH_LL_DriverVBUS(%d), phost->id=%d, HOST_FS=%d\n"), (int) state, (int) phost->id, (int) HOST_FS);
	if (state != FALSE)
	{
		/* Drive high Charge pump */
		/* ToDo: Add IOE driver control */
		board_set_usbhostvbuson(1);
		board_update();
	}
	else
	{
		/* Drive low Charge pump */
		/* ToDo: Add IOE driver control */
		board_set_usbhostvbuson(0);
		board_update();
	}
	HARDWARE_DELAY_MS(200);
	return USBH_OK;
}

/**
  * @brief  Set toggle for a pipe.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @param  toggle: toggle (0/1)
  * @retval Status
  */
USBH_StatusTypeDef USBH_LL_SetToggle(USBH_HandleTypeDef *phost, uint8_t pipe, uint8_t toggle)
{
  HCD_HandleTypeDef *pHandle;
  pHandle = phost->pData;

  if(pHandle->hc[pipe].ep_is_in)
  {
    pHandle->hc[pipe].toggle_in = toggle;
  }
  else
  {
    pHandle->hc[pipe].toggle_out = toggle;
  }

  return USBH_OK;
}

/**
  * @brief  Return the current toggle of a pipe.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @retval toggle (0/1)
  */
uint8_t USBH_LL_GetToggle(USBH_HandleTypeDef *phost, uint8_t pipe)
{
  uint8_t toggle = 0;
  HCD_HandleTypeDef *pHandle;
  pHandle = phost->pData;

  if(pHandle->hc[pipe].ep_is_in)
  {
    toggle = pHandle->hc[pipe].toggle_in;
  }
  else
  {
    toggle = pHandle->hc[pipe].toggle_out;
  }
  return toggle;
}

/**
  * @brief  Delay routine for the USB Host Library
  * @param  Delay: Delay in ms
  * @retval None
  */
void USBH_Delay(uint32_t Delay)
{
	//HAL_Delay(Delay);
	local_delay_ms(Delay);
}

/**
  * @brief  Returns the USB status depending on the HAL status:
  * @param  hal_status: HAL status
  * @retval USB status
  */
static USBH_StatusTypeDef USBH_Get_USB_Status(HAL_StatusTypeDef hal_status)
{
  USBH_StatusTypeDef usb_status = USBH_OK;

  switch (hal_status)
  {
    case HAL_OK :
      usb_status = USBH_OK;
    break;
    case HAL_ERROR :
      usb_status = USBH_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBH_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBH_FAIL;
    break;
    default :
      usb_status = USBH_FAIL;
    break;
  }
  return usb_status;
}

#endif /* WITHUSBHW && defined (WITHUSBHW_HOST) */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
