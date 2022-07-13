/* $Id$ */
// allwnr_hal_usb.c
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//


#include "hardware.h"

#if CPUSTYPE_T113

#include "board.h"
#include "audio.h"
#include "formats.h"

#include "src/usb/usb200.h"
#include "src/usb/usbch9.h"

#include "usb_device.h"
#include "usbd_core.h"
#include "usbh_core.h"
#include "usbh_def.h"

#include "allwnr_t113s3_hal.h"
#include "allwnr_hal_usb.h"



//////////////////////////////////////////////////////////////////
 uint32_t usb_get_epx_fifo_access(pusb_struct pusb, uint32_t ep_no)
{
	return (USBOTG0_BASE + ((ep_no & 0xf) << 2));
}

 void usb_set_dev_addr(pusb_struct pusb, uint32_t addr)
{
	USBOTG0->FIFO [0].USB_TXFADDR = addr & 0x7F;
}

 void usb_set_ep0_addr(pusb_struct pusb, uint32_t fifo, uint32_t addr)
{
	USBOTG0->FIFO [fifo].USB_TXFADDR = addr & 0x7F;
}

 void usb_iso_update_enable(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val |= (0x1 << 7);	// MUSB2_MASK_ISOUPD
	USBOTG0->USB_POWER = reg_val;
}

 void usb_iso_update_disable(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val &= ~ (0x1 << 7);	// MUSB2_MASK_ISOUPD
	USBOTG0->USB_POWER = reg_val;
}

 void usb_soft_connect(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val |= (0x1 << 6);	// MUSB2_MASK_SOFTC
	USBOTG0->USB_POWER = reg_val;
}

 void usb_soft_disconnect(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val &= ~ (0x1 << 6);	// MUSB2_MASK_SOFTC
	USBOTG0->USB_POWER = reg_val;
}

 void usb_high_speed_enable(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val |= (0x1 << 5);
	USBOTG0->USB_POWER = reg_val;
}

 void usb_high_speed_disable(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val &= ~ (0x1 << 5);
	USBOTG0->USB_POWER = reg_val;
}

 uint32_t usb_is_high_speed(pusb_struct pusb)
{
	return (USBOTG0->USB_POWER >> 4) & 0x1;
}

 void usb_set_reset(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val |= (0x1 << 3);
	USBOTG0->USB_POWER = reg_val;
}

 void usb_clear_reset(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val &= ~ (0x1 << 3);
	USBOTG0->USB_POWER = reg_val;
}

 void usb_set_resume(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val |= (0x1 << 2);
	USBOTG0->USB_POWER = reg_val;
}

 void usb_clear_resume(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val &= ~ (0x1 << 2);
	USBOTG0->USB_POWER = reg_val;
}


 void usb_set_suspend(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val |= (0x1 << 1);
	USBOTG0->USB_POWER = reg_val;
}

 void usb_clear_suspend(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val &= ~ (0x1 << 1);
	USBOTG0->USB_POWER = reg_val;
}


 uint32_t usb_check_suspend(pusb_struct pusb)
{
	return (USBOTG0->USB_POWER >> 1) & 0x01;
}

 void usb_suspendm_enable(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val |= (0x1 << 0);
	USBOTG0->USB_POWER = reg_val;
}

 void usb_suspendm_disable(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val &= ~ (0x1 << 0);
	USBOTG0->USB_POWER = reg_val;
}

 uint32_t usb_get_ep0_interrupt_status(pusb_struct pusb)
{
	return USBOTG0->USB_INTTX & 0x1;
}

 void usb_clear_ep0_interrupt_status(pusb_struct pusb)
{
	USBOTG0->USB_INTTX = 0x1;
}

 uint32_t usb_get_eptx_interrupt_status(pusb_struct pusb)
{
	return USBOTG0->USB_INTTX & 0xFFFF;
}

 void usb_clear_eptx_interrupt_status(pusb_struct pusb, uint32_t bm)
{
	USBOTG0->USB_INTTX = bm & 0xFFFF;
}

 uint32_t usb_get_eprx_interrupt_status(pusb_struct pusb)
{
	return USBOTG0->USB_INTRX & 0xFFFF;
}

 void usb_clear_eprx_interrupt_status(pusb_struct pusb, uint32_t bm)
{
	USBOTG0->USB_INTRX = bm & 0xFFFF;
}

 void usb_set_eptx_interrupt_enable(pusb_struct pusb, uint32_t bm)
{
	uint16_t reg_val;

	reg_val = USBOTG0->USB_INTTXE;
	reg_val |= (bm & 0xFFFF);
	USBOTG0->USB_INTTXE = reg_val;
}

 void usb_clear_eptx_interrupt_enable(pusb_struct pusb, uint32_t bm)
{
	uint16_t reg_val;

	reg_val = USBOTG0->USB_INTTXE;
	reg_val &= ~ (bm & 0xFFFF);
	USBOTG0->USB_INTTXE = reg_val;
}

 void usb_set_eprx_interrupt_enable(pusb_struct pusb, uint32_t bm)
{
	uint16_t reg_val;

	reg_val = USBOTG0->USB_INTRXE;
	reg_val |= (bm & 0xFFFF);
	USBOTG0->USB_INTRXE = reg_val;
}

 void usb_clear_eprx_interrupt_enable(pusb_struct pusb, uint32_t bm)
{
	uint16_t reg_val;

	reg_val = USBOTG0->USB_INTRXE;
	reg_val &= ~ (bm & 0xFFFF);
	USBOTG0->USB_INTRXE = reg_val;
}

 uint32_t usb_get_bus_interrupt_status(pusb_struct pusb)
{
	return USBOTG0->USB_INTUSB & 0xFF;
}

 void usb_clear_bus_interrupt_status(pusb_struct pusb, uint32_t bm)
{
	USBOTG0->USB_INTUSB = bm & 0xFF;
	//put_bvalue(USBOTG0_BASE + USB_bINTRUSB_OFF, bm & 0xFF);
}

 uint32_t usb_get_bus_interrupt_enable(pusb_struct pusb)
{
	return USBOTG0->USB_INTUSBE & 0xFF;
	//return USBOTG0->USB_INTUSBE & 0xFF;
}

 void usb_set_bus_interrupt_enable(pusb_struct pusb, uint32_t bm)
{
	uint16_t reg_val;

	reg_val = USBOTG0->USB_INTUSBE;
	reg_val |= (bm & 0xFF);
	USBOTG0->USB_INTUSBE = reg_val;
}

 void usb_clear_bus_interrupt_enable(pusb_struct pusb, uint32_t bm)
{
	uint16_t reg_val;

	reg_val = USBOTG0->USB_INTUSBE;
	reg_val &= ~ (bm & 0xFF);
	USBOTG0->USB_INTUSBE = reg_val;
}

 uint32_t usb_get_frame_number(pusb_struct pusb)
{
	return USBOTG0->USB_FRAME & 0x7FF;
	//return get_hvalue(USBOTG0_BASE + USB_hFRAME_OFF) & 0x7FF;
}

 void usb_select_ep(pusb_struct pusb, uint32_t ep_no)
{
	if(ep_no > USB_MAX_EP_NO)
		return;
	USBOTG0->USB_EPINDEX = ep_no;
	//put_bvalue(USBOTG0_BASE + USB_bINDEX_OFF, ep_no);
}

 uint32_t usb_get_active_ep(pusb_struct pusb)
{
	return USBOTG0->USB_EPINDEX & 0x7FF & 0xf;
	//return get_bvalue(USBOTG0_BASE + USB_bINDEX_OFF) & 0xf;
}

 void usb_set_test_mode(pusb_struct pusb, uint32_t bm)
{
	USBOTG0->USB_TESTMODE = bm;
	//put_bvalue(USBOTG0_BASE + USB_bTESTMODE_OFF, bm & 0xFF);
}

 void usb_set_eptx_maxpkt(pusb_struct pusb, uint32_t maxpayload, uint32_t pktcnt)
{
	uint32_t reg_val;

	reg_val = (maxpayload & 0x7FF);
	reg_val |= ((pktcnt-1) & 0x1f) << 11;
	USBOTG0->USB_TXMAXP = reg_val;
}

 uint32_t usb_get_eptx_maxpkt(pusb_struct pusb)
{
	return USBOTG0->USB_TXMAXP;
}



 void usb_ep0_disable_ping(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val |= (0x1 << 8);
	USBOTG0->USB_CSR0 = reg_val;
}

 void usb_ep0_enable_ping(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val &= ~ (0x1 << 8);
	USBOTG0->USB_CSR0 = reg_val;
}

 void usb_ep0_flush_fifo(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val |= (0x1 << 8);
	USBOTG0->USB_CSR0 = reg_val;
}

 uint32_t usb_ep0_is_naktimeout(pusb_struct pusb)
{
	return (USBOTG0->USB_CSR0 >> 7) & 0x1;
}

 void usb_ep0_clear_naktimeout(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val &= ~ (0x1 << 7);
	USBOTG0->USB_CSR0 = reg_val;
}

 void usb_ep0_set_statuspkt(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val |= (0x1 << 6);
	USBOTG0->USB_CSR0 = reg_val;
}

 void usb_ep0_clear_statuspkt(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val &= ~ (0x1 << 6);
	USBOTG0->USB_CSR0 = reg_val;
}

 void usb_ep0_set_reqpkt(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val |= (0x1 << 5);
	USBOTG0->USB_CSR0 = reg_val;
}

 void usb_ep0_clear_setupend(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val |= (0x1 << 7);
	USBOTG0->USB_CSR0 = reg_val;
}

 void usb_ep0_clear_rxpktrdy(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val |= (0x1 << 6);
	USBOTG0->USB_CSR0 = reg_val;
}


 uint32_t usb_get_ep0_csr(pusb_struct pusb)
{
	uint32_t ret;

	ret = USBOTG0->USB_CSR0;
	return ret;
}

void usb_set_ep0_csr(pusb_struct pusb, uint32_t csr)
{
	 USBOTG0->USB_CSR0 = csr;
}

uint32_t usb_get_eptx_csr(pusb_struct pusb)
{
	return USBOTG0->USB_CSR0;
}

void usb_set_eptx_csr(pusb_struct pusb, uint32_t csr)
{
	 USBOTG0->USB_CSR0 = csr;
}

 void usb_eptx_flush_fifo(pusb_struct pusb)
{
	 USBOTG0->USB_CSR0 = (0x1 << 3);
}

 void usb_set_eprx_maxpkt(pusb_struct pusb, uint32_t maxpayload, uint32_t pktcnt)
{
	uint32_t reg_val;

	reg_val = maxpayload & 0x7FF;
	reg_val |= ((pktcnt-1) & 0x1f) << 11;
	USBOTG0->USB_RXMAXP = reg_val & 0xFFFF;
}

 uint32_t usb_get_eprx_maxpkt(pusb_struct pusb)
{
	return USBOTG0->USB_RXMAXP & 0xFFFF;
}

 uint32_t usb_get_eprx_csr(pusb_struct pusb)
{
	return get_hvalue(USBOTG0_BASE + USB_hRXCSR_OFF);
}

 void usb_set_eprx_csr(pusb_struct pusb, uint32_t csr)
{
	put_hvalue(USBOTG0_BASE + USB_hRXCSR_OFF, csr);
}

 void usb_set_eprx_csrhi(pusb_struct pusb, uint32_t csrhi)
{
	put_bvalue(USBOTG0_BASE + USB_hRXCSR_OFF + 1, csrhi);
}

 void usb_eprx_flush_fifo(pusb_struct pusb)
{
	put_hvalue(USBOTG0_BASE + USB_hRXCSR_OFF, 0x1 << 4);
}

 uint32_t usb_get_ep0_count(pusb_struct pusb)
{
	return USBOTG0->USB_RXCOUNT & 0x7F;
}

 uint32_t usb_get_eprx_count(pusb_struct pusb)
{
	return USBOTG0->USB_RXCOUNT & 0x1FFF;
}

 void usb_set_ep0_type(pusb_struct pusb, uint32_t speed)
{
	put_bvalue(USBOTG0_BASE + USB_bTXTYPE_OFF, (speed & 0x3) << 6);
}

 void usb_set_eptx_type(pusb_struct pusb, uint32_t speed, uint32_t protocol, uint32_t ep_no)
{
	uint32_t reg_val;

	reg_val = (speed & 0x3) << 6;
	reg_val |= (protocol & 0x3) << 4;
	reg_val |= (ep_no & 0xf) << 0;
	put_bvalue(USBOTG0_BASE + USB_bTXTYPE_OFF, reg_val);
}

 void usb_set_ep0_naklimit(pusb_struct pusb, uint32_t naklimit)
{
	put_bvalue(USBOTG0_BASE + USB_bTXINTERVAL_OFF, naklimit & 0x1f);
}

 void usb_set_eptx_interval(pusb_struct pusb, uint32_t interval)
{
	put_bvalue(USBOTG0_BASE + USB_bTXINTERVAL_OFF, interval & 0xFF);
}

 void usb_set_eprx_type(pusb_struct pusb, uint32_t speed, uint32_t protocol, uint32_t ep_no)
{
	uint32_t reg_val;

	reg_val = (speed & 0x3) << 6;
	reg_val |= (protocol & 0x3) << 4;
	reg_val |= (ep_no & 0xf) << 0;
	put_bvalue(USBOTG0_BASE + USB_bRXTYPE_OFF, reg_val);
}

 void usb_set_eprx_interval(pusb_struct pusb, uint32_t interval)
{
	put_bvalue(USBOTG0_BASE + USB_bRXINTERVAL_OFF, interval & 0xFF);
}

 uint32_t usb_get_core_config(pusb_struct pusb)
{
	return get_bvalue(USBOTG0_BASE + USB_bCORECONFIG_OFF);
}

 uint32_t usb_is_b_device(pusb_struct pusb)
{
	return (USBOTG0->USB_DEVCTL >> 7) & 0x1;
}

 uint32_t usb_device_connected_is_fs(pusb_struct pusb)
{
	return (USBOTG0->USB_DEVCTL >> 6) & 0x1;
}

 uint32_t usb_device_connected_is_ls(pusb_struct pusb)
{
	return (USBOTG0->USB_DEVCTL >> 5) & 0x1;
}

 // USB_VBUS_VBUSVLD
 uint32_t usb_get_vbus_level(pusb_struct pusb)
{
	return (USBOTG0->USB_DEVCTL >> 3) & 0x3;
}

 uint32_t usb_is_host(pusb_struct pusb)
{
	return (USBOTG0->USB_DEVCTL >> 2) & 0x1;
}

 void usb_set_hnp_request(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_DEVCTL;
	reg_val |= 0x1 << 1;
	USBOTG0->USB_DEVCTL = reg_val;
}

 uint32_t usb_hnp_in_porcess(pusb_struct pusb)
{
	return (USBOTG0->USB_DEVCTL >> 1) & 0x1;
}

 void usb_start_session(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = 0x1;
	USBOTG0->USB_DEVCTL = reg_val;
}

 void usb_end_session(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = 0x0;
	USBOTG0->USB_DEVCTL = reg_val;
}

 uint32_t usb_check_session(pusb_struct pusb)
{
	return USBOTG0->USB_DEVCTL & 0x1;
}

 uint32_t aw_log2(uint32_t x)
{
 	uint32_t input;
 	uint32_t val;

 	input = x;
 	val = 0;
 	while (input>1)
 	{
		input = input >> 1;
		val++;
 	}

 	return val;
}

 void usb_set_eptx_fifo_size(pusb_struct pusb, uint32_t is_dpb, uint32_t size)
{
	uint8_t reg_val;

	reg_val = 0;
	if(is_dpb) reg_val |= 0x1 << 4;
	reg_val |= aw_log2(size >> 3) & 0xf;
	USBOTG0->USB_TXFIFOSZ = reg_val;
}

 void usb_set_eprx_fifo_size(pusb_struct pusb, uint32_t is_dpb, uint32_t size)
{
	uint8_t reg_val;

	reg_val = 0;
	if(is_dpb) reg_val |= 0x1 << 4;
	reg_val |= aw_log2(size >> 3) & 0xf;
	USBOTG0->USB_RXFIFOSZ = reg_val;
}

 void usb_set_eptx_fifo_addr(pusb_struct pusb, uint32_t addr)
{
	USBOTG0->USB_TXFIFOADD = (addr >> 3) & 0x1FFF;
}

 void usb_set_eprx_fifo_addr(pusb_struct pusb, uint32_t addr)
{
	USBOTG0->USB_RXFIFOADD = (addr >> 3) & 0x1FFF;
}

 void usb_fifo_accessed_by_cpu(pusb_struct pusb)
{
	USBOTG0->USB_DMACTL &= ~ (0x1 << 0);
}

 void usb_fifo_accessed_by_dma(pusb_struct pusb, uint32_t ep_no, uint32_t is_tx)
{
	uint8_t reg_val;

	if (ep_no>USB_MAX_EP_NO)
		return;
	reg_val = 0x1;
	if(!is_tx) reg_val |= 0x1 << 1;
	reg_val |= (ep_no-1) << 2;
	USBOTG0->USB_DMACTL = reg_val;
}

