/* $Id$ */
// rza1xx_hal_usb.c
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//


#include "hardware.h"

#if CPUSTYLE_R7S721 && defined (WITHUSBHW_DEVICE)

//#define WITHNEWUSBHAL 1

#include "board.h"
#include "audio.h"
#include "formats.h"
#include "gpio.h"
#include "buffers.h"

#include <string.h>

#include "src/usb/usb200.h"
#include "src/usb/usbch9.h"

#include "usb_device.h"
#include "usbd_core.h"
#include "usbh_core.h"
#include "usbh_def.h"

#include "rza1xx_hal.h"
#include "rza1xx_hal_usb.h"

#include "USBEndpoints_RZ_A1.h"
#include "USBPhy_RZ_A1_Def.h"
#include "USBPhyTypes.h"

extern PCD_HandleTypeDef hpcd_USB_OTG;

volatile uint16_t *USBPhyHw_get_pipectr_reg(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe);
volatile uint16_t *USBPhyHw_get_pipetre_reg(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe);
volatile uint16_t *USBPhyHw_get_pipetrn_reg(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe);
volatile uint16_t *USBPhyHw_get_fifosel_reg(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe);
volatile iodefine_reg32_t *USBPhyHw_get_fifo_reg(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe);
volatile uint16_t *USBPhyHw_get_fifoctr_reg(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe);
void USBPhyHw_buf_to_fifo(PCD_HandleTypeDef *hpcd, uint16_t pipe);
void USBPhyHw_fifo_to_buf(PCD_HandleTypeDef *hpcd, uint16_t pipe);
void USBPhyHw_chg_curpipe(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe, uint16_t isel);
void USBPhyHw_set_mbw(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe, uint16_t data);
void USBPhyHw_set_pid(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe, uint16_t new_pid);
uint16_t USBPhyHw_get_pid(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe);
uint16_t USBPhyHw_write_data(PCD_HandleTypeDef *hpcd, uint16_t pipe);
uint16_t USBPhyHw_read_data(PCD_HandleTypeDef *hpcd, uint16_t pipe);
uint16_t USBPhyHw_EP2PIPE(uint16_t endpoint);
uint16_t USBPhyHw_PIPE2EP(uint16_t pipe);
uint16_t USBPhyHw_PIPE2FIFO(uint16_t pipe);
void USBPhyHw_forced_termination(PCD_HandleTypeDef *hpcd, uint16_t pipe, uint16_t status);
int USBPhyHw_chk_vbsts(USB_OTG_GlobalTypeDef * USBx);
//void USBPhyHw__usbisr(void);
//void USBPhyHw_disconnect(USB_OTG_GlobalTypeDef * USBx);
//void USBPhyHw_connect(USB_OTG_GlobalTypeDef * USBx);
void USBPhyHw_reset_usb(USB_OTG_GlobalTypeDef * USBx, uint16_t clockmode);

static void usb_save_request(USB_OTG_GlobalTypeDef * USBx, USBD_SetupReqTypedef *req);

// на RENESAS для работы с изохронными ендпоинтами используется DMA
//#define WITHDMAHW_UACIN 1		// при этой опции после пересоединения USB кабеля отваливается поток IN
//#define WITHDMAHW_UACOUT 1	// Устойчиво работает - но пропуск пакетов

#if 1

uint16_t USBPhyHw_EP2PIPE(uint16_t ep_addr)
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

	case USBD_CDCACM_INT_EP(USBD_EP_CDCACM_INT, 0):	return HARDWARE_USBD_PIPE_CDC_INT;
	case USBD_CDCACM_INT_EP(USBD_EP_CDCACM_INT, 1):	return HARDWARE_USBD_PIPE_CDC_INTb;

	case USBD_CDCACM_OUT_EP(USBD_EP_CDCACM_OUT, 0):	return HARDWARE_USBD_PIPE_CDC_OUT;
	case USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, 0):	return HARDWARE_USBD_PIPE_CDC_IN;
	case USBD_CDCACM_OUT_EP(USBD_EP_CDCACM_OUT, 1):	return HARDWARE_USBD_PIPE_CDC_OUTb;
	case USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, 1):	return HARDWARE_USBD_PIPE_CDC_INb;

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

// DCP (PIPE0) как аргумент недопустим - отображение неоднозначно.
uint16_t USBPhyHw_PIPE2EP(uint16_t pipe)
{
    if (pipe == USB_PIPE0) {
        return 0;
    }
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

	case HARDWARE_USBD_PIPE_CDC_INT: return USBD_CDCACM_INT_EP(USBD_EP_CDCACM_INT, 0);
	case HARDWARE_USBD_PIPE_CDC_INTb: return USBD_CDCACM_INT_EP(USBD_EP_CDCACM_INT, 1);

	case HARDWARE_USBD_PIPE_CDC_OUT: return USBD_CDCACM_OUT_EP(USBD_EP_CDCACM_OUT, 0);
	case HARDWARE_USBD_PIPE_CDC_IN: return USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, 0);

	case HARDWARE_USBD_PIPE_CDC_OUTb: return USBD_CDCACM_OUT_EP(USBD_EP_CDCACM_OUT, 1);
	case HARDWARE_USBD_PIPE_CDC_INb: return USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, 1);

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
#endif
////////////////////////////////

#if WITHNEWUSBHAL
/**** User Selection ****/
// looks like USB_DevInit
//void USBPhyHw_init(USB_OTG_GlobalTypeDef * USBx)
HAL_StatusTypeDef USB_DevInit(USB_OTG_GlobalTypeDef *USBx, USB_OTG_CfgTypeDef cfg)
{
	unsigned i;
//
//    if (this->events == NULL) {
//        //sleep_manager_lock_deep_sleep();
//    }
//    this->events = events;
//
//    /* registers me */
//    instance = this;

    /* Disable IRQ */

    if (USBx == & USB200) {
		CPG.STBCR7 &= ~ (CPG_STBCR7_MSTP71);
		(void) CPG.STBCR7;
	} else if (USBx == & USB201) {
		CPG.STBCR7 &= ~ (CPG_STBCR7_MSTP71 | CPG_STBCR7_MSTP70);
		(void) CPG.STBCR7;
	}

    /* module reset and clock select */
    USBPhyHw_reset_usb(USBx, USB_SYSCFG_UCKSEL_Value);                            /* USB_X1 48MHz */

    /* Set to USB Function and select speed */
    USBx->SYSCFG0 &= ~USB_DPRPU;
    USBx->SYSCFG0 &= ~USB_DRPD;
    USBx->SYSCFG0 &= ~USB_DCFM;                        /* USB Functoin */
    USBx->SYSCFG0 |= USB_USBE;

#if WITHUSBDEV_HSDESC
	const uint8_t usb_speed_HS =    1;        // 1: High-Speed  0: Full-Speed
#else /* WITHUSBDEV_HSDESC */
	const uint8_t usb_speed_HS =    0;        // 1: High-Speed  0: Full-Speed
#endif /* WITHUSBDEV_HSDESC */
    if (usb_speed_HS == 0) {
        USBx->SYSCFG0 &= ~ USB_HSE;                     /* Full-Speed */
    } else {
        USBx->SYSCFG0 |= USB_HSE;                      /* High-Speed */
    }
    HARDWARE_DELAY_US(1500);

	// When the function controller mode is selected, set all the bits in this register to 0.
	for (i = 0; i < USB20_DEVADD0_COUNT; ++ i) {
		volatile uint16_t * const DEVADDn = (& USBx->DEVADD0) + i;

		// Reserved bits: The write value should always be 0.
		* DEVADDn = 0;
		(void) * DEVADDn;
	}

	return HAL_OK;
}
#endif /* WITHNEWUSBHAL */

void USBPhyHw_deinit(USB_OTG_GlobalTypeDef * USBx)
{
	USB_DevDisconnect(USBx);

    if (USBx == & USB200)
	{
        CPG.STBCR7 |= CPG_STBCR7_MSTP71;
		(void) CPG.STBCR7;
	}
	else if (USBx == & USB201)
	{
	    CPG.STBCR7 |= (CPG_STBCR7_MSTP71 | CPG_STBCR7_MSTP70);
		(void) CPG.STBCR7;
	}

//    if (events != NULL) {
//        //sleep_manager_unlock_deep_sleep();
//    }
//    events = NULL;
}

int USBPhyHw_powered(USB_OTG_GlobalTypeDef * USBx)
{
    // return 1 if powered 0 otherwise. Devices which don't support
    //    this should always return 1
    return 1;
}

//void USBPhyHw_connect(USB_OTG_GlobalTypeDef * USBx)
HAL_StatusTypeDef  USB_DevConnect(USB_OTG_GlobalTypeDef *USBx)
{
    /* Enable pullup on D+ */
    USBx->INTENB0 |= (USB_VBSE | USB_SOFE | USB_DVSE | USB_CTRE | USB_BEMPE | USB_NRDYE | USB_BRDYE);
    USBx->SYSCFG0 |= USB_DPRPU;

//    InterruptHandlerRegister(USBIX_IRQn, &_usbisr);
//    GIC_SetPriority(USBIX_IRQn, 16);
//    GIC_SetConfiguration(USBIX_IRQn, 1);
//    GIC_EnableIRQ(USBIX_IRQn);
	//arm_hardware_set_handler_system(USBIX_IRQn, _usbisr);

    return HAL_OK;
}

//void USBPhyHw_disconnect(USB_OTG_GlobalTypeDef * USBx)
HAL_StatusTypeDef  USB_DevDisconnect(USB_OTG_GlobalTypeDef *USBx)
{
    /* Disable USB */
//    arm_hardware_disable_handler(USBIX_IRQn);
//    InterruptHandlerRegister(USBIX_IRQn, NULL);

    /* Disable pullup on D+ */
    USBx->SYSCFG0 &= ~USB_DPRPU;
    HARDWARE_DELAY_MS(1);
    USBx->SYSCFG0 |= USB_DCFM;
    HARDWARE_DELAY_MS(1);
    USBx->SYSCFG0 &= ~USB_DCFM;

    USBx->INTENB0 = 0;

    return HAL_OK;
}

void USBPhyHw_configure(USB_OTG_GlobalTypeDef * USBx)
{
}

void USBPhyHw_unconfigure(USB_OTG_GlobalTypeDef * USBx)
{
}

void USBPhyHw_sof_enable(USB_OTG_GlobalTypeDef * USBx)
{
    /* Enable SOF interrupt */
    USBx->INTENB0 |= USB_SOFE;
}

void USBPhyHw_sof_disable(USB_OTG_GlobalTypeDef * USBx)
{
    /* Disable SOF interrupt */
    USBx->INTENB0 &= ~USB_SOFE;
}

void USBPhyHw_set_address(USB_OTG_GlobalTypeDef * USBx, uint8_t address)
{
    if (address <= 127) {
        USBPhyHw_set_pid(USBx, USB_PIPE0, USB_PID_BUF);        /* Set BUF */
    } else {
        USBPhyHw_set_pid(USBx, USB_PIPE0, USB_PID_STALL);      /* Not specification */
    }
}

void USBPhyHw_remote_wakeup(USB_OTG_GlobalTypeDef * USBx)
{
}

#if 0
const usb_ep_table_t *USBPhyHw_endpoint_table(void)
{
    static const usb_ep_table_t rza1_table = {
        1, // No cost per endpoint - everything allocated up front
        {
            {USB_EP_ATTR_ALLOW_CTRL | USB_EP_ATTR_DIR_IN_AND_OUT, 0, 0},
            {USB_EP_ATTR_ALLOW_INT  | USB_EP_ATTR_DIR_IN_AND_OUT, 0, 0},
            {USB_EP_ATTR_ALLOW_BULK | USB_EP_ATTR_DIR_IN_AND_OUT, 0, 0},
            {USB_EP_ATTR_ALLOW_ISO  | USB_EP_ATTR_DIR_IN_AND_OUT, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
        }
    };
    return &rza1_table;
}
#endif

uint32_t USBPhyHw_ep0_set_max_packet(uint32_t max_packet)
{
    return MAX_PACKET_SIZE_EP0;
}

static void events_suspend(PCD_HandleTypeDef *hpcd, int state)
{
	PRINTF("%s:\n", __func__);
	HAL_PCD_SuspendCallback(hpcd);

}

static void events_sof(PCD_HandleTypeDef *hpcd, unsigned framenum)
{
	//PRINTF("%s:\n", __func__);
	HAL_PCD_SOFCallback(hpcd);

}

static void events_reset(PCD_HandleTypeDef *hpcd)
{
	//PRINTF("%s:\n", __func__);
	HAL_PCD_ResetCallback(hpcd);

}

static void events_address(PCD_HandleTypeDef *hpcd)
{
	//PRINTF("%s:\n", __func__);
	HAL_PCD_AdressedCallback(hpcd);

}

static void events_ep0_setup(PCD_HandleTypeDef *hpcd)
{
	//PRINTF("%s:\n", __func__);
	HAL_PCD_SetupStageCallback(hpcd);

}

static void events_ep0_in(PCD_HandleTypeDef *hpcd)
{
	USBD_HandleTypeDef * const pdev = hpcd->pData;
	//PRINTF("%s:\n", __func__);
	USBD_SetupReqTypedef *req = & pdev->request;

	PCD_EPTypeDef *ep;

	ep = &hpcd->IN_ep[0 & EP_ADDR_MSK];
	ep->xfer_buff += ep->maxpacket;	// пересланный размер может отличаться от максимального
	//ep->xfer_count += ep->maxpacket;	// ?
	//PRINTF("%s: ep->xfer_buff=%p, ep->xfer_count=%u, ep->maxpacket=%u\n", __func__, ep->xfer_buff, ep->xfer_count, ep->maxpacket);
	HAL_PCD_DataInStageCallback(hpcd, 0);

}

static void events_ep0_out(PCD_HandleTypeDef *hpcd)
{
	unsigned bcnt = hpcd->pipe_ctrl[USB_PIPE0].data_cnt;
	PCD_EPTypeDef *ep;
	//PRINTF("%s: bcnt=%u\n", __func__, bcnt);

	ep = &hpcd->OUT_ep[0 & EP_ADDR_MSK];
	ep->xfer_buff += bcnt;
	ep->xfer_count += bcnt;
	HAL_PCD_DataOutStageCallback(hpcd, 0);

}

static void events_in(PCD_HandleTypeDef *hpcd, uint16_t endpoint)
{
	//PRINTF("%s:\n", __func__);
	PCD_EPTypeDef *ep;
	ep = &hpcd->OUT_ep[endpoint & EP_ADDR_MSK];
	ep->xfer_buff += ep->maxpacket;	// пересланный размер может отличаться от максимального
	HAL_PCD_DataInStageCallback(hpcd, endpoint & EP_ADDR_MSK);

}

static void events_out(PCD_HandleTypeDef *hpcd, uint16_t endpoint)
{
	uint16_t pipe = USBPhyHw_EP2PIPE(endpoint);
	//PRINTF("%s:\n", __func__);
	unsigned bcnt = hpcd->pipe_ctrl[pipe].data_cnt;
	PCD_EPTypeDef *ep;

	ep = &hpcd->OUT_ep[endpoint & EP_ADDR_MSK];
	ep->xfer_buff += bcnt;
	ep->xfer_count += bcnt;
	HAL_PCD_DataOutStageCallback(hpcd, endpoint);

}

void USBPhyHw_ep0_setup_read_result(PCD_HandleTypeDef *hpcd, uint8_t *buffer, uint32_t size)
{
    memcpy(buffer, hpcd->setup_buffer, size);

}

void USBPhyHw_ep0_read(PCD_HandleTypeDef *hpcd, uint8_t *data, uint32_t size)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;

	hpcd->pipe_ctrl[USB_PIPE0].req_size  = size;
	hpcd->pipe_ctrl[USB_PIPE0].data_cnt  = size;
	hpcd->pipe_ctrl[USB_PIPE0].p_data    = data;

    USBPhyHw_chg_curpipe(USBx, USB_PIPE0, USB_ISEL_READ);      /* Switch FIFO and pipe number. */
    USBx->CFIFOCTR = USB_BCLR;                 /* Buffer clear */
    USBPhyHw_set_pid(USBx, USB_PIPE0, USB_PID_BUF);            /* Set BUF */
    USBx->BRDYENB |= (1 << USB_PIPE0);         /* Enable ready interrupt */
    USBx->NRDYENB |= (1 << USB_PIPE0);         /* Enable not ready interrupt */
}

uint32_t USBPhyHw_ep0_read_result(PCD_HandleTypeDef *hpcd)
{
    return hpcd->pipe_ctrl[USB_PIPE0].req_size;
}

static void data_end(PCD_HandleTypeDef *hpcd, uint16_t fifo, uint16_t err)
{
	//PRINTF("%s: err=%u\n", __func__, err);
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
	USBD_HandleTypeDef * const pdev = hpcd->pData;
	//USBD_CtlError(pdev, & pdev->request);

}

static void ctrl_end(PCD_HandleTypeDef *hpcd, uint16_t err)
{
	//PRINTF("%s: err=%u\n", __func__, err);
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
	USBD_HandleTypeDef * const pdev = hpcd->pData;
	//USBD_CtlError(pdev, & pdev->request);

}

