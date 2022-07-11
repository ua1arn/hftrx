/* $Id$ */
// allwnr_hal_usb.c
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//


#include "hardware.h"

#if CPUSTYPE_T113

#include "board.h"
#include "audio.h"
#include "formats.h"

#include "src/usb/usb200.h"
#include "src/usb/usbch9.h"

#include "usb_device.h"
#include "usbd_core.h"
#include "usbh_core.h"
#include "usbh_def.h"

#include "allwnr_t113s3_hal.h"
#include "allwnr_hal_usb.h"


/**
  * @brief  Set a STALL condition over an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  PCD_EPTypeDef *ep;

  if (((uint32_t)ep_addr & EP_ADDR_MSK) > hpcd->Init.dev_endpoints)
  {
    return HAL_ERROR;
  }

  if ((0x80U & ep_addr) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 1U;
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr];
    ep->is_in = 0U;
  }

  ep->is_stall = 1U;
  ep->num = ep_addr & EP_ADDR_MSK;
//
//  __HAL_LOCK(hpcd);
//
//#if ! WITHNEWUSBHAL
//  (void)USB_EPSetStall(hpcd->Instance, ep);
//#else
//  if ((ep_addr & EP_ADDR_MSK) == 0U)
//  {
//	  USBPhyHw_ep0_stall(hpcd);
//  }
//  else
//  {
//	  USBPhyHw_endpoint_stall(hpcd, ep_addr);
//
//  }
//#endif
//  if ((ep_addr & EP_ADDR_MSK) == 0U)
//  {
//    (void)USB_EP0_OutStart(hpcd->Instance, (uint8_t)hpcd->Init.dma_enable, (uint8_t *)hpcd->Setup);
//  }
//
//  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}
// https://github.com/abmwine/FreeBSD-src/blob/86cb59de6f4c60abd0ea3695ebe8fac26ff0af44/sys/dev/usb/controller/musb_otg_allwinner.c
// https://github.com/abmwine/FreeBSD-src/blob/86cb59de6f4c60abd0ea3695ebe8fac26ff0af44/sys/dev/usb/controller/musb_otg.c

void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
{
	//PCD_HandleTypeDef * const hpcd = & hpcd_USB_OTG;
	USBOTG_TypeDef * const USBx = hpcd->Instance;

	const unsigned intusb = USBx->USB_INTUSB;// & USBx->USB_INTUSBE;
	const unsigned inttx  = USBx->USB_INTTX;// & USBx->USB_INTTXE;
	const unsigned intrx  = USBx->USB_INTRX;// & USBx->USB_INTRXE;

	//PRINTF("HAL_PCD_IRQHandler: INTUSBE=%08lX INTRXE=%08lX INTTXE=%08lX\n", USBx->USB_INTUSBE, USBx->USB_INTRXE, USBx->USB_INTTXE);
	//PRINTF("HAL_PCD_IRQHandler: INTUSB=%08lX INTRX=%08lX INTTX=%08lX, frame=%04X\n", USBx->USB_INTUSB, USBx->USB_INTRX, USBx->USB_INTTX, USBx->USB_FRAME);

	/* check for any bus state change interrupts */

	if (intusb & (MUSB2_MASK_IRESET |
	    MUSB2_MASK_IRESUME | MUSB2_MASK_ISUSP |
	    MUSB2_MASK_ICONN | MUSB2_MASK_IDISC |
	    MUSB2_MASK_IVBUSERR))
	{
		//PRINTF("HAL_PCD_IRQHandler: bus interrupt\n");
		if (intusb & MUSB2_MASK_IRESET)
		{
			PRINTF("HAL_PCD_IRQHandler: bus interrupt MUSB2_MASK_IRESET\n");
		}
		if (intusb & MUSB2_MASK_IRESUME)
		{
			PRINTF("HAL_PCD_IRQHandler: bus interrupt MUSB2_MASK_IRESUME\n");
		}
		if (intusb & MUSB2_MASK_ISUSP)
		{
			PRINTF("HAL_PCD_IRQHandler: bus interrupt MUSB2_MASK_ISUSP\n");
		}
		if (intusb & MUSB2_MASK_ICONN)
		{
			PRINTF("HAL_PCD_IRQHandler: bus interrupt MUSB2_MASK_ICONN\n");
		}
		if (intusb & MUSB2_MASK_IDISC)
		{
			PRINTF("HAL_PCD_IRQHandler: bus interrupt MUSB2_MASK_IDISC\n");
		}
		if (intusb & MUSB2_MASK_IVBUSERR)
		{
			PRINTF("HAL_PCD_IRQHandler: bus interrupt MUSB2_MASK_IVBUSERR\n");
		}
	}
	if (intrx || inttx)
	{
		PRINTF("HAL_PCD_IRQHandler: real endpoint interrupt "
		    "rx=0x%04x, tx=0x%04x\n", intrx, inttx);
	}
	if ((intusb & MUSB2_MASK_ISOF) != 0)
	{
		// SOF handling
	}

	USBx->USB_INTUSB = intusb;
	USBx->USB_INTTX = inttx;
	USBx->USB_INTRX = intrx;
}

