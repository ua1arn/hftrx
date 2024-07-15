/*
 * usbd_cdc.c
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
*/

#include "hardware.h"

#if WITHUSBHW && WITHUSBCDCACM

#include "buffers.h"
#include "board.h"
#include "formats.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usb_device.h"
#include "usb200.h"
#include "usbch9.h"

#include <string.h>

#if WITHWAWXXUSB
	#define usbd_cdc1_getrts	usbd_cdc1_getrts_unused
	#define usbd_cdc1_getdtr	usbd_cdc1_getdtr_unused
	#define usbd_cdc2_getrts	usbd_cdc2_getrts_unused
	#define usbd_cdc2_getdtr	usbd_cdc2_getdtr_unused
	#define usbd_cdc_send		usbd_cdc_send_unused
	#define usbd_cdc_ready		usbd_cdc_ready_unused
	#define usbd_cdc_enablerx	usbd_cdc_enablerx_unused
	#define usbd_cdc_enabletx	usbd_cdc_enabletx_unused
	#define usbd_cdc_tx			usbd_cdc_tx_unused
#endif /* WITHWAWXXUSB */

// CDC class-specific request codes
// (usbcdc11.pdf, 6.2, Table 46)
// see Table 45 for info about the specific requests.
#define CDC_SEND_ENCAPSULATED_COMMAND           0x00
#define CDC_GET_ENCAPSULATED_RESPONSE           0x01
#define CDC_SET_COMM_FEATURE                    0x02
#define CDC_GET_COMM_FEATURE                    0x03
#define CDC_CLEAR_COMM_FEATURE                  0x04
#define CDC_SET_AUX_LINE_STATE                  0x10
#define CDC_SET_HOOK_STATE                      0x11
#define CDC_PULSE_SETUP                         0x12
#define CDC_SEND_PULSE                          0x13
#define CDC_SET_PULSE_TIME                      0x14
#define CDC_RING_AUX_JACK                       0x15
#define CDC_SET_LINE_CODING                     0x20
#define CDC_GET_LINE_CODING                     0x21
#define CDC_SET_CONTROL_LINE_STATE              0x22
#define CDC_SEND_BREAK                          0x23
#define CDC_SET_RINGER_PARMS                    0x30
#define CDC_GET_RINGER_PARMS                    0x31
#define CDC_SET_OPERATION_PARMS                 0x32
#define CDC_GET_OPERATION_PARMS                 0x33
#define CDC_SET_LINE_PARMS                      0x34
#define CDC_GET_LINE_PARMS                      0x35
#define CDC_DIAL_DIGITS                         0x36
#define CDC_SET_UNIT_PARAMETER                  0x37
#define CDC_GET_UNIT_PARAMETER                  0x38
#define CDC_CLEAR_UNIT_PARAMETER                0x39
#define CDC_GET_PROFILE                         0x3A
#define CDC_SET_ETHERNET_MULTICAST_FILTERS      0x40
#define CDC_SET_ETHERNET_PMP_FILTER             0x41
#define CDC_GET_ETHERNET_PMP_FILTER             0x42
#define CDC_SET_ETHERNET_PACKET_FILTER          0x43
#define CDC_GET_ETHERNET_STATISTIC              0x44
#define CDC_SET_ATM_DATA_FORMAT                 0x50
#define CDC_GET_ATM_DEVICE_STATISTICS           0x51
#define CDC_SET_ATM_DEFAULT_VC                  0x52
#define CDC_GET_ATM_VC_STATISTICS               0x53

// Communication feature selector codes
// (usbcdc11.pdf, 6.2.2..6.2.4, Table 47)
#define CDC_ABSTRACT_STATE                      0x01
#define CDC_COUNTRY_SETTING                     0x02

// Feature Status returned for ABSTRACT_STATE Selector
// (usbcdc11.pdf, 6.2.3, Table 48)
#define CDC_IDLE_SETTING                        (1 << 0)
#define CDC_DATA_MULTPLEXED_STATE               (1 << 1)