void USBPhyHw_ep0_write(PCD_HandleTypeDef *hpcd, uint8_t *buffer, uint32_t size)
{
	//PRINTF("%s: buffer=%p, size=%u\n", __func__, buffer, size);
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
    if ((buffer == NULL) || (size == 0)) {
        USBPhyHw_set_pid(USBx, USB_PIPE0, USB_PID_BUF);            /* Set BUF */
        return;
    }

    hpcd->pipe_ctrl[USB_PIPE0].req_size  = size;
    hpcd->pipe_ctrl[USB_PIPE0].data_cnt  = size;
    hpcd->pipe_ctrl[USB_PIPE0].p_data    = buffer;

    USBPhyHw_chg_curpipe(USBx, USB_PIPE0, USB_ISEL_WRITE);         /* Switch FIFO and pipe number. */
    USBx->CFIFOCTR = USB_BCLR;                     /* Buffer clear */
    /* Clear the PIPExBEMP status bit of the specified pipe to clear */
    USBx->BEMPSTS = (uint16_t)((~(1 << USB_PIPE0)) & BEMPSTS_MASK);

    /* Peripheral control sequence */
    switch (USBPhyHw_write_data(hpcd, USB_PIPE0)) {
        case USB_WRITING :                          /* Continue of data write */
            USBx->BRDYENB |= (1 << USB_PIPE0);     /* Enable Ready interrupt */
            USBx->NRDYENB |= (1 << USB_PIPE0);     /* Enable Not Ready Interrupt */
            USBPhyHw_set_pid(USBx, USB_PIPE0, USB_PID_BUF);
            break;
        case USB_WRITEEND :                         /* End of data write */
        case USB_WRITESHRT :                        /* End of data write */
            USBx->BEMPENB |= (1 << USB_PIPE0);     /* Enable Empty Interrupt */
            USBx->NRDYENB |= (1 << USB_PIPE0);     /* Enable Not Ready Interrupt */
            USBPhyHw_set_pid(USBx, USB_PIPE0, USB_PID_BUF);
            break;
        case USB_FIFOERROR :                        /* FIFO access error */
            ctrl_end(hpcd, (uint16_t)USB_DATA_ERR);
            break;
        default :
            break;
    }
}

void USBPhyHw_ep0_stall(PCD_HandleTypeDef *hpcd)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
    USBPhyHw_set_pid(USBx, USB_PIPE0, USB_PID_STALL);
    hpcd->run_later_ctrl_comp = 0;
}

void USBPhyHw_endpoint_stall(PCD_HandleTypeDef *hpcd, usb_ep_t endpoint)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
   uint16_t pipe = USBPhyHw_EP2PIPE(endpoint);

    USBPhyHw_set_pid(USBx, pipe, USB_PID_STALL);

    hpcd->pipe_ctrl [pipe].enable = 0;
    hpcd->pipe_ctrl [pipe].status = USB_DATA_STALL;
}

void USBPhyHw_endpoint_unstall(PCD_HandleTypeDef *hpcd, usb_ep_t endpoint)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
    uint16_t pipe = USBPhyHw_EP2PIPE(endpoint);
    volatile uint16_t *p_reg;

    USBPhyHw_set_pid(USBx, pipe, USB_PID_NAK);

    p_reg = USBPhyHw_get_pipectr_reg(USBx, pipe);
    /* Set toggle bit to DATA0 */
    *p_reg |= USB_SQCLR;
    /* Buffer Clear */
    *p_reg |= USB_ACLRM;
    *p_reg &= ~USB_ACLRM;

    hpcd->pipe_ctrl [pipe].enable = 0;
    hpcd->pipe_ctrl [pipe].status = USB_DATA_NONE;
}

int USBPhyHw_endpoint_read(PCD_HandleTypeDef *hpcd, usb_ep_t endpoint, uint8_t *data, uint32_t size)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
    uint16_t mxps;
    uint16_t trncnt;
    volatile uint16_t *p_reg;
    uint16_t pipe = USBPhyHw_EP2PIPE(endpoint);

    if (hpcd->pipe_ctrl [pipe].status == USB_DATA_STALL) {
        return 0;
    }

    hpcd->pipe_ctrl [pipe].status   = USB_DATA_READING;
    hpcd->pipe_ctrl [pipe].req_size = size;
    hpcd->pipe_ctrl [pipe].data_cnt = size;
    hpcd->pipe_ctrl [pipe].p_data   = data;
    hpcd->pipe_ctrl [pipe].enable   = 1;

    USBPhyHw_set_pid(USBx, pipe, USB_PID_NAK);                                     /* Set NAK */

    USBx->BEMPSTS = (uint16_t)((~(1 << pipe)) & BEMPSTS_MASK);     /* BEMP Status Clear */
    USBx->BRDYSTS = (uint16_t)((~(1 << pipe)) & BRDYSTS_MASK);     /* BRDY Status Clear */
    USBx->NRDYSTS = (uint16_t)((~(1 << pipe)) & NRDYSTS_MASK);     /* NRDY Status Clear */

    USBPhyHw_chg_curpipe(USBx, pipe, USB_ISEL_READ);                               /* Switch FIFO and pipe number. */
    p_reg = USBPhyHw_get_fifoctr_reg(USBx, pipe);
    *p_reg = USB_BCLR;                                              /* Clear BCLR */

    if (size != 0) {
        /* Max Packet Size */
        USBx->PIPESEL = pipe;                                          /* Pipe select */
        mxps = (uint16_t)(USBx->PIPEMAXP & USB_MXPS);
        /* Data size check */
        if ((size % mxps) == (uint32_t)0u) {
            trncnt = (uint16_t)(size / mxps);
        } else {
            trncnt = (uint16_t)((size / mxps) + (uint32_t)1u);
        }

        /* Set Transaction counter */
        p_reg = USBPhyHw_get_pipetre_reg(USBx, pipe);
        if (p_reg != NULL) {
            *p_reg |= USB_TRCLR;
        }
        p_reg = USBPhyHw_get_pipetrn_reg(USBx, pipe);
        if (p_reg != NULL) {
            *p_reg = trncnt;
        }
        p_reg = USBPhyHw_get_pipetre_reg(USBx, pipe);
        if (p_reg != NULL) {
            *p_reg |= USB_TRENB;
        }

        p_reg = USBPhyHw_get_pipectr_reg(USBx, pipe);
        /* Buffer Clear */
        *p_reg |= USB_ACLRM;
        *p_reg &= ~USB_ACLRM;
    }

    USBPhyHw_set_pid(USBx, pipe, USB_PID_BUF);                                     /* Set BUF */
    USBx->BRDYENB |= (1 << pipe);                                  /* Enable Ready Interrupt */
    USBx->NRDYENB |= (1 << pipe);                                  /* Enable Not Ready Interrupt */

    return 1;
}


#if WITHNEWUSBHAL
//uint32_t USBPhyHw_endpoint_read_result(PCD_HandleTypeDef *hpcd, usb_ep_t endpoint)
uint32_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
    uint16_t pipe = USBPhyHw_EP2PIPE(ep_addr);

    return hpcd->pipe_ctrl [pipe].req_size;
}
#else /* WITHNEWUSBHAL */
uint32_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
	return hpcd->OUT_ep[ep_addr & EP_ADDR_MSK].xfer_count;
}
#endif /* WITHNEWUSBHAL */

int USBPhyHw_endpoint_write(PCD_HandleTypeDef *hpcd, usb_ep_t endpoint, uint8_t *data, uint32_t size)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
    uint16_t pipe = USBPhyHw_EP2PIPE(endpoint);
    volatile uint16_t * const p_reg = USBPhyHw_get_pipectr_reg(USBx, pipe);

    if (hpcd->pipe_ctrl [pipe].status == USB_DATA_STALL) {
        return 0;
    }

    hpcd->pipe_ctrl [pipe].status   = USB_DATA_WRITING;
    hpcd->pipe_ctrl [pipe].req_size = size;
    hpcd->pipe_ctrl [pipe].data_cnt = size;
    hpcd->pipe_ctrl [pipe].p_data   = data;
    hpcd->pipe_ctrl [pipe].enable   = 1;

    USBPhyHw_set_pid(USBx, pipe, USB_PID_NAK);                                     /* Set NAK */

    USBx->BEMPSTS = (uint16_t)((~(1 << pipe)) & BEMPSTS_MASK);     /* BEMP Status Clear */
    USBx->BRDYSTS = (uint16_t)((~(1 << pipe)) & BRDYSTS_MASK);     /* BRDY Status Clear */
    USBx->NRDYSTS = (uint16_t)((~(1 << pipe)) & NRDYSTS_MASK);     /* NRDY Status Clear */

    /* Buffer Clear */
    *p_reg |= USB_ACLRM;
    *p_reg &= ~USB_ACLRM;

    USBPhyHw_buf_to_fifo(hpcd, pipe);                                              /* Buffer to FIFO data write */
    USBPhyHw_set_pid(USBx, pipe, USB_PID_BUF);                                     /* Set BUF */

    return 1;
}

void USBPhyHw_endpoint_abort(PCD_HandleTypeDef *hpcd, usb_ep_t endpoint)
{
    USBPhyHw_forced_termination(hpcd, USBPhyHw_EP2PIPE(endpoint), (uint16_t)USB_DATA_NONE);
}

void USBPhyHw_process(PCD_HandleTypeDef *hpcd)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
	USBD_HandleTypeDef * const pdev = hpcd->pData;
	//__DMB();
    /* Register Save */
    uint16_t intsts0 = USBx->INTSTS0;
    uint16_t brdysts = USBx->BRDYSTS;
    uint16_t nrdysts = USBx->NRDYSTS;
    uint16_t bempsts = USBx->BEMPSTS;
    uint16_t intenb0 = USBx->INTENB0;
    uint16_t brdyenb = USBx->BRDYENB;
    uint16_t nrdyenb = USBx->NRDYENB;
    uint16_t bempenb = USBx->BEMPENB;

    /* Interrupt status get */
    uint16_t ists0 = (uint16_t)(intsts0 & intenb0);
    uint16_t bsts  = (uint16_t)(brdysts & brdyenb);
    uint16_t nsts  = (uint16_t)(nrdysts & nrdyenb);
    uint16_t ests  = (uint16_t)(bempsts & bempenb);

    uint16_t i;

    if ((intsts0 & (USB_VBINT | USB_RESM | USB_SOFR | USB_DVST |
                    USB_CTRT | USB_BEMP | USB_NRDY | USB_BRDY)) == 0u) {
        return;
    }

    /***** Processing USB bus signal *****/
    /***** Resume signal *****/
    if ((ists0 & USB_RESM) == USB_RESM) {
        USBx->INTSTS0 = (uint16_t)~USB_RESM;
        USBx->INTENB0 &= (~USB_RSME);        /* RESM interrupt disable */
        events_suspend(hpcd, 1);
    }

    /***** Vbus change *****/
    else if ((ists0 & USB_VBINT) == USB_VBINT) {
        USBx->INTSTS0 = (uint16_t)~USB_VBINT;
        if (USBPhyHw_chk_vbsts(USBx)) {
            /* USB attach */
            /* Non processing. */
        } else {
            /* USB detach */
            for (i = USB_MIN_PIPE_NO; i < PIPE_NUM; i++) {
                if (hpcd->pipe_ctrl[i].enable) {
                    USBPhyHw_forced_termination(hpcd, i, (uint16_t)USB_DATA_NONE);
                }
            }
            USBx->INTSTS0 = 0;
            USBx->BRDYSTS = 0;
            USBx->NRDYSTS = 0;
            USBx->BEMPSTS = 0;
            USBx->BRDYENB = 0;
            USBx->NRDYENB = 0;
            USBx->BEMPENB = 0;
        }
    }

    /***** SOFR change *****/
    else if ((ists0 & USB_SOFR) == USB_SOFR) {
        USBx->INTSTS0 = (uint16_t)~USB_SOFR;
        events_sof(hpcd, USBx->FRMNUM & USB_FRNM);
    }

    /***** Processing device state *****/
    /***** DVST change *****/
    else if ((ists0 & USB_DVST) == USB_DVST) {
        USBx->INTSTS0 = (uint16_t)~USB_DVST;

        switch ((uint16_t)(intsts0 & USB_DVSQ)) {
            case USB_DS_POWR :
                break;
            case USB_DS_DFLT :
                USBx->DCPCFG = 0;                    /* DCP configuration register  (0x5C) */
                USBx->DCPMAXP = MAX_PACKET_SIZE_EP0; /* DCP maxpacket size register (0x5E) */

                events_reset(hpcd);
                break;
            case USB_DS_ADDS :
                events_address(hpcd);
              break;
            case USB_DS_CNFG :
                break;
            case USB_DS_SPD_POWR :
            case USB_DS_SPD_DFLT :
            case USB_DS_SPD_ADDR :
            case USB_DS_SPD_CNFG :
                events_suspend(hpcd, 0);
                break;
            default :
                break;
        }
    }

    /***** Processing PIPE0 data *****/
    else if (((ists0 & USB_BRDY) == USB_BRDY) && ((bsts & USB_BRDY0) == USB_BRDY0)) {
        /* ==== BRDY PIPE0 ==== */
        USBx->BRDYSTS = (uint16_t)((~USB_BRDY0) & BRDYSTS_MASK);

        /* When operating by the peripheral function, usb_brdy_pipe() is executed with PIPEx request because */
        /* two BRDY messages are issued even when the demand of PIPE0 and PIPEx has been generated at the same time. */
        if ((USBx->CFIFOSEL & USB_ISEL_WRITE) == USB_ISEL_WRITE) {
            switch (USBPhyHw_write_data(hpcd, USB_PIPE0)) {
                case USB_WRITEEND :
                case USB_WRITESHRT :
                    USBx->BRDYENB &= (~(1 << USB_PIPE0));
                    break;
                case USB_WRITING :
                    USBPhyHw_set_pid(USBx, USB_PIPE0, USB_PID_BUF);
                    break;
                case USB_FIFOERROR :
                    ctrl_end(hpcd, (uint16_t)USB_DATA_ERR);
                    break;
                default :
                    break;
            }
            events_ep0_in(hpcd);
        } else {
            switch (USBPhyHw_read_data(hpcd, USB_PIPE0)) {
                case USB_READEND :
                case USB_READSHRT :
                    USBx->BRDYENB &= (~(1 << USB_PIPE0));
                    hpcd->pipe_ctrl[USB_PIPE0].req_size -= hpcd->pipe_ctrl[USB_PIPE0].data_cnt;
                    break;
                case USB_READING :
                    USBPhyHw_set_pid(USBx, USB_PIPE0, USB_PID_BUF);
                    break;
                case USB_READOVER :
                    ctrl_end(hpcd, (uint16_t)USB_DATA_OVR);
                    hpcd->pipe_ctrl[USB_PIPE0].req_size -= hpcd->pipe_ctrl[USB_PIPE0].data_cnt;
                    break;
                case USB_FIFOERROR :
                    ctrl_end(hpcd, (uint16_t)USB_DATA_ERR);
                    break;
                default :
                    break;
            }
            events_ep0_out(hpcd);
        }
    } else if (((ists0 & USB_BEMP) == USB_BEMP) && ((ests & USB_BEMP0) == USB_BEMP0)) {
        /* ==== BEMP PIPE0 ==== */
        USBx->BEMPSTS = (uint16_t)((~USB_BEMP0) & BEMPSTS_MASK);

        events_ep0_in(hpcd);
    } else if (((ists0 & USB_NRDY) == USB_NRDY) && ((nsts & USB_NRDY0) == USB_NRDY0)) {
        /* ==== NRDY PIPE0 ==== */
        USBx->NRDYSTS = (uint16_t)((~USB_NRDY0) & NRDYSTS_MASK);
        /* Non processing. */
    }

    /***** Processing setup transaction *****/
    else if ((ists0 & USB_CTRT) == USB_CTRT) {
        USBx->INTSTS0 = (uint16_t)~USB_CTRT;

        /* CTSQ bit changes later than CTRT bit for ASSP. */
        /* CTSQ reloading */
        uint16_t stginfo = (uint16_t)(intsts0 & USB_CTSQ);
        if (stginfo != USB_CS_IDST) {
            if (((USB_CS_RDDS == stginfo) || (USB_CS_WRDS == stginfo)) || (USB_CS_WRND == stginfo)) {
                /* Save request register */
                uint16_t *bufO = &hpcd->setup_buffer[0];

                USBx->INTSTS0 = (uint16_t)~USB_VALID;
                *bufO++ = USBx->USBREQ;   /* data[0] <= bmRequest, data[1] <= bmRequestType */
                *bufO++ = USBx->USBVAL;   /* data[2] data[3] <= wValue */
                *bufO++ = USBx->USBINDX;  /* data[4] data[5] <= wIndex */
                *bufO++ = USBx->USBLENG;  /* data[6] data[7] <= wLength */

        		usb_save_request(USBx, & pdev->request);
            }
        }

        /* Switch on the control transfer stage (CTSQ). */
        switch (stginfo) {
            case USB_CS_IDST :  /* Idle or setup stage */
                break;
            case USB_CS_RDDS :  /* Control read data stage */
                events_ep0_setup(hpcd);
                break;
            case USB_CS_WRDS :  /* Control write data stage */
                events_ep0_setup(hpcd);
                break;
            case USB_CS_WRND :  /* Status stage of a control write where there is no data stage. */
                events_ep0_setup(hpcd);
                hpcd->run_later_ctrl_comp = 1;
                break;
            case USB_CS_RDSS :  /* Control read status stage */
                USBx->DCPCTR |= USB_CCPL;
                break;
            case USB_CS_WRSS :  /* Control write status stage */
                USBx->DCPCTR |= USB_CCPL;
                break;
            case USB_CS_SQER :  /* Control sequence error */
            default :           /* Illegal */
                ctrl_end(hpcd, (uint16_t)USB_DATA_ERR);
                break;
        }
    }

    /***** Processing PIPE1-MAX_PIPE_NO data *****/
    else if ((ists0 & USB_BRDY) == USB_BRDY) {
        /* ==== BRDY PIPEx ==== */
        USBx->BRDYSTS = (uint16_t)((~bsts) & BRDYSTS_MASK);

        for (i = USB_MIN_PIPE_NO; i < PIPE_NUM; i++) {
            if ((bsts & USB_BITSET(i)) != 0u) {
                /* Interrupt check */
                if (hpcd->pipe_ctrl[i].enable) {
                    USBx->PIPESEL = i;
                    if (USB_BUF2FIFO == (uint16_t)(USBx->PIPECFG & USB_DIRFIELD)) {
                        /* write */
                        USBPhyHw_buf_to_fifo(hpcd, i);         /* Buffer to FIFO data write */
                        events_in(hpcd, USBPhyHw_PIPE2EP(i));
                    } else {
                        /* read */
                        USBPhyHw_fifo_to_buf(hpcd, i);         /* FIFO to Buffer data read */
                        events_out(hpcd, USBPhyHw_PIPE2EP(i));
                    }
                }
            }
        }
    } else if ((ists0 & USB_BEMP) == USB_BEMP) {
        /* ==== BEMP PIPEx ==== */
        USBx->BEMPSTS = (uint16_t)((~ests) & BEMPSTS_MASK);

        for (i = USB_MIN_PIPE_NO; i < PIPE_NUM; i++) {
            if ((ests & USB_BITSET(i)) != 0) {
                /* Interrupt check */
                if (hpcd->pipe_ctrl[i].enable) {
                    /* MAX packet size error ? */
                    if (((USBPhyHw_get_pid(USBx, i) & USB_PID_STALL) == USB_PID_STALL) || ((USBPhyHw_get_pid(USBx, i) & USB_PID_STALL2) == USB_PID_STALL2)) {
                        USBPhyHw_forced_termination(hpcd, i, (uint16_t)USB_DATA_STALL);
                    } else {
                        if ((i >= USB_PIPE6) || ((*USBPhyHw_get_pipectr_reg(USBx, i) & USB_INBUFM) != USB_INBUFM)) {
                            data_end(hpcd, i, (uint16_t)USB_DATA_NONE);       /* End of data transfer */
                        } else {
                            USBx->BEMPENB |= (1 << i);
                        }
                    }
                    events_in(hpcd, USBPhyHw_PIPE2EP(i));
                }
            }
        }
    } else if ((ists0 & USB_NRDY) == USB_NRDY) {
        /* ==== NRDY PIPEx ==== */
        USBx->NRDYSTS = (uint16_t)((~nsts) & NRDYSTS_MASK);

        for (i = USB_MIN_PIPE_NO; i < PIPE_NUM; i++) {
            if ((nsts & USB_BITSET(i)) != 0) {
                /* Interrupt check */
                if (hpcd->pipe_ctrl[i].enable) {
                    if (((USBPhyHw_get_pid(USBx, i) & USB_PID_STALL) != USB_PID_STALL) && ((USBPhyHw_get_pid(USBx, i) & USB_PID_STALL2) != USB_PID_STALL2)) {
                        USBPhyHw_set_pid(USBx, i, USB_PID_BUF);
                    }
                }
            }
        }
    } else {
        /* Non processing. */
    }
}

