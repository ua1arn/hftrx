/*
 * usbd_cdc.c
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
*/

#include "hardware.h"

#if WITHUSBHW && WITHUSBCDCEEM

#include "formats.h"
#include "usb_core.h"

#if WITHLWIP

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "netif/etharp.h"


#define RNDIS_MTU 1500  // MTU value


typedef void (*cdcdeem_rxproc_t)(const uint8_t *data, int size);

static cdcdeem_rxproc_t cdceem_rxproc = NULL;

//static struct netif test_netif1, test_netif2;

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
static err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
{
//    int i;
//    struct pbuf *q;
//    static char data[RNDIS_MTU + 14 + 4];
//    int size = 0;
//    for (i = 0; i < 200; i++)
//    {
//        if (cdceem_can_send()) break;
//        local_delay_ms(1);
//    }
//    for(q = p; q != NULL; q = q->next)
//    {
//        if (size + q->len > RNDIS_MTU + 14)
//            return ERR_ARG;
//        memcpy(data + size, (char *)q->payload, q->len);
//        size += q->len;
//    }
//    if (!cdceem_can_send())
//        return ERR_MEM;

//    cdceem_send(data, size);

    return ERR_OK;
}


static struct netif cdceem_netif_data;



struct netif  * getNetifData(void)
{
	return &cdceem_netif_data;
}


static err_t cdceem_output_fn(struct netif *netif, struct pbuf *p, ip_addr_t *ipaddr)
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
	//PRINTF("cdc eem netif_init_cb\n");
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
	netif->output = etharp_output; //cdceem_output_fn;	// если бы не требовалось добавлять ethernet заголовки, передачва делалась бы тут.
												// и слкдующий callback linkoutput не требовался бы вообще
	netif->linkoutput = linkoutput_fn;	// используется внутри etharp_output
	return ERR_OK;
}


typedef struct cdceembuf_tag
{
	LIST_ENTRY item;
	struct pbuf *frame;
} ALIGNX_END cdceembuf_t;


static LIST_ENTRY cdceem_free;
static LIST_ENTRY cdceem_ready;

static void cdceem_buffers_initialize(void)
{
	static RAMFRAMEBUFF cdceembuf_t sliparray [64];
	unsigned i;

	InitializeListHead(& cdceem_free);	// Незаполненные
	InitializeListHead(& cdceem_ready);	// Для обработки

	for (i = 0; i < (sizeof sliparray / sizeof sliparray [0]); ++ i)
	{
		cdceembuf_t * const p = & sliparray [i];
		InsertHeadList(& cdceem_free, & p->item);
	}
}

static int cdceem_buffers_alloc(cdceembuf_t * * tp)
{
	if (! IsListEmpty(& cdceem_free))
	{
		const PLIST_ENTRY t = RemoveTailList(& cdceem_free);
		cdceembuf_t * const p = CONTAINING_RECORD(t, cdceembuf_t, item);
		* tp = p;
		return 1;
	}
	if (! IsListEmpty(& cdceem_ready))
	{
		const PLIST_ENTRY t = RemoveTailList(& cdceem_ready);
		cdceembuf_t * const p = CONTAINING_RECORD(t, cdceembuf_t, item);
		* tp = p;
		return 1;
	}
	return 0;
}

static int cdceem_buffers_ready_user(cdceembuf_t * * tp)
{
	system_disableIRQ();
	if (! IsListEmpty(& cdceem_ready))
	{
		const PLIST_ENTRY t = RemoveTailList(& cdceem_ready);
		system_enableIRQ();
		cdceembuf_t * const p = CONTAINING_RECORD(t, cdceembuf_t, item);
		* tp = p;
		return 1;
	}
	system_enableIRQ();
	return 0;
}


static void cdceem_buffers_release(cdceembuf_t * p)
{
	InsertHeadList(& cdceem_free, & p->item);
}

static void cdceem_buffers_release_user(cdceembuf_t * p)
{
	system_disableIRQ();
	cdceem_buffers_release(p);
	system_enableIRQ();
}

