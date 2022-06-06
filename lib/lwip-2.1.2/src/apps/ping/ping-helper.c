/*
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
 * This file was derived from the lwIP TCP/IP stack.
 * 
 */
#include <string.h>
#include <stdbool.h>
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/inet_chksum.h"
#include "lwip/prot/ip4.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "ping-helper.h"

#ifndef LWIP_RAW
#error "LWIP_RAW must be activated in lwipopts.h"
#endif /* LWIP_RAW */

/**
 * PING_DEBUG: Enable debugging for PING.
 */
#ifndef PING_DEBUG
#define PING_DEBUG     LWIP_DBG_ON
#endif

/** ping delay - in milliseconds */
#ifndef PING_MAX_RECEIVE_TIMEOUT
#define PING_MAX_RECEIVE_TIMEOUT     1000
#endif

/** ping identifier - must fit on a u16_t */
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 16
#endif

/* ping variables */
static u16_t ping_seq_num;
static u32_t ping_start_time;
static struct raw_pcb *ping_pcb;
static ip_addr_t ping_target_ip;
static bool ping_is_done = false;
static bool ping_is_requested = false;
static bool ping_result_eaten = false;
static ping_result_t current_ping_result;


static void ping_timeout(void *arg);

/** Prepare a echo ICMP request */
static void ping_prepare_echo(struct icmp_echo_hdr *iecho, u16_t len) {
	size_t i;
	size_t data_len = len - sizeof(struct icmp_echo_hdr);

	ICMPH_TYPE_SET(iecho, ICMP_ECHO);
	ICMPH_CODE_SET(iecho, 0);
	iecho->chksum = 0;
	iecho->id = PING_ID;
	iecho->seqno = htons(++ping_seq_num);

	/* fill the additional data buffer with some data */
	for (i = 0; i < data_len; i++) {
		((char*) iecho)[sizeof(struct icmp_echo_hdr) + i] = (char) i;
	}

	//this is.. weird.
	//if we calculate the checksum here, the hardware will send out an actually wrong ICMP checksum (zero).
	//it seems like it (the hardware) overwrites or XORs the memory with its calculated checksum
	//thus we must let the checksum be ZERO. Hardware will calculate it in the last second.
	//no idea where this is turned on/off, but otherwise this doesn't work
	//(router rejects ICMP packets with a bad checksum -- use tcpdump to check).
	iecho->chksum = inet_chksum(iecho, len);
	//printf("Checksum: %04x\n", (unsigned) iecho->chksum);
}


/* Ping using the raw ip */
static u8_t ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p,
		const ip_addr_t *addr) {
	struct icmp_echo_hdr *iecho;
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(pcb);
	LWIP_UNUSED_ARG(addr);
	LWIP_ASSERT("p != NULL", p != NULL);

	/* cast to IP ehader */
	struct ip_hdr* ip_hdr = (struct ip_hdr*) p->payload;
	current_ping_result.response_ip.addr = ip_hdr->src.addr;
	uint32_t ping_time_ms = (uint32_t)(sys_now()-ping_start_time);
	current_ping_result.response_time_ms = ping_time_ms;

	/* forward IP header to ICMP header */
	if (pbuf_header(p, -PBUF_IP_HLEN) == 0) {
		iecho = (struct icmp_echo_hdr *) p->payload;

		if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) {

			switch (ICMPH_TYPE(iecho)) {
            case ICMP_ER:
            	current_ping_result.result_code = PING_RES_ECHO_REPLY;
                break;
            case ICMP_DUR:
            	current_ping_result.result_code = PING_RES_DESTINATION_UNREACHABLE;
                break;
            case ICMP_TE:
            	current_ping_result.result_code = PING_RES_TIME_EXCEEDED;
                //iecho->code has more info
                break;
            default:
            	current_ping_result.result_code = PING_RES_UNHANDLED_ICMP_CODE;
                break;
            }

			LWIP_DEBUGF(PING_DEBUG, ("ping: recv "));
			ip_addr_debug_print(PING_DEBUG, addr);
			LWIP_DEBUGF(PING_DEBUG, (" %"U32_F" ms\n", ping_time_ms));

			//do not call into timeout function anymore!
			sys_untimeout(ping_timeout, ping_pcb);

			/* do some ping result processing */
			pbuf_free(p);

			//free raw PCB again, action is over
			raw_remove(ping_pcb);
			ping_pcb = NULL;

			current_ping_result.result_code = PING_RES_ECHO_REPLY;

			ping_is_done = true;
			ping_is_requested = false;
			return 1; /* eat the packet */
		} else {
			/* this might have been a ping request from someone else. wait for an eventual next packet. */
			current_ping_result.result_code = PING_RES_ID_OR_SEQNUM_MISMATCH;
			//rewind
			pbuf_header(p, PBUF_IP_HLEN);
		}
	}
	return 0; /* don't eat the packet */
}

