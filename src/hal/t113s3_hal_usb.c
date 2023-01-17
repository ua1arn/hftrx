/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if (CPUSTYLE_T113 || CPUSTYLE_F133)

#include "board.h"
#include "audio.h"
#include "formats.h"

#include "usb_device.h"
#include "usbd_def.h"

#include "t113s3_hal.h"
#include "t113s3_hal_usb.h"

#include "usbd_def.h"
#include "src/usb/usb200.h"
#include "src/usb/usbch9.h"

#define CDC_SET_LINE_CODING                     0x20
#define CDC_GET_LINE_CODING                     0x21
#define CDC_SET_CONTROL_LINE_STATE              0x22
#define CDC_SEND_BREAK                          0x23

#define APP_STATE_IDLE                 0
#define APP_STATE_DETACH               1
#define DFU_STATE_IDLE                 2
#define DFU_STATE_DNLOAD_SYNC          3
#define DFU_STATE_DNLOAD_BUSY          4
#define DFU_STATE_DNLOAD_IDLE          5
#define DFU_STATE_MANIFEST_SYNC        6
#define DFU_STATE_MANIFEST             7
#define DFU_STATE_MANIFEST_WAIT_RESET  8
#define DFU_STATE_UPLOAD_IDLE          9
#define DFU_STATE_ERROR                10

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

#define USB_DEV_SEC_BITS   		9//
#define USB_DEV_SEC_SIZE   		(0x1u << USB_DEV_SEC_BITS)//

#define USB_EP_FIFO_SIZE		512
#define USB_FIFO_ADDR_BLOCK		64//(USB_EP_FIFO_SIZE<<1)

#define MAX_DDMA_SIZE			(16*1024*1024)  //16MB

//#define USB_DEV0_TOTAL_CAP  	1//MB
//#define USB_DEV0_SEC_BITS   	9//
//#define USB_DEV0_SEC_SIZE   	(0x1u << USB_DEV0_SEC_BITS)//
//#define USB_DEV0_SEC_CNT    	(((USB_DEV0_TOTAL_CAP-1)<<11)|0xFFF)//

//#define USB_BO_DEV0_MEM_BASE	0x80800000//

//#define USB_BO_DEV0_BUF_BASE	(USB_BO_DEV0_MEM_BASE + (USB_DEV0_TOTAL_CAP<<20))//
#define USB_BO_DEV_BUF_SIZE	    (64 * 1024)//

//VBUS Level
#define USB_VBUS_SESSEND		0
#define USB_VUBS_SESSVLD		1
#define USB_VBUS_ABVALID		2
#define USB_VBUS_VBUSVLD		3

//////////////////////////////////////////////////////////////////////////
#define   USB2DRAM_PARAMS    	0x0f000f0f
#define   DRAM2USB_PARAMS    	0x0f0f0f00

#define get_bvalue(n)    		(*((volatile uint8_t *)(n)))          /* byte input */
#define put_bvalue(n,c)  		do { (*((volatile uint8_t *)(n)) = (c)); } while (0)    /* byte output */
#define get_hvalue(n)    		(*((volatile uint16_t *)(n)))          /* byte input */
#define put_hvalue(n,c)  		do { (*((volatile uint16_t *)(n)) = (c)); } while (0)    /* byte output */
#define get_wvalue(n)    		(*((volatile uint32_t *)(n)))          /* byte input */
#define put_wvalue(n,c)  		do { (*((volatile uint32_t *)(n)) = (c)); } while (0)    /* byte output */

#define  wBoot_dma_QueryState(hdma)  0
#define  wBoot_dma_stop(hdma)        do { } while (0)
#define  wBoot_dma_request(sect)     0

static usb_struct * volatile gpusb = NULL;

#if WITHUSBDMSC

#define USB_CBW_LEN				(31)
#define USB_CBW_SIG				(0x43425355)

#define USB_CSW_LEN				(13)
#define USB_CSW_SIG				(0x53425355)

#define  wBoot_part_start(part)      0
#define  wBoot_part_capacity(sect)   (AW_RAMDISK_SIZE/512)                     /* ����� �������� ������������ �����, ������ = 512 ���� */
#define  wBoot_part_count(sect)      1

static int wBoot_block_read(unsigned int start,unsigned int nblock,void *pBuffer)
{
 #if DISK_DDR
  memcpy((unsigned long*)pBuffer,(unsigned long*)(AW_RAMDISK_BASE+(start*512)),nblock*512);
 #else
//  TryRead:
  if (mmc_read_blocks(pBuffer,start,nblock)!=nblock)
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
  if (mmc_write_blocks(pBuffer,start,nblock)!=nblock)
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
#endif /* WITHUSBDMSC */

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
	ASSERT(ep_no <= USB_MAX_EP_NO);
	if (ep_no > USB_MAX_EP_NO)
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

/* host ? */
static void usb_set_ep0_type(pusb_struct pusb, uint32_t speed)
{
	USBOTG0->USB_RXTI = (speed & 0x3) << 6;
	//put_bvalue(USBOTG0_BASE + USB_bTXTYPE_OFF, (speed & 0x3) << 6);	// #define  USB_bRXTYPE_OFF      		(0x8E)
}

/* host ? */
static void usb_set_eptx_type(pusb_struct pusb, uint32_t speed, uint32_t protocol, uint32_t ep_no)
{
	uint32_t reg_val;

	ASSERT(ep_no < USB_MAX_EP_NO);
	reg_val = (speed & 0x3) << 6;
	reg_val |= (protocol & 0x3) << 4;
	reg_val |= (ep_no & 0xf) << 0;
	USBOTG0->USB_RXTI = reg_val;
	//put_bvalue(USBOTG0_BASE + USB_bTXTYPE_OFF, reg_val);
}

/* host ? */
static void usb_set_ep0_naklimit(pusb_struct pusb, uint32_t naklimit)
{
	USBOTG0->USB_RXNAKLIMIT = naklimit & 0x1f;
	//put_bvalue(USBOTG0_BASE + USB_bTXINTERVAL_OFF, naklimit & 0x1f);	// #define  USB_bRXINTERVAL_OFF  		(0x8F)
}

/* host ? */
static void usb_set_eptx_interval(pusb_struct pusb, uint32_t interval)
{
	USBOTG0->USB_RXNAKLIMIT = interval & 0xFF;
	//put_bvalue(USBOTG0_BASE + USB_bTXINTERVAL_OFF, interval & 0xFF);
}

/* host ? */
static void usb_set_eprx_type(pusb_struct pusb, uint32_t speed, uint32_t protocol, uint32_t ep_no)
{
	uint32_t reg_val;

	ASSERT(ep_no < USB_MAX_EP_NO);
	reg_val = (speed & 0x3) << 6;
	reg_val |= (protocol & 0x3) << 4;
	reg_val |= (ep_no & 0xf) << 0;
	USBOTG0->USB_RXTI = reg_val;
	//put_bvalue(USBOTG0_BASE + USB_bRXTYPE_OFF, reg_val);	// USB_bRXTYPE_OFF      		(0x8E)
}

/* host ? */
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

/* host ? */
static uint32_t usb_is_b_device(pusb_struct pusb)
{
	return (USBOTG0->USB_DEVCTL >> 7) & 0x1;
}

/* host ? */
static uint32_t usb_device_connected_is_fs(pusb_struct pusb)
{
	return (USBOTG0->USB_DEVCTL >> 6) & 0x1;
}

/* host ? */
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

/* host ? */
static void usb_start_session(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = 0x1;
	USBOTG0->USB_DEVCTL = reg_val;
}

/* host ? */
static void usb_end_session(pusb_struct pusb)
{
	uint8_t reg_val;

	reg_val = 0x0;
	USBOTG0->USB_DEVCTL = reg_val;
}

/* host ? */
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

//[31:16]-fifo address; [15]-double buffer; [14:0]-fifo size
static void usb_set_eptx_fifo_size(pusb_struct pusb, uint32_t is_dpb, uint32_t size)
{
	uint8_t reg_val;

	reg_val = 0;
	if (is_dpb) reg_val |= 0x1u << 4;
	reg_val |= aw_log2(size >> 3) & 0xf;
	USBOTG0->USB_TXFIFOSZ = reg_val;
}

//[31:16]-fifo address; [15]-double buffer; [14:0]-fifo size
static void usb_set_eprx_fifo_size(pusb_struct pusb, uint32_t is_dpb, uint32_t size)
{
	uint8_t reg_val;

	reg_val = 0;
	if (is_dpb) reg_val |= 0x1u << 4;
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
	while ((USBOTG0->USB_DMACTL & (0x1u << 0)) != 0)
		;
}

static void usb_fifo_accessed_by_dma(pusb_struct pusb, uint32_t ep_no, uint32_t is_tx)
{
	uint8_t reg_val;

	ASSERT(ep_no < USB_MAX_EP_NO);
	if (ep_no>USB_MAX_EP_NO)
		return;
	reg_val = 0x1;
	if (!is_tx) reg_val |= 0x1u << 1;
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

// host
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

// host
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

// host
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

// host
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

// host
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

// host
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

	if (id)
		USBOTG0->USB_ISCR |= (0x1u << 14);
	else
		USBOTG0->USB_ISCR &= ~ (0x1u << 14);
	USBOTG0->USB_ISCR |= (0x1u << 15);

}

/* src = 0..3 */
//	//0 * (1u << 10) |	// vbus25=0 dv=1
//	1 * (1u << 10) |	// vbus25=0 dv=1
//	//2 * (1u << 10) |	// vbus25=1 dv=3
//	//3 * (1u << 10) |	// vbus25=1 dv=3
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
	if (vbus)
		USBOTG0->USB_ISCR |= (0x1u << 12);
	else
		USBOTG0->USB_ISCR &= ~ (0x1u << 12);
	USBOTG0->USB_ISCR |= (0x1u << 13);
}

