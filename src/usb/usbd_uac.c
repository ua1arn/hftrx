/*
 * usbd_audio.c
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
*/

#include "hardware.h"

#if WITHUSBHW && WITHUSBUAC

#include "board.h"
#include "audio.h"
#include "src/display/display.h"
#include "formats.h"

#include "usb_device.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usb200.h"
#include "usbch9.h"

#include <string.h>


// Fill Layout 1 Parameter Block
static unsigned USBD_fill_range_lay1pb(uint8_t * b, uint_fast8_t v)
{
	unsigned n = 0;
/*
	If a subrange consists of only a single value,
	the corresponding triplet must contain that value for
	both its MIN and MAX subattribute
	and the RES subattribute must be set to zero.
*/

	n += USBD_poke_u16(b + n, 1);	// number of subranges
	n += USBD_poke_u8(b + n, v);	// MIN
	n += USBD_poke_u8(b + n, v);	// MAX
	n += USBD_poke_u8(b + n, 0);	// RES

	return n;
}

// Fill Layout 2 Parameter Block
static unsigned USBD_fill_range_lay2pb(uint8_t * b, uint_fast16_t v)
{
	unsigned n = 0;
/*
	If a subrange consists of only a single value,
	the corresponding triplet must contain that value for
	both its MIN and MAX subattribute
	and the RES subattribute must be set to zero.
*/

	n += USBD_poke_u16(b + n, 1);	// number of subranges
	n += USBD_poke_u16(b + n, v);	// MIN
	n += USBD_poke_u16(b + n, v);	// MAX
	n += USBD_poke_u16(b + n, 0);	// RES

	return n;
}

// Fill Layout 3 Parameter Block
static unsigned USBD_fill_range_lay3pb(uint8_t * b, uint_fast32_t v)
{
	unsigned n = 0;
/*
	If a subrange consists of only a single value,
	the corresponding triplet must contain that value for
	both its MIN and MAX subattribute
	and the RES subattribute must be set to zero.
*/

	n += USBD_poke_u16(b + n, 1);	// number of subranges
	n += USBD_poke_u32(b + n, v);	// MIN
	n += USBD_poke_u32(b + n, v);	// MAX
	n += USBD_poke_u32(b + n, 0);	// RES

	return n;
}

// Fill Layout 3 Parameter Block
// with two discrete options
static unsigned USBD_fill_range_lay3pb2opt(uint8_t * b, uint_fast32_t v1, uint_fast32_t v2)
{
	unsigned n = 0;
/*
	If a subrange consists of only a single value,
	the corresponding triplet must contain that value for
	both its MIN and MAX subattribute
	and the RES subattribute must be set to zero.
*/

	n += USBD_poke_u16(b + n, 2);	// number of subranges
	n += USBD_poke_u32(b + n, v1);	// MIN
	n += USBD_poke_u32(b + n, v1);	// MAX
	n += USBD_poke_u32(b + n, 0);	// RES
	n += USBD_poke_u32(b + n, v2);	// MIN
	n += USBD_poke_u32(b + n, v2);	// MAX
	n += USBD_poke_u32(b + n, 0);	// RES

	return n;
}

uint_fast16_t usbd_getuacinmaxpacket(void)
{
	uint_fast16_t maxpacket = UACIN_AUDIO48_DATASIZE;

#if ! WITHUSBUACIN2 || WITHUSBUACINOUTRENESAS
	#if WITHRTS96
		maxpacket = ulmax16(maxpacket, UACIN_RTS96_DATASIZE);
	#endif /* WITHRTS96 */
	#if WITHRTS192
		maxpacket = ulmax16(maxpacket, UACIN_RTS192_DATASIZE);
	#endif /* WITHRTS192 */
#endif /* ! WITHUSBUACIN2 || WITHUSBUACINOUTRENESAS */
	return maxpacket;
}

#if WITHUSBUACIN2

uint_fast16_t usbd_getuacinrtsmaxpacket(void)
{
	uint_fast16_t maxpacket = 64;
#if WITHRTS96
	maxpacket = ulmax16(maxpacket, UACIN_RTS96_DATASIZE);
#endif /* WITHRTS96 */
#if WITHRTS192
	maxpacket = ulmax16(maxpacket, UACIN_RTS192_DATASIZE);
#endif /* WITHRTS192 */
	return maxpacket;
}