// Control signal bitmap values for the SetControlLineState request
// (usbcdc11.pdf, 6.2.14, Table 51)
#define CDC_DTE_PRESENT                         (1 << 0)
#define CDC_ACTIVATE_CARRIER                    (1 << 1)

// CDC class-specific notification codes
// (usbcdc11.pdf, 6.3, Table 68)
// see Table 67 for Info about class-specific notifications
#define CDC_NOTIFICATION_NETWORK_CONNECTION     0x00
#define CDC_RESPONSE_AVAILABLE                  0x01
#define CDC_AUX_JACK_HOOK_STATE                 0x08
#define CDC_RING_DETECT                         0x09
#define CDC_NOTIFICATION_SERIAL_STATE           0x20
#define CDC_CALL_STATE_CHANGE                   0x28
#define CDC_LINE_STATE_CHANGE                   0x29
#define CDC_CONNECTION_SPEED_CHANGE             0x2A

// UART state bitmap values (Serial state notification).
// (usbcdc11.pdf, 6.3.5, Table 69)
#define CDC_SERIAL_STATE_OVERRUN                (1 << 6)  // receive data overrun error has occurred
#define CDC_SERIAL_STATE_PARITY                 (1 << 5)  // parity error has occurred
#define CDC_SERIAL_STATE_FRAMING                (1 << 4)  // framing error has occurred
#define CDC_SERIAL_STATE_RING                   (1 << 3)  // state of ring signal detection
#define CDC_SERIAL_STATE_BREAK                  (1 << 2)  // state of break detection
#define CDC_SERIAL_STATE_TX_CARRIER             (1 << 1)  // state of transmission carrier
#define CDC_SERIAL_STATE_RX_CARRIER             (1 << 0)  // state of receiver carrier

/*
 *
 *
 */

typedef union _UsbSetupPacket_t
{
  uint8_t Data[8];
  struct {
	  uint8_t  mRequestType;
    uint8_t             bRequest;
    uint16_t        wValue;
    uint16_t        wIndex;
    uint16_t        wLength;
  };
} UsbSetupPacket_t, * pUsbSetupPacket_t;
#pragma pack()
typedef union _SerialState_t
{
  uint16_t Data;
  struct
  {
    uint16_t  bRxCarrier  : 1;	// D0
    uint16_t  bTxCarrier  : 1;	// D1
    uint16_t  bBreak      : 1;	// D2
    uint16_t  bRingSignal : 1;	// D3
    uint16_t  bFraming    : 1;	// D4
    uint16_t  bParity     : 1;	// D5
    uint16_t  bOverRun    : 1;	// D5
    uint16_t              : 9;
  };
} SerialState_t, *pSerialState_t;

typedef struct _SerialStatePacket_t
{
  UsbSetupPacket_t UsbSetupPacket;
  SerialState_t    SerialState;
} SerialStatePacket_t, *pSerialStatePacket_t;

static __ALIGN_BEGIN uint8_t sendState [WITHUSBCDCACM_N] [10] __ALIGN_END;

static void notify(uint_fast8_t offset, uint_fast16_t state)
{
	const uint_fast8_t ifc = USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL, offset);
	const uint_fast16_t wLength = 2;
	uint8_t * p = sendState [offset];

//	USB_Setup_TypeDef * const pSetup = (USB_Setup_TypeDef *) p;

//	pSetup->b.bmRequestType = 0xA1;
//	pSetup->b.bRequest = 0x20;	// SERIAL_STATE
//	pSetup->b.wValue.w = 0;
//	pSetup->b.wIndex.w = ifc;
//	pSetup->b.wLength.w = 2;

	p [0] = 0xA1; // bmRequestType
	p [1] = 0x20; // bRequest SERIAL_STATE
	p [2] = LO_BYTE(0);	// wValue
	p [3] = HI_BYTE(0);
	p [4] = LO_BYTE(ifc);		// wIndex
	p [5] = HI_BYTE(ifc);
	p [6] = LO_BYTE(wLength);	// wLength
	p [7] = HI_BYTE(wLength);
	p [8] = LO_BYTE(state);		// data
	p [9] = HI_BYTE(state);
}

