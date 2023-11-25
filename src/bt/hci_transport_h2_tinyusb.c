/*
 * Copyright (C) 2020 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BLUEKITCHEN
 * GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at
 * contact@bluekitchen-gmbh.com
 *
 */

#define BTSTACK_FILE__ "hci_transport_h2_tinyusb.c"

/*
 *  hci_transport_h2_tinyusb.c
 *
 *  HCI Transport API implementation for tinyusb USB Host Stack
 *  https://github.com/hathach/tinyusb
 */

#include "hardware.h"
#include "formats.h"

#if WITHUSEUSBBT && WITHTINYUSB


#include <stddef.h>

#include "hci_transport_h2_tinyusb.h"

#include "bluetooth.h"

#include "btstack_debug.h"
#include "btstack_run_loop.h"

#include "tusb.h"


static void dibgprint(const char * title, const void * p, uint16_t n)
{
#if 0
	PRINTF("%s\n", title);
	printhex(0, p, n);
#endif
}

static  void (*packet_handler)(uint8_t packet_type, uint8_t *packet, uint16_t size) = NULL;

// data source for integration with BTstack Runloop
static btstack_data_source_t transport_data_source;


// higher-layer callbacks
static void (*tuh_packet_sent)(void);
static void (*tuh_packet_received)(uint8_t packet_type, uint8_t * packet, uint16_t size);

static int bth_idx = 0;
static int bth_dev_addr = 1;
static int pdrv = 0;

static void cmd_io_complete(tuh_xfer_t* xfer)
{
	dibgprint("packet sent2 (cmd)", NULL, 0);
	ASSERT(tuh_packet_sent);
  (*tuh_packet_sent)();
}

static void acl_io_complete(tuh_xfer_t* xfer)
{
	dibgprint("packet sent2 (acl)", NULL, 0);
	ASSERT(tuh_packet_sent);
	(*tuh_packet_sent)();
}

void tuh_bluetooth_set_packet_sent(void (*callback)(void)){
    tuh_packet_sent = callback;
}


void tuh_bluetooth_set_packet_received(void (*callback)(uint8_t packet_type, uint8_t * packet, uint16_t size)){
    tuh_packet_received = callback;
}

bool tuh_bluetooth_can_send_now(void){
     return /*st == ST_ALL_READY && */tuh_bth_can_send_now(bth_idx);
}

void tuh_bluetooth_send_cmd(const uint8_t * packet, uint16_t len){
	dibgprint("send_cmd", packet, len);
	tuh_bth_send_cmd(bth_idx, packet, len, cmd_io_complete, 0);
}

void tuh_bluetooth_send_acl(const uint8_t * packet, uint16_t len){
	dibgprint("send_acl", packet, len);
	tuh_bth_send_acl(bth_idx, packet, len, acl_io_complete, 0);
}


// Invoked when received notificaion
void tuh_bth_event_cb(uint8_t idx, uint8_t * buffer, uint16_t size)
{
	dibgprint("packet_received (event)", buffer, size);
	if (tuh_packet_received)
		tuh_packet_received(HCI_EVENT_PACKET, buffer, size);
}

void tuh_bth_rx_acl_cb(uint8_t idx, uint8_t* buffer, uint16_t count)
{
	dibgprint("packet_received (acl)", buffer, count);
	if (tuh_packet_received)
		tuh_packet_received(HCI_ACL_DATA_PACKET, buffer, count);

}

void tuh_bth_send_acl_cb(uint8_t idx)
{
	dibgprint("packet sent (acl)", NULL, 0);
	ASSERT(tuh_packet_sent);
	(*tuh_packet_sent)();
}

static void hci_transport_h2_tinyusb_process(btstack_data_source_t *ds, btstack_data_source_callback_type_t callback_type) {
    switch (callback_type){
        case DATA_SOURCE_CALLBACK_POLL:
			tuh_task();
            break;
        default:
            break;
    }
}

static void hci_transport_h2_tinyusb_block_sent(void) {
    static const uint8_t packet_sent_event[] = {HCI_EVENT_TRANSPORT_PACKET_SENT, 0};
    // notify upper stack that it can send again
    packet_handler(HCI_EVENT_PACKET, (uint8_t *) &packet_sent_event[0], sizeof(packet_sent_event));
}

static void hci_transport_h2_tinyusb_packet_received(uint8_t packet_type, uint8_t * packet, uint16_t size){
    packet_handler(packet_type, packet, size);
}

static void hci_transport_h2_tinyusb_init(const void * transport_config){
    UNUSED(transport_config);
    tuh_bluetooth_set_packet_sent(&hci_transport_h2_tinyusb_block_sent);
    tuh_bluetooth_set_packet_received(&hci_transport_h2_tinyusb_packet_received);
    log_info("hci_transport_h2_tinyusb_init");
}

static int hci_transport_h2_tinyusb_open(void){
    // set up polling data_source
    btstack_run_loop_set_data_source_handler(&transport_data_source, &hci_transport_h2_tinyusb_process);
    btstack_run_loop_enable_data_source_callbacks(&transport_data_source, DATA_SOURCE_CALLBACK_POLL);
    btstack_run_loop_add_data_source(&transport_data_source);
    return 0;
}

static int hci_transport_h2_tinyusb_close(void){
    // remove data source
    btstack_run_loop_disable_data_source_callbacks(&transport_data_source, DATA_SOURCE_CALLBACK_POLL);
    btstack_run_loop_remove_data_source(&transport_data_source);
    return -1;
}

static void hci_transport_h2_tinyusb_register_packet_handler(void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size)){
    packet_handler = handler;
}

static int hci_transport_h2_tinyusb_can_send_now(uint8_t packet_type){
    return tuh_bluetooth_can_send_now();
}

static int hci_transport_h2_tinyusb_send_packet(uint8_t packet_type, uint8_t * packet, int size){
    switch (packet_type){
        case HCI_COMMAND_DATA_PACKET:
            tuh_bluetooth_send_cmd(packet, size);
            return 0;
        case HCI_ACL_DATA_PACKET:
            tuh_bluetooth_send_acl(packet, size);
            return 0;
        default:
            break;
    }
    return -1;
}

static void hci_transport_h2_tinyusb_set_sco_config(uint16_t voice_setting, int num_connections){
    log_info("hci_transport_h2_tinyusb_send_packet, voice 0x%02x, num connections %u", voice_setting, num_connections);
}

const hci_transport_t * hci_transport_h2_tinyusb_instance(uint8_t idx) {

    static const hci_transport_t instance = {
            /* const char * name; */                                        "H4",
            /* void   (*init) (const void *transport_config); */            &hci_transport_h2_tinyusb_init,
            /* int    (*open)(void); */                                     &hci_transport_h2_tinyusb_open,
            /* int    (*close)(void); */                                    &hci_transport_h2_tinyusb_close,
            /* void   (*register_packet_handler)(void (*handler)(...); */   &hci_transport_h2_tinyusb_register_packet_handler,
            /* int    (*can_send_packet_now)(uint8_t packet_type); */       &hci_transport_h2_tinyusb_can_send_now,
            /* int    (*send_packet)(...); */                               &hci_transport_h2_tinyusb_send_packet,
            /* int    (*set_baudrate)(uint32_t baudrate); */                NULL,
            /* void   (*reset_link)(void); */                               NULL,
            /* void   (*set_sco_config)(uint16_t voice_setting, int num_connections); */ &hci_transport_h2_tinyusb_set_sco_config,
    };
    bth_idx = idx;
    return &instance;
}


#endif /* WITHUSEUSBBT && WITHTINYUSB */
