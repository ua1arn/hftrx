/**
  ******************************************************************************
  * @file    stm32h7xx_hal_hcd.h
  * @author  MCD Application Team
  * @brief   Header file of HCD HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef xxxxxx_HAL_EHCI_H
#define xxxxxx_HAL_EHCI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mslist.h"


/** Minimum alignment required for data structures
 *
 * With the exception of the periodic frame list (which is
 * page-aligned), data structures used by EHCI generally require
 * 32-byte alignment and must not cross a 4kB page boundary.  We
 * simplify this requirement by aligning each structure on its own
 * size, with a minimum of a 32 byte alignment.
 */
#define EHCI_MIN_ALIGN 32

/** Maximum transfer size
 *
 * EHCI allows for transfers of up to 20kB with page-alignment, or
 * 16kB with arbitrary alignment.
 */
#define EHCI_MTU 16384

/** Page-alignment required for some data structures */
#define EHCI_PAGE_ALIGN 4096

/** EHCI PCI BAR */
#define EHCI_BAR PCI_BASE_ADDRESS_0

/** Capability register length */
#define EHCI_CAP_CAPLENGTH 0x00

/** Host controller interface version number */
#define EHCI_CAP_HCIVERSION 0x02

/** Structural parameters */
#define EHCI_CAP_HCSPARAMS 0x04

/** Number of ports */
#define EHCI_HCSPARAMS_PORTS(params) ( ( (params) >> 0 ) & 0x0f )

/** Capability parameters */
#define EHCI_CAP_HCCPARAMS 0x08

/** 64-bit addressing capability */
#define EHCI_HCCPARAMS_ADDR64(params) ( ( (params) >> 0 ) & 0x1 )

/** Programmable frame list flag */
#define EHCI_HCCPARAMS_FLSIZE(params) ( ( (params) >> 1 ) & 0x1 )

/** EHCI extended capabilities pointer */
#define EHCI_HCCPARAMS_EECP(params) ( ( ( (params) >> 8 ) & 0xff ) )

/** EHCI extended capability ID */
#define EHCI_EECP_ID(eecp) ( ( (eecp) >> 0 ) & 0xff )

/** Next EHCI extended capability pointer */
#define EHCI_EECP_NEXT(eecp) ( ( ( (eecp) >> 8 ) & 0xff ) )

/** USB legacy support extended capability */
#define EHCI_EECP_ID_LEGACY 1

// /** USB legacy support BIOS owned semaphore */
// #define EHCI_USBLEGSUP_BIOS 0x02
//
// /** USB legacy support BIOS ownership flag */
// #define EHCI_USBLEGSUP_BIOS_OWNED 0x01
//
// /** USB legacy support OS owned semaphore */
// #define EHCI_USBLEGSUP_OS 0x03
//
// /** USB legacy support OS ownership flag */
// #define EHCI_USBLEGSUP_OS_OWNED 0x01
//
// /** USB legacy support control/status */
// #define EHCI_USBLEGSUP_CTLSTS 0x04
//
/** USB command register */
#define EHCI_OP_USBCMD 0x00

/** Run/stop */
#define EHCI_USBCMD_RUN 0x00000001UL

/** Host controller reset */
#define EHCI_USBCMD_HCRST 0x00000002UL

/** Frame list size */
#define EHCI_USBCMD_FLSIZE(flsize) ( (flsize) << 2 )

/** Frame list size mask */
#define EHCI_USBCMD_FLSIZE_MASK EHCI_USBCMD_FLSIZE ( 3 )

/** Default frame list size */
#define EHCI_FLSIZE_DEFAULT 0

/** Smallest allowed frame list size */
#define EHCI_FLSIZE_SMALL 2

/** Number of elements in frame list */
#define EHCI_PERIODIC_FRAMES(flsize) ( 1024 >> (flsize) )

/** Periodic schedule enable */
#define EHCI_USBCMD_PERIODIC 0x00000010UL

/** Asynchronous schedule enable */
#define EHCI_USBCMD_ASYNC 0x00000020UL

/** Asyncchronous schedule advance doorbell */
#define EHCI_USBCMD_ASYNC_ADVANCE 0x000040UL

/** USB status register */
#define EHCI_OP_USBSTS 0x04

/** USB interrupt */
#define EHCI_USBSTS_USBINT 0x00000001UL

/** USB error interrupt */
#define EHCI_USBSTS_USBERRINT 0x00000002UL

/** Port change detect */
#define EHCI_USBSTS_PORT 0x00000004UL

/** Frame list rollover */
#define EHCI_USBSTS_ROLLOVER 0x00000008UL

/** Host system error */
#define EHCI_USBSTS_SYSERR 0x00000010UL

/** Asynchronous schedule advanced */
#define EHCI_USBSTS_ASYNC_ADVANCE 0x00000020UL

/** Periodic schedule enabled */
#define EHCI_USBSTS_PERIODIC 0x00004000UL

/** Asynchronous schedule enabled */
#define EHCI_USBSTS_ASYNC 0x00008000UL

/** Host controller halted */
#define EHCI_USBSTS_HCH 0x00001000UL //STS_HCHALTED

/** USB status change mask */
#define EHCI_USBSTS_CHANGE                                              \
        ( EHCI_USBSTS_USBINT | EHCI_USBSTS_USBERRINT |                  \
          EHCI_USBSTS_PORT | EHCI_USBSTS_ROLLOVER |                     \
          EHCI_USBSTS_SYSERR | EHCI_USBSTS_ASYNC_ADVANCE )

/** USB interrupt enable register */
#define EHCI_OP_USBINTR 0x08

/** Frame index register */
#define EHCI_OP_FRINDEX 0x0c

/** Control data structure segment register */
#define EHCI_OP_CTRLDSSEGMENT 0x10

