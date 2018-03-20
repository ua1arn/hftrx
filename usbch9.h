/* $Id$ */
/*
*   usbch9.h
*
*   Def's for USB device communications stack - "Standard Request" layer,
*   conforming to USB 2.0 specification, Chapter 9.
*
*   Adapted from Philips PDIUSBD12 firmware library (CHAP_9.H)
*   MJB Nov.2005
*/


#ifndef __CHAP_9_H__
#define __CHAP_9_H__

//#include "gendef.h"
//#include "usbdef.h"

#define  NUMBER_STD_REQ     13
#define UNICODE_ENGLISH     (0x0409)    /* US_English (Ref: USB_LANGIDs.pdf) */

/***********
*   Control endpoint packet structures
*/
typedef struct _device_request
{
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} DEVICE_REQUEST;

#define MAX_CONTROLDATA_SIZE    16

typedef struct _control_xfer
{
	DEVICE_REQUEST DeviceRequest;
	uint16_t wLength;                 /* Transfer size (from last Rx'd device request) */
	uint16_t wCount;                  /* Bytes sent/rec'd so far in current transfer */
	uint8_t * pData;                  /* Pointer to source data for TX transfer (Ctrl IN) */
	uint8_t  dataBuffer[MAX_CONTROLDATA_SIZE];    /* Buffer for RX data transsfer (Ctrl OUT) */
} CONTROL_XFER;

extern  volatile CONTROL_XFER ControlData;


void 	cdc_set_control_line_state (void);
void    cdc_set_line_coding(void);
void    cdc_get_line_coding(void);
void    cdc_set_control_line_state(void);
void    cdc_send_break(void);
void    cdc_send_encapsulated_command(void);
void    cdc_get_encapsulated_command(void);

struct descholder
{
	const uint8_t * data;
	unsigned size;
};

// STM32F429:
//	valid EPs: dcp, 0x01/0x81, 0x02/0x82, 0x03/0x83 
// STM32F446, STM32F746:
//	valid EPs: dcp, 0x01/0x81, 0x02/0x82, 0x03/0x83, 0x04/0x84, 0x05/0x85


#if WITHUSBUAC
	#if WITHUSBUAC3

		#define WITHUSBHWCDC_N	2	// количество виртуальных последовательных портов

	#else /* WITHUSBUAC3 */

		#define WITHUSBHWCDC_N	2	// количество виртуальных последовательных портов

	#endif /* WITHUSBUAC3 */
#endif /* WITHUSBUAC */

#if ! defined (WITHUSBHWCDC_N)
	#define WITHUSBHWCDC_N	2	// количество виртуальных последовательных портов
#endif /* ! defined (WITHUSBHWCDC_N) */


// IN and INT Endpoints allocation
enum
{
	ep0inxxx = 0x80,
#if WITHUSBUAC
	#if WITHUSBUAC3
		USBD_EP_AUDIO_IN,	// ISOC IN Аудиоданные в компьютер из TRX
		USBD_EP_RTS_IN,	// ISOC IN Аудиоданные в компьютер из TRX
	#else
		USBD_EP_AUDIO_IN,	// ISOC IN Аудиоданные в компьютер из TRX
	#endif
#endif /* WITHUSBUAC */

#if WITHUSBCDCEEM
	USBD_EP_CDCEEM_IN,	// CDC IN Данные ком-порта в компьютер из TRX
#endif /* WITHUSBCDCEEM */

#if WITHUSBCDCECM
	USBD_EP_CDCECM_IN,	// CDC IN Данные ком-порта в компьютер из TRX
	USBD_EP_CDCECM_INT,	// CDC INT События ком-порта в компьютер из TRX
#endif /* WITHUSBCDCECM */

#if WITHUSBCDC
	USBD_EP_CDC_IN,		// CDC IN Данные ком-порта в компьютер из TRX
	USBD_EP_CDC_INb,	// CDC IN Данные ком-порта в компьютер из TRX
	USBD_EP_CDC_INlast = USBD_EP_CDC_IN + WITHUSBHWCDC_N - 1,

