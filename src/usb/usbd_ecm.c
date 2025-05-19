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

static nic_rxproc_t nic_rxproc;

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

/* USB handle declared in main.c */
#define ECM_MAX_SEGMENT_SIZE           1514

/* local function prototyping */

static USBD_StatusTypeDef USBD_ECM_ReceivePacket (USBD_HandleTypeDef *pdev, unsigned index);

static USBD_HandleTypeDef *registered_pdev;

__ALIGN_BEGIN static uint8_t ecm_rx_buffer[ECM_MAX_SEGMENT_SIZE] __ALIGN_END;
__ALIGN_BEGIN static uint8_t ecm_tx_buffer[ECM_MAX_SEGMENT_SIZE] __ALIGN_END;

__ALIGN_BEGIN static USBD_SetupReqTypedef notify __ALIGN_END =
{
  .bmRequest = 0x21,
  .bRequest = NETWORK_CONNECTION /* NETWORK_CONNECTION */,
  .wIndex = INTERFACE_CDCECM_CONTROL,
  .wValue = 1 /* Connected */,
  .wLength = 0,
};

__ALIGN_BEGIN static USBD_SetupReqTypedef notifyData __ALIGN_END =
{
  .bmRequest = 0x21,
  .bRequest = RESPONSE_AVAILABLE /* NETWORK_CONNECTION */,
  .wIndex = INTERFACE_CDCECM_CONTROL,
  .wValue = 0 /* Connected */,
  .wLength = 0,
};

//static int ecm_rx_index;
static int can_xmit;
//static int OutboundTransferNeedsRenewal;
//static uint8_t *ecm_tx_ptr;
//static int ecm_tx_remaining;
//static int ecm_tx_busy;
//static int copy_length;
//static volatile int ecmsendnotifyrequest;
static volatile int ecmsendnotifyrequestData;

//void usb_ecm_recv_renew(void)
//{
//	if (registered_pdev)
//	{
//		  USBD_StatusTypeDef outcome;
//
//		  outcome = USBD_LL_PrepareReceive(registered_pdev, USBD_EP_CDCECM_OUT, ecm_rx_buffer + ecm_rx_index, USBD_CDCECM_OUT_BUFSIZE);
//
//		  OutboundTransferNeedsRenewal = (USBD_OK != outcome); /* set if the HAL was busy so that we know to retry it */
//	}
//}

static USBD_StatusTypeDef USBD_ECM_Init (USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
  registered_pdev = pdev;

  USBD_LL_OpenEP(pdev, USBD_EP_CDCECM_IN, USBD_EP_TYPE_BULK, USBD_CDCECM_IN_BUFSIZE); /* Open EP IN */
  USBD_LL_OpenEP(pdev, USBD_EP_CDCECM_OUT, USBD_EP_TYPE_BULK, USBD_CDCECM_OUT_BUFSIZE); /* Open EP OUT */
  USBD_LL_OpenEP(pdev, USBD_EP_CDCECM_NOTIFY, USBD_EP_TYPE_INTR, USBD_CDCECM_NOTIFY_SIZE);	/* Open Command IN EP */

  USBD_LL_PrepareReceive(pdev, USBD_EP_CDCECM_OUT, ecm_rx_buffer, sizeof ecm_rx_buffer);
  can_xmit = 1 /* true */;

  return USBD_OK;
}

static USBD_StatusTypeDef USBD_ECM_DeInit (USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
  registered_pdev = NULL;

  /* Close EP IN */
  USBD_LL_CloseEP(pdev, USBD_EP_CDCECM_IN);

  /* Close EP OUT */
  USBD_LL_CloseEP(pdev, USBD_EP_CDCECM_OUT);

  /* Close Command IN EP */
  USBD_LL_CloseEP(pdev, USBD_EP_CDCECM_NOTIFY);

  can_xmit = 0 /* false */;

  return USBD_OK;
}