/** Periodic frame list base address register */
#define EHCI_OP_PERIODICLISTBASE 0x14

/** Current asynchronous list address register */
#define EHCI_OP_ASYNCLISTADDR 0x18

/** Configure flag register */
#define EHCI_OP_CONFIGFLAG 0x40

/** Configure flag */
#define EHCI_CONFIGFLAG_CF 0x00000001UL

/** Port status and control register */
#define EHCI_OP_PORTSC(port) ( 0x40 + ( (port) << 2 ) )

/** Current connect status */
#define EHCI_PORTSC_CCS 0x00000001UL

/** Connect status change */
#define EHCI_PORTSC_CSC 0x00000002UL

/** Port enabled */
#define EHCI_PORTSC_PED 0x00000004UL

/** Port enabled/disabled change */
#define EHCI_PORTSC_PEC 0x00000008UL

/** Over-current change */
#define EHCI_PORTSC_OCC 0x00000020UL

/** Port reset */
#define EHCI_PORTSC_PR 0x00000100UL

/** Line status */
#define EHCI_PORTSC_LINE_STATUS(portsc) ( ( (portsc) >> 10 ) & 0x3 )

/** Line status: low-speed device */
#define EHCI_PORTSC_LINE_STATUS_LOW 0x1

/** Port power */
#define EHCI_PORTSC_PP 0x00001000UL

/** Port owner */
#define EHCI_PORTSC_OWNER 0x00002000UL

/** Port status change mask */
#define EHCI_PORTSC_CHANGE \
        ( EHCI_PORTSC_CSC | EHCI_PORTSC_PEC | EHCI_PORTSC_OCC )

/** List terminator */
#define EHCI_LINK_TERMINATE 0x00000001UL

/** Frame list type */
#define EHCI_LINK_TYPE(type) ( (type) << 1 )

/** Queue head type */
#define EHCI_LINK_TYPE_QH EHCI_LINK_TYPE ( 1 )

/** A periodic frame list entry */
struct ehci_periodic_frame {
        /** First queue head */
        uint32_t link;
} ATTRPACKED;

/** A transfer descriptor */
struct ehci_transfer_descriptor {
        /** Next transfer descriptor */
        uint32_t next;
        /** Alternate next transfer descriptor */
        uint32_t alt;
        /** Status */
        uint8_t status;
        /** Flags */
        uint8_t flags;
        /** Transfer length */
        uint16_t len;
        /** Buffer pointers (low 32 bits) */
        uint32_t low[5];
        /** Extended buffer pointers (high 32 bits) */
        uint32_t high[5];
        /** Reserved */
        uint8_t reserved[12];
} ATTRPACKED;

/** Transaction error */
#define EHCI_STATUS_XACT_ERR 0x08

/** Babble detected */
#define EHCI_STATUS_BABBLE 0x10

/** Data buffer error */
#define EHCI_STATUS_BUFFER 0x20

/** Halted */
#define EHCI_STATUS_HALTED 0x40

/** Active */
#define EHCI_STATUS_ACTIVE 0x80

/** Ping */
#define EHCI_STATUS_PING 0x01

#define EHCI_STATUS_MASK 0xF8

/** PID code */
#define EHCI_FL_PID(code) ( (code) << 0 )

/** OUT token */
#define EHCI_FL_PID_OUT EHCI_FL_PID ( 0 )

/** IN token */
#define EHCI_FL_PID_IN EHCI_FL_PID ( 1 )

/** SETUP token */
#define EHCI_FL_PID_SETUP EHCI_FL_PID ( 2 )

/** Error counter */
#define EHCI_FL_CERR( count ) ( (count) << 2 )

/** Error counter maximum value */
#define EHCI_FL_CERR_MAX EHCI_FL_CERR ( 3 )

/** Interrupt on completion */
#define EHCI_FL_IOC 0x80

/** Length mask */
#define EHCI_LEN_MASK 0x7fff

/** Data toggle */
#define EHCI_LEN_TOGGLE 0x8000

/** A queue head */
struct ehci_queue_head {
        /** Horizontal link pointer */
        uint32_t link;
        /** Endpoint characteristics */
        uint32_t chr;
        /** Endpoint capabilities */
        uint32_t cap;
        /** Current transfer descriptor - current qTD */
        uint32_t current;
        /** Transfer descriptor cache */
        struct ehci_transfer_descriptor cache;
        /** for arrays align */
        uint8_t pad [16];
} ATTRPACKED;

/** Device address */
#define EHCI_CHR_ADDRESS( address ) ( (address) << 0 )

/** Endpoint number */
#define EHCI_CHR_ENDPOINT( address ) ( ( (address) & 0xf ) << 8 )

/** Endpoint speed */
#define EHCI_CHR_EPS( eps ) ( (eps) << 12 )

/** Full-speed endpoint */
#define EHCI_CHR_EPS_FULL EHCI_CHR_EPS ( 0 )

/** Low-speed endpoint */
#define EHCI_CHR_EPS_LOW EHCI_CHR_EPS ( 1 )

/** High-speed endpoint */
#define EHCI_CHR_EPS_HIGH EHCI_CHR_EPS ( 2 )

/** Explicit data toggles */
#define EHCI_CHR_TOGGLE 0x00004000UL

/** Head of reclamation list flag */
#define EHCI_CHR_HEAD 0x00008000UL

/** Maximum packet length */
#define EHCI_CHR_MAX_LEN( len ) ( (len) << 16 )

/** Control endpoint flag */
#define EHCI_CHR_CONTROL 0x08000000UL

/** Interrupt schedule mask */
#define EHCI_CAP_INTR_SCHED( uframe ) ( 1 << ( (uframe) + 0 ) )

/** Split completion schedule mask */
#define EHCI_CAP_SPLIT_SCHED( uframe ) ( 1 << ( (uframe) + 8 ) )

