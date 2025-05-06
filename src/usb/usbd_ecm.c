/*
 * usbd_cdc.c
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
 *
 * Много взято
 * https://github.com/rawaaw/cdc-eem-f103.git
 *
 * see also https://www.belcarra.com/2012/01/belcarra-eem-configuration-extension.html
*/

#include "hardware.h"

#if WITHUSBHW && WITHUSBCDCECM

#include "buffers.h"
#include "formats.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usb200.h"
#include "usbch9.h"

#define CDCECM_MTU 1500
extern  USBD_HandleTypeDef hUsbDeviceHS;


typedef void (*cdcecm_rxproc_t)(const uint8_t *data, int size);

static cdcecm_rxproc_t nic_rxproc = NULL;

typedef enum
{
  NETWORK_CONNECTION = 0x00,
  RESPONSE_AVAILABLE = 0x01,
  CONNECTION_SPEED_CHANGE = 0x2A
} USBD_CDC_NotifCodeTypeDef;

/** @defgroup USBD_CDC_ECM_Private_FunctionPrototypes
  * @{
  */

//static USBD_StatusTypeDef USBD_CDC_ECM_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx);
//static USBD_StatusTypeDef USBD_CDC_ECM_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx);
//static USBD_StatusTypeDef USBD_CDC_ECM_DataIn(USBD_HandleTypeDef *pdev, uint_fast8_t epnum);
//static USBD_StatusTypeDef USBD_CDC_ECM_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum);
//static USBD_StatusTypeDef USBD_CDC_ECM_EP0_RxReady(USBD_HandleTypeDef *pdev);
//static USBD_StatusTypeDef USBD_CDC_ECM_Setup(USBD_HandleTypeDef *pdev,
//		const USBD_SetupReqTypedef *req);
//#ifndef USE_USBD_COMPOSITE
//uint8_t *USBD_CDC_ECM_GetDeviceQualifierDescriptor(uint16_t *length);
//#endif /* USE_USBD_COMPOSITE */

#define CDC_ECM_ETH_MAX_SEGSZE CDCECM_MTU

#define CDC_ECM_CONNECT_SPEED_UPSTREAM                          12000000 //0x004C4B40U /* 5Mbps */
#define CDC_ECM_CONNECT_SPEED_DOWNSTREAM                        12000000 //0x004C4B40U /* 5Mbps */

#define CDC_ECM_DATA_BUFFER_SIZE                        2000U
#define CDC_ECM_HS_BINTERVAL 4
#define CDC_ECM_FS_BINTERVAL 1
/*---------------------------------------------------------------------*/
/*  CDC_ECM definitions                                                    */
/*---------------------------------------------------------------------*/
#define CDC_ECM_SEND_ENCAPSULATED_COMMAND                       0x00U
#define CDC_ECM_GET_ENCAPSULATED_RESPONSE                       0x01U
#define CDC_ECM_SET_ETH_MULTICAST_FILTERS                       0x40U
#define CDC_ECM_SET_ETH_PWRM_PATTERN_FILTER                     0x41U
#define CDC_ECM_GET_ETH_PWRM_PATTERN_FILTER                     0x42U
#define CDC_ECM_SET_ETH_PACKET_FILTER                           0x43U
#define CDC_ECM_GET_ETH_STATISTIC                               0x44U

#define CDC_ECM_NET_DISCONNECTED                                0x00U
#define CDC_ECM_NET_CONNECTED                                   0x01U


/* Ethernet statistics definitions */
#define CDC_ECM_XMIT_OK_VAL                                     CDC_ECM_ETH_STATS_VAL_ENABLED
#define CDC_ECM_XMIT_OK                                         0x01U
#define CDC_ECM_RVC_OK                                          0x02U
#define CDC_ECM_XMIT_ERROR                                      0x04U
#define CDC_ECM_RCV_ERROR                                       0x08U
#define CDC_ECM_RCV_NO_BUFFER                                   0x10U
#define CDC_ECM_DIRECTED_BYTES_XMIT                             0x20U
#define CDC_ECM_DIRECTED_FRAMES_XMIT                            0x40U
#define CDC_ECM_MULTICAST_BYTES_XMIT                            0x80U

#define CDC_ECM_MULTICAST_FRAMES_XMIT                           0x01U
#define CDC_ECM_BROADCAST_BYTES_XMIT                            0x02U
#define CDC_ECM_BROADCAST_FRAMES_XMIT                           0x04U
#define CDC_ECM_DIRECTED_BYTES_RCV                              0x08U
#define CDC_ECM_DIRECTED_FRAMES_RCV                             0x10U
#define CDC_ECM_MULTICAST_BYTES_RCV                             0x20U
#define CDC_ECM_MULTICAST_FRAMES_RCV                            0x40U
#define CDC_ECM_BROADCAST_BYTES_RCV                             0x80U

#define CDC_ECM_BROADCAST_FRAMES_RCV                            0x01U
#define CDC_ECM_RCV_CRC_ERROR                                   0x02U
#define CDC_ECM_TRANSMIT_QUEUE_LENGTH                           0x04U
#define CDC_ECM_RCV_ERROR_ALIGNMENT                             0x08U
#define CDC_ECM_XMIT_ONE_COLLISION                              0x10U
#define CDC_ECM_XMIT_MORE_COLLISIONS                            0x20U
#define CDC_ECM_XMIT_DEFERRED                                   0x40U
#define CDC_ECM_XMIT_MAX_COLLISIONS                             0x80U

