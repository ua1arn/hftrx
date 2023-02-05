/*
 * tusb_config.h
 *
 *  Created on: May 5, 2021
 *      Author: Jeremiah McCarthy
 */

#ifndef TUSB_CONFIG_H_
#define TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hardware.h"
#include "formats.h"


#define CFG_TUSB_MCU 9999999 //OPT_MCU_F1C100S

// Enable Device stack
//#define CFG_TUD_ENABLED       1
#define CFG_TUH_ENABLED 1
#define TUP_DCD_ENDPOINT_MAX    16
#define CFG_TUD_ENDPPOINT_MAX 	5
#define CFG_TUH_ENDPOINT_MAX 	16
#define CFG_TUH_ENDPPOINT_MAX 	16
#define TUP_USBIP_OHCI
#define CFG_TUSB_RHPORT0_MODE OPT_MODE_HOST
//#define BOARD_TUD_MAX_SPEED OPT_MODE_HIGH_SPEED
//#define BOARD_TUH_MAX_SPEED OPT_MODE_HIGH_SPEED
#define CFG_TUH_HUB 1
#define CFG_TUH_MSC	1
#define CFG_TUH_HID	1
#define CFG_TUH_CDC	1
#define BOARD_TUH_RHPORT 1
#define CFG_TUSB_DEBUG 3

#define CFG_TUH_DEVICE_MAX	4
#define CFG_TUH_ENUMERATION_BUFSIZE	2048

//--------------------------------------------------------------------+
// Board Specific Configuration
//--------------------------------------------------------------------+

// RHPort number used for device can be defined by board.mk, default to port 0
#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT      0
#endif

// RHPort max operational speed can defined by board.mk
#ifndef BOARD_TUD_MAX_SPEED
#define BOARD_TUD_MAX_SPEED   OPT_MODE_DEFAULT_SPEED
#endif

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

// defined by compiler flags for flexibility
#ifndef CFG_TUSB_MCU
#error CFG_TUSB_MCU must be defined
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS           OPT_OS_NONE
#endif

#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG        3
#endif

// Default is max speed that hardware controller could support with on-chip PHY
#define CFG_TUD_MAX_SPEED     BOARD_TUD_MAX_SPEED

/* USB DMA on some MCUs can only access a specific SRAM region with restriction on alignment.
 * Tinyusb use follows macros to declare transferring memory so that they can be put
 * into those specific section.
 * e.g
 * - CFG_TUSB_MEM SECTION : __attribute__ (( section(".usb_ram") ))
 * - CFG_TUSB_MEM_ALIGN   : __attribute__ ((aligned(4)))
 */
#ifndef CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_SECTION
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN        __attribute__ ((aligned(4)))
#endif

//--------------------------------------------------------------------
// DEVICE CONFIGURATION
//--------------------------------------------------------------------

#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE    64
#endif

//------------- CLASS -------------//
//#define CFG_TUD_DFU               1

// DFU buffer size, it has to be set to the buffer size used in TUD_DFU_DESCRIPTOR
#define CFG_TUD_DFU_XFER_BUFSIZE  (TUD_OPT_HIGH_SPEED ? 512 : 64)

#ifdef __cplusplus
 }
#endif

#endif /* TUSB_CONFIG_H_ */
