/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021, Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "hardware.h"

#if WITHTINYUSB
#include "formats.h"
#include "gpio.h"

#include "buffers.h"

#include "tusb.h"
#include "host/usbh.h"

#include "src/usb/usb200.h"
#include "src/usb/usbch9.h"
#include "usbd_def.h"

#if TUP_USBIP_OHCI && defined WITHUSBHW_OHCI
// Enable USB interrupt
void hcd_int_enable(uint8_t rhport)
{
	arm_hardware_enable_handler(WITHUSBHW_OHCI_IRQ);
}

// Disable USB interrupt
void hcd_int_disable(uint8_t rhport)
{
	arm_hardware_disable_handler(WITHUSBHW_OHCI_IRQ);
}
#endif /* TUP_USBIP_OHCI && defined WITHUSBHW_OHCI */


bool hcd_dcache_clean(void const* addr, uint32_t data_size) { dcache_clean((uintptr_t) addr, data_size); return true; }
bool hcd_dcache_invalidate(void const* addr, uint32_t data_size) { dcache_invalidate((uintptr_t) addr, data_size); return true; }
bool hcd_dcache_clean_invalidate(void const* addr, uint32_t data_size) { dcache_clean_invalidate((uintptr_t) addr, data_size); return true; }

void osal_task_delay(uint32_t msec) {
//  const uint32_t start = hcd_frame_number(_usbh_controller);
//  while ( ( hcd_frame_number(_usbh_controller) - start ) < msec ) {}
	local_delay_ms(msec);
}

//--------------------------------------------------------------------+
// Host HID
//--------------------------------------------------------------------+

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use. tuh_hid_parse_report_descriptor()
// can be used to parse common/simple enough descriptor.
// Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE, it will be skipped
// therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len)
{
  (void)desc_report;
  (void)desc_len;

  // Interface protocol (hid_interface_protocol_enum_t)
  const char* protocol_str[] = { "None", "Keyboard", "Mouse" };
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);

//  char tempbuf[256];
//  int count = sprintf(tempbuf, "[%04x:%04x][%u] HID Interface%u, Protocol = %s\r\n", vid, pid, dev_addr, instance, protocol_str[itf_protocol]);
//
//  tud_cdc_write(tempbuf, (uint32_t) count);
//  tud_cdc_write_flush();

  // Receive report from boot keyboard & mouse only
  // tuh_hid_report_received_cb() will be invoked when report is available
  if (itf_protocol == HID_ITF_PROTOCOL_KEYBOARD || itf_protocol == HID_ITF_PROTOCOL_MOUSE)
  {
    if ( !tuh_hid_receive_report(dev_addr, instance) )
    {
      //tud_cdc_write_str("Error: cannot request report\r\n");
    }
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
//  char tempbuf[256];
//  int count = sprintf(tempbuf, "[%u] HID Interface%u is unmounted\r\n", dev_addr, instance);
//  tud_cdc_write(tempbuf, (uint32_t) count);
//  tud_cdc_write_flush();
}

// look up new key in previous keys
static inline bool find_key_in_report(hid_keyboard_report_t const *report, uint8_t keycode)
{
  for(uint8_t i=0; i<6; i++)
  {
    if (report->keycode[i] == keycode)  return true;
  }

  return false;
}


// convert hid keycode to ascii and print via usb device CDC (ignore non-printable)
static void process_kbd_report(uint8_t dev_addr, hid_keyboard_report_t const *report)
{
  (void) dev_addr;
  static hid_keyboard_report_t prev_report = { 0, 0, {0} }; // previous report to check key released
  bool flush = false;

  for(uint8_t i=0; i<6; i++)
  {
    uint8_t keycode = report->keycode[i];
    if ( keycode )
    {
      if ( find_key_in_report(&prev_report, keycode) )
      {
        // exist in previous report means the current key is holding
      }else
      {
        // not existed in previous report means the current key is pressed

        // remap the key code for Colemak layout
        #ifdef KEYBOARD_COLEMAK
        uint8_t colemak_key_code = colemak[keycode];
        if (colemak_key_code != 0) keycode = colemak_key_code;
        #endif
        PRINTF("[%u] keycode=0x%02X\n", dev_addr, keycode);

//        bool const is_shift = report->modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT);
//        uint8_t ch = keycode2ascii[keycode][is_shift ? 1 : 0];
//
//        if (ch)
//        {
//          if (ch == '\n') tud_cdc_write("\r", 1);
//          tud_cdc_write(&ch, 1);
//          flush = true;
//        }
      }
    }
    // TODO example skips key released
  }

//  if (flush) tud_cdc_write_flush();

  prev_report = *report;
}

// send mouse report to usb device CDC
static void process_mouse_report(uint8_t dev_addr, hid_mouse_report_t const * report)
{
  //------------- button state  -------------//
  //uint8_t button_changed_mask = report->buttons ^ prev_report.buttons;
  char l = report->buttons & MOUSE_BUTTON_LEFT   ? 'L' : '-';
  char m = report->buttons & MOUSE_BUTTON_MIDDLE ? 'M' : '-';
  char r = report->buttons & MOUSE_BUTTON_RIGHT  ? 'R' : '-';

//  char tempbuf[32];
//  int count = sprintf(tempbuf, "[%u] %c%c%c %d %d %d\r\n", dev_addr, l, m, r, report->x, report->y, report->wheel);
//
//  tud_cdc_write(tempbuf, (uint32_t) count);
//  tud_cdc_write_flush();
  PRINTF("[%u] %c%c%c %d %d %d\n", dev_addr, l, m, r, report->x, report->y, report->wheel);
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) len;
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

  switch(itf_protocol)
  {
    case HID_ITF_PROTOCOL_KEYBOARD:
      process_kbd_report(dev_addr, (hid_keyboard_report_t const*) report );
    break;

    case HID_ITF_PROTOCOL_MOUSE:
      process_mouse_report(dev_addr, (hid_mouse_report_t const*) report );
    break;

    default: break;
  }

  // continue to request to receive report
  if ( !tuh_hid_receive_report(dev_addr, instance) )
  {
    //tud_cdc_write_str("Error: cannot request report\r\n");
  }
}

#if CFG_TUH_ENABLED && (defined (TUP_USBIP_OHCI) || defined (TUP_USBIP_EHCI))


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
/*
 * https://electronix.ru/forum/topic/179658-stm32h725-v-lqfp100-kto-to-zapuskal-s-vneshney-fizikoy-usb-hs-ulpi/?do=findComment&comment=1911585
 *
	Чтобы USB334x определялся как high-speed нужно в регистре DCFG выставить в 1 бит XCVRDLY.
	Без этого оно определяется как full-speed (причина описана в errata USB3340, решение найдено на буржуйских форумах).
	Для F2xx, F4xx, F7xx все то же самое, причем этот бит в заголовочном файле CMSIS не описан,
	хотя в руководстве пользователя F7xx, H7xx он есть.
*/

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

//	PHY 0x1c1b800 before:
//	01C1B800  00000000 00000001 00000000 00000000 00000002 00000000 023438E4 00000053
//	PHY 0x1c1b800 after:
//	01C1B800  00000701 00000001 00000000 00000000 00000000 00000000 023438E4 00000053

static void SetupHostUsbPhyc(USBPHYC_TypeDef * phy)
{
#if 0
	volatile uint32_t * base = (volatile uint32_t *) phy;

	PRINTF("PHY %p before:\n", phy);
	printhex32((uintptr_t) phy, phy, 0x20);
	base [0] = 0x00000701;	// 0x000 !!!! влияет
//	base [1] = 0x00000001;	// 0x004
//	base [2] = 0x00000000;	// 0x008
//	base [3] = 0x00000000;	// 0x00C
	base [4] = 0x00000000;	// 0x010 !!!! влияет
//	base [5] = 0x00000000;	// 0x014
//	base [6] = 0x023438e6;	// 0x018
//	base [7] = 0x00000053;	// 0x01C

	phy->HCI_ICR = 0x00000701;
	phy->REG_10 = 0x00000000;
	PRINTF("PHY %p after:\n", phy);
	printhex32((uintptr_t) phy, phy, 0x20);
#endif

	phy->HCI_ICR = 0x00000701;
	phy->PHY_CTRL = 0x00000000;

	return;
	phy->HCI_ICR |= (UINT32_C(1) << 0);

//	phy->HCI_ICR |= (UINT32_C(1) << 1);			// С этим битом не видит connect HSIC - This meaning is only valid when the controller is HCI1
//	phy->HCI_ICR |= (UINT32_C(1) << 17);

	phy->HCI_ICR |= (UINT32_C(1) << 20);		// EHCI HS force

	phy->HCI_ICR |= (0x0Fu << 8);	//

	//PRINTF("phy->HCI_ICR: %08X\n", (unsigned) phy->HCI_ICR);
}

