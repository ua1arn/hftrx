/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"


#if WITHUSBHW && WITHEHCIHW

#include "board.h"
#include "audio.h"
#include "formats.h"
#include "gpio.h"

#include "hal_ehci.h"
#include "usb_device.h"
#include "usb200.h"
#include "usbch9.h"
#include "usbh_core.h"

void Error_Handler(void);

#if defined (WITHUSBHW_EHCI)
	/* USB Host Core handle declaration. */
	RAMBIGDTCM __ALIGN_BEGIN USBH_HandleTypeDef hUsbHostHS __ALIGN_END;

	static ApplicationTypeDef Appli_state = APPLICATION_IDLE;

	static RAMBIGDTCM __ALIGN_BEGIN EHCI_HandleTypeDef hhcd_USB_EHCI __ALIGN_END;

#endif /* defined (WITHUSBHW_HOST) */

#if WITHUSEUSBFLASH
#include "../../Class/MSC/Inc/usbh_msc.h"
#endif /* WITHUSEUSBFLASH */

void board_ehci_initialize(EHCI_HandleTypeDef * hehci);

#include <string.h>

// See https://github.com/hulei123/git123/blob/b82c4abbe7c1bf336b956a613ceb31436938e063/src/usb_stack/usb_core/hal/fsl_usb_ehci_hal.h



static void ehci_queue_fill(uint32_t * qh, uintptr_t qnext)
{
	qh [0] = (qnext & 0xFFFFFFC0) | (1uL << 1);
}

static void ehci_itd_fill(uint32_t * itd)
{
	itd [0] = 0x01;
}


#if 0

#include "ehci.h"


typedef uintptr_t physaddr_t;

uint32_t readl(void * a)
{
	return * (volatile uint32_t *) a;
}

void writel(void * a, uint32_t v)
{
	* (volatile uint32_t *) a = v;
}


static struct usb_hub usb_hub0;
static struct ehci_device ehci_device0;

struct ehci_device * usb_hub_get_drvdata ( struct usb_hub *hub )
{
	return & ehci_device0;
}

struct ehci_device * usb_hub_get_drvdata2 ( struct usb_bus *bus )
{
	return & ehci_device0;
}

/** @file
 *
 * USB Enhanced Host Controller Interface (EHCI) driver
 *
 */

/**
 * Construct error code from transfer descriptor status
 *
 * @v status            Transfer descriptor status
 * @ret rc              Error code
 *
 * Bits 2-5 of the status code provide some indication as to the root
 * cause of the error.  We incorporate these into the error code as
 * reported to usb_complete_err().
 */
#define EIO_STATUS( status ) (-1)

/******************************************************************************
 *
 * Register access
 *
 ******************************************************************************
 */

/**
 * Initialise device
 *
 * @v ehci              EHCI device
 * @v regs              MMIO registers
 */
static void ehci_init (  ehci_device *ehci, void *regs ) {
	uint32_t hcsparams;
	uint32_t hccparams;
	size_t caplength;

	/* Locate capability and operational registers */
	ehci->cap = regs;
	caplength = readb ( ehci->cap + EHCI_CAP_CAPLENGTH );
	ehci->op = ( ehci->cap + caplength );
	DBGC2 ( ehci, "EHCI %s cap %08lx op %08lx\n", ehci->name,
			virt_to_phys ( ehci->cap ), virt_to_phys ( ehci->op ) );

	/* Read structural parameters */
	hcsparams = readl ( ehci->cap + EHCI_CAP_HCSPARAMS );
	ehci->ports = EHCI_HCSPARAMS_PORTS ( hcsparams );
	DBGC ( ehci, "EHCI %s has %d ports\n", ehci->name, ehci->ports );

	/* Read capability parameters 1 */
	hccparams = readl ( ehci->cap + EHCI_CAP_HCCPARAMS );
	ehci->addr64 = EHCI_HCCPARAMS_ADDR64 ( hccparams );
	ehci->flsize = ( EHCI_HCCPARAMS_FLSIZE ( hccparams ) ?
			EHCI_FLSIZE_SMALL : EHCI_FLSIZE_DEFAULT );
	ehci->eecp = EHCI_HCCPARAMS_EECP ( hccparams );
	DBGC2 ( ehci, "EHCI %s %d-bit flsize %d\n", ehci->name,
			( ehci->addr64 ? 64 : 32 ), ehci->flsize );
}

/**
 * Find extended capability
 *
 * @v ehci              EHCI device
 * @v pci               PCI device
 * @v id                Capability ID
 * @v offset            Offset to previous extended capability instance, or zero
 * @ret offset          Offset to extended capability, or zero if not found
 */
static unsigned int ehci_extended_capability ( struct ehci_device *ehci,
		struct pci_device *pci,
		unsigned int id,
		unsigned int offset ) {
	uint32_t eecp;

	/* Locate the extended capability */
	while ( 1 ) {

		/* Locate first or next capability as applicable */
		if ( offset ) {
			pci_read_config_dword ( pci, offset, &eecp );
			offset = EHCI_EECP_NEXT ( eecp );
		} else {
			offset = ehci->eecp;
		}
		if ( ! offset )
			return 0;

		/* Check if this is the requested capability */
		pci_read_config_dword ( pci, offset, &eecp );
		if ( EHCI_EECP_ID ( eecp ) == id )
			return offset;
	}
}

/**
 * Calculate buffer alignment
 *
 * @v len               Length
 * @ret align           Buffer alignment
 *
 * Determine alignment required for a buffer which must be aligned to
 * at least EHCI_MIN_ALIGN and which must not cross a page boundary.
 */
static inline size_t ehci_align ( size_t len ) {
	size_t align;

	/* Align to own length (rounded up to a power of two) */
	align = ( 1 << fls ( len - 1 ) );

	/* Round up to EHCI_MIN_ALIGN if needed */
	if ( align < EHCI_MIN_ALIGN )
		align = EHCI_MIN_ALIGN;

	return align;
}

/**
 * Check control data structure reachability
 *
 * @v ehci              EHCI device
 * @v ptr               Data structure pointer
 * @ret rc              Return status code
 */
static int ehci_ctrl_reachable ( struct ehci_device *ehci, void *ptr ) {
	physaddr_t phys = virt_to_phys ( ptr );
	uint32_t segment;

	/* Always reachable in a 32-bit build */
	if ( sizeof ( physaddr_t ) <= sizeof ( uint32_t ) )
		return 0;

	/* Reachable only if control segment matches in a 64-bit build */
	segment = ( ( ( uint64_t ) phys ) >> 32 );
	if ( segment == ehci->ctrldssegment )
		return 0;

	return -ENOTSUP;
}

/******************************************************************************
 *
 * Diagnostics
 *
 ******************************************************************************
 */

/**
 * Dump host controller registers
 *
 * @v ehci              EHCI device
 */
static __unused void ehci_dump ( struct ehci_device *ehci ) {
	uint8_t caplength;
	uint16_t hciversion;
	uint32_t hcsparams;
	uint32_t hccparams;
	uint32_t usbcmd;
	uint32_t usbsts;
	uint32_t usbintr;
	uint32_t frindex;
	uint32_t ctrldssegment;
	uint32_t periodiclistbase;
	uint32_t asynclistaddr;
	uint32_t configflag;

	/* Do nothing unless debugging is enabled */
	if ( ! DBG_LOG )
		return;

	/* Dump capability registers */
	caplength = readb ( ehci->cap + EHCI_CAP_CAPLENGTH );
	hciversion = readw ( ehci->cap + EHCI_CAP_HCIVERSION );
	hcsparams = readl ( ehci->cap + EHCI_CAP_HCSPARAMS );
	hccparams = readl ( ehci->cap + EHCI_CAP_HCCPARAMS );
	DBGC ( ehci, "EHCI %s caplen %02x hciversion %04x hcsparams %08x "
			"hccparams %08x\n", ehci->name, caplength, hciversion,
			hcsparams,  hccparams );

	/* Dump operational registers */
	usbcmd = readl ( ehci->op + EHCI_OP_USBCMD );
	usbsts = readl ( ehci->op + EHCI_OP_USBSTS );
	usbintr = readl ( ehci->op + EHCI_OP_USBINTR );
	frindex = readl ( ehci->op + EHCI_OP_FRINDEX );
	ctrldssegment = readl ( ehci->op + EHCI_OP_CTRLDSSEGMENT );
	periodiclistbase = readl ( ehci->op + EHCI_OP_PERIODICLISTBASE );
	asynclistaddr = readl ( ehci->op + EHCI_OP_ASYNCLISTADDR );
	configflag = readl ( ehci->op + EHCI_OP_CONFIGFLAG );
	DBGC ( ehci, "EHCI %s usbcmd %08x usbsts %08x usbint %08x frindx "
			"%08x\n", ehci->name, usbcmd, usbsts, usbintr, frindex );
	DBGC ( ehci, "EHCI %s ctrlds %08x period %08x asyncl %08x cfgflg "
			"%08x\n", ehci->name, ctrldssegment, periodiclistbase,
			asynclistaddr, configflag );
}

/******************************************************************************
 *
 * USB legacy support
 *
 ******************************************************************************
 */

/** Prevent the release of ownership back to BIOS */
static int ehci_legacy_prevent_release;

/**
 * Initialise USB legacy support
 *
 * @v ehci              EHCI device
 * @v pci               PCI device
 */
static void ehci_legacy_init ( struct ehci_device *ehci,
		struct pci_device *pci ) {
	unsigned int legacy;
	uint8_t bios;

	/* Locate USB legacy support capability (if present) */
	legacy = ehci_extended_capability ( ehci, pci, EHCI_EECP_ID_LEGACY, 0 );
	if ( ! legacy ) {
		/* Not an error; capability may not be present */
		DBGC ( ehci, "EHCI %s has no USB legacy support capability\n",
				ehci->name );
		return;
	}

	/* Check if legacy USB support is enabled */
	pci_read_config_byte ( pci, ( legacy + EHCI_USBLEGSUP_BIOS ), &bios );
	if ( ! ( bios & EHCI_USBLEGSUP_BIOS_OWNED ) ) {
		/* Not an error; already owned by OS */
		DBGC ( ehci, "EHCI %s USB legacy support already disabled\n",
				ehci->name );
		return;
	}

	/* Record presence of USB legacy support capability */
	ehci->legacy = legacy;
}

/**
 * Claim ownership from BIOS
 *
 * @v ehci              EHCI device
 * @v pci               PCI device
 */
