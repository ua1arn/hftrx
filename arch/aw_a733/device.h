/* Copyright (c) 2024 by Genadi V. Zawidowski. All rights reserved. */
#pragma once
#ifndef HEADER__2A4CD94A_A755_4786_BB96_E52021B2959C__INCLUDED
#define HEADER__2A4CD94A_A755_4786_BB96_E52021B2959C__INCLUDED
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* IRQs */

typedef enum IRQn
{
    SGI0_IRQn = 0,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI1_IRQn = 1,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI2_IRQn = 2,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI3_IRQn = 3,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI4_IRQn = 4,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI5_IRQn = 5,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI6_IRQn = 6,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI7_IRQn = 7,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI8_IRQn = 8,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI9_IRQn = 9,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI10_IRQn = 10,                                  /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI11_IRQn = 11,                                  /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI12_IRQn = 12,                                  /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI13_IRQn = 13,                                  /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI14_IRQn = 14,                                  /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI15_IRQn = 15,                                  /*!< GIC_INTERFACE GICC GIC CPU Interface */
    PPI_22_IRQn = 22,                                 /*!< GIC_INTERFACE nCOMMIRQ[7:0] Interrupt-driven debug communications operation */
    PMU_IRQn = 23,                                    /*!< GIC_INTERFACE nPMUIRQ[7:0] PMU interrupt request */
    CTI_IRQn = 24,                                    /*!< GIC_INTERFACE CTIIRQ[7:0] CTI interrupt */
    VirtualMaintenanceInterrupt_IRQn = 25,            /*!< GIC_INTERFACE nVCPUMNTIRQ[7:0] Virtual CPU interface maintenance interrupt */
    HypervisorTimer_IRQn = 26,                        /*!< GIC_INTERFACE nCNTHPIRQ[7:0] Hypervisor physical timer event */
    VirtualTimer_IRQn = 27,                           /*!< GIC_INTERFACE nCNTVIRQ[7:0] Virtual timer event */
    HypervisorVirtualTimer_IRQn = 28,                 /*!< GIC_INTERFACE nCNTHVIRQ[7:0] Hypervisor virtual timer event */
    SecurePhysicalTimer_IRQn = 29,                    /*!< GIC_INTERFACE nCNTPSIRQ[7:0] Secure physical timer event */
    NonSecurePhysicalTimer_IRQn = 30,                 /*!< GIC_INTERFACE nCNTPNSIRQ[7:0] Nonsecure physical timer event */
    UART0_IRQn = 34,                                  /*!< UART  */
    UART1_IRQn = 35,                                  /*!< UART  */
    UART2_IRQn = 36,                                  /*!< UART  */
    UART3_IRQn = 37,                                  /*!< UART  */
    UART4_IRQn = 38,                                  /*!< UART  */
    UART5_IRQn = 39,                                  /*!< UART  */
    UART6_IRQn = 40,                                  /*!< UART  */
    TWI0_IRQn = 43,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI1_IRQn = 44,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI2_IRQn = 45,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI3_IRQn = 46,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI4_IRQn = 47,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI5_IRQn = 48,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI6_IRQn = 49,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI7_IRQn = 50,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI8_IRQn = 51,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI9_IRQn = 52,                                   /*!< TWI Two Wire Interface (TWI) */
    TWI10_IRQn = 53,                                  /*!< TWI Two Wire Interface (TWI) */
    TWI11_IRQn = 54,                                  /*!< TWI Two Wire Interface (TWI) */
    TWI12_IRQn = 55,                                  /*!< TWI Two Wire Interface (TWI) */
    SPI0_IRQn = 56,                                   /*!< SPI Serial Peripheral Interface */
    SPI1_IRQn = 57,                                   /*!< SPI Serial Peripheral Interface */
    SPI2_IRQn = 58,                                   /*!< SPI Serial Peripheral Interface */
    SPI3_IRQn = 59,                                   /*!< SPI Serial Peripheral Interface */
    I2S_PCM4_IRQn = 61,                               /*!< I2S_PCM  */
    I2S_PCM0_IRQn = 74,                               /*!< I2S_PCM  */
    I2S_PCM1_IRQn = 75,                               /*!< I2S_PCM  */
    I2S_PCM2_IRQn = 76,                               /*!< I2S_PCM  */
    I2S_PCM3_IRQn = 77,                               /*!< I2S_PCM  */
    DE_IRQn = 82,                                     /*!< DE_TOP DE interrupt */
    G2D_IRQn = 83,                                    /*!< G2D_TOP Graphic 2D top */
    TCON0_LCD0_IRQn = 86,                             /*!< TCON_LCD Timing Controller_LCD (TCON_LCD) */
    TCON0_LCD1_IRQn = 87,                             /*!< TCON_LCD Timing Controller_LCD (TCON_LCD) */
    TCON_TV0_IRQn = 89,                               /*!< TCON_TV TV Output */
    TCON_TV1_IRQn = 90,                               /*!< TCON_TV TV Output */
    DSI0_IRQn = 91,                                   /*!< MIPI_DSI  */
    DSI1_IRQn = 92,                                   /*!< MIPI_DSI  */
    HDMI_IRQn = 93,                                   /*!< HDMI_TX  */
    EDP_IRQn = 94,                                    /*!< eDP_REG_PAD  */
    GPIOB_IRQn = 101,                                 /*!< GPIOINT GPIOB_NS */
    GPIOB_S_IRQn = 102,                               /*!< GPIOINT  */
    GPIOC_IRQn = 103,                                 /*!< GPIOINT GPIOC_NS */
    GPIOC_S_IRQn = 104,                               /*!< GPIOINT  */
    GPIOD_IRQn = 105,                                 /*!< GPIOINT GPIOD_NS */
    GPIOD_S_IRQn = 106,                               /*!< GPIOINT  */
    GPIOE_IRQn = 107,                                 /*!< GPIOINT GPIOE_NS */
    GPIOE_S_IRQn = 108,                               /*!< GPIOINT  */
    GPIOF_IRQn = 109,                                 /*!< GPIOINT GPIOF_NS */
    GPIOF_S_IRQn = 110,                               /*!< GPIOINT  */
    GPIOG_IRQn = 111,                                 /*!< GPIOINT GPIOG_NS */
    GPIOG_S_IRQn = 112,                               /*!< GPIOINT  */
    GPIOH_IRQn = 113,                                 /*!< GPIOINT GPIOH_NS */
    GPIOH_S_IRQn = 114,                               /*!< GPIOINT  */
    GPIOJ_IRQn = 117,                                 /*!< GPIOINT GPIOJ_NS */
    GPIOJ_S_IRQn = 118,                               /*!< GPIOINT  */
    GPIOK_IRQn = 119,                                 /*!< GPIOINT GPIOK_NS */
    GPIOK_S_IRQn = 120,                               /*!< GPIOINT  */
    TIMER0_IRQn = 121,                                /*!< TIMER0  */
    TIMER1_IRQn = 122,                                /*!< TIMER0  */
    TIMER2_IRQn = 123,                                /*!< TIMER0  */
    TIMER3_IRQn = 124,                                /*!< TIMER0  */
    TIMER4_IRQn = 125,                                /*!< TIMER0  */
    TIMER5_IRQn = 126,                                /*!< TIMER0  */
    TIMER6_IRQn = 127,                                /*!< TIMER0  */
    TIMER7_IRQn = 128,                                /*!< TIMER0  */
    TIMER8_IRQn = 129,                                /*!< TIMER0  */
    TIMER9_IRQn = 130,                                /*!< TIMER0  */
    TIMER1_0_IRQn = 131,                              /*!< TIMER1  */
    TIMER1_1_IRQn = 132,                              /*!< TIMER1  */
    TIMER1_2_IRQn = 133,                              /*!< TIMER1  */
    TIMER1_3_IRQn = 134,                              /*!< TIMER1  */
    DMA0_CPUX_NS_IRQn = 143,                          /*!< DMAC  */
    DMA0_CPUX_S_IRQn = 144,                           /*!< DMAC  */
    DMA1_CPUX_NS_IRQn = 145,                          /*!< DMAC  */
    DMA1_CPUX_S_IRQn = 146,                           /*!< DMAC  */
    USB2_IRQn = 187,                                  /*!< USB3P1_DRD  */
    USB0_DEVICE_IRQn = 188,                           /*!< USBOTG USB OTG Dual-Role Device controller */
    USB0_EHCI_IRQn = 189,                             /*!< USB_EHCI_Capability  */
    USB0_OHCI_IRQn = 190,                             /*!< USB_OHCI_Capability  */
    USB1_EHCI_IRQn = 191,                             /*!< USB_EHCI_Capability  */
    USB1_OHCI_IRQn = 192,                             /*!< USB_OHCI_Capability  */
    SMHC0_IRQn = 193,                                 /*!< SMHC SD-MMC Host Controller */
    SMHC1_IRQn = 194,                                 /*!< SMHC SD-MMC Host Controller */
    SMHC2_IRQn = 195,                                 /*!< SMHC SD-MMC Host Controller */
    SMHC3_IRQn = 196,                                 /*!< SMHC SD-MMC Host Controller */
    RTC_ALARM_IRQn = 228,                             /*!< RTC Real Time Clock (RTC) */
    S_GPIOL_S_IRQn = 229,                             /*!< GPIOINT  */
    S_GPIOL_IRQn = 230,                               /*!< GPIOINT S_GPIOL_NS */
    S_GPIOM_S_IRQn = 231,                             /*!< GPIOINT  */
    S_GPIOM_IRQn = 232,                               /*!< GPIOINT S_GPIOM_NS */
    S_UART0_IRQn = 233,                               /*!< UART  */
    S_UART1_IRQn = 234,                               /*!< UART  */
    S_TWI0_IRQn = 235,                                /*!< TWI Two Wire Interface (TWI) */
    S_TWI1_IRQn = 236,                                /*!< TWI Two Wire Interface (TWI) */
    S_TWI2_IRQn = 237,                                /*!< TWI Two Wire Interface (TWI) */
    S_SPI_IRQn = 242,                                 /*!< SPI Serial Peripheral Interface */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define GPIOINTD_BASE ((uintptr_t) 0x00020002)        /*!< GPIOINT  Base */
#define DE_DNS1_BASE ((uintptr_t) 0x01114000)         /*!< DE_DNS Denoise (DNS) Base */
#define DE_DNS2_BASE ((uintptr_t) 0x01134000)         /*!< DE_DNS Denoise (DNS) Base */
#define DE_DNS3_BASE ((uintptr_t) 0x01151400)         /*!< DE_DNS Denoise (DNS) Base */
#define DE_DNS4_BASE ((uintptr_t) 0x011D4000)         /*!< DE_DNS Denoise (DNS) Base */
#define DE_DNS5_BASE ((uintptr_t) 0x011F4000)         /*!< DE_DNS Denoise (DNS) Base */
#define DE_DNS6_BASE ((uintptr_t) 0x01214000)         /*!< DE_DNS Denoise (DNS) Base */
#define GPIOBLOCK_BASE ((uintptr_t) 0x02000080)       /*!< GPIOBLOCK Port Controller Base */
#define GPIOB_BASE ((uintptr_t) 0x02000100)           /*!< GPIO Port Controller Base */
#define GPIOINTB_BASE ((uintptr_t) 0x02000140)        /*!< GPIOINT  Base */
#define GPIOC_BASE ((uintptr_t) 0x02000180)           /*!< GPIO Port Controller Base */
#define GPIOINTC_BASE ((uintptr_t) 0x020001C0)        /*!< GPIOINT  Base */
#define GPIOD_BASE ((uintptr_t) 0x02000200)           /*!< GPIO Port Controller Base */
#define GPIOE_BASE ((uintptr_t) 0x02000280)           /*!< GPIO Port Controller Base */
#define GPIOINTE_BASE ((uintptr_t) 0x020002C0)        /*!< GPIOINT  Base */
#define GPIOF_BASE ((uintptr_t) 0x02000300)           /*!< GPIO Port Controller Base */
#define GPIOINTF_BASE ((uintptr_t) 0x02000340)        /*!< GPIOINT  Base */
#define GPIOG_BASE ((uintptr_t) 0x02000380)           /*!< GPIO Port Controller Base */
#define GPIOINTG_BASE ((uintptr_t) 0x020003C0)        /*!< GPIOINT  Base */
#define GPIOH_BASE ((uintptr_t) 0x02000400)           /*!< GPIO Port Controller Base */
#define GPIOINTH_BASE ((uintptr_t) 0x02000440)        /*!< GPIOINT  Base */
#define GPIOJ_BASE ((uintptr_t) 0x02000500)           /*!< GPIO Port Controller Base */
#define GPIOINTJ_BASE ((uintptr_t) 0x02000540)        /*!< GPIOINT  Base */
#define GPIOK_BASE ((uintptr_t) 0x02000580)           /*!< GPIO Port Controller Base */
#define GPIOINTK_BASE ((uintptr_t) 0x020005C0)        /*!< GPIOINT  Base */
#define CCU_BASE ((uintptr_t) 0x02002000)             /*!< CCU Clock Controller Unit (CCU) Base */
#define I2S0_BASE ((uintptr_t) 0x02033000)            /*!< I2S_PCM  Base */
#define I2S1_BASE ((uintptr_t) 0x02033000)            /*!< I2S_PCM  Base */
#define I2S2_BASE ((uintptr_t) 0x02034000)            /*!< I2S_PCM  Base */
#define I2S3_BASE ((uintptr_t) 0x02034000)            /*!< I2S_PCM  Base */
#define I2S4_BASE ((uintptr_t) 0x02034000)            /*!< I2S_PCM  Base */
#define Timer1_CPUX_BASE ((uintptr_t) 0x02052000)     /*!< TIMER1  Base */
#define CPUS_INTERRUPT_CTRL_BASE ((uintptr_t) 0x02055000)/*!< CPUS_INTERRUPT_CTRL  Base */
#define RV_INTERRUPT_CTRL_BASE ((uintptr_t) 0x02056000)/*!< RV_INTERRUPT_CTRL  Base */
#define UART0_BASE ((uintptr_t) 0x02500000)           /*!< UART  Base */
#define UART1_BASE ((uintptr_t) 0x02501000)           /*!< UART  Base */
#define UART2_BASE ((uintptr_t) 0x02502000)           /*!< UART  Base */
#define UART3_BASE ((uintptr_t) 0x02503000)           /*!< UART  Base */
#define UART4_BASE ((uintptr_t) 0x02504000)           /*!< UART  Base */
#define UART5_BASE ((uintptr_t) 0x02505000)           /*!< UART  Base */
#define UART6_BASE ((uintptr_t) 0x02506000)           /*!< UART  Base */
#define TWI0_BASE ((uintptr_t) 0x02510000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI1_BASE ((uintptr_t) 0x02511000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI2_BASE ((uintptr_t) 0x02512000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI3_BASE ((uintptr_t) 0x02513000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI4_BASE ((uintptr_t) 0x02514000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI5_BASE ((uintptr_t) 0x02515000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI6_BASE ((uintptr_t) 0x02516000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI7_BASE ((uintptr_t) 0x02517000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI8_BASE ((uintptr_t) 0x02518000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI9_BASE ((uintptr_t) 0x02519000)            /*!< TWI Two Wire Interface (TWI) Base */
#define TWI10_BASE ((uintptr_t) 0x0251A000)           /*!< TWI Two Wire Interface (TWI) Base */
#define TWI11_BASE ((uintptr_t) 0x0251B000)           /*!< TWI Two Wire Interface (TWI) Base */
#define TWI12_BASE ((uintptr_t) 0x0251C000)           /*!< TWI Two Wire Interface (TWI) Base */
#define SPI0_BASE ((uintptr_t) 0x02540000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI1_BASE ((uintptr_t) 0x02541000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI2_BASE ((uintptr_t) 0x02542000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI3_BASE ((uintptr_t) 0x02543000)            /*!< SPI Serial Peripheral Interface Base */
#define SID_BASE ((uintptr_t) 0x03006000)             /*!< SID Security ID Base */
#define Timer0_CPUX_BASE ((uintptr_t) 0x03009000)     /*!< TIMER0  Base */
#define GIC_BASE ((uintptr_t) 0x03400000)             /*!< GIC  Base */
#define GICD_BASE ((uintptr_t) 0x03400000)            /*!< GICD GIC Distributor Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0x03400000) /*!< GIC_DISTRIBUTOR GICD Base */
#define GICT_BASE ((uintptr_t) 0x03420000)            /*!< GICT GIC ITS translation Base */
#define GICP_BASE ((uintptr_t) 0x03430000)            /*!< GICP GIC Performance Monitoring Unit Base */
#define GITS_BASE ((uintptr_t) 0x03440000)            /*!< GITS GIC Secure Access Control Base */
#define GICR0_BASE ((uintptr_t) 0x03460000)           /*!< GICR GIC Redistributor  Base */
#define GIC_REDISTRIBUTOR_BASE ((uintptr_t) 0x03460000)/*!< GIC_REDISTRIBUTOR GICD GIC Distributor Base */
#define SMHC0_BASE ((uintptr_t) 0x04020000)           /*!< SMHC SD-MMC Host Controller Base */
#define SMHC1_BASE ((uintptr_t) 0x04021000)           /*!< SMHC SD-MMC Host Controller Base */
#define SMHC2_BASE ((uintptr_t) 0x04022000)           /*!< SMHC SD-MMC Host Controller Base */
#define SMHC3_BASE ((uintptr_t) 0x04023000)           /*!< SMHC SD-MMC Host Controller Base */
#define DMA1_BASE ((uintptr_t) 0x04024000)            /*!< DMAC  Base */
#define USB0_DEVICE_BASE ((uintptr_t) 0x04100000)     /*!< USBOTG USB OTG Dual-Role Device controller Base */
#define USB0_EHCI_BASE ((uintptr_t) 0x04101000)       /*!< USB_EHCI_Capability  Base */
#define USB1_EHCI_BASE ((uintptr_t) 0x04201000)       /*!< USB_EHCI_Capability  Base */
#define USB1_OHCI_BASE ((uintptr_t) 0x04201400)       /*!< USB_OHCI_Capability  Base */
#define DMA0_BASE ((uintptr_t) 0x04601000)            /*!< DMAC  Base */
#define DE_BASE ((uintptr_t) 0x05000000)              /*!< DE Display Engine (DE) Base */
#define DE_TOP_BASE ((uintptr_t) 0x05008000)          /*!< DE_TOP Display Engine (DE) TOP (APB) Base */
#define DE_MIXER0_GLB_BASE ((uintptr_t) 0x05008100)   /*!< DE_GLB Display Engine (DE) - Global Control Base */
#define DE_MIXER1_GLB_BASE ((uintptr_t) 0x05008140)   /*!< DE_GLB Display Engine (DE) - Global Control Base */
#define RTWB_RCQ_BASE ((uintptr_t) 0x05008200)        /*!< RTWB_RCQ  Base */
#define DE_VI1_BASE ((uintptr_t) 0x05101000)          /*!< DE_VI Display Engine (DE) - VI surface Base */
#define USB0_OHCI_BASE ((uintptr_t) 0x05101400)       /*!< USB_OHCI_Capability  Base */
#define DE_VSU1_BASE ((uintptr_t) 0x05104000)         /*!< DE_VSU Video Scaler Unit (VSU) Base */
#define DE_FCE1_BASE ((uintptr_t) 0x05110000)         /*!< DE_FCE Fresh and Contrast Enhancement (FCE) Base */
#define DE_BLS1_BASE ((uintptr_t) 0x05111000)         /*!< DE_BLS Blue Level Stretch (BLS) Base */
#define DE_FCC1_BASE ((uintptr_t) 0x05111400)         /*!< DE_FCC Fancy color curvature (FCC) Base */
#define DE_VI2_BASE ((uintptr_t) 0x05121000)          /*!< DE_VI Display Engine (DE) - VI surface Base */
#define DE_VSU2_BASE ((uintptr_t) 0x05124000)         /*!< DE_VSU Video Scaler Unit (VSU) Base */
#define DE_FCE2_BASE ((uintptr_t) 0x05130000)         /*!< DE_FCE Fresh and Contrast Enhancement (FCE) Base */
#define DE_BLS2_BASE ((uintptr_t) 0x05131000)         /*!< DE_BLS Blue Level Stretch (BLS) Base */
#define DE_FCC2_BASE ((uintptr_t) 0x05131400)         /*!< DE_FCC Fancy color curvature (FCC) Base */
#define DE_VI3_BASE ((uintptr_t) 0x05141000)          /*!< DE_VI Display Engine (DE) - VI surface Base */
#define DE_VSU3_BASE ((uintptr_t) 0x05144000)         /*!< DE_VSU Video Scaler Unit (VSU) Base */
#define DE_FCE3_BASE ((uintptr_t) 0x05150000)         /*!< DE_FCE Fresh and Contrast Enhancement (FCE) Base */
#define DE_BLS3_BASE ((uintptr_t) 0x05151000)         /*!< DE_BLS Blue Level Stretch (BLS) Base */
#define DE_FCC3_BASE ((uintptr_t) 0x05151400)         /*!< DE_FCC Fancy color curvature (FCC) Base */
#define DE_UI1_BASE ((uintptr_t) 0x051C1000)          /*!< DE_UI Display Engine (DE) - UI surface Base */
#define DE_UIS1_BASE ((uintptr_t) 0x051C4000)         /*!< DE_UIS UI Scaler(UIS) provides RGB format image resizing function Base */
#define DE_FCE4_BASE ((uintptr_t) 0x051D0000)         /*!< DE_FCE Fresh and Contrast Enhancement (FCE) Base */
#define DE_BLS4_BASE ((uintptr_t) 0x051D1000)         /*!< DE_BLS Blue Level Stretch (BLS) Base */
#define DE_FCC4_BASE ((uintptr_t) 0x051D1400)         /*!< DE_FCC Fancy color curvature (FCC) Base */
#define DE_UI2_BASE ((uintptr_t) 0x051E1000)          /*!< DE_UI Display Engine (DE) - UI surface Base */
#define DE_UIS2_BASE ((uintptr_t) 0x051E4000)         /*!< DE_UIS UI Scaler(UIS) provides RGB format image resizing function Base */
#define DE_FCE5_BASE ((uintptr_t) 0x051F0000)         /*!< DE_FCE Fresh and Contrast Enhancement (FCE) Base */
#define DE_BLS5_BASE ((uintptr_t) 0x051F1000)         /*!< DE_BLS Blue Level Stretch (BLS) Base */
#define DE_FCC5_BASE ((uintptr_t) 0x051F1400)         /*!< DE_FCC Fancy color curvature (FCC) Base */
#define DE_UI3_BASE ((uintptr_t) 0x05201000)          /*!< DE_UI Display Engine (DE) - UI surface Base */
#define DE_UIS3_BASE ((uintptr_t) 0x05204000)         /*!< DE_UIS UI Scaler(UIS) provides RGB format image resizing function Base */
#define DE_FCE6_BASE ((uintptr_t) 0x05210000)         /*!< DE_FCE Fresh and Contrast Enhancement (FCE) Base */
#define DE_BLS6_BASE ((uintptr_t) 0x05211000)         /*!< DE_BLS Blue Level Stretch (BLS) Base */
#define DE_FCC6_BASE ((uintptr_t) 0x05211400)         /*!< DE_FCC Fancy color curvature (FCC) Base */
#define DE_MIXER0_BLD_BASE ((uintptr_t) 0x05281000)   /*!< DE_BLD Display Engine (DE) - Blender Base */
#define DE_MIXER1_BLD_BASE ((uintptr_t) 0x052A1000)   /*!< DE_BLD Display Engine (DE) - Blender Base */
#define G2D_BASE ((uintptr_t) 0x05430000)             /*!< G2D_TOP Graphic 2D top Base */
#define G2D_TOP_BASE ((uintptr_t) 0x05430000)         /*!< G2D_TOP Graphic 2D top Base */
#define G2D_MIXER_BASE ((uintptr_t) 0x05430100)       /*!< G2D_MIXER Graphic 2D (G2D) Engine Video Mixer Base */
#define G2D_BLD_BASE ((uintptr_t) 0x05430400)         /*!< G2D_BLD Graphic 2D (G2D) Engine Blender Base */
#define G2D_V0_BASE ((uintptr_t) 0x05430800)          /*!< G2D_VI Graphic 2D VI surface Base */
#define G2D_UI0_BASE ((uintptr_t) 0x05431000)         /*!< G2D_UI Graphic 2D UI surface Base */
#define G2D_UI1_BASE ((uintptr_t) 0x05431800)         /*!< G2D_UI Graphic 2D UI surface Base */
#define G2D_UI2_BASE ((uintptr_t) 0x05432000)         /*!< G2D_UI Graphic 2D UI surface Base */
#define G2D_WB_BASE ((uintptr_t) 0x05433000)          /*!< G2D_WB Graphic 2D (G2D) Engine Write Back Base */
#define G2D_VSU_BASE ((uintptr_t) 0x05438000)         /*!< G2D_VSU Graphic 2D Video Scaler Base */
#define DISPLAY0_TOP_BASE ((uintptr_t) 0x05500000)    /*!< DISPLAY0_TOP VIDE0_OUT1_SYS Base */
#define TCON_LCD0_BASE ((uintptr_t) 0x05501000)       /*!< TCON_LCD Timing Controller_LCD (TCON_LCD) Base */
#define TCON_LCD1_BASE ((uintptr_t) 0x05502000)       /*!< TCON_LCD Timing Controller_LCD (TCON_LCD) Base */
#define COMBO_PHY_TX_DSI0_BASE ((uintptr_t) 0x05506000)/*!< MIPI_DSI  Base */
#define COMBO_PHY_TX_DSI1_BASE ((uintptr_t) 0x05508000)/*!< MIPI_DSI  Base */
#define DISPLAY1_TOP_BASE ((uintptr_t) 0x05510000)    /*!< DISPLAY1_TOP VIDEO_OUT1_SYS Base */
#define HDMI_TX0_BASE ((uintptr_t) 0x05520000)        /*!< HDMI_TX  Base */
#define TCON_TV0_BASE ((uintptr_t) 0x05730000)        /*!< TCON_TV TV Output Base */
#define TCON_TV1_BASE ((uintptr_t) 0x05731000)        /*!< TCON_TV TV Output Base */
#define eDP_CTRL_BASE ((uintptr_t) 0x05740000)        /*!< eDP_CTRL  Base */
#define eDP_REG_PAD_BASE ((uintptr_t) 0x05760000)     /*!< eDP_REG_PAD  Base */
#define USB3P1_DRD_BASE ((uintptr_t) 0x06A00000)      /*!< USB3P1_DRD  Base */
#define STBY_PRCM_BASE ((uintptr_t) 0x07010000)       /*!< PRCM  Base */
#define S_GPIOL_BASE ((uintptr_t) 0x07025000)         /*!< S_GPIO Secure Port Controller Base */
#define S_GPIOM_BASE ((uintptr_t) 0x07025030)         /*!< S_GPIO Secure Port Controller Base */
#define S_UART0_BASE ((uintptr_t) 0x07080000)         /*!< UART  Base */
#define S_UART1_BASE ((uintptr_t) 0x07081000)         /*!< UART  Base */
#define S_TWI0_BASE ((uintptr_t) 0x07083000)          /*!< TWI Two Wire Interface (TWI) Base */
#define S_TWI1_BASE ((uintptr_t) 0x07084000)          /*!< TWI Two Wire Interface (TWI) Base */
#define S_TWI2_BASE ((uintptr_t) 0x07085000)          /*!< TWI Two Wire Interface (TWI) Base */
#define RTC_BASE ((uintptr_t) 0x07090000)             /*!< RTC Real Time Clock (RTC) Base */
#define Timer0_CPUS_BASE ((uintptr_t) 0x07091000)     /*!< TIMER0  Base */
#define S_SPI_BASE ((uintptr_t) 0x07092000)           /*!< SPI Serial Peripheral Interface Base */
#define CPU_SUBSYS_CTRL_BASE ((uintptr_t) 0x08000000) /*!< CPU_SUBSYS_CTRL  Base */
#define TIMESTAMP_STA_BASE ((uintptr_t) 0x08010000)   /*!< TIMESTAMP_STA  Base */
#define TIMESTAMP_CTRL_BASE ((uintptr_t) 0x08020000)  /*!< TIMESTAMP_CTRL  Base */
#define CLUSTER_CFG_BASE ((uintptr_t) 0x08860000)     /*!< CLUSTER_CFG  Base */
#define CPU_PLL_CFG_BASE ((uintptr_t) 0x08870000)     /*!< CPU_PLL_CFG  Base */

#if defined(__aarch64__)
    #include <core64_ca.h>
#else
    #include <core_ca.h>
#endif

/*
 * @brief CCU
 */
/*!< CCU Clock Controller Unit (CCU) */
typedef struct CCU_Type
{
         RESERVED(0x000[0x0020 - 0x0000], uint8_t)
    __IOM uint32_t PLL_DDR_CTRL_REG;                  /*!< Offset 0x020 PLL_DDR Control Register  */
    __IOM uint32_t PLL_DDR_LOCK_CTRL_REG;             /*!< Offset 0x024 PLL_DDR Lock Control Register  */
    __IOM uint32_t PLL_DDR_PAT0_CTRL_REG;             /*!< Offset 0x028 PLL_DDR Pattern0 Control Register  */
    __IOM uint32_t PLL_DDR_PAT1_CTRL_REG;             /*!< Offset 0x02C PLL_DDR Pattern1 Control Register  */
    __IOM uint32_t PLL_DDR_BIAS_REG;                  /*!< Offset 0x030 PLL_DDR Bias Register  */
         RESERVED(0x034[0x00A0 - 0x0034], uint8_t)
    __IOM uint32_t PLL_PERI0_CTRL_REG;                /*!< Offset 0x0A0 PLL_PERI0 Control Register  */
    __IOM uint32_t PLL_PERI0_LOCK_CTRL_REG;           /*!< Offset 0x0A4 PLL_PERI0 Lock Control Register  */
    __IOM uint32_t PLL_PERI0_PAT0_CTRL_REG;           /*!< Offset 0x0A8 PLL_PERI0 Pattern0 Control Register  */
    __IOM uint32_t PLL_PERI0_PAT1_CTRL_REG;           /*!< Offset 0x0AC PLL_PERI0 Pattern1 Control Register  */
    __IOM uint32_t PLL_PERI0_BIAS_REG;                /*!< Offset 0x0B0 PLL_PERI0 Bias Register  */
         RESERVED(0x0B4[0x00C0 - 0x00B4], uint8_t)
    __IOM uint32_t PLL_PERI1_CTRL_REG;                /*!< Offset 0x0C0 PLL_PERI1 Control Register  */
    __IOM uint32_t PLL_PERI1_LOCK_CTRL_REG;           /*!< Offset 0x0C4 PLL_PERI1 Lock Control Register  */
    __IOM uint32_t PLL_PERI1_PAT0_CTRL_REG;           /*!< Offset 0x0C8 PLL_PERI1 Pattern0 Control Register  */
    __IOM uint32_t PLL_PERI1_PAT1_CTRL_REG;           /*!< Offset 0x0CC PLL_PERI1 Pattern1 Control Register  */
    __IOM uint32_t PLL_PERI1_BIAS_REG;                /*!< Offset 0x0D0 PLL_PERI1 Bias Register  */
         RESERVED(0x0D4[0x00E0 - 0x00D4], uint8_t)
    __IOM uint32_t PLL_GPU0_CTRL_REG;                 /*!< Offset 0x0E0 PLL_GPU0 Control Register  */
    __IOM uint32_t PLL_GPU0_LOCK_CTRL_REG;            /*!< Offset 0x0E4 PLL_GPU0 Lock Control Register  */
    __IOM uint32_t PLL_GPU0_PAT0_CTRL_REG;            /*!< Offset 0x0E8 PLL_GPU0 Pattern0 Control Register  */
    __IOM uint32_t PLL_GPU0_PAT1_CTRL_REG;            /*!< Offset 0x0EC PLL_GPU0 Pattern1 Control Register  */
    __IOM uint32_t PLL_GPU0_BIAS_REG;                 /*!< Offset 0x0F0 PLL_GPU0 Bias Register  */
         RESERVED(0x0F4[0x0120 - 0x00F4], uint8_t)
    __IOM uint32_t PLL_VIDEO0_CTRL_REG;               /*!< Offset 0x120 PLL_VIDEO0 Control Register  */
    __IOM uint32_t PLL_VIDEO0_LOCK_CTRL_REG;          /*!< Offset 0x124 PLL_VIDEO0 Lock Control Register  */
    __IOM uint32_t PLL_VIDEO0_PAT0_CTRL_REG;          /*!< Offset 0x128 PLL_VIDEO0 Pattern0 Control Register  */
    __IOM uint32_t PLL_VIDEO0_PAT1_CTRL_REG;          /*!< Offset 0x12C PLL_VIDEO0 Pattern1 Control Register  */
    __IOM uint32_t PLL_VIDEO0_BIAS_REG;               /*!< Offset 0x130 PLL_VIDEO0 Bias Register  */
         RESERVED(0x134[0x0140 - 0x0134], uint8_t)
    __IOM uint32_t PLL_VIDEO1_CTRL_REG;               /*!< Offset 0x140 PLL_VIDEO1 Control Register  */
    __IOM uint32_t PLL_VIDEO1_LOCK_CTRL_REG;          /*!< Offset 0x144 PLL_VIDEO1 Lock Control Register  */
    __IOM uint32_t PLL_VIDEO1_PAT0_CTRL_REG;          /*!< Offset 0x148 PLL_VIDEO1 Pattern0 Control Register  */
    __IOM uint32_t PLL_VIDEO1_PAT1_CTRL_REG;          /*!< Offset 0x14C PLL_VIDEO1 Pattern1 Control Register  */
    __IOM uint32_t PLL_VIDEO1_BIAS_REG;               /*!< Offset 0x150 PLL_VIDEO1 Bias Register  */
         RESERVED(0x154[0x0160 - 0x0154], uint8_t)
    __IOM uint32_t PLL_VIDEO2_CTRL_REG;               /*!< Offset 0x160 PLL_VIDEO2 Control Register  */
    __IOM uint32_t PLL_VIDEO2_LOCK_CTRL_REG;          /*!< Offset 0x164 PLL_VIDEO2 Lock Control Register  */
    __IOM uint32_t PLL_VIDEO2_PAT0_CTRL_REG;          /*!< Offset 0x168 PLL_VIDEO2 Pattern0 Control Register  */
    __IOM uint32_t PLL_VIDEO2_PAT1_CTRL_REG;          /*!< Offset 0x16C PLL_VIDEO2 Pattern1 Control Register  */
    __IOM uint32_t PLL_VIDEO2_BIAS_REG;               /*!< Offset 0x170 PLL_VIDEO2 Bias Register  */
         RESERVED(0x174[0x0220 - 0x0174], uint8_t)
    __IOM uint32_t PLL_VE0_CTRL_REG;                  /*!< Offset 0x220 PLL_VE0 Control Register  */
    __IOM uint32_t PLL_VE0_LOCK_CTRL_REG;             /*!< Offset 0x224 PLL_VE0 Lock Control Register  */
    __IOM uint32_t PLL_VE0_PAT0_CTRL_REG;             /*!< Offset 0x228 PLL_VE0 Pattern0 Control Register  */
    __IOM uint32_t PLL_VE0_PAT1_CTRL_REG;             /*!< Offset 0x22C PLL_VE0 Pattern1 Control Register  */
    __IOM uint32_t PLL_VE0_BIAS_REG;                  /*!< Offset 0x230 PLL_VE0 Bias Register  */
         RESERVED(0x234[0x0240 - 0x0234], uint8_t)
    __IOM uint32_t PLL_VE1_CTRL_REG;                  /*!< Offset 0x240 PLL_VE1 Control Register  */
    __IOM uint32_t PLL_VE1_LOCK_CTRL_REG;             /*!< Offset 0x244 PLL_VE1 Lock Control Register  */
    __IOM uint32_t PLL_VE1_PAT0_CTRL_REG;             /*!< Offset 0x248 PLL_VE1 Pattern0 Control Register  */
    __IOM uint32_t PLL_VE1_PAT1_CTRL_REG;             /*!< Offset 0x24C PLL_VE1 Pattern1 Control Register  */
    __IOM uint32_t PLL_VE1_BIAS_REG;                  /*!< Offset 0x250 PLL_VE1 Bias Register  */
         RESERVED(0x254[0x0260 - 0x0254], uint8_t)
    __IOM uint32_t PLL_AUDIO0_CTRL_REG;               /*!< Offset 0x260 PLL_AUDIO0 Control Register  */
    __IOM uint32_t PLL_AUDIO0_LOCK_CTRL_REG;          /*!< Offset 0x264 PLL_AUDIO0 Lock Control Register  */
    __IOM uint32_t PLL_AUDIO0_PAT0_CTRL_REG;          /*!< Offset 0x268 PLL_AUDIO0 Pattern0 Control Register  */
    __IOM uint32_t PLL_AUDIO0_PAT1_CTRL_REG;          /*!< Offset 0x26C PLL_AUDIO0 Pattern1 Control Register  */
    __IOM uint32_t PLL_AUDIO0_BIAS_REG;               /*!< Offset 0x270 PLL_AUDIO0 Bias Register  */
         RESERVED(0x274[0x0280 - 0x0274], uint8_t)
    __IOM uint32_t PLL_AUDIO1_CTRL_REG;               /*!< Offset 0x280 PLL_AUDIO1 Control Register */
    __IOM uint32_t PLL_AUDIO1_LOCK_CTRL_REG;          /*!< Offset 0x284 PLL_AUDIO1 Lock Control Register */
    __IOM uint32_t PLL_AUDIO1_PAT0_CTRL_REG;          /*!< Offset 0x288 PLL_AUDIO1 Pattern0 Control Register */
    __IOM uint32_t PLL_AUDIO1_PAT1_CTRL_REG;          /*!< Offset 0x28C PLL_AUDIO1 Pattern1 Control Register  */
    __IOM uint32_t PLL_AUDIO1_BIAS_REG;               /*!< Offset 0x290 PLL_AUDIO1 Bias Register */
         RESERVED(0x294[0x02A0 - 0x0294], uint8_t)
    __IOM uint32_t PLL_NPU_CTRL_REG;                  /*!< Offset 0x2A0 PLL_NPU Control Register  */
    __IOM uint32_t PLL_NPU_LOCK_CTRL_REG;             /*!< Offset 0x2A4 PLL_NPU Lock Control Register  */
    __IOM uint32_t PLL_NPU_PAT0_CTRL_REG;             /*!< Offset 0x2A8 PLL_NPU Pattern0 Control Register  */
    __IOM uint32_t PLL_NPU_PAT1_CTRL_REG;             /*!< Offset 0x2AC PLL_NPU Pattern1 Control Register  */
    __IOM uint32_t PLL_NPU_BIAS_REG;                  /*!< Offset 0x2B0 PLL_NPU Bias Register  */
         RESERVED(0x2B4[0x02E0 - 0x02B4], uint8_t)
    __IOM uint32_t PLL_DE_CTRL_REG;                   /*!< Offset 0x2E0 PLL_DE Control Register  */
    __IOM uint32_t PLL_DE_LOCK_CTRL_REG;              /*!< Offset 0x2E4 PLL_DE Lock Control Register  */
    __IOM uint32_t PLL_DE_PAT0_CTRL_REG;              /*!< Offset 0x2E8 PLL_DE Pattern0 Control Register  */
    __IOM uint32_t PLL_DE_PAT1_CTRL_REG;              /*!< Offset 0x2EC PLL_DE Pattern1 Control Register  */
    __IOM uint32_t PLL_DE_BIAS_REG;                   /*!< Offset 0x2F0 PLL_DE Bias Register  */
         RESERVED(0x2F4[0x0500 - 0x02F4], uint8_t)
    __IOM uint32_t AHB_CLK_REG;                       /*!< Offset 0x500 AHB Clock Register  */
         RESERVED(0x504[0x0510 - 0x0504], uint8_t)
    __IOM uint32_t APB0_CLK_REG;                      /*!< Offset 0x510 APB0 Clock Register  */
         RESERVED(0x514[0x0518 - 0x0514], uint8_t)
    __IOM uint32_t APB1_CLK_REG;                      /*!< Offset 0x518 APB1 Clock Register  */
         RESERVED(0x51C[0x0538 - 0x051C], uint8_t)
    __IOM uint32_t APB_UART_CLK_REG;                  /*!< Offset 0x538 APB_UART Clock Register  */
         RESERVED(0x53C[0x0540 - 0x053C], uint8_t)
    __IOM uint32_t TRACE_CLK_REG;                     /*!< Offset 0x540 TRACE Clock Register  */
         RESERVED(0x544[0x0560 - 0x0544], uint8_t)
    __IOM uint32_t GIC_CLK_REG;                       /*!< Offset 0x560 GIC Clock Register */
         RESERVED(0x564[0x0568 - 0x0564], uint8_t)
    __IOM uint32_t CPU_PERI_CLK_REG;                  /*!< Offset 0x568  */
         RESERVED(0x56C[0x0574 - 0x056C], uint8_t)
    __IOM uint32_t ITS0_BGR_REG;                      /*!< Offset 0x574 ITS0 Bus Gating Reset Register  */
         RESERVED(0x578[0x0580 - 0x0578], uint8_t)
    __IOM uint32_t NSI_CLK_REG;                       /*!< Offset 0x580 NSI Clock Register  */
    __IOM uint32_t NSI_BGR_REG;                       /*!< Offset 0x584 NSI Bus Gating Reset Register  */
    __IOM uint32_t MBUS_CLK_REG;                      /*!< Offset 0x588 MBUS Clock Register  */
    __IOM uint32_t IOMMU0_BGR_REG;                    /*!< Offset 0x58C IOMMU0 Bus Gating Reset Register  */
         RESERVED(0x590[0x0594 - 0x0590], uint8_t)
    __IOM uint32_t MSI_LITE0_BGR_REG;                 /*!< Offset 0x594 MSI_LITE0 Bus Gating Reset Register  */
         RESERVED(0x598[0x059C - 0x0598], uint8_t)
    __IOM uint32_t MSI_LITE1_BGR_REG;                 /*!< Offset 0x59C MSI_LITE1 Bus Gating Reset Register  */
         RESERVED(0x5A0[0x05A4 - 0x05A0], uint8_t)
    __IOM uint32_t MSI_LITE2_BGR_REG;                 /*!< Offset 0x5A4 MSI_LITE2 Bus Gating Reset Register  */
         RESERVED(0x5A8[0x05B4 - 0x05A8], uint8_t)
    __IOM uint32_t IOMMU1_BGR_REG;                    /*!< Offset 0x5B4 IOMMU1 Bus Gating Reset Register  */
         RESERVED(0x5B8[0x05C0 - 0x05B8], uint8_t)
    __IOM uint32_t AHB_MAT_CLK_GATING_REG;            /*!< Offset 0x5C0 AHB Master Clock Gating Register  */
         RESERVED(0x5C4[0x05E0 - 0x05C4], uint8_t)
    __IOM uint32_t MBUS_MAT_CLK_GATING_REG;           /*!< Offset 0x5E0 MBUS Master Clock Gating Register  */
    __IOM uint32_t MBUS_GATE_EN_REG;                  /*!< Offset 0x5E4 MBUS Gate Enable Register  */
         RESERVED(0x5E8[0x0704 - 0x05E8], uint8_t)
    __IOM uint32_t DMA0_BGR_REG;                      /*!< Offset 0x704 DMA0 Bus Gating Reset Register  */
         RESERVED(0x708[0x070C - 0x0708], uint8_t)
    __IOM uint32_t DMA1_BGR_REG;                      /*!< Offset 0x70C DMA1 Bus Gating Reset Register  */
         RESERVED(0x710[0x0724 - 0x0710], uint8_t)
    __IOM uint32_t SPINLOCK_BGR_REG;                  /*!< Offset 0x724 SPINLOCK Bus Gating Reset Register  */
         RESERVED(0x728[0x0744 - 0x0728], uint8_t)
    __IOM uint32_t MSGBOX0_BGR_REG;                   /*!< Offset 0x744 MSGBOX0 Bus Gating Reset Register  */
         RESERVED(0x748[0x0784 - 0x0748], uint8_t)
    __IOM uint32_t PWM0_BGR_REG;                      /*!< Offset 0x784 PWM0 Bus Gating Reset Register  */
         RESERVED(0x788[0x078C - 0x0788], uint8_t)
    __IOM uint32_t PWM1_BGR_REG;                      /*!< Offset 0x78C PWM1 Bus Gating Reset Register  */
         RESERVED(0x790[0x07A4 - 0x0790], uint8_t)
    __IOM uint32_t DBGSYS_BGR_REG;                    /*!< Offset 0x7A4 DBGSYS Bus Gating Reset Register  */
         RESERVED(0x7A8[0x07AC - 0x07A8], uint8_t)
    __IOM uint32_t SYSDAP_BGR_REG;                    /*!< Offset 0x7AC SYSDAP Bus Gating Reset Register  */
         RESERVED(0x7B0[0x0800 - 0x07B0], uint8_t)
    __IOM uint32_t TIMER0_CLK0_CLK_REG;               /*!< Offset 0x800 TIMER0_CLK0 Clock Register  */
    __IOM uint32_t TIMER0_CLK1_CLK_REG;               /*!< Offset 0x804 TIMER0_CLK1 Clock Register  */
    __IOM uint32_t TIMER0_CLK2_CLK_REG;               /*!< Offset 0x808 TIMER0_CLK2 Clock Register  */
    __IOM uint32_t TIMER0_CLK3_CLK_REG;               /*!< Offset 0x80C TIMER0_CLK3 Clock Register  */
    __IOM uint32_t TIMER0_CLK4_CLK_REG;               /*!< Offset 0x810 TIMER0_CLK4 Clock Register  */
    __IOM uint32_t TIMER0_CLK5_CLK_REG;               /*!< Offset 0x814 TIMER0_CLK5 Clock Register  */
    __IOM uint32_t TIMER0_CLK6_CLK_REG;               /*!< Offset 0x818 TIMER0_CLK6 Clock Register  */
    __IOM uint32_t TIMER0_CLK7_CLK_REG;               /*!< Offset 0x81C TIMER0_CLK7 Clock Register  */
    __IOM uint32_t TIMER0_CLK8_CLK_REG;               /*!< Offset 0x820 TIMER0_CLK8 Clock Register  */
    __IOM uint32_t TIMER0_CLK9_CLK_REG;               /*!< Offset 0x824 TIMER0_CLK9 Clock Register  */
         RESERVED(0x828[0x0850 - 0x0828], uint8_t)
    __IOM uint32_t TIMER0_BGR_REG;                    /*!< Offset 0x850 TIMER0 Bus Gating Reset Register  */
         RESERVED(0x854[0x0A00 - 0x0854], uint8_t)
    __IOM uint32_t DE0_CLK_REG;                       /*!< Offset 0xA00 DE0 Clock Register  */
    __IOM uint32_t DE0_BGR_REG;                       /*!< Offset 0xA04 DE0 Bus Gating Reset Register  */
         RESERVED(0xA08[0x0A20 - 0x0A08], uint8_t)
    __IOM uint32_t DI_CLK_REG;                        /*!< Offset 0xA20 DI Clock Register  */
    __IOM uint32_t DI_BGR_REG;                        /*!< Offset 0xA24 DI Bus Gating Reset Register  */
         RESERVED(0xA28[0x0A40 - 0x0A28], uint8_t)
    __IOM uint32_t G2D_CLK_REG;                       /*!< Offset 0xA40 G2D Clock Register  */
    __IOM uint32_t G2D_BGR_REG;                       /*!< Offset 0xA44 G2D Bus Gating Reset Register  */
         RESERVED(0xA48[0x0A60 - 0x0A48], uint8_t)
    __IOM uint32_t EINK_CLK_REG;                      /*!< Offset 0xA60 EINK Clock Register  */
    __IOM uint32_t EINK_PANEL_CLK_REG;                /*!< Offset 0xA64 EINK PANEL Clock Register */
         RESERVED(0xA68[0x0A6C - 0x0A68], uint8_t)
    __IOM uint32_t EINK_BGR_REG;                      /*!< Offset 0xA6C EINK Bus Gating Reset Register */
         RESERVED(0xA70[0x0A74 - 0x0A70], uint8_t)
    __IOM uint32_t DE_SYS_BGR_REG;                    /*!< Offset 0xA74 DE_SYS Bus Gating Reset Register  */
         RESERVED(0xA78[0x0A80 - 0x0A78], uint8_t)
    __IOM uint32_t VE_ENC0_CLK_REG;                   /*!< Offset 0xA80 VE_ENC0 Clock Register  */
         RESERVED(0xA84[0x0A88 - 0x0A84], uint8_t)
    __IOM uint32_t VE_DEC_CLK_REG;                    /*!< Offset 0xA88 VE_DEC Clock Register  */
    __IOM uint32_t VE_BGR_REG;                        /*!< Offset 0xA8C VE Bus Gating Reset Register  */
         RESERVED(0xA90[0x0AC0 - 0x0A90], uint8_t)
    __IOM uint32_t CE_CLK_REG;                        /*!< Offset 0xAC0 CE Clock Register  */
    __IOM uint32_t CE_BGR_REG;                        /*!< Offset 0xAC4 CE Bus Gating Reset Register  */
         RESERVED(0xAC8[0x0B00 - 0x0AC8], uint8_t)
    __IOM uint32_t NPU_CLK_REG;                       /*!< Offset 0xB00 NPU Clock Register  */
    __IOM uint32_t NPU_BGR_REG;                       /*!< Offset 0xB04 NPU Bus Gating Reset Register  */
         RESERVED(0xB08[0x0B20 - 0x0B08], uint8_t)
    __IOM uint32_t GPU0_CLK_REG;                      /*!< Offset 0xB20 GPU0 Clock Register  */
    __IOM uint32_t GPU0_GATING_REG;                   /*!< Offset 0xB24 GPU0 Gating Reset Configuration Register  */
         RESERVED(0xB28[0x0C00 - 0x0B28], uint8_t)
    __IOM uint32_t DRAM0_CLK_REG;                     /*!< Offset 0xC00 DRAM0 Clock Register  */
         RESERVED(0xC04[0x0C0C - 0x0C04], uint8_t)
    __IOM uint32_t DRAM0_BGR_REG;                     /*!< Offset 0xC0C DRAM0 Bus Gating Reset Register  */
         RESERVED(0xC10[0x0C80 - 0x0C10], uint8_t)
    __IOM uint32_t NAND0_CLK0_CLK_REG;                /*!< Offset 0xC80 NAND0 CLK0 Clock Register  */
    __IOM uint32_t NAND0_CLK1_CLK_REG;                /*!< Offset 0xC84 NAND0 CLK1 Clock Register  */
         RESERVED(0xC88[0x0C8C - 0x0C88], uint8_t)
    __IOM uint32_t NAND0_BGR_REG;                     /*!< Offset 0xC8C NAND0 Bus Gating Reset Register  */
         RESERVED(0xC90[0x0D00 - 0x0C90], uint8_t)
    __IOM uint32_t SMHC0_CLK_REG;                     /*!< Offset 0xD00 SMHC0 Clock Register  */
         RESERVED(0xD04[0x0D0C - 0x0D04], uint8_t)
    __IOM uint32_t SMHC0_BGR_REG;                     /*!< Offset 0xD0C SMHC0 Bus Gating Reset Register  */
    __IOM uint32_t SMHC1_CLK_REG;                     /*!< Offset 0xD10 SMHC1 Clock Register  */
         RESERVED(0xD14[0x0D1C - 0x0D14], uint8_t)
    __IOM uint32_t SMHC1_BGR_REG;                     /*!< Offset 0xD1C SMHC1 Bus Gating Reset Register  */
    __IOM uint32_t SMHC2_CLK_REG;                     /*!< Offset 0xD20 SMHC2 Clock Register  */
         RESERVED(0xD24[0x0D2C - 0x0D24], uint8_t)
    __IOM uint32_t SMHC2_BGR_REG;                     /*!< Offset 0xD2C SMHC2 Bus Gating Reset Register  */
    __IOM uint32_t SMHC3_CLK_REG;                     /*!< Offset 0xD30 SMHC3 Clock Register  */
         RESERVED(0xD34[0x0D3C - 0x0D34], uint8_t)
    __IOM uint32_t SMHC3_BGR_REG;                     /*!< Offset 0xD3C SMHC3 Bus Gating Reset Register  */
         RESERVED(0xD40[0x0D80 - 0x0D40], uint8_t)
    __IOM uint32_t UFS_AXI_CLK_REG;                   /*!< Offset 0xD80 UFS_AXI Clock Register  */
    __IOM uint32_t UFS_CFG_CLK_REG;                   /*!< Offset 0xD84 UFS_CFG Clock Register */
         RESERVED(0xD88[0x0D8C - 0x0D88], uint8_t)
    __IOM uint32_t UFS_BGR_REG;                       /*!< Offset 0xD8C UFS Bus Gating Reset Register  */
    __IOM uint32_t UFS_REF_CLK_EN_REG;                /*!< Offset 0xD90 UFS Reference Clock Enable Register */
         RESERVED(0xD94[0x0E00 - 0x0D94], uint8_t)
    __IOM uint32_t UART0_BGR_REG;                     /*!< Offset 0xE00 UART0 Bus Gating Reset Register  */
    __IOM uint32_t UART1_BGR_REG;                     /*!< Offset 0xE04 UART1 Bus Gating Reset Register  */
    __IOM uint32_t UART2_BGR_REG;                     /*!< Offset 0xE08 UART2 Bus Gating Reset Register  */
    __IOM uint32_t UART3_BGR_REG;                     /*!< Offset 0xE0C UART3 Bus Gating Reset Register  */
    __IOM uint32_t UART4_BGR_REG;                     /*!< Offset 0xE10 UART4 Bus Gating Reset Register  */
    __IOM uint32_t UART5_BGR_REG;                     /*!< Offset 0xE14 UART5 Bus Gating Reset Register  */
    __IOM uint32_t UART6_BGR_REG;                     /*!< Offset 0xE18 UART6 Bus Gating Reset Register  */
         RESERVED(0xE1C[0x0E80 - 0x0E1C], uint8_t)
    __IOM uint32_t TWI0_BGR_REG;                      /*!< Offset 0xE80 TWI0 Bus Gating Reset Register  */
    __IOM uint32_t TWI1_BGR_REG;                      /*!< Offset 0xE84 TWI1 Bus Gating Reset Register  */
    __IOM uint32_t TWI2_BGR_REG;                      /*!< Offset 0xE88 TWI2 Bus Gating Reset Register  */
    __IOM uint32_t TWI3_BGR_REG;                      /*!< Offset 0xE8C TWI3 Bus Gating Reset Register  */
    __IOM uint32_t TWI4_BGR_REG;                      /*!< Offset 0xE90 TWI4 Bus Gating Reset Register  */
    __IOM uint32_t TWI5_BGR_REG;                      /*!< Offset 0xE94 TWI5 Bus Gating Reset Register  */
    __IOM uint32_t TWI6_BGR_REG;                      /*!< Offset 0xE98 TWI6 Bus Gating Reset Register  */
    __IOM uint32_t TWI7_BGR_REG;                      /*!< Offset 0xE9C TWI7 Bus Gating Reset Register  */
    __IOM uint32_t TWI8_BGR_REG;                      /*!< Offset 0xEA0 TWI8 Bus Gating Reset Register  */
    __IOM uint32_t TWI9_BGR_REG;                      /*!< Offset 0xEA4 TWI9 Bus Gating Reset Register */
    __IOM uint32_t TWI10_BGR_REG;                     /*!< Offset 0xEA8 TWI10 Bus Gating Reset Register */
    __IOM uint32_t TWI11_BGR_REG;                     /*!< Offset 0xEAC TWI11 Bus Gating Reset Register */
    __IOM uint32_t TWI12_BGR_REG;                     /*!< Offset 0xEB0 TWI12 Bus Gating Reset Register */
         RESERVED(0xEB4[0x0F00 - 0x0EB4], uint8_t)
    __IOM uint32_t SPI0_CLK_REG;                      /*!< Offset 0xF00 SPI0 Clock Register  */
    __IOM uint32_t SPI0_BGR_REG;                      /*!< Offset 0xF04 SPI0 Bus Gating Reset Register  */
    __IOM uint32_t SPI1_CLK_REG;                      /*!< Offset 0xF08 SPI1 Clock Register  */
    __IOM uint32_t SPI1_BGR_REG;                      /*!< Offset 0xF0C SPI1 Bus Gating Reset Register  */
    __IOM uint32_t SPI2_CLK_REG;                      /*!< Offset 0xF10 SPI2 Clock Register  */
    __IOM uint32_t SPI2_BGR_REG;                      /*!< Offset 0xF14 SPI2 Bus Gating Reset Register  */
    __IOM uint32_t SPIF_CLK_REG;                      /*!< Offset 0xF18 SPIF Clock Register  */
    __IOM uint32_t SPIF_BGR_REG;                      /*!< Offset 0xF1C SPIF Bus Gating Reset Register  */
    __IOM uint32_t SPI3_CLK_REG;                      /*!< Offset 0xF20 SPI3 Clock Register  */
    __IOM uint32_t SPI3_BGR_REG;                      /*!< Offset 0xF24 SPI3 Bus Gating Reset Register  */
         RESERVED(0xF28[0x0FC0 - 0x0F28], uint8_t)
    __IOM uint32_t GPADC0_24M_CLK_REG;                /*!< Offset 0xFC0 GPADC0_24M Clock Register  */
    __IOM uint32_t GPADC0_BGR_REG;                    /*!< Offset 0xFC4 GPADC0 Bus Gating Reset Register  */
         RESERVED(0xFC8[0x0FE4 - 0x0FC8], uint8_t)
    __IOM uint32_t THS0_BGR_REG;                      /*!< Offset 0xFE4 THS0 Bus Gating Reset Register  */
         RESERVED(0xFE8[0x1000 - 0x0FE8], uint8_t)
    __IOM uint32_t IRRX_CLK_REG;                      /*!< Offset 0x1000 IRRX Clock Register  */
    __IOM uint32_t IRRX_BGR_REG;                      /*!< Offset 0x1004 IRRX Bus Gating Reset Register  */
    __IOM uint32_t IRTX_CLK_REG;                      /*!< Offset 0x1008 IRTX Clock Register  */
    __IOM uint32_t IRTX_BGR_REG;                      /*!< Offset 0x100C IRTX Bus Gating Reset Register  */
         RESERVED(0x1010[0x1024 - 0x1010], uint8_t)
    __IOM uint32_t LRADC_BGR_REG;                     /*!< Offset 0x1024 LRADC Bus Gating Reset Register  */
         RESERVED(0x1028[0x1200 - 0x1028], uint8_t)
    __IOM uint32_t I2SPCM0_CLK_REG;                   /*!< Offset 0x1200 I2SPCM0 Clock Register */
         RESERVED(0x1204[0x120C - 0x1204], uint8_t)
    __IOM uint32_t I2SPCM0_BGR_REG;                   /*!< Offset 0x120C I2SPCM0 Bus Gating Reset Register */
    __IOM uint32_t I2SPCM1_CLK_REG;                   /*!< Offset 0x1210 I2SPCM1 Clock Register */
         RESERVED(0x1214[0x121C - 0x1214], uint8_t)
    __IOM uint32_t I2SPCM1_BGR_REG;                   /*!< Offset 0x121C I2SPCM1 Bus Gating Reset Register  */
    __IOM uint32_t I2SPCM2_CLK_REG;                   /*!< Offset 0x1220 I2SPCM2 Clock Register */
    __IOM uint32_t I2SPCM2_ASRC_CLK_REG;              /*!< Offset 0x1224 I2SPCM2_ASRC Clock Register */
         RESERVED(0x1228[0x122C - 0x1228], uint8_t)
    __IOM uint32_t I2SPCM2_BGR_REG;                   /*!< Offset 0x122C I2SPCM2 Bus Gating Reset Register  */
    __IOM uint32_t I2SPCM3_CLK_REG;                   /*!< Offset 0x1230 I2SPCM3 Clock Register */
         RESERVED(0x1234[0x123C - 0x1234], uint8_t)
    __IOM uint32_t I2SPCM3_BGR_REG;                   /*!< Offset 0x123C I2SPCM3 Bus Gating Reset Register  */
    __IOM uint32_t I2SPCM4_CLK_REG;                   /*!< Offset 0x1240 I2SPCM4 Clock Register */
         RESERVED(0x1244[0x124C - 0x1244], uint8_t)
    __IOM uint32_t I2SPCM4_BGR_REG;                   /*!< Offset 0x124C I2SPCM4 Bus Gating Reset Register  */
         RESERVED(0x1250[0x1280 - 0x1250], uint8_t)
    __IOM uint32_t OWA_TX_CLK_REG;                    /*!< Offset 0x1280 OWA TX Clock Register */
    __IOM uint32_t OWA_RX_CLK_REG;                    /*!< Offset 0x1284 OWA RX Clock Register */
         RESERVED(0x1288[0x128C - 0x1288], uint8_t)
    __IOM uint32_t OWA_BGR_REG;                       /*!< Offset 0x128C OWA Bus Gating Reset Register  */
         RESERVED(0x1290[0x12C0 - 0x1290], uint8_t)
    __IOM uint32_t DMIC_CLK_REG;                      /*!< Offset 0x12C0 DMIC Clock Register */
         RESERVED(0x12C4[0x12CC - 0x12C4], uint8_t)
    __IOM uint32_t DMIC_BGR_REG;                      /*!< Offset 0x12CC DMIC Bus Gating Reset Register  */
         RESERVED(0x12D0[0x1300 - 0x12D0], uint8_t)
    __IOM uint32_t USB0_CLK_REG;                      /*!< Offset 0x1300 USB0 Clock Register */
    __IOM uint32_t USB0_BGR_REG;                      /*!< Offset 0x1304 USB0 Bus Gating Reset Register  */
    __IOM uint32_t USB1_CLK_REG;                      /*!< Offset 0x1308 USB1 Clock Register  */
    __IOM uint32_t USB1_BGR_REG;                      /*!< Offset 0x130C USB1 Bus Gating Reset Register  */
         RESERVED(0x1310[0x1340 - 0x1310], uint8_t)
    __IOM uint32_t USB0_USB1_REF_CLK_REG;             /*!< Offset 0x1340 USB0_USB1_REF Clock Register  */
         RESERVED(0x1344[0x1348 - 0x1344], uint8_t)
    __IOM uint32_t USB2_U2_REF_CLK_REG;               /*!< Offset 0x1348 USB2_U2_REF Clock Register  */
         RESERVED(0x134C[0x1350 - 0x134C], uint8_t)
    __IOM uint32_t USB2_SUSPEND_CLK_REG;              /*!< Offset 0x1350 USB2_SUSPEND Clock Register  */
    __IOM uint32_t USB2_MF_CLK_REG;                   /*!< Offset 0x1354 USB2_MF Clock Register  */
         RESERVED(0x1358[0x135C - 0x1358], uint8_t)
    __IOM uint32_t USB2_BGR_REG;                      /*!< Offset 0x135C USB2 Bus Gating Reset Register  */
    __IOM uint32_t USB2_U3_UTMI_CLK_REG;              /*!< Offset 0x1360 USB2_U3_UTMI Clock Register */
    __IOM uint32_t USB2_U2_PIPE_CLK_REG;              /*!< Offset 0x1364 USB2_U2_PIPE Clock Register */
         RESERVED(0x1368[0x1380 - 0x1368], uint8_t)
    __IOM uint32_t PCIE0_AUX_CLK_REG;                 /*!< Offset 0x1380 PCIE0_AUX Clock Register */
    __IOM uint32_t PCIE0_AXI_SLV_CLK_REG;             /*!< Offset 0x1384 PCIE0_AXI_SLV Clock Register */
         RESERVED(0x1388[0x138C - 0x1388], uint8_t)
    __IOM uint32_t PCIE0_BGR_REG;                     /*!< Offset 0x138C PCIE0 Bus Gating Reset Register */
         RESERVED(0x1390[0x13C0 - 0x1390], uint8_t)
    __IOM uint32_t SERDES_PHY_CFG_CLK_REG;            /*!< Offset 0x13C0 SERDES_PHY_CFG Clock Register */
    __IOM uint32_t SERDES_BGR_REG;                    /*!< Offset 0x13C4 SERDES Bus Gating Reset Register  */
         RESERVED(0x13C8[0x1400 - 0x13C8], uint8_t)
    __IOM uint32_t GMAC_PTP_CLK_REG;                  /*!< Offset 0x1400 GMAC_PTP Clock Register */
         RESERVED(0x1404[0x1410 - 0x1404], uint8_t)
    __IOM uint32_t GMAC0_PHY_CLK_REG;                 /*!< Offset 0x1410 GMAC0_PHY Clock Register  */
         RESERVED(0x1414[0x141C - 0x1414], uint8_t)
    __IOM uint32_t GMAC0_BGR_REG;                     /*!< Offset 0x141C GMAC0 Bus Gating Reset Register  */
         RESERVED(0x1420[0x1500 - 0x1420], uint8_t)
    __IOM uint32_t VO0_TCONLCD0_CLK_REG;              /*!< Offset 0x1500 VO0_TCONLCD0 Clock Register */
    __IOM uint32_t VO0_TCONLCD0_BGR_REG;              /*!< Offset 0x1504 VO0_TCONLCD0 Bus Gating Reset Register  */
    __IOM uint32_t VO0_TCONLCD1_CLK_REG;              /*!< Offset 0x1508 VO0_TCONLCD1 Clock Register  */
    __IOM uint32_t VO0_TCONLCD1_BGR_REG;              /*!< Offset 0x150C VO0_TCONLCD1 Bus Gating Reset Register  */
         RESERVED(0x1510[0x1544 - 0x1510], uint8_t)
    __IOM uint32_t LVDS0_BGR_REG;                     /*!< Offset 0x1544 LVDS0 Bus Gating Reset Register  */
         RESERVED(0x1548[0x154C - 0x1548], uint8_t)
    __IOM uint32_t LVDS1_BGR_REG;                     /*!< Offset 0x154C LVDS1 Bus Gating Reset Register  */
         RESERVED(0x1550[0x1580 - 0x1550], uint8_t)
    __IOM uint32_t DSI0_CLK_REG;                      /*!< Offset 0x1580 DSI0 Clock Register  */
    __IOM uint32_t DSI0_BGR_REG;                      /*!< Offset 0x1584 DSI0 Bus Gating Reset Register  */
    __IOM uint32_t DSI1_CLK_REG;                      /*!< Offset 0x1588 DSI1 Clock Register  */
    __IOM uint32_t DSI1_BGR_REG;                      /*!< Offset 0x158C DSI1 Bus Gating Reset Register  */
         RESERVED(0x1590[0x15C0 - 0x1590], uint8_t)
    __IOM uint32_t COMBPHY0_CLK_REG;                  /*!< Offset 0x15C0 COMBPHY0 Clock Register */
    __IOM uint32_t COMBPHY1_CLK_REG;                  /*!< Offset 0x15C4 COMBPHY1 Clock Register */
         RESERVED(0x15C8[0x1604 - 0x15C8], uint8_t)
    __IOM uint32_t TCONTV0_BGR_REG;                   /*!< Offset 0x1604 TCONTV0 Bus Gating Reset Register  */
         RESERVED(0x1608[0x160C - 0x1608], uint8_t)
    __IOM uint32_t TCONTV1_BGR_REG;                   /*!< Offset 0x160C TCONTV1 Bus Gating Reset Register  */
         RESERVED(0x1610[0x1640 - 0x1610], uint8_t)
    __IOM uint32_t EDP_TV_CLK_REG;                    /*!< Offset 0x1640 EDP_TV Clock Register  */
         RESERVED(0x1644[0x164C - 0x1644], uint8_t)
    __IOM uint32_t EDP_BGR_REG;                       /*!< Offset 0x164C EDP Bus Gating Reset Register  */
         RESERVED(0x1650[0x1680 - 0x1650], uint8_t)
    __IOM uint32_t HDMI_CEC_CLK_REG;                  /*!< Offset 0x1680 HDMI CEC Clock Register  */
    __IOM uint32_t HDMI_TV_CLK_REG;                   /*!< Offset 0x1684 HDMI_TV Clock Register  */
         RESERVED(0x1688[0x168C - 0x1688], uint8_t)
    __IOM uint32_t HDMI_BGR_REG;                      /*!< Offset 0x168C HDMI Bus Gating Reset Register  */
    __IOM uint32_t HDMI_SFR_CLK_REG;                  /*!< Offset 0x1690 HDMI SFR Clock Register */
    __IOM uint32_t HDCP_ESM_CLK_REG;                  /*!< Offset 0x1694 HDCP ESM Clock Register */
         RESERVED(0x1698[0x16C4 - 0x1698], uint8_t)
    __IOM uint32_t DPSS_TOP0_BGR_REG;                 /*!< Offset 0x16C4 DPSS_TOP0 Bus Gating Reset Register  */
         RESERVED(0x16C8[0x16CC - 0x16C8], uint8_t)
    __IOM uint32_t DPSS_TOP1_BGR_REG;                 /*!< Offset 0x16CC DPSS_TOP1 Bus Gating Reset Register  */
         RESERVED(0x16D0[0x16E4 - 0x16D0], uint8_t)
    __IOM uint32_t VIDEO_OUT0_BGR_REG;                /*!< Offset 0x16E4 VIDEO_OUT0 Bus Gating Reset Register  */
         RESERVED(0x16E8[0x16EC - 0x16E8], uint8_t)
    __IOM uint32_t VIDEO_OUT1_BGR_REG;                /*!< Offset 0x16EC VIDEO_OUT1 Bus Gating Reset Register  */
         RESERVED(0x16F0[0x1700 - 0x16F0], uint8_t)
    __IOM uint32_t LEDC_CLK_REG;                      /*!< Offset 0x1700 LEDC Clock Register  */
    __IOM uint32_t LEDC_BGR_REG;                      /*!< Offset 0x1704 LEDC Bus Gating Reset Register  */
         RESERVED(0x1708[0x1744 - 0x1708], uint8_t)
    __IOM uint32_t DSC_BGR_REG;                       /*!< Offset 0x1744 DSC Bus Gating Reset Register */
         RESERVED(0x1748[0x1800 - 0x1748], uint8_t)
    __IOM uint32_t CSI_MASTER0_CLK_REG;               /*!< Offset 0x1800 CSI Master0 Clock Register  */
    __IOM uint32_t CSI_MASTER1_CLK_REG;               /*!< Offset 0x1804 CSI Master1 Clock Register  */
    __IOM uint32_t CSI_MASTER2_CLK_REG;               /*!< Offset 0x1808 CSI Master2 Clock Register  */
         RESERVED(0x180C[0x1840 - 0x180C], uint8_t)
    __IOM uint32_t CSI_CLK_REG;                       /*!< Offset 0x1840 CSI Clock Register  */
    __IOM uint32_t CSI_BGR_REG;                       /*!< Offset 0x1844 CSI Bus Gating Reset Register  */
         RESERVED(0x1848[0x1860 - 0x1848], uint8_t)
    __IOM uint32_t ISP_CLK_REG;                       /*!< Offset 0x1860 ISP Clock Register  */
         RESERVED(0x1864[0x1884 - 0x1864], uint8_t)
    __IOM uint32_t VIDEO_IN_BGR_REG;                  /*!< Offset 0x1884 VIDEO_IN Bus Gating Reset Register  */
         RESERVED(0x1888[0x1904 - 0x1888], uint8_t)
    __IOM uint32_t DDRPLL_GATE_EN_REG;                /*!< Offset 0x1904 DDRPLL Gate Enable Register  */
    __IOM uint32_t PERI0PLL_GATE_EN_REG;              /*!< Offset 0x1908 PERI0PLL Gate Enable Register  */
    __IOM uint32_t PERI1PLL_GATE_EN_REG;              /*!< Offset 0x190C PERI1PLL Gate Enable Register  */
    __IOM uint32_t VIDEOPLL_GATE_EN_REG;              /*!< Offset 0x1910 VIDEOPLL Gate Enable Register  */
    __IOM uint32_t GPUPLL_GATE_EN_REG;                /*!< Offset 0x1914 GPUPLL Gate Enable Register  */
    __IOM uint32_t VEPLL_GATE_EN_REG;                 /*!< Offset 0x1918 VEPLL Gate Enable Register  */
    __IOM uint32_t AUDIOPLL_GATE_EN_REG;              /*!< Offset 0x191C AUDIOPLL Gate Enable Register  */
    __IOM uint32_t NPUPLL_GATE_EN_REG;                /*!< Offset 0x1920 NPUPLL Gate Enable Register */
         RESERVED(0x1924[0x1928 - 0x1924], uint8_t)
    __IOM uint32_t DEPLL_GATE_EN_REG;                 /*!< Offset 0x1928 DEPLL Gate Enable Register  */
         RESERVED(0x192C[0x1984 - 0x192C], uint8_t)
    __IOM uint32_t DDRPLL_GATE_STAT_REG;              /*!< Offset 0x1984 DDRPLL Gate Status Register  */
    __IOM uint32_t PERI0PLL_GATE_STAT_REG;            /*!< Offset 0x1988 PERI0PLL Gate Status Register  */
    __IOM uint32_t PERI1PLL_GATE_STAT_REG;            /*!< Offset 0x198C PERI1PLL Gate Status Register  */
    __IOM uint32_t VIDEOPLL_GATE_STAT_REG;            /*!< Offset 0x1990 VIDEOPLL Gate Status Register  */
    __IOM uint32_t GPUPLL_GATE_STAT_REG;              /*!< Offset 0x1994 GPUPLL Gate Status Register  */
    __IOM uint32_t VEPLL_GATE_STAT_REG;               /*!< Offset 0x1998 VEPLL Gate Status Register  */
    __IOM uint32_t AUDIOPLL_GATE_STAT_REG;            /*!< Offset 0x199C AUDIOPLL Gate Status Register  */
    __IOM uint32_t NPUPLL_GATE_STAT_REG;              /*!< Offset 0x19A0 NPUPLL Gate Status Register  */
         RESERVED(0x19A4[0x19A8 - 0x19A4], uint8_t)
    __IOM uint32_t DEPLL_GATE_STAT_REG;               /*!< Offset 0x19A8 DEPLL Gate Status Register  */
         RESERVED(0x19AC[0x1A00 - 0x19AC], uint8_t)
    __IOM uint32_t CLK24M_GATE_EN_REG;                /*!< Offset 0x1A00 CLK24M Gate Enable Register  */
         RESERVED(0x1A04[0x1B00 - 0x1A04], uint8_t)
    __IOM uint32_t CM_VI_CFG_REG;                     /*!< Offset 0x1B00 CM VI Enable Configuration Register */
    __IOM uint32_t CM_DESYS_CFG_REG;                  /*!< Offset 0x1B04 CM DESYS Enable Configuration Register */
         RESERVED(0x1B08[0x1B10 - 0x1B08], uint8_t)
    __IOM uint32_t CM_VE_DEC_CFG_REG;                 /*!< Offset 0x1B10 CM VE_DEC Enable Configuration Register */
    __IOM uint32_t CM_VE_ENC_CFG_REG;                 /*!< Offset 0x1B14 CM VE_ENC Enable Configuration Register */
         RESERVED(0x1B18[0x1B1C - 0x1B18], uint8_t)
    __IOM uint32_t CM_NPU_CFG_REG;                    /*!< Offset 0x1B1C CM NPU Enable Configuration Register */
         RESERVED(0x1B20[0x1B28 - 0x1B20], uint8_t)
    __IOM uint32_t CM_PCIE0_CFG_REG;                  /*!< Offset 0x1B28 CM PCIE0 Enable Configuration Register */
         RESERVED(0x1B2C[0x1B30 - 0x1B2C], uint8_t)
    __IOM uint32_t CM_USB2_CFG_REG;                   /*!< Offset 0x1B30 CM USB2 Enable Configuration Register */
    __IOM uint32_t CM_VO_CFG_REG;                     /*!< Offset 0x1B34 CM VO Enable Configuration Register */
    __IOM uint32_t CM_VO1_CFG_REG;                    /*!< Offset 0x1B38 CM VO1 Enable Configuration Register */
         RESERVED(0x1B3C[0x1C00 - 0x1B3C], uint8_t)
    __IOM uint32_t APB2JTAG_CLK_REG;                  /*!< Offset 0x1C00 APB2JTAG Clock Register */
    __IOM uint32_t APB2JTAG_BGR_REG;                  /*!< Offset 0x1C04 APB2JTAG Bus Gating Reset Register */
         RESERVED(0x1C08[0x1F00 - 0x1C08], uint8_t)
    __IOM uint32_t CCMU_SEC_SWITCH_REG;               /*!< Offset 0x1F00 CCMU Security Switch Register  */
         RESERVED(0x1F04[0x1F10 - 0x1F04], uint8_t)
    __IOM uint32_t SYSDAP_REQ_CTRL_REG;               /*!< Offset 0x1F10 SYSDAP REQ Control Register */
         RESERVED(0x1F14[0x1F20 - 0x1F14], uint8_t)
    __IOM uint32_t PLL_CFG0_REG;                      /*!< Offset 0x1F20 PLL Configuration0 Register  */
    __IOM uint32_t PLL_CFG1_REG;                      /*!< Offset 0x1F24 PLL Configuration1 Register  */
    __IOM uint32_t PLL_CFG2_REG;                      /*!< Offset 0x1F28 PLL Configuration2 Register  */
         RESERVED(0x1F2C[0x1F30 - 0x1F2C], uint8_t)
    __IOM uint32_t CCMU_FAN_GATE_REG;                 /*!< Offset 0x1F30 CCMU FANOUT CLOCK GATE Register */
    __IOM uint32_t CLK27M_FAN_REG;                    /*!< Offset 0x1F34 CLK27M FANOUT Register */
    __IOM uint32_t CLK_FAN_REG;                       /*!< Offset 0x1F38 PCLK FANOUT Register */
    __IOM uint32_t CCMU_FAN_REG;                      /*!< Offset 0x1F3C CCMU FANOUT Register */
} CCU_TypeDef; /* size of structure = 0x1F40 */
/*
 * @brief CLUSTER_CFG
 */
/*!< CLUSTER_CFG  */
typedef struct CLUSTER_CFG_Type
{
    struct
    {
        __IOM uint32_t C0_CPUx_CTRL_REG;              /*!< Offset 0x000 Cluster0 CPUx Control Register */
        __IOM uint32_t C0_CPUx_STATUS0;               /*!< Offset 0x004 Cluster0 CPUx Status 0 Register */
             RESERVED(0x008[0x1000 - 0x0008], uint8_t)
    } C0_CPU [0x008];                                 /*!< Offset 0x000 Cluster0 CPUx Control Register */
    __IOM uint32_t C0_RST_CTRL;                       /*!< Offset 0x8000 Cluster 0 Reset Control Register */
    __IOM uint32_t C0_CTRL_REG0;                      /*!< Offset 0x8004 Cluster 0 Control Register 0 */
    __IOM uint32_t C0_CTRL_REG1;                      /*!< Offset 0x8008 Cluster 0 Control Register 1 */
    __IOM uint32_t CLK_FORCE_CFG_REG;                 /*!< Offset 0x800C Clk Control ForceConfiguration Register */
    __IOM uint32_t CLUSTER_SPRAM_CFG_REG;             /*!< Offset 0x8010 CPU cluster sprammemoryConfiguration Register */
    __IOM uint32_t CLUSTER_RF2P_CFG_REG;              /*!< Offset 0x8014 CPU cluster rf2pmemoryConfiguration Register */
    __IOM uint32_t CLUSTER_RFP2P_CFG_REG;             /*!< Offset 0x8018 CPU cluster rfp2pmemoryConfiguration Register */
    __IOM uint32_t CLUSTER_SYSSPRAM_CFG_REG;          /*!< Offset 0x801C CPU cluster syssprammemoryConfiguration Register */
    __IOM uint32_t DSU_AXI_TH_CFG_REG;                /*!< Offset 0x8020 DSUAXI2TO1THConfiguration Register */
         RESERVED(0x8024[0x8030 - 0x8024], uint8_t)
    __IOM uint32_t CLU_DSU_STATUS_REG;                /*!< Offset 0x8030 ClusterDSU Status Register */
         RESERVED(0x8034[0x80A4 - 0x8034], uint8_t)
    __IOM uint32_t CLU_DSU_RST_CTRL;                  /*!< Offset 0x80A4 ClusterDSUresetselfrelease Register */
         RESERVED(0x80A8[0x80B0 - 0x80A8], uint8_t)
    __IOM uint32_t AXI0_MNT_CTRL_REG;                 /*!< Offset 0x80B0 DSUAXI0MONITOR Control Register */
    __IOM uint32_t AXI0_MNT_PRD_REG;                  /*!< Offset 0x80B4 DSUAXI0MONITORPeriod Register */
    __IOM uint32_t AXI0_MNT_RLTCY_REG;                /*!< Offset 0x80B8 DSUAXI0MONITORReadTotalLatency Register */
    __IOM uint32_t AXI0_MNT_WLTCY_REG;                /*!< Offset 0x80BC DSUAXI0MONITORWriteTotalLatency Register */
    __IOM uint32_t DSU_AXI0_MNT_RREQ_REG;             /*!< Offset 0x80C0 DSUAXI0MONITORReadRequesttimes Register */
    __IOM uint32_t DSU_AXI0_MNT_WREQ_REG;             /*!< Offset 0x80C4 DSUAXI0MONITORWriteRequesttimes Register */
    __IOM uint32_t DSU_AXI0_MNT_RBD_REG;              /*!< Offset 0x80C8 DSUAXI0MONITORReadBandwidth Register */
    __IOM uint32_t DSU_AXI0_MNT_WBD_REG;              /*!< Offset 0x80CC DSUAXI0MONITORWriteBandwidth Register */
    __IOM uint32_t DSU_AXI1_MNT_CTRL_REG;             /*!< Offset 0x80D0 DSUAXI1MONITOR Control Register */
    __IOM uint32_t DSU_AXI1_MNT_PRD_REG;              /*!< Offset 0x80D4 DSUAXI1MONITORPeriod Register */
    __IOM uint32_t DSU_AXI1_MNT_RLTCY_REG;            /*!< Offset 0x80D8 DSUAXI1MONITORReadTotalLatency Register */
    __IOM uint32_t DSU_AXI1_MNT_WLTCY_REG;            /*!< Offset 0x80DC DSUAXI1MONITORWriteTotalLatency Register */
    __IOM uint32_t DSU_AXI1_MNT_RREQ_REG;             /*!< Offset 0x80E0 DSUAXI1MONITORReadRequesttimes Register */
    __IOM uint32_t DSU_AXI1_MNT_WREQ_REG;             /*!< Offset 0x80E4 DSUAXI1MONITORWriteRequesttimes Register */
    __IOM uint32_t DSU_AXI1_MNT_RBD_REG;              /*!< Offset 0x80E8 DSUAXI1MONITORReadBandwidth Register */
    __IOM uint32_t DSU_AXI1_MNT_WBD_REG;              /*!< Offset 0x80EC DSUAXI1MONITORWriteBandwidth Register */
} CLUSTER_CFG_TypeDef; /* size of structure = 0x80F0 */
/*
 * @brief CPUS_INTERRUPT_CTRL
 */
/*!< CPUS_INTERRUPT_CTRL  */
typedef struct CPUS_INTERRUPT_CTRL_Type
{
         RESERVED(0x000[0x0010 - 0x0000], uint8_t)
    __IOM uint32_t INTC_CONFIG_REG [0x008];           /*!< Offset 0x010 Group Interrupt Configuration Register 0..7 */
         RESERVED(0x030[0x0100 - 0x0030], uint8_t)
    __IM  uint32_t SYS_INT_STATE [0x007];             /*!< Offset 0x100 System Interrput0..Interrput6 State Registers */
} CPUS_INTERRUPT_CTRL_TypeDef; /* size of structure = 0x11C */
/*
 * @brief CPU_PLL_CFG
 */
/*!< CPU_PLL_CFG  */
typedef struct CPU_PLL_CFG_Type
{
    __IOM uint32_t CPU_BACK_PLL_CTRL_REG;             /*!< Offset 0x000 CPU_BACK_PLL Control Register */
    __IOM uint32_t CPU_BACK_PLL_LOCK_CTRL_REG;        /*!< Offset 0x004 CPU_BACK_PLLlock Control Register */
    __IOM uint32_t CPU_BACK_PLL_PAT0_CTRL_REG;        /*!< Offset 0x008 CPU_BACK_PLL Pattern 0 Control Register */
    __IOM uint32_t CPU_BACK_PLL_PAT1_CTRL_REG;        /*!< Offset 0x00C CPU_BACK_PLL Pattern 1 Control Register */
    __IOM uint32_t CPU_BACK_PLL_BIAS_REG;             /*!< Offset 0x010 CPU_BACK_PLLBias Register */
         RESERVED(0x014[0x1000 - 0x0014], uint8_t)
    __IOM uint32_t CPU_L_PLL_CTRL_REG;                /*!< Offset 0x1000 CPU_L_PLL Control Register */
    __IOM uint32_t CPU_L_PLL_PAT0_CTRL_REG;           /*!< Offset 0x1004 CPU_L_PLL Pattern 0 Control Register */
    __IOM uint32_t CPU_L_PLL_PAT1_CTRL_REG;           /*!< Offset 0x1008 CPU_L_PLL Pattern 1 Control Register */
    __IOM uint32_t CPU_L_PLL_BIAS_REG;                /*!< Offset 0x100C CPU_L_PLLBias Register */
    __IOM uint32_t CPU_L_PLL_TUN0_REG;                /*!< Offset 0x1010 CPU_L_PLLTuning0 Register */
    __IOM uint32_t CPU_L_PLL_TUN1_REG;                /*!< Offset 0x1014 CPU_L_PLLTuning1 Register */
    __IOM uint32_t CPU_L_PLL_LFM_REG;                 /*!< Offset 0x1018 CPU_L_PLLLFM Register */
    __IOM uint32_t CPU_L_PLL_CLK_REG;                 /*!< Offset 0x101C CPU_L_PLLClock Register */
    __IOM uint32_t CPU_L_PLL_GATING_REG;              /*!< Offset 0x1020 CPU_L_PLLGatingConfiguration Register */
    __IOM uint32_t CPU_L_PLL_UNLOCK_IRQEN_REG;        /*!< Offset 0x1024 CPU_L_PLLUnlockIRQEnable Register */
    __IOM uint32_t CPU_L_PLL_UNLOCK_STAT_REG;         /*!< Offset 0x1028 CPU_L_PLLUnlock Status Register */
    __IOM uint32_t CPU_L_PLL_CTRL_STAT_REG;           /*!< Offset 0x102C CPU_L_PLL Control Status Register */
         RESERVED(0x1030[0x2000 - 0x1030], uint8_t)
    __IOM uint32_t CPU_B_PLL_CTRL_REG;                /*!< Offset 0x2000 CPU_B_PLL Control Register */
    __IOM uint32_t CPU_B_PLL_PAT0_CTRL_REG;           /*!< Offset 0x2004 CPU_B_PLL Pattern 0 Control Register */
    __IOM uint32_t CPU_B_PLL_PAT1_CTRL_REG;           /*!< Offset 0x2008 CPU_B_PLL Pattern 1 Control Register */
    __IOM uint32_t CPU_B_PLL_BIAS_REG;                /*!< Offset 0x200C CPU_B_PLLBias Register */
    __IOM uint32_t CPU_B_PLL_TUN0_REG;                /*!< Offset 0x2010 CPU_B_PLLTuning0 Register */
    __IOM uint32_t CPU_B_PLL_TUN1_REG;                /*!< Offset 0x2014 CPU_B_PLLTuning1 Register */
    __IOM uint32_t CPU_B_PLL_LFM_REG;                 /*!< Offset 0x2018 CPU_B_PLLLFM Register */
    __IOM uint32_t CPU_B_PLL_CLK_REG;                 /*!< Offset 0x201C CPU_B_PLLClock Register */
    __IOM uint32_t CPU_B_PLL_GATING_REG;              /*!< Offset 0x2020 CPU_B_PLLGatingConfiguration Register */
    __IOM uint32_t CPU_B_PLL_UNLOCK_IRQEN_REG;        /*!< Offset 0x2024 CPU_B_PLLUnlockIRQEnable Register */
    __IOM uint32_t CPU_B_PLL_UNLOCK_STAT_REG;         /*!< Offset 0x2028 CPU_B_PLLUnlock Status Register */
    __IOM uint32_t CPU_B_PLL_CTRL_STAT_REG;           /*!< Offset 0x202C CPU_B_PLL Control Status Register */
         RESERVED(0x2030[0x3000 - 0x2030], uint8_t)
    __IOM uint32_t CPU_DSU_PLL_CTRL_REG;              /*!< Offset 0x3000 CPU_DSU_PLL Control Register */
    __IOM uint32_t CPU_DSU_PLL_PAT0_CTRL_REG;         /*!< Offset 0x3004 CPU_DSU_PLL Pattern 0 Control Register */
    __IOM uint32_t CPU_DSU_PLL_PAT1_CTRL_REG;         /*!< Offset 0x3008 CPU_DSU_PLL Pattern 1 Control Register */
    __IOM uint32_t CPU_DSU_PLL_BIAS_REG;              /*!< Offset 0x300C CPU_DSU_PLLBias Register */
    __IOM uint32_t CPU_DSU_PLL_TUN0_REG;              /*!< Offset 0x3010 CPU_DSU_PLLTuning0 Register */
    __IOM uint32_t CPU_DSU_PLL_TUN1_REG;              /*!< Offset 0x3014 CPU_DSU_PLLTuning1 Register */
    __IOM uint32_t CPU_DSU_PLL_LFM_REG;               /*!< Offset 0x3018 CPU_DSU_PLLLFM Register */
    __IOM uint32_t CPU_DSU_PLL_CLK_REG;               /*!< Offset 0x301C CPU_DSU_PLLClock Register */
    __IOM uint32_t CPU_DSU_PLL_GATING_REG;            /*!< Offset 0x3020 CPU_DSU_PLLGatingConfiguration Register */
    __IOM uint32_t CPU_DSU_PLL_UNLOCK_IRQEN_REG;      /*!< Offset 0x3024 CPU_DSU_PLLUnlockIRQEnable Register */
    __IOM uint32_t CPU_DSU_PLL_UNLOCK_STAT_REG;       /*!< Offset 0x3028 CPU_DSU_PLLUnlock Status Register */
    __IOM uint32_t CPU_DSU_PLL_CTRL_STAT_REG;         /*!< Offset 0x302C CPU_DSU_PLL Control Status Register */
         RESERVED(0x3030[0x3034 - 0x3030], uint8_t)
    __IOM uint32_t CPU_SLOW_CLK_SEL_REG;              /*!< Offset 0x3034 CPUSlowClockSelect Register */
} CPU_PLL_CFG_TypeDef; /* size of structure = 0x3038 */
/*
 * @brief CPU_SUBSYS_CTRL
 */
/*!< CPU_SUBSYS_CTRL  */
typedef struct CPU_SUBSYS_CTRL_Type
{
    __IOM uint32_t GENER_CTRL_REG0;                   /*!< Offset 0x000 General Control Register 0 */
         RESERVED(0x004[0x000C - 0x0004], uint8_t)
    __IOM uint32_t CPU_SYS_RST_CTRL;                  /*!< Offset 0x00C CPU Subsystem Reset Control Register */
         RESERVED(0x010[0x0140 - 0x0010], uint8_t)
    __IOM uint32_t PLL_CTRL_REG0;                     /*!< Offset 0x140 PLL Control Register 0 */
    __IOM uint32_t PLL_CTRL_REG1;                     /*!< Offset 0x144 PLL Control Register 1 */
         RESERVED(0x148[0x0200 - 0x0148], uint8_t)
    __IOM uint32_t CPU_DA_DDR_CTRL_REG;               /*!< Offset 0x200 CPU Direct Access DDR Control Register */
         RESERVED(0x204[0x1000 - 0x0204], uint8_t)
    struct
    {
        __IOM uint32_t CPU_CTRL_REG;                  /*!< Offset 0x1000 Cluster 0 CPU0 Control Register */
        __IOM uint32_t RVBARADDR_L;                   /*!< Offset 0x1004 Cluster0 Reset Vector Base Address Register 0_L */
        __IOM uint32_t RVBARADDR_H;                   /*!< Offset 0x1008 Cluster0 Reset Vector Base Address Register 0_H */
             RESERVED(0x00C[0x1000 - 0x000C], uint8_t)
    } CLU0 [0x008];                                   /*!< Offset 0x1000 Cluster 0 CPUx Control Register */
} CPU_SUBSYS_CTRL_TypeDef; /* size of structure = 0x9000 */
/*
 * @brief DE_BLD
 */
/*!< DE_BLD Display Engine (DE) - Blender */
typedef struct DE_BLD_Type
{
    __IOM uint32_t BLD_EN_COLOR_CTL;                  /*!< Offset 0x000 BLD_FILL_COLOR_CTL Offset 0x000 BLD fill color control register */
    struct
    {
        __IOM uint32_t BLD_FILL_COLOR;                /*!< Offset 0x004 BLD fill color register */
        __IOM uint32_t BLD_CH_ISIZE;                  /*!< Offset 0x008 BLD input memory size register */
        __IOM uint32_t BLD_CH_OFFSET;                 /*!< Offset 0x00C BLD input memory offset register */
             RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    } CH [0x006];                                     /*!< Offset 0x004 Pipe [0..5] */
         RESERVED(0x064[0x0080 - 0x0064], uint8_t)
    __IOM uint32_t ROUTE;                             /*!< Offset 0x080 BLD_CH_RTCTL BLD routing control register (default value 0x00543210) */
    __IOM uint32_t PREMULTIPLY;                       /*!< Offset 0x084 BLD pre-multiply control register */
    __IOM uint32_t BKCOLOR;                           /*!< Offset 0x088  */
    __IOM uint32_t OUTPUT_SIZE;                       /*!< Offset 0x08C  */
    __IOM uint32_t BLD_MODE [0x006];                  /*!< Offset 0x090 BLD_CTL SUN8I_MIXER_BLEND_MODE blender0..blaener3 (or more) */
         RESERVED(0x0A8[0x00B0 - 0x00A8], uint8_t)
    __IOM uint32_t CK_CTL;                            /*!< Offset 0x0B0  */
    __IOM uint32_t CK_CFG;                            /*!< Offset 0x0B4  */
         RESERVED(0x0B8[0x00C0 - 0x00B8], uint8_t)
    __IOM uint32_t CK_MAX [0x004];                    /*!< Offset 0x0C0  */
         RESERVED(0x0D0[0x00E0 - 0x00D0], uint8_t)
    __IOM uint32_t CK_MIN [0x004];                    /*!< Offset 0x0E0  */
         RESERVED(0x0F0[0x00FC - 0x00F0], uint8_t)
    __IOM uint32_t OUT_CTL;                           /*!< Offset 0x0FC  */
    __IOM uint32_t CSC_CTL;                           /*!< Offset 0x100 SUN50I_MIXER_BLEND_CSC_CTL  */
         RESERVED(0x104[0x0110 - 0x0104], uint8_t)
    __IOM uint32_t CSC_COEFF [0x00C];                 /*!< Offset 0x110 SUN50I_MIXER_BLEND_CSC_COEFF(base, layer, x) ((base) + 0x110 + (layer)*0x30 + (x)*4) */
} DE_BLD_TypeDef; /* size of structure = 0x140 */
/*
 * @brief DE_BLS
 */
/*!< DE_BLS Blue Level Stretch (BLS) */
typedef struct DE_BLS_Type
{
    __IOM uint32_t BLS_CTRL_REG;                      /*!< Offset 0x000 BLS module control register */
    __IOM uint32_t BLS_SIZE_REG;                      /*!< Offset 0x004 BLS size register */
    __IOM uint32_t BLS_WIN0_REG;                      /*!< Offset 0x008 BLS window setting register0 */
    __IOM uint32_t BLS_WIN1_REG;                      /*!< Offset 0x00C BLS window setting register1 */
    __IOM uint32_t BLS_ATTLUT_REG [0x004];            /*!< Offset 0x010 0x10+N*0x4 BLS attenuation LUT register, +N*0x4 (N = 0,1,2,3) */
    __IOM uint32_t BLS_POS_REG;                       /*!< Offset 0x020 BLS blue zone position register */
         RESERVED(0x024[0x0030 - 0x0024], uint8_t)
    __IOM uint32_t BLS_GAINLUT_REG [0x004];           /*!< Offset 0x030 0x30+N*0x4 BLS GainLUT access register, +N*0x4, Total 16byte, 16*8bit (N = 0,1,2,3) */
} DE_BLS_TypeDef; /* size of structure = 0x040 */
/*
 * @brief DE_DNS
 */
/*!< DE_DNS Denoise (DNS) */
typedef struct DE_DNS_Type
{
    __IOM uint32_t DNS_CTL;                           /*!< Offset 0x000 DNS module control register */
    __IOM uint32_t DNS_SIZE;                          /*!< Offset 0x004 DNS size register */
} DE_DNS_TypeDef; /* size of structure = 0x008 */
/*
 * @brief DE_FCC
 */
/*!< DE_FCC Fancy color curvature (FCC) */
typedef struct DE_FCC_Type
{
    __IOM uint32_t FCC_CTL_REG;                       /*!< Offset 0x000 FCC Control Register */
    __IOM uint32_t FCC_INPUT_SIZE_REG;                /*!< Offset 0x004 FCC Input Size Register */
    __IOM uint32_t FCC_OUTPUT_WIN0_REG;               /*!< Offset 0x008 FCC Output Window0 Register */
    __IOM uint32_t FCC_OUTPUT_WIN1_REG;               /*!< Offset 0x00C FCC Output Window1 Register */
} DE_FCC_TypeDef; /* size of structure = 0x010 */
/*
 * @brief DE_FCE
 */
/*!< DE_FCE Fresh and Contrast Enhancement (FCE) */
typedef struct DE_FCE_Type
{
    __IOM uint32_t GCTRL_REG;                         /*!< Offset 0x000 Control register */
    __IOM uint32_t FCE_SIZE_REG;                      /*!< Offset 0x004 Size setting register */
    __IOM uint32_t FCE_WIN0_REG;                      /*!< Offset 0x008 Window setting 0 register */
    __IOM uint32_t FCE_WIN1_REG;                      /*!< Offset 0x00C Window setting 1 register */
         RESERVED(0x010[0x0020 - 0x0010], uint8_t)
    __IOM uint32_t HIST_SUM_REG;                      /*!< Offset 0x020 Histogram sum register */
    __IOM uint32_t HIST_STATUS_REG;                   /*!< Offset 0x024 Histogram status register */
    __IOM uint32_t CE_STATUS_REG;                     /*!< Offset 0x028 CE LUT status register */
    __IOM uint32_t CE_CC_REG;                         /*!< Offset 0x02C CE chroma compensation function setting register */
    __IOM uint32_t FTC_GAIN_REG;                      /*!< Offset 0x030 FTC gain setting register */
    __IOM uint32_t FTD_HUE_THR_REG;                   /*!< Offset 0x034 FTD hue threshold setting register */
    __IOM uint32_t FTD_CHROMA_THR_REG;                /*!< Offset 0x038 FTD chroma threshold setting register */
    __IOM uint32_t FTD_SLP_REG;                       /*!< Offset 0x03C FTD slop setting register */
    __IOM uint32_t CSC_ENABLE_REG;                    /*!< Offset 0x040 CSC enable setting register */
    __IOM uint32_t CSC_D0_REG;                        /*!< Offset 0x044 CSC Constant D0 Register */
    __IOM uint32_t CSC_D1_REG;                        /*!< Offset 0x048 CSC Constant D1 Register */
    __IOM uint32_t CSC_D2_REG;                        /*!< Offset 0x04C CSC Constant D2 Register */
    __IOM uint32_t CSC_C00_REG;                       /*!< Offset 0x050 CSC Coefficient 00 Register */
    __IOM uint32_t CSC_C01_REG;                       /*!< Offset 0x054 CSC Coefficient 01 Register */
    __IOM uint32_t CSC_C02_REG;                       /*!< Offset 0x058 CSC Coefficient 02 Register */
    __IOM uint32_t CSC_C03_REG;                       /*!< Offset 0x05C CSC Constant 03 Register */
    __IOM uint32_t CSC_C10_REG;                       /*!< Offset 0x060 CSC Coefficient 10 Register */
    __IOM uint32_t CSC_C11_REG;                       /*!< Offset 0x064 CSC Coefficient 11 Register */
    __IOM uint32_t CSC_C12_REG;                       /*!< Offset 0x068 CSC Coefficient 12 Register */
    __IOM uint32_t CSC_C13_REG;                       /*!< Offset 0x06C CSC Constant 13 Register */
    __IOM uint32_t CSC_C20_REG;                       /*!< Offset 0x070 CSC Coefficient 20 Register */
    __IOM uint32_t CSC_C21_REG;                       /*!< Offset 0x074 CSC Coefficient 21 Register */
    __IOM uint32_t CSC_C22_REG;                       /*!< Offset 0x078 CSC Coefficient 22 Register */
    __IOM uint32_t CSC_C23_REG;                       /*!< Offset 0x07C CSC Constant 23 Register */
         RESERVED(0x080[0x0200 - 0x0080], uint8_t)
    __IOM uint32_t CE_LUT_REGN [0x080];               /*!< Offset 0x200 0x200+N*4 CE LUT register N (N=0:127) */
    __IOM uint32_t HIST_CNT_REGN [0x100];             /*!< Offset 0x400 0x400+N*4 Histogram count register N (N=0:255) */
} DE_FCE_TypeDef; /* size of structure = 0x800 */
/*
 * @brief DE_GLB
 */
/*!< DE_GLB Display Engine (DE) - Global Control */
typedef struct DE_GLB_Type
{
    __IOM uint32_t GLB_CTL;                           /*!< Offset 0x000 Global control register */
    __IOM uint32_t GLB_STS;                           /*!< Offset 0x004 Global status register */
    __IOM uint32_t GLB_SIZE;                          /*!< Offset 0x008 Global size register */
    __IOM uint32_t GLB_CLK;                           /*!< Offset 0x00C Global clock register */
    __IOM uint32_t GLB_DBUFFER;                       /*!< Offset 0x010 Global double buffer control register */
         RESERVED(0x014[0x0040 - 0x0014], uint8_t)
} DE_GLB_TypeDef; /* size of structure = 0x040 */
/*
 * @brief DE_TOP
 */
/*!< DE_TOP Display Engine (DE) TOP (APB) */
typedef struct DE_TOP_Type
{
    __IOM uint32_t DE_SCLK_GATE;                      /*!< Offset 0x000 DE SCLK Gating Register */
    __IOM uint32_t DE_HCLK_GATE;                      /*!< Offset 0x004 DE HCLK Gating Register */
    __IOM uint32_t DE_AHB_RESET;                      /*!< Offset 0x008 DE AHB Reset Register DE_MBUS_CLOCK_ADDR */
    __IOM uint32_t DE_SCLK_DIV;                       /*!< Offset 0x00C DE SCLK Division Register  */
    __IOM uint32_t DE2TCON_MUX;                       /*!< Offset 0x010 DE MUX Register DE2TCON_MUX_OFFSET */
    __IOM uint32_t DE_VER_CTL;                        /*!< Offset 0x014 DE_VER_CTL_OFFSET */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IOM uint32_t DE_RTWB_MUX;                       /*!< Offset 0x020 DE_RTWB_MUX_OFFSET */
    __IOM uint32_t DE_CHN2CORE_MUX;                   /*!< Offset 0x024 DE_CHN2CORE_MUX_OFFSET */
    __IOM uint32_t DE_PORT2CHN_MUX [0x004];           /*!< Offset 0x028 DE_PORT2CHN_MUX_OFFSET(disp) (0x8028 + (disp) * 0x4) */
         RESERVED(0x038[0x00E0 - 0x0038], uint8_t)
    __IOM uint32_t DE_DEBUG_CTL;                      /*!< Offset 0x0E0 DE_DEBUG_CTL_OFFSET */
} DE_TOP_TypeDef; /* size of structure = 0x0E4 */
/*
 * @brief DE_UI
 */
/*!< DE_UI Display Engine (DE) - UI surface */
typedef struct DE_UI_Type
{
    struct
    {
        __IOM uint32_t ATTR;                          /*!< Offset 0x000  */
        __IOM uint32_t SIZE;                          /*!< Offset 0x004  */
        __IOM uint32_t COORD;                         /*!< Offset 0x008  */
        __IOM uint32_t PITCH;                         /*!< Offset 0x00C  */
        __IOM uint32_t TOP_LADDR;                     /*!< Offset 0x010  */
        __IOM uint32_t BOT_LADDR;                     /*!< Offset 0x014  */
        __IOM uint32_t FCOLOR;                        /*!< Offset 0x018  */
             RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    } CFG [0x004];                                    /*!< Offset 0x000  */
    __IOM uint32_t TOP_HADDR;                         /*!< Offset 0x080  */
    __IOM uint32_t BOT_HADDR;                         /*!< Offset 0x084  */
    __IOM uint32_t OVL_SIZE;                          /*!< Offset 0x088  */
         RESERVED(0x08C[0x0800 - 0x008C], uint8_t)
} DE_UI_TypeDef; /* size of structure = 0x800 */
/*
 * @brief DE_UIS
 */
/*!< DE_UIS UI Scaler(UIS) provides RGB format image resizing function */
typedef struct DE_UIS_Type
{
    __IOM uint32_t UIS_CTRL_REG;                      /*!< Offset 0x000 Control register */
         RESERVED(0x004[0x0008 - 0x0004], uint8_t)
    __IOM uint32_t UIS_STATUS_REG;                    /*!< Offset 0x008 Status register */
    __IOM uint32_t UIS_FIELD_CTRL_REG;                /*!< Offset 0x00C Field control register */
    __IOM uint32_t UIS_BIST_REG;                      /*!< Offset 0x010 BIST control register */
         RESERVED(0x014[0x0040 - 0x0014], uint8_t)
    __IOM uint32_t UIS_OUTSIZE_REG;                   /*!< Offset 0x040 Output size register */
    __IOM uint32_t UIS_GLOBAL_ALPHA_REG;              /*!< Offset 0x044 VSU Output Global Alpha Register */
         RESERVED(0x048[0x0080 - 0x0048], uint8_t)
    __IOM uint32_t UIS_Y_INSIZE_REG;                  /*!< Offset 0x080 Y Channel Input size register */
         RESERVED(0x084[0x0088 - 0x0084], uint8_t)
    __IOM uint32_t UIS_Y_HSTEP_REG;                   /*!< Offset 0x088 Y Channel Horizontal step register */
    __IOM uint32_t UIS_Y_VSTEP_REG;                   /*!< Offset 0x08C Y Channel Vertical step register */
    __IOM uint32_t UIS_Y_HPHASE_REG;                  /*!< Offset 0x090 Y Channel Horizontal initial phase register */
         RESERVED(0x094[0x0098 - 0x0094], uint8_t)
    __IOM uint32_t UIS_Y_VPHASE_REG;                  /*!< Offset 0x098 Y Channel Vertical initial phase 0 register */
         RESERVED(0x09C[0x00C0 - 0x009C], uint8_t)
    __IOM uint32_t UIS_C_INSIZE_REG;                  /*!< Offset 0x0C0 C Channel Input size register */
         RESERVED(0x0C4[0x00C8 - 0x00C4], uint8_t)
    __IOM uint32_t UIS_C_HSTEP_REG;                   /*!< Offset 0x0C8 C Channel Horizontal step register */
    __IOM uint32_t UIS_C_VSTEP_REG;                   /*!< Offset 0x0CC C Channel Vertical step register */
    __IOM uint32_t UIS_C_HPHASE_REG;                  /*!< Offset 0x0D0 C Channel Horizontal initial phase register */
         RESERVED(0x0D4[0x00D8 - 0x00D4], uint8_t)
    __IOM uint32_t UIS_C_VPHASE_REG;                  /*!< Offset 0x0D8 C Channel Vertical initial phase 0 register */
         RESERVED(0x0DC[0x0200 - 0x00DC], uint8_t)
    __IOM uint32_t UIS_Y_HCOEF0_REGN [0x040];         /*!< Offset 0x200 0x200+N*4 Y Channel Horizontal Filter Coefficient0 Register N N = M 1)) */
         RESERVED(0x300[0x0400 - 0x0300], uint8_t)
    __IOM uint32_t UIS_Y_VCOEF_REGN [0x040];          /*!< Offset 0x400 0x400+N*4 Y Channel Vertical Filter Coefficient Register N N = M 1)) */
         RESERVED(0x500[0x0600 - 0x0500], uint8_t)
    __IOM uint32_t UIS_C_HCOEF0_REGN [0x040];         /*!< Offset 0x600 0x600+N*4 C Channel Horizontal Filter Coefficient0 Register N N = M 1)) */
} DE_UIS_TypeDef; /* size of structure = 0x700 */
/*
 * @brief DE_VI
 */
/*!< DE_VI Display Engine (DE) - VI surface */
typedef struct DE_VI_Type
{
    struct
    {
        __IOM uint32_t ATTR;                          /*!< Offset 0x000  */
        __IOM uint32_t SIZE;                          /*!< Offset 0x004  */
        __IOM uint32_t COORD;                         /*!< Offset 0x008  */
        __IOM uint32_t PITCH [0x003];                 /*!< Offset 0x00C ix=0: Y, ix=1: U/UV channel, ix=3: V channel  */
        __IOM uint32_t TOP_LADDR [0x003];             /*!< Offset 0x018  */
        __IOM uint32_t BOT_LADDR [0x003];             /*!< Offset 0x024  */
    } CFG [0x004];                                    /*!< Offset 0x000  */
    __IOM uint32_t FCOLOR [0x004];                    /*!< Offset 0x0C0  */
    __IOM uint32_t TOP_HADDR [0x003];                 /*!< Offset 0x0D0  */
    __IOM uint32_t BOT_HADDR [0x003];                 /*!< Offset 0x0DC  */
    __IOM uint32_t OVL_SIZE [0x002];                  /*!< Offset 0x0E8 OVL_Y, OVL_UV overlay window size register */
    __IOM uint32_t HORI [0x002];                      /*!< Offset 0x0F0 OVL_V horizontal down sample control register */
    __IOM uint32_t VERT [0x002];                      /*!< Offset 0x0F8 OVL_V vertical down sample control register */
         RESERVED(0x100[0x0300 - 0x0100], uint8_t)
    __IOM uint32_t FBD_V_CTL;                         /*!< Offset 0x300 OVL_V FBD control register */
         RESERVED(0x304[0x0800 - 0x0304], uint8_t)
} DE_VI_TypeDef; /* size of structure = 0x800 */
/*
 * @brief DE_VSU
 */
/*!< DE_VSU Video Scaler Unit (VSU) */
typedef struct DE_VSU_Type
{
    __IOM uint32_t VSU_CTRL_REG;                      /*!< Offset 0x000 VSU Module Control Register */
         RESERVED(0x004[0x0008 - 0x0004], uint8_t)
    __IOM uint32_t VSU_STATUS_REG;                    /*!< Offset 0x008 VSU Status Register */
    __IOM uint32_t VSU_FIELD_CTRL_REG;                /*!< Offset 0x00C VSU Field Control Register */
    __IOM uint32_t VSU_SCALE_MODE_REG;                /*!< Offset 0x010 VSU Scale Mode Setting Register */
         RESERVED(0x014[0x0020 - 0x0014], uint8_t)
    __IOM uint32_t VSU_DIRECTION_THR_REG;             /*!< Offset 0x020 VSU Direction Detection Threshold Register */
    __IOM uint32_t VSU_EDGE_THR_REG;                  /*!< Offset 0x024 VSU Edge Detection Setting Register */
    __IOM uint32_t VSU_EDSCALER_CTRL_REG;             /*!< Offset 0x028 VSU Edge-Direction Scaler Control Register */
    __IOM uint32_t VSU_ANGLE_THR_REG;                 /*!< Offset 0x02C VSU Angle Reliability Setting Register */
    __IOM uint32_t VSU_SHARP_EN_REG;                  /*!< Offset 0x030 VSU Sharpness Control Enable Register */
    __IOM uint32_t VSU_SHARP_CORING_REG;              /*!< Offset 0x034 VSU Sharpness Control Coring Setting Register */
    __IOM uint32_t VSU_SHARP_GAIN0_REG;               /*!< Offset 0x038 VSU Sharpness Control Gain Setting 0 Register */
    __IOM uint32_t VSU_SHARP_GAIN1_REG;               /*!< Offset 0x03C VSU Sharpness Control Gain Setting 1 Register */
    __IOM uint32_t VSU_OUT_SIZE_REG;                  /*!< Offset 0x040 VSU Output Size Register */
    __IOM uint32_t VSU_GLOBAL_ALPHA_REG;              /*!< Offset 0x044 VSU Output Global Alpha Register */
         RESERVED(0x048[0x0080 - 0x0048], uint8_t)
    __IOM uint32_t VSU_Y_SIZE_REG;                    /*!< Offset 0x080 VSU Y Channel Size Register */
         RESERVED(0x084[0x0088 - 0x0084], uint8_t)
    __IOM uint32_t VSU_Y_HSTEP_REG;                   /*!< Offset 0x088 VSU Y Channel Horizontal Step Register */
    __IOM uint32_t VSU_Y_VSTEP_REG;                   /*!< Offset 0x08C VSU Y Channel Vertical Step Register */
    __IOM uint32_t VSU_Y_HPHASE_REG;                  /*!< Offset 0x090 VSU Y Channel Horizontal Initial Phase Register */
         RESERVED(0x094[0x0098 - 0x0094], uint8_t)
    __IOM uint32_t VSU_Y_VPHASE0_REG;                 /*!< Offset 0x098 VSU Y Channel Vertical Initial Phase 0 Register */
    __IOM uint32_t VSU_Y_VPHASE1_REG;                 /*!< Offset 0x09C VSU Y Channel Vertical Initial Phase 1 Register */
         RESERVED(0x0A0[0x00C0 - 0x00A0], uint8_t)
    __IOM uint32_t VSU_C_SIZE_REG;                    /*!< Offset 0x0C0 VSU C Channel Size Register */
         RESERVED(0x0C4[0x00C8 - 0x00C4], uint8_t)
    __IOM uint32_t VSU_C_HSTEP_REG;                   /*!< Offset 0x0C8 VSU C Channel Horizontal Step Register */
    __IOM uint32_t VSU_C_VSTEP_REG;                   /*!< Offset 0x0CC VSU C Channel Vertical Step Register */
    __IOM uint32_t VSU_C_HPHASE_REG;                  /*!< Offset 0x0D0 VSU C Channel Horizontal Initial Phase Register */
         RESERVED(0x0D4[0x00D8 - 0x00D4], uint8_t)
    __IOM uint32_t VSU_C_VPHASE0_REG;                 /*!< Offset 0x0D8 VSU C Channel Vertical Initial Phase 0 Register */
    __IOM uint32_t VSU_C_VPHASE1_REG;                 /*!< Offset 0x0DC VSU C Channel Vertical Initial Phase 1 Register */
         RESERVED(0x0E0[0x0200 - 0x00E0], uint8_t)
    __IOM uint32_t VSU_Y_HCOEF0_REGN [0x040];         /*!< Offset 0x200 0x200+N*4 VSU Y Channel Horizontal Filter Coefficient0 Register N N = M 1)) */
         RESERVED(0x300[0x0400 - 0x0300], uint8_t)
    __IOM uint32_t VSU_Y_VCOEF_REGN [0x040];          /*!< Offset 0x400 0x400+N*4 VSU Y Channel Vertical Filter Coefficient Register N N = M 1)) */
         RESERVED(0x500[0x0600 - 0x0500], uint8_t)
    __IOM uint32_t VSU_C_HCOEF0_REGN [0x040];         /*!< Offset 0x600 0x600+N*4 VSU C Channel Horizontal Filter Coefficient0 Register N N = M 1)) */
} DE_VSU_TypeDef; /* size of structure = 0x700 */
/*
 * @brief DISPLAY0_TOP
 */
/*!< DISPLAY0_TOP VIDE0_OUT1_SYS */
typedef struct DISPLAY0_TOP_Type
{
         RESERVED(0x000[0x0004 - 0x0000], uint8_t)
    __IOM uint32_t DSI_SRC_SEL;                       /*!< Offset 0x004 MIPI DSI Data Source Select Register */
    __IOM uint32_t DUAL_DSI_SKEW;                     /*!< Offset 0x008 Dual MIPI DSI Data Skew Register */
    __IOM uint32_t LCD_CLK_SRC_COMB_CLK_SRC;          /*!< Offset 0x00C LCD Clock And Combophy Clock Source Select Register */
         RESERVED(0x010[0x0020 - 0x0010], uint8_t)
    __IOM uint32_t VO0_MODULE_GATING;                 /*!< Offset 0x020 TCON_LCD Output To LVDS/DSI Gating Register */
} DISPLAY0_TOP_TypeDef; /* size of structure = 0x024 */
/*
 * @brief DISPLAY1_TOP
 */
/*!< DISPLAY1_TOP VIDEO_OUT1_SYS */
typedef struct DISPLAY1_TOP_Type
{
    __IOM uint32_t TV_CLK_EDP_I2S1_SRC;               /*!< Offset 0x000 TCON_TV Clock Select And EDP I2S1 Select Register */
         RESERVED(0x004[0x0020 - 0x0004], uint8_t)
    __IOM uint32_t VO1_MODULE_GATING;                 /*!< Offset 0x020 TCON_TV Output To HDMI Gating Register */
} DISPLAY1_TOP_TypeDef; /* size of structure = 0x024 */
/*
 * @brief DMAC
 */
/*!< DMAC  */
typedef struct DMAC_Type
{
         RESERVED(0x000[0x0020 - 0x0000], uint8_t)
    __IOM uint32_t DMAC_SEC_REG;                      /*!< Offset 0x020 DMA Security Register */
         RESERVED(0x024[0x0028 - 0x0024], uint8_t)
    __IOM uint32_t DMAC_AUTO_GATE_REG;                /*!< Offset 0x028 DMAC Auto Gating Register */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __IM  uint32_t DMAC_STA_REG;                      /*!< Offset 0x030 DMAC Status Register */
    __IOM uint32_t DMA_IRQ_CPU_EN_REG;                /*!< Offset 0x034 DMA IRQ Transfer to CPU Field Enable Register */
    __IOM uint32_t DMA_IRQ_CPUS_EN_REG;               /*!< Offset 0x038 DMA IRQ Transfer to CPUS Field Enable Register */
         RESERVED(0x03C[0x0100 - 0x003C], uint8_t)
    struct
    {
        __IOM uint32_t DMAC_EN_REGN;                  /*!< Offset 0x100 DMAC Channel Enable Register N (N = 0 to 15) 0x0100 + N*0x0040 */
        __IOM uint32_t DMAC_PAU_REGN;                 /*!< Offset 0x104 DMAC Channel Pause Register N (N = 0 to 15) 0x0104 + N*0x0040 */
        __IOM uint32_t DMAC_DESC_ADDR_REGN;           /*!< Offset 0x108 DMAC Channel Start Address Register N (N = 0 to 15) 0x0108 + N*0x0040 */
        __IM  uint32_t DMAC_CFG_REGN;                 /*!< Offset 0x10C DMAC Channel Configuration Register N (N = 0 to 15) 0x010C + N*0x0040 */
        __IM  uint32_t DMAC_CUR_SRC_REGN;             /*!< Offset 0x110 DMAC Channel Current Source Register N (N = 0 to 15) 0x0110 + N*0x0040 */
        __IM  uint32_t DMAC_CUR_DEST_REGN;            /*!< Offset 0x114 DMAC Channel Current Destination Register N (N = 0 to 15) 0x0114 + N*0x0040 */
        __IM  uint32_t DMAC_BCNT_LEFT_REGN;           /*!< Offset 0x118 DMAC Channel Byte Counter Left Register N (N = 0 to 15) 0x0118 + N*0x0040 */
        __IM  uint32_t DMAC_PARA_REGN;                /*!< Offset 0x11C DMAC Channel Parameter Register N (N = 0 to 15) 0x011C + N*0x0040 */
             RESERVED(0x020[0x0028 - 0x0020], uint8_t)
        __IOM uint32_t DMAC_MODE_REGN;                /*!< Offset 0x128 DMAC Mode Register N (N = 0 to 15) 0x0128 + N*0x0040 */
        __IM  uint32_t DMAC_FDESC_ADDR_REGN;          /*!< Offset 0x12C DMAC Former Descriptor Address Register N (N = 0 to 15) 0x012C + N*0x0040 */
        __IM  uint32_t DMAC_PKG_NUM_REGN;             /*!< Offset 0x130 DMAC Package Number Register N (N = 0 to 15) 0x0130 + N*0x0040 */
             RESERVED(0x034[0x0040 - 0x0034], uint8_t)
    } CH [0x010];                                     /*!< Offset 0x100 Channel [0..15] */
         RESERVED(0x500[0x1000 - 0x0500], uint8_t)
} DMAC_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief G2D_BLD
 */
/*!< G2D_BLD Graphic 2D (G2D) Engine Blender */
typedef struct G2D_BLD_Type
{
    __IOM uint32_t BLD_FILL_COLOR_CTL;                /*!< Offset 0x000 BLD_EN_CTL BLD_FILL_COLOR_CTL Allwinner_DE2.0_Spec_V1.0.pdf page 106   */
         RESERVED(0x004[0x0010 - 0x0004], uint8_t)
    __IOM uint32_t BLD_FILL_COLOR [0x002];            /*!< Offset 0x010 BLD_FILLC  N=0..1  */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IOM uint32_t BLD_CH_ISIZE [0x002];              /*!< Offset 0x020 BLD_CH_ISIZE N=0..1   */
         RESERVED(0x028[0x0030 - 0x0028], uint8_t)
    __IOM uint32_t BLD_CH_OFFSET [0x002];             /*!< Offset 0x030 BLD_CH_OFFSET N=0..1 */
         RESERVED(0x038[0x0040 - 0x0038], uint8_t)
    __IOM uint32_t BLD_PREMUL_CTL;                    /*!< Offset 0x040 BLD_PREMUL_CTL      */
    __IOM uint32_t BLD_BK_COLOR;                      /*!< Offset 0x044 BLD_BK_COLOR        */
    __IOM uint32_t BLD_SIZE;                          /*!< Offset 0x048 BLD_SIZE            */
    __IOM uint32_t BLD_CTL;                           /*!< Offset 0x04C BLD_CTL             */
    __IOM uint32_t BLD_KEY_CTL;                       /*!< Offset 0x050 BLD_KEY_CTL         */
    __IOM uint32_t BLD_KEY_CON;                       /*!< Offset 0x054 BLD_KEY_CON         */
    __IOM uint32_t BLD_KEY_MAX;                       /*!< Offset 0x058 BLD_KEY_MAX         */
    __IOM uint32_t BLD_KEY_MIN;                       /*!< Offset 0x05C BLD_KEY_MIN         */
    __IOM uint32_t BLD_OUT_COLOR;                     /*!< Offset 0x060 BLD_OUT_COLOR       */
         RESERVED(0x064[0x0080 - 0x0064], uint8_t)
    __IOM uint32_t ROP_CTL;                           /*!< Offset 0x080 ROP_CTL             */
    __IOM uint32_t ROP_INDEX [0x004];                 /*!< Offset 0x084 ROP_INDEX N=0..1 ch3_index0 (tina linux sources has two elements) */
         RESERVED(0x094[0x0100 - 0x0094], uint8_t)
    __IOM uint32_t BLD_CSC_CTL;                       /*!< Offset 0x100 BLD_CSC_CTL         */
         RESERVED(0x104[0x0110 - 0x0104], uint8_t)
    __IOM uint32_t BLD_CSC0_COEF00;                   /*!< Offset 0x110 BLD_CSC0_COEF00     */
    __IOM uint32_t BLD_CSC0_COEF01;                   /*!< Offset 0x114 BLD_CSC0_COEF01     */
    __IOM uint32_t BLD_CSC0_COEF02;                   /*!< Offset 0x118 BLD_CSC0_COEF02     */
    __IOM uint32_t BLD_CSC0_CONST0;                   /*!< Offset 0x11C BLD_CSC0_CONST0     */
    __IOM uint32_t BLD_CSC0_COEF10;                   /*!< Offset 0x120 BLD_CSC0_COEF10     */
    __IOM uint32_t BLD_CSC0_COEF11;                   /*!< Offset 0x124 BLD_CSC0_COEF11     */
    __IOM uint32_t BLD_CSC0_COEF12;                   /*!< Offset 0x128 BLD_CSC0_COEF12     */
    __IOM uint32_t BLD_CSC0_CONST1;                   /*!< Offset 0x12C BLD_CSC0_CONST1     */
    __IOM uint32_t BLD_CSC0_COEF20;                   /*!< Offset 0x130 BLD_CSC0_COEF20     */
    __IOM uint32_t BLD_CSC0_COEF21;                   /*!< Offset 0x134 BLD_CSC0_COEF21     */
    __IOM uint32_t BLD_CSC0_COEF22;                   /*!< Offset 0x138 BLD_CSC0_COEF22     */
    __IOM uint32_t BLD_CSC0_CONST2;                   /*!< Offset 0x13C BLD_CSC0_CONST2     */
    __IOM uint32_t BLD_CSC1_COEF00;                   /*!< Offset 0x140 BLD_CSC1_COEF00     */
    __IOM uint32_t BLD_CSC1_COEF01;                   /*!< Offset 0x144 BLD_CSC1_COEF01     */
    __IOM uint32_t BLD_CSC1_COEF02;                   /*!< Offset 0x148 BLD_CSC1_COEF02     */
    __IOM uint32_t BLD_CSC1_CONST0;                   /*!< Offset 0x14C BLD_CSC1_CONST0     */
    __IOM uint32_t BLD_CSC1_COEF10;                   /*!< Offset 0x150 BLD_CSC1_COEF10     */
    __IOM uint32_t BLD_CSC1_COEF11;                   /*!< Offset 0x154 BLD_CSC1_COEF11     */
    __IOM uint32_t BLD_CSC1_COEF12;                   /*!< Offset 0x158 BLD_CSC1_COEF12     */
    __IOM uint32_t BLD_CSC1_CONST1;                   /*!< Offset 0x15C BLD_CSC1_CONST1     */
    __IOM uint32_t BLD_CSC1_COEF20;                   /*!< Offset 0x160 BLD_CSC1_COEF20     */
    __IOM uint32_t BLD_CSC1_COEF21;                   /*!< Offset 0x164 BLD_CSC1_COEF21     */
    __IOM uint32_t BLD_CSC1_COEF22;                   /*!< Offset 0x168 BLD_CSC1_COEF22     */
    __IOM uint32_t BLD_CSC1_CONST2;                   /*!< Offset 0x16C BLD_CSC1_CONST2     */
    __IOM uint32_t BLD_CSC2_COEF00;                   /*!< Offset 0x170 BLD_CSC2_COEF00     */
    __IOM uint32_t BLD_CSC2_COEF01;                   /*!< Offset 0x174 BLD_CSC2_COEF01     */
    __IOM uint32_t BLD_CSC2_COEF02;                   /*!< Offset 0x178 BLD_CSC2_COEF02     */
    __IOM uint32_t BLD_CSC2_CONST0;                   /*!< Offset 0x17C BLD_CSC2_CONST0     */
    __IOM uint32_t BLD_CSC2_COEF10;                   /*!< Offset 0x180 BLD_CSC2_COEF10     */
    __IOM uint32_t BLD_CSC2_COEF11;                   /*!< Offset 0x184 BLD_CSC2_COEF11     */
    __IOM uint32_t BLD_CSC2_COEF12;                   /*!< Offset 0x188 BLD_CSC2_COEF12     */
    __IOM uint32_t BLD_CSC2_CONST1;                   /*!< Offset 0x18C BLD_CSC2_CONST1     */
    __IOM uint32_t BLD_CSC2_COEF20;                   /*!< Offset 0x190 BLD_CSC2_COEF20     */
    __IOM uint32_t BLD_CSC2_COEF21;                   /*!< Offset 0x194 BLD_CSC2_COEF21     */
    __IOM uint32_t BLD_CSC2_COEF22;                   /*!< Offset 0x198 BLD_CSC2_COEF22     */
    __IOM uint32_t BLD_CSC2_CONST2;                   /*!< Offset 0x19C BLD_CSC2_CONST2     */
} G2D_BLD_TypeDef; /* size of structure = 0x1A0 */
/*
 * @brief G2D_MIXER
 */
/*!< G2D_MIXER Graphic 2D (G2D) Engine Video Mixer */
typedef struct G2D_MIXER_Type
{
    __IOM uint32_t G2D_MIXER_CTRL;                    /*!< Offset 0x000 G2D mixer control */
    __IOM uint32_t G2D_MIXER_INTERRUPT;               /*!< Offset 0x004 G2D mixer interrupt */
    __IOM uint32_t G2D_MIXER_CLK;                     /*!< Offset 0x008 G2D mixer clock */
} G2D_MIXER_TypeDef; /* size of structure = 0x00C */
/*
 * @brief G2D_ROT
 */
/*!< G2D_ROT Graphic 2D Rotate */
typedef struct G2D_ROT_Type
{
    __IOM uint32_t ROT_CTL;                           /*!< Offset 0x000 ROT_CTL						 */
    __IOM uint32_t ROT_INT;                           /*!< Offset 0x004 ROT_INT				 */
    __IOM uint32_t ROT_TIMEOUT;                       /*!< Offset 0x008 ROT_TIMEOUT */
         RESERVED(0x00C[0x0020 - 0x000C], uint8_t)
    __IOM uint32_t ROT_IFMT;                          /*!< Offset 0x020 Input data attribute register */
    __IOM uint32_t ROT_ISIZE;                         /*!< Offset 0x024 Input data size register */
         RESERVED(0x028[0x0030 - 0x0028], uint8_t)
    __IOM uint32_t ROT_IPITCH0;                       /*!< Offset 0x030 Input Y/RGB/ARGB memory pitch register */
    __IOM uint32_t ROT_IPITCH1;                       /*!< Offset 0x034 Input U/UV memory pitch register */
    __IOM uint32_t ROT_IPITCH2;                       /*!< Offset 0x038 Input V memory pitch register */
         RESERVED(0x03C[0x0040 - 0x003C], uint8_t)
    __IOM uint32_t ROT_ILADD0;                        /*!< Offset 0x040 Input Y/RGB/ARGB memory address register0 */
    __IOM uint32_t ROT_IHADD0;                        /*!< Offset 0x044 Input Y/RGB/ARGB memory address register1 */
    __IOM uint32_t ROT_ILADD1;                        /*!< Offset 0x048 Input U/UV memory address register0 */
    __IOM uint32_t ROT_IHADD1;                        /*!< Offset 0x04C Input U/UV memory address register1 */
    __IOM uint32_t ROT_ILADD2;                        /*!< Offset 0x050 Input V memory address register0 */
    __IOM uint32_t ROT_IHADD2;                        /*!< Offset 0x054 Input V memory address register1 */
         RESERVED(0x058[0x0084 - 0x0058], uint8_t)
    __IOM uint32_t ROT_OSIZE;                         /*!< Offset 0x084 Output data size register */
         RESERVED(0x088[0x0090 - 0x0088], uint8_t)
    __IOM uint32_t ROT_OPITCH0;                       /*!< Offset 0x090 Output Y/RGB/ARGB memory pitch register */
    __IOM uint32_t ROT_OPITCH1;                       /*!< Offset 0x094 Output U/UV memory pitch register */
    __IOM uint32_t ROT_OPITCH2;                       /*!< Offset 0x098 Output V memory pitch register */
         RESERVED(0x09C[0x00A0 - 0x009C], uint8_t)
    __IOM uint32_t ROT_OLADD0;                        /*!< Offset 0x0A0 Output Y/RGB/ARGB memory address register0 */
    __IOM uint32_t ROT_OHADD0;                        /*!< Offset 0x0A4 Output Y/RGB/ARGB memory address register1 */
    __IOM uint32_t ROT_OLADD1;                        /*!< Offset 0x0A8 Output U/UV memory address register0 */
    __IOM uint32_t ROT_OHADD1;                        /*!< Offset 0x0AC Output U/UV memory address register1 */
    __IOM uint32_t ROT_OLADD2;                        /*!< Offset 0x0B0 Output V memory address register0 */
    __IOM uint32_t ROT_OHADD2;                        /*!< Offset 0x0B4 Output V memory address register1 */
         RESERVED(0x0B8[0x00C8 - 0x00B8], uint8_t)
    __IOM uint32_t LBC_ENC_CTL;                       /*!< Offset 0x0C8 Encode control (bit 31 - is lossy) */
    __IOM uint32_t LBC_CTL;                           /*!< Offset 0x0CC  */
    __IOM uint32_t LBC_DEC_CTL;                       /*!< Offset 0x0D0 Decode control (bit 31 - is lossy)  */
} G2D_ROT_TypeDef; /* size of structure = 0x0D4 */
/*
 * @brief G2D_TOP
 */
/*!< G2D_TOP Graphic 2D top */
typedef struct G2D_TOP_Type
{
    __IOM uint32_t G2D_SCLK_GATE;                     /*!< Offset 0x000 G2D SCLK gate   */
    __IOM uint32_t G2D_HCLK_GATE;                     /*!< Offset 0x004 g2d HCLK gate   */
    __IOM uint32_t G2D_AHB_RST;                       /*!< Offset 0x008 G2D AHB reset   */
    __IOM uint32_t G2D_SCLK_DIV;                      /*!< Offset 0x00C G2D SCLK div    */
    __IM  uint32_t G2D_VERSION;                       /*!< Offset 0x010  */
         RESERVED(0x014[0x0020 - 0x0014], uint8_t)
    __IOM uint32_t RCQ_IRQ_CTL;                       /*!< Offset 0x020  */
    __IOM uint32_t RCQ_STATUS;                        /*!< Offset 0x024  */
    __IOM uint32_t RCQ_CTRL;                          /*!< Offset 0x028  */
    __IOM uint32_t RCQ_HEADER_LOW_ADDR;               /*!< Offset 0x02C Register Configuration Queue Header bits 31..0 */
    __IOM uint32_t RCQ_HEADER_HIGH_ADDR;              /*!< Offset 0x030 Register Configuration Queue Header bits 39..32 */
    __IOM uint32_t RCQ_HEADER_LEN;                    /*!< Offset 0x034 Register Configuration Queue length bits 15..0 */
} G2D_TOP_TypeDef; /* size of structure = 0x038 */
/*
 * @brief G2D_UI
 */
/*!< G2D_UI Graphic 2D UI surface */
typedef struct G2D_UI_Type
{
    __IOM uint32_t UI_ATTR;                           /*!< Offset 0x000 UIx_ATTR */
    __IOM uint32_t UI_MBSIZE;                         /*!< Offset 0x004 UIx_MBSIZE */
    __IOM uint32_t UI_COOR;                           /*!< Offset 0x008 UIx_COOR */
    __IOM uint32_t UI_PITCH;                          /*!< Offset 0x00C UIx_PITCH */
    __IOM uint32_t UI_LADD;                           /*!< Offset 0x010 UIx_LADD */
    __IOM uint32_t UI_FILLC;                          /*!< Offset 0x014 UIx_FILLC */
    __IOM uint32_t UI_HADD;                           /*!< Offset 0x018 UIx_HADD */
    __IOM uint32_t UI_SIZE;                           /*!< Offset 0x01C UIx_SIZE */
} G2D_UI_TypeDef; /* size of structure = 0x020 */
/*
 * @brief G2D_VI
 */
/*!< G2D_VI Graphic 2D VI surface */
typedef struct G2D_VI_Type
{
    __IOM uint32_t V0_ATTCTL;                         /*!< Offset 0x000 V0_ATTCTL */
    __IOM uint32_t V0_MBSIZE;                         /*!< Offset 0x004 Source rectangle size (may be empty) */
    __IOM uint32_t V0_COOR;                           /*!< Offset 0x008 Target window position */
    __IOM uint32_t V0_PITCH0;                         /*!< Offset 0x00C V0_PITCH0 */
    __IOM uint32_t V0_PITCH1;                         /*!< Offset 0x010 V0_PITCH1 */
    __IOM uint32_t V0_PITCH2;                         /*!< Offset 0x014 V0_PITCH2 */
    __IOM uint32_t V0_LADD0;                          /*!< Offset 0x018 V0_LADD0 */
    __IOM uint32_t V0_LADD1;                          /*!< Offset 0x01C V0_LADD1 */
    __IOM uint32_t V0_LADD2;                          /*!< Offset 0x020 V0_LADD2 */
    __IOM uint32_t V0_FILLC;                          /*!< Offset 0x024 V0_FILLC */
    __IOM uint32_t V0_HADD;                           /*!< Offset 0x028 [31:24]: 0, [23:16]: LADD2 bits 40..32, [15:8]: LADD1 bits 40..32, [7:0]: LADD0 bits 40..32 */
    __IOM uint32_t V0_SIZE;                           /*!< Offset 0x02C Source window size (may not be empty) */
    __IOM uint32_t V0_HDS_CTL0;                       /*!< Offset 0x030 V0_HDS_CTL0 */
    __IOM uint32_t V0_HDS_CTL1;                       /*!< Offset 0x034 V0_HDS_CTL1 */
    __IOM uint32_t V0_VDS_CTL0;                       /*!< Offset 0x038 V0_VDS_CTL0 */
    __IOM uint32_t V0_VDS_CTL1;                       /*!< Offset 0x03C V0_VDS_CTL1 */
} G2D_VI_TypeDef; /* size of structure = 0x040 */
/*
 * @brief G2D_VSU
 */
/*!< G2D_VSU Graphic 2D Video Scaler */
typedef struct G2D_VSU_Type
{
    __IOM uint32_t VS_CTRL;                           /*!< Offset 0x000 VS_CTRL */
         RESERVED(0x004[0x0040 - 0x0004], uint8_t)
    __IOM uint32_t VS_OUT_SIZE;                       /*!< Offset 0x040 VS_OUT_SIZE */
    __IOM uint32_t VS_GLB_ALPHA;                      /*!< Offset 0x044 VS_GLB_ALPHA */
         RESERVED(0x048[0x0080 - 0x0048], uint8_t)
    __IOM uint32_t VS_Y_SIZE;                         /*!< Offset 0x080 VS_Y_SIZE */
         RESERVED(0x084[0x0088 - 0x0084], uint8_t)
    __IOM uint32_t VS_Y_HSTEP;                        /*!< Offset 0x088 VS_Y_HSTEP */
    __IOM uint32_t VS_Y_VSTEP;                        /*!< Offset 0x08C VS_Y_VSTEP */
    __IOM uint32_t VS_Y_HPHASE;                       /*!< Offset 0x090 VS_Y_HPHASE */
         RESERVED(0x094[0x0098 - 0x0094], uint8_t)
    __IOM uint32_t VS_Y_VPHASE0;                      /*!< Offset 0x098 VS_Y_VPHASE0 */
         RESERVED(0x09C[0x00C0 - 0x009C], uint8_t)
    __IOM uint32_t VS_C_SIZE;                         /*!< Offset 0x0C0 VS_C_SIZE */
         RESERVED(0x0C4[0x00C8 - 0x00C4], uint8_t)
    __IOM uint32_t VS_C_HSTEP;                        /*!< Offset 0x0C8 VS_C_HSTEP */
    __IOM uint32_t VS_C_VSTEP;                        /*!< Offset 0x0CC VS_C_VSTEP */
    __IOM uint32_t VS_C_HPHASE;                       /*!< Offset 0x0D0 VS_C_HPHASE */
         RESERVED(0x0D4[0x00D8 - 0x00D4], uint8_t)
    __IOM uint32_t VS_C_VPHASE0;                      /*!< Offset 0x0D8 VS_C_VPHASE0 */
         RESERVED(0x0DC[0x0200 - 0x00DC], uint8_t)
    __IOM uint32_t VS_Y_HCOEF [0x040];                /*!< Offset 0x200 VS_Y_HCOEF[N]. N=0..31 */
    __IOM uint32_t VS_Y_VCOEF [0x040];                /*!< Offset 0x300 VS_Y_VCOEF[N]. N=0..31 */
    __IOM uint32_t VS_C_HCOEF [0x040];                /*!< Offset 0x400 VS_C_HCOEF[N]. N=0..31 */
} G2D_VSU_TypeDef; /* size of structure = 0x500 */
/*
 * @brief G2D_WB
 */
/*!< G2D_WB Graphic 2D (G2D) Engine Write Back */
typedef struct G2D_WB_Type
{
    __IOM uint32_t WB_ATT;                            /*!< Offset 0x000 WB_ATT */
    __IOM uint32_t WB_SIZE;                           /*!< Offset 0x004 WB_SIZE */
    __IOM uint32_t WB_PITCH0;                         /*!< Offset 0x008 WB_PITCH0 */
    __IOM uint32_t WB_PITCH1;                         /*!< Offset 0x00C WB_PITCH1 */
    __IOM uint32_t WB_PITCH2;                         /*!< Offset 0x010 WB_PITCH2 */
    __IOM uint32_t WB_LADD0;                          /*!< Offset 0x014 WB_LADD0 */
    __IOM uint32_t WB_HADD0;                          /*!< Offset 0x018 WB_HADD0 */
    __IOM uint32_t WB_LADD1;                          /*!< Offset 0x01C WB_LADD1 */
    __IOM uint32_t WB_HADD1;                          /*!< Offset 0x020 WB_HADD1 */
    __IOM uint32_t WB_LADD2;                          /*!< Offset 0x024 WB_LADD2 */
    __IOM uint32_t WB_HADD2;                          /*!< Offset 0x028 WB_HADD2 */
} G2D_WB_TypeDef; /* size of structure = 0x02C */
/*
 * @brief GICA
 */
/*!< GICA Distributor registers (GICA) for message-based SPIs */
typedef struct GICA_Type
{
         RESERVED(0x000[0x0040 - 0x0000], uint8_t)
    __OM  uint32_t GICA_SETSPI_NSR;                   /*!< Offset 0x040 Aliased Non-secure SPI Set Register */
         RESERVED(0x044[0x0048 - 0x0044], uint8_t)
    __OM  uint32_t GICA_CLRSPI_NSR;                   /*!< Offset 0x048 Aliased Non-secure SPI Clear Register */
         RESERVED(0x04C[0x0050 - 0x004C], uint8_t)
    __OM  uint32_t GICA_SETSPI_SR;                    /*!< Offset 0x050 Aliased Secure SPI Set Register */
         RESERVED(0x054[0x0058 - 0x0054], uint8_t)
    __OM  uint32_t GICA_CLRSPI_SR;                    /*!< Offset 0x058 Aliased Secure SPI Clear Register */
         RESERVED(0x05C[0x10000 - 0x005C], uint8_t)
} GICA_TypeDef; /* size of structure = 0x10000 */
/*
 * @brief GICD
 */
/*!< GICD GIC Distributor */
typedef struct GICD_Type
{
    __IOM uint32_t GICD_CTLR;                         /*!< Offset 0x000 Distributor Control Register */
    __IM  uint32_t GICD_TYPER;                        /*!< Offset 0x004 Distributor Controller Type Register */
    __IM  uint32_t GICD_IIDR;                         /*!< Offset 0x008 Distributor Implementer Identification Register */
         RESERVED(0x00C[0x0020 - 0x000C], uint8_t)
    __IOM uint32_t GICD_FCTLR;                        /*!< Offset 0x020 Function Control Register */
    __IOM uint32_t GICD_SAC;                          /*!< Offset 0x024 Secure Access Control Register */
         RESERVED(0x028[0x0040 - 0x0028], uint8_t)
    __OM  uint32_t GICD_SETSPI_NSR;                   /*!< Offset 0x040 Non-secure SPI Set Register */
         RESERVED(0x044[0x0048 - 0x0044], uint8_t)
    __OM  uint32_t GICD_CLRSPI_NSR;                   /*!< Offset 0x048 Non-secure SPI Clear Register */
         RESERVED(0x04C[0x0050 - 0x004C], uint8_t)
    __OM  uint32_t GICD_SETSPI_SR;                    /*!< Offset 0x050 Secure SPI Set Register */
         RESERVED(0x054[0x0058 - 0x0054], uint8_t)
    __OM  uint32_t GICD_CLRSPI_SR;                    /*!< Offset 0x058 Non-secure SPI Set Register */
         RESERVED(0x05C[0x0080 - 0x005C], uint8_t)
    __IOM uint32_t GICD_IGROUPR [0x020];              /*!< Offset 0x080 Interrupt Group Registers */
    __IOM uint32_t GICD_ISENABLER [0x020];            /*!< Offset 0x100 Interrupt Set-Enable Registers */
    __IOM uint32_t GICD_ICENABLER [0x020];            /*!< Offset 0x180 Interrupt Clear-Enable Registers */
    __IOM uint32_t GICD_ISPENDR [0x020];              /*!< Offset 0x200 Interrupt Set-Pending Registers */
    __IOM uint32_t GICD_ICPENDR [0x020];              /*!< Offset 0x280 Interrupt Clear-Pending Registers */
    __IOM uint32_t GICD_ISACTIVER [0x020];            /*!< Offset 0x300 Interrupt Set-Active Registers */
    __IOM uint32_t GICD_ICACTIVER [0x020];            /*!< Offset 0x380 Interrupt Clear-Active Registers */
    __IOM uint32_t GICD_IPRIORITYR [0x100];           /*!< Offset 0x400 Interrupt Priority Registers */
    __IOM uint32_t GICD_ITARGETSR [0x100];            /*!< Offset 0x800 Interrupt Processor Targets Registersi */
    __IOM uint32_t GICD_ICFGR [0x040];                /*!< Offset 0xC00 Interrupt Configuration Registers */
    __IOM uint32_t GICD_IGRPMODR [0x020];             /*!< Offset 0xD00 Interrupt Group Modifier Registers */
         RESERVED(0xD80[0x0E00 - 0x0D80], uint8_t)
    __IOM uint32_t GICD_NSACR [0x040];                /*!< Offset 0xE00 Non-secure Access Control Registers */
         RESERVED(0xF00[0x6100 - 0x0F00], uint8_t)
    __IOM uint64_t GICD_IROUTER [0x3DC];              /*!< Offset 0x6100 Interrupt Routing Registers. */
         RESERVED(0x7FE0[0xC000 - 0x7FE0], uint8_t)
    __IOM uint32_t GICD_CHIPSR;                       /*!< Offset 0xC000 Chip Status Register */
    __IOM uint32_t GICD_DCHIPR;                       /*!< Offset 0xC004 Default Chip Register */
    __IOM uint64_t GICD_CHIPRn [0x004];               /*!< Offset 0xC008 Chip Registers */
         RESERVED(0xC028[0xE008 - 0xC028], uint8_t)
    __IOM uint32_t GICD_ICLARn [0x004];               /*!< Offset 0xE008 The first register is GICD_ICLAR2. 4.2.9 Interrupt Class Registers, GICD_ICLARn on page 4-118 */
         RESERVED(0xE018[0xE108 - 0xE018], uint8_t)
    __IOM uint32_t GICD_IERRRn;                       /*!< Offset 0xE108 The first register is GICD_IERRR1. 4.2.10 Interrupt Error Registers, GICD_IERRRn on page 4-119 */
         RESERVED(0xE10C[0xF000 - 0xE10C], uint8_t)
    __IM  uint64_t GICD_CFGID;                        /*!< Offset 0xF000 Configuration ID Register */
         RESERVED(0xF008[0xFFD0 - 0xF008], uint8_t)
    __IM  uint32_t GICD_PIDR4;                        /*!< Offset 0xFFD0 Peripheral ID 4 Register */
    __IM  uint32_t GICD_PIDR5;                        /*!< Offset 0xFFD4 Peripheral ID 5 Register */
    __IM  uint32_t GICD_PIDR6;                        /*!< Offset 0xFFD8 Peripheral ID 6 Register */
    __IM  uint32_t GICD_PIDR7;                        /*!< Offset 0xFFDC Peripheral ID 7 Register */
    __IM  uint32_t GICD_PIDR0;                        /*!< Offset 0xFFE0 Peripheral ID 0 Register */
    __IM  uint32_t GICD_PIDR1;                        /*!< Offset 0xFFE4 Peripheral ID 1 Register */
    __IM  uint32_t GICD_PIDR2;                        /*!< Offset 0xFFE8 Peripheral ID 2 Register */
    __IM  uint32_t GICD_PIDR3;                        /*!< Offset 0xFFEC Peripheral ID 3 Register */
    __IM  uint32_t GICD_CIDR0;                        /*!< Offset 0xFFF0 Component ID 0 Register */
    __IM  uint32_t GICD_CIDR1;                        /*!< Offset 0xFFF4 Component ID 1 Register */
    __IM  uint32_t GICD_CIDR2;                        /*!< Offset 0xFFF8 Component ID 2 Register */
    __IM  uint32_t GICD_CIDR3;                        /*!< Offset 0xFFFC Component ID 3 Register */
} GICD_TypeDef; /* size of structure = 0x10000 */
/*
 * @brief GICP
 */
/*!< GICP GIC Performance Monitoring Unit */
typedef struct GICP_Type
{
    __IOM uint32_t GICP_EVCNTRn [0x005];              /*!< Offset 0x000 Event Counter Registers */
         RESERVED(0x014[0x0400 - 0x0014], uint8_t)
    __IOM uint32_t GICP_EVTYPERn [0x005];             /*!< Offset 0x400 Event Type Configuration Registers */
         RESERVED(0x414[0x0600 - 0x0414], uint8_t)
    __IM  uint32_t GICP_SVRn [0x005];                 /*!< Offset 0x600 Shadow Value Registers */
         RESERVED(0x614[0x0A00 - 0x0614], uint8_t)
    __IOM uint32_t GICP_FRn [0x005];                  /*!< Offset 0xA00 Filter Registers */
         RESERVED(0xA14[0x0C00 - 0x0A14], uint8_t)
    __IOM uint64_t GICP_CNTENSET0;                    /*!< Offset 0xC00 Counter Enable Set Register 0 */
         RESERVED(0xC08[0x0C20 - 0x0C08], uint8_t)
    __IOM uint64_t GICP_CNTENCLR0;                    /*!< Offset 0xC20 Counter Enable Clear Register 0 */
         RESERVED(0xC28[0x0C40 - 0x0C28], uint8_t)
    __IOM uint64_t GICP_INTENSET0;                    /*!< Offset 0xC40 Interrupt Contribution Enable Set Register 0 */
         RESERVED(0xC48[0x0C60 - 0x0C48], uint8_t)
    __IOM uint64_t GICP_INTENCLR0;                    /*!< Offset 0xC60 Interrupt Contribution Enable Clear Register 0 */
         RESERVED(0xC68[0x0C80 - 0x0C68], uint8_t)
    __IOM uint64_t GICP_OVSCLR0;                      /*!< Offset 0xC80 Overflow Status Clear Register 0 */
         RESERVED(0xC88[0x0CC0 - 0x0C88], uint8_t)
    __IOM uint64_t GICP_OVSSET0;                      /*!< Offset 0xCC0 Overflow Status Set Register 0 */
         RESERVED(0xCC8[0x0D88 - 0x0CC8], uint8_t)
    __OM  uint32_t GICP_CAPR;                         /*!< Offset 0xD88 Counter Shadow Value Capture Register */
         RESERVED(0xD8C[0x0E00 - 0x0D8C], uint8_t)
    __IM  uint32_t GICP_CFGR;                         /*!< Offset 0xE00 Configuration Information Register */
    __IOM uint32_t GICP_CR;                           /*!< Offset 0xE04 Control Register */
         RESERVED(0xE08[0x0E50 - 0x0E08], uint8_t)
    __IOM uint32_t GICP_IRQCR;                        /*!< Offset 0xE50 Interrupt Configuration Register */
         RESERVED(0xE54[0xFFD0 - 0x0E54], uint8_t)
    __IM  uint32_t GICP_PIDR4;                        /*!< Offset 0xFFD0 Peripheral ID 4 Register */
    __IM  uint32_t GICP_PIDR5;                        /*!< Offset 0xFFD4 Peripheral ID 5 Register */
    __IM  uint32_t GICP_PIDR6;                        /*!< Offset 0xFFD8 Peripheral ID 6 Register */
    __IM  uint32_t GICP_PIDR7;                        /*!< Offset 0xFFDC Peripheral ID 7 Register */
    __IM  uint32_t GICP_PIDR0;                        /*!< Offset 0xFFE0 Peripheral ID 0 Register */
    __IM  uint32_t GICP_PIDR1;                        /*!< Offset 0xFFE4 Peripheral ID 1 Register */
    __IM  uint32_t GICP_PIDR2;                        /*!< Offset 0xFFE8 Peripheral ID 2 Register */
    __IM  uint32_t GICP_PIDR3;                        /*!< Offset 0xFFEC Peripheral ID 3 Register */
    __IM  uint32_t GICP_CIDR0;                        /*!< Offset 0xFFF0 Component ID 0 Register */
    __IM  uint32_t GICP_CIDR1;                        /*!< Offset 0xFFF4 Component ID 1 Register */
    __IM  uint32_t GICP_CIDR2;                        /*!< Offset 0xFFF8 Component ID 2 Register */
    __IM  uint32_t GICP_CIDR3;                        /*!< Offset 0xFFFC Component ID 3 Register */
} GICP_TypeDef; /* size of structure = 0x10000 */
/*
 * @brief GICR
 */
/*!< GICR GIC Redistributor  */
typedef struct GICR_Type
{
    __IOM uint32_t GICR_CTLR;                         /*!< Offset 0x000 Redistributor Control Register */
    __IM  uint32_t GICR_IIDR;                         /*!< Offset 0x004  */
    __IOM uint32_t GICR_TYPER;                        /*!< Offset 0x008  */
         RESERVED(0x00C[0x0014 - 0x000C], uint8_t)
    __IOM uint32_t GICR_WAKER;                        /*!< Offset 0x014  */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IOM uint32_t GICR_FCTLR;                        /*!< Offset 0x020  */
    __IOM uint32_t GICR_PWRR;                         /*!< Offset 0x024  */
    __IOM uint32_t GICR_CLASS;                        /*!< Offset 0x028  */
         RESERVED(0x02C[0x0040 - 0x002C], uint8_t)
    __IOM uint64_t GICR_SETLPIR;                      /*!< Offset 0x040  */
    __IOM uint64_t GICR_CLRLPIR;                      /*!< Offset 0x048  */
         RESERVED(0x050[0x0070 - 0x0050], uint8_t)
    __IOM uint64_t GICR_PROPBASER;                    /*!< Offset 0x070 Redistributor Properties Base Address Register */
    __IOM uint64_t GICR_PENDBASER;                    /*!< Offset 0x078 Redistributor LPI Pending Table Base Address Register */
         RESERVED(0x080[0x00A0 - 0x0080], uint8_t)
    __IOM uint64_t GICR_INVLPIR;                      /*!< Offset 0x0A0  */
    __IOM uint64_t GICR_INVALLR;                      /*!< Offset 0x0A8  */
         RESERVED(0x0B0[0x00C0 - 0x00B0], uint8_t)
    __IOM uint32_t GICR_SYNCR;                        /*!< Offset 0x0C0  */
         RESERVED(0x0C4[0xFFD0 - 0x00C4], uint8_t)
    __IM  uint32_t GICR_PIDR4;                        /*!< Offset 0xFFD0 Peripheral ID 4 Register */
    __IM  uint32_t GICR_PIDR5;                        /*!< Offset 0xFFD4 Peripheral ID 5 Register */
    __IM  uint32_t GICR_PIDR6;                        /*!< Offset 0xFFD8 Peripheral ID 6 Register */
    __IM  uint32_t GICR_PIDR7;                        /*!< Offset 0xFFDC Peripheral ID 7 Register */
    __IM  uint32_t GICR_PIDR0;                        /*!< Offset 0xFFE0 Peripheral ID 0 Register */
    __IM  uint32_t GICR_PIDR1;                        /*!< Offset 0xFFE4 Peripheral ID 1 Register */
    __IM  uint32_t GICR_PIDR2;                        /*!< Offset 0xFFE8 Peripheral ID 2 Register */
    __IM  uint32_t GICR_PIDR3;                        /*!< Offset 0xFFEC Peripheral ID 3 Register */
    __IM  uint32_t GICR_CIDR0;                        /*!< Offset 0xFFF0 Component ID 0 Register */
    __IM  uint32_t GICR_CIDR1;                        /*!< Offset 0xFFF4 Component ID 1 Register */
    __IM  uint32_t GICR_CIDR2;                        /*!< Offset 0xFFF8 Component ID 2 Register */
    __IM  uint32_t GICR_CIDR3;                        /*!< Offset 0xFFFC Component ID 3 Register */
} GICR_TypeDef; /* size of structure = 0x10000 */
/*
 * @brief GICR_PPI_LPI
 */
/*!< GICR_PPI_LPI GIC Redistributor  */
typedef struct GICR_PPI_LPI_Type
{
         RESERVED(0x000[0x0400 - 0x0000], uint8_t)
    __IOM uint32_t GICR_PPI_LPI_IPRIORITYR [0x008];   /*!< Offset 0x400 Interrupt Priority Registers */
         RESERVED(0x420[0x10000 - 0x0420], uint8_t)
} GICR_PPI_LPI_TypeDef; /* size of structure = 0x10000 */
/*
 * @brief GICT
 */
/*!< GICT GIC ITS translation */
typedef struct GICT_Type
{
    __IOM uint32_t GICT_CTLR;                         /*!< Offset 0x000 Distributor Control Register */
    __IOM uint32_t GICT_TYPER;                        /*!< Offset 0x004 Distributor Controller Type Register */
    __IOM uint32_t GICT_IIDR;                         /*!< Offset 0x008 Distributor Implementer Identification Register */
         RESERVED(0x00C[0x0020 - 0x000C], uint8_t)
    __IOM uint32_t GICT_FCTLR;                        /*!< Offset 0x020 Function Control Register */
         RESERVED(0x024[0x0028 - 0x0024], uint8_t)
    __IOM uint64_t GICT_OPR;                          /*!< Offset 0x028 Operations Register */
    __IOM uint64_t GICT_OPSR;                         /*!< Offset 0x030 Operation Status Register */
         RESERVED(0x038[0x0080 - 0x0038], uint8_t)
    __IOM uint64_t GICT_GICT_CBASER;                  /*!< Offset 0x080 Command Queue Control Register */
    __IOM uint64_t GICT_CWRITER;                      /*!< Offset 0x088 Command Queue Write Pointer Register */
    __IOM uint64_t GICT_CREADR;                       /*!< Offset 0x090 Command Queue Read Pointer Register */
         RESERVED(0x098[0x0100 - 0x0098], uint8_t)
    __IOM uint64_t GICT_BASER0;                       /*!< Offset 0x100 ITS Translation Table Descriptor Register0 */
    __IOM uint64_t GICT_BASER1;                       /*!< Offset 0x108 ITS Translation Table Descriptor Register0 */
         RESERVED(0x110[0xF000 - 0x0110], uint8_t)
    __IM  uint64_t GICT_CFGID;                        /*!< Offset 0xF000 Configuration ID Register */
         RESERVED(0xF008[0xFFD0 - 0xF008], uint8_t)
    __IM  uint32_t GICT_PIDR4;                        /*!< Offset 0xFFD0 Peripheral ID 4 Register */
    __IM  uint32_t GICT_PIDR5;                        /*!< Offset 0xFFD4 Peripheral ID 5 Register */
    __IM  uint32_t GICT_PIDR6;                        /*!< Offset 0xFFD8 Peripheral ID 6 Register */
    __IM  uint32_t GICT_PIDR7;                        /*!< Offset 0xFFDC Peripheral ID 7 Register */
    __IM  uint32_t GICT_PIDR0;                        /*!< Offset 0xFFE0 Peripheral ID 0 Register */
    __IM  uint32_t GICT_PIDR1;                        /*!< Offset 0xFFE4 Peripheral ID 1 Register */
    __IM  uint32_t GICT_PIDR2;                        /*!< Offset 0xFFE8 Peripheral ID 2 Register */
    __IM  uint32_t GICT_PIDR3;                        /*!< Offset 0xFFEC Peripheral ID 3 Register */
    __IM  uint32_t GICT_CIDR0;                        /*!< Offset 0xFFF0 Component ID 0 Register */
    __IM  uint32_t GICT_CIDR1;                        /*!< Offset 0xFFF4 Component ID 1 Register */
    __IM  uint32_t GICT_CIDR2;                        /*!< Offset 0xFFF8 Component ID 2 Register */
    __IM  uint32_t GICT_CIDR3;                        /*!< Offset 0xFFFC Component ID 3 Register */
} GICT_TypeDef; /* size of structure = 0x10000 */
/*
 * @brief GITS
 */
/*!< GITS GIC Secure Access Control */
typedef struct GITS_Type
{
         RESERVED(0x000[0xF000 - 0x0000], uint8_t)
    __IM  uint64_t GITS_CFGID;                        /*!< Offset 0xF000 Configuration ID Register */
         RESERVED(0xF008[0xFFD0 - 0xF008], uint8_t)
    __IM  uint32_t GITS_PIDR4;                        /*!< Offset 0xFFD0 Peripheral ID 4 Register */
    __IM  uint32_t GITS_PIDR5;                        /*!< Offset 0xFFD4 Peripheral ID 5 Register */
    __IM  uint32_t GITS_PIDR6;                        /*!< Offset 0xFFD8 Peripheral ID 6 Register */
    __IM  uint32_t GITS_PIDR7;                        /*!< Offset 0xFFDC Peripheral ID 7 Register */
    __IM  uint32_t GITS_PIDR0;                        /*!< Offset 0xFFE0 Peripheral ID 0 Register */
    __IM  uint32_t GITS_PIDR1;                        /*!< Offset 0xFFE4 Peripheral ID 1 Register */
    __IM  uint32_t GITS_PIDR2;                        /*!< Offset 0xFFE8 Peripheral ID 2 Register */
    __IM  uint32_t GITS_PIDR3;                        /*!< Offset 0xFFEC Peripheral ID 3 Register */
    __IM  uint32_t GITS_CIDR0;                        /*!< Offset 0xFFF0 Component ID 0 Register */
    __IM  uint32_t GITS_CIDR1;                        /*!< Offset 0xFFF4 Component ID 1 Register */
    __IM  uint32_t GITS_CIDR2;                        /*!< Offset 0xFFF8 Component ID 2 Register */
    __IM  uint32_t GITS_CIDR3;                        /*!< Offset 0xFFFC Component ID 3 Register */
} GITS_TypeDef; /* size of structure = 0x10000 */
/*
 * @brief GITS_TRANSLATION
 */
/*!< GITS_TRANSLATION  */
typedef struct GITS_TRANSLATION_Type
{
         RESERVED(0x000[0x0040 - 0x0000], uint8_t)
    __OM  uint32_t GITS_TRANSLATER;                   /*!< Offset 0x040 ITS Translation Register */
         RESERVED(0x044[0x10000 - 0x0044], uint8_t)
} GITS_TRANSLATION_TypeDef; /* size of structure = 0x10000 */
/*
 * @brief GPIO
 */
/*!< GPIO Port Controller */
typedef struct GPIO_Type
{
    __IOM uint32_t CFG [0x004];                       /*!< Offset 0x000 Configure Register */
    __IOM uint32_t DATA;                              /*!< Offset 0x010 Data Register */
    __IOM uint32_t DATA_SET;                          /*!< Offset 0x014 Data Set Register */
    __IOM uint32_t DATA_CLR;                          /*!< Offset 0x018 Data Clear Register */
         RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    __IOM uint32_t DRV [0x002];                       /*!< Offset 0x020 Multi_Driving Register */
         RESERVED(0x028[0x0030 - 0x0028], uint8_t)
    __IOM uint32_t PULL [0x002];                      /*!< Offset 0x030 Pull Register */
         RESERVED(0x038[0x0040 - 0x0038], uint8_t)
    __IOM uint32_t INT_CFG [0x004];                   /*!< Offset 0x040 External Interrupt Configure Register */
    __IOM uint32_t INT_CTL;                           /*!< Offset 0x050 External Interrupt Control Register */
    __IOM uint32_t INT_STA;                           /*!< Offset 0x054 External Interrupt Status Register */
    __IOM uint32_t INT_DEB;                           /*!< Offset 0x058 External Debounce Configure Register */
         RESERVED(0x05C[0x0070 - 0x005C], uint8_t)
    __IOM uint32_t SECURE;                            /*!< Offset 0x070 SECURE Configure Register */
         RESERVED(0x074[0x0080 - 0x0074], uint8_t)
} GPIO_TypeDef; /* size of structure = 0x080 */
/*
 * @brief GPIOBLOCK
 */
/*!< GPIOBLOCK Port Controller */
typedef struct GPIOBLOCK_Type
{
    struct
    {
        __IOM uint32_t CFG [0x004];                   /*!< Offset 0x000 Configure Register */
        __IOM uint32_t DATA;                          /*!< Offset 0x010 Data Register */
        __IOM uint32_t DATA_SET;                      /*!< Offset 0x014 Data Set Register */
        __IOM uint32_t DATA_CLR;                      /*!< Offset 0x018 Data Clear Register */
             RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
        __IOM uint32_t DRV [0x002];                   /*!< Offset 0x020 Multi_Driving Register */
             RESERVED(0x028[0x0030 - 0x0028], uint8_t)
        __IOM uint32_t PULL [0x002];                  /*!< Offset 0x030 Pull Register */
             RESERVED(0x038[0x0040 - 0x0038], uint8_t)
        __IOM uint32_t INT_CFG [0x004];               /*!< Offset 0x040 External Interrupt Configure Register */
        __IOM uint32_t INT_CTL;                       /*!< Offset 0x050 External Interrupt Control Register */
        __IOM uint32_t INT_STA;                       /*!< Offset 0x054 External Interrupt Status Register */
        __IOM uint32_t INT_DEB;                       /*!< Offset 0x058 External Debounce Configure Register */
             RESERVED(0x05C[0x0070 - 0x005C], uint8_t)
        __IOM uint32_t SECURE;                        /*!< Offset 0x070 SECURE Configure Register */
             RESERVED(0x074[0x0080 - 0x0074], uint8_t)
    } GPIO_PINS [0x009];                              /*!< Offset 0x000 GPIO pin control B, C, D. E, F, G, H, I, J, K */
} GPIOBLOCK_TypeDef; /* size of structure = 0x480 */
/*
 * @brief GPIOINT
 */
/*!< GPIOINT  */
typedef struct GPIOINT_Type
{
    __IOM uint32_t EINT_CFG [0x004];                  /*!< Offset 0x000 External Interrupt Configure Registers */
    __IOM uint32_t EINT_CTL;                          /*!< Offset 0x010 External Interrupt Control Register */
    __IOM uint32_t EINT_STATUS;                       /*!< Offset 0x014 External Interrupt Status Register */
    __IOM uint32_t EINT_DEB;                          /*!< Offset 0x018 External Interrupt Debounce Register */
         RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
} GPIOINT_TypeDef; /* size of structure = 0x020 */
/*
 * @brief HDMI_TX
 */
/*!< HDMI_TX  */
typedef struct HDMI_TX_Type
{
    __IM  uint8_t  HDMI_DESIGN_ID;                    /*!< Offset 0x000  */
    __IM  uint8_t  HDMI_REVISION_ID;                  /*!< Offset 0x001  */
    __IM  uint8_t  HDMI_PRODUCT_ID0;                  /*!< Offset 0x002  */
    __IM  uint8_t  HDMI_PRODUCT_ID1;                  /*!< Offset 0x003  */
    __IM  uint8_t  HDMI_CONFIG0_ID;                   /*!< Offset 0x004  */
    __IM  uint8_t  HDMI_CONFIG1_ID;                   /*!< Offset 0x005  */
    __IM  uint8_t  HDMI_CONFIG2_ID;                   /*!< Offset 0x006  */
    __IM  uint8_t  HDMI_CONFIG3_ID;                   /*!< Offset 0x007  */
         RESERVED(0x008[0x0100 - 0x0008], uint8_t)
    __IOM uint8_t  HDMI_IH_FC_STAT0;                  /*!< Offset 0x100  */
    __IOM uint8_t  HDMI_IH_FC_STAT1;                  /*!< Offset 0x101  */
    __IOM uint8_t  HDMI_IH_FC_STAT2;                  /*!< Offset 0x102  */
    __IOM uint8_t  HDMI_IH_AS_STAT0;                  /*!< Offset 0x103  */
    __IOM uint8_t  HDMI_IH_PHY_STAT0;                 /*!< Offset 0x104  */
    __IOM uint8_t  HDMI_IH_I2CM_STAT0;                /*!< Offset 0x105  */
    __IOM uint8_t  HDMI_IH_CEC_STAT0;                 /*!< Offset 0x106  */
    __IOM uint8_t  HDMI_IH_VP_STAT0;                  /*!< Offset 0x107  */
    __IOM uint8_t  HDMI_IH_I2CMPHY_STAT0;             /*!< Offset 0x108  */
    __IOM uint8_t  HDMI_IH_AHBDMAAUD_STAT0;           /*!< Offset 0x109  */
         RESERVED(0x10A[0x0180 - 0x010A], uint8_t)
    __IOM uint8_t  HDMI_IH_MUTE_FC_STAT0;             /*!< Offset 0x180  */
    __IOM uint8_t  HDMI_IH_MUTE_FC_STAT1;             /*!< Offset 0x181  */
    __IOM uint8_t  HDMI_IH_MUTE_FC_STAT2;             /*!< Offset 0x182  */
    __IOM uint8_t  HDMI_IH_MUTE_AS_STAT0;             /*!< Offset 0x183  */
    __IOM uint8_t  HDMI_IH_MUTE_PHY_STAT0;            /*!< Offset 0x184  */
    __IOM uint8_t  HDMI_IH_MUTE_I2CM_STAT0;           /*!< Offset 0x185  */
    __IOM uint8_t  HDMI_IH_MUTE_CEC_STAT0;            /*!< Offset 0x186  */
    __IOM uint8_t  HDMI_IH_MUTE_VP_STAT0;             /*!< Offset 0x187  */
    __IOM uint8_t  HDMI_IH_MUTE_I2CMPHY_STAT0;        /*!< Offset 0x188  */
    __IOM uint8_t  HDMI_IH_MUTE_AHBDMAAUD_STAT0;      /*!< Offset 0x189  */
         RESERVED(0x18A[0x01FF - 0x018A], uint8_t)
    __IOM uint8_t  HDMI_IH_MUTE;                      /*!< Offset 0x1FF  */
    __IOM uint8_t  HDMI_TX_INVID0;                    /*!< Offset 0x200  */
    __IOM uint8_t  HDMI_TX_INSTUFFING;                /*!< Offset 0x201  */
    __IOM uint8_t  HDMI_TX_GYDATA0;                   /*!< Offset 0x202  */
    __IOM uint8_t  HDMI_TX_GYDATA1;                   /*!< Offset 0x203  */
    __IOM uint8_t  HDMI_TX_RCRDATA0;                  /*!< Offset 0x204  */
    __IOM uint8_t  HDMI_TX_RCRDATA1;                  /*!< Offset 0x205  */
    __IOM uint8_t  HDMI_TX_BCBDATA0;                  /*!< Offset 0x206  */
    __IOM uint8_t  HDMI_TX_BCBDATA1;                  /*!< Offset 0x207  */
         RESERVED(0x208[0x0800 - 0x0208], uint8_t)
    __IOM uint8_t  HDMI_VP_STATUS;                    /*!< Offset 0x800  */
    __IOM uint8_t  HDMI_VP_PR_CD;                     /*!< Offset 0x801  */
    __IOM uint8_t  HDMI_VP_STUFF;                     /*!< Offset 0x802  */
    __IOM uint8_t  HDMI_VP_REMAP;                     /*!< Offset 0x803  */
    __IOM uint8_t  HDMI_VP_CONF;                      /*!< Offset 0x804  */
    __IOM uint8_t  HDMI_VP_STAT;                      /*!< Offset 0x805  */
    __IOM uint8_t  HDMI_VP_INT;                       /*!< Offset 0x806  */
    __IOM uint8_t  HDMI_VP_MASK;                      /*!< Offset 0x807  */
    __IOM uint8_t  HDMI_VP_POL;                       /*!< Offset 0x808  */
         RESERVED(0x809[0x1000 - 0x0809], uint8_t)
    __IOM uint8_t  HDMI_FC_INVIDCONF;                 /*!< Offset 0x1000  */
    __IOM uint8_t  HDMI_FC_INHACTV0;                  /*!< Offset 0x1001  */
    __IOM uint8_t  HDMI_FC_INHACTV1;                  /*!< Offset 0x1002  */
    __IOM uint8_t  HDMI_FC_INHBLANK0;                 /*!< Offset 0x1003  */
    __IOM uint8_t  HDMI_FC_INHBLANK1;                 /*!< Offset 0x1004  */
    __IOM uint8_t  HDMI_FC_INVACTV0;                  /*!< Offset 0x1005  */
    __IOM uint8_t  HDMI_FC_INVACTV1;                  /*!< Offset 0x1006  */
    __IOM uint8_t  HDMI_FC_INVBLANK;                  /*!< Offset 0x1007  */
    __IOM uint8_t  HDMI_FC_HSYNCINDELAY0;             /*!< Offset 0x1008  */
    __IOM uint8_t  HDMI_FC_HSYNCINDELAY1;             /*!< Offset 0x1009  */
    __IOM uint8_t  HDMI_FC_HSYNCINWIDTH0;             /*!< Offset 0x100A  */
    __IOM uint8_t  HDMI_FC_HSYNCINWIDTH1;             /*!< Offset 0x100B  */
    __IOM uint8_t  HDMI_FC_VSYNCINDELAY;              /*!< Offset 0x100C  */
    __IOM uint8_t  HDMI_FC_VSYNCINWIDTH;              /*!< Offset 0x100D  */
    __IOM uint8_t  HDMI_FC_INFREQ0;                   /*!< Offset 0x100E  */
    __IOM uint8_t  HDMI_FC_INFREQ1;                   /*!< Offset 0x100F  */
    __IOM uint8_t  HDMI_FC_INFREQ2;                   /*!< Offset 0x1010  */
    __IOM uint8_t  HDMI_FC_CTRLDUR;                   /*!< Offset 0x1011  */
    __IOM uint8_t  HDMI_FC_EXCTRLDUR;                 /*!< Offset 0x1012  */
    __IOM uint8_t  HDMI_FC_EXCTRLSPAC;                /*!< Offset 0x1013  */
    __IOM uint8_t  HDMI_FC_CH0PREAM;                  /*!< Offset 0x1014  */
    __IOM uint8_t  HDMI_FC_CH1PREAM;                  /*!< Offset 0x1015  */
    __IOM uint8_t  HDMI_FC_CH2PREAM;                  /*!< Offset 0x1016  */
    __IOM uint8_t  HDMI_FC_AVICONF3;                  /*!< Offset 0x1017  */
    __IOM uint8_t  HDMI_FC_GCP;                       /*!< Offset 0x1018  */
    __IOM uint8_t  HDMI_FC_AVICONF0;                  /*!< Offset 0x1019  */
    __IOM uint8_t  HDMI_FC_AVICONF1;                  /*!< Offset 0x101A  */
    __IOM uint8_t  HDMI_FC_AVICONF2;                  /*!< Offset 0x101B  */
    __IOM uint8_t  HDMI_FC_AVIVID;                    /*!< Offset 0x101C  */
    __IOM uint8_t  HDMI_FC_AVIETB0;                   /*!< Offset 0x101D  */
    __IOM uint8_t  HDMI_FC_AVIETB1;                   /*!< Offset 0x101E  */
    __IOM uint8_t  HDMI_FC_AVISBB0;                   /*!< Offset 0x101F  */
    __IOM uint8_t  HDMI_FC_AVISBB1;                   /*!< Offset 0x1020  */
    __IOM uint8_t  HDMI_FC_AVIELB0;                   /*!< Offset 0x1021  */
    __IOM uint8_t  HDMI_FC_AVIELB1;                   /*!< Offset 0x1022  */
    __IOM uint8_t  HDMI_FC_AVISRB0;                   /*!< Offset 0x1023  */
    __IOM uint8_t  HDMI_FC_AVISRB1;                   /*!< Offset 0x1024  */
    __IOM uint8_t  HDMI_FC_AUDICONF0;                 /*!< Offset 0x1025  */
    __IOM uint8_t  HDMI_FC_AUDICONF1;                 /*!< Offset 0x1026  */
    __IOM uint8_t  HDMI_FC_AUDICONF2;                 /*!< Offset 0x1027  */
    __IOM uint8_t  HDMI_FC_AUDICONF3;                 /*!< Offset 0x1028  */
    __IOM uint8_t  HDMI_FC_VSDIEEEID0;                /*!< Offset 0x1029  */
    __IOM uint8_t  HDMI_FC_VSDSIZE;                   /*!< Offset 0x102A  */
         RESERVED(0x102B[0x1030 - 0x102B], uint8_t)
    __IOM uint8_t  HDMI_FC_VSDIEEEID1;                /*!< Offset 0x1030  */
    __IOM uint8_t  HDMI_FC_VSDIEEEID2;                /*!< Offset 0x1031  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD0;               /*!< Offset 0x1032  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD1;               /*!< Offset 0x1033  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD2;               /*!< Offset 0x1034  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD3;               /*!< Offset 0x1035  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD4;               /*!< Offset 0x1036  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD5;               /*!< Offset 0x1037  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD6;               /*!< Offset 0x1038  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD7;               /*!< Offset 0x1039  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD8;               /*!< Offset 0x103A  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD9;               /*!< Offset 0x103B  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD10;              /*!< Offset 0x103C  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD11;              /*!< Offset 0x103D  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD12;              /*!< Offset 0x103E  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD13;              /*!< Offset 0x103F  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD14;              /*!< Offset 0x1040  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD15;              /*!< Offset 0x1041  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD16;              /*!< Offset 0x1042  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD17;              /*!< Offset 0x1043  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD18;              /*!< Offset 0x1044  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD19;              /*!< Offset 0x1045  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD20;              /*!< Offset 0x1046  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD21;              /*!< Offset 0x1047  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD22;              /*!< Offset 0x1048  */
    __IOM uint8_t  HDMI_FC_VSDPAYLOAD23;              /*!< Offset 0x1049  */
    __IOM uint8_t  HDMI_FC_SPDVENDORNAME0;            /*!< Offset 0x104A  */
    __IOM uint8_t  HDMI_FC_SPDVENDORNAME1;            /*!< Offset 0x104B  */
    __IOM uint8_t  HDMI_FC_SPDVENDORNAME2;            /*!< Offset 0x104C  */
    __IOM uint8_t  HDMI_FC_SPDVENDORNAME3;            /*!< Offset 0x104D  */
    __IOM uint8_t  HDMI_FC_SPDVENDORNAME4;            /*!< Offset 0x104E  */
    __IOM uint8_t  HDMI_FC_SPDVENDORNAME5;            /*!< Offset 0x104F  */
    __IOM uint8_t  HDMI_FC_SPDVENDORNAME6;            /*!< Offset 0x1050  */
    __IOM uint8_t  HDMI_FC_SPDVENDORNAME7;            /*!< Offset 0x1051  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME0;           /*!< Offset 0x1052  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME1;           /*!< Offset 0x1053  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME2;           /*!< Offset 0x1054  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME3;           /*!< Offset 0x1055  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME4;           /*!< Offset 0x1056  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME5;           /*!< Offset 0x1057  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME6;           /*!< Offset 0x1058  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME7;           /*!< Offset 0x1059  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME8;           /*!< Offset 0x105A  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME9;           /*!< Offset 0x105B  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME10;          /*!< Offset 0x105C  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME11;          /*!< Offset 0x105D  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME12;          /*!< Offset 0x105E  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME13;          /*!< Offset 0x105F  */
    __IOM uint8_t  HDMI_FC_SDPPRODUCTNAME14;          /*!< Offset 0x1060  */
    __IOM uint8_t  HDMI_FC_SPDPRODUCTNAME15;          /*!< Offset 0x1061  */
    __IOM uint8_t  HDMI_FC_SPDDEVICEINF;              /*!< Offset 0x1062  */
    __IOM uint8_t  HDMI_FC_AUDSCONF;                  /*!< Offset 0x1063  */
    __IOM uint8_t  HDMI_FC_AUDSSTAT;                  /*!< Offset 0x1064  */
         RESERVED(0x1065[0x1070 - 0x1065], uint8_t)
    __IOM uint8_t  HDMI_FC_DATACH0FILL;               /*!< Offset 0x1070  */
    __IOM uint8_t  HDMI_FC_DATACH1FILL;               /*!< Offset 0x1071  */
    __IOM uint8_t  HDMI_FC_DATACH2FILL;               /*!< Offset 0x1072  */
    __IOM uint8_t  HDMI_FC_CTRLQHIGH;                 /*!< Offset 0x1073  */
    __IOM uint8_t  HDMI_FC_CTRLQLOW;                  /*!< Offset 0x1074  */
    __IOM uint8_t  HDMI_FC_ACP0;                      /*!< Offset 0x1075  */
    __IOM uint8_t  HDMI_FC_ACP28;                     /*!< Offset 0x1076  */
    __IOM uint8_t  HDMI_FC_ACP27;                     /*!< Offset 0x1077  */
    __IOM uint8_t  HDMI_FC_ACP26;                     /*!< Offset 0x1078  */
    __IOM uint8_t  HDMI_FC_ACP25;                     /*!< Offset 0x1079  */
    __IOM uint8_t  HDMI_FC_ACP24;                     /*!< Offset 0x107A  */
    __IOM uint8_t  HDMI_FC_ACP23;                     /*!< Offset 0x107B  */
    __IOM uint8_t  HDMI_FC_ACP22;                     /*!< Offset 0x107C  */
    __IOM uint8_t  HDMI_FC_ACP21;                     /*!< Offset 0x107D  */
    __IOM uint8_t  HDMI_FC_ACP20;                     /*!< Offset 0x107E  */
    __IOM uint8_t  HDMI_FC_ACP19;                     /*!< Offset 0x107F  */
    __IOM uint8_t  HDMI_FC_ACP18;                     /*!< Offset 0x1080  */
    __IOM uint8_t  HDMI_FC_ACP17;                     /*!< Offset 0x1081  */
    __IOM uint8_t  HDMI_FC_ACP16;                     /*!< Offset 0x1082  */
    __IOM uint8_t  HDMI_FC_ACP15;                     /*!< Offset 0x1083  */
    __IOM uint8_t  HDMI_FC_ACP14;                     /*!< Offset 0x1084  */
    __IOM uint8_t  HDMI_FC_ACP13;                     /*!< Offset 0x1085  */
    __IOM uint8_t  HDMI_FC_ACP12;                     /*!< Offset 0x1086  */
    __IOM uint8_t  HDMI_FC_ACP11;                     /*!< Offset 0x1087  */
    __IOM uint8_t  HDMI_FC_ACP10;                     /*!< Offset 0x1088  */
    __IOM uint8_t  HDMI_FC_ACP9;                      /*!< Offset 0x1089  */
    __IOM uint8_t  HDMI_FC_ACP8;                      /*!< Offset 0x108A  */
    __IOM uint8_t  HDMI_FC_ACP7;                      /*!< Offset 0x108B  */
    __IOM uint8_t  HDMI_FC_ACP6;                      /*!< Offset 0x108C  */
    __IOM uint8_t  HDMI_FC_ACP5;                      /*!< Offset 0x108D  */
    __IOM uint8_t  HDMI_FC_ACP4;                      /*!< Offset 0x108E  */
    __IOM uint8_t  HDMI_FC_ACP3;                      /*!< Offset 0x108F  */
    __IOM uint8_t  HDMI_FC_ACP2;                      /*!< Offset 0x1090  */
    __IOM uint8_t  HDMI_FC_ACP1;                      /*!< Offset 0x1091  */
    __IOM uint8_t  HDMI_FC_ISCR1_0;                   /*!< Offset 0x1092  */
    __IOM uint8_t  HDMI_FC_ISCR1_16;                  /*!< Offset 0x1093  */
    __IOM uint8_t  HDMI_FC_ISCR1_15;                  /*!< Offset 0x1094  */
    __IOM uint8_t  HDMI_FC_ISCR1_14;                  /*!< Offset 0x1095  */
    __IOM uint8_t  HDMI_FC_ISCR1_13;                  /*!< Offset 0x1096  */
    __IOM uint8_t  HDMI_FC_ISCR1_12;                  /*!< Offset 0x1097  */
    __IOM uint8_t  HDMI_FC_ISCR1_11;                  /*!< Offset 0x1098  */
    __IOM uint8_t  HDMI_FC_ISCR1_10;                  /*!< Offset 0x1099  */
    __IOM uint8_t  HDMI_FC_ISCR1_9;                   /*!< Offset 0x109A  */
    __IOM uint8_t  HDMI_FC_ISCR1_8;                   /*!< Offset 0x109B  */
    __IOM uint8_t  HDMI_FC_ISCR1_7;                   /*!< Offset 0x109C  */
    __IOM uint8_t  HDMI_FC_ISCR1_6;                   /*!< Offset 0x109D  */
    __IOM uint8_t  HDMI_FC_ISCR1_5;                   /*!< Offset 0x109E  */
    __IOM uint8_t  HDMI_FC_ISCR1_4;                   /*!< Offset 0x109F  */
    __IOM uint8_t  HDMI_FC_ISCR1_3;                   /*!< Offset 0x10A0  */
    __IOM uint8_t  HDMI_FC_ISCR1_2;                   /*!< Offset 0x10A1  */
    __IOM uint8_t  HDMI_FC_ISCR1_1;                   /*!< Offset 0x10A2  */
    __IOM uint8_t  HDMI_FC_ISCR2_15;                  /*!< Offset 0x10A3  */
    __IOM uint8_t  HDMI_FC_ISCR2_14;                  /*!< Offset 0x10A4  */
    __IOM uint8_t  HDMI_FC_ISCR2_13;                  /*!< Offset 0x10A5  */
    __IOM uint8_t  HDMI_FC_ISCR2_12;                  /*!< Offset 0x10A6  */
    __IOM uint8_t  HDMI_FC_ISCR2_11;                  /*!< Offset 0x10A7  */
    __IOM uint8_t  HDMI_FC_ISCR2_10;                  /*!< Offset 0x10A8  */
    __IOM uint8_t  HDMI_FC_ISCR2_9;                   /*!< Offset 0x10A9  */
    __IOM uint8_t  HDMI_FC_ISCR2_8;                   /*!< Offset 0x10AA  */
    __IOM uint8_t  HDMI_FC_ISCR2_7;                   /*!< Offset 0x10AB  */
    __IOM uint8_t  HDMI_FC_ISCR2_6;                   /*!< Offset 0x10AC  */
    __IOM uint8_t  HDMI_FC_ISCR2_5;                   /*!< Offset 0x10AD  */
    __IOM uint8_t  HDMI_FC_ISCR2_4;                   /*!< Offset 0x10AE  */
    __IOM uint8_t  HDMI_FC_ISCR2_3;                   /*!< Offset 0x10AF  */
    __IOM uint8_t  HDMI_FC_ISCR2_2;                   /*!< Offset 0x10B0  */
    __IOM uint8_t  HDMI_FC_ISCR2_1;                   /*!< Offset 0x10B1  */
    __IOM uint8_t  HDMI_FC_ISCR2_0;                   /*!< Offset 0x10B2  */
    __IOM uint8_t  HDMI_FC_DATAUTO0;                  /*!< Offset 0x10B3  */
    __IOM uint8_t  HDMI_FC_DATAUTO1;                  /*!< Offset 0x10B4  */
    __IOM uint8_t  HDMI_FC_DATAUTO2;                  /*!< Offset 0x10B5  */
    __IOM uint8_t  HDMI_FC_DATMAN;                    /*!< Offset 0x10B6  */
    __IOM uint8_t  HDMI_FC_DATAUTO3;                  /*!< Offset 0x10B7  */
    __IOM uint8_t  HDMI_FC_RDRB0;                     /*!< Offset 0x10B8  */
    __IOM uint8_t  HDMI_FC_RDRB1;                     /*!< Offset 0x10B9  */
    __IOM uint8_t  HDMI_FC_RDRB2;                     /*!< Offset 0x10BA  */
    __IOM uint8_t  HDMI_FC_RDRB3;                     /*!< Offset 0x10BB  */
    __IOM uint8_t  HDMI_FC_RDRB4;                     /*!< Offset 0x10BC  */
    __IOM uint8_t  HDMI_FC_RDRB5;                     /*!< Offset 0x10BD  */
    __IOM uint8_t  HDMI_FC_RDRB6;                     /*!< Offset 0x10BE  */
    __IOM uint8_t  HDMI_FC_RDRB7;                     /*!< Offset 0x10BF  */
         RESERVED(0x10C0[0x10D0 - 0x10C0], uint8_t)
    __IOM uint8_t  HDMI_FC_STAT0;                     /*!< Offset 0x10D0  */
    __IOM uint8_t  HDMI_FC_INT0;                      /*!< Offset 0x10D1  */
    __IOM uint8_t  HDMI_FC_MASK0;                     /*!< Offset 0x10D2  */
    __IOM uint8_t  HDMI_FC_POL0;                      /*!< Offset 0x10D3  */
    __IOM uint8_t  HDMI_FC_STAT1;                     /*!< Offset 0x10D4  */
    __IOM uint8_t  HDMI_FC_INT1;                      /*!< Offset 0x10D5  */
    __IOM uint8_t  HDMI_FC_MASK1;                     /*!< Offset 0x10D6  */
    __IOM uint8_t  HDMI_FC_POL1;                      /*!< Offset 0x10D7  */
    __IOM uint8_t  HDMI_FC_STAT2;                     /*!< Offset 0x10D8  */
    __IOM uint8_t  HDMI_FC_INT2;                      /*!< Offset 0x10D9  */
    __IOM uint8_t  HDMI_FC_MASK2;                     /*!< Offset 0x10DA  */
    __IOM uint8_t  HDMI_FC_POL2;                      /*!< Offset 0x10DB  */
         RESERVED(0x10DC[0x10E0 - 0x10DC], uint8_t)
    __IOM uint8_t  HDMI_FC_PRCONF;                    /*!< Offset 0x10E0  */
         RESERVED(0x10E1[0x1100 - 0x10E1], uint8_t)
    __IOM uint8_t  HDMI_FC_GMD_STAT;                  /*!< Offset 0x1100  */
    __IOM uint8_t  HDMI_FC_GMD_EN;                    /*!< Offset 0x1101  */
    __IOM uint8_t  HDMI_FC_GMD_UP;                    /*!< Offset 0x1102  */
    __IOM uint8_t  HDMI_FC_GMD_CONF;                  /*!< Offset 0x1103  */
    __IOM uint8_t  HDMI_FC_GMD_HB;                    /*!< Offset 0x1104  */
    __IOM uint8_t  HDMI_FC_GMD_PB0;                   /*!< Offset 0x1105  */
    __IOM uint8_t  HDMI_FC_GMD_PB1;                   /*!< Offset 0x1106  */
    __IOM uint8_t  HDMI_FC_GMD_PB2;                   /*!< Offset 0x1107  */
    __IOM uint8_t  HDMI_FC_GMD_PB3;                   /*!< Offset 0x1108  */
    __IOM uint8_t  HDMI_FC_GMD_PB4;                   /*!< Offset 0x1109  */
    __IOM uint8_t  HDMI_FC_GMD_PB5;                   /*!< Offset 0x110A  */
    __IOM uint8_t  HDMI_FC_GMD_PB6;                   /*!< Offset 0x110B  */
    __IOM uint8_t  HDMI_FC_GMD_PB7;                   /*!< Offset 0x110C  */
    __IOM uint8_t  HDMI_FC_GMD_PB8;                   /*!< Offset 0x110D  */
    __IOM uint8_t  HDMI_FC_GMD_PB9;                   /*!< Offset 0x110E  */
    __IOM uint8_t  HDMI_FC_GMD_PB10;                  /*!< Offset 0x110F  */
    __IOM uint8_t  HDMI_FC_GMD_PB11;                  /*!< Offset 0x1110  */
    __IOM uint8_t  HDMI_FC_GMD_PB12;                  /*!< Offset 0x1111  */
    __IOM uint8_t  HDMI_FC_GMD_PB13;                  /*!< Offset 0x1112  */
    __IOM uint8_t  HDMI_FC_GMD_PB14;                  /*!< Offset 0x1113  */
    __IOM uint8_t  HDMI_FC_GMD_PB15;                  /*!< Offset 0x1114  */
    __IOM uint8_t  HDMI_FC_GMD_PB16;                  /*!< Offset 0x1115  */
    __IOM uint8_t  HDMI_FC_GMD_PB17;                  /*!< Offset 0x1116  */
    __IOM uint8_t  HDMI_FC_GMD_PB18;                  /*!< Offset 0x1117  */
    __IOM uint8_t  HDMI_FC_GMD_PB19;                  /*!< Offset 0x1118  */
    __IOM uint8_t  HDMI_FC_GMD_PB20;                  /*!< Offset 0x1119  */
    __IOM uint8_t  HDMI_FC_GMD_PB21;                  /*!< Offset 0x111A  */
    __IOM uint8_t  HDMI_FC_GMD_PB22;                  /*!< Offset 0x111B  */
    __IOM uint8_t  HDMI_FC_GMD_PB23;                  /*!< Offset 0x111C  */
    __IOM uint8_t  HDMI_FC_GMD_PB24;                  /*!< Offset 0x111D  */
    __IOM uint8_t  HDMI_FC_GMD_PB25;                  /*!< Offset 0x111E  */
    __IOM uint8_t  HDMI_FC_GMD_PB26;                  /*!< Offset 0x111F  */
    __IOM uint8_t  HDMI_FC_GMD_PB27;                  /*!< Offset 0x1120  */
         RESERVED(0x1121[0x1200 - 0x1121], uint8_t)
    __IOM uint8_t  HDMI_FC_DBGFORCE;                  /*!< Offset 0x1200  */
    __IOM uint8_t  HDMI_FC_DBGAUD0CH0;                /*!< Offset 0x1201  */
    __IOM uint8_t  HDMI_FC_DBGAUD1CH0;                /*!< Offset 0x1202  */
    __IOM uint8_t  HDMI_FC_DBGAUD2CH0;                /*!< Offset 0x1203  */
    __IOM uint8_t  HDMI_FC_DBGAUD0CH1;                /*!< Offset 0x1204  */
    __IOM uint8_t  HDMI_FC_DBGAUD1CH1;                /*!< Offset 0x1205  */
    __IOM uint8_t  HDMI_FC_DBGAUD2CH1;                /*!< Offset 0x1206  */
    __IOM uint8_t  HDMI_FC_DBGAUD0CH2;                /*!< Offset 0x1207  */
    __IOM uint8_t  HDMI_FC_DBGAUD1CH2;                /*!< Offset 0x1208  */
    __IOM uint8_t  HDMI_FC_DBGAUD2CH2;                /*!< Offset 0x1209  */
    __IOM uint8_t  HDMI_FC_DBGAUD0CH3;                /*!< Offset 0x120A  */
    __IOM uint8_t  HDMI_FC_DBGAUD1CH3;                /*!< Offset 0x120B  */
    __IOM uint8_t  HDMI_FC_DBGAUD2CH3;                /*!< Offset 0x120C  */
    __IOM uint8_t  HDMI_FC_DBGAUD0CH4;                /*!< Offset 0x120D  */
    __IOM uint8_t  HDMI_FC_DBGAUD1CH4;                /*!< Offset 0x120E  */
    __IOM uint8_t  HDMI_FC_DBGAUD2CH4;                /*!< Offset 0x120F  */
    __IOM uint8_t  HDMI_FC_DBGAUD0CH5;                /*!< Offset 0x1210  */
    __IOM uint8_t  HDMI_FC_DBGAUD1CH5;                /*!< Offset 0x1211  */
    __IOM uint8_t  HDMI_FC_DBGAUD2CH5;                /*!< Offset 0x1212  */
    __IOM uint8_t  HDMI_FC_DBGAUD0CH6;                /*!< Offset 0x1213  */
    __IOM uint8_t  HDMI_FC_DBGAUD1CH6;                /*!< Offset 0x1214  */
    __IOM uint8_t  HDMI_FC_DBGAUD2CH6;                /*!< Offset 0x1215  */
    __IOM uint8_t  HDMI_FC_DBGAUD0CH7;                /*!< Offset 0x1216  */
    __IOM uint8_t  HDMI_FC_DBGAUD1CH7;                /*!< Offset 0x1217  */
    __IOM uint8_t  HDMI_FC_DBGAUD2CH7;                /*!< Offset 0x1218  */
    __IOM uint8_t  HDMI_FC_DBGTMDS0;                  /*!< Offset 0x1219  */
    __IOM uint8_t  HDMI_FC_DBGTMDS1;                  /*!< Offset 0x121A  */
    __IOM uint8_t  HDMI_FC_DBGTMDS2;                  /*!< Offset 0x121B  */
         RESERVED(0x121C[0x3000 - 0x121C], uint8_t)
    __IOM uint8_t  HDMI_PHY_CONF0;                    /*!< Offset 0x3000  */
    __IOM uint8_t  HDMI_PHY_TST0;                     /*!< Offset 0x3001  */
    __IOM uint8_t  HDMI_PHY_TST1;                     /*!< Offset 0x3002  */
    __IOM uint8_t  HDMI_PHY_TST2;                     /*!< Offset 0x3003  */
    __IOM uint8_t  HDMI_PHY_STAT0;                    /*!< Offset 0x3004  */
    __IOM uint8_t  HDMI_PHY_INT0;                     /*!< Offset 0x3005  */
    __IOM uint8_t  HDMI_PHY_MASK0;                    /*!< Offset 0x3006  */
    __IOM uint8_t  HDMI_PHY_POL0;                     /*!< Offset 0x3007  */
         RESERVED(0x3008[0x3020 - 0x3008], uint8_t)
    __IOM uint8_t  HDMI_PHY_I2CM_SLAVE_ADDR;          /*!< Offset 0x3020  */
    __IOM uint8_t  HDMI_PHY_I2CM_ADDRESS_ADDR;        /*!< Offset 0x3021  */
    __IOM uint8_t  HDMI_PHY_I2CM_DATAO_1_ADDR;        /*!< Offset 0x3022  */
    __IOM uint8_t  HDMI_PHY_I2CM_DATAO_0_ADDR;        /*!< Offset 0x3023  */
    __IOM uint8_t  HDMI_PHY_I2CM_DATAI_1_ADDR;        /*!< Offset 0x3024  */
    __IOM uint8_t  HDMI_PHY_I2CM_DATAI_0_ADDR;        /*!< Offset 0x3025  */
    __IOM uint8_t  HDMI_PHY_I2CM_OPERATION_ADDR;      /*!< Offset 0x3026  */
    __IOM uint8_t  HDMI_PHY_I2CM_INT_ADDR;            /*!< Offset 0x3027  */
    __IOM uint8_t  HDMI_PHY_I2CM_CTLINT_ADDR;         /*!< Offset 0x3028  */
    __IOM uint8_t  HDMI_PHY_I2CM_DIV_ADDR;            /*!< Offset 0x3029  */
    __IOM uint8_t  HDMI_PHY_I2CM_SOFTRSTZ_ADDR;       /*!< Offset 0x302A  */
    __IOM uint8_t  HDMI_PHY_I2CM_SS_SCL_HCNT_1_ADDR;  /*!< Offset 0x302B  */
    __IOM uint8_t  HDMI_PHY_I2CM_SS_SCL_HCNT_0_ADDR;  /*!< Offset 0x302C  */
    __IOM uint8_t  HDMI_PHY_I2CM_SS_SCL_LCNT_1_ADDR;  /*!< Offset 0x302D  */
    __IOM uint8_t  HDMI_PHY_I2CM_SS_SCL_LCNT_0_ADDR;  /*!< Offset 0x302E  */
    __IOM uint8_t  HDMI_PHY_I2CM_FS_SCL_HCNT_1_ADDR;  /*!< Offset 0x302F  */
    __IOM uint8_t  HDMI_PHY_I2CM_FS_SCL_HCNT_0_ADDR;  /*!< Offset 0x3030  */
    __IOM uint8_t  HDMI_PHY_I2CM_FS_SCL_LCNT_1_ADDR;  /*!< Offset 0x3031  */
    __IOM uint8_t  HDMI_PHY_I2CM_FS_SCL_LCNT_0_ADDR;  /*!< Offset 0x3032  */
         RESERVED(0x3033[0x3100 - 0x3033], uint8_t)
    __IOM uint8_t  HDMI_AUD_CONF0;                    /*!< Offset 0x3100  */
    __IOM uint8_t  HDMI_AUD_CONF1;                    /*!< Offset 0x3101  */
    __IOM uint8_t  HDMI_AUD_INT;                      /*!< Offset 0x3102  */
    __IOM uint8_t  HDMI_AUD_CONF2;                    /*!< Offset 0x3103  */
         RESERVED(0x3104[0x3200 - 0x3104], uint8_t)
    __IOM uint8_t  HDMI_AUD_N1;                       /*!< Offset 0x3200  */
    __IOM uint8_t  HDMI_AUD_N2;                       /*!< Offset 0x3201  */
    __IOM uint8_t  HDMI_AUD_N3;                       /*!< Offset 0x3202  */
    __IOM uint8_t  HDMI_AUD_CTS1;                     /*!< Offset 0x3203  */
    __IOM uint8_t  HDMI_AUD_CTS2;                     /*!< Offset 0x3204  */
    __IOM uint8_t  HDMI_AUD_CTS3;                     /*!< Offset 0x3205  */
    __IOM uint8_t  HDMI_AUD_INPUTCLKFS;               /*!< Offset 0x3206  */
         RESERVED(0x3207[0x3302 - 0x3207], uint8_t)
    __IOM uint8_t  HDMI_AUD_SPDIFINT;                 /*!< Offset 0x3302  */
         RESERVED(0x3303[0x3400 - 0x3303], uint8_t)
    __IOM uint8_t  HDMI_AUD_CONF0_HBR;                /*!< Offset 0x3400  */
    __IOM uint8_t  HDMI_AUD_HBR_STATUS;               /*!< Offset 0x3401  */
    __IOM uint8_t  HDMI_AUD_HBR_INT;                  /*!< Offset 0x3402  */
    __IOM uint8_t  HDMI_AUD_HBR_POL;                  /*!< Offset 0x3403  */
    __IOM uint8_t  HDMI_AUD_HBR_MASK;                 /*!< Offset 0x3404  */
         RESERVED(0x3405[0x3500 - 0x3405], uint8_t)
    __IOM uint8_t  HDMI_GP_CONF0;                     /*!< Offset 0x3500  */
    __IOM uint8_t  HDMI_GP_CONF1;                     /*!< Offset 0x3501  */
    __IOM uint8_t  HDMI_GP_CONF2;                     /*!< Offset 0x3502  */
    __IOM uint8_t  HDMI_GP_STAT;                      /*!< Offset 0x3503  */
    __IOM uint8_t  HDMI_GP_INT;                       /*!< Offset 0x3504  */
    __IOM uint8_t  HDMI_GP_MASK;                      /*!< Offset 0x3505  */
    __IOM uint8_t  HDMI_GP_POL;                       /*!< Offset 0x3506  */
         RESERVED(0x3507[0x3600 - 0x3507], uint8_t)
    __IOM uint8_t  HDMI_AHB_DMA_CONF0;                /*!< Offset 0x3600  */
    __IOM uint8_t  HDMI_AHB_DMA_START;                /*!< Offset 0x3601  */
    __IOM uint8_t  HDMI_AHB_DMA_STOP;                 /*!< Offset 0x3602  */
    __IOM uint8_t  HDMI_AHB_DMA_THRSLD;               /*!< Offset 0x3603  */
    __IOM uint8_t  HDMI_AHB_DMA_STRADDR0;             /*!< Offset 0x3604  */
    __IOM uint8_t  HDMI_AHB_DMA_STRADDR1;             /*!< Offset 0x3605  */
    __IOM uint8_t  HDMI_AHB_DMA_STRADDR2;             /*!< Offset 0x3606  */
    __IOM uint8_t  HDMI_AHB_DMA_STRADDR3;             /*!< Offset 0x3607  */
    __IOM uint8_t  HDMI_AHB_DMA_STPADDR0;             /*!< Offset 0x3608  */
    __IOM uint8_t  HDMI_AHB_DMA_STPADDR1;             /*!< Offset 0x3609  */
    __IOM uint8_t  HDMI_AHB_DMA_STPADDR2;             /*!< Offset 0x360A  */
    __IOM uint8_t  HDMI_AHB_DMA_STPADDR3;             /*!< Offset 0x360B  */
    __IOM uint8_t  HDMI_AHB_DMA_BSTADDR0;             /*!< Offset 0x360C  */
    __IOM uint8_t  HDMI_AHB_DMA_BSTADDR1;             /*!< Offset 0x360D  */
    __IOM uint8_t  HDMI_AHB_DMA_BSTADDR2;             /*!< Offset 0x360E  */
    __IOM uint8_t  HDMI_AHB_DMA_BSTADDR3;             /*!< Offset 0x360F  */
    __IOM uint8_t  HDMI_AHB_DMA_MBLENGTH0;            /*!< Offset 0x3610  */
    __IOM uint8_t  HDMI_AHB_DMA_MBLENGTH1;            /*!< Offset 0x3611  */
    __IOM uint8_t  HDMI_AHB_DMA_STAT;                 /*!< Offset 0x3612  */
    __IOM uint8_t  HDMI_AHB_DMA_INT;                  /*!< Offset 0x3613  */
    __IOM uint8_t  HDMI_AHB_DMA_MASK;                 /*!< Offset 0x3614  */
    __IOM uint8_t  HDMI_AHB_DMA_POL;                  /*!< Offset 0x3615  */
    __IOM uint8_t  HDMI_AHB_DMA_CONF1;                /*!< Offset 0x3616  */
    __IOM uint8_t  HDMI_AHB_DMA_BUFFSTAT;             /*!< Offset 0x3617  */
    __IOM uint8_t  HDMI_AHB_DMA_BUFFINT;              /*!< Offset 0x3618  */
    __IOM uint8_t  HDMI_AHB_DMA_BUFFMASK;             /*!< Offset 0x3619  */
    __IOM uint8_t  HDMI_AHB_DMA_BUFFPOL;              /*!< Offset 0x361A  */
         RESERVED(0x361B[0x4000 - 0x361B], uint8_t)
    __IOM uint8_t  HDMI_MC_SFRDIV;                    /*!< Offset 0x4000  */
    __IOM uint8_t  HDMI_MC_CLKDIS;                    /*!< Offset 0x4001  */
    __IOM uint8_t  HDMI_MC_SWRSTZ;                    /*!< Offset 0x4002  */
    __IOM uint8_t  HDMI_MC_OPCTRL;                    /*!< Offset 0x4003  */
    __IOM uint8_t  HDMI_MC_FLOWCTRL;                  /*!< Offset 0x4004  */
    __IOM uint8_t  HDMI_MC_PHYRSTZ;                   /*!< Offset 0x4005  */
    __IOM uint8_t  HDMI_MC_LOCKONCLOCK;               /*!< Offset 0x4006  */
    __IOM uint8_t  HDMI_MC_HEACPHY_RST;               /*!< Offset 0x4007  */
         RESERVED(0x4008[0x4100 - 0x4008], uint8_t)
    __IOM uint8_t  HDMI_CSC_CFG;                      /*!< Offset 0x4100  */
    __IOM uint8_t  HDMI_CSC_SCALE;                    /*!< Offset 0x4101  */
    __IOM uint8_t  HDMI_CSC_COEF_A1_MSB;              /*!< Offset 0x4102  */
    __IOM uint8_t  HDMI_CSC_COEF_A1_LSB;              /*!< Offset 0x4103  */
    __IOM uint8_t  HDMI_CSC_COEF_A2_MSB;              /*!< Offset 0x4104  */
    __IOM uint8_t  HDMI_CSC_COEF_A2_LSB;              /*!< Offset 0x4105  */
    __IOM uint8_t  HDMI_CSC_COEF_A3_MSB;              /*!< Offset 0x4106  */
    __IOM uint8_t  HDMI_CSC_COEF_A3_LSB;              /*!< Offset 0x4107  */
    __IOM uint8_t  HDMI_CSC_COEF_A4_MSB;              /*!< Offset 0x4108  */
    __IOM uint8_t  HDMI_CSC_COEF_A4_LSB;              /*!< Offset 0x4109  */
    __IOM uint8_t  HDMI_CSC_COEF_B1_MSB;              /*!< Offset 0x410A  */
    __IOM uint8_t  HDMI_CSC_COEF_B1_LSB;              /*!< Offset 0x410B  */
    __IOM uint8_t  HDMI_CSC_COEF_B2_MSB;              /*!< Offset 0x410C  */
    __IOM uint8_t  HDMI_CSC_COEF_B2_LSB;              /*!< Offset 0x410D  */
    __IOM uint8_t  HDMI_CSC_COEF_B3_MSB;              /*!< Offset 0x410E  */
    __IOM uint8_t  HDMI_CSC_COEF_B3_LSB;              /*!< Offset 0x410F  */
    __IOM uint8_t  HDMI_CSC_COEF_B4_MSB;              /*!< Offset 0x4110  */
    __IOM uint8_t  HDMI_CSC_COEF_B4_LSB;              /*!< Offset 0x4111  */
    __IOM uint8_t  HDMI_CSC_COEF_C1_MSB;              /*!< Offset 0x4112  */
    __IOM uint8_t  HDMI_CSC_COEF_C1_LSB;              /*!< Offset 0x4113  */
    __IOM uint8_t  HDMI_CSC_COEF_C2_MSB;              /*!< Offset 0x4114  */
    __IOM uint8_t  HDMI_CSC_COEF_C2_LSB;              /*!< Offset 0x4115  */
    __IOM uint8_t  HDMI_CSC_COEF_C3_MSB;              /*!< Offset 0x4116  */
    __IOM uint8_t  HDMI_CSC_COEF_C3_LSB;              /*!< Offset 0x4117  */
    __IOM uint8_t  HDMI_CSC_COEF_C4_MSB;              /*!< Offset 0x4118  */
    __IOM uint8_t  HDMI_CSC_COEF_C4_LSB;              /*!< Offset 0x4119  */
         RESERVED(0x411A[0x5000 - 0x411A], uint8_t)
    __IOM uint8_t  HDMI_A_HDCPCFG0;                   /*!< Offset 0x5000  */
    __IOM uint8_t  HDMI_A_HDCPCFG1;                   /*!< Offset 0x5001  */
    __IOM uint8_t  HDMI_A_HDCPOBS0;                   /*!< Offset 0x5002  */
    __IOM uint8_t  HDMI_A_HDCPOBS1;                   /*!< Offset 0x5003  */
    __IOM uint8_t  HDMI_A_HDCPOBS2;                   /*!< Offset 0x5004  */
    __IOM uint8_t  HDMI_A_HDCPOBS3;                   /*!< Offset 0x5005  */
    __IOM uint8_t  HDMI_A_APIINTCLR;                  /*!< Offset 0x5006  */
    __IOM uint8_t  HDMI_A_APIINTSTAT;                 /*!< Offset 0x5007  */
    __IOM uint8_t  HDMI_A_APIINTMSK;                  /*!< Offset 0x5008  */
    __IOM uint8_t  HDMI_A_VIDPOLCFG;                  /*!< Offset 0x5009  */
    __IOM uint8_t  HDMI_A_OESSWCFG;                   /*!< Offset 0x500A  */
    __IOM uint8_t  HDMI_A_TIMER1SETUP0;               /*!< Offset 0x500B  */
    __IOM uint8_t  HDMI_A_TIMER1SETUP1;               /*!< Offset 0x500C  */
    __IOM uint8_t  HDMI_A_TIMER2SETUP0;               /*!< Offset 0x500D  */
    __IOM uint8_t  HDMI_A_TIMER2SETUP1;               /*!< Offset 0x500E  */
    __IOM uint8_t  HDMI_A_100MSCFG;                   /*!< Offset 0x500F  */
    __IOM uint8_t  HDMI_A_2SCFG0;                     /*!< Offset 0x5010  */
    __IOM uint8_t  HDMI_A_2SCFG1;                     /*!< Offset 0x5011  */
    __IOM uint8_t  HDMI_A_5SCFG0;                     /*!< Offset 0x5012  */
    __IOM uint8_t  HDMI_A_5SCFG1;                     /*!< Offset 0x5013  */
    __IOM uint8_t  HDMI_A_SRMVERLSB;                  /*!< Offset 0x5014  */
    __IOM uint8_t  HDMI_A_SRMVERMSB;                  /*!< Offset 0x5015  */
    __IOM uint8_t  HDMI_A_SRMCTRL;                    /*!< Offset 0x5016  */
    __IOM uint8_t  HDMI_A_SFRSETUP;                   /*!< Offset 0x5017  */
    __IOM uint8_t  HDMI_A_I2CHSETUP;                  /*!< Offset 0x5018  */
    __IOM uint8_t  HDMI_A_INTSETUP;                   /*!< Offset 0x5019  */
    __IOM uint8_t  HDMI_A_PRESETUP;                   /*!< Offset 0x501A  */
         RESERVED(0x501B[0x5020 - 0x501B], uint8_t)
    __IOM uint8_t  HDMI_A_SRM_BASE;                   /*!< Offset 0x5020  */
         RESERVED(0x5021[0x7D00 - 0x5021], uint8_t)
    __IOM uint8_t  HDMI_CEC_CTRL;                     /*!< Offset 0x7D00  */
    __IOM uint8_t  HDMI_CEC_STAT;                     /*!< Offset 0x7D01  */
    __IOM uint8_t  HDMI_CEC_MASK;                     /*!< Offset 0x7D02  */
    __IOM uint8_t  HDMI_CEC_POLARITY;                 /*!< Offset 0x7D03  */
    __IOM uint8_t  HDMI_CEC_INT;                      /*!< Offset 0x7D04  */
    __IOM uint8_t  HDMI_CEC_ADDR_L;                   /*!< Offset 0x7D05  */
    __IOM uint8_t  HDMI_CEC_ADDR_H;                   /*!< Offset 0x7D06  */
    __IOM uint8_t  HDMI_CEC_TX_CNT;                   /*!< Offset 0x7D07  */
    __IOM uint8_t  HDMI_CEC_RX_CNT;                   /*!< Offset 0x7D08  */
         RESERVED(0x7D09[0x7D10 - 0x7D09], uint8_t)
    __IOM uint8_t  HDMI_CEC_TX_DATA0;                 /*!< Offset 0x7D10  */
    __IOM uint8_t  HDMI_CEC_TX_DATA1;                 /*!< Offset 0x7D11  */
    __IOM uint8_t  HDMI_CEC_TX_DATA2;                 /*!< Offset 0x7D12  */
    __IOM uint8_t  HDMI_CEC_TX_DATA3;                 /*!< Offset 0x7D13  */
    __IOM uint8_t  HDMI_CEC_TX_DATA4;                 /*!< Offset 0x7D14  */
    __IOM uint8_t  HDMI_CEC_TX_DATA5;                 /*!< Offset 0x7D15  */
    __IOM uint8_t  HDMI_CEC_TX_DATA6;                 /*!< Offset 0x7D16  */
    __IOM uint8_t  HDMI_CEC_TX_DATA7;                 /*!< Offset 0x7D17  */
    __IOM uint8_t  HDMI_CEC_TX_DATA8;                 /*!< Offset 0x7D18  */
    __IOM uint8_t  HDMI_CEC_TX_DATA9;                 /*!< Offset 0x7D19  */
    __IOM uint8_t  HDMI_CEC_TX_DATA10;                /*!< Offset 0x7D1A  */
    __IOM uint8_t  HDMI_CEC_TX_DATA11;                /*!< Offset 0x7D1B  */
    __IOM uint8_t  HDMI_CEC_TX_DATA12;                /*!< Offset 0x7D1C  */
    __IOM uint8_t  HDMI_CEC_TX_DATA13;                /*!< Offset 0x7D1D  */
    __IOM uint8_t  HDMI_CEC_TX_DATA14;                /*!< Offset 0x7D1E  */
    __IOM uint8_t  HDMI_CEC_TX_DATA15;                /*!< Offset 0x7D1F  */
    __IOM uint8_t  HDMI_CEC_RX_DATA0;                 /*!< Offset 0x7D20  */
    __IOM uint8_t  HDMI_CEC_RX_DATA1;                 /*!< Offset 0x7D21  */
    __IOM uint8_t  HDMI_CEC_RX_DATA2;                 /*!< Offset 0x7D22  */
    __IOM uint8_t  HDMI_CEC_RX_DATA3;                 /*!< Offset 0x7D23  */
    __IOM uint8_t  HDMI_CEC_RX_DATA4;                 /*!< Offset 0x7D24  */
    __IOM uint8_t  HDMI_CEC_RX_DATA5;                 /*!< Offset 0x7D25  */
    __IOM uint8_t  HDMI_CEC_RX_DATA6;                 /*!< Offset 0x7D26  */
    __IOM uint8_t  HDMI_CEC_RX_DATA7;                 /*!< Offset 0x7D27  */
    __IOM uint8_t  HDMI_CEC_RX_DATA8;                 /*!< Offset 0x7D28  */
    __IOM uint8_t  HDMI_CEC_RX_DATA9;                 /*!< Offset 0x7D29  */
    __IOM uint8_t  HDMI_CEC_RX_DATA10;                /*!< Offset 0x7D2A  */
    __IOM uint8_t  HDMI_CEC_RX_DATA11;                /*!< Offset 0x7D2B  */
    __IOM uint8_t  HDMI_CEC_RX_DATA12;                /*!< Offset 0x7D2C  */
    __IOM uint8_t  HDMI_CEC_RX_DATA13;                /*!< Offset 0x7D2D  */
    __IOM uint8_t  HDMI_CEC_RX_DATA14;                /*!< Offset 0x7D2E  */
    __IOM uint8_t  HDMI_CEC_RX_DATA15;                /*!< Offset 0x7D2F  */
    __IOM uint8_t  HDMI_CEC_LOCK;                     /*!< Offset 0x7D30  */
    __IOM uint8_t  HDMI_CEC_WKUPCTRL;                 /*!< Offset 0x7D31  */
         RESERVED(0x7D32[0x7E00 - 0x7D32], uint8_t)
    __IOM uint8_t  HDMI_I2CM_SLAVE;                   /*!< Offset 0x7E00  */
    __IOM uint8_t  HDMI_I2CM_ADDRESS;                 /*!< Offset 0x7E01  */
    __IOM uint8_t  HDMI_I2CM_DATAO;                   /*!< Offset 0x7E02  */
    __IOM uint8_t  HDMI_I2CM_DATAI;                   /*!< Offset 0x7E03  */
    __IOM uint8_t  HDMI_I2CM_OPERATION;               /*!< Offset 0x7E04  */
    __IOM uint8_t  HDMI_I2CM_INT;                     /*!< Offset 0x7E05  */
    __IOM uint8_t  HDMI_I2CM_CTLINT;                  /*!< Offset 0x7E06  */
    __IOM uint8_t  HDMI_I2CM_DIV;                     /*!< Offset 0x7E07  */
    __IOM uint8_t  HDMI_I2CM_SEGADDR;                 /*!< Offset 0x7E08  */
    __IOM uint8_t  HDMI_I2CM_SOFTRSTZ;                /*!< Offset 0x7E09  */
    __IOM uint8_t  HDMI_I2CM_SEGPTR;                  /*!< Offset 0x7E0A  */
    __IOM uint8_t  HDMI_I2CM_SS_SCL_HCNT_1_ADDR;      /*!< Offset 0x7E0B  */
    __IOM uint8_t  HDMI_I2CM_SS_SCL_HCNT_0_ADDR;      /*!< Offset 0x7E0C  */
    __IOM uint8_t  HDMI_I2CM_SS_SCL_LCNT_1_ADDR;      /*!< Offset 0x7E0D  */
    __IOM uint8_t  HDMI_I2CM_SS_SCL_LCNT_0_ADDR;      /*!< Offset 0x7E0E  */
    __IOM uint8_t  HDMI_I2CM_FS_SCL_HCNT_1_ADDR;      /*!< Offset 0x7E0F  */
    __IOM uint8_t  HDMI_I2CM_FS_SCL_HCNT_0_ADDR;      /*!< Offset 0x7E10  */
    __IOM uint8_t  HDMI_I2CM_FS_SCL_LCNT_1_ADDR;      /*!< Offset 0x7E11  */
    __IOM uint8_t  HDMI_I2CM_FS_SCL_LCNT_0_ADDR;      /*!< Offset 0x7E12  */
} HDMI_TX_TypeDef; /* size of structure = 0x7E13 */
/*
 * @brief I2S_PCM
 */
/*!< I2S_PCM  */
typedef struct I2S_PCM_Type
{
    __IOM uint32_t I2S_PCM_CTL;                       /*!< Offset 0x000 I2S/PCM Control Register */
    __IOM uint32_t I2S_PCM_FMT0;                      /*!< Offset 0x004 I2S/PCM Format Register 0 */
    __IOM uint32_t I2S_PCM_FMT1;                      /*!< Offset 0x008 I2S/PCM Format Register 1 */
    __IOM uint32_t I2S_PCM_ISTA;                      /*!< Offset 0x00C I2S/PCM Interrupt Status Register */
    __IOM uint32_t I2S_PCM_RXFIFO;                    /*!< Offset 0x010 I2S/PCM RXFIFO Register */
    __IOM uint32_t I2S_PCM_FCTL;                      /*!< Offset 0x014 I2S/PCM FIFO Control Register */
    __IOM uint32_t I2S_PCM_FSTA;                      /*!< Offset 0x018 I2S/PCM FIFO Status Register */
    __IOM uint32_t I2S_PCM_INT;                       /*!< Offset 0x01C I2S/PCM DMA & Interrupt Control Register */
    __IOM uint32_t I2S_PCM_TXFIFO;                    /*!< Offset 0x020 I2S/PCM TXFIFO Register */
    __IOM uint32_t I2S_PCM_CLKD;                      /*!< Offset 0x024 I2S/PCM Clock Divide Register */
    __IOM uint32_t I2S_PCM_TXCNT;                     /*!< Offset 0x028 I2S/PCM TX Sample Counter Register */
    __IOM uint32_t I2S_PCM_RXCNT;                     /*!< Offset 0x02C I2S/PCM RX Sample Counter Register */
    __IOM uint32_t I2S_PCM_CHCFG;                     /*!< Offset 0x030 I2S/PCM Channel Configuration Register */
    __IOM uint32_t I2S_PCM_TX0CHSEL;                  /*!< Offset 0x034 I2S/PCM TX0 Channel Select Register */
    __IOM uint32_t I2S_PCM_TX1CHSEL;                  /*!< Offset 0x038 I2S/PCM TX1 Channel Select Register */
    __IOM uint32_t I2S_PCM_TX2CHSEL;                  /*!< Offset 0x03C I2S/PCM TX2 Channel Select Register */
    __IOM uint32_t I2S_PCM_TX3CHSEL;                  /*!< Offset 0x040 I2S/PCM TX3 Channel Select Register */
    __IOM uint32_t I2S_PCM_TX0CHMAP0;                 /*!< Offset 0x044 I2S/PCM TX0 Channel Mapping Register0 */
    __IOM uint32_t I2S_PCM_TX0CHMAP1;                 /*!< Offset 0x048 I2S/PCM TX0 Channel Mapping Register1 */
    __IOM uint32_t I2S_PCM_TX1CHMAP0;                 /*!< Offset 0x04C I2S/PCM TX1 Channel Mapping Register0 */
    __IOM uint32_t I2S_PCM_TX1CHMAP1;                 /*!< Offset 0x050 I2S/PCM TX1 Channel Mapping Register1 */
    __IOM uint32_t I2S_PCM_TX2CHMAP0;                 /*!< Offset 0x054 I2S/PCM TX2 Channel Mapping Register0 */
    __IOM uint32_t I2S_PCM_TX2CHMAP1;                 /*!< Offset 0x058 I2S/PCM TX2 Channel Mapping Register1 */
    __IOM uint32_t I2S_PCM_TX3CHMAP0;                 /*!< Offset 0x05C I2S/PCM TX3 Channel Mapping Register0 */
    __IOM uint32_t I2S_PCM_TX3CHMAP1;                 /*!< Offset 0x060 I2S/PCM TX3 Channel Mapping Register1 */
    __IOM uint32_t I2S_PCM_RXCHSEL;                   /*!< Offset 0x064 I2S/PCM RX Channel Select Register */
    __IOM uint32_t I2S_PCM_RXCHMAP [0x004];           /*!< Offset 0x068 I2S/PCM RX Channel Mapping Register0..3 */
         RESERVED(0x078[0x0080 - 0x0078], uint8_t)
    __IOM uint32_t MCLKCFG;                           /*!< Offset 0x080 ASRC MCLK Configuration Register */
    __IOM uint32_t FsoutCFG;                          /*!< Offset 0x084 ASRC Out Sample Rate Configuration Register */
    __IOM uint32_t FsinEXTCFG;                        /*!< Offset 0x088 ASRC Input Sample Pulse Extend Configuration Register */
    __IOM uint32_t ASRCEN;                            /*!< Offset 0x08C ASRC Enable Configure Register */
    __IOM uint32_t ASRCMANCFG;                        /*!< Offset 0x090 ASRC Manual Ratio Configuration Register */
    __IOM uint32_t ASRCRATIOSTAT;                     /*!< Offset 0x094 ASRC Status Register */
    __IOM uint32_t ASRCFIFOSTAT;                      /*!< Offset 0x098 ASRC FIFO Level Status Register */
    __IOM uint32_t ASRCMBISTCFG;                      /*!< Offset 0x09C ASRC MBIST Test Configuration Register */
    __IOM uint32_t ASRCMBISTSTAT;                     /*!< Offset 0x0A0 ASRC MBIST Test Status Register */
} I2S_PCM_TypeDef; /* size of structure = 0x0A4 */
/*
 * @brief PRCM
 */
/*!< PRCM  */
typedef struct PRCM_Type
{
    __IOM uint32_t AHBS_CLK_REG;                      /*!< Offset 0x000 AHBS Clock Register  */
         RESERVED(0x004[0x000C - 0x0004], uint8_t)
    __IOM uint32_t APBS0_CLK_REG;                     /*!< Offset 0x00C APBS0 Clock Register  */
    __IOM uint32_t APBS1_CLK_REG;                     /*!< Offset 0x010 APBS1 Clock Register  */
         RESERVED(0x014[0x0100 - 0x0014], uint8_t)
    __IOM uint32_t S_TIMER0_CLK_REG;                  /*!< Offset 0x100 S_TIMER0 Clock Register  */
    __IOM uint32_t S_TIMER1_CLK_REG;                  /*!< Offset 0x104 S_TIMER1 Clock Register  */
    __IOM uint32_t S_TIMER2_CLK_REG;                  /*!< Offset 0x108 S_TIMER2 Clock Register  */
    __IOM uint32_t S_TIMER3_CLK_REG;                  /*!< Offset 0x10C S_TIMER3 Clock Register  */
         RESERVED(0x110[0x011C - 0x0110], uint8_t)
    __IOM uint32_t S_TIMER_BGR_REG;                   /*!< Offset 0x11C S_TIMER BUS GATING RESET Register */
         RESERVED(0x120[0x012C - 0x0120], uint8_t)
    __IOM uint32_t S_TWD_BGR_REG;                     /*!< Offset 0x12C S_TWD BUS GATING RESET Register */
    __IOM uint32_t S_PWM_CLK_REG;                     /*!< Offset 0x130 S_PWM Clock Register  */
         RESERVED(0x134[0x013C - 0x0134], uint8_t)
    __IOM uint32_t S_PWM_BGR_REG;                     /*!< Offset 0x13C S_PWM BUS GATING RESET Register  */
         RESERVED(0x140[0x0150 - 0x0140], uint8_t)
    __IOM uint32_t S_SPI_CLK_REG;                     /*!< Offset 0x150 S_SPI Clock Register  */
         RESERVED(0x154[0x015C - 0x0154], uint8_t)
    __IOM uint32_t S_SPI_BGR_REG;                     /*!< Offset 0x15C S_SPI BUS GATING RESET Register  */
         RESERVED(0x160[0x017C - 0x0160], uint8_t)
    __IOM uint32_t S_MBOX_BGR_REG;                    /*!< Offset 0x17C S_MBOX BUS GATING RESET Register  */
         RESERVED(0x180[0x018C - 0x0180], uint8_t)
    __IOM uint32_t S_UART_BGR_REG;                    /*!< Offset 0x18C S_UART BUS GATING RESET Register  */
         RESERVED(0x190[0x019C - 0x0190], uint8_t)
    __IOM uint32_t S_TWI_BGR_REG;                     /*!< Offset 0x19C S_TWI BUS GATING RESET Register  */
         RESERVED(0x1A0[0x01AC - 0x01A0], uint8_t)
    __IOM uint32_t S_PPU_BGR_REG;                     /*!< Offset 0x1AC S_PPU BUS GATING RESET Register */
    __IOM uint32_t S_TZMA_BGR_REG;                    /*!< Offset 0x1B0 S_TZMA GATING RESET Register  */
         RESERVED(0x1B4[0x01BC - 0x01B4], uint8_t)
    __IOM uint32_t S_CPUS_BIST_BGR_REG;               /*!< Offset 0x1BC  */
    __IOM uint32_t S_IRRX_CLK_REG;                    /*!< Offset 0x1C0 S_IRRX Clock Register */
         RESERVED(0x1C4[0x01CC - 0x01C4], uint8_t)
    __IOM uint32_t S_IRRX_BGR_REG;                    /*!< Offset 0x1CC S_IRRX BUS GATING RESET Register */
         RESERVED(0x1D0[0x020C - 0x01D0], uint8_t)
    __IOM uint32_t RTC_BGR_REG;                       /*!< Offset 0x20C RTC BUS GATING RESET Register */
    __IOM uint32_t RISCV_24M_CLK_REG;                 /*!< Offset 0x210 RISCV_24M Clock Register */
         RESERVED(0x214[0x021C - 0x0214], uint8_t)
    __IOM uint32_t RISCV_BGR_REG;                     /*!< Offset 0x21C RISCV BUS GATING RESET Register */
         RESERVED(0x220[0x022C - 0x0220], uint8_t)
    __IOM uint32_t S_CPUCFG_BGR_REG;                  /*!< Offset 0x22C S_CPUCFG BUS GATING RESET Register */
         RESERVED(0x230[0x0290 - 0x0230], uint8_t)
    __IOM uint32_t PRCM_SEC_SWITCH_REG;               /*!< Offset 0x290 PRCM Security Switch Register  */
    __IOM uint32_t CPUX_ISO_CONFIG_REG;               /*!< Offset 0x294 CPUX Isolation Configuration Register   */
         RESERVED(0x298[0x0320 - 0x0298], uint8_t)
    __IOM uint32_t NMI_INT_CTRL_REG;                  /*!< Offset 0x320 NMI Interrupt Control Register */
    __IOM uint32_t NMI_INT_EN_REG;                    /*!< Offset 0x324 NMI Interrupt Enable Register */
    __IOM uint32_t NMI_INT_PEND_REG;                  /*!< Offset 0x328 NMI Interrupt Pending Register */
         RESERVED(0x32C[0x0338 - 0x032C], uint8_t)
    __IOM uint32_t DEV_BUS_AUTOG_CTRL_REG;            /*!< Offset 0x338 DEV_BUS_AUTOG_CTRL Register */
    __IOM uint32_t BUS_ACG_REG;                       /*!< Offset 0x33C Bus Auto Clock Gating Register */
         RESERVED(0x340[0x0360 - 0x0340], uint8_t)
    __IOM uint32_t MSRAMOC_CTRL_REG;                  /*!< Offset 0x360 MSRAMOC Control Register */
         RESERVED(0x364[0x0368 - 0x0364], uint8_t)
    __IOM uint32_t AHBS_RDY_TOUT_CTRL_REG;            /*!< Offset 0x368 AHBS Ready Timeout Control Register */
         RESERVED(0x36C[0x037C - 0x036C], uint8_t)
    __IOM uint32_t JTAG_PAD_SEL_REG;                  /*!< Offset 0x37C JTAG PAD Select Configuration Register */
         RESERVED(0x380[0x03A0 - 0x0380], uint8_t)
    __IOM uint32_t CPUX_ISO_EN_REG;                   /*!< Offset 0x3A0 CPUX ISO Enable Register */
         RESERVED(0x3A4[0x03E0 - 0x03A4], uint8_t)
    __IOM uint32_t CRY_CONFIG_REG;                    /*!< Offset 0x3E0 Crypt Configuration Register */
    __IOM uint32_t CRY_KEY_REG;                       /*!< Offset 0x3E4 Crypt Key Register */
    __IOM uint32_t CRY_EN_REG;                        /*!< Offset 0x3E8 Crypt Enable Register */
} PRCM_TypeDef; /* size of structure = 0x3EC */
/*
 * @brief RTC
 */
/*!< RTC Real Time Clock (RTC) */
typedef struct RTC_Type
{
    __IOM uint32_t LOSC_CTRL_REG;                     /*!< Offset 0x000 LOSC Control Register */
    __IOM uint32_t LOSC_AUTO_SWT_STA_REG;             /*!< Offset 0x004 LOSC Auto Switch Status Register */
    __IOM uint32_t INTOSC_CLK_PRESCAL_REG;            /*!< Offset 0x008 Internal OSC Clock Prescaler Register */
    __IOM uint32_t INTOSC_CLK_AUTO_CALI_REG;          /*!< Offset 0x00C Internal OSC Clock Auto Calibration Register */
    __IOM uint32_t RTC_DAY_REG;                       /*!< Offset 0x010 RTC Year-Month-Day Register */
    __IOM uint32_t RTC_HH_MM_SS_SET_REG;              /*!< Offset 0x014 RTC Hour-Minute-Second Register */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IOM uint32_t ALARM0_DAY_SET_REG;                /*!< Offset 0x020 Alarm 0 Day Set Register */
    __IOM uint32_t ALARM0_CUR_VLU_REG;                /*!< Offset 0x024 Alarm 0 Counter Current Value Register */
    __IOM uint32_t ALARM0_ENABLE_REG;                 /*!< Offset 0x028 Alarm 0 Enable Register */
    __IOM uint32_t ALARM0_IRQ_EN;                     /*!< Offset 0x02C Alarm 0 IRQ Enable Register */
    __IOM uint32_t ALARM0_IRQ_STA_REG;                /*!< Offset 0x030 Alarm 0 IRQ Status Register */
         RESERVED(0x034[0x0050 - 0x0034], uint8_t)
    __IOM uint32_t ALARM0_CONFIG_REG;                 /*!< Offset 0x050 Alarm 0 Config Register */
         RESERVED(0x054[0x0060 - 0x0054], uint8_t)
    __IOM uint32_t CLK32K_FOUT_CTRL_GATING_REG;       /*!< Offset 0x060 RTC_32K Fanout Control register */
         RESERVED(0x064[0x0100 - 0x0064], uint8_t)
    __IOM uint32_t GP_DATA_REGn [0x008];              /*!< Offset 0x100 (N=0~7) General Purpose Register */
         RESERVED(0x120[0x015C - 0x0120], uint8_t)
    __IOM uint32_t XO_CTRL_WP_REG;                    /*!< Offset 0x15C DCXO Control Write Protect Register */
    __IOM uint32_t XO_CTRL_REG;                       /*!< Offset 0x160 DCXO Control Register */
    __IOM uint32_t CALI_CTRL_REG;                     /*!< Offset 0x164 Calibration Control Register */
         RESERVED(0x168[0x016C - 0x0168], uint8_t)
    __IOM uint32_t XO_GATING_REG;                     /*!< Offset 0x16C DCXO Gating Ctrl Register  */
         RESERVED(0x170[0x0190 - 0x0170], uint8_t)
    __IOM uint32_t VDD_RTC_REG;                       /*!< Offset 0x190 VDD RTC Regulation Register */
         RESERVED(0x194[0x01F0 - 0x0194], uint8_t)
    __IOM uint32_t IC_CHARA_REG;                      /*!< Offset 0x1F0 IC Characteristic Register */
    __IOM uint32_t VDD_OFF_GATING_CTRL_REG;           /*!< Offset 0x1F4 VDD Off Gating Control Register */
         RESERVED(0x1F8[0x0204 - 0x01F8], uint8_t)
    __IOM uint32_t EFUSE_HV_PWRSWT_CTRL_REG;          /*!< Offset 0x204 Efuse High Voltage Power Switch Control Register */
         RESERVED(0x208[0x0310 - 0x0208], uint8_t)
    __IOM uint32_t RTC_SPI_CLK_CTRL_REG;              /*!< Offset 0x310 RTC SPI Clock Control Register */
} RTC_TypeDef; /* size of structure = 0x314 */
/*
 * @brief RTWB_RCQ
 */
/*!< RTWB_RCQ  */
typedef struct RTWB_RCQ_Type
{
    __IOM uint32_t RTWB_RCQ_IRQ;                      /*!< Offset 0x000 RTWB_RCQ_IRQ_OFFSET          (0x8200) */
    __IOM uint32_t RTWB_RCQ_STS;                      /*!< Offset 0x004 RTWB_RCQ_STS_OFFSET          (0x8204) */
         RESERVED(0x008[0x0010 - 0x0008], uint8_t)
    __IOM uint32_t RTWB_RCQ_CTL;                      /*!< Offset 0x010 RTWB_RCQ_CTL_OFFSET          (0x8210) */
} RTWB_RCQ_TypeDef; /* size of structure = 0x014 */
/*
 * @brief SID
 */
/*!< SID Security ID */
typedef struct SID_Type
{
         RESERVED(0x000[0x0040 - 0x0000], uint8_t)
    __IOM uint32_t SID_PRCTL;                         /*!< Offset 0x040  */
         RESERVED(0x044[0x0060 - 0x0044], uint8_t)
    __IOM uint32_t SID_RDKEY;                         /*!< Offset 0x060  */
         RESERVED(0x064[0x0200 - 0x0064], uint8_t)
    __IOM uint32_t SID_DATA [0x100];                  /*!< Offset 0x200 SID data (xfel display as 'sid' replay) */
         RESERVED(0x600[0x1000 - 0x0600], uint8_t)
} SID_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief SMHC
 */
/*!< SMHC SD-MMC Host Controller */
typedef struct SMHC_Type
{
    __IOM uint32_t SMHC_CTRL;                         /*!< Offset 0x000 Control Register */
    __IOM uint32_t SMHC_CLKDIV;                       /*!< Offset 0x004 Clock Control Register */
    __IOM uint32_t SMHC_TMOUT;                        /*!< Offset 0x008 Time Out Register */
    __IOM uint32_t SMHC_CTYPE;                        /*!< Offset 0x00C Bus Width Register */
    __IOM uint32_t SMHC_BLKSIZ;                       /*!< Offset 0x010 Block Size Register */
    __IOM uint32_t SMHC_BYTCNT;                       /*!< Offset 0x014 Byte Count Register */
    __IOM uint32_t SMHC_CMD;                          /*!< Offset 0x018 Command Register */
    __IOM uint32_t SMHC_CMDARG;                       /*!< Offset 0x01C Command Argument Register */
    __IOM uint32_t SMHC_RESP0;                        /*!< Offset 0x020 Response 0 Register */
    __IOM uint32_t SMHC_RESP1;                        /*!< Offset 0x024 Response 1 Register */
    __IOM uint32_t SMHC_RESP2;                        /*!< Offset 0x028 Response 2 Register */
    __IOM uint32_t SMHC_RESP3;                        /*!< Offset 0x02C Response 3 Register */
    __IOM uint32_t SMHC_INTMASK;                      /*!< Offset 0x030 Interrupt Mask Register */
    __IOM uint32_t SMHC_MINTSTS;                      /*!< Offset 0x034 Masked Interrupt Status Register */
    __IOM uint32_t SMHC_RINTSTS;                      /*!< Offset 0x038 Raw Interrupt Status Register */
    __IOM uint32_t SMHC_STATUS;                       /*!< Offset 0x03C Status Register */
    __IOM uint32_t SMHC_FIFOTH;                       /*!< Offset 0x040 FIFO Water Level Register */
    __IOM uint32_t SMHC_FUNS;                         /*!< Offset 0x044 FIFO Function Select Register */
    __IOM uint32_t SMHC_TCBCNT;                       /*!< Offset 0x048 Transferred Byte Count between Controller and Card */
    __IOM uint32_t SMHC_TBBCNT;                       /*!< Offset 0x04C Transferred Byte Count between Host Memory and Internal FIFO */
    __IOM uint32_t SMHC_DBGC;                         /*!< Offset 0x050 Current Debug Control Register */
    __IOM uint32_t SMHC_CSDC;                         /*!< Offset 0x054 CRC Status Detect Control Register (Only for SMHC2) */
    __IOM uint32_t SMHC_A12A;                         /*!< Offset 0x058 Auto Command 12 Argument Register */
    __IOM uint32_t SMHC_NTSR;                         /*!< Offset 0x05C SD New Timing Set Register (Only for SMHC0, SMHC1) */
         RESERVED(0x060[0x0078 - 0x0060], uint8_t)
    __IOM uint32_t SMHC_HWRST;                        /*!< Offset 0x078 Hardware Reset Register */
         RESERVED(0x07C[0x0080 - 0x007C], uint8_t)
    __IOM uint32_t SMHC_IDMAC;                        /*!< Offset 0x080 IDMAC Control Register */
    __IOM uint32_t SMHC_DLBA;                         /*!< Offset 0x084 Descriptor List Base Address Register */
    __IOM uint32_t SMHC_IDST;                         /*!< Offset 0x088 IDMAC Status Register */
    __IOM uint32_t SMHC_IDIE;                         /*!< Offset 0x08C IDMAC Interrupt Enable Register */
         RESERVED(0x090[0x0100 - 0x0090], uint8_t)
    __IOM uint32_t SMHC_THLD;                         /*!< Offset 0x100 Card Threshold Control Register */
    __IOM uint32_t SMHC_SFC;                          /*!< Offset 0x104 Sample FIFO Control Register (Only for SMHC2) */
    __IOM uint32_t SMHC_A23A;                         /*!< Offset 0x108 Auto Command 23 Argument Register (Only for SMHC2) */
    __IOM uint32_t EMMC_DDR_SBIT_DET;                 /*!< Offset 0x10C eMMC4.5 DDR Start Bit Detection Control Register */
    __IOM uint32_t SMHC_RES_CRC;                      /*!< Offset 0x110 Response CRC from Device (Only for SMHC0, SMHC1) */
    __IOM uint32_t SMHC_D7_D0_CRC [0x008];            /*!< Offset 0x114 CRC in Data7..Data0 from Device (Only for SMHC0, SMHC1) */
    __IOM uint32_t SMHC_CRC_STA;                      /*!< Offset 0x134 Write CRC Status Register (Only for SMHC0, SMHC1) */
    __IOM uint32_t SMHC_EXT_CMD;                      /*!< Offset 0x138 Extended Command Register (Only for SMHC2) */
    __IOM uint32_t SMHC_EXT_RESP;                     /*!< Offset 0x13C Extended Response Register (Only for SMHC2) */
    __IOM uint32_t SMHC_DRV_DL;                       /*!< Offset 0x140 Drive Delay Control Register */
    __IOM uint32_t SMHC_SAMP_DL;                      /*!< Offset 0x144 Sample Delay Control Register */
    __IOM uint32_t SMHC_DS_DL;                        /*!< Offset 0x148 Data Strobe Delay Control Register (Only for SMHC2) */
         RESERVED(0x14C[0x0200 - 0x014C], uint8_t)
    __IOM uint32_t SMHC_FIFO;                         /*!< Offset 0x200 Read/Write FIFO */
         RESERVED(0x204[0x1000 - 0x0204], uint8_t)
} SMHC_TypeDef; /* size of structure = 0x1000 */
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
    __IOM uint32_t SPI_CCR;                           /*!< Offset 0x024 SPI Clock Control Register */
    __IOM uint32_t SPI_SAMP_DL;                       /*!< Offset 0x028 SPI Sample Delay Control Register */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __IOM uint32_t SPI_MBC;                           /*!< Offset 0x030 SPI Master Burst Counter Register */
    __IOM uint32_t SPI_MTC;                           /*!< Offset 0x034 SPI Master Transmit Counter Register */
    __IOM uint32_t SPI_BCC;                           /*!< Offset 0x038 SPI Master Burst Control Register */
         RESERVED(0x03C[0x0040 - 0x003C], uint8_t)
    __IOM uint32_t SPI_BATCR;                         /*!< Offset 0x040 SPI Bit-Aligned Transfer Configure Register */
    __IOM uint32_t SPI_3W_CCR;                        /*!< Offset 0x044 SPI Bit-Aligned Clock Configuration Register */
    __IOM uint32_t SPI_TBR;                           /*!< Offset 0x048 SPI TX Bit Register */
    __IOM uint32_t SPI_RBR;                           /*!< Offset 0x04C SPI RX Bit Register */
         RESERVED(0x050[0x0088 - 0x0050], uint8_t)
    __IOM uint32_t SPI_NDMA_MODE_CTL;                 /*!< Offset 0x088 SPI Normal DMA Mode Control Register */
         RESERVED(0x08C[0x0200 - 0x008C], uint8_t)
    __IOM uint32_t SPI_TXD;                           /*!< Offset 0x200 SPI TX Data Register */
         RESERVED(0x204[0x0300 - 0x0204], uint8_t)
    __IOM uint32_t SPI_RXD;                           /*!< Offset 0x300 SPI RX Data Register */
         RESERVED(0x304[0x1000 - 0x0304], uint8_t)
} SPI_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief S_GPIO
 */