// Состояние - выбранные альтернативные конфигурации по каждому интерфейсу USB configuration descriptor
//static uint8_t altinterfaces [INTERFACE_count];

static volatile uint8_t usb_cdc_control_state [WITHUSBCDCACM_N];

static volatile uint8_t usbd_cdcX_rxenabled [WITHUSBCDCACM_N];	/* виртуальный флаг разрешения прерывания по приёму символа - HARDWARE_CDC_ONRXCHAR */
static __ALIGN_BEGIN uint8_t cdcXbuffout [WITHUSBCDCACM_N] [VIRTUAL_COM_PORT_OUT_DATA_SIZE] __ALIGN_END;
static __ALIGN_BEGIN uint8_t cdcXbuffin [WITHUSBCDCACM_N] [VIRTUAL_COM_PORT_IN_DATA_SIZE] __ALIGN_END;
static uint16_t cdcXbuffinlevel [WITHUSBCDCACM_N];

static __ALIGN_BEGIN uint8_t cdc_epXdatabuffout [64U] __ALIGN_END;

static uint32_t dwDTERate [WITHUSBCDCACM_N];

#define MAIN_CDC_OFFSET 0
#if WITHUSBCDCACM_N > 1
	#define SECOND_CDC_OFFSET 1
#endif /* WITHUSBCDCACM_N > 1 */

static LCLSPINLOCK_t catlock = LCLSPINLOCK_INIT;

/* управление по DTR происходит сразу, RTS только вместе со следующим DTR */
/* хранимое значение после получения CDC_SET_CONTROL_LINE_STATE */
/* Биты: RTS = 0x02, DTR = 0x01 */

// Обычно используется для переключения на передачу (PTT)
// вызывается в конексте system interrupt
uint_fast8_t usbd_cdc1_getrts(void)
{
	const unsigned offset = MAIN_CDC_OFFSET;
	LCLSPIN_LOCK(& catlock);
	const uint_fast8_t state =
		((usb_cdc_control_state [offset] & CDC_ACTIVATE_CARRIER) != 0) ||
		0;
	LCLSPIN_UNLOCK(& catlock);
	return state;
}

// Обычно используется для телеграфной манипуляции (KEYDOWN)
// вызывается в конексте system interrupt
uint_fast8_t usbd_cdc1_getdtr(void)
{
	const unsigned offset = MAIN_CDC_OFFSET;
	LCLSPIN_LOCK(& catlock);
	const uint_fast8_t state =
		((usb_cdc_control_state [offset] & CDC_DTE_PRESENT) != 0) ||
		0;
	LCLSPIN_UNLOCK(& catlock);
	return state;
}

// Обычно используется для переключения на передачу (PTT)
// вызывается в конексте system interrupt
uint_fast8_t usbd_cdc2_getrts(void)
{
#if WITHUSBCDCACM_N > 1
	const unsigned offset = SECOND_CDC_OFFSET;
	LCLSPIN_LOCK(& catlock);
	const uint_fast8_t state =
		((usb_cdc_control_state [offset] & CDC_ACTIVATE_CARRIER) != 0) ||
		0;
	LCLSPIN_UNLOCK(& catlock);
	return state;
#else /* WITHUSBCDCACM_N > 1 */
	return 0;
#endif /* WITHUSBCDCACM_N > 1 */
}

