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
    UART0_IRQn = 34,                                  /*!< UART  */
    UART1_IRQn = 35,                                  /*!< UART  */
    UART2_IRQn = 36,                                  /*!< UART  */
    UART3_IRQn = 37,                                  /*!< UART  */
    UART4_IRQn = 38,                                  /*!< UART  */
    UART5_IRQn = 39,                                  /*!< UART  */
    UART6_IRQn = 40,                                  /*!< UART  */
    UART7_IRQn = 41,                                  /*!< UART  */
    TWI0_IRQn = 42,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI1_IRQn = 43,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI2_IRQn = 44,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI3_IRQn = 45,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI4_IRQn = 46,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI5_IRQn = 47,                                   /*!< TWI Two Wire Interface (TWI) */
    SPI0_IRQn = 48,                                   /*!< SPI Serial Peripheral Interface */
    SPI1_IRQn = 49,                                   /*!< SPI Serial Peripheral Interface */
    SPI2_IRQn = 50,                                   /*!< SPI Serial Peripheral Interface */
    SPI3_IRQn = 53,                                   /*!< SPI Serial Peripheral Interface */
    UART8_IRQn = 180,                                 /*!< UART  */
    UART9_IRQn = 181,                                 /*!< UART  */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define GIC_INTERFACE_BASE 0
#define GIC_DISTRIBUTOR_BASE 0
#define TWI0_BASE ((uintptr_t) 0x02510000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI1_BASE ((uintptr_t) 0x02511000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI2_BASE ((uintptr_t) 0x02512000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI3_BASE ((uintptr_t) 0x02513000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI4_BASE ((uintptr_t) 0x02514000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI5_BASE ((uintptr_t) 0x02515000)            /*!< TWI Two Wire Interface (TWI) Base */
#define UART0_BASE ((uintptr_t) 0x02600000)           /*!< UART  Base */
#define UART1_BASE ((uintptr_t) 0x02610000)           /*!< UART  Base */
#define UART2_BASE ((uintptr_t) 0x02620000)           /*!< UART  Base */
#define UART3_BASE ((uintptr_t) 0x02630000)           /*!< UART  Base */
#define UART4_BASE ((uintptr_t) 0x02640000)           /*!< UART  Base */
#define UART5_BASE ((uintptr_t) 0x02650000)           /*!< UART  Base */
#define UART6_BASE ((uintptr_t) 0x02660000)           /*!< UART  Base */
#define UART7_BASE ((uintptr_t) 0x02670000)           /*!< UART  Base */
#define UART8_BASE ((uintptr_t) 0x02680000)           /*!< UART  Base */
#define UART9_BASE ((uintptr_t) 0x02690000)           /*!< UART  Base */
#define SPI0_BASE ((uintptr_t) 0x04025000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI1_BASE ((uintptr_t) 0x04026000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI2_BASE ((uintptr_t) 0x04027000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI3_BASE ((uintptr_t) 0x04028000)            /*!< SPI Serial Peripheral Interface Base */

#include <core_ca.h>

/*
 * @brief SPI
 */
