/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Simon Goldschmidt
 *
 */
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include "hardware.h"
#include "formats.h"
#include <stdint.h>
#include <inttypes.h>

void board_update_time(uint32_t sec);

/* Core OS & API settings */
#define NO_SYS                          1
#define LWIP_PROVIDE_ERRNO              1
#define LWIP_RAW                        1
#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0
#define LWIP_COMPAT_SOCKETS             0

/* Memory & Pool settings */
#define MEM_SIZE                        (4 * 32768)
#define MEM_ALIGNMENT                   64
#define MEMP_SEPARATE_POOLS             1
#define MEMP_NUM_PBUF                   160
#define MEMP_NUM_UDP_PCB                40
#define MEMP_NUM_TCP_PCB                320
#define MEMP_NUM_TCP_PCB_LISTEN         8
#define MEMP_NUM_SYS_TIMEOUT            8
#define MEMP_NUM_NETBUF                 80
#define MEMP_NUM_NETCONN                160
#define MEMP_NUM_TCPIP_MSG_API          160
#define MEMP_NUM_TCPIP_MSG_INPKT        640
#define MEMP_NUM_FRAG_PBUF              256

#define PBUF_POOL_SIZE                  20480
#define PBUF_POOL_BUFSIZE               17000
#define PBUF_LINK_HLEN                  16

/* ARP & Ethernet settings */
#define LWIP_ARP                        1
#define ARP_TABLE_SIZE                  100
#define ARP_QUEUEING                    1
#define ETHARP_SUPPORT_STATIC_ENTRIES   1
#define LWIP_ETHERNET                   1
#define ETH_PAD_SIZE                    2

/* IP & Protocols */
#define LWIP_IPV4                       1
#define IP_OPTIONS_ALLOWED              0
#define LWIP_ICMP                       1
#define LWIP_UDP                        1
#define LWIP_TCP                        1
#define LWIP_DNS                        1
#define LWIP_TCP_KEEPALIVE              1
#define LWIP_IP_ACCEPT_UDP_PORT(p)      ((p) == PP_NTOHS(67))

/* TCP Buffer & Audio Streaming settings */
#define TCP_MSS                         1460
#define TCP_WND                         (8 * TCP_MSS)
#define TCP_SND_BUF                     (16 * TCP_MSS)
#define TCP_SND_QUEUELEN                (2 * TCP_SND_BUF / TCP_MSS)
#define MEMP_NUM_TCP_SEG                TCP_SND_QUEUELEN
#define TCP_OVERSIZE                    TCP_MSS

/* Essential for thread-safe tcpip_try_callback() calls from Audio Task / DMA ISR */
#define TCPIP_MBOX_SIZE                 32

/* DHCP & Network Interface settings */
#define LWIP_DHCP                       1
#define DHCP_DOES_ARP_CHECK             0
#define LWIP_DHCP_DOES_ACD_CHECK        0
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_NETIF_HOSTNAME             1
#define LWIP_NETIF_LOOPBACK             1

/* SNTP settings */
#define SNTP_SUPPORT                    1
#define SNTP_UPDATE_DELAY               8640000
#define SNTP_DEBUG                      LWIP_DBG_ON
#define SNTP_SET_SYSTEM_TIME(sec)       board_update_time(sec)

/* Statistics & Checksum algorithm */
#define MEM_STATS                       0
#define SYS_STATS                       0
#define MEMP_STATS                      0
#define LINK_STATS                      0
#define LWIP_CHKSUM_ALGORITHM           3


/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT            0
typedef IRQL_t sys_prot_t;

#define HTTPD_FSDATA_FILE               "src/httpd_files/fsdata.txt"
#define LWIP_RAND()                     (hardware_get_random())

#if !WITHISBOOTLOADER
    #define LWIP_HTTPD_CGI              1
    #define LWIP_HTTPD_SSI              1
    #define LWIP_HTTPD_SSI_INCLUDE_TAG  0
    #define LWIP_HTTPD_SUPPORT_POST     1
    #define LWIP_HTTPD_STRNSTR_PRIVATE  0
#endif /* !WITHISBOOTLOADER */

struct netif;
void nic_initialize(struct netif *netif);
void nic_linkspool(void *ctx);
void nic_set_mac(void *ctx);

#define NIC_MTU 1536  // MTU value

#if CPUSTYLE_ALLWINNER

