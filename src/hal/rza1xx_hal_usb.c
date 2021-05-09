/*
 * rza1xx_hal_usb.c
 *
 *  Created on: May 5, 2021
 *      Author: gena
 */
/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//


#include "hardware.h"

#if CPUSTYLE_R7S721

#include "board.h"
#include "audio.h"
#include "formats.h"
#include "gpio.h"

#include "src/gui/gui.h"
#include "src/display/display.h"

#include <string.h>

#include "src/usb/usb200.h"
#include "src/usb/usbch9.h"

#include "usb_device.h"
#include "usbd_core.h"
#include "usbh_core.h"
#include "usbh_def.h"

#include "rza1xx_hal.h"
#include "rza1xx_hal_usb.h"

// на RENESAS для работы с изохронными ендпоинтами используется DMA
//#define WITHDMAHW_UACIN 1		// при этой опции после пересоединения USB кабеля отваливается поток IN
//#define WITHDMAHW_UACOUT 1	// Устойчиво работает - но пропуск пакетов

uint_fast8_t
usbd_epaddr2pipe(uint_fast8_t ep_addr)
{
	switch (ep_addr)
	{
	default:
		ASSERT(0);
		return 0;
	case 0x00: return 0;
	case 0x80: return 0;
#if WITHUSBUACOUT
	case USBD_EP_AUDIO_OUT:	return HARDWARE_USBD_PIPE_ISOC_OUT;
#endif /* WITHUSBUACOUT */
#if WITHUSBUACIN
	case USBD_EP_AUDIO_IN:	return HARDWARE_USBD_PIPE_ISOC_IN;
#endif /* WITHUSBUACIN */
#if WITHUSBCDCACM

#if WITHUSBCDCACMINTSHARING
	case USBD_EP_CDCACM_INTSHARED:				return HARDWARE_USBD_PIPE_CDC_INTSHARED;	// Shared EP
#else /* WITHUSBCDCACMINTSHARING */
	case USBD_CDCACM_EP(USBD_EP_CDCACM_INT, 0):	return HARDWARE_USBD_PIPE_CDC_INT;
	case USBD_CDCACM_EP(USBD_EP_CDCACM_INT, 1):	return HARDWARE_USBD_PIPE_CDC_INTb;
#endif /* WITHUSBCDCACMINTSHARING */
	case USBD_CDCACM_EP(USBD_EP_CDCACM_OUT, 0):	return HARDWARE_USBD_PIPE_CDC_OUT;
	case USBD_CDCACM_EP(USBD_EP_CDCACM_IN, 0):	return HARDWARE_USBD_PIPE_CDC_IN;
	case USBD_CDCACM_EP(USBD_EP_CDCACM_OUT, 1):	return HARDWARE_USBD_PIPE_CDC_OUTb;
	case USBD_CDCACM_EP(USBD_EP_CDCACM_IN, 1):	return HARDWARE_USBD_PIPE_CDC_INb;
#endif /* WITHUSBCDCACM */
#if WITHUSBCDCEEM
	case USBD_EP_CDCEEM_OUT:	return HARDWARE_USBD_PIPE_CDCEEM_OUT;
	case USBD_EP_CDCEEM_IN:		return HARDWARE_USBD_PIPE_CDCEEM_IN;
#endif /* WITHUSBCDCEEM */
#if WITHUSBRNDIS
	case USBD_EP_RNDIS_OUT:		return HARDWARE_USBD_PIPE_RNDIS_OUT;
	case USBD_EP_RNDIS_IN:		return HARDWARE_USBD_PIPE_RNDIS_IN;
	case USBD_EP_RNDIS_INT:		return HARDWARE_USBD_PIPE_RNDIS_INT;
#endif /* WITHUSBRNDIS */
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

#if WITHUSBCDCACMINTSHARING
#else /* WITHUSBCDCACMINTSHARING */
#endif /* WITHUSBCDCACMINTSHARING */

// DCP (PIPE0) как аргумент недопустим
static uint_fast8_t
usbd_pipe2epaddr(uint_fast8_t pipe)
{
	switch (pipe)
	{
	default:
		ASSERT(0);
		return 0;
#if WITHUSBUACOUT
	case HARDWARE_USBD_PIPE_ISOC_OUT: return USBD_EP_AUDIO_OUT;
#endif /* WITHUSBUACOUT */
#if WITHUSBUACIN
	case HARDWARE_USBD_PIPE_ISOC_IN: return USBD_EP_AUDIO_IN;
#endif /* WITHUSBUACIN */
#if WITHUSBCDCACM

#if WITHUSBCDCACMINTSHARING
	case HARDWARE_USBD_PIPE_CDC_INTSHARED: return USBD_EP_CDCACM_INTSHARED;
#else /* WITHUSBCDCACMINTSHARING */
	case HARDWARE_USBD_PIPE_CDC_INT: return USBD_CDCACM_EP(USBD_EP_CDCACM_INT, 0);
	case HARDWARE_USBD_PIPE_CDC_INTb: return USBD_CDCACM_EP(USBD_EP_CDCACM_INT, 1);
#endif /* WITHUSBCDCACMINTSHARING */

	case HARDWARE_USBD_PIPE_CDC_OUT: return USBD_CDCACM_EP(USBD_EP_CDCACM_OUT, 0);
	case HARDWARE_USBD_PIPE_CDC_IN: return USBD_CDCACM_EP(USBD_EP_CDCACM_IN, 0);

	case HARDWARE_USBD_PIPE_CDC_OUTb: return USBD_CDCACM_EP(USBD_EP_CDCACM_OUT, 1);
	case HARDWARE_USBD_PIPE_CDC_INb: return USBD_CDCACM_EP(USBD_EP_CDCACM_IN, 1);

#endif /* WITHUSBCDCACM */
#if WITHUSBCDCEEM
	case HARDWARE_USBD_PIPE_CDCEEM_OUT: return USBD_EP_CDCEEM_OUT;
	case HARDWARE_USBD_PIPE_CDCEEM_IN: return USBD_EP_CDCEEM_IN;
#endif /* WITHUSBCDCEEM */
#if WITHUSBRNDIS
	case HARDWARE_USBD_PIPE_RNDIS_OUT: return USBD_EP_RNDIS_OUT;
	case HARDWARE_USBD_PIPE_RNDIS_IN: return USBD_EP_RNDIS_IN;
	case HARDWARE_USBD_PIPE_RNDIS_INT: return USBD_EP_RNDIS_INT;
#endif /* WITHUSBRNDIS */
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

static RAMBIGDTCM USBALIGN_BEGIN uint8_t uacoutbuff0 [UACOUT_AUDIO48_DATASIZE] USBALIGN_END;
static RAMBIGDTCM USBALIGN_BEGIN uint8_t uacoutbuff1 [UACOUT_AUDIO48_DATASIZE] USBALIGN_END;

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

static uint_fast8_t usbd_wait_fifo(
		PCD_TypeDef * const USBx,
		uint_fast8_t pipe, 	// expected pipe
		uint_fast8_t isel,	// expected isel: 1: Writing to the DCP buffer memory is selected
		unsigned waitcnt
		)
{
	volatile uint16_t * const PIPEnCTR = get_pipectr_reg(USBx, pipe);
	while (
			(* PIPEnCTR & USB_DCPCTR_BSTS) == 0 ||
			(USBx->CFIFOSEL & USB_CFIFOSEL_ISEL_) != (isel << USB_CFIFOSEL_ISEL_SHIFT_) ||
			(USBx->CFIFOSEL & USB_CFIFOSEL_CURPIPE) != (pipe << USB_CFIFOSEL_CURPIPE_SHIFT) ||
			(USBx->CFIFOCTR & USB_CFIFOCTR_FRDY) == 0)	// FRDY
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
		((pipe << USB_CFIFOSEL_CURPIPE_SHIFT) & USB_CFIFOSEL_CURPIPE) |	// CURPIPE 0000: DCP
		((mbw << USB_CFIFOSEL_MBW_SHIFT) & USB_CFIFOSEL_MBW) |	// MBW 00: 8-bit width
		0;
	if (usbd_wait_fifo(USBx, pipe, 0, USBD_FRDY_COUNT_READ))
	{
		PRINTF(PSTR("USB_ReadPacketNec: usbd_wait_fifo error, pipe=%d, USBx->CFIFOSEL=%08lX\n"), (int) pipe, (unsigned long) USBx->CFIFOSEL);
		return 1;	// error
	}

	ASSERT(size == 0 || data != NULL);
	unsigned dtln = (USBx->CFIFOCTR & USB_CFIFOCTR_DTLN) >> USB_CFIFOCTR_DTLN_SHIFT;
	size = MIN(size, dtln);
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
		((pipe << USB_CFIFOSEL_CURPIPE_SHIFT) & USB_CFIFOSEL_CURPIPE) |	// CURPIPE 0000: DCP
		(0x01 << USB_CFIFOSEL_ISEL_SHIFT_) * (pipe == 0) |	// ISEL 1: Writing to the buffer memory is selected (for DCP)
		0;

	USBx->CFIFOSEL = cfifosel | (0x02 << USB_CFIFOSEL_MBW_SHIFT);	// MBW 10: 32-bit width
	if (usbd_wait_fifo(USBx, pipe, (pipe == 0), USBD_FRDY_COUNT_WRITE))
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


/**
  * @brief  Set the USB Device address.
  * @param  hpcd PCD handle
  * @param  address new device address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address)
{
  __HAL_LOCK(hpcd);
  hpcd->USB_Address = address;
  (void)USB_SetDevAddress(hpcd->Instance, address);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}
/**
  * @brief  Open and configure an endpoint.
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  ep_mps endpoint max packet size
  * @param  ep_type endpoint type
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                  uint16_t ep_mps, uint8_t ep_type)
{
  HAL_StatusTypeDef  ret = HAL_OK;
  PCD_EPTypeDef *ep;

  if ((ep_addr & 0x80U) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 1U;
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 0U;
  }

  ep->num = ep_addr & EP_ADDR_MSK;
  ep->maxpacket = ep_mps;
  ep->type = ep_type;

//  if (ep->is_in != 0U)
//  {
//    /* Assign a Tx FIFO */
//    ep->tx_fifo_num = ep->num;
//  }
  /* Set initial data PID. */
  if (ep_type == EP_TYPE_BULK)
  {
    ep->data_pid_start = 0U;
  }

  __HAL_LOCK(hpcd);
  (void)USB_ActivateEndpoint(hpcd->Instance, ep);
  __HAL_UNLOCK(hpcd);

  return ret;
}

/**
  * @brief  Deactivate an endpoint.
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  PCD_EPTypeDef *ep;

  if ((ep_addr & 0x80U) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 1U;
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 0U;
  }
  ep->num   = ep_addr & EP_ADDR_MSK;

  __HAL_LOCK(hpcd);
  (void)USB_DeactivateEndpoint(hpcd->Instance, ep);
  __HAL_UNLOCK(hpcd);
  return HAL_OK;
}


/**
  * @brief  Receive an amount of data.
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  pBuf pointer to the reception buffer
  * @param  len amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
  PCD_EPTypeDef *ep;

  ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];

  /*setup and start the Xfer */
  ep->xfer_buff = pBuf;
  ep->xfer_len = len;
  ep->xfer_count = 0U;
  ep->is_in = 0U;
  ep->num = ep_addr & EP_ADDR_MSK;

  if (hpcd->Init.dma_enable == 1U)
  {
	if (pBuf != NULL && len != 0)
		arm_hardware_flush_invalidate((uintptr_t) pBuf, len);
    ep->dma_addr = (uint32_t)pBuf;
  }

  if ((ep_addr & EP_ADDR_MSK) == 0U)
  {
    (void)USB_EP0StartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
  }
  else
  {
    (void)USB_EPStartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
  }

  return HAL_OK;
}
/**
  * @brief  Send an amount of data
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  pBuf pointer to the transmission buffer
  * @param  len amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, const uint8_t *pBuf, uint32_t len)
{
  PCD_EPTypeDef *ep;

  ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];

  /*setup and start the Xfer */
  ep->xfer_buff = (uint8_t *) pBuf;
  ep->xfer_len = len;
  ep->xfer_count = 0U;
  ep->is_in = 1U;
  ep->num = ep_addr & EP_ADDR_MSK;

  if (hpcd->Init.dma_enable == 1U)
  {
		if (pBuf != NULL && len != 0)
		{
			ASSERT(((uintptr_t) pBuf % DCACHEROWSIZE) == 0);
			arm_hardware_flush((uintptr_t) pBuf, len);
		}
    ep->dma_addr = (uint32_t)pBuf;
  }

  if ((ep_addr & EP_ADDR_MSK) == 0U)
  {
    (void)USB_EP0StartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
  }
  else
  {
    (void)USB_EPStartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
  }

  return HAL_OK;
}

/**
  * @brief  Set a STALL condition over an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  PCD_EPTypeDef *ep;

  if (((uint32_t)ep_addr & EP_ADDR_MSK) > hpcd->Init.dev_endpoints)
  {
    return HAL_ERROR;
  }

  if ((0x80U & ep_addr) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 1U;
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr];
    ep->is_in = 0U;
  }

  ep->is_stall = 1U;
  ep->num = ep_addr & EP_ADDR_MSK;

  __HAL_LOCK(hpcd);

  (void)USB_EPSetStall(hpcd->Instance, ep);

  if ((ep_addr & EP_ADDR_MSK) == 0U)
  {
    (void)USB_EP0_OutStart(hpcd->Instance, (uint8_t)hpcd->Init.dma_enable, (uint8_t *)hpcd->Setup);
  }

  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

/**
  * @brief  Clear a STALL condition over in an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  PCD_EPTypeDef *ep;

  if (((uint32_t)ep_addr & 0x0FU) > hpcd->Init.dev_endpoints)
  {
    return HAL_ERROR;
  }

  if ((0x80U & ep_addr) == 0x80U)
  {
    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 1U;
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
    ep->is_in = 0U;
  }

  ep->is_stall = 0U;
  ep->num = ep_addr & EP_ADDR_MSK;

  __HAL_LOCK(hpcd);
  (void)USB_EPClearStall(hpcd->Instance, ep);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

/**
  * @brief  Flush an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  __HAL_LOCK(hpcd);

  if ((ep_addr & 0x80U) == 0x80U)
  {
    (void)USB_FlushTxFifo(hpcd->Instance, (uint32_t)ep_addr & EP_ADDR_MSK);
  }
  else
  {
    (void)USB_FlushRxFifo(hpcd->Instance);
  }

  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

uint32_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
  return hpcd->OUT_ep[ep_addr & EP_ADDR_MSK].xfer_count;
}

static void usb_save_request(USB_OTG_GlobalTypeDef * USBx, USBD_SetupReqTypedef *req)
{
	const uint_fast16_t usbreq = USBx->USBREQ;

	req->bmRequest     = LO_BYTE(usbreq); //(pdata [0] >> 0) & 0x00FF;
	req->bRequest      = HI_BYTE(usbreq); //(pdata [0] >> 8) & 0x00FF;
	req->wValue        = USBx->USBVAL; //(pdata [0] >> 16) & 0xFFFF;
	req->wIndex        = USBx->USBINDX; //(pdata [1] >> 0) & 0xFFFF;
	req->wLength       = USBx->USBLENG; //(pdata [1] >> 16) & 0xFFFF;

#if 0
	PRINTF("%s: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n",
			__func__,
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

#if defined (WITHUSBHW_DEVICE)
/*
	r7s721_usbi0_handler trapped
	 BRDYSTS=0x00000000
	 INTSTS0=0x0000F899

	 		(hpcd->Init.Sof_enable != DISABLE) * USB_INTENB0_SOFE |	// SOFE	1: Frame Number Update Interrupt Enable
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
		// device
		uint_fast8_t i;
		//PRINTF(PSTR("HAL_PCD_IRQHandler trapped - BRDY, BRDYSTS=0x%04X\n"), USBx->BRDYSTS);
		const uint_fast16_t brdysts = USBx->BRDYSTS & USBx->BRDYENB;	// BRDY Interrupt Status Register
		USBx->BRDYSTS = ~ brdysts;	// 2. When BRDYM is 0, clearing this bit should be done before accessing the FIFO.

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
				hpcd->Init.speed = PCD_SPEED_HIGH;
				//hpcd->Init.ep0_mps = USB_OTG_MAX_EP0_SIZE; //USB_OTG_HS_MAX_PACKET_SIZE;
				/*
				hpcd->Instance->GUSBCFG = (hpcd->Instance->GUSBCFG & ~ USB_OTG_GUSBCFG_TRDT) |
				(uint32_t)((USBD_HS_TRDT_VALUE << USB_OTG_GUSBCFG_TRDT_Pos) & USB_OTG_GUSBCFG_TRDT) |
				0;
				*/
			}
			else
			{
				hpcd->Init.speed = PCD_SPEED_FULL;
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
		USBx->DCPCTR &= ~ USB_DCPCTR_PID;
		USBx->DCPCTR |= DEVDRV_USBF_PID_BUF;	// CCPL
		USBx->DCPCTR |= USB_DCPCTR_CCPL;	// CCPL
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

	//PRINTF(PSTR("HAL_HCD_IRQHandler trapped, intsts0=%04X, intsts0msk=%04X, intsts0=%04X, intsts1=%04X\n"), intsts0, intsts0msk, intsts1, intsts1msk);
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
		// host
		uint_fast8_t i;
		//PRINTF(PSTR("1 HAL_HCD_IRQHandler trapped - BRDY, BRDYSTS=0x%04X\n"), USBx->BRDYSTS);
		const uint_fast16_t brdysts = USBx->BRDYSTS & USBx->BRDYENB;	// BRDY Interrupt Status Register
		USBx->BRDYSTS = ~ brdysts;	// 2. When BRDYM is 0, clearing this bit should be done before accessing the FIFO.
		if ((brdysts & (1U << 0)) != 0)		// PIPE0 - DCP
		{
			const uint_fast8_t pipe = 0;
			USBH_HandleTypeDef * const phost = hhcd->pData;
			HCD_HCTypeDef * const hc = & hhcd->hc [phost->Control.pipe_in];
			//HCD_HCTypeDef * const hc = & hhcd->hc [0];
		  	unsigned bcnt;
		  	if (USB_ReadPacketNec(USBx, pipe, hc->xfer_buff, hc->xfer_len - hc->xfer_count, & bcnt) == 0)
		  	{
		  		if (bcnt == 0)
		  		{
		  			//PRINTF("NO DATA\n");
		  		}
		  		else
		  		{
			  		printhex((uintptr_t) hc->xfer_buff, hc->xfer_buff, bcnt);	// DEBUG
			  		//hc->xfer_buff += bcnt;
			  		//hc->xfer_count += bcnt;
		  		}
				//HAL_PCD_DataOutStageCallback(hpcd, ep->num);	// start next transfer
		  		hc->toggle_in ^= 1;
		  		hc->state = HC_XFRC;
		  		hc->urb_state  = URB_DONE;
		  	}
		  	else
		  	{
		  		TP();
		  		//control_stall(pdev);
		  	}
		}

#if 0

		for (i = 1; i < 16; ++ i)
		{
			const uint_fast8_t pipe = i;
			if ((brdysts & (1U << pipe)) != 0)
			{
				const uint_fast8_t epnt = usbd_pipe2epaddr(pipe);
				if ((epnt & 0x80) != 0)
				{
					HCD_HCTypeDef * const hc = & hhcd->hc [0];
					ASSERT(hc->xfer_len == 0 || hc->xfer_buff != NULL);
					hc->xfer_buff += hc->maxpacket;
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

		USBH_HandleTypeDef * const phost = hhcd->pData;
		HCD_HCTypeDef * const hc = & hhcd->hc [phost->Control.pipe_out];

	    hc->state = HC_XFRC;
	    hc->ErrCnt = 1;
	    hc->toggle_in ^= 1;
	    hc->urb_state  = URB_DONE;
	}
	if ((intsts1msk & USB_INTSTS1_SACK) != 0)	// SACK
	{
		USBx->INTSTS1 = (uint16_t) ~ USB_INTSTS1_SACK;
		PRINTF(PSTR("HAL_HCD_IRQHandler trapped - SACK\n"));
		//HAL_HCD_Connect_Callback(hhcd);
		//int err = USB_WritePacketNec(USBx, 0, NULL, 0);	// pipe=0: DCP
		//ASSERT(err == 0);
		//HAL_HCD_Connect_Callback(hhcd);

		USBH_HandleTypeDef * const phost = hhcd->pData;
		HCD_HCTypeDef * const hc = & hhcd->hc [phost->Control.pipe_out];

	    hc->state = HC_XFRC;
	    hc->ErrCnt = 0;
	    hc->toggle_in ^= 1;
	    hc->urb_state  = URB_DONE;
	}
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

uint_fast8_t USB_GetHostSpeedReady(USB_OTG_GlobalTypeDef *USBx)
{
	// 1xx: Reset handshake in progress
	return (((USBx->DVSTCTR0 & USB_DVSTCTR0_RHST) >> USB_DVSTCTR0_RHST_SHIFT) & 0x04) == 0;
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
	// 1xx: Reset handshake in progress
	while (((USBx->DVSTCTR0 & USB_DVSTCTR0_RHST) >> USB_DVSTCTR0_RHST_SHIFT) & 0x04)
		dbg_putchar('^');

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
  * @brief  USB_GetDevSpeed :Return the Dev Speed
  * @param  USBx : Selected device
  * @retval speed : device speed
  *          This parameter can be one of these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  *            @arg USB_OTG_SPEED_LOW: Low speed mode
  */
uint8_t USB_GetDevSpeed(USB_OTG_GlobalTypeDef *USBx)
{
	// 100: Reset handshake in progress
	while (((USBx->DVSTCTR0 & USB_DVSTCTR0_RHST) >> USB_DVSTCTR0_RHST_SHIFT) == 0x04)
		dbg_putchar('^');

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
  * @brief  USB_DriveVbus : activate or de-activate vbus
  * @param  state : VBUS state
  *          This parameter can be one of these values:
  *           0 : VBUS Active
  *           1 : VBUS Inactive
  * @retval HAL status
*/
HAL_StatusTypeDef USB_DriveVbus(USB_OTG_GlobalTypeDef *USBx, uint8_t state)
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
		USBx->SYSCFG0 &= ~ USB_SYSCFG_DCFM;	// DCFM 0: Devide controller mode is selected
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
  * @brief  USB_EP0StartXfer : setup and starts a transfer over the EP  0
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @param  dma: USB dma enabled or disabled
  *          This parameter can be one of these values:
  *           0 : DMA feature not used
  *           1 : DMA feature used
  * @retval HAL status
  */

HAL_StatusTypeDef USB_EP0StartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep, uint8_t dma)
{
	const uint_fast8_t pipe = 0;	// DCP
	ASSERT(dma == 0);
	//PRINTF(PSTR("USB_EP0StartXfer\n"));
	if (ep->is_in == 1)
	{
		//PRINTF("%s: IN direction, ep->xfer_len=%d, ep->maxpacket=%d\n", __func__, (int) ep->xfer_len, (int) ep->maxpacket);

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
		//PRINTF("%s: OUT direction\n", __func__);
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
HAL_StatusTypeDef USB_EPStartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep, uint8_t dma)
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

HAL_StatusTypeDef USB_HC_StartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_HCTypeDef *hc, uint8_t dma)
{
	PRINTF("USB_HC_StartXfer, xfer_buff=%p, ep_is_in=%d, ch_num=%d, ep_num=%d, xfer_len=%d\n", hc->xfer_buff, (int) hc->ep_is_in, (int) hc->ch_num, (int) hc->ep_num, (int) hc->xfer_len);
	ASSERT(dma == 0);
	uint8_t  is_oddframe;
	uint16_t num_packets = 0;
	uint16_t max_hc_pkt_count = 256;
	uint32_t tmpreg = 0;

	const uint_fast8_t pipe = 0;
	volatile uint16_t * const PIPEnCTR = get_pipectr_reg(USBx, pipe);
	volatile uint16_t * const PIPEnTRE = get_pipetre_reg(USBx, pipe);
	volatile uint16_t * const PIPEnTRN = get_pipetrn_reg(USBx, pipe);

	if (hc->speed == USB_OTG_SPEED_HIGH)
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
		  // foom device to host (read)
		  ////* tmpreg |= USB_OTG_HCCHAR_EPDIR;
	  }
	  else
	  {
		  // foom host to device (write)
		  ////* tmpreg &= ~USB_OTG_HCCHAR_EPDIR;
	  }

	  ////* tmpreg |= USB_OTG_HCCHAR_CHENA;
	  ////* USBx_HC(hc->ch_num)->HCCHAR = tmpreg;

	  if (dma == 0) /* Slave mode */
	  {
		////uint16_t len_words = 0;
	    if ((hc->ep_is_in == 0) && (hc->xfer_len > 0))
	    {
			  // foom host to device (write)
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

	      if (pipe == 0 && hc->xfer_len >= 8)
	      {
	    	  const unsigned devsel = 0x00;

	    		USB_Setup_TypeDef * const pSetup = (USB_Setup_TypeDef *) hc->xfer_buff;

	    		USBx->DCPCTR |= USB_DCPCTR_SUREQCLR;
	    		ASSERT(hc->xfer_len >= 8);
	    		ASSERT((USBx->DCPCTR & USB_DCPCTR_SUREQ) == 0);

				PRINTF("USB_HC_StartXfer: DCPMAXP=%08lX, dev_addr=%d, bmRequestType=%02X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n",
						(unsigned long) USBx->DCPMAXP,
						(int) hc->dev_addr,
						pSetup->b.bmRequestType, pSetup->b.bRequest, pSetup->b.wValue.w, pSetup->b.wIndex.w, pSetup->b.wLength.w);

				USBx->USBREQ =
						((pSetup->b.bRequest << USB_USBREQ_BREQUEST_SHIFT) & USB_USBREQ_BREQUEST) |
						((pSetup->b.bmRequestType << USB_USBREQ_BMREQUESTTYPE_SHIFT) & USB_USBREQ_BMREQUESTTYPE);
				USBx->USBVAL =
						(pSetup->b.wValue.w << USB_USBVAL_SHIFT) & USB_USBVAL;
				USBx->USBINDX =
						(pSetup->b.wIndex.w << USB_USBINDX_SHIFT) & USB_USBINDX;
				USBx->USBLENG =
						(pSetup->b.wLength.w << USB_USBLENG_SHIFT) & USB_USBLENG;

				USBx->DCPMAXP = (USBx->DCPMAXP & ~ (USB_DCPMAXP_DEVSEL | USB_DCPMAXP_MXPS)) |
						((devsel << USB_DCPMAXP_DEVSEL_SHIFT) & USB_DCPMAXP_DEVSEL) |	// DEVADD0 used
						((64 << USB_DCPMAXP_MXPS_SHIFT) & USB_DCPMAXP_MXPS) |
						0;

				// не влияет
				USBx->DCPCTR |= USB_DCPCTR_SQCLR;	// DATA0 as answer
				//USBx->DCPCTR |= USB_DCPCTR_SQSET;	// DATA1 as answer

				// влияет
				if (hc->xfer_len > 8)
				{
					// direction of data stage and status stage for control transfers
					// 1: Data transmitting direction
					// 0: Data receiving direction
					USBx->DCPCFG = USB_DCPCFG_DIR;
				}
				else
				{
					//USBx->DCPCFG = 0x80; //USB_DCPCFG_SHTNAK;	// Pipe disabled at the end of transfer
					USBx->DCPCFG = 0x0000;
				}
				USB_WritePacketNec(USBx, pipe, NULL, 0);	// ?

				USBx->DCPCTR |= USB_DCPCTR_SUREQ;

				//hc->xfer_buff += 8;		// size of USB_Setup_TypeDef
				//PRINTF("USB_HC_StartXfer: DCPMAXP=%08lX\n", (unsigned long) USBx->DCPMAXP);
	      }
	      else
	      {
			  /* Write packet into the Tx FIFO. */
			  //USB_WritePacket(USBx, hc->xfer_buff, hc->ch_num, hc->xfer_len, 0);
			  TP();
			  USB_WritePacketNec(USBx, pipe, hc->xfer_buff, hc->xfer_len);
				//hc->xfer_buff += hc->xfer_len;		//
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
HAL_StatusTypeDef USB_HC_Halt(USB_OTG_GlobalTypeDef *USBx, uint8_t hc_num)
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

////+++sack: не влияет
////	USBx->INTSTS1 = (uint16_t) ~ USB_INTSTS1_SIGN;
////	USBx->INTSTS1 = (uint16_t) ~ USB_INTSTS1_SACK;
//	USBx->NRDYSTS = (uint16_t) ~ (1uL << pipe);
//	USBx->BEMPSTS = (uint16_t) ~ (1uL << pipe);
//	USBx->BRDYSTS = (uint16_t) ~ (1uL << pipe);
////---sack: не влияет

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
	(void) * PIPEnCTR;
	while ((* PIPEnCTR & (USB_PIPEnCTR_1_5_PBUSY | USB_PIPEnCTR_1_5_CSSTS)) != 0)	// PBUSY, CSSTS
		;

	* PIPEnCTR = USB_PIPEnCTR_1_5_SQCLR;

	/* тут pipe не готова принимать данные */

	* PIPEnCTR = 0x0003;	// NAK->STALL
	(void) * PIPEnCTR;
	* PIPEnCTR = 0x0002;	// NAK->STALL
	(void) * PIPEnCTR;
	* PIPEnCTR = 0x0001;	// STALL->BUF
	(void) * PIPEnCTR;
	/* тут уже да */

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
// See https://git.um.si/grega.mocnik/mbed-os-ext/blob/master/Connectivity/features/unsupported/USBHost/targets/TARGET_RENESAS/TARGET_VK_RZ_A1H/usb1/src/host/usb1_host_usbsig.c

HAL_StatusTypeDef USB_ResetPort2(USB_OTG_GlobalTypeDef *USBx, uint8_t status)
{
	const portholder_t vbits =
			USB_DVSTCTR0_WKUP |
			USB_DVSTCTR0_RWUPE |
			USB_DVSTCTR0_USBRST |
			USB_DVSTCTR0_RESUME |
			USB_DVSTCTR0_UACT |
			0;

	// status 0: reset off, 1: reset on
	PRINTF("USB_ResetPort: status=%u\n", (unsigned) status);

	if (status)
	{
		USBx->DVSTCTR0 = (USBx->DVSTCTR0 & vbits) | USB_DVSTCTR0_USBRST;
		(void) USBx->DVSTCTR0;
		// Надо бы дождаться... Но виснем
//		while ((USBx->SYSSTS0 & USB_SYSSTS0_HTACT) != 0)
//			;
		USBx->DVSTCTR0 = (USBx->DVSTCTR0 & vbits) & ~ USB_DVSTCTR0_UACT;
		(void) USBx->DVSTCTR0;
	}
	else
	{
//		USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_HSE)) |
//				0 * USB_SYSCFG_HSE |	// HSE
//				0;
//		(void) USBx->SYSCFG0;

		USBx->DVSTCTR0 = (USBx->DVSTCTR0 & vbits) | USB_DVSTCTR0_UACT;
		(void) USBx->DVSTCTR0;
		USBx->DVSTCTR0 = (USBx->DVSTCTR0 & vbits) & ~ USB_DVSTCTR0_USBRST;
		(void) USBx->DVSTCTR0;
	}

	return HAL_OK;
}

HAL_StatusTypeDef USB_ResetPort(USB_OTG_GlobalTypeDef *USBx)
{
	USB_ResetPort2(USBx, 1);
	//HAL_Delay(100U);                                 /* See Note #1 */
	local_delay_ms(100);
	USB_ResetPort2(USBx, 0);
	//HAL_Delay(10U);
	local_delay_ms(10);

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

HAL_StatusTypeDef USB_HostInit(USB_OTG_GlobalTypeDef *USBx, USB_OTG_CfgTypeDef cfg)
{
	unsigned i;
	const uint_fast8_t HS = cfg.speed == PCD_SPEED_HIGH;

	USBx->SYSCFG0 &= ~ USB_SYSCFG_USBE;	// USBE 0: USB module operation is disabled.
	(void) USBx->SYSCFG0;

	USBx->SOFCFG =
		USB_SOFCFG_BRDYM |	// BRDYM
		0;
	(void) USBx->SOFCFG;

	USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_HSE)) |
			0;

	USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_DPRPU | USB_SYSCFG_DRPD)) |
			0 * USB_SYSCFG_DPRPU |	// DPRPU 0: Pulling up the D+ line is disabled.
			1 * USB_SYSCFG_DRPD |	// DRPD 1: Pulling down the lines is enabled.
			0;
	(void) USBx->SYSCFG0;

	USBx->SYSCFG0 |= USB_SYSCFG_USBE;	// USBE 1: USB module operation is enabled.
	(void) USBx->SYSCFG0;

	USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_HSE)) |
			HS * USB_SYSCFG_HSE |	// HSE
			0;
	(void) USBx->SYSCFG0;

	USBx->INTSTS0 = 0;
	USBx->INTSTS1 = 0;

	/*
	The RSME, DVSE, and CTRE bits can be set to 1 only when the function controller mode is selected; do not set these bits to 1
	to enable the corresponding interrupt output when the host controller mode is selected.
	*/
	USBx->INTENB0 =
		(cfg.Sof_enable != DISABLE) * USB_INTENB0_SOFE |	// SOFE	1: Frame Number Update Interrupt Enable
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

		// Reserved bits: The write value should always be 0.
		* DEVADDn =
			((1 ? 0x03 : 0x02) << USB_DEVADDn_USBSPD_SHIFT) |
			(0x00 << USB_DEVADDn_HUBPORT_SHIFT) |
			(0x00 << USB_DEVADDn_UPPHUB_SHIFT) |
			0;
		(void) * DEVADDn;
		//PRINTF("USB_HostInit: DEVADD%X=%04x\n", i, (unsigned) * DEVADDn);
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
HAL_StatusTypeDef USB_DoPing(USB_OTG_GlobalTypeDef *USBx, uint8_t ch_num)
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
HAL_StatusTypeDef USB_EPSetStall(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
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
HAL_StatusTypeDef USB_EPClearStall(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
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
HAL_StatusTypeDef USB_EP0_OutStart(USB_OTG_GlobalTypeDef *USBx, uint8_t dma, uint8_t *psetup)
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
// Place for USB_xxx functions
/**
  * @brief  USB_SetDevAddress : Stop the usb device mode
  * @param  USBx : Selected device
  * @param  address : new device address to be assigned
  *          This parameter can be a value from 0 to 255
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_SetDevAddress (USB_OTG_GlobalTypeDef *USBx, uint8_t address)
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

/**
  * @brief  Initializes the USB Core
  * @param  USBx: USB Instance
  * @param  cfg : pointer to a USB_OTG_CfgTypeDef structure that contains
  *         the configuration information for the specified USBx peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef USB_CoreInit(USB_OTG_GlobalTypeDef * USBx, USB_OTG_CfgTypeDef cfg)
{
	// P1 clock (66.7 MHz max) period = 15 ns
	// The cycle period required to consecutively access registers of this controller must be at least 67 ns.
	// TODO: compute BWAIT value on-the-fly
	// Use P1CLOCK_FREQ
	const uint_fast32_t bwait = MIN(MAX(calcdivround2(P1CLOCK_FREQ, 15000000uL), 2) - 2, 63);
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
HAL_StatusTypeDef USB_DevInit(USB_OTG_GlobalTypeDef *USBx, USB_OTG_CfgTypeDef cfg)
{
	unsigned i;

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
			(cfg.speed == PCD_SPEED_HIGH) * USB_SYSCFG_HSE |	// HSE
			0;
	(void) USBx->SYSCFG0;

	USBx->INTSTS0 = 0;
	USBx->INTSTS1 = 0;

	USBx->INTENB0 =
		(cfg.Sof_enable != DISABLE) * USB_INTENB0_SOFE |	// SOFE	1: Frame Number Update Interrupt Enable
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

		// Reserved bits: The write value should always be 0.
		* DEVADDn = 0;
		(void) * DEVADDn;
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

/**
  * @brief  Initializes the PCD according to the specified
  *         parameters in the PCD_InitTypeDef and create the associated handle.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd)
{
	  USB_OTG_GlobalTypeDef *USBx;
	  uint8_t i;

	  /* Check the PCD handle allocation */
	  if (hpcd == NULL)
	  {
	    return HAL_ERROR;
	  }

	  /* Check the parameters */
	  //assert_param(IS_PCD_ALL_INSTANCE(hpcd->Instance));

	  USBx = hpcd->Instance;

	  if (hpcd->State == HAL_PCD_STATE_RESET)
	  {
	    /* Allocate lock resource and initialize it */
	    hpcd->Lock = HAL_UNLOCKED;

	#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
	    hpcd->SOFCallback = HAL_PCD_SOFCallback;
	    hpcd->SetupStageCallback = HAL_PCD_SetupStageCallback;
	    hpcd->ResetCallback = HAL_PCD_ResetCallback;
	    hpcd->SuspendCallback = HAL_PCD_SuspendCallback;
	    hpcd->ResumeCallback = HAL_PCD_ResumeCallback;
	    hpcd->ConnectCallback = HAL_PCD_ConnectCallback;
	    hpcd->DisconnectCallback = HAL_PCD_DisconnectCallback;
	    hpcd->DataOutStageCallback = HAL_PCD_DataOutStageCallback;
	    hpcd->DataInStageCallback = HAL_PCD_DataInStageCallback;
	    hpcd->ISOOUTIncompleteCallback = HAL_PCD_ISOOUTIncompleteCallback;
	    hpcd->ISOINIncompleteCallback = HAL_PCD_ISOINIncompleteCallback;
	    hpcd->LPMCallback = HAL_PCDEx_LPM_Callback;
	    hpcd->BCDCallback = HAL_PCDEx_BCD_Callback;

	    if (hpcd->MspInitCallback == NULL)
	    {
	      hpcd->MspInitCallback = HAL_PCD_MspInit;
	    }

	    /* Init the low level hardware */
	    hpcd->MspInitCallback(hpcd);
	#else
	    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
	    HAL_PCD_MspInit(hpcd);
	#endif /* (USE_HAL_PCD_REGISTER_CALLBACKS) */
	  }

	  hpcd->State = HAL_PCD_STATE_BUSY;

	  /* Disable DMA mode for FS instance */
//	  if ((USBx->CID & (0x1U << 8)) == 0U)
//	  {
//	    hpcd->Init.dma_enable = 0U;
//	  }

	  /* Disable the Interrupts */
	  __HAL_PCD_DISABLE(hpcd);

	  /*Init the Core (common init.) */
	  if (USB_CoreInit(USBx, hpcd->Init) != HAL_OK)
	  {
	    hpcd->State = HAL_PCD_STATE_ERROR;
	    return HAL_ERROR;
	  }

	  /* Force Device Mode*/
	  (void)USB_SetCurrentMode(USBx, USB_DEVICE_MODE);

	  /* Init endpoints structures */
	  for (i = 0U; i < hpcd->Init.dev_endpoints; i++)
	  {
	    /* Init ep structure */
	    hpcd->IN_ep[i].is_in = 1U;
	    hpcd->IN_ep[i].num = i;
	    //hpcd->IN_ep[i].tx_fifo_num = i;
	    /* Control until ep is activated */
	    hpcd->IN_ep[i].type = EP_TYPE_CTRL;
	    hpcd->IN_ep[i].maxpacket = 0U;
	    hpcd->IN_ep[i].xfer_buff = 0U;
	    hpcd->IN_ep[i].xfer_len = 0U;
	  }

	  for (i = 0U; i < hpcd->Init.dev_endpoints; i++)
	  {
	    hpcd->OUT_ep[i].is_in = 0U;
	    hpcd->OUT_ep[i].num = i;
	    /* Control until ep is activated */
	    hpcd->OUT_ep[i].type = EP_TYPE_CTRL;
	    hpcd->OUT_ep[i].maxpacket = 0U;
	    hpcd->OUT_ep[i].xfer_buff = 0U;
	    hpcd->OUT_ep[i].xfer_len = 0U;
	  }

	  /* Init Device */
	  if (USB_DevInit(USBx, hpcd->Init) != HAL_OK)
	  {
	    hpcd->State = HAL_PCD_STATE_ERROR;
	    return HAL_ERROR;
	  }

	  hpcd->USB_Address = 0U;
	  hpcd->State = HAL_PCD_STATE_READY;

	  /* Activate LPM */
//	  if (hpcd->Init.lpm_enable == 1U)
//	  {
//	    (void)HAL_PCDEx_ActivateLPM(hpcd);
//	  }

	  (void)USB_DevDisconnect(USBx);

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
	  if (hpcd == NULL)
	  {
	    return HAL_ERROR;
	  }

	  hpcd->State = HAL_PCD_STATE_BUSY;

	  /* Stop Device */
	  if (USB_StopDevice(hpcd->Instance) != HAL_OK)
	  {
	    return HAL_ERROR;
	  }

	#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
	  if (hpcd->MspDeInitCallback == NULL)
	  {
	    hpcd->MspDeInitCallback = HAL_PCD_MspDeInit; /* Legacy weak MspDeInit  */
	  }

	  /* DeInit the low level hardware */
	  hpcd->MspDeInitCallback(hpcd);
	#else
	  /* DeInit the low level hardware: CLOCK, NVIC.*/
	  HAL_PCD_MspDeInit(hpcd);
	#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */

	  hpcd->State = HAL_PCD_STATE_RESET;

	  return HAL_OK;
}

/**
  * @}
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


static uint_fast8_t
USB_Is_OTG_HS(USB_OTG_GlobalTypeDef *USBx)
{
	return 1;
}

#endif /* CPUSTYLE_R7S721 */

