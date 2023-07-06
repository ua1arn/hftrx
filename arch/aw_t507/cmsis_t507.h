#pragma once
#ifndef HEADER_00003039_INCLUDED
#define HEADER_00003039_INCLUDED
#include <stdint.h>


/* IRQs */

typedef enum IRQn
{
    SGI0_IRQn = 0,                                    /*!<   Interrupt */
    SGI1_IRQn = 1,                                    /*!<   Interrupt */
    SGI2_IRQn = 2,                                    /*!<   Interrupt */
    SGI3_IRQn = 3,                                    /*!<   Interrupt */
    SGI4_IRQn = 4,                                    /*!<   Interrupt */
    SGI5_IRQn = 5,                                    /*!<   Interrupt */
    SGI6_IRQn = 6,                                    /*!<   Interrupt */
    SGI7_IRQn = 7,                                    /*!<   Interrupt */
    SGI8_IRQn = 8,                                    /*!<   Interrupt */
    SGI9_IRQn = 9,                                    /*!<   Interrupt */
    SGI10_IRQn = 10,                                  /*!<   Interrupt */
    SGI11_IRQn = 11,                                  /*!<   Interrupt */
    SGI12_IRQn = 12,                                  /*!<   Interrupt */
    SGI13_IRQn = 13,                                  /*!<   Interrupt */
    SGI14_IRQn = 14,                                  /*!<   Interrupt */
    SGI15_IRQn = 15,                                  /*!<   Interrupt */
    VirtualMaintenanceInterrupt_IRQn = 25,            /*!<   Interrupt */
    HypervisorTimer_IRQn = 26,                        /*!<   Interrupt */
    VirtualTimer_IRQn = 27,                           /*!<   Interrupt */
    Legacy_nFIQ_IRQn = 28,                            /*!<   Interrupt */
    SecurePhysicalTimer_IRQn = 29,                    /*!<   Interrupt */
    NonSecurePhysicalTimer_IRQn = 30,                 /*!<   Interrupt */
    Legacy_nIRQ_IRQn = 31,                            /*!<   Interrupt */
    UART0_IRQn = 34,                                  /*!< UART  Interrupt */
    UART1_IRQn = 35,                                  /*!< UART  Interrupt */
    UART2_IRQn = 36,                                  /*!< UART  Interrupt */
    UART3_IRQn = 37,                                  /*!< UART  Interrupt */
    UART4_IRQn = 38,                                  /*!< UART  Interrupt */
    UART5_IRQn = 39,                                  /*!< UART  Interrupt */
    TWI0_IRQn = 41,                                   /*!< TWI  Interrupt */
    TWI1_IRQn = 42,                                   /*!< TWI  Interrupt */
    TWI2_IRQn = 43,                                   /*!< TWI  Interrupt */
    TWI3_IRQn = 44,                                   /*!< TWI  Interrupt */
    GPIOB_NS_IRQn = 101,                              /*!< GPIOINT  Interrupt */
    GPIOB_S_IRQn = 102,                               /*!< GPIOINT  Interrupt */
    GPIOC_NS_IRQn = 103,                              /*!< GPIOINT  Interrupt */
    USBOTG0_IRQn = 103,                               /*!< USBOTG USB OTG Dual-Role Device controller Interrupt */
    GPIOC_S_IRQn = 104,                               /*!< GPIOINT  Interrupt */
    USBEHCI0_IRQn = 104,                              /*!< USB_EHCI_Capability  Interrupt */
    GPIOD_NS_IRQn = 105,                              /*!< GPIOINT  Interrupt */
    USBOHCI0_IRQn = 105,                              /*!< USB_OHCI_Capability  Interrupt */
    GPIOD_S_IRQn = 106,                               /*!< GPIOINT  Interrupt */
    USBEHCI1_IRQn = 106,                              /*!< USB_EHCI_Capability  Interrupt */
    GPIOE_NS_IRQn = 107,                              /*!< GPIOINT  Interrupt */
    USBOHCI1_IRQn = 107,                              /*!< USB_OHCI_Capability  Interrupt */
    GPIOE_S_IRQn = 108,                               /*!< GPIOINT  Interrupt */
    GPIOF_NS_IRQn = 109,                              /*!< GPIOINT  Interrupt */
    GPIOF_S_IRQn = 110,                               /*!< GPIOINT  Interrupt */
    GPIOG_NS_IRQn = 111,                              /*!< GPIOINT  Interrupt */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define GPU_BASE ((uintptr_t) 0x01800000)             /*!< GPU Base */
#define USBPHY0_BASE ((uintptr_t) 0x01C1A800)         /*!< USBPHYC Base */
#define USBPHY1_BASE ((uintptr_t) 0x01C1B800)         /*!< USBPHYC Base */
#define GPIOBLOCK_L_BASE ((uintptr_t) 0x01F02C00)     /*!< GPIOBLOCK Base */
#define TWI0_BASE ((uintptr_t) 0x02502000)            /*!< TWI Base */
#define TWI1_BASE ((uintptr_t) 0x02502400)            /*!< TWI Base */
#define TWI2_BASE ((uintptr_t) 0x02502800)            /*!< TWI Base */
#define TWI3_BASE ((uintptr_t) 0x02502C00)            /*!< TWI Base */
#define TWI4_BASE ((uintptr_t) 0x02503000)            /*!< TWI Base */
#define GPIOA_BASE ((uintptr_t) 0x0300B000)           /*!< GPIO Base */
#define GPIOBLOCK_BASE ((uintptr_t) 0x0300B000)       /*!< GPIOBLOCK Base */
#define GPIOE_BASE ((uintptr_t) 0x0300B000)           /*!< GPIO Base */
#define GPIOF_BASE ((uintptr_t) 0x0300B000)           /*!< GPIO Base */
#define GPIOG_BASE ((uintptr_t) 0x0300B000)           /*!< GPIO Base */
#define GPIOH_BASE ((uintptr_t) 0x0300B000)           /*!< GPIO Base */
#define GPIOI_BASE ((uintptr_t) 0x0300B000)           /*!< GPIO Base */
#define GPIOL_BASE ((uintptr_t) 0x0300B000)           /*!< GPIO Base */
#define GPIOB_BASE ((uintptr_t) 0x0300B024)           /*!< GPIO Base */
#define GPIOC_BASE ((uintptr_t) 0x0300B048)           /*!< GPIO Base */
#define GPIOD_BASE ((uintptr_t) 0x0300B06C)           /*!< GPIO Base */
#define GPIOINTA_BASE ((uintptr_t) 0x0300B200)        /*!< GPIOINT Base */
#define GPIOINTC_BASE ((uintptr_t) 0x0300B200)        /*!< GPIOINT Base */
#define GPIOINTD_BASE ((uintptr_t) 0x0300B200)        /*!< GPIOINT Base */
#define GPIOINTE_BASE ((uintptr_t) 0x0300B200)        /*!< GPIOINT Base */
#define GPIOINTF_BASE ((uintptr_t) 0x0300B200)        /*!< GPIOINT Base */
#define GPIOINTG_BASE ((uintptr_t) 0x0300B200)        /*!< GPIOINT Base */
#define GPIOINTH_BASE ((uintptr_t) 0x0300B200)        /*!< GPIOINT Base */
#define GPIOINTI_BASE ((uintptr_t) 0x0300B200)        /*!< GPIOINT Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0x03021000) /*!<  Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0x03022000)   /*!<  Base */
#define UART0_BASE ((uintptr_t) 0x05000000)           /*!< UART Base */
#define UART1_BASE ((uintptr_t) 0x05000400)           /*!< UART Base */
#define UART2_BASE ((uintptr_t) 0x05000800)           /*!< UART Base */
#define UART3_BASE ((uintptr_t) 0x05000C00)           /*!< UART Base */
#define UART4_BASE ((uintptr_t) 0x05001000)           /*!< UART Base */
#define UART5_BASE ((uintptr_t) 0x05001400)           /*!< UART Base */
#define USB0_BASE ((uintptr_t) 0x05100000)            /*!< USBOTG Base */
#define USBOTG0_BASE ((uintptr_t) 0x05100000)         /*!< USBOTG Base */
#define USB1_BASE ((uintptr_t) 0x05200000)            /*!< USB_EHCI_Capability Base */
#define USBOHCI1_BASE ((uintptr_t) 0x05200400)        /*!< USB_OHCI_Capability Base */
#define USB3_BASE ((uintptr_t) 0x05310000)            /*!< USB_EHCI_Capability Base */
#define USBOHCI3_BASE ((uintptr_t) 0x05310400)        /*!< USB_OHCI_Capability Base */
#define USB4_BASE ((uintptr_t) 0x05311000)            /*!< USB_EHCI_Capability Base */
#define USBOHCI4_BASE ((uintptr_t) 0x05311400)        /*!< USB_OHCI_Capability Base */

/*
 * @brief UART
 */
/*!< UART  */
typedef struct UART_Type
{
    volatile uint32_t UART_RBR_THR_DLL;               /*!< Offset 0x000 UART Receive Buffer Register/Transmit Holding Register */
    volatile uint32_t UART_DLH_IER;                   /*!< Offset 0x004  */
    volatile uint32_t UART_IIR_FCR;                   /*!< Offset 0x008 UART Interrupt Identity Register/UART FIFO Control Register */
    volatile uint32_t UART_LCR;                       /*!< Offset 0x00C UART Line Control Register */
    volatile uint32_t UART_MCR;                       /*!< Offset 0x010 UART Modem Control Register */
    volatile uint32_t UART_LSR;                       /*!< Offset 0x014 UART Line Status Register */
    volatile uint32_t UART_MSR;                       /*!< Offset 0x018 UART Modem Status Register */
    volatile uint32_t UART_SCH;                       /*!< Offset 0x01C UART Scratch Register */
             uint32_t reserved_0x020 [0x0017];
    volatile uint32_t UART_USR;                       /*!< Offset 0x07C UART Status Register */
    volatile uint32_t UART_TFL;                       /*!< Offset 0x080 UART Transmit FIFO Level Register */
    volatile uint32_t UART_RFL;                       /*!< Offset 0x084 UART Receive FIFO Level Register */
    volatile uint32_t UART_HSK;                       /*!< Offset 0x088 UART DMA Handshake Configuration Register */
    volatile uint32_t UART_DMA_REQ_EN;                /*!< Offset 0x08C UART DMA Request Enable Register */
             uint32_t reserved_0x090 [0x0005];
    volatile uint32_t UART_HALT;                      /*!< Offset 0x0A4 UART Halt TX Register */
             uint32_t reserved_0x0A8 [0x0002];
    volatile uint32_t UART_DBG_DLL;                   /*!< Offset 0x0B0 UART Debug DLL Register */
    volatile uint32_t UART_DBG_DLH;                   /*!< Offset 0x0B4 UART Debug DLH Register */
             uint32_t reserved_0x0B8 [0x000E];
    volatile uint32_t UART_A_FCC;                     /*!< Offset 0x0F0 UART FIFO Clock Control Register */
             uint32_t reserved_0x0F4 [0x0003];
    volatile uint32_t UART_A_RXDMA_CTRL;              /*!< Offset 0x100 UART RXDMA Control Register */
    volatile uint32_t UART_A_RXDMA_STR;               /*!< Offset 0x104 UART RXDMA Start Register */
    volatile uint32_t UART_A_RXDMA_STA;               /*!< Offset 0x108 UART RXDMA Status Register */
    volatile uint32_t UART_A_RXDMA_LMT;               /*!< Offset 0x10C UART RXDMA Limit Register */
    volatile uint32_t UART_A_RXDMA_SADDRL;            /*!< Offset 0x110 UART RXDMA Buffer Start Address Low Register */
    volatile uint32_t UART_A_RXDMA_SADDRH;            /*!< Offset 0x114 UART RXDMA Buffer Start Address High Register */
    volatile uint32_t UART_A_RXDMA_BL;                /*!< Offset 0x118 UART RXDMA Buffer Length Register */
             uint32_t reserved_0x11C;
    volatile uint32_t UART_A_RXDMA_IE;                /*!< Offset 0x120 UART RXDMA Interrupt Enable Register */
    volatile uint32_t UART_A_RXDMA_IS;                /*!< Offset 0x124 UART RXDMA Interrupt Status Register */
    volatile uint32_t UART_A_RXDMA_WADDRL;            /*!< Offset 0x128 UART RXDMA Write Address Low Register */
    volatile uint32_t UART_A_RXDMA_WADDRH;            /*!< Offset 0x12C UART RXDMA Write Address high Register */
    volatile uint32_t UART_A_RXDMA_RADDRL;            /*!< Offset 0x130 UART RXDMA Read Address Low Register */
    volatile uint32_t UART_A_RXDMA_RADDRH;            /*!< Offset 0x134 UART RXDMA Read Address high Register */
    volatile uint32_t UART_A_RXDMA_DCNT;              /*!< Offset 0x138 UART RADMA Data Count Register */
} UART_TypeDef; /* size of structure = 0x13C */
/*
 * @brief GPIO
 */
/*!< GPIO  */
typedef struct GPIO_Type
{
    volatile uint32_t CFG [0x004];                    /*!< Offset 0x000 Configure Register */
    volatile uint32_t DATA;                           /*!< Offset 0x010 Data Register */
    volatile uint32_t DRV [0x002];                    /*!< Offset 0x014 Multi_Driving Register */
    volatile uint32_t PULL [0x002];                   /*!< Offset 0x01C Pull Register */
} GPIO_TypeDef; /* size of structure = 0x024 */
/*
 * @brief GPIOINT
 */
/*!< GPIOINT  */
typedef struct GPIOINT_Type
{
    volatile uint32_t EINT_CFG [0x004];               /*!< Offset 0x000 External Interrupt Configure Registers */
    volatile uint32_t EINT_CTL;                       /*!< Offset 0x010 External Interrupt Control Register */
    volatile uint32_t EINT_STATUS;                    /*!< Offset 0x014 External Interrupt Status Register */
    volatile uint32_t EINT_DEB;                       /*!< Offset 0x018 External Interrupt Debounce Register */
             uint32_t reserved_0x01C;
} GPIOINT_TypeDef; /* size of structure = 0x020 */
/*
 * @brief GPIOBLOCK
 */
/*!< GPIOBLOCK  */
typedef struct GPIOBLOCK_Type
{
    struct
    {
        volatile uint32_t CFG [0x004];                /*!< Offset 0x000 Configure Register */
        volatile uint32_t DATA;                       /*!< Offset 0x010 Data Register */
        volatile uint32_t DRV [0x002];                /*!< Offset 0x014 Multi_Driving Register */
        volatile uint32_t PULL [0x002];               /*!< Offset 0x01C Pull Register */
    } GPIO_PINS [0x008];                              /*!< Offset 0x000 GPIO pin control */
             uint32_t reserved_0x120 [0x0038];
    struct
    {
        volatile uint32_t EINT_CFG [0x004];           /*!< Offset 0x200 External Interrupt Configure Registers */
        volatile uint32_t EINT_CTL;                   /*!< Offset 0x210 External Interrupt Control Register */
        volatile uint32_t EINT_STATUS;                /*!< Offset 0x214 External Interrupt Status Register */
        volatile uint32_t EINT_DEB;                   /*!< Offset 0x218 External Interrupt Debounce Register */
                 uint32_t reserved_0x01C;
    } GPIO_INTS [0x008];                              /*!< Offset 0x200 GPIO interrupt control */
} GPIOBLOCK_TypeDef; /* size of structure = 0x300 */
/*
 * @brief TWI
 */
/*!< TWI  */
typedef struct TWI_Type
{
    volatile uint32_t TWI_ADDR;                       /*!< Offset 0x000 TWI Slave Address Register */
    volatile uint32_t TWI_XADDR;                      /*!< Offset 0x004 TWI Extended Slave Address Register */
    volatile uint32_t TWI_DATA;                       /*!< Offset 0x008 TWI Data Byte Register */
    volatile uint32_t TWI_CNTR;                       /*!< Offset 0x00C TWI Control Register */
    volatile uint32_t TWI_STAT;                       /*!< Offset 0x010 TWI Status Register */
    volatile uint32_t TWI_CCR;                        /*!< Offset 0x014 TWI Clock Control Register */
    volatile uint32_t TWI_SRST;                       /*!< Offset 0x018 TWI Software Reset Register */
    volatile uint32_t TWI_EFR;                        /*!< Offset 0x01C TWI Enhance Feature Register */
    volatile uint32_t TWI_LCR;                        /*!< Offset 0x020 TWI Line Control Register */
             uint32_t reserved_0x024 [0x0077];
    volatile uint32_t TWI_DRV_CTRL;                   /*!< Offset 0x200 TWI_DRV Control Register */
    volatile uint32_t TWI_DRV_CFG;                    /*!< Offset 0x204 TWI_DRV Transmission Configuration Register */
    volatile uint32_t TWI_DRV_SLV;                    /*!< Offset 0x208 TWI_DRV Slave ID Register */
    volatile uint32_t TWI_DRV_FMT;                    /*!< Offset 0x20C TWI_DRV Packet Format Register */
    volatile uint32_t TWI_DRV_BUS_CTRL;               /*!< Offset 0x210 TWI_DRV Bus Control Register */
    volatile uint32_t TWI_DRV_INT_CTRL;               /*!< Offset 0x214 TWI_DRV Interrupt Control Register */
    volatile uint32_t TWI_DRV_DMA_CFG;                /*!< Offset 0x218 TWI_DRV DMA Configure Register */
    volatile uint32_t TWI_DRV_FIFO_CON;               /*!< Offset 0x21C TWI_DRV FIFO Content Register */
             uint32_t reserved_0x220 [0x0038];
    volatile uint32_t TWI_DRV_SEND_FIFO_ACC;          /*!< Offset 0x300 TWI_DRV Send Data FIFO Access Register */
    volatile uint32_t TWI_DRV_RECV_FIFO_ACC;          /*!< Offset 0x304 TWI_DRV Receive Data FIFO Access Register */
} TWI_TypeDef; /* size of structure = 0x308 */
/*
 * @brief USBEHCI
 */
/*!< USBEHCI  */
typedef struct USBEHCI_Type
{
    volatile uint16_t E_CAPLENGTH;                    /*!< Offset 0x000 EHCI Capability Register Length Register */
    volatile uint16_t E_HCIVERSION;                   /*!< Offset 0x002 EHCI Host Interface Version Number Register */
    volatile uint32_t E_HCSPARAMS;                    /*!< Offset 0x004 EHCI Host Control Structural Parameter Register */
    volatile uint32_t E_HCCPARAMS;                    /*!< Offset 0x008 EHCI Host Control Capability Parameter Register */
    volatile uint32_t E_HCSPPORTROUTE;                /*!< Offset 0x00C EHCI Companion Port Route Description */
    volatile uint32_t E_USBCMD;                       /*!< Offset 0x010 EHCI USB Command Register */
    volatile uint32_t E_USBSTS;                       /*!< Offset 0x014 EHCI USB Status Register */
    volatile uint32_t E_USBINTR;                      /*!< Offset 0x018 EHCI USB Interrupt Enable Register */
    volatile uint32_t E_FRINDEX;                      /*!< Offset 0x01C EHCI USB Frame Index Register */
    volatile uint32_t E_CTRLDSSEGMENT;                /*!< Offset 0x020 EHCI 4G Segment Selector Register */
    volatile uint32_t E_PERIODICLISTBASE;             /*!< Offset 0x024 EHCI Frame List Base Address Register */
    volatile uint32_t E_ASYNCLISTADDR;                /*!< Offset 0x028 EHCI Next Asynchronous List Address Register */
             uint32_t reserved_0x02C [0x0009];
    volatile uint32_t E_CONFIGFLAG;                   /*!< Offset 0x050 EHCI Configured Flag Register */
    volatile uint32_t E_PORTSC;                       /*!< Offset 0x054 EHCI Port Status/Control Register */
             uint32_t reserved_0x058 [0x00EA];
    volatile uint32_t O_HcRevision;                   /*!< Offset 0x400 OHCI Revision Register (not documented) */
    volatile uint32_t O_HcControl;                    /*!< Offset 0x404 OHCI Control Register */
    volatile uint32_t O_HcCommandStatus;              /*!< Offset 0x408 OHCI Command Status Register */
    volatile uint32_t O_HcInterruptStatus;            /*!< Offset 0x40C OHCI Interrupt Status Register */
    volatile uint32_t O_HcInterruptEnable;            /*!< Offset 0x410 OHCI Interrupt Enable Register */
    volatile uint32_t O_HcInterruptDisable;           /*!< Offset 0x414 OHCI Interrupt Disable Register */
    volatile uint32_t O_HcHCCA;                       /*!< Offset 0x418 OHCI HCCA Base */
    volatile uint32_t O_HcPeriodCurrentED;            /*!< Offset 0x41C OHCI Period Current ED Base */
    volatile uint32_t O_HcControlHeadED;              /*!< Offset 0x420 OHCI Control Head ED Base */
    volatile uint32_t O_HcControlCurrentED;           /*!< Offset 0x424 OHCI Control Current ED Base */
    volatile uint32_t O_HcBulkHeadED;                 /*!< Offset 0x428 OHCI Bulk Head ED Base */
    volatile uint32_t O_HcBulkCurrentED;              /*!< Offset 0x42C OHCI Bulk Current ED Base */
    volatile uint32_t O_HcDoneHead;                   /*!< Offset 0x430 OHCI Done Head Base */
    volatile uint32_t O_HcFmInterval;                 /*!< Offset 0x434 OHCI Frame Interval Register */
    volatile uint32_t O_HcFmRemaining;                /*!< Offset 0x438 OHCI Frame Remaining Register */
    volatile uint32_t O_HcFmNumber;                   /*!< Offset 0x43C OHCI Frame Number Register */
    volatile uint32_t O_HcPerioddicStart;             /*!< Offset 0x440 OHCI Periodic Start Register */
    volatile uint32_t O_HcLSThreshold;                /*!< Offset 0x444 OHCI LS Threshold Register */
    volatile uint32_t O_HcRhDescriptorA;              /*!< Offset 0x448 OHCI Root Hub Descriptor Register A */
    volatile uint32_t O_HcRhDesriptorB;               /*!< Offset 0x44C OHCI Root Hub Descriptor Register B */
    volatile uint32_t O_HcRhStatus;                   /*!< Offset 0x450 OHCI Root Hub Status Register */
    volatile uint32_t O_HcRhPortStatus [0x001];       /*!< Offset 0x454 OHCI Root Hub Port Status Register */
} USBEHCI_TypeDef; /* size of structure = 0x458 */
/*
 * @brief USBOTG
 */
/*!< USBOTG USB OTG Dual-Role Device controller */
typedef struct USBOTG_Type
{
    volatile uint32_t USB_EPFIFO [0x006];             /*!< Offset 0x000 USB_EPFIFO [0..5] USB FIFO Entry for Endpoint N */
             uint32_t reserved_0x018 [0x000A];
    volatile uint32_t USB_GCS;                        /*!< Offset 0x040 USB_POWER, USB_DEVCTL, USB_EPINDEX, USB_DMACTL USB Global Control and Status Register */
    volatile uint16_t USB_INTTX;                      /*!< Offset 0x044 USB_INTTX USB_EPINTF USB Endpoint Interrupt Flag Register */
    volatile uint16_t USB_INTRX;                      /*!< Offset 0x046 USB_INTRX USB_EPINTF */
    volatile uint16_t USB_INTTXE;                     /*!< Offset 0x048 USB_INTTXE USB_EPINTE USB Endpoint Interrupt Enable Register */
    volatile uint16_t USB_INTRXE;                     /*!< Offset 0x04A USB_INTRXE USB_EPINTE */
    volatile uint32_t USB_INTUSB;                     /*!< Offset 0x04C USB_INTUSB USB_BUSINTF USB Bus Interrupt Flag Register */
    volatile uint32_t USB_INTUSBE;                    /*!< Offset 0x050 USB_INTUSBE USB_BUSINTE USB Bus Interrupt Enable Register */
    volatile uint32_t USB_FNUM;                       /*!< Offset 0x054 USB Frame Number Register */
             uint32_t reserved_0x058 [0x0009];
    volatile uint32_t USB_TESTC;                      /*!< Offset 0x07C USB_TESTC USB Test Control Register */
    volatile uint16_t USB_TXMAXP;                     /*!< Offset 0x080 USB_TXMAXP USB EP1~5 Tx Control and Status Register */
    volatile uint16_t USB_CSR0;                       /*!< Offset 0x082 [15:8]: USB_TXCSRH, [7:0]: USB_TXCSRL */
    volatile uint16_t USB_RXMAXP;                     /*!< Offset 0x084 USB_RXMAXP USB EP1~5 Rx Control and Status Register */
    volatile uint16_t USB_RXCSR;                      /*!< Offset 0x086 USB_RXCSR */
    volatile uint16_t USB_RXCOUNT;                    /*!< Offset 0x088 USB_RXCOUNT */
    volatile uint16_t USB_RXPKTCNT;                   /*!< Offset 0x08A USB_RXPKTCNT */
    volatile uint8_t  USB_TXTI;                       /*!< Offset 0x08C USB_TXTI */
    volatile uint8_t  USB_TXNAKLIMIT;                 /*!< Offset 0x08D USB_TXNAKLIMIT */
    volatile uint8_t  USB_RXTI;                       /*!< Offset 0x08E USB_RXTI */
    volatile uint8_t  USB_RXNAKLIMIT;                 /*!< Offset 0x08F USB_RXNAKLIMIT */
    volatile uint16_t USB_TXFIFOSZ;                   /*!< Offset 0x090 USB_TXFIFOSZ */
    volatile uint16_t USB_TXFIFOADD;                  /*!< Offset 0x092 USB_TXFIFOADD */
    volatile uint16_t USB_RXFIFOSZ;                   /*!< Offset 0x094 USB_RXFIFOSZ */
    volatile uint16_t USB_RXFIFOADD;                  /*!< Offset 0x096 USB_RXFIFOADD */
    struct
    {
        volatile uint16_t USB_TXFADDR;                /*!< Offset 0x098 USB_TXFADDR */
        volatile uint8_t  USB_TXHADDR;                /*!< Offset 0x09A USB_TXHADDR */
        volatile uint8_t  USB_TXHUBPORT;              /*!< Offset 0x09B USB_TXHUBPORT */
        volatile uint8_t  USB_RXFADDR;                /*!< Offset 0x09C USB_RXFADDR */
                 uint8_t reserved_0x005 [0x0001];
        volatile uint8_t  USB_RXHADDR;                /*!< Offset 0x09E USB_RXHADDR */
        volatile uint8_t  USB_RXHUBPORT;              /*!< Offset 0x09F USB_RXHUBPORT */
    } FIFO [0x010];                                   /*!< Offset 0x098 FIFOs [0..5] */
             uint32_t reserved_0x118 [0x00BA];
    volatile uint32_t USB_ISCR;                       /*!< Offset 0x400 HCI Interface Register (HCI_Interface) */
    volatile uint32_t USBPHY_PHYCTL;                  /*!< Offset 0x404 USBPHY_PHYCTL */
    volatile uint32_t HCI_CTRL3;                      /*!< Offset 0x408 HCI Control 3 Register (bist) */
             uint32_t reserved_0x40C;
    volatile uint32_t PHY_CTRL;                       /*!< Offset 0x410 PHY Control Register (PHY_Control) */
             uint32_t reserved_0x414 [0x0003];
    volatile uint32_t PHY_OTGCTL;                     /*!< Offset 0x420 Control PHY routing to EHCI or OTG */
    volatile uint32_t PHY_STATUS;                     /*!< Offset 0x424 PHY Status Register */
    volatile uint32_t USB_SPDCR;                      /*!< Offset 0x428 HCI SIE Port Disable Control Register */
             uint32_t reserved_0x42C [0x0035];
    volatile uint32_t USB_DMA_INTE;                   /*!< Offset 0x500 USB DMA Interrupt Enable Register */
    volatile uint32_t USB_DMA_INTS;                   /*!< Offset 0x504 USB DMA Interrupt Status Register */
             uint32_t reserved_0x508 [0x000E];
    struct
    {
        volatile uint32_t CHAN_CFG;                   /*!< Offset 0x540 USB DMA Channel Configuration Register */
        volatile uint32_t SDRAM_ADD;                  /*!< Offset 0x544 USB DMA Channel Configuration Register */
        volatile uint32_t BC;                         /*!< Offset 0x548 USB DMA Byte Counter Register/USB DMA RESIDUAL Byte Counter Register */
                 uint32_t reserved_0x00C;
    } USB_DMA [0x008];                                /*!< Offset 0x540  */
} USBOTG_TypeDef; /* size of structure = 0x5C0 */
/*
 * @brief USBPHYC
 */
/*!< USBPHYC  */
typedef struct USBPHYC_Type
{
    volatile uint32_t HCI_ICR;                        /*!< Offset 0x000 HCI Interface Control Register */
    volatile uint32_t HSIC_STATUS;                    /*!< Offset 0x004 HSIC status Register This register is valid on HCI1. */
} USBPHYC_TypeDef; /* size of structure = 0x008 */
/*
 * @brief USB_EHCI_Capability
 */
/*!< USB_EHCI_Capability  */
typedef struct USB_EHCI_Capability_Type
{
    volatile uint32_t HCCAPBASE;                      /*!< Offset 0x000 EHCI Capability Register (HCIVERSION and CAPLENGTH) register */
    volatile uint32_t HCSPARAMS;                      /*!< Offset 0x004 EHCI Host Control Structural Parameter Register */
    volatile uint32_t HCCPARAMS;                      /*!< Offset 0x008 EHCI Host Control Capability Parameter Register */
    volatile uint32_t HCSPPORTROUTE;                  /*!< Offset 0x00C EHCI Companion Port Route Description */
    volatile uint32_t USBCMD;                         /*!< Offset 0x010 EHCI USB Command Register */
    volatile uint32_t USBSTS;                         /*!< Offset 0x014 EHCI USB Status Register */
    volatile uint32_t USBINTR;                        /*!< Offset 0x018 EHCI USB Interrupt Enable Register */
    volatile uint32_t FRINDEX;                        /*!< Offset 0x01C EHCI USB Frame Index Register */
    volatile uint32_t CTRLDSSEGMENT;                  /*!< Offset 0x020 EHCI 4G Segment Selector Register */
    volatile uint32_t PERIODICLISTBASE;               /*!< Offset 0x024 EHCI Frame List Base Address Register */
    volatile uint32_t ASYNCLISTADDR;                  /*!< Offset 0x028 EHCI Next Asynchronous List Address Register */
} USB_EHCI_Capability_TypeDef; /* size of structure = 0x02C */
/*
 * @brief USB_OHCI_Capability
 */
/*!< USB_OHCI_Capability  */
typedef struct USB_OHCI_Capability_Type
{
    volatile uint32_t O_HcRevision;                   /*!< Offset 0x000 OHCI Revision Register (not documented) */
    volatile uint32_t O_HcControl;                    /*!< Offset 0x004 OHCI Control Register */
    volatile uint32_t O_HcCommandStatus;              /*!< Offset 0x008 OHCI Command Status Register */
    volatile uint32_t O_HcInterruptStatus;            /*!< Offset 0x00C OHCI Interrupt Status Register */
    volatile uint32_t O_HcInterruptEnable;            /*!< Offset 0x010 OHCI Interrupt Enable Register */
    volatile uint32_t O_HcInterruptDisable;           /*!< Offset 0x014 OHCI Interrupt Disable Register */
    volatile uint32_t O_HcHCCA;                       /*!< Offset 0x018 OHCI HCCA Base */
    volatile uint32_t O_HcPeriodCurrentED;            /*!< Offset 0x01C OHCI Period Current ED Base */
    volatile uint32_t O_HcControlHeadED;              /*!< Offset 0x020 OHCI Control Head ED Base */
    volatile uint32_t O_HcControlCurrentED;           /*!< Offset 0x024 OHCI Control Current ED Base */
    volatile uint32_t O_HcBulkHeadED;                 /*!< Offset 0x028 OHCI Bulk Head ED Base */
    volatile uint32_t O_HcBulkCurrentED;              /*!< Offset 0x02C OHCI Bulk Current ED Base */
    volatile uint32_t O_HcDoneHead;                   /*!< Offset 0x030 OHCI Done Head Base */
    volatile uint32_t O_HcFmInterval;                 /*!< Offset 0x034 OHCI Frame Interval Register */
    volatile uint32_t O_HcFmRemaining;                /*!< Offset 0x038 OHCI Frame Remaining Register */
    volatile uint32_t O_HcFmNumber;                   /*!< Offset 0x03C OHCI Frame Number Register */
    volatile uint32_t O_HcPerioddicStart;             /*!< Offset 0x040 OHCI Periodic Start Register */
    volatile uint32_t O_HcLSThreshold;                /*!< Offset 0x044 OHCI LS Threshold Register */
    volatile uint32_t O_HcRhDescriptorA;              /*!< Offset 0x048 OHCI Root Hub Descriptor Register A */
    volatile uint32_t O_HcRhDesriptorB;               /*!< Offset 0x04C OHCI Root Hub Descriptor Register B */
    volatile uint32_t O_HcRhStatus;                   /*!< Offset 0x050 OHCI Root Hub Status Register */
    volatile uint32_t O_HcRhPortStatus [0x001];       /*!< Offset 0x054 OHCI Root Hub Port Status Register */
} USB_OHCI_Capability_TypeDef; /* size of structure = 0x058 */


/* Access pointers */

#define GPU ((GPU_TypeDef *) GPU_BASE)                /*!< GPU G31 MP2 register set access pointer */
#define USBPHY0 ((USBPHYC_TypeDef *) USBPHY0_BASE)    /*!< USBPHY0  register set access pointer */
#define USBPHY1 ((USBPHYC_TypeDef *) USBPHY1_BASE)    /*!< USBPHY1  register set access pointer */
#define GPIOBLOCK_L ((GPIOBLOCK_TypeDef *) GPIOBLOCK_L_BASE)/*!< GPIOBLOCK_L  register set access pointer */
#define TWI0 ((TWI_TypeDef *) TWI0_BASE)              /*!< TWI0  register set access pointer */
#define TWI1 ((TWI_TypeDef *) TWI1_BASE)              /*!< TWI1  register set access pointer */
#define TWI2 ((TWI_TypeDef *) TWI2_BASE)              /*!< TWI2  register set access pointer */
#define TWI3 ((TWI_TypeDef *) TWI3_BASE)              /*!< TWI3  register set access pointer */
#define TWI4 ((TWI_TypeDef *) TWI4_BASE)              /*!< TWI4  register set access pointer */
#define GPIOA ((GPIO_TypeDef *) GPIOA_BASE)           /*!< GPIOA  register set access pointer */
#define GPIOBLOCK ((GPIOBLOCK_TypeDef *) GPIOBLOCK_BASE)/*!< GPIOBLOCK  register set access pointer */
#define GPIOE ((GPIO_TypeDef *) GPIOE_BASE)           /*!< GPIOE  register set access pointer */
#define GPIOF ((GPIO_TypeDef *) GPIOF_BASE)           /*!< GPIOF  register set access pointer */
#define GPIOG ((GPIO_TypeDef *) GPIOG_BASE)           /*!< GPIOG  register set access pointer */
#define GPIOH ((GPIO_TypeDef *) GPIOH_BASE)           /*!< GPIOH  register set access pointer */
#define GPIOI ((GPIO_TypeDef *) GPIOI_BASE)           /*!< GPIOI  register set access pointer */
#define GPIOL ((GPIO_TypeDef *) GPIOL_BASE)           /*!< GPIOL  register set access pointer */
#define GPIOB ((GPIO_TypeDef *) GPIOB_BASE)           /*!< GPIOB  register set access pointer */
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE)           /*!< GPIOC  register set access pointer */
#define GPIOD ((GPIO_TypeDef *) GPIOD_BASE)           /*!< GPIOD  register set access pointer */
#define GPIOINTA ((GPIOINT_TypeDef *) GPIOINTA_BASE)  /*!< GPIOINTA  register set access pointer */
#define GPIOINTC ((GPIOINT_TypeDef *) GPIOINTC_BASE)  /*!< GPIOINTC  register set access pointer */
#define GPIOINTD ((GPIOINT_TypeDef *) GPIOINTD_BASE)  /*!< GPIOINTD  register set access pointer */
#define GPIOINTE ((GPIOINT_TypeDef *) GPIOINTE_BASE)  /*!< GPIOINTE  register set access pointer */
#define GPIOINTF ((GPIOINT_TypeDef *) GPIOINTF_BASE)  /*!< GPIOINTF  register set access pointer */
#define GPIOINTG ((GPIOINT_TypeDef *) GPIOINTG_BASE)  /*!< GPIOINTG  register set access pointer */
#define GPIOINTH ((GPIOINT_TypeDef *) GPIOINTH_BASE)  /*!< GPIOINTH  register set access pointer */
#define GPIOINTI ((GPIOINT_TypeDef *) GPIOINTI_BASE)  /*!< GPIOINTI  register set access pointer */
#define GIC_DISTRIBUTOR ((_TypeDef *) GIC_DISTRIBUTOR_BASE)/*!< GIC_DISTRIBUTOR  register set access pointer */
#define GIC_INTERFACE ((_TypeDef *) GIC_INTERFACE_BASE)/*!< GIC_INTERFACE  register set access pointer */
#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0  register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1  register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2  register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3  register set access pointer */
#define UART4 ((UART_TypeDef *) UART4_BASE)           /*!< UART4  register set access pointer */
#define UART5 ((UART_TypeDef *) UART5_BASE)           /*!< UART5  register set access pointer */
#define USB0 ((USBOTG_TypeDef *) USB0_BASE)           /*!< USB0 USB OTG Dual-Role Device controller register set access pointer */
#define USBOTG0 ((USBOTG_TypeDef *) USBOTG0_BASE)     /*!< USBOTG0 USB OTG Dual-Role Device controller register set access pointer */
#define USB1 ((USB_EHCI_Capability_TypeDef *) USB1_BASE)/*!< USB1  register set access pointer */
#define USBOHCI1 ((USB_OHCI_Capability_TypeDef *) USBOHCI1_BASE)/*!< USBOHCI1  register set access pointer */
#define USB3 ((USB_EHCI_Capability_TypeDef *) USB3_BASE)/*!< USB3  register set access pointer */
#define USBOHCI3 ((USB_OHCI_Capability_TypeDef *) USBOHCI3_BASE)/*!< USBOHCI3  register set access pointer */
#define USB4 ((USB_EHCI_Capability_TypeDef *) USB4_BASE)/*!< USB4  register set access pointer */
#define USBOHCI4 ((USB_OHCI_Capability_TypeDef *) USBOHCI4_BASE)/*!< USBOHCI4  register set access pointer */


#endif /* HEADER_00003039_INCLUDED */