// Обычно используется для телеграфной манипуляции (KEYDOWN)
// вызывается в конексте system interrupt
uint_fast8_t usbd_cdc2_getdtr(void)
{
#if WITHUSBCDCACM_N > 1
	const unsigned offset = SECOND_CDC_OFFSET;
	LCLSPIN_LOCK(& catlock);
	const uint_fast8_t state =
		((usb_cdc_control_state [offset] & CDC_DTE_PRESENT) != 0) ||
		0;
	LCLSPIN_UNLOCK(& catlock);
	return state;
#else /* WITHUSBCDCACM_N > 1 */
	return 0;
#endif /* WITHUSBCDCACM_N > 1 */
}

static volatile uint8_t usbd_cdc_txenabled [WITHUSBCDCACM_N];	/* виртуальный флаг разрешения прерывания по готовности передатчика - HARDWARE_CDC_ONTXCHAR*/
static USBD_HandleTypeDef * volatile gpdev = NULL;
static volatile uint8_t usbd_cdc_txstate [WITHUSBCDCACM_N];	/* склько осталось повторов ожидания конца передачи */

/* временное решение для передачи (вызывается при запрещённых прерываниях). */
void usbd_cdc_send(const void * buff, size_t length)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	const unsigned offset = MAIN_CDC_OFFSET;
	if (gpdev != NULL && usbd_cdc_txstate [offset] == 0)
	{
		USBD_HandleTypeDef * const pdev = gpdev;

		const uint_fast8_t CDCInEpAdd = USB_ENDPOINT_IN(USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, offset));
		const size_t n = ulmin(length, VIRTUAL_COM_PORT_IN_DATA_SIZE);
		memcpy(cdcXbuffin [offset], buff, n);

	    /* Update the packet total length */
	    pdev->ep_in[CDCInEpAdd & 0xFU].total_length = n;
	    /* Transmit next packet */
		USBD_LL_Transmit(pdev, CDCInEpAdd, cdcXbuffin [offset], n);

		usbd_cdc_txstate [offset] = 1;
	}
	LowerIrql(oldIrql);
}

uint_fast8_t usbd_cdc_ready(void)	/* временное решение для передачи */
{
	const uint32_t waittm = 50;
	const unsigned offset = MAIN_CDC_OFFSET;
	if (gpdev == NULL)
		return 0;
	if (usbd_cdc_txstate [offset] != 0)
	{
			return 0;
	}
	return 1;
}

/* Разрешение/запрещение прерывания по передаче символа */
void usbd_cdc_enabletx(uint_fast8_t state)	/* вызывается из обработчика прерываний */
{
	const unsigned offset = MAIN_CDC_OFFSET;
	LCLSPIN_LOCK(& catlock);
	usbd_cdc_txenabled [offset] = state;
	LCLSPIN_UNLOCK(& catlock);
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void usbd_cdc_enablerx(uint_fast8_t state)	/* вызывается из обработчика прерываний */
{
	const unsigned offset = MAIN_CDC_OFFSET;
	LCLSPIN_LOCK(& catlock);
	usbd_cdcX_rxenabled [offset] = state;
	LCLSPIN_UNLOCK(& catlock);
}

/* передача символа после прерывания о готовности передатчика - вызывается из HARDWARE_CDC_ONTXCHAR */
void usbd_cdc_tx(void * ctx, uint_fast8_t c)
{
	const unsigned offset = MAIN_CDC_OFFSET;
	USBD_HandleTypeDef * const pdev = (USBD_HandleTypeDef *) ctx;

	LCLSPIN_LOCK(& catlock);
	ASSERT(cdcXbuffinlevel  [offset] < VIRTUAL_COM_PORT_IN_DATA_SIZE);
	cdcXbuffin [offset] [cdcXbuffinlevel [offset] ++] = c;
	LCLSPIN_UNLOCK(& catlock);
}

/* использование буфера принятых данных */
static void cdcXout_buffer_save(
	const uint8_t * data,
	unsigned length,
	unsigned offset
	)
{
	unsigned i;
	//printhex(offset * 256, data, length);
	if (offset != MAIN_CDC_OFFSET)
		return;
	for (i = 0; usbd_cdcX_rxenabled [offset] && i < length; ++ i)
	{
		HARDWARE_CDC_ONRXCHAR(offset, data [i]);
	}
}

/* проверка принадлежности номера интерфейса к набору CINTROL */
static uint_fast8_t
usbd_cdc_iscontrol_ifc(uint_fast8_t interfacev)
{
	unsigned offset;

	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
		if (interfacev == USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL, offset))	// CDC control interface
			return 1;
	}
	return 0;
}

