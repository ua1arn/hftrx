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

#define UNICODE_ENGLISH     (0x0409)    /* US_English (Ref: USB_LANGIDs.pdf) */

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

#if WITHUSBRNDIS
	USBD_EP_RNDIS_INT,
	USBD_EP_RNDIS_IN,
#endif /* WITHUSBRNDIS */

#if WITHUSBUAC
	#if WITHUSBUAC3
		USBD_EP_AUDIO_IN,	// ISOC IN Аудиоданные в компьютер из TRX
		USBD_EP_RTS_IN,	// ISOC IN Аудиоданные в компьютер из TRX
	#else
		USBD_EP_AUDIO_IN,	// ISOC IN Аудиоданные в компьютер из TRX
	#endif
#endif /* WITHUSBUAC */

#if WITHUSBCDC
	USBD_EP_CDC_IN,		// CDC IN Данные ком-порта в компьютер из TRX
	USBD_EP_CDC_INb,	// CDC IN Данные ком-порта в компьютер из TRX
	USBD_EP_CDC_INlast = USBD_EP_CDC_IN + WITHUSBHWCDC_N - 1,

	USBD_EP_CDC_INT,	// CDC INT События ком-порта в компьютер из TRX
	USBD_EP_CDC_INTb,	// CDC INT События ком-порта в компьютер из TRX
	USBD_EP_CDC_INTlast = USBD_EP_CDC_INT + WITHUSBHWCDC_N - 1,
#endif /* WITHUSBCDC */

#if WITHUSBCDCEEM
	USBD_EP_CDCEEM_IN,	// CDC IN Данные ком-порта в компьютер из TRX
#endif /* WITHUSBCDCEEM */

#if WITHUSBCDCECM
	USBD_EP_CDCECM_INT,	// CDC INT События ком-порта в компьютер из TRX
	USBD_EP_CDCECM_IN,	// CDC IN Данные ком-порта в компьютер из TRX
#endif /* WITHUSBCDCECM */

#if WITHUSBHID
	//USBD_EP_HIDMOUSE_INT,	// HID INT События манипулятора в компьютер из TRX
#endif /* WITHUSBHID */

#if WITHUSBDFU
		/* no endpoints need */
#endif /* WITHUSBDFU */
	//
	epincount
};

// OUT Endpoints allocation
enum
{
	ep0outxxx = 0x00,

#if WITHUSBRNDIS
	USBD_EP_RNDIS_OUT,
#endif /* WITHUSBRNDIS */

#if WITHUSBUAC
	#if WITHUSBUAC3
		USBD_EP_AUDIO_OUT,
	#else
		USBD_EP_AUDIO_OUT,	// ISOC OUT Аудиоданные от компьютера в TRX
	#endif
#endif /* WITHUSBUAC */

#if WITHUSBCDC
	USBD_EP_CDC_OUT,	// CDC OUT Данные ком-порта от компьютера в TRX
	USBD_EP_CDC_OUTb,	// CDC OUT Данные ком-порта от компьютера в TRX
	USBD_EP_CDC_OUTlast = USBD_EP_CDC_OUT + WITHUSBHWCDC_N - 1,
#endif /* WITHUSBCDC */

#if WITHUSBCDCEEM
	USBD_EP_CDCEEM_OUT,	// CDC OUT Данные ком-порта от компьютера в TRX
#endif /* WITHUSBCDCEEM */

#if WITHUSBCDCECM
	USBD_EP_CDCECM_OUT,	// CDC OUT Данные ком-порта от компьютера в TRX
#endif /* WITHUSBCDCECM */

#if WITHUSBHID
#endif /* WITHUSBHID */

#if WITHUSBDFU
		/* no endpoints need */
#endif /* WITHUSBDFU */
	//
	epoutcount
};

#if WITHUSBCDC
	#define VIRTUAL_COM_PORT_INT_SIZE 			10
	#if WITHUSBHWHIGHSPEEDDESC
		#define VIRTUAL_COM_PORT_IN_DATA_SIZE			USB_OTG_HS_MAX_PACKET_SIZE
		#define VIRTUAL_COM_PORT_OUT_DATA_SIZE			USB_OTG_HS_MAX_PACKET_SIZE
	#else /* WITHUSBHWHIGHSPEEDDESC */
		#define VIRTUAL_COM_PORT_IN_DATA_SIZE			USB_OTG_FS_MAX_PACKET_SIZE
		#define VIRTUAL_COM_PORT_OUT_DATA_SIZE			USB_OTG_FS_MAX_PACKET_SIZE
	#endif /* WITHUSBHWHIGHSPEEDDESC */