static void usb_drive_vbus(pusb_struct pusb, uint32_t vbus, uint32_t index)
{
	if (index == 0)
	{
		uint32_t temp;
//	//Set PB9 Output,USB0-DRV SET
//	 temp = get_wvalue(0x01c20800+0x28);
//	 temp &= ~ (0x7 << 4);
//	 temp |= (0x1u << 1);
//	 put_wvalue(0x01c20800+0x28, temp);
//
//	 if (vbus)
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
	ASSERT(ep_no < (USB_MAX_EP_NO + 1));
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

//
//static int32_t part_index = 0;

static int32_t dram_copy(uintptr_t src_addr, uintptr_t dest_addr, uint32_t bytes)
{
	memcpy((void *)src_addr, (void *)dest_addr, bytes);

	return 1;
}

static const uint8_t TestPkt[54] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xEE, 0xEE, 0xEE,
	0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xBF, 0xDF,
	0xEF, 0xF7, 0xFB, 0xFD, 0xFC, 0x7E, 0xBF, 0xDF, 0xEF, 0xF7,
	0xFB, 0xFD, 0x7E, 0x00
};

///////////////////////////////////////////////////////////
//For MassStorage Only
///////////////////////////////////////////////////////////
static const uint8_t InquiryData[]  =
{
	0x00, 0x80, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
	'A', 'W',
	'T',  'e' , 'c' , 'h' , ' ',  ' ',  'U' , 'S',  'B', ' ',
	'S',  't',  'o' , 'r' , 'a' , 'g' , 'e' , ' ',  ' ', ' ',
	' ',
	0x20, 0x30, 0x31, 0x30, 0x30
};

static const uint8_t SenseData[] = {0x03, 0x00, 0x00, 0x00};

static uint32_t aw_module(uint32_t x, uint32_t y)
{
	uint32_t val;

	if (y==0)
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

static void usb_read_ep_fifo(pusb_struct pusb, uint32_t ep_no, uintptr_t dest_addr, uint32_t count)
{
	uint8_t temp;
	uint8_t saved;

	if (ep_no>USB_MAX_EP_NO)
	{
		return;
	}
	saved = usb_get_fifo_access_config(pusb);
	usb_fifo_accessed_by_cpu(pusb);

	const uintptr_t pipe = usb_get_ep_fifo_addr(pusb, ep_no);

	if ((dest_addr % 4) == 0 && (count % 4) == 0)
	{
		uint32_t * dest = (uint32_t *) dest_addr;
		uint32_t i;
		for(i=0; i<count; i += 4)
		{
			* dest ++ = get_wvalue(pipe);
		}
	}
	else if ((dest_addr % 2) == 0 && (count % 2) == 0)
	{
		uint16_t * dest = (uint16_t *) dest_addr;
		uint32_t i;
		for(i=0; i<count; i += 2)
		{
			* dest ++ = get_hvalue(pipe);
		}
	}
	else
	{
		uint8_t * dest = (uint8_t *) dest_addr;
		uint32_t i;
		for(i=0; i<count; i++)
		{
			* dest ++ = get_bvalue(pipe);
		}
	}

	usb_set_fifo_access_config(pusb, saved);
}


static void usb_write_ep_fifo(pusb_struct pusb, uint32_t ep_no, uintptr_t src_addr, uint32_t count)
{
	uint8_t  saved;

	if (ep_no>USB_MAX_EP_NO)
	{
		return;
	}
	if (count == 0)
		return;
	saved = usb_get_fifo_access_config(pusb);
	usb_fifo_accessed_by_cpu(pusb);

	const uintptr_t pipe = usb_get_ep_fifo_addr(pusb, ep_no);
	if ((src_addr % 4) == 0 && (count % 4) == 0)
	{
		const uint32_t * src = (uint32_t *) src_addr;
		uint32_t i;
		for(i=0; i<count; i += 4)
		{
			put_wvalue(pipe, * src ++);
		}
	}
	else if ((src_addr % 2) == 0 && (count % 2) == 0)
	{
		const uint16_t * src = (uint16_t *) src_addr;
		uint32_t i;
		for(i=0; i<count; i += 2)
		{
			put_hvalue(pipe, * src ++);
		}
	}
	else
	{
		const uint8_t * src = (const uint8_t *) src_addr;
		uint32_t i;
		for(i=0; i<count; i++)
		{
			put_bvalue(pipe, * src ++);
		}
	}

	usb_set_fifo_access_config(pusb, saved);
}


static void usb_ep0_complete_send(pusb_struct pusb, uintptr_t addr, uint32_t len)
{
	uint32_t is_last;
	uint32_t byte_trans;

	pusb->ep0_xfer_srcaddr = addr;
	pusb->ep0_xfer_residue = len;

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
}

static void usb_ep0_complete_send_data(pusb_struct pusb)
{
	if (pusb->ep0_xfer_residue)
 	{
		uint32_t is_last;
		uint32_t byte_trans;

 		pusb->ep0_xfer_srcaddr += pusb->ep0_maxpktsz;
 		if (pusb->ep0_xfer_residue == 0xffffffff)
	  	{
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
	 		usb_set_ep0_csr(pusb, 0x0a);	// USB_TXCSR_FLUSHFIFO | USB_TXCSR_FIFONOTEMP
	 	}
	 	else
	 	{
	 		usb_set_ep0_csr(pusb, 0x02);	// USB_TXCSR_FIFONOTEMP
		}

	 	if (usb_get_ep0_count(pusb))
	 	{
  			PRINTF("Error: COUNT0 = 0x%x\n", usb_get_ep0_count(pusb));
 		}
 	}
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
#if WITHUSBDEV_DMAENABLE
static uint32_t usb_dev_get_buf_base(pusb_struct pusb, uint32_t buf_tag)
{
	return (pusb->device.bo_bufbase + buf_tag*USB_BO_DEV_BUF_SIZE);
}
#endif

static USB_RETVAL epx_out_handler_dev(pusb_struct pusb, uint32_t ep_no, uintptr_t dst_addr, uint32_t byte_count, uint32_t ep_type)
{
	USB_RETVAL ret = USB_RETVAL_NOTCOMP;
	uint32_t maxpkt;
	uint32_t ep_save = usb_get_active_ep(pusb);
	static uint32_t epout_timeout = 0;
#if WITHUSBDEV_DMAENABLE
	__dma_setting_t  p;
	uint32_t dram_addr;
#endif

	usb_select_ep(pusb, ep_no);
	maxpkt = usb_get_eprx_maxpkt(pusb);
	maxpkt = (maxpkt&0x7ff)*(((maxpkt&0xf800)>>11)+1);

	switch (pusb->eprx_xfer_state[ep_no-1])
	{
		case USB_EPX_SETUP:
		{
			pusb->eprx_buf_tagv[ep_no-1] = 0;
			pusb->eprx_xfer_addrv[ep_no-1] = dst_addr;
			pusb->eprx_xfer_residuev[ep_no-1] = byte_count;
			pusb->eprx_xfer_tranferredv[ep_no-1] = 0;


			if (!maxpkt)
			{
				usb_select_ep(pusb, ep_save);
				return USB_RETVAL_COMPOK;
			}

			if (byte_count>=maxpkt)
			{
				uint32_t xfer_count=0;

#if WITHUSBDEV_DMAENABLE
				xfer_count = min(pusb->eprx_xfer_residue, USB_BO_DEV_BUF_SIZE);
				pusb->dma_last_transfer = xfer_count;
				usb_fifo_accessed_by_dma(pusb, ep_no, 0);  //rx

				dram_addr = usb_dev_get_buf_base(pusb, pusb->eprx_buf_tag);

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
			    xfer_count = min(pusb->eprx_xfer_residuev[ep_no-1], maxpkt);
			    if (usb_get_eprx_csr(pusb) & USB_RXCSR_RXPKTRDY)
				{
					usb_fifo_accessed_by_cpu(pusb);
					if (usb_get_fifo_access_config(pusb) & 0x1)
					{
						PRINTF("Error: CPU Access Failed!!\n");
					}
					usb_read_ep_fifo(pusb, ep_no, pusb->eprx_xfer_addrv[ep_no-1], xfer_count);
					usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb) & USB_RXCSR_ISO); //Clear RxPktRdy
					pusb->eprx_xfer_residuev[ep_no-1] -= xfer_count;
					pusb->eprx_xfer_addrv[ep_no-1] += xfer_count;
					pusb->eprx_xfer_tranferredv[ep_no-1]  += xfer_count;
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
					usb_read_ep_fifo(pusb, ep_no, pusb->eprx_xfer_addrv[ep_no-1], byte_count);
					usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb) & USB_RXCSR_ISO); //Clear RxPktRdy
					pusb->eprx_xfer_residuev[ep_no-1] -= byte_count;
					pusb->eprx_xfer_tranferredv[ep_no-1]  += byte_count;
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
#if WITHUSBDEV_DMAENABLE
		if (!wBoot_dma_QueryState(pusb->dma))
	 	{
	 		uint32_t data_xfered = pusb->dma_last_transfer;

		  	pdev->epx_xfer_residue -= data_xfered;
		  	pusb->dma_last_transfer  = 0;
		  	pdev->epx_buf_tag = pdev->epx_buf_tag ? 0:1;

		  	if (pdev->epx_xfer_residue)
		  	{
		  		uint32_t xfer_count = min(pdev->epx_xfer_residue, USB_BO_DEV_BUF_SIZE);

				pusb->dma_last_transfer = xfer_count;
		  		usb_fifo_accessed_by_dma(pusb, ep_no, 0);

		  		dram_addr = usb_dev_get_buf_base(pusb, pdev->epx_buf_tag);

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
		  		if (byte_count&0x1ff)
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

			if (dram_copy(usb_dev_get_buf_base(pusb, pdev->epx_buf_tag? 0:1), pdev->epx_xfer_addr, data_xfered))
		  	{
		  		pdev->epx_xfer_addr += data_xfered;
		  		pdev->epx_xfer_tranferred += data_xfered;
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
		if (pusb->eprx_xfer_residuev[ep_no-1]>0)
		{
			if (usb_get_eprx_csr(pusb) & USB_RXCSR_RXPKTRDY)
			{
				uint32_t xfer_count = min(pusb->eprx_xfer_residuev[ep_no-1], maxpkt);

				usb_fifo_accessed_by_cpu(pusb);
				if (usb_get_fifo_access_config(pusb) & 0x1)
				{
					PRINTF("Error: CPU Access Failed!!\n");
				}
				usb_read_ep_fifo(pusb, ep_no, pusb->eprx_xfer_addrv[ep_no-1], xfer_count);
				usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb) & USB_RXCSR_ISO); //Clear RxPktRdy
				pusb->eprx_xfer_residuev[ep_no-1] -= xfer_count;
				pusb->eprx_xfer_addrv[ep_no-1] += xfer_count;
				pusb->eprx_xfer_tranferredv[ep_no-1]  += xfer_count;
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
			break;
	}

	usb_select_ep(pusb, ep_save);

	return ret;
}

static USB_RETVAL epx_in_handler_dev_iso(pusb_struct pusb, uint32_t ep_no, uintptr_t src_addr, uint32_t byte_count, uint32_t ep_type)
{
  	USB_RETVAL ret = USB_RETVAL_COMPOK;
	uint32_t ep_save = usb_get_active_ep(pusb);
	usb_select_ep(pusb, ep_no);
    if ((USB_TXCSR_FIFONOTEMP & usb_get_eptx_csr(pusb)) == 0)
    {
		uint32_t saved = usb_get_fifo_access_config(pusb);
		usb_fifo_accessed_by_cpu(pusb);
        usb_write_ep_fifo(pusb, ep_no, src_addr, byte_count);
    	usb_set_eptx_csr(pusb, USB_TXCSR_TXFIFO | USB_TXCSR_TXPKTRDY | (usb_get_eptx_csr(pusb) & USB_TXCSR_ISO));
    	usb_set_fifo_access_config(pusb, saved);
    }
    else
    {
    	ret = USB_RETVAL_NOTCOMP;
    }
	usb_select_ep(pusb, ep_save);
	return ret;
}

static USB_RETVAL epx_in_handler_dev(pusb_struct pusb, uint32_t ep_no, uintptr_t src_addr, uint32_t byte_count, uint32_t ep_type)
{
  	USB_RETVAL ret = USB_RETVAL_NOTCOMP;
	uint32_t maxpkt;
	uint32_t ep_save = usb_get_active_ep(pusb);
#if WITHUSBDEV_DMAENABLE
	__dma_setting_t  p;
	uint32_t dram_addr;
	uint32_t ping_pang_addr;
#endif
	usb_select_ep(pusb, ep_no);
	maxpkt = usb_get_eptx_maxpkt(pusb);
	maxpkt = (maxpkt&0x7ff)*(((maxpkt&0xf800)>>11)+1);

	switch (pusb->eptx_xfer_state[ep_no-1])
	{
		case USB_EPX_SETUP:
		{
			pusb->eptx_buf_tagv[ep_no-1] = 0;
			pusb->eptx_xfer_addrv[ep_no-1] = src_addr;
			pusb->eptx_xfer_residuev[ep_no-1] = byte_count;
			pusb->eptx_xfer_tranferredv[ep_no-1] = 0;

			if (!maxpkt)
			{
				usb_select_ep(pusb, ep_save);
				return USB_RETVAL_COMPOK;
			}

			if (byte_count>=maxpkt)
		 	{
#if WITHUSBDEV_DMAENABLE
				uint32_t xfer_count = 0;

		 		xfer_count = min(pdev->eptx_xfer_residue, USB_BO_DEV_BUF_SIZE);
		 		ping_pang_addr = usb_dev_get_buf_base(pusb, pusb->eptx_buf_tag);
		 		if (dram_copy(pusb->eptx_xfer_addr, ping_pang_addr, xfer_count))
	 			{
	 				pdev->eptx_xfer_addrv[ep_no-1] += xfer_count;
	 				pdev->eptx_xfer_residuev[ep_no-1] -= xfer_count;
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

		  		dram_addr = usb_dev_get_buf_base(pusb, pusb->eptx_buf_tag);

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

			    pusb->eptx_buf_tagv[ep_no-1] = pusb->epx_buf_tagv[ep_no-1] ? 0:1;

			    if (pusb->eptx_xfer_residue)
		    	{
		    		xfer_count = min(pusb->eptx_xfer_residuev[ep_no-1], USB_BO_DEV_BUF_SIZE);

		    		if (dram_copy(pdev->epx_xfer_addr, usb_dev_get_buf_base(pusb, pdev->eptx_buf_tag), xfer_count))
	    			{
	    				pusb->eptx_xfer_addrv[ep_no-1] += xfer_count;
	    				pusb->eptx_xfer_residuev[ep_no-1] -= xfer_count;
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
		    		pusb->dma_last_transferv[ep_no-1] = 0;
		    	}

		    	usb_set_eptx_csr(pusb, USB_TXCSR_AUTOSET|USB_TXCSR_TXFIFO|USB_TXCSR_DMAREQEN|USB_TXCSR_DMAREQMODE);

				pusb->eptx_xfer_statev[ep_no-1][ep_no-1] = USB_EPX_DATA;
#else
				usb_fifo_accessed_by_cpu(pusb);
				usb_write_ep_fifo(pusb, ep_no, pusb->eptx_xfer_addrv[ep_no-1], maxpkt);
				pusb->eptx_xfer_residuev[ep_no-1] -= maxpkt;
			  	pusb->eptx_xfer_tranferredv[ep_no-1] += maxpkt;
			  	pusb->eptx_xfer_addrv[ep_no-1] += maxpkt;
				usb_set_eptx_csr(pusb, USB_TXCSR_TXFIFO|USB_TXCSR_TXPKTRDY);
			    pusb->eptx_xfer_state[ep_no-1] = USB_EPX_DATA;
#endif
		 	}
			else
			{
				usb_fifo_accessed_by_cpu(pusb);

				usb_write_ep_fifo(pusb, ep_no, pusb->eptx_xfer_addrv[ep_no-1], byte_count);
				if (usb_get_fifo_access_config(pusb) & 0x1)
				{
					PRINTF("Error: FIFO Access Config Error!!\n");
			  	}
				usb_set_eptx_csr(pusb, USB_TXCSR_TXFIFO|USB_TXCSR_TXPKTRDY);
				pusb->eptx_xfer_state[ep_no-1] = USB_EPX_END;
			  	pusb->eptx_xfer_residuev[ep_no-1] = 0;
			  	pusb->eptx_xfer_tranferredv[ep_no-1] = byte_count;
			}
		}
		break;

		case USB_EPX_DATA:
		{
#if WITHUSBDEV_DMAENABLE
			if (!wBoot_dma_QueryState(pusb->dma))
		 	{
		 		if (pusb->dma_last_transfer)
	 			{
			  		dram_addr = usb_dev_get_buf_base(pusb, pusb->eptx_buf_tag);

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
			        wlibc_CleanFlushDCacheRegion((void *)dram_addr, pusb->dma_last_transferv[ep_no-1]);

			        wBoot_dma_start(pusb->dma, dram_addr, usb_get_ep_fifo_addr(pusb, ep_no), pusb->dma_last_transferv[ep_no-1]);

			    	pusb->epx_buf_tagv[ep_no-1] = pusb->epx_buf_tagv[ep_no-1] ? 0:1;

			    	if (pusb->eptx_xfer_residue)//Copy Data from storage to buffer
			    	{
			    		uint32_t xfer_count = min(pusb->eptx_xfer_residue, USB_BO_DEV_BUF_SIZE);

			    		if (dram_copy(pusb->eptx_xfer_addr, usb_dev_get_buf_base(pusb, pusb->epx_buf_tagv[ep_no-1]), xfer_count))
		    			{
		    				pusb->eptx_xfer_addrv[ep_no-1] += xfer_count;
		    				pusb->eptx_xfer_residuev[ep_no-1] -= xfer_count;
		    			}
		    			else
		    			{
		    				PRINTF("Error: storage to buffer copy error!!\n");
				        	ret = USB_RETVAL_COMPERR;
		    			}
			    		pusb->dma_last_transferv[ep_no-1] = xfer_count;
			    	}
			    	else
			    	{
			    		pusb->dma_last_transferv[ep_no-1] = 0;
			    	}
	 			}
	 			else
	 			{
	 				pusb->eptx_xfer_tranferredv[ep_no-1] = byte_count;
			    	maxpkt = usb_get_eptx_maxpkt(pusb);
			    	maxpkt = (maxpkt&0x7ff)*(((maxpkt&0xf800)>>11)+1);
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
				if (pusb->eptx_xfer_residuev[ep_no-1] > 0)   //Data is not transfered over
				{
					uint32_t xfer_count = min(pusb->eptx_xfer_residuev[ep_no-1], maxpkt);

					usb_fifo_accessed_by_cpu(pusb);
					usb_write_ep_fifo(pusb, ep_no, pusb->eptx_xfer_addrv[ep_no-1], xfer_count);
					pusb->eptx_xfer_residuev[ep_no-1] -= xfer_count;
			  		pusb->eptx_xfer_tranferredv[ep_no-1] += xfer_count;
			  		pusb->eptx_xfer_addrv[ep_no-1] += xfer_count;
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
			break;
	}
	usb_select_ep(pusb, ep_save);

	return ret;
}


#if WITHUSBDMSC && WITHWAWXXUSB

static uint64_t write_len;
static uint64_t write_offset;

static USB_RETVAL parse_cbw(const pCBWPKG pCBW, const pCSWPKG pCSW, pusb_struct pusb)
{
	const uint32_t bo_ep_in = (USBD_EP_MSC_IN & 0x0F);
	const uint32_t bo_ep_out = (USBD_EP_MSC_OUT & 0x0F);
	usb_device_msc * const pdev = & pusb->device_msc;
	USB_RETVAL fret = USB_RETVAL_NOTCOMP;
	switch (pCBW->CBWCB [0])
	{
	case 0x00: //Test Unit Ready
		pCSW->dCSWSig = USB_CSW_SIG;
		pCSW->dCSWDataRes = 0;
		pCSW->bCSWStatus = 0;
		pdev->bo_xfer_addr = (uintptr_t) pCSW;
		pdev->bo_xfer_residue = USB_CSW_LEN;
		pdev->bo_xfer_tranferred = 0;
		fret = epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
		if (fret == USB_RETVAL_NOTCOMP)
		{
			pdev->bo_state = USB_BO_CSW;
		}
		else
		{
			if (fret == USB_RETVAL_COMPOK)
			{
				pdev->bo_xfer_tranferred = pdev->bo_xfer_residue;
				pdev->bo_xfer_residue = 0;
				pdev->bo_state = USB_BO_CBW;
			}
			else
			{
				pdev->bo_state = USB_BO_CBW;
				PRINTF("Error: CSW Send Error!!\n");
			}
		}
		break;
	case 0x1e: //Remove Allow/Prevent
		pCSW->dCSWSig = USB_CSW_SIG;
		pCSW->dCSWDataRes = 0;
		pCSW->bCSWStatus = 0;
		pdev->bo_xfer_addr = (uintptr_t) pCSW;
		pdev->bo_xfer_residue = USB_CSW_LEN;
		pdev->bo_xfer_tranferred = 0;
		fret = epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
		if (fret == USB_RETVAL_NOTCOMP)
		{
			pdev->bo_state = USB_BO_CSW;
		}
		else
		{
			pdev->bo_state = USB_BO_CBW;
			PRINTF("Error: CSW Send Error!!\n");
		}
		break;
	case 0x2f: //Verify(10)
		pCSW->dCSWSig = USB_CSW_SIG;
		pCSW->dCSWDataRes = 0;
		pCSW->bCSWStatus = 0;
		pdev->bo_xfer_addr = (uintptr_t) pCSW;
		pdev->bo_xfer_residue = USB_CSW_LEN;
		pdev->bo_xfer_tranferred = 0;
		fret = epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
		if (fret == USB_RETVAL_NOTCOMP)
		{
			pdev->bo_state = USB_BO_CSW;
		}
		else
		{
			pdev->bo_state = USB_BO_CBW;
			PRINTF("Error: CSW Send Error!!\n");
		}
		break;
	case 0x12: //Inquiry
		pdev->bo_xfer_addr = (uintptr_t) InquiryData;
		pdev->bo_xfer_residue = min(pCBW->dCBWDTL, 36);
		pdev->bo_xfer_tranferred = 0;
		fret = epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
		if (fret == USB_RETVAL_NOTCOMP)
		{
			pdev->bo_state = USB_BO_TXDATA;
		}
		else
		{
			pdev->bo_state = USB_BO_CBW;
			PRINTF("Error: Data Send Error!!\n");
		}
		break;
	case 0x23: //Read Format Capacity
	{
		uint8_t formatcap [12];
		uint32_t sec_cnt [2];
		formatcap [0] = 0x00;
		formatcap [1] = 0x00;
		formatcap [2] = 0x00;
		formatcap [3] = 0x08;
		formatcap [8] = 0x02;
		formatcap [9] = 0x00;
		formatcap [10] = 0x02;
		formatcap [11] = 0x00;
		//sec_cnt[0] = (pdev->bo_seccnt+1);
		sec_cnt [0] = wBoot_part_capacity(pCBW->bCBWLUN);
		//PRINTF("part index = %d, format capacity = %d\n", pCBW->bCBWLUN, sec_cnt[0]);
		formatcap [4] = sec_cnt [0] >> 24;
		formatcap [5] = sec_cnt [0] >> 16;
		formatcap [6] = sec_cnt [0] >> 8;
		formatcap [7] = sec_cnt [0];
		pdev->bo_xfer_addr = (uintptr_t) formatcap;
		pdev->bo_xfer_residue = min(pCBW->dCBWDTL, 12);
		pdev->bo_xfer_tranferred = 0;
		fret = epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
		if (fret == USB_RETVAL_NOTCOMP)
		{
			pdev->bo_state = USB_BO_TXDATA;
		}
		else
		{
			pdev->bo_state = USB_BO_CBW;
			PRINTF("Error: Data Send Error!!\n");
		}
	}
		break;
	case 0x25: //Read Capacity
	{
		uint8_t capacity [8];
		uint32_t sec_cnt [2];
		capacity [4] = 0x00;
		capacity [5] = 0x00;
		capacity [6] = 0x02;
		capacity [7] = 0x00;
		//sec_cnt[0] = (pdev->bo_seccnt);
		//sec_cnt[0] = (32768); //16Mbyte vdisk space
		sec_cnt [0] = wBoot_part_capacity(pCBW->bCBWLUN) - 1;
		//PRINTF("part index = %d, capacity = %d\n", pCBW->bCBWLUN, sec_cnt[0]);
		capacity [0] = sec_cnt [0] >> 24;
		capacity [1] = sec_cnt [0] >> 16;
		capacity [2] = sec_cnt [0] >> 8;
		capacity [3] = sec_cnt [0];
		pdev->bo_xfer_addr = (uintptr_t) capacity;
		pdev->bo_xfer_residue = min(pCBW->dCBWDTL, 8);
		pdev->bo_xfer_tranferred = 0;
		fret = epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
		if (fret == USB_RETVAL_NOTCOMP)
		{
			pdev->bo_state = USB_BO_TXDATA;
		}
		else
		{
			pdev->bo_state = USB_BO_CBW;
			PRINTF("Error: Data Send Error!!\n");
		}
	}
		break;
	case 0x28: //Read(10)  write to host
	{
		uint32_t read_len;
		uint32_t read_offset;
		int32_t part_start;

		read_len = USBD_peek_u16_BE(& pCBW->CBWCB [7]);
		read_offset = USBD_peek_u32_BE(& pCBW->CBWCB [2]);

		pdev->bo_xfer_addr = pdev->bo_memory_base;
		//����flash����
		part_start = wBoot_part_start(pCBW->bCBWLUN);
		//PRINTF("part index = %d, start = %d\n", pCBW->bCBWLUN, part_start);
		if (wBoot_block_read(read_offset + part_start, read_len, (void*) pdev->bo_memory_base) < 0)
		{
			PRINTF("part index = %d, start = %d, read_start = %d, len = %d\n", pCBW->bCBWLUN, part_start, read_offset + part_start, read_len);

			pCSW->dCSWSig = USB_CSW_SIG;
			pCSW->dCSWDataRes = pCBW->dCBWDTL;
			pCSW->bCSWStatus = 1;
			pdev->bo_xfer_addr = (uintptr_t) pCSW;
			pdev->bo_xfer_residue = USB_CSW_LEN;
			pdev->bo_xfer_tranferred = 0;
			epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
			pdev->bo_state = USB_BO_CSW;

			PRINTF("Error: Flash Read Fail\n");
			break;
		}
		read_len <<= USB_DEV_SEC_BITS; //���� read_len ���ֽ���
		read_offset <<= USB_DEV_SEC_BITS; //From Blocks to Bytes  //���� read_offset ��ƫ�Ƶ��ֽ���
		pdev->bo_xfer_residue = min(read_len, MAX_DDMA_SIZE); //Max USB Packet is 64KB    //??
		pdev->bo_xfer_tranferred = 0;
	}
		fret = epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
		if (fret == USB_RETVAL_NOTCOMP)
		{
			pdev->bo_state = USB_BO_TXDATA;
		}
		else
		{
			pdev->bo_state = USB_BO_CBW;
			PRINTF("Error: Data Send Error!!\n");
		}
		break;
	case 0x1A: //Mode Sense(6)
		pdev->bo_xfer_addr = (uintptr_t) SenseData;
		pdev->bo_xfer_residue = min(pCBW->dCBWDTL, 4);
		pdev->bo_xfer_tranferred = 0;
		fret = epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
		if (fret == USB_RETVAL_NOTCOMP)
		{
			pdev->bo_state = USB_BO_TXDATA;
		}
		else
		{
			pdev->bo_state = USB_BO_CBW;
			PRINTF("Error: Data Send Error!!\n");
		}
		break;
	case 0x2A: //Write(10)   read from host
	{
		write_len = USBD_peek_u16_BE(& pCBW->CBWCB [7]);
		write_offset = USBD_peek_u32_BE(& pCBW->CBWCB [2]);

		write_len <<= USB_DEV_SEC_BITS; //���� write_len ���ֽ���
		write_offset <<= USB_DEV_SEC_BITS; //���� write_offset ��ƫ�Ƶ��ֽ���

		pdev->bo_xfer_addr = pdev->bo_memory_base;
		pdev->bo_xfer_residue = min(write_len, MAX_DDMA_SIZE);
		pdev->bo_xfer_tranferred = 0;
	}
		fret = epx_out_handler_dev(pusb, bo_ep_out, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
		if (fret == USB_RETVAL_COMPOK)
		{
			int32_t flash_ret, start, nsector;
			//write to flash
			start = (write_offset >> 9) + wBoot_part_start(pCBW->bCBWLUN);
			nsector = write_len >> USB_DEV_SEC_BITS;
			flash_ret = wBoot_block_write(start, nsector, (void*) pdev->bo_memory_base);
			PRINTF("part index = %d, start = %d\n", pCBW->bCBWLUN, start);
			if (flash_ret < 0)
			{
				PRINTF("flash write start %d sector %d failed\n", start, nsector);
				pCSW->dCSWSig = USB_CSW_SIG;
				pCSW->dCSWDataRes = pCBW->dCBWDTL;
				pCSW->bCSWStatus = 1;
				pdev->bo_xfer_addr = (uintptr_t) pCSW;
				pdev->bo_xfer_residue = USB_CSW_LEN;
				pdev->bo_xfer_tranferred = 0;
				epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
				pdev->bo_state = USB_BO_CSW;
				PRINTF("Error: Flash Write Fail\n");
				break;
			}
			pdev->bo_xfer_tranferred = pdev->bo_xfer_residue;
			pdev->bo_xfer_residue = 0;
			pCSW->dCSWSig = USB_CSW_SIG;
			pCSW->dCSWDataRes = pCBW->dCBWDTL - pdev->bo_xfer_tranferred;
			pCSW->bCSWStatus = 0;
			pdev->bo_xfer_addr = (uintptr_t) pCSW;
			pdev->bo_xfer_residue = USB_CSW_LEN;
			pdev->bo_xfer_tranferred = 0;
			fret = epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
			if (fret == USB_RETVAL_NOTCOMP)
			{
				pdev->bo_state = USB_BO_CSW;
			}
			else
			{
				pdev->bo_state = USB_BO_CBW;
				PRINTF("Error: CSW Send Error!!\n");
			}
		}
		else
		{
			if (fret == USB_RETVAL_NOTCOMP)
			{
				pdev->bo_state = USB_BO_RXDATA;
			}
			else
			{
				pdev->bo_state = USB_BO_CBW;
				PRINTF("Error: Rx Data Error!!\n");
			}
		}
		break;
	default:
		break;
	}
	return fret;
}

static USB_RETVAL usb_dev_bulk_xfer_msc(pusb_struct pusb)
{
	usb_device_msc * const pdev = & pusb->device_msc;
	uint32_t rx_count=0;
	const uint32_t bo_ep_in = (USBD_EP_MSC_IN & 0x0F);
	const uint32_t bo_ep_out = (USBD_EP_MSC_OUT & 0x0F);
	const pCBWPKG pCBW = (pCBWPKG)(pusb->buffer);
	const pCSWPKG pCSW = (pCSWPKG)(pusb->buffer);
	USB_RETVAL ret = USB_RETVAL_NOTCOMP;
	const uint32_t ep_save = usb_get_active_ep(pusb);
	USB_RETVAL fret = USB_RETVAL_NOTCOMP;

	switch (pdev->bo_state)
	{
		case USB_BO_IDLE:
		case USB_BO_CBW:
		 	if (!pusb->eprx_flag[bo_ep_out-1])
  		 	{
  				break;
  		 	}

			pusb->eprx_flag[bo_ep_out-1]--;
	  		usb_select_ep(pusb, bo_ep_out);
	  		if (!(usb_get_eprx_csr(pusb) & USB_RXCSR_RXPKTRDY))
	  		{
	  			break;
	  		}
	  		rx_count = usb_get_eprx_count(pusb);
			if (rx_count != USB_CBW_LEN)
	  		{
	  			usb_eprx_flush_fifo(pusb);
	  			PRINTF("Error: Not CBW, RX Data Length=%d\n",rx_count);
	  			break;
	  		}
	  		do
	  		{
	  			ret = epx_out_handler_dev(pusb, bo_ep_out, (uintptr_t)pusb->buffer, rx_count, USB_PRTCL_BULK);
	  		}
	  		while(ret == USB_RETVAL_NOTCOMP);

	  		if (ret == USB_RETVAL_COMPERR)
	  		{
	  			PRINTF("Error: RX CBW Error\n");
	  			break;
	  		}
	  		else
	  		{
	  			ret = USB_RETVAL_NOTCOMP;
	  		}

	  		if (pCBW->dCBWSig != USB_CBW_SIG)
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
			fret = parse_cbw(pCBW, pCSW, pusb);
			break;

		case USB_BO_RXDATA:
			fret = epx_out_handler_dev(pusb, bo_ep_out, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
			if (fret==USB_RETVAL_COMPOK)
			{
				int32_t flash_ret, start, nsector;
				//write to flash
				start = (write_offset>>9) + wBoot_part_start(pCBW->bCBWLUN);
				nsector = write_len>>USB_DEV_SEC_BITS;
				flash_ret = wBoot_block_write(start, nsector, (void *)pdev->bo_memory_base);
				if (flash_ret < 0)
				{
					PRINTF("flash write start %d sector %d failed\n", start, nsector);
					pCSW->dCSWSig = USB_CSW_SIG;
					pCSW->dCSWDataRes = pCBW->dCBWDTL;
					pCSW->bCSWStatus = 1;
					pdev->bo_xfer_addr = (uintptr_t)pCSW;
					pdev->bo_xfer_residue = USB_CSW_LEN;
					pdev->bo_xfer_tranferred = 0;
					epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
					pdev->bo_state = USB_BO_CSW;

					PRINTF("Error: Flash Write Fail\n");
					break;
				}


				pdev->bo_xfer_tranferred = pdev->bo_xfer_residue;
				pdev->bo_xfer_residue = 0;
				pCSW->dCSWSig = USB_CSW_SIG;
				pCSW->dCSWDataRes = pCBW->dCBWDTL - pdev->bo_xfer_tranferred;
				pCSW->bCSWStatus = 0;
				pdev->bo_xfer_addr = (uintptr_t)pCSW;
				pdev->bo_xfer_residue = USB_CSW_LEN;
				pdev->bo_xfer_tranferred = 0;
				epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
				pdev->bo_state = USB_BO_CSW;
			}
			else if (fret == USB_RETVAL_COMPERR)
			{
				PRINTF("Error: RxData Error\n");
				pdev->bo_state = USB_BO_CBW;
			}
			break;

		case USB_BO_TXDATA:
			fret = epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
			if (fret==USB_RETVAL_COMPOK)
			{
				pdev->bo_xfer_tranferred = pdev->bo_xfer_residue;
				pdev->bo_xfer_residue = 0;
				pCSW->dCSWSig = USB_CSW_SIG;
				pCSW->dCSWDataRes = pCBW->dCBWDTL - pdev->bo_xfer_tranferred;
				pCSW->bCSWStatus = 0;
				pdev->bo_xfer_addr = (uintptr_t)pCSW;
				pdev->bo_xfer_residue = USB_CSW_LEN;
				pdev->bo_xfer_tranferred = 0;
				epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
				pdev->bo_state = USB_BO_CSW;
			}
			else if (fret == USB_RETVAL_COMPERR)
			{
				PRINTF("Error: TxData Error\n");
				pdev->bo_state = USB_BO_CBW;
			}
			break;

		case USB_BO_CSW:
			fret = epx_in_handler_dev(pusb, bo_ep_in, pdev->bo_xfer_addr, pdev->bo_xfer_residue, USB_PRTCL_BULK);
			if (fret==USB_RETVAL_COMPOK)
			{
				pdev->bo_xfer_tranferred = pdev->bo_xfer_residue;
				pdev->bo_xfer_residue = 0; //min(pCBW->dCBWDTL, 36);
				pdev->bo_state = USB_BO_CBW;
			}
			else if (fret==USB_RETVAL_COMPERR)
			{
				pdev->bo_state = USB_BO_CBW;
			  	PRINTF("Error: Tx CSW Error!!\n");
			}
			break;

		default:
			break;
	}
	usb_select_ep(pusb, ep_save);

	return ret;
}

static void usb_struct_idle_msc(pusb_struct pusb)
{
	usb_device_msc * const pdev = & pusb->device_msc;
	pdev->bo_state = USB_BO_IDLE;
}

static void usb_dev_bulk_xfer_msc_initialize(pusb_struct pusb)
{
	static uint8_t ALIGNX_BEGIN device_bo_memory_base [128 * 1024] ALIGNX_END;
	static uint8_t ALIGNX_BEGIN device_bo_bufbase [USB_BO_DEV_BUF_SIZE] ALIGNX_END;
	usb_device_msc * const pdev = & pusb->device_msc;

	pdev->MaxLUNv [0] = wBoot_part_count(1) - 1;
	PRINTF("part count = %d\n", pdev->MaxLUNv [0] + 1);
	pdev->bo_memory_base = (uintptr_t) device_bo_memory_base;//(uint32_t)wBoot_malloc(128 * 1024);		//use to storage user data
	pdev->bo_bufbase = (uintptr_t) device_bo_bufbase;//(uint32_t)wBoot_malloc(64 * 1024);				//use to usb ping-pang buffer
}

#endif /* WITHUSBDMSC */

#if WITHUSBCDCACM && WITHWAWXXUSB



// Control signal bitmap values for the SetControlLineState request
// (usbcdc11.pdf, 6.2.14, Table 51)
#define CDC_DTE_PRESENT                         (1 << 0)
#define CDC_ACTIVATE_CARRIER                    (1 << 1)

// Состояние - выбранные альтернативные конфигурации по каждому интерфейсу USB configuration descriptor
//static uint8_t altinterfaces [INTERFACE_count];

static volatile uint16_t usb_cdc_control_state [INTERFACE_count];

static volatile uint8_t usbd_cdcX_rxenabled [WITHUSBCDCACM_N];	/* виртуальный флаг разрешения прерывания по приёму символа - HARDWARE_CDC_ONRXCHAR */
static __ALIGN_BEGIN uint8_t cdcXbuffout [WITHUSBCDCACM_N] [VIRTUAL_COM_PORT_OUT_DATA_SIZE] __ALIGN_END;
static __ALIGN_BEGIN uint8_t cdcXbuffin [WITHUSBCDCACM_N] [VIRTUAL_COM_PORT_IN_DATA_SIZE] __ALIGN_END;
static uint16_t cdcXbuffinlevel [WITHUSBCDCACM_N];

static __ALIGN_BEGIN uint8_t cdc_epXdatabuffout [USB_OTG_MAX_EP0_SIZE] __ALIGN_END;

static uint32_t dwDTERate [INTERFACE_count];

#define MAIN_CDC_OFFSET 0
#if WITHUSBCDCACM_N > 1
	#define SECOND_CDC_OFFSET 1
#endif /* WITHUSBCDCACM_N > 1 */

static SPINLOCK_t catlock = SPINLOCK_INIT;

/* управление по DTR происходит сразу, RTS только вместе со следующим DTR */
/* хранимое значение после получения CDC_SET_CONTROL_LINE_STATE */
/* Биты: RTS = 0x02, DTR = 0x01 */

// Обычно используется для переключения на передачу (PTT)
// вызывается в конексте system interrupt
uint_fast8_t usbd_cdc1_getrts(void)
{
	const unsigned offset = MAIN_CDC_OFFSET;
	SPIN_LOCK(& catlock);
	const uint_fast8_t state =
		((usb_cdc_control_state [USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL, offset)] & CDC_ACTIVATE_CARRIER) != 0) ||
		0;
	SPIN_UNLOCK(& catlock);
	return state;
}

// Обычно используется для телеграфной манипуляции (KEYDOWN)
// вызывается в конексте system interrupt
uint_fast8_t usbd_cdc1_getdtr(void)
{
	const unsigned offset = MAIN_CDC_OFFSET;
	SPIN_LOCK(& catlock);
	const uint_fast8_t state =
		((usb_cdc_control_state [USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL, offset)] & CDC_DTE_PRESENT) != 0) ||
		0;
	SPIN_UNLOCK(& catlock);
	return state;
}

// Обычно используется для переключения на передачу (PTT)
// вызывается в конексте system interrupt
uint_fast8_t usbd_cdc2_getrts(void)
{
#if WITHUSBCDCACM_N > 1
	const unsigned offset = SECOND_CDC_OFFSET;
	SPIN_LOCK(& catlock);
	const uint_fast8_t state =
		((usb_cdc_control_state [USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL, offset)] & CDC_ACTIVATE_CARRIER) != 0) ||
		0;
	SPIN_UNLOCK(& catlock);
	return state;
#else /* WITHUSBCDCACM_N > 1 */
	return 0;
#endif /* WITHUSBCDCACM_N > 1 */
}

// Обычно используется для телеграфной манипуляции (KEYDOWN)
// вызывается в конексте system interrupt
uint_fast8_t usbd_cdc2_getdtr(void)
{
#if WITHUSBCDCACM_N > 1
	const unsigned offset = SECOND_CDC_OFFSET;
	SPIN_LOCK(& catlock);
	const uint_fast8_t state =
		((usb_cdc_control_state [USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL, offset)] & CDC_DTE_PRESENT) != 0) ||
		0;
	SPIN_UNLOCK(& catlock);
	return state;
#else /* WITHUSBCDCACM_N > 1 */
	return 0;
#endif /* WITHUSBCDCACM_N > 1 */
}

static volatile uint8_t usbd_cdc_txenabled [WITHUSBCDCACM_N];	/* виртуальный флаг разрешения прерывания по готовности передатчика - HARDWARE_CDC_ONTXCHAR*/
static volatile uint8_t usbd_cdc_zlp_pending [WITHUSBCDCACM_N];
static uint32_t usbd_cdc_txlen [WITHUSBCDCACM_N];	/* количество данных в буфере */

/* временное решение для передачи (вызывается при запрещённых прерываниях). */
void usbd_cdc_send(const void * buff, size_t length)
{
	const unsigned offset = MAIN_CDC_OFFSET;
	if (gpusb != NULL)
	{
		usb_struct * const pusb = gpusb;
		const uint32_t bo_ep_in = (USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, offset) & 0x0F);
		if (pusb->eptx_ret[bo_ep_in-1] != USB_RETVAL_COMPOK)
			return;
		USB_RETVAL ret = USB_RETVAL_NOTCOMP;
		const size_t n = ulmin(length, VIRTUAL_COM_PORT_IN_DATA_SIZE);
		memcpy(cdcXbuffin [offset], buff, n);
		usbd_cdc_zlp_pending [offset] = n == VIRTUAL_COM_PORT_IN_DATA_SIZE;
		usbd_cdc_txlen [offset] = n;
		//printhex(0, cdcXbuffin [offset], usbd_cdc_txlen [offset]);
		pusb->eptx_ret[bo_ep_in-1] = epx_in_handler_dev(pusb, bo_ep_in, (uintptr_t) cdcXbuffin [offset], usbd_cdc_txlen [offset], USB_PRTCL_BULK);
// 		do
//  		{
//  			ret = epx_in_handler_dev(pusb, bo_ep_in, (uintptr_t) cdcXbuffin [offset], usbd_cdc_txlen [offset], USB_PRTCL_BULK);
//  		}
//  		while(ret == USB_RETVAL_NOTCOMP);
	}
}

uint_fast8_t usbd_cdc_ready(void)	/* временное решение для передачи */
{
	const unsigned offset = MAIN_CDC_OFFSET;
	if (gpusb != NULL)
	{
		usb_struct * const pusb = gpusb;
		const uint32_t bo_ep_in = (USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, offset) & 0x0F);
		if (pusb->eptx_ret[bo_ep_in-1] != USB_RETVAL_COMPOK)
			return 0;
		return 1;
	}
	return 0;
}

/* Разрешение/запрещение прерывания по передаче символа */
void usbd_cdc_enabletx(uint_fast8_t state)	/* вызывается из обработчика прерываний */
{
	const unsigned offset = MAIN_CDC_OFFSET;
	SPIN_LOCK(& catlock);
	usbd_cdc_txenabled [offset] = state;
	SPIN_UNLOCK(& catlock);
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void usbd_cdc_enablerx(uint_fast8_t state)	/* вызывается из обработчика прерываний */
{
	const unsigned offset = MAIN_CDC_OFFSET;
	SPIN_LOCK(& catlock);
	usbd_cdcX_rxenabled [offset] = state;
	SPIN_UNLOCK(& catlock);
}

/* передача символа после прерывания о готовности передатчика - вызывается из HARDWARE_CDC_ONTXCHAR */
void
usbd_cdc_tx(void * ctx, uint_fast8_t c)
{
	const unsigned offset = MAIN_CDC_OFFSET;
	//USBD_HandleTypeDef * const pdev = ctx;
	(void) ctx;
	SPIN_LOCK(& catlock);
	ASSERT(cdcXbuffinlevel  [offset] < VIRTUAL_COM_PORT_IN_DATA_SIZE);
	cdcXbuffin [offset] [cdcXbuffinlevel [offset] ++] = c;
	SPIN_UNLOCK(& catlock);
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
		HARDWARE_CDC_ONRXCHAR(offset, data [i]);
	}
}

static void usb_dev_bulk_xfer_cdc(pusb_struct pusb, unsigned offset)
{
	const uint32_t ep_save = usb_get_active_ep(pusb);
	const uint32_t bo_ep_in = (USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, offset) & 0x0F);
	const uint32_t bo_ep_out = (USBD_CDCACM_OUT_EP(USBD_EP_CDCACM_OUT, offset) & 0x0F);
	uint32_t rx_count=0;

	do
	{
		USB_RETVAL ret = USB_RETVAL_NOTCOMP;
		// Handle OUT pipe (from host to device)
	 	if (!pusb->eprx_flag[bo_ep_out-1])
		{
			break;
		}
		pusb->eprx_flag[bo_ep_out-1]--;

  		usb_select_ep(pusb, bo_ep_out);
  		if (!(usb_get_eprx_csr(pusb) & USB_RXCSR_RXPKTRDY))
  		{
  			break;
  		}
  		rx_count = usb_get_eprx_count(pusb);
  		do
  		{
  			ret = epx_out_handler_dev(pusb, bo_ep_out, (uintptr_t)cdcXbuffout [offset], rx_count, USB_PRTCL_BULK);
  		}
  		while(ret == USB_RETVAL_NOTCOMP);

  		if (ret == USB_RETVAL_COMPERR)
  		{
  			PRINTF("Error: RX CDC Error\n");
  			break;
  		}
  		else
  		{
  			ret = USB_RETVAL_NOTCOMP;
  			// использование данных
  			//printhex(0, cdcXbuffout [offset], rx_count);
  			cdcXout_buffer_save(cdcXbuffout [offset], rx_count, 0);
   		}
	} while (0);

	do
	{
		switch (pusb->eptx_ret[bo_ep_in-1])
		{
		case USB_RETVAL_NOTCOMP:
			pusb->eptx_ret[bo_ep_in-1] = epx_in_handler_dev(pusb, bo_ep_in, 0, 0, USB_PRTCL_BULK);
			break;
		case USB_RETVAL_COMPERR:
			pusb->eptx_ret[bo_ep_in-1] = USB_RETVAL_COMPOK;
			break;
		case USB_RETVAL_COMPOK:
			break;
		}
//		// Handle IN pipe (from device to host)
//	 	if (!pusb->eptx_flag[bo_ep_in-1])
//		{
//			break;
//		}
//	 	pusb->eptx_flag[bo_ep_in-1]--;
	 	//TP();

	} while (0);

	usb_select_ep(pusb, ep_save);
}


static void usb_struct_idle_cdc(pusb_struct pusb)
{
//	pdev->bo_state = USB_BO_IDLE;
}

static void usb_dev_bulk_xfer_cdc_initialize(pusb_struct pusb)
{
}

#endif /* WITHUSBCDCACM */

#if WITHUSBUAC && WITHWAWXXUSB

/* Audio output */
static uintptr_t uacinaddr = 0;
static uint_fast16_t uacinsize = 0;

#if WITHUSBUACIN2
/* RTS output */
static uintptr_t uacinrtsaddr = 0;
static uint_fast16_t uacinrtssize = 0;
#endif /* WITHUSBUACIN2 */

static void usb_dev_iso_xfer_uac(PCD_HandleTypeDef *hpcd)
{
	usb_struct * const pusb = & hpcd->awxx_usb;
	const uint32_t ep_save = usb_get_active_ep(pusb);

	do
	{
		USB_RETVAL ret = USB_RETVAL_NOTCOMP;
		uint32_t rx_count=0;
		static uint8_t uacoutbuff [UACOUT_AUDIO48_DATASIZE];
		const uint32_t bo_ep_out = (USBD_EP_AUDIO_OUT & 0x0F);

		// Handle OUT pipe (from host to device)
	 	if (!pusb->eprx_flag[bo_ep_out-1])
		{
			break;
		}
		pusb->eprx_flag[bo_ep_out-1]--;

  		usb_select_ep(pusb, bo_ep_out);
  		if (!(usb_get_eprx_csr(pusb) & USB_RXCSR_RXPKTRDY))
  		{
  			break;
  		}
  		rx_count = usb_get_eprx_count(pusb);
  		ASSERT(UACOUT_AUDIO48_DATASIZE == rx_count || 0 == rx_count);
  		do
  		{
  			ret = epx_out_handler_dev(pusb, bo_ep_out, (uintptr_t)uacoutbuff, rx_count, USB_PRTCL_ISO);
  		}
  		while(ret == USB_RETVAL_NOTCOMP);

  		if (ret == USB_RETVAL_COMPERR)
  		{
  			PRINTF("Error: RX UAC Error\n");
  			break;
  		}
  		else
  		{
  			ret = USB_RETVAL_NOTCOMP;
  		}
  		{
			// использование данных
			//printhex(0, uacoutbuff, rx_count);
			uacout_buffer_save_system(uacoutbuff, rx_count, UACOUT_AUDIO48_FMT_CHANNELS, UACOUT_AUDIO48_SAMPLEBITS);
 		}
	} while (0);

	usb_select_ep(pusb, ep_save);

#if WITHUSBUACIN
	{

		if (uacinaddr)
		{
			USB_RETVAL ret = USB_RETVAL_NOTCOMP;
			const uint32_t bo_ep_in = USBD_EP_AUDIO_IN & 0x0F;	// ISOC IN Аудиоданные в компьютер из TRX

			ret = epx_in_handler_dev_iso(pusb, bo_ep_in, uacinaddr, uacinsize, USB_PRTCL_ISO);
			if (ret == USB_RETVAL_COMPOK)
			{
				global_disableIRQ();
				release_dmabufferx(uacinaddr);
				global_enableIRQ();
				uacinaddr = 0;
			}
		}
	}

#if WITHUSBUACIN2
	{
		if (uacinrtsaddr)
		{
			USB_RETVAL ret = USB_RETVAL_NOTCOMP;
			const uint32_t bo_ep_rts_in = USBD_EP_RTS_IN & 0x0F;	// ISOC IN Аудиоданные в компьютер из TRX

			ret = epx_in_handler_dev_iso(pusb, bo_ep_rts_in, uacinrtsaddr, uacinrtssize, USB_PRTCL_ISO);
			if (ret == USB_RETVAL_COMPOK)
			{
				global_disableIRQ();
				release_dmabufferxrts(uacinrtsaddr);
				global_enableIRQ();
				uacinrtsaddr = 0;
			}
		}
	}
#endif /* WITHUSBUACIN2 */
#endif /* WITHUSBUACIN */
}


static void usb_struct_idle_uac(pusb_struct pusb)
{
//	pdev->bo_state = USB_BO_IDLE;
}

static void usb_dev_iso_xfer_uac_initialize(pusb_struct pusb)
{
}

#endif /* WITHUSBUAC */


static void usb_struct_idle(pusb_struct pusb)
{
	//Reset Function Address
	usb_set_dev_addr(pusb, 0x00);

#if WITHUSBDMSC && WITHWAWXXUSB
	usb_struct_idle_msc(pusb);
#endif /* WITHUSBDMSC */
#if WITHUSBCDCACM && WITHWAWXXUSB
	usb_struct_idle_cdc(pusb);
#endif /* WITHUSBCDCACM */
#if WITHUSBUAC && WITHWAWXXUSB
	usb_struct_idle_uac(pusb);
#endif /* WITHUSBUAC */
}

static void set_ep_iso(pusb_struct pusb, uint32_t ep_no, uint32_t ep_dir)
{
	usb_select_ep(pusb, ep_no);
	if (ep_dir)
	{
		// IN
		usb_set_eptx_csr(pusb, usb_get_eptx_csr(pusb) | USB_TXCSR_ISO);
	}
	else
	{
		// OUT
		usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb) | USB_RXCSR_ISO);
	}
}

static uint32_t set_fifo_ep(pusb_struct pusb, uint32_t ep_no, uint32_t ep_dir, uint32_t maxpktsz, uint32_t is_dpb, uint32_t fifo_addr)
{
	const uint32_t alignedepfifosize = ((maxpktsz + 64 + (USB_FIFO_ADDR_BLOCK - 1)) & (~ (USB_FIFO_ADDR_BLOCK - 1)));  //Align to USB_FIFO_ADDR_BLOCK
	//const uint32_t is_dpb = 1;	// double buffer
	const uint32_t maxpayload = maxpktsz;
	const uint32_t pktcnt = 1;//USB_EP_FIFO_SIZE + (maxpktsz - 1) / maxpktsz;

	//PRINTF("set_fifo_ep: ep_no=%02X, ep_dir=%d, pktcnt=%u, maxpktsz=%u\n", ep_no, ep_dir, pktcnt, maxpktsz);
	//ASSERT(USB_EP_FIFO_SIZE >= maxpktsz);
	usb_select_ep(pusb, ep_no);
	if (ep_dir)
	{
		// IN
		usb_set_eptx_maxpkt(pusb, maxpayload, pktcnt);
		usb_set_eptx_fifo_addr(pusb, fifo_addr);
		usb_set_eptx_fifo_size(pusb, is_dpb, alignedepfifosize);
		usb_eptx_flush_fifo(pusb);
		usb_eptx_flush_fifo(pusb);
	}
	else
	{
		// OUT
		usb_set_eprx_maxpkt(pusb, maxpayload, pktcnt);
		usb_set_eprx_fifo_addr(pusb, fifo_addr);
		usb_set_eprx_fifo_size(pusb, is_dpb, alignedepfifosize);
		usb_eprx_flush_fifo(pusb);
		usb_eprx_flush_fifo(pusb);
	}
	/* двойной размер после округления - возможно double buffer */
	fifo_addr += alignedepfifosize * (is_dpb ? 2 : 1);
	return fifo_addr;
}

static void awxx_setup_fifo(pusb_struct pusb)
{
	const uint32_t fifo_base = 1024;
	uint32_t fifo_addr = fifo_base;
	const uint32_t ep_dir_in = 1;
	const uint32_t ep_dir_out = 0;

#if WITHUSBDMTP
	{
		//set_fifo_ep: ep_no=02, ep_attr=02, ep_dir=0, bIntfProtocol=50, maxpktsz=512
		//set_fifo_ep: ep_no=01, ep_attr=02, ep_dir=1, bIntfProtocol=50, maxpktsz=512
	#if WITHUSBDEV_HSDESC
		fifo_addr = set_fifo_ep(pusb, (USBD_EP_MTP_IN & 0x0F), ep_dir_in, MTP_DATA_MAX_PACKET_SIZE, 1, fifo_addr);
		fifo_addr = set_fifo_ep(pusb, (USBD_EP_MTP_OUT & 0x0F), ep_dir_out, MTP_DATA_MAX_PACKET_SIZE, 1, fifo_addr);
	#else /* WITHUSBDEV_HSDESC */
		fifo_addr = set_fifo_ep(pusb, (USBD_EP_MTP_IN & 0x0F), ep_dir_in, MTP_DATA_MAX_PACKET_SIZE, 1, fifo_addr);
		fifo_addr = set_fifo_ep(pusb, (USBD_EP_MTP_OUT & 0x0F), ep_dir_out, MTP_DATA_MAX_PACKET_SIZE, 1, fifo_addr);
	#endif /* WITHUSBDEV_HSDESC */
		  /* Open INTR EP IN */
		fifo_addr = set_fifo_ep(pusb, (USBD_EP_MTP_INT & 0x0F), ep_dir_in, MTP_CMD_PACKET_SIZE, 0, fifo_addr);
	}
#endif /* WITHUSBDMTP */

#if WITHUSBDMSC
	{
		//set_fifo_ep: ep_no=02, ep_attr=02, ep_dir=0, bIntfProtocol=50, maxpktsz=512
		//set_fifo_ep: ep_no=01, ep_attr=02, ep_dir=1, bIntfProtocol=50, maxpktsz=512
	#if WITHUSBDEV_HSDESC
		fifo_addr = set_fifo_ep(pusb, (USBD_EP_MSC_IN & 0x0F), ep_dir_in, MSC_DATA_MAX_PACKET_SIZE_HS, 1, fifo_addr);
		fifo_addr = set_fifo_ep(pusb, (USBD_EP_MSC_OUT & 0x0F), ep_dir_out, MSC_DATA_MAX_PACKET_SIZE_HS, 1, fifo_addr);
	#else /* WITHUSBDEV_HSDESC */
		fifo_addr = set_fifo_ep(pusb, (USBD_EP_MSC_IN & 0x0F), ep_dir_in, MSC_DATA_MAX_PACKET_SIZE_FS, 1, fifo_addr);
		fifo_addr = set_fifo_ep(pusb, (USBD_EP_MSC_OUT & 0x0F), ep_dir_out, MSC_DATA_MAX_PACKET_SIZE_FS, 1, fifo_addr);
	#endif /* WITHUSBDEV_HSDESC */
	}
#endif /* WITHUSBDMSC */
#if WITHUSBCDCACM
	{
		unsigned offset;
		for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
		{
			const uint_fast8_t pipeint =  USBD_CDCACM_INT_EP(USBD_EP_CDCACM_INT, offset) & 0x0F;
			const uint_fast8_t pipein = USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, offset) & 0x0F;
			const uint_fast8_t pipeout = USBD_CDCACM_OUT_EP(USBD_EP_CDCACM_OUT, offset) & 0x0F;

			fifo_addr = set_fifo_ep(pusb, pipeint, ep_dir_in, VIRTUAL_COM_PORT_INT_SIZE, 0, fifo_addr);
			fifo_addr = set_fifo_ep(pusb, pipein, ep_dir_in, VIRTUAL_COM_PORT_IN_DATA_SIZE, 1, fifo_addr);
			fifo_addr = set_fifo_ep(pusb, pipeout, ep_dir_out, VIRTUAL_COM_PORT_OUT_DATA_SIZE, 1, fifo_addr);
		}
	}
#endif /* WITHUSBCDCACM */
#if WITHUSBUACOUT
	{
		fifo_addr = set_fifo_ep(pusb, (USBD_EP_AUDIO_OUT & 0x0F), ep_dir_out, usbd_getuacoutmaxpacket(), 1, fifo_addr);	// ISOC OUT Аудиоданные от компьютера в TRX
		set_ep_iso(pusb, (USBD_EP_AUDIO_OUT & 0x0F), ep_dir_out);
	}
#endif /* WITHUSBUACOUT */
#if WITHUSBUACIN
	{
		fifo_addr = set_fifo_ep(pusb, (USBD_EP_AUDIO_IN & 0x0F), ep_dir_in, usbd_getuacinmaxpacket(), 0, fifo_addr);	// ISOC IN Аудиоданные в компьютер из TRX
		set_ep_iso(pusb, (USBD_EP_AUDIO_IN & 0x0F), ep_dir_in);
	#if WITHUSBUACIN2
		fifo_addr = set_fifo_ep(pusb, (USBD_EP_RTS_IN & 0x0F), ep_dir_in, usbd_getuacinrtsmaxpacket(), 0, fifo_addr);	// ISOC IN Аудиоданные в компьютер из TRX
		set_ep_iso(pusb, (USBD_EP_RTS_IN & 0x0F), ep_dir_in);
	#endif
	}
#endif /* WITHUSBUACIN */
	//PRINTF("awxx_setup_fifo: fifo_addr = %u\n", fifo_addr);
	// Device and host controller share a 8K SRAM and a physical PHY
	//ASSERT(fifo_addr < 8192);	/* 8 kB */
}


///////////////////////////////////////////////////////////////////
//                 usb control transfer
///////////////////////////////////////////////////////////////////

#if WITHWAWXXUSB

static uint32_t ep0_set_config_handler_dev(pusb_struct pusb)
{
	return 1;
}

static unsigned gbaudrate = 115200;

// INTERFACE_DFU_CONTROL bRequest codes
enum
{
  DFU_DETACH = 0,
  DFU_DNLOAD ,		// Write to flash
  DFU_UPLOAD,		// Read from flash
  DFU_GETSTATUS,
  DFU_CLRSTATUS,
  DFU_GETSTATE,
  DFU_ABORT
};

static int dev_state = DFU_STATE_IDLE;
static uint8_t dev_status [6];

// После возврата отсюда ставим
//	pusb->ep0_xfer_state = USB_EP0_DATA;
static int32_t ep0_in_handler_dev(pusb_struct pusb)
{
	uint32_t temp = 0;
	pSetupPKG ep0_setup = (pSetupPKG)(pusb->buffer);

	const uint_fast8_t interfacev = LO_BYTE(ep0_setup->wIndex);
	if ((ep0_setup->bmRequest & USB_REQ_TYPE_MASK)==USB_REQ_TYPE_STANDARD)
	{
		const uint_fast8_t index = LO_BYTE(ep0_setup->wValue);
		switch (ep0_setup->bRequest)
		{
			case USB_REQ_GET_STATUS :
			{
    			//PRINTF("usb_device: Get Status, sLength=%04X\n", ep0_setup->wLength);
				static uint8_t ALIGNX_BEGIN buff [64] ALIGNX_END;
				USBD_poke_u16(& buff [0], 0x0001); // D1=remote wakeup D0=self powered
				pusb->ep0_xfer_srcaddr = (uintptr_t) buff;
				pusb->ep0_xfer_residue = min(2, ep0_setup->wLength);
			}
		    	break;
			case USB_REQ_GET_DESCRIPTOR :
				switch (HI_BYTE(ep0_setup->wValue))
				{
					case USB_DESC_TYPE_DEVICE:              //Get Device Desc
						pusb->ep0_maxpktsz = USB_OTG_MAX_EP0_SIZE; // *((uint8_t*)(pusb->device.dev_desc+7));
						pusb->ep0_xfer_srcaddr = (uintptr_t)DeviceDescrTbl [temp].data;
						pusb->ep0_xfer_residue = min(DeviceDescrTbl [temp].size, ep0_setup->wLength);
						break;
					case USB_DESC_TYPE_CONFIGURATION:              //Get Configuration Desc
					   	if (index < USBD_CONFIGCOUNT)
					   	{
							pusb->ep0_xfer_srcaddr = (uintptr_t)ConfigDescrTbl [index].data;
							pusb->ep0_xfer_residue = min(ConfigDescrTbl [index].size, ep0_setup->wLength);
					   	}
						else
						{
						    pusb->ep0_xfer_residue = 0;
							PRINTF("Unknown Configuration Desc!!\n");
						}
						break;
					case USB_DESC_TYPE_STRING:             //Get String Desc
					   	if (index == 0xEE && MsftStringDescr [0].size != 0)
					   	{
							// WCID devices support
							// Microsoft OS String Descriptor, ReqLength=0x12
							// See OS_Desc_Intro.doc, Table 3 describes the OS string descriptor’s fields.
							pusb->ep0_xfer_srcaddr = (uintptr_t)MsftStringDescr [0].data;
							pusb->ep0_xfer_residue = min(MsftStringDescr [0].size, ep0_setup->wLength);
					   	}
					   	else if (index < usbd_get_stringsdesc_count())
						{
							pusb->ep0_xfer_srcaddr = (uintptr_t)StringDescrTbl [index].data;
							pusb->ep0_xfer_residue = min(StringDescrTbl [index].size, ep0_setup->wLength);
						}
						else
						{
						    pusb->ep0_xfer_residue = 0;
							PRINTF("Unknown String Desc!! 0x%02X\n", index);
						}
						break;
					case USB_DESC_TYPE_INTERFACE:           //Get Interface Desc
					    pusb->ep0_xfer_residue = 0;
					    PRINTF("usb_device: Get Interface Descriptor\n");
				    	break;
					case USB_DESC_TYPE_ENDPOINT:          //Get Endpoint Desc
					    pusb->ep0_xfer_residue = 0;
					    PRINTF("usb_device: Get Endpoint Descriptor\n");
				    	break;
					case USB_DESC_TYPE_DEVICE_QUALIFIER:           //Get Device Qualifier
						pusb->ep0_xfer_srcaddr = (uintptr_t)DeviceQualifierTbl [0].data;
						pusb->ep0_xfer_residue = min(DeviceQualifierTbl [0].size, ep0_setup->wLength);
				    	break;
					case USB_DESC_TYPE_OTG:
					    pusb->ep0_xfer_srcaddr = (uintptr_t) OtgDescTbl[0].data;
					    pusb->ep0_xfer_residue = min(OtgDescTbl [0].size, ep0_setup->wLength);
				    	break;
					case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
						pusb->ep0_xfer_srcaddr = (uintptr_t)OtherSpeedConfigurationTbl [0].data;
						pusb->ep0_xfer_residue = min(OtherSpeedConfigurationTbl [0].size, ep0_setup->wLength);
				    	break;
					case USB_DESC_TYPE_BOS:
						if (BinaryDeviceObjectStoreTbl [0].size != 0)
						{
							pusb->ep0_xfer_residue = min(BinaryDeviceObjectStoreTbl [0].size, ep0_setup->wLength);
							pusb->ep0_xfer_srcaddr = (uintptr_t)BinaryDeviceObjectStoreTbl [0].data;
						}
						else
						{
							pusb->ep0_xfer_residue = 0;
						}
						break;
					default  :
					    pusb->ep0_xfer_residue = 0;
					    PRINTF("usb_device: Get Unknown Descriptor 0x%02X\n", HI_BYTE(ep0_setup->wValue));
				}
		      	break;
	     	case USB_REQ_GET_CONFIGURATION :
			{
				static uint8_t ALIGNX_BEGIN buff [64] ALIGNX_END;
				PRINTF("usb_device: Get Configuration\n");
				pusb->ep0_xfer_srcaddr = (uintptr_t) buff;
				pusb->ep0_xfer_residue = 1;
			}
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
	        	PRINTF("usb_device: Unknown Standard Request ifc=%u, bRequest=0x%02X\n", interfacev, ep0_setup->bRequest);
		    	break;
		}
	}
	else if ((ep0_setup->bmRequest & USB_REQ_TYPE_MASK)==USB_REQ_TYPE_CLASS)
	{
		// class
		switch (interfacev)
		{
#if WITHUSBCDCACM
			case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL, 0):
			{
				switch (ep0_setup->bRequest)
				{
					case CDC_SET_LINE_CODING:
					{
						ASSERT(0);
					}
					break;
				case CDC_GET_LINE_CODING:
					{
						// work ok
						static uint8_t ALIGNX_BEGIN buff [64] ALIGNX_END;
						//PRINTF("ep0_in_handler_dev: CDC_GET_LINE_CODING: ifc=%u, %02X\n", interfacev, LO_BYTE(ep0_setup->bRequest));
						USBD_poke_u32(& buff [0], gbaudrate); // dwDTERate
						buff [4] = 0;	// 1 stop bit
						buff [5] = 0;	// parity=none
						buff [6] = 8;	// bDataBits
						pusb->ep0_xfer_srcaddr = (uintptr_t) buff;
						pusb->ep0_xfer_residue =  min(7, ep0_setup->wLength);;
					}
					break;
				case CDC_SET_CONTROL_LINE_STATE:
					{
						ASSERT(0);
					}
					break;
				}
			}
			break;
#endif /* WITHUSBCDCACM */
#if WITHUSBDMSC
		case INTERFACE_MSC_CONTROL:
			{
				switch (ep0_setup->bRequest)
				{
				case 0x00FE :
					pusb->ep0_xfer_srcaddr = (uintptr_t) pusb->device_msc.MaxLUNv;
					pusb->ep0_xfer_residue = 1;
					PRINTF("usb_device: Get MaxLUN, ifc=%u\n", interfacev);
					break;
				}
			}
			break;
#endif /* WITHUSBDMSC */
#if WITHUSBUAC
		case INTERFACE_AUDIO_CONTROL_MIKE:
			{
				static uint8_t ALIGNX_BEGIN buff [64] ALIGNX_END;

				//PRINTF("ep0_in_handler_dev: INTERFACE_AUDIO_CONTROL_MIKE Class-Specific Request ifc=%u, bRequest=0x%02X, wLength=0x%04X\n", interfacev, ep0_setup->bRequest, ep0_setup->wLength);
				pusb->ep0_xfer_srcaddr = (uintptr_t) buff;
				pusb->ep0_xfer_residue = min(ARRAY_SIZE(buff), ep0_setup->wLength);
			}
			break;
#if WITHUSBUACIN2
		case INTERFACE_AUDIO_CONTROL_RTS:
			{
				static uint8_t ALIGNX_BEGIN buff [64] ALIGNX_END;

				//PRINTF("ep0_in_handler_dev: INTERFACE_AUDIO_CONTROL_RTS Class-Specific Request ifc=%u, bRequest=0x%02X, wLength=0x%04X\n", interfacev, ep0_setup->bRequest, ep0_setup->wLength);
				pusb->ep0_xfer_srcaddr = (uintptr_t) buff;
				pusb->ep0_xfer_residue = min(ARRAY_SIZE(buff), ep0_setup->wLength);
			}
			break;
#endif /* WITHUSBUACIN2 */
#endif /* WITHUSBUAC */
#if WITHUSBDFU
			case INTERFACE_DFU_CONTROL:
			{
				PRINTF("ep0_in_handler_dev: INTERFACE_DFU_CONTROL: ifc=%u, req=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n", interfacev, ep0_setup->bRequest, ep0_setup->wValue, ep0_setup->wIndex, ep0_setup->wLength);
				switch (ep0_setup->bRequest)
				{
				case DFU_DETACH:
					TP();
					break;
				case DFU_DNLOAD:
					TP();
					break;
				case DFU_UPLOAD:
					TP();
					break;
				case DFU_GETSTATUS:
					dev_status[0] = 0;
				    dev_status[4] = dev_state;
					pusb->ep0_xfer_srcaddr = (uintptr_t) dev_status;
					pusb->ep0_xfer_residue = min(6, ep0_setup->wLength);
					break;
				case DFU_CLRSTATUS:
					TP();
					break;
				case DFU_GETSTATE:
					TP();
					break;
				case DFU_ABORT:
					TP();
					break;
				default:
					pusb->ep0_xfer_residue = 0;
					PRINTF("ep0_in_handler_dev: INTERFACE_DFU_CONTROL Class-Specific Request ifc=%u, bRequest=0x%02X\n", interfacev, ep0_setup->bRequest);
					break;
				}
			}
			break;
#endif /* WITHUSBDFU */
#if WITHUSBDMTP
			case INTERFACE_MTP_CONTROL:
			{
				/* MTP class requests */
				#define MTP_REQ_CANCEL                                              0x64U
				#define MTP_REQ_GET_EXT_EVENT_DATA                                  0x65U
				#define MTP_REQ_RESET                                               0x66U
				#define MTP_REQ_GET_DEVICE_STATUS                                   0x67U
				/* MTP response code */
				#define MTP_RESPONSE_OK                                             0x2001U
				#define MTP_RESPONSE_GENERAL_ERROR                                  0x2002U
				#define MTP_RESPONSE_PARAMETER_NOT_SUPPORTED                        0x2006U
				#define MTP_RESPONSE_INCOMPLETE_TRANSFER                            0x2007U
				#define MTP_RESPONSE_INVALID_STORAGE_ID                             0x2008U
				#define MTP_RESPONSE_INVALID_OBJECT_HANDLE                          0x2009U
				#define MTP_RESPONSE_DEVICEPROP_NOT_SUPPORTED                       0x200AU
				#define MTP_RESPONSE_STORE_FULL                                     0x200CU
				#define MTP_RESPONSE_ACCESS_DENIED                                  0x200FU
				#define MTP_RESPONSE_STORE_NOT_AVAILABLE                            0x2013U
				#define MTP_RESPONSE_SPECIFICATION_BY_FORMAT_NOT_SUPPORTED          0x2014U
				#define MTP_RESPONSE_NO_VALID_OBJECT_INFO                           0x2015U
				#define MTP_RESPONSE_DEVICE_BUSY                                    0x2019U
				#define MTP_RESPONSE_INVALID_PARENT_OBJECT                          0x201AU
				#define MTP_RESPONSE_INVALID_PARAMETER                              0x201DU
				#define MTP_RESPONSE_SESSION_ALREADY_OPEN                           0x201EU
				#define MTP_RESPONSE_TRANSACTION_CANCELLED                          0x201FU
				#define MTP_RESPONSE_INVALID_OBJECT_PROP_CODE                       0xA801U
				#define MTP_RESPONSE_SPECIFICATION_BY_GROUP_UNSUPPORTED             0xA807U
				#define MTP_RESPONSE_OBJECT_PROP_NOT_SUPPORTED                      0xA80AU

				//static int dev_state = DFU_STATE_IDLE;
				static uint8_t dev_status [4];

				switch (ep0_setup->bRequest)
				{
				case MTP_REQ_RESET:
					pusb->ep0_xfer_residue = min(0, ep0_setup->wLength);
					PRINTF("ep0_in: INTERFACE_MTP_CONTROL MTP_REQ_RESET ifc=%u, bRequest=0x%02X\n", interfacev, ep0_setup->bRequest);
					break;
				case MTP_REQ_GET_DEVICE_STATUS:
					USBD_poke_u32(dev_status, (MTP_RESPONSE_OK << 16) | 4);
					pusb->ep0_xfer_srcaddr = (uintptr_t) dev_status;
					pusb->ep0_xfer_residue = 4;//min(4, ep0_setup->wLength);
					PRINTF("ep0_in: INTERFACE_MTP_CONTROL MTP_REQ_GET_DEVICE_STATUS ifc=%u, bRequest=0x%02X\n", interfacev, ep0_setup->bRequest);
					break;
				default:
					pusb->ep0_xfer_residue = min(0, ep0_setup->wLength);
					PRINTF("ep0_in: INTERFACE_MTP_CONTROL Class-Specific Request ifc=%u, bRequest=0x%02X\n", interfacev, ep0_setup->bRequest);
					break;
				}
			}
			break;
#endif /* WITHUSBDMTP */
		default:
			pusb->ep0_xfer_residue = 0;
			PRINTF("ep0_in: Unknown Class-Specific Request ifc=%u, bRequest=0x%02X\n", interfacev, ep0_setup->bRequest);
			break;
		}
	}
	else if ((ep0_setup->bmRequest & USB_REQ_TYPE_MASK)==USB_REQ_TYPE_VENDOR)
	{
		if (ep0_setup->bRequest == USBD_WCID_VENDOR_CODE && ep0_setup->wIndex == 0x05)
		{
			const uint_fast8_t ifc = LO_BYTE(ep0_setup->wValue);
			//PRINTF(PSTR("MS USBD_StdItfReq: bmRequest=%04X, bRequest=%02X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
			// Extended Properties OS Descriptor
			// See OS_Desc_Ext_Prop.doc, Extended Properties Descriptor Format

			// Extended Properties OS Descriptor support
			if (ifc < ARRAY_SIZE(ExtOsPropDescTbl) && ExtOsPropDescTbl[ifc].size != 0)
			{
				pusb->ep0_xfer_srcaddr = (uintptr_t)ExtOsPropDescTbl [ifc].data;
				pusb->ep0_xfer_residue = min(ExtOsPropDescTbl [ifc].size, ep0_setup->wLength);
			}
			else
			{
				pusb->ep0_xfer_residue = 0;
			}
		}
		else if (ep0_setup->bRequest == USBD_WCID_VENDOR_CODE && ep0_setup->wIndex == 0x04)
		{
			//const uint_fast8_t ifc = LO_BYTE(ep0_setup->wValue);
			//PRINTF("usb_device: WCID Vendor-Specific Request = 0x%02X, wValue=0x%04X, wIndex=0x%04X, wLength=0x%04X\n", ep0_setup->bRequest, ep0_setup->wValue, ep0_setup->wIndex, ep0_setup->wLength);

			pusb->ep0_xfer_srcaddr = (uintptr_t)MsftCompFeatureDescr [0].data;
			pusb->ep0_xfer_residue = min(MsftCompFeatureDescr [0].size, ep0_setup->wLength);
		}
		else
		{
			PRINTF("usb_device: Unknown Vendor-Specific Request = 0x%02X, wValue=0x%04X, wIndex=0x%04X, wLength=0x%04X\n", ep0_setup->bRequest, ep0_setup->wValue, ep0_setup->wIndex, ep0_setup->wLength);
			pusb->ep0_xfer_residue = 0;
		}
	}
	else
	{
		pusb->ep0_xfer_residue = 0;
		PRINTF("usb_device: Unknown EP0 IN!!, 0x%02X\n", ep0_setup->bmRequest & USB_REQ_TYPE_MASK);
	}

	usb_ep0_complete_send(pusb, pusb->ep0_xfer_srcaddr, pusb->ep0_xfer_residue);

	return 0;
}

static int32_t ep0_out_handler_dev(pusb_struct pusb)
{
	pSetupPKG ep0_setup = (pSetupPKG)(pusb->buffer);

	const uint_fast8_t interfacev = LO_BYTE(ep0_setup->wIndex);
	switch (ep0_setup->bRequest)
	{
		case 0x01 :
			if (ep0_setup->wIndex&0x80)
			{

			}
			else
			{

			}
		  	break;

		case 0x03 :
			 switch (ep0_setup->wValue)
			 {
			 	case 0x0002:
	          		switch (ep0_setup->wIndex)
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
							usb_write_ep_fifo(pusb, 0, (uintptr_t)TestPkt, 53);
							usb_set_ep0_csr(pusb, 0x02);
							usb_set_test_mode(pusb, 0x08);

	          				PRINTF("usb_device: Send Test Packet Now...\n");
	         				break;
				        	default:
				          	PRINTF("usb_device: Unknown Test Mode: 0x%x\n", ep0_setup->wIndex);
				    }
	        		break;

				case 0x0003:
				case 0x0004:
				case 0x0005:
					PRINTF("usb_device: HNP Enable...\n");
					break;

				default:
				PRINTF("usb_device: Unknown SetFeature Value: 0x%x\n", ep0_setup->wValue);
			}
			break;

		case 0x05 :
			usb_set_dev_addr(pusb, LO_BYTE(ep0_setup->wValue));
       		//PRINTF("usb_device: Set Address 0x%x\n", LO_BYTE(ep0_setup->wValue));
			break;
		case 0x07 :
       		PRINTF("usb_device: Set Descriptor\n");
      		break;
    	case 0x09 :
       		//PRINTF("usb_device: Set Config\n");
     		ep0_set_config_handler_dev(pusb);
    		break;
    	case 0x0B :
    		switch (interfacev)
    		{
#if WITHUSBUACOUT
       		case INTERFACE_AUDIO_MIKE:
    	       	//PRINTF("usb_device: in48 Set Interface ifc=%u, alt=0x%02X\n", interfacev, LO_BYTE(ep0_setup->wValue));
				buffers_set_uacinalt(LO_BYTE(ep0_setup->wValue));
				break;
#endif /* WITHUSBUACOUT */
#if WITHUSBUACIN
      		case INTERFACE_AUDIO_SPK:
    	       	//PRINTF("usb_device: out48 Set Interface ifc=%u, alt=0x%02X\n", interfacev, LO_BYTE(ep0_setup->wValue));
				buffers_set_uacoutalt(LO_BYTE(ep0_setup->wValue));
				break;
#if WITHUSBUACIN2
      		case INTERFACE_AUDIO_RTS:
    	       	//PRINTF("usb_device: rts Set Interface ifc=%u, alt=0x%02X\n", interfacev, LO_BYTE(ep0_setup->wValue));
				buffers_set_uacinrtsalt(LO_BYTE(ep0_setup->wValue));
				break;
#endif /* WITHUSBUACIN2 */
#endif /* WITHUSBUACIN */
      		default:
      			break;
    		}
	      	break;
    	case CDC_SET_LINE_CODING:
    		// work
    		//PRINTF("ep0_out: CDC_SET_LINE_CODING: ifc=%u\n", interfacev);
    		pusb->ep0_xfer_state = USB_EP0_DATA;	// continue read parameters block in ep0_in_handler_dev
			pusb->ep0_xfer_residue = 0;
	      	break;
    	case CDC_SET_CONTROL_LINE_STATE:
    		// work
    		//PRINTF("ep0_out: CDC_SET_CONTROL_LINE_STATE: ifc=%u %02X\n", interfacev, LO_BYTE(ep0_setup->wValue));
       		pusb->ep0_xfer_state = USB_EP0_SETUP;
			pusb->ep0_xfer_residue = 0;
	      	break;
    	default   :
     		PRINTF("usb_device: Unknown EP0 OUT: ifc=%u, 0x%02X, wLength=0x%04X\n", interfacev, ep0_setup->bRequest, ep0_setup->wLength);
			pusb->ep0_xfer_residue = 0;
	      	break;
	}

	return 0;
}


static uint32_t usb_dev_sof_handler(PCD_HandleTypeDef *hpcd)
{
	usb_struct * const pusb = & hpcd->awxx_usb;
#if WITHUSBUACIN
	{
		USB_RETVAL ret = USB_RETVAL_NOTCOMP;
		const uint32_t bo_ep_in = USBD_EP_AUDIO_IN & 0x0F;	// ISOC IN Аудиоданные в компьютер из TRX

		if (uacinaddr == 0)
		{
			global_disableIRQ();
			uacinaddr = getfilled_dmabufferx(& uacinsize);
			global_enableIRQ();
		}


		if (uacinaddr)
		{
			ret = epx_in_handler_dev_iso(pusb, bo_ep_in, uacinaddr, uacinsize, USB_PRTCL_ISO);
			if (ret == USB_RETVAL_COMPOK)
			{
				global_disableIRQ();
				release_dmabufferx(uacinaddr);
				global_enableIRQ();
				uacinaddr = 0;
			}
		}
	}

#if WITHUSBUACIN2
	{
		const uint32_t bo_ep_rts_in = USBD_EP_RTS_IN & 0x0F;	// ISOC IN Аудиоданные в компьютер из TRX

		if (uacinrtsaddr != 0)
		{
			global_disableIRQ();
			release_dmabufferxrts(uacinrtsaddr);
			global_enableIRQ();
		}

		global_disableIRQ();
		uacinrtsaddr = getfilled_dmabufferxrts(& uacinrtssize);
		global_enableIRQ();
		if (uacinrtsaddr)
		{
			USB_RETVAL ret = USB_RETVAL_NOTCOMP;

			ret = epx_in_handler_dev_iso(pusb, bo_ep_rts_in, uacinrtsaddr, uacinrtssize, USB_PRTCL_ISO);
			if (ret == USB_RETVAL_COMPOK)
			{
				global_disableIRQ();
				release_dmabufferxrts(uacinrtsaddr);
				global_enableIRQ();
				uacinrtsaddr = 0;
			}
		}
	}
#endif /* WITHUSBUACIN2 */
#endif /* WITHUSBUACIN */

	return 0;
}

#endif /* WITHWAWXXUSB */

static uint32_t usb_dev_ep0xfer_handler(PCD_HandleTypeDef *hpcd)
{
	usb_struct * const pusb = & hpcd->awxx_usb;
	//uint32_t i=0;
	uint32_t ep0_csr=0;
	//uint32_t src_addr;

	if (pusb->role != USB_ROLE_DEV) return 0;
//	if (!pusb->ep0_flag) return 0;
//	pusb->ep0_flag--;

	usb_select_ep(pusb, 0);
	ep0_csr = usb_get_ep0_csr(pusb);

	if (pusb->ep0_xfer_state == USB_EP0_DATA)  //Control IN Data Stage or Stage Status
	{
		if (ep0_csr & 0x1u)	// ? USB_TXCSR_TXPKTRDY
		{
			pusb->ep0_xfer_state = USB_EP0_SETUP;
		}
		else if (ep0_csr & (0x1u << 4))
		{
			usb_set_ep0_csr(pusb, 0x80);
			PRINTF("usb_dev_ep0xfer_handler: WRN: EP0 Setup End!!\n");
		}
		else if (!(ep0_csr & (0x1u << 1)))	// ? USB_RXCSR_FIFOFULL, USB_TXCSR_FIFONOTEMP
		{
			usb_ep0_complete_send_data(pusb);
		}
		else
		{
			PRINTF("usb_dev_ep0xfer_handler: WRN: Unknown EP0 Interrupt, CSR=0x%x!!\n", ep0_csr);
		}
	}

	if (pusb->ep0_xfer_state == USB_EP0_SETUP)  //Setup or Control OUT Status Stage
	{
#if WITHWAWXXUSB
		pSetupPKG ep0_setup = (pSetupPKG)(pusb->buffer);
#else /* WITHWAWXXUSB */
		pSetupPKG ep0_setup = (pSetupPKG)(hpcd->Setup);
#endif /* WITHWAWXXUSB */

		if (ep0_csr & 0x1)// ? USB_TXCSR_TXPKTRDY
		{
			uint32_t ep0_count = usb_get_ep0_count(pusb);

			if (ep0_count == 8)
			{
				//pusb->ep0_flag = 0;
				usb_read_ep_fifo(pusb, 0, (uintptr_t)ep0_setup, 8);

				if (ep0_setup->bmRequest & 0x80) //in
				{

					usb_set_ep0_csr(pusb, 0x40);

#if WITHWAWXXUSB
					ep0_in_handler_dev(pusb);
#else
					HAL_PCD_SetupStageCallback(hpcd);
#endif

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
				// Not 8 bytes
				const uint_fast8_t interfacev = LO_BYTE(ep0_setup->wIndex);
				if (ep0_setup->bmRequest&0x80)//in
				{
			    	PRINTF("usb_dev_ep0xfer_handler (not 8): ifc=%u, req=%02X, EP0 Rx Error Length = 0x%x\n", interfacev, ep0_setup->bRequest, ep0_count);
					usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb) & USB_RXCSR_ISO); //Clear RxPktRdy - добавил но не уверен в необходимовсти
				  	usb_ep0_flush_fifo(pusb);

				}
				else
				{
					// OUT
					static uint8_t buff [512];
					usb_read_ep_fifo(pusb, 0, (uintptr_t)buff, min(sizeof buff, ep0_count));
					usb_set_eprx_csr(pusb, usb_get_eprx_csr(pusb) & USB_RXCSR_ISO); //Clear RxPktRdy
				  	usb_ep0_flush_fifo(pusb);

				  	// Parse setup packet on output
				  	switch (interfacev)
				  	{
#if WITHUSBDFU && WITHWAWXXUSB
				  	case INTERFACE_DFU_CONTROL:
						PRINTF("usb_dev_ep0xfer_handler: DFU: EP0 OUT (not 8): ifc=%u, req=%02X, wValue=%04X, wIndex=%04X, wLength=%04X, ep0_count=%u\n", interfacev, ep0_setup->bRequest, ep0_setup->wValue, ep0_setup->wIndex, ep0_setup->wLength, ep0_count);
						printhex(0, buff, ep0_count);

						switch (ep0_setup->bRequest)
						{
						case DFU_DETACH:
							TP();
							break;
						case DFU_DNLOAD:
							TP();
							break;
						case DFU_UPLOAD:
							TP();
							break;
						case DFU_GETSTATUS:
							dev_status[0] = 0;
						    dev_status[4] = dev_state;
							pusb->ep0_xfer_srcaddr = (uintptr_t) dev_status;
							pusb->ep0_xfer_residue = min(6, ep0_setup->wLength);
							break;
						case DFU_CLRSTATUS:
							TP();
							break;
						case DFU_GETSTATE:
							TP();
							break;
						case DFU_ABORT:
							TP();
							break;
						default:
							pusb->ep0_xfer_residue = 0;
							PRINTF("usb_dev_ep0xfer_handler: INTERFACE_DFU_CONTROL Class-Specific Request ifc=%u, bRequest=0x%02X\n", interfacev, ep0_setup->bRequest);
							break;
						}
				  		break;
#endif /* WITHUSBDFU */

#if WITHUSBCDCACM && WITHWAWXXUSB
				  	case USBD_CDCACM_IFC(INTERFACE_CDC_CONTROL, 0):
					  	// Parse setup packet on output
					  	switch (ep0_setup->bRequest)
					  	{
					  	case CDC_SET_LINE_CODING:
					  		// work
					  		//PRINTF("usb_dev_ep0xfer_handler: CDC: EP0 OUT (not 8): CDC_SET_LINE_CODING, baudrate=%u\n", USBD_peek_u32(buff));
					  		gbaudrate = USBD_peek_u32(buff);
					  		break;
					  	default:
					  		// work
							PRINTF("usb_dev_ep0xfer_handler: CDC: EP0 OUT (not 8): ifc=%u, req=%02X, wValue=%04X, wIndex=%04X, wLength=%04X, ep0_count=%u\n", interfacev, ep0_setup->bRequest, ep0_setup->wValue, ep0_setup->wIndex, ep0_setup->wLength, ep0_count);
							//printhex(0, buff, ep0_count);
					  		break;
					  	}
				  		break;
#endif /* WITHUSBCDCACM */

#if WITHUSBUACIN && WITHWAWXXUSB
					case INTERFACE_AUDIO_CONTROL_MIKE:
#if WITHUSBUACIN2
					case INTERFACE_AUDIO_CONTROL_RTS:
#endif /* WITHUSBUACIN2 */
					  	// Parse setup packet on output
					  	switch (ep0_setup->bRequest)
					  	{
					  	case AUDIO_REQUEST_SET_CUR:
					  		//PRINTF("AUDIO: EP0 OUT (not 8): AUDIO_REQUEST_SET_CUR: ifc=%u, wValue=%04X, wIndex=%04X, wLength=%04X, ep0_count=%u, v=%u\n", interfacev, ep0_setup->wValue, ep0_setup->wIndex, ep0_setup->wLength, ep0_count, ep0_count == 1 ? buff [0] : USBD_peek_u16(buff));
					  		break;
					  	default:
					  		// work
							PRINTF("AUDIO: EP0 OUT (not 8): ifc=%u, req=%02X, wValue=%04X, wIndex=%04X, wLength=%04X, ep0_count=%u\n", interfacev, ep0_setup->bRequest, ep0_setup->wValue, ep0_setup->wIndex, ep0_setup->wLength, ep0_count);
							//printhex(0, buff, ep0_count);
					  		break;
					  	}
				  		break;
#endif /* WITHUSBUACIN */

					default:
#if WITHWAWXXUSB
						PRINTF("xxx: EP0 OUT (not 8): ifc=%u, req=%02X, wValue=%04X, wIndex=%04X, wLength=%04X, ep0_count=%u\n", interfacev, ep0_setup->bRequest, ep0_setup->wValue, ep0_setup->wIndex, ep0_setup->wLength, ep0_count);
#else
	                    HAL_PCD_SetupStageCallback(hpcd);
#endif
				  		break;

				  	}
					pusb->ep0_xfer_residue = 0;
				}
			}
		}
		else
		{
#if WITHWAWXXUSB
			ep0_out_handler_dev(pusb);
#else
			HAL_PCD_SetupStageCallback(hpcd);
#endif
		}
	}

	return 1;
}

