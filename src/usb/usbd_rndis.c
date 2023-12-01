/*
 * usbd_rndis.c
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
*/

#include "hardware.h"

#if WITHUSBHW && WITHUSBRNDIS

#include "buffers.h"
#include <stdbool.h>
#include <stddef.h>
#include "rndis_protocol.h"


#include "formats.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usb200.h"
#include "usbch9.h"

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/autoip.h"
#include "netif/etharp.h"
#include "lwip/ip.h"

//
//#define RNDIS_CONTROL_OUT_PMAADDRESS                    (0x08 * 4)                //8 bytes per EP
//#define RNDIS_CONTROL_IN_PMAADDRESS                     (RNDIS_CONTROL_OUT_PMAADDRESS + USB_MAX_EP0_SIZE)
//#define RNDIS_NOTIFICATION_IN_PMAADDRESS                (RNDIS_CONTROL_IN_PMAADDRESS + USB_MAX_EP0_SIZE)
//#define RNDIS_DATA_IN_PMAADDRESS                        (RNDIS_NOTIFICATION_IN_PMAADDRESS + RNDIS_NOTIFICATION_IN_SZ)
//#define RNDIS_DATA_OUT_PMAADDRESS                       (RNDIS_DATA_IN_PMAADDRESS + USBD_RNDIS_IN_BUFSIZE)

#define RNDIS_MTU                                       1500                           // MTU value
#if WITHUSBDEV_HSDESC
	#define ETH_LINK_SPEED                                  480000000                       // bits per sec
#else /* WITHUSBDEV_HSDESC */
	#define ETH_LINK_SPEED                                  12000000                       // bits per sec
#endif /* WITHUSBDEV_HSDESC */
#define RNDIS_VENDOR                                    "fetisov"                      // NIC vendor name
#define STATION_HWADDR                                  0x30,0x89,0x84,0x6A,0x96,0xAA  // station MAC
#define PERMANENT_HWADDR                                0x30,0x89,0x84,0x6A,0x96,0xAA  // permanent MAC

#define ETH_HEADER_SIZE                 14
#define ETH_MIN_PACKET_SIZE             60
#define ETH_MAX_PACKET_SIZE             (ETH_HEADER_SIZE + RNDIS_MTU)
#define RNDIS_HEADER_SIZE               (sizeof (rndis_data_packet_t))
#define RNDIS_RX_BUFFER_SIZE            (RNDIS_HEADER_SIZE + ETH_MAX_PACKET_SIZE)

typedef void (*rndis_rxproc_t)(const uint8_t *data, int size);
static rndis_state_t rndis_state;
static rndis_rxproc_t rndis_rxproc = NULL;

static usb_eth_stat_t usb_eth_stat = { 0, 0, 0, 0 };

static int rndis_tx_start(uint8_t *data, uint16_t size);
static int rndis_tx_started(void);

static int rndis_can_send(void);
static void rndis_send(const void *data, int size);


typedef struct rndisbuf_tag
{
	LIST_ENTRY item;
	struct pbuf *frame;
} rndisbuf_t;


static LIST_ENTRY rndis_free;
static LIST_ENTRY rndis_ready;

static void rndis_buffers_initialize(void)
{
	static RAMFRAMEBUFF rndisbuf_t sliparray [64];
	unsigned i;

	InitializeListHead(& rndis_free);	// Незаполненные
	InitializeListHead(& rndis_ready);	// Для обработки

	for (i = 0; i < (sizeof sliparray / sizeof sliparray [0]); ++ i)
	{
		rndisbuf_t * const p = & sliparray [i];
		InsertHeadList(& rndis_free, & p->item);
	}
}

static int rndis_buffers_alloc(rndisbuf_t * * tp)
{
	if (! IsListEmpty(& rndis_free))
	{
		const PLIST_ENTRY t = RemoveTailList(& rndis_free);
		rndisbuf_t * const p = CONTAINING_RECORD(t, rndisbuf_t, item);
		* tp = p;
		return 1;
	}
	if (! IsListEmpty(& rndis_ready))
	{
		const PLIST_ENTRY t = RemoveTailList(& rndis_ready);
		rndisbuf_t * const p = CONTAINING_RECORD(t, rndisbuf_t, item);
		* tp = p;
		return 1;
	}
	return 0;
}

static int rndis_buffers_ready_user(rndisbuf_t * * tp)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	if (! IsListEmpty(& rndis_ready))
	{
		const PLIST_ENTRY t = RemoveTailList(& rndis_ready);
		LowerIrql(oldIrql);
		rndisbuf_t * const p = CONTAINING_RECORD(t, rndisbuf_t, item);
		* tp = p;
		return 1;
	}
	LowerIrql(oldIrql);
	return 0;
}


static void rndis_buffers_release(rndisbuf_t * p)
{
	InsertHeadList(& rndis_free, & p->item);
}

static void rndis_buffers_release_user(rndisbuf_t * p)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	rndis_buffers_release(p);
	LowerIrql(oldIrql);
}

// сохранить принятый
static void rndis_buffers_rx(rndisbuf_t * p)
{
	InsertHeadList(& rndis_ready, & p->item);
}