/*!< S_GPIO Secure Port Controller */
typedef struct S_GPIO_Type
{
    __IOM uint32_t CFG [0x004];                       /*!< Offset 0x000 Configure Register */
    __IOM uint32_t DATA;                              /*!< Offset 0x010 Data Register */
    __IOM uint32_t DRV [0x004];                       /*!< Offset 0x014 Multi_Driving Register */
    __IOM uint32_t PULL [0x002];                      /*!< Offset 0x024 Pull Register */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
} S_GPIO_TypeDef; /* size of structure = 0x030 */
/*
 * @brief S_GPIOBLOCK
 */
/*!< S_GPIOBLOCK S_GPIO Port Controller */
typedef struct S_GPIOBLOCK_Type
{
    struct
    {
        __IOM uint32_t CFG [0x004];                   /*!< Offset 0x000 Configure Register */
        __IOM uint32_t DATA;                          /*!< Offset 0x010 Data Register */
        __IOM uint32_t DRV [0x004];                   /*!< Offset 0x014 Multi_Driving Register */
        __IOM uint32_t PULL [0x002];                  /*!< Offset 0x024 Pull Register */
             RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    } GPIO_PINS [0x002];                              /*!< Offset 0x000 GPIO pin control L, M */
         RESERVED(0x060[0x0200 - 0x0060], uint8_t)
    struct
    {
        __IOM uint32_t EINT_CFG [0x004];              /*!< Offset 0x200 External Interrupt Configure Registers */
        __IOM uint32_t EINT_CTL;                      /*!< Offset 0x210 External Interrupt Control Register */
        __IOM uint32_t EINT_STATUS;                   /*!< Offset 0x214 External Interrupt Status Register */
        __IOM uint32_t EINT_DEB;                      /*!< Offset 0x218 External Interrupt Debounce Register */
             RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    } GPIO_INTS [0x002];                              /*!< Offset 0x200 GPIO interrupt control */
         RESERVED(0x240[0x0340 - 0x0240], uint8_t)
    __IOM uint32_t GPIO_POW_MOD_SEL;                  /*!< Offset 0x340 PIO Group Withstand Voltage Mode Select Register */
    __IOM uint32_t GPIO_POW_MS_CTL;                   /*!< Offset 0x344 PIO Group Withstand Voltage Mode Select Control Register */
    __IOM uint32_t GPIO_POW_VAL;                      /*!< Offset 0x348 PIO Group Power Value Register */
} S_GPIOBLOCK_TypeDef; /* size of structure = 0x34C */
/*
 * @brief TCON_LCD
 */