	USBD_EP_CDC_INT,	// CDC INT События ком-порта в компьютер из TRX
	USBD_EP_CDC_INTb,	// CDC INT События ком-порта в компьютер из TRX
	USBD_EP_CDC_INTlast = USBD_EP_CDC_INT + WITHUSBHWCDC_N - 1,
#endif /* WITHUSBCDC */

#if WITHUSBHID
	//USBD_EP_HIDMOUSE_INT,	// HID INT События манипулятора в компьютер из TRX
#endif /* WITHUSBHID */
	epincount
};

// OUT Endpoints allocation
enum
{
	ep0outxxx = 0x00,
#if WITHUSBUAC
	#if WITHUSBUAC3
		USBD_EP_AUDIO_OUT,
	#else
		USBD_EP_AUDIO_OUT,	// ISOC OUT Аудиоданные от компьютера в TRX
	#endif
#endif /* WITHUSBUAC */

#if WITHUSBCDCEEM
	USBD_EP_CDCEEM_OUT,	// CDC OUT Данные ком-порта от компьютера в TRX
#endif /* WITHUSBCDCEEM */

#if WITHUSBCDCECM
	USBD_EP_CDCECM_OUT,	// CDC OUT Данные ком-порта от компьютера в TRX
#endif /* WITHUSBCDCECM */

#if WITHUSBCDC
	USBD_EP_CDC_OUT,	// CDC OUT Данные ком-порта от компьютера в TRX
	USBD_EP_CDC_OUTb,	// CDC OUT Данные ком-порта от компьютера в TRX
	USBD_EP_CDC_OUTlast = USBD_EP_CDC_OUT + WITHUSBHWCDC_N - 1,
#endif /* WITHUSBCDC */
	epoutcount
};

#if WITHUSBCDC
	#define VIRTUAL_COM_PORT_INT_SIZE 			10
	#define VIRTUAL_COM_PORT_DATA_SIZE			64
#endif /* WITHUSBCDC */

#if WITHUSBCDCEEM
	#define USBD_CDCEEM_BUFSIZE	64
#endif /* WITHUSBCDCEEM */

#if WITHUSBCDCECM
	#define USBD_CDCECM_BUFSIZE	64
#endif /* WITHUSBCDCECM */

#if WITHUSBHID
	#define HIDMOUSE_INT_DATA_SIZE 4
#endif /* WITHUSBHID */



//#if CPUSTYLE_R7S721

	/* 
		По звуковому каналу передается стерео, 16 бит, 48 кГц - 224 байт размер данных в ендпонтт
		По каналу real-time спектра стерео, 32 бит, 192 кГц - 288*2*4 = 2304 байта
	*/

#if WITHRTS96

	// stereo, 24 bit samples
	#define HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_RTS96		24
	#define HARDWARE_USBD_AUDIO_IN_CHANNELS_RTS		2
	#define VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS96		(DMABUFFSIZE96RTS * sizeof (uint8_t))

	#define HSINTERVAL_RTS96 4	// endpoint descriptor parameters
	#define FSINTERVAL_RTS96 1

#endif /* WITHRTS96 */
#if WITHRTS192

	// По каналу real-time спектра стерео, 32 бит, 192 кГц - 288*2*4 = 2304 байта
	// stereo, 32 bit samples
	#define HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_RTS192	32
	#define HARDWARE_USBD_AUDIO_IN_CHANNELS_RTS		2
	#define VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS192		(DMABUFFSIZE192RTS * sizeof (int8_t))

	#define HSINTERVAL_RTS192 3	// 500 us
	#define FSINTERVAL_RTS192 1

