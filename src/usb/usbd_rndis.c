/*
 * usbd_cdc.c
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
*/

#include "hardware.h"

#if WITHUSBHW && WITHUSBRNDIS

#include "formats.h"
#include "usb_core.h"

#include "lwip/opt.h"

#include "lwip/init.h"
//#include "lwip/stats.h"
//#include "lwip/sys.h"
//#include "lwip/mem.h"
//#include "lwip/memp.h"
//#include "lwip/pbuf.h"
#include "lwip/netif.h"
//#include "lwip/sockets.h"
#include "lwip/ip.h"
//#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/dhcp.h"
//#include "lwip/priv/tcp_priv.h"
//#include "lwip/igmp.h"
//#include "lwip/dns.h"
#include "src/dhcp-server/dhserver.h"
#include "src/dns-server/dnserver.h"
#include "src/lwip-1.4.1/apps/httpserver_raw/httpd.h"
//#include "lwip/timeouts.h"
//#include "lwip/etharp.h"
//#include "lwip/ip6.h"
//#include "lwip/nd6.h"
//#include "lwip/mld6.h"
//#include "lwip/api.h"

#include <stdbool.h>
#include <stddef.h>
#include "rndis_protocol.h"


/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
#define PADDR(ptr) ((ip_addr_t *)ptr)

/* Exported constants --------------------------------------------------------*/

/* LAN */
#define HWADDR                          {0x30,0x89,0x84,0x6A,0x96,0x34}
#define IPADDR                          {192, 168, 7, 1}
#define NETMASK                         {255, 255, 255, 0}
#define GATEWAY                         {0, 0, 0, 0}

#define DHCP_ENTRIES_QNT                3
#define DHCP_ENTRIES                    {\
                                        { {0}, {192, 168, 7, 2}, {255, 255, 255, 0}, 24 * 60 * 60 }, \
                                        { {0}, {192, 168, 7, 3}, {255, 255, 255, 0}, 24 * 60 * 60 }, \
                                        { {0}, {192, 168, 7, 4}, {255, 255, 255, 0}, 24 * 60 * 60 } \
                                        }
#define DHCP_CONFIG                     { \
                                        IPADDR, 67, \
                                        IPADDR, \
                                        "stm", \
                                        DHCP_ENTRIES_QNT, \
                                        entries \
                                        }



//#define TX_ZLP_TEST

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void init_lwip(void);
void init_htserv(void);
void init_dhserv(void);
void init_dnserv(void);
void init_netif(void);
void usb_polling(void);


#define RNDIS_CONTROL_OUT_PMAADDRESS                    (0x08 * 4)                //8 bytes per EP
#define RNDIS_CONTROL_IN_PMAADDRESS                     (RNDIS_CONTROL_OUT_PMAADDRESS + USB_MAX_EP0_SIZE)
#define RNDIS_NOTIFICATION_IN_PMAADDRESS                (RNDIS_CONTROL_IN_PMAADDRESS + USB_MAX_EP0_SIZE)
#define RNDIS_DATA_IN_PMAADDRESS                        (RNDIS_NOTIFICATION_IN_PMAADDRESS + RNDIS_NOTIFICATION_IN_SZ)
#define RNDIS_DATA_OUT_PMAADDRESS                       (RNDIS_DATA_IN_PMAADDRESS + USBD_RNDIS_IN_BUFSIZE)

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
#define RNDIS_HEADER_SIZE               sizeof(rndis_data_packet_t)
#define RNDIS_RX_BUFFER_SIZE            (ETH_MAX_PACKET_SIZE + RNDIS_HEADER_SIZE)

typedef void (*rndis_rxproc_t)(const uint8_t *data, int size);
extern rndis_state_t rndis_state;
extern rndis_rxproc_t rndis_rxproc;

extern USBD_ClassTypeDef  usbd_rndis;
extern usb_eth_stat_t usb_eth_stat;
extern rndis_state_t rndis_state;

static bool rndis_rx_start(void);
uint8_t *rndis_rx_data(void);
uint16_t rndis_rx_size(void);

