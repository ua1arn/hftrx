/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if WITHUSBHW

#include "formats.h"
#include "board.h"
#include "buffers.h"
#include "audio.h"

#include "usb_device.h"
#include "usbd_def.h"
#include "usb200.h"
#include "usbch9.h"
#include "usbd_ctlreq.h"
#include "usbd_ioreq.h"

#include "spi.h"
#include "bootloader.h"

#include <string.h>
#include <wchar.h>

// UAC audio device
// USB\VID_FFFF&PID_0736&REV_0100&MI_00
// USB\VID_FFFF&PID_0736&MI_00

// ACM serial device:
//
// USB\VID_FFFF&PID_0736&REV_0100&MI_03
// USB\VID_FFFF&PID_0736&MI_03

// CDC ECM
//
// USB\Class_02&SubClass_06

// CDC EEM 
//
// USB\Class_02&SubClass_0c&Prot_07
// USB\Class_02&SubClass_0c
// USB\Class_02
//
// http://blog.metrotek.spb.ru/2011/07/07/usb-set-na-cortex-m3/

// See https://community.osr.com/discussion/249588/where-is-the-usb-2-1-specification
//	The value of the bcdUSB field in the standard USB 2.0 Device Descriptor is used to indicate that the device supports the request to read the BOS Descriptor (i.e. GetDescriptor(BOS)). Devices that support the BOS descriptor must have a bcdUSB value of 0201H or larger.
// USB2_LinkPowerMangement_ECN[final].pdf
// See Binary Device Object Store (BOS) Descriptor

#if (USBD_LPM_ENABLED == 1)
	#define USB_FUNCTION_BCD_USB	0x0201	// 0x0201 in ST samples
	//#define USB_FUNCTION_BCD_USB	0x0210	// USB Spec 2.1
#else /* (USBD_LPM_ENABLED == 1) */
	#define USB_FUNCTION_BCD_USB	0x0200
#endif /* (USBD_LPM_ENABLED == 1) */

#if 0
	// ACME Inc. STM32F723E-DISCO UAC2
	#define USB_FUNCTION_VENDOR_ID_OVERRIDE		0xBEEF
	#define USB_FUNCTION_PRODUCT_ID_OVERRIDE	0x5740
#endif
#if 0
	// E1DA Cosmos ADC PCM32/384
	#define USB_FUNCTION_VENDOR_ID_OVERRIDE		0x2FC6
	#define USB_FUNCTION_PRODUCT_ID_OVERRIDE	0x6012
#endif
#if 0
	// REFLEX 2.0 Config 2.0
	#define USB_FUNCTION_VENDOR_ID_OVERRIDE		0x0483
	#define USB_FUNCTION_PRODUCT_ID_OVERRIDE	0xA210
#endif

/* Remove hive in \HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\usbflags\ */

#define USB_FUNCTION_VENDOR_ID	0xFFFF	// Generic
//#define USB_FUNCTION_VENDOR_ID	0x041C	// Altera Corp.
//#define USB_FUNCTION_VENDOR_ID	0x04d9	// Holtek Semiconductor, Inc.
//#define USB_FUNCTION_VENDOR_ID	0x1D6B	// Linux Foundation
//#define USB_FUNCTION_VENDOR_ID	0x08BB	// Texas Instruments Japan (08BBh/2902h)

// From STMicroelectronics Communication Device Class driver (CDC) INF FILE:
//#define USB_FUNCTION_VENDOR_ID	0x0483	// ST (SGS Thomson Microelectronics)
//#define USB_FUNCTION_PRODUCT_ID	0x5740
//#define USB_FUNCTION_RELEASE_NO	0x0200	// bcdDevice

//#define USB_FUNCTION_PRODUCT_ID	0x5732	// PID Need for FT8CN in conjunction with VID 0x0483

/* Слитное добавление L перед открывающей кавыйкой строкового литерала */
#define _LSTRM(_s) L ## _s
#define LSTRM(_s) _LSTRM(_s)

#ifndef WITHBRANDSTR
	//#define WITHBRANDSTR "Falcon"
	#define WITHBRANDSTR "Storch"
#endif

#if WITHISBOOTLOADER

	#define PRODUCTSTR WITHBRANDSTR " Bootloader"
	#define LPRODUCTSTR LSTRM(WITHBRANDSTR) L" Bootloader"
	#define USB_FUNCTION_PRODUCT_ID	0x0754

	#define BUILD_ID 1	// модификатор serial number
	#define USB_FUNCTION_RELEASE_NO	0x0000		// bcdDevice

#elif WITHUSBUAC && WITHUSBUACIN2

	#define PRODUCTSTR WITHBRANDSTR
	#define LPRODUCTSTR LSTRM(WITHBRANDSTR)
	#if WITHUSBUACINOUTRENESAS
		#define USB_FUNCTION_PRODUCT_ID	0x0731
	#elif WITHUSBUACINOUT
		#define USB_FUNCTION_PRODUCT_ID	0x0733
	#elif WITHUSBDFU
		#define USB_FUNCTION_PRODUCT_ID	0x073B
	#else /* WITHUSBDFU */
		#define USB_FUNCTION_PRODUCT_ID	0x0737
	#endif /* WITHUSBDFU */

	#if WITHRTS96
		#define BUILD_ID 6	// модификатор serial number
		#define USB_FUNCTION_RELEASE_NO	0x0106		// bcdDevice
	#elif WITHRTS192
		#define BUILD_ID 5	// модификатор serial number
		#define USB_FUNCTION_RELEASE_NO	0x0105		// bcdDevice
	#else
		#define BUILD_ID 4	// модификатор serial number
		#define USB_FUNCTION_RELEASE_NO	0x0104		// bcdDevice
	#endif
#else /* WITHUSBUAC && WITHUSBUACIN2 */
	#define PRODUCTSTR WITHBRANDSTR
	#define LPRODUCTSTR LSTRM(WITHBRANDSTR)

	#if WITHUSBUACINOUTRENESAS
		#define USB_FUNCTION_PRODUCT_ID	0x0732
	#elif WITHUSBUACINOUT
		#define USB_FUNCTION_PRODUCT_ID	0x0734
	#elif WITHUSBDFU
		#define USB_FUNCTION_PRODUCT_ID	0x073C
	#else /* WITHUSBDFU */
		#define USB_FUNCTION_PRODUCT_ID	0x0738
	#endif /* WITHUSBDFU */

	#if WITHRTS96
		#define BUILD_ID 2	// модификатор serial number
		#define USB_FUNCTION_RELEASE_NO	0x0102		// bcdDevice
	#elif WITHRTS192
		#define BUILD_ID 1	// модификатор serial number
		#define USB_FUNCTION_RELEASE_NO	0x0101		// bcdDevice
	#else
		#define BUILD_ID 0	// модификатор serial number
		#define USB_FUNCTION_RELEASE_NO	0x0100		// bcdDevice
	#endif
#endif /* WITHUSBUAC && WITHUSBUACIN2 */

enum
{
	STRING_ID_0 = 0, /* Language ID */

	STRING_ID_manuf, /* Manufacturer */
	STRING_ID_prod, /* Product */
	STRING_ID_sn, /* SerialNumber */

#if WITHUSBCDCACM
	// USB CDC strings
	STRING_ID_4a, /* Serial Port #%d */
	STRING_ID_4x = (STRING_ID_4a + WITHUSBCDCACM_N - 1),
#endif /* WITHUSBCDCACM */
	STRING_ID_5,
	STRING_ID_5a,
#if WITHUSBCDCECM
	STRING_ID_MACADDRESS,	// iMacAddress
#endif /* WITHUSBCDCECM */

#if 0
	// USB UAC strings
	STRING_ID_a0, /* by offset RX demodulator */
	STRING_ID_aLAST = STRING_ID_a0 + UACOFFS_count - 1,

	STRING_ID_d0,
	STRING_ID_dLAST = STRING_ID_d0 + UACOFFS_count - 1,

	STRING_ID_e0,
	STRING_ID_eLAST = STRING_ID_e0 + UACOFFS_count - 1,
#endif

	STRING_ID_MODE0,
	STRING_ID_MODELAST = STRING_ID_MODE0 + UACOFFS_count - 1,

	//STRING_ID_b,	// tag for USB Speaker Audio Feature Unit Descriptor

	STRING_ID_Left, STRING_ID_Right,	// Идут подряд

	STRING_ID_RNDIS,
	STRING_ID_HIDa,
	STRING_ID_IQSPECTRUM,

	STRING_ID_DFU,
#if WIHSPIDFHW || WIHSPIDFSW
	STRING_ID_DFU_0,
	STRING_ID_DFU_1,
#endif /* WIHSPIDFHW || WIHSPIDFSW */
	STRING_ID_DFU_2,	/* RAM target for debug */
	// 
	STRING_ID_MTP,
	STRING_ID_MSC,
	//
	STRING_ID_count
};

struct stringtempl
{
	uint_fast8_t id;
	const char * str;
};

static const struct stringtempl strtemplates [] =
{
	{ STRING_ID_manuf, "MicroGenSF", },		// Manufacturer
	{ STRING_ID_prod, PRODUCTSTR, },	// Product

	{ STRING_ID_5, PRODUCTSTR " CDC EEM", },
	{ STRING_ID_5a, PRODUCTSTR " CDC ECM", },
	{ STRING_ID_RNDIS, PRODUCTSTR " Remote NDIS", },

	{ STRING_ID_DFU, PRODUCTSTR " DFU", },
#if WITHUSBDMTP
	{ STRING_ID_MTP, PRODUCTSTR " MTP", },
#endif /* WITHUSBDMTP */
#if WITHUSBDMSC
	{ STRING_ID_MSC, PRODUCTSTR " MSC", },
#endif /* WITHUSBDMSC */

#if WITHUSBUACINOUTRENESAS
//	{ STRING_ID_a0, PRODUCTSTR " Audio", },		// tag for Interface Descriptor 0/0 Audio
//	{ STRING_ID_a1, PRODUCTSTR " Audio1", },	// tag for Interface Descriptor 0/0 Audio
//	{ STRING_ID_a2, PRODUCTSTR " Audio2", },		// tag for Interface Descriptor 0/0 Audio
#elif WITHUSBUACINOUT
//	{ STRING_ID_a0, PRODUCTSTR " Voice", },		// tag for Interface Descriptor 0/0 Audio
//	{ STRING_ID_a1, PRODUCTSTR " Spectrum", },	// tag for Interface Descriptor 0/0 Audio
#else /* WITHUSBUACINOUT */
//	{ STRING_ID_a0, PRODUCTSTR " RX Voice", },		// tag for Interface Descriptor 0/0 Audio
//	{ STRING_ID_a1, PRODUCTSTR " RX Spectrum", },	// tag for Interface Descriptor 0/0 Audio
//	{ STRING_ID_a2, PRODUCTSTR " TX Voice", },		// tag for Interface Descriptor 0/0 Audio
#endif /* WITHUSBUACINOUT */

	//{ STRING_ID_b, "xxx_id11", },	// tag for USB Speaker Audio Feature Unit Descriptor

//	{ STRING_ID_d0, "TX audio USB streaming", },	// вход модулятора (ASIO4ALL) - pin name
//	{ STRING_ID_d1, "TX audio USB streaming", },	// Audio Control Input Terminal Descriptor  - pin name
//	{ STRING_ID_d2, "TX audio USB streaming", },	// Audio Control Input Terminal Descriptor - pin name

//	{ STRING_ID_e0, "RX audio USB streaming", },	// выход демодулятора (ASIO4ALL) - pin name
//	{ STRING_ID_e1, "RX audio USB streaming", },	// Audio Control Output Terminal Descriptor  - pin name
//	{ STRING_ID_e1, "RX audio USB streaming", },	// Audio Control Output Terminal Descriptor  - pin name

	{ STRING_ID_MODE0 + UACOFFS_IN48_OUT48, PRODUCTSTR " Voice", },	// MAC OS specific
	{ STRING_ID_MODE0 + UACOFFS_IN48, PRODUCTSTR " RX Voice", },	// MAC OS specific
	{ STRING_ID_MODE0 + UACOFFS_OUT48, PRODUCTSTR " TX Voice", },	// MAC OS specific
	{ STRING_ID_MODE0 + UACOFFS_INRTS, PRODUCTSTR " RX Spectrum", },	// MAC OS specific
	{ STRING_ID_MODE0 + UACOFFS_IN48_INRTS, PRODUCTSTR " RX Voice/Spectrum", },	// MAC OS specific

	{ STRING_ID_Left, "USB", },	// tag for USB Speaker Audio Feature Unit Descriptor
	{ STRING_ID_Right, "LSB", },	// tag for USB Speaker Audio Feature Unit Descriptor
	{ STRING_ID_HIDa, "HID xxx", },
#if 0//CTLSTYLE_OLEG4Z_V1
	{ STRING_ID_IQSPECTRUM, PRODUCTSTR " Spectre", },	// tag for Interface Descriptor 0/0 Audio
#else /* CTLSTYLE_OLEG4Z_V1 */
	{ STRING_ID_IQSPECTRUM, "RX IQ Output", },
#endif /* CTLSTYLE_OLEG4Z_V1 */
};

static uint_fast8_t usb_ft8cn;	/* совместимость VID/PID для работы с программой FT8CN */

static unsigned usbd_get_productId(void)
{

#if defined (USB_FUNCTION_PRODUCT_ID_OVERRIDE)
	return USB_FUNCTION_PRODUCT_ID_OVERRIDE;
#endif /* USB_FUNCTION_PRODUCT_ID_OVERRIDE */

	if (usb_ft8cn != 0)
		return 0x5732;	// Need for FT8CN
	unsigned v = 0;

#if WITHISBOOTLOADER && 0
	v = USB_FUNCTION_PRODUCT_ID;
#else /* WITHISBOOTLOADER */
	#if WITHUSBCDCACM
		v |= WITHUSBCDCACM_N * (1u << 12);
	#endif /* WITHUSBCDCACM */
#if WITHUSBDFU
	v |= (1u << 11);
#endif /* WWITHUSBDFU */
	#if WITHUAC2
		v |= (1u << 10);
	#endif /* WITHUAC2 */
	#if WITHUSBUACINOUTRENESAS
		v |= (7u << 7);
	#elif WITHUSBUACINOUT
		v |= (6u << 7);
	#elif WITHUSBUAC
		v |= (5u << 7);
	#endif
	#if WITHUSBDMTP
		v |= (1u << 6);
	#endif /* WITHUSBDMTP */
	#if WITHUSBDMSC
		v |= (1u << 5);
	#endif /* WITHUSBDMSC */
	#if WITHUSBHID
		v |= (1u << 4);
	#endif /* WITHUSBDMSC */
	#if (WITHUSBCDCEEM || WITHUSBCDCECM || WITHUSBRNDIS)
		v |= (1u << 3);
	#endif /* (WITHUSBCDCEEM || WITHUSBCDCECM || WITHUSBRNDIS) */

#endif /* WITHISBOOTLOADER */

	return v;
}


static unsigned usbd_get_vendorId(void)
{
#if defined (USB_FUNCTION_VENDOR_ID_OVERRIDE)
	return USB_FUNCTION_VENDOR_ID_OVERRIDE;
#endif /* USB_FUNCTION_VENDOR_ID_OVERRIDE */

	if (usb_ft8cn != 0)
		return 0x0483;	// Need for FT8CN - ST (SGS Thomson Microelectronics)
	return USB_FUNCTION_VENDOR_ID;
}

// usb_20.pdf:
// 5.9 High-Speed, High Bandwidth Endpoints
// 9.6.6 Endpoint
// Table 9-14. Allowed wMaxPacketSize Values for Different Numbers of Transactions per Microframe
static uint_fast16_t encodeMaxPacketSize(uint_fast32_t size)
{
	// For all endpoints, bits 10..0 specify the maximum
	// packet size (in bytes).

	// A high-speed endpoint can move up to 3072 bytes per microframe
	// For high-speed isochronous and interrupt endpoints:
	// Bits 12..11 specify the number of additional transaction
	// opportunities per microframe:
	// 00 = None (1 transaction per microframe)
	// 01 = 1 additional (2 per microframe)
	// 10 = 2 additional (3 per microframe)

	ASSERT(size <= 3072);
	// FIXME:
	if (size <= 1024)
		return size;	// 1..1024
	if (size <= 2048)
		return (0x01 << 11) | ((size + 1) / 2);	// 513..1024
	else
		return (0x02 << 11) | ((size + 2) / 3);	// 683..1024
}

// возврат степени 2 от числа (не являющиеся 1 2 4 8... округляются до ближайшего меньшего).
static uint_fast8_t
usb_ilog2(
	unsigned long v		// число на анализ
	)
{
	uint_fast8_t n;
	ASSERT(v != 0);

	for (n = 0; v != 1; ++ n)
		v >>= 1;

	return n;
}

// See audio10.pdf - 3.7.2.3 Audio Channel Cluster Format
static uint_fast8_t
UAC1_count_channels(
	uint_fast16_t wChannelConfig
	)
{
	uint_fast8_t bNrChannels;

	wChannelConfig &= 0x0FFF;	// считаются только predefined поля;
	for (bNrChannels = 0; wChannelConfig != 0; wChannelConfig >>= 1)
	{
		bNrChannels += (wChannelConfig & 0x01) != 0;
	}
	return bNrChannels;
}

// See audio10.pdf - 3.7.2.3 Audio Channel Cluster Format
static uint_fast8_t
UAC2_count_channels(
	uint_fast16_t wChannelConfig
	)
{
	uint_fast8_t bNrChannels;

	//wChannelConfig &= 0x0FFF;	// считаются только predefined поля;
	for (bNrChannels = 0; wChannelConfig != 0; wChannelConfig >>= 1)
	{
		bNrChannels += (wChannelConfig & 0x01) != 0;
	}
	return bNrChannels;
}

/* Header Functional Descriptor */
static unsigned CDC_HeaderFunctionalDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 5;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{ 
		const uint_fast16_t bcdCDC = CDC_V1_10;	/* bcdCDC: spec release number */
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = CDC_INTERFACE_DESCRIPTOR_TYPE;   /* bDescriptorType: CS_INTERFACE */
		* buff ++ = 0x00;   /* bDescriptorSubtype: Header Func Desc */
		* buff ++ = LO_BYTE(bcdCDC);			/* bcdCDC: spec release number */
		* buff ++ = HI_BYTE(bcdCDC);
	}
	return length;
}

#if WITHUSBUAC

// IN/OUT path topology parameters
//

#if 1

// Isochronous, Adaptive, Data - FS IN endpoint from UAC1 device
static const uint_fast8_t USBD_UAC1_IN_EP_ATTRIBUTES =
	USB_ENDPOINT_TYPE_ISOCHRONOUS |
	USB_ENDPOINT_SYNC_ADAPTIVE |
	USB_ENDPOINT_USAGE_DATA;

static const uint_fast8_t USBD_UAC1_OUT_EP_ATTRIBUTES =
	USB_ENDPOINT_TYPE_ISOCHRONOUS |
	USB_ENDPOINT_SYNC_ADAPTIVE |
	USB_ENDPOINT_USAGE_DATA;

// UAC2 Windows 10
// For the Adaptive IN case the driver does not support a feedforward endpoint.
// Isochronous, Asynchronous, Data
static const uint_fast8_t USBD_UAC2_IN_EP_ATTRIBUTES =
	USB_ENDPOINT_TYPE_ISOCHRONOUS |
	USB_ENDPOINT_SYNC_ASYNCHRONOUS |
	USB_ENDPOINT_USAGE_DATA;

// UAC2 Windows 10
// For the asynchronous OUT case the driver supports explicit feedback only.
// Isochronous, Adaptive, Data
static const uint_fast8_t USBD_UAC2_OUT_EP_ATTRIBUTES =
	USB_ENDPOINT_TYPE_ISOCHRONOUS |
	USB_ENDPOINT_SYNC_ADAPTIVE |
	USB_ENDPOINT_USAGE_DATA;

#else

static const uint_fast8_t USBD_UAC1_IN_EP_ATTRIBUTES =
	USB_ENDPOINT_USAGE_DATA |
	USB_ENDPOINT_TYPE_ISOCHRONOUS;

// UAC2 Windows 10
// For the Adaptive IN case the driver does not support a feedforward endpoint.
static const uint_fast8_t USBD_UAC2_IN_EP_ATTRIBUTES =
	USB_ENDPOINT_USAGE_DATA |
	USB_ENDPOINT_TYPE_ISOCHRONOUS;

static const uint_fast8_t USBD_UAC1_OUT_EP_ATTRIBUTES =
	USB_ENDPOINT_USAGE_DATA |
	USB_ENDPOINT_TYPE_ISOCHRONOUS;

// UAC2 Windows 10
// For the asynchronous OUT case the driver supports explicit feedback only.
static const uint_fast8_t USBD_UAC2_OUT_EP_ATTRIBUTES =
	USB_ENDPOINT_USAGE_DATA |
	USB_ENDPOINT_TYPE_ISOCHRONOUS;
#endif


// Отличаются назначения битов
#define UAC2_IT_bmControls	(1u << 0)	// D1..0: Copy Protect Control, D7..6: Cluster Control
#define UAC2_OT_bmControls	(1u << 0)	// D1..0: Copy Protect Control

// 4.7.2.1 Clock Source Descriptor

//In the following code bmAttributes field is 0x01; 
//which means that clock type is internal fixed clock.
// AC Clock Source Descriptor
// CS_SAM_FREQ_CONTROL = 1
// CS_CLOCK_VALID_CONTROL = 2


static unsigned UAC2_ClockSource(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bClockID
	)
{
	const uint_fast8_t length = 8;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения; а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = CS_INTERFACE;       /* bDescriptorType(0x24): CS_INTERFACE */ 
		* buff ++ = 0x0A;       /* bDescriptorSubType(0x0A): CLOCK_SOURCE */
		* buff ++ = bClockID;   /* bClockID: CLOCK_SOURCE_ID */
		* buff ++ = (0u << 2) | (3u << 0);	// bmAttributes
		* buff ++ = (1u << 2) | (1u << 0);	// bmControls D3..2: Clock Validity Control D1..0: Clock Frequency Control
		* buff ++ = TERMINAL_ID_UNDEFINED;       /* bAssocTerminal(0x00) */ 
		* buff ++ = STRING_ID_0;/* iClockSource: Not requested */
	}
	return length;
}

// 4.7.2.3 Clock Multiplier Descriptor
// Clock Multiplier
static unsigned UAC2_ClockMultiplier_(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bClockID,
	uint_fast8_t bCSourceID
	)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 1u: host-readeable parameters
		const uint_fast8_t bmControls = (1u << 2) | (1u << 0);	/* D1..0: Clock Numerator Control, D3..2: Clock Denominator Control */
		// Вызов для заполнения; а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = CS_INTERFACE;  	/* bDescriptorType(0x24): CS_INTERFACE */
		* buff ++ = 0x0C;       	/* bDescriptorSubType CLOCK_MULTIPLIER */
		* buff ++ = bClockID;   	/* bClockID */
		* buff ++ = bCSourceID;   	/* bCSourceID */
		* buff ++ = bmControls;
		* buff ++ = STRING_ID_0;	/* iClockMultiplier: Not requested */
	}
	return length;
}

/* UAC IAD */
// Interface Association Descriptor Audio
// Audio10.pdf 4.3.2.8 Associated Interface Descriptor
// documented in USB ECN : Interface Association Descriptor - InterfaceAssociationDesc_ecn.pdf
static unsigned UAC2_InterfaceAssociationDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bFirstInterface,
	uint_fast8_t bInterfaceCount,
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 8;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_ASSOC_DESCRIPTOR_TYPE;	// bDescriptorType: IAD
		* buff ++ = bFirstInterface;			// bFirstInterface
		* buff ++ = bInterfaceCount;	// bInterfaceCount
		* buff ++ = USB_DEVICE_CLASS_AUDIO;	// bFunctionClass: Audio
		* buff ++ = 0x00;		// bFunctionSubClass AUDIO_SUBCLASS_UNDEFINED
		* buff ++ = AUDIO_PROTOCOL_IP_VERSION_02_00;	// bFunctionProtocol
		* buff ++ = STRING_ID_MODE0 + offset;	// Interface string index
	}
	return length;
}

static unsigned UAC2_AC_InterfaceDesc(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bInterfaceNumber,
	uint_fast8_t bAlternateSetting,
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;      /* bDescriptorType */
		* buff ++ = bInterfaceNumber;					/* bInterfaceNumber */
		* buff ++ = bAlternateSetting;					/* bAlternateSetting */
		* buff ++ = 0x00;                               /* bNumEndpoints */
		* buff ++ = USB_DEVICE_CLASS_AUDIO;             /* bInterfaceClass */
		* buff ++ = AUDIO_SUBCLASS_AUDIOCONTROL;        /* bInterfaceSubClass */
		* buff ++ = AUDIO_PROTOCOL_IP_VERSION_02_00;   	/* bInterfaceProtocol */
		* buff ++ = STRING_ID_0;        			      /* iInterface */
		/* 09 byte*/
	}
	return length;
}

// Audio Control Input Terminal Descriptor
// 4.7.2.4 Input Terminal Descriptor
// audio20.pdf: Table 4-9: Input Terminal Descriptor
// Stereo signal source
// Audio only
// IN path topology
// 4.7.2.4 Input Terminal Descriptor
static unsigned UAC2_AudioControlIT_IN48(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bTerminalID,
	uint_fast8_t bCSourceID,	/* clock source ID */
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 17;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Table 4-9: Input Terminal Descriptor
		const uint_fast16_t wTerminalType = AUDIO_TERMINAL_RADIO_RECEIVER;
		const uint_fast32_t wChannelConfig = UACIN_CONFIG_IN48;
		const uint_fast16_t bmControls = UAC2_IT_bmControls;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE; // CS_INTERFACE Descriptor Type
		* buff ++ = AUDIO_CONTROL_INPUT_TERMINAL;    // INPUT_TERMINAL 0x02 descriptor subtype
		* buff ++ = bTerminalID;                   // bTerminalID ID of this Terminal.
		* buff ++ = LO_BYTE(wTerminalType);			/* wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;        // bAssocTerminal No association
		* buff ++ = bCSourceID;						// bCSourceID
		// The bNrChannels, wChannelConfig and iChannelNames fields together constitute the cluster descriptor
		* buff ++ = UAC2_count_channels(wChannelConfig);    /* bNrChannels */
		* buff ++ = LO_BYTE(wChannelConfig);   /* bmChannelConfig size = 4 bytes Mono sets no position bits */
		* buff ++ = HI_BYTE(wChannelConfig);
		* buff ++ = HI_24BY(wChannelConfig);
		* buff ++ = HI_32BY(wChannelConfig);
		* buff ++ = STRING_ID_Left;							/* iChannelNames */
		* buff ++ = LO_BYTE(bmControls);					/* bmControls */
		* buff ++ = HI_BYTE(bmControls);
		* buff ++ = 0;//STRING_ID_MODE0 + offset;					// unused  (iTerminal)
		/* 17 bytes*/
	}
	return length;
}


// Audio Control Input Terminal Descriptor
// audio10.pdf: Table 4-3: Input Terminal Descriptor
// Stereo signal source
// Audio или RTS
// 4.7.2.4 Input Terminal Descriptor
static unsigned UAC2_AudioControlIT_IN48_INRTS(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bTerminalID,
	uint_fast8_t bCSourceID,	/* clock source ID */
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 17;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 4.3.2.1 Input Terminal Descriptor
		const uint_fast16_t wTerminalType = AUDIO_TERMINAL_RADIO_RECEIVER;
		const uint_fast32_t wChannelConfig = UACIN_CONFIG_IN48_INRTS;
		const uint_fast16_t bmControls = UAC2_IT_bmControls;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE; // CS_INTERFACE Descriptor Type
		* buff ++ = AUDIO_CONTROL_INPUT_TERMINAL;    // INPUT_TERMINAL 0x02 descriptor subtype
		* buff ++ = bTerminalID;                   // bTerminalID ID of this Terminal.
		* buff ++ = LO_BYTE(wTerminalType);			/* wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;        // bAssocTerminal No association
		* buff ++ = bCSourceID;						// bCSourceID
		// The bNrChannels, wChannelConfig and iChannelNames fields together constitute the cluster descriptor
		* buff ++ = UAC2_count_channels(wChannelConfig);    /* bNrChannels */
		* buff ++ = LO_BYTE(wChannelConfig);   /* bmChannelConfig size = 4 bytes Mono sets no position bits */
		* buff ++ = HI_BYTE(wChannelConfig);
		* buff ++ = HI_24BY(wChannelConfig);
		* buff ++ = HI_32BY(wChannelConfig);
		* buff ++ = STRING_ID_Left;							/* iChannelNames */
		* buff ++ = LO_BYTE(bmControls);					/* bmControls */
		* buff ++ = HI_BYTE(bmControls);
		* buff ++ = 0;//STRING_ID_MODE0 + offset;						// iTerminal - Index of a string descriptor, describing the Input Terminal. Receiver Output
		/* 17 bytes*/
	}
	return length;
}

// 4.7.2.4 Input Terminal Descriptor
static unsigned UAC2_AudioControlIT_INRTS(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bTerminalID,
	uint_fast8_t bCSourceID,	/* clock source ID */
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 17;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 4.3.2.1 Input Terminal Descriptor 
		const uint_fast16_t wTerminalType = AUDIO_TERMINAL_RADIO_RECEIVER;
		const uint_fast32_t wChannelConfig = UACIN_CONFIG_INRTS;
		const uint_fast16_t bmControls = UAC2_IT_bmControls;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE; // CS_INTERFACE Descriptor Type
		* buff ++ = AUDIO_CONTROL_INPUT_TERMINAL;    // INPUT_TERMINAL 0x02 descriptor subtype
		* buff ++ = bTerminalID;                   // bTerminalID ID of this Terminal.
		* buff ++ = LO_BYTE(wTerminalType);			/* wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;        // bAssocTerminal No association
		* buff ++ = bCSourceID;						// bCSourceID
		// The bNrChannels, wChannelConfig and iChannelNames fields together constitute the cluster descriptor
		* buff ++ = UAC2_count_channels(wChannelConfig);    /* bNrChannels */
		* buff ++ = LO_BYTE(wChannelConfig);   /* bmChannelConfig size = 4 bytes Mono sets no position bits */
		* buff ++ = HI_BYTE(wChannelConfig);
		* buff ++ = HI_24BY(wChannelConfig);
		* buff ++ = HI_32BY(wChannelConfig);
		* buff ++ = STRING_ID_Left;							/* iChannelNames */
		* buff ++ = LO_BYTE(bmControls);					/* bmControls */
		* buff ++ = HI_BYTE(bmControls);
		* buff ++ = STRING_ID_MODE0 + offset;		// iTerminal - Index of a string descriptor, describing the Input Terminal. Receiver Output
		/* 17 bytes*/
	}
	return length;
}