#endif /* WITHRTS192 */

	// stereo, 16 bit samples
	// По звуковому каналу передается стерео, 16 бит, 48 кГц - 288 байт размер данных в ендпонтт
	#define HARDWARE_USBD_AUDIO_IN_SAMPLEBITS_AUDIO48	16
	#define HARDWARE_USBD_AUDIO_IN_CHANNELS_AUDIO48		2
	#define VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_AUDIO48		(DMABUFFSIZE16 * sizeof (uint16_t))

	#define HSINTERVAL_AUDIO48 4	// endpoint descriptor parameters - для обеспечсения 1 кГц периода
	#define FSINTERVAL_AUDIO48 1

	#define HARDWARE_USBD_AUDIO_OUT_SAMPLEBITS	16
	#if WITHUABUACOUTAUDIO48MONO
		#define HARDWARE_USBD_AUDIO_OUT_CHANNELS	1
	#else /* WITHUABUACOUTAUDIO48MONO */
		#define HARDWARE_USBD_AUDIO_OUT_CHANNELS	2
	#endif /* WITHUABUACOUTAUDIO48MONO */
	#define VIRTUAL_AUDIO_PORT_DATA_SIZE_OUT	( \
		((ARMI2SRATE100 + 99) / 100000) * \
			((HARDWARE_USBD_AUDIO_OUT_SAMPLEBITS * HARDWARE_USBD_AUDIO_OUT_CHANNELS + 7) / 8) \
			)

#define HARDWARE_USBD_AUDIO_IN_CHANNELS	2	/* для всех каналов в IN направлении */

//#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX
//#else
//	#error Unsupported USB hardware 
//#endif

#define  HI_32BY(w)  (((w) >> 24) & 0xFF)   /* Extract 31..24 bits from unsigned word */
#define  HI_24BY(w)  (((w) >> 16) & 0xFF)   /* Extract 23..16 bits from unsigned word */
#define  HI_BYTE(w)  (((w) >> 8) & 0xFF)   /* Extract high-order byte from unsigned word */
#define  LO_BYTE(w)  ((w) & 0xFF)          /* Extract low-order byte from unsigned word */

#define  ARRAY_SIZE(a)  (sizeof(a)/sizeof(a[0]))

// Descriptor IDs
// Идентификаторы могут быть произвольными числами (кроме нуля)
enum
{
	TERMINAL_ID_UNDEFINED = 0,

	TERMINAL_ID_IT_1 = 10,	// USB Speaker Input Terminal
	TERMINAL_ID_OT_3 = 15,	// Output Terminal AUDIO_TERMINAL_RADIO_TRANSMITTER
	TERMINAL_ID_FU_5 = 20,	// Audio Feature Unit in transmitter path (audio)

	TERMINAL_ID_IT_2 = 30,	// Input Terminal (AUDIO) AUDIO_TERMINAL_RADIO_RECEIVER
	TERMINAL_ID_OT_4 = 35,	// Output Terminal
	TERMINAL_ID_FU_AUDIO = 40,	// Audio Feature Unit in RX path

	TERMINAL_ID_ITRTS_2 = 70,	// Input Terminal (AUDIO) AUDIO_TERMINAL_RADIO_RECEIVER
	TERMINAL_ID_OTRTS_4 = 75,	// Output Terminal
	TERMINAL_ID_FU_RTS = 80,		// Audio Feature Unit in RX spectrum path


	TERMINAL_ID_SELECTOR_6 = 150,	// Input selector - вынесено из-за неудобства использования: значение проверяется

	TERMINAL_ID_count
};

#define TERMINAL_ID_SELECTOR_6_INPUTS 2


#if WITHUSBUAC
	#if WITHUSBUAC3
		//#define INTERFACE_UAC_count 2	/* количество интерфейсов в одном UAC */
	#else /* WITHUSBUAC3 */
		#define INTERFACE_UAC_count 3	/* количество интерфейсов в одном UAC */
	#endif /* WITHUSBUAC3 */
#endif /* WITHUSBUAC */

