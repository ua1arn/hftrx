/**
  ******************************************************************************
  * @file    usbd_req.c
  * @author  MCD Application Team
  * @brief   This file provides the standard USB requests following chapter 9.
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

/* Includes ------------------------------------------------------------------*/
#include "usbd_ctlreq.h"
#include "usbd_ioreq.h"

#if CPUSTYLE_R7S721
#include "rza1xx_hal_usb.h"
#endif /* CPUSTYLE_R7S721 */

/** @addtogroup STM32_USBD_STATE_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_REQ
  * @brief USB standard requests module
  * @{
  */

/** @defgroup USBD_REQ_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_REQ_Private_Defines
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_REQ_Private_Macros
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_REQ_Private_Variables
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_REQ_Private_FunctionPrototypes
  * @{
  */
static void USBD_GetDescriptor(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req);
static void USBD_SetAddress(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req);
static USBD_StatusTypeDef USBD_SetConfig(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req);
static void USBD_GetConfig(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req);
static void USBD_GetStatus(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req);
static void USBD_SetFeature(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req);
static void USBD_ClrFeature(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req);
static uint8_t USBD_GetLen(uint8_t *buf);

/**
  * @}
  */


/** @defgroup USBD_REQ_Private_Functions
  * @{
  */


/**
  * @brief  USBD_StdDevReq
  *         Handle standard usb device requests
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
USBD_StatusTypeDef USBD_StdDevReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
	USBD_StatusTypeDef ret = USBD_OK;

	if ((req->bmRequest & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR && req->bRequest == USBD_WCID_VENDOR_CODE)
	{
		// WCID devices support - WINUSB driver request
		//const uint_fast8_t ifc = LO_BYTE(req->wValue);	// INTERFACE_DFU_CONTROL

		if (MsftCompFeatureDescr [0].size != 0)
		{
			USBD_CtlSendData(pdev, MsftCompFeatureDescr [0].data, MIN(MsftCompFeatureDescr [0].size, req->wLength));
		}
		else
		{
			USBD_CtlError(pdev, req);
		}
		return ret;
	}

	switch (req->bmRequest & USB_REQ_TYPE_MASK)
	{
	case USB_REQ_TYPE_CLASS:
	case USB_REQ_TYPE_VENDOR:
		//ret = (USBD_StatusTypeDef)pdev->pClass->Setup(pdev, req);
		for (unsigned di = 0; di < pdev->nClasses; ++ di)
		{
			pdev->pClasses [di]->Setup(pdev, req);
		}
		break;

	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest)
		{
		case USB_REQ_GET_DESCRIPTOR:
			USBD_GetDescriptor(pdev, req);
			break;

		case USB_REQ_SET_ADDRESS:
			USBD_SetAddress(pdev, req);
			break;

		case USB_REQ_SET_CONFIGURATION:
			ret = USBD_SetConfig(pdev, req);
			break;

		case USB_REQ_GET_CONFIGURATION:
			USBD_GetConfig(pdev, req);
			break;

		case USB_REQ_GET_STATUS:
			USBD_GetStatus(pdev, req);
			break;

		case USB_REQ_SET_FEATURE:
			USBD_SetFeature(pdev, req);
			break;

		case USB_REQ_CLEAR_FEATURE:
			USBD_ClrFeature(pdev, req);
			break;

		default:
			USBD_CtlError(pdev, req);
			break;
		}
		break;

	default:
		USBD_CtlError(pdev, req);
		break;
	}

	return ret;
}

/**
  * @brief  USBD_StdItfReq
  *         Handle standard usb interface requests
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
USBD_StatusTypeDef USBD_StdItfReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_StatusTypeDef ret = USBD_OK;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_CLASS:
    case USB_REQ_TYPE_VENDOR:
    case USB_REQ_TYPE_STANDARD:
      switch (pdev->dev_state)
      {
        case USBD_STATE_DEFAULT:
        case USBD_STATE_ADDRESSED:
        case USBD_STATE_CONFIGURED:
          {
  			// Extended Properties OS Descriptor support
  			// wIndex==0x05. Indicates that the request is for an extended properties OS descriptor.
  			if (req->bRequest == USBD_WCID_VENDOR_CODE && req->wIndex == 0x05)
  			{
  				const uint_fast8_t ifc = LO_BYTE(req->wValue);
  				//PRINTF(PSTR("MS USBD_StdItfReq: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
  				// Extended Properties OS Descriptor
  				// See OS_Desc_Ext_Prop.doc, Extended Properties Descriptor Format

  				// Extended Properties OS Descriptor support
  				if (ifc < ARRAY_SIZE(ExtOsPropDescTbl) && ExtOsPropDescTbl[ifc].size != 0)
  				{
  					USBD_CtlSendData(pdev, ExtOsPropDescTbl[ifc].data, MIN(ExtOsPropDescTbl[ifc].size, req->wLength));
  				}
  				else
  				{
  					//TP();
  					USBD_CtlError(pdev, req);
  					return USBD_OK;
  				}
  			}

            //ret = (USBD_StatusTypeDef)pdev->pClass->Setup(pdev, req);
  			for (unsigned di = 0; di < pdev->nClasses; ++ di)
  			{
  				pdev->pClasses [di]->Setup(pdev, req);
  			}

//            if ((req->wLength == 0U) && (ret == USBD_OK))
//            {
//              (void)USBD_CtlSendStatus(pdev);
			// Уже какой-то ответ в обработчике сформирован.
			// Если нет - ошибка, там надо вызвать например USBD_CtlSendStatus
			// Этот запрос был без данных
			//USBD_CtlSendStatus(pdev); // по идее, в обработчике Setup должен быть вызван USBD_CtlSendStatus/USBD_CtlError
//            }
			if (req->wLength == 0)
			{
				//TP();
				//PRINTF(PSTR("USBD_StdItfReq: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
				// Уже какой-то ответ в обработчике сформирован.
				// Если нет - ошибка, там надо вызвать например USBD_CtlSendStatus
				// Этот запрос был без данных
				//USBD_CtlSendStatus(pdev); // по идее, в обработчике Setup должен быть вызван USBD_CtlSendStatus/USBD_CtlError
			}
			else
			{
#if CPUSTYLE_R7S721
				// FIXME: Hack code!!!!
				if ((req->bmRequest & USB_REQ_TYPE_DIR) == 0)
				{
					((PCD_HandleTypeDef *) pdev->pData)->run_later_ctrl_comp = 1;
				}
#endif /* CPUSTYLE_R7S721 */
			}
          }
          break;

        default:
          USBD_CtlError(pdev, req);
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      break;
  }

  return ret;
}

