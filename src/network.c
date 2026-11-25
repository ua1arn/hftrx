/*
 * network.c
 *
 *  Created on: Dec 10, 2020
 *      Author: gena
 */

/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	/* зависящие от процессора функции работы с портами */

#if WITHLWIP

#include "sdcard.h"
#include "clocks.h"

#include <stdbool.h>
#include "lwip/opt.h"

#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/ip.h"
#include "lwip/udp.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"

#include "../lib/dhcp-server/dhserver.h"
#include "../lib/dns-server/dnserver.h"
#include "lwip/apps/httpd.h"


//#define DHCP_SERVER 1
//#define DNS_SERVER 1

static ip4_addr_t myIP = IPADDR4_INIT_BYTES(192, 168, 17, 33);
static ip4_addr_t myNETMASK = IPADDR4_INIT_BYTES(255, 255, 255, 0);
static ip4_addr_t myGATEWAY = IPADDR4_INIT_BYTES( 0, 0, 0, 0);
#define LOCALNETNAME "storch"


/*
 *  В конфигурации описано имя и размер
 *
 *  #define LWIP_RAM_HEAP_POINTER		lwipBuffer
 *  #define MEM_SIZE                        32768
 */

#if defined (LWIP_RAM_HEAP_POINTER)
	ALIGNX_BEGIN RAMNC uint8_t LWIP_RAM_HEAP_POINTER [MEM_SIZE] ALIGNX_END;
#endif /* defined (LWIP_RAM_HEAP_POINTER) */


//#define TX_ZLP_TEST
static void init_lwip(void);
static void init_htserv(void);
static void init_dhserv(void);
static void init_dnserv(void);

#if DHCP_SERVER
static void init_dhserv(void)
{
	PRINTF("network_initialize: init_dhserv\n");
	static dhcp_entry_t dhcpentries [] =
	{
		{ {0}, IPADDR4_INIT_BYTES(192, 168, 7, 2), IPADDR4_INIT_BYTES(255, 255, 255, 0), 24 * 60 * 60 },
		{ {0}, IPADDR4_INIT_BYTES(192, 168, 7, 3), IPADDR4_INIT_BYTES(255, 255, 255, 0), 24 * 60 * 60 },
		{ {0}, IPADDR4_INIT_BYTES(192, 168, 7, 4), IPADDR4_INIT_BYTES(255, 255, 255, 0), 24 * 60 * 60 },
		{ {0}, IPADDR4_INIT_BYTES(192, 168, 7, 5), IPADDR4_INIT_BYTES(255, 255, 255, 0), 24 * 60 * 60 },
	};

	static dhcp_config_t dhcp_config =
	{
		IPADDR4_INIT_BYTES(192, 168, 7, 1), 67,
		IPADDR4_INIT_BYTES(192, 168, 7, 1),
		LOCALNETNAME,
		ARRAY_SIZE(dhcpentries),
		dhcpentries
	};

	for (int i = 0; i < ARRAY_SIZE(dhcpentries); ++ i)
	{
		const u8_t p1 = ip4_addr1_val(myIP);
		const u8_t p2 = ip4_addr2_val(myIP);
		const u8_t p3 = ip4_addr3_val(myIP);
		const u8_t p4 = ip4_addr4_val(myIP) + 5;
		IP4_ADDR(& dhcpentries [i].addr, p1, p2, p3, p4);
		dhcpentries [i].subnet = myNETMASK;
	}
	ip4_addr_copy(dhcp_config.router, myGATEWAY);	// 0.0.0.0 - не назначается (иначе не работают другие интерфейсы)
	ip4_addr_copy(dhcp_config.dns, myIP);

	while (dhserv_init(& dhcp_config) != ERR_OK)
		;
}
#endif /* DHCP_SERVER */


