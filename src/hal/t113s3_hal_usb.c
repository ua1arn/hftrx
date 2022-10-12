/* $Id$ */
// allwnr_hal_usb.c
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//


#include "hardware.h"

#if (CPUSTYPE_T113 || CPUSTYPE_F133)

#include "board.h"
#include "audio.h"
#include "formats.h"

#include "src/usb/usb200.h"
#include "src/usb/usbch9.h"

#include "usb_device.h"
#include "usbd_core.h"
#include "usbh_core.h"
#include "usbh_def.h"

#include "t113s3_hal.h"
#include "t113s3_hal_usb.h"

#define DISK_DDR 1 /* ������� USB: 0 - SD-�����, 1 - DDR-������ */

#define AW_RAMDISK_BASE                (getRamDiskBase()) //(AW_USBD_BASE+AW_USBD_SIZE)             /* ������� ����� ������ ����� � ������ */

#define AW_RAMDISK_SIZE                (getRamDiskSize()) //(CARD.capacity) /* (32*0x100000) */       /* ������ ����� � ������ */

#ifndef min
#define min( x, y ) ( (x) < (y) ? (x) : (y) )
#endif

#define USB0_ROLE  				USB_ROLE_DEV

#if WITHUSBDEV_HSDESC
	#define USB0_SPEED 				USB_SPEED_HS
#else /* WITHUSBDEV_HSDESC */
	#define USB0_SPEED 				USB_SPEED_FS
#endif /* WITHUSBDEV_HSDESC */

#define BULK_IN_EP    			0x1
#define BULK_OUT_EP				0x2

#define USB_DEV_SEC_BITS   		9//
#define USB_DEV_SEC_SIZE   		(0x1u << USB_DEV_SEC_BITS)//


#define USB_EP_FIFO_SIZE		512
#define USB_FIFO_ADDR_BLOCK		64//(USB_EP_FIFO_SIZE<<1)

#define USB_MAX_LUN    			0//

#define MAX_DDMA_SIZE			(16*1024*1024)  //16MB

#define USB_DEV0_TOTAL_CAP  	1//MB
#define USB_DEV0_SEC_BITS   	9//
#define USB_DEV0_SEC_SIZE   	(0x1u << USB_DEV0_SEC_BITS)//
#define USB_DEV0_SEC_CNT    	(((USB_DEV0_TOTAL_CAP-1)<<11)|0xFFF)//

//#define USB_BO_DEV0_MEM_BASE	0x80800000//

//#define USB_BO_DEV0_BUF_BASE	(USB_BO_DEV0_MEM_BASE + (USB_DEV0_TOTAL_CAP<<20))//
#define USB_BO_DEV_BUF_SIZE	    ( 64 * 1024)//

//VBUS Level
#define USB_VBUS_SESSEND		0
#define USB_VUBS_SESSVLD		1
#define USB_VBUS_ABVALID		2
#define USB_VBUS_VBUSVLD		3


////////////////////////////////////////////////////////////////////////////
//enum USB_DEVICE_CLASS {UDC_MassStorage=0x08, UDC_HID=0x03, UDC_HUB=0x09};

#define USB_DEV_DESC_LEN		18
#define USB_CONFIG_DESC_LEN		9
#define USB_INTF_DESC_LEN		9
#define USB_ENDP_DESC_LEN		7

#define USB_CBW_LEN				(31)
#define USB_CBW_SIG				(0x43425355)

#define USB_CSW_LEN				(13)
#define USB_CSW_SIG				(0x53425355)



//////////////////////////////////////////////////////////////////////////
#define   USB2DRAM_PARAMS    	0x0f000f0f
#define   DRAM2USB_PARAMS    	0x0f0f0f00



#define get_bvalue(n)    		(*((volatile uint8_t *)(n)))          /* byte input */
#define put_bvalue(n,c)  		do { (*((volatile uint8_t *)(n)) = (c)); } while (0)    /* byte output */

#define  wBoot_part_start(part)      0
#define  wBoot_dma_QueryState(hdma)  0
#define  wBoot_dma_stop(hdma)        do { } while (0)
#define  wBoot_part_capacity(sect)   (AW_RAMDISK_SIZE/512)                     /* ����� �������� ������������ �����, ������ = 512 ���� */
#define  wBoot_part_count(sect)      1
#define  wBoot_dma_request(sect)     0

//#define  device_bo_memory_base       AW_USBD_BASE
//#define  device_bo_bufbase           (AW_USBD_BASE+0x20000)


//uint64_t USBWriteNum=0; //������� ����� ���������� �������� �� ������� USB

static int wBoot_block_read(unsigned int start,unsigned int nblock,void *pBuffer)
{
 #if DISK_DDR
  memcpy((unsigned long*)pBuffer,(unsigned long*)(AW_RAMDISK_BASE+(start*512)),nblock*512);
 #else
//  TryRead:
  if(mmc_read_blocks(pBuffer,start,nblock)!=nblock)
  {
   UART0_puts("SD read error from PC side...\n");
//   sdcard_init();
//   udelay(1);
//   goto TryRead;
  }
 #endif
  return 0;
}

static int wBoot_block_write(unsigned int start,unsigned int nblock,void *pBuffer)
{
 #if DISK_DDR
  memcpy((unsigned long*)(AW_RAMDISK_BASE+(start*512)),(unsigned long *)pBuffer,nblock*512);
 #else
//  TryWrite:
  if(mmc_write_blocks(pBuffer,start,nblock)!=nblock)
  {
   UART0_puts("SD write error from PC side...\n");
//   sdcard_init();
//   udelay(1);
//   goto TryWrite;
  }

  USBWriteNum+=nblock;

 #endif
  return 0;
}


//////////////////////////////////////////////////////////////////

static void usb_set_dev_addr(pusb_struct pusb, uint32_t addr)
{
	USBOTG0->FIFO [0].USB_TXFADDR = addr & 0x7F;
}

static void usb_set_ep0_addr(pusb_struct pusb, uint32_t fifo, uint32_t addr)
{
	USBOTG0->FIFO [fifo].USB_TXFADDR = addr & 0x7F;
}

static void usb_iso_update_enable(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val |= (0x1u << 7);	// MUSB2_MASK_ISOUPD
	USBOTG0->USB_POWER = reg_val;
}

static void usb_iso_update_disable(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val &= ~ (0x1u << 7);	// MUSB2_MASK_ISOUPD
	USBOTG0->USB_POWER = reg_val;
}

static void usb_soft_connect(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val |= (0x1u << 6);	// MUSB2_MASK_SOFTC
	USBOTG0->USB_POWER = reg_val;
}

static void usb_soft_disconnect(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val &= ~ (0x1u << 6);	// MUSB2_MASK_SOFTC
	USBOTG0->USB_POWER = reg_val;
}

static void usb_high_speed_enable(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val |= (0x1u << 5);
	USBOTG0->USB_POWER = reg_val;
}

static void usb_high_speed_disable(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val &= ~ (0x1u << 5);
	USBOTG0->USB_POWER = reg_val;
}

static uint32_t usb_is_high_speed(pusb_struct pusb)
{
	return (USBOTG0->USB_POWER >> 4) & 0x1;
}

static void usb_set_reset(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val |= (0x1u << 3);
	USBOTG0->USB_POWER = reg_val;
}

static void usb_clear_reset(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val &= ~ (0x1u << 3);
	USBOTG0->USB_POWER = reg_val;
}

static void usb_set_resume(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val |= (0x1u << 2);
	USBOTG0->USB_POWER = reg_val;
}

static void usb_clear_resume(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val &= ~ (0x1u << 2);
	USBOTG0->USB_POWER = reg_val;
}


static void usb_set_suspend(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val |= (0x1u << 1);
	USBOTG0->USB_POWER = reg_val;
}

static void usb_clear_suspend(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val &= ~ (0x1u << 1);
	USBOTG0->USB_POWER = reg_val;
}


static uint32_t usb_check_suspend(pusb_struct pusb)
{
	return (USBOTG0->USB_POWER >> 1) & 0x01;
}

static void usb_suspendm_enable(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val |= (0x1u << 0);
	USBOTG0->USB_POWER = reg_val;
}

static void usb_suspendm_disable(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_POWER;
	reg_val &= ~ (0x1u << 0);
	USBOTG0->USB_POWER = reg_val;
}

static uint32_t usb_get_ep0_interrupt_status(pusb_struct pusb)
{
	return USBOTG0->USB_INTTX & 0x1;
}

static void usb_clear_ep0_interrupt_status(pusb_struct pusb)
{
	USBOTG0->USB_INTTX = 0x1;
}

static uint32_t usb_get_eptx_interrupt_status(pusb_struct pusb)
{
	return USBOTG0->USB_INTTX & 0xFFFF;
}

static void usb_clear_eptx_interrupt_status(pusb_struct pusb, uint32_t bm)
{
	USBOTG0->USB_INTTX = bm & 0xFFFF;
}

static uint32_t usb_get_eprx_interrupt_status(pusb_struct pusb)
{
	return USBOTG0->USB_INTRX & 0xFFFF;
}

static void usb_clear_eprx_interrupt_status(pusb_struct pusb, uint32_t bm)
{
	USBOTG0->USB_INTRX = bm & 0xFFFF;
}

static void usb_set_eptx_interrupt_enable(pusb_struct pusb, uint32_t bm)
{
	uint16_t reg_val;

	reg_val = USBOTG0->USB_INTTXE;
	reg_val |= (bm & 0xFFFF);
	USBOTG0->USB_INTTXE = reg_val;
}

static void usb_clear_eptx_interrupt_enable(pusb_struct pusb, uint32_t bm)
{
	uint16_t reg_val;

	reg_val = USBOTG0->USB_INTTXE;
	reg_val &= ~ (bm & 0xFFFF);
	USBOTG0->USB_INTTXE = reg_val;
}

static void usb_set_eprx_interrupt_enable(pusb_struct pusb, uint32_t bm)
{
	uint16_t reg_val;

	reg_val = USBOTG0->USB_INTRXE;
	reg_val |= (bm & 0xFFFF);
	USBOTG0->USB_INTRXE = reg_val;
}

static void usb_clear_eprx_interrupt_enable(pusb_struct pusb, uint32_t bm)
{
	uint16_t reg_val;

	reg_val = USBOTG0->USB_INTRXE;
	reg_val &= ~ (bm & 0xFFFF);
	USBOTG0->USB_INTRXE = reg_val;
}

static uint32_t usb_get_bus_interrupt_status(pusb_struct pusb)
{
	return USBOTG0->USB_INTUSB & 0xFF;
}

static void usb_clear_bus_interrupt_status(pusb_struct pusb, uint32_t bm)
{
	USBOTG0->USB_INTUSB = bm & 0xFF;
	//put_bvalue(USBOTG0_BASE + USB_bINTRUSB_OFF, bm & 0xFF);
}

static uint32_t usb_get_bus_interrupt_enable(pusb_struct pusb)
{
	return USBOTG0->USB_INTUSBE & 0xFF;
	//return USBOTG0->USB_INTUSBE & 0xFF;
}

static void usb_set_bus_interrupt_enable(pusb_struct pusb, uint32_t bm)
{
	uint16_t reg_val;

	reg_val = USBOTG0->USB_INTUSBE;
	reg_val |= (bm & 0xFF);
	USBOTG0->USB_INTUSBE = reg_val;
}

static void usb_clear_bus_interrupt_enable(pusb_struct pusb, uint32_t bm)
{
	uint16_t reg_val;

	reg_val = USBOTG0->USB_INTUSBE;
	reg_val &= ~ (bm & 0xFF);
	USBOTG0->USB_INTUSBE = reg_val;
}

static uint32_t usb_get_frame_number(pusb_struct pusb)
{
	return USBOTG0->USB_FRAME & 0x7FF;
	//return get_hvalue(USBOTG0_BASE + USB_hFRAME_OFF) & 0x7FF;
}

static void usb_select_ep(pusb_struct pusb, uint32_t ep_no)
{
	if(ep_no > USB_MAX_EP_NO)
		return;
	USBOTG0->USB_EPINDEX = ep_no;
	//put_bvalue(USBOTG0_BASE + USB_bINDEX_OFF, ep_no);
}

static uint32_t usb_get_active_ep(pusb_struct pusb)
{
	return USBOTG0->USB_EPINDEX & 0x0F;
	//return get_bvalue(USBOTG0_BASE + USB_bINDEX_OFF) & 0xf;
}