/*!< TCON_LCD Timing Controller_LCD (TCON_LCD) */
typedef struct TCON_LCD_Type
{
    __IOM uint32_t LCD_GCTL_REG;                      /*!< Offset 0x000 LCD Global Control Register */
    __IOM uint32_t LCD_GINT0_REG;                     /*!< Offset 0x004 LCD Global Interrupt Register0 */
    __IOM uint32_t LCD_GINT1_REG;                     /*!< Offset 0x008 LCD Global Interrupt Register1 */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IOM uint32_t LCD_FRM_CTL_REG;                   /*!< Offset 0x010 LCD FRM Control Register */
    __IOM uint32_t LCD_FRM_SEED_REG [0x006];          /*!< Offset 0x014 LCD FRM Seed Register (N=0,1,2,3,4,5) 0x0014+N*0x04 */
    __IOM uint32_t LCD_FRM_TAB_REG;                   /*!< Offset 0x02C 4 LCD FRM Table Register (N=0,1,2,3) 0x002C+N*0x04 */
         RESERVED(0x030[0x003C - 0x0030], uint8_t)
    __IOM uint32_t LCD_3D_FIFO_REG;                   /*!< Offset 0x03C LCD 3D FIFO Register */
    __IOM uint32_t LCD_CTL_REG;                       /*!< Offset 0x040 LCD Control Register */
    __IOM uint32_t LCD_DCLK_REG;                      /*!< Offset 0x044 LCD Data Clock Register */
    __IOM uint32_t LCD_BASIC0_REG;                    /*!< Offset 0x048 LCD Basic Timing Register0 */
    __IOM uint32_t LCD_BASIC1_REG;                    /*!< Offset 0x04C LCD Basic Timing Register1 */
    __IOM uint32_t LCD_BASIC2_REG;                    /*!< Offset 0x050 LCD Basic Timing Register2 */
    __IOM uint32_t LCD_BASIC3_REG;                    /*!< Offset 0x054 LCD Basic Timing Register3 */
    __IOM uint32_t LCD_HV_IF_REG;                     /*!< Offset 0x058 LCD HV Panel Interface Register */
         RESERVED(0x05C[0x0060 - 0x005C], uint8_t)
    __IOM uint32_t LCD_CPU_IF_REG;                    /*!< Offset 0x060 LCD CPU Panel Interface Register */
    __IOM uint32_t LCD_CPU_WR_REG;                    /*!< Offset 0x064 LCD CPU Panel Write Data Regist er */
    __IOM uint32_t LCD_CPU_RD0_REG;                   /*!< Offset 0x068 LCD CPU Panel Read Data Register0 */
    __IOM uint32_t LCD_CPU_RD1_REG;                   /*!< Offset 0x06C LCD CPU Panel Read Data Register1 */
         RESERVED(0x070[0x0084 - 0x0070], uint8_t)
    __IOM uint32_t LCD_LVDS_IF_REG;                   /*!< Offset 0x084 LCD LVDS Configure Register - AKA tcon0_lvds_ctl */
    __IOM uint32_t LCD_IO_POL_REG;                    /*!< Offset 0x088 LCD IO Polarity Register */
    __IOM uint32_t LCD_IO_TRI_REG;                    /*!< Offset 0x08C LCD IO Control Register */
         RESERVED(0x090[0x00FC - 0x0090], uint8_t)
    __IOM uint32_t LCD_DEBUG_REG;                     /*!< Offset 0x0FC LCD Debug Register */
    __IOM uint32_t LCD_CEU_CTL_REG;                   /*!< Offset 0x100 LCD CEU Control Register */
         RESERVED(0x104[0x0110 - 0x0104], uint8_t)
    __IOM uint32_t LCD_CEU_COEF_MUL_REG [0x001];      /*!< Offset 0x110 LCD CEU Coefficient Register0(N=0..10) 0x0110+N*0x04 */
         RESERVED(0x114[0x011C - 0x0114], uint8_t)
    __IOM uint32_t LCD_CEU_COEF_ADD_REG [0x003];      /*!< Offset 0x11C LCD CEU Coefficient Register1(N=0,1,2) 0x011C+N*0x10 */
         RESERVED(0x128[0x0140 - 0x0128], uint8_t)
    __IOM uint32_t LCD_CEU_COEF_RANG_REG [0x003];     /*!< Offset 0x140 LCD CEU Coefficient Register2(N=0,1,2) 0x0140+N*0x04 */
         RESERVED(0x14C[0x0160 - 0x014C], uint8_t)
    __IOM uint32_t LCD_CPU_TRI0_REG;                  /*!< Offset 0x160 LCD CPU Panel Trigger Register0 */
    __IOM uint32_t LCD_CPU_TRI1_REG;                  /*!< Offset 0x164 LCD CPU Panel Trigger Register1 */
    __IOM uint32_t LCD_CPU_TRI2_REG;                  /*!< Offset 0x168 LCD CPU Panel Trigger Register2 */
    __IOM uint32_t LCD_CPU_TRI3_REG;                  /*!< Offset 0x16C LCD CPU Panel Trigger Register3 */
    __IOM uint32_t LCD_CPU_TRI4_REG;                  /*!< Offset 0x170 LCD CPU Panel Trigger Register4 */
    __IOM uint32_t LCD_CPU_TRI5_REG;                  /*!< Offset 0x174 LCD CPU Panel Trigger Register5 */
         RESERVED(0x178[0x0180 - 0x0178], uint8_t)
    __IOM uint32_t LCD_CMAP_CTL_REG;                  /*!< Offset 0x180 LCD Color Map Control Register */
         RESERVED(0x184[0x0190 - 0x0184], uint8_t)
    __IOM uint32_t LCD_CMAP_ODD0_REG;                 /*!< Offset 0x190 LCD Color Map Odd Line Register0 */
    __IOM uint32_t LCD_CMAP_ODD1_REG;                 /*!< Offset 0x194 LCD Color Map Odd Line Register1 */
    __IOM uint32_t LCD_CMAP_EVEN0_REG;                /*!< Offset 0x198 LCD Color Map Even Line Register0 */
    __IOM uint32_t LCD_CMAP_EVEN1_REG;                /*!< Offset 0x19C LCD Color Map Even Line Register1 */
         RESERVED(0x1A0[0x01F0 - 0x01A0], uint8_t)
    __IOM uint32_t LCD_SAFE_PERIOD_REG;               /*!< Offset 0x1F0 LCD Safe Period Register */
         RESERVED(0x1F4[0x0200 - 0x01F4], uint8_t)
    __IOM uint32_t tcon_mul_ctl;                      /*!< Offset 0x200 https://github.com/qiaoweibiao/T507_Kernel/blob/98fcc7d3f112e51b0edfb71536da89cb2115106f/drivers/video/fbdev/sunxi/disp2/disp/de/lowlevel_sun50iw1/de_lcd_type.h#L691 */
         RESERVED(0x204[0x0220 - 0x0204], uint8_t)
    __IOM uint32_t LCD_LVDS_ANA_REG [0x002];          /*!< Offset 0x220 LCD LVDS Analog Register 0/1 LCD_LVDS0_ANA_REG LCD_LVDS1_ANA_REG */
         RESERVED(0x228[0x023C - 0x0228], uint8_t)
    __IOM uint32_t LCD_FSYNC_GEN_CTRL_REG;            /*!< Offset 0x23C Module Enable and Output Value Register */
    __IOM uint32_t LCD_FSYNC_GEN_DLY_REG;             /*!< Offset 0x240 Fsync Active Time Register */
         RESERVED(0x244[0x0400 - 0x0244], uint8_t)
    __IOM uint32_t LCD_GAMMA_TABLE_REG [0x100];       /*!< Offset 0x400 LCD Gamma Table Register 0x0400-0x07FF */
         RESERVED(0x800[0x0FF4 - 0x0800], uint8_t)
    __IOM uint32_t LCD_3D_FIFO_BIST_REG;              /*!< Offset 0xFF4 LCD 3D FIFO Bist Register */
    __IOM uint32_t LCD_TRI_FIFO_BIST_REG;             /*!< Offset 0xFF8 LCD Trigger FIFO Bist Register */
         RESERVED(0xFFC[0x1000 - 0x0FFC], uint8_t)
} TCON_LCD_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief TCON_TV
 */