// сохранить принятый
static void cdceem_buffers_rx(cdceembuf_t * p)
{
	InsertHeadList(& cdceem_ready, & p->item);
}

static void on_packet(const uint8_t *data, int size)
{
	cdceembuf_t * p;
	if (cdceem_buffers_alloc(& p) != 0)
	{
		struct pbuf *frame;
		frame = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
		if (frame == NULL)
		{
			cdceem_buffers_release(p);
			return;
		}
		memcpy(frame->payload, data, size);
		frame->len = size;

		p->frame = frame;
		cdceem_buffers_rx(p);
	}
}



// Receiving Ethernet packets
// user-mode function
void usb_polling(void)
{
	cdceembuf_t * p;
	if (cdceem_buffers_ready_user(& p) != 0)
	{
		err_t e = ethernet_input(p->frame, & cdceem_netif_data);
		if (e != ERR_OK)
		{
			  /* This means the pbuf is freed or consumed,
			     so the caller doesn't have to free it again */
		}

		cdceem_buffers_release_user(p);
	}
}


void init_netif(void)
{
	cdceem_buffers_initialize();

	static const  uint8_t hwaddrv [6]  = { HWADDR };

	static ip_addr_t netmask;// [4] = NETMASK;
	static ip_addr_t gateway;// [4] = GATEWAY;

	IP4_ADDR(& netmask, myNETMASK [0], myNETMASK [1], myNETMASK [2], myNETMASK [3]);
	IP4_ADDR(& gateway, myGATEWAY [0], myGATEWAY [1], myGATEWAY [2], myGATEWAY [3]);

	static ip_addr_t vaddr;// [4]  = IPADDR;
	IP4_ADDR(& vaddr, myIP [0], myIP [1], myIP [2], myIP [3]);

	struct netif  *netif = & cdceem_netif_data;
	netif->hwaddr_len = 6;
	memcpy(netif->hwaddr, hwaddrv, 6);

	netif = netif_add(netif, & vaddr, & netmask, & gateway, NULL, netif_init_cb, ip_input);
	netif_set_default(netif);

	while (!netif_is_up(netif))
		;

	cdceem_rxproc = on_packet;		// разрешаем принимать пакеты даптеру и отправлять в LWIP
}

#endif /* WITHLWIP */

// CDC class-specific request codes
// (usbcdc11.pdf, 6.2, Table 46)
// see Table 45 for info about the specific requests.
#define CDC_SET_CONTROL_LINE_STATE              0x22

static USBALIGN_BEGIN uint8_t cdceem1buffout [USBD_CDCEEM_BUFSIZE] USBALIGN_END;
static USBALIGN_BEGIN uint8_t cdceem1buffin [USBD_CDCEEM_BUFSIZE] USBALIGN_END;
static RAMDTCM uint_fast16_t cdceem1buffinlevel;
static USBALIGN_BEGIN uint8_t cdceem_epXdatabuffout [USB_OTG_MAX_EP0_SIZE] USBALIGN_END;


static uint_fast32_t ulmin32(uint_fast32_t a, uint_fast32_t b)
{
	return a < b ? a : b;
}

static uint_fast32_t ulmax32(uint_fast32_t a, uint_fast32_t b)
{
	return a > b ? a : b;
}

static uint_fast16_t ulmin16(uint_fast16_t a, uint_fast16_t b)
{
	return a < b ? a : b;
}

static uint_fast16_t ulmax16(uint_fast16_t a, uint_fast16_t b)
{
	return a > b ? a : b;
}


/* Собираем поток из CDC EEM USB пакетов */
// construct EEM packets
enum
{
	CDCEEMOUT_COMMAND,
	CDCEEMOUT_DATA,
	CDCEEMOUT_CRC,
	//
	CDCEEMOUT_nstates
};