#endif /* WITHUSBUACIN2 */

uint_fast16_t usbd_getuacoutmaxpacket(void)
{
	return UACOUT_AUDIO48_DATASIZE;
}


// Состояние - выбранные альтернативные конфигурации по каждому интерфейсу USB configuration descriptor
static uint8_t altinterfaces [INTERFACE_count];

static uintptr_t uacinaddr = 0;
static uint_fast16_t uacinsize = 0;
static uintptr_t uacinrtsaddr = 0;
static uint_fast16_t uacinrtssize = 0;

static __ALIGN_BEGIN uint8_t uacoutbuff [UACOUT_AUDIO48_DATASIZE] __ALIGN_END;

static __ALIGN_BEGIN uint8_t uac_ep0databuffout [USB_OTG_MAX_EP0_SIZE] __ALIGN_END;


static USBD_StatusTypeDef USBD_UAC_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
#if WITHUSBUACIN
	USBD_LL_CloseEP(pdev, USBD_EP_AUDIO_IN);
	if (uacinaddr != 0)
	{
		IRQL_t oldIrql;
		RiseIrql(IRQL_REALTIME, & oldIrql);
		release_dmabufferx(uacinaddr);
		LowerIrql(oldIrql);
		uacinaddr = 0;
	}
	altinterfaces [INTERFACE_AUDIO_MIKE] = 0;
	buffers_set_uacinalt(altinterfaces [INTERFACE_AUDIO_MIKE]);

#if WITHUSBUACIN2

	USBD_LL_CloseEP(pdev, USBD_EP_RTS_IN);

	if (uacinrtsaddr != 0)
	{
		IRQL_t oldIrql;
		RiseIrql(IRQL_REALTIME, & oldIrql);
		release_dmabufferxrts(uacinrtsaddr);
		LowerIrql(oldIrql);
		uacinrtsaddr = 0;
	}
	altinterfaces [INTERFACE_AUDIO_RTS] = 0;
	buffers_set_uacinrtsalt(altinterfaces [INTERFACE_AUDIO_RTS]);
#endif /* WITHUSBUACIN2 */
#endif /* WITHUSBUACIN */

#if WITHUSBUACOUT
	altinterfaces [INTERFACE_AUDIO_SPK] = 0;
	USBD_LL_CloseEP(pdev, USBD_EP_AUDIO_OUT);
	//terminalsprops [TERMINAL_ID_SELECTOR_6] [AUDIO_CONTROL_UNDEFINED] = 1;
	buffers_set_uacoutalt(altinterfaces [INTERFACE_AUDIO_SPK]);
	uacout_buffer_stop();