static int ping_send(struct raw_pcb *raw, ip_addr_t *addr) {
	struct pbuf *p;
	struct icmp_echo_hdr *iecho;
	size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
	int send_err = 0;

	LWIP_DEBUGF(PING_DEBUG, ("ping: send "));
	ip_addr_debug_print(PING_DEBUG, addr);
	LWIP_DEBUGF(PING_DEBUG, ("\n"));
	LWIP_ASSERT("ping_size <= 0xffff", ping_size <= 0xffff);

	p = pbuf_alloc(PBUF_IP, (u16_t) ping_size, PBUF_RAM);
	if (!p) {
		return PING_ERR_NO_MEM;
	}
	if ((p->len == p->tot_len) && (p->next == NULL)) {
		iecho = (struct icmp_echo_hdr *) p->payload;

		ping_prepare_echo(iecho, (u16_t) ping_size);
		//hexDump("ICMP payload", p->payload, p->tot_len);

		send_err = raw_sendto(raw, p, addr);
		ping_start_time = sys_now();
	}
	pbuf_free(p);
	return send_err == ERR_OK ? PING_ERR_OK : PING_ERR_SEND_FAILED;
}

static void ping_timeout(void *arg) {
	struct raw_pcb *pcb = (struct raw_pcb*) arg;
	LWIP_ASSERT("ping_timeout: no pcb given!", pcb != NULL);

	if(ping_is_requested) {
		if(!ping_is_done) {
			uint32_t ping_time_ms = (uint32_t)(sys_now()-ping_start_time);
			printf("Ping timed out\n");

			/* globally remove PCB */
			if(ping_pcb != NULL) {
				raw_remove(ping_pcb);
				ping_pcb = NULL;
			}

			current_ping_result.response_time_ms = ping_time_ms;
			current_ping_result.result_code = PING_RES_TIMEOUT;
			ping_is_done = true;
			ping_is_requested = false;
		}
	}
}

static int ping_raw_init(ip_addr_t target) {
	/* set target */
	ping_target_ip = target;
	/* allocate new raw pcb for ICMP protocol */
	ping_pcb = raw_new(IP_PROTO_ICMP);
	if(ping_pcb == NULL) {
		return PING_ERR_NO_MEM;
	}
	/* set receive function */
	raw_recv(ping_pcb, ping_recv, NULL);
	/* bind on all IP addresses */
	if(raw_bind(ping_pcb, IP_ADDR_ANY) != ERR_OK) {
		return PING_ERR_BIND_FAILED;
	}
	/* use LWIP system timers to check for timeout*/
	sys_timeout(PING_MAX_RECEIVE_TIMEOUT, ping_timeout, ping_pcb);
	return PING_ERR_OK;
}

int ping_send_now(void) {
	LWIP_ASSERT("ping_pcb != NULL", ping_pcb != NULL);
	return ping_send(ping_pcb, &ping_target_ip);
}

int ping_ip(ip_addr_t ping_addr) {
	int ret = 0;
	if(ping_is_requested) {
		return PING_ERR_PING_STILL_IN_PROGRESS;
	}
	ret = ping_raw_init(ping_addr);
	if(ret)
		return ret;
	ret = ping_send_now();
	if(ret == PING_ERR_OK) {
		memset(&current_ping_result, 0, sizeof(current_ping_result));
		ping_is_requested = true;
		ping_is_done = false;
		ping_result_eaten = false;
	}
	return ret;
}

int ping_ip_result(ping_result_t *res) {
	if(!res) {
		return PING_ERR_INVALID_ARG;
	}
	if(!ping_is_done) {
		return PING_ERR_PING_STILL_IN_PROGRESS;
	}
	if(!ping_result_eaten) {
		//the thing is done
		*res = current_ping_result;
		ping_result_eaten = true;
		return PING_ERR_OK;
	}
	return PING_ERR_RESULT_ALREADY_CONSUMED;
}