/**
  * @brief  USB_DevConnect : Connect the USB device by enabling Rpu
  * @param  USBx  Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_DevConnect(USBOTG_TypeDef *USBx)
{
	PRINTF("USB_DevConnect\n");
	musb2_start();
	return HAL_OK;
	/* Enable all nnterrupts */
	USBx->USB_INTUSBE = 0xFF & ~ MUSB2_MASK_ISOF;
	USBx->USB_INTRXE = 0x3F;
	USBx->USB_INTTXE = 0x3F;

	PRINTF("USB_DevConnect: USB_INTUSBE=%08lX\n", USBx->USB_INTUSBE);
	PRINTF("USB_DevConnect: USB_INTTXE=%08lX\n", USBx->USB_INTTXE);
	PRINTF("USB_DevConnect: USB_INTRXE=%08lX\n", USBx->USB_INTRXE);
//    /* Enable pullup on D+ */
//    USBx->INTENB0 |= (USB_VBSE | USB_SOFE | USB_DVSE | USB_CTRE | USB_BEMPE | USB_NRDYE | USB_BRDYE);
//    USBx->SYSCFG0 |= USB_DPRPU;
//
//    /* Enable USB */
	//arm_hardware_set_handler_system(USB0_DEVICE_IRQn, device_OTG_HS_IRQHandler);
////    InterruptHandlerRegister(USBIX_IRQn, &_usbisr);
////    GIC_SetPriority(USBIX_IRQn, 16);
////    GIC_SetConfiguration(USBIX_IRQn, 1);
////    GIC_EnableIRQ(USBIX_IRQn);
//	//arm_hardware_set_handler_system(USBIX_IRQn, _usbisr);

    /* Enable pullup on D+ */
	USBx->USB_POWER |= MUSB2_MASK_SOFTC;

    return HAL_OK;
}

/**
  * @brief  USB_DevDisconnect : Disconnect the USB device by disabling Rpu
  * @param  USBx  Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_DevDisconnect(USBOTG_TypeDef *USBx)
{
	musb2_stop();
	return HAL_OK;
	PRINTF("USB_DevDisconnect\n");
	USBx->USB_INTUSBE = 0;
	USBx->USB_INTTXE = 0;
	USBx->USB_INTRXE = 0;
    /* Disable USB */
    //arm_hardware_disable_handler(USB0_DEVICE_IRQn);
////    InterruptHandlerRegister(USBIX_IRQn, NULL);
//    if (USBx == & UDB0)
//	{
//    	arm_hardware_disable_handler(USBI0_IRQn);
//	}
//	else if (USBx == & USB201)
//	{
//		arm_hardware_disable_handler(USBI1_IRQn);
//	}
//
	/* Disable pullup on D+ */
	USBx->USB_POWER &= ~ MUSB2_MASK_SOFTC;

    return HAL_OK;
}

/**
  * @brief  USB_StopDevice : Stop the usb device mode
  * @param  USBx  Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef USB_StopDevice(USBOTG_TypeDef *USBx)
{
  HAL_StatusTypeDef ret = HAL_OK;
//  uint32_t USBx_BASE = (uint32_t)USBx;
//  uint32_t i;
//
//  /* Clear Pending interrupt */
//  for (i = 0U; i < 15U; i++)
//  {
//    USBx_INEP(i)->DIEPINT = 0xFB7FU;
//    USBx_OUTEP(i)->DOEPINT = 0xFB7FU;
//  }
//
//  /* Clear interrupt masks */
//  USBx_DEVICE->DIEPMSK  = 0U;
//  USBx_DEVICE->DOEPMSK  = 0U;
//  USBx_DEVICE->DAINTMSK = 0U;
//
//  /* Flush the FIFO */
//  ret = USB_FlushRxFifo(USBx);
//  if (ret != HAL_OK)
//  {
//    return ret;
//  }
//
//  ret = USB_FlushTxFifo(USBx,  0x10U);
//  if (ret != HAL_OK)
//  {
//    return ret;
//  }

  return ret;
}

