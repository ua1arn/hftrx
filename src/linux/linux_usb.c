/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "radio.h"
#include "formats.h"
#include "board.h"
#include "buffers.h"
#include "audio.h"

#if LINUX_SUBSYSTEM && IQ_VIA_USB

#include "linux_subsystem.h"
#include "common.h"
#include <libusb-1.0/libusb.h>

#define VENDOR_ID           	0x1266
#define PRODUCT_ID          	0x5634
#define ENDPOINT_ISO_IN         0x81
#define ENDPOINT_CMD_IN     	0x83
#define ENDPOINT_CMD_OUT    	0x04

#define NUM_ISO_PACKETS     	8
#define PACKET_SIZE         	DMABUFFSIZE32RX * 4

#define CLIENT_HANDSHAKE_CMD    0x444E5348 	// "HSND"
#define CLIENT_READY_CMD        0x44414552	// "READ"
#define CLIENT_DISCONNECT_CMD   0x43534944	// "DISC"
#define CLIENT_PING_CMD         0x474E4950	// "PING"
#define CLIENT_MODEM_CTRL_CMD   0x4D444F4D	// "MODM"
#define CLIENT_FREQ_RTS_CMD   	0x53545246	// "FRTS"
#define CLIENT_FREQ_FTW_CMD   	0x31575446	// "FTW1"
#define CLIENT_FREQ_FTW2_CMD   	0x32575446	// "FTW2"

static pthread_mutex_t mutex_send;
static struct cond_thread ct_iqready;
pthread_t usbs_t, usbiq_t;
static volatile int exit_requested = 0;

static struct libusb_device_handle * devh = NULL;
unsigned char iso_rx_buffer[PACKET_SIZE * NUM_ISO_PACKETS];
struct libusb_transfer * transfer = NULL;

uint8_t iqbuf_rx[PACKET_SIZE];

volatile static uint8_t rx_fir_shift = 0, rx_cic_shift = 0, tx_shift = 0,
		tx_state = 0, resetn_modem = 1, hw_vfo_sel = 0;
volatile static uint8_t fir_load_rst = 0, iq_test = 0, wnb_state = 0,
		resetn_stream = 0;

// Callback для изохронного приёма данных
static void LIBUSB_CALL transfer_callback(struct libusb_transfer * transfer)
{
	if (transfer->status == LIBUSB_TRANSFER_COMPLETED)
	{
		int num_packets = transfer->num_iso_packets;
		struct libusb_iso_packet_descriptor * pkt;

		for (int i = 0; i < num_packets; i++)
		{
			pkt = & transfer->iso_packet_desc[i];

			if (pkt->status != LIBUSB_TRANSFER_COMPLETED) continue;

			if (pkt->actual_length == PACKET_SIZE)
			{
				int offset = 0;
				for (int j = 0; j < i; j ++)
					offset += transfer->iso_packet_desc[j].length;

				uint8_t * data = (uint8_t *) (transfer->buffer + offset);
				memcpy(iqbuf_rx, data, PACKET_SIZE);
				safe_cond_signal(& ct_iqready);
			}
		}
	} else if (!exit_requested)
		printf("Transfer error: %s\n", libusb_error_name(transfer->status));

	if (exit_requested)
		return;

	int r = libusb_submit_transfer(transfer);
	if (r < 0 && r != LIBUSB_ERROR_BUSY)
		printf("Error resubmitting transfer: %s\n", libusb_error_name(r));
}

int receive_confirmation(uint32_t expected_cmd)
{
	unsigned char buffer[64];
	int actual_length;
	int r;

	if (exit_requested)
		return -1;

	r = libusb_bulk_transfer(devh, ENDPOINT_CMD_IN, buffer, sizeof(buffer),
			& actual_length, 500);
	if (r != 0)
	{
		printf("Failed to receive confirmation: %s\n", libusb_error_name(r));
		return -1;
	}

	if (actual_length != 4)
	{
		printf("Received %d bytes, expected 4\n", actual_length);
		return -1;
	}

	uint32_t received_cmd = * (uint32_t *) buffer;
	if (received_cmd == expected_cmd)
		return 0;
	else
	{
		printf("ACK mismatch: expected 0x%08X, got 0x%08X\n", expected_cmd, received_cmd);
		return -1;
	}
}