#if WITHNEWUSBHAL
// Renesas, usb device
//void USBPhyHw__usbisr(void)
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
{
	//PCD_HandleTypeDef * const hpcd = & hpcd_USB_OTG;
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
    //arm_hardware_disable_handler(USBIX_IRQn);

	hpcd->run_later_ctrl_comp = 0;

    //events_start_process();
    USBPhyHw_process(hpcd);

    if (hpcd->run_later_ctrl_comp) {
    	USBx->DCPCTR &= (~USB_PID);
    	USBx->DCPCTR |= USB_PID_BUF;

    	USBx->DCPCTR |= USB_CCPL;
    }

    /* Re-enable interrupt */
    //GIC_ClearPendingIRQ(USBIX_IRQn);
    //GIC_EnableIRQ(USBIX_IRQn);
}
#endif /* WITHNEWUSBHAL */

void USBPhyHw_chg_curpipe(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe, uint16_t isel)
{
    volatile uint16_t *p_reg;
    uint16_t fifo_use;
    uint16_t isel_val;
    uint16_t buf;

    fifo_use = USBPhyHw_PIPE2FIFO(pipe);
    if ((fifo_use == USB_FUNCTION_D0FIFO_USE) || (fifo_use == USB_FUNCTION_D1FIFO_USE)) {
        isel_val = 0;
    } else {
        isel_val = isel;
    }

    p_reg = USBPhyHw_get_fifosel_reg(USBx, pipe);
    buf  = *p_reg;
    buf &= (uint16_t)(~(USB_RCNT | USB_ISEL | USB_CURPIPE | USB_MBW));
    buf |= (uint16_t)((USB_RCNT | isel_val | pipe | USB_MBW_32) & (USB_RCNT | USB_ISEL | USB_CURPIPE | USB_MBW));
    *p_reg = buf;

    do {
        HARDWARE_DELAY_US(1);
        buf = *p_reg;
    } while ((buf & (uint16_t)(USB_ISEL | USB_CURPIPE)) != (uint16_t)(isel_val | pipe));
}

uint16_t USBPhyHw_is_set_frdy(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe, uint16_t isel)
{
    volatile uint16_t *p_reg;
    uint16_t buffer;
    int retry_cnt = 0;

    USBPhyHw_chg_curpipe(USBx, pipe, isel);                    /* Changes the FIFO port by the pipe. */
    p_reg = USBPhyHw_get_fifoctr_reg(USBx, pipe);
    for (retry_cnt = 0; retry_cnt < 10; retry_cnt++) {
        buffer = *p_reg;
        if ((uint16_t)(buffer & USB_FRDY) == USB_FRDY) {
            return (buffer);
        }
        HARDWARE_DELAY_US(1);
    }

    return (USB_FIFOERROR);
}

uint8_t *USBPhyHw_read_fifo(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe, uint16_t count, uint8_t *read_p)
{
	volatile iodefine_reg32_t * p_reg;
    uint16_t even;
    uint16_t odd;
    uint32_t odd_byte_data_temp;

    p_reg = USBPhyHw_get_fifo_reg(USBx, pipe);
    for (even = (uint16_t)(count >> 2); (even != 0); --even) {
        /* 32bit FIFO access */
        *((uint32_t *)read_p) = p_reg->UINT32;
        read_p += sizeof(uint32_t);
    }
    odd = count % 4;
    if (count < 4) {
        odd = count;
    }
    if (odd != 0) {
        /* 32bit FIFO access */
        odd_byte_data_temp = p_reg->UINT32;
        /* Condition compilation by the difference of the endian */
        do {
            *read_p = (uint8_t)(odd_byte_data_temp & 0x000000ff);
            odd_byte_data_temp = odd_byte_data_temp >> 8;
            /* Renewal read pointer */
            read_p += sizeof(uint8_t);
            odd--;
        } while (odd != 0);
    }

    return read_p;
}

uint16_t USBPhyHw_read_data(PCD_HandleTypeDef *hpcd, uint16_t pipe)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
    volatile uint16_t *p_reg;
    uint16_t count;
    uint16_t buffer;
    uint16_t mxps;
    uint16_t dtln;
    uint16_t end_flag;

    /* Changes FIFO port by the pipe. */
    buffer = USBPhyHw_is_set_frdy(USBx, pipe, 0);
    if (buffer == USB_FIFOERROR) {
        return (USB_FIFOERROR);                 /* FIFO access error */
    }
    dtln = (uint16_t)(buffer & USB_DTLN);

    /* Max Packet Size */
    if (pipe == USB_PIPE0) {
        mxps = (uint16_t)(USBx->DCPMAXP & USB_MAXP);
    } else {
        USBx->PIPESEL = pipe;                  /* Pipe select */
        mxps = (uint16_t)(USBx->PIPEMAXP & USB_MXPS);
    }

    if (hpcd->pipe_ctrl [pipe].data_cnt < dtln) {
        /* Buffer Over ? */
        end_flag = USB_READOVER;
        USBPhyHw_set_pid(USBx, pipe, USB_PID_NAK);             /* Set NAK */
        count = (uint16_t)hpcd->pipe_ctrl [pipe].data_cnt;
        hpcd->pipe_ctrl [pipe].data_cnt = dtln;
    } else if (hpcd->pipe_ctrl [pipe].data_cnt == dtln) {
        /* Just Receive Size */
        count = dtln;
        if ((count == 0) || ((dtln % mxps) != 0)) {
            /* Just Receive Size */
            /* Peripheral Function */
            end_flag = USB_READSHRT;
        } else {
            end_flag = USB_READEND;
            USBPhyHw_set_pid(USBx, pipe, USB_PID_NAK);         /* Set NAK */
        }
    } else {
        /* Continuous Receive data */
        count = dtln;
        end_flag = USB_READING;
        if (count == 0) {
            /* Null Packet receive */
            end_flag = USB_READSHRT;
            USBPhyHw_set_pid(USBx, pipe, USB_PID_NAK);         /* Set NAK */
        }
        if ((count % mxps) != 0) {
            /* Null Packet receive */
            end_flag = USB_READSHRT;
            USBPhyHw_set_pid(USBx, pipe, USB_PID_NAK);         /* Set NAK */
        }
    }

    if (dtln == 0) { /* 0 length packet */
        p_reg = USBPhyHw_get_fifoctr_reg(USBx, pipe);
        *p_reg = USB_BCLR;                      /* Clear BCLR */
    } else {
    	hpcd->pipe_ctrl [pipe].p_data = USBPhyHw_read_fifo(USBx, pipe, count, hpcd->pipe_ctrl [pipe].p_data);
    }
    hpcd->pipe_ctrl [pipe].data_cnt -= count;

    return end_flag;
}

void USBPhyHw_fifo_to_buf(PCD_HandleTypeDef *hpcd, uint16_t pipe)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
    /* Check FIFO access sequence */
    switch (USBPhyHw_read_data(hpcd, pipe)) {
        case USB_READING :                                      /* Continue of data read */
            break;
        case USB_READEND :                                      /* End of data read */
            data_end(hpcd, pipe, (uint16_t)USB_DATA_OK);
            hpcd->pipe_ctrl [pipe].req_size -= hpcd->pipe_ctrl [pipe].data_cnt;
            break;
        case USB_READSHRT :                                     /* End of data read */
            data_end(hpcd, pipe, (uint16_t)USB_DATA_SHT);
            hpcd->pipe_ctrl [pipe].req_size -= hpcd->pipe_ctrl [pipe].data_cnt;
            break;
        case USB_READOVER :                                     /* Buffer over */
            USBPhyHw_forced_termination(hpcd, pipe, (uint16_t)USB_DATA_OVR);
            hpcd->pipe_ctrl [pipe].req_size -= hpcd->pipe_ctrl [pipe].data_cnt;
            break;
        case USB_FIFOERROR :                                    /* FIFO access error */
        default:
            USBPhyHw_forced_termination(hpcd, pipe, (uint16_t)USB_DATA_ERR);
            break;
    }
}

uint8_t *USBPhyHw_write_fifo(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe, uint16_t count, uint8_t *write_p)
{
	//PRINTF("%s: count=%u, write_p=%p\n", __func__, count, write_p);
	volatile iodefine_reg32_t * const p_reg = USBPhyHw_get_fifo_reg(USBx, pipe);
    uint16_t even;
    uint16_t odd;

    USBPhyHw_set_mbw(USBx, pipe, USB_MBW_32);                                /* 32bit access */
    for (even = (uint16_t)(count >> 2); (even != 0); --even) {
        p_reg->UINT32 = *((uint32_t *)write_p);
        write_p += sizeof(uint32_t);
    }
    odd = count % 4;
    if (count < 4) {
        odd = count;
    }
    if ((odd & (uint16_t)0x0002u) != 0u) {
        USBPhyHw_set_mbw(USBx, pipe, USB_MBW_16);                            /* 16bit access */
        p_reg->UINT16[R_IO_H] = *((uint16_t *)write_p);
        write_p += sizeof(uint16_t);
    }
    if ((odd & (uint16_t)0x0001u) != 0u) {
        USBPhyHw_set_mbw(USBx, pipe, USB_MBW_8);                             /* 8bit access */
        p_reg->UINT8[R_IO_HH] = *write_p;
        write_p++;
    }

    return write_p;
}

uint16_t USBPhyHw_write_data(PCD_HandleTypeDef *hpcd, uint16_t pipe)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
    uint16_t size;
    uint16_t count;
    uint16_t mxps;
    uint16_t end_flag;
    uint16_t buffer;

    /* Changes FIFO port by the pipe. */
    if (pipe == USB_PIPE0) {
        buffer = USBPhyHw_is_set_frdy(USBx, pipe, USB_ISEL_WRITE);
    } else {
        buffer = USBPhyHw_is_set_frdy(USBx, pipe, 0);
    }

    if (buffer == USB_FIFOERROR) {
        return (USB_FIFOERROR);
    }

    if (pipe == USB_PIPE0) {
        /* Max Packet Size */
        mxps = (uint16_t)(USBx->DCPMAXP & USB_MAXP);

        /* Data buffer size */
        if ((USBx->DCPCFG & USB_CNTMDFIELD) == USB_CFG_CNTMDON) {
            size = USB_PIPE0BUF;
        } else {
            size = mxps;
        }
    } else {
        /* Max Packet Size */
        USBx->PIPESEL = pipe;                          /* Pipe select */
        mxps = (uint16_t)(USBx->PIPEMAXP & USB_MXPS);

        /* Data buffer size */
        if ((USBx->PIPECFG & USB_CNTMDFIELD) == USB_CFG_CNTMDON) {
            size = (uint16_t)((uint16_t)((USBx->PIPEBUF >> USB_BUFSIZE_BIT) + 1) * USB_PIPEXBUF);
        } else {
            size = mxps;
        }
    }

    /* Data size check */
    if (hpcd->pipe_ctrl [pipe].data_cnt <= (uint32_t)size) {
        count = (uint16_t)hpcd->pipe_ctrl [pipe].data_cnt;
        if (count == 0) {
            end_flag = USB_WRITESHRT;                   /* Null Packet is end of write */
        } else if ((count % mxps) != 0) {
            end_flag = USB_WRITESHRT;                   /* Short Packet is end of write */
        } else {
            end_flag = USB_WRITEEND;                    /* Just Send Size */
        }
    } else {
        /* Write continues */
        end_flag = USB_WRITING;
        count = size;
    }

    hpcd->pipe_ctrl [pipe].p_data = USBPhyHw_write_fifo(USBx, pipe, count, hpcd->pipe_ctrl [pipe].p_data);

    /* Check data count to remain */
    if (hpcd->pipe_ctrl [pipe].data_cnt < (uint32_t)size) {
        volatile uint16_t * const p_reg = USBPhyHw_get_fifoctr_reg(USBx, pipe);

    	hpcd->pipe_ctrl [pipe].data_cnt = 0u;                  /* Clear data count */

        if ((*p_reg & USB_BVAL) == 0u) {                /* Check BVAL */
            *p_reg |= USB_BVAL;                         /* Short Packet */
        }
    } else {
    	hpcd->pipe_ctrl [pipe].data_cnt -= count;              /* Total data count - count */
    }

    return end_flag;
}

void USBPhyHw_buf_to_fifo(PCD_HandleTypeDef *hpcd, uint16_t pipe)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
    /* Disable Ready Interrupt */
    USBx->BRDYENB &= (~(1 << pipe));

    /* Peripheral control sequence */
    switch (USBPhyHw_write_data(hpcd, pipe)) {
        case USB_WRITING :                          /* Continue of data write */
            USBx->BRDYENB |= (1 << pipe);          /* Enable Ready Interrupt */
            USBx->NRDYENB |= (1 << pipe);          /* Enable Not Ready Interrupt */
            break;
        case USB_WRITEEND :                         /* End of data write */
        case USB_WRITESHRT :                        /* End of data write */
            USBx->BEMPENB |= (1 << pipe);          /* Enable Empty Interrupt */
            USBx->NRDYENB |= (1 << pipe);          /* Enable Not Ready Interrupt */
            break;
        case USB_FIFOERROR :                        /* FIFO access error */
        default:
            USBPhyHw_forced_termination(hpcd, pipe, (uint16_t)USB_DATA_ERR);
            break;
    }
}

volatile uint16_t *USBPhyHw_get_pipectr_reg(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe)
{
    if (pipe == USB_PIPE0) {
        return & (USBx->DCPCTR);
    } else {
        return & (USBx->PIPE1CTR) + (pipe - USB_PIPE1);
    }
}

volatile uint16_t *USBPhyHw_get_pipetre_reg(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe)
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

volatile uint16_t *USBPhyHw_get_pipetrn_reg(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe)
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

volatile uint16_t *USBPhyHw_get_fifoctr_reg(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe)
{
    uint16_t fifo_use = USBPhyHw_PIPE2FIFO(pipe);

    if ((fifo_use & USB_FUNCTION_D0FIFO_USE) == USB_FUNCTION_D0FIFO_USE) {
        return & (USBx->D0FIFOCTR);
    } else if ((fifo_use & USB_FUNCTION_D1FIFO_USE) == USB_FUNCTION_D1FIFO_USE) {
        return & (USBx->D1FIFOCTR);
    } else {
        return & (USBx->CFIFOCTR);
    }
}