bool rndis_tx_start(uint8_t *data, uint16_t size);
bool rndis_tx_started(void);

bool rndis_can_send(void);
bool rndis_send(const void *data, int size);


/* Private types -------------------------------------------------------------*/
struct netif netif_data;
const char *state_cgi_handler(int index, int n_params, char *params[], char *values[]);
const char *ctl_cgi_handler(int index, int n_params, char *params[], char *values[]);

dhcp_entry_t entries[DHCP_ENTRIES_QNT] = DHCP_ENTRIES;
dhcp_config_t dhcp_config = DHCP_CONFIG;


static uint8_t received[RNDIS_MTU + 14];
static int recvSize = 0;

/* Private variables ---------------------------------------------------------*/
const uint8_t ipaddr[4]  = IPADDR;

/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static err_t netif_init_cb(struct netif *netif);
static err_t linkoutput_fn(struct netif *netif, struct pbuf *p);
static err_t output_fn(struct netif *netif, struct pbuf *p, ip_addr_t *ipaddr);
static bool dns_query_proc(const char *name, ip_addr_t *addr);
static uint16_t ssi_handler(int index, char *insert, int ins_len);

static int outputs = 0;

// Transceiving Ethernet packets
static err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
{
    int i;
    struct pbuf *q;
    static char data[RNDIS_MTU + 14 + 4];
    int size = 0;
    for (i = 0; i < 200; i++)
    {
        if (rndis_can_send()) break;
        local_delay_ms(1);
    }
    for(q = p; q != NULL; q = q->next)
    {
        if (size + q->len > RNDIS_MTU + 14)
            return ERR_ARG;
        memcpy(data + size, (char *)q->payload, q->len);
        size += q->len;
    }
    if (!rndis_can_send())
        return ERR_USE;
    rndis_send(data, size);
    outputs++;
    return ERR_OK;
}

// Receiving Ethernet packets
// user-mode function
void usb_polling(void)
{
	struct pbuf *frame;
	system_disableIRQ();
	if (recvSize == 0)
	{
		system_enableIRQ();
		return;
	}
	system_enableIRQ();

	frame = pbuf_alloc(PBUF_RAW, recvSize, PBUF_POOL);
	if (frame == NULL)
	{
		return;
	}

	system_disableIRQ();
	memcpy(frame->payload, received, recvSize);
	frame->len = recvSize;
	recvSize = 0;
	system_enableIRQ();

	err_t e = ethernet_input(frame, & netif_data);
	if (e != ERR_OK)
	{
		pbuf_free(frame);
	}
}


static err_t output_fn(struct netif *netif, struct pbuf *p, ip_addr_t *ipaddr)
{
  return etharp_output(netif, p, ipaddr);
}

static err_t netif_init_cb(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
  netif->mtu = RNDIS_MTU;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
  netif->state = NULL;
  netif->name[0] = 'E';
  netif->name[1] = 'X';
  netif->linkoutput = linkoutput_fn;
  netif->output = output_fn;
  return ERR_OK;
}
/*
TIMER_PROC(tcp_timer, TCP_TMR_INTERVAL * 1000, 1, NULL)
{
  tcp_tmr();
}
*/

static bool dns_query_proc(const char *name, ip_addr_t *addr)
{
  if (strcmp(name, "run.stm") == 0 || strcmp(name, "www.run.stm") == 0)
  {
    addr->addr = *(uint32_t *)ipaddr;
    return true;
  }
  return false;
}


static void on_packet(const uint8_t *data, int size)
{
    memcpy(received, data, size);
    recvSize = size;
}

void init_lwip()
{
//	PRINTF("init_lwip start\n");
  uint8_t hwaddr[6]  = HWADDR;
  uint8_t netmask[4] = NETMASK;
  uint8_t gateway[4] = GATEWAY;

  struct netif  *netif = &netif_data;
  rndis_rxproc = on_packet;

  lwip_init();
  netif->hwaddr_len = 6;
  memcpy(netif->hwaddr, hwaddr, 6);

  netif = netif_add(netif, PADDR(ipaddr), PADDR(netmask), PADDR(gateway), NULL, netif_init_cb, ip_input);
  netif_set_default(netif);

  //stmr_add(&tcp_timer);
//	PRINTF("init_lwip done\n");
}