/* проверка принадлежности номера интерфейса к набору DATA */
static uint_fast8_t
usbd_cdc_isisdata_ifc(uint_fast8_t interfacev)
{
	unsigned offset;

	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
		if (interfacev == USBD_CDCACM_IFC(INTERFACE_CDC_DATA, offset))	// CDC data interface
			return 1;
	}
	return 0;
}

// При возврате из этой функции в usbd_core.c происходит вызов USBD_CtlSendStatus
static USBD_StatusTypeDef USBD_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	const USBD_SetupReqTypedef * const req = & pdev->request;

	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	//PRINTF(PSTR("1 USBD_CDC_EP0_RxReady: interfacev=%u: bRequest=%u, wLength=%u\n"), interfacev, req->bRequest, req->wLength);
	if (usbd_cdc_iscontrol_ifc(interfacev) || usbd_cdc_isisdata_ifc(interfacev))
	{
		switch (req->bRequest)
		{
		case CDC_SET_LINE_CODING:
			{
				const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
				const unsigned offset = USBD_CDCACM_OFFSET_BY_INT_IFV(interfacev);
				ASSERT(req->wLength == 7);
				dwDTERate [offset] = USBD_peek_u32(& cdc_epXdatabuffout [0]);
				//PRINTF(PSTR("USBD_CDC_EP0_RxReady: CDC_SET_LINE_CODING: interfacev=%u, dwDTERate=%lu, bits=%u\n"), interfacev, dwDTERate [offset], cdc_epXdatabuffout [6]);
			}
			break;
		default:
			// непонятно, для чего эти данные?
            PRINTF("req->bReques=%02X\n", req->bRequest);
            TP();
			break;
		}
	}
	return USBD_OK;
}
/*
* @param  epnum: endpoint index without direction bit
*
 */
