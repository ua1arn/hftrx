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

#if WITHUSBHW && WITHUSBCDCNCM

#include "buffers.h"
#include "formats.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usb200.h"
#include "usbch9.h"

//	Class-Specific Request Codes for Ethernet subclass.

#define 	SET_ETHERNET_PM_FILTER   0x41
#define 	GET_ETHERNET_PM_FILTER   0x42
#define 	SET_ETHERNET_PACKET_FILTER   0x43
#define 	GET_ETHERNET_STATISTIC   0x44
#define 	GET_NTB_PARAMETERS   0x80

//	Class-Specific Request Codes for NCM subclass.

#define 	GET_NET_ADDRESS   0x81
#define 	SET_NET_ADDRESS   0x82
#define 	GET_NTB_FORMAT   0x83
#define 	SET_NTB_FORMAT   0x84
#define 	GET_NTB_INPUT_SIZE   0x85
#define 	SET_NTB_INPUT_SIZE   0x86
#define 	GET_MAX_DATAGRAM_SIZE   0x87
#define 	SET_MAX_DATAGRAM_SIZE   0x88
#define 	GET_CRC_MODE   0x89
#define 	SET_CRC_MODE   0x8A
#define 	PACKET_TYPE_MULTICAST   0x10

typedef void (*cdcncm_rxproc_t)(const uint8_t *data, int size);

static cdcncm_rxproc_t nic_rxproc = NULL;

typedef enum
{
  NETWORK_CONNECTION = 0x00,
  RESPONSE_AVAILABLE = 0x01,
  CONNECTION_SPEED_CHANGE = 0x2A
} USBD_CDC_NotifCodeTypeDef;

/** @defgroup USBD_CDC_NCM_Private_FunctionPrototypes
  * @{
  */

/* USB handle declared in main.c */
//#define NCM_MAX_SEGMENT_SIZE           1514

#define CFG_TUD_NCM_IN_NTB_MAX_SIZE 2048
#define CFG_TUD_NCM_OUT_NTB_MAX_SIZE 2048


typedef struct __PACKED
{
	uint16_t wLength;
	uint16_t bmNtbFormatsSupported;
	uint32_t dwNtbInMaxSize;
	uint16_t wNdbInDivisor;
	uint16_t wNdbInPayloadRemainder;
	uint16_t wNdbInAlignment;
	uint16_t wReserved;
	uint32_t dwNtbOutMaxSize;
	uint16_t wNdbOutDivisor;
	uint16_t wNdbOutPayloadRemainder;
	uint16_t wNdbOutAlignment;
	uint16_t wNtbOutMaxDatagrams;
} Ntb_parameters_t;

typedef struct __PACKED
{
	uint32_t dwSignature;
	uint16_t wHeaderLength;
	uint16_t wSequence;
	uint16_t wBlockLength;
	uint16_t wNdpIndex;
} nth16_t;

typedef struct __PACKED
{
	uint16_t wDatagramIndex;
	uint16_t wDatagramLength;
} ndp16_datagram_t;

typedef struct __PACKED
{
	uint32_t dwSignature;
	uint16_t wLength;
	uint16_t wNextNdpIndex;
	ndp16_datagram_t datagram[];
} ndp16_t;

typedef union __PACKED {
	struct {
		nth16_t nth;
		ndp16_t ndp;
	};
	uint8_t data[CFG_TUD_NCM_IN_NTB_MAX_SIZE];
} transmit_ntb_t;

//struct ncm_notify_struct
//{
//	tusb_control_request_t header;
//	uint32_t downlink, uplink;
//};

__ALIGN_BEGIN static Ntb_parameters_t ntbParams __ALIGN_END =
{
    .wLength                 = sizeof(Ntb_parameters_t),
    .bmNtbFormatsSupported   = 0x01,
    .dwNtbInMaxSize          = CFG_TUD_NCM_IN_NTB_MAX_SIZE,
    .wNdbInDivisor           = 4,
    .wNdbInPayloadRemainder  = 0,
    .wNdbInAlignment         = 4, //CFG_TUD_NCM_ALIGNMENT,
    .wReserved               = 0,
    .dwNtbOutMaxSize         = CFG_TUD_NCM_OUT_NTB_MAX_SIZE,
    .wNdbOutDivisor          = 4,
    .wNdbOutPayloadRemainder = 0,
    .wNdbOutAlignment        = 4, //CFG_TUD_NCM_ALIGNMENT,
    .wNtbOutMaxDatagrams     = 0
};

