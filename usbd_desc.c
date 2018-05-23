/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "pio.h"
#include "board.h"
#include "audio.h"

#include "display.h"
#include "formats.h"
#include <string.h>
#include <wchar.h>

#if WITHUSBHW

#include "usb200.h"
#include "usbch9.h"

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

#define USB_FUNCTION_BCD_USB	0x0200	// 0x0201 in ST samples
#define USB_FUNCTION_VENDOR_ID	0xFFFF	// Generic
//#define USB_FUNCTION_VENDOR_ID	0x041C	// Altera Corp.
//#define USB_FUNCTION_VENDOR_ID	0x04d9	// Holtek Semiconductor, Inc.
#define USB_FUNCTION_PRODUCT_ID	0x0736

// From STMicroelectronics Comunication Device Class driver (CDC) INF FILE:
//#define USB_FUNCTION_VENDOR_ID	0x0483	// STM
//#define USB_FUNCTION_PRODUCT_ID	0x5740
//#define USB_FUNCTION_RELEASE_NO	0x0200

#if WITHUSBUAC && WITHUSBUAC3
	#if WITHRTS96
		#define BUILD_ID 6	// модификатор serial sumber
		#define USB_FUNCTION_RELEASE_NO	0x0106
	#elif WITHRTS192
		#define BUILD_ID 5	// модификатор serial sumber
		#define USB_FUNCTION_RELEASE_NO	0x0105
	#else
		#define BUILD_ID 4	// модификатор serial sumber
		#define USB_FUNCTION_RELEASE_NO	0x0104
	#endif
#else /* WITHUSBUAC && WITHUSBUAC3 */
	#if WITHRTSNOAUDIO
		#define BUILD_ID 3	// модификатор serial sumber
		#define USB_FUNCTION_RELEASE_NO	0x0103
	#elif WITHRTS96
		#define BUILD_ID 2	// модификатор serial sumber
		#define USB_FUNCTION_RELEASE_NO	0x0102
	#elif WITHRTS192
		#define BUILD_ID 1	// модификатор serial sumber
		#define USB_FUNCTION_RELEASE_NO	0x0101
	#else
		#define BUILD_ID 0	// модификатор serial sumber
		#define USB_FUNCTION_RELEASE_NO	0x0100
	#endif
#endif /* WITHUSBUAC && WITHUSBUAC3 */

enum
{
	STRING_ID_0 = 0, /* Language ID */

	STRING_ID_1, /* Manufacturer */
	STRING_ID_2, /* Product */
	STRING_ID_3, /* SerialNumber */

	// USB CDC strings
	STRING_ID_4a, /*  */
	STRING_ID_4b, /*  */

	STRING_ID_5,
	STRING_ID_5a,
	STRING_ID_MACADDRESS,	// iMacAddress

	// USB UAC strings
	STRING_ID_a0, /* tag for Interface Descriptor 0/0 Audio */
	STRING_ID_a1, /* tag for Interface Descriptor 0/0 Spectrum */

	STRING_ID_d0,
	STRING_ID_d1,

	STRING_ID_e0,
	STRING_ID_e1,

	STRING_ID_x0, /*  */
	STRING_ID_x1, /*  */

	STRING_ID_y0, /*  */
	STRING_ID_y1, /*  */

	STRING_ID_z0, /*  */
	STRING_ID_z1, /*  */

	//STRING_ID_b,	// tag for USB Speaker Audio Feature Unit Descriptor

	STRING_ID_Left, STRING_ID_Right,	// Идут подряд

	STRING_ID_RNDIS,
	STRING_ID_HIDa,
	STRING_ID_IQSPECTRUM,
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
	{ STRING_ID_1, "MicroGenSF", },		// Manufacturer
	{ STRING_ID_2, "Storch TRX", },	// Product
	{ STRING_ID_4a, "Storch TRX CAT", },
	{ STRING_ID_4b, "Storch TRX CTL", },
	{ STRING_ID_5, "Storch TRX CDC EEM", },
	{ STRING_ID_5a, "Storch TRX CDC ECM", },
	{ STRING_ID_RNDIS, "Storch TRX Remote NDIS", },

	{ STRING_ID_a0, "Storch TRX Voice", },	// tag for Interface Descriptor 0/0 Audio
	{ STRING_ID_a1, "Storch TRX Spectrum", },	// tag for Interface Descriptor 0/0 Audio

	//{ STRING_ID_b, "xxx_id11", },	// tag for USB Speaker Audio Feature Unit Descriptor

	{ STRING_ID_d0, "Transmitter Input1", },	// Audio Control Input Terminal Descriptor 
	{ STRING_ID_d1, "Transmitter Input2", },	// Audio Control Input Terminal Descriptor 

	{ STRING_ID_e0, "Receiver Output 1", },	// Audio Control Output Terminal Descriptor 
	{ STRING_ID_e1, "Receiver Output 2", },	// Audio Control Output Terminal Descriptor 

	{ STRING_ID_x0, "xxxx In 1", },	// Audio Control Output Terminal Descriptor 
	{ STRING_ID_x1, "xxxx In 2", },	// Audio Control Output Terminal Descriptor 

	{ STRING_ID_y0, "yyyy In 1", },	// Audio Control Output Terminal Descriptor 
	{ STRING_ID_y1, "yyyy In 2", },	// Audio Control Output Terminal Descriptor 

	{ STRING_ID_z0, "zzzz In 1", },	// Audio Control Output Terminal Descriptor 
	{ STRING_ID_z1, "zzzz In 2", },	// Audio Control Output Terminal Descriptor 

	{ STRING_ID_Left, "USB", },	// tag for USB Speaker Audio Feature Unit Descriptor
	{ STRING_ID_Right, "LSB", },	// tag for USB Speaker Audio Feature Unit Descriptor
	{ STRING_ID_HIDa, "HID xxx", },
	{ STRING_ID_IQSPECTRUM, "RX IQ Output", },
};
#if 0

static int
toprintc(int c)
{
	if (c < 0x20 || c >= 0x7f)
		return '.';
	return c;
}

void
static printhex(unsigned long voffs, const unsigned char * buff, unsigned length)
{
	unsigned i, j;
	unsigned rows = (length + 15) / 16;

	for (i = 0; i < rows; ++ i)
	{
		int trl = 16;
		//const int trl = ((length - 1) - i * 16) % 16 + 1;	// количество символов в данной строке

		debug_printf_P(PSTR("%08lX "), voffs + i * 16);
		for (j = 0; j < trl; ++ j)
			debug_printf_P(PSTR(" %02X"), buff [i * 16 + j]);

		debug_printf_P(PSTR("%*s"), (16 - trl) * 3, "");

		debug_printf_P(PSTR("  "));
		for (j = 0; j < trl; ++ j)
			debug_printf_P(PSTR("%c"), toprintc(buff [i * 16 + j]));

		debug_printf_P(PSTR("\n"));
	}
}
#endif


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

	if (size <= 1024)
		return size;	// 1..1024
	if (size <= 2048)
		return (0x01 << 11) | ((size + 1) / 2);	// 513..1024
	else
		return (0x02 << 11) | ((size + 2) / 3);	// 683..1024
}

#if WITHUSBUAC

#if 0
	// Вариант Oleg UR3IQO
	static const uint_fast8_t USBD_UACIN_EP_ATTRIBUTES = 
		USB_ENDPOINT_USAGE_DATA | 
		USB_ENDPOINT_SYNC_SYNCHRONOUS | 
		USB_ENDPOINT_TYPE_ISOCHRONOUS;

	static const uint_fast8_t USBD_UACOUT_EP_ATTRIBUTES = 
		USB_ENDPOINT_USAGE_DATA | 
		USB_ENDPOINT_SYNC_SYNCHRONOUS | 
		USB_ENDPOINT_TYPE_ISOCHRONOUS;
#else
	// Мой вариант
	static const uint_fast8_t USBD_UACIN_EP_ATTRIBUTES = 
		USB_ENDPOINT_USAGE_IMPLICIT_FEEDBACK | 
		USB_ENDPOINT_SYNC_ASYNCHRONOUS | 
		USB_ENDPOINT_TYPE_ISOCHRONOUS;

	static const uint_fast8_t USBD_UACOUT_EP_ATTRIBUTES = 
		USB_ENDPOINT_USAGE_DATA | 
		USB_ENDPOINT_SYNC_ASYNCHRONOUS | 
		USB_ENDPOINT_TYPE_ISOCHRONOUS;
#endif


#if 0

//In the following code bmAttributes field is 0x01; 
//which means that clock type is internal fixed clock.
/* Clock Source Descriptor(4.7.2.1) */ 
static unsigned r9fill_clock_source(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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
		* buff ++ = 0x10;       /* bClockID(0x10): CLOCK_SOURCE_ID */ 
		* buff ++ = 0x01;       /* bmAttributes(0x01): internal fixed clock */ 
		* buff ++ = 0x07;       /* bmControls(0x07):                    
								clock frequency control: 0b11 - host programmable;                    
								clock validity control: 0b01 - host read only */ 
		* buff ++ = TERMINAL_ID_UNDEFINED;       /* bAssocTerminal(0x00) */ 
		* buff ++ = 0x01;       /* iClockSource(0x01): Not requested */
	}
	return length;
}

#endif

/* UAC IAD */
// Interface Association Descriptor Audio
// Audio10.pdf 4.3.2.8 Associated Interface Descriptor
// documented in USB ECN : Interface Association Descriptor - InterfaceAssociationDescriptor_ecn.pdf
static unsigned UAC_InterfaceAssociationDescriptor(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, 
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
		* buff ++ = STRING_ID_a0 + offset;	// Interface string index
	}
	return length;
}

/* USB Speaker Standard interface descriptor */
// Interface Descriptor 0/0 Audio, 0 Endpoints
static unsigned r9fill_3(uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
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
		* buff ++ = STRING_ID_a0 + offset;               /* iInterface */
		/* 09 byte*/
	}
	return length;
}

