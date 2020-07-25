/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "src/gui/gui.h"
#include "hardware.h"
#include "board.h"
#include "audio.h"

#include "src/display/display.h"
#include "formats.h"
#include <string.h>

#include "gpio.h"

#if WITHUSBHW

#include "usb200.h"
#include "usbch9.h"
#include "usb_core.h"

static uint_fast8_t notseq;

#if CPUSTYLE_R7S721 && WITHUSBUAC
	// на RENESAS для работы с изохронными ендпоинтами используется DMA
	//#define WITHDMAHW_UACIN 1		// при этой опции после пересоединения USB кабеля отвалтвается поток IN
	#define WITHDMAHW_UACOUT 1	// Устойчиво работает - но пропуск пакетов
#endif /* CPUSTYLE_R7S721 */

#if CPUSTYLE_R7S721

static uint_fast8_t
usbd_epaddr2pipe(uint_fast8_t ep_addr)
{
	switch (ep_addr)
	{
	default:
		ASSERT(0);
		return 0;
	case 0x00: return 0;
	case 0x80: return 0;
#if WITHUSBUAC
	case USBD_EP_AUDIO_OUT:	return HARDWARE_USBD_PIPE_ISOC_OUT;
	case USBD_EP_AUDIO_IN:	return HARDWARE_USBD_PIPE_ISOC_IN;
#endif /* WITHUSBUAC */
#if WITHUSBCDC
	case USBD_EP_CDC_OUT:	return HARDWARE_USBD_PIPE_CDC_OUT;
	case USBD_EP_CDC_IN:	return HARDWARE_USBD_PIPE_CDC_IN;
	case USBD_EP_CDC_INT:	return HARDWARE_USBD_PIPE_CDC_INT;
	case USBD_EP_CDC_OUTb:	return HARDWARE_USBD_PIPE_CDC_OUTb;
	case USBD_EP_CDC_INb:	return HARDWARE_USBD_PIPE_CDC_INb;
	case USBD_EP_CDC_INTb:	return HARDWARE_USBD_PIPE_CDC_INTb;
#endif /* WITHUSBCDC */
#if WITHUSBCDCEEM
	case USBD_EP_CDCEEM_OUT:	return HARDWARE_USBD_PIPE_CDCEEM_OUT;
	case USBD_EP_CDCEEM_IN:		return HARDWARE_USBD_PIPE_CDCEEM_IN;
#endif /* WITHUSBCDCEEM */
	}
}

static uint_fast8_t usbd_is_dmapipe(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t pipe)
{
	switch (pipe)
	{
	default: break;
#if WITHDMAHW_UACIN
	case HARDWARE_USBD_PIPE_ISOC_IN:
		  return 1;
#endif /* WITHDMAHW_UACIN */
#if WITHDMAHW_UACOUT
	case HARDWARE_USBD_PIPE_ISOC_OUT:
		  return 1;
#endif /* WITHNDMA_UACOUT */
	}
	return 0;
}

// DCP (PIPE0) как аргумент недопустим
static uint_fast8_t
usbd_pipe2epaddr(uint_fast8_t pipe)
{
	switch (pipe)
	{
	default:
		ASSERT(0);
		return 0;
#if WITHUSBUAC
	case HARDWARE_USBD_PIPE_ISOC_OUT: return USBD_EP_AUDIO_OUT;
	case HARDWARE_USBD_PIPE_ISOC_IN: return USBD_EP_AUDIO_IN;
#endif /* WITHUSBUAC */
#if WITHUSBCDC
	case HARDWARE_USBD_PIPE_CDC_OUT: return USBD_EP_CDC_OUT;
	case HARDWARE_USBD_PIPE_CDC_IN: return USBD_EP_CDC_IN;
	case HARDWARE_USBD_PIPE_CDC_INT: return USBD_EP_CDC_INT;
	case HARDWARE_USBD_PIPE_CDC_OUTb: return USBD_EP_CDC_OUTb;
	case HARDWARE_USBD_PIPE_CDC_INb: return USBD_EP_CDC_INb;
	case HARDWARE_USBD_PIPE_CDC_INTb: return USBD_EP_CDC_INTb;
#endif /* WITHUSBCDC */
#if WITHUSBCDCEEM
	case HARDWARE_USBD_PIPE_CDCEEM_OUT: return USBD_EP_CDCEEM_OUT;
	case HARDWARE_USBD_PIPE_CDCEEM_IN: return USBD_EP_CDCEEM_IN;
#endif /* WITHUSBCDCEEM */
	}
}

static uint_fast8_t
usbd_getpipe(const USB_OTG_EPTypeDef * ep)
{
	if (ep->is_in)
		return usbd_epaddr2pipe(ep->num | 0x80);
	else
		return usbd_epaddr2pipe(ep->num);
}

#endif /* CPUSTYLE_R7S721 */

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
uint_fast32_t
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
unsigned USBD_poke_u32(uint8_t * buff, uint_fast32_t v)
{
	buff [0] = LO_BYTE(v);
	buff [1] = HI_BYTE(v);
	buff [2] = HI_24BY(v);
	buff [3] = HI_32BY(v);

	return 4;
}

/* получить 32-бит значение */
/* Big endian memory layout */
uint_fast32_t
USBD_peek_u32_BE(
	const uint8_t * buff
	)
{
	return
		((uint_fast32_t) buff [0] << 24) +
		((uint_fast32_t) buff [1] << 16) +
		((uint_fast32_t) buff [2] << 8) +
		((uint_fast32_t) buff [3] << 0);
}

/* записать в буфер для ответа 32-бит значение */
/* Big endian memory layout */
unsigned USBD_poke_u32_BE(uint8_t * buff, uint_fast32_t v)
{
	buff [3] = LO_BYTE(v);
	buff [2] = HI_BYTE(v);
	buff [1] = HI_24BY(v);
	buff [0] = HI_32BY(v);

	return 4;
}

/* записать в буфер для ответа 24-бит значение */
unsigned USBD_poke_u24(uint8_t * buff, uint_fast32_t v)
{
	buff [0] = LO_BYTE(v);
	buff [1] = HI_BYTE(v);
	buff [2] = HI_24BY(v);

	return 3;
}

/* записать в буфер для ответа 16-бит значение */
unsigned USBD_poke_u16(uint8_t * buff, uint_fast16_t v)
{
	buff [0] = LO_BYTE(v);
	buff [1] = HI_BYTE(v);

	return 2;
}

/* записать в буфер для ответа 16-бит значение */
/* Big endian memory layout */
unsigned USBD_poke_u16_BE(uint8_t * buff, uint_fast16_t v)
{
	buff [1] = LO_BYTE(v);
	buff [0] = HI_BYTE(v);

	return 2;
}