/**
  * @brief  USB_SetCurrentMode : Set functional mode
  * @param  USBx : Selected device
  * @param  mode :  current core mode
  *          This parameter can be one of these values:
  *            @arg USB_OTG_DEVICE_MODE: Peripheral mode
  *            @arg USB_OTG_HOST_MODE: Host mode
  *            @arg USB_OTG_DRD_MODE: Dual Role Device mode
  * @retval HAL status
  */
HAL_StatusTypeDef USB_SetCurrentMode(USBOTG_TypeDef *USBx, USB_OTG_ModeTypeDef mode)
{
//	switch (mode)
//	{
//	case USB_OTG_DEVICE_MODE:
//		USBx->SYSCFG0 &= ~ USB_SYSCFG_DCFM;	// DCFM 0: Devide controller mode is selected
//		(void) USBx->SYSCFG0;
//		break;
//	case USB_OTG_HOST_MODE:
//		USBx->SYSCFG0 |= USB_SYSCFG_DCFM;	// DCFM 1: Host controller mode is selected
//		(void) USBx->SYSCFG0;
//		break;
//	case USB_OTG_DRD_MODE:
//		ASSERT(0);
//		//USBx->SYSCFG0 |= USB_SYSCFG_DCFM;	// DCFM 1: Host controller mode is selected
//		break;
//	}

	return HAL_OK;
}

