#include "ping-helper.h"
#include <string.h>

extern struct netif * netif;

int ping_send_ip(const char * ip_str)
{
	ip_addr_t target_ping_ip;
	int res = ipaddr_aton(ip_str, & target_ping_ip);

	if (! res) 					// invalid conversion string to IP address
		return -10;

	ip_addr_t gw_addr = netif->gw;
	if (! ip4_addr1(& gw_addr)) // gateway not assigned by DHCP
		return -11;

	res = ping_ip(target_ping_ip);
	if (res != PING_ERR_OK)		// error while sending ICMP request
		return res;

	return PING_ERR_OK;
}

int ping_check_response(void)
{
	ping_result_t res;
	memset(& res, 0, sizeof(res));
	int retcode = ping_ip_result(& res);

	if(retcode == PING_ERR_OK)
	{
		if (res.result_code == PING_RES_ECHO_REPLY)
		{
			return res.response_time_ms;
		}
	}
	return retcode;
}

void start_ping_action(void * arg)
{
	const char str[] = "8.8.8.8";
	int ret = ping_send_ip(str);
	PRINTF("Starting to ping IP: %s\n", str);
	if(ret != PING_ERR_OK)
		PRINTF("Error while sending ping: %d\n", ret);

	//every 4 seconds, start a new ping attempt
	sys_timeout(4000, start_ping_action, NULL);
}

void check_ping_result(void * arg)
{
	ping_result_t res;
	memset(&res, 0, sizeof(res));
	int retcode = ping_ip_result(&res);
	if(retcode == PING_ERR_OK) {
        if (res.result_code == PING_RES_ECHO_REPLY) {
        	PRINTF("Good ping from %s %u ms\n", ipaddr_ntoa(&res.response_ip),
                    (unsigned) res.response_time_ms);
        } else {
        	PRINTF("Bad ping err %d\n", res.result_code);
        }
	}

	sys_timeout(100, check_ping_result, NULL);
}