/**
  * @brief  USBD_StdEPReq
  *         Handle standard usb endpoint requests
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
USBD_StatusTypeDef USBD_StdEPReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_EndpointTypeDef *pep;
  uint8_t ep_addr;
  USBD_StatusTypeDef ret = USBD_OK;
  ep_addr = LOBYTE(req->wIndex);

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_CLASS:
    case USB_REQ_TYPE_VENDOR:
      //ret = (USBD_StatusTypeDef)pdev->pClass->Setup(pdev, req);
		for (unsigned di = 0; di < pdev->nClasses; ++ di)
		{
			pdev->pClasses [di]->Setup(pdev, req);
		}
      break;

    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_SET_FEATURE:
          switch (pdev->dev_state)
          {
            case USBD_STATE_ADDRESSED:
              if ((ep_addr != 0x00U) && (ep_addr != 0x80U))
              {
                (void)USBD_LL_StallEP(pdev, ep_addr);
                (void)USBD_LL_StallEP(pdev, 0x80U);
              }
              else
              {
                USBD_CtlError(pdev, req);
              }
              break;

            case USBD_STATE_CONFIGURED:
              if (req->wValue == USB_FEATURE_EP_HALT)
              {
                if ((ep_addr != 0x00U) && (ep_addr != 0x80U) && (req->wLength == 0x00U))
                {
                  (void)USBD_LL_StallEP(pdev, ep_addr);
                }
              }
              (void)USBD_CtlSendStatus(pdev);

              break;

            default:
              USBD_CtlError(pdev, req);
              break;
          }
          break;

        case USB_REQ_CLEAR_FEATURE:

          switch (pdev->dev_state)
          {
            case USBD_STATE_ADDRESSED:
              if ((ep_addr != 0x00U) && (ep_addr != 0x80U))
              {
                (void)USBD_LL_StallEP(pdev, ep_addr);
                (void)USBD_LL_StallEP(pdev, 0x80U);
              }
              else
              {
                USBD_CtlError(pdev, req);
              }
              break;

            case USBD_STATE_CONFIGURED:
              if (req->wValue == USB_FEATURE_EP_HALT)
              {
                if ((ep_addr & 0x7FU) != 0x00U)
                {
                  (void)USBD_LL_ClearStallEP(pdev, ep_addr);
                }
                (void)USBD_CtlSendStatus(pdev);
                //ret = (USBD_StatusTypeDef)pdev->pClass->Setup(pdev, req);
    			for (unsigned di = 0; di < pdev->nClasses; ++ di)
    			{
    				pdev->pClasses [di]->Setup(pdev, req);
    			}
              }
              break;

            default:
              USBD_CtlError(pdev, req);
              break;
          }
          break;

        case USB_REQ_GET_STATUS:
          switch (pdev->dev_state)
          {
            case USBD_STATE_ADDRESSED:
              if ((ep_addr != 0x00U) && (ep_addr != 0x80U))
              {
                USBD_CtlError(pdev, req);
                break;
              }
              pep = ((ep_addr & 0x80U) == 0x80U) ? &pdev->ep_in[ep_addr & 0x7FU] : \
                    &pdev->ep_out[ep_addr & 0x7FU];

              USBD_poke_u16(pep->epstatus, 0x0000U);

              (void)USBD_CtlSendData(pdev, pep->epstatus, MIN(2U, req->wLength));
              break;

            case USBD_STATE_CONFIGURED:
              if ((ep_addr & 0x80U) == 0x80U)
              {
                if (pdev->ep_in[ep_addr & 0xFU].is_used == 0U)
                {
                  USBD_CtlError(pdev, req);
                  break;
                }
              }
              else
              {
                if (pdev->ep_out[ep_addr & 0xFU].is_used == 0U)
                {
                  USBD_CtlError(pdev, req);
                  break;
                }
              }

              pep = ((ep_addr & 0x80U) == 0x80U) ? &pdev->ep_in[ep_addr & 0x7FU] : \
                    &pdev->ep_out[ep_addr & 0x7FU];

              if ((ep_addr == 0x00U) || (ep_addr == 0x80U))
              {
                USBD_poke_u16(pep->epstatus, 0x0000U);
              }
              else if (USBD_LL_IsStallEP(pdev, ep_addr) != 0U)
              {
                  USBD_poke_u16(pep->epstatus, 0x0001U);
              }
              else
              {
                  USBD_poke_u16(pep->epstatus, 0x0000U);
              }

              (void)USBD_CtlSendData(pdev, pep->epstatus, MIN(2U, req->wLength));
              break;

            default:
              USBD_CtlError(pdev, req);
              break;
          }
          break;

        default:
          USBD_CtlError(pdev, req);
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      break;
  }

  return ret;
}


/**
  * @brief  USBD_GetDescriptor
  *         Handle Get Descriptor requests
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static void USBD_GetDescriptor(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	uint16_t len;
	const uint8_t *pbuf;
	const uint_fast8_t index = LO_BYTE(req->wValue);

	//PRINTF(PSTR("USBD_GetDescriptor: %d, wLength=%04X (%d dec), ix=%u\n"), (int) HI_BYTE(req->wValue), req->wLength, req->wLength, LO_BYTE(req->wValue));

	switch (HI_BYTE(req->wValue))
	{
	case USB_DESC_TYPE_DEVICE:
		len = DeviceDescrTbl [0].size;
		pbuf = DeviceDescrTbl [0].data;
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (index < ARRAY_SIZE(ConfigDescrTbl) && ConfigDescrTbl [index].size != 0)
		{
			len = ConfigDescrTbl [index].size;
			pbuf = ConfigDescrTbl [index].data;
		}
		else
		{
			USBD_CtlError(pdev, req);
			return;
		}
		break;

	case USB_DESC_TYPE_STRING:
		{
			const uint_fast16_t LangID = LO_BYTE(req->wIndex);
			switch (index)
			{
			case 0x65:
			case 0xF8:
				// Запрос появляется при запуске MixW2
				//len = StringDescrTbl [STRING_ID_7].size;
				//pbuf = StringDescrTbl [STRING_ID_7].data;
				//break;
				USBD_CtlError(pdev, req);
				return;

			case 0xEE:
				// WCID devices support
				// Microsoft OS String Descriptor, ReqLength=0x12
				// See OS_Desc_Intro.doc, Table 3 describes the OS string descriptor’s fields.
				if (MsftStringDescr [0].data != NULL && MsftStringDescr [0].size != 0)
				{
					len = MsftStringDescr [0].size;
					pbuf = MsftStringDescr [0].data;
				}
				else
				{
					USBD_CtlError(pdev, req);
					return;
				}
				break;

			default:
				if (index < usbd_get_stringsdesc_count() && StringDescrTbl [index].size != 0)
				{
					len = StringDescrTbl [index].size;
					pbuf = StringDescrTbl [index].data;
				}
				else
				{
					//PRINTF(PSTR("USBD_GetDescriptor: %02X\n"), HI_BYTE(req->wValue));
					USBD_CtlError(pdev, req);
					return;
				}
				break;
			} /* case */
		}
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		if (index < ARRAY_SIZE(DeviceQualifierTbl) && DeviceQualifierTbl [index].size != 0)
		{
			len = DeviceQualifierTbl [index].size;
			pbuf = DeviceQualifierTbl [index].data;
		}
		else
		{
			USBD_CtlError(pdev, req);
			return;
		}
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (pdev->dev_speed == USBD_SPEED_HIGH && index < ARRAY_SIZE(OtherSpeedConfigurationTbl) && OtherSpeedConfigurationTbl [index].size != 0)
		{
			len = OtherSpeedConfigurationTbl [index].size;
			pbuf = OtherSpeedConfigurationTbl [index].data;
		}
		else
		{
			USBD_CtlError(pdev, req);
			return;
		}
		break;

	case USB_DESC_TYPE_BOS:
		if (BinaryDeviceObjectStoreTbl [0].size != 0)
		{
			len = BinaryDeviceObjectStoreTbl [0].size;
			pbuf = BinaryDeviceObjectStoreTbl [0].data;
		}
		else
		{
			USBD_CtlError(pdev, req);
			return;
		}
		break;

	case USB_DESC_TYPE_OTG:
		if (OtgDescTbl [0].size != 0)
		{
			len = OtgDescTbl [0].size;
			pbuf = OtgDescTbl [0].data;
		}
		else
		{
			USBD_CtlError(pdev, req);
			return;
		}
		break;

	default:
		USBD_CtlError(pdev, req);
		return;
	}

	if ((len != 0) && (req->wLength != 0))
	{
		//USBD_DbgLog("USBD_GetDescriptor: %02X, wLength=%04X (%d dec), ix=%u, datalen=%u", HI_BYTE(req->wValue), req->wLength, req->wLength, LO_BYTE(req->wValue), len);
		USBD_CtlSendData(pdev, pbuf, MIN(len, req->wLength));
	}

}