static void ehci_legacy_claim ( struct ehci_device *ehci,
		struct pci_device *pci ) {
	unsigned int legacy = ehci->legacy;
	uint32_t ctlsts;
	uint8_t bios;
	unsigned int i;

	/* Do nothing unless legacy support capability is present */
	if ( ! legacy )
		return;

	/* Dump original SMI usage */
	pci_read_config_dword ( pci, ( legacy + EHCI_USBLEGSUP_CTLSTS ),
			&ctlsts );
	if ( ctlsts ) {
		DBGC ( ehci, "EHCI %s BIOS using SMIs: %08x\n",
				ehci->name, ctlsts );
	}

	/* Claim ownership */
	pci_write_config_byte ( pci, ( legacy + EHCI_USBLEGSUP_OS ),
			EHCI_USBLEGSUP_OS_OWNED );

	/* Wait for BIOS to release ownership */
	for ( i = 0 ; i < EHCI_USBLEGSUP_MAX_WAIT_MS ; i++ ) {

		/* Check if BIOS has released ownership */
		pci_read_config_byte ( pci, ( legacy + EHCI_USBLEGSUP_BIOS ),
				&bios );
		if ( ! ( bios & EHCI_USBLEGSUP_BIOS_OWNED ) ) {
			DBGC ( ehci, "EHCI %s claimed ownership from BIOS\n",
					ehci->name );
			pci_read_config_dword ( pci, ( legacy +
					EHCI_USBLEGSUP_CTLSTS ),
					&ctlsts );
			if ( ctlsts ) {
				DBGC ( ehci, "EHCI %s warning: BIOS retained "
						"SMIs: %08x\n", ehci->name, ctlsts );
			}
			return;
		}

		/* Delay */
		mdelay ( 1 );
	}

	/* BIOS did not release ownership.  Claim it forcibly by
	 * disabling all SMIs.
	 */
	DBGC ( ehci, "EHCI %s could not claim ownership from BIOS: forcibly "
			"disabling SMIs\n", ehci->name );
	pci_write_config_dword ( pci, ( legacy + EHCI_USBLEGSUP_CTLSTS ), 0 );
}

/**
 * Release ownership back to BIOS
 *
 * @v ehci              EHCI device
 * @v pci               PCI device
 */
static void ehci_legacy_release ( struct ehci_device *ehci,
		struct pci_device *pci ) {
	unsigned int legacy = ehci->legacy;
	uint32_t ctlsts;

	/* Do nothing unless legacy support capability is present */
	if ( ! legacy )
		return;

	/* Do nothing if releasing ownership is prevented */
	if ( ehci_legacy_prevent_release ) {
		DBGC ( ehci, "EHCI %s not releasing ownership to BIOS\n",
				ehci->name );
		return;
	}

	/* Release ownership */
	pci_write_config_byte ( pci, ( legacy + EHCI_USBLEGSUP_OS ), 0 );
	DBGC ( ehci, "EHCI %s released ownership to BIOS\n", ehci->name );

	/* Dump restored SMI usage */
	pci_read_config_dword ( pci, ( legacy + EHCI_USBLEGSUP_CTLSTS ),
			&ctlsts );
	DBGC ( ehci, "EHCI %s BIOS reclaimed SMIs: %08x\n",
			ehci->name, ctlsts );
}

/******************************************************************************
 *
 * Companion controllers
 *
 ******************************************************************************
 */

/**
 * Poll child companion controllers
 *
 * @v ehci              EHCI device
 */
static void ehci_poll_companions ( struct ehci_device *ehci ) {
	struct usb_bus *bus;
	struct device_description *desc;

	/* Poll any USB buses belonging to child companion controllers */
	for_each_usb_bus ( bus ) {

		/* Get underlying devices description */
		desc = &bus->dev->desc;

		/* Skip buses that are not PCI devices */
		if ( desc->bus_type != BUS_TYPE_PCI )
			continue;

		/* Skip buses that are not part of the same PCI device */
		if ( PCI_FIRST_FUNC ( desc->location ) !=
				PCI_FIRST_FUNC ( ehci->bus->dev->desc.location ) )
			continue;

		/* Skip buses that are not UHCI or OHCI PCI devices */
		if ( ( desc->class != PCI_CLASS ( PCI_CLASS_SERIAL,
				PCI_CLASS_SERIAL_USB,
				PCI_CLASS_SERIAL_USB_UHCI ))&&
				( desc->class != PCI_CLASS ( PCI_CLASS_SERIAL,
						PCI_CLASS_SERIAL_USB,
						PCI_CLASS_SERIAL_USB_OHCI ) ))
			continue;

		/* Poll child companion controller bus */
		DBGC2 ( ehci, "EHCI %s polling companion %s\n",
				ehci->name, bus->name );
		usb_poll ( bus );
	}
}

/**
 * Locate EHCI companion controller
 *
 * @v pci               PCI device
 * @ret busdevfn        EHCI companion controller bus:dev.fn (if any)
 */
unsigned int ehci_companion ( struct pci_device *pci ) {
	struct pci_device tmp;
	unsigned int busdevfn;
	int rc;

	/* Look for an EHCI function on the same PCI device */
	busdevfn = pci->busdevfn;
	while ( ++busdevfn <= PCI_LAST_FUNC ( pci->busdevfn ) ) {
		pci_init ( &tmp, busdevfn );
		if ( ( rc = pci_read_config ( &tmp ) ) != 0 )
			continue;
		if ( tmp.class == PCI_CLASS ( PCI_CLASS_SERIAL,
				PCI_CLASS_SERIAL_USB,
				PCI_CLASS_SERIAL_USB_EHCI ) )
			return busdevfn;
	}

	return 0;
}

/******************************************************************************
 *
 * Run / stop / reset
 *
 ******************************************************************************
 */

/**
 * Start EHCI device
 *
 * @v ehci              EHCI device
 */
static void ehci_run ( struct ehci_device *ehci ) {
	uint32_t usbcmd;

	/* Set run/stop bit */
	usbcmd = readl ( ehci->op + EHCI_OP_USBCMD );
	usbcmd &= ~EHCI_USBCMD_FLSIZE_MASK;
	usbcmd |= ( EHCI_USBCMD_RUN | EHCI_USBCMD_FLSIZE ( ehci->flsize ) |
			EHCI_USBCMD_PERIODIC | EHCI_USBCMD_ASYNC );
	writel ( usbcmd, ehci->op + EHCI_OP_USBCMD );
}

/**
 * Stop EHCI device
 *
 * @v ehci              EHCI device
 * @ret rc              Return status code
 */
static int ehci_stop ( struct ehci_device *ehci ) {
	uint32_t usbcmd;
	uint32_t usbsts;
	unsigned int i;

	/* Clear run/stop bit */
	usbcmd = readl ( ehci->op + EHCI_OP_USBCMD );
	usbcmd &= ~( EHCI_USBCMD_RUN | EHCI_USBCMD_PERIODIC |
			EHCI_USBCMD_ASYNC );
	writel ( usbcmd, ehci->op + EHCI_OP_USBCMD );

	/* Wait for device to stop */
	for ( i = 0 ; i < EHCI_STOP_MAX_WAIT_MS ; i++ ) {

		/* Check if device is stopped */
		usbsts = readl ( ehci->op + EHCI_OP_USBSTS );
		if ( usbsts & EHCI_USBSTS_HCH )
			return 0;

		/* Delay */
		mdelay ( 1 );
	}

	DBGC ( ehci, "EHCI %s timed out waiting for stop\n", ehci->name );
	return -ETIMEDOUT;
}

/**
 * Reset EHCI device
 *
 * @v ehci              EHCI device
 * @ret rc              Return status code
 */
static int ehci_reset ( struct ehci_device *ehci ) {
	uint32_t usbcmd;
	unsigned int i;
	int rc;

	/* The EHCI specification states that resetting a running
	 * device may result in undefined behaviour, so try stopping
	 * it first.
	 */
	if ( ( rc = ehci_stop ( ehci ) ) != 0 ) {
		/* Ignore errors and attempt to reset the device anyway */
	}

	/* Reset device */
	writel ( EHCI_USBCMD_HCRST, ehci->op + EHCI_OP_USBCMD );

	/* Wait for reset to complete */
	for ( i = 0 ; i < EHCI_RESET_MAX_WAIT_MS ; i++ ) {

		/* Check if reset is complete */
		usbcmd = readl ( ehci->op + EHCI_OP_USBCMD );
		if ( ! ( usbcmd & EHCI_USBCMD_HCRST ) )
			return 0;

		/* Delay */
		mdelay ( 1 );
	}

	DBGC ( ehci, "EHCI %s timed out waiting for reset\n", ehci->name );
	return -ETIMEDOUT;
}

/******************************************************************************
 *
 * Transfer descriptor rings
 *
 ******************************************************************************
 */

/**
 * Allocate transfer descriptor ring
 *
 * @v ehci              EHCI device
 * @v ring              Transfer descriptor ring
 * @ret rc              Return status code
 */
static int ehci_ring_alloc ( struct ehci_device *ehci,
		struct ehci_ring *ring ) {
	struct ehci_transfer_descriptor *desc;
	struct ehci_transfer_descriptor *next;
	unsigned int i;
	size_t len;
	uint32_t link;
	int rc;

	/* Initialise structure */
	memset ( ring, 0, sizeof ( *ring ) );

	/* Allocate I/O buffers */
	ring->iobuf = zalloc ( EHCI_RING_COUNT * sizeof ( ring->iobuf[0] ) );
	if ( ! ring->iobuf ) {
		rc = -ENOMEM;
		goto err_alloc_iobuf;
	}

	/* Allocate queue head */
	ring->head = malloc_phys ( sizeof ( *ring->head ),
			ehci_align ( sizeof ( *ring->head ) ) );
	if ( ! ring->head ) {
		rc = -ENOMEM;
		goto err_alloc_queue;
	}
	if ( ( rc = ehci_ctrl_reachable ( ehci, ring->head ) ) != 0 ) {
		DBGC ( ehci, "EHCI %s queue head unreachable\n", ehci->name );
		goto err_unreachable_queue;
	}
	memset ( ring->head, 0, sizeof ( *ring->head ) );

	/* Allocate transfer descriptors */
	len = ( EHCI_RING_COUNT * sizeof ( ring->desc[0] ) );
	ring->desc = malloc_phys ( len, sizeof ( ring->desc[0] ) );
	if ( ! ring->desc ) {
		rc = -ENOMEM;
		goto err_alloc_desc;
	}
	memset ( ring->desc, 0, len );

	/* Initialise transfer descriptors */
	for ( i = 0 ; i < EHCI_RING_COUNT ; i++ ) {
		desc = &ring->desc[i];
		if ( ( rc = ehci_ctrl_reachable ( ehci, desc ) ) != 0 ) {
			DBGC ( ehci, "EHCI %s descriptor unreachable\n",
					ehci->name );
			goto err_unreachable_desc;
		}
		next = &ring->desc[ ( i + 1 ) % EHCI_RING_COUNT ];
		link = virt_to_phys ( next );
		desc->next = cpu_to_le32 ( link );
		desc->alt = cpu_to_le32 ( link );
	}

	/* Initialise queue head */
	link = virt_to_phys ( &ring->desc[0] );
	ring->head->cache.next = cpu_to_le32 ( link );

	return 0;

	err_unreachable_desc:
	free_phys ( ring->desc, len );
	err_alloc_desc:
	err_unreachable_queue:
	free_phys ( ring->head, sizeof ( *ring->head ) );
	err_alloc_queue:
	free ( ring->iobuf );
	err_alloc_iobuf:
	return rc;
}