static bool dns_query_proc(const char *name, ip4_addr_t *addr)
{
	PRINTF("dns_query_proc: name='%s'\n", name);
	if (strcmp(name, "wpad." LOCALNETNAME) == 0)
	{
		PRINTF("dns_query_proc: name='%s' -> loopback\n", name);
		ip4_addr_set_loopback(addr);
		return true;
	}
  if (
		  strcmp(name, LOCALNETNAME) == 0 ||
		  strcmp(name, "run." LOCALNETNAME) == 0 ||
		  strcmp(name, "www.run." LOCALNETNAME) == 0
		  )
  {
		PRINTF("dns_query_proc: name='%s' !!!!!!!!!!!!!!!!!!!!!!!!\n", name);
	ip4_addr_copy(* addr, myIP);
    return true;
  }
  return false;
}


#if DNS_SERVER
static void init_dnserv(void)
{
	//IP4_ADDR(& ipaddr, myIP [0], myIP [1], myIP [2], myIP [3]);

	while (dnserv_init(IP_ADDR_ANY, 53, dns_query_proc) != ERR_OK)
		;
}
#endif

static void init_lwip()
{
	//PRINTF("init_lwip start\n");


	lwip_init();
	//stmr_add(&tcp_timer);
	//PRINTF("init_lwip done\n");
}

#if LWIP_HTTPD_CGI


const char *state_cgi_handler(int index, int n_params, char *params[], char *values[])
{
  return "/state.shtml";
}

static const char *ssi_tags_table[] =
{
    "systick", /* 0 */
    "btn",     /* 1 */
    "acc",     /* 2 */
    "ledg",    /* 3 */
    "ledo",    /* 4 */
    "ledr"     /* 5 */
};


static int led_g = false;
static int led_o = false;
static int led_r = false;

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