#endif /* WITHUSBCDC */

#if WITHUSBHID
	#define HIDMOUSE_INT_DATA_SIZE 4
#endif /* WITHUSBHID */

#if WITHUSBCDCEEM
	#if WITHUSBHWHIGHSPEEDDESC
		#define USBD_CDCEEM_BUFSIZE			USB_OTG_HS_MAX_PACKET_SIZE
	#else /* WITHUSBHWHIGHSPEEDDESC */
		#define USBD_CDCEEM_BUFSIZE			USB_OTG_FS_MAX_PACKET_SIZE
	#endif /* WITHUSBHWHIGHSPEEDDESC */
#endif /* WITHUSBCDCEEM */

#if WITHUSBCDCECM
	#define USBD_CDCECM_INT_SIZE	8	// interrupt data
	#if WITHUSBHWHIGHSPEEDDESC
		#define USBD_CDCECM_IN_BUFSIZE			USB_OTG_HS_MAX_PACKET_SIZE
		#define USBD_CDCECM_OUT_BUFSIZE			USB_OTG_HS_MAX_PACKET_SIZE
	#else /* WITHUSBHWHIGHSPEEDDESC */
		#define USBD_CDCECM_IN_BUFSIZE			USB_OTG_FS_MAX_PACKET_SIZE
		#define USBD_CDCECM_OUT_BUFSIZE			USB_OTG_FS_MAX_PACKET_SIZE
	#endif /* WITHUSBHWHIGHSPEEDDESC */
#endif /* WITHUSBCDCECM */

#if WITHUSBRNDIS
	#define USBD_RNDIS_INT_SIZE	8	// interrupt data
	#if WITHUSBHWHIGHSPEEDDESC
		#define USBD_RNDIS_IN_BUFSIZE			USB_OTG_HS_MAX_PACKET_SIZE
		#define USBD_RNDIS_OUT_BUFSIZE			USB_OTG_HS_MAX_PACKET_SIZE
	#else /* WITHUSBHWHIGHSPEEDDESC */
		#define USBD_RNDIS_IN_BUFSIZE			USB_OTG_FS_MAX_PACKET_SIZE	// bulk data
		#define USBD_RNDIS_OUT_BUFSIZE			USB_OTG_FS_MAX_PACKET_SIZE	// bulk data
	#endif /* WITHUSBHWHIGHSPEEDDESC */
#endif /* WITHUSBRNDIS */


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
// Применена автоматическая нумерация терминалов в каждой цепочке.
enum
{
	TERMINAL_ID_UNDEFINED = 0,

	TERMINAL_UACOUT48 = 1,	// fill_UACOUT48_function
	TERMINAL_UACIN48_UACINRTS = 35,	// fill_UACIN48_function or fill_UACIN48_INRTS_function
	TERMINAL_UACINRTS = 75,	// fill_UACINRTS_function

	TERMINAL_ID_SELECTOR_6 = 150,	// Input selector - вынесено из-за неудобства использования: значение проверяется

	//TERMINAL_ID_count
};

#define MAX_TERMINALS_IN_INTERFACE 6

#define TERMINAL_ID_SELECTOR_6_INPUTS 2

#define	WITHPLAINDESCROPTOR	1		/* не используется множество конфигураций */

#if WITHPLAINDESCROPTOR

#if WITHUSBUAC
	#if WITHUSBUAC3
		//#define INTERFACE_UAC_count 2	/* количество интерфейсов в одном UAC */
	#else /* WITHUSBUAC3 */
		//#define INTERFACE_UAC_count 3	/* количество интерфейсов в одном UAC */
	#endif /* WITHUSBUAC3 */
#endif /* WITHUSBUAC */