/* !USB Speaker Input Terminal Descriptor */
// Audio Control Input Terminal Descriptor 
// audio10.pdf: Table 4-3: Input Terminal Descriptor
// audio48 only
// AC Input Terminal Descriptor
// OOT path topology element
// 4.7.2.4 Input Terminal Descriptor
// FIXME: problem at UACOUT48
static unsigned UAC2_AudioControlIT_OUT48(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize, 
	uint_fast8_t bTerminalID,
	uint_fast8_t bCSourceID,	/* clock source ID */
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 17;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Table 4-9: Input Terminal Descriptor
		const uint_fast16_t wTerminalType = AUDIO_TERMINAL_USB_STREAMING;
		const uint_fast32_t wChannelConfig = UACOUT_CONFIG_OUT48;
		const uint_fast16_t bmControls = UAC2_IT_bmControls;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;									/* 0 bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;		/* 1 bDescriptorType */
		* buff ++ = AUDIO_CONTROL_INPUT_TERMINAL;			/* 2 bDescriptorSubtype */
		* buff ++ = bTerminalID;							/* 3 bTerminalID */
		* buff ++ = LO_BYTE(wTerminalType);					/* 4 wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;					/* 6 bAssocTerminal */
		* buff ++ = bCSourceID;								/* 7 bCSourceID */
		// The bNrChannels, wChannelConfig and iChannelNames fields together constitute the cluster descriptor
		* buff ++ = UAC2_count_channels(wChannelConfig);							/* 8 bNrChannels */
		* buff ++ = LO_BYTE(wChannelConfig);                /* 9 wChannelConfig 0x0003  Front Left; Front Right */
		* buff ++ = HI_BYTE(wChannelConfig);
		* buff ++ = HI_24BY(wChannelConfig);
		* buff ++ = HI_32BY(wChannelConfig);
		* buff ++ = STRING_ID_Left;							/* 13 iChannelNames */
		* buff ++ = LO_BYTE(bmControls);					/* 14 bmControls */
		* buff ++ = HI_BYTE(bmControls);
		* buff ++ = STRING_ID_MODE0 + offset;					/* 16 iTerminal - появляется как pop-up в панели управления ASIO4ALL */
		/* 17 bytes*/
	}
	return length;
}

/*! USB Microphone Output Terminal Descriptor bSourceID -> bTerminalID */
// Audio Control Output Terminal Descriptor 
// Audio или RTS
// IN path topology
// 4.7.2.5 Output Terminal Descriptor
static unsigned UAC2_AudioControlOT_IN(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize, 
	uint_fast8_t bTerminalID,
	uint_fast8_t bSourceID,
	uint_fast8_t bCSourceID,	/* clock source ID */
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 12;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 
		const uint_fast16_t wTerminalType = AUDIO_TERMINAL_USB_STREAMING;
		const uint_fast16_t bmControls = UAC2_OT_bmControls;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE; // CS_INTERFACE Descriptor Type (bDescriptorType)
		* buff ++ = AUDIO_CONTROL_OUTPUT_TERMINAL;   // OUTPUT_TERMINAL descriptor subtype (bDescriptorSubtype)
		* buff ++ = bTerminalID;                            // ID of this Terminal. (bTerminalID)
		* buff ++ = LO_BYTE(wTerminalType);					/* wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;           // unused         (bAssocTerminal)
		* buff ++ = bSourceID;                            // From Input Terminal.(bSourceID)
		* buff ++ = bCSourceID;
		* buff ++ = LO_BYTE(bmControls);			/* 9 bmControls */
		* buff ++ = HI_BYTE(bmControls);
		* buff ++ = STRING_ID_MODE0 + offset;					// unused  (iTerminal)
	}
	return length;
}

#if 0
// Selector Unit Descriptor
static unsigned UAC_AudioSelectorUnit_IN(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bUnitID
	)
{
	//unsigned i;
	const uint_fast8_t bNrInPins = TERMINAL_ID_SELECTOR_6_INPUTS;	// количество входных потоков
	const uint_fast8_t length = 6 + bNrInPins;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		* buff ++ = length;							/* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;/* bDescriptorType */
		* buff ++ = AUDIO_CONTROL_SELECTOR_UNIT;	/* bDescriptorSubtype */
		* buff ++ = bUnitID;             			/* bUnitID */
		* buff ++ = bNrInPins;             			/* bNrInPins */
		* buff ++ = TERMINAL_ID_IT_2;				/* baSourceID(0) */
		* buff ++ = TERMINAL_ID_FU_5c;				/* baSourceID(1) */
		* buff ++ = 0;								/* iSelector (string ID) - unused */
	}
	return length;
}
#endif

// Audio Control Feature Unit Descriptor 
// 4.7.2.8 Feature Unit Descriptor
// В нашем случае используется для подавления отображения раздельных элементов регулировки уровня по каналам
// IN path topology
static unsigned UAC2_AudioFeatureUnit_IN(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bUnitID,
	uint_fast8_t bSourceID,
	uint_fast8_t offset
	)
{
	// Параметр определяет, ккие управляющие элементы появляются на страничке управления "Custom"
	// Причем, на этой страничке собраны все управляющие элементы со всех Feature Unit Descriptor
	// в пути сигнала данного источника звука.
	// Each element:
	// 0x00 - not present
	// 0x01 - If a Control is present but read-only,
	// 0x03 - If a Control is also Host programmable
	// в нашем случае для предотвращения управления со стороны операционной системы
	// говорим, что соответствющие элементы у нас есть...
	const uint_fast32_t bmaControls0 =
		1 * (0x03 << 0) |	// Mute Control
		1 * (0x03 << 2) |	// Volume Control
		0 * (0x01 << 4) |	// Bass Control
		0 * (0x01 << 6) |	// Mid Control
		0 * (0x01 << 8) |	// Treble Control
		// and so on...
		0;

	const uint_fast32_t bmaControlsv [] =
	{
		bmaControls0,	// master
		0,				// left
		0,				// right
	};

	const uint_fast8_t ch = ARRAY_SIZE(bmaControlsv); //1 + UAC2_IN_bNrChannels; // 1: Only master channel controls, 3: master, left and right
	const uint_fast8_t length = 6 + 4 * ch;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		uint_fast8_t i;
		// See 4.3.2.5 Feature Unit Descriptor for details
		* buff ++ = length;							/* 0 bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;/* 1 bDescriptorType */
		* buff ++ = AUDIO_CONTROL_FEATURE_UNIT;     /* 2 bDescriptorSubtype */
		* buff ++ = bUnitID;             			/* 3 bUnitID */
		* buff ++ = bSourceID;						/* 4 bSourceID */
		for (i = 0; i < ch; ++ i)
		{
			uint_fast32_t v = bmaControlsv [i];
			uint_fast8_t cs = 4;
			while (cs --)
			{
				* buff ++ = (uint8_t) v;
				v >>= 8;
			}
		}
		* buff ++ = STRING_ID_MODE0 + offset;                    /* 5+(ch+1)*4 iTerminal iFeature */
		/* 6 + 4 * ch bytes */
	}
	return length;
}

// 4.7.2.8 Feature Unit Descriptor
// В нашем случае используется для подавления отображения раздельных элементов регулировки уровня по каналам
// OOT path topology
// FIXME: problem at UACOUT48
static unsigned UAC2_AudioFeatureUnit_OUT(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bUnitID,
	uint_fast8_t bSourceID,
	uint_fast8_t offset
	)
{
	// Параметр определяет, ккие управляющие элементы появляются на страничке управления "Custom"
	// Причем, на этой страничке собраны все управляющие элементы со всех Feature Unit Descriptor
	// в пути сигнала данного источника звука.
	// Each element:
	// 0x00 - not present
	// 0x01 - If a Control is present but read-only,
	// 0x03 - If a Control is also Host programmable
	// в нашем случае для предотвращения управления со стороны операционной системы
	// говорим, что соответствющие элементы у нас есть...
	const uint_fast32_t bmaControls0 =
		1 * (0x03 << 0) |	// Mute Control
		1 * (0x03 << 2) |	// Volume Control
		0 * (0x03 << 4) |	// Bass Control
		0 * (0x03 << 6) |	// Mid Control
		0 * (0x03 << 8) |	// Treble Control
		// and so on...
		0;

	const uint_fast32_t bmaControlsv [] =
	{
		bmaControls0,	// master
		0,				// left
		0,				// right
	};
	const uint_fast8_t ch = ARRAY_SIZE(bmaControlsv); // 1: Only master channel controls, 3: master, left and right
	const uint_fast8_t length = 6 + 4 * ch;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		uint_fast8_t i;
		// See 4.7.2.8 Feature Unit Descriptor for details
		* buff ++ = length;							/* 0 bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;/* 1 bDescriptorType */
		* buff ++ = AUDIO_CONTROL_FEATURE_UNIT;     /* 2 bDescriptorSubtype */
		* buff ++ = bUnitID;             			/* 3 bUnitID */
		* buff ++ = bSourceID;						/* 4 bSourceID */
		for (i = 0; i < ch; ++ i)
		{
			uint_fast32_t v = bmaControlsv [i];
			uint_fast8_t cs = 4;
			while (cs --)
			{
				* buff ++ = (uint8_t) v;
				v >>= 8;
			}
		}
		* buff ++ = STRING_ID_MODE0 + offset;                    /* 5+(ch+1)*4 iTerminal */
		/* 6 + 4 * ch bytes */
	}
	return length;
}

//
// Если выход AUDIO_TERMINAL_RADIO_TRANSMITTER, закладки enchancements нет
// Audio Control Output Terminal Descriptor 
// bSourceID -> bTerminalID
// audio48 only
// OOT path topology - final element (modulator, speaker, ...)
// 4.7.2.5 Output Terminal Descriptor
// FIXME: problem at UACOUT48
static unsigned UAC2_AudioControlOT_OUT48(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize, 
	uint_fast8_t bTerminalID,
	uint_fast8_t bSourceID,
	uint_fast8_t bCSourceID,
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 12;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 4.3.2.2 Output Terminal Descriptor 
		const uint_fast16_t wTerminalType = AUDIO_TERMINAL_RADIO_TRANSMITTER;
		const uint_fast16_t bmControls = UAC2_OT_bmControls;

		* buff ++ = length;							/* 0 bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;/* 1 bDescriptorType */
		* buff ++ = AUDIO_CONTROL_OUTPUT_TERMINAL;  /* 2 bDescriptorSubtype */
		* buff ++ = bTerminalID;					/* 3 bTerminalID */
		* buff ++ = LO_BYTE(wTerminalType);			/* 4 wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;          /* 6 bAssocTerminal */
		* buff ++ = bSourceID;						/* 7 bSourceID */
		* buff ++ = bCSourceID;						/* 8 bCSourceID */
		* buff ++ = LO_BYTE(bmControls);			/* 9 bmControls */
		* buff ++ = HI_BYTE(bmControls);
		* buff ++ = 0;//STRING_ID_MODE0 + offset;					// unused  (iTerminal)
		/* 9 byte*/
	}
	return length;
}

// Номера терминалов и string id, используемые при заполнении дескрипторов топологии аудиоустройств
typedef struct topology_terminals
{
	uint8_t bTerminalID;	// терминал, завершающий поток обработки
	uint8_t bFeatureUnitID;
	uint8_t bFeatureUnit2ID;
//	uint8_t bOSC;	// Выходной темрминал цепочки с ClockMultiplier
	uint8_t bCSourceID;
} tpgt_t;

// Заполнение схемы ввода звука
// IN data flow
// Элементы добавлояются в дескриптор в порядке обратном порядку прохождения информационного потока
// 
static unsigned UAC2_TopologyIN48(
	uint_fast8_t fill, uint8_t * p, unsigned maxsize, 
	const tpgt_t * tgpt,
	uint_fast8_t offset
	)
{
	unsigned n = 0;

	n += UAC2_ClockSource(fill, p + n, maxsize - n, tgpt->bCSourceID);
	////n += UAC2_ClockMultiplier(fill, p + n, maxsize - n, tgpt->bCSourceID, tgpt->bOSC);

	n += UAC2_AudioControlOT_IN(fill, p + n, maxsize - n,  tgpt->bTerminalID, tgpt->bFeatureUnitID, tgpt->bCSourceID, offset);	/* AUDIO_TERMINAL_USB_STREAMING Terminal Descriptor TERMINAL_ID_FU_AUDIO -> TERMINAL_UACIN48_UACINRTS */
	n += UAC2_AudioFeatureUnit_IN(fill, p + n, maxsize - n, tgpt->bFeatureUnitID, tgpt->bFeatureUnit2ID, offset);	/* USB microphone Audio Feature Unit Descriptor TERMINAL_UACOUT48 -> TERMINAL_ID_FU_AUDIO */
	n += UAC2_AudioControlIT_IN48(fill, p + n, maxsize - n, tgpt->bFeatureUnit2ID, tgpt->bCSourceID, offset);	/* AUDIO_TERMINAL_RADIO_RECEIVER */

	return n;
}

static unsigned UAC2_Topology_inout_IN48(
	uint_fast8_t fill, uint8_t * p, unsigned maxsize,
	const tpgt_t * tgpt,
	uint_fast8_t offset
	)
{
	unsigned n = 0;

	n += UAC2_ClockSource(fill, p + n, maxsize - n, tgpt->bCSourceID);
	////n += UAC2_ClockMultiplier(fill, p + n, maxsize - n, tgpt->bCSourceID, tgpt->bOSC);

	n += UAC2_AudioControlIT_IN48(fill, p + n, maxsize - n, tgpt->bFeatureUnit2ID, tgpt->bCSourceID, offset);	/* AUDIO_TERMINAL_RADIO_RECEIVER */
	n += UAC2_AudioFeatureUnit_IN(fill, p + n, maxsize - n, tgpt->bFeatureUnitID, tgpt->bFeatureUnit2ID, offset);	/* USB microphone Audio Feature Unit Descriptor TERMINAL_UACOUT48 -> TERMINAL_ID_FU_AUDIO */
	n += UAC2_AudioControlOT_IN(fill, p + n, maxsize - n,  tgpt->bTerminalID, tgpt->bFeatureUnitID, tgpt->bCSourceID, offset);	/* AUDIO_TERMINAL_USB_STREAMING Terminal Descriptor TERMINAL_ID_FU_AUDIO -> TERMINAL_UACIN48_UACINRTS */

	return n;
}

// Заполнение схемы вывода звука
// OOT path topology
// audio48 only
// OOT path topology
static unsigned UAC2_Topology_inout_OUT48(
	uint_fast8_t fill, uint8_t * p, unsigned maxsize,
	const tpgt_t * tgpt,
	uint_fast8_t offset
	)
{
	unsigned n = 0;

	// Already done in UAC2_Topology_inout_IN48
//	n += UAC2_ClockSource(fill, p + n, maxsize - n, tgpt->bOSC);
//	n += UAC2_ClockMultiplier(fill, p + n, maxsize - n, tgpt->bCSourceID, tgpt->bOSC);

	n += UAC2_AudioControlIT_OUT48(fill, p + n, maxsize - n, tgpt->bTerminalID, tgpt->bCSourceID, offset);	/* AUDIO_TERMINAL_USB_STREAMING Input Terminal Descriptor TERMINAL_UACOUT48 */
	n += UAC2_AudioFeatureUnit_OUT(fill, p + n, maxsize - n, tgpt->bFeatureUnitID, tgpt->bTerminalID, offset);	/* USB Speaker Audio Feature Unit Descriptor TERMINAL_UACOUT48 -> TERMINAL_ID_FU_5 */
	n += UAC2_AudioControlOT_OUT48(fill, p + n, maxsize - n, tgpt->bFeatureUnit2ID, tgpt->bFeatureUnitID, tgpt->bCSourceID, offset);	/* AUDIO_TERMINAL_RADIO_TRANSMITTER Output Terminal Descriptor TERMINAL_ID_FU_5 -> TERMINAL_ID_OT_3 */

	return n;
}

// Заполнение схемы ввода звука
// IN data flow
// Элементы добавлояются в дескриптор в порядке обратном порядку прохождения информационного потока
// IN path topology
static unsigned UAC2_TopologyIN48_INRTS(
	uint_fast8_t fill, uint8_t * p, unsigned maxsize,
	const tpgt_t * tgpt,
	uint_fast8_t offset
	)
{
	unsigned n = 0;

	n += UAC2_ClockSource(fill, p + n, maxsize - n, tgpt->bCSourceID);
	////n += UAC2_ClockMultiplier(fill, p + n, maxsize - n, tgpt->bCSourceID, tgpt->bOSC);

	n += UAC2_AudioControlOT_IN(fill, p + n, maxsize - n,  tgpt->bTerminalID, tgpt->bFeatureUnitID, tgpt->bCSourceID, offset);	/* AUDIO_TERMINAL_USB_STREAMING Terminal Descriptor TERMINAL_ID_FU_AUDIO -> TERMINAL_UACIN48_UACINRTS */
	n += UAC2_AudioFeatureUnit_IN(fill, p + n, maxsize - n, tgpt->bFeatureUnitID, tgpt->bFeatureUnit2ID, offset);	/* USB microphone Audio Feature Unit Descriptor TERMINAL_UACOUT48 -> TERMINAL_ID_FU_AUDIO */
	n += UAC2_AudioControlIT_IN48_INRTS(fill, p + n, maxsize - n, tgpt->bFeatureUnit2ID, tgpt->bCSourceID, offset);	/* AUDIO_TERMINAL_RADIO_RECEIVER */

	return n;
}

// Заполнение схемы вывода звука
// OUT data flow
// audio48 only
// OOT path topology
// FIXME: problem at UACOUT48
static unsigned UAC2_TopologyOUT48(
	uint_fast8_t fill, uint8_t * p, unsigned maxsize, 
	const tpgt_t * tgpt,
	uint_fast8_t offset
	)
{
	unsigned n = 0;

	n += UAC2_ClockSource(fill, p + n, maxsize - n, tgpt->bCSourceID);
	////n += UAC2_ClockMultiplier(fill, p + n, maxsize - n, tgpt->bCSourceID, tgpt->bOSC);

	n += UAC2_AudioControlIT_OUT48(fill, p + n, maxsize - n, tgpt->bTerminalID, tgpt->bCSourceID, offset);	/* AUDIO_TERMINAL_USB_STREAMING Input Terminal Descriptor TERMINAL_UACOUT48 */
	n += UAC2_AudioFeatureUnit_OUT(fill, p + n, maxsize - n, tgpt->bFeatureUnitID, tgpt->bTerminalID, offset);	/* USB Speaker Audio Feature Unit Descriptor TERMINAL_UACOUT48 -> TERMINAL_ID_FU_5 */
	n += UAC2_AudioControlOT_OUT48(fill, p + n, maxsize - n, tgpt->bFeatureUnit2ID, tgpt->bFeatureUnitID, tgpt->bCSourceID, offset);	/* AUDIO_TERMINAL_RADIO_TRANSMITTER Output Terminal Descriptor TERMINAL_ID_FU_5 -> TERMINAL_ID_OT_3 */

	return n;
}

// Элементы добавлояются в дескриптор в порядке обратном порядку прохождения информационного потока
// 
static unsigned UAC2_TopologyINRTS(
	uint_fast8_t fill, uint8_t * p, unsigned maxsize, 
	const tpgt_t * tgpt,
	uint_fast8_t offset
	)
{
	unsigned n = 0;

	n += UAC2_ClockSource(fill, p + n, maxsize - n, tgpt->bCSourceID);
	////n += UAC2_ClockMultiplier(fill, p + n, maxsize - n, tgpt->bCSourceID, tgpt->bOSC);

	n += UAC2_AudioControlOT_IN(fill, p + n, maxsize - n,  tgpt->bTerminalID, tgpt->bFeatureUnitID, tgpt->bCSourceID, offset);	/* AUDIO_TERMINAL_USB_STREAMING Terminal Descriptor TERMINAL_ID_IT_2 -> TERMINAL_UACIN48_UACINRTS */
	n += UAC2_AudioFeatureUnit_IN(fill, p + n, maxsize - n, tgpt->bFeatureUnitID, tgpt->bFeatureUnit2ID, offset);	/* USB microphone Audio Feature Unit Descriptor TERMINAL_UACOUT48 -> TERMINAL_ID_FU_RTS */
	n += UAC2_AudioControlIT_INRTS(fill, p + n, maxsize - n, tgpt->bFeatureUnit2ID, tgpt->bCSourceID, offset);	/* AUDIO_TERMINAL_RADIO_RECEIVER */

	return n;
}

typedef unsigned (* uac_pathfn_t)(uint_fast8_t fill, uint8_t * p, unsigned maxsize, const tpgt_t * tgpt, uint_fast8_t offset);

// 4.7.2 Class-Specific AC Interface Descriptor
static unsigned UAC2_HeaderDescriptor(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	const uint_fast8_t * coll,		// array of data interfaces in this functional device
	const uac_pathfn_t * paths,
	const tpgt_t * const * tgpt,
	uint_fast8_t bInCollection,		// size of array
	uint_fast8_t offset
	)
{
	uint_fast8_t i;
	unsigned terminalsLength = 0;
	const uint_fast8_t length = 9;
	for (i = 0; i < bInCollection; ++ i)
		terminalsLength += (paths [i])(0, buff + length + terminalsLength, maxsize - length - terminalsLength, tgpt [i], offset);
	const unsigned wTotalLength = terminalsLength + length;
	ASSERT(maxsize >= wTotalLength);
	if (maxsize < wTotalLength)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 4.3.2 Class-Specific AC Interface Descriptor
		const uint_fast16_t bcdADC = 0x0200;	// Revision of class specification - 2.0
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* 0 bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;/* 1 bDescriptorType */
		* buff ++ = AUDIO_CONTROL_HEADER;           /* 2 bDescriptorSubtype */
		* buff ++ = LO_BYTE(bcdADC);				/* 3 bcdADC */
		* buff ++ = HI_BYTE(bcdADC);
		* buff ++ = 0x02;							/* 4 bCategory (2:HOME_THEATER,  8:IO_BOX) Table A-7: Audio Function Category Codes */
		* buff ++ = LO_BYTE(wTotalLength);			/* 6 wTotalLength */
		* buff ++ = HI_BYTE(wTotalLength);
		* buff ++ = 0x00;							/* 8 bmControls D1..0: Latency Control */
		/* 9 bytes*/
		terminalsLength = 0;
		for (i = 0; i < bInCollection; ++ i)
			terminalsLength += (paths [i])(fill, buff + terminalsLength, maxsize - length - terminalsLength, tgpt [i], offset);
	}
	return wTotalLength;

}

// Interface Descriptor 2/1 Audio, 0 or 1 Endpoint
static unsigned UAC2_InterfaceDesc(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bInterfaceNumber,
	uint_fast8_t bAlternateSetting,
	uint_fast8_t bNumEndpoints,
	uint_fast8_t offset,
	uint_fast8_t iInterface		/* название цстройства, появляется в списке устройств на MAC OS */
	)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;  // INTERFACE descriptor type (bDescriptorType)
		* buff ++ = bInterfaceNumber;			// Index of this interface. (bInterfaceNumber)
		* buff ++ = bAlternateSetting;				// Index of this alternate setting. (bAlternateSetting) - zero-based index
		* buff ++ = bNumEndpoints;					// bNumEndpoints
		* buff ++ = USB_DEVICE_CLASS_AUDIO;			// AUDIO (bInterfaceClass)
		* buff ++ = AUDIO_SUBCLASS_AUDIOSTREAMING;  // AUDIO_STREAMING (bInterfaceSubclass)
		* buff ++ = AUDIO_PROTOCOL_IP_VERSION_02_00;/* bInterfaceProtocol */
		* buff ++ = iInterface;					/* Unused iInterface */
		/* 9 byte*/
	}
	return length;
}

static unsigned align2(unsigned n)
{
	return (n + 1) / 2 * 2;
}
/* USB Speaker Audio Type I Format Interface Descriptor */
// USBLyzer: Audio Streaming Format Type 1 Descriptor
static unsigned UAC2_FormatTypeDesc_OUT48(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 6;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast32_t samplefreq1 = dsp_get_samplerateuacout();
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;		/* bDescriptorType */
		* buff ++ = AUDIO_STREAMING_FORMAT_TYPE;			/* bDescriptorSubtype */
		* buff ++ = AUDIO_FORMAT_TYPE_I;							/* bFormatType */
		* buff ++ = UACOUT_AUDIO48_SAMPLEBYTES;	//bSubslotSize
		* buff ++ = UACOUT_AUDIO48_SAMPLEBYTES * 8;	//bBitResolution
		/* 6 byte*/
	}
	return length;
}

/* Endpoint 1 - Standard Descriptor */
// out: from computer to our device
static unsigned UAC2_EP_Desc_OUT48(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress, int highspeed)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(usbd_getuacoutmaxpacket());
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;         /* bDescriptorType */
		* buff ++ = bEndpointAddress;               /* bEndpointAddress 1 out endpoint*/
		* buff ++ = USBD_UAC2_OUT_EP_ATTRIBUTES;       						    /* bmAttributes */
		* buff ++ = LO_BYTE(wMaxPacketSize);              /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = highspeed ? HSINTERVAL_AUDIO48 : FSINTERVAL_AUDIO48;    /* bInterval */
		/* 7 byte*/
	}
	return length;
}

//static unsigned UAC2_EP_Feedback_Desc_OUT48(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress, int highspeed)
//{
//	const uint_fast8_t length = 7;
//	ASSERT(maxsize >= length);
//	if (maxsize < length)
//		return 0;
//	if (fill != 0 && buff != NULL)
//	{
//		const uint_fast16_t wMaxPacketSize = 4; // Feedback endpoint data size
//		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
//		* buff ++ = length;						  /* bLength */
//		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;         /* bDescriptorType */
//		* buff ++ = bEndpointAddress;               /* bEndpointAddress 1 out endpoint*/
//		* buff ++ = USB_ENDPOINT_USAGE_FEEDBACK | USB_ENDPOINT_TYPE_ISOCHRONOUS;       						    /* bmAttributes */
//		* buff ++ = LO_BYTE(wMaxPacketSize);              /* wMaxPacketSize */
//		* buff ++ = HI_BYTE(wMaxPacketSize);
//		* buff ++ = highspeed ? HSINTERVAL_AUDIO48 : FSINTERVAL_AUDIO48;    /* bInterval */
//		/* 7 byte*/
//	}
//	return length;
//}

/* Endpoint - Audio Streaming Descriptor*/
static unsigned UAC2_AS_EP_Desc_OUT48(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 8;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wLockDelay = 0;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = AUDIO_ENDPOINT_DESCRIPTOR_TYPE; /* 0x25 bDescriptorType */
		* buff ++ = AUDIO_ENDPOINT_GENERAL;         /* 0x01 bDescriptor */
		* buff ++ = 0x00;                           /* bmAttributes (D7=MaxPacketsOnly) */
		* buff ++ = 0x00;                           /* bmControls */
		* buff ++ = 0x00;                           /* bLockDelayUnits */
		* buff ++ = LO_BYTE(wLockDelay);			/* wLockDelay */
		* buff ++ = HI_BYTE(wLockDelay);
		/* 8 byte*/
	}
	return length;
}

/* USB Microphone Type I Format Type Descriptor (CODE == 6)*/
// Audio Streaming Format Type Descriptor 
// Frmts20 final.pdf: Table 2-2: Type I Format Type Descriptor
static unsigned UAC2_FormatTypeDescroptor_IN48(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 6;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;		/* bDescriptorType */
		* buff ++ = AUDIO_STREAMING_FORMAT_TYPE;			/* bDescriptorSubtype */
		* buff ++ = AUDIO_FORMAT_TYPE_I;							/* bFormatType */
		* buff ++ = UACIN_AUDIO48_SAMPLEBYTES;	//bSubslotSize
		* buff ++ = UACIN_AUDIO48_SAMPLEBYTES * 8;	//bBitResolution
		/* 6 byte*/
	}
	return length;
}

/* USB Microphone Standard Endpoint Descriptor (CODE == 8)*/ //Standard AS Isochronous Audio Data Endpoint Descriptor
// Endpoint Descriptor 82 2 In, Isochronous, 125 us
static unsigned UAC2_EP_Desc_IN48(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress, uint8_t offset)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(usbd_getuacinmaxpacket()); // was: 0x300
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	// bDescriptorType
		* buff ++ = bEndpointAddress;                    // bEndpointAddress
		* buff ++ = USBD_UAC2_IN_EP_ATTRIBUTES; // bmAttributes
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = highspeed ? HSINTERVAL_AUDIO48 : FSINTERVAL_AUDIO48;    /* bInterval */
	}
	return length;
}

#if WITHRTS96

/* USB Microphone Type I Format Type Descriptor (CODE == 6)*/
// Audio Streaming Format Type Descriptor 
static unsigned UAC2_FormatTypeDesc_RTS96(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 6;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;		/* bDescriptorType */
		* buff ++ = AUDIO_STREAMING_FORMAT_TYPE;			/* bDescriptorSubtype */
		* buff ++ = AUDIO_FORMAT_TYPE_I;							/* bFormatType */
		* buff ++ = UACIN_RTS96_SAMPLEBYTES;		//bSubslotSize
		* buff ++ = UACIN_RTS96_SAMPLEBYTES * 8;	//bBitResolution
		/* 6 byte*/
	}
	return length;
}

/* USB Microphone Standard Endpoint Descriptor (CODE == 8)*/ //Standard AS Isochronous Audio Data Endpoint Descriptor
// Endpoint Descriptor 82 2 In, Isochronous, 125 us
static unsigned UAC2_EP_Desc_IN_RTS96(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress, uint8_t offset)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(usbd_getuacinrtsmaxpacket()); // was: 0x300
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	// bDescriptorType
		* buff ++ = bEndpointAddress;                    // bEndpointAddress
		* buff ++ = USBD_UAC2_IN_EP_ATTRIBUTES; // bmAttributes
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = highspeed ? HSINTERVAL_RTS96 : FSINTERVAL_RTS96;    /* bInterval */
	}
	return length;
}

#endif /* WITHRTS96 */

#if WITHRTS192