//static void usb_power_polling_dev(pusb_struct pusb)
//{
//	if (pusb->role != USB_ROLE_DEV) return;
//  	if (pusb->connect) return;
//
//  	if (usb_get_vbus_level(pusb) == USB_VBUS_VBUSVLD)
//  	{
//		if (pusb->timer != USB_DEVICE_VBUS_DET_TIMER)
//		{
//			if (pusb->timer != USB_IDLE_TIMER)  //timer should not occupied by any other ones at this time
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
//	PRINTF("USB Connect!!\n");
//
//	return;
//}

static uint32_t usb_device_function(PCD_HandleTypeDef *hpcd)
{
	usb_struct * const pusb = & hpcd->awxx_usb;
	if (pusb->role != USB_ROLE_DEV)
	{
		return 0;
	}

#if WITHUSBDMSC && WITHWAWXXUSB
	usb_dev_bulk_xfer_msc(pusb);
#endif /* WITHUSBDMSC */
#if WITHUSBUAC && WITHWAWXXUSB
	usb_dev_iso_xfer_uac(hpcd);
#endif /* WITHUSBUAC */
#if WITHUSBCDCACM && WITHWAWXXUSB
	unsigned offset;
	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
		usb_dev_bulk_xfer_cdc(pusb, offset);
	}