static void on_packet(const uint8_t *data, int size)
{
	rndisbuf_t * p;
	if (rndis_buffers_alloc(& p) != 0)
	{
		struct pbuf *frame;
		frame = pbuf_alloc(PBUF_RAW, size + ETH_PAD_SIZE, PBUF_POOL);
		if (frame == NULL)
		{
			TP();
			rndis_buffers_release(p);
			return;
		}
		pbuf_header(frame, - ETH_PAD_SIZE);
		err_t e = pbuf_take(frame, data, size);
		pbuf_header(frame, + ETH_PAD_SIZE);
		if (e == ERR_OK)
		{
			p->frame = frame;
			rndis_buffers_rx(p);
		}
		else
		{
			pbuf_free(frame);
			rndis_buffers_release(p);
		}

	}
}



static struct netif rndis_netif_data;


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
static err_t rndis_linkoutput_fn(struct netif *netif, struct pbuf *p)
{
	//PRINTF("rndis_linkoutput_fn\n");
    int i;
    struct pbuf *q;
    static char data [RNDIS_HEADER_SIZE + RNDIS_MTU + 14 + 4];
    int size = 0;

    for (i = 0; i < 200; i++)
    {
        if (rndis_can_send()) break;
        local_delay_ms(1);
    }

    if (!rndis_can_send())
    {
		return ERR_MEM;
    }

	pbuf_header(p, - ETH_PAD_SIZE);
    size = pbuf_copy_partial(p, data, sizeof data, 0);

    rndis_send(data, size);

    return ERR_OK;
}


static err_t rndis_output_fn(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
	err_t e = etharp_output(netif, q, ipaddr);
	if (e == ERR_OK)
	{
#if 0
		rndis_data_packet_t * hdr;
		unsigned size = q->len;
		// добавляем свои заголовки требуеющиеся для физического уповня
		  /* make room for RNDIS header - should not fail */
		  if (pbuf_header(q, RNDIS_HEADER_SIZE) != 0) {
		    /* bail out */
		    LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
		      ("rndis_output_fn: could not allocate room for header.\n"));
		    return ERR_BUF;
		  }

		  hdr = (rndis_data_packet_t *) q->payload;
		  memset(hdr, 0, RNDIS_HEADER_SIZE);
		  hdr->MessageType = REMOTE_NDIS_PACKET_MSG;
		  hdr->MessageLength = RNDIS_HEADER_SIZE + size;
		  hdr->DataOffset = RNDIS_HEADER_SIZE - offsetof(rndis_data_packet_t, DataOffset);
		  hdr->DataLength = size;
#endif
	}
	return e;
}

static err_t netif_init_cb(struct netif *netif)
{
	PRINTF("rndis netif_init_cb\n");
	LWIP_ASSERT("netif != NULL", (netif != NULL));
#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "storch";
#endif /* LWIP_NETIF_HOSTNAME */
	netif->mtu = RNDIS_MTU;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
	netif->state = NULL;
	netif->name[0] = 'E';
	netif->name[1] = 'X';
	netif->output = rndis_output_fn;	// если бы не требовалось добавлять ethernet заголовки, передачва делалась бы тут.
												// и слкдующий callback linkoutput не требовался бы вообще
	netif->linkoutput = rndis_linkoutput_fn;	// используется внутри etharp_output
	return ERR_OK;
}

void init_netif(void)
{
	rndis_buffers_initialize();
	rndis_rxproc = on_packet;		// разрешаем принимать пакеты адаптеру и отправлять в LWIP

	static const  uint8_t hwaddrv [6]  = { HWADDR };

	static ip_addr_t netmask;// [4] = NETMASK;
	static ip_addr_t gateway;// [4] = GATEWAY;

	IP4_ADDR(& netmask, myNETMASK [0], myNETMASK [1], myNETMASK [2], myNETMASK [3]);
	IP4_ADDR(& gateway, myGATEWAY [0], myGATEWAY [1], myGATEWAY [2], myGATEWAY [3]);

	static ip_addr_t vaddr;// [4]  = IPADDR;
	IP4_ADDR(& vaddr, myIP [0], myIP [1], myIP [2], myIP [3]);

	struct netif  *netif = &rndis_netif_data;
	netif->hwaddr_len = 6;
	memcpy(netif->hwaddr, hwaddrv, 6);

	netif = netif_add(netif, & vaddr, & netmask, & gateway, NULL, netif_init_cb, ip_input);
	netif_set_default(netif);

	while (!netif_is_up(netif))
		;

#if LWIP_AUTOIP
	  autoip_start(netif);
#endif /* LWIP_AUTOIP */
}



// Receiving Ethernet packets
// user-mode function
void usb_polling(void)
{
	rndisbuf_t * p;
	if (rndis_buffers_ready_user(& p) != 0)
	{
		struct pbuf *frame = p->frame;
		rndis_buffers_release_user(p);

		err_t e = ethernet_input(frame, & rndis_netif_data);
		if (e != ERR_OK)
		{
			  /* This means the pbuf is freed or consumed,
			     so the caller doesn't have to free it again */
		}

	}
}

struct netif  * getNetifData(void)
{
	return &rndis_netif_data;
}

static void USBD_RNDIS_ColdInit(void)
{
}

/*
* The MIT License (MIT)
*
* Copyright (c) 2015 by Sergey Fetisov <fsenok@gmail.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

/*
* version: 1.0 demo (7.02.2015)
*/

