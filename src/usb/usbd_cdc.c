/*
 * usbd_cdc.c
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
*/

#include "hardware.h"

#if WITHUSBHW && WITHUSBCDCACM

#include "board.h"
#include "audio.h"
#include "src/display/display.h"
#include "formats.h"
#include <string.h>

#include "gpio.h"
#include "spi.h"

#include "usb_core.h"


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

// Состояние - выбранные альтернативные конфигурации по каждому интерфейсу USB configuration descriptor
//static RAMBIGDTCM uint8_t altinterfaces [INTERFACE_count];

static RAMBIGDTCM volatile uint_fast16_t usb_cdc_control_state [INTERFACE_count];


static uint_fast32_t ulmin32(uint_fast32_t a, uint_fast32_t b)
{
	return a < b ? a : b;
}

static uint_fast32_t ulmax32(uint_fast32_t a, uint_fast32_t b)
{
	return a > b ? a : b;
}

static uint_fast16_t ulmin16(uint_fast16_t a, uint_fast16_t b)
{
	return a < b ? a : b;
}

static uint_fast16_t ulmax16(uint_fast16_t a, uint_fast16_t b)
{
	return a > b ? a : b;
}

static RAMBIGDTCM volatile uint_fast8_t usbd_cdcX_rxenabled [WITHUSBCDCACM_N];	/* виртуальный флаг разрешения прерывания по приёму символа - HARDWARE_CDC_ONRXCHAR */
static RAMBIGDTCM USBALIGN_BEGIN uint8_t cdcXbuffout [WITHUSBCDCACM_N] [VIRTUAL_COM_PORT_OUT_DATA_SIZE] USBALIGN_END;
static RAMBIGDTCM USBALIGN_BEGIN uint8_t cdcXbuffin [WITHUSBCDCACM_N] [VIRTUAL_COM_PORT_IN_DATA_SIZE] USBALIGN_END;
static RAMBIGDTCM uint_fast16_t cdcXbuffinlevel [WITHUSBCDCACM_N];

static RAMBIGDTCM USBALIGN_BEGIN uint8_t cdc_epXdatabuffout [USB_OTG_MAX_EP0_SIZE] USBALIGN_END;

static RAMBIGDTCM uint_fast32_t dwDTERate [INTERFACE_count];

/* управление по DTR происходит сразу, RTS только вместе со следующим DTR */
/* хранимое значение после получения CDC_SET_CONTROL_LINE_STATE */
/* Биты: RTS = 0x02, DTR = 0x01 */

// Обычно используется для переключения на передачу (PTT)
uint_fast8_t usbd_cdc1_getrts(void)
{
	return
		((usb_cdc_control_state [USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 0)] & CDC_ACTIVATE_CARRIER) != 0) ||
		0;
}

// Обычно используется для телеграфной манипуляции (KEYDOWN)
uint_fast8_t usbd_cdc1_getdtr(void)
{
	return
		((usb_cdc_control_state [USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 0)] & CDC_DTE_PRESENT) != 0) ||
		0;
}

// Обычно используется для переключения на передачу (PTT)
uint_fast8_t usbd_cdc2_getrts(void)
{
	const unsigned offset = 1;
	return
#if WITHUSBCDCACM_N > 1
		((usb_cdc_control_state [USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, offset)] & CDC_ACTIVATE_CARRIER) != 0) ||
#endif
		0;
}

// Обычно используется для телеграфной манипуляции (KEYDOWN)
uint_fast8_t usbd_cdc2_getdtr(void)
{
	const unsigned offset = 1;
	return
#if WITHUSBCDCACM_N > 1
		((usb_cdc_control_state [USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, offset)] & CDC_DTE_PRESENT) != 0) ||
#endif
		0;
}

static RAMBIGDTCM volatile uint_fast8_t usbd_cdc_txenabled [WITHUSBCDCACM_N];	/* виртуальный флаг разрешения прерывания по готовности передатчика - HARDWARE_CDC_ONTXCHAR*/