static uint_fast8_t cdceemoutstate = CDCEEMOUT_COMMAND;
static uint_fast32_t cdceemoutscore = 0;
static uint_fast32_t cdceemoutacc;
static uint_fast32_t cdceemlength;
static uint_fast32_t cdceematcrc;
static uint_fast32_t cdceemnpackets;

// see SIZEOF_ETHARP_PACKET
static uint8_t cdceembuff [1514];

static void cdceemout_initialize(void)
{
	cdceemoutstate = CDCEEMOUT_COMMAND;
	cdceemoutscore = 0;
}

static void cdceemout_buffer_print(
	const uint8_t * data,
	uint_fast16_t length
	)
{
	// debug print
	uint_fast16_t pos;
	for (pos = 0; pos < length; ++ pos)
	{
		PRINTF(PSTR("%02X%s"), data [pos], ((pos + 1) % 16) == 0 ? ",\n" : ", ");
	}
	PRINTF(PSTR("\n"));
}

// Ethernet II frame print (64..1518 bytes)
static void cdceemout_buffer_print2(
	const uint8_t * data,
	uint_fast16_t length
	)
{
	// +++ MAC header
	PRINTF(PSTR("Dst MAC = %02X:%02X:%02X:%02X:%02X:%02X, "), data [0], data [1], data [2], data [3], data [4], data [5]);
	PRINTF(PSTR("Src MAC = %02X:%02X:%02X:%02X:%02X:%02X, "), data [6], data [7], data [8], data [9], data [10], data [11]);
	const uint_fast16_t EtherType = data [12] * 256 + data [13];	// 0x0800
	PRINTF(PSTR("EtherType %04X\n"), EtherType);
	// --- MAC header
	data += 14;
	length = length >= 14 ? length - 14 : 0;

	switch (EtherType)
	{
	case 0x0800:
		// IPv4 datagram
		PRINTF(PSTR("IPv4 datagram: "));
		PRINTF(PSTR("Protocol=%02X, "), data [9]);	// 0x11 == UDP,
		PRINTF(PSTR("Src IP=%d.%d.%d.%d, "), data [12], data [13], data [14], data [15]);
		PRINTF(PSTR("Dst IP=%d.%d.%d.%d\n"), data [16], data [17], data [18], data [19]);
		{
		}
		break;

	case 0x86DD:
		// IPv6 datagram
		//PRINTF(PSTR("IPv6 datagram\n"));
		break;

	case 0x0806:
		// ARP frame
		//PRINTF(PSTR("ARP frame\n"));
		break;

	case 0x88CC:
		// IEEE Std 802.1AB - Link Layer Discovery Protocol (LLDP)s
		//PRINTF(PSTR("LLDP\n"));
		break;

	default:
		{
			// pyloaddebug print
			uint_fast16_t pos;
			for (pos = 0; pos < length; ++ pos)
			{
				PRINTF(PSTR("%02X%s"), data [pos], ((pos + 1) % 16) == 0 ? ",\n" : ", ");
			}
			PRINTF(PSTR("\n"));
		}
		break;
	}


}

static USBALIGN_BEGIN uint8_t dbd [2] USBALIGN_END;

