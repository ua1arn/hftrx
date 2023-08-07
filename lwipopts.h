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

#if 0 // old settings

#if 0//WITHISBOOTLOADER
	/* Prevent having to link sys_arch.c (we don't test the API layers in unit tests) */
	#define NO_SYS                          1
	#define LWIP_RAW                        1
	#define LWIP_NETCONN                    0
	#define LWIP_SOCKET                     0
#if ARM_STM32MP1_LFBGA354_CTLSTYLE_PANGUBOARD_V1P2_H_INCLUDED
	#define LWIP_DHCP                       1
#endif /* ARM_STM32MP1_LFBGA354_CTLSTYLE_PANGUBOARD_V1P2_H_INCLUDED */
	#define LWIP_ICMP                       1
	#define LWIP_UDP                        1
	#define LWIP_TCP                        1
	//#define LWIP_HAVE_SLIPIF				1
	#define LWIP_IP_ACCEPT_UDP_PORT(p)      ((p) == PP_NTOHS(67))

	#define TCP_MSS                         (1500 /*mtu*/ - 20 /*iphdr*/ - 20 /*tcphhr*/)
	#define TCP_SND_BUF                     (2 * TCP_MSS)

	#define ETHARP_SUPPORT_STATIC_ENTRIES   1

#if WITHISBOOTLOADER

	#define LWIP_HTTPD_CGI                  1
	#define LWIP_HTTPD_SSI                  1
	#define LWIP_HTTPD_SSI_INCLUDE_TAG      0
	#define LWIP_HTTPD_SUPPORT_POST			1

#endif
	#define LWIP_NETIF_LOOPBACK 1
	#define LWIP_HTTPD_STRNSTR_PRIVATE		0


#else /* WITHISBOOTLOADER */
	/* Prevent having to link sys_arch.c (we don't test the API layers in unit tests) */
	#define NO_SYS                          1
	#define NO_SYS_NO_TIMERS                1
	#define LWIP_RAW                        1
	#define LWIP_NETCONN                    0
	#define LWIP_SOCKET                     0
#if 1//ARM_STM32MP1_LFBGA354_CTLSTYLE_PANGUBOARD_V1P2_H_INCLUDED
	#define LWIP_DHCP                       1
#endif /* ARM_STM32MP1_LFBGA354_CTLSTYLE_PANGUBOARD_V1P2_H_INCLUDED */
	#define LWIP_ICMP                       1
	#define LWIP_UDP                        1
	#define LWIP_TCP                        1
	//#define LWIP_HAVE_SLIPIF				1
	#define LWIP_IP_ACCEPT_UDP_PORT(p)      ((p) == PP_NTOHS(67))

	#define TCP_MSS                         (1500 /*mtu*/ - 20 /*iphdr*/ - 20 /*tcphhr*/)
	#define TCP_SND_BUF                     (2 * TCP_MSS)

	#define ETHARP_SUPPORT_STATIC_ENTRIES   1

#if 1//WITHHTTPUPDATE || WITHISBOOTLOADER

	#define LWIP_HTTPD_CGI                  1
	#define LWIP_HTTPD_SSI                  1
	#define LWIP_HTTPD_SSI_INCLUDE_TAG      0
	#define LWIP_HTTPD_SUPPORT_POST			1

#endif /* WITHHTTPUPDATE || WITHISBOOTLOADER */

	#define LWIP_NETIF_LOOPBACK 1
	#define LWIP_HTTPD_STRNSTR_PRIVATE		0

	//#define LWIP_AUTOIP 1

#endif	/* WITHISBOOTLOADER */

#define UDP_TTL 64
#define ETH_PAD_SIZE                    64

#if 1
	#define LWIP_RAM_HEAP_POINTER		lwipBuffer
	#define MEM_SIZE                        (16384 * 1024uL)
	extern uint8_t LWIP_RAM_HEAP_POINTER [MEM_SIZE];
#endif

#define MEM_ALIGNMENT                   8

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT 1
typedef unsigned sys_prot_t;

//#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 1
//#define MEMP_MEM_MALLOC 1

//#define PBUF_POOL_SIZE 1024	// 512 for 400 k is okay
//
//#define X32_F "08X"
//#define S32_F "ld"
//#define U32_F "lu"
//
//#define X16_F "04X"
//#define S16_F "d"
//#define U16_F "u"

#define LWIP_DEBUG             1
#define LWIP_DBG_MIN_LEVEL     LWIP_DBG_LEVEL_ALL
#define LWIP_DBG_TYPES_ON      (LWIP_DBG_TRACE | LWIP_DBG_STATE | LWIP_DBG_FRESH | LWIP_DBG_HALT)