/* Разрешение/запрещение прерывания по передаче символа */
void usbd_cdc_enabletx(uint_fast8_t state)	/* вызывается из обработчика прерываний */
{
	const unsigned offset = 0;
	usbd_cdc_txenabled [offset] = state;
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void usbd_cdc_enablerx(uint_fast8_t state)	/* вызывается из обработчика прерываний */
{
	const unsigned offset = 0;
	usbd_cdcX_rxenabled [offset] = state;
}

/* передача символа после прерывания о готовности передатчика - вызывается из HARDWARE_CDC_ONTXCHAR */
void
usbd_cdc_tx(void * ctx, uint_fast8_t c)
{
	const unsigned offset = 0;
	USBD_HandleTypeDef * const pdev = ctx;
	(void) ctx;
	ASSERT(cdcXbuffinlevel  [offset] < VIRTUAL_COM_PORT_IN_DATA_SIZE);
	cdcXbuffin [offset] [cdcXbuffinlevel [offset] ++] = c;
}

/* использование буфера принятых данных */
static void cdcXout_buffer_save(
	const uint8_t * data,
	unsigned length,
	unsigned offset
	)
{
	unsigned i;
	//PRINTF("1:%u '%*.*s'", length, length, length, data);

	for (i = 0; usbd_cdcX_rxenabled [offset] && i < length; ++ i)
	{
		HARDWARE_CDC_ONRXCHAR(data [i]);
	}
}

static USBD_StatusTypeDef USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	if (epnum >= USBD_CDCACM_EP(USBD_EP_CDC_OUT, 0) && epnum < USBD_CDCACM_EP(USBD_EP_CDC_OUT, WITHUSBCDCACM_N))
	{
		const unsigned offset = USBD_CDCACM_OFFSET_BY_EP(epnum, USBD_EP_CDC_OUT);
		/* CDC EP OUT */
		// use CDC data
		cdcXout_buffer_save(cdcXbuffout [offset], USBD_LL_GetRxDataSize(pdev, epnum), offset);	/* использование буфера принятых данных */
		//memcpy(cdc1buffin, cdc1buffout, cdc1buffinlevel = USBD_LL_GetRxDataSize(pdev, epnum));
		/* Prepare Out endpoint to receive next cdc data packet */
		USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(epnum), cdcXbuffout [offset], VIRTUAL_COM_PORT_OUT_DATA_SIZE);
	}
	return USBD_OK;
}

// При возврате из этой функции в usbd_core.c происходит вызов USBD_CtlSendStatus
static USBD_StatusTypeDef USBD_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	const USBD_SetupReqTypedef * const req = & pdev->request;

	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	//PRINTF(PSTR("1 USBD_CDC_EP0_RxReady: interfacev=%u: bRequest=%u, wLength=%u\n"), interfacev, req->bRequest, req->wLength);
	if (interfacev >= USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 0) && interfacev < USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, WITHUSBCDCACM_N))
	{
		switch (req->bRequest)
		{
		case CDC_SET_LINE_CODING:
			{
				const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
				ASSERT(req->wLength == 7);
				dwDTERate [interfacev] = USBD_peek_u32(& cdc_epXdatabuffout [0]);
				//PRINTF(PSTR("USBD_CDC_EP0_RxReady: CDC_SET_LINE_CODING: interfacev=%u, dwDTERate=%lu, bits=%u\n"), interfacev, dwDTERate [interfacev], cdc_epXdatabuffout [6]);
			}
			break;
		default:
			// непонятно, для чего эти данные?
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
	//PRINTF("USBD_CDC_DataIn: epnum=%d\n", (int) epnum);
	if (epnum >= USBD_CDCACM_EP(USBD_EP_CDC_IN, 0) && epnum < USBD_CDCACM_EP(USBD_EP_CDC_IN, WITHUSBCDCACM_N))
	{
		const unsigned offset = USBD_CDCACM_OFFSET_BY_EP(epnum, USBD_EP_CDC_IN) & 0x7F;
#if 0
		// test usb tx fifo initialization
		#define TLENNNN (VIRTUAL_COM_PORT_IN_DATA_SIZE - 0)
		memset(cdcXbuffin [offset], '$', TLENNNN);
		USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), cdcXbuffin [offset], TLENNNN);
		break;
#endif
		while (usbd_cdc_txenabled [offset] && (cdcXbuffinlevel [offset] < ARRAY_SIZE(cdcXbuffin [offset])))
		{
			HARDWARE_CDC_ONTXCHAR(pdev);	// при отсутствии данных usbd_cdc_txenabled устанавливается в 0
		}
		USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), cdcXbuffin [offset], cdcXbuffinlevel [offset]);
		cdcXbuffinlevel [offset] = 0;
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDC_Setup(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	static USBALIGN_BEGIN uint8_t buff [32] USBALIGN_END;	// was: 7
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	if ((req->bmRequest & USB_REQ_TYPE_DIR) != 0)
	{
		// IN direction
		switch (req->bmRequest & USB_REQ_TYPE_MASK)
		{
		case USB_REQ_TYPE_CLASS:
			switch (interfacev)
			{
			case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 0):	// CDC control interface
#if WITHUSBCDCACM_N >= 2
			case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 1):	// CDC control interface
#endif /* WITHUSBCDCACM_N >= 2 */
#if WITHUSBCDCACM_N >= 3
			case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 2):	// CDC control interface
#endif /* WITHUSBCDCACM_N >= 3 */
#if WITHUSBCDCACM_N >= 4
			case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 3):	// CDC control interface