/* Последовательность в данном enum должна соответствовать порядку использования в fill_Configuration_main_group */
enum
{

#if WITHUSBRNDIS
	INTERFACE_RNDIS_CONTROL_5,	/* RNDIS control Interface */
	INTERFACE_RNDIS_DATA_6,		/* RNDIS data Interface */
#endif /* WITHUSBRNDIS */

#if WITHUSBCDC
	INTERFACE_CDC_base,
	INTERFACE_CDC_CONTROL_3a = INTERFACE_CDC_base,	/* CDC ACM control Interface */
	INTERFACE_CDC_DATA_4a,		/* CDC ACM data Interface */
	INTERFACE_CDC_CONTROL_3b,	/* CDC ACM control Interface */
	INTERFACE_CDC_DATA_4b,		/* CDC ACM data Interface */
	INTERFACE_CDC_last = INTERFACE_CDC_base + WITHUSBHWCDC_N * 2 - 1,
#endif /* WITHUSBCDC */

#if WITHUSBUAC
	#if WITHUSBUAC3
		INTERFACE_AUDIO_CONTROL_MIKE,		/* AUDIO receiever out control interface */
		INTERFACE_AUDIO_MIKE,		/* USB receiver output  Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3)*/ //zero-bandwidth interface
		INTERFACE_AUDIO_CONTROL_RTS,		/* AUDIO spectrum control interface */
		INTERFACE_AUDIO_RTS,		/* USB spectrum Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3)*/ //zero-bandwidth interface
		//INTERFACE_AUDIO_last = INTERFACE_AUDIO_CONTROL_SPK + 3,
	#else /* WITHUSBUAC3 */
		INTERFACE_AUDIO_CONTROL_MIKE,		/* AUDIO receiever out control interface */
		INTERFACE_AUDIO_MIKE,		/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3)*/ //zero-bandwidth interface
		//INTERFACE_AUDIO_last = INTERFACE_AUDIO_CONTROL_SPK + 2,
	#endif /* WITHUSBUAC3 */
	//#if WITHTX
		INTERFACE_AUDIO_CONTROL_SPK,		/* AUDIO transmitter input control interface */
		INTERFACE_AUDIO_SPK,			/* USB Speaker Standard AS Interface Descriptor - Audio Streaming Zero Bandwith */
	//#endif /* WITHTX */
#endif /* WITHUSBUAC */

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

#if WITHUSBDFU
	INTERFACE_DFU_CONTROL,		/* DFU control Interface */
#endif /* WITHUSBDFU */
	// 
	INTERFACE_count				/* Значение для configuration descriptor */
};

#define INTERFACE_CDCACM_count 2	/* количество интерфейсов в одном CDC */
#define INTERFACE_CDCEEM_count 1	/* количество интерфейсов в одном CDC EEM */
#define INTERFACE_CDCECM_count 2	/* количество интерфейсов в одном CDC EEM */
#define INTERFACE_HID_count 1	/* количество интерфейсов в одном HID */
#define INTERFACE_RNDIS_count 2	/* количество интерфейсов в одном RNDIS */
#define INTERFACE_DFU_count 1	/* количество интерфейсов в одном DFU */

//#define INTERFACE_UAC_count (INTERFACE_AUDIO_last - INTERFACE_AUDIO_CONTROL_SPK)