volatile uint16_t *USBPhyHw_get_fifosel_reg(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe)
{
    uint16_t fifo_use = USBPhyHw_PIPE2FIFO(pipe);

    if ((fifo_use & USB_FUNCTION_D0FIFO_USE) == USB_FUNCTION_D0FIFO_USE) {
        return & (USBx->D0FIFOSEL);
    } else if ((fifo_use & USB_FUNCTION_D1FIFO_USE) == USB_FUNCTION_D1FIFO_USE) {
        return & (USBx->D1FIFOSEL);
    } else {
        return & (USBx->CFIFOSEL);
    }
}

volatile iodefine_reg32_t *USBPhyHw_get_fifo_reg(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe)
{
    uint16_t fifo_use = USBPhyHw_PIPE2FIFO(pipe);

    if ((fifo_use & USB_FUNCTION_D0FIFO_USE) == USB_FUNCTION_D0FIFO_USE) {
        return & (USBx->D0FIFO);
    } else if ((fifo_use & USB_FUNCTION_D1FIFO_USE) == USB_FUNCTION_D1FIFO_USE) {
        return & (USBx->D1FIFO);
    } else {
        return & (USBx->CFIFO);
    }
}

uint16_t USBPhyHw_get_pid(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe)
{
    volatile uint16_t *p_reg;

    p_reg = USBPhyHw_get_pipectr_reg(USBx, pipe);
    return (uint16_t)(*p_reg & USB_PID);
}

void USBPhyHw_set_mbw(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe, uint16_t data)
{
    volatile uint16_t *p_reg;

    p_reg = USBPhyHw_get_fifosel_reg(USBx, pipe);
    *p_reg &= (~USB_MBW);
    if (data != 0) {
        *p_reg |= data;
    }
}

void USBPhyHw_set_pid(USB_OTG_GlobalTypeDef * USBx, uint16_t pipe, uint16_t new_pid)
{
    volatile uint16_t *p_reg;
    uint16_t old_pid;

    p_reg = USBPhyHw_get_pipectr_reg(USBx, pipe);
    old_pid = USBPhyHw_get_pid(USBx, pipe);

    switch (new_pid) {
        case USB_PID_STALL:
            if ((old_pid & USB_PID_BUF) == USB_PID_BUF) {
                *p_reg &= (~USB_PID);
                *p_reg |= USB_PID_STALL2;
            } else {
                *p_reg &= (~USB_PID);
                *p_reg |= new_pid;
            }
            break;
        case USB_PID_BUF:
            if (((old_pid & USB_PID_STALL) == USB_PID_STALL) ||
                    ((old_pid & USB_PID_STALL2) == USB_PID_STALL2)) {
                *p_reg &= (~USB_PID);
                *p_reg |= USB_PID_NAK;
            }
            *p_reg &= (~USB_PID);
            *p_reg |= new_pid;
            break;
        case USB_PID_NAK:
            if ((old_pid & USB_PID_STALL2) == USB_PID_STALL2) {
                *p_reg &= (~USB_PID);
                *p_reg |= USB_PID_STALL;
            }
            *p_reg &= (~USB_PID);
            *p_reg |= new_pid;

            do {
                HARDWARE_DELAY_US(1);
                p_reg = USBPhyHw_get_pipectr_reg(USBx, pipe);
            } while ((*p_reg & USB_PBUSY) == USB_PBUSY);
            break;
        default:
            *p_reg &= (~USB_PID);
            *p_reg |= new_pid;
            break;
    }
}

#if 0

/* There are maintenance routine of SHTNAK and BFRE bits in original sample program.
* This sample is not programmed. Do maintenance the "def_pipecfg" array if you want it. */
static const struct PIPECFGREC {
    uint16_t    endpoint;
    uint16_t    pipesel;
    uint16_t    pipecfg;
    uint16_t    pipebuf;
    uint16_t    pipemaxp;
    uint16_t    pipeperi;
} def_pipecfg[] = {
    /* EP0OUT and EP0IN are configured by USB IP */
    {
        EP1OUT, /* EP1: Host -> Func, INT */
        6 | USB_FUNCTION_D0FIFO_USE,
        USB_TYPFIELD_INT  | USB_BFREOFF | USB_CFG_DBLBOFF | USB_CFG_CNTMDON  |                   USB_DIR_P_OUT | 1,
        USB_BUF_SIZE(64) | 0x04u,
        MAX_PACKET_SIZE_EP1,
        3,
    },
    {
        EP1IN,  /* EP1: Host <- Func, INT */
        7 | USB_FUNCTION_D1FIFO_USE,
        USB_TYPFIELD_INT  | USB_BFREOFF | USB_CFG_DBLBOFF | USB_CFG_CNTMDOFF |                   USB_DIR_P_IN  | 1,
        USB_BUF_SIZE(64) | 0x05u,
        MAX_PACKET_SIZE_EP1,
        3,
    },
    {
        EP2OUT, /* EP2: Host -> Func, BULK */
        3 | USB_FUNCTION_D0FIFO_USE,
        USB_TYPFIELD_BULK | USB_BFREOFF | USB_CFG_DBLBON  | USB_CFG_CNTMDON  | USB_SHTNAKFIELD | USB_DIR_P_OUT | 2,
        USB_BUF_SIZE(2048) | 0x30u,
        MAX_PACKET_SIZE_EP2,
        0,
    },
    {
        EP2IN,  /* EP2: Host <- Func, BULK */
        4 | USB_FUNCTION_D1FIFO_USE,
        USB_TYPFIELD_BULK | USB_BFREOFF | USB_CFG_DBLBOFF | USB_CFG_CNTMDON  |                   USB_DIR_P_IN  | 2,
        USB_BUF_SIZE(2048) | 0x50u,
        MAX_PACKET_SIZE_EP2,
        0,
    },
    {
        EP3OUT, /* EP3: Host -> Func, ISO */
        1 | USB_FUNCTION_D0FIFO_USE,
        USB_TYPFIELD_ISO  | USB_BFREOFF | USB_CFG_DBLBON  | USB_CFG_CNTMDOFF | USB_SHTNAKFIELD | USB_DIR_P_OUT | 3,
        USB_BUF_SIZE(512) | 0x10u,
        MAX_PACKET_SIZE_EP3,
        0,
    },
    {
        EP3IN,  /* EP3: Host <- Func, ISO */
        2 | USB_FUNCTION_D1FIFO_USE,
        USB_TYPFIELD_ISO  | USB_BFREOFF | USB_CFG_DBLBON  | USB_CFG_CNTMDOFF |                    USB_DIR_P_IN  | 3,
        USB_BUF_SIZE(512) | 0x20u,
        MAX_PACKET_SIZE_EP3,
        0,
    },
    { /* terminator */
        0, 0, 0, 0, 0, 0
    },
};


int USBPhyHw_endpoint_add(USB_OTG_GlobalTypeDef * USBx, usb_ep_t endpoint, uint32_t max_packet, usb_ep_type_t type)
{
    const struct PIPECFGREC *cfg;
    uint16_t pipe;
    volatile uint16_t *p_reg;

    if ((endpoint == EP0OUT) || (endpoint == EP0IN)) {
        return 1;
    }

    for (cfg = &def_pipecfg[0]; cfg->pipesel != 0; cfg++) {
        if (cfg->endpoint == endpoint) {
            break;
        }
    }
    if (cfg->pipesel == 0) {
        return 0;
    }

    pipe = (cfg->pipesel & USB_CURPIPE);

    /* Interrupt Disable */
    USBx->BRDYENB &= (~(1 << pipe));   /* Disable Ready Interrupt */
    USBx->NRDYENB &= (~(1 << pipe));   /* Disable Not Ready Interrupt */
    USBx->BEMPENB &= (~(1 << pipe));   /* Disable Empty Interrupt */

    USBPhyHw_set_pid(USBx, pipe, USB_PID_NAK);

    /* CurrentPIPE Clear */
    if ((USBx->CFIFOSEL & USB_CURPIPE) == pipe) {
        USBx->CFIFOSEL &= ~USB_CURPIPE;
    }
    if ((USBx->D0FIFOSEL & USB_CURPIPE) == pipe) {
        USBx->D0FIFOSEL &= ~USB_CURPIPE;
    }
    if ((USBx->D1FIFOSEL & USB_CURPIPE) == pipe) {
        USBx->D1FIFOSEL &= ~USB_CURPIPE;
    }

    /* PIPE Configuration */
    USBx->PIPESEL  = pipe;             /* Pipe select */
    USBx->PIPECFG  = cfg->pipecfg;
    USBx->PIPEBUF  = cfg->pipebuf;
    USBx->PIPEMAXP = cfg->pipemaxp;
    USBx->PIPEPERI = cfg->pipeperi;

    p_reg = USBPhyHw_get_pipectr_reg(USBx, pipe);
    /* Set toggle bit to DATA0 */
    *p_reg |= USB_SQCLR;
    /* Buffer Clear */
    *p_reg |= USB_ACLRM;
    *p_reg &= ~USB_ACLRM;

    return 1;
}

void USBPhyHw_endpoint_remove(USB_OTG_GlobalTypeDef * USBx, usb_ep_t endpoint)
{
    uint16_t pipe = USBPhyHw_EP2PIPE(endpoint);

    /* Interrupt Disable */
    USBx->BRDYENB &= (~(1 << pipe));   /* Disable Ready Interrupt */
    USBx->NRDYENB &= (~(1 << pipe));   /* Disable Not Ready Interrupt */
    USBx->BEMPENB &= (~(1 << pipe));   /* Disable Empty Interrupt */

    USBPhyHw_set_pid(USBx, pipe, USB_PID_NAK);

    /* CurrentPIPE Clear */
    if ((USBx->CFIFOSEL & USB_CURPIPE) == pipe) {
        USBx->CFIFOSEL &= ~USB_CURPIPE;
    }
    if ((USBx->D0FIFOSEL & USB_CURPIPE) == pipe) {
        USBx->D0FIFOSEL &= ~USB_CURPIPE;
    }
    if ((USBx->D1FIFOSEL & USB_CURPIPE) == pipe) {
        USBx->D1FIFOSEL &= ~USB_CURPIPE;
    }

    /* PIPE Configuration */
    USBx->PIPESEL = pipe;              /* Pipe select */
    USBx->PIPECFG = 0;

    pipe_ctrl [pipe].enable = 0;
    pipe_ctrl [pipe].status = USB_DATA_NONE;
}

uint16_t USBPhyHw_EP2PIPE(uint16_t endpoint)
{
    const struct PIPECFGREC *cfg;

    for (cfg = &def_pipecfg[0]; cfg->pipesel != 0; cfg++) {
        if (cfg->endpoint == endpoint) {
            break;
        }
    }
    return (cfg->pipesel & USB_CURPIPE);
}

uint16_t USBPhyHw_PIPE2EP(uint16_t pipe)
{
    const struct PIPECFGREC *cfg;

    if (pipe == USB_PIPE0) {
        return 0;
    }
    for (cfg = &def_pipecfg[0]; cfg->pipesel != 0; cfg++) {
        if ((cfg->pipesel & USB_CURPIPE) == pipe) {
            break;
        }
    }
    return cfg->endpoint;
}

uint16_t USBPhyHw_PIPE2FIFO(uint16_t pipe)
{
    const struct PIPECFGREC *cfg;
    uint16_t fifo_use;

    if (pipe == USB_PIPE0) {
        fifo_use = USB_FUNCTION_CFIFO_USE;
    } else {
        for (cfg = &def_pipecfg[0]; cfg->pipesel != 0; cfg++) {
            if ((cfg->pipesel & USB_CURPIPE) == pipe) {
                break;
            }
        }
        if ((cfg->pipesel & USB_FUNCTION_D0FIFO_USE) == USB_FUNCTION_D0FIFO_USE) {
            fifo_use = USB_FUNCTION_D0FIFO_USE;
        } else if ((cfg->pipesel & USB_FUNCTION_D1FIFO_USE) == USB_FUNCTION_D1FIFO_USE) {
            fifo_use = USB_FUNCTION_D1FIFO_USE;
        } else {
            fifo_use = USB_FUNCTION_CFIFO_USE;
        }
    }

    return fifo_use;
}
#else

uint16_t USBPhyHw_PIPE2FIFO(uint16_t pipe)
{
    //const struct PIPECFGREC *cfg;
    uint16_t fifo_use;

    if (pipe == USB_PIPE0) {
        fifo_use = USB_FUNCTION_CFIFO_USE;
    } else {
    	switch (pipe) {
#if WITHDMAHW_UACIN
    	case HARDWARE_USBD_PIPE_ISOC_IN:
            fifo_use = USB_FUNCTION_D1FIFO_USE;
            break;
#endif /* WITHDMAHW_UACIN */
#if WITHDMAHW_UACOUT
    	case HARDWARE_USBD_PIPE_ISOC_OUT:
            fifo_use = USB_FUNCTION_D0FIFO_USE;
            break;
#endif /* WITHNDMA_UACOUT */
    	default:
            fifo_use = USB_FUNCTION_CFIFO_USE;
            break;
    	}
//        for (cfg = &def_pipecfg[0]; cfg->pipesel != 0; cfg++) {
//            if ((cfg->pipesel & USB_CURPIPE) == pipe) {
//                break;
//            }
//        }
//        if ((cfg->pipesel & USB_FUNCTION_D0FIFO_USE) == USB_FUNCTION_D0FIFO_USE) {
//            fifo_use = USB_FUNCTION_D0FIFO_USE;
//        } else if ((cfg->pipesel & USB_FUNCTION_D1FIFO_USE) == USB_FUNCTION_D1FIFO_USE) {
//            fifo_use = USB_FUNCTION_D1FIFO_USE;
//        } else {
//            fifo_use = USB_FUNCTION_CFIFO_USE;
//        }
    }

    return fifo_use;
}

#endif

void USBPhyHw_reset_usb(USB_OTG_GlobalTypeDef * USBx, uint16_t clockmode)
{
    if (USBx == & USB200) {
        if ((USB200.SYSCFG0 & USB_UPLLE) == USB_UPLLE) {
            if ((USB200.SYSCFG0 & USB_UCKSEL) != clockmode) {
                USB200.SUSPMODE &= ~(USB_SUSPMODE_SUSPM);
                USB200.SYSCFG0 = 0;
                USB200.SYSCFG0 = (clockmode | USB_UPLLE);
                HARDWARE_DELAY_US(1000);
                USB200.SUSPMODE |= USB_SUSPMODE_SUSPM;
            } else {
                USB200.SUSPMODE &= ~(USB_SUSPMODE_SUSPM);
                HARDWARE_DELAY_US(1000);
                USB200.SUSPMODE |= USB_SUSPMODE_SUSPM;
            }
        } else {
            USB200.SUSPMODE &= ~(USB_SUSPMODE_SUSPM);
            USB200.SYSCFG0 = 0;
            USB200.SYSCFG0 = (clockmode | USB_UPLLE);
            HARDWARE_DELAY_US(1000);
            USB200.SUSPMODE |= USB_SUSPMODE_SUSPM;
        }
	} else if (USBx == & USB201) {
	    /* UCKSEL and UPLLE bit is only USB0. If USB1, set to SYSCFG0 for USB0. */
	    if ((USB200.SYSCFG0 & USB_UPLLE) == USB_UPLLE) {
	        if ((USB200.SYSCFG0 & USB_UCKSEL) != clockmode) {
	            USB201.SUSPMODE &= ~(USB_SUSPMODE_SUSPM);
	            USB200.SUSPMODE &= ~(USB_SUSPMODE_SUSPM);
	            USB201.SYSCFG0 = 0;
	            USB200.SYSCFG0 = 0;
	            USB200.SYSCFG0 = (clockmode | USB_UPLLE);
	            HARDWARE_DELAY_US(1000);
	            USB200.SUSPMODE |= USB_SUSPMODE_SUSPM;
	            USB201.SUSPMODE |= USB_SUSPMODE_SUSPM;
	        } else {
	            USB201.SUSPMODE &= ~(USB_SUSPMODE_SUSPM);
	            HARDWARE_DELAY_US(1000);
	            USB201.SUSPMODE |= USB_SUSPMODE_SUSPM;
	        }
	    } else {
	        USB201.SUSPMODE &= ~(USB_SUSPMODE_SUSPM);
	        USB200.SUSPMODE &= ~(USB_SUSPMODE_SUSPM);
	        USB201.SYSCFG0 = 0;
	        USB200.SYSCFG0 = 0;
	        USB200.SYSCFG0 = (clockmode | USB_UPLLE);
	        HARDWARE_DELAY_US(1000);
	        USB200.SUSPMODE |= USB_SUSPMODE_SUSPM;
	        USB201.SUSPMODE |= USB_SUSPMODE_SUSPM;
	    }
	}
	// P1 clock (66.7 MHz max) period = 15 ns
	// The cycle period required to consecutively access registers of this controller must be at least 67 ns.
	// TODO: compute BWAIT value on-the-fly
	// Use P1CLOCK_FREQ
	const uint_fast32_t bwait = MIN(MAX(calcdivround2(P1CLOCK_FREQ, 15000000uL), 2) - 2, 63);
	USBx->BUSWAIT = (bwait << USB_BUSWAIT_BWAIT_SHIFT) & USB_BUSWAIT_BWAIT;	// 5 cycles = 75 nS minimum
	(void) USBx->BUSWAIT;
    //USBx->BUSWAIT = (uint16_t)(USB_BWAIT_3 & USB_BWAIT);   /* 3 : 5 access cycles  waits */

    USBx->CFIFOSEL  = USB_MBW_32;
    USBx->D0FIFOSEL = USB_MBW_32;
    USBx->D1FIFOSEL = USB_MBW_32;
}