/* USB Microphone Type I Format Type Descriptor (CODE == 6)*/
// Audio Streaming Format Type Descriptor 
static unsigned UAC2_FormatTypeDescroptor_RTS192(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 6;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;		/* bDescriptorType */
		* buff ++ = AUDIO_STREAMING_FORMAT_TYPE;			/* bDescriptorSubtype */
		* buff ++ = AUDIO_FORMAT_TYPE_I;							/* bFormatType */
		* buff ++ = UACIN_RTS192_SAMPLEBYTES;		//bSubslotSize
		* buff ++ = UACIN_RTS192_SAMPLEBYTES * 8;	//bBitResolution
		/* 6 byte*/
	}
	return length;
}

/* USB Microphone Standard Endpoint Descriptor (CODE == 8)*/ //Standard AS Isochronous Audio Data Endpoint Descriptor
// Endpoint Descriptor 82 2 In, Isochronous, 125 us
static unsigned UAC2_fill_27_RTS192(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress, uint8_t offset)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(UACIN_RTS192_DATASIZE); // was: 0x300
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	// bDescriptorType
		* buff ++ = bEndpointAddress;                    // bEndpointAddress
		* buff ++ = USBD_UAC2_IN_EP_ATTRIBUTES; // bmAttributes
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = highspeed ? HSINTERVAL_RTS192 : FSINTERVAL_RTS192;    /* bInterval */
	}
	return length;
}

#endif /* WITHRTS192 */

// 4.7.2 Class-Specific AC Endpoint Descriptor
static unsigned UAC2_AS_EP_Desc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 8;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wLockDelay = 0;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_ENDPOINT_DESCRIPTOR_TYPE;    // CS_ENDPOINT Descriptor Type (bDescriptorType) 0x25
		* buff ++ = AUDIO_ENDPOINT_GENERAL;            // GENERAL subtype. (bDescriptorSubtype) 0x01
		* buff ++ = 0x00;                           /* bmAttributes (D7=MaxPacketsOnly) */
		* buff ++ = 0x00;                           /* bmControls B1:B0: kegacy control */
		* buff ++ = 0x00;                           /* bLockDelayUnits */
		* buff ++ = LO_BYTE(wLockDelay);			/* wLockDelay */
		* buff ++ = HI_BYTE(wLockDelay);
	}
	return length;
}

/* USB Microphone Type I Format Type Descriptor (CODE == 6)*/
// Audio Streaming Format Type Descriptor
static unsigned UAC1_FormatTypeDesc_IN48(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 11;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast32_t samplefreq1 = dsp_get_samplerateuacin_audio48();
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;// CS_INTERFACE Descriptor Type (bDescriptorType) 0x24
		* buff ++ = AUDIO_STREAMING_FORMAT_TYPE;   // FORMAT_TYPE subtype. (bDescriptorSubtype) 0x02
		* buff ++ = AUDIO_FORMAT_TYPE_I;							/* bFormatType */
		* buff ++ = UACIN_FMT_CHANNELS_AUDIO48;		/* bNrChannels */
		* buff ++ = UACIN_AUDIO48_SAMPLEBYTES; /* bSubFrameSize :  2 Bytes per frame (16bits) */
		* buff ++ = UACIN_AUDIO48_SAMPLEBYTES * 8;		/* bBitResolution (16-bits per sample) */
		* buff ++ = 1;										/* bSamFreqType only one frequency supported */
		* buff ++ = LO_BYTE(samplefreq1);	/* Audio sampling frequency coded on 3 bytes */
		* buff ++ = HI_BYTE(samplefreq1);
		* buff ++ = HI_24BY(samplefreq1);
	}
	return length;
}

/* USB Microphone Standard Endpoint Descriptor (CODE == 8)*/ //Standard AS Isochronous Audio Data Endpoint Descriptor
// Endpoint Descriptor 82 2 In, Isochronous, 125 us
static unsigned UAC1_EP_Desc_IN48(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress, uint8_t offset)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(UACIN_AUDIO48_DATASIZE); // was: 0x300
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	// bDescriptorType
		* buff ++ = bEndpointAddress;                    // bEndpointAddress
		* buff ++ = USBD_UAC1_IN_EP_ATTRIBUTES; // bmAttributes
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize);
		* buff ++ = highspeed ? HSINTERVAL_AUDIO48 : FSINTERVAL_AUDIO48;    /* bInterval */
		* buff ++ = 0x00;                       // Unused. (bRefresh)
		* buff ++ = 0x00;                       // Unused. (bSynchAddress)
	}
	return length;
}

#if WITHRTS96

/* USB Microphone Type I Format Type Descriptor (CODE == 6)*/
// Audio Streaming Format Type Descriptor
static unsigned UAC1_FormatTypeDesc_RTS96(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 11;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast32_t samplefreq1 = dsp_get_samplerateuacin_RTS96();
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;// CS_INTERFACE Descriptor Type (bDescriptorType) 0x24
		* buff ++ = AUDIO_STREAMING_FORMAT_TYPE;   // FORMAT_TYPE subtype. (bDescriptorSubtype) 0x02
		* buff ++ = AUDIO_FORMAT_TYPE_I;							/* bFormatType */
		* buff ++ = UACIN_FMT_CHANNELS_RTS96;		/* bNrChannels */
		* buff ++ = UACIN_RTS96_SAMPLEBYTES;		//bSubFrameSize
		* buff ++ = UACIN_RTS96_SAMPLEBYTES * 8;	//bBitResolution
		* buff ++ = 1;										/* bSamFreqType only one frequency supported */
		* buff ++ = LO_BYTE(samplefreq1);	/* Audio sampling frequency coded on 3 bytes */
		* buff ++ = HI_BYTE(samplefreq1);
		* buff ++ = HI_24BY(samplefreq1);
	}
	return length;
}

/* USB Microphone Standard Endpoint Descriptor (CODE == 8)*/ //Standard AS Isochronous Audio Data Endpoint Descriptor
// Endpoint Descriptor 82 2 In, Isochronous, 125 us
static unsigned UAC1_EP_Desc_RTS96(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress, uint8_t offset)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(usbd_getuacinrtsmaxpacket()); // was: 0x300
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	// bDescriptorType
		* buff ++ = bEndpointAddress;                    // bEndpointAddress
		* buff ++ = USBD_UAC1_IN_EP_ATTRIBUTES; // bmAttributes
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize);
		* buff ++ = highspeed ? HSINTERVAL_RTS96 : FSINTERVAL_RTS96;    /* bInterval */
		* buff ++ = 0x00;                       // Unused. (bRefresh)
		* buff ++ = 0x00;                       // Unused. (bSynchAddress)
	}
	return length;
}

#endif /* WITHRTS96 */

#if WITHRTS192

/* USB Microphone Type I Format Type Descriptor (CODE == 6)*/
// Audio Streaming Format Type Descriptor
static unsigned UAC1_FormatTypeDesc_RTS192(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 11;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast32_t samplefreq1 = dsp_get_samplerateuacin_RTS192();
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;// CS_INTERFACE Descriptor Type (bDescriptorType) 0x24
		* buff ++ = AUDIO_STREAMING_FORMAT_TYPE;   // FORMAT_TYPE subtype. (bDescriptorSubtype) 0x02
		* buff ++ = AUDIO_FORMAT_TYPE_I;							/* bFormatType */
		* buff ++ = UACIN_FMT_CHANNELS_RTS192;		/* bNrChannels */
		* buff ++ = UACIN_RTS192_SAMPLEBYTES;		//bSubFrameSize
		* buff ++ = UACIN_RTS192_SAMPLEBYTES * 8;	//bBitResolution
		* buff ++ = 1;										/* bSamFreqType only one frequency supported */
		* buff ++ = LO_BYTE(samplefreq1);	/* Audio sampling frequency coded on 3 bytes */
		* buff ++ = HI_BYTE(samplefreq1);
		* buff ++ = HI_24BY(samplefreq1);
	}
	return length;
}

/* USB Microphone Standard Endpoint Descriptor (CODE == 8)*/ //Standard AS Isochronous Audio Data Endpoint Descriptor
// Endpoint Descriptor 82 2 In, Isochronous, 125 us
static unsigned UAC1_EP_Desc_RTS192(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress, uint8_t offset)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(UACIN_RTS192_DATASIZE); // was: 0x300
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	// bDescriptorType
		* buff ++ = bEndpointAddress;                    // bEndpointAddress
		* buff ++ = USBD_UAC1_IN_EP_ATTRIBUTES; // bmAttributes
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize);
		* buff ++ = highspeed ? HSINTERVAL_RTS192 : FSINTERVAL_RTS192;    /* bInterval */
		* buff ++ = 0x00;                       // Unused. (bRefresh)
		* buff ++ = 0x00;                       // Unused. (bSynchAddress)
	}
	return length;
}

#endif /* WITHRTS192 */

// 4.9.2 Class-Specific AS Interface Descriptor
// Количество каналов определается тут
static unsigned UAC2_AS_InterfaceDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bTerminalLink, uint_fast32_t bmChannelConfig)
{
	const uint_fast8_t length = 16;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast32_t bmFormats = 1u << 0;	// PCM
		const uint_fast8_t bmControls = 0x00;	// D1..0: Active Alternate Setting Control
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		// Table 4-27: Class-Specific AS Interface Descriptor
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;      /* bDescriptorType CS_INTERFACE */
		* buff ++ = AUDIO_STREAMING_GENERAL;              /* bDescriptorSubtype AS_GENERAL */
		* buff ++ = bTerminalLink;                        /* bTerminalLink */
		* buff ++ = bmControls;		/* bmControls */
		* buff ++ = AUDIO_FORMAT_TYPE_I;	//	bFormatType   (FORMAT_TYPE_1)
		* buff ++ = LO_BYTE(bmFormats);                  /* bmFormats */
		* buff ++ = HI_BYTE(bmFormats);
		* buff ++ = HI_24BY(bmFormats);
		* buff ++ = HI_32BY(bmFormats);
		* buff ++ = UAC2_count_channels(bmChannelConfig);		/* bNrChannels   */
		* buff ++ = LO_BYTE(bmChannelConfig);                 /* bmChannelConfig */
		* buff ++ = HI_BYTE(bmChannelConfig);
		* buff ++ = HI_24BY(bmChannelConfig);
		* buff ++ = HI_32BY(bmChannelConfig);
		* buff ++ = STRING_ID_Left;	/* iChannelNames */
		/* 16 byte*/
	}
	return length;
}

#if WITHUSBUACIN2

static unsigned fill_UAC2_INRTS_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed, uint_fast8_t offset)
{
	const uint_fast8_t rtscontrolifv = INTERFACE_AUDIO_CONTROL_RTS;
	const uint_fast8_t rtsifv = INTERFACE_AUDIO_RTS;
	const uac_pathfn_t rtspath = UAC2_TopologyINRTS;
	const uint_fast8_t terminalID = UACTEix(TERMINAL_ID_USBSTREAM, offset);
	const tpgt_t rtsterms =
	{
			.bTerminalID = terminalID,
			.bFeatureUnitID = UACTEix(TERMINAL_ID_FU2a_IN, offset),
			.bFeatureUnit2ID = UACTEix(TERMINAL_ID_FU2b_IN, offset),	// промежуточный terminal этой топологии
//		    .bOSC = UACTEix(TERMINAL_ID_CLKSOURCE, offset),
//		    .bCSourceID = UACTEix(TERMINAL_ID_CLKMULTIPLIER, offset),
			.bCSourceID = UACTEix(TERMINAL_ID_CLKSOURCE, offset),
	};
	const tpgt_t * const rtstermsv [] = { & rtsterms };
	const uint_fast8_t iInterfaceINRTS = STRING_ID_MODE0 + offset;
	const uint_fast8_t epinrts = USB_ENDPOINT_IN(USBD_EP_RTS_IN);
	uint_fast8_t ialt = 0;
	unsigned n = 0;

	n += UAC2_InterfaceAssociationDesc(fill, p + n, maxsize - n, rtscontrolifv, 2, offset);	/* INTERFACE_AUDIO_CONTROL_SPK Interface Association Descriptor Audio */

	// IN data flow: USB Microphone
	// INTERFACE_AUDIO_MIKE - audio streaming interface
	n += UAC2_AC_InterfaceDesc(fill, p + n, maxsize - n, rtscontrolifv, 0x00, offset);	/* INTERFACE_AUDIO_CONTROL_RTS - Interface Descriptor 0/0 Audio, 0 Endpoints */
	n += UAC2_HeaderDescriptor(fill, p + n, maxsize - n, & rtsifv, & rtspath, rtstermsv, 1, offset);	/* bcdADC Audio Control Interface Header Descriptor */
	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, rtsifv, ialt ++, 0, offset, iInterfaceINRTS);	/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3) */ //zero-bandwidth interface

#if WITHRTS96
	// IN data flow: radio RX spectrum data
	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, rtsifv, ialt ++, 1, offset, iInterfaceINRTS);	/* INTERFACE_AUDIO_RTS_2 Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC2_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalID, UACIN_CONFIG_INRTS);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
	n += UAC2_FormatTypeDesc_RTS96(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC2_EP_Desc_IN_RTS96(fill, p + n, maxsize - n, highspeed, epinrts, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += UAC2_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* WITHRTS96 */

#if WITHRTS192
	// IN data flow: radio RX spectrum data
	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, rtsifv, ialt ++, 1, offset, iInterfaceINRTS);	/* INTERFACE_AUDIO_RTS_2 Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC2_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalID, UACIN_CONFIG_INRTS);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
	n += UAC2_FormatTypeDescroptor_RTS192(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC2_fill_27_RTS192(fill, p + n, maxsize - n, highspeed, epinrts, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += UAC2_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* WITHRTS192 */
	return n;
}

#endif /* WITHUSBUACIN2 */

#if WITHUSBUACIN

// AUDIO48 only IN (radio to host) audio function
static unsigned fill_UAC2_IN48_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed, uint_fast8_t offset)
{
	uint_fast8_t ialt = 0;
	unsigned n = 0;
	const uint_fast8_t controlifv = INTERFACE_AUDIO_CONTROL_MIKE;	/* AUDIO receiever out control interface */
	const uint_fast8_t mikeifv = INTERFACE_AUDIO_MIKE;
	const uac_pathfn_t mikepath = UAC2_TopologyIN48;
	const uint_fast8_t terminalID = UACTEix(TERMINAL_ID_USBSTREAM, offset);
	const tpgt_t miketerms =
	{
			.bTerminalID = terminalID,
			.bFeatureUnitID = UACTEix(TERMINAL_ID_FU2a_IN, offset),
			.bFeatureUnit2ID = UACTEix(TERMINAL_ID_FU2b_IN, offset),	// промежуточный terminal этой топологии
//			.bOSC = UACTEix(TERMINAL_ID_CLKSOURCE, offset),
//			.bCSourceID = UACTEix(TERMINAL_ID_CLKMULTIPLIER, UACOFFS_IN48),
			.bCSourceID = UACTEix(TERMINAL_ID_CLKSOURCE, offset),
	};
	const tpgt_t * const miketermsv [] = { & miketerms };
	const uint_fast8_t iInterfaceIN48 = STRING_ID_MODE0 + offset;

	const uint_fast8_t epin = USB_ENDPOINT_IN(USBD_EP_AUDIO_IN);

	n += UAC2_InterfaceAssociationDesc(fill, p + n, maxsize - n, controlifv, 2, offset);	/* INTERFACE_AUDIO_CONTROL_SPK Interface Association Descriptor Audio */
	// INTERFACE_AUDIO_CONTROL_SPK - audio control interface
	n += UAC2_AC_InterfaceDesc(fill, p + n, maxsize - n, controlifv, 0x00, offset);	/* INTERFACE_AUDIO_CONTROL_SPK - Interface Descriptor 0/0 Audio, 0 Endpoints */
	n += UAC2_HeaderDescriptor(fill, p + n, maxsize - n, & mikeifv, & mikepath, miketermsv, 1, offset);	/* bcdADC Audio Control Interface Header Descriptor */

	// IN data flow: off
	// INTERFACE_AUDIO_MIKE - audio streaming interface
	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 0, offset, iInterfaceIN48);	/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3) */ //zero-bandwidth interface

	// IN data flow: radio RX audio data
	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 1, offset, iInterfaceIN48);	/* INTERFACE_AUDIO_MIKE Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC2_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalID, UACIN_CONFIG_IN48);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
	n += UAC2_FormatTypeDescroptor_IN48(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC2_EP_Desc_IN48(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += UAC2_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - ???????????? ?????????????*/

	return n;
}


/* на одном устройстве различные форматы для передачи в компьютер спектра и звука */
static unsigned fill_UAC2_IN48_INRTS_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed, uint_fast8_t offset)
{
	uint_fast8_t ialt = 0;
	unsigned n = 0;
	const uint_fast8_t controlifv = INTERFACE_AUDIO_CONTROL_MIKE;
	const uint_fast8_t mikeifv = INTERFACE_AUDIO_MIKE;
	const uint_fast8_t epin = USB_ENDPOINT_IN(USBD_EP_AUDIO_IN);
	const uac_pathfn_t mikepath = UAC2_TopologyIN48_INRTS;
	const uint_fast8_t terminalID = UACTEix(TERMINAL_ID_USBSTREAM, offset);
	const tpgt_t miketerms =
	{
			.bTerminalID = terminalID,
			.bFeatureUnitID = UACTEix(TERMINAL_ID_FU2a_IN, offset),
			.bFeatureUnit2ID = UACTEix(TERMINAL_ID_FU2b_IN, offset),	// промежуточный terminal этой топологии
//		    .bOSC = UACTEix(TERMINAL_ID_CLKSOURCE, offset),
//		    .bCSourceID = UACTEix(TERMINAL_ID_CLKMULTIPLIER, offset),
			.bCSourceID = UACTEix(TERMINAL_ID_CLKSOURCE, offset),
	};
	const tpgt_t * const miketermsv [] = { & miketerms };
	const uint_fast8_t iInterfaceIN48_INRTS = STRING_ID_MODE0 + offset;

	n += UAC2_InterfaceAssociationDesc(fill, p + n, maxsize - n, controlifv, 2, offset);	/* INTERFACE_AUDIO_CONTROL_SPK Interface Association Descriptor Audio */
	// INTERFACE_AUDIO_CONTROL_SPK - audio control interface
	n += UAC2_AC_InterfaceDesc(fill, p + n, maxsize - n, controlifv, 0x00, offset);	/* INTERFACE_AUDIO_CONTROL_SPK - Interface Descriptor 0/0 Audio, 0 Endpoints */
	n += UAC2_HeaderDescriptor(fill, p + n, maxsize - n, & mikeifv, & mikepath, miketermsv, 1, offset);	/* bcdADC Audio Control Interface Header Descriptor */

	// IN data flow: off
	// INTERFACE_AUDIO_MIKE - audio streaming interface
	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 0, offset, iInterfaceIN48_INRTS);	/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3) */ //zero-bandwidth interface

	// IN data flow: demodulator
	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 1, offset, iInterfaceIN48_INRTS);	/* INTERFACE_AUDIO_MIKE Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC2_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalID, UACIN_CONFIG_IN48);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
	n += UAC2_FormatTypeDescroptor_IN48(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC2_EP_Desc_IN48(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += UAC2_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/

#if WITHRTS96
	// IN data flow: radio RX spectrum data
	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 1, offset, iInterfaceIN48_INRTS);	/* INTERFACE_AUDIO_MIKE Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC2_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalID, UACIN_CONFIG_INRTS);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
	n += UAC2_FormatTypeDesc_RTS96(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC2_EP_Desc_IN_RTS96(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += UAC2_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* WITHRTS96 */

#if WITHRTS192
	// IN data flow: radio RX spectrum data
	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 1, offset, iInterfaceIN48_INRTS);	/* INTERFACE_AUDIO_MIKE Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC2_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalID, UACIN_CONFIG_INRTS);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
	n += UAC2_AS_EP_Desc_RTS192(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC2_fill_27_RTS192(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += UAC2_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* WITHRTS192 */
	return n;
}

#endif /* WITHUSBUACIN */

#if WITHUSBUACOUT

// AUDIO48 only OUT (host to radio) audio function
static unsigned fill_UAC2_OUT48_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed, uint_fast8_t offset)
{
	uint_fast8_t ialt = 0;
	unsigned n = 0;
	const uint_fast8_t controlifv = INTERFACE_AUDIO_CONTROL_SPK;	/* AUDIO transmitter input control interface */
	const uint_fast8_t modulatorifv = INTERFACE_AUDIO_SPK;
	const uac_pathfn_t modulatorpath = UAC2_TopologyOUT48;
	const uint_fast8_t terminalID = UACTEix(TERMINAL_ID_USBSTREAM, offset);
	const tpgt_t modulatorterms =
	{
			.bTerminalID = terminalID,
			.bFeatureUnitID = UACTEix(TERMINAL_ID_FU2a_OUT, offset),
			.bFeatureUnit2ID = UACTEix(TERMINAL_ID_FU2b_OUT, offset),	// промежуточный terminal этой топологии
			//.bOSC = UACTEix(TERMINAL_ID_CLKSOURCE, offset),
			//.bCSourceID = UACTEix(TERMINAL_ID_CLKMULTIPLIER, UACOFFS_OUT48),
			.bCSourceID = UACTEix(TERMINAL_ID_CLKSOURCE, offset),

	};
	const tpgt_t * const modulatortermsv [] = { & modulatorterms };
	const uint_fast8_t bNumEndpoints = 1;//2;
	const uint_fast8_t epout = USB_ENDPOINT_OUT(USBD_EP_AUDIO_OUT);
	//const uint_fast8_t epoutfeedback = USB_ENDPOINT_IN(USBD_EP_AUDIO_IN_FB);
	const uint_fast8_t iInterfaceOUT48 = STRING_ID_MODE0 + offset;

	n += UAC2_InterfaceAssociationDesc(fill, p + n, maxsize - n, controlifv, 2, offset);	/* INTERFACE_AUDIO_CONTROL_SPK Interface Association Descriptor Audio */
	// INTERFACE_AUDIO_CONTROL_SPK - modulator audio control interface
	n += UAC2_AC_InterfaceDesc(fill, p + n, maxsize - n, controlifv, 0x00, offset);	/* INTERFACE_AUDIO_CONTROL_SPK - Interface Descriptor 0/0 Audio, 0 Endpoints */
	n += UAC2_HeaderDescriptor(fill, p + n, maxsize - n, & modulatorifv, & modulatorpath, modulatortermsv, 1, offset);	/* bcdADC Audio Control Interface Header Descriptor */

	// OUT data flow: off
	// INTERFACE_AUDIO_SPK - audio streaming interface
	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, modulatorifv, ialt ++, 0, offset, iInterfaceOUT48);	/* INTERFACE_AUDIO_SPK - Interface 1, Alternate Setting 0 */

	// OUT data flow: modulator
	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, modulatorifv, ialt ++, bNumEndpoints, offset, iInterfaceOUT48);	/* INTERFACE_AUDIO_SPK -  Interface 1, Alternate Setting 1 */
	n += UAC2_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalID, UACOUT_CONFIG_OUT48);	/* USB Speaker Audio Streaming Interface Descriptor (for output TERMINAL_UACOUT48 + offset) */
	n += UAC2_FormatTypeDesc_OUT48(fill, p + n, maxsize - n);	/* USB Speaker Audio Type I Format Interface Descriptor (one sample rate) 48000 */
	n += UAC2_EP_Desc_OUT48(fill, p + n, maxsize - n, epout, highspeed);	/* Endpoint USBD_EP_AUDIO_OUT - Standard Descriptor */
	//n += UAC2_EP_Feedback_Desc_OUT48(fill, p + n, maxsize - n, epoutfeedback, highspeed);	/* Endpoint USBD_EP_AUDIO_OUT - Standard Descriptor */
	n += UAC2_AS_EP_Desc_OUT48(fill, p + n, maxsize - n);	/* Endpoint - Audio Streaming Descriptor */

	return n;
}
#endif /* WITHUSBUACOUT */

/* UAC IAD */
// Interface Association Descriptor Audio
// Audio10.pdf 4.3.2.8 Associated Interface Descriptor
// documented in USB ECN : Interface Association Descriptor - InterfaceAssociationDesc_ecn.pdf
static unsigned UAC1_InterfaceAssociationDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bFirstInterface,
	uint_fast8_t bInterfaceCount,
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 8;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_ASSOC_DESCRIPTOR_TYPE;	// bDescriptorType: IAD
		* buff ++ = bFirstInterface;			// bFirstInterface
		* buff ++ = bInterfaceCount;	// bInterfaceCount
		* buff ++ = USB_DEVICE_CLASS_AUDIO;	// bFunctionClass: Audio
		* buff ++ = 0x00;	// bFunctionSubClass
		* buff ++ = 0x00;	// bFunctionProtocol
		* buff ++ = STRING_ID_MODE0 + offset;	// Interface string index
	}
	return length;
}

/* USB Speaker Standard interface descriptor */
// Interface Descriptor 0/0 Audio, 0 Endpoints
static unsigned UAC1_AC_InterfaceDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bInterfaceNumber,
	uint_fast8_t bAlternateSetting,
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;      /* bDescriptorType */
		* buff ++ = bInterfaceNumber;					/* bInterfaceNumber */
		* buff ++ = bAlternateSetting;					/* bAlternateSetting */
		* buff ++ = 0x00;                               /* bNumEndpoints */
		* buff ++ = USB_DEVICE_CLASS_AUDIO;             /* bInterfaceClass */
		* buff ++ = AUDIO_SUBCLASS_AUDIOCONTROL;        /* bInterfaceSubClass */
		* buff ++ = AUDIO_PROTOCOL_UNDEFINED;           /* bInterfaceProtocol */
		* buff ++ = STRING_ID_0;           			    /* iInterface */
		/* 09 byte*/
	}
	return length;
}

// Audio Control Input Terminal Descriptor
// audio10.pdf: Table 4-3: Input Terminal Descriptor
// Sereo signal source
// Audio only
static unsigned UAC1_AC_IT_IN48(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bTerminalID, uint_fast8_t offset)
{
	const uint_fast8_t length = 12;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 4.3.2.1 Input Terminal Descriptor
		const uint_fast16_t wTerminalType = AUDIO_TERMINAL_RADIO_RECEIVER;
		const uint_fast16_t wChannelConfig = UACIN_CONFIG_IN48;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE; // CS_INTERFACE Descriptor Type
		* buff ++ = AUDIO_CONTROL_INPUT_TERMINAL;    // INPUT_TERMINAL 0x02 descriptor subtype
		* buff ++ = bTerminalID;                   // bTerminalID ID of this Terminal.
		* buff ++ = LO_BYTE(wTerminalType);			/* wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;        // bAssocTerminal No association
		// The bNrChannels, wChannelConfig and iChannelNames fields together constitute the cluster descriptor
		* buff ++ = UAC1_count_channels(wChannelConfig);    /* bNrChannels */
		* buff ++ = LO_BYTE(wChannelConfig);   /* bmChannelConfig size = 4 bytes Mono sets no position bits */
		* buff ++ = HI_BYTE(wChannelConfig);
		* buff ++ = STRING_ID_Left;							/* iChannelNames */
		* buff ++ = 0;//STRING_ID_MODE0 + offset;						// iTerminal - Index of a string descriptor, describing the Input Terminal. Receiver Output

	}
	return length;
}


// Audio Control Input Terminal Descriptor
// audio10.pdf: Table 4-3: Input Terminal Descriptor
// Sereo signal source
// Audio или RTS
static unsigned UAC1_AC_IT_IN48_INRTS(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bTerminalID, uint_fast8_t offset)
{
	const uint_fast8_t length = 12;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 4.3.2.1 Input Terminal Descriptor
		const uint_fast16_t wTerminalType = AUDIO_TERMINAL_RADIO_RECEIVER;
		const uint_fast16_t wChannelConfig = UACIN_CONFIG_IN48_INRTS;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE; // CS_INTERFACE Descriptor Type
		* buff ++ = AUDIO_CONTROL_INPUT_TERMINAL;    // INPUT_TERMINAL 0x02 descriptor subtype
		* buff ++ = bTerminalID;                   // bTerminalID ID of this Terminal.
		* buff ++ = LO_BYTE(wTerminalType);			/* wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;        // bAssocTerminal No association
		// The bNrChannels, wChannelConfig and iChannelNames fields together constitute the cluster descriptor
		* buff ++ = UAC1_count_channels(wChannelConfig);    /* bNrChannels */
		* buff ++ = LO_BYTE(wChannelConfig);   /* bmChannelConfig size = 4 bytes Mono sets no position bits */
		* buff ++ = HI_BYTE(wChannelConfig);
		* buff ++ = STRING_ID_Left;							/* iChannelNames */
		* buff ++ = 0;//STRING_ID_MODE0 + offset;						// iTerminal - Index of a string descriptor, describing the Input Terminal. Receiver Output

	}
	return length;
}

static unsigned UAC1_AC_IT_INRTS(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bTerminalID, uint_fast8_t offset)
{
	const uint_fast8_t length = 12;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 4.3.2.1 Input Terminal Descriptor
		const uint_fast16_t wTerminalType = AUDIO_TERMINAL_RADIO_RECEIVER;
		const uint_fast16_t wChannelConfig = UACIN_CONFIG_INRTS;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE; // CS_INTERFACE Descriptor Type
		* buff ++ = AUDIO_CONTROL_INPUT_TERMINAL;    // INPUT_TERMINAL 0x02 descriptor subtype
		* buff ++ = bTerminalID;                   // bTerminalID ID of this Terminal.
		* buff ++ = LO_BYTE(wTerminalType);			/* wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;        // bAssocTerminal No association
		// The bNrChannels, wChannelConfig and iChannelNames fields together constitute the cluster descriptor
		* buff ++ = UAC1_count_channels(wChannelConfig);    /* bNrChannels */
		* buff ++ = LO_BYTE(wChannelConfig);   /* bmChannelConfig size = 4 bytes Mono sets no position bits */
		* buff ++ = HI_BYTE(wChannelConfig);
		* buff ++ = STRING_ID_Left;							/* iChannelNames */
		* buff ++ = STRING_ID_IQSPECTRUM;		// iTerminal - Index of a string descriptor, describing the Input Terminal. Receiver Output

	}
	return length;
}