/**
 * Free transfer descriptor ring
 *
 * @v ring              Transfer descriptor ring
 */
static void ehci_ring_free ( struct ehci_ring *ring ) {
	unsigned int i;

	/* Sanity checks */
	ASSERT( ehci_ring_fill ( ring ) == 0 );
	for ( i = 0 ; i < EHCI_RING_COUNT ; i++ )
		ASSERT( ring->iobuf[i] == NULL );

	/* Free transfer descriptors */
	free_phys ( ring->desc, ( EHCI_RING_COUNT *
			sizeof ( ring->desc[0] ) ) );

	/* Free queue head */
	free_phys ( ring->head, sizeof ( *ring->head ) );

	/* Free I/O buffers */
	free ( ring->iobuf );
}

/**
 * Enqueue transfer descriptors
 *
 * @v ehci              EHCI device
 * @v ring              Transfer descriptor ring
 * @v iobuf             I/O buffer
 * @v xfers             Transfers
 * @v count             Number of transfers
 * @ret rc              Return status code
 */
static int ehci_enqueue ( struct ehci_device *ehci, struct ehci_ring *ring,
		struct io_buffer *iobuf,
		const struct ehci_transfer *xfer,
		unsigned int count ) {
	struct ehci_transfer_descriptor *desc;
	physaddr_t phys;
	void *data;
	size_t len;
	size_t offset;
	size_t frag_len;
	unsigned int toggle;
	unsigned int index;
	unsigned int i;

	/* Sanity check */
	ASSERT( iobuf != NULL );
	ASSERT( count > 0 );

	/* Fail if ring does not have sufficient space */
	if ( ehci_ring_remaining ( ring ) < count )
		return -ENOBUFS;

	/* Fail if any portion is unreachable */
	for ( i = 0 ; i < count ; i++ ) {
		if ( ! xfer[i].len )
			continue;
		phys = ( virt_to_phys ( xfer[i].data ) + xfer[i].len - 1 );
		if ( ( phys > 0xffffffffUL ) && ( ! ehci->addr64 ) )
			return -ENOTSUP;
	}

	/* Enqueue each transfer, recording the I/O buffer with the last */
	for ( ; count ; ring->prod++, xfer++ ) {

		/* Populate descriptor header */
		index = ( ring->prod % EHCI_RING_COUNT );
		desc = &ring->desc[index];
		toggle = ( xfer->flags & EHCI_FL_TOGGLE );
		ASSERT( xfer->len <= EHCI_LEN_MASK );
		ASSERT( EHCI_FL_TOGGLE == EHCI_LEN_TOGGLE );
		desc->len = cpu_to_le16 ( xfer->len | toggle );
		desc->flags = ( xfer->flags | EHCI_FL_CERR_MAX );

		/* Populate buffer pointers */
		data = xfer->data;
		len = xfer->len;
		for ( i = 0 ; len ; i++ ) {

			/* Calculate length of this fragment */
			phys = virt_to_phys ( data );
			offset = ( phys & ( EHCI_PAGE_ALIGN - 1 ) );
			frag_len = ( EHCI_PAGE_ALIGN - offset );
			if ( frag_len > len )
				frag_len = len;

			/* Sanity checks */
			ASSERT( ( i == 0 ) || ( offset == 0 ) );
			ASSERT( i < ( sizeof ( desc->low ) /
					sizeof ( desc->low[0] ) ) );

			/* Populate buffer pointer */
			desc->low[i] = cpu_to_le32 ( phys );
			if ( sizeof ( physaddr_t ) > sizeof ( uint32_t ) ) {
				desc->high[i] =
						cpu_to_le32 ( ((uint64_t) phys) >> 32 );
			}

			/* Move to next fragment */
			data += frag_len;
			len -= frag_len;
		}

		/* Ensure everything is valid before activating descriptor */
		wmb();
		desc->status = EHCI_STATUS_ACTIVE;

		/* Record I/O buffer against last ring index */
		if ( --count == 0 )
			ring->iobuf[index] = iobuf;
	}

	return 0;
}

/**
 * Dequeue a transfer descriptor
 *
 * @v ring              Transfer descriptor ring
 * @ret iobuf           I/O buffer (or NULL)
 */
static struct io_buffer * ehci_dequeue ( struct ehci_ring *ring ) {
	struct ehci_transfer_descriptor *desc;
	struct io_buffer *iobuf;
	unsigned int index = ( ring->cons % EHCI_RING_COUNT );

	/* Sanity check */
	ASSERT( ehci_ring_fill ( ring ) > 0 );

	/* Mark descriptor as inactive (and not halted) */
	desc = &ring->desc[index];
	desc->status = 0;

	/* Retrieve I/O buffer */
	iobuf = ring->iobuf[index];
	ring->iobuf[index] = NULL;

	/* Update consumer counter */
	ring->cons++;

	return iobuf;
}

/******************************************************************************
 *
 * Schedule management
 *
 ******************************************************************************
 */

/**
 * Get link value for a queue head
 *
 * @v queue             Queue head
 * @ret link            Link value
 */
static inline uint32_t ehci_link_qh ( struct ehci_queue_head *queue ) {

	return ( virt_to_phys ( queue ) | EHCI_LINK_TYPE_QH );
}

/**
 * (Re)build asynchronous schedule
 *
 * @v ehci              EHCI device
 */
static void ehci_async_schedule ( struct ehci_device *ehci ) {
	struct ehci_endpoint *endpoint;
	struct ehci_queue_head *queue;
	uint32_t link;

	/* Build schedule in reverse order of execution.  Provided
	 * that we only ever add or remove single endpoints, this can
	 * safely run concurrently with hardware execution of the
	 * schedule.
	 */
	link = ehci_link_qh ( ehci->head );
	list_for_each_entry_reverse ( endpoint, &ehci->async, schedule ) {
		queue = endpoint->ring.head;
		queue->link = cpu_to_le32 ( link );
		wmb();
		link = ehci_link_qh ( queue );
	}
	ehci->head->link = cpu_to_le32 ( link );
	wmb();
}

/**
 * Add endpoint to asynchronous schedule
 *
 * @v endpoint          Endpoint
 */
static void ehci_async_add ( struct ehci_endpoint *endpoint ) {
	struct ehci_device *ehci = endpoint->ehci;

	/* Add to end of schedule */
	list_add_tail ( &endpoint->schedule, &ehci->async );

	/* Rebuild schedule */
	ehci_async_schedule ( ehci );
}

/**
 * Remove endpoint from asynchronous schedule
 *
 * @v endpoint          Endpoint
 * @ret rc              Return status code
 */
static int ehci_async_del ( struct ehci_endpoint *endpoint ) {
	struct ehci_device *ehci = endpoint->ehci;
	uint32_t usbcmd;
	uint32_t usbsts;
	unsigned int i;

	/* Remove from schedule */
	list_check_contains_entry ( endpoint, &ehci->async, schedule );
	list_del ( &endpoint->schedule );

	/* Rebuild schedule */
	ehci_async_schedule ( ehci );

	/* Request notification when asynchronous schedule advances */
	usbcmd = readl ( ehci->op + EHCI_OP_USBCMD );
	usbcmd |= EHCI_USBCMD_ASYNC_ADVANCE;
	writel ( usbcmd, ehci->op + EHCI_OP_USBCMD );

	/* Wait for asynchronous schedule to advance */
	for ( i = 0 ; i < EHCI_ASYNC_ADVANCE_MAX_WAIT_MS ; i++ ) {

		/* Check for asynchronous schedule advancing */
		usbsts = readl ( ehci->op + EHCI_OP_USBSTS );
		if ( usbsts & EHCI_USBSTS_ASYNC_ADVANCE ) {
			usbsts &= ~EHCI_USBSTS_CHANGE;
			usbsts |= EHCI_USBSTS_ASYNC_ADVANCE;
			writel ( usbsts, ehci->op + EHCI_OP_USBSTS );
			return 0;
		}

		/* Delay */
		mdelay ( 1 );
	}

	/* Bad things will probably happen now */
	DBGC ( ehci, "EHCI %s timed out waiting for asynchronous schedule "
			"to advance\n", ehci->name );
	return -ETIMEDOUT;
}

/**
 * (Re)build periodic schedule
 *
 * @v ehci              EHCI device
 */
static void ehci_periodic_schedule ( struct ehci_device *ehci ) {
	struct ehci_endpoint *endpoint;
	struct ehci_queue_head *queue;
	uint32_t link;
	unsigned int frames;
	unsigned int max_interval;
	unsigned int i;

	/* Build schedule in reverse order of execution.  Provided
	 * that we only ever add or remove single endpoints, this can
	 * safely run concurrently with hardware execution of the
	 * schedule.
	 */
	DBGCP ( ehci, "EHCI %s periodic schedule: ", ehci->name );
	link = EHCI_LINK_TERMINATE;
	list_for_each_entry_reverse ( endpoint, &ehci->periodic, schedule ) {
		queue = endpoint->ring.head;
		queue->link = cpu_to_le32 ( link );
		wmb();
		DBGCP ( ehci, "%s%d",
				( ( link == EHCI_LINK_TERMINATE ) ? "" : "<-" ),
				endpoint->ep->interval );
		link = ehci_link_qh ( queue );
	}
	DBGCP ( ehci, "\n" );

	/* Populate periodic frame list */
	DBGCP ( ehci, "EHCI %s periodic frame list:", ehci->name );
	frames = EHCI_PERIODIC_FRAMES ( ehci->flsize );
	for ( i = 0 ; i < frames ; i++ ) {

		/* Calculate maximum interval (in microframes) which
		 * may appear as part of this frame list.
		 */
		if ( i == 0 ) {
			/* Start of list: include all endpoints */
			max_interval = -1U;
		} else {
			/* Calculate highest power-of-two frame interval */
			max_interval = ( 1 << ( ffs ( i ) - 1 ) );
			/* Convert to microframes */
			max_interval <<= 3;
			/* Round up to nearest 2^n-1 */
			max_interval = ( ( max_interval << 1 ) - 1 );
		}

		/* Find first endpoint in schedule satisfying this
		 * maximum interval constraint.
		 */
		link = EHCI_LINK_TERMINATE;
		list_for_each_entry ( endpoint, &ehci->periodic, schedule ) {
			if ( endpoint->ep->interval <= max_interval ) {
				queue = endpoint->ring.head;
				link = ehci_link_qh ( queue );
				DBGCP ( ehci, " %d:%d",
						i, endpoint->ep->interval );
				break;
			}
		}
		ehci->frame[i].link = cpu_to_le32 ( link );
	}
	wmb();
	DBGCP ( ehci, "\n" );
}