// RNDIS to USB Mapping https://msdn.microsoft.com/en-us/library/windows/hardware/ff570657(v=vs.85).aspx
// https://docs.microsoft.com/en-us/windows-hardware/drivers/network/data-channel-characteristics
//
//#include "usbd_conf.h"
//#include "usbd_rndis.h"
//#include "usbd_desc.h"
//#include "usbd_ctlreq.h"

/*******************************************************************************
Private constants
*******************************************************************************/

const uint32_t OIDSupportedList[] =
{
  OID_GEN_SUPPORTED_LIST,
  OID_GEN_HARDWARE_STATUS,
  OID_GEN_MEDIA_SUPPORTED,
  OID_GEN_MEDIA_IN_USE,
  //    OID_GEN_MAXIMUM_LOOKAHEAD,
  OID_GEN_MAXIMUM_FRAME_SIZE,
  OID_GEN_LINK_SPEED,
  //    OID_GEN_TRANSMIT_BUFFER_SPACE,
  //    OID_GEN_RECEIVE_BUFFER_SPACE,
  OID_GEN_TRANSMIT_BLOCK_SIZE,
  OID_GEN_RECEIVE_BLOCK_SIZE,
  OID_GEN_VENDOR_ID,
  OID_GEN_VENDOR_DESCRIPTION,
  OID_GEN_VENDOR_DRIVER_VERSION,
  OID_GEN_CURRENT_PACKET_FILTER,
  //    OID_GEN_CURRENT_LOOKAHEAD,
  //    OID_GEN_DRIVER_VERSION,
  OID_GEN_MAXIMUM_TOTAL_SIZE,
  OID_GEN_PROTOCOL_OPTIONS,
  OID_GEN_MAC_OPTIONS,
  OID_GEN_MEDIA_CONNECT_STATUS,
  OID_GEN_MAXIMUM_SEND_PACKETS,
  OID_802_3_PERMANENT_ADDRESS,
  OID_802_3_CURRENT_ADDRESS,
  OID_802_3_MULTICAST_LIST,
  OID_802_3_MAXIMUM_LIST_SIZE,
  OID_802_3_MAC_OPTIONS
};

#define OID_LIST_LENGTH (sizeof(OIDSupportedList) / sizeof(*OIDSupportedList))
#define ENC_BUF_SIZE    (OID_LIST_LENGTH * 4 + 32)

/*******************************************************************************
Private function definitions
*******************************************************************************/
static void response_available(USBD_HandleTypeDef *pdev);

/*********************************************
RNDIS Device library callbacks
*********************************************/
static USBD_StatusTypeDef  usbd_rndis_init                         (USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx);
static USBD_StatusTypeDef  usbd_rndis_deinit                       (USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx);
static USBD_StatusTypeDef  usbd_rndis_setup                        (USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req);
static USBD_StatusTypeDef  usbd_rndis_ep0_recv                     (USBD_HandleTypeDef *pdev);
static USBD_StatusTypeDef  usbd_rndis_data_in                      (USBD_HandleTypeDef *pdev, uint_fast8_t epnum);
static USBD_StatusTypeDef  usbd_rndis_data_out                     (USBD_HandleTypeDef *pdev, uint_fast8_t epnum);
static USBD_StatusTypeDef  usbd_rndis_sof                          (USBD_HandleTypeDef *pdev);
static USBD_StatusTypeDef  rndis_iso_in_incomplete                 (USBD_HandleTypeDef *pdev, uint_fast8_t epnum);
static USBD_StatusTypeDef  rndis_iso_out_incomplete                (USBD_HandleTypeDef *pdev, uint_fast8_t epnum);

/*******************************************************************************
Private variables
*******************************************************************************/
static USBD_HandleTypeDef *hold_pDev;

static uint8_t station_hwaddr[6] = { STATION_HWADDR };
static uint8_t permanent_hwaddr[6] = { PERMANENT_HWADDR };
static rndis_state_t rndis_state = rndis_uninitialized;
static uint32_t oid_packet_filter = 0x0000000;
static __ALIGN_BEGIN uint8_t encapsulated_buffer [ENC_BUF_SIZE] __ALIGN_END;

static uint16_t rndis_tx_data_size = 0;
static int rndis_tx_transmitting = false;
static int rndis_tx_ZLP = false;
static __ALIGN_BEGIN uint8_t usb_rx_buffer [USBD_RNDIS_OUT_BUFSIZE] __ALIGN_END;

static uint8_t *rndis_tx_ptr = NULL;
static int rndis_first_tx = 1;
static int rndis_tx_size = 0;
static int rndis_sended = 0;

static uint8_t rndis_rx_buffer [RNDIS_RX_BUFFER_SIZE];
static int rndis_received;


