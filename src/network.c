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

#if WITHLWIP

#include <stdbool.h>
#include "lwip/opt.h"

#include "lwip/init.h"
#include "lwip/ip.h"
#include "lwip/udp.h"
#include "lwip/dhcp.h"

#include "src/dhcp-server/dhserver.h"
#include "src/dns-server/dnserver.h"
#include "src/lwip-1.4.1/apps/httpserver_raw/httpd.h"

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

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
static void init_lwip(void);
static void init_htserv(void);
static void init_dhserv(void);
static void init_dnserv(void);




/* Private types -------------------------------------------------------------*/
static const char *state_cgi_handler(int index, int n_params, char *params[], char *values[]);
static const char *ctl_cgi_handler(int index, int n_params, char *params[], char *values[]);

static dhcp_entry_t entries[DHCP_ENTRIES_QNT] = DHCP_ENTRIES;
static dhcp_config_t dhcp_config = DHCP_CONFIG;


/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static err_t netif_init_cb(struct netif *netif);
static err_t linkoutput_fn(struct netif *netif, struct pbuf *p);
static err_t output_fn(struct netif *netif, struct pbuf *p, ip_addr_t *ipaddr);
static bool dns_query_proc(const char *name, ip_addr_t *addr);
static uint16_t ssi_handler(int index, char *insert, int ins_len);


/* Private variables ---------------------------------------------------------*/
static const uint8_t localipaddr[4]  = IPADDR;

static bool dns_query_proc(const char *name, ip_addr_t *addr)
{
  if (
		  strcmp(name, "run.stm") == 0 ||
		  strcmp(name, "www.run.stm") == 0
		  )
  {
    addr->addr = *(uint32_t *)localipaddr;
    return true;
  }
  return false;
}


static void init_lwip()
{
//	PRINTF("init_lwip start\n");


  lwip_init();
   //stmr_add(&tcp_timer);
//	PRINTF("init_lwip done\n");
}



static void init_dnserv(void)
{
	uint8_t ipaddr [4] = IPADDR;
	while (dnserv_init(PADDR(ipaddr), 53, dns_query_proc) != ERR_OK)
		;
}

static void init_dhserv(void)
{
  while (dhserv_init(& dhcp_config) != ERR_OK)
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
#endif






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
        res = local_snprintf_P(insert, ins_len, "%u", (unsigned)++ ttt);
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


void network_initialize(void)
{
	  init_lwip();
	  init_netif();
	  init_dhserv();
	  init_dnserv();
	  init_htserv();
	  //echo_init();

}

#else /*  WITHLWIP */

void network_initialize(void)
{

}

#endif /*  WITHLWIP */