int USBPhyHw_chk_vbsts(USB_OTG_GlobalTypeDef * USBx)
{
    uint16_t buf1;
    uint16_t buf2;
    uint16_t buf3;
    int connect_flg = 0;

    /* VBUS chattering cut */
    do {
        buf1 = USBx->INTSTS0;
        HARDWARE_DELAY_US(10);
        buf2 = USBx->INTSTS0;
        HARDWARE_DELAY_US(10);
        buf3 = USBx->INTSTS0;
    } while (((buf1 & USB_VBSTS) != (buf2 & USB_VBSTS)) || ((buf2 & USB_VBSTS) != (buf3 & USB_VBSTS)));

    /* VBUS status judge */
    if ((buf1 & USB_VBSTS) != (uint16_t)0) {
        connect_flg = 1;
    }

    return connect_flg;
}

void USBPhyHw_ctrl_end(USB_OTG_GlobalTypeDef * USBx, uint16_t status)
{
    /* Interrupt disable */
    USBx->BEMPENB &= (~(1 << USB_PIPE0));  /* Disable Empty Interrupt */
    USBx->BRDYENB &= (~(1 << USB_PIPE0));  /* Disable Ready Interrupt */
    USBx->NRDYENB &= (~(1 << USB_PIPE0));  /* Disable Not Ready Interrupt */

    USBPhyHw_set_mbw(USBx, USB_PIPE0, USB_MBW_32);

    if ((status == USB_DATA_ERR) || (status == USB_DATA_OVR)) {
        USBPhyHw_set_pid(USBx, USB_PIPE0, USB_PID_STALL);  /* Request error */
    } else if (status == USB_DATA_STOP) {
        USBPhyHw_set_pid(USBx, USB_PIPE0, USB_PID_NAK);    /* Pipe stop */
    } else {
        USBx->DCPCTR |= USB_CCPL;          /* Set CCPL bit */
    }
}

void USBPhyHw_data_end(PCD_HandleTypeDef *hpcd, uint16_t pipe, uint16_t status)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
    volatile uint16_t *p_reg;

    /* Disable Interrupt */
    USBx->BRDYENB &= (~(1 << pipe));           /* Disable Ready Interrupt */
    USBx->NRDYENB &= (~(1 << pipe));           /* Disable Not Ready Interrupt */
    USBx->BEMPENB &= (~(1 << pipe));           /* Disable Empty Interrupt */

    USBPhyHw_set_pid(USBx, pipe, USB_PID_NAK);                 /* Set NAK */

    /* Disable Transaction count */
    p_reg = USBPhyHw_get_pipetre_reg(USBx, pipe);
    if (p_reg != NULL) {
        *p_reg &= (~USB_TRENB);
        *p_reg |= USB_TRCLR;
    }

    if (hpcd->pipe_ctrl [pipe].enable) {
        /* Check PIPE TYPE */
        USBx->PIPESEL = pipe;                  /* Pipe select */
        if ((USBx->PIPECFG & USB_TYPFIELD) != USB_TYPFIELD_ISO) {
            /* Transfer information set */
        	hpcd->pipe_ctrl [pipe].enable = 0;
        	hpcd->pipe_ctrl [pipe].status = status;
        } else if ((uint16_t)(USBx->PIPECFG & USB_DIRFIELD) == USB_BUF2FIFO) {
            /* ISO OUT Transfer (restart) */
        	hpcd->pipe_ctrl [pipe].status = USB_DATA_WRITING;
        } else {
            /* ISO IN Transfer (restart) */
        	hpcd->pipe_ctrl [pipe].status = USB_DATA_READING;
        }
    }
}

void USBPhyHw_forced_termination(PCD_HandleTypeDef *hpcd, uint16_t pipe, uint16_t status)
{
	USB_OTG_GlobalTypeDef * const USBx = hpcd->Instance;
    volatile uint16_t *p_reg;

    /* Disable Interrupt */
    USBx->BRDYENB &= (~(1 << pipe));           /* Disable Ready Interrupt */
    USBx->NRDYENB &= (~(1 << pipe));           /* Disable Not Ready Interrupt */
    USBx->BEMPENB &= (~(1 << pipe));           /* Disable Empty Interrupt */

    USBPhyHw_set_pid(USBx, pipe, USB_PID_NAK);                 /* Set NAK */

    /* Disable Transaction count */
    p_reg = USBPhyHw_get_pipetre_reg(USBx, pipe);
    if (p_reg != NULL) {
        *p_reg &= (~USB_TRENB);
        *p_reg |= USB_TRCLR;
    }

    USBPhyHw_set_mbw(USBx, pipe, USB_MBW_32);

    USBPhyHw_chg_curpipe(USBx, pipe, 0);                   /* Changes the FIFO port by the pipe. */

    p_reg = USBPhyHw_get_pipectr_reg(USBx, pipe);
    /* Buffer Clear */
    *p_reg |= USB_ACLRM;
    *p_reg &= ~USB_ACLRM;

    hpcd->pipe_ctrl [pipe].enable = 0;
    hpcd->pipe_ctrl [pipe].status  = status;
}


////////////////////////////////
static uint_fast8_t
usbd_getpipe(const USB_OTG_EPTypeDef * ep)
{
	if (ep->is_in)
		return USBPhyHw_EP2PIPE(ep->num | 0x80);
	else
		return USBPhyHw_EP2PIPE(ep->num);
}


#if WITHDMAHW_UACIN

// Сейчас эта память будет записываться по DMA куда-то
// Потом содержимое не требуется
static uintptr_t
dma_flushxrtstx(uintptr_t addr, unsigned long size)
{
	dcache_clean_invalidate(addr, size);
	return addr;
}

// USB AUDIO
// DMA по передаче USB0 DMA1 - обработчик прерывания
// DMA по передаче USB1 DMA1 - обработчик прерывания
// Use dcache_clean
// Работает на ARM_REALTIME_PRIORITY
static void RAMFUNC_NONILINE r7s721_usbX_dma1_dmatx_handler(void)
{
#error To be complete for DMA
	DMAC12.CHCFG_n |= DMAC12_CHCFG_n_REN;	// REN bit
	//__DMB();
	// SR (bt 7)
	// Indicates the register set currently selected in register mode.
	// 0: Next0 Register Set
	// 1: Next1 Register Set
	const uint_fast8_t b = (DMAC12.CHSTAT_n & DMAC12_CHSTAT_n_SR) != 0;	// SR
	if (b != 0)
	{
		uint_fast16_t size;
		const uintptr_t addr = getfilled_dmabufferuacinX(& size);	// для передачи в компьютер

		const uintptr_t wasaddr = DMAC12.N0SA_n;
		DMAC12.N0TB_n = size;
		DMAC12.N0SA_n = dma_flushxrtstx(addr, size);
		release_dmabufferuacinX(wasaddr);
	}
	else
	{
		uint_fast16_t size;
		const uintptr_t addr = getfilled_dmabufferuacinX(& size);	// для передачи в компьютер

		const uintptr_t wasaddr = DMAC12.N1SA_n;
		DMAC12.N1TB_n = size;
		DMAC12.N1SA_n = dma_flushxrtstx(addr, size);
		release_dmabufferuacinX(wasaddr);
	}

}


// audio codec
// DMA по передаче USB0 DMA1
// Use dcache_clean

static void r7s721_usb0_dma1_dmatx_initialize(uint_fast8_t pipe)
{
#error To be complete for DMA
	USB_OTG_GlobalTypeDef * const USBx = & USB200;

	enum { id = 12 };	// 12: DMAC12
	// DMAC12
	/* Set Source Start Address */

    /* Set Destination Start Address */
    DMAC12.N0DA_n = (uintptr_t) & USBx->D1FIFO.UINT32;	// Fixed destination address
    DMAC12.N1DA_n = (uintptr_t) & USBx->D1FIFO.UINT32;	// Fixed destination address

    /* Set Transfer Size */
    DMAC12.N0TB_n = UACIN_AUDIO48_DATASIZE_DMAC;	// размер в байтах
    DMAC12.N1TB_n = UACIN_AUDIO48_DATASIZE_DMAC;	// размер в байтах

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
	DMAC12.CHCTRL_n = 1 * (1U << 0);		// SETEN
}

// audio codec
// DMA по передаче USB1 DMA1
// Use dcache_clean

