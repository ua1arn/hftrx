/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if WITHUSBHW && WITHEHCIHW

#include "board.h"
#include "gpio.h"

#include "usb_device.h"
#include "usbh_core.h"
#include "ehci.h"

#if defined (WITHUSBHW_EHCI)

#ifndef WITHEHCIHW_EHCIPORT
	#define WITHEHCIHW_EHCIPORT 0
#endif /* WITHEHCIHW_EHCIPORT */

#ifndef WITHOHCIHW_OHCIPORT
	#define WITHOHCIHW_OHCIPORT 0
#endif /* WITHOHCIHW_OHCIPORT */

#define WITHEHCIHWSOFTSPOLL 1	/* не использовать аппаратные прерывания, HID_MOUSE написана не-thread safe */
//#define WITHTINYUSB 1


#if WITHTINYUSB

#include "host/usbh.h"

// Enable USB interrupt
void hcd_int_enable(uint8_t rhport)
{

}

// Disable USB interrupt
void hcd_int_disable(uint8_t rhport)
{

}

#if CFG_TUH_CDC

#include "class/cdc/cdc.h"
#include "class/cdc/cdc_host.h"

static uint8_t serial_in_buffer [128];
// invoked ISR context
void tuh_cdc_xfer_isr(uint8_t dev_addr, xfer_result_t event, cdc_pipeid_t pipe_id, uint32_t xferred_bytes)
{
  (void) event;
  (void) pipe_id;
  (void) xferred_bytes;

  //printf(serial_in_buffer);
  //tu_memclr(serial_in_buffer, sizeof(serial_in_buffer));

  tuh_cdc_receive(dev_addr, serial_in_buffer, sizeof(serial_in_buffer), true); // waiting for next data
}
#endif /* CFG_TUH_CDC */

#if CFG_TUH_HID

#include "class/hid/hid.h"
#include "class/hid/hid_host.h"

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use. tuh_hid_parse_report_descriptor()
// can be used to parse common/simple enough descriptor.
// Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE, it will be skipped
// therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len)
{
  (void)desc_report;
  (void)desc_len;
  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);

  printf("HID device address = %d, instance = %d is mounted\r\n", dev_addr, instance);
  printf("VID = %04x, PID = %04x\r\n", vid, pid);

  // Sony DualShock 4 [CUH-ZCT2x]
//  if ( is_sony_ds4(dev_addr) )
//  {
//    // request to receive report
//    // tuh_hid_report_received_cb() will be invoked when report is available
//    if ( !tuh_hid_receive_report(dev_addr, instance) )
//    {
//      printf("Error: cannot request to receive report\r\n");
//    }
//  }
}
// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

//  switch (itf_protocol)
//  {
//    case HID_ITF_PROTOCOL_KEYBOARD:
//      TU_LOG2("HID receive boot keyboard report\r\n");
//      process_kbd_report( (hid_keyboard_report_t const*) report );
//    break;
//
//    case HID_ITF_PROTOCOL_MOUSE:
//      TU_LOG2("HID receive boot mouse report\r\n");
//      process_mouse_report( (hid_mouse_report_t const*) report );
//    break;
//
//    default:
//      // Generic report requires matching ReportID and contents with previous parsed report info
//      process_generic_report(dev_addr, instance, report, len);
//    break;
//  }

  // continue to request to receive report
  if ( !tuh_hid_receive_report(dev_addr, instance) )
  {
    printf("Error: cannot request to receive report\r\n");
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
  printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);

}

#endif /* CFG_TUH_HID */
#endif /* WITHTINYUSB */

/* USB Host Core handle declaration. */
USBH_HandleTypeDef hUsbHostHS;

static EHCI_HandleTypeDef hehci_USB;

#if WITHUSEUSBFLASH
#include "../../Class/MSC/Inc/usbh_msc.h"
#endif /* WITHUSEUSBFLASH */
#include "../../Class/HID/Inc/usbh_hid.h"
#include "../../Class/HUB/Inc/usbh_hub.h"

#if CPUSTYLE_XC7Z

XUSBPS_Registers * EHCIxToUSBx(void * p)
{
    XUSBPS_Registers * const USBx = (WITHUSBHW_EHCI == EHCI0) ? USB0 : USB1;
    return USBx;
}

#elif (CPUSTYLE_A64)

//USBPHYC_TypeDef * EHCIxToUSBPHYC(void * p)
//{
//	USBPHYC_TypeDef * const PHYCx = (WITHUSBHW_EHCI == USBEHCI1) ? USBPHY1 : USBPHY0;
//    return PHYCx;
//}

static void SetupUsbPhyc(USBPHYC_TypeDef * phy)
{
	phy->HCI_ICR |= (1u << 0);

//	phy->HCI_ICR |= (1u << 1);
//	phy->HCI_ICR |= (1u << 17);

	phy->HCI_ICR |= (1u << 20);		// EHCI HS force

	phy->HCI_ICR |= (0x0Fu << 8);	//

	PRINTF("phy->HCI_ICR: %08X\n", (unsigned) phy->HCI_ICR);
}


#elif (CPUSTYLE_T113 || CPUSTYLE_F133)

USBPHYC_TypeDef * EHCIxToUSBPHYC(void * p)
{
	USBPHYC_TypeDef * const PHYCx = (WITHUSBHW_EHCI == USBEHCI1) ? USBPHY1 : USBPHY0;
    return PHYCx;
}

#endif /* CPUSTYLE_XC7Z */

/* Вызываем после выполнения reset контроллера */
void USBH_POSTRESET_INIT(void)
{
#if CPUSTYLE_XC7Z
	XUSBPS_Registers * const USBx = EHCIxToUSBx(WITHUSBHW_EHCI);


	USBx->MODE = (USBx->MODE & ~ (0x0003)) |
		//0x02 |		// IDLE
		0x03 |		// HOST
		0;

#endif /* CPUSTYLE_XC7Z */
}


// See https://github.com/hulei123/git123/blob/b82c4abbe7c1bf336b956a613ceb31436938e063/src/usb_stack/usb_core/hal/fsl_usb_ehci_hal.h
// https://github.com/LucasIankowski/T2LabSisop/blob/3fe926e01623ca007afc9d7a80c764418d92c2bd/drivers/usb/host/ehci-q.c

// Taken from
//	https://github.com/xushanpu123/xsp-daily-work/blob/ce4b31db29a560400ac948053b451a2122631490/rCore-Tutorial-v3/qemu-5.0.0/roms/ipxe/src/drivers/usb/ehci.c
//	https://github.com/xushanpu123/xsp-daily-work/blob/ce4b31db29a560400ac948053b451a2122631490/rCore-Tutorial-v3/qemu-5.0.0/roms/ipxe/src/include/ipxe/usb.h

static uintptr_t virt_to_phys(volatile void * v)
{
	return (uintptr_t) v;
}

/**
 * Get link value for a queue head
 *
 * @v queue             Queue head
 * @ret link            Link value
 */
static inline uint32_t ehci_link_qh ( struct ehci_queue_head *queue ) {

	return ( virt_to_phys ( queue ) | EHCI_LINK_TYPE_QH );
}

#if ! defined (__BYTE_ORDER__)
	#error No __BYTE_ORDER__ defined
#endif

#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

static unsigned le32_to_cpu(uint32_t v)
{
	return v;
}

static uint32_t cpu_to_le32(unsigned v)
{
	return v;
}

static unsigned le16_to_cpu(uint16_t v)
{
	return v;
}

static uint16_t cpu_to_le16(unsigned v)
{
	return v;
}

#else

static unsigned le32_to_cpu(uint32_t v)
{
	return __REV(v);
}

static uint32_t cpu_to_le32(unsigned v)
{
	return __REV(v);
}

static unsigned le16_to_cpu(uint16_t v)
{
	return __REV16(v);
}

static uint16_t cpu_to_le16(unsigned v)
{
	return __REV16(v);
}

#endif

//
///* установка указаных в mask битов в состояние data */
//static void le32_modify(volatile uint32_t * variable, uint_fast32_t mask, uint_fast32_t data)
//{
//	const uint_fast32_t v = * variable;
//	const uint_fast32_t m = cpu_to_le32(mask);
//	* variable = (v & ~ m) | (cpu_to_le32(data) & m);
//}

/* установка указаных в mask битов в состояние data */
//static void le16_modify(volatile uint16_t * variable, uint_fast16_t mask, uint_fast16_t data)
//{
//	const uint_fast16_t v = * variable;
//	const uint_fast16_t m = cpu_to_le16(mask);
//	* variable = (v & ~ m) | (cpu_to_le16(data) & m);
//}

/* установка указаных в mask битов в состояние data */
static void le8_modify(volatile uint8_t * variable, uint_fast8_t mask, uint_fast8_t data)
{
	const uint_fast8_t v = * variable;
	const uint_fast8_t m = mask;
	* variable = (v & ~ m) | (data & m);
}

/**
 * Get link value for a queue head
 *
 * @v queue             Queue head
 * @ret link            Link value
 */
static inline uint32_t ehci_link_qhv ( volatile struct ehci_queue_head *queue ) {

	return ( virt_to_phys ( queue ) | EHCI_LINK_TYPE_QH );
}


/*
 * Terminate (T). 1=Last QH (pointer is invalid). 0=Pointer is valid.
 * If the queue head is in the context of the periodic list, a one bit in this field indicates to the host controller that
 * this is the end of the periodic list. This bit is ignored by the host controller when the queue head is in the Asynchronous schedule.
 * Software must ensure that queue heads reachable by the host controller always have valid horizontal link pointers. See Section 4.8.2
 *
 */
static void asynclist_item(volatile struct ehci_queue_head * p, uint32_t link, int Head)
{
	ASSERT((virt_to_phys(p) & 0x01F) == 0);
	//ASSERT((virt_to_phys(link) & 0x01F) == 0);
	//memset((void *) p, 0xFF, sizeof * p);
	p->link = cpu_to_le32(link); //ehci_link_qhv(link);	// Using of List Termination here prohibited

	p->cap = EHCI_CAP_MULT(1);
	p->chr = cpu_to_le32(EHCI_CHR_HEAD * Head);
	p->cache.status = EHCI_STATUS_HALTED;
	p->cache.len = 0 * EHCI_LEN_TOGGLE;
	p->cache.next = cpu_to_le32(EHCI_LINK_TERMINATE);
	p->cache.alt = cpu_to_le32(EHCI_LINK_TERMINATE);
}

static void qtd_item2_set_toggle(volatile struct ehci_transfer_descriptor * p, int state)
{
	//le16_modify(& p->len, EHCI_LEN_TOGGLE, ! ! state * EHCI_LEN_TOGGLE);
	const uint_fast16_t v = p->len;
	const uint_fast16_t m = cpu_to_le16(EHCI_LEN_TOGGLE);
	p->len = (v & ~ m) | (cpu_to_le16(! ! state * EHCI_LEN_TOGGLE) & m);
}

static void qtd_item2_set_length(volatile struct ehci_transfer_descriptor * p, unsigned length)
{
	//le16_modify(& p->len, EHCI_LEN_MASK, length);	/* не модифицируем флаг EHCI_LEN_TOGGLE */
	const uint_fast16_t v = p->len;
	const uint_fast16_t m = cpu_to_le16(EHCI_LEN_MASK);
	p->len = (v & ~ m) | (cpu_to_le16(length) & m);
}

static uint_fast8_t qtd_item2_buff(volatile struct ehci_transfer_descriptor * p, volatile uint8_t * data, uint32_t length)
{
	unsigned i;
	ASSERT(offsetof(struct ehci_transfer_descriptor, high) == 32);
	qtd_item2_set_length(p, length);	/* не модифицируем флаг EHCI_LEN_TOGGLE */

	for (i = 0; i < ARRAY_SIZE(p->low) && length != 0; ++ i)
	{
		/* Calculate length of this fragment */
		const uintptr_t phys = virt_to_phys ( data );
		const uintptr_t offset = ( phys & ( EHCI_PAGE_ALIGN - 1 ) );
		uint32_t frag_len = ( EHCI_PAGE_ALIGN - offset );
		if ( frag_len > length )
			frag_len = length;

		/* Sanity checks */
		ASSERT( ( i == 0 ) || ( offset == 0 ) );

		/* Populate buffer pointer */
		p->low [i] = cpu_to_le32 ( phys );
		if ( sizeof ( uintptr_t ) > sizeof ( uint32_t ) ) {
			p->high [i] =
					cpu_to_le32 ( ((uint64_t) phys) >> 32 );
		}

		/* Move to next fragment */
		data += frag_len;
		length -= frag_len;
	}
	return length != 0;		// 0 - без ошибок
}
// fill 3.5 Queue Element Transfer Descriptor (qTD)
static void qtd_item2(volatile struct ehci_transfer_descriptor * p, unsigned pid, unsigned ping)
{
	ASSERT(offsetof(struct ehci_transfer_descriptor, high) == 32);
	//memset((void *) p, 0, sizeof * p);

	p->next = cpu_to_le32(EHCI_LINK_TERMINATE);	// возможно потребуется адрес следующего буфера
	p->alt = cpu_to_le32(EHCI_LINK_TERMINATE);


//	p->len = cpu_to_le16((length & EHCI_LEN_MASK) | (pid != EHCI_FL_PID_SETUP) * EHCI_LEN_TOGGLE);	// Data toggle.
//														// This bit controls the data toggle sequence. This bit should be set for IN and OUT transactions and
//														// cleared for SETUP packets
// 	Zynq7000:
//	Note: The HCD must not program Cerr to a value of 0 when the QH.EPS field is programmed with
//	a value indicating a FS or LS device. This combination could result in undefined behavior

	p->flags = pid | 1 * EHCI_FL_CERR_MAX | EHCI_FL_IOC;	// Current Page (C_Page) field = 0
	p->status = 0 * EHCI_STATUS_ACTIVE | EHCI_STATUS_PING * (ping != 0);
	//le8_modify(& p->status, EHCI_STATUS_MASK | EHCI_STATUS_PING, EHCI_STATUS_PING * (ping != 0));
}


/*
 * Terminate (T). 1=Last QH (pointer is invalid). 0=Pointer is valid.
 * If the queue head is in the context of the periodic list, a one bit in this field indicates to the host controller that
 * this is the end of the periodic list. This bit is ignored by the host controller when the queue head is in the Asynchronous schedule.
 * Software must ensure that queue heads reachable by the host controller always have valid horizontal link pointers. See Section 4.8.2
 *
 */
/*
	* @param  ep_type Endpoint Type.
	*          This parameter can be one of these values:
	*            EP_TYPE_CTRL: Control type/
	*            EP_TYPE_ISOC: Isochronous type/
	*            EP_TYPE_BULK: Bulk type/
	*            EP_TYPE_INTR: Interrupt type/
	*/
