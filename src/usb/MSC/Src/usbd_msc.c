/**
  ******************************************************************************
  * @file    usbd_msc.c
  * @author  MCD Application Team
  * @brief   This file provides all the MSC core functions.
  *
  * @verbatim
  *
  *          ===================================================================
  *                                MSC Class  Description
  *          ===================================================================
  *           This module manages the MSC class V1.0 following the "Universal
  *           Serial Bus Mass Storage Class (MSC) Bulk-Only Transport (BOT) Version 1.0
  *           Sep. 31, 1999".
  *           This driver implements the following aspects of the specification:
  *             - Bulk-Only Transport protocol
  *             - Subclass : SCSI transparent command set (ref. SCSI Primary Commands - 3 (SPC-3))
  *
  *  @endverbatim
  *
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

#include "hardware.h"

#if WITHUSBHW && WITHUSBDMSC

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
- "stm32xxxxx_{eval}{discovery}{adafruit}_sd.c"
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include "../Inc/usbd_msc.h"

#include "src/usb/usb200.h"
#include "src/usb/usbch9.h"

extern USBD_StorageTypeDef mscStorage;

USBD_MSC_BOT_HandleTypeDef mscBOT;

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup MSC_CORE
  * @brief Mass storage core module
  * @{
  */

/** @defgroup MSC_CORE_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup MSC_CORE_Private_Defines
  * @{
  */

/**
  * @}
  */


/** @defgroup MSC_CORE_Private_Macros
  * @{
  */
/**
  * @}
  */


/** @defgroup MSC_CORE_Private_FunctionPrototypes
  * @{
  */
USBD_StatusTypeDef USBD_MSC_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx);
USBD_StatusTypeDef USBD_MSC_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx);
USBD_StatusTypeDef USBD_MSC_Setup(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req);
USBD_StatusTypeDef USBD_MSC_DataIn(USBD_HandleTypeDef *pdev, uint_fast8_t epnum);
USBD_StatusTypeDef USBD_MSC_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum);

//uint8_t *USBD_MSC_GetHSCfgDesc(uint16_t *length);
//uint8_t *USBD_MSC_GetFSCfgDesc(uint16_t *length);
//uint8_t *USBD_MSC_GetOtherSpeedCfgDesc(uint16_t *length);
//uint8_t *USBD_MSC_GetDeviceQualifierDescriptor(uint16_t *length);

/**
  * @}
  */

#if 1//WITHUSERAMDISK

static int8_t ramdisk_Init(uint8_t lun)
{
	// not called
	return 0;
}

static int8_t ramdisk_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
	* block_size = 512;
	* block_num = 11111;//getRamDiskSize() / 512;

	return 0; // 0 - okatn or non-zero - no media
}
static int8_t ramdisk_IsReady(uint8_t lun)
{
	return 0;	// zero - ready, non-nero - not ready
}
static int8_t ramdisk_IsWriteProtected(uint8_t lun)
{
	return 0; // 0 or non-zero - is write protected
}
static int8_t ramdisk_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
//	uintptr_t const offset = getRamDiskBase() + (blk_addr * 512);
//	memcpy(buf, (void *) offset, 512 * blk_len);
//	return 0;
	return -1;	// error
}
static int8_t ramdisk_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
//	uintptr_t const offset = getRamDiskBase() + (blk_addr * 512);
//	memcpy((void *) offset, buf, 512 * blk_len);
//	return 0;
	return -1;	// error
}
static int8_t ramdisk_GetMaxLun(void)
{
	return 1;
}

// for each LUN
static int8_t ramdisk_Inquiry [1 * STANDARD_INQUIRY_DATA_LEN];


USBD_StorageTypeDef mscStorage = {
	ramdisk_Init,
	ramdisk_GetCapacity,
	ramdisk_IsReady,
	ramdisk_IsWriteProtected,
	ramdisk_Read,
	ramdisk_Write,
	ramdisk_GetMaxLun,
	ramdisk_Inquiry
};
#endif /* WITHUSERAMDISK */


/** @defgroup MSC_CORE_Private_Functions
  * @{
  */

/**
  * @brief  USBD_MSC_Init
  *         Initialize  the mass storage configuration
  * @param  pdev: device instance
  * @param  cfgidx: configuration index
  * @retval status
  */