static int rndis_tx_start(uint8_t *data, uint16_t size)
{
	unsigned laststxended;
	static uint8_t first [USBD_RNDIS_IN_BUFSIZE];
	rndis_data_packet_t *hdr;

  //if tx buffer is already transfering or has incorrect length
  if ((rndis_tx_transmitting) || (size > ETH_MAX_PACKET_SIZE) || (size == 0))
  {
    usb_eth_stat.txbad++;
    return false;
  }

  rndis_tx_transmitting = 1;
  rndis_tx_ptr = data;
  rndis_tx_data_size = size;


  hdr = (rndis_data_packet_t *)first;
  memset(hdr, 0, RNDIS_HEADER_SIZE);
  hdr->MessageType = REMOTE_NDIS_PACKET_MSG;
  hdr->MessageLength = RNDIS_HEADER_SIZE + size;
  hdr->DataOffset = RNDIS_HEADER_SIZE - offsetof(rndis_data_packet_t, DataOffset);
  hdr->DataLength = size;

  laststxended = USBD_RNDIS_IN_BUFSIZE - RNDIS_HEADER_SIZE;
  if (laststxended > size)
    laststxended = size;
  memcpy(first + RNDIS_HEADER_SIZE, data, laststxended);
  rndis_tx_ptr += laststxended;
  rndis_tx_data_size -= laststxended;


  //http://habrahabr.ru/post/248729/
  if (hdr->MessageLength % USBD_RNDIS_IN_BUFSIZE == 0)
    rndis_tx_ZLP = 1;

  ASSERT(hold_pDev != NULL);
  //We should disable USB_OUT(EP3) IRQ, because if IRQ will happens with locked HAL (__HAL_LOCK()
  //in USBD_LL_Transmit()), the program will fail with big probability
  USBD_LL_Transmit (hold_pDev,  USBD_EP_RNDIS_IN, first, USBD_RNDIS_IN_BUFSIZE);

  //Increment error counter and then decrement in data_in if OK
  usb_eth_stat.txbad++;

  return 1;
}

/*******************************************************************************
                            /API functions
*******************************************************************************/

static USBD_StatusTypeDef usbd_rndis_init(USBD_HandleTypeDef  *pdev, uint_fast8_t cfgidx)
{

	USBD_LL_OpenEP(pdev,
				 USBD_EP_RNDIS_OUT,
				 USBD_EP_TYPE_BULK,
				 USBD_RNDIS_OUT_BUFSIZE);

	USBD_LL_OpenEP(pdev,
				 USBD_EP_RNDIS_INT,
				 USBD_EP_TYPE_INTR,
				 USBD_RNDIS_INT_SIZE);

	USBD_LL_OpenEP(pdev,
				 USBD_EP_RNDIS_IN,
				 USBD_EP_TYPE_BULK,
				 USBD_RNDIS_IN_BUFSIZE);

  USBD_LL_PrepareReceive(pdev,
                         USBD_EP_RNDIS_OUT, usb_rx_buffer, USBD_RNDIS_OUT_BUFSIZE);
  rndis_received = 0;
  hold_pDev = pdev;
  ASSERT(hold_pDev != NULL);

  return USBD_OK;
}

static USBD_StatusTypeDef  usbd_rndis_deinit(USBD_HandleTypeDef  *pdev, uint_fast8_t cfgidx)
{
	USBD_LL_CloseEP(pdev, USBD_EP_RNDIS_INT);
	USBD_LL_CloseEP(pdev, USBD_EP_RNDIS_IN);
	USBD_LL_CloseEP(pdev, USBD_EP_RNDIS_OUT);
	return USBD_OK;
}

static USBD_StatusTypeDef usbd_rndis_setup(USBD_HandleTypeDef  *pdev, const USBD_SetupReqTypedef *req)
{
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
	if (interfacev != INTERFACE_RNDIS_CONTROL && interfacev != INTERFACE_RNDIS_DATA)
		   return USBD_OK;
 switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :
    if (req->wLength != 0) // is data setup packet?
    {
      /* Check if the request is Device-to-Host */
      if (req->bmRequest & 0x80)
      {
        USBD_CtlSendData (pdev,
                          encapsulated_buffer,
                          ((rndis_generic_msg_t *)encapsulated_buffer)->MessageLength);
      }
      else /* Host-to-Device requeset */
      {
        USBD_CtlPrepareRx (pdev,
                           encapsulated_buffer,
                           req->wLength);
      }
    }
    return USBD_OK;

  default:
    return USBD_OK;
  }
}

#define INFBUF ((uint32_t *)(encapsulated_buffer + sizeof(rndis_query_cmplt_t)))

static void rndis_query_cmplt32(USBD_HandleTypeDef  *pdev, int status, uint32_t data)
{
  rndis_query_cmplt_t *c;
  c = (rndis_query_cmplt_t *)encapsulated_buffer;
  c->MessageType = REMOTE_NDIS_QUERY_CMPLT;
  c->MessageLength = sizeof(rndis_query_cmplt_t) + 4;
  c->InformationBufferLength = 4;
  c->InformationBufferOffset = 16;
  c->Status = status;
  *(uint32_t *)(c + 1) = data;
  response_available(pdev);
}

static void rndis_query_cmplt(USBD_HandleTypeDef  *pdev, int status, const void *data, int size)
{
  rndis_query_cmplt_t *c;
  c = (rndis_query_cmplt_t *)encapsulated_buffer;
  c->MessageType = REMOTE_NDIS_QUERY_CMPLT;
  c->MessageLength = sizeof(rndis_query_cmplt_t) + size;
  c->InformationBufferLength = size;
  c->InformationBufferOffset = 16;
  c->Status = status;
  memcpy(c + 1, data, size);
  response_available(pdev);
}