static void asynclist_item2(EHCI_HCTypeDef * hc, volatile struct ehci_queue_head * p, uint32_t current, int Head)
{
	ASSERT((virt_to_phys(p) & 0x01F) == 0);
	uint32_t chr;
	/* Determine basic characteristics */
	chr = EHCI_CHR_ADDRESS(hc->dev_addr) |	// Default DCFG_DAD field = 0
			EHCI_CHR_ENDPOINT(hc->ep_num) |	/* маскирование всего, кроме младших 4=х бит выполняется */
			EHCI_CHR_MAX_LEN(hc->max_packet );

	/* Control endpoints require manual control of the data toggle */
	if (hc->ep_type == EP_TYPE_CTRL)
		chr |= EHCI_CHR_TOGGLE;

	/* Determine endpoint speed */
	if (hc->speed == USBH_SPEED_HIGH) {
		chr |= EHCI_CHR_EPS_HIGH;
	} else {
		if (hc->speed == USBH_SPEED_FULL ) {
			chr |= EHCI_CHR_EPS_FULL;
		} else {
			chr |= EHCI_CHR_EPS_LOW;
		}
		if (hc->ep_type == EP_TYPE_CTRL)
			chr |= EHCI_CHR_CONTROL;

	}

	uint32_t cap;
	cap =
			EHCI_CAP_MULT(1) | 	// 00b: reserved, 01b One transaction to be issued for this endpoint per micro-frame
			EHCI_CAP_TT_HUB(hc->tt_hubaddr) |
			EHCI_CAP_TT_PORT(hc->tt_prtaddr);

	if (hc->ep_type == EP_TYPE_INTR) {
		if (0 && hc->tt_hubaddr != 0 && hc->tt_hubaddr != 0) {
			if (hc->ep_type == EP_TYPE_INTR)
				cap |= EHCI_CAP_SPLIT_SCHED_DEFAULT;
		} else {
			unsigned i;
			for (i = 7; i < 8; ++ i) {
				cap |= EHCI_CAP_INTR_SCHED(i);
			}
		}
	}

	// RL, C, Maximum Packet Length, H, dtc, EPS, EndPt, I, Device Address
	p->chr = cpu_to_le32(chr | EHCI_CHR_HEAD * Head);
	// Mult, Port Number, Hub Addr, uFrame C-mask, uFrame S-mask
	p->cap = cpu_to_le32(cap);

	ASSERT((current & EHCI_LINK_TERMINATE) == 0);	/* "T" bit disallowed here */
	ASSERT((current & 0x001E) == 0);
	p->current = cpu_to_le32(current);	/* Where to place result of transfer */
	//p->cache.next = cpu_to_le32(EHCI_LINK_TERMINATE);
	//p->cache.status = EHCI_STATUS_ACTIVE;
}

HAL_StatusTypeDef EHCI_DriveVbus(USB_EHCI_CapabilityTypeDef * const EHCIx, uint8_t state) {
	//PRINTF("EHCI_DriveVbus: state=%d\n", (int) state);
	board_set_usbhostvbuson(state);
	board_update();
	return HAL_OK;
}

HAL_StatusTypeDef EHCI_StopHost(USB_EHCI_CapabilityTypeDef * const EHCIx) {

 	//USB_EHCI_CapabilityTypeDef * const EHCIx = (USB_EHCI_CapabilityTypeDef *) hehci->Instance;

 	EHCIx->USBINTR = 0;


	/* Clear run/stop bit */
 	unsigned usbcmd;
	usbcmd = EHCIx->USBCMD;
	usbcmd &= ~( EHCI_USBCMD_RUN | EHCI_USBCMD_PERIODIC |
			EHCI_USBCMD_ASYNC );
	EHCIx->USBCMD = usbcmd;
	(void) EHCIx->USBCMD;

	unsigned i;
	/* Wait for device to stop */
	for ( i = 0 ; 1 || i < 100 ; i++ ) {
		unsigned usbsts;
		/* Check if device is stopped */
		usbsts = EHCIx->USBSTS;
		if ( usbsts & EHCI_USBSTS_HCH )
			break; //return HAL_OK;

		/* Delay */
		//local_delay_ms( 1 );
	}

	return HAL_OK;
}

/**
  * @brief  Return Host Current Frame number
  * @param  USBx  Selected device
  * @retval current frame number
  */
uint32_t HAL_EHCI_GetCurrentFrame(EHCI_HandleTypeDef * hehci)
{
 	USB_EHCI_CapabilityTypeDef * const EHCIx = hehci->Instance;

	return EHCIx->FRINDEX;
}

static void EHCI_StopAsync(USB_EHCI_CapabilityTypeDef * EHCIx)
{
//	EHCIx->USBCMD |= EHCI_USBCMD_ASYNC_ADVANCE;
//	(void) EHCIx->USBCMD;
//	while ((EHCIx->USBSTS & EHCI_USBCMD_ASYNC_ADVANCE) != 0)
//		;

	// Stop ASYNC queue
	EHCIx->USBCMD &= ~ EHCI_USBCMD_ASYNC;
	(void) EHCIx->USBCMD;
	while ((EHCIx->USBSTS & EHCI_USBSTS_ASYNC) != 0)
	{
		ASSERT((EHCIx->USBSTS & EHCI_USBSTS_HCH) == 0);
	}
}

static void EHCI_StartAsync(USB_EHCI_CapabilityTypeDef * EHCIx)
{
	// Run ASYNC queue
	EHCIx->USBCMD |= EHCI_USBCMD_ASYNC;
	(void) EHCIx->USBCMD;
	while ((EHCIx->USBSTS & EHCI_USBSTS_ASYNC) == 0)
	{
		ASSERT((EHCIx->USBSTS & EHCI_USBSTS_HCH) == 0);
	}
}

/**
  * @brief  Initialize a host channel.
  * @param  hehci HCD handle
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
  * @param  tt_hubaddr HUB address.
  *          device address of the transaction translator’s hub.
  * @param  tt_prtaddr Port address
  *          port number of the recipient transaction translator.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_EHCI_HC_Init(EHCI_HandleTypeDef *hehci,
                                  uint8_t ch_num,
                                  uint8_t epnum,
                                  uint8_t dev_address,
                                  uint8_t speed,
                                  uint8_t ep_type,
                                  uint16_t mps,
								  uint8_t tt_hubaddr,
								  uint8_t tt_prtaddr)
{
	HAL_StatusTypeDef status = HAL_OK;
	EHCI_HCTypeDef * const hc = & hehci->hc [ch_num];
	USB_EHCI_CapabilityTypeDef * const EHCIx = hehci->Instance;

	__HAL_LOCK(hehci);
	EHCI_StopAsync(EHCIx);

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
	}
	else
	{
		hc->ep_is_in = 0U;
	}

	hc->speed = speed;

	InitializeListHead(& hc->tdlist);


// TODO: use queue head
//  status =  USB_HC_Init(hehci->Instance,
//                        ch_num,
//                        epnum,
//                        dev_address,
//                        speed,
//                        ep_type,
//                        mps, tt_hubaddr, tt_prtadd);
	qtd_item2_set_toggle(& hehci->asynclisthead [hc->ch_num].cache, 0);
	qtd_item2_set_toggle(& hehci->itdsarray [hc->ch_num].cache, 0);
	//PRINTF("HAL_EHCI_HC_Init: hc->ch_num=%d\n");

	dcache_clean_invalidate((uintptr_t) & hehci->asynclisthead, sizeof hehci->asynclisthead);
	dcache_clean_invalidate((uintptr_t) & hehci->itdsarray, sizeof hehci->itdsarray);
	dcache_clean_invalidate((uintptr_t) & hehci->qtds, sizeof hehci->qtds);

	EHCI_StartAsync(EHCIx);
	__HAL_UNLOCK(hehci);

	return status;
}

/**
  * @brief  Halt a host channel.
  * @param  hehci HCD handle
  * @param  ch_num Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_EHCI_HC_Halt(EHCI_HandleTypeDef *hehci, uint8_t ch_num)
{
	HAL_StatusTypeDef status = HAL_OK;
	EHCI_HCTypeDef * const hc = & hehci->hc [ch_num];
	USB_EHCI_CapabilityTypeDef * const EHCIx = hehci->Instance;

	__HAL_LOCK(hehci);
	// TODO: use queue head
	EHCI_StopAsync(EHCIx);

	unsigned i = ch_num;

	asynclist_item(& hehci->asynclisthead [i], ehci_link_qhv(& hehci->asynclisthead [(i + 1) % ARRAY_SIZE(hehci->asynclisthead)]), i == 0);
	asynclist_item(& hehci->itdsarray [i], EHCI_LINK_TERMINATE | EHCI_LINK_TYPE(1), 1);

	{

		hehci->qtds [i].status = 0;//EHCI_STATUS_HALTED;
		hehci->qtds [i].len = 0;	// toggle bit = 0
		hehci->qtds [i].next = cpu_to_le32(EHCI_LINK_TERMINATE);
		hehci->qtds [i].alt = cpu_to_le32(EHCI_LINK_TERMINATE);
	}

	hc->ehci_urb_state = URB_IDLE;

	dcache_clean_invalidate((uintptr_t) & hehci->asynclisthead, sizeof hehci->asynclisthead);
	dcache_clean_invalidate((uintptr_t) & hehci->itdsarray, sizeof hehci->itdsarray);
	dcache_clean_invalidate((uintptr_t) & hehci->qtds, sizeof hehci->qtds);

	EHCI_StartAsync(EHCIx);

	//(void)USB_HC_Halt(hehci->Instance, (uint8_t)ch_num);
	__HAL_UNLOCK(hehci);

	return status;
}

/**
  * @brief  Connect callback.
  * @param  hehci: EHCI handle
  * @retval None
  */
void HAL_EHCI_Connect_Callback(EHCI_HandleTypeDef *hehci)
{
  USBH_LL_Connect(hehci->pData);
}

/**
  * @brief  Disconnect callback.
  * @param  hehci: EHCI handle
  * @retval None
  */
void HAL_EHCI_Disconnect_Callback(EHCI_HandleTypeDef *hehci)
{
  USBH_LL_Disconnect(hehci->pData);
}

/**
* @brief  Port Port Enabled callback.
  * @param  hehci: EHCI handle
  * @retval None
  */
void HAL_EHCI_PortEnabled_Callback(EHCI_HandleTypeDef *hehci)
{
  USBH_LL_PortEnabled(hehci->pData);
}

/**
  * @brief  SOF callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_EHCI_SOF_Callback(EHCI_HandleTypeDef *hhcd)
{
  USBH_LL_IncTimer(hhcd->pData);
}


static EHCI_HCTypeDef * ghc = NULL;
/**
  * @brief  Handle USB_EHCI interrupt request.
  * @param  hehci USB_EHCI handle
  * @retval None
  */
void HAL_EHCI_IRQHandler(EHCI_HandleTypeDef * hehci)
{
 	USB_EHCI_CapabilityTypeDef * const EHCIx = hehci->Instance;

 	/* Защита от вызовов при неинициализированном объекте при опросе. */
 	if (EHCIx == NULL)
 		return;

 	const uint_fast32_t usbsts = le32_to_cpu(EHCIx->USBSTS);
 	//const uint_fast32_t usbstsMasked = usbsts & le32_to_cpu(EHCIx->USBSTS & EHCIx->USBINTR);
	uint_fast32_t portsc = le32_to_cpu(hehci->portsc [WITHEHCIHW_EHCIPORT]);
 	//PRINTF("HAL_EHCI_IRQHandler: USBSTS=%08X, portsc=%08X\n", usbsts, portsc);

 	if ((usbsts & (1u << 13)) != 0)
 	{
 		// Reclamation - R/O status bit
 	 	//PRINTF("HAL_EHCI_IRQHandler: Reclamation, usbsts=%08X\n", usbsts);
 	}
 	if ((usbsts & (1u << 0)))	// USB Interrupt (USBINT) - see EHCI_FL_IOC usage
 	{
 		EHCIx->USBSTS = cpu_to_le32(1u << 0);	// Clear USB Interrupt (USBINT)
 		//PRINTF("HAL_EHCI_IRQHandler: USB Interrupt (USBINT), usbsts=%08X\n", usbsts);
		__DMB();     // ensure the ordering of data cache maintenance operations and their effects

 		//unsigned ch_num;
 		//for (ch_num = 0; ch_num < ARRAY_SIZE(hehci->hc); ++ ch_num)
 		if (ghc != NULL)
 		{
			//EHCI_HCTypeDef * const hc = & hehci->hc [ch_num];
			EHCI_HCTypeDef * const hc = ghc;

			//if (hc->ehci_urb_state != URB_IDLE)
			//	continue;
			ASSERT(hc->ehci_urb_state == URB_IDLE);
//			if (/*hc->ch_num >= ARRAY_SIZE(hehci->hc) || */ hc->ch_num != ch_num)
//				continue;
			const volatile struct ehci_transfer_descriptor * const qtd = & hehci->qtds [hc->ch_num];
			const uint_fast8_t status = qtd->status;
			unsigned len = le16_to_cpu(qtd->len) & EHCI_LEN_MASK;
			unsigned pktcnt = hc->xfer_len - len;
	 		//PRINTF("HAL_EHCI_IRQHandler: (USBINT), hc=%d(#%d,hub=%d,prt=%d,spd=%d), usbsts=%08X, status=%02X, pktcnt=%u\n", hc->ch_num, hc->dev_addr, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed, usbsts, status, pktcnt);
			if ((status & EHCI_STATUS_HALTED) != 0)
			{
		 		//PRINTF("HAL_EHCI_IRQHandler: HALTED: hc=%d(#%d,hub=%d,prt=%d,spd=%d), usbsts=%08X, status=%02X, pktcnt=%u\n", hc->ch_num, hc->dev_addr, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed, usbsts, status, pktcnt);
				/* serious "can't proceed" faults reported by the hardware */
				// Тут разбирать по особенностям ошибки
		 		if (0)
		 		{

		 		}
		 		else if ((status & EHCI_STATUS_BABBLE) != 0)
				{
//		 			PRINTF("ch_num=%u: EHCI_STATUS_BABBLE\n", hc->ch_num);
					hc->ehci_urb_state = URB_NOTREADY;
					hc->ehci_urb_state = URB_STALL;
				}
				else if ((status & EHCI_STATUS_BUFFER) != 0)
				{
//		 			PRINTF("ch_num=%u: EHCI_STATUS_BUFFER\n", hc->ch_num);
					hc->ehci_urb_state = URB_STALL;
				}
				else if ((status & EHCI_STATUS_XACT_ERR) != 0)
				{
//		 			PRINTF("ch_num=%u: EHCI_STATUS_XACT_ERR, pktcnt=%u\n", hc->ch_num, (unsigned) pktcnt);
//		 			printhex((uintptr_t) hc->xfer_buff, hc->xfer_buff, 32);
					hc->ehci_urb_state = URB_NOTREADY;
					hc->ehci_urb_state = URB_STALL;
				}
				else
				{
//		 			PRINTF("ch_num=%u: stall\n", hc->ch_num);
					hc->ehci_urb_state = URB_STALL;
				}
	 			ghc = NULL;
			}
			else if ((status & EHCI_STATUS_ACTIVE) != 0)
			{
				//continue;	/* обмен еще не закончился */
			}
			else
			{

//	 			PRINTF("ch_num=%u: ok\n", hc->ch_num);
//	 			if (hc->ep_is_in && /* hc->ep_type == EP_TYPE_INTR */)
//	 			{
//	 				PRINTF("status=%02X ", status);
//	 				printhex((uintptr_t) hc->xfer_buff, hc->xfer_buff, pktcnt);
//	 			}

				hc->xfer_buff += pktcnt;
				hc->xfer_count += pktcnt;
				// Transaction done
	 			hc->ehci_urb_state = URB_DONE;
	 			ghc = NULL;
			}
 		}

 		ASSERT((sizeof (struct ehci_transfer_descriptor) % DCACHEROWSIZE) == 0);	/* чтобы invalidate не затронул соседние данные */
 		dcache_invalidate((uintptr_t) & hehci->qtds, sizeof hehci->qtds);	/* чтобы следующая проверка могла работать */
 		dcache_invalidate((uintptr_t) & hehci->asynclisthead, sizeof hehci->asynclisthead);

 		HAL_EHCI_SOF_Callback(hehci);
 	}

 	if ((usbsts & (1u << 1)))	// USB Error Interrupt (USBERRINT)
 	{
 		EHCIx->USBSTS = cpu_to_le32(1u << 1);	// Clear USB Error Interrupt (USBERRINT) interrupt
 		//PRINTF("HAL_EHCI_IRQHandler: USB Error\n");
 		//hehci->urbState = USBH_URB_ERROR;
 	}

 	if ((usbsts & (1u << 2)))	// Port Change Detect
 	{
 		EHCIx->USBSTS = cpu_to_le32(1u << 2);	// Clear Port Change Detect interrupt
 		uint_fast32_t portsc = le32_to_cpu(hehci->portsc [WITHEHCIHW_EHCIPORT]);
 		const int pe = (portsc >> 2) & 0x01;
		//PRINTF("HAL_EHCI_IRQHandler: Port Change Detect, usbsts=%08X, portsc=%08X, ls=%lu, pe=%lu, ccs=%d, po=%d\n", usbsts, portsc, (portsc >> 10) & 0x03, (portsc >> 2) & 0x01, !! (portsc & EHCI_PORTSC_CCS), !! (portsc & EHCI_PORTSC_OWNER));
 		// PORTSC[0]=00001002 - on disconnect
 		// PORTSC[0]=00001803 - on connect
// 		unsigned i;
// 		for (i = 0; i < hehci->nports; ++ i)
// 	 	{
// 	 		PRINTF("HAL_EHCI_IRQHandler: PORTSC[%u]=%08X\n", i, hehci->portsc [i]);
// 	 	}

//		if ((portsc & EHCI_PORTSC_PED) != 0)
// 		{
// 			portsc &= ~ EHCI_PORTSC_PED;
// 			hehci->ports [WITHEHCIHW_EHCIPORT] = portsc;
// 			(void) hehci->ports [WITHEHCIHW_EHCIPORT];
//
// 			HAL_EHCI_PortEnabled_Callback(hehci);
// 		}

 		if ((portsc & EHCI_PORTSC_CCS) != 0)
 		{
// 			PRINTF("Device Connect handler, portsc=%08X\n", portsc);
			HAL_EHCI_PortEnabled_Callback(hehci);
 			HAL_EHCI_Connect_Callback(hehci);
 		}
 		else if ((portsc & EHCI_PORTSC_OWNER) != 0)
		{
			// OHCI connected
 			PRINTF("Device Disonnect handler, OHCI ownership portsc=%08X\n", portsc);
		}
 		else
 		{
// 			PRINTF("Device Disconnect handler, portsc=%08X\n", portsc);
			HAL_EHCI_Disconnect_Callback(hehci);
 		}
	}

 	if ((usbsts & (1u << 3)))	// Frame List Rollower
 	{
 		EHCIx->USBSTS = cpu_to_le32(1u << 3);	// Clear Frame List Rollower interrupt
// 		PRINTF("HAL_EHCI_IRQHandler: Frame List Rollower\n");

 		HAL_EHCI_SOF_Callback(hehci);
 	}

 	if ((usbsts & (1u << 4)))	// Host System Error
 	{
 		EHCIx->USBSTS = cpu_to_le32(1u << 4);	// Clear Host System Error interrupt
 		unsigned portsc = le32_to_cpu(hehci->portsc [WITHEHCIHW_EHCIPORT]);
		PRINTF("HAL_EHCI_IRQHandler: Host System Error, usbsts=%08X, portsc=%08X, ls=%u, pe=%u, ccs=%d\n", usbsts, portsc, (portsc >> 10) & 0x03, (portsc >> 2) & 0x01, !! (portsc & EHCI_PORTSC_CCS));
		//hehci->urbState = USBH_URB_ERROR;
 	}

 	if ((usbsts & (1u << 5)))	// Interrupt On Async Advance
 	{
 		EHCIx->USBSTS = cpu_to_le32(1u << 5);	// Clear Interrupt On Async Advance
 		//PRINTF("HAL_EHCI_IRQHandler: Interrupt On Async Advance\n");
 	}
}