static void r7s721_usb1_dma1_dmatx_initialize(uint_fast8_t pipe)
{
#error To be complete for DMA
	USB_OTG_GlobalTypeDef * const USBx = & USB201;

	enum { id = 12 };	// 12: DMAC12
	// DMAC12
	/* Set Source Start Address */

    /* Set Destination Start Address */
    DMAC12.N0DA_n = (uintptr_t) & USBx->D1FIFO.UINT32;	// Fixed destination address
    DMAC12.N1DA_n = (uintptr_t) & USBx->D1FIFO.UINT32;	// Fixed destination address

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

#endif /* WITHDMAHW_UACIN */

#if WITHDMAHW_UACOUT


static uintptr_t dma_invalidateuacout48(uintptr_t addr)
{
	ASSERT((addr % DCACHEROWSIZE) == 0);
	ASSERT((cachesize_dmabufferuacout48() % DCACHEROWSIZE) == 0);
	dcache_invalidate(addr, cachesize_dmabufferuacout48());
	return addr;
}

// USB AUDIO
// DMA по приему USB0 DMA0 - обработчик прерывания
// DMA по приему USB1 DMA0 - обработчик прерывания
// Работает на ARM_REALTIME_PRIORITY
static RAMFUNC_NONILINE void r7s721_usbX_dma0_dmarx_handler(void)
{
#error To be complete for DMA
	//__DMB();
	// SR (bt 7)
	// Indicates the register set currently selected in register mode.
	// 0: Next0 Register Set
	// 1: Next1 Register Set
	const uint_fast8_t b = (DMAC13.CHSTAT_n & (UINT32_C(1) << DMAC13_CHSTAT_n_SR_SHIFT)) != 0;	// SR
	if (b != 0)
	{
		const uintptr_t addr = DMAC13.N0DA_n;
		DMAC13.N0DA_n = dma_invalidateuacout48(allocate_dmabufferuacout48());
		DMAC13.CHCFG_n |= DMAC13_CHCFG_n_REN;	// REN bit
		save_dmabufferuacout48(addr);
	}
	else
	{
		const uintptr_t addr = DMAC13.N1DA_n;
		DMAC13.N1DA_n = dma_invalidateuacout48(allocate_dmabufferuacout48());
		DMAC13.CHCFG_n |= DMAC13_CHCFG_n_REN;	// REN bit
		save_dmabufferuacout48(addr);
	}

}


// USB AUDIO
// DMA по приёму usb0_dma0
static void r7s721_usb0_dma0_dmarx_initialize(uint_fast8_t pipe)
{
	USB_OTG_GlobalTypeDef * const USBx = & USB200;

	unsigned uacinsize0;
	uintptr_t uacinaddr0 = getfilled_dmabufferuacinX(& uacinsize0);
	unsigned uacinsize1;
	uintptr_t uacinaddr1 = getfilled_dmabufferuacinX(& uacinsize1);

	dcache_clean_invalidate(uacinaddr0, uacinsize0);
	dcache_clean_invalidate(uacinaddr1, uacinsize1);

	enum { id = 13 };	// 13: DMAC13
	// DMAC13
	/* Set Source Start Address */
	/* регистры USB PIPE (HARDWARE_USBD_PIPE_ISOC_OUT) */
    DMAC13.N0SA_n = (uintptr_t) & USBx->D0FIFO.UINT32;	// Fixed source address
    DMAC13.N1SA_n = (uintptr_t) & USBx->D0FIFO.UINT32;	// Fixed source address

	/* Set Destination Start Address */
	DMAC13.N0DA_n = uacinaddr0;
	DMAC13.N1DA_n = uacinaddr1;

    /* Set Transfer Size */
    DMAC13.N0TB_n = uacinsize0;	// размер в байтах
    DMAC13.N1TB_n = uacinsize1;	// размер в байтах

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

	enum { id = 13 };	// 13: DMAC13
	// DMAC13
	/* Set Source Start Address */
	/* регистры USB PIPE (HARDWARE_USBD_PIPE_ISOC_OUT) */
    DMAC13.N0SA_n = (uintptr_t) & USBx->D0FIFO.UINT32;	// Fixed source address
    DMAC13.N1SA_n = (uintptr_t) & USBx->D0FIFO.UINT32;	// Fixed source address

	/* Set Destination Start Address */
	DMAC13.N0DA_n = dma_invalidateuacout48(allocate_dmabufferuacout48());
	DMAC13.N1DA_n = dma_invalidateuacout48(allocate_dmabufferuacout48());

    /* Set Transfer Size */
    DMAC13.N0TB_n = UACOUT_AUDIO48_DATASIZE_DMAC;	// размер в байтах
    DMAC13.N1TB_n = UACOUT_AUDIO48_DATASIZE_DMAC;	// размер в байтах

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


#define USBD_FRDY_COUNT_WRITE 10
#define USBD_FRDY_COUNT_READ 10

static uint_fast8_t usbd_wait_fifo(
		PCD_TypeDef * const USBx,
		uint_fast8_t pipe, 	// expected pipe
		uint_fast8_t isel,	// expected isel: 1: Writing to the DCP buffer memory is selected
		unsigned waitcnt
		)
{
	volatile uint16_t * const PIPEnCTR = USBPhyHw_get_pipectr_reg(USBx, pipe);
	while (
			(* PIPEnCTR & USB_DCPCTR_BSTS) == 0 ||
			(USBx->CFIFOSEL & USB_CFIFOSEL_ISEL_) != (isel << USB_CFIFOSEL_ISEL_SHIFT_) ||
			(USBx->CFIFOSEL & USB_CFIFOSEL_CURPIPE) != (pipe << USB_CFIFOSEL_CURPIPE_SHIFT) ||
			(USBx->CFIFOCTR & USB_CFIFOCTR_FRDY) == 0)	// FRDY
	{
		HARDWARE_DELAY_US(1);
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
		USBPhyHw_set_pid(USBx, 0, DEVDRV_USBF_PID_BUF);
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
    ep->dma_addr = (uint32_t)pBuf;
  }

  if ((ep_addr & EP_ADDR_MSK) == 0U)
  {
#if WITHNEWUSBHAL
	  USBPhyHw_ep0_read(hpcd, pBuf, len);
#else /* WITHNEWUSBHAL */
    (void)USB_EP0StartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
#endif /* WITHNEWUSBHAL */
  }
  else
  {
#if WITHNEWUSBHAL
	  USBPhyHw_endpoint_read(hpcd, ep_addr, pBuf, len);
#else /* WITHNEWUSBHAL */
    (void)USB_EPStartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
#endif /* WITHNEWUSBHAL */
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
    ep->dma_addr = (uint32_t)pBuf;
  }

  if ((ep_addr & EP_ADDR_MSK) == 0U)
  {
#if WITHNEWUSBHAL
	USBPhyHw_ep0_write(hpcd, (uint8_t *) pBuf, len);
#else /* WITHNEWUSBHAL */
    (void)USB_EP0StartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
#endif /* WITHNEWUSBHAL */
  }
  else
  {
#if WITHNEWUSBHAL
	  USBPhyHw_endpoint_write(hpcd, ep_addr, (uint8_t *) pBuf, len);
#else /* WITHNEWUSBHAL */
    (void)USB_EPStartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
#endif /* WITHNEWUSBHAL */
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

#if ! WITHNEWUSBHAL
  (void)USB_EPSetStall(hpcd->Instance, ep);
#else
  if ((ep_addr & EP_ADDR_MSK) == 0U)
  {
	  USBPhyHw_ep0_stall(hpcd);
  }
  else
  {
	  USBPhyHw_endpoint_stall(hpcd, ep_addr);

  }
#endif
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
#if ! WITHNEWUSBHAL
  (void)USB_EPClearStall(hpcd->Instance, ep);
#else
  if ((ep_addr & EP_ADDR_MSK) == 0U)
  {
	  USBPhyHw_endpoint_unstall(hpcd, 0);
  }
  else
  {
	  USBPhyHw_endpoint_unstall(hpcd, ep_addr);

  }
#endif
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
#if ! WITHNEWUSBHAL
// Renesas, usb device
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
{
	//__DMB();
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
					const uint_fast8_t pid = USBPhyHw_get_pid(USBx, pipe);
                    if (((pid & DEVDRV_USBF_PID_STALL) != DEVDRV_USBF_PID_STALL) && ((pid & DEVDRV_USBF_PID_STALL2) != DEVDRV_USBF_PID_STALL2)) {
                        USBPhyHw_set_pid(USBx, pipe, DEVDRV_USBF_PID_BUF);
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
				const uint_fast8_t epnt = USBPhyHw_PIPE2EP(pipe);
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
		case 0x00:
			// 000: Powered state
			break;
		case 0x01:
			// 001: Default state
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
		//PRINTF(PSTR("HAL_PCD_IRQHandler trapped - VBINT, VBSTS=%d\n"), (intsts0 & USB_INTSTS0_VBSTS) != 0);
	//	usbd_handle_vbuse(usbd_getvbus());
	}
	if (hpcd->run_later_ctrl_comp != 0)
	{
		USBx->DCPCTR &= ~ USB_DCPCTR_PID;
		USBx->DCPCTR |= DEVDRV_USBF_PID_BUF;	// CCPL
		USBx->DCPCTR |= USB_DCPCTR_CCPL;	// CCPL
	}
}

#endif /* ! WITHNEWUSBHAL */

// Renesas, usb host
void HAL_HCD_IRQHandler(HCD_HandleTypeDef *hhcd)
{
	//__DMB();
	USB_OTG_GlobalTypeDef * const USBx = hhcd->Instance;
	const uint_fast16_t intsts0 = USBx->INTSTS0;
	const uint_fast16_t intsts1 = USBx->INTSTS1;
	const uint_fast16_t intsts0msk = intsts0 & USBx->INTENB0;
	const uint_fast16_t intsts1msk = intsts1 & USBx->INTENB1;

	//PRINTF(PSTR("HAL_HCD_IRQHandler trapped, intsts0=%04X, intsts0msk=%04X, intsts1=%04X, intsts1msk=%04X\n"), intsts0, intsts0msk, intsts1, intsts1msk);
	//PRINTF(PSTR("HAL_HCD_IRQHandler trapped, hhcd=%p, USBx=%p, host=%p\n"), hhcd, USBx, hhcd->pData);
	if ((intsts0msk & USB_INTSTS0_SOFR) != 0)	// SOFR
	{
		USBx->INTSTS0 = (uint16_t) ~ USB_INTSTS0_SOFR;	// Clear SOFR
		//PRINTF(PSTR("HAL_HCD_IRQHandler trapped - SOFR\n"));
	}
	if ((intsts0msk & USB_INTSTS0_BEMP) != 0)	// BEMP
	{
		uint_fast8_t pipe;
		PRINTF(PSTR("HAL_HCD_IRQHandler trapped - BEMP, BEMPSTS=0x%04X\n"), USBx->BEMPSTS);
		const uint_fast16_t bempsts = USBx->BEMPSTS & USBx->BEMPENB;	// BEMP Interrupt Status Register
		USBx->BEMPSTS = ~ bempsts;
		for (pipe = 0; pipe < 16; ++ pipe)
		{
			const uint_fast16_t mask = (uint_fast16_t) 1 << pipe;
			if ((bempsts & mask) != 0)
			{
				USBH_HandleTypeDef * const phost = hhcd->pData;
				RZ_HCD_HCTypeDef * const hc = & hhcd->hc [pipe == 0 ? phost->Control.pipe_out : pipe];
				const unsigned bcnt = hc->xfer_len;
		  		hc->xfer_buff += bcnt;
		  		hc->xfer_count += bcnt;

				USBx->BEMPENB &= ~ (0x01uL << pipe);
				//HAL_PCD_DataOutStageCallback(hpcd, ep->num);	// start next transfer
		  		hc->toggle_in ^= 1;
		  		hc->state = HC_XFRC;
		  		hc->urb_state  = URB_DONE;
	  		}
		}

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
					const uint_fast8_t pid = USBPhyHw_get_pid(USBx, pipe);
                    if (((pid & DEVDRV_USBF_PID_STALL) != DEVDRV_USBF_PID_STALL) && ((pid & DEVDRV_USBF_PID_STALL2) != DEVDRV_USBF_PID_STALL2)) {
                        USBPhyHw_set_pid(USBx, pipe, DEVDRV_USBF_PID_BUF);
                    }
                }
			}
		}
#endif
	}
	if ((intsts0msk & USB_INTSTS0_BRDY) != 0)	// BRDY
	{
		// host
		uint_fast8_t pipe;
		uint_fast8_t i;
		PRINTF(PSTR("HAL_HCD_IRQHandler trapped - BRDY, BRDYSTS=0x%04X\n"), USBx->BRDYSTS);
		const uint_fast16_t brdysts = USBx->BRDYSTS & USBx->BRDYENB;	// BRDY Interrupt Status Register
		USBx->BRDYSTS = ~ brdysts;	// 2. When BRDYM is 0, clearing this bit should be done before accessing the FIFO.
		for (pipe = 0; pipe < 16; ++ pipe)
		{
			const uint_fast16_t mask = (uint_fast16_t) 1 << pipe;
			if ((brdysts & mask) != 0)
			{
				USBH_HandleTypeDef * const phost = hhcd->pData;
				RZ_HCD_HCTypeDef * const hc = & hhcd->hc [pipe == 0 ? phost->Control.pipe_in : pipe];
				unsigned bcnt;
				if (USB_ReadPacketNec(USBx, pipe, hc->xfer_buff, hc->xfer_len - hc->xfer_count, & bcnt) == 0)
				{
					if (bcnt == 0)
					{
						PRINTF("NO DATA. hc->xfer_len=%d, hc->xfer_count=%d\n", (int) hc->xfer_len, (int) hc->xfer_count);
					}
					else
					{
						printhex((uintptr_t) hc->xfer_buff, hc->xfer_buff, bcnt);	// DEBUG
						hc->xfer_buff += bcnt;
						hc->xfer_count += bcnt;
					}
					USBx->BRDYENB &= ~ (0x01uL << pipe);
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
		}

#if 0

		for (i = 1; i < 16; ++ i)
		{
			const uint_fast8_t pipe = i;
			if ((brdysts & (1U << pipe)) != 0)
			{
				const uint_fast8_t epnt = USBPhyHw_PIPE2EP(pipe);
				if ((epnt & 0x80) != 0)
				{
					RZ_HCD_HCTypeDef * const hc = & hhcd->hc [0];
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
	if ((intsts1msk & USB_INTSTS1_BCHG) != 0)	// BCHG - сейчас отключено.
	{
		USBx->INTSTS1 = (uint16_t) ~ USB_INTSTS1_BCHG;
		PRINTF(PSTR("HAL_HCD_IRQHandler trapped - BCHG\n"));
		if (HAL_HCD_GetCurrentSpeedReady(hhcd))
		{
			// куда поставить?
			HAL_HCD_PortEnabled_Callback(hhcd);
		}
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
		HAL_HCD_PortEnabled_Callback(hhcd);	// пока тут
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
		const unsigned ch_num = 0;
		USBx->INTSTS1 = (uint16_t) ~ USB_INTSTS1_SIGN;
		PRINTF(PSTR("HAL_HCD_IRQHandler trapped - SIGN\n"));
		//HAL_HCD_Connect_Callback(hhcd);

		USBH_HandleTypeDef * const phost = hhcd->pData;
		RZ_HCD_HCTypeDef * const hc = & hhcd->hc [phost->Control.pipe_out];

  		USBx->INTENB1 &= ~ (USB_INTENB1_SIGNE | USB_INTENB1_SACKE);

  		hc->state = HC_XFRC;
	    hc->ErrCnt = 1;
	    hc->toggle_in ^= 1;
	    hc->urb_state  = URB_STALL;		// setup stage fail

#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
      hhcd->HC_NotifyURBChangeCallback(hhcd, (uint8_t)ch_num, hhcd->hc[ch_num].urb_state);
#else
      HAL_HCD_HC_NotifyURBChange_Callback(hhcd, (uint8_t)ch_num, hhcd->hc[ch_num].urb_state);
#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */
	}
	if ((intsts1msk & USB_INTSTS1_SACK) != 0)	// SACK
	{
		const unsigned ch_num = 0;
		USBx->INTSTS1 = (uint16_t) ~ USB_INTSTS1_SACK;
		PRINTF(PSTR("HAL_HCD_IRQHandler trapped - SACK\n"));
		//HAL_HCD_Connect_Callback(hhcd);
		//int err = USB_WritePacketNec(USBx, 0, NULL, 0);	// pipe=0: DCP
		//ASSERT(err == 0);
		//HAL_HCD_Connect_Callback(hhcd);

		USBH_HandleTypeDef * const phost = hhcd->pData;
		RZ_HCD_HCTypeDef * const hc = & hhcd->hc [phost->Control.pipe_out];

  		USBx->INTENB1 &= ~ (USB_INTENB1_SIGNE | USB_INTENB1_SACKE);
	    hc->state = HC_XFRC;
	    hc->ErrCnt = 0;
	    hc->toggle_in ^= 1;
	    hc->urb_state = URB_DONE;	// SETUP stage complete

#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
      hhcd->HC_NotifyURBChangeCallback(hhcd, (uint8_t)ch_num, hhcd->hc[ch_num].urb_state);
#else
      HAL_HCD_HC_NotifyURBChange_Callback(hhcd, (uint8_t)ch_num, hhcd->hc[ch_num].urb_state);
#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */
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
	if (((USBx->DVSTCTR0 & USB_DVSTCTR0_RHST) >> USB_DVSTCTR0_RHST_SHIFT) & 0x04)
	{
		//PRINTF("USB_GetHostSpeedReady: Reset handshake in progress\n");
		return 0;
	}
	return 1;
	switch ((USBx->DVSTCTR0 & USB_DVSTCTR0_RHST) >> USB_DVSTCTR0_RHST_SHIFT)
	{
	case 0x01:
		//PRINTF("USB_GetHostSpeedReady: detected low speed\n");
		return 1;
	case 0x02:
		//PRINTF("USB_GetHostSpeedReady: detected full speed\n");
		return 1;
	case 0x03:
		//PRINTF("USB_GetHostSpeedReady: detected high speed\n");
		return 1;
	default:
		//PRINTF("USB_GetHostSpeedReady: unknown speed (code=%02X)\n", (USBx->DVSTCTR0 & USB_DVSTCTR0_RHST) >> USB_DVSTCTR0_RHST_SHIFT);
		return 0;
	}
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
	//return USB_OTG_SPEED_HIGH;
	// 1xx: Reset handshake in progress
	while (((USBx->DVSTCTR0 & USB_DVSTCTR0_RHST) >> USB_DVSTCTR0_RHST_SHIFT) & 0x04)
		dbg_putchar('^');
	switch ((USBx->DVSTCTR0 & USB_DVSTCTR0_RHST) >> USB_DVSTCTR0_RHST_SHIFT)
	{
	case 0x01:
		PRINTF("USB_GetHostSpeed: detected low speed\n");
		return USB_OTG_SPEED_LOW;
	case 0x02:
		PRINTF("USB_GetHostSpeed: detected full speed\n");
		return USB_OTG_SPEED_FULL;
	case 0x03:
		PRINTF("USB_GetHostSpeed: detected high speed\n");
		return USB_OTG_SPEED_HIGH;
	default:
		PRINTF("USB_GetHostSpeed: unknown speed (code=%02X)\n", (USBx->DVSTCTR0 & USB_DVSTCTR0_RHST) >> USB_DVSTCTR0_RHST_SHIFT);
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
	volatile uint16_t * const PIPEnCTR = USBPhyHw_get_pipectr_reg(USBx, pipe);

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
		USBx->BEMPENB |= (1uL << pipe);	// Прерывание окончания передачи передающего буфера
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
	volatile uint16_t * const PIPEnCTR = USBPhyHw_get_pipectr_reg(USBx, pipe);

	* PIPEnCTR = 0x0000;	// NAK
	while ((* PIPEnCTR & (USB_PIPEnCTR_1_5_PBUSY | USB_PIPEnCTR_1_5_CSSTS)) != 0)	// PBUSY, CSSTS
		;

	USBx->NRDYENB &= ~ (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера
	USBx->BRDYENB &= ~ (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера
	USBx->BEMPENB &= ~ (1uL << pipe);	// Прерывание окончания передачи передающего буфера
	if (usbd_is_dmapipe(USBx, pipe))
	{
		usbd_detachdma(USBx, pipe);
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

HAL_StatusTypeDef USB_EP0StartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep, uint8_t dma)
{
	const uint_fast8_t pipe = 0;	// DCP
	ASSERT(dma == 0);
	//PRINTF(PSTR("USB_EP0StartXfer\n"));
	if (ep->is_in == 1)
	{
		//PRINTF("%s: IN direction, ep->xfer_len=%d, ep->maxpacket=%d\n", __func__, (int) ep->xfer_len, (int) ep->maxpacket);

		USBPhyHw_set_pid(USBx, pipe, DEVDRV_USBF_PID_NAK);

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
			//ASSERT(err == 0);
		}

		USBPhyHw_set_pid(USBx, pipe, DEVDRV_USBF_PID_BUF);
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
	//PRINTF("USB_HC_StartXfer, xfer_buff=%p, ep_is_in=%d, ch_num=%d, ep_num=%d, xfer_len=%d\n", hc->xfer_buff, (int) hc->ep_is_in, (int) hc->ch_num, (int) hc->ep_num, (int) hc->xfer_len);
	ASSERT(dma == 0);
	uint8_t  is_oddframe;
	uint16_t num_packets = 0;
	uint16_t max_hc_pkt_count = 256;
	uint32_t tmpreg = 0;

	const uint_fast8_t pipe = 0;
	volatile uint16_t * const PIPEnCTR = USBPhyHw_get_pipectr_reg(USBx, pipe);
	volatile uint16_t * const PIPEnTRE = USBPhyHw_get_pipetre_reg(USBx, pipe);
	volatile uint16_t * const PIPEnTRN = USBPhyHw_get_pipetrn_reg(USBx, pipe);

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


	  //if (dma == 0) /* Slave mode */
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

	      if (pipe == 0)
	      {
	    	  const unsigned devsel = 0x00;

	    		USB_Setup_TypeDef * const pSetup = (USB_Setup_TypeDef *) hc->xfer_buff;

	      		USBx->DCPCTR |= USB_DCPCTR_SUREQCLR;

				//USBx->DCPCTR |= USB_DCPCTR_SQCLR;	// DATA0 as answer
				USBx->DCPCTR |= USB_DCPCTR_SQSET;	// DATA1 as answer

	    		ASSERT((USBx->DCPCTR & USB_DCPCTR_SUREQ) == 0);

//				PRINTF("USB_HC_StartXfer: DCPMAXP=%08lX, dev_addr=%d, bmRequestType=%02X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n",
//						(unsigned long) USBx->DCPMAXP,
//						(int) hc->dev_addr,
//						pSetup->b.bmRequestType, pSetup->b.bRequest, pSetup->b.wValue.w, pSetup->b.wIndex.w, pSetup->b.wLength.w);

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
				//USB_WritePacketNec(USBx, pipe, NULL, 0);	// ?

				USBx->DCPCTR |= USB_DCPCTR_SUREQ;	// Writing setup packet data to the registers and writing 1 to the SUREQ bit in DCPCTR transmits the specified data for setup transactions.

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
	volatile uint16_t * const PIPEnCTR = USBPhyHw_get_pipectr_reg(USBx, pipe);

	* PIPEnCTR = 0x0000;	// NAK
	while ((* PIPEnCTR & (USB_PIPEnCTR_1_5_PBUSY | USB_PIPEnCTR_1_5_CSSTS)) != 0)	// PBUSY, CSSTS
		;

	USBx->NRDYENB &= ~ (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера
	USBx->BRDYENB &= ~ (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера
	USBx->BEMPENB &= ~ (1uL << pipe);	// Прерывание окончания передачи передающего буфера
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
	uint16_t mps, uint8_t tt_hubaddr, uint8_t tt_prtaddr
	)
{
	PRINTF("USB_HC_Init, ch_num=%d, epnum=%d, ep_type=%d, mps=%lu\n", (int) ch_num, (int) epnum, (int) ep_type, (unsigned long) mps);

	const uint_fast8_t pipe = 0;
	volatile uint16_t * const PIPEnCTR = USBPhyHw_get_pipectr_reg(USBx, pipe);

	/* Clear old interrupt conditions for this host channel. */
	////USBx_HC(ch_num)->HCINT = 0xFFFFFFFF;
//
//	  USBx_HC((uint32_t)ch_num)->HCSPLT = (USBx_HC((uint32_t)ch_num)->HCSPLT & ~ (USB_OTG_HCSPLT_HUBADDR_Msk | USB_OTG_HCSPLT_PRTADDR_Msk)) |
//			  ((uint32_t) tt_hubaddr < USB_OTG_HCSPLT_HUBADDR_Pos) |
//			  ((uint32_t) tt_prtaddr < USB_OTG_HCSPLT_PRTADDR_Pos) |
//			  0;

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
//		if (ep_type != USBD_EP_TYPE_CTRL)
//			USBx->NRDYENB |= (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера
//		USBx->BRDYENB |= (1uL << pipe);	// Прерывание по заполненности приёмного (OUT) буфера
//		USBx->BEMPENB |= (1uL << pipe);	// Прерывание окончания передачи передающего буфера

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
  * @brief  Start the host driver.
  * @param  hhcd HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_Start(HCD_HandleTypeDef *hhcd)
{
  __HAL_LOCK(hhcd);
  __HAL_HCD_ENABLE(hhcd);
  (void)USB_DriveVbus(hhcd->Instance, 1U);
  __HAL_UNLOCK(hhcd);

  return HAL_OK;
}

/**
  * @brief  Stop the host driver.
  * @param  hhcd HCD handle
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_HCD_Stop(HCD_HandleTypeDef *hhcd)
{
  __HAL_LOCK(hhcd);
  (void)USB_StopHost(hhcd->Instance);
  __HAL_UNLOCK(hhcd);

  return HAL_OK;
}

/**
  * @brief  Reset the host port.
  * @param  hhcd HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_ResetPort(HCD_HandleTypeDef *hhcd)
{
  return (USB_ResetPort(hhcd->Instance));
}

/**
  * @brief  Reset the host port.
  * @param  hhcd HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_ResetPort2(HCD_HandleTypeDef *hhcd, uint8_t resetActiveState)
{
  return (USB_ResetPort2(hhcd->Instance, resetActiveState));
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


/**
  * @brief  USB_ResetPort : Reset Host Port
  * @param  USBx  Selected device
  * @retval HAL status
  * @note (1)The application must wait at least 10 ms
  *   before clearing the reset bit.
  */
HAL_StatusTypeDef USB_ResetPort(USB_OTG_GlobalTypeDef *USBx)
{
	USB_ResetPort2(USBx, 1);
	//HAL_Delay(100U);                                 /* See Note #1 */
	HARDWARE_DELAY_MS(100);
	USB_ResetPort2(USBx, 0);
	//HAL_Delay(10U);
	HARDWARE_DELAY_MS(10);

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
			1 * USB_SYSCFG_DRPD |	// DRPD 1: Pulling down the D+ and D- lines is enabled.
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
		//1 * USB_INTENB1_BCHGE |		// BCHG
		1 * USB_INTENB1_DTCHE |		// DTCH
		1 * USB_INTENB1_ATTCHE |	// ATTCH
		//1 * USB_INTENB1_SIGNE |		// SIGN
		//1 * USB_INTENB1_SACKE |		// SACK
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
			((0 ? 0x03 : 0x02) << USB_DEVADDn_USBSPD_SHIFT) |
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
	USBPhyHw_set_pid(USBx, usbd_getpipe(ep), DEVDRV_USBF_PID_STALL);
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
	USBPhyHw_set_pid(USBx, pipe, DEVDRV_USBF_PID_NAK);

    p_reg = USBPhyHw_get_pipectr_reg(USBx, pipe);
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
		USB_SYSCFG_UCKSEL_Value |	// UCKSEL 1: The 12-MHz EXTAL clock is selected.
		0;
	(void) SYSCFG0_0;
	HARDWARE_DELAY_MS(2);	// required 1 ms delay - see R01UH0437EJ0200 Rev.2.00 28.4.1 System Control and Oscillation Control

	USBx->SUSPMODE |= USB_SUSPMODE_SUSPM;	// SUSPM 1: The clock supplied to this module is enabled.
	(void) USBx->SUSPMODE;

	return HAL_OK;
}

#if ! WITHNEWUSBHAL
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
#endif /* ! WITHNEWUSBHAL */

/**
  * @brief  USB_StopDevice : Stop the usb device mode
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef USB_StopDevice(USB_OTG_GlobalTypeDef *USBx)
{
	USBx->INTENB0 = 0;
	USBx->INTENB1 = 0;

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


/**
  * @brief  Initialize the host driver.
  * @param  hhcd HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_Init(HCD_HandleTypeDef *hhcd)
{
  USB_OTG_GlobalTypeDef *USBx;

  /* Check the HCD handle allocation */
  if (hhcd == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  //assert_param(IS_HCD_ALL_INSTANCE(hhcd->Instance));

  USBx = hhcd->Instance;

  if (hhcd->State == HAL_HCD_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hhcd->Lock = HAL_UNLOCKED;

#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
    hhcd->SOFCallback = HAL_HCD_SOF_Callback;
    hhcd->ConnectCallback = HAL_HCD_Connect_Callback;
    hhcd->DisconnectCallback = HAL_HCD_Disconnect_Callback;
    hhcd->PortEnabledCallback = HAL_HCD_PortEnabled_Callback;
    hhcd->PortDisabledCallback = HAL_HCD_PortDisabled_Callback;
    hhcd->HC_NotifyURBChangeCallback = HAL_HCD_HC_NotifyURBChange_Callback;

    if (hhcd->MspInitCallback == NULL)
    {
      hhcd->MspInitCallback = HAL_HCD_MspInit;
    }

    /* Init the low level hardware */
    hhcd->MspInitCallback(hhcd);
#else
    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
    HAL_HCD_MspInit(hhcd);
#endif /* (USE_HAL_HCD_REGISTER_CALLBACKS) */
  }

  hhcd->State = HAL_HCD_STATE_BUSY;
//
//  /* Disable DMA mode for FS instance */
//  if ((USBx->CID & (0x1U << 8)) == 0U)
//  {
//    hhcd->Init.dma_enable = 0U;
//  }

  /* Disable the Interrupts */
  __HAL_HCD_DISABLE(hhcd);

  /* Init the Core (common init.) */
  (void)USB_CoreInit(USBx, hhcd->Init);

  /* Force Host Mode*/
  (void)USB_SetCurrentMode(USBx, USB_HOST_MODE);

  /* Init Host */
  (void)USB_HostInit(USBx, hhcd->Init);

  hhcd->State = HAL_HCD_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Initialize a host channel.
  * @param  hhcd HCD handle
  * @param  ch_num Channel number.
  *         This parameter can be a value from 1 to 15
  * @param  epnum Endpoint number.
  *          This parameter can be a value from 1 to 15
  * @param  dev_address Current device address
  *          This parameter can be a value from 0 to 255
  * @param  speed Current device speed.
  *          This parameter can be one of these values:
  *            HCD_DEVICE_SPEED_HIGH: High speed mode,
  *            HCD_DEVICE_SPEED_FULL: Full speed mode,
  *            HCD_DEVICE_SPEED_LOW: Low speed mode
  * @param  ep_type Endpoint Type.
  *          This parameter can be one of these values:
  *            EP_TYPE_CTRL: Control type,
  *            EP_TYPE_ISOC: Isochronous type,
  *            EP_TYPE_BULK: Bulk type,
  *            EP_TYPE_INTR: Interrupt type
  * @param  mps Max Packet Size.
  *          This parameter can be a value from 0 to32K
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_HC_Init(HCD_HandleTypeDef *hhcd,
                                  uint8_t ch_num,
                                  uint8_t epnum,
                                  uint8_t dev_address,
                                  uint8_t speed,
                                  uint8_t ep_type,
								  uint16_t mps, uint8_t tt_hubaddr, uint8_t tt_prtaddr)
 {
	HAL_StatusTypeDef status;
	RZ_HCD_HCTypeDef *const hc = &hhcd->hc[ch_num];

	__HAL_LOCK(hhcd);
	hc->do_ping = 0U;
	hc->dev_addr = dev_address;
	hc->max_packet = mps;
	hc->ch_num = ch_num;
	hc->ep_type = ep_type;
	hc->ep_num = epnum & 0x7FU;
	hc->tt_hubaddr = tt_hubaddr;
	hc->tt_prtaddr = tt_prtaddr;

	if ((epnum & 0x80U) == 0x80U)
	{
		hc->ep_is_in = 1U;
	} else {
		hc->ep_is_in = 0U;
	}

	hc->speed = speed;

	status = USB_HC_Init(hhcd->Instance, ch_num, epnum, dev_address, speed,
			ep_type, mps, tt_hubaddr, tt_prtaddr);
	__HAL_UNLOCK(hhcd);

	return status;
}

/**
  * @brief  Halt a host channel.
  * @param  hhcd HCD handle
  * @param  ch_num Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_HC_Halt(HCD_HandleTypeDef *hhcd, uint8_t ch_num)
{
  HAL_StatusTypeDef status = HAL_OK;

  __HAL_LOCK(hhcd);
  (void)USB_HC_Halt(hhcd->Instance, (uint8_t)ch_num);
  __HAL_UNLOCK(hhcd);

  return status;
}

/**
  * @brief  DeInitialize the host driver.
  * @param  hhcd HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_DeInit(HCD_HandleTypeDef *hhcd)
{
  /* Check the HCD handle allocation */
  if (hhcd == NULL)
  {
    return HAL_ERROR;
  }

  hhcd->State = HAL_HCD_STATE_BUSY;

#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
  if (hhcd->MspDeInitCallback == NULL)
  {
    hhcd->MspDeInitCallback = HAL_HCD_MspDeInit; /* Legacy weak MspDeInit  */
  }

  /* DeInit the low level hardware */
  hhcd->MspDeInitCallback(hhcd);
#else
  /* DeInit the low level hardware: CLOCK, NVIC.*/
  HAL_HCD_MspDeInit(hhcd);
#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */

  __HAL_HCD_DISABLE(hhcd);

  hhcd->State = HAL_HCD_STATE_RESET;

  return HAL_OK;
}


/**
  * @brief  Submit a new URB for processing.
  * @param  hhcd HCD handle
  * @param  ch_num Channel number.
  *         This parameter can be a value from 1 to 15
  * @param  direction Channel number.
  *          This parameter can be one of these values:
  *           0 : Output / 1 : Input
  * @param  ep_type Endpoint Type.
  *          This parameter can be one of these values:
  *            EP_TYPE_CTRL: Control type/
  *            EP_TYPE_ISOC: Isochronous type/
  *            EP_TYPE_BULK: Bulk type/
  *            EP_TYPE_INTR: Interrupt type/
  * @param  token Endpoint Type.
  *          This parameter can be one of these values:
  *            0: HC_PID_SETUP / 1: HC_PID_DATA1
  * @param  pbuff pointer to URB data
  * @param  length Length of URB data
  * @param  do_ping activate do ping protocol (for high speed only).
  *          This parameter can be one of these values:
  *           0 : do ping inactive / 1 : do ping active
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_HC_SubmitRequest(HCD_HandleTypeDef *hhcd,
                                           uint8_t ch_num,
                                           uint8_t direction,
                                           uint8_t ep_type,
                                           uint8_t token,
                                           uint8_t *pbuff,
										   uint32_t length,
                                           uint8_t do_ping)
{
	RZ_HCD_HCTypeDef *const hc = & hhcd->hc [ch_num];
	hhcd->hc[ch_num].ep_is_in = direction;
	hhcd->hc[ch_num].ep_type = ep_type;

	if (token == 0U) {
		hhcd->hc[ch_num].data_pid = HC_PID_SETUP;
		hhcd->hc[ch_num].do_ping = do_ping;
	} else {
		hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
	}

	/* Manage Data Toggle */
	switch (ep_type) {
	case EP_TYPE_CTRL:
		if ((token == 1U) && (direction == 0U)) /*send data */
		{
			if (length == 0U) {
				/* For Status OUT stage, Length==0, Status Out PID = 1 */
				hhcd->hc[ch_num].toggle_out = 1U;
			}

			/* Set the Data Toggle bit as per the Flag */
			if (hhcd->hc[ch_num].toggle_out == 0U) {
				/* Put the PID 0 */
				hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
			} else {
				/* Put the PID 1 */
				hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
			}
		}
		break;

	case EP_TYPE_BULK:
		if (direction == 0U) {
			/* Set the Data Toggle bit as per the Flag */
			if (hhcd->hc[ch_num].toggle_out == 0U) {
				/* Put the PID 0 */
				hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
			} else {
				/* Put the PID 1 */
				hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
			}
		} else {
			if (hhcd->hc[ch_num].toggle_in == 0U) {
				hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
			} else {
				hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
			}
		}

		break;
	case EP_TYPE_INTR:
		if (direction == 0U) {
			/* Set the Data Toggle bit as per the Flag */
			if (hhcd->hc[ch_num].toggle_out == 0U) {
				/* Put the PID 0 */
				hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
			} else {
				/* Put the PID 1 */
				hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
			}
		} else {
			if (hhcd->hc[ch_num].toggle_in == 0U) {
				hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
			} else {
				hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
			}
		}
		break;

	case EP_TYPE_ISOC:
		hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
		break;

	default:
		break;
	}

	hhcd->hc[ch_num].xfer_buff = pbuff;
	hhcd->hc[ch_num].xfer_len = length;
	hhcd->hc[ch_num].urb_state = URB_IDLE;
	hhcd->hc[ch_num].xfer_count = 0U;
	hhcd->hc[ch_num].ch_num = ch_num;
	hhcd->hc[ch_num].state = HC_IDLE;

	USB_OTG_GlobalTypeDef * const USBx = hhcd->Instance;

	const unsigned devsel = 0x00;
	volatile uint16_t * const DEVADDn = (& USBx->DEVADD0) + devsel;
	* DEVADDn =
		((hc->speed == PCD_SPEED_FULL ? 0x03 : 0x02) << USB_DEVADDn_USBSPD_SHIFT) |
		(hc->tt_prtaddr << USB_DEVADDn_HUBPORT_SHIFT) |
		(hc->tt_hubaddr << USB_DEVADDn_UPPHUB_SHIFT) |
		0;
	(void) * DEVADDn;

//  if (direction == 0)
//  {
//	  // OUT
//	  printhex(0, pbuff, length);
//  }
//
//  return USB_HC_StartXfer(hhcd->Instance, &hhcd->hc[ch_num], (uint8_t)hhcd->Init.dma_enable);


	switch (ep_type)
	{
	case EP_TYPE_CTRL:
		if (token == 0)
		{
			const uint_fast8_t pipe = 0;
			USB_Setup_TypeDef * const pSetup = (USB_Setup_TypeDef *) hc->xfer_buff;
			// Setup

			PRINTF("HAL_HCD_HC_SubmitRequest: SETUP, ch_num=%u, ep_num=%u, hc->xfer_buff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", hc->ch_num, hc->ep_num, hc->xfer_buff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
			//PRINTF("HAL_HCD_HC_SubmitRequest: max_packet=%u, tt_hub=%d, tt_prt=%d, speed=%d\n", hc->max_packet, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed);
			printhex(0, pbuff, hc->xfer_len);

//			VERIFY(0 == qtd_item2_buff(qtdrequest, hc->xfer_buff, hc->xfer_len));
//			qtd_item2(qtdrequest, EHCI_FL_PID_SETUP, do_ping);
			dcache_clean((uintptr_t) hc->xfer_buff, hc->xfer_len);

			USBx->BEMPENB &= ~ (0x01uL << pipe);	// пока не запросили - принимать не разрешаем
			USBx->NRDYENB &= ~ (0x01uL << pipe);	// пока не запросили - принимать не разрешаем
	  		USBx->BRDYENB &= ~ (0x01uL << pipe);	// пока не запросили - принимать не разрешаем
	  		USBx->INTENB1 |= (USB_INTENB1_SIGNE | USB_INTENB1_SACKE);
//
//			// бит toggle хранится в памяти overlay и модифицируется сейчас в соответствии с требовании для SETUP запросов
//			qtd_item2_set_toggle(qtdrequest, 0);

      		//USBx->DCPCTR |= USB_DCPCTR_SUREQCLR;
    		ASSERT((USBx->DCPCTR & USB_DCPCTR_SUREQ) == 0);

			//USBx->DCPCTR |= USB_DCPCTR_SQCLR;	// DATA0 as answer
			USBx->DCPCTR |= USB_DCPCTR_SQSET;	// DATA1 as answer


			// Reserved bits: The write value should always be 0.

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

			USBx->BEMPENB &= ~ (0x01uL << pipe);	// Прерывание окончания передачи передающего буфера
			USBx->NRDYENB &= ~ (0x01uL << pipe);
	  		USBx->BRDYENB &= ~ (0x01uL << pipe);		// запросили - принимать разрешаем

			USBx->DCPCTR |= USB_DCPCTR_SUREQ;	// Writing setup packet data to the registers and writing 1 to the SUREQ bit in DCPCTR transmits the specified data for setup transactions.

		}
		else if (direction == 0)
		{
			const uint_fast8_t pipe = 0;
			// Data OUT
			PRINTF("HAL_HCD_HC_SubmitRequest: OUT, ch_num=%u, ep_num=%u, hc->xfer_buff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", hc->ch_num, hc->ep_num, hc->xfer_buff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
			//PRINTF("HAL_HCD_HC_SubmitRequest: max_packet=%u, tt_hub=%d, tt_prt=%d, speed=%d\n", hc->max_packet, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed);
			printhex(0, pbuff, hc->xfer_len);

//			VERIFY(0 == qtd_item2_buff(qtdrequest, hc->xfer_buff, hc->xfer_len));
//			qtd_item2(qtdrequest, EHCI_FL_PID_OUT, do_ping);
			dcache_clean((uintptr_t) hc->xfer_buff, hc->xfer_len);
//
     		//USBx->DCPCTR |= USB_DCPCTR_SUREQCLR;
    		//ASSERT((USBx->DCPCTR & USB_DCPCTR_SUREQ) == 0);
//			// бит toggle хранится в памяти overlay и модифицируется сейчас в соответствии с требовании для SETUP запросов
//			qtd_item2_set_toggle(qtdrequest, 1);
			USBx->DCPCTR |= USB_DCPCTR_SQCLR;	// DATA0 as answer
			//USBx->DCPCTR |= USB_DCPCTR_SQSET;	// DATA1 as answer
			//USBx->DCPCFG = USB_DCPCFG_DIR;


			USBx->BEMPENB |= (0x01uL << pipe);	// Прерывание окончания передачи передающего буфера
			USBx->NRDYENB &= ~ (0x01uL << pipe);
	  		USBx->BRDYENB &= ~ (0x01uL << pipe);		// запросили - принимать разрешаем
	  		USBx->DCPCTR |= USB_DCPCTR_PINGE * (do_ping != 0);
	  		USBx->DCPCTR |= USB_DCPCTR_PINGE * (1 != 0);

			ASSERT(USB_WritePacketNec(USBx, pipe, hc->xfer_buff, hc->xfer_len) == 0);
			USBx->DCPCTR |= USB_DCPCTR_SUREQ;	// Writing setup packet data to the registers and writing 1 to the SUREQ bit in DCPCTR transmits the specified data for setup transactions.

		}
		else
		{
			const uint_fast8_t pipe = 0;
			// Data In
			PRINTF("HAL_HCD_HC_SubmitRequest: IN, ch_num=%u, ep_num=%u, hc->xfer_buff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", hc->ch_num, hc->ep_num, hc->xfer_buff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
			//PRINTF("HAL_HCD_HC_SubmitRequest: max_packet=%u, tt_hub=%d, tt_prt=%d, speed=%d\n", hc->max_packet, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed);

//			VERIFY(0 == qtd_item2_buff(qtdrequest, hc->xfer_buff, hc->xfer_len));
//			qtd_item2(qtdrequest, EHCI_FL_PID_IN, 0);
			dcache_clean_invalidate((uintptr_t) hc->xfer_buff, hc->xfer_len);
//
//			// бит toggle хранится в памяти overlay и модифицируется сейчас в соответствии с требовании для SETUP запросов
//			qtd_item2_set_toggle(qtdrequest, 1);

     		//USBx->DCPCTR |= USB_DCPCTR_SUREQCLR;
    		//ASSERT((USBx->DCPCTR & USB_DCPCTR_SUREQ) == 0);
			//USBx->DCPCTR |= USB_DCPCTR_SQCLR;	// DATA0 as answer
			//USBx->DCPCTR |= USB_DCPCTR_SQSET;	// DATA1 as answer
      		//USBx->DCPCTR |= USB_DCPCTR_SUREQCLR;
			//USBx->DCPCTR |= USB_DCPCTR_SQCLR;	// DATA0 as answer
			//USBx->DCPCFG = 0 * USB_DCPCFG_DIR;

			USBx->BEMPENB &= ~ (0x01uL << pipe);	// Прерывание окончания передачи передающего буфера
			USBx->NRDYENB &= ~ (0x01uL << pipe);
	  		USBx->BRDYENB |= (0x01uL << pipe);		// запросили - принимать разрешаем
			//USBx->DCPCTR |= USB_DCPCTR_SUREQ;	// Writing setup packet data to the registers and writing 1 to the SUREQ bit in DCPCTR transmits the specified data for setup transactions.
		}
		break;

		// See also USBH_EP_BULK - используется host library для
		// see also USB_EP_TYPE_BULK
		// https://www.usbmadesimple.co.uk/ums_7.htm#high_speed_bulk_trans
	case EP_TYPE_BULK:
		if (hc->ep_is_in == 0)
		{
			const uint_fast8_t pipe = hc->ch_num;
			// BULK Data OUT
			PRINTF("HAL_HCD_HC_SubmitRequest: BULK OUT, ch_num=%u, ep_num=%u, hc->xfer_buff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", hc->ch_num, hc->ep_num, hc->xfer_buff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
			//PRINTF("HAL_HCD_HC_SubmitRequest: max_packet=%u, tt_hub=%d, tt_prt=%d, speed=%d\n", hc->max_packet, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed);
			//printhex((uintptr_t) hc->xfer_buff, hc->xfer_buff, hc->xfer_len);

//			VERIFY(0 == qtd_item2_buff(qtdrequest, hc->xfer_buff, hc->xfer_len));
//			qtd_item2(qtdrequest, EHCI_FL_PID_OUT, do_ping);
			dcache_clean((uintptr_t) hc->xfer_buff, hc->xfer_len);

	  		USBx->DCPCTR |= USB_DCPCTR_PINGE * (do_ping != 0);
			// бит toggle хранится в памяти overlay и модифицируется самим контроллером

			ASSERT(USB_WritePacketNec(USBx, pipe, hc->xfer_buff, hc->xfer_len) == 0);

			USBx->BEMPENB |= (0x01uL << pipe);	// Прерывание окончания передачи передающего буфера
	  		USBx->BRDYENB &= ~ (0x01uL << pipe);		// запросили - принимать разрешаем
		}
		else
		{
			const uint_fast8_t pipe = hc->ch_num;
			// BULK Data IN
			PRINTF("HAL_HCD_HC_SubmitRequest: BULK IN, ch_num=%u, ep_num=%u, hc->xfer_buff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", hc->ch_num, hc->ep_num, hc->xfer_buff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
			//PRINTF("HAL_HCD_HC_SubmitRequest: max_packet=%u, tt_hub=%d, tt_prt=%d, speed=%d\n", hc->max_packet, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed);

//			VERIFY(0 == qtd_item2_buff(qtdrequest, hc->xfer_buff, hc->xfer_len));
//			qtd_item2(qtdrequest, EHCI_FL_PID_IN, 0);
			dcache_clean_invalidate((uintptr_t) hc->xfer_buff, hc->xfer_len);

			// бит toggle хранится в памяти overlay и модифицируется самим контроллером
			USBx->BEMPENB &= ~ (0x01uL << pipe);	// Прерывание окончания передачи передающего буфера
	  		USBx->BRDYENB |= (0x01uL << pipe);		// запросили - принимать разрешаем
		}
		break;

	case EP_TYPE_INTR:
		//le8_modify( & qtdrequest->status, 0x04, 1 * 0x04);
		if (hc->ep_is_in == 0)
		{
			const uint_fast8_t pipe = hc->ch_num;
			// INTERRUPT Data OUT
			PRINTF("HAL_HCD_HC_SubmitRequest: INTERRUPT OUT, ch_num=%u, ep_num=%u, hc->xfer_buff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", hc->ch_num, hc->ep_num, hc->xfer_buff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
			//PRINTF("HAL_HCD_HC_SubmitRequest: max_packet=%u, tt_hub=%d, tt_prt=%d, speed=%d\n", hc->max_packet, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed);
//			VERIFY(0 == qtd_item2_buff(qtdrequest, hc->xfer_buff, hc->xfer_len));
//			qtd_item2(qtdrequest, EHCI_FL_PID_OUT, do_ping);
			dcache_clean((uintptr_t) hc->xfer_buff, hc->xfer_len);

	  		USBx->DCPCTR |= USB_DCPCTR_PINGE * (do_ping != 0);
			ASSERT(USB_WritePacketNec(USBx, pipe, hc->xfer_buff, hc->xfer_len) == 0);

			// бит toggle хранится в памяти overlay и модифицируется самим контроллером
			USBx->BEMPENB |= (1uL << pipe);	// Прерывание окончания передачи передающего буфера
		}
		else
		{
			const uint_fast8_t pipe = hc->ch_num;
			// INTERRUPT Data IN
			PRINTF("HAL_HCD_HC_SubmitRequest: INTERRUPT IN, ch_num=%u, ep_num=%u, hc->xfer_buff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", hc->ch_num, hc->ep_num, hc->xfer_buff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
			//PRINTF("HAL_HCD_HC_SubmitRequest: max_packet=%u, tt_hub=%d, tt_prt=%d, speed=%d\n", hc->max_packet, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed);
//			VERIFY(0 == qtd_item2_buff(qtdrequest, hc->xfer_buff, hc->xfer_len));
//			qtd_item2(qtdrequest, EHCI_FL_PID_IN, 1);
			dcache_clean_invalidate((uintptr_t) hc->xfer_buff, hc->xfer_len);

			// бит toggle хранится в памяти overlay и модифицируется самим контроллером
	  		USBx->BRDYENB |= (0x01uL << pipe);		// запросили - принимать разрешаем
		}
		break;

	default:
		ASSERT(0);
		break;
	}

	return HAL_OK;
}


/**
  * @brief  Return  URB state for a channel.
  * @param  hhcd HCD handle
  * @param  chnum Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval URB state.
  *          This parameter can be one of these values:
  *            URB_IDLE/
  *            URB_DONE/
  *            URB_NOTREADY/
  *            URB_NYET/
  *            URB_ERROR/
  *            URB_STALL
  */
HCD_URBStateTypeDef HAL_HCD_HC_GetURBState(HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
//	local_delay_ms(300);
//	return URB_DONE;

	//PRINTF("HAL_HCD_HC_GetURBState: hc=%u, urb_state=%u\n", chnum, hhcd->hc[chnum].urb_state);
	return hhcd->hc [chnum].urb_state;
}


/**
  * @brief  Return the last host transfer size.
  * @param  hhcd HCD handle
  * @param  chnum Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval last transfer size in byte
  */
uint32_t HAL_HCD_HC_GetXferCount(HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  return hhcd->hc[chnum].xfer_count;
}

/**
  * @brief  Return the last host transfer size.
  * @param  hhcd HCD handle
  * @param  chnum Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval last transfer size in byte
  */
uint32_t HAL_HCD_HC_GetMaxPacket(HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  return hhcd->hc[chnum].max_packet;
}

/**
  * @brief  Return the Host Channel state.
  * @param  hhcd HCD handle
  * @param  chnum Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval Host channel state
  *          This parameter can be one of these values:
  *            HC_IDLE/
  *            HC_XFRC/
  *            HC_HALTED/
  *            HC_NYET/
  *            HC_NAK/
  *            HC_STALL/
  *            HC_XACTERR/
  *            HC_BBLERR/
  *            HC_DATATGLERR
  */
HCD_HCStateTypeDef  HAL_HCD_HC_GetState(HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  return hhcd->hc[chnum].state;
}

/**
  * @brief  Return the current Host frame number.
  * @param  hhcd HCD handle
  * @retval Current Host frame number
  */
uint32_t HAL_HCD_GetCurrentFrame(HCD_HandleTypeDef *hhcd)
{
  return (USB_GetCurrentFrame(hhcd->Instance));
}

/**
  * @brief  Return the Host enumeration speed.
  * @param  hhcd HCD handle
  * @retval Enumeration speed
  */
uint32_t HAL_HCD_GetCurrentSpeed(HCD_HandleTypeDef *hhcd)
{
  return (USB_GetHostSpeed(hhcd->Instance));
}

uint_fast8_t HAL_HCD_GetCurrentSpeedReady(HCD_HandleTypeDef *hhcd)
{
	  return (USB_GetHostSpeedReady(hhcd->Instance));
}

void
usbd_pipes_initialize(USBD_HandleTypeDef * pdev)
{
	PCD_HandleTypeDef * hpcd = pdev->pData;
	unsigned offset;
	PCD_TypeDef * const USBx = hpcd->Instance;
	PRINTF(PSTR("usbd_pipes_initialize\n"));
	/*
		at initialize:
		usbd_handler_brdy: после инициализации появляется для тех pipe, у которых dir=0 (read direction)
	*/
	{
		USBx->DCPCFG =
				0x0000;
		USBx->DCPMAXP =
				(USB_OTG_MAX_EP0_SIZE << USB_DCPMAXP_MXPS_SHIFT) & USB_DCPMAXP_MXPS;
		USBx->DCPCTR &= ~ USB_DCPCTR_PID;
		USBx->DCPCTR = 0;
	}
	unsigned bufnumb64 = 0x10;
#if WITHUSBCDCACM
	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
		{
			// Прерывание CDC в компьютер из трансивера
			const uint_fast8_t epnum = USBD_CDCACM_INT_EP(USBD_EP_CDCACM_INT, offset);
			const uint_fast8_t pipe = USBPhyHw_EP2PIPE(epnum);
			const uint_fast8_t dir = 1;
			//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

			USBx->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
			while ((USBx->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
				;
			USBx->PIPECFG =
				(0x0F & epnum) * (1uL << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
				dir * (1uL << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
				2 * (1uL << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 2: Interrupt transfer
				0 * USB_PIPECFG_DBLB |		// DBLB - для interrupt должен быть 0
				0;
			const uint32_t bufsize64 = (VIRTUAL_COM_PORT_INT_SIZE + 63) / 64;
			USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
			USBx->PIPEMAXP = VIRTUAL_COM_PORT_INT_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
			bufnumb64 += bufsize64 * 1; // * 2 for DBLB
			ASSERT(bufnumb64 <= 0x100);

			USBx->PIPESEL = 0;
		}
		{
			// Данные CDC из компьютера в трансивер
			const uint_fast8_t epnum = USBD_CDCACM_OUT_EP(USBD_EP_CDCACM_OUT, offset);
			const uint_fast8_t pipe = USBPhyHw_EP2PIPE(epnum);
			const uint_fast8_t dir = 0;
			//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

			USBx->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
			while ((USBx->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
				;
			USBx->PIPECFG =
				(0x0F & epnum) * (1uL << USB_PIPECFG_EPNUM_SHIFT) |	// EPNUM endpoint
				dir * (1uL << USB_PIPECFG_DIR_SHIFT) |			// DIR 1: Transmitting direction 0: Receiving direction
				1 * (1uL << USB_PIPECFG_TYPE_SHIFT) |			// TYPE 1: Bulk transfer
				1 * USB_PIPECFG_DBLB |				// DBLB
				0;
			const uint32_t bufsize64 = (VIRTUAL_COM_PORT_OUT_DATA_SIZE + 63) / 64;
			USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
			USBx->PIPEMAXP = VIRTUAL_COM_PORT_OUT_DATA_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
			bufnumb64 += bufsize64 * 2; // * 2 for DBLB
			ASSERT(bufnumb64 <= 0x100);

			USBx->PIPESEL = 0;
		}
		{
			// Данные CDC в компьютер из трансивера
			const uint_fast8_t epnum = USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, offset);
			const uint_fast8_t pipe = USBPhyHw_EP2PIPE(epnum);
			const uint_fast8_t dir = 1;
			//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

			USBx->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
			while ((USBx->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
				;
			USBx->PIPECFG =
				(0x0F & epnum) * (1uL << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
				dir * (1uL << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
				1 * (1uL << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 1: Bulk transfer
				1 * USB_PIPECFG_DBLB |		// DBLB
				0;
			const uint32_t bufsize64 = (VIRTUAL_COM_PORT_IN_DATA_SIZE + 63) / 64;
			USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
			USBx->PIPEMAXP = VIRTUAL_COM_PORT_IN_DATA_SIZE << USB_PIPEMAXP_MXPS_SHIFT;
			bufnumb64 += bufsize64 * 2; // * 2 for DBLB
			ASSERT(bufnumb64 <= 0x100);

			USBx->PIPESEL = 0;
		}
	}
#endif /* WITHUSBCDCACM */

#if WITHUSBUACIN
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
			(0x0F & epnum) * (1uL << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1uL << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			3 * (1uL << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 11: Isochronous transfer
			dblb * USB_PIPECFG_DBLB |		// DBLB
			0;
		//USBx->PIPEPERI =
		//	1 * (1uL << 12) |	// IFS
		//	0;
		const uint32_t bufsize64 = (maxpacket + 63) / 64;
		USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		USBx->PIPEMAXP = maxpacket << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * (dblb + 1); // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		USBx->PIPESEL = 0;
	}
#endif /* WITHUSBUACIN */

#if WITHUSBUACOUT
	if (1)
	{
		// Данные AUDIO из компьютера в трансивер
		// Используется канал DMA D0
		const uint_fast8_t pipe = HARDWARE_USBD_PIPE_ISOC_OUT;	// PIPE1
		const uint_fast8_t epnum = USBD_EP_AUDIO_OUT;
		const uint_fast8_t dir = 0;
		const uint_fast16_t maxpacket = UACOUT_AUDIO48_DATASIZE_DMAC;
		const uint_fast8_t dblb = 1;
		//PRINTF(PSTR("usbd_pipe_initialize: pipe=%u endpoint=%02X\n"), pipe, epnum);

		USBx->PIPESEL = pipe << USB_PIPESEL_PIPESEL_SHIFT;
		while ((USBx->PIPESEL & USB_PIPESEL_PIPESEL) != (pipe << USB_PIPESEL_PIPESEL_SHIFT))
			;
		ASSERT(pipe == 1);
		USBx->PIPECFG =
			(0x0F & epnum) * (1uL << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1uL << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			3 * (1uL << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 11: Isochronous transfer
			dblb * USB_PIPECFG_DBLB |		// DBLB
			0;

		const uint32_t bufsize64 = (maxpacket + 63) / 64;
		USBx->PIPEBUF = ((bufsize64 - 1) << USB_PIPEBUF_BUFSIZE_SHIFT) | (bufnumb64 << USB_PIPEBUF_BUFNMB_SHIFT);
		USBx->PIPEMAXP = maxpacket << USB_PIPEMAXP_MXPS_SHIFT;
		bufnumb64 += bufsize64 * (dblb + 1); // * 2 for DBLB
		ASSERT(bufnumb64 <= 0x100);

		USBx->PIPESEL = 0;
	}
#endif /* WITHUSBUACOUT */

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
			(0x0F & epnum) * (1uL << USB_PIPECFG_EPNUM_SHIFT) |	// EPNUM endpoint
			dir * (1uL << USB_PIPECFG_DIR_SHIFT) |			// DIR 1: Transmitting direction 0: Receiving direction
			1 * (1uL << USB_PIPECFG_TYPE_SHIFT) |			// TYPE 1: Bulk transfer
			1 * (1uL << USB_PIPECFG_DBLB_SHIFT) |				// DBLB
			0;
		const uint32_t bufsize64 = (USBD_CDCEEM_BUFSIZE + 63) / 64;

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
			(0x0F & epnum) * (1uL << USB_PIPECFG_EPNUM_SHIFT) |		// EPNUM endpoint
			dir * (1uL << USB_PIPECFG_DIR_SHIFT) |		// DIR 1: Transmitting direction 0: Receiving direction
			1 * (1uL << USB_PIPECFG_TYPE_SHIFT) |		// TYPE 1: Bulk transfer
			1 * USB_PIPECFG_DBLB |		// DBLB
			0;
		const uint32_t bufsize64 = (USBD_CDCEEM_BUFSIZE + 63) / 64;

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

#endif /* CPUSTYLE_R7S721 && defined (WITHUSBHW_DEVICE) */