/** Default split completion schedule mask
 *
 * We schedule all split starts in microframe 0, on the assumption
 * that we will never have to deal with more than sixteen actively
 * interrupting devices via the same transaction translator.  We
 * schedule split completions for all remaining microframes after
 * microframe 1 (in which the low-speed or full-speed transaction is
 * assumed to execute).  This is a very crude approximation designed
 * to avoid the need for calculating exactly when low-speed and
 * full-speed transactions will execute.  Since we only ever deal with
 * interrupt endpoints (rather than isochronous endpoints), the volume
 * of periodic traffic is extremely low, and this approximation should
 * remain valid.
 */
#define EHCI_CAP_SPLIT_SCHED_DEFAULT                                    \
        ( EHCI_CAP_SPLIT_SCHED ( 2 ) | EHCI_CAP_SPLIT_SCHED ( 3 ) |     \
          EHCI_CAP_SPLIT_SCHED ( 4 ) | EHCI_CAP_SPLIT_SCHED ( 5 ) |     \
          EHCI_CAP_SPLIT_SCHED ( 6 ) | EHCI_CAP_SPLIT_SCHED ( 7 ) )

/** Transaction translator hub address */
#define EHCI_CAP_TT_HUB( address ) ( (address) << 16 )

/** Transaction translator port number */
#define EHCI_CAP_TT_PORT( port ) ( (port) << 23 )

/** High-bandwidth pipe multiplier */
#define EHCI_CAP_MULT( mult ) ( (mult) << 30 )

/** Number of transfer descriptors in a ring
 *
 * This is a policy decision.
 */
#define EHCI_RING_COUNT 64

/** Time to delay after enabling power to a port
 *
 * This is not mandated by EHCI; we use the value given for xHCI.
 */
#define EHCI_PORT_POWER_DELAY_MS 20

/** Time to delay after releasing ownership of a port
 *
 * This is a policy decision.
 */
#define EHCI_DISOWN_DELAY_MS 100

/** Maximum time to wait for BIOS to release ownership
 *
 * This is a policy decision.
 */
#define EHCI_USBLEGSUP_MAX_WAIT_MS 100

/** Maximum time to wait for asynchronous schedule to advance
 *
 * This is a policy decision.
 */
#define EHCI_ASYNC_ADVANCE_MAX_WAIT_MS 100

/** Maximum time to wait for host controller to stop
 *
 * This is a policy decision.
 */
#define EHCI_STOP_MAX_WAIT_MS 100

/** Maximum time to wait for reset to complete
 *
 * This is a policy decision.
 */
#define EHCI_RESET_MAX_WAIT_MS 500

/** Maximum time to wait for a port reset to complete
 *
 * This is a policy decision.
 */
#define EHCI_PORT_RESET_MAX_WAIT_MS 500

/** Set initial data toggle */
#define EHCI_FL_TOGGLE 0x8000

enum { FLS = EHCI_PERIODIC_FRAMES(EHCI_FLSIZE_DEFAULT) };

// ------------------------------------------------------------------------------------------------
// Limits

#define MAX_QH                          8
#define MAX_TD                          32

// ------------------------------------------------------------------------------------------------
// Host Controller Capability Registers

typedef struct EhciCapRegs
{
	volatile uint8_t capLength;
	volatile uint8_t reserved;
	volatile uint16_t hciVersion;
	volatile uint32_t hcsParams;
	volatile uint32_t hccParams;
	volatile uint64_t hcspPortRoute;
} ATTRPACKED EhciCapRegs;
// ------------------------------------------------------------------------------------------------
// Host Controller Structural Parameters Register

#define HCSPARAMS_N_PORTS_MASK          (15uL << 0)   // Number of Ports
#define HCSPARAMS_PPC                   (1uL << 4)    // Port Power Control
#define HCSPARAMS_PORT_ROUTE            (1uL << 7)    // Port Routing Rules
#define HCSPARAMS_N_PCC_MASK            (15uL << 8)   // Number of Ports per Companion Controller
#define HCSPARAMS_N_PCC_SHIFT           8
#define HCSPARAMS_N_CC_MASK             (15uL << 12)  // Number of Companion Controllers
#define HCSPARAMS_N_CC_SHIFT            12
#define HCSPARAMS_P_INDICATOR           (1uL << 16)   // Port Indicator
#define HCSPARAMS_DPN_MASK              (15uL << 20)  // Debug Port Number
#define HCSPARAMS_DPN_SHIFT             20

// ------------------------------------------------------------------------------------------------
// Host Controller Capability Parameters Register

#define HCCPARAMS_64_BIT                (1uL << 0)    // 64-bit Addressing Capability
#define HCCPARAMS_PFLF                  (1uL << 1)    // Programmable Frame List Flag
#define HCCPARAMS_ASPC                  (1uL << 2)    // Asynchronous Schedule Park Capability
#define HCCPARAMS_IST_MASK              (15uL << 4)   // Isochronous Sheduling Threshold
#define HCCPARAMS_EECP_MASK             (255uL << 8)  // EHCI Extended Capabilities Pointer
#define HCCPARAMS_EECP_SHIFT            8

// ------------------------------------------------------------------------------------------------
// Host Controller Operational Registers

//typedef struct EhciOpRegs
//{
//    volatile uint32_t usbCmd;
//    volatile uint32_t usbSts;
//    volatile uint32_t usbIntr;
//    volatile uint32_t frameIndex;
//    volatile uint32_t ctrlDsSegment;
//    volatile uint32_t periodicListBase;
//    volatile uint32_t asyncListAddr;
//    volatile uint32_t reserved[9];
//    volatile uint32_t configFlag;
//    volatile uint32_t ports[];
//} EhciOpRegs;

// ------------------------------------------------------------------------------------------------
// USB Command Register