static USBD_StatusTypeDef USBD_CDC_DataIn(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	if (USB_ENDPOINT_IN(epnum) >= USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, 0) && USB_ENDPOINT_IN(epnum) < USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, WITHUSBCDCACM_N))
	{
		PCD_HandleTypeDef * const hpcd = (PCD_HandleTypeDef *) pdev->pData;
		const unsigned ix = epnum & 0x0F;
		//PRINTF("USBD_CDC_DataIn: epnum=%02X\n", (unsigned) epnum);
		if ((pdev->ep_in[ix].total_length > 0U) && ((pdev->ep_in[ix].total_length % hpcd->IN_ep[ix].maxpacket) == 0U))
		{
			/* Update the packet total length */
			pdev->ep_in[ix].total_length = 0U;

			/* Send ZLP */
			(void)USBD_LL_Transmit(pdev, epnum, NULL, 0U);
		}
		else
		{
			const unsigned offset = USBD_CDCACM_OFFSET_BY_IN_EP(USB_ENDPOINT_IN(epnum), USBD_EP_CDCACM_IN);
			ASSERT(offset < WITHUSBCDCACM_N);
			usbd_cdc_txstate [offset] = 0;
		}
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	if (epnum >= USBD_CDCACM_OUT_EP(USBD_EP_CDCACM_OUT, 0) && epnum < USBD_CDCACM_OUT_EP(USBD_EP_CDCACM_OUT, WITHUSBCDCACM_N))
	{
		const unsigned offset = USBD_CDCACM_OFFSET_BY_OUT_EP(epnum, USBD_EP_CDCACM_OUT);
		/* CDC EP OUT */
		// use CDC data
		cdcXout_buffer_save(cdcXbuffout [offset], USBD_LL_GetRxDataSize(pdev, epnum), offset);	/* использование буфера принятых данных */
		//memcpy(cdc1buffin, cdc1buffout, cdc1buffinlevel = USBD_LL_GetRxDataSize(pdev, epnum));
		/* Prepare Out endpoint to receive next cdc data packet */
		USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(epnum), cdcXbuffout [offset], VIRTUAL_COM_PORT_OUT_DATA_SIZE);
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDC_Setup(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	static __ALIGN_BEGIN uint8_t buff [32] __ALIGN_END;	// was: 7
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	if ((req->bmRequest & USB_REQ_TYPE_DIR) != 0)
	{
		// IN direction
		switch (req->bmRequest & USB_REQ_TYPE_MASK)
		{
		case USB_REQ_TYPE_CLASS:
			if (usbd_cdc_iscontrol_ifc(interfacev) || usbd_cdc_isisdata_ifc(interfacev))
			{
				const unsigned offset = USBD_CDCACM_OFFSET_BY_INT_IFV(interfacev);
				switch (req->bRequest)
				{
				case CDC_GET_LINE_CODING:
					//PRINTF(PSTR("USBD_CDC_Setup IN: CDC_GET_LINE_CODING, dwDTERate=%lu\n"), (unsigned long) dwDTERate [offset]);
					USBD_poke_u32(& buff [0], dwDTERate [offset]); // dwDTERate
					buff [4] = 0;	// 1 stop bit
					buff [5] = 0;	// parity=none
					buff [6] = 8;	// bDataBits

					USBD_CtlSendData(pdev, buff, ulmin16(7, req->wLength));
					break;

				default:
		            PRINTF("req->bReques=%02X\n", req->bRequest);
					TP();
					USBD_CtlError(pdev, req);
					break;
				}
			}
			break;

		case USB_REQ_TYPE_STANDARD:
			if (usbd_cdc_iscontrol_ifc(interfacev) || usbd_cdc_isisdata_ifc(interfacev))
			{
				switch (req->bRequest)
				{
					case USB_REQ_GET_INTERFACE:
						{
							static __ALIGN_BEGIN uint8_t buff [64] __ALIGN_END;
							//PRINTF(PSTR("USBD_CDC_Setup: USB_REQ_TYPE_STANDARD USB_REQ_GET_INTERFACE dir=%02X interfacev=%d, req->wLength=%d\n"), req->bmRequest & 0x80, interfacev, (int) req->wLength);
							buff [0] = 0;
							USBD_CtlSendData(pdev, buff, ulmin16(ARRAY_SIZE(buff), req->wLength));
						}
						break;
					default:
			            PRINTF("req->bReques=%02X\n", req->bRequest);
						TP();
						break;
				}
			}
			break;
		}
	}
	else
	{
		// OUT direction
		switch (req->bmRequest & USB_REQ_TYPE_MASK)
		{
		case USB_REQ_TYPE_CLASS:
			if (usbd_cdc_iscontrol_ifc(interfacev) || usbd_cdc_isisdata_ifc(interfacev))
			{
				const unsigned offset = USBD_CDCACM_OFFSET_BY_INT_IFV(interfacev);
				switch (req->bRequest)
				{
				case CDC_SET_CONTROL_LINE_STATE:
					// Выполнение этого запроса не требует дополнительного чтения данных
					//PRINTF(PSTR("USBD_CDC_Setup OUT: CDC_SET_CONTROL_LINE_STATE, wValue=%04X\n"), req->wValue);
					usb_cdc_control_state [offset] = LO_BYTE(req->wValue);
					ASSERT(req->wLength == 0);
					break;

				case CDC_SET_LINE_CODING:
					//PRINTF(PSTR("USBD_CDC_Setup OUT: CDC_SET_LINE_CODING, wValue=%04X\n"), req->wValue);
					break;

				default:
					PRINTF("req->bReques=%02X\n", req->bRequest);
					TP();
					break;
				}
				/* все запросы этого класса устройств */
				if (req->wLength != 0)
				{
					USBD_CtlPrepareRx(pdev, cdc_epXdatabuffout, ulmin16(ARRAY_SIZE(cdc_epXdatabuffout), req->wLength));
				}
				else
				{
					USBD_CtlSendStatus(pdev);
				}
			}
			break;

		case USB_REQ_TYPE_STANDARD:
			switch (req->bRequest)
			{
			case USB_REQ_SET_INTERFACE:
				if (usbd_cdc_iscontrol_ifc(interfacev) || usbd_cdc_isisdata_ifc(interfacev))
				{
					// Only zero value here
					//altinterfaces [interfacev] = LO_BYTE(req->wValue);
					//PRINTF("USBD_CDC_Setup: CDC interface %d set to %d\n", (int) interfacev, (int) altinterfaces [interfacev]);
					//bufers_set_cdcalt(altinterfaces [interfacev]);
					USBD_CtlSendStatus(pdev);
				}
				break;
			default:
				TP();
				break;
			}
			break;

		default:
			TP();
			break;
		}
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDC_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	uint_fast8_t offset;

  	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
		usbd_cdc_txenabled [offset] = 0;
		//usbd_cdc_rxenabled [offset] = 0;
		usbd_cdc_txstate [offset] = 0;
		usb_cdc_control_state [offset] = 0;
		dwDTERate [offset] = 115200;

		/* cdc Open EP IN */
		USBD_LL_OpenEP(pdev, USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, offset), USBD_EP_TYPE_BULK, VIRTUAL_COM_PORT_IN_DATA_SIZE);
		/* cdc Open EP OUT */
		USBD_LL_OpenEP(pdev, USBD_CDCACM_OUT_EP(USBD_EP_CDCACM_OUT, offset), USBD_EP_TYPE_BULK, VIRTUAL_COM_PORT_OUT_DATA_SIZE);
		/* CDC Open EP interrupt */
		USBD_LL_OpenEP(pdev, USBD_CDCACM_INT_EP(USBD_EP_CDCACM_INT, offset), USBD_EP_TYPE_INTR, VIRTUAL_COM_PORT_INT_SIZE);
		/* CDC Prepare Out endpoint to receive 1st packet */
		USBD_LL_PrepareReceive(pdev, USBD_CDCACM_OUT_EP(USBD_EP_CDCACM_OUT, offset), cdcXbuffout [offset],  VIRTUAL_COM_PORT_OUT_DATA_SIZE);
	}

 	gpdev = pdev;
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	uint_fast8_t offset;

	gpdev = NULL;
 	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
		USBD_LL_CloseEP(pdev, USBD_CDCACM_INT_EP(USBD_EP_CDCACM_INT, offset));
		USBD_LL_CloseEP(pdev, USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, offset));
		USBD_LL_CloseEP(pdev, USBD_CDCACM_OUT_EP(USBD_EP_CDCACM_OUT, offset));
	}

	HARDWARE_CDC_ONDISCONNECT();

	/* при потере связи с host снять запрос на передачу */
 	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
 		usb_cdc_control_state [offset] = 0;
	}
	return USBD_OK;
}