USBD_StatusTypeDef USBD_MSC_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_MSC_BOT_HandleTypeDef * const hmsc = & mscBOT; //(USBD_MSC_BOT_HandleTypeDef *)pdev->pClassData;

//  USBD_MSC_BOT_HandleTypeDef *hmsc;
//
//  hmsc = USBD_malloc(sizeof(USBD_MSC_BOT_HandleTypeDef));
//
//  if (hmsc == NULL)
//  {
//    pdev->pClassData = NULL;
//    return (uint8_t)USBD_EMEM;
//  }
//
//  pdev->pClassData = (void *)hmsc;

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Open EP OUT */
    (void)USBD_LL_OpenEP(pdev, MSC_EPOUT_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_HS_PACKET);
    pdev->ep_out[MSC_EPOUT_ADDR & 0xFU].is_used = 1U;

    /* Open EP IN */
    (void)USBD_LL_OpenEP(pdev, MSC_EPIN_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_HS_PACKET);
    pdev->ep_in[MSC_EPIN_ADDR & 0xFU].is_used = 1U;
  }
  else
  {
    /* Open EP OUT */
    (void)USBD_LL_OpenEP(pdev, MSC_EPOUT_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_FS_PACKET);
    pdev->ep_out[MSC_EPOUT_ADDR & 0xFU].is_used = 1U;

    /* Open EP IN */
    (void)USBD_LL_OpenEP(pdev, MSC_EPIN_ADDR, USBD_EP_TYPE_BULK, MSC_MAX_FS_PACKET);
    pdev->ep_in[MSC_EPIN_ADDR & 0xFU].is_used = 1U;
  }

  /* Init the BOT  layer */
  MSC_BOT_Init(pdev);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_MSC_DeInit
  *         DeInitialize  the mass storage configuration
  * @param  pdev: device instance
  * @param  cfgidx: configuration index
  * @retval status
  */
USBD_StatusTypeDef USBD_MSC_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
  UNUSED(cfgidx);

  /* Close MSC EPs */
  (void)USBD_LL_CloseEP(pdev, MSC_EPOUT_ADDR);
  pdev->ep_out[MSC_EPOUT_ADDR & 0xFU].is_used = 0U;

  /* Close EP IN */
  (void)USBD_LL_CloseEP(pdev, MSC_EPIN_ADDR);
  pdev->ep_in[MSC_EPIN_ADDR & 0xFU].is_used = 0U;

  /* Free MSC Class Resources */
  //if (pdev->pClassData != NULL)
  {
    /* De-Init the BOT layer */
    MSC_BOT_DeInit(pdev);

//    (void)USBD_free(pdev->pClassData);
//    pdev->pClassData = NULL;
  }

  return (uint8_t)USBD_OK;
}
/**
  * @brief  USBD_MSC_Setup
  *         Handle the MSC specific requests
  * @param  pdev: device instance
  * @param  req: USB request
  * @retval status
  */