static const tCGI cgi_uri_table[] =
{
    { "/state.cgi", state_cgi_handler },
    { "/ctl.cgi",   ctl_cgi_handler },
};



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
    static unsigned ttt;
    switch (index)
    {
    case 0: /* systick */
        res = local_snprintf_P(insert, ins_len, "%u", hamradio_get_freq_a());
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
#endif /* TX_ZLP_TEST */


void init_htserv(void)
{
  http_set_cgi_handlers(cgi_uri_table, sizeof(cgi_uri_table) / sizeof(tCGI));
  http_set_ssi_handler(ssi_handler, ssi_tags_table, sizeof(ssi_tags_table) / sizeof(char *));

  httpd_init();
}

#endif /* LWIP_HTTPD_CGI */


#if SYS_LIGHTWEIGHT_PROT

sys_prot_t sys_arch_protect(void)
{
	IRQL_t irql;
	RiseIrql(IRQL_SYSTEM, & irql);
	return (sys_prot_t) irql;
}

void sys_arch_unprotect(sys_prot_t pval)
{
	LowerIrql((IRQL_t) pval);
}

#endif /* SYS_LIGHTWEIGHT_PROT */


static void lwip_1s_spool(void * ctx)
{
	(void) ctx;
	sys_check_timeouts();
	nic_linkspool(ctx);
}

#if ! LWIP_ARP && 0
/**
 * This function has to be completed by user in case of ARP OFF.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if ...
 */
static err_t low_level_output_arp_off(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
  err_t errval;
  errval = ERR_OK;

/* USER CODE BEGIN 5 */

/* USER CODE END 5 */

  return errval;

}
#endif /* LWIP_ARP */

#if LWIP_SUPPORT_CUSTOM_PBUF && 0

/* Memory Pool Declaration */
LWIP_MEMPOOL_DECLARE(RX_POOL, 10, sizeof(struct pbuf_custom), "Zero-copy RX PBUF pool");

////    custom_pbuf  = (struct pbuf_custom*)LWIP_MEMPOOL_ALLOC(RX_POOL);
////    custom_pbuf->custom_free_function = pbuf_free_custom;
////
////    p = pbuf_alloced_custom(PBUF_RAW, framelength, PBUF_REF, custom_pbuf, RxBuff->buffer, framelength);
//	  /* Initialize the RX POOL */
//	  LWIP_MEMPOOL_INIT(RX_POOL);
/**
  * @brief  Custom Rx pbuf free callback
  * @param  pbuf: pbuf to be freed
  * @retval None
  */
void pbuf_free_custom(struct pbuf *p)
{
  struct pbuf_custom* custom_pbuf = (struct pbuf_custom*)p;

  LWIP_MEMPOOL_FREE(RX_POOL, custom_pbuf);
}
#endif

static struct netif nic_netif_data;

#if 0

static err_t nic_output_fn(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr)
{
	err_t e = etharp_output(netif, p, ipaddr);
	if (e == ERR_OK)
	{
		// добавляем свои заголовки требуеющиеся для физического уповня

	}
	return e;
}
#endif

static err_t netif_init_cb(struct netif *netif)
{
	PRINTF("netif_init_cb\n");
	LWIP_ASSERT("netif != NULL", (netif != NULL));
#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "storch";
#endif /* LWIP_NETIF_HOSTNAME */
	netif->mtu = NIC_MTU;
	netif->flags = NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;

	/* Accept broadcast address and ARP traffic */
	/* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
	#if LWIP_ARP
		netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
	#else
		netif->flags |= NETIF_FLAG_BROADCAST;
	#endif /* LWIP_ARP */

	//netif->flags |= NETIF_FLAG_ETHERNET;	// на потом

	netif->state = NULL;
	netif->name[0] = 'E';
	netif->name[1] = 'X';
	netif->output = etharp_output; //nic_output_fn;	// если бы не требовалось добавлять ethernet заголовки, передачва делалась бы тут.
												// и слкдующий callback linkoutput не требовался бы вообще
	//netif->linkoutput = alw_low_level_output;//nic_linkoutput_fn;	// используется внутри etharp_output
	//netif->linkoutput = nic_linkoutput_fn;	// используется внутри etharp_output
	return ERR_OK;
}


static void init_netif(void)
{
//#if ETH_PAD_SIZE != 0
//	#error Wrong ETH_PAD_SIZE value
//#endif

	static const  uint8_t hwaddrv [6]  = { HWADDR };

	struct netif  *netif = & nic_netif_data;
	netif->hwaddr_len = 6;
	memcpy(netif->hwaddr, hwaddrv, 6);

#if DHCP_SERVER
	netif = netif_add(netif, & myIP, & myNETMASK, & myGATEWAY, NULL, netif_init_cb, ip_input);
	netif_set_default(netif);
#else
	netif = netif_add(netif, IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY, NULL, netif_init_cb, ip_input);
	//netif_set_link_up(netif);
	//netif_set_up(netif);
#endif


#if LWIP_AUTOIP
	  autoip_start(netif);
#endif /* LWIP_AUTOIP */

}


// UDP for data

uint8_t radio_send_payload(const uint8_t * payload, unsigned size){
	printhex(0, payload, size);
	return 0;
//	tx_radio_packet_configure(&sr_cfg, &tx_buf, size, &payload[0]);
//	return sr_send_radio_packet(&sr_cfg, &tx_buf, &sr_state);
//	if(size <= 0) return 0;
//	return radio_queue_put(&payload[0], size);
}

#define RADIOPORT 10010 //0x271A
static const uint_fast16_t myAppPort = RADIOPORT;	// net: 0x271A
static uint_fast16_t otherSideAppPort = RADIOPORT;	// net: 0x271A
static struct udp_pcb *pcbudpMyApp = NULL;

// вызывается для передачи в host host данных от радиоблока
void tx_data_to_host(
	const void * dbuff, unsigned dsize,
	const ip_addr_t *IPdest
	)
{
	// передача пакета средствами LWIP
	ip_addr_t d = * IPdest;	// для тестов

	PRINTF("SendToAddr: %u.%u.%u.%u, port=%u\n", ip4_addr1(& d), ip4_addr2(& d), ip4_addr3(& d), ip4_addr4(& d), otherSideAppPort);
	struct pbuf *out = pbuf_alloc(PBUF_TRANSPORT, dsize, PBUF_POOL);
	if (out == NULL)
	{

		#ifdef DEBUG_UART_MESSAGES
			printf("pult_tx_data: no memory\n");
		#endif
		return;
	}
    pbuf_take(out, dbuff, dsize);
	err_t err = udp_sendto(pcbudpMyApp, out, & d, otherSideAppPort);
	pbuf_free(out);
	if (err != ERR_OK)
	{
		#ifdef DEBUG_UART_MESSAGES
			printf("pult_tx_data: udp_sendto error\n");
		#endif
		return;
	}
	//PRINTF("pult_tx_data trough LWIP done\n");
}



//void send_payload_to_host(const uint8_t *dbuff, unsigned size)
//{
//  uint8_t ta [4];
//  ta [0] = entries [0].addr [0];
//  ta [1] = entries [0].addr [1];
//  ta [2] = entries [0].addr [2];
//  ta [3] = 255;//entries [0].addr [3];
//  tx_data_to_host(dbuff, size, ta);
//}

// вызывается по приёму от host данных для радиоблока
static void udpapp_recv_proc(void *arg,
		struct udp_pcb *pcb,
		struct pbuf *p,
	   const ip_addr_t *addr, u16_t port)
{
//	PRINTF("udpapp_recv_proc from %d.%d.%d.%d from port %u\n",
//			(addr->addr >> 0) & 0xFF, (addr->addr >> 8) & 0xFF, (addr->addr >> 16) & 0xFF, (addr->addr >> 24) & 0xFF,
//			port
//			);

    //const uint8_t srcIp [4] = { ip4_addr1_16(addr), ip4_addr2_16(addr), ip4_addr3_16(addr), ip4_addr4_16(addr) };
	//PRINTF("complete packet:\n");
    uint8_t fromhostdata [124];
    u16_t size = pbuf_copy_partial(p, fromhostdata, sizeof fromhostdata, 0);
	pbuf_free(p);

#ifdef DEBUG_UART_MESSAGES
	printf("From host:\n");

	printhex(0, fromhostdata, size);
#endif

	radio_send_payload(&fromhostdata[0], size);
//	send_payload_to_host(&fromhostdata[0], size);

	char b [128];
	//int n = local_snprintf_P(b, ARRAY_SIZE(b), "You send message!");
	int n = local_snprintf_P(b, ARRAY_SIZE(b), "You send message from port %u with %u bytes of data!", port, size);
	printhex(0, b, n);
	tx_data_to_host(b, n, addr);
}

static err_t udpapperv_app_init(void)
{
	ip_addr_t * const ba = NULL;
	err_t err;
	// UDP to application
	pcbudpMyApp = udp_new();
	//ASSERT(pcbudpMyApp != NULL);
	if (pcbudpMyApp == NULL)
		return ERR_MEM;
	err = udp_bind(pcbudpMyApp, ba, myAppPort);
	if (err != ERR_OK)
	{
		//udpappserv_free();
		return err;
	}
	udp_recv(pcbudpMyApp, udpapp_recv_proc, NULL);

	return ERR_OK;
}

#include "audio.h"
#include "lwip_flac_audio_stream/audio_server.h"
#include "lwip_flac_audio_stream/webm_pcm_stream.h"

static adapter_t audionet_out;

static void audio_32ms(void * ctx)
{
	int16_t buff [WEBM_BLOCK_SAMPLES];
	unsigned i;
	for (i = 0; i < ARRAY_SIZE(buff); ++ i)
	{
		FLOAT_t v = get_lout();
		get_lout(), get_lout(), get_lout(), get_lout(), get_lout();
		buff [i] = adpt_output(& audionet_out, v);
	}
	audio_server_push_pcm(buff);
}
void network_initialize(void)
{
	struct netif  *netif = & nic_netif_data;

	adpt_initialize(& audionet_out, 16, 0, "audionet_out");
	init_lwip();
	//nic_buffers_initialize();
	nic_initialize(netif);
	init_netif();
	nic_set_mac(netif);

#if 1
	  PRINTF("network_initialize: start DHCP & DNS\n");
	#if DHCP_SERVER
		  init_dhserv();
	#endif /* DHCP_SERVER */
	#if DNS_SERVER
		  init_dnserv();
	#endif
#endif

#if LWIP_HTTPD_CGI
	  PRINTF("network_initialize: start HTTP server\n");
	  //init_htserv();
#endif /* LWIP_HTTPD_CGI */
	  //echo_init();
	udpapperv_app_init();
	{
		static ticker_t ticker;
		static dpcobj_t dpcobj;

		dpcobj_initialize(& dpcobj, lwip_1s_spool, & nic_netif_data, "lwip_1s_spool");
		ticker_initialize_user(& ticker, 1000, & dpcobj, "lwip_1s_spool");
		ticker_add(& ticker);
	}
	if (1)
	{
		audio_server_init(8000, 1, 16);
		static ticker_t ticker;
		static dpcobj_t dpcobj;

		dpcobj_initialize(& dpcobj, audio_32ms, NULL, "sudio_send");
		ticker_initialize_user(& ticker, 60, & dpcobj, "sudio_send");
		ticker_add(& ticker);
	}
}


#if 1

#if 0
u32_t lwip_sys_now;

u32_t
sys_jiffies(void)
{
  return lwip_sys_now;
}

u32_t
sys_now(void)
{
  return lwip_sys_now;
}
#endif

void
sys_init(void)
{
}

#if !NO_SYS

test_sys_arch_waiting_fn the_waiting_fn;

void
test_sys_arch_wait_callback(test_sys_arch_waiting_fn waiting_fn)
{
  the_waiting_fn = waiting_fn;
}

err_t
sys_sem_new(sys_sem_t *sem, u8_t count)
{
  LWIP_ASSERT("sem != NULL", sem != NULL);
  *sem = count + 1;
  return ERR_OK;
}

void
sys_sem_free(sys_sem_t *sem)
{
  LWIP_ASSERT("sem != NULL", sem != NULL);
  *sem = 0;
}

void
sys_sem_set_invalid(sys_sem_t *sem)
{
  LWIP_ASSERT("sem != NULL", sem != NULL);
  *sem = 0;
}

/* semaphores are 1-based because RAM is initialized as 0, which would be valid */
u32_t
sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  u32_t ret = 0;
  LWIP_ASSERT("sem != NULL", sem != NULL);
  LWIP_ASSERT("*sem > 0", *sem > 0);
  if (*sem == 1) {
    /* need to wait */
    if(!timeout)
    {
      /* wait infinite */
      LWIP_ASSERT("cannot wait without waiting callback", the_waiting_fn != NULL);
      do {
        int expectSomething = the_waiting_fn(sem, NULL);
        LWIP_ASSERT("*sem > 0", *sem > 0);
        LWIP_ASSERT("expecting a semaphore count but it's 0", !expectSomething || (*sem > 1));
        ret++;
        if (ret == SYS_ARCH_TIMEOUT) {
          ret--;
        }
      } while(*sem == 1);
    }
    else
    {
      if (the_waiting_fn) {
        int expectSomething = the_waiting_fn(sem, NULL);
        LWIP_ASSERT("expecting a semaphore count but it's 0", !expectSomething || (*sem > 1));
      }
      LWIP_ASSERT("*sem > 0", *sem > 0);
      if (*sem == 1) {
        return SYS_ARCH_TIMEOUT;
      }
      ret = 1;
    }
  }
  LWIP_ASSERT("*sem > 0", *sem > 0);
  (*sem)--;
  LWIP_ASSERT("*sem > 0", *sem > 0);
  /* return the time we waited for the sem */
  return ret;
}