/* !USB Speaker Input Terminal Descriptor */
// Audio Control Input Terminal Descriptor
// audio10.pdf: Table 4-3: Input Terminal Descriptor
// audio48 only
static unsigned UAC1_AC_IT_OUT48(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bTerminalID,
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 12;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 4.3.2.1 Input Terminal Descriptor
		const uint_fast16_t wTerminalType = AUDIO_TERMINAL_USB_STREAMING;
		const uint_fast16_t wChannelConfig = UACOUT_CONFIG_OUT48;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;									/* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;		/* bDescriptorType */
		* buff ++ = AUDIO_CONTROL_INPUT_TERMINAL;			/* bDescriptorSubtype */
		* buff ++ = bTerminalID;							/* bTerminalID */
		* buff ++ = LO_BYTE(wTerminalType);					/* wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;					/* bAssocTerminal */
		// The bNrChannels, wChannelConfig and iChannelNames fields together constitute the cluster descriptor
		* buff ++ = UAC1_count_channels(wChannelConfig);							/* bNrChannels */
		* buff ++ = LO_BYTE(wChannelConfig);                /* wChannelConfig 0x0003  Front Left; Front Right */
		* buff ++ = HI_BYTE(wChannelConfig);
		* buff ++ = STRING_ID_Left;							/* iChannelNames */
		* buff ++ = STRING_ID_MODE0 + offset;					/* iTerminal - появляется как pop-up в панели управления ASIO4ALL */
		/* 12 bytes*/
	}
	return length;
}

/*! USB Microphone Output Terminal Descriptor bSourceID -> bTerminalID */
// Audio Control Output Terminal Descriptor
// Audio или RTS
static unsigned UAC1_AC_OT_IN(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bTerminalID,
	uint_fast8_t bSourceID,
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		//
		const uint_fast16_t wTerminalType = AUDIO_TERMINAL_USB_STREAMING;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE; // CS_INTERFACE Descriptor Type (bDescriptorType)
		* buff ++ = AUDIO_CONTROL_OUTPUT_TERMINAL;   // OUTPUT_TERMINAL descriptor subtype (bDescriptorSubtype)
		* buff ++ = bTerminalID;                            // ID of this Terminal. (bTerminalID)
		* buff ++ = LO_BYTE(wTerminalType);					/* wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;           // unused         (bAssocTerminal)
		* buff ++ = bSourceID;                            // From Input Terminal.(bSourceID)
		* buff ++ = STRING_ID_MODE0 + offset;					// unused  (iTerminal)
	}
	return length;
}

#if 0
// Selector Unit Descriptor
static unsigned UAC1_AudioSelectorUnit_IN(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bUnitID
	)
{
	//unsigned i;
	const uint_fast8_t bNrInPins = TERMINAL_ID_SELECTOR_6_INPUTS;	// количество входных потоков
	const uint_fast8_t length = 6 + bNrInPins;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		* buff ++ = length;							/* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;/* bDescriptorType */
		* buff ++ = AUDIO_CONTROL_SELECTOR_UNIT;	/* bDescriptorSubtype */
		* buff ++ = bUnitID;             			/* bUnitID */
		* buff ++ = bNrInPins;             			/* bNrInPins */
		* buff ++ = TERMINAL_ID_IT_2;				/* baSourceID(0) */
		* buff ++ = TERMINAL_ID_FU_5c;				/* baSourceID(1) */
		* buff ++ = 0;								/* iSelector (string ID) - unused */
	}
	return length;
}
#endif

// Audio Control Feature Unit Descriptor
// See 4.3.2.5 Feature Unit Descriptor for details
// В нашем случае используется для подавления отображения раздельных элементов регулировки уровня по каналам
static unsigned UAC1_AudioFeatureUnit(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bUnitID,
	uint_fast8_t bSourceID,
	uint_fast8_t offset
	)
{
	// Параметр определяет, ккие управляющие элементы появляются на страничке управления "Custom"
	// Причем, на этой страничке собраны все управляющие элементы со всех Feature Unit Descriptor
	// в пути сигнала данного источника звука.
	// Не может быть нулём.
	const uint_fast32_t bmaControls0 =
		AUDIO_CONTROL_MUTE |
		AUDIO_CONTROL_VOLUME |
		//AUDIO_CONTROL_AUTOMATIC_GAIN |
		//AUDIO_CONTROL_GRAPHIC_EQUALIZER |
		//AUDIO_CONTROL_LOUDNESS |		// "Custom" property page added
		0;
	const uint_fast32_t bmaControlsv [] =
	{
		bmaControls0,
	};
	const uint_fast8_t ch = ARRAY_SIZE(bmaControlsv); // 1: Only master channel controls, 3: master, left and right
	const uint_fast8_t bControlSize = 2;	/* Достаточно, чтобы вместить все определенные для bmaControls биты */
	const uint_fast8_t length = 7 + bControlSize * ch;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		uint_fast8_t i;
		// See 4.3.2.5 Feature Unit Descriptor for details
		* buff ++ = length;							/* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;/* bDescriptorType */
		* buff ++ = AUDIO_CONTROL_FEATURE_UNIT;     /* bDescriptorSubtype */
		* buff ++ = bUnitID;             			/* bUnitID */
		* buff ++ = bSourceID;						/* bSourceID */
		* buff ++ = bControlSize;                   /* bControlSize - колтчество элементов в следующем элементе, повторяющемся для каждого канала */
		for (i = 0; i < ch; ++ i)
		{
			uint_fast32_t v = bmaControlsv [i];
			uint_fast8_t cs = bControlSize;
			while (cs --)
			{
				* buff ++ = (uint8_t) v;
				v >>= 8;
			}
		}
		* buff ++ = STRING_ID_MODE0 + offset;                    /* iTerminal */
		/* 10 byte*/
	}
	return length;
}

//
// Если выход AUDIO_TERMINAL_RADIO_TRANSMITTER, закладки enchancements нет
// Audio Control Output Terminal Descriptor
// bSourceID -> bTerminalID
// audio48 only
static unsigned UAC1_AC_OT_OUT(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bTerminalID,
	uint_fast8_t bSourceID,
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 4.3.2.2 Output Terminal Descriptor
		const uint_fast16_t wTerminalType = AUDIO_TERMINAL_RADIO_TRANSMITTER;

		* buff ++ = length;							/* 0 bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;/* 1 bDescriptorType */
		* buff ++ = AUDIO_CONTROL_OUTPUT_TERMINAL;  /* 2 bDescriptorSubtype */
		* buff ++ = bTerminalID;					/* 3 bTerminalID */
		* buff ++ = LO_BYTE(wTerminalType);			/* 4 wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;          /* 6 bAssocTerminal */
		* buff ++ = bSourceID;						/* 7 bSourceID */
		* buff ++ = 0;//STRING_ID_MODE0 + offset;					// unused  (iTerminal)
		/* 9 byte*/
	}
	return length;
}

// Заполнение схемы ввода звука
// IN data flow
// Элементы добавлояются в дескриптор в порядке обратном порядку прохождения информационного потока
//
static unsigned UAC1_TopologyIN48(
	uint_fast8_t fill, uint8_t * p, unsigned maxsize,
	const tpgt_t * tgpt,
	uint_fast8_t offset
	)
{
	unsigned n = 0;

	n += UAC1_AC_OT_IN(fill, p + n, maxsize - n,  tgpt->bTerminalID, tgpt->bFeatureUnitID, offset);	/* AUDIO_TERMINAL_USB_STREAMING Terminal Descriptor TERMINAL_ID_FU_AUDIO -> TERMINAL_UACIN48_UACINRTS */
	n += UAC1_AudioFeatureUnit(fill, p + n, maxsize - n, tgpt->bFeatureUnitID, tgpt->bFeatureUnit2ID, offset);	/* USB microphone Audio Feature Unit Descriptor TERMINAL_UACOUT48 -> TERMINAL_ID_FU_AUDIO */
	n += UAC1_AC_IT_IN48(fill, p + n, maxsize - n, tgpt->bFeatureUnit2ID, offset);	/* AUDIO_TERMINAL_RADIO_RECEIVER */

	return n;
}

// Заполнение схемы ввода звука
// IN data flow
// Элементы добавлояются в дескриптор в порядке обратном порядку прохождения информационного потока
//
static unsigned UAC1_TopologyIN48_INRTS(
	uint_fast8_t fill, uint8_t * p, unsigned maxsize,
	const tpgt_t * tgpt,
	uint_fast8_t offset
	)
{
	unsigned n = 0;

	n += UAC1_AC_OT_IN(fill, p + n, maxsize - n,  tgpt->bTerminalID, tgpt->bFeatureUnitID, offset);	/* AUDIO_TERMINAL_USB_STREAMING Terminal Descriptor TERMINAL_ID_FU_AUDIO -> TERMINAL_UACIN48_UACINRTS */
	n += UAC1_AudioFeatureUnit(fill, p + n, maxsize - n, tgpt->bFeatureUnitID, tgpt->bFeatureUnit2ID, offset);	/* USB microphone Audio Feature Unit Descriptor TERMINAL_UACOUT48 -> TERMINAL_ID_FU_AUDIO */
	n += UAC1_AC_IT_IN48_INRTS(fill, p + n, maxsize - n, tgpt->bFeatureUnit2ID, offset);	/* AUDIO_TERMINAL_RADIO_RECEIVER */

	return n;
}

// Заполнение схемы вывода звука
// OUT data flow
// audio48 only
static unsigned UAC1_TopologyOUT48(
	uint_fast8_t fill, uint8_t * p, unsigned maxsize,
	const tpgt_t * tgpt,
	uint_fast8_t offset
	)
{
	unsigned n = 0;

	n += UAC1_AC_IT_OUT48(fill, p + n, maxsize - n, tgpt->bTerminalID, offset);	/* AUDIO_TERMINAL_USB_STREAMING Input Terminal Descriptor TERMINAL_UACOUT48 */
	n += UAC1_AudioFeatureUnit(fill, p + n, maxsize - n, tgpt->bFeatureUnitID, tgpt->bTerminalID, offset);	/* USB Speaker Audio Feature Unit Descriptor TERMINAL_UACOUT48 -> TERMINAL_ID_FU_5 */
	n += UAC1_AC_OT_OUT(fill, p + n, maxsize - n, tgpt->bFeatureUnit2ID, tgpt->bFeatureUnitID, offset);	/* AUDIO_TERMINAL_RADIO_TRANSMITTER Output Terminal Descriptor TERMINAL_ID_FU_5 -> TERMINAL_ID_OT_3 */

	return n;
}

// Элементы добавлояются в дескриптор в порядке обратном порядку прохождения информационного потока
//
static unsigned UAC1_TopologyINRTS(
	uint_fast8_t fill, uint8_t * p, unsigned maxsize,
	const tpgt_t * tgpt,
	uint_fast8_t offset
	)
{
	unsigned n = 0;

	n += UAC1_AC_OT_IN(fill, p + n, maxsize - n,  tgpt->bTerminalID, tgpt->bFeatureUnitID, offset);	/* AUDIO_TERMINAL_USB_STREAMING Terminal Descriptor TERMINAL_ID_IT_2 -> TERMINAL_UACIN48_UACINRTS */
	n += UAC1_AudioFeatureUnit(fill, p + n, maxsize - n, tgpt->bFeatureUnitID, tgpt->bFeatureUnit2ID, offset);	/* USB microphone Audio Feature Unit Descriptor TERMINAL_UACOUT48 -> TERMINAL_ID_FU_RTS */
	n += UAC1_AC_IT_INRTS(fill, p + n, maxsize - n, tgpt->bFeatureUnit2ID, offset);	/* AUDIO_TERMINAL_RADIO_RECEIVER */

	return n;
}

/* USB Speaker Class-specific AC Interface Descriptor */
// Audio Control Interface Header Descriptor
static unsigned UAC1_HeaderDescriptor(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	const uint_fast8_t * coll,		// array of data interfaces in this functional device
	const uac_pathfn_t * paths,
	const tpgt_t * const * tgptv,
	uint_fast8_t bInCollection,		// size of array
	uint_fast8_t offset
	)
{
	uint_fast8_t i;
	unsigned terminalsLength = 0;
	const uint_fast8_t length = 8 + bInCollection;
	for (i = 0; i < bInCollection; ++ i)
		terminalsLength += (paths [i])(0, buff + length + terminalsLength, maxsize - length - terminalsLength, tgptv [i], offset);
	const unsigned wTotalLength = terminalsLength + length;
	ASSERT(maxsize >= wTotalLength);
	if (maxsize < wTotalLength)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 4.3.2 Class-Specific AC Interface Descriptor
		const uint_fast16_t bcdADC = 0x0100;	// Revision of class specification - 1.0
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;/* bDescriptorType */
		* buff ++ = AUDIO_CONTROL_HEADER;           /* bDescriptorSubtype */
		* buff ++ = LO_BYTE(bcdADC);				/* bcdADC */
		* buff ++ = HI_BYTE(bcdADC);
		* buff ++ = LO_BYTE(wTotalLength);			/* wTotalLength */
		* buff ++ = HI_BYTE(wTotalLength);
		* buff ++ = bInCollection;					/* bInCollection=2:  1 - AudioStreaming Out; 2 - AudioStreaming In*/
		for (i = 0; i < bInCollection; ++ i)
			* buff ++ = coll [i];					/* baInterfaceNr(i) */
		/* 10 bytes*/
		terminalsLength = 0;
		for (i = 0; i < bInCollection; ++ i)
			terminalsLength += (paths [i])(fill, buff + terminalsLength, maxsize - length - terminalsLength, tgptv [i], offset);
	}
	return wTotalLength;

}

/* USB Speaker Audio Type I Format Interface Descriptor */
// USBLyzer: Audio Streaming Format Type Descriptor
static unsigned UAC1_FormatTypeDesc_OUT48(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 11;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast32_t samplefreq1 = dsp_get_samplerateuacout();
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;		/* bDescriptorType */
		* buff ++ = AUDIO_STREAMING_FORMAT_TYPE;			/* bDescriptorSubtype */
		* buff ++ = AUDIO_FORMAT_TYPE_I;							/* bFormatType */
		* buff ++ = UACOUT_FMT_CHANNELS_AUDIO48;		/* bNrChannels */
		* buff ++ = UACOUT_AUDIO48_SAMPLEBYTES; /* bSubFrameSize :  2 Bytes per frame (16bits) */
		* buff ++ = UACOUT_AUDIO48_SAMPLEBYTES * 8;		/* bBitResolution (16-bits per sample) */
		* buff ++ = 1;										/* bSamFreqType only one frequency supported */
		* buff ++ = LO_BYTE(samplefreq1);	/* Audio sampling frequency coded on 3 bytes */
		* buff ++ = HI_BYTE(samplefreq1);
		* buff ++ = HI_24BY(samplefreq1);
		/* 11 byte*/
	}
	return length;
}

/* Endpoint 1 - Standard Descriptor */
// out: from computer to our device
static unsigned UAC1_EP_Desc_OUT48(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress, int highspeed)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(usbd_getuacoutmaxpacket());
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;         /* bDescriptorType */
		* buff ++ = bEndpointAddress;               /* bEndpointAddress 1 out endpoint*/
		* buff ++ = USBD_UAC1_OUT_EP_ATTRIBUTES;       						    /* bmAttributes */
		* buff ++ = LO_BYTE(wMaxPacketSize);              /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize);
		* buff ++ = highspeed ? HSINTERVAL_AUDIO48 : FSINTERVAL_AUDIO48;    /* bInterval */
		* buff ++ = 0x00;                                 /* bRefresh */
		* buff ++ = 0;                       /* bSynchAddress */
		/* 9 byte*/
	}
	return length;
}

// Interface Descriptor
static unsigned UAC1_InterfaceDesc(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bInterfaceNumber,
	uint_fast8_t bAlternateSetting,
	uint_fast8_t bNumEndpoints,
	uint_fast8_t offset,
	uint_fast8_t iInterface		/* название цстройства, появляется в списке устройств на MAC OS */
	)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;  // INTERFACE descriptor type (bDescriptorType)
		* buff ++ = bInterfaceNumber;			// Index of this interface. (bInterfaceNumber)
		* buff ++ = bAlternateSetting;				// Index of this alternate setting. (bAlternateSetting) - zero-based index
		* buff ++ = bNumEndpoints;					// bNumEndpoints
		* buff ++ = USB_DEVICE_CLASS_AUDIO;			// AUDIO (bInterfaceClass)
		* buff ++ = AUDIO_SUBCLASS_AUDIOSTREAMING;  // AUDIO_STREAMING (bInterfaceSubclass)
		* buff ++ = AUDIO_PROTOCOL_UNDEFINED;             /* bInterfaceProtocol */
		* buff ++ = iInterface;					/* Unused iInterface */
		/* 9 byte*/
	}
	return length;
}

/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
static unsigned UAC1_AS_EP_Desc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wLockDelay = 0;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_ENDPOINT_DESCRIPTOR_TYPE;    // CS_ENDPOINT Descriptor Type (bDescriptorType) 0x25
		* buff ++ = AUDIO_ENDPOINT_GENERAL;            // GENERAL subtype. (bDescriptorSubtype) 0x01
		* buff ++ = 0x00;                              // No sampling frequency control; no pitch control; no packet padding.(bmAttributes)
		* buff ++ = 0x02;                              // bLockDelayUnits
		* buff ++ = LO_BYTE(wLockDelay);			/* wLockDelay */
		* buff ++ = HI_BYTE(wLockDelay);
	}
	return length;
}

/* USB Speaker Audio Streaming Interface Descriptor */
// USBLyzer: Audio Streaming Interface Descriptor
// audio10.pdf: Table 4-19: Class-Specific AS Interface Descriptor
static unsigned UAC1_AS_InterfaceDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bTerminalLink)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wFormatTag = AUDIO_FORMAT_PCM;	/* wFormatTag */
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;      /* bDescriptorType CS_INTERFACE */
		* buff ++ = AUDIO_STREAMING_GENERAL;              /* bDescriptorSubtype AS_GENERAL */
		* buff ++ = bTerminalLink;                        /* bTerminalLink */
		* buff ++ = 0x01;                                 /* bDelay */
		* buff ++ = LO_BYTE(wFormatTag);                  /* wFormatTag - Audio Data Format that */
		* buff ++ = HI_BYTE(wFormatTag);
		/* 07 byte*/
	}
	return length;
}

#if WITHUSBUACIN2

static unsigned fill_UAC1_INRTS_function(
		uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed,
		uint_fast8_t offset)
{
	const uint_fast8_t rtscontrolifv = INTERFACE_AUDIO_CONTROL_RTS;
	const uint_fast8_t rtsifv = INTERFACE_AUDIO_RTS;
	const uac_pathfn_t rtspath = UAC1_TopologyINRTS;
	const uint_fast8_t terminalID = UACTEix(TERMINAL_ID_USBSTREAM, offset);
	const tpgt_t rtsterms =
	{
		.bTerminalID = terminalID,
		.bFeatureUnitID = UACTEix(TERMINAL_ID_FU1a_IN, offset),
		.bFeatureUnit2ID = UACTEix(TERMINAL_ID_FU1b_IN, offset),	// промежуточный terminal этой топологии
	};
	const tpgt_t * const rtstermsv [] = { & rtsterms };
	const uint_fast8_t epinrts = USB_ENDPOINT_IN(USBD_EP_RTS_IN);
	const uint_fast8_t iInterfaceINRTS = STRING_ID_MODE0 + UACOFFS_INRTS;
	uint_fast8_t ialt = 0;
	unsigned n = 0;

	n += UAC1_InterfaceAssociationDesc(fill, p + n, maxsize - n, rtscontrolifv, 2, offset);	/* INTERFACE_AUDIO_CONTROL_SPK Interface Association Descriptor Audio */

	// IN data flow: USB Microphone
	// INTERFACE_AUDIO_MIKE - audio streaming interface
	n += UAC1_AC_InterfaceDesc(fill, p + n, maxsize - n, rtscontrolifv, 0x00, offset);	/* INTERFACE_AUDIO_CONTROL_RTS - Interface Descriptor 0/0 Audio, 0 Endpoints */
	n += UAC1_HeaderDescriptor(fill, p + n, maxsize - n, & rtsifv, & rtspath, rtstermsv, 1, offset);	/* bcdADC Audio Control Interface Header Descriptor */
	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, rtsifv, ialt ++, 0, offset, iInterfaceINRTS);	/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3) */ //zero-bandwidth interface

#if WITHRTS96
	// IN data flow: radio RX spectrum data
	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, rtsifv, ialt ++, 1, offset, iInterfaceINRTS);	/* INTERFACE_AUDIO_RTS_2 Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC1_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalID);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
	n += UAC1_FormatTypeDesc_RTS96(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC1_EP_Desc_RTS96(fill, p + n, maxsize - n, highspeed, epinrts, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += UAC1_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* WITHRTS96 */

#if WITHRTS192
	// IN data flow: radio RX spectrum data
	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, rtsifv, ialt ++, 1, offset, iInterfaceINRTS);	/* INTERFACE_AUDIO_RTS_2 Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC1_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalID);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
	n += UAC2_AS_EP_Desc_RTS192(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC1_EP_Desc_RTS192(fill, p + n, maxsize - n, highspeed, epinrts, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += UAC1_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* WITHRTS192 */

	return n;
}

#endif /* WITHUSBUACIN2 */

#if WITHUSBUACIN
// AUDIO48 only IN (radio to host) audio function
static unsigned fill_UAC1_IN48_function(
		uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed,
		uint_fast8_t offset)
{
	const uint_fast8_t controlifv = INTERFACE_AUDIO_CONTROL_MIKE;	/* AUDIO receiever out control interface */
	const uint_fast8_t mikeifv = INTERFACE_AUDIO_MIKE;
	const uac_pathfn_t mikepath = UAC1_TopologyIN48;
	const uint_fast8_t terminalID = UACTEix(TERMINAL_ID_USBSTREAM, offset);
	const tpgt_t miketerms =
	{
			.bTerminalID = terminalID,
			.bFeatureUnitID = UACTEix(TERMINAL_ID_FU1a_IN, offset),
			.bFeatureUnit2ID = UACTEix(TERMINAL_ID_FU1b_IN, offset),	// промежуточный terminal этой топологии
	};
	const tpgt_t * const miketermsv [] = { & miketerms };
	const uint_fast8_t epin = USB_ENDPOINT_IN(USBD_EP_AUDIO_IN);
	const uint_fast8_t iInterfaceIN48 = STRING_ID_MODE0 + offset;
	uint_fast8_t ialt = 0;
	unsigned n = 0;

	n += UAC1_InterfaceAssociationDesc(fill, p + n, maxsize - n, controlifv, 2, offset);	/* INTERFACE_AUDIO_CONTROL_SPK Interface Association Descriptor Audio */
	// INTERFACE_AUDIO_CONTROL_SPK - audio control interface
	n += UAC1_AC_InterfaceDesc(fill, p + n, maxsize - n, controlifv, 0x00, offset);	/* INTERFACE_AUDIO_CONTROL_SPK - Interface Descriptor 0/0 Audio, 0 Endpoints */
	n += UAC1_HeaderDescriptor(fill, p + n, maxsize - n, & mikeifv, & mikepath, miketermsv, 1, offset);	/* bcdADC Audio Control Interface Header Descriptor */

	// IN data flow: USB Microphone
	// INTERFACE_AUDIO_MIKE - audio streaming interface
	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 0, offset, iInterfaceIN48);	/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3) */ //zero-bandwidth interface

	// IN data flow: radio RX audio data
	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 1, offset, iInterfaceIN48);	/* INTERFACE_AUDIO_MIKE Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC1_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalID);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
	n += UAC1_FormatTypeDesc_IN48(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC1_EP_Desc_IN48(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += UAC1_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - ???????????? ?????????????*/

	return n;
}

/* на одном устройстве различные форматы для передачи в компьютер спектра и звука */
static unsigned fill_UAC1_IN48_INRTS_function(
		uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed,
		uint_fast8_t offset)
{
	const uint_fast8_t controlifv = INTERFACE_AUDIO_CONTROL_MIKE;
	const uint_fast8_t mikeifv = INTERFACE_AUDIO_MIKE;
	const uint_fast8_t epin = USB_ENDPOINT_IN(USBD_EP_AUDIO_IN);
	const uac_pathfn_t mikepath = UAC1_TopologyIN48_INRTS;
	const uint_fast8_t terminalID = UACTEix(TERMINAL_ID_USBSTREAM, offset);
	const tpgt_t miketerms =
	{
			.bTerminalID = terminalID,
			.bFeatureUnitID = UACTEix(TERMINAL_ID_FU1a_IN, offset),
			.bFeatureUnit2ID = UACTEix(TERMINAL_ID_FU1b_IN, offset),	// промежуточный terminal этой топологии
	};
	const tpgt_t * const miketermsv [] = { & miketerms };
	const uint_fast8_t iInterfaceIN48_INRTS = STRING_ID_MODE0 + offset;
	uint_fast8_t ialt = 0;
	unsigned n = 0;

	n += UAC1_InterfaceAssociationDesc(fill, p + n, maxsize - n, controlifv, 2, offset);	/* INTERFACE_AUDIO_CONTROL_SPK Interface Association Descriptor Audio */
	// INTERFACE_AUDIO_CONTROL_SPK - audio control interface
	n += UAC1_AC_InterfaceDesc(fill, p + n, maxsize - n, controlifv, 0x00, offset);	/* INTERFACE_AUDIO_CONTROL_SPK - Interface Descriptor 0/0 Audio, 0 Endpoints */
	n += UAC1_HeaderDescriptor(fill, p + n, maxsize - n, & mikeifv, & mikepath, miketermsv, 1, offset);	/* bcdADC Audio Control Interface Header Descriptor */

	// IN data flow: demodulator
	// INTERFACE_AUDIO_MIKE - audio streaming interface
	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 0, offset, iInterfaceIN48_INRTS);	/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3) */ //zero-bandwidth interface

	// IN data flow: demodulator
	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 1, offset, iInterfaceIN48_INRTS);	/* INTERFACE_AUDIO_MIKE Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC1_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalID);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
	n += UAC1_FormatTypeDesc_IN48(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC1_EP_Desc_IN48(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += UAC1_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/

#if WITHRTS96
	// IN data flow: radio RX spectrum data
	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 1, offset, iInterfaceIN48_INRTS);	/* INTERFACE_AUDIO_MIKE Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC1_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalID);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
	n += UAC1_FormatTypeDesc_RTS96(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC1_EP_Desc_RTS96(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += UAC1_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* WITHRTS96 */

#if WITHRTS192
	// IN data flow: radio RX spectrum data
	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 1, offset, iInterfaceIN48_INRTS);	/* INTERFACE_AUDIO_MIKE Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC1_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalID);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
	n += UAC2_AS_EP_Desc_RTS192(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC1_EP_Desc_RTS192(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += UAC1_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* WITHRTS192 */

	return n;
}
#endif /* WITHUSBUACIN */

#if WITHUSBUACOUT
// AUDIO48 only OUT (host to radio) audio function
static unsigned fill_UAC1_OUT48_function(
		uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed,
		uint_fast8_t offset)
{
	const uint_fast8_t controlifv = INTERFACE_AUDIO_CONTROL_SPK;	/* AUDIO transmitter input control interface */
	const uint_fast8_t modulatorifv = INTERFACE_AUDIO_SPK;
	const uac_pathfn_t modulatorpath = UAC1_TopologyOUT48;
	const uint_fast8_t terminalID = UACTEix(TERMINAL_ID_USBSTREAM, offset);
	const tpgt_t modulatorterms =
	{
			.bTerminalID = terminalID,
			.bFeatureUnitID = UACTEix(TERMINAL_ID_FU1a_OUT, offset),
			.bFeatureUnit2ID = UACTEix(TERMINAL_ID_FU1b_OUT, offset),	// промежуточный terminal этой топологии
	};
	const tpgt_t * const modulatortermsv [] = { & modulatorterms };
	const uint_fast8_t epout = USB_ENDPOINT_OUT(USBD_EP_AUDIO_OUT);
	const uint_fast8_t iInterfaceOUT48 = STRING_ID_MODE0 + offset;
	uint_fast8_t ialt = 0;
	unsigned n = 0;

	n += UAC1_InterfaceAssociationDesc(fill, p + n, maxsize - n, controlifv, 2, offset);	/* INTERFACE_AUDIO_CONTROL_SPK Interface Association Descriptor Audio */
	// INTERFACE_AUDIO_CONTROL_SPK - modulator audio control interface
	n += UAC1_AC_InterfaceDesc(fill, p + n, maxsize - n, controlifv, 0x00, offset);	/* INTERFACE_AUDIO_CONTROL_SPK - Interface Descriptor 0/0 Audio, 0 Endpoints */
	n += UAC1_HeaderDescriptor(fill, p + n, maxsize - n, & modulatorifv, & modulatorpath, modulatortermsv, 1, offset);	/* bcdADC Audio Control Interface Header Descriptor */

	// OUT data flow: modulator
	// INTERFACE_AUDIO_SPK - audio streaming interface
	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, modulatorifv, ialt ++, 0, offset, iInterfaceOUT48);	/* INTERFACE_AUDIO_SPK - Interface 1, Alternate Setting 0 */

	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, modulatorifv, ialt ++, 1, offset, iInterfaceOUT48);	/* INTERFACE_AUDIO_SPK -  Interface 1, Alternate Setting 1 */
	n += UAC1_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalID);	/* USB Speaker Audio Streaming Interface Descriptor (for output TERMINAL_UACOUT48 + offset) */
	n += UAC1_FormatTypeDesc_OUT48(fill, p + n, maxsize - n);	/* USB Speaker Audio Type I Format Interface Descriptor (one sample rate) 48000 */
	n += UAC1_EP_Desc_OUT48(fill, p + n, maxsize - n, epout, highspeed);	/* Endpoint USBD_EP_AUDIO_OUT - Standard Descriptor */
	n += UAC1_AS_EP_Desc(fill, p + n, maxsize - n);	/* Endpoint - Audio Streaming Descriptor */

	return n;
}
#endif /* WITHUSBUACOUT */


typedef struct audiopath_tag
{
	uac_pathfn_t pathfn;
	uint8_t terminalID;
} audiopath_t;