#define CMD_RS                          (1uL << 0)    // Run/Stop 1=Run, 0-stop
#define CMD_HCRESET                     (1uL << 1)    // Host Controller Reset
#define CMD_FLS_MASK                    (3uL << 2)    // Frame List Size
#define CMD_FLS_SHIFT                   2
#define CMD_PSE                         (1uL << 4)    // Periodic Schedule Enable
#define CMD_ASE                         (1uL << 5)    // Asynchronous Schedule Enable
#define CMD_IOAAD                       (1uL << 6)    // Interrupt on Async Advance Doorbell
#define CMD_LHCR                        (1uL << 7)    // Light Host Controller Reset
#define CMD_ASPMC_MASK                  (3uL << 8)    // Asynchronous Schedule Park Mode Count
#define CMD_ASPMC_SHIFT                 8
#define CMD_ASPME                       (1uL << 11)   // Asynchronous Schedule Park Mode Enable
#define CMD_ITC_MASK                    (255uL << 16) // Interrupt Threshold Control
#define CMD_ITC_SHIFT                   16

// ------------------------------------------------------------------------------------------------
// USB Status Register

#define STS_USBINT                      (1uL << 0)    // USB Interrupt
#define STS_ERROR                       (1uL << 1)    // USB Error Interrupt
#define STS_PCD                         (1uL << 2)    // Port Change Detect
#define STS_FLR                         (1uL << 3)    // Frame List Rollover
#define STS_HSE                         (1uL << 4)    // Host System Error
#define STS_IOAA                        (1uL << 5)    // Interrupt on Async Advance
//#define STS_HCHALTED                    (1uL << 12)   // Host Controller Halted EHCI_USBSTS_HCH
#define STS_RECLAMATION                 (1uL << 13)   // Reclamation
#define STS_PSS                         (1uL << 14)   // Periodic Schedule Status
#define STS_ASS                         (1uL << 15)   // Asynchronous Schedule Status

// ------------------------------------------------------------------------------------------------
// USB Interrupt Enable Register

#define INTR_USBINT                     (1uL << 0)    // USB Interrupt Enable
#define INTR_ERROR                      (1uL << 1)    // USB Error Interrupt Enable
#define INTR_PCD                        (1uL << 2)    // Port Change Interrupt Enable
#define INTR_FLR                        (1uL << 3)    // Frame List Rollover Enable
#define INTR_HSE                        (1uL << 4)    // Host System Error Enable
#define INTR_IOAA                       (1uL << 5)    // Interrupt on Async Advance Enable

// ------------------------------------------------------------------------------------------------
// Frame Index Register

#define FR_MASK                         0x3fff

// ------------------------------------------------------------------------------------------------
// Configure Flag Register

#define CF_PORT_ROUTE                   (1uL << 0)    // Configure Flag (CF)

// ------------------------------------------------------------------------------------------------
// Port Status and Control Registers

#define PORTSC_CONNECTION                 (1uL << 0)    // Current Connect Status
#define PORTSC_CONNECTION_CHANGE          (1uL << 1)    // Connect Status Change
#define PORTSC_ENABLE                     (1uL << 2)    // Port Enabled
#define PORTSC_ENABLE_CHANGE              (1uL << 3)    // Port Enable Change
#define PORTSC_OVER_CURRENT               (1uL << 4)    // Over-current Active
#define PORTSC_OVER_CURRENT_CHANGE        (1uL << 5)    // Over-current Change
#define PORTSC_FPR                        (1uL << 6)    // Force Port Resume
#define PORTSC_SUSPEND                    (1uL << 7)    // Suspend
#define PORTSC_RESET                      (1uL << 8)    // Port Reset
#define PORTSC_LS_MASK                    (3uL << 10)   // Line Status
#define PORTSC_LS_SHIFT                   10
#define PORTSC_POWER                      (1uL << 12)   // Port Power
#define PORTSC_OWNER                      (1uL << 13)   // Port Owner
#define PORTSC_IC_MASK                    (3uL << 14)   // Port Indicator Control
#define PORTSC_IC_SHIFT                   14
#define PORTSC_TC_MASK                    (15uL << 16)  // Port Test Control
#define PORTSC_TC_SHIFT                   16
#define PORTSC_WKCNNT_E                   (1uL << 20)   // Wake on Connect Enable
#define PORTSC_WKDSCNNT_E                 (1uL << 21)   // Wake on Disconnect Enable
#define PORTSC_WKOC_E                     (1uL << 22)   // Wake on Over-current Enable
#define PORTSC_RWC                        (PORTSC_CONNECTION_CHANGE | PORTSC_ENABLE_CHANGE | PORTSC_OVER_CURRENT_CHANGE)


struct ohci_registers
{
	volatile uint32_t HcRevision;			/* 0x000 */
	volatile uint32_t HcControl;			/* 0x004 */
	volatile uint32_t HcCommandStatus;		/* 0x008 */
	volatile uint32_t HcInterruptStatus;	/* 0x00C */
	volatile uint32_t HcInterruptEnable;	/* 0x010 */
	volatile uint32_t HcInterruptDisable;	/* 0x014 */
	volatile uint32_t HcHCCA;				/* 0x018 */
	volatile uint32_t HcPeriodCurrentED;	/* 0x01C */
	volatile uint32_t HcControlHeadED;		/* 0x020 */
	volatile uint32_t HcControlCurrentED;	/* 0x024 */
	volatile uint32_t HcBulkHeadED;			/* 0x028 */
	volatile uint32_t HcBulkCurrentED;		/* 0x02C */
	volatile uint32_t HcDoneHead;			/* 0x030 */
	volatile uint32_t HcFmInterval;			/* 0x034 */
	volatile uint32_t HcFmRemaining;		/* 0x038 */
	volatile uint32_t HcFmNumber;			/* 0x03C */
	volatile uint32_t HcPeriodicStart;		/* 0x040 */
	volatile uint32_t HcLSThreshold;		/* 0x044 */
	volatile uint32_t HcRhDescriptorA;		/* 0x048 */
	volatile uint32_t HcRhDescriptorB;		/* 0x04C */
	volatile uint32_t HcRhStatus;			/* 0x050 */
	volatile uint32_t HcRhPortStatus [1];	/* 0x054 */
} ATTRPACKED;