void
sys_sem_signal(sys_sem_t *sem)
{
  LWIP_ASSERT("sem != NULL", sem != NULL);
  LWIP_ASSERT("*sem > 0", *sem > 0);
  (*sem)++;
  LWIP_ASSERT("*sem > 0", *sem > 0);
}

err_t
sys_mutex_new(sys_mutex_t *mutex)
{
  LWIP_ASSERT("mutex != NULL", mutex != NULL);
  *mutex = 1; /* 1 allocated */
  return ERR_OK;
}

void
sys_mutex_free(sys_mutex_t *mutex)
{
  /* parameter check */
  LWIP_ASSERT("mutex != NULL", mutex != NULL);
  LWIP_ASSERT("*mutex >= 1", *mutex >= 1);
  *mutex = 0;
}

void
sys_mutex_set_invalid(sys_mutex_t *mutex)
{
  LWIP_ASSERT("mutex != NULL", mutex != NULL);
  *mutex = 0;
}

void
sys_mutex_lock(sys_mutex_t *mutex)
{
  /* nothing to do, no multithreading supported */
  LWIP_ASSERT("mutex != NULL", mutex != NULL);
  /* check that the mutext is valid and unlocked (no nested locking) */
  LWIP_ASSERT("*mutex >= 1", *mutex == 1);
  /* we count up just to check the correct pairing of lock/unlock */
  (*mutex)++;
  LWIP_ASSERT("*mutex >= 1", *mutex >= 1);
}