/**
 * Add endpoint to periodic schedule
 *
 * @v endpoint          Endpoint
 */
static void ehci_periodic_add ( struct ehci_endpoint *endpoint ) {
	struct ehci_device *ehci = endpoint->ehci;
	struct ehci_endpoint *before;
	unsigned int interval = endpoint->ep->interval;

	/* Find first endpoint with a smaller interval */
	list_for_each_entry ( before, &ehci->periodic, schedule ) {
		if ( before->ep->interval < interval )
			break;
	}
	list_add_tail ( &endpoint->schedule, &before->schedule );

	/* Rebuild schedule */
	ehci_periodic_schedule ( ehci );
}

/**
 * Remove endpoint from periodic schedule
 *
 * @v endpoint          Endpoint
 * @ret rc              Return status code
 */
static int ehci_periodic_del ( struct ehci_endpoint *endpoint ) {
	struct ehci_device *ehci = endpoint->ehci;

	/* Remove from schedule */
	list_check_contains_entry ( endpoint, &ehci->periodic, schedule );
	list_del ( &endpoint->schedule );

	/* Rebuild schedule */
	ehci_periodic_schedule ( ehci );

	/* Delay for a whole USB frame (with a 100% safety margin) */
	mdelay ( 2 );

	return 0;
}

/**
 * Add endpoint to appropriate schedule
 *
 * @v endpoint          Endpoint
 */
static void ehci_schedule_add ( struct ehci_endpoint *endpoint ) {
	struct usb_endpoint *ep = endpoint->ep;
	unsigned int attr = ( ep->attributes & USB_ENDPOINT_ATTR_TYPE_MASK );

	if ( attr == USB_ENDPOINT_ATTR_INTERRUPT ) {
		ehci_periodic_add ( endpoint );
	} else {
		ehci_async_add ( endpoint );
	}
}

/**
 * Remove endpoint from appropriate schedule
 *
 * @v endpoint          Endpoint
 * @ret rc              Return status code
 */
static int ehci_schedule_del ( struct ehci_endpoint *endpoint ) {
	struct usb_endpoint *ep = endpoint->ep;
	unsigned int attr = ( ep->attributes & USB_ENDPOINT_ATTR_TYPE_MASK );

	if ( attr == USB_ENDPOINT_ATTR_INTERRUPT ) {
		return ehci_periodic_del ( endpoint );
	} else {
		return ehci_async_del ( endpoint );
	}
}

/******************************************************************************
 *
 * Endpoint operations
 *
 ******************************************************************************
 */

/**
 * Determine endpoint characteristics
 *
 * @v ep                USB endpoint
 * @ret chr             Endpoint characteristics
 */
static uint32_t ehci_endpoint_characteristics ( struct usb_endpoint *ep ) {
	struct usb_device *usb = ep->usb;
	unsigned int attr = ( ep->attributes & USB_ENDPOINT_ATTR_TYPE_MASK );
	uint32_t chr;

	/* Determine basic characteristics */
	chr = ( EHCI_CHR_ADDRESS ( usb->address ) |
			EHCI_CHR_ENDPOINT ( ep->address ) |
			EHCI_CHR_MAX_LEN ( ep->mtu ) );

	/* Control endpoints require manual control of the data toggle */
	if ( attr == USB_ENDPOINT_ATTR_CONTROL )
		chr |= EHCI_CHR_TOGGLE;

	/* Determine endpoint speed */
	if ( usb->speed == USB_SPEED_HIGH ) {
		chr |= EHCI_CHR_EPS_HIGH;
	} else {
		if ( usb->speed == USB_SPEED_FULL ) {
			chr |= EHCI_CHR_EPS_FULL;
		} else {
			chr |= EHCI_CHR_EPS_LOW;
		}
		if ( attr == USB_ENDPOINT_ATTR_CONTROL )
			chr |= EHCI_CHR_CONTROL;
	}

	return chr;
}

/**
 * Determine endpoint capabilities
 *
 * @v ep                USB endpoint
 * @ret cap             Endpoint capabilities
 */
static uint32_t ehci_endpoint_capabilities ( struct usb_endpoint *ep ) {
	struct usb_device *usb = ep->usb;
	struct usb_port *tt = usb_transaction_translator ( usb );
	unsigned int attr = ( ep->attributes & USB_ENDPOINT_ATTR_TYPE_MASK );
	uint32_t cap;
	unsigned int i;

	/* Determine basic capabilities */
	cap = EHCI_CAP_MULT ( ep->burst + 1 );

	/* Determine interrupt schedule mask, if applicable */
	if ( ( attr == USB_ENDPOINT_ATTR_INTERRUPT ) &&
			( ( ep->interval != 0 ) /* avoid infinite loop */ ) ) {
		for ( i = 0 ; i < 8 /* microframes per frame */ ;
				i += ep->interval ) {
			cap |= EHCI_CAP_INTR_SCHED ( i );
		}
	}

	/* Set transaction translator hub address and port, if applicable */
	if ( tt ) {
		ASSERT( tt->hub->usb );
		cap |= ( EHCI_CAP_TT_HUB ( tt->hub->usb->address ) |
				EHCI_CAP_TT_PORT ( tt->address ) );
		if ( attr == USB_ENDPOINT_ATTR_INTERRUPT )
			cap |= EHCI_CAP_SPLIT_SCHED_DEFAULT;
	}

	return cap;
}

/**
 * Update endpoint characteristics and capabilities
 *
 * @v ep                USB endpoint
 */
static void ehci_endpoint_update ( struct usb_endpoint *ep ) {
	struct ehci_endpoint *endpoint = usb_endpoint_get_hostdata ( ep );
	struct ehci_queue_head *head;

	/* Update queue characteristics and capabilities */
	head = endpoint->ring.head;
	head->chr = cpu_to_le32 ( ehci_endpoint_characteristics ( ep ) );
	head->cap = cpu_to_le32 ( ehci_endpoint_capabilities ( ep ) );
}

/**
 * Open endpoint
 *
 * @v ep                USB endpoint
 * @ret rc              Return status code
 */
static int ehci_endpoint_open ( struct usb_endpoint *ep ) {
	struct usb_device *usb = ep->usb;
	struct ehci_device *ehci = usb_get_hostdata ( usb );
	struct ehci_endpoint *endpoint;
	int rc;

	/* Allocate and initialise structure */
	endpoint = zalloc ( sizeof ( *endpoint ) );
	if ( ! endpoint ) {
		rc = -ENOMEM;
		goto err_alloc;
	}
	endpoint->ehci = ehci;
	endpoint->ep = ep;
	usb_endpoint_set_hostdata ( ep, endpoint );

	/* Initialise descriptor ring */
	if ( ( rc = ehci_ring_alloc ( ehci, &endpoint->ring ) ) != 0 )
		goto err_ring_alloc;

	/* Update queue characteristics and capabilities */
	ehci_endpoint_update ( ep );

	/* Add to list of endpoints */
	list_add_tail ( &endpoint->list, &ehci->endpoints );

	/* Add to schedule */
	ehci_schedule_add ( endpoint );

	return 0;

	ehci_ring_free ( &endpoint->ring );
	err_ring_alloc:
	free ( endpoint );
	err_alloc:
	return rc;
}

/**
 * Close endpoint
 *
 * @v ep                USB endpoint
 */
static void ehci_endpoint_close ( struct usb_endpoint *ep ) {
	struct ehci_endpoint *endpoint = usb_endpoint_get_hostdata ( ep );
	struct ehci_device *ehci = endpoint->ehci;
	struct usb_device *usb = ep->usb;
	struct io_buffer *iobuf;
	int rc;

	/* Remove from schedule */
	if ( ( rc = ehci_schedule_del ( endpoint ) ) != 0 ) {
		/* No way to prevent hardware from continuing to
		 * access the memory, so leak it.
		 */
		DBGC ( ehci, "EHCI %s %s could not unschedule: %s\n",
				usb->name, usb_endpoint_name ( ep ), strerror ( rc ) );
		return;
	}

	/* Cancel any incomplete transfers */
	while ( ehci_ring_fill ( &endpoint->ring ) ) {
		iobuf = ehci_dequeue ( &endpoint->ring );
		if ( iobuf )
			usb_complete_err ( ep, iobuf, -ECANCELED );
	}

	/* Remove from list of endpoints */
	list_del ( &endpoint->list );

	/* Free descriptor ring */
	ehci_ring_free ( &endpoint->ring );

	/* Free endpoint */
	free ( endpoint );
}

/**
 * Reset endpoint
 *
 * @v ep                USB endpoint
 * @ret rc              Return status code
 */
static int ehci_endpoint_reset ( struct usb_endpoint *ep ) {
	struct ehci_endpoint *endpoint = usb_endpoint_get_hostdata ( ep );
	struct ehci_ring *ring = &endpoint->ring;
	struct ehci_transfer_descriptor *cache = &ring->head->cache;
	uint32_t link;

	/* Sanity checks */
	ASSERT( ! ( cache->status & EHCI_STATUS_ACTIVE ) );
	ASSERT( cache->status & EHCI_STATUS_HALTED );

	/* Reset residual count */
	ring->residual = 0;

	/* Reset data toggle */
	cache->len = 0;

	/* Prepare to restart at next unconsumed descriptor */
	link = virt_to_phys ( &ring->desc[ ring->cons % EHCI_RING_COUNT ] );
	cache->next = cpu_to_le32 ( link );

	/* Restart ring */
	wmb();
	cache->status = 0;

	return 0;
}

/**
 * Update MTU
 *
 * @v ep                USB endpoint
 * @ret rc              Return status code
 */
static int ehci_endpoint_mtu ( struct usb_endpoint *ep ) {

	/* Update endpoint characteristics and capabilities */
	ehci_endpoint_update ( ep );

	return 0;
}

/**
 * Enqueue message transfer
 *
 * @v ep                USB endpoint
 * @v iobuf             I/O buffer
 * @ret rc              Return status code
 */
static int ehci_endpoint_message ( struct usb_endpoint *ep,
		struct io_buffer *iobuf ) {
	struct ehci_endpoint *endpoint = usb_endpoint_get_hostdata ( ep );
	struct ehci_device *ehci = endpoint->ehci;
	struct usb_setup_packet *packet;
	unsigned int input;
	struct ehci_transfer xfers[3];
	struct ehci_transfer *xfer = xfers;
	size_t len;
	int rc;

	/* Construct setup stage */
	ASSERT( iob_len ( iobuf ) >= sizeof ( *packet ) );
	packet = iobuf->data;
	iob_pull ( iobuf, sizeof ( *packet ) );
	xfer->data = packet;
	xfer->len = sizeof ( *packet );
	xfer->flags = EHCI_FL_PID_SETUP;
	xfer++;

	/* Construct data stage, if applicable */
	len = iob_len ( iobuf );
	input = ( packet->request & cpu_to_le16 ( USB_DIR_IN ) );
	if ( len ) {
		xfer->data = iobuf->data;
		xfer->len = len;
		xfer->flags = ( EHCI_FL_TOGGLE |
				( input ? EHCI_FL_PID_IN : EHCI_FL_PID_OUT ) );
		xfer++;
	}

	/* Construct status stage */
	xfer->data = NULL;
	xfer->len = 0;
	xfer->flags = ( EHCI_FL_TOGGLE | EHCI_FL_IOC |
			( ( len && input ) ? EHCI_FL_PID_OUT : EHCI_FL_PID_IN));
	xfer++;

	/* Enqueue transfer */
	if ( ( rc = ehci_enqueue ( ehci, &endpoint->ring, iobuf, xfers,
			( xfer - xfers ) ) ) != 0 )
		return rc;

	return 0;
}

