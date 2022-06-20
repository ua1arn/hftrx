/* $Id$ */
// allwnr_hal_usb.c
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//


#include "hardware.h"

#if CPUSTYPE_ALLWNT113

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
  * @brief  Initializes the PCD according to the specified
  *         parameters in the PCD_InitTypeDef and create the associated handle.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd)
{
//	  USB_OTG_GlobalTypeDef *USBx;
//	  uint8_t i;
//
//	  /* Check the PCD handle allocation */
//	  if (hpcd == NULL)
//	  {
//	    return HAL_ERROR;
//	  }
//
//	  /* Check the parameters */
//	  //assert_param(IS_PCD_ALL_INSTANCE(hpcd->Instance));
//
//	  USBx = hpcd->Instance;
//
//	  if (hpcd->State == HAL_PCD_STATE_RESET)
//	  {
//	    /* Allocate lock resource and initialize it */
//	    hpcd->Lock = HAL_UNLOCKED;
//
//	#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
//	    hpcd->SOFCallback = HAL_PCD_SOFCallback;
//	    hpcd->SetupStageCallback = HAL_PCD_SetupStageCallback;
//	    hpcd->ResetCallback = HAL_PCD_ResetCallback;
//	    hpcd->SuspendCallback = HAL_PCD_SuspendCallback;
//	    hpcd->ResumeCallback = HAL_PCD_ResumeCallback;
//	    hpcd->ConnectCallback = HAL_PCD_ConnectCallback;
//	    hpcd->DisconnectCallback = HAL_PCD_DisconnectCallback;
//	    hpcd->DataOutStageCallback = HAL_PCD_DataOutStageCallback;
//	    hpcd->DataInStageCallback = HAL_PCD_DataInStageCallback;
//	    hpcd->ISOOUTIncompleteCallback = HAL_PCD_ISOOUTIncompleteCallback;
//	    hpcd->ISOINIncompleteCallback = HAL_PCD_ISOINIncompleteCallback;
//	    hpcd->LPMCallback = HAL_PCDEx_LPM_Callback;
//	    hpcd->BCDCallback = HAL_PCDEx_BCD_Callback;
//
//	    if (hpcd->MspInitCallback == NULL)
//	    {
//	      hpcd->MspInitCallback = HAL_PCD_MspInit;
//	    }
//
//	    /* Init the low level hardware */
//	    hpcd->MspInitCallback(hpcd);
//	#else
//	    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
//	    HAL_PCD_MspInit(hpcd);
//	#endif /* (USE_HAL_PCD_REGISTER_CALLBACKS) */
//	  }
//
//	  hpcd->State = HAL_PCD_STATE_BUSY;
//
//	  /* Disable DMA mode for FS instance */
////	  if ((USBx->CID & (0x1U << 8)) == 0U)
////	  {
////	    hpcd->Init.dma_enable = 0U;
////	  }
//
//	  /* Disable the Interrupts */
//	  __HAL_PCD_DISABLE(hpcd);
//
//	  /*Init the Core (common init.) */
//	  if (USB_CoreInit(USBx, hpcd->Init) != HAL_OK)
//	  {
//	    hpcd->State = HAL_PCD_STATE_ERROR;
//	    return HAL_ERROR;
//	  }
//
//	  /* Force Device Mode*/
//	  (void)USB_SetCurrentMode(USBx, USB_DEVICE_MODE);
//
//	  /* Init endpoints structures */
//	  for (i = 0U; i < hpcd->Init.dev_endpoints; i++)
//	  {
//	    /* Init ep structure */
//	    hpcd->IN_ep[i].is_in = 1U;
//	    hpcd->IN_ep[i].num = i;
//	    //hpcd->IN_ep[i].tx_fifo_num = i;
//	    /* Control until ep is activated */
//	    hpcd->IN_ep[i].type = EP_TYPE_CTRL;
//	    hpcd->IN_ep[i].maxpacket = 0U;
//	    hpcd->IN_ep[i].xfer_buff = 0U;
//	    hpcd->IN_ep[i].xfer_len = 0U;
//	  }
//
//	  for (i = 0U; i < hpcd->Init.dev_endpoints; i++)
//	  {
//	    hpcd->OUT_ep[i].is_in = 0U;
//	    hpcd->OUT_ep[i].num = i;
//	    /* Control until ep is activated */
//	    hpcd->OUT_ep[i].type = EP_TYPE_CTRL;
//	    hpcd->OUT_ep[i].maxpacket = 0U;
//	    hpcd->OUT_ep[i].xfer_buff = 0U;
//	    hpcd->OUT_ep[i].xfer_len = 0U;
//	  }
//
//	  /* Init Device */
//	  if (USB_DevInit(USBx, hpcd->Init) != HAL_OK)
//	  {
//	    hpcd->State = HAL_PCD_STATE_ERROR;
//	    return HAL_ERROR;
//	  }
//
//	  hpcd->USB_Address = 0U;
//	  hpcd->State = HAL_PCD_STATE_READY;
//
//	  /* Activate LPM */
////	  if (hpcd->Init.lpm_enable == 1U)
////	  {
////	    (void)HAL_PCDEx_ActivateLPM(hpcd);
////	  }
//
//	  (void)USB_DevDisconnect(USBx);

	  return HAL_OK;
}

/**
  * @brief  DeInitializes the PCD peripheral.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DeInit(PCD_HandleTypeDef *hpcd)
{
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
//  USB_DevConnect (hpcd->Instance);
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
//  USB_StopDevice(hpcd->Instance);
//  USB_DevDisconnect (hpcd->Instance);
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

#endif /* CPUSTYPE_ALLWNT113 */