// called in context of interrupt
static void cdceemout_buffer_save(
	const uint8_t * data,
	uint_fast16_t length
	)
{
	uint_fast16_t pos;
	PRINTF("cdceemout_buffer_save: EP len=%u\n", length);
	for (pos = 0; pos < length; )
	{
		switch (cdceemoutstate)
		{
		case CDCEEMOUT_COMMAND:
			cdceemoutacc = cdceemoutacc * 256 + data [pos ++];
			if (++ cdceemoutscore >= 2)
			{
				const uint_fast8_t w = (cdceemoutacc & 0xFFFF);
				cdceemoutscore = 0;
				// разбор команды
				const uint_fast8_t bmType = (w >> 15) & 0x0001;	// 0: EEM data payload 1: EEM Command
				if (bmType == 0)
				{
					const uint_fast8_t bmCRC = (w >> 14) & 0x0001;	// 0: Ethernet Frame CRC is set to 0xdeadbeef
					cdceemlength = (w >> 0) & 0x3FFF;					// размер включая 4 байта СКС
					if (cdceemlength > 0)
					{
						if (cdceemlength >= 4)
						{
							cdceematcrc = cdceemlength - 4;
							cdceemoutstate = CDCEEMOUT_DATA;
						}
					}
					//PRINTF(PSTR("Data: bmCRC=%02X, cdceemlength=%u, pyload=0x%04X\n"), bmCRC, cdceemlength, cdceematcrc);
				}
				else
				{
					const uint_fast8_t bmEEMCmd = (w >> 11) & 0x0007;	// 0: Ethernet Frame CRC is set to 0xdeadbeef
					const uint_fast16_t bmEEMCmdParam = (w >> 11) & 0x07FF;
					PRINTF(PSTR("Command: bmEEMCmd=%02X, bmEEMCmdParam=%u\n"), bmEEMCmd, bmEEMCmdParam);
				}
			}
			break;

		case CDCEEMOUT_DATA:
			{
				const uint_fast16_t chunk = ulmin16(length - pos, cdceematcrc - cdceemoutscore);
				memcpy(cdceembuff + cdceemoutscore, data + pos, ulmin16(sizeof cdceembuff - cdceemoutscore, chunk)); // use data bytes
				pos += chunk;
				if ((cdceemoutscore += chunk) >= cdceematcrc)
				{
					cdceemoutscore = 0;
					cdceemoutstate = CDCEEMOUT_CRC;
				}
			}
			break;

		case CDCEEMOUT_CRC:
			cdceemoutacc = cdceemoutacc * 256 + data [pos ++];
			if (++ cdceemoutscore >= 4)
			{
				cdceemoutscore = 0;
				cdceemoutstate = CDCEEMOUT_COMMAND;

				++ cdceemnpackets;
				//PRINTF(PSTR("CDCEEMOUT packets=%lu\n"), (unsigned long) cdceemnpackets);

				//PRINTF(PSTR("crc=%08lX\n"), (cdceemoutacc & 0xFFFFFFFF));
				// Тут полностью собран ethernet пакет, используем его (или например печатаем содержимое).
				PRINTF("cdceemout_buffer_save: LWIP len=%u\n", cdceematcrc);
#if WITHLWIP
				// Save to LWIP
				{
					if (cdceem_rxproc != NULL)
						cdceem_rxproc(cdceembuff, cdceematcrc);

				}

#elif 0
				// Отладочная печать
				PRINTF(PSTR("Data pyload length=0x%04X\n"), cdceematcrc);
				//cdceemout_buffer_print(cdceembuff, cdceematcrc);
				cdceemout_buffer_print2(cdceembuff, cdceematcrc);
#endif
			}
		}

	}

}

/*
* @param  epnum: endpoint index without direction bit
*
 */
static USBD_StatusTypeDef USBD_CDCEEM_DataIn(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	//PRINTF("USBD_CDCEEM_DataIn: epnum=%d\n", (int) epnum);
	switch (epnum)
	{
	case (USBD_EP_CDCEEM_IN & 0x7F):
		//USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), cdceem1buffin, cdceem1buffinlevel);
		//cdceem1buffinlevel = 0;
		USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), dbd, sizeof dbd);
		break;
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDCEEM_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	switch (epnum)
	{
	case USBD_EP_CDCEEM_OUT:
		/* EEM EP OUT */
		// use CDC EEM data
		cdceemout_buffer_save(cdceem1buffout, USBD_LL_GetRxDataSize(pdev, epnum));	/* использование буфера принятых данных */
		//memcpy(cdc1buffin, cdc1buffout, cdc1buffinlevel = USBD_LL_GetRxDataSize(pdev, epnum));
		/* Prepare Out endpoint to receive next cdc data packet */
		USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(epnum), cdceem1buffout, USBD_CDCEEM_BUFSIZE);
		break;
	default:
		break;
	}
	return USBD_OK;
}

