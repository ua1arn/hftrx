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
//#include "bsp/board.h"

#include "tusb.h"
#include "host/usbh.h"

#if TUP_USBIP_OHCI
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
#endif


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

#if defined (TUP_USBIP_OHCI) || defined (TUP_USBIP_EHCI)


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

#elif (CPUSTYLE_T507)

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


void USBH_OHCI_IRQHandler(void)
{
#if TUP_USBIP_OHCI
	hcd_int_handler(BOARD_TUH_RHPORT, 1);
#endif
}

void USBH_EHCI_IRQHandler(void)
{
#if TUP_USBIP_EHCI
	hcd_int_handler(BOARD_TUH_RHPORT, 1);
#endif
}

// Host clocks intialize
void ohciehci_clk_init(void)
{

#if CPUSTYLE_T507

	CCU->USB0_CLK_REG |= 0x20000000;	// @0x0A70 was: 0x40000000

	CCU->USB2_CLK_REG |= 0x60000000;	// WAS: 0 - USBPHY2RST, SCLK_GATING _USBPHY2
	CCU->USB_BGR_REG |= 0x00400040; // WAS 01000100 - USBEHCI2_RST USBEHCI2_GATING

	const unsigned OHCIx_12M_SRC_SEL = 1;	// OHCI3_12M_SRC_SEL 01: 12M divided from 24 MHz

//	{
//		// PHY2 enable
//		CCU->USB2_CLK_REG = (CCU->USB2_CLK_REG & ~ (UINT32_C(0x03) << 24)) | (OHCIx_12M_SRC_SEL << 24);
//		CCU->USB2_CLK_REG |= (UINT32_C(1) << 30);	// USBPHY2_RST
//		CCU->USB2_CLK_REG |= (UINT32_C(1) << 29);	// SCLK_GATING_USBPHY2
//		SetupHostUsbPhyc(USBPHYC2);
//	}

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

		CCU->USB_BGR_REG |= (UINT32_C(1) << (4 + ix));	// USBEHCI0_GATING
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (20 + ix));	// USBEHCI0_RST
		CCU->USB_BGR_REG |= (UINT32_C(1) << (20 + ix));	// USBEHCI0_RST

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

		CCU->USB_BGR_REG |= (UINT32_C(1) << (4 + ix));	// USBEHCI1_GATING
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (20 + ix));	// USBEHCI1_RST
		CCU->USB_BGR_REG |= (UINT32_C(1) << (20 + ix));	// USBEHCI1_RST

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

		CCU->USB_BGR_REG |= (UINT32_C(1) << (4 + ix));	// USBEHCI2_GATING
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (20 + ix));	// USBEHCI2_RST
		CCU->USB_BGR_REG |= (UINT32_C(1) << (20 + ix));	// USBEHCI2_RST

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

		CCU->USB_BGR_REG |= (UINT32_C(1) << (4 + ix));	// USBEHCI3_GATING
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << (20 + ix));	// USBEHCI3_RST
		CCU->USB_BGR_REG |= (UINT32_C(1) << (20 + ix));	// USBEHCI3_RST

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

	//PRINTF("From boot: allwnrt113_get_peripll2x_freq=%" PRIuFAST32 "\n", allwnrt113_get_peripll2x_freq());

	// DCXO24M -> PLL_PERI
	// 0, 1 - work. 2 - not work
	const unsigned ohci_src = 0x01u; 	// 00: 12M divided from 48 MHz 01: 12M divided from 24 MHz 10: RTC_32K
	if ((void *) WITHUSBHW_EHCI == USBEHCI0)
	{
		// Turn off USBOTG0
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 24);	// USBOTG0_RST
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 8);	// USBOTG0_GATING

		// Enable
		CCU->USB0_CLK_REG |= (UINT32_C(1) << 31);	// USB0_CLKEN - Gating Special Clock For OHCI0
		CCU->USB0_CLK_REG |= (UINT32_C(1) << 30);	// USBPHY0_RSTN

		// OHCI0 12M Source Select
		CCU->USB0_CLK_REG = (CCU->USB0_CLK_REG & ~ (0x03u << 24)) |
			(ohci_src << 24) | 	// 00: 12M divided from 48 MHz 01: 12M divided from 24 MHz 10: RTC_32K
			0;

		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 16);	// USBOHCI0_RST
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 20);	// USBEHCI0_RST

		CCU->USB_BGR_REG |= (UINT32_C(1) << 0);	// USBOHCI0_GATING
		CCU->USB_BGR_REG |= (UINT32_C(1) << 4);	// USBEHCI0_GATING

		CCU->USB_BGR_REG |= (UINT32_C(1) << 16);	// USBOHCI0_RST
		CCU->USB_BGR_REG |= (UINT32_C(1) << 20);	// USBEHCI0_RST

		USBOTG0->PHY_OTGCTL &= ~ (UINT32_C(1) << 0); 	// Host mode. Route phy0 to EHCI/OHCI

	#if WITHEHCIHWSOFTSPOLL == 0
		arm_hardware_set_handler_system(USB0_OHCI_IRQn, USBH_OHCI_IRQHandler);
		arm_hardware_set_handler_system(USB0_EHCI_IRQn, USBH_EHCI_IRQHandler);
	#endif /* WITHEHCIHWSOFTSPOLL == 0 */

	}
	else
	{
		// "правильный" канал

		CCU->USB1_CLK_REG |= (UINT32_C(1) << 31);	// USB1_CLKEN Gating Special Clock For OHCI1
		CCU->USB1_CLK_REG |= (UINT32_C(1) << 30);	// USBPHY1_RSTN

		// OHCI0 12M Source Select
		CCU->USB1_CLK_REG = (CCU->USB1_CLK_REG & ~ (0x03 << 24)) |
			(ohci_src << 24) | 	// 00: 12M divided from 48 MHz 01: 12M divided from 24 MHz 10: RTC_32K
			0;

		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 17);	// USBOHCI1_RST
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 21);	// USBEHCI1_RST

		CCU->USB_BGR_REG |= (UINT32_C(1) << 1);	// USBOHCI1_GATING
		CCU->USB_BGR_REG |= (UINT32_C(1) << 5);	// USBEHCI1_GATING

		CCU->USB_BGR_REG |= (UINT32_C(1) << 17);	// USBOHCI1_RST
		CCU->USB_BGR_REG |= (UINT32_C(1) << 21);	// USBEHCI1_RST

	#if WITHEHCIHWSOFTSPOLL == 0
		arm_hardware_set_handler_system(USB1_OHCI_IRQn, USBH_OHCI_IRQHandler);
		arm_hardware_set_handler_system(USB1_EHCI_IRQn, USBH_EHCI_IRQHandler);
	#endif /* WITHEHCIHWSOFTSPOLL == 0 */

	}
#if WITHTINYUSB
	arm_hardware_disable_handler(WITHUSBHW_EHCI_IRQ);
	arm_hardware_disable_handler(WITHUSBHW_OHCI_IRQ);
#endif /* WITHTINYUSB */

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
	else
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

	#warning HAL_EHCI_MspInit Not implemented for CPUSTYLE_xxxxx

#endif

}
#endif
#endif /* WITHTINYUSB */