#define MAC_OPT NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA | \
NDIS_MAC_OPTION_RECEIVE_SERIALIZED  | \
  NDIS_MAC_OPTION_TRANSFERS_NOT_PEND  | \
    NDIS_MAC_OPTION_NO_LOOPBACK

static const char * const rndis_vendor = RNDIS_VENDOR;

static void rndis_query(USBD_HandleTypeDef  *pdev)
{
  switch (((rndis_query_msg_t *)encapsulated_buffer)->Oid)
  {
  case OID_GEN_SUPPORTED_LIST:         rndis_query_cmplt(pdev, RNDIS_STATUS_SUCCESS, OIDSupportedList, 4 * OID_LIST_LENGTH); return;
  case OID_GEN_VENDOR_DRIVER_VERSION:  rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, 0x00001000);  return;
  case OID_802_3_CURRENT_ADDRESS:      rndis_query_cmplt(pdev, RNDIS_STATUS_SUCCESS, &station_hwaddr, 6); return;
  case OID_802_3_PERMANENT_ADDRESS:    rndis_query_cmplt(pdev, RNDIS_STATUS_SUCCESS, &permanent_hwaddr, 6); return;
  case OID_GEN_MEDIA_SUPPORTED:        rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, NDIS_MEDIUM_802_3); return;
  case OID_GEN_MEDIA_IN_USE:           rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, NDIS_MEDIUM_802_3); return;
  case OID_GEN_PHYSICAL_MEDIUM:        rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, NDIS_MEDIUM_802_3); return;
  case OID_GEN_HARDWARE_STATUS:        rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, 0); return;
  case OID_GEN_LINK_SPEED:             rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, ETH_LINK_SPEED / 100); return;
  case OID_GEN_VENDOR_ID:              rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, 0x00FFFFFF); return;
  case OID_GEN_VENDOR_DESCRIPTION:     rndis_query_cmplt(pdev, RNDIS_STATUS_SUCCESS, rndis_vendor, strlen(rndis_vendor) + 1); return;
  case OID_GEN_CURRENT_PACKET_FILTER:  rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, oid_packet_filter); return;
  case OID_GEN_MAXIMUM_FRAME_SIZE:     rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, ETH_MAX_PACKET_SIZE - ETH_HEADER_SIZE); return;
  case OID_GEN_MAXIMUM_TOTAL_SIZE:     rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, ETH_MAX_PACKET_SIZE); return;
  case OID_GEN_TRANSMIT_BLOCK_SIZE:    rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, ETH_MAX_PACKET_SIZE); return;
  case OID_GEN_RECEIVE_BLOCK_SIZE:     rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, ETH_MAX_PACKET_SIZE); return;
  case OID_GEN_MEDIA_CONNECT_STATUS:   rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, NDIS_MEDIA_STATE_CONNECTED); return;
  //	case OID_GEN_CURRENT_LOOKAHEAD:      rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, RNDIS_RX_BUFFER_SIZE); return;
  case OID_GEN_RNDIS_CONFIG_PARAMETER: rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, 0); return;
  case OID_802_3_MAXIMUM_LIST_SIZE:    rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, 1); return;
  case OID_802_3_MULTICAST_LIST:       rndis_query_cmplt32(pdev, RNDIS_STATUS_NOT_SUPPORTED, 0); return;
  case OID_802_3_MAC_OPTIONS:          rndis_query_cmplt32(pdev, RNDIS_STATUS_NOT_SUPPORTED, 0); return;
  case OID_GEN_MAC_OPTIONS:            rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, /*MAC_OPT*/ 0); return;
  case OID_802_3_RCV_ERROR_ALIGNMENT:  rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, 0); return;
  case OID_802_3_XMIT_ONE_COLLISION:   rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, 0); return;
  case OID_802_3_XMIT_MORE_COLLISIONS: rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, 0); return;
  case OID_GEN_XMIT_OK:                rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, usb_eth_stat.txok); return;
  case OID_GEN_RCV_OK:                 rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, usb_eth_stat.rxok); return;
  case OID_GEN_RCV_ERROR:              rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, usb_eth_stat.rxbad); return;
  case OID_GEN_XMIT_ERROR:             rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, usb_eth_stat.txbad); return;
  case OID_GEN_RCV_NO_BUFFER:          rndis_query_cmplt32(pdev, RNDIS_STATUS_SUCCESS, 0); return;
  default:                             rndis_query_cmplt(pdev, RNDIS_STATUS_FAILURE, NULL, 0); return;
  }
}

#undef INFBUF
#define INFBUF ((uint32_t *)((uint8_t *)&(m->RequestId) + m->InformationBufferOffset))
#define CFGBUF ((rndis_config_parameter_t *) INFBUF)
#define PARMNAME  ((uint8_t *)CFGBUF + CFGBUF->ParameterNameOffset)
#define PARMVALUE ((uint8_t *)CFGBUF + CFGBUF->ParameterValueOffset)
#define PARMVALUELENGTH	CFGBUF->ParameterValueLength
#define PARM_NAME_LENGTH 25 /* Maximum parameter name length */