static void USBD_CDC_ColdInit(void)
{
	uint_fast8_t offset;
 	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
 		usb_cdc_control_state [offset] = 0;
	}
}

const USBD_ClassTypeDef USBD_CLASS_CDCACM =
{
	USBD_CDC_ColdInit,
	USBD_CDC_Init,	// Init
	USBD_CDC_DeInit,	// DeInit
	USBD_CDC_Setup,		// Setup
	NULL,	// EP0_TxSent
	USBD_CDC_EP0_RxReady,	// EP0_RxReady
	USBD_CDC_DataIn,	// DataIn
	USBD_CDC_DataOut,	// DataOut
	NULL,	//USBD_XXX_SOF,	// SOF
	NULL,	//USBD_XXX_IsoINIncomplete,	// IsoINIncomplete
	NULL,	//USBD_XXX_IsoOUTIncomplete,	// IsoOUTIncomplete
};

#if WITHDEBUG && WITHDEBUG_CDC

// Очереди символов для обмена с host
enum { qSZdevice = 8192 };

static uint8_t debugusb_queue [qSZdevice];
static unsigned debugusb_qp, debugusb_qg;

// Передать символ в host
static uint_fast8_t	debugusb_qput(uint_fast8_t c)
{
	unsigned qpt = debugusb_qp;
	const unsigned next = (qpt + 1) % qSZdevice;
	if (next != debugusb_qg)
	{
		debugusb_queue [qpt] = c;
		debugusb_qp = next;
		return 1;
	}
	return 0;
}

