/*
 * usbd_cdc.c
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
*/

#include "hardware.h"

#if WITHUSBHW && WITHUSBCDCEEM

#include "formats.h"
#include "usb_core.h"

// CDC class-specific request codes
// (usbcdc11.pdf, 6.2, Table 46)
// see Table 45 for info about the specific requests.
#define CDC_SET_CONTROL_LINE_STATE              0x22

static unsigned cdceem1buffinlevel;
static USBALIGN_BEGIN uint8_t cdceem1buffout [USBD_CDCEEM_BUFSIZE] USBALIGN_END;
static USBALIGN_BEGIN uint8_t cdceem1buffin [USBD_CDCEEM_BUFSIZE] USBALIGN_END;
static RAMDTCM uint_fast16_t cdceem1buffinlevel;
static USBALIGN_BEGIN uint8_t cdceem_epXdatabuffout [USB_OTG_MAX_EP0_SIZE] USBALIGN_END;


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

/*
* @param  epnum: endpoint index without direction bit
*
 */
static USBD_StatusTypeDef USBD_CDCEEM_DataIn(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	//PRINTF("USBD_CDCEEM_DataIn: epnum=%d\n", (int) epnum);
	switch (epnum)
	{
	case (USBD_EP_CDCEEM_IN & 0x7F):
		USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), cdceem1buffin, cdceem1buffinlevel);
		cdceem1buffinlevel = 0;
		break;
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDCEEM_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	switch (epnum)
	{
	case USBD_EP_CDCEEM_OUT:
		/* EEM EP OUT */
		// use CDC data
		//cdc1out_buffer_save(cdc1buffout, USBD_LL_GetRxDataSize(pdev, epnum));	/* использование буфера принятых данных */
		//memcpy(cdc1buffin, cdc1buffout, cdc1buffinlevel = USBD_LL_GetRxDataSize(pdev, epnum));
		/* Prepare Out endpoint to receive next cdc data packet */
		USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(epnum), cdceem1buffout, USBD_CDCEEM_BUFSIZE);
		break;
	default:
		break;
	}
	return USBD_OK;
}

// При возврате из этой функции в usbd_core.c происходит вызов USBD_CtlSendStatus
static USBD_StatusTypeDef USBD_CDCEEM_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	const USBD_SetupReqTypedef * const req = & pdev->request;

	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	//PRINTF(PSTR("1 USBD_CDC_EP0_RxReady: interfacev=%u: bRequest=%u, wLength=%u\n"), interfacev, req->bRequest, req->wLength);
	switch (interfacev)
	{
	case INTERFACE_CDCEEM_DATA_6:	// CDC EEM interface
		{
			switch (req->bRequest)
			{
//			case CDC_SET_LINE_CODING:
//				{
////					const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
////					ASSERT(req->wLength == 7);
////					dwDTERate [interfacev] = USBD_peek_u32(& cdceem_epXdatabuffout [0]);
//					//PRINTF(PSTR("USBD_CDCEEM_EP0_RxReady: CDC_SET_LINE_CODING: interfacev=%u, dwDTERate=%lu, bits=%u\n"), interfacev, dwDTERate [interfacev], cdceem_epXdatabuffout [6]);
//				}
//				break;
			default:
				// непонятно, для чего эти данные?
				TP();
				break;
			}
		}
		break;

	default:
		break;
	}
	return USBD_OK;
}


static USBD_StatusTypeDef USBD_CDCEEM_Setup(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
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

			case INTERFACE_CDCEEM_DATA_6:	// CDC EEM interface
				{
					switch (req->bRequest)
					{
//					case CDC_GET_LINE_CODING:
//						PRINTF(PSTR("USBD_CDC_Setup IN: CDC_GET_LINE_CODING, dwDTERate=%lu\n"), (unsigned long) dwDTERate [interfacev]);
//						USBD_poke_u32(& buff [0], dwDTERate [interfacev]); // dwDTERate
//						buff [4] = 0;	// 1 stop bit
//						buff [5] = 0;	// parity=none
//						buff [6] = 8;	// bDataBits
//
//						USBD_CtlSendData(pdev, buff, ulmin16(7, req->wLength));
						break;

					default:
						TP();
						PRINTF(PSTR("USBD_CDCEEM_Setup IN: bRequest=%02X, wValue=%04X\n"), (unsigned) req->bRequest, (unsigned) req->wValue);
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

			case INTERFACE_CDCEEM_DATA_6:	// CDC EEM interface
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
			case INTERFACE_CDCEEM_DATA_6:	// CDC EEM interface
				switch (req->bRequest)
				{
				case CDC_SET_CONTROL_LINE_STATE:
					// Выполнение этого запроса не требует дополнительного чтения данных
					PRINTF(PSTR("USBD_CDCEEM_Setup OUT: CDC_SET_CONTROL_LINE_STATE, wValue=%04X, wLength=%04X\n"), req->wValue, (unsigned) req->wLength);
//					usb_cdc_control_state [interfacev] = req->wValue;
					ASSERT(req->wLength == 0);
					break;

				default:
					TP();
					PRINTF(PSTR("USBD_CDCEEM_Setup OUT: bRequest=%02X, wValue=%04X, wLength=%04X\n"), (unsigned) req->bRequest, (unsigned) req->wValue, (unsigned) req->wLength);
					break;
				}
				/* все запросы этого класса устройств */
				if (req->wLength != 0)
				{
					USBD_CtlPrepareRx(pdev, cdceem_epXdatabuffout, ulmin16(ARRAY_SIZE(cdceem_epXdatabuffout), req->wLength));
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
				case INTERFACE_CDCEEM_DATA_6:	// CDC EEM interface
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

static USBD_StatusTypeDef USBD_CDCEEM_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	/* CDC EEM Open EP IN */
	USBD_LL_OpenEP(pdev, USBD_EP_CDCEEM_IN, USBD_EP_TYPE_BULK, USBD_CDCEEM_BUFSIZE);
	USBD_LL_Transmit(pdev, USBD_EP_CDCEEM_IN, NULL, 0);
	/* CDC EEM Open EP OUT */
	USBD_LL_OpenEP(pdev, USBD_EP_CDCEEM_OUT, USBD_EP_TYPE_BULK, USBD_CDCEEM_BUFSIZE);
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDCEEM_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	USBD_LL_CloseEP(pdev, USBD_EP_CDCEEM_IN);
	USBD_LL_CloseEP(pdev, USBD_EP_CDCEEM_OUT);
	return USBD_OK;
}

static void USBD_CDCEEM_ColdInit(void)
{
}

const USBD_ClassTypeDef USBD_CLASS_EEM =
{
	USBD_CDCEEM_ColdInit,
	USBD_CDCEEM_Init,	// Init
	USBD_CDCEEM_DeInit,	// DeInit
	USBD_CDCEEM_Setup,		// Setup
	NULL,	// EP0_TxSent
	USBD_CDCEEM_EP0_RxReady,	// EP0_RxReady
	USBD_CDCEEM_DataIn,	// DataIn
	USBD_CDCEEM_DataOut,	// DataOut
	NULL,	//USBD_XXX_SOF,	// SOF
	NULL,	//USBD_XXX_IsoINIncomplete,	// IsoINIncomplete
	NULL,	//USBD_XXX_IsoOUTIncomplete,	// IsoOUTIncomplete
};

#endif /* WITHUSBHW && WITHUSBCDCEEM */