void linux_usb_stop(void)
{
	exit_requested = 1;
}

int send_command(uint32_t cmd)
{
	int transferred;
	int r;

	if (! devh || exit_requested)
		return -1;

	pthread_mutex_lock(& mutex_send);

	unsigned char buf[4];
	buf[0] = cmd & 0xFF;
	buf[1] = (cmd >> 8) & 0xFF;
	buf[2] = (cmd >> 16) & 0xFF;
	buf[3] = (cmd >> 24) & 0xFF;

//	printf("Sending %.4s to EP 0x%02x... ", (char *) & cmd, ENDPOINT_CMD_OUT);
	r = libusb_bulk_transfer(devh, ENDPOINT_CMD_OUT, buf, 4, & transferred, 1000);
	if (r < 0)
	{
//		printf("Failed to send: %s\n", libusb_strerror(r));
		goto exit;
	}

	r = receive_confirmation(cmd);
//	printf("%s\n", r ? "ACK" : "ERROR");

exit:
	pthread_mutex_unlock(& mutex_send);
	return r;
}

// without ACK
int send_command2(uint32_t cmd, uint32_t data)
{
	int transferred;
	int r;

	if (! devh || exit_requested)
		return -1;

	pthread_mutex_lock(& mutex_send);

	unsigned char buf[8];
	buf[0] = cmd & 0xFF;
	buf[1] = (cmd >> 8) & 0xFF;
	buf[2] = (cmd >> 16) & 0xFF;
	buf[3] = (cmd >> 24) & 0xFF;

	buf[4] = data & 0xFF;
	buf[5] = (data >> 8) & 0xFF;
	buf[6] = (data >> 16) & 0xFF;
	buf[7] = (data >> 24) & 0xFF;

//	printf("Sending %.4s to EP 0x%02x... ", (char *) & cmd, ENDPOINT_CMD_OUT);
	r = libusb_bulk_transfer(devh, ENDPOINT_CMD_OUT, buf, 8, & transferred, 1000);
	if (r < 0)
	{
//		printf("Failed to send: %s\n", libusb_strerror(r));
		goto exit;
	}

exit:
	pthread_mutex_unlock(& mutex_send);
	return r;
}

void * usb_service_thread(void * args)
{
	struct timeval timeout = { 0, 10000 }; // 10 мс
	time_t last_ping;

	while (! exit_requested)
	{
#if 0
        time_t now = time(NULL);
        if (now - last_ping >= 1)
        {
            send_command(CLIENT_PING_CMD);
            last_ping = now;
        }
#endif

		int r = libusb_handle_events_timeout(NULL, & timeout);
		if (r < 0 && r != LIBUSB_ERROR_INTERRUPTED)
		{
			printf("libusb_handle_events failed: %s\n", libusb_error_name(r));
			break;
		}
	}

	if (transfer)
	{
		libusb_cancel_transfer(transfer);

		for (int i = 0; i < 20; i ++)
			libusb_handle_events_timeout(NULL, & timeout);
	}

	libusb_free_transfer(transfer);
	libusb_release_interface(devh, 0);
	libusb_close(devh);
}

void * usb_iq_thread(void * args)
{
	time_t told = time(NULL);
	int c = 0;

	while (! exit_requested)
	{
		safe_cond_wait(& ct_iqready);

#if 1
		c += DMABUFCLUSTER;

		time_t tnow = time(NULL);
		if (tnow - told >= 1)
		{
			printf("rx iq rate %ld\n", c);
			c = 0;
			told = tnow;
		}
#endif

		uintptr_t addr32rx = allocate_dmabuffer32rx();
		memcpy((uint8_t *) addr32rx, iqbuf_rx, PACKET_SIZE);
		save_dmabuffer32rx(addr32rx);
	}
}