// Audio Control Input Terminal Descriptor 
// audio10.pdf: Table 4-3: Input Terminal Descriptor
// Sereo signal source
// Audio или RTS
static unsigned UAC_AudioControlIT_IN(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bTerminalID, uint_fast8_t offset)
{
	const uint_fast8_t length = 12;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 4.3.2.1 Input Terminal Descriptor 
		const uint_fast16_t wTerminalType = AUDIO_TERMINAL_RADIO_RECEIVER;
		const uint_fast8_t bNrChannels = HARDWARE_USBD_AUDIO_IN_CHANNELS;
		const uint_fast16_t wChannelConfig = bNrChannels == 1 ? 
			AUDIO_CHANNEL_M : // Mono
			(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R);	// Left Front & Right Front
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE; // CS_INTERFACE Descriptor Type
		* buff ++ = AUDIO_CONTROL_INPUT_TERMINAL;    // INPUT_TERMINAL 0x02 descriptor subtype
		* buff ++ = bTerminalID;                   // bTerminalID ID of this Terminal.
		* buff ++ = LO_BYTE(wTerminalType);			/* wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;        // bAssocTerminal No association
		// The bNrChannels, wChannelConfig and iChannelNames fields together constitute the cluster descriptor
		* buff ++ = bNrChannels;    /* bNrChannels */
		* buff ++ = LO_BYTE(wChannelConfig);   /* bmChannelConfig size = 4 bytes Mono sets no position bits */
		* buff ++ = HI_BYTE(wChannelConfig);
		* buff ++ = STRING_ID_Left;							/* iChannelNames */
		* buff ++ = 0;						// iTerminal - Index of a string descriptor, describing the Input Terminal. Receiver Output

	}
	return length;
}

static unsigned UAC_AudioControlIT_INRTS(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bTerminalID, uint_fast8_t offset)
{
	const uint_fast8_t length = 12;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		// 4.3.2.1 Input Terminal Descriptor 
		const uint_fast16_t wTerminalType = AUDIO_TERMINAL_RADIO_RECEIVER;
		const uint_fast8_t bNrChannels = HARDWARE_USBD_AUDIO_IN_CHANNELS_RTS; // для канала со спектром всегда стерео. но это не тут указано
		const uint_fast16_t wChannelConfig = bNrChannels == 1 ? 
			AUDIO_CHANNEL_M : // Mono
			(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R);	// Left Front & Right Front
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE; // CS_INTERFACE Descriptor Type
		* buff ++ = AUDIO_CONTROL_INPUT_TERMINAL;    // INPUT_TERMINAL 0x02 descriptor subtype
		* buff ++ = bTerminalID;                   // bTerminalID ID of this Terminal.
		* buff ++ = LO_BYTE(wTerminalType);			/* wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;        // bAssocTerminal No association
		// The bNrChannels, wChannelConfig and iChannelNames fields together constitute the cluster descriptor
		* buff ++ = bNrChannels;    /* bNrChannels */
		* buff ++ = LO_BYTE(wChannelConfig);   /* bmChannelConfig size = 4 bytes Mono sets no position bits */
		* buff ++ = HI_BYTE(wChannelConfig);
		* buff ++ = 0;							/* iChannelNames */
		* buff ++ = STRING_ID_IQSPECTRUM;		// iTerminal - Index of a string descriptor, describing the Input Terminal. Receiver Output

	}
	return length;
}

/* !USB Speaker Input Terminal Descriptor */
// Audio Control Input Terminal Descriptor 
// audio10.pdf: Table 4-3: Input Terminal Descriptor
// audio48 only
static unsigned UAC_AudioControlIT_OUT(
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
		const uint_fast8_t bNrChannels = HARDWARE_USBD_AUDIO_OUT_CHANNELS;
		const uint_fast16_t wChannelConfig = bNrChannels == 1 ? 
			AUDIO_CHANNEL_M : // Mono
			(AUDIO_CHANNEL_L | AUDIO_CHANNEL_R);	// Left Front & Right Front

		* buff ++ = length;									/* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;		/* bDescriptorType */
		* buff ++ = AUDIO_CONTROL_INPUT_TERMINAL;			/* bDescriptorSubtype */
		* buff ++ = bTerminalID;							/* bTerminalID */
		* buff ++ = LO_BYTE(wTerminalType);					/* wTerminalType */
		* buff ++ = HI_BYTE(wTerminalType);
		* buff ++ = TERMINAL_ID_UNDEFINED;					/* bAssocTerminal */
		// The bNrChannels, wChannelConfig and iChannelNames fields together constitute the cluster descriptor
		* buff ++ = bNrChannels;							/* bNrChannels */
		* buff ++ = LO_BYTE(wChannelConfig);                /* wChannelConfig 0x0003  Front Left; Front Right */
		* buff ++ = HI_BYTE(wChannelConfig);
		* buff ++ = STRING_ID_Left;							/* iChannelNames */
		* buff ++ = STRING_ID_d0 + offset;					/* iTerminal - появляется как pop-up в панели управления ASIO4ALL */
		/* 12 bytes*/
	}
	return length;
}

/*! USB Microphone Output Terminal Descriptor bSourceID -> bTerminalID */
// Audio Control Output Terminal Descriptor 
// Audio или RTS
static unsigned UAC_AudioControlOT_IN(
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
		* buff ++ = STRING_ID_e0 + offset;					// unused  (iTerminal)
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
// See 4.3.2.5 Feature Unit Descriptor for details
// В нашем случае используется для подавления отображения раздельных элементов регулировки уровня по каналам
static unsigned UAC_AudioFeatureUnit(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	uint_fast8_t bUnitID,
	uint_fast8_t bSourceID
	)
{
	// Параметр определяет, ккие управляющие элементы появляются на страничке управления "Custom"
	// Причем, на этой страничке собраны все управляющие элементы со всех Feature Unit Descriptor 
	// в пути сигнала данного источника звука.
	// Не может быть нулём.
	const uint_fast32_t bmaControls = 
		AUDIO_CONTROL_MUTE |
		AUDIO_CONTROL_VOLUME |
		//AUDIO_CONTROL_AUTOMATIC_GAIN |
		//AUDIO_CONTROL_GRAPHIC_EQUALIZER |
		//AUDIO_CONTROL_LOUDNESS |		// "Custom" property page added
		0;

	const uint_fast8_t n = 1; // 1: Only master channel controls, 3: master, left and right
	const uint_fast8_t bControlSize = 2;	/* Достаточно, чтобы вместить все определенные для bmaControls биты */
	const uint_fast8_t length = 7 + bControlSize * n;
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
		for (i = 0; i < n; ++ i)
		{
			uint_fast32_t v = bmaControls;
			uint_fast8_t cs = bControlSize;
			while (cs --)
			{
				* buff ++ = (uint8_t) v;
				v >>= 8;
			}
		}
		* buff ++ = 0;//STRING_ID_b;                    /* iTerminal */
		/* 10 byte*/
	}
	return length;
}

//
// Если выход AUDIO_TERMINAL_RADIO_TRANSMITTER, закладки enchancements нет
// Audio Control Output Terminal Descriptor 
// bSourceID -> bTerminalID
// audio48 only
static unsigned UAC_AudioControlOT_OUT(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize, 
	uint_fast8_t bTerminalID,
	uint_fast8_t bSourceID
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
		* buff ++ = 0;                    /* 8 iTerminal*/
		/* 9 byte*/
	}
	return length;
}

#define WITHUSENOFU 0	// 1 - без использования Feature Unit, 0 - с использованием, игнорирование управления громкостью

// Заполнение схемы ввода звука
// IN data flow
static unsigned UAC_AudioControlIfCircuitIN(uint_fast8_t fill, uint8_t * p, unsigned maxsize, uint_fast8_t offset)
{
	unsigned n = 0;

	if (WITHUSENOFU)
	{
		// Только один источник для компьютера
		n += UAC_AudioControlIT_IN(fill, p + n, maxsize - n, TERMINAL_ID_IT_2 + offset, offset);	/* AUDIO_TERMINAL_RADIO_RECEIVER */
		n += UAC_AudioControlOT_IN(fill, p + n, maxsize - n,  TERMINAL_ID_OT_4 + offset, TERMINAL_ID_IT_2 + offset, offset);	/* AUDIO_TERMINAL_USB_STREAMING Terminal Descriptor TERMINAL_ID_IT_2 -> TERMINAL_ID_OT_4 */
	}
	else
	{
		// Только один источник для компьютера
		n += UAC_AudioControlIT_IN(fill, p + n, maxsize - n, TERMINAL_ID_IT_2 + offset, offset);	/* AUDIO_TERMINAL_RADIO_RECEIVER */
		n += UAC_AudioFeatureUnit(fill, p + n, maxsize - n, TERMINAL_ID_FU_AUDIO + offset, TERMINAL_ID_IT_2 + offset);	/* USB microphone Audio Feature Unit Descriptor TERMINAL_ID_IT_1 -> TERMINAL_ID_FU_AUDIO */
		n += UAC_AudioControlOT_IN(fill, p + n, maxsize - n,  TERMINAL_ID_OT_4 + offset, TERMINAL_ID_FU_AUDIO + offset, offset);	/* AUDIO_TERMINAL_USB_STREAMING Terminal Descriptor TERMINAL_ID_FU_AUDIO -> TERMINAL_ID_OT_4 */
	}

	return n;
}

// Заполнение схемы вывода звука
// OUT data flow
// audio48 only
static unsigned UAC_AudioControlIfCircuitOUT(uint_fast8_t fill, uint8_t * p, unsigned maxsize, uint_fast8_t offset)
{
	unsigned n = 0;

	if (WITHUSENOFU)
	{
		// без feature unit между IT и OT
		n += UAC_AudioControlIT_OUT(fill, p + n, maxsize - n, TERMINAL_ID_IT_1 + offset, offset);	/* AUDIO_TERMINAL_USB_STREAMING Input Terminal Descriptor TERMINAL_ID_IT_1 + offset */
		n += UAC_AudioControlOT_OUT(fill, p + n, maxsize - n, TERMINAL_ID_OT_3 + offset, TERMINAL_ID_IT_1 + offset);	/* AUDIO_TERMINAL_RADIO_TRANSMITTER Output Terminal Descriptor TERMINAL_ID_IT_1 + offset -> TERMINAL_ID_OT_3 + offset */
	}
	else
	{
		n += UAC_AudioControlIT_OUT(fill, p + n, maxsize - n, TERMINAL_ID_IT_1 + offset, offset);	/* AUDIO_TERMINAL_USB_STREAMING Input Terminal Descriptor TERMINAL_ID_IT_1 */
		n += UAC_AudioFeatureUnit(fill, p + n, maxsize - n, TERMINAL_ID_FU_5 + offset, TERMINAL_ID_IT_1 + offset);	/* USB Speaker Audio Feature Unit Descriptor TERMINAL_ID_IT_1 -> TERMINAL_ID_FU_5 */
		n += UAC_AudioControlOT_OUT(fill, p + n, maxsize - n, TERMINAL_ID_OT_3 + offset, TERMINAL_ID_FU_5 + offset);	/* AUDIO_TERMINAL_RADIO_TRANSMITTER Output Terminal Descriptor TERMINAL_ID_FU_5 -> TERMINAL_ID_OT_3 */
	}

	return n;
}