void HAL_OHCI_IRQHandler(EHCI_HandleTypeDef * hehci)
{
#if WITHTINYUSB
	hcd_int_handler(BOARD_TUH_RHPORT);
	return;
#endif /* WITHTINYUSB */
	if (hehci->ohci == NULL)
		return;
	//ASSERT(0);
	const unsigned HcInterruptStatus = le32_to_cpu(hehci->ohci->HcInterruptStatus);
	if ((HcInterruptStatus & (1u << 6)) != 0)
	{
		PRINTF("HAL_OHCI_IRQHandler: RootHubStatusChange HcInterruptStatus=%08X\n", HcInterruptStatus);

	}
	if ((HcInterruptStatus & (1u << 5)) != 0)
	{
		//PRINTF("HAL_OHCI_IRQHandler: FrameNumberOverflow HcInterruptStatus=%08X\n", HcInterruptStatus);

	}
	if ((HcInterruptStatus & (1u << 4)) != 0)
	{
		PRINTF("HAL_OHCI_IRQHandler: UnrecoverableError HcInterruptStatus=%08X\n", HcInterruptStatus);

	}
	if ((HcInterruptStatus & (1u << 3)) != 0)
	{
		PRINTF("HAL_OHCI_IRQHandler: ResumeDetected HcInterruptStatus=%08X\n", HcInterruptStatus);

	}
	if ((HcInterruptStatus & (1u << 2)) != 0)
	{
		//PRINTF("HAL_OHCI_IRQHandler: StartofFrame HcInterruptStatus=%08X\n", HcInterruptStatus);

	}
	if ((HcInterruptStatus & (1u << 1)) != 0)
	{
		PRINTF("HAL_OHCI_IRQHandler: WritebackDoneHead HcInterruptStatus=%08X\n", HcInterruptStatus);

	}
	if ((HcInterruptStatus & (1u << 0)) != 0)
	{
		PRINTF("HAL_OHCI_IRQHandler: SchedulingOverrun HcInterruptStatus=%08X\n", HcInterruptStatus);

	}

	hehci->ohci->HcInterruptStatus = cpu_to_le32(HcInterruptStatus);	/* reset interrupt */
}

HAL_StatusTypeDef HAL_EHCI_Init(EHCI_HandleTypeDef *hehci)
{
	unsigned i;
	USB_EHCI_CapabilityTypeDef * const EHCIx = hehci->Instance;

	HAL_EHCI_MspInit(hehci);	// включить тактирование, настроить PHYC PLL

	// 	ehci_init(& ehcidevice0, hehci->Instance);
	//    INIT_LIST_HEAD(& ehcidevice0.endpoints);
	//    INIT_LIST_HEAD(& ehcidevice0.async);
	//
	// 	VERIFY(ehci_reset(& ehcidevice0) == 0);
	//	ehci_dump(& ehcidevice0);
	// power cycle for USB dongle
	// 	board_set_usbhostvbuson(0);
	// 	board_update();
	// 	HARDWARE_DELAY_MS(200);
	//	board_set_usbhostvbuson(1);
	//	board_update();
	//	HARDWARE_DELAY_MS(200);

	// https://github.com/pdoane/osdev/blob/master/usb/ehci.c

	// USBH_EHCI_HCICAPLENGTH == EHCIx->HCCAPBASE
	// USBH_EHCI_HCSPARAMS == EHCIx->HCSPARAMS
	// USBH_EHCI_HCCPARAMS == EHCIx->HCCPARAMS
	// OHCI BASE = USB1HSFSP2_BASE	(MPU_AHB6_PERIPH_BASE + 0xC000)
	// EHCI BASE = USB1HSFSP1_BASE	(MPU_AHB6_PERIPH_BASE + 0xD000)

	// Calculate Operational Register Space base address
	const uintptr_t opregspacebase = (uintptr_t) &EHCIx->HCCAPBASE
			+ (EHCIx->HCCAPBASE & 0x00FF);
	hehci->nports = (EHCIx->HCSPARAMS >> 0) & 0x0F;
	hehci->portsc = (__IO uint32_t*) (opregspacebase + 0x0044);
	hehci->configFlag = (__IO uint32_t*) (opregspacebase + 0x0040);

	 PRINTF("HAL_EHCI_Init: NPORTS=%u\n", (unsigned) hehci->nports);
	ASSERT(WITHEHCIHW_EHCIPORT < hehci->nports);
	//EhciOpRegs * const opRegs = (EhciOpRegs*) opregspacebase;
	//hehci->ehci.capRegs = (EhciCapRegs*) EHCIx;

	ASSERT((virt_to_phys(& hehci->periodiclist) & 0xFFF) == 0);
	InitializeListHead(& hehci->hcListAsync);// Host channels, ожидающие обмена в ASYNCLISTADDR
	InitializeListHead(& hehci->hcListPeriodic);	// Host channels, ожидающие обмена в PERIODICLISTBASE
	SPINLOCK_INITIALIZE(& hehci->asynclock);

	// https://habr.com/ru/post/426421/
	// Read the Command register
	// Читаем командный регистр
	// Write it back, setting bit 2 (the Reset bit)
	// Записываем его обратно, выставляя бит 2(Reset)
	// and making sure the two schedule Enable bits are clear.
	// и проверяем, что 2 очереди выключены
	EHCIx->USBCMD = (EHCIx->USBCMD & ~ (CMD_ASE | CMD_PSE)) | CMD_HCRESET;
	// A small delay here would be good. You don't want to read
	// Небольшая задержка здесь будет неплоха, Вы не должны читать
	// the register before it has a chance to actually set the bit
	// регистр перед тем, как у него не появится шанса выставить бит
	(void) EHCIx->USBCMD;
	// Now wait for the controller to clear the reset bit.
	// Ждем пока контроллер сбросит бит Reset
	while ((EHCIx->USBCMD & CMD_HCRESET) != 0)
		;
	// Again, a small delay here would be good to allow the
	// reset to actually become complete.
	// Опять задержка
	(void) EHCIx->USBCMD;

	USBH_POSTRESET_INIT();

#if WITHUSBHOST_HIGHSPEEDULPI
	//ulpi_chip_debug();
#endif /* WITHUSBHOST_HIGHSPEEDULPI */


	// wait for the halted bit to become set
	// Ждем пока бит Halted не будет выставлен
	while ((EHCIx->USBSTS & EHCI_USBSTS_HCH) == 0)
		;
	// Выделяем и выравниваем фрейм лист, пул для очередей и пул для дескрипторов
	// Замечу, что все мои дескрипторы и элементы очереди выровнены на границу 128 байт

	// Disable interrupts
	// Отключаем прерывания
	//hc->opRegs->usbIntr = 0;
	EHCIx->USBINTR = 0;

	/* подготовка кольцевого списка QH */
	for (i = 0; i < ARRAY_SIZE(hehci->asynclisthead); ++ i)
	{
		asynclist_item(& hehci->asynclisthead[i],
				ehci_link_qhv(
						& hehci->asynclisthead[(i + 1)
								% ARRAY_SIZE(hehci->asynclisthead)]), i == 0);
	}
	/* подготовка списка dts */
	for (i = 0; i < ARRAY_SIZE(hehci->qtds); ++ i)
	{
		//memset(& hehci->qtds [i], 0xFF, sizeof hehci->qtds [i]);
		hehci->qtds[i].status = EHCI_STATUS_HALTED;
	}
	/* подготовка списка QH для periodic frame list */
	for (i = 0; i < ARRAY_SIZE(hehci->itdsarray); ++ i)
	{
		asynclist_item(& hehci->itdsarray[i], EHCI_LINK_TERMINATE | EHCI_LINK_TYPE(1), 1);
	}

	dcache_clean_invalidate((uintptr_t) & hehci->asynclisthead, sizeof hehci->asynclisthead);
	dcache_clean_invalidate((uintptr_t) & hehci->itdsarray, sizeof hehci->itdsarray);
	dcache_clean_invalidate((uintptr_t) & hehci->qtds, sizeof hehci->qtds);
	/*
	 * Terminate (T). 1=Last QH (pointer is invalid). 0=Pointer is valid.
	 * If the queue head is in the context of the periodic list, a one bit in this field indicates to the host controller that
	 * this is the end of the periodic list. This bit is ignored by the host controller when the queue head is in the Asynchronous schedule.
	 * Software must ensure that queue heads reachable by the host controller always have valid horizontal link pointers. See Section 4.8.2
	 *
	 */

	// Periodic frame list
	for (i = 0; i < ARRAY_SIZE(hehci->periodiclist); ++i) {
		hehci->periodiclist[i].link = EHCI_LINK_TERMINATE;// 0 - valid, 1 - invalid
	}
	dcache_clean_invalidate((uintptr_t) & hehci->periodiclist, sizeof hehci->periodiclist);

	// Setup frame list
	// Устанавливаем ссылку на фреймлист
	//hc->opRegs->frameIndex = 0;
	EHCIx->FRINDEX = 0;
	//hc->opRegs->periodicListBase = (u32)(uintptr_t)hc->frameList;
	EHCIx->PERIODICLISTBASE = cpu_to_le32(virt_to_phys(& hehci->periodiclist));

	// копируем адрес асинхронной очереди в регистр
	//hc->opRegs->asyncListAddr = (u32)(uintptr_t)hc->asyncQH;
	const uint64_t asyncListAddr = cpu_to_le32(virt_to_phys(& hehci->asynclisthead));
	EHCIx->ASYNCLISTADDR = asyncListAddr;
	ASSERT(EHCIx->ASYNCLISTADDR == (asyncListAddr & 0xFFFFFFFF));
	// Устанавливаем сегмент в 0
	//hc->opRegs->ctrlDsSegment = 0;
	EHCIx->CTRLDSSEGMENT = cpu_to_le32(asyncListAddr >> 32);
	EHCIx->USBSTS = ~ cpu_to_le32(0);	// Clear status

	// Expected results (STM32MP1):
	//	1 HAL_EHCI_Init: PORTSC=00002000
	//	2 HAL_EHCI_Init: PORTSC=00000000
	//	3 HAL_EHCI_Init: PORTSC=00001000

	//PRINTF("1 HAL_EHCI_Init: PORTSC=%08X @%p\n", hehci->portsc [WITHEHCIHW_EHCIPORT], & hehci->portsc [WITHEHCIHW_EHCIPORT]);
	/* Route all ports to EHCI controller */
	//PRINTF("1 *hehci->configFlag=%u\n",(unsigned) *hehci->configFlag);
	* hehci->configFlag = EHCI_CONFIGFLAG_CF;
	(void) * hehci->configFlag;
	//PRINTF("2 *hehci->configFlag=%u\n",(unsigned) *hehci->configFlag);
	//PRINTF("2 HAL_EHCI_Init: PORTSC=%08X\n",hehci->portsc [WITHEHCIHW_EHCIPORT]);

	/* Enable power to all ports */
	unsigned porti = WITHEHCIHW_EHCIPORT;
	//for (porti = 0; porti < hehci->nports; ++ porti)
	{
		uint_fast32_t portsc = hehci->portsc[porti];

 		portsc &= ~ EHCI_PORTSC_OWNER;	// take ownership to EHCI - already zero after set EHCI_CONFIGFLAG_CF
		portsc &= ~ EHCI_PORTSC_CHANGE;
		portsc |= EHCI_PORTSC_PP;

		hehci->portsc[porti] = portsc;
		(void) hehci->portsc[porti];
	}
	/* Wait 20ms after potentially enabling power to a port */
	//local_delay_ms ( EHCI_PORT_POWER_DELAY_MS );
	local_delay_ms(50);
	//PRINTF("3 HAL_EHCI_Init: PORTSC=%08X\n", hehci->portsc [WITHEHCIHW_EHCIPORT]);

	// OHCI init

	if (hehci->ohci != NULL)
	{
		PRINTF("OHCI Init, hehci->ohci=%p\n", hehci->ohci);
		PRINTF("OHCI: HcRevision=%08X\n", le32_to_cpu(hehci->ohci->HcRevision));
		hehci->ohci->HcCommandStatus |= cpu_to_le32(1u << 0);	// HCR HostControllerReset - issue a software reset
		(void) hehci->ohci->HcCommandStatus;
		while ((le32_to_cpu(hehci->ohci->HcCommandStatus) & (1u << 0)) != 0)
			;

		hehci->ohci->HcCommandStatus = cpu_to_le32(1u << 3);	// OwnershipChangeRequest

		unsigned PowerOnToPowerGoodTime = ((le32_to_cpu(hehci->ohci->HcRhDescriptorA) >> 24) & 0xFF) * 2;
		PRINTF("OHCI: PowerOnToPowerGoodTime=%u\n", PowerOnToPowerGoodTime);

//		PRINTF("OHCI: HcCommandStatus=%08X\n", le32_to_cpu(hehci->ohci->HcCommandStatus));
//		PRINTF("OHCI: HcRevision=%08X\n", le32_to_cpu(hehci->ohci->HcRevision));
//		PRINTF("OHCI: HcControl=%08X\n", le32_to_cpu(hehci->ohci->HcControl));
//		PRINTF("OHCI: HcFmInterval=%08X\n", le32_to_cpu(hehci->ohci->HcFmInterval));
//		PRINTF("OHCI: HcRhDescriptorA=%08X\n", le32_to_cpu(hehci->ohci->HcRhDescriptorA));
//		PRINTF("OHCI: HcRhDescriptorB=%08X\n", le32_to_cpu(hehci->ohci->HcRhDescriptorB));
		PRINTF("OHCI: HcRhStatus=%08X\n", le32_to_cpu(hehci->ohci->HcRhStatus));
		PRINTF("OHCI: HcRhPortStatus[%d]=%08X\n", WITHOHCIHW_OHCIPORT, le32_to_cpu(hehci->ohci->HcRhPortStatus[WITHOHCIHW_OHCIPORT]));

		hehci->ohci->HcRhPortStatus[WITHOHCIHW_OHCIPORT] = cpu_to_le32(1u << 8); // PortPowerStatus
		local_delay_ms(PowerOnToPowerGoodTime);
//		OHCI: HcCommandStatus=00000000
//		OHCI: HcRevision=00000010
//		OHCI: HcControl=000000C0
//		OHCI: HcFmInterval=00002EDF
//		OHCI: HcRhDescriptorA=02001201
//		OHCI: HcRhDescriptorB=00000000
//		OHCI: HcRhPortStatus[0]=00000100
//		OHCI: HcRhPortStatus[1]=00000000

//		hehci->ohci->HcRhPortStatus[WITHOHCIHW_OHCIPORT] = cpu_to_le32(1u << 0); // PortEnableStatus

		PRINTF("init OHCI: HcRhStatus=%08X\n", le32_to_cpu(hehci->ohci->HcRhStatus));
		PRINTF("init OHCI: HcRhPortStatus[%d]=%08X\n", WITHOHCIHW_OHCIPORT, le32_to_cpu(hehci->ohci->HcRhPortStatus[WITHOHCIHW_OHCIPORT]));

		PRINTF("init OHCI: HcRevision=%08X\n", le32_to_cpu(hehci->ohci->HcRevision));
		hehci->ohci->HcRevision |= cpu_to_le32(1u << 5);	// BulkListEnable
		hehci->ohci->HcRevision |= cpu_to_le32(1u << 4);	// ControlListEnable
		//hehci->ohci->HcRevision |= cpu_to_le32(1u << 2);	// PeriodicListEnable
		PRINTF("init OHCI: HcRevision=%08X\n", le32_to_cpu(hehci->ohci->HcRevision));
	}

	return HAL_OK;
}