uint8_t usb_get_fifo_access_config(pusb_struct pusb)
{
	return USBOTG0->USB_DMACTL;
}

uint32_t usb_get_dma_ep_no(pusb_struct pusb)
{
	return ((USBOTG0->USB_DMACTL & 0x1f) >> 2)+1;
}

void usb_set_fifo_access_config(pusb_struct pusb, uint8_t config)
{
	USBOTG0->USB_DMACTL = config;
}

uint32_t usb_get_fsm(pusb_struct pusb)
{
	return USBOTG0->USB_FSM;
	//return get_bvalue(USBOTG0_BASE + USB_FSM_OFF);
}


void usb_set_eptx_faddr(pusb_struct pusb, uint32_t fifo, uint32_t faddr)
{
	USBOTG0->FIFO [fifo].USB_TXFADDR = faddr & 0x7F;
	//(USBOTG0->USB_TXFADDR = faddr & 0x7F);
}

void usb_set_eptx_haddr(pusb_struct pusb, uint32_t fifo, uint32_t haddr, uint32_t is_mtt)
{
	uint8_t reg_val;

	reg_val = 0;
	if (is_mtt)
		reg_val |= 0x1 << 7;
	reg_val |= haddr & 0x7F;

	USBOTG0->FIFO [fifo].USB_TXHADDR = reg_val;
	//put_bvalue(USBOTG0_BASE + USB_bTXHADDR_OFF, reg_val);
}

void usb_set_eptx_hport(pusb_struct pusb, uint32_t fifo, uint32_t hport)
{
	USBOTG0->FIFO [fifo].USB_TXHUBPORT = hport & 0x7F;
	//put_bvalue(USBOTG0_BASE + USB_bTXHPORT_OFF, hport & 0x7F);
}

void usb_set_eprx_faddr(pusb_struct pusb, uint32_t fifo, uint32_t faddr)
{
	USBOTG0->FIFO [fifo].USB_RXFADDR = faddr & 0x7F;
	//put_bvalue(USBOTG0_BASE + USB_bRXFADDR_OFF, faddr & 0x7F);
}

void usb_set_eprx_haddr(pusb_struct pusb, uint32_t fifo, uint32_t haddr, uint32_t is_mtt)
{
	uint8_t reg_val;

	reg_val = 0;
	if (is_mtt)
		reg_val |= 0x1 << 7;
	reg_val |= haddr & 0x7F;

	USBOTG0->FIFO [fifo].USB_RXHADDR = reg_val;
	//put_bvalue(USBOTG0_BASE + USB_bRXHADDR_OFF, reg_val);
}

void usb_set_eprx_hport(pusb_struct pusb, uint32_t fifo, uint32_t hport)
{
	USBOTG0->FIFO [fifo].USB_RXHUBPORT = hport & 0x7F;
	//put_bvalue(USBOTG0_BASE + USB_bRXHPORT_OFF, hport & 0x7F);
}


void usb_set_reqpkt_count(pusb_struct pusb, uint32_t count)
{
	USBOTG0->USB_RXPKTCNT = count;
}

uint32_t usb_get_reqpkt_count(pusb_struct pusb)
{
	 return USBOTG0->USB_RXPKTCNT;
}

uint32_t usb_get_lsvbusvld(pusb_struct pusb)
{
	return (USBOTG0->USB_ISCR >> 30) & 0x1;
}

uint32_t usb_get_extvbusvld(pusb_struct pusb)
{
	return (USBOTG0->USB_ISCR >> 29) & 0x1;
}

uint32_t usb_get_extiddig(pusb_struct pusb)
{
	return (USBOTG0->USB_ISCR >> 28) & 0x1;
}

uint32_t usb_get_linestate(pusb_struct pusb)
{
	return (USBOTG0->USB_ISCR >> 26) & 0x3;
}

uint32_t usb_get_vbusvalid(pusb_struct pusb)
{
	return (USBOTG0->USB_ISCR >> 25) & 0x1;
}

uint32_t usb_get_iddig(pusb_struct pusb)
{
#ifdef CONFIG_AW_FPGA_PLATFORM
	 return (USBOTG0->USB_DEVCTL >> 0x7) & 0x1;
#else
	 return (USBOTG0->USB_ISCR >> 24) & 0x1;
#endif
}

uint32_t usb_get_fs_linestate(pusb_struct pusb)
{
	return ((USBOTG0->USB_ISCR) >> 20) & 0x03;
}

void usb_iddig_pullup_enable(pusb_struct pusb)
{
	USBOTG0->USB_ISCR |= (0x1 << 17);
}

void usb_iddig_pullup_disable(pusb_struct pusb)
{
	USBOTG0->USB_ISCR &= ~ (0x1 << 17);
}

void usb_dpdm_pullup_enable(pusb_struct pusb)
{
	USBOTG0->USB_ISCR |= (0x1 << 16);
}

void usb_dpdm_pullup_disable(pusb_struct pusb)
{
	USBOTG0->USB_ISCR &= ~ (0x1 << 16);
}

void usb_release_id(pusb_struct pusb)
{
	USBOTG0->USB_ISCR &= ~ (0x3 << 14);
}

void usb_force_id(pusb_struct pusb, uint32_t id)
{

	if(id) 	USBOTG0->USB_ISCR |= (0x1 << 14);
	else 	USBOTG0->USB_ISCR &= ~ (0x1 << 14);
	USBOTG0->USB_ISCR |= (0x1 << 15);

}

/* src = 0..3 */
void usb_vbus_src(pusb_struct pusb, uint32_t src)
{
	USBOTG0->USB_ISCR &= ~ (0x3 << 10);
	USBOTG0->USB_ISCR |= ((src & 0x03) << 10);
}

void usb_release_vbus(pusb_struct pusb)
{
	USBOTG0->USB_ISCR &= ~ (0x3 << 12);
}

void usb_force_vbus(pusb_struct pusb, uint32_t vbus)
{
	if(vbus) 	USBOTG0->USB_ISCR |= (0x1 << 12);
	else 		USBOTG0->USB_ISCR &= ~ (0x1 << 12);
	USBOTG0->USB_ISCR |= (0x1 << 13);
}

void usb_drive_vbus(pusb_struct pusb, uint32_t vbus, uint32_t index)
{
	uint32_t temp;
	if(index == 0)
	{
//	//Set PB9 Output,USB0-DRV SET
//	 temp = get_wvalue(0x01c20800+0x28);
//	 temp &= ~ (0x7 << 4);
//	 temp |= (0x1 << 1);
//	 put_wvalue(0x01c20800+0x28, temp);
//
//	 if(vbus)
//	 {
//	 	temp = get_wvalue(0x01c20800+0x34);
//	 	temp |= (0x1 << 9);
//	 	put_wvalue(0x01c20800+0x34,temp);
//	 }
//	 else
//	 {
//	 temp = get_wvalue(0x01c20800+0x34);
//	 	temp &= ~ (0x1 << 9);
//	 	put_wvalue(0x01c20800+0x34,temp);
//	 }
	}

}

uintptr_t usb_get_ep_fifo_addr(pusb_struct pusb, uint32_t ep_no)
{
	return (uintptr_t) & USBOTG0->USB_EPFIFO [ep_no];
	//return (USBOTG0_BASE + USB_bFIFO_OFF(ep_no));
}

// uint32_t usb_dma_write_config(pusb_struct pusb)
//{
//	return (((USB_DMA_CONFIG|(0x1f&pusb->drq_no)) << 16)|DRAM_DMA_CONFIG);
//}
//
// uint32_t usb_dma_read_config(pusb_struct pusb)
//{
//	return ((DRAM_DMA_CONFIG << 16)|(USB_DMA_CONFIG|(0x1f&pusb->drq_no)));
//}


#define USB_NO_DMA		1


const unsigned char TestPkt[54] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA,
	                                 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEE, 0xEE, 0xEE,
	                                 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
	                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xBF, 0xDF,
	                                 0xEF, 0xF7, 0xFB, 0xFD, 0xFC, 0x7E, 0xBF, 0xDF, 0xEF, 0xF7,
	                                 0xFB, 0xFD, 0x7E, 0x00};



void usb_read_ep_fifo(pusb_struct pusb, uint32_t ep_no, uint32_t dest_addr, uint32_t count)
{
	uint8_t temp;
	uint8_t saved;
	uint32_t dest;
	uint32_t i;

	if (ep_no>USB_MAX_EP_NO)
	{
		return;
	}
	saved = usb_get_fifo_access_config(pusb);
	usb_fifo_accessed_by_cpu(pusb);

	dest = dest_addr;
	const uintptr_t pipe = usb_get_ep_fifo_addr(pusb, ep_no);
	for (i=0; i<count; i++)
	{
		temp = get_bvalue(pipe);
		put_bvalue(dest, temp);
		dest += 1;
	}
	if ((count!=31) && (count!=8))
	{
		//USB_HAL_DBG("rxcount=%d, rxdata=0x%x\n", count, *((uint32_t*)dest_addr));
	}

	usb_set_fifo_access_config(pusb, saved);
}


void usb_write_ep_fifo(pusb_struct pusb, uint32_t ep_no, uint32_t src_addr, uint32_t count)
{
	uint8_t  temp;
	uint8_t  saved;
	uint32_t src;
	uint32_t i;

	if (ep_no>USB_MAX_EP_NO)
	{
		return;
	}
	saved = usb_get_fifo_access_config(pusb);
	usb_fifo_accessed_by_cpu(pusb);

	src = src_addr;
	const uintptr_t pipe = usb_get_ep_fifo_addr(pusb, ep_no);
	for (i=0; i<count; i++)
	{
		temp = get_bvalue(src);
		put_bvalue(pipe, temp);
		src += 1;
	}

	usb_set_fifo_access_config(pusb, saved);
}