static unsigned UAC_AudioControlIfCircuitINRTS(uint_fast8_t fill, uint8_t * p, unsigned maxsize, uint_fast8_t offset)
{
	unsigned n = 0;

	if (WITHUSENOFU)
	{
		// Только один источник для компьютера
		n += UAC_AudioControlIT_INRTS(fill, p + n, maxsize - n, TERMINAL_ID_ITRTS_2 + offset, offset);	/* AUDIO_TERMINAL_RADIO_RECEIVER */
		n += UAC_AudioControlOT_IN(fill, p + n, maxsize - n,  TERMINAL_ID_OTRTS_4 + offset, TERMINAL_ID_ITRTS_2 + offset, offset);	/* AUDIO_TERMINAL_USB_STREAMING Terminal Descriptor TERMINAL_ID_IT_2 -> TERMINAL_ID_OT_4 */
	}
	else
	{
		n += UAC_AudioControlIT_INRTS(fill, p + n, maxsize - n, TERMINAL_ID_ITRTS_2 + offset, offset);	/* AUDIO_TERMINAL_RADIO_RECEIVER */
		n += UAC_AudioFeatureUnit(fill, p + n, maxsize - n, TERMINAL_ID_FU_RTS + offset, TERMINAL_ID_ITRTS_2 + offset);	/* USB microphone Audio Feature Unit Descriptor TERMINAL_ID_IT_1 -> TERMINAL_ID_FU_RTS */
		n += UAC_AudioControlOT_IN(fill, p + n, maxsize - n,  TERMINAL_ID_OTRTS_4 + offset, TERMINAL_ID_FU_RTS + offset, offset);	/* AUDIO_TERMINAL_USB_STREAMING Terminal Descriptor TERMINAL_ID_IT_2 -> TERMINAL_ID_OT_4 */
	}

	return n;
}

static unsigned UAC_AudioControlIfCircuits(uint_fast8_t fill, uint8_t * p, unsigned maxsize, uint_fast8_t offset)
{
	unsigned n = 0;

	n += UAC_AudioControlIfCircuitIN(fill, p + n, maxsize - n, offset);	// Заполнение схемы ввода звука
	n += UAC_AudioControlIfCircuitOUT(fill, p + n, maxsize - n, offset);	// Заполнение схемы вывода звука

	return n;
}


/* USB Speaker Class-specific AC Interface Descriptor */
// Audio Control Interface Header Descriptor 
static unsigned UACINOUT_AudioControlIfHeader(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize,
	const uint_fast8_t * coll,
	uint_fast8_t bInCollection,
	unsigned (* audiocontrolifcircuits)(uint_fast8_t fill, uint8_t * p, unsigned maxsize, uint_fast8_t offset),
	uint_fast8_t offset
	)
{
	const uint_fast8_t length = 8 + bInCollection;
	const unsigned wTotalLength = length + audiocontrolifcircuits(0, buff + length, maxsize - length, offset);
	uint_fast8_t i;
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
		audiocontrolifcircuits(fill, buff, maxsize - length, offset);
	}
	return wTotalLength;

}

/* USB Speaker Standard AS Interface Descriptor - Audio Streaming Zero Bandwith */
/* Interface 1, Alternate Setting 0                                             */
// USBLyzer: Interface Descriptor 0/0 Audio, 0 Endpoints
static unsigned r9fill_10(
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
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;  /* bDescriptorType */
		* buff ++ = bInterfaceNumber; //;                 /* bInterfaceNumber */
		* buff ++ = bAlternateSetting;								/* bAlternateSetting  - zero-based index */
		* buff ++ = 0x00;                           /* bNumEndpoints */
		* buff ++ = USB_DEVICE_CLASS_AUDIO;         /* bInterfaceClass */
		* buff ++ = AUDIO_SUBCLASS_AUDIOSTREAMING;  /* bInterfaceSubClass */
		* buff ++ = AUDIO_PROTOCOL_UNDEFINED;       /* bInterfaceProtocol - unused */
		* buff ++ = STRING_ID_y0 + offset;          /* iInterface - unused */
		/* 9 byte*/
	}
	return length;
}

/* USB Speaker Standard AS Interface Descriptor - Audio Streaming Operational */
/* 48000 Hz sample rate, 2 channel, stereo - for stereo signal */
/* Interface 1, Alternate Setting 1                                           */
// USBLyzer: Interface Descriptor 1/1 Audio
// USBLyzer: Interface Descriptor 1/2 Audio
static unsigned r9fill_11(
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
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_INTERFACE_DESCRIPTOR_TYPE;  /* bDescriptorType */
		* buff ++ = bInterfaceNumber;	            /* bInterfaceNumber */
		* buff ++ = bAlternateSetting;				/* bAlternateSetting  - zero-based index */
		* buff ++ = 0x01;							/* bNumEndpoints */
		* buff ++ = USB_DEVICE_CLASS_AUDIO;         /* bInterfaceClass */
		* buff ++ = AUDIO_SUBCLASS_AUDIOSTREAMING;  /* bInterfaceSubClass */
		* buff ++ = AUDIO_PROTOCOL_UNDEFINED;       /* bInterfaceProtocol - unused */
		* buff ++ = STRING_ID_z0 + offset;                    /* iInterface - unused */
		/* 9 byte*/
	}
	return length;
}

/* USB Speaker Audio Type I Format Interface Descriptor */
// USBLyzer: Audio Streaming Format Type Descriptor 
static unsigned r9fill_13(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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
		* buff ++ = HARDWARE_USBD_AUDIO_OUT_CHANNELS;		/* bNrChannels */
		* buff ++ = (HARDWARE_USBD_AUDIO_OUT_SAMPLEBITS + 7) / 8; /* bSubFrameSize :  2 Bytes per frame (16bits) */
		* buff ++ = HARDWARE_USBD_AUDIO_OUT_SAMPLEBITS;		/* bBitResolution (16-bits per sample) */
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
static unsigned r9fill_14(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress, int highspeed)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(VIRTUAL_AUDIO_PORT_DATA_SIZE_OUT);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;         /* bDescriptorType */
		* buff ++ = bEndpointAddress;               /* bEndpointAddress 1 out endpoint*/
		* buff ++ = USBD_UACOUT_EP_ATTRIBUTES;       						    /* bmAttributes */
		* buff ++ = LO_BYTE(wMaxPacketSize);              /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = highspeed ? HSINTERVAL_AUDIO48 : FSINTERVAL_AUDIO48;    /* bInterval */
		* buff ++ = 0x00;                                 /* bRefresh */
		* buff ++ = 0;                       /* bSynchAddress */
		/* 9 byte*/
	}
	return length;
}

/* Endpoint - Audio Streaming Descriptor*/
static unsigned r9fill_15(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 7;
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
		* buff ++ = 0x00;                           /* bmAttributes */
		* buff ++ = 0x00;                           /* bLockDelayUnits */
		* buff ++ = LO_BYTE(wLockDelay);			/* wLockDelay */
		* buff ++ = HI_BYTE(wLockDelay);
		/* 07 byte*/
	}
	return length;
}

// Interface Descriptor 2/1 Audio, 1 Endpoint
static unsigned r9fill_24(
	uint_fast8_t fill, uint8_t * buff, unsigned maxsize, 
	uint_fast8_t bInterfaceNumber, 
	uint_fast8_t bAlternateSetting, 
	uint_fast8_t bNumEndpoints, uint_fast8_t offset)
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
		* buff ++ = STRING_ID_x0 + offset;					/* Unused iInterface */
		/* 9 byte*/
	}
	return length;
}

#if ! WITHRTSNOAUDIO
/* USB Microphone Type I Format Type Descriptor (CODE == 6)*/
// Audio Streaming Format Type Descriptor 
static unsigned UAC_r9fill_26_audio48(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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
		* buff ++ = HARDWARE_USBD_AUDIO_IN_CHANNELS_AUDIO48;		/* bNrChannels */
		* buff ++ = (HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_AUDIO48 + 7) / 8; /* bSubFrameSize :  2 Bytes per frame (16bits) */
		* buff ++ = HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_AUDIO48;		/* bBitResolution (16-bits per sample) */
		* buff ++ = 1;										/* bSamFreqType only one frequency supported */
		* buff ++ = LO_BYTE(samplefreq1);	/* Audio sampling frequency coded on 3 bytes */
		* buff ++ = HI_BYTE(samplefreq1);
		* buff ++ = HI_24BY(samplefreq1);
	}
	return length;
}

/* USB Microphone Standard Endpoint Descriptor (CODE == 8)*/ //Standard AS Isochronous Audio Data Endpoint Descriptor
// Endpoint Descriptor 82 2 In, Isochronous, 125 us
static unsigned UAC_r9fill_27_audio48(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress, uint8_t offset)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_AUDIO48); // was: 0x300
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	// bDescriptorType
		* buff ++ = bEndpointAddress;                    // bEndpointAddress
		* buff ++ = USBD_UACIN_EP_ATTRIBUTES; // bmAttributes
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = highspeed ? HSINTERVAL_AUDIO48 : FSINTERVAL_AUDIO48;    /* bInterval */
		* buff ++ = 0x00;                       // Unused. (bRefresh)
		* buff ++ = 0x00;                       // Unused. (bSynchAddress)
	}
	return length;
}
#endif /* ! WITHRTSNOAUDIO */

#if WITHRTS96

/* USB Microphone Type I Format Type Descriptor (CODE == 6)*/
// Audio Streaming Format Type Descriptor 
static unsigned UAC_r9fill_26_rts96(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 11;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast32_t samplefreq1 = dsp_get_samplerateuacin_rts96();
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;// CS_INTERFACE Descriptor Type (bDescriptorType) 0x24
		* buff ++ = AUDIO_STREAMING_FORMAT_TYPE;   // FORMAT_TYPE subtype. (bDescriptorSubtype) 0x02
		* buff ++ = AUDIO_FORMAT_TYPE_I;							/* bFormatType */
		* buff ++ = HARDWARE_USBD_AUDIO_IN_CHANNELS_RTS;		/* bNrChannels */
		* buff ++ = (HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_RTS96 + 7) / 8; /* bSubFrameSize :  2 Bytes per frame (16bits) */
		* buff ++ = HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_RTS96;		/* bBitResolution (16-bits per sample) */
		* buff ++ = 1;										/* bSamFreqType only one frequency supported */
		* buff ++ = LO_BYTE(samplefreq1);	/* Audio sampling frequency coded on 3 bytes */
		* buff ++ = HI_BYTE(samplefreq1);
		* buff ++ = HI_24BY(samplefreq1);
	}
	return length;
}