HAL_StatusTypeDef HAL_EHCI_DeInit(EHCI_HandleTypeDef *hehci)
{
	HAL_EHCI_MspDeInit(hehci);
	return HAL_OK;
}

void USBH_OHCI_IRQHandler(void)
{
	//ehci_bus_poll(& usbbus0);
	HAL_OHCI_IRQHandler(& hehci_USB);
}

void USBH_EHCI_IRQHandler(void)
{
	//ASSERT(0);
	//ehci_bus_poll(& usbbus0);
	HAL_EHCI_IRQHandler(& hehci_USB);
}

void ohci_disconnect_handler(void)
{
//	hehci_USB.ohci->HcCommandStatus = cpu_to_le32(1u << 3);	// OwnershipChangeRequest
	HAL_EHCI_Disconnect_Callback(& hehci_USB);
}

#if WITHUSBHOST_HIGHSPEEDULPI
#if CPUSTYLE_XC7Z && defined (WITHUSBHW_EHCI)
// https://xilinx.github.io/embeddedsw.github.io/usbps/doc/html/api/xusbps__hw_8h.html

//	USB ULPI Viewport Register (ULPIVIEW) bit positions.
#define XUSBPS_ULPIVIEW_DATWR_MASK   	0x000000FF	// 	ULPI Data Write. More...
#define	XUSBPS_ULPIVIEW_DATWR_SHIFT		0
#define XUSBPS_ULPIVIEW_DATRD_MASK   	0x0000FF00	// 	ULPI Data Read. More...
#define XUSBPS_ULPIVIEW_DATRD_SHIFT	   	8			// 	ULPI Data Address. More...
#define XUSBPS_ULPIVIEW_ADDR_MASK   	0x00FF0000	// 	ULPI Data Address. More...
#define XUSBPS_ULPIVIEW_ADDR_SHIFT	   	16			// 	ULPI Data Address. More...
#define XUSBPS_ULPIVIEW_PORT_MASK   	0x07000000	// 	ULPI Port Number. More...
#define XUSBPS_ULPIVIEW_SS_MASK   		0x08000000	// 	ULPI Synchronous State. More...
#define XUSBPS_ULPIVIEW_RW_MASK   		0x20000000	// 	ULPI Read/Write Control. More...
#define XUSBPS_ULPIVIEW_RUN_MASK   		0x40000000	// 	ULPI Run. More...
#define XUSBPS_ULPIVIEW_WU_MASK   		0x80000000	// 	ULPI Wakeup. More...

static void ulpi_reg_write(uint_fast8_t addr, uint_fast8_t data)
{
	XUSBPS_Registers * const USBx = EHCIxToUSBx(WITHUSBHW_EHCI);

	USBx->ULPIVIEW = (USBx->ULPIVIEW & ~ (XUSBPS_ULPIVIEW_ADDR_MASK | XUSBPS_ULPIVIEW_DATWR_MASK | XUSBPS_ULPIVIEW_RUN_MASK | XUSBPS_ULPIVIEW_RW_MASK)) |
			XUSBPS_ULPIVIEW_RW_MASK | 	// Select write direction
			(((uint_fast32_t) addr << XUSBPS_ULPIVIEW_ADDR_SHIFT) & XUSBPS_ULPIVIEW_ADDR_MASK) |
			(((uint_fast32_t) data << XUSBPS_ULPIVIEW_DATWR_SHIFT) & XUSBPS_ULPIVIEW_DATWR_MASK) |
			0;

	USBx->ULPIVIEW |= XUSBPS_ULPIVIEW_RUN_MASK;

	while ((USBx->ULPIVIEW & XUSBPS_ULPIVIEW_RUN_MASK) != 0)
		;
}

static uint_fast8_t ulpi_reg_read(uint_fast8_t addr)
{
	XUSBPS_Registers * const USBx = EHCIxToUSBx(WITHUSBHW_EHCI);

	USBx->ULPIVIEW = (USBx->ULPIVIEW & ~ (XUSBPS_ULPIVIEW_ADDR_MASK | XUSBPS_ULPIVIEW_DATWR_MASK | XUSBPS_ULPIVIEW_RUN_MASK | XUSBPS_ULPIVIEW_RW_MASK)) |
			(((uint_fast32_t) addr << XUSBPS_ULPIVIEW_ADDR_SHIFT) & XUSBPS_ULPIVIEW_ADDR_MASK) |
			0;

	USBx->ULPIVIEW |= XUSBPS_ULPIVIEW_RUN_MASK;

	while ((USBx->ULPIVIEW & XUSBPS_ULPIVIEW_RUN_MASK) != 0)
		;

	return (USBx->ULPIVIEW & XUSBPS_ULPIVIEW_DATRD_MASK) >> XUSBPS_ULPIVIEW_DATRD_SHIFT;
}
#endif /* CPUSTYLE_XC7Z */

void ulpi_chip_initialize(void)
{
	//return;
	// USB3340
	ulpi_reg_read(0x16);	/* Scratch Register - dummy read */

	// Address = 00h (read only) Vendor ID Low = 0x24
	// Address = 01h (read only) Vendor ID High = 0x04
	// Address = 02h (read only) Product ID Low = 0x09
	// Address = 03h (read only)  Product ID High = 0x00
	const uint_fast8_t v0 = ulpi_reg_read(0x00);
	const uint_fast8_t v1 = ulpi_reg_read(0x01);
	const uint_fast8_t v2 = ulpi_reg_read(0x02);
	const uint_fast8_t v3 = ulpi_reg_read(0x03);
	const uint_fast16_t vid = v1 * 256 + v0;
	const uint_fast16_t pid = v3 * 256 + v2;
	PRINTF("ulpi_chip_initialize: ULPI chip: VendorID=%04X, productID=%04X\n", (unsigned) vid, (unsigned) pid);

	ulpi_reg_write(0x16, 0xE5);	/* Scratch Register - dummy read */

//	if (vid != 0x0424 || pid != 0x0009)
//		return;

	//	7.1.1.5 Function Control
	//	Address = 04-06h (read), 04h (write), 05h (set), 06h (clear)
	//ulpi_reg_write(0x06, 0x03);	/* Function Control - XcvrSelect[1:0] = 00 00b: Enables HS transceiver */

}

void ulpi_chip_sethost(uint_fast8_t state)
{
	// USB3340
	return;

	// Address = 00h (read only) Vendor ID Low = 0x24
	// Address = 01h (read only) Vendor ID High = 0x04
	// Address = 02h (read only) Product ID Low = 0x09
	// Address = 03h (read only)  Product ID High = 0x00
	const uint_fast8_t v0 = ulpi_reg_read(0x00);
	const uint_fast8_t v1 = ulpi_reg_read(0x01);
	const uint_fast8_t v2 = ulpi_reg_read(0x02);
	const uint_fast8_t v3 = ulpi_reg_read(0x03);
	const uint_fast16_t vid = v1 * 256 + v0;
	const uint_fast16_t pid = v3 * 256 + v2;
	//PRINTF("ulpi_chip_sethost: ULPI chip: VendorID=%04X, productID=%04X\n", (unsigned) vid, (unsigned) pid);

	if (vid != 0x0424 || pid != 0x0009)
		return;

	//	7.1.1.7 OTG Control
	//	Address = 0A-0Ch (read), 0Ah (write), 0Bh (set), 0Ch (clear
	// 7.1.2.1 Carkit Control
	// Address = 19-1Bh (read), 19h (write), 1Ah (set), 1Bh (clear)

	if (state)
	{
		// Tie ID down
		ulpi_reg_write(0x0C, (0x01 << 0));	// Clear IdPullup bit
		ulpi_reg_write(0x1A, (0x01 << 1));	// Set IdGndDrv bit
	}
	else
	{
		// Tie ID up
		ulpi_reg_write(0x1B, (0x01 << 1));	// Clear IdGndDrv bit
		ulpi_reg_write(0x0B, (0x01 << 0));	// Set IdPullup bit
	}
	//PRINTF("ULPI chip: reg19=%02X\n", ulpi_reg_read(0x19));
}

void ulpi_chip_debug(void)
{
	PRINTF("sts=%08X, portsc=%08X\n", EHCIxToUSBx(WITHUSBHW_EHCI)->ISR, EHCIxToUSBx(WITHUSBHW_EHCI)->PORTSCR1);
	return;
	PRINTF("Function Control (0x04): %02X\n", 	ulpi_reg_read(0x04));
	PRINTF("Interface Control (0x07): %02X\n", 	ulpi_reg_read(0x07));
	PRINTF("OTG Control (0x0A): %02X\n", 		ulpi_reg_read(0x0A));
	PRINTF("USB Interrupt Enable Rising  (0x0D): %02X\n", ulpi_reg_read(0x0D));
	PRINTF("USB Interrupt Enable Falling  (0x10): %02X\n", ulpi_reg_read(0x10));
	PRINTF("USB Interrupt Status  (0x13): %02X\n", ulpi_reg_read(0x13));
	PRINTF("USB Interrupt Latch  (0x14): %02X\n", ulpi_reg_read(0x14));
	PRINTF("Debug  (0x15): %02X\n", ulpi_reg_read(0x15));
	PRINTF("Scratch Register  (0x16): %02X\n", ulpi_reg_read(0x16));
	PRINTF("Carkit Control  (0x19): %02X\n", ulpi_reg_read(0x19));
	PRINTF("Carkit Interrupt Enable  (0x1D): %02X\n", ulpi_reg_read(0x1D));
	PRINTF("Carkit Interrupt Status  (0x20): %02X\n", ulpi_reg_read(0x20));
	PRINTF("Carkit Interrupt Latch  (0x21): %02X\n", ulpi_reg_read(0x21));
	PRINTF("HS Compensation Register  (0x31): %02X\n", ulpi_reg_read(0x31));
	PRINTF("USB-IF Charger Detection  (0x32): %02X\n", ulpi_reg_read(0x32));
	PRINTF("Headset Audio Mode  (0x32): %02X\n", ulpi_reg_read(0x33));
}

void ulpi_chip_vbuson(uint_fast8_t state)
{
	return;
	// USB3340

	// Address = 00h (read only) Vendor ID Low = 0x24
	// Address = 01h (read only) Vendor ID High = 0x04
	// Address = 02h (read only) Product ID Low = 0x09
	// Address = 03h (read only)  Product ID High = 0x00
	const uint_fast8_t v0 = ulpi_reg_read(0x00);
	const uint_fast8_t v1 = ulpi_reg_read(0x01);
	const uint_fast8_t v2 = ulpi_reg_read(0x02);
	const uint_fast8_t v3 = ulpi_reg_read(0x03);
	const uint_fast16_t vid = v1 * 256 + v0;
	const uint_fast16_t pid = v3 * 256 + v2;
	//PRINTF("ulpi_chip_vbuson: ULPI chip: VendorID=%04X, productID=%04X\n", (unsigned) vid, (unsigned) pid);

//	if (vid != 0x0424 || pid != 0x0009)
//		return;

	//	7.1.1.7 OTG Control
	//	Address = 0A-0Ch (read), 0Ah (write), 0Bh (set), 0Ch (clear)
	//PRINTF("ULPI chip: reg0A=%02X\n", ulpi_reg_read(0x0A));
	if (state)
		ulpi_reg_write(0x0B, (0x01 << 6));	// Set DrvVbusExternal bit
	else
		ulpi_reg_write(0x0C, (0x01 << 6));	// Clear DrvVbusExternal bit
	PRINTF("ULPI chip: reg0A=%02X\n", ulpi_reg_read(0x0A));

}