/* local function prototyping */

static USBD_HandleTypeDef *registered_pdev;

__ALIGN_BEGIN static uint8_t ncm_rx_buffer[CFG_TUD_NCM_OUT_NTB_MAX_SIZE] __ALIGN_END;
__ALIGN_BEGIN static uint8_t ncm_tx_buffer[CFG_TUD_NCM_IN_NTB_MAX_SIZE] __ALIGN_END;

__ALIGN_BEGIN static USBD_SetupReqTypedef notify __ALIGN_END =
{
  .bmRequest = 0x21,
  .bRequest = NETWORK_CONNECTION /* NETWORK_CONNECTION */,
  .wIndex = INTERFACE_CDCNCM_CONTROL,
  .wValue = 1 /* Connected */,
  .wLength = 0,
};

__ALIGN_BEGIN static USBD_SetupReqTypedef notifyData __ALIGN_END =
{
  .bmRequest = 0x21,
  .bRequest = RESPONSE_AVAILABLE /* NETWORK_CONNECTION */,
  .wIndex = INTERFACE_CDCNCM_CONTROL,
  .wValue = 1 /* Connected */,
  .wLength = 0,
};

static volatile int can_xmit;

static void ncm_parse(const uint8_t * data, unsigned length)
{
	unsigned index = 0;
//	PRINTF("ncm_parse:\n");
//	printhex(0, data, length);
	switch (USBD_peek_u32_BE(data + 0))
	{
	case 0x4E434D48:
		index = USBD_peek_u16(data + offsetof(nth16_t, wNdpIndex));
		break;
	default:
		TP();
		return;
	}
	// Parse datagram header
	while (index != 0)
	{
		const uint8_t * const h2 = data + index;
		// Parse h2
		switch (USBD_peek_u32_BE(data + index + 0))
		{
		case 0x4E434D30:
		case 0x4E434D31:
			// NCM0 (NCM1 - with CRC32)
			{
				//const ndp16_t * h2 = (const ndp16_t *) parser;

				// Parse datagrams
				unsigned datagram;
				for (datagram = 0; ; ++ datagram)
				{
					unsigned wDatagramIndex = USBD_peek_u16(h2 + offsetof(ndp16_t, datagram [datagram].wDatagramIndex));
					unsigned wDatagramLength = USBD_peek_u16(h2 + offsetof(ndp16_t, datagram [datagram].wDatagramLength));
					if (wDatagramIndex == 0 || wDatagramLength == 0)
						break;
					if (nic_rxproc)
						nic_rxproc(data + wDatagramIndex, wDatagramLength);
				}
				index = USBD_peek_u16(h2 + offsetof(ndp16_t, wNextNdpIndex));
			}
			break;
		default:
			TP();
			return;
		}
	}
}

int nic_can_send(void)
{
	int outcome;
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);

	outcome = can_xmit;

	LowerIrql(oldIrql);

	return outcome;
}

static uint_fast16_t ncm_tx_seqnumber;

static size_t alignup4(size_t v)
{
	return (v + 3) / 4 * 4;
}