int linux_usb_init(void)
{
	int r = 0;

	r = libusb_init(NULL);
	if (r < 0)
	{
		printf("Failed to initialise libusb: %s\n", libusb_error_name(r));
		return -EXIT_FAILURE;
	}

	devh = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
	if (devh == NULL)
	{
		printf("Could not open device with VID:PID %04x:%04x\n", VENDOR_ID, PRODUCT_ID);
		return -EXIT_FAILURE;
	}

	printf("Resetting USB device %04x:%04x: ", VENDOR_ID, PRODUCT_ID);
	r = libusb_reset_device(devh);
	if (r == 0)
		printf("successful.\n");
	else if (r == LIBUSB_ERROR_NOT_FOUND)
	{
		printf("disconnected during reset.\n");
		return -EXIT_FAILURE;
	} else if (r == LIBUSB_ERROR_ACCESS)
	{
		printf("insufficient permissions to reset device.\n");
		return -EXIT_FAILURE;
	} else
		printf("failed: %s\n", libusb_error_name(r));

	usleep(100000);

	r = libusb_set_auto_detach_kernel_driver(devh, 1);
	if (r < 0)
		printf("Failed to set auto-detach kernel driver: %s\n", libusb_error_name(r));

	r = libusb_claim_interface(devh, 0);
	if (r < 0)
	{
		printf("Error claiming interface 0: %s\n", libusb_error_name(r));
		return -EXIT_FAILURE;
	}
//	printf("Interface 0 claimed.\n");

	r = libusb_set_interface_alt_setting(devh, 0, 0);
	if (r < 0) {
		printf("Error setting alt setting for interface 0: %s\n",
				libusb_error_name(r));
	}
//	else printf("Alt setting 0 for interface 0 set.\n");

#if 1
	struct libusb_config_descriptor * config;
	libusb_device * dev = libusb_get_device(devh);
	r = libusb_get_active_config_descriptor(dev, & config);
	if (r < 0)
	{
		printf("Cannot get config descriptor: %s\n", libusb_strerror(r));
		return -1;
	}

	printf("Device %04x:%04x has %d interfaces\n", VENDOR_ID, PRODUCT_ID, config->bNumInterfaces);

	for (int i = 0; i < config->bNumInterfaces; i ++)
	{
		const struct libusb_interface * interface = & config->interface[i];
		printf("Interface %d has %d altsettings\n", i, interface->num_altsetting);

		for (int j = 0; j < interface->num_altsetting; j ++)
		{
			const struct libusb_interface_descriptor *interface_desc = & interface->altsetting[j];
			printf("  Altsetting %d has %d endpoints\n", j, interface_desc->bNumEndpoints);

			for (int k = 0; k < interface_desc->bNumEndpoints; k ++)
			{
				const struct libusb_endpoint_descriptor * endpoint = & interface_desc->endpoint[k];
				printf("    Endpoint %d: 0x%02x (type: %d, direction: %s)\n", k,
						endpoint->bEndpointAddress, endpoint->bmAttributes & 0x03,
						(endpoint->bEndpointAddress & 0x80) ? "IN" : "OUT");
			}
		}
	}

	libusb_free_config_descriptor(config);
#endif

	printf("Flushing stale BULK IN data...\n");
	unsigned char flush_buf[64];
	int flushed;
	while (libusb_bulk_transfer(devh, ENDPOINT_CMD_IN, flush_buf,
			sizeof(flush_buf), &flushed, 50) == 0 && flushed > 0)
	{
		printf("Flushed %d bytes of stale confirmation data\n", flushed);
	}

	if (send_command(CLIENT_HANDSHAKE_CMD) < 0)
		return -EXIT_FAILURE;
	if (send_command(CLIENT_READY_CMD) < 0)
		return -EXIT_FAILURE;

	transfer = libusb_alloc_transfer(NUM_ISO_PACKETS);
	if (! transfer)
	{
		printf("Could not allocate transfer\n");
		return -EXIT_FAILURE;
	}

	libusb_fill_iso_transfer(transfer, devh, ENDPOINT_ISO_IN, iso_rx_buffer,
			sizeof(iso_rx_buffer), NUM_ISO_PACKETS, transfer_callback, NULL, 0);

	for (int i = 0; i < NUM_ISO_PACKETS; i ++)
		transfer->iso_packet_desc[i].length = PACKET_SIZE;

	r = libusb_submit_transfer(transfer);
	if (r < 0)
	{
		printf("Error submitting transfer: %s\n", libusb_error_name(r));
		return -EXIT_FAILURE;
	}

	linux_init_cond(& ct_iqready);
	pthread_mutex_init(& mutex_send, NULL);

	linux_create_thread(& usbs_t, usb_service_thread, 50, spool_thread_core);

	return EXIT_SUCCESS;
}