// ------------------------------------------------------------------------------------------------
// Transfer Descriptor
//
//typedef struct EhciTD
//{
//    volatile uint32_t link;
//    volatile uint32_t altLink;
//    volatile uint32_t token;
//    volatile uint32_t buffer[5];
//    volatile uint32_t extBuffer[5];
//
//    // internal fields
//    uint32_t tdNext;
//    uint32_t active;
//    uint8_t pad[4];
//} EhciTD;

// TD Link Pointer
#define PTR_TERMINATE                   (1uL << 0)

#define PTR_TYPE_MASK                   (3uL << 1)
#define PTR_ITD                         (0uL << 1)
#define PTR_QH                          (1uL << 1)
#define PTR_SITD                        (2uL << 1)
#define PTR_FSTN                        (3uL << 1)

// TD Token
#define TD_TOK_PING                     (1uL << 0)    // Ping State
#define TD_TOK_STS                      (1uL << 1)    // Split Transaction State
#define TD_TOK_MMF                      (1uL << 2)    // Missed Micro-Frame
#define TD_TOK_XACT                     (1uL << 3)    // Transaction Error
#define TD_TOK_BABBLE                   (1uL << 4)    // Babble Detected
#define TD_TOK_DATABUFFER               (1uL << 5)    // Data Buffer Error
#define TD_TOK_HALTED                   (1uL << 6)    // Halted
#define TD_TOK_ACTIVE                   (1uL << 7)    // Active
#define TD_TOK_PID_MASK                 (3uL << 8)    // PID Code
#define TD_TOK_PID_SHIFT                8
#define TD_TOK_CERR_MASK                (3uL << 10)   // Error Counter
#define TD_TOK_CERR_SHIFT               10
#define TD_TOK_C_PAGE_MASK              (7uL << 12)   // Current Page
#define TD_TOK_C_PAGE_SHIFT             12
#define TD_TOK_IOC                      (1uL << 15)   // Interrupt on Complete
#define TD_TOK_LEN_MASK                 0x7fff0000uL  // Total Bytes to Transfer
#define TD_TOK_LEN_SHIFT                16
#define TD_TOK_D                        (1uL << 31)   // Data Toggle
#define TD_TOK_D_SHIFT                  31

#define USB_PACKET_OUT                  0           // token 0xe1
#define USB_PACKET_IN                   1           // token 0x69
#define USB_PACKET_SETUP                2           // token 0x2d

// ------------------------------------------------------------------------------------------------
// Queue Head
//
//typedef struct EhciQH
//{
//    uint32_t qhlp;       // Queue Head Horizontal Link Pointer
//    uint32_t ch;         // Endpoint Characteristics
//    uint32_t caps;       // Endpoint Capabilities
//    volatile uint32_t curLink;
//
//    // matches a transfer descriptor
//    volatile uint32_t nextLink;
//    volatile uint32_t altLink;
//    volatile uint32_t token;
//    volatile uint32_t buffer[5];
//    volatile uint32_t extBuffer[5];
//
//    // internal fields
////    UsbTransfer *transfer;
////    Link qhLink;
//    uint32_t tdHead;
//    uint32_t active;
//    uint8_t pad[20];
//} EhciQH;

//// Endpoint Characteristics
//#define QH_CH_DEVADDR_MASK              0x0000007fuL  // Device Address
//#define QH_CH_INACTIVE                  0x00000080uL  // Inactive on Next Transaction
//#define QH_CH_ENDP_MASK                 0x00000f00uL  // Endpoint Number
//#define QH_CH_ENDP_SHIFT                8
//#define QH_CH_EPS_MASK                  0x00003000uL  // Endpoint Speed
//#define QH_CH_EPS_SHIFT                 12
//#define QH_CH_DTC                       0x00004000uL  // Data Toggle Control
//#define QH_CH_H                         0x00008000uL  // Head of Reclamation List Flag
//#define QH_CH_MPL_MASK                  0x07ff0000uL  // Maximum Packet Length
//#define QH_CH_MPL_SHIFT                 16
//#define QH_CH_CONTROL                   0x08000000uL  // Control Endpoint Flag
//#define QH_CH_NAK_RL_MASK               0xf0000000uL  // Nak Count Reload
//#define QH_CH_NAK_RL_SHIFT              28
//
//// Endpoint Capabilities
//#define QH_CAP_INT_SCHED_SHIFT          0
//#define QH_CAP_INT_SCHED_MASK           0x000000ffuL  // Interrupt Schedule Mask
//#define QH_CAP_SPLIT_C_SHIFT            8
//#define QH_CAP_SPLIT_C_MASK             0x0000ff00uL  // Split Completion Mask
//#define QH_CAP_HUB_ADDR_SHIFT           16
//#define QH_CAP_HUB_ADDR_MASK            0x007f0000uL  // Hub Address
//#define QH_CAP_PORT_MASK                0x3f800000uL  // Port Number
//#define QH_CAP_PORT_SHIFT               23
//#define QH_CAP_MULT_MASK                0xc0000000uL  // High-Bandwidth Pipe Multiplier
//#define QH_CAP_MULT_SHIFT               30

/** @addtogroup xxxxxx_HAL_Driver
  * @{
  */

/** @addtogroup HCD HCD
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup EHCI_Exported_Types HCD Exported Types
  * @{
  */

/** @defgroup EHCI_Exported_Types_Group1 HCD State Structure definition
  * @{
  */