void init_netif(void)
{
  while (!netif_is_up(&netif_data));
}

void init_dnserv(void)
{
	uint8_t ipaddr [4] = IPADDR;
	while (dnserv_init(PADDR(ipaddr), 53, dns_query_proc) != ERR_OK)
		;
}

void init_dhserv(void)
{
  while (dhserv_init(&dhcp_config) != ERR_OK)
	  ;
}

#if 1
static const char *ssi_tags_table[] =
{
    "systick", /* 0 */
    "btn",     /* 1 */
    "acc",     /* 2 */
    "ledg",    /* 3 */
    "ledo",    /* 4 */
    "ledr"     /* 5 */
};

static const tCGI cgi_uri_table[] =
{
    { "/state.cgi", state_cgi_handler },
    { "/ctl.cgi",   ctl_cgi_handler },
};


void init_htserv(void)
{
  http_set_cgi_handlers(cgi_uri_table, sizeof(cgi_uri_table) / sizeof(tCGI));
  http_set_ssi_handler(ssi_handler, ssi_tags_table, sizeof(ssi_tags_table) / sizeof(char *));
  httpd_init();
}

static uint8_t PORTC[8];

const char *state_cgi_handler(int index, int n_params, char *params[], char *values[])
{
  return "/state.shtml";
}


bool led_g = false;
bool led_o = false;
bool led_r = false;

const char *ctl_cgi_handler(int index, int n_params, char *params[], char *values[])
{
    int i;
    for (i = 0; i < n_params; i++)
    {
        if (strcmp(params[i], "g") == 0) led_g = *values[i] == '1';
        if (strcmp(params[i], "o") == 0) led_o = *values[i] == '1';
        if (strcmp(params[i], "r") == 0) led_r = *values[i] == '1';
    }


    return "/state.shtml";
}

#ifdef TX_ZLP_TEST
static uint16_t ssi_handler(int index, char *insert, int ins_len)
{
  int res;
  static uint8_t i;
  static uint8_t c;

  if (ins_len < 32) return 0;

  if (c++ == 10)
  {
    i++;
    i &= 0x07;
    c = 0;
  }
  switch (index)
  {
  case 0: // systick
    res = local_snprintf_P(insert, ins_len, "%s", "1234");
    break;
  case 1: // PORTC
    {
      res = local_snprintf_P(insert, ins_len, "%u, %u, %u, %u, %u, %u, %u, %u", 10, 10, 10, 10, 10, 10, 10, 10);
      break;
    }
  case 2: // PA0
    *insert = '0' + (i == 0);
    res = 1;
    break;
  case 3: // PA1
    *insert = '0' + (i == 1);
    res = 1;
    break;
  case 4: // PA2
    *insert = '0' + (i == 2);
    res = 1;
    break;
  case 5: // PA3
    *insert = '0' + (i == 3);
    res = 1;
    break;
  case 6: // PA4
    *insert = '0' + (i == 4);
    res = 1;
    break;
  case 7: // PA5
    *insert = '0' + (i == 5);
    res = 1;
    break;
  case 8: // PA6
    *insert = '0' + (i == 6);
    res = 1;
    break;
  case 9: // PA7
    *insert = '0' + (i == 7);
    res = 1;
    break;
  }

  return res;
}
#else
static u16_t ssi_handler(int index, char *insert, int ins_len)
{
    int res = 0;

    if (ins_len < 32) return 0;

    switch (index)
    {
    case 0: /* systick */
        res = local_snprintf_P(insert, ins_len, "%u", (unsigned)111);
        break;
    case 1: /* btn */
        res = local_snprintf_P(insert, ins_len, "%i", 1);
        break;
    case 2: /* acc */
    {
        int acc[3];
        acc[0] = 1;
        acc[1] = 2;
        acc[2] = 4;
        res = local_snprintf_P(insert, ins_len, "%i, %i, %i", acc[0], acc[1], acc[2]);
        break;
    }
    case 3: /* ledg */
        *insert = '0' + (1 & 1);
        res = 1;
        break;
    case 4: /* ledo */
        *insert = '0' + (1 & 1);
        res = 1;
        break;
    case 5: /* ledr */
        *insert = '0' + (1 & 1);
        res = 1;
        break;
    }

    return res;
}
#endif