#define CDC_ECM_RCV_OVERRUN                                     0x40U
#define CDC_ECM_XMIT_UNDERRUN                                   0x40U
#define CDC_ECM_XMIT_HEARTBEAT_FAILURE                          0x40U
#define CDC_ECM_XMIT_TIMES_CRS_LOST                             0x40U
#define CDC_ECM_XMIT_LATE_COLLISIONS                            0x40U

#define CDC_ECM_ETH_STATS_RESERVED                              0xE0U
#define CDC_ECM_BMREQUEST_TYPE_ECM                              0xA1U

/*
 * ECM Class specification revision 1.2
 * Table 3: Ethernet Networking Functional Descriptor
 */

typedef struct
{
  uint8_t bFunctionLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubType;
  uint8_t iMacAddress;
  uint8_t bEthernetStatistics3;
  uint8_t bEthernetStatistics2;
  uint8_t bEthernetStatistics1;
  uint8_t bEthernetStatistics0;
  uint16_t wMaxSegmentSize;
  uint16_t bNumberMCFiltes;
  uint8_t bNumberPowerFiltes;
} __PACKED USBD_ECMFuncDescTypeDef;

typedef struct
{
  uint8_t   bmRequest;
  uint8_t   bRequest;
  uint16_t  wValue;
  uint16_t  wIndex;
  uint16_t  wLength;
  uint8_t   data[8];
} USBD_CDC_ECM_NotifTypeDef;

typedef struct
{
  uint32_t data[CDC_ECM_DATA_BUFFER_SIZE / 4U];      /* Force 32-bit alignment */
  uint8_t  CmdOpCode;
  uint8_t  CmdLength;
  uint8_t  Reserved1;  /* Reserved Byte to force 4 bytes alignment of following fields */
  uint8_t  Reserved2;  /* Reserved Byte to force 4 bytes alignment of following fields */
  uint8_t  *RxBuffer;
  uint8_t  *TxBuffer;
  uint32_t RxLength;
  uint32_t TxLength;

  __IO uint32_t TxState;
  __IO uint32_t RxState;

  __IO uint32_t  MaxPcktLen;
  __IO uint32_t  LinkStatus;
  __IO uint32_t  NotificationStatus;
  USBD_CDC_ECM_NotifTypeDef   Req;
} USBD_CDC_ECM_HandleTypeDef;

typedef struct
{
  int8_t (* Init)(void);
  int8_t (* DeInit)(void);
  int8_t (* Control)(uint8_t cmd, uint8_t *pbuf, uint16_t length);
  int8_t (* Receive)(uint8_t *Buf, uint32_t *Len);
  int8_t (* TransmitCplt)(uint8_t *Buf, uint32_t *Len, uint8_t epnum);
  int8_t (* Process)(USBD_HandleTypeDef *pdev);
  const uint8_t *pStrDesc;
} USBD_CDC_ECM_ItfTypeDef;


/* Private function prototypes -----------------------------------------------*/
static int8_t CDC_ECM_Itf_Init(void);
static int8_t CDC_ECM_Itf_DeInit(void);
static int8_t CDC_ECM_Itf_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_ECM_Itf_Receive(uint8_t *pbuf, uint32_t *Len);
static int8_t CDC_ECM_Itf_TransmitCplt(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);
static int8_t CDC_ECM_Itf_Process(USBD_HandleTypeDef *pdev);

static USBD_CDC_ECM_HandleTypeDef ghcdc;

USBD_CDC_ECM_ItfTypeDef USBD_CDC_ECM_fops =
{
  CDC_ECM_Itf_Init,
  CDC_ECM_Itf_DeInit,
  CDC_ECM_Itf_Control,
  CDC_ECM_Itf_Receive,
  CDC_ECM_Itf_TransmitCplt,
  CDC_ECM_Itf_Process,
  //(uint8_t *)RNDIS_HWADDR_STR,
};


/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
uint8_t  USBD_CDC_ECM_RegisterInterface(USBD_HandleTypeDef *pdev,
                                        USBD_CDC_ECM_ItfTypeDef *fops);

uint8_t  USBD_CDC_ECM_SetRxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff);

uint8_t  USBD_CDC_ECM_ReceivePacket(USBD_HandleTypeDef *pdev);

#ifdef USE_USBD_COMPOSITE
uint8_t  USBD_CDC_ECM_TransmitPacket(USBD_HandleTypeDef *pdev, uint8_t ClassId);
uint8_t  USBD_CDC_ECM_SetTxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff,
                                  uint32_t length, uint8_t ClassId);
#else
uint8_t  USBD_CDC_ECM_TransmitPacket(USBD_HandleTypeDef *pdev);
uint8_t  USBD_CDC_ECM_SetTxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff,
                                  uint32_t length);
#endif /* USE_USBD_COMPOSITE */
uint8_t  USBD_CDC_ECM_SendNotification(USBD_HandleTypeDef *pdev,
                                       USBD_CDC_NotifCodeTypeDef  Notif,
                                       uint16_t bVal, uint8_t *pData);
/**
  * @}
  */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif /* ( __ICCARM__ ) */
__ALIGN_BEGIN static uint8_t UserRxBuffer[CDC_ECM_ETH_MAX_SEGSZE + 100]__ALIGN_END;

/* Transmitted Data over CDC_ECM (CDC_ECM interface) are stored in this buffer */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif /* ( __ICCARM__ ) */
__ALIGN_BEGIN  static uint8_t UserTxBuffer[CDC_ECM_ETH_MAX_SEGSZE + 100]__ALIGN_END;