//__inline void usb_set_eptx_fifo_size(pusb_struct pusb, uint32_t is_dpb, uint32_t size)
//{
//	uint8_t reg_val;
//
//	reg_val = 0;
//	if (is_dpb)
//	{
//		reg_val |= 0x1 << 4;
//	}
//	reg_val |= aw_log2(size>>3) & 0xf;
//	put_bvalue(pusb->reg_base + USB_bTxFIFOsz_OFF, reg_val);
//}
///////////////////////////////////////////////////////////////////
//                 usb bulk transfer
///////////////////////////////////////////////////////////////////
/*
************************************************************************************************************
*
*                                             usb_dev_get_buf_base
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ����ȡPING_PANG buffer��ַ
*
*
************************************************************************************************************
*/
#ifndef USB_NO_DMA
static uint32_t usb_dev_get_buf_base(pusb_struct pusb, uint32_t buf_tag)
{
	return (pusb->device.bo_bufbase + buf_tag*USB_BO_DEV_BUF_SIZE);
}
#endif
/*
************************************************************************************************************
*
*                                             epx_out_handler_dev
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ����ȡUSB FIFO���ݺ���
*
*
************************************************************************************************************
*/
static USB_RETVAL epx_out_handler_dev(pusb_struct pusb, uint32_t ep_no, uint32_t dst_addr, uint32_t byte_count, uint32_t ep_type)
{
	USB_RETVAL ret = USB_RETVAL_NOTCOMP;
	uint32_t maxpkt;
	uint32_t ep_save = usb_get_active_ep(pusb);
	static uint32_t epout_timeout = 0;
#ifndef USB_NO_DMA
	__dma_setting_t  p;
	uint32_t dram_addr;
#endif

	usb_select_ep(pusb, ep_no);
	maxpkt = usb_get_eprx_maxpkt(pusb);
	maxpkt = (maxpkt & 0x7ff)*(((maxpkt & 0xf800)>>11)+1);

	switch(pusb->eprx_xfer_state[ep_no-1])
	{
		case USB_EPX_SETUP:
		{
			pusb->device.epx_buf_tag = 0;
			pusb->device.epx_xfer_addr = dst_addr;
			pusb->device.epx_xfer_residue = byte_count;
			pusb->device.epx_xfer_tranferred = 0;


			if (!maxpkt)
			{
				return USB_RETVAL_COMPOK;
			}

			if (byte_count>=maxpkt)
			{
				uint32_t xfer_count=0;

#ifndef USB_NO_DMA
				xfer_count = min(pusb->device.epx_xfer_residue, USB_BO_DEV_BUF_SIZE);
				pusb->dma_last_transfer = xfer_count;
				usb_fifo_accessed_by_dma(pusb, ep_no, 0);  //rx

				dram_addr = usb_dev_get_buf_base(pusb, pusb->device.epx_buf_tag);

				p.cfg.src_drq_type      = DDMA_SRC_USB0;
				p.cfg.src_addr_type     = 1;   //IO address
				p.cfg.src_burst_length  = 1;   //burst length = 4
				p.cfg.src_data_width    = 2;   //32bit
				p.cfg.dst_drq_type      = DDMA_DST_SDRAM;
				p.cfg.dst_addr_type     = 0;   //linear
				p.cfg.dst_burst_length  = 1;   //burst length = 4
				p.cfg.dst_data_width    = 2;   //32bit
				p.cfg.wait_state        = 0;
				p.cfg.continuous_mode   = 0;

				p.pgsz                  = 0;
				p.pgstp                 = 0;
				p.cmt_blk_cnt           = USB2DRAM_PARAMS;

				wBoot_dma_Setting(pusb->dma, (void *) & p);
				//���Ƕ�����������ʱ��ˢ��Ŀ�ĵ�ַ��ԭ�����ˢ��DRAM(SRAM)
				wlibc_CleanFlushDCacheRegion((void *)dram_addr, xfer_count);

				wBoot_dma_start(pusb->dma, usb_get_ep_fifo_addr(pusb, ep_no), dram_addr, xfer_count);

				usb_set_eprx_csrhi(pusb, (USB_RXCSR_AUTOCLR|USB_RXCSR_DMAREQEN)>>8);
				pusb->eprx_xfer_state[ep_no-1] = USB_EPX_DATA;
#else
			    xfer_count = min(pusb->device.epx_xfer_residue, maxpkt);
			    if (usb_get_eprx_csr(pusb) & USB_RXCSR_RXPKTRDY)
				{
					usb_fifo_accessed_by_cpu(pusb);
					if (usb_get_fifo_access_config(pusb) & 0x1)
					{
						PRINTF("Error: CPU Access Failed!!\n");
					}
					usb_read_ep_fifo(pusb, ep_no, pusb->device.epx_xfer_addr, xfer_count);
					usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb) & USB_RXCSR_ISO); //Clear RxPktRdy
					pusb->device.epx_xfer_residue -= xfer_count;
					pusb->device.epx_xfer_addr += xfer_count;
					pusb->device.epx_xfer_tranferred  += xfer_count;
					pusb->eprx_xfer_state[ep_no-1] = USB_EPX_DATA;
					epout_timeout = 0;
				}
				else
				{
					epout_timeout ++;

					if (epout_timeout < 0x1000)
					{
						ret = USB_RETVAL_NOTCOMP;
					}
					else
					{
						ret = USB_RETVAL_COMPERR;
						PRINTF("Error: RxPktRdy Timeout!!\n");
					}
				}
#endif
			}
			else
			{
				if (usb_get_eprx_csr(pusb) & USB_RXCSR_RXPKTRDY)
				{
					usb_fifo_accessed_by_cpu(pusb);
					if (usb_get_fifo_access_config(pusb) & 0x1)
					{
						PRINTF("Error: CPU Access Failed!!\n");
					}
					usb_read_ep_fifo(pusb, ep_no, pusb->device.epx_xfer_addr, byte_count);
					usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb) & USB_RXCSR_ISO); //Clear RxPktRdy
					pusb->device.epx_xfer_residue -= byte_count;
					pusb->device.epx_xfer_tranferred  += byte_count;
					pusb->eprx_xfer_state[ep_no-1] = USB_EPX_SETUP;
					ret = USB_RETVAL_COMPOK;
					epout_timeout = 0;
				}
				else
				{
					epout_timeout ++;

					if (epout_timeout < 0x1000)
					{
						ret = USB_RETVAL_NOTCOMP;
					}
					else
					{
						ret = USB_RETVAL_COMPERR;
						PRINTF("Error: RxPktRdy Timeout!!\n");
					}
				}
			}
		}
		break;

		case USB_EPX_DATA:
#ifndef USB_NO_DMA
		if (!wBoot_dma_QueryState(pusb->dma))
	 	{
	 		uint32_t data_xfered = pusb->dma_last_transfer;

		  	pusb->device.epx_xfer_residue -= data_xfered;
		  	pusb->dma_last_transfer  = 0;
		  	pusb->device.epx_buf_tag = pusb->device.epx_buf_tag ? 0:1;

		  	if (pusb->device.epx_xfer_residue)
		  	{
		  		uint32_t xfer_count = min(pusb->device.epx_xfer_residue, USB_BO_DEV_BUF_SIZE);

				pusb->dma_last_transfer = xfer_count;
		  		usb_fifo_accessed_by_dma(pusb, ep_no, 0);

		  		dram_addr = usb_dev_get_buf_base(pusb, pusb->device.epx_buf_tag);

			    p.cfg.src_drq_type      = DDMA_SRC_USB0;
			    p.cfg.src_addr_type     = 1;   //IO address
			    p.cfg.src_burst_length  = 1;   //burst length = 4
			    p.cfg.src_data_width    = 2;   //32bit
			    p.cfg.dst_drq_type      = DDMA_DST_SDRAM;
			    p.cfg.dst_addr_type     = 0;   //linear
			    p.cfg.dst_burst_length  = 1;   //burst length = 4
			    p.cfg.dst_data_width    = 2;   //32bit
			    p.cfg.wait_state        = 0;
			    p.cfg.continuous_mode   = 0;

			    p.pgsz                  = 0;
			    p.pgstp                 = 0;
			    p.cmt_blk_cnt           = USB2DRAM_PARAMS;

			    wBoot_dma_Setting(pusb->dma, (void *) & p);
			    //���Ƕ�����������ʱ��ˢ��Ŀ�ĵ�ַ��ԭ�����ˢ��DRAM(SRAM)
			    wlibc_CleanFlushDCacheRegion((void *)dram_addr, xfer_count);

			    wBoot_dma_start(pusb->dma, usb_get_ep_fifo_addr(pusb, ep_no), dram_addr, xfer_count);
		  	}
		  	else
		  	{
		  		if (byte_count & 0x1ff)
			    {
				    usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb) & USB_RXCSR_ISO);
			    }
			    else
			   	{
				   usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb) & (USB_RXCSR_ISO|USB_RXCSR_RXPKTRDY));
			   	}
			    pusb->eprx_xfer_state[ep_no-1] = USB_EPX_SETUP;
			    ret = USB_RETVAL_COMPOK;
		  	}

			if (dram_copy(usb_dev_get_buf_base(pusb, pusb->device.epx_buf_tag? 0:1), pusb->device.epx_xfer_addr, data_xfered))
		  	{
		  		pusb->device.epx_xfer_addr += data_xfered;
		  		pusb->device.epx_xfer_tranferred += data_xfered;
		  	}
		  	else
		  	{
		  		PRINTF("Error: buffer to storage copy error!!\n");
			    pusb->eprx_xfer_state[ep_no-1] = USB_EPX_SETUP;
			    ret = USB_RETVAL_COMPERR;
		  	}
	 	}
	 	else
	 	{
	 		PRINTF("dma busy\n");
	 	}
#else
		if (pusb->device.epx_xfer_residue>0)
		{
			if (usb_get_eprx_csr(pusb) & USB_RXCSR_RXPKTRDY)
			{
				uint32_t xfer_count = min(pusb->device.epx_xfer_residue, maxpkt);

				usb_fifo_accessed_by_cpu(pusb);
				if (usb_get_fifo_access_config(pusb) & 0x1)
				{
					PRINTF("Error: CPU Access Failed!!\n");
				}
				usb_read_ep_fifo(pusb, ep_no, pusb->device.epx_xfer_addr, xfer_count);
				usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb) & USB_RXCSR_ISO); //Clear RxPktRdy
				pusb->device.epx_xfer_residue -= xfer_count;
				pusb->device.epx_xfer_addr += xfer_count;
				pusb->device.epx_xfer_tranferred  += xfer_count;
				pusb->eprx_xfer_state[ep_no-1] = USB_EPX_DATA;
				epout_timeout = 0;
			}
			else
			{
				epout_timeout ++;

				if (epout_timeout < 0x1000)
				{
					ret = USB_RETVAL_NOTCOMP;
				}
				else
				{
					ret = USB_RETVAL_COMPERR;
					PRINTF("Error: RxPktRdy Timeout!!\n");
				}
			}
		}
		else
		{
			pusb->eprx_xfer_state[ep_no-1] = USB_EPX_SETUP;
			ret = USB_RETVAL_COMPOK;
		}