/*!< SPI Serial Peripheral Interface */
typedef struct SPI_Type
{
         RESERVED(0x000[0x0004 - 0x0000], uint8_t)
    __IOM uint32_t SPI_GCR;                           /*!< Offset 0x004 SPI Global Control Register */
    __IOM uint32_t SPI_TCR;                           /*!< Offset 0x008 SPI Transfer Control Register */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IOM uint32_t SPI_IER;                           /*!< Offset 0x010 SPI Interrupt Control Register */
    __IOM uint32_t SPI_ISR;                           /*!< Offset 0x014 SPI Interrupt Status Register */
    __IOM uint32_t SPI_FCR;                           /*!< Offset 0x018 SPI FIFO Control Register */
    __IOM uint32_t SPI_FSR;                           /*!< Offset 0x01C SPI FIFO Status Register */
    __IOM uint32_t SPI_WCR;                           /*!< Offset 0x020 SPI Wait Clock Register */
    __IOM uint32_t SPI_CCR;                           /*!< Offset 0x024 SPI Clock Control Register (not documented) */
    __IOM uint32_t SPI_SAMP_DL;                       /*!< Offset 0x028 SPI Sample Delay Control Register */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __IOM uint32_t SPI_MBC;                           /*!< Offset 0x030 SPI Master Burst Counter Register */
    __IOM uint32_t SPI_MTC;                           /*!< Offset 0x034 SPI Master Transmit Counter Register */
    __IOM uint32_t SPI_BCC;                           /*!< Offset 0x038 SPI Master Burst Control Register */
         RESERVED(0x03C[0x0040 - 0x003C], uint8_t)
    __IOM uint32_t SPI_BATCR;                         /*!< Offset 0x040 SPI Bit-Aligned Transfer Configure Register */
    __IOM uint32_t SPI_BA_CCR;                        /*!< Offset 0x044 SPI Bit-Aligned Clock Configuration Register */
    __IOM uint32_t SPI_TBR;                           /*!< Offset 0x048 SPI TX Bit Register */
    __IOM uint32_t SPI_RBR;                           /*!< Offset 0x04C SPI RX Bit Register */
         RESERVED(0x050[0x0088 - 0x0050], uint8_t)
    __IOM uint32_t SPI_NDMA_MODE_CTL;                 /*!< Offset 0x088 SPI Normal DMA Mode Control Register */
         RESERVED(0x08C[0x0100 - 0x008C], uint8_t)
    __IOM uint32_t DBI_CTL_0;                         /*!< Offset 0x100 DBI Control Register 0 */
    __IOM uint32_t DBI_CTL_1;                         /*!< Offset 0x104 DBI Control Register 1 */
    __IOM uint32_t DBI_CTL_2;                         /*!< Offset 0x108 DBI Control Register 2 */
    __IOM uint32_t DBI_TIMER;                         /*!< Offset 0x10C DBI Timer Control Register */
    __IOM uint32_t DBI_VIDEO_SZIE;                    /*!< Offset 0x110 DBI Video Size Configuration Register */
         RESERVED(0x114[0x0120 - 0x0114], uint8_t)
    __IOM uint32_t DBI_INT;                           /*!< Offset 0x120 DBI Interrupt Register */
    __IOM uint32_t DBI_DEBUG_0;                       /*!< Offset 0x124 DBI BEBUG 0 Register */
    __IOM uint32_t DBI_DEBUG_1;                       /*!< Offset 0x128 DBI BEBUG 1 Register */
         RESERVED(0x12C[0x0200 - 0x012C], uint8_t)
    __IOM uint32_t SPI_TXD;                           /*!< Offset 0x200 SPI TX Data Register */
         RESERVED(0x204[0x0300 - 0x0204], uint8_t)
    __IOM uint32_t SPI_RXD;                           /*!< Offset 0x300 SPI RX Data Register */
         RESERVED(0x304[0x1000 - 0x0304], uint8_t)
} SPI_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief TWI
 */