/**
  * @brief  USBD_SetAddress
  *         Set device address
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static void USBD_SetAddress(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
  uint8_t  dev_addr;

  if ((req->wIndex == 0U) && (req->wLength == 0U) && (req->wValue < 128U))
  {
    dev_addr = (uint8_t)(req->wValue) & 0x7FU;

    if (pdev->dev_state == USBD_STATE_CONFIGURED)
    {
      USBD_CtlError(pdev, req);
    }
    else
    {
      pdev->dev_address = dev_addr;
      (void)USBD_LL_SetUSBAddress(pdev, dev_addr);
      (void)USBD_CtlSendStatus(pdev);

      if (dev_addr != 0U)
      {
        pdev->dev_state = USBD_STATE_ADDRESSED;
      }
      else
      {
        pdev->dev_state = USBD_STATE_DEFAULT;
      }
    }
  }
  else
  {
    USBD_CtlError(pdev, req);
  }
}

/**
  * @brief  USBD_SetConfig
  *         Handle Set device configuration request
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static USBD_StatusTypeDef USBD_SetConfig(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	USBD_StatusTypeDef ret = USBD_OK;
  const uint_fast8_t cfgidx = LO_BYTE(req->wValue);

  if (cfgidx > USBD_MAX_NUM_CONFIGURATION)
  {
    USBD_CtlError(pdev, req);
    return USBD_FAIL;
  }

  switch (pdev->dev_state)
  {
    case USBD_STATE_ADDRESSED:
      if (cfgidx != 0U)
      {
          pdev->dev_config [0] = cfgidx;

        ret = USBD_SetClassConfig(pdev, cfgidx);

        if (ret != USBD_OK)
        {
          USBD_CtlError(pdev, req);
        }
        else
        {
          (void)USBD_CtlSendStatus(pdev);
          pdev->dev_state = USBD_STATE_CONFIGURED;
        }
      }
      else
      {
        (void)USBD_CtlSendStatus(pdev);
      }
      break;

    case USBD_STATE_CONFIGURED:
      if (cfgidx == 0U)
      {
        pdev->dev_state = USBD_STATE_ADDRESSED;
        pdev->dev_config [0] = cfgidx;
         (void)USBD_ClrClassConfig(pdev, cfgidx);
        (void)USBD_CtlSendStatus(pdev);
      }
      else if (cfgidx != pdev->dev_config [0])
      {
        /* Clear old configuration */
        (void)USBD_ClrClassConfig(pdev, pdev->dev_config [0]);

        /* set new configuration */
        pdev->dev_config [0] = cfgidx;

        ret = USBD_SetClassConfig(pdev, cfgidx);

        if (ret != USBD_OK)
        {
          USBD_CtlError(pdev, req);
          (void)USBD_ClrClassConfig(pdev, pdev->dev_config [0]);
          pdev->dev_state = USBD_STATE_ADDRESSED;
        }
        else
        {
          (void)USBD_CtlSendStatus(pdev);
        }
      }
      else
      {
        (void)USBD_CtlSendStatus(pdev);
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      (void)USBD_ClrClassConfig(pdev, cfgidx);
      ret = USBD_FAIL;
      break;
  }

  return ret;
}