#endif
		break;

		default:
			PRINTF("Error: Wrong eprx_xfer_state=%d\n", pusb->eprx_xfer_state[ep_no-1]);
			pusb->eprx_xfer_state[ep_no-1] = USB_EPX_SETUP;
	}

	usb_select_ep(pusb, ep_save);

	return ret;
}
/*
************************************************************************************************************
*
*                                             epx_in_handler_dev
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ��д��USB FIFO���ݺ���
*
*
************************************************************************************************************
*/
static USB_RETVAL epx_in_handler_dev(pusb_struct pusb, uint32_t ep_no, uint32_t src_addr, uint32_t byte_count, uint32_t ep_type)
{
  	USB_RETVAL ret = USB_RETVAL_NOTCOMP;
	uint32_t maxpkt;
	uint32_t ep_save = usb_get_active_ep(pusb);
#ifndef USB_NO_DMA
	__dma_setting_t  p;
	uint32_t dram_addr;
	uint32_t ping_pang_addr;
#endif
	usb_select_ep(pusb, ep_no);
	maxpkt = usb_get_eptx_maxpkt(pusb);
	maxpkt = (maxpkt & 0x7ff)*(((maxpkt & 0xf800)>>11)+1);

	switch(pusb->eptx_xfer_state[ep_no-1])
	{
		case USB_EPX_SETUP:
		{
			pusb->device.epx_buf_tag = 0;
			pusb->device.epx_xfer_addr = src_addr;
			pusb->device.epx_xfer_residue = byte_count;
			pusb->device.epx_xfer_tranferred = 0;

			if (!maxpkt)
			{
				return USB_RETVAL_COMPOK;
			}

			if (byte_count>=maxpkt)
		 	{
#ifndef USB_NO_DMA
				uint32_t xfer_count = 0;

		 		xfer_count = min(pusb->device.epx_xfer_residue, USB_BO_DEV_BUF_SIZE);
		 		ping_pang_addr = usb_dev_get_buf_base(pusb, pusb->device.epx_buf_tag);
		 		if (dram_copy(pusb->device.epx_xfer_addr, ping_pang_addr, xfer_count))
	 			{
	 				pusb->device.epx_xfer_addr += xfer_count;
	 				pusb->device.epx_xfer_residue -= xfer_count;
	 			}
		 		else
	 			{
	 				PRINTF("Error: storage to buffer copy error!!\n");
			    	ret = USB_RETVAL_COMPERR;
			    	break;
	 			}

				usb_fifo_accessed_by_dma(pusb, ep_no, 1);
		 		if (!(usb_get_fifo_access_config(pusb) & 0x1))
			    {
					PRINTF("Error: FIFO Access Config Error!!\n");
			    }

		  		dram_addr = usb_dev_get_buf_base(pusb, pusb->device.epx_buf_tag);

			    p.cfg.src_drq_type      = DDMA_DST_SDRAM;
			    p.cfg.src_addr_type     = 0;   //linear
			    p.cfg.src_burst_length  = 1;   //burst length = 4
			    p.cfg.src_data_width    = 2;   //32bit
			    p.cfg.dst_drq_type      = DDMA_SRC_USB0;
			    p.cfg.dst_addr_type     = 1;   //IO address
			    p.cfg.dst_burst_length  = 1;   //burst length = 4
			    p.cfg.dst_data_width    = 2;   //32bit
			    p.cfg.wait_state        = 0;
			    p.cfg.continuous_mode   = 0;

			    p.pgsz                  = 0;
			    p.pgstp                 = 0;
			    p.cmt_blk_cnt           = DRAM2USB_PARAMS;

			    wBoot_dma_Setting(pusb->dma, (void *) & p);
		        //����д������д��ʱ��ˢ��Ŀ�ĵ�ַ��ԭ�����ˢ��DRAM(SRAM)
		        wlibc_CleanFlushDCacheRegion((void *)dram_addr, xfer_count);

				wBoot_dma_start(pusb->dma, dram_addr, (uint32_t)ping_pang_addr, xfer_count);

			    pusb->device.epx_buf_tag = pusb->device.epx_buf_tag ? 0:1;

			    if (pusb->device.epx_xfer_residue)
		    	{
		    		xfer_count = min(pusb->device.epx_xfer_residue, USB_BO_DEV_BUF_SIZE);

		    		if (dram_copy(pusb->device.epx_xfer_addr, usb_dev_get_buf_base(pusb, pusb->device.epx_buf_tag), xfer_count))
	    			{
	    				pusb->device.epx_xfer_addr += xfer_count;
	    				pusb->device.epx_xfer_residue -= xfer_count;
	    			}
		    		else
	    			{
	    				PRINTF("Error: storage to buffer copy error!!\n");
			        	ret = USB_RETVAL_COMPERR;
	    			}
		    		pusb->dma_last_transfer = xfer_count;
		    	}
			    else
		    	{
		    		pusb->dma_last_transfer = 0;
		    	}

		    	usb_set_eptx_csr(pusb, USB_TXCSR_AUTOSET|USB_TXCSR_TXFIFO|USB_TXCSR_DMAREQEN|USB_TXCSR_DMAREQMODE);

				pusb->eptx_xfer_state[ep_no-1] = USB_EPX_DATA;
#else
				usb_fifo_accessed_by_cpu(pusb);
				usb_write_ep_fifo(pusb, ep_no, pusb->device.epx_xfer_addr, maxpkt);
				pusb->device.epx_xfer_residue -= maxpkt;
			  	pusb->device.epx_xfer_tranferred += maxpkt;
			  	pusb->device.epx_xfer_addr += maxpkt;
				usb_set_eptx_csr(pusb, USB_TXCSR_TXFIFO|USB_TXCSR_TXPKTRDY);
			    pusb->eptx_xfer_state[ep_no-1] = USB_EPX_DATA;
#endif
		 	}
			else
			{
				usb_fifo_accessed_by_cpu(pusb);

				usb_write_ep_fifo(pusb, ep_no, pusb->device.epx_xfer_addr, byte_count);
				if (usb_get_fifo_access_config(pusb) & 0x1)
				{
					PRINTF("Error: FIFO Access Config Error!!\n");
			  	}
				usb_set_eptx_csr(pusb, USB_TXCSR_TXFIFO|USB_TXCSR_TXPKTRDY);
				pusb->eptx_xfer_state[ep_no-1] = USB_EPX_END;
			  	pusb->device.epx_xfer_residue = 0;
			  	pusb->device.epx_xfer_tranferred = byte_count;
			}
		}
		break;

		case USB_EPX_DATA:
		{
#ifndef USB_NO_DMA
			if (!wBoot_dma_QueryState(pusb->dma))
		 	{
		 		if (pusb->dma_last_transfer)
	 			{
			  		dram_addr = usb_dev_get_buf_base(pusb, pusb->device.epx_buf_tag);

				    p.cfg.src_drq_type      = DDMA_DST_SDRAM;
				    p.cfg.src_addr_type     = 0;   //linear
				    p.cfg.src_burst_length  = 1;   //burst length = 4
				    p.cfg.src_data_width    = 2;   //32bit
				    p.cfg.dst_drq_type      = DDMA_SRC_USB0;
				    p.cfg.dst_addr_type     = 1;   //IO address
				    p.cfg.dst_burst_length  = 1;   //burst length = 4
				    p.cfg.dst_data_width    = 2;   //32bit
				    p.cfg.wait_state        = 0;
				    p.cfg.continuous_mode   = 0;

				    p.pgsz                  = 0;
				    p.pgstp                 = 0;
				    p.cmt_blk_cnt           = DRAM2USB_PARAMS;

				    wBoot_dma_Setting(pusb->dma, (void *) & p);
			        //����д������д��ʱ��ˢ��Ŀ�ĵ�ַ��ԭ�����ˢ��DRAM(SRAM)
			        wlibc_CleanFlushDCacheRegion((void *)dram_addr, pusb->dma_last_transfer);

			        wBoot_dma_start(pusb->dma, dram_addr, usb_get_ep_fifo_addr(pusb, ep_no), pusb->dma_last_transfer);

			    	pusb->device.epx_buf_tag = pusb->device.epx_buf_tag ? 0:1;

			    	if (pusb->device.epx_xfer_residue)//Copy Data from storage to buffer
			    	{
			    		uint32_t xfer_count = min(pusb->device.epx_xfer_residue, USB_BO_DEV_BUF_SIZE);

			    		if (dram_copy(pusb->device.epx_xfer_addr, usb_dev_get_buf_base(pusb, pusb->device.epx_buf_tag), xfer_count))
		    			{
		    				pusb->device.epx_xfer_addr += xfer_count;
		    				pusb->device.epx_xfer_residue -= xfer_count;
		    			}
		    			else
		    			{
		    				PRINTF("Error: storage to buffer copy error!!\n");
				        	ret = USB_RETVAL_COMPERR;
		    			}
			    		pusb->dma_last_transfer = xfer_count;
			    	}
			    	else
			    	{
			    		pusb->dma_last_transfer = 0;
			    	}
	 			}
	 			else
	 			{
	 				pusb->device.epx_xfer_tranferred = byte_count;
			    	maxpkt = usb_get_eptx_maxpkt(pusb);
			    	maxpkt = (maxpkt & 0x7ff)*(((maxpkt & 0xf800)>>11)+1);
			    	if (aw_module(byte_count, maxpkt))
			    	{
				   		usb_set_eptx_csr(pusb, usb_get_eptx_csr(pusb)|USB_TXCSR_TXPKTRDY);
			   	 	}
			    	pusb->eptx_xfer_state[ep_no-1] = USB_EPX_END;
	 			}
		 	}
#else
			if (!(usb_get_eptx_csr(pusb) & USB_TXCSR_TXPKTRDY))
			{
				if (pusb->device.epx_xfer_residue > 0)   //Data is not transfered over
				{
					uint32_t xfer_count = min(pusb->device.epx_xfer_residue, maxpkt);

					usb_fifo_accessed_by_cpu(pusb);
					usb_write_ep_fifo(pusb, ep_no, pusb->device.epx_xfer_addr, xfer_count);
					pusb->device.epx_xfer_residue -= xfer_count;
			  		pusb->device.epx_xfer_tranferred += xfer_count;
			  		pusb->device.epx_xfer_addr += xfer_count;
					usb_set_eptx_csr(pusb, USB_TXCSR_TXFIFO|USB_TXCSR_TXPKTRDY);
			    	pusb->eptx_xfer_state[ep_no-1] = USB_EPX_DATA;
				}
				else if (!(usb_get_eptx_csr(pusb) & USB_TXCSR_FIFONOTEMP))
				{

					pusb->eptx_xfer_state[ep_no-1] = USB_EPX_END;
				}
			}
#endif
		}
		break;

		case USB_EPX_END:
		if (!(usb_get_eptx_csr(pusb) & 0x3))
		{
			usb_set_eptx_csr(pusb, usb_get_eptx_csr(pusb) & USB_TXCSR_ISO);
			pusb->eptx_xfer_state[ep_no-1] = USB_EPX_SETUP;
			ret = USB_RETVAL_COMPOK;
		}
		break;

		default:
			PRINTF("Error: Wrong eptx_xfer_state=%d\n", pusb->eptx_xfer_state[ep_no-1]);
			pusb->eptx_xfer_state[ep_no-1] = USB_EPX_SETUP;
	}
	usb_select_ep(pusb, ep_save);

	return ret;
}
/*
************************************************************************************************************
*
*                                             usb_dev_bulk_xfer
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
USB_RETVAL usb_dev_bulk_xfer(pusb_struct pusb)
{
	uint32_t rx_count=0;
//	pCBWPKG pCBW;
//	pCSWPKG pCSW;
	USB_RETVAL ret = USB_RETVAL_NOTCOMP;
	uint32_t ep_save = usb_get_active_ep(pusb);
//	USB_RETVAL fret = USB_RETVAL_NOTCOMP;

//	pCBW = (pCBWPKG)(pusb->buffer);
//  	pCSW = (pCSWPKG)(pusb->buffer);

	switch(pusb->device.bo_state)
	{
//		case USB_BO_IDLE:
//		case USB_BO_CBW:
//		 	if (!pusb->eprx_flag[pusb->device.bo_ep_out-1])
//  		 	{
//  				break;
//  		 	}
//
//			pusb->eprx_flag[pusb->device.bo_ep_out-1]--;
//	  		usb_select_ep(pusb, pusb->device.bo_ep_out);
//	  		if (!(usb_get_eprx_csr(pusb) & USB_RXCSR_RXPKTRDY))
//	  		{
//	  			break;
//	  		}
//	  		rx_count = usb_get_eprx_count(pusb);
//			if (rx_count != USB_CBW_LEN)
//	  		{
//	  			usb_eprx_flush_fifo(pusb);
//	  			PRINTF("Error: Not CBW, RX Data Length=%d\n",rx_count);
//	  			break;
//	  		}
//	  		do
//	  		{
//	  			ret = epx_out_handler_dev(pusb, pusb->device.bo_ep_out, (uint32_t)pusb->buffer, rx_count, USB_PRTCL_BULK);
//	  		}
//	  		while(ret == USB_RETVAL_NOTCOMP);
//
//	  		if (ret == USB_RETVAL_COMPERR)
//	  		{
//	  			PRINTF("Error: RX CBW Error\n");
//	  			break;
//	  		}
//	  		else
//	  		{
//	  			ret = USB_RETVAL_NOTCOMP;
//	  		}
//	  		if (pCBW->dCBWSig != USB_CBW_SIG)
//	  		{
//	  			PRINTF("Error: Not CBW, Error Signature=0x%x\n", pCBW->dCBWSig);
//	  			break;
//	  		}
#if 0
			{
				uint32_t i;
	            PRINTF("dCBWSig = %x\n", pCBW->dCBWSig);
	            PRINTF("dCBWTag = %x\n", pCBW->dCBWTag);
	            PRINTF("dCBWDTL = %x\n", pCBW->dCBWDTL);
	            PRINTF("bmCBWFlg = %x\n", pCBW->bmCBWFlg);
	            PRINTF("bCBWLUN = %x\n", pCBW->bCBWLUN);
	            PRINTF("bCBWCBL = %x\n", pCBW->bCBWCBL);
	            for (i=0;i<16;i++)
	            {
	            	PRINTF("CBWCB[%d] = %x\n", i, pCBW->CBWCB[i]);
				}
			}
#endif
//	  		switch(pCBW->CBWCB[0])
//	  		{
//	  			case 0x00://Test Unit Ready
//					pCSW->dCSWSig = USB_CSW_SIG;
//					pCSW->dCSWDataRes = 0;
//					pCSW->bCSWStatus = 0;

//					pusb->device.bo_xfer_addr = (uint32_t)pCSW;
//					pusb->device.bo_xfer_residue = USB_CSW_LEN;
//					pusb->device.bo_xfer_tranferred = 0;
//					fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//					if (fret == USB_RETVAL_NOTCOMP)
//					{
//						pusb->device.bo_state = USB_BO_CSW;
//					}
//					else
//					{
//						if (fret == USB_RETVAL_COMPOK)
//						{
//						 	pusb->device.bo_xfer_tranferred = pusb->device.bo_xfer_residue;
//							pusb->device.bo_xfer_residue = 0;
//							pusb->device.bo_state = USB_BO_CBW;
//						}
//						else
//						{
//						 	pusb->device.bo_state = USB_BO_CBW;
//							PRINTF("Error: CSW Send Error!!\n");
//						}
//					}
//	  				break;
//	  			case 0x1e://Remove Allow/Prevent
//					pCSW->dCSWSig = USB_CSW_SIG;
//					pCSW->dCSWDataRes = 0;
//					pCSW->bCSWStatus = 0;
//					pusb->device.bo_xfer_addr = (uint32_t)pCSW;
//					pusb->device.bo_xfer_residue = USB_CSW_LEN;
//					pusb->device.bo_xfer_tranferred = 0;
//					fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//					if (fret == USB_RETVAL_NOTCOMP)
//					{
//						pusb->device.bo_state = USB_BO_CSW;
//					}
//					else
//					{
//						pusb->device.bo_state = USB_BO_CBW;
//						PRINTF("Error: CSW Send Error!!\n");
//					}
//	  				break;
//	  			case 0x2f://Verify(10)
//					pCSW->dCSWSig = USB_CSW_SIG;
//					pCSW->dCSWDataRes = 0;
//					pCSW->bCSWStatus = 0;
//					pusb->device.bo_xfer_addr = (uint32_t)pCSW;
//					pusb->device.bo_xfer_residue = USB_CSW_LEN;
//					pusb->device.bo_xfer_tranferred = 0;
//					fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//					if (fret == USB_RETVAL_NOTCOMP)
//					{
//					 	pusb->device.bo_state = USB_BO_CSW;
//					}
//					else
//					{
//						pusb->device.bo_state = USB_BO_CBW;
//						PRINTF("Error: CSW Send Error!!\n");
//					}
//	  				break;
//	  			case 0x12://Inquiry
//	  				pusb->device.bo_xfer_addr = (uint32_t)InquiryData;
//	  				pusb->device.bo_xfer_residue = min(pCBW->dCBWDTL, 36);
//	  				pusb->device.bo_xfer_tranferred = 0;
//	  				fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//					if (fret == USB_RETVAL_NOTCOMP)
//					{
//						pusb->device.bo_state = USB_BO_TXDATA;
//					}
//					else
//					{
//						pusb->device.bo_state = USB_BO_CBW;
//						PRINTF("Error: Data Send Error!!\n");
//					}
//	  				break;
//	  			case 0x23://Read Format Capacity
//					{
//						uint8_t formatcap[12];
//						uint32_t sec_cnt[2];
//
//						formatcap[0] = 0x00;
//						formatcap[1] = 0x00;
//						formatcap[2] = 0x00;
//						formatcap[3] = 0x08;
//						formatcap[8] = 0x02;
//						formatcap[9] = 0x00;
//						formatcap[10] = 0x02;
//						formatcap[11] = 0x00;
//
//						//sec_cnt[0] = (pusb->device.bo_seccnt+1);
//			    		sec_cnt[0] = wBoot_part_capacity(pCBW->bCBWLUN);
//						//PRINTF("part index = %d, format capacity = %d\n", pCBW->bCBWLUN, sec_cnt[0]);
//						formatcap[4] = sec_cnt[0]>>24;
//						formatcap[5] = sec_cnt[0]>>16;
//						formatcap[6] = sec_cnt[0]>>8;
//						formatcap[7] = sec_cnt[0];
//
//						pusb->device.bo_xfer_addr = (uint32_t)formatcap;
//						pusb->device.bo_xfer_residue = min(pCBW->dCBWDTL, 12);
//						pusb->device.bo_xfer_tranferred = 0;
//						fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//						if (fret == USB_RETVAL_NOTCOMP)
//						{
//							pusb->device.bo_state = USB_BO_TXDATA;
//						}
//						else
//						{
//							pusb->device.bo_state = USB_BO_CBW;
//							PRINTF("Error: Data Send Error!!\n");
//						}
//					}
//	  				break;
//	  			case 0x25://Read Capacity
//					{
//						uint8_t capacity[8];
//						uint32_t sec_cnt[2];
//
//						capacity[4] = 0x00;
//						capacity[5] = 0x00;
//						capacity[6] = 0x02;
//						capacity[7] = 0x00;
//
//						//sec_cnt[0] = (pusb->device.bo_seccnt);
//					//sec_cnt[0] = (32768); //16Mbyte vdisk space
//						sec_cnt[0] = wBoot_part_capacity(pCBW->bCBWLUN) - 1;
//						//PRINTF("part index = %d, capacity = %d\n", pCBW->bCBWLUN, sec_cnt[0]);
//
//						capacity[0] = sec_cnt[0]>>24;
//						capacity[1] = sec_cnt[0]>>16;
//						capacity[2] = sec_cnt[0]>>8;
//						capacity[3] = sec_cnt[0];
//
//						pusb->device.bo_xfer_addr = (uint32_t)capacity;
//						pusb->device.bo_xfer_residue = min(pCBW->dCBWDTL, 8);
//						pusb->device.bo_xfer_tranferred = 0;
//						fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//						if (fret == USB_RETVAL_NOTCOMP)
//						{
//							pusb->device.bo_state = USB_BO_TXDATA;
//						}
//						else
//						{
//							pusb->device.bo_state = USB_BO_CBW;
//							PRINTF("Error: Data Send Error!!\n");
//						}
//					}
//	  				break;
//	  			case 0x28://Read(10)  write to host
//					{
//						uint32_t read_len = 0;
//						uint32_t read_offset = 0;
//						int32_t part_start;
//
//						read_len = pCBW->CBWCB[7];
//						read_len  << = 8;
//						read_len |= pCBW->CBWCB[8];      //���� read_len ��������
//						//read_len  << = USB_DEV_SEC_BITS;   //���� read_len ���ֽ���
//
//						read_offset = pCBW->CBWCB[2];
//						read_offset  << = 8;
//						read_offset |= pCBW->CBWCB[3];
//						read_offset  << = 8;
//						read_offset |= pCBW->CBWCB[4];
//						read_offset  << = 8;
//						read_offset |= pCBW->CBWCB[5];		//���� read_offset ��ƫ�Ƶ�����
//						//read_offset  << = USB_DEV_SEC_BITS;  //From Blocks to Bytes  //���� read_offset ��ƫ�Ƶ��ֽ���
//						pusb->device.bo_xfer_addr = pusb->device.bo_memory_base;
//
//						//����flash����
//			            part_start = wBoot_part_start(pCBW->bCBWLUN);
//						//PRINTF("part index = %d, start = %d\n", pCBW->bCBWLUN, part_start);
//						if (wBoot_block_read(read_offset + part_start, read_len, (void *)pusb->device.bo_memory_base) < 0)
//						{
//							PRINTF("part index = %d, start = %d, read_start = %d, len = %d\n", pCBW->bCBWLUN, part_start, read_offset + part_start, read_len);
//							pCSW->dCSWSig = USB_CSW_SIG;
//							pCSW->dCSWDataRes = pCBW->dCBWDTL;
//							pCSW->bCSWStatus = 1;
//							pusb->device.bo_xfer_addr = (uint32_t)pCSW;
//							pusb->device.bo_xfer_residue = USB_CSW_LEN;
//							pusb->device.bo_xfer_tranferred = 0;
//							epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//							pusb->device.bo_state = USB_BO_CSW;
//
//							PRINTF("Error: Flash Read Fail\n");
//							break;
//						}
//						read_len  << = USB_DEV_SEC_BITS;   //���� read_len ���ֽ���
//						read_offset  << = USB_DEV_SEC_BITS;  //From Blocks to Bytes  //���� read_offset ��ƫ�Ƶ��ֽ���
//
//						pusb->device.bo_xfer_residue = min(read_len, MAX_DDMA_SIZE); //Max USB Packet is 64KB    //??
//						pusb->device.bo_xfer_tranferred = 0;
//					}
//					fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//					if (fret == USB_RETVAL_NOTCOMP)
//					{
//						pusb->device.bo_state = USB_BO_TXDATA;
//					}
//					else
//					{
//						pusb->device.bo_state = USB_BO_CBW;
//						PRINTF("Error: Data Send Error!!\n");
//					}
//	  				break;
//	  			case 0x1A://Mode Sense(6)
//					pusb->device.bo_xfer_addr = (uint32_t)SenseData;
//					pusb->device.bo_xfer_residue = min(pCBW->dCBWDTL, 4);
//					pusb->device.bo_xfer_tranferred = 0;
//					fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//					if (fret == USB_RETVAL_NOTCOMP)
//					{
//					 	pusb->device.bo_state = USB_BO_TXDATA;
//					}
//					else
//					{
//						pusb->device.bo_state = USB_BO_CBW;
//						PRINTF("Error: Data Send Error!!\n");
//					}
//	  				break;
//	  			case 0x2A://Write(10)   read from host
//	  				{
//						write_len = pCBW->CBWCB[7];
//						write_len  << = 8;
//						write_len |= pCBW->CBWCB[8];			//���� write_len ��������
//						write_len  << = USB_DEV_SEC_BITS;         //���� write_len ���ֽ���
//
//						write_offset = pCBW->CBWCB[2];
//						write_offset  << = 8;
//						write_offset |= pCBW->CBWCB[3];
//						write_offset  << = 8;
//						write_offset |= pCBW->CBWCB[4];
//						write_offset  << = 8;
//						write_offset |= pCBW->CBWCB[5];			//���� write_offset ��ƫ�Ƶ�����
//						write_offset  << = USB_DEV_SEC_BITS;		//���� write_offset ��ƫ�Ƶ��ֽ���
//
//						pusb->device.bo_xfer_addr = pusb->device.bo_memory_base;
//
//						pusb->device.bo_xfer_residue = min(write_len, MAX_DDMA_SIZE);
//						pusb->device.bo_xfer_tranferred = 0;
//	  				}
//	  				fret = epx_out_handler_dev(pusb, pusb->device.bo_ep_out, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//	  				if (fret==USB_RETVAL_COMPOK)
//					{
//						int32_t flash_ret, start, nsector;
//						//write to flash
//						start = (write_offset>>9) + wBoot_part_start(pCBW->bCBWLUN);
//						nsector = write_len>>USB_DEV_SEC_BITS;
//						flash_ret = wBoot_block_write(start, nsector, (void *)pusb->device.bo_memory_base);
//						PRINTF("part index = %d, start = %d\n", pCBW->bCBWLUN, start);
//						if (flash_ret < 0)
//						{
//							PRINTF("flash write start %d sector %d failed\n", start, nsector);
//							pCSW->dCSWSig = USB_CSW_SIG;
//							pCSW->dCSWDataRes = pCBW->dCBWDTL;
//							pCSW->bCSWStatus = 1;
//							pusb->device.bo_xfer_addr = (uint32_t)pCSW;
//							pusb->device.bo_xfer_residue = USB_CSW_LEN;
//							pusb->device.bo_xfer_tranferred = 0;
//							epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//							pusb->device.bo_state = USB_BO_CSW;
//
//							PRINTF("Error: Flash Write Fail\n");
//							break;
//						}
//						pusb->device.bo_xfer_tranferred = pusb->device.bo_xfer_residue;
//						pusb->device.bo_xfer_residue = 0;
//						pCSW->dCSWSig = USB_CSW_SIG;
//						pCSW->dCSWDataRes = pCBW->dCBWDTL - pusb->device.bo_xfer_tranferred;
//						pCSW->bCSWStatus = 0;
//						pusb->device.bo_xfer_addr = (uint32_t)pCSW;
//						pusb->device.bo_xfer_residue = USB_CSW_LEN;
//						pusb->device.bo_xfer_tranferred = 0;
//						fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//						if (fret == USB_RETVAL_NOTCOMP)
//						{
//						 	pusb->device.bo_state = USB_BO_CSW;
//						}
//						else
//						{
//							pusb->device.bo_state = USB_BO_CBW;
//							PRINTF("Error: CSW Send Error!!\n");
//						}
//					}
//	  				else
//  					{
//						if (fret == USB_RETVAL_NOTCOMP)
//						{
//							pusb->device.bo_state = USB_BO_RXDATA;
//						}
//						else
//						{
//							pusb->device.bo_state = USB_BO_CBW;
//							PRINTF("Error: Rx Data Error!!\n");
//						}
//  					}
//	  				break;
//	  			default:
//	  				break;
//	  		}
			break;

//		case USB_BO_RXDATA:
//			fret = epx_out_handler_dev(pusb, pusb->device.bo_ep_out, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//			if (fret==USB_RETVAL_COMPOK)
//			{
//				int32_t flash_ret, start, nsector;
//				//write to flash
//				start = (write_offset>>9) + wBoot_part_start(pCBW->bCBWLUN);
//				nsector = write_len>>USB_DEV_SEC_BITS;
//				flash_ret = wBoot_block_write(start, nsector, (void *)pusb->device.bo_memory_base);
//				if (flash_ret < 0)
//				{
//					PRINTF("flash write start %d sector %d failed\n", start, nsector);
//					pCSW->dCSWSig = USB_CSW_SIG;
//					pCSW->dCSWDataRes = pCBW->dCBWDTL;
//					pCSW->bCSWStatus = 1;
//					pusb->device.bo_xfer_addr = (uint32_t)pCSW;
//					pusb->device.bo_xfer_residue = USB_CSW_LEN;
//					pusb->device.bo_xfer_tranferred = 0;
//					epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//					pusb->device.bo_state = USB_BO_CSW;
//
//					PRINTF("Error: Flash Write Fail\n");
//					break;
//				}
//
//
//				pusb->device.bo_xfer_tranferred = pusb->device.bo_xfer_residue;
//				pusb->device.bo_xfer_residue = 0;
//				pCSW->dCSWSig = USB_CSW_SIG;
//				pCSW->dCSWDataRes = pCBW->dCBWDTL - pusb->device.bo_xfer_tranferred;
//				pCSW->bCSWStatus = 0;
//				pusb->device.bo_xfer_addr = (uint32_t)pCSW;
//				pusb->device.bo_xfer_residue = USB_CSW_LEN;
//				pusb->device.bo_xfer_tranferred = 0;
//				epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//				pusb->device.bo_state = USB_BO_CSW;
//			}
//			else if (fret == USB_RETVAL_COMPERR)
//			{
//				PRINTF("Error: RxData Error\n");
//				pusb->device.bo_state = USB_BO_CBW;
//			}
//			break;
//
//		case USB_BO_TXDATA:
//			fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//			if (fret==USB_RETVAL_COMPOK)
//			{
//				pusb->device.bo_xfer_tranferred = pusb->device.bo_xfer_residue;
//				pusb->device.bo_xfer_residue = 0;
//				pCSW->dCSWSig = USB_CSW_SIG;
//				pCSW->dCSWDataRes = pCBW->dCBWDTL - pusb->device.bo_xfer_tranferred;
//				pCSW->bCSWStatus = 0;
//				pusb->device.bo_xfer_addr = (uint32_t)pCSW;
//				pusb->device.bo_xfer_residue = USB_CSW_LEN;
//				pusb->device.bo_xfer_tranferred = 0;
//				epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//				pusb->device.bo_state = USB_BO_CSW;
//			}
//			else if (fret == USB_RETVAL_COMPERR)
//			{
//				PRINTF("Error: TxData Error\n");
//				pusb->device.bo_state = USB_BO_CBW;
//			}
//			break;
//
//		case USB_BO_CSW:
//			fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
//			if (fret==USB_RETVAL_COMPOK)
//			{
//				pusb->device.bo_xfer_tranferred = pusb->device.bo_xfer_residue;
//				pusb->device.bo_xfer_residue = 0; //min(pCBW->dCBWDTL, 36);
//				pusb->device.bo_state = USB_BO_CBW;
//			}
//			else if (fret==USB_RETVAL_COMPERR)
//			{
//				pusb->device.bo_state = USB_BO_CBW;
//			  	PRINTF("Error: Tx CSW Error!!\n");
//			}
//			break;

		default:
			break;
	}
	usb_select_ep(pusb, ep_save);

	return ret;
}
///////////////////////////////////////////////////////////////////
//                 usb control transfer
///////////////////////////////////////////////////////////////////

/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
static uint32_t ep0_set_config_handler_dev(pusb_struct pusb, uint32_t config_ix)
{
//	uConfigDes *pconfig;
//	uIntfDes *pintf;
//	uint32_t i;
//	uint32_t fifo_addr = 1024;    //
//
//	pconfig = (uConfigDes *)(pusb->device.config_desc);
//	if (pconfig->bConfigVal != ep0_setup->wValue)
//	{
//		PRINTF("Error: Right Configval %d; Error Configval %d\n", pconfig->bConfigVal, ep0_setup->wValue);
//		return 0;
//	}
//	pintf = (uIntfDes *)( & pusb->device.config_desc[USB_CONFIG_DESC_LEN]);
//	for (i=0; i<pintf->bNumEP; i++)
//	{
//		uEPDes *pep;
//		uint32_t ep_no;
//		uint32_t ep_attr;
//		uint32_t maxpktsz;
//		//uint32_t interval;
//		uint32_t ep_dir;
//
//		pep = (uEPDes*)( & pusb->device.config_desc[USB_CONFIG_DESC_LEN + USB_INTF_DESC_LEN + USB_ENDP_DESC_LEN*i]);
//		ep_no = pep->bEPAddr & 0xf;
//		ep_dir = (pep->bEPAddr>>7) & 0x1; //0 for OUT, and 1 for IN
//		ep_attr = pep->bmAttrib & 0x3;
//		maxpktsz = pep->wMaxPktSize1 & 0x7;
//		maxpktsz  << = 8;
//		maxpktsz |= pep->wMaxPktSize0 & 0xff;
//
//		usb_select_ep(pusb, ep_no);
//		if (ep_dir)
//		{
//			usb_set_eptx_maxpkt(pusb, maxpktsz, USB_EP_FIFO_SIZE/maxpktsz);
//			usb_set_eptx_fifo_addr(pusb, fifo_addr);
//			usb_set_eptx_fifo_size(pusb, 1, USB_EP_FIFO_SIZE);
//			pusb->device.eptx_prtcl[ep_no-1] = ep_attr;
//			pusb->device.eptx_fifo[ep_no-1] = (fifo_addr << 16)|(0x1 << 15)|(maxpktsz & 0x7fff);
//			fifo_addr += ((USB_EP_FIFO_SIZE << 1)+(USB_FIFO_ADDR_BLOCK-1)) & (~(USB_FIFO_ADDR_BLOCK-1));  //Align to USB_FIFO_ADDR_BLOCK
//			if (pintf->bIntfProtocol == 0x50)  //Bulk Only Device
//			{
//				pusb->device.bo_ep_in = ep_no;
//			}
//			usb_eptx_flush_fifo(pusb);
//			usb_eptx_flush_fifo(pusb);
//		}
//		else
//		{
//			usb_set_eprx_maxpkt(pusb, maxpktsz, USB_EP_FIFO_SIZE/maxpktsz);
//			usb_set_eprx_fifo_addr(pusb, fifo_addr);
//			usb_set_eprx_fifo_size(pusb, 1, USB_EP_FIFO_SIZE);
//			pusb->device.eprx_prtcl[ep_no-1] = ep_attr;
//			pusb->device.eprx_fifo[ep_no-1] = (fifo_addr << 16)|(0x1 << 15)|(maxpktsz & 0x7fff);
//			fifo_addr += ((USB_EP_FIFO_SIZE << 1)+(USB_FIFO_ADDR_BLOCK-1)) & (~(USB_FIFO_ADDR_BLOCK-1));
//			if (pintf->bIntfProtocol == 0x50)  //Bulk Only Device
//			{
//				pusb->device.bo_ep_out = ep_no;
//			}
//			usb_eprx_flush_fifo(pusb);
//			usb_eprx_flush_fifo(pusb);
//		}
//	}

	return 1;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
static int32_t ep0_in_handler_dev(pusb_struct pusb, const uSetupPKG * ep0_setup)
{
	uint32_t temp = 0;
	uint16_t len;
	const uint8_t *pbuf;
	const uint_fast8_t index = LO_BYTE(ep0_setup->wValue);

	if ((ep0_setup->bmRequest & 0x60)==0x00)
	{
		switch(ep0_setup->bRequest)
		{
			case USB_REQ_GET_STATUS :
    			PRINTF("usb_device: Get Status\n");
				pusb->ep0_xfer_residue = 0;
    			break;
			case USB_REQ_GET_DESCRIPTOR :
				switch(HI_BYTE(ep0_setup->wValue))
				{
					case 0x01:              //Get Device Desc
						pusb->ep0_maxpktsz = 64;
						pusb->ep0_xfer_srcaddr = (uint32_t) DeviceDescrTbl [0].data;
						pusb->ep0_xfer_residue = min(DeviceDescrTbl [0].size, ep0_setup->wLength);
						break;
					case 0x02:              //Get Configuration Desc
						if (index < ARRAY_SIZE(ConfigDescrTbl) &&  ConfigDescrTbl [index].size != 0)
						{
							pusb->ep0_xfer_srcaddr = (uint32_t) ConfigDescrTbl [index].data;
							pusb->ep0_xfer_residue = min(ConfigDescrTbl [index].size, ep0_setup->wLength);
						}
						else
						{
							PRINTF("Unkown Config Desc!!\n");
							pusb->ep0_xfer_residue = 0;
						}
						break;
					case 0x03:             //Get String Desc
						if (index == 0xEE)
						{
							// WCID devices support
							// Microsoft OS String Descriptor, ReqLength=0x12
							// See OS_Desc_Intro.doc, Table 3 describes the OS string descriptor’s fields.
							if (MsftStringDescr [0].data != NULL &&  MsftStringDescr [0].size != 0)
							{
								pusb->ep0_xfer_srcaddr = (uint32_t) MsftStringDescr [0].data;
								pusb->ep0_xfer_residue = min(MsftStringDescr [0].size, ep0_setup->wLength);
							}
							else
							{
								pusb->ep0_xfer_residue = 0;
							}
						}
						else if (index < usbd_get_stringsdesc_count() &&  StringDescrTbl [index].size != 0)
						{
							pusb->ep0_xfer_srcaddr = (uint32_t) StringDescrTbl [index].data;
							pusb->ep0_xfer_residue = min(StringDescrTbl [index].size, ep0_setup->wLength);
						}
						else
						{
							PRINTF("Unkown String Desc!!\n");
							pusb->ep0_xfer_residue = 0;
						}
						break;
					case 0x04:           //Get Interface Desc
					    pusb->ep0_xfer_residue = 0;
					    PRINTF("usb_device: Get Interface Descriptor\n");
				    	break;
					case 0x05:          //Get Endpoint Desc
					    pusb->ep0_xfer_residue = 0;
					    PRINTF("usb_device: Get Endpoint Descriptor\n");
				    	break;
					case 0x06:           //Get Device Qualifier
						if (index < ARRAY_SIZE(DeviceQualifierTbl) &&  DeviceQualifierTbl [index].size != 0)
						{
							pusb->ep0_xfer_srcaddr = (uint32_t)DeviceQualifierTbl [index].data;
							pusb->ep0_xfer_residue = min(DeviceQualifierTbl [index].size, ep0_setup->wLength);
						}
						else
						{
							pusb->ep0_xfer_residue = 0;
						}
				    	break;
//					case 0x09:
//					    pusb->ep0_xfer_srcaddr = (uint32_t)pusb->device.otg_desc;
//					    pusb->ep0_xfer_residue = min(*((uint8_t*)pusb->ep0_xfer_srcaddr), ep0_setup->wLength);
//				    	break;
					case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
						if (pusb->speed == USB_SPEED_HS &&  index < ARRAY_SIZE(OtherSpeedConfigurationTbl) &&  OtherSpeedConfigurationTbl [index].size != 0)
						{
							pusb->ep0_xfer_srcaddr = (uint32_t) OtherSpeedConfigurationTbl [index].data;
							pusb->ep0_xfer_residue = min(OtherSpeedConfigurationTbl [index].size, ep0_setup->wLength);
						}
						break;
					default  :
					    pusb->ep0_xfer_residue = 0;
					    PRINTF("usb_device: Get Unkown Descriptor 0x%02X\n", (ep0_setup->wValue>>8) & 0xff);
				}
		      	break;
	     	case USB_REQ_GET_CONFIGURATION :
				pusb->ep0_xfer_residue = 0;
				PRINTF("usb_device: Get Configuration\n");
				break;
			case USB_REQ_GET_INTERFACE :
				pusb->ep0_xfer_residue = 0;
				PRINTF("usb_device: Get Interface\n");
				break;
			case USB_REQ_SYNCH_FRAME :
				pusb->ep0_xfer_residue = 0;
				PRINTF("usb_device: Sync Frame\n");
				break;
	      	default   :
	        	pusb->ep0_xfer_residue = 0;
	        	PRINTF("usb_device: Unkown Standard Request = 0x%x\n", ep0_setup->bRequest);
		}
	}
	else if ((ep0_setup->bmRequest & 0x60)==0x20)
	{
		switch(ep0_setup->bRequest)
		{
//			case 0x00FE :
//				pusb->ep0_xfer_srcaddr = (uint32_t) & (pusb->device.MaxLUN);
//				pusb->ep0_xfer_residue = 1;
//				PRINTF("usb_device: Get MaxLUN\n");
//				break;
			default     :
				pusb->ep0_xfer_residue = 0;
				PRINTF("usb_device: Unkown Class-Specific Request = 0x%x\n", ep0_setup->bRequest);
		}
	}
	else
	{
		pusb->ep0_xfer_residue = 0;
		PRINTF("usb_device: Unkown EP0 IN!!\n");
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
static int32_t ep0_out_handler_dev(pusb_struct pusb, const uSetupPKG * ep0_setup)
{

	switch(ep0_setup->bRequest)
	{
		case USB_REQ_CLEAR_FEATURE :
			if (ep0_setup->wIndex & 0x80)
			{

			}
			else
			{

			}
		  	break;

		case USB_REQ_SET_FEATURE :
			 switch (ep0_setup->wValue)
			 {
			 	case 0x0002:
	          		switch(ep0_setup->wIndex)
	          		{
	           			case 0x0100:
	             			usb_set_test_mode(pusb, 0x02);
	             			PRINTF("usb_device: Send Test J Now...\n");
	            			break;
						case 0x0200:
							usb_set_test_mode(pusb, 0x04);
							PRINTF("usb_device: Send Test K Now...\n");
							break;
						case 0x0300:
							usb_set_test_mode(pusb, 0x01);
							PRINTF("usb_device: Test SE0_NAK Now...\n");
							break;
						case 0x0400:
							usb_write_ep_fifo(pusb, 0, (uint32_t)TestPkt, 53);
							usb_set_ep0_csr(pusb, 0x02);
							usb_set_test_mode(pusb, 0x08);

	          				PRINTF("usb_device: Send Test Packet Now...\n");
	         				break;
				        	default:
				          	PRINTF("usb_device: Unkown Test Mode: 0x%x\n", ep0_setup->wIndex);
				    }
	        		break;

				case 0x0003:
				case 0x0004:
				case 0x0005:
					PRINTF("usb_device: HNP Enable...\n");
					break;

				default:
				PRINTF("usb_device: Unkown SetFeature Value: 0x%02X\n", ep0_setup->wValue);
			}
			break;

		case USB_REQ_SET_ADDRESS :
			usb_set_dev_addr(pusb, LO_BYTE(ep0_setup->wValue));
       		pusb->device.func_addr = LO_BYTE(ep0_setup->wValue);
       		PRINTF("usb_device: Set Address 0x%02X\n", LO_BYTE(ep0_setup->wValue));
			break;

		case USB_REQ_SET_DESCRIPTOR :
       		PRINTF("usb_device: Set Descriptor\n");
      		break;
    	case USB_REQ_SET_CONFIGURATION :
       		PRINTF("usb_device: Set Configuration 0x%02X\n", LO_BYTE(ep0_setup->wValue));
    		ep0_set_config_handler_dev(pusb, LO_BYTE(ep0_setup->wValue));
    		break;
    	case USB_REQ_SET_INTERFACE :
	       	PRINTF("usb_device: Set Interface 0x%02X\n", LO_BYTE(ep0_setup->wIndex));
	      	break;
    	default   :
      		PRINTF("usb_device: Unkown EP0 OUT: 0x%02X!!\n", ep0_setup->bRequest);
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
uint32_t usb_dev_ep0xfer_handler(pusb_struct pusb)
{
	//uint32_t i=0;
	uint32_t ep0_csr;
	uint32_t is_last;
	uint32_t byte_trans;
	//uint32_t src_addr;

//	if (pusb->role != USB_ROLE_DEV) return 0;
//	if (!pusb->ep0_flag) return 0;
//	pusb->ep0_flag--;

	usb_select_ep(pusb, 0);
  	ep0_csr = usb_get_ep0_csr(pusb);

	if (pusb->ep0_xfer_state == USB_EP0_DATA)  //Control IN Data Stage or Stage Status
	{
		if (ep0_csr & 0x1)
		{
			pusb->ep0_xfer_state = USB_EP0_SETUP;
		}
		else if (ep0_csr & (0x1 << 4))
		{
			usb_set_ep0_csr(pusb, 0x80);
			PRINTF("WRN: EP0 Setup End!!\n");
		}
		else if (!(ep0_csr & (0x1 << 1)))
		{
			if (pusb->ep0_xfer_residue)
		 	{
		 		pusb->ep0_xfer_srcaddr += pusb->ep0_maxpktsz;
		 		if (pusb->ep0_xfer_residue == 0xffffffff)
			  	{
		 			TP();
				  	is_last = 1;
				  	byte_trans = 0;
				  	pusb->ep0_xfer_residue = 0;
			   	}
			  	else
			   	{
			   		if (pusb->ep0_xfer_residue < pusb->ep0_maxpktsz)
			    	{
						is_last = 1;
						byte_trans = pusb->ep0_xfer_residue;
						pusb->ep0_xfer_residue = 0;
			    	}
			   		else if (pusb->ep0_xfer_residue == pusb->ep0_maxpktsz)
					{
						is_last = 0;
						byte_trans = pusb->ep0_xfer_residue;
						pusb->ep0_xfer_residue = 0xffffffff;  //Send a zero packet next time
					}
					else
					{
						is_last = 0;
						byte_trans = pusb->ep0_maxpktsz;
						pusb->ep0_xfer_residue -= pusb->ep0_maxpktsz;
			   		}
			 	}

			 	usb_write_ep_fifo(pusb, 0, pusb->ep0_xfer_srcaddr, byte_trans);

			 	if (is_last || (!byte_trans))
			 	{
			 		usb_set_ep0_csr(pusb, 0x0a);
			 	}
			 	else
			 	{
			 		usb_set_ep0_csr(pusb, 0x02);
				}

			 	if (usb_get_ep0_count(pusb))
			 	{
		  			PRINTF("Error: COUNT0 = 0x%x\n", usb_get_ep0_count(pusb));
		 		}
		 	}
		}
		else
		{
			PRINTF("WRN: Unkown EP0 Interrupt, CSR=0x%x!!\n", ep0_csr);
		}
	}

	if (pusb->ep0_xfer_state == USB_EP0_SETUP)  //Setup or Control OUT Status Stage
	{
		const uSetupPKG * const ep0_setup = (const uSetupPKG *) (pusb->setupb);
		if (ep0_csr & 0x1)
		{
			uint32_t ep0_count = usb_get_ep0_count(pusb);

			if (ep0_count==8)
			{
				//pusb->ep0_flag = 0;
				usb_read_ep_fifo(pusb, 0, (uint32_t)pusb->setupb, 8);

				if (ep0_setup->bmRequest & 0x80)//in
				{
					usb_set_ep0_csr(pusb, 0x40);
					ep0_in_handler_dev(pusb, ep0_setup);

					if (pusb->ep0_xfer_residue<pusb->ep0_maxpktsz)
					{
						is_last = 1;
						byte_trans = pusb->ep0_xfer_residue;
						pusb->ep0_xfer_residue = 0;
					}
				 	else if (pusb->ep0_xfer_residue==pusb->ep0_maxpktsz)
					{
						is_last = 0;
						byte_trans = pusb->ep0_xfer_residue;
						pusb->ep0_xfer_residue = 0xffffffff;
					}
					else
					{
						is_last = 0;
						byte_trans = pusb->ep0_maxpktsz;
						pusb->ep0_xfer_residue -= pusb->ep0_maxpktsz;
					}

				 	usb_write_ep_fifo(pusb, 0, pusb->ep0_xfer_srcaddr, byte_trans);
				 	if (is_last || (!byte_trans))
				 	{
				 		usb_set_ep0_csr(pusb, 0x0a);
				   	}
				   	else
				   	{
				   		usb_set_ep0_csr(pusb, 0x02);
				   	}

				   	pusb->ep0_xfer_state = USB_EP0_DATA;
				}
				else                         //out
				{
					usb_set_ep0_csr(pusb, 0x48);
					pusb->ep0_xfer_state = USB_EP0_SETUP;
				}
			}
			else
			{
			  	usb_ep0_flush_fifo(pusb);
		    	PRINTF("Error: EP0 Rx Error Length = 0x%x\n", ep0_count);
			}
		}
		else
		{
			ep0_out_handler_dev(pusb, ep0_setup);
		}
	}

	return 1;
}
///////////////////////////////////////////////////////////////////
//                usb storage function
///////////////////////////////////////////////////////////////////
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
uint32_t usb_bus_irq_handler_dev(pusb_struct pusb, uint32_t busirq_status)
{
	//uint32_t busirq_en;

	if (pusb->role != USB_ROLE_DEV) return 0;
  	//if (!pusb->busirq_flag) return 0;

	uint32_t ep_save = usb_get_active_ep(pusb);
  	usb_select_ep(pusb, 0);
  	//pusb->busirq_flag--;
  	//busirq_en = usb_get_bus_interrupt_enable(pusb);


//  	if (busirq_status & USB_BUSINT_CONNECT)
//  	{
//
//	  	busirq_status  & = ~ USB_BUSINT_CONNECT;
//	  	PRINTF("uCommect\n");
//  	}
//  	if (busirq_status & USB_BUSINT_DISCONN)
//  	{
//
//	  	busirq_status  & = ~ USB_BUSINT_DISCONN;
//	  	PRINTF("uDisommect\n");
//  	}

 	if (busirq_status & USB_BUSINT_SUSPEND)
  	{
	  	//busirq_status  & = ~ USB_BUSINT_SUSPEND;
	  	//Suspend Service Subroutine
	  	pusb->suspend = 1;

		if (wBoot_dma_QueryState(pusb->dma))
		{
			PRINTF("Error: DMA for EP is not finished after Bus Suspend\n");
		}
		wBoot_dma_stop(pusb->dma);
	  	PRINTF("uSuspend\n");
  	}

	if (busirq_status & USB_BUSINT_RESUME)
	{
		//busirq_status  & = ~USB_BUSINT_RESUME;
		//Resume Service Subroutine
		pusb->suspend = 0;
		PRINTF("uResume\n");
	}

	if (busirq_status & USB_BUSINT_RESET)
	{
		uint32_t i;
		//busirq_status  & = ~USB_BUSINT_RESET;
		//Device Reset Service Subroutine
		pusb->connect = 1;
		pusb->reset = 1;
		pusb->suspend = 0;
		pusb->rst_cnt ++;
		for (i=0; i<USB_MAX_EP_NO; i++)
		{
			//pusb->eptx_flag[i] = 0;
			//pusb->eprx_flag[i] = 0;
			pusb->eptx_xfer_state[i] = USB_EPX_SETUP;
			pusb->eprx_xfer_state[i] = USB_EPX_SETUP;
		}
		pusb->device.func_addr = 0x00;
		pusb->device.bo_state = USB_BO_IDLE;
		pusb->timer = USB_IDLE_TIMER;

		//Reset Function Address
		usb_set_dev_addr(pusb, 0x00);
		//Bus Reset may disable all interrupt enable, re-enable the interrupts need
		usb_set_bus_interrupt_enable(pusb, USB_BUSINT_DEV_WORK);
		usb_set_eptx_interrupt_enable(pusb, 0xffff);
		usb_set_eprx_interrupt_enable(pusb, 0xfffe);

		if (wBoot_dma_QueryState(pusb->dma))
		{
			PRINTF("Error: DMA for EP is not finished after Bus Reset\n");
		}
		wBoot_dma_stop(pusb->dma);
	  	PRINTF("uReset\n");
	}

  	if (busirq_status & USB_BUSINT_SESSEND)
  	{
  		//busirq_status  & = ~USB_BUSINT_SESSEND;
  		//Device Reset Service Subroutine
  		pusb->connect = 0;
		pusb->reset = 0;
		pusb->suspend = 1;
		PRINTF("uSessend\n");
  	}

  	usb_select_ep(pusb, ep_save);

  	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
//
//void usb_power_polling_dev(pusb_struct pusb)
//{
//	if (pusb->role != USB_ROLE_DEV) return;
// 	if (pusb->connect) return;
//
//  	if (usb_get_vbus_level(pusb) == USB_VBUS_VBUSVLD)
//  	{
//		if (pusb->timer != USB_DEVICE_VBUS_DET_TIMER)
//		{
//			if (pusb->timer != USB_IDLE_TIMER)  //timer should not occupied by any other ones at this time
//			{
//				PRINTF("usb_power_polling_dev: Error: Timer Ocuppied\n");
//			}
//		    pusb->timer = USB_DEVICE_VBUS_DET_TIMER;
//
//		   	pusb->loop = 0;
//		   	pusb->power_debouce = 0x50;
//
//			return;
//		}
//		else if (pusb->power_debouce > 0)
//		{
//			pusb->loop ++;
//		   	pusb->power_debouce --;
//
//			return;
//		}
//	}
//  	else
//  	{
//    	pusb->timer = USB_IDLE_TIMER;  //Release Timer and return
//  		pusb->loop = 0;
//  		return;
//  	}
//
//	pusb->loop ++;
//	pusb->timer = USB_IDLE_TIMER;
//	pusb->loop = 0;
//	usb_soft_connect(pusb);
//	pusb->connect = 1;
//	PRINTF("usb_power_polling_dev: USB Connect!!\n");
//
//	return;
//}
///////////////////////////


//static uint32_t bus_irq_count = 0;
//static uint32_t ep0_irq_count = 0;
//static uint32_t eptx_irq_count = 0;
//static uint32_t eprx_irq_count = 0;

//static usb_struct awxx_usb[USB_SIE_COUNT];

//extern  uint32_t usb_device_function(pusb_struct pusb);

void musb2_prepare(pusb_struct pusb)
{
	uint32_t i;

	//pusb->index = 0; /* usb_drive_vbus parameter */
	//pusb->reg_base = USBOTG0_BASE;
	//pusb->irq_no = USB0_DEVICE_IRQn;
	//pusb->drq_no = 0x04;

	pusb->role = USB_ROLE_DEV;  //USB_ROLE_HST; //USB_ROLE_UNK