/**
 * Calculate number of transfer descriptors
 *
 * @v len               Length of data
 * @v zlp               Append a zero-length packet
 * @ret count           Number of transfer descriptors
 */
static unsigned int ehci_endpoint_count ( size_t len, int zlp ) {
	unsigned int count;

	/* Split into 16kB transfers.  A single transfer can handle up
	 * to 20kB if it happens to be page-aligned, or up to 16kB
	 * with arbitrary alignment.  We simplify the code by assuming
	 * that we can fit only 16kB into each transfer.
	 */
	count = ( ( len + EHCI_MTU - 1 ) / EHCI_MTU );

	/* Append a zero-length transfer if applicable */
	if ( zlp || ( count == 0 ) )
		count++;

	return count;
}

/**
 * Enqueue stream transfer
 *
 * @v ep                USB endpoint
 * @v iobuf             I/O buffer
 * @v zlp               Append a zero-length packet
 * @ret rc              Return status code
 */
static int ehci_endpoint_stream ( struct usb_endpoint *ep,
		struct io_buffer *iobuf, int zlp ) {
	struct ehci_endpoint *endpoint = usb_endpoint_get_hostdata ( ep );
	struct ehci_device *ehci = endpoint->ehci;
	void *data = iobuf->data;
	size_t len = iob_len ( iobuf );
	unsigned int count = ehci_endpoint_count ( len, zlp );
	unsigned int input = ( ep->address & USB_DIR_IN );
	unsigned int flags = ( input ? EHCI_FL_PID_IN : EHCI_FL_PID_OUT );
	struct ehci_transfer xfers[count];
	struct ehci_transfer *xfer = xfers;
	size_t xfer_len;
	unsigned int i;
	int rc;

	/* Create transfers */
	for ( i = 0 ; i < count ; i++ ) {

		/* Calculate transfer length */
		xfer_len = EHCI_MTU;
		if ( xfer_len > len )
			xfer_len = len;

		/* Create transfer */
		xfer->data = data;
		xfer->len = xfer_len;
		xfer->flags = flags;

		/* Move to next transfer */
		data += xfer_len;
		len -= xfer_len;
		xfer++;
	}
	xfer[-1].flags |= EHCI_FL_IOC;

	/* Enqueue transfer */
	if ( ( rc = ehci_enqueue ( ehci, &endpoint->ring, iobuf, xfers,
			count ) ) != 0 )
		return rc;

	return 0;
}

/**
 * Poll for completions
 *
 * @v endpoint          Endpoint
 */
static void ehci_endpoint_poll ( struct ehci_endpoint *endpoint ) {
	struct ehci_device *ehci = endpoint->ehci;
	struct ehci_ring *ring = &endpoint->ring;
	struct ehci_transfer_descriptor *desc;
	struct usb_endpoint *ep = endpoint->ep;
	struct usb_device *usb = ep->usb;
	struct io_buffer *iobuf;
	unsigned int index;
	unsigned int status;
	int rc;

	/* Consume all completed descriptors */
	while ( ehci_ring_fill ( &endpoint->ring ) ) {

		/* Stop if we reach an uncompleted descriptor */
		rmb();
		index = ( ring->cons % EHCI_RING_COUNT );
		desc = &ring->desc[index];
		status = desc->status;
		if ( status & EHCI_STATUS_ACTIVE )
			break;

		/* Consume this descriptor */
		iobuf = ehci_dequeue ( ring );

		/* If we have encountered an error, then consume all
		 * remaining descriptors in this transaction, report
		 * the error to the USB core, and stop further
		 * processing.
		 */
		if ( status & EHCI_STATUS_HALTED ) {
			rc = -EIO_STATUS ( status );
			DBGC ( ehci, "EHCI %s %s completion %d failed (status "
					"%02x): %s\n", usb->name,
					usb_endpoint_name ( ep ), index, status,
					strerror ( rc ) );
			while ( ! iobuf )
				iobuf = ehci_dequeue ( ring );
			usb_complete_err ( endpoint->ep, iobuf, rc );
			return;
		}

		/* Accumulate residual data count */
		ring->residual += ( le16_to_cpu ( desc->len ) & EHCI_LEN_MASK );

		/* If this is not the end of a transaction (i.e. has
		 * no I/O buffer), then continue to next descriptor.
		 */
		if ( ! iobuf )
			continue;

		/* Update I/O buffer length */
		iob_unput ( iobuf, ring->residual );
		ring->residual = 0;

		/* Report completion to USB core */
		usb_complete ( endpoint->ep, iobuf );
	}
}

/******************************************************************************
 *
 * Device operations
 *
 ******************************************************************************
 */

/**
 * Open device
 *
 * @v usb               USB device
 * @ret rc              Return status code
 */
static int ehci_device_open ( struct usb_device *usb ) {
	struct ehci_device *ehci = usb_bus_get_hostdata ( usb->port->hub->bus );

	usb_set_hostdata ( usb, ehci );
	return 0;
}

/**
 * Close device
 *
 * @v usb               USB device
 */
static void ehci_device_close ( struct usb_device *usb ) {
	struct ehci_device *ehci = usb_get_hostdata ( usb );
	struct usb_bus *bus = ehci->bus;

	/* Free device address, if assigned */
	if ( usb->address )
		usb_free_address ( bus, usb->address );
}

/**
 * Assign device address
 *
 * @v usb               USB device
 * @ret rc              Return status code
 */
static int ehci_device_address ( struct usb_device *usb ) {
	struct ehci_device *ehci = usb_get_hostdata ( usb );
	struct usb_bus *bus = ehci->bus;
	struct usb_endpoint *ep0 = usb_endpoint ( usb, USB_EP0_ADDRESS );
	int address;
	int rc;

	/* Sanity checks */
	ASSERT( usb->address == 0 );
	ASSERT( ep0 != NULL );

	/* Allocate device address */
	address = usb_alloc_address ( bus );
	if ( address < 0 ) {
		rc = address;
		DBGC ( ehci, "EHCI %s could not allocate address: %s\n",
				usb->name, strerror ( rc ) );
		goto err_alloc_address;
	}

	/* Set address */
	if ( ( rc = usb_set_address ( usb, address ) ) != 0 )
		goto err_set_address;

	/* Update device address */
	usb->address = address;

	/* Update control endpoint characteristics and capabilities */
	ehci_endpoint_update ( ep0 );

	return 0;

	err_set_address:
	usb_free_address ( bus, address );
	err_alloc_address:
	return rc;
}

/******************************************************************************
 *
 * Hub operations
 *
 ******************************************************************************
 */

/**
 * Open hub
 *
 * @v hub               USB hub
 * @ret rc              Return status code
 */
static int ehci_hub_open ( struct usb_hub *hub __unused ) {

	/* Nothing to do */
	return 0;
}

/**
 * Close hub
 *
 * @v hub               USB hub
 */
static void ehci_hub_close ( struct usb_hub *hub __unused ) {

	/* Nothing to do */
}

/******************************************************************************
 *
 * Root hub operations
 *
 ******************************************************************************
 */

/**
 * Open root hub
 *
 * @v hub               USB hub
 * @ret rc              Return status code
 */
static int ehci_root_open ( struct usb_hub *hub ) {
	struct ehci_device *ehci = usb_hub_get_drvdata ( hub );
	uint32_t portsc;
	unsigned int i;

	/* Route all ports to EHCI controller */
	writel ( EHCI_CONFIGFLAG_CF, ehci->op + EHCI_OP_CONFIGFLAG );

	/* Enable power to all ports */
	for ( i = 1 ; i <= ehci->ports ; i++ ) {
		portsc = readl ( ehci->op + EHCI_OP_PORTSC ( i ) );
		portsc &= ~EHCI_PORTSC_CHANGE;
		portsc |= EHCI_PORTSC_PP;
		writel ( portsc, ehci->op + EHCI_OP_PORTSC ( i ) );
	}

	/* Wait 20ms after potentially enabling power to a port */
	mdelay ( EHCI_PORT_POWER_DELAY_MS );

	return 0;
}

/**
 * Close root hub
 *
 * @v hub               USB hub
 */
static void ehci_root_close ( struct usb_hub *hub ) {
	struct ehci_device *ehci = usb_hub_get_drvdata ( hub );

	/* Route all ports back to companion controllers */
	writel ( 0, ehci->op + EHCI_OP_CONFIGFLAG );
}

/**
 * Enable port
 *
 * @v hub               USB hub
 * @v port              USB port
 * @ret rc              Return status code
 */
static int ehci_root_enable ( struct usb_hub *hub, struct usb_port *port ) {
	struct ehci_device *ehci = usb_hub_get_drvdata ( hub );
	uint32_t portsc;
	unsigned int line;
	unsigned int i;

	/* Check for a low-speed device */
	portsc = readl ( ehci->op + EHCI_OP_PORTSC ( port->address ) );
	line = EHCI_PORTSC_LINE_STATUS ( portsc );
	if ( line == EHCI_PORTSC_LINE_STATUS_LOW ) {
		DBGC ( ehci, "EHCI %s-%d detected low-speed device: "
				"disowning\n", ehci->name, port->address );
		goto disown;
	}

	/* Reset port */
	portsc &= ~( EHCI_PORTSC_PED | EHCI_PORTSC_CHANGE );
	portsc |= EHCI_PORTSC_PR;
	writel ( portsc, ehci->op + EHCI_OP_PORTSC ( port->address ) );
	mdelay ( USB_RESET_DELAY_MS );
	portsc &= ~EHCI_PORTSC_PR;
	writel ( portsc, ehci->op + EHCI_OP_PORTSC ( port->address ) );

	/* Wait for reset to complete */
	for ( i = 0 ; i < EHCI_PORT_RESET_MAX_WAIT_MS ; i++ ) {

		/* Check port status */
		portsc = readl ( ehci->op + EHCI_OP_PORTSC ( port->address ) );
		if ( ! ( portsc & EHCI_PORTSC_PR ) ) {
			if ( portsc & EHCI_PORTSC_PED )
				return 0;
			DBGC ( ehci, "EHCI %s-%d not enabled after reset: "
					"disowning\n", ehci->name, port->address );
			goto disown;
		}

		/* Delay */
		mdelay ( 1 );
	}

	DBGC ( ehci, "EHCI %s-%d timed out waiting for port to reset\n",
			ehci->name, port->address );
	return -ETIMEDOUT;

	disown:
	/* Disown port */
	portsc &= ~EHCI_PORTSC_CHANGE;
	portsc |= EHCI_PORTSC_OWNER;
	writel ( portsc, ehci->op + EHCI_OP_PORTSC ( port->address ) );

	/* Delay to allow child companion controllers to settle */
	mdelay ( EHCI_DISOWN_DELAY_MS );

	/* Poll child companion controllers */
	ehci_poll_companions ( ehci );

	return -ENODEV;
}