void
sys_mutex_unlock(sys_mutex_t *mutex)
{
  /* nothing to do, no multithreading supported */
  LWIP_ASSERT("mutex != NULL", mutex != NULL);
  LWIP_ASSERT("*mutex >= 1", *mutex >= 1);
  /* we count down just to check the correct pairing of lock/unlock */
  (*mutex)--;
  LWIP_ASSERT("*mutex >= 1", *mutex >= 1);
}


sys_thread_t
sys_thread_new(const char *name, lwip_thread_fn function, void *arg, int stacksize, int prio)
{
  LWIP_UNUSED_ARG(name);
  LWIP_UNUSED_ARG(function);
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(stacksize);
  LWIP_UNUSED_ARG(prio);
  /* threads not supported */
  return 0;
}

err_t
sys_mbox_new(sys_mbox_t *mbox, int size)
{
  int mboxsize = size;
  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  LWIP_ASSERT("size >= 0", size >= 0);
  if (size == 0) {
    mboxsize = 1024;
  }
  mbox->head = mbox->tail = 0;
  mbox->sem = mbox; /* just point to something for sys_mbox_valid() */
  mbox->q_mem = (void**)malloc(sizeof(void*)*mboxsize);
  mbox->size = mboxsize;
  mbox->used = 0;

  memset(mbox->q_mem, 0, sizeof(void*)*mboxsize);
  return ERR_OK;
}