static void rndis_handle_config_parm(const char *data, int keyoffset, int valoffset, int keylen, int vallen)
{
  //	if (strncmp(parmname, "rawmode", 7) == 0)
  //	{
  //		if (parmvalue[0] == '0')
  //		{
  //			usbstick_mode.raw = 0;
  //		}
  //		else
  //		{
  //			usbstick_mode.raw = 1;
  //		}
  //	}
}

static void rndis_packetFilter(uint32_t newfilter)
{
  if (newfilter & NDIS_PACKET_TYPE_PROMISCUOUS)
  {
    //		USB_ETH_HOOK_SET_PROMISCIOUS_MODE(1);
  }
  else
  {
    //		USB_ETH_HOOK_SET_PROMISCIOUS_MODE(false);
  }
}

static void rndis_handle_set_msg(void  *pdev)
{
	rndis_set_cmplt_t *c;
	rndis_set_msg_t *m;
	rndis_Oid_t oid;

	c = (rndis_set_cmplt_t *)encapsulated_buffer;
	m = (rndis_set_msg_t *)encapsulated_buffer;

	/* Never have longer parameter names than PARM_NAME_LENGTH */
	/*
	char parmname[PARM_NAME_LENGTH+1];
	uint8_t i;
	int8_t parmlength;
	*/

	/* The parameter name seems to be transmitted in uint16_t, but */
	/* we want this in uint8_t. Hence have to throw out some info... */

	/*
	if (CFGBUF->ParameterNameLength > (PARM_NAME_LENGTH*2))
	{
		parmlength = PARM_NAME_LENGTH * 2;
	}
	else
	{
		parmlength = CFGBUF->ParameterNameLength;
	}
	i = 0;
	while (parmlength > 0)
	{
		// Convert from uint16_t to char array.
		parmname[i] = (char)*(PARMNAME + 2*i); // FSE! FIX IT!
		parmlength -= 2;
		i++;
	}
	*/

	oid = m->Oid;
	c->MessageType = REMOTE_NDIS_SET_CMPLT;
	c->MessageLength = sizeof(rndis_set_cmplt_t);
	c->Status = RNDIS_STATUS_SUCCESS;

	switch (oid)
	{
		/* Parameters set up in 'Advanced' tab */
		case OID_GEN_RNDIS_CONFIG_PARAMETER:
			{
                rndis_config_parameter_t *p;
				char *ptr = (char *)m;
				ptr += sizeof(rndis_generic_msg_t);
				ptr += m->InformationBufferOffset;
				p = (rndis_config_parameter_t *)ptr;
				rndis_handle_config_parm(ptr, p->ParameterNameOffset, p->ParameterValueOffset, p->ParameterNameLength, p->ParameterValueLength);
			}
			break;

		/* Mandatory general OIDs */
		case OID_GEN_CURRENT_PACKET_FILTER:
			oid_packet_filter = *INFBUF;
			if (oid_packet_filter)
			{
				rndis_packetFilter(oid_packet_filter);
				rndis_state = rndis_data_initialized;
			}
			else
			{
				rndis_state = rndis_initialized;
			}
			break;

		case OID_GEN_CURRENT_LOOKAHEAD:
			break;

		case OID_GEN_PROTOCOL_OPTIONS:
			break;

		/* Mandatory 802_3 OIDs */
		case OID_802_3_MULTICAST_LIST:
			break;

		/* Power Managment: fails for now */
		case OID_PNP_ADD_WAKE_UP_PATTERN:
		case OID_PNP_REMOVE_WAKE_UP_PATTERN:
		case OID_PNP_ENABLE_WAKE_UP:
		default:
			c->Status = RNDIS_STATUS_FAILURE;
			break;
	}

	response_available(pdev);

	return;
}

static int laststxended = 0;

static USBD_StatusTypeDef usbd_cdc_transfer(void *pdev)
{
	hold_pDev = pdev;
	  ASSERT(hold_pDev != NULL);
	if (laststxended != 0 || rndis_tx_ptr == NULL || rndis_tx_size <= 0)
		return USBD_OK;
	if (rndis_first_tx)
	{
		static uint8_t first [USBD_RNDIS_IN_BUFSIZE];
		rndis_data_packet_t *hdr;

		hdr = (rndis_data_packet_t *)first;
		memset(hdr, 0, sizeof(rndis_data_packet_t));
		hdr->MessageType = REMOTE_NDIS_PACKET_MSG;
		hdr->MessageLength = sizeof(rndis_data_packet_t) + rndis_tx_size;
		hdr->DataOffset = sizeof(rndis_data_packet_t) - offsetof(rndis_data_packet_t, DataOffset);
		hdr->DataLength = rndis_tx_size;

		laststxended = USBD_RNDIS_IN_BUFSIZE - sizeof(rndis_data_packet_t);
		if (laststxended > rndis_tx_size) laststxended = rndis_tx_size;
		memcpy(first + sizeof(rndis_data_packet_t), rndis_tx_ptr, laststxended);

		  ASSERT(hold_pDev != NULL);
		USBD_LL_Transmit(hold_pDev,
						USBD_EP_RNDIS_IN,
						(uint8_t *)&first,
						sizeof(rndis_data_packet_t) + laststxended);
	}
	else
	{
		int n = rndis_tx_size;
		if (n > USBD_RNDIS_IN_BUFSIZE)
			n = USBD_RNDIS_IN_BUFSIZE;

		USBD_LL_Transmit(hold_pDev,
						USBD_EP_RNDIS_IN,
						rndis_tx_ptr,
						n);
		laststxended = n;
	}
	return USBD_OK;
}