/**
  * @brief  USB_DevInit : Initializes the USB_OTG controller registers
  *         for device mode
  * @param  USBx : Selected device
  * @param  cfg  : pointer to a USB_OTG_CfgTypeDef structure that contains
  *         the configuration information for the specified USBx peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef USB_DevInit(USBOTG_TypeDef *USBx, USB_OTG_CfgTypeDef cfg)
{
	//TP();
//	unsigned i;
//
//	USBx->SYSCFG0 &= ~ USB_SYSCFG_USBE;	// USBE 0: USB module operation is disabled.
//	(void) USBx->SYSCFG0;
//
//	USBx->SOFCFG =
//		//USB_SOFCFG_BRDYM |	// BRDYM
//		0;
//	(void) USBx->SOFCFG;
//
//	USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_DPRPU | USB_SYSCFG_DRPD)) |
//			0 * USB_SYSCFG_DPRPU |	// DPRPU 0: Pulling up the D+ line is disabled.
//			0 * USB_SYSCFG_DRPD |	// DRPD 0: Pulling down the lines is disabled.
//			0;
//	(void) USBx->SYSCFG0;
//
//	USBx->SYSCFG0 |= USB_SYSCFG_USBE;	// USBE 1: USB module operation is enabled.
//	(void) USBx->SYSCFG0;
//
//	//PRINTF("USB_DevInit: cfg->speed=%d\n", (int) cfg->speed);
//	USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_HSE)) |
//			(cfg.speed == PCD_SPEED_HIGH) * USB_SYSCFG_HSE |	// HSE
//			0;
//	(void) USBx->SYSCFG0;
//
//	USBx->INTSTS0 = 0;
//	USBx->INTSTS1 = 0;
//
//	USBx->INTENB0 =
//		(cfg.Sof_enable != DISABLE) * USB_INTENB0_SOFE |	// SOFE	1: Frame Number Update Interrupt Enable
//		1 * USB_INTENB0_DVSE |	// DVSE
//		//1 * USB_INTENB0_VBSE |	// VBSE
//		1 * USB_INTENB0_CTRE |	// CTRE
//		1 * USB_INTENB0_BEMPE |	// BEMPE
//		1 * USB_INTENB0_NRDYE |	// NRDYE
//		1 * USB_INTENB0_BRDYE |	// BRDYE
//		1 * USB_INTENB0_RSME |	// RSME
//		0;
//	USBx->INTENB1 = 0;
//
//	// When the function controller mode is selected, set all the bits in this register to 0.
//	for (i = 0; i < USB20_DEVADD0_COUNT; ++ i)
//	{
//		volatile uint16_t * const DEVADDn = (& USBx->DEVADD0) + i;
//
//		// Reserved bits: The write value should always be 0.
//		* DEVADDn = 0;
//		(void) * DEVADDn;
//	}

	return HAL_OK;
}

HAL_StatusTypeDef USB_CoreInit(USBOTG_TypeDef * USBx, USB_OTG_CfgTypeDef cfg)
{
//	// P1 clock (66.7 MHz max) period = 15 ns
//	// The cycle period required to consecutively access registers of this controller must be at least 67 ns.
//	// TODO: compute BWAIT value on-the-fly
//	// Use P1CLOCK_FREQ
//	const uint_fast32_t bwait = MIN(MAX(calcdivround2(P1CLOCK_FREQ, 15000000uL), 2) - 2, 63);
//	USBx->BUSWAIT = (bwait << USB_BUSWAIT_BWAIT_SHIFT) & USB_BUSWAIT_BWAIT;	// 5 cycles = 75 nS minimum
//	(void) USBx->BUSWAIT;
//
//	USBx->SUSPMODE &= ~ USB_SUSPMODE_SUSPM;	// SUSPM 0: The clock supplied to this module is stopped.
//	(void) USBx->SUSPMODE;
//
//	// This setting shared for USB200 and USB201
//	SYSCFG0_0 = (SYSCFG0_0 & ~ (USB_SYSCFG_UPLLE | USB_SYSCFG_UCKSEL)) |
//		1 * USB_SYSCFG_UPLLE |	// UPLLE 1: Enables operation of the internal PLL.
//		USB_SYSCFG_UCKSEL_Value |	// UCKSEL 1: The 12-MHz EXTAL clock is selected.
//		0;
//	(void) SYSCFG0_0;
//	HARDWARE_DELAY_MS(2);	// required 1 ms delay - see R01UH0437EJ0200 Rev.2.00 28.4.1 System Control and Oscillation Control
//
//	USBx->SUSPMODE |= USB_SUSPMODE_SUSPM;	// SUSPM 1: The clock supplied to this module is enabled.
//	(void) USBx->SUSPMODE;

	return HAL_OK;
}

/**
  * @brief  Initializes the PCD according to the specified
  *         parameters in the PCD_InitTypeDef and create the associated handle.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd)
{
	  USBOTG_TypeDef *USBx;
	  uint8_t i;

	  /* Check the PCD handle allocation */
	  if (hpcd == NULL)
	  {
	    return HAL_ERROR;
	  }

	  /* Check the parameters */
	  //assert_param(IS_PCD_ALL_INSTANCE(hpcd->Instance));

	  USBx = hpcd->Instance;

	  if (hpcd->State == HAL_PCD_STATE_RESET)
	  {
	    /* Allocate lock resource and initialize it */
	    hpcd->Lock = HAL_UNLOCKED;

	#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
	    hpcd->SOFCallback = HAL_PCD_SOFCallback;
	    hpcd->SetupStageCallback = HAL_PCD_SetupStageCallback;
	    hpcd->ResetCallback = HAL_PCD_ResetCallback;
	    hpcd->SuspendCallback = HAL_PCD_SuspendCallback;
	    hpcd->ResumeCallback = HAL_PCD_ResumeCallback;
	    hpcd->ConnectCallback = HAL_PCD_ConnectCallback;
	    hpcd->DisconnectCallback = HAL_PCD_DisconnectCallback;
	    hpcd->DataOutStageCallback = HAL_PCD_DataOutStageCallback;
	    hpcd->DataInStageCallback = HAL_PCD_DataInStageCallback;
	    hpcd->ISOOUTIncompleteCallback = HAL_PCD_ISOOUTIncompleteCallback;
	    hpcd->ISOINIncompleteCallback = HAL_PCD_ISOINIncompleteCallback;
	    hpcd->LPMCallback = HAL_PCDEx_LPM_Callback;
	    hpcd->BCDCallback = HAL_PCDEx_BCD_Callback;

	    if (hpcd->MspInitCallback == NULL)
	    {
	      hpcd->MspInitCallback = HAL_PCD_MspInit;
	    }

	    /* Init the low level hardware */
	    hpcd->MspInitCallback(hpcd);
	#else
	    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
	    HAL_PCD_MspInit(hpcd);
	#endif /* (USE_HAL_PCD_REGISTER_CALLBACKS) */
	  }

	  hpcd->State = HAL_PCD_STATE_BUSY;

	  /* Disable DMA mode for FS instance */
