/*
 * usbd_audio.c
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
*/

#include "hardware.h"
#include "pio.h"
#include "board.h"
#include "audio.h"
#include "spifuncs.h"

#include "display/display.h"
#include "formats.h"
#include <string.h>

#if WITHUSBHW && WITHUSBUAC

#include "usb_core.h"


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

/* получить 32-бит значение */
static uint_fast32_t
USBD_peek_u32(
	const uint8_t * buff
	)
{
	return
		((uint_fast32_t) buff [3] << 24) +
		((uint_fast32_t) buff [2] << 16) +
		((uint_fast32_t) buff [1] << 8) +
		((uint_fast32_t) buff [0] << 0);
}

/* записать в буфер для ответа 32-бит значение */
static void USBD_poke_u32(uint8_t * buff, uint_fast32_t v)
{
	buff [0] = LO_BYTE(v);
	buff [1] = HI_BYTE(v);
	buff [2] = HI_24BY(v);
	buff [3] = HI_32BY(v);
}

/* записать в буфер для ответа 24-бит значение */
static void USBD_poke_u24(uint8_t * buff, uint_fast32_t v)
{
	buff [0] = LO_BYTE(v);
	buff [1] = HI_BYTE(v);
	buff [2] = HI_24BY(v);
}

/* записать в буфер для ответа 16-бит значение */
static void USBD_poke_u16(uint8_t * buff, uint_fast16_t v)
{
	buff [0] = LO_BYTE(v);
	buff [1] = HI_BYTE(v);
}

/* записать в буфер для ответа 8-бит значение */
static void USBD_poke_u8(uint8_t * buff, uint_fast8_t v)
{
	buff [0] = v;
}

// Fill Layout 1 Parameter Block
static unsigned USBD_fill_range_lay1pb(uint8_t * b, uint_fast8_t v)
{
/*
	If a subrange consists of only a single value,
	the corresponding triplet must contain that value for
	both its MIN and MAX subattribute
	and the RES subattribute must be set to zero.
*/

	USBD_poke_u16(b + 0, 1);	// number of subranges
	USBD_poke_u8(b + 2, v);	// MIN
	USBD_poke_u8(b + 3, v);	// MAX
	USBD_poke_u8(b + 4, 0);	// RES

	return 5;
}

// Fill Layout 2 Parameter Block
static unsigned USBD_fill_range_lay2pb(uint8_t * b, uint_fast16_t v)
{
/*
	If a subrange consists of only a single value,
	the corresponding triplet must contain that value for
	both its MIN and MAX subattribute
	and the RES subattribute must be set to zero.
*/

	USBD_poke_u16(b + 0, 1);	// number of subranges
	USBD_poke_u16(b + 2, v);	// MIN
	USBD_poke_u16(b + 3, v);	// MAX
	USBD_poke_u16(b + 4, 0);	// RES

	return 8;
}

// Fill Layout 3 Parameter Block
static unsigned USBD_fill_range_lay3pb(uint8_t * b, uint_fast32_t v)
{
/*
	If a subrange consists of only a single value,
	the corresponding triplet must contain that value for
	both its MIN and MAX subattribute
	and the RES subattribute must be set to zero.
*/

	USBD_poke_u16(b + 0, 1);	// number of subranges
	USBD_poke_u32(b + 2, v);	// MIN
	USBD_poke_u32(b + 6, v);	// MAX
	USBD_poke_u32(b + 10, 0);	// RES

	return 14;
}

// Fill Layout 3 Parameter Block
// with two discrete options
static unsigned USBD_fill_range_lay3pb2opt(uint8_t * b, uint_fast32_t v1, uint_fast32_t v2)
{
/*
	If a subrange consists of only a single value,
	the corresponding triplet must contain that value for
	both its MIN and MAX subattribute
	and the RES subattribute must be set to zero.
*/

	USBD_poke_u16(b + 0, 2);	// number of subranges
	USBD_poke_u32(b + 2, v1);	// MIN
	USBD_poke_u32(b + 6, v1);	// MAX
	USBD_poke_u32(b + 10, 0);	// RES
	USBD_poke_u32(b + 14, v2);	// MIN
	USBD_poke_u32(b + 18, v2);	// MAX
	USBD_poke_u32(b + 22, 0);	// RES

	return 26;
}