// Control Channel
//	https://msdn.microsoft.com/en-us/library/windows/hardware/ff546124(v=vs.85).aspx
//	https://docs.microsoft.com/en-us/windows-hardware/drivers/network/data-channel-characteristics
static USBD_StatusTypeDef usbd_rndis_ep0_recv(USBD_HandleTypeDef  *pdev)
{
	const USBD_SetupReqTypedef * const req = & pdev->request;
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
	if (interfacev != INTERFACE_RNDIS_CONTROL && interfacev != INTERFACE_RNDIS_DATA)
		   return USBD_OK;

  switch (((rndis_generic_msg_t *)encapsulated_buffer)->MessageType)
  {
  case REMOTE_NDIS_INITIALIZE_MSG:
    {
      rndis_initialize_cmplt_t *m;
      m = ((rndis_initialize_cmplt_t *)encapsulated_buffer);
      //m->MessageID is same as before
      m->MessageType = REMOTE_NDIS_INITIALIZE_CMPLT;
      m->MessageLength = sizeof(rndis_initialize_cmplt_t);
      m->MajorVersion = RNDIS_MAJOR_VERSION;
      m->MinorVersion = RNDIS_MINOR_VERSION;
      m->Status = RNDIS_STATUS_SUCCESS;
      m->DeviceFlags = RNDIS_DF_CONNECTIONLESS;
      m->Medium = RNDIS_MEDIUM_802_3;
      m->MaxPacketsPerTransfer = 1;
      m->MaxTransferSize = RNDIS_RX_BUFFER_SIZE;
      m->PacketAlignmentFactor = 0;
      m->AfListOffset = 0;
      m->AfListSize = 0;
      rndis_state = rndis_initialized;
      response_available(pdev);
    }
    break;

  case REMOTE_NDIS_QUERY_MSG:
    rndis_query(pdev);
    break;

  case REMOTE_NDIS_SET_MSG:
    rndis_handle_set_msg(pdev);
    break;

  case REMOTE_NDIS_RESET_MSG:
    {
      rndis_reset_cmplt_t * m;
      m = ((rndis_reset_cmplt_t *)encapsulated_buffer);
      rndis_state = rndis_uninitialized;
      m->MessageType = REMOTE_NDIS_RESET_CMPLT;
      m->MessageLength = sizeof(rndis_reset_cmplt_t);
      m->Status = RNDIS_STATUS_SUCCESS;
      m->AddressingReset = 1; /* Make it look like we did something */
      //	m->AddressingReset = 0; //Windows halts if set to 1 for some reason
      response_available(pdev);
    }
    break;

  case REMOTE_NDIS_KEEPALIVE_MSG:
    {
      rndis_keepalive_cmplt_t * m;
      m = (rndis_keepalive_cmplt_t *)encapsulated_buffer;
      m->MessageType = REMOTE_NDIS_KEEPALIVE_CMPLT;
      m->MessageLength = sizeof(rndis_keepalive_cmplt_t);
      m->Status = RNDIS_STATUS_SUCCESS;
    }
    // We have data to send back
    response_available(pdev);
    break;

  default:
    break;
  }
  return USBD_OK;
}

// Data Channel         https://msdn.microsoft.com/en-us/library/windows/hardware/ff546305(v=vs.85).aspx
//                      https://msdn.microsoft.com/en-us/library/windows/hardware/ff570635(v=vs.85).aspx (prev version)
//	https://docs.microsoft.com/en-us/windows-hardware/drivers/network/data-channel-characteristics
static USBD_StatusTypeDef usbd_rndis_data_in(USBD_HandleTypeDef*pdev, uint_fast8_t epnum)
{
	epnum &= 0x0F;
	if (epnum == (USBD_EP_RNDIS_IN & 0x0F))
	{
		rndis_first_tx = false;
		rndis_sended += laststxended;
		rndis_tx_size -= laststxended;
		rndis_tx_ptr += laststxended;
		laststxended = 0;
		usbd_cdc_transfer(pdev);

	}
	return USBD_OK;
}

static void handle_rxpacket(const uint8_t *data, int size)
{
	rndis_data_packet_t *p;
	p = (rndis_data_packet_t *)data;
	if (size < sizeof(rndis_data_packet_t)) return;
	if (p->MessageType != REMOTE_NDIS_PACKET_MSG || p->MessageLength != size) return;
	if (p->DataOffset + offsetof(rndis_data_packet_t, DataOffset) + p->DataLength != size)
	{
		usb_eth_stat.rxbad++;
		return;
	}
	usb_eth_stat.rxok++;
	if (rndis_rxproc != NULL)
		rndis_rxproc(&rndis_rx_buffer[p->DataOffset + offsetof(rndis_data_packet_t, DataOffset)], p->DataLength);
	/*
  if (size < RNDIS_HEADER_SIZE)
  {
    usb_eth_stat.rxbad++;
    return;
  }
  //To exclude Rx ZLP bug
  if ((pheader->MessageType != REMOTE_NDIS_PACKET_MSG) ||
      ((pheader->MessageLength != size) && (pheader->MessageLength != size - 1)))
  {
    usb_eth_stat.rxbad++;
    return;
  }
  size = pheader->MessageLength;
  if (pheader->DataOffset + offsetof(rndis_data_packet_t, DataOffset) + pheader->DataLength != size)
  {
    usb_eth_stat.rxbad++;
    return;
  }
  if (!rndis_rx_started)
  {
    usb_eth_stat.rxbad++;
    return;
  }
  rndis_rx_data_size = pheader->DataLength;
  rndis_rx_started = false;

  usb_eth_stat.rxok++;
  rndis_rx_ready_cb();
  */
}