#endif /* WITHUSBCDCACM */
#if WITHUSBCDCACM && ! WITHWAWXXUSB
	unsigned offset;
	for (offset = 0; offset < WITHUSBCDCACM_N; ++ offset)
	{
		const uint32_t bo_ep_in = (USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, offset) & 0x0F);
		switch (pusb->eptx_ret[bo_ep_in-1])
		{
		case USB_RETVAL_NOTCOMP:
			pusb->eptx_ret[bo_ep_in-1] = epx_in_handler_dev(pusb, bo_ep_in, 0, 0, USB_PRTCL_BULK);
			break;
		case USB_RETVAL_COMPERR:
			pusb->eptx_ret[bo_ep_in-1] = USB_RETVAL_COMPOK;
			break;
		case USB_RETVAL_COMPOK:
			break;
		}
	}
#endif /* WITHUSBCDCACM */

	return 1;
}

//static uint32_t bus_irq_count = 0;
//static uint32_t ep0_irq_count = 0;
//static uint32_t eptx_irq_count = 0;
//static uint32_t eprx_irq_count = 0;

void usb_device_function0(USBD_HandleTypeDef * pdev)
{
	PCD_HandleTypeDef * hpcd = pdev->pData;
	ASSERT(hpcd != NULL);
	system_disableIRQ();
	usb_device_function(hpcd);
	system_enableIRQ();
}