// При возврате из этой функции в usbd_core.c происходит вызов USBD_CtlSendStatus
static USBD_StatusTypeDef USBD_CDCEEM_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	const USBD_SetupReqTypedef * const req = & pdev->request;

	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	//PRINTF(PSTR("1 USBD_CDC_EP0_RxReady: interfacev=%u: bRequest=%u, wLength=%u\n"), interfacev, req->bRequest, req->wLength);
	switch (interfacev)
	{
	case INTERFACE_CDCEEM_DATA:	// CDC EEM interface
		{
			switch (req->bRequest)
			{
//			case CDC_SET_LINE_CODING:
//				{
////					const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
////					ASSERT(req->wLength == 7);
////					dwDTERate [interfacev] = USBD_peek_u32(& cdceem_epXdatabuffout [0]);
//					//PRINTF(PSTR("USBD_CDCEEM_EP0_RxReady: CDC_SET_LINE_CODING: interfacev=%u, dwDTERate=%lu, bits=%u\n"), interfacev, dwDTERate [interfacev], cdceem_epXdatabuffout [6]);
//				}
//				break;
			default:
				// непонятно, для чего эти данные?
				TP();
				break;
			}
		}
		break;

	default:
		break;
	}
	return USBD_OK;
}


static USBD_StatusTypeDef USBD_CDCEEM_Setup(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	static USBALIGN_BEGIN uint8_t buff [32] USBALIGN_END;	// was: 7
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	if ((req->bmRequest & USB_REQ_TYPE_DIR) != 0)
	{
		// IN direction
		switch (req->bmRequest & USB_REQ_TYPE_MASK)
		{
		case USB_REQ_TYPE_CLASS:
			switch (interfacev)
			{

			case INTERFACE_CDCEEM_DATA:	// CDC EEM interface
				{
					switch (req->bRequest)
					{
//					case CDC_GET_LINE_CODING:
//						PRINTF(PSTR("USBD_CDC_Setup IN: CDC_GET_LINE_CODING, dwDTERate=%lu\n"), (unsigned long) dwDTERate [interfacev]);
//						USBD_poke_u32(& buff [0], dwDTERate [interfacev]); // dwDTERate
//						buff [4] = 0;	// 1 stop bit
//						buff [5] = 0;	// parity=none
//						buff [6] = 8;	// bDataBits
//
//						USBD_CtlSendData(pdev, buff, ulmin16(7, req->wLength));
						break;

					default:
						TP();
						PRINTF(PSTR("USBD_CDCEEM_Setup IN: bRequest=%02X, wValue=%04X\n"), (unsigned) req->bRequest, (unsigned) req->wValue);
						USBD_CtlError(pdev, req);
						break;
					}
				}
				break;

			default:
				break;
			}
			break;

		case USB_REQ_TYPE_STANDARD:
			switch (interfacev)
			{

			case INTERFACE_CDCEEM_DATA:	// CDC EEM interface
				{
					case USB_REQ_GET_INTERFACE:
					{
						static USBALIGN_BEGIN uint8_t buff [64] USBALIGN_END;
						//PRINTF(PSTR("USBD_CDC_Setup: USB_REQ_TYPE_STANDARD USB_REQ_GET_INTERFACE dir=%02X interfacev=%d, req->wLength=%d\n"), req->bmRequest & 0x80, interfacev, (int) req->wLength);
						buff [0] = 0;
						USBD_CtlSendData(pdev, buff, ulmin16(ARRAY_SIZE(buff), req->wLength));
					}
					break;
				}
			}
			break;
		}
	}
	else
	{
		// OUT direction
		switch (req->bmRequest & USB_REQ_TYPE_MASK)
		{
		case USB_REQ_TYPE_CLASS:
			switch (interfacev)
			{
			case INTERFACE_CDCEEM_DATA:	// CDC EEM interface
				switch (req->bRequest)
				{
				case CDC_SET_CONTROL_LINE_STATE:
					// Выполнение этого запроса не требует дополнительного чтения данных
					PRINTF(PSTR("USBD_CDCEEM_Setup OUT: CDC_SET_CONTROL_LINE_STATE, wValue=%04X, wLength=%04X\n"), req->wValue, (unsigned) req->wLength);
//					usb_cdc_control_state [interfacev] = req->wValue;
					ASSERT(req->wLength == 0);
					break;

				default:
					TP();
					PRINTF(PSTR("USBD_CDCEEM_Setup OUT: bRequest=%02X, wValue=%04X, wLength=%04X\n"), (unsigned) req->bRequest, (unsigned) req->wValue, (unsigned) req->wLength);
					break;
				}
				/* все запросы этого класса устройств */
				if (req->wLength != 0)
				{
					USBD_CtlPrepareRx(pdev, cdceem_epXdatabuffout, ulmin16(ARRAY_SIZE(cdceem_epXdatabuffout), req->wLength));
				}
				else
				{
					USBD_CtlSendStatus(pdev);
				}
				break;

			default:
				break;
			}
			break;

		case USB_REQ_TYPE_STANDARD:
			switch (req->bRequest)
			{
			case USB_REQ_SET_INTERFACE:
				switch (interfacev)
				{
				case INTERFACE_CDCEEM_DATA:	// CDC EEM interface
					// Only zero value here
					//altinterfaces [interfacev] = LO_BYTE(req->wValue);
					PRINTF("USBD_CDCEEM_Setup: CDC interface %d set to %d\n", (int) interfacev, (int) LO_BYTE(req->wValue));
					//bufers_set_cdcalt(altinterfaces [interfacev]);
					USBD_CtlSendStatus(pdev);
					break;

				default:
					// Другие интерфейсы - ничего не отправляем.
					//TP();
					break;
				}
			}
			break;

		default:
			break;
		}
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDCEEM_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	/* CDC EEM Open EP IN */
	USBD_LL_OpenEP(pdev, USBD_EP_CDCEEM_IN, USBD_EP_TYPE_BULK, USBD_CDCEEM_BUFSIZE);
	USBD_LL_Transmit(pdev, USBD_EP_CDCEEM_IN, NULL, 0);
	/* CDC EEM Open EP OUT */
	USBD_LL_OpenEP(pdev, USBD_EP_CDCEEM_OUT, USBD_EP_TYPE_BULK, USBD_CDCEEM_BUFSIZE);
    /* CDC EEM Prepare Out endpoint to receive 1st packet */
    USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(USBD_EP_CDCEEM_OUT), cdceem1buffout,  USBD_CDCEEM_BUFSIZE);

    cdceemoutstate = CDCEEMOUT_COMMAND;

	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDCEEM_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	USBD_LL_CloseEP(pdev, USBD_EP_CDCEEM_IN);
	USBD_LL_CloseEP(pdev, USBD_EP_CDCEEM_OUT);
	return USBD_OK;
}

static void USBD_CDCEEM_ColdInit(void)
{
}

const USBD_ClassTypeDef USBD_CLASS_CDC_EEM =
{
	USBD_CDCEEM_ColdInit,
	USBD_CDCEEM_Init,	// Init
	USBD_CDCEEM_DeInit,	// DeInit
	USBD_CDCEEM_Setup,		// Setup
	NULL,	// EP0_TxSent
	USBD_CDCEEM_EP0_RxReady,	// EP0_RxReady
	USBD_CDCEEM_DataIn,	// DataIn
	USBD_CDCEEM_DataOut,	// DataOut
	NULL,	//USBD_XXX_SOF,	// SOF
	NULL,	//USBD_XXX_IsoINIncomplete,	// IsoINIncomplete
	NULL,	//USBD_XXX_IsoOUTIncomplete,	// IsoOUTIncomplete
};

#endif /* WITHUSBHW && WITHUSBCDCEEM */