static void usb_set_test_mode(pusb_struct pusb, uint32_t bm)
{
	USBOTG0->USB_TESTMODE = bm;
	//put_bvalue(USBOTG0_BASE + USB_bTESTMODE_OFF, bm & 0xFF);
}

static void usb_set_eptx_maxpkt(pusb_struct pusb, uint32_t maxpayload, uint32_t pktcnt)
{
	uint32_t reg_val;

	reg_val = (maxpayload & 0x7FF);
	reg_val |= ((pktcnt-1) & 0x1F) << 11;
	USBOTG0->USB_TXMAXP = reg_val;
}

static uint32_t usb_get_eptx_maxpkt(pusb_struct pusb)
{
	return USBOTG0->USB_TXMAXP;
}



static void usb_ep0_disable_ping(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val |= (0x1u << 8);
	USBOTG0->USB_CSR0 = reg_val;
}

static void usb_ep0_enable_ping(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val &= ~ (0x1u << 8);
	USBOTG0->USB_CSR0 = reg_val;
}

static void usb_ep0_flush_fifo(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val |= (0x1u << 8);
	USBOTG0->USB_CSR0 = reg_val;
}

static uint32_t usb_ep0_is_naktimeout(pusb_struct pusb)
{
	return (USBOTG0->USB_CSR0 >> 7) & 0x1;
}

static void usb_ep0_clear_naktimeout(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val &= ~ (0x1u << 7);
	USBOTG0->USB_CSR0 = reg_val;
}

static void usb_ep0_set_statuspkt(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val |= (0x1u << 6);
	USBOTG0->USB_CSR0 = reg_val;
}

static void usb_ep0_clear_statuspkt(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val &= ~ (0x1u << 6);
	USBOTG0->USB_CSR0 = reg_val;
}

static void usb_ep0_set_reqpkt(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val |= (0x1u << 5);
	USBOTG0->USB_CSR0 = reg_val;
}

static void usb_ep0_clear_setupend(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val |= (0x1u << 7);
	USBOTG0->USB_CSR0 = reg_val;
}

static void usb_ep0_clear_rxpktrdy(pusb_struct pusb)
{
	uint32_t reg_val;

	reg_val = USBOTG0->USB_CSR0;
	reg_val |= (0x1u << 6);
	USBOTG0->USB_CSR0 = reg_val;
}


static uint32_t usb_get_ep0_csr(pusb_struct pusb)
{
	uint32_t ret;

	ret = USBOTG0->USB_CSR0;
	return ret;
}

/* read out FIFO status */
static void usb_set_ep0_csr(pusb_struct pusb, uint32_t csr)
{
	 USBOTG0->USB_CSR0 = csr;
}

static uint32_t usb_get_eptx_csr(pusb_struct pusb)
{
	return USBOTG0->USB_CSR0;
}

static void usb_set_eptx_csr(pusb_struct pusb, uint32_t csr)
{
	 USBOTG0->USB_CSR0 = csr;
}

static void usb_eptx_flush_fifo(pusb_struct pusb)
{
	 USBOTG0->USB_CSR0 = (0x1u << 3);
}

static void usb_set_eprx_maxpkt(pusb_struct pusb, uint32_t maxpayload, uint32_t pktcnt)
{
	uint32_t reg_val;

	reg_val = maxpayload & 0x7FF;
	reg_val |= ((pktcnt-1) & 0x1f) << 11;
	USBOTG0->USB_RXMAXP = reg_val & 0xFFFF;
}

static uint32_t usb_get_eprx_maxpkt(pusb_struct pusb)
{
	return USBOTG0->USB_RXMAXP & 0xFFFF;
}

static uint32_t usb_get_eprx_csr(pusb_struct pusb)
{
	return USBOTG0->USB_RXCSR;
	//return get_hvalue(USBOTG0_BASE + USB_hRXCSR_OFF);
}

static void usb_set_eprx_csr(pusb_struct pusb, uint32_t csr)
{
	USBOTG0->USB_RXCSR = csr;
	//put_hvalue(USBOTG0_BASE + USB_hRXCSR_OFF, csr);
}

static void usb_set_eprx_csrhi(pusb_struct pusb, uint32_t csrhi)
{
	USBOTG0->USB_RXCSR = (USBOTG0->USB_RXCSR & ~ 0xFF00) | ((csrhi  & 0xFF) << 8);
	//put_bvalue(USBOTG0_BASE + USB_hRXCSR_OFF + 1, csrhi);
}

static void usb_eprx_flush_fifo(pusb_struct pusb)
{
	USBOTG0->USB_RXCSR = 0x1u << 4;
	//put_hvalue(USBOTG0_BASE + USB_hRXCSR_OFF, 0x1u << 4);
}

static uint32_t usb_get_ep0_count(pusb_struct pusb)
{
	return USBOTG0->USB_RXCOUNT & 0x7F;
}

static uint32_t usb_get_eprx_count(pusb_struct pusb)
{
	return USBOTG0->USB_RXCOUNT & 0x1FFF;
}

static void usb_set_ep0_type(pusb_struct pusb, uint32_t speed)
{
	USBOTG0->USB_RXTI = (speed & 0x3) << 6;
	//put_bvalue(USBOTG0_BASE + USB_bTXTYPE_OFF, (speed & 0x3) << 6);	// #define  USB_bRXTYPE_OFF      		(0x8E)
}

static void usb_set_eptx_type(pusb_struct pusb, uint32_t speed, uint32_t protocol, uint32_t ep_no)
{
	uint32_t reg_val;

	reg_val = (speed & 0x3) << 6;
	reg_val |= (protocol & 0x3) << 4;
	reg_val |= (ep_no & 0xf) << 0;
	USBOTG0->USB_RXTI = reg_val;
	//put_bvalue(USBOTG0_BASE + USB_bTXTYPE_OFF, reg_val);
}

static void usb_set_ep0_naklimit(pusb_struct pusb, uint32_t naklimit)
{
	USBOTG0->USB_RXNAKLIMIT = naklimit & 0x1f;
	//put_bvalue(USBOTG0_BASE + USB_bTXINTERVAL_OFF, naklimit & 0x1f);	// #define  USB_bRXINTERVAL_OFF  		(0x8F)
}

static void usb_set_eptx_interval(pusb_struct pusb, uint32_t interval)
{
	USBOTG0->USB_RXNAKLIMIT = interval & 0xFF;
	//put_bvalue(USBOTG0_BASE + USB_bTXINTERVAL_OFF, interval & 0xFF);
}

static void usb_set_eprx_type(pusb_struct pusb, uint32_t speed, uint32_t protocol, uint32_t ep_no)
{
	uint32_t reg_val;

	reg_val = (speed & 0x3) << 6;
	reg_val |= (protocol & 0x3) << 4;
	reg_val |= (ep_no & 0xf) << 0;
	USBOTG0->USB_RXTI = reg_val;
	//put_bvalue(USBOTG0_BASE + USB_bRXTYPE_OFF, reg_val);	// USB_bRXTYPE_OFF      		(0x8E)
}

static void usb_set_eprx_interval(pusb_struct pusb, uint32_t interval)
{
	USBOTG0->USB_RXNAKLIMIT = interval & 0xFF;
	//put_bvalue(USBOTG0_BASE + USB_bRXINTERVAL_OFF, interval & 0xFF);
}
//
//static uint32_t usb_get_core_config(pusb_struct pusb)
//{
//	return USBOTG0->USB_CORECONFIG;
//	//return get_bvalue(USBOTG0_BASE + USB_bCORECONFIG_OFF);	// #define  USB_bCORECONFIG_OFF		(0xC0)
//}

static uint32_t usb_is_b_device(pusb_struct pusb)
{
	return (USBOTG0->USB_DEVCTL >> 7) & 0x1;
}

static uint32_t usb_device_connected_is_fs(pusb_struct pusb)
{
	return (USBOTG0->USB_DEVCTL >> 6) & 0x1;
}

static uint32_t usb_device_connected_is_ls(pusb_struct pusb)
{
	return (USBOTG0->USB_DEVCTL >> 5) & 0x1;
}

// USB_VBUS_VBUSVLD
static uint32_t usb_get_vbus_level(pusb_struct pusb)
{
	return (USBOTG0->USB_DEVCTL >> 3) & 0x3;
}

static uint32_t usb_is_host(pusb_struct pusb)
{
	return (USBOTG0->USB_DEVCTL >> 2) & 0x1;
}

static void usb_set_hnp_request(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = USBOTG0->USB_DEVCTL;
	reg_val |= 0x1u << 1;
	USBOTG0->USB_DEVCTL = reg_val;
}

static uint32_t usb_hnp_in_porcess(pusb_struct pusb)
{
	return (USBOTG0->USB_DEVCTL >> 1) & 0x1;
}

static void usb_start_session(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = 0x1;
	USBOTG0->USB_DEVCTL = reg_val;
}

static void usb_end_session(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = 0x0;
	USBOTG0->USB_DEVCTL = reg_val;
}

static uint32_t usb_check_session(pusb_struct pusb)
{
	return USBOTG0->USB_DEVCTL & 0x1;
}

static uint32_t aw_log2(uint32_t x)
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

static void usb_set_eptx_fifo_size(pusb_struct pusb, uint32_t is_dpb, uint32_t size)
{
	uint8_t reg_val;

	reg_val = 0;
	if(is_dpb) reg_val |= 0x1u << 4;
	reg_val |= aw_log2(size >> 3) & 0xf;
	USBOTG0->USB_TXFIFOSZ = reg_val;
}

static void usb_set_eprx_fifo_size(pusb_struct pusb, uint32_t is_dpb, uint32_t size)
{
	uint8_t reg_val;

	reg_val = 0;
	if(is_dpb) reg_val |= 0x1u << 4;
	reg_val |= aw_log2(size >> 3) & 0xf;
	USBOTG0->USB_RXFIFOSZ = reg_val;
}

static void usb_set_eptx_fifo_addr(pusb_struct pusb, uint32_t addr)
{
	USBOTG0->USB_TXFIFOADD = (addr >> 3) & 0x1FFF;
}

static void usb_set_eprx_fifo_addr(pusb_struct pusb, uint32_t addr)
{
	USBOTG0->USB_RXFIFOADD = (addr >> 3) & 0x1FFF;
}

static void usb_fifo_accessed_by_cpu(pusb_struct pusb)
{
	USBOTG0->USB_DMACTL &= ~ (0x1u << 0);
}

static void usb_fifo_accessed_by_dma(pusb_struct pusb, uint32_t ep_no, uint32_t is_tx)
{
	uint8_t reg_val;

	if (ep_no>USB_MAX_EP_NO)
		return;
	reg_val = 0x1;
	if(!is_tx) reg_val |= 0x1u << 1;
	reg_val |= (ep_no-1) << 2;
	USBOTG0->USB_DMACTL = reg_val;
}

static uint8_t usb_get_fifo_access_config(pusb_struct pusb)
{
	return USBOTG0->USB_DMACTL;
}

static uint32_t usb_get_dma_ep_no(pusb_struct pusb)
{
	return ((USBOTG0->USB_DMACTL & 0x1f) >> 2)+1;
}

static void usb_set_fifo_access_config(pusb_struct pusb, uint8_t config)
{
	USBOTG0->USB_DMACTL = config;
}

static uint32_t usb_get_fsm(pusb_struct pusb)
{
	return USBOTG0->USB_FSM;
	//return get_bvalue(USBOTG0_BASE + USB_FSM_OFF);
}

static void usb_set_eptx_faddr(pusb_struct pusb, uint32_t fifo, uint32_t faddr)
{
	USBOTG0->FIFO [fifo].USB_TXFADDR = faddr & 0x7F;
	//(USBOTG0->USB_TXFADDR = faddr & 0x7F);
}

static void usb_set_eptx_haddr(pusb_struct pusb, uint32_t fifo, uint32_t haddr, uint32_t is_mtt)
{
	uint8_t reg_val;

	reg_val = 0;
	if (is_mtt)
		reg_val |= 0x1u << 7;
	reg_val |= haddr & 0x7F;

	USBOTG0->FIFO [fifo].USB_TXHADDR = reg_val;
	//put_bvalue(USBOTG0_BASE + USB_bTXHADDR_OFF, reg_val);
}

