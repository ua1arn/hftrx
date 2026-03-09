/* Copyright (c) 2024 by Genadi V. Zawidowski. All rights reserved. */
#pragma once
#ifndef HEADER__858514A9_1AE9_4033_9FAD_BC80BB4264BB__INCLUDED
#define HEADER__858514A9_1AE9_4033_9FAD_BC80BB4264BB__INCLUDED
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* IRQs */

typedef enum IRQn
{

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */


#include <core_ca.h>

/*
 * @brief USBEHCI
 */
/*!< USBEHCI  */
typedef struct USBEHCI_Type
{
    __IOM uint16_t E_CAPLENGTH;                       /*!< Offset 0x000 EHCI Capability Register Length Register */
    __IOM uint16_t E_HCIVERSION;                      /*!< Offset 0x002 EHCI Host Interface Version Number Register */
    __IOM uint32_t E_HCSPARAMS;                       /*!< Offset 0x004 EHCI Host Control Structural Parameter Register */
    __IOM uint32_t E_HCCPARAMS;                       /*!< Offset 0x008 EHCI Host Control Capability Parameter Register */
    __IOM uint32_t E_HCSPPORTROUTE;                   /*!< Offset 0x00C EHCI Companion Port Route Description */
    __IOM uint32_t E_USBCMD;                          /*!< Offset 0x010 EHCI USB Command Register */
    __IOM uint32_t E_USBSTS;                          /*!< Offset 0x014 EHCI USB Status Register */
    __IOM uint32_t E_USBINTR;                         /*!< Offset 0x018 EHCI USB Interrupt Enable Register */
    __IOM uint32_t E_FRINDEX;                         /*!< Offset 0x01C EHCI USB Frame Index Register */
    __IOM uint32_t E_CTRLDSSEGMENT;                   /*!< Offset 0x020 EHCI 4G Segment Selector Register */
    __IOM uint32_t E_PERIODICLISTBASE;                /*!< Offset 0x024 EHCI Frame List Base Address Register */
    __IOM uint32_t E_ASYNCLISTADDR;                   /*!< Offset 0x028 EHCI Next Asynchronous List Address Register */
         RESERVED(0x02C[0x0050 - 0x002C], uint8_t)
    __IOM uint32_t E_CONFIGFLAG;                      /*!< Offset 0x050 EHCI Configured Flag Register */
    __IOM uint32_t E_PORTSC;                          /*!< Offset 0x054 EHCI Port Status/Control Register */
         RESERVED(0x058[0x0400 - 0x0058], uint8_t)
    __IOM uint32_t O_HcRevision;                      /*!< Offset 0x400 OHCI Revision Register (not documented) */
    __IOM uint32_t O_HcControl;                       /*!< Offset 0x404 OHCI Control Register */
    __IOM uint32_t O_HcCommandStatus;                 /*!< Offset 0x408 OHCI Command Status Register */
    __IOM uint32_t O_HcInterruptStatus;               /*!< Offset 0x40C OHCI Interrupt Status Register */
    __IOM uint32_t O_HcInterruptEnable;               /*!< Offset 0x410 OHCI Interrupt Enable Register */
    __IOM uint32_t O_HcInterruptDisable;              /*!< Offset 0x414 OHCI Interrupt Disable Register */
    __IOM uint32_t O_HcHCCA;                          /*!< Offset 0x418 OHCI HCCA Base */
    __IOM uint32_t O_HcPeriodCurrentED;               /*!< Offset 0x41C OHCI Period Current ED Base */
    __IOM uint32_t O_HcControlHeadED;                 /*!< Offset 0x420 OHCI Control Head ED Base */
    __IOM uint32_t O_HcControlCurrentED;              /*!< Offset 0x424 OHCI Control Current ED Base */
    __IOM uint32_t O_HcBulkHeadED;                    /*!< Offset 0x428 OHCI Bulk Head ED Base */
    __IOM uint32_t O_HcBulkCurrentED;                 /*!< Offset 0x42C OHCI Bulk Current ED Base */
    __IOM uint32_t O_HcDoneHead;                      /*!< Offset 0x430 OHCI Done Head Base */
    __IOM uint32_t O_HcFmInterval;                    /*!< Offset 0x434 OHCI Frame Interval Register */
    __IOM uint32_t O_HcFmRemaining;                   /*!< Offset 0x438 OHCI Frame Remaining Register */
    __IOM uint32_t O_HcFmNumber;                      /*!< Offset 0x43C OHCI Frame Number Register */
    __IOM uint32_t O_HcPerioddicStart;                /*!< Offset 0x440 OHCI Periodic Start Register */
    __IOM uint32_t O_HcLSThreshold;                   /*!< Offset 0x444 OHCI LS Threshold Register */
    __IOM uint32_t O_HcRhDescriptorA;                 /*!< Offset 0x448 OHCI Root Hub Descriptor Register A */
    __IOM uint32_t O_HcRhDesriptorB;                  /*!< Offset 0x44C OHCI Root Hub Descriptor Register B */
    __IOM uint32_t O_HcRhStatus;                      /*!< Offset 0x450 OHCI Root Hub Status Register */
    __IOM uint32_t O_HcRhPortStatus [0x001];          /*!< Offset 0x454 OHCI Root Hub Port Status Register */
} USBEHCI_TypeDef; /* size of structure = 0x458 */
/*
 * @brief USBOTG
 */
/*!< USBOTG USB OTG Dual-Role Device controller */
typedef struct USBOTG_Type
{
    __IOM uint32_t USB_EPFIFO [0x010];                /*!< Offset 0x000 USB_EPFIFO [0..5] USB FIFO Entry for Endpoint N */
    __IOM uint32_t USB_GCS;                           /*!< Offset 0x040 USB_POWER, USB_DEVCTL, USB_EPINDEX, USB_DMACTL USB Global Control and Status Register */
    __IOM uint32_t USB_EPINTF;                        /*!< Offset 0x044 USB_INTTX - lower 16, USB_INTRX - high 16, USB_EPINTF USB Endpoint Interrupt Flag Register */
    __IOM uint32_t USB_EPINTE;                        /*!< Offset 0x048 USB_INTTXE - lower 16, USB_INTRXE - high 16,  USB_EPINTE USB Endpoint Interrupt Enable Register */
    __IOM uint32_t USB_INTUSB;                        /*!< Offset 0x04C USB_INTUSB USB_BUSINTF USB Bus Interrupt Flag Register */
    __IOM uint32_t USB_INTUSBE;                       /*!< Offset 0x050 USB_INTUSBE USB_BUSINTE USB Bus Interrupt Enable Register */
    __IOM uint32_t USB_FNUM;                          /*!< Offset 0x054 USB Frame Number Register */
         RESERVED(0x058[0x007C - 0x0058], uint8_t)
    __IOM uint32_t USB_TESTC;                         /*!< Offset 0x07C USB_TESTC USB Test Control Register */
    __IOM uint16_t USB_TXMAXP;                        /*!< Offset 0x080 USB_TXMAXP USB EP1~5 Tx Control and Status Register */
    __IOM uint16_t USB_TXCSRHI;                       /*!< Offset 0x082 [15:8]: USB_TXCSRH, [7:0]: USB_TXCSRL */
    __IOM uint16_t USB_RXMAXP;                        /*!< Offset 0x084 USB_RXMAXP USB EP1~5 Rx Control and Status Register */
    __IOM uint16_t USB_RXCSRHI;                       /*!< Offset 0x086 USB_RXCSR */
    __IOM uint32_t USB_RXCOUNT;                       /*!< Offset 0x088 USB_RXCOUNT, USB_RXPKTCNT - high 16 bits */
    __IOM uint32_t USB_EPATTR;                        /*!< Offset 0x08C USB_EPATTR USB EP0 Attribute Register, USB EP1~5 Attribute Register */
    __IOM uint32_t USB_TXFIFO;                        /*!< Offset 0x090 USB_TXFIFO (bits 28:16 Start address of the endpoint FIFO is in units of 8 bytes) */
    __IOM uint32_t USB_RXFIFO;                        /*!< Offset 0x094 USB_RXFIFO (bits 28:16 Start address of the endpoint FIFO is in units of 8 bytes) */
    struct
    {
        __IOM uint16_t USB_TXFADDR;                   /*!< Offset 0x098 USB_TXFADDR */
        __IOM uint8_t  USB_TXHADDR;                   /*!< Offset 0x09A USB_TXHADDR */
        __IOM uint8_t  USB_TXHUBPORT;                 /*!< Offset 0x09B USB_TXHUBPORT */
        __IOM uint8_t  USB_RXFADDR;                   /*!< Offset 0x09C USB_RXFADDR */
             RESERVED(0x005[0x0006 - 0x0005], uint8_t)
        __IOM uint8_t  USB_RXHADDR;                   /*!< Offset 0x09E USB_RXHADDR */
        __IOM uint8_t  USB_RXHUBPORT;                 /*!< Offset 0x09F USB_RXHUBPORT */
    } FIFO [0x010];                                   /*!< Offset 0x098 FIFOs [0..5] */
         RESERVED(0x118[0x0400 - 0x0118], uint8_t)
    __IOM uint32_t USB_ISCR;                          /*!< Offset 0x400 HCI Interface Register (HCI_Interface) */
    __IOM uint32_t USBPHY_PHYCTL;                     /*!< Offset 0x404 USBPHY_PHYCTL */
    __IOM uint32_t HCI_CTRL3;                         /*!< Offset 0x408 HCI Control 3 Register (bist) */
         RESERVED(0x40C[0x0410 - 0x040C], uint8_t)
    __IOM uint32_t PHY_CTRL;                          /*!< Offset 0x410 PHY Control Register (PHY_Control) */
         RESERVED(0x414[0x0420 - 0x0414], uint8_t)
    __IOM uint32_t PHY_OTGCTL;                        /*!< Offset 0x420 Control PHY routing to EHCI or OTG */
    __IOM uint32_t PHY_STATUS;                        /*!< Offset 0x424 PHY Status Register */
    __IOM uint32_t USB_SPDCR;                         /*!< Offset 0x428 HCI SIE Port Disable Control Register */
         RESERVED(0x42C[0x0500 - 0x042C], uint8_t)
    __IOM uint32_t USB_DMA_INTE;                      /*!< Offset 0x500 USB DMA Interrupt Enable Register */
    __IOM uint32_t USB_DMA_INTS;                      /*!< Offset 0x504 USB DMA Interrupt Status Register */
         RESERVED(0x508[0x0540 - 0x0508], uint8_t)
    struct
    {
        __IOM uint32_t CHAN_CFG;                      /*!< Offset 0x540 USB DMA Channel Configuration Register */
        __IOM uint32_t SDRAM_ADD;                     /*!< Offset 0x544 USB DMA SDRAM Start Address Register  */
        __IOM uint32_t BC;                            /*!< Offset 0x548 USB DMA Byte Counter Register */
        __IM  uint32_t RESIDUAL_BC;                   /*!< Offset 0x54C USB DMA RESIDUAL Byte Counter Register */
    } USB_DMA [0x010];                                /*!< Offset 0x540  */
} USBOTG_TypeDef; /* size of structure = 0x640 */
/*
 * @brief USB_EHCI_Capability
 */
/*!< USB_EHCI_Capability  */
typedef struct USB_EHCI_Capability_Type
{
    __IM  uint32_t HCCAPBASE;                         /*!< Offset 0x000 EHCI Capability Register (HCIVERSION and CAPLENGTH) register */
    __IM  uint32_t HCSPARAMS;                         /*!< Offset 0x004 EHCI Host Control Structural Parameter Register */
    __IM  uint32_t HCCPARAMS;                         /*!< Offset 0x008 EHCI Host Control Capability Parameter Register */
    __IOM uint32_t HCSPPORTROUTE;                     /*!< Offset 0x00C EHCI Companion Port Route Description */
    __IOM uint32_t USBCMD;                            /*!< Offset 0x010 EHCI USB Command Register */
    __IOM uint32_t USBSTS;                            /*!< Offset 0x014 EHCI USB Status Register */
    __IOM uint32_t USBINTR;                           /*!< Offset 0x018 EHCI USB Interrupt Enable Register */
    __IOM uint32_t FRINDEX;                           /*!< Offset 0x01C EHCI USB Frame Index Register */
    __IOM uint32_t CTRLDSSEGMENT;                     /*!< Offset 0x020 EHCI 4G Segment Selector Register */
    __IOM uint32_t PERIODICLISTBASE;                  /*!< Offset 0x024 EHCI Frame List Base Address Register */
    __IOM uint32_t ASYNCLISTADDR;                     /*!< Offset 0x028 EHCI Next Asynchronous List Address Register */
} USB_EHCI_Capability_TypeDef; /* size of structure = 0x02C */
/*
 * @brief USB_OHCI_Capability
 */
/*!< USB_OHCI_Capability  */
typedef struct USB_OHCI_Capability_Type
{
    __IOM uint32_t O_HcRevision;                      /*!< Offset 0x000 OHCI Revision Register (not documented) */
    __IOM uint32_t O_HcControl;                       /*!< Offset 0x004 OHCI Control Register */
    __IOM uint32_t O_HcCommandStatus;                 /*!< Offset 0x008 OHCI Command Status Register */
    __IOM uint32_t O_HcInterruptStatus;               /*!< Offset 0x00C OHCI Interrupt Status Register */
    __IOM uint32_t O_HcInterruptEnable;               /*!< Offset 0x010 OHCI Interrupt Enable Register */
    __IOM uint32_t O_HcInterruptDisable;              /*!< Offset 0x014 OHCI Interrupt Disable Register */
    __IOM uint32_t O_HcHCCA;                          /*!< Offset 0x018 OHCI HCCA Base */
    __IOM uint32_t O_HcPeriodCurrentED;               /*!< Offset 0x01C OHCI Period Current ED Base */
    __IOM uint32_t O_HcControlHeadED;                 /*!< Offset 0x020 OHCI Control Head ED Base */
    __IOM uint32_t O_HcControlCurrentED;              /*!< Offset 0x024 OHCI Control Current ED Base */
    __IOM uint32_t O_HcBulkHeadED;                    /*!< Offset 0x028 OHCI Bulk Head ED Base */
    __IOM uint32_t O_HcBulkCurrentED;                 /*!< Offset 0x02C OHCI Bulk Current ED Base */
    __IOM uint32_t O_HcDoneHead;                      /*!< Offset 0x030 OHCI Done Head Base */
    __IOM uint32_t O_HcFmInterval;                    /*!< Offset 0x034 OHCI Frame Interval Register */
    __IOM uint32_t O_HcFmRemaining;                   /*!< Offset 0x038 OHCI Frame Remaining Register */
    __IOM uint32_t O_HcFmNumber;                      /*!< Offset 0x03C OHCI Frame Number Register */
    __IOM uint32_t O_HcPerioddicStart;                /*!< Offset 0x040 OHCI Periodic Start Register */
    __IOM uint32_t O_HcLSThreshold;                   /*!< Offset 0x044 OHCI LS Threshold Register */
    __IOM uint32_t O_HcRhDescriptorA;                 /*!< Offset 0x048 OHCI Root Hub Descriptor Register A */
    __IOM uint32_t O_HcRhDesriptorB;                  /*!< Offset 0x04C OHCI Root Hub Descriptor Register B */
    __IOM uint32_t O_HcRhStatus;                      /*!< Offset 0x050 OHCI Root Hub Status Register */
    __IOM uint32_t O_HcRhPortStatus [0x001];          /*!< Offset 0x054 OHCI Root Hub Port Status Register */
} USB_OHCI_Capability_TypeDef; /* size of structure = 0x058 */


/* Defines */



/* Access pointers */


#ifdef __cplusplus
 }
#endif /* __cplusplus */

#endif /* HEADER__858514A9_1AE9_4033_9FAD_BC80BB4264BB__INCLUDED */