#if WITHUSBDEV_HSDESC
	pusb->speed = USB_SPEED_HS;
#else /* WITHUSBDEV_HSDESC */
	pusb->speed = USB_SPEED_FS;
#endif /* WITHUSBDEV_HSDESC */

	//pusb->ep0_flag = 0;
	pusb->ep0_xfer_state = USB_EP0_SETUP;
	//dma
	pusb->dma = wBoot_dma_request(1);
	if (!pusb->dma)
	{
		PRINTF("usb error: request dma fail\n");
	}

}

// ep_ix: 0..USB_MAX_EP_NO-1 (other then EP0)
void usb_dev_eprx_handler(pusb_struct pusb, uint32_t ep_ix)
{
	PRINTF("usb_dev_eprx_handler: ep_ix=%u\n", ep_ix);
}

// ep_ix: 0..USB_MAX_EP_NO-1 (other then EP0)
void usb_dev_eptx_handler(pusb_struct pusb, uint32_t ep_ix)
{
	PRINTF("usb_dev_eptx_handler: ep_ix=%u\n", ep_ix);
}

void usb_struct_init(pusb_struct pusb)
{
	uint32_t i;

	//pusb->sof_count = 0;

	//pusb->busirq_flag = 0;
	//pusb->busirq_status = 0;

	pusb->connect = 0;
	pusb->reset = 0;
	pusb->suspend = 1;

	pusb->rst_cnt = 0;
	pusb->cur_fsm = 0;
	pusb->fsm_cnt = 0;

	//pusb->ep0_flag = 0;
	pusb->ep0_xfer_state = USB_EP0_SETUP;
	pusb->ep0_maxpktsz = 64;

	for (i=0; i<USB_MAX_EP_NO; i++)
	{
		//pusb->eptx_flag[i] = 0;
		//pusb->eprx_flag[i] = 0;
		pusb->eptx_xfer_state[i] = USB_EPX_SETUP;
		pusb->eprx_xfer_state[i] = USB_EPX_SETUP;
	}

	pusb->device.func_addr = 0x00;
	pusb->device.bo_state = USB_BO_IDLE;
	pusb->device.bo_ep_in = 1;
	pusb->device.bo_ep_out = 1;


	pusb->timer = USB_IDLE_TIMER;
	pusb->loop = 0;

	//pusb->dma.valid = 0;
	//pusb->dma.type = DMAT_DDMA;                    //

//	bus_irq_count = 0;
//	ep0_irq_count = 0;
//	eptx_irq_count = 0;
//	eprx_irq_count = 0;

	return;
}