void nic_send(const uint8_t *data, int size)
{
//	static uint8_t ncm_txheader [] =
//	{
//		0x4E, 0x43, 0x4D, 0x48, 0x0C, 0x00, 0x01, 0x00, 0x28, 0x01, 0x0C, 0x00, 0x4E, 0x43, 0x4D, 0x30,
//		0x10, 0x00, 0x00, 0x00, 0xBA, 0x00, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//	};
	const size_t header_size =
			sizeof (nth16_t) +
			offsetof(ndp16_t, datagram [2]) +	// One element and list terminator
			0;

	if (!registered_pdev || !can_xmit)
		return;

	const size_t packet_size = ulmin32(size, sizeof ncm_tx_buffer - header_size);
	memcpy(ncm_tx_buffer + header_size, data, packet_size);

	ASSERT(packet_size == size);

	uint8_t * parser = ncm_tx_buffer;

	// Prepare h1
//	nth16_t * h1 = (nth16_t *) parser;
//	h1->dwSignature = 0x484D434E;	// NCMH
//	h1->wBlockLength = header_size + packet_size;
//	h1->wSequence = ncm_tx_seqnumber ++;
//	h1->wHeaderLength = sizeof (nth16_t);
//	h1->wNdpIndex = sizeof (nth16_t);
	USBD_poke_u32(parser + offsetof(nth16_t, dwSignature), 0x484D434E);
	USBD_poke_u16(parser + offsetof(nth16_t, wBlockLength), alignup4(header_size + packet_size));
	USBD_poke_u16(parser + offsetof(nth16_t, wSequence), ncm_tx_seqnumber ++);
	USBD_poke_u16(parser + offsetof(nth16_t, wHeaderLength), sizeof (nth16_t));
	USBD_poke_u16(parser + offsetof(nth16_t, wNdpIndex), sizeof (nth16_t));

	parser += sizeof (nth16_t);

	// Prepare h2
//	ndp16_t * h2 = (ndp16_t *) parser;
//	h2->dwSignature = 0x304D434E;	// NCM0	- without CRC32
//	h2->wLength = 0x0010;
//	h2->wNextNdpIndex = 0x0000;
	USBD_poke_u32(parser + offsetof(ndp16_t, dwSignature), 0x304D434E);
	USBD_poke_u16(parser + offsetof(ndp16_t, wLength), 0x0010);
	USBD_poke_u16(parser + offsetof(ndp16_t, wNextNdpIndex), 0x0000);

//		PRINTF("h2->wNextNdpIndex=%04X\n", h2->wNextNdpIndex);
//		PRINTF("h2->datagram [0].wDatagramLength=%04X\n", h2->datagram [0].wDatagramLength);
//		PRINTF("h2->wLength=%04X\n", h2->wLength);

//	h2->datagram [0].wDatagramIndex = header_size;
//	h2->datagram [0].wDatagramLength = packet_size;
	USBD_poke_u16(parser + offsetof(ndp16_t, datagram [0].wDatagramIndex), header_size);
	USBD_poke_u16(parser + offsetof(ndp16_t, datagram [0].wDatagramLength), packet_size);
	// List terminator
//	h2->datagram [1].wDatagramIndex = 0;
//	h2->datagram [1].wDatagramLength = 0;
	USBD_poke_u16(parser + offsetof(ndp16_t, datagram [1].wDatagramIndex), 0);
	USBD_poke_u16(parser + offsetof(ndp16_t, datagram [1].wDatagramLength), 0);

	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);

	const USBD_StatusTypeDef st = USBD_LL_Transmit(registered_pdev, USBD_EP_CDCNCM_IN, ncm_tx_buffer, header_size + packet_size);

	can_xmit = 0;

	LowerIrql(oldIrql);
}

static USBD_StatusTypeDef USBD_NCM_Init (USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	USBD_LL_OpenEP(pdev, USBD_EP_CDCNCM_IN, USBD_EP_TYPE_BULK, USBD_CDCNCM_IN_BUFSIZE); /* Open EP IN */
	USBD_LL_OpenEP(pdev, USBD_EP_CDCNCM_OUT, USBD_EP_TYPE_BULK, USBD_CDCNCM_OUT_BUFSIZE); /* Open EP OUT */
	USBD_LL_OpenEP(pdev, USBD_EP_CDCNCM_INT, USBD_EP_TYPE_INTR, USBD_CDCNCM_INT_SIZE);	/* Open Command IN EP */

	USBD_LL_PrepareReceive(pdev, USBD_EP_CDCNCM_OUT, ncm_rx_buffer, CFG_TUD_NCM_OUT_NTB_MAX_SIZE);

	can_xmit = 1 /* true */;
	ncm_tx_seqnumber = 0;
	registered_pdev = pdev;

	return USBD_OK;
}

static USBD_StatusTypeDef USBD_NCM_DeInit (USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	registered_pdev = NULL;
	can_xmit = 0 /* false */;

	USBD_LL_CloseEP(pdev, USBD_EP_CDCNCM_IN);
	USBD_LL_CloseEP(pdev, USBD_EP_CDCNCM_OUT);
	USBD_LL_CloseEP(pdev, USBD_EP_CDCNCM_INT);

	return USBD_OK;
}