#if WITHUSBUACIN && WITHUSBUACOUT
/* на одном устройстве различные форматы для передачи в компьютер спектра и звука */
static unsigned fill_UAC1_IN48_OUT48_function(
	uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed,
	uint_fast8_t offset)
{
	unsigned n = 0;
	uint_fast8_t ialt = 0;
	uint_fast8_t oalt = 0;

	const uint_fast8_t controlifv = INTERFACE_AUDIO_CONTROL_MIKE;	/* AUDIO receiever out control interface */
	const uint_fast8_t mikeifv = INTERFACE_AUDIO_MIKE;
	const uint_fast8_t modulatorifv = INTERFACE_AUDIO_SPK;
	const uint_fast8_t terminalInID = UACTEix(TERMINAL_ID_USBSTREAM, UACOFFS_IN48);
	const uint_fast8_t terminalOutID = UACTEix(TERMINAL_ID_USBSTREAM, UACOFFS_OUT48);
	const uint_fast8_t iInterfaceIN48 = STRING_ID_MODE0 + UACOFFS_IN48;
	const uint_fast8_t iInterfaceOUT48 = STRING_ID_MODE0 + UACOFFS_OUT48;
	const uint_fast8_t iInterfaceINRTS = STRING_ID_MODE0 + UACOFFS_INRTS;
	const uint_fast8_t coll [] =
	{
		mikeifv,
		modulatorifv,
	};
	const tpgt_t miketerms =
	{
			.bTerminalID = terminalInID,
			.bFeatureUnitID = UACTEix(TERMINAL_ID_FU1a_IN, UACOFFS_IN48),
			.bFeatureUnit2ID = UACTEix(TERMINAL_ID_FU1b_IN, UACOFFS_IN48),	// промежуточный terminal этой топологии
	};
	const tpgt_t modulatorterms =
	{
			.bTerminalID = terminalOutID,
			.bFeatureUnitID = UACTEix(TERMINAL_ID_FU1a_OUT, UACOFFS_OUT48),
			.bFeatureUnit2ID = UACTEix(TERMINAL_ID_FU1b_OUT, UACOFFS_OUT48),	// промежуточный terminal этой топологии
	};
	const tpgt_t * const termsv [] = { & miketerms,  & modulatorterms };
	static const uac_pathfn_t paths [] =
	{
			UAC1_TopologyIN48,
			UAC1_TopologyOUT48,
	};
	const uint_fast8_t epin = USB_ENDPOINT_IN(USBD_EP_AUDIO_IN);
	const uint_fast8_t epout = USB_ENDPOINT_OUT(USBD_EP_AUDIO_OUT);

	ASSERT(controlifv + 1 == mikeifv);
	ASSERT(controlifv + 2 == modulatorifv);

	n += UAC1_InterfaceAssociationDesc(fill, p + n, maxsize - n, controlifv, 3, offset);	/* INTERFACE_AUDIO_CONTROL_SPK Interface Association Descriptor Audio */
	// INTERFACE_AUDIO_CONTROL_SPK - audio control interface
	n += UAC1_AC_InterfaceDesc(fill, p + n, maxsize - n, controlifv, 0x00, offset);	/* INTERFACE_AUDIO_CONTROL_SPK - Interface Descriptor 0/0 Audio, 0 Endpoints */
	n += UAC1_HeaderDescriptor(fill, p + n, maxsize - n, coll, paths, termsv, ARRAY_SIZE(coll), offset);	/* bcdADC Audio Control Interface Header Descriptor */

	// IN data flow: USB Microphone
	// INTERFACE_AUDIO_MIKE - audio streaming interface
	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 0, offset, iInterfaceIN48);	/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3) */ //zero-bandwidth interface

	// IN data flow: radio RX audio data
	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 1, offset, iInterfaceIN48);	/* INTERFACE_AUDIO_MIKE Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC1_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalInID);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
	n += UAC1_FormatTypeDesc_IN48(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC1_EP_Desc_IN48(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += UAC1_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - ???????????? ?????????????*/

#if WITHUSBUACINOUTRENESAS
	/* совмещённое усройство ввода/вывода (и спектр измененем параметров устройства) */

	#if WITHRTS96
		// IN data flow: radio RX spectrum data
		n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 1, offset, iInterfaceINRTS);	/* INTERFACE_AUDIO_MIKE Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
		n += UAC1_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalInID);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
		n += UAC1_FormatTypeDesc_RTS96(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
		n += UAC1_EP_Desc_RTS96(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
		n += UAC1_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
	#endif /* WITHRTS96 */

	#if WITHRTS192
		// IN data flow: radio RX spectrum data
		n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, ialt ++, 1, offset, iInterfaceINRTS);	/* INTERFACE_AUDIO_MIKE Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
		n += UAC1_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalInID);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
		n += UAC2_AS_EP_Desc_RTS192(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
		n += UAC1_EP_Desc_RTS192(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
		n += UAC1_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
	#endif /* WITHRTS192 */

#endif /* WITHUSBUACINOUTRENESAS */

	// OUT data flow: modulator
	// INTERFACE_AUDIO_SPK - audio streaming interface
	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, modulatorifv, oalt ++, 0, offset, iInterfaceOUT48);	/* INTERFACE_AUDIO_SPK - Interface 1, Alternate Setting 0 */

	n += UAC1_InterfaceDesc(fill, p + n, maxsize - n, modulatorifv, oalt ++, 1, offset, iInterfaceOUT48);	/* INTERFACE_AUDIO_SPK -  Interface 1, Alternate Setting 1 */
	n += UAC1_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalOutID);	/* USB Speaker Audio Streaming Interface Descriptor (for output TERMINAL_UACOUT48 + offset) */
	n += UAC1_FormatTypeDesc_OUT48(fill, p + n, maxsize - n);	/* USB Speaker Audio Type I Format Interface Descriptor (one sample rate) 48000 */
	n += UAC1_EP_Desc_OUT48(fill, p + n, maxsize - n, epout, highspeed);	/* Endpoint USBD_EP_AUDIO_OUT - Standard Descriptor */
	n += UAC1_AS_EP_Desc(fill, p + n, maxsize - n);	/* Endpoint - Audio Streaming Descriptor */

	return n;
}

// Объединенное устройство. Необюходимо в случае UAC2 из-за возможности применить shared clock source
static unsigned fill_UAC2_IN48_OUT48_function(
	uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed,
	uint_fast8_t offset)
{
	unsigned n = 0;

	const uint_fast8_t controlifv = INTERFACE_AUDIO_CONTROL_MIKE;	/* AUDIO receiever out control interface */
	const uint_fast8_t mikeifv = INTERFACE_AUDIO_MIKE;
	const uint_fast8_t modulatorifv = INTERFACE_AUDIO_SPK;
	const uint_fast8_t terminalInID = UACTEix(TERMINAL_ID_USBSTREAM, UACOFFS_IN48);
	const uint_fast8_t terminalOutID = UACTEix(TERMINAL_ID_USBSTREAM, UACOFFS_OUT48);
	const uint_fast8_t iInterfaceIN48 = STRING_ID_MODE0 + UACOFFS_IN48;
	const uint_fast8_t iInterfaceOUT48 = STRING_ID_MODE0 + UACOFFS_OUT48;
	const uint_fast8_t iInterfaceINRTS = STRING_ID_MODE0 + UACOFFS_INRTS;
	const uint_fast8_t coll [] =
	{
		mikeifv,
		modulatorifv,
	};
	const tpgt_t miketerms =
	{
			.bTerminalID = terminalInID,
			.bFeatureUnitID = UACTEix(TERMINAL_ID_FU2a_IN, offset),
			.bFeatureUnit2ID = UACTEix(TERMINAL_ID_FU2b_IN, offset),	// промежуточный terminal этой топологии
//		    .bOSC = UACTEix(TERMINAL_ID_CLKSOURCE, offset),
//		    .bCSourceID = UACTEix(TERMINAL_ID_CLKMULTIPLIER, offset),
			.bCSourceID = UACTEix(TERMINAL_ID_CLKSOURCE, offset),
	};
	const tpgt_t modulatorterms =
	{
			.bTerminalID = terminalOutID,
			.bFeatureUnitID = UACTEix(TERMINAL_ID_FU2a_OUT, offset),
			.bFeatureUnit2ID = UACTEix(TERMINAL_ID_FU2b_OUT, offset),	// промежуточный terminal этой топологии
//		    .bOSC = UACTEix(TERMINAL_ID_CLKSOURCE, offset),
//		    .bCSourceID = UACTEix(TERMINAL_ID_CLKMULTIPLIER, offset),
			.bCSourceID = UACTEix(TERMINAL_ID_CLKSOURCE, offset),
	};
	const tpgt_t * const termsv [] = { & miketerms,  & modulatorterms };
	static const uac_pathfn_t paths [] =
	{
            UAC2_Topology_inout_IN48,
            UAC2_Topology_inout_OUT48,
	};
	const uint_fast8_t epin = USB_ENDPOINT_IN(USBD_EP_AUDIO_IN);
	const uint_fast8_t epout = USB_ENDPOINT_OUT(USBD_EP_AUDIO_OUT);

	n += UAC2_InterfaceAssociationDesc(fill, p + n, maxsize - n, controlifv, /*bInterfaceCount */3, offset);	/* INTERFACE_AUDIO_CONTROL_0 Interface Association Descriptor Audio */
	// INTERFACE_AUDIO_CONTROL_0 - audio control interface
	n += UAC2_AC_InterfaceDesc(fill, p + n, maxsize - n, controlifv, 0x00, offset);	/* INTERFACE_AUDIO_CONTROL_0 - Interface Descriptor 0/0 Audio, 0 Endpoints */
	n += UAC2_HeaderDescriptor(fill, p + n, maxsize - n, coll, paths, termsv, ARRAY_SIZE(coll), offset);	/* bcdADC Audio Control Interface Header Descriptor */

	// IN data flow: USB Microphone
	// INTERFACE_AUDIO_MIKE_2 - audio streaming interface
	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, UACINALT_NONE, 0, offset, iInterfaceIN48);	/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3) */ //zero-bandwidth interface

	// IN data flow: radio RX audio data
	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, UACINALT_AUDIO48, 1, offset, iInterfaceIN48);	/* INTERFACE_AUDIO_MIKE_2 Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC2_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalInID, UACIN_CONFIG_IN48);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_ID_OT_4) (CODE == 5) */
	n += UAC2_FormatTypeDescroptor_IN48(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC2_EP_Desc_IN48(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += UAC2_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - ???????????? ?????????????*/

#if WITHUSBUACINOUTRENESAS
	/* совмещённое усройство ввода/вывода (и спектр измененем параметров устройства) */

	#if WITHRTS96
		// IN data flow: radio RX spectrum data
		n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, UACINALT_RTS96, 1, offset, iInterfaceINRTS);	/* INTERFACE_AUDIO_MIKE Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
		n += UAC2_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalInID, UACIN_CONFIG_INRTS);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
		n += UAC2_FormatTypeDesc_RTS96(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
		n += UAC2_EP_Desc_IN_RTS96(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
		n += UAC2_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
	#endif /* WITHRTS96 */

	#if WITHRTS192
		// IN data flow: radio RX spectrum data
		n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, mikeifv, UACINALT_RTS192, 1, offset, iInterfaceINRTS);	/* INTERFACE_AUDIO_MIKE Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
		n += UAC2_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalInID, UACIN_CONFIG_INRTS);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_UACIN48_UACINRTS) (CODE == 5) */
		n += UAC2_FormatTypeDesc_RTS192(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
		n += UAC2_EP_Desc_IN_RTS192(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
		n += UAC2_AS_EP_Desc(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
	#endif /* WITHRTS192 */

#endif /* WITHUSBUACINOUTRENESAS */

	// OUT data flow: TX audo
	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, modulatorifv, UACOUTALT_NONE, 0, offset, iInterfaceOUT48);	/* INTERFACE_AUDIO_SPK - Interface 1, Alternate Setting 0 */

	n += UAC2_InterfaceDesc(fill, p + n, maxsize - n, modulatorifv, UACOUTALT_AUDIO48, 1, offset, iInterfaceOUT48);	/* INTERFACE_AUDIO_SPK - Interface 1, Alternate Setting 0 */
	n += UAC2_AS_InterfaceDesc(fill, p + n, maxsize - n, terminalOutID, UACOUT_CONFIG_OUT48);	/* USB Speaker Audio Streaming Interface Descriptor (for output TERMINAL_ID_IT_1 + offset) */
	n += UAC2_FormatTypeDesc_OUT48(fill, p + n, maxsize - n);	/* USB Speaker Audio Type I Format Interface Descriptor (one sample rate) 48000 */
	n += UAC2_EP_Desc_OUT48(fill, p + n, maxsize - n, epout, highspeed);	/* Endpoint USBD_EP_AUDIO_OUT - Standard Descriptor */
	n += UAC2_AS_EP_Desc_OUT48(fill, p + n, maxsize - n);	/* Endpoint - Audio Streaming Descriptor */

	return n;
}

#endif /* WITHUSBUACIN && WITHUSBUACOUT */

static unsigned fill_UAC1_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed)
{
	unsigned n = 0;

#if WITHUSBUACIN && WITHUSBUACOUT && WITHUSBUACINOUTRENESAS
	/* совмещённое усройство ввода/вывода (и спектр измененем параметров устройства) */
	n += fill_UAC1_IN48_OUT48_function(fill, p + n, maxsize - n, highspeed, UACOFFS_IN48_OUT48);

#elif WITHUSBUACIN && WITHUSBUACOUT && WITHUSBUACINOUT
	/* отдельные функции для передачи в компьютер спектра и двунапаправленная звука */
	n += fill_UAC1_IN48_OUT48_function(fill, p + n, maxsize - n, highspeed, UACOFFS_IN48_OUT48);
	#if WITHRTS96 || WITHRTS192
		n += fill_UAC1_INRTS_function(fill, p + n, maxsize - n, highspeed, UACOFFS_INRTS);
	#endif /* WITHRTS96 || WITHRTS192 */

#else /* WITHUSBUACIN && WITHUSBUACOUT && WITHUSBUACINOUT */
	#if WITHUSBUACIN2
		/* отдельные функции для передачи в компьютер спектра и звука */
		n += fill_UAC1_IN48_function(fill, p + n, maxsize - n, highspeed, UACOFFS_IN48);
		#if WITHRTS96 || WITHRTS192
			n += fill_UAC1_INRTS_function(fill, p + n, maxsize - n, highspeed, UACOFFS_INRTS);
		#else /* WITHRTS96 || WITHRTS192 */
			#error WITHRTS96 or WITHRTS192 required for WITHUSBUACIN2
		#endif /* WITHRTS96 || WITHRTS192 */

	#elif WITHUSBUACIN
		/* на одном устройстве различные форматы для передачи в компьютер спектра и звука */
		n += fill_UAC1_IN48_INRTS_function(fill, p + n, maxsize - n, highspeed, UACOFFS_IN48_INRTS);

	#endif /* WITHUSBUACIN2 */

	#if WITHUSBUACOUT

		n += fill_UAC1_OUT48_function(fill, p + n, maxsize - n, highspeed, UACOFFS_OUT48);
	#endif /* WITHUSBUACOUT */

#endif /* WITHUSBUACIN && WITHUSBUACOUT && WITHUSBUACINOUT */

	return n;
}


static unsigned fill_UAC2_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed)
{
	unsigned n = 0;

#if WITHUSBUACIN && WITHUSBUACOUT && WITHUSBUACINOUTRENESAS
	/* совмещённое усройство ввода/вывода (и спектр измененем параметров устройства) */
	n += fill_UAC2_IN48_OUT48_function(fill, p + n, maxsize - n, highspeed, UACOFFS_IN48_OUT48);

#elif WITHUSBUACIN && WITHUSBUACOUT && WITHUSBUACINOUT
	/* отдельные функции для передачи в компьютер спектра и двунапаправленная звука */
	n += fill_UAC2_IN48_OUT48_function(fill, p + n, maxsize - n, highspeed, UACOFFS_IN48_OUT48);
	#if WITHRTS96 || WITHRTS192
		n += fill_UAC2_INRTS_function(fill, p + n, maxsize - n, highspeed, UACOFFS_INRTS);
	#endif /* WITHRTS96 || WITHRTS192 */

#else /* WITHUSBUACIN && WITHUSBUACOUT && WITHUSBUACINOUT */
	#if WITHUSBUACIN2
		/* отдельные функции для передачи в компьютер спектра и звука */
		n += fill_UAC2_IN48_function(fill, p + n, maxsize - n, highspeed, UACOFFS_IN48);
		#if WITHRTS96 || WITHRTS192
			n += fill_UAC2_INRTS_function(fill, p + n, maxsize - n, highspeed, UACOFFS_INRTS);
		#else /* WITHRTS96 || WITHRTS192 */
			#error WITHRTS96 or WITHRTS192 required for WITHUSBUACIN2
		#endif /* WITHRTS96 || WITHRTS192 */

	#elif WITHUSBUACIN
		/* на одном устройстве различные форматы для передачи в компьютер спектра и звука */
		n += fill_UAC2_IN48_INRTS_function(fill, p + n, maxsize - n, highspeed, UACOFFS_IN48_INRTS);

	#endif /* WITHUSBUACIN2 */

	#if WITHUSBUACOUT

		n += fill_UAC2_OUT48_function(fill, p + n, maxsize - n, highspeed, UACOFFS_OUT48);
	#endif /* WITHUSBUACOUT */

#endif /* WITHUSBUACIN && WITHUSBUACOUT && WITHUSBUACINOUT */

	return n;
}

#endif /* WITHUSBUAC */

#if WITHUSBCDCACM
/* CDC IAD */

// ISBLyzer: Interface Association Descriptor Abstract Control Model
// documented in USB ECN : Interface Association Descriptor - InterfaceAssociationDesc_ecn.pdf
static unsigned CDCACM_InterfaceAssociationDesc_a(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t offset)
{
	const uint_fast8_t length = 8;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 0x02/0x02/0x01 - cdc
		// 0x02/0x0c/0x07 - CDC Ethernet Emulation Model
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_ASSOC_DESCRIPTOR_TYPE;	// bDescriptorType: IAD
		* buff ++ = USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL, offset);	// bFirstInterface
		* buff ++ = INTERFACE_CDCACM_count;	// bInterfaceCount
		* buff ++ = USB_DEVICE_CLASS_COMMUNICATIONS;	// bFunctionClass: CDC
		* buff ++ = CDC_ABSTRACT_CONTROL_MODEL;			// bFunctionSubClass
		* buff ++ = 0x00; //CDC_PROTOCOL_COMMON_AT_COMMANDS;	// bFunctionProtocol
		* buff ++ = STRING_ID_4a + offset;				// iFunction - Serial Port #%d - появляется, если сделать не тот bFunctionSubClass
	}
	return length;
}

/*Interface Descriptor*/
// USBLyzer: Interface Descriptor 3/0 CDC Control, 1 Endpoint 
static unsigned CDCACM_InterfaceDescControlIf_a(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t offset, uint_fast8_t bNumEndpoints)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;  /* bDescriptorType: Interface */  /* Interface descriptor type */
		* buff ++ = USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL, offset);   /* bInterfaceNumber: Number of Interface */
		* buff ++ = 0;		/* bAlternateSetting: Alternate setting  - zero-based index */
		* buff ++ = bNumEndpoints;   /* bNumEndpoints: One endpoints used (interrupt type) */
		* buff ++ = CDC_COMMUNICATION_INTERFACE_CLASS;   /* bInterfaceClass: Communication Interface Class */
		* buff ++ = CDC_ABSTRACT_CONTROL_MODEL;   /* bInterfaceSubClass: Abstract Control Model */
		* buff ++ = 0x00; //CDC_PROTOCOL_COMMON_AT_COMMANDS;   /* bInterfaceProtocol: Common AT commands */
		* buff ++ = STRING_ID_0;   /* iInterface */
	}
	return length;
}

/* Call Managment Functional Descriptor */
// Call Management Functional Descriptor 
static unsigned CDCACM_CallManagementDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t offset)
{
	const uint_fast8_t length = 5;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = CS_INTERFACE;   /* bDescriptorType: CS_INTERFACE */
		* buff ++ = 0x01;   /* bDescriptorSubtype: Call Management Func Desc */
		* buff ++ = 0x00;   /* bmCapabilities: D0+D1 */
		* buff ++ = USBD_CDCACM_IFC(INTERFACE_CDC_DATA, offset);   /* bDataInterface: Zero based index of the interface in this configuration.(bInterfaceNum) */
	}
	return length;
}

/* Union Functional Descriptor */
// Union Functional Descriptor 
static unsigned CDC_UnionFunctionalDesc_a(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t offset)
{
	const uint_fast8_t length = 5;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						/* bFunctionLength */
		* buff ++ = CS_INTERFACE;				/* bDescriptorType: CS_INTERFACE */
		* buff ++ = CDC_UNION;						/* bDescriptorSubtype: Union func desc */
		* buff ++ = USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL, offset);	/* bMasterInterface: Communication class interface -  Zero based index of the interface in this configuration (bInterfaceNum) */
		* buff ++ = USBD_CDCACM_IFC(INTERFACE_CDC_DATA, offset);		/* bSlaveInterface0: Data Class Interface -  Zero based index of the interface in this configuration (bInterfaceNum) */
	}
	return length;
}

/* Data class interface descriptor*/
// USBLyzer: Interface Descriptor 4/0 CDC Data, 2 Endpoints
static unsigned CDCACM_InterfaceDescDataIf_a(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize, 
	uint_fast8_t bAlternateSetting, uint_fast8_t bNumEndpoints, 
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;  /* bDescriptorType: */
		* buff ++ = USBD_CDCACM_IFC(INTERFACE_CDC_DATA, offset);   /* bInterfaceNumber: Number of Interface */
		* buff ++ = bAlternateSetting;		/* bAlternateSetting: Alternate setting  - zero-based index  */
		* buff ++ = bNumEndpoints;   /* bNumEndpoints: Two endpoints used: data in and data out */
		* buff ++ = CDC_DATA_INTERFACE_CLASS;   /* bInterfaceClass: CDC */
		* buff ++ = 0x00;   /* bInterfaceSubClass: */
		* buff ++ = 0x00;   /* bInterfaceProtocol: */
		* buff ++ = STRING_ID_0;   /* iInterface: */
	}
	return length;
}

/* ACM Functional Descriptor */
// Abstract Control Management Functional Descriptor
static unsigned CDCACM_ACMFunctionalDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 4;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		// defined in PSTN120.pdf 5.3.2 Abstract Control Management Functional Descriptor 
		* buff ++ = length;						  /* bLength */
		* buff ++ = CS_INTERFACE;   /* bDescriptorType: CS_INTERFACE */
		* buff ++ = 0x02;   /* bDescriptorSubtype: Abstract Control Management desc */
		* buff ++ = 0x02;   /* bmCapabilities 0x02: Line Coding requests and Serial State notification supported */
	}
	return length;
}

/* Notification Endpoint Descriptor */
static unsigned CDCACM_Control_EP(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(VIRTUAL_COM_PORT_INT_SIZE);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE; 	/* bDescriptorType: Endpoint */
		* buff ++ = bEndpointAddress;			/* bEndpointAddress: (IN) */
		* buff ++ = USB_ENDPOINT_TYPE_INTERRUPT;   	/* bmAttributes: Interrupt */
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = highspeed ? HSINTERVAL_256MS : FSINTERVAL_255MS;   						/* bInterval: 255 mS */
	}
	return length;
}

/* Data Out Endpoint Descriptor*/
static unsigned CDCACM_Data_EP_OUT(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(VIRTUAL_COM_PORT_OUT_DATA_SIZE);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;   /* bDescriptorType: Endpoint */
		* buff ++ = bEndpointAddress;				/* bEndpointAddress: (OUT2) */
		* buff ++ = USB_ENDPOINT_TYPE_BULK;   		/* bmAttributes: Bulk */
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = 0x00;    						/* bInterval: ignore for Bulk transfer */
	}
	return length;
}

/* Data In Endpoint Descriptor*/
static unsigned CDCACM_Data_EP_IN(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(VIRTUAL_COM_PORT_IN_DATA_SIZE);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	/* bDescriptorType: Endpoint */
		* buff ++ = bEndpointAddress;				/* bEndpointAddress: (IN2) */
		* buff ++ = USB_ENDPOINT_TYPE_BULK;   		/* bmAttributes: Bulk */
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = 0x00;    						/* bInterval: ignore for Bulk transfer */
	}
	return length;
}

static unsigned fill_CDCACM_function_a(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed, uint_fast8_t offset)
{
	unsigned n = 0;
	const uint8_t inep = USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, offset);
	const uint8_t outnep = USBD_CDCACM_OUT_EP(USBD_EP_CDCACM_OUT, offset);
	const uint8_t intnep = USBD_CDCACM_INT_EP(USBD_EP_CDCACM_INT, offset);;

	// CDC
	n += CDCACM_InterfaceAssociationDesc_a(fill, p + n, maxsize - n, offset);	/* CDC: Interface Association Descriptor Abstract Control Model */
	n += CDCACM_InterfaceDescControlIf_a(fill, p + n, maxsize - n, offset, 0x01);	/* INTERFACE_CDC_CONTROL Interface Descriptor 3/0 CDC Control, 1 Endpoint */
	n += CDC_HeaderFunctionalDesc(fill, p + n, maxsize - n);	/* Header Functional Descriptor*/
	n += CDCACM_CallManagementDesc(fill, p + n, maxsize - n, offset);	/* Call Managment Functional Descriptor*/
	n += CDCACM_ACMFunctionalDesc(fill, p + n, maxsize - n);	/* ACM Functional Descriptor */
	n += CDC_UnionFunctionalDesc_a(fill, p + n, maxsize - n, offset);	/* Union Functional Descriptor INTERFACE_CDC_CONTROL & INTERFACE_CDC_DATA */
	n += CDCACM_Control_EP(fill, p + n, maxsize - n, highspeed, USB_ENDPOINT_IN(intnep));	/* Endpoint Descriptor 86 6 In, Interrupt */

	n += CDCACM_InterfaceDescDataIf_a(fill, p + n, maxsize - n, 0x00, 2, offset);	/* INTERFACE_CDC_DATA Data class interface descriptor */
	n += CDCACM_Data_EP_OUT(fill, p + n, maxsize - n, USB_ENDPOINT_OUT(outnep));	/* Endpoint Descriptor USBD_EP_CDCACM_OUT Out, Bulk, 64 bytes */
	n += CDCACM_Data_EP_IN(fill, p + n, maxsize - n, USB_ENDPOINT_IN(inep));	/* Endpoint Descriptor USBD_EP_CDCACM_IN In, Bulk, 64 bytes */

	return n;
}

// Group of CDC
static unsigned fill_CDCACM_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed)
{
	unsigned offset;
	unsigned n = 0;

	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
		n += fill_CDCACM_function_a(fill, p + n, maxsize - n, highspeed, offset);
	return n;
}

#endif /* WITHUSBCDCACM */

#if WITHUSBCDCEEM

static unsigned CDCEEM_InterfaceAssociationDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bFirstInterface,
	uint_fast8_t bInterfaceCount
	)
{
	const uint_fast8_t length = 8;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 0x02/0x02/0x01 - cdc
		// 0x02/0x0c/0x07 - CDC Ethernet Emulation Model
		// http://blog.metrotek.spb.ru/2011/07/07/usb-set-na-cortex-m3/

		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_ASSOC_DESCRIPTOR_TYPE;	// bDescriptorType: IAD
		* buff ++ = bFirstInterface;				// bFirstInterface
		* buff ++ = bInterfaceCount;	// bInterfaceCount
		* buff ++ = USB_DEVICE_CLASS_COMMUNICATIONS;	// bFunctionClass: CDC
		* buff ++ = CDC_ETHERNET_EMULATION_MODEL;		// bFunctionSubClass - Ethernet Networking
		* buff ++ = 0x07;						// bFunctionProtocol
		* buff ++ = STRING_ID_5;				// iFunction - CDC Ethernet Control Model (EEM)
	}
	return length;
}

// Информация о типе требуемого драйвера берется отсюда по кодам в bInterfaceClass, bInterfaceSubclass, bInterfaceProtocol
static unsigned CDCEEM_fill_24(uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bInterfaceNumber, 
	uint_fast8_t bAlternateSetting, 
	uint_fast8_t bNumEndpoints
	)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 0x02/0x0c/0x07 - CDC Ethernet Emulation Model
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;    // INTERFACE descriptor type (bDescriptorType) 0x04
		* buff ++ = bInterfaceNumber; // Index of this interface. (bInterfaceNumber) ?????????? (3<) (1<<) (1<M)
		* buff ++ = bAlternateSetting;				// 0 Index of this alternate setting. (bAlternateSetting) - zero-based index 
		* buff ++ = bNumEndpoints;					// bNumEndpoints
		* buff ++ = 0x02;							// bInterfaceClass
		* buff ++ = CDC_ETHERNET_EMULATION_MODEL;	/* bInterfaceSubclass */
		* buff ++ = 0x07;							/* bInterfaceProtocol */
		* buff ++ = STRING_ID_0;					/* Unused iInterface */
		/* 9 byte*/
	}
	return length;
}

// Endpoint Descriptor
static unsigned CDCEEM_fill_37(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(USBD_CDCEEM_BUFSIZE);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;   /* bDescriptorType: Endpoint */
		* buff ++ = bEndpointAddress;				/* bEndpointAddress: (OUT2) */
		* buff ++ = USB_ENDPOINT_TYPE_BULK;   		/* bmAttributes: Bulk */
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = 0x01;    						/* bInterval: ignore for Bulk transfer */
	}
	return length;
}

/*Endpoint 2 IN Descriptor*/
// Endpoint Descriptor 84 4 In, Bulk, 64 bytes
static unsigned CDCEEM_fill_38(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(USBD_CDCEEM_BUFSIZE);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	/* bDescriptorType: Endpoint */
		* buff ++ = bEndpointAddress;				/* bEndpointAddress: (IN2) */
		* buff ++ = USB_ENDPOINT_TYPE_BULK;   		/* bmAttributes: Bulk */
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = 0x01;    						/* bInterval: ignore for full speed Bulk endpoints */
	}
	return length;
}

// https://electronix.ru/forum/applications/core/interface/file/attachment.php?id=118456
// https://github.com/rawaaw/cdc-eem-f103
/* CDC Ethernet Emulation Model */
static unsigned fill_CDCEEM_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed)
{
	unsigned n = 0;

	// iadclasscode_r10.pdf
	// InterfaceAssociationDesc требуется только для многоинтерфейсных
	// Провда, там написано что iadclasscode_r10.pdf
	n += CDCEEM_InterfaceAssociationDesc(fill, p + n, maxsize - n, INTERFACE_CDCEEM_DATA, INTERFACE_CDCEEM_count);	/* CDC EEM: Interface Association Descriptor Abstract Control Model */
	n += CDCEEM_fill_24(fill, p + n, maxsize - n, INTERFACE_CDCEEM_DATA, 0x00, 2);	/* INTERFACE_CDCEEM_DATA Data class interface descriptor */
	n += CDCEEM_fill_38(fill, p + n, maxsize - n, USB_ENDPOINT_IN(USBD_EP_CDCEEM_IN));	/* Endpoint Descriptor USBD_EP_CDCECM_IN In, Bulk, 64 bytes */
	n += CDCEEM_fill_37(fill, p + n, maxsize - n, USB_ENDPOINT_OUT(USBD_EP_CDCEEM_OUT));	/* Endpoint Descriptor USBD_EP_CDCECM_OUT Out, Bulk, 64 bytes */

	return n;
}