static void usb_init(pusb_struct pusb)
{
	//uint32_t i=0;
	//uint32_t temp;


	//usb_set_phytune(pusb);
	usb_drive_vbus(pusb, 0, 0);

	usb_force_id(pusb, 1);

	if (pusb->speed == USB_SPEED_FS)
		usb_high_speed_disable(pusb);
	else
		usb_high_speed_enable(pusb);

	usb_suspendm_enable(pusb);

	usb_vbus_src(pusb, 0x1);
	usb_release_vbus(pusb);
	usb_force_vbus(pusb, 1);

	usb_select_ep(pusb, 0);
	usb_ep0_flush_fifo(pusb);

	//PRINTF("USB Device!!\n");

	pusb->role = USB_ROLE_DEV;

	usb_clear_bus_interrupt_enable(pusb, 0xff);
	usb_set_bus_interrupt_enable(pusb, USB_BUSINT_DEV_WORK);
	usb_set_eptx_interrupt_enable(pusb, 0xffff);
	usb_set_eprx_interrupt_enable(pusb, 0xfffe);

	pusb->otg_dev = USB_OTG_B_DEVICE;

	// перенесено из опроса
	usb_soft_connect(pusb);
	pusb->connect = 1;

	return;
}
//
//uint32_t musb2_stop(usb_struct * pusb)
//{
//	arm_hardware_disable_handler(pusb->irq_no);
//
//	return 1;
//}
//
//void musb2_process(void)
//{
//
// 	 usb_device_function(& awxx_usb[0]);
//}

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
//
//  __HAL_LOCK(hpcd);
//
//#if ! WITHNEWUSBHAL
//  (void)USB_EPSetStall(hpcd->Instance, ep);
//#else
//  if ((ep_addr & EP_ADDR_MSK) == 0U)
//  {
//	  USBPhyHw_ep0_stall(hpcd);
//  }
//  else
//  {
//	  USBPhyHw_endpoint_stall(hpcd, ep_addr);
//
//  }
//#endif
//  if ((ep_addr & EP_ADDR_MSK) == 0U)
//  {
//    (void)USB_EP0_OutStart(hpcd->Instance, (uint8_t)hpcd->Init.dma_enable, (uint8_t *)hpcd->Setup);
//  }
//
//  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}
// https://github.com/abmwine/FreeBSD-src/blob/86cb59de6f4c60abd0ea3695ebe8fac26ff0af44/sys/dev/usb/controller/musb_otg_allwinner.c
// https://github.com/abmwine/FreeBSD-src/blob/86cb59de6f4c60abd0ea3695ebe8fac26ff0af44/sys/dev/usb/controller/musb_otg.c