#elif (CPUSTYLE_T507 || CPUSTYLE_H616)

//USBPHYC_TypeDef * EHCIxToUSBPHYC(void * p)
//{
//	USBPHYC_TypeDef * const PHYCx = (WITHUSBHW_EHCI == USBEHCI1) ? USBPHY1 : USBPHY0;
//    return PHYCx;
//}


//	PHY 0x5310800 before:
//	05310800  00000000 00000001 00000000 00000000 00000000 00000000 023438E4 00000053
//	PHY 0x5310800 after:
//	05310800  00000701 00000001 00000000 00000000 00000000 00000000 023438E4 00000053
//	PHY 0x5311800 before:
//	05311800  00000000 00000101 00000000 00000000 00000008 00000000 023438E4 00000053
//	PHY 0x5311800 after:
//	05311800  00000701 00000001 00000000 00000000 00000000 00000000 023438E4 00000053

void SetupHostUsbPhyc(USBPHYC_TypeDef * phy)
{
#if 0
	volatile uint32_t * base = (volatile uint32_t *) phy;

	PRINTF("PHY %p before:\n", phy);
	printhex32((uintptr_t) phy, phy, 0x20);
	base [0] = 0x00000701;	// 0x000
//	base [1] = 0x00000001;	// 0x004
//	base [2] = 0x00000000;	// 0x008
//	base [3] = 0x00000000;	// 0x00C
	base [4] = 0x00000000;	// 0x010
//	base [5] = 0x00000000;	// 0x014
//	base [6] = 0x023438e6;	// 0x018
//	base [7] = 0x00000053;	// 0x01C
	PRINTF("PHY %p after:\n", phy);
	printhex32((uintptr_t) phy, phy, 0x20);
#endif
	phy->USB_CTRL = 0x00000701;
	phy->PHY_CTRL = 0x00000000;
	return;

	// EHCI0: phy->USB_CTRL
	PRINTF("%p->USB_CTRL=%08X (@%p)\n", phy, (unsigned) phy->USB_CTRL, & phy->USB_CTRL);

	//phy->USB_CTRL = 0x4300CC01;
	//USBPHYC1->USB_CTRL &= ~ (UINT32_C(1) << 0);	// 1: Enable UTMI interface, disable ULPI interface, 0: Enable ULPI interface, disable UTMI interface
	phy->USB_CTRL |= (UINT32_C(1) << 0);	// 1: Enable UTMI interface, disable ULPI interface, 0: Enable ULPI interface, disable UTMI interface
	//phy->USB_CTRL |= (UINT32_C(1) << 28);	// DMA Transfer Status Enable
	//phy->USB_CTRL |= (UINT32_C(1) << 1);	// EHCI HS force required ?
	//phy->USB_CTRL |= (UINT32_C(1) << 20);	// EHCI HS force - resulted LOW speed
	PRINTF("%p->USB_CTRL=%08X\n", phy, (unsigned) phy->USB_CTRL);
}


#elif (CPUSTYLE_T113 || CPUSTYLE_F133) && defined (WITHUSBHW_EHCI)

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


void USBH_OHCI_IRQHandler(void)
{
#if TUP_USBIP_OHCI && defined (WITHUSBHW_OHCI)
	hcd_int_handler(BOARD_TUH_RHPORT, 1);
#endif
}

void USBH_EHCI_IRQHandler(void)
{
#if TUP_USBIP_EHCI && defined (WITHUSBHW_EHCI)
	hcd_int_handler(BOARD_TUH_RHPORT, 1);
#endif
}

// Host clocks intialize
void ohciehci_clk_init(void)
{
#if defined (WITHUSBHW_EHCI) || defined (WITHUSBHW_OHCI)
#if CPUSTYLE_T507 || CPUSTYLE_H616

	CCU->USB0_CLK_REG |= 0x20000000;	// @0x0A70 was: 0x40000000

	CCU->USB2_CLK_REG |= 0x60000000;	// WAS: 0 - USBPHY2RST, SCLK_GATING _USBPHY2
	CCU->USB_BGR_REG |= 0x00400040; // WAS 01000100 - USBEHCI2_RST USBEHCI2_GATING

	const unsigned OHCIx_12M_SRC_SEL = 1;	// OHCI3_12M_SRC_SEL 01: 12M divided from 24 MHz

	{
		// PHY2 enable
		CCU->USB2_CLK_REG = (CCU->USB2_CLK_REG & ~ (UINT32_C(0x03) << 24)) | (OHCIx_12M_SRC_SEL << 24);
		CCU->USB2_CLK_REG |= (UINT32_C(1) << 30);	// USBPHY2_RST
		CCU->USB2_CLK_REG |= (UINT32_C(1) << 29);	// SCLK_GATING_USBPHY2
		SetupHostUsbPhyc(USBPHYC2);
	}

	if (0)
	{

	}
	else if (WITHUSBHW_EHCI == USB20_HOST0_EHCI)
	{
		const unsigned ix = 0;
		// EHCI0/OHCI0
		arm_hardware_disable_handler(USB20_OTG_DEVICE_IRQn);
		arm_hardware_disable_handler(USB20_HOST0_OHCI_IRQn);
		arm_hardware_disable_handler(USB20_HOST0_EHCI_IRQn);

		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 24);	// USBOTG_RST

#if TUP_USBIP_EHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << (4 + ix));	// USBEHCI0_GATING
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (20 + ix));	// USBEHCI0_RST
		CCU->USB_BGR_REG |= (UINT32_C(1) << (20 + ix));	// USBEHCI0_RST
#endif
#if defined WITHUSBHW_OHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << (0 + ix));	// USBOHCI0_GATING
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (16 + ix));	// USBOHCI0_RST
		CCU->USB_BGR_REG |= (UINT32_C(1) << (16 + ix));	// USBOHCI0_RST
		CCU->USB0_CLK_REG |= (UINT32_C(1) << 31);	// SCLK_GATING_OHCI0
#else
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (16 + ix));	// USBOHCI0_RST
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (0 + ix));	// USBOHCI0_GATING
#endif

		CCU->USB0_CLK_REG &= ~ (UINT32_C(1) << 30);	// USBPHY0_RST
		CCU->USB0_CLK_REG = (CCU->USB0_CLK_REG & ~ (UINT32_C(0x03) << 24)) | (OHCIx_12M_SRC_SEL << 24);
		CCU->USB0_CLK_REG |= (UINT32_C(1) << 30);	// USBPHY0_RST
		CCU->USB0_CLK_REG |= (UINT32_C(1) << 29);	// SCLK_GATING_USBPHY0

		SetupHostUsbPhyc(USBPHYC0);

	#if WITHEHCIHWSOFTSPOLL == 0
		arm_hardware_set_handler_system(USB20_HOST0_OHCI_IRQn, USBH_OHCI_IRQHandler);
		arm_hardware_set_handler_system(USB20_HOST0_EHCI_IRQn, USBH_EHCI_IRQHandler);
	#endif /* WITHEHCIHWSOFTSPOLL == 0 */
	}
	else if (WITHUSBHW_EHCI == USB20_HOST1_EHCI)
	{
		const unsigned ix = 1;
		// EHCI1/OHCI1
		arm_hardware_disable_handler(USB20_HOST1_OHCI_IRQn);
		arm_hardware_disable_handler(USB20_HOST1_EHCI_IRQn);

#if TUP_USBIP_EHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << (4 + ix));	// USBEHCI1_GATING
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (20 + ix));	// USBEHCI1_RST
		CCU->USB_BGR_REG |= (UINT32_C(1) << (20 + ix));	// USBEHCI1_RST
