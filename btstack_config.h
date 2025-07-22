//
// btstack_config.h for STM32 F4 Discovery + USB Bluetooth Dongle port
//
// Documentation: https://bluekitchen-gmbh.com/btstack/#how_to/
//

#include "hardware.h"
#include "formats.h"

#ifndef BTSTACK_CONFIG_H
#define BTSTACK_CONFIG_H

#define HAVE_ASSERT
#define btstack_assert(condition)  do {ASSERT(condition);} while (0)
#define ENABLE_LOG_DEBUG

// Port related features
//#define HAVE_BTSTACK_STDIN
#define HAVE_EMBEDDED_TIME_MS
//#define HAVE_HAL_AUDIO
//#define HAVE_HAL_AUDIO_SINK_STEREO_ONLY
//#define HAVE_BTSTACK_AUDIO_EFFECTIVE_SAMPLERATE

// BTstack features that can be enabled
//#define ENABLE_BLE
////#define ENABLE_BTSTACK_ASSERT
#define ENABLE_CC256X_BAUDRATE_CHANGE_FLOWCONTROL_BUG_WORKAROUND
#define ENABLE_CLASSIC
#define ENABLE_HFP_WIDE_BAND_SPEECH
//#define ENABLE_LE_CENTRAL
//#define ENABLE_L2CAP_LE_CREDIT_BASED_FLOW_CONTROL_MODE
//#define ENABLE_LE_PERIPHERAL
#define ENABLE_LOG_INFO
#define ENABLE_LOG_ERROR
#define ENABLE_PRINTF_HEXDUMP
#define ENABLE_SCO_OVER_HCI
//#define ENABLE_SCO_STEREO_PLAYBACK
//#define ENABLE_SEGGER_RTT

// BTstack configuration. buffers, sizes, ...
#define HCI_ACL_PAYLOAD_SIZE (1691 + 4)
#define HCI_INCOMING_PRE_BUFFER_SIZE 14 // sizeof BNEP header, avoid memcpy
#define HCI_OUTGOING_PRE_BUFFER_SIZE  4

#define MAX_NR_AVDTP_CONNECTIONS 1
#define MAX_NR_AVDTP_STREAM_ENDPOINTS 1
#define MAX_NR_AVRCP_CONNECTIONS 2
#define MAX_NR_BNEP_CHANNELS 1
#define MAX_NR_BNEP_SERVICES 1
#define MAX_NR_BTSTACK_LINK_KEY_DB_MEMORY_ENTRIES  2
#define MAX_NR_GATT_CLIENTS 1
#define MAX_NR_HCI_CONNECTIONS 2
#define MAX_NR_HFP_CONNECTIONS 1
#define MAX_NR_HID_HOST_CONNECTIONS 1
#define MAX_NR_HIDS_CLIENTS 1
#define MAX_NR_L2CAP_CHANNELS  8 // was: 4
#define MAX_NR_L2CAP_SERVICES  5 // was: 3
#define MAX_NR_RFCOMM_CHANNELS 6	// was: 1
#define MAX_NR_RFCOMM_MULTIPLEXERS 1
#define MAX_NR_RFCOMM_SERVICES 4	// was: 1
#define MAX_NR_SERVICE_RECORD_ITEMS 6		// was: 4
#define MAX_NR_SM_LOOKUP_ENTRIES 3
#define MAX_NR_WHITELIST_ENTRIES 1

// Link Key DB and LE Device DB using TLV on top of Flash Sector interface
#define NVM_NUM_DEVICE_DB_ENTRIES 4
#define NVM_NUM_LINK_KEYS 4

#endif