/* записать в буфер для ответа 8-бит значение */
unsigned USBD_poke_u8(uint8_t * buff, uint_fast8_t v)
{
	buff [0] = v;

	return 1;
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
/*static */ USBALIGN_BEGIN USBH_HandleTypeDef hUSB_Host USBALIGN_END;
static ApplicationTypeDef Appli_state = APPLICATION_IDLE;


#if CPUSTYLE_R7S721

#if WITHDMAHW_UACIN

// Сейчас эта память будет записываться по DMA куда-то
// Потом содержимое не требуется
static uintptr_t
dma_flushxrtstx(uintptr_t addr, unsigned long size)
{
	arm_hardware_flush_invalidate(addr, size);
	return addr;
}

// USB AUDIO
// Канал DMA ещё занят - оставляем в очереди, иначе получить данные через getfilled_dmabufferx
void refreshDMA_uacin(void)
{
	if ((DMAC12.CHSTAT_n & DMAC12_CHSTAT_n_EN) != 0)
	{
		// Канал DMA ещё занят - новые данные не требуются.
		return;
	}
	if (DMAC12.N0SA_n != 0)
	{
		// Ситуация - прерывание по концу передачи возникло сейчас,
		// но ещё не обработано.
		return;
	}

	// При наличии следующего блока - запускаем передачу
	uint_fast16_t size;
	const uintptr_t addr = getfilled_dmabufferx(& size);	// для передачи в компьютер - может вернуть 0
	if (addr != 0)
	{
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_ISOC_IN;	// PIPE2
		WITHUSBHW_DEVICE->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((WITHUSBHW_DEVICE->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		WITHUSBHW_DEVICE->PIPEMAXP = size << USB_PIPEMAXP_MXPS_SHIFT;
		WITHUSBHW_DEVICE->PIPESEL = 0;

		DMAC12.N0SA_n = dma_flushxrtstx(addr, size);
		DMAC12.N0TB_n = size;	// размер в байтах

		//DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_SWRST;		// SWRST
		//DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_CLRINTMSK;	// CLRINTMSK
		DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_SETEN;		// SETEN
	}
	else
	{
		//DMAC12.N0SA_n = 0;
		//DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_CLREN;		// CLREN
		//DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_SWRST;		// SWRST
		//DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_CLRINTMSK;	// CLRINTMSK
	}
}

// USB AUDIO
// DMA по передаче USB0 DMA1 - обработчик прерывания
// DMA по передаче USB1 DMA1 - обработчик прерывания
// Use arm_hardware_flush
// Работает на ARM_REALTIME_PRIORITY
static void RAMFUNC_NONILINE r7s721_usbX_dma1_dmatx_handler(void)
{
	__DMB();
	ASSERT(DMAC12.N0SA_n != 0);
	release_dmabufferx(DMAC12.N0SA_n);

	// При наличии следующего блока - запускаем передачу
	uint_fast16_t size;
	const uintptr_t addr = getfilled_dmabufferx(& size);	// для передачи в компьютер - может вернуть 0
	if (addr != 0)
	{
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_ISOC_IN;	// PIPE2
		WITHUSBHW_DEVICE->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((WITHUSBHW_DEVICE->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
				;
		WITHUSBHW_DEVICE->PIPEMAXP = size << USB_PIPEMAXP_MXPS_SHIFT;
		WITHUSBHW_DEVICE->PIPESEL = 0;

		DMAC12.N0SA_n = dma_flushxrtstx(addr, size);
		DMAC12.N0TB_n = size;	// размер в байтах

		//DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_SWRST;		// SWRST
		//DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_CLRINTMSK;	// CLRINTMSK
		DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_SETEN;		// SETEN
	}
	else
	{
		DMAC12.N0SA_n = 0;
		DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_CLREN;		// CLREN
		//DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_SWRST;		// SWRST
		//DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_CLRINTMSK;	// CLRINTMSK
	}
}


// audio codec
// DMA по передаче USB0 DMA1
// Use arm_hardware_flush

static void r7s721_usb0_dma1_dmatx_initialize(uint_fast8_t pipe)
{
	USB_OTG_GlobalTypeDef * const USBx = & USB200;

	enum { id = 12 };	// 12: DMAC12
	// DMAC12
	/* Set Source Start Address */

    /* Set Destination Start Address */
    DMAC12.N0DA_n = (uintptr_t) & USBx->D1FIFO.UINT32;	// Fixed destination address
    //DMAC12.N1DA_n = (uintptr_t) & USBx->D1FIFO.UINT32;	// Fixed destination address

    /* Set Transfer Size */
    //DMAC12.N0TB_n = DMABUFFSIZE16 * sizeof (aubufv_t);	// размер в байтах
    //DMAC12.N1TB_n = DMABUFFSIZE16 * sizeof (aubufv_t);	// размер в байтах

	// Values from Table 9.4 On-Chip Peripheral Module Requests
	// USB0_DMA1 (channel 1 transmit FIFO empty)
	const uint_fast8_t mid = 0x21;
	const uint_fast8_t rid = 3;
	const uint_fast8_t tm = 0;
	const uint_fast8_t am = 2;
	const uint_fast8_t lvl = 1;
	const uint_fast8_t reqd = 1;
	const uint_fast8_t hien = 1;

	DMAC12.CHCFG_n =
		0 * (1U << DMAC12_CHCFG_n_DMS_SHIFT) |		// DMS	0: Register mode
		0 * (1U << DMAC12_CHCFG_n_REN_SHIFT) |		// REN	0: Does not continue DMA transfers.
		0 * (1U << DMAC12_CHCFG_n_RSW_SHIFT) |		// RSW	1: Inverts RSEL automatically after a DMA transaction.
		0 * (1U << DMAC12_CHCFG_n_RSEL_SHIFT) |		// RSEL	0: Executes the Next0 Register Set
		0 * (1U << DMAC12_CHCFG_n_SBE_SHIFT) |		// SBE	0: Stops the DMA transfer without sweeping the buffer (initial value).
		0 * (1U << DMAC12_CHCFG_n_DEM_SHIFT) |		// DEM	0: Does not mask the DMA transfer end interrupt - прерывания каждый раз после TC
		tm * (1U << DMAC12_CHCFG_n_TM_SHIFT) |		// TM	0: Single transfer mode - берётся из Table 9.4
		1 * (1U << DMAC12_CHCFG_n_DAD_SHIFT) |		// DAD	1: Fixed destination address
		0 * (1U << DMAC12_CHCFG_n_SAD_SHIFT) |		// SAD	0: Increment source address
		2 * (1U << DMAC12_CHCFG_n_DDS_SHIFT) |		// DDS	2: 32 bits, 1: 16 bits (Destination Data Size)
		2 * (1U << DMAC12_CHCFG_n_SDS_SHIFT) |		// SDS	2: 32 bits, 1: 16 bits (Source Data Size)
		am * (1U << DMAC12_CHCFG_n_AM_SHIFT) |		// AM	1: ACK mode: Level mode (active until the transfer request from an on-chip peripheral module
		lvl * (1U << DMAC12_CHCFG_n_LVL_SHIFT) |	// LVL	1: Detects based on the level.
		hien * (1U << DMAC12_CHCFG_n_HIEN_SHIFT) |	// HIEN	1: When LVL = 1: Detects a request when the signal is at the High level.
		reqd * (1U << DMAC12_CHCFG_n_REQD_SHIFT) |	// REQD		Request Direction
		(id & 0x07) * (1U << DMAC12_CHCFG_n_SEL_SHIFT) | // SEL	0: CH0/CH8
		0;

	enum { dmarsshift = (id & 0x01) * 16 };
	DMAC1213.DMARS = (DMAC1213.DMARS & ~ ((DMAC1213_DMARS_CH12_MID | DMAC1213_DMARS_CH12_RID) << dmarsshift)) |
		mid * (1U << (DMAC1213_DMARS_CH12_MID_SHIFT + dmarsshift)) |		// MID
		rid * (1U << (DMAC1213_DMARS_CH12_RID_SHIFT + dmarsshift)) |		// RID
		0;

    DMAC815.DCTRL_0_7 = (DMAC815.DCTRL_0_7 & ~ (/*(1U << 1) | */(1U << 0))) |
		//1 * (1U << 1) |		// LVINT	1: Level output
		1 * (1U << 0) |		// PR		1: Round robin mode
		0;

#if 1
	// Разрешение DMA
	// Сперва без DREQE
	USBx->D1FIFOSEL =
		pipe * (1uL << USB_DnFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
		2 * (1uL << USB_DnFIFOSEL_MBW_SHIFT) |	// MBW 10: 32-bit width
		0 * (1uL << USB_DnFIFOSEL_DREQE_SHIFT) | // DREQE 1: DMA transfer request is enabled.
		0;
	(void) USBx->D1FIFOSEL;

	// Потом выставить DREQE
	USBx->D1FIFOSEL =
		pipe * (1uL << USB_DnFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
		2 * (1uL << USB_DnFIFOSEL_MBW_SHIFT) |	// MBW 10: 32-bit width
		1 * (1uL << USB_DnFIFOSEL_DREQE_SHIFT) | // DREQE 1: DMA transfer request is enabled.
		0;
	(void) USBx->D1FIFOSEL;
#endif

	arm_hardware_set_handler_realtime(DMAINT12_IRQn, r7s721_usbX_dma1_dmatx_handler);

	DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_SWRST;		// SWRST
	DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_CLRINTMSK;	// CLRINTMSK
	//DMAC12.CHCTRL_n = 1 * (1U << 0);		// SETEN
}

// audio codec
// DMA по передаче USB1 DMA1
// Use arm_hardware_flush

static void r7s721_usb1_dma1_dmatx_initialize(uint_fast8_t pipe)
{
	USB_OTG_GlobalTypeDef * const USBx = & USB201;

	enum { id = 12 };	// 12: DMAC12
	// DMAC12
	/* Set Source Start Address */

    /* Set Destination Start Address */
    DMAC12.N0DA_n = (uintptr_t) & USBx->D1FIFO.UINT32;	// Fixed destination address
    //DMAC12.N1DA_n = (uintptr_t) & USBx->D1FIFO.UINT32;	// Fixed destination address

    /* Set Transfer Size */
    //DMAC12.N0TB_n = DMABUFFSIZE16 * sizeof (aubufv_t);	// размер в байтах
    //DMAC12.N1TB_n = DMABUFFSIZE16 * sizeof (aubufv_t);	// размер в байтах

	// Values from Table 9.4 On-Chip Peripheral Module Requests
	// USB1_DMA1 (channel 1 transmit FIFO empty)
	const uint_fast8_t mid = 0x23;
	const uint_fast8_t rid = 3;
	const uint_fast8_t tm = 0;
	const uint_fast8_t am = 2;
	const uint_fast8_t lvl = 1;
	const uint_fast8_t reqd = 1;
	const uint_fast8_t hien = 1;

	DMAC12.CHCFG_n =
		0 * (1U << DMAC12_CHCFG_n_DMS_SHIFT) |		// DMS	0: Register mode
		0 * (1U << DMAC12_CHCFG_n_REN_SHIFT) |		// REN	0: Does not continue DMA transfers.
		0 * (1U << DMAC12_CHCFG_n_RSW_SHIFT) |		// RSW	1: Inverts RSEL automatically after a DMA transaction.
		0 * (1U << DMAC12_CHCFG_n_RSEL_SHIFT) |		// RSEL	0: Executes the Next0 Register Set
		0 * (1U << DMAC12_CHCFG_n_SBE_SHIFT) |		// SBE	0: Stops the DMA transfer without sweeping the buffer (initial value).
		0 * (1U << DMAC12_CHCFG_n_DEM_SHIFT) |		// DEM	0: Does not mask the DMA transfer end interrupt - прерывания каждый раз после TC
		tm * (1U << DMAC12_CHCFG_n_TM_SHIFT) |		// TM	0: Single transfer mode - берётся из Table 9.4
		1 * (1U << DMAC12_CHCFG_n_DAD_SHIFT) |		// DAD	1: Fixed destination address
		0 * (1U << DMAC12_CHCFG_n_SAD_SHIFT) |		// SAD	0: Increment source address
		2 * (1U << DMAC12_CHCFG_n_DDS_SHIFT) |		// DDS	2: 32 bits, 1: 16 bits (Destination Data Size)
		2 * (1U << DMAC12_CHCFG_n_SDS_SHIFT) |		// SDS	2: 32 bits, 1: 16 bits (Source Data Size)
		am * (1U << DMAC12_CHCFG_n_AM_SHIFT) |		// AM	1: ACK mode: Level mode (active until the transfer request from an on-chip peripheral module
		lvl * (1U << DMAC12_CHCFG_n_LVL_SHIFT) |	// LVL	1: Detects based on the level.
		hien * (1U << DMAC12_CHCFG_n_HIEN_SHIFT) |	// HIEN	1: When LVL = 1: Detects a request when the signal is at the High level.
		reqd * (1U << DMAC12_CHCFG_n_REQD_SHIFT) |	// REQD		Request Direction
		(id & 0x07) * (1U << DMAC12_CHCFG_n_SEL_SHIFT) | // SEL	0: CH0/CH8
		0;

	enum { dmarsshift = (id & 0x01) * 16 };
	DMAC1213.DMARS = (DMAC1213.DMARS & ~ ((DMAC1213_DMARS_CH12_MID | DMAC1213_DMARS_CH12_RID) << dmarsshift)) |
		mid * (1U << (DMAC1213_DMARS_CH12_MID_SHIFT + dmarsshift)) |		// MID
		rid * (1U << (DMAC1213_DMARS_CH12_RID_SHIFT + dmarsshift)) |		// RID
		0;

    DMAC815.DCTRL_0_7 = (DMAC815.DCTRL_0_7 & ~ (/*(1U << 1) | */(1U << 0))) |
		//1 * (1U << 1) |		// LVINT	1: Level output
		1 * (1U << 0) |		// PR		1: Round robin mode
		0;


#if 1
	// Разрешение DMA
	// Сперва без DREQE
	USBx->D1FIFOSEL =
		pipe * (1uL << USB_DnFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
		2 * (1uL << USB_DnFIFOSEL_MBW_SHIFT) |	// MBW 10: 32-bit width
		0 * (1uL << USB_DnFIFOSEL_DREQE_SHIFT) | // DREQE 1: DMA transfer request is enabled.
		0;
	(void) USBx->D1FIFOSEL;

	// Потом выставить DREQE
	USBx->D1FIFOSEL =
		pipe * (1uL << USB_DnFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
		2 * (1uL << USB_DnFIFOSEL_MBW_SHIFT) |	// MBW 10: 32-bit width
		1 * (1uL << USB_DnFIFOSEL_DREQE_SHIFT) | // DREQE 1: DMA transfer request is enabled.
		0;
	(void) USBx->D1FIFOSEL;
#endif
	arm_hardware_set_handler_realtime(DMAINT12_IRQn, r7s721_usbX_dma1_dmatx_handler);

	DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_SWRST;		// SWRST
	DMAC12.CHCTRL_n = DMAC12_CHCTRL_n_CLRINTMSK;	// CLRINTMSK
	//DMAC12.CHCTRL_n = 1 * (1U << 0);		// SETEN
}

static void r7s721_usb0_dma1_dmatx_stop(uint_fast8_t pipe)
{
	USB_OTG_GlobalTypeDef * const USBx = & USB200;
	USBx->D1FIFOSEL = 0;
}

static void r7s721_usb1_dma1_dmatx_stop(uint_fast8_t pipe)
{
	USB_OTG_GlobalTypeDef * const USBx = & USB201;
	USBx->D1FIFOSEL = 0;
}

#else /* WITHDMAHW_UACIN */

// USB AUDIO
// Канал DMA ещё занят - оставляем в очереди, иначе получить данные через getfilled_dmabufferx
void refreshDMA_uacin(void)
{
}

#endif /* WITHDMAHW_UACIN */

#if WITHDMAHW_UACOUT

static USBALIGN_BEGIN uint8_t uacoutbuff0 [UACOUT_AUDIO48_DATASIZE] USBALIGN_END;
static USBALIGN_BEGIN uint8_t uacoutbuff1 [UACOUT_AUDIO48_DATASIZE] USBALIGN_END;

// USB AUDIO
// DMA по приему USB0 DMA0 - обработчик прерывания
// DMA по приему USB1 DMA0 - обработчик прерывания
// Работает на ARM_REALTIME_PRIORITY
static RAMFUNC_NONILINE void r7s721_usbX_dma0_dmarx_handler(void)
{
	__DMB();
	// Enable switch to next regidters set
	DMAC13.CHCFG_n |= DMAC13_CHCFG_n_REN;	// REN bit
	// SR (bt 7)
	// Indicates the register set currently selected in register mode.
	// 0: Next0 Register Set
	// 1: Next1 Register Set
	const uint_fast8_t b = (DMAC13.CHSTAT_n & DMAC13_CHSTAT_n_SR) != 0;	// SR
	// Фаза в данном случае отличается от проверенной на передаче в кодек (функция r7s721_ssif0_txdma).
	// Прием с автопереключением больше нигде не подтвержден.
	if (b == 0)
	{
		uacout_buffer_save_realtime(uacoutbuff0, UACOUT_AUDIO48_DATASIZE, UACOUT_AUDIO48_FMT_CHANNELS, UACOUT_AUDIO48_SAMPLEBITS);
		arm_hardware_flush_invalidate((uintptr_t) uacoutbuff0, UACOUT_AUDIO48_DATASIZE);
	}
	else
	{
		uacout_buffer_save_realtime(uacoutbuff1, UACOUT_AUDIO48_DATASIZE, UACOUT_AUDIO48_FMT_CHANNELS, UACOUT_AUDIO48_SAMPLEBITS);
		arm_hardware_flush_invalidate((uintptr_t) uacoutbuff1, UACOUT_AUDIO48_DATASIZE);
	}
}


// USB AUDIO
// DMA по приёму usb0_dma0
static void r7s721_usb0_dma0_dmarx_initialize(uint_fast8_t pipe)
{
	USB_OTG_GlobalTypeDef * const USBx = & USB200;

	arm_hardware_flush_invalidate((uintptr_t) uacoutbuff0, UACOUT_AUDIO48_DATASIZE);
	arm_hardware_flush_invalidate((uintptr_t) uacoutbuff1, UACOUT_AUDIO48_DATASIZE);

	enum { id = 13 };	// 13: DMAC13
	// DMAC13
	/* Set Source Start Address */
	/* регистры USB PIPE (HARDWARE_USBD_PIPE_ISOC_OUT) */
    DMAC13.N0SA_n = (uintptr_t) & USBx->D0FIFO.UINT32;	// Fixed source address
    DMAC13.N1SA_n = (uintptr_t) & USBx->D0FIFO.UINT32;	// Fixed source address

	/* Set Destination Start Address */
	DMAC13.N0DA_n = (uintptr_t) uacoutbuff0;
	DMAC13.N1DA_n = (uintptr_t) uacoutbuff1;

    /* Set Transfer Size */
    DMAC13.N0TB_n = UACOUT_AUDIO48_DATASIZE;	// размер в байтах
    DMAC13.N1TB_n = UACOUT_AUDIO48_DATASIZE;	// размер в байтах

	// Values from Table 9.4 On-Chip Peripheral Module Requests
	// USB0_DMA0 (channel 0 receive FIFO full)
	const uint_fast8_t mid = 0x20;
	const uint_fast8_t rid = 3;
	const uint_fast8_t tm = 0;
	const uint_fast8_t am = 2;
	const uint_fast8_t lvl = 1;
	const uint_fast8_t reqd = 0;
	const uint_fast8_t hien = 1;

	DMAC13.CHCFG_n =
		0 * (1U << 31) |	// DMS	0: Register mode
		1 * (1U << 30) |	// REN	1: Continues DMA transfers.
		1 * (1U << 29) |	// RSW	1: Inverts RSEL automatically after a DMA transaction.
		0 * (1U << 28) |	// RSEL	0: Executes the Next0 Register Set
		0 * (1U << 27) |	// SBE	0: Stops the DMA transfer without sweeping the buffer (initial value).
		0 * (1U << 24) |	// DEM	0: Does not mask the DMA transfer end interrupt - прерывания каждый раз после TC
		tm * (1U << 22) |	// TM	0: Single transfer mode - берётся из Table 9.4
		0 * (1U << 21) |	// DAD	0: Increment destination address
		1 * (1U << 20) |	// SAD	1: Fixed source address
		2 * (1U << 16) |	// DDS	2: 32 bits, 1: 16 bits (Destination Data Size)
		2 * (1U << 12) |	// SDS	2: 32 bits, 1: 16 bits (Source Data Size)
		am * (1U << 8) |	// AM	1: ACK mode: Level mode (active until the transfer request from an on-chip peripheral module
		lvl * (1U << 6) |	// LVL	1: Detects based on the level.
		hien * (1U << 5) |	// HIEN	1: When LVL = 1: Detects a request when the signal is at the High level.
		reqd * (1U << 3) |	// REQD		Request Direction
		(id & 0x07) * (1U << 0) |		// SEL	0: CH0/CH8
		0;

	enum { dmarsshift = (id & 0x01) * 16 };
	DMAC1213.DMARS = (DMAC1213.DMARS & ~ (0x1FFul << dmarsshift)) |
		mid * (1U << (2 + dmarsshift)) |		// MID
		rid * (1U << (0 + dmarsshift)) |		// RID
		0;

    DMAC815.DCTRL_0_7 = (DMAC815.DCTRL_0_7 & ~ (/*(1U << 1) | */(1U << 0))) |
		//1 * (1U << 1) |		// LVINT	1: Level output
		1 * (1U << 0) |		// PR		1: Round robin mode
		0;

#if 1
	// Разрешение DMA
	// Сперва без DREQE
	USBx->D0FIFOSEL =
		pipe * (1uL << USB_DnFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
		2 * (1uL << USB_DnFIFOSEL_MBW_SHIFT) |	// MBW 10: 32-bit width
		0 * (1uL << USB_DnFIFOSEL_DREQE_SHIFT) | // DREQE 1: DMA transfer request is enabled.
		0;
	(void) USBx->D0FIFOSEL;

	// Потом выставить DREQE
	USBx->D0FIFOSEL =
		pipe * (1uL << USB_DnFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
		2 * (1uL << USB_DnFIFOSEL_MBW_SHIFT) |	// MBW 10: 32-bit width
		1 * (1uL << USB_DnFIFOSEL_DREQE_SHIFT) | // DREQE 1: DMA transfer request is enabled.
		0;
	(void) USBx->D0FIFOSEL;
#endif

	arm_hardware_set_handler_realtime(DMAINT13_IRQn, r7s721_usbX_dma0_dmarx_handler);

	DMAC13.CHCTRL_n = DMAC13_CHCTRL_n_SWRST;		// SWRST
	DMAC13.CHCTRL_n = DMAC13_CHCTRL_n_CLRINTMSK;	// CLRINTMSK
	DMAC13.CHCTRL_n = DMAC13_CHCTRL_n_SETEN;		// SETEN
}

// USB AUDIO
// DMA по приёму usb0_dma0
static void r7s721_usb1_dma0_dmarx_initialize(uint_fast8_t pipe)
{
	USB_OTG_GlobalTypeDef * const USBx = & USB201;

	arm_hardware_flush_invalidate((uintptr_t) uacoutbuff0, UACOUT_AUDIO48_DATASIZE);
	arm_hardware_flush_invalidate((uintptr_t) uacoutbuff1, UACOUT_AUDIO48_DATASIZE);

	enum { id = 13 };	// 13: DMAC13
	// DMAC13
	/* Set Source Start Address */
	/* регистры USB PIPE (HARDWARE_USBD_PIPE_ISOC_OUT) */
    DMAC13.N0SA_n = (uintptr_t) & USBx->D0FIFO.UINT32;	// Fixed source address
    DMAC13.N1SA_n = (uintptr_t) & USBx->D0FIFO.UINT32;	// Fixed source address

	/* Set Destination Start Address */
	DMAC13.N0DA_n = (uintptr_t) uacoutbuff0;
	DMAC13.N1DA_n = (uintptr_t) uacoutbuff1;

    /* Set Transfer Size */
    DMAC13.N0TB_n = UACOUT_AUDIO48_DATASIZE;	// размер в байтах
    DMAC13.N1TB_n = UACOUT_AUDIO48_DATASIZE;	// размер в байтах

	// Values from Table 9.4 On-Chip Peripheral Module Requests
	// USB1_DMA0 (channel 0 receive FIFO full)
	const uint_fast8_t mid = 0x22;
	const uint_fast8_t rid = 3;
	const uint_fast8_t tm = 0;
	const uint_fast8_t am = 2;
	const uint_fast8_t lvl = 1;
	const uint_fast8_t reqd = 0;
	const uint_fast8_t hien = 1;

	DMAC13.CHCFG_n =
		0 * (1U << 31) |	// DMS	0: Register mode
		1 * (1U << 30) |	// REN	1: Continues DMA transfers.
		1 * (1U << 29) |	// RSW	1: Inverts RSEL automatically after a DMA transaction.
		0 * (1U << 28) |	// RSEL	0: Executes the Next0 Register Set
		0 * (1U << 27) |	// SBE	0: Stops the DMA transfer without sweeping the buffer (initial value).
		0 * (1U << 24) |	// DEM	0: Does not mask the DMA transfer end interrupt - прерывания каждый раз после TC
		tm * (1U << 22) |	// TM	0: Single transfer mode - берётся из Table 9.4
		0 * (1U << 21) |	// DAD	0: Increment destination address
		1 * (1U << 20) |	// SAD	1: Fixed source address
		2 * (1U << 16) |	// DDS	2: 32 bits, 1: 16 bits (Destination Data Size)
		2 * (1U << 12) |	// SDS	2: 32 bits, 1: 16 bits (Source Data Size)
		am * (1U << 8) |	// AM	1: ACK mode: Level mode (active until the transfer request from an on-chip peripheral module
		lvl * (1U << 6) |	// LVL	1: Detects based on the level.
		hien * (1U << 5) |	// HIEN	1: When LVL = 1: Detects a request when the signal is at the High level.
		reqd * (1U << 3) |	// REQD		Request Direction
		(id & 0x07) * (1U << 0) |		// SEL	0: CH0/CH8
		0;

	enum { dmarsshift = (id & 0x01) * 16 };
	DMAC1213.DMARS = (DMAC1213.DMARS & ~ (0x1FFul << dmarsshift)) |
		mid * (1U << (2 + dmarsshift)) |		// MID
		rid * (1U << (0 + dmarsshift)) |		// RID
		0;

    DMAC815.DCTRL_0_7 = (DMAC815.DCTRL_0_7 & ~ (/*(1U << 1) | */(1U << 0))) |
		//1 * (1U << 1) |		// LVINT	1: Level output
		1 * (1U << 0) |		// PR		1: Round robin mode
		0;

#if 1
	// Разрешение DMA
	// Сперва без DREQE
	USBx->D0FIFOSEL =
		pipe * (1uL << USB_DnFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
		2 * (1uL << USB_DnFIFOSEL_MBW_SHIFT) |	// MBW 10: 32-bit width
		0 * (1uL << USB_DnFIFOSEL_DREQE_SHIFT) | // DREQE 1: DMA transfer request is enabled.
		0;
	(void) USBx->D0FIFOSEL;

	// Потом выставить DREQE
	USBx->D0FIFOSEL =
		pipe * (1uL << USB_DnFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
		2 * (1uL << USB_DnFIFOSEL_MBW_SHIFT) |	// MBW 10: 32-bit width
		1 * (1uL << USB_DnFIFOSEL_DREQE_SHIFT) | // DREQE 1: DMA transfer request is enabled.
		0;
	(void) USBx->D0FIFOSEL;
#endif

    arm_hardware_set_handler_realtime(DMAINT13_IRQn, r7s721_usbX_dma0_dmarx_handler);

	DMAC13.CHCTRL_n = DMAC13_CHCTRL_n_SWRST;		// SWRST
	DMAC13.CHCTRL_n = DMAC13_CHCTRL_n_CLRINTMSK;	// CLRINTMSK
	DMAC13.CHCTRL_n = DMAC13_CHCTRL_n_SETEN;		// SETEN
}

static void r7s721_usb0_dma0_dmarx_stop(uint_fast8_t pipe)
{
	USB_OTG_GlobalTypeDef * const USBx = & USB200;
	USBx->D0FIFOSEL = 0;
}

static void r7s721_usb1_dma0_dmarx_stop(uint_fast8_t pipe)
{
	USB_OTG_GlobalTypeDef * const USBx = & USB201;
	USBx->D0FIFOSEL = 0;
}

#endif /* WITHDMAHW_UACOUT */

#else  /* CPUSTYLE_R7S721 */

// USB AUDIO
// Канал DMA ещё занят - оставляем в очереди, иначе получить данные через getfilled_dmabufferx
void refreshDMA_uacin(void)
{
}

#endif /* CPUSTYLE_R7S721 */

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

enum
{
	USB_PIPE0,
	USB_PIPE1,
	USB_PIPE2,
	USB_PIPE3,
	USB_PIPE4,
	USB_PIPE5,
	USB_PIPE6,
	USB_PIPE7,
	USB_PIPE8,
	USB_PIPE9,
	USB_PIPE10,
	USB_PIPE11,
	USB_PIPE12,
	USB_PIPE13,
	USB_PIPE14,
	USB_PIPE15
};

static volatile uint16_t *get_pipectr_reg(PCD_TypeDef * const USBx, uint_fast8_t pipe)
{
    if (pipe == USB_PIPE0) {
        return & (USBx->DCPCTR);
    } else {
        return & (USBx->PIPE1CTR) + (pipe - USB_PIPE1);
    }
}

static volatile uint16_t * get_pipetre_reg(PCD_TypeDef * const USBx, uint_fast8_t pipe)
{
    if ((pipe >= USB_PIPE1) && (pipe <= USB_PIPE5)) {
        return & (USBx->PIPE1TRE) + ((pipe - USB_PIPE1) * 2);
    } else if ((pipe >= USB_PIPE9) && (pipe <= USB_PIPE10)) {
        return & (USBx->PIPE9TRE) + ((pipe - USB_PIPE9) * 2);
    } else if ((pipe >= USB_PIPE11) && (pipe <= USB_PIPE15)) {
        return & (USBx->PIPEBTRE) + ((pipe - USB_PIPE11) * 2);
    } else {
        return NULL;
    }
}

static volatile uint16_t * get_pipetrn_reg(PCD_TypeDef * const USBx, uint_fast8_t pipe)
{
    if ((pipe >= USB_PIPE1) && (pipe <= USB_PIPE5)) {
        return & (USBx->PIPE1TRN) + ((pipe - USB_PIPE1) * 2);
    } else if ((pipe >= USB_PIPE9) && (pipe <= USB_PIPE10)) {
        return & (USBx->PIPE9TRN) + ((pipe - USB_PIPE9) * 2);
    } else if ((pipe >= USB_PIPE11) && (pipe <= USB_PIPE15)) {
        return & (USBx->PIPEBTRN) + ((pipe - USB_PIPE11) * 2);
    } else {
        return NULL;
    }
}

static volatile uint16_t * get_fifoctr_reg(PCD_TypeDef * const USBx, uint_fast8_t pipe)
{
	return & (USBx->CFIFOCTR);
/*
    uint16_t fifo_use = PIPE2FIFO(pipe);

    if ((fifo_use & USB_FUNCTION_D0FIFO_USE) == USB_FUNCTION_D0FIFO_USE) {
        return & (USBx->D0FIFOCTR);
    } else if ((fifo_use & USB_FUNCTION_D1FIFO_USE) == USB_FUNCTION_D1FIFO_USE) {
        return & (USBx->D1FIFOCTR);
    } else {
        return & (USBx->CFIFOCTR);
    }
*/
}

static volatile uint16_t * get_fifosel_reg(PCD_TypeDef * const USBx, uint_fast8_t pipe)
{
    return & (USBx->CFIFOSEL);

/*
    uint16_t fifo_use = PIPE2FIFO(pipe);

    if ((fifo_use & USB_FUNCTION_D0FIFO_USE) == USB_FUNCTION_D0FIFO_USE) {
        return (uint16_t *) & (USBx->D0FIFOSEL);
    } else if ((fifo_use & USB_FUNCTION_D1FIFO_USE) == USB_FUNCTION_D1FIFO_USE) {
        return (uint16_t *) & (USBx->D1FIFOSEL);
    } else {
        return (uint16_t *) & (USBx->CFIFOSEL);
    }
*/
}

static volatile uint32_t * get_fifo_reg(PCD_TypeDef * const USBx, uint_fast8_t pipe)
{
	return & (USBx->CFIFO.UINT32);

/*
    uint16_t fifo_use = PIPE2FIFO(pipe);

    if ((fifo_use & USB_FUNCTION_D0FIFO_USE) == USB_FUNCTION_D0FIFO_USE) {
        return & (USBx->D0FIFO);
    } else if ((fifo_use & USB_FUNCTION_D1FIFO_USE) == USB_FUNCTION_D1FIFO_USE) {
        return & (USBx->D1FIFO);
    } else {
        return & (USBx->CFIFO);
    }
*/
}


static uint_fast8_t get_pid(PCD_TypeDef * const USBx, uint_fast8_t pipe)
{
    volatile uint16_t * const PIPEnCTR = get_pipectr_reg(USBx, pipe);
    return * PIPEnCTR & USB_PIPEnCTR_1_5_PID;
}

static void set_mbw(PCD_TypeDef * const USBx, uint_fast8_t pipe, uint16_t data)
{
    volatile uint16_t *p_reg;

    p_reg = get_fifosel_reg(USBx, pipe);
    *p_reg &= (~USB_CFIFOSEL_MBW);
    if (data != 0) {
        *p_reg |= data;
    }
}

static void set_pid(PCD_TypeDef * const USBx, uint_fast8_t pipe, uint_fast8_t new_pid)
{
    volatile uint16_t * const p_reg = get_pipectr_reg(USBx, pipe);
    uint16_t old_pid;


    old_pid = get_pid(USBx, pipe);

    switch (new_pid) {
        case DEVDRV_USBF_PID_STALL:
            if ((old_pid & DEVDRV_USBF_PID_BUF) == DEVDRV_USBF_PID_BUF) {
                *p_reg &= (~USB_PIPEnCTR_1_5_PID);
                *p_reg |= DEVDRV_USBF_PID_STALL2;
            } else {
                *p_reg &= (~USB_PIPEnCTR_1_5_PID);
                *p_reg |= new_pid;
            }
            break;
        case DEVDRV_USBF_PID_BUF:
            if (((old_pid & DEVDRV_USBF_PID_STALL) == DEVDRV_USBF_PID_STALL) ||
                    ((old_pid & DEVDRV_USBF_PID_STALL2) == DEVDRV_USBF_PID_STALL2)) {
                *p_reg &= (~USB_PIPEnCTR_1_5_PID);
                *p_reg |= DEVDRV_USBF_PID_NAK;
            }
            *p_reg &= (~USB_PIPEnCTR_1_5_PID);
            *p_reg |= new_pid;
            break;
        case DEVDRV_USBF_PID_NAK:
            if ((old_pid & DEVDRV_USBF_PID_STALL2) == DEVDRV_USBF_PID_STALL2) {
                *p_reg &= (~USB_PIPEnCTR_1_5_PID);
                *p_reg |= DEVDRV_USBF_PID_STALL;
            }
            *p_reg &= (~USB_PIPEnCTR_1_5_PID);
            *p_reg |= new_pid;

            do {
                local_delay_us(1);
                //p_reg = get_pipectr_reg(USBx, pipe);
            } while ((*p_reg & USB_PIPEnCTR_1_5_PBUSY) == USB_PIPEnCTR_1_5_PBUSY);
            break;
        default:
            *p_reg &= (~USB_PIPEnCTR_1_5_PID);
            *p_reg |= new_pid;
            break;
    }
}

#define USBD_FRDY_COUNT_WRITE 10
#define USBD_FRDY_COUNT_READ 10

static uint_fast8_t usbd_wait_fifo(PCD_TypeDef * const USBx, uint_fast8_t pipe, unsigned waitcnt)
{
	while ((USBx->CFIFOSEL & USB_CFIFOSEL_CURPIPE) != (pipe << USB_CFIFOSEL_CURPIPE_SHIFT) || (USBx->CFIFOCTR & USB_CFIFOCTR_FRDY) == 0)	// FRDY
	{
		local_delay_us(1);
		if (-- waitcnt == 0)
		{
            return 1;
		}
	}
	return 0;
}

// Эта функция не должна общаться с DCPCTR - она универсальная
static uint_fast8_t USB_ReadPacketNec(PCD_TypeDef * const USBx, uint_fast8_t pipe, uint8_t * data, unsigned size, unsigned * readcnt)
{
	uint_fast16_t mbw;
	// Допустимое выравнивание смотрим по размеру приемного буфера
	switch (size & 0x03)
	{
	case 0:
		mbw = 0x02;	// MBW 10: 32-bit width
		break;
	case 2:
		mbw = 0x01;	// MBW 01: 16-bit width
		break;
	default:
		mbw = 0x00;	// MBW 00: 8-bit width
		break;
	}

	//PRINTF(PSTR("USB_ReadPacketNec: pipe=%d, data=%p, size=%d\n"), (int) pipe, data, (int) size);
	USBx->CFIFOSEL =
		(pipe << USB_CFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
		(mbw << USB_CFIFOSEL_MBW_SHIFT) |	// MBW 00: 8-bit width
		0;

	if (usbd_wait_fifo(USBx, pipe, USBD_FRDY_COUNT_READ))
	{
		PRINTF(PSTR("USB_ReadPacketNec: usbd_wait_fifo error, pipe=%d, USBx->CFIFOSEL=%08lX\n"), (int) pipe, (unsigned long) USBx->CFIFOSEL);
		return 1;	// error
	}

	ASSERT(size == 0 || data != NULL);
	unsigned dtln = (USBx->CFIFOCTR & USB_CFIFOCTR_DTLN) >> USB_CFIFOCTR_DTLN_SHIFT;
	size = ulmin16(size, dtln);
	* readcnt = size;

	if (size == 0)
	{
		USBx->CFIFOCTR = USB_CFIFOCTR_BCLR;	// BCLR
	}
	else if (mbw == 0x02)
	{
		// MBW 10: 32-bit width
		uint32_t * data32 = (uint32_t *) data;
		size = (size + 3) / 4;
		do
		{
			* data32 ++ = USBx->CFIFO.UINT32;
		} while (-- size);
	}
	else if (mbw == 0x01)
	{
		// MBW 01: 16-bit width
		uint16_t * data16 = (uint16_t *) data;
		size = (size + 1) / 2;
		do
		{
			* data16 ++ = USBx->CFIFO.UINT16 [R_IO_H]; // H=1
		} while (-- size);
	}
	else
	{
		// MBW 00: 8-bit width
		do
		{
			* data ++ = USBx->CFIFO.UINT8 [R_IO_HH]; // HH=3
		} while (-- size);
	}
	return 0;	// OK
}

// Эта функция не должна общаться с DCPCTR - она универсальная
static uint_fast8_t
USB_WritePacketNec(PCD_TypeDef * const USBx, uint_fast8_t pipe, const uint8_t * data, unsigned size)
{
	ASSERT(size == 0 || data != NULL);
#if 0
	if (data != NULL && size != 0)
		PRINTF(PSTR("USB_WritePacketNec: pipe=%d, size=%d, data@%p[]={%02x,%02x,%02x,%02x,%02x,..}\n"), pipe, size, data, data [0], data [1], data [2], data [3], data [4]);
	else if (size == 0 && pipe == 0)
	{
		PRINTF(PSTR("USB_WritePacketNec: DCP ZLP\n"));
	}
	else
	{
		PRINTF(PSTR("USB_WritePacketNec: pipe=%d, size=%d, data[]={}\n"), pipe, size);
	}
#endif

	if (size == 0 && pipe == 0)
	{
		// set pid=buf
		set_pid(USBx, 0, DEVDRV_USBF_PID_BUF);
		return 0;
	}

	const uint_fast16_t cfifosel =
		(pipe << USB_CFIFOSEL_CURPIPE_SHIFT) |	// CURPIPE 0000: DCP
		(0x01 << USB_CFIFOSEL_ISEL_SHIFT_) * (pipe == 0) |	// ISEL 1: Writing to the buffer memory is selected (for DCP)
		0;

	USBx->CFIFOSEL = cfifosel | (0x02 << USB_CFIFOSEL_MBW_SHIFT);	// MBW 10: 32-bit width
	if (usbd_wait_fifo(USBx, pipe, USBD_FRDY_COUNT_WRITE))
	{
		PRINTF(PSTR("USB_WritePacketNec: usbd_wait_fifo error, USBx->CFIFOSEL=%08lX\n"), (unsigned long) USBx->CFIFOSEL);
		return 1;	// error
	}
	ASSERT(size == 0 || data != NULL);
	unsigned size32 = size >> 2;
	unsigned size8 = size & 0x03;
	if (size32 != 0)
	{
		// 32 bit transfers
		const uint32_t * data32 = (const uint32_t *) data;
		do
		{
			USBx->CFIFO.UINT32 = * data32 ++;
		} while (-- size32);
		data = (const uint8_t *) data32;
	}
	if (size8 != 0)
	{
		// switch to 8 bit width (32->8 permitted without wait)
		USBx->CFIFOSEL = cfifosel | (0x00 << USB_CFIFOSEL_MBW_SHIFT);	// MBW 00: 8-bit width
		(void) USBx->CFIFOSEL;
		do
		{
			USBx->CFIFO.UINT8 [R_IO_HH] = * data ++; // R_IO_HH=3
		} while (-- size8);
	}
    USBx->CFIFOCTR = USB_CFIFOCTR_BVAL;	// BVAL
	return 0;	// OK
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
	PRINTF(PSTR("usb_save_request: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"),
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
		//TP();
        /* TX COMPLETE */
		//HAL_PCD_DataInStageCallback(hpcd, 0);
		USBx->DCPCTR |= USB_DCPCTR_CCPL;	// CCPL
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
        //HAL_PCD_DataOutStageCallback(hpcd, 0);
		USBx->DCPCTR |= USB_DCPCTR_CCPL;	// CCPL
		break;

	case 5:
		//PRINTF(PSTR("actions_seq5\n"));
		// 5: Control write (no data) status stage
		// seq5 -> seq0
		usb_save_request(USBx, & pdev->request);
		USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_VALID;	// Clear VALID - in seq 1, 3 and 5
		HAL_PCD_SetupStageCallback(hpcd);
		hpcd->run_later_ctrl_comp = 1;
		//USBx->DCPCTR |= USB_DCPCTR_CCPL;	// CCPL
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

static void
usbd_pipes_initialize(PCD_HandleTypeDef * hpcd)
{
	PCD_TypeDef * const USBx = hpcd->Instance;
	PRINTF(PSTR("usbd_pipes_initialize\n"));
	/*
		at initialize:
		usbd_handler_brdy: после инициализации появляется для тех pipe, у которых dir=0 (read direction)
	*/
	{
		USBx->DCPMAXP = (USB_OTG_MAX_EP0_SIZE << USB_DCPMAXP_MXPS_SHIFT);
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

		USBx->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((USBx->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 3);
		USBx->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |	// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |			// DIR 1: Transmitting direction 0: Receiving direction
			1 * (1u << USB_PIPECFG_TYPE_SHIFT) |			// TYPE 1: Bulk transfer
			1 * (1u << 9) |				// DBLB
			0;
		const unsigned bufsize64 = (VIRTUAL_COM_PORT_OUT_DATA_SIZE + 63) / 64;

		USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		USBx->PIPEMAXP = VIRTUAL_COM_PORT_OUT_DATA_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 2; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		USBx->PIPESEL = 0;
	}
	if (1)
	{
		// Данные CDC в компьютер из трансивера
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_CDC_IN;	// PIPE4
		const uint_fast8_t epnum = USBD_EP_CDC_IN;
		const uint_fast8_t dir = 1;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		USBx->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((USBx->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 4);
		USBx->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			1 * (1u << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 1: Bulk transfer
			1 * USB_PIPECFG_DBLB |		// DBLB
			0;
		const unsigned bufsize64 = (VIRTUAL_COM_PORT_IN_DATA_SIZE + 63) / 64;

		USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		USBx->PIPEMAXP = VIRTUAL_COM_PORT_IN_DATA_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 2; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		USBx->PIPESEL = 0;
	}
	if (1)
	{
		// Прерывание CDC в компьютер из трансивера
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_CDC_INT;	// PIPE6
		const uint_fast8_t epnum = USBD_EP_CDC_INT;
		const uint_fast8_t dir = 1;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		USBx->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((USBx->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 6);
		USBx->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			2 * (1u << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 2: Interrupt transfer
			0 * USB_PIPECFG_DBLB |		// DBLB - для interrupt должен быть 0
			0;
		const unsigned bufsize64 = (VIRTUAL_COM_PORT_INT_SIZE + 63) / 64;
		USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		USBx->PIPEMAXP = VIRTUAL_COM_PORT_INT_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 1; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		USBx->PIPESEL = 0;
	}
	if (1)
	{
		// Данные CDC из компьютера в трансивер
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_CDC_OUTb;	// PIPE14
		const uint_fast8_t epnum = USBD_EP_CDC_OUTb;
		const uint_fast8_t dir = 0;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		USBx->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((USBx->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 14);
		USBx->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |	// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |			// DIR 1: Transmitting direction 0: Receiving direction
			1 * (1u << USB_PIPECFG_TYPE_SHIFT) |			// TYPE 1: Bulk transfer
			1 * (1u << 9) |				// DBLB
			0;
		const unsigned bufsize64 = (VIRTUAL_COM_PORT_OUT_DATA_SIZE + 63) / 64;
		USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		USBx->PIPEMAXP = VIRTUAL_COM_PORT_OUT_DATA_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 2; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		USBx->PIPESEL = 0;
	}
	if (1)
	{
		// Данные CDC в компьютер из трансивера
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_CDC_INb;	// PIPE15
		const uint_fast8_t epnum = USBD_EP_CDC_INb;
		const uint_fast8_t dir = 1;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		USBx->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((USBx->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 15);
		USBx->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			1 * (1u << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 1: Bulk transfer
			1 * USB_PIPECFG_DBLB |		// DBLB
			0;
		const unsigned bufsize64 = (VIRTUAL_COM_PORT_IN_DATA_SIZE + 63) / 64;
		USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		USBx->PIPEMAXP = VIRTUAL_COM_PORT_IN_DATA_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 2; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		USBx->PIPESEL = 0;
	}
	if (1)
	{
		// Прерывание CDC в компьютер из трансивера
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_CDC_INTb;	// PIPE7
		const uint_fast8_t epnum = USBD_EP_CDC_INTb;
		const uint_fast8_t dir = 1;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		USBx->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((USBx->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 7);
		USBx->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			2 * (1u << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 2: Interrupt transfer
			0 * USB_PIPECFG_DBLB |		// DBLB - для interrupt должен быть 0
			0;
		const unsigned bufsize64 = (VIRTUAL_COM_PORT_INT_SIZE + 63) / 64;
		USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		USBx->PIPEMAXP = VIRTUAL_COM_PORT_INT_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 1; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		USBx->PIPESEL = 0;
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
		const uint_fast8_t dblb = 0;	// убрано, т.к PIPEMAXP динамически меняется - поведение не понятно.
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		USBx->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((USBx->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 2);
		USBx->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			3 * (1u << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 11: Isochronous transfer
			dblb * USB_PIPECFG_DBLB |		// DBLB
			0;
		//USBx->PIPEPERI =
		//	1 * (1U << 12) |	// IFS
		//	0;
		const unsigned bufsize64 = (maxpacket + 63) / 64;
		USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		USBx->PIPEMAXP = maxpacket << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * (dblb + 1); // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		USBx->PIPESEL = 0;
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
		const uint_fast16_t maxpacket = UACOUT_AUDIO48_DATASIZE;
		const uint_fast8_t dblb = 1;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		USBx->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((USBx->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 1);
		USBx->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			3 * (1u << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 11: Isochronous transfer
			dblb * USB_PIPECFG_DBLB |		// DBLB
			0;

		const unsigned bufsize64 = (maxpacket + 63) / 64;
		USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		USBx->PIPEMAXP = maxpacket << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * (dblb + 1); // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		USBx->PIPESEL = 0;
	}
#endif /* WITHUSBUAC */

#if WITHUSBCDCEEM
	if (1)
	{
		// Данные CDC EEM из компьютера в трансивер
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_CDCEEM_OUT;	// PIPE12
		const uint_fast8_t epnum = USBD_EP_CDCEEM_OUT;
		const uint_fast8_t dir = 0;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		USBx->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((USBx->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 12);
		USBx->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |	// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |			// DIR 1: Transmitting direction 0: Receiving direction
			1 * (1u << USB_PIPECFG_TYPE_SHIFT) |			// TYPE 1: Bulk transfer
			1 * (1u << 9) |				// DBLB
			0;
		const unsigned bufsize64 = (USBD_CDCEEM_BUFSIZE + 63) / 64;

		USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		USBx->PIPEMAXP = USBD_CDCEEM_BUFSIZE << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 2; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		USBx->PIPESEL = 0;
	}
	if (1)
	{
		// Данные CDC в компьютер из трансивера
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_CDCEEM_IN;	// PIPE13
		const uint_fast8_t epnum = USBD_EP_CDCEEM_IN;
		const uint_fast8_t dir = 1;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		USBx->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((USBx->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 13);
		USBx->PIPECFG =
			(0x0F & epnum) * (1u << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1u << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			1 * (1u << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 1: Bulk transfer
			1 * USB_PIPECFG_DBLB |		// DBLB
			0;
		const unsigned bufsize64 = (USBD_CDCEEM_BUFSIZE + 63) / 64;

		USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		USBx->PIPEMAXP = USBD_CDCEEM_BUFSIZE << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * 2; // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		USBx->PIPESEL = 0;
	}
#endif /* WITHUSBCDCEEM */

	/*
	uint_fast8_t pipe;
	for (pipe = 1; pipe <= 15; ++ pipe)
	{
		USBx->PIPESEL = pipe;
		PRINTF(PSTR("USB pipe%02d PIPEBUF=%04X PIPEMAXP=%u\n"), pipe, USBx->PIPEBUF, USBx->PIPEMAXP & USB_PIPEMAXP_MXPS);
	}
	*/
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
	__DMB();
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
	USBD_HandleTypeDef * const pdev = hpcd->pData;
	const uint_fast16_t intsts0 = USBx->INTSTS0;
	const uint_fast16_t intsts1 = USBx->INTSTS1;
	const uint_fast16_t intsts0msk = intsts0 & USBx->INTENB0;
	const uint_fast16_t intsts1msk = intsts1 & USBx->INTENB1;
	hpcd->run_later_ctrl_comp = 0;

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
			USB_OTG_EPTypeDef * const ep = & hpcd->IN_ep [0];
			// Отсюда вызывается с проблемными параметрами на RENESAS
			//TP();
			/* TX COMPLETE */
			// Использование maxpacket вместо xfer_len важно для обработки персылок больше чем maxpacket
			ep->xfer_buff += ep->maxpacket;	// пересланный размер может отличаться от максимального
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
				if (pipe == 0)
					continue;
				//usbd_handler_nrdy(pdev, pipe);
                /*if (pipe_ctrl[i].enable) */ {
					const uint_fast8_t pid = get_pid(USBx, pipe);
                    if (((pid & DEVDRV_USBF_PID_STALL) != DEVDRV_USBF_PID_STALL) && ((pid & DEVDRV_USBF_PID_STALL2) != DEVDRV_USBF_PID_STALL2)) {
                        set_pid(USBx, pipe, DEVDRV_USBF_PID_BUF);
                    }
                }
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
		  	unsigned bcnt;
		  	if (USB_ReadPacketNec(USBx, 0, ep->xfer_buff, ep->xfer_len - ep->xfer_count, & bcnt) == 0)
		  	{
				ep->xfer_buff += bcnt;
				ep->xfer_count += bcnt;
				HAL_PCD_DataOutStageCallback(hpcd, ep->num);	// start next transfer
		  	}
		  	else
		  	{
		  		TP();
		  		//control_stall(pdev);
		  	}
		}

		for (i = 1; i < 16; ++ i)
		{
			const uint_fast8_t pipe = i;
			if ((brdysts & (1U << pipe)) != 0)
			{
				const uint_fast8_t epnt = usbd_pipe2epaddr(pipe);
				if ((epnt & 0x80) != 0)
				{
					USB_OTG_EPTypeDef * const ep = & hpcd->IN_ep [epnt & 0x7F];
					ASSERT(ep->xfer_len == 0 || ep->xfer_buff != NULL);
					ep->xfer_buff += ep->maxpacket;
					HAL_PCD_DataInStageCallback(hpcd, 0x7f & epnt);
				}
				else
				{
					USB_OTG_EPTypeDef * const ep = & hpcd->OUT_ep [epnt];
				  	unsigned bcnt;
				  	if (USB_ReadPacketNec(USBx, pipe, ep->xfer_buff, ep->xfer_len - ep->xfer_count, & bcnt) == 0)
				  	{
						ep->xfer_buff += bcnt;
						ep->xfer_count += bcnt;
						HAL_PCD_DataOutStageCallback(hpcd, ep->num);	// start next transfer
				  	}
				  	else
				  	{
				  		// todo: not control ep
				  		//control_stall(pdev);
				  		TP();
				  	}
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
				PRINTF("strange srsq=%d\n", (int) ctsq);
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
			if (USB_GetDevSpeed(hpcd->Instance) == USB_OTG_SPEED_HIGH)
			{
				hpcd->Init.pcd_speed = PCD_SPEED_HIGH;
				//hpcd->Init.ep0_mps = USB_OTG_MAX_EP0_SIZE; //USB_OTG_HS_MAX_PACKET_SIZE;
				/*
				hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) |
				(uint32_t)((USBD_HS_TRDT_VALUE << USB_OTG_GUSBCFG_TRDT_Pos) & USB_OTG_GUSBCFG_TRDT) |
				0;
				*/
			}
			else
			{
				hpcd->Init.pcd_speed = PCD_SPEED_FULL;
				//hpcd->Init.ep0_mps = USB_OTG_MAX_EP0_SIZE; //USB_OTG_FS_MAX_PACKET_SIZE ;
			}
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
		//usbd_handle_resume(USBx);
	}
	if ((intsts0msk & USB_INTSTS0_VBINT) != 0)	// VBINT
	{
		USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_VBINT;	// Clear VBINT - enabled by VBSE
		PRINTF(PSTR("HAL_PCD_IRQHandler trapped - VBINT, VBSTS=%d\n"), (intsts0 & USB_INTSTS0_VBSTS) != 0);
	//	usbd_handle_vbuse(usbd_getvbus());
	}
	if (hpcd->run_later_ctrl_comp != 0)
	{
		USBx->DCPCTR &= USB_DCPCTR_PID;
		USBx->DCPCTR |= DEVDRV_USBF_PID_BUF;	// CCPL
		USBx->DCPCTR |= USB_DCPCTR_CCPL;	// CCPL
	}
}

static void RAMFUNC_NONILINE device_USBI0_IRQHandler(void)
{
	HAL_PCD_IRQHandler(& hpcd_USB_OTG);
}

static void RAMFUNC_NONILINE device_USBI1_IRQHandler(void)
{
	HAL_PCD_IRQHandler(& hpcd_USB_OTG);
}

void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{

	if (hpcd->Instance == & USB200)
	{
		arm_hardware_set_handler_system(USBI0_IRQn, device_USBI0_IRQHandler);

		/* ---- Supply clock to the USB20(channel 0) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */

		HARDWARE_USB0_INITIALIZE();

	}
	else if (hpcd->Instance == & USB201)
	{
		arm_hardware_set_handler_system(USBI1_IRQn, device_USBI1_IRQHandler);

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
	__DMB();
	USB_OTG_GlobalTypeDef * const USBx = hhcd->Instance;
	const uint_fast16_t intsts0 = USBx->INTSTS0;
	const uint_fast16_t intsts1 = USBx->INTSTS1;
	const uint_fast16_t intsts0msk = intsts0 & USBx->INTENB0;
	const uint_fast16_t intsts1msk = intsts1 & USBx->INTENB1;

	PRINTF(PSTR("HAL_HCD_IRQHandler trapped, intsts0=%04X, intsts1=%04X\n"), intsts0, intsts1);
	if ((intsts0msk & USB_INTSTS0_SOFR) != 0)	// SOFR
	{
		USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_SOFR;	// Clear SOFR
		//PRINTF(PSTR("HAL_HCD_IRQHandler trapped - SOFR\n"));
	}
	if ((intsts0msk & USB_INTSTS0_BEMP) != 0)	// BEMP
	{
		PRINTF(PSTR("HAL_HCD_IRQHandler trapped - BEMP, BEMPSTS=0x%04X\n"), USBx->BEMPSTS);
		const uint_fast16_t bempsts = USBx->BEMPSTS & USBx->BEMPENB;	// BEMP Interrupt Status Register
		USBx->BEMPSTS = ~ bempsts;
#if 0
		if ((bempsts & (1U << 0)) != 0)	// PIPE0, DCP
		{
			USB_OTG_EPTypeDef * const ep = & hpcd->IN_ep [0];
			// Отсюда вызывается с проблемными параметрами на RENESAS
			//TP();
			/* TX COMPLETE */
			// Использование maxpacket вместо xfer_len важно для обработки персылок больше чем maxpacket
			ep->xfer_buff += ep->maxpacket;	// пересланный размер может отличаться от максимального
			HAL_PCD_DataInStageCallback(hpcd, 0);

			//if (control_transmit2(pdev) != 0)
			//{
			//	control_stall(pdev);
			//	TP();
			//}
		}
#endif
	}
	if ((intsts0msk & USB_INTSTS0_NRDY) != 0)	// NRDY
	{
		uint_fast8_t pipe;
		PRINTF(PSTR("HAL_HCD_IRQHandler trapped - NRDY, NRDYSTS=0x%04X\n"), USBx->NRDYSTS);

/*
	  	unsigned bcnt;
	  	static uint8_t tmpb [256];
	  	if (USB_ReadPacketNec(USBx, 0, tmpb, 256, & bcnt) == 0)
	  	{
	  		printhex(0, tmpb, bcnt);
	  	}
	  	else
	  	{
	  		TP();
	  		//control_stall(pdev);
	  	}
*/

#if 1
		const uint_fast16_t nrdysts = USBx->NRDYSTS & USBx->NRDYENB;	// NRDY Interrupt Status Register
		for (pipe = 0; pipe < 16; ++ pipe)
		{
			const uint_fast16_t mask = (uint_fast16_t) 1 << pipe;
			if ((nrdysts & mask) != 0)
			{
				USBx->NRDYSTS = ~ mask;
				if (pipe == 0)
					continue;
				//usbd_handler_nrdy(pdev, pipe);
                /*if (pipe_ctrl[i].enable) */ {
					const uint_fast8_t pid = get_pid(USBx, pipe);
                    if (((pid & DEVDRV_USBF_PID_STALL) != DEVDRV_USBF_PID_STALL) && ((pid & DEVDRV_USBF_PID_STALL2) != DEVDRV_USBF_PID_STALL2)) {
                        set_pid(USBx, pipe, DEVDRV_USBF_PID_BUF);
                    }
                }
			}
		}
#endif
	}
	if ((intsts0msk & USB_INTSTS0_BRDY) != 0)	// BRDY
	{
		uint_fast8_t i;
		PRINTF(PSTR("HAL_HCD_IRQHandler trapped - BRDY, BRDYSTS=0x%04X\n"), USBx->BRDYSTS);
		const uint_fast16_t brdysts = USBx->BRDYSTS & USBx->BRDYENB;	// BRDY Interrupt Status Register
		USBx->BRDYSTS = ~ brdysts;
#if 0
		if ((brdysts & (1U << 0)) != 0)		// PIPE0 - DCP
		{
			USB_OTG_EPTypeDef * const ep = & hpcd->OUT_ep [0];
		  	unsigned bcnt;
		  	if (USB_ReadPacketNec(USBx, 0, ep->xfer_buff, ep->xfer_len - ep->xfer_count, & bcnt) == 0)
		  	{
				ep->xfer_buff += bcnt;
				ep->xfer_count += bcnt;
				HAL_PCD_DataOutStageCallback(hpcd, ep->num);	// start next transfer
		  	}
		  	else
		  	{
		  		TP();
		  		//control_stall(pdev);
		  	}
		}

		for (i = 1; i < 16; ++ i)
		{
			const uint_fast8_t pipe = i;
			if ((brdysts & (1U << pipe)) != 0)
			{
				const uint_fast8_t epnt = usbd_pipe2epaddr(pipe);
				if ((epnt & 0x80) != 0)
				{
					USB_OTG_EPTypeDef * const ep = & hpcd->IN_ep [epnt & 0x7F];
					ASSERT(ep->xfer_len == 0 || ep->xfer_buff != NULL);
					ep->xfer_buff += ep->maxpacket;
					HAL_PCD_DataInStageCallback(hpcd, 0x7f & epnt);
				}
				else
				{
					USB_OTG_EPTypeDef * const ep = & hpcd->OUT_ep [epnt];
				  	unsigned bcnt;
				  	if (USB_ReadPacketNec(USBx, pipe, ep->xfer_buff, ep->xfer_len - ep->xfer_count, & bcnt) == 0)
				  	{
						ep->xfer_buff += bcnt;
						ep->xfer_count += bcnt;
						HAL_PCD_DataOutStageCallback(hpcd, ep->num);	// start next transfer
				  	}
				  	else
				  	{
				  		// todo: not control ep
				  		//control_stall(pdev);
				  		TP();
				  	}
				}
			}
		}
#endif
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
	if ((intsts1msk & USB_INTSTS1_SIGN) != 0)	// SIGN
	{
		USBx->INTSTS1 = (uint16_t) ~ USB_INTSTS1_SIGN;
		PRINTF(PSTR("HAL_HCD_IRQHandler trapped - SIGN\n"));
		//HAL_HCD_Connect_Callback(hhcd);
	}
	if ((intsts1msk & USB_INTSTS1_SACK) != 0)	// SACK
	{
		USBx->INTSTS1 = (uint16_t) ~ USB_INTSTS1_SACK;
		PRINTF(PSTR("HAL_HCD_IRQHandler trapped - SACK\n"));
		//HAL_HCD_Connect_Callback(hhcd);
		//int err = USB_WritePacketNec(USBx, 0, NULL, 0);	// pipe=0: DCP
		//ASSERT(err == 0);
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
		arm_hardware_set_handler_system(USBI0_IRQn, host_USBI0_IRQHandler);

		/* ---- Supply clock to the USB20(channel 0) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */

		HARDWARE_USB0_INITIALIZE();
	}
	else if (hpcd->Instance == & USB201)
	{
		arm_hardware_set_handler_system(USBI1_IRQn, host_USBI1_IRQHandler);

		/* ---- Supply clock to the USB20(channel 1) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP70;	// Module Stop 70 0: Channel 1 of the USB 2.0 host/function module runs.
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */

		HARDWARE_USB1_INITIALIZE();
	}
}

void HAL_HCD_MspDeInit(HCD_HandleTypeDef* hpcd)
{
	if (hpcd->Instance == & USB200)
	{
		const IRQn_ID_t int_id = USBI0_IRQn;
		IRQ_Disable(int_id);

		/* ---- Supply clock to the USB20(channel 0) ---- */
		//CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		//(void) CPG.STBCR7;			/* Dummy read */

		//HARDWARE_USB0_UNINITIALIZE();

	}
	else if (hpcd->Instance == & USB201)
	{
		const IRQn_ID_t int_id = USBI1_IRQn;
		IRQ_Disable(int_id);

		/* ---- Supply clock to the USB20(channel 1) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP70;	// Module Stop 70 0: Channel 1 of the USB 2.0 host/function module runs.
		//CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */

		//HARDWARE_USB1_UNINITIALIZE();
	}
	hpcd->Instance->SYSCFG0 &= ~ USB_SYSCFG_USBE;
	hpcd->Instance->INTENB0 = 0;
	hpcd->Instance->INTENB1 = 0;
}

/**
  * @brief  Return Host Current Frame number
  * @param  USBx : Selected device
  * @retval current frame number
*/
uint32_t USB_GetCurrentFrame(USB_OTG_GlobalTypeDef *USBx)
{
	uint_fast16_t fn = (USBx->FRMNUM & USB_FRMNUM_FRNM) >> USB_FRMNUM_FRNM_SHIFT;
	for (;;)
	{
		const uint_fast16_t fn2 = (USBx->FRMNUM & USB_FRMNUM_FRNM) >> USB_FRMNUM_FRNM_SHIFT;
		if (fn == fn2)
			break;
		fn = fn2;

	}
	return fn;
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
uint32_t USB_GetHostSpeed(USB_OTG_GlobalTypeDef *USBx)
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
HAL_StatusTypeDef USB_DriveVbus(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t state)
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

static void usbd_attachdma(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t pipe)
{
	switch (pipe)
	{
	default: break;
#if WITHDMAHW_UACIN
	case HARDWARE_USBD_PIPE_ISOC_IN:
		if (USBx == & USB200)
		{
			// USB0 TX DMA
			r7s721_usb0_dma1_dmatx_initialize(pipe);
		}
		else if (USBx == & USB201)
		{
			// USB1 TX DMA
			r7s721_usb1_dma1_dmatx_initialize(pipe);
		}
		else
		{
			ASSERT(0);
		}
		break;
#endif /* WITHDMAHW_UACIN */
#if WITHDMAHW_UACOUT
	case HARDWARE_USBD_PIPE_ISOC_OUT:
		if (USBx == & USB200)
		{
			// USB0 RX DMA
			r7s721_usb0_dma0_dmarx_initialize(pipe);

		}
		else if (USBx == & USB201)
		{
			// USB1 RX DMA
			r7s721_usb1_dma0_dmarx_initialize(pipe);

		}
		else
		{
			ASSERT(0);
		}
		break;
#endif /* WITHNDMA_UACOUT */
	}
}

static void usbd_detachdma(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t pipe)
{
	switch (pipe)
	{
	default: break;
#if WITHDMAHW_UACIN
	case HARDWARE_USBD_PIPE_ISOC_IN:
		if (USBx == & USB200)
		{
			// USB0 TX DMA
			r7s721_usb0_dma1_dmatx_stop(pipe);
		}
		else if (USBx == & USB201)
		{
			// USB1 TX DMA
			r7s721_usb1_dma1_dmatx_stop(pipe);
		}
		else
		{
			ASSERT(0);
		}
		break;
#endif /* WITHDMAHW_UACIN */
#if WITHDMAHW_UACOUT
	case HARDWARE_USBD_PIPE_ISOC_OUT:
		if (USBx == & USB200)
		{
			// USB0 RX DMA
			r7s721_usb0_dma0_dmarx_stop(pipe);

		}
		else if (USBx == & USB201)
		{
			// USB1 RX DMA
			r7s721_usb1_dma0_dmarx_stop(pipe);

		}
		else
		{
			ASSERT(0);
		}
		break;
#endif /* WITHNDMA_UACOUT */
	}
}
/**
  * @brief  Activate and configure an endpoint
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_ActivateEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
{
	const uint_fast8_t pipe = usbd_getpipe(ep);
	volatile uint16_t * const PIPEnCTR = get_pipectr_reg(USBx, pipe);

	* PIPEnCTR = 0x0000;	// NAK
	while ((* PIPEnCTR & (USB_PIPEnCTR_1_5_PBUSY | USB_PIPEnCTR_1_5_CSSTS)) != 0)	// PBUSY, CSSTS
		;

	* PIPEnCTR = USB_PIPEnCTR_1_5_SQCLR;

	* PIPEnCTR = 0x0003;	// NAK->STALL
	* PIPEnCTR = 0x0002;	// NAK->STALL
	* PIPEnCTR = 0x0001;	// STALL->BUF

	USBx->NRDYENB |= (1uL << pipe);
	if (usbd_is_dmapipe(USBx, pipe))
	{
		usbd_attachdma(USBx, pipe);
	}
	else
	{
		// В endpoints, задействованных в DMA обмене, не разрешать.
		USBx->BRDYENB |= (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера

	}

	if (pipe == 0)
	{
		USBx->BEMPENB |= (1uL << pipe);	// Прерывание окончания передачи передающего (IN) буфера
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
	// RENESAS: may be used as DMA abstraction
	const uint_fast8_t pipe = usbd_getpipe(ep);
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
	const uint_fast8_t pipe = usbd_getpipe(ep);
	volatile uint16_t * const PIPEnCTR = get_pipectr_reg(USBx, pipe);

	* PIPEnCTR = 0x0000;	// NAK
	while ((* PIPEnCTR & (USB_PIPEnCTR_1_5_PBUSY | USB_PIPEnCTR_1_5_CSSTS)) != 0)	// PBUSY, CSSTS
		;

	USBx->NRDYENB &= ~ (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера
	USBx->BRDYENB &= ~ (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера
	USBx->BEMPENB &= ~ (1uL << pipe);	// Прерывание окончания передачи передающего (IN) буфера
	if (usbd_is_dmapipe(USBx, pipe))
	{
		usbd_detachdma(USBx, pipe);
	}
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
  * @brief  USB_GetDevSpeed :Return the Dev Speed
  * @param  USBx : Selected device
  * @retval speed : device speed
  *          This parameter can be one of these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  *            @arg USB_OTG_SPEED_LOW: Low speed mode
  */
uint_fast8_t USB_GetDevSpeed(USB_OTG_GlobalTypeDef *USBx)
{
	switch ((USBx->DVSTCTR0 & USB_DVSTCTR0_RHST) >> USB_DVSTCTR0_RHST_SHIFT)
	{
	case 0x02:
		return USB_OTG_SPEED_FULL;
	case 0x03:
		return USB_OTG_SPEED_HIGH;
	default:
		return USB_OTG_SPEED_LOW;
	}
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

		set_pid(USBx, pipe, DEVDRV_USBF_PID_NAK);

		/* IN endpoint */
		if (ep->xfer_len == 0)
		{
			/* Zero Length Packet */
			//int err = USB_WritePacketNec(USBx, pipe, NULL, 0);	// pipe=0: DCP
			//ASSERT(err == 0);
		}
		else
		{
			if (ep->xfer_len > ep->maxpacket)
			{
				ep->xfer_len = ep->maxpacket;
			}
			ASSERT(ep->xfer_len == 0 || ep->xfer_buff != NULL);

			int err = USB_WritePacketNec(USBx, pipe, ep->xfer_buff, ep->xfer_len);	// pipe=0: DCP
			ASSERT(err == 0);
		}

		set_pid(USBx, pipe, DEVDRV_USBF_PID_BUF);
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
	ASSERT(dma == 0);
	if (ep->is_in == 1)
	{
		/* IN endpoint */
		//USB_OTG_INEndpointTypeDef * const inep = USBx_INEP(ep->num);
		//PRINTF(PSTR("USB_EPStartXfer IN, ep->num=%d, ep->pipe_num=%d\n"), (int) ep->num, (int) ep->pipe_num);
		/* IN endpoint */
		int err = USB_WritePacketNec(USBx, usbd_getpipe(ep), ep->xfer_buff, ep->xfer_len);
		////ASSERT(err == 0);
	}
	else
	{
		/* OUT endpoint */
		//USB_OTG_OUTEndpointTypeDef * const outep = USBx_OUTEP(ep->num);
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
	PRINTF("USB_HC_StartXfer, ep_is_in=%d, ch_num=%d, ep_num=%d, xfer_len=%d\n", (int) hc->ep_is_in, (int) hc->ch_num, (int) hc->ep_num, (int) hc->xfer_len);
	ASSERT(dma == 0);
	uint8_t  is_oddframe;
	uint16_t num_packets = 0;
	uint16_t max_hc_pkt_count = 256;
	uint32_t tmpreg = 0;

	const uint_fast8_t pipe = 0;
	volatile uint16_t * const PIPEnCTR = get_pipectr_reg(USBx, pipe);
	volatile uint16_t * const PIPEnTRE = get_pipetre_reg(USBx, pipe);
	volatile uint16_t * const PIPEnTRN = get_pipetrn_reg(USBx, pipe);

	USBx->DVSTCTR0 |= USB_DVSTCTR0_UACT;
	(void) USBx->DVSTCTR0;

	if (hc->usbh_otg_speed == USB_OTG_SPEED_HIGH)
	{
		// HS
		if ((dma == 0) && (hc->do_ping == 1))
		{
			USB_DoPing(USBx, hc->ch_num);
			return HAL_OK;
		}
		else if (dma == 1)
		{
			////* USBx_HC(hc->ch_num)->HCINTMSK &= ~(USB_OTG_HCINTMSK_NYET | USB_OTG_HCINTMSK_ACKM);
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
/*
	  USBx_HC(hc->ch_num)->HCTSIZ = (((hc->xfer_len) & USB_OTG_HCTSIZ_XFRSIZ)) |
	    ((num_packets << USB_OTG_HCTSIZ_PKTCNT_Pos) & USB_OTG_HCTSIZ_PKTCNT) |
	      (((hc->data_pid) << USB_OTG_HCTSIZ_DPID_Pos) & USB_OTG_HCTSIZ_DPID);
*/

	  if (dma)
	  {
	    /* xfer_buff MUST be 32-bits aligned */
	    ////* USBx_HC(hc->ch_num)->HCDMA = (uintptr_t) hc->xfer_buff;
	  }

	  // FRMNUM or UFRMNUM ?
	  ////is_oddframe = (USB_GetCurrentFrame(USBx) & 0x01) ? 0 : 1;

	  ////* USBx_HC(hc->ch_num)->HCCHAR &= ~ USB_OTG_HCCHAR_ODDFRM;
	  ////* USBx_HC(hc->ch_num)->HCCHAR |= (is_oddframe << USB_OTG_HCCHAR_ODDFRM_Pos);

	  /* Set host channel enable */
	  ////* tmpreg = USBx_HC(hc->ch_num)->HCCHAR;
	  ////* tmpreg &= ~USB_OTG_HCCHAR_CHDIS;

	  /* make sure to set the correct ep direction */
	  if (hc->ep_is_in)
	  {
		  ////* tmpreg |= USB_OTG_HCCHAR_EPDIR;
	  }
	  else
	  {
		  ////* tmpreg &= ~USB_OTG_HCCHAR_EPDIR;
	  }

	  ////* tmpreg |= USB_OTG_HCCHAR_CHENA;
	  ////* USBx_HC(hc->ch_num)->HCCHAR = tmpreg;

	  if (dma == 0) /* Slave mode */
	  {
		////uint16_t len_words = 0;
	    if ((hc->ep_is_in == 0) && (hc->xfer_len > 0))
	    {
	      switch(hc->ep_type)
	      {
	        /* Non periodic transfer */
	      case USBD_EP_TYPE_CTRL:
	      case USBD_EP_TYPE_BULK:

	        ////len_words = (hc->xfer_len + 3) / 4;

	        /* check if there is enough space in FIFO space */
	        ////* if(len_words > (USBx->HNPTXSTS & 0xFFFF))
	        {
	          /* need to process data in nptxfempty interrupt */
	          ////* USBx->GINTMSK |= USB_OTG_GINTMSK_NPTXFEM;
	        }
	        break;
	        /* Periodic transfer */
	      case USBD_EP_TYPE_INTR:
	      case USBD_EP_TYPE_ISOC:
	        ////len_words = (hc->xfer_len + 3) / 4;
	        /* check if there is enough space in FIFO space */
	        ////* if (len_words > (USBx_HOST->HPTXSTS & 0xFFFF)) /* split the transfer */
	        {
	          /* need to process data in ptxfempty interrupt */
	        	////* USBx->GINTMSK |= USB_OTG_GINTMSK_PTXFEM;
	        }
	        break;

	      default:
	        break;
	      }

	      if (hc->ep_num == 0)
	      {

	    		USB_Setup_TypeDef * const pSetup = (USB_Setup_TypeDef *) hc->xfer_buff;
	    		ASSERT(hc->xfer_len >= 8);
	    		ASSERT((USBx->DCPCTR & USB_DCPCTR_SUREQ) == 0);

				PRINTF("USB_HC_StartXfer: DCPMAXP=%08lX, dev_addr=%d, bmRequestType=%02X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n",
						USBx->DCPMAXP,
						(int) hc->dev_addr,
						pSetup->b.bmRequestType, pSetup->b.bRequest, pSetup->b.wValue.w, pSetup->b.wIndex.w, pSetup->b.wLength.w);

				USBx->USBREQ =
						(pSetup->b.bRequest << USB_USBREQ_BREQUEST_SHIFT) |
						(pSetup->b.bmRequestType << USB_USBREQ_BMREQUESTTYPE_SHIFT);
				USBx->USBVAL =
						(pSetup->b.wValue.w << USB_USBVAL_SHIFT);
				USBx->USBINDX =
						(pSetup->b.wIndex.w << USB_USBINDX_SHIFT);
				USBx->USBLENG =
						(pSetup->b.wLength.w << USB_USBLENG_SHIFT);

				USBx->DCPMAXP = (USBx->DCPMAXP & ~ (USB_DCPMAXP_DEVSEL)) |
						(0x00 << USB_DCPMAXP_DEVSEL_SHIFT) |	// DEVADD0 used
						0;

				USBx->DCPCTR |= USB_DCPCTR_SQCLR;	// DATA0 as answer
				//USBx->DCPCTR |= USB_DCPCTR_SQSET;	// DATA1 as answer

				USBx->DCPCFG &= ~ USB_DCPCFG_DIR;
				//USBx->DCPCFG |= USB_DCPCFG_DIR;

				//USBx->DCPCTR |= USB_DCPCTR_SUREQCLR;
				USBx->DCPCTR |= USB_DCPCTR_SUREQ;
	      }
	      else
	      {
			  /* Write packet into the Tx FIFO. */
			  //USB_WritePacket(USBx, hc->xfer_buff, hc->ch_num, hc->xfer_len, 0);
			  TP();
			  USB_WritePacketNec(USBx, hc->ch_num, hc->xfer_buff, hc->xfer_len);
	      }
	    }
	  }


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
	PRINTF("USB_HC_Halt, hc_num=%d\n", (int) hc_num);
	//const uint_fast8_t pipe = usbd_getpipe(ep);
	const uint_fast8_t pipe = 0;
	volatile uint16_t * const PIPEnCTR = get_pipectr_reg(USBx, pipe);

	* PIPEnCTR = 0x0000;	// NAK
	while ((* PIPEnCTR & (USB_PIPEnCTR_1_5_PBUSY | USB_PIPEnCTR_1_5_CSSTS)) != 0)	// PBUSY, CSSTS
		;

	USBx->NRDYENB &= ~ (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера
	USBx->BRDYENB &= ~ (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера
	USBx->BEMPENB &= ~ (1uL << pipe);	// Прерывание окончания передачи передающего (IN) буфера
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
HAL_StatusTypeDef USB_HC_Init(
	USB_OTG_GlobalTypeDef *USBx,
	uint8_t ch_num,
	uint8_t epnum,
	uint8_t dev_address,
	uint8_t usb_otg_speed,
	uint8_t ep_type,
	uint16_t mps
	)
{
	PRINTF("USB_HC_Init, ch_num=%d, epnum=%d, ep_type=%d, mps=%lu\n", (int) ch_num, (int) epnum, (int) ep_type, (unsigned long) mps);

	const uint_fast8_t pipe = 0;
	volatile uint16_t * const PIPEnCTR = get_pipectr_reg(USBx, pipe);

	/* Clear old interrupt conditions for this host channel. */
	////USBx_HC(ch_num)->HCINT = 0xFFFFFFFF;

//	USBx->INTSTS1 = (uint16_t) ~ USB_INTSTS1_SIGN;
//	USBx->INTSTS1 = (uint16_t) ~ USB_INTSTS1_SACK;
	USBx->NRDYSTS = (uint16_t) ~ (1uL << pipe);
	USBx->BEMPSTS = (uint16_t) ~ (1uL << pipe);
	USBx->BRDYSTS = (uint16_t) ~ (1uL << pipe);

	/* Enable channel interrupts required for this transfer. */
	switch (ep_type)
	{
	case USBD_EP_TYPE_CTRL:
	case USBD_EP_TYPE_BULK:

/*
	    USBx_HC(ch_num)->HCINTMSK = USB_OTG_HCINTMSK_XFRCM  |
	                                USB_OTG_HCINTMSK_STALLM |
	                                USB_OTG_HCINTMSK_TXERRM |
	                                USB_OTG_HCINTMSK_DTERRM |
	                                USB_OTG_HCINTMSK_AHBERR |
	                                USB_OTG_HCINTMSK_NAKM |
									0;
*/
		if (ep_type != USBD_EP_TYPE_CTRL)
			USBx->NRDYENB |= (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера
		USBx->BRDYENB |= (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера
		USBx->BEMPENB |= (1uL << pipe);	// Прерывание окончания передачи передающего (IN) буфера

	    if (epnum & 0x80)
	    {
			// IN (to host) direction
			//USBx_HC(ch_num)->HCINTMSK |= USB_OTG_HCINTMSK_BBERRM;
	    }
	    else
	    {
	    	// OUT (to device) direction
	      ////if (USBx != USB_OTG_FS)
	      {
			  // HS
	        ////USBx_HC(ch_num)->HCINTMSK |= (USB_OTG_HCINTMSK_NYET | USB_OTG_HCINTMSK_ACKM);
	      }
	    }
	    break;

	  case USBD_EP_TYPE_INTR:

/*
	    USBx_HC(ch_num)->HCINTMSK = USB_OTG_HCINTMSK_XFRCM  |
	                                USB_OTG_HCINTMSK_STALLM |
	                                USB_OTG_HCINTMSK_TXERRM |
	                                USB_OTG_HCINTMSK_DTERRM |
	                                USB_OTG_HCINTMSK_NAKM   |
	                                USB_OTG_HCINTMSK_AHBERR |
	                                USB_OTG_HCINTMSK_FRMORM ;
*/

	    if (epnum & 0x80)
	    {
			// IN (to host) direction
	      //USBx_HC(ch_num)->HCINTMSK |= USB_OTG_HCINTMSK_BBERRM;
	    }

	    break;

	  case USBD_EP_TYPE_ISOC:

/*
	    USBx_HC(ch_num)->HCINTMSK = USB_OTG_HCINTMSK_XFRCM  |
	                                USB_OTG_HCINTMSK_ACKM   |
	                                USB_OTG_HCINTMSK_AHBERR |
	                                USB_OTG_HCINTMSK_FRMORM ;
*/

	    if (epnum & 0x80)
	    {
	    	// IN (to host) direction

	      //USBx_HC(ch_num)->HCINTMSK |= (USB_OTG_HCINTMSK_TXERRM | USB_OTG_HCINTMSK_BBERRM);
	    }
	    break;
	  }

	  /* Enable the top level host channel interrupt. */
	  ////USBx_HOST->HAINTMSK |= (1 << ch_num);

	  /* Make sure host channel interrupts are enabled. */
	  ////USBx->GINTMSK |= USB_OTG_GINTMSK_HCIM;

	  /* Program the HCCHAR register */
/*
	  USBx_HC(ch_num)->HCCHAR = (((dev_address << 22) & USB_OTG_HCCHAR_DAD)  |
	                             (((epnum & 0x7F)<< 11) & USB_OTG_HCCHAR_EPNUM)|
	                             ((((epnum & 0x80) == 0x80)<< 15) & USB_OTG_HCCHAR_EPDIR)|
	                             (((speed == HPRT0_PRTSPD_LOW_SPEED)<< 17) & USB_OTG_HCCHAR_LSDEV)|
	                             ((ep_type << 18) & USB_OTG_HCCHAR_EPTYP)|
	                             (mps & USB_OTG_HCCHAR_MPSIZ));
*/

	  if (ep_type == USBD_EP_TYPE_INTR)
	  {
	    //USBx_HC(ch_num)->HCCHAR |= USB_OTG_HCCHAR_ODDFRM ;
	  }


	* PIPEnCTR = 0x0000;	// NAK
	while ((* PIPEnCTR & (USB_PIPEnCTR_1_5_PBUSY | USB_PIPEnCTR_1_5_CSSTS)) != 0)	// PBUSY, CSSTS
		;

	* PIPEnCTR = USB_PIPEnCTR_1_5_SQCLR;

	* PIPEnCTR = 0x0003;	// NAK->STALL
	* PIPEnCTR = 0x0002;	// NAK->STALL
	* PIPEnCTR = 0x0001;	// STALL->BUF

	return HAL_OK;
}

/**
* @brief  USB_OTG_ResetPort : Reset Host Port
  * @param  USBx : Selected device
  * @param  status : activate reset
  * @retval HAL status
  * @note : (1)The application must wait at least 10 ms
  *   before clearing the reset bit.
  */
// вызывается только для HOST
HAL_StatusTypeDef USB_ResetPort(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t status)
{
	// status 0: reset off, 1: reset on
	if (status)
	{
		USBx->DVSTCTR0 &= ~ USB_DVSTCTR0_UACT;
		(void) USBx->DVSTCTR0;

		USBx->DCPCTR |= USB_DCPCTR_SUREQCLR;

		USBx->DVSTCTR0 |= USB_DVSTCTR0_USBRST;
		(void) USBx->DVSTCTR0;
	}
	else
	{
		USBx->DVSTCTR0 &= ~ USB_DVSTCTR0_USBRST;
		(void) USBx->DVSTCTR0;

//		USBx->DVSTCTR0 |= USB_DVSTCTR0_UACT;
//		(void) USBx->DVSTCTR0;
	}

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
	uint_fast8_t i;

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
			(cfg->pcd_speed == PCD_SPEED_HIGH) * USB_SYSCFG_HSE |	// HSE
			//(1) * USB_SYSCFG_HSE |	// HSE
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
		0 * USB_INTENB0_CTRE |	// CTRE - not enable for host
		0 * USB_INTENB0_DVSE |	// DVSE - not enable for host
		0 * USB_INTENB0_RSME |	// RSME - not enable for host
		0;

	USBx->INTENB1 =
		//1 * USB_INTENB1_BCHGE |	// BCHG
		1 * USB_INTENB1_DTCHE |		// DTCH
		1 * USB_INTENB1_ATTCHE |	// ATTCH
		1 * USB_INTENB1_SIGNE |		// SIGN
		1 * USB_INTENB1_SACKE |		// SACK
		0;

	//	Note 1. When the host controller mode is selected, the bits in this register should be set before starting communication using each pipe.
	//	(1) When the host controller mode is selected, this module refers to the settings of the UPPHUB bits and HUBPORT bits to
	//	generate packets for split transactions.
	//	(2) When the host controller mode is selected, this module refers to the setting of the USBSPD bits to generate packets.
	//	Note 2. The bits in this register should be modified while no valid pipes are using the settings of this register. Valid pipes refer to the ones
	//	satisfying both of conditions (1) and (2) below.
	//	(1) This register is selected by the DEVSEL bits as the communication target.
	//	(2) The PID bits are set to BUF for the pertinent pipe or the pertinent pipe is the DCP with SUREQ being 1.

	for (i = 0; i < USB20_DEVADD0_COUNT; ++ i)
	{
		volatile uint16_t * const DEVADDn = (& USBx->DEVADD0) + i;

		* DEVADDn = (* DEVADDn & ~ (USB_DEVADDn_USBSPD | USB_DEVADDn_HUBPORT | USB_DEVADDn_UPPHUB)) |
			(((cfg->pcd_speed == PCD_SPEED_HIGH) ? 0x03 : 0x02) << USB_DEVADDn_USBSPD_SHIFT) |
			(0x00 << USB_DEVADDn_HUBPORT_SHIFT) |
			(0x00 << USB_DEVADDn_UPPHUB_SHIFT) |
			0;
	}
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
	PRINTF("USB_DoPing, ch_num=%d\n", (int) ch_num);
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
	//PRINTF(PSTR("USB_EPSetStall\n"));
	set_pid(USBx, usbd_getpipe(ep), DEVDRV_USBF_PID_STALL);
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
	// For bulk and interrupt endpoints also set even frame number
    volatile uint16_t * p_reg;
	const uint_fast8_t pipe = usbd_getpipe(ep);
	//PRINTF(PSTR("USB_EPClearStall\n"));
	set_pid(USBx, pipe, DEVDRV_USBF_PID_NAK);

    p_reg = get_pipectr_reg(USBx, pipe);
    /* Set toggle bit to DATA0 */
    * p_reg |= USB_PIPEnCTR_1_5_SQCLR;
    /* Buffer Clear */
    * p_reg |= USB_PIPEnCTR_1_5_ACLRM;
    * p_reg &= ~USB_PIPEnCTR_1_5_ACLRM;

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

#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1


/* Exported macros -----------------------------------------------------------*/
/** @defgroup PCD_Exported_Macros PCD Exported Macros
 *  @brief macros to handle interrupts and specific clock configurations
 * @{
 */

#define __HAL_PCD_GET_FLAG(h, imask)      ((USB_ReadInterrupts((h)->Instance) & (imask)) == (imask))

#define __HAL_PCD_CLEAR_FLAG(h, imask)    do { \
		(h)->Instance->GINTSTS = (imask); \
		(void) (h)->Instance->GINTSTS; \
	} while (0)

#define __HAL_PCD_IS_INVALID_INTERRUPT(h)         (USB_ReadInterrupts((h)->Instance) == 0)


#define __HAL_PCD_UNGATE_PHYCLOCK(h) do { \
		* (__IO uint32_t *) ((uintptr_t) (h)->Instance + USB_OTG_PCGCCTL_BASE) &= ~ (USB_OTG_PCGCCTL_STOPCLK); \
		(void) * (__IO uint32_t *) ((uintptr_t) (h)->Instance + USB_OTG_PCGCCTL_BASE); \
	} while (0)

#define __HAL_PCD_GATE_PHYCLOCK(h) do { \
		*(__IO uint32_t *)((uint32_t)((h)->Instance) + USB_OTG_PCGCCTL_BASE) |= USB_OTG_PCGCCTL_STOPCLK; \
		(void) * (__IO uint32_t *) ((uintptr_t) (h)->Instance + USB_OTG_PCGCCTL_BASE); \
	} while (0)

#define __HAL_PCD_IS_PHY_SUSPENDED(h)            ((* (__IO uint32_t *)((uint32_t)((h)->Instance) + USB_OTG_PCGCCTL_BASE)) & 0x10)

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
HAL_StatusTypeDef USB_CoreInit(USB_OTG_GlobalTypeDef * USBx, const USB_OTG_CfgTypeDef *cfg)
{
	// P1 clock (66.7 MHz max) period = 15 ns
	// The cycle period required to consecutively access registers of this controller must be at least 67 ns.
	// TODO: compute BWAIT value on-the-fly
	// Use P1CLOCK_FREQ
	const uint_fast32_t bwait = ulmin32(ulmax32(calcdivround2(P1CLOCK_FREQ, 15000000uL), 2) - 2, 63);
	USBx->BUSWAIT = (bwait << USB_BUSWAIT_BWAIT_SHIFT) & USB_BUSWAIT_BWAIT;	// 5 cycles = 75 nS minimum
	(void) USBx->BUSWAIT;

	USBx->SUSPMODE &= ~ USB_SUSPMODE_SUSPM;	// SUSPM 0: The clock supplied to this module is stopped.
	(void) USBx->SUSPMODE;

	// This setting shared for USB200 and USB201
	SYSCFG0_0 = (SYSCFG0_0 & ~ (USB_SYSCFG_UPLLE | USB_SYSCFG_UCKSEL)) |
		1 * USB_SYSCFG_UPLLE |	// UPLLE 1: Enables operation of the internal PLL.
		1 * USB_SYSCFG_UCKSEL |	// UCKSEL 1: The 12-MHz EXTAL clock is selected.
		0;
	(void) SYSCFG0_0;
	HARDWARE_DELAY_MS(2);	// required 1 ms delay - see R01UH0437EJ0200 Rev.2.00 28.4.1 System Control and Oscillation Control

	USBx->SUSPMODE |= USB_SUSPMODE_SUSPM;	// SUSPM 1: The clock supplied to this module is enabled.
	(void) USBx->SUSPMODE;

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
	uint_fast8_t i;

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

	//PRINTF("USB_DevInit: cfg->speed=%d\n", (int) cfg->speed);
	USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_HSE)) |
			(cfg->pcd_speed == PCD_SPEED_HIGH) * USB_SYSCFG_HSE |	// HSE
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

	// When the function controller mode is selected, set all the bits in this register to 0.
	for (i = 0; i < USB20_DEVADD0_COUNT; ++ i)
	{
		volatile uint16_t * const DEVADDn = (& USBx->DEVADD0) + i;

		* DEVADDn = 0;
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
HAL_StatusTypeDef USB_FlushTxFifoEx(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t num)
{
	return HAL_OK;
}


static uint_fast8_t
USB_Is_OTG_HS(USB_OTG_GlobalTypeDef *USBx)
{
	return 1;
}

#endif /* CPUSTYLE_R7S721 */


#if (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)



#define USB_OTG_CORE_ID_300A          0x4F54300AU
#define USB_OTG_CORE_ID_310A          0x4F54310AU
#define USB_OTG_CORE_ID_320A          0x4F54320AU

static uint_fast32_t
USB_GetSNPSiD(USB_OTG_GlobalTypeDef *USBx)
{
#if CPUSTYLE_STM32H7XX
	return USBx->GSNPSID;
#elif CPUSTYLE_STM32MP1
	return USB_OTG_CORE_ID_320A;
#else
	return * (__IO uint32_t *) (& USBx->CID + 0x1U);
#endif
}


static uint_fast8_t
USB_Is_OTG_HS(USB_OTG_GlobalTypeDef *USBx)
{
#if CPUSTYLE_STM32MP1
	return 1;
#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX
	return (USBx->CID & (0x1U << 8)) != 0U;
#else
		return 0;
#endif
}

#if defined(USBPHYC)
// STM32MP1xx

#define ULL(v) ((unsigned long long) (v))
#define UL(v) ((unsigned long) (v))
#define U(v) ((unsigned) (v))

#define BIT_32(nr)			(U(1) << (nr))
#define BIT_64(nr)			(ULL(1) << (nr))

/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h. For example
 * GENMASK_64(39, 21) gives us the 64bit vector 0x000000ffffe00000.
 */
#if defined(__LINKER__) || defined(__ASSEMBLER__)
#define GENMASK_32(h, l) \
	(((0xFFFFFFFF) << (l)) & (0xFFFFFFFF >> (32 - 1 - (h))))

#define GENMASK_64(h, l) \
	((~0 << (l)) & (~0 >> (64 - 1 - (h))))
#else
#define GENMASK_32(h, l) \
	(((~UINT32_C(0)) << (l)) & (~UINT32_C(0) >> (32 - 1 - (h))))

#define GENMASK_64(h, l) \
	(((~UINT64_C(0)) << (l)) & (~UINT64_C(0) >> (64 - 1 - (h))))
#endif

#ifdef __aarch64__
#define GENMASK				GENMASK_64
#else
#define GENMASK				GENMASK_32
#endif

#ifdef __aarch64__
#define BIT				BIT_64
#else
#define BIT				BIT_32
#endif

/* STM32_USBPHYC_PLL bit fields */
#define USBPHYC_PLL_PLLNDIV_Msk		GENMASK(6, 0)
#define USBPHYC_PLL_PLLNDIV_Pos		0
#define USBPHYC_PLL_PLLODF_Msk		GENMASK(9, 7)
#define USBPHYC_PLL_PLLODF_Pos		7
#define USBPHYC_PLL_PLLFRACIN_Msk	GENMASK(25, 10)
#define USBPHYC_PLL_PLLFRACIN_Pos	10
#define USBPHYC_PLL_PLLEN_Msk			BIT(26)
#define USBPHYC_PLL_PLLSTRB_Msk			BIT(27)
#define USBPHYC_PLL_PLLSTRBYP_Msk		BIT(28)
#define USBPHYC_PLL_PLLFRACCTL_Msk		BIT(29)
#define USBPHYC_PLL_PLLDITHEN0_Msk		BIT(30)	// PLL dither 2 (triangular)
#define USBPHYC_PLL_PLLDITHEN1_Msk		BIT(31)	// PLL dither 1 (rectangular)

/* STM32_USBPHYC_MISC bit fields */
#define USBPHYC_MISC_SWITHOST_Msk		BIT(0)
#define USBPHYC_MISC_SWITHOST_Pos		0


// STM32MP1 UTMI interface
HAL_StatusTypeDef USB_HS_PHYCInit(USB_OTG_GlobalTypeDef *USBx)
{
	//PRINTF("USB_HS_PHYCInit start\n");
	// Clock source
	RCC->MP_APB4ENSETR = RCC_MC_APB4ENSETR_USBPHYEN;
	(void)RCC-> MP_APB4ENSETR;
	RCC->MP_APB4LPENSETR = RCC_MC_APB4LPENSETR_USBPHYLPEN;
	(void) RCC->MP_APB4LPENSETR;

	if (1)
	{
		// USBOSRC
		//	0: pll4_r_ck clock selected as kernel peripheral clock (default after reset)
		//	1: clock provided by the USB PHY (rcc_ck_usbo_48m) selected as kernel peripheral clock
		// USBPHYSRC
		//  0x0: hse_ker_ck clock selected as kernel peripheral clock (default after reset)
		//  0x1: pll4_r_ck clock selected as kernel peripheral clock
		//  0x2: hse_ker_ck/2 clock selected as kernel peripheral clock
		RCC->USBCKSELR = (RCC->USBCKSELR & ~ (RCC_USBCKSELR_USBOSRC_Msk | RCC_USBCKSELR_USBPHYSRC_Msk)) |
			(0x01 << RCC_USBCKSELR_USBOSRC_Pos) |	// 50 MHz max rcc_ck_usbo_48m
			(0x01 << RCC_USBCKSELR_USBPHYSRC_Pos) |	// 38.4 MHz max pll4_r_ck
			0;
		(void) RCC->USBCKSELR;

	}

	if (1)
	{
		// https://github.com/Xilinx/u-boot-xlnx/blob/master/drivers/phy/phy-stm32-usbphyc.c

		// PLL
		//PRINTF("USB_HS_PHYCInit: stop PLL.\n");
		USBPHYC->PLL &= ~ USBPHYC_PLL_PLLEN_Msk;
		(void) USBPHYC->PLL;

		while ((USBPHYC->PLL & USBPHYC_PLL_PLLEN_Msk) != 0)
			;
		//PRINTF("USB_HS_PHYCInit: stop PLL done.\n");

		const uint_fast32_t USBPHYCPLLFREQUENCY = 1440000000uL;
		const uint_fast32_t pll4_r_ck = PLL4_FREQ_R;
		const uint_fast32_t ODF = 0;	// игнорируется
		// 1440 MHz
		const ldiv_t d = ldiv(USBPHYCPLLFREQUENCY, pll4_r_ck);
		const uint_fast32_t N = d.quot;

		const uint_fast32_t FRACTMAX = (USBPHYC_PLL_PLLFRACIN_Msk >> USBPHYC_PLL_PLLFRACIN_Pos) + 1;
		const uint_fast32_t FRACT = d.rem * (uint_fast64_t) FRACTMAX / pll4_r_ck;

//		uint_fast64_t FRACT = (uint_fast64_t) USBPHYCPLLFREQUENCY << 16;
//		FRACT /= pll4_r_ck;
//		FRACT = FRACT - (d.quot << 16);

		//PRINTF("USB_HS_PHYCInit: pll4_r_ck=%u, N=%u, FRACT=%u, ODF=%u\n", pll4_r_ck, N, (unsigned) (FRACT & 0xFFFF), ODF);

		USBPHYC->PLL =
				(USBPHYC->PLL & ~ (USBPHYC_PLL_PLLDITHEN0_Msk | USBPHYC_PLL_PLLDITHEN1_Msk |
					USBPHYC_PLL_PLLEN_Msk | USBPHYC_PLL_PLLNDIV_Msk | USBPHYC_PLL_PLLODF_Msk |
					USBPHYC_PLL_PLLFRACIN_Msk | USBPHYC_PLL_PLLFRACCTL_Msk | USBPHYC_PLL_PLLSTRB_Msk | USBPHYC_PLL_PLLSTRBYP_Msk)) |
			((N) << USBPHYC_PLL_PLLNDIV_Pos) |	// Целая часть делителя
			((ODF) << USBPHYC_PLL_PLLODF_Pos) |	// PLLODF - игнорируется
			USBPHYC_PLL_PLLSTRBYP_Msk |
			(((FRACT) << USBPHYC_PLL_PLLFRACIN_Pos) & USBPHYC_PLL_PLLFRACIN_Msk) |
			((d.rem != 0) * USBPHYC_PLL_PLLFRACCTL_Msk) |
			USBPHYC_PLL_PLLDITHEN0_Msk |
			USBPHYC_PLL_PLLDITHEN1_Msk |
			0;
		(void) USBPHYC->PLL;

		//PRINTF("USB_HS_PHYCInit: start PLL.\n");
		USBPHYC->PLL |= USBPHYC_PLL_PLLEN_Msk;
		(void) USBPHYC->PLL;

		local_delay_ms(10);

		while ((USBPHYC->PLL & USBPHYC_PLL_PLLEN_Msk) == 0)
			;
		//PRINTF("USB_HS_PHYCInit: start PLL done.\n");
	}

	// MISC
	//	0: Select OTG controller for 2nd PHY port
	//	1: Select Host controller for 2nd PHY port
	USBPHYC->MISC = (USBPHYC->MISC & ~ (USBPHYC_MISC_SWITHOST_Msk)) |
		(0x00 << USBPHYC_MISC_SWITHOST_Pos) |	// 0: Select OTG controller for 2nd PHY port
		0;
	(void) USBPHYC->MISC;

	if (0)
	{
//		USBPHYC_PHY1->TUNE = (USBPHYC->TUNE & ~ (xxx | xxxx)) |
//			(0x00 << ssss) |
//			(0x00 << ssss) |
//			(0x00 << ssss) |
//			0;
		USBPHYC_PHY1->TUNE = 0x04070004;
		(void) USBPHYC_PHY1->TUNE;
	}
	else
	{
//		USBPHYC_PHY2->TUNE = (USBPHYC->TUNE & ~ (xxx | xxxx)) |
//			(0x00 << ssss) |
//			(0x00 << ssss) |
//			(0x00 << ssss) |
//			0;
		USBPHYC_PHY2->TUNE = 0x04070004;
		(void) USBPHYC_PHY2->TUNE;
	}

	//PRINTF("USB_HS_PHYCInit done\n");
	return HAL_OK;
}

#elif defined(USB_HS_PHYC)
// STM32F723xx, STM32F730xx, STM32F733xx

/**
  * @brief  Enables control of a High Speed USB PHYВ’s
  *         Init the low level hardware : GPIO, CLOCK, NVIC...
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef USB_HS_PHYCInit(USB_OTG_GlobalTypeDef *USBx)
{
	PRINTF("USB_HS_PHYCInit start\n");
  uint32_t count = 0;

  /* Enable LDO */
  USB_HS_PHYC->USB_HS_PHYC_LDO |= USB_HS_PHYC_LDO_ENABLE;

  /* wait for LDO Ready */
  while ((USB_HS_PHYC->USB_HS_PHYC_LDO & USB_HS_PHYC_LDO_STATUS) == RESET)
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

	PRINTF("USB_HS_PHYCInit done\n");
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
HAL_StatusTypeDef USB_CoreInit(USB_OTG_GlobalTypeDef * USBx, const USB_OTG_CfgTypeDef *cfg)
{
  if (cfg->phy_itface == USB_OTG_ULPI_PHY)
  {
    USBx->GCCFG &= ~ (USB_OTG_GCCFG_PWRDWN);
	(void) USBx->GCCFG;

    /* Init The ULPI Interface */
    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);
	(void) USBx->GUSBCFG;

    /* Select vbus source */
    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
	(void) USBx->GUSBCFG;

    if(cfg->use_external_vbus == USB_ENABLE)
    {
		USBx->GUSBCFG |= USB_OTG_GUSBCFG_ULPIEVBUSD;
		(void) USBx->GUSBCFG;
    }
    /* Reset after a PHY select  */
    USB_CoreReset(USBx);
  }

#if defined(USB_HS_PHYC) || defined (USBPHYC)
 	else if (cfg->phy_itface == USB_OTG_HS_EMBEDDED_PHY)
	{

		//USBx->GUSBCFG &= ~ USB_OTG_GUSBCFG_PHYSEL_Msk;	// 0: USB 2.0 internal UTMI high-speed PHY.

		USBx->GCCFG &= ~(USB_OTG_GCCFG_PWRDWN);
		(void) USBx->GCCFG;

		/* Init The UTMI Interface */
		USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);
		(void) USBx->GUSBCFG;

		/* Select vbus source */
		USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
		(void) USBx->GUSBCFG;

		/* Select UTMI Interace */
		USBx->GUSBCFG &= ~ USB_OTG_GUSBCFG_ULPI_UTMI_SEL;
		(void) USBx->GUSBCFG;

		USBx->GCCFG |= USB_OTG_GCCFG_PHYHSEN;
		(void) USBx->GCCFG;

		/* Enables control of a High Speed USB PHY */
		USB_HS_PHYCInit(USBx);

		if(cfg->use_external_vbus == USB_ENABLE)
		{
			USBx->GUSBCFG |= USB_OTG_GUSBCFG_ULPIEVBUSD;
			(void) USBx->GUSBCFG;
		}
		/* Reset after a PHY select  */
		USB_CoreReset(USBx);

	}
#endif /* defined(USB_HS_PHYC) || defined (USBPHYC) */

  else /* FS interface (embedded Phy) */
  {
    /* Select FS Embedded PHY */
    USBx->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL;
	(void) USBx->GUSBCFG;

    /* Reset after a PHY select and set Host mode */
    USB_CoreReset(USBx);

    /* Deactivate the power down*/
    USBx->GCCFG = USB_OTG_GCCFG_PWRDWN;
	(void) USBx->GCCFG;
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
		#if CPUSTYLE_STM32MP1
			// 0x00, 0x01, 0x03, 0x05, 0x07
			//	0000 Single: Bus transactions use single 32 bit accesses (not recommended)
			//	0001 INCR: Bus transactions use unspecified length accesses (not recommended, uses the
			//				INCR AHB bus command)
			//	0011 INCR4: Bus transactions target 4x 32 bit accesses
			//	0101 INCR8: Bus transactions target 8x 32 bit accesses
			//	0111 INCR16: Bus transactions based on 16x 32 bit accesses
			//	Others: Reserved
			(0x07uL << USB_OTG_GAHBCFG_HBSTLEN_Pos) |
		#elif CPUSTYLE_STM32H7XX
			(USB_OTG_GAHBCFG_HBSTLEN_1 | USB_OTG_GAHBCFG_HBSTLEN_2) | // (0x06 << USB_OTG_GAHBCFG_HBSTLEN_Pos)
		#else /* CPUSTYLE_STM32H7XX */
			(5 << USB_OTG_GAHBCFG_HBSTLEN_Pos) |	// INCR8
			//(7 << USB_OTG_GAHBCFG_HBSTLEN_Pos) |	// INCR16
		#endif /* CPUSTYLE_STM32H7XX */
			USB_OTG_GAHBCFG_DMAEN |
			0;
		(void) USBx->GAHBCFG;
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
	(void) USBx->GAHBCFG;

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
	(void) USBx->GAHBCFG;

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
	(void) USBx->GUSBCFG;

	if (mode == USB_OTG_HOST_MODE)
	{
		USBx->GUSBCFG |= USB_OTG_GUSBCFG_FHMOD;
		(void) USBx->GUSBCFG;
	}
	else if (mode == USB_OTG_DEVICE_MODE)
	{
		USBx->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
		(void) USBx->GUSBCFG;
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
	#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx)


	if (cfg->vbus_sensing_enable == USB_DISABLE)
	{
		/* Deactivate VBUS Sensing B */
		USBx->GCCFG &= ~ USB_OTG_GCCFG_VBDEN;
		(void) USBx->GCCFG;

		/* B-peripheral session valid override enable*/
		USBx->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
		(void) USBx->GOTGCTL;
		USBx->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;
		(void) USBx->GOTGCTL;
	}
	else
	{
		USBx->GCCFG |= USB_OTG_GCCFG_VBDEN;
		(void) USBx->GCCFG;
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
		if(cfg->pcd_speed == PCD_SPEED_HIGH)
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
		if(cfg->pcd_speed == PCD_SPEED_HIGH)
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
      (void) USBx_INEP(i)->DIEPCTL;
   }
    else
    {
      USBx_INEP(i)->DIEPCTL = 0;
      (void) USBx_INEP(i)->DIEPCTL;
    }

    USBx_INEP(i)->DIEPTSIZ = 0;
    (void) USBx_INEP(i)->DIEPTSIZ;
    USBx_INEP(i)->DIEPINT  = 0xFF;
    (void) USBx_INEP(i)->DIEPINT;
  }

  for (i = 0; i < cfg->dev_endpoints; i++)
  {
    if ((USBx_OUTEP(i)->DOEPCTL & USB_OTG_DOEPCTL_EPENA) == USB_OTG_DOEPCTL_EPENA)
    {
      USBx_OUTEP(i)->DOEPCTL = (USB_OTG_DOEPCTL_EPDIS | USB_OTG_DOEPCTL_SNAK);
      (void) USBx_OUTEP(i)->DOEPCTL;
    }
    else
    {
      USBx_OUTEP(i)->DOEPCTL = 0;
      (void) USBx_OUTEP(i)->DOEPCTL;
    }

    USBx_OUTEP(i)->DOEPTSIZ = 0;
    (void) USBx_OUTEP(i)->DOEPTSIZ;
    USBx_OUTEP(i)->DOEPINT  = 0xFF;
    (void) USBx_OUTEP(i)->DOEPINT;
  }

  USBx_DEVICE->DIEPMSK &= ~(USB_OTG_DIEPMSK_TXFURM);
  (void) USBx_DEVICE->DIEPMSK;

  // xyz
  if (cfg->dma_enable == USB_ENABLE)
  {
	  // See also USBx->GAHBCFG

#if 0//CPUSTYLE_STM32H7XX
    /*Set threshold parameters */
    USBx_DEVICE->DTHRCTL = (USB_OTG_DTHRCTL_TXTHRLEN_8 | USB_OTG_DTHRCTL_RXTHRLEN_8);
    (void) USBx_DEVICE->DTHRCTL;

    USBx_DEVICE->DTHRCTL |= (USB_OTG_DTHRCTL_RXTHREN | USB_OTG_DTHRCTL_ISOTHREN |
		USB_OTG_DTHRCTL_NONISOTHREN | 0x08000000);
    (void) USBx_DEVICE->DTHRCTL;

#else /* CPUSTYLE_STM32H7XX */

	#if CPUSTYLE_STM32MP1
		  const uint_fast32_t TXTHRLEN = 1;		// in DWORDS: The threshold length has to be at least eight DWORDS.
		  const uint_fast32_t RXTHRLEN = 1;	// in DWORDS: 128 - енумерация проходтит, 256 - нет.
	#elif CPUSTYLE_STM32H7XX
		  const uint_fast32_t TXTHRLEN = 256;		// in DWORDS: The threshold length has to be at least eight DWORDS.
		  const uint_fast32_t RXTHRLEN = 8;	// in DWORDS: 128 - енумерация проходтит, 256 - нет.
	#else /* CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1 */
		  const uint_fast32_t TXTHRLEN = 64;		// in DWORDS: The threshold length has to be at least eight DWORDS.
		  const uint_fast32_t RXTHRLEN = 2;	// in DWORDS:
	#endif /* CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1 */
	  // Configuration register applies only to USB OTG HS
    /*Set threshold parameters */
    USBx_DEVICE->DTHRCTL = (USBx_DEVICE->DTHRCTL & ~ (USB_OTG_DTHRCTL_TXTHRLEN | USB_OTG_DTHRCTL_RXTHRLEN |
									USB_OTG_DTHRCTL_RXTHREN | USB_OTG_DTHRCTL_ISOTHREN | USB_OTG_DTHRCTL_NONISOTHREN | USB_OTG_DTHRCTL_ARPEN)) |
		TXTHRLEN * USB_OTG_DTHRCTL_TXTHRLEN_0 |		// Transmit (IN) threshold length
		RXTHRLEN * USB_OTG_DTHRCTL_RXTHRLEN_0 | // see HBSTLEN bit in OTG_GAHBCFG).
		//USB_OTG_DTHRCTL_RXTHREN |		//  Receive (OUT) threshold enable
		//USB_OTG_DTHRCTL_ISOTHREN |		// ISO IN endpoint threshold enable
		//USB_OTG_DTHRCTL_NONISOTHREN |	// Nonisochronous IN endpoints threshold enable
		USB_OTG_DTHRCTL_ARPEN |			// Arbiter parking enable controls internal DMA arbiter parking for IN endpoints.
		0;
    (void) USBx_DEVICE->DTHRCTL;

 #endif /* CPUSTYLE_STM32H7XX */

    (void) USBx_DEVICE->DTHRCTL;
 }

	/* Disable all interrupts. */
	USBx->GINTMSK = 0;
	(void) USBx->GINTMSK;

	/* Clear any pending interrupts */
	USBx->GINTSTS = 0xBFFFFFFF;
	(void) USBx->GINTSTS;

	/* Enable the common interrupts */
	if (cfg->dma_enable == USB_DISABLE)
	{
		USBx->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM;
		(void) USBx->GINTMSK;
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
	(void) USBx->GINTMSK;

	if (cfg->Sof_enable)
	{
		USBx->GINTMSK |= USB_OTG_GINTMSK_SOFM;
		(void) USBx->GINTMSK;
	}

	if (cfg->vbus_sensing_enable == USB_ENABLE)
	{
		USBx->GINTMSK |= (USB_OTG_GINTMSK_SRQIM | USB_OTG_GINTMSK_OTGINT);
		(void) USBx->GINTMSK;
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
HAL_StatusTypeDef USB_FlushTxFifoEx(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t num)
{
	uint32_t count = 0;

	USBx->GRSTCTL = (USBx->GRSTCTL & ~ (USB_OTG_GRSTCTL_TXFNUM)) |
		USB_OTG_GRSTCTL_TXFFLSH |
		num * USB_OTG_GRSTCTL_TXFNUM_0 |
		0;
	(void) USBx->GRSTCTL;

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
	(void) USBx->GRSTCTL;

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
	(void) USBx->GRSTCTL;

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
HAL_StatusTypeDef USB_SetDevSpeed(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t usb_otg_speed)
{
	uint_fast32_t speed;
	switch (usb_otg_speed)
	{
	case USB_OTG_SPEED_HIGH:
		speed = 0x00;
		break;
	case USB_OTG_SPEED_FULL:
		speed = 0x03;	// 01: Full speed using HS, 03: Full speed using internal FS PHY
		break;
	case USB_OTG_SPEED_LOW:
		// N/A
		speed = 0x01;
		break;
	case USB_OTG_SPEED_HIGH_IN_FULL:
		speed = 0x01;
		break;
	default:
		ASSERT(0);
		speed = 0x00;
		break;
	}
	USBx_DEVICE->DCFG = (USBx_DEVICE->DCFG & ~ (USB_OTG_DCFG_DSPD_Msk)) |
		(speed << USB_OTG_DCFG_DSPD_Pos) |
		0;
	(void) USBx_DEVICE->DCFG;
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
	uint_fast8_t speed = USB_OTG_SPEED_FULL;

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
		(void) USBx_DEVICE->DAINTMSK;

		if ((inep->DIEPCTL & USB_OTG_DIEPCTL_USBAEP) == 0)
		{
			inep->DIEPCTL = (inep->DIEPCTL & ~ (USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP | USB_OTG_DIEPCTL_TXFNUM)) |
				((ep->maxpacket << USB_OTG_DIEPCTL_MPSIZ_Pos) & USB_OTG_DIEPCTL_MPSIZ ) |
				(ep->type << USB_OTG_DIEPCTL_EPTYP_Pos ) |
				(ep->tx_fifo_num << USB_OTG_DIEPCTL_TXFNUM_Pos ) |	// TX FIFO index
				USB_OTG_DIEPCTL_SD0PID_SEVNFRM |
				USB_OTG_DIEPCTL_USBAEP |
				0;
			(void) inep->DIEPCTL;
		}

	}
	else
	{
		USB_OTG_OUTEndpointTypeDef * const outep = USBx_OUTEP(ep->num);

		USBx_DEVICE->DAINTMSK |= USB_OTG_DAINTMSK_OEPM & ((1 << ep->num) << USB_OTG_DAINTMSK_OEPM_Pos);
		(void) USBx_DEVICE->DAINTMSK;

		if ((outep->DOEPCTL & USB_OTG_DOEPCTL_USBAEP) == 0)
		{
			outep->DOEPCTL = (outep->DOEPCTL & ~ (USB_OTG_DOEPCTL_MPSIZ | USB_OTG_DOEPCTL_EPTYP)) |
				((ep->maxpacket << USB_OTG_DOEPCTL_MPSIZ_Pos) & USB_OTG_DOEPCTL_MPSIZ ) |
				(ep->type << USB_OTG_DOEPCTL_EPTYP_Pos ) |
				USB_OTG_DOEPCTL_SD0PID_SEVNFRM |
				USB_OTG_DOEPCTL_USBAEP |
				0;
			(void) outep->DOEPCTL;
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
			(void) inep->DIEPCTL;
		}

		USBx_DEVICE->DEACHMSK |= USB_OTG_DAINTMSK_IEPM & ((1 << (ep->num)) << USB_OTG_DAINTMSK_IEPM_Pos);
		(void) USBx_DEVICE->DEACHMSK;
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
			(void) outep->DOEPCTL;
		}

		USBx_DEVICE->DEACHMSK |= USB_OTG_DAINTMSK_OEPM & ((1 << (ep->num)) << USB_OTG_DAINTMSK_OEPM_Pos);
		(void) USBx_DEVICE->DEACHMSK;
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
		(void) USBx_DEVICE->DEACHMSK;
		USBx_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_IEPM & ((1 << (ep->num)) << USB_OTG_DAINTMSK_IEPM_Pos));
		(void) USBx_DEVICE->DAINTMSK;
		inep->DIEPCTL &= ~ USB_OTG_DIEPCTL_USBAEP;
		(void) inep->DIEPCTL;
	}
	else
	{
		USB_OTG_OUTEndpointTypeDef * const outep = USBx_OUTEP(ep->num);

		USBx_DEVICE->DEACHMSK &= ~(USB_OTG_DAINTMSK_OEPM & ((1 << (ep->num)) << USB_OTG_DAINTMSK_OEPM_Pos));
		(void) USBx_DEVICE->DEACHMSK;
		USBx_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_OEPM & ((1 << (ep->num)) << USB_OTG_DAINTMSK_OEPM_Pos));
		(void) USBx_DEVICE->DAINTMSK;
		outep->DOEPCTL &= ~USB_OTG_DOEPCTL_USBAEP;
		(void) outep->DOEPCTL;
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
		(void) inep->DIEPCTL;
		USBx_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_IEPM & ((1 << ep->num)) << USB_OTG_DAINTMSK_IEPM_Pos);
		(void) USBx_DEVICE->DAINTMSK;
	}
	else
	{
		USB_OTG_OUTEndpointTypeDef * const outep = USBx_OUTEP(ep->num);

		outep->DOEPCTL &= ~ USB_OTG_DOEPCTL_USBAEP;
		(void) outep->DOEPCTL;
		USBx_DEVICE->DAINTMSK &= ~ (USB_OTG_DAINTMSK_OEPM & ((1 << ep->num) << USB_OTG_DAINTMSK_OEPM_Pos));
		(void) USBx_DEVICE->DAINTMSK;
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

		/* Zero Length Packet? */
		if (ep->xfer_len == 0)
		{
			inep->DIEPTSIZ = (inep->DIEPTSIZ & ~ (USB_OTG_DIEPTSIZ_XFRSIZ | USB_OTG_DIEPTSIZ_PKTCNT | USB_OTG_DIEPTSIZ_MULCNT)) |
				(USB_OTG_DIEPTSIZ_PKTCNT & (1 << USB_OTG_DIEPTSIZ_PKTCNT_Pos)) |
				0;
			(void) inep->DIEPTSIZ;
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
			(void) inep->DIEPTSIZ;
		}
		if (dma == USB_ENABLE)
		{
			inep->DIEPDMA = ep->dma_addr;
			(void) inep->DIEPDMA;
		}
		else
		{
			if (ep->type != USBD_EP_TYPE_ISOC)
			{
				/* Enable the Tx FIFO Empty Interrupt for this EP */
				if (ep->xfer_len > 0)
				{
					USBx_DEVICE->DIEPEMPMSK |= (1uL << ep->num);
					(void) USBx_DEVICE->DIEPEMPMSK;
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
				(void) inep->DIEPCTL;
			}
			else
			{
				if ((USBx_DEVICE->DSTS & (1uL << USB_OTG_DSTS_FNSOF_Pos)) == 0)
				{
					inep->DIEPCTL |= USB_OTG_DIEPCTL_SODDFRM;
					(void) inep->DIEPCTL;
				}
				else
				{
					inep->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
					(void) inep->DIEPCTL;
				}
			}
		}

		/* EP enable, IN data in FIFO */
		inep->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
		(void) inep->DIEPCTL;

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
			(void) outep->DOEPTSIZ;
		}
		else
		{
			const uint32_t pktcnt = (ep->xfer_len + ep->maxpacket - 1) / ep->maxpacket;
			outep->DOEPTSIZ = (outep->DOEPTSIZ & ~ (USB_OTG_DOEPTSIZ_XFRSIZ | USB_OTG_DOEPTSIZ_PKTCNT)) |
				(USB_OTG_DOEPTSIZ_PKTCNT & (pktcnt << USB_OTG_DOEPTSIZ_PKTCNT_Pos)) |
				(USB_OTG_DOEPTSIZ_XFRSIZ & ((ep->maxpacket * pktcnt) << USB_OTG_DOEPTSIZ_XFRSIZ_Pos)) |
				0;
			(void) outep->DOEPTSIZ;
		}

		if (dma == USB_ENABLE)
		{
			outep->DOEPDMA = (uint32_t) ep->xfer_buff;
			(void) outep->DOEPDMA;
		}

		if (ep->type == USBD_EP_TYPE_ISOC)
		{
			if (USB_GetDevSpeed(USBx) == USB_OTG_SPEED_HIGH)	// romanetz
			{
				if (notseq)
					outep->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM;
				else
					outep->DOEPCTL |= USB_OTG_DOEPCTL_SODDFRM;
				(void) outep->DOEPCTL;
			}
			else
			{
				if ((USBx_DEVICE->DSTS & (1uL << USB_OTG_DSTS_FNSOF_Pos)) == 0)
				{
					outep->DOEPCTL |= USB_OTG_DOEPCTL_SODDFRM;
					(void) outep->DOEPCTL;
				}
				else
				{
					outep->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM;
					(void) outep->DOEPCTL;
				}
			}
		}
		/* EP enable */
		outep->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
		(void) outep->DOEPCTL;
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
		(void) inep->DIEPTSIZ;
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
		(void) inep->DIEPTSIZ;

    }

    if (dma == USB_ENABLE)
    {
      inep->DIEPDMA = ep->dma_addr;
      (void) inep->DIEPDMA;
    }
    else
    {
      /* Enable the Tx FIFO Empty Interrupt for this EP */
      if (ep->xfer_len > 0)
      {
        USBx_DEVICE->DIEPEMPMSK |= (0x1uL << ep->num);
        (void) USBx_DEVICE->DIEPEMPMSK;
      }
    }

    /* EP enable, IN data in FIFO */
    inep->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
    (void) inep->DIEPCTL;
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
		(void) outep->DOEPTSIZ;


		if (dma == USB_ENABLE)
		{
			outep->DOEPDMA = (uint32_t) ep->xfer_buff;
			(void) outep->DOEPDMA;
		}

		/* EP enable */
		outep->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
		(void) outep->DOEPCTL;
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

	for ( i = 0; i < count32b; i ++, dest += 4 )
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

	ASSERT(data != NULL || size == 0);

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
	ASSERT(dest != NULL || len == 0);
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
  uint16_t num_packets = 0;
  uint16_t max_hc_pkt_count = 256;
  uint32_t tmpreg = 0;

  if ((USBx != USB_OTG_FS) && (hc->usbh_otg_speed == USB_OTG_SPEED_HIGH))
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
    USBx_HC(hc->ch_num)->HCDMA = (uintptr_t) hc->xfer_buff;
  }

  is_oddframe = (USBx_HOST->HFNUM & 0x01) ? 0 : 1;
  USBx_HC(hc->ch_num)->HCCHAR &= ~ USB_OTG_HCCHAR_ODDFRM;
  USBx_HC(hc->ch_num)->HCCHAR |= (is_oddframe << USB_OTG_HCCHAR_ODDFRM_Pos);

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
	  uint16_t len_words = 0;

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
		if ((outep->DOEPCTL & USB_OTG_DOEPCTL_EPENA) == 0)
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
HAL_StatusTypeDef  USB_DevConnect(USB_OTG_GlobalTypeDef *USBx)
{
	//PRINTF(PSTR("USB_DevConnect (USBx=%p)\n"), USBx);
	USBx_DEVICE->DCTL &= ~ USB_OTG_DCTL_SDIS;
	(void) USBx_DEVICE->DCTL;
	ASSERT((USBx_DEVICE->DCTL & USB_OTG_DCTL_SDIS) == 0);	// Chech USB_OTH clock is active

	HARDWARE_DELAY_MS(3);
	return HAL_OK;
}


/**
  * @brief  USB_DevDisconnect : Disconnect the USB device by disabling the pull-up/pull-down
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_DevDisconnect(USB_OTG_GlobalTypeDef *USBx)
{
	//PRINTF(PSTR("USB_DevDisconnect (USBx=%p)\n"), USBx);
	USBx_DEVICE->DCTL |= USB_OTG_DCTL_SDIS;
	(void) USBx_DEVICE->DCTL;
	ASSERT((USBx_DEVICE->DCTL & USB_OTG_DCTL_SDIS) != 0);	// Chech USB_OTH clock is active

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
  (void) USBx->GINTSTS;
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
	(void) USBx_INEP(0)->DIEPCTL;

	if ((USBx_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD) == DSTS_ENUMSPD_LS_PHY_6MHZ)
	{
		USBx_INEP(0)->DIEPCTL |= (DEP0CTL_MPS_8 << USB_OTG_DIEPCTL_MPSIZ_Pos);	// 8 bytes = code 3
		(void) USBx_INEP(0)->DIEPCTL;
	}
	USBx_DEVICE->DCTL |= USB_OTG_DCTL_CGINAK;
	(void) USBx_DEVICE->DCTL;

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
	(void) USBx_OUTEP(0)->DOEPTSIZ;

	if (dma == USB_ENABLE)
	{
		arm_hardware_flush_invalidate((uintptr_t) psetup, 4 * 12);	// need
		USBx_OUTEP(0)->DOEPDMA = (uint32_t) psetup;
		(void) USBx_OUTEP(0)->DOEPDMA;

		/* EP enable */
		USBx_OUTEP(0)->DOEPCTL =
			USB_OTG_DOEPCTL_EPENA |
			USB_OTG_DOEPCTL_USBAEP |
			0;
		(void) USBx_OUTEP(0)->DOEPCTL;
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
  (void) USBx->GRSTCTL;

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
#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32F7XX || defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || \
    defined(STM32F412Rx) || defined(STM32F412Cx)

	USBx->GCCFG |= USB_OTG_GCCFG_VBDEN;
	(void) USBx->GCCFG;

#else
	USBx->GCCFG &=~ (USB_OTG_GCCFG_VBUSASEN);
	(void) USBx->GCCFG;
	USBx->GCCFG &=~ (USB_OTG_GCCFG_VBUSBSEN);
	(void) USBx->GCCFG;
	USBx->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
	(void) USBx->GCCFG;
#endif /* STM32F446xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Rx || STM32F412Vx || STM32F412Cx */

	/* Disable the FS/LS support mode only */
	if ((cfg->pcd_speed == PCD_SPEED_FULL) && (USBx != USB_OTG_FS))
	{
		USBx_HOST->HCFG |= USB_OTG_HCFG_FSLSS;
		(void) USBx_HOST->HCFG;
	}
	else
	{
		USBx_HOST->HCFG &= ~ (USB_OTG_HCFG_FSLSS);
		(void) USBx_HOST->HCFG;
	}

	/* Make sure the FIFOs are flushed. */
	USB_FlushTxFifoAll(USBx); /* all Tx FIFOs */
	USB_FlushRxFifo(USBx);

	/* Clear all pending HC Interrupts */
	for (i = 0; i < cfg->Host_channels; i++)
	{
		USBx_HC(i)->HCINT = 0xFFFFFFFF;
		(void) USBx_HC(i)->HCINT;
		USBx_HC(i)->HCINTMSK = 0;
		(void) USBx_HC(i)->HCINTMSK;
	}

	/* Enable VBUS driving */
	USB_DriveVbus(USBx, 1);

	HARDWARE_DELAY_MS(200);

	/* Disable all interrupts. */
	USBx->GINTMSK = 0;
	(void) USBx->GINTMSK;

	/* Clear any pending interrupts */
	USBx->GINTSTS = 0xFFFFFFFF;
	(void) USBx->GINTSTS;

	if (USBx == USB_OTG_FS)
	{
		/* set Rx FIFO size */
		USBx->GRXFSIZ  = (uint32_t )0x80;
		(void) USBx->GRXFSIZ;
		USBx->DIEPTXF0_HNPTXFSIZ = (uint32_t )(((0x60 << 16)& USB_OTG_NPTXFD) | 0x80);
		(void) USBx->DIEPTXF0_HNPTXFSIZ;
		USBx->HPTXFSIZ = (uint32_t )(((0x40 << 16)& USB_OTG_HPTXFSIZ_PTXFD) | 0xE0);
		(void) USBx->HPTXFSIZ;
	}
	else
	{
		/* set Rx FIFO size */
		USBx->GRXFSIZ  = (uint32_t) 0x200;
		(void) USBx->GRXFSIZ;
		USBx->DIEPTXF0_HNPTXFSIZ = (uint32_t) (((0x100 << 16)& USB_OTG_NPTXFD) | 0x200);
		(void) USBx->DIEPTXF0_HNPTXFSIZ;
		USBx->HPTXFSIZ = (uint32_t) (((0xE0 << 16)& USB_OTG_HPTXFSIZ_PTXFD) | 0x300);
		(void) USBx->HPTXFSIZ;
	}

	/* Enable the common interrupts */
	if (cfg->dma_enable == USB_DISABLE)
	{
		USBx->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM;
		(void) USBx->GINTMSK;
	}

	/* Enable interrupts matching to the Host mode ONLY */
	USBx->GINTMSK |= (
		USB_OTG_GINTMSK_PRTIM |
		USB_OTG_GINTMSK_HCIM |
		USB_OTG_GINTMSK_SOFM |
		USB_OTG_GINTSTS_DISCINT |
		USB_OTG_GINTMSK_PXFRM_IISOOXFRM |
		USB_OTG_GINTMSK_WUIM |
		0);
	(void) USBx->GINTMSK;

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
    (void) USBx_HOST->HFIR;
  }
  else if (freq ==  HCFG_6_MHZ)
  {
    USBx_HOST->HFIR = (uint32_t)6000;
    (void) USBx_HOST->HFIR;
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
// вызывается только для HOST
HAL_StatusTypeDef USB_ResetPort(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t state)
{
  uint32_t hprt0 = USBx_HPRT0;

  hprt0 &= ~ (USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET | USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG);

  if (state)
	  USBx_HPRT0 = (USB_OTG_HPRT_PRST | hprt0);
  else
	  USBx_HPRT0 = ((~ USB_OTG_HPRT_PRST) & hprt0);
  (void) USBx_HPRT0;

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
    (void) USBx_HPRT0;
  }
  if (((hprt0 & USB_OTG_HPRT_PPWR) == USB_OTG_HPRT_PPWR) && (state == 0 ))
  {
    USBx_HPRT0 = ((~USB_OTG_HPRT_PPWR) & hprt0);
    (void) USBx_HPRT0;
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
uint32_t USB_GetHostSpeed(USB_OTG_GlobalTypeDef *USBx)
{
	switch ((USBx_HPRT0 & USB_OTG_HPRT_PSPD_Msk) >> USB_OTG_HPRT_PSPD_Pos)
	{
	case 0x00:
		return USB_OTG_SPEED_HIGH;
	case 0x01:
		return USB_OTG_SPEED_FULL;
	case 0x02:
		return USB_OTG_SPEED_LOW;
	default:
	case 0x03:
		// reserved
		return USB_OTG_SPEED_FULL;
	}
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
	//PRINTF("HAL_PCD_Init start\n");
	uint_fast8_t i;

	/* Check the PCD handle allocation */
	if (hpcd == NULL)
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

	/* Init the Core (common init.) */
	USB_CoreInit(hpcd->Instance, & hpcd->Init);

	/* Force Device Mode*/
	USB_SetCurrentMode(hpcd->Instance, USB_OTG_DEVICE_MODE);

	/* Init endpoints structures */
	for (i = 0; i < 15 ; i ++)
	{
		/* Init ep structure */
		hpcd->IN_ep[i].is_in = 1;
		hpcd->IN_ep[i].num = i;
#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1
		hpcd->IN_ep[i].tx_fifo_num = i;
#endif /* CPUSTYLE_STM32F || CPUSTYLE_STM32MP1 */
		/* Control until ep is activated */
		hpcd->IN_ep[i].type = USBD_EP_TYPE_CTRL;
		hpcd->IN_ep[i].maxpacket = 0;
		hpcd->IN_ep[i].xfer_buff = NULL;
		hpcd->IN_ep[i].xfer_len = 0;
	}

	for (i = 0; i < 15 ; i ++)
	{
		hpcd->OUT_ep[i].is_in = 0;
		hpcd->OUT_ep[i].num = i;
#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1
		hpcd->IN_ep[i].tx_fifo_num = i;
#endif /* CPUSTYLE_STM32F || CPUSTYLE_STM32MP1 */
		/* Control until ep is activated */
		hpcd->OUT_ep[i].type = USBD_EP_TYPE_CTRL;
		hpcd->OUT_ep[i].maxpacket = 0;
		hpcd->OUT_ep[i].xfer_buff = NULL;
		hpcd->OUT_ep[i].xfer_len = 0;

		////hpcd->Instance->DIEPTXF[i] = 0;
	}

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
	//PRINTF("HAL_PCD_Init done\n");
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

#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1
	if (ep->is_in)
	{
		/* Assign a Tx FIFO */
		ep->tx_fifo_num = tx_fifo_num;
	}
#endif /* CPUSTYLE_STM32F */

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
	ASSERT(len == 0 || pBuf != NULL);
	USB_OTG_EPTypeDef * const ep = & hpcd->IN_ep [ep_addr & 0x7F];

	//PRINTF("HAL_PCD_EP_Transmit: ep=%p, ep_addr=%X, pBuf=%p, len=%u. maxpacket=%u\n", ep, ep_addr, pBuf, len, ep->maxpacket);
	/*setup and start the Xfer */
	ep->xfer_buff = (uint8_t *) pBuf;
	ep->xfer_len = len;
	ep->xfer_count = 0;
	ep->is_in = 1;
	ep->num = ep_addr & 0x7F;

	if (hpcd->Init.dma_enable == USB_ENABLE)
	{
		if (pBuf != NULL && len != 0)
		{
			ASSERT(((uintptr_t) pBuf % DCACHEROWSIZE) == 0);
			arm_hardware_flush((uintptr_t) pBuf, len);
		}
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
    hpcd->run_later_ctrl_comp = 0;

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
    USB_FlushTxFifoEx(hpcd->Instance, ep_addr & 0x0F);
  }
  else
  {
    USB_FlushRxFifo(hpcd->Instance);
  }

  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

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

	/* Set USB Current Speed */
	switch (hpcd->Init.pcd_speed)
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
	//PRINTF(PSTR("HAL_PCD_ResetCallback: speed=%d\n"), (int) speed);
	// Set speed information for enumeration responce
	USBD_LL_SetSpeed((USBD_HandleTypeDef*)hpcd->pData, speed);	// USBD_SPEED_xxx

	/*Reset Device*/
	USBD_LL_Reset((USBD_HandleTypeDef*)hpcd->pData);
}

#if (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)


/**
  * @brief  Suspend callback.
  * When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
	//PRINTF("HAL_PCD_SuspendCallback\n");
   /* Inform USB library that core enters in suspend Mode */
  USBD_LL_Suspend((USBD_HandleTypeDef*)hpcd->pData);
  __HAL_PCD_GATE_PHYCLOCK(hpcd);
  /*Enter in STOP mode */
  /* USER CODE BEGIN 2 */
#if defined (SCB_SCR_SLEEPDEEP_Msk)
  if (hpcd->Init.low_power_enable)
  {
    /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register */
    SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
  }
#endif /* defined(SCB_SCR_SLEEPDEEP_Msk) */
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
	//PRINTF("HAL_PCD_ResumeCallback\n");
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
	PCD_TypeDef * const USBx = hpcd->Instance;

	PRINTF(PSTR("usbd_fifo_initialize: bigbuff=%d, fullsize=%u, power-on GRXFSIZ=%u\n"), (int) bigbuff, (unsigned) fullsize, USBx->GRXFSIZ & USB_OTG_GRXFSIZ_RXFD);
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

#if WITHUSBUACIN
	{
		/* endpoint передачи звука в компьютер */
		const uint_fast8_t pipe = (USBD_EP_AUDIO_IN) & 0x7F;

		const uint_fast16_t uacinmaxpacket = usbd_getuacinmaxpacket();

		const uint_fast16_t size4 = nuacinpackets * (size2buff4(uacinmaxpacket) + add3tx);
		ASSERT(last4 >= size4);
		last4 -= size4;
		USBx->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
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
		USBx->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
		PRINTF(PSTR("usbd_fifo_initialize3 - UAC3 %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
	}
#endif /* WITHUSBUACIN2 */
#endif /* WITHUSBUACIN */
#if WITHUSBUACOUT
	{
		numoutendpoints += 1;
		maxoutpacketsize4 = ulmax16(maxoutpacketsize4, nuacoutpackets * size2buff4(UACOUT_AUDIO48_DATASIZE));
	}
#endif /* WITHUSBUACOUT */
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
			USBx->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4dummy, size4dummy);
			USBx->DIEPTXF [pipeint - 1] = usbd_makeTXFSIZ(last4dummy, size4dummy);

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
			USBx->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
			USBx->DIEPTXF [pipeint - 1] = usbd_makeTXFSIZ(last4dummy, size4dummy);
			PRINTF(PSTR("usbd_fifo_initialize4 CDC %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
		}
	}

#endif /* WITHUSBCDC */

#if WITHUSBCDCEEM
	{
		/* полнофункциональное устройство */
		const uint_fast8_t pipe = USBD_EP_CDCEEM_IN & 0x7F;

		numoutendpoints += 1;
		const int ncdceemindatapackets = 1 * mul2 + 1, ncdceemoutdatapackets = 3;

		maxoutpacketsize4 = ulmax16(maxoutpacketsize4, ncdceemoutdatapackets * size2buff4(USBD_CDCEEM_BUFSIZE));


		const uint_fast16_t size4 = ncdceemindatapackets * (size2buff4(USBD_CDCEEM_BUFSIZE) + add3tx);
		ASSERT(last4 >= size4);
		last4 -= size4;
		USBx->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
		PRINTF(PSTR("usbd_fifo_initialize5 EEM %u bytes: 4*(full4-last4)=%u\n"), 4 * size4, 4 * (full4 - last4));
	}
#endif /* WITHUSBCDCEEM */

#if WITHUSBHID && 0
	{
		/* ... устройство */
		const uint_fast8_t pipe = USBD_EP_HIDMOUSE_INT & 0x7F;

		const uint_fast16_t size4 = size2buff4(HIDMOUSE_INT_DATA_SIZE);
		ASSERT(last4 >= size4);
		last4 -= size4;
		USBx->DIEPTXF [pipe - 1] = usbd_makeTXFSIZ(last4, size4);
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
		USBx->DIEPTXF0_HNPTXFSIZ = usbd_makeTXFSIZ(last4, size4);
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
		USBx->GRXFSIZ = (USBx->GRXFSIZ & ~ USB_OTG_GRXFSIZ_RXFD) |
			(last4 << USB_OTG_GRXFSIZ_RXFD_Pos) |	// was: size4 - то что осталось
			0;
		base4 += size4;
	}

	if (base4 > last4 || last4 > full4)
	{
		char b [64];
		PRINTF(PSTR("usbd_fifo_initialize error: base4=%u, last4=%u, fullsize=%u\n"), (base4 * 4), (last4 * 4), fullsize);
		local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("used=%u"), (base4 * 4) + (fullsize - last4 * 4));
		colmain_setcolors(COLORMAIN_RED, BGCOLOR);
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
		colmain_setcolors(COLORMAIN_GREEN, BGCOLOR);
		display_at(0, 0, b);
		HARDWARE_DELAY_MS(2000);
#endif
	}

	USB_FlushRxFifo(USBx);
	USB_FlushTxFifoAll(USBx);
}
#endif /* (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX) */

// На RENESAS ничего не делаем - req уже заполнен чтением из контроллера USB
static void USBD_ParseSetupRequest(USBD_SetupReqTypedef *req, const uint32_t * pdata)
{
#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1
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
#endif /* CPUSTYLE_STM32F */
#if 0
	PRINTF(PSTR("USBD_ParseSetupRequest: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"),
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
USBD_StatusTypeDef  USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
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
USBD_StatusTypeDef  USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
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
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t  ep_addr)
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
USBD_StatusTypeDef  USBD_Start(USBD_HandleTypeDef *pdev)
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
		/* For each device function */
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
USBD_StatusTypeDef  USBD_RunTestMode(USBD_HandleTypeDef  *pdev)
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
		/* For each device function */
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
		/* For each device function */
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
	//PRINTF(PSTR("USBD_CtlPrepareRx: len=%d\n"), (int) len);
	/* Set EP0 State */
	pdev->ep0_state = USBD_EP0_DATA_OUT;
	pdev->ep_out[0].total_length = len;
	pdev->ep_out[0].rem_length = len;
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
	//PRINTF(PSTR("USBD_CtlContinueRx: total_length=%d, rem_length=%d\n"), (int) pdev->ep_out[0].total_length, (int) pdev->ep_out[0].rem_length);

	ASSERT(pdev->ep0_state == USBD_EP0_DATA_OUT);
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
	//PRINTF(PSTR("USBD_CtlReceiveStatus: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
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
USBD_StatusTypeDef USBD_CtlSendData(USBD_HandleTypeDef  *pdev,
                              const uint8_t *pbuf,
                               uint16_t len)
{
	//PRINTF("USBD_CtlSendData: pdev=%p, pbuf=%p, len=%d, pep=%p\n", pdev, pbuf, len, & pdev->ep_in [0]);
	ASSERT(len == 0 || pbuf != NULL);
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
USBD_StatusTypeDef USBD_CtlContinueSendData(USBD_HandleTypeDef  *pdev,
                                       const uint8_t *pbuf,
                                       uint16_t len)
{
	//PRINTF("USBD_CtlContinueSendData: pdev=%p, pbuf=%p, len=%d, pep=%p\n", pdev, pbuf, len, & pdev->ep_in [0]);
	ASSERT(len == 0 || pbuf != NULL);
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
	//PRINTF(PSTR("USBD_CtlError: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);

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
	//PRINTF(PSTR("USBD_StdItfReq: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);

	USBD_StatusTypeDef ret = USBD_OK;

	switch (pdev->dev_state)
	{
	case USBD_STATE_CONFIGURED:
		{
			// Extended Properties OS Descriptor support
			// wIndex==0x05. Indicates that the request is for an extended properties OS descriptor.
			if (req->bRequest == USBD_WCID_VENDOR_CODE && req->wIndex == 0x05)
			{
				const uint_fast8_t ifc = LO_BYTE(req->wValue);
				PRINTF(PSTR("MS USBD_StdItfReq: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
				// Extended Properties OS Descriptor
				// See OS_Desc_Ext_Prop.doc, Extended Properties Descriptor Format

				// Extended Properties OS Descriptor support
				if (ExtOsPropDescTbl[ifc].size != 0)
				{
					USBD_CtlSendData(pdev, ExtOsPropDescTbl[ifc].data, ulmin16(ExtOsPropDescTbl[ifc].size, req->wLength));
				}
				else
				{
					TP();
					USBD_CtlError(pdev, req);
					return USBD_OK;
				}
			}
			uint_fast8_t di;	// device function index
			for (di = 0; di < pdev->nClasses; ++ di)
			{
				pdev->pClasses [di]->Setup(pdev, req);
			}
			if (req->wLength == 0)
			{
				//TP();
				//PRINTF(PSTR("USBD_StdItfReq: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
				// Уже какой-то ответ в обработчике сформирован.
				// Если нет - ошибка, там надо вызвать например USBD_CtlSendStatus
				// Этот запрос был без данных
				//USBD_CtlSendStatus(pdev); // по идее, в обработчике Setup должен быть вызван USBD_CtlSendStatus/USBD_CtlError
			}
			else
			{
#if CPUSTYLE_R7S721
				// FIXME:
				// Hack code!!!!
				if ((req->bmRequest & USB_REQ_TYPE_DIR) == 0)
				{
					((PCD_HandleTypeDef *) pdev->pData)->run_later_ctrl_comp = 1;
				}
#endif
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
	//PRINTF(PSTR("USBD_StdEPReq: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
	uint8_t   ep_addr;
	USBD_StatusTypeDef ret = USBD_OK;
	USBD_EndpointTypeDef   *pep;
	ep_addr  = LO_BYTE(req->wIndex);

  /* Check if it is a class request */
  if ((req->bmRequest & 0x60) == 0x20)
  {
		TP();
		uint_fast8_t di;	// device function index
		for (di = 0; di < pdev->nClasses; ++ di)
			pdev->pClasses [di]->Setup(pdev, req);

    return USBD_OK;
  }

  switch (req->bRequest)
  {
  case USB_REQ_SET_FEATURE :
 	//PRINTF(PSTR("USBD_StdEPReq: USB_REQ_SET_FEATURE: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"),
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

      //USBD_CtlSendStatus(pdev);	// по идее, в обработчике Setup должен быть вызван USBD_CtlSendStatus/USBD_CtlError

      break;

    default:
	  TP();
      USBD_CtlError(pdev, req);
      break;
    }
    break;

  case USB_REQ_CLEAR_FEATURE :
 	//PRINTF(PSTR("USBD_StdEPReq: USB_REQ_CLEAR_FEATURE: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"),
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
			// по идее, в обработчике Setup должен быть вызван USBD_CtlSendStatus/USBD_CtlError
        }
        else
        {
        	USBD_CtlSendStatus(pdev);

        }
      }
      break;

    default:
		TP();
      USBD_CtlError(pdev, req);
      break;
    }
    break;

  case USB_REQ_GET_STATUS:
	  /*
	         Например, стандартный запрос GetStatus может быть направлен на устройство,
	  	   интерфейс или конечную точку.
	  	   Когда он направлен на устройство, он возвращает флаги, показывающие статус удаленного пробуждения (remote wakeup),
	  	   и является ли устройство самопитаемым.
	  	   Однако тот же запрос, направленный к интерфейсу,
	  	   всегда вернет 0, или если запрос будет направлен на конечную точку, то он вернет состояние флага halt (флаг останова) для конечной точки.
	  */
 	//PRINTF(PSTR("USBD_StdEPReq: USB_REQ_GET_STATUS: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"),
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
      if (USBD_LL_IsStallEP(pdev, ep_addr))
      {
		pep->epstatus [0] = 0x01;
		pep->epstatus [1] = 0x00;
     }
      else
      {
        pep->epstatus [0] = 0x00;
        pep->epstatus [1] = 0x00;
      }

      USBD_CtlSendData(pdev, pep->epstatus, 2);
      break;

    default:
		TP();
      USBD_CtlError(pdev, req);
      break;
    }
    break;

  default:
	  TP();
	//PRINTF(PSTR("USBD_StdEPReq: ???? bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"),
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
				// WCID devices support
				// Microsoft OS String Descriptor, ReqLength=0x12
				// See OS_Desc_Intro.doc, Table 3 describes the OS string descriptor’s fields.
				if (MsftStringDescr [0].data != NULL && MsftStringDescr [0].size != 0)
				{
					len = MsftStringDescr [0].size;
					pbuf = MsftStringDescr [0].data;
				}
				else
				{
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
		USBD_CtlSendData(pdev, pbuf, ulmin16(len, req->wLength));
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
	//PRINTF(PSTR("USBD_SetConfig: cfgidx=%d, pdev->dev_state=%d\n"), (int) cfgidx, (int) pdev->dev_state);

	switch (pdev->dev_state)
	{
	case USBD_STATE_ADDRESSED:
		if (cfgidx)
		{
			pdev->dev_config [0] = cfgidx;
			pdev->dev_state = USBD_STATE_CONFIGURED;
			if (USBD_SetClassConfig(pdev, cfgidx) == USBD_FAIL)
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
			if (USBD_SetClassConfig(pdev, cfgidx) == USBD_FAIL)
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
	//PRINTF(PSTR("USBD_GetConfig:\n"));
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
			USBD_CtlSendData(pdev, pdev->dev_default_config, 1);
			break;

		case USBD_STATE_CONFIGURED:
			USBD_CtlSendData(pdev, pdev->dev_config, 1);
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
// device status
static void USBD_GetStatus(USBD_HandleTypeDef *pdev,
                           const USBD_SetupReqTypedef *req)
{
	/*
	       Например, стандартный запрос GetStatus может быть направлен на устройство,
		   интерфейс или конечную точку.
		   Когда он направлен на устройство, он возвращает флаги, показывающие статус удаленного пробуждения (remote wakeup),
		   и является ли устройство самопитаемым.
		   Однако тот же запрос, направленный к интерфейсу,
		   всегда вернет 0, или если запрос будет направлен на конечную точку, то он вернет состояние флага halt (флаг останова) для конечной точки.
	*/

	//PRINTF(PSTR("USBD_GetStatus:\n"));
	switch (pdev->dev_state)
	{
	case USBD_STATE_ADDRESSED:
	case USBD_STATE_CONFIGURED:

		#if USBD_SELF_POWERED != 0
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

		USBD_CtlSendData(pdev, pdev->dev_config_status, 2);
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
		//USBD_CtlSendStatus(pdev);	// по идее, в обработчике Setup должен быть вызван USBD_CtlSendStatus/USBD_CtlError
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
			//USBD_CtlSendStatus(pdev);	// по идее, в обработчике Setup должен быть вызван USBD_CtlSendStatus/USBD_CtlError
		}
		else
		{
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
	//PRINTF(PSTR("USBD_StdDevReq: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
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
		USBD_GetStatus (pdev, req);	// device status
		break;

	case USB_REQ_SET_FEATURE:
		USBD_SetFeature (pdev, req);
		break;

	case USB_REQ_CLEAR_FEATURE:
		USBD_ClrFeature (pdev, req);
		break;

#if WITHUSBWCID
	case USBD_WCID_VENDOR_CODE:
		// WCID devices support
		if (MsftCompFeatureDescr[0].size != 0)
		{
			USBD_CtlSendData(pdev, MsftCompFeatureDescr[0].data, ulmin16(MsftCompFeatureDescr[0].size, req->wLength));
		}
		else
		{
			TP();
			USBD_CtlError(pdev, req);
		}
		return USBD_OK;
#endif /* WITHUSBWCID */

	default:
		TP();
		PRINTF(PSTR("USBD_StdDevReq: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
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

	switch (pdev->request.bmRequest & USB_FUNCTION_bmRequestTypeRecip)
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
		USBD_LL_StallEP(pdev, pdev->request.bmRequest & USB_FUNCTION_bmRequestTypeDir);
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
				pep->rem_length -= pep->maxpacket;

				USBD_CtlContinueRx(pdev, pdata, ulmin16(pep->rem_length, pep->maxpacket));
			}
			else
			{
				if ((pdev->dev_state == USBD_STATE_CONFIGURED))
				{
		        	  uint_fast8_t di;
		        	  for (di = 0; di < pdev->nClasses; ++ di)
		        	  {
		        		  /* For each device function */
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
			  /* For each device function */
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
		//PRINTF(PSTR("USBD_LL_DataInStage: EP0: pdata=%p, pdev->ep0_data_len=%d\n"), pdata, (int) pdev->ep0_data_len);

		if (pdev->ep0_state == USBD_EP0_DATA_IN)
		{
			if (pep->rem_length > pep->maxpacket)
			{
				pep->rem_length -= pep->maxpacket;
				USBD_CtlContinueSendData(pdev, pdata, pep->rem_length);

				/* Prepare endpoint for premature end of transfer */
				USBD_LL_PrepareReceive(pdev, 0, NULL, 0);
			}
			else
			{
				/* last packet is MPS multiple, so send ZLP packet */
				if ((pep->total_length % pep->maxpacket == 0) &&
						(pep->total_length >= pep->maxpacket) &&
						 (pep->total_length < pdev->ep0_data_len ))
				{
					USBD_CtlContinueSendData(pdev, NULL, 0);
					pdev->ep0_data_len = 0;

					/* Prepare endpoint for premature end of transfer */
					USBD_LL_PrepareReceive(pdev, 0, NULL, 0);
				}
				else
				{
					if (pdev->dev_state == USBD_STATE_CONFIGURED)
					{
						uint_fast8_t di;
						for (di = 0; di < pdev->nClasses; ++ di)
						{
							/* For each device function */
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
		// For other (non-control) endpoints
		uint_fast8_t di;
		for (di = 0; di < pdev->nClasses; ++ di)
		{
			/* For each device function */
			const USBD_ClassTypeDef * const pClass = pdev->pClasses [di];
			if (pClass->DataIn != NULL)
				pClass->DataIn(pdev, epnum);	// epnum without direction bit
		}
	}
	else
	{
		TP();
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
		/* For each device function */
		const USBD_ClassTypeDef * const pClass = pdev->pClasses [di];
		//if (pdev->pClassData)
		{
			pClass->DeInit(pdev, pdev->dev_config [0]);
			//pdev->pClassData = NULL; // TODO: make own data For each device function. Is not used now
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
	pdev->dev_speed = speed;	// USBD_SPEED_xxx for enumeration responce
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
			/* For each device function */
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
		  /* For each device function */
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

#if (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)
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

/**
  * @brief  Handle PCD interrupt request.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
// F4, F7, H7...
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
{
	__DMB();
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

	    /* Handle RxQLevel Interrupt */
	    if (__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_RXFLVL))
	    {
	      USB_MASK_INTERRUPT(hpcd->Instance, USB_OTG_GINTSTS_RXFLVL); //mgs:????
	      const uint_fast32_t grxstsp = USBx->GRXSTSP;
	      USB_OTG_EPTypeDef * const ep = & hpcd->OUT_ep [(grxstsp & USB_OTG_GRXSTSP_EPNUM_Msk) >> USB_OTG_GRXSTSP_EPNUM_Pos];

	      if (((grxstsp & USB_OTG_GRXSTSP_PKTSTS_Msk) >> USB_OTG_GRXSTSP_PKTSTS_Pos) ==  STS_DATA_UPDT)
	      {
			const unsigned bcnt = (grxstsp & USB_OTG_GRXSTSP_BCNT_Msk) >> USB_OTG_GRXSTSP_BCNT_Pos;
	        if (bcnt != 0)
	        {
//	        	if (ep->xfer_buff == NULL)
//	        	{
//	        		PRINTF("ep=%d\n", (int) ((grxstsp & USB_OTG_GRXSTSP_EPNUM_Msk) >> USB_OTG_GRXSTSP_EPNUM_Pos));
//	        	}
				ASSERT(ep->xfer_buff != NULL);
	          USB_ReadPacket(USBx, ep->xfer_buff, bcnt);
	          ep->xfer_buff += bcnt;
	          ep->xfer_count += bcnt;
	        }
	      }
	      else if (((grxstsp & USB_OTG_GRXSTSP_PKTSTS_Msk) >> USB_OTG_GRXSTSP_PKTSTS_Pos) ==  STS_SETUP_UPDT)
	      {
			const unsigned bcnt = (grxstsp & USB_OTG_GRXSTSP_BCNT) >> USB_OTG_GRXSTSP_BCNT_Pos;
			ASSERT(bcnt == 8);
			ASSERT((grxstsp & USB_OTG_GRXSTSP_EPNUM) == 0);
	        USB_ReadPacket(USBx, (uint8_t *) hpcd->PSetup, 8);
	        ep->xfer_count += bcnt;
	      }
	      USB_UNMASK_INTERRUPT(hpcd->Instance, USB_OTG_GINTSTS_RXFLVL); //mgs:????
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
		#if CPUSTYLE_STM32MP1
					  if (hpcd->Init.dma_enable == USB_ENABLE)
					  {
						  // USB_OTG_DOEPINT_STPKTRX
						if (USBx_OUTEP(0)->DOEPINT & (1 << 15))	// Setup packet received
						{
						  CLEAR_OUT_EP_INTR(epnum, (1 << 15));
						}
					  }
		#elif CPUSTYLE_STM32H7XX
					/* setup/out transaction management for Core ID >= 310A */
					if (USBx->GSNPSID >= USB_OTG_CORE_ID_310A)
					{
					  if (hpcd->Init.dma_enable == USB_ENABLE)
					  {
						  // USB_OTG_DOEPINT_STPKTRX
						if (USBx_OUTEP(0)->DOEPINT & (1 << 15))	// Setup packet received
						{
						  CLEAR_OUT_EP_INTR(epnum, (1 << 15));
						}
					  }
					}
		#endif /* CPUSTYLE_STM32H7XX */

					if (hpcd->Init.dma_enable == USB_ENABLE)
					{
					  hpcd->OUT_ep [epnum].xfer_count = hpcd->OUT_ep [epnum].maxpacket - ((USBx_OUTEP(epnum)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ) >> USB_OTG_DOEPTSIZ_XFRSIZ_Pos);
					  hpcd->OUT_ep [epnum].xfer_buff += hpcd->OUT_ep [epnum].maxpacket;
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
		#if CPUSTYLE_STM32MP1
					  if (hpcd->Init.dma_enable == USB_ENABLE)
					  {
						  // USB_OTG_DOEPINT_STPKTRX
						if (USBx_OUTEP(0)->DOEPINT & (1 << 15))
						{
						  CLEAR_OUT_EP_INTR(epnum, (1 << 15));
						}
					  }
		#elif CPUSTYLE_STM32H7XX
					/* setup/out transaction management for Core ID >= 310A */
					if (USBx->GSNPSID >= USB_OTG_CORE_ID_310A)
					{
					  if (hpcd->Init.dma_enable == USB_ENABLE)
					  {
						  // USB_OTG_DOEPINT_STPKTRX
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
				// Использование maxpacket вместо xfer_len важно для обработки персылок больше чем maxpacket
				  inep->xfer_buff += inep->maxpacket; // пересланный размер может отличаться от максимального
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
      USB_FlushTxFifoEx(hpcd->Instance,  0);	/* flush control endpoint tx FIFO */

      for (i = 0; i < hpcd->Init.dev_endpoints ; ++ i)
      {
        USBx_INEP(i)->DIEPINT = 0xFF;
        USBx_INEP(i)->DIEPCTL &= ~ USB_OTG_DIEPCTL_STALL;
        USBx_OUTEP(i)->DOEPINT = 0xFF;
        USBx_OUTEP(i)->DOEPCTL &= ~ USB_OTG_DOEPCTL_STALL;
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
        hpcd->Init.pcd_speed = PCD_SPEED_HIGH;
        //hpcd->Init.ep0_mps = USB_OTG_MAX_EP0_SIZE; //USB_OTG_HS_MAX_PACKET_SIZE;
        hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) |
			(uint32_t)((USBD_HS_TRDT_VALUE << USB_OTG_GUSBCFG_TRDT_Pos) & USB_OTG_GUSBCFG_TRDT) |
			0;
      }
      else
      {
        hpcd->Init.pcd_speed = PCD_SPEED_FULL;
        //hpcd->Init.ep0_mps = USB_OTG_MAX_EP0_SIZE;//USB_OTG_FS_MAX_PACKET_SIZE ;

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
// STM32F1XX
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


        if ((hpcd->USB_Address > 0) && ( ep->xfer_len == 0))
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
            ep->xfer_buff += ep->xfer_count;
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
        ep->xfer_buff += ep->xfer_count;

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
/**
  * @}
  */

/** @defgroup USBH_CTLREQ_Exported_FunctionsPrototype
  * @{
  */
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

#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
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
                              uint8_t usb_otg_speed,
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
  USBx_HC(ch_num)->HCCHAR = (((dev_address << USB_OTG_HCCHAR_DAD_Pos) & USB_OTG_HCCHAR_DAD)  |
                             (((epnum & 0x7F)<< USB_OTG_HCCHAR_EPNUM_Pos) & USB_OTG_HCCHAR_EPNUM)|
                             ((((epnum & 0x80) == 0x80)<< USB_OTG_HCCHAR_EPDIR_Pos) & USB_OTG_HCCHAR_EPDIR)|
                             (((usb_otg_speed == USB_OTG_SPEED_LOW) << USB_OTG_HCCHAR_LSDEV_Pos) & USB_OTG_HCCHAR_LSDEV)|
                             ((ep_type << USB_OTG_HCCHAR_EPTYP_Pos) & USB_OTG_HCCHAR_EPTYP)|
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
uint8_t  USBH_LL_GetToggle(USBH_HandleTypeDef *phost, uint8_t pipe)
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
	//PRINTF("USBH_BulkSendData\n");
  if (phost->device.usb_otg_speed != USB_OTG_SPEED_HIGH)
  {
    do_ping = 0;
  }

  USBH_LL_SubmitURB(phost,                     /* Driver handle    */
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
	//PRINTF("USBH_BulkReceiveData\n");
  USBH_LL_SubmitURB(phost,                     /* Driver handle    */
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
	//PRINTF("USBH_InterruptReceiveData\n");
  USBH_LL_SubmitURB(phost,                     /* Driver handle    */
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
	//PRINTF("USBH_InterruptSendData\n");
  USBH_LL_SubmitURB(phost,                     /* Driver handle    */
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
	//PRINTF("USBH_IsocReceiveData\n");
  USBH_LL_SubmitURB(phost,                     /* Driver handle    */
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
	//PRINTF("USBH_IsocSendData\n");
  USBH_LL_SubmitURB(phost,                     /* Driver handle    */
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

/**
  * @brief  DeInitializes PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *pcdHandle)
{
#if CPUSTYLE_R7S721

	if (pcdHandle->Instance == & USB200)
	{
		const IRQn_ID_t int_id = USBI0_IRQn;
		IRQ_Disable(int_id);

		/* ---- Supply clock to the USB20(channel 0) ---- */
		//CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		//(void) CPG.STBCR7;			/* Dummy read */

		//HARDWARE_USB0_UNINITIALIZE();

	}
	else if (pcdHandle->Instance == & USB201)
	{
		const IRQn_ID_t int_id = USBI1_IRQn;
		IRQ_Disable(int_id);

		/* ---- Supply clock to the USB20(channel 1) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP70;	// Module Stop 70 0: Channel 1 of the USB 2.0 host/function module runs.
		//CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */

		//HARDWARE_USB1_UNINITIALIZE();
	}
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

#elif CPUSTYLE_STM32MP1

	  if (pcdHandle->Instance == USB_OTG_HS)
	  {
	    /* Peripheral interrupt Deinit*/
	    IRQ_Disable(OTG_IRQn);
	  }

#elif CPUSTYLE_STM32F

#if defined (USB_OTG_HS)
	  if (pcdHandle->Instance == USB_OTG_HS)
	  {
	    /* Peripheral interrupt Deinit*/
	    NVIC_DisableIRQ(OTG_HS_IRQn);
	  }
#endif /* defined (USB_OTG_HS) */
#if defined (USB_OTG_FS)
	  if (pcdHandle->Instance == USB_OTG_FS)
	  {
	    /* Peripheral interrupt Deinit*/
	    NVIC_DisableIRQ(OTG_FS_IRQn);
	  }
#endif /* defined (USB_OTG_FS) */

#else
	#error HAL_PCD_MspDeInit should be implemented
#endif
}

#ifdef WITHUSBHW_DEVICE

/**
  * @brief  Initializes the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_LL_Init(PCD_HandleTypeDef * hpcd, USBD_HandleTypeDef *pdev)
{
	/* Link The driver to the stack */
	hpcd->pData = pdev;
	pdev->pData = hpcd;

	hpcd->Instance = WITHUSBHW_DEVICE;

#if CPUSTYLE_R7S721
	// Значение ep0_mps и speed обновится после reset шины
	#if WITHUSBDEV_HSDESC
		hpcd->Init.pcd_speed = PCD_SPEED_HIGH;
		//hpcd->Init.ep0_mps = USB_OTG_MAX_EP0_SIZE; //USB_OTG_HS_MAX_PACKET_SIZE;
	#else /* WITHUSBDEV_HSDESC */
		hpcd->Init.pcd_speed = PCD_SPEED_FULL;
		//hpcd->Init.ep0_mps = USB_OTG_MAX_EP0_SIZE; //USB_OTG_FS_MAX_PACKET_SIZE;
	#endif /* WITHUSBDEV_HSDESC */
	hpcd->Init.phy_itface = USB_OTG_EMBEDDED_PHY;

	hpcd->Init.dev_endpoints = 15;

#else /* CPUSTYLE_R7S721 */

	#if WITHUSBDEV_HSDESC
		hpcd->Init.pcd_speed = PCD_SPEED_HIGH;
		//hpcd->Init.ep0_mps = USB_OTG_MAX_EP0_SIZE; //DEP0CTL_MPS_64;
	#else /* WITHUSBDEV_HSDESC */
		hpcd->Init.pcd_speed = PCD_SPEED_FULL;
		//hpcd->Init.ep0_mps = USB_OTG_MAX_EP0_SIZE; //DEP0CTL_MPS_64;
	#endif /* WITHUSBDEV_HSDESC */

	#if WITHUSBDEV_HIGHSPEEDULPI
		hpcd->Init.phy_itface = USB_OTG_ULPI_PHY;
	#elif WITHUSBDEV_HIGHSPEEDPHYC
		hpcd->Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
	#else /* WITHUSBDEV_HIGHSPEEDULPI */
		hpcd->Init.phy_itface = USB_OTG_EMBEDDED_PHY;
	#endif /* WITHUSBDEV_HIGHSPEEDULPI */
		hpcd->Init.dev_endpoints = 9;	// or 6 for FS devices

#endif /* CPUSTYLE_R7S721 */

	#if WITHUSBDEV_VBUSSENSE	/* используется предопределенный вывод VBUS_SENSE */
		hpcd->Init.vbus_sensing_enable = USB_ENABLE;
	#else /* WITHUSBDEV_VBUSSENSE */
		hpcd->Init.vbus_sensing_enable = USB_DISABLE;
	#endif /* WITHUSBDEV_VBUSSENSE */

	#if WITHUSBDEV_DMAENABLE
		hpcd->Init.dma_enable = USB_ENABLE; // xyz
	#else /* WITHUSBDEV_DMAENABLE */
		hpcd->Init.dma_enable = USB_DISABLE; // xyz
	#endif /* WITHUSBDEV_DMAENABLE */
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
#else /* CPUSTYLE_R7S721 */
	if (USB_Is_OTG_HS(hpcd->Instance))
	{
		// У OTH_HS размер FIFO 4096 байт
		usbd_fifo_initialize(hpcd, 4096, 1);
	}
	else
	{
		// У OTH_FS размер FIFO 1280 байт
		usbd_fifo_initialize(hpcd, 1280, 0);
	}
#endif /* CPUSTYLE_R7S721 */
	return USBD_OK;
}

/**
  * @brief  De-Initializes the Low Level portion of the Device driver.
  * @param  pdev: Device handle
  * @retval USBD Status
  */
USBD_StatusTypeDef  USBD_LL_DeInit(USBD_HandleTypeDef *pdev)
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
USBD_StatusTypeDef USBD_Init2(USBD_HandleTypeDef *pdev)
{
	/* Check whether the USB Host handle is valid */
	if (pdev == NULL)
	{
		PRINTF("Invalid Device handle\n");
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
	pdev->dev_state = USBD_STATE_DEFAULT;

	/* Free Class Resources */
	for (di = 0; di < pdev->nClasses; ++ di)
	{
		  /* For each device function */
		  const USBD_ClassTypeDef * const pClass = pdev->pClasses [di];
		  pClass->DeInit(pdev, pdev->dev_config [0]);
	}

	/* Stop the low level driver  */
	USBD_LL_Stop(pdev);

	/* Initialize low level driver */
	USBD_LL_DeInit(pdev);

	return USBD_OK;
}


// BOOTLOADER support
static uint_fast8_t device_vbusbefore;

uint_fast8_t hardware_usbd_get_vbusbefore(void)
{
	return device_vbusbefore;
}

static uint_fast8_t hardware_usbd_get_vbusnow0(void)
{
#if CPUSTYLE_R7S721
	return (WITHUSBHW_DEVICE->INTSTS0 & USB_INTSTS0_VBSTS) != 0;

#elif (CPUSTYLE_STM32F || CPUSTYLE_STM32MP1) && defined (USB_OTG_GOTGCTL_BSESVLD_Msk) && WITHUSBDEV_VBUSSENSE
	return (WITHUSBHW_DEVICE->GOTGCTL & USB_OTG_GOTGCTL_BSESVLD_Msk) != 0;

#else /* CPUSTYLE_R7S721 */
	return 0;

#endif /* CPUSTYLE_R7S721 */
}


uint_fast8_t hardware_usbd_get_vbusnow(void)
{
	uint_fast8_t st0;
	uint_fast8_t st = hardware_usbd_get_vbusnow0();

	do
	{
		st0 = st;
		st = hardware_usbd_get_vbusnow0();
	} while (st0 != st);
	return st;
}
/* вызывается при запрещённых прерываниях. */
static void hardware_usbd_initialize(void)
{
	PRINTF("hardware_usbd_initialize start\n");
#if WITHUSBDEV_HSDESC
	usbd_descriptors_initialize(1);

#else /* WITHUSBDEV_HSDESC */
	usbd_descriptors_initialize(0);

#endif /* WITHUSBDEV_HSDESC */

	USBD_Init2(& hUsbDevice);

	// поддержка работы бутлоадера на платах, где есть подпитка VBUS от DP через защитные диоды
	device_vbusbefore = hardware_usbd_get_vbusnow();
	//PRINTF(PSTR("hardware_usbd_initialize: device_vbusbefore=%d\n"), (int) device_vbusbefore);

#if WITHUSBUAC
	USBD_AddClass(& hUsbDevice, & USBD_CLASS_UAC);
#endif /* WITHUSBUAC */
#if WITHUSBCDC
	USBD_AddClass(& hUsbDevice, & USBD_CLASS_CDC);
#endif /* WITHUSBCDC */
#if WITHUSBDFU
	USBD_AddClass(& hUsbDevice, & USBD_CLASS_DFU);
#endif /* WITHUSBDFU */
#if WITHUSBCDCEEM
	USBD_AddClass(& hUsbDevice, & USBD_CLASS_CDC_EEM);
#endif /* WITHUSBCDCEEM */
	PRINTF("hardware_usbd_initialize done\n");
}

/* вызывается при запрещённых прерываниях. */
static void board_usbd_initialize(void)
{
	hardware_usbd_initialize();
}


#endif /* WITHUSBHW_DEVICE */

#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1
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

#endif /* CPUSTYLE_STM32F */

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
HAL_StatusTypeDef HAL_HCD_ResetPort(HCD_HandleTypeDef *hhcd, uint_fast8_t state)
{
	// state 0: reset off, 1: reset on
	USB_ResetPort(hhcd->Instance, state);
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

  hal_status = HAL_HCD_Start(phost->pData);	// USB_DriveVbus inside

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
  * @retval USB_OTG_SPEED_xxx Speeds
  */
uint8_t USBH_LL_GetSpeed(USBH_HandleTypeDef *phost)
{
	uint8_t speed;

	switch (HAL_HCD_GetCurrentSpeed(phost->pData))
	{
	case USB_OTG_SPEED_HIGH:
		speed = USB_OTG_SPEED_HIGH;
		break;

	case USB_OTG_SPEED_HIGH_IN_FULL:
		speed = USB_OTG_SPEED_FULL;
		break;

	case USB_OTG_SPEED_LOW:
		speed = USB_OTG_SPEED_LOW;
		break;

	default:
	case USB_OTG_SPEED_FULL:
		speed = USB_OTG_SPEED_FULL;
		break;
	}
	PRINTF("USBH_LL_GetSpeed: (high=%d, full=%d, low=%d) speed=%d\n", (int) USB_OTG_SPEED_HIGH, (int) USB_OTG_SPEED_FULL, (int) USB_OTG_SPEED_LOW, (int) speed);
	return speed;
}

/**
  * @brief  USBH_LL_ResetPort
  *         Reset the Host Port of the Low Level Driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_ResetPort(USBH_HandleTypeDef *phost, uint_fast8_t state)
{
	// state 0: reset off, 1: reset on
	const HAL_StatusTypeDef hal_status = HAL_HCD_ResetPort(phost->pData, state);
	USBH_StatusTypeDef usb_status = USBH_OK;

	switch (hal_status)
	{
	case HAL_OK:
		usb_status = USBH_OK;
		break;
	case HAL_ERROR:
		usb_status = USBH_FAIL;
		break;
	case HAL_BUSY:
		usb_status = USBH_BUSY;
		break;
	case HAL_TIMEOUT:
		usb_status = USBH_FAIL;
		break;
	default:
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
	HARDWARE_DELAY_MS(200);
	return USBH_OK;
}

#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

/**
  * @brief  Initializes the PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
#if CPUSTYLE_STM32MP1
	// Set 3.3 volt DETECTOR enable
	PWR->CR3 |= PWR_CR3_USB33DEN_Msk;
	(void) PWR->CR3;
	while ((PWR->CR3 & PWR_CR3_USB33DEN_Msk) == 0)
		;

	// Wait 3.3 volt REGULATOR ready
	while ((PWR->CR3 & PWR_CR3_USB33RDY_Msk) == 0)
		;

	RCC->MP_AHB2ENSETR = RCC_MC_AHB2ENSETR_USBOEN;
	(void) RCC->MP_AHB2ENSETR;
	RCC->MP_AHB2LPENSETR = RCC_MC_AHB2LPENSETR_USBOLPEN;
	(void) RCC->MP_AHB2LPENSETR;

	if (hpcd->Instance == USB1_OTG_HS)	// legacy name is USB_OTG_HS
	{
		if (hpcd->Init.phy_itface == USB_OTG_ULPI_PHY)
		{
			//USBD_HS_ULPI_INITIALIZE();

//			RCC->AHB1ENR |= RCC_AHB1ENR_USB1OTGHSEN | RCC_AHB1ENR_USB1OTGHSULPIEN;	/* USB/OTG HS with ULPI */
//			(void) RCC->AHB1ENR;
//			RCC->AHB1LPENR |= RCC_AHB1LPENR_USB1OTGHSLPEN; /* USB/OTG HS  */
//			(void) RCC->AHB1LPENR;
//			RCC->AHB1LPENR |= RCC_AHB1LPENR_USB1OTGHSULPILPEN; /* USB/OTG HS ULPI  */
//			(void) RCC->AHB1LPENR;
		}
		else
		{
			USBD_HS_FS_INITIALIZE();
		}
		//RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;	/* USB/OTG HS companion - VBUS? */
		//(void) RCC->APB4ENR;

//		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM1EN;
//		(void) RCC->AHB2ENR;
//		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM2EN;
//		(void) RCC->AHB2ENR;
//		RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM3EN;
//		(void) RCC->AHB2ENR;


		if (hpcd->Init.use_dedicated_ep1 == USB_ENABLE)
		{
			//arm_hardware_set_handler_system(OTG_HS_EP1_OUT_IRQn, device_OTG_HS_EP1_OUT_IRQHandler);
			//arm_hardware_set_handler_system(OTG_HS_EP1_IN_IRQn, device_OTG_HS_EP1_IN_IRQHandler);
		}
		arm_hardware_set_handler_system(OTG_IRQn, device_OTG_HS_IRQHandler);

	}

#elif CPUSTYLE_STM32H7XX

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
			arm_hardware_set_handler_system(OTG_HS_EP1_OUT_IRQn, device_OTG_HS_EP1_OUT_IRQHandler);
			arm_hardware_set_handler_system(OTG_HS_EP1_IN_IRQn, device_OTG_HS_EP1_IN_IRQHandler);
		}
		arm_hardware_set_handler_system(OTG_HS_IRQn, device_OTG_HS_IRQHandler);

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
	//PRINTF(PSTR("HAL_HCD_MspInit()\n"));
	if (hcdHandle->Instance == USB_OTG_FS)
	{
		#if CPUSTYLE_STM32MP1

			// Set 3.3 volt DETECTOR enable
			PWR->CR3 |= PWR_CR3_USB33DEN_Msk;
			(void) PWR->CR3;
			while ((PWR->CR3 & PWR_CR3_USB33DEN_Msk) == 0)
				;

			// Wait 3.3 volt REGULATOR ready
			while ((PWR->CR3 & PWR_CR3_USB33RDY_Msk) == 0)
				;

			RCC->MP_AHB2ENSETR = RCC_MC_AHB2ENSETR_USBOEN;
			(void) RCC->MP_AHB2ENSETR;
			RCC->MP_AHB2LPENSETR = RCC_MC_AHB2LPENSETR_USBOLPEN;
			(void) RCC->MP_AHB2LPENSETR;

			arm_hardware_set_handler_system(OTG_IRQn, host_OTG_FS_IRQHandler);

		#elif CPUSTYLE_STM32H7XX

			//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
			//PRINTF(PSTR("HAL_HCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);
			USBD_FS_INITIALIZE();

			RCC->AHB1ENR |= RCC_AHB1ENR_USB2OTGFSEN_Msk;	/* USB/OTG FS  */
			(void) RCC->AHB1ENR;
			RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN_Msk;	/* USB/OTG FS companion - VBUS? */
			(void) RCC->APB2ENR;

			arm_hardware_set_handler_system(OTG_FS_IRQn, host_OTG_FS_IRQHandler);

		#else
			//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
			//PRINTF(PSTR("HAL_HCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);

			USBD_FS_INITIALIZE();

			RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN_Msk;	/* USB/OTG FS  */
			(void) RCC->AHB2ENR;
			RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN_Msk;	/* USB/OTG FS companion - VBUS? */
			(void) RCC->APB2ENR;

			arm_hardware_set_handler_system(OTG_FS_IRQn, host_OTG_FS_IRQHandler);

		#endif
	}
	else if (hcdHandle->Instance == USB_OTG_HS)
	{
	#if CPUSTYLE_STM32MP1

		// Set 3.3 volt DETECTOR enable
		PWR->CR3 |= PWR_CR3_USB33DEN_Msk;
		(void) PWR->CR3;
		while ((PWR->CR3 & PWR_CR3_USB33DEN_Msk) == 0)
			;

		// Wait 3.3 volt REGULATOR ready
		while ((PWR->CR3 & PWR_CR3_USB33RDY_Msk) == 0)
			;

		RCC->MP_AHB2ENSETR = RCC_MC_AHB2ENSETR_USBOEN;
		(void) RCC->MP_AHB2ENSETR;
		RCC->MP_AHB2LPENSETR = RCC_MC_AHB2LPENSETR_USBOLPEN;
		(void) RCC->MP_AHB2LPENSETR;

		arm_hardware_set_handler_system(OTG_IRQn, host_OTG_HS_IRQHandler);

	#elif CPUSTYLE_STM32H7XX

		//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
		//PRINTF(PSTR("HAL_HCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);
		USBD_FS_INITIALIZE();

		RCC->AHB1ENR |= RCC_AHB1ENR_USB1OTGHSEN_Msk;	/* USB/OTG HS  */
		(void) RCC->AHB1ENR;
		RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN_Msk;	/* USB/OTG HS companion - VBUS? */
		(void) RCC->APB2ENR;

		arm_hardware_set_handler_system(OTG_HS_IRQn, host_OTG_HS_IRQHandler);

	#else
		//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
		//PRINTF(PSTR("HAL_HCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);

		USBD_FS_INITIALIZE();

		RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN_Msk;	/* USB/OTG HS  */
		(void) RCC->AHB2ENR;
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN_Msk;	/* USB/OTG HS companion - VBUS? */
		(void) RCC->APB2ENR;

		arm_hardware_set_handler_system(OTG_HS_IRQn, host_OTG_HS_IRQHandler);


	#endif
	}
	else
	{
		ASSERT(0);
	}
}

void HAL_HCD_MspDeInit(HCD_HandleTypeDef* hpcd)
{
#if CPUSTYLE_STM32MP1

	if (hpcd->Instance == USB_OTG_HS)
	{
		/* Peripheral interrupt Deinit*/
		IRQ_Disable(OTG_IRQn);
	}

#else

#if defined (USB_OTG_HS)
	if (hpcd->Instance == USB_OTG_HS)
	{
		/* Peripheral interrupt Deinit*/
		NVIC_DisableIRQ(OTG_HS_IRQn);
	}
#endif /* defined (USB_OTG_HS) */

#if defined (USB_OTG_FS)
	if (hpcd->Instance == USB_OTG_FS)
	{
		/* Peripheral interrupt Deinit*/
		NVIC_DisableIRQ(OTG_FS_IRQn);
	}
#endif /* defined (USB_OTG_FS) */

#endif
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
#endif /* CPUSTYLE_STM32F */
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
	if (hhcd == NULL)
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
	USB_SetCurrentMode(hhcd->Instance, USB_OTG_HOST_MODE);

	/* Init Host */
	USB_HostInit(hhcd->Instance, & hhcd->Init);

	hhcd->State = HAL_HCD_STATE_READY;

	return HAL_OK;
}


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
	/* Link The driver to the stack */
	hhcd_USB_OTG.pData = phost;
	phost->pData = & hhcd_USB_OTG;

	hhcd_USB_OTG.Instance = WITHUSBHW_HOST;

#if CPUSTYLE_R7S721
	hhcd_USB_OTG.Init.Host_channels = 16;
	hhcd_USB_OTG.Init.pcd_speed = PCD_SPEED_HIGH;
	//hhcd_USB_OTG.Init.pcd_speed = PCD_SPEED_FULL;
	hhcd_USB_OTG.Init.dma_enable = USB_DISABLE;
	hhcd_USB_OTG.Init.phy_itface = HCD_PHY_EMBEDDED;

#elif CPUSTYLE_STM32MP1
	hhcd_USB_OTG.Init.Host_channels = 16;
	hhcd_USB_OTG.Init.pcd_speed = PCD_SPEED_HIGH;
	#if WITHUSBHOST_DMAENABLE
		hhcd_USB_OTG.Init.dma_enable = USB_ENABLE;	 // xyz HOST
	#else /* WITHUSBHOST_DMAENABLE */
		hhcd_USB_OTG.Init.dma_enable = USB_DISABLE;	 // xyz HOST
	#endif /* WITHUSBHOST_DMAENABLE */
	hhcd_USB_OTG.Init.phy_itface = HCD_PHY_EMBEDDED;
	hhcd_USB_OTG.Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
	#if WITHUSBHOST_HIGHSPEEDULPI
		hhcd_USB_OTG.Init.phy_itface = USB_OTG_ULPI_PHY;
	#elif WITHUSBHOST_HIGHSPEEDPHYC
		hhcd_USB_OTG.Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
	#else /* WITHUSBHOST_HIGHSPEEDULPI */
		hhcd_USB_OTG.Init.phy_itface = USB_OTG_EMBEDDED_PHY;
	#endif /* WITHUSBHOST_HIGHSPEEDULPI */


#else /* CPUSTYLE_R7S721 */
	hhcd_USB_OTG.Init.Host_channels = 16;
	hhcd_USB_OTG.Init.pcd_speed = PCD_SPEED_FULL;
	#if WITHUSBHOST_DMAENABLE
		hhcd_USB_OTG.Init.dma_enable = USB_ENABLE;	 // xyz HOST
	#else /* WITHUSBHOST_DMAENABLE */
		hhcd_USB_OTG.Init.dma_enable = USB_DISABLE;	 // xyz HOST
	#endif /* WITHUSBHOST_DMAENABLE */
	hhcd_USB_OTG.Init.phy_itface = HCD_PHY_EMBEDDED;

#endif /* CPUSTYLE_R7S721 */

	hhcd_USB_OTG.Init.Sof_enable = USB_DISABLE;
	if (HAL_HCD_Init(& hhcd_USB_OTG) != HAL_OK)
	{
		ASSERT(0);
	}

	USBH_LL_SetTimer(phost, HAL_HCD_GetCurrentFrame(& hhcd_USB_OTG));
	return USBH_OK;
}

#endif /* defined (WITHUSBHW_HOST) */

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
                                  uint8_t usbh_otg_speed,
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
  hhcd->hc[ch_num].usbh_otg_speed = usbh_otg_speed;

  status = USB_HC_Init(hhcd->Instance,
                        ch_num,
                        epnum,
                        dev_address,
                        usbh_otg_speed,
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
void  USBH_LL_IncTimer(USBH_HandleTypeDef *phost)
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
USBH_StatusTypeDef  USBH_LL_DeInit(USBH_HandleTypeDef *phost)
{
	HAL_StatusTypeDef hal_status = HAL_HCD_DeInit(phost->pData);
	USBH_StatusTypeDef usb_status = USBH_OK;

	switch (hal_status)
	{
	case HAL_OK:
		usb_status = USBH_OK;
		break;
	case HAL_ERROR:
		usb_status = USBH_FAIL;
		break;
	case HAL_BUSY:
		usb_status = USBH_BUSY;
		break;
	case HAL_TIMEOUT:
		usb_status = USBH_FAIL;
		break;
	default:
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

	for (idx = 0 ; idx < USBHNPIPES; idx++)
	{
		if ((phost->Pipes [idx] & 0x8000) == 0)
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
USBH_StatusTypeDef USBH_FreePipe(USBH_HandleTypeDef *phost, uint8_t idx)
{
	if (idx < USBHNPIPES)
	{
		phost->Pipes [idx] &= 0x7FFF;
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

#if defined (WITHUSBHW_HOST)

/**
  * @brief  HCD_Init
  *         Initialize the HOST Core.
  * @param  phost: Host Handle
  * @param  pUsrFunc: User Callback
  * @retval USBH Status
  */
/* вызывается при разрешённых прерываниях. */
USBH_StatusTypeDef  USBH_Init(USBH_HandleTypeDef *phost, void (*pUsrFunc)(USBH_HandleTypeDef *phost, uint8_t ))
{
  /* Check whether the USB Host handle is valid */
  if (phost == NULL)
  {
    PRINTF(PSTR("Invalid Host handle\n"));
    return USBH_FAIL;
  }

  /* Set DRiver ID */
  //phost->id = id;

  /* Unlink class*/
  phost->pActiveClass = NULL;
  phost->ClassNumber = 0;

  /* Restore default states and prepare EP0 */
  DeInitStateMachine(phost);

  /* Assign User process */
  if (pUsrFunc != NULL)
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

#endif /* defined (WITHUSBHW_HOST) */

/*
 * user callback definition
*/
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
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
void  USBH_HandleSof(USBH_HandleTypeDef *phost)
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
USBH_StatusTypeDef USBH_LL_OpenPipe(
	USBH_HandleTypeDef *phost,
	uint8_t pipe_num,
	uint8_t epnum,
	uint8_t dev_address,
	uint8_t usbh_otg_speed,
	uint8_t ep_type,
	uint16_t mps
	)
{
	const HAL_StatusTypeDef hal_status = HAL_HCD_HC_Init(phost->pData, pipe_num, epnum, dev_address, usbh_otg_speed, ep_type, mps);
	USBH_StatusTypeDef usb_status;

	switch (hal_status)
	{
	case HAL_OK:
		usb_status = USBH_OK;
		break;
	case HAL_ERROR:
		usb_status = USBH_FAIL;
		break;
	case HAL_BUSY:
		usb_status = USBH_BUSY;
		break;
	case HAL_TIMEOUT:
		usb_status = USBH_FAIL;
		break;
	default:
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
USBH_StatusTypeDef   USBH_LL_ClosePipe(USBH_HandleTypeDef *phost, uint8_t pipe)
{
	const HAL_StatusTypeDef hal_status = HAL_HCD_HC_Halt(phost->pData, pipe);
	USBH_StatusTypeDef usb_status = USBH_OK;

	switch (hal_status)
	{
	case HAL_OK:
		usb_status = USBH_OK;
		break;
	case HAL_ERROR:
		usb_status = USBH_FAIL;
		break;
	case HAL_BUSY:
		usb_status = USBH_BUSY;
		break;
	case HAL_TIMEOUT:
		usb_status = USBH_FAIL;
		break;
	default:
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
USBH_URBStateTypeDef  USBH_LL_GetURBState(USBH_HandleTypeDef *phost, uint8_t pipe)
{
  return (USBH_URBStateTypeDef) HAL_HCD_HC_GetURBState(phost->pData, pipe);
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

USBH_StatusTypeDef   USBH_LL_SubmitURB(USBH_HandleTypeDef *phost,
                                            uint8_t pipe,
                                            uint8_t direction ,
                                            uint8_t ep_type,  
                                            uint8_t token, 
                                            uint8_t* pbuff, 
                                            uint16_t length,
                                            uint8_t do_ping )
{
	HAL_StatusTypeDef hal_status = HAL_HCD_HC_SubmitRequest(phost->pData, pipe, direction, ep_type,
		token,
		pbuff,
		length,
		do_ping
		);
	USBH_StatusTypeDef usb_status = USBH_OK;

	switch (hal_status)
	{
	case HAL_OK:
		usb_status = USBH_OK;
		break;
	case HAL_ERROR:
		usb_status = USBH_FAIL;
		break;
	case HAL_BUSY:
		usb_status = USBH_BUSY;
		break;
	case HAL_TIMEOUT:
		usb_status = USBH_FAIL;
		break;
	default:
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
USBH_StatusTypeDef USBH_CtlSendSetup(USBH_HandleTypeDef *phost,
                                uint8_t *buff,
                                uint8_t pipe_num)
{

	//PRINTF("USBH_CtlSendSetup\n");
  USBH_LL_SubmitURB(phost,                     /* Driver handle    */
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
	//PRINTF("USBH_CtlSendData\n");
  if (phost->device.usb_otg_speed != USB_OTG_SPEED_HIGH)
  {
    do_ping = 0;
  }
  
  USBH_LL_SubmitURB(phost,                     /* Driver handle    */
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
	//PRINTF("USBH_CtlReceiveData\n");
  USBH_LL_SubmitURB(phost,                     /* Driver handle    */
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
  * @param  speed : USB device speed (Full/Low) - USB_OTG_SPEED_xxx
  * @param  ep_type: end point type (Bulk/int/ctl)
  * @param  mps: max pkt size
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_OpenPipe  (USBH_HandleTypeDef *phost,
                            uint8_t pipe_num,
                            uint8_t epnum,
                            uint8_t dev_address,
                            uint8_t usb_otg_speed,	// USB_OTG_SPEED_xxx
                            uint8_t ep_type,
                            uint16_t mps)
{

  USBH_LL_OpenPipe(phost,
                        pipe_num,
                        epnum,
                        dev_address,
						usb_otg_speed,
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
  phost->device.usb_otg_speed = USB_OTG_SPEED_FULL;

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
void  USBH_ParseDevDesc(USBH_DevDescTypeDef* dev_desc,
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
void  USBH_ParseEPDesc(USBH_EpDescTypeDef  *ep_descriptor,
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
  return USBH_CtlReq(phost, buff, length );
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
  return USBH_CtlReq(phost, 0, 0 );
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
	if (nticks > 1)
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
      USBH_OpenPipe(phost,
                           phost->Control.pipe_in,
                           0x80,
                           phost->device.address,
                           phost->device.usb_otg_speed,
                           USBH_EP_CONTROL,
                           phost->Control.pipe_size);

      /* Open Control pipes */
      USBH_OpenPipe(phost,
                           phost->Control.pipe_out,
                           0x00,
                           phost->device.address,
                           phost->device.usb_otg_speed,
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
    	USBH_UsrLog("PID: %04X", phost->device.DevDesc.idProduct );
    	USBH_UsrLog("VID: %04X", phost->device.DevDesc.idVendor );

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
      USBH_UsrLog("Address (#%d) assigned.", phost->device.address);
      phost->EnumState = ENUM_GET_CFG_DESC;

      /* modify control channels to update device address */
      USBH_OpenPipe(phost,
                           phost->Control.pipe_in,
                           0x80,
                           phost->device.address,
                           phost->device.usb_otg_speed,
                           USBH_EP_CONTROL,
                           phost->Control.pipe_size);

      /* Open Control pipes */
      USBH_OpenPipe(phost,
                           phost->Control.pipe_out,
                           0x00,
                           phost->device.address,
                           phost->device.usb_otg_speed,
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
    	  USBH_UsrLog("Manufacturer : %s",  (char *)phost->device.Data);
        phost->EnumState = ENUM_GET_PRODUCT_STRING_DESC;

#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
#endif
      }
    }
    else
    {
    	USBH_UsrLog("Manufacturer : N/A");
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
    	  USBH_UsrLog("Product : %s",  (char *)phost->device.Data);
        phost->EnumState = ENUM_GET_SERIALNUM_STRING_DESC;
      }
    }
    else
    {
    	USBH_UsrLog("Product : N/A");
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
    	  USBH_UsrLog("Serial Number : %s",  (char *)phost->device.Data);
        Status = USBH_OK;
      }
    }
    else
    {
    	USBH_UsrLog("Serial Number : N/A");
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
		PRINTF(PSTR("USBH_Process: HOST_DEV_BUS_RESET_ON\n"));
		USBH_LL_ResetPort(phost, 1);
		USBH_ProcessDelay(phost, HOST_DEV_BUS_RESET_OFF, 50);
		break;

	case HOST_DEV_BUS_RESET_OFF:
		PRINTF(PSTR("USBH_Process: HOST_DEV_BUS_RESET_OFF\n"));
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
		PRINTF(PSTR("USBH_Process: HOST_DEV_WAIT_FOR_ATTACHMENT\n"));
		break;

	case HOST_DEV_BEFORE_ATTACHED:
		// в этом состоянии игнорируем disconnect - возникает при старте устройства с установленным USB устройством
		  //PRINTF(PSTR("USBH_Process: HOST_DEV_BEFORE_ATTACHED\n"));
		/* Wait for 100 ms after Reset */
		USBH_ProcessDelay(phost, HOST_DEV_ATTACHED, 100);
		break;

	case HOST_DEV_ATTACHED:
		  PRINTF(PSTR("USBH_Process: HOST_DEV_ATTACHED\n"));
		/* после таймаута */
		phost->device.usb_otg_speed = USBH_LL_GetSpeed(phost);

		phost->gState = HOST_ENUMERATION;

		phost->Control.pipe_out = USBH_AllocPipe(phost, 0x00);
		phost->Control.pipe_in  = USBH_AllocPipe(phost, 0x80);


		/* Open Control pipes */
		USBH_OpenPipe(phost,
			phost->Control.pipe_in,
			0x80,
			phost->device.address,
			phost->device.usb_otg_speed,
			USBH_EP_CONTROL,
			phost->Control.pipe_size);

		/* Open Control pipes */
		USBH_OpenPipe(phost,
			phost->Control.pipe_out,
			0x00,
			phost->device.address,
			phost->device.usb_otg_speed,
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

#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1
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

			ASSERT(hhcd->hc [channelnum].xfer_buff != NULL);
			USB_ReadPacket(hhcd->Instance, hhcd->hc [channelnum].xfer_buff, pktcnt);

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
        if (hhcd->Init.pcd_speed == PCD_SPEED_FULL)
        {
          USBx_HOST->HFIR = (uint32_t)60000;
        }
      }
      HAL_HCD_Connect_Callback(hhcd);

      if (hhcd->Init.pcd_speed == PCD_SPEED_HIGH)
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

#endif /* CPUSTYLE_STM32F */

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
#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1
/**
  * @brief  This function handles HCD interrupt request.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_IRQHandler(HCD_HandleTypeDef *hhcd)
{
	__DMB();
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

#endif /* CPUSTYLE_STM32F */


// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
static void
board_usb_tspool(void * ctx)
{
#if defined (WITHUSBHW_HOST)
	USBH_Process(& hUSB_Host);

#endif /* defined (WITHUSBHW_HOST) */
}

void
board_usb_spool(void)
{
#if defined (WITHUSBHW_HOST)
	//USBH_Process(& hUSB_Host);

#endif /* defined (WITHUSBHW_HOST) */
}

static ticker_t usbticker;

#if WITHUSEUSBFLASH
/* class definition */
#include "MSC/Inc/usbh_msc.h"
#endif /* WITHUSEUSBFLASH */

/* вызывается при запрещённых прерываниях. */
void board_usb_initialize(void)
{
	//PRINTF(PSTR("board_usb_initialize start.\n"));

#if defined (WITHUSBHW_DEVICE)
	board_usbd_initialize();	// USB device support
#endif /* defined (WITHUSBHW_DEVICE) */

#if defined (WITHUSBHW_HOST)
	/* Init Host Library,Add Supported Class and Start the library*/
	USBH_Init(& hUSB_Host, USBH_UserProcess);

	#if WITHUSEUSBFLASH
		USBH_RegisterClass(& hUSB_Host, & USBH_msc);
	#endif /* WITHUSEUSBFLASH */
	ticker_initialize(& usbticker, 1, board_usb_tspool, NULL);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.

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
	USBH_Start(& hUSB_Host);
#endif /* defined (WITHUSBHW_HOST) */

	PRINTF(PSTR("board_usb_activate done.\n"));
}

/* вызывается при разрешённых прерываниях. */
void board_usb_deactivate(void)
{
	//PRINTF(PSTR("board_usb_activate start.\n"));

#if defined (WITHUSBHW_HOST)
	USBH_Stop(& hUSB_Host);
#endif /* defined (WITHUSBHW_HOST) */

#if defined (WITHUSBHW_DEVICE)
	  USBD_Stop(& hUsbDevice);
#endif /* defined (WITHUSBHW_DEVICE) */

	//PRINTF(PSTR("board_usb_activate done.\n"));
}

/* вызывается при разрешенных прерываниях. */
void board_usb_deinitialize(void)
{
#if defined (WITHUSBHW_HOST)
	USBH_DeInit(& hUSB_Host);
#endif /* defined (WITHUSBHW_HOST) */

#if defined (WITHUSBHW_DEVICE)
	USBD_DeInit(& hUsbDevice);
#endif /* defined (WITHUSBHW_DEVICE) */
}

uint_fast8_t hamradio_get_usbh_active(void)
{
#if defined (WITHUSBHW_HOST)
	return hUSB_Host.device.is_connected != 0 && hUSB_Host.gState == HOST_CLASS;
	return hUSB_Host.device.is_connected != 0;
#else
	return  0;
#endif /* defined (WITHUSBHW_HOST) */
}


#endif /* WITHUSBHW */

#if WITHEHCIHW


// ------------------------------------------------------------------------------------------------
// Limits

#define MAX_QH                          8
#define MAX_TD                          32

// ------------------------------------------------------------------------------------------------
// PCI Configuration Registers

// EECP-based
#define USBLEGSUP                       0x00        // USB Legacy Support Extended Capability
#define UBSLEGCTLSTS                    0x04        // USB Legacy Support Control/Status

// ------------------------------------------------------------------------------------------------
// USB Legacy Support Register

#define USBLEGSUP_HC_OS                 0x01000000uL  // HC OS Owned Semaphore
#define USBLEGSUP_HC_BIOS               0x00010000uL  // HC BIOS Owned Semaphore
#define USBLEGSUP_NCP_MASK              0x0000ff00uL  // Next EHCI Extended Capability Pointer
#define USBLEGSUP_CAPID                 0x000000ffuL  // Capability ID

// ------------------------------------------------------------------------------------------------
// Host Controller Capability Registers

typedef struct EhciCapRegs
{
    uint8_t capLength;
    uint8_t reserved;
    uint16_t hciVersion;
    uint32_t hcsParams;
    uint32_t hccParams;
    uint64_t hcspPortRoute;
} ATTRPACKED EhciCapRegs;

// ------------------------------------------------------------------------------------------------
// Host Controller Structural Parameters Register

#define HCSPARAMS_N_PORTS_MASK          (15uL << 0)   // Number of Ports
#define HCSPARAMS_PPC                   (1uL << 4)    // Port Power Control
#define HCSPARAMS_PORT_ROUTE            (1uL << 7)    // Port Routing Rules
#define HCSPARAMS_N_PCC_MASK            (15uL << 8)   // Number of Ports per Companion Controller
#define HCSPARAMS_N_PCC_SHIFT           8
#define HCSPARAMS_N_CC_MASK             (15uL << 12)  // Number of Companion Controllers
#define HCSPARAMS_N_CC_SHIFT            12
#define HCSPARAMS_P_INDICATOR           (1uL << 16)   // Port Indicator
#define HCSPARAMS_DPN_MASK              (15uL << 20)  // Debug Port Number
#define HCSPARAMS_DPN_SHIFT             20

// ------------------------------------------------------------------------------------------------
// Host Controller Capability Parameters Register

#define HCCPARAMS_64_BIT                (1uL << 0)    // 64-bit Addressing Capability
#define HCCPARAMS_PFLF                  (1uL << 1)    // Programmable Frame List Flag
#define HCCPARAMS_ASPC                  (1uL << 2)    // Asynchronous Schedule Park Capability
#define HCCPARAMS_IST_MASK              (15uL << 4)   // Isochronous Sheduling Threshold
#define HCCPARAMS_EECP_MASK             (255uL << 8)  // EHCI Extended Capabilities Pointer
#define HCCPARAMS_EECP_SHIFT            8

// ------------------------------------------------------------------------------------------------
// Host Controller Operational Registers

typedef struct EhciOpRegs
{
    volatile uint32_t usbCmd;
    volatile uint32_t usbSts;
    volatile uint32_t usbIntr;
    volatile uint32_t frameIndex;
    volatile uint32_t ctrlDsSegment;
    volatile uint32_t periodicListBase;
    volatile uint32_t asyncListAddr;
    volatile uint32_t reserved[9];
    volatile uint32_t configFlag;
    volatile uint32_t ports[];
} EhciOpRegs;

// ------------------------------------------------------------------------------------------------
// USB Command Register

#define CMD_RS                          (1uL << 0)    // Run/Stop
#define CMD_HCRESET                     (1uL << 1)    // Host Controller Reset
#define CMD_FLS_MASK                    (3uL << 2)    // Frame List Size
#define CMD_FLS_SHIFT                   2
#define CMD_PSE                         (1uL << 4)    // Periodic Schedule Enable
#define CMD_ASE                         (1uL << 5)    // Asynchronous Schedule Enable
#define CMD_IOAAD                       (1uL << 6)    // Interrupt on Async Advance Doorbell
#define CMD_LHCR                        (1uL << 7)    // Light Host Controller Reset
#define CMD_ASPMC_MASK                  (3uL << 8)    // Asynchronous Schedule Park Mode Count
#define CMD_ASPMC_SHIFT                 8
#define CMD_ASPME                       (1uL << 11)   // Asynchronous Schedule Park Mode Enable
#define CMD_ITC_MASK                    (255uL << 16) // Interrupt Threshold Control
#define CMD_ITC_SHIFT                   16

// ------------------------------------------------------------------------------------------------
// USB Status Register

#define STS_USBINT                      (1uL << 0)    // USB Interrupt
#define STS_ERROR                       (1uL << 1)    // USB Error Interrupt
#define STS_PCD                         (1uL << 2)    // Port Change Detect
#define STS_FLR                         (1uL << 3)    // Frame List Rollover
#define STS_HSE                         (1uL << 4)    // Host System Error
#define STS_IOAA                        (1uL << 5)    // Interrupt on Async Advance
#define STS_HCHALTED                    (1uL << 12)   // Host Controller Halted
#define STS_RECLAMATION                 (1uL << 13)   // Reclamation
#define STS_PSS                         (1uL << 14)   // Periodic Schedule Status
#define STS_ASS                         (1uL << 15)   // Asynchronous Schedule Status

// ------------------------------------------------------------------------------------------------
// USB Interrupt Enable Register

#define INTR_USBINT                     (1uL << 0)    // USB Interrupt Enable
#define INTR_ERROR                      (1uL << 1)    // USB Error Interrupt Enable
#define INTR_PCD                        (1uL << 2)    // Port Change Interrupt Enable
#define INTR_FLR                        (1uL << 3)    // Frame List Rollover Enable
#define INTR_HSE                        (1uL << 4)    // Host System Error Enable
#define INTR_IOAA                       (1uL << 5)    // Interrupt on Async Advance Enable

// ------------------------------------------------------------------------------------------------
// Frame Index Register

#define FR_MASK                         0x3fff

// ------------------------------------------------------------------------------------------------
// Configure Flag Register

#define CF_PORT_ROUTE                   (1uL << 0)    // Configure Flag (CF)

// ------------------------------------------------------------------------------------------------
// Port Status and Control Registers

#define PORT_CONNECTION                 (1uL << 0)    // Current Connect Status
#define PORT_CONNECTION_CHANGE          (1uL << 1)    // Connect Status Change
#define PORT_ENABLE                     (1uL << 2)    // Port Enabled
#define PORT_ENABLE_CHANGE              (1uL << 3)    // Port Enable Change
#define PORT_OVER_CURRENT               (1uL << 4)    // Over-current Active
#define PORT_OVER_CURRENT_CHANGE        (1uL << 5)    // Over-current Change
#define PORT_FPR                        (1uL << 6)    // Force Port Resume
#define PORT_SUSPEND                    (1uL << 7)    // Suspend
#define PORT_RESET                      (1uL << 8)    // Port Reset
#define PORT_LS_MASK                    (3uL << 10)   // Line Status
#define PORT_LS_SHIFT                   10
#define PORT_POWER                      (1uL << 12)   // Port Power
#define PORT_OWNER                      (1uL << 13)   // Port Owner
#define PORT_IC_MASK                    (3uL << 14)   // Port Indicator Control
#define PORT_IC_SHIFT                   14
#define PORT_TC_MASK                    (15uL << 16)  // Port Test Control
#define PORT_TC_SHIFT                   16
#define PORT_WKCNNT_E                   (1uL << 20)   // Wake on Connect Enable
#define PORT_WKDSCNNT_E                 (1uL << 21)   // Wake on Disconnect Enable
#define PORT_WKOC_E                     (1uL << 22)   // Wake on Over-current Enable
#define PORT_RWC                        (PORT_CONNECTION_CHANGE | PORT_ENABLE_CHANGE | PORT_OVER_CURRENT_CHANGE)

// ------------------------------------------------------------------------------------------------
// Transfer Descriptor

typedef struct EhciTD
{
    volatile uint32_t link;
    volatile uint32_t altLink;
    volatile uint32_t token;
    volatile uint32_t buffer[5];
    volatile uint32_t extBuffer[5];

    // internal fields
    uint32_t tdNext;
    uint32_t active;
    uint8_t pad[4];
} EhciTD;

// TD Link Pointer
#define PTR_TERMINATE                   (1uL << 0)

#define PTR_TYPE_MASK                   (3uL << 1)
#define PTR_ITD                         (0uL << 1)
#define PTR_QH                          (1uL << 1)
#define PTR_SITD                        (2uL << 1)
#define PTR_FSTN                        (3uL << 1)

// TD Token
#define TD_TOK_PING                     (1uL << 0)    // Ping State
#define TD_TOK_STS                      (1uL << 1)    // Split Transaction State
#define TD_TOK_MMF                      (1uL << 2)    // Missed Micro-Frame
#define TD_TOK_XACT                     (1uL << 3)    // Transaction Error
#define TD_TOK_BABBLE                   (1uL << 4)    // Babble Detected
#define TD_TOK_DATABUFFER               (1uL << 5)    // Data Buffer Error
#define TD_TOK_HALTED                   (1uL << 6)    // Halted
#define TD_TOK_ACTIVE                   (1uL << 7)    // Active
#define TD_TOK_PID_MASK                 (3uL << 8)    // PID Code
#define TD_TOK_PID_SHIFT                8
#define TD_TOK_CERR_MASK                (3uL << 10)   // Error Counter
#define TD_TOK_CERR_SHIFT               10
#define TD_TOK_C_PAGE_MASK              (7uL << 12)   // Current Page
#define TD_TOK_C_PAGE_SHIFT             12
#define TD_TOK_IOC                      (1uL << 15)   // Interrupt on Complete
#define TD_TOK_LEN_MASK                 0x7fff0000uL  // Total Bytes to Transfer
#define TD_TOK_LEN_SHIFT                16
#define TD_TOK_D                        (1uL << 31)   // Data Toggle
#define TD_TOK_D_SHIFT                  31

#define USB_PACKET_OUT                  0           // token 0xe1
#define USB_PACKET_IN                   1           // token 0x69
#define USB_PACKET_SETUP                2           // token 0x2d

// ------------------------------------------------------------------------------------------------
// Queue Head

typedef struct EhciQH
{
    uint32_t qhlp;       // Queue Head Horizontal Link Pointer
    uint32_t ch;         // Endpoint Characteristics
    uint32_t caps;       // Endpoint Capabilities
    volatile uint32_t curLink;

    // matches a transfer descriptor
    volatile uint32_t nextLink;
    volatile uint32_t altLink;
    volatile uint32_t token;
    volatile uint32_t buffer[5];
    volatile uint32_t extBuffer[5];

    // internal fields
//    UsbTransfer *transfer;
//    Link qhLink;
    uint32_t tdHead;
    uint32_t active;
    uint8_t pad[20];
} EhciQH;

// Endpoint Characteristics
#define QH_CH_DEVADDR_MASK              0x0000007fuL  // Device Address
#define QH_CH_INACTIVE                  0x00000080uL  // Inactive on Next Transaction
#define QH_CH_ENDP_MASK                 0x00000f00uL  // Endpoint Number
#define QH_CH_ENDP_SHIFT                8
#define QH_CH_EPS_MASK                  0x00003000uL  // Endpoint Speed
#define QH_CH_EPS_SHIFT                 12
#define QH_CH_DTC                       0x00004000uL  // Data Toggle Control
#define QH_CH_H                         0x00008000uL  // Head of Reclamation List Flag
#define QH_CH_MPL_MASK                  0x07ff0000uL  // Maximum Packet Length
#define QH_CH_MPL_SHIFT                 16
#define QH_CH_CONTROL                   0x08000000uL  // Control Endpoint Flag
#define QH_CH_NAK_RL_MASK               0xf0000000uL  // Nak Count Reload
#define QH_CH_NAK_RL_SHIFT              28

// Endpoint Capabilities
#define QH_CAP_INT_SCHED_SHIFT          0
#define QH_CAP_INT_SCHED_MASK           0x000000ffuL  // Interrupt Schedule Mask
#define QH_CAP_SPLIT_C_SHIFT            8
#define QH_CAP_SPLIT_C_MASK             0x0000ff00uL  // Split Completion Mask
#define QH_CAP_HUB_ADDR_SHIFT           16
#define QH_CAP_HUB_ADDR_MASK            0x007f0000uL  // Hub Address
#define QH_CAP_PORT_MASK                0x3f800000uL  // Port Number
#define QH_CAP_PORT_SHIFT               23
#define QH_CAP_MULT_MASK                0xc0000000uL  // High-Bandwidth Pipe Multiplier
#define QH_CAP_MULT_SHIFT               30

// ------------------------------------------------------------------------------------------------
// Device

typedef struct EhciController
{
    EhciCapRegs *capRegs;
    EhciOpRegs *opRegs;
    uint32_t *frameList;
    EhciQH *qhPool;
    EhciTD *tdPool;
    EhciQH *asyncQH;
    EhciQH *periodicQH;
} EhciController;


void USBH_OHCI_IRQHandler(void)
{
	TP();
}

void USBH_EHCI_IRQHandler(void)
{
	TP();
}
// USB EHCI controller
void board_ehci_initialize(void)
{
	PRINTF("board_ehci_initialize start.\n");

	RCC->MP_AHB6ENSETR = RCC_MC_AHB6ENSETR_USBHEN;
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6LPENSETR = RCC_MC_AHB6LPENSETR_USBHLPEN;
	(void) RCC->MP_AHB6LPENSETR;
	if (0)
	{
		/* SYSCFG clock enable */
		RCC->MP_APB3ENSETR = RCC_MC_APB3ENSETR_SYSCFGEN;
		(void) RCC->MP_APB3ENSETR;
		RCC->MP_APB3LPENSETR = RCC_MC_APB3LPENSETR_SYSCFGLPEN;
		(void) RCC->MP_APB3LPENSETR;
		/*
		 * Interconnect update : select master using the port 1.
		 * MCU interconnect (USBH) = AXI_M1, AXI_M2.
		 */
//		SYSCFG->ICNR |= SYSCFG_ICNR_AXI_M1;
//		(void) SYSCFG->ICNR;
//		SYSCFG->ICNR |= SYSCFG_ICNR_AXI_M2;
//		(void) SYSCFG->ICNR;
	}

	// https://github.com/pdoane/osdev/blob/master/usb/ehci.c

	// USBH_EHCI_HCICAPLENGTH == USB1_EHCI->HCCAPBASE
	// USBH_EHCI_HCSPARAMS == USB1_EHCI->HCSPARAMS
	// USBH_EHCI_HCCPARAMS == USB1_EHCI->HCCPARAMS
	// OHCI BASE = USB1HSFSP2_BASE	(MPU_AHB6_PERIPH_BASE + 0xC000)
	// EHCI BASE = USB1HSFSP1_BASE	(MPU_AHB6_PERIPH_BASE + 0xD000)

	PRINTF("board_ehci_initialize: HCCAPBASE=%08lX\n", (unsigned long) USB1_EHCI->HCCAPBASE);
	PRINTF("board_ehci_initialize: HCSPARAMS=%08lX\n", (unsigned long) USB1_EHCI->HCSPARAMS);
	PRINTF("board_ehci_initialize: HCCPARAMS=%08lX\n", (unsigned long) USB1_EHCI->HCCPARAMS);
    // Check extended capabilities
    uint_fast32_t eecp = (USB1_EHCI->HCCPARAMS & HCCPARAMS_EECP_MASK) >> HCCPARAMS_EECP_SHIFT;
    if (eecp >= 0x40)
    {
    	PRINTF("board_ehci_initialize: eecp=%08lX\n", (unsigned long) eecp);
       // Disable BIOS legacy support
//        uint legsup = PciRead32(id, eecp + USBLEGSUP);
//
//        if (legsup & USBLEGSUP_HC_BIOS)
//        {
//            PciWrite32(id, eecp + USBLEGSUP, legsup | USBLEGSUP_HC_OS);
//            for (;;)
//            {
//                legsup = PciRead32(id, eecp + USBLEGSUP);
//                if (~legsup & USBLEGSUP_HC_BIOS && legsup & USBLEGSUP_HC_OS)
//                {
//                    break;
//                }
//            }
//        }
    }


//	USBH_EHCI_IRQn
	//USBH_OHCI_IRQn                   = 106,    /*!< USB OHCI global interrupt                                            */
	//USBH_EHCI_IRQn                   = 107,    /*!< USB EHCI global interrupt                                            */
	arm_hardware_set_handler_system(USBH_OHCI_IRQn, USBH_OHCI_IRQHandler);
	arm_hardware_set_handler_system(USBH_EHCI_IRQn, USBH_EHCI_IRQHandler);

	PRINTF("board_ehci_initialize done.\n");
}

#endif /* WITHEHCIHW */