uint_fast16_t usbd_getuacinmaxpacket(void)
{
	uint_fast16_t maxpacket = VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_AUDIO48;

#if ! WITHUSBUACIN2
	#if WITHRTS96
		maxpacket = ulmax16(maxpacket, VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS96);
	#endif /* WITHRTS96 */
	#if WITHRTS192
		maxpacket = ulmax16(maxpacket, VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS192);
	#endif /* WITHRTS192 */
#endif /* ! WITHUSBUACIN2 */
	return maxpacket;
}

#if WITHUSBUACIN2

uint_fast16_t usbd_getuacinrtsmaxpacket(void)
{
	uint_fast16_t maxpacket = 64;
#if WITHRTS96
	maxpacket = ulmax16(maxpacket, VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS96);
#endif /* WITHRTS96 */
#if WITHRTS192
	maxpacket = ulmax16(maxpacket, VIRTUAL_AUDIO_PORT_DATA_SIZE_IN_RTS192);
#endif /* WITHRTS192 */
	return maxpacket;
}

#endif /* WITHUSBUACIN2 */



// Состояние - выбранные альтернативные конфигурации по каждому интерфейсу USB configuration descriptor
static RAMDTCM uint8_t altinterfaces [INTERFACE_count];



static RAMDTCM uint8_t terminalsprops [256] [16];

static RAMDTCM uintptr_t uacinaddr = 0;
static RAMDTCM uint_fast16_t uacinsize = 0;
static RAMDTCM uintptr_t uacinrtsaddr = 0;
static RAMDTCM uint_fast16_t uacinrtssize = 0;

static USBALIGN_BEGIN uint8_t uacoutbuff [VIRTUAL_AUDIO_PORT_DATA_SIZE_OUT] USBALIGN_END;

static USBALIGN_BEGIN uint8_t uac_ep0databuffout [USB_OTG_MAX_EP0_SIZE] USBALIGN_END;


static USBD_StatusTypeDef USBD_UAC_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	uint_fast8_t offset;
	//PRINTF(PSTR("USBD_UAC_DeInit\n"));


	{
		USBD_LL_CloseEP(pdev, USBD_EP_AUDIO_IN);

		if (uacinaddr != 0)
		{
			global_disableIRQ();
			release_dmabufferx(uacinaddr);
			global_enableIRQ();
			uacinaddr = 0;
		}
		altinterfaces [INTERFACE_AUDIO_MIKE] = 0;
		altinterfaces [INTERFACE_AUDIO_SPK] = 0;
		buffers_set_uacinalt(altinterfaces [INTERFACE_AUDIO_MIKE]);

#if WITHUSBUACIN2
		USBD_LL_CloseEP(pdev, USBD_EP_RTS_IN);
		if (uacinrtsaddr != 0)
		{
			global_disableIRQ();
			release_dmabufferxrts(uacinrtsaddr);
			global_enableIRQ();
			uacinrtsaddr = 0;
		}
		altinterfaces [INTERFACE_AUDIO_RTS] = 0;
		buffers_set_uacinrtsalt(altinterfaces [INTERFACE_AUDIO_RTS]);
#endif /* WITHUSBUACIN2 */

		USBD_LL_CloseEP(pdev, USBD_EP_AUDIO_OUT);
		terminalsprops [TERMINAL_ID_SELECTOR_6] [AUDIO_CONTROL_UNDEFINED] = 1;
		buffers_set_uacoutalt(altinterfaces [INTERFACE_AUDIO_SPK]);
		uacout_buffer_stop();
	}

	//PRINTF(PSTR("USBD_XXX_DeInit done\n"));
	return USBD_OK;
}