#else /* WITHPLAINDESCROPTOR */

	#if WITHUSBUAC
		#if WITHUSBUAC3
			//#define INTERFACE_UAC_count 2	/* количество интерфейсов в одном UAC */
		#else /* WITHUSBUAC3 */
			//#define INTERFACE_UAC_count 3	/* количество интерфейсов в одном UAC */
		#endif /* WITHUSBUAC3 */
	#endif /* WITHUSBUAC */

	#if WITHUSBRNDIS
		enum
		{

			INTERFACE_RNDIS_CONTROL_5,	/* RNDIS control Interface */
			INTERFACE_RNDIS_DATA_6,		/* RNDIS data Interface */
		};
	#endif /* WITHUSBRNDIS */
	#if WITHUSBUAC
		enum
		{

			#if WITHUSBUAC3
				INTERFACE_AUDIO_CONTROL_SPK,		/* AUDIO transmitter input control interface */
				INTERFACE_AUDIO_SPK,			/* USB Speaker Standard AS Interface Descriptor - Audio Streaming Zero Bandwith */
				INTERFACE_AUDIO_MIKE,		/* USB receiver output  Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3)*/ //zero-bandwidth interface
				INTERFACE_AUDIO_CONTROL_RTS,		/* AUDIO spectrum control interface */
				INTERFACE_AUDIO_RTS,		/* USB spectrum Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3)*/ //zero-bandwidth interface
				//INTERFACE_AUDIO_last = INTERFACE_AUDIO_CONTROL_SPK + 3,
			#else
				INTERFACE_AUDIO_CONTROL_SPK,		/* AUDIO control interface */
				INTERFACE_AUDIO_SPK,		/* USB Speaker Standard AS Interface Descriptor - Audio Streaming Zero Bandwith */
				INTERFACE_AUDIO_MIKE,		/* USB Microphone Standard AS Interface Descriptor (Alt. Set. 0) (CODE == 3)*/ //zero-bandwidth interface
				//INTERFACE_AUDIO_last = INTERFACE_AUDIO_CONTROL_SPK + 2,
			#endif
		};
	#endif /* WITHUSBUAC */
	#if WITHUSBCDC
		enum
		{

			INTERFACE_CDC_base,
			INTERFACE_CDC_CONTROL_3a = INTERFACE_CDC_base,	/* CDC ACM control Interface */
			INTERFACE_CDC_DATA_4a,		/* CDC ACM data Interface */
			INTERFACE_CDC_CONTROL_3b,	/* CDC ACM control Interface */
			INTERFACE_CDC_DATA_4b,		/* CDC ACM data Interface */
			INTERFACE_CDC_last = INTERFACE_CDC_base + WITHUSBHWCDC_N * 2 - 1,
		};
	#endif /* WITHUSBCDC */
	#if WITHUSBCDCEEM
		enum
		{
			INTERFACE_CDCEEM_DATA_6		/* CDC ECM/CDC EEM data Interface */
		};
	#endif /* WITHUSBCDCEEM */
	#if WITHUSBCDCECM
		enum
		{
			INTERFACE_CDCECM_CONTROL_5,	/* CDC ECM control Interface */
			INTERFACE_CDCECM_DATA_6		/* CDC ECM/CDC EEM data Interface */
		};
	#endif /* WITHUSBCDCECM */
	#if WITHUSBHID
		enum
		{
			INTERFACE_HID_CONTROL_7	/* HID control Interface */
			//
		};
	#endif /* WITHUSBHID */

	#if WITHUSBDFU
		enum
		{
			INTERFACE_DFU_CONTROL		/* DFU control Interface */
			//
		};
	#endif /* WITHUSBDFU */

	#define INTERFACE_CDCACM_count 2	/* количество интерфейсов в одном CDC */
	#define INTERFACE_CDCEEM_count 1	/* количество интерфейсов в одном CDC EEM */
	#define INTERFACE_CDCECM_count 2	/* количество интерфейсов в одном CDC ECM */
	#define INTERFACE_HID_count 1	/* количество интерфейсов в одном HID */
	#define INTERFACE_RNDIS_count 2	/* количество интерфейсов в одном RNDIS */
	#define INTERFACE_DFU_count 1	/* количество интерфейсов в одном DFU */

	enum
	{
		UNUSED_cfgidx = 0,
		// sequence of IDs should be same as used in usbd_descriptors_initialize
		CDCECM_cfgidx = 1,
		RNDIS_cfgidx = 2,
		//
		UNUSED2_cfgidx
	};

	#define INTERFACE_count 4 //(MAX(INTERFACE_RNDIS_count, INTERFACE_CDCECM_count))

#endif /* WITHPLAINDESCROPTOR */

	/*---------- -----------*/
//#define USBD_MAX_NUM_INTERFACES     7	// ?
/*---------- -----------*/
//#define USBD_MAX_NUM_CONFIGURATION     3
/*---------- -----------*/
#define USBD_DEBUG_LEVEL     0
/*---------- -----------*/
#define USBD_LPM_ENABLED     0
/*---------- -----------*/
#define USBD_SELF_POWERED     0


enum
{
	UACINALT_NONE = 0,
	UACINALT_AUDIO48,
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


struct descholder
{
	const uint8_t * data;
	unsigned size;
};

#define USBD_CONFIGCOUNT 4

extern struct descholder MsftStringDescr [1];	// Microsoft OS String Descriptor
extern struct descholder MsftCompFeatureDescr [1];	// Microsoft Compatible ID Feature Descriptor
extern struct descholder StringDescrTbl [];
extern struct descholder ConfigDescrTbl [USBD_CONFIGCOUNT];
extern struct descholder DeviceDescrTbl [USBD_CONFIGCOUNT];
extern struct descholder DeviceQualifierTbl [USBD_CONFIGCOUNT];
extern struct descholder OtherSpeedConfigurationTbl [USBD_CONFIGCOUNT];
extern struct descholder BinaryDeviceObjectStoreTbl [1];
extern struct descholder HIDReportDescrTbl [1];
uint_fast8_t usbd_get_stringsdesc_count(void);

#define DFU_VENDOR_CODE 0x44

#define USBD_DFU_XFER_SIZE 64

#endif  /* __CHAP_9_H__ */