USBD_StatusTypeDef USBD_MSC_Setup(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
  USBD_MSC_BOT_HandleTypeDef * const hmsc = & mscBOT; //(USBD_MSC_BOT_HandleTypeDef *)pdev->pClassData;
  USBD_StatusTypeDef ret = USBD_OK;
  uint16_t status_info = 0U;

  if (hmsc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    /* Class request */
    case USB_REQ_TYPE_CLASS:
      switch (req->bRequest)
      {
        case BOT_GET_MAX_LUN:
          if ((req->wValue  == 0U) && (req->wLength == 1U) &&
              ((req->bmRequest & 0x80U) == 0x80U))
          {
            hmsc->max_lun = mscStorage.GetMaxLun();
            (void)USBD_CtlSendData(pdev, (uint8_t *)&hmsc->max_lun, 1U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case BOT_RESET :
          if ((req->wValue  == 0U) && (req->wLength == 0U) &&
              ((req->bmRequest & 0x80U) != 0x80U))
          {
            MSC_BOT_Reset(pdev);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;
    /* Interface & Endpoint request */
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_GET_STATUS:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            (void)USBD_CtlSendData(pdev, (uint8_t *)&status_info, 2U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_GET_INTERFACE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            (void)USBD_CtlSendData(pdev, (uint8_t *)&hmsc->interface, 1U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_SET_INTERFACE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            hmsc->interface = (uint8_t)(req->wValue);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_CLEAR_FEATURE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            if (req->wValue == USB_FEATURE_EP_HALT)
            {
              /* Flush the FIFO */
              (void)USBD_LL_FlushEP(pdev, (uint8_t)req->wIndex);

              /* Handle BOT error */
              MSC_BOT_CplClrFeature(pdev, (uint8_t)req->wIndex);
            }
          }
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }

  return (uint8_t)ret;
}

/**
  * @brief  USBD_MSC_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
USBD_StatusTypeDef USBD_MSC_DataIn(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	  if ((epnum & 0x0F) == (USBD_EP_MSC_IN & 0x0F))
		  MSC_BOT_DataIn(pdev, epnum);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_MSC_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
USBD_StatusTypeDef USBD_MSC_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	if ((epnum & 0x0F) == (USBD_EP_MSC_OUT & 0x0F))
		MSC_BOT_DataOut(pdev, epnum);

	return (uint8_t)USBD_OK;
}
//
///**
//  * @brief  USBD_MSC_GetHSCfgDesc
//  *         return configuration descriptor
//  * @param  length : pointer data length
//  * @retval pointer to descriptor buffer
//  */
//uint8_t *USBD_MSC_GetHSCfgDesc(uint16_t *length)
//{
//  *length = (uint16_t)sizeof(USBD_MSC_CfgHSDesc);
//
//  return USBD_MSC_CfgHSDesc;
//}
//
///**
//  * @brief  USBD_MSC_GetFSCfgDesc
//  *         return configuration descriptor
//  * @param  length : pointer data length
//  * @retval pointer to descriptor buffer
//  */
//uint8_t *USBD_MSC_GetFSCfgDesc(uint16_t *length)
//{
//  *length = (uint16_t)sizeof(USBD_MSC_CfgFSDesc);
//
//  return USBD_MSC_CfgFSDesc;
//}
//
///**
//  * @brief  USBD_MSC_GetOtherSpeedCfgDesc
//  *         return other speed configuration descriptor
//  * @param  length : pointer data length
//  * @retval pointer to descriptor buffer
//  */
//uint8_t *USBD_MSC_GetOtherSpeedCfgDesc(uint16_t *length)
//{
//  *length = (uint16_t)sizeof(USBD_MSC_OtherSpeedCfgDesc);
//
//  return USBD_MSC_OtherSpeedCfgDesc;
//}
///**
//  * @brief  DeviceQualifierDescriptor
//  *         return Device Qualifier descriptor
//  * @param  length : pointer data length
//  * @retval pointer to descriptor buffer
//  */
//uint8_t *USBD_MSC_GetDeviceQualifierDescriptor(uint16_t *length)
//{
//  *length = (uint16_t)sizeof(USBD_MSC_DeviceQualifierDesc);
//
//  return USBD_MSC_DeviceQualifierDesc;
//}

/**
  * @brief  USBD_MSC_RegisterStorage
  * @param  fops: storage callback
  * @retval status
  */
uint8_t USBD_MSC_RegisterStorage(USBD_HandleTypeDef *pdev, USBD_StorageTypeDef *fops)
{
  if (fops == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

 //// pdev->pUserData = fops;

  return (uint8_t)USBD_OK;
}

/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */


static void USBD_MSC_ColdInit(void)
{

}

/** @defgroup MSC_CORE_Private_Variables
  * @{
  */


USBD_ClassTypeDef  USBD_CLASS_MSC =
{
  USBD_MSC_ColdInit,
  USBD_MSC_Init,
  USBD_MSC_DeInit,
  USBD_MSC_Setup,
  NULL, /*EP0_TxSent*/
  NULL, /*EP0_RxReady*/
  USBD_MSC_DataIn,
  USBD_MSC_DataOut,
  NULL, /*SOF */
  NULL,
  NULL,
//  USBD_MSC_GetHSCfgDesc,
//  USBD_MSC_GetFSCfgDesc,
//  USBD_MSC_GetOtherSpeedCfgDesc,
//  USBD_MSC_GetDeviceQualifierDescriptor,
};


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
#endif /* WITHUSBHW && WITHUSBDMSC */