void usbd_pipes_initialize(USBD_HandleTypeDef * pdev)
{
	PCD_HandleTypeDef * hpcd = pdev->pData;
	ASSERT(hpcd != NULL);
	usb_struct * const pusb = & hpcd->awxx_usb;
	awxx_setup_fifo(pusb);
}

static void usb_params_init(PCD_HandleTypeDef *hpcd)
{
	usb_struct * const pusb = & hpcd->awxx_usb;
	uint32_t i;

	//usb_clock_init();

//	pusb->index = 0;
//	pusb->reg_base = USBOTG0_BASE;
//	pusb->irq_no = GIC_SRC_USB0;
//	pusb->drq_no = 0x04;

	pusb->role = USB0_ROLE;  //USB_ROLE_HST; //USB_ROLE_UNK
	pusb->speed = USB0_SPEED;

#if WITHUSBDMSC && WITHWAWXXUSB
	usb_dev_bulk_xfer_msc_initialize(pusb);
#endif /* WITHUSBDMSC */
#if WITHUSBCDCACM && WITHWAWXXUSB
	usb_dev_bulk_xfer_cdc_initialize(pusb);
#endif /* WITHUSBCDCACM */
#if WITHUSBUAC && WITHWAWXXUSB
	usb_dev_iso_xfer_uac_initialize(pusb);
#endif /* WITHUSBUAC */

	//pusb->ep0_flag = 0;
	pusb->ep0_xfer_state = USB_EP0_SETUP;
	//dma
	pusb->dma = wBoot_dma_request(1);
	if (!pusb->dma)
	{
		PRINTF("usb error: request dma fail\n");
	}

}