// Получить символ в host
static uint_fast8_t debugusb_qget(uint_fast8_t * pc)
{
	if (debugusb_qp != debugusb_qg)
	{
		* pc = debugusb_queue [debugusb_qg];
		debugusb_qg = (debugusb_qg + 1) % qSZdevice;
		return 1;
	}
	return 0;
}

// получить состояние очереди передачи
static uint_fast8_t debugusb_qempty(void)
{
	return debugusb_qp == debugusb_qg;
}

enum { qSZhost = 32 };

static uint8_t debugusb_ci_queue [qSZhost];
static unsigned debugusb_ci_qp, debugusb_ci_qg;

// Передать символ в device
static uint_fast8_t	debugusb_ci_qput(uint_fast8_t c)
{
	unsigned qpt = debugusb_ci_qp;
	const unsigned next = (qpt + 1) % qSZhost;
	if (next != debugusb_ci_qg)
	{
		debugusb_ci_queue [qpt] = c;
		debugusb_ci_qp = next;
		return 1;
	}
	return 0;
}

// Получить символ в host
static uint_fast8_t debugusb_ci_qget(uint_fast8_t * pc)
{
	if (debugusb_ci_qp != debugusb_ci_qg)
	{
		* pc = debugusb_ci_queue [debugusb_ci_qg];
		debugusb_ci_qg = (debugusb_ci_qg + 1) % qSZhost;
		return 1;
	}
	return 0;
}

// получить состояние очереди передачи
static uint_fast8_t debugusb_ci_qempty(void)
{
	return debugusb_ci_qp == debugusb_ci_qg;
}

uint_fast8_t debugusb_putchar(uint_fast8_t c)/* передача символа если готов порт */
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	const uint_fast8_t f = debugusb_qput(c);
	if (f)
		HARDWARE_DEBUG_ENABLETX(1);
	LowerIrql(oldIrql);
	return f;
}

uint_fast8_t debugusb_getchar(char * cp) /* приём символа, если готов порт */
{
	uint_fast8_t c;
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	const uint_fast8_t f = debugusb_ci_qget(& c);
	LowerIrql(oldIrql);
	if (f)
		* cp = c;
	return f;
}

void debugusb_parsechar(uint_fast8_t c)				/* вызывается из обработчика прерываний */
{
	debugusb_ci_qput(c);
}

void debugusb_sendchar(void * ctx)							/* вызывается из обработчика прерываний */
{
	uint_fast8_t c;
	if (debugusb_qget(& c))
	{
		HARDWARE_DEBUG_TX(ctx, c);
		if (debugusb_qempty())
			HARDWARE_DEBUG_ENABLETX(0);
	}
	else
	{
		HARDWARE_DEBUG_ENABLETX(0);
	}
}

// Вызывается из user-mode программы при запрещённых прерываниях.
void debugusb_initialize(void)
{
}

#endif /* WITHDEBUG && WITHDEBUG_CDC */

#endif /* WITHUSBHW && WITHUSBCDCACM */