void
sys_mbox_free(sys_mbox_t *mbox)
{
  /* parameter check */
  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  LWIP_ASSERT("mbox->sem != NULL", mbox->sem != NULL);
  LWIP_ASSERT("mbox->sem == mbox", mbox->sem == mbox);
  LWIP_ASSERT("mbox->q_mem != NULL", mbox->q_mem != NULL);
  mbox->sem = NULL;
  free(mbox->q_mem);
  mbox->q_mem = NULL;
}

void
sys_mbox_set_invalid(sys_mbox_t *mbox)
{
  LWIP_ASSERT("mbox != NULL", mbox != NULL);
  LWIP_ASSERT("mbox->q_mem == NULL", mbox->q_mem == NULL);
  mbox->sem = NULL;
  mbox->q_mem = NULL;
}

void
sys_mbox_post(sys_mbox_t *q, void *msg)
{
  LWIP_ASSERT("q != SYS_MBOX_NULL", q != SYS_MBOX_NULL);
  LWIP_ASSERT("q->sem == q", q->sem == q);
  LWIP_ASSERT("q->q_mem != NULL", q->q_mem != NULL);
  LWIP_ASSERT("q->used >= 0", q->used >= 0);
  LWIP_ASSERT("q->size > 0", q->size > 0);

  LWIP_ASSERT("mbox already full", q->used < q->size);

  q->q_mem[q->head] = msg;
  q->head++;
  if (q->head >= (unsigned int)q->size) {
    q->head = 0;
  }
  LWIP_ASSERT("mbox is full!", q->head != q->tail);
  q->used++;
}