#endif
#if defined WITHUSBHW_OHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << (0 + ix));	// USBOHCI1_GATING
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (16 + ix));	// USBOHCI1_RST
		CCU->USB_BGR_REG |= (UINT32_C(1) << (16 + ix));	// USBOHCI1_RST
		CCU->USB1_CLK_REG |= (UINT32_C(1) << 31);	// SCLK_GATING_OHCI1
#else
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (16 + ix));	// USBOHCI1_RST
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (0 + ix));	// USBOHCI1_GATING
#endif

		CCU->USB1_CLK_REG &= ~ (UINT32_C(1) << 30);	// USBPHY1_RST
		CCU->USB1_CLK_REG = (CCU->USB1_CLK_REG & ~ (UINT32_C(0x03) << 24)) | (OHCIx_12M_SRC_SEL << 24);
		CCU->USB1_CLK_REG |= (UINT32_C(1) << 30);	// USBPHY1_RST
		CCU->USB1_CLK_REG |= (UINT32_C(1) << 29);	// SCLK_GATING_USBPHY1

		SetupHostUsbPhyc(USBPHYC1);

	#if WITHEHCIHWSOFTSPOLL == 0
		arm_hardware_set_handler_system(USB20_HOST1_OHCI_IRQn, USBH_OHCI_IRQHandler);
		arm_hardware_set_handler_system(USB20_HOST1_EHCI_IRQn, USBH_EHCI_IRQHandler);
	#endif /* WITHEHCIHWSOFTSPOLL == 0 */
	}
	else if (WITHUSBHW_EHCI == USB20_HOST2_EHCI)
	{
		const unsigned ix = 2;
		// EHCI2/OHCI2
		arm_hardware_disable_handler(USB20_HOST2_OHCI_IRQn);
		arm_hardware_disable_handler(USB20_HOST2_EHCI_IRQn);

#if TUP_USBIP_EHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << (4 + ix));	// USBEHCI2_GATING
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (20 + ix));	// USBEHCI2_RST
		CCU->USB_BGR_REG |= (UINT32_C(1) << (20 + ix));	// USBEHCI2_RST
#endif
#if defined WITHUSBHW_OHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << (0 + ix));	// USBOHCI2_GATING
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (16 + ix));	// USBOHCI2_RST
		CCU->USB_BGR_REG |= (UINT32_C(1) << (16 + ix));	// USBOHCI2_RST
		CCU->USB2_CLK_REG |= (UINT32_C(1) << 31);	// SCLK_GATING_OHCI2
#else
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (16 + ix));	// USBOHCI2_RST
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (0 + ix));	// USBOHCI2_GATING
#endif

		//CCU->USB2_CLK_REG &= ~ (UINT32_C(1) << 30);	// USBPHY2_RST
		CCU->USB2_CLK_REG = (CCU->USB2_CLK_REG & ~ (UINT32_C(0x03) << 24)) | (OHCIx_12M_SRC_SEL << 24);
		CCU->USB2_CLK_REG |= (UINT32_C(1) << 30);	// USBPHY2_RST
		CCU->USB2_CLK_REG |= (UINT32_C(1) << 29);	// SCLK_GATING_USBPHY2

		SetupHostUsbPhyc(USBPHYC2);

	#if WITHEHCIHWSOFTSPOLL == 0
		arm_hardware_set_handler_system(USB20_HOST2_OHCI_IRQn, USBH_OHCI_IRQHandler);
		arm_hardware_set_handler_system(USB20_HOST2_EHCI_IRQn, USBH_EHCI_IRQHandler);
	#endif /* WITHEHCIHWSOFTSPOLL == 0 */
	}
	else if (WITHUSBHW_EHCI == USB20_HOST3_EHCI)
	{
		const unsigned ix = 3;
		// EHCI3/OHCI3
		arm_hardware_disable_handler(USB20_HOST3_OHCI_IRQn);
		arm_hardware_disable_handler(USB20_HOST3_EHCI_IRQn);

#if TUP_USBIP_EHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << (4 + ix));	// USBEHCI3_GATING
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (20 + ix));	// USBEHCI3_RST
		CCU->USB_BGR_REG |= (UINT32_C(1) << (20 + ix));	// USBEHCI3_RST
#endif
#if defined WITHUSBHW_OHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << (0 + ix));	// USBOHCI3_GATING
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (16 + ix));	// USBOHCI3_RST
		CCU->USB_BGR_REG |= (UINT32_C(1) << (16 + ix));	// USBOHCI3_RST
		CCU->USB3_CLK_REG |= (UINT32_C(1) << 31);	// SCLK_GATING_OHCI3
#else
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (16 + ix));	// USBOHCI3_RST
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (0 + ix));	// USBOHCI3_GATING
#endif

		CCU->USB3_CLK_REG &= ~ (UINT32_C(1) << 30);	// USBPHY3_RST
		CCU->USB3_CLK_REG = (CCU->USB3_CLK_REG & ~ (UINT32_C(0x03) << 24)) | (OHCIx_12M_SRC_SEL << 24);
		CCU->USB3_CLK_REG |= (UINT32_C(1) << 30);	// USBPHY3_RST
		CCU->USB3_CLK_REG |= (UINT32_C(1) << 29);	// SCLK_GATING_USBPHY3

		SetupHostUsbPhyc(USBPHYC3);

	#if WITHEHCIHWSOFTSPOLL == 0
		arm_hardware_set_handler_system(USB20_HOST3_OHCI_IRQn, USBH_OHCI_IRQHandler);
		arm_hardware_set_handler_system(USB20_HOST3_EHCI_IRQn, USBH_EHCI_IRQHandler);
	#endif /* WITHEHCIHWSOFTSPOLL == 0 */
	}
#if WITHTINYUSB
	arm_hardware_disable_handler(WITHUSBHW_EHCI_IRQ);
	arm_hardware_disable_handler(WITHUSBHW_OHCI_IRQ);
#endif /* WITHTINYUSB */