/*!< TCON_TV TV Output */
typedef struct TCON_TV_Type
{
    __IOM uint32_t TV_GCTL_REG;                       /*!< Offset 0x000 TV Global Control Register */
    __IOM uint32_t TV_GINT0_REG;                      /*!< Offset 0x004 TV Global Interrupt Register0 */
    __IOM uint32_t TV_GINT1_REG;                      /*!< Offset 0x008 TV Global Interrupt Register1 */
         RESERVED(0x00C[0x0040 - 0x000C], uint8_t)
    __IOM uint32_t TV_SRC_CTL_REG;                    /*!< Offset 0x040 TV Source Control Register */
         RESERVED(0x044[0x0088 - 0x0044], uint8_t)
    __IOM uint32_t TV_IO_POL_REG;                     /*!< Offset 0x088 TV IO Polarity Register */
    __IOM uint32_t TV_IO_TRI_REG;                     /*!< Offset 0x08C TV IO Control Register */
    __IOM uint32_t TV_CTL_REG;                        /*!< Offset 0x090 TV Control Register */
    __IOM uint32_t TV_BASIC0_REG;                     /*!< Offset 0x094 TV Basic Timing Register0 */
    __IOM uint32_t TV_BASIC1_REG;                     /*!< Offset 0x098 TV Basic Timing Register1 */
    __IOM uint32_t TV_BASIC2_REG;                     /*!< Offset 0x09C TV Basic Timing Register2 */
    __IOM uint32_t TV_BASIC3_REG;                     /*!< Offset 0x0A0 TV Basic Timing Regi ster3 */
    __IOM uint32_t TV_BASIC4_REG;                     /*!< Offset 0x0A4 TV Basic Timing Register4 */
    __IOM uint32_t TV_BASIC5_REG;                     /*!< Offset 0x0A8 TV Basic Timing Register5 */
         RESERVED(0x0AC[0x00F8 - 0x00AC], uint8_t)
    __IOM uint32_t TV_ECC_FIFO_REG;                   /*!< Offset 0x0F8 TV ECC FIFO Register */
    __IOM uint32_t TV_DEBUG_REG;                      /*!< Offset 0x0FC TV Debug Register */
    __IOM uint32_t TV_CEU_CTL_REG;                    /*!< Offset 0x100 TV CEU Control Register */
         RESERVED(0x104[0x0110 - 0x0104], uint8_t)
    __IOM uint32_t TV_CEU_COEF_MUL_REG [0x00B];       /*!< Offset 0x110 TV CEU Coefficient MUL R egister (N=0..10) */
         RESERVED(0x13C[0x0140 - 0x013C], uint8_t)
    __IOM uint32_t TV_CEU_COEF_RANG_REG [0x003];      /*!< Offset 0x140 TV CEU Coefficient Range R egister (N=0,1,2) */
         RESERVED(0x14C[0x01F0 - 0x014C], uint8_t)
    __IOM uint32_t TV_SAFE_PERIOD_REG;                /*!< Offset 0x1F0 TV Safe Period Register */
         RESERVED(0x1F4[0x0200 - 0x01F4], uint8_t)
    __IOM uint32_t tcon_mul_ctl;                      /*!< Offset 0x200 https://github.com/qiaoweibiao/T507_Kernel/blob/98fcc7d3f112e51b0edfb71536da89cb2115106f/drivers/video/fbdev/sunxi/disp2/disp/de/lowlevel_sun50iw1/de_lcd_type.h#L691 */
         RESERVED(0x204[0x0300 - 0x0204], uint8_t)
    __IOM uint32_t TV_FILL_CTL_REG;                   /*!< Offset 0x300 TV Fill Data Control Register */
    struct
    {
        __IOM uint32_t TV_FILL_BEGIN_REG;             /*!< Offset 0x304 TV Fill Data Begin Register 0x0304+N*0x0C(N=0..2) */
        __IOM uint32_t TV_FILL_END_REG;               /*!< Offset 0x308 TV Fill Data End Register  0x0308+N*0x0C(N=0..2) */
        __IOM uint32_t TV_FILL_DATA_REG;              /*!< Offset 0x30C TV Fill Data Value Register  0x030C+N*0x0C(N=0..2) */
    } TV_FILL [0x003];                                /*!< Offset 0x304 V Fill Data 0..2 */
         RESERVED(0x328[0x0330 - 0x0328], uint8_t)
    __IOM uint32_t TV_DATA_IO_POL0_REG;               /*!< Offset 0x330 TV Data IO Polarity0 Register */
    __IOM uint32_t TV_DATA_IO_POL1_REG;               /*!< Offset 0x334 TV Data IO Polarity1 Register */
    __IOM uint32_t TV_DATA_IO_TRI0_REG;               /*!< Offset 0x338 TV Data IO Trigger0 Register */
    __IOM uint32_t TV_DATA_IO_TRI1_REG;               /*!< Offset 0x33C TV Data IO Trigger1 Register */
    __IOM uint32_t TV_PIXELDEPTH_MODE_REG;            /*!< Offset 0x340 TV Pixel */
         RESERVED(0x344[0x1000 - 0x0344], uint8_t)
} TCON_TV_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief TIMER
 */