#endif /* WITHUSBCDCEEM */

#if WITHUSBCDCECM

// ISBLyzer: Interface Association Descriptor Abstract Control Model
// documented in USB ECN : Interface Association Descriptor - InterfaceAssociationDesc_ecn.pdf
static unsigned CDCECM_InterfaceAssociationDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bFirstInterface,
	uint_fast8_t bInterfaceCount
	)
{
	const uint_fast8_t length = 8;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 0x02/0x02/0x01 - cdc
		// 0x02/0x0c/0x07 - CDC Ethernet Emulation Model
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_ASSOC_DESCRIPTOR_TYPE;	// bDescriptorType: IAD
		* buff ++ = bFirstInterface;				// bFirstInterface
		* buff ++ = bInterfaceCount;	// bInterfaceCount
		* buff ++ = USB_DEVICE_CLASS_COMMUNICATIONS;	// bFunctionClass: CDC
		* buff ++ = CDC_ETHERNET_NETWORKING_CONTROL_MODEL;						// bFunctionSubClass - Ethernet Networking
		* buff ++ = 0x00;						// bFunctionProtocol
		* buff ++ = STRING_ID_5a;				// iFunction - CDC Ethernet Control Model (ECM)
	}
	return length;
}


/*Interface Descriptor*/
// USBLyzer: Interface Descriptor 3/0 CDC Control, 1 Endpoint 
static unsigned CDCECM_InterfaceDescControlIf(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;  /* bDescriptorType: Interface */  /* Interface descriptor type */
		* buff ++ = INTERFACE_CDCECM_CONTROL;   /* bInterfaceNumber: Number of Interface */
		* buff ++ = 0;		/* bAlternateSetting: Alternate setting  - zero-based index */
		* buff ++ = 0x01;   /* bNumEndpoints: One endpoints used (interrupt type) */
		* buff ++ = CDC_COMMUNICATION_INTERFACE_CLASS;   /* bInterfaceClass: Communication Interface Class */
		* buff ++ = CDC_ETHERNET_NETWORKING_CONTROL_MODEL;   /* bInterfaceSubClass: Ethernet Networking */
		* buff ++ = 0x00;   /* bInterfaceProtocol */
		* buff ++ = STRING_ID_0;   /* iInterface */
	}
	return length;
}


/* Union Functional Descriptor */
// Union Functional Descriptor 
static unsigned CDCECM_UnionFunctionalDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 5;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						/* bFunctionLength */
		* buff ++ = CS_INTERFACE;				/* bDescriptorType: CS_INTERFACE */
		* buff ++ = CDC_UNION;						/* bDescriptorSubtype: Union func desc */
		* buff ++ = INTERFACE_CDCECM_CONTROL;	/* bMasterInterface: Communication class interface -  Zero based index of the interface in this configuration (bInterfaceNum) */
		* buff ++ = INTERFACE_CDCECM_DATA;	/* bSlaveInterface0: Data Class Interface -  Zero based index of the interface in this configuration (bInterfaceNum) */
	}
	return length;
}

// Ethernet Networking Functional Descriptor
static unsigned CDCECM_EthernetNetworkingFunctionalDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 13;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast32_t bmEthernetStatistics = 0;
		const uint_fast16_t wMaxSegmentSize = 1514;
		const uint_fast16_t wNumberMCFilters = 0;
		const uint_fast8_t bNumberPowerFilters = 0;

		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						/* bFunctionLength */
		* buff ++ = CS_INTERFACE;				/* bDescriptorType: CS_INTERFACE */
		* buff ++ = 0x0F;						/* bDescriptorSubtype: Ethernet Networking Functional Descriptor */
		* buff ++ = STRING_ID_MACADDRESS;		/* iMacAddress */
		* buff ++ = LO_BYTE(bmEthernetStatistics);	/* bmEthernetStatistics */
		* buff ++ = HI_BYTE(bmEthernetStatistics);
		* buff ++ = HI_24BY(bmEthernetStatistics);
		* buff ++ = HI_32BY(bmEthernetStatistics);
		* buff ++ = LO_BYTE(wMaxSegmentSize);	/* wMaxSegmentSize */
		* buff ++ = HI_BYTE(wMaxSegmentSize);
		* buff ++ = LO_BYTE(wNumberMCFilters);	/* wNumberMCFilters */
		* buff ++ = HI_BYTE(wNumberMCFilters);
		* buff ++ = bNumberPowerFilters;	/* bNumberPowerFilters */
	}
	return length;
}


/* Endpoint 3 Descriptor */
// Endpoint Descriptor 86 6 In, Interrupt

static unsigned CDCECM_fill_35(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(USBD_CDCECM_INT_SIZE);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE; 	/* bDescriptorType: Endpoint */
		* buff ++ = bEndpointAddress;			/* bEndpointAddress: (IN) */
		* buff ++ = USB_ENDPOINT_TYPE_INTERRUPT;   	/* bmAttributes: Interrupt */
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = highspeed ? HSINTERVAL_32MS : FSINTERVAL_32MS;   		/* bInterval: 32 mS */
	}
	return length;
}

// Endpoint Descriptor
static unsigned CDCECM_fill_37(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(USBD_CDCECM_OUT_BUFSIZE);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;   /* bDescriptorType: Endpoint */
		* buff ++ = bEndpointAddress;				/* bEndpointAddress: (OUT2) */
		* buff ++ = USB_ENDPOINT_TYPE_BULK;   		/* bmAttributes: Bulk */
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = 0x00;    						/* bInterval: ignore for Bulk transfer */
	}
	return length;
}

/*Endpoint 2 IN Descriptor*/
// Endpoint Descriptor 84 4 In, Bulk, 64 bytes
static unsigned CDCECM_fill_38(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(USBD_CDCECM_IN_BUFSIZE);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	/* bDescriptorType: Endpoint */
		* buff ++ = bEndpointAddress;				/* bEndpointAddress: (IN2) */
		* buff ++ = USB_ENDPOINT_TYPE_BULK;   		/* bmAttributes: Bulk */
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = 0x00;    						/* bInterval: ignore for Bulk transfer */
	}
	return length;
}

static unsigned CDCECM_InterfaceDescDataIf(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize, 
	uint_fast8_t bInterfaceNumber, 
	uint_fast8_t bAlternateSetting, 
	uint_fast8_t bNumEndpoints
	)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;    // INTERFACE descriptor type (bDescriptorType) 0x04
		* buff ++ = bInterfaceNumber; // Index of this interface. (bInterfaceNumber) ?????????? (3<) (1<<) (1<M)
		* buff ++ = bAlternateSetting;				// 0 Index of this alternate setting. (bAlternateSetting) - zero-based index 
		* buff ++ = bNumEndpoints;							// 2 endpoints.   (bNumEndpoints)
		* buff ++ = CDC_DATA_INTERFACE_CLASS;		// 10 CDC Data (bInterfaceClass)
		* buff ++ = 0x00;							// bInterfaceSubclass)
		* buff ++ = 0x00;             /* 0 bInterfaceProtocol */
		* buff ++ = STRING_ID_0;                   /* 0 Unused iInterface */
		/* 9 byte*/
	}
	return length;
}

/* CDC Ethernet Control Model */
static unsigned fill_CDCECM_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed)
{
	uint_fast8_t ialt = 0;
	unsigned n = 0;

	n += CDCECM_InterfaceAssociationDesc(fill, p + n, maxsize - n, INTERFACE_CDCECM_CONTROL, INTERFACE_CDCECM_count);	/* CDC: Interface Association Descriptor Abstract Control Model */
	n += CDCECM_InterfaceDescControlIf(fill, p + n, maxsize - n);	/* INTERFACE_CDC_CONTROL Interface Descriptor 3/0 CDC Control, 1 Endpoint */
	n += CDC_HeaderFunctionalDesc(fill, p + n, maxsize - n);	/* Header Functional Descriptor*/
	n += CDCECM_UnionFunctionalDesc(fill, p + n, maxsize - n);	/* Union Functional Descriptor INTERFACE_CDC_CONTROL & INTERFACE_CDC_DATA */
	n += CDCECM_EthernetNetworkingFunctionalDesc(fill, p + n, maxsize - n);	/* Union Functional Descriptor INTERFACE_CDC_CONTROL & INTERFACE_CDC_DATA */
	n += CDCECM_fill_35(fill, p + n, maxsize - n, highspeed, USB_ENDPOINT_IN(USBD_EP_CDCECM_INT));	/* Endpoint Descriptor 86 6 In, Interrupt */

	n += CDCECM_InterfaceDescDataIf(fill, p + n, maxsize - n, INTERFACE_CDCECM_DATA, ialt ++, 0);	/* INTERFACE_CDCECM_DATA Data class interface descriptor */

	n += CDCECM_InterfaceDescDataIf(fill, p + n, maxsize - n, INTERFACE_CDCECM_DATA, ialt ++, 2);	/* INTERFACE_CDCECM_DATA Data class interface descriptor */
	n += CDCECM_fill_37(fill, p + n, maxsize - n, USB_ENDPOINT_OUT(USBD_EP_CDCECM_OUT));	/* Endpoint Descriptor USBD_EP_CDCECM_OUT Out, Bulk, 64 bytes */
	n += CDCECM_fill_38(fill, p + n, maxsize - n, USB_ENDPOINT_IN(USBD_EP_CDCECM_IN));	/* Endpoint Descriptor USBD_EP_CDCECM_IN In, Bulk, 64 bytes */

	return n;
}

#endif /* WITHUSBCDCECM */

#if WITHUSBRNDIS


// Interface Association Descriptor RF Controller
// documented in USB ECN : Interface Association Descriptor - InterfaceAssociationDesc_ecn.pdf
static unsigned RNDIS_InterfaceAssociationDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 8;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_ASSOC_DESCRIPTOR_TYPE;	// bDescriptorType: IAD
		* buff ++ = INTERFACE_RNDIS_CONTROL;				// bFirstInterface
		* buff ++ = INTERFACE_RNDIS_count;	// bInterfaceCount
		* buff ++ = USB_DEVICE_CLASS_WIRELESS_CONTROLLER;	// bFunctionClass: CDC
		* buff ++ = 0x01;						// bFunctionSubClass - RF Controller
		* buff ++ = 0x03;						// bFunctionProtocol - Remote NDIS
		* buff ++ = STRING_ID_RNDIS;			// iFunction - Remote NDIS
	}
	return length;
}


/*Interface Descriptor*/
// Interface Descriptor 0/0 Wireless Controller, 1 Endpoint
//  Communication Class INTERFACE descriptor          
// https://msdn.microsoft.com/en-US/library/ee485851(v=winembedded.60).aspx
static unsigned RNDIS_InterfaceDescControlIf(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;  /* bDescriptorType: Interface */  /* Interface descriptor type */
		* buff ++ = INTERFACE_RNDIS_CONTROL;   /* bInterfaceNumber: Number of Interface */
		* buff ++ = 0;		/* bAlternateSetting: Alternate setting  - zero-based index */
		* buff ++ = 0x01;   /* bNumEndpoints: One endpoints used (interrupt type) */
		* buff ++ = USB_DEVICE_CLASS_WIRELESS_CONTROLLER;	/* bInterfaceClass: Wireless Controller */
		* buff ++ = 0x01;						// bFunctionSubClass - RF Controller
		* buff ++ = 0x03;   /* bInterfaceProtocol - Remote NDIS */
		* buff ++ = STRING_ID_0;   /* iInterface */
	}
	return length;
}

/* Call Managment Functional Descriptor */
// Call Management Functional Descriptor 
static unsigned RNDIS_fill_32(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 5;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = CS_INTERFACE;   /* bDescriptorType: CS_INTERFACE */
		* buff ++ = 0x01;   /* bDescriptorSubtype: Call Management Func Desc */
		* buff ++ = 0x00;   /* bmCapabilities: D0+D1 */
		* buff ++ = INTERFACE_RNDIS_DATA;   /* bDataInterface: Zero based index of the interface in this configuration.(bInterfaceNum) */
	}
	return length;
}

// Abstract Control Management Functional Descriptor
static unsigned RNDIS_ACMFunctionalDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 4;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		// defined in PSTN120.pdf 5.3.2 Abstract Control Management Functional Descriptor 
		* buff ++ = length;						  /* bLength */
		* buff ++ = CS_INTERFACE;   /* bDescriptorType: CS_INTERFACE */
		* buff ++ = 0x02;   /* bDescriptorSubtype: Abstract Control Management desc */
		* buff ++ = 0x00;   /* bmCapabilities 0x00: Requests/notifications not supported */
	}
	return length;
}


/* Union Functional Descriptor */
// Union Functional Descriptor 
static unsigned RNDIS_UnionFunctionalDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 5;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						/* bFunctionLength */
		* buff ++ = CS_INTERFACE;				/* bDescriptorType: CS_INTERFACE */
		* buff ++ = CDC_UNION;						/* bDescriptorSubtype: Union func desc */
		* buff ++ = INTERFACE_RNDIS_CONTROL;	/* bMasterInterface: Communication class interface -  Zero based index of the interface in this configuration (bInterfaceNum) */
		* buff ++ = INTERFACE_RNDIS_DATA;	/* bSlaveInterface0: Data Class Interface -  Zero based index of the interface in this configuration (bInterfaceNum) */
	}
	return length;
}

/* Endpoint 3 Descriptor */
// Endpoint Descriptor 86 6 In, Interrupt

static unsigned RNDIS_fill_35(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(USBD_RNDIS_INT_SIZE);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE; 	/* bDescriptorType: Endpoint */
		* buff ++ = bEndpointAddress;			/* bEndpointAddress: (IN) */
		* buff ++ = USB_ENDPOINT_TYPE_INTERRUPT;   	/* bmAttributes: Interrupt */
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = highspeed ? HSINTERVAL_1MS : FSINTERVAL_1MS;   		/* bInterval: 1 mS */
	}
	return length;
}

/* Data class interface descriptor*/
// USBLyzer: Interface Descriptor 1/0 CDC Data, 2 Endpoints
static unsigned RNDIS_InterfaceDescDataIf(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;  /* bDescriptorType: */
		* buff ++ = INTERFACE_RNDIS_DATA;			 /* bInterfaceNumber: Number of Interface */
		* buff ++ = 0;		/* bAlternateSetting: Alternate setting  - zero-based index  */
		* buff ++ = 0x02;   /* bNumEndpoints: Two endpoints used: data in and data out */
		* buff ++ = CDC_DATA_INTERFACE_CLASS;   /* bInterfaceClass: CDC */
		* buff ++ = 0x00;   /* bInterfaceSubClass: */
		* buff ++ = 0x00;   /* bInterfaceProtocol: */
		* buff ++ = STRING_ID_0;   /* iInterface: */
	}
	return length;
}

/*Endpoint 2 IN Descriptor*/
// Endpoint Descriptor 84 4 In, Bulk, 64 bytes
static unsigned RNDIS_fill_38(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(USBD_RNDIS_IN_BUFSIZE);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	/* bDescriptorType: Endpoint */
		* buff ++ = bEndpointAddress;				/* bEndpointAddress: (IN2) */
		* buff ++ = USB_ENDPOINT_TYPE_BULK;   		/* bmAttributes: Bulk */
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = 0x00;    						/* bInterval: ignore for Bulk transfer */
	}
	return length;
}

// Endpoint Descriptor
static unsigned RNDIS_fill_37(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(USBD_RNDIS_OUT_BUFSIZE);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;   /* bDescriptorType: Endpoint */
		* buff ++ = bEndpointAddress;				/* bEndpointAddress: (OUT2) */
		* buff ++ = USB_ENDPOINT_TYPE_BULK;   		/* bmAttributes: Bulk */
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = 0x00;    						/* bInterval: ignore for Bulk transfer */
	}
	return length;
}


/* CDC Ethernet Control Model */
static unsigned fill_RNDIS_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed)
{
	unsigned n = 0;
	// Interface Association Descriptor RF Controller
	// Configuration descriptor           https://msdn.microsoft.com/en-US/library/ee482887(v=winembedded.60).aspx
	n += RNDIS_InterfaceAssociationDesc(fill, p + n, maxsize - n);	/* CDC: Interface Association Descriptor Abstract Control Model */
	// Interface Descriptor 0/0 Wireless Controller, 1 Endpoint
	//  Communication Class INTERFACE descriptor          https://msdn.microsoft.com/en-US/library/ee485851(v=winembedded.60).aspx
	n += RNDIS_InterfaceDescControlIf(fill, p + n, maxsize - n);	/* INTERFACE_CDC_CONTROL Interface Descriptor 3/0 CDC Control, 1 Endpoint */
	//  Functional Descriptors for Communication Class Interface per RNDIS spec.
	// Header Functional Descriptor
	////n += CDC_HeaderFunctionalDesc(fill, p + n, maxsize - n);
	// Call Management Functional Descriptor
	n += RNDIS_fill_32(fill, p + n, maxsize - n);
	// Abstract Control Management Functional Descriptor
	n += RNDIS_ACMFunctionalDesc(fill, p + n, maxsize - n);
	// Union Functional Descriptor
	n += RNDIS_UnionFunctionalDesc(fill, p + n, maxsize - n);
	// Endpoint descriptors for Communication Class Interface     https://msdn.microsoft.com/en-US/library/ee482509(v=winembedded.60).aspx
	n += RNDIS_fill_35(fill, p + n, maxsize - n, highspeed, USB_ENDPOINT_IN(USBD_EP_RNDIS_INT));	/* Endpoint Descriptor 86 6 In, Interrupt */
	//  Data Class INTERFACE descriptor           https://msdn.microsoft.com/en-US/library/ee481260(v=winembedded.60).aspx
	n += RNDIS_InterfaceDescDataIf(fill, p + n, maxsize - n);	/* INTERFACE_CDC_DATA Data class interface descriptor */
	// IN Endpoint descriptor     https://msdn.microsoft.com/en-US/library/ee484483(v=winembedded.60).aspx
	n += RNDIS_fill_38(fill, p + n, maxsize - n, USB_ENDPOINT_IN(USBD_EP_RNDIS_IN));	/* Endpoint Descriptor USBD_EP_CDCECM_IN In, Bulk, 64 bytes */
	// OUT Endpoint descriptor     https://msdn.microsoft.com/en-US/library/ee482464(v=winembedded.60).aspx
	n += RNDIS_fill_37(fill, p + n, maxsize - n, USB_ENDPOINT_OUT(USBD_EP_RNDIS_OUT));	/* Endpoint Descriptor USBD_EP_CDCECM_OUT Out, Bulk, 64 bytes */

	return n;
}

#endif /* WITHUSBRNDIS */

#if WITHUSBHID

static unsigned HID_InterfaceDescXXXX(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;								/* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;		/* bDescriptorType: */
		* buff ++ = INTERFACE_HID_CONTROL;			// bInterfaceNumber
		* buff ++ = 0x00;								/* bAlternateSetting */
#if defined (HIDKEYBOARD_INT_DATA_SIZE)
		* buff ++ = 0x01;								/* bNumEndpoints */
		* buff ++ = USB_DEVICE_CLASS_HUMAN_INTERFACE;   /* bInterfaceClass */
		* buff ++ = 0x01;								/* bInterfaceSubClass = boot interfsce */
		* buff ++ = 0x01;							    /* bInterfaceProtocol: 1 = keyboard, 2 = mouse */
#elif defined (HIDMOUSE_INT_DATA_SIZE)
		* buff ++ = 0x01;								/* bNumEndpoints */
		* buff ++ = USB_DEVICE_CLASS_HUMAN_INTERFACE;   /* bInterfaceClass */
		* buff ++ = 0;/0x01;								/* bInterfaceSubClass = boot interfsce */
		* buff ++ = 0x02;							    /* bInterfaceProtocol: 1 = keyboard, 2 = mouse */
#else
		* buff ++ = 0;//0x01;								/* bNumEndpoints */
		* buff ++ = USB_DEVICE_CLASS_HUMAN_INTERFACE;   /* bInterfaceClass */
		* buff ++ = 0;//0x01;								/* bInterfaceSubClass = boot interfsce */
		* buff ++ = 0; //0x01;							    /* bInterfaceProtocol: 1 = keyboard, 2 = mouse */
#endif
		* buff ++ = 0;//STRING_ID_HIDa;						/* iInterface */
	}
	return length;
}

static unsigned HID_Descriptor(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, unsigned wDescriptorLength)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t bcdHID = 0x0111;	// Revision of class specification - 1.11
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;					/* bLength */
		* buff ++ = HID_DESCRIPTOR_TYPE;	/* bDescriptorType = HID */
		* buff ++ = LO_BYTE(bcdHID);		/* bcdHID */
		* buff ++ = HI_BYTE(bcdHID);
		* buff ++ = 0x00;					/* bCountryCode */
		* buff ++ = 1;						/* bNumDescriptors=1 */
		* buff ++ = HID_REPORT_DESC;		/* bDescriptorType: 0x22: report */
		* buff ++ = LO_BYTE(wDescriptorLength);			/* wTotalLength */
		* buff ++ = HI_BYTE(wDescriptorLength);
		/* 9 bytes*/
	}
	return length;

}

#if defined (HIDMOUSE_INT_DATA_SIZE)
// Endpoint Descriptor In, Interrupt
static unsigned fill_HID_Mouse_IntEP(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(HIDMOUSE_INT_DATA_SIZE);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE; 	/* bDescriptorType: Endpoint */
		* buff ++ = bEndpointAddress;			/* bEndpointAddress: (IN) */
		* buff ++ = USB_ENDPOINT_TYPE_INTERRUPT;   	/* bmAttributes: Interrupt */
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = highspeed ? HSINTERVAL_8MS : FSINTERVAL_8MS;	/* bInterval: 8 mS */
	}
	return length;
}
#endif /* defined (HIDMOUSE_INT_DATA_SIZE) */

#if defined (HIDKEYBOARD_INT_DATA_SIZE)
// Endpoint Descriptor In, Interrupt
static unsigned fill_HID_Keyboard_IntEP(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(HIDKEYBOARD_INT_DATA_SIZE);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE; 	/* bDescriptorType: Endpoint */
		* buff ++ = bEndpointAddress;			/* bEndpointAddress: (IN) */
		* buff ++ = USB_ENDPOINT_TYPE_INTERRUPT;   	/* bmAttributes: Interrupt */
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize);
		* buff ++ = highspeed ? HSINTERVAL_8MS : FSINTERVAL_8MS;	/* bInterval: 8 mS */
	}
	return length;
}
#endif /* defined (HIDKEYBOARD_INT_DATA_SIZE) */

// HID Report Descriptor
// mouse
static const uint8_t HID_report_desc_mouse []=
{
	0x05, 0x01, //	Usage Page (Generic Desktop)
	0x09, 0x02, //	Usage (Mouse)
	0xA1, 0x01, //	Collection (Application)
	0x09, 0x01, //     Usage (Pointer)
	0xA1, 0x00, //     Collection (Physical)
	0x05, 0x09, //         Usage Page (Button)
	0x19, 0x01, //         Usage Minimum (Button 1)
	0x29, 0x03, //         Usage Maximum (Button 3)
	0x15, 0x00, //         Logical Minimum (0)
	0x25, 0x01, //         Logical Maximum (1)
	0x95, 0x08, //         Report Count (8)
	0x75, 0x01, //         Report Size (1)
	0x81, 0x02, //         Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)
	0x05, 0x01, //         Usage Page (Generic Desktop)
	0x09, 0x30, //         Usage (X)
	0x09, 0x31, //         Usage (Y)
	0x09, 0x38, //         Usage (Wheel)
	0x15, 0x81, //         Logical Minimum (-127)
	0x25, 0x7F, //         Logical Maximum (127)
	0x75, 0x08, //         Report Size (8)
	0x95, 0x03, //         Report Count (3)
	0x81, 0x06, //         Input (Data,Var,Rel,NWrp,Lin,Pref,NNul,Bit)
	0xC0,		//		End Collection
	0xC0,		//	End Collection
};

// HID Report Descriptor
// keyboard
static const uint8_t HID_report_desc_keyboard []=
{
    0x05, 0x01, // Usage Page (Generic Desktop)
    0x09, 0x06, // Usage (Keyboard)
    0xA1, 0x01, // Collection (Application)
    0x05, 0x07, // Usage Page (Key Codes)
    0x19, 0xE0, // Usage Minimum (224)
    0x29, 0xE7, // Usage Maximum (231)
    0x15, 0x00, // Logical Minimum (0)
    0x25, 0x01, // Logical Maximum (1)
    0x75, 0x01, // Report Size (1)
    0x95, 0x08, // Report Count (8)
    0x81, 0x02, // Input (Data, Variable, Absolute) -- Modifier byte
    0x95, 0x01, // Report Count (1)
    0x75, 0x08, // Report Size (8)
    0x81, 0x03, // (81 01) Input (Constant) -- Reserved byte
    0x95, 0x05, // Report Count (5)
    0x75, 0x01, // Report Size (1)
    0x05, 0x08, // Usage Page (Page# for LEDs)
    0x19, 0x01, // Usage Minimum (1)
    0x29, 0x05, // Usage Maximum (5)
    0x91, 0x02, // Output (Data, Variable, Absolute) -- LED report
    0x95, 0x01, // Report Count (1)
    0x75, 0x03, // Report Size (3)
    0x91, 0x03, // (91 03) Output (Constant) -- LED report padding
    0x95, 0x06, // Report Count (6)
    0x75, 0x08, // Report Size (8)
    0x15, 0x00, // Logical Minimum (0)
    0x25, 0x66, // Logical Maximum(102)  // was 0x65
    0x05, 0x07, // Usage Page (Key Codes)
    0x19, 0x00, // Usage Minimum (0)
    0x29, 0x66, // Usage Maximum (102) // was 0x65
    0x81, 0x00, // Input (Data, Array) -- Key arrays (6 bytes)
    0xC0,       // End Collection
};
// HID Report Descriptor
// keyboard 4 arrows ("joystick")
static const uint8_t HID_report_desc_keyboard_4arrows []=
{
	0x05, 0x01, // Usage Page (Generic Desktop)
	0x09, 0x06, // Usage (Keyboard)
	0xA1, 0x01, // Collection (Application)
	0x05, 0x07, // Usage Page (Key Codes)
	0x19, 0xE0, // Usage Minimum (224)
	0x29, 0xE7, // Usage Maximum (231)
	0x15, 0x00, // Logical Minimum (0)
	0x25, 0x01, // Logical Maximum (1)
	0x75, 0x01, // Report Size (1)
	0x95, 0x08, // Report Count (8)
	0x81, 0x02, // Input (Data, Variable, Absolute) -- Modifier byte
	0x05, 0x0C, // Report Count (1)
	0x95, 0x01, // Report Size (8)
	0x75, 0x08,
	0x15, 0x00,
	0x26, 0xFF, 0x00,
	0x19, 0x00,
	0x29, 0xFF,
	0x81, 0x00,
	0x05, 0x08,
	0x19, 0x01,
	0x29, 0x08,
	0x95, 0x08,
	0x75, 0x01,
	0x91, 0x02,
	0x05, 0x07,
	0x95, 0x06,
	0x75, 0x08,
	0x15, 0x00,
	0x26, 0xE7, 0x00,
	0x19, 0x00,	// Usage Minimum (0)
	0x29, 0xE7,	// Usage Maximum (102) // was 0x65
	0x81, 0x00,	// Input (Data, Array) -- Key arrays (6 bytes)
	0xC0,		// End Collection
};

// HID Report Descriptor
// keyboard
static const uint8_t HID_report_desc_display []=
{
    0x05, 0x14,                    // USAGE_PAGE (Alphnumeric Display)
    0x09, 0x01,                    // USAGE (Alphanumeric Display)
    0x15, 0x00,                    // LOGICAL_MINIMUM (0)
    0xa1, 0x02,                    // COLLECTION (Logical)
    0x09, 0x20,                    //   USAGE (Display Attributes Report)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x09, 0x35,                    //     USAGE (Rows)
    0x09, 0x36,                    //     USAGE (Columns)
    0x09, 0x3d,                    //     USAGE (Character Width)
    0x09, 0x3e,                    //     USAGE (Character Height)
    0x85, 0x01,                    //     REPORT_ID (1)
    0x25, 0x1f,                    //     LOGICAL_MAXIMUM (31)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x95, 0x04,                    //     REPORT_COUNT (4)
    0xb1, 0x03,                    //     FEATURE (Cnst,Var,Abs)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x09, 0x21,                    //     USAGE (ASCII Character Set)
    0x09, 0x22,                    //     USAGE (Data Read Back)
    0x09, 0x29,                    //     USAGE (Vertical Scroll)
    0xb1, 0x03,                    //     FEATURE (Cnst,Var,Abs)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0xb1, 0x03,                    //     FEATURE (Cnst,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x25, 0x02,                    //   LOGICAL_MAXIMUM (2)
    0x09, 0x2d,                    //   USAGE (Display Status)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x09, 0x2e,                    //     USAGE (Stat Not Ready)
    0x09, 0x2f,                    //     USAGE (Stat Ready)
    0x09, 0x30,                    //     USAGE (Err Not a loadable character)
    0x81, 0x40,                    //     INPUT (Data,Ary,Abs,Null)
    0xc0,                          //   END_COLLECTION
    0x09, 0x32,                    //   USAGE (Cursor Position Report)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x85, 0x02,                    //     REPORT_ID (2)
    0x75, 0x04,                    //     REPORT_SIZE (4)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x25, 0x0f,                    //     LOGICAL_MAXIMUM (15)
    0x09, 0x34,                    //     USAGE (Column)
    0xb1, 0x22,                    //     FEATURE (Data,Var,Abs,NPrf)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x09, 0x33,                    //     USAGE (Row)
    0xb1, 0x22,                    //     FEATURE (Data,Var,Abs,NPrf)
    0xc0,                          //   END_COLLECTION
    0x09, 0x2b,                    //   USAGE (Character Report)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x85, 0x03,                    //     REPORT_ID (3)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x04,                    //     REPORT_COUNT (4)
    0x25, 0x7e,                    //     LOGICAL_MAXIMUM (126)
    0x09, 0x2c,                    //     USAGE (Display Data)
    0xb2, 0x02, 0x01,              //     FEATURE (Data,Var,Abs,Buf)
    0xc0,                          //   END_COLLECTION
    0x85, 0x04,                    //   REPORT_ID (4)
    0x09, 0x3b,                    //   USAGE (Font Report)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x7e,                    //     LOGICAL_MAXIMUM (126)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x09, 0x2c,                    //     USAGE (Display Data)
    0x91, 0x02,                    //     OUTPUT (Data,Var,Abs)
    0x95, 0x05,                    //     REPORT_COUNT (5)
    0x09, 0x3c,                    //     USAGE (Font Data)
    0x92, 0x02, 0x01,              //     OUTPUT (Data,Var,Abs,Buf)
    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
};