/*!< TWI Two Wire Interface (TWI) */
typedef struct TWI_Type
{
    __IOM uint32_t TWI_ADDR;                          /*!< Offset 0x000 TWI Slave Address Register */
    __IOM uint32_t TWI_XADDR;                         /*!< Offset 0x004 TWI Extended Slave Address Register */
    __IOM uint32_t TWI_DATA;                          /*!< Offset 0x008 TWI Data Byte Register */
    __IOM uint32_t TWI_CNTR;                          /*!< Offset 0x00C TWI Control Register */
    __IOM uint32_t TWI_STAT;                          /*!< Offset 0x010 TWI Status Register */
    __IOM uint32_t TWI_CCR;                           /*!< Offset 0x014 TWI Clock Control Register */
    __IOM uint32_t TWI_SRST;                          /*!< Offset 0x018 TWI Software Reset Register */
    __IOM uint32_t TWI_EFR;                           /*!< Offset 0x01C TWI Enhance Feature Register */
    __IOM uint32_t TWI_LCR;                           /*!< Offset 0x020 TWI Line Control Register */
         RESERVED(0x024[0x0200 - 0x0024], uint8_t)
    __IOM uint32_t TWI_DRV_CTRL;                      /*!< Offset 0x200 TWI_DRV Control Register */
    __IOM uint32_t TWI_DRV_CFG;                       /*!< Offset 0x204 TWI_DRV Transmission Configuration Register */
    __IOM uint32_t TWI_DRV_SLV;                       /*!< Offset 0x208 TWI_DRV Slave ID Register */
    __IOM uint32_t TWI_DRV_FMT;                       /*!< Offset 0x20C TWI_DRV Packet Format Register */
    __IOM uint32_t TWI_DRV_BUS_CTRL;                  /*!< Offset 0x210 TWI_DRV Bus Control Register */
    __IOM uint32_t TWI_DRV_INT_CTRL;                  /*!< Offset 0x214 TWI_DRV Interrupt Control Register */
    __IOM uint32_t TWI_DRV_DMA_CFG;                   /*!< Offset 0x218 TWI_DRV DMA Configure Register */
    __IOM uint32_t TWI_DRV_FIFO_CON;                  /*!< Offset 0x21C TWI_DRV FIFO Content Register */
         RESERVED(0x220[0x0300 - 0x0220], uint8_t)
    __IOM uint32_t TWI_DRV_SEND_FIFO_ACC;             /*!< Offset 0x300 TWI_DRV Send Data FIFO Access Register */
    __IOM uint32_t TWI_DRV_RECV_FIFO_ACC;             /*!< Offset 0x304 TWI_DRV Receive Data FIFO Access Register */
         RESERVED(0x308[0x0400 - 0x0308], uint8_t)
} TWI_TypeDef; /* size of structure = 0x400 */
/*
 * @brief UART
 */
/*!< UART  */
typedef struct UART_Type
{
    __IOM uint32_t UART_RBR_THR_DLL;                  /*!< Offset 0x000 UART Receive Buffer Register/Transmit Holding Register */
    __IOM uint32_t UART_DLH_IER;                      /*!< Offset 0x004  */
    __IOM uint32_t UART_IIR_FCR;                      /*!< Offset 0x008 UART Interrupt Identity Register/UART FIFO Control Register */
    __IOM uint32_t UART_LCR;                          /*!< Offset 0x00C UART Line Control Register */
    __IOM uint32_t UART_MCR;                          /*!< Offset 0x010 UART Modem Control Register */
    __IOM uint32_t UART_LSR;                          /*!< Offset 0x014 UART Line Status Register */
    __IOM uint32_t UART_MSR;                          /*!< Offset 0x018 UART Modem Status Register */
    __IOM uint32_t UART_SCH;                          /*!< Offset 0x01C UART Scratch Register */
         RESERVED(0x020[0x007C - 0x0020], uint8_t)
    __IOM uint32_t UART_USR;                          /*!< Offset 0x07C UART Status Register */
    __IOM uint32_t UART_TFL;                          /*!< Offset 0x080 UART Transmit FIFO Level Register */
    __IOM uint32_t UART_RFL;                          /*!< Offset 0x084 UART Receive FIFO Level Register */
    __IOM uint32_t UART_HSK;                          /*!< Offset 0x088 UART DMA Handshake Configuration Register */
    __IOM uint32_t UART_DMA_REQ_EN;                   /*!< Offset 0x08C UART DMA Request Enable Register */
         RESERVED(0x090[0x00A4 - 0x0090], uint8_t)
    __IOM uint32_t UART_HALT;                         /*!< Offset 0x0A4 UART Halt TX Register */
         RESERVED(0x0A8[0x00B0 - 0x00A8], uint8_t)
    __IOM uint32_t UART_DBG_DLL;                      /*!< Offset 0x0B0 UART Debug DLL Register */
    __IOM uint32_t UART_DBG_DLH;                      /*!< Offset 0x0B4 UART Debug DLH Register */
         RESERVED(0x0B8[0x00F0 - 0x00B8], uint8_t)
    __IOM uint32_t UART_A_FCC;                        /*!< Offset 0x0F0 UART FIFO Clock Control Register */
         RESERVED(0x0F4[0x0100 - 0x00F4], uint8_t)
    __IOM uint32_t UART_A_RXDMA_CTRL;                 /*!< Offset 0x100 UART RXDMA Control Register */
    __IOM uint32_t UART_A_RXDMA_STR;                  /*!< Offset 0x104 UART RXDMA Start Register */
    __IOM uint32_t UART_A_RXDMA_STA;                  /*!< Offset 0x108 UART RXDMA Status Register */
    __IOM uint32_t UART_A_RXDMA_LMT;                  /*!< Offset 0x10C UART RXDMA Limit Register */
    __IOM uint32_t UART_A_RXDMA_SADDRL;               /*!< Offset 0x110 UART RXDMA Buffer Start Address Low Register */
    __IOM uint32_t UART_A_RXDMA_SADDRH;               /*!< Offset 0x114 UART RXDMA Buffer Start Address High Register */
    __IOM uint32_t UART_A_RXDMA_BL;                   /*!< Offset 0x118 UART RXDMA Buffer Length Register */
         RESERVED(0x11C[0x0120 - 0x011C], uint8_t)
    __IOM uint32_t UART_A_RXDMA_IE;                   /*!< Offset 0x120 UART RXDMA Interrupt Enable Register */
    __IOM uint32_t UART_A_RXDMA_IS;                   /*!< Offset 0x124 UART RXDMA Interrupt Status Register */
    __IOM uint32_t UART_A_RXDMA_WADDRL;               /*!< Offset 0x128 UART RXDMA Write Address Low Register */
    __IOM uint32_t UART_A_RXDMA_WADDRH;               /*!< Offset 0x12C UART RXDMA Write Address high Register */
    __IOM uint32_t UART_A_RXDMA_RADDRL;               /*!< Offset 0x130 UART RXDMA Read Address Low Register */
    __IOM uint32_t UART_A_RXDMA_RADDRH;               /*!< Offset 0x134 UART RXDMA Read Address high Register */
    __IOM uint32_t UART_A_RXDMA_DCNT;                 /*!< Offset 0x138 UART RADMA Data Count Register */
         RESERVED(0x13C[0x0400 - 0x013C], uint8_t)
} UART_TypeDef; /* size of structure = 0x400 */
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

