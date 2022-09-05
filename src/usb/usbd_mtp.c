/*
 * usbd_mtp.c
 *
 *  Created on: Sep 5, 2022
 *      Author: gena
 */
#include "hardware.h"

#if WITHUSBHW && WITHUSBDMTP

#include "formats.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usb200.h"
#include "usbch9.h"

static const uint8_t MTPInEpAdd = USBD_EP_MTP_IN;
static const uint8_t MTPOutEpAdd = USBD_EP_MTP_OUT;
static const uint8_t MTPCmdEpAdd = USBD_EP_MTP_INT;


/**
  * @}
  */

/** @defgroup USBD_MTP_Private_Functions
  * @{
  */

/**
  * @brief  USBD_MTP_Init
  *         Initialize the MTP interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_MTP_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_MTP_HandleTypeDef *hmtp;

  hmtp = (USBD_MTP_HandleTypeDef *)USBD_malloc(sizeof(USBD_MTP_HandleTypeDef));

  if (hmtp == NULL)
  {
    pdev->pClassDataCmsit[pdev->classId] = NULL;
    return (uint8_t)USBD_EMEM;
  }

  /* Setup the pClassData pointer */
  pdev->pClassDataCmsit[pdev->classId] = (void *)hmtp;
  pdev->pClassData = pdev->pClassDataCmsit[pdev->classId];

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  MTPInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  MTPOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  MTPCmdEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */


  /* Initialize all variables */
  (void)USBD_memset(hmtp, 0, sizeof(USBD_MTP_HandleTypeDef));

  /* Setup the max packet size according to selected speed */
  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    hmtp->MaxPcktLen = MTP_DATA_MAX_HS_PACKET_SIZE;
  }
  else
  {
    hmtp->MaxPcktLen = MTP_DATA_MAX_FS_PACKET_SIZE;
  }

  /* Open EP IN */
  (void)USBD_LL_OpenEP(pdev, MTPInEpAdd, USBD_EP_TYPE_BULK, hmtp->MaxPcktLen);
  pdev->ep_in[MTPInEpAdd & 0xFU].is_used = 1U;

  /* Open EP OUT */
  (void)USBD_LL_OpenEP(pdev, MTPOutEpAdd, USBD_EP_TYPE_BULK, hmtp->MaxPcktLen);
  pdev->ep_out[MTPOutEpAdd & 0xFU].is_used = 1U;

  /* Open INTR EP IN */
  (void)USBD_LL_OpenEP(pdev, MTPCmdEpAdd, USBD_EP_TYPE_INTR, MTP_CMD_PACKET_SIZE);
  pdev->ep_in[MTPCmdEpAdd & 0xFU].is_used = 1U;

  /* Init the MTP  layer */
  (void)USBD_MTP_STORAGE_Init(pdev);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_MTP_DeInit
  *         DeInitialize the MTP layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_MTP_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
  UNUSED(cfgidx);

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this MTP class instance */
  MTPInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  MTPOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  MTPCmdEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  /* Close EP IN */
  (void)USBD_LL_CloseEP(pdev, MTPInEpAdd);
  pdev->ep_in[MTPInEpAdd & 0xFU].is_used = 0U;

  /* Close EP OUT */
  (void)USBD_LL_CloseEP(pdev, MTPOutEpAdd);
  pdev->ep_out[MTPOutEpAdd & 0xFU].is_used = 0U;

  /* Close EP Command */
  (void)USBD_LL_CloseEP(pdev, MTPCmdEpAdd);
  pdev->ep_in[MTPCmdEpAdd & 0xFU].is_used = 0U;

  /* Free MTP Class Resources */
  if (pdev->pClassDataCmsit[pdev->classId] != NULL)
  {
    /* De-Init the MTP layer */
    (void)USBD_MTP_STORAGE_DeInit(pdev);

    (void)USBD_free(pdev->pClassDataCmsit[pdev->classId]);
    pdev->pClassDataCmsit[pdev->classId] = NULL;
    pdev->pClassData = NULL;
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_MTP_Setup
  *         Handle the MTP specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t USBD_MTP_Setup(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
  USBD_MTP_HandleTypeDef *hmtp = (USBD_MTP_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];
  USBD_StatusTypeDef ret = USBD_OK;
  uint16_t len = 0U;

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this MTP class instance */
  MTPOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  if (hmtp == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    /* Class request */
    case USB_REQ_TYPE_CLASS :
      switch (req->bRequest)
      {
        case  MTP_REQ_CANCEL:
          len = MIN(hmtp->MaxPcktLen, req->wLength);
          (void)USBD_CtlPrepareRx(pdev, (uint8_t *)(hmtp->rx_buff), len);
          break;

        case MTP_REQ_GET_EXT_EVENT_DATA:
          break;

        case MTP_REQ_RESET:
          /* Stop low layer file system operations if any */
          USBD_MTP_STORAGE_Cancel(pdev, MTP_PHASE_IDLE);

          (void)USBD_LL_PrepareReceive(pdev, MTPOutEpAdd, (uint8_t *)&hmtp->rx_buff, hmtp->MaxPcktLen);
          break;

        case MTP_REQ_GET_DEVICE_STATUS:
          switch (hmtp->MTP_ResponsePhase)
          {
            case MTP_READ_DATA :
              len = 4U;
              hmtp->dev_status = ((uint32_t)MTP_RESPONSE_DEVICE_BUSY << 16) | len;
              break;

            case MTP_RECEIVE_DATA :
              len = 4U;
              hmtp->dev_status = ((uint32_t)MTP_RESPONSE_TRANSACTION_CANCELLED << 16) | len;
              break;

            case MTP_PHASE_IDLE :
              len = 4U;
              hmtp->dev_status = ((uint32_t)MTP_RESPONSE_OK << 16) | len;
              break;

            default:
              break;
          }
          (void)USBD_CtlSendData(pdev, (uint8_t *)&hmtp->dev_status, len);
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
        case USB_REQ_GET_INTERFACE :

          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            hmtp->alt_setting = 0U;
            (void)USBD_CtlSendData(pdev, (uint8_t *)&hmtp->alt_setting, 1U);
          }
          break;

        case USB_REQ_SET_INTERFACE :
          if (pdev->dev_state != USBD_STATE_CONFIGURED)
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_CLEAR_FEATURE:

          /* Re-activate the EP */
          (void)USBD_LL_CloseEP(pdev, (uint8_t)req->wIndex);

          if ((((uint8_t)req->wIndex) & 0x80U) == 0x80U)
          {
            (void)USBD_LL_OpenEP(pdev, ((uint8_t)req->wIndex), USBD_EP_TYPE_BULK, hmtp->MaxPcktLen);
          }
          else
          {
            (void)USBD_LL_OpenEP(pdev, ((uint8_t)req->wIndex), USBD_EP_TYPE_BULK, hmtp->MaxPcktLen);
          }
          break;

        default:
          break;
      }
      break;

    default:
      break;
  }
  return (uint8_t)ret;
}