// HID Report Descriptor
// keyboard
static const uint8_t hid_report_desc_telephony [] =
{
    0x95, 0x01,                    // REPORT_COUNT (1)
    0x05, 0x0b,                    // USAGE_PAGE (Telephony Devices)
    0x09, 0x01,                    // USAGE (Phone)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x07,                    //   USAGE (Programmable Button)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x06,                    //     USAGE_MAXIMUM (Button 6)
    0x75, 0x03,                    //     REPORT_SIZE (3)
    0x15, 0x01,                    //     LOGICAL_MINIMUM (1)
    0x25, 0x06,                    //     LOGICAL_MAXIMUM (6)
    0x81, 0x00,                    //     INPUT (Data,Ary,Abs)
    0xc0,                          //   END_COLLECTION
    0x05, 0x0b,                    //   USAGE_PAGE (Telephony Devices)
    0x09, 0x06,                    //   USAGE (Telephony Key Pad)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x0c,                    //     USAGE_MAXIMUM (Button 12)
    0x25, 0x0c,                    //     LOGICAL_MAXIMUM (12)
    0x75, 0x04,                    //     REPORT_SIZE (4)
    0x81, 0x00,                    //     INPUT (Data,Ary,Abs)
    0xc0,                          //   END_COLLECTION
    0x05, 0x0b,                    //   USAGE_PAGE (Telephony Devices)
    0x09, 0x20,                    //   USAGE (Hook Switch)
    0x09, 0x29,                    //   USAGE (Alternate Function)
    0x09, 0x2c,                    //   USAGE (Conference)
    0x09, 0x25,                    //   USAGE (Transfer)
    0x09, 0x26,                    //   USAGE (Drop)
    0x09, 0x23,                    //   USAGE (Hold)
    0x09, 0x2b,                    //   USAGE (Speaker Phone)
    0x25, 0x07,                    //   LOGICAL_MAXIMUM (7)
    0x75, 0x03,                    //   REPORT_SIZE (3)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0x05, 0x0c,                    //   USAGE_PAGE (Consumer Devices)
    0x09, 0xe0,                    //   USAGE (Volume)
    0x15, 0xff,                    //   LOGICAL_MINIMUM (-1)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x02,                    //   REPORT_SIZE (2)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x75, 0x04,                    //   REPORT_SIZE (4)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x09, 0x3a,                    //   USAGE (Usage Selected Indicator)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x05, 0x0b,                    //     USAGE_PAGE (Telephony Devices)
    0x09, 0x07,                    //     USAGE (Programmable Button)
    0xa1, 0x02,                    //     COLLECTION (Logical)
    0x05, 0x09,                    //       USAGE_PAGE (Button)
    0x19, 0x01,                    //       USAGE_MINIMUM (Button 1)
    0x29, 0x02,                    //       USAGE_MAXIMUM (Button 2)
    0x95, 0x02,                    //       REPORT_COUNT (2)
    0x91, 0x02,                    //       OUTPUT (Data,Var,Abs)
    0xc0,                          //     END_COLLECTION
    0xc0,                          //   END_COLLECTION
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x09, 0x3b,                    //   USAGE (Usage In Use Indicator)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x05, 0x0b,                    //     USAGE_PAGE (Telephony Devices)
    0x09, 0x07,                    //     USAGE (Programmable Button)
    0xa1, 0x02,                    //     COLLECTION (Logical)
    0x05, 0x09,                    //       USAGE_PAGE (Button)
    0x19, 0x01,                    //       USAGE_MINIMUM (Undefined)
    0x29, 0x06,                    //       USAGE_MAXIMUM (Undefined)
    0x95, 0x06,                    //       REPORT_COUNT (6)
    0x91, 0x02,                    //       OUTPUT (Data,Var,Abs)
    0xc0,                          //     END_COLLECTION
    0x05, 0x0b,                    //     USAGE_PAGE (Telephony Devices)
    0x09, 0x29,                    //     USAGE (Alternate Function)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x91, 0x02,                    //     OUTPUT (Data,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x09, 0x3c,                    //   USAGE (Usage Multi Mode Indicator)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x05, 0x0b,                    //     USAGE_PAGE (Telephony Devices)
    0x09, 0x73,                    //     USAGE (Message)
    0xa1, 0x02,                    //     COLLECTION (Logical)
    0x05, 0x08,                    //       USAGE_PAGE (LEDs)
    0x09, 0x3d,                    //       USAGE (Indicator On)
    0x09, 0x40,                    //       USAGE (Indicator Fast Blink)
    0x09, 0x41,                    //       USAGE (Indicator Off)
    0x75, 0x02,                    //       REPORT_SIZE (2)
    0x91, 0x00,                    //       OUTPUT (Data,Ary,Abs)
    0xc0,                          //     END_COLLECTION
    0xc0,                          //   END_COLLECTION
    0x75, 0x05,                    //   REPORT_SIZE (5)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0xc0                           // END_COLLECTION
};

// HID Report Descriptor used for this function
//#define HIDREPORTDESC HID_report_desc_keyboard
#define HIDREPORTDESC HID_report_desc_keyboard_4arrows
//#define HIDREPORTDESC HID_report_desc_mouse
//#define HIDREPORTDESC HID_report_desc_display
//#define HIDREPORTDESC hid_report_desc_telephony


/* HID Human Interface Device */
static unsigned fill_HID_XXXX_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed)
{
	const void * const pattern = HIDREPORTDESC;
	const unsigned patternlength = sizeof HIDREPORTDESC;
	//
	unsigned n = 0;
	//
	n += HID_InterfaceDescXXXX(fill, p + n, maxsize - n);	/* HID: HID Interface Descriptor */
	n += HID_Descriptor(fill, p + n, maxsize - n, patternlength);	/* HID Descriptor */
#if defined (HIDMOUSE_INT_DATA_SIZE)
	n += fill_HID_Mouse_IntEP(fill, p + n, maxsize - n, highspeed, USB_ENDPOINT_IN(USBD_EP_HIDMOUSE_INT));
#elif defined (HIDKEYBOARD_INT_DATA_SIZE)
	n += fill_HID_Keyboard_IntEP(fill, p + n, maxsize - n, highspeed, USB_ENDPOINT_IN(USBD_EP_HIDKEYBOARD_INT));
#else
#endif
	return n;
}

#endif /* WITHUSBHID */

#if WITHUSBDFU

static unsigned DFU_InterfaceAssociationDesc(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bFirstInterface,
	uint_fast8_t bInterfaceCount
	)
{
	const uint_fast8_t length = 8;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_ASSOC_DESCRIPTOR_TYPE;	// bDescriptorType: IAD
		* buff ++ = bFirstInterface;			// bFirstInterface
		* buff ++ = bInterfaceCount;	// bInterfaceCount
		* buff ++ = 0xFE;	// bFunctionClass: Application-Specific
		* buff ++ = 0x01;	// bFunctionSubClass Application-Specific
		* buff ++ = 0x02;	// bFunctionProtocol 1-Runtime protocol, 2-DFU mode protocol.
		* buff ++ = STRING_ID_DFU;				/* iInterface */
	}
	return length;
}


/* Table 4.1 Run-Time DFU Interface Descriptor */
static unsigned DFU_InterfaceDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bInterfaceNumber,
	uint_fast8_t bAlternateSetting,
	uint_fast8_t iInterface
	)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE; /* bDescriptorType */
		* buff ++ = bInterfaceNumber;			/* bInterfaceNumber */
		* buff ++ = bAlternateSetting;			/* bAlternateSetting */
		* buff ++ = 0x00;                       /* bNumEndpoints */
		* buff ++ = 0xFE;						/* bInterfaceClass Application-Specific */
		* buff ++ = 0x01;						/* bInterfaceSubClass Device Firmware Upgrade */
		* buff ++ = 0x02;						/* bInterfaceProtocol: 1-Runtime protocol, 2-DFU mode protocol. */
		* buff ++ = iInterface;					/* iInterface */
		/* 09 byte*/
	}
	return length;
}

/* 4.1.3 Run-Time DFU Functional Descriptor */
static unsigned DFU_FunctionalDescReadWrite(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast16_t wTransferSize
	)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// ? DFU_DETACH_MASK
		const uint_fast8_t bmAttributes = 
			//1 * (1u << 3) |		/* Bit 3: bitWillDetach - есть у STM32H7 */
			1 * (1u << 2) |		/* Bit 2: bitManifestationTolerant  */
			1 * (1u << 1) |		/* Bit 1: upload capable ( bitCanUpload ) */
			1 * (1u << 0) |		/* Bit 0: download capable  */
			0;
		const uint_fast16_t bcdDFUVersion = 0x0110;
		const uint_fast16_t wDetachTimeOut = 500;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = DFU_DESCRIPTOR_TYPE;	// bDescriptorType: DFU FUNCTIONAL descriptor type 
		* buff ++ = bmAttributes;		// bmAttributes
		* buff ++ = LO_BYTE(wDetachTimeOut);	
		* buff ++ = HI_BYTE(wDetachTimeOut);	
		* buff ++ = LO_BYTE(wTransferSize);		
		* buff ++ = HI_BYTE(wTransferSize);		
		* buff ++ = LO_BYTE(bcdDFUVersion);		
		* buff ++ = HI_BYTE(bcdDFUVersion);		
	}
	return length;
}
/* 4.1.3 Run-Time DFU Functional Descriptor */
static unsigned DFU_FunctionalDescWriteOnly(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast16_t wTransferSize
	)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// ? DFU_DETACH_MASK
		const uint_fast8_t bmAttributes = 
			1 * (1u << 3) |		/* Bit 3: bitWillDetach - есть у STM32H7 */
			1 * (1u << 2) |		/* Bit 2: bitManifestationTolerant  */
			//1 * (1u << 1) |		/* Bit 1: upload capable ( bitCanUpload ) */
			1 * (1u << 0) |		/* Bit 0: download capable  */
			0;
		const uint_fast16_t bcdDFUVersion = 0x0110;
		const uint_fast16_t wDetachTimeOut = 500;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = DFU_DESCRIPTOR_TYPE;	// bDescriptorType: DFU FUNCTIONAL descriptor type 
		* buff ++ = bmAttributes;		// bmAttributes
		* buff ++ = LO_BYTE(wDetachTimeOut);	
		* buff ++ = HI_BYTE(wDetachTimeOut);	
		* buff ++ = LO_BYTE(wTransferSize);		
		* buff ++ = HI_BYTE(wTransferSize);		
		* buff ++ = LO_BYTE(bcdDFUVersion);		
		* buff ++ = HI_BYTE(bcdDFUVersion);		
	}
	return length;
}


/* DFU USB Device Firmware Upgrade support */
static unsigned fill_DFU_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed)
{
	uint_fast8_t ialt = 0;
	unsigned n = 0;
	//
	n += DFU_InterfaceAssociationDesc(fill, p + n, maxsize - n, INTERFACE_DFU_CONTROL, INTERFACE_DFU_count);

#if WIHSPIDFHW || WIHSPIDFSW
	n += DFU_InterfaceDesc(fill, p + n, maxsize - n, INTERFACE_DFU_CONTROL, ialt, STRING_ID_DFU_0);	/* DFU Interface Descriptor */
	n += DFU_FunctionalDescReadWrite(fill, p + n, maxsize - n, usbd_dfu_get_xfer_size(ialt));	/* DFU Functional Descriptor */
	ialt += 1;

#if BOOTLOADER_SELFSIZE
	n += DFU_InterfaceDesc(fill, p + n, maxsize - n, INTERFACE_DFU_CONTROL, ialt, STRING_ID_DFU_1);	/* DFU Interface Descriptor */
	n += DFU_FunctionalDescReadWrite(fill, p + n, maxsize - n, usbd_dfu_get_xfer_size(ialt));	/* DFU Functional Descriptor */
	ialt += 1;
#endif /* BOOTLOADER_SELFSIZE */

#endif /* WIHSPIDFHW || WIHSPIDFSW */

#if WITHISBOOTLOADER && defined (BOOTLOADER_RAMAREA) && BOOTLOADER_RAMSIZE
	n += DFU_InterfaceDesc(fill, p + n, maxsize - n, INTERFACE_DFU_CONTROL, ialt, STRING_ID_DFU_2);	/* DFU Interface Descriptor */
	n += DFU_FunctionalDescWriteOnly(fill, p + n, maxsize - n, usbd_dfu_get_xfer_size(ialt));	/* DFU Functional Descriptor */
	ialt += 1;
#endif /* WITHISBOOTLOADER && defined (BOOTLOADER_RAMAREA) && BOOTLOADER_RAMSIZE */

	ASSERT(ialt != 0);	/* If no DFU valid options enabled. */
	return n;
}

#endif /* WITHUSBDFU */

#if WITHUSBDMTP

// Interface Association Descriptor
// documented in USB ECN : Interface Association Descriptor - InterfaceAssociationDesc_ecn.pdf
static unsigned MTP_InterfaceAssociationDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bFirstInterface,
	uint_fast8_t bInterfaceCount
	)
{
	const uint_fast8_t length = 8;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_ASSOC_DESCRIPTOR_TYPE;	// bDescriptorType: IAD
		* buff ++ = bFirstInterface;			// bFirstInterface
		* buff ++ = bInterfaceCount;	// bInterfaceCount
		* buff ++ = USB_MTP_INTRERFACE_CLASS;	// bFunctionClass: Audio
		* buff ++ = USB_MTP_INTRERFACE_SUB_CLASS;	// bFunctionSubClass
		* buff ++ = USB_MTP_INTRERFACE_PROTOCOL;	// bFunctionProtocol
		* buff ++ = STRING_ID_MTP;	// Interface string index
		/* 8 bytes */
	}
	return length;
}

static unsigned MTP_InterfaceDescControlIf(
		uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
		uint_fast8_t bInterfaceNumber,
		uint_fast8_t bAlternateSetting
		)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;  // INTERFACE descriptor type (bDescriptorType)
		* buff ++ = bInterfaceNumber;				// Index of this interface. (bInterfaceNumber)
		* buff ++ = bAlternateSetting;				// Index of this alternate setting. (bAlternateSetting) - zero-based index
		* buff ++ = 0x03;							// bNumEndpoints
		* buff ++ = USB_MTP_INTRERFACE_CLASS;		// bFunctionClass: Audio
		* buff ++ = USB_MTP_INTRERFACE_SUB_CLASS;	// bFunctionSubClass
		* buff ++ = USB_MTP_INTRERFACE_PROTOCOL;	// bFunctionProtocol
		* buff ++ = STRING_ID_0;					/* Unused iInterface */
		/* 9 bytes */
	}
	return length;
}

//	/********************  MTP **** interface ********************/
//	MTP_INTERFACE_DESC_SIZE,                      /* bLength: Interface Descriptor size */
//	USB_DESC_TYPE_INTERFACE,                      /* bDescriptorType: Interface descriptor type */
//	MTP_CMD_ITF_NBR,                              /* bInterfaceNumber: Number of Interface */
//	0x00,                                         /* bAlternateSetting: Alternate setting */
//	0x03,                                         /* bNumEndpoints:  */
//	USB_MTP_INTRERFACE_CLASS,                     /* bInterfaceClass: bInterfaceClass: user's interface for MTP */
//	USB_MTP_INTRERFACE_SUB_CLASS,                 /* bInterfaceSubClass:Abstract Control Model */
//	USB_MTP_INTRERFACE_PROTOCOL,                  /* bInterfaceProtocol: Common AT commands */
//	0x00,                                         /* iInterface: */
//
//	/********************  MTP   Endpoints ********************/
//	MTP_ENDPOINT_DESC_SIZE,                       /* Endpoint descriptor length = 7 */
//	USB_DESC_TYPE_ENDPOINT,                       /* Endpoint descriptor type */
//	MTP_IN_EP,                                    /* Endpoint address (IN, address 1) */
//	USBD_EP_TYPE_BULK,                            /* Bulk endpoint type */
//	LOBYTE(MTP_DATA_MAX_FS_PACKET_SIZE),
//	HIBYTE(MTP_DATA_MAX_FS_PACKET_SIZE),
//	0x00,                                         /* Polling interval in milliseconds */
//
//	MTP_ENDPOINT_DESC_SIZE,                       /* Endpoint descriptor length = 7 */
//	USB_DESC_TYPE_ENDPOINT,                       /* Endpoint descriptor type */
//	MTP_OUT_EP,                                   /* Endpoint address (OUT, address 1) */
//	USBD_EP_TYPE_BULK,                            /* Bulk endpoint type */
//	LOBYTE(MTP_DATA_MAX_FS_PACKET_SIZE),
//	HIBYTE(MTP_DATA_MAX_FS_PACKET_SIZE),
//	0x00,                                         /* Polling interval in milliseconds */
//
//	MTP_ENDPOINT_DESC_SIZE,                       /* bLength: Endpoint Descriptor size */
//	USB_DESC_TYPE_ENDPOINT,                       /* bDescriptorType:*/
//	MTP_CMD_EP,                                   /* bEndpointAddress: Endpoint Address (IN) */
//	USBD_EP_TYPE_INTR,                            /* bmAttributes: Interrupt endpoint */
//	LOBYTE(MTP_CMD_PACKET_SIZE),
//	HIBYTE(MTP_CMD_PACKET_SIZE),
//	MTP_FS_BINTERVAL                              /* Polling interval in milliseconds */
static unsigned MTP_EndpointIn(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(MTP_DATA_MAX_PACKET_SIZE); // was: 0x300
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	// bDescriptorType
		* buff ++ = bEndpointAddress;                    // bEndpointAddress
		* buff ++ = USBD_EP_TYPE_BULK; 				// bmAttributes
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize);
		* buff ++ = 0x00;  						  /* bInterval */
	}
	return length;
}

static unsigned MTP_EndpointOut(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(MTP_DATA_MAX_PACKET_SIZE); // was: 0x300
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	// bDescriptorType
		* buff ++ = bEndpointAddress;                    // bEndpointAddress
		* buff ++ = USBD_EP_TYPE_BULK; // bmAttributes
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize);
		* buff ++ = 0x00;  						  /* bInterval */
	}
	return length;
}

static unsigned MTP_EndpointInt(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(MTP_CMD_PACKET_SIZE); // was: 0x300
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength: Endpoint Descriptor size */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	// bDescriptorType
		* buff ++ = bEndpointAddress;                    // bEndpointAddress
		* buff ++ = USBD_EP_TYPE_INTR; // bmAttributes: Interrupt endpoint
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize);
		* buff ++ = highspeed ? MTP_HS_BINTERVAL : MTP_FS_BINTERVAL;    /* bInterval */
	}
	return length;
}

static unsigned fill_MTP_XXXX_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed)
{
	unsigned n = 0;
	//
	n += MTP_InterfaceAssociationDesc(fill, p + n, maxsize - n, INTERFACE_MTP_CONTROL, INTERFACE_MTP_count);
	n += MTP_InterfaceDescControlIf(fill, p + n, maxsize - n, INTERFACE_MTP_CONTROL, 0);	/* INTERFACE_CDC_CONTROL Interface Descriptor 3/0 CDC Control, 1 Endpoint */
	n += MTP_EndpointIn(fill, p + n, maxsize - n, highspeed, USBD_EP_MTP_IN);
	n += MTP_EndpointOut(fill, p + n, maxsize - n, highspeed, USBD_EP_MTP_OUT);
	n += MTP_EndpointInt(fill, p + n, maxsize - n, highspeed, USBD_EP_MTP_INT);
	//
	return n;
}

#endif /* WITHUSBDMTP */

#if WITHUSBDMSC

// Interface Association Descriptor
// documented in USB ECN : Interface Association Descriptor - InterfaceAssociationDesc_ecn.pdf
static unsigned MSC_InterfaceAssociationDesc(uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bFirstInterface,
	uint_fast8_t bInterfaceCount
	)
{
	const uint_fast8_t length = 8;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_INTERFACE_ASSOC_DESCRIPTOR_TYPE;	// bDescriptorType: IAD
		* buff ++ = bFirstInterface;			// bFirstInterface
		* buff ++ = bInterfaceCount;	// bInterfaceCount
		* buff ++ = USB_DEVICE_CLASS_STORAGE;	// bFunctionClass: Mass Storage
		* buff ++ = 0x06;	// bFunctionSubClass SCSI Transparent Command Set
		* buff ++ = 0x50;	// bFunctionProtocol Bulk-Only Transport
		* buff ++ = STRING_ID_MSC;	// Interface string index
		/* 8 bytes */
	}
	return length;
}

static unsigned MSC_InterfaceDescControlIf(
		uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
		uint_fast8_t bInterfaceNumber,
		uint_fast8_t bAlternateSetting
		)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;  // INTERFACE descriptor type (bDescriptorType)
		* buff ++ = bInterfaceNumber;				// Index of this interface. (bInterfaceNumber)
		* buff ++ = bAlternateSetting;				// Index of this alternate setting. (bAlternateSetting) - zero-based index
		* buff ++ = 0x02;							// bNumEndpoints
		* buff ++ = USB_DEVICE_CLASS_STORAGE;	// bFunctionClass: Mass Storage
		* buff ++ = 0x06;	// bFunctionSubClass SCSI Transparent Command Set
		* buff ++ = 0x50;	// bFunctionProtocol Bulk-Only Transport
		* buff ++ = STRING_ID_0;					/* Unused iInterface */
		/* 9 bytes */
	}
	return length;
}
static unsigned MSC_EndpointIn(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(highspeed ? MSC_DATA_MAX_PACKET_SIZE_HS : MSC_DATA_MAX_PACKET_SIZE_FS); // was: 0x300
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	// bDescriptorType
		* buff ++ = bEndpointAddress;                    // bEndpointAddress
		* buff ++ = USBD_EP_TYPE_BULK; 				// bmAttributes
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize);
		* buff ++ = 0x00;  						  /* bInterval */
	}
	return length;
}

static unsigned MSC_EndpointOut(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(highspeed ? MSC_DATA_MAX_PACKET_SIZE_HS : MSC_DATA_MAX_PACKET_SIZE_FS); // was: 0x300
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	// bDescriptorType
		* buff ++ = bEndpointAddress;                    // bEndpointAddress
		* buff ++ = USBD_EP_TYPE_BULK; // bmAttributes
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize);
		* buff ++ = 0x00;  						  /* bInterval */
	}
	return length;
}

static unsigned fill_MSC_XXXX_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed)
{
	unsigned n = 0;
	//
	n += MSC_InterfaceAssociationDesc(fill, p + n, maxsize - n, INTERFACE_MSC_CONTROL, INTERFACE_MSC_count);
	n += MSC_InterfaceDescControlIf(fill, p + n, maxsize - n, INTERFACE_MSC_CONTROL, 0);	/* INTERFACE_CDC_CONTROL Interface Descriptor 3/0 CDC Control, 1 Endpoint */
	n += MSC_EndpointIn(fill, p + n, maxsize - n, highspeed, USBD_EP_MSC_IN);
	n += MSC_EndpointOut(fill, p + n, maxsize - n, highspeed, USBD_EP_MSC_OUT);
	//
	return n;
}

#endif /* WITHUSBDMSC */

// последовательность должна соответствовать порядку в enum interfaces_tag
static unsigned fill_Configuration_compound(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed)
{
	unsigned n = 0;

#if WITHUSBDFU
	n += fill_DFU_function(fill, p + n, maxsize - n, highspeed);
#endif /* WITHUSBDFU */

#if WITHUSBCDCACM
	/* создаем одно или несколько (WITHUSBCDCACM_N) устройств */
	n += fill_CDCACM_function(fill, p + n, maxsize - n, highspeed);
#endif /* WITHUSBCDCACM */

#if WITHUSBUAC
	#if WITHUAC2
		n += fill_UAC2_function(fill, p + n, maxsize - n, highspeed);
	#else /* WITHUAC2 */
		n += fill_UAC1_function(fill, p + n, maxsize - n, highspeed);
	#endif /* WITHUAC2 */
#endif /* WITHUSBUAC */

#if WITHUSBRNDIS
	n += fill_RNDIS_function(fill, p + n, maxsize - n, highspeed);
#endif /* WITHUSBRNDIS */

#if WITHUSBCDCEEM
	n += fill_CDCEEM_function(fill, p + n, maxsize - n, highspeed);
#endif /* WITHUSBCDCEEM */

#if WITHUSBCDCECM
	n += fill_CDCECM_function(fill, p + n, maxsize - n, highspeed);
#endif /* WITHUSBCDCECM */

#if WITHUSBHID
	n += fill_HID_XXXX_function(fill, p + n, maxsize - n, highspeed);
#endif /* WITHUSBHID */

#if WITHUSBDMTP
	n += fill_MTP_XXXX_function(fill, p + n, maxsize - n, highspeed);
#endif /* WITHUSBDMTP */

#if WITHUSBDMSC
	n += fill_MSC_XXXX_function(fill, p + n, maxsize - n, highspeed);
#endif /* WITHUSBDMSC */

	return n;
}

typedef unsigned (* fill_func_t)(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed);

// Only for high speed capable devices 
// Other Speed Configuration descriptor - pass highspeed=1
// For all devices
// Configuration descriptor - pass highspeed=0
static unsigned fill_Configuration_descriptor(
		uint8_t * buff, unsigned maxsize, int highspeed,
		const uint_fast8_t bConfigurationValue, // = 0x01;
		const uint_fast8_t bNumInterfaces, // = INTERFACE_count;
		const fill_func_t fill_main_group	// fill functional descriptor(s)
		)
{
	const int highspeedEPs = highspeed;
	unsigned length = 9;
	unsigned totalsize = length + fill_main_group(0, buff, maxsize - length, highspeedEPs);
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;                                      /* bLength */ 
		* buff ++ = /*highspeed ? USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE : */ USB_CONFIGURATION_DESCRIPTOR_TYPE;      /* descriptor type */
		* buff ++ = LO_BYTE(totalsize);		/* length of packed config descr. (16 bit) */
		* buff ++ = HI_BYTE(totalsize);		/* length of packed config descr. (16 bit) */
		* buff ++ = bNumInterfaces;			/* bNumInterfaces  */
		* buff ++ = bConfigurationValue;    /* bConfigurationValue - Value to use as an argument to the SetConfiguration() request to select this configuration */
		* buff ++ = STRING_ID_0;       		/* iConfiguration - Index of string descriptor describing this configuration */
		* buff ++ = 0xC0;                   /* bmAttributes  BUS Powred, self powered. See USBD_SELF_POWERED */
		* buff ++ = USB_CONFIG_POWER_MA(250);/* bMaxPower = 250 mA. Сделано как попытка улучшить работу через активные USB изоляторы для обеспечения их питания. */

		fill_main_group(1, buff, maxsize - length, highspeedEPs);
	}
	return totalsize;
}

// Device Descriptor
static unsigned fill_Device_descriptor(uint8_t * buff, unsigned maxsize, uint_fast8_t bNumConfigurations)
{
	const unsigned length = 18;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	const uint_fast16_t iVendorId = usbd_get_vendorId();
	const uint_fast16_t iProductId = usbd_get_productId();
	if (buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;					    /*  0:bLength */
		* buff ++ = USB_DEVICE_DESCRIPTOR_TYPE;         /*  1:bDescriptorType */
		* buff ++ = LO_BYTE(USB_FUNCTION_BCD_USB);      /*  2:bcdUSB_FUNCTION_lo */
		* buff ++ = HI_BYTE(USB_FUNCTION_BCD_USB);		/*  3:bcdUSB_FUNCTION_hi */
		* buff ++ = USB_DEVICE_CLASS_MISCELLANEOUS;		/*  4:bDeviceClass */
		* buff ++ = 2;		                            /*  5:bDeviceSubClass - Common Class Sub Class */
		* buff ++ = 1;									/*  6:bDeviceProtocol - Interface Association Descriptor protocol */
		* buff ++ = USB_OTG_MAX_EP0_SIZE;               /*  7:bMaxPacketSize0 (for DCP), For 3x: 09H is the only valid value in this field when operating at Gen X speed. */
		* buff ++ = LO_BYTE(iVendorId);    				/*  8:idVendor_lo */
		* buff ++ = HI_BYTE(iVendorId);					/*  9:idVendor_hi */
		* buff ++ = LO_BYTE(iProductId);   				/* 10:idProduct_lo */
		* buff ++ = HI_BYTE(iProductId);				/* 11:idProduct_hi */
		* buff ++ = LO_BYTE(USB_FUNCTION_RELEASE_NO);   /* 12:bcdDevice_lo */
		* buff ++ = HI_BYTE(USB_FUNCTION_RELEASE_NO);	/* 13:bcdDevice_hi */
		* buff ++ = STRING_ID_manuf;                        /* 14:iManufacturer */
		* buff ++ = STRING_ID_prod;                        /* 15:iProduct */
		* buff ++ = STRING_ID_sn;                        /* 16:iSerialNumber */
		* buff ++ = bNumConfigurations;                 /* 17:bNumConfigurations */
	}
	return length;
}