/* USB Microphone Standard Endpoint Descriptor (CODE == 8)*/ //Standard AS Isochronous Audio Data Endpoint Descriptor
// Endpoint Descriptor 82 2 In, Isochronous, 125 us
static unsigned UAC_r9fill_27_rts96(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress, uint8_t offset)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS96); // was: 0x300
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	// bDescriptorType
		* buff ++ = bEndpointAddress;                    // bEndpointAddress
		* buff ++ = USBD_UACIN_EP_ATTRIBUTES; // bmAttributes
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
static unsigned UAC_r9fill_26_rts192(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
{
	const uint_fast8_t length = 11;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast32_t samplefreq1 = dsp_get_samplerateuacin_rts192();
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;						  /* bLength */
		* buff ++ = AUDIO_INTERFACE_DESCRIPTOR_TYPE;// CS_INTERFACE Descriptor Type (bDescriptorType) 0x24
		* buff ++ = AUDIO_STREAMING_FORMAT_TYPE;   // FORMAT_TYPE subtype. (bDescriptorSubtype) 0x02
		* buff ++ = AUDIO_FORMAT_TYPE_I;							/* bFormatType */
		* buff ++ = HARDWARE_USBD_AUDIO_IN_CHANNELS_RTS;		/* bNrChannels */
		* buff ++ = (HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_RTS192 + 7) / 8; /* bSubFrameSize :  2 Bytes per frame (16bits) */
		* buff ++ = HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_RTS192;		/* bBitResolution (16-bits per sample) */
		* buff ++ = 1;										/* bSamFreqType only one frequency supported */
		* buff ++ = LO_BYTE(samplefreq1);	/* Audio sampling frequency coded on 3 bytes */
		* buff ++ = HI_BYTE(samplefreq1);
		* buff ++ = HI_24BY(samplefreq1);
	}
	return length;
}

/* USB Microphone Standard Endpoint Descriptor (CODE == 8)*/ //Standard AS Isochronous Audio Data Endpoint Descriptor
// Endpoint Descriptor 82 2 In, Isochronous, 125 us
static unsigned UAC_r9fill_27_rts192(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress, uint8_t offset)
{
	const uint_fast8_t length = 9;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS192); // was: 0x300
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE;	// bDescriptorType
		* buff ++ = bEndpointAddress;                    // bEndpointAddress
		* buff ++ = USBD_UACIN_EP_ATTRIBUTES; // bmAttributes
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = highspeed ? HSINTERVAL_RTS192 : FSINTERVAL_RTS192;    /* bInterval */
		* buff ++ = 0x00;                       // Unused. (bRefresh)
		* buff ++ = 0x00;                       // Unused. (bSynchAddress)
	}
	return length;
}

#endif /* WITHRTS192 */

