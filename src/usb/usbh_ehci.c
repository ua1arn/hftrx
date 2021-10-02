/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if WITHUSBHW && WITHEHCIHW

#include "board.h"
#include "formats.h"
#include "gpio.h"

#include "hal_ehci.h"
#include "usb_device.h"
#include "usb200.h"
#include "usbch9.h"
#include "usbh_core.h"

#include <string.h>

#define WITHEHCIHWSOFTSPOLL 1	/* не использовать аппаратные прерывания - ускоряет работу в случае super loop архитектуры */

void Error_Handler(void);

#if defined (WITHUSBHW_EHCI)
	/* USB Host Core handle declaration. */
	RAMBIGDTCM __ALIGN_BEGIN USBH_HandleTypeDef hUsbHostHS __ALIGN_END;

	// MORI
//	USBH_HandleTypeDef hUSBHost[5];
//	HCD_HandleTypeDef _hHCD[2];
//	EHCI_HandleTypeDef _hEHCI[2];

	static ApplicationTypeDef Appli_state = APPLICATION_IDLE;

	static RAMBIGDTCM __ALIGN_BEGIN EHCI_HandleTypeDef hehci_USB __ALIGN_END;

#endif /* defined (WITHUSBHW_HOST) */

#if WITHUSEUSBFLASH
#include "../../Class/MSC/Inc/usbh_msc.h"
#endif /* WITHUSEUSBFLASH */
#include "../../Class/HID/Inc/usbh_hid.h"
#include "../../Class/HUB/Inc/usbh_hub.h"


// See https://github.com/hulei123/git123/blob/b82c4abbe7c1bf336b956a613ceb31436938e063/src/usb_stack/usb_core/hal/fsl_usb_ehci_hal.h
// https://github.com/LucasIankowski/T2LabSisop/blob/3fe926e01623ca007afc9d7a80c764418d92c2bd/drivers/usb/host/ehci-q.c

// Taken from
//	https://github.com/xushanpu123/xsp-daily-work/blob/ce4b31db29a560400ac948053b451a2122631490/rCore-Tutorial-v3/qemu-5.0.0/roms/ipxe/src/drivers/usb/ehci.c
//	https://github.com/xushanpu123/xsp-daily-work/blob/ce4b31db29a560400ac948053b451a2122631490/rCore-Tutorial-v3/qemu-5.0.0/roms/ipxe/src/include/ipxe/usb.h


#define DBG_LOG 1

#define ENOTSUP 1
#define ENOMEM 1
#define ETIMEDOUT 1
#define ENOBUFS 1
#define ENODEV 1
#define ECANCELED 1
#define ENOENT 1

enum { FLS = EHCI_PERIODIC_FRAMES(EHCI_FLSIZE_DEFAULT) };

#if 1

static SPINLOCK_t asynclock = SPINLOCK_INIT;

// Periodic frame list
// Periodic Schedule list - PERIODICLISTBASE use
static volatile __attribute__((used, aligned(4096))) struct ehci_periodic_frame periodiclist [FLS];

// Asynchronous Schedule list - ASYNCLISTADDR use
// list of queue headers
// выравнивание заменено с 32 на DATA CACHE PAGE
static volatile __attribute__((used, aligned(DCACHEROWSIZE))) struct ehci_queue_head asynclisthead [16];
static volatile __attribute__((used, aligned(DCACHEROWSIZE))) struct ehci_transfer_descriptor qtds [16];

static EHCI_HCTypeDef * volatile ghc;

#endif

typedef uintptr_t physaddr_t;

static uint8_t readb(physaddr_t a)
{
	return * (volatile uint8_t *) a;
}

static uint16_t readw(physaddr_t a)
{
	return * (volatile uint16_t *) a;
}

static uint32_t readl(physaddr_t a)
{
	return * (volatile uint32_t *) a;
}

static void writel(uint32_t v, physaddr_t a)
{
	* (volatile uint32_t *) a = v;
}

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


static uint32_t cpu_to_le32(unsigned long v)
{
	return v;
}

static unsigned short le16_to_cpu(uint16_t v)
{
	return v;
}