#elif CPUSTYLE_A64

	PRINTF("allwnr_a64_get_pll_hsic_freq()=%" PRIuFAST32 "\n", allwnr_a64_get_pll_hsic_freq());
	PRINTF("allwnr_a64_get_mbus_freq()=%" PRIuFAST32 "\n", allwnr_a64_get_mbus_freq());
	PRINTF("allwnr_a64_get_pll_ddr0_freq()=%" PRIuFAST64 "\n", allwnr_a64_get_pll_ddr0_freq());

	// xfel boot
	//	USBPHY_CFG_REG: 00000101
	//	BUS_CLK_GATING_REG0: 00800000
	//	BUS_SOFT_RST_REG0: 00800000

	CCU->USBPHY_CFG_REG = (CCU->USBPHY_CFG_REG & ~ (
			(UINT32_C(3) << 16) |	// SCLK_GATING_OHCI - 11:OTG-OHCI and OHCI0 Clock is ON
			(UINT32_C(1) << 11) |
			(UINT32_C(1) << 10) |
			(UINT32_C(1) << 2) |
			0)) |
		(UINT32_C(3) << 16) |	// SCLK_GATING_OHCI - 11:OTG-OHCI and OHCI0 Clock is ON
		//(UINT32_C(1) << 11) |	// SCLK_GATING_12M  Gating Special 12M Clock For HSIC
		//(UINT32_C(1) << 10) |	// SCLK_GATING_HSIC Gating Special Clock For HSIC
		(UINT32_C(1) << 2) |	// USBHSIC_RST
		0;

	const unsigned OHCIx_12M_SRC_SEL = 0u;	// 00: 12M divided from 48M, 01: 12M divided from 24M, 10: LOSC

	if ((void *) WITHUSBHW_EHCI == USBEHCI0)
	{
		//PRINTF("Enable USBEHCI0 clocks\n");
		ASSERT((void *) WITHUSBHW_EHCI == USBEHCI0);	/* host and usb-otg port */

		CCU->USBPHY_CFG_REG = (CCU->USBPHY_CFG_REG & ~ (
				(3u << 20) |	// OHCI0_12M_SRC_SEL
				(UINT32_C(1) << 8) |	// SCLK_GATING_USBPHY0 - xfel boot setup
				(UINT32_C(1) << 0) |	// USBPHY0_RST
				0)) |
			(OHCIx_12M_SRC_SEL << 20) |	// OHCI0_12M_SRC_SEL
			(UINT32_C(1) << 8) |	// SCLK_GATING_USBPHY0 - xfel boot setup
			(UINT32_C(1) << 0) |	// USBPHY0_RST - xfel boot setup
			0;

		CCU->BUS_SOFT_RST_REG0 &= ~ (UINT32_C(1) << 23);	// USB-OTG-Device_RST

		CCU->BUS_CLK_GATING_REG0 |= (UINT32_C(1) << 28);	// USB-OTG-OHCI_GATING.
		CCU->BUS_CLK_GATING_REG0 |= (UINT32_C(1) << 24);	// USB-OTG-EHCI_GATING.
		//CCU->BUS_CLK_GATING_REG0 |= (UINT32_C(1) << 23);	// USB-OTG-Device_GATING.	- xfel boot setup

		CCU->BUS_SOFT_RST_REG0 |= (UINT32_C(1) << 28);	// USB-OTG-OHCI_RST.
		CCU->BUS_SOFT_RST_REG0 |= (UINT32_C(1) << 24);	// USB-OTG-EHCI_RST
		//CCU->BUS_SOFT_RST_REG0 |= (UINT32_C(1) << 23);	// USB-OTG-Device_RST.	- xfel boot setup

		SetupHostUsbPhyc(USBPHY0);

#if WITHEHCIHWSOFTSPOLL == 0
		arm_hardware_set_handler_system(USBOHCI0_IRQn, USBH_OHCI_IRQHandler);
		arm_hardware_set_handler_system(USBEHCI0_IRQn, USBH_EHCI_IRQHandler);
#endif /* WITHEHCIHWSOFTSPOLL == 0 */
	}
	else
	{
		//PRINTF("Enable USBEHCI1 clocks\n");
		ASSERT((void *) WITHUSBHW_EHCI == USBEHCI1);	/* host-only port */

		CCU->USBPHY_CFG_REG = (CCU->USBPHY_CFG_REG & ~ (
				(3u << 22) |	// OHCI1_12M_SRC_SEL
				(UINT32_C(1) << 9) |
				(UINT32_C(1) << 1) |
				0)) |
			(OHCIx_12M_SRC_SEL << 22) |	// OHCI1_12M_SRC_SEL
			(UINT32_C(1) << 9) |	// SCLK_GATING_USBPHY1
			(UINT32_C(1) << 1) |	// USBPHY1_RST
			0;

		CCU->BUS_CLK_GATING_REG0 |= (UINT32_C(1) << 29);	// USBOHCI0_GATING.
		CCU->BUS_CLK_GATING_REG0 |= (UINT32_C(1) << 25);	// USBEHCI0_GATING.

		CCU->BUS_SOFT_RST_REG0 |= (UINT32_C(1) << 29);	// USB-OHCI0_RST.
		CCU->BUS_SOFT_RST_REG0 |= (UINT32_C(1) << 25);	// USB-EHCI0_RST.

		SetupHostUsbPhyc(USBPHY1);

#if WITHEHCIHWSOFTSPOLL == 0
	arm_hardware_set_handler_system(USBOHCI1_IRQn, USBH_OHCI_IRQHandler);
	arm_hardware_set_handler_system(USBEHCI1_IRQn, USBH_EHCI_IRQHandler);
#endif /* WITHEHCIHWSOFTSPOLL == 0 */
	}
#if WITHTINYUSB
	arm_hardware_disable_handler(WITHUSBHW_EHCI_IRQ);
	arm_hardware_disable_handler(WITHUSBHW_OHCI_IRQ);
#endif /* WITHTINYUSB */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133)

	//PRINTF("From boot: allwnr_t113_get_peripll2x_freq=%" PRIuFAST32 "\n", allwnr_t113_get_peripll2x_freq());

	// DCXO24M -> PLL_PERI
	// 0, 1 - work. 2 - not work
	const unsigned ohci_src = 0x01u; 	// 00: 12M divided from 48 MHz 01: 12M divided from 24 MHz 10: RTC_32K
	if ((void *) WITHUSBHW_EHCI == USBEHCI0)
	{
		arm_hardware_disable_handler(USB0_EHCI_IRQn);
		arm_hardware_disable_handler(USB0_OHCI_IRQn);
		// Turn off USBOTG0
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 24);	// USBOTG0_RST
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 8);	// USBOTG0_GATING

#if TUP_USBIP_EHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << 4);	// USBEHCI0_GATING
#endif
#if TUP_USBIP_OHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << 0);	// USBOHCI0_GATING
#endif

		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 20);	// USBEHCI0_RST
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 16);	// USBOHCI0_RST
#if TUP_USBIP_EHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << 20);	// USBEHCI0_RST
#endif
#if TUP_USBIP_OHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << 16);	// USBOHCI0_RST
#endif

		// Enable
		CCU->USB0_CLK_REG |= (UINT32_C(1) << 31);	// USB0_CLKEN - Gating Special Clock For OHCI0
		CCU->USB0_CLK_REG |= (UINT32_C(1) << 30);	// USBPHY0_RSTN

		// OHCI0 12M Source Select
		CCU->USB0_CLK_REG = (CCU->USB0_CLK_REG & ~ (0x03u << 24)) |
			(ohci_src << 24) | 	// 00: 12M divided from 48 MHz 01: 12M divided from 24 MHz 10: RTC_32K
			0;

		USBOTG0->PHY_OTGCTL &= ~ (UINT32_C(1) << 0); 	// Host mode. Route phy0 to EHCI/OHCI

	#if WITHEHCIHWSOFTSPOLL == 0
		arm_hardware_set_handler_system(USB0_OHCI_IRQn, USBH_OHCI_IRQHandler);
		arm_hardware_set_handler_system(USB0_EHCI_IRQn, USBH_EHCI_IRQHandler);
	#endif /* WITHEHCIHWSOFTSPOLL == 0 */

	}
	else if ((void *) WITHUSBHW_EHCI == USBEHCI1)
	{
		// "правильный" канал
		arm_hardware_disable_handler(USB1_EHCI_IRQn);
		arm_hardware_disable_handler(USB1_OHCI_IRQn);

#if TUP_USBIP_EHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << 5);	// USBEHCI1_GATING
#endif
#if TUP_USBIP_OHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << 1);	// USBOHCI1_GATING
#endif

		CCU->USB1_CLK_REG |= (UINT32_C(1) << 31);	// USB1_CLKEN Gating Special Clock For OHCI1
		CCU->USB1_CLK_REG |= (UINT32_C(1) << 30);	// USBPHY1_RSTN

		// OHCI0 12M Source Select
		CCU->USB1_CLK_REG = (CCU->USB1_CLK_REG & ~ (0x03 << 24)) |
			(ohci_src << 24) | 	// 00: 12M divided from 48 MHz 01: 12M divided from 24 MHz 10: RTC_32K
			0;

		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 17);	// USBOHCI1_RST
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 21);	// USBEHCI1_RST
#if TUP_USBIP_EHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << 21);	// USBEHCI1_RST
#endif
#if TUP_USBIP_OHCI
		CCU->USB_BGR_REG |= (UINT32_C(1) << 17);	// USBOHCI1_RST