//#define HTTPD_DEBUG           LWIP_DBG_ON
//#define ETHARP_DEBUG           LWIP_DBG_ON
#define NETIF_DEBUG            LWIP_DBG_ON
//#define PBUF_DEBUG             LWIP_DBG_ON
#define API_LIB_DEBUG          LWIP_DBG_ON
#define API_MSG_DEBUG          LWIP_DBG_ON
#define SOCKETS_DEBUG          LWIP_DBG_ON
#define ICMP_DEBUG             LWIP_DBG_ON
#define IGMP_DEBUG             LWIP_DBG_ON
#define INET_DEBUG             LWIP_DBG_ON
#define IP_DEBUG               LWIP_DBG_ON
#define IP_REASS_DEBUG         LWIP_DBG_ON
//#define RAW_DEBUG              LWIP_DBG_ON
//#define MEM_DEBUG              LWIP_DBG_ON
//#define MEMP_DEBUG             LWIP_DBG_ON
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
#define UDP_DEBUG              LWIP_DBG_ON
#define TCPIP_DEBUG            LWIP_DBG_ON
#define SLIP_DEBUG             LWIP_DBG_ON
#define DHCP_DEBUG             LWIP_DBG_ON
#define AUTOIP_DEBUG           LWIP_DBG_ON
#define DNS_DEBUG              LWIP_DBG_ON
#define IP6_DEBUG              LWIP_DBG_ON

void display_vtty_printf(const char * format, ...);
//#define LWIP_PLATFORM_DIAG(mmsg) do { PRINTF mmsg; } while (0)
#define LWIP_PLATFORM_DIAG(mmsg) do { PRINTF mmsg; /*display_vtty_printf mmsg; */} while (0)

#endif /* old settings */

void board_update_time(uint32_t sec);

#define LWIP_RAW                    1
#define NO_SYS 						1
#define LWIP_SOCKET 				0
#define LWIP_COMPAT_SOCKETS			0
#define LWIP_NETCONN 				0

//#define NO_SYS_NO_TIMERS 			1

#define LWIP_TCP_KEEPALIVE 			0

#define MEM_SIZE                    131072
#define MEM_ALIGNMENT               64 //8
#define MEMP_NUM_PBUF 				16
#define MEMP_NUM_PBUF 				16
#define MEMP_NUM_UDP_PCB 			4
#define MEMP_NUM_TCP_PCB 			32
#define MEMP_NUM_TCP_PCB_LISTEN 	8
#define MEMP_NUM_TCP_SEG 			256
#define MEMP_NUM_SYS_TIMEOUT 		8
#define MEMP_NUM_NETBUF 			8
#define MEMP_NUM_NETCONN 			16
#define MEMP_NUM_TCPIP_MSG_API 		16
#define MEMP_NUM_TCPIP_MSG_INPKT 	64

#define MEMP_NUM_SYS_TIMEOUT 		8
#define PBUF_POOL_SIZE 				2048
#define PBUF_POOL_BUFSIZE 			1700
#define PBUF_LINK_HLEN 				16

#define ARP_TABLE_SIZE 				10
#define ARP_QUEUEING 				1

#define IP_OPTIONS 					0
#define IP_FORWARD 					0
#define IP_REASSEMBLY 				1
#define IP_FRAG 					1
#define IP_REASS_MAX_PBUFS 			128
#define IP_FRAG_MAX_MTU 			1500
#define IP_DEFAULT_TTL 				255
#define LWIP_CHKSUM_ALGORITHM 		3

#define CONFIG_LINKSPEED_AUTODETECT 1
#define LWIP_TCP_KEEPALIVE 			0

#define LWIP_UDP 					1
#define UDP_TTL 					55

#define LWIP_TCP 					1
#define TCP_MSS 					1460
#define TCP_SND_BUF 				8192
#define TCP_WND 					2048
#define TCP_TTL 					255
#define TCP_MAXRTX 					12
#define TCP_SYNMAXRTX				4
#define TCP_QUEUE_OOSEQ 			1
#define TCP_SND_QUEUELEN   			16 * TCP_SND_BUF/TCP_MSS
#define CHECKSUM_GEN_TCP 			0
#define CHECKSUM_GEN_UDP 			0
#define CHECKSUM_GEN_IP  			0
#define CHECKSUM_CHECK_TCP  		0
#define CHECKSUM_CHECK_UDP  		0
#define CHECKSUM_CHECK_IP 			0
#define LWIP_FULL_CSUM_OFFLOAD_RX	1
#define LWIP_FULL_CSUM_OFFLOAD_TX	1

#define MEMP_SEPARATE_POOLS 		1
#define MEMP_NUM_FRAG_PBUF 			256
#define IP_OPTIONS_ALLOWED 			0
#define TCP_OVERSIZE 				CP_MSS

#define LWIP_DHCP 					1
#define DHCP_DOES_ARP_CHECK 		1

#define SNTP_SUPPORT      			1
#define SNTP_UPDATE_DELAY 			8640000
#define SNTP_DEBUG                  LWIP_DBG_ON

//#define LWIP_DEBUG             		1
//#define LWIP_DBG_MIN_LEVEL     		LWIP_DBG_LEVEL_ALL
//#define LWIP_DBG_TYPES_ON      		(LWIP_DBG_TRACE | LWIP_DBG_STATE | LWIP_DBG_FRESH | LWIP_DBG_HALT)

#define SNTP_SET_SYSTEM_TIME(sec) board_update_time(sec)

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT 1
typedef unsigned sys_prot_t;

#define ETH_RX_BUFFER_SIZE 32768

#endif /* __LWIPOPTS_H__ */