void usb_struct_init(PCD_HandleTypeDef *hpcd)
{
	usb_struct * const pusb = & hpcd->awxx_usb;
	uint32_t i=0;

	//pusb->sof_count = 0;

//	pusb->rst_cnt = 0;
//	pusb->cur_fsm = 0;
//	pusb->fsm_cnt = 0;

	//pusb->ep0_flag = 0;
	pusb->ep0_xfer_state = USB_EP0_SETUP;
	pusb->ep0_maxpktsz = 64;
	pusb->ep0_ret = USB_RETVAL_COMPOK;

	for(i=0; i<USB_MAX_EP_NO; i++)
	{
		pusb->eptx_flag[i] = 0;
		pusb->eprx_flag[i] = 0;
		pusb->eptx_xfer_state[i] = USB_EPX_SETUP;
		pusb->eprx_xfer_state[i] = USB_EPX_SETUP;
		pusb->eprx_ret[i] = USB_RETVAL_COMPOK;
		pusb->eptx_ret[i] = USB_RETVAL_COMPOK;
	}

	usb_set_dev_addr(pusb, 0x00);
	usb_struct_idle(pusb);

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
void usb_init(PCD_HandleTypeDef *hpcd)
{

	usb_struct * const pusb = & hpcd->awxx_usb;
	//uint32_t i=0;
	//uint32_t temp;

	usb_struct_init(hpcd);

	//usb_set_phytune(pusb);
	//usb_drive_vbus(pusb, 0, pusb->index);
	usb_drive_vbus(pusb, 0, 0);

	usb_force_id(pusb, 1);

	if (pusb->speed==USB_SPEED_FS)
		usb_high_speed_disable(pusb);
	else
		usb_high_speed_enable(pusb);

	usb_suspendm_enable(pusb);

	//usb_vbus_src(pusb, 0x0);	// 11..10 = state. 0 - not work, 1, 2, 3 - work
	//usb_release_vbus(pusb);	// 13 = 0, 12 = 0
	//usb_force_vbus(pusb, 1);	// 13=1, 12=state. state=1: игнорировать состояние входа VBUS, считать единичным
	usb_vbus_src(pusb, 0x0);
	usb_force_vbus(pusb, 1);

	usb_select_ep(pusb, 0);
	usb_ep0_flush_fifo(pusb);

	//PRINTF("USB Device!!\n");

	pusb->role = USB_ROLE_DEV;

	usb_clear_bus_interrupt_enable(pusb, 0xff);
	usb_set_bus_interrupt_enable(pusb, USB_BUSINT_DEV_WORK);
	usb_set_eptx_interrupt_enable(pusb, 0x003f);
	usb_set_eprx_interrupt_enable(pusb, 0x003e);

	pusb->otg_dev = USB_OTG_B_DEVICE;

	return;
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
	usb_struct * const pusb = & hpcd->awxx_usb;

	const uint32_t irqstatus = usb_get_bus_interrupt_status(pusb) & usb_get_bus_interrupt_enable(pusb);
	const uint32_t ep_save = usb_get_active_ep(pusb);

	//sof interrupt

	if (irqstatus & USB_BUSINT_SOF)
	{
		//pusb->sof_count ++;
#if WITHWAWXXUSB
		usb_dev_sof_handler(hpcd);
#else
		HAL_PCD_SOFCallback(hpcd);
#endif
		usb_clear_bus_interrupt_status(pusb, USB_BUSINT_SOF);
	}

	//bus interrupt
  	if (irqstatus & USB_BUSINT_SUSPEND)
  	{
		usb_clear_bus_interrupt_status(pusb, USB_BUSINT_SUSPEND);
	  	//Suspend Service Subroutine

		if (wBoot_dma_QueryState(pusb->dma))
		{
			PRINTF("Error: DMA for EP is not finished after Bus Suspend\n");
		}
		wBoot_dma_stop(pusb->dma);
#if ! WITHWAWXXUSB
		HAL_PCD_SuspendCallback(hpcd);
#endif
	  	//PRINTF("uSuspend\n");
		gpusb = NULL;
  	}

	if (irqstatus & USB_BUSINT_RESUME)
	{
		usb_clear_bus_interrupt_status(pusb, USB_BUSINT_RESUME);
		//Resume Service Subroutine
#if ! WITHWAWXXUSB
		HAL_PCD_ResumeCallback(hpcd);
#endif
		//PRINTF("uResume\n");
	}

	if (irqstatus & USB_BUSINT_RESET)
	{
		usb_clear_bus_interrupt_status(pusb, USB_BUSINT_RESET);
		uint32_t temp;
		uint32_t i;
		//Device Reset Service Subroutine
		//pusb->rst_cnt ++;
		for(i=0; i<USB_MAX_EP_NO; i++)
		{
			pusb->eptx_flag[i] = 0;
			pusb->eprx_flag[i] = 0;
			pusb->eptx_xfer_state[i] = USB_EPX_SETUP;
			pusb->eprx_xfer_state[i] = USB_EPX_SETUP;
			pusb->eprx_ret[i] = USB_RETVAL_COMPOK;
			pusb->eptx_ret[i] = USB_RETVAL_COMPOK;
		}
		usb_struct_idle(pusb);

		//pusb->timer = USB_IDLE_TIMER;

		//Bus Reset may disable all interrupt enable, re-enable the interrupts need
		usb_clear_bus_interrupt_enable(pusb, 0xff);
		usb_set_bus_interrupt_enable(pusb, USB_BUSINT_DEV_WORK);
		usb_set_eptx_interrupt_enable(pusb, 0x003f);
		usb_set_eprx_interrupt_enable(pusb, 0x003e);

		awxx_setup_fifo(pusb);

		if (wBoot_dma_QueryState(pusb->dma))
		{
			PRINTF("Error: DMA for EP is not finished after Bus Reset\n");
		}
		wBoot_dma_stop(pusb->dma);
		gpusb = pusb;
#if ! WITHWAWXXUSB
		HAL_PCD_ResetCallback(hpcd);
#endif
	  	//PRINTF("uReset\n");
	}

  	if (irqstatus & USB_BUSINT_SESSEND)
  	{
		usb_clear_bus_interrupt_status(pusb, USB_BUSINT_SESSEND);
  		//Device Reset Service Subroutine
		//PRINTF("uSessend\n");
  	}


	{
		//tx interrupt
		uint32_t temp;
		uint32_t i;
		temp = usb_get_eptx_interrupt_status(pusb);
		usb_clear_eptx_interrupt_status(pusb, temp);
		if (temp&0x01)
		{
			//pusb->ep0_flag ++;
			usb_dev_ep0xfer_handler(hpcd);
			//ep0_irq_count ++;
		}
		if (temp&0xfffe)
		{
			for(i=0; i<USB_MAX_EP_NO; i++)
			{
				if (temp & (0x2<<i))
				{
					pusb->eptx_flag[i] ++;
				}
			}
			//eptx_irq_count ++;
		}
	}

	{
		//rx interrupt
		uint32_t temp;
		uint32_t i;
		temp = usb_get_eprx_interrupt_status(pusb);
		usb_clear_eprx_interrupt_status(pusb, temp);
		if (temp&0xfffe)
		{
			for(i=0; i<USB_MAX_EP_NO; i++)
			{
				if (temp & (0x2<<i))
				{
					pusb->eprx_flag[i] ++;
				}
			}
			//eprx_irq_count ++;
		}
	}
  	usb_select_ep(pusb, ep_save);

  	usb_device_function(hpcd);
}

/**
  * @brief  USB_DevConnect : Connect the USB device by enabling Rpu
  * @param  USBx  Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_DevConnect(USBOTG_TypeDef *USBx)
{
	//PRINTF("USB_DevConnect\n");

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
	//PRINTF("USB_DevDisconnect\n");

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

	  usb_params_init(hpcd);

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
//	PRINTF("HAL_PCD_DeInit\n");
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
	pusb_struct pusb = & hpcd->awxx_usb;

	usb_struct_init(hpcd);

	usb_init(hpcd);

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
#if ! WITHWAWXXUSB
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
    ep->dma_addr = (uintptr_t)pBuf;
  }

  if ((ep_addr & EP_ADDR_MSK) == 0U)
  {
//#if WITHNEWUSBHAL
//	  USBPhyHw_ep0_read(hpcd, pBuf, len);
//#else /* WITHNEWUSBHAL */
//    (void)USB_EP0StartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
//#endif /* WITHNEWUSBHAL */
		usb_struct * const pusb = & hpcd->awxx_usb;

		pusb->ep0_xfer_srcaddr = (uintptr_t) pBuf;
		pusb->ep0_xfer_residue = len;

	   	pusb->ep0_xfer_state = USB_EP0_DATA;

	   	//PRINTF("HAL_PCD_EP_Receive: len=%u\n", len);
  }
  else
  {
//#if WITHNEWUSBHAL
//	  USBPhyHw_endpoint_read(hpcd, ep_addr, pBuf, len);
//#else /* WITHNEWUSBHAL */
//    (void)USB_EPStartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
//#endif /* WITHNEWUSBHAL */
  }