static void usb_set_eptx_hport(pusb_struct pusb, uint32_t fifo, uint32_t hport)
{
	USBOTG0->FIFO [fifo].USB_TXHUBPORT = hport & 0x7F;
	//put_bvalue(USBOTG0_BASE + USB_bTXHPORT_OFF, hport & 0x7F);
}

static void usb_set_eprx_faddr(pusb_struct pusb, uint32_t fifo, uint32_t faddr)
{
	USBOTG0->FIFO [fifo].USB_RXFADDR = faddr & 0x7F;
	//put_bvalue(USBOTG0_BASE + USB_bRXFADDR_OFF, faddr & 0x7F);
}

static void usb_set_eprx_haddr(pusb_struct pusb, uint32_t fifo, uint32_t haddr, uint32_t is_mtt)
{
	uint8_t reg_val;

	reg_val = 0;
	if (is_mtt)
		reg_val |= 0x1u << 7;
	reg_val |= haddr & 0x7F;

	USBOTG0->FIFO [fifo].USB_RXHADDR = reg_val;
	//put_bvalue(USBOTG0_BASE + USB_bRXHADDR_OFF, reg_val);
}

static void usb_set_eprx_hport(pusb_struct pusb, uint32_t fifo, uint32_t hport)
{
	USBOTG0->FIFO [fifo].USB_RXHUBPORT = hport & 0x7F;
	//put_bvalue(USBOTG0_BASE + USB_bRXHPORT_OFF, hport & 0x7F);
}


static void usb_set_reqpkt_count(pusb_struct pusb, uint32_t count)
{
	USBOTG0->USB_RXPKTCNT = count;
}

static uint32_t usb_get_reqpkt_count(pusb_struct pusb)
{
	 return USBOTG0->USB_RXPKTCNT;
}

static uint32_t usb_get_lsvbusvld(pusb_struct pusb)
{
	return (USBOTG0->USB_ISCR >> 30) & 0x1;
}

static uint32_t usb_get_extvbusvld(pusb_struct pusb)
{
	return (USBOTG0->USB_ISCR >> 29) & 0x1;
}

static uint32_t usb_get_extiddig(pusb_struct pusb)
{
	return (USBOTG0->USB_ISCR >> 28) & 0x1;
}

static uint32_t usb_get_linestate(pusb_struct pusb)
{
	return (USBOTG0->USB_ISCR >> 26) & 0x3;
}

static uint32_t usb_get_vbusvalid(pusb_struct pusb)
{
	return (USBOTG0->USB_ISCR >> 25) & 0x1;
}

static uint32_t usb_get_iddig(pusb_struct pusb)
{
#ifdef CONFIG_AW_FPGA_PLATFORM
	 return (USBOTG0->USB_DEVCTL >> 0x7) & 0x1;
#else
	 return (USBOTG0->USB_ISCR >> 24) & 0x1;
#endif
}

static uint32_t usb_get_fs_linestate(pusb_struct pusb)
{
	return ((USBOTG0->USB_ISCR) >> 20) & 0x03;
}

static void usb_iddig_pullup_enable(pusb_struct pusb)
{
	USBOTG0->USB_ISCR |= (0x1u << 17);
}

static void usb_iddig_pullup_disable(pusb_struct pusb)
{
	USBOTG0->USB_ISCR &= ~ (0x1u << 17);
}

static void usb_dpdm_pullup_enable(pusb_struct pusb)
{
	USBOTG0->USB_ISCR |= (0x1u << 16);
}

static void usb_dpdm_pullup_disable(pusb_struct pusb)
{
	USBOTG0->USB_ISCR &= ~ (0x1u << 16);
}

static void usb_release_id(pusb_struct pusb)
{
	USBOTG0->USB_ISCR &= ~ (0x3 << 14);
}

static void usb_force_id(pusb_struct pusb, uint32_t id)
{

	if(id) 	USBOTG0->USB_ISCR |= (0x1u << 14);
	else 	USBOTG0->USB_ISCR &= ~ (0x1u << 14);
	USBOTG0->USB_ISCR |= (0x1u << 15);

}

/* src = 0..3 */
static void usb_vbus_src(pusb_struct pusb, uint32_t src)
{
	USBOTG0->USB_ISCR &= ~ (0x3 << 10);
	USBOTG0->USB_ISCR |= ((src & 0x03) << 10);
}

static void usb_release_vbus(pusb_struct pusb)
{
	USBOTG0->USB_ISCR &= ~ (0x3 << 12);
}

static void usb_force_vbus(pusb_struct pusb, uint32_t vbus)
{
	if(vbus) 	USBOTG0->USB_ISCR |= (0x1u << 12);
	else 		USBOTG0->USB_ISCR &= ~ (0x1u << 12);
	USBOTG0->USB_ISCR |= (0x1u << 13);
}

static void usb_drive_vbus(pusb_struct pusb, uint32_t vbus, uint32_t index)
{
uint32_t temp;
	if(index == 0)
	{
//	//Set PB9 Output,USB0-DRV SET
//	 temp = get_wvalue(0x01c20800+0x28);
//	 temp &= ~ (0x7 << 4);
//	 temp |= (0x1u << 1);
//	 put_wvalue(0x01c20800+0x28, temp);
//
//	 if(vbus)
//	 {
//	 	temp = get_wvalue(0x01c20800+0x34);
//	 	temp |= (0x1u << 9);
//	 	put_wvalue(0x01c20800+0x34,temp);
//	 }
//	 else
//	 {
//	 temp = get_wvalue(0x01c20800+0x34);
//	 	temp &= ~ (0x1u << 9);
//	 	put_wvalue(0x01c20800+0x34,temp);
//	 }
	}

}

static uintptr_t usb_get_ep_fifo_addr(pusb_struct pusb, uint32_t ep_no)
{
	return (uintptr_t) & USBOTG0->USB_EPFIFO [ep_no];
	//return (USBOTG0_BASE + USB_bFIFO_OFF(ep_no));
}

//static uint32_t usb_dma_write_config(pusb_struct pusb)
//{
//	return (((USB_DMA_CONFIG|(0x1f&pusb->drq_no)) << 16)|DRAM_DMA_CONFIG);
//}
//
//static uint32_t usb_dma_read_config(pusb_struct pusb)
//{
//	return ((DRAM_DMA_CONFIG << 16)|(USB_DMA_CONFIG|(0x1f&pusb->drq_no)));
//}


#define USB_NO_DMA		1

static uint32_t write_len = 0;
static uint32_t write_offset = 0;
static int32_t part_index = 0;


static int32_t dram_copy(uint32_t src_addr, uint32_t dest_addr, uint32_t bytes)
{
	memcpy((void *)src_addr, (void *)dest_addr, bytes);

	return 1;
}



static const unsigned char USB_HS_BULK_DevDesc[18]    = {0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40,
	                                        0x1A, 0x1F,   //VID -- Vendor ID
	                                        0x00, 0x07,   //PID -- Product ID
	                                        0x00, 0x01, 0x02, 0x03, 0x01, 0x01};

static const unsigned char USB_HS_BULK_ConfigDesc[32] = {//Configuration Descriptor
	                                        0x09, 0x02, 0x20, 0x00, 0x01,
	                                        0x08,  //Value for SetConfig Request
	                                        0x00, 0x80,
	                                        50,  //Max Power = 2*n mA
	                                        //Interface Descriptor
	                                        0x09, 0x04, 0x00, 0x00, 0x02,
	                                        0x08,  //Interface Class    -- 0x08, MassStorage
	                                        0x06,  //Interface SubClass -- 0x06, SPC-2
	                                        0x50,  //Interface Protocol -- Bulk Only
	                                        0x00,
	                                        //Bulk-only IN Endpoint Descriptor
	                                        0x07, 0x05, (0x80|BULK_IN_EP), 0x02,
	                                        0x00, 0x02,
	                                        0x00,
	                                        //Bulk-only OUT Endpoint Descriptor
	                                        0x07, 0x05, BULK_OUT_EP, 0x02,
	                                        0x00, 0x02,
	                                        0x00
	                                        };

static const unsigned char USB_FS_BULK_DevDesc[18]    = {0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
	                                        0x1A, 0x1F,   //VID -- Vendor ID
	                                        0x00, 0x07,   //PID -- Product ID
	                                        0x00, 0x01, 0x02, 0x03, 0x01, 0x01};

static const unsigned char USB_FS_BULK_ConfigDesc[32] = {//Configuration Descriptor
	                                        0x09, 0x02, 0x20, 0x00, 0x01,
	                                        0x08,  //Value for SetConfig Request
	                                        0x00, 0x80,
	                                        50,  //Max Power = 2*n mA
	                                        //Interface Descriptor
	                                        0x09, 0x04, 0x00, 0x00, 0x02,
	                                        0x08,  //Interface Class    -- 0x08, MassStorage
	                                        0x06,  //Interface SubClass -- 0x06, SPC-2
	                                        0x50,  //Interface Protocol -- Bulk Only
	                                        0x00,
	                                        //Bulk-only IN Endpoint Descriptor
	                                        0x07, 0x05, (0x80|BULK_IN_EP), 0x02,
	                                        0x40, 0x00,
	                                        0x00,
	                                        //Bulk-only OUT Endpoint Descriptor
	                                        0x07, 0x05, BULK_OUT_EP, 0x02,
	                                        0x40, 0x00,
	                                        0x00
	                                        };



static const unsigned char  LangID[4]        = {0x04, 0x03, 0x09, 0x04};
static const unsigned char  iSerialNum0[30]   = {0x1E, 0x03, '2' , 0x00, '0' , 0x00, '1' , 0x00, '0' , 0x00,
	                                       '1' , 0x00, '2' , 0x00, '0' , 0x00, '1' , 0x00, '1' , 0x00,
	                                       '2' , 0x00, '0' , 0x00, '0' , 0x00, '0' , 0x00, '1' , 0x00
	                                       }; //"20101201120001"
static const unsigned char  iSerialNum1[30]   = {0x1E, 0x03, '2' , 0x00, '0' , 0x00, '1' , 0x00, '0' , 0x00,
	                                       '1' , 0x00, '2' , 0x00, '0' , 0x00, '1' , 0x00, '1' , 0x00,
	                                       '2' , 0x00, '0' , 0x00, '0' , 0x00, '0' , 0x00, '2' , 0x00
	                                       }; //"20101201120002"
static const unsigned char  iSerialNum2[30]   = {0x1E, 0x03, '2' , 0x00, '0' , 0x00, '1' , 0x00, '0' , 0x00,
	                                       '1' , 0x00, '2' , 0x00, '0' , 0x00, '1' , 0x00, '1' , 0x00,
	                                       '2' , 0x00, '0' , 0x00, '0' , 0x00, '0' , 0x00, '3' , 0x00
	                                       }; //"20101201120003"
static const unsigned char  iManufacturer[42]=  {0x28,  0x03, 'A' , 0x00, 'l' , 0x00, 'l' , 0x00, 'W' , 0x00,
	                                       'i',  0x00, 'n' , 0x00, 'n' , 0x00, 'e' , 0x00, 'r' , 0x00,
	                                       ' ',  0x00, 'T' , 0x00, 'e' , 0x00, 'c' , 0x00, 'h' , 0x00,
	                                       'n' , 0x00, 'o' , 0x00, 'l' , 0x00, 'o' , 0x00, 'g' , 0x00,
	                                       'y' , 0x00};  //AllWinner Technology
#if 0
static const unsigned char  iProduct[22]     = {0x16, 0x03, 'U' , 0x00, 'S' , 0x00, 'B' , 0x00, ' ' , 0x00,
	                                       'T' , 0x00, 'e' , 0x00, 's' , 0x00, 't' , 0x00, 'e' , 0x00,
	                                       'r' , 0x00};      //"USB Tester"
#endif
static const unsigned char  iProduct_new[90]     = {0x22, 0x03, 'U', 0x00, 'S', 0x00, 'B', 0x00, ' ', 0x00,
	                                          'S', 0x00, 't', 0x00, 'o', 0x00, 'r', 0x00, 'a', 0x00,
	                                          'g', 0x00, 'e', 0x00, ' ', 0x00, 'T', 0x00, 'o', 0x00,
	                                          'o', 0x00, 'l', 0x00};  //USB Storage Tool



static const unsigned char TestPkt[54] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA,
	                                 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEE, 0xEE, 0xEE,
	                                 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
	                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xBF, 0xDF,
	                                 0xEF, 0xF7, 0xFB, 0xFD, 0xFC, 0x7E, 0xBF, 0xDF, 0xEF, 0xF7,
	                                 0xFB, 0xFD, 0x7E, 0x00};