#endif /* WITHUSBHOST_HIGHSPEEDULPI */


/**
  * @brief  Start the host driver.
  * @param  hehci EHCI handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_EHCI_Start(EHCI_HandleTypeDef *hehci)
{
 	USB_EHCI_CapabilityTypeDef * const EHCIx = (USB_EHCI_CapabilityTypeDef *) hehci->Instance;
 	// Enable controller
 	// Запускаем контроллер, 8 микро-фреймов, включаем
 	// последовательную и асинхронную очередь
 	//hc->opRegs->usbCmd = (8 << CMD_ITC_SHIFT) | CMD_PSE | CMD_ASE | CMD_RS;
     EHCIx->USBCMD =
      		//(8uL << CMD_ITC_SHIFT) |	// прерывание на 8 микро-фреймов (1 мс) - медленно
     		(1uL << CMD_ITC_SHIFT) |	// прерывание каждый микро-фреймо (125 мкс) - передача в 3 раза быстрее (0.4 -> 1.4 мегабайта в секунду)
 			((uint_fast32_t) EHCI_FLSIZE_DEFAULT << CMD_FLS_SHIFT)	| // Frame list size is 1024 elements
 			EHCI_USBCMD_PERIODIC |	 // Periodic Schedule Enable - PERIODICLISTBASE use
			EHCI_USBCMD_ASYNC |	// Asynchronous Schedule Enable - ASYNCLISTADDR use
 			//CMD_RS |	// Run/Stop 1=Run, 0-stop
 			0;

     EHCIx->USBCMD |= EHCI_USBCMD_RUN;	// 1=Run, 0-stop
 	(void) EHCIx->USBCMD;

  	while ((EHCIx->USBSTS & EHCI_USBSTS_HCH) != 0)
 		;

	ASSERT((EHCIx->USBSTS & EHCI_USBSTS_HCH) == 0);

#if ! WITHEHCIHWSOFTSPOLL
 	EHCIx->USBINTR |=
 			INTR_IOAA |	// Interrupt on ASync Advance Enable
			INTR_HSE |	// Host System Error Interrupt Enable
 			INTR_FLR |	// Frame List Rollower Interrupt Enable - требуется для опознания HIGH SPEED устройств
			INTR_PCD |	// Port Change Interrupt Enable
			INTR_ERROR |	// USB Error Interrupt Enable
			INTR_USBINT |	// USB Interrupt Enable
			0;
#else/* ! WITHEHCIHWSOFTSPOLL */
 	EHCIx->USBINTR = 0;
#endif /* ! WITHEHCIHWSOFTSPOLL */

	__HAL_LOCK(hehci);
	__HAL_EHCI_ENABLE(hehci);
	(void) EHCI_DriveVbus(hehci->Instance, 1U);
	__HAL_UNLOCK(hehci);

#if WITHTINYUSB
	tuh_init(BOARD_TUH_RHPORT);
#endif /* WITHTINYUSB */

	return HAL_OK;
}

/**
  * @brief  Stop the host driver.
  * @param  hehci EHCI handle
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_EHCI_Stop(EHCI_HandleTypeDef *hehci)
{
	__HAL_LOCK(hehci);
	(void) EHCI_StopHost(hehci->Instance);

	__HAL_UNLOCK(hehci);

	return HAL_OK;
}


/**
  * @brief  Submit a new URB for processing.
  * @param  hehci EHCI handle
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
static HAL_StatusTypeDef HAL_EHCI_HC_SubmitRequest(EHCI_HandleTypeDef *hehci,
                                           uint8_t ch_num,
                                           uint8_t direction,
                                           uint8_t ep_type,
                                           uint8_t token,
                                           uint8_t *pbuff,
										   uint32_t length,
                                           uint8_t do_ping)
{
	EHCI_HCTypeDef * const hc = & hehci->hc [ch_num];

	hc->ep_is_in = direction;
	hc->ep_type = ep_type;

	if (token == 0U)
	{
		//hc->data_pid = HC_PID_SETUP;
		hc->do_ping = do_ping;
	}
	else
	{
		//hc->data_pid = HC_PID_DATA1;
	}

	/* Manage Data Toggle */
	switch (ep_type)
	{
	case EP_TYPE_CTRL:
		if ((token == 1U) && (direction == 0U)) /*send data */
		{
//			if (length == 0U)
//			{
//				/* For Status OUT stage, Length==0, Status Out PID = 1 */
//				hc->toggle_out = 1U;
//			}
//
//			/* Set the Data Toggle bit as per the Flag */
//			if (hc->toggle_out == 0U)
//			{
//				/* Put the PID 0 */
//				hc->data_pid = HC_PID_DATA0;
//			}
//			else
//			{
//				/* Put the PID 1 */
//				hc->data_pid = HC_PID_DATA1;
//			}
		}
		break;

	case EP_TYPE_BULK:
		if (direction == 0U)
		{
//			/* Set the Data Toggle bit as per the Flag */
//			if (hc->toggle_out == 0U)
//			{
//				/* Put the PID 0 */
//				hc->data_pid = HC_PID_DATA0;
//			}
//			else
//			{
//				/* Put the PID 1 */
//				hc->data_pid = HC_PID_DATA1;
//			}
		}
		else
		{
//			if (hc->toggle_in == 0U)
//			{
//				hc->data_pid = HC_PID_DATA0;
//			}
//			else
//			{
//				hc->data_pid = HC_PID_DATA1;
//			}
		}

		break;
	case EP_TYPE_INTR:
		if (direction == 0U)
		{
//			/* Set the Data Toggle bit as per the Flag */
//			if (hc->toggle_out == 0U)
//			{
//				/* Put the PID 0 */
//				hc->data_pid = HC_PID_DATA0;
//			}
//			else
//			{
//				/* Put the PID 1 */
//				hc->data_pid = HC_PID_DATA1;
//			}
		}
		else
		{
//			if (hc->toggle_in == 0U)
//			{
//				hc->data_pid = HC_PID_DATA0;
//			}
//			else
//			{
//				hc->data_pid = HC_PID_DATA1;
//			}
		}
		break;

	case EP_TYPE_ISOC:
//		hc->data_pid = HC_PID_DATA0;
		break;

	default:
		ASSERT(0);
		break;
	}

	ghc = hc;

	hc->xfer_buff = pbuff;
	hc->xfer_len = length;
	hc->ehci_urb_state = URB_IDLE;
	hc->xfer_count = 0U;
	hc->ch_num = ch_num;
	hc->state = HC_IDLE;

	//PRINTF("HAL_EHCI_HC_SubmitRequest: ch_num=%u, ep_num=%u, max_packet=%u, do_ping=%d\n",  hc->ch_num, hc->ep_num, hc->max_packet, do_ping);
	//return USB_HC_StartXfer(hehci->Instance, & hehci->hc[ch_num], (uint8_t)hehci->Init.dma_enable);
//
//  	  struct ehci_queue_head * head0 = ( struct ehci_queue_head *) async;
//	memset ( head0, 0, sizeof ( *head0 ) );
//	head0->chr = cpu_to_le32 ( EHCI_CHR_HEAD );
//	head0->cache.next = cpu_to_le32 ( EHCI_LINK_TERMINATE );
//	head0->cache.status = EHCI_STATUS_HALTED;


	const  int isintr = 0;//hc->ep_type == EP_TYPE_INTR;
	volatile struct ehci_queue_head * const qh = & hehci->asynclisthead [ch_num];
	volatile struct ehci_transfer_descriptor * const qtdarray = & hehci->qtds [ch_num];
	volatile struct ehci_transfer_descriptor * const qtdrequest = isintr ? & hehci->itdsarray [ch_num].cache : & hehci->asynclisthead [ch_num].cache;

	switch (ep_type)
	{
	case EP_TYPE_CTRL:
		if (token == 0)
		{
			// Setup
//			PRINTF("HAL_EHCI_HC_SubmitRequest: SETUP, pbuff=%p, length=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", hc->xfer_buff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
//			PRINTF("HAL_EHCI_HC_SubmitRequest: ch_num=%u, ep_num=%u, max_packet=%u, tt_hub=%d, tt_prt=%d, speed=%d\n", hc->ch_num, hc->ep_num, hc->max_packet, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed);
			//printhex(0, pbuff, hc->xfer_len);

			VERIFY(0 == qtd_item2_buff(qtdrequest, hc->xfer_buff, hc->xfer_len));
			qtd_item2(qtdrequest, EHCI_FL_PID_SETUP, do_ping);
			dcache_clean((uintptr_t) hc->xfer_buff, hc->xfer_len);

			// бит toggle хранится в памяти overlay и модифицируется сейчас в соответствии с требовании для SETUP запросов
			qtd_item2_set_toggle(qtdrequest, 0);

		}
		else if (direction == 0)
		{
			// Data OUT
//			PRINTF("HAL_EHCI_HC_SubmitRequest: OUT, pbuff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", pbuff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
//			PRINTF("HAL_EHCI_HC_SubmitRequest: ch_num=%u, ep_num=%u, max_packet=%u, tt_hub=%d, tt_prt=%d, speed=%d\n", hc->ch_num, hc->ep_num, hc->max_packet, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed);
			//printhex(0, pbuff, hc->xfer_len);

			VERIFY(0 == qtd_item2_buff(qtdrequest, hc->xfer_buff, hc->xfer_len));
			qtd_item2(qtdrequest, EHCI_FL_PID_OUT, do_ping);
			dcache_clean((uintptr_t) hc->xfer_buff, hc->xfer_len);

			// бит toggle хранится в памяти overlay и модифицируется сейчас в соответствии с требовании для SETUP запросов
			qtd_item2_set_toggle(qtdrequest, 1);

		}
		else
		{
			// Data In
//			PRINTF("HAL_EHCI_HC_SubmitRequest: IN, hc->xfer_buff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", hc->xfer_buff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
//			PRINTF("HAL_EHCI_HC_SubmitRequest: ch_num=%u, ep_num=%u, max_packet=%u, tt_hub=%d, tt_prt=%d, speed=%d\n", hc->ch_num, hc->ep_num, hc->max_packet, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed);

			VERIFY(0 == qtd_item2_buff(qtdrequest, hc->xfer_buff, hc->xfer_len));
			qtd_item2(qtdrequest, EHCI_FL_PID_IN, 0);
			dcache_clean_invalidate((uintptr_t) hc->xfer_buff, hc->xfer_len);

			// бит toggle хранится в памяти overlay и модифицируется сейчас в соответствии с требовании для SETUP запросов
			qtd_item2_set_toggle(qtdrequest, 1);
		}
		break;

		// See also USBH_EP_BULK - используется host library для
		// see also USB_EP_TYPE_BULK
		// https://www.usbmadesimple.co.uk/ums_7.htm#high_speed_bulk_trans
	case EP_TYPE_BULK:
		if (hc->ep_is_in == 0)
		{
			// BULK Data OUT
//			PRINTF("HAL_EHCI_HC_SubmitRequest: BULK OUT, hc->xfer_buff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%dd\n",
//					hc->xfer_buff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
//			PRINTF("HAL_EHCI_HC_SubmitRequest: ch_num=%u, ep_num=%u, max_packet=%u, tt_hub=%d, tt_prt=%d, speed=%d\n", hc->ch_num, hc->ep_num, hc->max_packet, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed);
			//printhex((uintptr_t) hc->xfer_buff, hc->xfer_buff, hc->xfer_len);

			VERIFY(0 == qtd_item2_buff(qtdrequest, hc->xfer_buff, hc->xfer_len));
			qtd_item2(qtdrequest, EHCI_FL_PID_OUT, do_ping);
			dcache_clean((uintptr_t) hc->xfer_buff, hc->xfer_len);

			// бит toggle хранится в памяти overlay и модифицируется самим контроллером
		}
		else
		{
			// BULK Data IN
//			PRINTF("HAL_EHCI_HC_SubmitRequest: BULK IN, hc->xfer_buff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n",
//					hc->xfer_buff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
//			PRINTF("HAL_EHCI_HC_SubmitRequest: ch_num=%u, ep_num=%u, max_packet=%u, tt_hub=%d, tt_prt=%d, speed=%d\n", hc->ch_num, hc->ep_num, hc->max_packet, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed);

			VERIFY(0 == qtd_item2_buff(qtdrequest, hc->xfer_buff, hc->xfer_len));
			qtd_item2(qtdrequest, EHCI_FL_PID_IN, 0);
			dcache_clean_invalidate((uintptr_t) hc->xfer_buff, hc->xfer_len);

			// бит toggle хранится в памяти overlay и модифицируется самим контроллером
		}
		break;

	case EP_TYPE_INTR:
		//le8_modify(& qtdrequest->status, 0x04, 1 * 0x04);
		if (hc->ep_is_in == 0)
		{
			// INTERRUPT Data OUT
//			PRINTF("HAL_EHCI_HC_SubmitRequest: INTERRUPT OUT, pbuff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", pbuff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
//			PRINTF("HAL_EHCI_HC_SubmitRequest: ch_num=%u, ep_num=%u, max_packet=%u, tt_hub=%d, tt_prt=%d, speed=%d\n", hc->ch_num, hc->ep_num, hc->max_packet, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed);
			VERIFY(0 == qtd_item2_buff(qtdrequest, hc->xfer_buff, hc->xfer_len));
			qtd_item2(qtdrequest, EHCI_FL_PID_OUT, do_ping);
			dcache_clean((uintptr_t) hc->xfer_buff, hc->xfer_len);

			// бит toggle хранится в памяти overlay и модифицируется самим контроллером
		}
		else
		{
			// INTERRUPT Data IN
//			PRINTF("HAL_EHCI_HC_SubmitRequest: INTERRUPT IN, pbuff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", pbuff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
//			PRINTF("HAL_EHCI_HC_SubmitRequest: ch_num=%u, ep_num=%u, max_packet=%u, tt_hub=%d, tt_prt=%d, speed=%d\n", hc->ch_num, hc->ep_num, hc->max_packet, hc->tt_hubaddr, hc->tt_prtaddr, hc->speed);
			VERIFY(0 == qtd_item2_buff(qtdrequest, hc->xfer_buff, hc->xfer_len));
			qtd_item2(qtdrequest, EHCI_FL_PID_IN, 1);
			dcache_clean_invalidate((uintptr_t) hc->xfer_buff, hc->xfer_len);

			// бит toggle хранится в памяти overlay и модифицируется самим контроллером
		}
		break;

	default:
		ASSERT(0);
		break;
	}

	le8_modify(& qtdrequest->status, EHCI_STATUS_MASK, EHCI_STATUS_ACTIVE);

	if (isintr == 0)
	{
		/* для того, чобы не срабатывало преждевременно - убрать после перехода на списки работающих пересылок */
		le8_modify(& qtdarray->status, EHCI_STATUS_MASK, EHCI_STATUS_ACTIVE);

		asynclist_item2(hc, qh, virt_to_phys(qtdarray), hc->ch_num == 0);

	}
	else
	{
		/* для того, чобы не срабатывало преждевременно - убрать после перехода на списки работающих пересылок */
		le8_modify(& qtdarray->status, EHCI_STATUS_MASK, EHCI_STATUS_ACTIVE);

		asynclist_item2(hc, qh, virt_to_phys(qtdarray), 1);

		hehci->periodiclist [ch_num].link = cpu_to_le32(virt_to_phys(qtdarray));
	}

	return HAL_OK;
}