/**
  * @brief  USBD_MTP_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t USBD_MTP_DataIn(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
  UNUSED(epnum);
  USBD_MTP_HandleTypeDef *hmtp = (USBD_MTP_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];
  uint16_t len;

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this MTP class instance */
  MTPInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  MTPOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  if (epnum == (MTPInEpAdd & 0x7FU))
  {
    switch (hmtp->MTP_ResponsePhase)
    {
      case MTP_RESPONSE_PHASE :
        (void)USBD_MTP_STORAGE_SendContainer(pdev, REP_TYPE);

        /* prepare to receive next operation */
        len = MIN(hmtp->MaxPcktLen, pdev->request.wLength);

        (void)USBD_LL_PrepareReceive(pdev, MTPOutEpAdd, (uint8_t *)&hmtp->rx_buff, len);
        hmtp->MTP_ResponsePhase = MTP_PHASE_IDLE;
        break;

      case MTP_READ_DATA :
        (void)USBD_MTP_STORAGE_ReadData(pdev);

        /* prepare to receive next operation */
        len = MIN(hmtp->MaxPcktLen, pdev->request.wLength);

        (void)USBD_LL_PrepareReceive(pdev, MTPInEpAdd, (uint8_t *)&hmtp->rx_buff, len);
        break;

      case MTP_PHASE_IDLE :
        /* prepare to receive next operation */
        len = MIN(hmtp->MaxPcktLen, pdev->request.wLength);

        (void)USBD_LL_PrepareReceive(pdev, MTPOutEpAdd, (uint8_t *)&hmtp->rx_buff, len);

        break;
      default:
        break;
    }
  }
  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_MTP_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t USBD_MTP_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
  UNUSED(epnum);
  USBD_MTP_HandleTypeDef *hmtp = (USBD_MTP_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];
  uint16_t len;

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this MTP class instance */
  MTPOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  (void)USBD_MTP_STORAGE_ReceiveOpt(pdev);

  switch (hmtp->MTP_ResponsePhase)
  {
    case MTP_RESPONSE_PHASE :

      if (hmtp->ResponseLength == MTP_CONT_HEADER_SIZE)
      {
        (void)USBD_MTP_STORAGE_SendContainer(pdev, REP_TYPE);
        hmtp->MTP_ResponsePhase = MTP_PHASE_IDLE;
      }
      else
      {
        (void)USBD_MTP_STORAGE_SendContainer(pdev, DATA_TYPE);
      }
      break;

    case MTP_READ_DATA :
      (void)USBD_MTP_STORAGE_ReadData(pdev);
      break;

    case MTP_RECEIVE_DATA :
      (void)USBD_MTP_STORAGE_ReceiveData(pdev);

      /* prepare endpoint to receive operations */
      len = MIN(hmtp->MaxPcktLen, pdev->request.wLength);

      (void)USBD_LL_PrepareReceive(pdev, MTPOutEpAdd, (uint8_t *)&hmtp->rx_buff, len);
      break;

    case MTP_PHASE_IDLE :
      /* prepare to receive next operation */
      len = MIN(hmtp->MaxPcktLen, pdev->request.wLength);

      (void)USBD_LL_PrepareReceive(pdev, MTPOutEpAdd, (uint8_t *)&hmtp->rx_buff, len);
      break;

    default:
      break;
  }

  return (uint8_t)USBD_OK;
}

static void USBD_MTP_ColdInit(void)
{
}

const USBD_ClassTypeDef USBD_CLASS_MTP =
{
	USBD_MTP_ColdInit,
	USBD_MTP_Init,
	USBD_MTP_DeInit,
	USBD_MTP_Setup,
	NULL, /*EP0_TxSent*/
	NULL, /*EP0_RxReady*/
	USBD_MTP_DataIn,
	USBD_MTP_DataOut,
	NULL, /*SOF */
	NULL, /*ISOIn*/
	NULL, /*ISOOut*/
};

#endif /* WITHUSBHW && WITHUSBDMTP */
