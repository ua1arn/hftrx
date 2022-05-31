#ifndef LWIP_PING_H
#define LWIP_PING_H

#include "lwip/ip_addr.h"

typedef enum {
    PING_RES_TIMEOUT,               /* no response received in time */
    PING_RES_ID_OR_SEQNUM_MISMATCH, /* response ID or sequence number mismatched */
    PING_RES_ECHO_REPLY,            /* ping answer received */
    PING_RES_DESTINATION_UNREACHABLE, /* destination unreachable received */
    PING_RES_TIME_EXCEEDED,         /* for TTL to low or time during defrag exceeded (see wiki) */
    PING_RES_UNHANDLED_ICMP_CODE,   /* for all ICMP types which are not specifically handled */
} ping_result_code;

typedef struct {
    ping_result_code result_code;
    u32_t response_time_ms;
    ip_addr_t response_ip;
} ping_result_t;

#define PING_ERR_OK 0							/* no error */
#define PING_ERR_NO_MEM -1						/* out of memory */
#define PING_ERR_BIND_FAILED -2					/* raw_bind failed */
#define PING_ERR_NO_PING_REQUESTED -3			/* requested ping result without requesting ping */
#define PING_ERR_PING_STILL_IN_PROGRESS -4		/* requested ping still in progress */
#define PING_ERR_INVALID_ARG -5					/* invalid function argument */
#define PING_ERR_RESULT_ALREADY_CONSUMED -6		/* result was already retrieved */
#define PING_ERR_SEND_FAILED -7					/* raw_sendto failed */

/**
 * Starts a ping.
 * @param ping_addr target IP address to ping
 * @return 0 on success, negative on error
 */
int ping_ip(ip_addr_t ping_addr);

/**
 * Checks for the result of a ping.
 * If the result is PING_ERR_OK, the ping result is saved into the memory pointed to by res.
 * Call this function periodically or after some time ti retrieve the result.
 * @param res result memory
 * @return 0 on success, negative on error
 */
int ping_ip_result(ping_result_t *res);

#endif /* LWIP_PING_H */