/**
 * Disable port
 *
 * @v hub               USB hub
 * @v port              USB port
 * @ret rc              Return status code
 */
static int ehci_root_disable ( struct usb_hub *hub, struct usb_port *port ) {
	struct ehci_device *ehci = usb_hub_get_drvdata ( hub );
	uint32_t portsc;

	/* Disable port */
	portsc = readl ( ehci->op + EHCI_OP_PORTSC ( port->address ) );
	portsc &= ~( EHCI_PORTSC_PED | EHCI_PORTSC_CHANGE );
	writel ( portsc, ehci->op + EHCI_OP_PORTSC ( port->address ) );

	return 0;
}

/**
 * Update root hub port speed
 *
 * @v hub               USB hub
 * @v port              USB port
 * @ret rc              Return status code
 */
static int ehci_root_speed ( struct usb_hub *hub, struct usb_port *port ) {
	struct ehci_device *ehci = usb_hub_get_drvdata ( hub );
	uint32_t portsc;
	unsigned int speed;
	unsigned int line;
	int ccs;
	int csc;
	int ped;

	/* Read port status */
	portsc = readl ( ehci->op + EHCI_OP_PORTSC ( port->address ) );
	DBGC2 ( ehci, "EHCI %s-%d status is %08x\n",
			ehci->name, port->address, portsc );
	ccs = ( portsc & EHCI_PORTSC_CCS );
	csc = ( portsc & EHCI_PORTSC_CSC );
	ped = ( portsc & EHCI_PORTSC_PED );
	line = EHCI_PORTSC_LINE_STATUS ( portsc );

	/* Record disconnections and clear changes */
	port->disconnected |= csc;
	writel ( portsc, ehci->op + EHCI_OP_PORTSC ( port->address ) );

	/* Determine port speed */
	if ( ! ccs ) {
		/* Port not connected */
		speed = USB_SPEED_NONE;
	} else if ( line == EHCI_PORTSC_LINE_STATUS_LOW ) {
		/* Detected as low-speed */
		speed = USB_SPEED_LOW;
	} else if ( ped ) {
		/* Port already enabled: must be high-speed */
		speed = USB_SPEED_HIGH;
	} else {
		/* Not low-speed and not yet enabled.  Could be either
		 * full-speed or high-speed; we can't yet tell.
		 */
		speed = USB_SPEED_FULL;
	}
	port->speed = speed;
	return 0;
}

/**
 * Clear transaction translator buffer
 *
 * @v hub               USB hub
 * @v port              USB port
 * @v ep                USB endpoint
 * @ret rc              Return status code
 */
static int ehci_root_clear_tt ( struct usb_hub *hub, struct usb_port *port,
		struct usb_endpoint *ep ) {
	struct ehci_device *ehci = usb_hub_get_drvdata ( hub );

	/* Should never be called; this is a root hub */
	DBGC ( ehci, "EHCI %s-%d nonsensical CLEAR_TT for %s %s\n", ehci->name,
			port->address, ep->usb->name, usb_endpoint_name ( ep ) );

	return -ENOTSUP;
}

/**
 * Poll for port status changes
 *
 * @v hub               USB hub
 * @v port              USB port
 */
static void ehci_root_poll ( struct usb_hub *hub, struct usb_port *port ) {
	struct ehci_device *ehci = usb_hub_get_drvdata ( hub );
	uint32_t portsc;
	uint32_t change;

	/* Do nothing unless something has changed */
	portsc = readl ( ehci->op + EHCI_OP_PORTSC ( port->address ) );
	change = ( portsc & EHCI_PORTSC_CHANGE );
	if ( ! change )
		return;

	/* Record disconnections and clear changes */
	port->disconnected |= ( portsc & EHCI_PORTSC_CSC );
	writel ( portsc, ehci->op + EHCI_OP_PORTSC ( port->address ) );

	/* Report port status change */
	usb_port_changed ( port );
}

/******************************************************************************
 *
 * Bus operations
 *
 ******************************************************************************
 */

/**
 * Open USB bus
 *
 * @v bus               USB bus
 * @ret rc              Return status code
 */
static int ehci_bus_open ( struct usb_bus *bus ) {
	struct ehci_device *ehci = usb_bus_get_hostdata ( bus );
	unsigned int frames;
	size_t len;
	int rc;

	/* Sanity checks */
	ASSERT( list_empty ( &ehci->async ) );
	ASSERT( list_empty ( &ehci->periodic ) );

	/* Allocate and initialise asynchronous queue head */
	ehci->head = malloc_phys ( sizeof ( *ehci->head ),
			ehci_align ( sizeof ( *ehci->head ) ) );
	if ( ! ehci->head ) {
		rc = -ENOMEM;
		goto err_alloc_head;
	}
	memset ( ehci->head, 0, sizeof ( *ehci->head ) );
	ehci->head->chr = cpu_to_le32 ( EHCI_CHR_HEAD );
	ehci->head->cache.next = cpu_to_le32 ( EHCI_LINK_TERMINATE );
	ehci->head->cache.status = EHCI_STATUS_HALTED;
	ehci_async_schedule ( ehci );
	writel ( virt_to_phys ( ehci->head ),
			ehci->op + EHCI_OP_ASYNCLISTADDR );

	/* Use async queue head to determine control data structure segment */
	ehci->ctrldssegment =
			( ( ( uint64_t ) virt_to_phys ( ehci->head ) ) >> 32 );
	if ( ehci->addr64 ) {
		writel ( ehci->ctrldssegment, ehci->op + EHCI_OP_CTRLDSSEGMENT);
	} else if ( ehci->ctrldssegment ) {
		DBGC ( ehci, "EHCI %s CTRLDSSEGMENT not supported\n",
				ehci->name );
		rc = -ENOTSUP;
		goto err_ctrldssegment;
	}

	/* Allocate periodic frame list */
	frames = EHCI_PERIODIC_FRAMES ( ehci->flsize );
	len = ( frames * sizeof ( ehci->frame[0] ) );
	ehci->frame = malloc_phys ( len, EHCI_PAGE_ALIGN );
	if ( ! ehci->frame ) {
		rc = -ENOMEM;
		goto err_alloc_frame;
	}
	if ( ( rc = ehci_ctrl_reachable ( ehci, ehci->frame ) ) != 0 ) {
		DBGC ( ehci, "EHCI %s frame list unreachable\n", ehci->name );
		goto err_unreachable_frame;
	}
	ehci_periodic_schedule ( ehci );
	writel ( virt_to_phys ( ehci->frame ),
			ehci->op + EHCI_OP_PERIODICLISTBASE );

	/* Start controller */
	ehci_run ( ehci );

	return 0;

	ehci_stop ( ehci );
	err_unreachable_frame:
	free_phys ( ehci->frame, len );
	err_alloc_frame:
	err_ctrldssegment:
	free_phys ( ehci->head, sizeof ( *ehci->head ) );
	err_alloc_head:
	return rc;
}

/**
 * Close USB bus
 *
 * @v bus               USB bus
 */
static void ehci_bus_close ( struct usb_bus *bus ) {
	struct ehci_device *ehci = usb_bus_get_hostdata ( bus );
	unsigned int frames = EHCI_PERIODIC_FRAMES ( ehci->flsize );

	/* Sanity checks */
	ASSERT( list_empty ( &ehci->async ) );
	ASSERT( list_empty ( &ehci->periodic ) );

	/* Stop controller */
	ehci_stop ( ehci );

	/* Free periodic frame list */
	free_phys ( ehci->frame, ( frames * sizeof ( ehci->frame[0] ) ) );

	/* Free asynchronous schedule */
	free_phys ( ehci->head, sizeof ( *ehci->head ) );
}

/**
 * Poll USB bus
 *
 * @v bus               USB bus
 */
static void ehci_bus_poll ( struct usb_bus *bus ) {
	struct ehci_device *ehci = usb_bus_get_hostdata ( bus );
	struct usb_hub *hub = bus->hub;
	struct ehci_endpoint *endpoint;
	unsigned int i;
	uint32_t usbsts;
	uint32_t change;

	/* Do nothing unless something has changed */
	usbsts = readl ( ehci->op + EHCI_OP_USBSTS );
	ASSERT( usbsts & EHCI_USBSTS_ASYNC );
	ASSERT( usbsts & EHCI_USBSTS_PERIODIC );
	ASSERT( ! ( usbsts & EHCI_USBSTS_HCH ) );
	change = ( usbsts & EHCI_USBSTS_CHANGE );
	if ( ! change )
		return;

	/* Acknowledge changes */
	writel ( usbsts, ehci->op + EHCI_OP_USBSTS );

	/* Process completions, if applicable */
	if ( change & ( EHCI_USBSTS_USBINT | EHCI_USBSTS_USBERRINT ) ) {

		/* Iterate over all endpoints looking for completed
		 * descriptors.  We trust that completion handlers are
		 * minimal and will not do anything that could
		 * plausibly affect the endpoint list itself.
		 */
		list_for_each_entry ( endpoint, &ehci->endpoints, list )
                        		 ehci_endpoint_poll ( endpoint );
	}

	/* Process port status changes, if applicable */
	if ( change & EHCI_USBSTS_PORT ) {

		/* Iterate over all ports looking for status changes */
		for ( i = 1 ; i <= ehci->ports ; i++ )
			ehci_root_poll ( hub, usb_port ( hub, i ) );
	}

	/* Report fatal errors */
	if ( change & EHCI_USBSTS_SYSERR )
		DBGC ( ehci, "EHCI %s host system error\n", ehci->name );
}

#endif

HAL_StatusTypeDef EHCI_DriveVbus(USB_EHCI_CapabilityTypeDef *const EHCIx, uint8_t state) {
	PRINTF("EHCI_DriveVbus: state=%d\n", (int) state);
	board_set_usbhostvbuson(state);
	board_update();
	return HAL_OK;
}