err_t
sys_mbox_trypost(sys_mbox_t *q, void *msg)
{
  LWIP_ASSERT("q != SYS_MBOX_NULL", q != SYS_MBOX_NULL);
  LWIP_ASSERT("q->sem == q", q->sem == q);
  LWIP_ASSERT("q->q_mem != NULL", q->q_mem != NULL);
  LWIP_ASSERT("q->used >= 0", q->used >= 0);
  LWIP_ASSERT("q->size > 0", q->size > 0);
  LWIP_ASSERT("q->used <= q->size", q->used <= q->size);

  if (q->used == q->size) {
    return ERR_MEM;
  }
  sys_mbox_post(q, msg);
  return ERR_OK;
}

err_t
sys_mbox_trypost_fromisr(sys_mbox_t *q, void *msg)
{
  return sys_mbox_trypost(q, msg);
}

u32_t
sys_arch_mbox_fetch(sys_mbox_t *q, void **msg, u32_t timeout)
{
  u32_t ret = 0;
  u32_t ret2;
  LWIP_ASSERT("q != SYS_MBOX_NULL", q != SYS_MBOX_NULL);
  LWIP_ASSERT("q->sem == q", q->sem == q);
  LWIP_ASSERT("q->q_mem != NULL", q->q_mem != NULL);
  LWIP_ASSERT("q->used >= 0", q->used >= 0);
  LWIP_ASSERT("q->size > 0", q->size > 0);

  if (q->used == 0) {
    /* need to wait */
    /* need to wait */
    if(!timeout)
    {
      /* wait infinite */
      LWIP_ASSERT("cannot wait without waiting callback", the_waiting_fn != NULL);
      do {
        int expectSomething = the_waiting_fn(NULL, q);
        LWIP_ASSERT("q->used >= 0", q->used >= 0);
        LWIP_ASSERT("expecting item available but it's 0", !expectSomething || (q->used > 0));
        ret++;
        if (ret == SYS_ARCH_TIMEOUT) {
          ret--;
        }
      } while(q->used == 0);
    }
    else
    {
      if (the_waiting_fn) {
        int expectSomething = the_waiting_fn(NULL, q);
        LWIP_ASSERT("expecting item available count but it's 0", !expectSomething || (q->used > 0));
      }
      LWIP_ASSERT("q->used >= 0", q->used >= 0);
      if (q->used == 0) {
        if(msg) {
          *msg = NULL;
        }
        return SYS_ARCH_TIMEOUT;
      }
      ret = 1;
    }
  }
  LWIP_ASSERT("q->used > 0", q->used > 0);
  ret2 = sys_arch_mbox_tryfetch(q, msg);
  LWIP_ASSERT("got no message", ret2 == 0);
  return ret;
}

u32_t
sys_arch_mbox_tryfetch(sys_mbox_t *q, void **msg)
{
  LWIP_ASSERT("q != SYS_MBOX_NULL", q != SYS_MBOX_NULL);
  LWIP_ASSERT("q->sem == q", q->sem == q);
  LWIP_ASSERT("q->q_mem != NULL", q->q_mem != NULL);
  LWIP_ASSERT("q->used >= 0", q->used >= 0);
  LWIP_ASSERT("q->size > 0", q->size > 0);

  if (!q->used) {
    return SYS_ARCH_TIMEOUT;
  }
  if(msg) {
    *msg = q->q_mem[q->tail];
  }

  q->tail++;
  if (q->tail >= (unsigned int)q->size) {
    q->tail = 0;
  }
  q->used--;
  LWIP_ASSERT("q->used >= 0", q->used >= 0);
  return 0;
}