#endif

	#if WITHEHCIHWSOFTSPOLL == 0
		arm_hardware_set_handler_system(USB1_OHCI_IRQn, USBH_OHCI_IRQHandler);
		arm_hardware_set_handler_system(USB1_EHCI_IRQn, USBH_EHCI_IRQHandler);
	#endif /* WITHEHCIHWSOFTSPOLL == 0 */

	}
	else
	{
		ASSERT(0);
	}

	if ((void *) WITHUSBHW_EHCI == USBEHCI0)
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
		USBPHY0->PHY_CTRL &= ~ (UINT32_C(1) << 3);		// PHY_CTL_SIDDQ
		USBPHY0->PHY_CTRL |= (UINT32_C(1) << 5);		// PHY_CTL_VBUSVLDEXT
		USBPHY0->USB_CTRL |= (UINT32_C(1) << 0);		// 1: Enable UTMI interface, disable ULPI interface


	}
	else if ((void *) WITHUSBHW_EHCI == USBEHCI1)
	{
		// "правильный" канал
		// USBPHY1

		USBPHY1->USB_CTRL |= (UINT32_C(1) << 0);	// 1: Enable UTMI interface, disable ULPI interface
		USBPHY1->USB_CTRL |=
				(UINT32_C(1) << 11) |	// 1: Use INCR16 when appropriate
				(UINT32_C(1) << 10) |	// 1: Use INCR8 when appropriate
				(UINT32_C(1) << 9) |	// 1: Use INCR4 when appropriate
				(UINT32_C(1) << 8) |	// 1: Start INCRx burst only on burst x-align address Note: This bit must enable if any bit of bit[11:9] is enabled
				0;

		USBPHY1->PHY_CTRL &= ~ (UINT32_C(1) << 3); 	// SIDDQ 0: Write 0 to enable phy
//		USBPHY1->USB_SPDCR = (USBPHY1->USB_SPDCR & ~ ((0x03u << 0) | 0) |
//			(0x02u << 0) |
//			0;

	}
	else
	{
		ASSERT(0);
	}

#if WITHTINYUSB
	arm_hardware_disable_handler(WITHUSBHW_EHCI_IRQ);
	arm_hardware_disable_handler(WITHUSBHW_OHCI_IRQ);
#endif /* WITHTINYUSB */

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
#if WITHTINYUSB
	arm_hardware_disable_handler(WITHUSBHW_EHCI_IRQ);
	arm_hardware_disable_handler(WITHUSBHW_OHCI_IRQ);
#endif /* WITHTINYUSB */

#elif CPUSTYLE_XC7Z

		XUSBPS_Registers * const USBx = EHCIxToUSBx(WITHUSBHW_EHCI);
		enum {  SRCSEL_SHIFT = 4 };
		const unsigned SRCSEL_MASK = (7u << SRCSEL_SHIFT);
		if (WITHUSBHW_EHCI == EHCI0)
		{
			enum { usbIX = 0 };
			//PRINTF("HAL_EHCI_MspInit: EHCI0\n");

			SCLR->SLCR_UNLOCK = 0x0000DF0DU;
			SCLR->APER_CLK_CTRL |= (UINT32_C(1) << (usbIX + 2));	// APER_CLK_CTRL.USB0_CPU_1XCLKACT

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

			SCLR->USB_RST_CTRL |= (UINT32_C(1) << usbIX);
			(void) SCLR->USB_RST_CTRL;
			SCLR->USB_RST_CTRL &= ~ (UINT32_C(1) << usbIX);
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
			SCLR->APER_CLK_CTRL |= (UINT32_C(1) << (usbIX + 2));	// APER_CLK_CTRL.USB1_CPU_1XCLKACT

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

			SCLR->USB_RST_CTRL |= (UINT32_C(1) << usbIX);
			(void) SCLR->USB_RST_CTRL;
			SCLR->USB_RST_CTRL &= ~ (UINT32_C(1) << usbIX);
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
#if WITHTINYUSB
	arm_hardware_disable_handler(WITHUSBHW_EHCI_IRQ);
	arm_hardware_disable_handler(WITHUSBHW_OHCI_IRQ);
#endif /* WITHTINYUSB */

#else

	#warning ohciehci_clk_init Not implemented for CPUSTYLE_xxxxx

#endif
#endif /* defined (WITHUSBHW_EHCI) || defined (WITHUSBHW_OHCI) */
}
#endif /* CFG_TUH_ENABLED && (defined (TUP_USBIP_OHCI) || defined (TUP_USBIP_EHCI)) */

#if CFG_TUD_ENABLED


uint8_t const * tud_descriptor_device_cb(void)
{
  (void) index; // for multiple configurations
  return DeviceDescrTbl [0].data;
}
// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
  (void) index; // for multiple configurations
  return ConfigDescrTbl [index].data;
}

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void) langid;
  if (index < usbd_get_stringsdesc_count())
	  return (uint16_t const *) StringDescrTbl [index].data;
  else
	  return NULL;
}

#if WITHUSBCDCACM && WITHWAWXXUSB

static LCLSPINLOCK_t lockusbdev = LCLSPINLOCK_INIT;
//static usb_struct * volatile gpusb = NULL;

// Control signal bitmap values for the SetControlLineState request
// (usbcdc11.pdf, 6.2.14, Table 51)
#define CDC_DTE_PRESENT                         (1 << 0)
#define CDC_ACTIVATE_CARRIER                    (1 << 1)

// Состояние - выбранные альтернативные конфигурации по каждому интерфейсу USB configuration descriptor
//static uint8_t altinterfaces [INTERFACE_count];

static volatile uint16_t usb_cdc_control_state [WITHUSBCDCACM_N];

static volatile uint8_t usbd_cdcX_rxenabled [WITHUSBCDCACM_N];	/* виртуальный флаг разрешения прерывания по приёму символа - HARDWARE_CDC_ONRXCHAR */
static __ALIGN_BEGIN uint8_t cdcXbuffout [WITHUSBCDCACM_N] [VIRTUAL_COM_PORT_OUT_DATA_SIZE] __ALIGN_END;
static __ALIGN_BEGIN uint8_t cdcXbuffin [WITHUSBCDCACM_N] [VIRTUAL_COM_PORT_IN_DATA_SIZE] __ALIGN_END;
static uint16_t cdcXbuffinlevel [WITHUSBCDCACM_N];

static __ALIGN_BEGIN uint8_t cdc_epXdatabuffout [USB_OTG_MAX_EP0_SIZE] __ALIGN_END;

//static uint32_t dwDTERate [WITHUSBCDCACM_N];

#define MAIN_CDC_OFFSET 0
#if WITHUSBCDCACM_N > 1
	#define SECOND_CDC_OFFSET 1
#endif /* WITHUSBCDCACM_N > 1 */

static LCLSPINLOCK_t catlock = LCLSPINLOCK_INIT;

/* управление по DTR происходит сразу, RTS только вместе со следующим DTR */
/* хранимое значение после получения CDC_SET_CONTROL_LINE_STATE */
/* Биты: RTS = 0x02, DTR = 0x01 */

// Обычно используется для переключения на передачу (PTT)
// вызывается в конексте system interrupt
uint_fast8_t usbd_cdc1_getrts(void)
{
	const unsigned offset = MAIN_CDC_OFFSET;
	LCLSPIN_LOCK(& catlock);
	const uint_fast8_t state =
		((usb_cdc_control_state [offset] & CDC_ACTIVATE_CARRIER) != 0) ||
		0;
	LCLSPIN_UNLOCK(& catlock);
	return state;
}

// Обычно используется для телеграфной манипуляции (KEYDOWN)
// вызывается в конексте system interrupt
uint_fast8_t usbd_cdc1_getdtr(void)
{
	const unsigned offset = MAIN_CDC_OFFSET;
	LCLSPIN_LOCK(& catlock);
	const uint_fast8_t state =
		((usb_cdc_control_state [offset] & CDC_DTE_PRESENT) != 0) ||
		0;
	LCLSPIN_UNLOCK(& catlock);
	return state;
}