#endif


static void USBD_RNDIS_ColdInit(void)
{
	  init_lwip();
	  init_netif();
	  init_dhserv();
	  init_dnserv();
	  //init_htserv();
	  //echo_init();
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
void response_available(USBD_HandleTypeDef *pdev);

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
USBD_HandleTypeDef *pDev;

uint8_t station_hwaddr[6] = { STATION_HWADDR };
uint8_t permanent_hwaddr[6] = { PERMANENT_HWADDR };
usb_eth_stat_t usb_eth_stat = { 0, 0, 0, 0 };
rndis_state_t rndis_state = rndis_uninitialized;
uint32_t oid_packet_filter = 0x0000000;
uint8_t encapsulated_buffer[ENC_BUF_SIZE];

uint16_t rndis_tx_data_size = 0;
bool rndis_tx_transmitting = false;
bool rndis_tx_ZLP = false;
USBALIGN_BEGIN uint8_t usb_rx_buffer [USBD_RNDIS_OUT_BUFSIZE] USBALIGN_END ;
USBALIGN_BEGIN uint8_t rndis_rx_buffer [RNDIS_RX_BUFFER_SIZE]  USBALIGN_END;
rndis_rxproc_t rndis_rxproc = NULL;
uint16_t rndis_rx_data_size = 0;
bool rndis_rx_started = false;
uint8_t *rndis_tx_ptr = NULL;
bool rndis_first_tx = true;
int rndis_tx_size = 0;
int rndis_sended = 0;

/*******************************************************************************
                            API functions
*******************************************************************************/
/*__weak */void rndis_initialized_cb(void)
{
}

static bool rndis_rx_start(void)
{
  if (rndis_rx_started)
    return false;

  rndis_rx_started = true;
  USBD_LL_PrepareReceive(pDev,
                         USBD_EP_RNDIS_OUT,
						 usb_rx_buffer,
						 USBD_RNDIS_OUT_BUFSIZE);
  return true;
}

uint8_t *rndis_rx_data(void)
{
  if (rndis_rx_size())
    return rndis_rx_buffer + RNDIS_HEADER_SIZE;
  else
    return NULL;
}

uint16_t rndis_rx_size(void)
{
  if (!rndis_rx_started)
    return rndis_rx_data_size;
  else
    return 0;
}

/* __weak */ void rndis_rx_ready_cb(void)
{
}

bool rndis_tx_start(uint8_t *data, uint16_t size)
{
	unsigned sended;
	static uint8_t first [USBD_RNDIS_IN_BUFSIZE];
	rndis_data_packet_t *hdr;

  //if tx buffer is already transfering or has incorrect length
  if ((rndis_tx_transmitting) || (size > ETH_MAX_PACKET_SIZE) || (size == 0))
  {
    usb_eth_stat.txbad++;
    return false;
  }

  rndis_tx_transmitting = true;
  rndis_tx_ptr = data;
  rndis_tx_data_size = size;


  hdr = (rndis_data_packet_t *)first;
  memset(hdr, 0, RNDIS_HEADER_SIZE);
  hdr->MessageType = REMOTE_NDIS_PACKET_MSG;
  hdr->MessageLength = RNDIS_HEADER_SIZE + size;
  hdr->DataOffset = RNDIS_HEADER_SIZE - offsetof(rndis_data_packet_t, DataOffset);
  hdr->DataLength = size;

  sended = USBD_RNDIS_IN_BUFSIZE - RNDIS_HEADER_SIZE;
  if (sended > size)
    sended = size;
  memcpy(first + RNDIS_HEADER_SIZE, data, sended);
  rndis_tx_ptr += sended;
  rndis_tx_data_size -= sended;


  //http://habrahabr.ru/post/248729/
  if (hdr->MessageLength % USBD_RNDIS_IN_BUFSIZE == 0)
    rndis_tx_ZLP = true;

  //We should disable USB_OUT(EP3) IRQ, because if IRQ will happens with locked HAL (__HAL_LOCK()
  //in USBD_LL_Transmit()), the program will fail with big probability
  USBD_LL_Transmit (pDev,
                    USBD_EP_RNDIS_IN,
                    (uint8_t *)first,
                    USBD_RNDIS_IN_BUFSIZE);

  //Increment error counter and then decrement in data_in if OK
  usb_eth_stat.txbad++;

  return true;
}

bool rndis_tx_started(void)
{
  return rndis_tx_transmitting;
}

/*__weak */void rndis_tx_ready_cb(void)
{
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
  /*
  USBD_LL_PrepareReceive(pdev,
                         USBD_EP_RNDIS_OUT,
                         rndis_rx_buffer,
                         RNDIS_RX_BUFFER_SIZE);  */
  pDev = pdev;

  rndis_rx_start();
  return USBD_OK;
}

static USBD_StatusTypeDef  usbd_rndis_deinit(USBD_HandleTypeDef  *pdev, uint_fast8_t cfgidx)
{
  USBD_LL_CloseEP(pdev,
              USBD_EP_RNDIS_INT);
  USBD_LL_CloseEP(pdev,
		  USBD_EP_RNDIS_IN);
  USBD_LL_CloseEP(pdev,
		  USBD_EP_RNDIS_OUT);
  return USBD_OK;
}

static USBD_StatusTypeDef usbd_rndis_setup(USBD_HandleTypeDef  *pdev, const USBD_SetupReqTypedef *req)
{
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

static const char *rndis_vendor = RNDIS_VENDOR;

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
    //		USB_ETH_HOOK_SET_PROMISCIOUS_MODE(true);
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

static int sended = 0;

static USBD_StatusTypeDef usbd_cdc_transfer(void *pdev)
{
	if (sended != 0 || rndis_tx_ptr == NULL || rndis_tx_size <= 0) return USBD_OK;
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

		sended = USBD_RNDIS_IN_BUFSIZE - sizeof(rndis_data_packet_t);
		if (sended > rndis_tx_size) sended = rndis_tx_size;
		memcpy(first + sizeof(rndis_data_packet_t), rndis_tx_ptr, sended);

		USBD_LL_Transmit (pDev,
						USBD_EP_RNDIS_IN,
						(uint8_t *)&first,
						sizeof(rndis_data_packet_t) + sended);
	}
	else
	{
		int n = rndis_tx_size;
		if (n > USBD_RNDIS_IN_BUFSIZE) n = USBD_RNDIS_IN_BUFSIZE;

		USBD_LL_Transmit (pDev,
						USBD_EP_RNDIS_IN,
						rndis_tx_ptr,
						n);
		sended = n;
	}
	return USBD_OK;
}

// Control Channel      https://msdn.microsoft.com/en-us/library/windows/hardware/ff546124(v=vs.85).aspx
static USBD_StatusTypeDef usbd_rndis_ep0_recv(USBD_HandleTypeDef  *pdev)
{
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
//                      https://msdn.microsoft.com/en-us/library/windows/hardware/ff570635(v=vs.85).aspx
static USBD_StatusTypeDef usbd_rndis_data_in(USBD_HandleTypeDef*pdev, uint_fast8_t epnum)
{
	epnum &= 0x0F;
	if (epnum == (USBD_EP_RNDIS_IN & 0x0F))
	{
		rndis_first_tx = false;
		rndis_sended += sended;
		rndis_tx_size -= sended;
		rndis_tx_ptr += sended;
		sended = 0;
		usbd_cdc_transfer(pdev);

	}
	return USBD_OK;
}

static void handle_packet(const uint8_t *data, int size)
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

// Data Channel         https://msdn.microsoft.com/en-us/library/windows/hardware/ff546305(v=vs.85).aspx
static USBD_StatusTypeDef usbd_rndis_data_out(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	static int rndis_received = 0;
	if (epnum == USBD_EP_RNDIS_OUT)
	{
		PCD_EPTypeDef *ep = &(((PCD_HandleTypeDef*)(pdev->pData))->OUT_ep[epnum]);
		if (rndis_received + ep->xfer_count > RNDIS_RX_BUFFER_SIZE)
		{
			usb_eth_stat.rxbad++;
			rndis_received = 0;
		}
		else
		{
			if (rndis_received + ep->xfer_count <= RNDIS_RX_BUFFER_SIZE)
			{
				memcpy(&rndis_rx_buffer[rndis_received], usb_rx_buffer, ep->xfer_count);
				rndis_received += ep->xfer_count;
				if (ep->xfer_count != USBD_RNDIS_OUT_BUFSIZE)
				{
					handle_packet(rndis_rx_buffer, rndis_received);
					rndis_received = 0;
				}
			}
			else
			{
					rndis_received = 0;
					usb_eth_stat.rxbad++;
			}
		}
		USBD_LL_PrepareReceive(pDev,
							   USBD_EP_RNDIS_OUT,
							   usb_rx_buffer,
							   USBD_RNDIS_OUT_BUFSIZE);
		  //	DCD_EP_PrepareRx(pdev, USBD_EP_RNDIS_OUT, (uint8_t*)usb_rx_buffer, USBD_RNDIS_OUT_BUFSIZE);
	}
  //  PCD_EPTypeDef *ep = &((PCD_HandleTypeDef*)pdev->pData)->OUT_ep[epnum];
   // handle_packet((rndis_data_packet_t*)rndis_rx_buffer, RNDIS_RX_BUFFER_SIZE - ep->xfer_len - USBD_RNDIS_OUT_BUFSIZE + ep->xfer_count);
  return USBD_OK;
}

// Start Of Frame event management
static USBD_StatusTypeDef usbd_rndis_sof(USBD_HandleTypeDef *pdev)
{
	return usbd_cdc_transfer(pdev);
}

static USBD_StatusTypeDef rndis_iso_in_incomplete(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	return usbd_cdc_transfer(pdev);
}

static USBD_StatusTypeDef rndis_iso_out_incomplete(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	USBD_LL_PrepareReceive(pDev,
							USBD_EP_RNDIS_OUT,
							usb_rx_buffer,
							USBD_RNDIS_OUT_BUFSIZE);
	return USBD_OK;
}

void response_available(USBD_HandleTypeDef *pdev)
{
  __disable_irq();
  USBD_LL_Transmit (pdev,
                    USBD_EP_RNDIS_INT,
                    (uint8_t *)"\x01\x00\x00\x00\x00\x00\x00\x00",
                    USBD_RNDIS_INT_SIZE);
  __enable_irq();   //TODO
}


bool rndis_can_send(void)
{
	return rndis_tx_size <= 0;
}

bool rndis_send(const void *data, int size)
{
	//rndis_tx_start(data, size);
/*
	while (transmit_ok == 1)
	transmit_ok = 1;
	USBD_LL_Transmit (pDev,
				      USBD_EP_RNDIS_IN,
	                  (uint8_t *)data,
					  size);
*/


	if (size <= 0 ||
		size > ETH_MAX_PACKET_SIZE ||
		rndis_tx_size > 0) return false;

	__disable_irq();
	rndis_first_tx = true;
	rndis_tx_ptr = (uint8_t *)data;
	rndis_tx_size = size;
	rndis_sended = 0;
	__enable_irq();

	usbd_cdc_transfer(pDev);

	return true;
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
	usbd_rndis_sof,
	rndis_iso_in_incomplete,
	rndis_iso_out_incomplete,
};




#endif /* WITHUSBHW && WITHUSBRNDIS */