//	  if ((USBx->CID & (0x1U << 8)) == 0U)
//	  {
//	    hpcd->Init.dma_enable = 0U;
//	  }

	  /* Disable the Interrupts */
	  __HAL_PCD_DISABLE(hpcd);

	  /*Init the Core (common init.) */
	  if (USB_CoreInit(USBx, hpcd->Init) != HAL_OK)
	  {
	    hpcd->State = HAL_PCD_STATE_ERROR;
	    return HAL_ERROR;
	  }

	  /* Force Device Mode*/
	  (void)USB_SetCurrentMode(USBx, USB_DEVICE_MODE);

	  /* Init endpoints structures */
	  for (i = 0U; i < hpcd->Init.dev_endpoints; i++)
	  {
	    /* Init ep structure */
	    hpcd->IN_ep[i].is_in = 1U;
	    hpcd->IN_ep[i].num = i;
	    //hpcd->IN_ep[i].tx_fifo_num = i;
	    /* Control until ep is activated */
	    hpcd->IN_ep[i].type = EP_TYPE_CTRL;
	    hpcd->IN_ep[i].maxpacket = 0U;
	    hpcd->IN_ep[i].xfer_buff = 0U;
	    hpcd->IN_ep[i].xfer_len = 0U;
	  }

	  for (i = 0U; i < hpcd->Init.dev_endpoints; i++)
	  {
	    hpcd->OUT_ep[i].is_in = 0U;
	    hpcd->OUT_ep[i].num = i;
	    /* Control until ep is activated */
	    hpcd->OUT_ep[i].type = EP_TYPE_CTRL;
	    hpcd->OUT_ep[i].maxpacket = 0U;
	    hpcd->OUT_ep[i].xfer_buff = 0U;
	    hpcd->OUT_ep[i].xfer_len = 0U;
	  }

	  /* Init Device */
	  if (USB_DevInit(USBx, hpcd->Init) != HAL_OK)
	  {
	    hpcd->State = HAL_PCD_STATE_ERROR;
	    return HAL_ERROR;
	  }

	  hpcd->USB_Address = 0U;
	  hpcd->State = HAL_PCD_STATE_READY;

	  /* Activate LPM */
//	  if (hpcd->Init.lpm_enable == 1U)
//	  {
//	    (void)HAL_PCDEx_ActivateLPM(hpcd);
//	  }

	  (void)USB_DevDisconnect(USBx);

	  return HAL_OK;
}

/**
  * @brief  DeInitializes the PCD peripheral.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DeInit(PCD_HandleTypeDef *hpcd)
{
	PRINTF("HAL_PCD_DeInit\n");
//	  /* Check the PCD handle allocation */
//	  if (hpcd == NULL)
//	  {
//	    return HAL_ERROR;
//	  }
//
//	  hpcd->State = HAL_PCD_STATE_BUSY;
//
//	  /* Stop Device */
//	  if (USB_StopDevice(hpcd->Instance) != HAL_OK)
//	  {
//	    return HAL_ERROR;
//	  }
//
//	#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
//	  if (hpcd->MspDeInitCallback == NULL)
//	  {
//	    hpcd->MspDeInitCallback = HAL_PCD_MspDeInit; /* Legacy weak MspDeInit  */
//	  }
//
//	  /* DeInit the low level hardware */
//	  hpcd->MspDeInitCallback(hpcd);
//	#else
//	  /* DeInit the low level hardware: CLOCK, NVIC.*/
//	  HAL_PCD_MspDeInit(hpcd);
//	#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
//
//	  hpcd->State = HAL_PCD_STATE_RESET;

	  return HAL_OK;
}

/**
  * @}
  */