#endif /* WITHUSBCDCACM_N >= 4 */
				{
					switch (req->bRequest)
					{
					case CDC_GET_LINE_CODING:
						//PRINTF(PSTR("USBD_CDC_Setup IN: CDC_GET_LINE_CODING, dwDTERate=%lu\n"), (unsigned long) dwDTERate [interfacev]);
						USBD_poke_u32(& buff [0], dwDTERate [interfacev]); // dwDTERate
						buff [4] = 0;	// 1 stop bit
						buff [5] = 0;	// parity=none
						buff [6] = 8;	// bDataBits

						USBD_CtlSendData(pdev, buff, ulmin16(7, req->wLength));
						break;

					default:
						TP();
						USBD_CtlError(pdev, req);
						break;
					}
				}
				break;

			default:
				break;
			}
			break;

		case USB_REQ_TYPE_STANDARD:
			switch (interfacev)
			{
			case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 0):	// CDC control interface
			case USBD_CDCACM_IFC(INTERFACE_CDC_DATA_4a, 0):	// CDC data interface
#if WITHUSBCDCACM_N >= 2
			case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 1):	// CDC control interface
			case USBD_CDCACM_IFC(INTERFACE_CDC_DATA_4a, 1):	// CDC data interface
#endif /* WITHUSBCDCACM_N >= 2 */
#if WITHUSBCDCACM_N >= 3
			case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 2):	// CDC control interface
			case USBD_CDCACM_IFC(INTERFACE_CDC_DATA_4a, 2):	// CDC data interface
#endif /* WITHUSBCDCACM_N >= 3 */
#if WITHUSBCDCACM_N >= 4
			case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 3):	// CDC control interface
			case USBD_CDCACM_IFC(INTERFACE_CDC_DATA_4a, 3):	// CDC data interface
#endif /* WITHUSBCDCACM_N >= 4 */
				switch (req->bRequest)
				{
					case USB_REQ_GET_INTERFACE:
					{
						static USBALIGN_BEGIN uint8_t buff [64] USBALIGN_END;
						//PRINTF(PSTR("USBD_CDC_Setup: USB_REQ_TYPE_STANDARD USB_REQ_GET_INTERFACE dir=%02X interfacev=%d, req->wLength=%d\n"), req->bmRequest & 0x80, interfacev, (int) req->wLength);
						buff [0] = 0;
						USBD_CtlSendData(pdev, buff, ulmin16(ARRAY_SIZE(buff), req->wLength));
					}
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
			switch (interfacev)
			{
			case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 0):	// CDC control interface
#if WITHUSBCDCACM_N > 1
			case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 1):	// CDC control interface
#endif /* WITHUSBCDCACM_N > 1 */
#if WITHUSBCDCACM_N > 2
			case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 2):	// CDC control interface
#endif /* WITHUSBCDCACM_N > 2 */
#if WITHUSBCDCACM_N > 3
			case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 3):	// CDC control interface