void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
{
	pusb_struct pusb = & hpcd->awxx_usb;

	//uint32_t index;

	//bus interrupt
	{
		uint32_t busintstatus;
		busintstatus = usb_get_bus_interrupt_status(pusb);
		usb_clear_bus_interrupt_status(pusb, busintstatus);

//		if (busintstatus & USB_BUSINT_SOF)
//		{
//			//pusb->sof_count ++;
//			busintstatus &= ~ USB_BUSINT_SOF;
//		}

		busintstatus &= usb_get_bus_interrupt_enable(pusb);
		if (busintstatus != 0)
		{
			//pusb->busirq_status |= temp;
			//pusb->busirq_flag ++;
			//bus_irq_count ++;
			usb_bus_irq_handler_dev(pusb, busintstatus);
		}
	}

	//tx interrupt, ep0 interrupt
	{
		uint32_t eptxstatus;
		eptxstatus = usb_get_eptx_interrupt_status(pusb);
		usb_clear_eptx_interrupt_status(pusb, eptxstatus);

		if (eptxstatus & 0x01)
		{
			//pusb->ep0_flag ++;
			//ep0_irq_count ++;
			usb_dev_ep0xfer_handler(pusb);
		}
		if (eptxstatus & 0xfffe)
		{
			uint32_t i;
			for (i=0; i<USB_MAX_EP_NO; i++)
			{
				if (eptxstatus & (0x2 << i))
				{
					//pusb->eptx_flag[i] ++;
					usb_dev_eptx_handler(pusb, i);
				}
			}
			//eptx_irq_count ++;
		}
	}

	//rx interrupt
	{
		uint32_t eprxstatus;
		eprxstatus = usb_get_eprx_interrupt_status(pusb);
		usb_clear_eprx_interrupt_status(pusb, eprxstatus);

		if (eprxstatus & 0xfffe)
		{
			uint32_t i;
			for (i=0; i<USB_MAX_EP_NO; i++)
			{
				if (eprxstatus & (0x2 << i))
				{
					//pusb->eprx_flag[i] ++;
					usb_dev_eprx_handler(pusb, i);
				}
			}
			//eprx_irq_count ++;
		}
	}
}

/**
  * @brief  USB_DevConnect : Connect the USB device by enabling Rpu
  * @param  USBx  Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_DevConnect(USBOTG_TypeDef *USBx)
{
	PRINTF("USB_DevConnect\n");
    return HAL_OK;
	/* Enable all nnterrupts */
	USBx->USB_INTUSBE = 0xFF & ~ MUSB2_MASK_ISOF;
	USBx->USB_INTRXE = 0x3F;
	USBx->USB_INTTXE = 0x3F;

	PRINTF("USB_DevConnect: USB_INTUSBE=%08lX\n", USBx->USB_INTUSBE);
	PRINTF("USB_DevConnect: USB_INTTXE=%08lX\n", USBx->USB_INTTXE);
	PRINTF("USB_DevConnect: USB_INTRXE=%08lX\n", USBx->USB_INTRXE);