typedef enum
{
  HAL_EHCI_STATE_RESET    = 0x00,
  HAL_EHCI_STATE_READY    = 0x01,
  HAL_EHCI_STATE_ERROR    = 0x02,
  HAL_EHCI_STATE_BUSY     = 0x03,
  HAL_EHCI_STATE_TIMEOUT  = 0x04
} EHCI_StateTypeDef;


typedef struct
{
  uint8_t   dev_addr;           /*!< USB device address.
                                     This parameter must be a number between Min_Data = 1 and Max_Data = 255    */

  uint8_t   ch_num;             /*!< Host channel number.
                                     This parameter must be a number between Min_Data = 1 and Max_Data = 15     */

  uint8_t   ep_num;             /*!< Endpoint number.
                                     This parameter must be a number between Min_Data = 1 and Max_Data = 15     */

  uint8_t   ep_is_in;           /*!< Endpoint direction
                                     This parameter must be a number between Min_Data = 0 and Max_Data = 1      */

  uint8_t   speed;              /*!< USB Host speed.
                                     This parameter can be any value of @ref USB_Core_Speed_                    */

  uint8_t   do_ping;            /*!< Enable or disable the use of the PING protocol for HS mode.                */

  uint8_t   process_ping;       /*!< Execute the PING protocol for HS mode.                                     */

  uint8_t   ep_type;            /*!< Endpoint Type.
                                     This parameter can be any value of @ref USB_EP_Type_                       */

  uint16_t  max_packet;         /*!< Endpoint Max packet size.
                                     This parameter must be a number between Min_Data = 0 and Max_Data = 64KB   */

//  uint8_t   data_pid;           /*!< Initial data PID.
//                                     This parameter must be a number between Min_Data = 0 and Max_Data = 1      */

  uint8_t   * volatile xfer_buff;         /*!< Pointer to transfer buffer.                                                */

  uint32_t  XferSize;             /*!< OTG Channel transfer size.                                                   */

  uint32_t  xfer_len;           /*!< Current transfer length.                                                   */

  uint32_t  volatile xfer_count;         /*!< Partial transfer length in case of multi packet transfer.                  */

//  uint8_t   toggle_in;          /*!< IN transfer current toggle flag.
//                                     This parameter must be a number between Min_Data = 0 and Max_Data = 1      */
//
//  uint8_t   toggle_out;         /*!< OUT transfer current toggle flag
//                                     This parameter must be a number between Min_Data = 0 and Max_Data = 1      */

  uintptr_t  dma_addr;           /*!< 32 bits aligned transfer buffer address.                                   */

  uint32_t  ErrCnt;             /*!< Host channel error count.                                                  */

  volatile USB_OTG_URBStateTypeDef ehci_urb_state;  /*!< URB state.
                                            This parameter can be any value of @ref USB_OTG_URBStateTypeDef */

  USB_OTG_HCStateTypeDef state;       /*!< Host Channel state.
                                            This parameter can be any value of @ref USB_OTG_HCStateTypeDef  */

  uint8_t tt_hubaddr;
  uint8_t tt_prtaddr;

  VLIST_ENTRY tdlist;

} EHCI_HCTypeDef;

typedef USB_EHCI_CapabilityTypeDef   EHCI_TypeDef;	/* CPU I/O registers */
typedef USB_OTG_CfgTypeDef      EHCI_InitTypeDef;
//typedef USB_OTG_HCTypeDef       EHCI_HCTypeDef;
typedef USB_OTG_URBStateTypeDef EHCI_URBStateTypeDef;
typedef USB_OTG_HCStateTypeDef  EHCI_HCStateTypeDef;
/**
  * @}
  */

#define EHCI_COUNT_HC 16
/** @defgroup EHCI_Exported_Types_Group2 HCD Handle Structure definition
  * @{
  */