HAL_StatusTypeDef EHCI_StopHost(USB_EHCI_CapabilityTypeDef *const EHCIx) {

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


/**
  * @brief  Handle USB_EHCI interrupt request.
  * @param  hehci USB_EHCI handle
  * @retval None
  */
void HAL_EHCI_IRQHandler(EHCI_HandleTypeDef * hehci)
{
	unsigned i;
 	USB_EHCI_CapabilityTypeDef * const EHCIx = hehci->Instance;
 	PRINTF("HAL_EHCI_IRQHandler: USBSTS=%08lX\n", EHCIx->USBSTS);
 	for (i = 0; i < hehci->nports; ++ i)
 	{
 		PRINTF("HAL_EHCI_IRQHandler: PORTSC[%u]=%08lX\n", i, hehci->portsc [i]);
 	}

 	const uint_fast32_t usbsts = EHCIx->USBSTS;

 	if ((usbsts & (0x01uL << 0)))	// USB Interrupt (USBINT)
 	{
 		EHCIx->USBSTS = (0x01uL << 0);	// Clear USB Interrupt (USBINT)
 	}

 	if ((usbsts & (0x01uL << 1)))	// USB Error Interrupt (USBERRINT)
 	{
 		EHCIx->USBSTS = (0x01uL << 1);	// Clear USB Error Interrupt (USBERRINT) interrupt
 	}

 	if ((usbsts & (0x01uL << 2)))	// Port Change Detect
 	{
 		EHCIx->USBSTS = (0x01uL << 2);	// Clear Port Change Detect interrupt
 	}

 	if ((usbsts & (0x01uL << 3)))	// Frame List Rollower
 	{
 		EHCIx->USBSTS = (0x01uL << 3);	// Clear Frame List Rollower interrupt
 	}

 	if ((usbsts & (0x01uL << 4)))	// Host System Error
 	{
 		EHCIx->USBSTS = (0x01uL << 4);	// Clear Host System Error interrupt
 	}

 	if ((usbsts & (0x01uL << 5)))	// Interrupt On Async Advance
 	{
 		EHCIx->USBSTS = (0x01uL << 5);	// Clear Interrupt On Async Advance
 	}
}

HAL_StatusTypeDef HAL_EHCI_Init(EHCI_HandleTypeDef *hehci)
{
 	USB_EHCI_CapabilityTypeDef * const EHCIx = hehci->Instance;
 	PRINTF("HAL_EHCI_Init\n");
 	return HAL_OK;
}

HAL_StatusTypeDef HAL_EHCI_DeInit(EHCI_HandleTypeDef *hehci)
{

	return HAL_OK;
}

void USBH_OHCI_IRQHandler(void)
{
	HAL_EHCI_IRQHandler(& hhcd_USB_EHCI);
}

void USBH_EHCI_IRQHandler(void)
{
	HAL_EHCI_IRQHandler(& hhcd_USB_EHCI);
}

void HAL_EHCI_MspInit(EHCI_HandleTypeDef * hehci)
{
#if CPUSTYLE_STM32MP1
 	PRINTF("HAL_EHCI_MspInit\n");
	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_USBHEN;
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6LPENSETR = RCC_MP_AHB6LPENSETR_USBHLPEN;
	(void) RCC->MP_AHB6LPENSETR;
	if (0) {
		/* SYSCFG clock enable */
		RCC->MP_APB3ENSETR = RCC_MP_APB3ENSETR_SYSCFGEN;
		(void) RCC->MP_APB3ENSETR;
		RCC->MP_APB3LPENSETR = RCC_MP_APB3LPENSETR_SYSCFGLPEN;
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
	USB_HS_PHYCInit();	// move to USB_CoreInit

	arm_hardware_set_handler_system(USBH_OHCI_IRQn, USBH_OHCI_IRQHandler);
	arm_hardware_set_handler_system(USBH_EHCI_IRQn, USBH_EHCI_IRQHandler);

#else

	#warning HAL_EHCI_MspInit Not implemented for CPUSTYLE_xxxxx

#endif
}

void HAL_EHCI_MspDeInit(EHCI_HandleTypeDef * hehci)
{
#if CPUSTYLE_STM32MP1

	IRQ_Disable(USBH_OHCI_IRQn);
	IRQ_Disable(USBH_EHCI_IRQn);

	RCC->MP_AHB6ENCLRR = RCC_MP_AHB6LPENCLRR_USBHLPEN;
	(void) RCC->MP_AHB6ENCLRR;
	RCC->MP_AHB6ENCLRR = RCC_MP_AHB6ENCLRR_USBHEN;
	(void) RCC->MP_AHB6ENCLRR;

#else

	#warning HAL_EHCI_MspDeInit Not implemented for CPUSTYLE_xxxxx

#endif
}


/**
  * @brief  Start the host driver.
  * @param  hehci HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_EHCI_Start(EHCI_HandleTypeDef *hehci)
 {
	__HAL_LOCK(hehci);
	__HAL_EHCI_ENABLE(hehci);
	(void) EHCI_DriveVbus(hehci->Instance, 1U);
	__HAL_UNLOCK(hehci);

	board_ehci_initialize(&hhcd_USB_EHCI);		// USB EHCI controller tests

	return HAL_OK;
}

/**
  * @brief  Stop the host driver.
  * @param  hehci HCD handle
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_EHCI_Stop(EHCI_HandleTypeDef *hehci)
{
  __HAL_LOCK(hehci);
  (void)EHCI_StopHost(hehci->Instance);
  __HAL_UNLOCK(hehci);

  return HAL_OK;
}

// USe in HAL_EHCI_Start
 // USB EHCI controller
void board_ehci_initialize(EHCI_HandleTypeDef * hehci)
{
 	PRINTF("board_ehci_initialize start.\n");

 	USB_EHCI_CapabilityTypeDef * const EHCIx = (USB_EHCI_CapabilityTypeDef *) hehci->Instance;
 	HAL_EHCI_MspInit(hehci);

 	static __attribute__((used, aligned(4096))) uint8_t buff0 [4096];

     static __attribute__((used, aligned(32))) uint32_t itd0 [16];
     static __attribute__((used, aligned(32))) uint32_t queue0 [16];

     ehci_itd_fill(itd0);
     ehci_queue_fill(queue0, (uintptr_t) queue0);

     unsigned i;

 	static __attribute__((used, aligned(4096))) uint32_t asyncbuff [1024];
 	for (i = 0; i < ARRAY_SIZE(asyncbuff); ++ i)
     {
     	asyncbuff [i] = 0x01;	// 0 - valid, 1 - invalid
     }

 	//enum { FLS = 1024, FLS_code = 0 };
     //enum { FLS = 512, FLS_code = 1 };
     enum { FLS = 256, FLS_code = 2 };
 	// Periodic frame list
 	static __attribute__((used, aligned(4096))) uint32_t framesbuff [FLS];
     for (i = 0; i < FLS; ++ i)
     {
     	framesbuff [i] = 0x01;	// 0 - valid, 1 - invalid
     }

     framesbuff [0] =
     		(uintptr_t) & itd0 |
 			(0uL << 1) |	//  0 — изосинхронный TD(iTD), 1 — очередь QH,
 			(0uL << 0);	// 0 - valid, 1 - invalid
     framesbuff [1] =
     		(uintptr_t) & queue0 |
 			(1uL << 1) |	//  0 — изосинхронный TD(iTD), 1 — очередь QH,
 			(0uL << 0);	// 0 - valid, 1 - invalid

     arm_hardware_flush((uintptr_t) framesbuff, sizeof framesbuff);
     arm_hardware_flush((uintptr_t) asyncbuff, sizeof asyncbuff);
     arm_hardware_flush((uintptr_t) itd0, sizeof itd0);
     arm_hardware_flush((uintptr_t) queue0, sizeof queue0);

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

 	PRINTF("board_ehci_initialize: HCCAPBASE=%08lX\n", (unsigned long) EHCIx->HCCAPBASE);
 	PRINTF("board_ehci_initialize: HCSPARAMS=%08lX\n", (unsigned long) EHCIx->HCSPARAMS);
 	PRINTF("board_ehci_initialize: HCCPARAMS=%08lX\n", (unsigned long) EHCIx->HCCPARAMS);

 	// Calculate Operational Register Space base address
 	const uintptr_t opregspacebase = (uintptr_t) & EHCIx->HCCAPBASE + (EHCIx->HCCAPBASE & 0x00FF);
 	hehci->nports = (EHCIx->HCSPARAMS >> 0) & 0x0F;
 	hehci->portsc = ((__IO unsigned long *) (opregspacebase + 0x0044));


 	hehci->ehci.opRegs = (EhciOpRegs *) opregspacebase;
 	hehci->ehci.capRegs = (EhciCapRegs *) EHCIx;

 	PRINTF("board_ehci_initialize: nports=%u\n", hehci->nports);
 	for (i = 0; i < hehci->nports; ++ i)
 	{
 		PRINTF("board_ehci_initialize: PORTSC[%u]=%08lX\n", i, hehci->ehci.opRegs->ports [i]);
 	}

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


     // Check extended capabilities
     uint_fast32_t eecp = (EHCIx->HCCPARAMS & HCCPARAMS_EECP_MASK) >> HCCPARAMS_EECP_SHIFT;
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
 	// Disable interrupts
 	// Отключаем прерывания
 	//hc->opRegs->usbIntr = 0;
     EHCIx->USBINTR = 0;
 	// Setup frame list
 	// Устанавливаем ссылку на фреймлист
 	//hc->opRegs->frameIndex = 0;
     EHCIx->FRINDEX = 0;
 	//hc->opRegs->periodicListBase = (u32)(uintptr_t)hc->frameList;
     EHCIx->PERIODICLISTBASE = (uintptr_t) framesbuff;
 	// копируем адрес асинхронной очереди в регистр
 	//hc->opRegs->asyncListAddr = (u32)(uintptr_t)hc->asyncQH;
     EHCIx->ASYNCLISTADDR = (uintptr_t) asyncbuff;
 	// Устанавливаем сегмент в 0
 	//hc->opRegs->ctrlDsSegment = 0;
     EHCIx->CTRLDSSEGMENT = 0x00000000;
 	// Clear status
 	// Чистим статус
 	//hc->opRegs->usbSts = ~0;
     EHCIx->USBSTS = ~ 0uL;
     ASSERT(& EHCIx->USBSTS == & hehci->ehci.opRegs->usbSts);
 	// Enable controller
 	// Запускаем контроллер, 8 микро-фреймов, включаем
 	// последовательную и асинхронную очередь
 	//hc->opRegs->usbCmd = (8 << CMD_ITC_SHIFT) | CMD_PSE | CMD_ASE | CMD_RS;
     EHCIx->USBCMD =
     		(8uL << CMD_ITC_SHIFT) |	// одно прерывание в 8 микро-фреймов (1 мс)
 			((uint_fast32_t) FLS_code << CMD_FLS_SHIFT)	| // Frame list size is 1024 elements
 			//CMD_PSE |	 // Periodic Schedule Enable - PERIODICLISTBASE use
 			//CMD_ASE |	// Asynchronous Schedule Enable - ASYNCLISTADDR use
 			//CMD_RS |	// Run/Stop
 			0;

     EHCIx->USBCMD |= CMD_RS;
 	(void) EHCIx->USBCMD;

  	while ((EHCIx->USBSTS & STS_HCHALTED) != 0)
 		;

 	// Configure all devices to be managed by the EHCI
 	// Говорим, что завершили
 	//hc->opRegs->configFlag = 1;
 	//WOR(configFlagO, 1);
	//
	//PRINTF("board_ehci_initialize: USBCMD=%08lX\n", (unsigned long) EHCIx->USBCMD);
	//PRINTF("board_ehci_initialize: USBSTS=%08lX\n", (unsigned long) EHCIx->USBSTS);
	//PRINTF("board_ehci_initialize: USBINTR=%08lX\n", (unsigned long) EHCIx->USBINTR);
	//PRINTF("board_ehci_initialize: CTRLDSSEGMENT=%08lX\n", (unsigned long) EHCIx->CTRLDSSEGMENT);
	//PRINTF("board_ehci_initialize: PERIODICLISTBASE=%08lX\n", (unsigned long) EHCIx->PERIODICLISTBASE);
	//PRINTF("board_ehci_initialize: ASYNCLISTADDR=%08lX\n", (unsigned long) EHCIx->ASYNCLISTADDR);
	////PRINTF("board_ehci_initialize: asyncbuff=%08lX\n", (unsigned long) & asyncbuff);
	//PRINTF("board_ehci_initialize: FRINDEX=%08lX\n", (unsigned long) EHCIx->FRINDEX);
	////local_delay_ms(10);
	//PRINTF("board_ehci_initialize: FRINDEX=%08lX\n", (unsigned long) EHCIx->FRINDEX);
	////local_delay_ms(20);
	//PRINTF("board_ehci_initialize: FRINDEX=%08lX\n", (unsigned long) EHCIx->FRINDEX);
	////local_delay_ms(30);
	PRINTF("fl=%08lX %08lX\n", hehci->ehci.frameList, EHCIx->PERIODICLISTBASE);

 //	USBH_EHCI_IRQn
 	//USBH_OHCI_IRQn                   = 106,    /*!< USB OHCI global interrupt                                            */
 	//USBH_EHCI_IRQn                   = 107,    /*!< USB EHCI global interrupt                                            */


 	EHCIx->USBINTR |=
 			(1uL << 5) |	// Interrupt on ASync Advance Enable
 			(1uL << 4) |	// Host System Error Interrupt Enable
 			//(1uL << 3) |	// Frame List Rollower Interrupt Enable
 			(1uL << 2) |	// Port Change Interrupt Enable
 			(1uL << 1) |	// USB Error Interrupt Enable
 			(1uL << 0) |	// USB Interrupt Enable
			0;

 	PRINTF("board_ehci_initialize done.\n");
 }


/**
  * @brief  Returns the USB status depending on the HAL status:
  * @param  hal_status: HAL status
  * @retval USB status
  */
USBH_StatusTypeDef USBH_Get_USB_Status(HAL_StatusTypeDef hal_status)
{
  USBH_StatusTypeDef usb_status = USBH_OK;

  switch (hal_status)
  {
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
		uint16_t length, uint8_t do_ping) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBH_StatusTypeDef usb_status = USBH_OK;

//   hal_status = HAL_EHCI_HC_SubmitRequest(phost->pData, pipe, direction ,
//                                         ep_type, token, pbuff, length,
//                                         do_ping);
	PRINTF("USBH_LL_SubmitURB:\n");
	printhex(0, pbuff, length);
	hal_status = HAL_ERROR;

	usb_status =  USBH_Get_USB_Status(hal_status);

	return usb_status;
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
	return URB_ERROR;
	//return (USBH_URBStateTypeDef)HAL_EHCI_HC_GetURBState (phost->pData, pipe);
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

	if (pHandle->hc[pipe].ep_is_in) {
		pHandle->hc[pipe].toggle_in = toggle;
	} else {
		pHandle->hc[pipe].toggle_out = toggle;
	}

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
	EHCI_HandleTypeDef *pHandle;
	pHandle = phost->pData;

	if (pHandle->hc[pipe].ep_is_in) {
		toggle = pHandle->hc[pipe].toggle_in;
	} else {
		toggle = pHandle->hc[pipe].toggle_out;
	}
	return toggle;
}

/**
  * @brief  Return the USB host speed from the low level driver.
  * @param  phost: Host handle
  * @retval USBH speeds
  */
USBH_SpeedTypeDef USBH_LL_GetSpeed(USBH_HandleTypeDef *phost)
{
  USBH_SpeedTypeDef speed = USBH_SPEED_FULL;
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
  return  speed;
}

/**
  * @brief  Reset the Host port of the low level driver.
  * @param  phost: Host handle
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_ResetPort(USBH_HandleTypeDef *phost)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBH_StatusTypeDef usb_status = USBH_OK;
//
//  hal_status = HAL_EHCI_ResetPort(phost->pData);
//
  usb_status = USBH_Get_USB_Status(hal_status);

  return usb_status;
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
  return 0; //HAL_EHCI_HC_GetXferCount(phost->pData, pipe);
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
//
//  hal_status = HAL_EHCI_HC_Init(phost->pData, pipe_num, epnum,
//                               dev_address, speed, ep_type, mps);
//
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
//
//  hal_status = HAL_EHCI_HC_Halt(phost->pData, pipe);
//
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
  HAL_Delay(Delay);
}

/**
  * @brief  Initialize the low level portion of the host driver.
  * @param  phost: Host handle
  * @retval USBH status
  */
USBH_StatusTypeDef USBH_LL_Init(USBH_HandleTypeDef *phost)
{

	hhcd_USB_EHCI.pData = phost;
	phost->pData = & hhcd_USB_EHCI;

	hhcd_USB_EHCI.Instance = WITHUSBHW_EHCI;

#if 1//defined (WITHUSBHW_EHCI)
	hhcd_USB_EHCI.Init.Host_channels = 16;
	hhcd_USB_EHCI.Init.speed = PCD_SPEED_FULL; //PCD_SPEED_HIGH; При high не происходит SACK
	hhcd_USB_EHCI.Init.dma_enable = DISABLE;
	hhcd_USB_EHCI.Init.phy_itface = USB_OTG_EMBEDDED_PHY;

#elif 0//CPUSTYLE_R7S721
	hhcd_USB_EHCI.Init.Host_channels = 16;
	hhcd_USB_EHCI.Init.speed = PCD_SPEED_FULL; //PCD_SPEED_HIGH; При high не происходит SACK
	hhcd_USB_EHCI.Init.dma_enable = DISABLE;
	hhcd_USB_EHCI.Init.phy_itface = USB_OTG_EMBEDDED_PHY;

#elif 0//CPUSTYLE_STM32MP1
	hhcd_USB_EHCI.Init.Host_channels = 16;
	hhcd_USB_EHCI.Init.speed = PCD_SPEED_HIGH;
	#if WITHUSBHOST_DMAENABLE
		hhcd_USB_EHCI.Init.dma_enable = ENABLE;	 // xyz HOST
	#else /* WITHUSBHOST_DMAENABLE */
		hhcd_USB_EHCI.Init.dma_enable = DISABLE;	 // xyz HOST
	#endif /* WITHUSBHOST_DMAENABLE */
	hhcd_USB_EHCI.Init.phy_itface = EHCI_PHY_EMBEDDED;
	hhcd_USB_EHCI.Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
	#if WITHUSBHOST_HIGHSPEEDULPI
		hhcd_USB_EHCI.Init.phy_itface = USB_OTG_ULPI_PHY;
	#elif WITHUSBHOST_HIGHSPEEDPHYC
		hhcd_USB_EHCI.Init.phy_itface = USB_OTG_HS_EMBEDDED_PHY;
	#else /* WITHUSBHOST_HIGHSPEEDULPI */
		hhcd_USB_EHCI.Init.phy_itface = USB_OTG_EMBEDDED_PHY;
	#endif /* WITHUSBHOST_HIGHSPEEDULPI */

#else /* CPUSTYLE_R7S721 */
//	hhcd_USB_EHCI.Init.Host_channels = 16;
//	hhcd_USB_EHCI.Init.pcd_speed = PCD_SPEED_FULL;
//	#if WITHUSBHOST_DMAENABLE
//		hhcd_USB_EHCI.Init.dma_enable = USB_ENABLE;	 // xyz HOST
//	#else /* WITHUSBHOST_DMAENABLE */
//		hhcd_USB_EHCI.Init.dma_enable = USB_DISABLE;	 // xyz HOST
//	#endif /* WITHUSBHOST_DMAENABLE */
//	hhcd_USB_EHCI.Init.phy_itface = EHCI_PHY_EMBEDDED;

#endif /* CPUSTYLE_R7S721 */

	hhcd_USB_EHCI.Init.Sof_enable = DISABLE;

	if (HAL_EHCI_Init(& hhcd_USB_EHCI) != HAL_OK)
	{
		ASSERT(0);
	}

	USBH_LL_SetTimer(phost, HAL_EHCI_GetCurrentFrame(& hhcd_USB_EHCI));
	return USBH_OK;

	/* Init USB_IP */

  /* Link the driver to the stack. */
  hhcd_USB_EHCI.pData = phost;
  phost->pData = &hhcd_USB_EHCI;

	hhcd_USB_EHCI.Instance = WITHUSBHW_EHCI;

  hhcd_USB_EHCI.Init.Host_channels = 12;
  hhcd_USB_EHCI.Init.speed = EHCI_SPEED_FULL;
	#if WITHUSBHOST_DMAENABLE
	hhcd_USB_EHCI.Init.dma_enable = ENABLE;	 // xyz HOST
	#else /* WITHUSBHOST_DMAENABLE */
	hhcd_USB_EHCI.Init.dma_enable = DISABLE;	 // xyz HOST
	#endif /* WITHUSBHOST_DMAENABLE */
  hhcd_USB_EHCI.Init.phy_itface = USB_OTG_EMBEDDED_PHY;
  hhcd_USB_EHCI.Init.Sof_enable = DISABLE;
  hhcd_USB_EHCI.Init.low_power_enable = DISABLE;
  hhcd_USB_EHCI.Init.vbus_sensing_enable = DISABLE;
  hhcd_USB_EHCI.Init.use_external_vbus = DISABLE;
  if (HAL_EHCI_Init(&hhcd_USB_EHCI) != HAL_OK)
  {
    Error_Handler( );
  }

  USBH_LL_SetTimer(phost, HAL_EHCI_GetCurrentFrame(&hhcd_USB_EHCI));

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
//
//  hal_status = HAL_EHCI_Stop(phost->pData);
//
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
	//ticker_initialize(& usbticker, 1, board_usb_tspool, NULL);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.

}

void MX_USB_HOST_DeInit(void)
{

}

#endif /* defined (WITHUSBHW_EHCI) */

#endif /* WITHUSBHW && WITHEHCIHW */


