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

#include "display/display.h"
#include "formats.h"
#include <string.h>

#if WITHUSBHW

#include "usb200.h"
#include "usbch9.h"
#include "usb_core.h"

static uint_fast8_t notseq;

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

/**
  * @}
  */

/* PCD Handle Structure */
static USBALIGN_BEGIN PCD_HandleTypeDef hpcd_USB_OTG USBALIGN_END;
/* USB Device Core handle declaration */
static USBALIGN_BEGIN USBD_HandleTypeDef hUsbDevice USBALIGN_END;

/* HCD Handle Structure */
static USBALIGN_BEGIN HCD_HandleTypeDef hhcd_USB_OTG USBALIGN_END;
/* USB Host Core handle declaration */
static USBALIGN_BEGIN USBH_HandleTypeDef hUsbHost USBALIGN_END;
static ApplicationTypeDef Appli_state = APPLICATION_IDLE;

#if CPUSTYLE_R7S721

#define DEVDRV_USBF_PIPE_IDLE                       (0x00)
#define DEVDRV_USBF_PIPE_WAIT                       (0x01)
#define DEVDRV_USBF_PIPE_DONE                       (0x02)
#define DEVDRV_USBF_PIPE_NORES                      (0x03)
#define DEVDRV_USBF_PIPE_STALL                      (0x04)

#define DEVDRV_USBF_PID_NAK                         (0x0000u)
#define DEVDRV_USBF_PID_BUF                         (0x0001u)
#define DEVDRV_USBF_PID_STALL                       (0x0002u)
#define DEVDRV_USBF_PID_STALL2                      (0x0003u)


static void control_stall(USBD_HandleTypeDef *pdev)
{
	USB_OTG_GlobalTypeDef * const USBx = ((PCD_HandleTypeDef *) pdev->pData)->Instance;
	USBx->DCPCTR = (USBx->DCPCTR & ~ (USB_DCPCTR_PID)) |
		//DEVDRV_USBF_PID_NAK * (1uL << USB_DCPCTR_PID_SHIFT) |	// PID 00: NAK
		//1 * (1uL << USB_DCPCTR_PID_SHIFT) |	// PID 01: BUF response (depending on the buffer state)
		DEVDRV_USBF_PID_STALL * (1uL << USB_DCPCTR_PID_SHIFT) |	// PID 02: STALL response
		0;
}

// ACK
static void dcp_acksend(USB_OTG_GlobalTypeDef * const Instance)
{
	if (((Instance->DCPCTR & USB_DCPCTR_PID) >> USB_DCPCTR_PID_SHIFT) == 0x03)
	{
		Instance->DCPCTR = (Instance->DCPCTR & ~ USB_DCPCTR_PID) |
			DEVDRV_USBF_PID_NAK * (1uL << USB_DCPCTR_PID_SHIFT) |	// PID 00: NAK response
			0;
	}
	//control_transmit0single(pdev, NULL, 0);

	Instance->DCPCTR = (Instance->DCPCTR & ~ USB_DCPCTR_PID) |
		0x01 * MASK2LSB(USB_DCPCTR_PID) |	// PID 01: BUF response (depending on the buffer state)
		0;

}


#define USBD_FRDY_COUNT 10

static uint_fast8_t usbd_wait_fifo(PCD_TypeDef * const USBx, uint_fast8_t pipe, unsigned waitcnt)
{
	while ((USBx->CFIFOSEL & USB_CFIFOSEL_CURPIPE) != (pipe << USB_CFIFOSEL_CURPIPE_SHIFT) || (USBx->CFIFOCTR & USB_CFIFOCTR_FRDY) == 0)	// FRDY
	{
		if (-- waitcnt == 0)
		{
#if 0
			// From USB HOST
			if (pipe == 0)
			{
				pid = (USBx->DCPCTR & USB_DCPCTR_PID) >> USB_DCPCTR_PID_SHIFT;
			}
			else
			{
				volatile uint16_t * const PIPEnCTR = (& Instance->PIPE1CTR) + (pipe - 1);
			}
            /* Clear the pipe */
            USB_PIPECTR(pUSB, iPipeNumber)->BIT.PID = 0;
			Instance->CFIFOCTR = USB_CFIFOCTR_BCLR;	// BCLR (wrong: могди не переключиться на FIFO)
            USB_PIPECTR(pUSB, iPipeNumber)->BIT.ACLRM = 1;
            USB_PIPECTR(pUSB, iPipeNumber)->BIT.ACLRM = 0;
            USB_PIPECTR(pUSB, iPipeNumber)->BIT.ACLRM = 1;
            USB_PIPECTR(pUSB, iPipeNumber)->BIT.ACLRM = 0;
#endif
            return 1;
		}
	}
	return 0;
}


// NAK
static void nak_ep0_unused(USBD_HandleTypeDef *pdev)
{
	//PRINTF(PSTR("nak_ep0\n"));

	USB_OTG_GlobalTypeDef * const USBx = ((PCD_HandleTypeDef *) pdev->pData)->Instance;

	if (((USBx->DCPCTR & USB_DCPCTR_PID) >> USB_DCPCTR_PID_SHIFT) == 0x03)
	{
		USBx->DCPCTR = (USBx->DCPCTR & ~ USB_DCPCTR_PID) |
			//1 * (1uL << USB_DCPCTR_CCPL_SHIFT) |	// CCPL - Не имеет значения в моих тестах
			DEVDRV_USBF_PID_NAK * (1uL << USB_DCPCTR_PID_SHIFT) |	// PID 00: NAK response
			0;
	}

	const uint_fast8_t pipe = 0;	// DCP

	USBx->CFIFOSEL =
		//1 * (1uL << USB_CFIFOSEL_RCNT_SHIFT) |		// RCNT
		(pipe << USB_CFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
		1 * (1uL << USB_CFIFOSEL_ISEL_SHIFT_) * (pipe == 0) |	// ISEL 1: Writing to the buffer memory is selected (for DCP)
		0;
	if (usbd_wait_fifo(USBx, pipe, USBD_FRDY_COUNT))
		return;

	USBx->CFIFOCTR = (1uL << USB_CFIFOCTR_BCLR_SHIFT);	// BCLR

	USBx->DCPCTR = (USBx->DCPCTR & ~ (0x03)) |
		0 * (1uL << 0) |	// PID 00: NAK
		//1 * (1uL << 0) |	// PID 01: BUF response (depending on the buffer state)
		//2 * (1uL << 0) |	// PID 02: STALL response
		0;
}

// STALL
static void stall_ep0(USBD_HandleTypeDef *pdev)
{
	//PRINTF(PSTR("stall_ep0\n"));

	USB_OTG_GlobalTypeDef * const USBx = ((PCD_HandleTypeDef *) pdev->pData)->Instance;
	const uint_fast8_t pipe = 0;	// DCP

	USBx->CFIFOSEL =
		//1 * (1uL << USB_CFIFOSEL_RCNT_SHIFT) |		// RCNT
		(pipe << USB_CFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
		1 * (1uL << USB_CFIFOSEL_ISEL_SHIFT_) * (pipe == 0) |	// ISEL 1: Writing to the buffer memory is selected (for DCP)
		0;

	if (usbd_wait_fifo(USBx, pipe, USBD_FRDY_COUNT) == 0)
	{
		USBx->CFIFOCTR = USB_CFIFOCTR_BCLR;	// BCLR
	}

	USBx->DCPCTR = (USBx->DCPCTR & ~ (USB_DCPCTR_PID)) |
		//DEVDRV_USBF_PID_NAK * (1uL << 0) |	// PID 00: NAK
		//1 * (1uL << 0) |	// PID 01: BUF response (depending on the buffer state)
		DEVDRV_USBF_PID_STALL * (1uL << USB_DCPCTR_PID_SHIFT) |	// PID 02: STALL response
		0;

}

static uint_fast16_t /* volatile */ g_usb0_function_PipeIgnore [16];

// Эта функция не должна общаться с DCPCTR - она универсальная
static uint_fast8_t usbd_read_data(PCD_TypeDef * const USBx, uint_fast8_t pipe, uint8_t * data, unsigned size, unsigned * readcnt)
{
	//PRINTF(PSTR("selected read from c_fifo%u 0, CFIFOCTR=%04X, CFIFOSEL=%04X\n"), pipe, Instance->CFIFOCTR, Instance->CFIFOSEL);
	USBx->CFIFOSEL =
		//1 * (1uL << USB_CFIFOSEL_RCNT_SHIFT) |		// RCNT
		(pipe << USB_CFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
		0 * USB_CFIFOSEL_MBW |	// MBW 00: 8-bit width
		0;

	if (usbd_wait_fifo(USBx, pipe, USBD_FRDY_COUNT))
	{
		PRINTF(PSTR("usbd_read_data: usbd_wait_fifo error, pipe=%d, USBx->CFIFOSEL=%08lX\n"), (int) pipe, (unsigned long) USBx->CFIFOSEL);
		return 1;	// error
	}

	g_usb0_function_PipeIgnore [pipe] = 0;
	unsigned count = 0;
	unsigned size8 = (USBx->CFIFOCTR & USB_CFIFOCTR_DTLN) >> USB_CFIFOCTR_DTLN_SHIFT;
	size = ulmin16(size, size8);
	//PRINTF(PSTR("selected read from c_fifo%u 3, CFIFOCTR=%04X, CFIFOSEL=%04X\n"), pipe, Instance->CFIFOCTR, Instance->CFIFOSEL);
	count = size;
	while (size --)
	{
		* data ++ = USBx->CFIFO.UINT8 [R_IO_HH]; // HH=3
	}
	//PRINTF(PSTR("selected read from c_fifo%u 4, CFIFOCTR=%04X, CFIFOSEL=%04X\n"), pipe, Instance->CFIFOCTR, Instance->CFIFOSEL);

	USBx->CFIFOCTR = USB_CFIFOCTR_BCLR;	// BCLR
	* readcnt = count;
	//PRINTF(PSTR("selected read from c_fifo%u 5, CFIFOCTR=%04X, CFIFOSEL=%04X\n"), pipe, Instance->CFIFOCTR, Instance->CFIFOSEL);
	return 0;	// OK
}

static uint_fast8_t
usbd_write_data(PCD_TypeDef * const USBx, uint_fast8_t pipe, const uint8_t * data, unsigned size)
{
#if 0
	if (data != NULL && size != 0)
		PRINTF(PSTR("usbd_write_data, pipe=%d, size=%d, data[]={%02x,%02x,%02x,%02x,%02x,..}\n"), pipe, size, data [0], data [1], data [2], data [3], data [4]);
	else
	{
		PRINTF(PSTR("usbd_write_data, pipe=%d, size=%d, data[]={}\n"), pipe, size);
	}
#endif

	if (size == 0 && pipe == 0)
	{
		// set pid=buf
		dcp_acksend(USBx);
		return 0;
	}

	USBx->CFIFOSEL =
		//1 * (1uL << USB_CFIFOSEL_RCNT_SHIFT) |		// RCNT
		(pipe << USB_CFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
		0 * USB_CFIFOSEL_MBW |	// MBW 00: 8-bit width
		1 * USB_CFIFOSEL_ISEL_ * (pipe == 0) |	// ISEL 1: Writing to the buffer memory is selected (for DCP)
		0;

	if (usbd_wait_fifo(USBx, pipe, USBD_FRDY_COUNT))
	{
		PRINTF(PSTR("usbd_write_data: usbd_wait_fifo error, USBx->CFIFOSEL=%08lX\n"), (unsigned long) USBx->CFIFOSEL);
		return 1;	// error
	}

	g_usb0_function_PipeIgnore [pipe] = 0;
    while (size --)
	{
    	USBx->CFIFO.UINT8 [R_IO_HH] = * data ++; // HH=3
	}
    USBx->CFIFOCTR = USB_CFIFOCTR_BVAL;	// BVAL
	return 0;	// OK
}


// NRDY pipe Interrupt handler
static void
usbd_handler_nrdy(USBD_HandleTypeDef *pdev, uint_fast8_t pipe)
{
	USB_OTG_GlobalTypeDef * const Instance = ((PCD_HandleTypeDef *) pdev->pData)->Instance;

	//PRINTF(PSTR("usbd_handler_nrdy: pipe=%u\n"), pipe);
	uint_fast8_t pid;
	if (pipe == 0)
	{
		pid = (Instance->DCPCTR & USB_DCPCTR_PID) >> USB_DCPCTR_PID_SHIFT;
	}
	else
	{
		volatile uint16_t * const PIPEnCTR = (& Instance->PIPE1CTR) + (pipe - 1);

		if (pipe == HARDWARE_USBD_PIPE_CDC_IN)
		{
			//* PIPEnCTR = (* PIPEnCTR & ~ 0x03) | 0x0003;	// NAK->STALL
			//* PIPEnCTR = (* PIPEnCTR & ~ 0x03) | 0x0002;	// NAK->STALL
			//* PIPEnCTR = (* PIPEnCTR & ~ 0x03) | 0x0001;	// STALL->BUF
		}

		if (pipe == HARDWARE_USBD_PIPE_CDC_INb)
		{
			//* PIPEnCTR = (* PIPEnCTR & ~ 0x03) | 0x0003;	// NAK->STALL
			//* PIPEnCTR = (* PIPEnCTR & ~ 0x03) | 0x0002;	// NAK->STALL
			//* PIPEnCTR = (* PIPEnCTR & ~ 0x03) | 0x0001;	// STALL->BUF
		}

		pid = (* PIPEnCTR & USB_PIPEnCTR_1_5_PID) >> USB_PIPEnCTR_1_5_PID_SHIFT;
	}

	if (pid == DEVDRV_USBF_PID_STALL || pid == DEVDRV_USBF_PID_STALL2)
	{
		//g_usb0_function_pipe_status [pipe] = DEVDRV_USBF_PIPE_STALL;
	}
	else if (++ g_usb0_function_PipeIgnore [pipe] >= 3)
	{
		//g_usb0_function_pipe_status [pipe] = DEVDRV_USBF_PIPE_NORES;
	}
	else
	{
		if (pipe == 0)
		{
			Instance->DCPCTR = (Instance->DCPCTR & ~ USB_DCPCTR_PID) |
				DEVDRV_USBF_PID_NAK * (1uL << USB_DCPCTR_PID_SHIFT) |	// PID 00: NAK
				0;
			(void) Instance->DCPCTR;
			Instance->DCPCTR = (Instance->DCPCTR & ~ USB_DCPCTR_PID) |
				DEVDRV_USBF_PID_BUF * (1uL << USB_DCPCTR_PID_SHIFT) |	// PID 01: BUF response (depending on the buffer state)
				0;
			(void) Instance->DCPCTR;
		}
		else
		{
			volatile uint16_t * const PIPEnCTR = (& Instance->PIPE1CTR) + (pipe - 1);

			* PIPEnCTR = (* PIPEnCTR & ~ USB_PIPEnCTR_1_5_PID) |
				DEVDRV_USBF_PID_NAK * (1uL << USB_PIPEnCTR_1_5_PID_SHIFT) |	// PID 00: NAK
				0;
			(void) * PIPEnCTR;
			* PIPEnCTR = (* PIPEnCTR & ~ USB_PIPEnCTR_1_5_PID) |
				DEVDRV_USBF_PID_BUF * (1uL << USB_PIPEnCTR_1_5_PID_SHIFT) |	// PID 01: BUF response (depending on the buffer state)
				0;
			(void) * PIPEnCTR;
		}
	}
	(void) pid;
}

static void usb_save_request(USB_OTG_GlobalTypeDef * USBx, USBD_SetupReqTypedef *req)
{
	const uint_fast16_t usbreq = USBx->USBREQ;

	req->bmRequest     = LO_BYTE(usbreq & USB_FUNCTION_bmRequestType); //(pdata [0] >> 0) & 0x00FF;
	req->bRequest      = HI_BYTE(usbreq & USB_FUNCTION_bRequest); //(pdata [0] >> 8) & 0x00FF;
	req->wValue        = USBx->USBVAL; //(pdata [0] >> 16) & 0xFFFF;
	req->wIndex        = USBx->USBINDX; //(pdata [1] >> 0) & 0xFFFF;
	req->wLength       = USBx->USBLENG; //(pdata [1] >> 16) & 0xFFFF;

#if 0
	PRINTF(PSTR("usb_save_request: bmRequest=%04X, bRequest=%04X, wValue=%04X, wIndex=%04X, wLength=%04X\n"),
		req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
#endif
}

static void usbd_handle_ctrt(PCD_HandleTypeDef *hpcd, uint_fast8_t ctsq)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
	USBD_HandleTypeDef * const pdev = hpcd->pData;


	/*
		0: Idle or setup stage
		1: Control read data stage
		2: Control read status stage
		3: Control write data stage
		4: Control write status stage
		5: Control write (no data) status stage
		6: Control transfer sequence error
	*/
	// Right sequences:
	// seq1 OUT token -> seq2 -> seq0
	// seq3 IN token -> seq4 ->seq0
	// seq5 -> seq0
	switch (ctsq)	// CTSQ
	{
	case 0:
		// Idle or setup stage
		//actions_seq0(Instance, ReqRequest, ReqType, ReqTypeType, ReqTypeRecip, ReqValue, ReqIndex, ReqLength);
		//pdev->ep0_state = USBD_EP0_IDLE;
		break;

	case 1:
		//PRINTF(PSTR("actions_seq1\n"));
		// 1: Control read data stage
		// seq1 OUT token -> seq2 -> seq0
		usb_save_request(USBx, & pdev->request);
		USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_VALID;	// Clear VALID - in seq 1, 3 and 5
		//pdev->ep0_state = USBD_EP0_DATA_IN;
		HAL_PCD_SetupStageCallback(hpcd);
		break;

	case 2:
		//PRINTF(PSTR("actions_seq2\n"));
		// 2: Control read status stage
		//actions_seq2(pdev, & pdev->request);
		// после - usbdFunctionReq_seq1 - напимер после запроса GET_LINE_CODING
		// EP0_TxSent callback here
		// End of sending data trough EP0
		// xxx_TxSent
		//pdev->ep0_state = USBD_EP0_STATUS_IN;
		HAL_PCD_DataInStageCallback(hpcd, 0);
		USBx->DCPCTR |= USB_DCPCTR_CCPL;	// CCPL
		(void) USBx->DCPCTR;
		break;

	case 3:
		//PRINTF(PSTR("actions_seq3\n"));
		// 3: Control write data stage
		// seq3 IN token -> seq4 ->seq0
		usb_save_request(USBx, & pdev->request);
		USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_VALID;	// Clear VALID - in seq 1, 3 and 5
		//pdev->ep0_state = USBD_EP0_DATA_OUT;
		HAL_PCD_SetupStageCallback(hpcd);
		break;

	case 4:
		//PRINTF(PSTR("actions_seq4\n"));
		// 4: Control write status stage
		// после usbd_handler_brdy8_dcp_out
		// End of receieve data trough EP0
		// Set Line Coding here
		// xxx_EP0_RxReady
		//pdev->ep0_state = USBD_EP0_STATUS_OUT;
        HAL_PCD_DataOutStageCallback(hpcd, 0);
		USBx->DCPCTR |= USB_DCPCTR_CCPL;	// CCPL
		(void) USBx->DCPCTR;
		break;

	case 5:
		//PRINTF(PSTR("actions_seq5\n"));
		// 5: Control write (no data) status stage
		// seq5 -> seq0
		usb_save_request(USBx, & pdev->request);
		USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_VALID;	// Clear VALID - in seq 1, 3 and 5
		HAL_PCD_SetupStageCallback(hpcd);
		USBx->DCPCTR |= USB_DCPCTR_CCPL;	// CCPL
		(void) USBx->DCPCTR;
		break;

	case 6:
		//PRINTF(PSTR("actions_seq6\n"));
		// 6: Control transfer sequence error
		USBD_CtlError(pdev, & pdev->request);
		break;

	default:
		PRINTF(PSTR("actions_seqXXX\n"));
		// x: Control transfer sequence error
		USBD_CtlError(pdev, & pdev->request);
		break;
	}
}

static const struct { uint8_t pipe, ep; } usbd_usedpipes [] =
{
#if WITHUSBUAC
	{	HARDWARE_USBD_PIPE_ISOC_OUT,	USBD_EP_AUDIO_OUT, },		// ISOC OUT Аудиоданные от компьютера в TRX - D0FIFOB0
	{	HARDWARE_USBD_PIPE_ISOC_IN,		USBD_EP_AUDIO_IN, },		// ISOC IN Аудиоданные в компьютер из TRX - D0FIFOB1
#endif /* WITHUSBUAC */
#if WITHUSBCDC
	{	HARDWARE_USBD_PIPE_CDC_OUT,		USBD_EP_CDC_OUT, },		// CDC OUT Данные ком-порта от компьютера в TRX
	{	HARDWARE_USBD_PIPE_CDC_IN,		USBD_EP_CDC_IN, },		// CDC IN Данные ком-порта в компьютер из TRX
	{	HARDWARE_USBD_PIPE_CDC_INT,		USBD_EP_CDC_INT, },
	{	HARDWARE_USBD_PIPE_CDC_OUTb,	USBD_EP_CDC_OUTb, },	// CDC OUT dummy interfacei
	{	HARDWARE_USBD_PIPE_CDC_INb,		USBD_EP_CDC_INb, },		// CDC IN dummy interfacei
	{	HARDWARE_USBD_PIPE_CDC_INTb,	USBD_EP_CDC_INTb, },
#endif /* WITHUSBCDC */
};


static const struct { uint8_t pipe, ep; } brdyenbpipes2 [] =
{
	//{ 0x00, 0x00 },		// DCP
#if WITHUSBCDC
	{ HARDWARE_USBD_PIPE_CDC_OUT, USBD_EP_CDC_OUT },		// CDC OUT Данные ком-порта от компьютера в TRX
	{ HARDWARE_USBD_PIPE_CDC_IN, USBD_EP_CDC_IN },		// CDC IN Данные ком-порта в компьютер из TRX
	{ HARDWARE_USBD_PIPE_CDC_OUTb, USBD_EP_CDC_OUTb },	// CDC OUT dummy interfacei
	{ HARDWARE_USBD_PIPE_CDC_INb, USBD_EP_CDC_INb },		// CDC IN dummy interfacei
#endif /* WITHUSBCDC */
};


static void usbd_pipes_enable(PCD_TypeDef * const Instance)
{
	uint_fast8_t i;

	for (i = 0; i < sizeof usbd_usedpipes / sizeof usbd_usedpipes [0]; ++ i)
	{
		const uint_fast8_t pipe = usbd_usedpipes [i].pipe;
		volatile uint16_t * const PIPEnCTR = (& Instance->PIPE1CTR) + (pipe - 1);

		* PIPEnCTR = 0x0000;	// NAK
		while ((* PIPEnCTR & (USB_PIPEnCTR_1_5_PBUSY | USB_PIPEnCTR_1_5_CSSTS)) != 0)	// PBUSY, CSSTS
			;

		* PIPEnCTR = USB_PIPEnCTR_1_5_SQCLR;

		* PIPEnCTR = 0x0003;	// NAK->STALL
		* PIPEnCTR = 0x0002;	// NAK->STALL
		* PIPEnCTR = 0x0001;	// STALL->BUF
		Instance->NRDYENB |= (1u << pipe);

		g_usb0_function_PipeIgnore [pipe] = 0;
	}
	for (i = 0; i < sizeof brdyenbpipes2 / sizeof brdyenbpipes2 [0]; ++ i)
	{
		const uint_fast8_t pipe = brdyenbpipes2 [i].pipe;
		Instance->BRDYENB |= (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) или для заполнения передающего (IN) буфера
	}
	if (1)
	{
		// DCP, control pipe
		uint_fast8_t pipe = 0;	// PIPE0
		Instance->BRDYENB |= (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера
		Instance->BEMPENB |= (1uL << pipe);	// Прерывание окончания передачи передающего (IN) буфера
	}
}

static void usbd_pipes_disable(PCD_TypeDef * const Instance)
{
	uint_fast8_t i;

	if (1)
	{
		// DCP, control pipe
		uint_fast8_t pipe = 0;	// PIPE0
		Instance->BRDYENB &= ~ (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера
		Instance->BEMPENB &= ~ (1uL << pipe);	// Прерывание окончания передачи передающего (IN) буфера
	}
	for (i = 0; i < sizeof brdyenbpipes2 / sizeof brdyenbpipes2 [0]; ++ i)
	{
		const uint_fast8_t pipe = brdyenbpipes2 [i].pipe;
		Instance->BRDYENB &= ~ (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) или для заполнения передающего (IN) буфера
	}
	for (i = 0; i < sizeof usbd_usedpipes / sizeof usbd_usedpipes [0]; ++ i)
	{
		const uint_fast8_t pipe = usbd_usedpipes [i].pipe;
		volatile uint16_t * const PIPEnCTR = (& Instance->PIPE1CTR) + (pipe - 1);

		* PIPEnCTR = 0x0000;	// NAK
		while ((* PIPEnCTR & (USB_PIPEnCTR_1_5_PBUSY | USB_PIPEnCTR_1_5_CSSTS)) != 0)	// PBUSY, CSSTS
			;

		//* PIPEnCTR = 0x0003;	// NAK->STALL
		//* PIPEnCTR = 0x0002;	// NAK->STALL
		//* PIPEnCTR = 0x0001;	// STALL->BUF
		Instance->NRDYENB &= ~ (1u << pipe);
	}
}

static void
usbd_pipes_initialize(PCD_HandleTypeDef * hpcd)
{
	PCD_TypeDef * const Instance = hpcd->Instance;
	PRINTF(PSTR("usbd_pipes_initialize\n"));
	/*
		at initialize:
		usbd_handler_brdy: после инициализации появляется для тех pipe, у которых dir=0 (read direction)
	*/
	{
		Instance->DCPMAXP = (USB_OTG_MAX_EP0_SIZE << USB_DCPMAXP_MXPS_SHIFT);
	}
	unsigned bufnumb64 = 0x10;
#if WITHUSBCDC
	if (1)
	{
		// Данные CDC из компьютера в трансивер
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_CDC_OUT;	// PIPE3
		const uint_fast8_t epnum = USBD_EP_CDC_OUT;
		const uint_fast8_t dir = 0;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		Instance->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((Instance->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 3);
		Instance->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |	// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |			// DIR 1: Transmitting direction 0: Receiving direction
			1 * (1u << USB_PIPECFG_TYPE_SHIFT) |			// TYPE 1: Bulk transfer
			1 * (1u << 9) |				// DBLB
			0;
		const unsigned bufsize64 = (VIRTUAL_COM_PORT_OUT_DATA_SIZE + 63) / 64;

		Instance->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		Instance->PIPEMAXP = VIRTUAL_COM_PORT_OUT_DATA_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 2; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		Instance->PIPESEL = 0;
	}
	if (1)
	{
		// Данные CDC в компьютер из трансивера
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_CDC_IN;	// PIPE4
		const uint_fast8_t epnum = USBD_EP_CDC_IN;
		const uint_fast8_t dir = 1;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		Instance->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((Instance->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 4);
		Instance->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			1 * (1u << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 1: Bulk transfer
			1 * USB_PIPECFG_DBLB |		// DBLB
			0;
		const unsigned bufsize64 = (VIRTUAL_COM_PORT_IN_DATA_SIZE + 63) / 64;

		Instance->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		Instance->PIPEMAXP = VIRTUAL_COM_PORT_IN_DATA_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 2; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		Instance->PIPESEL = 0;
	}
	if (1)
	{
		// Прерывание CDC в компьютер из трансивера
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_CDC_INT;	// PIPE6
		const uint_fast8_t epnum = USBD_EP_CDC_INT;
		const uint_fast8_t dir = 1;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		Instance->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((Instance->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 6);
		Instance->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			2 * (1u << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 2: Interrupt transfer
			0 * USB_PIPECFG_DBLB |		// DBLB - для interrupt должен быть 0
			0;
		const unsigned bufsize64 = (VIRTUAL_COM_PORT_INT_SIZE + 63) / 64;
		Instance->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		Instance->PIPEMAXP = VIRTUAL_COM_PORT_INT_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 1; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		Instance->PIPESEL = 0;
	}
	if (1)
	{
		// Данные CDC из компьютера в трансивер
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_CDC_OUTb;	// PIPE14
		const uint_fast8_t epnum = USBD_EP_CDC_OUTb;
		const uint_fast8_t dir = 0;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		Instance->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((Instance->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 14);
		Instance->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |	// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |			// DIR 1: Transmitting direction 0: Receiving direction
			1 * (1u << USB_PIPECFG_TYPE_SHIFT) |			// TYPE 1: Bulk transfer
			1 * (1u << 9) |				// DBLB
			0;
		const unsigned bufsize64 = (VIRTUAL_COM_PORT_OUT_DATA_SIZE + 63) / 64;
		Instance->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		Instance->PIPEMAXP = VIRTUAL_COM_PORT_OUT_DATA_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 2; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		Instance->PIPESEL = 0;
	}
	if (1)
	{
		// Данные CDC в компьютер из трансивера
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_CDC_INb;	// PIPE15
		const uint_fast8_t epnum = USBD_EP_CDC_INb;
		const uint_fast8_t dir = 1;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		Instance->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((Instance->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 15);
		Instance->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			1 * (1u << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 1: Bulk transfer
			1 * USB_PIPECFG_DBLB |		// DBLB
			0;
		const unsigned bufsize64 = (VIRTUAL_COM_PORT_IN_DATA_SIZE + 63) / 64;
		Instance->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		Instance->PIPEMAXP = VIRTUAL_COM_PORT_IN_DATA_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 2; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		Instance->PIPESEL = 0;
	}
	if (1)
	{
		// Прерывание CDC в компьютер из трансивера
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_CDC_INTb;	// PIPE7
		const uint_fast8_t epnum = USBD_EP_CDC_INTb;
		const uint_fast8_t dir = 1;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		Instance->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((Instance->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 7);
		Instance->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			2 * (1u << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 2: Interrupt transfer
			0 * USB_PIPECFG_DBLB |		// DBLB - для interrupt должен быть 0
			0;
		const unsigned bufsize64 = (VIRTUAL_COM_PORT_INT_SIZE + 63) / 64;
		Instance->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		Instance->PIPEMAXP = VIRTUAL_COM_PORT_INT_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 1; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		Instance->PIPESEL = 0;
	}
#endif /* WITHUSBCDC */

#if WITHUSBUAC
	if (1)
	{
		// Данные AUDIO из трансивера в компьютер
		// Используется канал DMA D1
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_ISOC_IN;	// PIPE2
		const uint_fast8_t epnum = USBD_EP_AUDIO_IN;
		const uint_fast8_t dir = 1;

		const uint_fast16_t maxpacket = usbd_getuacinmaxpacket();
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		Instance->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((Instance->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 2);
		Instance->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			3 * (1u << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 11: Isochronous transfer
			////1 * USB_PIPECFG_DBLB |		// DBLB - убрано, т.к PIPEMAXP динамически меняется - поведение не понятно.
			0;
		//Instance->PIPEPERI =
		//	1 * (1U << 12) |	// IFS
		//	0;
		const unsigned bufsize64 = (maxpacket + 63) / 64;
		Instance->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		Instance->PIPEMAXP = maxpacket << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 2; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		Instance->PIPESEL = 0;

		// Разрешение DMA
		// Сперва без DREQE
		Instance->D1FIFOSEL =
			pipe * (1uL << USB_DnFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
			2 * (1uL << USB_DnFIFOSEL_MBW_SHIFT) |	// MBW 10: 32-bit width
			0 * (1uL << USB_DnFIFOSEL_DREQE_SHIFT) | // DREQE 1: DMA transfer request is enabled.
			0;
		(void) Instance->D1FIFOSEL;

		// Потом выставить DREQE
		Instance->D1FIFOSEL =
			pipe * (1uL << USB_DnFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
			2 * (1uL << USB_DnFIFOSEL_MBW_SHIFT) |	// MBW 10: 32-bit width
			1 * (1uL << USB_DnFIFOSEL_DREQE_SHIFT) | // DREQE 1: DMA transfer request is enabled.
			0;
		(void) Instance->D1FIFOSEL;
	}
#endif /* WITHUSBUAC */
#if WITHUSBUAC
	if (1)
	{
		// Данные AUDIO из компьютера в трансивер
		// Используется канал DMA D0
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_ISOC_OUT;	// PIPE1
		const uint_fast8_t epnum = USBD_EP_AUDIO_OUT;
		const uint_fast8_t dir = 0;
		const uint_fast16_t maxpacket = VIRTUAL_AUDIO_PORT_DATA_SIZE_OUT;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		Instance->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((Instance->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 1);
		Instance->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			3 * (1u << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 11: Isochronous transfer
			1 * USB_PIPECFG_DBLB |		// DBLB
			0;

		const unsigned bufsize64 = (maxpacket + 63) / 64;
		Instance->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		Instance->PIPEMAXP = maxpacket << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 2; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		Instance->PIPESEL = 0;
		// Разрешение DMA
		// Сперва без DREQE
		Instance->D0FIFOSEL =
			pipe * (1uL << USB_DnFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
			2 * (1uL << USB_DnFIFOSEL_MBW_SHIFT) |	// MBW 10: 32-bit width
			0 * (1uL << USB_DnFIFOSEL_DREQE_SHIFT) | // DREQE 1: DMA transfer request is enabled.
			0;
		(void) Instance->D0FIFOSEL;

		// Потом выставить DREQE
		Instance->D0FIFOSEL =
			pipe * (1uL << USB_DnFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
			2 * (1uL << USB_DnFIFOSEL_MBW_SHIFT) |	// MBW 10: 32-bit width
			1 * (1uL << USB_DnFIFOSEL_DREQE_SHIFT) | // DREQE 1: DMA transfer request is enabled.
			0;
		(void) Instance->D0FIFOSEL;
	}
#endif /* WITHUSBUAC */

	/*
	uint_fast8_t pipe;
	for (pipe = 1; pipe <= 15; ++ pipe)
	{
		Instance->PIPESEL = pipe;
		PRINTF(PSTR("USB pipe%02d PIPEBUF=%04X PIPEMAXP=%u\n"), pipe, Instance->PIPEBUF, Instance->PIPEMAXP & USB_PIPEMAXP_MXPS);
	}
	*/
}



static void usbd_handle_resume(PCD_TypeDef * const USBx)
{
	//PRINTF(PSTR("usbd_handle_resume\n"));

	//memset(altinterfaces, 0, sizeof altinterfaces);
#if WITHUSBCDC && 0
	usb_cdc_control_state [INTERFACE_CDC_CONTROL_3a] = 0;
	usb_cdc_control_state [INTERFACE_CDC_CONTROL_3b] = 0;
	dwDTERate [INTERFACE_CDC_CONTROL_3a] = 115200;
	dwDTERate [INTERFACE_CDC_CONTROL_3b] = 115200;
#endif /* WITHUSBCDC */
#if WITHUSBUAC && 0
	terminalsprops [TERMINAL_ID_SELECTOR_6] [AUDIO_CONTROL_UNDEFINED] = 1;
	buffers_set_uacinalt(altinterfaces [INTERFACE_AUDIO_MIKE]);
	buffers_set_uacoutalt(altinterfaces [INTERFACE_AUDIO_SPK]);
	#if WITHUSBUACIN2
		buffers_set_uacinrtsalt(altinterfaces [INTERFACE_AUDIO_RTS]);
	#endif /* WITHUSBUACIN2 */
#endif /* WITHUSBUAC */
}

// not used now
static void usbd_handle_suspend(PCD_TypeDef * const USBx)
{
	//PRINTF(PSTR("usbd_handle_suspend\n"));
	usbd_pipes_disable(USBx);

	//memset(altinterfaces, 0, sizeof altinterfaces);
#if WITHUSBCDC && 0
	/* при потере связи с host снять запрос на передачу */
	usb_cdc_control_state [INTERFACE_CDC_CONTROL_3a] = 0;
	usb_cdc_control_state [INTERFACE_CDC_CONTROL_3b] = 0;
	HARDWARE_CDC_ONDISCONNECT();
#endif /* WITHUSBCDC */
#if WITHUSBUAC && 0
	terminalsprops [TERMINAL_ID_SELECTOR_6] [AUDIO_CONTROL_UNDEFINED] = 1;
	buffers_set_uacinalt(altinterfaces [INTERFACE_AUDIO_MIKE]);
	buffers_set_uacoutalt(altinterfaces [INTERFACE_AUDIO_SPK]);
	#if WITHUSBUACIN2
		buffers_set_uacinrtsalt(altinterfaces [INTERFACE_AUDIO_RTS]);
	#endif /* WITHUSBUACIN2 */
#endif /* WITHUSBUAC */
}

#if defined (WITHUSBHW_DEVICE)
/*
	r7s721_usbi0_handler trapped
	 BRDYSTS=0x00000000
	 INTSTS0=0x0000F899

	 		(hpcd->Init.Sof_enable != USB_FALSE) * USB_INTENB0_SOFE |	// SOFE	1: Frame Number Update Interrupt Enable
		1 * USB_INTENB0_DVSE |	// DVSE
		//1 * USB_INTENB0_VBSE |	// VBSE
		1 * USB_INTENB0_CTRE |	// CTRE
		1 * USB_INTENB0_BEMPE |	// BEMPE
		1 * USB_INTENB0_NRDYE |	// NRDYE
		1 * USB_INTENB0_BRDYE |	// BRDYE
		1 * USB_INTENB0_RSME |	// RSME

*/
// Renesas, usb device
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
	USBD_HandleTypeDef * const pdev = hpcd->pData;
	const uint_fast16_t intsts0 = USBx->INTSTS0;
	const uint_fast16_t intsts1 = USBx->INTSTS1;
	const uint_fast16_t intsts0msk = intsts0 & USBx->INTENB0;
	const uint_fast16_t intsts1msk = intsts1 & USBx->INTENB1;

	if ((intsts0msk & USB_INTSTS0_SOFR) != 0)	// SOFR
	{
		USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_SOFR;	// Clear SOFR
		//PRINTF(PSTR("HAL_PCD_IRQHandler trapped - SOFR\n"));
		HAL_PCD_SOFCallback(hpcd);
	}
	if ((intsts0msk & USB_INTSTS0_BEMP) != 0)	// BEMP
	{
		//PRINTF(PSTR("HAL_PCD_IRQHandler trapped - BEMP, BEMPSTS=0x%04X\n"), USBx->BEMPSTS);
		const uint_fast16_t bempsts = USBx->BEMPSTS & USBx->BEMPENB;	// BEMP Interrupt Status Register
		USBx->BEMPSTS = ~ bempsts;

		if ((bempsts & (1U << 0)) != 0)	// PIPE0, DCP
		{
			HAL_PCD_DataInStageCallback(hpcd, 0);

			//if (control_transmit2(pdev) != 0)
			//{
			//	control_stall(pdev);
			//	TP();
			//}
		}
	}
	if ((intsts0msk & USB_INTSTS0_NRDY) != 0)	// NRDY
	{
		uint_fast8_t pipe;
		//PRINTF(PSTR("HAL_PCD_IRQHandler trapped - NRDY, NRDYSTS=0x%04X\n"), USBx->NRDYSTS);
		const uint_fast16_t nrdysts = USBx->NRDYSTS & USBx->NRDYENB;	// NRDY Interrupt Status Register
		for (pipe = 0; pipe < 16; ++ pipe)
		{
			const uint_fast16_t mask = (uint_fast16_t) 1 << pipe;
			if ((nrdysts & mask) != 0)
			{
				USBx->NRDYSTS = ~ mask;
				usbd_handler_nrdy(pdev, pipe);
			}
		}
	}
	if ((intsts0msk & USB_INTSTS0_BRDY) != 0)	// BRDY
	{
		uint_fast8_t i;
		//PRINTF(PSTR("HAL_PCD_IRQHandler trapped - BRDY, BRDYSTS=0x%04X\n"), USBx->BRDYSTS);
		const uint_fast16_t brdysts = USBx->BRDYSTS & USBx->BRDYENB;	// BRDY Interrupt Status Register
		USBx->BRDYSTS = ~ brdysts;

		if ((brdysts & (1U << 0)) != 0)		// PIPE0 - DCP
		{
			USB_OTG_EPTypeDef * const ep = & hpcd->OUT_ep [0];
		  	unsigned count;
		  	if (usbd_read_data(USBx, 0, ep->xfer_buff, ep->xfer_len - ep->xfer_count, & count) == 0)
		  	{
				//usbd_handler_brdy8_dcp_out(pdev, 0);	// usbd_read_data inside
				ep->xfer_buff += count;
				ep->xfer_count += count;
		  	}
		  	else
		  	{
		  		control_stall(pdev);
		  	}
		}

		for (i = 0; i < sizeof brdyenbpipes2 / sizeof brdyenbpipes2 [0]; ++ i)
		{
			const uint_fast8_t pipe = brdyenbpipes2 [i].pipe;
			const uint_fast8_t epnt = brdyenbpipes2 [i].ep;
			if ((brdysts & (1U << pipe)) != 0)
			{
				if ((epnt & 0x80) != 0)
				{
					//usbd_handler_brdy_bulk_in8(pdev, pipe, ep);	// usbd_write_data inside
					USB_OTG_EPTypeDef * const ep = & hpcd->IN_ep [epnt & 0x7F];
				  	if (usbd_write_data(USBx, ep->pipe_num, ep->xfer_buff, ep->xfer_len) == 0)
				  	{
						ep->xfer_buff += ep->xfer_len;
						ep->xfer_count += ep->xfer_len;
				  	}
				  	else
				  	{
				  		// todo: not control ep
				  		//control_stall(pdev);
				  	}
					HAL_PCD_DataInStageCallback(hpcd, 0x7f & epnt);
				}
				else
				{
					//usbd_handler_brdy_bulk_out8(pdev, pipe, ep);	// usbd_read_data inside
					USB_OTG_EPTypeDef * const ep = & hpcd->OUT_ep [epnt];
				  	unsigned count;
				  	if (usbd_read_data(USBx, ep->pipe_num, ep->xfer_buff, ep->xfer_len - ep->xfer_count, & count) == 0)
				  	{
						ep->xfer_buff += count;
						ep->xfer_count += count;
				  	}
				  	else
				  	{
				  		// todo: not control ep
				  		//control_stall(pdev);
				  		TP();
				  	}
					HAL_PCD_DataOutStageCallback(hpcd, ep->num);	// start next transfer
				}
			}
		}
	}
	if ((intsts0msk & USB_INTSTS0_CTRT) != 0)	// CTRT
	{
		const uint_fast8_t ctsq = (intsts0 & USB_INTSTS0_CTSQ) >> USB_INTSTS0_CTSQ_SHIFT;
		if (0) //((intsts0 & USB_INTSTS0_VALID) != 0)
		{
			// Setup syage detectd
			usb_save_request(USBx, & pdev->request);
			USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_VALID;	// Clear VALID - in seq 1, 3 and 5
			if (ctsq != 0 && ctsq != 1 && ctsq != 3 && ctsq != 5)
			{
				PRINTF("srsq=%d\n", (int) ctsq);
			}
			ASSERT(ctsq == 0 || ctsq == 1 || ctsq == 3 || ctsq == 5);
			//ASSERT((intsts0 & USB_INTSTS0_VALID) != 0);
			HAL_PCD_SetupStageCallback(hpcd);
	        //PRINTF("setup phase ended\n");
		}
		//PRINTF(PSTR("HAL_PCD_IRQHandler trapped - CTRT, CTSQ=%d\n"), (intsts0 >> 0) & 0x07);
		USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_CTRT;	// Clear CTRT

		usbd_handle_ctrt(hpcd, ctsq);
	}
	if ((intsts0msk & USB_INTSTS0_DVST) != 0)	// DVSE
	{
		//PRINTF(PSTR("HAL_PCD_IRQHandler trapped - DVSE, DVSQ=%d\n"), (intsts0 & USB_INTSTS0_DVSQ) >> USB_INTSTS0_DVSQ_SHIFT);
		USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_DVST;	// Clear DVSE
		switch ((intsts0 & USB_INTSTS0_DVSQ) >> USB_INTSTS0_DVSQ_SHIFT)
		{
		case 0x01:
			HAL_PCD_ResetCallback(hpcd);
			break;

		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
			//HAL_PCD_SuspendCallback(hpcd);
			break;

		case 0x02:
			// address state
			HAL_PCD_AdressedCallback(hpcd);
			break;
		//case xxx:
		//	HAL_PCD_ResumeCallback(hpcd);
		//	break;
		default:
			break;
		}
	}
	if ((intsts0msk & USB_INTSTS0_RESM) != 0)	// RESM
	{
		USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_RESM;	// Clear RESM
		//PRINTF(PSTR("HAL_PCD_IRQHandler trapped - RESM\n"));
		usbd_pipes_enable(USBx);
		usbd_handle_resume(USBx);
	}
	if ((intsts0msk & USB_INTSTS0_VBINT) != 0)	// VBINT
	{
		USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_VBINT;	// Clear VBINT - enabled by VBSE
		PRINTF(PSTR("HAL_PCD_IRQHandler trapped - VBINT, VBSTS=%d\n"), (intsts0 & USB_INTSTS0_VBSTS) != 0);
	//	usbd_handle_vbuse(usbd_getvbus());
	}
}

void device_USBI0_IRQHandler(void)
{
	HAL_PCD_IRQHandler(& hpcd_USB_OTG);
}

void device_USBI1_IRQHandler(void)
{
	HAL_PCD_IRQHandler(& hpcd_USB_OTG);
}



void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{

	if (hpcd->Instance == & USB200)
	{
		const IRQn_ID_t int_id = USBI0_IRQn;
		IRQ_SetHandler(int_id, device_USBI0_IRQHandler);
		IRQ_SetPriority(int_id, ARM_SYSTEM_PRIORITY);
		IRQ_Enable(int_id);

		/* ---- Supply clock to the USB20(channel 0) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */

		HARDWARE_USB0_INITIALIZE();

	}
	else if (hpcd->Instance == & USB201)
	{
		const IRQn_ID_t int_id = USBI1_IRQn;
		IRQ_SetHandler(int_id, device_USBI1_IRQHandler);
		IRQ_SetPriority(int_id, ARM_SYSTEM_PRIORITY);
		IRQ_Enable(int_id);

		/* ---- Supply clock to the USB20(channel 1) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP70;	// Module Stop 70 0: Channel 1 of the USB 2.0 host/function module runs.
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */

		HARDWARE_USB1_INITIALIZE();
	}

}

// Renesas, usb host
void HAL_HCD_IRQHandler(HCD_HandleTypeDef *hhcd)
{
	USB_OTG_GlobalTypeDef * const USBx = hhcd->Instance;
	const uint_fast16_t intsts0 = USBx->INTSTS0;
	const uint_fast16_t intsts1 = USBx->INTSTS1;
	const uint_fast16_t intsts0msk = intsts0 & USBx->INTENB0;
	const uint_fast16_t intsts1msk = intsts1 & USBx->INTENB1;

	//PRINTF(PSTR("HAL_HCD_IRQHandler trapped, intsts0=%04X, intsts1=%04X\n"), intsts0, intsts1);
	if ((intsts0msk & USB_INTSTS0_SOFR) != 0)	// SOFR
	{
		USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_SOFR;	// Clear SOFR
		//PRINTF(PSTR("HAL_HCD_IRQHandler trapped - SOFR\n"));
	}
	if ((intsts1msk & USB_INTSTS1_BCHG) != 0)	// BCHG
	{
		USBx->INTSTS1 = (uint16_t) ~ USB_INTSTS1_BCHG;
		//PRINTF(PSTR("HAL_HCD_IRQHandler trapped - BCHG\n"));
	}
	if ((intsts1msk & USB_INTSTS1_DTCH) != 0)	// DTCH
	{
		USBx->INTSTS1 = (uint16_t) ~ USB_INTSTS1_DTCH;
		//PRINTF(PSTR("HAL_HCD_IRQHandler trapped - DTCH\n"));
		HAL_HCD_Disconnect_Callback(hhcd);
	}
	if ((intsts1msk & USB_INTSTS1_ATTCH) != 0)	// ATTCH
	{
		USBx->INTSTS1 = (uint16_t) ~ USB_INTSTS1_ATTCH;
		//PRINTF(PSTR("HAL_HCD_IRQHandler trapped - ATTCH\n"));
		HAL_HCD_Connect_Callback(hhcd);
	}
	if ((intsts0msk & (1uL << USB_INTSTS0_VBINT_SHIFT)) != 0)	// VBINT
	{
		USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_VBINT;	// Clear VBINT - enabled by VBSE
		//PRINTF(PSTR("HAL_HCD_IRQHandler trapped - VBINT, VBSTS=%d\n"), (intsts0 & USB_INTSTS0_VBSTS) != 0);	// TODO: masked VBSTS
	//	usbd_handle_vbuse(usbd_getvbus());
	}
}

static void host_USBI0_IRQHandler(void)
{
	HAL_HCD_IRQHandler(& hhcd_USB_OTG);
}

static void host_USBI1_IRQHandler(void)
{
	HAL_HCD_IRQHandler(& hhcd_USB_OTG);
}

void HAL_HCD_MspInit(HCD_HandleTypeDef* hpcd)
{
	if (hpcd->Instance == & USB200)
	{
		const IRQn_ID_t int_id = USBI0_IRQn;
		IRQ_SetHandler(int_id, host_USBI0_IRQHandler);
		IRQ_SetPriority(int_id, ARM_SYSTEM_PRIORITY);
		IRQ_Enable(int_id);

		/* ---- Supply clock to the USB20(channel 0) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */

		HARDWARE_USB0_INITIALIZE();

	}
	else if (hpcd->Instance == & USB201)
	{
		const IRQn_ID_t int_id = USBI1_IRQn;
		IRQ_SetHandler(int_id, host_USBI1_IRQHandler);
		IRQ_SetPriority(int_id, ARM_SYSTEM_PRIORITY);
		IRQ_Enable(int_id);

		/* ---- Supply clock to the USB20(channel 1) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP70;	// Module Stop 70 0: Channel 1 of the USB 2.0 host/function module runs.
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */

		HARDWARE_USB1_INITIALIZE();
	}
}

void HAL_HCD_MspDeInit(HCD_HandleTypeDef* hpcd)
{
	hpcd->Instance->SYSCFG0 &= ~ USB_SYSCFG_USBE;
	hpcd->Instance->INTENB0 = 0;
	hpcd->Instance->INTENB1 = 0;
}

/**
  * @brief  Return Host Current Frame number
  * @param  USBx : Selected device
  * @retval current frame number
*/
uint32_t USB_GetCurrentFrame (USB_OTG_GlobalTypeDef *USBx)
{
	return (USBx->FRMNUM & USB_FRMNUM_FRNM) >> USB_FRMNUM_FRNM_SHIFT;
}


/**
  * @brief  Return Host Core speed
  * @param  USBx : Selected device
  * @retval speed : Host speed
  *          This parameter can be one of these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  *            @arg USB_OTG_SPEED_LOW: Low speed mode
  */
uint32_t USB_GetHostSpeed (USB_OTG_GlobalTypeDef *USBx)
{
	switch ((USBx->DVSTCTR0 & USB_DVSTCTR0_RHST) >> USB_DVSTCTR0_RHST_SHIFT)
	{
	case 0x01:
		return USB_OTG_SPEED_LOW;
	case 0x02:
		return USB_OTG_SPEED_FULL;
	case 0x03:
		return USB_OTG_SPEED_HIGH;
	default:
		return USB_OTG_SPEED_LOW;
	}
}


/**
  * @brief  USB_DriveVbus : activate or de-activate vbus
  * @param  state : VBUS state
  *          This parameter can be one of these values:
  *           0 : VBUS Active
  *           1 : VBUS Inactive
  * @retval HAL status
*/
HAL_StatusTypeDef USB_DriveVbus (USB_OTG_GlobalTypeDef *USBx, uint_fast8_t state)
{
	// renesas version
	/*
  uint32_t hprt0 = USBx_HPRT0;
  hprt0 &= ~(USB_OTG_HPRT_PENA    | USB_OTG_HPRT_PCDET |
                         USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG );

  if (((hprt0 & USB_OTG_HPRT_PPWR) == 0 ) && (state == 1 ))
  {
    USBx_HPRT0 = (USB_OTG_HPRT_PPWR | hprt0);
  }
  if (((hprt0 & USB_OTG_HPRT_PPWR) == USB_OTG_HPRT_PPWR) && (state == 0 ))
  {
    USBx_HPRT0 = ((~USB_OTG_HPRT_PPWR) & hprt0);
  }
  */
  return HAL_OK;
}


/**
  * @brief  USB_EnableGlobalInt
  *         Enables the controller's Global Int in the AHB Config reg
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EnableGlobalInt(USB_OTG_GlobalTypeDef *USBx)
{
  ////USBx->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
  return HAL_OK;
}


/**
  * @brief  USB_DisableGlobalInt
  *         Disable the controller's Global Int in the AHB Config reg
  * @param  USBx : Selected device
  * @retval HAL status
*/
HAL_StatusTypeDef USB_DisableGlobalInt(USB_OTG_GlobalTypeDef *USBx)
{
  ////USBx->GAHBCFG &= ~ USB_OTG_GAHBCFG_GINT;
  return HAL_OK;
}


#endif /* defined (WITHUSBHW_DEVICE) */

#define __HAL_PCD_ENABLE(h)                   do { /*USB_EnableGlobalInt((h)->Instance); */} while (0)
#define __HAL_PCD_DISABLE(h)                  do { /*USB_DisableGlobalInt((h)->Instance); */} while (0)


/**
  * @brief  USB_SetCurrentMode : Set functional mode
  * @param  USBx : Selected device
  * @param  mode :  current core mode
  *          This parameter can be one of these values:
  *            @arg USB_OTG_DEVICE_MODE: Peripheral mode
  *            @arg USB_OTG_HOST_MODE: Host mode
  *            @arg USB_OTG_DRD_MODE: Dual Role Device mode
  * @retval HAL status
  */
HAL_StatusTypeDef USB_SetCurrentMode(USB_OTG_GlobalTypeDef *USBx, USB_OTG_ModeTypeDef mode)
{
	switch (mode)
	{
	case USB_OTG_DEVICE_MODE:
		USBx->SYSCFG0 &= ~ USB_SYSCFG_DCFM;	// DCFM01: Devide controller mode is selected
		(void) USBx->SYSCFG0;
		break;
	case USB_OTG_HOST_MODE:
		USBx->SYSCFG0 |= USB_SYSCFG_DCFM;	// DCFM 1: Host controller mode is selected
		(void) USBx->SYSCFG0;
		break;
	case USB_OTG_DRD_MODE:
		ASSERT(0);
		//USBx->SYSCFG0 |= USB_SYSCFG_DCFM;	// DCFM 1: Host controller mode is selected
		break;
	}

	return HAL_OK;
}
/**
  * @brief  Activate and configure an endpoint
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_ActivateEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
{
	return HAL_OK;
}

/**
  * @brief  Activate and configure a dedicated endpoint
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_ActivateDedicatedEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
{
	return HAL_OK;
}

/**
  * @brief  De-activate and de-initialize an endpoint
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_DeactivateEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
{
	return HAL_OK;
}

/**
  * @brief  USB_DevDisconnect : Disconnect the USB device by disabling the pull-up/pull-down
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_DevDisconnect (USB_OTG_GlobalTypeDef *USBx)
{
	//PRINTF(PSTR("USB_DevDisconnect (USBx=%p)\n"), USBx);

	USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_DPRPU | USB_SYSCFG_DRPD)) |
			0 * USB_SYSCFG_DPRPU |	// DPRPU 0: Pulling up the D+ line is disabled.
			0 * USB_SYSCFG_DRPD |	// DRPD0: Pulling down the lines is disabled.
			0;
	(void) USBx->SYSCFG0;

	HARDWARE_DELAY_MS(3);

	return HAL_OK;
}

/**
  * @brief  USB_DevConnect : Connect the USB device by enabling the pull-up/pull-down
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_DevConnect (USB_OTG_GlobalTypeDef *USBx)
{
	//PRINTF(PSTR("USB_DevConnect (USBx=%p)\n"), USBx);

	USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_DPRPU | USB_SYSCFG_DRPD)) |
			1 * USB_SYSCFG_DPRPU |	// DPRPU 1: Pulling up the D+ line is enabled.
			0 * USB_SYSCFG_DRPD |	// DRPD 0: Pulling down the lines is disabled.
			0;
	(void) USBx->SYSCFG0;

	HARDWARE_DELAY_MS(3);

	return HAL_OK;
}

/**
  * @brief  USB_EP0StartXfer : setup and starts a transfer over the EP  0
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @param  dma: USB dma enabled or disabled
  *          This parameter can be one of these values:
  *           0 : DMA feature not used
  *           1 : DMA feature used
  * @retval HAL status
  */

HAL_StatusTypeDef USB_EP0StartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep, uint_fast8_t dma)
{
	const uint_fast8_t pipe = 0;	// DCP
	ASSERT(dma == 0);
	//PRINTF(PSTR("USB_EP0StartXfer\n"));
	if (ep->is_in == 1)
	{
		//PRINTF(PSTR("USB_EP0StartXfer: IN direction, ep->xfer_len=%d, ep->maxpacket=%d\n"), (int) ep->xfer_len, (int) ep->maxpacket);

		USBx->DCPCTR = (USBx->DCPCTR & ~ USB_DCPCTR_PID) |
			DEVDRV_USBF_PID_NAK * (1uL << USB_DCPCTR_PID_SHIFT) |	// PID 00: NAK
			0;

		/* IN endpoint */
		if (ep->xfer_len == 0)
		{
  		   /* Zero Length Packet */
			int err = usbd_write_data(USBx, pipe, NULL, 0);	// pipe=0: DCP
			////ASSERT(err == 0);
		}
		else
		{
			if(ep->xfer_len > ep->maxpacket)
			{
				ep->xfer_len = ep->maxpacket;
			}
			int err = usbd_write_data(USBx, pipe, ep->xfer_buff, ep->xfer_len);	// pipe=0: DCP
			////ASSERT(err == 0);
			// эти манипуляци пока не по оригиналу
			ep->xfer_buff += ep->xfer_len;
			ep->xfer_count += ep->xfer_len;
		}

		USBx->DCPCTR = (USBx->DCPCTR & ~ USB_DCPCTR_PID) |
			DEVDRV_USBF_PID_BUF * (1uL << USB_DCPCTR_PID_SHIFT) |	// PID 01: BUF response (depending on the buffer state)
			0;
	}
	else
	{
		//PRINTF(PSTR("USB_EP0StartXfer: OUT direction\n"));
		/* OUT endpoint */
		if (ep->xfer_len > 0)
		{
			ep->xfer_len = ep->maxpacket;
		}
		// initiate read
	}
	return HAL_OK;
}

/**
  * @brief  USB_EPStartXfer : setup and starts a transfer over an EP
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @param  dma: USB dma enabled or disabled
  *          This parameter can be one of these values:
  *           0 : DMA feature not used
  *           1 : DMA feature used
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EPStartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep, uint_fast8_t dma)
{
	if (ep->is_in == 1)
	{
		//PRINTF(PSTR("USB_EPStartXfer IN, ep->num=%d, ep->pipe_num=%d\n"), (int) ep->num, (int) ep->pipe_num);
		/* IN endpoint */
	}
	else
	{
		//PRINTF(PSTR("USB_EPStartXfer OUT, ep->num=%d, ep->pipe_num=%d\n"), (int) ep->num, (int) ep->pipe_num);
		/* OUT endpoint */

		/* EP enable */
		//outep->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
	}
	return HAL_OK;
}

/**
  * @brief  Start a transfer over a host channel
  * @param  USBx : Selected device
  * @param  hc : pointer to host channel structure
  * @param  dma: USB dma enabled or disabled
  *          This parameter can be one of the these values:
  *           0 : DMA feature not used
  *           1 : DMA feature used
  * @retval HAL state
  */

HAL_StatusTypeDef USB_HC_StartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_HCTypeDef *hc, uint_fast8_t dma)
{
	ASSERT(dma == 0);

	return HAL_OK;
}

/**
  * @brief  Halt a host channel
  * @param  USBx : Selected device
  * @param  hc_num : Host Channel number
  *         This parameter can be a value from 1 to 15
  * @retval HAL state
  */
HAL_StatusTypeDef USB_HC_Halt(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t hc_num)
{
	return HAL_OK;
}


/**
* @brief  USB_OTG_ResetPort : Reset Host Port
  * @param  USBx : Selected device
  * @param  state : activate reset
  * @retval HAL status
  * @note : (1)The application must wait at least 10 ms
  *   before clearing the reset bit.
  */
HAL_StatusTypeDef USB_ResetPort(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t state)
{
	if (state)
	{
		USBx->DVSTCTR0 |= USB_DVSTCTR0_USBRST;
		(void) USBx->DVSTCTR0;
	}
	else
	{
		USBx->DVSTCTR0 &= ~ USB_DVSTCTR0_USBRST;
		(void) USBx->DVSTCTR0;
	}

	return HAL_OK;
}

/**
  * @brief  Initialize a host channel
  * @param  USBx : Selected device
  * @param  ch_num : Channel number
  *         This parameter can be a value from 1 to 15
  * @param  epnum : Endpoint number
  *          This parameter can be a value from 1 to 15
  * @param  dev_address : Current device address
  *          This parameter can be a value from 0 to 255
  * @param  speed : Current device speed
  *          This parameter can be one of the these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  *            @arg USB_OTG_SPEED_LOW: Low speed mode
  * @param  ep_type : Endpoint Type
  *          This parameter can be one of the these values:
  *            @arg USBD_EP_TYPE_CTRL: Control type
  *            @arg USBD_EP_TYPE_ISOC: Isochronous type
  *            @arg USBD_EP_TYPE_BULK: Bulk type
  *            @arg USBD_EP_TYPE_INTR: Interrupt type
  * @param  mps : Max Packet Size
  *          This parameter can be a value from 0 to32K
  * @retval HAL state
  */
HAL_StatusTypeDef USB_HC_Init(USB_OTG_GlobalTypeDef *USBx,
                              uint8_t ch_num,
                              uint8_t epnum,
                              uint8_t dev_address,
                              uint8_t speed,
                              uint8_t ep_type,
                              uint16_t mps)
{
	return HAL_OK;
}

/**
  * @brief  USB_HostInit : Initializes the USB OTG controller registers
  *         for Host mode
  * @param  USBx : Selected device
  * @param  cfg  : pointer to a USB_OTG_CfgTypeDef structure that contains
  *         the configuration information for the specified USBx peripheral.
  * @retval HAL status
  */

HAL_StatusTypeDef USB_HostInit(USB_OTG_GlobalTypeDef *USBx, const USB_OTG_CfgTypeDef * cfg)
{
	//hpcd->Instance->CFIFOSEL = 0;	// не помогает с первым чтением из DCP
	//hpcd->Instance->CFIFOCTR = USB_CFIFOCTR_BCLR;	// BCLR

	USBx->SYSCFG0 &= ~ USB_SYSCFG_USBE;	// USBE 0: USB module operation is disabled.
	(void) USBx->SYSCFG0;

	USBx->SOFCFG =
		//USB_SOFCFG_BRDYM |	// BRDYM
		0;

	USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_DPRPU | USB_SYSCFG_DRPD)) |
			0 * USB_SYSCFG_DPRPU |	// DPRPU 0: Pulling up the D+ line is disabled.
			1 * USB_SYSCFG_DRPD |	// DRPD 1: Pulling down the lines is enabled.
			0;
	(void) USBx->SYSCFG0;

	USBx->SYSCFG0 |= USB_SYSCFG_USBE;	// USBE 1: USB module operation is enabled.
	(void) USBx->SYSCFG0;

	USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_HSE)) |
			(cfg->speed == USBD_SPEED_HIGH) * USB_SYSCFG_HSE |	// HSE
			0;
	(void) USBx->SYSCFG0;

	USBx->INTSTS0 = 0;
	USBx->INTSTS1 = 0;

	/*
	The RSME, DVSE, and CTRE bits can be set to 1 only when the function controller mode is selected; do not set these bits to 1
	to enable the corresponding interrupt output when the host controller mode is selected.
	*/
	USBx->INTENB0 =
		(cfg->Sof_enable != USB_FALSE) * USB_INTENB0_SOFE |	// SOFE	1: Frame Number Update Interrupt Enable
		//1 * USB_INTENB0_VBSE |	// VBSE
		1 * USB_INTENB0_BEMPE |	// BEMPE
		1 * USB_INTENB0_NRDYE |	// NRDYE
		1 * USB_INTENB0_BRDYE |	// BRDYE
		//1 * USB_INTENB0_CTRE |	// CTRE
		//1 * USB_INTENB0_DVSE |	// DVSE
		//1 * USB_INTENB0_RSME |	// RSME
		0;

	USBx->INTENB1 =
		//1 * USB_INTENB1_BCHGE |	// BCHG
		1 * USB_INTENB1_DTCHE |	// DTCH
		1 * USB_INTENB1_ATTCHE |	// ATTCH
		0;


	return HAL_OK;
}

/**
  * @brief  Stop Host Core
  * @param  USBx : Selected device
  * @retval HAL state
  */
HAL_StatusTypeDef USB_StopHost(USB_OTG_GlobalTypeDef *USBx)
{
	return HAL_OK;
}
/**
  * @brief  Initiate Do Ping protocol
  * @param  USBx : Selected device
  * @param  hc_num : Host Channel number
  *         This parameter can be a value from 1 to 15
  * @retval HAL state
  */
HAL_StatusTypeDef USB_DoPing(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t ch_num)
{
	return HAL_OK;
}


/**
  * @brief  USB_EPSetStall : set a stall condition over an EP
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EPSetStall(USB_OTG_GlobalTypeDef *USBx, const USB_OTG_EPTypeDef *ep)
{
	uint_fast8_t pipe = ep->pipe_num;
	//PRINTF(PSTR("USB_EPSetStall\n"));

	if (pipe == 0)
	{
		USBx->DCPCTR = (USBx->DCPCTR & ~ (USB_DCPCTR_PID)) |
			//DEVDRV_USBF_PID_NAK * (1uL << 0) |	// PID 00: NAK
			//1 * (1uL << 0) |	// PID 01: BUF response (depending on the buffer state)
			DEVDRV_USBF_PID_STALL * (1uL << USB_DCPCTR_PID_SHIFT) |	// PID 02: STALL response
			0;
		(void) USBx->DCPCTR;
	}
	else
	{
/*
		USBx->CFIFOSEL =
			//1 * (1uL << USB_CFIFOSEL_RCNT_SHIFT) |		// RCNT
			(pipe << USB_CFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
			1 * (1uL << USB_CFIFOSEL_ISEL_SHIFT_) * (pipe == 0) |	// ISEL 1: Writing to the buffer memory is selected (for DCP)
			0;

		if (usbd_wait_fifo(USBx, pipe, USBD_FRDY_COUNT) == 0)
		{
			USBx->CFIFOCTR = USB_CFIFOCTR_BCLR;	// BCLR
		}
*/
	}


	return HAL_OK;
}

/**
  * @brief  USB_EPClearStall : Clear a stall condition over an EP
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EPClearStall(USB_OTG_GlobalTypeDef *USBx, const USB_OTG_EPTypeDef *ep)
{
	uint_fast8_t pipe = ep->pipe_num;
	//PRINTF(PSTR("USB_EPSetStall\n"));

	if (pipe == 0)
	{
		// TODO: check thos code
		USBx->DCPCTR = (USBx->DCPCTR & ~ (USB_DCPCTR_PID)) |
			DEVDRV_USBF_PID_NAK * (1uL << 0) |	// PID 00: NAK
			//1 * (1uL << 0) |	// PID 01: BUF response (depending on the buffer state)
			//DEVDRV_USBF_PID_STALL * (1uL << USB_DCPCTR_PID_SHIFT) |	// PID 02: STALL response
			0;
		(void) USBx->DCPCTR;
	}
	else
	{

	}
	return HAL_OK;
}

/**
  * @brief  Prepare the EP0 to start the first control setup
  * @param  USBx : Selected device
  * @param  dma: USB dma enabled or disabled
  *          This parameter can be one of these values:
  *           0 : DMA feature not used
  *           1 : DMA feature used
  * @param  psetup : pointer to setup packet
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EP0_OutStart(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t dma, uint8_t *psetup)
{
	return HAL_OK;
}

HAL_StatusTypeDef USB_FlushTxFifoAll(USB_OTG_GlobalTypeDef *USBx)
{
	return HAL_OK;
}

/**
  * @brief  USB_FlushRxFifo : Flush Rx FIFO
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef USB_FlushRxFifo(USB_OTG_GlobalTypeDef *USBx)
{
	return HAL_OK;
}
/**
  * @brief  De-activate and de-initialize a dedicated endpoint
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_DeactivateDedicatedEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
{
	return HAL_OK;
}

#endif /* CPUSTYLE_R7S721 */
// here add DFU

#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX


/* Exported macros -----------------------------------------------------------*/
/** @defgroup PCD_Exported_Macros PCD Exported Macros
 *  @brief macros to handle interrupts and specific clock configurations
 * @{
 */
#define __HAL_PCD_ENABLE(h)                   do { USB_EnableGlobalInt((h)->Instance); } while (0)
#define __HAL_PCD_DISABLE(h)                  do { USB_DisableGlobalInt((h)->Instance); } while (0)

#define __HAL_PCD_GET_FLAG(h, imask)      ((USB_ReadInterrupts((h)->Instance) & (imask)) == (imask))
#define __HAL_PCD_CLEAR_FLAG(h, imask)    do { (((h)->Instance->GINTSTS) = (imask)); } while (0)
#define __HAL_PCD_IS_INVALID_INTERRUPT(h)         (USB_ReadInterrupts((h)->Instance) == 0)


#define __HAL_PCD_UNGATE_PHYCLOCK(h) do { \
	*(__IO uint32_t *)((uint32_t)((h)->Instance) + USB_OTG_PCGCCTL_BASE) &= ~ (USB_OTG_PCGCCTL_STOPCLK); \
	} while (0)

#define __HAL_PCD_GATE_PHYCLOCK(h) do { \
	*(__IO uint32_t *)((uint32_t)((h)->Instance) + USB_OTG_PCGCCTL_BASE) |= USB_OTG_PCGCCTL_STOPCLK; \
	} while (0)

#define __HAL_PCD_IS_PHY_SUSPENDED(h)            ((*(__IO uint32_t *)((uint32_t)((h)->Instance) + USB_OTG_PCGCCTL_BASE))&0x10)

/** @defgroup USB_CORE_Frame_Interval_   USB CORE Frame Interval
  * @{
  */
#define DCFG_FRAME_INTERVAL_80                 0U
#define DCFG_FRAME_INTERVAL_85                 1U
#define DCFG_FRAME_INTERVAL_90                 2U
#define DCFG_FRAME_INTERVAL_95                 3U


#endif

#endif /* WITHUSBHW */

#if WITHUSBHW

#define UNUSED(x) ((void)(x))

	static uint32_t HAL_GetTick(void)
	{
		return HARDWARE_GETTICK_MS();
	}

/** @brief Reset the Handle's State field.
  * @param h: specifies the Peripheral Handle.
  * @note  This macro can be used for the following purpose:
  *          - When the Handle is declared as local variable; before passing it as parameter
  *            to HAL_PPP_Init() for the first time, it is mandatory to use this macro
  *            to set to 0 the Handle's "State" field.
  *            Otherwise, "State" field may have any random value and the first time the function
  *            HAL_PPP_Init() is called, the low level hardware initialization will be missed
  *            (i.e. HAL_PPP_MspInit() will not be executed).
  *          - When there is a need to reconfigure the low level hardware: instead of calling
  *            HAL_PPP_DeInit() then HAL_PPP_Init(), user can make a call to this macro then HAL_PPP_Init().
  *            In this later function, when the Handle's "State" field is set to 0, it will execute the function
  *            HAL_PPP_MspInit() which will reconfigure the low level hardware.
  * @retval None
  */
#define __HAL_RESET_HANDLE_STATE(h) ((h)->State = 0U)

#if (USE_RTOS == 1)
  /* Reserved for future use */
  #error "USE_RTOS should be 0 in the current HAL release"
#else
  #define __HAL_LOCK(h)                                           \
                                do{                                        \
                                    if ((h)->Lock == HAL_LOCKED)   \
                                    {                                      \
                                       return HAL_BUSY;                    \
                                    }                                      \
                                    else                                   \
                                    {                                      \
                                       (h)->Lock = HAL_LOCKED;    \
                                    }                                      \
                                  }while (0)

  #define __HAL_UNLOCK(h)                                          \
                                  do{                                       \
                                      (h)->Lock = HAL_UNLOCKED;    \
                                    }while (0)
#endif /* USE_RTOS */


HAL_StatusTypeDef USB_DoPing(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t ch_num);

#if CPUSTYLE_R7S721
// Place for USB_xxx functions
/**
  * @brief  USB_SetDevAddress : Stop the usb device mode
  * @param  USBx : Selected device
  * @param  address : new device address to be assigned
  *          This parameter can be a value from 0 to 255
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_SetDevAddress (USB_OTG_GlobalTypeDef *USBx, uint_fast8_t address)
{
	return HAL_OK;
}

/**
  * @brief  Initializes the USB Core
  * @param  USBx: USB Instance
  * @param  cfg : pointer to a USB_OTG_CfgTypeDef structure that contains
  *         the configuration information for the specified USBx peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef USB_CoreInit(USB_OTG_GlobalTypeDef *USBx, const USB_OTG_CfgTypeDef *cfg)
{
	USBx->SUSPMODE &= ~ USB_SUSPMODE_SUSPM;	// SUSPM 0: The clock supplied to this module is stopped.

	SYSCFG0_0 = (SYSCFG0_0 & ~ (USB_SYSCFG_UPLLE | USB_SYSCFG_UCKSEL)) |
		1 * USB_SYSCFG_UPLLE |	// UPLLE 1: Enables operation of the internal PLL.
		1 * USB_SYSCFG_UCKSEL |	// UCKSEL 1: The 12-MHz EXTAL clock is selected.
		0;
	HARDWARE_DELAY_MS(2);	// required 1 ms delay - see R01UH0437EJ0200 Rev.2.00 28.4.1 System Control and Oscillation Control

	USBx->SUSPMODE |= USB_SUSPMODE_SUSPM;	// SUSPM 1: The clock supplied to this module is enabled.

	return HAL_OK;
}

/**
  * @brief  USB_DevInit : Initializes the USB_OTG controller registers
  *         for device mode
  * @param  USBx : Selected device
  * @param  cfg  : pointer to a USB_OTG_CfgTypeDef structure that contains
  *         the configuration information for the specified USBx peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef USB_DevInit(USB_OTG_GlobalTypeDef *USBx, const USB_OTG_CfgTypeDef * cfg)
{
	USBx->SYSCFG0 &= ~ USB_SYSCFG_USBE;	// USBE 0: USB module operation is disabled.
	(void) USBx->SYSCFG0;

	USBx->SOFCFG =
		//USB_SOFCFG_BRDYM |	// BRDYM
		0;
	(void) USBx->SOFCFG;

	USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_DPRPU | USB_SYSCFG_DRPD)) |
			0 * USB_SYSCFG_DPRPU |	// DPRPU 0: Pulling up the D+ line is disabled.
			0 * USB_SYSCFG_DRPD |	// DRPD 0: Pulling down the lines is disabled.
			0;
	(void) USBx->SYSCFG0;

	USBx->SYSCFG0 |= USB_SYSCFG_USBE;	// USBE 1: USB module operation is enabled.
	(void) USBx->SYSCFG0;

	PRINTF("USB_DevInit: cfg->speed=%d\n", (int) cfg->speed);
	USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_HSE)) |
			(cfg->speed == USBD_SPEED_HIGH) * USB_SYSCFG_HSE |	// HSE
			0;
	(void) USBx->SYSCFG0;

	USBx->INTSTS0 = 0;
	USBx->INTSTS1 = 0;

	USBx->INTENB0 =
		(cfg->Sof_enable != USB_FALSE) * USB_INTENB0_SOFE |	// SOFE	1: Frame Number Update Interrupt Enable
		1 * USB_INTENB0_DVSE |	// DVSE
		//1 * USB_INTENB0_VBSE |	// VBSE
		1 * USB_INTENB0_CTRE |	// CTRE
		1 * USB_INTENB0_BEMPE |	// BEMPE
		1 * USB_INTENB0_NRDYE |	// NRDYE
		1 * USB_INTENB0_BRDYE |	// BRDYE
		1 * USB_INTENB0_RSME |	// RSME
		0;
	USBx->INTENB1 = 0;

	return HAL_OK;
}


/**
  * @brief  USB_StopDevice : Stop the usb device mode
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef USB_StopDevice(USB_OTG_GlobalTypeDef *USBx)
{
	return HAL_OK;
}

/**
  * @brief  USB_OTG_FlushTxFifo : Flush a Tx FIFO
  * @param  USBx : Selected device
  * @param  num : FIFO number
  *         This parameter can be a value from 1 to 15
            16 means Flush all Tx FIFOs
  * @retval HAL status
  */
HAL_StatusTypeDef USB_FlushTxFifo(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t num)
{
	return HAL_OK;
}

#endif /* CPUSTYLE_R7S721 */

#if (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX)

#ifdef USB_HS_PHYC
/**
  * @brief  Enables control of a High Speed USB PHYВ’s
  *         Init the low level hardware : GPIO, CLOCK, NVIC...
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef USB_HS_PHYCInit(USB_OTG_GlobalTypeDef *USBx)
{
  uint32_t count = 0;

  /* Enable LDO */
  USB_HS_PHYC->USB_HS_PHYC_LDO |= USB_HS_PHYC_LDO_ENABLE;

  /* wait for LDO Ready */
  while((USB_HS_PHYC->USB_HS_PHYC_LDO & USB_HS_PHYC_LDO_STATUS) == RESET)
  {
    if (++count > 200000)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Controls PHY frequency operation selection */
  if (REFINFREQ == 12000000) /* HSE = 12MHz */
  {
    USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x0 << 1);
  }
  else if (REFINFREQ == 12500000) /* HSE = 12.5MHz */
  {
    USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x2 << 1);
  }
  else if (REFINFREQ == 16000000) /* HSE = 16MHz */
  {
    USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x3 << 1);
  }

  else if (REFINFREQ == 24000000) /* HSE = 24MHz */
  {
    USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x4 << 1);
  }
  else if (REFINFREQ == 25000000) /* HSE = 25MHz */
  {
    USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x5 << 1);
  }
  else if (REFINFREQ == 32000000) /* HSE = 32MHz */
  {
    USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x7 << 1);
  }

  /* Control the tuning interface of the High Speed PHY */
  USB_HS_PHYC->USB_HS_PHYC_TUNE |= USB_HS_PHYC_TUNE_VALUE;

  /* Enable PLL internal PHY */
  USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL_PLLEN;

  /* 2ms Delay required to get internal phy clock stable */
  HARDWARE_DELAY_MS(2);

  return HAL_OK;
}

#endif /* USB_HS_PHYC */

/**
  * @brief  Initializes the USB Core
  * @param  USBx: USB Instance
  * @param  cfg : pointer to a USB_OTG_CfgTypeDef structure that contains
  *         the configuration information for the specified USBx peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef USB_CoreInit(USB_OTG_GlobalTypeDef *USBx, const USB_OTG_CfgTypeDef *cfg)
{
  if (cfg->phy_itface == USB_OTG_ULPI_PHY)
  {
    USBx->GCCFG &= ~(USB_OTG_GCCFG_PWRDWN);

    /* Init The ULPI Interface */
    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);

    /* Select vbus source */
    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
    if(cfg->use_external_vbus == USB_ENABLE)
    {
      USBx->GUSBCFG |= USB_OTG_GUSBCFG_ULPIEVBUSD;
    }
    /* Reset after a PHY select  */
    USB_CoreReset(USBx);
  }
#ifdef USB_HS_PHYC

  else if (cfg->phy_itface == USB_OTG_HS_EMBEDDED_PHY)
  {
    USBx->GCCFG &= ~(USB_OTG_GCCFG_PWRDWN);

    /* Init The UTMI Interface */
    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);

    /* Select vbus source */
    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);

    /* Select UTMI Interace */
    USBx->GUSBCFG &= ~ USB_OTG_GUSBCFG_ULPI_UTMI_SEL;
    USBx->GCCFG |= USB_OTG_GCCFG_PHYHSEN;

    /* Enables control of a High Speed USB PHY */
    USB_HS_PHYCInit(USBx);

    if(cfg->use_external_vbus == USB_ENABLE)
    {
      USBx->GUSBCFG |= USB_OTG_GUSBCFG_ULPIEVBUSD;
    }
    /* Reset after a PHY select  */
    USB_CoreReset(USBx);

  }
#endif
  else /* FS interface (embedded Phy) */
  {
    /* Select FS Embedded PHY */
    USBx->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL;

    /* Reset after a PHY select and set Host mode */
    USB_CoreReset(USBx);

    /* Deactivate the power down*/
    USBx->GCCFG = USB_OTG_GCCFG_PWRDWN;
  }

	// xyz
	// see Internal DMA Mode—Internal Bus Master burst type: in QL-Hi-Speed-USB-2.0-OTG-Controller-Data-Sheet.pdf
	if (cfg->dma_enable == USB_ENABLE)
	{
		USBx->GAHBCFG = (USBx->GAHBCFG & ~ (USB_OTG_GAHBCFG_HBSTLEN | USB_OTG_GAHBCFG_DMAEN | USB_OTG_GAHBCFG_TXFELVL | USB_OTG_GAHBCFG_PTXFELVL)) |
			// See USBx_DEVICE->DTHRCTL
			//USB_OTG_GAHBCFG_TXFELVL |		// host and device
			//USB_OTG_GAHBCFG_PTXFELVL |	// host only
			//(0 << USB_OTG_GAHBCFG_HBSTLEN_Pos) |	// Single
			//(1 << USB_OTG_GAHBCFG_HBSTLEN_Pos) |	// INCR
			//(3 << USB_OTG_GAHBCFG_HBSTLEN_Pos) |	// INCR4
		#if CPUSTYLE_STM32H7XX
			(USB_OTG_GAHBCFG_HBSTLEN_1 | USB_OTG_GAHBCFG_HBSTLEN_2) | // (0x06 << USB_OTG_GAHBCFG_HBSTLEN_Pos)
		#else /* CPUSTYLE_STM32H7XX */
			(5 << USB_OTG_GAHBCFG_HBSTLEN_Pos) |	// INCR8
			//(7 << USB_OTG_GAHBCFG_HBSTLEN_Pos) |	// INCR16
		#endif /* CPUSTYLE_STM32H7XX */
			USB_OTG_GAHBCFG_DMAEN |
			0;
	}

	return HAL_OK;
}

/**
  * @brief  USB_EnableGlobalInt
  *         Enables the controller's Global Int in the AHB Config reg
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EnableGlobalInt(USB_OTG_GlobalTypeDef *USBx)
{
  USBx->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
  return HAL_OK;
}


/**
  * @brief  USB_DisableGlobalInt
  *         Disable the controller's Global Int in the AHB Config reg
  * @param  USBx : Selected device
  * @retval HAL status
*/
HAL_StatusTypeDef USB_DisableGlobalInt(USB_OTG_GlobalTypeDef *USBx)
{
  USBx->GAHBCFG &= ~ USB_OTG_GAHBCFG_GINT;
  return HAL_OK;
}

/**
  * @brief  USB_SetCurrentMode : Set functional mode
  * @param  USBx : Selected device
  * @param  mode :  current core mode
  *          This parameter can be one of these values:
  *            @arg USB_OTG_DEVICE_MODE: Peripheral mode
  *            @arg USB_OTG_HOST_MODE: Host mode
  *            @arg USB_OTG_DRD_MODE: Dual Role Device mode
  * @retval HAL status
  */
HAL_StatusTypeDef USB_SetCurrentMode(USB_OTG_GlobalTypeDef *USBx, USB_OTG_ModeTypeDef mode)
{
  USBx->GUSBCFG &= ~ (USB_OTG_GUSBCFG_FHMOD | USB_OTG_GUSBCFG_FDMOD);

  if (mode == USB_OTG_HOST_MODE)
  {
    USBx->GUSBCFG |= USB_OTG_GUSBCFG_FHMOD;
  }
  else if (mode == USB_OTG_DEVICE_MODE)
  {
    USBx->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
  }
  HARDWARE_DELAY_MS(50);

  return HAL_OK;
}

/**
  * @brief  USB_DevInit : Initializes the USB_OTG controller registers
  *         for device mode
  * @param  USBx : Selected device
  * @param  cfg  : pointer to a USB_OTG_CfgTypeDef structure that contains
  *         the configuration information for the specified USBx peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef USB_DevInit(USB_OTG_GlobalTypeDef *USBx, const USB_OTG_CfgTypeDef * cfg)
{
	uint32_t i = 0;

	// lib
	/*Activate VBUS Sensing B */
	#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx)


	if (cfg->vbus_sensing_enable == USB_DISABLE)
	{
		/* Deactivate VBUS Sensing B */
		USBx->GCCFG &= ~ USB_OTG_GCCFG_VBDEN;

		/* B-peripheral session valid override enable*/
		USBx->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
		USBx->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;
	}
	else
	{
		USBx->GCCFG |= USB_OTG_GCCFG_VBDEN;
	}

	#else


	if (cfg->vbus_sensing_enable == USB_DISABLE)
	{
		USBx->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
	}
	else
	{
		USBx->GCCFG |= USB_OTG_GCCFG_VBUSBSEN;
	}

	#endif /* STM32F446xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Rx || STM32F412Vx || STM32F412Cx */

	#if 0
	// old
	/*Activate VBUS Sensing B */
	USBx->GCCFG |= USB_OTG_GCCFG_VBDEN;

	if (cfg->vbus_sensing_enable == USB_DISABLE)
	{
		/* Deactivate VBUS Sensing B */
		USBx->GCCFG &= ~ USB_OTG_GCCFG_VBDEN;

		/* B-peripheral session valid override enable*/
		USBx->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
		USBx->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;
	}
	#endif
	/* Restart the Phy Clock */
	USBx_PCGCCTL = 0;

	/* Device mode configuration */
	USBx_DEVICE->DCFG |= DCFG_FRAME_INTERVAL_80;

	if(cfg->phy_itface  == USB_OTG_ULPI_PHY)
	{
		if(cfg->speed == USB_OTG_SPEED_HIGH)
		{
			/* Set High speed phy */
			USB_SetDevSpeed(USBx, USB_OTG_SPEED_HIGH);
		}
		else
		{
			/* set High speed phy in Full speed mode */
			USB_SetDevSpeed(USBx, USB_OTG_SPEED_HIGH_IN_FULL);
		}
	}

	else if(cfg->phy_itface  == USB_OTG_HS_EMBEDDED_PHY)
	{
		if(cfg->speed == USB_OTG_SPEED_HIGH)
		{
			/* Set High speed phy */
			USB_SetDevSpeed(USBx, USB_OTG_SPEED_HIGH);
		}
		else
		{
			/* set High speed phy in Full speed mode */
			USB_SetDevSpeed(USBx, USB_OTG_SPEED_HIGH_IN_FULL);
		}
	}
	else
	{
		/* Set Full speed phy */
		USB_SetDevSpeed(USBx, USB_OTG_SPEED_FULL);
	}


  /* Flush the FIFOs */
  USB_FlushTxFifoAll(USBx); /* all Tx FIFOs */
  USB_FlushRxFifo(USBx);

  /* Clear all pending Device Interrupts */
  USBx_DEVICE->DIEPMSK = 0;
  USBx_DEVICE->DOEPMSK = 0;
  USBx_DEVICE->DAINT = 0xFFFFFFFF;
  USBx_DEVICE->DAINTMSK = 0;

  for (i = 0; i < cfg->dev_endpoints; i++)
  {
    if ((USBx_INEP(i)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) == USB_OTG_DIEPCTL_EPENA)
    {
      USBx_INEP(i)->DIEPCTL = (USB_OTG_DIEPCTL_EPDIS | USB_OTG_DIEPCTL_SNAK);
    }
    else
    {
      USBx_INEP(i)->DIEPCTL = 0;
    }
    
    USBx_INEP(i)->DIEPTSIZ = 0;
    USBx_INEP(i)->DIEPINT  = 0xFF;
  }

  for (i = 0; i < cfg->dev_endpoints; i++)
  {
    if ((USBx_OUTEP(i)->DOEPCTL & USB_OTG_DOEPCTL_EPENA) == USB_OTG_DOEPCTL_EPENA)
    {
      USBx_OUTEP(i)->DOEPCTL = (USB_OTG_DOEPCTL_EPDIS | USB_OTG_DOEPCTL_SNAK);
    }
    else
    {
      USBx_OUTEP(i)->DOEPCTL = 0;
    }
    
    USBx_OUTEP(i)->DOEPTSIZ = 0;
    USBx_OUTEP(i)->DOEPINT  = 0xFF;
  }

  USBx_DEVICE->DIEPMSK &= ~(USB_OTG_DIEPMSK_TXFURM);

  // xyz
  if (cfg->dma_enable == USB_ENABLE)
  {
	  // See also USBx->GAHBCFG

#if 0//CPUSTYLE_STM32H7XX
    /*Set threshold parameters */
    USBx_DEVICE->DTHRCTL = (USB_OTG_DTHRCTL_TXTHRLEN_8 | USB_OTG_DTHRCTL_RXTHRLEN_8);
    USBx_DEVICE->DTHRCTL |= (USB_OTG_DTHRCTL_RXTHREN | USB_OTG_DTHRCTL_ISOTHREN |
		USB_OTG_DTHRCTL_NONISOTHREN | 0x08000000);

#else /* CPUSTYLE_STM32H7XX */

	#if CPUSTYLE_STM32H7XX
		  const int TXTHRLEN = 256;		// in DWORDS: The threshold length has to be at least eight DWORDS.
		  const int RXTHRLEN = 8;	// in DWORDS: 128 - енумерация проходтит, 256 - нет.
	#else /* CPUSTYLE_STM32H7XX */
		  const int TXTHRLEN = 64;		// in DWORDS: The threshold length has to be at least eight DWORDS.
		  const int RXTHRLEN = 2;	// in DWORDS:
	#endif /* CPUSTYLE_STM32H7XX */
	  // Configuration register applies only to USB OTG HS
    /*Set threshold parameters */
    USBx_DEVICE->DTHRCTL = (USBx_DEVICE->DTHRCTL & ~ (USB_OTG_DTHRCTL_TXTHRLEN | USB_OTG_DTHRCTL_RXTHRLEN |
									USB_OTG_DTHRCTL_RXTHREN | USB_OTG_DTHRCTL_ISOTHREN | USB_OTG_DTHRCTL_NONISOTHREN | USB_OTG_DTHRCTL_ARPEN)) |
		TXTHRLEN * USB_OTG_DTHRCTL_TXTHRLEN_0 |		// Transmit (IN) threshold length
		RXTHRLEN * USB_OTG_DTHRCTL_RXTHRLEN_0 | // see HBSTLEN bit in OTG_GAHBCFG).
		USB_OTG_DTHRCTL_RXTHREN |		//  Receive (OUT) threshold enable
		USB_OTG_DTHRCTL_ISOTHREN |		// ISO IN endpoint threshold enable
		USB_OTG_DTHRCTL_NONISOTHREN |	// Nonisochronous IN endpoints threshold enable
		USB_OTG_DTHRCTL_ARPEN |			// Arbiter parking enable
		0;
 #endif /* CPUSTYLE_STM32H7XX */

    (void) USBx_DEVICE->DTHRCTL;
 }

	/* Disable all interrupts. */
	USBx->GINTMSK = 0;

	/* Clear any pending interrupts */
	USBx->GINTSTS = 0xBFFFFFFF;

	/* Enable the common interrupts */
	if (cfg->dma_enable == USB_DISABLE)
	{
		USBx->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM;
	}

	////USBx->GINTMSK |= USB_OTG_GINTMSK_GINAKEFFM;	// debug

	/* Enable interrupts matching to the Device mode ONLY */
	USBx->GINTMSK |=
		USB_OTG_GINTMSK_USBSUSPM |
		USB_OTG_GINTMSK_USBRST |
		USB_OTG_GINTMSK_ENUMDNEM |
		USB_OTG_GINTMSK_IEPINT |
		USB_OTG_GINTMSK_OEPINT   |
		USB_OTG_GINTMSK_IISOIXFRM|
		USB_OTG_GINTMSK_PXFRM_IISOOXFRM |
		USB_OTG_GINTMSK_WUIM |
		USB_OTG_GINTMSK_MMISM |
		0;

	if (cfg->Sof_enable)
	{
		USBx->GINTMSK |= USB_OTG_GINTMSK_SOFM;
	}

	if (cfg->vbus_sensing_enable == USB_ENABLE)
	{
		USBx->GINTMSK |= (USB_OTG_GINTMSK_SRQIM | USB_OTG_GINTMSK_OTGINT);
	}

	return HAL_OK;
}


/**
  * @brief  USB_OTG_FlushTxFifo : Flush a Tx FIFO
  * @param  USBx : Selected device
  * @param  num : FIFO number
  *         This parameter can be a value from 1 to 15
            16 means Flush all Tx FIFOs
  * @retval HAL status
  */
HAL_StatusTypeDef USB_FlushTxFifo(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t num)
{
	uint32_t count = 0;

	USBx->GRSTCTL = (USBx->GRSTCTL & ~ (USB_OTG_GRSTCTL_TXFNUM)) |
		USB_OTG_GRSTCTL_TXFFLSH |
		num * USB_OTG_GRSTCTL_TXFNUM_0 |
		0;

	do
	{
		if (++count > 200000)
		{
			TP();
			return HAL_TIMEOUT;
		}
	}
	while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) != 0);

	return HAL_OK;
}

HAL_StatusTypeDef USB_FlushTxFifoAll(USB_OTG_GlobalTypeDef *USBx)
{
	uint_fast32_t count = 0;

	USBx->GRSTCTL = (USBx->GRSTCTL & ~ (USB_OTG_GRSTCTL_TXFNUM)) |
		USB_OTG_GRSTCTL_TXFFLSH |
		0x10 * USB_OTG_GRSTCTL_TXFNUM_0 |
		0;

	do
	{
		if (++ count > 200000)
		{
			TP();
			return HAL_TIMEOUT;
		}
	}
	while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) != 0);

	return HAL_OK;
}


/**
  * @brief  USB_FlushRxFifo : Flush Rx FIFO
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef USB_FlushRxFifo(USB_OTG_GlobalTypeDef *USBx)
{
	uint_fast32_t count = 0;

	USBx->GRSTCTL |= USB_OTG_GRSTCTL_RXFFLSH;

	do
	{
		if (++count > 200000)
		{
			TP();
			return HAL_TIMEOUT;
		}
	}
	while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH) == USB_OTG_GRSTCTL_RXFFLSH);

	return HAL_OK;
}

/**
  * @brief  USB_SetDevSpeed :Initializes the DevSpd field of DCFG register
  *         depending the PHY type and the enumeration speed of the device.
  * @param  USBx : Selected device
  * @param  speed : device speed
  *          This parameter can be one of these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_HIGH_IN_FULL: High speed core in Full Speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  *            @arg USB_OTG_SPEED_LOW: Low speed mode
  * @retval  Hal status
  */
HAL_StatusTypeDef USB_SetDevSpeed(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t speed)
{
	USBx_DEVICE->DCFG = (USBx_DEVICE->DCFG & ~ (USB_OTG_DCFG_DSPD)) |
		(USB_OTG_DCFG_DSPD & speed * USB_OTG_DCFG_DSPD_0) |
		0;
	return HAL_OK;
}

/**
  * @brief  USB_GetDevSpeed :Return the  Dev Speed
  * @param  USBx : Selected device
  * @retval speed : device speed
  *          This parameter can be one of these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  *            @arg USB_OTG_SPEED_LOW: Low speed mode
  */
uint_fast8_t USB_GetDevSpeed(USB_OTG_GlobalTypeDef *USBx)
{
	uint_fast8_t speed = 0;

	if ((USBx_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD) == DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ)
	{
		speed = USB_OTG_SPEED_HIGH;
	}
	else if (((USBx_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD) == DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ)||
		((USBx_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD) == DSTS_ENUMSPD_FS_PHY_48MHZ))
	{
		speed = USB_OTG_SPEED_FULL;
	}
	else if ((USBx_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD) == DSTS_ENUMSPD_LS_PHY_6MHZ)
	{
		speed = USB_OTG_SPEED_LOW;
	}

	return speed;
}

/**
  * @brief  Activate and configure an endpoint
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_ActivateEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
{
	if (ep->is_in == 1)
	{
		USB_OTG_INEndpointTypeDef * const inep = USBx_INEP(ep->num);
		USBx_DEVICE->DAINTMSK |= USB_OTG_DAINTMSK_IEPM & ((1 << ep->num) << USB_OTG_DAINTMSK_IEPM_Pos);

		if ((inep->DIEPCTL & USB_OTG_DIEPCTL_USBAEP) == 0)
		{
			inep->DIEPCTL = (inep->DIEPCTL & ~ (USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP | USB_OTG_DIEPCTL_TXFNUM)) |
				((ep->maxpacket << USB_OTG_DIEPCTL_MPSIZ_Pos) & USB_OTG_DIEPCTL_MPSIZ ) |
				(ep->type << USB_OTG_DIEPCTL_EPTYP_Pos ) |
				(ep->tx_fifo_num << USB_OTG_DIEPCTL_TXFNUM_Pos ) |	// TX FIFO index
				USB_OTG_DIEPCTL_SD0PID_SEVNFRM |
				USB_OTG_DIEPCTL_USBAEP |
				0;
		}

	}
	else
	{
		USB_OTG_OUTEndpointTypeDef * const outep = USBx_OUTEP(ep->num);

		USBx_DEVICE->DAINTMSK |= USB_OTG_DAINTMSK_OEPM & ((1 << ep->num) << USB_OTG_DAINTMSK_OEPM_Pos);

		if ((outep->DOEPCTL & USB_OTG_DOEPCTL_USBAEP) == 0)
		{
			outep->DOEPCTL = (outep->DOEPCTL & ~ (USB_OTG_DOEPCTL_MPSIZ | USB_OTG_DOEPCTL_EPTYP)) |
				((ep->maxpacket << USB_OTG_DOEPCTL_MPSIZ_Pos) & USB_OTG_DOEPCTL_MPSIZ ) |
				(ep->type << USB_OTG_DOEPCTL_EPTYP_Pos ) |
				USB_OTG_DOEPCTL_SD0PID_SEVNFRM |
				USB_OTG_DOEPCTL_USBAEP |
				0;
		}
	}
	return HAL_OK;
}
/**
  * @brief  Activate and configure a dedicated endpoint
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_ActivateDedicatedEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
{
	/* Read DEPCTLn register */
	if (ep->is_in == 1)
	{
		USB_OTG_INEndpointTypeDef * const inep = USBx_INEP(ep->num);
		if ((inep->DIEPCTL & USB_OTG_DIEPCTL_USBAEP) == 0)
		{
			inep->DIEPCTL = (inep->DIEPCTL & ~ (USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP | USB_OTG_DIEPCTL_TXFNUM)) |
				((ep->maxpacket << USB_OTG_DIEPCTL_MPSIZ_Pos) & USB_OTG_DIEPCTL_MPSIZ ) |
				(ep->type << USB_OTG_DIEPCTL_EPTYP_Pos ) |
				(ep->tx_fifo_num << USB_OTG_DIEPCTL_TXFNUM_Pos ) |	// TX FIFO index
				(USB_OTG_DIEPCTL_SD0PID_SEVNFRM) |
				(USB_OTG_DIEPCTL_USBAEP) |
				0;
		}

		USBx_DEVICE->DEACHMSK |= USB_OTG_DAINTMSK_IEPM & ((1 << (ep->num)) << USB_OTG_DAINTMSK_IEPM_Pos);
	}
	else
	{
		USB_OTG_OUTEndpointTypeDef * const outep = USBx_OUTEP(ep->num);
		if ((outep->DOEPCTL & USB_OTG_DOEPCTL_USBAEP) == 0)
		{
			outep->DOEPCTL = (outep->DOEPCTL & ~ (USB_OTG_DOEPCTL_MPSIZ | USB_OTG_DOEPCTL_EPTYP)) |
				((ep->maxpacket << USB_OTG_DOEPCTL_MPSIZ_Pos) & USB_OTG_DOEPCTL_MPSIZ ) |
				(ep->type << USB_OTG_DOEPCTL_EPTYP_Pos ) |
				//(ep->num << USB_OTG_DIEPCTL_TXFNUM_Pos ) |	// TX FIFO index - зачем это здесь?
				(USB_OTG_DOEPCTL_USBAEP) |
				0;
		}

		USBx_DEVICE->DEACHMSK |= USB_OTG_DAINTMSK_OEPM & ((1 << (ep->num)) << USB_OTG_DAINTMSK_OEPM_Pos);
	}

	return HAL_OK;
}
/**
  * @brief  De-activate and de-initialize an endpoint
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_DeactivateEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
{
	/* Read DEPCTLn register */
	if (ep->is_in == 1)
	{
		USB_OTG_INEndpointTypeDef * const inep = USBx_INEP(ep->num);
		USBx_DEVICE->DEACHMSK &= ~(USB_OTG_DAINTMSK_IEPM & ((1 << (ep->num)) << USB_OTG_DAINTMSK_IEPM_Pos));
		USBx_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_IEPM & ((1 << (ep->num)) << USB_OTG_DAINTMSK_IEPM_Pos));
		inep->DIEPCTL &= ~ USB_OTG_DIEPCTL_USBAEP;
	}
	else
	{
		USB_OTG_OUTEndpointTypeDef * const outep = USBx_OUTEP(ep->num);
		USBx_DEVICE->DEACHMSK &= ~(USB_OTG_DAINTMSK_OEPM & ((1 << (ep->num)) << USB_OTG_DAINTMSK_OEPM_Pos));
		USBx_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_OEPM & ((1 << (ep->num)) << USB_OTG_DAINTMSK_OEPM_Pos));
		outep->DOEPCTL &= ~USB_OTG_DOEPCTL_USBAEP;
	}
	return HAL_OK;
}

/**
  * @brief  De-activate and de-initialize a dedicated endpoint
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_DeactivateDedicatedEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
{
	/* Read DEPCTLn register */
	if (ep->is_in == 1)
	{
		USB_OTG_INEndpointTypeDef * const inep = USBx_INEP(ep->num);
		inep->DIEPCTL &= ~ USB_OTG_DIEPCTL_USBAEP;
		USBx_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_IEPM & ((1 << ep->num)) << USB_OTG_DAINTMSK_IEPM_Pos);
	}
	else
	{
		USB_OTG_OUTEndpointTypeDef * const outep = USBx_OUTEP(ep->num);
		outep->DOEPCTL &= ~ USB_OTG_DOEPCTL_USBAEP;
		USBx_DEVICE->DAINTMSK &= ~ (USB_OTG_DAINTMSK_OEPM & ((1 << ep->num) << USB_OTG_DAINTMSK_OEPM_Pos));
	}
	return HAL_OK;
}

/**
  * @brief  USB_EPStartXfer : setup and starts a transfer over an EP
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @param  dma: USB dma enabled or disabled
  *          This parameter can be one of these values:
  *           0 : DMA feature not used
  *           1 : DMA feature used
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EPStartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep, uint_fast8_t dma)
{
	if (ep->is_in == 1)
	{
		/* IN endpoint */
		USB_OTG_INEndpointTypeDef * const inep = USBx_INEP(ep->num);

	#if 0
		while (! (((inep->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >> USB_OTG_DTXFSTS_INEPTFSAV_Pos) >= ((ep->xfer_len + 3) / 4)))
		{
			PRINTF(PSTR("ep%d: avl=%04lX, need=%04lX, sts=%08lX\n"), ep->num, ((inep->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >> USB_OTG_DTXFSTS_INEPTFSAV_Pos), ((ep->xfer_len + 3) / 4), USBx->GRSTCTL);
			USB_FlushTxFifo(USBx, ep->num);
			//ASSERT(((inep->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >> USB_OTG_DTXFSTS_INEPTFSAV_Pos) >= ((ep->xfer_len + 3) / 4));
		}
		switch (ep->num)
		{
		case 0:
			ASSERT(((inep->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >> USB_OTG_DTXFSTS_INEPTFSAV_Pos) >= ((ep->xfer_len + 3) / 4));
			break;
		case 1:
			ASSERT(((inep->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >> USB_OTG_DTXFSTS_INEPTFSAV_Pos) >= ((ep->xfer_len + 3) / 4));
			break;
		case 2:
			ASSERT(((inep->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >> USB_OTG_DTXFSTS_INEPTFSAV_Pos) >= ((ep->xfer_len + 3) / 4));
			break;
		case 3:
			ASSERT(((inep->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >> USB_OTG_DTXFSTS_INEPTFSAV_Pos) >= ((ep->xfer_len + 3) / 4));
			break;
		case 4:
			ASSERT(((inep->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >> USB_OTG_DTXFSTS_INEPTFSAV_Pos) >= ((ep->xfer_len + 3) / 4));
			break;
		case 5:
			ASSERT(((inep->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >> USB_OTG_DTXFSTS_INEPTFSAV_Pos) >= ((ep->xfer_len + 3) / 4));
			break;
		case 6:
			ASSERT(((inep->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >> USB_OTG_DTXFSTS_INEPTFSAV_Pos) >= ((ep->xfer_len + 3) / 4));
			break;
		case 7:
			ASSERT(((inep->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >> USB_OTG_DTXFSTS_INEPTFSAV_Pos) >= ((ep->xfer_len + 3) / 4));
			break;
		case 8:
			ASSERT(((inep->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >> USB_OTG_DTXFSTS_INEPTFSAV_Pos) >= ((ep->xfer_len + 3) / 4));
			break;
		case 9:
			ASSERT(((inep->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >> USB_OTG_DTXFSTS_INEPTFSAV_Pos) >= ((ep->xfer_len + 3) / 4));
			break;
		}
	#endif

		/* Zero Length Packet? */
		if (ep->xfer_len == 0)
		{
			inep->DIEPTSIZ = (inep->DIEPTSIZ & ~ (USB_OTG_DIEPTSIZ_XFRSIZ | USB_OTG_DIEPTSIZ_PKTCNT | USB_OTG_DIEPTSIZ_MULCNT)) |
				(USB_OTG_DIEPTSIZ_PKTCNT & (1 << USB_OTG_DIEPTSIZ_PKTCNT_Pos)) |
				0;
		}
		else
		{
			const uint32_t pktcnt = (ep->xfer_len + ep->maxpacket - 1) / ep->maxpacket;
			/* Program the transfer size and packet count
			* as follows: xfersize = N * maxpacket +
			* short_packet pktcnt = N + (short_packet
			* exist ? 1 : 0)
			*/
			//ASSERT((ep->xfer_len % 4) == 0);
			inep->DIEPTSIZ = (inep->DIEPTSIZ & ~ (USB_OTG_DIEPTSIZ_XFRSIZ | USB_OTG_DIEPTSIZ_PKTCNT | USB_OTG_DIEPTSIZ_MULCNT)) |
				(USB_OTG_DIEPTSIZ_PKTCNT & (pktcnt << USB_OTG_DIEPTSIZ_PKTCNT_Pos)) |
				(USB_OTG_DIEPTSIZ_XFRSIZ & (ep->xfer_len << USB_OTG_DIEPTSIZ_XFRSIZ_Pos)) |
				(USB_OTG_DIEPTSIZ_MULCNT & ((ep->type == USBD_EP_TYPE_ISOC) << USB_OTG_DIEPTSIZ_MULCNT_Pos)) |
				0;
		}
		if (dma == USB_ENABLE)
		{
			inep->DIEPDMA = ep->dma_addr;
		}
		else
		{
			if (ep->type != USBD_EP_TYPE_ISOC)
			{
				/* Enable the Tx FIFO Empty Interrupt for this EP */
				if (ep->xfer_len > 0)
				{
					USBx_DEVICE->DIEPEMPMSK |= (1uL << ep->num);
				}
			}
		}

		if (ep->type == USBD_EP_TYPE_ISOC)
		{
			if (USB_GetDevSpeed(USBx) == USB_OTG_SPEED_HIGH)	// romanetz
			{
				if (! notseq)
					inep->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
				else
					inep->DIEPCTL |= USB_OTG_DIEPCTL_SODDFRM;
			}
			else
			{
				if ((USBx_DEVICE->DSTS & (1uL << USB_OTG_DSTS_FNSOF_Pos)) == 0)
				{
					inep->DIEPCTL |= USB_OTG_DIEPCTL_SODDFRM;
				}
				else
				{
					inep->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
				}
			}
		}

		/* EP enable, IN data in FIFO */
		inep->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);

		if (ep->type == USBD_EP_TYPE_ISOC)
		{
			USB_WritePacket(USBx, ep->xfer_buff, ep->tx_fifo_num, ep->xfer_len, dma);
		}    
	}
	else
	{
		/* OUT endpoint */
		USB_OTG_OUTEndpointTypeDef * const outep = USBx_OUTEP(ep->num);
		/* Program the transfer size and packet count as follows:
		* pktcnt = N
		* xfersize = N * maxpacket
		*/

		// почему-то используется ep->maxpacket а не ep->xfer_len

		if (ep->xfer_len == 0)
		{
			outep->DOEPTSIZ = (outep->DOEPTSIZ & ~ (USB_OTG_DOEPTSIZ_XFRSIZ | USB_OTG_DOEPTSIZ_PKTCNT)) |
				(USB_OTG_DOEPTSIZ_PKTCNT & (1 << USB_OTG_DOEPTSIZ_PKTCNT_Pos)) |
				(USB_OTG_DOEPTSIZ_XFRSIZ & (ep->maxpacket << USB_OTG_DOEPTSIZ_XFRSIZ_Pos)) |
				0;
		}
		else
		{
			const uint32_t pktcnt = (ep->xfer_len + ep->maxpacket - 1) / ep->maxpacket;
			outep->DOEPTSIZ = (outep->DOEPTSIZ & ~ (USB_OTG_DOEPTSIZ_XFRSIZ | USB_OTG_DOEPTSIZ_PKTCNT)) |
				(USB_OTG_DOEPTSIZ_PKTCNT & (pktcnt << USB_OTG_DOEPTSIZ_PKTCNT_Pos)) |
				(USB_OTG_DOEPTSIZ_XFRSIZ & ((ep->maxpacket * pktcnt) << USB_OTG_DOEPTSIZ_XFRSIZ_Pos)) |
				0;
		}

		if (dma == USB_ENABLE)
		{
			outep->DOEPDMA = (uint32_t) ep->xfer_buff;
		}

		if (ep->type == USBD_EP_TYPE_ISOC)
		{
			if (USB_GetDevSpeed(USBx) == USB_OTG_SPEED_HIGH)	// romanetz
			{
				if (notseq)
					outep->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM;
				else
					outep->DOEPCTL |= USB_OTG_DOEPCTL_SODDFRM;
			}
			else
			{
				if ((USBx_DEVICE->DSTS & (1uL << USB_OTG_DSTS_FNSOF_Pos)) == 0)
				{
					outep->DOEPCTL |= USB_OTG_DOEPCTL_SODDFRM;
				}
				else
				{
					outep->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM;
				}
			}
		}
		/* EP enable */
		outep->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
	}
	return HAL_OK;
}

/**
  * @brief  USB_EP0StartXfer : setup and starts a transfer over the EP  0
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @param  dma: USB dma enabled or disabled
  *          This parameter can be one of these values:
  *           0 : DMA feature not used
  *           1 : DMA feature used
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EP0StartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep, uint_fast8_t dma)
{
  /* IN endpoint */
  if (ep->is_in == 1)
  {
 	  USB_OTG_INEndpointTypeDef * const inep = USBx_INEP(ep->num);
    if (ep->xfer_len == 0)
    {
  	   /* Zero Length Packet? */
    //inep->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT);
      //inep->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (1 << USB_OTG_DIEPTSIZ_PKTCNT_Pos)) ;
      //inep->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ);

	inep->DIEPTSIZ = (inep->DIEPTSIZ & ~ (USB_OTG_DIEPTSIZ_XFRSIZ | USB_OTG_DIEPTSIZ_PKTCNT)) |
		(USB_OTG_DIEPTSIZ_PKTCNT & (1 << USB_OTG_DIEPTSIZ_PKTCNT_Pos)) |
		(USB_OTG_DIEPTSIZ_XFRSIZ & (0 << USB_OTG_DIEPTSIZ_XFRSIZ_Pos)) |
		0;
    }
    else
    {
      /* Program the transfer size and packet count
      * as follows: xfersize = N * maxpacket +
      * short_packet pktcnt = N + (short_packet
      * exist ? 1 : 0)
      */
      //inep->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ);
      //inep->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT);

      if(ep->xfer_len > ep->maxpacket)
      {
        ep->xfer_len = ep->maxpacket;
      }
      //inep->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (1 << USB_OTG_DIEPTSIZ_PKTCNT_Pos)) ;
      //inep->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_XFRSIZ & ep->xfer_len);

		inep->DIEPTSIZ = (inep->DIEPTSIZ & ~ (USB_OTG_DIEPTSIZ_XFRSIZ | USB_OTG_DIEPTSIZ_PKTCNT)) |
			(USB_OTG_DIEPTSIZ_PKTCNT & (1 << USB_OTG_DIEPTSIZ_PKTCNT_Pos)) |
			(USB_OTG_DIEPTSIZ_XFRSIZ & (ep->xfer_len << USB_OTG_DIEPTSIZ_XFRSIZ_Pos)) |
			0;

    }

    if (dma == USB_ENABLE)
    {
      inep->DIEPDMA = ep->dma_addr;
    }
    else
    {
      /* Enable the Tx FIFO Empty Interrupt for this EP */
      if (ep->xfer_len > 0)
      {
        USBx_DEVICE->DIEPEMPMSK |= (0x1uL << ep->num);
      }
    }

    /* EP enable, IN data in FIFO */
    inep->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
  }
  else /* OUT endpoint */
  {
		USB_OTG_OUTEndpointTypeDef * const outep = USBx_OUTEP(ep->num);
		/* Program the transfer size and packet count as follows:
		* pktcnt = N
		* xfersize = N * maxpacket
		*/
		//outep->DOEPTSIZ &= ~(USB_OTG_DOEPTSIZ_XFRSIZ);
		//outep->DOEPTSIZ &= ~(USB_OTG_DOEPTSIZ_PKTCNT);

		if (ep->xfer_len > 0)
		{
			ep->xfer_len = ep->maxpacket;
		}

		//outep->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_PKTCNT & (1 << USB_OTG_DIEPTSIZ_PKTCNT_Pos));
		//outep->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_XFRSIZ & (ep->maxpacket));

		outep->DOEPTSIZ = (outep->DOEPTSIZ & ~ (USB_OTG_DOEPTSIZ_XFRSIZ | USB_OTG_DOEPTSIZ_PKTCNT)) |
			(USB_OTG_DOEPTSIZ_PKTCNT & (1 << USB_OTG_DOEPTSIZ_PKTCNT_Pos)) |
			(USB_OTG_DOEPTSIZ_XFRSIZ & (ep->maxpacket << USB_OTG_DOEPTSIZ_XFRSIZ_Pos)) |
			0;


		if (dma == USB_ENABLE)
		{
			outep->DOEPDMA = (uint32_t) ep->xfer_buff;
		}

		/* EP enable */
		outep->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
	}
	return HAL_OK;
}

#if 0
/**
  * @brief  USB_WritePacket : Writes a packet into the Tx FIFO associated
  *         with the EP/channel
  * @param  USBx : Selected device
  * @param  src :  pointer to source buffer
  * @param  ch_ep_num : endpoint or host channel number
  * @param  len : Number of bytes to write
  * @param  dma: USB dma enabled or disabled
  *          This parameter can be one of these values:
  *           0 : DMA feature not used
  *           1 : DMA feature used
  * @retval HAL status
  */
void USB_WritePacket(USB_OTG_GlobalTypeDef *USBx, const uint8_t *src, uint_fast8_t tx_fifo_num, uint_fast16_t len, uint_fast8_t dma)
{

	if (dma == USB_DISABLE)
	{
		uint_fast32_t i = 0;
		uint_fast32_t count32b = (len + 3) / 4;

		for (i = 0; i < count32b; i++, src += 4)
		{
			USBx_DFIFO(tx_fifo_num) = * ((KEYWORDPACKED uint32_t *) src);
		}
	}
}

/**
  * @brief  USB_ReadPacket : read a packet from the Tx FIFO associated
  *         with the EP/channel
  * @param  USBx : Selected device
  * @param  src : source pointer
  * @param  ch_ep_num : endpoint or host channel number
  * @param  len : Number of bytes to read
  * @param  dma: USB dma enabled or disabled
  *          This parameter can be one of these values:
  *           0 : DMA feature not used
  *           1 : DMA feature used
  * @retval pointer to destination buffer
  */
void USB_ReadPacket(USB_OTG_GlobalTypeDef *USBx, uint8_t *dest, uint_fast16_t len)
{
	uint_fast32_t i = 0;
	uint_fast32_t count32b = (len + 3) / 4;

	for ( i = 0; i < count32b; i++, dest += 4 )
	{
		* (KEYWORDPACKED uint32_t *) dest = USBx_DFIFO(0);
	}
}

#else

/**
  * @brief  USB_WritePacket : Writes a packet into the Tx FIFO associated
  *         with the EP/channel
  * @param  USBx : Selected device
  * @param  src :  pointer to source buffer
  * @param  ch_ep_num : endpoint or host channel number
  * @param  len : Number of bytes to write
  * @param  dma: USB dma enabled or disabled
  *          This parameter can be one of these values:
  *           0 : DMA feature not used
  *           1 : DMA feature used
  * @retval HAL status
  */
void USB_WritePacket(USB_OTG_GlobalTypeDef *USBx, const uint8_t * data, uint_fast8_t tx_fifo_num, uint_fast16_t size, uint_fast8_t dma)
{
	//if (data != NULL && size != 0)
	//	PRINTF(PSTR("USB_WritePacket, pipe=%d, size=%d, data[]={%02x,%02x,%02x,%02x,%02x,..}\n"), ch_ep_num, size, data [0], data [1], data [2], data [3], data [4]);
	//else
	//	PRINTF(PSTR("USB_WritePacket, pipe=%d, size=%d, data[]={}\n"), ch_ep_num, size);


	if (dma == USB_DISABLE)
	{
		uint_fast16_t count32b = (size + 3) / 4;
		const uint32_t * data32 = (const uint32_t *) data;
		volatile uint32_t * const txfifo32 = & USBx_DFIFO(tx_fifo_num);

		for (; count32b >= 16; count32b -= 16)
		{
			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;

			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;

			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;

			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;
		}
		for (; count32b >= 4; count32b -= 4)
		{
			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;
			* txfifo32 = * data32 ++;
		}
		for (; count32b != 0; count32b -= 1)
		{
			* txfifo32 = * data32 ++;
		}
	}
}

/**
  * @brief  USB_ReadPacket : read a packet from the Tx FIFO associated
  *         with the EP/channel
  * @param  USBx : Selected device
  * @param  src : source pointer
  * @param  ch_ep_num : endpoint or host channel number
  * @param  len : Number of bytes to read
  * @param  dma: USB dma enabled or disabled
  *          This parameter can be one of these values:
  *           0 : DMA feature not used
  *           1 : DMA feature used
  * @retval pointer to destination buffer
  */
void USB_ReadPacket(USB_OTG_GlobalTypeDef *USBx, uint8_t * dest, uint_fast16_t len)
{
	uint_fast16_t count32b = (len + 3) / 4;
	uint32_t * dest32 = (uint32_t *) dest;
	const volatile uint32_t * const rxfifo32 = & USBx_DFIFO(0);

	for (; count32b >= 16; count32b -= 16)
	{
		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;

		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;

		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;

		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;
	}
	for (; count32b >= 4; count32b -= 4)
	{
		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;
		* dest32 ++ = * rxfifo32;
	}
	for (; count32b != 0; count32b -= 1)
	{
		* dest32 ++ = * rxfifo32;
	}
}
#endif



/**
  * @brief  Start a transfer over a host channel
  * @param  USBx : Selected device
  * @param  hc : pointer to host channel structure
  * @param  dma: USB dma enabled or disabled
  *          This parameter can be one of the these values:
  *           0 : DMA feature not used
  *           1 : DMA feature used
  * @retval HAL state
  */

HAL_StatusTypeDef USB_HC_StartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_HCTypeDef *hc, uint_fast8_t dma)
{
  uint8_t  is_oddframe = 0;
  uint16_t len_words = 0;
  uint16_t num_packets = 0;
  uint16_t max_hc_pkt_count = 256;
  uint32_t tmpreg = 0;

  if ((USBx != USB_OTG_FS) && (hc->speed == USB_OTG_SPEED_HIGH))
  {
	  // HS
    if ((dma == 0) && (hc->do_ping == 1))
    {
      USB_DoPing(USBx, hc->ch_num);
      return HAL_OK;
    }
    else if(dma == 1)
    {
      USBx_HC(hc->ch_num)->HCINTMSK &= ~(USB_OTG_HCINTMSK_NYET | USB_OTG_HCINTMSK_ACKM);
      hc->do_ping = 0;
    }
  }

  /* Compute the expected number of packets associated to the transfer */
  if (hc->xfer_len > 0)
  {
    num_packets = (hc->xfer_len + hc->max_packet - 1) / hc->max_packet;

    if (num_packets > max_hc_pkt_count)
    {
      num_packets = max_hc_pkt_count;
      hc->xfer_len = num_packets * hc->max_packet;
    }
  }
  else
  {
    num_packets = 1;
  }
  if (hc->ep_is_in)
  {
    hc->xfer_len = num_packets * hc->max_packet;
  }



  /* Initialize the HCTSIZn register */
  USBx_HC(hc->ch_num)->HCTSIZ = (((hc->xfer_len) & USB_OTG_HCTSIZ_XFRSIZ)) |
    ((num_packets << USB_OTG_HCTSIZ_PKTCNT_Pos) & USB_OTG_HCTSIZ_PKTCNT) |
      (((hc->data_pid) << USB_OTG_HCTSIZ_DPID_Pos) & USB_OTG_HCTSIZ_DPID);

  if (dma)
  {
    /* xfer_buff MUST be 32-bits aligned */
    USBx_HC(hc->ch_num)->HCDMA = (uint32_t)hc->xfer_buff;
  }

  is_oddframe = (USBx_HOST->HFNUM & 0x01) ? 0 : 1;
  USBx_HC(hc->ch_num)->HCCHAR &= ~ USB_OTG_HCCHAR_ODDFRM;
  USBx_HC(hc->ch_num)->HCCHAR |= (is_oddframe << 29);

  /* Set host channel enable */
  tmpreg = USBx_HC(hc->ch_num)->HCCHAR;
  tmpreg &= ~USB_OTG_HCCHAR_CHDIS;

  /* make sure to set the correct ep direction */
  if (hc->ep_is_in)
  {
    tmpreg |= USB_OTG_HCCHAR_EPDIR;
  }
  else
  {
     tmpreg &= ~USB_OTG_HCCHAR_EPDIR;
  }

  tmpreg |= USB_OTG_HCCHAR_CHENA;
  USBx_HC(hc->ch_num)->HCCHAR = tmpreg;

  if (dma == 0) /* Slave mode */
  {
    if ((hc->ep_is_in == 0) && (hc->xfer_len > 0))
    {
      switch(hc->ep_type)
      {
        /* Non periodic transfer */
      case USBD_EP_TYPE_CTRL:
      case USBD_EP_TYPE_BULK:

        len_words = (hc->xfer_len + 3) / 4;

        /* check if there is enough space in FIFO space */
        if(len_words > (USBx->HNPTXSTS & 0xFFFF))
        {
          /* need to process data in nptxfempty interrupt */
          USBx->GINTMSK |= USB_OTG_GINTMSK_NPTXFEM;
        }
        break;
        /* Periodic transfer */
      case USBD_EP_TYPE_INTR:
      case USBD_EP_TYPE_ISOC:
        len_words = (hc->xfer_len + 3) / 4;
        /* check if there is enough space in FIFO space */
        if(len_words > (USBx_HOST->HPTXSTS & 0xFFFF)) /* split the transfer */
        {
          /* need to process data in ptxfempty interrupt */
          USBx->GINTMSK |= USB_OTG_GINTMSK_PTXFEM;
        }
        break;

      default:
        break;
      }

      /* Write packet into the Tx FIFO. */
      USB_WritePacket(USBx, hc->xfer_buff, hc->ch_num, hc->xfer_len, 0);
    }
  }

  return HAL_OK;
}

/**
  * @brief  USB_EPSetStall : set a stall condition over an EP
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EPSetStall(USB_OTG_GlobalTypeDef *USBx, const USB_OTG_EPTypeDef *ep)
{
	//PRINTF(PSTR("USB_EPSetStall, ep->num=%02X\n"), ep->num);
	if (ep->is_in == 1)
	{
		USB_OTG_INEndpointTypeDef * const inep = USBx_INEP(ep->num);
		if (((inep->DIEPCTL) & USB_OTG_DIEPCTL_EPENA) == 0)
		{
			inep->DIEPCTL &= ~ USB_OTG_DIEPCTL_EPDIS;
		}
		inep->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
	}
	else
	{
		USB_OTG_OUTEndpointTypeDef * const outep = USBx_OUTEP(ep->num);
		if (((outep->DOEPCTL) & USB_OTG_DOEPCTL_EPENA) == 0)
		{
			outep->DOEPCTL &= ~ USB_OTG_DOEPCTL_EPDIS;
		}
		outep->DOEPCTL |= USB_OTG_DOEPCTL_STALL;
	}
	return HAL_OK;
}


/**
  * @brief  USB_EPClearStall : Clear a stall condition over an EP
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EPClearStall(USB_OTG_GlobalTypeDef *USBx, const USB_OTG_EPTypeDef *ep)
{
	//PRINTF(PSTR("USB_EPClearStall, ep->num=%02X\n"), ep->num);
	if (ep->is_in == 1)
	{
		USB_OTG_INEndpointTypeDef * const inep = USBx_INEP(ep->num);
		inep->DIEPCTL &= ~ USB_OTG_DIEPCTL_STALL;
		if (ep->type == USBD_EP_TYPE_INTR || ep->type == USBD_EP_TYPE_BULK)
		{
			inep->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM; /* DATA0 */
		}
	}
	else
	{
		USB_OTG_OUTEndpointTypeDef * const outep = USBx_OUTEP(ep->num);
		outep->DOEPCTL &= ~ USB_OTG_DOEPCTL_STALL;
		if (ep->type == USBD_EP_TYPE_INTR || ep->type == USBD_EP_TYPE_BULK)
		{
			outep->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM; /* DATA0 */
		}
	}
	return HAL_OK;
}

/**
  * @brief  USB_StopDevice : Stop the usb device mode
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef USB_StopDevice(USB_OTG_GlobalTypeDef *USBx)
{
  uint32_t i;
  
  /* Clear Pending interrupt */
  for (i = 0; i < 15 ; i++)
  {
    USBx_INEP(i)->DIEPINT  = 0xFF;
    USBx_OUTEP(i)->DOEPINT  = 0xFF;
  }
  USBx_DEVICE->DAINT = 0xFFFFFFFF;

  /* Clear interrupt masks */
  USBx_DEVICE->DIEPMSK  = 0;
  USBx_DEVICE->DOEPMSK  = 0;
  USBx_DEVICE->DAINTMSK = 0;

  /* Flush the FIFO */
  USB_FlushRxFifo(USBx);
  USB_FlushTxFifoAll(USBx); /* all Tx FIFOs */

  return HAL_OK;
}

/**
  * @brief  USB_SetDevAddress : Stop the usb device mode
  * @param  USBx : Selected device
  * @param  address : new device address to be assigned
  *          This parameter can be a value from 0 to 255
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_SetDevAddress (USB_OTG_GlobalTypeDef *USBx, uint_fast8_t address)
{
	USBx_DEVICE->DCFG = (USBx_DEVICE->DCFG & ~ (USB_OTG_DCFG_DAD)) |
		address * USB_OTG_DCFG_DAD_0 |
		0;

	return HAL_OK;
}

/**
  * @brief  USB_DevConnect : Connect the USB device by enabling the pull-up/pull-down
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_DevConnect (USB_OTG_GlobalTypeDef *USBx)
{
	//PRINTF(PSTR("USB_DevConnect (USBx=%p)\n"), USBx);
	USBx_DEVICE->DCTL &= ~ USB_OTG_DCTL_SDIS;
	ASSERT((USBx_DEVICE->DCTL & USB_OTG_DCTL_SDIS) == 0);

	HARDWARE_DELAY_MS(3);

	return HAL_OK;  
}


/**
  * @brief  USB_DevDisconnect : Disconnect the USB device by disabling the pull-up/pull-down
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_DevDisconnect (USB_OTG_GlobalTypeDef *USBx)
{
	//PRINTF(PSTR("USB_DevDisconnect (USBx=%p)\n"), USBx);

	USBx_DEVICE->DCTL |= USB_OTG_DCTL_SDIS;
	ASSERT((USBx_DEVICE->DCTL & USB_OTG_DCTL_SDIS) != 0);

	HARDWARE_DELAY_MS(3);

	return HAL_OK;  
}

/**
  * @brief  USB_ReadInterrupts: return the global USB interrupt status
  * @param  USBx : Selected device
  * @retval HAL status
  */
uint32_t  USB_ReadInterrupts (USB_OTG_GlobalTypeDef *USBx)
{
  uint32_t v = 0;

  v = USBx->GINTSTS;
  v &= USBx->GINTMSK;
  return v;
}

/**
  * @brief  USB_ReadDevAllOutEpInterrupt: return the USB device OUT endpoints interrupt status
  * @param  USBx : Selected device
  * @retval HAL status
  */
uint32_t USB_ReadDevAllOutEpInterrupt (USB_OTG_GlobalTypeDef *USBx)
{
  uint32_t v;
  v  = USBx_DEVICE->DAINT;
  v &= USBx_DEVICE->DAINTMSK;
  return ((v & 0xffff0000) >> 16);
}

/**
  * @brief  USB_ReadDevAllInEpInterrupt: return the USB device IN endpoints interrupt status
  * @param  USBx : Selected device
  * @retval HAL status
  */
uint32_t USB_ReadDevAllInEpInterrupt (USB_OTG_GlobalTypeDef *USBx)
{
  uint32_t v;
  v  = USBx_DEVICE->DAINT;
  v &= USBx_DEVICE->DAINTMSK;
  return ((v & 0xFFFF));
}

/**
  * @brief  Returns Device OUT EP Interrupt register
  * @param  USBx : Selected device
  * @param  epnum : endpoint number
  *          This parameter can be a value from 0 to 15
  * @retval Device OUT EP Interrupt register
  */
uint32_t USB_ReadDevOutEPInterrupt (USB_OTG_GlobalTypeDef *USBx, uint_fast8_t epnum)
{
  uint32_t v;
  v  = USBx_OUTEP(epnum)->DOEPINT;
  v &= USBx_DEVICE->DOEPMSK;
  return v;
}

/**
  * @brief  Returns Device IN EP Interrupt register
  * @param  USBx : Selected device
  * @param  epnum : endpoint number
  *          This parameter can be a value from 0 to 15
  * @retval Device IN EP Interrupt register
  */
uint32_t USB_ReadDevInEPInterrupt (USB_OTG_GlobalTypeDef *USBx, uint_fast8_t epnum)
{
  uint32_t v, msk, emp;

  msk = USBx_DEVICE->DIEPMSK;
  emp = USBx_DEVICE->DIEPEMPMSK;	// TXFE interrupt one bit per IN endpoint
  msk |= ((emp >> epnum) & 0x1) << USB_OTG_DIEPINT_TXFE_Pos;
  v = USBx_INEP(epnum)->DIEPINT & msk;
  return v;
}

/**
  * @brief  USB_ClearInterrupts: clear a USB interrupt
  * @param  USBx : Selected device
  * @param  interrupt : interrupt flag
  * @retval None
  */
void  USB_ClearInterrupts (USB_OTG_GlobalTypeDef *USBx, uint32_t interrupt)
{
  USBx->GINTSTS |= interrupt;
}

/**
  * @brief  Returns USB core mode
  * @param  USBx : Selected device
  * @retval return core mode : Host or Device
  *          This parameter can be one of these values:
  *           0 : Host
  *           1 : Device
  */
uint32_t USB_GetMode(USB_OTG_GlobalTypeDef *USBx)
{
  return ((USBx->GINTSTS ) & 0x1);
}


/**
  * @brief  Activate EP0 for Setup transactions
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_ActivateSetup (USB_OTG_GlobalTypeDef *USBx)
{
	/* Set the MPS of the IN EP based on the enumeration speed */
	USBx_INEP(0)->DIEPCTL &= ~ USB_OTG_DIEPCTL_MPSIZ;	// 64 bytes = code 0

	if ((USBx_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD) == DSTS_ENUMSPD_LS_PHY_6MHZ)
	{
		USBx_INEP(0)->DIEPCTL |= (DEP0CTL_MPS_8 << USB_OTG_DIEPCTL_MPSIZ_Pos);	// 8 bytes = code 3
	}
	USBx_DEVICE->DCTL |= USB_OTG_DCTL_CGINAK;

	return HAL_OK;
}


/**
  * @brief  Prepare the EP0 to start the first control setup
  * @param  USBx : Selected device
  * @param  dma: USB dma enabled or disabled
  *          This parameter can be one of these values:
  *           0 : DMA feature not used
  *           1 : DMA feature used
  * @param  psetup : pointer to setup packet
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EP0_OutStart(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t dma, uint8_t *psetup)
{
	USBx_OUTEP(0)->DOEPTSIZ = 0;
	USBx_OUTEP(0)->DOEPTSIZ |= USB_OTG_DOEPTSIZ_PKTCNT & (1 << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
	USBx_OUTEP(0)->DOEPTSIZ |= USB_OTG_DOEPTSIZ_XFRSIZ & ((3 * 8) << USB_OTG_DOEPTSIZ_XFRSIZ_Pos);
	USBx_OUTEP(0)->DOEPTSIZ |= USB_OTG_DOEPTSIZ_STUPCNT;

	if (dma == USB_ENABLE)
	{
		arm_hardware_flush_invalidate((uintptr_t) psetup, 4 * 12);	// need
		USBx_OUTEP(0)->DOEPDMA = (uint32_t) psetup;
		/* EP enable */
		USBx_OUTEP(0)->DOEPCTL =
			USB_OTG_DOEPCTL_EPENA |
			USB_OTG_DOEPCTL_USBAEP |
			0;
	}

	return HAL_OK;
}


/**
  * @brief  Reset the USB Core (needed after USB clock settings change)
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef USB_CoreReset(USB_OTG_GlobalTypeDef *USBx)
{
  uint32_t count = 0;

  /* Wait for AHB master IDLE state. */
  do
  {
    if (++count > 200000)
    {
      return HAL_TIMEOUT;
    }
  }
  while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL) == 0);

  /* Core Soft Reset */
  count = 0;
  USBx->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;

  do
  {
    if (++count > 200000)
    {
      return HAL_TIMEOUT;
    }
  }
  while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_CSRST) == USB_OTG_GRSTCTL_CSRST);

  return HAL_OK;
}

/**
  * @brief  USB_HostInit : Initializes the USB OTG controller registers
  *         for Host mode
  * @param  USBx : Selected device
  * @param  cfg  : pointer to a USB_OTG_CfgTypeDef structure that contains
  *         the configuration information for the specified USBx peripheral.
  * @retval HAL status
  */

HAL_StatusTypeDef USB_HostInit(USB_OTG_GlobalTypeDef *USBx, const USB_OTG_CfgTypeDef * cfg)
{
  uint32_t i;

  /* Restart the Phy Clock */
  USBx_PCGCCTL = 0;
  /* Activate VBUS Sensing B */
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || \
    defined(STM32F412Rx) || defined(STM32F412Cx)
  USBx->GCCFG |= USB_OTG_GCCFG_VBDEN;
#else
  USBx->GCCFG &=~ (USB_OTG_GCCFG_VBUSASEN);
  USBx->GCCFG &=~ (USB_OTG_GCCFG_VBUSBSEN);
  USBx->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
#endif /* STM32F446xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Rx || STM32F412Vx || STM32F412Cx */

  /* Disable the FS/LS support mode only */
  if ((cfg->speed == USB_OTG_SPEED_FULL) && (USBx != USB_OTG_FS))
  {
    USBx_HOST->HCFG |= USB_OTG_HCFG_FSLSS;
  }
  else
  {
    USBx_HOST->HCFG &= ~ (USB_OTG_HCFG_FSLSS);
  }

  /* Make sure the FIFOs are flushed. */
  USB_FlushTxFifoAll(USBx); /* all Tx FIFOs */
  USB_FlushRxFifo(USBx);

  /* Clear all pending HC Interrupts */
  for (i = 0; i < cfg->Host_channels; i++)
  {
    USBx_HC(i)->HCINT = 0xFFFFFFFF;
    USBx_HC(i)->HCINTMSK = 0;
  }

  /* Enable VBUS driving */
  USB_DriveVbus(USBx, 1);

  HARDWARE_DELAY_MS(200);

  /* Disable all interrupts. */
  USBx->GINTMSK = 0;

  /* Clear any pending interrupts */
  USBx->GINTSTS = 0xFFFFFFFF;

  if(USBx == USB_OTG_FS)
  {
    /* set Rx FIFO size */
    USBx->GRXFSIZ  = (uint32_t )0x80;
    USBx->DIEPTXF0_HNPTXFSIZ = (uint32_t )(((0x60 << 16)& USB_OTG_NPTXFD) | 0x80);
    USBx->HPTXFSIZ = (uint32_t )(((0x40 << 16)& USB_OTG_HPTXFSIZ_PTXFD) | 0xE0);
  }
  else
  {
    /* set Rx FIFO size */
    USBx->GRXFSIZ  = (uint32_t )0x200;
    USBx->DIEPTXF0_HNPTXFSIZ = (uint32_t )(((0x100 << 16)& USB_OTG_NPTXFD) | 0x200);
    USBx->HPTXFSIZ = (uint32_t )(((0xE0 << 16)& USB_OTG_HPTXFSIZ_PTXFD) | 0x300);
  }

  /* Enable the common interrupts */
  if (cfg->dma_enable == USB_DISABLE)
  {
    USBx->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM;
  }

  /* Enable interrupts matching to the Host mode ONLY */
  USBx->GINTMSK |= (USB_OTG_GINTMSK_PRTIM            | USB_OTG_GINTMSK_HCIM |
                    USB_OTG_GINTMSK_SOFM             |USB_OTG_GINTSTS_DISCINT|
                    USB_OTG_GINTMSK_PXFRM_IISOOXFRM  | USB_OTG_GINTMSK_WUIM);

  return HAL_OK;
}

/**
  * @brief  USB_InitFSLSPClkSel : Initializes the FSLSPClkSel field of the
  *         HCFG register on the PHY type and set the right frame interval
  * @param  USBx : Selected device
  * @param  freq : clock frequency
  *          This parameter can be one of these values:
  *           HCFG_48_MHZ : Full Speed 48 MHz Clock
  *           HCFG_6_MHZ : Low Speed 6 MHz Clock
  * @retval HAL status
  */
HAL_StatusTypeDef USB_InitFSLSPClkSel(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t freq)
{
  USBx_HOST->HCFG &= ~(USB_OTG_HCFG_FSLSPCS);
  USBx_HOST->HCFG |= (freq & USB_OTG_HCFG_FSLSPCS);
  
  if (freq ==  HCFG_48_MHZ)
  {
    USBx_HOST->HFIR = (uint32_t)48000;
  }
  else if (freq ==  HCFG_6_MHZ)
  {
    USBx_HOST->HFIR = (uint32_t)6000;
  }
  return HAL_OK;  
}

/**
* @brief  USB_OTG_ResetPort : Reset Host Port
  * @param  USBx : Selected device
  * @param  state : activate reset
  * @retval HAL status
  * @note : (1)The application must wait at least 10 ms
  *   before clearing the reset bit.
  */
HAL_StatusTypeDef USB_ResetPort(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t state)
{
  uint32_t hprt0 = USBx_HPRT0;

  hprt0 &= ~ (USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET | USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG);

  if (state)
	  USBx_HPRT0 = (USB_OTG_HPRT_PRST | hprt0);
  else
	  USBx_HPRT0 = ((~ USB_OTG_HPRT_PRST) & hprt0);

  return HAL_OK;
}

/**
  * @brief  USB_DriveVbus : activate or de-activate vbus
  * @param  state : VBUS state
  *          This parameter can be one of these values:
  *           0 : VBUS Active
  *           1 : VBUS Inactive
  * @retval HAL status
*/
HAL_StatusTypeDef USB_DriveVbus (USB_OTG_GlobalTypeDef *USBx, uint_fast8_t state)
{
  uint32_t hprt0 = USBx_HPRT0;
  hprt0 &= ~(USB_OTG_HPRT_PENA    | USB_OTG_HPRT_PCDET |
                         USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG );

  if (((hprt0 & USB_OTG_HPRT_PPWR) == 0 ) && (state == 1 ))
  {
    USBx_HPRT0 = (USB_OTG_HPRT_PPWR | hprt0);
  }
  if (((hprt0 & USB_OTG_HPRT_PPWR) == USB_OTG_HPRT_PPWR) && (state == 0 ))
  {
    USBx_HPRT0 = ((~USB_OTG_HPRT_PPWR) & hprt0);
  }
  return HAL_OK;
}

/**
  * @brief  Return Host Core speed
  * @param  USBx : Selected device
  * @retval speed : Host speed
  *          This parameter can be one of these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  *            @arg USB_OTG_SPEED_LOW: Low speed mode
  */
uint32_t USB_GetHostSpeed (USB_OTG_GlobalTypeDef *USBx)
{
  return ((USBx_HPRT0 & USB_OTG_HPRT_PSPD) / MASK2LSB(USB_OTG_HPRT_PSPD));
}

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  PCD_LPM_L0_ACTIVE = 0x00U, /* on */
  PCD_LPM_L1_ACTIVE = 0x01U, /* LPM L1 sleep */
}PCD_LPM_MsgTypeDef;

typedef enum
{
  PCD_BCD_ERROR                     = 0xFF,
  PCD_BCD_CONTACT_DETECTION         = 0xFE,
  PCD_BCD_STD_DOWNSTREAM_PORT       = 0xFD,
  PCD_BCD_CHARGING_DOWNSTREAM_PORT  = 0xFC,
  PCD_BCD_DEDICATED_CHARGING_PORT   = 0xFB,
  PCD_BCD_DISCOVERY_COMPLETED       = 0x00,

}PCD_BCD_MsgTypeDef;

HAL_StatusTypeDef HAL_PCDEx_ActivateLPM(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCDEx_DeActivateLPM(PCD_HandleTypeDef *hpcd);
void HAL_PCDEx_LPM_Callback(PCD_HandleTypeDef *hpcd, PCD_LPM_MsgTypeDef msg);


/** @defgroup PCD_Private_Macros PCD Private Macros
  * @{
  */
#define PCD_MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define PCD_MAX(a, b)  (((a) > (b)) ? (a) : (b))
/**
  * @}
  */

/* Private functions prototypes ----------------------------------------------*/
/** @defgroup PCD_Private_Functions PCD Private Functions
  * @{
  */
/**
  * @}
  */

#if USB_OTG_GLPMCFG_LPMEN
/**
  * @brief  Activate LPM feature
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_ActivateLPM(PCD_HandleTypeDef *hpcd)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;

	hpcd->lpm_active = USB_ENABLE;
	hpcd->LPM_State = LPM_L0;
	USBx->GINTMSK |= USB_OTG_GINTMSK_LPMINTM;
	USBx->GLPMCFG |= (USB_OTG_GLPMCFG_LPMEN | USB_OTG_GLPMCFG_LPMACK | USB_OTG_GLPMCFG_ENBESL);

	return HAL_OK;
}

/**
  * @brief  Deactivate LPM feature.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_DeActivateLPM(PCD_HandleTypeDef *hpcd)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;

	hpcd->lpm_active = USB_DISABLE;
	USBx->GINTMSK &= ~USB_OTG_GINTMSK_LPMINTM;
	USBx->GLPMCFG &= ~(USB_OTG_GLPMCFG_LPMEN | USB_OTG_GLPMCFG_LPMACK | USB_OTG_GLPMCFG_ENBESL);

	return HAL_OK;
}


#if (USBD_LPM_ENABLED == 1)
/**
  * @brief  HAL_PCDEx_LPM_Callback : Send LPM message to user layer
  * @param  hpcd: PCD handle
  * @param  msg: LPM message
  * @retval HAL status
  */
void HAL_PCDEx_LPM_Callback(PCD_HandleTypeDef *hpcd, PCD_LPM_MsgTypeDef msg)
{
  switch ( msg)
  {
  case PCD_LPM_L0_ACTIVE:
    if (hpcd->Init.low_power_enable)
    {
      SystemClock_Config();

      /* Reset SLEEPDEEP bit of Cortex System Control Register */
      SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
    }
    __HAL_PCD_UNGATE_PHYCLOCK(hpcd);
    USBD_LL_Resume(hpcd->pData);
    break;

  case PCD_LPM_L1_ACTIVE:
    __HAL_PCD_GATE_PHYCLOCK(hpcd);
    USBD_LL_Suspend(hpcd->pData);

    /*Enter in STOP mode */
    if (hpcd->Init.low_power_enable)
    {
      /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register */
      SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
    }
    break;
  }
}
#else /* USB_OTG_GLPMCFG_LPMEN */
/**
  * @brief  Send LPM message to user layer callback.
  * @param  hpcd: PCD handle
  * @param  msg: LPM message
  * @retval HAL status
  */
void HAL_PCDEx_LPM_Callback(PCD_HandleTypeDef *hpcd, PCD_LPM_MsgTypeDef msg)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  UNUSED(msg);
}

#endif

#endif /* USB_OTG_GLPMCFG_LPMEN */

#ifdef USB_OTG_GCCFG_BCDEN


/**
  * @brief  HAL_PCDEx_BatteryCharging_Callback : Send BatteryCharging message to user layer
  * @param  hpcd: PCD handle
  * @param  msg: LPM message
  * @retval HAL status
  */
void HAL_PCDEx_BCD_Callback(PCD_HandleTypeDef *hpcd, PCD_BCD_MsgTypeDef msg)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpcd);
  UNUSED(msg);
}

/**
  * @brief  HAL_PCDEx_BCD_VBUSDetect : handle BatteryCharging Process
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
void HAL_PCDEx_BCD_VBUSDetect(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
  uint32_t tickstart = HAL_GetTick();

  /* Start BCD When device is connected */
  if (USBx_DEVICE->DCTL & USB_OTG_DCTL_SDIS)
  {
    /* Enable DCD : Data Contact Detect */
    USBx->GCCFG |= USB_OTG_GCCFG_DCDEN;

    /* Wait Detect flag or a timeout is happen*/
    while ((USBx->GCCFG & USB_OTG_GCCFG_DCDET) == 0U)
    {
      /* Check for the Timeout */
      if ((HAL_GetTick() - tickstart ) > 1000U)
      {
        HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_ERROR);
        return;
      }
    }

    /* Right response got */
	HARDWARE_DELAY_MS(100);

    /* Check Detect flag*/
    if (USBx->GCCFG & USB_OTG_GCCFG_DCDET)
    {
      HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_CONTACT_DETECTION);
    }

    /*Primary detection: checks if connected to Standard Downstream Port
    (without charging capability) */
    USBx->GCCFG &=~ USB_OTG_GCCFG_DCDEN;
    USBx->GCCFG |=  USB_OTG_GCCFG_PDEN;
    HARDWARE_DELAY_MS(100);

    if (!(USBx->GCCFG & USB_OTG_GCCFG_PDET))
    {
      /* Case of Standard Downstream Port */
      HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_STD_DOWNSTREAM_PORT);
    }
    else
    {
      /* start secondary detection to check connection to Charging Downstream
      Port or Dedicated Charging Port */
      USBx->GCCFG &=~ USB_OTG_GCCFG_PDEN;
      USBx->GCCFG |=  USB_OTG_GCCFG_SDEN;
      HARDWARE_DELAY_MS(100);

      if ((USBx->GCCFG) & USB_OTG_GCCFG_SDET)
      {
        /* case Dedicated Charging Port  */
        HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_DEDICATED_CHARGING_PORT);
      }
      else
      {
        /* case Charging Downstream Port  */
        HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_CHARGING_DOWNSTREAM_PORT);
      }
    }
    /* Battery Charging capability discovery finished */
    HAL_PCDEx_BCD_Callback(hpcd, PCD_BCD_DISCOVERY_COMPLETED);
  }
}

/**
  * @brief  HAL_PCDEx_ActivateBCD : active BatteryCharging feature
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_ActivateBCD(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;

  hpcd->battery_charging_active = USB_ENABLE;
  USBx->GCCFG |= (USB_OTG_GCCFG_BCDEN);
  
  return HAL_OK;
}

/**
  * @brief  HAL_PCDEx_DeActivateBCD : de-active BatteryCharging feature
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_DeActivateBCD(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
  hpcd->battery_charging_active = USB_DISABLE;
  USBx->GCCFG &= ~(USB_OTG_GCCFG_BCDEN);
  return HAL_OK;
}

#endif //#ifdef USB_OTG_GCCFG_BCDEN


/**
  * @brief  Activate remote wakeup signalling.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;

  if ((USBx_DEVICE->DSTS & USB_OTG_DSTS_SUSPSTS) == USB_OTG_DSTS_SUSPSTS)
  {
    /* Activate Remote wakeup signaling */
    USBx_DEVICE->DCTL |= USB_OTG_DCTL_RWUSIG;
  }
  return HAL_OK;
}

/**
  * @brief  De-activate remote wakeup signalling.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd)
{
  USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
  
  /* De-activate Remote wakeup signaling */
   USBx_DEVICE->DCTL &= ~ (USB_OTG_DCTL_RWUSIG);
  return HAL_OK;
}
#endif /* (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX) */

/* Exported functions --------------------------------------------------------*/
/** @defgroup PCD_Exported_Functions PCD Exported Functions
  * @{
  */
/**
  * @brief  Return the PCD handle state.
  * @param  hpcd: PCD handle
  * @retval HAL state
  */
PCD_StateTypeDef HAL_PCD_GetState(PCD_HandleTypeDef *hpcd)
{
  return hpcd->State;
}

void HAL_PCD_SetState(PCD_HandleTypeDef *hpcd, PCD_StateTypeDef state)
{
  hpcd->State = state;
}

/** @defgroup PCD_Exported_Functions_Group1 Initialization and de-initialization functions
 *  @brief    Initialization and Configuration functions
 *
@verbatim
 ===============================================================================
            ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the PCD according to the specified
  *         parameters in the PCD_InitTypeDef and create the associated handle.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd)
{ 
  uint32_t i = 0;

  /* Check the PCD handle allocation */
  if(hpcd == NULL)
  {
    return HAL_ERROR;
  }
  /* Check the parameters */
  //assert_param(IS_PCD_ALL_INSTANCE(hpcd->Instance));

  HAL_PCD_SetState(hpcd, HAL_PCD_STATE_BUSY);
  
  /* Init the low level hardware : GPIO, CLOCK, NVIC... */
  HAL_PCD_MspInit(hpcd);

  /* Disable the Interrupts */
 __HAL_PCD_DISABLE(hpcd);

 /*Init the Core (common init.) */
 USB_CoreInit(hpcd->Instance, & hpcd->Init);

 /* Force Device Mode*/
 USB_SetCurrentMode(hpcd->Instance , USB_OTG_DEVICE_MODE);

	/* Init endpoints structures */
	for (i = 0; i < 15 ; i++)
	{
		/* Init ep structure */
		hpcd->IN_ep[i].is_in = 1;
		hpcd->IN_ep[i].num = i;
		hpcd->IN_ep[i].tx_fifo_num = i;
		/* Control until ep is activated */
		hpcd->IN_ep[i].type = USBD_EP_TYPE_CTRL;
		hpcd->IN_ep[i].maxpacket = 0;
		hpcd->IN_ep[i].xfer_buff = NULL;
		hpcd->IN_ep[i].xfer_len = 0;
	}

	for (i = 0; i < 15 ; i++)
	{
		hpcd->OUT_ep[i].is_in = 0;
		hpcd->OUT_ep[i].num = i;
		hpcd->IN_ep[i].tx_fifo_num = i;
		/* Control until ep is activated */
		hpcd->OUT_ep[i].type = USBD_EP_TYPE_CTRL;
		hpcd->OUT_ep[i].maxpacket = 0;
		hpcd->OUT_ep[i].xfer_buff = NULL;
		hpcd->OUT_ep[i].xfer_len = 0;

		////hpcd->Instance->DIEPTXF[i] = 0;
	}

#if CPUSTYLE_R7S721

	// Заполнение полей специфичных для RENESAS
	for (i = 0; i < sizeof usbd_usedpipes / sizeof usbd_usedpipes [0]; ++ i)
	{
		const uint_fast8_t pipe = usbd_usedpipes [i].pipe;
		const uint_fast8_t ep = usbd_usedpipes [i].ep;
		if (ep & 0x80)
		{
			hpcd->IN_ep[ep & 0x7F].pipe_num = pipe;
		}
		else
		{
			hpcd->OUT_ep[ep & 0x7F].pipe_num = pipe;
		}
	}
	hpcd->IN_ep[0].pipe_num = 0;
	hpcd->OUT_ep[0].pipe_num = 0;

#endif /* CPUSTYLE_R7S721 */

	/* Init Device */
	USB_DevInit(hpcd->Instance, & hpcd->Init);

	HAL_PCD_SetState(hpcd, HAL_PCD_STATE_READY);

#ifdef USB_OTG_GLPMCFG_LPMEN
 /* Activate LPM */
 if (hpcd->Init.lpm_enable == USB_ENABLE)
 {
   HAL_PCDEx_ActivateLPM(hpcd);
 }
#endif /* USB_OTG_GLPMCFG_LPMEN */

#ifdef USB_OTG_GCCFG_BCDEN
 /* Activate Battery charging */
 if (hpcd->Init.battery_charging_enable == USB_ENABLE)
 {
   HAL_PCDEx_ActivateBCD(hpcd);
 }
#endif /* USB_OTG_GCCFG_BCDEN */

 USB_DevDisconnect (hpcd->Instance);
 return HAL_OK;
}

/**
  * @brief  DeInitializes the PCD peripheral.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DeInit(PCD_HandleTypeDef *hpcd)
{
  /* Check the PCD handle allocation */
  if(hpcd == NULL)
  {
    return HAL_ERROR;
  }

  HAL_PCD_SetState(hpcd, HAL_PCD_STATE_BUSY);

  /* Stop Device */
  HAL_PCD_Stop(hpcd);

  /* DeInit the low level hardware */
  HAL_PCD_MspDeInit(hpcd);

  HAL_PCD_SetState(hpcd, HAL_PCD_STATE_RESET);
  
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup PCD_Exported_Functions_Group2 Input and Output operation functions
 *  @brief   Data transfers functions
 *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the PCD data
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Start The USB OTG Device.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd)
{
  __HAL_LOCK(hpcd);
  USB_DevConnect (hpcd->Instance);
  __HAL_PCD_ENABLE(hpcd);
  __HAL_UNLOCK(hpcd);
  return HAL_OK;
}

/**
  * @brief  Stop The USB OTG Device.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd)
{
  __HAL_LOCK(hpcd);
  __HAL_PCD_DISABLE(hpcd);
  USB_StopDevice(hpcd->Instance);
  USB_DevDisconnect (hpcd->Instance);
  __HAL_UNLOCK(hpcd);
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup PCD_Exported_Functions_Group3 Peripheral Control functions
 *  @brief   management functions
 *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the PCD data
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Set the USB Device address.
  * @param  hpcd: PCD handle
  * @param  address: new device address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint_fast8_t address)
{
  __HAL_LOCK(hpcd);
  USB_SetDevAddress(hpcd->Instance, address);
  __HAL_UNLOCK(hpcd);
  return HAL_OK;
}
/**
  * @brief  Open and configure an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @param  ep_mps: endpoint max packet size
  * @param  ep_type: endpoint type
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint_fast8_t ep_addr, uint_fast8_t tx_fifo_num, uint_fast16_t ep_mps, uint_fast8_t ep_type)
{
	HAL_StatusTypeDef  ret = HAL_OK;
	USB_OTG_EPTypeDef *ep;

	if ((ep_addr & 0x80) == 0x80)
	{
		ep = & hpcd->IN_ep [ep_addr & 0x7F];
	}
	else
	{
		ep = & hpcd->OUT_ep [ep_addr & 0x7F];
	}
	ep->num   = ep_addr & 0x7F;

	ep->is_in = (0x80 & ep_addr) != 0;
	ep->maxpacket = ep_mps;
	ep->type = ep_type;
	if (ep->is_in)
	{
		/* Assign a Tx FIFO */
		ep->tx_fifo_num = tx_fifo_num;
	}
	/* Set initial data PID. */
	if (ep_type == USBD_EP_TYPE_BULK )
	{
		//ep->data_pid_start = 0; // нигде не используется
	}

	__HAL_LOCK(hpcd);

	//alex
	if ((hpcd->Init.use_dedicated_ep1 == USB_ENABLE) && (ep->num == 1))
		USB_ActivateDedicatedEndpoint(hpcd->Instance, ep);
	else
		USB_ActivateEndpoint(hpcd->Instance, ep);
	__HAL_UNLOCK(hpcd);
	return ret;
}


/**
  * @brief  Deactivate an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint_fast8_t ep_addr)
{
	USB_OTG_EPTypeDef *ep;

	if ((ep_addr & 0x80) == 0x80)
	{
		ep = & hpcd->IN_ep [ep_addr & 0x7F];
	}
	else
	{
		ep = & hpcd->OUT_ep [ep_addr & 0x7F];
	}
	ep->num = ep_addr & 0x7F;

	ep->is_in = (0x80 & ep_addr) != 0;

	__HAL_LOCK(hpcd);
	if ((hpcd->Init.use_dedicated_ep1 == USB_ENABLE) && (ep->num == 1))
		USB_DeactivateDedicatedEndpoint(hpcd->Instance, ep);
	else
		USB_DeactivateEndpoint(hpcd->Instance, ep);
	__HAL_UNLOCK(hpcd);
	return HAL_OK;
}


/**
  * @brief  Receive an amount of data.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @param  pBuf: pointer to the reception buffer
  * @param  len: amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint_fast8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
	USB_OTG_EPTypeDef * const ep = & hpcd->OUT_ep [ep_addr & 0x7F];

	/*setup and start the Xfer */
	ep->xfer_buff = pBuf;
	ep->xfer_len = len;
	ep->xfer_count = 0;
	ep->is_in = 0;
	ep->num = ep_addr & 0x7F;

	if (hpcd->Init.dma_enable == USB_ENABLE)
	{
		if (pBuf != NULL && len != 0)
			arm_hardware_flush_invalidate((uintptr_t) pBuf, len);
		ep->dma_addr = (uintptr_t) pBuf;
	}

	__HAL_LOCK(hpcd);

	if ((ep_addr & 0x7F) == 0 )
	{
		USB_EP0StartXfer(hpcd->Instance, ep, hpcd->Init.dma_enable);
	}
	else
	{
		USB_EPStartXfer(hpcd->Instance, ep, hpcd->Init.dma_enable);
	}

	__HAL_UNLOCK(hpcd);

	return HAL_OK;
}

/**
  * @brief  Get Received Data Size.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval Data Size
  */
uint16_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint_fast8_t ep_addr)
{
  return hpcd->OUT_ep [ep_addr & 0x7F].xfer_count;
}
/**
  * @brief  Send an amount of data.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @param  pBuf: pointer to the transmission buffer
  * @param  len: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint_fast8_t ep_addr, const uint8_t *pBuf, uint_fast32_t len)
{

	USB_OTG_EPTypeDef * const ep = & hpcd->IN_ep [ep_addr & 0x7F];

	/*setup and start the Xfer */
	ep->xfer_buff = (uint8_t *) pBuf;
	ep->xfer_len = len;
	ep->xfer_count = 0;
	ep->is_in = 1;
	ep->num = ep_addr & 0x7F;

	if (hpcd->Init.dma_enable == USB_ENABLE)
	{
		if (pBuf != NULL && len != 0)
			arm_hardware_flush((uintptr_t) pBuf, len);
		ep->dma_addr = (uintptr_t) pBuf;
	}

	__HAL_LOCK(hpcd);

	if ((ep_addr & 0x7F) == 0 )
	{
		USB_EP0StartXfer(hpcd->Instance, ep, hpcd->Init.dma_enable);
	}
	else
	{
		USB_EPStartXfer(hpcd->Instance, ep, hpcd->Init.dma_enable);
	}

	__HAL_UNLOCK(hpcd);

	return HAL_OK;
}

/**
  * @brief  Set a STALL condition over an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint_fast8_t ep_addr)
{
  USB_OTG_EPTypeDef *ep;

 if ((ep_addr & 0x0F) > hpcd->Init.dev_endpoints)
  {
	 ASSERT(0);
    return HAL_ERROR;
  }

 if ((0x80 & ep_addr) == 0x80)
  {
    ep = & hpcd->IN_ep [ep_addr & 0x7F];
  }
  else
  {
    ep = & hpcd->OUT_ep [ep_addr];
  }

  ep->is_stall = 1;
  ep->num   = ep_addr & 0x7F;
  ep->is_in = ((ep_addr & 0x80) == 0x80);


  __HAL_LOCK(hpcd);
  USB_EPSetStall(hpcd->Instance , ep);
  if ((ep_addr & 0x7F) == 0)
  {
    USB_EP0_OutStart(hpcd->Instance, hpcd->Init.dma_enable, (uint8_t *)hpcd->PSetup);
  }
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

/**
  * @brief  Clear a STALL condition over in an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint_fast8_t ep_addr)
{
  USB_OTG_EPTypeDef * ep;

 if ((ep_addr & 0x0F) > hpcd->Init.dev_endpoints)
  {
	 ASSERT(0);
    return HAL_ERROR;
  }

 if ((0x80 & ep_addr) != 0)
  {
    ep = & hpcd->IN_ep [ep_addr & 0x7F];
  }
  else
  {
    ep = & hpcd->OUT_ep [ep_addr];
  }

  ep->is_stall = 0;
  ep->num   = ep_addr & 0x7F;
  ep->is_in = ((ep_addr & 0x80) != 0);

  __HAL_LOCK(hpcd);
  USB_EPClearStall(hpcd->Instance , ep);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

/**
  * @brief  Flush an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint_fast8_t ep_addr)
{
  __HAL_LOCK(hpcd);

  if ((ep_addr & 0x80) != 0)
  {
    USB_FlushTxFifo(hpcd->Instance, ep_addr & 0x0F);
  }
  else
  {
    USB_FlushRxFifo(hpcd->Instance);
  }

  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

/**
  * @}
  */





/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/** @addtogroup USBD_OTG_DRIVER
  * @{
  */

/** @defgroup USBD_CONF
  * @brief usb otg low level driver configuration file
  * @{
  */

/** @defgroup USBD_CONF_Exported_Defines
  * @{
  */


static void Error_Handler(void);

/**
  * @brief  Setup stage callback
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_SetupStage((USBD_HandleTypeDef*)hpcd->pData, hpcd->PSetup);
}

/**
  * @brief  Data Out stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number - without direction bit
  * @retval None
  */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_DataOutStage((USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->OUT_ep [epnum].xfer_buff);
}

/**
  * @brief  Data In stage callback..
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number - without direction bit
  * @retval None
  */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_DataInStage((USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->IN_ep [epnum].xfer_buff);
}

/**
  * @brief  SOF callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_SOF((USBD_HandleTypeDef*)hpcd->pData);
}

/**
  * @brief  Reset callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
	USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

	/*Set USB Current Speed*/
	switch (hpcd->Init.speed)
	{
	case PCD_SPEED_HIGH:
		speed = USBD_SPEED_HIGH;
		break;
	case PCD_SPEED_FULL:
		speed = USBD_SPEED_FULL;
		break;

	default:
		speed = USBD_SPEED_FULL;
		break;
	}
	PRINTF(PSTR("HAL_PCD_ResetCallback: speed=%d\n"), (int) speed);
	USBD_LL_SetSpeed((USBD_HandleTypeDef*)hpcd->pData, speed);	// USBD_SPEED_xxx

	/*Reset Device*/
	USBD_LL_Reset((USBD_HandleTypeDef*)hpcd->pData);
}

#if (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX)


/**
  * @brief  Suspend callback.
  * When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
   /* Inform USB library that core enters in suspend Mode */
  USBD_LL_Suspend((USBD_HandleTypeDef*)hpcd->pData);
  __HAL_PCD_GATE_PHYCLOCK(hpcd);
  /*Enter in STOP mode */
  /* USER CODE BEGIN 2 */
  if (hpcd->Init.low_power_enable)
  {
    /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register */
    SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
  }
  /* USER CODE END 2 */
}

/**
  * @brief  Resume callback.
    When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
  /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
  USBD_LL_Resume((USBD_HandleTypeDef*)hpcd->pData);

}

/**
  * @brief  ISOOUTIncomplete callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_IsoOUTIncomplete((USBD_HandleTypeDef*)hpcd->pData, epnum);
}

/**
  * @brief  ISOINIncomplete callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_IsoINIncomplete((USBD_HandleTypeDef*)hpcd->pData, epnum);
}

/**
  * @brief  ConnectCallback callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_DevConnected((USBD_HandleTypeDef*)hpcd->pData);
}

/**
  * @brief  Disconnect callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_DevDisconnected((USBD_HandleTypeDef*)hpcd->pData);
}

/*******************************************************************************
                       LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/

#if 0
	// эти константы описаны только для STM32F4xx
	/****************************** USB Exported Constants ************************/
	#define USB_OTG_FS_HOST_MAX_CHANNEL_NBR                8U
	#define USB_OTG_FS_MAX_IN_ENDPOINTS                    4U    /* Including EP0 */
	#define USB_OTG_FS_MAX_OUT_ENDPOINTS                   4U    /* Including EP0 */
	#define USB_OTG_FS_TOTAL_FIFO_SIZE                     1280U /* in Bytes */

	#define USB_OTG_HS_HOST_MAX_CHANNEL_NBR                12U
	#define USB_OTG_HS_MAX_IN_ENDPOINTS                    6U    /* Including EP0 */
	#define USB_OTG_HS_MAX_OUT_ENDPOINTS                   6U    /* Including EP0 */
	#define USB_OTG_HS_TOTAL_FIFO_SIZE                     4096U /* in Bytes */
#endif



  /*  TXn min size = 16 words. (n  : Transmit FIFO index)
      When a TxFIFO is not used, the Configuration should be as follows:
          case 1 :  n > m    and Txn is not used    (n,m  : Transmit FIFO indexes)
         --> Txm can use the space allocated for Txn.
         case2  :  n < m    and Txn is not used    (n,m  : Transmit FIFO indexes)
         --> Txn should be configured with the minimum space of 16 words
     The FIFO is used optimally when used TxFIFOs are allocated in the top
         of the FIFO.Ex: use EP1 and EP2 as IN instead of EP1 and EP3 as IN ones.
     When DMA is used 3n * FIFO locations should be reserved for internal DMA registers */

static uint32_t usbd_makeTXFSIZ(uint_fast16_t base, uint_fast16_t size)
{
	return
		((uint32_t) size << USB_OTG_DIEPTXF_INEPTXFD_Pos) |
		((uint32_t) base << USB_OTG_DIEPTXF_INEPTXSA_Pos) |
		0;
}

// Преобразование размера в байтах размера данных в требования к fifo
// Расчет аргумента функции HAL_PCDEx_SetRxFiFo, HAL_PCDEx_SetTxFiFo
static uint_fast16_t size2buff4(uint_fast16_t size)
{
	const uint_fast16_t size4 = (size + 3) / 4;		// размер в 32-бит значениях
	return ulmax16(0x10, size4);
}

static void usbd_fifo_initialize(PCD_HandleTypeDef * hpcd, uint_fast16_t fullsize, uint_fast8_t bigbuff)
{
	uint_fast8_t i;
	const int add3tx = bigbuff ? 3 : 1;	// tx fifo add places
	const int mul2 = bigbuff ? 3 : 1;	// tx fifo buffers
	PCD_TypeDef * const instance = hpcd->Instance;

	PRINTF(PSTR("usbd_fifo_initialize: power-on GRXFSIZ=%u\n"), instance->GRXFSIZ & USB_OTG_GRXFSIZ_RXFD);
	// DocID028270 Rev 2 (RM0410): 41.11.3 FIFO RAM allocation
	// DocID028270 Rev 2 (RM0410): 41.16.6 Device programming model

/*
3. Set up the Data FIFO RAM for each of the FIFOs –
	Program the OTG_GRXFSIZ register, to be able to receive control OUT data and setup data.
	If thresholding is not enabled, at a minimum, this must be equal to
	1 max packet size of control endpoint 0 +
	2 Words (for the status of the control OUT data packet) +
	10 Words (for setup packets). –
	Program the OTG_DIEPTXF0 register (depending on the FIFO number chosen)
	to be able to transmit control IN data.
	At a minimum, this must be equal to 1 max packet size of control endpoint 0.

  */
	uint_fast16_t maxoutpacketsize4 = size2buff4(USB_OTG_MAX_EP0_SIZE);

	// добавление шести обеспечивает работу конфигурации с единственным устройством HID
	maxoutpacketsize4 += 6;

	//uint_fast16_t base4;
	uint_fast8_t numcontrolendpoints = 1;
	uint_fast8_t numoutendpoints = 1;

	// при addplaces = 3 появился звук на передаче в трансивер (при 2-х компортах)
	// но если CDC и UAC включать поодиночке, обмен не нарушается и при 0.
	// todo: найти все-таки документ https://www.synopsys.com/ip_prototyping_kit_usb3otgv2_drd_pc.pdf
	// еще интересен QL-Hi-Speed-USB-2.0-OTG-Controller-Data-Sheet.pdf

	uint_fast8_t addplaces = 3;

	const uint_fast16_t full4 = fullsize / 4;
	uint_fast16_t last4 = full4;
	uint_fast16_t base4 = 0;
#if WITHUSBCDC
	// параметры TX FIFO для ендпоинтов, в которые никогда не будут идти данные для передачи
	const uint_fast16_t size4dummy = 0;//0x10;//bigbuff ? 0x10 : 4;
	//last4 -= size4dummy;
	const uint_fast16_t last4dummy = last4;
#endif /* WITHUSBCDC */

	PRINTF(PSTR("usbd_fifo_initialize1: 4*(full4-last4)=%u\n"), 4 * (full4 - last4));

#if WITHUSBUAC
	{
		/* endpoint передачи звука в компьютер */
		const uint_fast8_t pipe = (USBD_EP_AUDIO_IN) & 0x7F;

		numoutendpoints += 1;
#if WITHUSBUACIN2
		#if WITHRTS96
			const int nuacinpackets = 1 * mul2, nuacoutpackets = 1 * mul2;
		#elif WITHRTS192
			const int nuacinpackets = 1 * mul2, nuacoutpackets = 1 * mul2;
		#else /* WITHRTS96 || WITHRTS192 */
			const int nuacinpackets = 1 * mul2, nuacoutpackets = 1 * mul2;
		#endif /* WITHRTS96 || WITHRTS192 */
#else /* WITHUSBUACIN2 */
		#if WITHRTS96
			const int nuacinpackets = 1 * mul2, nuacoutpackets = 1 * mul2;
		#elif WITHRTS192
			const int nuacinpackets = 1 * mul2, nuacoutpackets = 1 * mul2;
		#else /* WITHRTS96 || WITHRTS192 */
			const int nuacinpackets = 2 * mul2, nuacoutpackets = 1 * mul2;
		#endif /* WITHRTS96 || WITHRTS192 */
#endif /* WITHUSBUACIN2 */
		const uint_fast16_t uacinmaxpacket = usbd_getuacinmaxpacket();
		maxoutpacketsize4 = ulmax16(maxoutpacketsize4, nuacoutpackets * size2buff4(VIRTUAL_AUDIO_PORT_DATA_SIZE_OUT));

		const uint_fast16_t size4 = nuacinpackets * (size2buff4(uacinmaxpacket) + add3tx);
		ASSERT(last4 >= size4);
		last4 -= size4;
		instance->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
		PRINTF(PSTR("usbd_fifo_initialize2 - UAC %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
	}
#if WITHUSBUACIN2
	{
		/* endpoint передачи звука (спектра) в компьютер */
		const uint_fast8_t pipe = (USBD_EP_RTS_IN) & 0x7F;

		const int nuacinpackets = 1 * mul2;
		const uint_fast16_t uacinmaxpacket = usbd_getuacinrtsmaxpacket();

		const uint_fast16_t size4 = nuacinpackets * (size2buff4(uacinmaxpacket) + add3tx);
		ASSERT(last4 >= size4);
		last4 -= size4;
		instance->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
		PRINTF(PSTR("usbd_fifo_initialize3 - UAC3 %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
	}
#endif /* WITHUSBUACIN2 */
#endif /* WITHUSBUAC */

#if WITHUSBCDC
	for (i = 0; i < WITHUSBHWCDC_N; ++ i)
	{
		/* полнофункциональное устройство */
		const uint_fast8_t pipe = (USBD_EP_CDC_IN + i) & 0x7F;
		const uint_fast8_t pipeint = (USBD_EP_CDC_INT + i) & 0x7F;
		numoutendpoints += 1;
		if (bigbuff == 0 && i > 0)
		{
			// на маленьких контроллерах только первый USB CDC может обмениваться данными
			instance->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4dummy, size4dummy);
			instance->DIEPTXF [pipeint - 1] = usbd_makeTXFSIZ(last4dummy, size4dummy);

		}
		else
		{
			#if WITHUSBUAC
				#if WITHUSBUACIN2
					const int ncdcindatapackets = 1 * mul2, ncdcoutdatapackets = 3;
				#elif WITHRTS96 || WITHRTS192
					const int ncdcindatapackets = 2 * mul2, ncdcoutdatapackets = 3;
				#else /* WITHRTS96 || WITHRTS192 */
					const int ncdcindatapackets = 2 * mul2, ncdcoutdatapackets = 3;
				#endif /* WITHRTS96 || WITHRTS192 */
			#else /* WITHUSBUAC */
				const int ncdcindatapackets = 4, ncdcoutdatapackets = 4;
			#endif /* WITHUSBUAC */

			maxoutpacketsize4 = ulmax16(maxoutpacketsize4, ncdcoutdatapackets * size2buff4(VIRTUAL_COM_PORT_OUT_DATA_SIZE));


			const uint_fast16_t size4 = ncdcindatapackets * (size2buff4(VIRTUAL_COM_PORT_IN_DATA_SIZE) + add3tx);
			ASSERT(last4 >= size4);
			last4 -= size4;
			instance->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
			instance->DIEPTXF [pipeint - 1] = usbd_makeTXFSIZ(last4dummy, size4dummy);
			PRINTF(PSTR("usbd_fifo_initialize4 CDC %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
		}
	}

#endif /* WITHUSBCDC */

#if WITHUSBHID && 0
	{
		/* ... устройство */
		const uint_fast8_t pipe = USBD_EP_HIDMOUSE_INT & 0x7F;

		const uint_fast16_t size4 = size2buff4(HIDMOUSE_INT_DATA_SIZE);
		ASSERT(last4 >= size4);
		last4 -= size4;
		instance->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
		PRINTF(PSTR("usbd_fifo_initialize8 HID %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
	}
#endif /* WITHUSBHID */

	PRINTF(PSTR("usbd_fifo_initialize9: 4*(full4-last4)=%u\n"), 4 * (full4 - last4));

	/* control endpoint TX FIFO */
	{
		/* Установить размер TX FIFO EP0 */
		const uint_fast16_t size4 = 2 * (size2buff4(USB_OTG_MAX_EP0_SIZE) + add3tx);
		ASSERT(last4 >= size4);
		last4 -= size4;
		instance->DIEPTXF0_HNPTXFSIZ = usbd_makeTXFSIZ(last4, size4);
		PRINTF(PSTR("usbd_fifo_initialize10 TX FIFO %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
	}
	/* control endpoint RX FIFO */
	{
		/* Установить размер RX FIFO -  теперь все что осталоь - используем last4 вместо size4 */
		// (4 * number of control endpoints + 6) +
		// ((largest USB packet used / 4) + 1 for status information) +
		// (2 * number of OUT endpoints) +
		// 1 for Global NAK
		const uint_fast16_t size4 =
				(4 * numcontrolendpoints + 6) +
				(maxoutpacketsize4 + 1) +
				(2 * numoutendpoints) +
				1 +
				addplaces;

		PRINTF(PSTR("usbd_fifo_initialize11 RX FIFO %u bytes: 4*(full4-last4)=%u bytes (last4=%u, size4=%u)\n"), 4 * size4, 4 * (full4 - last4), last4, size4);
		ASSERT(last4 >= size4);
		instance->GRXFSIZ = (instance->GRXFSIZ & ~ USB_OTG_GRXFSIZ_RXFD) |
			(last4 << USB_OTG_GRXFSIZ_RXFD_Pos) |	// was: size4 - то что осталось
			0;
		base4 += size4;
	}

	if (base4 > last4 || last4 > full4)
	{
		char b [64];
		PRINTF(PSTR("usbd_fifo_initialize error: base4=%u, last4=%u, fullsize=%u\n"), (base4 * 4), (last4 * 4), fullsize);
		local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("used=%u"), (base4 * 4) + (fullsize - last4 * 4));
		display_setcolors(COLOR_RED, BGCOLOR);
		display_at(0, 0, b);
		for (;;)
			;
	}
	else
	{
		PRINTF(PSTR("usbd_fifo_initialize: base4=%u, last4=%u, fullsize=%u\n"), (base4 * 4), (last4 * 4), fullsize);
#if 0
		// Диагностическая выдача использованного объёма FIFO RAM
		char b [64];

		local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("used=%u"), (base4 * 4) + (fullsize - last4 * 4));
		display_setcolors(COLOR_GREEN, BGCOLOR);
		display_at(0, 0, b);
		HARDWARE_DELAY_MS(2000);
#endif
	}

	USB_FlushRxFifo(instance);
	USB_FlushTxFifoAll(instance);
}
#endif /* (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX) */

// На RENESAS ничего не делаем - req уже заполнен чтением из контроллера USB
static void USBD_ParseSetupRequest(USBD_SetupReqTypedef *req, const uint32_t * pdata)
{
#if CPUSTYLE_STM32
	req->bmRequest     = (pdata [0] >> 0) & 0x00FF;
	req->bRequest      = (pdata [0] >> 8) & 0x00FF;
	req->wValue        = (pdata [0] >> 16) & 0xFFFF;
	req->wIndex        = (pdata [1] >> 0) & 0xFFFF;
	req->wLength       = (pdata [1] >> 16) & 0xFFFF;

#elif CPUSTYLE_R7S721 && 0
	const uint_fast16_t usbreq = USBx->USBREQ;

	req->bmRequest     = LO_BYTE(usbreq & USB_FUNCTION_bmRequestType); //(pdata [0] >> 0) & 0x00FF;
	req->bRequest      = HI_BYTE(usbreq & USB_FUNCTION_bRequest); //(pdata [0] >> 8) & 0x00FF;
	req->wValue        = USBx->USBVAL; //(pdata [0] >> 16) & 0xFFFF;
	req->wIndex        = USBx->USBINDX; //(pdata [1] >> 0) & 0xFFFF;
	req->wLength       = USBx->USBLENG; //(pdata [1] >> 16) & 0xFFFF;
#else
	//#error Undefined CPUSTYLE_xxx
#endif /* CPUSTYLE_STM32 */
#if 0
	PRINTF(PSTR("USBD_ParseSetupRequest: bmRequest=%04X, bRequest=%04X, wValue=%04X, wIndex=%04X, wLength=%04X\n"),
		req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
#endif
}




/**
  * @brief  Starts the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_LL_Start(USBD_HandleTypeDef *pdev)
{
  HAL_PCD_Start((PCD_HandleTypeDef*)pdev->pData);
  return USBD_OK;
}

/**
  * @brief  Stops the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_LL_Stop (USBD_HandleTypeDef *pdev)
{
  HAL_PCD_Stop(pdev->pData);
  return USBD_OK;
}

/**
  * @brief  Opens an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  ep_type: Endpoint Type
  * @param  ep_mps: Endpoint Max Packet Size
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_LL_OpenEP(
	USBD_HandleTypeDef *pdev,
	uint8_t  ep_addr,
	uint8_t  ep_type,
	uint16_t ep_mps)
{
	HAL_PCD_EP_Open((PCD_HandleTypeDef*) pdev->pData,
		ep_addr,
		ep_addr & 0x7F,	// tx_fifo_num
		ep_mps,
		ep_type);

	return USBD_OK;
}

/**
  * @brief  Closes an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_LL_CloseEP (USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_PCD_EP_Close((PCD_HandleTypeDef*) pdev->pData, ep_addr);
  return USBD_OK;
}

/**
  * @brief  Flushes an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_LL_FlushEP (USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_PCD_EP_Flush((PCD_HandleTypeDef*) pdev->pData, ep_addr);
  return USBD_OK;
}

/**
  * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_LL_StallEP (USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_PCD_EP_SetStall((PCD_HandleTypeDef *) pdev->pData, ep_addr);
  return USBD_OK;
}

/**
  * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_LL_ClearStallEP (USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
	HAL_PCD_EP_ClrStall((PCD_HandleTypeDef *) pdev->pData, ep_addr);
	return USBD_OK;
}

/**
  * @brief  Returns Stall condition.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval Stall (1: Yes, 0: No)
  */
uint8_t USBD_LL_IsStallEP (USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
	PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef *) pdev->pData;

	if ((ep_addr & 0x80) != 0)
	{
		return hpcd->IN_ep [ep_addr & 0x7F].is_stall;
	}
	else
	{
		return hpcd->OUT_ep [ep_addr & 0x7F].is_stall;
	}
}
/**
  * @brief  Assigns a USB address to the device.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_LL_SetUSBAddress (USBD_HandleTypeDef *pdev, uint8_t dev_addr)
{
  HAL_PCD_SetAddress((PCD_HandleTypeDef*) pdev->pData, dev_addr);
  return USBD_OK;
}

/**
  * @brief  Transmits data over an endpoint.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  pbuf: Pointer to data to be sent
  * @param  size: Data size
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_LL_Transmit(USBD_HandleTypeDef *pdev,
                                      uint_fast8_t  ep_addr,
                                      const uint8_t  *pbuf,
                                      uint_fast32_t  size)
{
  HAL_PCD_EP_Transmit((PCD_HandleTypeDef*) pdev->pData, ep_addr, pbuf, size);
  return USBD_OK;
}

/**
  * @brief  Prepares an endpoint for reception.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  pbuf: Pointer to data to be received
  * @param  size: Data size
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev,
                                           uint8_t  ep_addr,
                                           uint8_t  *pbuf,
                                           uint16_t  size)
{
	HAL_PCD_EP_Receive((PCD_HandleTypeDef*) pdev->pData, ep_addr, pbuf, size);
	return USBD_OK;
}

/**
  * @brief  Returns the last transfered packet size.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval Recived Data Size
  */
uint32_t USBD_LL_GetRxDataSize  (USBD_HandleTypeDef *pdev, uint8_t  ep_addr)
{
  return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef*) pdev->pData, ep_addr);
}

/** @defgroup USBD_CORE_Private_Functions
* @{
*/
/**
  * @brief  USBD_Start
  *         Start the USB Device Core.
  * @param  pdev: Device Handle
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_Start  (USBD_HandleTypeDef *pdev)
{

  /* Start the low level driver  */
  USBD_LL_Start(pdev);

  return USBD_OK;
}
/**
  * @brief  USBD_Stop
  *         Stop the USB Device Core.
  * @param  pdev: Device Handle
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_Stop(USBD_HandleTypeDef *pdev)
{
	/* Free Class Resources */
	uint_fast8_t di;
	for (di = 0; di < pdev->nClasses; ++ di)
	{
		/* for each device function */
		const USBD_ClassTypeDef * const pClass = pdev->pClasses [di];
		pClass->DeInit(pdev, pdev->dev_config [0]);
	}

	/* Stop the low level driver  */
	USBD_LL_Stop(pdev);

	return USBD_OK;
}

/**
* @brief  USBD_RunTestMode
*         Launch test mode process
* @param  pdev: device instance
* @retval status
*/
USBD_StatusTypeDef  USBD_RunTestMode (USBD_HandleTypeDef  *pdev)
{
  return USBD_OK;
}


/**
* @brief  USBD_SetClassConfig
*        Configure device and start the interfacei
* @param  pdev: device instance
* @param  cfgidx: configuration index
* @retval status
*/

USBD_StatusTypeDef USBD_SetClassConfig(USBD_HandleTypeDef  *pdev, uint_fast8_t cfgidx)
{
	USBD_StatusTypeDef   ret = pdev->nClasses == 0 ? USBD_FAIL : USBD_OK;
	uint_fast8_t di;

	for (di = 0; di < pdev->nClasses; ++ di)
	{
		/* for each device function */
		const USBD_ClassTypeDef * const pClass = pdev->pClasses [di];
		if (pClass != NULL)
		{
			/* Set configuration  and Start the Class*/
			if (pClass->Init(pdev, cfgidx) != USBD_OK)
			{
				ret = USBD_FAIL;
			}
		}
	}
	return ret;
}

/**
* @brief  USBD_ClrClassConfig
*         Clear current configuration
* @param  pdev: device instance
* @param  cfgidx: configuration index
* @retval status: USBD_StatusTypeDef
*/
USBD_StatusTypeDef USBD_ClrClassConfig(USBD_HandleTypeDef  *pdev, uint_fast8_t cfgidx)
{
	/* Clear configuration  and De-initialize the Class process*/
	uint_fast8_t di;
	for (di = 0; di < pdev->nClasses; ++ di)
	{
		/* for each device function */
		const USBD_ClassTypeDef * const pClass = pdev->pClasses [di];
		pClass->DeInit(pdev, cfgidx);
	}
	return USBD_OK;
}


/**
* @brief  USBD_CtlPrepareRx
*         receive data on the ctl pipe
* @param  pdev: device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be received
* @retval status
*/
USBD_StatusTypeDef  USBD_CtlPrepareRx(USBD_HandleTypeDef  *pdev,
                                  uint8_t *pbuf,
                                  uint16_t len)
{
	//PRINTF(PSTR("USBD_CtlPrepareRx, len=%d\n"), (int) len);
	/* Set EP0 State */
	pdev->ep0_state = USBD_EP0_DATA_OUT;
	pdev->ep_out[0].total_length = len;
	pdev->ep_out[0].rem_length   = len;
	/* Start the transfer */
	USBD_LL_PrepareReceive(pdev, 0, pbuf, len);

	return USBD_OK;
}

/**
* @brief  USBD_CtlContinueRx
*         continue receive data on the ctl pipe
* @param  pdev: device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be received
* @retval status
*/
USBD_StatusTypeDef  USBD_CtlContinueRx(USBD_HandleTypeDef  *pdev,
                                          uint8_t *pbuf,
                                          uint16_t len)
{
	//PRINTF(PSTR("USBD_CtlContinueRx\n"));

	USBD_LL_PrepareReceive (pdev,
					  0,
					  pbuf,
					  len);
	return USBD_OK;
}
/**
* @brief  USBD_CtlSendStatus
*         send zero length packet on the ctl pipe
* @param  pdev: device instance
* @retval status
*/
USBD_StatusTypeDef  USBD_CtlSendStatus(USBD_HandleTypeDef  *pdev)
{
	//PRINTF(PSTR("USBD_CtlSendStatus\n"));
	/* Set EP0 State */
	pdev->ep0_state = USBD_EP0_STATUS_IN;

	/* Start the transfer */
	USBD_LL_Transmit(pdev, 0x00, NULL, 0);

	return USBD_OK;
}

/**
* @brief  USBD_CtlReceiveStatus
*         receive zero length packet on the ctl pipe
* @param  pdev: device instance
* @retval status
*/
USBD_StatusTypeDef  USBD_CtlReceiveStatus(USBD_HandleTypeDef  *pdev)
{
	USBD_SetupReqTypedef * const req = & pdev->request;
	//PRINTF(PSTR("USBD_CtlReceiveStatus: bmRequest=%04X, bRequest=%04X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
	//PRINTF(PSTR("USBD_CtlReceiveStatus\n"));

	/* Set EP0 State */
	pdev->ep0_state = USBD_EP0_STATUS_OUT;

	/* Start the transfer */
	USBD_LL_PrepareReceive( pdev, 0, NULL, 0);

	return USBD_OK;
}


/**
* @brief  USBD_CtlSendData
*         send data on the ctl pipe
* @param  pdev: device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be sent
* @retval status
*/
USBD_StatusTypeDef  USBD_CtlSendData(USBD_HandleTypeDef  *pdev,
                              const uint8_t *pbuf,
                               uint16_t len)
{
	/* Set EP0 State */
	pdev->ep0_state = USBD_EP0_DATA_IN;
	pdev->ep_in[0].total_length = len;
	pdev->ep_in[0].rem_length = len;
	/* Start the transfer */
	USBD_LL_Transmit(pdev, 0x00, pbuf, len);

	return USBD_OK;
}

/**
* @brief  USBD_CtlContinueSendData
*         continue sending data on the ctl pipe
* @param  pdev: device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be sent
* @retval status
*/
USBD_StatusTypeDef  USBD_CtlContinueSendData (USBD_HandleTypeDef  *pdev,
                                       const uint8_t *pbuf,
                                       uint16_t len)
{
	/* Start the next transfer */
	USBD_LL_Transmit(pdev, 0x00, pbuf, len);

	return USBD_OK;
}

/**
* @brief  USBD_CtlError
*         Handle USB low level Error
* @param  pdev: device instance
* @param  req: usb request
* @retval None
*/

void USBD_CtlError(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
#if 0
	PRINTF(PSTR("USBD_CtlError: bmRequest=%04X, bRequest=%04X, wValue=%04X, wIndex=%04X, wLength=%04X\n"),
		req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
#endif
	USBD_LL_StallEP(pdev, 0x80);
	USBD_LL_StallEP(pdev, 0);
}

/**
* @brief  USBD_StdItfReq
*         Handle standard usb interfacei requests
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
USBD_StatusTypeDef  USBD_StdItfReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef  *req)
{
	//PRINTF(PSTR("USBD_StdItfReq: bmRequest=%04X, bRequest=%04X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);

	USBD_StatusTypeDef ret = USBD_OK;

	switch (pdev->dev_state)
	{
	case USBD_STATE_CONFIGURED:
		{
			uint_fast8_t di;	// device function index
			for (di = 0; di < pdev->nClasses; ++ di)
			{
				ret = pdev->pClasses [di]->Setup(pdev, req);
				if (ret != USBD_OK)
				{
					TP();
					break;
				}
			}
			if ((req->wLength == 0) && (ret == USBD_OK))
			{
				USBD_CtlSendStatus(pdev);
			}
			else
			{
#if CPUSTYLE_R7S721
				USBD_LL_Transmit(pdev, 0x00, NULL, 0);
#endif
				//TP();
			}
		}
		break;

	default:
		TP();
		USBD_CtlError(pdev, req);
		break;
	}
	return USBD_OK;
}

/**
* @brief  USBD_StdEPReq
*         Handle standard usb endpoint requests
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
USBD_StatusTypeDef  USBD_StdEPReq(USBD_HandleTypeDef * pdev, USBD_SetupReqTypedef * req)
{
	//PRINTF(PSTR("USBD_StdEPReq: bmRequest=%04X, bRequest=%04X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
	uint8_t   ep_addr;
	USBD_StatusTypeDef ret = USBD_OK;
	USBD_EndpointTypeDef   *pep;
	ep_addr  = LO_BYTE(req->wIndex);

  /* Check if it is a class request */
  if ((req->bmRequest & 0x60) == 0x20)
  {
		uint_fast8_t di;	// device function index
		for (di = 0; di < pdev->nClasses; ++ di)
			pdev->pClasses [di]->Setup(pdev, req);

    return USBD_OK;
  }

  switch (req->bRequest)
  {
  case USB_REQ_SET_FEATURE :
 	//PRINTF(PSTR("USBD_StdEPReq: USB_REQ_SET_FEATURE: bmRequest=%04X, bRequest=%04X, wValue=%04X, wIndex=%04X, wLength=%04X\n"),
	//	req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);

    switch (pdev->dev_state)
    {
    case USBD_STATE_ADDRESSED:
      if ((ep_addr != 0x00) && (ep_addr != 0x80))
      {
        USBD_LL_StallEP(pdev, ep_addr);
      }
      break;

    case USBD_STATE_CONFIGURED:
      if (req->wValue == USB_FEATURE_EP_HALT)
      {
        if ((ep_addr != 0x00) && (ep_addr != 0x80))
        {
          USBD_LL_StallEP(pdev, ep_addr);

        }
      }
	uint_fast8_t di;	// device function index
	for (di = 0; di < pdev->nClasses; ++ di)
		pdev->pClasses [di]->Setup(pdev, req);
      USBD_CtlSendStatus(pdev);

      break;

    default:
	  TP();
      USBD_CtlError(pdev, req);
      break;
    }
    break;

  case USB_REQ_CLEAR_FEATURE :
 	//PRINTF(PSTR("USBD_StdEPReq: USB_REQ_CLEAR_FEATURE: bmRequest=%04X, bRequest=%04X, wValue=%04X, wIndex=%04X, wLength=%04X\n"),
	//	req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);

    switch (pdev->dev_state)
    {
    case USBD_STATE_ADDRESSED:
      if ((ep_addr != 0x00) && (ep_addr != 0x80))
      {
        USBD_LL_StallEP(pdev, ep_addr);
      }
      break;

    case USBD_STATE_CONFIGURED:
      if (req->wValue == USB_FEATURE_EP_HALT)
      {
        if ((ep_addr & 0x7F) != 0x00)
        {
          USBD_LL_ClearStallEP(pdev, ep_addr);
		uint_fast8_t di;	// device function index
		for (di = 0; di < pdev->nClasses; ++ di)
			pdev->pClasses [di]->Setup(pdev, req);
        }
        USBD_CtlSendStatus(pdev);
      }
      break;

    default:
		TP();
      USBD_CtlError(pdev, req);
      break;
    }
    break;

  case USB_REQ_GET_STATUS:
 	//PRINTF(PSTR("USBD_StdEPReq: USB_REQ_GET_STATUS: bmRequest=%04X, bRequest=%04X, wValue=%04X, wIndex=%04X, wLength=%04X\n"),
	//	req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
    switch (pdev->dev_state)
    {
    case USBD_STATE_ADDRESSED:
      if ((ep_addr & 0x7F) != 0x00)
      {
        USBD_LL_StallEP(pdev, ep_addr);
      }
      break;

    case USBD_STATE_CONFIGURED:
      pep = ((ep_addr & 0x80) != 0) ? &pdev->ep_in[ep_addr & 0x7F]: &pdev->ep_out[ep_addr & 0x7F];
      if(USBD_LL_IsStallEP(pdev, ep_addr))
      {
		pep->epstatus [0] = 0x01;
		pep->epstatus [1] = 0x00;
     }
      else
      {
        pep->epstatus [0] = 0x00;
        pep->epstatus [1] = 0x00;
      }

      USBD_CtlSendData (pdev, pep->epstatus, 2);
      break;

    default:
		TP();
      USBD_CtlError(pdev, req);
      break;
    }
    break;

  default:
	  TP();
	//PRINTF(PSTR("USBD_StdEPReq: ???? bmRequest=%04X, bRequest=%04X, wValue=%04X, wIndex=%04X, wLength=%04X\n"),
	//	req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
    break;
  }
  return ret;
}
/**
* @brief  USBD_GetDescriptor
*         Handle Get Descriptor requests
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_GetDescriptor(USBD_HandleTypeDef *pdev,
                               const USBD_SetupReqTypedef *req)
{
	uint16_t len;
	const uint8_t *pbuf;
	const uint_fast8_t index = LO_BYTE(req->wValue);

	//PRINTF(PSTR("USBD_GetDescriptor: %d, wLength=%04X (%d dec), ix=%u\n"), (int) HI_BYTE(req->wValue), req->wLength, req->wLength, LO_BYTE(req->wValue));

	switch (HI_BYTE(req->wValue))
	{
	case USB_DESC_TYPE_DEVICE:
		len = DeviceDescrTbl [0].size;
		pbuf = DeviceDescrTbl [0].data;
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (index < ARRAY_SIZE(ConfigDescrTbl) && ConfigDescrTbl [index].size != 0)
		{
			len = ConfigDescrTbl [index].size;
			pbuf = ConfigDescrTbl [index].data;
		}
		else
		{
			TP();
			USBD_CtlError(pdev, req);
			return;
		}
		break;

	case USB_DESC_TYPE_STRING:
		{
			const uint_fast16_t LangID = LO_BYTE(req->wIndex);
			switch (index)
			{
			case 0x65:
			case 0xF8:
				// Запрос появляется при запуске MixW2
				//len = StringDescrTbl [STRING_ID_7].size;
				//pbuf = StringDescrTbl [STRING_ID_7].data;
				//break;
				TP();
				USBD_CtlError(pdev, req);
				return;

			case 0xEE:
				// Microsoft OS String Descriptor, ReqLength=0x12
				if (MsftStringDescr [0].data != NULL && MsftStringDescr [0].size != 0)
				{
					//TP();
					len = MsftStringDescr [0].size;
					pbuf = MsftStringDescr [0].data;
				}
				else
				{
					//TP();
					USBD_CtlError(pdev, req);
					return;
				}
				break;

			default:
				if (index < usbd_get_stringsdesc_count() && StringDescrTbl [index].size != 0)
				{
					len = StringDescrTbl [index].size;
					pbuf = StringDescrTbl [index].data;
				}
				else
				{
					TP();
					PRINTF(PSTR("USBD_GetDescriptor: %02X\n"), HI_BYTE(req->wValue));
					USBD_CtlError(pdev, req);
					return;
				}
				break;
			} /* case */
		}
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		if (index < ARRAY_SIZE(DeviceQualifierTbl) && DeviceQualifierTbl [index].size != 0)
		{
			len = DeviceQualifierTbl [index].size;
			pbuf = DeviceQualifierTbl [index].data;
		}
		else
		{
			//TP();
			USBD_CtlError(pdev, req);
			return;
		}
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (pdev->dev_speed == USBD_SPEED_HIGH && index < ARRAY_SIZE(DeviceQualifierTbl) && DeviceQualifierTbl [index].size != 0)
		{
			len = OtherSpeedConfigurationTbl [index].size;
			pbuf = OtherSpeedConfigurationTbl [index].data;
		}
		else
		{
			TP();
			USBD_CtlError(pdev, req);
			return;
		}
		break;

	case USB_DESC_TYPE_BOS:
		if (BinaryDeviceObjectStoreTbl [0].size != 0)
		{
			len = BinaryDeviceObjectStoreTbl [0].size;
			pbuf = BinaryDeviceObjectStoreTbl [0].data;
		}
		else
		{
			TP();
			USBD_CtlError(pdev, req);
			return;
		}
		break;

	default:
		TP();
		USBD_CtlError(pdev, req);
		return;
	}

	if ((len != 0) && (req->wLength != 0))
	{
		//PRINTF(PSTR("USBD_GetDescriptor: %02X, wLength=%04X (%d dec), ix=%u, datalen=%u\n"), HI_BYTE(req->wValue), req->wLength, req->wLength, LO_BYTE(req->wValue), len);
		USBD_CtlSendData (pdev, pbuf, ulmin16(len, req->wLength));
	}

}

/**
* @brief  USBD_SetAddress
*         Set device address
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
// on renesas not exists
// HAL_PCD_IRQHandler trapped - DVSE, DVSQ
static void USBD_SetAddress(USBD_HandleTypeDef *pdev,
                            const USBD_SetupReqTypedef *req)
{
	//PRINTF(PSTR("USBD_SetAddress 0x%02X:\n"), LO_BYTE(req->wValue) & 0x7F);

	if ((req->wIndex == 0) && (req->wLength == 0))
	{

		if (pdev->dev_state == USBD_STATE_CONFIGURED)
		{
			TP();
			USBD_CtlError(pdev, req);
		}
		else
		{
			const uint_fast8_t dev_addr = LO_BYTE(req->wValue) & 0x7F;
			pdev->dev_address = dev_addr;
			USBD_LL_SetUSBAddress(pdev, dev_addr);
			USBD_CtlSendStatus(pdev);

			if (dev_addr != 0)
			{
				pdev->dev_state  = USBD_STATE_ADDRESSED;
			}
			else
			{
				pdev->dev_state  = USBD_STATE_DEFAULT;
			}
		}
	}
	else
	{
		TP();
		USBD_CtlError(pdev, req);
	}
}

#if CPUSTYLE_R7S721

// RENESAS specific function
static uint_fast8_t USB_GetAdress(USB_OTG_GlobalTypeDef * USBx)
{
	return (USBx->USBADDR & USB_USBADDR_USBADDR) >> USB_USBADDR_USBADDR_SHIFT;
}

// RENESAS specific function
void HAL_PCD_AdressedCallback(PCD_HandleTypeDef *hpcd)
{
	USBD_HandleTypeDef * const pdev = hpcd->pData;
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
	//PRINTF(PSTR("HAL_PCD_AdressedCallback\n"));

	if (pdev->dev_state == USBD_STATE_CONFIGURED)
	{
		TP();
		USBD_CtlError(pdev, & pdev->request);
	}
	else
	{
		const uint_fast8_t dev_addr = USB_GetAdress(USBx);
		pdev->dev_address = dev_addr;
		//USBD_LL_SetUSBAddress(pdev, dev_addr);
		//USBD_CtlSendStatus(pdev);

		if (dev_addr != 0)
		{
			pdev->dev_state = USBD_STATE_ADDRESSED;
		}
		else
		{
			pdev->dev_state = USBD_STATE_DEFAULT;
		}
	}

}

#endif /* CPUSTYLE_R7S721 */

/**
* @brief  USBD_SetConfig
*         Handle Set device configuration request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_SetConfig(USBD_HandleTypeDef *pdev,
                           const USBD_SetupReqTypedef *req)
{

	const uint_fast8_t  cfgidx = LO_BYTE(req->wValue);
	PRINTF(PSTR("USBD_SetConfig: cfgidx=%d, pdev->dev_state=%d\n"), cfgidx, pdev->dev_state);

	switch (pdev->dev_state)
	{
	case USBD_STATE_ADDRESSED:
		if (cfgidx)
		{
			pdev->dev_config [0] = cfgidx;
			pdev->dev_state = USBD_STATE_CONFIGURED;
			if(USBD_SetClassConfig(pdev, cfgidx) == USBD_FAIL)
			{
				TP();
				USBD_CtlError(pdev, req);
				return;
			}
			//TP();
			USBD_CtlSendStatus(pdev);
		}
		else
		{
			//TP();
			USBD_CtlSendStatus(pdev);
		}
		break;

	case USBD_STATE_CONFIGURED:
		if (cfgidx == 0)
		{
			pdev->dev_state = USBD_STATE_ADDRESSED;
			pdev->dev_config [0] = cfgidx;
			USBD_ClrClassConfig(pdev, cfgidx);
			USBD_CtlSendStatus(pdev);
		}
		else if (cfgidx != pdev->dev_config [0])
		{
			//TP();
			/* Clear old configuration */
			USBD_ClrClassConfig(pdev, pdev->dev_config [0]);

			/* set new configuration */
			pdev->dev_config [0] = cfgidx;
			if(USBD_SetClassConfig(pdev, cfgidx) == USBD_FAIL)
			{
				TP();
				USBD_CtlError(pdev, req);
				return;
			}
			USBD_CtlSendStatus(pdev);
			//TP();
		}
		else
		{
			// Set same configuration
			USBD_CtlSendStatus(pdev);
			//TP();
		}
		break;

	default:
		TP();
		USBD_CtlError(pdev, req);
		break;
	}
}

/**
* @brief  USBD_GetConfig
*         Handle Get device configuration request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_GetConfig(USBD_HandleTypeDef *pdev,
                           const USBD_SetupReqTypedef *req)
{
	PRINTF(PSTR("USBD_GetConfig:\n"));

	if (req->wLength != 1)
	{
		TP();
		USBD_CtlError(pdev, req);
	}
	else
	{
		switch (pdev->dev_state )
		{
		case USBD_STATE_ADDRESSED:
			pdev->dev_default_config [0] = 0;
			USBD_CtlSendData (pdev, pdev->dev_default_config, 1);
			break;

		case USBD_STATE_CONFIGURED:

			USBD_CtlSendData (pdev, pdev->dev_config, 1);
			break;

		default:
			TP();
			USBD_CtlError(pdev, req);
			break;
			}
	}
}

/**
* @brief  USBD_GetStatus
*         Handle Get Status request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_GetStatus(USBD_HandleTypeDef *pdev,
                           const USBD_SetupReqTypedef *req)
{

	//PRINTF(PSTR("USBD_GetStatus:\n"));
	switch (pdev->dev_state)
	{
	case USBD_STATE_ADDRESSED:
	case USBD_STATE_CONFIGURED:

		#if (USBD_SELF_POWERED == 1)
			pdev->dev_config_status [0] = USB_CONFIG_SELF_POWERED;
			pdev->dev_config_status [1] = 0;
		#else
			pdev->dev_config_status [0] = 0;
			pdev->dev_config_status [1] = 0;
		#endif

		if (pdev->dev_remote_wakeup)
		{
			pdev->dev_config_status [0] |= USB_CONFIG_REMOTE_WAKEUP;
		}

		USBD_CtlSendData (pdev, pdev->dev_config_status, 2);
		break;

	default :
		TP();
		USBD_CtlError(pdev, req);
		break;
	}
}


/**
* @brief  USBD_SetFeature
*         Handle Set device feature request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_SetFeature(USBD_HandleTypeDef *pdev,
                            const USBD_SetupReqTypedef *req)
{

	if (req->wValue == USB_FEATURE_REMOTE_WAKEUP)
	{
		pdev->dev_remote_wakeup = 1;
		uint_fast8_t di;	// device function index
		for (di = 0; di < pdev->nClasses; ++ di)
			pdev->pClasses [di]->Setup(pdev, req);
		//pdev->pClass->Setup (pdev, req);
		USBD_CtlSendStatus(pdev);
	}
}


/**
* @brief  USBD_ClrFeature
*         Handle clear device feature request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_ClrFeature(USBD_HandleTypeDef *pdev,
                            const USBD_SetupReqTypedef *req)
{
	switch (pdev->dev_state)
	{
	case USBD_STATE_ADDRESSED:
	case USBD_STATE_CONFIGURED:
		if (req->wValue == USB_FEATURE_REMOTE_WAKEUP)
		{
			pdev->dev_remote_wakeup = 0;
			uint_fast8_t di;	// device function index
			for (di = 0; di < pdev->nClasses; ++ di)
				pdev->pClasses [di]->Setup(pdev, req);
			//pdev->pClass->Setup (pdev, req);
			USBD_CtlSendStatus(pdev);
		}
		break;

	default :
		USBD_CtlError(pdev , req);
		break;
	}
}



/**
* @brief  USBD_StdDevReq
*         Handle standard usb device requests
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
USBD_StatusTypeDef  USBD_StdDevReq(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef  *req)
{
	//PRINTF(PSTR("USBD_StdDevReq: bmRequest=%04X, bRequest=%04X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
	//PRINTF(PSTR("USBD_StdDevReq: req->bRequest=%d\n"), (int) req->bRequest);
	USBD_StatusTypeDef ret = USBD_OK;

	switch (req->bRequest)
	{
	case USB_REQ_GET_DESCRIPTOR:
		USBD_GetDescriptor(pdev, req);
		break;

	case USB_REQ_SET_ADDRESS:
		USBD_SetAddress(pdev, req);
		break;

	case USB_REQ_SET_CONFIGURATION:
		USBD_SetConfig (pdev, req);
		break;

	case USB_REQ_GET_CONFIGURATION:
		USBD_GetConfig (pdev, req);
		break;

	case USB_REQ_GET_STATUS:
		USBD_GetStatus (pdev, req);
		break;

	case USB_REQ_SET_FEATURE:
		USBD_SetFeature (pdev, req);
		break;

	case USB_REQ_CLEAR_FEATURE:
		USBD_ClrFeature (pdev, req);
		break;

	default:
		TP();
		PRINTF(PSTR("USBD_StdDevReq: bmRequest=%04X, bRequest=%04X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
		USBD_CtlError(pdev, req);
		break;
	}

	return ret;
}
/**
* @brief  USBD_SetupStage
*         Handle the setup stage
* @param  pdev: device instance
* @retval status
*/
USBD_StatusTypeDef USBD_LL_SetupStage(USBD_HandleTypeDef *pdev, const uint32_t *psetup)
{
	//PRINTF("USBD_LL_SetupStage\n");
	USBD_ParseSetupRequest(& pdev->request, psetup);

	pdev->ep0_state = USBD_EP0_SETUP;
	pdev->ep0_data_len = pdev->request.wLength;

	switch (pdev->request.bmRequest & 0x1F)
	{
	case USB_REQ_RECIPIENT_DEVICE:
		USBD_StdDevReq(pdev, & pdev->request);
		break;

	case USB_REQ_RECIPIENT_INTERFACE:
		USBD_StdItfReq(pdev, & pdev->request);
		break;

	case USB_REQ_RECIPIENT_ENDPOINT:
		USBD_StdEPReq(pdev, & pdev->request);
		break;

	default:
		USBD_LL_StallEP(pdev, pdev->request.bmRequest & 0x80);
		break;
	}
	return USBD_OK;
}

/**
* @brief  USBD_DataOutStage
*         Handle data OUT stage
* @param  pdev: device instance
* @param  epnum: endpoint index
* @retval status
*/
USBD_StatusTypeDef USBD_LL_DataOutStage(USBD_HandleTypeDef *pdev, uint_fast8_t epnum, uint8_t *pdata)
{
	//PRINTF(PSTR("USBD_LL_DataOutStage:\n"));

	if (epnum == 0)
	{
		//PRINTF(PSTR("USBD_LL_DataOutStage: EP0 epnum=%d, pdev->ep0_state=%d\n"), (int) epnum, (int) pdev->ep0_state);

		if (pdev->ep0_state == USBD_EP0_DATA_OUT)
		{
			USBD_EndpointTypeDef * const pep = & pdev->ep_out [0];
			if (pep->rem_length > pep->maxpacket)
			{
				pep->rem_length -=  pep->maxpacket;

				USBD_CtlContinueRx(pdev, pdata, ulmin16(pep->rem_length, pep->maxpacket));
			}
			else
			{
				if ((pdev->dev_state == USBD_STATE_CONFIGURED))
				{
		        	  uint_fast8_t di;
		        	  for (di = 0; di < pdev->nClasses; ++ di)
		        	  {
		        		  /* for each device function */
		        		  const USBD_ClassTypeDef * const pClass = pdev->pClasses [di];
		              	if (pClass->EP0_RxReady != NULL)
		              		pClass->EP0_RxReady(pdev);
		        	  }
				}
				USBD_CtlSendStatus(pdev);
			}
		}
		else
		{
			//TP(); // появяляется на STM32 - в состоянии USBD_EP0_STATUS_OUT
		}
	}
	else if (pdev->dev_state == USBD_STATE_CONFIGURED)
	{
		  uint_fast8_t di;
		  for (di = 0; di < pdev->nClasses; ++ di)
		  {
			  /* for each device function */
			  const USBD_ClassTypeDef * const pClass = pdev->pClasses [di];
			  if (pClass->DataOut != NULL)
				  	 pClass->DataOut(pdev, epnum);
		  }
	}
	return USBD_OK;
}

/**
* @brief  USBD_DataInStage
*         Handle data in stage
* @param  pdev: device instance
* @param  epnum: endpoint index without direction bit
* @retval status
*/

USBD_StatusTypeDef USBD_LL_DataInStage(USBD_HandleTypeDef *pdev, uint_fast8_t epnum, uint8_t * pdata)
{
	//PRINTF(PSTR("USBD_LL_DataInStage:\n"));

	if (epnum == 0)
	{
		USBD_EndpointTypeDef * const pep = & pdev->ep_in [0];
		//PRINTF(PSTR("USBD_LL_DataInStage: IN: EP0: epnum=%02X\n"), epnum);

		if (pdev->ep0_state == USBD_EP0_DATA_IN)
		{
			if (pep->rem_length > pep->maxpacket)
			{
				pep->rem_length -=  pep->maxpacket;

				USBD_CtlContinueSendData (pdev,
									  pdata,
									  pep->rem_length);

				/* Prepare endpoint for premature end of transfer */
				USBD_LL_PrepareReceive (pdev,
									0,
									NULL,
									0);
			}
			else
			{ /* last packet is MPS multiple, so send ZLP packet */
				if ((pep->total_length % pep->maxpacket == 0) &&
						(pep->total_length >= pep->maxpacket) &&
						 (pep->total_length < pdev->ep0_data_len ))
				{

					USBD_CtlContinueSendData(pdev, NULL, 0);
					pdev->ep0_data_len = 0;

					/* Prepare endpoint for premature end of transfer */
					USBD_LL_PrepareReceive (pdev,
										0,
										NULL,
										0);
				}
				else
				{
					if (pdev->dev_state == USBD_STATE_CONFIGURED)
					{
						uint_fast8_t di;
						for (di = 0; di < pdev->nClasses; ++ di)
						{
							/* for each device function */
							const USBD_ClassTypeDef * const pClass = pdev->pClasses [di];
							if (pClass->EP0_TxSent != NULL)
								pClass->EP0_TxSent(pdev);
						}
					}
					USBD_CtlReceiveStatus(pdev);
				}
			}
		}

		if (pdev->dev_test_mode == 1)
		{
			USBD_RunTestMode(pdev);
			pdev->dev_test_mode = 0;
		}
	}
	else if (pdev->dev_state == USBD_STATE_CONFIGURED)
	{
		  uint_fast8_t di;
		  for (di = 0; di < pdev->nClasses; ++ di)
		  {
			  /* for each device function */
			  const USBD_ClassTypeDef * const pClass = pdev->pClasses [di];
			  if (pClass->DataIn != NULL)
				  pClass->DataIn(pdev, epnum);	// epnum without direction bit
		  }
	}
	return USBD_OK;
}

/**
* @brief  USBD_LL_Reset
*         Handle Reset event
* @param  pdev: device instance
* @retval status
*/

USBD_StatusTypeDef USBD_LL_Reset(USBD_HandleTypeDef  *pdev)
{
	/* Open EP0 OUT */
	USBD_LL_OpenEP(pdev, 0x00, USBD_EP_TYPE_CTRL, USB_OTG_MAX_EP0_SIZE);

	pdev->ep_out[0].maxpacket = USB_OTG_MAX_EP0_SIZE;

	/* Open EP0 IN */
	USBD_LL_OpenEP(pdev, 0x80, USBD_EP_TYPE_CTRL, USB_OTG_MAX_EP0_SIZE);

	pdev->ep_in[0].maxpacket = USB_OTG_MAX_EP0_SIZE;
	/* Upon Reset call user call back */
	pdev->dev_state = USBD_STATE_DEFAULT;

	uint_fast8_t di;
	for (di = 0; di < pdev->nClasses; ++ di)
	{
		/* for each device function */
		const USBD_ClassTypeDef * const pClass = pdev->pClasses [di];
		//if (pdev->pClassData)
		{
			pClass->DeInit(pdev, pdev->dev_config [0]);
			//pdev->pClassData = NULL; // TODO: make own data for each device function. Is not used now
		}
	}

	return USBD_OK;
}




/**
* @brief  USBD_LL_SetSpeed
*         Handle Reset event
* @param  pdev: device instance
* @retval status
*/
USBD_StatusTypeDef USBD_LL_SetSpeed(USBD_HandleTypeDef  *pdev, USBD_SpeedTypeDef speed)
{
	pdev->dev_speed = speed;	// USBD_SPEED_xxx
	return USBD_OK;
}

/**
* @brief  USBD_Suspend
*         Handle Suspend event
* @param  pdev: device instance
* @retval status
*/

USBD_StatusTypeDef USBD_LL_Suspend(USBD_HandleTypeDef  *pdev)
{
	pdev->dev_old_state = pdev->dev_state;
	pdev->dev_state  = USBD_STATE_SUSPENDED;
	return USBD_OK;
}

/**
* @brief  USBD_Resume
*         Handle Resume event
* @param  pdev: device instance
* @retval status
*/

USBD_StatusTypeDef USBD_LL_Resume(USBD_HandleTypeDef  *pdev)
{
	pdev->dev_state = pdev->dev_old_state;
	return USBD_OK;
}

/**
* @brief  USBD_SOF
*         Handle SOF event
* @param  pdev: device instance
* @retval status
*/

USBD_StatusTypeDef USBD_LL_SOF(USBD_HandleTypeDef  *pdev)
{
	if(pdev->dev_state == USBD_STATE_CONFIGURED)
	{
		uint_fast8_t di;
		for (di = 0; di < pdev->nClasses; ++ di)
		{
			/* for each device function */
			const USBD_ClassTypeDef * const pClass = pdev->pClasses [di];
			if(pClass->SOF != NULL)
			{
				pClass->SOF(pdev);
			}
		}
	}
	return USBD_OK;
}

/**
* @brief  USBD_IsoINIncomplete
*         Handle iso in incomplete event
* @param  pdev: device instance
* @retval status
*/
USBD_StatusTypeDef USBD_LL_IsoINIncomplete(USBD_HandleTypeDef  *pdev, uint_fast8_t epnum)
{
	ASSERT(epnum != 0);
	USBD_EndpointTypeDef * const pep = & pdev->ep_in [epnum];
	// epnum всегда 0
	//notseq  [epnum] = ! notseq  [epnum];
	//TP(); // постоянно проходим тут, если нет передачи звука из трансивера в компьютер
	return USBD_OK;
}

/**
* @brief  USBD_IsoOUTIncomplete
*         Handle iso out incomplete event
* @param  pdev: device instance
* @retval status
*/
USBD_StatusTypeDef USBD_LL_IsoOUTIncomplete(USBD_HandleTypeDef  *pdev, uint_fast8_t epnum)
{
	ASSERT(epnum != 0);
	USBD_EndpointTypeDef * const pep = & pdev->ep_out [epnum];
	// epnum всегда 0
	//notseq [epnum] = ! notseq  [epnum];
	//TP(); // постоянно проходим тут, если нет передачи звука из компьютера в трансивер
	return USBD_OK;
}

/**
* @brief  USBD_DevConnected
*         Handle device connection event
* @param  pdev: device instance
* @retval status
*/
USBD_StatusTypeDef USBD_LL_DevConnected(USBD_HandleTypeDef  *pdev)
{
	//PRINTF(PSTR("USBD_LL_DevConnected\n"));
	return USBD_OK;
}

/**
* @brief  USBD_DevDisconnected
*         Handle device disconnection event
* @param  pdev: device instance
* @retval status
*/
USBD_StatusTypeDef USBD_LL_DevDisconnected(USBD_HandleTypeDef  *pdev)
{
	//PRINTF(PSTR("USBD_LL_DevDisconnected\n"));
	/* Free Class Resources */
	pdev->dev_state = USBD_STATE_DEFAULT;

	  uint_fast8_t di;
	  for (di = 0; di < pdev->nClasses; ++ di)
	  {
		  /* for each device function */
		  const USBD_ClassTypeDef * const pClass = pdev->pClasses [di];
		  pClass->DeInit(pdev, pdev->dev_config [0]);
	  }

	return USBD_OK;
}

/**
  * @brief  USBD_AddClass
  *         Link class driver to Device Core.
  * @param  pDevice : Device Handle
  * @param  pclass: Class handle
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_AddClass(USBD_HandleTypeDef *pdev, const USBD_ClassTypeDef *pclass)
{
	USBD_StatusTypeDef   status = USBD_FAIL;
	if (pclass != NULL && pdev->nClasses < USBD_MAX_NUM_CLASSES)
	{
		/* link the class to the USB Device handle */
		pdev->pClasses [pdev->nClasses ++] = pclass;
		status = USBD_OK;

		/* Инициализации, которые необходимо выполнить до разрешения прерываний */
		if (pclass->ColdInit != NULL)
			pclass->ColdInit();
	}
	else
	{
		//USBD_ErrLog("Can not register device class %p", pclass);
		status = USBD_FAIL;
	}

	return status;
}

#if (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX)
/**
  * @brief  Check FIFO for the next packet to be loaded.
  * @param  hpcd: PCD handle
  * @param  epnum : endpoint number
  * @retval HAL status
  */
// вызывается только при работе без DMA
static HAL_StatusTypeDef PCD_WriteEmptyTxFifo(PCD_HandleTypeDef *hpcd, uint32_t epnum)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
	USB_OTG_EPTypeDef * const ep = & hpcd->IN_ep [epnum];
	int32_t len;
	uint32_t len32b;

	ASSERT(hpcd->Init.dma_enable == USB_DISABLE);

	len = ep->xfer_len - ep->xfer_count;
	ASSERT(len >= 0);
	if (len > ep->maxpacket)
		len = ep->maxpacket;
	len32b = (len + 3) / 4;
	int i = 0;
	// todo: разобраться, почему тут цикл. Это же обработчик прерывания
	while  (
		((USBx_INEP(epnum)->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) >> USB_OTG_DTXFSTS_INEPTFSAV_Pos) > len32b &&
		ep->xfer_count < ep->xfer_len &&
		ep->xfer_len != 0
		)
	{
		/* Write the FIFO */
		len = ep->xfer_len - ep->xfer_count;
		ASSERT(len >= 0);
		if (len > ep->maxpacket)
			len = ep->maxpacket;
		len32b = (len + 3) / 4;

		USB_WritePacket(USBx, ep->xfer_buff, ep->tx_fifo_num, len, hpcd->Init.dma_enable);

		ep->xfer_buff += len;
		ep->xfer_count += len;
		++ i;	// debug
	}

	if (len <= 0)
	{
		USBx_DEVICE->DIEPEMPMSK &= ~ (1uL << epnum);

	}
	ASSERT(i < 2);
	return HAL_OK;
}


#define USB_OTG_CORE_ID_310A          0x4F54310A
#define USB_OTG_CORE_ID_320A          0x4F54320A

/**
  * @brief  Handle PCD interrupt request.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
// F4, F7, H7...
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
{
	//	PRINTF(PSTR("HAL_PCD_IRQHandler:\n"));
	USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;

	/* ensure that we are in device mode */
	if (USB_GetMode(hpcd->Instance) == USB_OTG_MODE_DEVICE)
	{
		/* avoid spurious interrupt */
		if (__HAL_PCD_IS_INVALID_INTERRUPT(hpcd))
		{
			return;
		}

		if (__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_MMIS))
		{
			/* incorrect mode (device/host regidter access), acknowledge the interrupt */
			__HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_MMIS);
			TP();
		}

		/* OUT endpoints interrupts */
		if (__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_OEPINT))
		{
			/* Read in the device interrupt bits */
			uint_fast32_t ep_intr = USB_ReadDevAllOutEpInterrupt(hpcd->Instance);
			//PRINTF(PSTR("HAL_PCD_IRQHandler: USB_OTG_GINTSTS_OEPINT, ep_intr=%02X, DAINTMSK=%08lX\n"), ep_intr, USBx_DEVICE->DAINTMSK);

			uint_fast32_t epnum = 0;
			while (ep_intr)
			{
				if (ep_intr & 0x1)
				{
				  const uint32_t epint = USB_ReadDevOutEPInterrupt(hpcd->Instance, epnum);

				  if ((epint & USB_OTG_DOEPINT_XFRC) == USB_OTG_DOEPINT_XFRC)
				  {
					CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_XFRC);
		#if CPUSTYLE_STM32H7XX
					/* setup/out transaction management for Core ID >= 310A */
					if (USBx->GSNPSID >= USB_OTG_CORE_ID_310A)
					{
					  if(hpcd->Init.dma_enable == USB_ENABLE)
					  {
						if(USBx_OUTEP(0)->DOEPINT & (1 << 15))
						{
						  CLEAR_OUT_EP_INTR(epnum, (1 << 15));
						}
					  }
					}
		#endif /* CPUSTYLE_STM32H7XX */

					if (hpcd->Init.dma_enable == USB_ENABLE)
					{
					  hpcd->OUT_ep [epnum].xfer_count = hpcd->OUT_ep [epnum].maxpacket - ((USBx_OUTEP(epnum)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ) >> USB_OTG_DOEPTSIZ_XFRSIZ_Pos);
					  hpcd->OUT_ep[ epnum].xfer_buff += hpcd->OUT_ep [epnum].maxpacket;
					}

					HAL_PCD_DataOutStageCallback(hpcd, epnum);
					if (hpcd->Init.dma_enable == USB_ENABLE)
					{
					  if ((epnum == 0) && (hpcd->OUT_ep [epnum].xfer_len == 0))
					  {
						 /* this is ZLP, so prepare EP0 for next setup */
						USB_EP0_OutStart(hpcd->Instance, USB_ENABLE, (uint8_t *) hpcd->PSetup);
					  }
					}
				  }

				  if ((epint & USB_OTG_DOEPINT_STUP) == USB_OTG_DOEPINT_STUP)
				  {
		#if CPUSTYLE_STM32H7XX
					/* setup/out transaction management for Core ID >= 310A */
					if (USBx->GSNPSID >= USB_OTG_CORE_ID_310A)
					{
					  if(hpcd->Init.dma_enable == USB_ENABLE)
					  {
						if(USBx_OUTEP(0)->DOEPINT & (1 << 15))
						{
						  CLEAR_OUT_EP_INTR(epnum, (1 << 15));
						}
					  }
					}
		#endif /* CPUSTYLE_STM32H7XX */

					/* Inform the upper layer that a setup packet is available */
					HAL_PCD_SetupStageCallback(hpcd);
					CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_STUP);
				  }

			  if ((epint & USB_OTG_DOEPINT_OTEPDIS) == USB_OTG_DOEPINT_OTEPDIS)
			  {
				CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_OTEPDIS);
			  }
	#ifdef USB_OTG_DOEPINT_OTEPSPR
			  /* Clear Status Phase Received interrupt */
			  if ((epint & USB_OTG_DOEPINT_OTEPSPR) == USB_OTG_DOEPINT_OTEPSPR)
			  {
				CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_OTEPSPR);
			  }
	#endif /* USB_OTG_DOEPINT_OTEPSPR */
			}
			epnum ++;
			ep_intr >>= 1;
		  }
		}

		/* IN endpoints interrupts */
		if (__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_IEPINT))
		{
		  /* Read in the device interrupt bits */
		  uint_fast32_t ep_intr = USB_ReadDevAllInEpInterrupt(hpcd->Instance);

		  //PRINTF(PSTR("HAL_PCD_IRQHandler: USB_OTG_GINTSTS_IEPINT, ep_intr=%02X, DAINTMSK=%08lX\n"), ep_intr, USBx_DEVICE->DAINTMSK);
		  uint_fast32_t epnum = 0;

		  while (ep_intr)
		  {
			if (ep_intr & 0x1) /* In ITR */
			{
			  const uint32_t epint = USB_ReadDevInEPInterrupt(hpcd->Instance, epnum);
			  USB_OTG_EPTypeDef * const inep = & hpcd->IN_ep [epnum];

			  if ((epint & USB_OTG_DIEPINT_XFRC) == USB_OTG_DIEPINT_XFRC)
			  {
				USBx_DEVICE->DIEPEMPMSK &= ~ (0x1uL << epnum);

				CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_XFRC);

				if (hpcd->Init.dma_enable == USB_ENABLE)
				{
				  inep->xfer_buff += inep->maxpacket; // пересланный размер может отличаться от максимального
				  //inep->xfer_buff += inep->xfer_len; // может быть, так?
				}

				HAL_PCD_DataInStageCallback(hpcd, inep->num);

				if (hpcd->Init.dma_enable == USB_ENABLE)
				{
				  /* this is ZLP, so prepare EP0 for next setup */
				  if ((epnum == 0) && (inep->xfer_len == 0))
				  {
					/* prepare to rx more setup packets */
					USB_EP0_OutStart(hpcd->Instance, USB_ENABLE, (uint8_t *) hpcd->PSetup);
				  }
				}
			  }

			  if ((epint & USB_OTG_DIEPINT_TOC) == USB_OTG_DIEPINT_TOC)
			  {
				CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_TOC);
			  }
			  if ((epint & USB_OTG_DIEPINT_ITTXFE) == USB_OTG_DIEPINT_ITTXFE)
			  {
				CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_ITTXFE);
			  }
			  if ((epint & USB_OTG_DIEPINT_INEPNE) == USB_OTG_DIEPINT_INEPNE)
			  {
				CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_INEPNE);
			  }
			  if ((epint & USB_OTG_DIEPINT_EPDISD) == USB_OTG_DIEPINT_EPDISD)
			  {
				CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_EPDISD);
			  }
			  if ((epint & USB_OTG_DIEPINT_TXFE) == USB_OTG_DIEPINT_TXFE)
			  {
				  // see (USBx->GAHBCFG & USB_OTG_GAHBCFG_TXFELVL)
				PCD_WriteEmptyTxFifo(hpcd, epnum);	// вызывается только при работе без DMA
			  }
			}
			epnum ++;
			ep_intr >>= 1;
		  }
		}

	/* Handle Resume Interrupt */
	if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_WKUINT))
	{
		/* Clear the Remote Wake-up Signaling */
		USBx_DEVICE->DCTL &= ~ USB_OTG_DCTL_RWUSIG;

#ifdef USB_OTG_GLPMCFG_LPMEN
		if(hpcd->LPM_State == LPM_L1)
		{
			hpcd->LPM_State = LPM_L0;
			HAL_PCDEx_LPM_Callback(hpcd, PCD_LPM_L0_ACTIVE);
		}
		else
#endif /* USB_OTG_GLPMCFG_LPMEN */
		{
			HAL_PCD_ResumeCallback(hpcd);
		}
		__HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_WKUINT);
	}

	/* Handle Suspend Interrupt */
	if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_USBSUSP))
	{
		if ((USBx_DEVICE->DSTS & USB_OTG_DSTS_SUSPSTS) == USB_OTG_DSTS_SUSPSTS)
		{

			HAL_PCD_SuspendCallback(hpcd);
		}
	__HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_USBSUSP);
	}

#ifdef USB_OTG_GLPMCFG_LPMEN
	/* Handle LPM Interrupt */
	if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_LPMINT))
	{
		__HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_LPMINT);
		if( hpcd->LPM_State == LPM_L0)
		{
			hpcd->LPM_State = LPM_L1;
			hpcd->BESL = (hpcd->Instance->GLPMCFG & USB_OTG_GLPMCFG_BESL) / MASK2LSB(USB_OTG_GLPMCFG_BESL);
			HAL_PCDEx_LPM_Callback(hpcd, PCD_LPM_L1_ACTIVE);
		}
		else
		{
			HAL_PCD_SuspendCallback(hpcd);
		}
	}
#endif /* USB_OTG_GLPMCFG_LPMEN */

    /* Handle Reset Interrupt */
    if (__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_USBRST))
    {
	  uint32_t i;
      USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;
      USB_FlushTxFifo(hpcd->Instance,  0);/* flush control endpoint tx FIFO */

      for (i = 0; i < hpcd->Init.dev_endpoints ; ++ i)
      {
        USBx_INEP(i)->DIEPINT = 0xFF;
        USBx_INEP(i)->DIEPCTL &= ~USB_OTG_DIEPCTL_STALL;
        USBx_OUTEP(i)->DOEPINT = 0xFF;
        USBx_OUTEP(i)->DOEPCTL &= ~USB_OTG_DOEPCTL_STALL;
      }
      USBx_DEVICE->DAINT = 0xFFFFFFFF;
      USBx_DEVICE->DAINTMSK |= (1uL << USB_OTG_DAINTMSK_IEPM_Pos) | (1uL << USB_OTG_DAINTMSK_OEPM_Pos);

      if (hpcd->Init.use_dedicated_ep1 == USB_ENABLE)
      {
		//#ifdef USB_OTG_DOEPINT_OTEPSPR
		  //USBx_DEVICE->DOUTEP1MSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM | USB_OTG_DOEPMSK_OTEPSPRM);
 		//#else
		  USBx_DEVICE->DOUTEP1MSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM);
 		//#endif
        USBx_DEVICE->DINEP1MSK |= (USB_OTG_DIEPMSK_TOM | USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_EPDM);
      }

	#ifdef USB_OTG_DOEPMSK_OTEPSPRM
		USBx_DEVICE->DOEPMSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM | USB_OTG_DOEPMSK_OTEPSPRM);
	#else
		USBx_DEVICE->DOEPMSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM);
	#endif

    USBx_DEVICE->DIEPMSK |= (USB_OTG_DIEPMSK_TOM | USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_EPDM);

      /* Set Default Address to 0 */
      USBx_DEVICE->DCFG &= ~USB_OTG_DCFG_DAD;

      /* setup EP0 to receive SETUP packets */
      USB_EP0_OutStart(hpcd->Instance, hpcd->Init.dma_enable, (uint8_t *)hpcd->PSetup);

      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_USBRST);
    }

    /* Handle Enumeration done Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_ENUMDNE))
    {
      USB_ActivateSetup(hpcd->Instance);
      //hpcd->Instance->GUSBCFG &= ~USB_OTG_GUSBCFG_TRDT;

      if (USB_GetDevSpeed(hpcd->Instance) == USB_OTG_SPEED_HIGH)
      {
        hpcd->Init.speed            = PCD_SPEED_HIGH;
        hpcd->Init.ep0_mps          = USB_OTG_HS_MAX_PACKET_SIZE;
        hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) |
			(uint32_t)((USBD_HS_TRDT_VALUE << USB_OTG_GUSBCFG_TRDT_Pos) & USB_OTG_GUSBCFG_TRDT) |
			0;
      }
      else
      {
        hpcd->Init.speed            = PCD_SPEED_FULL;
        hpcd->Init.ep0_mps          = USB_OTG_FS_MAX_PACKET_SIZE ;

        /* The USBTRD is configured according to the tables below, depending on AHB frequency
        used by application. In the low AHB frequency range it is used to stretch enough the USB response
        time to IN tokens, the USB turnaround time, so to compensate for the longer AHB read access
        latency to the Data FIFO */

		if (0)
		{
          hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) | (uint32_t)((0xF * USB_OTG_GUSBCFG_TRDT_0) & USB_OTG_GUSBCFG_TRDT);
		}
		else
		{
		uint32_t hclk;
        /* Get hclk frequency value */
        hclk = CPU_FREQ; //HAL_RCC_GetHCLKFreq();

		if ((hclk >= 14200000)&&(hclk < 15000000))
        {
          /* hclk Clock Range between 14.2-15 MHz */
          hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) | (uint32_t)((0xF * USB_OTG_GUSBCFG_TRDT_0) & USB_OTG_GUSBCFG_TRDT);
        }

        else if ((hclk >= 15000000)&&(hclk < 16000000))
        {
          /* hclk Clock Range between 15-16 MHz */
          hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) | (uint32_t)((0xE * USB_OTG_GUSBCFG_TRDT_0) & USB_OTG_GUSBCFG_TRDT);
        }

        else if ((hclk >= 16000000)&&(hclk < 17200000))
        {
          /* hclk Clock Range between 16-17.2 MHz */
          hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) | (uint32_t)((0xD * USB_OTG_GUSBCFG_TRDT_0) & USB_OTG_GUSBCFG_TRDT);
        }

        else if ((hclk >= 17200000)&&(hclk < 18500000))
        {
          /* hclk Clock Range between 17.2-18.5 MHz */
          hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) | (uint32_t)((0xC * USB_OTG_GUSBCFG_TRDT_0) & USB_OTG_GUSBCFG_TRDT);
        }

        else if ((hclk >= 18500000)&&(hclk < 20000000))
        {
          /* hclk Clock Range between 18.5-20 MHz */
          hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) | (uint32_t)((0xB * USB_OTG_GUSBCFG_TRDT_0) & USB_OTG_GUSBCFG_TRDT);
        }

        else if ((hclk >= 20000000)&&(hclk < 21800000))
        {
          /* hclk Clock Range between 20-21.8 MHz */
          hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) | (uint32_t)((0xA * USB_OTG_GUSBCFG_TRDT_0) & USB_OTG_GUSBCFG_TRDT);
        }

        else if ((hclk >= 21800000)&&(hclk < 24000000))
        {
          /* hclk Clock Range between 21.8-24 MHz */
          hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) | (uint32_t)((0x9 * USB_OTG_GUSBCFG_TRDT_0) & USB_OTG_GUSBCFG_TRDT);
        }

        else if ((hclk >= 24000000)&&(hclk < 27700000))
        {
          /* hclk Clock Range between 24-27.7 MHz */
          hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) | (uint32_t)((0x8 * USB_OTG_GUSBCFG_TRDT_0) & USB_OTG_GUSBCFG_TRDT);
        }

        else if ((hclk >= 27700000)&&(hclk < 32000000))
        {
          /* hclk Clock Range between 27.7-32 MHz */
          hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) | (uint32_t)((0x7 * USB_OTG_GUSBCFG_TRDT_0) & USB_OTG_GUSBCFG_TRDT);
        }

        else /* if(hclk >= 32000000) */
        {
          /* hclk Clock Range between 32-200 MHz */
          hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) | (uint32_t)((0x6 * USB_OTG_GUSBCFG_TRDT_0) & USB_OTG_GUSBCFG_TRDT);
        }
		}
      }

      HAL_PCD_ResetCallback(hpcd);

      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_ENUMDNE);
    }

    /* Handle RxQLevel Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_RXFLVL))
    {
      //USB_MASK_INTERRUPT(hpcd->Instance, USB_OTG_GINTSTS_RXFLVL); //mgs:????
      const uint_fast32_t grxstsp = USBx->GRXSTSP;
      USB_OTG_EPTypeDef * const ep = & hpcd->OUT_ep [grxstsp & USB_OTG_GRXSTSP_EPNUM];

      if (((grxstsp & USB_OTG_GRXSTSP_PKTSTS) >> USB_OTG_GRXSTSP_PKTSTS_Pos) ==  STS_DATA_UPDT)
      {
		const unsigned bcnt = (grxstsp & USB_OTG_GRXSTSP_BCNT) >> USB_OTG_GRXSTSP_BCNT_Pos;
        if (bcnt != 0)
        {
          USB_ReadPacket(USBx, ep->xfer_buff, bcnt);
          ep->xfer_buff += bcnt;
          ep->xfer_count += bcnt;
        }
      }
      else if (((grxstsp & USB_OTG_GRXSTSP_PKTSTS) >> USB_OTG_GRXSTSP_PKTSTS_Pos) ==  STS_SETUP_UPDT)
      {
		const unsigned bcnt = (grxstsp & USB_OTG_GRXSTSP_BCNT) >> USB_OTG_GRXSTSP_BCNT_Pos;
		ASSERT(bcnt == 8);
		ASSERT((grxstsp & USB_OTG_GRXSTSP_EPNUM) == 0);
        USB_ReadPacket(USBx, (uint8_t *) hpcd->PSetup, 8);
        ep->xfer_count += bcnt;
      }
      //USB_UNMASK_INTERRUPT(hpcd->Instance, USB_OTG_GINTSTS_RXFLVL); //mgs:????
    }

    /* Handle SOF Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_SOF))
    {
      HAL_PCD_SOFCallback(hpcd);
      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_SOF);
    }

    /* Handle Incomplete ISO IN Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_IISOIXFR))
    {
#if WITHUSBUAC
	// device only: Incomplete isochronous IN transfer
    	uint8_t epnum = USBD_EP_AUDIO_IN;	// TODO: use right value - now ignored
      HAL_PCD_ISOINIncompleteCallback(hpcd, epnum);
#endif /* WITHUSBUAC */
      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_IISOIXFR);
    }

    /* Handle Incomplete ISO OUT Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT))
    {
#if WITHUSBUAC
	// device: INCOMPISOOUT: Incomplete isochronous OUT transfer
	// host: IPXFR: Incomplete periodic transfer
    	uint8_t epnum = USBD_EP_AUDIO_OUT;	// TODO: use right value - now ignored
      HAL_PCD_ISOOUTIncompleteCallback(hpcd, epnum);
#endif /* WITHUSBUAC */
      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT);
    }

    /* Handle Connection event Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_SRQINT))
    {
      HAL_PCD_ConnectCallback(hpcd);
      __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_SRQINT);
    }

    /* Handle Disconnection event Interrupt */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_OTGINT))
    {
	  uint32_t temp = hpcd->Instance->GOTGINT;

      if ((temp & USB_OTG_GOTGINT_SEDET) == USB_OTG_GOTGINT_SEDET)
      {
        HAL_PCD_DisconnectCallback(hpcd);
      }
      hpcd->Instance->GOTGINT |= temp;
    }

    if(__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_BOUTNAKEFF))
    {
		PRINTF(PSTR("Global_NAK effective\n"));

		//USBx_DEVICE->DCTL = USB_OTG_DCTL_CGONAK;
    }

  }
}

void device_OTG_HS_IRQHandler(void)
{
#if defined (WITHUSBHW_DEVICE)
	HAL_PCD_IRQHandler(& hpcd_USB_OTG);
#endif /* defined (WITHUSBHW_DEVICE) */
}

void host_OTG_HS_IRQHandler(void)
{
#if defined (WITHUSBHW_HOST)
	HAL_HCD_IRQHandler(& hhcd_USB_OTG);
#endif /* defined (WITHUSBHW_HOST) */
}

/**
* @brief This function handles USB On The Go HS End Point 1 Out global interrupt.
*/
void device_OTG_HS_EP1_OUT_IRQHandler(void)
{
#if defined (WITHUSBHW_DEVICE)
	HAL_PCD_IRQHandler(& hpcd_USB_OTG);
#endif /* defined (WITHUSBHW_DEVICE) */
}

void host_OTG_HS_EP1_OUT_IRQHandler(void)
{
#if defined (WITHUSBHW_HOST)
	HAL_HCD_IRQHandler(& hhcd_USB_OTG);
#endif /* defined (WITHUSBHW_HOST) */
}

/**
* @brief This function handles USB On The Go HS End Point 1 In global interrupt.
*/
void device_OTG_HS_EP1_IN_IRQHandler(void)
{
#if defined (WITHUSBHW_DEVICE)
	HAL_PCD_IRQHandler(& hpcd_USB_OTG);
#endif /* defined (WITHUSBHW_DEVICE) */
}

void host_OTG_HS_EP1_IN_IRQHandler(void)
{
#if defined (WITHUSBHW_HOST)
	HAL_HCD_IRQHandler(& hhcd_USB_OTG);
#endif /* defined (WITHUSBHW_HOST) */
}

void device_OTG_FS_IRQHandler(void)
{
#if defined (WITHUSBHW_DEVICE)
	HAL_PCD_IRQHandler(& hpcd_USB_OTG);
#endif /* defined (WITHUSBHW_DEVICE) */
}

void host_OTG_FS_IRQHandler(void)
{
#if defined (WITHUSBHW_HOST)
	HAL_HCD_IRQHandler(& hhcd_USB_OTG);
#endif /* defined (WITHUSBHW_HOST) */
}


#elif CPUSTYLE_STM32F1XX

/**
  * @brief  This function handles PCD Endpoint interrupt request.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
static HAL_StatusTypeDef PCD_EP_ISR_Handler(PCD_HandleTypeDef *hpcd)
{
  PCD_EPTypeDef *ep = NULL;
  uint16_t count = 0;
  uint8_t epindex = 0;
  uint16_t wIstr = 0;
  uint16_t wEPVal = 0;

  /* stay in loop while pending interrupts */
  while (((wIstr = hpcd->Instance->ISTR) & USB_ISTR_CTR) != 0)
  {
    /* extract highest priority endpoint number */
    epindex = (uint8_t)(wIstr & USB_ISTR_EP_ID);

    if (epindex == 0)
    {
      /* Decode and service control endpoint interrupt */

      /* DIR bit = origin of the interrupt */
      if ((wIstr & USB_ISTR_DIR) == 0)
      {
        /* DIR = 0 */

        /* DIR = 0      => IN  int */
        /* DIR = 0 implies that (EP_CTR_TX = 1) always  */
        PCD_CLEAR_TX_EP_CTR(hpcd->Instance, PCD_ENDP0);
        ep = &hpcd->IN_ep[0];

        ep->xfer_count = PCD_GET_EP_TX_CNT(hpcd->Instance, ep->num);
        ep->xfer_buff += ep->xfer_count;

        /* TX COMPLETE */
        HAL_PCD_DataInStageCallback(hpcd, 0);


        if ((hpcd->USB_Address > 0)&& ( ep->xfer_len == 0))
        {
          hpcd->Instance->DADDR = (hpcd->USB_Address | USB_DADDR_EF);
          hpcd->USB_Address = 0;
        }

      }
      else
      {
        /* DIR = 1 */

        /* DIR = 1 & CTR_RX       => SETUP or OUT int */
        /* DIR = 1 & (CTR_TX | CTR_RX) => 2 int pending */
        ep = &hpcd->OUT_ep[0];
        wEPVal = PCD_GET_ENDPOINT(hpcd->Instance, PCD_ENDP0);

        if ((wEPVal & USB_EP_SETUP) != 0)
        {
          /* Get SETUP Packet*/
          ep->xfer_count = PCD_GET_EP_RX_CNT(hpcd->Instance, ep->num);
          USB_ReadPMA(hpcd->Instance, (uint8_t*)hpcd->Setup ,ep->pmaadress , ep->xfer_count);
          /* SETUP bit kept frozen while CTR_RX = 1*/
          PCD_CLEAR_RX_EP_CTR(hpcd->Instance, PCD_ENDP0);

          /* Process SETUP Packet*/
          HAL_PCD_SetupStageCallback(hpcd);
        }

        else if ((wEPVal & USB_EP_CTR_RX) != 0)
        {
          PCD_CLEAR_RX_EP_CTR(hpcd->Instance, PCD_ENDP0);
          /* Get Control Data OUT Packet*/
          ep->xfer_count = PCD_GET_EP_RX_CNT(hpcd->Instance, ep->num);

          if (ep->xfer_count != 0)
          {
            USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaadress, ep->xfer_count);
            ep->xfer_buff+=ep->xfer_count;
          }

          /* Process Control Data OUT Packet*/
           HAL_PCD_DataOutStageCallback(hpcd, 0);

          PCD_SET_EP_RX_CNT(hpcd->Instance, PCD_ENDP0, ep->maxpacket);
          PCD_SET_EP_RX_STATUS(hpcd->Instance, PCD_ENDP0, USB_EP_RX_VALID);
        }
      }
    }
    else
    {
      /* Decode and service non control endpoints interrupt  */

      /* process related endpoint register */
      wEPVal = PCD_GET_ENDPOINT(hpcd->Instance, epindex);
      if ((wEPVal & USB_EP_CTR_RX) != 0)
      {
        /* clear int flag */
        PCD_CLEAR_RX_EP_CTR(hpcd->Instance, epindex);
        ep = &hpcd->OUT_ep[epindex];

        /* OUT double Buffering*/
        if (ep->doublebuffer == 0)
        {
          count = PCD_GET_EP_RX_CNT(hpcd->Instance, ep->num);
          if (count != 0)
          {
            USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaadress, count);
          }
        }
        else
        {
          if (PCD_GET_ENDPOINT(hpcd->Instance, ep->num) & USB_EP_DTOG_RX)
          {
            /*read from endpoint BUF0Addr buffer*/
            count = PCD_GET_EP_DBUF0_CNT(hpcd->Instance, ep->num);
            if (count != 0)
            {
              USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr0, count);
            }
          }
          else
          {
            /*read from endpoint BUF1Addr buffer*/
            count = PCD_GET_EP_DBUF1_CNT(hpcd->Instance, ep->num);
            if (count != 0)
            {
              USB_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr1, count);
            }
          }
          PCD_FreeUserBuffer(hpcd->Instance, ep->num, PCD_EP_DBUF_OUT);
        }
        /*multi-packet on the NON control OUT endpoint*/
        ep->xfer_count+=count;
        ep->xfer_buff+=count;

        if ((ep->xfer_len == 0) || (count < ep->maxpacket))
        {
          /* RX COMPLETE */
          HAL_PCD_DataOutStageCallback(hpcd, ep->num);
        }
        else
        {
          HAL_PCD_EP_Receive(hpcd, ep->num, ep->xfer_buff, ep->xfer_len);
        }

      } /* if ((wEPVal & EP_CTR_RX) */

      if ((wEPVal & USB_EP_CTR_TX) != 0)
      {
        ep = &hpcd->IN_ep[epindex];

        /* clear int flag */
        PCD_CLEAR_TX_EP_CTR(hpcd->Instance, epindex);

        /* IN double Buffering*/
        if (ep->doublebuffer == 0)
        {
          ep->xfer_count = PCD_GET_EP_TX_CNT(hpcd->Instance, ep->num);
          if (ep->xfer_count != 0)
          {
            USB_WritePMA(hpcd->Instance, ep->xfer_buff, ep->pmaadress, ep->xfer_count);
          }
        }
        else
        {
          if (PCD_GET_ENDPOINT(hpcd->Instance, ep->num) & USB_EP_DTOG_TX)
          {
            /*read from endpoint BUF0Addr buffer*/
            ep->xfer_count = PCD_GET_EP_DBUF0_CNT(hpcd->Instance, ep->num);
            if (ep->xfer_count != 0)
            {
              USB_WritePMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr0, ep->xfer_count);
            }
          }
          else
          {
            /*read from endpoint BUF1Addr buffer*/
            ep->xfer_count = PCD_GET_EP_DBUF1_CNT(hpcd->Instance, ep->num);
            if (ep->xfer_count != 0)
            {
              USB_WritePMA(hpcd->Instance, ep->xfer_buff, ep->pmaaddr1, ep->xfer_count);
            }
          }
          PCD_FreeUserBuffer(hpcd->Instance, ep->num, PCD_EP_DBUF_IN);
        }
        /*multi-packet on the NON control IN endpoint*/
        ep->xfer_count = PCD_GET_EP_TX_CNT(hpcd->Instance, ep->num);
        ep->xfer_buff+=ep->xfer_count;

        /* Zero Length Packet? */
        if (ep->xfer_len == 0)
        {
          /* TX COMPLETE */
          HAL_PCD_DataInStageCallback(hpcd, ep->num);
        }
        else
        {
          HAL_PCD_EP_Transmit(hpcd, ep->num, ep->xfer_buff, ep->xfer_len);
        }
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  This function handles PCD interrupt request.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
// STM32F1xx
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
{
  uint32_t wInterrupt_Mask = 0;

  if (__HAL_PCD_GET_FLAG (hpcd, USB_ISTR_CTR))
  {
    /* servicing of the endpoint correct transfer interrupt */
    /* clear of the CTR flag into the sub */
    PCD_EP_ISR_Handler(hpcd);
  }

  if (__HAL_PCD_GET_FLAG (hpcd, USB_ISTR_RESET))
  {
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_RESET);
    HAL_PCD_ResetCallback(hpcd);
    HAL_PCD_SetAddress(hpcd, 0);
  }

  if (__HAL_PCD_GET_FLAG (hpcd, USB_ISTR_PMAOVR))
  {
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_PMAOVR);
  }
  if (__HAL_PCD_GET_FLAG (hpcd, USB_ISTR_ERR))
  {
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_ERR);
  }

  if (__HAL_PCD_GET_FLAG (hpcd, USB_ISTR_WKUP))
  {
    hpcd->Instance->CNTR &= ~(USB_CNTR_LP_MODE);

    /*set wInterrupt_Mask global variable*/
    wInterrupt_Mask = USB_CNTR_CTRM  | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_ERRM | USB_CNTR_ESOFM | USB_CNTR_RESETM;

    /*Set interrupt mask*/
    hpcd->Instance->CNTR = wInterrupt_Mask;

    HAL_PCD_ResumeCallback(hpcd);

    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_WKUP);
  }

  if (__HAL_PCD_GET_FLAG (hpcd, USB_ISTR_SUSP))
  {
    /* clear of the ISTR bit must be done after setting of CNTR_FSUSP */
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_SUSP);

    /* Force low-power mode in the macrocell */
    hpcd->Instance->CNTR |= USB_CNTR_FSUSP;
    hpcd->Instance->CNTR |= USB_CNTR_LP_MODE;
    if (__HAL_PCD_GET_FLAG (hpcd, USB_ISTR_WKUP) == 0)
    {
      HAL_PCD_SuspendCallback(hpcd);
    }
  }

  if (__HAL_PCD_GET_FLAG (hpcd, USB_ISTR_SOF))
  {
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_SOF);
    HAL_PCD_SOFCallback(hpcd);
  }

  if (__HAL_PCD_GET_FLAG (hpcd, USB_ISTR_ESOF))
  {
    /* clear ESOF flag in ISTR */
    __HAL_PCD_CLEAR_FLAG(hpcd, USB_ISTR_ESOF);
  }
}


/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles USB low priority or CAN RX0 interrupts.
*/
// F1
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  HAL_PCD_IRQHandler(& hpcd_USB_OTG);
}

#endif /* CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX */

/** @defgroup USBH_CTLREQ_Exported_Defines
  * @{
  */
/*Standard Feature Selector for clear feature command*/
#define FEATURE_SELECTOR_ENDPOINT         0X00
#define FEATURE_SELECTOR_DEVICE           0X01


#define INTERFACE_DESC_TYPE               0x04
#define ENDPOINT_DESC_TYPE                0x05
#define INTERFACE_DESC_SIZE               0x09

#define ValBit(VAR,POS)                               (VAR & (1 << POS))
#define SetBit(VAR,POS)                               (VAR |= (1 << POS))
#define ClrBit(VAR,POS)                               (VAR &= ((1 << POS)^255))

#define  LE16(addr)             (((uint16_t)(*((uint8_t *)(addr))))\
                                + (((uint16_t)(*(((uint8_t *)(addr)) + 1))) << 8))

#define  LE16S(addr)              (uint16_t)(LE16((addr)))

#define  LE32(addr)              ((((uint32_t)(*(((uint8_t *)(addr)) + 0))) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 1))) << 8) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 2))) << 16) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 3))) << 24)))

#define  LE64(addr)              ((((uint64_t)(*(((uint8_t *)(addr)) + 0))) + \
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 1))) <<  8) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 2))) << 16) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 3))) << 24) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 4))) << 32) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 5))) << 40) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 6))) << 48) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 7))) << 56)))


#define  LE24(addr)              ((((uint32_t)(*(((uint8_t *)(addr)) + 0))) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 1))) << 8) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 2))) << 16)))


#define  LE32S(addr)              (int32_t)(LE32((addr)))

#define  USB_LEN_DESC_HDR                               0x02
#define  USB_LEN_DEV_DESC                               0x12
#define  USB_LEN_CFG_DESC                               0x09
#define  USB_LEN_IF_DESC                                0x09
#define  USB_LEN_EP_DESC                                0x07
#define  USB_LEN_OTG_DESC                               0x03
#define  USB_LEN_SETUP_PKT                              0x08

/* bmRequestType :D7 Data Phase Transfer Direction  */
#define  USB_REQ_DIR_MASK                               0x80
#define  USB_H2D                                        0x00
#define  USB_D2H                                        0x80

/* bmRequestType D6..5 Type */
#define  USB_REQ_TYPE_STANDARD                          0x00
#define  USB_REQ_TYPE_CLASS                             0x20
#define  USB_REQ_TYPE_VENDOR                            0x40
#define  USB_REQ_TYPE_RESERVED                          0x60

/* bmRequestType D4..0 Recipient */
#define  USB_REQ_RECIPIENT_DEVICE                       0x00
#define  USB_REQ_RECIPIENT_INTERFACE                    0x01
#define  USB_REQ_RECIPIENT_ENDPOINT                     0x02
#define  USB_REQ_RECIPIENT_OTHER                        0x03

/* Table 9-4. Standard Request Codes  */
/* bRequest , Value */
#define  USB_REQ_GET_STATUS                             0x00
#define  USB_REQ_CLEAR_FEATURE                          0x01
#define  USB_REQ_SET_FEATURE                            0x03
#define  USB_REQ_SET_ADDRESS                            0x05
#define  USB_REQ_GET_DESCRIPTOR                         0x06
#define  USB_REQ_SET_DESCRIPTOR                         0x07
#define  USB_REQ_GET_CONFIGURATION                      0x08
#define  USB_REQ_SET_CONFIGURATION                      0x09
#define  USB_REQ_GET_INTERFACE                          0x0A
#define  USB_REQ_SET_INTERFACE                          0x0B
#define  USB_REQ_SYNCH_FRAME                            0x0C

/* Table 9-5. Descriptor Types of USB Specifications */
#define  USB_DESC_TYPE_DEVICE                              1
#define  USB_DESC_TYPE_CONFIGURATION                       2
#define  USB_DESC_TYPE_STRING                              3
#define  USB_DESC_TYPE_INTERFACE                           4
#define  USB_DESC_TYPE_ENDPOINT                            5
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                    6
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION           7
#define  USB_DESC_TYPE_INTERFACE_POWER                     8
#define  USB_DESC_TYPE_HID                                 0x21
#define  USB_DESC_TYPE_HID_REPORT                          0x22

/**
  * @}
  */

/** @defgroup USBH_CTLREQ_Exported_FunctionsPrototype
  * @{
  */
USBH_StatusTypeDef USBH_CtlReq     (USBH_HandleTypeDef *phost,
                             uint8_t             *buff,
                             uint16_t            length);

USBH_StatusTypeDef USBH_GetDescriptor(USBH_HandleTypeDef *phost,
                               uint8_t  req_type,
                               uint16_t value_idx,
                               uint8_t* buff,
                               uint16_t length );

USBH_StatusTypeDef USBH_Get_DevDesc(USBH_HandleTypeDef *phost,
                             uint8_t length);

USBH_StatusTypeDef USBH_Get_StringDesc(USBH_HandleTypeDef *phost,
                                uint8_t string_index,
                                uint8_t *buff,
                                uint16_t length);

USBH_StatusTypeDef USBH_SetCfg(USBH_HandleTypeDef *phost,
                        uint16_t configuration_value);

USBH_StatusTypeDef USBH_Get_CfgDesc(USBH_HandleTypeDef *phost,
                             uint16_t length);

USBH_StatusTypeDef USBH_SetAddress(USBH_HandleTypeDef *phost,
                            uint8_t DeviceAddress);

USBH_StatusTypeDef USBH_SetInterface(USBH_HandleTypeDef *phost,
                        uint8_t ep_num, uint8_t altSetting);

USBH_StatusTypeDef USBH_ClrFeature(USBH_HandleTypeDef *phost,
                                   uint8_t ep_num);

USBH_DescHeader_t      *USBH_GetNextDesc (uint8_t   *pbuf,
                                                  uint16_t  *ptr);



/* Exported functions --------------------------------------------------------*/
/** @defgroup HCD_Exported_Functions HCD Exported Functions
  * @{
  */
/**
  * @}
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/** @addtogroup HCD_Private_Functions
  * @{
  */
static void HCD_HC_IN_IRQHandler(HCD_HandleTypeDef *hhcd, uint8_t chnum);
static void HCD_HC_OUT_IRQHandler(HCD_HandleTypeDef *hhcd, uint8_t chnum);
static void HCD_RXQLVL_IRQHandler(HCD_HandleTypeDef *hhcd);
static void HCD_Port_IRQHandler(HCD_HandleTypeDef *hhcd);



typedef enum
{
  MSC_INIT = 0,
  MSC_IDLE,
  MSC_TEST_UNIT_READY,
  MSC_READ_CAPACITY10,
  MSC_READ_INQUIRY,
  MSC_REQUEST_SENSE,
  MSC_READ,
  MSC_WRITE,
  MSC_UNRECOVERED_ERROR,
  MSC_PERIODIC_CHECK,
}
MSC_StateTypeDef;

typedef enum
{
  MSC_OK,
  MSC_NOT_READY,
  MSC_ERROR,

}
MSC_ErrorTypeDef;

typedef enum
{
  MSC_REQ_IDLE = 0,
  MSC_REQ_RESET,
  MSC_REQ_GET_MAX_LUN,
  MSC_REQ_ERROR,
}
MSC_ReqStateTypeDef;

#ifndef MAX_SUPPORTED_LUN
    #define MAX_SUPPORTED_LUN       2
#endif


// Capacity data.
typedef struct
{
  uint32_t block_nbr;
  uint16_t block_size;
} SCSI_CapacityTypeDef;


// Sense data.
typedef struct
{
  uint8_t key;
  uint8_t asc;
  uint8_t ascq;
} SCSI_SenseTypeDef;

// INQUIRY data.
typedef struct
{
  uint8_t PeripheralQualifier;
  uint8_t DeviceType;
  uint8_t RemovableMedia;
  uint8_t vendor_id[9];
  uint8_t product_id[17];
  uint8_t revision_id[5];
}SCSI_StdInquiryDataTypeDef;

/** @defgroup USBH_MSC_SCSI_Exported_Defines
  * @{
  */
#define OPCODE_TEST_UNIT_READY            0x00
#define OPCODE_READ_CAPACITY10            0x25
#define OPCODE_READ10                     0x28
#define OPCODE_WRITE10                    0x2A
#define OPCODE_REQUEST_SENSE              0x03
#define OPCODE_INQUIRY                    0x12

#define DATA_LEN_MODE_TEST_UNIT_READY        0
#define DATA_LEN_READ_CAPACITY10             8
#define DATA_LEN_INQUIRY                    36
#define DATA_LEN_REQUEST_SENSE              14

#define CBW_CB_LENGTH                       16
#define CBW_LENGTH                          10

/** @defgroup USBH_MSC_SCSI_Exported_Defines
  * @{
  */
#define SCSI_SENSE_KEY_NO_SENSE                          0x00
#define SCSI_SENSE_KEY_RECOVERED_ERROR                   0x01
#define SCSI_SENSE_KEY_NOT_READY                         0x02
#define SCSI_SENSE_KEY_MEDIUM_ERROR                      0x03
#define SCSI_SENSE_KEY_HARDWARE_ERROR                    0x04
#define SCSI_SENSE_KEY_ILLEGAL_REQUEST                   0x05
#define SCSI_SENSE_KEY_UNIT_ATTENTION                    0x06
#define SCSI_SENSE_KEY_DATA_PROTECT                      0x07
#define SCSI_SENSE_KEY_BLANK_CHECK                       0x08
#define SCSI_SENSE_KEY_VENDOR_SPECIFIC                   0x09
#define SCSI_SENSE_KEY_COPY_ABORTED                      0x0A
#define SCSI_SENSE_KEY_ABORTED_COMMAND                   0x0B
#define SCSI_SENSE_KEY_VOLUME_OVERFLOW                   0x0D
#define SCSI_SENSE_KEY_MISCOMPARE                        0x0E
/**
  * @}
  */


/** @defgroup USBH_MSC_SCSI_Exported_Defines
  * @{
  */
#define SCSI_ASC_NO_ADDITIONAL_SENSE_INFORMATION         0x00
#define SCSI_ASC_LOGICAL_UNIT_NOT_READY                  0x04
#define SCSI_ASC_INVALID_FIELD_IN_CDB                    0x24
#define SCSI_ASC_WRITE_PROTECTED                         0x27
#define SCSI_ASC_FORMAT_ERROR                            0x31
#define SCSI_ASC_INVALID_COMMAND_OPERATION_CODE          0x20
#define SCSI_ASC_NOT_READY_TO_READY_CHANGE               0x28
#define SCSI_ASC_MEDIUM_NOT_PRESENT                      0x3A
/**
  * @}
  */


/** @defgroup USBH_MSC_SCSI_Exported_Defines
  * @{
  */
#define SCSI_ASCQ_FORMAT_COMMAND_FAILED                  0x01
#define SCSI_ASCQ_INITIALIZING_COMMAND_REQUIRED          0x02
#define SCSI_ASCQ_OPERATION_IN_PROGRESS                  0x07


typedef enum {
  BOT_OK          = 0,
  BOT_FAIL        = 1,
  BOT_PHASE_ERROR = 2,
  BOT_BUSY        = 3
}
BOT_StatusTypeDef;

typedef enum {
  BOT_CMD_IDLE  = 0,
  BOT_CMD_SEND,
  BOT_CMD_WAIT,
}
BOT_CMDStateTypeDef;

/* CSW Status Definitions */
typedef enum
{

   BOT_CSW_CMD_PASSED   =        0x00,
   BOT_CSW_CMD_FAILED   =        0x01,
   BOT_CSW_PHASE_ERROR  =        0x02,
}
BOT_CSWStatusTypeDef;

typedef enum {
  BOT_SEND_CBW  = 1,
  BOT_SEND_CBW_WAIT,
  BOT_DATA_IN,
  BOT_DATA_IN_WAIT,
  BOT_DATA_OUT,
  BOT_DATA_OUT_WAIT,
  BOT_RECEIVE_CSW,
  BOT_RECEIVE_CSW_WAIT,
  BOT_ERROR_IN,
  BOT_ERROR_OUT,
  BOT_UNRECOVERED_ERROR
}
BOT_StateTypeDef;

typedef union
{
  struct __CBW
  {
    uint32_t Signature;
    uint32_t Tag;
    uint32_t DataTransferLength;
    uint8_t  Flags;
    uint8_t  LUN;
    uint8_t  CBLength;
    uint8_t  CB[16];
  }field;
  uint8_t data[31];
}
BOT_CBWTypeDef;

typedef union
{
  struct __CSW
  {
    uint32_t Signature;
    uint32_t Tag;
    uint32_t DataResidue;
    uint8_t  Status;
  }field;
  uint8_t data[13];
}
BOT_CSWTypeDef;

typedef struct
{
  uint32_t                   data[16];
  BOT_StateTypeDef           state;
  BOT_StateTypeDef           prev_state;
  BOT_CMDStateTypeDef        cmd_state;
  BOT_CBWTypeDef             cbw;
  uint8_t                    Reserved1;
  BOT_CSWTypeDef             csw;
  uint8_t                    Reserved2[3];
  uint8_t                    *pbuf;
}
BOT_HandleTypeDef;

/**
  * @}
  */



/** @defgroup USBH_MSC_BOT_Exported_Defines
  * @{
  */
#define BOT_CBW_SIGNATURE            0x43425355
#define BOT_CBW_TAG                  0x20304050
#define BOT_CSW_SIGNATURE            0x53425355
#define BOT_CBW_LENGTH               31
#define BOT_CSW_LENGTH               13



#define BOT_SEND_CSW_DISABLE         0
#define BOT_SEND_CSW_ENABLE          1

#define BOT_DIR_IN                   0
#define BOT_DIR_OUT                  1
#define BOT_DIR_BOTH                 2

#define BOT_PAGE_LENGTH              512


#define BOT_CBW_CB_LENGTH            16


#define USB_REQ_BOT_RESET                0xFF
#define USB_REQ_GET_MAX_LUN              0xFE

#define MAX_BULK_STALL_COUNT_LIMIT       0x04   /* If STALL is seen on Bulk
                                         Endpoint continuously, this means
                                         that device and Host has phase error
                                         Hence a Reset is needed */


#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
/**
  * @brief Read all host channel interrupts status
  * @param  USBx : Selected device
  * @retval HAL state
  */
uint32_t USB_HC_ReadInterrupt (USB_OTG_GlobalTypeDef *USBx)
{
  return ((USBx_HOST->HAINT) & 0xFFFF);
}

/**
  * @brief  Halt a host channel
  * @param  USBx : Selected device
  * @param  hc_num : Host Channel number
  *         This parameter can be a value from 1 to 15
  * @retval HAL state
  */
HAL_StatusTypeDef USB_HC_Halt(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t hc_num)
{
  uint32_t count = 0;

  /* Check for space in the request queue to issue the halt. */
  if (((USBx_HC(hc_num)->HCCHAR) & (HCCHAR_CTRL << 18)) || ((USBx_HC(hc_num)->HCCHAR) & (HCCHAR_BULK << 18)))
  {
    USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHDIS;

    if ((USBx->HNPTXSTS & 0xFFFF) == 0)
    {
      USBx_HC(hc_num)->HCCHAR &= ~USB_OTG_HCCHAR_CHENA;
      USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHENA;
      USBx_HC(hc_num)->HCCHAR &= ~USB_OTG_HCCHAR_EPDIR;
      do
      {
        if (++count > 1000)
        {
          break;
        }
      }
      while ((USBx_HC(hc_num)->HCCHAR & USB_OTG_HCCHAR_CHENA) == USB_OTG_HCCHAR_CHENA);
    }
    else
    {
      USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHENA;
    }
  }
  else
  {
    USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHDIS;

    if ((USBx_HOST->HPTXSTS & 0xFFFF) == 0)
    {
      USBx_HC(hc_num)->HCCHAR &= ~USB_OTG_HCCHAR_CHENA;
      USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHENA;
      USBx_HC(hc_num)->HCCHAR &= ~USB_OTG_HCCHAR_EPDIR;
      do
      {
        if (++count > 1000)
        {
          break;
        }
      }
      while ((USBx_HC(hc_num)->HCCHAR & USB_OTG_HCCHAR_CHENA) == USB_OTG_HCCHAR_CHENA);
    }
    else
    {
       USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHENA;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Initialize a host channel
  * @param  USBx : Selected device
  * @param  ch_num : Channel number
  *         This parameter can be a value from 1 to 15
  * @param  epnum : Endpoint number
  *          This parameter can be a value from 1 to 15
  * @param  dev_address : Current device address
  *          This parameter can be a value from 0 to 255
  * @param  speed : Current device speed
  *          This parameter can be one of the these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  *            @arg USB_OTG_SPEED_LOW: Low speed mode
  * @param  ep_type : Endpoint Type
  *          This parameter can be one of the these values:
  *            @arg USBD_EP_TYPE_CTRL: Control type
  *            @arg USBD_EP_TYPE_ISOC: Isochronous type
  *            @arg USBD_EP_TYPE_BULK: Bulk type
  *            @arg USBD_EP_TYPE_INTR: Interrupt type
  * @param  mps : Max Packet Size
  *          This parameter can be a value from 0 to32K
  * @retval HAL state
  */
HAL_StatusTypeDef USB_HC_Init(USB_OTG_GlobalTypeDef *USBx,
                              uint8_t ch_num,
                              uint8_t epnum,
                              uint8_t dev_address,
                              uint8_t speed,
                              uint8_t ep_type,
                              uint16_t mps)
{

  /* Clear old interrupt conditions for this host channel. */
  USBx_HC(ch_num)->HCINT = 0xFFFFFFFF;

  /* Enable channel interrupts required for this transfer. */
  switch (ep_type)
  {
  case USBD_EP_TYPE_CTRL:
  case USBD_EP_TYPE_BULK:

    USBx_HC(ch_num)->HCINTMSK = USB_OTG_HCINTMSK_XFRCM  |
                                USB_OTG_HCINTMSK_STALLM |
                                USB_OTG_HCINTMSK_TXERRM |
                                USB_OTG_HCINTMSK_DTERRM |
                                USB_OTG_HCINTMSK_AHBERR |
                                USB_OTG_HCINTMSK_NAKM |
								0;

    if (epnum & 0x80)
    {
      USBx_HC(ch_num)->HCINTMSK |= USB_OTG_HCINTMSK_BBERRM;
    }
    else
    {
      if(USBx != USB_OTG_FS)
      {
		  // HS
        USBx_HC(ch_num)->HCINTMSK |= (USB_OTG_HCINTMSK_NYET | USB_OTG_HCINTMSK_ACKM);
      }
    }
    break;
  case USBD_EP_TYPE_INTR:

    USBx_HC(ch_num)->HCINTMSK = USB_OTG_HCINTMSK_XFRCM  |
                                USB_OTG_HCINTMSK_STALLM |
                                USB_OTG_HCINTMSK_TXERRM |
                                USB_OTG_HCINTMSK_DTERRM |
                                USB_OTG_HCINTMSK_NAKM   |
                                USB_OTG_HCINTMSK_AHBERR |
                                USB_OTG_HCINTMSK_FRMORM ;

    if (epnum & 0x80)
    {
      USBx_HC(ch_num)->HCINTMSK |= USB_OTG_HCINTMSK_BBERRM;
    }

    break;
  case USBD_EP_TYPE_ISOC:

    USBx_HC(ch_num)->HCINTMSK = USB_OTG_HCINTMSK_XFRCM  |
                                USB_OTG_HCINTMSK_ACKM   |
                                USB_OTG_HCINTMSK_AHBERR |
                                USB_OTG_HCINTMSK_FRMORM ;

    if (epnum & 0x80)
    {
      USBx_HC(ch_num)->HCINTMSK |= (USB_OTG_HCINTMSK_TXERRM | USB_OTG_HCINTMSK_BBERRM);
    }
    break;
  }

  /* Enable the top level host channel interrupt. */
  USBx_HOST->HAINTMSK |= (1 << ch_num);

  /* Make sure host channel interrupts are enabled. */
  USBx->GINTMSK |= USB_OTG_GINTMSK_HCIM;

  /* Program the HCCHAR register */
  USBx_HC(ch_num)->HCCHAR = (((dev_address << 22) & USB_OTG_HCCHAR_DAD)  |
                             (((epnum & 0x7F)<< 11) & USB_OTG_HCCHAR_EPNUM)|
                             ((((epnum & 0x80) == 0x80)<< 15) & USB_OTG_HCCHAR_EPDIR)|
                             (((speed == HPRT0_PRTSPD_LOW_SPEED)<< 17) & USB_OTG_HCCHAR_LSDEV)|
                             ((ep_type << 18) & USB_OTG_HCCHAR_EPTYP)|
                             (mps & USB_OTG_HCCHAR_MPSIZ));

  if (ep_type == USBD_EP_TYPE_INTR)
  {
    USBx_HC(ch_num)->HCCHAR |= USB_OTG_HCCHAR_ODDFRM ;
  }

  return HAL_OK;
}



#endif /* CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX */

//++++ HC

/**
  * @}
  */


/** @addtogroup HCD_Exported_Functions_Group2
  *  @brief   HCD IO operation functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    This subsection provides a set of functions allowing to manage the USB Host Data
    Transfer

@endverbatim
  * @{
  */


/**
  * @}
  */

/** @addtogroup HCD_Exported_Functions_Group3
 *  @brief   Peripheral management functions
 *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the HCD data
    transfers.

@endverbatim
  * @{
  */

/**
  * @}
  */

/** @addtogroup HCD_Exported_Functions_Group4
 *  @brief   Peripheral State functions
 *
@verbatim
 ===============================================================================
                      ##### Peripheral State functions #####
 ===============================================================================
    [..]
    This subsection permits to get in run-time the status of the peripheral
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @}
  */


/**
  * @}
  */

/** @addtogroup HCD_Private_Functions
  * @{
  */
/*******************************************************************************
                       LL Driver Interface (USB Host Library --> HCD)
*******************************************************************************/

/**
  * @brief  USBH_LL_GetLastXferSize
  *         Return the last transfered packet size.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @retval Packet Size
  */
uint32_t USBH_LL_GetLastXferSize  (USBH_HandleTypeDef *phost, uint8_t pipe)
{
  return HAL_HCD_HC_GetXferCount(phost->pData, pipe);
}

/**
  * @brief  USBH_LL_SetToggle
  *         Set toggle for a pipe.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @param  pipe_num: Pipe index
  * @param  toggle: toggle (0/1)
  * @retval Status
  */
USBH_StatusTypeDef   USBH_LL_SetToggle   (USBH_HandleTypeDef *phost, uint8_t pipe, uint8_t toggle)
{
  HCD_HandleTypeDef *pHandle;
  pHandle = phost->pData;
  
  if(pHandle->hc[pipe].ep_is_in)
  {
    pHandle->hc[pipe].toggle_in = toggle;
  }
  else
  {
    pHandle->hc[pipe].toggle_out = toggle;
  }

  return USBH_OK;
}

/**
  * @brief  USBH_LL_GetToggle
  *         Return the current toggle of a pipe.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @retval toggle (0/1)
  */
uint8_t  USBH_LL_GetToggle   (USBH_HandleTypeDef *phost, uint8_t pipe)
{
  uint8_t toggle = 0;
  HCD_HandleTypeDef *pHandle;
  pHandle = phost->pData;

  if(pHandle->hc[pipe].ep_is_in)
  {
    toggle = pHandle->hc[pipe].toggle_in;
  }
  else
  {
    toggle = pHandle->hc[pipe].toggle_out;
  }
  return toggle;
}

static uint16_t USBH_GetFreePipe (USBH_HandleTypeDef *phost);


/**
  * @brief  USBH_ClosePipe
  *         Close a  pipe
  * @param  phost: Host Handle
  * @param  pipe_num: Pipe Number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_ClosePipe  (USBH_HandleTypeDef *phost,
                            uint8_t pipe_num)
{

  USBH_LL_ClosePipe(phost, pipe_num);

  return USBH_OK;

}

/**
  * @brief  USBH_BulkSendData
  *         Sends the Bulk Packet to the device
  * @param  phost: Host Handle
  * @param  buff: Buffer pointer from which the Data will be sent to Device
  * @param  length: Length of the data to be sent
  * @param  pipe_num: Pipe Number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_BulkSendData (USBH_HandleTypeDef *phost,
                                uint8_t *buff,
                                uint16_t length,
                                uint8_t pipe_num,
                                uint8_t do_ping )
{
  if (phost->device.speed != USBH_SPEED_HIGH)
  {
    do_ping = 0;
  }

  USBH_LL_SubmitURB (phost,                     /* Driver handle    */
                          pipe_num,             /* Pipe index       */
                          0,                    /* Direction : IN   */
                          USBH_EP_BULK,         /* EP type          */
                          USBH_PID_DATA,        /* Type Data        */
                          buff,                 /* data buffer      */
                          length,               /* data length      */
                          do_ping);             /* do ping (HS Only)*/
  return USBH_OK;
}


/**
  * @brief  USBH_BulkReceiveData
  *         Receives IN bulk packet from device
  * @param  phost: Host Handle
  * @param  buff: Buffer pointer in which the received data packet to be copied
  * @param  length: Length of the data to be received
  * @param  pipe_num: Pipe Number
  * @retval USBH Status.
  */
USBH_StatusTypeDef USBH_BulkReceiveData(USBH_HandleTypeDef *phost,
                                uint8_t *buff,
                                uint16_t length,
                                uint8_t pipe_num)
{
  USBH_LL_SubmitURB (phost,                     /* Driver handle    */
                          pipe_num,             /* Pipe index       */
                          1,                    /* Direction : IN   */
                          USBH_EP_BULK,         /* EP type          */
                          USBH_PID_DATA,        /* Type Data        */
                          buff,                 /* data buffer      */
                          length,               /* data length      */
                          0);
  return USBH_OK;
}


/**
  * @brief  USBH_InterruptReceiveData
  *         Receives the Device Response to the Interrupt IN token
  * @param  phost: Host Handle
  * @param  buff: Buffer pointer in which the response needs to be copied
  * @param  length: Length of the data to be received
  * @param  pipe_num: Pipe Number
  * @retval USBH Status.
  */
USBH_StatusTypeDef USBH_InterruptReceiveData(USBH_HandleTypeDef *phost,
                                uint8_t *buff,
                                uint8_t length,
                                uint8_t pipe_num)
{
  USBH_LL_SubmitURB (phost,                     /* Driver handle    */
                          pipe_num,             /* Pipe index       */
                          1,                    /* Direction : IN   */
                          USBH_EP_INTERRUPT,    /* EP type          */
                          USBH_PID_DATA,        /* Type Data        */
                          buff,                 /* data buffer      */
                          length,               /* data length      */
                          0);
  
  return USBH_OK;
}

/**
  * @brief  USBH_InterruptSendData
  *         Sends the data on Interrupt OUT Endpoint
  * @param  phost: Host Handle
  * @param  buff: Buffer pointer from where the data needs to be copied
  * @param  length: Length of the data to be sent
  * @param  pipe_num: Pipe Number
  * @retval USBH Status.
  */
USBH_StatusTypeDef USBH_InterruptSendData(USBH_HandleTypeDef *phost,
                                uint8_t *buff,
                                uint8_t length,
                                uint8_t pipe_num)
{
  USBH_LL_SubmitURB (phost,                     /* Driver handle    */
                          pipe_num,             /* Pipe index       */
                          0,                    /* Direction : OUT   */
                          USBH_EP_INTERRUPT,    /* EP type          */
                          USBH_PID_DATA,        /* Type Data        */
                          buff,                 /* data buffer      */
                          length,               /* data length      */
                          0);
  
  return USBH_OK;
}

/**
  * @brief  USBH_IsocReceiveData
  *         Receives the Device Response to the Isochronous IN token
  * @param  phost: Host Handle
  * @param  buff: Buffer pointer in which the response needs to be copied
  * @param  length: Length of the data to be received
  * @param  pipe_num: Pipe Number
  * @retval USBH Status.
  */
USBH_StatusTypeDef USBH_IsocReceiveData(USBH_HandleTypeDef *phost,
                                uint8_t *buff,
                                uint32_t length,
                                uint8_t pipe_num)
{
  USBH_LL_SubmitURB (phost,                     /* Driver handle    */
                          pipe_num,             /* Pipe index       */
                          1,                    /* Direction : IN   */
                          USBH_EP_ISO,          /* EP type          */
                          USBH_PID_DATA,        /* Type Data        */
                          buff,                 /* data buffer      */
                          length,               /* data length      */
                          0);


  return USBH_OK;
}

/**
  * @brief  USBH_IsocSendData
  *         Sends the data on Isochronous OUT Endpoint
  * @param  phost: Host Handle
  * @param  buff: Buffer pointer from where the data needs to be copied
  * @param  length: Length of the data to be sent
  * @param  pipe_num: Pipe Number
  * @retval USBH Status.
  */
USBH_StatusTypeDef USBH_IsocSendData(USBH_HandleTypeDef *phost,
                                uint8_t *buff,
                                uint32_t length,
                                uint8_t pipe_num)
{
  USBH_LL_SubmitURB (phost,                     /* Driver handle    */
                          pipe_num,             /* Pipe index       */
                          0,                    /* Direction : OUT   */
                          USBH_EP_ISO,          /* EP type          */
                          USBH_PID_DATA,        /* Type Data        */
                          buff,                 /* data buffer      */
                          length,               /* data length      */
                          0);

  return USBH_OK;
}

/**
* @}
*/


#if (USBH_USE_OS == 1)
/**
  * @brief  USB Host Thread task
  * @param  pvParameters not used
  * @retval None
  */
static void USBH_Process_OS(void const * argument)
{
  osEvent event;

  for(;;)
  {
    event = osMessageGet(((USBH_HandleTypeDef *)argument)->os_event, osWaitForever );

    if( event.status == osEventMessage )
    {
      USBH_Process((USBH_HandleTypeDef *)argument);
    }
   }
}

/**
* @brief  USBH_LL_NotifyURBChange
*         Notify URB state Change
* @param  phost: Host handle
* @retval USBH Status
*/
USBH_StatusTypeDef  USBH_LL_NotifyURBChange (USBH_HandleTypeDef *phost)
{
  osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
  return USBH_OK;
}
#endif


/* Memory management macros */
//#define USBH_malloc               malloc
//#define USBH_free                 free
#define USBH_memset               memset
#define USBH_memcpy               memcpy


uint8_t USBH_AllocPipe  (USBH_HandleTypeDef *phost, uint8_t ep_addr);
USBH_StatusTypeDef USBH_FreePipe  (USBH_HandleTypeDef *phost, uint8_t idx);
USBH_StatusTypeDef USBH_OpenPipe  (USBH_HandleTypeDef *phost,
                           uint8_t pipe_num,
                           uint8_t epnum,
                           uint8_t dev_address,
                           uint8_t speed,
                           uint8_t ep_type,
                           uint16_t mps);


#if WITHUSEUSBFLASH

/** @defgroup USBH_MSC_BOT_Exported_FunctionsPrototype
  * @{
  */
USBH_StatusTypeDef USBH_MSC_BOT_REQ_Reset(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_MSC_BOT_REQ_GetMaxLUN(USBH_HandleTypeDef *phost, uint8_t *Maxlun);

USBH_StatusTypeDef USBH_MSC_BOT_Init(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_MSC_BOT_Process (USBH_HandleTypeDef *phost, uint8_t lun);
USBH_StatusTypeDef USBH_MSC_BOT_Error(USBH_HandleTypeDef *phost, uint8_t lun);



/** @defgroup USBH_MSC_SCSI_Exported_FunctionsPrototype
  * @{
  */
USBH_StatusTypeDef USBH_MSC_SCSI_TestUnitReady (USBH_HandleTypeDef *phost,
                                                uint8_t lun);

USBH_StatusTypeDef USBH_MSC_SCSI_ReadCapacity (USBH_HandleTypeDef *phost,
                                               uint8_t lun,
                                               SCSI_CapacityTypeDef *capacity);

USBH_StatusTypeDef USBH_MSC_SCSI_Inquiry (USBH_HandleTypeDef *phost,
                                               uint8_t lun,
                                               SCSI_StdInquiryDataTypeDef *inquiry);

USBH_StatusTypeDef USBH_MSC_SCSI_RequestSense (USBH_HandleTypeDef *phost,
                                               uint8_t lun,
                                               SCSI_SenseTypeDef *sense_data);

USBH_StatusTypeDef USBH_MSC_SCSI_Write(USBH_HandleTypeDef *phost,
                                     uint8_t lun,
                                     uint32_t address,
                                     uint8_t *pbuf,
                                     uint32_t length);

USBH_StatusTypeDef USBH_MSC_SCSI_Read(USBH_HandleTypeDef *phost,
                                     uint8_t lun,
                                     uint32_t address,
                                     uint8_t *pbuf,
                                     uint32_t length);


/* Structure for LUN */
typedef struct
{
  MSC_StateTypeDef            state;
  MSC_ErrorTypeDef            error;
  USBH_StatusTypeDef          prev_ready_state;
  SCSI_CapacityTypeDef        capacity;
  SCSI_SenseTypeDef           sense;
  SCSI_StdInquiryDataTypeDef  inquiry;
  uint8_t                     state_changed;

}
MSC_LUNTypeDef;

/* Structure for MSC process */
typedef struct _MSC_Process
{
  uint32_t             max_lun;
  uint8_t              InPipe;
  uint8_t              OutPipe;
  uint8_t              OutEp;
  uint8_t              InEp;
  uint16_t             OutEpSize;
  uint16_t             InEpSize;
  MSC_StateTypeDef     state;
  MSC_ErrorTypeDef     error;
  MSC_ReqStateTypeDef  req_state;
  MSC_ReqStateTypeDef  prev_req_state;
  BOT_HandleTypeDef    hbot;
  MSC_LUNTypeDef       unit[MAX_SUPPORTED_LUN];
  uint16_t             current_lun;
  uint16_t             rw_lun;
  uint32_t             timer;
}
MSC_HandleTypeDef;


/**
  * @}
  */



/** @defgroup USBH_MSC_CORE_Exported_Defines
  * @{
  */

#define USB_REQ_BOT_RESET                0xFF
#define USB_REQ_GET_MAX_LUN              0xFE


/* MSC Class Codes */
#define USB_MSC_CLASS                                   0x08

/* Interface Descriptor field values for HID Boot Protocol */
#define MSC_BOT                                        0x50
#define MSC_TRANSPARENT                                0x06



/** @defgroup USBH_MSC_BOT_Private_FunctionPrototypes
* @{
*/
static USBH_StatusTypeDef USBH_MSC_BOT_Abort(USBH_HandleTypeDef *phost, uint8_t lun, uint8_t dir);
static BOT_CSWStatusTypeDef USBH_MSC_DecodeCSW(USBH_HandleTypeDef *phost);
/**
* @}
*/


/** @defgroup USBH_MSC_BOT_Exported_Variables
* @{
*/
/**
* @}
*/


/** @defgroup USBH_MSC_BOT_Private_Functions
* @{
*/

/**
  * @brief  USBH_MSC_BOT_REQ_Reset
  *         The function the MSC BOT Reset request.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_BOT_REQ_Reset(USBH_HandleTypeDef *phost)
{

  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS |
                              USB_REQ_RECIPIENT_INTERFACE;

  phost->Control.setup.b.bRequest = USB_REQ_BOT_RESET;
  phost->Control.setup.b.wValue.w = 0;
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;
  
  return USBH_CtlReq(phost, 0 , 0 );
}

/**
  * @brief  USBH_MSC_BOT_REQ_GetMaxLUN
  *         The function the MSC BOT GetMaxLUN request.
  * @param  phost: Host handle
  * @param  Maxlun: pointer to Maxlun variable
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_BOT_REQ_GetMaxLUN(USBH_HandleTypeDef *phost, uint8_t *Maxlun)
{
  phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS |
                              USB_REQ_RECIPIENT_INTERFACE;

  phost->Control.setup.b.bRequest = USB_REQ_GET_MAX_LUN;
  phost->Control.setup.b.wValue.w = 0;
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 1;

  return USBH_CtlReq(phost, Maxlun , 1 );
}



/**
  * @brief  USBH_MSC_BOT_Init
  *         The function Initializes the BOT protocol.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_BOT_Init(USBH_HandleTypeDef *phost)
{
  
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;

  MSC_Handle->hbot.cbw.field.Signature = BOT_CBW_SIGNATURE;
  MSC_Handle->hbot.cbw.field.Tag = BOT_CBW_TAG;
  MSC_Handle->hbot.state = BOT_SEND_CBW;
  MSC_Handle->hbot.cmd_state = BOT_CMD_SEND;

  return USBH_OK;
}



/**
  * @brief  USBH_MSC_BOT_Process
  *         The function handle the BOT protocol.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_BOT_Process (USBH_HandleTypeDef *phost, uint8_t lun)
{
  USBH_StatusTypeDef   status = USBH_BUSY;
  USBH_StatusTypeDef   error  = USBH_BUSY;
  BOT_CSWStatusTypeDef CSW_Status = BOT_CSW_CMD_FAILED;
  USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  uint8_t toggle = 0;
  
  switch (MSC_Handle->hbot.state)
  {
  case BOT_SEND_CBW:
    MSC_Handle->hbot.cbw.field.LUN = lun;
    MSC_Handle->hbot.state = BOT_SEND_CBW_WAIT;
    USBH_BulkSendData (phost,
                       MSC_Handle->hbot.cbw.data,
                       BOT_CBW_LENGTH,
                       MSC_Handle->OutPipe,
                       1);

    break;

  case BOT_SEND_CBW_WAIT:

    URB_Status = USBH_LL_GetURBState(phost, MSC_Handle->OutPipe);

    if(URB_Status == USBH_URB_DONE)
    {
      if (MSC_Handle->hbot.cbw.field.DataTransferLength != 0 )
      {
        /* If there is Data Transfer Stage */
        if (((MSC_Handle->hbot.cbw.field.Flags) & USB_REQ_DIR_MASK) == USB_D2H)
        {
          /* Data Direction is IN */
          MSC_Handle->hbot.state = BOT_DATA_IN;
        }
        else
        {
          /* Data Direction is OUT */
          MSC_Handle->hbot.state = BOT_DATA_OUT;
        }
      }

      else
      {/* If there is NO Data Transfer Stage */
        MSC_Handle->hbot.state = BOT_RECEIVE_CSW;
      }
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif
    
    }
    else if(URB_Status == USBH_URB_NOTREADY)
    {
      /* Re-send CBW */
      MSC_Handle->hbot.state = BOT_SEND_CBW;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif
    }
    else if(URB_Status == USBH_URB_STALL)
    {
      MSC_Handle->hbot.state  = BOT_ERROR_OUT;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif
    }
    break;

  case BOT_DATA_IN:
    /* Send first packet */
    USBH_BulkReceiveData (phost,
                          MSC_Handle->hbot.pbuf,
                          MSC_Handle->InEpSize ,
                          MSC_Handle->InPipe);

    MSC_Handle->hbot.state  = BOT_DATA_IN_WAIT;

    break;

  case BOT_DATA_IN_WAIT:

    URB_Status = USBH_LL_GetURBState(phost, MSC_Handle->InPipe);

    if(URB_Status == USBH_URB_DONE)
    {
      /* Adjust Data pointer and data length */
      if(MSC_Handle->hbot.cbw.field.DataTransferLength > MSC_Handle->InEpSize)
      {
          MSC_Handle->hbot.pbuf += MSC_Handle->InEpSize;
          MSC_Handle->hbot.cbw.field.DataTransferLength -= MSC_Handle->InEpSize;
      }
      else
      {
        MSC_Handle->hbot.cbw.field.DataTransferLength = 0;
      }

      /* More Data To be Received */
      if(MSC_Handle->hbot.cbw.field.DataTransferLength > 0)
      {
        /* Send next packet */
        USBH_BulkReceiveData (phost,
                              MSC_Handle->hbot.pbuf,
                              MSC_Handle->InEpSize ,
                              MSC_Handle->InPipe);

      }
      else
      {
        /* If value was 0, and successful transfer, then change the state */
        MSC_Handle->hbot.state  = BOT_RECEIVE_CSW;
#if (USBH_USE_OS == 1)
        osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif
      }
    }
    else if(URB_Status == USBH_URB_STALL)
    {
      /* This is Data IN Stage STALL Condition */
      MSC_Handle->hbot.state  = BOT_ERROR_IN;

      /* Refer to USB Mass-Storage Class : BOT (www.usb.org)
      6.7.2 Host expects to receive data from the device
      3. On a STALL condition receiving data, then:
      The host shall accept the data received.
      The host shall clear the Bulk-In pipe.
      4. The host shall attempt to receive a CSW.*/

#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif
    }
    break;

  case BOT_DATA_OUT:

    USBH_BulkSendData (phost,
                       MSC_Handle->hbot.pbuf,
                       MSC_Handle->OutEpSize ,
                       MSC_Handle->OutPipe,
                       1);


    MSC_Handle->hbot.state  = BOT_DATA_OUT_WAIT;
    break;

  case BOT_DATA_OUT_WAIT:
    URB_Status = USBH_LL_GetURBState(phost, MSC_Handle->OutPipe);

    if(URB_Status == USBH_URB_DONE)
    {
      /* Adjust Data pointer and data length */
      if(MSC_Handle->hbot.cbw.field.DataTransferLength > MSC_Handle->OutEpSize)
      {
          MSC_Handle->hbot.pbuf += MSC_Handle->OutEpSize;
          MSC_Handle->hbot.cbw.field.DataTransferLength -= MSC_Handle->OutEpSize;
      }
      else
      {
        MSC_Handle->hbot.cbw.field.DataTransferLength = 0;
      }

      /* More Data To be Sent */
      if(MSC_Handle->hbot.cbw.field.DataTransferLength > 0)
      {
        USBH_BulkSendData (phost,
                           MSC_Handle->hbot.pbuf,
                           MSC_Handle->OutEpSize ,
                           MSC_Handle->OutPipe,
                           1);
      }
      else
      {
        /* If value was 0, and successful transfer, then change the state */
        MSC_Handle->hbot.state  = BOT_RECEIVE_CSW;
      }
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif
    }

    else if(URB_Status == USBH_URB_NOTREADY)
    {
      /* Resend same data */
      MSC_Handle->hbot.state  = BOT_DATA_OUT;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif
    }

    else if(URB_Status == USBH_URB_STALL)
    {
      MSC_Handle->hbot.state  = BOT_ERROR_OUT;

      /* Refer to USB Mass-Storage Class : BOT (www.usb.org)
      6.7.3 Ho - Host expects to send data to the device
      3. On a STALL condition sending data, then:
      " The host shall clear the Bulk-Out pipe.
      4. The host shall attempt to receive a CSW.
      */
#if (USBH_USE_OS == 1)
      osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif
    }
    break;

  case BOT_RECEIVE_CSW:

    USBH_BulkReceiveData (phost,
                          MSC_Handle->hbot.csw.data,
                          BOT_CSW_LENGTH ,
                          MSC_Handle->InPipe);

    MSC_Handle->hbot.state  = BOT_RECEIVE_CSW_WAIT;
    break;

  case BOT_RECEIVE_CSW_WAIT:

    URB_Status = USBH_LL_GetURBState(phost, MSC_Handle->InPipe);

    /* Decode CSW */
    if(URB_Status == USBH_URB_DONE)
    {
      MSC_Handle->hbot.state = BOT_SEND_CBW;
      MSC_Handle->hbot.cmd_state = BOT_CMD_SEND;
      CSW_Status = USBH_MSC_DecodeCSW(phost);

      if(CSW_Status == BOT_CSW_CMD_PASSED)
      {
        status = USBH_OK;
      }
      else
      {
        status = USBH_FAIL;
      }
#if (USBH_USE_OS == 1)
      osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif
    }
    else if(URB_Status == USBH_URB_STALL)
    {
      MSC_Handle->hbot.state  = BOT_ERROR_IN;
#if (USBH_USE_OS == 1)
      osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif
    }
    break;

  case BOT_ERROR_IN:
    error = USBH_MSC_BOT_Abort(phost, lun, BOT_DIR_IN);

    if (error == USBH_OK)
    {
      MSC_Handle->hbot.state = BOT_RECEIVE_CSW;
    }
    else if (error == USBH_UNRECOVERED_ERROR)
    {
      /* This means that there is a STALL Error limit, Do Reset Recovery */
      MSC_Handle->hbot.state = BOT_UNRECOVERED_ERROR;
    }
    break;

  case BOT_ERROR_OUT:
    error = USBH_MSC_BOT_Abort(phost, lun, BOT_DIR_OUT);

    if (error == USBH_OK)
    {

      toggle = USBH_LL_GetToggle(phost, MSC_Handle->OutPipe);
      USBH_LL_SetToggle(phost, MSC_Handle->OutPipe, 1- toggle);
      USBH_LL_SetToggle(phost, MSC_Handle->InPipe, 0);
      MSC_Handle->hbot.state = BOT_ERROR_IN;
    }
    else if (error == USBH_UNRECOVERED_ERROR)
    {
      MSC_Handle->hbot.state = BOT_UNRECOVERED_ERROR;
    }
    break;


  case BOT_UNRECOVERED_ERROR:
    status = USBH_MSC_BOT_REQ_Reset(phost);
    if (status == USBH_OK)
    {
      MSC_Handle->hbot.state = BOT_SEND_CBW;
    }
    break;

  default:
    break;
  }
  return status;
}

/**
  * @brief  USBH_MSC_BOT_Abort
  *         The function handle the BOT Abort process.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  dir: direction (0: out / 1 : in)
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_MSC_BOT_Abort(USBH_HandleTypeDef *phost, uint8_t lun, uint8_t dir)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  
  switch (dir)
  {
  case BOT_DIR_IN :
    /* send ClrFeture on Bulk IN endpoint */
    status = USBH_ClrFeature(phost, MSC_Handle->InEp);

    break;

  case BOT_DIR_OUT :
    /*send ClrFeature on Bulk OUT endpoint */
    status = USBH_ClrFeature(phost, MSC_Handle->OutEp);
    break;

  default:
    break;
  }
  return status;
}

/**
  * @brief  USBH_MSC_BOT_DecodeCSW
  *         This function decodes the CSW received by the device and updates the
  *         same to upper layer.
  * @param  phost: Host handle
  * @retval USBH Status
  * @notes
  *     Refer to USB Mass-Storage Class : BOT (www.usb.org)
  *    6.3.1 Valid CSW Conditions :
  *     The host shall consider the CSW valid when:
  *     1. dCSWSignature is equal to 53425355h
  *     2. the CSW is 13 (Dh) bytes in length,
  *     3. dCSWTag matches the dCBWTag from the corresponding CBW.
  */

static BOT_CSWStatusTypeDef USBH_MSC_DecodeCSW(USBH_HandleTypeDef *phost)
{
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  BOT_CSWStatusTypeDef status = BOT_CSW_CMD_FAILED;
  
    /*Checking if the transfer length is different than 13*/
    if(USBH_LL_GetLastXferSize(phost, MSC_Handle->InPipe) != BOT_CSW_LENGTH)
    {
      /*(4) Hi > Dn (Host expects to receive data from the device,
      Device intends to transfer no data)
      (5) Hi > Di (Host expects to receive data from the device,
      Device intends to send data to the host)
      (9) Ho > Dn (Host expects to send data to the device,
      Device intends to transfer no data)
      (11) Ho > Do  (Host expects to send data to the device,
      Device intends to receive data from the host)*/


      status = BOT_CSW_PHASE_ERROR;
    }
    else
    { /* CSW length is Correct */

      /* Check validity of the CSW Signature and CSWStatus */
      if(MSC_Handle->hbot.csw.field.Signature == BOT_CSW_SIGNATURE)
      {/* Check Condition 1. dCSWSignature is equal to 53425355h */

        if(MSC_Handle->hbot.csw.field.Tag == MSC_Handle->hbot.cbw.field.Tag)
        {
          /* Check Condition 3. dCSWTag matches the dCBWTag from the
          corresponding CBW */

          if(MSC_Handle->hbot.csw.field.Status == 0)
          {
            /* Refer to USB Mass-Storage Class : BOT (www.usb.org)

            Hn Host expects no data transfers
            Hi Host expects to receive data from the device
            Ho Host expects to send data to the device

            Dn Device intends to transfer no data
            Di Device intends to send data to the host
            Do Device intends to receive data from the host

            Section 6.7
            (1) Hn = Dn (Host expects no data transfers,
            Device intends to transfer no data)
            (6) Hi = Di (Host expects to receive data from the device,
            Device intends to send data to the host)
            (12) Ho = Do (Host expects to send data to the device,
            Device intends to receive data from the host)

            */

            status = BOT_CSW_CMD_PASSED;
          }
          else if(MSC_Handle->hbot.csw.field.Status == 1)
          {
            status = BOT_CSW_CMD_FAILED;
          }

          else if(MSC_Handle->hbot.csw.field.Status == 2)
          {
            /* Refer to USB Mass-Storage Class : BOT (www.usb.org)
            Section 6.7
            (2) Hn < Di ( Host expects no data transfers,
            Device intends to send data to the host)
            (3) Hn < Do ( Host expects no data transfers,
            Device intends to receive data from the host)
            (7) Hi < Di ( Host expects to receive data from the device,
            Device intends to send data to the host)
            (8) Hi <> Do ( Host expects to receive data from the device,
            Device intends to receive data from the host)
            (10) Ho <> Di (Host expects to send data to the device,
            Di Device intends to send data to the host)
            (13) Ho < Do (Host expects to send data to the device,
            Device intends to receive data from the host)
            */

            status = BOT_CSW_PHASE_ERROR;
          }
        } /* CSW Tag Matching is Checked  */
      } /* CSW Signature Correct Checking */
      else
      {
        /* If the CSW Signature is not valid, We sall return the Phase Error to
        Upper Layers for Reset Recovery */
        
        status = BOT_CSW_PHASE_ERROR;
      }
    } /* CSW Length Check*/

  return status;
}


static USBH_StatusTypeDef USBH_MSC_InterfaceInit  (USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_MSC_InterfaceDeInit  (USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_MSC_Process(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_MSC_ClassRequest(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_MSC_SOFProcess(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_MSC_RdWrProcess(USBH_HandleTypeDef *phost, uint8_t lun);

static USBH_ClassTypeDef  USBH_msc =
{
  "MSC",
  USB_MSC_CLASS,
  USBH_MSC_InterfaceInit,
  USBH_MSC_InterfaceDeInit,
  USBH_MSC_ClassRequest,
  USBH_MSC_Process,
  USBH_MSC_SOFProcess,
  NULL,
};


/**
  * @}
  */


/** @defgroup USBH_MSC_CORE_Exported_Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup USBH_MSC_CORE_Private_Functions
  * @{
  */

/**
  * @brief  USBH_MSC_InterfaceInit
  *         The function init the MSC class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_MSC_InterfaceInit (USBH_HandleTypeDef *phost)
{
  uint8_t interfacei = 0;
  USBH_StatusTypeDef status = USBH_FAIL ;
  MSC_HandleTypeDef *MSC_Handle;

  static MSC_HandleTypeDef msc_handle;
  interfacei = USBH_FindInterface(phost, phost->pActiveClass->ClassCode, MSC_TRANSPARENT, MSC_BOT);

  if (interfacei == 0xFF) /* Not Valid Interface */
  {
    PRINTF(PSTR("Cannot Find the interfacei for %s class.\n"), phost->pActiveClass->Name);
    status = USBH_FAIL;
  }
  else
  {
    USBH_SelectInterface (phost, interfacei);

    //phost->pActiveClass->pData = (MSC_HandleTypeDef *)USBH_malloc (sizeof(MSC_HandleTypeDef));
    phost->pActiveClass->pData = & msc_handle;
    MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;

    if (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress & 0x80)
    {
      MSC_Handle->InEp = (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress);
      MSC_Handle->InEpSize  = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].wMaxPacketSize;
    }
    else
    {
      MSC_Handle->OutEp = (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress);
      MSC_Handle->OutEpSize  = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].wMaxPacketSize;
    }
    
    if (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[1].bEndpointAddress & 0x80)
    {
      MSC_Handle->InEp = (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[1].bEndpointAddress);
      MSC_Handle->InEpSize  = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[1].wMaxPacketSize;
    }
    else
    {
      MSC_Handle->OutEp = (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[1].bEndpointAddress);
      MSC_Handle->OutEpSize  = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[1].wMaxPacketSize;
    }
    
    MSC_Handle->current_lun = 0;
    MSC_Handle->rw_lun = 0;
    MSC_Handle->state = MSC_INIT;
    MSC_Handle->error = MSC_OK;
    MSC_Handle->req_state = MSC_REQ_IDLE;
    MSC_Handle->OutPipe = USBH_AllocPipe(phost, MSC_Handle->OutEp);
    MSC_Handle->InPipe = USBH_AllocPipe(phost, MSC_Handle->InEp);

    USBH_MSC_BOT_Init(phost);
    
    /* De-Initialize LUNs information */
    USBH_memset(MSC_Handle->unit, 0, sizeof(MSC_Handle->unit));
    
    /* Open the new channels */
    USBH_OpenPipe  (phost,
                    MSC_Handle->OutPipe,
                    MSC_Handle->OutEp,
                    phost->device.address,
                    phost->device.speed,
                    USB_EP_TYPE_BULK,
                    MSC_Handle->OutEpSize);

    USBH_OpenPipe  (phost,
                    MSC_Handle->InPipe,
                    MSC_Handle->InEp,
                    phost->device.address,
                    phost->device.speed,
                    USB_EP_TYPE_BULK,
                    MSC_Handle->InEpSize);


    USBH_LL_SetToggle  (phost, MSC_Handle->InPipe,0);
    USBH_LL_SetToggle  (phost, MSC_Handle->OutPipe,0);
    status = USBH_OK;
  }
  return status;
}

/**
  * @brief  USBH_MSC_InterfaceDeInit
  *         The function DeInit the Pipes used for the MSC class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_InterfaceDeInit (USBH_HandleTypeDef *phost)
{
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;

  if (MSC_Handle->OutPipe)
  {
    USBH_ClosePipe(phost, MSC_Handle->OutPipe);
    USBH_FreePipe(phost, MSC_Handle->OutPipe);
    MSC_Handle->OutPipe = 0;     /* Reset the Channel as Free */
  }

  if (MSC_Handle->InPipe)
  {
    USBH_ClosePipe(phost, MSC_Handle->InPipe);
    USBH_FreePipe(phost, MSC_Handle->InPipe);
    MSC_Handle->InPipe = 0;     /* Reset the Channel as Free */
  }

  if (phost->pActiveClass->pData)
  {
    ////USBH_free (phost->pActiveClass->pData);
    phost->pActiveClass->pData = 0;
  }

  return USBH_OK;
}

/**
  * @brief  USBH_MSC_ClassRequest
  *         The function is responsible for handling Standard requests
  *         for MSC class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_MSC_ClassRequest(USBH_HandleTypeDef *phost)
{
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  USBH_StatusTypeDef status = USBH_BUSY;
  uint8_t i;

  /* Switch MSC REQ state machine */
  switch (MSC_Handle->req_state)
  {
  case MSC_REQ_IDLE:
  case MSC_REQ_GET_MAX_LUN:
    /* Issue GetMaxLUN request */
    status = USBH_MSC_BOT_REQ_GetMaxLUN(phost, (uint8_t *)&MSC_Handle->max_lun);
    
    /* When devices do not support the GetMaxLun request, this should
       be considred as only one logical unit is supported */
    if(status == USBH_NOT_SUPPORTED)
    {
      MSC_Handle->max_lun = 0;
      status = USBH_OK;
    }
    
    if(status == USBH_OK)
    {
      MSC_Handle->max_lun = (uint8_t )(MSC_Handle->max_lun) + 1;
      PRINTF(PSTR("Number of supported LUN: %lu\n"), (int32_t)(MSC_Handle->max_lun));
      
      for(i = 0; i < MSC_Handle->max_lun; i++)
      {
        MSC_Handle->unit[i].prev_ready_state = USBH_FAIL;
        MSC_Handle->unit[i].state_changed = 0;
      }
    }
    break;
    
  case MSC_REQ_ERROR :
    /* a Clear Feature should be issued here */
    if(USBH_ClrFeature(phost, 0x00) == USBH_OK)
    {
      MSC_Handle->req_state = MSC_Handle->prev_req_state;
    }
    break;

  default:
    break;
  }

  return status;
}


/**
  * @brief  USBH_MSC_SCSI_TestUnitReady
  *         Issue TestUnitReady command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_SCSI_TestUnitReady (USBH_HandleTypeDef *phost,
                                                uint8_t lun)
{
  USBH_StatusTypeDef    error = USBH_FAIL ;
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  
  switch(MSC_Handle->hbot.cmd_state)
  {
  case BOT_CMD_SEND:
    
    /*Prepare the CBW and relevent field*/
    MSC_Handle->hbot.cbw.field.DataTransferLength = DATA_LEN_MODE_TEST_UNIT_READY;
    MSC_Handle->hbot.cbw.field.Flags = USB_EP_DIR_OUT;
    MSC_Handle->hbot.cbw.field.CBLength = CBW_LENGTH;
    
    USBH_memset(MSC_Handle->hbot.cbw.field.CB, 0, CBW_CB_LENGTH);
    MSC_Handle->hbot.cbw.field.CB[0]  = OPCODE_TEST_UNIT_READY;

    MSC_Handle->hbot.state = BOT_SEND_CBW;
    MSC_Handle->hbot.cmd_state = BOT_CMD_WAIT;
    error = USBH_BUSY;
    break;

  case BOT_CMD_WAIT:
    error = USBH_MSC_BOT_Process(phost, lun);
    break;
    
  default:
    break;
  }

  return error;
}

/**
  * @brief  USBH_MSC_SCSI_ReadCapacity
  *         Issue Read Capacity command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  capacity: pointer to the capacity structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_SCSI_ReadCapacity (USBH_HandleTypeDef *phost,
                                               uint8_t lun,
                                               SCSI_CapacityTypeDef *capacity)
{
  USBH_StatusTypeDef    error = USBH_BUSY ;
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  
  switch(MSC_Handle->hbot.cmd_state)
  {
  case BOT_CMD_SEND:

    /*Prepare the CBW and relevent field*/
    MSC_Handle->hbot.cbw.field.DataTransferLength = DATA_LEN_READ_CAPACITY10;
    MSC_Handle->hbot.cbw.field.Flags = USB_EP_DIR_IN;
    MSC_Handle->hbot.cbw.field.CBLength = CBW_LENGTH;

    USBH_memset(MSC_Handle->hbot.cbw.field.CB, 0, CBW_CB_LENGTH);
    MSC_Handle->hbot.cbw.field.CB[0]  = OPCODE_READ_CAPACITY10;

    MSC_Handle->hbot.state = BOT_SEND_CBW;

    MSC_Handle->hbot.cmd_state = BOT_CMD_WAIT;
    MSC_Handle->hbot.pbuf = (uint8_t *)MSC_Handle->hbot.data;
    error = USBH_BUSY;
    break;

  case BOT_CMD_WAIT:

    error = USBH_MSC_BOT_Process(phost, lun);

    if(error == USBH_OK)
    {
      /*assign the capacity*/
      capacity->block_nbr = MSC_Handle->hbot.pbuf[3] | (MSC_Handle->hbot.pbuf[2] << 8) |
                           (MSC_Handle->hbot.pbuf[1] << 16) | (MSC_Handle->hbot.pbuf[0] << 24);

      /*assign the page length*/
      capacity->block_size = MSC_Handle->hbot.pbuf[7] | (MSC_Handle->hbot.pbuf[6] << 8);
    }
    break;

  default:
    break;
  }

  return error;
}

/**
  * @brief  USBH_MSC_SCSI_Inquiry
  *         Issue Inquiry command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  capacity: pointer to the inquiry structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_SCSI_Inquiry (USBH_HandleTypeDef *phost,
                                               uint8_t lun,
                                               SCSI_StdInquiryDataTypeDef *inquiry)
{
  USBH_StatusTypeDef    error = USBH_FAIL ;
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  switch(MSC_Handle->hbot.cmd_state)
  {
  case BOT_CMD_SEND:

    /*Prepare the CBW and relevent field*/
    MSC_Handle->hbot.cbw.field.DataTransferLength = DATA_LEN_INQUIRY;
    MSC_Handle->hbot.cbw.field.Flags = USB_EP_DIR_IN;
    MSC_Handle->hbot.cbw.field.CBLength = CBW_LENGTH;

    USBH_memset(MSC_Handle->hbot.cbw.field.CB, 0, CBW_LENGTH);
    MSC_Handle->hbot.cbw.field.CB[0]  = OPCODE_INQUIRY;
    MSC_Handle->hbot.cbw.field.CB[1]  = (lun << 5);
    MSC_Handle->hbot.cbw.field.CB[2]  = 0;
    MSC_Handle->hbot.cbw.field.CB[3]  = 0;
    MSC_Handle->hbot.cbw.field.CB[4]  = 0x24;
    MSC_Handle->hbot.cbw.field.CB[5]  = 0;
        
    MSC_Handle->hbot.state = BOT_SEND_CBW;

    MSC_Handle->hbot.cmd_state = BOT_CMD_WAIT;
    MSC_Handle->hbot.pbuf = (uint8_t *)MSC_Handle->hbot.data;
    error = USBH_BUSY;
    break;

  case BOT_CMD_WAIT:

    error = USBH_MSC_BOT_Process(phost, lun);

    if(error == USBH_OK)
    {
      USBH_memset(inquiry, 0, sizeof(SCSI_StdInquiryDataTypeDef));
      /*assign Inquiry Data */
      inquiry->DeviceType = MSC_Handle->hbot.pbuf[0] & 0x1F;
      inquiry->PeripheralQualifier = MSC_Handle->hbot.pbuf[0] >> 5;
      inquiry->RemovableMedia = (MSC_Handle->hbot.pbuf[1] & 0x80)== 0x80;
      USBH_memcpy (inquiry->vendor_id, &MSC_Handle->hbot.pbuf[8], 8);
      USBH_memcpy (inquiry->product_id, &MSC_Handle->hbot.pbuf[16], 16);
      USBH_memcpy (inquiry->revision_id, &MSC_Handle->hbot.pbuf[32], 4);
    }
    break;
    
  default:
    break;
  }

  return error;
}

/**
  * @brief  USBH_MSC_SCSI_RequestSense
  *         Issue RequestSense command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  capacity: pointer to the sense data structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_SCSI_RequestSense (USBH_HandleTypeDef *phost,
                                               uint8_t lun,
                                               SCSI_SenseTypeDef *sense_data)
{
  USBH_StatusTypeDef    error = USBH_FAIL ;
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  
  switch(MSC_Handle->hbot.cmd_state)
  {
  case BOT_CMD_SEND:
    
    /*Prepare the CBW and relevent field*/
    MSC_Handle->hbot.cbw.field.DataTransferLength = DATA_LEN_REQUEST_SENSE;
    MSC_Handle->hbot.cbw.field.Flags = USB_EP_DIR_IN;
    MSC_Handle->hbot.cbw.field.CBLength = CBW_LENGTH;
    
    USBH_memset(MSC_Handle->hbot.cbw.field.CB, 0, CBW_CB_LENGTH);
    MSC_Handle->hbot.cbw.field.CB[0]  = OPCODE_REQUEST_SENSE;
    MSC_Handle->hbot.cbw.field.CB[1]  = (lun << 5);
    MSC_Handle->hbot.cbw.field.CB[2]  = 0;
    MSC_Handle->hbot.cbw.field.CB[3]  = 0;
    MSC_Handle->hbot.cbw.field.CB[4]  = DATA_LEN_REQUEST_SENSE;
    MSC_Handle->hbot.cbw.field.CB[5]  = 0;
    
    MSC_Handle->hbot.state = BOT_SEND_CBW;
    MSC_Handle->hbot.cmd_state = BOT_CMD_WAIT;
    MSC_Handle->hbot.pbuf = (uint8_t *)MSC_Handle->hbot.data;
    error = USBH_BUSY;
    break;
    
  case BOT_CMD_WAIT:
    
    error = USBH_MSC_BOT_Process(phost, lun);
    
    if(error == USBH_OK)
    {
      sense_data->key  = MSC_Handle->hbot.pbuf[2] & 0x0F;
      sense_data->asc  = MSC_Handle->hbot.pbuf[12];
      sense_data->ascq = MSC_Handle->hbot.pbuf[13];
    }
    break;
    
  default:
    break;
  }

  return error;
}

/**
  * @brief  USBH_MSC_SCSI_Write
  *         Issue write10 command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to write
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_SCSI_Write(USBH_HandleTypeDef *phost,
                                     uint8_t lun,
                                     uint32_t address,
                                     uint8_t *pbuf,
                                     uint32_t length)
{
  USBH_StatusTypeDef    error = USBH_FAIL ;

  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  
  switch(MSC_Handle->hbot.cmd_state)
  {
  case BOT_CMD_SEND:

    /*Prepare the CBW and relevent field*/
    MSC_Handle->hbot.cbw.field.DataTransferLength = length * 512;
    MSC_Handle->hbot.cbw.field.Flags = USB_EP_DIR_OUT;
    MSC_Handle->hbot.cbw.field.CBLength = CBW_LENGTH;

    USBH_memset(MSC_Handle->hbot.cbw.field.CB, 0, CBW_CB_LENGTH);
    MSC_Handle->hbot.cbw.field.CB[0]  = OPCODE_WRITE10;

    /*logical block address*/
    MSC_Handle->hbot.cbw.field.CB[2]  = (((uint8_t*)&address)[3]);
    MSC_Handle->hbot.cbw.field.CB[3]  = (((uint8_t*)&address)[2]);
    MSC_Handle->hbot.cbw.field.CB[4]  = (((uint8_t*)&address)[1]);
    MSC_Handle->hbot.cbw.field.CB[5]  = (((uint8_t*)&address)[0]);


    /*Transfer length */
    MSC_Handle->hbot.cbw.field.CB[7]  = (((uint8_t *)&length)[1]) ;
    MSC_Handle->hbot.cbw.field.CB[8]  = (((uint8_t *)&length)[0]) ;

    
    MSC_Handle->hbot.state = BOT_SEND_CBW;
    MSC_Handle->hbot.cmd_state = BOT_CMD_WAIT;
    MSC_Handle->hbot.pbuf = pbuf;
    error = USBH_BUSY;
    break;
    
  case BOT_CMD_WAIT:
    error = USBH_MSC_BOT_Process(phost, lun);
    break;
    
  default:
    break;
  }
  
  return error;
}

/**
  * @brief  USBH_MSC_SCSI_Read
  *         Issue Read10 command.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to read
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_SCSI_Read(USBH_HandleTypeDef *phost,
                                     uint8_t lun,
                                     uint32_t address,
                                     uint8_t *pbuf,
                                     uint32_t length)
{
  USBH_StatusTypeDef    error = USBH_FAIL ;
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  
  switch(MSC_Handle->hbot.cmd_state)
  {
  case BOT_CMD_SEND:  
    
    /*Prepare the CBW and relevent field*/
    MSC_Handle->hbot.cbw.field.DataTransferLength = length * 512;
    MSC_Handle->hbot.cbw.field.Flags = USB_EP_DIR_IN;
    MSC_Handle->hbot.cbw.field.CBLength = CBW_LENGTH;
    
    USBH_memset(MSC_Handle->hbot.cbw.field.CB, 0, CBW_CB_LENGTH);
    MSC_Handle->hbot.cbw.field.CB[0]  = OPCODE_READ10;

    /*logical block address*/
    MSC_Handle->hbot.cbw.field.CB[2]  = (((uint8_t*)&address)[3]);
    MSC_Handle->hbot.cbw.field.CB[3]  = (((uint8_t*)&address)[2]);
    MSC_Handle->hbot.cbw.field.CB[4]  = (((uint8_t*)&address)[1]);
    MSC_Handle->hbot.cbw.field.CB[5]  = (((uint8_t*)&address)[0]);


    /*Transfer length */
    MSC_Handle->hbot.cbw.field.CB[7]  = (((uint8_t *)&length)[1]) ;
    MSC_Handle->hbot.cbw.field.CB[8]  = (((uint8_t *)&length)[0]) ;

    
    MSC_Handle->hbot.state = BOT_SEND_CBW;
    MSC_Handle->hbot.cmd_state = BOT_CMD_WAIT;
    MSC_Handle->hbot.pbuf = pbuf;
    error = USBH_BUSY; 
    break;
    
  case BOT_CMD_WAIT: 
    error = USBH_MSC_BOT_Process(phost, lun);
    break;
    
  default:
    break;
  }
  
  return error;
}


/**
  * @brief  USBH_MSC_Process
  *         The function is for managing state machine for MSC data transfers
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_MSC_Process(USBH_HandleTypeDef *phost)
{
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  USBH_StatusTypeDef error = USBH_BUSY ;
  USBH_StatusTypeDef scsi_status = USBH_BUSY ;
  USBH_StatusTypeDef ready_status = USBH_BUSY ;
  
  switch (MSC_Handle->state)
  {
  case MSC_INIT:
    
    if(MSC_Handle->current_lun < MSC_Handle->max_lun)
    {

      MSC_Handle->unit[MSC_Handle->current_lun].error = MSC_NOT_READY;
      /* Switch MSC REQ state machine */
      switch (MSC_Handle->unit[MSC_Handle->current_lun].state)
      {
      case MSC_INIT:
        PRINTF(PSTR("LUN #%d: \n"), MSC_Handle->current_lun);
        MSC_Handle->unit[MSC_Handle->current_lun].state = MSC_READ_INQUIRY;
        MSC_Handle->timer = phost->Timer;

      case MSC_READ_INQUIRY:
        scsi_status = USBH_MSC_SCSI_Inquiry(phost, MSC_Handle->current_lun, &MSC_Handle->unit[MSC_Handle->current_lun].inquiry);

        if( scsi_status == USBH_OK)
        {
          PRINTF(PSTR("Inquiry Vendor  : %s\n"), MSC_Handle->unit[MSC_Handle->current_lun].inquiry.vendor_id);
          PRINTF(PSTR("Inquiry Product : %s\n"), MSC_Handle->unit[MSC_Handle->current_lun].inquiry.product_id);
          PRINTF(PSTR("Inquiry Version : %s\n"), MSC_Handle->unit[MSC_Handle->current_lun].inquiry.revision_id);
          MSC_Handle->unit[MSC_Handle->current_lun].state = MSC_TEST_UNIT_READY;
        }
        if( scsi_status == USBH_FAIL)
        {
          MSC_Handle->unit[MSC_Handle->current_lun].state = MSC_REQUEST_SENSE;
        }
        else if(scsi_status == USBH_UNRECOVERED_ERROR)
        {
          MSC_Handle->unit[MSC_Handle->current_lun].state = MSC_IDLE;
          MSC_Handle->unit[MSC_Handle->current_lun].error = MSC_ERROR;
        }
        break;

      case MSC_TEST_UNIT_READY:
        ready_status = USBH_MSC_SCSI_TestUnitReady(phost, MSC_Handle->current_lun);
        
        if( ready_status == USBH_OK)
        {
          if( MSC_Handle->unit[MSC_Handle->current_lun].prev_ready_state != USBH_OK)
          {
            MSC_Handle->unit[MSC_Handle->current_lun].state_changed = 1;
            PRINTF(PSTR("MSC Device ready\n"));
          }
          else
          {
            MSC_Handle->unit[MSC_Handle->current_lun].state_changed = 0;
          }
          MSC_Handle->unit[MSC_Handle->current_lun].state = MSC_READ_CAPACITY10;
          MSC_Handle->unit[MSC_Handle->current_lun].error = MSC_OK;
          MSC_Handle->unit[MSC_Handle->current_lun].prev_ready_state = USBH_OK;
        }
        if( ready_status == USBH_FAIL)
        {
          /* Media not ready, so try to check again during 10s */
          if( MSC_Handle->unit[MSC_Handle->current_lun].prev_ready_state != USBH_FAIL)
          {
            MSC_Handle->unit[MSC_Handle->current_lun].state_changed = 1;
            PRINTF(PSTR("MSC Device NOT ready\n"));
          }
          else
          {
            MSC_Handle->unit[MSC_Handle->current_lun].state_changed = 0;
          }
            MSC_Handle->unit[MSC_Handle->current_lun].state = MSC_REQUEST_SENSE;
            MSC_Handle->unit[MSC_Handle->current_lun].error = MSC_NOT_READY;
            MSC_Handle->unit[MSC_Handle->current_lun].prev_ready_state = USBH_FAIL;
        }
        else if(ready_status == USBH_UNRECOVERED_ERROR)
        {
          MSC_Handle->unit[MSC_Handle->current_lun].state = MSC_IDLE;
          MSC_Handle->unit[MSC_Handle->current_lun].error = MSC_ERROR;
        }
        break;

      case MSC_READ_CAPACITY10:
        scsi_status = USBH_MSC_SCSI_ReadCapacity(phost,MSC_Handle->current_lun, &MSC_Handle->unit[MSC_Handle->current_lun].capacity) ;

        if(scsi_status == USBH_OK)
        {
          if(MSC_Handle->unit[MSC_Handle->current_lun].state_changed == 1)
          {
            PRINTF(PSTR("MSC Device capacity : %lu Bytes\n"),
              (int32_t)(MSC_Handle->unit[MSC_Handle->current_lun].capacity.block_nbr * MSC_Handle->unit[MSC_Handle->current_lun].capacity.block_size));
            PRINTF(PSTR("Block number : %lu\n"), (int32_t)(MSC_Handle->unit[MSC_Handle->current_lun].capacity.block_nbr));
            PRINTF(PSTR("Block Size   : %lu\n"), (int32_t)(MSC_Handle->unit[MSC_Handle->current_lun].capacity.block_size));
          }
          MSC_Handle->unit[MSC_Handle->current_lun].state = MSC_IDLE;
          MSC_Handle->unit[MSC_Handle->current_lun].error = MSC_OK;
          MSC_Handle->current_lun++;
        }
        else if( scsi_status == USBH_FAIL)
        {
          MSC_Handle->unit[MSC_Handle->current_lun].state = MSC_REQUEST_SENSE;
        }
        else if(scsi_status == USBH_UNRECOVERED_ERROR)
        {
          MSC_Handle->unit[MSC_Handle->current_lun].state = MSC_IDLE;
          MSC_Handle->unit[MSC_Handle->current_lun].error = MSC_ERROR;
        }
        break;

      case MSC_REQUEST_SENSE:
        scsi_status = USBH_MSC_SCSI_RequestSense(phost,  MSC_Handle->current_lun, &MSC_Handle->unit[MSC_Handle->current_lun].sense);

        if( scsi_status == USBH_OK)
        {
          if ((MSC_Handle->unit[MSC_Handle->current_lun].sense.key == SCSI_SENSE_KEY_UNIT_ATTENTION) ||
             (MSC_Handle->unit[MSC_Handle->current_lun].sense.key == SCSI_SENSE_KEY_NOT_READY) )
          {

            if ((phost->Timer - MSC_Handle->timer) < 10000)
            {
              MSC_Handle->unit[MSC_Handle->current_lun].state = MSC_TEST_UNIT_READY;
              break;
            }
          }

          PRINTF(PSTR("Sense Key  : %x\n"), MSC_Handle->unit[MSC_Handle->current_lun].sense.key);
          PRINTF(PSTR("Additional Sense Code : %x\n"), MSC_Handle->unit[MSC_Handle->current_lun].sense.asc);
          PRINTF(PSTR("Additional Sense Code Qualifier: %x\n"), MSC_Handle->unit[MSC_Handle->current_lun].sense.ascq);
          MSC_Handle->unit[MSC_Handle->current_lun].state = MSC_IDLE;
          MSC_Handle->current_lun++;
        }
        if( scsi_status == USBH_FAIL)
        {
          PRINTF(PSTR("MSC Device NOT ready\n"));
          MSC_Handle->unit[MSC_Handle->current_lun].state = MSC_UNRECOVERED_ERROR;
        }
        else if(scsi_status == USBH_UNRECOVERED_ERROR)
        {
          MSC_Handle->unit[MSC_Handle->current_lun].state = MSC_IDLE;
          MSC_Handle->unit[MSC_Handle->current_lun].error = MSC_ERROR;
        }
        break;

      case MSC_UNRECOVERED_ERROR:
        MSC_Handle->current_lun++;
        break;

      default:
        break;
      }

#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CLASS_EVENT, 0);
#endif
    }
    else
    {
      MSC_Handle->current_lun = 0;
    MSC_Handle->state = MSC_IDLE;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CLASS_EVENT, 0);
#endif
    phost->pUser(phost, HOST_USER_CLASS_ACTIVE);
    }
    break;

  case MSC_IDLE:
    error = USBH_OK;
    break;

  default:
    break;
  }
  return error;
}


/**
  * @brief  USBH_MSC_SOFProcess
  *         The function is for SOF state
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_MSC_SOFProcess(USBH_HandleTypeDef *phost)
{

  return USBH_OK;
}
/**
  * @brief  USBH_MSC_RdWrProcess
  *         The function is for managing state machine for MSC I/O Process
  * @param  phost: Host handle
  * @param  lun: logical Unit Number
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_MSC_RdWrProcess(USBH_HandleTypeDef *phost, uint8_t lun)
{
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  USBH_StatusTypeDef error = USBH_BUSY ;
  USBH_StatusTypeDef scsi_status = USBH_BUSY ;
  
  /* Switch MSC REQ state machine */
  switch (MSC_Handle->unit[lun].state)
  {

  case MSC_READ:
    scsi_status = USBH_MSC_SCSI_Read(phost,lun, 0, NULL, 0) ;
    
    if(scsi_status == USBH_OK)
    {
      MSC_Handle->unit[lun].state = MSC_IDLE;
      error = USBH_OK;
    }
    else if( scsi_status == USBH_FAIL)
    {
      MSC_Handle->unit[lun].state = MSC_REQUEST_SENSE;
    }
    else if(scsi_status == USBH_UNRECOVERED_ERROR)
    {
      MSC_Handle->unit[lun].state = MSC_UNRECOVERED_ERROR;
          error = USBH_FAIL;
    }
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CLASS_EVENT, 0);
#endif
    break;
    
  case MSC_WRITE:
    scsi_status = USBH_MSC_SCSI_Write(phost,lun, 0, NULL, 0) ;
    
    if(scsi_status == USBH_OK)
    {
        MSC_Handle->unit[lun].state = MSC_IDLE;
        error = USBH_OK;
    }
    else if( scsi_status == USBH_FAIL)
    {
      MSC_Handle->unit[lun].state = MSC_REQUEST_SENSE;
    }
    else if(scsi_status == USBH_UNRECOVERED_ERROR)
    {
      MSC_Handle->unit[lun].state = MSC_UNRECOVERED_ERROR;
          error = USBH_FAIL;
    }
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CLASS_EVENT, 0);
#endif
    break;

  case MSC_REQUEST_SENSE:
    scsi_status = USBH_MSC_SCSI_RequestSense(phost, lun, &MSC_Handle->unit[lun].sense);
    
    if( scsi_status == USBH_OK)
    {
      PRINTF(PSTR("Sense Key  : %x\n"), MSC_Handle->unit[lun].sense.key);
      PRINTF(PSTR("Additional Sense Code : %x\n"), MSC_Handle->unit[lun].sense.asc);
      PRINTF(PSTR("Additional Sense Code Qualifier: %x\n"), MSC_Handle->unit[lun].sense.ascq);
      MSC_Handle->unit[lun].state = MSC_IDLE;
      MSC_Handle->unit[lun].error = MSC_ERROR;

      error = USBH_FAIL;
    }
    if( scsi_status == USBH_FAIL)
    {
      PRINTF(PSTR("MSC Device NOT ready\n"));
    }
    else if(scsi_status == USBH_UNRECOVERED_ERROR)
    {
      MSC_Handle->unit[lun].state = MSC_UNRECOVERED_ERROR;
          error = USBH_FAIL;
    }
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CLASS_EVENT, 0);
#endif
    break;
    
  default:
    break;

  }
  return error;
}

/**
  * @brief  USBH_MSC_IsReady
  *         The function check if the MSC function is ready
  * @param  phost: Host handle
  * @retval USBH Status
  */
uint8_t  USBH_MSC_IsReady (USBH_HandleTypeDef *phost)
{
    MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;

  if (phost->gState == HOST_CLASS)
  {
    return (MSC_Handle->state == MSC_IDLE);
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  USBH_MSC_GetMaxLUN
  *         The function return the Max LUN supported
  * @param  phost: Host handle
  * @retval logical Unit Number supported
  */
int8_t  USBH_MSC_GetMaxLUN (USBH_HandleTypeDef *phost)
{
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;

  if ((phost->gState == HOST_CLASS) && (MSC_Handle->state == MSC_IDLE))
  {
    return  MSC_Handle->max_lun;
  }
  return 0xFF;
}

/**
  * @brief  USBH_MSC_UnitIsReady
  *         The function check whether a LUN is ready
  * @param  phost: Host handle
  * @param  lun: logical Unit Number
  * @retval Lun status (0: not ready / 1: ready)
  */
uint8_t  USBH_MSC_UnitIsReady (USBH_HandleTypeDef *phost, uint8_t lun)
{
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  
  if (phost->gState == HOST_CLASS)
  {
    return (MSC_Handle->unit[lun].error == MSC_OK);
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  USBH_MSC_GetLUNInfo
  *         The function return a LUN information
  * @param  phost: Host handle
  * @param  lun: logical Unit Number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_GetLUNInfo(USBH_HandleTypeDef *phost, uint8_t lun, MSC_LUNTypeDef *info)
{
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  if (phost->gState == HOST_CLASS)
  {
    USBH_memcpy(info,&MSC_Handle->unit[lun], sizeof(MSC_LUNTypeDef));
    return USBH_OK;
  }
  else
  {
    return USBH_FAIL;
  }
}

/**
  * @brief  USBH_MSC_Read
  *         The function performs a Read operation
  * @param  phost: Host handle
  * @param  lun: logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to read
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_Read(USBH_HandleTypeDef *phost,
                                     uint8_t lun,
                                     uint32_t address,
                                     uint8_t *pbuf,
                                     uint32_t length)
{
  uint32_t timeout;
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  
  if ((phost->device.is_connected == 0) ||
      (phost->gState != HOST_CLASS) ||
      (MSC_Handle->unit[lun].state != MSC_IDLE))
  {
    return  USBH_FAIL;
  }
  MSC_Handle->state = MSC_READ;
  MSC_Handle->unit[lun].state = MSC_READ;
  MSC_Handle->rw_lun = lun;
  USBH_MSC_SCSI_Read(phost,
                     lun,
                     address,
                     pbuf,
                     length);

  timeout = phost->Timer;

  while (USBH_MSC_RdWrProcess(phost, lun) == USBH_BUSY)
  {
    if (((phost->Timer - timeout) > (10000 * length)) || (phost->device.is_connected == 0))
    {
      MSC_Handle->state = MSC_IDLE;
      return USBH_FAIL;
    }
  }
  MSC_Handle->state = MSC_IDLE;
  return USBH_OK;
}

/**
  * @brief  USBH_MSC_Write
  *         The function performs a Write operation
  * @param  phost: Host handle
  * @param  lun: logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to write
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_Write(USBH_HandleTypeDef *phost,
                                     uint8_t lun,
                                     uint32_t address,
                                     uint8_t *pbuf,
                                     uint32_t length)
{
  uint32_t timeout;
  MSC_HandleTypeDef *MSC_Handle = (MSC_HandleTypeDef *) phost->pActiveClass->pData;
  
  if ((phost->device.is_connected == 0) ||
      (phost->gState != HOST_CLASS) ||
      (MSC_Handle->unit[lun].state != MSC_IDLE))
  {
    return  USBH_FAIL;
  }
  MSC_Handle->state = MSC_WRITE;
  MSC_Handle->unit[lun].state = MSC_WRITE;
  MSC_Handle->rw_lun = lun;
  USBH_MSC_SCSI_Write(phost,
                     lun,
                     address,
                     pbuf,
                     length);

  timeout = phost->Timer;
  while (USBH_MSC_RdWrProcess(phost, lun) == USBH_BUSY)
  {
    if (((phost->Timer - timeout) >  (10000 * length)) || (phost->device.is_connected == 0))
    {
      MSC_Handle->state = MSC_IDLE;
      return USBH_FAIL;
    }
  }
  MSC_Handle->state = MSC_IDLE;
  return USBH_OK;
}

#endif /* WITHUSEUSBFLASH */
/**
  * @brief  DeInitializes PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *pcdHandle)
{
#if CPUSTYLE_R7S721

	pcdHandle->Instance->SYSCFG0 &= ~ USB_SYSCFG_USBE;
	pcdHandle->Instance->INTENB0 = 0;
	pcdHandle->Instance->INTENB1 = 0;

#elif CPUSTYLE_STM32H7XX

	  if (pcdHandle->Instance == USB1_OTG_HS)
	  {
	    /* Peripheral interrupt Deinit*/
	    NVIC_DisableIRQ(OTG_HS_IRQn);
	  }
	  else if (pcdHandle->Instance == USB2_OTG_FS)
	  {
	    /* Peripheral interrupt Deinit*/
	    NVIC_DisableIRQ(OTG_FS_IRQn);
	  }

#elif CPUSTYLE_STM32

#if defined (USB_OTG_HS)
	  if (pcdHandle->Instance == USB_OTG_HS)
	  {
	    /* Peripheral interrupt Deinit*/
	    NVIC_DisableIRQ(OTG_HS_IRQn);
	  }
#endif /* defined (USB_OTG_HS) */
	  if (pcdHandle->Instance == USB_OTG_FS)
	  {
	    /* Peripheral interrupt Deinit*/
	    NVIC_DisableIRQ(OTG_FS_IRQn);
	  }

#else
	#error HAL_PCD_MspDeInit should be implemented
#endif
}


/**
  * @brief  Initializes the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
static USBD_StatusTypeDef  USBD_LL_Init(PCD_HandleTypeDef * hpcd, USBD_HandleTypeDef *pdev)
{
	/* Link The driver to the stack */
	hpcd->pData = pdev;
	pdev->pData = hpcd;

	hpcd->Instance = WITHUSBHW_DEVICE;

#if CPUSTYLE_R7S721

	#if WITHUSBDEV_HSDESC
		hpcd->Init.speed = PCD_SPEED_HIGH;
	#else /* WITHUSBDEV_HSDESC */
		hpcd->Init.speed = PCD_SPEED_FULL;
	#endif /* WITHUSBDEV_HSDESC */
	hpcd->Init.phy_itface = USB_OTG_EMBEDDED_PHY;

#else /* CPUSTYLE_R7S721 */

	#if WITHUSBDEV_HSDESC
		hpcd->Init.speed = PCD_SPEED_HIGH;
	#else /* WITHUSBDEV_HSDESC */
		hpcd->Init.speed = PCD_SPEED_FULL;
	#endif /* WITHUSBDEV_HSDESC */

	#if WITHUSBDEV_HIGHSPEEDULPI
		hpcd->Init.phy_itface = USB_OTG_ULPI_PHY;
	#elif WITHUSBDEV_HIGHSPEEDPHYC
		hpcd->Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
	#else /* WITHUSBDEV_HIGHSPEEDULPI */
		hpcd->Init.phy_itface = USB_OTG_EMBEDDED_PHY;
	#endif /* WITHUSBDEV_HIGHSPEEDULPI */

#endif /* CPUSTYLE_R7S721 */

	#if WITHUSBDEV_VBUSSENSE	/* используется предопределенный вывод VBUS_SENSE */
		hpcd->Init.vbus_sensing_enable = USB_ENABLE;
	#else /* WITHUSBDEV_VBUSSENSE */
		hpcd->Init.vbus_sensing_enable = USB_DISABLE;
	#endif /* WITHUSBDEV_VBUSSENSE */

	hpcd->Init.dev_endpoints = 9;	// or 6 for FS devices
	hpcd->Init.dma_enable = ! USB_ENABLE; // xyz
	hpcd->Init.ep0_mps = DEP0CTL_MPS_64;
	hpcd->Init.Sof_enable = USB_DISABLE;
	hpcd->Init.low_power_enable = USB_DISABLE;
	hpcd->Init.lpm_enable = USB_DISABLE;
	hpcd->Init.battery_charging_enable = USB_ENABLE;
	hpcd->Init.use_dedicated_ep1 = ! USB_ENABLE; // xyz
	hpcd->Init.use_external_vbus = USB_DISABLE;

	if (HAL_PCD_Init(hpcd) != HAL_OK)
	{
		ASSERT(0);
	}

#if CPUSTYLE_R7S721
	usbd_pipes_initialize(hpcd);
#elif CPUSTYLE_STM32H7XX
	// У OTH_HS размер FIFO 4096 байт
	usbd_fifo_initialize(hpcd, 4096, 1);
#else /* CPUSTYLE_R7S721 */

#if defined (USB_OTG_HS)
	if (hpcd->Instance == USB_OTG_HS)
	{
		// У OTH_HS размер FIFO 4096 байт
		usbd_fifo_initialize(hpcd, 4096, 1);
	}
	else
#endif /* defined (USB_OTG_HS) */
	{
		// У OTH_FS размер FIFO 1280 байт
		usbd_fifo_initialize(hpcd, 1280, 1);
	}
	// У OTH_HS размер FIFO 4096 байт
	usbd_fifo_initialize(hpcd, 4096, 1);
#endif /* CPUSTYLE_R7S721 */

	return USBD_OK;
}

/**
  * @brief  De-Initializes the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
static USBD_StatusTypeDef  USBD_LL_DeInit (USBD_HandleTypeDef *pdev)
{
  HAL_PCD_DeInit((PCD_HandleTypeDef*)pdev->pData);
  return USBD_OK;
}

/**
* @brief  USBD_Init2
*         Initializes the device stack and load the class driver
* @param  pdev: device instance
* @param  pdesc: Descriptor structure address
* @param  id: Low level core index
* @retval None
*/
static USBD_StatusTypeDef USBD_Init2(USBD_HandleTypeDef *pdev)
{
	/* Check whether the USB Host handle is valid */
	if(pdev == NULL)
	{
		//USBD_ErrLog("Invalid Device handle");
		return USBD_FAIL;
	}
	pdev->nClasses = 0;


	/* Set Device initial State */
	pdev->dev_state  = USBD_STATE_DEFAULT;
	//pdev->id = id;
	/* Initialize low level driver */
	/* Init Device Library,Add Supported Class and Start the library*/
	USBD_LL_Init(& hpcd_USB_OTG, pdev);

	return USBD_OK;
}

/**
* @brief  USBD_DeInit
*         Re-Initialize th device library
* @param  pdev: device instances
* @retval status: status
*/
USBD_StatusTypeDef USBD_DeInit(USBD_HandleTypeDef *pdev)
{
	uint_fast8_t di;
	/* Set Default State */
	pdev->dev_state  = USBD_STATE_DEFAULT;

	/* Free Class Resources */
	for (di = 0; di < pdev->nClasses; ++ di)
	{
		  /* for each device function */
		  const USBD_ClassTypeDef * const pClass = pdev->pClasses [di];
		  pClass->DeInit(pdev, pdev->dev_config [0]);
	}

	/* Stop the low level driver  */
	USBD_LL_Stop(pdev);

	/* Initialize low level driver */
	USBD_LL_DeInit(pdev);

	return USBD_OK;
}

/* вызывается при запрещённых прерываниях. */
static void hardware_usbd_initialize(void)
{
#if WITHUSBDEV_HSDESC
	usbd_descriptors_initialize(1);
#else /* WITHUSBDEV_HSDESC */
	usbd_descriptors_initialize(0);
#endif /* WITHUSBDEV_HSDESC */


	USBD_Init2(& hUsbDevice);
#if WITHUSBUAC
	USBD_AddClass(& hUsbDevice, & USBD_CLASS_UAC);
#endif /* WITHUSBUAC */
#if WITHUSBCDC
	USBD_AddClass(& hUsbDevice, & USBD_CLASS_CDC);
#endif /* WITHUSBCDC */
#if WITHUSBDFU
	USBD_AddClass(& hUsbDevice, & USBD_CLASS_DFU);
#endif /* WITHUSBDFU */
}

/* вызывается при запрещённых прерываниях. */
static void board_usbd_initialize(void)
{

	hardware_usbd_initialize();
	hardware_usbd_dma_initialize();
	hardware_usbd_dma_enable();
}


#if CPUSTYLE_STM32
/**
  * @brief  Initiate Do Ping protocol
  * @param  USBx : Selected device
  * @param  hc_num : Host Channel number
  *         This parameter can be a value from 1 to 15
  * @retval HAL state
  */
HAL_StatusTypeDef USB_DoPing(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t ch_num)
{
  uint8_t  num_packets = 1;
  uint32_t tmpreg = 0;

  USBx_HC(ch_num)->HCTSIZ = ((num_packets << USB_OTG_HCTSIZ_PKTCNT_Pos) & USB_OTG_HCTSIZ_PKTCNT) |
                                USB_OTG_HCTSIZ_DOPING;

  /* Set host channel enable */
  tmpreg = USBx_HC(ch_num)->HCCHAR;
  tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
  tmpreg |= USB_OTG_HCCHAR_CHENA;
  USBx_HC(ch_num)->HCCHAR = tmpreg;

  return HAL_OK;
}

/**
  * @brief  Stop Host Core
  * @param  USBx : Selected device
  * @retval HAL state
  */
HAL_StatusTypeDef USB_StopHost(USB_OTG_GlobalTypeDef *USBx)
{
  uint8_t i;
  uint32_t count = 0;
  uint32_t value;

  USB_DisableGlobalInt(USBx);

    /* Flush FIFO */
  USB_FlushTxFifoAll(USBx);
  USB_FlushRxFifo(USBx);

  /* Flush out any leftover queued requests. */
  for (i = 0; i <= 15; i++)
  {

    value = USBx_HC(i)->HCCHAR ;
    value |=  USB_OTG_HCCHAR_CHDIS;
    value &= ~USB_OTG_HCCHAR_CHENA;
    value &= ~USB_OTG_HCCHAR_EPDIR;
    USBx_HC(i)->HCCHAR = value;
  }

  /* Halt all channels to put them into a known state. */
  for (i = 0; i <= 15; i++)
  {

    value = USBx_HC(i)->HCCHAR ;

    value |= USB_OTG_HCCHAR_CHDIS;
    value |= USB_OTG_HCCHAR_CHENA;
    value &= ~USB_OTG_HCCHAR_EPDIR;

    USBx_HC(i)->HCCHAR = value;
    do
    {
      if (++count > 1000)
      {
        break;
      }
    }
    while ((USBx_HC(i)->HCCHAR & USB_OTG_HCCHAR_CHENA) == USB_OTG_HCCHAR_CHENA);
  }

  /* Clear any pending Host interrupts */
  USBx_HOST->HAINT = 0xFFFFFFFF;
  USBx->GINTSTS = 0xFFFFFFFF;

  USB_EnableGlobalInt(USBx);
  return HAL_OK;
}

#endif /* CPUSTYLE_STM32 */

/**
  * @brief  Start the host driver
  * @param  hhcd: HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_Start(HCD_HandleTypeDef *hhcd)
{
  __HAL_LOCK(hhcd);
  __HAL_HCD_ENABLE(hhcd);
  USB_DriveVbus(hhcd->Instance, 1);
  __HAL_UNLOCK(hhcd);
  return HAL_OK;
}

/**
  * @brief  Stop the host driver
  * @param  hhcd: HCD handle
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_HCD_Stop(HCD_HandleTypeDef *hhcd)
{
  __HAL_LOCK(hhcd);
  USB_StopHost(hhcd->Instance);
  __HAL_UNLOCK(hhcd);
  return HAL_OK;
}

/**
  * @brief  Reset the host port
  * @param  hhcd: HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_ResetPort(HCD_HandleTypeDef *hhcd, uint_fast8_t status)
{
	USB_ResetPort(hhcd->Instance, status);
	return HAL_OK;
}

/**
  * @brief  Return the HCD state
  * @param  hhcd: HCD handle
  * @retval HAL state
  */
HCD_StateTypeDef HAL_HCD_GetState(HCD_HandleTypeDef *hhcd)
{
  return hhcd->State;
}


/**
  * @brief  Return the Host enumeration speed
  * @param  hhcd: HCD handle
  * @retval Enumeration speed
  */
uint32_t HAL_HCD_GetCurrentSpeed(HCD_HandleTypeDef *hhcd)
{
  return (USB_GetHostSpeed(hhcd->Instance));
}

/**
  * @brief  USBH_LL_Start
  *         Start the Low Level portion of the Host driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_LL_Start(USBH_HandleTypeDef *phost)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  hal_status = HAL_HCD_Start(phost->pData);

  switch (hal_status) {
    case HAL_OK :
      usb_status = USBH_OK;
    break;
    case HAL_ERROR :
      usb_status = USBH_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBH_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBH_FAIL;
    break;
    default :
      usb_status = USBH_FAIL;
    break;
  }
  return usb_status;
}

/**
  * @brief  USBH_LL_Stop
  *         Stop the Low Level portion of the Host driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_LL_Stop (USBH_HandleTypeDef *phost)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  hal_status = HAL_HCD_Stop(phost->pData);

  switch (hal_status) {
    case HAL_OK :
      usb_status = USBH_OK;
    break;
    case HAL_ERROR :
      usb_status = USBH_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBH_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBH_FAIL;
    break;
    default :
      usb_status = USBH_FAIL;
    break;
  }
  return usb_status;
}

/**
  * @brief  USBH_LL_GetSpeed
  *         Return the USB Host Speed from the Low Level Driver.
  * @param  phost: Host handle
  * @retval USBH Speeds
  */
USBH_SpeedTypeDef USBH_LL_GetSpeed  (USBH_HandleTypeDef *phost)
{
	USBH_SpeedTypeDef speed;

	switch (HAL_HCD_GetCurrentSpeed(phost->pData))
	{
	case USB_OTG_SPEED_HIGH:
		speed = USBH_SPEED_HIGH;
		break;

	case USB_OTG_SPEED_HIGH_IN_FULL:
		speed = USBH_SPEED_FULL;
		break;

	case USB_OTG_SPEED_LOW:
		speed = USBH_SPEED_LOW;
		break;

	default:
	case USB_OTG_SPEED_FULL:
		speed = USBH_SPEED_FULL;
		break;
	}
	PRINTF(PSTR("USBH_LL_GetSpeed: (high=0, full=1, low=2) speed=%d\n"), (int) speed);
	return speed;
}

/**
  * @brief  USBH_LL_ResetPort
  *         Reset the Host Port of the Low Level Driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_ResetPort (USBH_HandleTypeDef *phost, uint_fast8_t status)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  hal_status = HAL_HCD_ResetPort(phost->pData, status);
  switch (hal_status) {
    case HAL_OK :
      usb_status = USBH_OK;
    break;
    case HAL_ERROR :
      usb_status = USBH_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBH_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBH_FAIL;
    break;
    default :
      usb_status = USBH_FAIL;
    break;
  }
  return usb_status;
}

/**
  * @brief  USBH_LL_DriverVBUS
  *         Drive VBUS.
  * @param  phost: Host handle
  * @param  state : VBUS state
  *          This parameter can be one of the these values:
  *           1 : VBUS Active
  *           0 : VBUS Inactive
  * @retval Status
  */
USBH_StatusTypeDef  USBH_LL_DriverVBUS(USBH_HandleTypeDef *phost, uint_fast8_t state)
{
	//PRINTF(PSTR("USBH_LL_DriverVBUS(%d), phost->id=%d, HOST_FS=%d\n"), (int) state, (int) phost->id, (int) HOST_FS);
	if (phost->id == HOST_FS) // compare to WITHUSBHW_HOST
	{
		if (state != USB_FALSE)
		{
			/* Drive high Charge pump */
			/* ToDo: Add IOE driver control */
			board_set_usbflashpoweron(1);
			board_update();
		}
		else
		{
			/* Drive low Charge pump */
			/* ToDo: Add IOE driver control */
			board_set_usbflashpoweron(0);
			board_update();
		}
	}
	HARDWARE_DELAY_MS(200);
	return USBH_OK;
}

#if CPUSTYLE_STM32

/**
  * @brief  Initializes the PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
#if CPUSTYLE_STM32H7XX

	//PWR->CR3 |= PWR_CR3_USBREGEN;

	//while ((PWR->CR3 & PWR_CR3_USB33RDY) == 0)
	//	;
	//PWR->CR3 |= PWR_CR3_USBREGEN;
	//while ((PWR->CR3 & PWR_CR3_USB33RDY) == 0)
	//	;
	PWR->CR3 |= PWR_CR3_USB33DEN;

	if (hpcd->Instance == USB1_OTG_HS)	// legacy name is USB_OTG_HS
	{
		if (hpcd->Init.phy_itface == USB_OTG_ULPI_PHY)
		{
			USBD_HS_ULPI_INITIALIZE();
			RCC->AHB1ENR |= RCC_AHB1ENR_USB1OTGHSEN | RCC_AHB1ENR_USB1OTGHSULPIEN;	/* USB/OTG HS with ULPI */
			(void) RCC->AHB1ENR;
			RCC->AHB1LPENR |= RCC_AHB1LPENR_USB1OTGHSLPEN; /* USB/OTG HS  */
			(void) RCC->AHB1LPENR;
			RCC->AHB1LPENR |= RCC_AHB1LPENR_USB1OTGHSULPILPEN; /* USB/OTG HS ULPI  */
			(void) RCC->AHB1LPENR;
		}
		else
		{
			USBD_HS_FS_INITIALIZE();

			PRINTF(PSTR("HAL_PCD_MspInitEx: HS without ULPI\n"));

			RCC->AHB1ENR |= RCC_AHB1ENR_USB1OTGHSEN; /* USB/OTG HS  */
			(void) RCC->AHB1ENR;
			RCC->AHB1LPENR |= RCC_AHB1LPENR_USB1OTGHSLPEN; /* USB/OTG HS  */
			(void) RCC->AHB1LPENR;
			RCC->AHB1LPENR &= ~ RCC_AHB1LPENR_USB1OTGHSULPILPEN; /* USB/OTG HS ULPI  */
			(void) RCC->AHB1LPENR;
		}
		//RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;	/* USB/OTG HS companion - VBUS? */
		//(void) RCC->APB4ENR;
		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM1EN;
		(void) RCC->AHB2ENR;
		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM2EN;
		(void) RCC->AHB2ENR;
		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM3EN;
		(void) RCC->AHB2ENR;


		if (hpcd->Init.use_dedicated_ep1 == USB_ENABLE)
		{
			NVIC_SetVector(OTG_HS_EP1_OUT_IRQn, (uintptr_t) & device_OTG_HS_EP1_OUT_IRQHandler);
			NVIC_SetPriority(OTG_HS_EP1_OUT_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(OTG_HS_EP1_OUT_IRQn);	// OTG_HS_EP1_OUT_IRQHandler() enable

			NVIC_SetVector(OTG_HS_EP1_IN_IRQn, (uintptr_t) & device_OTG_HS_EP1_IN_IRQHandler);
			NVIC_SetPriority(OTG_HS_EP1_IN_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(OTG_HS_EP1_IN_IRQn);	// OTG_HS_EP1_IN_IRQHandler() enable
		}
		NVIC_SetVector(OTG_HS_IRQn, (uintptr_t) & device_OTG_HS_IRQHandler);
		NVIC_SetPriority(OTG_HS_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(OTG_HS_IRQn);	// OTG_HS_IRQHandler() enable

	}
	else if (hpcd->Instance == USB2_OTG_FS)	// legacy name is USB_OTG_FS
	{
		if (hpcd->Init.phy_itface == USB_OTG_ULPI_PHY)
		{
			USBD_FS_INITIALIZE();
			RCC->AHB1ENR |= RCC_AHB1ENR_USB2OTGHSEN | RCC_AHB1ENR_USB2OTGHSULPIEN;	/* USB/OTG HS with ULPI */
			(void) RCC->AHB1ENR;
		}
		else
		{
			USBD_FS_INITIALIZE();
			RCC->AHB1ENR |= RCC_AHB1ENR_USB2OTGHSEN;	/* USB/OTG HS  */
			(void) RCC->AHB1ENR;
		}
		//RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;	/* USB/OTG FS companion - VBUS? */
		//(void) RCC->APB4ENR;
		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM1EN;
		(void) RCC->AHB2ENR;
		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM2EN;
		(void) RCC->AHB2ENR;
		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM3EN;
		(void) RCC->AHB2ENR;

		NVIC_SetVector(OTG_FS_IRQn, (uintptr_t) & device_OTG_FS_IRQHandler);
		NVIC_SetPriority(OTG_FS_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(OTG_FS_IRQn);	// OTG_FS_IRQHandler() enable

	}

#elif defined (STM32F40_41xxx)

	//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
	//PRINTF(PSTR("HAL_PCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);

	USBD_FS_INITIALIZE();
	RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;	/* USB/OTG FS  */
	(void) RCC->AHB2ENR;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;	/* USB/OTG FS companion - VBUS? */
	(void) RCC->APB2ENR;

	NVIC_SetVector(OTG_FS_IRQn, (uintptr_t) & device_OTG_FS_IRQHandler);
	NVIC_SetPriority(OTG_FS_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(OTG_FS_IRQn);	// OTG_FS_IRQHandler() enable

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	if (hpcd->Instance == USB_OTG_HS)
	{
		//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
		//PRINTF(PSTR("HAL_PCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);

		if (hpcd->Init.phy_itface == USB_OTG_ULPI_PHY)
		{
			USBD_HS_ULPI_INITIALIZE();

			PRINTF(PSTR("HAL_PCD_MspInit: HS and ULPI\n"));
			RCC->AHB1ENR |= RCC_AHB1ENR_OTGHSEN;		/* USB/OTG HS  */
			(void) RCC->AHB1ENR;
			RCC->AHB1LPENR |= RCC_AHB1LPENR_OTGHSLPEN;		/* USB/OTG HS  */
			(void) RCC->AHB1LPENR;
			RCC->AHB1ENR |= RCC_AHB1ENR_OTGHSULPIEN;		/* USB/OTG HS with ULPI */
			(void) RCC->AHB1ENR;
			RCC->AHB1LPENR |= RCC_AHB1LPENR_OTGHSULPILPEN;	/* USB/OTG HS  */
			(void) RCC->AHB1LPENR;
		}
		else
		{
			USBD_HS_FS_INITIALIZE();

			PRINTF(PSTR("HAL_PCD_MspInit: HS without ULPI\n"));
			RCC->AHB1ENR |= RCC_AHB1ENR_OTGHSEN;	/* USB/OTG HS  */
			(void) RCC->AHB1ENR;
			RCC->AHB1LPENR |= RCC_AHB1LPENR_OTGHSLPEN; /* USB/OTG HS  */
			(void) RCC->AHB1LPENR;
			RCC->AHB1LPENR &= ~ RCC_AHB1LPENR_OTGHSULPILPEN; /* USB/OTG HS ULPI  */
			(void) RCC->AHB1LPENR;
		}

		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;	/* USB/OTG HS companion - VBUS? */
		(void) RCC->APB2ENR;

		if (hpcd->Init.use_dedicated_ep1 == USB_ENABLE)
		{
			NVIC_SetVector(OTG_HS_EP1_OUT_IRQn, (uintptr_t) & device_OTG_HS_EP1_OUT_IRQHandler);
			NVIC_SetPriority(OTG_HS_EP1_OUT_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(OTG_HS_EP1_OUT_IRQn);	// OTG_HS_EP1_OUT_IRQHandler() enable

			NVIC_SetVector(OTG_HS_EP1_IN_IRQn, (uintptr_t) & device_OTG_HS_EP1_IN_IRQHandler);
			NVIC_SetPriority(OTG_HS_EP1_IN_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(OTG_HS_EP1_IN_IRQn);	// OTG_HS_EP1_IN_IRQHandler() enable
		}
		NVIC_SetVector(OTG_HS_IRQn, (uintptr_t) & device_OTG_HS_IRQHandler);
		NVIC_SetPriority(OTG_HS_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(OTG_HS_IRQn);	// OTG_HS_IRQHandler() enable

	}
	else if (hpcd->Instance == USB_OTG_FS)
	{
		//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
		//PRINTF(PSTR("HAL_PCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);

		USBD_FS_INITIALIZE();
		RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;	/* USB/OTG FS  */
		(void) RCC->AHB2ENR;
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;	/* USB/OTG FS companion - VBUS? */
		(void) RCC->APB2ENR;

		NVIC_SetVector(OTG_FS_IRQn, (uintptr_t) & device_OTG_FS_IRQHandler);
		NVIC_SetPriority(OTG_FS_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(OTG_FS_IRQn);	// OTG_FS_IRQHandler() enable

	}

#endif
}

/* MSP Init */

void HAL_HCD_MspInit(HCD_HandleTypeDef* hcdHandle)
{
	PRINTF(PSTR("HAL_HCD_MspInit()\n"));
	if (hcdHandle->Instance == USB_OTG_FS)
	{
		#if CPUSTYLE_STM32H7XX

			//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
			//PRINTF(PSTR("HAL_HCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);
			USBD_FS_INITIALIZE();

			RCC->AHB1ENR |= RCC_AHB1ENR_USB1OTGHSEN;	/* USB/OTG HS  */
			(void) RCC->AHB1ENR;
			RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;	/* USB/OTG HS companion - VBUS? */
			(void) RCC->APB2ENR;

			NVIC_SetVector(OTG_FS_IRQn, (uintptr_t) & host_OTG_FS_IRQHandler);
			NVIC_SetPriority(OTG_FS_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(OTG_FS_IRQn);	// OTG_FS_IRQHandler() enable

		#else
			//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
			//PRINTF(PSTR("HAL_HCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);

			USBD_FS_INITIALIZE();

			RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;	/* USB/OTG FS  */
			(void) RCC->AHB2ENR;
			RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;	/* USB/OTG FS companion - VBUS? */
			(void) RCC->APB2ENR;

			NVIC_SetVector(OTG_FS_IRQn, (uintptr_t) & host_OTG_FS_IRQHandler);
			NVIC_SetPriority(OTG_FS_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(OTG_FS_IRQn);	// OTG_FS_IRQHandler() enable

		#endif
	}
	else
	{
		ASSERT(0);
	}
}

void HAL_HCD_MspDeInit(HCD_HandleTypeDef* hpcd)
{
#if defined (USB_OTG_HS)
  if (hpcd->Instance == USB_OTG_HS)
  {
    /* Peripheral interrupt Deinit*/
    NVIC_DisableIRQ(OTG_HS_IRQn);
  }
#endif /* defined (USB_OTG_HS) */
  if (hpcd->Instance == USB_OTG_FS)
  {
	    /* Peripheral interrupt Deinit*/
	    NVIC_DisableIRQ(OTG_FS_IRQn);
  }
}

/**
  * @brief  Return Host Current Frame number
  * @param  USBx : Selected device
  * @retval current frame number
*/
uint32_t USB_GetCurrentFrame (USB_OTG_GlobalTypeDef *USBx)
{
	return (USBx_HOST->HFNUM & USB_OTG_HFNUM_FRNUM);
}


#elif CPUSTYLE_R7S721
#endif /* CPUSTYLE_STM32 */
/**
  * @brief  USBH_LL_SetTimer
  *         Set the initial Host Timer tick
  * @param  phost: Host Handle
  * @retval None
  */
void  USBH_LL_SetTimer  (USBH_HandleTypeDef *phost, uint32_t time)
{
	phost->Timer = time;
}

/**
  * @brief  Return the current Host frame number
  * @param  hhcd: HCD handle
  * @retval Current Host frame number
  */
uint32_t HAL_HCD_GetCurrentFrame(HCD_HandleTypeDef *hhcd)
{
	return (USB_GetCurrentFrame(hhcd->Instance));
}


/**
  * @brief  Initialize the host driver
  * @param  hhcd: HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_Init(HCD_HandleTypeDef *hhcd)
{
	/* Check the HCD handle allocation */
	if(hhcd == NULL)
	{
		return HAL_ERROR;
	}

	/* Check the parameters */
	//assert_param(IS_HCD_ALL_INSTANCE(hhcd->Instance));

	hhcd->State = HAL_HCD_STATE_BUSY;

	/* Init the low level hardware : GPIO, CLOCK, NVIC... */
	HAL_HCD_MspInit(hhcd);

	/* Disable the Interrupts */
	__HAL_HCD_DISABLE(hhcd);

	/*Init the Core (common init.) */
	USB_CoreInit(hhcd->Instance, & hhcd->Init);

	/* Force Host Mode*/
	USB_SetCurrentMode(hhcd->Instance , USB_OTG_HOST_MODE);

	/* Init Host */
	USB_HostInit(hhcd->Instance, & hhcd->Init);

	hhcd->State= HAL_HCD_STATE_READY;

	return HAL_OK;
}


/**
  * @brief  Initialize a host channel
  * @param  hhcd: HCD handle
  * @param  ch_num: Channel number.
  *         This parameter can be a value from 1 to 15
  * @param  epnum: Endpoint number.
  *          This parameter can be a value from 1 to 15
  * @param  dev_address : Current device address
  *          This parameter can be a value from 0 to 255
  * @param  speed: Current device speed.
  *          This parameter can be one of these values:
  *            HCD_SPEED_HIGH: High speed mode,
  *            HCD_SPEED_FULL: Full speed mode,
  *            HCD_SPEED_LOW: Low speed mode
  * @param  ep_type: Endpoint Type.
  *          This parameter can be one of these values:
  *            USBD_EP_TYPE_CTRL: Control type,
  *            USBD_EP_TYPE_ISOC: Isochronous type,
  *            USBD_EP_TYPE_BULK: Bulk type,
  *            USBD_EP_TYPE_INTR: Interrupt type
  * @param  mps: Max Packet Size.
  *          This parameter can be a value from 0 to32K
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_HC_Init(HCD_HandleTypeDef *hhcd,
                                  uint8_t ch_num,
                                  uint8_t epnum,
                                  uint8_t dev_address,
                                  uint8_t speed,
                                  uint8_t ep_type,
                                  uint16_t mps)
{
  HAL_StatusTypeDef status = HAL_OK;

  __HAL_LOCK(hhcd);

  hhcd->hc[ch_num].dev_addr = dev_address;
  hhcd->hc[ch_num].max_packet = mps;
  hhcd->hc[ch_num].ch_num = ch_num;
  hhcd->hc[ch_num].ep_type = ep_type;
  hhcd->hc[ch_num].ep_num = epnum & 0x7F;
  hhcd->hc[ch_num].ep_is_in = ((epnum & 0x80) == 0x80);
  hhcd->hc[ch_num].speed = speed;

  status = USB_HC_Init(hhcd->Instance,
                        ch_num,
                        epnum,
                        dev_address,
                        speed,
                        ep_type,
                        mps);
  __HAL_UNLOCK(hhcd);

  return status;
}

/**
  * @brief  Halt a host channel
  * @param  hhcd: HCD handle
  * @param  ch_num: Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_HC_Halt(HCD_HandleTypeDef *hhcd, uint8_t ch_num)
{
  HAL_StatusTypeDef status = HAL_OK;

  __HAL_LOCK(hhcd);
  USB_HC_Halt(hhcd->Instance, ch_num);
  __HAL_UNLOCK(hhcd);

  return status;
}
// HAL_HCD_HC_xxxxx

/**
  * @brief  Return  URB state for a channel
  * @param  hhcd: HCD handle
  * @param  chnum: Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval URB state.
  *          This parameter can be one of these values:
  *            URB_IDLE/
  *            URB_DONE/
  *            URB_NOTREADY/
  *            URB_NYET/
  *            URB_ERROR/
  *            URB_STALL/
  */
HCD_URBStateTypeDef HAL_HCD_HC_GetURBState(HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  return hhcd->hc[chnum].urb_state;
}


/**
  * @brief  Return the last host transfer size
  * @param  hhcd: HCD handle
  * @param  chnum: Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval last transfer size in byte
  */
uint32_t HAL_HCD_HC_GetXferCount(HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  return hhcd->hc[chnum].xfer_count;
}

/**
  * @brief  Return the Host Channel state
  * @param  hhcd: HCD handle
  * @param  chnum: Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval Host channel state
  *          This parameter can be one of the these values:
  *            HC_IDLE/
  *            HC_XFRC/
  *            HC_HALTED/
  *            HC_NYET/
  *            HC_NAK/
  *            HC_STALL/
  *            HC_XACTERR/
  *            HC_BBLERR/
  *            HC_DATATGLERR/
  */
HCD_HCStateTypeDef  HAL_HCD_HC_GetState(HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  return hhcd->hc[chnum].state;
}

/**
  * @brief  Submit a new URB for processing
  * @param  hhcd: HCD handle
  * @param  ch_num: Channel number.
  *         This parameter can be a value from 1 to 15
  * @param  direction: Channel number.
  *          This parameter can be one of these values:
  *           0 : Output / 1 : Input
  * @param  ep_type: Endpoint Type.
  *          This parameter can be one of these values:
  *            USBD_EP_TYPE_CTRL: Control type/
  *            USBD_EP_TYPE_ISOC: Isochronous type/
  *            USBD_EP_TYPE_BULK: Bulk type/
  *            USBD_EP_TYPE_INTR: Interrupt type/
  * @param  token: Endpoint Type.
  *          This parameter can be one of these values:
  *            0: HC_PID_SETUP / 1: HC_PID_DATA1
  * @param  pbuff: pointer to URB data
  * @param  length: Length of URB data
  * @param  do_ping: activate do ping protocol (for high speed only).
  *          This parameter can be one of these values:
  *           0 : do ping inactive / 1 : do ping active
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_HC_SubmitRequest(HCD_HandleTypeDef *hhcd,
                                            uint8_t ch_num,
                                            uint8_t direction ,
                                            uint8_t ep_type,
                                            uint8_t token,
                                            uint8_t* pbuff,
                                            uint16_t length,
                                            uint8_t do_ping)
{
  hhcd->hc[ch_num].ep_is_in = direction;
  hhcd->hc[ch_num].ep_type  = ep_type;

  if(token == 0)
  {
    hhcd->hc[ch_num].data_pid = HC_PID_SETUP;
  }
  else
  {
    hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
  }

  /* Manage Data Toggle */
  switch(ep_type)
  {
  case USBD_EP_TYPE_CTRL:
    if ((token == 1) && (direction == 0)) /*send data */
    {
      if (length == 0 )
      { /* For Status OUT stage, Length==0, Status Out PID = 1 */
        hhcd->hc[ch_num].toggle_out = 1;
      }

      /* Set the Data Toggle bit as per the Flag */
      if (hhcd->hc[ch_num].toggle_out == 0)
      { /* Put the PID 0 */
        hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
      }
      else
      { /* Put the PID 1 */
        hhcd->hc[ch_num].data_pid = HC_PID_DATA1 ;
      }
      if(hhcd->hc[ch_num].urb_state  != URB_NOTREADY)
      {
        hhcd->hc[ch_num].do_ping = do_ping;
      }
    }
    break;

  case USBD_EP_TYPE_BULK:
    if(direction == 0)
    {
      /* Set the Data Toggle bit as per the Flag */
      if (hhcd->hc[ch_num].toggle_out == 0)
      { /* Put the PID 0 */
        hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
      }
      else
      { /* Put the PID 1 */
        hhcd->hc[ch_num].data_pid = HC_PID_DATA1 ;
      }
      if(hhcd->hc[ch_num].urb_state  != URB_NOTREADY)
      {
        hhcd->hc[ch_num].do_ping = do_ping;
      }
    }
    else
    {
      if( hhcd->hc[ch_num].toggle_in == 0)
      {
        hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
      }
      else
      {
        hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
      }
    }

    break;
  case USBD_EP_TYPE_INTR:
    if(direction == 0)
    {
      /* Set the Data Toggle bit as per the Flag */
      if (hhcd->hc[ch_num].toggle_out == 0)
      { /* Put the PID 0 */
        hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
      }
      else
      { /* Put the PID 1 */
        hhcd->hc[ch_num].data_pid = HC_PID_DATA1 ;
      }
    }
    else
    {
      if( hhcd->hc[ch_num].toggle_in == 0)
      {
        hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
      }
      else
      {
        hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
      }
    }
    break;

  case USBD_EP_TYPE_ISOC:
    hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
    break;
  }

  hhcd->hc[ch_num].xfer_buff = pbuff;
  hhcd->hc[ch_num].xfer_len  = length;
  hhcd->hc[ch_num].urb_state =   URB_IDLE;
  hhcd->hc[ch_num].xfer_count = 0 ;
  hhcd->hc[ch_num].ch_num = ch_num;
  hhcd->hc[ch_num].state = HC_IDLE;

  return USB_HC_StartXfer(hhcd->Instance, &(hhcd->hc[ch_num]), hhcd->Init.dma_enable);
}



/**
  * @brief  DeInitialize the host driver
  * @param  hhcd: HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_DeInit(HCD_HandleTypeDef *hhcd)
{
  /* Check the HCD handle allocation */
  if(hhcd == NULL)
  {
    return HAL_ERROR;
  }

  hhcd->State = HAL_HCD_STATE_BUSY;

  /* DeInit the low level hardware */
  HAL_HCD_MspDeInit(hhcd);

   __HAL_HCD_DISABLE(hhcd);

  hhcd->State = HAL_HCD_STATE_RESET;

  return HAL_OK;
}

/**
  * @brief  USBH_LL_IncTimer
  *         Increment Host Timer tick
  * @param  phost: Host Handle
  * @retval None
  */
void  USBH_LL_IncTimer  (USBH_HandleTypeDef *phost)
{
  phost->Timer ++;
  USBH_HandleSof(phost);
}

/**
  * @brief  USBH_LL_DeInit
  *         De-Initialize the Low Level portion of the Host driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_LL_DeInit (USBH_HandleTypeDef *phost)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  hal_status = HAL_HCD_DeInit(phost->pData);

  switch (hal_status) {
    case HAL_OK :
      usb_status = USBH_OK;
    break;
    case HAL_ERROR :
      usb_status = USBH_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBH_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBH_FAIL;
    break;
    default :
      usb_status = USBH_FAIL;
    break;
  }
  return usb_status;
}


/**
  * @brief  USBH_GetFreePipe
  * @param  phost: Host Handle
  *         Get a free Pipe number for allocation to a device endpoint
  * @retval idx: Free Pipe number
  */
static uint16_t USBH_GetFreePipe (USBH_HandleTypeDef *phost)
{
  uint8_t idx = 0;

  for (idx = 0 ; idx < 11 ; idx++)
  {
	if ((phost->Pipes[idx] & 0x8000) == 0)
	{
	   return idx;
	}
  }
  return 0xFFFF;
}

/**
  * @brief  USBH_Alloc_Pipe
  *         Allocate a new Pipe
  * @param  phost: Host Handle
  * @param  ep_addr: End point for which the Pipe to be allocated
  * @retval Pipe number
  */
uint8_t USBH_AllocPipe  (USBH_HandleTypeDef *phost, uint8_t ep_addr)
{
  uint16_t pipe;

  pipe = USBH_GetFreePipe(phost);

  if (pipe != 0xFFFF)
  {
	phost->Pipes[pipe] = 0x8000 | ep_addr;
  }
  return pipe;
}

/**
  * @brief  USBH_Free_Pipe
  *         Free the USB Pipe
  * @param  phost: Host Handle
  * @param  idx: Pipe number to be freed
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_FreePipe  (USBH_HandleTypeDef *phost, uint8_t idx)
{
   if(idx < 11)
   {
	 phost->Pipes[idx] &= 0x7FFF;
   }
   return USBH_OK;
}


/**
  * @brief  USBH_Start
  *         Start the USB Host Core.
  * @param  phost: Host Handle
  * @retval USBH Status
  */
/* вызывается при разрешённых прерываниях. */
USBH_StatusTypeDef  USBH_Start(USBH_HandleTypeDef *phost)
{
	/* Start the low level driver  */
	USBH_LL_Start(phost);

	/* Activate VBUS on the port */
	USBH_LL_DriverVBUS(phost, USB_TRUE);

	return USBH_OK;
}

/**
  * @brief  USBH_Stop
  *         Stop the USB Host Core.
  * @param  phost: Host Handle
  * @retval USBH Status
  */
/* вызывается при разрешённых прерываниях. */
USBH_StatusTypeDef  USBH_Stop(USBH_HandleTypeDef *phost)
{
  /* Stop and cleanup the low level driver  */
  USBH_LL_Stop(phost);

  /* DeActivate VBUS on the port */
  USBH_LL_DriverVBUS (phost, USB_FALSE);

  /* Free Control Pipes */
  USBH_FreePipe  (phost, phost->Control.pipe_in);
  USBH_FreePipe  (phost, phost->Control.pipe_out);

  return USBH_OK;
}


/**
  * @brief  HCD_ReEnumerate
  *         Perform a new Enumeration phase.
  * @param  phost: Host Handle
  * @retval USBH Status
  */
/* вызывается при разрешённых прерываниях. */
USBH_StatusTypeDef  USBH_ReEnumerate(USBH_HandleTypeDef *phost)
{
	/*Stop Host */
	USBH_Stop(phost);

	/*Device has disconnected, so wait for 200 ms */
	HARDWARE_DELAY_MS(200);

	/* Set State machines in default state */
	DeInitStateMachine(phost);

	/* Start again the host */
	USBH_Start(phost);

#if (USBH_USE_OS == 1)
	osMessagePut ( phost->os_event, USBH_PORT_EVENT, 0);
#endif
	return USBH_OK;
}


/**
  * @brief  HCD_Init
  *         Initialize the HOST Core.
  * @param  phost: Host Handle
  * @param  pUsrFunc: User Callback
  * @retval USBH Status
  */
/* вызывается при разрешённых прерываниях. */
USBH_StatusTypeDef  USBH_Init(USBH_HandleTypeDef *phost, void (*pUsrFunc)(USBH_HandleTypeDef *phost, uint8_t ), uint8_t id)
{
  /* Check whether the USB Host handle is valid */
  if (phost == NULL)
  {
    PRINTF(PSTR("Invalid Host handle\n"));
    return USBH_FAIL;
  }

  /* Set DRiver ID */
  phost->id = id;

  /* Unlink class*/
  phost->pActiveClass = NULL;
  phost->ClassNumber = 0;

  /* Restore default states and prepare EP0 */
  DeInitStateMachine(phost);

  /* Assign User process */
  if(pUsrFunc != NULL)
  {
    phost->pUser = pUsrFunc;
  }

#if (USBH_USE_OS == 1)

  /* Create USB Host Queue */
  osMessageQDef(USBH_Queue, 10, uint16_t);
  phost->os_event = osMessageCreate (osMessageQ(USBH_Queue), NULL);

  /*Create USB Host Task */
#if defined (USBH_PROCESS_STACK_SIZE)
  osThreadDef(USBH_Thread, USBH_Process_OS, USBH_PROCESS_PRIO, 0, USBH_PROCESS_STACK_SIZE);
#else
  osThreadDef(USBH_Thread, USBH_Process_OS, USBH_PROCESS_PRIO, 0, 8 * configMINIMAL_STACK_SIZE);
#endif
  phost->thread = osThreadCreate (osThread(USBH_Thread), phost);
#endif  /* (USBH_USE_OS == 1)  */

  /* Initialize low level driver */
  USBH_LL_Init(phost);
  return USBH_OK;
}

/**
  * @brief  HCD_Init
  *         De-Initialize the Host portion of the driver.
  * @param  phost: Host Handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_DeInit(USBH_HandleTypeDef *phost)
{
	DeInitStateMachine(phost);

	if (phost->pData != NULL)
	{
		phost->pActiveClass->pData = NULL;
		USBH_LL_Stop(phost);
	}

	return USBH_OK;
}


/*
 * user callback definition
*/
static void USBH_UserProcess  (USBH_HandleTypeDef *phost, uint8_t id)
{

	/* USER CODE BEGIN CALL_BACK_1 */
	switch(id)
	{
	case HOST_USER_SELECT_CONFIGURATION:
		break;

	case HOST_USER_DISCONNECTION:
		Appli_state = APPLICATION_DISCONNECT;
		break;

	case HOST_USER_CLASS_ACTIVE:
		Appli_state = APPLICATION_READY;
		break;

	case HOST_USER_CONNECTION:
		Appli_state = APPLICATION_START;
		break;

	default:
		break;
	}
	/* USER CODE END CALL_BACK_1 */
}

/**
  * @brief  USBH_HandleSof
  *         Call SOF process
  * @param  phost: Host Handle
  * @retval None
  */
void  USBH_HandleSof  (USBH_HandleTypeDef *phost)
{
	if ((phost->gState == HOST_CLASS) && (phost->pActiveClass != NULL))
	{
		phost->pActiveClass->SOFProcess(phost);
	}
}
/**
  * @brief  USBH_LL_Connect
  *         Handle USB Host connexion event
  * @param  phost: Host Handle
  * @retval USBH_Status
  */
USBH_StatusTypeDef  USBH_LL_Connect(USBH_HandleTypeDef *phost)
{
	switch (phost->gState)
	{
	case HOST_IDLE:
		PRINTF(PSTR("USBH_LL_Connect at HOST_IDLE\n"));
		phost->device.is_connected = 1;

		if (phost->pUser != NULL)
		{
			phost->pUser(phost, HOST_USER_CONNECTION);
		}
		break;

	case HOST_DEV_WAIT_FOR_ATTACHMENT:
		PRINTF(PSTR("USBH_LL_Connect at HOST_DEV_WAIT_FOR_ATTACHMENT\n"));
		phost->gState = HOST_DEV_BEFORE_ATTACHED;
		break;

	default:
		break;
	}

#if (USBH_USE_OS == 1)
	osMessagePut ( phost->os_event, USBH_PORT_EVENT, 0);
#endif

	return USBH_OK;
}

/**
  * @brief  USBH_LL_Disconnect
  *         Handle USB Host disconnection event
  * @param  phost: Host Handle
  * @retval USBH_Status
  */
USBH_StatusTypeDef  USBH_LL_Disconnect(USBH_HandleTypeDef *phost)
{
	switch (phost->gState)
	{
	case HOST_DEV_BEFORE_ATTACHED:
		PRINTF(PSTR("USBH_LL_Disconnect at HOST_DEV_BEFORE_ATTACHED\n"));
		return USBH_OK;

	default:
		PRINTF(PSTR("USBH_LL_Disconnect at phost->gState=%d\n"), (int) phost->gState);
		break;
	}
	/*Stop Host */
	USBH_LL_Stop(phost);

	/* Free Control Pipes */
	USBH_FreePipe  (phost, phost->Control.pipe_in);
	USBH_FreePipe  (phost, phost->Control.pipe_out);

	phost->device.is_connected = 0;

	if (phost->pUser != NULL)
	{
		phost->pUser(phost, HOST_USER_DISCONNECTION);
	}

	//PRINTF(PSTR("USB Device disconnected\n"));

	/* Start the low level driver  */
	USBH_LL_Start(phost);

	phost->gState = HOST_DEV_DISCONNECTED;

#if (USBH_USE_OS == 1)
	osMessagePut ( phost->os_event, USBH_PORT_EVENT, 0);
#endif

	return USBH_OK;
}

/**
  * @brief  USBH_LL_OpenPipe
  *         Open a pipe of the Low Level Driver.
  * @param  phost: Host handle
  * @param  pipe_num: Pipe index
  * @param  epnum: Endpoint Number
  * @param  dev_address: Device USB address
  * @param  speed: Device Speed
  * @param  ep_type: Endpoint Type
  * @param  mps: Endpoint Max Packet Size
  * @retval USBH Status
  */
USBH_StatusTypeDef   USBH_LL_OpenPipe    (USBH_HandleTypeDef *phost,
                                      uint8_t pipe_num,
                                      uint8_t epnum,
                                      uint8_t dev_address,
                                      uint8_t speed,
                                      uint8_t ep_type,
                                      uint16_t mps)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  hal_status = HAL_HCD_HC_Init(phost->pData,
                               pipe_num,
                               epnum,
                               dev_address,
                               speed,
                               ep_type,
                              mps);
  switch (hal_status) {
    case HAL_OK :
      usb_status = USBH_OK;
    break;
    case HAL_ERROR :
      usb_status = USBH_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBH_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBH_FAIL;
    break;
    default :
      usb_status = USBH_FAIL;
    break;
  }
  return usb_status;
}

/**
  * @brief  USBH_LL_ClosePipe
  *         Close a pipe of the Low Level Driver.
  * @param  phost: Host handle
  * @param  pipe_num: Pipe index
  * @retval USBH Status
  */
USBH_StatusTypeDef   USBH_LL_ClosePipe   (USBH_HandleTypeDef *phost, uint8_t pipe)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  hal_status = HAL_HCD_HC_Halt(phost->pData, pipe);
  switch (hal_status) {
    case HAL_OK :
      usb_status = USBH_OK;
    break;
    case HAL_ERROR :
      usb_status = USBH_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBH_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBH_FAIL;
    break;
    default :
      usb_status = USBH_FAIL;
    break;
  }
  return usb_status;
}


/**
  * @brief  USBH_LL_GetURBState
  *         Get a URB state from the low level driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  *         This parameter can be a value from 1 to 15
  * @retval URB state
  *          This parameter can be one of the these values:
  *            @arg URB_IDLE
  *            @arg URB_DONE
  *            @arg URB_NOTREADY
  *            @arg URB_NYET
  *            @arg URB_ERROR
  *            @arg URB_STALL
  */
USBH_URBStateTypeDef  USBH_LL_GetURBState (USBH_HandleTypeDef *phost, uint8_t pipe)
{
  return (USBH_URBStateTypeDef)HAL_HCD_HC_GetURBState (phost->pData, pipe);
}



/**
  * @brief  USBH_LL_SubmitURB
  *         Submit a new URB to the low level driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  *         This parameter can be a value from 1 to 15
  * @param  direction : Channel number
  *          This parameter can be one of the these values:
  *           0 : Output
  *           1 : Input
  * @param  ep_type : Endpoint Type
  *          This parameter can be one of the these values:
  *            @arg USBD_EP_TYPE_CTRL: Control type
  *            @arg USBD_EP_TYPE_ISOC: Isochrounous type
  *            @arg USBD_EP_TYPE_BULK: Bulk type
  *            @arg USBD_EP_TYPE_INTR: Interrupt type
  * @param  token : Endpoint Type
  *          This parameter can be one of the these values:
  *            @arg 0: PID_SETUP
  *            @arg 1: PID_DATA
  * @param  pbuff : pointer to URB data
  * @param  length : Length of URB data
  * @param  do_ping : activate do ping protocol (for high speed only)
  *          This parameter can be one of the these values:
  *           0 : do ping inactive
  *           1 : do ping active
  * @retval Status
  */

USBH_StatusTypeDef   USBH_LL_SubmitURB  (USBH_HandleTypeDef *phost,
                                            uint8_t pipe,
                                            uint8_t direction ,
                                            uint8_t ep_type,  
                                            uint8_t token, 
                                            uint8_t* pbuff, 
                                            uint16_t length,
                                            uint8_t do_ping )
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  hal_status = HAL_HCD_HC_SubmitRequest (phost->pData,
                                         pipe,
                                         direction ,
                                         ep_type,
                                         token,
                                         pbuff,
                                         length,
                                         do_ping);
  switch (hal_status) {
    case HAL_OK :
      usb_status = USBH_OK;
    break;
    case HAL_ERROR :
      usb_status = USBH_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBH_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBH_FAIL;
    break;
    default :
      usb_status = USBH_FAIL;
    break;
  }
  return usb_status;
}

/**
  * @brief  USBH_CtlSendSetup
  *         Sends the Setup Packet to the Device
  * @param  phost: Host Handle
  * @param  buff: Buffer pointer from which the Data will be send to Device
  * @param  pipe_num: Pipe Number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_CtlSendSetup (USBH_HandleTypeDef *phost,
                                uint8_t *buff,
                                uint8_t pipe_num)
{

  USBH_LL_SubmitURB (phost,                     /* Driver handle    */
                          pipe_num,             /* Pipe index       */
                          0,                    /* Direction : OUT  */
                          USBH_EP_CONTROL,      /* EP type          */
                          USBH_PID_SETUP,       /* Type setup       */
                          buff,                 /* data buffer      */
                          USBH_SETUP_PKT_SIZE,  /* data length      */
                          0);
  return USBH_OK;
}


/**
  * @brief  USBH_CtlSendData
  *         Sends a data Packet to the Device
  * @param  phost: Host Handle
  * @param  buff: Buffer pointer from which the Data will be sent to Device
  * @param  length: Length of the data to be sent
  * @param  pipe_num: Pipe Number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_CtlSendData (USBH_HandleTypeDef *phost,
                                uint8_t *buff,
                                uint16_t length,
                                uint8_t pipe_num,
                                uint8_t do_ping )
{
  if (phost->device.speed != USBH_SPEED_HIGH)
  {
    do_ping = 0;
  }
  
  USBH_LL_SubmitURB (phost,                     /* Driver handle    */
                          pipe_num,             /* Pipe index       */
                          0,                    /* Direction : OUT  */
                          USBH_EP_CONTROL,      /* EP type          */
                          USBH_PID_DATA,        /* Type Data        */
                          buff,                 /* data buffer      */
                          length,               /* data length      */
                          do_ping);             /* do ping (HS Only)*/
  
  return USBH_OK;
}


/**
  * @brief  USBH_CtlReceiveData
  *         Receives the Device Response to the Setup Packet
  * @param  phost: Host Handle
  * @param  buff: Buffer pointer in which the response needs to be copied
  * @param  length: Length of the data to be received
  * @param  pipe_num: Pipe Number
  * @retval USBH Status.
  */
USBH_StatusTypeDef USBH_CtlReceiveData(USBH_HandleTypeDef *phost,
                                uint8_t* buff,
                                uint16_t length,
                                uint8_t pipe_num)
{
  USBH_LL_SubmitURB (phost,                     /* Driver handle    */
                          pipe_num,             /* Pipe index       */
                          1,                    /* Direction : IN   */
                          USBH_EP_CONTROL,      /* EP type          */
                          USBH_PID_DATA,        /* Type Data        */
                          buff,                 /* data buffer      */
                          length,               /* data length      */
                          0);
  return USBH_OK;

}



/**
  * @brief  USBH_Open_Pipe
  *         Open a  pipe
  * @param  phost: Host Handle
  * @param  pipe_num: Pipe Number
  * @param  dev_address: USB Device address allocated to attached device
  * @param  speed : USB device speed (Full/Low)
  * @param  ep_type: end point type (Bulk/int/ctl)
  * @param  mps: max pkt size
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_OpenPipe  (USBH_HandleTypeDef *phost,
                            uint8_t pipe_num,
                            uint8_t epnum,
                            uint8_t dev_address,
                            uint8_t speed,
                            uint8_t ep_type,
                            uint16_t mps)
{

  USBH_LL_OpenPipe(phost,
                        pipe_num,
                        epnum,
                        dev_address,
                        speed,
                        ep_type,
                        mps);
  
  return USBH_OK;

}


/**
  * @brief  DeInitStateMachine
  *         De-Initialize the Host state machine.
  * @param  phost: Host Handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  DeInitStateMachine(USBH_HandleTypeDef *phost)
{
  uint32_t i = 0;

  /* Clear Pipes flags*/
  for ( ; i < USBH_MAX_PIPES_NBR; i++)
  {
    phost->Pipes[i] = 0;
  }
  
  for(i = 0; i< USBH_MAX_DATA_BUFFER; i++)
  {
    phost->device.Data[i] = 0;
  }

  phost->gState = HOST_IDLE;
  phost->EnumState = ENUM_IDLE;
  phost->RequestState = CMD_SEND;
  phost->Timer = 0;

  phost->Control.state = CTRL_SETUP;
  phost->Control.pipe_size = USBH_MPS_DEFAULT;
  phost->Control.errorcount = 0;

  phost->device.address = USBH_ADDRESS_DEFAULT;
  phost->device.speed   = USBH_SPEED_FULL;

  return USBH_OK;
}

//+++ enumeration support functions

/**
  * @brief  USBH_ParseDevDesc
  *         This function Parses the device descriptor
  * @param  dev_desc: device_descriptor destination address
  * @param  buf: Buffer where the source descriptor is available
  * @param  length: Length of the descriptor
  * @retval None
  */
void  USBH_ParseDevDesc (USBH_DevDescTypeDef* dev_desc,
                                uint8_t *buf,
                                uint16_t length)
{
  dev_desc->bLength            = *(uint8_t  *) (buf +  0);
  dev_desc->bDescriptorType    = *(uint8_t  *) (buf +  1);
  dev_desc->bcdUSB             = LE16 (buf +  2);
  dev_desc->bDeviceClass       = *(uint8_t  *) (buf +  4);
  dev_desc->bDeviceSubClass    = *(uint8_t  *) (buf +  5);
  dev_desc->bDeviceProtocol    = *(uint8_t  *) (buf +  6);
  dev_desc->bMaxPacketSize     = *(uint8_t  *) (buf +  7);
  
  if (length > 8)
  { /* For 1st time after device connection, Host may issue only 8 bytes for
    Device Descriptor Length  */
    dev_desc->idVendor           = LE16 (buf +  8);
    dev_desc->idProduct          = LE16 (buf + 10);
    dev_desc->bcdDevice          = LE16 (buf + 12);
    dev_desc->iManufacturer      = *(uint8_t  *) (buf + 14);
    dev_desc->iProduct           = *(uint8_t  *) (buf + 15);
    dev_desc->iSerialNumber      = *(uint8_t  *) (buf + 16);
    dev_desc->bNumConfigurations = *(uint8_t  *) (buf + 17);
  }
}

/**
  * @brief  USBH_ParseCfgDesc
  *         This function Parses the configuration descriptor
  * @param  cfg_desc: Configuration Descriptor address
  * @param  buf: Buffer where the source descriptor is available
  * @param  length: Length of the descriptor
  * @retval None
  */
void USBH_ParseCfgDesc (USBH_CfgDescTypeDef* cfg_desc,
                               uint8_t *buf,
                               uint16_t length)
{
	USBH_InterfaceDescTypeDef    *pif ;
	USBH_EpDescTypeDef           *pep;
	USBH_DescHeader_t             *pdesc = (USBH_DescHeader_t *)buf;

	uint_fast8_t iadmode = 0;
	pdesc   = (USBH_DescHeader_t *) buf;

	/* Parse configuration descriptor */
	cfg_desc->bLength             = *(uint8_t  *) (buf + 0);
	cfg_desc->bDescriptorType     = *(uint8_t  *) (buf + 1);
	cfg_desc->wTotalLength        = LE16 (buf + 2);
	cfg_desc->bNumInterfaces      = *(uint8_t  *) (buf + 4);
	cfg_desc->bConfigurationValue = *(uint8_t  *) (buf + 5);
	cfg_desc->iConfiguration      = *(uint8_t  *) (buf + 6);
	cfg_desc->bmAttributes        = *(uint8_t  *) (buf + 7);
	cfg_desc->bMaxPower           = *(uint8_t  *) (buf + 8);


	if (length > USB_CONFIGURATION_DESC_SIZE)
	{
		uint16_t                      ptr;
		int8_t                        if_ix = 0;
		int8_t                        ep_ix = 0;
		ptr = USB_LEN_CFG_DESC;
		pif = (USBH_InterfaceDescTypeDef *) NULL;


    while ((if_ix < USBH_MAX_NUM_INTERFACES ) && (ptr < cfg_desc->wTotalLength))
    {
      pdesc = USBH_GetNextDesc((uint8_t *)pdesc, &ptr);

      if (pdesc->bDescriptorType == USB_INTERFACE_ASSOC_DESCRIPTOR_TYPE)
	  {
		  iadmode = 1;
		  USBH_IfAssocDescTypeDef * piad = (USBH_IfAssocDescTypeDef *) pdesc;
		PRINTF(PSTR("USBH_ParseCfgDesc: USB_INTERFACE_ASSOC_DESCRIPTOR_TYPE: 0x%02X/x%02X, nif=%d, firstIf=%d\n"), piad->bFunctionClass, piad->bFunctionSubClass, piad->bInterfaceCount, piad->bFirstInterface);
	  }
      else if (/*iadmode == 0 && */ pdesc->bDescriptorType == USB_DESC_TYPE_INTERFACE)
      {
        pif = &cfg_desc->Itf_Desc[if_ix];
        USBH_ParseInterfaceDesc (pif, (uint8_t *)pdesc);

		PRINTF(PSTR("USBH_ParseCfgDesc: ifix=%d, 0x%02X/0x%02X/0x%02X, nEP=%d\n"), pif->bInterfaceNumber, pif->bInterfaceClass, pif->bInterfaceSubClass, pif->bInterfaceProtocol, pif->bNumEndpoints);
		// 0x08/0x06/0x50: USB flash
		// 0xFF/0x00/0x00: CP2102
		// 0x03/0x01/0x02: USB-PS/2 Optical Mouse
		//
		// Composite device (Audio+2xCDC):
		// 0x01/0x01/0x00, nEP=0
		// 0x01/0x02/0x00, nEP=0
		// 0x01/0x02/0x00, nEP=1
		// 0x01/0x02/0x00, nEP=0
		// 0x01/0x02/0x00, nEP=1
		// 0x02/0x02/0x01, nEP=1
		// 0x0A/0x00/0x00, nEP=2
		// 0x02/0x02/0x01, nEP=1
		// 0x0A/0x00/0x00, nEP=2


        ep_ix = 0;
        pep = (USBH_EpDescTypeDef *)NULL;
        while ((ep_ix < pif->bNumEndpoints) && (ptr < cfg_desc->wTotalLength))
        {
          pdesc = USBH_GetNextDesc((uint8_t*) pdesc, &ptr);
          if (pdesc->bDescriptorType   == USB_DESC_TYPE_ENDPOINT)
          {
            pep = &cfg_desc->Itf_Desc[if_ix].Ep_Desc[ep_ix];
            USBH_ParseEPDesc (pep, (uint8_t *)pdesc);
            ep_ix++;
          }
        }
        if_ix++;
      }
	  else
	  {
		//PRINTF(PSTR("USBH_ParseCfgDesc: bDescriptorType=0x%02X\n"), pdesc->bDescriptorType);
	  }
    }
  }
	(void) iadmode;
}



/**
  * @brief  USBH_ParseInterfaceDesc
  *         This function Parses the interfacei descriptor
  * @param  if_descriptor : Interface descriptor destination
  * @param  buf: Buffer where the descriptor data is available
  * @retval None
  */
void  USBH_ParseInterfaceDesc (USBH_InterfaceDescTypeDef *if_descriptor,
                                      uint8_t *buf)
{
  if_descriptor->bLength            = *(uint8_t  *) (buf + 0);
  if_descriptor->bDescriptorType    = *(uint8_t  *) (buf + 1);
  if_descriptor->bInterfaceNumber   = *(uint8_t  *) (buf + 2);
  if_descriptor->bAlternateSetting  = *(uint8_t  *) (buf + 3);
  if_descriptor->bNumEndpoints      = *(uint8_t  *) (buf + 4);
  if_descriptor->bInterfaceClass    = *(uint8_t  *) (buf + 5);
  if_descriptor->bInterfaceSubClass = *(uint8_t  *) (buf + 6);
  if_descriptor->bInterfaceProtocol = *(uint8_t  *) (buf + 7);
  if_descriptor->iInterface         = *(uint8_t  *) (buf + 8);
}

/**
  * @brief  USBH_ParseEPDesc
  *         This function Parses the endpoint descriptor
  * @param  ep_descriptor: Endpoint descriptor destination address
  * @param  buf: Buffer where the parsed descriptor stored
  * @retval None
  */
void  USBH_ParseEPDesc (USBH_EpDescTypeDef  *ep_descriptor,
                               uint8_t *buf)
{

  ep_descriptor->bLength          = *(uint8_t  *) (buf + 0);
  ep_descriptor->bDescriptorType  = *(uint8_t  *) (buf + 1);
  ep_descriptor->bEndpointAddress = *(uint8_t  *) (buf + 2);
  ep_descriptor->bmAttributes     = *(uint8_t  *) (buf + 3);
  ep_descriptor->wMaxPacketSize   = LE16 (buf + 4);
  ep_descriptor->bInterval        = *(uint8_t  *) (buf + 6);
}

/**
  * @brief  USBH_ParseStringDesc
  *         This function Parses the string descriptor
  * @param  psrc: Source pointer containing the descriptor data
  * @param  pdest: Destination address pointer
  * @param  length: Length of the descriptor
  * @retval None
  */
void USBH_ParseStringDesc (uint8_t* psrc,
                                  uint8_t* pdest,
                                  uint16_t length)
{
  uint16_t strlength;
  uint16_t idx;
  
  /* The UNICODE string descriptor is not NULL-terminated. The string length is
  computed by substracting two from the value of the first byte of the descriptor.
  */
  
  /* Check which is lower size, the Size of string or the length of bytes read
  from the device */
  
  if (psrc[1] == USB_DESC_TYPE_STRING)
  { /* Make sure the Descriptor is String Type */

    /* psrc[0] contains Size of Descriptor, subtract 2 to get the length of string */
    strlength = ( ( (psrc[0]-2) <= length) ? (psrc[0]-2) :length);
    psrc += 2; /* Adjust the offset ignoring the String Len and Descriptor type */

    for (idx = 0; idx < strlength; idx+=2 )
    {/* Copy Only the string and ignore the UNICODE ID, hence add the src */
      *pdest = psrc[idx];
      pdest++;
    }
    *pdest = 0; /* mark end of string */
  }
}

/**
  * @brief  USBH_GetNextDesc
  *         This function return the next descriptor header
  * @param  buf: Buffer where the cfg descriptor is available
  * @param  ptr: data pointer inside the cfg descriptor
  * @retval next header
  */
USBH_DescHeader_t  *USBH_GetNextDesc(uint8_t   *pbuf, uint16_t  *ptr)
{
  USBH_DescHeader_t  *pnext;

  *ptr += ((USBH_DescHeader_t *)pbuf)->bLength;
  pnext = (USBH_DescHeader_t *)((uint8_t *)pbuf +
         ((USBH_DescHeader_t *)pbuf)->bLength);

  return(pnext);
}


/**
  * @brief  USBH_HandleControl
  *         Handles the USB control transfer state machine
  * @param  phost: Host Handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_HandleControl (USBH_HandleTypeDef *phost)
{
  uint8_t direction;
  USBH_StatusTypeDef status = USBH_BUSY;
  USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;

  switch (phost->Control.state)
  {
  case CTRL_SETUP:
    /* send a SETUP packet */
    USBH_CtlSendSetup     (phost,
	                   (uint8_t *)phost->Control.setup.d8 ,
	                   phost->Control.pipe_out);

    phost->Control.state = CTRL_SETUP_WAIT;
    break;

  case CTRL_SETUP_WAIT:

    URB_Status = USBH_LL_GetURBState(phost, phost->Control.pipe_out);
    /* case SETUP packet sent successfully */
    if(URB_Status == USBH_URB_DONE)
    {
      direction = (phost->Control.setup.b.bmRequestType & USB_REQ_DIR_MASK);

      /* check if there is a data stage */
      if (phost->Control.setup.b.wLength.w != 0 )
      {
        if (direction == USB_D2H)
        {
          /* Data Direction is IN */
          phost->Control.state = CTRL_DATA_IN;
        }
        else
        {
          /* Data Direction is OUT */
          phost->Control.state = CTRL_DATA_OUT;
        }
      }
      /* No DATA stage */
      else
      {
        /* If there is No Data Transfer Stage */
        if (direction == USB_D2H)
        {
          /* Data Direction is IN */
          phost->Control.state = CTRL_STATUS_OUT;
        }
        else
        {
          /* Data Direction is OUT */
          phost->Control.state = CTRL_STATUS_IN;
        }
      }
#if (USBH_USE_OS == 1)
      osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }
    else if(URB_Status == USBH_URB_ERROR)
    {
      phost->Control.state = CTRL_ERROR;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }
    break;

  case CTRL_DATA_IN:
    /* Issue an IN token */
     phost->Control.timer = phost->Timer;
    USBH_CtlReceiveData(phost,
                        phost->Control.buff,
                        phost->Control.length,
                        phost->Control.pipe_in);

    phost->Control.state = CTRL_DATA_IN_WAIT;
    break;

  case CTRL_DATA_IN_WAIT:

    URB_Status = USBH_LL_GetURBState(phost , phost->Control.pipe_in);

    /* check is DATA packet transferred successfully */
    if  (URB_Status == USBH_URB_DONE)
    {
      phost->Control.state = CTRL_STATUS_OUT;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }

    /* manage error cases*/
    if  (URB_Status == USBH_URB_STALL)
    {
      /* In stall case, return to previous machine state*/
      status = USBH_NOT_SUPPORTED;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }
    else if (URB_Status == USBH_URB_ERROR)
    {
      /* Device error */
      phost->Control.state = CTRL_ERROR;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }
    break;

  case CTRL_DATA_OUT:

    USBH_CtlSendData (phost,
                      phost->Control.buff,
                      phost->Control.length ,
                      phost->Control.pipe_out,
                      1);
     phost->Control.timer = phost->Timer;
    phost->Control.state = CTRL_DATA_OUT_WAIT;
    break;

  case CTRL_DATA_OUT_WAIT:

    URB_Status = USBH_LL_GetURBState(phost , phost->Control.pipe_out);

    if  (URB_Status == USBH_URB_DONE)
    { /* If the Setup Pkt is sent successful, then change the state */
      phost->Control.state = CTRL_STATUS_IN;
#if (USBH_USE_OS == 1)
      osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }

    /* handle error cases */
    else if  (URB_Status == USBH_URB_STALL)
    {
      /* In stall case, return to previous machine state*/
      phost->Control.state = CTRL_STALLED;
      status = USBH_NOT_SUPPORTED;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }
    else if  (URB_Status == USBH_URB_NOTREADY)
    {
      /* Nack received from device */
      phost->Control.state = CTRL_DATA_OUT;

#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }
    else if (URB_Status == USBH_URB_ERROR)
    {
      /* device error */
      phost->Control.state = CTRL_ERROR;
      status = USBH_FAIL;

#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }
    break;


  case CTRL_STATUS_IN:
    /* Send 0 bytes out packet */
    USBH_CtlReceiveData (phost,
                         0,
                         0,
                         phost->Control.pipe_in);
    phost->Control.timer = phost->Timer;
    phost->Control.state = CTRL_STATUS_IN_WAIT;

    break;

  case CTRL_STATUS_IN_WAIT:

    URB_Status = USBH_LL_GetURBState(phost , phost->Control.pipe_in);

    if  ( URB_Status == USBH_URB_DONE)
    { /* Control transfers completed, Exit the State Machine */
      phost->Control.state = CTRL_COMPLETE;
      status = USBH_OK;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }

    else if (URB_Status == USBH_URB_ERROR)
    {
      phost->Control.state = CTRL_ERROR;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }
     else if(URB_Status == USBH_URB_STALL)
    {
      /* Control transfers completed, Exit the State Machine */
      status = USBH_NOT_SUPPORTED;

#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }
    break;

  case CTRL_STATUS_OUT:
    USBH_CtlSendData (phost,
                      0,
                      0,
                      phost->Control.pipe_out,
                      1);
     phost->Control.timer = phost->Timer;
    phost->Control.state = CTRL_STATUS_OUT_WAIT;
    break;

  case CTRL_STATUS_OUT_WAIT:

    URB_Status = USBH_LL_GetURBState(phost , phost->Control.pipe_out);
    if  (URB_Status == USBH_URB_DONE)
    {
      status = USBH_OK;
      phost->Control.state = CTRL_COMPLETE;

#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }
    else if  (URB_Status == USBH_URB_NOTREADY)
    {
      phost->Control.state = CTRL_STATUS_OUT;

#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }
    else if (URB_Status == USBH_URB_ERROR)
    {
      phost->Control.state = CTRL_ERROR;

#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    }
    break;

  case CTRL_ERROR:
    /*
    After a halt condition is encountered or an error is detected by the
    host, a control endpoint is allowed to recover by accepting the next Setup
    PID; i.e., recovery actions via some other pipe are not required for control
    endpoints. For the Default Control Pipe, a device reset will ultimately be
    required to clear the halt or error condition if the next Setup PID is not
    accepted.
    */
    if (++ phost->Control.errorcount <= USBH_MAX_ERROR_COUNT)
    {
      /* try to recover control */
      USBH_LL_Stop(phost);

      /* Do the transmission again, starting from SETUP Packet */
      phost->Control.state = CTRL_SETUP;
      phost->RequestState = CMD_SEND;
    }
    else
    {
      phost->pUser(phost, HOST_USER_UNRECOVERED_ERROR);
      phost->Control.errorcount = 0;
      PRINTF(PSTR("Control error\n"));
      status = USBH_FAIL;
    }
    break;

  default:
    break;
  }
  return status;
}


/**
  * @brief  USBH_CtlReq
  *         USBH_CtlReq sends a control request and provide the status after
  *            completion of the request
  * @param  phost: Host Handle
  * @param  req: Setup Request Structure
  * @param  buff: data buffer address to store the response
  * @param  length: length of the response
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_CtlReq     (USBH_HandleTypeDef *phost,
                             uint8_t             *buff,
                             uint16_t            length)
{
  USBH_StatusTypeDef status;
  status = USBH_BUSY;

  switch (phost->RequestState)
  {
  case CMD_SEND:
    /* Start a SETUP transfer */
    phost->Control.buff = buff;
    phost->Control.length = length;
    phost->Control.state = CTRL_SETUP;
    phost->RequestState = CMD_WAIT;
    status = USBH_BUSY;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CONTROL_EVENT, 0);
#endif
    break;

  case CMD_WAIT:
    status = USBH_HandleControl(phost);
     if (status == USBH_OK)
    {
      /* Commands successfully sent and Response Received  */
      phost->RequestState = CMD_SEND;
      phost->Control.state = CTRL_IDLE;
      status = USBH_OK;
    }
    else if (status == USBH_FAIL)
    {
      /* Failure Mode */
      phost->RequestState = CMD_SEND;
      status = USBH_FAIL;
    }
    break;

  default:
    break;
  }
  return status;
}



/**
  * @brief  USBH_Get_DevDesc
  *         Issue Get Device Descriptor command to the device. Once the response
  *         received, it parses the device descriptor and updates the status.
  * @param  phost: Host Handle
  * @param  length: Length of the descriptor
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_Get_DevDesc(USBH_HandleTypeDef *phost, uint8_t length)
{
  USBH_StatusTypeDef status;

  if ((status = USBH_GetDescriptor(phost,
                                  USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,
                                  USB_DESC_DEVICE,
                                  phost->device.Data,
                                  length)) == USBH_OK)
  {
    /* Commands successfully sent and Response Received */
    USBH_ParseDevDesc(&phost->device.DevDesc, phost->device.Data, length);
  }
  return status;
}

/**
  * @brief  USBH_Get_CfgDesc
  *         Issues Configuration Descriptor to the device. Once the response
  *         received, it parses the configuration descriptor and updates the
  *         status.
  * @param  phost: Host Handle
  * @param  length: Length of the descriptor
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_Get_CfgDesc(USBH_HandleTypeDef *phost,
                             uint16_t length)

{
  USBH_StatusTypeDef status;
  uint8_t *pData;
#if (USBH_KEEP_CFG_DESCRIPTOR == 1)
  pData = phost->device.CfgDesc_Raw;
#else
  pData = phost->device.Data;
#endif
  if ((status = USBH_GetDescriptor(phost,
                                  USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,
                                  USB_DESC_CONFIGURATION,
                                  pData,
                                  length)) == USBH_OK)
  {

    /* Commands successfully sent and Response Received  */
    USBH_ParseCfgDesc (&phost->device.CfgDesc,
                       pData,
                       length);

  }
  return status;
}


/**
  * @brief  USBH_Get_StringDesc
  *         Issues string Descriptor command to the device. Once the response
  *         received, it parses the string descriptor and updates the status.
  * @param  phost: Host Handle
  * @param  string_index: String index for the descriptor
  * @param  buff: Buffer address for the descriptor
  * @param  length: Length of the descriptor
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_Get_StringDesc(USBH_HandleTypeDef *phost,
                                uint8_t string_index,
                                uint8_t *buff,
                                uint16_t length)
{
  USBH_StatusTypeDef status;
  if ((status = USBH_GetDescriptor(phost,
                                  USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,
                                  USB_DESC_STRING | string_index,
                                  phost->device.Data,
                                  length)) == USBH_OK)
  {
    /* Commands successfully sent and Response Received  */
    USBH_ParseStringDesc(phost->device.Data,buff, length);
  }
  return status;
}

/**
  * @brief  USBH_GetDescriptor
  *         Issues Descriptor command to the device. Once the response received,
  *         it parses the descriptor and updates the status.
  * @param  phost: Host Handle
  * @param  req_type: Descriptor type
  * @param  value_idx: Value for the GetDescriptr request
  * @param  buff: Buffer to store the descriptor
  * @param  length: Length of the descriptor
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_GetDescriptor(USBH_HandleTypeDef *phost,
                               uint8_t  req_type,
                               uint16_t value_idx,
                               uint8_t* buff,
                               uint16_t length )
{
  if (phost->RequestState == CMD_SEND)
  {
    phost->Control.setup.b.bmRequestType = USB_D2H | req_type;
    phost->Control.setup.b.bRequest = USB_REQ_GET_DESCRIPTOR;
    phost->Control.setup.b.wValue.w = value_idx;

    if ((value_idx & 0xff00) == USB_DESC_STRING)
    {
      phost->Control.setup.b.wIndex.w = 0x0409;
    }
    else
    {
      phost->Control.setup.b.wIndex.w = 0;
    }
    phost->Control.setup.b.wLength.w = length;
  }
  return USBH_CtlReq(phost, buff , length );
}

/**
  * @brief  USBH_SetAddress
  *         This command sets the address to the connected device
  * @param  phost: Host Handle
  * @param  DeviceAddress: Device address to assign
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_SetAddress(USBH_HandleTypeDef *phost,
                                   uint8_t DeviceAddress)
{
  if (phost->RequestState == CMD_SEND)
  {
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_DEVICE |
      USB_REQ_TYPE_STANDARD;

    phost->Control.setup.b.bRequest = USB_REQ_SET_ADDRESS;

    phost->Control.setup.b.wValue.w = (uint16_t)DeviceAddress;
    phost->Control.setup.b.wIndex.w = 0;
    phost->Control.setup.b.wLength.w = 0;
  }
  return USBH_CtlReq(phost, 0 , 0 );
}

/**
  * @brief  USBH_SetCfg
  *         The command sets the configuration value to the connected device
  * @param  phost: Host Handle
  * @param  cfg_idx: Configuration value
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_SetCfg(USBH_HandleTypeDef *phost,
                               uint16_t cfg_idx)
{
  if (phost->RequestState == CMD_SEND)
  {
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_DEVICE |
      USB_REQ_TYPE_STANDARD;
    phost->Control.setup.b.bRequest = USB_REQ_SET_CONFIGURATION;
    phost->Control.setup.b.wValue.w = cfg_idx;
    phost->Control.setup.b.wIndex.w = 0;
    phost->Control.setup.b.wLength.w = 0;
  }

  return USBH_CtlReq(phost, 0 , 0 );
}

/**
  * @brief  USBH_SetInterface
  *         The command sets the Interface value to the connected device
  * @param  phost: Host Handle
  * @param  altSetting: Interface value
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_SetInterface(USBH_HandleTypeDef *phost,
                        uint8_t ep_num, uint8_t altSetting)
{

  if (phost->RequestState == CMD_SEND)
  {
    phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE |
      USB_REQ_TYPE_STANDARD;

    phost->Control.setup.b.bRequest = USB_REQ_SET_INTERFACE;
    phost->Control.setup.b.wValue.w = altSetting;
    phost->Control.setup.b.wIndex.w = ep_num;
    phost->Control.setup.b.wLength.w = 0;
  }
  return USBH_CtlReq(phost, 0 , 0 );
}

/**
  * @brief  USBH_ClrFeature
  *         This request is used to clear or disable a specific feature.
  * @param  phost: Host Handle
  * @param  ep_num: endpoint number
  * @param  hc_num: Host channel number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_ClrFeature(USBH_HandleTypeDef *phost,
                                   uint8_t ep_num)
{
  if (phost->RequestState == CMD_SEND)
  {
    phost->Control.setup.b.bmRequestType = USB_H2D |
      USB_REQ_RECIPIENT_ENDPOINT |
        USB_REQ_TYPE_STANDARD;

    phost->Control.setup.b.bRequest = USB_REQ_CLEAR_FEATURE;
    phost->Control.setup.b.wValue.w = FEATURE_SELECTOR_ENDPOINT;
    phost->Control.setup.b.wIndex.w = ep_num;
    phost->Control.setup.b.wLength.w = 0;
  }
  return USBH_CtlReq(phost, 0 , 0 );
}


/** @defgroup USBH_CTLREQ_Private_Functions
* @{
*/
/**
  * @brief  USBH_RegisterClass
  *         Link class driver to Host Core.
  * @param  phost : Host Handle
  * @param  pclass: Class handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_RegisterClass(USBH_HandleTypeDef *phost, USBH_ClassTypeDef *pclass)
{
  USBH_StatusTypeDef   status = USBH_OK;

  if(pclass != 0)
  {
    if (phost->ClassNumber < USBH_MAX_NUM_SUPPORTED_CLASS)
    {
      /* link the class to the USB Host handle */
      phost->pClass[phost->ClassNumber ++] = pclass;
      status = USBH_OK;
    }
    else
    {
      PRINTF(PSTR("Max Class Number reached\n"));
      status = USBH_FAIL;
    }
  }
  else
  {
    PRINTF(PSTR("Invalid Class handle\n"));
    status = USBH_FAIL;
  }

  return status;
}

/**
  * @brief  USBH_SelectInterface
  *         Select current interfacei.
  * @param  phost: Host Handle
  * @param  interfacei: Interface number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_SelectInterface(USBH_HandleTypeDef *phost, uint8_t interfacei)
{
  USBH_StatusTypeDef   status = USBH_OK;

  if(interfacei < phost->device.CfgDesc.bNumInterfaces)
  {
    phost->device.current_interface = interfacei;
    PRINTF(PSTR("Switching to Interface (#%d)\n"), interfacei);
    PRINTF(PSTR("Class    : %xh\n"), phost->device.CfgDesc.Itf_Desc[interfacei].bInterfaceClass );
    PRINTF(PSTR("SubClass : %xh\n"), phost->device.CfgDesc.Itf_Desc[interfacei].bInterfaceSubClass );
    PRINTF(PSTR("Protocol : %xh\n"), phost->device.CfgDesc.Itf_Desc[interfacei].bInterfaceProtocol );
  }
  else
  {
    PRINTF(PSTR("Cannot Select This Interface.\n"));
    status = USBH_FAIL;
  }
  return status;
}

/**
  * @brief  USBH_GetActiveClass
  *         Return Device Class.
  * @param  phost: Host Handle
  * @param  interfacei: Interface index
  * @retval Class Code
  */
uint8_t USBH_GetActiveClass(USBH_HandleTypeDef *phost)
{
   return (phost->device.CfgDesc.Itf_Desc [0].bInterfaceClass);
}
/**
  * @brief  USBH_FindInterface
  *         Find the interfacei index for a specific class.
  * @param  phost: Host Handle
  * @param  Class: Class code
  * @param  SubClass: SubClass code
  * @param  Protocol: Protocol code
  * @retval interfacei index in the configuration structure
  * @note : (1)interfacei index 0xFF means interfacei index not found
  */
uint8_t  USBH_FindInterface(USBH_HandleTypeDef *phost, uint8_t Class, uint8_t SubClass, uint8_t Protocol)
{
  USBH_InterfaceDescTypeDef    *pif ;
  USBH_CfgDescTypeDef          *pcfg ;
  int8_t                        if_ix = 0;

  pif = (USBH_InterfaceDescTypeDef *)0;
  pcfg = &phost->device.CfgDesc;

  while (if_ix < USBH_MAX_NUM_INTERFACES)
  {
    pif = &pcfg->Itf_Desc[if_ix];
    if (((pif->bInterfaceClass == Class) || (Class == 0xFF))&&
       ((pif->bInterfaceSubClass == SubClass) || (SubClass == 0xFF))&&
         ((pif->bInterfaceProtocol == Protocol) || (Protocol == 0xFF)))
    {
      return  if_ix;
    }
    if_ix++;
  }
  return 0xFF;
}

/**
  * @brief  USBH_FindInterfaceIndex
  *         Find the interfacei index for a specific class interfacei and alternate setting number.
  * @param  phost: Host Handle
  * @param  interface_number: interfacei number
  * @param  alt_settings    : alternate setting number
  * @retval interfacei index in the configuration structure
  * @note : (1)interfacei index 0xFF means interfacei index not found
  */
uint8_t  USBH_FindInterfaceIndex(USBH_HandleTypeDef *phost, uint8_t interface_number, uint8_t alt_settings)
{
  USBH_InterfaceDescTypeDef    *pif ;
  USBH_CfgDescTypeDef          *pcfg ;
  int8_t                        if_ix = 0;

  pif = (USBH_InterfaceDescTypeDef *)0;
  pcfg = &phost->device.CfgDesc;

  while (if_ix < USBH_MAX_NUM_INTERFACES)
  {
    pif = &pcfg->Itf_Desc[if_ix];
    if ((pif->bInterfaceNumber == interface_number) && (pif->bAlternateSetting == alt_settings))
    {
      return  if_ix;
    }
    if_ix++;
  }
  return 0xFF;
}

//--- enumeration support functions

static void USBH_HandleEnumDelay(
	USBH_HandleTypeDef *phost,
	ENUM_StateTypeDef state,
	unsigned delayMS
	)
{
	const uint_fast16_t nticks = NTICKS(delayMS);
	if (nticks > 2)
	{
		phost->EnumPushTicks = nticks;
		phost->EnumPushedState = state;
		phost->EnumState = ENUM_DELAY;
	}
	else
	{
		phost->EnumState = state;
	}
}

/**
  * @brief  USBH_HandleEnum
  *         This function includes the complete enumeration process
  * @param  phost: Host Handle
  * @retval USBH_Status
  */
USBH_StatusTypeDef USBH_HandleEnum(USBH_HandleTypeDef *phost)
{
  USBH_StatusTypeDef Status = USBH_BUSY;

  switch (phost->EnumState)
  {
  case ENUM_IDLE:
    /* Get Device Desc for only 1st 8 bytes : To get EP0 MaxPacketSize */
    if (USBH_Get_DevDesc(phost, 8) == USBH_OK)
    {
      phost->Control.pipe_size = phost->device.DevDesc.bMaxPacketSize;

      phost->EnumState = ENUM_GET_FULL_DEV_DESC;

      /* modify control channels configuration for MaxPacket size */
      USBH_OpenPipe (phost,
                           phost->Control.pipe_in,
                           0x80,
                           phost->device.address,
                           phost->device.speed,
                           USBH_EP_CONTROL,
                           phost->Control.pipe_size);

      /* Open Control pipes */
      USBH_OpenPipe (phost,
                           phost->Control.pipe_out,
                           0x00,
                           phost->device.address,
                           phost->device.speed,
                           USBH_EP_CONTROL,
                           phost->Control.pipe_size);

    }
    break;

  case ENUM_DELAY:
	  if (-- phost->EnumPushTicks == 0)
	  {
		  phost->EnumState = phost->EnumPushedState;
	  }
    break;

  case ENUM_GET_FULL_DEV_DESC:
    /* Get FULL Device Desc  */
    if (USBH_Get_DevDesc(phost, USB_DEVICE_DESC_SIZE) == USBH_OK)
    {
      PRINTF(PSTR("PID: %04X\n"), phost->device.DevDesc.idProduct );
      PRINTF(PSTR("VID: %04X\n"), phost->device.DevDesc.idVendor );

      phost->EnumState = ENUM_SET_ADDR;

    }
    break;

  case ENUM_SET_ADDR:
    /* set address */
    if (USBH_SetAddress(phost, USBH_DEVICE_ADDRESS) == USBH_OK)
    {
	  USBH_HandleEnumDelay(phost, ENUM_SET_ADDR2, 5);
	}
	break;

  case ENUM_SET_ADDR2:
    /* set address - after delay */
	{
      phost->device.address = USBH_DEVICE_ADDRESS;

      /* user callback for device address assigned */
      PRINTF(PSTR("Address (#%d) assigned.\n"), phost->device.address);
      phost->EnumState = ENUM_GET_CFG_DESC;

      /* modify control channels to update device address */
      USBH_OpenPipe (phost,
                           phost->Control.pipe_in,
                           0x80,
                           phost->device.address,
                           phost->device.speed,
                           USBH_EP_CONTROL,
                           phost->Control.pipe_size);

      /* Open Control pipes */
      USBH_OpenPipe (phost,
                           phost->Control.pipe_out,
                           0x00,
                           phost->device.address,
                           phost->device.speed,
                           USBH_EP_CONTROL,
                           phost->Control.pipe_size);
    }
    break;

  case ENUM_GET_CFG_DESC:
    /* get standard configuration descriptor (9 bytes) */
    if (USBH_Get_CfgDesc(phost,
                          USB_CONFIGURATION_DESC_SIZE) == USBH_OK)
    {
      phost->EnumState = ENUM_GET_FULL_CFG_DESC;
    }
    break;

  case ENUM_GET_FULL_CFG_DESC:
    /* get FULL config descriptor (config, interface, endpoints) */
    if (USBH_Get_CfgDesc(phost,
                         phost->device.CfgDesc.wTotalLength) == USBH_OK)
    {
      phost->EnumState = ENUM_GET_MFC_STRING_DESC;
    }
    break;

  case ENUM_GET_MFC_STRING_DESC:
    if (phost->device.DevDesc.iManufacturer != 0)
    { /* Check that Manufacturer String is available */

      if (USBH_Get_StringDesc(phost,
                               phost->device.DevDesc.iManufacturer,
                                phost->device.Data ,
                               0xff) == USBH_OK)
      {
        /* User callback for Manufacturing string */
        PRINTF(PSTR("Manufacturer : %s\n"),  (char *)phost->device.Data);
        phost->EnumState = ENUM_GET_PRODUCT_STRING_DESC;

#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
      }
    }
    else
    {
     PRINTF(PSTR("Manufacturer : N/A\n"));
     phost->EnumState = ENUM_GET_PRODUCT_STRING_DESC;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
    }
    break;

  case ENUM_GET_PRODUCT_STRING_DESC:
    if (phost->device.DevDesc.iProduct != 0)
    { /* Check that Product string is available */
      if (USBH_Get_StringDesc(phost,
                               phost->device.DevDesc.iProduct,
                               phost->device.Data,
                               0xff) == USBH_OK)
      {
        /* User callback for Product string */
        PRINTF(PSTR("Product : %s\n"),  (char *)phost->device.Data);
        phost->EnumState = ENUM_GET_SERIALNUM_STRING_DESC;
      }
    }
    else
    {
      PRINTF(PSTR("Product : N/A\n"));
      phost->EnumState = ENUM_GET_SERIALNUM_STRING_DESC;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
    }
    break;

  case ENUM_GET_SERIALNUM_STRING_DESC:
    if (phost->device.DevDesc.iSerialNumber != 0)
    { /* Check that Serial number string is available */
      if (USBH_Get_StringDesc(phost,
                               phost->device.DevDesc.iSerialNumber,
                               phost->device.Data,
                               0xff) == USBH_OK)
      {
        /* User callback for Serial number string */
         PRINTF(PSTR("Serial Number : %s\n"),  (char *)phost->device.Data);
        Status = USBH_OK;
      }
    }
    else
    {
      PRINTF(PSTR("Serial Number : N/A\n"));
      Status = USBH_OK;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
    }
    break;

  default:
    break;
  }
  return Status;
}

static void USBH_ProcessDelay(
	USBH_HandleTypeDef *phost,
	HOST_StateTypeDef state,
	unsigned delayMS
	)
{
	const uint_fast16_t nticks = NTICKS(delayMS);
	if (nticks > 1)
	{
		phost->gPushTicks = nticks;
		phost->gPushState = state;
		phost->gState = HOST_DELAY;
	}
	else
	{
		phost->gState = state;
	}
}


/**
  * @brief  USBH_Process
  *         Background process of the USB Core.
  * @param  phost: Host Handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_Process(USBH_HandleTypeDef *phost)
{
	USBH_StatusTypeDef status = USBH_FAIL;

	switch (phost->gState)
	{
	case HOST_IDLE:
		if (phost->device.is_connected)
		{
			/* Wait for 200 ms after connection */
			USBH_ProcessDelay(phost, HOST_DEV_BUS_RESET_ON, 200);
		}
		break;

	case HOST_DEV_BUS_RESET_ON:
		//PRINTF(PSTR("USBH_Process: HOST_DEV_BUS_RESET_ON\n"));
		USBH_LL_ResetPort(phost, 1);
		USBH_ProcessDelay(phost, HOST_DEV_BUS_RESET_OFF, 50);
		break;

	case HOST_DEV_BUS_RESET_OFF:
		//PRINTF(PSTR("USBH_Process: HOST_DEV_BUS_RESET_OFF\n"));
		USBH_LL_ResetPort(phost, 0);

	#if (USBH_USE_OS == 1)
			osMessagePut ( phost->os_event, USBH_PORT_EVENT, 0);
	#endif
		phost->gState = HOST_DEV_WAIT_FOR_ATTACHMENT;
		break;

	case HOST_DELAY:
		if (-- phost->gPushTicks == 0)
			phost->gState = phost->gPushState;
		break;

	case HOST_DEV_WAIT_FOR_ATTACHMENT:
		//PRINTF(PSTR("USBH_Process: HOST_DEV_WAIT_FOR_ATTACHMENT\n"));
		break;

	case HOST_DEV_BEFORE_ATTACHED:
		// в этом состоянии игнорируем disconnect - возникает при старте устройства с установленным USB устройством
		  //PRINTF(PSTR("USBH_Process: HOST_DEV_BEFORE_ATTACHED\n"));
		/* Wait for 100 ms after Reset */
		USBH_ProcessDelay(phost, HOST_DEV_ATTACHED, 100);
		break;

	case HOST_DEV_ATTACHED:
		  //PRINTF(PSTR("USBH_Process: HOST_DEV_ATTACHED\n"));
		/* после таймаута */
		phost->device.speed = USBH_LL_GetSpeed(phost);

		phost->gState = HOST_ENUMERATION;

		phost->Control.pipe_out = USBH_AllocPipe (phost, 0x00);
		phost->Control.pipe_in  = USBH_AllocPipe (phost, 0x80);


		/* Open Control pipes */
		USBH_OpenPipe (phost,
			phost->Control.pipe_in,
			0x80,
			phost->device.address,
			phost->device.speed,
			USBH_EP_CONTROL,
			phost->Control.pipe_size);

		/* Open Control pipes */
		USBH_OpenPipe (phost,
			phost->Control.pipe_out,
			0x00,
			phost->device.address,
			phost->device.speed,
			USBH_EP_CONTROL,
			phost->Control.pipe_size);

#if (USBH_USE_OS == 1)
		osMessagePut ( phost->os_event, USBH_PORT_EVENT, 0);
#endif

		break;

	case HOST_ENUMERATION:
		//PRINTF(PSTR("USBH_Process: HOST_ENUMERATION\n"));
		/* Check for enumeration status */
		if (USBH_HandleEnum(phost) == USBH_OK)
		{
			/* The function shall return USBH_OK when full enumeration is complete */
			PRINTF(PSTR("Enumeration done.\n"));
			phost->device.current_interface = 0;
			if (phost->device.DevDesc.bNumConfigurations == 1)
			{
				PRINTF(PSTR("This device has only 1 configuration.\n"));
				phost->gState = HOST_SET_CONFIGURATION;
			}
			else
			{
				phost->gState = HOST_INPUT;
			}

		}
		break;

  case HOST_INPUT:
	  //PRINTF(PSTR("USBH_Process: HOST_INPUT\n"));
    {
      /* user callback for end of device basic enumeration */
      if (phost->pUser != NULL)
      {
        phost->pUser(phost, HOST_USER_SELECT_CONFIGURATION);
        phost->gState = HOST_SET_CONFIGURATION;

#if (USBH_USE_OS == 1)
        osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
      }
    }
    break;

  case HOST_SET_CONFIGURATION:
	  //PRINTF(PSTR("USBH_Process: HOST_SET_CONFIGURATION\n"));
    /* set configuration */
    if (USBH_SetCfg(phost, phost->device.CfgDesc.bConfigurationValue) == USBH_OK)
    {
      phost->gState  = HOST_CHECK_CLASS;
      PRINTF(PSTR("Default configuration set.\n"));

    }
    break;

	case HOST_CHECK_CLASS:
		// Если USBH_GetActiveClass(phost) == 0x01 - работаем с составным устройством.
		//PRINTF(PSTR("USBH_Process: HOST_CHECK_CLASS (0x%02X)\n"), (unsigned) USBH_GetActiveClass(phost));
		if (phost->ClassNumber == 0)
		{
			PRINTF(PSTR("No Class has been registered.\n"));
			phost->gState = HOST_ABORT_STATE;
		}
		else
		{
			uint_fast8_t idx = 0;
			phost->pActiveClass = NULL;

			for (idx = 0; idx < phost->ClassNumber; idx ++)
			{
				if (phost->pClass [idx] != NULL && phost->pClass [idx]->ClassCode == USBH_GetActiveClass(phost))
				{
					phost->pActiveClass = phost->pClass [idx];
					break;	// или должен был последний из встретив
				}
			}

			if (phost->pActiveClass != NULL)
			{
				if (phost->pActiveClass->Init(phost) == USBH_OK)
				{
					phost->gState  = HOST_CLASS_REQUEST;
					PRINTF(PSTR("%s class started.\n"), phost->pActiveClass->Name);

					/* Inform user that a class has been activated */
					phost->pUser(phost, HOST_USER_CLASS_SELECTED);
				}
				else
				{
					phost->gState = HOST_ABORT_STATE;
					PRINTF(PSTR("Device not supporting %s class.\n"), phost->pActiveClass->Name);
				}
			}
			else
			{
				phost->gState  = HOST_ABORT_STATE;
				PRINTF(PSTR("No registered class for this device.\n"));
			}
		}

#if (USBH_USE_OS == 1)
		osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
    break;

  case HOST_CLASS_REQUEST:
	  //PRINTF(PSTR("USBH_Process: HOST_CLASS_REQUEST\n"));
    /* process class standard control requests state machine */
    if (phost->pActiveClass != NULL)
    {
      status = phost->pActiveClass->Requests(phost);

      if(status == USBH_OK)
      {
        phost->gState  = HOST_CLASS;
      }
    }
    else
    {
      phost->gState  = HOST_ABORT_STATE;
      PRINTF(PSTR("Invalid Class Driver.\n"));

#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
    }
    break;

  case HOST_CLASS:
	  //PRINTF(PSTR("USBH_Process: HOST_CLASS\n"));
    /* process class state machine */
    if (phost->pActiveClass != NULL)
    {
      phost->pActiveClass->BgndProcess(phost);
    }
    break;

  case HOST_DEV_DISCONNECTED :
	  //PRINTF(PSTR("USBH_Process: HOST_DEV_DISCONNECTED\n"));
    DeInitStateMachine(phost);

    /* Re-Initilaize Host for new Enumeration */
    if (phost->pActiveClass != NULL)
    {
      phost->pActiveClass->DeInit(phost);
      phost->pActiveClass = NULL;
    }
    break;

  case HOST_ABORT_STATE:
	  //PRINTF(PSTR("USBH_Process: HOST_ABORT_STATE\n"));
    break;

  default :
	  TP();
	  //PRINTF(PSTR("USBH_Process: default\n"));
    break;
  }
 return USBH_OK;
}

#if CPUSTYLE_STM32
//++++ host interrupt handlers

/**
  * @brief  This function handles Rx Queue Level interrupt requests.
  * @param  hhcd: HCD handle
  * @retval none
  */
static void HCD_RXQLVL_IRQHandler(HCD_HandleTypeDef *hhcd)
{
	USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;

	const uint_fast32_t grxstsp = hhcd->Instance->GRXSTSP;
	const uint_fast8_t channelnum = (grxstsp & USB_OTG_GRXSTSP_EPNUM) >> USB_OTG_GRXSTSP_EPNUM_Pos;
	const uint_fast32_t pktsts = (grxstsp & USB_OTG_GRXSTSP_PKTSTS) >> USB_OTG_GRXSTSP_PKTSTS_Pos;
	const uint_fast32_t pktcnt = (grxstsp & USB_OTG_GRXSTSP_BCNT) >> USB_OTG_GRXSTSP_BCNT_Pos;

	switch (pktsts)
	{
	case GRXSTS_PKTSTS_IN:
		/* Read the data into the host buffer. */
		if ((pktcnt > 0) && (hhcd->hc [channelnum].xfer_buff != (void *)0))
		{

			USB_ReadPacket(hhcd->Instance, hhcd->hc[channelnum].xfer_buff, pktcnt);

			/*manage multiple Xfer */
			hhcd->hc [channelnum].xfer_buff += pktcnt;
			hhcd->hc [channelnum].xfer_count += pktcnt;

			if ((USBx_HC(channelnum)->HCTSIZ & USB_OTG_HCTSIZ_PKTCNT) > 0)
			{
				/* re-activate the channel when more packets are expected */
				USBx_HC(channelnum)->HCCHAR &= ~ USB_OTG_HCCHAR_CHDIS;
				USBx_HC(channelnum)->HCCHAR |= USB_OTG_HCCHAR_CHENA;
				hhcd->hc [channelnum].toggle_in ^= 1;
			}
		}
		break;

	case GRXSTS_PKTSTS_DATA_TOGGLE_ERR:
		break;

	case GRXSTS_PKTSTS_IN_XFER_COMP:
	case GRXSTS_PKTSTS_CH_HALTED:
	default:
		break;
	}
}

/**
  * @brief  This function handles Host Port interrupt requests.
  * @param  hhcd: HCD handle
  * @retval None
  */
static void HCD_Port_IRQHandler(HCD_HandleTypeDef *hhcd)
{
  USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;
  uint32_t hprt0, hprt0_dup;

  /* Handle Host Port Interrupts */
  hprt0 = USBx_HPRT0;
  hprt0_dup = USBx_HPRT0;

  hprt0_dup &= ~(USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET |
                 USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG );

  /* Check whether Port Connect detected */
  if ((hprt0 & USB_OTG_HPRT_PCDET) == USB_OTG_HPRT_PCDET)
  {
    if ((hprt0 & USB_OTG_HPRT_PCSTS) == USB_OTG_HPRT_PCSTS)
    {
      USB_MASK_INTERRUPT(hhcd->Instance, USB_OTG_GINTSTS_DISCINT);
      HAL_HCD_Connect_Callback(hhcd);
    }
    hprt0_dup  |= USB_OTG_HPRT_PCDET;

  }

  /* Check whether Port Enable Changed */
  if ((hprt0 & USB_OTG_HPRT_PENCHNG) == USB_OTG_HPRT_PENCHNG)
  {
    hprt0_dup |= USB_OTG_HPRT_PENCHNG;

    if ((hprt0 & USB_OTG_HPRT_PENA) == USB_OTG_HPRT_PENA)
    {
      if(hhcd->Init.phy_itface  == USB_OTG_EMBEDDED_PHY)
      {
        if ((hprt0 & USB_OTG_HPRT_PSPD) == (HPRT0_PRTSPD_LOW_SPEED << 17))
        {
          USB_InitFSLSPClkSel(hhcd->Instance ,HCFG_6_MHZ );
        }
        else
        {
          USB_InitFSLSPClkSel(hhcd->Instance ,HCFG_48_MHZ );
        }
      }
      else
      {
        if (hhcd->Init.speed == HCD_SPEED_FULL)
        {
          USBx_HOST->HFIR = (uint32_t)60000;
        }
      }
      HAL_HCD_Connect_Callback(hhcd);

      if (hhcd->Init.speed == HCD_SPEED_HIGH)
      {
        USB_UNMASK_INTERRUPT(hhcd->Instance, USB_OTG_GINTSTS_DISCINT);
      }
    }
    else
    {
      /* Cleanup HPRT */
      USBx_HPRT0 &= ~(USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET |
        USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG );

      USB_UNMASK_INTERRUPT(hhcd->Instance, USB_OTG_GINTSTS_DISCINT);
    }
  }

  /* Check For an overcurrent */
  if ((hprt0 & USB_OTG_HPRT_POCCHNG) == USB_OTG_HPRT_POCCHNG)
  {
    hprt0_dup |= USB_OTG_HPRT_POCCHNG;
  }

  /* Clear Port Interrupts */
  USBx_HPRT0 = hprt0_dup;
}
/**
  * @brief  This function handles Host Channel IN interrupt requests.
  * @param  hhcd: HCD handle
  * @param  chnum: Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval none
  */
static void HCD_HC_IN_IRQHandler(HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;

  if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_AHBERR)
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_AHBERR);
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
  }
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_ACK)
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_ACK);
  }

  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_STALL)
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
    hhcd->hc[chnum].state = HC_STALL;
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK);
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_STALL);
    USB_HC_Halt(hhcd->Instance, chnum);
  }
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_DTERR)
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
    USB_HC_Halt(hhcd->Instance, chnum);
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK);
    hhcd->hc[chnum].state = HC_DATATGLERR;
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_DTERR);
  }

  if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_FRMOR)
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
    USB_HC_Halt(hhcd->Instance, chnum);
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_FRMOR);
  }

  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_XFRC)
  {

    if (hhcd->Init.dma_enable)
    {
      hhcd->hc[chnum].xfer_count = hhcd->hc[chnum].xfer_len -
                               (USBx_HC(chnum)->HCTSIZ & USB_OTG_HCTSIZ_XFRSIZ);
    }

    hhcd->hc[chnum].state = HC_XFRC;
    hhcd->hc[chnum].ErrCnt = 0;
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_XFRC);


    if ((hhcd->hc[chnum].ep_type == USBD_EP_TYPE_CTRL)||
        (hhcd->hc[chnum].ep_type == USBD_EP_TYPE_BULK))
    {
      __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
      USB_HC_Halt(hhcd->Instance, chnum);
      __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK);

    }
    else if(hhcd->hc[chnum].ep_type == USBD_EP_TYPE_INTR)
    {
      USBx_HC(chnum)->HCCHAR |= USB_OTG_HCCHAR_ODDFRM;
      hhcd->hc[chnum].urb_state = URB_DONE;
      HAL_HCD_HC_NotifyURBChange_Callback(hhcd, chnum, hhcd->hc[chnum].urb_state);
    }
    hhcd->hc[chnum].toggle_in ^= 1;

  }
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_CHH)
  {
    __HAL_HCD_MASK_HALT_HC_INT(chnum);

    if(hhcd->hc[chnum].state == HC_XFRC)
    {
      hhcd->hc[chnum].urb_state  = URB_DONE;
    }

    else if (hhcd->hc[chnum].state == HC_STALL)
    {
      hhcd->hc[chnum].urb_state  = URB_STALL;
    }

    else if ((hhcd->hc[chnum].state == HC_XACTERR) ||
            (hhcd->hc[chnum].state == HC_DATATGLERR))
    {
      if(hhcd->hc[chnum].ErrCnt++ > 3)
      {
        hhcd->hc[chnum].ErrCnt = 0;
        hhcd->hc[chnum].urb_state = URB_ERROR;
      }
      else
      {
        hhcd->hc[chnum].urb_state = URB_NOTREADY;
      }

      /* re-activate the channel  */
      USBx_HC(chnum)->HCCHAR &= ~USB_OTG_HCCHAR_CHDIS;
      USBx_HC(chnum)->HCCHAR |= USB_OTG_HCCHAR_CHENA;
    }
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_CHH);
    HAL_HCD_HC_NotifyURBChange_Callback(hhcd, chnum, hhcd->hc[chnum].urb_state);
  }

  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_TXERR)
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
     hhcd->hc[chnum].ErrCnt++;
     hhcd->hc[chnum].state = HC_XACTERR;
     USB_HC_Halt(hhcd->Instance, chnum);
     __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_TXERR);
  }
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_NAK)
  {
    if(hhcd->hc[chnum].ep_type == USBD_EP_TYPE_INTR)
    {
      __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
      USB_HC_Halt(hhcd->Instance, chnum);
    }

    hhcd->hc[chnum].state = HC_NAK;
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK);

    if  ((hhcd->hc[chnum].ep_type == USBD_EP_TYPE_CTRL)||
         (hhcd->hc[chnum].ep_type == USBD_EP_TYPE_BULK))
    {
      /* re-activate the channel  */
      USBx_HC(chnum)->HCCHAR &= ~USB_OTG_HCCHAR_CHDIS;
      USBx_HC(chnum)->HCCHAR |= USB_OTG_HCCHAR_CHENA;
    }
  }
}

/**
  * @brief  This function handles Host Channel OUT interrupt requests.
  * @param  hhcd: HCD handle
  * @param  chnum: Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval none
  */
static void HCD_HC_OUT_IRQHandler(HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;

  if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_AHBERR)
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_AHBERR);
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
  }
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_ACK)
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_ACK);

    if( hhcd->hc[chnum].do_ping == 1)
    {
      hhcd->hc[chnum].state = HC_NYET;
      __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
      USB_HC_Halt(hhcd->Instance, chnum);
      hhcd->hc[chnum].urb_state  = URB_NOTREADY;
    }
  }

  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_NYET)
  {
    hhcd->hc[chnum].state = HC_NYET;
    hhcd->hc[chnum].ErrCnt= 0;
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
    USB_HC_Halt(hhcd->Instance, chnum);
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NYET);

  }

  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_FRMOR)
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
    USB_HC_Halt(hhcd->Instance, chnum);
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_FRMOR);
  }

  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_XFRC)
  {
      hhcd->hc[chnum].ErrCnt = 0;
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
    USB_HC_Halt(hhcd->Instance, chnum);
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_XFRC);
    hhcd->hc[chnum].state = HC_XFRC;

  }

  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_STALL)
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_STALL);
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
    USB_HC_Halt(hhcd->Instance, chnum);
    hhcd->hc[chnum].state = HC_STALL;
  }

  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_NAK)
  {
    hhcd->hc[chnum].ErrCnt = 0;
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
    USB_HC_Halt(hhcd->Instance, chnum);
    hhcd->hc[chnum].state = HC_NAK;
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK);
  }

  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_TXERR)
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
    USB_HC_Halt(hhcd->Instance, chnum);
    hhcd->hc[chnum].state = HC_XACTERR;
     __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_TXERR);
  }

  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_DTERR)
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
    USB_HC_Halt(hhcd->Instance, chnum);
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK);
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_DTERR);
    hhcd->hc[chnum].state = HC_DATATGLERR;
  }


  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_CHH)
  {
    __HAL_HCD_MASK_HALT_HC_INT(chnum);

    if(hhcd->hc[chnum].state == HC_XFRC)
    {
      hhcd->hc[chnum].urb_state  = URB_DONE;
      if (hhcd->hc[chnum].ep_type == USBD_EP_TYPE_BULK)
      {
        hhcd->hc[chnum].toggle_out ^= 1;
      }
    }
    else if (hhcd->hc[chnum].state == HC_NAK)
    {
      hhcd->hc[chnum].urb_state  = URB_NOTREADY;
    }

    else if (hhcd->hc[chnum].state == HC_NYET)
    {
      hhcd->hc[chnum].urb_state  = URB_NOTREADY;
      hhcd->hc[chnum].do_ping = 0;
    }

    else if (hhcd->hc[chnum].state == HC_STALL)
    {
      hhcd->hc[chnum].urb_state  = URB_STALL;
    }

    else if ((hhcd->hc[chnum].state == HC_XACTERR) ||
            (hhcd->hc[chnum].state == HC_DATATGLERR))
    {
      if(hhcd->hc[chnum].ErrCnt++ > 3)
      {
        hhcd->hc[chnum].ErrCnt = 0;
        hhcd->hc[chnum].urb_state = URB_ERROR;
      }
      else
      {
        hhcd->hc[chnum].urb_state = URB_NOTREADY;
      }

      /* re-activate the channel  */
      USBx_HC(chnum)->HCCHAR &= ~USB_OTG_HCCHAR_CHDIS;
      USBx_HC(chnum)->HCCHAR |= USB_OTG_HCCHAR_CHENA;
    }

    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_CHH);
    HAL_HCD_HC_NotifyURBChange_Callback(hhcd, chnum, hhcd->hc[chnum].urb_state);
  }
}

#endif /* CPUSTYLE_STM32 */

/*******************************************************************************
                       LL Driver Callbacks (HCD -> USB Host Library)
*******************************************************************************/

/**
  * @brief  SOF callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_IncTimer (hhcd->pData);
}

/**
  * @brief  SOF callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_Connect(hhcd->pData);
}

/**
  * @brief  SOF callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_Disconnect(hhcd->pData);
}

/**
  * @brief  Notify URB state change callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state)
{
  /* To be used with OS to sync URB state with the global state machine */
#if (USBH_USE_OS == 1)
  USBH_LL_NotifyURBChange(hhcd->pData);
#endif
}

//---- host interrupt handlers
#if CPUSTYLE_STM32
/**
  * @brief  This function handles HCD interrupt request.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_IRQHandler(HCD_HandleTypeDef *hhcd)
{
	USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;

	/* ensure that we are in device mode */
	if (USB_GetMode(hhcd->Instance) == USB_OTG_MODE_HOST)
	{
		/* avoid spurious interrupt */
		if (__HAL_HCD_IS_INVALID_INTERRUPT(hhcd))
		{
			return;
		}

		if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT))
		{
			/* incorrect mode, acknowledge the interrupt */
			__HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT);
		}

		if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_IISOIXFR))
		{
			/* incorrect mode, acknowledge the interrupt */
			__HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_IISOIXFR);
		}

		if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_PTXFE))
		{
			/* incorrect mode, acknowledge the interrupt */
			__HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_PTXFE);
		}

		if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_MMIS))
		{
			/* incorrect mode, acknowledge the interrupt */
			__HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_MMIS);
		}

		/* Handle Host Disconnect Interrupts */
		if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_DISCINT))
		{

			/* Cleanup HPRT */
			USBx_HPRT0 &= ~(USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET |
			USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG );

			/* Handle Host Port Interrupts */
			HAL_HCD_Disconnect_Callback(hhcd);
			USB_InitFSLSPClkSel(hhcd->Instance, HCFG_48_MHZ );
			__HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_DISCINT);
		}

		/* Handle Host Port Interrupts */
		if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_HPRTINT))
		{
			HCD_Port_IRQHandler (hhcd);
		}

		/* Handle Host SOF Interrupts */
		if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_SOF))
		{
			HAL_HCD_SOF_Callback(hhcd);
			__HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_SOF);
		}

		/* Handle Host channel Interrupts */
		if (__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_HCINT))
		{
			uint32_t i;
			const uint32_t interrupt = USB_HC_ReadInterrupt(hhcd->Instance);
			for (i = 0; i < hhcd->Init.Host_channels ; i++)
			{
				if (interrupt & (1 << i))
				{
					if ((USBx_HC(i)->HCCHAR) &  USB_OTG_HCCHAR_EPDIR)
					{
						HCD_HC_IN_IRQHandler (hhcd, i);
					}
					else
					{
						HCD_HC_OUT_IRQHandler (hhcd, i);
					}
				}
			}
			__HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_HCINT);
		}

		/* Handle Rx Queue Level Interrupts */
		if(__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_RXFLVL))
		{
			USB_MASK_INTERRUPT(hhcd->Instance, USB_OTG_GINTSTS_RXFLVL);

			HCD_RXQLVL_IRQHandler (hhcd);

			USB_UNMASK_INTERRUPT(hhcd->Instance, USB_OTG_GINTSTS_RXFLVL);
		}
	}
}

#endif /* CPUSTYLE_STM32 */


#if defined (WITHUSBHW_DEVICE)
//++++ сюда переносим используемые хостом функции

#endif /* WITHUSBHW_DEVICE */

#if defined (WITHUSBHW_HOST)
//++++ сюда переносим используемые хостом функции

/**
  * @brief  USBH_LL_Init
  *         Initialize the Low Level portion of the Host driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_LL_Init(USBH_HandleTypeDef *phost)
{
	/* Init USB_IP */
	if (phost->id == HOST_FS)
	{
		/* Link The driver to the stack */
		hhcd_USB_OTG.pData = phost;
		phost->pData = & hhcd_USB_OTG;

		hhcd_USB_OTG.Instance = WITHUSBHW_HOST;
		hhcd_USB_OTG.Init.Host_channels = 16;
		hhcd_USB_OTG.Init.speed = PCD_SPEED_HIGH;
		hhcd_USB_OTG.Init.dma_enable = USB_DISABLE;
		hhcd_USB_OTG.Init.phy_itface = HCD_PHY_EMBEDDED;
		hhcd_USB_OTG.Init.Sof_enable = USB_DISABLE;
		if (HAL_HCD_Init(& hhcd_USB_OTG) != HAL_OK)
		{
			ASSERT(0);
		}

		USBH_LL_SetTimer(phost, HAL_HCD_GetCurrentFrame(&hhcd_USB_OTG));
	}
	return USBH_OK;
}

#endif /* defined (WITHUSBHW_HOST) */

// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
static void
board_usb_spool(void * ctx)
{
#if defined (WITHUSBHW_HOST)
	USBH_Process(& hUsbHost);
#endif /* defined (WITHUSBHW_HOST) */
}

static ticker_t usbticker;

/* вызывается при запрещённых прерываниях. */
void board_usb_initialize(void)
{
	//PRINTF(PSTR("board_usb_initialize start.\n"));

#if defined (WITHUSBHW_DEVICE)
	board_usbd_initialize();	// USB device support
#endif /* defined (WITHUSBHW_DEVICE) */

#if defined (WITHUSBHW_HOST)
	/* Init Host Library,Add Supported Class and Start the library*/
	USBH_Init(& hUsbHost, USBH_UserProcess, HOST_FS);

	#if WITHUSEUSBFLASH
		USBH_RegisterClass(&hUsbHost, & USBH_msc);
	#endif /* WITHUSEUSBFLASH */
	ticker_initialize(& usbticker, 1, board_usb_spool, NULL);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.

#endif /* defined (WITHUSBHW_HOST) */

	//PRINTF(PSTR("board_usb_initialize done.\n"));
}

/* вызывается при разрешённых прерываниях. */
void board_usb_activate(void)
{
	PRINTF(PSTR("board_usb_activate start.\n"));

#if defined (WITHUSBHW_DEVICE)
	USBD_Start(& hUsbDevice);
#endif /* defined (WITHUSBHW_DEVICE) */

#if defined (WITHUSBHW_HOST)
	USBH_Start(& hUsbHost);
#endif /* defined (WITHUSBHW_HOST) */
	PRINTF(PSTR("board_usb_activate done.\n"));
}

/* вызывается при разрешённых прерываниях. */
void board_usb_deactivate(void)
{
	//PRINTF(PSTR("board_usb_activate start.\n"));
#if defined (WITHUSBHW_HOST)
	USBH_Stop(& hUsbHost);
#endif /* defined (WITHUSBHW_HOST) */

#if defined (WITHUSBHW_DEVICE)
	  USBD_Stop(&hUsbDevice);
#endif /* defined (WITHUSBHW_DEVICE) */
	//PRINTF(PSTR("board_usb_activate done.\n"));
}

/* вызывается при разрешенных прерываниях. */
void board_usb_deinitialize(void)
{
#if defined (WITHUSBHW_HOST)
	USBH_DeInit(& hUsbHost);
#endif /* defined (WITHUSBHW_HOST) */

#if defined (WITHUSBHW_DEVICE)
	USBD_DeInit(& hUsbDevice);
#endif /* defined (WITHUSBHW_DEVICE) */
}

uint_fast8_t hamradio_get_usbh_active(void)
{
#if defined (WITHUSBHW_HOST)
	return hUsbHost.device.is_connected != 0;
#else
	return  0;
#endif /* defined (WITHUSBHW_HOST) */
}


#endif /* WITHUSBHW */