/**
  * @brief  Return  URB state for a channel.
  * @param  hehci EHCI handle
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
EHCI_URBStateTypeDef HAL_EHCI_HC_GetURBState(EHCI_HandleTypeDef *hehci, uint8_t chnum)
{
	return hehci->hc [chnum].ehci_urb_state;
}


/**
  * @brief  Return the last host transfer size.
  * @param  hehci EHCI handle
  * @param  chnum Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval last transfer size in byte
  */
uint32_t HAL_EHCI_HC_GetXferCount(EHCI_HandleTypeDef *hehci, uint8_t chnum)
{
	return hehci->hc [chnum].xfer_count;
}

uint32_t HAL_EHCI_HC_GetMaxPacket(EHCI_HandleTypeDef *hehci, uint8_t chnum)
{
	return hehci->hc [chnum].max_packet;
}



/**
  * @brief  Returns the USB status depending on the HAL status:
  * @param  hal_status: HAL status
  * @retval USB status
  */
static USBH_StatusTypeDef USBH_Get_USB_Status(HAL_StatusTypeDef hal_status)
{
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
 * @brief  Submit a new URB to the low level driver.
 * @param  phost: Host handle
 * @param  pipe: Pipe index
 *         This parameter can be a value from 1 to 15
 * @param  direction : Channel number
 *          This parameter can be one of the these values:
 *           0 : Output
 *           1 : Input
 * @param  ep_type : Endpoint Type
 *          This parameter can be one of the these values:
 *            @arg EP_TYPE_CTRL: Control type
 *            @arg EP_TYPE_ISOC: Isochrounous type
 *            @arg EP_TYPE_BULK: Bulk type
 *            @arg EP_TYPE_INTR: Interrupt type
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
USBH_StatusTypeDef USBH_LL_SubmitURB(USBH_HandleTypeDef *phost, uint8_t pipe,
		uint8_t direction, uint8_t ep_type, uint8_t token, uint8_t *pbuff,
		uint32_t length, uint8_t do_ping)
{
	EHCI_HandleTypeDef * const hehci = phost->pData;
	USB_EHCI_CapabilityTypeDef * const EHCIx = hehci->Instance;

	HAL_StatusTypeDef hal_status = HAL_OK;
	USBH_StatusTypeDef usb_status = USBH_OK;


	//PRINTF("USBH_LL_SubmitURB: direction=%d, ep_type=%d, token=%d\n", direction, ep_type, token);
	//printhex(0, pbuff, length);

	// TODO: use EHCI_USBCMD_ASYNC_ADVANCE

	EHCI_StopAsync(EHCIx);

	hal_status = HAL_EHCI_HC_SubmitRequest(phost->pData, pipe, direction ,
								 ep_type, token, pbuff, length,
								 do_ping);

	dcache_clean_invalidate((uintptr_t) & hehci->periodiclist, sizeof hehci->periodiclist);
	dcache_clean_invalidate((uintptr_t) & hehci->asynclisthead, sizeof hehci->asynclisthead);
	dcache_clean_invalidate((uintptr_t) & hehci->qtds, sizeof hehci->qtds);

	EHCI_StartAsync(EHCIx);

	usb_status =  USBH_Get_USB_Status(hal_status);

	return usb_status;	// везде игнорируется.
}

/**
 * @brief  Get a URB state from the low level driver.
 * @param  phost: Host handle
 * @param  pipe: Pipe index
 *         This parameter can be a value from 1 to 15
 * @retval URB state
 *          This parameter can be one of the these values:
 *            @arg URB_IDLE
 *            @arg URB_DONE - nex state
 *            @arg URB_NOTREADY = USBH_URB_NOTREADY = end waiting
 *            @arg URB_NYET
 *            @arg URB_ERROR - USBH_URB_ERROR = end waiting
 *            @arg URB_STALL
 */
USBH_URBStateTypeDef USBH_LL_GetURBState(USBH_HandleTypeDef *phost,
		uint8_t pipe)
{
	EHCI_HandleTypeDef * const hehci = phost->pData;

	system_disableIRQ();
	SPIN_LOCK(& hehci->asynclock);
	HAL_EHCI_IRQHandler(& hehci_USB);
	HAL_OHCI_IRQHandler(& hehci_USB);
	SPIN_UNLOCK(& hehci->asynclock);
	system_enableIRQ();

#if WITHINTEGRATEDDSP
	audioproc_spool_user();		// решение проблем с прерыванием звука при записи файлов
#endif /* WITHINTEGRATEDDSP */

	EHCI_URBStateTypeDef state2 = HAL_EHCI_HC_GetURBState(phost->pData, pipe);
	EHCI_URBStateTypeDef state;
	do
	{
		state = state2;
		state2 = HAL_EHCI_HC_GetURBState(phost->pData, pipe);
	} while (state != state2);
	return (USBH_URBStateTypeDef) state2;
}

USBH_SpeedTypeDef USBH_LL_GetPipeSpeed(USBH_HandleTypeDef *phost, uint8_t pipe_num)
{
	EHCI_HandleTypeDef *hehci = phost->pData;
	return hehci->hc [pipe_num].speed;

}

uint_fast8_t USBH_LL_GetSpeedReady(USBH_HandleTypeDef *phost) {
	return 1; 	//HAL_EHCI_GetCurrentSpeedReady(phost->pData);
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
USBH_StatusTypeDef USBH_LL_DriverVBUS(USBH_HandleTypeDef *phost, uint8_t state) {
	//PRINTF(PSTR("USBH_LL_DriverVBUS(%d), phost->id=%d, HOST_FS=%d\n"), (int) state, (int) phost->id, (int) HOST_FS);
	if (state != FALSE) {
		/* Drive high Charge pump */
		/* ToDo: Add IOE driver control */
		board_set_usbhostvbuson(1);
		board_update();
	} else {
		/* Drive low Charge pump */
		/* ToDo: Add IOE driver control */
		board_set_usbhostvbuson(0);
		board_update();
	}
	HARDWARE_DELAY_MS(200);
	return USBH_OK;
}

/**
 * @brief  Set toggle for a pipe.
 * @param  phost: Host handle
 * @param  pipe: Pipe index
 * @param  toggle: toggle (0/1)
 * @retval Status
 */
USBH_StatusTypeDef USBH_LL_SetToggle(USBH_HandleTypeDef *phost, uint8_t ch_num,
		uint8_t toggle) {
	EHCI_HandleTypeDef *pHandle;
	pHandle = phost->pData;
	EHCI_HandleTypeDef * const hehci = phost->pData;
	ASSERT(pHandle != NULL);
	USB_EHCI_CapabilityTypeDef * const EHCIx = (USB_EHCI_CapabilityTypeDef*) pHandle->Instance;

//	if (pHandle->hc[ch_num].ep_is_in) {
//		pHandle->hc[ch_num].toggle_in = toggle;
//	} else {
//		pHandle->hc[ch_num].toggle_out = toggle;
//	}

	EHCI_StopAsync(EHCIx);

	EHCI_HCTypeDef * const hc = & hehci->hc [ch_num];
	const  int isintr = 0;//hc->ep_type == EP_TYPE_INTR;
	volatile struct ehci_transfer_descriptor * const qtdrequest = isintr ? & hehci->qtds [ch_num] : & hehci->asynclisthead [ch_num].cache;
	qtd_item2_set_toggle(qtdrequest, toggle);
	dcache_clean_invalidate((uintptr_t) & hehci->asynclisthead, sizeof hehci->asynclisthead);
	dcache_clean_invalidate((uintptr_t) & hehci->periodiclist, sizeof hehci->periodiclist);

	EHCI_StartAsync(EHCIx);

	return USBH_OK;
}

/**
 * @brief  Return the current toggle of a pipe.
 * @param  phost: Host handle
 * @param  pipe: Pipe index
 * @retval toggle (0/1)
 */
uint8_t USBH_LL_GetToggle(USBH_HandleTypeDef *phost, uint8_t ch_num) {
	EHCI_HandleTypeDef * const hehci = phost->pData;
	uint8_t toggle = 0;
//	EHCI_HandleTypeDef *pHandle;
//	pHandle = phost->pData;
//	ASSERT(pHandle != NULL);

//	if (pHandle->hc[pipe].ep_is_in) {
//		toggle = pHandle->hc[pipe].toggle_in;
//	} else {
//		toggle = pHandle->hc[pipe].toggle_out;
//	}

	EHCI_HCTypeDef * const hc = & hehci->hc [ch_num];
	const  int isintr = 0;//hc->ep_type == EP_TYPE_INTR;
	volatile struct ehci_transfer_descriptor * const qtdrequest = isintr ? & hehci->qtds [ch_num] : & hehci->asynclisthead [ch_num].cache;
	toggle = (le16_to_cpu(qtdrequest->len) & EHCI_LEN_TOGGLE) != 0;

	return toggle;
}

/**
  * @brief  Return the USB host speed from the low level driver.
  * @param  phost: Host handle
  * @retval USBH speeds
  */
USBH_SpeedTypeDef USBH_LL_GetSpeed(USBH_HandleTypeDef *phost)
{
	USBH_SpeedTypeDef speed = USBH_SPEED_HIGH;
//
//  switch (HAL_EHCI_GetCurrentSpeed(phost->pData))
//  {
//  case 0 :
//    speed = USBH_SPEED_HIGH;
//    break;
//
//  case 1 :
//    speed = USBH_SPEED_FULL;
//    break;
//
//  case 2 :
//    speed = USBH_SPEED_LOW;
//    break;
//
//  default:
//   speed = USBH_SPEED_FULL;
//    break;
//  }
//
	/* Determine port speed */
	EHCI_HandleTypeDef * const hehci = phost->pData;
	const uint_fast32_t portsc = hehci->portsc [WITHEHCIHW_EHCIPORT];
	const unsigned ccs = (portsc & EHCI_PORTSC_CCS);
	const unsigned line = EHCI_PORTSC_LINE_STATUS(portsc);
	const unsigned ped = (portsc & EHCI_PORTSC_PED) != 0;
#if CPUSTYLE_XC7Z
	// Use XUSBPS_PORTSCR1
	switch ((portsc >> 26) & 0x03)	// PORTSCR_PSPD
	{
	case 0x00:
		speed = USBH_SPEED_FULL;
		PRINTF("speed=USBH_SPEED_FULL, portsc=%08X\n", portsc);
		break;
	case 0x01:
		speed = USBH_SPEED_LOW;
		PRINTF("speed=USBH_SPEED_LOW, portsc=%08X\n", portsc);
		break;
	case 0x02:
		speed = USBH_SPEED_HIGH;
		PRINTF("speed=USBH_SPEED_HIGH, portsc=%08X\n", portsc);
		break;
	case 0x03:
		speed = USBH_SPEED_HIGH;
		PRINTF("speed=not connected, portsc=%08X\n", portsc);
		break;
	}
#else /* CPUSTYLE_XC7Z */
	if ( ! ccs)
	{
		/* Port not connected */
		//speed = USB_SPEED_NONE;
		PRINTF("speed=USB_SPEED_NONE, portsc=%08X\n", portsc);
	}
	else if (line == EHCI_PORTSC_LINE_STATUS_LOW)
	{
		/* Detected as low-speed */
		speed = USBH_SPEED_LOW;
		PRINTF("speed=USB_SPEED_LOW, portsc=%08X\n", portsc);
	}
	else if (ped)
	{
		/* Port already enabled: must be high-speed */
		speed = USBH_SPEED_HIGH;
		PRINTF("speed=USB_SPEED_HIGH, portsc=%08X\n", portsc);
	}
	else
	{
		/* Not low-speed and not yet enabled.  Could be either
		 * full-speed or high-speed; we can't yet tell.
		 */
		speed = USBH_SPEED_FULL;
		PRINTF("speed=USB_SPEED_FULL, portsc=%08X\n", portsc);
	}

	if (speed != USBH_SPEED_HIGH && hehci->ohci != 0)
	{
		ASSERT(hehci->ohci != NULL);
		// передать управление портом к companion controller (OHCI)
		uint_fast32_t portsc = hehci->portsc [WITHEHCIHW_EHCIPORT];
		portsc |= EHCI_PORTSC_OWNER;
		hehci->portsc [WITHEHCIHW_EHCIPORT] = portsc;
		(void) hehci->portsc [WITHEHCIHW_EHCIPORT];

		PRINTF("OHCI: HcRhStatus=%08X\n", le32_to_cpu(hehci->ohci->HcRhStatus));
		PRINTF("OHCI: HcRhPortStatus[%d]=%08X\n", WITHOHCIHW_OHCIPORT, le32_to_cpu(hehci->ohci->HcRhPortStatus[WITHOHCIHW_OHCIPORT]));
		unsigned PowerOnToPowerGoodTime = ((le32_to_cpu(hehci->ohci->HcRhDescriptorA) >> 24) & 0xFF) * 2;
//		hehci->ohci->HcCommandStatus = cpu_to_le32(1u << 3);	// OwnershipChangeRequest
//		while ((hehci->ohci->HcControl & cpu_to_le32(1u << 3)) == 0)
//			;


//		hehci->ohci->HcRhPortStatus[WITHOHCIHW_OHCIPORT] = cpu_to_le32(1u << 1);    // pes

		PRINTF("OHCI: HcRhStatus=%08X\n", le32_to_cpu(hehci->ohci->HcRhStatus));
		PRINTF("OHCI: HcRhPortStatus[%d]=%08X\n", WITHOHCIHW_OHCIPORT, le32_to_cpu(hehci->ohci->HcRhPortStatus[WITHOHCIHW_OHCIPORT]));
		local_delay_ms(PowerOnToPowerGoodTime);
//		hehci->ohci->HcRhPortStatus[WITHOHCIHW_OHCIPORT] = cpu_to_le32(1u << 0); // PortEnableStatus
		local_delay_ms(PowerOnToPowerGoodTime);

		PRINTF("OHCI: HcRhStatus=%08X\n", le32_to_cpu(hehci->ohci->HcRhStatus));
		PRINTF("OHCI: HcRhPortStatus[%d]=%08X\n", WITHOHCIHW_OHCIPORT, le32_to_cpu(hehci->ohci->HcRhPortStatus[WITHOHCIHW_OHCIPORT]));

//		hehci->ohci->HcRhPortStatus[WITHOHCIHW_OHCIPORT] = cpu_to_le32(1u << 4);	// port reset
//		while ((hehci->ohci->HcRhPortStatus[WITHOHCIHW_OHCIPORT] & cpu_to_le32(1u << 4)) != 0)
//			;
		PRINTF("OHCI: %s Speed device attached\n", (le32_to_cpu(hehci->ohci->HcRhPortStatus[WITHOHCIHW_OHCIPORT]) & (1u << 9)) ? "Low" : "Full");
	}
#endif /* CPUSTYLE_XC7Z */
	//PRINTF("USBH_LL_GetSpeed: EHCI_PORTSC_OWNER=%d\n", !! (hehci->portsc [WITHEHCIHW_EHCIPORT] & EHCI_PORTSC_OWNER));
	return speed;
}

/**
  * @brief  USBH_LL_ResetPort2
  *         Reset the Host Port of the Low Level Driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_ResetPort2(USBH_HandleTypeDef *phost, unsigned resetIsActive)	/* Without delays */
{

	HAL_StatusTypeDef hal_status = HAL_OK;
	USBH_StatusTypeDef usb_status = USBH_OK;
	//
	//	  hal_status = HAL_EHCI_ResetPort2(phost->pData, resetIsActive);
	//
	EHCI_HandleTypeDef * const hehci = phost->pData;
	USB_EHCI_CapabilityTypeDef * const EHCIx = hehci->Instance;
//	PRINTF("USBH_LL_ResetPort2: 1 active=%d, : USBCMD=%08X USBSTS=%08X PORTSC[%u]=%08X\n", (int) resetIsActive, EHCIx->USBCMD, EHCIx->USBSTS, WITHEHCIHW_EHCIPORT, hehci->portsc [WITHEHCIHW_EHCIPORT]);

	if (resetIsActive)
	{
 		uint_fast32_t portsc = hehci->portsc [WITHEHCIHW_EHCIPORT];
 		portsc &= ~ EHCI_PORTSC_OWNER;	// take ownership to EHCI
		/* Reset port */
 		portsc &= ~ (EHCI_PORTSC_PED | EHCI_PORTSC_CHANGE);
 		portsc |= EHCI_PORTSC_PR;

 		hehci->portsc [WITHEHCIHW_EHCIPORT] = portsc;
 		(void) hehci->portsc [WITHEHCIHW_EHCIPORT];
	}
	else
	{
		uint_fast32_t portsc = hehci->portsc [WITHEHCIHW_EHCIPORT];
 		portsc &= ~ EHCI_PORTSC_OWNER;	// take ownership to EHCI
 		/* Release Reset port */
 		portsc &= ~ EHCI_PORTSC_PR;	 /** Port reset */

 		hehci->portsc [WITHEHCIHW_EHCIPORT] = portsc;
 		(void) hehci->portsc [WITHEHCIHW_EHCIPORT];
#if CPUSTYLE_XC7Z
		local_delay_ms(1000);
		//HAL_Delay(5);
#endif /* CPUSTYLE_XC7Z */
	}
	//PRINTF("USBH_LL_ResetPort2: 2 active=%d, : USBCMD=%08X USBSTS=%08X PORTSC[%u]=%08X\n", (int) resetIsActive, EHCIx->USBCMD, EHCIx->USBSTS, WITHEHCIHW_EHCIPORT, hehci->portsc [WITHEHCIHW_EHCIPORT]);
	//PRINTF("USBH_LL_ResetPort2: EHCI_PORTSC_OWNER=%d\n", !! (hehci->portsc [WITHEHCIHW_EHCIPORT] & EHCI_PORTSC_OWNER));

	usb_status = USBH_Get_USB_Status(hal_status);

	return usb_status;
}


/**
  * @brief  Return the last transferred packet size.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @retval Packet size
  */
uint32_t USBH_LL_GetLastXferSize(USBH_HandleTypeDef *phost, uint8_t pipe)
{
	uint32_t size2 = HAL_EHCI_HC_GetXferCount(phost->pData, pipe);
	uint32_t size;
	do
	{
		size = size2;
		size2 = HAL_EHCI_HC_GetXferCount(phost->pData, pipe);
	} while (size != size2);
	return size2;
}

/**
  * @brief  Return the maximum possible transferred packet size.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @param  size: expectes transfer packet size
  * @retval Packet size
  */
uint32_t USBH_LL_GetAdjXferSize(USBH_HandleTypeDef *phost, uint8_t pipe, uint32_t size)
{
	  //return ulmin32(size, HAL_EHCI_HC_GetMaxPacket(phost->pData, pipe));	// Default implementation
	  return ulmin32(size, 4 * 4096uL);		/* 16 кБ - предел того что можно описать сегментами в transfer descriptor */
}

/**
  * @brief  Open a pipe of the low level driver.
  * @param  phost: Host handle
  * @param  pipe_num: Pipe index
  * @param  epnum: Endpoint number
  * @param  dev_address: Device USB address
  * @param  speed: Device Speed
  * @param  ep_type: Endpoint type
  * @param  mps: Endpoint max packet size
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_OpenPipe(USBH_HandleTypeDef *phost, uint8_t pipe_num, uint8_t epnum,
					const USBH_TargetTypeDef * dev_target,
					uint8_t ep_type,
					uint16_t mps)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;
	EHCI_HandleTypeDef * const hehci = phost->pData;
	USB_EHCI_CapabilityTypeDef * const EHCIx = hehci->Instance;

  hal_status = HAL_EHCI_HC_Init(phost->pData, pipe_num, epnum,
		  dev_target->dev_address, dev_target->speed, ep_type, mps, dev_target->tt_hubaddr, dev_target->tt_prtaddr);

  ////hal_status =  ehci_endpoint_open(& usbdev0->control) == 0 ? HAL_OK : HAL_ERROR;
  usb_status = USBH_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Close a pipe of the low level driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_ClosePipe(USBH_HandleTypeDef *phost, uint8_t pipe)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;

  //hal_status = HAL_EHCI_HC_Halt(phost->pData, pipe);	// TODO: разобраться с остановкой ASYNC опроса

 ////ehci_endpoint_close(& usbdev0->control);
  usb_status = USBH_Get_USB_Status(hal_status);

  return usb_status;
}

/**
  * @brief  Delay routine for the USB Host Library
  * @param  Delay: Delay in ms
  * @retval None
  */
void USBH_Delay(uint32_t Delay)
{
	//HAL_Delay(Delay);
	local_delay_ms(Delay);
}

/**
  * @brief  Initialize the low level portion of the host driver.
  * @param  phost: Host handle
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_Init(USBH_HandleTypeDef *phost)
{

	hehci_USB.pData = phost;
	phost->pData = & hehci_USB;

	hehci_USB.Instance = WITHUSBHW_EHCI;
#if defined (WITHUSBHW_OHCI)
	hehci_USB.ohci = (struct ohci_registers *) WITHUSBHW_OHCI;
#else /* defined (WITHUSBHW_OHCI) */
	hehci_USB.ohci = NULL;
#endif /* defined (WITHUSBHW_OHCI) */

	hehci_USB.Init.Host_channels = 16;
	hehci_USB.Init.speed = PCD_SPEED_HIGH;
	hehci_USB.Init.dma_enable = ENABLE;
	hehci_USB.Init.phy_itface = USB_OTG_EMBEDDED_PHY;

	hehci_USB.Init.Sof_enable = DISABLE;

	if (HAL_EHCI_Init(& hehci_USB) != HAL_OK)
	{
		ASSERT(0);
	}

	USBH_LL_SetTimer(phost, HAL_EHCI_GetCurrentFrame(& hehci_USB));
	return USBH_OK;
}

/**
  * @brief  De-Initialize the low level portion of the host driver.
  * @param  phost: Host handle
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_DeInit(USBH_HandleTypeDef *phost)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBH_StatusTypeDef usb_status = USBH_OK;

	hal_status = HAL_EHCI_DeInit(phost->pData);

	usb_status = USBH_Get_USB_Status(hal_status);
	phost->pData = NULL;

	return usb_status;
}

/**
  * @brief  Start the low level portion of the host driver.
  * @param  phost: Host handle
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_Start(USBH_HandleTypeDef *phost)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBH_StatusTypeDef usb_status = USBH_OK;

	hal_status = HAL_EHCI_Start(phost->pData);

	usb_status = USBH_Get_USB_Status(hal_status);

	return usb_status;
}

/**
  * @brief  Stop the low level portion of the host driver.
  * @param  phost: Host handle
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_Stop(USBH_HandleTypeDef *phost)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBH_StatusTypeDef usb_status = USBH_OK;

	hal_status = HAL_EHCI_Stop(phost->pData);

	usb_status = USBH_Get_USB_Status(hal_status);

	return usb_status;
}

/* User-mode function */
void MX_USB_HOST_Init(void)
{
	static ticker_t usbticker;
	/* Init Host Library,Add Supported Class and Start the library*/
	USBH_Init(& hUsbHostHS, USBH_UserProcess, 0);

#if WITHUSEUSBFLASH
	USBH_RegisterClass(& hUsbHostHS, & USBH_msc);
#endif /* WITHUSEUSBFLASH */
#if 1
	USBH_RegisterClass(& hUsbHostHS, & HUB_Class);
	USBH_RegisterClass(& hUsbHostHS, & HID_Class);
#endif /* WITHUSEUSBFLASH */
	//ticker_initialize(& usbticker, 1, board_usb_tspool, NULL);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
	//ticker_add(& usbticker);

}

/* User-mode function */
void MX_USB_HOST_DeInit(void)
{
	USBH_DeInit(& hUsbHostHS);
}

/* User-mode function */
void MX_USB_HOST_Process(void)
{
	EHCI_HandleTypeDef * const hehci = hUsbHostHS.pData;
	USBH_Process(& hUsbHostHS);

	system_disableIRQ();
	SPIN_LOCK(& hehci->asynclock);
	HAL_EHCI_IRQHandler(& hehci_USB);
	HAL_OHCI_IRQHandler(& hehci_USB);
	SPIN_UNLOCK(& hehci->asynclock);
	system_enableIRQ();
#if WITHTINYUSB
    tuh_task();
#endif /* WITHTINYUSB */
}

#endif /* defined (WITHUSBHW_EHCI) */

#endif /* WITHUSBHW && WITHEHCIHW */


#if WITHUSBHW && WITHEHCIHW


#if defined (WITHUSBHW_EHCI)

#include "ehci.h"

void HAL_EHCI_MspInit(EHCI_HandleTypeDef * hehci)
{
#if CPUSTYLE_A64

	// xfel boot
	//	USBPHY_CFG_REG: 00000101
	//	BUS_CLK_GATING_REG0: 00800000
	//	BUS_SOFT_RST_REG0: 00800000

	PRINTF("USBPHY_CFG_REG: %08X\n", (unsigned) CCU->USBPHY_CFG_REG);
	PRINTF("BUS_CLK_GATING_REG0: %08X\n", (unsigned) CCU->BUS_CLK_GATING_REG0);
	PRINTF("BUS_SOFT_RST_REG0: %08X\n", (unsigned) CCU->BUS_SOFT_RST_REG0);

	PRINTF("USBPHY0->HCI_ICR: %08X\n", (unsigned) USBPHY0->HCI_ICR);
	PRINTF("USBPHY1->HCI_ICR: %08X\n", (unsigned) USBPHY1->HCI_ICR);

	USBPHY0->HCI_ICR |= 0x01;
	USBPHY1->HCI_ICR |= 0x01;

	PRINTF("USBPHY0->HCI_ICR: %08X\n", (unsigned) USBPHY0->HCI_ICR);
	PRINTF("USBPHY1->HCI_ICR: %08X\n", (unsigned) USBPHY1->HCI_ICR);

	CCU->USBPHY_CFG_REG = (CCU->USBPHY_CFG_REG & ~ (
				(3u << 16) |	// SCLK_GATING_OHCI - 11:OTG-OHCI and OHCI0 Clock is ON
				(1u << 11) |
				(1u << 10) |
				(1u << 2) |
			0)) |
			(3u << 16) |	// SCLK_GATING_OHCI - 11:OTG-OHCI and OHCI0 Clock is ON
			(1u << 11) |	// SCLK_GATING_12M  Gating Special 12M Clock For HSIC
			(1u << 10) |	// SCLK_GATING_HSIC Gating Special Clock For HSIC
			(1u << 2) |	// USBHSIC_RST
			0;

	const unsigned OHCIx_12M_SRC_SEL = 0u;	// 00: 12M divided from 48M, 01: 12M divided from 24M, 10: LOSC
	if ((void *) WITHUSBHW_EHCI == USBEHCI1)
	{
		PRINTF("Enable USBEHCI1 clocks\n");
		ASSERT((void *) WITHUSBHW_EHCI == USBEHCI1);	/* host-only port */

		CCU->USBPHY_CFG_REG = (CCU->USBPHY_CFG_REG & ~ (
					(3u << 22) |	// OHCI1_12M_SRC_SEL
					(1u << 9) |
					(1u << 1) |
					0)) |
				(OHCIx_12M_SRC_SEL << 22) |	// OHCI1_12M_SRC_SEL
				(1u << 9) |	// SCLK_GATING_USBPHY1
				(1u << 1) |	// USBPHY1_RST
				0;

		// USBEHCI1, USBOHCI1 - 0x01C1B000
		CCU->BUS_CLK_GATING_REG0 |= (1u << 29);	// USBOHCI0_GATING.
		CCU->BUS_CLK_GATING_REG0 |= (1u << 25);	// USBEHCI0_GATING.

		CCU->BUS_SOFT_RST_REG0 |= (1u << 29);	// USB-OHCI0_RST.
		CCU->BUS_SOFT_RST_REG0 |= (1u << 25);	// USB-EHCI0_RST.

		SetupUsbPhyc(USBPHY1);
	}
	else
	{
		ASSERT((void *) WITHUSBHW_EHCI == USBEHCI0);	/* host and usb-otg port */

		CCU->USBPHY_CFG_REG = (CCU->USBPHY_CFG_REG & ~ (
					(3u << 20) |	// OHCI0_12M_SRC_SEL
					(1u << 8) |	// SCLK_GATING_USBPHY0 - xfel boot setup
					(1u << 0) |	// USBPHY0_RST
					0)) |
				(OHCIx_12M_SRC_SEL << 20) |	// OHCI0_12M_SRC_SEL
				(1u << 8) |	// SCLK_GATING_USBPHY0 - xfel boot setup
				(1u << 0) |	// USBPHY0_RST - xfel boot setup
				0;

		CCU->BUS_CLK_GATING_REG0 |= (1u << 28);	// USB-OTG-OHCI_GATING.
		CCU->BUS_CLK_GATING_REG0 |= (1u << 24);	// USB-OTG-EHCI_GATING.
		//CCU->BUS_CLK_GATING_REG0 |= (1u << 23);	// USB-OTG-Device_GATING.	- xfel boot setup

		CCU->BUS_SOFT_RST_REG0 |= (1u << 28);	// USB-OTG-OHCI_RST.
		CCU->BUS_SOFT_RST_REG0 |= (1u << 24);	// USB-OTG-EHCI_RST
		//CCU->BUS_SOFT_RST_REG0 |= (1u << 23);	// USB-OTG-Device_RST.	- xfel boot setup

		SetupUsbPhyc(USBPHY0);

	}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133)

//	PRINTF("From boot: allwnrt113_get_pll_peri_800M_freq=%lu\n", allwnrt113_get_pll_peri_800M_freq());

	if (EHCIxToUSBPHYC(WITHUSBHW_EHCI) == USBPHY0)
	{
		// Turn off USBOTG0
		CCU->USB_BGR_REG &= ~ (1u << 24);	// USBOTG0_RST
		CCU->USB_BGR_REG &= ~ (1u << 8);	// USBOTG0_GATING

		// Enable
		CCU->USB0_CLK_REG |= (1u << 31);	// USB0_CLKEN - Gating Special Clock For OHCI0
		CCU->USB0_CLK_REG |= (1u << 30);	// USBPHY0_RSTN

		CCU->USB_BGR_REG |= (1u << 16);	// USBOHCI0_RST
		CCU->USB_BGR_REG |= (1u << 20);	// USBEHCI0_RST

		CCU->USB_BGR_REG |= (1u << 0);	// USBOHCI0_GATING
		CCU->USB_BGR_REG |= (1u << 4);	// USBEHCI0_GATING


		// OHCI0 12M Source Select
		CCU->USB0_CLK_REG = (CCU->USB0_CLK_REG & ~ (0x03 << 24)) |
			(0x01 << 24) | 	// 00: 12M divided from 48 MHz 01: 12M divided from 24 MHz 10: RTC_32K
			0;

		USBOTG0->PHY_OTGCTL &= ~ (1uL << 0); 	// Host mode. Route phy0 to EHCI/OHCI

	#if WITHEHCIHWSOFTSPOLL == 0
		arm_hardware_set_handler_system(USB0_OHCI_IRQn, USBH_OHCI_IRQHandler);
		arm_hardware_set_handler_system(USB0_EHCI_IRQn, USBH_EHCI_IRQHandler);
	#endif /* WITHEHCIHWSOFTSPOLL == 0 */
	}
	else
	{
		CCU->USB1_CLK_REG |= (1u << 31);	// USB1_CLKEN
		CCU->USB1_CLK_REG |= (1u << 30);	// USBPHY1_RSTN

		CCU->USB_BGR_REG |= (1u << 17);	// USBOHCI1_RST
		CCU->USB_BGR_REG |= (1u << 21);	// USBEHCI1_RST

		CCU->USB_BGR_REG |= (1u << 1);	// USBOHCI1_GATING
		CCU->USB_BGR_REG |= (1u << 5);	// USBEHCI1_GATING

		// OHCI0 12M Source Select
		CCU->USB1_CLK_REG = (CCU->USB1_CLK_REG & ~ (0x03 << 24)) |
			(0x01 << 24) | 	// 00: 12M divided from 48 MHz 01: 12M divided from 24 MHz 10: RTC_32K
			0;

	#if WITHEHCIHWSOFTSPOLL == 0
		arm_hardware_set_handler_system(USB1_OHCI_IRQn, USBH_OHCI_IRQHandler);
		arm_hardware_set_handler_system(USB1_EHCI_IRQn, USBH_EHCI_IRQHandler);
	#endif /* WITHEHCIHWSOFTSPOLL == 0 */
	}

	if (EHCIxToUSBPHYC(WITHUSBHW_EHCI) == USBPHY1)
	{
		// USBPHY1
		USBPHY1->USB_CTRL |= (1uL << 0);	// 1: Enable UTMI interface, disable ULPI interface
		USBPHY1->USB_CTRL |=
				(1uL << 11) |	// 1: Use INCR16 when appropriate
				(1uL << 10) |	// 1: Use INCR8 when appropriate
				(1uL << 9) |	// 1: Use INCR4 when appropriate
				(1uL << 8) |	// 1: Start INCRx burst only on burst x-align address Note: This bit must enable if any bit of bit[11:9] is enabled
				0;

		USBPHY1->PHY_CTRL &= ~ (1uL << 3); 	// SIDDQ 0: Write 0 to enable phy
	}
	else
	{
		// USBPHY0

		// https://github.com/guanglun/r329-linux/blob/d6dced5dc9353fad5319ef5fb84e677e2b9a96b4/arch/arm64/boot/dts/allwinner/sun50i-r329.dtsi#L462
		//	/* A83T specific control bits for PHY0 */
		//	#define PHY_CTL_VBUSVLDEXT		BIT(5)
		//	#define PHY_CTL_SIDDQ			BIT(3)
		//	#define PHY_CTL_H3_SIDDQ		BIT(1)

		USBPHY0->USB_CTRL = 0x4300FC00;	// после запуска из QSPI было 0x40000000
		// Looks like 9.6.6.24 0x0810 PHY Control Register (Default Value: 0x0000_0008)
		//USB0_PHY->PHY_CTRL = 0x20;			// после запуска из QSPI было 0x00000008 а из загрузчика 0x00020
		USBPHY0->PHY_CTRL &= ~ (1uL << 3);		// PHY_CTL_SIDDQ
		USBPHY0->PHY_CTRL |= (1uL << 5);		// PHY_CTL_VBUSVLDEXT
		USBPHY0->USB_CTRL |= (1uL << 0);		// 1: Enable UTMI interface, disable ULPI interface

	}

#elif CPUSTYLE_STM32MP1

	USBD_EHCI_INITIALIZE();
	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_USBHEN;
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6LPENSETR = RCC_MP_AHB6LPENSETR_USBHLPEN;
	(void) RCC->MP_AHB6LPENSETR;

	{
		/* SYSCFG clock enable */
		RCC->MP_APB3ENSETR = RCC_MP_APB3ENSETR_SYSCFGEN;
		(void) RCC->MP_APB3ENSETR;
		RCC->MP_APB3LPENSETR = RCC_MP_APB3LPENSETR_SYSCFGLPEN;
		(void) RCC->MP_APB3LPENSETR;
		/*
		 * Interconnect update : select master using the port 1.
		 * MCU interconnect (USBH) = AXI_M1, AXI_M2.
		 */
		//SYSCFG->ICNR |= SYSCFG_ICNR_AXI_M1;
		(void) SYSCFG->ICNR;
		//SYSCFG->ICNR |= SYSCFG_ICNR_AXI_M2;
		(void) SYSCFG->ICNR;
	}

	USB_HS_PHYCInit();

#if WITHEHCIHWSOFTSPOLL == 0
	arm_hardware_set_handler_system(USBH_OHCI_IRQn, USBH_OHCI_IRQHandler);
	arm_hardware_set_handler_system(USBH_EHCI_IRQn, USBH_EHCI_IRQHandler);
#endif /* WITHEHCIHWSOFTSPOLL == 0 */

#elif CPUSTYLE_XC7Z

		XUSBPS_Registers * const USBx = EHCIxToUSBx(WITHUSBHW_EHCI);
		enum {  SRCSEL_SHIFT = 4 };
		const unsigned SRCSEL_MASK = (7u << SRCSEL_SHIFT);
		if (WITHUSBHW_EHCI == EHCI0)
		{
			enum { usbIX = 0 };
			//PRINTF("HAL_EHCI_MspInit: EHCI0\n");

			SCLR->SLCR_UNLOCK = 0x0000DF0DU;
			SCLR->APER_CLK_CTRL |= (1u << (usbIX + 2));	// APER_CLK_CTRL.USB0_CPU_1XCLKACT

			SCLR->USB0_CLK_CTRL = (SCLR->USB0_CLK_CTRL & ~ SRCSEL_MASK) |
				(0x04uL << SRCSEL_SHIFT) |	// SRCSEL
				0;
			(void) SCLR->USB0_CLK_CTRL;

	#if WITHUSBHOST_HIGHSPEEDULPI
			ulpi_chip_initialize();
			ulpi_chip_sethost(1);
			TARGET_USBFS_VBUSON_SET(1);
			//ulpi_chip_debug();
	#endif /* WITHUSBHOST_HIGHSPEEDULPI */

			SCLR->USB_RST_CTRL |= (1u << usbIX);
			(void) SCLR->USB_RST_CTRL;
			SCLR->USB_RST_CTRL &= ~ (1u << usbIX);
			(void) SCLR->USB_RST_CTRL;

			//USBH_POSTRESET_INIT();

#if WITHEHCIHWSOFTSPOLL == 0
			arm_hardware_set_handler_system(USB0_IRQn, USBH_EHCI_IRQHandler);
#endif /* WITHEHCIHWSOFTSPOLL == 0 */
		}
		else if (WITHUSBHW_EHCI == EHCI1)
		{
			enum { usbIX = 1 };
			//PRINTF("HAL_EHCI_MspInit: EHCI1\n");

			SCLR->SLCR_UNLOCK = 0x0000DF0DU;
			SCLR->APER_CLK_CTRL |= (1u << (usbIX + 2));	// APER_CLK_CTRL.USB1_CPU_1XCLKACT

			SCLR->USB1_CLK_CTRL = (SCLR->USB1_CLK_CTRL & ~ SRCSEL_MASK) |
				(0x04uL << SRCSEL_SHIFT) |	// SRCSEL
				0;
			(void) SCLR->USB1_CLK_CTRL;

	#if WITHUSBHOST_HIGHSPEEDULPI
			ulpi_chip_initialize();
			ulpi_chip_sethost(1);
			TARGET_USBFS_VBUSON_SET(1);
			//ulpi_chip_debug();
	#endif /* WITHUSBHOST_HIGHSPEEDULPI */

			SCLR->USB_RST_CTRL |= (1u << usbIX);
			(void) SCLR->USB_RST_CTRL;
			SCLR->USB_RST_CTRL &= ~ (1u << usbIX);
			(void) SCLR->USB_RST_CTRL;

			//USBH_POSTRESET_INIT();

#if WITHEHCIHWSOFTSPOLL == 0
			arm_hardware_set_handler_system(USB1_IRQn, USBH_EHCI_IRQHandler);
#endif /* WITHEHCIHWSOFTSPOLL == 0 */
		}
		else
		{
			ASSERT(0);
		}

#else

	#warning HAL_EHCI_MspInit Not implemented for CPUSTYLE_xxxxx

#endif
}