// UAC1: Выполнение запроса к FeatureUnit UAC1
// see UAC1_AudioFeatureUnit
static unsigned USBD_UAC1_FeatureUnit_req(
	const USBD_SetupReqTypedef *req,
	uint8_t * buff
	)
{
	const uint_fast8_t terminalID = HI_BYTE(req->wIndex);
	const uint_fast8_t controlID = HI_BYTE(req->wValue);	// AUDIO_MUTE_CONTROL, AUDIO_VOLUME_CONTROL, ...
	//const uint_fast8_t channelNumber = LO_BYTE(req->wValue);
	switch (req->bRequest)
	{
	default:
		// Undefined request
		TP();
		return 0;

	case AUDIO_REQUEST_GET_CUR:
		//PRINTF(PSTR("USBD_UAC1_FeatureUnit_req: AUDIO_REQUEST_GET_CUR: interfacev=%u,  %u\n"), interfacev, terminalID);
		buff [0] = terminalsprops [terminalID] [controlID];
		return 1;

	case AUDIO_REQUEST_GET_MIN:
		//PRINTF(PSTR("USBD_UAC1_FeatureUnit_req: AUDIO_REQUEST_GET_MIN: interfacev=%u,  %u\n"), interfacev, terminalID);
		buff [0] = 0;
		return 1;

	case AUDIO_REQUEST_GET_MAX:
		//PRINTF(PSTR("USBD_UAC1_FeatureUnit_req: AUDIO_REQUEST_GET_MAX: interfacev=%u,  %u\n"), interfacev, terminalID);
		buff [0] = 100;
		return 1;

	case AUDIO_REQUEST_GET_RES:
		//PRINTF(PSTR("USBD_UAC1_FeatureUnit_req: AUDIO_REQUEST_GET_MAX: interfacev=%u,  %u\n"), interfacev, terminalID);
		buff [0] = 1;
		return 1;
	}
}

// UAC1: Выполнение запроса к Selector UAC1
// see UAC1_AudioFeatureUnit
static unsigned USBD_UAC1_Selector_req(
	const USBD_SetupReqTypedef *req,
	uint8_t * buff
	)
{
	const uint_fast8_t terminalID = HI_BYTE(req->wIndex);
	const uint_fast8_t controlID = HI_BYTE(req->wValue);	// AUDIO_MUTE_CONTROL, AUDIO_VOLUME_CONTROL, ...
	//const uint_fast8_t channelNumber = LO_BYTE(req->wValue);
	switch (req->bRequest)
	{
	default:
		// Undefined request
		TP();
		return 0;

	case AUDIO_REQUEST_GET_CUR:
		//PRINTF(PSTR("USBD_UAC1_Selector_req: AUDIO_REQUEST_GET_CUR: interfacev=%u,  %u\n"), interfacev, terminalID);
		buff [0] = terminalsprops [terminalID] [controlID];
		return 1;

	case AUDIO_REQUEST_GET_MIN:
		//PRINTF(PSTR("USBD_UAC1_Selector_req: AUDIO_REQUEST_GET_MIN: interfacev=%u,  %u\n"), interfacev, terminalID);
		buff [0] = 1;
		return 1;

	case AUDIO_REQUEST_GET_MAX:
		//PRINTF(PSTR("USBD_UAC1_Selector_req: AUDIO_REQUEST_GET_MAX: interfacev=%u,  %u\n"), interfacev, terminalID);
		buff [0] = TERMINAL_ID_SELECTOR_6_INPUTS;
		return 1;

	case AUDIO_REQUEST_GET_RES:
		//PRINTF(PSTR("USBD_UAC1_Selector_req: AUDIO_REQUEST_GET_MAX: interfacev=%u,  %u\n"), interfacev, terminalID);
		buff [0] = 1;
		return 1;
	}
}