static USBD_StatusTypeDef USBD_NCM_Setup (USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	USBD_StatusTypeDef ret = USBD_OK;
	PRINTF("USBD_NCM_Setup: ");
	printhex(0, req, sizeof * req);

// Android log:
//	USBD_NCM_Setup: 00000000: 01 0B 01 00 01 00 00 00                          ........
//	USBD_NCM_Setup: 00000000: 01 0B 00 00 01 00 00 00                          ........
//	USBD_NCM_Setup: 00000000: A1 80 00 00 00 00 1C 00                          ........

	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	if (interfacev != INTERFACE_CDCNCM_CONTROL && interfacev != INTERFACE_CDCNCM_DATA)
		return USBD_OK;

	switch (req->bRequest)
	{
	case USB_REQ_SET_INTERFACE:
		// For INTERFACE_CDCNCM_DATA
		if (pdev->dev_state == USBD_STATE_CONFIGURED) {
			//hhid->AltSetting = LO_BYTE(req->wValue);
			USBD_CtlSendStatus(pdev);
		} else {
			USBD_CtlError(pdev, req);
			ret = USBD_FAIL;
		}
		break;

	case 0x43 /* SET_ETHERNET_PACKET_FILTER */:
		//	USBD_NCM_Setup: 00000000: 21 43 0E 00 00 00 00 00
		// For INTERFACE_CDCNCM_CONTROL
		USBD_CtlSendStatus(pdev);
	    //ncmsendnotifyrequest = 1;
		USBD_LL_Transmit(pdev, USBD_EP_CDCNCM_INT, (uint8_t *) & notify, sizeof notify);
	    break;

	case GET_NTB_PARAMETERS:
		USBD_CtlSendData(pdev, (const uint8_t *) & ntbParams, ulmin16(sizeof ntbParams, req->wLength));
		break;

	case SET_NTB_INPUT_SIZE:
		TP();
		USBD_CtlSendStatus(pdev);
		break;

	default:
		//USBD_NCM_Setup: 00000000: 40 01 35 00 01 00 01 00                          @.5.....
		TP();
		USBD_CtlSendStatus(pdev);
		break;
	}

	return ret;
}

static USBD_StatusTypeDef USBD_NCM_DataIn (USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	switch ((epnum | 0x80))
	{
	case USBD_EP_CDCNCM_IN:
		//PRINTF("Data sent\n");
		can_xmit = 1;
		break;
	case USBD_EP_CDCNCM_INT:
		PRINTF("Notify sent\n");
		break;
	default:
		//TP();
		break;
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_NCM_DataOut (USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	uint32_t RxLength;

	if (USBD_EP_CDCNCM_OUT != epnum)
		return USBD_OK;
	/* Get the received data length */
	RxLength = USBD_LL_GetRxDataSize (pdev, epnum);

//	PRINTF("USBD_NCM_DataOut:\n");
//	printhex(0, ncm_rx_buffer, RxLength);

	ncm_parse(ncm_rx_buffer, RxLength);	// last in transaction
	USBD_LL_PrepareReceive(pdev, USBD_EP_CDCNCM_OUT, ncm_rx_buffer, CFG_TUD_NCM_OUT_NTB_MAX_SIZE);

	return USBD_OK;
}

static void USBD_CDC_NCM_Cold_Init(void)
{

}

/* CDC_NCM interface class callbacks structure */
USBD_ClassTypeDef USBD_CLASS_CDC_NCM =
{
	USBD_CDC_NCM_Cold_Init,
	USBD_NCM_Init,
	USBD_NCM_DeInit,
	USBD_NCM_Setup,
	NULL,   /* EP0_TxSent, */
	NULL,	/* USBD_NCM_EP0_RxReady, */
	USBD_NCM_DataIn,
	USBD_NCM_DataOut,
	NULL, //USBD_NCM_SOF,	//USBD_XXX_SOF,	// SOF
	NULL,	//USBD_XXX_IsoINIncomplete,	// IsoINIncomplete
	NULL,	//USBD_XXX_IsoOUTIncomplete,	// IsoOUTIncomplete
};

// Debug function
void req1(void)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);

	if (registered_pdev)
		USBD_LL_Transmit(registered_pdev, USBD_EP_CDCNCM_INT, (uint8_t *) & notifyData, sizeof notifyData);

	LowerIrql(oldIrql);
}

// Debug function
void req2(void)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);

	if (registered_pdev)
		USBD_LL_Transmit(registered_pdev, USBD_EP_CDCNCM_INT, (uint8_t *) & notify, sizeof notify);

	LowerIrql(oldIrql);
}


void nic_initialize(void)
{
	nic_rxproc = nic_on_packet;		// разрешаем принимать пакеты адаптеру и отправлять в LWIP
}

#endif /* WITHUSBHW && WITHUSBCDCNCM */