/*!< TIMER  */
typedef struct TIMER_Type
{
    __IOM uint32_t TMR_IRQ_EN_REG;                    /*!< Offset 0x000 Timer IRQ Enable Register */
    __IOM uint32_t TMR_IRQ_STA_REG;                   /*!< Offset 0x004 Timer Status Register */
    __IOM uint32_t TIMER_SEC_REG;                     /*!< Offset 0x008 Timer Secure Register */
         RESERVED(0x00C[0x0020 - 0x000C], uint8_t)
    struct
    {
        __IOM uint32_t CTRL_REG;                      /*!< Offset 0x020 Timer n Control Register */
        __IOM uint32_t TIMER_IVL_REG;                 /*!< Offset 0x024 TimerLowIntervalValueRegister */
        __IOM uint32_t TIMER_CVL_REG;                 /*!< Offset 0x028 TimerLowCurrentValueRegister */
        __IOM uint32_t TIMER_IVH_REG;                 /*!< Offset 0x02C TimerHighIntervalValueRegiste */
        __IOM uint32_t TIMER_CVH_REG;                 /*!< Offset 0x030 TimerHighCurrentValueRegister */
        __IOM uint32_t TIMER_CV_READ_SEL_REG;         /*!< Offset 0x034 TimerCurrentValueReadSelectRegister */
             RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    } TMR [0x002];                                    /*!< Offset 0x020 Timer */
         RESERVED(0x060[0x0400 - 0x0060], uint8_t)
} TIMER_TypeDef; /* size of structure = 0x400 */
/*
 * @brief TIMER1
 */