/**
  * @brief  USBD_GetConfig
  *         Handle Get device configuration request
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static void USBD_GetConfig(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
  if (req->wLength != 1U)
  {
    USBD_CtlError(pdev, req);
  }
  else
  {
    switch (pdev->dev_state)
    {
      case USBD_STATE_DEFAULT:
      case USBD_STATE_ADDRESSED:
        pdev->dev_default_config [0] = 0U;
        (void)USBD_CtlSendData(pdev, pdev->dev_default_config, MIN(1U, req->wLength));
        break;

      case USBD_STATE_CONFIGURED:
        (void)USBD_CtlSendData(pdev, pdev->dev_config, MIN(1U, req->wLength));
        break;

      default:
        USBD_CtlError(pdev, req);
        break;
    }
  }
}

/**
  * @brief  USBD_GetStatus
  *         Handle Get Status request
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static void USBD_GetStatus(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	switch (pdev->dev_state)
	{
	case USBD_STATE_DEFAULT:
	case USBD_STATE_ADDRESSED:
	case USBD_STATE_CONFIGURED:
		if (req->wLength != 0x2U)
		{
			//PRINTF("USBD_GetStatus: req->wLength=%u\n", req->wLength);
			USBD_CtlError(pdev, req);
			break;
		}
		//PRINTF("USBD_GetStatus: req->wLength=%u\n", req->wLength);
		pdev->dev_config_status [0] = 0;
		pdev->dev_config_status [1] = 0;

#if (USBD_SELF_POWERED == 1U)
		pdev->dev_config_status [0] |= USB_CONFIG_SELF_POWERED;
#endif

		if (pdev->dev_remote_wakeup)
		{
			pdev->dev_config_status [0] |= USB_CONFIG_REMOTE_WAKEUP;
		}

		(void) USBD_CtlSendData(pdev, pdev->dev_config_status, MIN(2U, req->wLength));
		break;

	default:
		USBD_CtlError(pdev, req);
		break;
	}
}


/**
  * @brief  USBD_SetFeature
  *         Handle Set device feature request
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static void USBD_SetFeature(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
  if (req->wValue == USB_FEATURE_REMOTE_WAKEUP)
  {
    pdev->dev_remote_wakeup = 1U;
    (void)USBD_CtlSendStatus(pdev);
  }
}


/**
  * @brief  USBD_ClrFeature
  *         Handle clear device feature request
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static void USBD_ClrFeature(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
  switch (pdev->dev_state)
  {
    case USBD_STATE_DEFAULT:
    case USBD_STATE_ADDRESSED:
    case USBD_STATE_CONFIGURED:
      if (req->wValue == USB_FEATURE_REMOTE_WAKEUP)
      {
        pdev->dev_remote_wakeup = 0U;
        (void)USBD_CtlSendStatus(pdev);
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      break;
  }
}


/**
  * @brief  USBD_ParseSetupRequest
  *         Copy buffer into setup structure
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval None
  */