#endif /* WITHUSBUACOUT */
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
	/* значения для немодифицируемых управляющих элементов громкости */
	enum
	{
		VolMin = 0,
		VolMax = 256,
		VolRes = 4,
		VolCur = VolMax
	};
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
	const uint_fast8_t terminalID = HI_BYTE(req->wIndex);
	const uint_fast8_t CS = HI_BYTE(req->wValue);	// The Control Selector indicates which type of Control this request is manipulating. (Volume, Mute, etc.)
	const uint_fast8_t CN = LO_BYTE(req->wValue);	// The Channel Number (CN) indicates which logical channel of the cluster is to be influenced
	// CS=1: Mute - supports only CUR (1 byte)
	// CS=2: Volume supports CUR, MIN, MAX, and RES (2 byte)
	const uint_fast8_t val8 = req->wLength == 1 ? buff [0] : UINT8_MAX;
	const uint_fast16_t val16 =  req->wLength == 2 ? buff [1] * 256 + buff [0] : UINT16_MAX;
	//PRINTF(PSTR("USBD_UAC1_FeatureUnit_req: AUDIO_REQUEST_SET_CUR: interfacev=%u,  terminal=%u, CS=%d, CN=%d, value=%d\n"), interfacev, terminalID, CS, CN, val8);
	if (CS == AUDIO_MUTE_CONTROL)
	{
		// Mute control
		if (req->bRequest == AUDIO_REQUEST_GET_CUR)
		{
			// Mute control CUR request response
			return ulmin16(USBD_poke_u8(buff, 0), req->wLength);
		}
		// AUDIO_REQUEST_SET_CUR
		//PRINTF(PSTR("USBD_UAC1_FeatureUnit_req: AUDIO_REQUEST_SET_CUR: interfacev=%u,  terminal=%u, CS=%d, CN=%d, value=%d\n"), interfacev, terminalID, CS, CN, val8);
		return 0;
	}
	else if (CS == AUDIO_VOLUME_CONTROL)
	{
		// Volume control
		switch (req->bRequest)
		{
		case AUDIO_REQUEST_GET_CUR:
			//PRINTF(PSTR("USBD_UAC1_FeatureUnit_req: AUDIO_REQUEST_GET_CUR: interfacev=%u,  terminal=%u, CS=%d, CN=%d\n"), interfacev, terminalID, CS, CN);
			return ulmin16(USBD_poke_u16(buff, VolCur), req->wLength);

		case AUDIO_REQUEST_SET_CUR:
			//PRINTF(PSTR("USBD_UAC1_FeatureUnit_req: AUDIO_REQUEST_SET_CUR: interfacev=%u,  terminal=%u, CS=%d, CN=%d, value=%d\n"), interfacev, terminalID, CS, CN, val16);
			//terminalsprops [terminalID] [controlID] = buff [0];
			return 0;

		case AUDIO_REQUEST_GET_MIN:
			//PRINTF(PSTR("USBD_UAC1_FeatureUnit_req: AUDIO_REQUEST_GET_MIN: interfacev=%u,  terminal=%u, CS=%d, CN=%d, \n"), interfacev, terminalID, CS, CN);
			return ulmin16(USBD_poke_u16(buff, VolMin), req->wLength);

		case AUDIO_REQUEST_GET_MAX:
			//PRINTF(PSTR("USBD_UAC1_FeatureUnit_req: AUDIO_REQUEST_GET_MAX: interfacev=%u,  terminal=%u, CS=%d, CN=%d, \n"), interfacev, terminalID, CS, CN);
			return ulmin16(USBD_poke_u16(buff, VolMax), req->wLength);

		case AUDIO_REQUEST_GET_RES:
			//PRINTF(PSTR("USBD_UAC1_FeatureUnit_req: AUDIO_REQUEST_GET_RES: interfacev=%u,  terminal=%u, CS=%d, CN=%d, \n"), interfacev, terminalID, CS, CN);
			return ulmin16(USBD_poke_u16(buff, VolRes), req->wLength);
		default:
			TP();	// here then connecting to Android
			return 0;
		}
	}
	else
	{
		PRINTF(PSTR("X USBD_UAC1_FeatureUnit_req: interfacev=%u,  terminal=%u, CS=%d, CN=%d, value=%d\n"), interfacev, terminalID, CS, CN, val8);
		TP();
	}
	return 0;
}