static uint8_t CDC_ECMInitialized = 0U;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  CDC_ECM_Itf_Init
  *         Initializes the CDC_ECM media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_Init(void)
{
  if (CDC_ECMInitialized == 0U)
  {
    /*
      Initialize the TCP/IP stack here
    */

    CDC_ECMInitialized = 1U;
  }

  /* Set Application Buffers */
#ifdef USE_USBD_COMPOSITE
  (void)USBD_CDC_ECM_SetTxBuffer(&hUsbDeviceHS, UserTxBuffer, 0U, 0U);
#else
  (void)USBD_CDC_ECM_SetTxBuffer(&hUsbDeviceHS, UserTxBuffer, 0U);
#endif /* USE_USBD_COMPOSITE */
  (void)USBD_CDC_ECM_SetRxBuffer(&hUsbDeviceHS, UserRxBuffer);

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_DeInit
  *         DeInitializes the CDC_ECM media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_DeInit(void)
{
#ifdef USE_USBD_COMPOSITE
	USBD_CDC_ECM_HandleTypeDef *const hcdc_cdc_ecm = & ghcdc;
#else
	USBD_CDC_ECM_HandleTypeDef *const hcdc_cdc_ecm = & ghcdc;
#endif /* USE_USBD_COMPOSITE */

  /* Notify application layer that link is down */
  hcdc_cdc_ecm->LinkStatus = 0U;

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_Control
  *         Manage the CDC_ECM class requests
  * @param  Cmd: Command code
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length)
{
	PRINTF("CDC_ECM_Itf_Control:\n");
	printhex(0, pbuf, length);
#ifdef USE_USBD_COMPOSITE
	USBD_CDC_ECM_HandleTypeDef *const hcdc_cdc_ecm = & ghcdc;
#else
	USBD_CDC_ECM_HandleTypeDef *const hcdc_cdc_ecm = & ghcdc;
#endif /* USE_USBD_COMPOSITE */

  switch (cmd)
  {
    case CDC_ECM_SEND_ENCAPSULATED_COMMAND:
      /* Add your code here */
      break;

    case CDC_ECM_GET_ENCAPSULATED_RESPONSE:
      /* Add your code here */
      break;

    case CDC_ECM_SET_ETH_MULTICAST_FILTERS:
      /* Add your code here */
      break;

    case CDC_ECM_SET_ETH_PWRM_PATTERN_FILTER:
      /* Add your code here */
      break;

    case CDC_ECM_GET_ETH_PWRM_PATTERN_FILTER:
      /* Add your code here */
      break;

    case CDC_ECM_SET_ETH_PACKET_FILTER:
      /* Check if this is the first time we enter */
      if (hcdc_cdc_ecm->LinkStatus == 0U)
      {
        /*
          Setup the Link up at TCP/IP level
        */
        hcdc_cdc_ecm->LinkStatus = 1U;

        /* Modification for MacOS which doesn't send SetInterface before receiving INs */
        if (hcdc_cdc_ecm->NotificationStatus == 0U)
        {
          /* Send notification: NETWORK_CONNECTION Event */
          (void)USBD_CDC_ECM_SendNotification(&hUsbDeviceHS, NETWORK_CONNECTION,
                                              CDC_ECM_NET_CONNECTED, NULL);

          /* Prepare for sending Connection Speed Change notification */
          hcdc_cdc_ecm->NotificationStatus = 1U;
        }
      }
      /* Add your code here */
      break;

    case CDC_ECM_GET_ETH_STATISTIC:
      /* Add your code here */
      break;

    default:
      break;
  }
  UNUSED(length);
  UNUSED(pbuf);

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_Receive
  *         Data received over USB OUT endpoint are sent over CDC_ECM interface
  *         through this function.
  * @param  Buf: Buffer of data to be transmitted
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_Receive(uint8_t *Buf, uint32_t *Len)
{
  /* Get the CDC_ECM handler pointer */
#ifdef USE_USBD_COMPOSITE
	USBD_CDC_ECM_HandleTypeDef *const hcdc_cdc_ecm = & ghcdc;
#else
	USBD_CDC_ECM_HandleTypeDef *const hcdc_cdc_ecm = & ghcdc;
#endif /* USE_USBD_COMPOSITE */

  /* Call Eth buffer processing */
  hcdc_cdc_ecm->RxState = 1U;

  if (nic_rxproc)
	  nic_rxproc(Buf, * Len);
  UNUSED(Len);
  UNUSED(Buf);

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_TransmitCplt
  *         Data transmitted callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_TransmitCplt(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_Process
  *         Data received over USB OUT endpoint are sent over CDC_ECM interface
  *         through this function.
  * @param  pdef: pointer to the USB Device Handle
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_Process(USBD_HandleTypeDef *pdev)
{
  /* Get the CDC_ECM handler pointer */
#ifdef USE_USBD_COMPOSITE
	USBD_CDC_ECM_HandleTypeDef *const hcdc_cdc_ecm = & ghcdc;
#else
	USBD_CDC_ECM_HandleTypeDef *const hcdc_cdc_ecm = & ghcdc;
#endif /* USE_USBD_COMPOSITE */

  if (hcdc_cdc_ecm == NULL)
  {
    return (-1);
  }

  if (hcdc_cdc_ecm->LinkStatus != 0U)
  {
    /*
      Read a received packet from the Ethernet buffers and send it
      to the lwIP for handling
      Call here the TCP/IP background tasks.
    */
  }

  return (0);
}


static uint32_t ConnSpeedTab[2] = {CDC_ECM_CONNECT_SPEED_UPSTREAM,
                                   CDC_ECM_CONNECT_SPEED_DOWNSTREAM
                                  };

/**
  * @}
  */

/** @defgroup USBD_CDC_ECM_Private_Variables
  * @{
  */
static uint8_t ECMInEpAdd = USBD_EP_CDCECM_IN;
static uint8_t ECMOutEpAdd = USBD_EP_CDCECM_OUT;
static uint8_t ECMCmdEpAdd = USBD_EP_CDCECM_INT;

/**
  * @}
  */

/** @defgroup USBD_CDC_ECM_Private_Functions
  * @{
  */

/**
  * @brief  USBD_CDC_ECM_Setup
  *         Handle the CDC_ECM specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static USBD_StatusTypeDef USBD_CDC_ECM_Setup(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	  USBD_CDC_ECM_HandleTypeDef *const hcdc = & ghcdc;
  USBD_CDC_ECM_ItfTypeDef *EcmInterface = & USBD_CDC_ECM_fops; //(USBD_CDC_ECM_ItfTypeDef *)pdev->pUserData[pdev->classId];
  USBD_StatusTypeDef ret = USBD_OK;
  uint16_t len;
  uint16_t status_info = 0U;
  uint8_t ifalt = 0U;

  if (hcdc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_CLASS :
      if (req->wLength != 0U)
      {
        if ((req->bmRequest & 0x80U) != 0U)
        {
          EcmInterface->Control(req->bRequest, (uint8_t *)hcdc->data, req->wLength);

          len = MIN(CDC_ECM_DATA_BUFFER_SIZE, req->wLength);
          (void)USBD_CtlSendData(pdev, (uint8_t *)hcdc->data, len);
        }
        else
        {
          hcdc->CmdOpCode = req->bRequest;
          hcdc->CmdLength = (uint8_t)MIN(req->wLength, USB_MAX_EP0_SIZE);

          (void)USBD_CtlPrepareRx(pdev, (uint8_t *)hcdc->data, hcdc->CmdLength);
        }
      }
      else
      {
        EcmInterface->Control(req->bRequest, (uint8_t *)req, 0U);
      }
      break;

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
            (void)USBD_CtlSendData(pdev, &ifalt, 1U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_SET_INTERFACE:
          if (pdev->dev_state != USBD_STATE_CONFIGURED)
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_CLEAR_FEATURE:
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

  ret = USBD_OK;
  return (uint8_t)ret;
}

/**
  * @brief  USBD_CDC_ECM_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static USBD_StatusTypeDef USBD_CDC_ECM_DataIn(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	  USBD_CDC_ECM_HandleTypeDef *const hcdc = & ghcdc;
  PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef *)pdev->pData;

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  ECMInEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

//  if (pdev->pClassDataCmsit[pdev->classId] == NULL)
//  {
//    return (uint8_t)USBD_FAIL;
//  }

  if (epnum == (ECMInEpAdd & 0x7FU))
  {
    if ((pdev->ep_in[epnum & 0xFU].total_length > 0U) &&
        ((pdev->ep_in[epnum & 0xFU].total_length % hpcd->IN_ep[epnum & 0xFU].maxpacket) == 0U))
    {
      /* Update the packet total length */
      pdev->ep_in[epnum & 0xFU].total_length = 0U;

      /* Send ZLP */
      (void)USBD_LL_Transmit(pdev, epnum, NULL, 0U);
    }
    else
    {
      hcdc->TxState = 0U;
      if (USBD_CDC_ECM_fops.TransmitCplt != NULL)
      {
        USBD_CDC_ECM_fops.TransmitCplt(hcdc->TxBuffer,
                                                                                  &hcdc->TxLength, epnum);
      }
    }
  }
  else if (epnum == (ECMCmdEpAdd & 0x7FU))
  {
    if (hcdc->NotificationStatus != 0U)
    {
      (void)USBD_CDC_ECM_SendNotification(pdev, CONNECTION_SPEED_CHANGE, 0U, (uint8_t *)ConnSpeedTab);

      hcdc->NotificationStatus = 0U;
    }
  }
  else
  {
    return (uint8_t)USBD_FAIL;
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_ECM_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static USBD_StatusTypeDef USBD_CDC_ECM_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	  USBD_CDC_ECM_HandleTypeDef *const hcdc = & ghcdc;
  uint32_t CurrPcktLen;

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  ECMOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

//  if (pdev->pClassDataCmsit[pdev->classId] == NULL)
//  {
//    return (uint8_t)USBD_FAIL;
//  }

  if (epnum == ECMOutEpAdd)
  {
    /* Get the received data length */
    CurrPcktLen = USBD_LL_GetRxDataSize(pdev, epnum);

    /* Increment the frame length */
    hcdc->RxLength += CurrPcktLen;

    /* If the buffer size is less than max packet size: it is the last packet in current frame */
    if ((CurrPcktLen < hcdc->MaxPcktLen) || (hcdc->RxLength >= CDC_ECM_ETH_MAX_SEGSZE))
    {
      /* USB data will be immediately processed, this allow next USB traffic being
      NAKed till the end of the application Xfer */

      /* Process data by application (ie. copy to app buffer or notify user)
      hcdc->RxLength must be reset to zero at the end of the call of this function */
    	PRINTF("Rx::\n");
    	printhex(0, hcdc->RxBuffer, hcdc->RxLength);
      USBD_CDC_ECM_fops.Receive(hcdc->RxBuffer, &hcdc->RxLength);
    }
    else
    {
      /* Prepare Out endpoint to receive next packet in current/new frame */
      (void)USBD_LL_PrepareReceive(pdev, ECMOutEpAdd,
                                   (uint8_t *)(hcdc->RxBuffer + hcdc->RxLength),
                                   hcdc->MaxPcktLen);
    }
  }
  else
  {
    return (uint8_t)USBD_FAIL;
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_ECM_EP0_RxReady
  *         Handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static USBD_StatusTypeDef USBD_CDC_ECM_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  USBD_CDC_ECM_HandleTypeDef * const hcdc = & ghcdc; //(USBD_CDC_ECM_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

  if (hcdc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  //if ((pdev->pUserData[pdev->classId] != NULL) && (hcdc->CmdOpCode != 0xFFU))
  {
    USBD_CDC_ECM_fops.Control(hcdc->CmdOpCode,
                                                                         (uint8_t *)hcdc->data,
                                                                         (uint16_t)hcdc->CmdLength);
    hcdc->CmdOpCode = 0xFFU;

  }
  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_ECM_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CD  Interface callback
  * @retval status
  */
//uint8_t USBD_CDC_ECM_RegisterInterface(USBD_HandleTypeDef *pdev,
//                                       USBD_CDC_ECM_ItfTypeDef *fops)
//{
//  if (fops == NULL)
//  {
//    return (uint8_t)USBD_FAIL;
//  }
//
//  pdev->pUserData[pdev->classId] = fops;
//
//  return (uint8_t)USBD_OK;
//}


/**
  * @brief  USBD_CDC_ECM_SetTxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Tx Buffer
  * @param  length: Tx Buffer length
  * @param  ClassId: The Class ID
  * @retval status
  */
#ifdef USE_USBD_COMPOSITE
uint8_t USBD_CDC_ECM_SetTxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff, uint32_t length, uint8_t ClassId)
{
	  USBD_CDC_ECM_HandleTypeDef *const hcdc = & ghcdc;
#else
uint8_t USBD_CDC_ECM_SetTxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff, uint32_t length)
{
	  USBD_CDC_ECM_HandleTypeDef *const hcdc = & ghcdc;
#endif /* USE_USBD_COMPOSITE */

  if (hcdc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hcdc->TxBuffer = pbuff;
  hcdc->TxLength = length;

  return (uint8_t)USBD_OK;
}


/**
  * @brief  USBD_CDC_ECM_SetRxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Rx Buffer
  * @retval status
  */
uint8_t USBD_CDC_ECM_SetRxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff)
{
	  USBD_CDC_ECM_HandleTypeDef *const hcdc = & ghcdc;

  if (hcdc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hcdc->RxBuffer = pbuff;

  return (uint8_t)USBD_OK;
}


/**
  * @brief  USBD_CDC_ECM_TransmitPacket
  *         Transmit packet on IN endpoint
  * @param  pdev: device instance
  * @param  ClassId: The Class ID
  * @retval status
  */
#ifdef USE_USBD_COMPOSITE
uint8_t USBD_CDC_ECM_TransmitPacket(USBD_HandleTypeDef *pdev, uint8_t ClassId)
{
	  USBD_CDC_ECM_HandleTypeDef *const hcdc = & ghcdc;
#else
uint8_t USBD_CDC_ECM_TransmitPacket(USBD_HandleTypeDef *pdev)
{
	  USBD_CDC_ECM_HandleTypeDef *const hcdc = & ghcdc;
#endif /* USE_USBD_COMPOSITE */

  USBD_StatusTypeDef ret = USBD_BUSY;

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  ECMInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, ClassId);
#endif /* USE_USBD_COMPOSITE */

  if (hcdc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  if (hcdc->TxState == 0U)
  {
    /* Tx Transfer in progress */
    hcdc->TxState = 1U;

    /* Update the packet total length */
    pdev->ep_in[ECMInEpAdd & 0xFU].total_length = hcdc->TxLength;

    /* Transmit next packet */
    (void)USBD_LL_Transmit(pdev, ECMInEpAdd, hcdc->TxBuffer, hcdc->TxLength);

    ret = USBD_OK;
  }

  return (uint8_t)ret;
}


/**
  * @brief  USBD_CDC_ECM_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_CDC_ECM_ReceivePacket(USBD_HandleTypeDef *pdev)
{
	  USBD_CDC_ECM_HandleTypeDef *const hcdc = & ghcdc;

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  ECMOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

//  if (pdev->pClassDataCmsit[pdev->classId] == NULL)
//  {
//    return (uint8_t)USBD_FAIL;
//  }

  /* Prepare Out endpoint to receive next packet */
  (void)USBD_LL_PrepareReceive(pdev, ECMOutEpAdd, hcdc->RxBuffer, hcdc->MaxPcktLen);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_ECM_SendNotification
  *         Transmit Notification packet on CMD IN interrupt endpoint
  * @param  pdev: device instance
  *         Notif: value of the notification type (from CDC_ECM_Notification_TypeDef enumeration list)
  *         bVal: value of the notification switch (ie. 0x00 or 0x01 for Network Connection notification)
  *         pData: pointer to data buffer (ie. upstream and downstream connection speed values)
  * @retval status
  */
uint8_t USBD_CDC_ECM_SendNotification(USBD_HandleTypeDef *pdev,
                                      USBD_CDC_NotifCodeTypeDef Notif,
                                      uint16_t bVal, uint8_t *pData)
{
  uint32_t Idx;
  uint32_t ReqSize = 0U;
  USBD_CDC_ECM_HandleTypeDef *const hcdc = & ghcdc;
  USBD_StatusTypeDef ret = USBD_OK;

  if (hcdc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  ECMCmdEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  /* Initialize the request fields */
  (hcdc->Req).bmRequest = CDC_ECM_BMREQUEST_TYPE_ECM;
  (hcdc->Req).bRequest = (uint8_t)Notif;

  switch ((hcdc->Req).bRequest)
  {
    case NETWORK_CONNECTION:
      (hcdc->Req).wValue = bVal;
      (hcdc->Req).wIndex = INTERFACE_CDCECM_CONTROL;
      (hcdc->Req).wLength = 0U;

      for (Idx = 0U; Idx < 8U; Idx++)
      {
        (hcdc->Req).data[Idx] = 0U;
      }
      ReqSize = 8U;
      break;

    case RESPONSE_AVAILABLE:
      (hcdc->Req).wValue = 0U;
      (hcdc->Req).wIndex = INTERFACE_CDCECM_CONTROL;
      (hcdc->Req).wLength = 0U;
      for (Idx = 0U; Idx < 8U; Idx++)
      {
        (hcdc->Req).data[Idx] = 0U;
      }
      ReqSize = 8U;
      break;

    case CONNECTION_SPEED_CHANGE:
      (hcdc->Req).wValue = 0U;
      (hcdc->Req).wIndex = INTERFACE_CDCECM_CONTROL;
      (hcdc->Req).wLength = 0x0008U;
      ReqSize = 16U;

      /* Check pointer to data buffer */
      if (pData != NULL)
      {
        for (Idx = 0U; Idx < 8U; Idx++)
        {
          (hcdc->Req).data[Idx] = pData[Idx];
        }
      }
      break;

    default:
      ret = USBD_FAIL;
      break;
  }

  /* Transmit notification packet */
  if (ReqSize != 0U)
  {
    (void)USBD_LL_Transmit(pdev, ECMCmdEpAdd, (uint8_t *)&hcdc->Req, ReqSize);
  }

  return (uint8_t)ret;
}


/**
  * @brief  USBD_CDC_ECM_Init
  *         Initialize the CDC_ECM interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static USBD_StatusTypeDef USBD_CDC_ECM_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	  UNUSED(cfgidx);

	  USBD_CDC_ECM_HandleTypeDef *const hcdc = & ghcdc;

	#ifdef USE_USBD_COMPOSITE
	  /* Get the Endpoints addresses allocated for this class instance */
	  ECMInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
	  ECMOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
	  ECMCmdEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
	#endif /* USE_USBD_COMPOSITE */

//	  if (hcdc == NULL)
//	  {
//	    pdev->pClassDataCmsit[pdev->classId] = NULL;
//	    return (uint8_t)USBD_EMEM;
//	  }

	  (void)memset(hcdc, 0, sizeof(USBD_CDC_ECM_HandleTypeDef));

//	  pdev->pClassDataCmsit[pdev->classId] = (void *)hcdc;
//	  pdev->pClassData = pdev->pClassDataCmsit[pdev->classId];

	  if (pdev->dev_speed == USBD_SPEED_HIGH)
	  {
	    /* Open EP IN */
	    (void)USBD_LL_OpenEP(pdev, ECMInEpAdd, USBD_EP_TYPE_BULK,
	    		USBD_CDCECM_IN_BUFSIZE);

	    pdev->ep_in[ECMInEpAdd & 0xFU].is_used = 1U;

	    /* Open EP OUT */
	    (void)USBD_LL_OpenEP(pdev, ECMOutEpAdd, USBD_EP_TYPE_BULK,
	    		USBD_CDCECM_OUT_BUFSIZE);

	    pdev->ep_out[ECMOutEpAdd & 0xFU].is_used = 1U;

	    /* Set bInterval for CDC ECM CMD Endpoint */
	    pdev->ep_in[ECMCmdEpAdd & 0xFU].bInterval = CDC_ECM_HS_BINTERVAL;
	  }
	  else
	  {
	    /* Open EP IN */
	    (void)USBD_LL_OpenEP(pdev, ECMInEpAdd, USBD_EP_TYPE_BULK,
	    		USBD_CDCECM_IN_BUFSIZE);

	    pdev->ep_in[ECMInEpAdd & 0xFU].is_used = 1U;

	    /* Open EP OUT */
	    (void)USBD_LL_OpenEP(pdev, ECMOutEpAdd, USBD_EP_TYPE_BULK,
	    		USBD_CDCECM_OUT_BUFSIZE);

	    pdev->ep_out[ECMOutEpAdd & 0xFU].is_used = 1U;

	    /* Set bInterval for CDC ECM CMD Endpoint */
	    pdev->ep_in[ECMCmdEpAdd & 0xFU].bInterval = CDC_ECM_FS_BINTERVAL;
	  }

	  /* Open Command IN EP */
	  (void)USBD_LL_OpenEP(pdev, ECMCmdEpAdd, USBD_EP_TYPE_INTR, USBD_CDCECM_INT_SIZE);
	  pdev->ep_in[ECMCmdEpAdd & 0xFU].is_used = 1U;

	  hcdc->RxBuffer = NULL;

	  /* Init  physical Interface components */
	  USBD_CDC_ECM_fops.Init();

	  /* Init Xfer states */
	  hcdc->TxState = 0U;
	  hcdc->RxState = 0U;
	  hcdc->RxLength = 0U;
	  hcdc->TxLength = 0U;
	  hcdc->LinkStatus = 0U;
	  hcdc->NotificationStatus = 0U;
	  hcdc->MaxPcktLen = (pdev->dev_speed == USBD_SPEED_HIGH) ? USB_FS_MAX_PACKET_SIZE : \
			  USB_FS_MAX_PACKET_SIZE;

	  if (hcdc->RxBuffer == NULL)
	  {
	    return (uint8_t)USBD_EMEM;
	  }

	  /* Prepare Out endpoint to receive next packet */
	  (void)USBD_LL_PrepareReceive(pdev, ECMOutEpAdd, hcdc->RxBuffer, hcdc->MaxPcktLen);

	  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CDC_ECM_DeInit
  *         DeInitialize the CDC_ECM layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static USBD_StatusTypeDef USBD_CDC_ECM_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
  UNUSED(cfgidx);

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  ECMInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  ECMOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  ECMCmdEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  /* Close EP IN */
  (void)USBD_LL_CloseEP(pdev, ECMInEpAdd);
  pdev->ep_in[ECMInEpAdd & 0xFU].is_used = 0U;

  /* Close EP OUT */
  (void)USBD_LL_CloseEP(pdev, ECMOutEpAdd);
  pdev->ep_out[ECMOutEpAdd & 0xFU].is_used = 0U;

  /* Close Command IN EP */
  (void)USBD_LL_CloseEP(pdev, ECMCmdEpAdd);
  pdev->ep_in[ECMCmdEpAdd & 0xFU].is_used = 0U;
  pdev->ep_in[ECMCmdEpAdd & 0xFU].bInterval = 0U;

  /* DeInit  physical Interface components */
//  if (pdev->pClassDataCmsit[pdev->classId] != NULL)
//  {
//    ((USBD_CDC_ECM_ItfTypeDef *)pdev->pUserData[pdev->classId])->DeInit();
//    USBD_free(pdev->pClassDataCmsit[pdev->classId]);
//    pdev->pClassDataCmsit[pdev->classId] = NULL;
//    pdev->pClassData = NULL;
//  }

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

static void USBD_CDC_ECM_Cold_Init(void)
{

}

/* CDC_ECM interface class callbacks structure */
USBD_ClassTypeDef USBD_CLASS_CDC_ECM =
{
	USBD_CDC_ECM_Cold_Init,
	USBD_CDC_ECM_Init,
	USBD_CDC_ECM_DeInit,
	USBD_CDC_ECM_Setup,
	NULL,                 /* EP0_TxSent, */
	USBD_CDC_ECM_EP0_RxReady,
	USBD_CDC_ECM_DataIn,
	USBD_CDC_ECM_DataOut,
	NULL,	//USBD_XXX_SOF,	// SOF
	NULL,	//USBD_XXX_IsoINIncomplete,	// IsoINIncomplete
	NULL,	//USBD_XXX_IsoOUTIncomplete,	// IsoOUTIncomplete
};


static int nic_can_send(void)
{
	return 0;
}

static void nic_send(const uint8_t *data, int size)
{
}



#if WITHLWIP

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/autoip.h"
#include "netif/etharp.h"
#include "lwip/ip.h"


static struct netif cdcecm_netif_data;

struct netif  * getNetifData(void)
{
	return & cdcecm_netif_data;
}


/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

// Transceiving Ethernet packets
static err_t cdcecm_linkoutput_fn(struct netif *netif, struct pbuf *p)
{
	//PRINTF("cdceem_linkoutput_fn\n");
    int i;
    struct pbuf *q;
    //static uint8_t data [ETH_PAD_SIZE + CDCEEM_MTU + 14 + 4];
    static uint8_t data [8192];
    int size = 0;

    for (i = 0; i < 200; i++)
    {
        if (nic_can_send()) break;
        local_delay_ms(1);
    }

    if (!nic_can_send())
    {
		return ERR_MEM;
    }

	//PRINTF("cdceem_linkoutput_fn: 2\n");
	//printhex(0, p->payload, p->len);
    VERIFY(0 == pbuf_header(p, - ETH_PAD_SIZE));
    size = pbuf_copy_partial(p, data, sizeof data, 0);
//    if (size > sizeof dataX)
//    {
//    	PRINTF("************************ cdceem_linkoutput_fn: size = %d\n", size);
//    	ASSERT(0);
//    }

    nic_send(data, size);
    for (i = 0; i < 200; i++)
    {
        if (nic_can_send()) break;
        local_delay_ms(1);
    }
    return ERR_OK;
}


static err_t cdcecm_output_fn(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr)
{
	err_t e = etharp_output(netif, p, ipaddr);
	if (e == ERR_OK)
	{
		// добавляем свои заголовки требуеющиеся для физического уповня

	}
	return e;
}

static err_t netif_init_cb(struct netif *netif)
{
	PRINTF("cdc eem netif_init_cb\n");
	LWIP_ASSERT("netif != NULL", (netif != NULL));
#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "storch";
#endif /* LWIP_NETIF_HOSTNAME */
	netif->mtu = CDCECM_MTU;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
	netif->state = NULL;
	netif->name[0] = 'E';
	netif->name[1] = 'X';
	netif->output = etharp_output; //cdcecm_output_fn;	// если бы не требовалось добавлять ethernet заголовки, передачва делалась бы тут.
												// и слкдующий callback linkoutput не требовался бы вообще
	netif->linkoutput = cdcecm_linkoutput_fn;	// используется внутри etharp_output
	return ERR_OK;
}


typedef struct nic_buffer_tag
{
	VLIST_ENTRY item;
	struct pbuf *frame;
} nic_buffer_t;

static IRQLSPINLOCK_t nicbufflock = IRQLSPINLOCK_INIT;
#define NICBUFFLOCK_IRQL IRQL_SYSTEM

static void nicbuff_lock(IRQL_t * oldIrql)
{
	IRQLSPIN_LOCK(& nicbufflock, oldIrql, NICBUFFLOCK_IRQL);
}

static void nicbuff_unlock(IRQL_t irql)
{
	IRQLSPIN_UNLOCK(& nicbufflock, irql);
}

static VLIST_ENTRY nic_buffers_free;
static VLIST_ENTRY nic_buffers_ready;

static void nic_buffers_initialize(void)
{
	static RAMFRAMEBUFF nic_buffer_t sliparray [64];
	unsigned i;

	InitializeListHead(& nic_buffers_free);	// Незаполненные
	InitializeListHead(& nic_buffers_ready);	// Для обработки

	for (i = 0; i < (sizeof sliparray / sizeof sliparray [0]); ++ i)
	{
		nic_buffer_t * const p = & sliparray [i];
		InsertHeadVList(& nic_buffers_free, & p->item);
	}
}

static int nic_buffer_alloc(nic_buffer_t * * tp)
{
	IRQL_t oldIrql;
	nicbuff_lock(& oldIrql);
	if (! IsListEmpty(& nic_buffers_free))
	{
		const PVLIST_ENTRY t = RemoveTailVList(& nic_buffers_free);
		nicbuff_unlock(oldIrql);
		nic_buffer_t * const p = CONTAINING_RECORD(t, nic_buffer_t, item);
		* tp = p;
		return 1;
	}
	nicbuff_unlock(oldIrql);
	return 0;
}

static int nic_buffer_ready(nic_buffer_t * * tp)
{
	IRQL_t oldIrql;
	nicbuff_lock(& oldIrql);
	if (! IsListEmpty(& nic_buffers_ready))
	{
		const PVLIST_ENTRY t = RemoveTailVList(& nic_buffers_ready);
		nicbuff_unlock(oldIrql);
		nic_buffer_t * const p = CONTAINING_RECORD(t, nic_buffer_t, item);
		* tp = p;
		return 1;
	}
	nicbuff_unlock(oldIrql);
	return 0;
}

static void nic_buffer_release(nic_buffer_t * p)
{
	IRQL_t oldIrql;
	nicbuff_lock(& oldIrql);
	InsertHeadVList(& nic_buffers_free, & p->item);
	nicbuff_unlock(oldIrql);
}

// сохранить принятый
static void nic_buffer_rx(nic_buffer_t * p)
{
	IRQL_t oldIrql;
	nicbuff_lock(& oldIrql);
	InsertHeadVList(& nic_buffers_ready, & p->item);
	nicbuff_unlock(oldIrql);
}

static void on_packet(const uint8_t *data, int size)
{
	nic_buffer_t * p;
	if (nic_buffer_alloc(& p) != 0)
	{
		struct pbuf *frame;
		frame = pbuf_alloc(PBUF_RAW, size + ETH_PAD_SIZE, PBUF_POOL);
		if (frame == NULL)
		{
			TP();
			nic_buffer_release(p);
			return;
		}
		VERIFY(0 == pbuf_header(frame, - ETH_PAD_SIZE));
		err_t e = pbuf_take(frame, data, size);
		VERIFY(0 == pbuf_header(frame, + ETH_PAD_SIZE));
		if (e == ERR_OK)
		{
			//PRINTF("on_packet:\n");
			//printhex(0, frame->payload, frame->len);
			p->frame = frame;
			nic_buffer_rx(p);
		}
		else
		{
			pbuf_free(frame);
			nic_buffer_release(p);
		}

	}
}



// Receiving Ethernet packets
// user-mode function
static void netif_polling(void * ctx)
{
	(void) ctx;
	nic_buffer_t * p;
	while (nic_buffer_ready(& p) != 0)
	{
		struct pbuf *frame = p->frame;
		nic_buffer_release(p);
		//PRINTF("ethernet_input:\n");
		//printhex(0, frame->payload, frame->len);
		//local_delay_ms(20);
		err_t e = ethernet_input(frame, getNetifData());
		if (e != ERR_OK)
		{
			  /* This means the pbuf is freed or consumed,
			     so the caller doesn't have to free it again */
		}
	}
}


void init_netif(void)
{
#if ETH_PAD_SIZE != 0
	#error Wrong ETH_PAD_SIZE value
#endif
	nic_buffers_initialize();
	nic_rxproc = on_packet;		// разрешаем принимать пакеты адаптеру и отправлять в LWIP

	static const  uint8_t hwaddrv [6]  = { HWADDR };

	static ip_addr_t netmask;// [4] = NETMASK;
	static ip_addr_t gateway;// [4] = GATEWAY;

	IP4_ADDR(& netmask, myNETMASK [0], myNETMASK [1], myNETMASK [2], myNETMASK [3]);
	IP4_ADDR(& gateway, myGATEWAY [0], myGATEWAY [1], myGATEWAY [2], myGATEWAY [3]);

	static ip_addr_t vaddr;// [4]  = IPADDR;
	IP4_ADDR(& vaddr, myIP [0], myIP [1], myIP [2], myIP [3]);

	struct netif  *netif = getNetifData();
	netif->hwaddr_len = 6;
	memcpy(netif->hwaddr, hwaddrv, 6);

	netif = netif_add(netif, & vaddr, & netmask, & gateway, NULL, netif_init_cb, ip_input);
	netif_set_default(netif);

	while (!netif_is_up(netif))
		;

#if LWIP_AUTOIP
	  autoip_start(netif);
#endif /* LWIP_AUTOIP */
	{
		static dpcobj_t dpcobj;

		dpcobj_initialize(& dpcobj, netif_polling, NULL);
		board_dpc_addentry(& dpcobj, board_dpc_coreid());
	}
}


#endif /* WITHLWIP */


#endif /* WITHUSBHW && WITHUSBCDCECM */

