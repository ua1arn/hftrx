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

static int ecm_rx_index;
static int can_xmit;
static int OutboundTransferNeedsRenewal;
//static uint8_t *ecm_tx_ptr;
//static int ecm_tx_remaining;
//static int ecm_tx_busy;
//static int copy_length;
static volatile int ecmsendnotifyrequest;
static volatile int ecmsendnotifyrequestData;

void usb_ecm_recv_renew(void)
{
	if (registered_pdev)
	{
		  USBD_StatusTypeDef outcome;

		  outcome = USBD_LL_PrepareReceive(registered_pdev, USBD_EP_CDCECM_OUT, ecm_rx_buffer + ecm_rx_index, USBD_CDCECM_OUT_BUFSIZE);

		  OutboundTransferNeedsRenewal = (USBD_OK != outcome); /* set if the HAL was busy so that we know to retry it */
	}
}

static USBD_StatusTypeDef USBD_ECM_Init (USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
  registered_pdev = pdev;

  USBD_LL_OpenEP(pdev, USBD_EP_CDCECM_IN, USBD_EP_TYPE_BULK, USBD_CDCECM_IN_BUFSIZE); /* Open EP IN */
  USBD_LL_OpenEP(pdev, USBD_EP_CDCECM_OUT, USBD_EP_TYPE_BULK, USBD_CDCECM_OUT_BUFSIZE); /* Open EP OUT */
  USBD_LL_OpenEP(pdev, USBD_EP_CDCECM_INT, USBD_EP_TYPE_INTR, USBD_CDCECM_INT_SIZE);	/* Open Command IN EP */

  usb_ecm_recv_renew();
  can_xmit = 1 /* true */;
  OutboundTransferNeedsRenewal = 0 /* false */;
//  ecm_tx_busy = 0;
//  ecm_tx_remaining = 0;

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
  USBD_LL_CloseEP(pdev, USBD_EP_CDCECM_INT);

  can_xmit = 0 /* false */;

  return USBD_OK;
}

static USBD_StatusTypeDef USBD_ECM_Setup (USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	USBD_StatusTypeDef ret = USBD_OK;
//	PRINTF("USBD_ECM_Setup: ");
//	printhex(0, req, sizeof * req);

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
	    notify.wIndex = INTERFACE_CDCECM_CONTROL;
	    ecmsendnotifyrequest = 1;
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
	case USBD_EP_CDCECM_INT:
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
  uint32_t RxLength;

  if (USBD_EP_CDCECM_OUT != epnum)
    return USBD_OK;
  /* Get the received data length */
  RxLength = USBD_LL_GetRxDataSize (pdev, epnum);

  ecm_rx_index += RxLength;

//  PRINTF("USBD_ECM_DataOut:\n");
//  printhex(0, ecm_rx_buffer, RxLength);

  if (RxLength < USBD_CDCECM_OUT_BUFSIZE)
  {
	  if (nic_rxproc)
	  {
		  nic_rxproc(ecm_rx_buffer, ecm_rx_index);
	  }
    ecm_rx_index = 0;
    usb_ecm_recv_renew();
  }
  else
  {
    /* Initiate next USB packet transfer */
    usb_ecm_recv_renew();
  }

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


static void nic_send(const uint8_t *data, int size)
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
//	PRINTF("cdcecm_linkoutput_fn\n");
    int i;
    struct pbuf *q;
    static uint8_t data [ETH_PAD_SIZE + CDCECM_MTU + 14 + 4];
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
	{
		IRQL_t oldIrql;
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		if (ecmsendnotifyrequest)
		{
			ecmsendnotifyrequest = 0;
			if (registered_pdev)
				USBD_LL_Transmit(registered_pdev, USBD_EP_CDCECM_INT, (uint8_t *) & notify, sizeof notify);
		}
		LowerIrql(oldIrql);

	}
	{
		IRQL_t oldIrql;
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		if (ecmsendnotifyrequestData)
		{
			ecmsendnotifyrequestData = 0;
			if (registered_pdev)
				USBD_LL_Transmit(registered_pdev, USBD_EP_CDCECM_INT, (uint8_t *) & notifyData, sizeof notifyData);
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