/* Zero-copy support: Synopsys DesignWare / Allwinner EMAC DMA Descriptor Layout */
struct emac_dma_desc {
    volatile uint32_t status;
    volatile uint32_t control;
    volatile uint32_t buf_addr;
    volatile uint32_t next_desc;
    uint32_t pad[(64 - 16) / 4];
};

/* Custom data appended to pbuf_t */
typedef struct listsupport_tag
{
    void *sign1;
    LIST_ENTRY item __ALIGNED(4);
    struct emac_dma_desc dmadesc __ALIGNED(DCACHEROWSIZE);
    int dummyAllign __ALIGNED(DCACHEROWSIZE);
    void *sign2;
} listsupport_t;

#define LWIP_PBUF_CUSTOM_DATA           listsupport_t custom_item;

#define LWIP_RAM_HEAP_POINTER           lwipBuffer
extern uint8_t LWIP_RAM_HEAP_POINTER[MEM_SIZE];

#else
    /* Other CPUs */
    #define CHECKSUM_GEN_IP             1
    #define CHECKSUM_GEN_UDP            1
    #define CHECKSUM_GEN_TCP            1
    #define CHECKSUM_GEN_ICMP           1
    #define CHECKSUM_GEN_ICMP6          1
    #define CHECKSUM_CHECK_IP           1
    #define CHECKSUM_CHECK_UDP          1
    #define CHECKSUM_CHECK_TCP          1
    #define CHECKSUM_CHECK_ICMP         1
    #define CHECKSUM_CHECK_ICMP6        1
#endif /* CPUSTYLE_ALLWINNER */


//#define LWIP_DEBUG                     1
/* Debug settings */
#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_ALL
//#define LWIP_DBG_TYPES_ON               (LWIP_DBG_TRACE | LWIP_DBG_STATE | LWIP_DBG_FRESH | LWIP_DBG_HALT)
/* Debug settings */
#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_ALL
//#define LWIP_DBG_TYPES_ON               (LWIP_DBG_TRACE | LWIP_DBG_STATE | LWIP_DBG_FRESH | LWIP_DBG_HALT)
#define LWIP_DBG_TYPES_ON           	(LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)

//#define HTTPD_DEBUG           LWIP_DBG_ON
//#define ETHARP_DEBUG           LWIP_DBG_ON
//#define NETIF_DEBUG            LWIP_DBG_ON
#define PBUF_DEBUG               LWIP_DBG_ON
//#define API_LIB_DEBUG          LWIP_DBG_ON
//#define API_MSG_DEBUG          LWIP_DBG_ON
//#define SOCKETS_DEBUG          LWIP_DBG_ON
//#define ICMP_DEBUG             LWIP_DBG_ON
//#define IGMP_DEBUG             LWIP_DBG_ON
//#define INET_DEBUG             LWIP_DBG_ON
//#define IP_DEBUG               LWIP_DBG_ON
//#define IP_REASS_DEBUG         LWIP_DBG_ON
//#define RAW_DEBUG              LWIP_DBG_ON
#define MEM_DEBUG              LWIP_DBG_ON
#define MEMP_DEBUG             LWIP_DBG_ON
//#define SYS_DEBUG              LWIP_DBG_ON
//#define TIMERS_DEBUG           LWIP_DBG_ON
//#define TCP_DEBUG              LWIP_DBG_ON
//#define TCP_INPUT_DEBUG        LWIP_DBG_ON
//#define TCP_FR_DEBUG           LWIP_DBG_ON
//#define TCP_RTO_DEBUG          LWIP_DBG_ON
//#define TCP_CWND_DEBUG         LWIP_DBG_ON
//#define TCP_WND_DEBUG          LWIP_DBG_ON
//#define TCP_OUTPUT_DEBUG       LWIP_DBG_ON
//#define TCP_RST_DEBUG          LWIP_DBG_ON
//#define TCP_QLEN_DEBUG         LWIP_DBG_ON
//#define UDP_DEBUG              LWIP_DBG_ON
//#define TCPIP_DEBUG            LWIP_DBG_ON
//#define SLIP_DEBUG             LWIP_DBG_ON
//#define DHCP_DEBUG             LWIP_DBG_ON
//#define AUTOIP_DEBUG           LWIP_DBG_ON
//#define DNS_DEBUG              LWIP_DBG_ON
//#define IP6_DEBUG              LWIP_DBG_ON

#define SNTP_SET_SYSTEM_TIME(sec) board_update_time(sec)

#endif /* __LWIPOPTS_H__ */