// UAC2: Выполнение запроса FeatureUnit UAC2
// see UAC2_AudioFeatureUnit
static unsigned USBD_UAC2_FeatureUnit_req(
	const USBD_SetupReqTypedef *req,
	uint8_t * buff
	)
{
	const uint_fast8_t terminalID = HI_BYTE(req->wIndex);
	const uint_fast8_t controlID = HI_BYTE(req->wValue);	// AUDIO_MUTE_CONTROL, AUDIO_VOLUME_CONTROL, ...
	const uint_fast8_t channelNumber = LO_BYTE(req->wValue);
	//PRINTF(PSTR("1 req->bRequest=%u: interfacev=%u,  controlID=%u, channelNumber=%u, terminalID=%u\n"), req->bRequest, interfacev, controlID, channelNumber, terminalID);
	switch (req->bRequest)
	{
	default:
		// Undefined request
		TP();
		return 0;

	case 0x01:	// CURR
		switch (controlID)
		{
		case 1:
			// MUTE
			USBD_poke_u8(buff, 0);
			return 1;
		case 2:
			// VOLUME
			USBD_poke_u16(buff, 0);
			return 2;
		default:
			// Undefined control ID
			TP();
			return 0;
		}
		break;

	case 0x02:	// RANGE
		switch (controlID)
		{
		case 1:
			// MUTE
			return USBD_fill_range_lay1pb(buff, 0);
		case 2:
			// VOLUME
			return USBD_fill_range_lay2pb(buff, 0);
		default:
			// Undefined control ID
			TP();
			return 0;
		}
		break;
	}
	return 0;
}

// UAC2: Выполнение запроса CURR/RANGE
static unsigned USBD_UAC2_ClockSource_req(
	const USBD_SetupReqTypedef *req,
	uint8_t * buff
	)
{
	const uint_fast8_t terminalID = HI_BYTE(req->wIndex);
	const uint_fast8_t controlID = HI_BYTE(req->wValue);
	const uint_fast8_t channelNumber = LO_BYTE(req->wValue);
	switch (req->bRequest)
	{
	case 0x01:	// CURR
		switch (terminalID)
		{
		default:
			// Undefined terminalID
			TP();
			return 0;

		case TERMINAL_ID_CLKSOURCE_UACIN48_UACINRTS:
		case TERMINAL_ID_CLKSOURCE_UACINOUT:
			switch (controlID)
			{
			default:
				// Undefined control ID
				TP();
				return 0;
			case 1:
				// FREQ
				// todo: depend on mode
				USBD_poke_u32(buff + 0, dsp_get_samplerateuacin_audio48()); // sample rate
				return 4;
			case 2:
				// VALID
				USBD_poke_u8(buff + 0, 1); // valid
				return 1;
			}
			break;
		case TERMINAL_ID_CLKSOURCE_UACINRTS:
			switch (controlID)
			{
			default:
				// Undefined control ID
				TP();
				return 0;
			case 1:
				// FREQ
				USBD_poke_u32(buff + 0, dsp_get_samplerateuacin_rts()); // sample rate
				return 4;
			case 2:
				// VALID
				USBD_poke_u8(buff + 0, 1); // valid
				return 1;
			}
			break;
		case TERMINAL_ID_CLKSOURCE_UACIN48:
		case TERMINAL_ID_CLKSOURCE_UACOUT48:
			switch (controlID)
			{
			default:
				// Undefined control ID
				TP();
				return 0;
			case 1:
				// FREQ
				USBD_poke_u32(buff + 0, dsp_get_samplerateuacin_audio48()); // sample rate
				return 4;
			case 2:
				// VALID
				USBD_poke_u8(buff + 0, 1); // valid
				return 1;
			}
			break;
		}
		break;
	case 0x02:	// RANGE
		// The Clock Validity Control must have only the CUR attribute
		switch (terminalID)
		{
		default:
			// Undefined terminalID
			TP();
			return 0;

		case TERMINAL_ID_CLKSOURCE_UACIN48_UACINRTS:
		case TERMINAL_ID_CLKSOURCE_UACINOUT:
			switch (controlID)
			{
			default:
				// Undefined control ID
				TP();
				return 0;
			case 1:
				// FREQ
				return USBD_fill_range_lay3pb2opt(buff, dsp_get_samplerateuacin_audio48(), dsp_get_samplerateuacin_rts());
			}
			break;
		case TERMINAL_ID_CLKSOURCE_UACINRTS:
			switch (controlID)
			{
			default:
				// Undefined control ID
				TP();
				return 0;
			case 1:
				// FREQ
				return USBD_fill_range_lay3pb(buff, dsp_get_samplerateuacin_rts());
			}
			break;
		case TERMINAL_ID_CLKSOURCE_UACIN48:
		case TERMINAL_ID_CLKSOURCE_UACOUT48:
			switch (controlID)
			{
			default:
				// Undefined control ID
				TP();
				return 0;
			case 1:
				// FREQ
				return USBD_fill_range_lay3pb(buff, dsp_get_samplerateuacin_audio48());
			}
			break;
		}
		break;
	}
	return 0;
}



