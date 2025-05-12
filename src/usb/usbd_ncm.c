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

#define CDCNCM_MTU 1500


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


#define CFG_TUD_NCM_IN_NTB_MAX_SIZE 2048//(CDCNCM_MTU + 14) //3200
#define CFG_TUD_NCM_OUT_NTB_MAX_SIZE 2048//(CDCNCM_MTU + 14) //3200


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

//struct ecm_notify_struct
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

//static unsigned ncmheadersz = 0x00BA;

//static int ncm_rx_index;
static int can_xmit;
//static int OutboundTransferNeedsRenewal;
//static uint8_t *ncm_tx_ptr;
//static int ncm_tx_remaining;
//static int ncm_tx_busy;
//static int copy_length;
//static volatile int ncmsendnotifyrequest;
static volatile int ncmsendnotifyrequestData;

static uint_fast32_t ecm_blocklength;	// Накоплено входных данных

static void ncm_parse(const uint8_t * data, unsigned length)
{
	static uint_fast32_t ecm_blockscore;		// Входные данные

	static uint_fast32_t ecm_outdatalength;	// Накоплено входных данных
	static uint_fast32_t ecm_outdatascore;	// Накапливаем для пердачи в LwIP

//	PRINTF("ncm_parse:\n");
//	printhex(0, data, length);
//	PRINTF("ncm_parse: length=%03X, ecm_outdatascore=%03X, ecm_outdatalength=%03X, ecm_blockscore=%03X, ecm_blocklength=%03X\n", length, ecm_outdatascore, ecm_outdatalength, ecm_blockscore, ecm_blocklength);

	static uint8_t out_data [CFG_TUD_NCM_IN_NTB_MAX_SIZE];
	if (ecm_blocklength == 0)
	{

		const uint8_t * parser = data;

		// Parse h1
		const nth16_t * h1 = (const nth16_t *) parser;
		ASSERT(USBD_peek_u32_BE(parser + 0) == 0x4E434D48);	// NCMH

//		PRINTF("h1->wBlockLength=%04X\n", h1->wBlockLength);
//		PRINTF("h1->wNdpIndex=%04X\n", h1->wNdpIndex);
//		PRINTF("h1->wBlockLength=%04X\n", h1->wBlockLength);

		ecm_blocklength = h1->wBlockLength;
		ecm_blockscore = 0;

		parser += h1->wNdpIndex;

		// Parse h2
		ASSERT(USBD_peek_u32_BE(parser + 0) == 0x4E434D30);	// NCM0
		const ndp16_t * h2 = (const ndp16_t *) parser;

//		PRINTF("h2->wNextNdpIndex=%04X\n", h2->wNextNdpIndex);
//		PRINTF("h2->datagram [0].wDatagramLength=%04X\n", h2->datagram [0].wDatagramLength);
//		PRINTF("h2->wLength=%04X\n", h2->wLength);

		parser = data + h2->datagram [0].wDatagramIndex;
		ecm_outdatalength = h2->datagram [0].wDatagramLength;
		ecm_outdatascore = 0;

		unsigned chunk = ulmin32(sizeof out_data - ecm_outdatascore, length - h2->datagram [0].wDatagramIndex);
		//PRINTF("1 chunk=%03X\n", chunk);

//		PRINTF("use data 1:\n");
//		printhex(0, parser, chunk);

		memcpy(out_data + ecm_outdatascore, parser, chunk);
		ecm_outdatascore += chunk;
		ecm_outdatalength -= chunk;
	}
	else if (ecm_outdatalength >= 0)
	{
		//PRINTF("ecm_outdatascore=%03X, ecm_outdatalength=%03X, ecm_blockscore=%03X, ecm_blocklength=%03X\n", ecm_outdatascore, ecm_outdatalength, ecm_blockscore, ecm_blocklength);
		unsigned chunk = ulmin32(sizeof out_data - ecm_outdatascore, ulmin32(ecm_outdatalength, length));
		//PRINTF("2 chunk=%03X\n", chunk);
//		PRINTF("use data 2:\n");
//		printhex(0, data, chunk);

		memcpy(out_data + ecm_outdatascore, data, chunk);
		ecm_outdatascore += chunk;
		ecm_outdatalength -= chunk;
	}

	//PRINTF("1 post corr: length=%03X, ecm_outdatascore=%03X, ecm_outdatalength=%03X, ecm_blockscore=%03X, ecm_blocklength=%03X\n", length, ecm_outdatascore, ecm_outdatalength, ecm_blockscore, ecm_blocklength);
	{
		ecm_blocklength -= length;
		ecm_blockscore += length;
	}
	//PRINTF("2 post corr: length=%03X, ecm_outdatascore=%03X, ecm_outdatalength=%03X, ecm_blockscore=%03X, ecm_blocklength=%03X\n", length, ecm_outdatascore, ecm_outdatalength, ecm_blockscore, ecm_blocklength);

	if (ecm_blocklength == 0)
	{
		//PRINTF("eth data:\n");
		//printhex(0, out_data, ecm_outdatascore);
		nic_rxproc(out_data, ecm_outdatascore);
		ecm_blockscore = 0;
		ecm_blocklength = 0;
		ecm_outdatascore = 0;
		ecm_outdatalength = 0;
	}
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

static int nic_can_send(void)
{
	int outcome;

	__disable_irq();
	outcome = can_xmit;
	__enable_irq();

	return outcome;
}


static uint8_t ncm_txheader [] =
{
	0x4E, 0x43, 0x4D, 0x48, 0x0C, 0x00, 0x01, 0x00, 0x28, 0x01, 0x0C, 0x00, 0x4E, 0x43, 0x4D, 0x30,
	0x10, 0x00, 0x00, 0x00, 0xBA, 0x00, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static void nic_send(const uint8_t *data, int size)
{
	if (!registered_pdev || !can_xmit)
		return;

	memcpy(ncm_tx_buffer + 0, ncm_txheader, sizeof ncm_txheader);
	int packet_size = ulmin16(size, sizeof ncm_tx_buffer - sizeof ncm_txheader);
	memcpy(ncm_tx_buffer + sizeof ncm_txheader, data, packet_size);

	ASSERT(packet_size == size);


	uint8_t * parser = ncm_tx_buffer;

	// Parse h1
	nth16_t * h1 = (nth16_t *) parser;
	ASSERT(USBD_peek_u32_BE(parser + 0) == 0x4E434D48);	// NCMH

//		PRINTF("h1->wBlockLength=%04X\n", h1->wBlockLength);
//		PRINTF("h1->wNdpIndex=%04X\n", h1->wNdpIndex);
//		PRINTF("h1->wBlockLength=%04X\n", h1->wBlockLength);

	h1->wBlockLength = sizeof ncm_txheader + packet_size;

	parser += h1->wNdpIndex;

	// Parse h2
	ASSERT(USBD_peek_u32_BE(parser + 0) == 0x4E434D30);	// NCM0
	ndp16_t * h2 = (ndp16_t *) parser;

//		PRINTF("h2->wNextNdpIndex=%04X\n", h2->wNextNdpIndex);
//		PRINTF("h2->datagram [0].wDatagramLength=%04X\n", h2->datagram [0].wDatagramLength);
//		PRINTF("h2->wLength=%04X\n", h2->wLength);

	//parser = data + h2->datagram [0].wDatagramIndex;
	h2->datagram [0].wDatagramLength = packet_size;

	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	can_xmit = 0 /* false */;
	USBD_LL_Transmit(registered_pdev, USBD_EP_CDCNCM_IN, ncm_tx_buffer, sizeof ncm_txheader + packet_size);
	LowerIrql(oldIrql);

}

static USBD_StatusTypeDef USBD_NCM_Init (USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
  registered_pdev = pdev;

  USBD_LL_OpenEP(pdev, USBD_EP_CDCNCM_IN, USBD_EP_TYPE_BULK, USBD_CDCNCM_IN_BUFSIZE); /* Open EP IN */
  USBD_LL_OpenEP(pdev, USBD_EP_CDCNCM_OUT, USBD_EP_TYPE_BULK, USBD_CDCNCM_OUT_BUFSIZE); /* Open EP OUT */
  USBD_LL_OpenEP(pdev, USBD_EP_CDCNCM_INT, USBD_EP_TYPE_INTR, USBD_CDCNCM_INT_SIZE);	/* Open Command IN EP */

  //ncm_rx_index = 0;
  //usb_ncm_recv_renew();
  USBD_LL_PrepareReceive(pdev, USBD_EP_CDCNCM_OUT, ncm_rx_buffer, CFG_TUD_NCM_OUT_NTB_MAX_SIZE);
  can_xmit = 1 /* true */;
  ecm_blocklength = 0;
  //OutboundTransferNeedsRenewal = 0 /* false */;
//  ncm_tx_busy = 0;
//  ncm_tx_remaining = 0;

  return USBD_OK;
}

static USBD_StatusTypeDef USBD_NCM_DeInit (USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
  registered_pdev = NULL;

  /* Close EP IN */
  USBD_LL_CloseEP(pdev, USBD_EP_CDCNCM_IN);

  /* Close EP OUT */
  USBD_LL_CloseEP(pdev, USBD_EP_CDCNCM_OUT);

  /* Close Command IN EP */
  USBD_LL_CloseEP(pdev, USBD_EP_CDCNCM_INT);

  can_xmit = 0 /* false */;

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
		TP();
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
	NULL,                 /* EP0_TxSent, */
	NULL,	/* USBD_NCM_EP0_RxReady, */
	USBD_NCM_DataIn,
	USBD_NCM_DataOut,
	NULL, //USBD_NCM_SOF,	//USBD_XXX_SOF,	// SOF
	NULL,	//USBD_XXX_IsoINIncomplete,	// IsoINIncomplete
	NULL,	//USBD_XXX_IsoOUTIncomplete,	// IsoOUTIncomplete
};






#if WITHLWIP

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/autoip.h"
#include "netif/etharp.h"
#include "lwip/ip.h"


static struct netif cdcncm_netif_data;

struct netif  * getNetifData(void)
{
	return & cdcncm_netif_data;
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
static err_t cdcncm_linkoutput_fn(struct netif *netif, struct pbuf *p)
{
//	PRINTF("cdcncm_linkoutput_fn\n");
    int i;
    struct pbuf *q;
    static uint8_t data [ETH_PAD_SIZE + CDCNCM_MTU + 14 + 4];
    //static uint8_t data [8192];
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

    VERIFY(0 == pbuf_header(p, - ETH_PAD_SIZE));
    size = pbuf_copy_partial(p, data, sizeof data, 0);

    nic_send(data, size);
//    for (i = 0; i < 200; i++)
//    {
//        if (nic_can_send()) break;
//        local_delay_ms(1);
//    }
    return ERR_OK;
}


static err_t cdcncm_output_fn(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr)
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
	netif->mtu = CDCNCM_MTU;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
	netif->state = NULL;
	netif->name[0] = 'E';
	netif->name[1] = 'X';
	netif->output = etharp_output; //cdcncm_output_fn;	// если бы не требовалось добавлять ethernet заголовки, передачва делалась бы тут.
												// и слкдующий callback linkoutput не требовался бы вообще
	netif->linkoutput = cdcncm_linkoutput_fn;	// используется внутри etharp_output
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
//	{
//		IRQL_t oldIrql;
//		RiseIrql(IRQL_SYSTEM, & oldIrql);
//		if (ncmsendnotifyrequest)
//		{
//			ncmsendnotifyrequest = 0;
//			if (registered_pdev)
//				USBD_LL_Transmit(registered_pdev, USBD_EP_CDCNCM_INT, (uint8_t *) & notify, sizeof notify);
//		}
//		LowerIrql(oldIrql);
//
//	}
	{
		IRQL_t oldIrql;
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		if (ncmsendnotifyrequestData)
		{
			ncmsendnotifyrequestData = 0;
			if (registered_pdev)
				USBD_LL_Transmit(registered_pdev, USBD_EP_CDCNCM_INT, (uint8_t *) & notifyData, sizeof notifyData);
		}
		LowerIrql(oldIrql);

	}
	(void) ctx;
	nic_buffer_t * p;
	while (nic_buffer_ready(& p) != 0)
	{
		struct pbuf *frame = p->frame;
		nic_buffer_release(p);
//		PRINTF("ethernet_input:\n");
//		printhex(0, frame->payload, frame->len);
		//local_delay_ms(20);
		err_t e = ethernet_input(frame, getNetifData());
		if (e != ERR_OK)
		{
			  /* This means the pbuf is freed or consumed,
			     so the caller doesn't have to free it again */
		}
	}
}


void req(void)
{
	ncmsendnotifyrequestData = 1;
}

void req2(void)
{
	//ncmsendnotifyrequest = 1;
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


#endif /* WITHUSBHW && WITHUSBCDCNCM */