static const unsigned char* USB_StrDec0[4]    = {LangID, iSerialNum0, iManufacturer, iProduct_new};

static const unsigned char USB_DevQual[10]   = {0x0A, 0x06, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00};

static const unsigned char USB_OTGDesc[3] = {0x03, 0x09, 0x03};

///////////////////////////////////////////////////////////
//For MassStorage Only
///////////////////////////////////////////////////////////
static const unsigned char  InquiryData[]  = {  0x00, 0x80, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 'A', 'W',
	                                     'T',  'e' , 'c' , 'h' , ' ',  ' ',  'U' , 'S',  'B', ' ',
	                                     'S',  't',  'o' , 'r' , 'a' , 'g' , 'e' , ' ',  ' ', ' ',
	                                     ' ',  0x20, 0x30, 0x31, 0x30, 0x30};

static const unsigned char SenseData[] = {0x03, 0x00, 0x00, 0x00};





static uint32_t aw_module(uint32_t x, uint32_t y)
{
	uint32_t val;

	if(y==0)
	{
		return 0;
	}
	val = x;
	while(val>=y)
	{
		val -= y;
	}

	return val;
}

static void usb_read_ep_fifo(pusb_struct pusb, uint32_t ep_no, uint32_t dest_addr, uint32_t count)
{
	uint8_t temp;
	uint8_t saved;
	uint32_t dest;
	uint32_t i;

	if(ep_no>USB_MAX_EP_NO)
	{
		return;
	}
	saved = usb_get_fifo_access_config(pusb);
	usb_fifo_accessed_by_cpu(pusb);

	const uintptr_t pipe = usb_get_ep_fifo_addr(pusb, ep_no);
	dest = dest_addr;
	for(i=0; i<count; i++)
	{
		temp = get_bvalue(pipe);
		put_bvalue(dest, temp);
		dest += 1;
	}
	if((count!=31)&&(count!=8))
	{
		//USB_HAL_DBG("rxcount=%d, rxdata=0x%x\n", count, *((uint32_t*)dest_addr));
	}

	usb_set_fifo_access_config(pusb, saved);
}