#if (USE_HAL_EHCI_REGISTER_CALLBACKS == 1U)
typedef struct __EHCI_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_EHCI_REGISTER_CALLBACKS */
{
	// Periodic frame list
	// Periodic Schedule list - PERIODICLISTBASE use
	volatile __attribute__((aligned(4096)))  struct ehci_periodic_frame periodiclist [FLS];

	// Asynchronous Schedule list - ASYNCLISTADDR use
	// list of queue headers
	// выравнивание заменено с 32 на DATA CACHE PAGE
	volatile __attribute__((aligned(DCACHEROWSIZE)))  struct ehci_queue_head asynclisthead [EHCI_COUNT_HC];
	volatile __attribute__((aligned(DCACHEROWSIZE)))  struct ehci_transfer_descriptor qtds [EHCI_COUNT_HC];
	volatile __attribute__((aligned(DCACHEROWSIZE)))  struct ehci_queue_head itdsarray [EHCI_COUNT_HC];

	VLIST_ENTRY hcListAsync;
	VLIST_ENTRY hcListPeriodic;

	LCLSPINLOCK_t asynclock;

	EHCI_TypeDef *Instance; /*!< Register base address    */
	EHCI_InitTypeDef Init; /*!< HCD required parameters  */
	EHCI_HCTypeDef hc [EHCI_COUNT_HC]; /*!< Host channels parameters */
	HAL_LockTypeDef Lock; /*!< HCD peripheral status    */
	__IO EHCI_StateTypeDef State; /*!< HCD communication state  */
	__IO uint32_t ErrorCode; /*!< HCD Error code           */
	void *pData; /*!< Pointer Stack Handler - for USBH_LL_xxx functions   */
	unsigned long nports;
	__IO uint32_t *portsc;
	__IO uint32_t *configFlag;

	struct ohci_registers * ohci;

#if (USE_HAL_EHCI_REGISTER_CALLBACKS == 1U)
	void (* SOFCallback)(struct __EHCI_HandleTypeDef *hhcd);                               /*!< USB OTG HCD SOF callback                */
	void (* ConnectCallback)(struct __EHCI_HandleTypeDef *hhcd);                           /*!< USB OTG HCD Connect callback            */
	void (* DisconnectCallback)(struct __EHCI_HandleTypeDef *hhcd);                        /*!< USB OTG HCD Disconnect callback         */
	void (* PortEnabledCallback)(struct __EHCI_HandleTypeDef *hhcd);                       /*!< USB OTG HCD Port Enable callback        */
	void (* PortDisabledCallback)(struct __EHCI_HandleTypeDef *hhcd);                      /*!< USB OTG HCD Port Disable callback       */
	void (* HC_NotifyURBChangeCallback)(struct __EHCI_HandleTypeDef *hhcd, uint8_t chnum,
									  EHCI_URBStateTypeDef urb_state);                   /*!< USB OTG HCD Host Channel Notify URB Change callback  */

	void (* MspInitCallback)(struct __EHCI_HandleTypeDef *hhcd);                           /*!< USB OTG HCD Msp Init callback           */
	void (* MspDeInitCallback)(struct __EHCI_HandleTypeDef *hhcd);                         /*!< USB OTG HCD Msp DeInit callback         */
#endif /* USE_HAL_EHCI_REGISTER_CALLBACKS */

} EHCI_HandleTypeDef;
/**
  * @}
  */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup EHCI_Exported_Constants HCD Exported Constants
  * @{
  */

/** @defgroup EHCI_Speed HCD Speed
  * @{
  */
#define EHCI_SPEED_HIGH               USBH_HS_SPEED
#define EHCI_SPEED_FULL               USBH_FSLS_SPEED
#define EHCI_SPEED_LOW                USBH_FSLS_SPEED

/**
  * @}
  */

/** @defgroup EHCI_PHY_Module HCD PHY Module
  * @{
  */
#define EHCI_PHY_ULPI                 1U
#define EHCI_PHY_EMBEDDED             2U
/**
  * @}
  */

/** @defgroup EHCI_Error_Code_definition HCD Error Code definition
  * @brief  HCD Error Code definition
  * @{
  */
#if (USE_HAL_EHCI_REGISTER_CALLBACKS == 1U)
#define  HAL_EHCI_ERROR_INVALID_CALLBACK                        (0x00000010U)    /*!< Invalid Callback error  */
#endif /* USE_HAL_EHCI_REGISTER_CALLBACKS */

/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup EHCI_Exported_Macros HCD Exported Macros
  *  @brief macros to handle interrupts and specific clock configurations
  * @{
  */
#define __HAL_EHCI_ENABLE(__HANDLE__)                   do { } while (0) //(void)USB_EnableGlobalInt ((__HANDLE__)->Instance)
#define __HAL_EHCI_DISABLE(__HANDLE__)                  do { } while (0) //(void)USB_DisableGlobalInt ((__HANDLE__)->Instance)

//#define __HAL_EHCI_GET_FLAG(__HANDLE__, __INTERRUPT__)      ((USB_ReadInterrupts((__HANDLE__)->Instance) & (__INTERRUPT__)) == (__INTERRUPT__))
//#define __HAL_EHCI_CLEAR_FLAG(__HANDLE__, __INTERRUPT__)    (((__HANDLE__)->Instance->GINTSTS) = (__INTERRUPT__))
//#define __HAL_EHCI_IS_INVALID_INTERRUPT(__HANDLE__)         (USB_ReadInterrupts((__HANDLE__)->Instance) == 0U)
//
//#define __HAL_EHCI_CLEAR_HC_INT(chnum, __INTERRUPT__)  (USBx_HC(chnum)->HCINT = (__INTERRUPT__))
//#define __HAL_EHCI_MASK_HALT_HC_INT(chnum)             (USBx_HC(chnum)->HCINTMSK &= ~USB_OTG_HCINTMSK_CHHM)
//#define __HAL_EHCI_UNMASK_HALT_HC_INT(chnum)           (USBx_HC(chnum)->HCINTMSK |= USB_OTG_HCINTMSK_CHHM)
//#define __HAL_EHCI_MASK_ACK_HC_INT(chnum)              (USBx_HC(chnum)->HCINTMSK &= ~USB_OTG_HCINTMSK_ACKM)
//#define __HAL_EHCI_UNMASK_ACK_HC_INT(chnum)            (USBx_HC(chnum)->HCINTMSK |= USB_OTG_HCINTMSK_ACKM)
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup EHCI_Exported_Functions HCD Exported Functions
  * @{
  */

/** @defgroup EHCI_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
HAL_StatusTypeDef HAL_EHCI_Init(EHCI_HandleTypeDef *hhcd);
HAL_StatusTypeDef HAL_EHCI_DeInit(EHCI_HandleTypeDef *hhcd);
HAL_StatusTypeDef HAL_EHCI_HC_Init(EHCI_HandleTypeDef *hhcd, uint8_t ch_num,
                                  uint8_t epnum, uint8_t dev_address,
                                  uint8_t speed, uint8_t ep_type, uint16_t mps,
								  uint8_t tt_hubaddr,
								  uint8_t tt_prtaddr);

HAL_StatusTypeDef HAL_EHCI_HC_Halt(EHCI_HandleTypeDef *hhcd, uint8_t ch_num);
void              HAL_EHCI_MspInit(EHCI_HandleTypeDef *hhcd);
void              HAL_EHCI_MspDeInit(EHCI_HandleTypeDef *hhcd);

#if (USE_HAL_EHCI_REGISTER_CALLBACKS == 1U)
/** @defgroup HAL_EHCI_Callback_ID_enumeration_definition HAL USB OTG HCD Callback ID enumeration definition
  * @brief  HAL USB OTG HCD Callback ID enumeration definition
  * @{
  */
typedef enum
{
  HAL_EHCI_SOF_CB_ID            = 0x01,       /*!< USB HCD SOF callback ID           */
  HAL_EHCI_CONNECT_CB_ID        = 0x02,       /*!< USB HCD Connect callback ID       */
  HAL_EHCI_DISCONNECT_CB_ID     = 0x03,       /*!< USB HCD Disconnect callback ID    */
  HAL_EHCI_PORT_ENABLED_CB_ID   = 0x04,       /*!< USB HCD Port Enable callback ID   */
  HAL_EHCI_PORT_DISABLED_CB_ID  = 0x05,       /*!< USB HCD Port Disable callback ID  */

  HAL_EHCI_MSPINIT_CB_ID        = 0x06,       /*!< USB HCD MspInit callback ID       */
  HAL_EHCI_MSPDEINIT_CB_ID      = 0x07        /*!< USB HCD MspDeInit callback ID     */

} HAL_EHCI_CallbackIDTypeDef;
/**
  * @}
  */

/** @defgroup HAL_EHCI_Callback_pointer_definition HAL USB OTG HCD Callback pointer definition
  * @brief  HAL USB OTG HCD Callback pointer definition
  * @{
  */

typedef void (*pEHCI_CallbackTypeDef)(EHCI_HandleTypeDef *hhcd);                   /*!< pointer to a common USB OTG HCD callback function  */
typedef void (*pEHCI_HC_NotifyURBChangeCallbackTypeDef)(EHCI_HandleTypeDef *hhcd,
                                                       uint8_t epnum,
                                                       EHCI_URBStateTypeDef urb_state);   /*!< pointer to USB OTG HCD host channel  callback */
/**
  * @}
  */

HAL_StatusTypeDef HAL_EHCI_RegisterCallback(EHCI_HandleTypeDef *hhcd,
                                           HAL_EHCI_CallbackIDTypeDef CallbackID,
                                           pEHCI_CallbackTypeDef pCallback);

HAL_StatusTypeDef HAL_EHCI_UnRegisterCallback(EHCI_HandleTypeDef *hhcd,
                                             HAL_EHCI_CallbackIDTypeDef CallbackID);

HAL_StatusTypeDef HAL_EHCI_RegisterHC_NotifyURBChangeCallback(EHCI_HandleTypeDef *hhcd,
                                                             pEHCI_HC_NotifyURBChangeCallbackTypeDef pCallback);

HAL_StatusTypeDef HAL_EHCI_UnRegisterHC_NotifyURBChangeCallback(EHCI_HandleTypeDef *hhcd);
#endif /* USE_HAL_EHCI_REGISTER_CALLBACKS */
/**
  * @}
  */

/* I/O operation functions  ***************************************************/
/** @addtogroup EHCI_Exported_Functions_Group2 Input and Output operation functions
  * @{
  */
/* Non-Blocking mode: Interrupt */
void HAL_EHCI_IRQHandler(EHCI_HandleTypeDef *hhcd);
void HAL_EHCI_SOF_Callback(EHCI_HandleTypeDef *hhcd);
void HAL_EHCI_Connect_Callback(EHCI_HandleTypeDef *hhcd);
void HAL_EHCI_Disconnect_Callback(EHCI_HandleTypeDef *hhcd);
void HAL_EHCI_PortEnabled_Callback(EHCI_HandleTypeDef *hhcd);
void HAL_EHCI_PortDisabled_Callback(EHCI_HandleTypeDef *hhcd);
void HAL_EHCI_HC_NotifyURBChange_Callback(EHCI_HandleTypeDef *hhcd, uint8_t chnum,
                                         EHCI_URBStateTypeDef urb_state);
/**
  * @}
  */

/* Peripheral Control functions  **********************************************/
/** @addtogroup EHCI_Exported_Functions_Group3 Peripheral Control functions
  * @{
  */
HAL_StatusTypeDef HAL_EHCI_ResetPort(EHCI_HandleTypeDef *hhcd);
HAL_StatusTypeDef HAL_EHCI_ResetPort2(EHCI_HandleTypeDef *hhcd, uint8_t resetActiveState);
HAL_StatusTypeDef HAL_EHCI_Start(EHCI_HandleTypeDef *hhcd);
HAL_StatusTypeDef HAL_EHCI_Stop(EHCI_HandleTypeDef *hhcd);
/**
  * @}
  */

/* Peripheral State functions  ************************************************/
/** @addtogroup EHCI_Exported_Functions_Group4 Peripheral State functions
  * @{
  */
EHCI_StateTypeDef        HAL_EHCI_GetState(EHCI_HandleTypeDef *hhcd);
EHCI_URBStateTypeDef     HAL_EHCI_HC_GetURBState(EHCI_HandleTypeDef *hhcd, uint8_t chnum);
EHCI_HCStateTypeDef      HAL_EHCI_HC_GetState(EHCI_HandleTypeDef *hhcd, uint8_t chnum);
uint32_t                HAL_EHCI_HC_GetXferCount(EHCI_HandleTypeDef *hhcd, uint8_t chnum);
uint32_t                HAL_EHCI_GetCurrentFrame(EHCI_HandleTypeDef *hhcd);
uint32_t                HAL_EHCI_GetCurrentSpeed(EHCI_HandleTypeDef *hhcd);
uint_fast8_t 			HAL_EHCI_GetCurrentSpeedReady(EHCI_HandleTypeDef *hhcd);

/**
  * @}
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup EHCI_Private_Macros HCD Private Macros
  * @{
  */
/**
  * @}
  */
/* Private functions prototypes ----------------------------------------------*/

/**
 * @}
 */
/**
 * @}
 */


/** @file
 *
 * USB Enhanced Host Controller Interface (EHCI) driver
 *
 */

#ifdef __cplusplus
}
#endif

#endif /* xxxxxx_HAL_EHCI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