static USBD_StatusTypeDef USBD_UAC_Setup(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	static USBALIGN_BEGIN uint8_t buff [32] USBALIGN_END;	// was: 7
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	//PRINTF(PSTR("USBD_UAC_Setup: bRequest=%02X, wIndex=%04X, wLength=%04X, wValue=%04X (interfacev=%02X)\n"), req->bRequest, req->wIndex, req->wLength, req->wValue, interfacev);
	unsigned len = 0;
	if ((req->bmRequest & USB_REQ_TYPE_DIR) != 0)
	{
		// IN direction
		switch (req->bmRequest & USB_REQ_TYPE_MASK)
		{
		case USB_REQ_TYPE_CLASS:
			switch (interfacev)
			{
		#if WITHUSBUACIN2
			case INTERFACE_AUDIO_CONTROL_RTS:	/* AUDIO spectrum control interface */
		#endif /* WITHUSBUACIN2 */
			case INTERFACE_AUDIO_CONTROL_MIKE:	// AUDIO control interface
			case INTERFACE_AUDIO_CONTROL_SPK:	// AUDIO control interface
				{
					const uint_fast8_t terminalID = HI_BYTE(req->wIndex);
					const uint_fast8_t controlID = HI_BYTE(req->wValue);	// AUDIO_MUTE_CONTROL, AUDIO_VOLUME_CONTROL, ...
					const uint_fast8_t channelNumber = LO_BYTE(req->wValue);
					switch (terminalID)
					{
					default:
						TP();
						PRINTF(PSTR("USBD_UAC_Setup IN: default path 1: req->bRequest=%02X\n"), req->bRequest);
						len = 0;
						break;
					case TERMINAL_ID_CLKSOURCE_UACIN48_UACINRTS:
					case TERMINAL_ID_CLKSOURCE_UACINOUT:
					case TERMINAL_ID_CLKSOURCE_UACINRTS:
					case TERMINAL_ID_CLKSOURCE_UACIN48:
					case TERMINAL_ID_CLKSOURCE_UACOUT48:
						len = USBD_UAC2_ClockSource_req(req, buff);
						break;

					case TERMINAL_ID_FU2_IN + 0 * MAX_TERMINALS_IN_INTERFACE:
					case TERMINAL_ID_FU2_IN + 1 * MAX_TERMINALS_IN_INTERFACE:
					case TERMINAL_ID_FU2_IN + 2 * MAX_TERMINALS_IN_INTERFACE:
					case TERMINAL_ID_FU2_OUT + 0 * MAX_TERMINALS_IN_INTERFACE:
					case TERMINAL_ID_FU2_OUT + 1 * MAX_TERMINALS_IN_INTERFACE:
					case TERMINAL_ID_FU2_OUT + 2 * MAX_TERMINALS_IN_INTERFACE:
						len = USBD_UAC2_FeatureUnit_req(req, buff);
						break;

					case TERMINAL_ID_FU1_IN + 0 * MAX_TERMINALS_IN_INTERFACE:
					case TERMINAL_ID_FU1_IN + 1 * MAX_TERMINALS_IN_INTERFACE:
					case TERMINAL_ID_FU1_IN + 2 * MAX_TERMINALS_IN_INTERFACE:
					case TERMINAL_ID_FU1_OUT + 0 * MAX_TERMINALS_IN_INTERFACE:
					case TERMINAL_ID_FU1_OUT + 1 * MAX_TERMINALS_IN_INTERFACE:
					case TERMINAL_ID_FU1_OUT + 2 * MAX_TERMINALS_IN_INTERFACE:
						len = USBD_UAC1_FeatureUnit_req(req, buff);
						break;

					case TERMINAL_ID_SELECTOR_6:
						len = USBD_UAC1_Selector_req(req, buff);
						break;
					}
					ASSERT(len != 0);
					ASSERT(req->wLength != 0);
					USBD_CtlSendData(pdev, buff, ulmin16(len, req->wLength));
					break;
				} // audio interfaces
				break;

			default:
				// Other interfaces
				break;
			}
			break;
		case USB_REQ_TYPE_STANDARD:
			switch (req->bRequest)
			{
			case USB_REQ_GET_INTERFACE :
				PRINTF(PSTR("USBD_UAC_Setup IN: USB_REQ_TYPE_STANDARD USB_REQ_GET_INTERFACE dir=%02X interfacev=%d\n"), req->bmRequest & 0x80, interfacev);
				switch (interfacev)
				{

			#if WITHUSBUACIN2
				case INTERFACE_AUDIO_CONTROL_RTS:	/* AUDIO spectrum control interface */
			#endif /* WITHUSBUACIN2 */
				case INTERFACE_AUDIO_CONTROL_MIKE:	// AUDIO control interface
				case INTERFACE_AUDIO_CONTROL_SPK:	// AUDIO control interface
					buff [0] = altinterfaces [interfacev];
					USBD_CtlSendData(pdev, buff, ulmin16(ARRAY_SIZE(buff), req->wLength));
					break;

				default:
					// Other interfaces
					break;
				}
				break;
			}
			break;

		default:
			TP();
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

		#if WITHUSBUACIN2
			case INTERFACE_AUDIO_CONTROL_RTS:	/* AUDIO spectrum control interface */
		#endif /* WITHUSBUACIN2 */
			case INTERFACE_AUDIO_CONTROL_MIKE:	// AUDIO control interface
			case INTERFACE_AUDIO_CONTROL_SPK:	// AUDIO control interface
				switch (req->bRequest)
				{
				default:
					break;
				}
				/* все запросы этого класса устройств */
				if (req->wLength != 0)
				{
					USBD_CtlPrepareRx (pdev, uac_ep0databuffout, ulmin16(ARRAY_SIZE(uac_ep0databuffout), req->wLength));
				}
				else
				{
					USBD_CtlSendStatus(pdev);
				}
				break;

			default:
				// Other interfaces
				break;

			}
			break;

		case USB_REQ_TYPE_STANDARD:
			switch (req->bRequest)
			{
			case USB_REQ_SET_INTERFACE :
				switch (interfacev)
				{
				case INTERFACE_AUDIO_MIKE: // Audio interface: recording device
					//PRINTF(PSTR("USBD_UAC_Setup: USB_REQ_TYPE_STANDARD USB_REQ_SET_INTERFACE INTERFACE_AUDIO_MIKE interfacev=%d, value=%d\n"), interfacev, LO_BYTE(req->wValue));
					altinterfaces [interfacev] = LO_BYTE(req->wValue);
					buffers_set_uacinalt(altinterfaces [interfacev]);
					USBD_CtlSendStatus(pdev);
					break;
				case INTERFACE_AUDIO_SPK:	// DATA OUT Audio interface: playback device
					//PRINTF(PSTR("USBD_UAC_Setup: USB_REQ_TYPE_STANDARD USB_REQ_SET_INTERFACE INTERFACE_AUDIO_SPK interfacev=%d, value=%d\n"), interfacev, LO_BYTE(req->wValue));
					altinterfaces [interfacev] = LO_BYTE(req->wValue);
					buffers_set_uacoutalt(altinterfaces [interfacev]);
					USBD_CtlSendStatus(pdev);
					break;
			#if WITHUSBUACIN2
				case INTERFACE_AUDIO_RTS: // Audio interface: recording device
					//PRINTF(PSTR("USBD_UAC_Setup: USB_REQ_TYPE_STANDARD USB_REQ_SET_INTERFACE INTERFACE_AUDIO_RTS interfacev=%d, value=%d\n"), interfacev, LO_BYTE(req->wValue));
					altinterfaces [interfacev] = LO_BYTE(req->wValue);
					buffers_set_uacinrtsalt(altinterfaces [interfacev]);
					USBD_CtlSendStatus(pdev);
					break;
			#endif /* WITHUSBUACIN2 */

				default:
					// Other interfaces
					break;
				}
			}
			break;
		}
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_UAC_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	switch (epnum)
	{
	case USBD_EP_AUDIO_OUT:
		/* UAC EP OUT */
		// use audio data
		uacout_buffer_save(uacoutbuff, USBD_LL_GetRxDataSize(pdev, epnum));
		/* Prepare Out endpoint to receive next audio data packet */
		USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(epnum), uacoutbuff, VIRTUAL_AUDIO_PORT_DATA_SIZE_OUT);
		break;
	}
	return USBD_OK;
}
static USBD_StatusTypeDef USBD_UAC_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	const USBD_SetupReqTypedef * const req = & pdev->request;

	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	//PRINTF(PSTR("1 USBD_XXX_EP0_RxReady: interfacev=%u: bRequest=%u, wLength=%u\n"), interfacev, req->bRequest, req->wLength);
	switch (interfacev)
	{
#if WITHUSBUACIN2
	case INTERFACE_AUDIO_CONTROL_RTS:	/* AUDIO spectrum control interface */
#endif /* WITHUSBUACIN2 */
	case INTERFACE_AUDIO_CONTROL_MIKE:	// AUDIO control interface
	case INTERFACE_AUDIO_CONTROL_SPK:	// AUDIO control interface
		{
			switch (req->bRequest)
			{
			case AUDIO_REQUEST_SET_CUR:
				{
					const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
					const uint_fast8_t terminalID = HI_BYTE(req->wIndex);
					const uint_fast8_t controlID = HI_BYTE(req->wValue);	// AUDIO_MUTE_CONTROL, AUDIO_VOLUME_CONTROL, ...
					terminalsprops [terminalID] [controlID] = uac_ep0databuffout [0];
					//PRINTF(PSTR("USBD_XXX_EP0_RxReady: AUDIO_REQUEST_SET_CUR: interfacev=%u, %u=%u\n"), interfacev, terminalID, uac_ep0databuffout [0]);
				}
				break;
			default:
				{
					const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
					const uint_fast8_t terminalID = HI_BYTE(req->wIndex);
					const uint_fast8_t controlID = HI_BYTE(req->wValue);	// AUDIO_MUTE_CONTROL, AUDIO_VOLUME_CONTROL, ...
					//PRINTF(PSTR("USBD_XXX_EP0_RxReady: request=%u: interfacev=%u, %u=%u\n"), req->bRequest, interfacev, terminalID, uac_ep0databuffout [0]);
				}
				break;
			}
		}
		break;

	default:
		// Other interfaces
		break;
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_UAC_DataIn(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	// epnum without direction bit
	//PRINTF(PSTR("USBD_LL_DataInStage: IN: epnum=%02X\n"), epnum);
	switch (epnum)
	{
	case ((USBD_EP_AUDIO_IN) & 0x7F):
		if (uacinaddr != 0)
		{
			global_disableIRQ();
			release_dmabufferx(uacinaddr);
			global_enableIRQ();
		}

		global_disableIRQ();
		uacinaddr = getfilled_dmabufferx(& uacinsize);
		global_enableIRQ();

		if (uacinaddr != 0)
		{
			USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), (const uint8_t *) uacinaddr, uacinsize);
		}
		else
		{
			USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), NULL, 0);
		}
		break;