void usb_iq_start(void)
{
	linux_create_thread(& usbiq_t, usb_iq_thread, 90, iq_thread_core);

	modem_reset(0);
	usleep(5000);
	modem_reset(1);
}

void update_modem_ctrl(void)
{
	uint32_t v = ((rx_fir_shift & 0xFF) << rx_fir_shift_pos) 	| ((tx_shift & 0xFF) << tx_shift_pos)
			| ((rx_cic_shift & 0xFF) << rx_cic_shift_pos) 		| (!! tx_state << tx_state_pos)
			| (!! resetn_modem << resetn_modem_pos) 			| (!! hw_vfo_sel << hw_vfo_sel_pos)
			| (!! hamradio_get_gadcrand() << adc_rand_pos) 		| (!! iq_test << iq_test_pos)
			| (!! wnb_state << wnb_pos)							| (!! resetn_stream << stream_reset_pos)
			| (!! fir_load_rst << fir_load_reset_pos)			| 0;

	send_command2(CLIENT_MODEM_CTRL_CMD, v);
}

void modem_reset(uint8_t val)
{
	resetn_modem = val;
	update_modem_ctrl();
}

uint8_t iq_shift_fir_rx(uint8_t val) // 52
{
	if (val > 0)
	{
		if (val >= rx_fir_shift_min && val <= rx_fir_shift_max)
			rx_fir_shift = val & 0xFF;

		update_modem_ctrl();
	}
	return rx_fir_shift;
}

uint8_t iq_shift_cic_rx(uint8_t val)
{
	if (val > 0)
	{
		if (val >= rx_cic_shift_min && val <= rx_cic_shift_max)
			rx_cic_shift = val & 0xFF;

		update_modem_ctrl();
	}
	return rx_cic_shift;
}

uint8_t iq_shift_tx(uint8_t val)
{
	if (val > 0)
	{
		if (val >= tx_shift_min && val <= tx_shift_max)
			tx_shift = val & 0xFF;

		update_modem_ctrl();
	}
	return tx_shift;
}

void linux_dds_rts(const uint_least64_t * val)
{
	uint32_t v = *val;
	mirror_ncorts = v;
	send_command2(CLIENT_FREQ_RTS_CMD, v);
}

void linux_dds_ftw(const uint_least64_t * val)
{
	uint32_t v = *val;
	mirror_nco1 = v;
	send_command2(CLIENT_FREQ_FTW_CMD, v);
}

void linux_dds_ftw_sub(const uint_least64_t * val)
{
	uint32_t v = *val;
	mirror_nco2 = v;
#if WITHUSEDUALWATCH
	send_command2(CLIENT_FREQ_FTW2_CMD, v);
#endif /* WITHUSEDUALWATCH */
}

void linux_rxtx_state(uint8_t tx)
{
	tx_state = tx;
	update_modem_ctrl();
}

uint32_t iq_cic_test_process(void)
{
	return 0;
}

void iq_cic_test(uint32_t val)
{
	iq_test = val != 0;
	update_modem_ctrl();
}

#endif /* LINUX_SUBSYSTEM && IQ_VIA_USB */