/*!< TIMER1  */
typedef struct TIMER1_Type
{
    __IOM uint32_t TMR_IRQ_EN_REG;                    /*!< Offset 0x000 Timer IRQ Enable Register */
    __IOM uint32_t TMR_IRQ_STA_REG;                   /*!< Offset 0x004 Timer Status Register */
         RESERVED(0x008[0x0010 - 0x0008], uint8_t)
    struct
    {
        __IOM uint32_t CTRL_REG;                      /*!< Offset 0x010 Timer n Control Register */
        __IOM uint32_t INTV_VALUE_REG;                /*!< Offset 0x014 Timer n Interval Value Register */
        __IOM uint32_t CUR_VALUE_REG;                 /*!< Offset 0x018 Timer n Current Value Register */
             RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    } TMR [0x004];                                    /*!< Offset 0x010 Timer */
         RESERVED(0x050[0x0400 - 0x0050], uint8_t)
} TIMER1_TypeDef; /* size of structure = 0x400 */
/*
 * @brief TIMESTAMP_CTRL
 */
/*!< TIMESTAMP_CTRL  */
typedef struct TIMESTAMP_CTRL_Type
{
    __IOM uint32_t TSTAMP_CTRL_REG;                   /*!< Offset 0x000 Timestamp Control Register */
         RESERVED(0x004[0x0008 - 0x0004], uint8_t)
    __IOM uint32_t CNT_CTRL_LOW_REG;                  /*!< Offset 0x008 Counter Low Register */
    __IOM uint32_t CNT_CTRL_HI_REG;                   /*!< Offset 0x00C Counter High Register */
         RESERVED(0x010[0x0020 - 0x0010], uint8_t)
    __IOM uint32_t CNT_FREQID_REG;                    /*!< Offset 0x020 Counte rBase Frequency ID Register */
} TIMESTAMP_CTRL_TypeDef; /* size of structure = 0x024 */
/*
 * @brief TIMESTAMP_STA
 */