#if WITHUSBUACIN2
	case ((USBD_EP_RTS_IN) & 0x7F):
		if (uacinrtsaddr != 0)
		{
			global_disableIRQ();
			release_dmabufferxrts(uacinrtsaddr);
			global_enableIRQ();
		}

		global_disableIRQ();
		uacinrtsaddr = getfilled_dmabufferxrts(& uacinrtssize);
		global_enableIRQ();

		if (uacinrtsaddr != 0)
		{
			USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), (const uint8_t *) uacinrtsaddr, uacinrtssize);
		}
		else
		{
			USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), NULL, 0);
		}
		break;
#endif /* WITHUSBUACIN2 */
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_UAC_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	//PRINTF(PSTR("USBD_XXX_Init: cfgidx=%d\n"), cfgidx);
	altinterfaces [INTERFACE_AUDIO_MIKE] = 0;
	altinterfaces [INTERFACE_AUDIO_SPK] = 0;
	terminalsprops [TERMINAL_ID_SELECTOR_6] [AUDIO_CONTROL_UNDEFINED] = 1;
	buffers_set_uacinalt(altinterfaces [INTERFACE_AUDIO_MIKE]);
	buffers_set_uacoutalt(altinterfaces [INTERFACE_AUDIO_SPK]);

	/* uac Open EP IN */
	USBD_LL_OpenEP(pdev, USBD_EP_AUDIO_IN, USBD_EP_TYPE_ISOC, usbd_getuacinmaxpacket());	// was: VIRTUAL_AUDIO_PORT_DATA_SIZE_IN
	USBD_LL_Transmit(pdev, USBD_EP_AUDIO_IN, NULL, 0);

