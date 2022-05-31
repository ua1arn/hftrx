#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "ping-helper.h"
#include <string.h>

extern struct netif *echo_netif;

void start_ping_action(void * arg)
{
	//get gateway IP from global net interface
	ip_addr_t gw_addr = echo_netif->gw;
	uint8_t gw_ip_part_1 = ip4_addr1(&gw_addr);
	//check if DHCP already succeeded by checking against non 0 ip part
	int ret = 0;
	if(gw_ip_part_1 != 0) {
		ip_addr_t target_ping_ip; // = gw_addr;
		//select target:
		//gateway
		//target_ping_ip = gw_addr;
		//static IPs
		//IP_ADDR4(&target_ping_ip, 192,168,1,180);
		ipaddr_aton("192.168.1.111", & target_ping_ip);

		PRINTF("Starting to ping IP: %d.%d.%d.%d.\n", (int)ip4_addr1(&target_ping_ip),
				(int)ip4_addr2(&target_ping_ip), (int)ip4_addr3(&target_ping_ip),
				(int)ip4_addr4(&target_ping_ip));
		if((ret = ping_ip(target_ping_ip)) != PING_ERR_OK) {
			PRINTF("Error while sending ping: %d\n", ret);
		}
	}
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
	} else {
		//printf("No ping result available yet: %d\n", retcode);
	}
	sys_timeout(100, check_ping_result, NULL);
}