#endif
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
#if ! WITHWAWXXUSB
  usb_struct * const pusb = & hpcd->awxx_usb;
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
    ep->dma_addr = (uintptr_t)pBuf;
  }

  if ((ep_addr & EP_ADDR_MSK) == 0U)
  {
    //(void)USB_EP0StartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);

		pusb->ep0_xfer_srcaddr = (uintptr_t) pBuf;
		pusb->ep0_xfer_residue = len;

		usb_ep0_complete_send(pusb, (uintptr_t) pBuf, len);

	   	pusb->ep0_xfer_state = USB_EP0_DATA;
  }
  else
  {
    //(void)USB_EPStartXfer(hpcd->Instance, ep, (uint8_t)hpcd->Init.dma_enable);
		const uint32_t bo_ep_in = ep->num;
		switch (pusb->eptx_ret[bo_ep_in-1])
		{
		default:
			break;
		case USB_RETVAL_COMPOK:
			pusb->eptx_ret[bo_ep_in-1] = epx_in_handler_dev(pusb, bo_ep_in, (uintptr_t) pBuf, len, USB_PRTCL_BULK);
			break;
		}
  }
#endif
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
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address)
{
  //__HAL_LOCK(hpcd);
  hpcd->USB_Address = address;
  //(void)USB_SetDevAddress(hpcd->Instance, address);
  usb_set_dev_addr(& hpcd->awxx_usb, address);
  //__HAL_UNLOCK(hpcd);

  return HAL_OK;
}

#endif /* (CPUSTYLE_T113 || CPUSTYLE_F133) */