#if WITHUSBUACIN2
	altinterfaces [INTERFACE_AUDIO_RTS] = 0;
	buffers_set_uacinrtsalt(altinterfaces [INTERFACE_AUDIO_RTS]);

	/* uac Open EP IN */
	USBD_LL_OpenEP(pdev, USBD_EP_RTS_IN, USBD_EP_TYPE_ISOC, usbd_getuacinrtsmaxpacket());	// was: VIRTUAL_AUDIO_PORT_DATA_SIZE_IN
	USBD_LL_Transmit(pdev, USBD_EP_RTS_IN, NULL, 0);

#endif /* WITHUSBUACIN2 */

	/* UAC Open EP OUT */
	USBD_LL_OpenEP(pdev,
				   USBD_EP_AUDIO_OUT,
				   USBD_EP_TYPE_ISOC,
				   VIRTUAL_AUDIO_PORT_DATA_SIZE_OUT);

   /* UAC Prepare Out endpoint to receive 1st packet */
	USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(USBD_EP_AUDIO_OUT), uacoutbuff, VIRTUAL_AUDIO_PORT_DATA_SIZE_OUT);

	uacout_buffer_start();
	return USBD_OK;
}

static void USBD_UAC_ColdInit(void)
{
	uacout_buffer_initialize();
	memset(altinterfaces, 0, sizeof altinterfaces);
}

const USBD_ClassTypeDef USBD_CLASS_UAC =
{
	USBD_UAC_ColdInit,
	USBD_UAC_Init,	// Init
	USBD_UAC_DeInit,	// DeInit
	USBD_UAC_Setup,		// Setup
	NULL,	//USBD_XXX_EP0_TxSent,	// EP0_TxSent
	USBD_UAC_EP0_RxReady,	//USBD_XXX_EP0_RxReady,	// EP0_RxReady
	USBD_UAC_DataIn,	// DataIn
	USBD_UAC_DataOut,	// DataOut
	NULL,	//USBD_XXX_SOF,	// SOF
	NULL,	//USBD_XXX_IsoINIncomplete,	// IsoINIncomplete
	NULL,	//USBD_XXX_IsoOUTIncomplete,	// IsoOUTIncomplete
};


#endif /* WITHUSBHW && WITHUSBUAC */