static uint16_t cpu_to_le16(unsigned long v)
{
	return v;
}
//
///* установка указаных в mask битов в состояние data */
//static void le32_modify(volatile uint32_t * variable, uint_fast32_t mask, uint_fast32_t data)
//{
//	const uint_fast32_t v = * variable;
//	const uint_fast32_t m = cpu_to_le32(mask);
//	* variable = (v & ~ m) | (cpu_to_le16(data) & m);
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
	const uint_fast8_t m = cpu_to_le16(mask);
	* variable = (v & ~ m) | (cpu_to_le16(data) & m);
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
static void asynclist_item(volatile struct ehci_queue_head * p, volatile struct ehci_queue_head * link, int Head)
{
	ASSERT((virt_to_phys(p) & 0x01F) == 0);
	ASSERT((virt_to_phys(link) & 0x01F) == 0);
	//memset((void *) p, 0xFF, sizeof * p);
	p->link = ehci_link_qhv(link);	// Using of List Termination here prohibited

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

static uint_fast8_t qtd_item2_buff(volatile struct ehci_transfer_descriptor * p, volatile uint8_t * data, unsigned length)
{
	unsigned i;
	ASSERT(offsetof(struct ehci_transfer_descriptor, high) == 32);
	qtd_item2_set_length(p, length);	/* не модифицируем флаг EHCI_LEN_TOGGLE */

	for (i = 0; i < ARRAY_SIZE(p->low) && length != 0; ++ i)
	{
		/* Calculate length of this fragment */
		const uintptr_t phys = virt_to_phys ( data );
		const unsigned offset = ( phys & ( EHCI_PAGE_ALIGN - 1 ) );
		unsigned frag_len = ( EHCI_PAGE_ALIGN - offset );
		if ( frag_len > length )
			frag_len = length;

		/* Sanity checks */
		ASSERT( ( i == 0 ) || ( offset == 0 ) );

		/* Populate buffer pointer */
		p->low [i] = cpu_to_le32 ( phys );
		if ( sizeof ( physaddr_t ) > sizeof ( uint32_t ) ) {
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
	p->flags = pid | 0*EHCI_FL_CERR_MAX | EHCI_FL_IOC;	// Current Page (C_Page) field = 0
	p->status = 0*EHCI_STATUS_ACTIVE | EHCI_STATUS_PING * (ping != 0);
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
			EHCI_CAP_TT_HUB(0) |
			EHCI_CAP_TT_PORT(0);
//	if ( tt ) {
//		ASSERT( tt->hub->usb );
//		cap |= ( EHCI_CAP_TT_HUB ( tt->hub->usb->address ) |
//				EHCI_CAP_TT_PORT ( tt->address ) );
//		if ( attr == USB_ENDPOINT_ATTR_INTERRUPT )
//			cap |= EHCI_CAP_SPLIT_SCHED_DEFAULT;
//	}

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

// USB EHCI controller
void board_ehci_initialize(EHCI_HandleTypeDef * hehci)
{
//	PRINTF("board_ehci_initialize start.\n");

	USB_EHCI_CapabilityTypeDef *const EHCIx = (USB_EHCI_CapabilityTypeDef*) hehci->Instance;
	//EhciController *const ehci = & hehci->ehci;
	unsigned i;

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
	const uintptr_t opregspacebase = (uintptr_t) & EHCIx->HCCAPBASE + (EHCIx->HCCAPBASE & 0x00FF);
	hehci->nports = (EHCIx->HCSPARAMS >> 0) & 0x0F;
	hehci->portsc = ((__IO unsigned long*) (opregspacebase + 0x0044));

	ASSERT(WITHEHCIHW_EHCIPORT < hehci->nports);
	EhciOpRegs * const opRegs = (EhciOpRegs*) opregspacebase;
	//hehci->ehci.capRegs = (EhciCapRegs*) EHCIx;

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
	// wait for the halted bit to become set
	// Ждем пока бит Halted не будет выставлен
	while ((EHCIx->USBSTS & STS_HCHALTED) == 0)
		;
	// Выделяем и выравниваем фрейм лист, пул для очередей и пул для дескрипторов
	// Замечу, что все мои дескрипторы и элементы очереди выравнены на границу 128 байт

	// Disable interrupts
	// Отключаем прерывания
	//hc->opRegs->usbIntr = 0;
	EHCIx->USBINTR = 0;

	/* подготовка кольцевого списка QH */
    for (i = 0; i < ARRAY_SIZE(asynclisthead); ++ i)
    {
        asynclist_item(& asynclisthead [i], & asynclisthead [(i + 1) % ARRAY_SIZE(asynclisthead)], i == 0);
    }
	/* подготовка списка dts */
    for (i = 0; i < ARRAY_SIZE(qtds); ++ i)
    {
        //memset(& qtds [i], 0xFF, sizeof qtds [i]);
        qtds [i].status = EHCI_STATUS_HALTED;
    }

	arm_hardware_flush_invalidate((uintptr_t) & asynclisthead, sizeof asynclisthead);
	arm_hardware_flush_invalidate((uintptr_t) & qtds, sizeof qtds);
	/*
	 * Terminate (T). 1=Last QH (pointer is invalid). 0=Pointer is valid.
	 * If the queue head is in the context of the periodic list, a one bit in this field indicates to the host controller that
	 * this is the end of the periodic list. This bit is ignored by the host controller when the queue head is in the Asynchronous schedule.
	 * Software must ensure that queue heads reachable by the host controller always have valid horizontal link pointers. See Section 4.8.2
	 *
	 */

	// Periodic frame list
	for (i = 0; i < ARRAY_SIZE(periodiclist); ++ i)
	{
		periodiclist [i].link = EHCI_LINK_TERMINATE;	// 0 - valid, 1 - invalid
	}
	arm_hardware_flush_invalidate((uintptr_t) & periodiclist, sizeof periodiclist);

	// Setup frame list
	// Устанавливаем ссылку на фреймлист
	//hc->opRegs->frameIndex = 0;
	EHCIx->FRINDEX = 0;
	//hc->opRegs->periodicListBase = (u32)(uintptr_t)hc->frameList;
	EHCIx->PERIODICLISTBASE = virt_to_phys(& periodiclist);

	// копируем адрес асинхронной очереди в регистр
	//hc->opRegs->asyncListAddr = (u32)(uintptr_t)hc->asyncQH;
	EHCIx->ASYNCLISTADDR = virt_to_phys(& asynclisthead);
	ASSERT(EHCIx->ASYNCLISTADDR == virt_to_phys(& asynclisthead));
	// Устанавливаем сегмент в 0
	//hc->opRegs->ctrlDsSegment = 0;
	EHCIx->CTRLDSSEGMENT = 0x00000000;
	// Clear status
	// Чистим статус
	//hc->opRegs->usbSts = ~0;
	EHCIx->USBSTS = ~ 0uL;
	//ASSERT( & EHCIx->USBSTS == & hehci->ehci.opRegs->usbSts);

	unsigned porti = WITHEHCIHW_EHCIPORT;

	/* Print state of all ports */
	//for (porti = 0; porti < hehci->nports; ++ porti)
	{
		//unsigned long portsc = hehci->portsc [porti];
		//PRINTF("portsc[%u]=%08lX\n", porti, portsc);
	}

	/* Route all ports to EHCI controller */
	//writel ( EHCI_CONFIGFLAG_CF, ehci->op + EHCI_OP_CONFIGFLAG );
	opRegs->configFlag = EHCI_CONFIGFLAG_CF;
	(void) opRegs->configFlag;

	/* Enable power to all ports */
	//for (porti = 0; porti < hehci->nports; ++ porti)
	{
		unsigned long portsc = hehci->portsc [porti];

		portsc &= ~ EHCI_PORTSC_CHANGE;
		//portsc |= EHCI_PORTSC_OWNER;	// ???
		portsc |= EHCI_PORTSC_PP;

		hehci->portsc [porti] = portsc;
		(void) hehci->portsc [porti];
	}
	/* Wait 20ms after potentially enabling power to a port */
	//local_delay_ms ( EHCI_PORT_POWER_DELAY_MS );
	local_delay_ms(50);

//	PRINTF("board_ehci_initialize done.\n");
}

HAL_StatusTypeDef EHCI_DriveVbus(USB_EHCI_CapabilityTypeDef *const EHCIx, uint8_t state) {
	//PRINTF("EHCI_DriveVbus: state=%d\n", (int) state);
	board_set_usbhostvbuson(state);
	board_update();
	return HAL_OK;
}

HAL_StatusTypeDef EHCI_StopHost(USB_EHCI_CapabilityTypeDef *const EHCIx) {

	//PRINTF("%s:\n", __func__);
 	//USB_EHCI_CapabilityTypeDef * const EHCIx = (USB_EHCI_CapabilityTypeDef *) hehci->Instance;

 	EHCIx->USBINTR = 0;


	/* Clear run/stop bit */
 	unsigned long usbcmd;
	usbcmd = EHCIx->USBCMD;
	usbcmd &= ~( EHCI_USBCMD_RUN | EHCI_USBCMD_PERIODIC |
			EHCI_USBCMD_ASYNC );
	EHCIx->USBCMD = usbcmd;
	(void) EHCIx->USBCMD;

	unsigned i;
	/* Wait for device to stop */
	for ( i = 0 ; 1 || i < 100 ; i++ ) {
		unsigned long usbsts;
		/* Check if device is stopped */
		usbsts = EHCIx->USBSTS;
		if ( usbsts & EHCI_USBSTS_HCH )
			break; //return HAL_OK;

		/* Delay */
		//local_delay_ms( 1 );
	}

	ghc = NULL;

	return HAL_OK;
}

/**
  * @brief  Return Host Current Frame number
  * @param  USBx  Selected device
  * @retval current frame number
  */
uint32_t HAL_EHCI_GetCurrentFrame(EHCI_HandleTypeDef * hehci)
{
 	//EhciController * const ehci = & hehci->ehci;
 	USB_EHCI_CapabilityTypeDef * const EHCIx = hehci->Instance;

	return EHCIx->FRINDEX;
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
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_EHCI_HC_Init(EHCI_HandleTypeDef *hehci,
                                  uint8_t ch_num,
                                  uint8_t epnum,
                                  uint8_t dev_address,
                                  uint8_t speed,
                                  uint8_t ep_type,
                                  uint16_t mps)
{
	HAL_StatusTypeDef status = HAL_OK;
	EHCI_HCTypeDef *const hc = & hehci->hc [ch_num];
	USB_EHCI_CapabilityTypeDef *const EHCIx = hehci->Instance;

	__HAL_LOCK(hehci);
	// TODO: use queue head
	// Stop ASYNC queue
	EHCIx->USBCMD &= ~ EHCI_USBCMD_ASYNC;
	(void) EHCIx->USBCMD;
	while ((EHCIx->USBSTS & EHCI_USBSTS_ASYNC) != 0)
		;

	hc->do_ping = 0U;
	hc->dev_addr = dev_address;
	hc->max_packet = mps;
	hc->ch_num = ch_num;
	hc->ep_type = ep_type;
	hc->ep_num = epnum & 0x7FU;

	if ((epnum & 0x80U) == 0x80U)
	{
		hc->ep_is_in = 1U;
	}
	else
	{
		hc->ep_is_in = 0U;
	}

	hc->speed = speed;

// TODO: use queue head
//  status =  USB_HC_Init(hehci->Instance,
//                        ch_num,
//                        epnum,
//                        dev_address,
//                        speed,
//                        ep_type,
//                        mps);
	qtd_item2_set_toggle( & asynclisthead [hc->ch_num].cache, 0);
	//PRINTF("HAL_EHCI_HC_Init: hc->ch_num=%d\n");

	arm_hardware_flush_invalidate((uintptr_t) & asynclisthead, sizeof asynclisthead);
	arm_hardware_flush_invalidate((uintptr_t) & qtds, sizeof qtds);

	// Run ASYNC queue
	EHCIx->USBCMD |= EHCI_USBCMD_ASYNC;
	while ((EHCIx->USBSTS & EHCI_USBSTS_ASYNC) == 0)
		;

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
	EHCI_HCTypeDef *const hc = & hehci->hc [ch_num];
	USB_EHCI_CapabilityTypeDef *const EHCIx = hehci->Instance;

	__HAL_LOCK(hehci);
	// TODO: use queue head
	// Stop ASYNC queue
	EHCIx->USBCMD &= ~ EHCI_USBCMD_ASYNC;
	(void) EHCIx->USBCMD;
	while ((EHCIx->USBSTS & EHCI_USBSTS_ASYNC) != 0)
		;
	unsigned i = ch_num;
	/* подготовка кольцевого списка QH */
	//for (i = 0; i < ARRAY_SIZE(asynclisthead); ++ i)
	{
		asynclist_item( & asynclisthead [i], & asynclisthead [(i + 1) % ARRAY_SIZE(asynclisthead)], i == 0);
		qtds [i].status = EHCI_STATUS_HALTED;
		qtds [i].len = 0;	// toggle bit = 0
		qtds [i].next = cpu_to_le32(EHCI_LINK_TERMINATE);
		qtds [i].alt = cpu_to_le32(EHCI_LINK_TERMINATE);
	}

	hc->ehci_urb_state - URB_IDLE;

	arm_hardware_flush_invalidate((uintptr_t) & asynclisthead, sizeof asynclisthead);
	arm_hardware_flush_invalidate((uintptr_t) & qtds, sizeof qtds);

	// Run ASYNC queue
	EHCIx->USBCMD |= EHCI_USBCMD_ASYNC;
	while ((EHCIx->USBSTS & EHCI_USBSTS_ASYNC) == 0)
		;

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

 	const uint_fast32_t usbsts = EHCIx->USBSTS;
 	const uint_fast32_t usbstsMasked = usbsts & EHCIx->USBSTS & EHCIx->USBINTR;
	unsigned long portsc = hehci->portsc [WITHEHCIHW_EHCIPORT];
 	//PRINTF("HAL_EHCI_IRQHandler: USBSTS=%08lX\n", usbsts);

 	if ((usbsts & (0x01uL << 13)) != 0)
 	{
 		// Reclamation
 	 	//PRINTF("HAL_EHCI_IRQHandler: Reclamation, usbsts=%08lX\n", usbsts);
 	 	//EHCIx->USBCMD &= ~ (CMD_ASE);
 	}
 	if ((usbsts & (0x01uL << 0)))	// USB Interrupt (USBINT) - see EHCI_FL_IOC usage
 	{
 		EHCIx->USBSTS = (0x01uL << 0);	// Clear USB Interrupt (USBINT)
 		//PRINTF("HAL_EHCI_IRQHandler: USB Interrupt (USBINT), usbsts=%08lX\n", usbsts);

 		//unsigned ch_num;
 		//for (ch_num = 0; ch_num < ARRAY_SIZE(asynclisthead); ++ ch_num)
 		if (ghc != NULL)
 		{
			EHCI_HCTypeDef * const hc = ghc;//& hehci->hc [ch_num];
			volatile struct ehci_transfer_descriptor * const qtd = & qtds [hc->ch_num];
			volatile struct ehci_transfer_descriptor * const qtdoverl = & asynclisthead [hc->ch_num].cache;
			const uint_fast8_t status = qtd->status;
			unsigned len = le16_to_cpu(qtd->len) & EHCI_LEN_MASK;
			unsigned pktcnt = hc->xfer_len - len;
	 		//PRINTF("HAL_EHCI_IRQHandler: USB Interrupt (USBINT), hc=%d, usbsts=%08lX, status=%02X, pktcnt=%u\n", hc->ch_num, usbsts, status, pktcnt);
			if ((status & EHCI_STATUS_HALTED) != 0)
			{
				/* serious "can't proceed" faults reported by the hardware */
				// Тут разбирать по особенностям ошибки
				hc->ehci_urb_state = USBH_URB_STALL;
//				PRINTF("HAL_EHCI_IRQHandler: USB Interrupt (USBINT), usbsts=%08lX, qtds[%d]=%02X, urbState=%d\n",
//							(unsigned long) usbsts,
//							ch_num,
//							(unsigned) qtds [ch_num].status,
//							hc->ehci_urb_state
//						);

			}
			else if ((status & EHCI_STATUS_ACTIVE) != 0)
			{
				//continue;	/* обмен еще не закончился */
				//TP();
				goto nextIteration;
			}
			else if ((status & EHCI_STATUS_BABBLE) != 0)
			{
				hc->ehci_urb_state = USBH_URB_STALL;
			}
			else if ((status & EHCI_STATUS_BUFFER) != 0)
			{
				hc->ehci_urb_state = USBH_URB_STALL;
			}
//			else if ((status & EHCI_STATUS_XACT_ERR) != 0)
//			{
//				/* Наличие этого бита при отсутствующем EHCI_STATUS_HALTED = это не ошибка */
//				hc->ehci_urb_state = USBH_URB_STALL;
//			}
			else
			{

//	 			if (hc->ep_is_in)
//	 			{
//	 				printhex((uintptr_t) hc->xfer_buff, hc->xfer_buff, pktcnt);
//	 			}
				hc->xfer_buff += pktcnt;
				hc->xfer_count += pktcnt;
				// продолжаем, если многосегменый обмен
				if (pktcnt == hc->max_packet && hc->xfer_len > hc->max_packet && hc->xfer_len > hc->xfer_count)
				{
					// Restart next transaction
					qtd_item2_buff(qtdoverl, hc->xfer_buff, hc->xfer_len - hc->xfer_count);
					le8_modify( & qtdoverl->status, EHCI_STATUS_MASK, EHCI_STATUS_ACTIVE);

					/* для того, чобы не срабатывало преждевременно - убрать после перехода на списки работающих пересылок */
					le8_modify( & qtd->status, EHCI_STATUS_MASK, EHCI_STATUS_ACTIVE);
					goto nextIteration;
				}
				// Transaction done
	 			hc->ehci_urb_state = URB_DONE;
			}
			ghc = NULL;
		nextIteration:
			;
 		}
 		else
 		{
 			//PRINTF("HAL_EHCI_IRQHandler: ghc already NULL\n");
 	 		//ASSERT(0);
 		}
 		ASSERT((sizeof (struct ehci_transfer_descriptor) % DCACHEROWSIZE) == 0);	/* чтобы invalidate не затронул соседние данные */
 		arm_hardware_invalidate((uintptr_t) & qtds, sizeof qtds);	/* чтобы следующая проверка могла работать */
 		arm_hardware_flush_invalidate((uintptr_t) & asynclisthead, sizeof asynclisthead);
 	}

 	if ((usbsts & (0x01uL << 1)))	// USB Error Interrupt (USBERRINT)
 	{
 		EHCIx->USBSTS = (0x01uL << 1);	// Clear USB Error Interrupt (USBERRINT) interrupt
 		//PRINTF("HAL_EHCI_IRQHandler: USB Error\n");
 		unsigned i;
// 		for (i = 0; i < hehci->nports; ++ i)
// 	 	{
// 	 		PRINTF("HAL_EHCI_IRQHandler: PORTSC[%u]=%08lX\n", i, hehci->portsc [i]);
// 	 	}
 		//hehci->urbState = USBH_URB_ERROR;
 	}

 	if ((usbsts & (0x01uL << 2)))	// Port Change Detect
 	{
 		EHCIx->USBSTS = (0x01uL << 2);	// Clear Port Change Detect interrupt
 		unsigned long portsc = hehci->portsc [WITHEHCIHW_EHCIPORT];
		//PRINTF("HAL_EHCI_IRQHandler: Port Change Detect, usbsts=%08lX, portsc=%08lX, ls=%lu, pe=%lu, ccs=%d\n", usbsts, portsc, (portsc >> 10) & 0x03, (portsc >> 2) & 0x01, !! (portsc & EHCI_PORTSC_CCS));
 		// PORTSC[0]=00001002 - on disconnect
 		// PORTSC[0]=00001803 - on connect
// 		unsigned i;
// 		for (i = 0; i < hehci->nports; ++ i)
// 	 	{
// 	 		PRINTF("HAL_EHCI_IRQHandler: PORTSC[%u]=%08lX\n", i, hehci->portsc [i]);
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
 			//PRINTF("DEv Connect handler\n");
			HAL_EHCI_PortEnabled_Callback(hehci);
 			HAL_EHCI_Connect_Callback(hehci);
 		}
 		else
 		{
 			//PRINTF("DEv Disconnect handler\n");
			HAL_EHCI_Disconnect_Callback(hehci);
 		}
	}

 	if ((usbsts & (0x01uL << 3)))	// Frame List Rollower
 	{
 		EHCIx->USBSTS = (0x01uL << 3);	// Clear Frame List Rollower interrupt
// 		PRINTF("HAL_EHCI_IRQHandler: Frame List Rollower\n");
// 		unsigned i;
// 		for (i = 0; i < hehci->nports; ++ i)
// 	 	{
// 	 		PRINTF("HAL_EHCI_IRQHandler: PORTSC[%u]=%08lX\n", i, hehci->portsc [i]);
// 	 	}
 	}

 	if ((usbsts & (0x01uL << 4)))	// Host System Error
 	{
 		EHCIx->USBSTS = (0x01uL << 4);	// Clear Host System Error interrupt
 		unsigned long portsc = hehci->portsc [WITHEHCIHW_EHCIPORT];
		PRINTF("HAL_EHCI_IRQHandler: Host System Error, usbsts=%08lX, portsc=%08lX, ls=%lu, pe=%lu, ccs=%d\n", usbsts, portsc, (portsc >> 10) & 0x03, (portsc >> 2) & 0x01, !! (portsc & EHCI_PORTSC_CCS));
		//hehci->urbState = USBH_URB_ERROR;
 	}

 	if ((usbsts & (0x01uL << 5)))	// Interrupt On Async Advance
 	{
 		EHCIx->USBSTS = (0x01uL << 5);	// Clear Interrupt On Async Advance
 		//PRINTF("HAL_EHCI_IRQHandler: Interrupt On Async Advance\n");
 		unsigned i;
 		for (i = 0; i < hehci->nports; ++ i)
 	 	{
 	 		PRINTF("HAL_EHCI_IRQHandler: PORTSC[%u]=%08lX\n", i, hehci->portsc [i]);
 	 	}
 	}
}

HAL_StatusTypeDef HAL_EHCI_Init(EHCI_HandleTypeDef *hehci)
{
	//PRINTF("%s:\n", __func__);
 	//EhciController * const ehci = & hehci->ehci;
 	USB_EHCI_CapabilityTypeDef * const EHCIx = hehci->Instance;
 	//PRINTF("HAL_EHCI_Init\n");

 	board_ehci_initialize(hehci);
	//PRINTF("%s: done\n", __func__);
 	return HAL_OK;
}

HAL_StatusTypeDef HAL_EHCI_DeInit(EHCI_HandleTypeDef *hehci)
{

	return HAL_OK;
}

void USBH_OHCI_IRQHandler(void)
{
	ASSERT(0);
	//ehci_bus_poll(& usbbus0);
	//HAL_EHCI_IRQHandler(& hehci_USB);
}

void USBH_EHCI_IRQHandler(void)
{
	//ASSERT(0);
	//ehci_bus_poll(& usbbus0);
	HAL_EHCI_IRQHandler(& hehci_USB);
}

void HAL_EHCI_MspInit(EHCI_HandleTypeDef * hehci)
{
	//PRINTF("%s:\n", __func__);

#if CPUSTYLE_STM32MP1

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

	// OHCI at USB1HSFSP2_BASE
	// EHCI at USB1HSFSP1_BASE
	//printhex(USB1HSFSP2_BASE, (void *) USB1HSFSP2_BASE, 0x0058);
	volatile uint32_t * const HcCommandStatus = (volatile uint32_t *) (USB1HSFSP2_BASE + 0x008); // HcCommandStatus Register
	* HcCommandStatus |= 0x00000001uL;	// HCR HostControllerReset

#if WITHEHCIHWSOFTSPOLL == 0
	arm_hardware_set_handler_system(USBH_OHCI_IRQn, USBH_OHCI_IRQHandler);
	arm_hardware_set_handler_system(USBH_EHCI_IRQn, USBH_EHCI_IRQHandler);
#endif /* WITHEHCIHWSOFTSPOLL == 0 */

#else

	#warning HAL_EHCI_MspInit Not implemented for CPUSTYLE_xxxxx

#endif
	//PRINTF("%s: done\n", __func__);
}

void HAL_EHCI_MspDeInit(EHCI_HandleTypeDef * hehci)
{
	//PRINTF("%s:\n", __func__);

#if CPUSTYLE_STM32MP1

	IRQ_Disable(USBH_OHCI_IRQn);
	IRQ_Disable(USBH_EHCI_IRQn);

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

#else

	#warning HAL_EHCI_MspDeInit Not implemented for CPUSTYLE_xxxxx

#endif
	//PRINTF("%s: done\n", __func__);
}


/**
  * @brief  Start the host driver.
  * @param  hehci EHCI handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_EHCI_Start(EHCI_HandleTypeDef *hehci)
{
	//PRINTF("%s:\n", __func__);
 	USB_EHCI_CapabilityTypeDef * const EHCIx = (USB_EHCI_CapabilityTypeDef *) hehci->Instance;
	//EhciController * const ehci = & hehci->ehci;
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

  	while ((EHCIx->USBSTS & STS_HCHALTED) != 0)
 		;

#if 1
 	EHCIx->USBINTR |=
 			INTR_IOAA |	// Interrupt on ASync Advance Enable
			INTR_HSE |	// Host System Error Interrupt Enable
 			INTR_FLR |	// Frame List Rollower Interrupt Enable - требуется для опознания HIGH SPEED устройств
			INTR_PCD |	// Port Change Interrupt Enable
			INTR_ERROR |	// USB Error Interrupt Enable
			INTR_USBINT |	// USB Interrupt Enable
			0;
#endif

	__HAL_LOCK(hehci);
	__HAL_EHCI_ENABLE(hehci);
	(void) EHCI_DriveVbus(hehci->Instance, 1U);
	__HAL_UNLOCK(hehci);

	//PRINTF("%s: done\n", __func__);
	return HAL_OK;
}

/**
  * @brief  Stop the host driver.
  * @param  hehci EHCI handle
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_EHCI_Stop(EHCI_HandleTypeDef *hehci)
{
	//PRINTF("%s:\n", __func__);
 	//EhciController * const ehci = & hehci->ehci;
  __HAL_LOCK(hehci);
  (void)EHCI_StopHost(hehci->Instance);
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
HAL_StatusTypeDef HAL_EHCI_HC_SubmitRequest(EHCI_HandleTypeDef *hehci,
                                           uint8_t ch_num,
                                           uint8_t direction,
                                           uint8_t ep_type,
                                           uint8_t token,
                                           uint8_t *pbuff,
                                           uint16_t length,
                                           uint8_t do_ping)
{
	EHCI_HCTypeDef *const hc = & hehci->hc [ch_num];

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

	hc->xfer_buff = pbuff;
	hc->xfer_len = length;
	hc->ehci_urb_state = URB_IDLE;
	hc->xfer_count = 0U;
	hc->ch_num = ch_num;
	hc->state = HC_IDLE;

	//PRINTF("HAL_EHCI_HC_SubmitRequest: ch_num=%u, ep_num=%u, max_packet=%u, do_ping=%d\n",  hc->ch_num, hc->ep_num, hc->max_packet, do_ping);
	//return USB_HC_StartXfer(hehci->Instance, &hehci->hc[ch_num], (uint8_t)hehci->Init.dma_enable);
//
//  	  struct ehci_queue_head * head0 = ( struct ehci_queue_head *) async;
//	memset ( head0, 0, sizeof ( *head0 ) );
//	head0->chr = cpu_to_le32 ( EHCI_CHR_HEAD );
//	head0->cache.next = cpu_to_le32 ( EHCI_LINK_TERMINATE );
//	head0->cache.status = EHCI_STATUS_HALTED;

#if 0
	ehci_async_schedule ( ehci );
	writel ( virt_to_phys ( head0 ),
			ehci->op + EHCI_OP_ASYNCLISTADDR );

	/* Use async queue head to determine control data structure segment */
	ehci->ctrldssegment =
			( ( ( uint64_t ) virt_to_phys ( head0 ) ) >> 32 );
	if ( ehci->addr64 ) {
		writel ( ehci->ctrldssegment, ehci->op + EHCI_OP_CTRLDSSEGMENT);
	} else if ( ehci->ctrldssegment ) {
		PRINTF("EHCI %s CTRLDSSEGMENT not supported\n",
				ehci->name );
		rc = -ENOTSUP;
		goto err_ctrldssegment;
	}
#endif

	ASSERT(ghc == NULL);
	volatile struct ehci_queue_head *const qh = & asynclisthead [hc->ch_num];
	volatile struct ehci_transfer_descriptor *qtd = & qtds [hc->ch_num];
	volatile struct ehci_transfer_descriptor *qtdoverl = & asynclisthead [hc->ch_num].cache;

	switch (ep_type)
	{
	case EP_TYPE_CTRL:
		if (token == 0)
		{
			// Setup
			//PRINTF("HAL_EHCI_HC_SubmitRequest: SETUP, pbuff=%p, length=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", hc->xfer_buff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
			//printhex(0, pbuff, hc->xfer_len);

			VERIFY(0 == qtd_item2_buff(qtdoverl, hc->xfer_buff, hc->xfer_len));
			qtd_item2(qtdoverl, EHCI_FL_PID_SETUP, do_ping);
			arm_hardware_flush((uintptr_t) hc->xfer_buff, hc->xfer_len);

			// бит toggle хранится в памяти overlay и модифицируется сейчас в соответствии с требовании для SETUP запросов
			qtd_item2_set_toggle(qtdoverl, 0);

		}
		else if (direction == 0)
		{
			// Data OUT
			//PRINTF("HAL_EHCI_HC_SubmitRequest: OUT, pbuff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", pbuff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
			//printhex(0, pbuff, hc->xfer_len);

			VERIFY(0 == qtd_item2_buff(qtdoverl, hc->xfer_buff, hc->xfer_len));
			qtd_item2(qtdoverl, EHCI_FL_PID_OUT, do_ping);
			arm_hardware_flush((uintptr_t) hc->xfer_buff, hc->xfer_len);

			// бит toggle хранится в памяти overlay и модифицируется сейчас в соответствии с требовании для SETUP запросов
			qtd_item2_set_toggle(qtdoverl, 1);

		}
		else
		{
			// Data In
			//PRINTF("HAL_EHCI_HC_SubmitRequest: IN, hc->xfer_buff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n", hc->xfer_buff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);

			VERIFY(0 == qtd_item2_buff(qtdoverl, hc->xfer_buff, hc->xfer_len));
			qtd_item2(qtdoverl, EHCI_FL_PID_IN, 0);
			arm_hardware_flush_invalidate((uintptr_t) hc->xfer_buff, hc->xfer_len);

			// бит toggle хранится в памяти overlay и модифицируется сейчас в соответствии с требовании для SETUP запросов
			qtd_item2_set_toggle(qtdoverl, 1);
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
//			PRINTF("HAL_EHCI_HC_SubmitRequest: ch_num=%u, ep_num=%u, max_packet=%u\n", hc->ch_num, hc->ep_num, hc->max_packet);
//			printhex((uintptr_t) hc->xfer_buff, hc->xfer_buff, hc->xfer_len);

			VERIFY(0 == qtd_item2_buff(qtdoverl, hc->xfer_buff, hc->xfer_len));
			qtd_item2(qtdoverl, EHCI_FL_PID_OUT, do_ping);
			arm_hardware_flush((uintptr_t) hc->xfer_buff, hc->xfer_len);

			// бит toggle хранится в памяти overlay и модифицируется самим контроллером
		}
		else
		{
			// BULK Data IN
//			PRINTF("HAL_EHCI_HC_SubmitRequest: BULK IN, hc->xfer_buff=%p, hc->xfer_len=%u, addr=%u, do_ping=%d, hc->do_ping=%d\n",
//					hc->xfer_buff, (unsigned) hc->xfer_len, hc->dev_addr, do_ping, hc->do_ping);
//			PRINTF("HAL_EHCI_HC_SubmitRequest: ch_num=%u, ep_num=%u, max_packet=%u\n", hc->ch_num, hc->ep_num, hc->max_packet);

			VERIFY(0 == qtd_item2_buff(qtdoverl, hc->xfer_buff, hc->xfer_len));
			qtd_item2(qtdoverl, EHCI_FL_PID_IN, 0);
			arm_hardware_flush_invalidate((uintptr_t) hc->xfer_buff, hc->xfer_len);

			// бит toggle хранится в памяти overlay и модифицируется самим контроллером
		}
		break;

	default:
		ASSERT(0);
		break;
	}

	le8_modify( & qtdoverl->status, EHCI_STATUS_MASK, EHCI_STATUS_ACTIVE);

	/* для того, чобы не срабатывало преждевременно - убрать после перехода на списки работающих пересылок */
	le8_modify( & qtd->status, EHCI_STATUS_MASK, EHCI_STATUS_ACTIVE);

	asynclist_item2(hc, qh, virt_to_phys(qtd), hc->ch_num == 0);

	arm_hardware_flush_invalidate((uintptr_t) & asynclisthead, sizeof asynclisthead);
	arm_hardware_flush_invalidate((uintptr_t) & qtds, sizeof qtds);

	/*  убрать после перехода на списки работающих пересылок */
	ghc = hc;

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

#if defined (WITHUSBHW_EHCI)

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
		uint16_t length, uint8_t do_ping)
{
	EHCI_HandleTypeDef * const hehci = phost->pData;
	//EhciController * const ehci = & hehci->ehci;
	USB_EHCI_CapabilityTypeDef * const EHCIx = hehci->Instance;

	HAL_StatusTypeDef hal_status = HAL_OK;
	USBH_StatusTypeDef usb_status = USBH_OK;


	//PRINTF("USBH_LL_SubmitURB: direction=%d, ep_type=%d, token=%d\n", direction, ep_type, token);
	//printhex(0, pbuff, length);

	// Stop ASYNC queue
	EHCIx->USBCMD &= ~ EHCI_USBCMD_ASYNC;
	(void) EHCIx->USBCMD;
	while ((EHCIx->USBSTS & EHCI_USBSTS_ASYNC) != 0)
		;

	hal_status = HAL_EHCI_HC_SubmitRequest(phost->pData, pipe, direction ,
								 ep_type, token, pbuff, length,
								 do_ping);

	// Run ASYNC queue
	EHCIx->USBCMD |= EHCI_USBCMD_ASYNC;
	while ((EHCIx->USBSTS & EHCI_USBSTS_ASYNC) == 0)
		;

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
 *            @arg URB_DONE
 *            @arg URB_NOTREADY
 *            @arg URB_NYET
 *            @arg URB_ERROR
 *            @arg URB_STALL
 */
USBH_URBStateTypeDef USBH_LL_GetURBState(USBH_HandleTypeDef *phost,
		uint8_t pipe) {
#if WITHEHCIHWSOFTSPOLL
	HAL_EHCI_IRQHandler(& hehci_USB);
#else /* WITHEHCIHWSOFTSPOLL */
	system_disableIRQ();
	SPIN_LOCK(& asynclock);
	HAL_EHCI_IRQHandler(& hehci_USB);
	SPIN_UNLOCK(& asynclock);
	system_enableIRQ();
#endif /* WITHEHCIHWSOFTSPOLL */
	return (USBH_URBStateTypeDef)HAL_EHCI_HC_GetURBState (phost->pData, pipe);
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
USBH_StatusTypeDef USBH_LL_SetToggle(USBH_HandleTypeDef *phost, uint8_t pipe,
		uint8_t toggle) {
	EHCI_HandleTypeDef *pHandle;
	pHandle = phost->pData;
	ASSERT(pHandle != NULL);
	USB_EHCI_CapabilityTypeDef *const EHCIx = (USB_EHCI_CapabilityTypeDef*) pHandle->Instance;

//	if (pHandle->hc[pipe].ep_is_in) {
//		pHandle->hc[pipe].toggle_in = toggle;
//	} else {
//		pHandle->hc[pipe].toggle_out = toggle;
//	}

	// Stop ASYNC queue
	EHCIx->USBCMD &= ~ EHCI_USBCMD_ASYNC;
	(void) EHCIx->USBCMD;
	while ((EHCIx->USBSTS & EHCI_USBSTS_ASYNC) != 0)
		;

	volatile struct ehci_transfer_descriptor *qtdoverl = & asynclisthead [pipe].cache;
	qtd_item2_set_toggle(qtdoverl, toggle);
	arm_hardware_flush_invalidate((uintptr_t) & asynclisthead, sizeof asynclisthead);

	// Run ASYNC queue
	EHCIx->USBCMD |= EHCI_USBCMD_ASYNC;
	while ((EHCIx->USBSTS & EHCI_USBSTS_ASYNC) == 0)
		;

	return USBH_OK;
}

/**
 * @brief  Return the current toggle of a pipe.
 * @param  phost: Host handle
 * @param  pipe: Pipe index
 * @retval toggle (0/1)
 */
uint8_t USBH_LL_GetToggle(USBH_HandleTypeDef *phost, uint8_t pipe) {
	uint8_t toggle = 0;
//	EHCI_HandleTypeDef *pHandle;
//	pHandle = phost->pData;
//	ASSERT(pHandle != NULL);

//	if (pHandle->hc[pipe].ep_is_in) {
//		toggle = pHandle->hc[pipe].toggle_in;
//	} else {
//		toggle = pHandle->hc[pipe].toggle_out;
//	}

	volatile struct ehci_transfer_descriptor *qtdoverl = & asynclisthead [pipe].cache;
	toggle = (le16_to_cpu(qtdoverl->len) & EHCI_LEN_TOGGLE) != 0;

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
//	/* Determine port speed */
//	if ( ! ccs)
//	{
//		/* Port not connected */
//		speed = USB_SPEED_NONE;
//		PRINTF("speed = USB_SPEED_NONE\n");
//	}
//	else if (line == EHCI_PORTSC_LINE_STATUS_LOW)
//	{
//		/* Detected as low-speed */
//		speed = USB_SPEED_LOW;
//		PRINTF("speed = USB_SPEED_LOW\n");
//	}
//	else if (ped)
//	{
//		/* Port already enabled: must be high-speed */
//		speed = USB_SPEED_HIGH;
//		PRINTF("speed = USB_SPEED_HIGH\n");
//	}
//	else
//	{
//		/* Not low-speed and not yet enabled.  Could be either
//		 * full-speed or high-speed; we can't yet tell.
//		 */
//		speed = USB_SPEED_FULL;
//		PRINTF("speed = USB_SPEED_FULL\n");
//	}

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
	//PRINTF("USBH_LL_ResetPort2: 1 active=%d, : USBCMD=%08lX USBSTS=%08lX PORTSC[%u]=%08lX\n", (int) resetIsActive, EHCIx->USBCMD, EHCIx->USBSTS, WITHEHCIHW_EHCIPORT, ehci->opRegs->ports [WITHEHCIHW_EHCIPORT]);

	if (resetIsActive)
	{
 		unsigned long portsc = hehci->portsc [WITHEHCIHW_EHCIPORT];
 		/* Reset port */
 		portsc &= ~ (EHCI_PORTSC_PED | EHCI_PORTSC_CHANGE);
 		portsc |= EHCI_PORTSC_PR;

 		hehci->portsc [WITHEHCIHW_EHCIPORT] = portsc;
 		(void) hehci->portsc [WITHEHCIHW_EHCIPORT];
	}
	else
	{
		unsigned long portsc = hehci->portsc [WITHEHCIHW_EHCIPORT];
 		/* Release Reset port */
 		portsc &= ~EHCI_PORTSC_PR;	 /** Port reset */

 		hehci->portsc [WITHEHCIHW_EHCIPORT] = portsc;
 		(void) hehci->portsc [WITHEHCIHW_EHCIPORT];
	}
	//local_delay_ms(1000);
	//HAL_Delay(5);
	//PRINTF("USBH_LL_ResetPort2: 2 active=%d, : USBCMD=%08lX USBSTS=%08lX PORTSC[%u]=%08lX\n", (int) resetIsActive, EHCIx->USBCMD, EHCIx->USBSTS, WITHEHCIHW_EHCIPORT, ehci->opRegs->ports [WITHEHCIHW_EHCIPORT]);

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
	return HAL_EHCI_HC_GetXferCount(phost->pData, pipe);
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
                                    uint8_t dev_address, uint8_t speed, uint8_t ep_type, uint16_t mps)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;
	EHCI_HandleTypeDef * const hehci = phost->pData;
	USB_EHCI_CapabilityTypeDef * const EHCIx = hehci->Instance;

  // TODO: use queue head
  // Stop ASYNC queue
  EHCIx->USBCMD &= ~ EHCI_USBCMD_ASYNC;
  (void) EHCIx->USBCMD;
  while ((EHCIx->USBSTS & EHCI_USBSTS_ASYNC) != 0)
  	;

  hal_status = HAL_EHCI_HC_Init(phost->pData, pipe_num, epnum,
                               dev_address, speed, ep_type, mps);


  // Run ASYNC queue
  EHCIx->USBCMD |= EHCI_USBCMD_ASYNC;
  while ((EHCIx->USBSTS & EHCI_USBSTS_ASYNC) == 0)
  	;
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
//
//  hal_status = HAL_EHCI_DeInit(phost->pData);
//
	usb_status = USBH_Get_USB_Status(hal_status);

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

/*
 * user callback definition
*/
void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
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

void MX_USB_HOST_DeInit(void)
{

}

void MX_USB_HOST_Process(void)
{
	USBH_Process(& hUsbHostHS);
#if WITHEHCIHWSOFTSPOLL
	HAL_EHCI_IRQHandler(& hehci_USB);
#else /* WITHEHCIHWSOFTSPOLL */
	system_disableIRQ();
	SPIN_LOCK(& asynclock);
	HAL_EHCI_IRQHandler(& hehci_USB);
	SPIN_UNLOCK(& asynclock);
	system_enableIRQ();
#endif /* WITHEHCIHWSOFTSPOLL */
	//local_delay_ms(100);
	//PRINTF(".");
}

#endif /* defined (WITHUSBHW_EHCI) */

#endif /* WITHUSBHW && WITHEHCIHW */