// Only for high speed capable devices 
// Device Qualifier Descriptor 
static unsigned fill_DeviceQualifier_descriptor(
	uint8_t * buff, unsigned maxsize,
	uint_fast8_t bNumConfigurations	/* number of other-speed configurations */
	)
{
	const unsigned length = 10;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;                                 /*  0:bLength */
		* buff ++ = USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE;	/*  1:bDescriptorType */
		* buff ++ = LO_BYTE(USB_FUNCTION_BCD_USB);			/*  2:bcdUSB_FUNCTION_lo */
		* buff ++ = HI_BYTE(USB_FUNCTION_BCD_USB);			/*  3:bcdUSB_FUNCTION_hi */
		* buff ++ = USB_DEVICE_CLASS_MISCELLANEOUS;			/*  4:bDeviceClass - Miscellaneous */
		* buff ++ = 2;										/*  5:bDeviceSubClass - Common Class Sub Class */
		* buff ++ = 1;										/*  6:bDeviceProtocol - Interface Association Descriptor protocol */
		* buff ++ = USB_OTG_MAX_EP0_SIZE;                   /*  7:bMaxPacketSize0 (for DCP), For 3x: 09H is the only valid value in this field when operating at Gen X speed. */
		* buff ++ = bNumConfigurations;                     /*  8:bNumConfigurations - number of other-speed configurations */
		* buff ++ = 0;                                      /*  9:bReserved */
	}
	return length;
}

// Device Capability Descriptor - USB 2.0 Extension
static unsigned fill_devcaps_usb20ext(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast32_t bmAttributes = 
#if (USBD_LPM_ENABLED == 1)
			(1uL << 1) |	/* Link Power Management capability bit set */
#endif /* (USBD_LPM_ENABLED == 1) */
			0;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						/* bLength */
		* buff ++ = USB_DEVICE_CAPABITY_DESCRIPTOR_TYPE;	/* bDescriptorType: Device Capability */
		* buff ++ = 0x02;						/* bDevCapabilityType: 0x02: USB 2.0 Extension */
		* buff ++ = LO_BYTE(bmAttributes);
		* buff ++ = HI_BYTE(bmAttributes);
		* buff ++ = HI_24BY(bmAttributes);
		* buff ++ = HI_32BY(bmAttributes);
	}
	return length;
}

// Device Capability Descriptor - Container ID 
static unsigned fill_devcaps_ContainerID(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
    typedef struct _GUID {
        uint32_t Data1;
        uint16_t Data2;
        uint16_t Data3;
        uint8_t Data4 [8];
    } GUID;

	const uint_fast8_t length = 20;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// {1D614FE7-58C2-42d8-942E-CC2A696218DB}
		static const GUID ContainerID = 
			{ 0x1d614fe7, 0x58c2, 0x42d8, { 0x94, 0x2e, 0xcc, 0x2a, 0x69, 0x62, 0x18, 0xdb } };

		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						/* bLength */
		* buff ++ = USB_DEVICE_CAPABITY_DESCRIPTOR_TYPE;	/* bDescriptorType: Device Capability */
		* buff ++ = 0x04;						/* bDevCapabilityType: 0x04: Container ID */
		* buff ++ = 0x00;						/* bReserved */
		memcpy(buff, & ContainerID, 16);		/* ContainerID */
	}
	return length;
}

static unsigned fill_DevCaps_group(uint_fast8_t fill, uint8_t * p, unsigned maxsize, uint_fast8_t bNumDeviceCaps)
{
	unsigned n = 0;
	// Device Capability Descriptor - USB 2.0 Extension 
	n += fill_devcaps_usb20ext(fill, p + n, maxsize - n);
	// Device Capability Descriptor - SuperSpeed USB 
	//n += fill_devcaps_ContainerID(fill, p + n, maxsize - n);
	// Device Capability Descriptor - Container ID 
	n += fill_devcaps_ContainerID(fill, p + n, maxsize - n);

	ASSERT(bNumDeviceCaps == 2);
	return n;
}

// Binary Device Object Store (BOS) Descriptor
static unsigned fill_BinaryDeviceObjectStore_descriptor(uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t bNumDeviceCaps = 2;
	unsigned length = 5;
	unsigned totalsize = length + fill_DevCaps_group(0, buff, maxsize - length, bNumDeviceCaps);
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;                                 /*  0:bLength */
		* buff ++ = USB_BOS_TYPE;							/*  1:bDescriptorType */
		* buff ++ = LO_BYTE(totalsize);						/* wTotalLength length of packed config descr. (16 bit) */
		* buff ++ = HI_BYTE(totalsize);						/* wTotalLength length of packed config descr. (16 bit) */
		* buff ++ = bNumDeviceCaps;										/*  4:bNumDeviceCaps */
		fill_DevCaps_group(1, buff, maxsize - length, bNumDeviceCaps);
	}
	return totalsize;
}

// OTG Descriptor
// See table 6.1 of OTG1_0a.pdf
static unsigned fill_OTG_descriptor(uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t bmAttributes = 0x00;	// D1: HNP support, D0: SRP support
	unsigned length = 3;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;                 /* bLength */
		* buff ++ = USB_DESC_TYPE_OTG;		/* bDescriptorType (0x09 - OTG) */
		* buff ++ = bmAttributes;			/* bmAttributes */
	}
	return length;
}

static unsigned fill_align4(uint8_t * buff, unsigned maxsize)
{
	const uintptr_t granulation = DCACHEROWSIZE;
	return (granulation - ((uintptr_t) buff & (granulation - 1))) & (granulation - 1);
}

static unsigned fill_langid_descriptor(uint8_t * buff, unsigned maxsize, uint_fast16_t langid)
{
	const unsigned length = 4;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;                          /* bLength */ 
		* buff ++ = USB_STRING_DESCRIPTOR_TYPE;           /* descriptor type */ 
		* buff ++ = LO_BYTE(langid); 
		* buff ++ = HI_BYTE(langid); 
	}
	return length;
}

// todo: ограничить размер дескриптора значением 254
static unsigned fill_string_descriptor(uint8_t * buff, unsigned maxsize, const char * s)
{
	const unsigned length = 2 + 2 * strlen(s);
	ASSERT(length < 256 && maxsize >= length);
	if (maxsize < length || length >= 256)
		return 0;
	if (buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;                                      /* bLength */ 
		* buff ++ = USB_STRING_DESCRIPTOR_TYPE;           /* descriptor type */ 
		for (;;)
		{
			const int c = (unsigned char) * s ++;
			if (c == '\0')
				break;
			const wchar_t wc = c;	// todo: для не-english преобразовывать
			* buff ++ = LO_BYTE(wc);
			* buff ++ = HI_BYTE(wc);
		}
	}
	return length;
}

// Инициализация дескриптора произвольным массивом данных
static unsigned fill_pattern_descriptor(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, const void * pattern, unsigned length)
{
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill && buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		memcpy(buff, pattern, length);
	}
	return length;
}

static unsigned fill_string_c2descriptor(
	uint_fast8_t fill,
	uint8_t * buff,
	unsigned maxsize,
	const wchar_t * s,
	size_t n	// nul include
	)
{
	const unsigned length = 2 + n * 2;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill && buff != NULL)
	{
		* buff ++ = LO_BYTE(n * 2);   /* dwLength */
		* buff ++ = HI_BYTE(n * 2);
		while (n --)
		{
			const wchar_t c = * s ++;
			* buff ++ = LO_BYTE(c);   /* character */
			* buff ++ = HI_BYTE(c);
		}
	}
	return length;
}

static unsigned fill_string_c4descriptor(
	uint_fast8_t fill,
	uint8_t * buff,
	unsigned maxsize,
	const wchar_t * s,
	size_t n	// nul include
	)
{
	const unsigned length = 4 + n * 2;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill && buff != NULL)
	{
		* buff ++ = LO_BYTE(n * 2);   /* dwLength */
		* buff ++ = HI_BYTE(n * 2);
		* buff ++ = HI_24BY(n * 2);
		* buff ++ = HI_32BY(n * 2);
		while (n --)
		{
			const wchar_t c = * s ++;
			* buff ++ = LO_BYTE(c);   /* character */
			* buff ++ = HI_BYTE(c);
		}
	}
	return length;
}

// Add value to HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum\USB\VID_F120&PID_0754&MI_01\9&13a6429d&0&0001\Device Parameters

static unsigned fill_extprop_descriptor(
	uint8_t * buff,
	unsigned maxsize,
	uint_fast16_t dwPropertyDataType,	// 0x00000001 (Unicode string)  0x00000002 (Unicode string with environment variables) 0x07 0x00000001 (Unicode string)
	const wchar_t * name,	// binary array start
	size_t namelen,		// binary array length
	const wchar_t * value,
	size_t valuelen
	)
{
	unsigned lengthprops =
			//{
			4 + 	// dwSize
			4 +		// dwPropertyDataType
			fill_string_c2descriptor(0, buff, maxsize, name, namelen) +	// wPropertyNameLength, bPropertyName
			fill_string_c4descriptor(0, buff, maxsize, value, valuelen) +	// dwPropertyDataLength, bPropertyData
			//}
			0;
	unsigned lengthheader =
			4 +		// dwLength
			2 + 	// bcdVersion
			2 + 	// wIndex
			2 +		// wCount
			0;
	unsigned length = lengthheader + lengthprops;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (1 && buff != NULL)
	{
		const uint_fast16_t bcdVersion = 0x0100;
		const uint_fast16_t wIndex = 0x05;
		const uint_fast16_t wCount = 1;
		uint_fast16_t n;
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = LO_BYTE(length);   /* dwLength */
		* buff ++ = HI_BYTE(length);
		* buff ++ = HI_24BY(length);
		* buff ++ = HI_32BY(length);
		* buff ++ = LO_BYTE(bcdVersion);   /* bcdVersion */
		* buff ++ = HI_BYTE(bcdVersion);
		* buff ++ = LO_BYTE(wIndex);   /* wIndex */
		* buff ++ = HI_BYTE(wIndex);
		* buff ++ = LO_BYTE(wCount);   /* wCount */
		* buff ++ = HI_BYTE(wCount);
		n = wCount;
		while (n --)
		{
			// The DEVICE_LABEL custom property section contains the device label.
			//const uint_fast16_t dwPropertyDataType = 0x01;	// 0x00000001 (Unicode string)
			//const uint_fast16_t dwPropertyDataType = 0x02;	// 0x00000002 (Unicode string with environment variables)
			// properties
			* buff ++ = LO_BYTE(lengthprops);   /* dwSize */
			* buff ++ = HI_BYTE(lengthprops);
			* buff ++ = HI_24BY(lengthprops);
			* buff ++ = HI_32BY(lengthprops);
			* buff ++ = LO_BYTE(dwPropertyDataType);   /* dwPropertyDataType */
			* buff ++ = HI_BYTE(dwPropertyDataType);
			* buff ++ = HI_24BY(dwPropertyDataType);
			* buff ++ = HI_32BY(dwPropertyDataType);
			buff += fill_string_c2descriptor(1, buff, maxsize, name, namelen);
			buff += fill_string_c4descriptor(1, buff, maxsize, value, valuelen);
		}
	}
	return length;
}

// todo: ограничить размер дескриптора значением 254
static unsigned fill_wstring_descriptor(uint8_t * buff, unsigned maxsize, const wchar_t * s)
{
	const unsigned length = 2 + 2 * wcslen(s);
	ASSERT(length < 256 && maxsize >= length);
	if (maxsize < length || length >= 256)
		return 0;
	if (buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;                                      /* bLength */ 
		* buff ++ = USB_STRING_DESCRIPTOR_TYPE;           /* descriptor type */ 
		for (;;)
		{
			const int c = * s ++;
			if (c == L'\0')
				break;
			const wchar_t wc = c;
			* buff ++ = LO_BYTE(wc);
			* buff ++ = HI_BYTE(wc);
		}
	}
	return length;
}

#if CTLSTYLE_V3D && WITHSDRAMHW
	static RAMLOW __ALIGN_BEGIN uint8_t alldescbuffer [256 * 13] __ALIGN_END;
#else
	static __ALIGN_BEGIN uint8_t alldescbuffer [256 * 13] __ALIGN_END;
#endif /* CTLSTYLE_V3D && WITHSDRAMHW */

struct descholder MsftStringDescr [1];
struct descholder MsftCompFeatureDescr [1];	// Microsoft Compatible ID Feature Descriptor
struct descholder StringDescrTbl [STRING_ID_count];
struct descholder ConfigDescrTbl [USBD_CONFIGCOUNT];
struct descholder DeviceDescrTbl [USBD_CONFIGCOUNT];

struct descholder OtherSpeedConfigurationTbl [USBD_CONFIGCOUNT];
struct descholder DeviceQualifierTbl [USBD_CONFIGCOUNT];
struct descholder BinaryDeviceObjectStoreTbl [1];
struct descholder HIDReportDescrTbl [1];
struct descholder ExtOsPropDescTbl [32];
struct descholder OtgDescTbl [1];


uint_fast8_t usbd_get_stringsdesc_count(void)
{
	return ARRAY_SIZE(StringDescrTbl);
}


// Динамическое формирование дескрипторов
/* вызывается при запрещённых прерываниях. */
void usbd_descriptors_initialize(uint_fast8_t HSdesc)
{
	unsigned score = 0;
	unsigned offset;

	static const struct
	{
		fill_func_t fp;
		uint_fast8_t count;
		uint_fast8_t confvalue;
	} funcs [] =
	{
#if WITHPLAINDESCROPTOR
			{ fill_Configuration_compound,	INTERFACE_count, 1, }
#else /* WITHPLAINDESCROPTOR */
			{ fill_RNDIS_function,		INTERFACE_RNDIS_count, 	RNDIS_cfgidx, },	// bConfigurationValue = 1
			{ fill_CDCECM_function,		INTERFACE_CDCECM_count, CDCECM_cfgidx },	// bConfigurationValue = 2
#endif /* WITHPLAINDESCROPTOR */
	};
	const uint_fast8_t bNumConfigurations = ARRAY_SIZE(funcs);
	const uint_fast8_t bNumOtherSpeedConfigurations = 0;

	{
		// Device Descriptor
		unsigned partlen;
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_Device_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, bNumConfigurations);
		//partlen = fill_pattern_descriptor(1, alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, fullSpdDesc, sizeof fullSpdDesc);
		DeviceDescrTbl [0].size = partlen;
		DeviceDescrTbl [0].data = alldescbuffer + score;
		score += partlen;
	}

	{
		// Configuration Descriptors list
		unsigned partlen;
		uint_fast8_t index;
		for (index = 0; index < bNumConfigurations; ++ index)
		{
			// Configuration Descriptor
			score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
			partlen = fill_Configuration_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, HSdesc, funcs [index].confvalue, funcs [index].count, funcs [index].fp);
			ConfigDescrTbl [index].size = partlen;
			ConfigDescrTbl [index].data = alldescbuffer + score;
			score += partlen;
		}
	}

#if 1//WITHUSBWCID
	{
		// При наличии этого дескриптора система начинает запрашивать по всем интеряейсам MsftCompFeatureDescr
		// Microsoft OS String Descriptor 

		// https://github.com/pbatard/libwdi/wiki/WCID-Devices#Example
		// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\usbflags
		// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum\USB
		// See OS_Desc_Intro.doc, Table 3 describes the OS string descriptor’s fields.

		static const uint8_t MsftStringDescrProto [18] =
		{
			0x12,	// Descriptor length (18 bytes)
			0x03,	// Descriptor type (3 = String)
			'M', 0,	// Signature: "MSFT100"
			'S', 0,
			'F', 0,
			'T', 0,
			'1', 0,
			'0', 0,
			'0', 0,
			USBD_WCID_VENDOR_CODE,	// Vendor Code - for vendor request
			0x00,	// padding
		};
		unsigned partlen;

		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_pattern_descriptor(1, alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, MsftStringDescrProto, sizeof MsftStringDescrProto);
		MsftStringDescr [0].size = partlen;
		MsftStringDescr [0].data = alldescbuffer + score;
		score += partlen;
	}
	{


	}
#endif /* WITHUSBWCID */

#if WITHUSBDFU && WITHUSBWCID
	{
		//const uint_fast8_t ifc = INTERFACE_DFU_CONTROL;
		// https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors
		// https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-os-1-0-descriptors-specification
		// Microsoft Compatible ID Feature Descriptor
		static const uint8_t MsftCompFeatureDescrProto [40] =
		{
			0x28, 0x00, 0x00, 0x00,	// Descriptor length (40 bytes)
			0x00, 0x01,	// Version ('1.0') 
			0x04, 0x00,	// Compatibility ID Descriptor index
			0x01,							// Number of sections (1)
			0x00, 0x00, 0x00, 0x00,			// Reserved 
			0x00, 0x00, 0x00,				// Reserved 
			INTERFACE_DFU_CONTROL,			// Interface Number
			0x01,							// reserved
#if 1
			'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,				// Compatible ID - требует установки драйвера, не WCID драйвере
#else
			//'L', 'I', 'B', 'U', 'S', 'B', '0', 0x00,				// Compatible ID
			'L', 'I', 'B', 'U', 'S', 'B', 'K', 0x00,				// Compatible ID - требует установки драйвера, не WCID драйвере
#endif
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,			// Sub-Compatible ID 
			0x00, 0x00, 0x00, 0x00,			// Reserved 
			0x00, 0x00,						// Reserved 
		};
		unsigned partlen;

		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_pattern_descriptor(1, alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, MsftCompFeatureDescrProto, sizeof MsftCompFeatureDescrProto);
		MsftCompFeatureDescr [0].size = partlen;
		MsftCompFeatureDescr [0].data = alldescbuffer + score;
		score += partlen;
	}
#endif /* WITHUSBDFU */
#if WITHUSBDFU
	if (INTERFACE_count > 1)
	{
		// Add property: label=DeviceInterfaceGUIDs
		// Value (REG_MULTI_SZ)={1E9AA6EB-4456-4a09-B756-D3783B11CB87}
		// my own generated GUID
		const uint_fast8_t ifc = INTERFACE_DFU_CONTROL;
		unsigned partlen;

		// Device Qualifier
		static const wchar_t label [] = L"DeviceInterfaceGUIDs";
		const size_t labellen = ARRAY_SIZE(label);
		static const wchar_t value [] =
				L"{1E9AA6EB-4456-4a09-B756-D3783B11CB87}" L"\0"		// Extra NUL for multi-sz value
				;
		const size_t valuelen = ARRAY_SIZE(value);
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_extprop_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, 0x0007, label, labellen, value, valuelen);
		ExtOsPropDescTbl [ifc].size = partlen;
		ExtOsPropDescTbl [ifc].data = alldescbuffer + score;
		score += partlen;
	}
	else
	{
		const uint_fast8_t ifc = INTERFACE_DFU_CONTROL;
		unsigned partlen;

		// Device Qualifier
		static const wchar_t label [] = L"Label";
		const size_t labellen = ARRAY_SIZE(label);
		static const wchar_t value [] = LPRODUCTSTR L" DFU interface";
		const size_t valuelen = ARRAY_SIZE(value);
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_extprop_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, 0x0001, label, labellen, value, valuelen);
		ExtOsPropDescTbl [ifc].size = partlen;
		ExtOsPropDescTbl [ifc].data = alldescbuffer + score;
		score += partlen;
	}
#endif /* WITHUSBDFU */

#if WITHUSBCDCACM
//	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
//	{
//		const uint_fast8_t ifc = USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL, offset);
//		unsigned partlen;
//
//		// Device Qualifier
//		static const wchar_t label [] = L"Label";
//		const size_t labellen = wcslen(label) + 1;
//		wchar_t value [32];
//		swprintf(value, ARRAY_SIZE(value), /*LPRODUCTSTR */ L" Serial Port #%d", offset + 1);
//		const size_t valuelen = wcslen(value) + 1;
//		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
//		partlen = fill_extprop_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, 0x0001, label, labellen, value, valuelen);
//		ExtOsPropDescTbl [ifc].size = partlen;
//		ExtOsPropDescTbl [ifc].data = alldescbuffer + score;
//		score += partlen;
//	}
#endif /* WITHUSBCDCACM */

	if ((USB_FUNCTION_BCD_USB >= 0x0201) || (HSdesc != 0))
	{
		unsigned partlen;
		// Device Qualifier
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_DeviceQualifier_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, bNumOtherSpeedConfigurations);
		DeviceQualifierTbl [0].size = partlen;
		DeviceQualifierTbl [0].data = alldescbuffer + score;
		score += partlen;

#if 0
		/*
			The other_speed_configuration descriptor shown in Table 9-11 describes a 
			configuration of a highspeed capable device if it were operating at its 
			other possible speed.  
			The structure of the other_speed_configuration is identical to a 
			configuration descriptor.
		*/
		{
			// Other Speed Configuration descriptors list
			unsigned partlen;
			uint_fast8_t index;
			score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
			partlen = 0;
			for (index = 0; index < bNumConfigurations; ++ index)
			{
				// Configuration Descriptor
				partlen = fill_Configuration_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, ! HSdesc, funcs [index].confvalue, funcs [index].count, funcs [index].fp);
				OtherSpeedConfigurationTbl [0].size = partlen;
				OtherSpeedConfigurationTbl [0].data = alldescbuffer + score;
				score += partlen;
			}
		}
#endif
	}
	else
	{
		// Device Qualifier
		DeviceQualifierTbl [0].size = 0;
		DeviceQualifierTbl [0].data = NULL;

		// Other Speed Configuration
		OtherSpeedConfigurationTbl [0].size = 0;
		OtherSpeedConfigurationTbl [0].data = NULL;
	}

	// Binary Device Object Store (BOS) Descriptor
	// The value of the bcdUSB field in the standard USB 2.0 Device Descriptor
	// is used to indicate that the device supports the request to
	// read the BOS Descriptor (i.e. GetDescriptor(BOS)).
	// Devices that support the BOS descriptor must have a bcdUSB value of 0201H or larger.
	if (USB_FUNCTION_BCD_USB >= 0x0201)
	{
		unsigned partlen;
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_BinaryDeviceObjectStore_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		BinaryDeviceObjectStoreTbl [0].size = partlen;
		BinaryDeviceObjectStoreTbl [0].data = alldescbuffer + score;
		score += partlen;
	}
	else
	{
		BinaryDeviceObjectStoreTbl [0].size = 0;
		BinaryDeviceObjectStoreTbl [0].data = NULL;
	}
	{
		unsigned partlen;
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_OTG_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		OtgDescTbl [0].size = partlen;
		OtgDescTbl [0].data = alldescbuffer + score;
		score += partlen;
	}

	// String descriptors
	{
		// Language ID (mandatory)
		unsigned partlen;
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_langid_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, UNICODE_ENGLISH);
		StringDescrTbl [STRING_ID_0].size = partlen;
		StringDescrTbl [STRING_ID_0].data = alldescbuffer + score;
		score += partlen;

		// All string IDs, except serial number
		unsigned i;
		for (i = 0; i < ARRAY_SIZE(strtemplates); ++ i)
		{
			const uint_fast8_t id = strtemplates [i].id;
			ASSERT(id < ARRAY_SIZE(StringDescrTbl));

			score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
			partlen = fill_string_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, strtemplates [i].str);
			StringDescrTbl [id].size = partlen;
			StringDescrTbl [id].data = alldescbuffer + score;
			score += partlen;
		}

	#if WITHUSBCDCACM
		/* Формирование названий VCP */
		unsigned offset;
		for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
		{
			const uint_fast8_t id = STRING_ID_4a + offset;
			char label [32];
			local_snprintf_P(label, ARRAY_SIZE(label), PSTR(PRODUCTSTR " Serial Port #%d"), offset + 1);

			score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
			partlen = fill_string_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, label);
			StringDescrTbl [id].size = partlen;
			StringDescrTbl [id].data = alldescbuffer + score;
			score += partlen;
		}
	#endif /* WITHUSBCDCACM */

	}

#if WITHUSBHID
	{
		unsigned partlen;
		const uint_fast8_t id = 0;
		ASSERT(id < ARRAY_SIZE(HIDReportDescrTbl));

		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_pattern_descriptor(1, alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, HIDREPORTDESC, sizeof HIDREPORTDESC);
		HIDReportDescrTbl [id].size = partlen;
		HIDReportDescrTbl [id].data = alldescbuffer + score;
		score += partlen;
	}
#endif /* WITHUSBHID */
	
#if WITHUSBDFU
#if WIHSPIDFHW || WIHSPIDFSW
#if defined (BOOTLOADER_APPSIZE)
	{
		const int status = testchipDATAFLASH();

		static const char strFlashDesc_4 [] = "@SPI Flash APPLICATION: %s/0x%08lx/%02u*%03uKg";	// 128 k for bootloader
		unsigned partlen;
		const uint_fast8_t id = STRING_ID_DFU_0;
		char b [128];
		local_snprintf_P(b, ARRAY_SIZE(b), strFlashDesc_4,
			status ? USBD_DFU_FLASHNAME : nameDATAFLASH,
			(unsigned long) BOOTLOADER_APPBASE,
			(unsigned) (BOOTLOADER_APPSIZE / sectorsizeDATAFLASH()),
			(unsigned) (sectorsizeDATAFLASH() / 1024)
			);

		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_string_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, b);
		StringDescrTbl [id].size = partlen;
		StringDescrTbl [id].data = alldescbuffer + score;
		score += partlen;
	}
#endif /* defined (BOOTLOADER_APPSIZE) */
#if BOOTLOADER_SELFSIZE
	{
		// Re-write bootloader parameters
		static const char strFlashDesc_4 [] = "@SPI Flash BOOTLOADER: %s/0x%08lx/%02u*%03uKg";
		unsigned partlen;
		const uint_fast8_t id = STRING_ID_DFU_1;
		char b [128];
		const int status = testchipDATAFLASH();
		local_snprintf_P(b, ARRAY_SIZE(b), strFlashDesc_4,
			status ? USBD_DFU_FLASHNAME : nameDATAFLASH,
			(unsigned long) BOOTLOADER_SELFBASE,
			(unsigned) (BOOTLOADER_SELFSIZE / sectorsizeDATAFLASH()),
			(unsigned) (sectorsizeDATAFLASH() / 1024)
			);

		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_string_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, b);
		StringDescrTbl [id].size = partlen;
		StringDescrTbl [id].data = alldescbuffer + score;
		score += partlen;
	}
#endif /* BOOTLOADER_SELFSIZE */
#endif /* WIHSPIDFHW || WIHSPIDFSW */
#if WITHISBOOTLOADER && defined (BOOTLOADER_RAMAREA) && BOOTLOADER_RAMSIZE
	{
		// RAM target for debug
		static const char strFlashDesc_3 [] = "@RAM APPLICATION/0x%08lx/%02u*%03uKg";
		unsigned partlen;
		const uint_fast8_t id = STRING_ID_DFU_2;
		char b [128];
		local_snprintf_P(b, ARRAY_SIZE(b), strFlashDesc_3,
			(unsigned long) BOOTLOADER_RAMAREA,
			(unsigned) (BOOTLOADER_RAMSIZE / BOOTLOADER_RAMPAGESIZE),
			(unsigned) (BOOTLOADER_RAMPAGESIZE / 1024)
			);
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_string_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, b);
		StringDescrTbl [id].size = partlen;
		StringDescrTbl [id].data = alldescbuffer + score;
		score += partlen;
	}
#endif /* WITHISBOOTLOADER && defined (BOOTLOADER_RAMAREA) && BOOTLOADER_RAMSIZE */
#endif /* WITHUSBDFU */
#if WITHUSBCDCECM
	{
		static const  uint8_t hw [6]  = { HWADDR };
		unsigned partlen;
		// Формирование MAC адреса данного устройства
		// TODO: При модификации не забыть про достоверность значений
		const uint_fast8_t id = STRING_ID_MACADDRESS;
		char b [64];
		local_snprintf_P(b, ARRAY_SIZE(b), PSTR("%02X%02X%02X%02X%02X%02X"), hw [0], hw [1], hw [2], hw [3], hw [4], hw [5]);
		//local_snprintf_P(b, ARRAY_SIZE(b), PSTR("0023543C471C"));
		// Unic serial number
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_string_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, b);
		StringDescrTbl [id].size = partlen;
		StringDescrTbl [id].data = alldescbuffer + score;
		score += partlen;
	}
#endif /* WITHUSBCDCECM */

#if CTLSTYLE_SW2011ALL || WITHUSBNOUNIQUE
	{
		// на трансиверае SW20xx минимальная привязка к уникальным особенностям трансивера
		unsigned partlen;
		const uint_fast8_t id = STRING_ID_sn;
		char b [64];
		local_snprintf_P(b, ARRAY_SIZE(b), PSTR("SN:19640302_%lu"), (unsigned long) BUILD_ID);
		// Unic serial number
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_string_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, b);
		StringDescrTbl [id].size = partlen;
		StringDescrTbl [id].data = alldescbuffer + score;
		score += partlen;
	}
#elif 0 //CPUSTYLE_STM32F && defined(UID_BASE)
	{
		unsigned partlen;
		const uint_fast8_t id = STRING_ID_sn;
		const uint32_t * const base = (const uint32_t *) UID_BASE;
		char b [64];
		local_snprintf_P(b, ARRAY_SIZE(b), PSTR("SN:%08lX%08lX%08lX_%lu_%lu"), (unsigned long) base [0], (unsigned long) base [1], (unsigned long) base [2], (unsigned long) (REFERENCE_FREQ * DDS1_CLK_MUL), (unsigned long) BUILD_ID);
		// Unic serial number
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_string_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, b);
		StringDescrTbl [id].size = partlen;
		StringDescrTbl [id].data = alldescbuffer + score;
		score += partlen;
	}
#else
	{
		unsigned partlen;
		const uint_fast8_t id = STRING_ID_sn;
		char b [64];
		local_snprintf_P(b, ARRAY_SIZE(b), PSTR("SN:19640302_%lu_%lu"), (unsigned long) (REFERENCE_FREQ * DDS1_CLK_MUL), (unsigned long) BUILD_ID);
		// Unic serial number
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_string_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, b);
		StringDescrTbl [id].size = partlen;
		StringDescrTbl [id].data = alldescbuffer + score;
		score += partlen;
	}
#endif /* CPUSTYLE_STM32F && defined(UID_BASE) */

	dcache_clean_invalidate((uintptr_t) alldescbuffer, sizeof alldescbuffer);
	//PRINTF(PSTR("usbd_descriptors_initialize: total length=%u at %p\n"), score, alldescbuffer);
}

/* совместимость VID/PID для работы с программой FT8CN */
void board_set_usb_ft8cn(uint_fast8_t v)
{
	uint_fast8_t n = v != 0;
	if (usb_ft8cn != n)
	{
		usb_ft8cn = n;
		IRQL_t oldIrql;
		RiseIrql(IRQL_SYSTEM, & oldIrql);
	#if WITHUSBDEV_HSDESC
		usbd_descriptors_initialize(1);

	#else /* WITHUSBDEV_HSDESC */
		usbd_descriptors_initialize(0);

	#endif /* WITHUSBDEV_HSDESC */
		LowerIrql(oldIrql);
	}
}

#endif /* WITHUSBHW */