void HAL_EHCI_MspDeInit(EHCI_HandleTypeDef * hehci)
{
#if CPUSTYLE_A64

	if ((void *) WITHUSBHW_EHCI == USBEHCI1)
	{
		// USBEHCI1, USBOHCI1
		CCU->BUS_CLK_GATING_REG0 &= ~ (1u << 29);	// USBOHCI0_GATING.
		CCU->BUS_CLK_GATING_REG0 &= ~ (1u << 25);	// USBEHCI0_GATING.

		CCU->BUS_SOFT_RST_REG0 &= ~ (1u << 29);	// USB-OHCI0_RST.
		CCU->BUS_SOFT_RST_REG0 &= ~ (1u << 25);	// USB-EHCI0_RST.

	}
	else
	{
		CCU->BUS_CLK_GATING_REG0 &= ~ (1u << 28);	// USB-OTG-OHCI_GATING.
		CCU->BUS_CLK_GATING_REG0 &= ~ (1u << 24);	// USB-OTG-EHCI_GATING.
		CCU->BUS_CLK_GATING_REG0 &= ~ (1u << 23);	// USB-OTG-Device_GATING.

		CCU->BUS_SOFT_RST_REG0 &= ~ (1u << 28);	// USB-OTG-OHCI_RST.
		CCU->BUS_SOFT_RST_REG0 &= ~ (1u << 24);	// USB-OTG-EHCI_RST
		CCU->BUS_SOFT_RST_REG0 &= ~ (1u << 23);	// USB-OTG-Device_RST.

		//CCU->USBPHY_CFG_REG

	}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133)

	if (EHCIxToUSBPHYC(WITHUSBHW_EHCI) == USBPHY0)
	{
		ASSERT(0);					/* тут нет EHCI */

		CCU->USB_BGR_REG &= ~ (1u << 0);	// USBOHCI0_GATING
	#if WITHEHCIHWSOFTSPOLL == 0
		arm_hardware_disable_handler(USB0_OHCI_IRQn);
		arm_hardware_disable_handler(USB0_EHCI_IRQn);
	#endif /* WITHEHCIHWSOFTSPOLL == 0 */

		CCU->USB_BGR_REG &= ~ (1u << 4);	// USBEHCI0_GATING
		//CCU->USB_BGR_REG &= ~ (1u << 8);	// USBOTG0_GATING
		CCU->USB_BGR_REG &= ~ (1u << 16);	// USBOHCI0_RST
		CCU->USB_BGR_REG &= ~ (1u << 20);	// USBEHCI0_RST
		CCU->USB_BGR_REG &= ~ (1u << 24);	// USBOTG0_RST

		CCU->USB0_CLK_REG &= ~ (1u << 30);	// USBPHY0_RSTN
		CCU->USB0_CLK_REG &= ~ (1u << 31);	// USB0_CLKEN - Gating Special Clock For OHCI0
	}
	else
	{
	#if WITHEHCIHWSOFTSPOLL == 0
		arm_hardware_disable_handler(USB1_OHCI_IRQn);
		arm_hardware_disable_handler(USB1_EHCI_IRQn);
	#endif /* WITHEHCIHWSOFTSPOLL == 0 */

		CCU->USB_BGR_REG &= ~ (1u << 1);	// USBOHCI1_GATING
		CCU->USB_BGR_REG &= ~ (1u << 5);	// USBEHCI1_GATING
		CCU->USB_BGR_REG &= ~ (1u << 17);	// USBOHCI1_RST
		CCU->USB_BGR_REG &= ~ (1u << 21);	// USBEHCI1_RST

		CCU->USB1_CLK_REG &= ~ (1u << 30);	// USBPHY1_RSTN
		CCU->USB1_CLK_REG &= ~ (1u << 31);	// USB1_CLKEN
	}