// Обычно используется для переключения на передачу (PTT)
// вызывается в конексте system interrupt
uint_fast8_t usbd_cdc2_getrts(void)
{
#if WITHUSBCDCACM_N > 1
	const unsigned offset = SECOND_CDC_OFFSET;
	LCLSPIN_LOCK(& catlock);
	const uint_fast8_t state =
		((usb_cdc_control_state [offset] & CDC_ACTIVATE_CARRIER) != 0) ||
		0;
	LCLSPIN_UNLOCK(& catlock);
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
	LCLSPIN_LOCK(& catlock);
	const uint_fast8_t state =
		((usb_cdc_control_state [offset] & CDC_DTE_PRESENT) != 0) ||
		0;
	LCLSPIN_UNLOCK(& catlock);
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
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	LCLSPIN_LOCK(& lockusbdev);

	const unsigned offset = MAIN_CDC_OFFSET;
//	if (gpusb != NULL)
//	{
//		usb_struct * const pusb = gpusb;
//		const uint32_t bo_ep_in = (USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, offset) & 0x0F);
//		if (pusb->eptx_ret[bo_ep_in-1] != USB_RETVAL_COMPOK)
//			return;
//		USB_RETVAL ret = USB_RETVAL_NOTCOMP;
//		const size_t n = ulmin(length, VIRTUAL_COM_PORT_IN_DATA_SIZE);
//		memcpy(cdcXbuffin [offset], buff, n);
//		usbd_cdc_zlp_pending [offset] = n == VIRTUAL_COM_PORT_IN_DATA_SIZE;
//		usbd_cdc_txlen [offset] = n;
//		//printhex(0, cdcXbuffin [offset], usbd_cdc_txlen [offset]);
//		pusb->eptx_ret[bo_ep_in-1] = epx_in_handler_dev(pusb, bo_ep_in, (uintptr_t) cdcXbuffin [offset], usbd_cdc_txlen [offset], USB_PRTCL_BULK);
//// 		do
////  		{
////  			ret = epx_in_handler_dev(pusb, bo_ep_in, (uintptr_t) cdcXbuffin [offset], usbd_cdc_txlen [offset], USB_PRTCL_BULK);
////  		}
////  		while(ret == USB_RETVAL_NOTCOMP);
//	}
	LCLSPIN_UNLOCK(& lockusbdev);
	LowerIrql(oldIrql);
}

uint_fast8_t usbd_cdc_ready(void)	/* временное решение для передачи */
{
	const unsigned offset = MAIN_CDC_OFFSET;
//	if (gpusb != NULL)
//	{
//		usb_struct * const pusb = gpusb;
//		const uint32_t bo_ep_in = (USBD_CDCACM_IN_EP(USBD_EP_CDCACM_IN, offset) & 0x0F);
//		if (pusb->eptx_ret[bo_ep_in-1] != USB_RETVAL_COMPOK)
//			return 0;
//		return 1;
//	}
	return 0;
}

/* Разрешение/запрещение прерывания по передаче символа */
void usbd_cdc_enabletx(uint_fast8_t state)	/* вызывается из обработчика прерываний */
{
	const unsigned offset = MAIN_CDC_OFFSET;
	LCLSPIN_LOCK(& catlock);
	usbd_cdc_txenabled [offset] = state;
	LCLSPIN_UNLOCK(& catlock);
}

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void usbd_cdc_enablerx(uint_fast8_t state)	/* вызывается из обработчика прерываний */
{
	const unsigned offset = MAIN_CDC_OFFSET;
	LCLSPIN_LOCK(& catlock);
	usbd_cdcX_rxenabled [offset] = state;
	LCLSPIN_UNLOCK(& catlock);
}