void USBD_ParseSetupRequest(USBD_SetupReqTypedef *req, uint8_t *pdata)
{
#if ! CPUSTYLE_R7S721
  uint8_t *pbuff = pdata;

  req->bmRequest = *(uint8_t *)(pbuff);

  pbuff++;
  req->bRequest = *(uint8_t *)(pbuff);

  pbuff++;
  req->wValue = SWAPBYTE(pbuff);

  pbuff++;
  pbuff++;
  req->wIndex = SWAPBYTE(pbuff);

  pbuff++;
  pbuff++;
  req->wLength = SWAPBYTE(pbuff);
#endif
}


/**
  * @brief  USBD_CtlError
  *         Handle USB low level Error
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval None
  */
void (USBD_CtlError)(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req, const char * file, int line)
{
  UNUSED(req);

  USBD_ErrLog("USBD_CtlError: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X", req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
  USBD_ErrLog("USBD_CtlError: %s(%d)", file, line);

  (void)USBD_LL_StallEP(pdev, 0x80U);
  (void)USBD_LL_StallEP(pdev, 0U);
}


/**
  * @brief  USBD_GetString
  *         Convert Ascii string into unicode one
  * @param  desc : descriptor buffer
  * @param  unicode : Formatted string buffer (unicode)
  * @param  len : descriptor length
  * @retval None
  */
#if 0
void USBD_GetString(uint8_t *desc, uint8_t *unicode, uint16_t *len)
{
  uint8_t idx = 0U;
  uint8_t *pdesc;

  if (desc == NULL)
  {
    return;
  }

  pdesc = desc;
  *len = ((uint16_t)USBD_GetLen(pdesc) * 2U) + 2U;

  unicode[idx] = *(uint8_t *)len;
  idx++;
  unicode[idx] = USB_DESC_TYPE_STRING;
  idx++;

  while (*pdesc != (uint8_t)'\0')
  {
    unicode[idx] = *pdesc;
    pdesc++;
    idx++;

    unicode[idx] = 0U;
    idx++;
  }
}


/**
  * @brief  USBD_GetLen
  *         return the string length
   * @param  buf : pointer to the ascii string buffer
  * @retval string length
  */
static uint8_t USBD_GetLen(uint8_t *buf)
{
  uint8_t  len = 0U;
  uint8_t *pbuff = buf;

  while (*pbuff != (uint8_t)'\0')
  {
    len++;
    pbuff++;
  }

  return len;
}
#endif
/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