static USBD_StatusTypeDef USBD_ECM_Setup (USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	USBD_StatusTypeDef ret = USBD_OK;
//	PRINTF("USBD_ECM_Setup: ");
//	printhex(0, req, sizeof * req);
	// Android log:
	//	USBD_ECM_Setup: 00000000: 01 0B 00 00 01 00 00 00                          ........
	//	USBD_ECM_Setup: 00000000: 21 43 0C 00 00 00 00 00                          !C......
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	if (interfacev != INTERFACE_CDCECM_CONTROL && interfacev != INTERFACE_CDCECM_DATA)
		return USBD_OK;

	switch (req->bRequest)
	{
	case USB_REQ_SET_INTERFACE:
		// SBD_ECM_Setup: 00000000: 01 0B 00 00 01 00 00 00
		// For INTERFACE_CDCECM_DATA
		if (pdev->dev_state == USBD_STATE_CONFIGURED) {
			//hhid->AltSetting = LO_BYTE(req->wValue);
			USBD_CtlSendStatus(pdev);
		} else {
			USBD_CtlError(pdev, req);
			ret = USBD_FAIL;
		}
		break;

	case 0x43 /* SET_ETHERNET_PACKET_FILTER */:
		//	USBD_ECM_Setup: 00000000: 21 43 0E 00 00 00 00 00
		// For INTERFACE_CDCECM_CONTROL
		//TP();
		//USBD_CtlSendStatus(pdev);
	    //notify.wIndex = INTERFACE_CDCECM_CONTROL;
	    //ecmsendnotifyrequest = 1;
		USBD_CtlSendStatus(pdev);
		USBD_LL_Transmit(pdev, USBD_EP_CDCECM_NOTIFY, (uint8_t *) & notify, sizeof notify);
	    break;

	default:
		TP();
		break;
	}

  return ret;
}

//static void ecm_incoming_attempt(void)
//{
//  int chunk_size;
//
//  if (!ecm_tx_remaining || ecm_tx_busy)
//    return;
//
//  chunk_size = ecm_tx_remaining;
//  if (chunk_size > USBD_CDCECM_IN_BUFSIZE)
//    chunk_size = USBD_CDCECM_IN_BUFSIZE;
//
//  /* ST stack always returns a success code, so reading the return value is pointless */
//  USBD_LL_Transmit(registered_pdev, USBD_EP_CDCECM_IN, ecm_tx_ptr, chunk_size);
//
//  ecm_tx_ptr += chunk_size;
//  ecm_tx_remaining -= chunk_size;
//  ecm_tx_busy = 1;
//}

static USBD_StatusTypeDef USBD_ECM_DataIn (USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	switch ((epnum | 0x80))
	{
	case USBD_EP_CDCECM_IN:
		//PRINTF("Data sent\n");
		can_xmit = 1;
		break;
	case USBD_EP_CDCECM_NOTIFY:
		//PRINTF("Notify sent\n");
		break;
	default:
		TP();
		break;
	}
  return USBD_OK;
}

static USBD_StatusTypeDef USBD_ECM_DataOut (USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	if (USBD_EP_CDCECM_OUT != epnum)
		return USBD_OK;
	if (nic_rxproc)
	{
		nic_rxproc(ecm_rx_buffer, USBD_LL_GetRxDataSize (pdev, epnum));
	}
	USBD_LL_PrepareReceive(pdev, USBD_EP_CDCECM_OUT, ecm_rx_buffer, sizeof ecm_rx_buffer);

	return USBD_OK;
}

int nic_can_send(void)
{
	int outcome;

	__disable_irq();
	outcome = can_xmit;
	__enable_irq();

	return outcome;
}


void nic_send(const uint8_t *data, int size)
{
	if (!registered_pdev || !can_xmit)
		return;
	int packet_size = ulmin16(size, sizeof ecm_tx_buffer);
	memcpy(ecm_tx_buffer, data, packet_size);
	ASSERT(packet_size == size);


	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	can_xmit = 0 /* false */;
	USBD_LL_Transmit(registered_pdev, USBD_EP_CDCECM_IN, ecm_tx_buffer, packet_size);
	LowerIrql(oldIrql);

}

static void USBD_CDC_ECM_Cold_Init(void)
{

}

/* CDC_ECM interface class callbacks structure */
USBD_ClassTypeDef USBD_CLASS_CDC_ECM =
{
	USBD_CDC_ECM_Cold_Init,
	USBD_ECM_Init,
	USBD_ECM_DeInit,
	USBD_ECM_Setup,
	NULL,                 /* EP0_TxSent, */
	NULL,	/* USBD_ECM_EP0_RxReady, */
	USBD_ECM_DataIn,
	USBD_ECM_DataOut,
	NULL, //USBD_ECM_SOF,	//USBD_XXX_SOF,	// SOF
	NULL,	//USBD_XXX_IsoINIncomplete,	// IsoINIncomplete
	NULL,	//USBD_XXX_IsoOUTIncomplete,	// IsoOUTIncomplete
};




void nic_initialize(void)
{
	nic_rxproc = nic_on_packet;		// разрешаем принимать пакеты адаптеру и отправлять в LWIP
}


#endif /* WITHUSBHW && WITHUSBCDCECM */