/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
static unsigned r9fill_28(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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
static unsigned UAC_AudioStreamingIf(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bTerminalLink)
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

#if WITHUSBUAC3

// UNUSED
static unsigned UAC_AudioControlIfCircuitsAUDIOandRTS(uint_fast8_t fill, uint8_t * p, unsigned maxsize, uint_fast8_t offset)
{
	unsigned n = 0;

	n += UAC_AudioControlIfCircuits(fill, p + n, maxsize - n, offset);	// Заполнение схемы ввода звука
	n += UAC_AudioControlIfCircuitINRTS(fill, p + n, maxsize - n, offset);	// Заполнение схемы ввода звука

	return n;
}

// AUDIO48 and RTS output audio function
// не используется - так как запуск HDSDR & ASIO4ALL сьивает акдиоканал
static unsigned fill_UACINOUT48andRTS_function_UNUSED(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed, uint_fast8_t offset)
{
	unsigned n = 0;
	const uint_fast8_t controlifv = INTERFACE_AUDIO_CONTROL_0;
	const uint_fast8_t mikeifv = INTERFACE_AUDIO_MIKE_2;
	const uint_fast8_t modulatorifv = INTERFACE_AUDIO_SPK_1;
	const uint_fast8_t rtsifv = INTERFACE_AUDIO_RTS_3;
	const uint_fast8_t coll [] =
	{
		modulatorifv,
		mikeifv,
		rtsifv,
	};

	const uint_fast8_t epin = USB_ENDPOINT_IN(USBD_EP_AUDIO_IN);
	const uint_fast8_t epout = USB_ENDPOINT_OUT(USBD_EP_AUDIO_OUT);
	const uint_fast8_t epinrts = USB_ENDPOINT_IN(USBD_EP_RTS_IN);

	n += UAC_InterfaceAssociationDescriptor(fill, p + n, maxsize - n, controlifv, 4, offset);	/* INTERFACE_AUDIO_CONTROL_0 Interface Association Descriptor Audio */
	// INTERFACE_AUDIO_CONTROL_0 - audio conntrol interface
	n += r9fill_3(fill, p + n, maxsize - n, controlifv, 0x00, offset);	/* INTERFACE_AUDIO_CONTROL_0 - Interface Descriptor 0/0 Audio, 0 Endpoints */
	n += UACINOUT_AudioControlIfHeader(fill, p + n, maxsize - n, coll, sizeof coll / sizeof coll [0], UAC_AudioControlIfCircuitsAUDIOandRTS, offset);	/* bcdADC Audio Control Interface Header Descriptor */

	// OUT data flow: USB Speaker
	// INTERFACE_AUDIO_SPK_1 - audio streaming interface
	n += r9fill_10(fill, p + n, maxsize - n, modulatorifv, 0x00, offset);	/* INTERFACE_AUDIO_SPK_1 - Interface 1, Alternate Setting 0 */

	n += r9fill_11(fill, p + n, maxsize - n, modulatorifv, 0x01, offset);	/* INTERFACE_AUDIO_SPK_1 -  Interface 1, Alternate Setting 1 */
	n += UAC_AudioStreamingIf(fill, p + n, maxsize - n, TERMINAL_ID_IT_1 + offset);	/* USB Speaker Audio Streaming Interface Descriptor (for output TERMINAL_ID_IT_1 + offset) */
	n += r9fill_13(fill, p + n, maxsize - n);	/* USB Speaker Audio Type I Format Interface Descriptor (one sample rate) 48000 */
	n += r9fill_14(fill, p + n, maxsize - n, epout, highspeed);	/* Endpoint USBD_EP_AUDIO_OUT - Standard Descriptor */
	n += r9fill_15(fill, p + n, maxsize - n);	/* Endpoint - Audio Streaming Descriptor */
	// IN data flow: USB Microphone
	// INTERFACE_AUDIO_MIKE_2 - audio streaming interface
	n += r9fill_24(fill, p + n, maxsize - n, mikeifv, UACINALT_NONE, 0, offset);	/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3) */ //zero-bandwidth interface

	// IN data flow: radio RX audio data
	n += r9fill_24(fill, p + n, maxsize - n, mikeifv, UACINALT_AUDIO48, 1, offset);	/* INTERFACE_AUDIO_MIKE_2 Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC_AudioStreamingIf(fill, p + n, maxsize - n, TERMINAL_ID_OT_4 + offset);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_ID_OT_4) (CODE == 5) */
	n += UAC_r9fill_26_audio48(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC_r9fill_27_audio48(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += r9fill_28(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/

#if WITHRTS96 || WITHRTS192
	n += r9fill_24(fill, p + n, maxsize - n, rtsifv, UACINRTSALT_NONE, 0, offset);	/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3) */ //zero-bandwidth interface

#if WITHRTS96
	// IN data flow: radio RX specrum data
	n += r9fill_24(fill, p + n, maxsize - n, rtsifv, UACINRTSALT_RTS96, 1, offset);	/* INTERFACE_AUDIO_RTS_2 Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC_AudioStreamingIf(fill, p + n, maxsize - n, TERMINAL_ID_OTRTS_4 + offset);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_ID_OT_4) (CODE == 5) */
	n += UAC_r9fill_26_rts96(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC_r9fill_27_rts96(fill, p + n, maxsize - n, highspeed, epinrts, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += r9fill_28(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* WITHRTS96 */

#if WITHRTS192
	// IN data flow: radio RX specrum data
	n += r9fill_24(fill, p + n, maxsize - n, rtsifv, UACINRTSALT_RTS192, 1, offset);	/* INTERFACE_AUDIO_RTS_2 Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC_AudioStreamingIf(fill, p + n, maxsize - n, TERMINAL_ID_OTRTS_4 + offset);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_ID_OT_4) (CODE == 5) */
	n += UAC_r9fill_26_rts192(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC_r9fill_27_rts192(fill, p + n, maxsize - n, highspeed, epinrts, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += r9fill_28(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* WITHRTS192 */

#endif /* WITHRTS96 || WITHRTS192 */

	return n;
}

// AUDIO48 only output audio function
static unsigned fill_UACINOUT48_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed, uint_fast8_t offset)
{
	unsigned n = 0;
	const uint_fast8_t controlifv = INTERFACE_AUDIO_CONTROL_0;
	const uint_fast8_t mikeifv = INTERFACE_AUDIO_MIKE_2;
	const uint_fast8_t modulatorifv = INTERFACE_AUDIO_SPK_1;
	const uint_fast8_t coll [] =
	{
		modulatorifv,
		mikeifv,
	};

	const uint_fast8_t epin = USB_ENDPOINT_IN(USBD_EP_AUDIO_IN);
	const uint_fast8_t epout = USB_ENDPOINT_OUT(USBD_EP_AUDIO_OUT);

	n += UAC_InterfaceAssociationDescriptor(fill, p + n, maxsize - n, controlifv, 3, offset);	/* INTERFACE_AUDIO_CONTROL_0 Interface Association Descriptor Audio */
	// INTERFACE_AUDIO_CONTROL_0 - audio conntrol interface
	n += r9fill_3(fill, p + n, maxsize - n, controlifv, 0x00, offset);	/* INTERFACE_AUDIO_CONTROL_0 - Interface Descriptor 0/0 Audio, 0 Endpoints */
	n += UACINOUT_AudioControlIfHeader(fill, p + n, maxsize - n, coll, sizeof coll / sizeof coll [0], UAC_AudioControlIfCircuits, offset);	/* bcdADC Audio Control Interface Header Descriptor */

	// OUT data flow: USB Speaker
	// INTERFACE_AUDIO_SPK_1 - audio streaming interface
	n += r9fill_10(fill, p + n, maxsize - n, modulatorifv, 0x00, offset);	/* INTERFACE_AUDIO_SPK_1 - Interface 1, Alternate Setting 0 */

	n += r9fill_11(fill, p + n, maxsize - n, modulatorifv, 0x01, offset);	/* INTERFACE_AUDIO_SPK_1 -  Interface 1, Alternate Setting 1 */
	n += UAC_AudioStreamingIf(fill, p + n, maxsize - n, TERMINAL_ID_IT_1 + offset);	/* USB Speaker Audio Streaming Interface Descriptor (for output TERMINAL_ID_IT_1 + offset) */
	n += r9fill_13(fill, p + n, maxsize - n);	/* USB Speaker Audio Type I Format Interface Descriptor (one sample rate) 48000 */
	n += r9fill_14(fill, p + n, maxsize - n, epout, highspeed);	/* Endpoint USBD_EP_AUDIO_OUT - Standard Descriptor */
	n += r9fill_15(fill, p + n, maxsize - n);	/* Endpoint - Audio Streaming Descriptor */
	// IN data flow: USB Microphone
	// INTERFACE_AUDIO_MIKE_2 - audio streaming interface
	n += r9fill_24(fill, p + n, maxsize - n, mikeifv, UACINALT_NONE, 0, offset);	/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3) */ //zero-bandwidth interface

	// IN data flow: radio RX audio data
	n += r9fill_24(fill, p + n, maxsize - n, mikeifv, UACINALT_AUDIO48, 1, offset);	/* INTERFACE_AUDIO_MIKE_2 Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC_AudioStreamingIf(fill, p + n, maxsize - n, TERMINAL_ID_OT_4 + offset);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_ID_OT_4) (CODE == 5) */
	n += UAC_r9fill_26_audio48(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC_r9fill_27_audio48(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += r9fill_28(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - ???????????? ?????????????*/

	return n;
}

#if 1


static unsigned UAC_AudioControlIfCircuitsRTS(uint_fast8_t fill, uint8_t * p, unsigned maxsize, uint_fast8_t offset)
{
	unsigned n = 0;

	n += UAC_AudioControlIfCircuitINRTS(fill, p + n, maxsize - n, offset);	// Заполнение схемы ввода звука

	return n;
}

static unsigned fill_UACINRTS_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed, uint_fast8_t offset)
{
	unsigned n = 0;
	const uint_fast8_t rtscontrolifv = INTERFACE_AUDIO_CONTROL_1;
	const uint_fast8_t rtsifv = INTERFACE_AUDIO_RTS_3;

	const uint_fast8_t epinrts = USB_ENDPOINT_IN(USBD_EP_RTS_IN);

	n += UAC_InterfaceAssociationDescriptor(fill, p + n, maxsize - n, rtscontrolifv, 2, offset);	/* INTERFACE_AUDIO_CONTROL_0 Interface Association Descriptor Audio */

	// IN data flow: USB Microphone
	// INTERFACE_AUDIO_MIKE_2 - audio streaming interface
	n += r9fill_3(fill, p + n, maxsize - n, rtscontrolifv, 0x00, offset);	/* INTERFACE_AUDIO_CONTROL_2 - Interface Descriptor 0/0 Audio, 0 Endpoints */
	n += UACINOUT_AudioControlIfHeader(fill, p + n, maxsize - n, & rtsifv, 1, UAC_AudioControlIfCircuitsRTS, offset);	/* bcdADC Audio Control Interface Header Descriptor */
	n += r9fill_24(fill, p + n, maxsize - n, rtsifv, UACINRTSALT_NONE, 0, offset);	/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3) */ //zero-bandwidth interface

#if WITHRTS96
	// IN data flow: radio RX specrum data
	n += r9fill_24(fill, p + n, maxsize - n, rtsifv, UACINRTSALT_RTS96, 1, offset);	/* INTERFACE_AUDIO_RTS_2 Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC_AudioStreamingIf(fill, p + n, maxsize - n, TERMINAL_ID_OTRTS_4 + offset);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_ID_OT_4) (CODE == 5) */
	n += UAC_r9fill_26_rts96(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC_r9fill_27_rts96(fill, p + n, maxsize - n, highspeed, epinrts, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += r9fill_28(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* WITHRTS96 */

#if WITHRTS192
	// IN data flow: radio RX specrum data
	n += r9fill_24(fill, p + n, maxsize - n, rtsifv, UACINRTSALT_RTS192, 1, offset);	/* INTERFACE_AUDIO_RTS_2 Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC_AudioStreamingIf(fill, p + n, maxsize - n, TERMINAL_ID_OTRTS_4 + offset);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_ID_OT_4) (CODE == 5) */
	n += UAC_r9fill_26_rts192(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC_r9fill_27_rts192(fill, p + n, maxsize - n, highspeed, epinrts, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += r9fill_28(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* WITHRTS192 */
	return n;
}
#endif

#else /* WITHUSBUAC3 */

static unsigned fill_UACINOUT_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed, uint_fast8_t offset)
{
	unsigned n = 0;
	const uint_fast8_t controlifv = INTERFACE_AUDIO_CONTROL_0 + offset * INTERFACE_UAC_count;
	const uint_fast8_t mikeifv = INTERFACE_AUDIO_MIKE_2 + offset * INTERFACE_UAC_count;
	const uint_fast8_t modulatorifv = INTERFACE_AUDIO_SPK_1 + offset * INTERFACE_UAC_count;
	const uint_fast8_t coll [] =
	{
		modulatorifv,
		mikeifv,
	};
	const uint_fast8_t epin = USB_ENDPOINT_IN(USBD_EP_AUDIO_IN + offset);
	const uint_fast8_t epout = USB_ENDPOINT_OUT(USBD_EP_AUDIO_OUT + offset);

	n += UAC_InterfaceAssociationDescriptor(fill, p + n, maxsize - n, controlifv, 3, offset);	/* INTERFACE_AUDIO_CONTROL_0 Interface Association Descriptor Audio */
	// INTERFACE_AUDIO_CONTROL_0 - audio conntrol interface
	n += r9fill_3(fill, p + n, maxsize - n, controlifv, 0x00, offset);	/* INTERFACE_AUDIO_CONTROL_0 - Interface Descriptor 0/0 Audio, 0 Endpoints */
	n += UACINOUT_AudioControlIfHeader(fill, p + n, maxsize - n, coll, sizeof coll / sizeof coll [0], UAC_AudioControlIfCircuits, offset);	/* bcdADC Audio Control Interface Header Descriptor */

	// OUT data flow: USB Speaker
	// INTERFACE_AUDIO_SPK_1 - audio streaming interface
	n += r9fill_10(fill, p + n, maxsize - n, modulatorifv, 0x00, offset);	/* INTERFACE_AUDIO_SPK_1 - Interface 1, Alternate Setting 0 */

	n += r9fill_11(fill, p + n, maxsize - n, modulatorifv, 0x01, offset);	/* INTERFACE_AUDIO_SPK_1 -  Interface 1, Alternate Setting 1 */
	n += UAC_AudioStreamingIf(fill, p + n, maxsize - n, TERMINAL_ID_IT_1 + offset);	/* USB Speaker Audio Streaming Interface Descriptor (for output TERMINAL_ID_IT_1 + offset) */
	n += r9fill_13(fill, p + n, maxsize - n);	/* USB Speaker Audio Type I Format Interface Descriptor (one sample rate) 48000 */
	n += r9fill_14(fill, p + n, maxsize - n, epout, highspeed);	/* Endpoint USBD_EP_AUDIO_OUT - Standard Descriptor */
	n += r9fill_15(fill, p + n, maxsize - n);	/* Endpoint - Audio Streaming Descriptor */
	// IN data flow: USB Microphone
	// INTERFACE_AUDIO_MIKE_2 - audio streaming interface
	n += r9fill_24(fill, p + n, maxsize - n, mikeifv, UACINALT_NONE, 0, offset);	/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3) */ //zero-bandwidth interface

#if ! WITHRTSNOAUDIO
	// IN data flow: radio RX audio data
	n += r9fill_24(fill, p + n, maxsize - n, mikeifv, UACINALT_AUDIO48, 1, offset);	/* INTERFACE_AUDIO_MIKE_2 Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC_AudioStreamingIf(fill, p + n, maxsize - n, TERMINAL_ID_OT_4 + offset);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_ID_OT_4) (CODE == 5) */
	n += UAC_r9fill_26_audio48(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC_r9fill_27_audio48(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += r9fill_28(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* ! WITHRTSNOAUDIO */

#if WITHRTS96
	// IN data flow: radio RX specrum data
	n += r9fill_24(fill, p + n, maxsize - n, mikeifv, UACINALT_RTS96, 1, offset);	/* INTERFACE_AUDIO_MIKE_2 Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC_AudioStreamingIf(fill, p + n, maxsize - n, TERMINAL_ID_OT_4 + offset);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_ID_OT_4) (CODE == 5) */
	n += UAC_r9fill_26_rts96(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC_r9fill_27_rts96(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += r9fill_28(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* WITHRTS96 */

#if WITHRTS192
	// IN data flow: radio RX specrum data
	n += r9fill_24(fill, p + n, maxsize - n, mikeifv, UACINALT_RTS192, 1, offset);	/* INTERFACE_AUDIO_MIKE_2 Interface Descriptor 2/1 Audio, 1 Endpoint, bAlternateSetting=0x01 */
	n += UAC_AudioStreamingIf(fill, p + n, maxsize - n, TERMINAL_ID_OT_4 + offset);	/* USB Microphone Class-specific AS General Interface Descriptor (for output TERMINAL_ID_OT_4) (CODE == 5) */
	n += UAC_r9fill_26_rts192(fill, p + n, maxsize - n);		/* USB Microphone Type I Format Type Descriptor (CODE == 6) 48000 */
	n += UAC_r9fill_27_rts192(fill, p + n, maxsize - n, highspeed, epin, offset);	/* Endpoint Descriptor USBD_EP_AUDIO_IN In, Isochronous, 125 us */
	n += r9fill_28(fill, p + n, maxsize - n);	/* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor (CODE == 7) OK - подтверждено документацией*/
#endif /* WITHRTS192 */
	return n;
}
#endif /* WITHUSBUAC3 */

#endif /* WITHUSBUAC */

#if WITHUSBCDC
/* CDC IAD */

// ISBLyzer: Interface Association Descriptor Abstract Control Model
// documented in USB ECN : Interface Association Descriptor - InterfaceAssociationDescriptor_ecn.pdf
static unsigned CDCACM_InterfaceAssociationDescriptor_a(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t offset)
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
		* buff ++ = INTERFACE_CDC_CONTROL_3a + offset * INTERFACE_CDCACM_count;	// bFirstInterface
		* buff ++ = INTERFACE_CDCACM_count;	// bInterfaceCount
		* buff ++ = USB_DEVICE_CLASS_COMMUNICATIONS;	// bFunctionClass: CDC
		* buff ++ = CDC_ABSTRACT_CONTROL_MODEL;			// bFunctionSubClass
		* buff ++ = CDC_PROTOCOL_COMMON_AT_COMMANDS;	// bFunctionProtocol
		* buff ++ = STRING_ID_4a + offset;				// iFunction - Storch HF TRX CAT - появляется, если сделать не тот bFunctionSubClass
	}
	return length;
}

/*Interface Descriptor*/
// USBLyzer: Interface Descriptor 3/0 CDC Control, 1 Endpoint 
static unsigned CDCACM_InterfaceDescriptorControlIf_a(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t offset)
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
		* buff ++ = INTERFACE_CDC_CONTROL_3a + offset * INTERFACE_CDCACM_count;   /* bInterfaceNumber: Number of Interface */
		* buff ++ = 0;		/* bAlternateSetting: Alternate setting  - zero-based index */
		* buff ++ = 0x01;   /* bNumEndpoints: One endpoints used (interrupt type) */
		* buff ++ = CDC_COMMUNICATION_INTERFACE_CLASS;   /* bInterfaceClass: Communication Interface Class */
		* buff ++ = 0x02;   /* bInterfaceSubClass: Abstract Control Model */
		* buff ++ = 0x01;   /* bInterfaceProtocol: Common AT commands */
		* buff ++ = STRING_ID_0;   /* iInterface */
	}
	return length;
}

/* Call Managment Functional Descriptor */
// Call Management Functional Descriptor 
static unsigned CDCACM_r9fill_32_a(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t offset)
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
		* buff ++ = INTERFACE_CDC_DATA_4a + offset * INTERFACE_CDCACM_count;   /* bDataInterface: Zero based index of the interface in this configuration.(bInterfaceNum) */
	}
	return length;
}

/* Union Functional Descriptor */
// Union Functional Descriptor 
static unsigned CDC_UnionFunctionalDescriptor_a(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t offset)
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
		* buff ++ = INTERFACE_CDC_CONTROL_3a + offset * INTERFACE_CDCACM_count;	/* bMasterInterface: Communication class interface -  Zero based index of the interface in this configuration (bInterfaceNum) */
		* buff ++ = INTERFACE_CDC_DATA_4a + offset * INTERFACE_CDCACM_count;		/* bSlaveInterface0: Data Class Interface -  Zero based index of the interface in this configuration (bInterfaceNum) */
	}
	return length;
}

/* Data class interface descriptor*/
// USBLyzer: Interface Descriptor 4/0 CDC Data, 2 Endpoints
static unsigned CDC_InterfaceDescriptorDataIf_a(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t offset)
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
		* buff ++ = INTERFACE_CDC_DATA_4a + offset * INTERFACE_CDCACM_count;   /* bInterfaceNumber: Number of Interface */
		* buff ++ = 0;		/* bAlternateSetting: Alternate setting  - zero-based index  */
		* buff ++ = 0x02;   /* bNumEndpoints: Two endpoints used: data in and data out */
		* buff ++ = CDC_DATA_INTERFACE_CLASS;   /* bInterfaceClass: CDC */
		* buff ++ = 0x00;   /* bInterfaceSubClass: */
		* buff ++ = 0x00;   /* bInterfaceProtocol: */
		* buff ++ = STRING_ID_0;   /* iInterface: */
	}
	return length;
}

/* Header Functional Descriptor */
static unsigned r9fill_31(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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

/* ACM Functional Descriptor */
// Abstract Control Management Functional Descriptor
static unsigned r9fill_33(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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

/* Endpoint 3 Descriptor */
// Endpoint Descriptor 86 6 In, Interrupt
static unsigned r9fill_35(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress)
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
		* buff ++ = highspeed ? 0x10 : 0xFF;   						/* bInterval: 255 mS */
	}
	return length;
}

/*Endpoint 2 OUT Descriptor*/
// Endpoint Descriptor 03 3 Out, Bulk, 64 bytes
static unsigned r9fill_37(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(VIRTUAL_COM_PORT_DATA_SIZE);
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
static unsigned r9fill_38(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(VIRTUAL_COM_PORT_DATA_SIZE);
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
	const uint8_t inep = USBD_EP_CDC_IN + offset;
	const uint8_t outnep = USBD_EP_CDC_OUT + offset;
	const uint8_t intnep = USBD_EP_CDC_INT + offset;

	// CDC
	n += CDCACM_InterfaceAssociationDescriptor_a(fill, p + n, maxsize - n, offset);	/* CDC: Interface Association Descriptor Abstract Control Model */
	n += CDCACM_InterfaceDescriptorControlIf_a(fill, p + n, maxsize - n, offset);	/* INTERFACE_CDC_CONTROL_3a Interface Descriptor 3/0 CDC Control, 1 Endpoint */
	n += r9fill_31(fill, p + n, maxsize - n);	/* Header Functional Descriptor*/
	n += CDCACM_r9fill_32_a(fill, p + n, maxsize - n, offset);	/* Call Managment Functional Descriptor*/
	n += r9fill_33(fill, p + n, maxsize - n);	/* ACM Functional Descriptor */
	n += CDC_UnionFunctionalDescriptor_a(fill, p + n, maxsize - n, offset);	/* Union Functional Descriptor INTERFACE_CDC_CONTROL_3a & INTERFACE_CDC_DATA_4a */
	n += r9fill_35(fill, p + n, maxsize - n, highspeed, USB_ENDPOINT_IN(intnep));	/* Endpoint Descriptor 86 6 In, Interrupt */

	n += CDC_InterfaceDescriptorDataIf_a(fill, p + n, maxsize - n, offset);	/* INTERFACE_CDC_DATA_4a Data class interface descriptor */
	n += r9fill_37(fill, p + n, maxsize - n, USB_ENDPOINT_OUT(outnep));	/* Endpoint Descriptor USBD_EP_CDC_OUT Out, Bulk, 64 bytes */
	n += r9fill_38(fill, p + n, maxsize - n, USB_ENDPOINT_IN(inep));	/* Endpoint Descriptor USBD_EP_CDC_IN In, Bulk, 64 bytes */

	return n;
}

#endif /* WITHUSBCDC */

#if WITHUSBCDCEEM

static unsigned CDCEEM_InterfaceAssociationDescriptor(
	uint_fast8_t fill, 
	uint8_t * buff, 
	unsigned maxsize
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
		* buff ++ = INTERFACE_CDCEEM_DATA_6;				// bFirstInterface
		* buff ++ = INTERFACE_CDCEEM_count;	// bInterfaceCount
		* buff ++ = USB_DEVICE_CLASS_COMMUNICATIONS;	// bFunctionClass: CDC
		* buff ++ = CDC_ETHERNET_EMULATION_MODEL;		// bFunctionSubClass - Ethernet Networking
		* buff ++ = 0x07;						// bFunctionProtocol
		* buff ++ = STRING_ID_5;				// iFunction - CDC Ethernet Control Model (EEM)
	}
	return length;
}

// Информация о типе требуемого драйвера берется отсюда по кодам в bInterfaceClass, bInterfaceSubclass, bInterfaceProtocol
static unsigned CDCEEM_r9fill_24(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, 
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
		* buff ++ = STRING_ID_5;					/* Unused iInterface */
		/* 9 byte*/
	}
	return length;
}

// Endpoint Descriptor
static unsigned CDCEEM_r9fill_37(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
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
static unsigned CDCEEM_r9fill_38(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
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

/* CDC Ethernet Emulation Model */
static unsigned fill_CDCEEM_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed)
{
	unsigned n = 0;

	// iadclasscode_r10.pdf
	// InterfaceAssociationDescriptor требуется только для многоинтерфейсных 
	// Провда, там написано что iadclasscode_r10.pdf
	n += CDCEEM_InterfaceAssociationDescriptor(fill, p + n, maxsize - n);	/* CDC EEM: Interface Association Descriptor Abstract Control Model */
	n += CDCEEM_r9fill_24(fill, p + n, maxsize - n, INTERFACE_CDCEEM_DATA_6, 0x00, 2);	/* INTERFACE_CDCEEM_DATA_6 Data class interface descriptor */
	n += CDCEEM_r9fill_38(fill, p + n, maxsize - n, USB_ENDPOINT_IN(USBD_EP_CDCEEM_IN));	/* Endpoint Descriptor USBD_EP_CDCECM_IN In, Bulk, 64 bytes */
	n += CDCEEM_r9fill_37(fill, p + n, maxsize - n, USB_ENDPOINT_OUT(USBD_EP_CDCEEM_OUT));	/* Endpoint Descriptor USBD_EP_CDCECM_OUT Out, Bulk, 64 bytes */

	return n;
}

#endif /* WITHUSBCDCEEM */

#if WITHUSBCDCECM

// ISBLyzer: Interface Association Descriptor Abstract Control Model
// documented in USB ECN : Interface Association Descriptor - InterfaceAssociationDescriptor_ecn.pdf
static unsigned CDCECM_InterfaceAssociationDescriptor(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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
		* buff ++ = INTERFACE_CDCECM_CONTROL_5;				// bFirstInterface
		* buff ++ = INTERFACE_CDCECM_count;	// bInterfaceCount
		* buff ++ = USB_DEVICE_CLASS_COMMUNICATIONS;	// bFunctionClass: CDC
		* buff ++ = CDC_ETHERNET_NETWORKING_CONTROL_MODEL;						// bFunctionSubClass - Ethernet Networking
		* buff ++ = 0x00;						// bFunctionProtocol
		* buff ++ = STRING_ID_5a;				// iFunction - CDC Ethernet Control Model (ECM)
	}
	return length;
}


/*Interface Descriptor*/
// USBLyzer: Interface Descriptor 3/0 CDC Control, 1 Endpoint 
static unsigned CDCECM_InterfaceDescriptorControlIf(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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
		* buff ++ = INTERFACE_CDCECM_CONTROL_5;   /* bInterfaceNumber: Number of Interface */
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
static unsigned CDCECM_UnionFunctionalDescriptor(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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
		* buff ++ = INTERFACE_CDCECM_CONTROL_5;	/* bMasterInterface: Communication class interface -  Zero based index of the interface in this configuration (bInterfaceNum) */
		* buff ++ = INTERFACE_CDCECM_DATA_6;	/* bSlaveInterface0: Data Class Interface -  Zero based index of the interface in this configuration (bInterfaceNum) */
	}
	return length;
}

// Ethernet Networking Functional Descriptor
static unsigned CDCECM_EthernetNetworkingFunctionalDescriptor(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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


/* Header Functional Descriptor */
static unsigned CDCECM_r9fill_31(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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

/* Endpoint 3 Descriptor */
// Endpoint Descriptor 86 6 In, Interrupt

static unsigned CDCECM_r9fill_35(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(16);
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;							/* bLength */
		* buff ++ = USB_ENDPOINT_DESCRIPTOR_TYPE; 	/* bDescriptorType: Endpoint */
		* buff ++ = bEndpointAddress;			/* bEndpointAddress: (IN) */
		* buff ++ = USB_ENDPOINT_TYPE_INTERRUPT;   	/* bmAttributes: Interrupt */
		* buff ++ = LO_BYTE(wMaxPacketSize);        /* wMaxPacketSize */
		* buff ++ = HI_BYTE(wMaxPacketSize); 
		* buff ++ = highspeed ? 0x09 : 32;   		/* bInterval: 32 mS */
	}
	return length;
}

// Endpoint Descriptor
static unsigned CDCECM_r9fill_37(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(USBD_CDCECM_BUFSIZE);
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
static unsigned CDCECM_r9fill_38(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(USBD_CDCECM_BUFSIZE);
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

static unsigned CDCECM_r9fill_24(
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
	unsigned n = 0;

	n += CDCECM_InterfaceAssociationDescriptor(fill, p + n, maxsize - n);	/* CDC: Interface Association Descriptor Abstract Control Model */
	n += CDCECM_InterfaceDescriptorControlIf(fill, p + n, maxsize - n);	/* INTERFACE_CDC_CONTROL_3a Interface Descriptor 3/0 CDC Control, 1 Endpoint */
	n += CDCECM_r9fill_31(fill, p + n, maxsize - n);	/* Header Functional Descriptor*/
	n += CDCECM_UnionFunctionalDescriptor(fill, p + n, maxsize - n);	/* Union Functional Descriptor INTERFACE_CDC_CONTROL_3a & INTERFACE_CDC_DATA_4a */
	n += CDCECM_EthernetNetworkingFunctionalDescriptor(fill, p + n, maxsize - n);	/* Union Functional Descriptor INTERFACE_CDC_CONTROL_3a & INTERFACE_CDC_DATA_4a */
	n += CDCECM_r9fill_35(fill, p + n, maxsize - n, highspeed, USB_ENDPOINT_IN(USBD_EP_CDCECM_INT));	/* Endpoint Descriptor 86 6 In, Interrupt */

	n += CDCECM_r9fill_24(fill, p + n, maxsize - n, INTERFACE_CDCECM_DATA_6, 0x00, 0);	/* INTERFACE_CDCECM_DATA_6 Data class interface descriptor */
	n += CDCECM_r9fill_24(fill, p + n, maxsize - n, INTERFACE_CDCECM_DATA_6, 0x01, 2);	/* INTERFACE_CDCECM_DATA_6 Data class interface descriptor */
	n += CDCECM_r9fill_38(fill, p + n, maxsize - n, USB_ENDPOINT_IN(USBD_EP_CDCECM_IN));	/* Endpoint Descriptor USBD_EP_CDCECM_IN In, Bulk, 64 bytes */
	n += CDCECM_r9fill_37(fill, p + n, maxsize - n, USB_ENDPOINT_OUT(USBD_EP_CDCECM_OUT));	/* Endpoint Descriptor USBD_EP_CDCECM_OUT Out, Bulk, 64 bytes */

	return n;
}

#endif /* WITHUSBCDCECM */

#if WITHUSBRNDIS


// Interface Association Descriptor RF Controller
// documented in USB ECN : Interface Association Descriptor - InterfaceAssociationDescriptor_ecn.pdf
static unsigned RNDIS_InterfaceAssociationDescriptor(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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
		* buff ++ = INTERFACE_RNDIS_CONTROL_5;				// bFirstInterface
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
static unsigned RNDIS_InterfaceDescriptorControlIf(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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
		* buff ++ = INTERFACE_RNDIS_CONTROL_5;   /* bInterfaceNumber: Number of Interface */
		* buff ++ = 0;		/* bAlternateSetting: Alternate setting  - zero-based index */
		* buff ++ = 0x01;   /* bNumEndpoints: One endpoints used (interrupt type) */
		* buff ++ = USB_DEVICE_CLASS_WIRELESS_CONTROLLER;	/* bInterfaceClass: Wireless Controller */
		* buff ++ = 0x01;						// bFunctionSubClass - RF Controller
		* buff ++ = 0x03;   /* bInterfaceProtocol - Remote NDIS */
		* buff ++ = STRING_ID_0;   /* iInterface */
	}
	return length;
}

/* Header Functional Descriptor */
static unsigned RNDIS_r9fill_31(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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


/* Call Managment Functional Descriptor */
// Call Management Functional Descriptor 
static unsigned RNDIS_r9fill_32(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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
		* buff ++ = INTERFACE_RNDIS_DATA_6;   /* bDataInterface: Zero based index of the interface in this configuration.(bInterfaceNum) */
	}
	return length;
}

// Abstract Control Management Functional Descriptor
static unsigned RNDIS_r9fill_33(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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
static unsigned RNDIS_UnionFunctionalDescriptor(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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
		* buff ++ = INTERFACE_RNDIS_CONTROL_5;	/* bMasterInterface: Communication class interface -  Zero based index of the interface in this configuration (bInterfaceNum) */
		* buff ++ = INTERFACE_RNDIS_DATA_6;	/* bSlaveInterface0: Data Class Interface -  Zero based index of the interface in this configuration (bInterfaceNum) */
	}
	return length;
}

/* Endpoint 3 Descriptor */
// Endpoint Descriptor 86 6 In, Interrupt

static unsigned RNDIS_r9fill_35(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, int highspeed, uint_fast8_t bEndpointAddress)
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
		* buff ++ = highspeed ? HSINTERVAL_AUDIO48 : FSINTERVAL_AUDIO48;   		/* bInterval: 1 mS */
	}
	return length;
}

/* Data class interface descriptor*/
// USBLyzer: Interface Descriptor 1/0 CDC Data, 2 Endpoints
static unsigned RNDIS_InterfaceDescriptorDataIf(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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
		* buff ++ = INTERFACE_RNDIS_DATA_6;			 /* bInterfaceNumber: Number of Interface */
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
static unsigned RNDIS_r9fill_38(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(USBD_RNDIS_BUFSIZE);
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
static unsigned RNDIS_r9fill_37(uint_fast8_t fill, uint8_t * buff, unsigned maxsize, uint_fast8_t bEndpointAddress)
{
	const uint_fast8_t length = 7;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (fill != 0 && buff != NULL)
	{
		const uint_fast16_t wMaxPacketSize = encodeMaxPacketSize(USBD_RNDIS_BUFSIZE);
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
	n += RNDIS_InterfaceAssociationDescriptor(fill, p + n, maxsize - n);	/* CDC: Interface Association Descriptor Abstract Control Model */
	// Interface Descriptor 0/0 Wireless Controller, 1 Endpoint
	//  Communication Class INTERFACE descriptor          https://msdn.microsoft.com/en-US/library/ee485851(v=winembedded.60).aspx
	n += RNDIS_InterfaceDescriptorControlIf(fill, p + n, maxsize - n);	/* INTERFACE_CDC_CONTROL_3a Interface Descriptor 3/0 CDC Control, 1 Endpoint */
	//  Functional Descriptors for Communication Class Interface per RNDIS spec.
	// Header Functional Descriptor
	////n += RNDIS_r9fill_31(fill, p + n, maxsize - n);
	// Call Management Functional Descriptor
	n += RNDIS_r9fill_32(fill, p + n, maxsize - n);
	// Abstract Control Management Functional Descriptor
	n += RNDIS_r9fill_33(fill, p + n, maxsize - n);
	// Union Functional Descriptor
	n += RNDIS_UnionFunctionalDescriptor(fill, p + n, maxsize - n);
	// Endpoint descriptors for Communication Class Interface     https://msdn.microsoft.com/en-US/library/ee482509(v=winembedded.60).aspx
	n += RNDIS_r9fill_35(fill, p + n, maxsize - n, highspeed, USB_ENDPOINT_IN(USBD_EP_RNDIS_INT));	/* Endpoint Descriptor 86 6 In, Interrupt */
	//  Data Class INTERFACE descriptor           https://msdn.microsoft.com/en-US/library/ee481260(v=winembedded.60).aspx
	n += RNDIS_InterfaceDescriptorDataIf(fill, p + n, maxsize - n);	/* INTERFACE_CDC_DATA_4a Data class interface descriptor */
	// IN Endpoint descriptor     https://msdn.microsoft.com/en-US/library/ee484483(v=winembedded.60).aspx
	n += RNDIS_r9fill_38(fill, p + n, maxsize - n, USB_ENDPOINT_IN(USBD_EP_RNDIS_IN));	/* Endpoint Descriptor USBD_EP_CDCECM_IN In, Bulk, 64 bytes */
	// OUT Endpoint descriptor     https://msdn.microsoft.com/en-US/library/ee482464(v=winembedded.60).aspx
	n += RNDIS_r9fill_37(fill, p + n, maxsize - n, USB_ENDPOINT_OUT(USBD_EP_RNDIS_OUT));	/* Endpoint Descriptor USBD_EP_CDCECM_OUT Out, Bulk, 64 bytes */
	return n;
	}

#endif /* WITHUSBRNDIS */

#if WITHUSBHID

static unsigned HID_InterfaceDescriptorXXXX(uint_fast8_t fill, uint8_t * buff, unsigned maxsize)
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
		* buff ++ = INTERFACE_HID_CONTROL_7;			// bInterfaceNumber
		* buff ++ = 0x00;								/* bAlternateSetting */
		* buff ++ = 0;//0x01;								/* bNumEndpoints */
		* buff ++ = USB_DEVICE_CLASS_HUMAN_INTERFACE;   /* bInterfaceClass */
		* buff ++ = 0;//0x01;								/* bInterfaceSubClass = boot interfsce */
		* buff ++ = 0;//0x02;							    /* bInterfaceProtocol: 1 = keyboard, 2 = mouse */
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

#if 0
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
		* buff ++ = highspeed ? 0x07 : 0x0A;   						/* bInterval: 10 mS */
	}
	return length;
}
#endif

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


/* HID Human Interface Device */
static unsigned fill_HID_XXXX_function(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed)
{
	// HID Report Descriptor used for this function
	#define HIDREPORTDESC HID_report_desc_keyboard
	//#define HIDREPORTDESC HID_report_desc_mouse
	//#define HIDREPORTDESC HID_report_desc_display
	//#define HIDREPORTDESC hid_report_desc_telephony

	const void * const pattern = HIDREPORTDESC;
	const unsigned patternlength = sizeof HIDREPORTDESC;
	//
	unsigned n = 0;
	//
	n += HID_InterfaceDescriptorXXXX(fill, p + n, maxsize - n);	/* HID: HID Interface Descriptor */
	n += HID_Descriptor(fill, p + n, maxsize - n, patternlength);	/* HID Descriptor */
	//n += fill_HID_Mouse_IntEP(fill, p + n, maxsize - n, highspeed, USB_ENDPOINT_IN(USBD_EP_HIDMOUSE_INT));

	return n;
}

#endif /* WITHUSBHID */

static unsigned fill_Configuration_main_group(uint_fast8_t fill, uint8_t * p, unsigned maxsize, int highspeed)
{
	unsigned n = 0;
	unsigned offset;

#if WITHUSBRNDIS
	n += fill_RNDIS_function(fill, p + n, maxsize - n, highspeed);
#endif /* WITHUSBRNDIS */

#if WITHUSBUAC
#if WITHUSBUAC3
		n += fill_UACINOUT48_function(fill, p + n, maxsize - n, highspeed, 0);
		//n += fill_UACINOUT48andRTS_function(fill, p + n, maxsize - n, highspeed, 0);
#if WITHRTS96 || WITHRTS192
		n += fill_UACINRTS_function(fill, p + n, maxsize - n, highspeed, 1);
#else /* WITHRTS96 || WITHRTS192 */
	#error WITHRTS96 or WITHRTS192 required for WITHUSBUAC3
#endif /* WITHRTS96 || WITHRTS192 */
#else /* WITHUSBUAC3 */
	n += fill_UACINOUT_function(fill, p + n, maxsize - n, highspeed, 0);
#endif /* WITHUSBUAC3 */
#endif /* WITHUSBUAC */

#if WITHUSBCDC
	for (offset = 0; offset < WITHUSBHWCDC_N; ++ offset)
		n += fill_CDCACM_function_a(fill, p + n, maxsize - n, highspeed, offset);
#endif /* WITHUSBCDC */

#if WITHUSBCDCEEM
	n += fill_CDCEEM_function(fill, p + n, maxsize - n, highspeed);
#endif /* WITHUSBCDCEEM */

#if WITHUSBCDCECM
	n += fill_CDCECM_function(fill, p + n, maxsize - n, highspeed);
#endif /* WITHUSBCDCECM */

#if WITHUSBHID
	n += fill_HID_XXXX_function(fill, p + n, maxsize - n, highspeed);
#endif /* WITHUSBHID */
	return n;
}

// Only for high speed capable devices 
// Other Speed Configuration descriptor - pass highspeed=1
// For all devices
// Configuration descriptor - pass highspeed=0
static unsigned fill_Configuration_descriptor(uint8_t * buff, unsigned maxsize, int highspeed)
{
#if WITHUSBHWHIGHSPEED && WITHUSBHWHIGHSPEEDDESC
	const int highspeedEPs = 1;
#else /* WITHUSBHWHIGHSPEED && WITHUSBHWHIGHSPEEDDESC */
	const int highspeedEPs = 0;
#endif /* WITHUSBHWHIGHSPEED && WITHUSBHWHIGHSPEEDDESC */
	unsigned length = 9;
	unsigned totalsize = length + fill_Configuration_main_group(0, buff, maxsize - length, highspeedEPs);

	const uint_fast8_t bNumInterfaces = INTERFACE_count;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
	if (buff != NULL)
	{
		// Вызов для заполнения, а не только для проверки занимаемого места в буфере
		* buff ++ = length;                                      /* bLength */ 
		* buff ++ = highspeed ? USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE : USB_CONFIGURATION_DESCRIPTOR_TYPE;      /* descriptor type */ 
		* buff ++ = LO_BYTE(totalsize);		/* length of packed config descr. (16 bit) */
		* buff ++ = HI_BYTE(totalsize);		/* length of packed config descr. (16 bit) */
		* buff ++ = bNumInterfaces;			/* bNumInterfaces  */
		* buff ++ = 0x01;                   /* bConfigurationValue */
		* buff ++ = 0x00;                   /* iConfiguration */
		* buff ++ = 0xC0;                   /* bmAttributes  BUS Powred, self powered */
		* buff ++ = USB_CONFIG_POWER_MA(250);/* bMaxPower = 250 mA. Сделано как попытка улучшить работу через активные USB изоляторы для обеспечения их питания. */

		fill_Configuration_main_group(1, buff, maxsize - length, highspeedEPs);
	}
	return totalsize;
}

// Device Descriptor
static unsigned fill_Device_descriptor(uint8_t * buff, unsigned maxsize)
{
	const unsigned length = 18;
	ASSERT(maxsize >= length);
	if (maxsize < length)
		return 0;
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
		* buff ++ = USB_OTG_MAX_EP0_SIZE;               /*  7:bMaxPacketSize0 (for DCP) */
		* buff ++ = LO_BYTE(USB_FUNCTION_VENDOR_ID);    /*  8:idVendor_lo */
		* buff ++ = HI_BYTE(USB_FUNCTION_VENDOR_ID);	/*  9:idVendor_hi */
		* buff ++ = LO_BYTE(USB_FUNCTION_PRODUCT_ID);   /* 10:idProduct_lo */
		* buff ++ = HI_BYTE(USB_FUNCTION_PRODUCT_ID);	/* 11:idProduct_hi */
		* buff ++ = LO_BYTE(USB_FUNCTION_RELEASE_NO);   /* 12:bcdDevice_lo */
		* buff ++ = HI_BYTE(USB_FUNCTION_RELEASE_NO);	/* 13:bcdDevice_hi */
		* buff ++ = STRING_ID_1;                        /* 14:iManufacturer */
		* buff ++ = STRING_ID_2;                        /* 15:iProduct */
		* buff ++ = STRING_ID_3;                        /* 16:iSerialNumber */
		* buff ++ = 1;                                  /* 17:bNumConfigurations */
	}
	return length;
}

// Only for high speed capable devices 
// Device Qualifier Descriptor 
static unsigned fill_DeviceQualifier_descriptor(uint8_t * buff, unsigned maxsize)
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
		* buff ++ = USB_OTG_MAX_EP0_SIZE;                   /*  7:bMaxPacketSize0 (for DCP) */
		* buff ++ = 1;                                      /*  8:bNumConfigurations */
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
			(1uL << 1) |	/* Link Power Management capability bit set */
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

static unsigned fill_align4(uint8_t * buff, unsigned maxsize)
{
	const uintptr_t granulation = 32;
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


static ALIGNX_BEGIN uint8_t alldescbuffer [2048] ALIGNX_END;

struct descholder StringDescrTbl [STRING_ID_count];
struct descholder ConfigDescrTbl [1];
struct descholder DeviceDescrTbl [1];

struct descholder OtherSpeedConfigurationTbl [1];
struct descholder DeviceQualifierTbl [1];
struct descholder BinaryDeviceObjectStoreTbl [1];
struct descholder HIDReportDescrTbl [1];


uint_fast8_t usbd_get_stringsdesc_count(void)
{
	return ARRAY_SIZE(StringDescrTbl);
}


// Динамическое формирование дескрипторов
/* вызывается при запрещённых прерываниях. */
void usbd_descriptors_initialize(uint_fast8_t HSdesc)
{
	unsigned partlen;
	unsigned score = 0;

	// Device Descriptor
	score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
	partlen = fill_Device_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
	//partlen = fill_pattern_descriptor(1, alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, fullSpdDesc, sizeof fullSpdDesc);
	DeviceDescrTbl [0].size = partlen;
	DeviceDescrTbl [0].data = alldescbuffer + score;
	score += partlen;

	// Configuration Descriptor
	score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
	partlen = fill_Configuration_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, 0);
	//partlen = fill_pattern_descriptor(1, alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, fullSpdConfDesc, sizeof fullSpdConfDesc);
	ConfigDescrTbl [0].size = partlen;
	ConfigDescrTbl [0].data = alldescbuffer + score;
	score += partlen;

	if (HSdesc != 0)
	{
		// Device Qualifier
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_DeviceQualifier_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
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
		// Other Speed Configuration
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_Configuration_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, 0);
		OtherSpeedConfigurationTbl [0].size = partlen;
		OtherSpeedConfigurationTbl [0].data = alldescbuffer + score;
		score += partlen;
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
	if (USB_FUNCTION_BCD_USB > 0x0200)
	{
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

	// Language ID (mandatory)
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

#if WITHUSBHID
	{
		const uint_fast8_t id = 0;
		ASSERT(id < ARRAY_SIZE(HIDReportDescrTbl));

		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_pattern_descriptor(1, alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, HIDREPORTDESC, sizeof HIDREPORTDESC);
		HIDReportDescrTbl [id].size = partlen;
		HIDReportDescrTbl [id].data = alldescbuffer + score;
		score += partlen;
	}
#endif /* WITHUSBHID */
	
#if WITHUSBCDCECM
	{
		// Формирование MAC адреса данного устройства
		// TODO: При модификации не забыть про достоверность значений
		const uint_fast8_t id = STRING_ID_MACADDRESS;
		char b [64];
		local_snprintf_P(b, ARRAY_SIZE(b), PSTR("F835DDA0C873"));
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
		const uint_fast8_t id = STRING_ID_3;
		char b [64];
		local_snprintf_P(b, ARRAY_SIZE(b), PSTR("SN:19640302_%lu"), (unsigned long) BUILD_ID);
		// Unic serial number
		score += fill_align4(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score);
		partlen = fill_string_descriptor(alldescbuffer + score, ARRAY_SIZE(alldescbuffer) - score, b);
		StringDescrTbl [id].size = partlen;
		StringDescrTbl [id].data = alldescbuffer + score;
		score += partlen;
	}
#elif CPUSTYLE_STM32F && defined(UID_BASE)
	{
		const uint_fast8_t id = STRING_ID_3;
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
		const uint_fast8_t id = STRING_ID_3;
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
	arm_hardware_flush_invalidate((uintptr_t) alldescbuffer, score);
	debug_printf_P(PSTR("usbd_descriptors_initialize: total length=%u\n"), score);
}


#endif /* WITHUSBHW */