//    /* Enable pullup on D+ */
//    USBx->INTENB0 |= (USB_VBSE | USB_SOFE | USB_DVSE | USB_CTRE | USB_BEMPE | USB_NRDYE | USB_BRDYE);
//    USBx->SYSCFG0 |= USB_DPRPU;
//
//    /* Enable USB */
//	arm_hardware_set_handler_system(USB0_DEVICE_IRQn, device_OTG_HS_IRQHandler);
////    InterruptHandlerRegister(USBIX_IRQn, &_usbisr);
////    GIC_SetPriority(USBIX_IRQn, 16);
////    GIC_SetConfiguration(USBIX_IRQn, 1);
////    GIC_EnableIRQ(USBIX_IRQn);
//	//arm_hardware_set_handler_system(USBIX_IRQn, _usbisr);

    /* Enable pullup on D+ */
	USBx->USB_POWER |= MUSB2_MASK_SOFTC;

    return HAL_OK;
}

/**
  * @brief  USB_DevDisconnect : Disconnect the USB device by disabling Rpu
  * @param  USBx  Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_DevDisconnect(USBOTG_TypeDef *USBx)
{
	PRINTF("USB_DevDisconnect\n");
	return HAL_OK;
	USBx->USB_INTUSBE = 0;
	USBx->USB_INTTXE = 0;
	USBx->USB_INTRXE = 0;
	/* Disable USB */
	//arm_hardware_disable_handler(USB0_DEVICE_IRQn);
////    InterruptHandlerRegister(USBIX_IRQn, NULL);
//    if (USBx == & UDB0)
//	{
//    	arm_hardware_disable_handler(USBI0_IRQn);
//	}
//	else if (USBx == & USB201)
//	{
//		arm_hardware_disable_handler(USBI1_IRQn);
//	}
//
	/* Disable pullup on D+ */
	USBx->USB_POWER &= ~ MUSB2_MASK_SOFTC;

	return HAL_OK;
}

/**
  * @brief  USB_StopDevice : Stop the usb device mode
  * @param  USBx  Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef USB_StopDevice(USBOTG_TypeDef *USBx)
{
  HAL_StatusTypeDef ret = HAL_OK;
//  uint32_t USBx_BASE = (uint32_t)USBx;
//  uint32_t i;
//
//  /* Clear Pending interrupt */
//  for (i = 0U; i < 15U; i++)
//  {
//    USBx_INEP(i)->DIEPINT = 0xFB7FU;
//    USBx_OUTEP(i)->DOEPINT = 0xFB7FU;
//  }
//
//  /* Clear interrupt masks */
//  USBx_DEVICE->DIEPMSK  = 0U;
//  USBx_DEVICE->DOEPMSK  = 0U;
//  USBx_DEVICE->DAINTMSK = 0U;
//
//  /* Flush the FIFO */
//  ret = USB_FlushRxFifo(USBx);
//  if (ret != HAL_OK)
//  {
//    return ret;
//  }
//
//  ret = USB_FlushTxFifo(USBx,  0x10U);
//  if (ret != HAL_OK)
//  {
//    return ret;
//  }

  return ret;
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
HAL_StatusTypeDef USB_SetCurrentMode(USBOTG_TypeDef *USBx, USB_OTG_ModeTypeDef mode)
{
//	switch (mode)
//	{
//	case USB_OTG_DEVICE_MODE:
//		USBx->SYSCFG0 &= ~ USB_SYSCFG_DCFM;	// DCFM 0: Devide controller mode is selected
//		(void) USBx->SYSCFG0;
//		break;
//	case USB_OTG_HOST_MODE:
//		USBx->SYSCFG0 |= USB_SYSCFG_DCFM;	// DCFM 1: Host controller mode is selected
//		(void) USBx->SYSCFG0;
//		break;
//	case USB_OTG_DRD_MODE:
//		ASSERT(0);
//		//USBx->SYSCFG0 |= USB_SYSCFG_DCFM;	// DCFM 1: Host controller mode is selected
//		break;
//	}

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
HAL_StatusTypeDef USB_DevInit(USBOTG_TypeDef *USBx, USB_OTG_CfgTypeDef cfg)
{
	//TP();
//	unsigned i;
//
//	USBx->SYSCFG0 &= ~ USB_SYSCFG_USBE;	// USBE 0: USB module operation is disabled.
//	(void) USBx->SYSCFG0;
//
//	USBx->SOFCFG =
//		//USB_SOFCFG_BRDYM |	// BRDYM
//		0;
//	(void) USBx->SOFCFG;
//
//	USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_DPRPU | USB_SYSCFG_DRPD)) |
//			0 * USB_SYSCFG_DPRPU |	// DPRPU 0: Pulling up the D+ line is disabled.
//			0 * USB_SYSCFG_DRPD |	// DRPD 0: Pulling down the lines is disabled.
//			0;
//	(void) USBx->SYSCFG0;
//
//	USBx->SYSCFG0 |= USB_SYSCFG_USBE;	// USBE 1: USB module operation is enabled.
//	(void) USBx->SYSCFG0;
//
//	//PRINTF("USB_DevInit: cfg->speed=%d\n", (int) cfg->speed);
//	USBx->SYSCFG0 = (USBx->SYSCFG0 & ~ (USB_SYSCFG_HSE)) |
//			(cfg.speed == PCD_SPEED_HIGH) * USB_SYSCFG_HSE |	// HSE
//			0;
//	(void) USBx->SYSCFG0;
//
//	USBx->INTSTS0 = 0;
//	USBx->INTSTS1 = 0;
//
//	USBx->INTENB0 =
//		(cfg.Sof_enable != DISABLE) * USB_INTENB0_SOFE |	// SOFE	1: Frame Number Update Interrupt Enable
//		1 * USB_INTENB0_DVSE |	// DVSE
//		//1 * USB_INTENB0_VBSE |	// VBSE
//		1 * USB_INTENB0_CTRE |	// CTRE
//		1 * USB_INTENB0_BEMPE |	// BEMPE
//		1 * USB_INTENB0_NRDYE |	// NRDYE
//		1 * USB_INTENB0_BRDYE |	// BRDYE
//		1 * USB_INTENB0_RSME |	// RSME
//		0;
//	USBx->INTENB1 = 0;
//
//	// When the function controller mode is selected, set all the bits in this register to 0.
//	for (i = 0; i < USB20_DEVADD0_COUNT; ++ i)
//	{
//		volatile uint16_t * const DEVADDn = (& USBx->DEVADD0) + i;
//
//		// Reserved bits: The write value should always be 0.
//		* DEVADDn = 0;
//		(void) * DEVADDn;
//	}

	return HAL_OK;
}

HAL_StatusTypeDef USB_CoreInit(USBOTG_TypeDef * USBx, USB_OTG_CfgTypeDef cfg)
{
//	// P1 clock (66.7 MHz max) period = 15 ns
//	// The cycle period required to consecutively access registers of this controller must be at least 67 ns.
//	// TODO: compute BWAIT value on-the-fly
//	// Use P1CLOCK_FREQ
//	const uint_fast32_t bwait = MIN(MAX(calcdivround2(P1CLOCK_FREQ, 15000000uL), 2) - 2, 63);
//	USBx->BUSWAIT = (bwait << USB_BUSWAIT_BWAIT_SHIFT) & USB_BUSWAIT_BWAIT;	// 5 cycles = 75 nS minimum
//	(void) USBx->BUSWAIT;
//
//	USBx->SUSPMODE &= ~ USB_SUSPMODE_SUSPM;	// SUSPM 0: The clock supplied to this module is stopped.
//	(void) USBx->SUSPMODE;
//
//	// This setting shared for USB200 and USB201
//	SYSCFG0_0 = (SYSCFG0_0 & ~ (USB_SYSCFG_UPLLE | USB_SYSCFG_UCKSEL)) |
//		1 * USB_SYSCFG_UPLLE |	// UPLLE 1: Enables operation of the internal PLL.
//		USB_SYSCFG_UCKSEL_Value |	// UCKSEL 1: The 12-MHz EXTAL clock is selected.
//		0;
//	(void) SYSCFG0_0;
//	HARDWARE_DELAY_MS(2);	// required 1 ms delay - see R01UH0437EJ0200 Rev.2.00 28.4.1 System Control and Oscillation Control
//
//	USBx->SUSPMODE |= USB_SUSPMODE_SUSPM;	// SUSPM 1: The clock supplied to this module is enabled.
//	(void) USBx->SUSPMODE;

	return HAL_OK;
}

/**
  * @brief  Initializes the PCD according to the specified
  *         parameters in the PCD_InitTypeDef and create the associated handle.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd)
{
	  USBOTG_TypeDef *USBx;
	  uint8_t i;

	  /* Check the PCD handle allocation */
	  if (hpcd == NULL)
	  {
	    return HAL_ERROR;
	  }

		musb2_prepare(& hpcd->awxx_usb);

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
	PRINTF("HAL_PCD_DeInit\n");
//	  /* Check the PCD handle allocation */
//	  if (hpcd == NULL)
//	  {
//	    return HAL_ERROR;
//	  }
//
//	  hpcd->State = HAL_PCD_STATE_BUSY;
//
//	  /* Stop Device */
//	  if (USB_StopDevice(hpcd->Instance) != HAL_OK)
//	  {
//	    return HAL_ERROR;
//	  }
//
//	#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
//	  if (hpcd->MspDeInitCallback == NULL)
//	  {
//	    hpcd->MspDeInitCallback = HAL_PCD_MspDeInit; /* Legacy weak MspDeInit  */
//	  }
//
//	  /* DeInit the low level hardware */
//	  hpcd->MspDeInitCallback(hpcd);
//	#else
//	  /* DeInit the low level hardware: CLOCK, NVIC.*/
//	  HAL_PCD_MspDeInit(hpcd);
//	#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
//
//	  hpcd->State = HAL_PCD_STATE_RESET;

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
//  __HAL_LOCK(hpcd);

	usb_struct_init(& hpcd->awxx_usb);

	usb_init(& hpcd->awxx_usb);

	USB_DevConnect (hpcd->Instance);
//  __HAL_PCD_ENABLE(hpcd);
//  __HAL_UNLOCK(hpcd);
  return HAL_OK;
}

/**
  * @brief  Stop The USB OTG Device.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd)
{
//  __HAL_LOCK(hpcd);
//  __HAL_PCD_DISABLE(hpcd);
  USB_StopDevice(hpcd->Instance);
  USB_DevDisconnect (hpcd->Instance);
	//musb2_stop(& hpcd->awxx_usb);
//  __HAL_UNLOCK(hpcd);
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
//  PCD_EPTypeDef *ep;
//
//  if ((ep_addr & 0x80U) == 0x80U)
//  {
//    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
//    ep->is_in = 1U;
//  }
//  else
//  {
//    ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
//    ep->is_in = 0U;
//  }
//
//  ep->num = ep_addr & EP_ADDR_MSK;
//  ep->maxpacket = ep_mps;
//  ep->type = ep_type;
//
////  if (ep->is_in != 0U)
////  {
////    /* Assign a Tx FIFO */
////    ep->tx_fifo_num = ep->num;
////  }
//  /* Set initial data PID. */
//  if (ep_type == EP_TYPE_BULK)
//  {
//    ep->data_pid_start = 0U;
//  }
//
//  __HAL_LOCK(hpcd);
//  (void)USB_ActivateEndpoint(hpcd->Instance, ep);
//  __HAL_UNLOCK(hpcd);

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
//  PCD_EPTypeDef *ep;
//
//  if ((ep_addr & 0x80U) == 0x80U)
//  {
//    ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
//    ep->is_in = 1U;
//  }
//  else
//  {
//    ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
//    ep->is_in = 0U;
//  }
//  ep->num   = ep_addr & EP_ADDR_MSK;
//
//  __HAL_LOCK(hpcd);
//  (void)USB_DeactivateEndpoint(hpcd->Instance, ep);
//  __HAL_UNLOCK(hpcd);
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
//  PCD_EPTypeDef *ep;
//
//  ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];
//
//  /*setup and start the Xfer */
//  ep->xfer_buff = pBuf;
//  ep->xfer_len = len;
//  ep->xfer_count = 0U;
//  ep->is_in = 0U;
//  ep->num = ep_addr & EP_ADDR_MSK;
//
//  if (hpcd->Init.dma_enable == 1U)
//  {
//    ep->dma_addr = (uint32_t)pBuf;
//  }
//
//  if ((ep_addr & EP_ADDR_MSK) == 0U)
//  {
//#if WITHNEWUSBHAL
//	  USBPhyHw_ep0_read(hpcd, pBuf, len);
//#else /* WITHNEWUSBHAL */
//    (void)USB_EP0StartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
//#endif /* WITHNEWUSBHAL */
//  }
//  else
//  {
//#if WITHNEWUSBHAL
//	  USBPhyHw_endpoint_read(hpcd, ep_addr, pBuf, len);
//#else /* WITHNEWUSBHAL */
//    (void)USB_EPStartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
//#endif /* WITHNEWUSBHAL */
//  }

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
//  PCD_EPTypeDef *ep;
//
//  ep = &hpcd->IN_ep[ep_addr & EP_ADDR_MSK];
//
//  /*setup and start the Xfer */
//  ep->xfer_buff = (uint8_t *) pBuf;
//  ep->xfer_len = len;
//  ep->xfer_count = 0U;
//  ep->is_in = 1U;
//  ep->num = ep_addr & EP_ADDR_MSK;
//
//  if (hpcd->Init.dma_enable == 1U)
//  {
//    ep->dma_addr = (uint32_t)pBuf;
//  }
//
//  if ((ep_addr & EP_ADDR_MSK) == 0U)
//  {
//#if WITHNEWUSBHAL
//	USBPhyHw_ep0_write(hpcd, (uint8_t *) pBuf, len);
//#else /* WITHNEWUSBHAL */
//    (void)USB_EP0StartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
//#endif /* WITHNEWUSBHAL */
//  }
//  else
//  {
//#if WITHNEWUSBHAL
//	  USBPhyHw_endpoint_write(hpcd, ep_addr, (uint8_t *) pBuf, len);
//#else /* WITHNEWUSBHAL */
//    (void)USB_EPStartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
//#endif /* WITHNEWUSBHAL */
//  }

  return HAL_OK;
}

uint32_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
	return hpcd->OUT_ep [ep_addr & EP_ADDR_MSK].xfer_count;
}

#endif /* CPUSTYPE_T113 */