// Data Channel
//	https://msdn.microsoft.com/en-us/library/windows/hardware/ff546305(v=vs.85).aspx
//	https://docs.microsoft.com/en-us/windows-hardware/drivers/network/data-channel-characteristics
static USBD_StatusTypeDef usbd_rndis_data_out(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	if (epnum == USBD_EP_RNDIS_OUT)
	{
		const unsigned xfercount = USBD_LL_GetRxDataSize(pdev, epnum);

		PCD_EPTypeDef *ep = &(((PCD_HandleTypeDef*)(pdev->pData))->OUT_ep[epnum]);
		if (rndis_received + xfercount > RNDIS_RX_BUFFER_SIZE)
		{
			usb_eth_stat.rxbad++;
			rndis_received = 0;
		}
		else
		{
			if ((rndis_received + xfercount) <= RNDIS_RX_BUFFER_SIZE)
			{
				memcpy(& rndis_rx_buffer [rndis_received], usb_rx_buffer, xfercount);
				rndis_received += xfercount;
				if (xfercount != USBD_RNDIS_OUT_BUFSIZE)
				{
					handle_rxpacket(rndis_rx_buffer, rndis_received);
					rndis_received = 0;
				}
			}
			else
			{
					rndis_received = 0;
					usb_eth_stat.rxbad++;
			}
		}
		USBD_LL_PrepareReceive(hold_pDev, USBD_EP_RNDIS_OUT, usb_rx_buffer, USBD_RNDIS_OUT_BUFSIZE);
		  //	DCD_EP_PrepareRx(pdev, USBD_EP_RNDIS_OUT, usb_rx_buffer, USBD_RNDIS_OUT_BUFSIZE);
	}
  //  PCD_EPTypeDef *ep = &((PCD_HandleTypeDef*)pdev->pData)->OUT_ep[epnum];
   // handle_rxpacket((rndis_data_packet_t*)rndis_rx_buffer, RNDIS_RX_BUFFER_SIZE - ep->xfer_len - USBD_RNDIS_OUT_BUFSIZE + xfercount);
  return USBD_OK;
}


static void response_available(USBD_HandleTypeDef *pdev)
{
	static __ALIGN_BEGIN uint8_t sendState [8] __ALIGN_END;
	uint_fast32_t code = 0x01;
	uint_fast32_t reserved0 = 0x09;

	ASSERT(USBD_RNDIS_INT_SIZE == sizeof sendState);
	sendState [0] = LO_BYTE(code);
	sendState [1] = HI_BYTE(code);
	sendState [2] = HI_24BY(code);
	sendState [3] = HI_32BY(code);
	sendState [4] = LO_BYTE(reserved0);
	sendState [5] = HI_BYTE(reserved0);
	sendState [6] = HI_24BY(reserved0);
	sendState [7] = HI_32BY(reserved0);

	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	USBD_LL_Transmit (pdev, USBD_EP_RNDIS_INT, sendState, USBD_RNDIS_INT_SIZE);
	LowerIrql(oldIrql);
}


static int rndis_can_send(void)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	int f = rndis_tx_size <= 0 && hold_pDev != NULL;
	LowerIrql(oldIrql);
	return f;
}

static void rndis_send(const void *data, int size)
{
	ASSERT(data != NULL);
	ASSERT(hold_pDev != NULL);
	//rndis_tx_start(data, size);
/*
	while (transmit_ok == 1)
	transmit_ok = 1;
	USBD_LL_Transmit (pDev,
				      USBD_EP_RNDIS_IN,
	                  (uint8_t *)data,
					  size);
*/

	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);

	if (size <= 0 ||
		size > ETH_MAX_PACKET_SIZE ||
		rndis_tx_size > 0 || hold_pDev == NULL)

		{
		LowerIrql(oldIrql);
			return;
		}


	rndis_first_tx = 1;
	rndis_tx_ptr = (uint8_t *)data;
	rndis_tx_size = size;
	rndis_sended = 0;


	ASSERT(hold_pDev != NULL);
	usbd_cdc_transfer(hold_pDev);
	LowerIrql(oldIrql);
}

const USBD_ClassTypeDef USBD_CLASS_RNDIS =
{
	USBD_RNDIS_ColdInit,
	usbd_rndis_init,
	usbd_rndis_deinit,
	usbd_rndis_setup,
	NULL,
	usbd_rndis_ep0_recv,
	usbd_rndis_data_in,
	usbd_rndis_data_out,
	NULL, /*SOF */
	NULL, /*ISOIn*/
	NULL, /*ISOOut*/
};


#endif /* WITHUSBHW && WITHUSBRNDIS */