/* передача символа после прерывания о готовности передатчика - вызывается из HARDWARE_CDC_ONTXCHAR */
void
usbd_cdc_tx(void * ctx, uint_fast8_t c)
{
	const unsigned offset = MAIN_CDC_OFFSET;
	//USBD_HandleTypeDef * const pdev = ctx;
	(void) ctx;
	LCLSPIN_LOCK(& catlock);
	ASSERT(cdcXbuffinlevel  [offset] < VIRTUAL_COM_PORT_IN_DATA_SIZE);
	cdcXbuffin [offset] [cdcXbuffinlevel [offset] ++] = c;
	LCLSPIN_UNLOCK(& catlock);
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
#endif

void tusb_dev_handled(void)
{
	dcd_int_handler(0);

}

void usbdevice_clk_init(void)
{
#if CPUSTYLE_R7S721

	if (WITHUSBHW_DEVICE == & USB200)
	{

		/* ---- Supply clock to the USB20(channel 0) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */
	    //memset(WITHUSBHW_DEVICE, 0, sizeof * WITHUSBHW_DEVICE);

		HARDWARE_USB0_INITIALIZE();

	}
	else if (WITHUSBHW_DEVICE == & USB201)
	{

		/* ---- Supply clock to the USB20(channel 1) ---- */
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP70;	// Module Stop 70 0: Channel 1 of the USB 2.0 host/function module runs.
		CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
		(void) CPG.STBCR7;			/* Dummy read */
	    //memset(WITHUSBHW_DEVICE, 0, sizeof * WITHUSBHW_DEVICE);

		HARDWARE_USB1_INITIALIZE();
	}

#elif CPUSTYLE_STM32MP1
	// Set 3.3 volt DETECTOR enable
	LL_PWR_EnableUSBVoltageDetector();
	while (LL_PWR_IsEnabledUSBVoltageDetector() == 0)
		;

	// Wait 3.3 volt REGULATOR ready
	while (LL_PWR_IsActiveFlag_USB() == 0)
		;

	__HAL_RCC_USBO_CLK_ENABLE();
	__HAL_RCC_USBO_CLK_SLEEP_ENABLE();

	//RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_USBOEN;
	(void) RCC->MP_AHB2ENSETR;
	//RCC->MP_AHB2LPENSETR = RCC_MP_AHB2LPENSETR_USBOLPEN;
	(void) RCC->MP_AHB2LPENSETR;

	__HAL_RCC_USBO_FORCE_RESET();
	__HAL_RCC_USBO_RELEASE_RESET();
	//RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_USBOEN;
	(void) RCC->MP_AHB2ENSETR;
	//RCC->MP_AHB2LPENSETR = RCC_MP_AHB2LPENSETR_USBOLPEN;
	(void) RCC->MP_AHB2LPENSETR;

	if (WITHUSBHW_DEVICE == USB1_OTG_HS)	// legacy name is USB_OTG_HS
	{
		if (pcdHandle->Init.phy_itface == USB_OTG_ULPI_PHY)
		{
			ASSERT(0);	// ULPI not supported by CPU
			//USBD_HS_ULPI_INITIALIZE();

//			__HAL_RCC_USBO_CLK_ENABLE();
//			__HAL_RCC_USBO_CLK_SLEEP_ENABLE();
//			__HAL_RCC_USBOULPI_CLK_ENABLE();
//			__HAL_RCC_USBOULPI_CLK_SLEEP_ENABLE();

//			RCC->MP_AHB2ENSETR |= RCC_MP_AHB2ENSETR_USBOEN | RCC_MP_AHB2ENSETR_USBOULPIEN;	/* USB/OTG HS with ULPI */
//			(void) RCC->AHB1ENR;
//			RCC->MP_AHB2LPENSETR |= RCC_MP_AHB2LPENSETR_USBOLPEN; /* USB/OTG HS  */
//			(void) RCC->MP_AHB2LPENSETR;
//			RCC->MP_AHB2LPENSETR |= RCC_MP_AHB2LPENSETR_USBOULPILPEN; /* USB/OTG HS ULPI  */
//			(void) RCC->MP_AHB2LPENSETR;
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


		if (pcdHandle->Init.use_dedicated_ep1 == ENABLE)
		{
			//arm_hardware_set_handler_system(OTG_HS_EP1_OUT_IRQn, device_OTG_HS_EP1_OUT_IRQHandler);
			//arm_hardware_set_handler_system(OTG_HS_EP1_IN_IRQn, device_OTG_HS_EP1_IN_IRQHandler);
		}
		arm_hardware_set_handler_system(OTG_IRQn, tusb_dev_handled);

	}

#elif CPUSTYLE_STM32H7XX

	//PWR->CR3 |= PWR_CR3_USBREGEN;

	//while ((PWR->CR3 & PWR_CR3_USB33RDY) == 0)
	//	;
	//PWR->CR3 |= PWR_CR3_USBREGEN;
	//while ((PWR->CR3 & PWR_CR3_USB33RDY) == 0)
	//	;
	PWR->CR3 |= PWR_CR3_USB33DEN;

	if (WITHUSBHW_DEVICE == USB1_OTG_HS)	// legacy name is USB_OTG_HS
	{
		if (pcdHandle->Init.phy_itface == USB_OTG_ULPI_PHY)
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


		if (pcdHandle->Init.use_dedicated_ep1 == ENABLE)
		{
			arm_hardware_set_handler_system(OTG_HS_EP1_OUT_IRQn, device_OTG_HS_EP1_OUT_IRQHandler);
			arm_hardware_set_handler_system(OTG_HS_EP1_IN_IRQn, device_OTG_HS_EP1_IN_IRQHandler);
		}
		arm_hardware_set_handler_system(OTG_HS_IRQn, tusb_dev_handled);

	}
	else if (WITHUSBHW_DEVICE == USB2_OTG_FS)	// legacy name is USB_OTG_FS
	{
		if (pcdHandle->Init.phy_itface == USB_OTG_ULPI_PHY)
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

	if (WITHUSBHW_DEVICE == USB_OTG_HS)
	{
		//const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
		//PRINTF(PSTR("HAL_PCD_MspInit: stm32f4xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f4xx_pllq, PLL_FREQ / stm32f4xx_pllq);

		if (pcdHandle->Init.phy_itface == USB_OTG_ULPI_PHY)
		{
			USBD_HS_ULPI_INITIALIZE();

			//PRINTF(PSTR("HAL_PCD_MspInit: USB_OTG_HS and ULPI\n"));
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

			//PRINTF(PSTR("HAL_PCD_MspInit: USB_OTG_HS without ULPI\n"));
			RCC->AHB1ENR |= RCC_AHB1ENR_OTGHSEN;	/* USB/OTG HS  */
			(void) RCC->AHB1ENR;
			RCC->AHB1LPENR |= RCC_AHB1LPENR_OTGHSLPEN; /* USB/OTG HS  */
			(void) RCC->AHB1LPENR;
			RCC->AHB1LPENR &= ~ RCC_AHB1LPENR_OTGHSULPILPEN; /* USB/OTG HS ULPI  */
			(void) RCC->AHB1LPENR;
		}

		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;	/* USB/OTG HS companion - VBUS? */
		(void) RCC->APB2ENR;

		if (pcdHandle->Init.use_dedicated_ep1 == ENABLE)
		{
			NVIC_SetVector(OTG_HS_EP1_OUT_IRQn, (uintptr_t) & device_OTG_HS_EP1_OUT_IRQHandler);
			NVIC_SetPriority(OTG_HS_EP1_OUT_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(OTG_HS_EP1_OUT_IRQn);	// OTG_HS_EP1_OUT_IRQHandler() enable

			NVIC_SetVector(OTG_HS_EP1_IN_IRQn, (uintptr_t) & device_OTG_HS_EP1_IN_IRQHandler);
			NVIC_SetPriority(OTG_HS_EP1_IN_IRQn, ARM_SYSTEM_PRIORITY);
			NVIC_EnableIRQ(OTG_HS_EP1_IN_IRQn);	// OTG_HS_EP1_IN_IRQHandler() enable
		}
		NVIC_SetVector(OTG_HS_IRQn, (uintptr_t) & tusb_dev_handled);
		NVIC_SetPriority(OTG_HS_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(OTG_HS_IRQn);	// OTG_HS_IRQHandler() enable

	}
	else if (WITHUSBHW_DEVICE == USB_OTG_FS)
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

#elif CPUSTYLE_T507 || CPUSTYLE_H616

//	void SetupUsbPhyc(USBPHYC_TypeDef * phy);
//
//	// Отличия в CCU
////	CCU->SMHC2_CLK_REG = 0;
////	CCU->SMHC_BGR_REG = 0;
//
//	CCU->USB0_CLK_REG |= 0x20000000;	// @0x0A70 was: 0x40000000
//
//	CCU->USB2_CLK_REG |= 0x60000000;	// WAS: 0 - USBPHY2RST, SCLK_GATING _USBPHY2
//	CCU->USB_BGR_REG |= 0x00400040; // WAS 01000100 - USBEHCI2_RST USBEHCI2_GATING

	{
		CCU->USB0_CLK_REG |= 0x20000000;	// @0x0A70 was: 0x40000000

		CCU->USB2_CLK_REG |= 0x60000000;	// WAS: 0 - USBPHY2RST, SCLK_GATING _USBPHY2
		CCU->USB_BGR_REG |= 0x00400040; // WAS 01000100 - USBEHCI2_RST USBEHCI2_GATING

//		const unsigned OHCIx_12M_SRC_SEL = 1;	// OHCI3_12M_SRC_SEL 01: 12M divided from 24 MHz
//
//		{
//			// PHY2 enable
//			CCU->USB2_CLK_REG = (CCU->USB2_CLK_REG & ~ (UINT32_C(0x03) << 24)) | (OHCIx_12M_SRC_SEL << 24);
//			CCU->USB2_CLK_REG |= (UINT32_C(1) << 30);	// USBPHY2_RST
//			CCU->USB2_CLK_REG |= (UINT32_C(1) << 29);	// SCLK_GATING_USBPHY2
//			SetupUsbPhyc(USBPHYC2);
//		}

	}
	arm_hardware_disable_handler(USB20_HOST0_OHCI_IRQn);
	arm_hardware_disable_handler(USB20_HOST0_EHCI_IRQn);
	arm_hardware_disable_handler(USB20_OTG_DEVICE_IRQn);

	CCU->USB_BGR_REG |= (UINT32_C(1) << 8);	// USBOTG_GATING
	CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 24);	// USBOTG_RST
	CCU->USB_BGR_REG |= (UINT32_C(1) << 24);	// USBOTG_RST

	CCU->USB0_CLK_REG &= ~ (UINT32_C(1) << 31);	// SCLK_GATING_OHCI0
	CCU->USB0_CLK_REG &= ~ (UINT32_C(1) << 30);	// USBPHY0_RST
	CCU->USB0_CLK_REG |= (UINT32_C(1) << 30);	// USBPHY0_RST

	//USB20_OTG_PHYC->USB_CTRL = UINT32_C(0x4300CC01);	// младший бит не влияет... вообще ничего не влияет
	//USBPHYC0->USB_CTRL =  UINT32_C(0x4300CC00);	// влияет!
	arm_hardware_set_handler_system(USB20_OTG_DEVICE_IRQn, tusb_dev_handled);
	arm_hardware_disable_handler(USB20_OTG_DEVICE_IRQn);

	// Work
	//	USB20_OTG_PHYC_BASE:
	//	05100400  4300CC00 00000000 00000000 00000000 00000022 00000000 023438E4 00000000
	//	05100420  00000001 00000008 00000000 00000000 00000000 00000000 00000000 00000000
	//	05100440  00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
	//	05100460  00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
	// bad
	//	USB20_OTG_PHYC_BASE:
	//	05100400  40000000 00000000 00000000 00000000 00000002 00000000 023438E4 00000000
	//	05100420  00000001 00000004 00000000 00000000 00000000 00000000 00000000 00000000
	//	05100440  00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
	//	05100460  00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

	USB20_OTG_PHYC->USB_CTRL = 0x4300CC00;
	USB20_OTG_PHYC->PHY_CTRL = 0x00000022;
	USB20_OTG_PHYC->HSIC_PHY_tune3 = 0x00000008;

//	PRINTF("USB20_OTG_PHYC_BASE:\n");
//	printhex32(USB20_OTG_PHYC_BASE, USB20_OTG_PHYC, 128);
//	PRINTF("USBPHYC0:\n");
//	printhex32(USBPHYC0_BASE, USBPHYC0, 128);

//	PRINTF("CCU:\n");
//	printhex32(CCU_BASE, CCU, sizeof * CCU);

#elif CPUSTYLE_A64

	//	Allwinner USB DRD support (musb_otg)
	//
	//	Allwinner USB DRD is based on the Mentor USB OTG controller, with a
	//	different register layout and a few missing registers.
	// Turn off EHCI0
//	PRINTF("1 HAL_PCD_MspInit: USBx->PHY_CTRL=%08lX\n", USBx->PHY_CTRL);
//	PRINTF("1 HAL_PCD_MspInit: USBx->USB_CTRL=%08lX\n", USBx->USB_CTRL);
//	PRINTF("1 HAL_PCD_MspInit: CCU->USB0_CLK_REG=%08lX\n", CCU->USB0_CLK_REG);

    arm_hardware_disable_handler(USBOTG0_IRQn);
    arm_hardware_disable_handler(USBEHCI0_IRQn);
    arm_hardware_disable_handler(USBOHCI0_IRQn);

//    PRINTF("USBPHY_CFG_REG = %08X\n", CCU->USBPHY_CFG_REG);

    CCU->USBPHY_CFG_REG &= ~ (1u << 8);	// SCLK_GATING_USBPHY0
    CCU->USBPHY_CFG_REG &= ~ (1u << 0);	// USBPHY0_RST
	// Turn on USBOTG0
    CCU->USBPHY_CFG_REG |= (1u << 8);	// SCLK_GATING_USBPHY0
    CCU->USBPHY_CFG_REG |= (1u << 0);	// USBPHY0_RST

    //CCU->USBPHY_CFG_REG |= (1u << 2);	// USBHSIC_RST ???

	CCU->BUS_SOFT_RST_REG0 &= ~ (1u << 29);	// USB-OHCI0_RST.
	CCU->BUS_SOFT_RST_REG0 &= ~ (1u << 25);	// USB-EHCI0_RST.

	CCU->BUS_CLK_GATING_REG0 &= ~ (1u << 29);	// USBOHCI0_GATING.
	CCU->BUS_CLK_GATING_REG0 &= ~ (1u << 25);	// USBEHCI0_GATING.

	CCU->BUS_CLK_GATING_REG0 |= (1u << 23);	// USB-OTG-Device_GATING.
	CCU->BUS_SOFT_RST_REG0 |= (1u << 23);	// USB-OTG-Device_RST.


	arm_hardware_set_handler_system(USBOTG0_IRQn, tusb_dev_handled);
	arm_hardware_disable_handler(USBOTG0_IRQn);

	// https://github.com/abmwine/FreeBSD-src/blob/86cb59de6f4c60abd0ea3695ebe8fac26ff0af44/sys/dev/usb/controller/musb_otg_allwinner.c
	// https://github.com/abmwine/FreeBSD-src/blob/86cb59de6f4c60abd0ea3695ebe8fac26ff0af44/sys/dev/usb/controller/musb_otg.c

	// https://github.com/guanglun/r329-linux/blob/d6dced5dc9353fad5319ef5fb84e677e2b9a96b4/arch/arm64/boot/dts/allwinner/sun50i-r329.dtsi#L462
	//	/* A83T specific control bits for PHY0 */
	//	#define PHY_CTL_VBUSVLDEXT		BIT(5)
	//	#define PHY_CTL_SIDDQ			BIT(3)
	//	#define PHY_CTL_H3_SIDDQ		BIT(1)

//	USBOTG0->PHY_OTGCTL |= (1uL << 0); 	// Device mode. Route phy0 to OTG0
//
//	USBOTG0->USB_ISCR = 0;//0x4300FC00;	// после запуска из QSPI было 0x40000000
//	// Looks like 9.6.6.24 0x0810 PHY Control Register (Default Value: 0x0000_0008)
//	//USB0_PHY->PHY_CTRL = 0x20;		// после запуска из QSPI было 0x00000008 а из загрузчика 0x00020
//	USBOTG0->PHY_CTRL &= ~ (1uL << 3);	// PHY_CTL_SIDDQ
//	USBOTG0->PHY_CTRL |= (1uL << 5);	// PHY_CTL_VBUSVLDEXT
//    PRINTF("USBPHY_CFG_REG = %08X\n", CCU->USBPHY_CFG_REG);

#elif CPUSTYLE_T113 || CPUSTYLE_F133
	//	Allwinner USB DRD support (musb_otg)
	//
	//	Allwinner USB DRD is based on the Mentor USB OTG controller, with a
	//	different register layout and a few missing registers.
	// Turn off EHCI0
//	PRINTF("1 HAL_PCD_MspInit: USBx->PHY_CTRL=%08lX\n", USBx->PHY_CTRL);
//	PRINTF("1 HAL_PCD_MspInit: USBx->USB_CTRL=%08lX\n", USBx->USB_CTRL);
//	PRINTF("1 HAL_PCD_MspInit: CCU->USB0_CLK_REG=%08lX\n", CCU->USB0_CLK_REG);

    arm_hardware_disable_handler(USB0_DEVICE_IRQn);
    arm_hardware_disable_handler(USB0_EHCI_IRQn);
    arm_hardware_disable_handler(USB0_OHCI_IRQn);

	CCU->USB_BGR_REG &= ~ (1uL << 16);	// USBOHCI0_RST
	CCU->USB_BGR_REG &= ~ (1uL << 20);	// USBEHCI0_RST
	CCU->USB_BGR_REG &= ~ (1uL << 24);	// USBOTG0_RST

	CCU->USB_BGR_REG &= ~ (1uL << 0);	// USBOHCI0_GATING
	CCU->USB_BGR_REG &= ~ (1uL << 4);	// USBEHCI0_GATING

	// Turn on USBOTG0
	CCU->USB_BGR_REG |= (1uL << 8);	// USBOTG0_GATING
	CCU->USB_BGR_REG &= ~ (1uL << 24);	// USBOTG0_RST Assert
	CCU->USB_BGR_REG |= (1uL << 24);	// USBOTG0_RST De-assert

	// Enable
	CCU->USB0_CLK_REG &= ~ (1uL << 31);	// USB0_CLKEN - Gating Special Clock For OHCI0 0: Clock is OFF

	CCU->USB0_CLK_REG &= ~ (1uL << 30);	// USBPHY0_RSTN Assert
	CCU->USB0_CLK_REG |= (1uL << 30);	// USBPHY0_RSTN De-assert

	// https://github.com/abmwine/FreeBSD-src/blob/86cb59de6f4c60abd0ea3695ebe8fac26ff0af44/sys/dev/usb/controller/musb_otg_allwinner.c
	// https://github.com/abmwine/FreeBSD-src/blob/86cb59de6f4c60abd0ea3695ebe8fac26ff0af44/sys/dev/usb/controller/musb_otg.c

	// https://github.com/guanglun/r329-linux/blob/d6dced5dc9353fad5319ef5fb84e677e2b9a96b4/arch/arm64/boot/dts/allwinner/sun50i-r329.dtsi#L462
	//	/* A83T specific control bits for PHY0 */
	//	#define PHY_CTL_VBUSVLDEXT		BIT(5)
	//	#define PHY_CTL_SIDDQ			BIT(3)
	//	#define PHY_CTL_H3_SIDDQ		BIT(1)

	USBOTG0->PHY_OTGCTL |= (1uL << 0); 	// Device mode. Route phy0 to OTG0

	USBOTG0->USB_ISCR = 0;//0x4300FC00;	// после запуска из QSPI было 0x40000000
	// Looks like 9.6.6.24 0x0810 PHY Control Register (Default Value: 0x0000_0008)
	//USB0_PHY->PHY_CTRL = 0x20;		// после запуска из QSPI было 0x00000008 а из загрузчика 0x00020
	USBOTG0->PHY_CTRL &= ~ (1uL << 3);	// PHY_CTL_SIDDQ
	USBOTG0->PHY_CTRL |= (1uL << 5);	// PHY_CTL_VBUSVLDEXT

	arm_hardware_set_handler_system(USB0_DEVICE_IRQn, tusb_dev_handled);
	arm_hardware_disable_handler(USB0_DEVICE_IRQn);

#else
	#error usbdevice_clk_init should be implemented

#endif

}
#endif /* CFG_TUD_ENABLED */

#endif /* WITHTINYUSB */