// UAC1: Выполнение запроса к Selector UAC1
// see UAC1_AudioFeatureUnit
static unsigned USBD_UAC1_Selector_req(
	const USBD_SetupReqTypedef *req,
	uint8_t * buff
	)
{
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
	const uint_fast8_t terminalID = HI_BYTE(req->wIndex);
	const uint_fast8_t val8 = req->wLength == 1 ? buff [0] : UINT8_MAX;
	switch (req->bRequest)
	{
	default:
		// Undefined request
		TP();
		return 0;

	case AUDIO_REQUEST_SET_CUR:
		//PRINTF(PSTR("USBD_UAC1_Selector_req: AUDIO_REQUEST_SET_CUR: interfacev=%u,  terminal=%u, value=%d\n"), interfacev, terminalID, val8);
		//terminalsprops [terminalID] [controlID] = buff [0];
		return 0;

	case AUDIO_REQUEST_GET_CUR:
		//PRINTF(PSTR("USBD_UAC1_Selector_req: AUDIO_REQUEST_GET_CUR: interfacev=%u,  terminal=%u\n"), interfacev, terminalID);
		return ulmin16(USBD_poke_u8(buff, 1), req->wLength);

	case AUDIO_REQUEST_GET_MIN:
		//PRINTF(PSTR("USBD_UAC1_Selector_req: AUDIO_REQUEST_GET_MIN: interfacev=%u,  terminal=%u\n"), interfacev, terminalID);
		return ulmin16(USBD_poke_u8(buff, 1), req->wLength);

	case AUDIO_REQUEST_GET_MAX:
		//PRINTF(PSTR("USBD_UAC1_Selector_req: AUDIO_REQUEST_GET_MAX: interfacev=%u,  terminal=%u\n"), interfacev, terminalID);
		return ulmin16(USBD_poke_u8(buff, TERMINAL_ID_SELECTOR_6_INPUTS), req->wLength);

	case AUDIO_REQUEST_GET_RES:
		//PRINTF(PSTR("USBD_UAC1_Selector_req: AUDIO_REQUEST_GET_MIN: interfacev=%u,  terminal=%u\n"), interfacev, terminalID);
		return ulmin16(USBD_poke_u8(buff, 1), req->wLength);
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

#if WITHUAC2

// UAC2: Выполнение запроса CURR
static unsigned USBD_UAC2_CloclMultiplier_req_48k(
	const USBD_SetupReqTypedef *req,
	uint8_t * buff
	)
{
	const uint_fast8_t terminalID = HI_BYTE(req->wIndex);
	const uint_fast8_t controlID = HI_BYTE(req->wValue);
	const uint_fast8_t channelNumber = LO_BYTE(req->wValue);
	const uint_fast32_t denominator = FPGADECIMATION;
	const uint_fast32_t numerator = DDS1_CLK_MUL;

	switch (req->bRequest)
	{
	case 0x01:	// CURR
		switch (controlID)
		{
		default:
			// Undefined control ID
			TP();
			return 0;
		case 1:
			// CM_NUMERATOR_CONTROL
			return USBD_poke_u16(buff + 0, numerator); // numerator
		case 2:
			// CM_DENOMINATOR_CONTROL
			return USBD_poke_u16(buff + 0, denominator); // denominator
		}
		break;
	}
	return 0;
}

// UAC2: Выполнение запроса CURR
static unsigned USBD_UAC2_CloclMultiplier_req_96k(
	const USBD_SetupReqTypedef *req,
	uint8_t * buff
	)
{
	const uint_fast8_t terminalID = HI_BYTE(req->wIndex);
	const uint_fast8_t controlID = HI_BYTE(req->wValue);
	const uint_fast8_t channelNumber = LO_BYTE(req->wValue);
	const uint_fast32_t denominator = FPGADECIMATION / 2;
	const uint_fast32_t numerator = DDS1_CLK_MUL;

	switch (req->bRequest)
	{
	case 0x01:	// CURR
		switch (controlID)
		{
		default:
			// Undefined control ID
			TP();
			return 0;
		case 1:
			// CM_NUMERATOR_CONTROL
			return USBD_poke_u16(buff + 0, numerator); // numerator
		case 2:
			// CM_DENOMINATOR_CONTROL
			return USBD_poke_u16(buff + 0, denominator); // denominator
		}
		break;
	}
	return 0;
}
#endif /* WITHUAC2 */

// UAC2: Выполнение запроса CURR/RANGE
static unsigned USBD_UAC2_ClockSource_req(
	const USBD_SetupReqTypedef *req,
	uint8_t * buff
	)
{
	const uint_fast8_t terminalID = HI_BYTE(req->wIndex);
	const uint_fast8_t controlID = HI_BYTE(req->wValue);
	const uint_fast8_t channelNumber = LO_BYTE(req->wValue);
	const uint_fast32_t freq = REFERENCE_FREQ;

	switch (req->bRequest)
	{
	case 0x01:	// CURR
		switch (controlID)
		{
		default:
			// Undefined control ID
			TP();
			return 0;
		case 1:
			// FREQ
			return USBD_poke_u32(buff + 0, freq); // sample rate
		case 2:
			// VALID
			return USBD_poke_u8(buff + 0, 1); // valid
		}
		break;
	case 0x02:	// RANGE
		// The Clock Validity Control must have only the CUR attribute
		switch (controlID)
		{
		default:
			// Undefined control ID
			TP();
			return 0;
		case 1:
			// FREQ
			return USBD_fill_range_lay3pb(buff, freq);
		}
		break;
	}
	return 0;
}

//	As described in the Introducing EZ-USB® chapter on page 13 the host and device maintain a data toggle bit, which is toggled
//	between data packet transfers. There are certain times when the firmware must reset an endpoint’s data toggle bit to ’0’:
//	■ After a configuration changes (for example, after the host issues a Set Configuration request).
//	■ After an interface’s alternate setting changes (i.e., after the host issues a Set Interface request).
//	■ After the host sends a ‘Clear Feature - Endpoint Stall’ request to an endpoint.
//	For the first two, the firmware must clear the data toggle bits for all endpoints contained in the affected interfaces. For the
//	third, only one endpoint’s data toggle bit is cleared.
//	The TOGCTL register contains bits to set or clear an endpoint data toggle bit, as well as to read the current state of a toggle
//	bit.
//	At this writing, there is no known reason for firmware to set an endpoint toggle to ‘1’. Also, since the EZ-USB handles all
//	data toggle management, normally there is no reason to know the state of a data toggle. These capabilities are included in
//	the TOGCTL register for completeness and debug purposes.

static USBD_StatusTypeDef USBD_UAC_Setup(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	static __ALIGN_BEGIN uint8_t buff [32] __ALIGN_END;	// was: 7
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

#if WITHUSBWCID
	// WCID devices support
	// В документе от Микрософт по другому расположены данные в запросе: LO_BYTE(req->wValue) это результат запуска и тестирования
	if (req->bRequest == USBD_WCID_VENDOR_CODE &&
			(
#if WITHUSBUACIN2
					LO_BYTE(req->wValue) == INTERFACE_AUDIO_CONTROL_RTS ||
#endif /* WITHUSBUACIN2 */
#if WITHUSBUACIN
					LO_BYTE(req->wValue) == INTERFACE_AUDIO_CONTROL_MIKE ||
#endif /* WITHUSBUACIN */
#if WITHUSBUACOUT
					LO_BYTE(req->wValue) == INTERFACE_AUDIO_CONTROL_SPK ||
#endif /* WITHUSBUACOUT */
					0)
			&& req->wIndex == 0x05)
	{
		PRINTF(PSTR("MS USBD_UAC_Setup: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
		return USBD_OK;
	}
#endif /* WITHUSBWCID */

	//PRINTF(PSTR("USBD_UAC_Setup: bmRequest=%04X, bRequest=%02X, wIndex=%04X, wLength=%04X, wValue=%04X (interfacev=%02X)\n"), req->bmRequest, req->bRequest, req->wIndex, req->wLength, req->wValue, interfacev);
	unsigned len = 0;
	if ((req->bmRequest & USB_REQ_TYPE_DIR) != 0)
	{
		// IN direction
		switch (req->bmRequest & USB_REQ_TYPE_MASK)
		{
		case USB_REQ_TYPE_CLASS:
			switch (interfacev)
			{
#if WITHUSBUACIN
		#if WITHUSBUACIN2
			case INTERFACE_AUDIO_CONTROL_RTS:	/* AUDIO spectrum control interface */
		#endif /* WITHUSBUACIN2 */
			case INTERFACE_AUDIO_CONTROL_MIKE:	// AUDIO control interface
#endif /* WITHUSBUACIN */
#if WITHUSBUACOUT
			case INTERFACE_AUDIO_CONTROL_SPK:	// AUDIO control interface
#endif /* WITHUSBUACOUT */
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
#if WITHUAC2
					case TERMINAL_ID_CLKMULTIPLIER_UACIN48_UACINRTS:
					case TERMINAL_ID_CLKMULTIPLIER_UACIN48:
					case TERMINAL_ID_CLKMULTIPLIER_UACOUT48:
					case TERMINAL_ID_CLKMULTIPLIER_UACINOUT:
						len = USBD_UAC2_CloclMultiplier_req_48k(req, buff);
						break;

					case TERMINAL_ID_CLKMULTIPLIER_UACINRTS:
						len = USBD_UAC2_CloclMultiplier_req_96k(req, buff);
						break;

					case TERMINAL_ID_CLKSOURCE + 0 * MAX_TERMINALS_IN_INTERFACE:
					case TERMINAL_ID_CLKSOURCE + 1 * MAX_TERMINALS_IN_INTERFACE:
					case TERMINAL_ID_CLKSOURCE + 2 * MAX_TERMINALS_IN_INTERFACE:
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
#endif /* WITHUAC2 */
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
			case USB_REQ_GET_INTERFACE:
				// не видел вызовов этой функции
				switch (interfacev)
				{
#if WITHUSBUACIN
			#if WITHUSBUACIN2
				case INTERFACE_AUDIO_CONTROL_RTS:	/* AUDIO spectrum control interface */
				case INTERFACE_AUDIO_RTS:	/* AUDIO spectrum control interface */
			#endif /* WITHUSBUACIN2 */
				case INTERFACE_AUDIO_CONTROL_MIKE:	// AUDIO control interface
				case INTERFACE_AUDIO_MIKE:	// AUDIO control interface
#endif /* WITHUSBUACIN */
#if WITHUSBUACOUT
				case INTERFACE_AUDIO_CONTROL_SPK:	// AUDIO control interface
				case INTERFACE_AUDIO_SPK:	// AUDIO control interface
#endif /* WITHUSBUACOUT */
					//PRINTF(PSTR("USBD_UAC_Setup IN: USB_REQ_TYPE_STANDARD USB_REQ_GET_INTERFACE dir=%02X interfacev=%d, req->wLength=%d\n"), req->bmRequest & 0x80, interfacev, (int) req->wLength);
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
			// Other request targets
			//TP();
			//PRINTF(PSTR("X USBD_UAC_Setup: bRequest=%02X, wIndex=%04X, wLength=%04X, wValue=%04X (interfacev=%02X)\n"), req->bRequest, req->wIndex, req->wLength, req->wValue, interfacev);
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
#if WITHUSBUACIN
		#if WITHUSBUACIN2
			case INTERFACE_AUDIO_CONTROL_RTS:	/* AUDIO spectrum control interface */
		#endif /* WITHUSBUACIN2 */
			case INTERFACE_AUDIO_CONTROL_MIKE:	// AUDIO control interface
#endif /* WITHUSBUACIN */
#if WITHUSBUACOUT
			case INTERFACE_AUDIO_CONTROL_SPK:	// AUDIO control interface
#endif /* WITHUSBUACOUT */
				switch (req->bRequest)
				{
				default:
					TP();	// here then connecting to Android
					//PRINTF(PSTR("USBD_UAC_Setup: OUT: USB_REQ_TYPE_CLASS bRequest=%02X interfacev=%d, value=%d, wIndex=%04X, length=%d\n"), (unsigned) req->bRequest, (int) interfacev, (int) LO_BYTE(req->wValue), (unsigned) req->wIndex, (int) req->wLength);
					break;

				case 0x01:
					// class request with code 0x01
					// USB_REQ_CLEAR_FEATURE ???
					// set parameters to feature unit!!!
					//TP();
					//PRINTF(PSTR("USBD_UAC_Setup: OUT: USB_REQ_TYPE_CLASS bRequest=%02X interfacev=%d, value=%d, wIndex=%04X, length=%d\n"), (unsigned) req->bRequest, (int) interfacev, (int) LO_BYTE(req->wValue), (unsigned) req->wIndex, (int) req->wLength);
					break;
				}
				/* все запросы этого класса устройств */
				if (req->wLength != 0)
				{
					USBD_CtlPrepareRx(pdev, uac_ep0databuffout, ulmin16(ARRAY_SIZE(uac_ep0databuffout), req->wLength));
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
			case USB_REQ_SET_INTERFACE:
				//PRINTF(PSTR("USBD_UAC_Setup: USB_REQ_TYPE_STANDARD USB_REQ_SET_INTERFACE interfacev=%d, value=%d\n"), (int) interfacev, (int) LO_BYTE(req->wValue));
				switch (interfacev)
				{
#if WITHUSBUACIN
			#if WITHUSBUACIN2
				case INTERFACE_AUDIO_RTS: // Audio interface: recording device
					//PRINTF(PSTR("USBD_UAC_Setup: USB_REQ_TYPE_STANDARD USB_REQ_SET_INTERFACE INTERFACE_AUDIO_RTS interfacev=%d, value=%d\n"), (int) interfacev, (int) LO_BYTE(req->wValue));
					altinterfaces [interfacev] = LO_BYTE(req->wValue);
					buffers_set_uacinrtsalt(altinterfaces [interfacev]);
					USBD_CtlSendStatus(pdev);
					break;
			#endif /* WITHUSBUACIN2 */
				case INTERFACE_AUDIO_MIKE: // Audio interface: recording device
					//PRINTF(PSTR("USBD_UAC_Setup: USB_REQ_TYPE_STANDARD USB_REQ_SET_INTERFACE INTERFACE_AUDIO_MIKE interfacev=%d, value=%d\n"), (int) interfacev, (int) LO_BYTE(req->wValue));
					altinterfaces [interfacev] = LO_BYTE(req->wValue);
					buffers_set_uacinalt(altinterfaces [interfacev]);
					USBD_CtlSendStatus(pdev);
					break;
#endif /* WITHUSBUACIN */
#if WITHUSBUACOUT
				case INTERFACE_AUDIO_SPK:	// DATA OUT Audio interface: playback device
					//PRINTF(PSTR("USBD_UAC_Setup: USB_REQ_TYPE_STANDARD USB_REQ_SET_INTERFACE INTERFACE_AUDIO_SPK interfacev=%d, value=%d\n"), (int) interfacev, (int) LO_BYTE(req->wValue));
					altinterfaces [interfacev] = LO_BYTE(req->wValue);
					buffers_set_uacoutalt(altinterfaces [interfacev]);
					USBD_CtlSendStatus(pdev);
					break;
#endif /* WITHUSBUACOUT */

				default:
					// Other interfaces
					//TP();
					break;
				}
			}
			break;

		default:
			// Other request targets
			break;
		}
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_UAC_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	switch (epnum)
	{
#if WITHUSBUACOUT
	case USBD_EP_AUDIO_OUT:
		/* UAC EP OUT */
		// use audio data
		uacout_buffer_save(uacoutbuff, USBD_LL_GetRxDataSize(pdev, epnum), UACOUT_FMT_CHANNELS_AUDIO48, UACOUT_AUDIO48_SAMPLEBITS);
		/* Prepare Out endpoint to receive next audio data packet */
		USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(epnum), uacoutbuff, UACOUT_AUDIO48_DATASIZE);
		break;
#endif /* WITHUSBUACOUT */
	}
	return USBD_OK;
}
static USBD_StatusTypeDef USBD_UAC_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	const USBD_SetupReqTypedef * const req = & pdev->request;

	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	//PRINTF(PSTR("1 USBD_XXX_EP0_RxReady: interfacev=%u: bRequest=%u, wIndex=%04X, wValue=%04X, wLength=%u\n"), interfacev, req->bRequest, req->wIndex, req->wValue, req->wLength);
	switch (interfacev)
	{
#if WITHUSBUACIN
#if WITHUSBUACIN2
	case INTERFACE_AUDIO_CONTROL_RTS:	/* AUDIO spectrum control interface */
#endif /* WITHUSBUACIN2 */
	case INTERFACE_AUDIO_CONTROL_MIKE:	// AUDIO control interface
#endif /* WITHUSBUACIN */
#if WITHUSBUACOUT
	case INTERFACE_AUDIO_CONTROL_SPK:	// AUDIO control interface
#endif /* WITHUSBUACOUT */
		{
			//PRINTF(PSTR("2 USBD_XXX_EP0_RxReady: interfacev=%u: bRequest=%u, wIndex=%04X, wValue=%04X, wLength=%u\n"), interfacev, req->bRequest, req->wIndex, req->wValue, req->wLength);
			const uint_fast8_t terminalID = HI_BYTE(req->wIndex);

			switch (terminalID)
			{
			case TERMINAL_ID_FU1_IN + 0 * MAX_TERMINALS_IN_INTERFACE:
			case TERMINAL_ID_FU1_IN + 1 * MAX_TERMINALS_IN_INTERFACE:
			case TERMINAL_ID_FU1_IN + 2 * MAX_TERMINALS_IN_INTERFACE:
			case TERMINAL_ID_FU1_OUT + 0 * MAX_TERMINALS_IN_INTERFACE:
			case TERMINAL_ID_FU1_OUT + 1 * MAX_TERMINALS_IN_INTERFACE:
			case TERMINAL_ID_FU1_OUT + 2 * MAX_TERMINALS_IN_INTERFACE:
				USBD_UAC1_FeatureUnit_req(req, uac_ep0databuffout);
				return USBD_OK;
			case TERMINAL_ID_SELECTOR_6:
				USBD_UAC1_Selector_req(req, uac_ep0databuffout);
				return USBD_OK;

			default:
				{
					const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
					const uint_fast8_t terminalID = HI_BYTE(req->wIndex);
					const uint_fast8_t controlID = HI_BYTE(req->wValue);	// AUDIO_MUTE_CONTROL, AUDIO_VOLUME_CONTROL, ...
					PRINTF(PSTR("USBD_XXX_EP0_RxReady: request=%u: interfacev=%u, %u=%u\n"), req->bRequest, interfacev, terminalID, uac_ep0databuffout [0]);
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
	IRQL_t oldIrql;
	// epnum without direction bit
	//PRINTF(PSTR("USBD_LL_DataInStage: IN: epnum=%02X\n"), epnum);
	switch (epnum)
	{
#if WITHUSBUACIN
	case ((USBD_EP_AUDIO_IN) & 0x7F):
		if (uacinaddr != 0)
		{
			RiseIrql(IRQL_REALTIME, & oldIrql);
			release_dmabufferx(uacinaddr);
			LowerIrql(oldIrql);
		}

		RiseIrql(IRQL_REALTIME, & oldIrql);
		uacinaddr = getfilled_dmabufferx(& uacinsize);
		LowerIrql(oldIrql);

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
			RiseIrql(IRQL_REALTIME, & oldIrql);
			release_dmabufferxrts(uacinrtsaddr);
			LowerIrql(oldIrql);
		}

		RiseIrql(IRQL_REALTIME, & oldIrql);
		uacinrtsaddr = getfilled_dmabufferxrts(& uacinrtssize);
		LowerIrql(oldIrql);

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
#endif /* WITHUSBUACIN */
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_UAC_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	//PRINTF(PSTR("USBD_XXX_Init: cfgidx=%d\n"), cfgidx);
	//terminalsprops [TERMINAL_ID_SELECTOR_6] [AUDIO_CONTROL_UNDEFINED] = 1;

#if WITHUSBUACIN
	buffers_set_uacinalt(altinterfaces [INTERFACE_AUDIO_MIKE]);
	altinterfaces [INTERFACE_AUDIO_MIKE] = 0;
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
#endif /* WITHUSBUACIN */
#if WITHUSBUACOUT
	buffers_set_uacoutalt(altinterfaces [INTERFACE_AUDIO_SPK]);
	altinterfaces [INTERFACE_AUDIO_SPK] = 0;
	/* UAC Open EP OUT */
	USBD_LL_OpenEP(pdev, USBD_EP_AUDIO_OUT, USBD_EP_TYPE_ISOC, UACOUT_AUDIO48_DATASIZE);
   /* UAC Prepare Out endpoint to receive 1st packet */
	USBD_LL_PrepareReceive(pdev, USBD_EP_AUDIO_OUT, uacoutbuff, UACOUT_AUDIO48_DATASIZE);

	uacout_buffer_start();
#endif /* WITHUSBUACOUT */
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

// USB AUDIO
// Канал DMA ещё занят - оставляем в очереди, иначе получить данные через getfilled_dmabufferx
void __weak refreshDMA_uacin(void)
{
}


#endif /* WITHUSBHW && WITHUSBUAC */