#endif /* WITHUSBCDCACM_N > 3 */
				switch (req->bRequest)
				{
				case CDC_SET_CONTROL_LINE_STATE:
					// Выполнение этого запроса не требует дополнительного чтения данных
					PRINTF(PSTR("USBD_CDC_Setup OUT: CDC_SET_CONTROL_LINE_STATE, wValue=%04X\n"), req->wValue);
					usb_cdc_control_state [interfacev] = req->wValue;
					ASSERT(req->wLength == 0);
					break;

				default:
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
				break;

			default:
				break;
			}
			break;

		case USB_REQ_TYPE_STANDARD:
			switch (req->bRequest)
			{
			case USB_REQ_SET_INTERFACE:
				switch (interfacev)
				{
				case INTERFACE_CDC_CONTROL_3a:	// CDC control interface
				case INTERFACE_CDC_DATA_4a:	// CDC data interface
#if WITHUSBCDCACM_N >= 2
				case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 1):	// CDC control interface
				case USBD_CDCACM_IFC(INTERFACE_CDC_DATA_4a, 1):	// CDC data interface
#endif /* WITHUSBCDCACM_N >= 2 */
#if WITHUSBCDCACM_N >= 3
				case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 2):	// CDC control interface
				case USBD_CDCACM_IFC(INTERFACE_CDC_DATA_4a, 2):	// CDC data interface
#endif /* WITHUSBCDCACM_N >= 3 */
#if WITHUSBCDCACM_N >= 4
				case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, 3):	// CDC control interface
				case USBD_CDCACM_IFC(INTERFACE_CDC_DATA_4a, 3):	// CDC data interface
#endif /* WITHUSBCDCACM_N >= 4 */
					// Only zero value here
					//altinterfaces [interfacev] = LO_BYTE(req->wValue);
					//PRINTF("USBD_CDC_Setup: CDC interface %d set to %d\n", (int) interfacev, (int) altinterfaces [interfacev]);
					//bufers_set_cdcalt(altinterfaces [interfacev]);
					USBD_CtlSendStatus(pdev);
					break;

				default:
					// Другие интерфейсы - ничего не отправляем.
					//TP();
					break;
				}
			}
			break;

		default:
			break;
		}
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDC_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	uint_fast8_t offset;

    /* cdc Open EP IN */
 	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
	   USBD_LL_OpenEP(pdev,
			   	   	   USBD_CDCACM_EP(USBD_EP_CDC_IN, offset),
					   USBD_EP_TYPE_BULK,
					   VIRTUAL_COM_PORT_IN_DATA_SIZE);

 		USBD_LL_Transmit(pdev, USBD_CDCACM_EP(USBD_EP_CDC_IN, offset), NULL, 0);
	     /* cdc Open EP OUT */
		USBD_LL_OpenEP(pdev, USBD_CDCACM_EP(USBD_EP_CDC_OUT, offset), USBD_EP_TYPE_BULK, VIRTUAL_COM_PORT_OUT_DATA_SIZE);
		/* CDC Open EP interrupt */
		USBD_LL_OpenEP(pdev, USBD_CDCACM_EP(USBD_EP_CDC_INT, offset), USBD_EP_TYPE_INTR, VIRTUAL_COM_PORT_INT_SIZE);
	}

 	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
		/* CDC Prepare Out endpoint to receive 1st packet */
		USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(USBD_CDCACM_EP(USBD_EP_CDC_OUT, offset)), cdcXbuffout [offset],  VIRTUAL_COM_PORT_OUT_DATA_SIZE);
		usb_cdc_control_state [USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, offset)] = 0;
		dwDTERate [USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, offset)] = 115200;
	}

	return USBD_OK;

}

static USBD_StatusTypeDef USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	uint_fast8_t offset;

 	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
		USBD_LL_CloseEP(pdev, USBD_CDCACM_EP(USBD_EP_CDC_IN, offset));
		USBD_LL_CloseEP(pdev, USBD_CDCACM_EP(USBD_EP_CDC_OUT, offset));
		USBD_LL_CloseEP(pdev, USBD_CDCACM_EP(USBD_EP_CDC_INT, offset));
	}

	HARDWARE_CDC_ONDISCONNECT();

	/* при потере связи с host снять запрос на передачу */
 	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
 		usb_cdc_control_state [USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, offset)] = 0;
	}
	return USBD_OK;
}

static void USBD_CDC_ColdInit(void)
{
	uint_fast8_t offset;
 	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
 		usb_cdc_control_state [USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL_3a, offset)] = 0;
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

#endif /* WITHUSBHW && WITHUSBCDCACM */