static void usb_write_ep_fifo(pusb_struct pusb, uint32_t ep_no, uint32_t src_addr, uint32_t count)
{
	uint8_t  temp;
	uint8_t  saved;
	uint32_t src;
	uint32_t i;

	if(ep_no>USB_MAX_EP_NO)
	{
		return;
	}
	saved = usb_get_fifo_access_config(pusb);
	usb_fifo_accessed_by_cpu(pusb);

	src = src_addr;
	const uintptr_t pipe = usb_get_ep_fifo_addr(pusb, ep_no);
	for(i=0; i<count; i++)
	{
		temp = get_bvalue(src);
		put_bvalue(pipe, temp);
		src += 1;
	}

	usb_set_fifo_access_config(pusb, saved);
}
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
	maxpkt = (maxpkt&0x7ff)*(((maxpkt&0xf800)>>11)+1);

	switch(pusb->eprx_xfer_state[ep_no-1])
	{
		case USB_EPX_SETUP:
		{
			pusb->device.epx_buf_tag = 0;
			pusb->device.epx_xfer_addr = dst_addr;
			pusb->device.epx_xfer_residue = byte_count;
			pusb->device.epx_xfer_tranferred = 0;


			if(!maxpkt)
			{
				return USB_RETVAL_COMPOK;
			}

			if(byte_count>=maxpkt)
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

				wBoot_dma_Setting(pusb->dma, (void *)&p);
				//���Ƕ�����������ʱ��ˢ��Ŀ�ĵ�ַ��ԭ�����ˢ��DRAM(SRAM)
				wlibc_CleanFlushDCacheRegion((void *)dram_addr, xfer_count);

				wBoot_dma_start(pusb->dma, usb_get_ep_fifo_addr(pusb, ep_no), dram_addr, xfer_count);

				usb_set_eprx_csrhi(pusb, (USB_RXCSR_AUTOCLR|USB_RXCSR_DMAREQEN)>>8);
				pusb->eprx_xfer_state[ep_no-1] = USB_EPX_DATA;
#else
			    xfer_count = min(pusb->device.epx_xfer_residue, maxpkt);
			    if(usb_get_eprx_csr(pusb)&USB_RXCSR_RXPKTRDY)
				{
					usb_fifo_accessed_by_cpu(pusb);
					if(usb_get_fifo_access_config(pusb)&0x1)
					{
						PRINTF("Error: CPU Access Failed!!\n");
					}
					usb_read_ep_fifo(pusb, ep_no, pusb->device.epx_xfer_addr, xfer_count);
					usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb)&USB_RXCSR_ISO); //Clear RxPktRdy
					pusb->device.epx_xfer_residue -= xfer_count;
					pusb->device.epx_xfer_addr += xfer_count;
					pusb->device.epx_xfer_tranferred  += xfer_count;
					pusb->eprx_xfer_state[ep_no-1] = USB_EPX_DATA;
					epout_timeout = 0;
				}
				else
				{
					epout_timeout ++;

					if(epout_timeout < 0x1000)
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
				if(usb_get_eprx_csr(pusb)&USB_RXCSR_RXPKTRDY)
				{
					usb_fifo_accessed_by_cpu(pusb);
					if(usb_get_fifo_access_config(pusb)&0x1)
					{
						PRINTF("Error: CPU Access Failed!!\n");
					}
					usb_read_ep_fifo(pusb, ep_no, pusb->device.epx_xfer_addr, byte_count);
					usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb)&USB_RXCSR_ISO); //Clear RxPktRdy
					pusb->device.epx_xfer_residue -= byte_count;
					pusb->device.epx_xfer_tranferred  += byte_count;
					pusb->eprx_xfer_state[ep_no-1] = USB_EPX_SETUP;
					ret = USB_RETVAL_COMPOK;
					epout_timeout = 0;
				}
				else
				{
					epout_timeout ++;

					if(epout_timeout < 0x1000)
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
		if(!wBoot_dma_QueryState(pusb->dma))
	 	{
	 		uint32_t data_xfered = pusb->dma_last_transfer;

		  	pusb->device.epx_xfer_residue -= data_xfered;
		  	pusb->dma_last_transfer  = 0;
		  	pusb->device.epx_buf_tag = pusb->device.epx_buf_tag ? 0:1;

		  	if(pusb->device.epx_xfer_residue)
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

			    wBoot_dma_Setting(pusb->dma, (void *)&p);
			    //���Ƕ�����������ʱ��ˢ��Ŀ�ĵ�ַ��ԭ�����ˢ��DRAM(SRAM)
			    wlibc_CleanFlushDCacheRegion((void *)dram_addr, xfer_count);

			    wBoot_dma_start(pusb->dma, usb_get_ep_fifo_addr(pusb, ep_no), dram_addr, xfer_count);
		  	}
		  	else
		  	{
		  		if(byte_count&0x1ff)
			    {
				    usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb)&USB_RXCSR_ISO);
			    }
			    else
			   	{
				   usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb)&(USB_RXCSR_ISO|USB_RXCSR_RXPKTRDY));
			   	}
			    pusb->eprx_xfer_state[ep_no-1] = USB_EPX_SETUP;
			    ret = USB_RETVAL_COMPOK;
		  	}

			if(dram_copy(usb_dev_get_buf_base(pusb, pusb->device.epx_buf_tag? 0:1), pusb->device.epx_xfer_addr, data_xfered))
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
		if(pusb->device.epx_xfer_residue>0)
		{
			if(usb_get_eprx_csr(pusb)&USB_RXCSR_RXPKTRDY)
			{
				uint32_t xfer_count = min(pusb->device.epx_xfer_residue, maxpkt);

				usb_fifo_accessed_by_cpu(pusb);
				if(usb_get_fifo_access_config(pusb)&0x1)
				{
					PRINTF("Error: CPU Access Failed!!\n");
				}
				usb_read_ep_fifo(pusb, ep_no, pusb->device.epx_xfer_addr, xfer_count);
				usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb)&USB_RXCSR_ISO); //Clear RxPktRdy
				pusb->device.epx_xfer_residue -= xfer_count;
				pusb->device.epx_xfer_addr += xfer_count;
				pusb->device.epx_xfer_tranferred  += xfer_count;
				pusb->eprx_xfer_state[ep_no-1] = USB_EPX_DATA;
				epout_timeout = 0;
			}
			else
			{
				epout_timeout ++;

				if(epout_timeout < 0x1000)
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
	maxpkt = (maxpkt&0x7ff)*(((maxpkt&0xf800)>>11)+1);

	switch(pusb->eptx_xfer_state[ep_no-1])
	{
		case USB_EPX_SETUP:
		{
			pusb->device.epx_buf_tag = 0;
			pusb->device.epx_xfer_addr = src_addr;
			pusb->device.epx_xfer_residue = byte_count;
			pusb->device.epx_xfer_tranferred = 0;

			if(!maxpkt)
			{
				return USB_RETVAL_COMPOK;
			}

			if(byte_count>=maxpkt)
		 	{
#ifndef USB_NO_DMA
				uint32_t xfer_count = 0;

		 		xfer_count = min(pusb->device.epx_xfer_residue, USB_BO_DEV_BUF_SIZE);
		 		ping_pang_addr = usb_dev_get_buf_base(pusb, pusb->device.epx_buf_tag);
		 		if(dram_copy(pusb->device.epx_xfer_addr, ping_pang_addr, xfer_count))
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
		 		if(!(usb_get_fifo_access_config(pusb)&0x1))
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

			    wBoot_dma_Setting(pusb->dma, (void *)&p);
		        //����д������д��ʱ��ˢ��Ŀ�ĵ�ַ��ԭ�����ˢ��DRAM(SRAM)
		        wlibc_CleanFlushDCacheRegion((void *)dram_addr, xfer_count);

				wBoot_dma_start(pusb->dma, dram_addr, (uint32_t)ping_pang_addr, xfer_count);

			    pusb->device.epx_buf_tag = pusb->device.epx_buf_tag ? 0:1;

			    if(pusb->device.epx_xfer_residue)
		    	{
		    		xfer_count = min(pusb->device.epx_xfer_residue, USB_BO_DEV_BUF_SIZE);

		    		if(dram_copy(pusb->device.epx_xfer_addr, usb_dev_get_buf_base(pusb, pusb->device.epx_buf_tag), xfer_count))
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
				if(usb_get_fifo_access_config(pusb)&0x1)
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
			if(!wBoot_dma_QueryState(pusb->dma))
		 	{
		 		if(pusb->dma_last_transfer)
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

				    wBoot_dma_Setting(pusb->dma, (void *)&p);
			        //����д������д��ʱ��ˢ��Ŀ�ĵ�ַ��ԭ�����ˢ��DRAM(SRAM)
			        wlibc_CleanFlushDCacheRegion((void *)dram_addr, pusb->dma_last_transfer);

			        wBoot_dma_start(pusb->dma, dram_addr, usb_get_ep_fifo_addr(pusb, ep_no), pusb->dma_last_transfer);

			    	pusb->device.epx_buf_tag = pusb->device.epx_buf_tag ? 0:1;

			    	if(pusb->device.epx_xfer_residue)//Copy Data from storage to buffer
			    	{
			    		uint32_t xfer_count = min(pusb->device.epx_xfer_residue, USB_BO_DEV_BUF_SIZE);

			    		if(dram_copy(pusb->device.epx_xfer_addr, usb_dev_get_buf_base(pusb, pusb->device.epx_buf_tag), xfer_count))
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
			    	maxpkt = (maxpkt&0x7ff)*(((maxpkt&0xf800)>>11)+1);
			    	if(aw_module(byte_count, maxpkt))
			    	{
				   		usb_set_eptx_csr(pusb, usb_get_eptx_csr(pusb)|USB_TXCSR_TXPKTRDY);
			   	 	}
			    	pusb->eptx_xfer_state[ep_no-1] = USB_EPX_END;
	 			}
		 	}
#else
			if(!(usb_get_eptx_csr(pusb)&USB_TXCSR_TXPKTRDY))
			{
				if(pusb->device.epx_xfer_residue > 0)   //Data is not transfered over
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
				else if(!(usb_get_eptx_csr(pusb)&USB_TXCSR_FIFONOTEMP))
				{

					pusb->eptx_xfer_state[ep_no-1] = USB_EPX_END;
				}
			}
#endif
		}
		break;

		case USB_EPX_END:
		if(!(usb_get_eptx_csr(pusb)&0x3))
		{
			usb_set_eptx_csr(pusb, usb_get_eptx_csr(pusb)&USB_TXCSR_ISO);
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
static USB_RETVAL usb_dev_bulk_xfer(pusb_struct pusb)
{
	uint32_t rx_count=0;
	pCBWPKG pCBW;
	pCSWPKG pCSW;
	USB_RETVAL ret = USB_RETVAL_NOTCOMP;
	uint32_t ep_save = usb_get_active_ep(pusb);
	USB_RETVAL fret = USB_RETVAL_NOTCOMP;

	pCBW = (pCBWPKG)(pusb->buffer);
  	pCSW = (pCSWPKG)(pusb->buffer);

	switch(pusb->device.bo_state)
	{
		case USB_BO_IDLE:
		case USB_BO_CBW:
		 	if(!pusb->eprx_flag[pusb->device.bo_ep_out-1])
  		 	{
  				break;
  		 	}

			pusb->eprx_flag[pusb->device.bo_ep_out-1]--;
	  		usb_select_ep(pusb, pusb->device.bo_ep_out);
	  		if(!(usb_get_eprx_csr(pusb)&USB_RXCSR_RXPKTRDY))
	  		{
	  			break;
	  		}
	  		rx_count = usb_get_eprx_count(pusb);
			if(rx_count != USB_CBW_LEN)
	  		{
	  			usb_eprx_flush_fifo(pusb);
	  			PRINTF("Error: Not CBW, RX Data Length=%d\n",rx_count);
	  			break;
	  		}
	  		do
	  		{
	  			ret = epx_out_handler_dev(pusb, pusb->device.bo_ep_out, (uint32_t)pusb->buffer, rx_count, USB_PRTCL_BULK);
	  		}
	  		while(ret == USB_RETVAL_NOTCOMP);

	  		if(ret == USB_RETVAL_COMPERR)
	  		{
	  			PRINTF("Error: RX CBW Error\n");
	  			break;
	  		}
	  		else
	  		{
	  			ret = USB_RETVAL_NOTCOMP;
	  		}
	  		if(pCBW->dCBWSig != USB_CBW_SIG)
	  		{
	  			PRINTF("Error: Not CBW, Error Signature=0x%x\n", pCBW->dCBWSig);
	  			break;
	  		}
#if 0
			{
				uint32_t i;
	            PRINTF("dCBWSig = %x\n", pCBW->dCBWSig);
	            PRINTF("dCBWTag = %x\n", pCBW->dCBWTag);
	            PRINTF("dCBWDTL = %x\n", pCBW->dCBWDTL);
	            PRINTF("bmCBWFlg = %x\n", pCBW->bmCBWFlg);
	            PRINTF("bCBWLUN = %x\n", pCBW->bCBWLUN);
	            PRINTF("bCBWCBL = %x\n", pCBW->bCBWCBL);
	            for(i=0;i<16;i++)
	            {
	            	PRINTF("CBWCB[%d] = %x\n", i, pCBW->CBWCB[i]);
				}
			}
#endif
	  		switch(pCBW->CBWCB[0])
	  		{
	  			case 0x00://Test Unit Ready
					pCSW->dCSWSig = USB_CSW_SIG;
					pCSW->dCSWDataRes = 0;
					pCSW->bCSWStatus = 0;
					pusb->device.bo_xfer_addr = (uint32_t)pCSW;
					pusb->device.bo_xfer_residue = USB_CSW_LEN;
					pusb->device.bo_xfer_tranferred = 0;
					fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
					if(fret == USB_RETVAL_NOTCOMP)
					{
						pusb->device.bo_state = USB_BO_CSW;
					}
					else
					{
						if(fret == USB_RETVAL_COMPOK)
						{
						 	pusb->device.bo_xfer_tranferred = pusb->device.bo_xfer_residue;
							pusb->device.bo_xfer_residue = 0;
							pusb->device.bo_state = USB_BO_CBW;
						}
						else
						{
						 	pusb->device.bo_state = USB_BO_CBW;
							PRINTF("Error: CSW Send Error!!\n");
						}
					}
	  				break;
	  			case 0x1e://Remove Allow/Prevent
					pCSW->dCSWSig = USB_CSW_SIG;
					pCSW->dCSWDataRes = 0;
					pCSW->bCSWStatus = 0;
					pusb->device.bo_xfer_addr = (uint32_t)pCSW;
					pusb->device.bo_xfer_residue = USB_CSW_LEN;
					pusb->device.bo_xfer_tranferred = 0;
					fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
					if(fret == USB_RETVAL_NOTCOMP)
					{
						pusb->device.bo_state = USB_BO_CSW;
					}
					else
					{
						pusb->device.bo_state = USB_BO_CBW;
						PRINTF("Error: CSW Send Error!!\n");
					}
	  				break;
	  			case 0x2f://Verify(10)
					pCSW->dCSWSig = USB_CSW_SIG;
					pCSW->dCSWDataRes = 0;
					pCSW->bCSWStatus = 0;
					pusb->device.bo_xfer_addr = (uint32_t)pCSW;
					pusb->device.bo_xfer_residue = USB_CSW_LEN;
					pusb->device.bo_xfer_tranferred = 0;
					fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
					if(fret == USB_RETVAL_NOTCOMP)
					{
					 	pusb->device.bo_state = USB_BO_CSW;
					}
					else
					{
						pusb->device.bo_state = USB_BO_CBW;
						PRINTF("Error: CSW Send Error!!\n");
					}
	  				break;
	  			case 0x12://Inquiry
	  				pusb->device.bo_xfer_addr = (uint32_t)InquiryData;
	  				pusb->device.bo_xfer_residue = min(pCBW->dCBWDTL, 36);
	  				pusb->device.bo_xfer_tranferred = 0;
	  				fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
					if(fret == USB_RETVAL_NOTCOMP)
					{
						pusb->device.bo_state = USB_BO_TXDATA;
					}
					else
					{
						pusb->device.bo_state = USB_BO_CBW;
						PRINTF("Error: Data Send Error!!\n");
					}
	  				break;
	  			case 0x23://Read Format Capacity
					{
						uint8_t formatcap[12];
						uint32_t sec_cnt[2];

						formatcap[0] = 0x00;
						formatcap[1] = 0x00;
						formatcap[2] = 0x00;
						formatcap[3] = 0x08;
						formatcap[8] = 0x02;
						formatcap[9] = 0x00;
						formatcap[10] = 0x02;
						formatcap[11] = 0x00;

						//sec_cnt[0] = (pusb->device.bo_seccnt+1);
			    		sec_cnt[0] = wBoot_part_capacity(pCBW->bCBWLUN);
						//PRINTF("part index = %d, format capacity = %d\n", pCBW->bCBWLUN, sec_cnt[0]);
						formatcap[4] = sec_cnt[0]>>24;
						formatcap[5] = sec_cnt[0]>>16;
						formatcap[6] = sec_cnt[0]>>8;
						formatcap[7] = sec_cnt[0];

						pusb->device.bo_xfer_addr = (uint32_t)formatcap;
						pusb->device.bo_xfer_residue = min(pCBW->dCBWDTL, 12);
						pusb->device.bo_xfer_tranferred = 0;
						fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
						if(fret == USB_RETVAL_NOTCOMP)
						{
							pusb->device.bo_state = USB_BO_TXDATA;
						}
						else
						{
							pusb->device.bo_state = USB_BO_CBW;
							PRINTF("Error: Data Send Error!!\n");
						}
					}
	  				break;
	  			case 0x25://Read Capacity
					{
						uint8_t capacity[8];
						uint32_t sec_cnt[2];

						capacity[4] = 0x00;
						capacity[5] = 0x00;
						capacity[6] = 0x02;
						capacity[7] = 0x00;

						//sec_cnt[0] = (pusb->device.bo_seccnt);
					//sec_cnt[0] = (32768); //16Mbyte vdisk space
						sec_cnt[0] = wBoot_part_capacity(pCBW->bCBWLUN) - 1;
						//PRINTF("part index = %d, capacity = %d\n", pCBW->bCBWLUN, sec_cnt[0]);

						capacity[0] = sec_cnt[0]>>24;
						capacity[1] = sec_cnt[0]>>16;
						capacity[2] = sec_cnt[0]>>8;
						capacity[3] = sec_cnt[0];

						pusb->device.bo_xfer_addr = (uint32_t)capacity;
						pusb->device.bo_xfer_residue = min(pCBW->dCBWDTL, 8);
						pusb->device.bo_xfer_tranferred = 0;
						fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
						if(fret == USB_RETVAL_NOTCOMP)
						{
							pusb->device.bo_state = USB_BO_TXDATA;
						}
						else
						{
							pusb->device.bo_state = USB_BO_CBW;
							PRINTF("Error: Data Send Error!!\n");
						}
					}
	  				break;
	  			case 0x28://Read(10)  write to host
					{
						uint32_t read_len = 0;
						uint32_t read_offset = 0;
						int32_t part_start;

						read_len = pCBW->CBWCB[7];
						read_len <<= 8;
						read_len |= pCBW->CBWCB[8];      //���� read_len ��������
						//read_len <<= USB_DEV_SEC_BITS;   //���� read_len ���ֽ���

						read_offset = pCBW->CBWCB[2];
						read_offset <<= 8;
						read_offset |= pCBW->CBWCB[3];
						read_offset <<= 8;
						read_offset |= pCBW->CBWCB[4];
						read_offset <<= 8;
						read_offset |= pCBW->CBWCB[5];		//���� read_offset ��ƫ�Ƶ�����
						//read_offset <<= USB_DEV_SEC_BITS;  //From Blocks to Bytes  //���� read_offset ��ƫ�Ƶ��ֽ���
						pusb->device.bo_xfer_addr = pusb->device.bo_memory_base;

						//����flash����
			            part_start = wBoot_part_start(pCBW->bCBWLUN);
						//PRINTF("part index = %d, start = %d\n", pCBW->bCBWLUN, part_start);
						if(wBoot_block_read(read_offset + part_start, read_len, (void *)pusb->device.bo_memory_base) < 0)
						{
							PRINTF("part index = %d, start = %d, read_start = %d, len = %d\n", pCBW->bCBWLUN, part_start, read_offset + part_start, read_len);
							pCSW->dCSWSig = USB_CSW_SIG;
							pCSW->dCSWDataRes = pCBW->dCBWDTL;
							pCSW->bCSWStatus = 1;
							pusb->device.bo_xfer_addr = (uint32_t)pCSW;
							pusb->device.bo_xfer_residue = USB_CSW_LEN;
							pusb->device.bo_xfer_tranferred = 0;
							epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
							pusb->device.bo_state = USB_BO_CSW;

							PRINTF("Error: Flash Read Fail\n");
							break;
						}
						read_len <<= USB_DEV_SEC_BITS;   //���� read_len ���ֽ���
						read_offset <<= USB_DEV_SEC_BITS;  //From Blocks to Bytes  //���� read_offset ��ƫ�Ƶ��ֽ���

						pusb->device.bo_xfer_residue = min(read_len, MAX_DDMA_SIZE); //Max USB Packet is 64KB    //??
						pusb->device.bo_xfer_tranferred = 0;
					}
					fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
					if(fret == USB_RETVAL_NOTCOMP)
					{
						pusb->device.bo_state = USB_BO_TXDATA;
					}
					else
					{
						pusb->device.bo_state = USB_BO_CBW;
						PRINTF("Error: Data Send Error!!\n");
					}
	  				break;
	  			case 0x1A://Mode Sense(6)
					pusb->device.bo_xfer_addr = (uint32_t)SenseData;
					pusb->device.bo_xfer_residue = min(pCBW->dCBWDTL, 4);
					pusb->device.bo_xfer_tranferred = 0;
					fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
					if(fret == USB_RETVAL_NOTCOMP)
					{
					 	pusb->device.bo_state = USB_BO_TXDATA;
					}
					else
					{
						pusb->device.bo_state = USB_BO_CBW;
						PRINTF("Error: Data Send Error!!\n");
					}
	  				break;
	  			case 0x2A://Write(10)   read from host
	  				{
						write_len = pCBW->CBWCB[7];
						write_len <<= 8;
						write_len |= pCBW->CBWCB[8];			//���� write_len ��������
						write_len <<= USB_DEV_SEC_BITS;         //���� write_len ���ֽ���

						write_offset = pCBW->CBWCB[2];
						write_offset <<= 8;
						write_offset |= pCBW->CBWCB[3];
						write_offset <<= 8;
						write_offset |= pCBW->CBWCB[4];
						write_offset <<= 8;
						write_offset |= pCBW->CBWCB[5];			//���� write_offset ��ƫ�Ƶ�����
						write_offset <<= USB_DEV_SEC_BITS;		//���� write_offset ��ƫ�Ƶ��ֽ���

						pusb->device.bo_xfer_addr = pusb->device.bo_memory_base;

						pusb->device.bo_xfer_residue = min(write_len, MAX_DDMA_SIZE);
						pusb->device.bo_xfer_tranferred = 0;
	  				}
	  				fret = epx_out_handler_dev(pusb, pusb->device.bo_ep_out, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
	  				if(fret==USB_RETVAL_COMPOK)
					{
						int32_t flash_ret, start, nsector;
						//write to flash
						start = (write_offset>>9) + wBoot_part_start(pCBW->bCBWLUN);
						nsector = write_len>>USB_DEV_SEC_BITS;
						flash_ret = wBoot_block_write(start, nsector, (void *)pusb->device.bo_memory_base);
						PRINTF("part index = %d, start = %d\n", pCBW->bCBWLUN, start);
						if(flash_ret < 0)
						{
							PRINTF("flash write start %d sector %d failed\n", start, nsector);
							pCSW->dCSWSig = USB_CSW_SIG;
							pCSW->dCSWDataRes = pCBW->dCBWDTL;
							pCSW->bCSWStatus = 1;
							pusb->device.bo_xfer_addr = (uint32_t)pCSW;
							pusb->device.bo_xfer_residue = USB_CSW_LEN;
							pusb->device.bo_xfer_tranferred = 0;
							epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
							pusb->device.bo_state = USB_BO_CSW;

							PRINTF("Error: Flash Write Fail\n");
							break;
						}
						pusb->device.bo_xfer_tranferred = pusb->device.bo_xfer_residue;
						pusb->device.bo_xfer_residue = 0;
						pCSW->dCSWSig = USB_CSW_SIG;
						pCSW->dCSWDataRes = pCBW->dCBWDTL - pusb->device.bo_xfer_tranferred;
						pCSW->bCSWStatus = 0;
						pusb->device.bo_xfer_addr = (uint32_t)pCSW;
						pusb->device.bo_xfer_residue = USB_CSW_LEN;
						pusb->device.bo_xfer_tranferred = 0;
						fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
						if(fret == USB_RETVAL_NOTCOMP)
						{
						 	pusb->device.bo_state = USB_BO_CSW;
						}
						else
						{
							pusb->device.bo_state = USB_BO_CBW;
							PRINTF("Error: CSW Send Error!!\n");
						}
					}
	  				else
  					{
						if(fret == USB_RETVAL_NOTCOMP)
						{
							pusb->device.bo_state = USB_BO_RXDATA;
						}
						else
						{
							pusb->device.bo_state = USB_BO_CBW;
							PRINTF("Error: Rx Data Error!!\n");
						}
  					}
	  				break;
	  			default:
	  				break;
	  		}
			break;

		case USB_BO_RXDATA:
			fret = epx_out_handler_dev(pusb, pusb->device.bo_ep_out, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
			if(fret==USB_RETVAL_COMPOK)
			{
				int32_t flash_ret, start, nsector;
				//write to flash
				start = (write_offset>>9) + wBoot_part_start(pCBW->bCBWLUN);
				nsector = write_len>>USB_DEV_SEC_BITS;
				flash_ret = wBoot_block_write(start, nsector, (void *)pusb->device.bo_memory_base);
				if(flash_ret < 0)
				{
					PRINTF("flash write start %d sector %d failed\n", start, nsector);
					pCSW->dCSWSig = USB_CSW_SIG;
					pCSW->dCSWDataRes = pCBW->dCBWDTL;
					pCSW->bCSWStatus = 1;
					pusb->device.bo_xfer_addr = (uint32_t)pCSW;
					pusb->device.bo_xfer_residue = USB_CSW_LEN;
					pusb->device.bo_xfer_tranferred = 0;
					epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
					pusb->device.bo_state = USB_BO_CSW;

					PRINTF("Error: Flash Write Fail\n");
					break;
				}


				pusb->device.bo_xfer_tranferred = pusb->device.bo_xfer_residue;
				pusb->device.bo_xfer_residue = 0;
				pCSW->dCSWSig = USB_CSW_SIG;
				pCSW->dCSWDataRes = pCBW->dCBWDTL - pusb->device.bo_xfer_tranferred;
				pCSW->bCSWStatus = 0;
				pusb->device.bo_xfer_addr = (uint32_t)pCSW;
				pusb->device.bo_xfer_residue = USB_CSW_LEN;
				pusb->device.bo_xfer_tranferred = 0;
				epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
				pusb->device.bo_state = USB_BO_CSW;
			}
			else if(fret == USB_RETVAL_COMPERR)
			{
				PRINTF("Error: RxData Error\n");
				pusb->device.bo_state = USB_BO_CBW;
			}
			break;

		case USB_BO_TXDATA:
			fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
			if(fret==USB_RETVAL_COMPOK)
			{
				pusb->device.bo_xfer_tranferred = pusb->device.bo_xfer_residue;
				pusb->device.bo_xfer_residue = 0;
				pCSW->dCSWSig = USB_CSW_SIG;
				pCSW->dCSWDataRes = pCBW->dCBWDTL - pusb->device.bo_xfer_tranferred;
				pCSW->bCSWStatus = 0;
				pusb->device.bo_xfer_addr = (uint32_t)pCSW;
				pusb->device.bo_xfer_residue = USB_CSW_LEN;
				pusb->device.bo_xfer_tranferred = 0;
				epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
				pusb->device.bo_state = USB_BO_CSW;
			}
			else if(fret == USB_RETVAL_COMPERR)
			{
				PRINTF("Error: TxData Error\n");
				pusb->device.bo_state = USB_BO_CBW;
			}
			break;

		case USB_BO_CSW:
			fret = epx_in_handler_dev(pusb, pusb->device.bo_ep_in, pusb->device.bo_xfer_addr, pusb->device.bo_xfer_residue, USB_PRTCL_BULK);
			if(fret==USB_RETVAL_COMPOK)
			{
				pusb->device.bo_xfer_tranferred = pusb->device.bo_xfer_residue;
				pusb->device.bo_xfer_residue = 0; //min(pCBW->dCBWDTL, 36);
				pusb->device.bo_state = USB_BO_CBW;
			}
			else if(fret==USB_RETVAL_COMPERR)
			{
				pusb->device.bo_state = USB_BO_CBW;
			  	PRINTF("Error: Tx CSW Error!!\n");
			}
			break;

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
static uint32_t ep0_set_config_handler_dev(pusb_struct pusb)
{
	pSetupPKG ep0_setup = (pSetupPKG)(pusb->buffer);
	uConfigDes *pconfig;
	uIntfDes *pintf;
	uint32_t i;
	uint32_t fifo_addr = 1024;    //

	pconfig = (uConfigDes *)(pusb->device.config_desc);
	if(pconfig->bConfigVal != ep0_setup->wValue)
	{
		PRINTF("Error: Right Configval %d; Error Configval %d\n", pconfig->bConfigVal, ep0_setup->wValue);
		return 0;
	}
	pintf = (uIntfDes *)(&pusb->device.config_desc[USB_CONFIG_DESC_LEN]);
	for(i=0; i<pintf->bNumEP; i++)
	{
		const uEPDes * const pep = (uEPDes*)(&pusb->device.config_desc[USB_CONFIG_DESC_LEN + USB_INTF_DESC_LEN + USB_ENDP_DESC_LEN*i]);
		const uint32_t ep_no = pep->bEPAddr & 0xf;
		const uint32_t ep_attr = pep->bmAttrib & 0x3;
		//uint32_t interval;
		const uint32_t ep_dir = (pep->bEPAddr>>7) & 0x1; //0 for OUT, and 1 for IN

		uint32_t maxpktsz;




		maxpktsz = pep->wMaxPktSize1 & 0x7;
		maxpktsz <<= 8;
		maxpktsz |= pep->wMaxPktSize0 & 0xff;

		usb_select_ep(pusb, ep_no);
		if(ep_dir)
		{
			usb_set_eptx_maxpkt(pusb, maxpktsz, USB_EP_FIFO_SIZE/maxpktsz);
			usb_set_eptx_fifo_addr(pusb, fifo_addr);
			usb_set_eptx_fifo_size(pusb, 1, USB_EP_FIFO_SIZE);
			pusb->device.eptx_prtcl[ep_no-1] = ep_attr;
			pusb->device.eptx_fifo[ep_no-1] = (fifo_addr<<16)|(0x1u << 15)|(maxpktsz&0x7fff);
			fifo_addr += ((USB_EP_FIFO_SIZE<<1)+(USB_FIFO_ADDR_BLOCK-1))&(~(USB_FIFO_ADDR_BLOCK-1));  //Align to USB_FIFO_ADDR_BLOCK
			if(pintf->bIntfProtocol == 0x50)  //Bulk Only Device
			{
				pusb->device.bo_ep_in = ep_no;
			}
			usb_eptx_flush_fifo(pusb);
			usb_eptx_flush_fifo(pusb);
		}
		else
		{
			usb_set_eprx_maxpkt(pusb, maxpktsz, USB_EP_FIFO_SIZE/maxpktsz);
			usb_set_eprx_fifo_addr(pusb, fifo_addr);
			usb_set_eprx_fifo_size(pusb, 1, USB_EP_FIFO_SIZE);
			pusb->device.eprx_prtcl[ep_no-1] = ep_attr;
			pusb->device.eprx_fifo[ep_no-1] = (fifo_addr<<16)|(0x1u << 15)|(maxpktsz&0x7fff);
			fifo_addr += ((USB_EP_FIFO_SIZE<<1)+(USB_FIFO_ADDR_BLOCK-1))&(~(USB_FIFO_ADDR_BLOCK-1));
			if(pintf->bIntfProtocol == 0x50)  //Bulk Only Device
			{
				pusb->device.bo_ep_out = ep_no;
			}
			usb_eprx_flush_fifo(pusb);
			usb_eprx_flush_fifo(pusb);
		}
	}

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
static int32_t ep0_in_handler_dev(pusb_struct pusb)
{
	uint32_t temp = 0;
	pSetupPKG ep0_setup = (pSetupPKG)(pusb->buffer);

	if((ep0_setup->bmRequest&0x60)==0x00)
	{
		switch(ep0_setup->bRequest)
		{
			case 0x00 :
    			PRINTF("usb_device: Get Status\n");
			case 0x06 :
				switch((ep0_setup->wValue>>8)&0xff)
				{
					case 0x01:              //Get Device Desc
						pusb->ep0_maxpktsz = *((uint8_t*)(pusb->device.dev_desc+7));
						pusb->ep0_xfer_srcaddr = (uint32_t)pusb->device.dev_desc;
						pusb->ep0_xfer_residue = min(*((uint8_t*)pusb->ep0_xfer_srcaddr), ep0_setup->wLength);
						break;
					case 0x02:              //Get Configuration Desc
						temp = pusb->device.config_desc[3] & 0xff;
						temp = temp << 8;
						temp |= pusb->device.config_desc[2] & 0xff;
						pusb->ep0_xfer_srcaddr = (uint32_t)pusb->device.config_desc;
						pusb->ep0_xfer_residue = min(temp, ep0_setup->wLength);
						break;
					case 0x03:             //Get String Desc
					   	temp = ep0_setup->wValue&0xff;
					   	if(temp < 4)
						{
							pusb->ep0_xfer_srcaddr = (uint32_t)pusb->device.str_desc[temp];
							pusb->ep0_xfer_residue = min(*((uint8_t*)pusb->ep0_xfer_srcaddr), ep0_setup->wLength);
						}
						else
						{
							PRINTF("Unkown String Desc!!\n");
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
					    pusb->ep0_xfer_srcaddr = (uint32_t)pusb->device.dev_qual;
					    pusb->ep0_xfer_residue = min(*((uint8_t*)pusb->ep0_xfer_srcaddr), ep0_setup->wLength);
				    	break;
					case 0x09:
					    pusb->ep0_xfer_srcaddr = (uint32_t)pusb->device.otg_desc;
					    pusb->ep0_xfer_residue = min(*((uint8_t*)pusb->ep0_xfer_srcaddr), ep0_setup->wLength);
				    	break;
					default  :
					    pusb->ep0_xfer_residue = 0;
					    PRINTF("usb_device: Get Unkown Descriptor\n");
				}
		      	break;
	     	case 0x08 :
				pusb->ep0_xfer_residue = 0;
				PRINTF("usb_device: Get Configuration\n");
				break;
			case 0x0A :
				pusb->ep0_xfer_residue = 0;
				PRINTF("usb_device: Get Interface\n");
				break;
			case 0x0C :
				pusb->ep0_xfer_residue = 0;
				PRINTF("usb_device: Sync Frame\n");
				break;
	      	default   :
	        	pusb->ep0_xfer_residue = 0;
	        	PRINTF("usb_device: Unkown Standard Request = 0x%x\n", ep0_setup->bRequest);
		}
	}
	else if((ep0_setup->bmRequest&0x60)==0x20)
	{
		switch(ep0_setup->bRequest)
		{
			case 0x00FE :
				pusb->ep0_xfer_srcaddr = (uint32_t)&(pusb->device.MaxLUN);
				pusb->ep0_xfer_residue = 1;
				PRINTF("usb_device: Get MaxLUN\n");
				break;
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
static int32_t ep0_out_handler_dev(pusb_struct pusb)
{
	pSetupPKG ep0_setup = (pSetupPKG)(pusb->buffer);

	switch(ep0_setup->bRequest)
	{
		case 0x01 :
			if(ep0_setup->wIndex&0x80)
			{

			}
			else
			{

			}
		  	break;

		case 0x03 :
			 switch(ep0_setup->wValue)
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
				PRINTF("usb_device: Unkown SetFeature Value: 0x%x\n", ep0_setup->wValue);
			}
			break;

		case 0x05 :
			usb_set_dev_addr(pusb, ep0_setup->wValue);
       		pusb->device.func_addr = ep0_setup->wValue;
       		PRINTF("usb_device: Set Address 0x%x\n", ep0_setup->wValue);
			break;

		case 0x07 :
       		PRINTF("usb_device: Set Descriptor\n");
      		break;
    	case 0x09 :
      		ep0_set_config_handler_dev(pusb);
    		break;
    	case 0x0B :
	       	PRINTF("usb_device: Set Interface\n");
	      	break;
    	default   :
      		PRINTF("usb_device: Unkown EP0 OUT: 0x%x!!\n", ep0_setup->bRequest);
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
static uint32_t usb_dev_ep0xfer(pusb_struct pusb)
{
	//uint32_t i=0;
	uint32_t temp=0;
	pSetupPKG ep0_setup = (pSetupPKG)(pusb->buffer);
	uint32_t is_last;
	uint32_t byte_trans;
	//uint32_t src_addr;

	if(pusb->role != USB_ROLE_DEV) return 0;
//	if(!pusb->ep0_flag) return 0;
//	pusb->ep0_flag--;

	usb_select_ep(pusb, 0);
  	temp = usb_get_ep0_csr(pusb);

	if(pusb->ep0_xfer_state == USB_EP0_DATA)  //Control IN Data Stage or Stage Status
	{
		if(temp&0x1)
		{
			pusb->ep0_xfer_state = USB_EP0_SETUP;
		}
		else if(temp&(0x1u << 4))
		{
			usb_set_ep0_csr(pusb, 0x80);
			PRINTF("WRN: EP0 Setup End!!\n");
		}
		else if(!(temp&(0x1u << 1)))
		{
			if(pusb->ep0_xfer_residue)
		 	{
		 		pusb->ep0_xfer_srcaddr += pusb->ep0_maxpktsz;
		 		if(pusb->ep0_xfer_residue == 0xffffffff)
			  	{
				  	is_last = 1;
				  	byte_trans = 0;
				  	pusb->ep0_xfer_residue = 0;
			   	}
			  	else
			   	{
			   		if(pusb->ep0_xfer_residue < pusb->ep0_maxpktsz)
			    	{
						is_last = 1;
						byte_trans = pusb->ep0_xfer_residue;
						pusb->ep0_xfer_residue = 0;
			    	}
			   		else if(pusb->ep0_xfer_residue == pusb->ep0_maxpktsz)
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
			 	if(is_last || (!byte_trans))
			 	{
			 		usb_set_ep0_csr(pusb, 0x0a);
			 	}
			 	else
			 	{
			 		usb_set_ep0_csr(pusb, 0x02);
				}

			 	if(usb_get_ep0_count(pusb))
			 	{
		  			PRINTF("Error: COUNT0 = 0x%x\n", usb_get_ep0_count(pusb));
		 		}
		 	}
		}
		else
		{
			PRINTF("WRN: Unkown EP0 Interrupt, CSR=0x%x!!\n", temp);
		}
	}
	if(pusb->ep0_xfer_state == USB_EP0_SETUP)  //Setup or Control OUT Status Stage
	{
		if(temp&0x1)
		{
			uint32_t ep0_count = usb_get_ep0_count(pusb);

			if(ep0_count==8)
			{
				//pusb->ep0_flag = 0;
				usb_read_ep_fifo(pusb, 0, (uint32_t)pusb->buffer, 8);

				if(ep0_setup->bmRequest&0x80)//in
				{
					usb_set_ep0_csr(pusb, 0x40);
					ep0_in_handler_dev(pusb);

					if(pusb->ep0_xfer_residue<pusb->ep0_maxpktsz)
					{
						is_last = 1;
						byte_trans = pusb->ep0_xfer_residue;
						pusb->ep0_xfer_residue = 0;
					}
				 	else if(pusb->ep0_xfer_residue==pusb->ep0_maxpktsz)
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
				 	if(is_last || (!byte_trans))
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
			ep0_out_handler_dev(pusb);
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

///*
//************************************************************************************************************
//*
//*                                             function
//*
//*    �������ƣ�
//*
//*    �����б�
//*
//*    ����ֵ  ��
//*
//*    ˵��    ��
//*
//*
//************************************************************************************************************
//*/
//static void usb_power_polling_dev(pusb_struct pusb)
//{
//	if(pusb->role != USB_ROLE_DEV) return;
//  	if(pusb->connect) return;
//
//  	if(usb_get_vbus_level(pusb) == USB_VBUS_VBUSVLD)
//  	{
//		if(pusb->timer != USB_DEVICE_VBUS_DET_TIMER)
//		{
//			if(pusb->timer != USB_IDLE_TIMER)  //timer should not occupied by any other ones at this time
//			{
//				PRINTF("Error: Timer Ocuppied\n");
//			}
//		    pusb->timer = USB_DEVICE_VBUS_DET_TIMER;
//
//		   	pusb->loop = 0;
//		   	pusb->power_debouce = 0x50;
//
//			return;
//		}
//		else if(pusb->power_debouce > 0)
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
//	PRINTF("USB Connect!!\n");
//
//	return;
//}
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
static uint32_t usb_device_function(pusb_struct pusb)
{
	if(pusb->role != USB_ROLE_DEV)
	{
		return 0;
	}

	usb_dev_bulk_xfer(pusb);

	return 1;
}

//static uint32_t bus_irq_count = 0;
//static uint32_t ep0_irq_count = 0;
//static uint32_t eptx_irq_count = 0;
//static uint32_t eprx_irq_count = 0;

void usb_device_function0(void)
{
	extern PCD_HandleTypeDef hpcd_USB_OTG;
	usb_struct * const pusb = & hpcd_USB_OTG.awxx_usb;
	usb_device_function(pusb);
}

static void usb_params_init(pusb_struct pusb)
{
	static uint8_t ALIGNX_BEGIN device_bo_memory_base [128 * 1024] ALIGNX_END;
	static uint8_t ALIGNX_BEGIN device_bo_bufbase [64 * 1024] ALIGNX_END;
	uint32_t i;

	//usb_clock_init();

//	pusb->index = 0;
//	pusb->reg_base = USBOTG0_BASE;
//	pusb->irq_no = GIC_SRC_USB0;
//	pusb->drq_no = 0x04;

	pusb->role = USB0_ROLE;  //USB_ROLE_HST; //USB_ROLE_UNK
	pusb->speed = USB0_SPEED;

	if(pusb->speed==USB_SPEED_HS)
	{
		pusb->device.dev_desc   = USB_HS_BULK_DevDesc;
		pusb->device.config_desc= USB_HS_BULK_ConfigDesc;
	}
	else
	{
		pusb->device.dev_desc   = USB_FS_BULK_DevDesc;
		pusb->device.config_desc= USB_FS_BULK_ConfigDesc;
	}
	for(i=0; i<ARRAY_SIZE(pusb->device.str_desc); i++)
	{
		pusb->device.str_desc[i]   = USB_StrDec0[i];
	}

	pusb->device.intf_desc  = USB_HS_BULK_ConfigDesc;
	pusb->device.endp_desc  = USB_HS_BULK_ConfigDesc;
	pusb->device.dev_qual   = USB_DevQual;
	pusb->device.otg_desc   = USB_OTGDesc;

	pusb->device.MaxLUN = wBoot_part_count(1) - 1;
	PRINTF("part count = %d\n", pusb->device.MaxLUN + 1);
	pusb->device.bo_memory_base = (uintptr_t) device_bo_memory_base;//(uint32_t)wBoot_malloc(128 * 1024);		//use to storage user data
	pusb->device.bo_bufbase = (uintptr_t) device_bo_bufbase;//(uint32_t)wBoot_malloc(64 * 1024);				//use to usb ping-pang buffer
	//pusb->ep0_flag = 0;
	pusb->ep0_xfer_state = USB_EP0_SETUP;
	//dma
	pusb->dma = wBoot_dma_request(1);
	if (!pusb->dma)
	{
		PRINTF("usb error: request dma fail\n");
	}

}

static void usb_irq_handler(pusb_struct pusb)
{
	//uint32_t index;
	uint32_t temp;
	uint32_t i;

	//bus interrupt
	temp = usb_get_bus_interrupt_status(pusb);
	usb_clear_bus_interrupt_status(pusb, temp);
	if(temp & USB_BUSINT_SOF)
	{
		//pusb->sof_count ++;
		temp &= ~USB_BUSINT_SOF;
	}

	if(temp & usb_get_bus_interrupt_enable(pusb))
	{
//		pusb->busirq_status |= temp;
//		pusb->busirq_flag ++;
		//bus_irq_count ++;
		unsigned busirq_status = temp & usb_get_bus_interrupt_enable(pusb);

		uint32_t busirq_en;
		uint32_t i;
		uint32_t ep_save = usb_get_active_ep(pusb);

//		if(pusb->role != USB_ROLE_DEV) return 0;
	//  	if(!pusb->busirq_flag) return 0;
	//  	pusb->busirq_flag--;
	  	usb_select_ep(pusb, 0);
	  	busirq_en = usb_get_bus_interrupt_enable(pusb);

	  	if(busirq_status & busirq_en & USB_BUSINT_SUSPEND)
	  	{
		  	//Suspend Service Subroutine

			if(wBoot_dma_QueryState(pusb->dma))
			{
				PRINTF("Error: DMA for EP is not finished after Bus Suspend\n");
			}
			wBoot_dma_stop(pusb->dma);
		  	PRINTF("uSuspend\n");
	  	}

		if(busirq_status & busirq_en & USB_BUSINT_RESUME)
		{
			//Resume Service Subroutine
			PRINTF("uResume\n");
		}

		if(busirq_status & busirq_en & USB_BUSINT_RESET)
		{
			//Device Reset Service Subroutine
			//pusb->rst_cnt ++;
			for(i=0; i<USB_MAX_EP_NO; i++)
			{
				pusb->eptx_flag[i] = 0;
				pusb->eprx_flag[i] = 0;
				pusb->eptx_xfer_state[i] = USB_EPX_SETUP;
				pusb->eprx_xfer_state[i] = USB_EPX_SETUP;
			}
			pusb->device.func_addr = 0x00;
			pusb->device.bo_state = USB_BO_IDLE;
			//pusb->timer = USB_IDLE_TIMER;

			//Reset Function Address
			usb_set_dev_addr(pusb, 0x00);
			//Bus Reset may disable all interrupt enable, re-enable the interrupts need
			usb_set_bus_interrupt_enable(pusb, 0xf7);
			usb_set_eptx_interrupt_enable(pusb, 0xffff);
			usb_set_eprx_interrupt_enable(pusb, 0xfffe);

			if(wBoot_dma_QueryState(pusb->dma))
			{
				PRINTF("Error: DMA for EP is not finished after Bus Reset\n");
			}
			wBoot_dma_stop(pusb->dma);
		  	PRINTF("uReset\n");
		}

	  	if(busirq_status & busirq_en & USB_BUSINT_SESSEND)
	  	{
	  		//Device Reset Service Subroutine
			PRINTF("uSessend\n");
	  	}

	  	usb_select_ep(pusb, ep_save);
	}

	//tx interrupt
	temp = usb_get_eptx_interrupt_status(pusb);
	usb_clear_eptx_interrupt_status(pusb, temp);
	if(temp&0x01)
	{
		//pusb->ep0_flag ++;
		usb_dev_ep0xfer(pusb);
		//ep0_irq_count ++;
	}
	if(temp&0xfffe)
	{
		for(i=0; i<USB_MAX_EP_NO; i++)
		{
			if (temp & (0x2<<i))
			{
				pusb->eptx_flag[i] ++;
				//usb_dev_bulk_xfer(pusb);
			}
		}
		//eptx_irq_count ++;
	}

	//rx interrupt
	temp = usb_get_eprx_interrupt_status(pusb);
	usb_clear_eprx_interrupt_status(pusb, temp);
	if(temp&0xfffe)
	{
		for(i=0; i<USB_MAX_EP_NO; i++)
		{
			if(temp & (0x2<<i))
			{
				pusb->eprx_flag[i] ++;
				//usb_dev_bulk_xfer(pusb);
			}
		}
		//eprx_irq_count ++;
	}

	return;
}

void usb_struct_init(pusb_struct pusb)
{
	uint32_t i=0;

	//pusb->sof_count = 0;

//	pusb->rst_cnt = 0;
//	pusb->cur_fsm = 0;
//	pusb->fsm_cnt = 0;

	//pusb->ep0_flag = 0;
	pusb->ep0_xfer_state = USB_EP0_SETUP;
	pusb->ep0_maxpktsz = 64;

	for(i=0; i<USB_MAX_EP_NO; i++)
	{
		pusb->eptx_flag[i] = 0;
		pusb->eprx_flag[i] = 0;
		pusb->eptx_xfer_state[i] = USB_EPX_SETUP;
		pusb->eprx_xfer_state[i] = USB_EPX_SETUP;
	}

	pusb->device.func_addr = 0x00;
	pusb->device.bo_state = USB_BO_IDLE;
	pusb->device.bo_ep_in = 1;
	pusb->device.bo_ep_out = 1;


	//pusb->timer = USB_IDLE_TIMER;
	//pusb->loop = 0;

	//pusb->dma.valid = 0;
	//pusb->dma.type = DMAT_DDMA;                    //

	//bus_irq_count = 0;
	//ep0_irq_count = 0;
	//eptx_irq_count = 0;
	//eprx_irq_count = 0;

	return;
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
void usb_init(pusb_struct pusb)
{
	//uint32_t i=0;
	//uint32_t temp;

	usb_struct_init(pusb);

	//usb_set_phytune(pusb);
	//usb_drive_vbus(pusb, 0, pusb->index);
	usb_drive_vbus(pusb, 0, 0);

	usb_force_id(pusb, 1);

	if(pusb->speed==USB_SPEED_FS)
		usb_high_speed_disable(pusb);
	else
		usb_high_speed_enable(pusb);

	usb_suspendm_enable(pusb);

	usb_vbus_src(pusb, 0x1);
	usb_release_vbus(pusb);
	usb_force_vbus(pusb, 1);

	usb_select_ep(pusb, 0);
	usb_ep0_flush_fifo(pusb);

	PRINTF("USB Device!!\n");

	pusb->role = USB_ROLE_DEV;

	usb_clear_bus_interrupt_enable(pusb, 0xff);
	usb_set_bus_interrupt_enable(pusb, 0xf7);
	usb_set_eptx_interrupt_enable(pusb, 0x3f);
	usb_set_eprx_interrupt_enable(pusb, 0x3e);

	pusb->otg_dev = USB_OTG_B_DEVICE;

	return;
}


static void musb2_prepare(PCD_HandleTypeDef *hpcd)
{
	pusb_struct pusb = & hpcd->awxx_usb;
	usb_params_init(pusb);
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
//
//  __HAL_LOCK(hpcd);
//#if ! WITHNEWUSBHAL
//  (void)USB_EPClearStall(hpcd->Instance, ep);
//#else
//  if ((ep_addr & EP_ADDR_MSK) == 0U)
//  {
//	  USBPhyHw_endpoint_unstall(hpcd, 0);
//  }
//  else
//  {
//	  USBPhyHw_endpoint_unstall(hpcd, ep_addr);
//
//  }
//#endif
//  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

// https://github.com/abmwine/FreeBSD-src/blob/86cb59de6f4c60abd0ea3695ebe8fac26ff0af44/sys/dev/usb/controller/musb_otg_allwinner.c
// https://github.com/abmwine/FreeBSD-src/blob/86cb59de6f4c60abd0ea3695ebe8fac26ff0af44/sys/dev/usb/controller/musb_otg.c

void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
{

	usb_irq_handler(& hpcd->awxx_usb);
}

//void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
//{
//	pusb_struct pusb = & hpcd->awxx_usb;
//
//	//uint32_t index;
//
//	//bus interrupt
//	{
//		uint32_t busintstatus;
//		busintstatus = usb_get_bus_interrupt_status(pusb);
//		usb_clear_bus_interrupt_status(pusb, busintstatus);
//
////		if (busintstatus & USB_BUSINT_SOF)
////		{
////			//pusb->sof_count ++;
////			busintstatus &= ~ USB_BUSINT_SOF;
////			HAL_PCD_SOFCallback(hpcd);
////		}
//
//		busintstatus &= usb_get_bus_interrupt_enable(pusb);
//		if (busintstatus != 0)
//		{
//			//pusb->busirq_status |= temp;
//			//pusb->busirq_flag ++;
//			//bus_irq_count ++;
//			usb_bus_irq_handler_dev(hpcd, busintstatus);
//		}
//	}
//
//	//tx interrupt, ep0 interrupt
//	{
//		uint32_t eptxstatus;
//		eptxstatus = usb_get_eptx_interrupt_status(pusb);
//		usb_clear_eptx_interrupt_status(pusb, eptxstatus);
//
//		if (eptxstatus & 0x01)
//		{
//			//pusb->ep0_flag ++;
//			//ep0_irq_count ++;
//			usb_dev_ep0xfer_handler(hpcd);
//		}
//		if (eptxstatus & 0xfffe)
//		{
//			uint32_t i;
//			for (i=0; i<USB_MAX_EP_NO; i++)
//			{
//				if (eptxstatus & (0x2 << i))
//				{
//					//pusb->eptx_flag[i] ++;
//					usb_dev_eptx_handler(hpcd, i);
//				}
//			}
//			//eptx_irq_count ++;
//		}
//	}
//
//	//rx interrupt
//	{
//		uint32_t eprxstatus;
//		eprxstatus = usb_get_eprx_interrupt_status(pusb);
//		usb_clear_eprx_interrupt_status(pusb, eprxstatus);
//
//		if (eprxstatus & 0xfffe)
//		{
//			uint32_t i;
//			for (i=0; i<USB_MAX_EP_NO; i++)
//			{
//				if (eprxstatus & (0x2 << i))
//				{
//					pusb->eprx_flag[i] ++;
//					usb_dev_eprx_handler(hpcd, i);
//				}
//			}
//			//eprx_irq_count ++;
//		}
//	}
//}

/**
  * @brief  USB_DevConnect : Connect the USB device by enabling Rpu
  * @param  USBx  Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_DevConnect(USBOTG_TypeDef *USBx)
{
	PRINTF("USB_DevConnect\n");

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

	  musb2_prepare(hpcd);

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
	pusb_struct awxx_usb = & hpcd->awxx_usb;

	usb_struct_init(&awxx_usb[0]);

	usb_init(&awxx_usb[0]);

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
//    ep->dma_addr = (uintptr_t)pBuf;
//  }
//
//  if ((ep_addr & EP_ADDR_MSK) == 0U)
//  {
////#if WITHNEWUSBHAL
////	  USBPhyHw_ep0_read(hpcd, pBuf, len);
////#else /* WITHNEWUSBHAL */
////    (void)USB_EP0StartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
////#endif /* WITHNEWUSBHAL */
//		usb_struct * const pusb = & hpcd->awxx_usb;
//		usb_device * const pdevice = & hpcd->awxx_device;
//
//		pusb->ep0_xfer_srcaddr = (uintptr_t) pBuf;
//		pusb->ep0_xfer_residue = len;
//
//	   	pusb->ep0_xfer_state = USB_EP0_DATA;
//
//	   	//PRINTF("HAL_PCD_EP_Receive: len=%u\n", len);
//  }
//  else
//  {
////#if WITHNEWUSBHAL
////	  USBPhyHw_endpoint_read(hpcd, ep_addr, pBuf, len);
////#else /* WITHNEWUSBHAL */
////    (void)USB_EPStartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
////#endif /* WITHNEWUSBHAL */
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
//    ep->dma_addr = (uintptr_t)pBuf;
//  }
//
//  if ((ep_addr & EP_ADDR_MSK) == 0U)
//  {
//		uint32_t is_last;
//		uint32_t byte_trans;
//    //(void)USB_EP0StartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
//		usb_struct * const pusb = & hpcd->awxx_usb;
//		usb_device * const pdevice = & pusb->device;
//
//		pusb->ep0_xfer_srcaddr = (uintptr_t) pBuf;
//		pusb->ep0_xfer_residue = len;
//
//		uint32_t ep_save = usb_get_active_ep(pusb);
//	  	usb_select_ep(pusb, 0);
//		//
//
//		if (pusb->ep0_xfer_residue < pusb->ep0_maxpktsz)
//		{
//			// Last packet
//			is_last = 1;
//			byte_trans = pusb->ep0_xfer_residue;
//			pusb->ep0_xfer_residue = 0;
//		}
//	 	else if (pusb->ep0_xfer_residue == pusb->ep0_maxpktsz)
//		{
//	 		// ZLP on next packet
//			is_last = 0;
//			byte_trans = pusb->ep0_xfer_residue;
//			pusb->ep0_xfer_residue = 0xffffffff;
//		}
//		else
//		{
//			// Not a last packet
//			is_last = 0;
//			byte_trans = pusb->ep0_maxpktsz;
//			pusb->ep0_xfer_residue -= pusb->ep0_maxpktsz;
//		}
//
//	 	usb_write_ep_fifo(pusb, 0, pusb->ep0_xfer_srcaddr, byte_trans);
//
//	 	if (is_last || (!byte_trans))
//	 	{
//	 		usb_set_ep0_csr(pusb, MUSB2_MASK_CSR0L_DATAEND | MUSB2_MASK_CSR0L_TXPKTRDY);
//	   	}
//	   	else
//	   	{
//	   		usb_set_ep0_csr(pusb, MUSB2_MASK_CSR0L_TXPKTRDY);
//	   	}
//	  	usb_select_ep(pusb, ep_save);
//
//	   	pusb->ep0_xfer_state = USB_EP0_DATA;
//  }
//  else
//  {
//    //(void)USB_EPStartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
//  }

  return HAL_OK;
}

uint32_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
	return hpcd->OUT_ep [ep_addr & EP_ADDR_MSK].xfer_count;
}


/**
  * @brief  Set the USB Device address.
  * @param  hpcd PCD handle
  * @param  address new device address
  * @retval HAL status
  */
//HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address)
//{
//  //__HAL_LOCK(hpcd);
//  hpcd->USB_Address = address;
//  //(void)USB_SetDevAddress(hpcd->Instance, address);
//  usb_set_dev_addr(& hpcd->awxx_usb, address);
//  //__HAL_UNLOCK(hpcd);
//
//  return HAL_OK;
//}

#endif /* (CPUSTYPE_T113 || CPUSTYPE_F133) */