/* Последовательность в данном enum должна соответствовать порядку использования в fill_Configuration_main_group */
enum
{
#if WITHUSBUAC
	#if WITHUSBUAC3
		INTERFACE_AUDIO_CONTROL_0,		/* AUDIO transmitter input control interface */
		INTERFACE_AUDIO_SPK_1,			/* USB Speaker Standard AS Interface Descriptor - Audio Streaming Zero Bandwith */
		INTERFACE_AUDIO_MIKE_2,		/* USB receiver output  Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3)*/ //zero-bandwidth interface
		/* */INTERFACE_AUDIO_CONTROL_1,		/* AUDIO spectrum control interface */
		INTERFACE_AUDIO_RTS_3,		/* USB spectrum Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3)*/ //zero-bandwidth interface
	#else
		INTERFACE_AUDIO_CONTROL_0,		/* AUDIO control interface */
		INTERFACE_AUDIO_SPK_1,		/* USB Speaker Standard AS Interface Descriptor - Audio Streaming Zero Bandwith */
		INTERFACE_AUDIO_MIKE_2,		/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3)*/ //zero-bandwidth interface
	#endif
#endif /* WITHUSBUAC */

#if WITHUSBCDC
	INTERFACE_CDC_base,
	INTERFACE_CDC_CONTROL_3a = INTERFACE_CDC_base,	/* CDC ACM control Interface */
	INTERFACE_CDC_DATA_4a,		/* CDC ACM data Interface */
	INTERFACE_CDC_CONTROL_3b,	/* CDC ACM control Interface */
	INTERFACE_CDC_DATA_4b,		/* CDC ACM data Interface */
	INTERFACE_CDC_last = INTERFACE_CDC_base + WITHUSBHWCDC_N * 2 - 1,
#endif /* WITHUSBCDC */

#if WITHUSBCDCEEM
	INTERFACE_CDCEEM_DATA_6,	/* CDC ECM/CDC EEM data Interface */
#endif /* WITHUSBCDCEEM */

#if WITHUSBCDCECM
	INTERFACE_CDCECM_CONTROL_5,	/* CDC ECM control Interface */
	INTERFACE_CDCECM_DATA_6,	/* CDC ECM/CDC EEM data Interface */
#endif /* WITHUSBCDCECM */

#if WITHUSBHID
	INTERFACE_HID_CONTROL_7,	/* HID control Interface */
#endif /* WITHUSBHID */
	// 
	INTERFACE_count				/* Значение для configuration descriptor */
};

#define INTERFACE_CDCACM_count 2	/* количество интерфейсов в одном CDC */
#define INTERFACE_CDCEEM_count 1	/* количество интерфейсов в одном CDC EEM */
#define INTERFACE_CDCECM_count 2	/* количество интерфейсов в одном CDC EEM */
#define INTERFACE_HID_count 1	/* количество интерфейсов в одном HID */


enum
{
	UACINALT_NONE = 0,
#if ! WITHRTSNOAUDIO
	UACINALT_AUDIO48,
#endif /* ! WITHRTSNOAUDIO */
#if ! WITHUSBUAC3
	#if WITHRTS96
		UACINALT_RTS96,
	#endif /* WITHRTS96 */
	#if WITHRTS192
		UACINALT_RTS192,
	#endif /* WITHRTS192 */
#endif /* ! WITHUSBUAC3 */
	//
	UACINALT_top
};

enum
{
	UACINRTSALT_NONE = 0,
#if WITHUSBUAC3
	#if WITHRTS96
		UACINRTSALT_RTS96,
	#endif /* WITHRTS96 */
	#if WITHRTS192
		UACINRTSALT_RTS192,
	#endif /* WITHRTS192 */
#endif /* WITHUSBUAC3 */
	//
	UACINRTSALT_top
};

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

	STRING_ID_HIDa,
	STRING_ID_IQSPECTRUM,
	// 
	STRING_ID_count
};

extern struct descholder StringDescrTbl [STRING_ID_count];
extern struct descholder ConfigDescrTbl [1];
extern struct descholder DeviceDescrTbl [1];
extern struct descholder DeviceQualifierTbl [1];
extern struct descholder OtherSpeedConfigurationTbl [1];
extern struct descholder BinaryDeviceObjectStoreTbl [1];
extern struct descholder HIDReportDescrTbl [1];

#endif  /* __CHAP_9_H__ */