#if LWIP_NETCONN_SEM_PER_THREAD
/* Simple implementation of this: unit tests only support one thread */
static sys_sem_t global_netconn_sem;

sys_sem_t* sys_arch_netconn_sem_get(void)
{
  return &global_netconn_sem;
}

void sys_arch_netconn_sem_alloc(void)
{
  sys_sem_new(&global_netconn_sem, 0);
}

void sys_arch_netconn_sem_free(void)
{
  sys_sem_free(&global_netconn_sem);
}
#endif /* LWIP_NETCONN_SEM_PER_THREAD */

#endif /* !NO_SYS */

#endif
static uint_fast8_t glob_eth_dhcp = 1;

void
board_set_eth_dhcp(uint_fast8_t v)
{
	struct netif * const netif = & nic_netif_data;
	uint_fast8_t n = !! v;
	if (glob_eth_dhcp != n)
	{
		glob_eth_dhcp = n;
		if (n)
		{
            err_t e = dhcp_start(netif);
            ASSERT(ERR_OK == e);
		}
		else
		{
            dhcp_release_and_stop(netif);
            netif_set_ipaddr(netif, IP4_ADDR_ANY4);
            netif_set_netmask(netif, IP4_ADDR_ANY4);
            netif_set_gw(netif, IP4_ADDR_ANY4);
		}
	}
}

uint_fast8_t board_get_eth_dhcp(void)
{
	return glob_eth_dhcp;
}


/* Параметры интерфейса для отображения в меню */
size_t getvaltextethaddr(char * buff, size_t count, int_fast32_t value)
{
	struct netif * const netif = & nic_netif_data;
	const ip4_addr_t addr = * netif_ip4_addr(netif);
	const u8_t p1 = ip4_addr1_val(addr);
	const u8_t p2 = ip4_addr2_val(addr);
	const u8_t p3 = ip4_addr3_val(addr);
	const u8_t p4 = ip4_addr4_val(addr);
	(void) value;
	/* Параметры интерфейса для отображения в меню  */
	return local_snprintf_P(buff, count, "%u.%u.%u.%u", p1, p2, p3, p4);
}

/* Параметры интерфейса для отображения в меню */
size_t getvaltextethmask(char * buff, size_t count, int_fast32_t value)
{
	struct netif * const netif = & nic_netif_data;
	const ip4_addr_t netmask = * netif_ip4_netmask(netif);
	const u8_t p1 = ip4_addr1_val(netmask);
	const u8_t p2 = ip4_addr2_val(netmask);
	const u8_t p3 = ip4_addr3_val(netmask);
	const u8_t p4 = ip4_addr4_val(netmask);
	(void) value;
	/* Параметры интерфейса для отображения в меню  */
	return local_snprintf_P(buff, count, "%u.%u.%u.%u", p1, p2, p3, p4);
}

/* Параметры интерфейса для отображения в меню */
size_t getvaltextethgateway(char * buff, size_t count, int_fast32_t value)
{
	struct netif * const netif = & nic_netif_data;
	const ip4_addr_t gw = * netif_ip4_gw(netif);
	const u8_t p1 = ip4_addr1_val(gw);
	const u8_t p2 = ip4_addr2_val(gw);
	const u8_t p3 = ip4_addr3_val(gw);
	const u8_t p4 = ip4_addr4_val(gw);
	(void) value;
	/* Параметры интерфейса для отображения в меню  */
	return local_snprintf_P(buff, count, "%u.%u.%u.%u", p1, p2, p3, p4);
}

/* Параметры интерфейса для отображения в меню */
size_t getvaltextethlinkstate(char * buff, size_t count, int_fast32_t value)
{
	struct netif * const netif = & nic_netif_data;
	(void) value;
	return local_snprintf_P(buff, count, "%s", netif_is_link_up(netif) ? "Up" : "Down");
}

#else

void network_initialize(void)
{

}

#endif /*  WITHLWIP */