/*!< TIMESTAMP_STA  */
typedef struct TIMESTAMP_STA_Type
{
    __IOM uint32_t CNT_LOW_REG;                       /*!< Offset 0x000 Counter Low Register */
    __IOM uint32_t CNT_HI_REG;                        /*!< Offset 0x004 Counter High Register */
} TIMESTAMP_STA_TypeDef; /* size of structure = 0x008 */
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
         RESERVED(0x08C[0x00B0 - 0x008C], uint8_t)
    __IOM uint32_t UART_DBG_DLL;                      /*!< Offset 0x0B0 UART Debug DLL Register */
    __IOM uint32_t UART_DBG_DLH;                      /*!< Offset 0x0B4 UART Debug DLH Register */
         RESERVED(0x0B8[0x00C0 - 0x00B8], uint8_t)
    __IOM uint32_t UART_485_CTL;                      /*!< Offset 0x0C0 UART RS485 Control and Status Register */
    __IOM uint32_t RS485_ADDR_MATCH;                  /*!< Offset 0x0C4 UART RS485 Addres Match Register  */
    __IOM uint32_t BUS_IDLE_CHECK;                    /*!< Offset 0x0C8 UART RS485 Bus Idle Check Register */
    __IOM uint32_t TX_DLY;                            /*!< Offset 0x0CC UART TX Delay Register */
         RESERVED(0x0D0[0x00D4 - 0x00D0], uint8_t)
    __IOM uint32_t FIFO_TM;                           /*!< Offset 0x0D4 FIFO TEST MODE Register */
    __IOM uint32_t READ_TX_FIFO;                      /*!< Offset 0x0D8 READ TX FIFO Register */
    __IOM uint32_t WRITE_RX_FIFO;                     /*!< Offset 0x0DC WRITE RX FIFO Register */
    __IOM uint32_t READ_RX_FIFO;                      /*!< Offset 0x0E0 READ RX FIFO Register */
    __IOM uint32_t RTSN_RST_DLY_TIME;                 /*!< Offset 0x0E4 RS485 RTSN PULL DOWM DELAY TIME Register */
         RESERVED(0x0E8[0x00F0 - 0x00E8], uint8_t)
    __IOM uint32_t UART_FCC;                          /*!< Offset 0x0F0 UART FIFO Clock Control Register */
} UART_TypeDef; /* size of structure = 0x0F4 */
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