/**
  * @brief  Start The USB OTG Device.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd)
{
//  __HAL_LOCK(hpcd);
	USB_DevConnect (hpcd->Instance);
//  __HAL_PCD_ENABLE(hpcd);
//  __HAL_UNLOCK(hpcd);
  return HAL_OK;
}

/**
  * @brief  Stop The USB OTG Device.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd)
{
//  __HAL_LOCK(hpcd);
//  __HAL_PCD_DISABLE(hpcd);
  USB_StopDevice(hpcd->Instance);
  USB_DevDisconnect (hpcd->Instance);
//  __HAL_UNLOCK(hpcd);
  return HAL_OK;
}

/**
  * @brief  Open and configure an endpoint.
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  ep_mps endpoint max packet size
  * @param  ep_type endpoint type
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                  uint16_t ep_mps, uint8_t ep_type)
{
  HAL_StatusTypeDef  ret = HAL_OK;
//  PCD_EPTypeDef *ep;
//
//  if ((ep_addr & 0x80U) == 0x80U)
//  {
//    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
//    ep->is_in = 1U;
//  }
//  else
//  {
//    ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
//    ep->is_in = 0U;
//  }
//
//  ep->num = ep_addr & EP_ADDR_MSK;
//  ep->maxpacket = ep_mps;
//  ep->type = ep_type;
//
////  if (ep->is_in != 0U)
////  {
////    /* Assign a Tx FIFO */
////    ep->tx_fifo_num = ep->num;
////  }
//  /* Set initial data PID. */
//  if (ep_type == EP_TYPE_BULK)
//  {
//    ep->data_pid_start = 0U;
//  }
//
//  __HAL_LOCK(hpcd);
//  (void)USB_ActivateEndpoint(hpcd->Instance, ep);
//  __HAL_UNLOCK(hpcd);

  return ret;
}

/**
  * @brief  Deactivate an endpoint.
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
//  PCD_EPTypeDef *ep;
//
//  if ((ep_addr & 0x80U) == 0x80U)
//  {
//    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
//    ep->is_in = 1U;
//  }
//  else
//  {
//    ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
//    ep->is_in = 0U;
//  }
//  ep->num   = ep_addr & EP_ADDR_MSK;
//
//  __HAL_LOCK(hpcd);
//  (void)USB_DeactivateEndpoint(hpcd->Instance, ep);
//  __HAL_UNLOCK(hpcd);
  return HAL_OK;
}


/**
  * @brief  Receive an amount of data.
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  pBuf pointer to the reception buffer
  * @param  len amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
//  PCD_EPTypeDef *ep;
//
//  ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
//
//  /*setup and start the Xfer */
//  ep->xfer_buff = pBuf;
//  ep->xfer_len = len;
//  ep->xfer_count = 0U;
//  ep->is_in = 0U;
//  ep->num = ep_addr & EP_ADDR_MSK;
//
//  if (hpcd->Init.dma_enable == 1U)
//  {
//    ep->dma_addr = (uint32_t)pBuf;
//  }
//
//  if ((ep_addr & EP_ADDR_MSK) == 0U)
//  {
//#if WITHNEWUSBHAL
//	  USBPhyHw_ep0_read(hpcd, pBuf, len);
//#else /* WITHNEWUSBHAL */
//    (void)USB_EP0StartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
//#endif /* WITHNEWUSBHAL */
//  }
//  else
//  {
//#if WITHNEWUSBHAL
//	  USBPhyHw_endpoint_read(hpcd, ep_addr, pBuf, len);
//#else /* WITHNEWUSBHAL */
//    (void)USB_EPStartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
//#endif /* WITHNEWUSBHAL */
//  }

  return HAL_OK;
}
/**
  * @brief  Send an amount of data
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  pBuf pointer to the transmission buffer
  * @param  len amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, const uint8_t *pBuf, uint32_t len)
{
//  PCD_EPTypeDef *ep;
//
//  ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
//
//  /*setup and start the Xfer */
//  ep->xfer_buff = (uint8_t *) pBuf;
//  ep->xfer_len = len;
//  ep->xfer_count = 0U;
//  ep->is_in = 1U;
//  ep->num = ep_addr & EP_ADDR_MSK;
//
//  if (hpcd->Init.dma_enable == 1U)
//  {
//    ep->dma_addr = (uint32_t)pBuf;
//  }
//
//  if ((ep_addr & EP_ADDR_MSK) == 0U)
//  {
//#if WITHNEWUSBHAL
//	USBPhyHw_ep0_write(hpcd, (uint8_t *) pBuf, len);
//#else /* WITHNEWUSBHAL */
//    (void)USB_EP0StartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
//#endif /* WITHNEWUSBHAL */
//  }
//  else
//  {
//#if WITHNEWUSBHAL
//	  USBPhyHw_endpoint_write(hpcd, ep_addr, (uint8_t *) pBuf, len);
//#else /* WITHNEWUSBHAL */
//    (void)USB_EPStartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
//#endif /* WITHNEWUSBHAL */
//  }

  return HAL_OK;
}

uint32_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
	return hpcd->OUT_ep [ep_addr & EP_ADDR_MSK].xfer_count;
}

#endif /* CPUSTYPE_T113 */