#define TWI0 ((TWI_TypeDef *) TWI0_BASE)              /*!< TWI0 Two Wire Interface (TWI) register set access pointer */
#define TWI1 ((TWI_TypeDef *) TWI1_BASE)              /*!< TWI1 Two Wire Interface (TWI) register set access pointer */
#define TWI2 ((TWI_TypeDef *) TWI2_BASE)              /*!< TWI2 Two Wire Interface (TWI) register set access pointer */
#define TWI3 ((TWI_TypeDef *) TWI3_BASE)              /*!< TWI3 Two Wire Interface (TWI) register set access pointer */
#define TWI4 ((TWI_TypeDef *) TWI4_BASE)              /*!< TWI4 Two Wire Interface (TWI) register set access pointer */
#define TWI5 ((TWI_TypeDef *) TWI5_BASE)              /*!< TWI5 Two Wire Interface (TWI) register set access pointer */
#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0  register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1  register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2  register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3  register set access pointer */
#define UART4 ((UART_TypeDef *) UART4_BASE)           /*!< UART4  register set access pointer */
#define UART5 ((UART_TypeDef *) UART5_BASE)           /*!< UART5  register set access pointer */
#define UART6 ((UART_TypeDef *) UART6_BASE)           /*!< UART6  register set access pointer */
#define UART7 ((UART_TypeDef *) UART7_BASE)           /*!< UART7  register set access pointer */
#define UART8 ((UART_TypeDef *) UART8_BASE)           /*!< UART8  register set access pointer */
#define UART9 ((UART_TypeDef *) UART9_BASE)           /*!< UART9  register set access pointer */
#define SPI0 ((SPI_TypeDef *) SPI0_BASE)              /*!< SPI0 Serial Peripheral Interface register set access pointer */
#define SPI1 ((SPI_TypeDef *) SPI1_BASE)              /*!< SPI1 Serial Peripheral Interface register set access pointer */
#define SPI2 ((SPI_TypeDef *) SPI2_BASE)              /*!< SPI2 Serial Peripheral Interface register set access pointer */
#define SPI3 ((SPI_TypeDef *) SPI3_BASE)              /*!< SPI3 Serial Peripheral Interface register set access pointer */

#ifdef __cplusplus
 }
#endif /* __cplusplus */

#endif /* HEADER__858514A9_1AE9_4033_9FAD_BC80BB4264BB__INCLUDED */