#define GPIOINTD ((GPIOINT_TypeDef *) GPIOINTD_BASE)  /*!< GPIOINTD  register set access pointer */
#define DE_DNS1 ((DE_DNS_TypeDef *) DE_DNS1_BASE)     /*!< DE_DNS1 Denoise (DNS) register set access pointer */
#define DE_DNS2 ((DE_DNS_TypeDef *) DE_DNS2_BASE)     /*!< DE_DNS2 Denoise (DNS) register set access pointer */
#define DE_DNS3 ((DE_DNS_TypeDef *) DE_DNS3_BASE)     /*!< DE_DNS3 Denoise (DNS) register set access pointer */
#define DE_DNS4 ((DE_DNS_TypeDef *) DE_DNS4_BASE)     /*!< DE_DNS4 Denoise (DNS) register set access pointer */
#define DE_DNS5 ((DE_DNS_TypeDef *) DE_DNS5_BASE)     /*!< DE_DNS5 Denoise (DNS) register set access pointer */
#define DE_DNS6 ((DE_DNS_TypeDef *) DE_DNS6_BASE)     /*!< DE_DNS6 Denoise (DNS) register set access pointer */
#define GPIOBLOCK ((GPIOBLOCK_TypeDef *) GPIOBLOCK_BASE)/*!< GPIOBLOCK Port Controller register set access pointer */
#define GPIOB ((GPIO_TypeDef *) GPIOB_BASE)           /*!< GPIOB Port Controller register set access pointer */
#define GPIOINTB ((GPIOINT_TypeDef *) GPIOINTB_BASE)  /*!< GPIOINTB  register set access pointer */
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE)           /*!< GPIOC Port Controller register set access pointer */
#define GPIOINTC ((GPIOINT_TypeDef *) GPIOINTC_BASE)  /*!< GPIOINTC  register set access pointer */
#define GPIOD ((GPIO_TypeDef *) GPIOD_BASE)           /*!< GPIOD Port Controller register set access pointer */
#define GPIOE ((GPIO_TypeDef *) GPIOE_BASE)           /*!< GPIOE Port Controller register set access pointer */
#define GPIOINTE ((GPIOINT_TypeDef *) GPIOINTE_BASE)  /*!< GPIOINTE  register set access pointer */
#define GPIOF ((GPIO_TypeDef *) GPIOF_BASE)           /*!< GPIOF Port Controller register set access pointer */
#define GPIOINTF ((GPIOINT_TypeDef *) GPIOINTF_BASE)  /*!< GPIOINTF  register set access pointer */
#define GPIOG ((GPIO_TypeDef *) GPIOG_BASE)           /*!< GPIOG Port Controller register set access pointer */
#define GPIOINTG ((GPIOINT_TypeDef *) GPIOINTG_BASE)  /*!< GPIOINTG  register set access pointer */
#define GPIOH ((GPIO_TypeDef *) GPIOH_BASE)           /*!< GPIOH Port Controller register set access pointer */
#define GPIOINTH ((GPIOINT_TypeDef *) GPIOINTH_BASE)  /*!< GPIOINTH  register set access pointer */
#define GPIOJ ((GPIO_TypeDef *) GPIOJ_BASE)           /*!< GPIOJ Port Controller register set access pointer */
#define GPIOINTJ ((GPIOINT_TypeDef *) GPIOINTJ_BASE)  /*!< GPIOINTJ  register set access pointer */
#define GPIOK ((GPIO_TypeDef *) GPIOK_BASE)           /*!< GPIOK Port Controller register set access pointer */
#define GPIOINTK ((GPIOINT_TypeDef *) GPIOINTK_BASE)  /*!< GPIOINTK  register set access pointer */
#define CCU ((CCU_TypeDef *) CCU_BASE)                /*!< CCU Clock Controller Unit (CCU) register set access pointer */
#define I2S0 ((I2S_PCM_TypeDef *) I2S0_BASE)          /*!< I2S0  register set access pointer */
#define I2S1 ((I2S_PCM_TypeDef *) I2S1_BASE)          /*!< I2S1  register set access pointer */
#define I2S2 ((I2S_PCM_TypeDef *) I2S2_BASE)          /*!< I2S2  register set access pointer */
#define I2S3 ((I2S_PCM_TypeDef *) I2S3_BASE)          /*!< I2S3  register set access pointer */
#define I2S4 ((I2S_PCM_TypeDef *) I2S4_BASE)          /*!< I2S4  register set access pointer */
#define Timer1_CPUX ((TIMER1_TypeDef *) Timer1_CPUX_BASE)/*!< Timer1_CPUX  register set access pointer */
#define CPUS_INTERRUPT_CTRL ((CPUS_INTERRUPT_CTRL_TypeDef *) CPUS_INTERRUPT_CTRL_BASE)/*!< CPUS_INTERRUPT_CTRL  register set access pointer */
#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0  register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1  register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2  register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3  register set access pointer */
#define UART4 ((UART_TypeDef *) UART4_BASE)           /*!< UART4  register set access pointer */
#define UART5 ((UART_TypeDef *) UART5_BASE)           /*!< UART5  register set access pointer */
#define UART6 ((UART_TypeDef *) UART6_BASE)           /*!< UART6  register set access pointer */
#define TWI0 ((TWI_TypeDef *) TWI0_BASE)              /*!< TWI0 Two Wire Interface (TWI) register set access pointer */
#define TWI1 ((TWI_TypeDef *) TWI1_BASE)              /*!< TWI1 Two Wire Interface (TWI) register set access pointer */
#define TWI2 ((TWI_TypeDef *) TWI2_BASE)              /*!< TWI2 Two Wire Interface (TWI) register set access pointer */
#define TWI3 ((TWI_TypeDef *) TWI3_BASE)              /*!< TWI3 Two Wire Interface (TWI) register set access pointer */
#define TWI4 ((TWI_TypeDef *) TWI4_BASE)              /*!< TWI4 Two Wire Interface (TWI) register set access pointer */
#define TWI5 ((TWI_TypeDef *) TWI5_BASE)              /*!< TWI5 Two Wire Interface (TWI) register set access pointer */
#define TWI6 ((TWI_TypeDef *) TWI6_BASE)              /*!< TWI6 Two Wire Interface (TWI) register set access pointer */
#define TWI7 ((TWI_TypeDef *) TWI7_BASE)              /*!< TWI7 Two Wire Interface (TWI) register set access pointer */
#define TWI8 ((TWI_TypeDef *) TWI8_BASE)              /*!< TWI8 Two Wire Interface (TWI) register set access pointer */
#define TWI9 ((TWI_TypeDef *) TWI9_BASE)              /*!< TWI9 Two Wire Interface (TWI) register set access pointer */
#define TWI10 ((TWI_TypeDef *) TWI10_BASE)            /*!< TWI10 Two Wire Interface (TWI) register set access pointer */
#define TWI11 ((TWI_TypeDef *) TWI11_BASE)            /*!< TWI11 Two Wire Interface (TWI) register set access pointer */
#define TWI12 ((TWI_TypeDef *) TWI12_BASE)            /*!< TWI12 Two Wire Interface (TWI) register set access pointer */
#define SPI0 ((SPI_TypeDef *) SPI0_BASE)              /*!< SPI0 Serial Peripheral Interface register set access pointer */
#define SPI1 ((SPI_TypeDef *) SPI1_BASE)              /*!< SPI1 Serial Peripheral Interface register set access pointer */
#define SPI2 ((SPI_TypeDef *) SPI2_BASE)              /*!< SPI2 Serial Peripheral Interface register set access pointer */
#define SPI3 ((SPI_TypeDef *) SPI3_BASE)              /*!< SPI3 Serial Peripheral Interface register set access pointer */
#define SID ((SID_TypeDef *) SID_BASE)                /*!< SID Security ID register set access pointer */
#define GICD ((GICD_TypeDef *) GICD_BASE)             /*!< GICD GIC Distributor register set access pointer */
#define GICT ((GICT_TypeDef *) GICT_BASE)             /*!< GICT GIC ITS translation register set access pointer */
#define GICP ((GICP_TypeDef *) GICP_BASE)             /*!< GICP GIC Performance Monitoring Unit register set access pointer */
#define GITS ((GITS_TypeDef *) GITS_BASE)             /*!< GITS GIC Secure Access Control register set access pointer */
#define GICR0 ((GICR_TypeDef *) GICR0_BASE)           /*!< GICR0 GIC Redistributor  register set access pointer */
#define SMHC0 ((SMHC_TypeDef *) SMHC0_BASE)           /*!< SMHC0 SD-MMC Host Controller register set access pointer */
#define SMHC1 ((SMHC_TypeDef *) SMHC1_BASE)           /*!< SMHC1 SD-MMC Host Controller register set access pointer */
#define SMHC2 ((SMHC_TypeDef *) SMHC2_BASE)           /*!< SMHC2 SD-MMC Host Controller register set access pointer */
#define SMHC3 ((SMHC_TypeDef *) SMHC3_BASE)           /*!< SMHC3 SD-MMC Host Controller register set access pointer */
#define DMA1 ((DMAC_TypeDef *) DMA1_BASE)             /*!< DMA1  register set access pointer */
#define USB0_DEVICE ((USBOTG_TypeDef *) USB0_DEVICE_BASE)/*!< USB0_DEVICE USB OTG Dual-Role Device controller register set access pointer */
#define USB0_EHCI ((USB_EHCI_Capability_TypeDef *) USB0_EHCI_BASE)/*!< USB0_EHCI  register set access pointer */
#define USB1_EHCI ((USB_EHCI_Capability_TypeDef *) USB1_EHCI_BASE)/*!< USB1_EHCI  register set access pointer */
#define USB1_OHCI ((USB_OHCI_Capability_TypeDef *) USB1_OHCI_BASE)/*!< USB1_OHCI  register set access pointer */
#define DMA0 ((DMAC_TypeDef *) DMA0_BASE)             /*!< DMA0  register set access pointer */
#define DE_TOP ((DE_TOP_TypeDef *) DE_TOP_BASE)       /*!< DE_TOP Display Engine (DE) TOP (APB) register set access pointer */
#define DE_MIXER0_GLB ((DE_GLB_TypeDef *) DE_MIXER0_GLB_BASE)/*!< DE_MIXER0_GLB Display Engine (DE) - Global Control register set access pointer */
#define DE_MIXER1_GLB ((DE_GLB_TypeDef *) DE_MIXER1_GLB_BASE)/*!< DE_MIXER1_GLB Display Engine (DE) - Global Control register set access pointer */
#define RTWB_RCQ ((RTWB_RCQ_TypeDef *) RTWB_RCQ_BASE) /*!< RTWB_RCQ  register set access pointer */
#define DE_VI1 ((DE_VI_TypeDef *) DE_VI1_BASE)        /*!< DE_VI1 Display Engine (DE) - VI surface register set access pointer */
#define USB0_OHCI ((USB_OHCI_Capability_TypeDef *) USB0_OHCI_BASE)/*!< USB0_OHCI  register set access pointer */
#define DE_VSU1 ((DE_VSU_TypeDef *) DE_VSU1_BASE)     /*!< DE_VSU1 Video Scaler Unit (VSU) register set access pointer */
#define DE_FCE1 ((DE_FCE_TypeDef *) DE_FCE1_BASE)     /*!< DE_FCE1 Fresh and Contrast Enhancement (FCE) register set access pointer */
#define DE_BLS1 ((DE_BLS_TypeDef *) DE_BLS1_BASE)     /*!< DE_BLS1 Blue Level Stretch (BLS) register set access pointer */
#define DE_FCC1 ((DE_FCC_TypeDef *) DE_FCC1_BASE)     /*!< DE_FCC1 Fancy color curvature (FCC) register set access pointer */
#define DE_VI2 ((DE_VI_TypeDef *) DE_VI2_BASE)        /*!< DE_VI2 Display Engine (DE) - VI surface register set access pointer */
#define DE_VSU2 ((DE_VSU_TypeDef *) DE_VSU2_BASE)     /*!< DE_VSU2 Video Scaler Unit (VSU) register set access pointer */
#define DE_FCE2 ((DE_FCE_TypeDef *) DE_FCE2_BASE)     /*!< DE_FCE2 Fresh and Contrast Enhancement (FCE) register set access pointer */
#define DE_BLS2 ((DE_BLS_TypeDef *) DE_BLS2_BASE)     /*!< DE_BLS2 Blue Level Stretch (BLS) register set access pointer */
#define DE_FCC2 ((DE_FCC_TypeDef *) DE_FCC2_BASE)     /*!< DE_FCC2 Fancy color curvature (FCC) register set access pointer */
#define DE_VI3 ((DE_VI_TypeDef *) DE_VI3_BASE)        /*!< DE_VI3 Display Engine (DE) - VI surface register set access pointer */
#define DE_VSU3 ((DE_VSU_TypeDef *) DE_VSU3_BASE)     /*!< DE_VSU3 Video Scaler Unit (VSU) register set access pointer */
#define DE_FCE3 ((DE_FCE_TypeDef *) DE_FCE3_BASE)     /*!< DE_FCE3 Fresh and Contrast Enhancement (FCE) register set access pointer */
#define DE_BLS3 ((DE_BLS_TypeDef *) DE_BLS3_BASE)     /*!< DE_BLS3 Blue Level Stretch (BLS) register set access pointer */
#define DE_FCC3 ((DE_FCC_TypeDef *) DE_FCC3_BASE)     /*!< DE_FCC3 Fancy color curvature (FCC) register set access pointer */
#define DE_UI1 ((DE_UI_TypeDef *) DE_UI1_BASE)        /*!< DE_UI1 Display Engine (DE) - UI surface register set access pointer */
#define DE_UIS1 ((DE_UIS_TypeDef *) DE_UIS1_BASE)     /*!< DE_UIS1 UI Scaler(UIS) provides RGB format image resizing function register set access pointer */
#define DE_FCE4 ((DE_FCE_TypeDef *) DE_FCE4_BASE)     /*!< DE_FCE4 Fresh and Contrast Enhancement (FCE) register set access pointer */
#define DE_BLS4 ((DE_BLS_TypeDef *) DE_BLS4_BASE)     /*!< DE_BLS4 Blue Level Stretch (BLS) register set access pointer */
#define DE_FCC4 ((DE_FCC_TypeDef *) DE_FCC4_BASE)     /*!< DE_FCC4 Fancy color curvature (FCC) register set access pointer */
#define DE_UI2 ((DE_UI_TypeDef *) DE_UI2_BASE)        /*!< DE_UI2 Display Engine (DE) - UI surface register set access pointer */
#define DE_UIS2 ((DE_UIS_TypeDef *) DE_UIS2_BASE)     /*!< DE_UIS2 UI Scaler(UIS) provides RGB format image resizing function register set access pointer */
#define DE_FCE5 ((DE_FCE_TypeDef *) DE_FCE5_BASE)     /*!< DE_FCE5 Fresh and Contrast Enhancement (FCE) register set access pointer */
#define DE_BLS5 ((DE_BLS_TypeDef *) DE_BLS5_BASE)     /*!< DE_BLS5 Blue Level Stretch (BLS) register set access pointer */
#define DE_FCC5 ((DE_FCC_TypeDef *) DE_FCC5_BASE)     /*!< DE_FCC5 Fancy color curvature (FCC) register set access pointer */
#define DE_UI3 ((DE_UI_TypeDef *) DE_UI3_BASE)        /*!< DE_UI3 Display Engine (DE) - UI surface register set access pointer */
#define DE_UIS3 ((DE_UIS_TypeDef *) DE_UIS3_BASE)     /*!< DE_UIS3 UI Scaler(UIS) provides RGB format image resizing function register set access pointer */
#define DE_FCE6 ((DE_FCE_TypeDef *) DE_FCE6_BASE)     /*!< DE_FCE6 Fresh and Contrast Enhancement (FCE) register set access pointer */
#define DE_BLS6 ((DE_BLS_TypeDef *) DE_BLS6_BASE)     /*!< DE_BLS6 Blue Level Stretch (BLS) register set access pointer */
#define DE_FCC6 ((DE_FCC_TypeDef *) DE_FCC6_BASE)     /*!< DE_FCC6 Fancy color curvature (FCC) register set access pointer */
#define DE_MIXER0_BLD ((DE_BLD_TypeDef *) DE_MIXER0_BLD_BASE)/*!< DE_MIXER0_BLD Display Engine (DE) - Blender register set access pointer */
#define DE_MIXER1_BLD ((DE_BLD_TypeDef *) DE_MIXER1_BLD_BASE)/*!< DE_MIXER1_BLD Display Engine (DE) - Blender register set access pointer */
#define G2D ((G2D_TOP_TypeDef *) G2D_BASE)            /*!< G2D Graphic 2D top register set access pointer */
#define G2D_TOP ((G2D_TOP_TypeDef *) G2D_TOP_BASE)    /*!< G2D_TOP Graphic 2D top register set access pointer */
#define G2D_MIXER ((G2D_MIXER_TypeDef *) G2D_MIXER_BASE)/*!< G2D_MIXER Graphic 2D (G2D) Engine Video Mixer register set access pointer */
#define G2D_BLD ((G2D_BLD_TypeDef *) G2D_BLD_BASE)    /*!< G2D_BLD Graphic 2D (G2D) Engine Blender register set access pointer */
#define G2D_V0 ((G2D_VI_TypeDef *) G2D_V0_BASE)       /*!< G2D_V0 Graphic 2D VI surface register set access pointer */
#define G2D_UI0 ((G2D_UI_TypeDef *) G2D_UI0_BASE)     /*!< G2D_UI0 Graphic 2D UI surface register set access pointer */
#define G2D_UI1 ((G2D_UI_TypeDef *) G2D_UI1_BASE)     /*!< G2D_UI1 Graphic 2D UI surface register set access pointer */
#define G2D_UI2 ((G2D_UI_TypeDef *) G2D_UI2_BASE)     /*!< G2D_UI2 Graphic 2D UI surface register set access pointer */
#define G2D_WB ((G2D_WB_TypeDef *) G2D_WB_BASE)       /*!< G2D_WB Graphic 2D (G2D) Engine Write Back register set access pointer */
#define G2D_VSU ((G2D_VSU_TypeDef *) G2D_VSU_BASE)    /*!< G2D_VSU Graphic 2D Video Scaler register set access pointer */
#define DISPLAY0_TOP ((DISPLAY0_TOP_TypeDef *) DISPLAY0_TOP_BASE)/*!< DISPLAY0_TOP VIDE0_OUT1_SYS register set access pointer */
#define TCON_LCD0 ((TCON_LCD_TypeDef *) TCON_LCD0_BASE)/*!< TCON_LCD0 Timing Controller_LCD (TCON_LCD) register set access pointer */
#define TCON_LCD1 ((TCON_LCD_TypeDef *) TCON_LCD1_BASE)/*!< TCON_LCD1 Timing Controller_LCD (TCON_LCD) register set access pointer */
#define DISPLAY1_TOP ((DISPLAY1_TOP_TypeDef *) DISPLAY1_TOP_BASE)/*!< DISPLAY1_TOP VIDEO_OUT1_SYS register set access pointer */
#define HDMI_TX0 ((HDMI_TX_TypeDef *) HDMI_TX0_BASE)  /*!< HDMI_TX0  register set access pointer */
#define TCON_TV0 ((TCON_TV_TypeDef *) TCON_TV0_BASE)  /*!< TCON_TV0 TV Output register set access pointer */
#define TCON_TV1 ((TCON_TV_TypeDef *) TCON_TV1_BASE)  /*!< TCON_TV1 TV Output register set access pointer */
#define STBY_PRCM ((PRCM_TypeDef *) STBY_PRCM_BASE)   /*!< STBY_PRCM  register set access pointer */
#define S_GPIOL ((S_GPIO_TypeDef *) S_GPIOL_BASE)     /*!< S_GPIOL Secure Port Controller register set access pointer */
#define S_GPIOM ((S_GPIO_TypeDef *) S_GPIOM_BASE)     /*!< S_GPIOM Secure Port Controller register set access pointer */
#define S_UART0 ((UART_TypeDef *) S_UART0_BASE)       /*!< S_UART0  register set access pointer */
#define S_UART1 ((UART_TypeDef *) S_UART1_BASE)       /*!< S_UART1  register set access pointer */
#define S_TWI0 ((TWI_TypeDef *) S_TWI0_BASE)          /*!< S_TWI0 Two Wire Interface (TWI) register set access pointer */
#define S_TWI1 ((TWI_TypeDef *) S_TWI1_BASE)          /*!< S_TWI1 Two Wire Interface (TWI) register set access pointer */
#define S_TWI2 ((TWI_TypeDef *) S_TWI2_BASE)          /*!< S_TWI2 Two Wire Interface (TWI) register set access pointer */
#define RTC ((RTC_TypeDef *) RTC_BASE)                /*!< RTC Real Time Clock (RTC) register set access pointer */
#define S_SPI ((SPI_TypeDef *) S_SPI_BASE)            /*!< S_SPI Serial Peripheral Interface register set access pointer */
#define CPU_SUBSYS_CTRL ((CPU_SUBSYS_CTRL_TypeDef *) CPU_SUBSYS_CTRL_BASE)/*!< CPU_SUBSYS_CTRL  register set access pointer */
#define TIMESTAMP_STA ((TIMESTAMP_STA_TypeDef *) TIMESTAMP_STA_BASE)/*!< TIMESTAMP_STA  register set access pointer */
#define TIMESTAMP_CTRL ((TIMESTAMP_CTRL_TypeDef *) TIMESTAMP_CTRL_BASE)/*!< TIMESTAMP_CTRL  register set access pointer */
#define CLUSTER_CFG ((CLUSTER_CFG_TypeDef *) CLUSTER_CFG_BASE)/*!< CLUSTER_CFG  register set access pointer */
#define CPU_PLL_CFG ((CPU_PLL_CFG_TypeDef *) CPU_PLL_CFG_BASE)/*!< CPU_PLL_CFG  register set access pointer */

#ifdef __cplusplus
 }
#endif /* __cplusplus */

#endif /* HEADER__2A4CD94A_A755_4786_BB96_E52021B2959C__INCLUDED */