#elif CPUSTYLE_STM32MP1

#if WITHEHCIHWSOFTSPOLL == 0
	arm_hardware_disable_handler(USBH_OHCI_IRQn);
	arm_hardware_disable_handler(USBH_EHCI_IRQn);
#endif /* WITHEHCIHWSOFTSPOLL == 0 */

	/* Perform USBH reset */
	RCC->AHB6RSTSETR = RCC_AHB6RSTSETR_USBHRST;
	(void) RCC->AHB6RSTSETR;
	RCC->AHB6RSTCLRR = RCC_AHB6RSTCLRR_USBHRST;
	(void) RCC->AHB6RSTCLRR;

	/* Clock Off */
	RCC->MP_AHB6LPENCLRR = RCC_MP_AHB6LPENCLRR_USBHLPEN;
	(void) RCC->MP_AHB6ENCLRR;
	RCC->MP_AHB6ENCLRR = RCC_MP_AHB6ENCLRR_USBHEN;
	(void) RCC->MP_AHB6ENCLRR;

#elif CPUSTYLE_XC7Z

		if (WITHUSBHW_EHCI == EHCI0)
		{
			enum { usbIX = 0 };

			arm_hardware_disable_handler(USB0_IRQn);

			SCLR->USB_RST_CTRL |= (1u << usbIX);
			(void) SCLR->USB_RST_CTRL;

		}
		else if (WITHUSBHW_EHCI == EHCI1)
		{
			enum { usbIX = 1 };

			arm_hardware_disable_handler(USB1_IRQn);

			SCLR->USB_RST_CTRL |= (1u << usbIX);
			(void) SCLR->USB_RST_CTRL;
		}
		else
		{
			ASSERT(0);
		}


#else

	#warning HAL_EHCI_MspDeInit Not implemented for CPUSTYLE_xxxxx

#endif
}

#endif /* defined (WITHUSBHW_EHCI) */

#endif /* WITHUSBHW && WITHEHCIHW */
