/* Copyright (c) 2024 by Genadi V. Zawidowski. All rights reserved. */
#pragma once
#ifndef HEADER__E88EF34D_BB95_4cfd_9438_7C6E40DDDD90__INCLUDED
#define HEADER__E88EF34D_BB95_4cfd_9438_7C6E40DDDD90__INCLUDED
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* IRQs */

typedef enum IRQn
{
    SGI0_IRQn = 0,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI1_IRQn = 1,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI2_IRQn = 2,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI3_IRQn = 3,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI4_IRQn = 4,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI5_IRQn = 5,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI6_IRQn = 6,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI7_IRQn = 7,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI8_IRQn = 8,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI9_IRQn = 9,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI10_IRQn = 10,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI11_IRQn = 11,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI12_IRQn = 12,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI13_IRQn = 13,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI14_IRQn = 14,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI15_IRQn = 15,                                  /*!< GIC_INTERFACE GIC CPU IF */
    VirtualMaintenanceInterrupt_IRQn = 25,            /*!< GIC_INTERFACE GIC CPU IF */
    HypervisorTimer_IRQn = 26,                        /*!< GIC_INTERFACE GIC CPU IF */
    VirtualTimer_IRQn = 27,                           /*!< GIC_INTERFACE GIC CPU IF */
    Legacy_nFIQ_IRQn = 28,                            /*!< GIC_INTERFACE GIC CPU IF */
    SecurePhysicalTimer_IRQn = 29,                    /*!< GIC_INTERFACE GIC CPU IF */
    NonSecurePhysicalTimer_IRQn = 30,                 /*!< GIC_INTERFACE GIC CPU IF */
    Legacy_nIRQ_IRQn = 31,                            /*!< GIC_INTERFACE GIC CPU IF */
    CPUX_MSGBOX_R_IRQn = 32,                          /*!< MSGBOX Message Box */
    UART0_IRQn = 34,                                  /*!< UART  */
    UART1_IRQn = 35,                                  /*!< UART  */
    UART2_IRQn = 36,                                  /*!< UART  */
    UART3_IRQn = 37,                                  /*!< UART  */
    UART4_IRQn = 38,                                  /*!< UART  */
    UART5_IRQn = 39,                                  /*!< UART  */
    TWI0_IRQn = 41,                                   /*!< TWI  */
    TWI1_IRQn = 42,                                   /*!< TWI  */
    TWI2_IRQn = 43,                                   /*!< TWI  */
    TWI3_IRQn = 44,                                   /*!< TWI  */
    SPI0_IRQn = 47,                                   /*!< SPI Serial Peripheral Interface */
    SPI1_IRQn = 48,                                   /*!< SPI Serial Peripheral Interface */
    LEDC_IRQn = 52,                                   /*!< LEDC LED Lamp Controller */
    CAN0_IRQn = 53,                                   /*!< CAN CAN (see Allwinner_T3_User_Manual_V1.0_cleaned.pdf as part of documentation) */
    CAN1_IRQn = 54,                                   /*!< CAN CAN (see Allwinner_T3_User_Manual_V1.0_cleaned.pdf as part of documentation) */
    OWA_IRQn = 55,                                    /*!< OWA One Wire Audio (TX only) */
    DMIC_IRQn = 56,                                   /*!< DMIC  */
    AUDIO_CODEC_IRQn = 57,                            /*!< AUDIO_CODEC Audio Codec */
    I2S_PCM1_IRQn = 59,                               /*!< I2S_PCM  */
    I2S_PCM2_IRQn = 60,                               /*!< I2S_PCM  */
    USB0_DEVICE_IRQn = 61,                            /*!< USBOTG  */
    USB0_EHCI_IRQn = 62,                              /*!< USB_EHCI_Capability  */
    USB0_OHCI_IRQn = 63,                              /*!< USB_OHCI_Capability  */
    USB1_EHCI_IRQn = 65,                              /*!< USB_EHCI_Capability  */
    USB1_OHCI_IRQn = 66,                              /*!< USB_OHCI_Capability  */
    SMHC0_IRQn = 72,                                  /*!< SMHC SD-MMC Host Controller */
    SMHC1_IRQn = 73,                                  /*!< SMHC SD-MMC Host Controller */
    SMHC2_IRQn = 74,                                  /*!< SMHC SD-MMC Host Controller */
    MCTL_IRQn = 75,                                   /*!< MCTL_COM  */
    SMC_IRQn = 76,                                    /*!< SMC Secure Memory Control (SMC) - Sets secure area of DRAM */
    EMAC_IRQn = 78,                                   /*!< EMAC  */
    TZMA_ERR_IRQn = 79,                               /*!< TZMA SRAM_A1 TZMA IRQ */
    CCU_FERR_IRQn = 80,                               /*!< CCU Clock Controller Unit (CCU) */
    AHB_HREADY_TIME_OUT_IRQn = 81,                    /*!< CCU SYS_CTRL ahb_hready time out */
    DMAC_NS_IRQn = 82,                                /*!< DMAC  */
    DMAC_S_IRQn = 83,                                 /*!< DMAC  */
    CE_NS_IRQn = 84,                                  /*!< CE Crypto Engine (CE) */
    CE_S_IRQn = 85,                                   /*!< CE Crypto Engine (CE) */
    HSTIMER0_IRQn = 87,                               /*!< HSTIMER High Speed Timer (HSTimer) */
    HSTIMER1_IRQn = 88,                               /*!< HSTIMER High Speed Timer (HSTimer) */
    GPADC_IRQn = 89,                                  /*!< GPADC  */
    THS_IRQn = 90,                                    /*!< THS Thermal Sensor */
    TIMER0_IRQn = 91,                                 /*!< TIMER  */
    TIMER1_IRQn = 92,                                 /*!< TIMER  */
    TPADC_IRQn = 94,                                  /*!< TPADC  */
    WATCHDOG_IRQn = 95,                               /*!< TIMER  */
    IOMMU_IRQn = 96,                                  /*!< IOMMU  */
    VE_IRQn = 98,                                     /*!< VE Video Encoding */
    GPIOB_NS_IRQn = 101,                              /*!< GPIOINT  */
    GPIOB_S_IRQn = 102,                               /*!< GPIOINT  */
    GPIOC_NS_IRQn = 103,                              /*!< GPIOINT  */
    GPIOC_S_IRQn = 104,                               /*!< GPIOINT  */
    GPIOD_NS_IRQn = 105,                              /*!< GPIOINT  */
    GPIOD_S_IRQn = 106,                               /*!< GPIOINT  */
    GPIOE_NS_IRQn = 107,                              /*!< GPIOINT  */
    GPIOE_S_IRQn = 108,                               /*!< GPIOINT  */
    GPIOF_NS_IRQn = 109,                              /*!< GPIOINT  */
    GPIOF_S_IRQn = 110,                               /*!< GPIOINT  */
    GPIOG_NS_IRQn = 111,                              /*!< GPIOINT  */
    DE_IRQn = 119,                                    /*!< DE_TOP Display Engine (DE) TOP */
    DI_IRQn = 120,                                    /*!< DI De-interlacer (DI) */
    G2D_IRQn = 121,                                   /*!< G2D_TOP Graphic 2D top */
    TCON_LCD0_IRQn = 122,                             /*!< TCON_LCD Timing Controller_LCD (TCON_LCD) */
    TCON_TV0_IRQn = 123,                              /*!< TCON_TV  */
    DSI0_IRQn = 124,                                  /*!< DSI MIPI DSI Display Interface */
    HDMI_TX0_IRQn = 125,                              /*!< HDMI_TX  */
    TVE_IRQn = 126,                                   /*!< TVE_TOP TV encoder interrupt */
    CSIC_DMA0_IRQn = 127,                             /*!< CSIC_DMA  */
    CSIC_DMA1_IRQn = 128,                             /*!< CSIC_DMA  */
    CSIC_PARSER0_IRQn = 132,                          /*!< CSIC_PARSER  */
    CSI_TOP_PKT_IRQn = 138,                           /*!< CSIC_TOP  */
    TVD_IRQn = 139,                                   /*!< TVD_TOP Video Decoding */
    ALARM0_IRQn = 176,                                /*!< RTC Real Time Clock */
    C0_CTI0_IRQn = 192,                               /*!< C0_CPUX_CFG  */
    C0_CTI1_IRQn = 193,                               /*!< C0_CPUX_CFG  */
    C0_COMMTX0_IRQn = 196,                            /*!< C0_CPUX_CFG  */
    C0_COMMTX1_IRQn = 197,                            /*!< C0_CPUX_CFG  */
    C0_COMMRX0_IRQn = 200,                            /*!< C0_CPUX_CFG  */
    C0_COMMRX1_IRQn = 201,                            /*!< C0_CPUX_CFG  */
    C0_PMU0_IRQn = 204,                               /*!< C0_CPUX_CFG  */
    C0_PMU1_IRQn = 205,                               /*!< C0_CPUX_CFG  */
    C0_AXI_ERROR_IRQn = 208,                          /*!< C0_CPUX_CFG  */
    AXI_WR_IRQ_IRQn = 210,                            /*!< C0_CPUX_CFG  */
    AXI_RD_IRQ_IRQn = 211,                            /*!< C0_CPUX_CFG  */
    DBGPWRUPREQ_out_0_IRQn = 212,                     /*!< C0_CPUX_CFG  */
    DBGPWRUPREQ_out_1_IRQn = 213,                     /*!< C0_CPUX_CFG  */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define DSP0_CFG_BASE ((uintptr_t) 0x01700000)        /*!< DSP_CFG  Base */
#define DSP_WDG_BASE ((uintptr_t) 0x01700400)         /*!< DSP_WDG  Base */
#define DSP_INTC_BASE ((uintptr_t) 0x01700800)        /*!< DSP_INTC XTensa HiFi4 Interrupt Controller Base */
#define DSP_TZMA_BASE ((uintptr_t) 0x01700C00)        /*!< DSP_TZMA  Base */
#define DSP_MSGBOX_BASE ((uintptr_t) 0x01701000)      /*!< MSGBOX Message Box Base */
#define xDSP_MSGBOX_BASE ((uintptr_t) 0x01701000)     /*!< xDSP_MSGBOX  Base */
#define VENCODER_BASE ((uintptr_t) 0x01C0E000)        /*!< VE Video Encoding Base */
#define GPIOBLOCK_BASE ((uintptr_t) 0x02000000)       /*!< GPIOBLOCK  Base */
#define GPIOB_BASE ((uintptr_t) 0x02000030)           /*!< GPIO  Base */
#define GPIOC_BASE ((uintptr_t) 0x02000060)           /*!< GPIO  Base */
#define GPIOD_BASE ((uintptr_t) 0x02000090)           /*!< GPIO  Base */
#define GPIOE_BASE ((uintptr_t) 0x020000C0)           /*!< GPIO  Base */
#define GPIOF_BASE ((uintptr_t) 0x020000F0)           /*!< GPIO  Base */
#define GPIOG_BASE ((uintptr_t) 0x02000120)           /*!< GPIO  Base */
#define GPIOINTB_BASE ((uintptr_t) 0x02000220)        /*!< GPIOINT  Base */
#define GPIOINTC_BASE ((uintptr_t) 0x02000240)        /*!< GPIOINT  Base */
#define GPIOINTD_BASE ((uintptr_t) 0x02000260)        /*!< GPIOINT  Base */
#define GPIOINTE_BASE ((uintptr_t) 0x02000280)        /*!< GPIOINT  Base */
#define GPIOINTF_BASE ((uintptr_t) 0x020002A0)        /*!< GPIOINT  Base */
#define GPIOINTG_BASE ((uintptr_t) 0x020002C0)        /*!< GPIOINT  Base */
#define SPC_BASE ((uintptr_t) 0x02000800)             /*!< SPC Secure Peripherals Control (SPC) - Sets secure property of peripherals Base */
#define PWM_BASE ((uintptr_t) 0x02000C00)             /*!< PWM Pulse Width Modulation module Base */
#define CCU_BASE ((uintptr_t) 0x02001000)             /*!< CCU Clock Controller Unit (CCU) Base */
#define CIR_TX_BASE ((uintptr_t) 0x02003000)          /*!< CIR_TX  Base */
#define TZMA_BASE ((uintptr_t) 0x02004000)            /*!< TZMA  Base */
#define LEDC_BASE ((uintptr_t) 0x02008000)            /*!< LEDC LED Lamp Controller Base */
#define GPADC_BASE ((uintptr_t) 0x02009000)           /*!< GPADC  Base */
#define THS_BASE ((uintptr_t) 0x02009400)             /*!< THS Thermal Sensor Base */
#define TPADC_BASE ((uintptr_t) 0x02009C00)           /*!< TPADC  Base */
#define IOMMU_BASE ((uintptr_t) 0x02010000)           /*!< IOMMU  Base */
#define AUDIO_CODEC_BASE ((uintptr_t) 0x02030000)     /*!< AUDIO_CODEC Audio Codec Base */
#define DMIC_BASE ((uintptr_t) 0x02031000)            /*!< DMIC  Base */
#define I2S1_BASE ((uintptr_t) 0x02033000)            /*!< I2S_PCM  Base */
#define I2S2_BASE ((uintptr_t) 0x02034000)            /*!< I2S_PCM  Base */
#define OWA_BASE ((uintptr_t) 0x02036000)             /*!< OWA One Wire Audio (TX only) Base */
#define TIMER_BASE ((uintptr_t) 0x02050000)           /*!< TIMER  Base */
#define UART0_BASE ((uintptr_t) 0x02500000)           /*!< UART  Base */
#define UART1_BASE ((uintptr_t) 0x02500400)           /*!< UART  Base */
#define UART2_BASE ((uintptr_t) 0x02500800)           /*!< UART  Base */
#define UART3_BASE ((uintptr_t) 0x02500C00)           /*!< UART  Base */
#define UART4_BASE ((uintptr_t) 0x02501000)           /*!< UART  Base */
#define UART5_BASE ((uintptr_t) 0x02501400)           /*!< UART  Base */
#define TWI0_BASE ((uintptr_t) 0x02502000)            /*!< TWI  Base */
#define TWI1_BASE ((uintptr_t) 0x02502400)            /*!< TWI  Base */
#define TWI2_BASE ((uintptr_t) 0x02502800)            /*!< TWI  Base */
#define TWI3_BASE ((uintptr_t) 0x02502C00)            /*!< TWI  Base */
#define CAN0_BASE ((uintptr_t) 0x02504000)            /*!< CAN CAN (see Allwinner_T3_User_Manual_V1.0_cleaned.pdf as part of documentation) Base */
#define CAN1_BASE ((uintptr_t) 0x02504400)            /*!< CAN CAN (see Allwinner_T3_User_Manual_V1.0_cleaned.pdf as part of documentation) Base */
#define SYS_CFG_BASE ((uintptr_t) 0x03000000)         /*!< SYS_CFG  Base */
#define DMAC_BASE ((uintptr_t) 0x03002000)            /*!< DMAC  Base */
#define CPUX_MSGBOX_BASE ((uintptr_t) 0x03003000)     /*!< MSGBOX Message Box Base */
#define SPINLOCK_BASE ((uintptr_t) 0x03005000)        /*!< SPINLOCK Spin Lock module Base */
#define SID_BASE ((uintptr_t) 0x03006000)             /*!< SID Security ID Base */
#define SMC_BASE ((uintptr_t) 0x03007000)             /*!< SMC Secure Memory Control (SMC) - Sets secure area of DRAM Base */
#define HSTIMER_BASE ((uintptr_t) 0x03008000)         /*!< HSTIMER High Speed Timer (HSTimer) Base */
#define DCU_BASE ((uintptr_t) 0x03010000)             /*!< DCU Debug control interface? Base */
#define GIC_BASE ((uintptr_t) 0x03020000)             /*!< GIC  Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0x03021000) /*!< GIC_DISTRIBUTOR GIC DISTRIBUTOR Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0x03022000)   /*!< GIC_INTERFACE GIC CPU IF Base */
#define GICVSELF_BASE ((uintptr_t) 0x03024000)        /*!< GICV  Base */
#define GICV_BASE ((uintptr_t) 0x03025000)            /*!< GICV  Base */
#define CE_NS_BASE ((uintptr_t) 0x03040000)           /*!< CE Crypto Engine (CE) Base */
#define CE_S_BASE ((uintptr_t) 0x03040800)            /*!< CE Crypto Engine (CE) Base */
#define MCTL_COM_BASE ((uintptr_t) 0x03102000)        /*!< MCTL_COM  Base */
#define DDRPHYC_BASE ((uintptr_t) 0x03103000)         /*!< DDRPHYC  Base */
#define MCTL_PHY_BASE ((uintptr_t) 0x03103000)        /*!< MCTL_PHY  Base */
#define SMHC0_BASE ((uintptr_t) 0x04020000)           /*!< SMHC SD-MMC Host Controller Base */
#define SMHC1_BASE ((uintptr_t) 0x04021000)           /*!< SMHC SD-MMC Host Controller Base */
#define SMHC2_BASE ((uintptr_t) 0x04022000)           /*!< SMHC SD-MMC Host Controller Base */
#define SPI0_BASE ((uintptr_t) 0x04025000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI1_BASE ((uintptr_t) 0x04026000)            /*!< SPI Serial Peripheral Interface Base */
#define SPI_DBI_BASE ((uintptr_t) 0x04026000)         /*!< SPI Serial Peripheral Interface Base */
#define USBOTG0_BASE ((uintptr_t) 0x04100000)         /*!< USBOTG  Base */
#define USBPHY0_BASE ((uintptr_t) 0x04100400)         /*!< USBPHYC  Base */
#define USBEHCI0_BASE ((uintptr_t) 0x04101000)        /*!< USB_EHCI_Capability  Base */
#define USBOHCI0_BASE ((uintptr_t) 0x04101400)        /*!< USB_OHCI_Capability  Base */
#define USBEHCI1_BASE ((uintptr_t) 0x04200000)        /*!< USB_EHCI_Capability  Base */
#define USBOHCI1_BASE ((uintptr_t) 0x04200400)        /*!< USB_OHCI_Capability  Base */
#define USBPHY1_BASE ((uintptr_t) 0x04200800)         /*!< USBPHYC  Base */
#define EMAC_BASE ((uintptr_t) 0x04500000)            /*!< EMAC  Base */
#define DE_BASE ((uintptr_t) 0x05000000)              /*!< DE Display Engine (DE) Base */
#define DE_TOP_BASE ((uintptr_t) 0x05000000)          /*!< DE_TOP Display Engine (DE) TOP Base */
#define DE_MIXER0_GLB_BASE ((uintptr_t) 0x05100000)   /*!< DE_GLB Display Engine (DE) - Global Control Base */
#define DE_MIXER0_BLD_BASE ((uintptr_t) 0x05101000)   /*!< DE_BLD Display Engine (DE) - Blender Base */
#define DE_MIXER0_VI1_BASE ((uintptr_t) 0x05102000)   /*!< DE_VI Display Engine (DE) - VI surface Base */
#define DE_MIXER0_UI1_BASE ((uintptr_t) 0x05103000)   /*!< DE_UI Display Engine (DE) - UI surface Base */
#define DE_MIXER0_VEP0_BASE ((uintptr_t) 0x05120000)  /*!< DE_VEP Fresh and Contrast Enhancement (FCE), Peak, Chrominance transient improvement (CTI) LCTI, Blue Level Stretch (BLS), ancy color curvature (FCC), VEP_TOP blocks Base */
#define DE_MIXER0_VSU1_BASE ((uintptr_t) 0x05120000)  /*!< DE_VSU Video Scaler Unit (VSU), VS Base */
#define DE_MIXER0_UIS1_BASE ((uintptr_t) 0x05140000)  /*!< DE_UIS UI Scaler(UIS) provides RGB format image resizing function Base */
#define DE_MIXER0_VEP1_BASE ((uintptr_t) 0x05140000)  /*!< DE_VEP Fresh and Contrast Enhancement (FCE), Peak, Chrominance transient improvement (CTI) LCTI, Blue Level Stretch (BLS), ancy color curvature (FCC), VEP_TOP blocks Base */
#define DE_MIXER0_VSU2_BASE ((uintptr_t) 0x05140000)  /*!< DE_VSU Video Scaler Unit (VSU), VS Base */
#define DE_MIXER0_UIS2_BASE ((uintptr_t) 0x05150000)  /*!< DE_UIS UI Scaler(UIS) provides RGB format image resizing function Base */
#define DE_MIXER0_UIS3_BASE ((uintptr_t) 0x05160000)  /*!< DE_UIS UI Scaler(UIS) provides RGB format image resizing function Base */
#define DE_MIXER0_DEP_BASE ((uintptr_t) 0x051A0000)   /*!< DE_DEP DRC (dynamic range controller) Base */
#define DE_MIXER1_GLB_BASE ((uintptr_t) 0x05200000)   /*!< DE_GLB Display Engine (DE) - Global Control Base */
#define DE_MIXER1_BLD_BASE ((uintptr_t) 0x05201000)   /*!< DE_BLD Display Engine (DE) - Blender Base */
#define DE_MIXER1_VI1_BASE ((uintptr_t) 0x05202000)   /*!< DE_VI Display Engine (DE) - VI surface Base */
#define DE_MIXER1_VEP0_BASE ((uintptr_t) 0x05220000)  /*!< DE_VEP Fresh and Contrast Enhancement (FCE), Peak, Chrominance transient improvement (CTI) LCTI, Blue Level Stretch (BLS), ancy color curvature (FCC), VEP_TOP blocks Base */
#define DE_MIXER1_VSU1_BASE ((uintptr_t) 0x05220000)  /*!< DE_VSU Video Scaler Unit (VSU), VS Base */
#define DE_MIXER1_UIS1_BASE ((uintptr_t) 0x05240000)  /*!< DE_UIS UI Scaler(UIS) provides RGB format image resizing function Base */
#define DI_BASE ((uintptr_t) 0x05400000)              /*!< DI De-interlacer (DI) Base */
#define G2D_TOP_BASE ((uintptr_t) 0x05410000)         /*!< G2D_TOP Graphic 2D top Base */
#define G2D_MIXER_BASE ((uintptr_t) 0x05410100)       /*!< G2D_MIXER Graphic 2D (G2D) Engine Video Mixer Base */
#define G2D_BLD_BASE ((uintptr_t) 0x05410400)         /*!< G2D_BLD Graphic 2D (G2D) Engine Blender Base */
#define G2D_V0_BASE ((uintptr_t) 0x05410800)          /*!< G2D_VI Graphic 2D VI surface Base */
#define G2D_UI0_BASE ((uintptr_t) 0x05411000)         /*!< G2D_UI Graphic 2D UI surface Base */
#define G2D_UI1_BASE ((uintptr_t) 0x05411800)         /*!< G2D_UI Graphic 2D UI surface Base */
#define G2D_UI2_BASE ((uintptr_t) 0x05412000)         /*!< G2D_UI Graphic 2D UI surface Base */
#define G2D_WB_BASE ((uintptr_t) 0x05413000)          /*!< G2D_WB Graphic 2D (G2D) Engine Write Back Base */
#define G2D_VSU_BASE ((uintptr_t) 0x05418000)         /*!< G2D_VSU Graphic 2D Video Scaler Base */
#define G2D_ROT_BASE ((uintptr_t) 0x05438000)         /*!< G2D_ROT Graphic 2D Rotate Base */
#define DSI0_BASE ((uintptr_t) 0x05450000)            /*!< DSI MIPI DSI Display Interface Base */
#define DSI_DPHY_BASE ((uintptr_t) 0x05451000)        /*!< DSI_DPHY MIPI DSI Physical Interface Base */
#define DISPLAY_TOP_BASE ((uintptr_t) 0x05460000)     /*!< DISPLAY_TOP display interface top (DISPLAY_TOP) Base */
#define TCON_LCD0_BASE ((uintptr_t) 0x05461000)       /*!< TCON_LCD Timing Controller_LCD (TCON_LCD) Base */
#define TCON_TV0_BASE ((uintptr_t) 0x05470000)        /*!< TCON_TV  Base */
#define HDMI_TX0_BASE ((uintptr_t) 0x05500000)        /*!< HDMI_TX  Base */
#define HDMI_PHY_BASE ((uintptr_t) 0x05510000)        /*!< HDMI_PHY  Base */
#define TVE_TOP_BASE ((uintptr_t) 0x05600000)         /*!< TVE_TOP TV Output TV Encoder (display out interface = CVBS OUT) Base */
#define TVE0_BASE ((uintptr_t) 0x05604000)            /*!< TV_Encoder TV Encoder (display out interface = CVBS OUT) Base */
#define CSI_BASE ((uintptr_t) 0x05800000)             /*!< CSI  Base */
#define CSIC_CCU_BASE ((uintptr_t) 0x05800000)        /*!< CSIC_CCU  Base */
#define CSIC_TOP_BASE ((uintptr_t) 0x05800800)        /*!< CSIC_TOP  Base */
#define CSIC_PARSER0_BASE ((uintptr_t) 0x05801000)    /*!< CSIC_PARSER  Base */
#define CSIC_DMA0_BASE ((uintptr_t) 0x05809000)       /*!< CSIC_DMA  Base */
#define CSIC_DMA1_BASE ((uintptr_t) 0x05809200)       /*!< CSIC_DMA  Base */
#define TVD_TOP_BASE ((uintptr_t) 0x05C00000)         /*!< TVD_TOP Video Decoding Base */
#define TVD0_BASE ((uintptr_t) 0x05C01000)            /*!< TVD0 Video Decoding Base */
#define RISC_CFG_BASE ((uintptr_t) 0x06010000)        /*!< RISC_CFG RISC-V core configuration register Base */
#define R_CPUCFG_BASE ((uintptr_t) 0x07000400)        /*!< R_CPUCFG  Base */
#define R_CCU_BASE ((uintptr_t) 0x07010000)           /*!< R_PRCM  Base */
#define R_PRCM_BASE ((uintptr_t) 0x07010000)          /*!< R_PRCM  Base */
#define CIR_RX_BASE ((uintptr_t) 0x07040000)          /*!< CIR_RX  Base */
#define RTC_BASE ((uintptr_t) 0x07090000)             /*!< RTC Real Time Clock Base */
#define CPU_SUBSYS_CTRL_BASE ((uintptr_t) 0x08100000) /*!< CPU_SUBSYS_CTRL  Base */
#define C0_CPUX_CFG_BASE ((uintptr_t) 0x09010000)     /*!< C0_CPUX_CFG  Base */

#include <core_ca.h>

/*
 * @brief AUDIO_CODEC
 */
/*!< AUDIO_CODEC Audio Codec */
typedef struct AUDIO_CODEC_Type
{
    __IO uint32_t AC_DAC_DPC;                         /*!< Offset 0x000 DAC Digital Part Control Register */
    __IO uint32_t DAC_VOL_CTRL;                       /*!< Offset 0x004 DAC Volume Control Register */
         RESERVED(0x008[0x0010 - 0x0008], uint8_t)
    __IO uint32_t AC_DAC_FIFOC;                       /*!< Offset 0x010 DAC FIFO Control Register */
    __IO uint32_t AC_DAC_FIFOS;                       /*!< Offset 0x014 DAC FIFO Status Register */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IO uint32_t AC_DAC_TXDATA;                      /*!< Offset 0x020 DAC TX DATA Register */
    __IO uint32_t AC_DAC_CNT;                         /*!< Offset 0x024 DAC TX FIFO Counter Register */
    __IO uint32_t AC_DAC_DG;                          /*!< Offset 0x028 DAC Debug Register */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __IO uint32_t AC_ADC_FIFOC;                       /*!< Offset 0x030 ADC FIFO Control Register */
    __IO uint32_t ADC_VOL_CTRL1;                      /*!< Offset 0x034 ADC Volume Control1 Register */
    __IO uint32_t AC_ADC_FIFOS;                       /*!< Offset 0x038 ADC FIFO Status Register */
         RESERVED(0x03C[0x0040 - 0x003C], uint8_t)
    __IO uint32_t AC_ADC_RXDATA;                      /*!< Offset 0x040 ADC RX Data Register */
    __IO uint32_t AC_ADC_CNT;                         /*!< Offset 0x044 ADC RX Counter Register */
         RESERVED(0x048[0x004C - 0x0048], uint8_t)
    __IO uint32_t AC_ADC_DG;                          /*!< Offset 0x04C ADC Debug Register */
    __IO uint32_t ADC_DIG_CTRL;                       /*!< Offset 0x050 ADC Digtial Control Register */
    __IO uint32_t VRA1SPEEDUP_DOWN_CTRL;              /*!< Offset 0x054 VRA1 Speedup Down Control Register */
         RESERVED(0x058[0x00F0 - 0x0058], uint8_t)
    __IO uint32_t AC_DAC_DAP_CTRL;                    /*!< Offset 0x0F0 DAC DAP Control Register */
         RESERVED(0x0F4[0x00F8 - 0x00F4], uint8_t)
    __IO uint32_t AC_ADC_DAP_CTR;                     /*!< Offset 0x0F8 ADC DAP Control Register */
         RESERVED(0x0FC[0x0100 - 0x00FC], uint8_t)
    __IO uint32_t AC_DAC_DRC_HHPFC;                   /*!< Offset 0x100 DAC DRC High HPF Coef Register */
    __IO uint32_t AC_DAC_DRC_LHPFC;                   /*!< Offset 0x104 DAC DRC Low HPF Coef Register */
    __IO uint32_t AC_DAC_DRC_CTRL;                    /*!< Offset 0x108 DAC DRC Control Register */
    __IO uint32_t AC_DAC_DRC_LPFHAT;                  /*!< Offset 0x10C DAC DRC Left Peak Filter High Attack Time Coef Register */
    __IO uint32_t AC_DAC_DRC_LPFLAT;                  /*!< Offset 0x110 DAC DRC Left Peak Filter Low Attack Time Coef Register */
    __IO uint32_t AC_DAC_DRC_RPFHAT;                  /*!< Offset 0x114 DAC DRC Right Peak Filter High Attack Time Coef Register */
    __IO uint32_t AC_DAC_DRC_RPFLAT;                  /*!< Offset 0x118 DAC DRC Right Peak Filter Low Attack Time Coef Register */
    __IO uint32_t AC_DAC_DRC_LPFHRT;                  /*!< Offset 0x11C DAC DRC Left Peak Filter High Release Time Coef Register */
    __IO uint32_t AC_DAC_DRC_LPFLRT;                  /*!< Offset 0x120 DAC DRC Left Peak Filter Low Release Time Coef Register */
    __IO uint32_t AC_DAC_DRC_RPFHRT;                  /*!< Offset 0x124 DAC DRC Right Peak filter High Release Time Coef Register */
    __IO uint32_t AC_DAC_DRC_RPFLRT;                  /*!< Offset 0x128 DAC DRC Right Peak filter Low Release Time Coef Register */
    __IO uint32_t AC_DAC_DRC_LRMSHAT;                 /*!< Offset 0x12C DAC DRC Left RMS Filter High Coef Register */
    __IO uint32_t AC_DAC_DRC_LRMSLAT;                 /*!< Offset 0x130 DAC DRC Left RMS Filter Low Coef Register */
    __IO uint32_t AC_DAC_DRC_RRMSHAT;                 /*!< Offset 0x134 DAC DRC Right RMS Filter High Coef Register */
    __IO uint32_t AC_DAC_DRC_RRMSLAT;                 /*!< Offset 0x138 DAC DRC Right RMS Filter Low Coef Register */
    __IO uint32_t AC_DAC_DRC_HCT;                     /*!< Offset 0x13C DAC DRC Compressor Threshold High Setting Register */
    __IO uint32_t AC_DAC_DRC_LCT;                     /*!< Offset 0x140 DAC DRC Compressor Slope High Setting Register */
    __IO uint32_t AC_DAC_DRC_HKC;                     /*!< Offset 0x144 DAC DRC Compressor Slope High Setting Register */
    __IO uint32_t AC_DAC_DRC_LKC;                     /*!< Offset 0x148 DAC DRC Compressor Slope Low Setting Register */
    __IO uint32_t AC_DAC_DRC_HOPC;                    /*!< Offset 0x14C DAC DRC Compresso */
    __IO uint32_t AC_DAC_DRC_LOPC;                    /*!< Offset 0x150 DAC DRC Compressor Low Output at Compressor Threshold Register */
    __IO uint32_t AC_DAC_DRC_HLT;                     /*!< Offset 0x154 DAC DRC Limiter Threshold High Setting Register */
    __IO uint32_t AC_DAC_DRC_LLT;                     /*!< Offset 0x158 DAC DRC Limiter Threshold Low Setting Register */
    __IO uint32_t AC_DAC_DRC_HKl;                     /*!< Offset 0x15C DAC DRC Limiter Slope High Setting Register */
    __IO uint32_t AC_DAC_DRC_LKl;                     /*!< Offset 0x160 DAC DRC Limiter Slope Low Setting Register */
    __IO uint32_t AC_DAC_DRC_HOPL;                    /*!< Offset 0x164 DAC DRC Limiter High Output at Limiter Threshold */
    __IO uint32_t AC_DAC_DRC_LOPL;                    /*!< Offset 0x168 DAC DRC Limiter Low Output at Limiter Threshold */
    __IO uint32_t AC_DAC_DRC_HET;                     /*!< Offset 0x16C DAC DRC Expander Threshold High Setting Register */
    __IO uint32_t AC_DAC_DRC_LET;                     /*!< Offset 0x170 DAC DRC Expander Threshold Low Setting Register */
    __IO uint32_t AC_DAC_DRC_HKE;                     /*!< Offset 0x174 DAC DRC Expander Slope High Setting Register */
    __IO uint32_t AC_DAC_DRC_LKE;                     /*!< Offset 0x178 DAC DRC Expander Slope Low Setting Register */
    __IO uint32_t AC_DAC_DRC_HOPE;                    /*!< Offset 0x17C DAC DRC Expander High Output at Expander Threshold */
    __IO uint32_t AC_DAC_DRC_LOPE;                    /*!< Offset 0x180 DAC DRC Expander Low Output at Expander Threshold */
    __IO uint32_t AC_DAC_DRC_HKN;                     /*!< Offset 0x184 DAC DRC Linear Slope High Setting Register */
    __IO uint32_t AC_DAC_DRC_LKN;                     /*!< Offset 0x188 DAC DRC Linear Slope Low Setting Register */
    __IO uint32_t AC_DAC_DRC_SFHAT;                   /*!< Offset 0x18C DAC DRC Smooth filter Gain High Attack Time Coef Register */
    __IO uint32_t AC_DAC_DRC_SFLAT;                   /*!< Offset 0x190 DAC DRC Smooth filter Gain Low Attack Time Coef Register */
    __IO uint32_t AC_DAC_DRC_SFHRT;                   /*!< Offset 0x194 DAC DRC Smooth filter Gain High Release Time Coef Register */
    __IO uint32_t AC_DAC_DRC_SFLRT;                   /*!< Offset 0x198 DAC DRC Smooth filter Gain Low Release Time Coef Register */
    __IO uint32_t AC_DAC_DRC_MXGHS;                   /*!< Offset 0x19C DAC DRC MAX Gain High Setting Register */
    __IO uint32_t AC_DAC_DRC_MXGLS;                   /*!< Offset 0x1A0 DAC DRC MAX Gain Low Setting Register */
    __IO uint32_t AC_DAC_DRC_MNGHS;                   /*!< Offset 0x1A4 DAC DRC MIN Gain High Setting Register */
    __IO uint32_t AC_DAC_DRC_MNGLS;                   /*!< Offset 0x1A8 DAC DRC MIN Gain Low Setting Register */
    __IO uint32_t AC_DAC_DRC_EPSHC;                   /*!< Offset 0x1AC DAC DRC Expander Smooth Time High Coef Register */
    __IO uint32_t AC_DAC_DRC_EPSLC;                   /*!< Offset 0x1B0 DAC DRC Expander Smooth Time Low Coef Register */
         RESERVED(0x1B4[0x01B8 - 0x01B4], uint8_t)
    __IO uint32_t AC_DAC_DRC_HPFHGAIN;                /*!< Offset 0x1B8 DAC DRC HPF Gain High Coef Register */
    __IO uint32_t AC_DAC_DRC_HPFLGAIN;                /*!< Offset 0x1BC DAC DRC HPF Gain Low Coef Register */
         RESERVED(0x1C0[0x0200 - 0x01C0], uint8_t)
    __IO uint32_t AC_ADC_DRC_HHPFC;                   /*!< Offset 0x200 ADC DRC High HPF Coef Register */
    __IO uint32_t AC_ADC_DRC_LHPFC;                   /*!< Offset 0x204 ADC DRC Low HPF Coef Register */
    __IO uint32_t AC_ADC_DRC_CTRL;                    /*!< Offset 0x208 ADC DRC Control Register */
    __IO uint32_t AC_ADC_DRC_LPFHAT;                  /*!< Offset 0x20C ADC DRC Left Peak Filter High Attack Time Coef Register */
    __IO uint32_t AC_ADC_DRC_LPFLAT;                  /*!< Offset 0x210 ADC DRC Left Peak Filter Low Attack Time Coef Register */
    __IO uint32_t AC_ADC_DRC_RPFHAT;                  /*!< Offset 0x214 ADC DRC Right Peak Filter High Attack Time Coef Register */
    __IO uint32_t AC_ADC_DRC_RPFLAT;                  /*!< Offset 0x218 ADC DRC Right Peak Filter Low Attack Time Coef Register */
    __IO uint32_t AC_ADC_DRC_LPFHRT;                  /*!< Offset 0x21C ADC DRC Left Peak Filter High Release Time Coef Register */
    __IO uint32_t AC_ADC_DRC_LPFLRT;                  /*!< Offset 0x220 ADC DRC Left Peak Filter Low Release Time Coef Register */
    __IO uint32_t AC_ADC_DRC_RPFHRT;                  /*!< Offset 0x224 ADC DRC Right Peak Filter High Release Time Coef Register */
    __IO uint32_t AC_ADC_DRC_RPFLRT;                  /*!< Offset 0x228 ADC DRC Right Peak Filter Low Release Time Coef Register */
    __IO uint32_t AC_ADC_DRC_LRMSHAT;                 /*!< Offset 0x22C ADC DRC Left RMS Filter High Coef Register */
    __IO uint32_t AC_ADC_DRC_LRMSLAT;                 /*!< Offset 0x230 ADC DRC Left RMS Filter Low Coef Register */
    __IO uint32_t AC_ADC_DRC_RRMSHAT;                 /*!< Offset 0x234 ADC DRC Right RMS Filter High Coef Register */
    __IO uint32_t AC_ADC_DRC_RRMSLAT;                 /*!< Offset 0x238 ADC DRC Right RMS Filter Low Coef Register */
    __IO uint32_t AC_ADC_DRC_HCT;                     /*!< Offset 0x23C ADC DRC Compressor Threshold High Setting Register */
    __IO uint32_t AC_ADC_DRC_LCT;                     /*!< Offset 0x240 ADC DRC Compressor Slope High Setting Register */
    __IO uint32_t AC_ADC_DRC_HKC;                     /*!< Offset 0x244 ADC DRC Compressor Slope High Setting Register */
    __IO uint32_t AC_ADC_DRC_LKC;                     /*!< Offset 0x248 ADC DRC Compressor Slope Low Setting Register */
    __IO uint32_t AC_ADC_DRC_HOPC;                    /*!< Offset 0x24C ADC DRC Compressor High Output at Compressor Threshold Register */
    __IO uint32_t AC_ADC_DRC_LOPC;                    /*!< Offset 0x250 ADC DRC Compressor Low Output at Compressor Threshold Register */
    __IO uint32_t AC_ADC_DRC_HLT;                     /*!< Offset 0x254 ADC DRC Limiter Threshold High Setting Register */
    __IO uint32_t AC_ADC_DRC_LLT;                     /*!< Offset 0x258 ADC DRC Limiter Threshold Low Setting Register */
    __IO uint32_t AC_ADC_DRC_HKl;                     /*!< Offset 0x25C ADC DRC Limiter Slope High Setting Register */
    __IO uint32_t AC_ADC_DRC_LKl;                     /*!< Offset 0x260 ADC DRC Limiter Slope Low Setting Register */
    __IO uint32_t AC_ADC_DRC_HOPL;                    /*!< Offset 0x264 ADC DRC Limiter High Output at Limiter Threshold */
    __IO uint32_t AC_ADC_DRC_LOPL;                    /*!< Offset 0x268 ADC DRC Limiter Low Output at Limiter Threshold */
    __IO uint32_t AC_ADC_DRC_HET;                     /*!< Offset 0x26C ADC DRC Expander Threshold High Setting Register */
    __IO uint32_t AC_ADC_DRC_LET;                     /*!< Offset 0x270 ADC DRC Expander Threshold Low Setting Register */
    __IO uint32_t AC_ADC_DRC_HKE;                     /*!< Offset 0x274 ADC DRC Expander Slope High Setting Register */
    __IO uint32_t AC_ADC_DRC_LKE;                     /*!< Offset 0x278 ADC DRC Expander Slope Low Setting Register */
    __IO uint32_t AC_ADC_DRC_HOPE;                    /*!< Offset 0x27C ADC DRC Expander High Output at Expander Threshold */
    __IO uint32_t AC_ADC_DRC_LOPE;                    /*!< Offset 0x280 ADC DRC Expander Low Output at Expander Threshold */
    __IO uint32_t AC_ADC_DRC_HKN;                     /*!< Offset 0x284 ADC DRC Linear Slope High Setting Register */
    __IO uint32_t AC_ADC_DRC_LKN;                     /*!< Offset 0x288 ADC DRC Linear Slope Low Setting Register */
    __IO uint32_t AC_ADC_DRC_SFHAT;                   /*!< Offset 0x28C ADC DRC Smooth filter Gain High Attack Time Coef Register */
    __IO uint32_t AC_ADC_DRC_SFLAT;                   /*!< Offset 0x290 ADC DRC Smooth filter Gain Low Attack Time Coef Register */
    __IO uint32_t AC_ADC_DRC_SFHRT;                   /*!< Offset 0x294 ADC DRC Smooth filter Gain High Release Time Coef Register */
    __IO uint32_t AC_ADC_DRC_SFLRT;                   /*!< Offset 0x298 ADC DRC Smooth filter Gain Low Release Time Coef Register */
    __IO uint32_t AC_ADC_DRC_MXGHS;                   /*!< Offset 0x29C ADC DRC MAX Gain High Setting Register */
    __IO uint32_t AC_ADC_DRC_MXGLS;                   /*!< Offset 0x2A0 ADC DRC MAX Gain Low Setting Register */
    __IO uint32_t AC_ADC_DRC_MNGHS;                   /*!< Offset 0x2A4 ADC DRC MIN Gain High Setting Register */
    __IO uint32_t AC_ADC_DRC_MNGLS;                   /*!< Offset 0x2A8 ADC DRC MIN Gain Low Setting Register */
    __IO uint32_t AC_ADC_DRC_EPSHC;                   /*!< Offset 0x2AC ADC DRC Expander Smooth Time High Coef Register */
    __IO uint32_t AC_ADC_DRC_EPSLC;                   /*!< Offset 0x2B0 ADC DRC Expander Smooth Time Low Coef Register */
         RESERVED(0x2B4[0x02B8 - 0x02B4], uint8_t)
    __IO uint32_t AC_ADC_DRC_HPFHGAIN;                /*!< Offset 0x2B8 ADC DRC HPF Gain High Coef Register */
    __IO uint32_t AC_ADC_DRC_HPFLGAIN;                /*!< Offset 0x2BC ADC DRC HPF Gain Low Coef Register */
         RESERVED(0x2C0[0x0300 - 0x02C0], uint8_t)
    __IO uint32_t ADC1_REG;                           /*!< Offset 0x300 ADC1 Analog Control Register */
    __IO uint32_t ADC2_REG;                           /*!< Offset 0x304 ADC2 Analog Control Register */
    __IO uint32_t ADC3_REG;                           /*!< Offset 0x308 ADC3 Analog Control Register */
         RESERVED(0x30C[0x0310 - 0x030C], uint8_t)
    __IO uint32_t DAC_REG;                            /*!< Offset 0x310 DAC Analog Control Register */
         RESERVED(0x314[0x0318 - 0x0314], uint8_t)
    __IO uint32_t MICBIAS_REG;                        /*!< Offset 0x318 MICBIAS Analog Control Register */
    __IO uint32_t RAMP_REG;                           /*!< Offset 0x31C BIAS Analog Control Register */
    __IO uint32_t BIAS_REG;                           /*!< Offset 0x320 BIAS Analog Control Register */
         RESERVED(0x324[0x0328 - 0x0324], uint8_t)
    __IO uint32_t HMIC_CTRL;                          /*!< Offset 0x328 * HMIC Control Register */
    __IO uint32_t HMIC_STS;                           /*!< Offset 0x32C * HMIC Status Register ( */
    __IO uint32_t ADC5_REG;                           /*!< Offset 0x330 ADC5 Analog Control Register */
         RESERVED(0x334[0x0340 - 0x0334], uint8_t)
    __IO uint32_t HP2_REG;                            /*!< Offset 0x340 * Headphone2 Analog Control Register */
         RESERVED(0x344[0x0348 - 0x0344], uint8_t)
    __IO uint32_t POWER_REG;                          /*!< Offset 0x348 * POWER Analog Control Register */
    __IO uint32_t ADC_CUR_REG;                        /*!< Offset 0x34C * ADC Current Analog Control Register */
} AUDIO_CODEC_TypeDef; /* size of structure = 0x350 */
/*
 * @brief C0_CPUX_CFG
 */
/*!< C0_CPUX_CFG  */
typedef struct C0_CPUX_CFG_Type
{
    __IO uint32_t C0_RST_CTRL;                        /*!< Offset 0x000 Cluster 0 Reset Control Register */
         RESERVED(0x004[0x0010 - 0x0004], uint8_t)
    __IO uint32_t C0_CTRL_REG0;                       /*!< Offset 0x010 Cluster 0 Control Register0 */
    __IO uint32_t C0_CTRL_REG1;                       /*!< Offset 0x014 Cluster 0 Control Register1 */
    __IO uint32_t C0_CTRL_REG2;                       /*!< Offset 0x018 Cluster 0 Control Register2 */
         RESERVED(0x01C[0x0024 - 0x001C], uint8_t)
    __IO uint32_t CACHE_CFG_REG;                      /*!< Offset 0x024 Cache Configuration Register */
         RESERVED(0x028[0x0080 - 0x0028], uint8_t)
    __IO uint32_t C0_CPU_STATUS;                      /*!< Offset 0x080 Cluster 0 CPU Status Register */
    __IO uint32_t L2_STATUS_REG;                      /*!< Offset 0x084 Cluster 0 L2 Status Register */
         RESERVED(0x088[0x00C0 - 0x0088], uint8_t)
    __IO uint32_t DBG_REG0;                           /*!< Offset 0x0C0 Cluster 0 Debug Control Register0 */
    __IO uint32_t DBG_REG1;                           /*!< Offset 0x0C4 Cluster 0 Debug Control Register1 */
         RESERVED(0x0C8[0x00D0 - 0x00C8], uint8_t)
    __IO uint32_t AXI_MNT_CTRL_REG;                   /*!< Offset 0x0D0 AXI Monitor Control Register */
    __IO uint32_t AXI_MNT_PRD_REG;                    /*!< Offset 0x0D4 AXI Monitor Period Register */
    __IO uint32_t AXI_MNT_RLTCY_REG;                  /*!< Offset 0x0D8 AXI Monitor Read Total Latency Register */
    __IO uint32_t AXI_MNT_WLTCY_REG;                  /*!< Offset 0x0DC AXI Monitor Write Total Latency Register */
    __IO uint32_t AXI_MNT_RREQ_REG;                   /*!< Offset 0x0E0 AXI Monitor Read Request Times Register */
    __IO uint32_t AXI_MNT_WREQ_REG;                   /*!< Offset 0x0E4 AXI Monitor Write Request Times Register */
    __IO uint32_t AXI_MNT_RBD_REG;                    /*!< Offset 0x0E8 AXI Monitor Read Bandwidth Register */
    __IO uint32_t AXI_MNT_WBD_REG;                    /*!< Offset 0x0EC AXI Monitor Write Bandwidth Register */
} C0_CPUX_CFG_TypeDef; /* size of structure = 0x0F0 */
/*
 * @brief CAN
 */
/*!< CAN CAN (see Allwinner_T3_User_Manual_V1.0_cleaned.pdf as part of documentation) */
typedef struct CAN_Type
{
    __IO uint32_t CAN_MSEL;                           /*!< Offset 0x000 CAN mode select register */
    __IO uint32_t CAN_CMD;                            /*!< Offset 0x004 CAN command register */
    __IO uint32_t CAN_STA;                            /*!< Offset 0x008 CAN status register */
    __IO uint32_t CAN_INT;                            /*!< Offset 0x00C CAN interrupt register */
    __IO uint32_t CAN_INTEN;                          /*!< Offset 0x010 CAN interrupt enable register */
    __IO uint32_t CAN_BUSTIME;                        /*!< Offset 0x014 CAN bus timing register */
    __IO uint32_t CAN_TEWL;                           /*!< Offset 0x018 CAN TX error warning limit register */
    __IO uint32_t CAN_ERRC;                           /*!< Offset 0x01C CAN error counter register */
    __IO uint32_t CAN_RMCNT;                          /*!< Offset 0x020 CAN receive message counter register */
    __IO uint32_t CAN_RBUF_SADDR;                     /*!< Offset 0x024 CAN receive buffer start address register */
    __IO uint32_t CAN_ACPC;                           /*!< Offset 0x028 CAN acceptance code 0 register(reset mode) */
    __IO uint32_t CAN_ACPM;                           /*!< Offset 0x02C CAN acceptance mask 0 register(reset mode) */
         RESERVED(0x030[0x0040 - 0x0030], uint8_t)
    __IO uint32_t CAN_TRBUF [0x00D];                  /*!< Offset 0x040 CAN TX/RX message buffer N (n=0..12) register */
         RESERVED(0x074[0x0180 - 0x0074], uint8_t)
    __IO uint32_t CAN_RBUF_RBACK [0x030];             /*!< Offset 0x180 CAN transmit buffer for read back register (0x0180 ~0x1b0) */
         RESERVED(0x240[0x0300 - 0x0240], uint8_t)
    __IO uint32_t CAN_VERSION;                        /*!< Offset 0x300 CAN Version Register */
         RESERVED(0x304[0x0400 - 0x0304], uint8_t)
} CAN_TypeDef; /* size of structure = 0x400 */
/*
 * @brief CCU
 */
/*!< CCU Clock Controller Unit (CCU) */
typedef struct CCU_Type
{
    __IO uint32_t PLL_CPU_CTRL_REG;                   /*!< Offset 0x000 PLL_CPU Control Register */
         RESERVED(0x004[0x0010 - 0x0004], uint8_t)
    __IO uint32_t PLL_DDR_CTRL_REG;                   /*!< Offset 0x010 PLL_DDR Control Register */
         RESERVED(0x014[0x0020 - 0x0014], uint8_t)
    __IO uint32_t PLL_PERI_CTRL_REG;                  /*!< Offset 0x020 PLL_PERI Control Register */
         RESERVED(0x024[0x0040 - 0x0024], uint8_t)
    __IO uint32_t PLL_VIDEO0_CTRL_REG;                /*!< Offset 0x040 PLL_VIDEO0 Control Register */
         RESERVED(0x044[0x0048 - 0x0044], uint8_t)
    __IO uint32_t PLL_VIDEO1_CTRL_REG;                /*!< Offset 0x048 PLL_VIDEO1 Control Register */
         RESERVED(0x04C[0x0058 - 0x004C], uint8_t)
    __IO uint32_t PLL_VE_CTRL_REG;                    /*!< Offset 0x058 PLL_VE Control Register */
         RESERVED(0x05C[0x0078 - 0x005C], uint8_t)
    __IO uint32_t PLL_AUDIO0_CTRL_REG;                /*!< Offset 0x078 PLL_AUDIO0 Control Register */
         RESERVED(0x07C[0x0080 - 0x007C], uint8_t)
    __IO uint32_t PLL_AUDIO1_CTRL_REG;                /*!< Offset 0x080 PLL_AUDIO1 Control Register */
         RESERVED(0x084[0x0110 - 0x0084], uint8_t)
    __IO uint32_t PLL_DDR_PAT0_CTRL_REG;              /*!< Offset 0x110 PLL_DDR Pattern0 Control Register */
    __IO uint32_t PLL_DDR_PAT1_CTRL_REG;              /*!< Offset 0x114 PLL_DDR Pattern1 Control Register */
         RESERVED(0x118[0x0120 - 0x0118], uint8_t)
    __IO uint32_t PLL_PERI_PAT0_CTRL_REG;             /*!< Offset 0x120 PLL_PERI Pattern0 Control Register */
    __IO uint32_t PLL_PERI_PAT1_CTRL_REG;             /*!< Offset 0x124 PLL_PERI Pattern1 Control Register */
         RESERVED(0x128[0x0140 - 0x0128], uint8_t)
    __IO uint32_t PLL_VIDEO0_PAT0_CTRL_REG;           /*!< Offset 0x140 PLL_VIDEO0 Pattern0 Control Register */
    __IO uint32_t PLL_VIDEO0_PAT1_CTRL_REG;           /*!< Offset 0x144 PLL_VIDEO0 Pattern1 Control Register */
    __IO uint32_t PLL_VIDEO1_PAT0_CTRL_REG;           /*!< Offset 0x148 PLL_VIDEO1 Pattern0 Control Register */
    __IO uint32_t PLL_VIDEO1_PAT1_CTRL_REG;           /*!< Offset 0x14C PLL_VIDEO1 Pattern1 Control Register */
         RESERVED(0x150[0x0158 - 0x0150], uint8_t)
    __IO uint32_t PLL_VE_PAT0_CTRL_REG;               /*!< Offset 0x158 PLL_VE Pattern0 Control Register */
    __IO uint32_t PLL_VE_PAT1_CTRL_REG;               /*!< Offset 0x15C PLL_VE Pattern1 Control Register */
         RESERVED(0x160[0x0178 - 0x0160], uint8_t)
    __IO uint32_t PLL_AUDIO0_PAT0_CTRL_REG;           /*!< Offset 0x178 PLL_AUDIO0 Pattern0 Control Register */
    __IO uint32_t PLL_AUDIO0_PAT1_CTRL_REG;           /*!< Offset 0x17C PLL_AUDIO0 Pattern1 Control Register */
    __IO uint32_t PLL_AUDIO1_PAT0_CTRL_REG;           /*!< Offset 0x180 PLL_AUDIO1 Pattern0 Control Register */
    __IO uint32_t PLL_AUDIO1_PAT1_CTRL_REG;           /*!< Offset 0x184 PLL_AUDIO1 Pattern1 Control Register */
         RESERVED(0x188[0x0300 - 0x0188], uint8_t)
    __IO uint32_t PLL_CPU_BIAS_REG;                   /*!< Offset 0x300 PLL_CPU Bias Register */
         RESERVED(0x304[0x0310 - 0x0304], uint8_t)
    __IO uint32_t PLL_DDR_BIAS_REG;                   /*!< Offset 0x310 PLL_DDR Bias Register */
         RESERVED(0x314[0x0320 - 0x0314], uint8_t)
    __IO uint32_t PLL_PERI_BIAS_REG;                  /*!< Offset 0x320 PLL_PERI Bias Register */
         RESERVED(0x324[0x0340 - 0x0324], uint8_t)
    __IO uint32_t PLL_VIDEO0_BIAS_REG;                /*!< Offset 0x340 PLL_VIDEO0 Bias Register */
         RESERVED(0x344[0x0348 - 0x0344], uint8_t)
    __IO uint32_t PLL_VIDEO1_BIAS_REG;                /*!< Offset 0x348 PLL_VIDEO1 Bias Register */
         RESERVED(0x34C[0x0358 - 0x034C], uint8_t)
    __IO uint32_t PLL_VE_BIAS_REG;                    /*!< Offset 0x358 PLL_VE Bias Register */
         RESERVED(0x35C[0x0378 - 0x035C], uint8_t)
    __IO uint32_t PLL_AUDIO0_BIAS_REG;                /*!< Offset 0x378 PLL_AUDIO0 Bias Register */
         RESERVED(0x37C[0x0380 - 0x037C], uint8_t)
    __IO uint32_t PLL_AUDIO1_BIAS_REG;                /*!< Offset 0x380 PLL_AUDIO1 Bias Register */
         RESERVED(0x384[0x0400 - 0x0384], uint8_t)
    __IO uint32_t PLL_CPU_TUN_REG;                    /*!< Offset 0x400 PLL_CPU Tuning Register */
         RESERVED(0x404[0x0500 - 0x0404], uint8_t)
    __IO uint32_t CPU_AXI_CFG_REG;                    /*!< Offset 0x500 CPU_AXI Configuration Register */
    __IO uint32_t CPU_GATING_REG;                     /*!< Offset 0x504 CPU_GATING Configuration Register */
         RESERVED(0x508[0x0510 - 0x0508], uint8_t)
    __IO uint32_t PSI_CLK_REG;                        /*!< Offset 0x510 PSI Clock Register */
         RESERVED(0x514[0x0520 - 0x0514], uint8_t)
    __IO uint32_t APB0_CLK_REG;                       /*!< Offset 0x520 APB0 Clock Register */
    __IO uint32_t APB1_CLK_REG;                       /*!< Offset 0x524 APB1 Clock Register */
         RESERVED(0x528[0x0540 - 0x0528], uint8_t)
    __IO uint32_t MBUS_CLK_REG;                       /*!< Offset 0x540 MBUS Clock Register */
         RESERVED(0x544[0x0600 - 0x0544], uint8_t)
    __IO uint32_t DE_CLK_REG;                         /*!< Offset 0x600 DE Clock Register */
         RESERVED(0x604[0x060C - 0x0604], uint8_t)
    __IO uint32_t DE_BGR_REG;                         /*!< Offset 0x60C DE Bus Gating Reset Register */
         RESERVED(0x610[0x0620 - 0x0610], uint8_t)
    __IO uint32_t DI_CLK_REG;                         /*!< Offset 0x620 DI Clock Register */
         RESERVED(0x624[0x062C - 0x0624], uint8_t)
    __IO uint32_t DI_BGR_REG;                         /*!< Offset 0x62C DI Bus Gating Reset Register */
    __IO uint32_t G2D_CLK_REG;                        /*!< Offset 0x630 G2D Clock Register */
         RESERVED(0x634[0x063C - 0x0634], uint8_t)
    __IO uint32_t G2D_BGR_REG;                        /*!< Offset 0x63C G2D Bus Gating Reset Register */
         RESERVED(0x640[0x0680 - 0x0640], uint8_t)
    __IO uint32_t CE_CLK_REG;                         /*!< Offset 0x680 CE Clock Register */
         RESERVED(0x684[0x068C - 0x0684], uint8_t)
    __IO uint32_t CE_BGR_REG;                         /*!< Offset 0x68C CE Bus Gating Reset Register */
    __IO uint32_t VE_CLK_REG;                         /*!< Offset 0x690 VE Clock Register */
         RESERVED(0x694[0x069C - 0x0694], uint8_t)
    __IO uint32_t VE_BGR_REG;                         /*!< Offset 0x69C VE Bus Gating Reset Register */
         RESERVED(0x6A0[0x070C - 0x06A0], uint8_t)
    __IO uint32_t DMA_BGR_REG;                        /*!< Offset 0x70C DMA Bus Gating Reset Register */
         RESERVED(0x710[0x071C - 0x0710], uint8_t)
    __IO uint32_t MSGBOX_BGR_REG;                     /*!< Offset 0x71C MSGBOX Bus Gating Reset Register */
         RESERVED(0x720[0x072C - 0x0720], uint8_t)
    __IO uint32_t SPINLOCK_BGR_REG;                   /*!< Offset 0x72C SPINLOCK Bus Gating Reset Register */
         RESERVED(0x730[0x073C - 0x0730], uint8_t)
    __IO uint32_t HSTIMER_BGR_REG;                    /*!< Offset 0x73C HSTIMER Bus Gating Reset Register */
    __IO uint32_t AVS_CLK_REG;                        /*!< Offset 0x740 AVS Clock Register */
         RESERVED(0x744[0x078C - 0x0744], uint8_t)
    __IO uint32_t DBGSYS_BGR_REG;                     /*!< Offset 0x78C DBGSYS Bus Gating Reset Register */
         RESERVED(0x790[0x07AC - 0x0790], uint8_t)
    __IO uint32_t PWM_BGR_REG;                        /*!< Offset 0x7AC PWM Bus Gating Reset Register */
         RESERVED(0x7B0[0x07BC - 0x07B0], uint8_t)
    __IO uint32_t IOMMU_BGR_REG;                      /*!< Offset 0x7BC IOMMU Bus Gating Reset Register */
         RESERVED(0x7C0[0x0800 - 0x07C0], uint8_t)
    __IO uint32_t DRAM_CLK_REG;                       /*!< Offset 0x800 DRAM Clock Register */
    __IO uint32_t MBUS_MAT_CLK_GATING_REG;            /*!< Offset 0x804 MBUS Master Clock Gating Register */
         RESERVED(0x808[0x080C - 0x0808], uint8_t)
    __IO uint32_t DRAM_BGR_REG;                       /*!< Offset 0x80C DRAM Bus Gating Reset Register */
         RESERVED(0x810[0x0830 - 0x0810], uint8_t)
    __IO uint32_t SMHC0_CLK_REG;                      /*!< Offset 0x830 SMHC0 Clock Register */
    __IO uint32_t SMHC1_CLK_REG;                      /*!< Offset 0x834 SMHC1 Clock Register */
    __IO uint32_t SMHC2_CLK_REG;                      /*!< Offset 0x838 SMHC2 Clock Register */
         RESERVED(0x83C[0x084C - 0x083C], uint8_t)
    __IO uint32_t SMHC_BGR_REG;                       /*!< Offset 0x84C SMHC Bus Gating Reset Register */
         RESERVED(0x850[0x090C - 0x0850], uint8_t)
    __IO uint32_t UART_BGR_REG;                       /*!< Offset 0x90C UART Bus Gating Reset Register */
         RESERVED(0x910[0x091C - 0x0910], uint8_t)
    __IO uint32_t TWI_BGR_REG;                        /*!< Offset 0x91C TWI Bus Gating Reset Register */
         RESERVED(0x920[0x092C - 0x0920], uint8_t)
    __IO uint32_t CAN_BGR_REG;                        /*!< Offset 0x92C CAN Bus Gating Reset Register */
         RESERVED(0x930[0x0940 - 0x0930], uint8_t)
    __IO uint32_t SPI0_CLK_REG;                       /*!< Offset 0x940 SPI0 Clock Register */
    __IO uint32_t SPI1_CLK_REG;                       /*!< Offset 0x944 SPI1 Clock Register */
         RESERVED(0x948[0x096C - 0x0948], uint8_t)
    __IO uint32_t SPI_BGR_REG;                        /*!< Offset 0x96C SPI Bus Gating Reset Register */
    __IO uint32_t EMAC_25M_CLK_REG;                   /*!< Offset 0x970 EMAC_25M Clock Register */
         RESERVED(0x974[0x097C - 0x0974], uint8_t)
    __IO uint32_t EMAC_BGR_REG;                       /*!< Offset 0x97C EMAC Bus Gating Reset Register */
         RESERVED(0x980[0x09C0 - 0x0980], uint8_t)
    __IO uint32_t IRTX_CLK_REG;                       /*!< Offset 0x9C0 IRTX Clock Register */
         RESERVED(0x9C4[0x09CC - 0x09C4], uint8_t)
    __IO uint32_t IRTX_BGR_REG;                       /*!< Offset 0x9CC IRTX Bus Gating Reset Register */
         RESERVED(0x9D0[0x09EC - 0x09D0], uint8_t)
    __IO uint32_t GPADC_BGR_REG;                      /*!< Offset 0x9EC GPADC Bus Gating Reset Register */
         RESERVED(0x9F0[0x09FC - 0x09F0], uint8_t)
    __IO uint32_t THS_BGR_REG;                        /*!< Offset 0x9FC THS Bus Gating Reset Register */
         RESERVED(0xA00[0x0A14 - 0x0A00], uint8_t)
    __IO uint32_t I2S1_CLK_REG;                       /*!< Offset 0xA14 I2S1 Clock Register */
    __IO uint32_t I2S2_CLK_REG;                       /*!< Offset 0xA18 I2S2 Clock Register */
    __IO uint32_t I2S2_ASRC_CLK_REG;                  /*!< Offset 0xA1C I2S2_ASRC Clock Register */
    __IO uint32_t I2S_BGR_REG;                        /*!< Offset 0xA20 I2S Bus Gating Reset Register */
    __IO uint32_t OWA_TX_CLK_REG;                     /*!< Offset 0xA24 OWA_TX Clock Register */
         RESERVED(0xA28[0x0A2C - 0x0A28], uint8_t)
    __IO uint32_t OWA_BGR_REG;                        /*!< Offset 0xA2C OWA Bus Gating Reset Register */
         RESERVED(0xA30[0x0A40 - 0x0A30], uint8_t)
    __IO uint32_t DMIC_CLK_REG;                       /*!< Offset 0xA40 DMIC Clock Register */
         RESERVED(0xA44[0x0A4C - 0x0A44], uint8_t)
    __IO uint32_t DMIC_BGR_REG;                       /*!< Offset 0xA4C DMIC Bus Gating Reset Register */
    __IO uint32_t AUDIO_CODEC_DAC_CLK_REG;            /*!< Offset 0xA50 AUDIO_CODEC_DAC Clock Register */
    __IO uint32_t AUDIO_CODEC_ADC_CLK_REG;            /*!< Offset 0xA54 AUDIO_CODEC_ADC Clock Register */
         RESERVED(0xA58[0x0A5C - 0x0A58], uint8_t)
    __IO uint32_t AUDIO_CODEC_BGR_REG;                /*!< Offset 0xA5C AUDIO_CODEC Bus Gating Reset Register */
         RESERVED(0xA60[0x0A70 - 0x0A60], uint8_t)
    __IO uint32_t USB0_CLK_REG;                       /*!< Offset 0xA70 USB0 Clock Register */
    __IO uint32_t USB1_CLK_REG;                       /*!< Offset 0xA74 USB1 Clock Register */
         RESERVED(0xA78[0x0A8C - 0x0A78], uint8_t)
    __IO uint32_t USB_BGR_REG;                        /*!< Offset 0xA8C USB Bus Gating Reset Register */
         RESERVED(0xA90[0x0ABC - 0x0A90], uint8_t)
    __IO uint32_t DPSS_TOP_BGR_REG;                   /*!< Offset 0xABC DPSS_TOP Bus Gating Reset Register */
         RESERVED(0xAC0[0x0B24 - 0x0AC0], uint8_t)
    __IO uint32_t DSI_CLK_REG;                        /*!< Offset 0xB24 DSI Clock Register */
         RESERVED(0xB28[0x0B4C - 0x0B28], uint8_t)
    __IO uint32_t DSI_BGR_REG;                        /*!< Offset 0xB4C DSI Bus Gating Reset Register */
         RESERVED(0xB50[0x0B60 - 0x0B50], uint8_t)
    __IO uint32_t TCONLCD_CLK_REG;                    /*!< Offset 0xB60 TCONLCD Clock Register */
         RESERVED(0xB64[0x0B7C - 0x0B64], uint8_t)
    __IO uint32_t TCONLCD_BGR_REG;                    /*!< Offset 0xB7C TCONLCD Bus Gating Reset Register */
    __IO uint32_t TCONTV_CLK_REG;                     /*!< Offset 0xB80 TCONTV Clock Register */
         RESERVED(0xB84[0x0B9C - 0x0B84], uint8_t)
    __IO uint32_t TCONTV_BGR_REG;                     /*!< Offset 0xB9C TCONTV Bus Gating Reset Register */
         RESERVED(0xBA0[0x0BAC - 0x0BA0], uint8_t)
    __IO uint32_t LVDS_BGR_REG;                       /*!< Offset 0xBAC LVDS Bus Gating Reset Register */
    __IO uint32_t TVE_CLK_REG;                        /*!< Offset 0xBB0 TVE Clock Register */
         RESERVED(0xBB4[0x0BBC - 0x0BB4], uint8_t)
    __IO uint32_t TVE_BGR_REG;                        /*!< Offset 0xBBC TVE Bus Gating Reset Register */
    __IO uint32_t TVD_CLK_REG;                        /*!< Offset 0xBC0 TVD Clock Register */
         RESERVED(0xBC4[0x0BDC - 0x0BC4], uint8_t)
    __IO uint32_t TVD_BGR_REG;                        /*!< Offset 0xBDC TVD Bus Gating Reset Register */
         RESERVED(0xBE0[0x0BF0 - 0x0BE0], uint8_t)
    __IO uint32_t LEDC_CLK_REG;                       /*!< Offset 0xBF0 LEDC Clock Register */
         RESERVED(0xBF4[0x0BFC - 0x0BF4], uint8_t)
    __IO uint32_t LEDC_BGR_REG;                       /*!< Offset 0xBFC LEDC Bus Gating Reset Register */
         RESERVED(0xC00[0x0C04 - 0x0C00], uint8_t)
    __IO uint32_t CSI_CLK_REG;                        /*!< Offset 0xC04 CSI Clock Register */
    __IO uint32_t CSI_MASTER_CLK_REG;                 /*!< Offset 0xC08 CSI Master Clock Register */
         RESERVED(0xC0C[0x0C1C - 0x0C0C], uint8_t)
    __IO uint32_t CSI_BGR_REG;                        /*!< Offset 0xC1C CSI Bus Gating Reset Register */
         RESERVED(0xC20[0x0C50 - 0x0C20], uint8_t)
    __IO uint32_t TPADC_CLK_REG;                      /*!< Offset 0xC50 TPADC Clock Register */
         RESERVED(0xC54[0x0C5C - 0x0C54], uint8_t)
    __IO uint32_t TPADC_BGR_REG;                      /*!< Offset 0xC5C TPADC Bus Gating Reset Register */
         RESERVED(0xC60[0x0C70 - 0x0C60], uint8_t)
    __IO uint32_t DSP_CLK_REG;                        /*!< Offset 0xC70 DSP Clock Register */
         RESERVED(0xC74[0x0C7C - 0x0C74], uint8_t)
    __IO uint32_t DSP_BGR_REG;                        /*!< Offset 0xC7C DSP Bus Gating Reset Register */
         RESERVED(0xC80[0x0D00 - 0x0C80], uint8_t)
    __IO uint32_t RISC_CLK_REG;                       /*!< Offset 0xD00 RISC Clock Register */
    __IO uint32_t RISC_GATING_REG;                    /*!< Offset 0xD04 RISC Gating Configuration Register */
         RESERVED(0xD08[0x0D0C - 0x0D08], uint8_t)
    __IO uint32_t RISC_CFG_BGR_REG;                   /*!< Offset 0xD0C RISC_CFG Bus Gating Reset Register */
         RESERVED(0xD10[0x0F04 - 0x0D10], uint8_t)
    __IO uint32_t PLL_LOCK_DBG_CTRL_REG;              /*!< Offset 0xF04 PLL Lock Debug Control Register */
    __IO uint32_t FRE_DET_CTRL_REG;                   /*!< Offset 0xF08 Frequency Detect Control Register */
    __IO uint32_t FRE_UP_LIM_REG;                     /*!< Offset 0xF0C Frequency Up Limit Register */
    __IO uint32_t FRE_DOWN_LIM_REG;                   /*!< Offset 0xF10 Frequency Down Limit Register */
         RESERVED(0xF14[0x0F20 - 0x0F14], uint8_t)
    __IO uint32_t RISC_RST_REG;                       /*!< Offset 0xF20 RISC LOCK RESET Register */
         RESERVED(0xF24[0x0F30 - 0x0F24], uint8_t)
    __IO uint32_t CCU_FAN_GATE_REG;                   /*!< Offset 0xF30 CCU FANOUT CLOCK GATE Register */
    __IO uint32_t CLK27M_FAN_REG;                     /*!< Offset 0xF34 CLK27M FANOUT Register */
    __IO uint32_t PCLK_FAN_REG;                       /*!< Offset 0xF38 PCLK FANOUT Register */
    __IO uint32_t CCU_FAN_REG;                        /*!< Offset 0xF3C CCU FANOUT Register */
         RESERVED(0xF40[0x1000 - 0x0F40], uint8_t)
} CCU_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief CE
 */
/*!< CE Crypto Engine (CE) */
typedef struct CE_Type
{
    __IO uint32_t CE_TDA;                             /*!< Offset 0x000 Task Descriptor Address */
         RESERVED(0x004[0x0008 - 0x0004], uint8_t)
    __IO uint32_t CE_ICR;                             /*!< Offset 0x008 Interrupt Control Register */
    __IO uint32_t CE_ISR;                             /*!< Offset 0x00C Interrupt Status Register */
    __IO uint32_t CE_TLR;                             /*!< Offset 0x010 Task Load Register */
    __IO uint32_t CE_TSR;                             /*!< Offset 0x014 Task Status Register */
    __IO uint32_t CE_ESR;                             /*!< Offset 0x018 Error Status Register */
         RESERVED(0x01C[0x0024 - 0x001C], uint8_t)
    __IO uint32_t CE_CSA;                             /*!< Offset 0x024 DMA Current Source Address */
    __IO uint32_t CE_CDA;                             /*!< Offset 0x028 DMA Current Destination Address */
    __IO uint32_t CE_TPR;                             /*!< Offset 0x02C Throughput Register */
} CE_TypeDef; /* size of structure = 0x030 */
/*
 * @brief CIR_RX
 */
/*!< CIR_RX  */
typedef struct CIR_RX_Type
{
    __IO uint32_t CIR_CTL;                            /*!< Offset 0x000 CIR Control Register */
         RESERVED(0x004[0x0010 - 0x0004], uint8_t)
    __IO uint32_t CIR_RXPCFG;                         /*!< Offset 0x010 CIR Receiver Pulse Configure Register */
         RESERVED(0x014[0x0020 - 0x0014], uint8_t)
    __IO uint32_t CIR_RXFIFO;                         /*!< Offset 0x020 CIR Receiver FIFO Register */
         RESERVED(0x024[0x002C - 0x0024], uint8_t)
    __IO uint32_t CIR_RXINT;                          /*!< Offset 0x02C CIR Receiver Interrupt Control Register */
    __IO uint32_t CIR_RXSTA;                          /*!< Offset 0x030 CIR Receiver Status Register */
    __IO uint32_t CIR_RXCFG;                          /*!< Offset 0x034 CIR Receiver Configure Register */
} CIR_RX_TypeDef; /* size of structure = 0x038 */
/*
 * @brief CIR_TX
 */
/*!< CIR_TX  */
typedef struct CIR_TX_Type
{
    __IO uint32_t CIR_TGLR;                           /*!< Offset 0x000 CIR Transmit Global Register */
    __IO uint32_t CIR_TMCR;                           /*!< Offset 0x004 CIR Transmit Modulation Control Register */
    __IO uint32_t CIR_TCR;                            /*!< Offset 0x008 CIR Transmit Control Register */
    __IO uint32_t CIR_IDC_H;                          /*!< Offset 0x00C CIR Transmit Idle Duration Threshold High Bit Register */
    __IO uint32_t CIR_IDC_L;                          /*!< Offset 0x010 CIR Transmit Idle Duration Threshold Low Bit Register */
    __IO uint32_t CIR_TICR_H;                         /*!< Offset 0x014 CIR Transmit Idle Counter High Bit Register */
    __IO uint32_t CIR_TICR_L;                         /*!< Offset 0x018 CIR Transmit Idle Counter Low Bit Register */
         RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    __IO uint32_t CIR_TEL;                            /*!< Offset 0x020 CIR TX FIFO Empty Level Register */
    __IO uint32_t CIR_TXINT;                          /*!< Offset 0x024 CIR Transmit Interrupt Control Register */
    __IO uint32_t CIR_TAC;                            /*!< Offset 0x028 CIR Transmit FIFO Available Counter Register */
    __IO uint32_t CIR_TXSTA;                          /*!< Offset 0x02C CIR Transmit Status Register */
    __IO uint32_t CIR_TXT;                            /*!< Offset 0x030 CIR Transmit Threshold Register */
    __IO uint32_t CIR_DMA;                            /*!< Offset 0x034 CIR DMA Control Register */
         RESERVED(0x038[0x0080 - 0x0038], uint8_t)
    __IO uint32_t CIR_TXFIFO;                         /*!< Offset 0x080 CIR Transmit FIFO Data Register */
} CIR_TX_TypeDef; /* size of structure = 0x084 */
/*
 * @brief CPU_SUBSYS_CTRL
 */
/*!< CPU_SUBSYS_CTRL  */
typedef struct CPU_SUBSYS_CTRL_Type
{
    __IO uint32_t GENER_CTRL_REG0;                    /*!< Offset 0x000 General Control Register0 */
         RESERVED(0x004[0x000C - 0x0004], uint8_t)
    __IO uint32_t GIC_JTAG_RST_CTRL;                  /*!< Offset 0x00C GIC and JTAG Reset Control Register */
    __IO uint32_t C0_INT_EN;                          /*!< Offset 0x010 Cluster0 Interrupt Enable Control Register */
    __IO uint32_t RQ_FIQ_STATUS;                      /*!< Offset 0x014 IRQ/FIQ Status Register */
    __IO uint32_t GENER_CTRL_REG2;                    /*!< Offset 0x018 General Control Register2 */
    __IO uint32_t DBG_STATE;                          /*!< Offset 0x01C Debug State Register */
} CPU_SUBSYS_CTRL_TypeDef; /* size of structure = 0x020 */
/*
 * @brief CSIC_CCU
 */
/*!< CSIC_CCU  */
typedef struct CSIC_CCU_Type
{
    __IO uint32_t CCU_CLK_MODE_REG;                   /*!< Offset 0x000 CCU Clock Mode Register */
    __IO uint32_t CCU_PARSER_CLK_EN_REG;              /*!< Offset 0x004 CCU Parser Clock Enable Register */
         RESERVED(0x008[0x000C - 0x0008], uint8_t)
    __IO uint32_t CCU_POST0_CLK_EN_REG;               /*!< Offset 0x00C CCU Post0 Clock Enable Register */
} CSIC_CCU_TypeDef; /* size of structure = 0x010 */
/*
 * @brief CSIC_DMA
 */
/*!< CSIC_DMA  */
typedef struct CSIC_DMA_Type
{
    __IO uint32_t CSIC_DMA_EN_REG;                    /*!< Offset 0x000 CSIC DMA Enable Register */
    __IO uint32_t CSIC_DMA_CFG_REG;                   /*!< Offset 0x004 CSIC DMA Configuration Register */
         RESERVED(0x008[0x0010 - 0x0008], uint8_t)
    __IO uint32_t CSIC_DMA_HSIZE_REG;                 /*!< Offset 0x010 CSIC DMA Horizontal Size Register */
    __IO uint32_t CSIC_DMA_VSIZE_REG;                 /*!< Offset 0x014 CSIC DMA Vertical Size Register */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IO uint32_t CSIC_DMA_F0_BUFA_REG;               /*!< Offset 0x020 CSIC DMA FIFO 0 Output Buffer-A Address Register */
    __IO uint32_t CSIC_DMA_F0_BUFA_RESULT_REG;        /*!< Offset 0x024 CSIC DMA FIFO 0 Output Buffer-A Address Result Register */
    __IO uint32_t CSIC_DMA_F1_BUFA_REG;               /*!< Offset 0x028 CSIC DMA FIFO 1 Output Buffer-A Address Register */
    __IO uint32_t CSIC_DMA_F1_BUFA_RESULT_REG;        /*!< Offset 0x02C CSIC DMA FIFO 1 Output Buffer-A Address Result Register */
    __IO uint32_t CSIC_DMA_F2_BUFA_REG;               /*!< Offset 0x030 CSIC DMA FIFO 2 Output Buffer-A Address Register */
    __IO uint32_t CSIC_DMA_F2_BUFA_RESULT_REG;        /*!< Offset 0x034 CSIC DMA FIFO 2 Output Buffer-A Address Result Register */
    __IO uint32_t CSIC_DMA_BUF_LEN_REG;               /*!< Offset 0x038 CSIC DMA Buffer Length Register */
    __IO uint32_t CSIC_DMA_FLIP_SIZE_REG;             /*!< Offset 0x03C CSIC DMA Flip Size Register */
    __IO uint32_t CSIC_DMA_VI_TO_TH0_REG;             /*!< Offset 0x040 CSIC DMA Video Input Timeout Threshold0 Register */
    __IO uint32_t CSIC_DMA_VI_TO_TH1_REG;             /*!< Offset 0x044 CSIC DMA Video Input Timeout Threshold1 Register */
    __IO uint32_t CSIC_DMA_VI_TO_CNT_VAL_REG;         /*!< Offset 0x048 CSIC DMA Video Input Timeout Counter Value Register */
    __IO uint32_t CSIC_DMA_CAP_STA_REG;               /*!< Offset 0x04C CSIC DMA Capture Status Register */
    __IO uint32_t CSIC_DMA_INT_EN_REG;                /*!< Offset 0x050 CSIC DMA Interrupt Enable Register */
    __IO uint32_t CSIC_DMA_INT_STA_REG;               /*!< Offset 0x054 CSIC DMA Interrupt Status Register */
    __IO uint32_t CSIC_DMA_LINE_CNT_REG;              /*!< Offset 0x058 CSIC DMA LINE Counter Register */
    __IO uint32_t CSIC_DMA_FRM_CNT_REG;               /*!< Offset 0x05C CSIC DMA Frame Counter Register */
    __IO uint32_t CSIC_DMA_FRM_CLK_CNT_REG;           /*!< Offset 0x060 CSIC DMA Frame Clock Counter Register */
    __IO uint32_t CSIC_DMA_ACC_ITNL_CLK_CNT_REG;      /*!< Offset 0x064 CSIC DMA Accumulated And Internal Clock Counter Register */
    __IO uint32_t CSIC_DMA_FIFO_STAT_REG;             /*!< Offset 0x068 CSIC DMA FIFO Statistic Register */
    __IO uint32_t CSIC_DMA_FIFO_THRS_REG;             /*!< Offset 0x06C CSIC DMA FIFO Threshold Register */
    __IO uint32_t CSIC_DMA_PCLK_STAT_REG;             /*!< Offset 0x070 CSIC DMA PCLK Statistic Register */
         RESERVED(0x074[0x0080 - 0x0074], uint8_t)
    __IO uint32_t CSIC_DMA_BUF_ADDR_FIFO0_ENTRY_REG;  /*!< Offset 0x080 CSIC DMA BUF Address FIFO0 Entry Register */
    __IO uint32_t CSIC_DMA_BUF_ADDR_FIFO1_ENTRY_REG;  /*!< Offset 0x084 CSIC DMA BUF Address FIFO1 Entry Register */
    __IO uint32_t CSIC_DMA_BUF_ADDR_FIFO2_ENTRY_REG;  /*!< Offset 0x088 CSIC DMA BUF Address FIFO2 Entry Register */
    __IO uint32_t CSIC_DMA_BUF_TH_REG;                /*!< Offset 0x08C CSIC DMA BUF Threshold Register */
    __IO uint32_t CSIC_DMA_BUF_ADDR_FIFO_CON_REG;     /*!< Offset 0x090 CSIC DMA BUF Address FIFO Content Register */
    __IO uint32_t CSIC_DMA_STORED_FRM_CNT_REG;        /*!< Offset 0x094 CSIC DMA Stored Frame Counter Register */
         RESERVED(0x098[0x01F4 - 0x0098], uint8_t)
    __IO uint32_t CSIC_FEATURE_REG;                   /*!< Offset 0x1F4 CSIC DMA Feature List Register */
} CSIC_DMA_TypeDef; /* size of structure = 0x1F8 */
/*
 * @brief CSIC_PARSER
 */
/*!< CSIC_PARSER  */
typedef struct CSIC_PARSER_Type
{
    __IO uint32_t PRS_EN_REG;                         /*!< Offset 0x000 Parser Enable Register */
    __IO uint32_t PRS_NCSIC_IF_CFG_REG;               /*!< Offset 0x004 Parser NCSIC Interface Configuration Register */
         RESERVED(0x008[0x000C - 0x0008], uint8_t)
    __IO uint32_t PRS_CAP_REG;                        /*!< Offset 0x00C Parser Capture Register */
    __IO uint32_t CSIC_PRS_SIGNAL_STA_REG;            /*!< Offset 0x010 CSIC Parser Signal Status Register */
    __IO uint32_t CSIC_PRS_NCSIC_BT656_HEAD_CFG_REG;  /*!< Offset 0x014 CSIC Parser NCSIC BT656 Header Configuration Register */
         RESERVED(0x018[0x0024 - 0x0018], uint8_t)
    __IO uint32_t PRS_C0_INFMT_REG;                   /*!< Offset 0x024 Parser Channel_0 Input Format Register */
    __IO uint32_t PRS_C0_OUTPUT_HSIZE_REG;            /*!< Offset 0x028 Parser Channel_0 Output Horizontal Size Register */
    __IO uint32_t PRS_C0_OUTPUT_VSIZE_REG;            /*!< Offset 0x02C Parser Channel_0 Output Vertical Size Register */
    __IO uint32_t PRS_C0_INPUT_PARA0_REG;             /*!< Offset 0x030 Parser Channel_0 Input Parameter0 Register */
    __IO uint32_t PRS_C0_INPUT_PARA1_REG;             /*!< Offset 0x034 Parser Channel_0 Input Parameter1 Register */
    __IO uint32_t PRS_C0_INPUT_PARA2_REG;             /*!< Offset 0x038 Parser Channel_0 Input Parameter2 Register */
    __IO uint32_t PRS_C0_INPUT_PARA3_REG;             /*!< Offset 0x03C Parser Channel_0 Input Parameter3 Register */
    __IO uint32_t PRS_C0_INT_EN_REG;                  /*!< Offset 0x040 Parser Channel_0 Interrupt Enable Register */
    __IO uint32_t PRS_C0_INT_STA_REG;                 /*!< Offset 0x044 Parser Channel_0 Interrupt Status Register */
    __IO uint32_t PRS_CH0_LINE_TIME_REG;              /*!< Offset 0x048 Parser Channel_0 Line Time Register */
         RESERVED(0x04C[0x0124 - 0x004C], uint8_t)
    __IO uint32_t PRS_C1_INFMT_REG;                   /*!< Offset 0x124 Parser Channel_1 Input Format Register */
    __IO uint32_t PRS_C1_OUTPUT_HSIZE_REG;            /*!< Offset 0x128 Parser Channel_1 Output Horizontal Size Register */
    __IO uint32_t PRS_C1_OUTPUT_VSIZE_REG;            /*!< Offset 0x12C Parser Channel_1 Output Vertical Size Register */
    __IO uint32_t PRS_C1_INPUT_PARA0_REG;             /*!< Offset 0x130 Parser Channel_1 Input Parameter0 Register */
    __IO uint32_t PRS_C1_INPUT_PARA1_REG;             /*!< Offset 0x134 Parser Channel_1 Input Parameter1 Register */
    __IO uint32_t PRS_C1_INPUT_PARA2_REG;             /*!< Offset 0x138 Parser Channel_1 Input Parameter2 Register */
    __IO uint32_t PRS_C1_INPUT_PARA3_REG;             /*!< Offset 0x13C Parser Channel_1 Input Parameter3 Register */
    __IO uint32_t PRS_C1_INT_EN_REG;                  /*!< Offset 0x140 Parser Channel_1 Interrupt Enable Register */
    __IO uint32_t PRS_C1_INT_STA_REG;                 /*!< Offset 0x144 Parser Channel_1 Interrupt Status Register */
    __IO uint32_t PRS_CH1_LINE_TIME_REG;              /*!< Offset 0x148 Parser Channel_1 Line Time Register */
         RESERVED(0x14C[0x0224 - 0x014C], uint8_t)
    __IO uint32_t PRS_C2_INFMT_REG;                   /*!< Offset 0x224 Parser Channel_2 Input Format Register */
    __IO uint32_t PRS_C2_OUTPUT_HSIZE_REG;            /*!< Offset 0x228 Parser Channel_2 Output Horizontal Size Register */
    __IO uint32_t PRS_C2_OUTPUT_VSIZE_REG;            /*!< Offset 0x22C Parser Channel_2 Output Vertical Size Register */
    __IO uint32_t PRS_C2_INPUT_PARA0_REG;             /*!< Offset 0x230 Parser Channel_2 Input Parameter0 Register */
    __IO uint32_t PRS_C2_INPUT_PARA1_REG;             /*!< Offset 0x234 Parser Channel_2 Input Parameter1 Register */
    __IO uint32_t PRS_C2_INPUT_PARA2_REG;             /*!< Offset 0x238 Parser Channel_2 Input Parameter2 Register */
    __IO uint32_t PRS_C2_INPUT_PARA3_REG;             /*!< Offset 0x23C Parser Channel_2 Input Parameter3 Register */
    __IO uint32_t PRS_C2_INT_EN_REG;                  /*!< Offset 0x240 Parser Channel_2 Interrupt Enable Register */
    __IO uint32_t PRS_C2_INT_STA_REG;                 /*!< Offset 0x244 Parser Channel_2 Interrupt Status Register */
    __IO uint32_t PRS_CH2_LINE_TIME_REG;              /*!< Offset 0x248 Parser Channel_2 Line Time Register */
         RESERVED(0x24C[0x0324 - 0x024C], uint8_t)
    __IO uint32_t PRS_C3_INFMT_REG;                   /*!< Offset 0x324 Parser Channel_3 Input Format Register */
    __IO uint32_t PRS_C3_OUTPUT_HSIZE_REG;            /*!< Offset 0x328 Parser Channel_3 Output Horizontal Size Register */
    __IO uint32_t PRS_C3_OUTPUT_VSIZE_REG;            /*!< Offset 0x32C Parser Channel_3 Output Vertical Size Register */
    __IO uint32_t PRS_C3_INPUT_PARA0_REG;             /*!< Offset 0x330 Parser Channel_3 Input Parameter0 Register */
    __IO uint32_t PRS_C3_INPUT_PARA1_REG;             /*!< Offset 0x334 Parser Channel_3 Input Parameter1 Register */
    __IO uint32_t PRS_C3_INPUT_PARA2_REG;             /*!< Offset 0x338 Parser Channel_3 Input Parameter2 Register */
    __IO uint32_t PRS_C3_INPUT_PARA3_REG;             /*!< Offset 0x33C Parser Channel_3 Input Parameter3 Register */
    __IO uint32_t PRS_C3_INT_EN_REG;                  /*!< Offset 0x340 Parser Channel_3 Interrupt Enable Register */
    __IO uint32_t PRS_C3_INT_STA_REG;                 /*!< Offset 0x344 Parser Channel_3 Interrupt Status Register */
    __IO uint32_t PRS_CH3_LINE_TIME_REG;              /*!< Offset 0x348 Parser Channel_3 Line Time Register */
         RESERVED(0x34C[0x0500 - 0x034C], uint8_t)
    __IO uint32_t CSIC_PRS_NCSIC_RX_SIGNAL0_DLY_ADJ_REG;/*!< Offset 0x500 CSIC Parser NCSIC RX Signal0 Delay Adjust Register */
         RESERVED(0x504[0x0514 - 0x0504], uint8_t)
    __IO uint32_t CSIC_PRS_NCSIC_RX_SIGNAL5_DLY_ADJ_REG;/*!< Offset 0x514 CSIC Parser NCSIC RX Signal5 Delay Adjust Register */
    __IO uint32_t CSIC_PRS_NCSIC_RX_SIGNAL6_DLY_ADJ_REG;/*!< Offset 0x518 CSIC Parser NCSIC RX Signal6 Delay Adjust Register */
} CSIC_PARSER_TypeDef; /* size of structure = 0x51C */
/*
 * @brief CSIC_TOP
 */
/*!< CSIC_TOP  */
typedef struct CSIC_TOP_Type
{
    __IO uint32_t CSIC_TOP_EN_REG;                    /*!< Offset 0x000 CSIC TOP Enable Register */
    __IO uint32_t CSIC_PTN_GEN_EN_REG;                /*!< Offset 0x004 CSIC Pattern Generation Enable Register */
    __IO uint32_t CSIC_PTN_CTRL_REG;                  /*!< Offset 0x008 CSIC Pattern Control Register */
         RESERVED(0x00C[0x0020 - 0x000C], uint8_t)
    __IO uint32_t CSIC_PTN_LEN_REG;                   /*!< Offset 0x020 CSIC Pattern Generation Length Register */
    __IO uint32_t CSIC_PTN_ADDR_REG;                  /*!< Offset 0x024 CSIC Pattern Generation Address Register */
    __IO uint32_t CSIC_PTN_ISP_SIZE_REG;              /*!< Offset 0x028 CSIC Pattern ISP Size Register */
         RESERVED(0x02C[0x00A0 - 0x002C], uint8_t)
    __IO uint32_t CSIC_DMA0_INPUT_SEL_REG;            /*!< Offset 0x0A0 CSIC DMA0 Input Select Register */
    __IO uint32_t CSIC_DMA1_INPUT_SEL_REG;            /*!< Offset 0x0A4 CSIC DMA1 Input Select Register */
         RESERVED(0x0A8[0x00DC - 0x00A8], uint8_t)
    __IO uint32_t CSIC_BIST_CS_REG;                   /*!< Offset 0x0DC CSIC BIST CS Register */
    __IO uint32_t CSIC_BIST_CONTROL_REG;              /*!< Offset 0x0E0 CSIC BIST Control Register */
    __IO uint32_t CSIC_BIST_START_REG;                /*!< Offset 0x0E4 CSIC BIST Start Register */
    __IO uint32_t CSIC_BIST_END_REG;                  /*!< Offset 0x0E8 CSIC BIST End Register */
    __IO uint32_t CSIC_BIST_DATA_MASK_REG;            /*!< Offset 0x0EC CSIC BIST Data Mask Register */
    __IO uint32_t CSIC_MBUS_REQ_MAX_REG;              /*!< Offset 0x0F0 CSIC MBUS REQ MAX Register */
         RESERVED(0x0F4[0x0100 - 0x00F4], uint8_t)
    __IO uint32_t CSIC_MULF_MOD_REG;                  /*!< Offset 0x100 CSIC Multi-Frame Mode Register */
    __IO uint32_t CSIC_MULF_INT_REG;                  /*!< Offset 0x104 CSIC Multi-Frame Interrupt Register */
} CSIC_TOP_TypeDef; /* size of structure = 0x108 */
/*
 * @brief DDRPHYC
 */
/*!< DDRPHYC  */
typedef struct DDRPHYC_Type
{
    __IO uint32_t PHYC_REG_000;                       /*!< Offset 0x000 Reg_000 */
    __IO uint32_t PHYC_REG_004;                       /*!< Offset 0x004 Reg_004 */
         RESERVED(0x008[0x000C - 0x0008], uint8_t)
    __IO uint32_t PHYC_REG_00C;                       /*!< Offset 0x00C Reg_00C */
    __IO uint32_t PHYC_REG_010;                       /*!< Offset 0x010 Reg_010 */
         RESERVED(0x014[0x0018 - 0x0014], uint8_t)
    __IO uint32_t PHYC_REG_018;                       /*!< Offset 0x018 Reg_018 */
         RESERVED(0x01C[0x002C - 0x001C], uint8_t)
    __IO uint32_t PHYC_REG_02C;                       /*!< Offset 0x02C Reg_02C */
    __IO uint32_t PHYC_REG_030;                       /*!< Offset 0x030 Reg_030 */
    __IO uint32_t PHYC_REG_034;                       /*!< Offset 0x034 Reg_034 */
    __IO uint32_t PHYC_REG_038;                       /*!< Offset 0x038 Reg_038 */
    __IO uint32_t PHYC_REG_03C;                       /*!< Offset 0x03C Reg_03C */
         RESERVED(0x040[0x0050 - 0x0040], uint8_t)
    __IO uint32_t PHYC_REG_050;                       /*!< Offset 0x050 Reg_050 */
    __IO uint32_t PHYC_REG_054;                       /*!< Offset 0x054 Reg_054 */
    __IO uint32_t PHYC_REG_058;                       /*!< Offset 0x058 Reg_058 */
    __IO uint32_t PHYC_REG_05C;                       /*!< Offset 0x05C Reg_05C */
    __IO uint32_t PHYC_REG_060;                       /*!< Offset 0x060 Reg_060 */
    __IO uint32_t PHYC_REG_064;                       /*!< Offset 0x064 Reg_064 */
    __IO uint32_t PHYC_REG_068;                       /*!< Offset 0x068 Reg_068 */
    __IO uint32_t PHYC_REG_06C;                       /*!< Offset 0x06C Reg_06C */
         RESERVED(0x070[0x0078 - 0x0070], uint8_t)
    __IO uint32_t PHYC_REG_078;                       /*!< Offset 0x078 Reg_078 */
    __IO uint32_t PHYC_REG_07C;                       /*!< Offset 0x07C Reg_07c */
    __IO uint32_t PHYC_REG_080;                       /*!< Offset 0x080 Reg_080 */
         RESERVED(0x084[0x008C - 0x0084], uint8_t)
    __IO uint32_t PHYC_REG_08C;                       /*!< Offset 0x08C Reg_08C */
    __IO uint32_t PHYC_REG_090;                       /*!< Offset 0x090 Reg_090 */
    __IO uint32_t PHYC_REG_094;                       /*!< Offset 0x094 Reg_094 */
         RESERVED(0x098[0x009C - 0x0098], uint8_t)
    __IO uint32_t PHYC_REG_09C;                       /*!< Offset 0x09C Reg_09C */
    __IO uint32_t PHYC_REG_0A0;                       /*!< Offset 0x0A0 Reg_0a0 */
         RESERVED(0x0A4[0x00B8 - 0x00A4], uint8_t)
    __IO uint32_t PHYC_REG_0B8;                       /*!< Offset 0x0B8 Reg_0B8 */
    __IO uint32_t PHYC_REG_0BC;                       /*!< Offset 0x0BC Reg_0BC */
    __IO uint32_t PHYC_REG_0C0;                       /*!< Offset 0x0C0 Reg_0C0 */
         RESERVED(0x0C4[0x0100 - 0x00C4], uint8_t)
    __IO uint32_t PHYC_REG_100;                       /*!< Offset 0x100 Reg_100 */
         RESERVED(0x104[0x0108 - 0x0104], uint8_t)
    __IO uint32_t PHYC_REG_108;                       /*!< Offset 0x108 Reg_108 */
    __IO uint32_t PHYC_REG_10C;                       /*!< Offset 0x10C Reg_10C */
    __IO uint32_t PHYC_REG_110;                       /*!< Offset 0x110 Reg_110 */
    __IO uint32_t PHYC_REG_114;                       /*!< Offset 0x114 Reg_114 */
         RESERVED(0x118[0x011C - 0x0118], uint8_t)
    __IO uint32_t PHYC_REG_11C;                       /*!< Offset 0x11C Reg_11C */
    __IO uint32_t PHYC_REG_120;                       /*!< Offset 0x120 Reg_120 */
         RESERVED(0x124[0x0140 - 0x0124], uint8_t)
    __IO uint32_t PHYC_REG_140;                       /*!< Offset 0x140 Reg_140 */
         RESERVED(0x144[0x0208 - 0x0144], uint8_t)
    __IO uint32_t PHYC_REG_208;                       /*!< Offset 0x208 Reg_208 */
         RESERVED(0x20C[0x0218 - 0x020C], uint8_t)
    __IO uint32_t PHYC_REG_218;                       /*!< Offset 0x218 Reg_218 */
    __IO uint32_t PHYC_REG_21C;                       /*!< Offset 0x21C Reg_21C */
         RESERVED(0x220[0x0228 - 0x0220], uint8_t)
    __IO uint32_t PHYC_REG_228 [0x006];               /*!< Offset 0x228 Reg_228 */
    __IO uint32_t PHYC_REG_240 [0x010];               /*!< Offset 0x240 Reg_240 */
    __IO uint32_t PHYC_REG_280;                       /*!< Offset 0x280 Reg_280 */
         RESERVED(0x284[0x0310 - 0x0284], uint8_t)
    __IO uint32_t PHYC_REG_310 [0x009];               /*!< Offset 0x310 Reg_310 Rank 0 */
    __IO uint32_t PHYC_REG_334;                       /*!< Offset 0x334 Reg_334 Rank 0 */
    __IO uint32_t PHYC_REG_338;                       /*!< Offset 0x338 Reg_338 Rank 0 */
    __IO uint32_t PHYC_REG_33C;                       /*!< Offset 0x33C Reg_33C Rank 0 */
         RESERVED(0x340[0x0344 - 0x0340], uint8_t)
    __IO uint32_t PHYC_REG_344;                       /*!< Offset 0x344 Reg_344 Rank 0 */
    __IO uint32_t PHYC_REG_348;                       /*!< Offset 0x348 Reg_348 Rank 0 */
         RESERVED(0x34C[0x0390 - 0x034C], uint8_t)
    __IO uint32_t PHYC_REG_390 [0x009];               /*!< Offset 0x390 Reg_390 */
    __IO uint32_t PHYC_REG_3B4;                       /*!< Offset 0x3B4 Reg_3B4 Rank 1 */
    __IO uint32_t PHYC_REG_3B8;                       /*!< Offset 0x3B8 Reg_3B8 Rank 1 */
    __IO uint32_t PHYC_REG_3BC;                       /*!< Offset 0x3BC Reg_3BC Rank 1 */
         RESERVED(0x3C0[0x03C4 - 0x03C0], uint8_t)
    __IO uint32_t PHYC_REG_3C4;                       /*!< Offset 0x3C4 Reg_3C4 Rank 1 */
    __IO uint32_t PHYC_REG_3C8;                       /*!< Offset 0x3C8 Reg_3C8 Rank 1 */
} DDRPHYC_TypeDef; /* size of structure = 0x3CC */
/*
 * @brief DE_BLD
 */
/*!< DE_BLD Display Engine (DE) - Blender */
typedef struct DE_BLD_Type
{
    __IO uint32_t BLD_EN_COLOR_CTL;                   /*!< Offset 0x000 BLD_FILL_COLOR_CTL Offset 0x000 BLD fill color control register */
    struct
    {
        __IO uint32_t BLD_FILL_COLOR;                 /*!< Offset 0x004 BLD fill color register */
        __IO uint32_t BLD_CH_ISIZE;                   /*!< Offset 0x008 BLD input memory size register */
        __IO uint32_t BLD_CH_OFFSET;                  /*!< Offset 0x00C BLD input memory offset register */
             RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    } CH [0x006];                                     /*!< Offset 0x004 Pipe [0..3] - but six elements */
         RESERVED(0x064[0x0080 - 0x0064], uint8_t)
    __IO uint32_t ROUTE;                              /*!< Offset 0x080 BLD_CH_RTCTL BLD routing control register (default value 0x00543210) */
    __IO uint32_t PREMULTIPLY;                        /*!< Offset 0x084 BLD pre-multiply control register */
    __IO uint32_t BKCOLOR;                            /*!< Offset 0x088  */
    __IO uint32_t OUTPUT_SIZE;                        /*!< Offset 0x08C  */
    __IO uint32_t BLD_MODE [0x006];                   /*!< Offset 0x090 BLD_CTL SUN8I_MIXER_BLEND_MODE */
         RESERVED(0x0A8[0x00B0 - 0x00A8], uint8_t)
    __IO uint32_t CK_CTL;                             /*!< Offset 0x0B0  */
    __IO uint32_t CK_CFG;                             /*!< Offset 0x0B4  */
         RESERVED(0x0B8[0x00C0 - 0x00B8], uint8_t)
    __IO uint32_t CK_MAX [0x004];                     /*!< Offset 0x0C0  */
         RESERVED(0x0D0[0x00E0 - 0x00D0], uint8_t)
    __IO uint32_t CK_MIN [0x004];                     /*!< Offset 0x0E0  */
         RESERVED(0x0F0[0x00FC - 0x00F0], uint8_t)
    __IO uint32_t OUT_CTL;                            /*!< Offset 0x0FC  */
} DE_BLD_TypeDef; /* size of structure = 0x100 */
/*
 * @brief DE_DEP
 */
/*!< DE_DEP DRC (dynamic range controller) */
typedef struct DE_DEP_Type
{
    struct
    {
        __IO uint32_t CFG;                            /*!< Offset 0x000  */
             RESERVED(0x004[0x8000 - 0x0004], uint8_t)
    } DRC [0x001];                                    /*!< Offset 0x000  */
} DE_DEP_TypeDef; /* size of structure = 0x8000 */
/*
 * @brief DE_GLB
 */
/*!< DE_GLB Display Engine (DE) - Global Control */
typedef struct DE_GLB_Type
{
    __IO uint32_t GLB_CTL;                            /*!< Offset 0x000 Global control register */
    __IO uint32_t GLB_STS;                            /*!< Offset 0x004 Global status register */
    __IO uint32_t GLB_DBUFFER;                        /*!< Offset 0x008 Global double buffer control register */
    __IO uint32_t GLB_SIZE;                           /*!< Offset 0x00C Global size register */
} DE_GLB_TypeDef; /* size of structure = 0x010 */
/*
 * @brief DE_TOP
 */
/*!< DE_TOP Display Engine (DE) TOP */
typedef struct DE_TOP_Type
{
    __IO uint32_t GATE_CFG;                           /*!< Offset 0x000 SCLK_GATE DE SCLK Gating Register */
    __IO uint32_t BUS_CFG;                            /*!< Offset 0x004 ? HCLK_GATE ? DE HCLK Gating Register */
    __IO uint32_t RST_CFG;                            /*!< Offset 0x008 AHB_RESET DE AHB Reset register */
    __IO uint32_t DIV_CFG;                            /*!< Offset 0x00C SCLK_DIV DE SCLK Division register */
    __IO uint32_t SEL_CFG;                            /*!< Offset 0x010 ? DE2TCON ? MUX register - lower bit swap DE outputs */
         RESERVED(0x014[0x0024 - 0x0014], uint8_t)
    __IO uint32_t DE_IP_CFG;                          /*!< Offset 0x024 DE IP Configure Register */
} DE_TOP_TypeDef; /* size of structure = 0x028 */
/*
 * @brief DE_UI
 */
/*!< DE_UI Display Engine (DE) - UI surface */
typedef struct DE_UI_Type
{
    struct
    {
        __IO uint32_t ATTR;                           /*!< Offset 0x000  */
        __IO uint32_t SIZE;                           /*!< Offset 0x004  */
        __IO uint32_t COORD;                          /*!< Offset 0x008  */
        __IO uint32_t PITCH;                          /*!< Offset 0x00C  */
        __IO uint32_t TOP_LADDR;                      /*!< Offset 0x010  */
        __IO uint32_t BOT_LADDR;                      /*!< Offset 0x014  */
        __IO uint32_t FCOLOR;                         /*!< Offset 0x018  */
             RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    } CFG [0x004];                                    /*!< Offset 0x000  */
    __IO uint32_t TOP_HADDR;                          /*!< Offset 0x080  */
    __IO uint32_t BOT_HADDR;                          /*!< Offset 0x084  */
    __IO uint32_t OVL_SIZE;                           /*!< Offset 0x088  */
         RESERVED(0x08C[0x0800 - 0x008C], uint8_t)
} DE_UI_TypeDef; /* size of structure = 0x800 */
/*
 * @brief DE_UIS
 */
/*!< DE_UIS UI Scaler(UIS) provides RGB format image resizing function */
typedef struct DE_UIS_Type
{
    __IO uint32_t UIS_CTRL_REG;                       /*!< Offset 0x000 Control register */
         RESERVED(0x004[0x0008 - 0x0004], uint8_t)
    __IO uint32_t UIS_STATUS_REG;                     /*!< Offset 0x008 Status register */
    __IO uint32_t UIS_FIELD_CTRL_REG;                 /*!< Offset 0x00C Field control register */
    __IO uint32_t UIS_BIST_REG;                       /*!< Offset 0x010 BIST control register */
         RESERVED(0x014[0x0040 - 0x0014], uint8_t)
    __IO uint32_t UIS_OUTSIZE_REG;                    /*!< Offset 0x040 Output size register */
         RESERVED(0x044[0x0080 - 0x0044], uint8_t)
    __IO uint32_t UIS_INSIZE_REG;                     /*!< Offset 0x080 Input size register */
         RESERVED(0x084[0x0088 - 0x0084], uint8_t)
    __IO uint32_t UIS_HSTEP_REG;                      /*!< Offset 0x088 Horizontal step register */
    __IO uint32_t UIS_VSTEP_REG;                      /*!< Offset 0x08C Vertical step register */
    __IO uint32_t UIS_HPHASE_REG;                     /*!< Offset 0x090 Horizontal initial phase register */
         RESERVED(0x094[0x0098 - 0x0094], uint8_t)
    __IO uint32_t UIS_VPHASE0_REG;                    /*!< Offset 0x098 Vertical initial phase 0 register */
    __IO uint32_t UIS_VPHASE1_REG;                    /*!< Offset 0x09C Vertical initial phase 1 register */
         RESERVED(0x0A0[0x0200 - 0x00A0], uint8_t)
    __IO uint32_t UIS_HCOEF_REGN [0x010];             /*!< Offset 0x200 Horizontal filter coefficient register N (N=0:15)#typeend */
} DE_UIS_TypeDef; /* size of structure = 0x240 */
/*
 * @brief DE_VEP
 */
/*!< DE_VEP Fresh and Contrast Enhancement (FCE), Peak, Chrominance transient improvement (CTI) LCTI, Blue Level Stretch (BLS), ancy color curvature (FCC), VEP_TOP blocks */
typedef struct DE_VEP_Type
{
    struct
    {
        __IO uint32_t CFG;                            /*!< Offset 0x000  */
             RESERVED(0x004[0x0800 - 0x0004], uint8_t)
    } FCE [0x001];                                    /*!< Offset 0x000  */
    struct
    {
        __IO uint32_t CFG;                            /*!< Offset 0x800  */
             RESERVED(0x004[0x0800 - 0x0004], uint8_t)
    } PEAK [0x001];                                   /*!< Offset 0x800  */
    struct
    {
        __IO uint32_t CFG;                            /*!< Offset 0x1000  */
             RESERVED(0x004[0x0800 - 0x0004], uint8_t)
    } LCTI [0x001];                                   /*!< Offset 0x1000  */
    struct
    {
        __IO uint32_t CFG;                            /*!< Offset 0x1800  */
             RESERVED(0x004[0x0800 - 0x0004], uint8_t)
    } BLS [0x001];                                    /*!< Offset 0x1800  */
    struct
    {
        __IO uint32_t CFG;                            /*!< Offset 0x2000  */
             RESERVED(0x004[0x0800 - 0x0004], uint8_t)
    } FCC [0x001];                                    /*!< Offset 0x2000  */
         RESERVED(0x2800[0x7000 - 0x2800], uint8_t)
    struct
    {
        __IO uint32_t CFG;                            /*!< Offset 0x7000  */
             RESERVED(0x004[0x1000 - 0x0004], uint8_t)
    } VEP_TOP [0x001];                                /*!< Offset 0x7000  */
} DE_VEP_TypeDef; /* size of structure = 0x8000 */
/*
 * @brief DE_VI
 */
/*!< DE_VI Display Engine (DE) - VI surface */
typedef struct DE_VI_Type
{
    struct
    {
        __IO uint32_t ATTR;                           /*!< Offset 0x000  */
        __IO uint32_t SIZE;                           /*!< Offset 0x004  */
        __IO uint32_t COORD;                          /*!< Offset 0x008  */
        __IO uint32_t PITCH [0x003];                  /*!< Offset 0x00C ix=0: Y, ix=1: U/UV channel, ix=3: V channel  */
        __IO uint32_t TOP_LADDR [0x003];              /*!< Offset 0x018  */
        __IO uint32_t BOT_LADDR [0x003];              /*!< Offset 0x024  */
    } CFG [0x004];                                    /*!< Offset 0x000  */
    __IO uint32_t FCOLOR [0x004];                     /*!< Offset 0x0C0  */
    __IO uint32_t TOP_HADDR [0x003];                  /*!< Offset 0x0D0  */
    __IO uint32_t BOT_HADDR [0x003];                  /*!< Offset 0x0DC  */
    __IO uint32_t OVL_SIZE [0x002];                   /*!< Offset 0x0E8 OVL_Y, OVL_UV overlay window size register */
    __IO uint32_t HORI [0x002];                       /*!< Offset 0x0F0 OVL_V horizontal down sample control register */
    __IO uint32_t VERT [0x002];                       /*!< Offset 0x0F8 OVL_V vertical down sample control register */
         RESERVED(0x100[0x0800 - 0x0100], uint8_t)
} DE_VI_TypeDef; /* size of structure = 0x800 */
/*
 * @brief DE_VSU
 */
/*!< DE_VSU Video Scaler Unit (VSU), VS */
typedef struct DE_VSU_Type
{
    __IO uint32_t VSU_CTRL_REG;                       /*!< Offset 0x000 VSU Module Control Register */
         RESERVED(0x004[0x0008 - 0x0004], uint8_t)
    __IO uint32_t VSU_STATUS_REG;                     /*!< Offset 0x008 VSU Status Register */
    __IO uint32_t VSU_FIELD_CTRL_REG;                 /*!< Offset 0x00C VSU Field Control Register */
    __IO uint32_t VSU_SCALE_MODE_REG;                 /*!< Offset 0x010 VSU Scale Mode Setting Register */
         RESERVED(0x014[0x0020 - 0x0014], uint8_t)
    __IO uint32_t VSU_DIRECTION_THR_REG;              /*!< Offset 0x020 VSU Direction Detection Threshold Register */
    __IO uint32_t VSU_EDGE_THR_REG;                   /*!< Offset 0x024 VSU Edge Detection Setting Register */
    __IO uint32_t VSU_EDSCALER_CTRL_REG;              /*!< Offset 0x028 VSU Edge-Direction Scaler Control Register */
    __IO uint32_t VSU_ANGLE_THR_REG;                  /*!< Offset 0x02C VSU Angle Reliability Setting Register */
    __IO uint32_t VSU_SHARP_EN_REG;                   /*!< Offset 0x030 VSU Sharpness Control Enable Register */
    __IO uint32_t VSU_SHARP_CORING_REG;               /*!< Offset 0x034 VSU Sharpness Control Coring Setting Register */
    __IO uint32_t VSU_SHARP_GAIN0_REG;                /*!< Offset 0x038 VSU Sharpness Control Gain Setting 0 Register */
    __IO uint32_t VSU_SHARP_GAIN1_REG;                /*!< Offset 0x03C VSU Sharpness Control Gain Setting 1 Register */
    __IO uint32_t VSU_OUT_SIZE_REG;                   /*!< Offset 0x040 VSU Output Size Register */
    __IO uint32_t VSU_GLOBAL_ALPHA_REG;               /*!< Offset 0x044 (null) */
         RESERVED(0x048[0x0080 - 0x0048], uint8_t)
    __IO uint32_t VSU_Y_SIZE_REG;                     /*!< Offset 0x080 VSU Y Channel Size Register */
         RESERVED(0x084[0x0088 - 0x0084], uint8_t)
    __IO uint32_t VSU_Y_HSTEP_REG;                    /*!< Offset 0x088 VSU Y Channel Horizontal Step Register */
    __IO uint32_t VSU_Y_VSTEP_REG;                    /*!< Offset 0x08C VSU Y Channel Vertical Step Register */
    __IO uint32_t VSU_Y_HPHASE_REG;                   /*!< Offset 0x090 VSU Y Channel Horizontal Initial Phase Register */
         RESERVED(0x094[0x0098 - 0x0094], uint8_t)
    __IO uint32_t VSU_Y_VPHASE0_REG;                  /*!< Offset 0x098 VSU Y Channel Vertical Initial Phase 0 Register */
    __IO uint32_t VSU_Y_VPHASE1_REG;                  /*!< Offset 0x09C VSU Y Channel Vertical Initial Phase 1 Register */
         RESERVED(0x0A0[0x00C0 - 0x00A0], uint8_t)
    __IO uint32_t VSU_C_SIZE_REG;                     /*!< Offset 0x0C0 VSU C Channel Size Register */
         RESERVED(0x0C4[0x00C8 - 0x00C4], uint8_t)
    __IO uint32_t VSU_C_HSTEP_REG;                    /*!< Offset 0x0C8 VSU C Channel Horizontal Step Register */
    __IO uint32_t VSU_C_VSTEP_REG;                    /*!< Offset 0x0CC VSU C Channel Vertical Step Register */
    __IO uint32_t VSU_C_HPHASE_REG;                   /*!< Offset 0x0D0 VSU C Channel Horizontal Initial Phase Register */
         RESERVED(0x0D4[0x00D8 - 0x00D4], uint8_t)
    __IO uint32_t VSU_C_VPHASE0_REG;                  /*!< Offset 0x0D8 VSU C Channel Vertical Initial Phase 0 Register */
    __IO uint32_t VSU_C_VPHASE1_REG;                  /*!< Offset 0x0DC VSU C Channel Vertical Initial Phase 1 Register */
         RESERVED(0x0E0[0x0200 - 0x00E0], uint8_t)
    __IO uint32_t VSU_Y_HCOEF0_REGN [0x020];          /*!< Offset 0x200 0x200+N*4 VSU Y Channel Horizontal Filter Coefficient0 Register N N = M 1)) */
         RESERVED(0x280[0x0300 - 0x0280], uint8_t)
    __IO uint32_t VSU_Y_HCOEF1_REGN [0x020];          /*!< Offset 0x300 0x300+N*4 VSU Y Channel Horizontal Filter Coefficient1 Register N N = M 1 */
         RESERVED(0x380[0x0400 - 0x0380], uint8_t)
    __IO uint32_t VSU_Y_VCOEF_REGN [0x020];           /*!< Offset 0x400 0x400+N*4 VSU Y Channel Vertical Filter Coefficient Register N N = M 1)) */
         RESERVED(0x480[0x0600 - 0x0480], uint8_t)
    __IO uint32_t VSU_C_HCOEF0_REGN [0x020];          /*!< Offset 0x600 0x600+N*4 VSU C Channel Horizontal Filter Coefficient0 Register N N = M 1)) */
         RESERVED(0x680[0x0700 - 0x0680], uint8_t)
    __IO uint32_t VSU_C_HCOEF1_REGN [0x020];          /*!< Offset 0x700 0x700+N*4 VSU C Channel Horizontal Filter Co efficient1 Register N N = M 1)) */
         RESERVED(0x780[0x0800 - 0x0780], uint8_t)
    __IO uint32_t VSU_C_VCOEF_REGN [0x020];           /*!< Offset 0x800 0x800+N*4 VSU C Channel Vertical Filter Coefficient Register N N = M 1)) */
} DE_VSU_TypeDef; /* size of structure = 0x880 */
/*
 * @brief DISPLAY_TOP
 */
/*!< DISPLAY_TOP display interface top (DISPLAY_TOP) */
typedef struct DISPLAY_TOP_Type
{
    __IO uint32_t TV_CLK_SRC_RGB_SRC;                 /*!< Offset 0x000 TCON_TV Clock Select and RGB Source Select Register */
         RESERVED(0x004[0x001C - 0x0004], uint8_t)
    __IO uint32_t DE_PORT_PERH_SEL;                   /*!< Offset 0x01C DE Source Select TCON R Register */
    __IO uint32_t MODULE_GATING;                      /*!< Offset 0x020 TCON Output to LVDS/DSI/TVE/HDMI Gating Register */
} DISPLAY_TOP_TypeDef; /* size of structure = 0x024 */
/*
 * @brief DMAC
 */
/*!< DMAC  */
typedef struct DMAC_Type
{
    __IO uint32_t DMAC_IRQ_EN_REG0;                   /*!< Offset 0x000 DMAC IRQ Enable Register 0 */
    __IO uint32_t DMAC_IRQ_EN_REG1;                   /*!< Offset 0x004 DMAC IRQ Enable Register 1 */
         RESERVED(0x008[0x0010 - 0x0008], uint8_t)
    __IO uint32_t DMAC_IRQ_PEND_REG0;                 /*!< Offset 0x010 DMAC IRQ Pending Register 0 */
    __IO uint32_t DMAC_IRQ_PEND_REG1;                 /*!< Offset 0x014 DMAC IRQ Pending Register 1 */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IO uint32_t DMA_SEC_REG;                        /*!< Offset 0x020 DMA Security Register */
         RESERVED(0x024[0x0028 - 0x0024], uint8_t)
    __IO uint32_t DMAC_AUTO_GATE_REG;                 /*!< Offset 0x028 DMAC Auto Gating Register */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __I  uint32_t DMAC_STA_REG;                       /*!< Offset 0x030 DMAC Status Register */
         RESERVED(0x034[0x0100 - 0x0034], uint8_t)
    struct
    {
        __IO uint32_t DMAC_EN_REGN;                   /*!< Offset 0x100 DMAC Channel Enable Register N (N = 0 to 15) 0x0100 + N*0x0040 */
        __IO uint32_t DMAC_PAU_REGN;                  /*!< Offset 0x104 DMAC Channel Pause Register N (N = 0 to 15) 0x0104 + N*0x0040 */
        __IO uint32_t DMAC_DESC_ADDR_REGN;            /*!< Offset 0x108 DMAC Channel Start Address Register N (N = 0 to 15) 0x0108 + N*0x0040 */
        __I  uint32_t DMAC_CFG_REGN;                  /*!< Offset 0x10C DMAC Channel Configuration Register N (N = 0 to 15) 0x010C + N*0x0040 */
        __I  uint32_t DMAC_CUR_SRC_REGN;              /*!< Offset 0x110 DMAC Channel Current Source Register N (N = 0 to 15) 0x0110 + N*0x0040 */
        __I  uint32_t DMAC_CUR_DEST_REGN;             /*!< Offset 0x114 DMAC Channel Current Destination Register N (N = 0 to 15) 0x0114 + N*0x0040 */
        __I  uint32_t DMAC_BCNT_LEFT_REGN;            /*!< Offset 0x118 DMAC Channel Byte Counter Left Register N (N = 0 to 15) 0x0118 + N*0x0040 */
        __I  uint32_t DMAC_PARA_REGN;                 /*!< Offset 0x11C DMAC Channel Parameter Register N (N = 0 to 15) 0x011C + N*0x0040 */
             RESERVED(0x020[0x0028 - 0x0020], uint8_t)
        __IO uint32_t DMAC_MODE_REGN;                 /*!< Offset 0x128 DMAC Mode Register N (N = 0 to 15) 0x0128 + N*0x0040 */
        __I  uint32_t DMAC_FDESC_ADDR_REGN;           /*!< Offset 0x12C DMAC Former Descriptor Address Register N (N = 0 to 15) 0x012C + N*0x0040 */
        __I  uint32_t DMAC_PKG_NUM_REGN;              /*!< Offset 0x130 DMAC Package Number Register N (N = 0 to 15) 0x0130 + N*0x0040 */
             RESERVED(0x034[0x0040 - 0x0034], uint8_t)
    } CH [0x010];                                     /*!< Offset 0x100 Channel [0..15] */
         RESERVED(0x500[0x1000 - 0x0500], uint8_t)
} DMAC_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief DMIC
 */
/*!< DMIC  */
typedef struct DMIC_Type
{
    __IO uint32_t DMIC_EN;                            /*!< Offset 0x000 DMIC Enable Control Register */
    __IO uint32_t DMIC_SR;                            /*!< Offset 0x004 DMIC Sample Rate Register */
    __IO uint32_t DMIC_CTR;                           /*!< Offset 0x008 DMIC Control Register */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IO uint32_t DMIC_DATA;                          /*!< Offset 0x010 DMIC Data Register */
    __IO uint32_t DMIC_INTC;                          /*!< Offset 0x014 MIC Interrupt Control Register */
    __IO uint32_t DMIC_INTS;                          /*!< Offset 0x018 DMIC Interrupt Status Register */
    __IO uint32_t DMIC_RXFIFO_CTR;                    /*!< Offset 0x01C DMIC RXFIFO Control Register */
    __IO uint32_t DMIC_RXFIFO_STA;                    /*!< Offset 0x020 DMIC RXFIFO Status Register */
    __IO uint32_t DMIC_CH_NUM;                        /*!< Offset 0x024 DMIC Channel Numbers Register */
    __IO uint32_t DMIC_CH_MAP;                        /*!< Offset 0x028 DMIC Channel Mapping Register */
    __IO uint32_t DMIC_CNT;                           /*!< Offset 0x02C DMIC Counter Register */
    __IO uint32_t DATA0_DATA1_VOL_CTR;                /*!< Offset 0x030 Data0 and Data1 Volume Control Register */
    __IO uint32_t DATA2_DATA3_VOL_CTR;                /*!< Offset 0x034 Data2 And Data3 Volume Control Register */
    __IO uint32_t HPF_EN_CTR;                         /*!< Offset 0x038 High Pass Filter Enable Control Register */
    __IO uint32_t HPF_COEF_REG;                       /*!< Offset 0x03C High Pass Filter Coefficient Register */
    __IO uint32_t HPF_GAIN_REG;                       /*!< Offset 0x040 High Pass Filter Gain Register */
         RESERVED(0x044[0x0400 - 0x0044], uint8_t)
} DMIC_TypeDef; /* size of structure = 0x400 */
/*
 * @brief DSI
 */
/*!< DSI MIPI DSI Display Interface */
typedef struct DSI_Type
{
    __IO uint32_t DSI_CTL_REG;                        /*!< Offset 0x000 dsi_gctl 0x00 - 0x0c */
    __IO uint32_t DSI_GINT0_REG;                      /*!< Offset 0x004 dsi_gint0  */
    __IO uint32_t DSI_GINT1_REG;                      /*!< Offset 0x008 dsi_gint1  */
    __IO uint32_t DSI_BASIC_CTL_REG;                  /*!< Offset 0x00C dsi_basic_ctl  */
    __IO uint32_t DSI_BASIC_CTL0_REG;                 /*!< Offset 0x010 dsi_basic_ctl0 0x10 - 0x1c */
    __IO uint32_t DSI_BASIC_CTL1_REG;                 /*!< Offset 0x014 dsi_basic_ctl1  */
    __IO uint32_t DSI_BASIC_SIZE0_REG;                /*!< Offset 0x018 dsi_basic_size0  */
    __IO uint32_t DSI_BASIC_SIZE1_REG;                /*!< Offset 0x01C dsi_basic_size1  */
    __IO uint32_t DSI_BASIC_INST0_REG [0x008];        /*!< Offset 0x020 dsi_inst_func[8] 0x20 - 0x3c */
    __IO uint32_t DSI_INST_LOOP_SEL_REG;              /*!< Offset 0x040 dsi_inst_loop_sel 0x40 - 0x5c */
    __IO uint32_t DSI_INST_LOOP_NUM_REG;              /*!< Offset 0x044 dsi_inst_loop_num  */
    __IO uint32_t DSI_INST_JUMP_SEL_REG;              /*!< Offset 0x048 dsi_inst_jump_sel  */
    __IO uint32_t DSI_INST_JUMP_CFG_REG [0x002];      /*!< Offset 0x04C dsi_inst_jump_cfg[2]  */
    __IO uint32_t DSI_INST_LOOP_NUM2_REG;             /*!< Offset 0x054 dsi_inst_loop_num2  */
         RESERVED(0x058[0x0060 - 0x0058], uint8_t)
    __IO uint32_t DSI_TRANS_START_REG;                /*!< Offset 0x060 dsi_trans_start 0x60 - 0x6c */
         RESERVED(0x064[0x0078 - 0x0064], uint8_t)
    __IO uint32_t DSI_TRANS_ZERO_REG;                 /*!< Offset 0x078 dsi_trans_zero  */
    __IO uint32_t DSI_TCON_DRQ_REG;                   /*!< Offset 0x07C dsi_tcon_drq  */
    __IO uint32_t DSI_PIXEL_CTL0_REG;                 /*!< Offset 0x080 dsi_pixel_ctl0 0x80 - 0x8c */
    __IO uint32_t DSI_PIXEL_CTL1_REG;                 /*!< Offset 0x084 dsi_pixel_ctl1  */
         RESERVED(0x088[0x0090 - 0x0088], uint8_t)
    __IO uint32_t DSI_PIXEL_PH_REG;                   /*!< Offset 0x090 dsi_pixel_ph 0x90 - 0x9c */
    __IO uint32_t DSI_PIXEL_PD_REG;                   /*!< Offset 0x094 dsi_pixel_pd  */
    __IO uint32_t DSI_PIXEL_PF0_REG;                  /*!< Offset 0x098 dsi_pixel_pf0  */
    __IO uint32_t DSI_PIXEL_PF1_REG;                  /*!< Offset 0x09C dsi_pixel_pf1  */
         RESERVED(0x0A0[0x00B0 - 0x00A0], uint8_t)
    __IO uint32_t DSI_SYNC_HSS_REG;                   /*!< Offset 0x0B0 dsi_sync_hss 0xb0 - 0xbc */
    __IO uint32_t DSI_SYNC_HSE_REG;                   /*!< Offset 0x0B4 dsi_sync_hse  */
    __IO uint32_t DSI_SYNC_VSS_REG;                   /*!< Offset 0x0B8 dsi_sync_vss  */
    __IO uint32_t DSI_SYNC_VSE_REG;                   /*!< Offset 0x0BC dsi_sync_vse  */
    __IO uint32_t DSI_BLK_HSA0_REG;                   /*!< Offset 0x0C0 dsi_blk_hsa0 0xc0 - 0xcc */
    __IO uint32_t DSI_BLK_HSA1_REG;                   /*!< Offset 0x0C4 dsi_blk_hsa1  */
    __IO uint32_t DSI_BLK_HBP0_REG;                   /*!< Offset 0x0C8 dsi_blk_hbp0  */
    __IO uint32_t DSI_BLK_HBP1_REG;                   /*!< Offset 0x0CC dsi_blk_hbp1  */
    __IO uint32_t DSI_BLK_HFP0_REG;                   /*!< Offset 0x0D0 dsi_blk_hfp0 0xd0 - 0xdc */
    __IO uint32_t DSI_BLK_HFP1_REG;                   /*!< Offset 0x0D4 dsi_blk_hfp1  */
         RESERVED(0x0D8[0x00E0 - 0x00D8], uint8_t)
    __IO uint32_t DSI_BLK_HBLK0_REG;                  /*!< Offset 0x0E0 dsi_blk_hblk0 0xe0 - 0xec */
    __IO uint32_t DSI_BLK_HBLK1_REG;                  /*!< Offset 0x0E4 dsi_blk_hblk1  */
    __IO uint32_t DSI_BLK_VBLK0_REG;                  /*!< Offset 0x0E8 dsi_blk_vblk0  */
    __IO uint32_t DSI_BLK_VBLK1_REG;                  /*!< Offset 0x0EC dsi_blk_vblk1  */
    __IO uint32_t DSI_BURST_LINE_REG;                 /*!< Offset 0x0F0 dsi_burst_line 0xf0 - 0x1fc */
    __IO uint32_t DSI_BURST_DRQ_REG;                  /*!< Offset 0x0F4 dsi_burst_drq  */
         RESERVED(0x0F8[0x0200 - 0x00F8], uint8_t)
    __IO uint32_t DSI_CMD_CTL_REG;                    /*!< Offset 0x200 dsi_cmd_ctl 0x200 - 0x23c */
         RESERVED(0x204[0x0240 - 0x0204], uint8_t)
    __IO uint32_t DSI_CMD_RX_REG [0x008];             /*!< Offset 0x240 dsi_cmd_rx[8] 0x240 - 0x2dc */
         RESERVED(0x260[0x02E0 - 0x0260], uint8_t)
    __IO uint32_t DSI_DEBUG_VIDEO0_REG;               /*!< Offset 0x2E0 dsi_debug_video0 0x2e0 - 0x2ec */
    __IO uint32_t DSI_DEBUG_VIDEO1_REG;               /*!< Offset 0x2E4 dsi_debug_video1  */
         RESERVED(0x2E8[0x02F0 - 0x02E8], uint8_t)
    __IO uint32_t DSI_DEBUG_INST_REG;                 /*!< Offset 0x2F0 dsi_debug_inst 0x2f0 - 0x2fc */
    __IO uint32_t DSI_DEBUG_FIFO_REG;                 /*!< Offset 0x2F4 dsi_debug_fifo  */
    __IO uint32_t DSI_DEBUG_DATA_REG;                 /*!< Offset 0x2F8 dsi_debug_data  */
         RESERVED(0x2FC[0x0300 - 0x02FC], uint8_t)
    __IO uint32_t DSI_CMD_TX_REG [0x040];             /*!< Offset 0x300 dsi_cmd_tx[64] 0x300 - 0x3fc */
} DSI_TypeDef; /* size of structure = 0x400 */
/*
 * @brief DSI_DPHY
 */
/*!< DSI_DPHY MIPI DSI Physical Interface */
typedef struct DSI_DPHY_Type
{
    __IO uint32_t DPHY_GCTL_REG;                      /*!< Offset 0x000  */
    __IO uint32_t DPHY_TX_CTL_REG;                    /*!< Offset 0x004  */
    __IO uint32_t DPHY_RX_CTL_REG;                    /*!< Offset 0x008  */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IO uint32_t DPHY_TX_TIME0_REG;                  /*!< Offset 0x010  */
    __IO uint32_t DPHY_TX_TIME1_REG;                  /*!< Offset 0x014  */
    __IO uint32_t DPHY_TX_TIME2_REG;                  /*!< Offset 0x018  */
    __IO uint32_t DPHY_TX_TIME3_REG;                  /*!< Offset 0x01C  */
    __IO uint32_t DPHY_TX_TIME4_REG;                  /*!< Offset 0x020  */
         RESERVED(0x024[0x0030 - 0x0024], uint8_t)
    __IO uint32_t DPHY_RX_TIME0_REG;                  /*!< Offset 0x030  */
    __IO uint32_t DPHY_RX_TIME1_REG;                  /*!< Offset 0x034  */
    __IO uint32_t DPHY_RX_TIME2_REG;                  /*!< Offset 0x038  */
         RESERVED(0x03C[0x0040 - 0x003C], uint8_t)
    __IO uint32_t DPHY_RX_TIME3_REG;                  /*!< Offset 0x040  */
         RESERVED(0x044[0x004C - 0x0044], uint8_t)
    __IO uint32_t DPHY_ANA0_REG;                      /*!< Offset 0x04C  */
    __IO uint32_t DPHY_ANA1_REG;                      /*!< Offset 0x050  */
    __IO uint32_t DPHY_ANA2_REG;                      /*!< Offset 0x054  */
    __IO uint32_t DPHY_ANA3_REG;                      /*!< Offset 0x058  */
    __IO uint32_t DPHY_ANA4_REG;                      /*!< Offset 0x05C  */
    __IO uint32_t DPHY_INT_EN0_REG;                   /*!< Offset 0x060  */
    __IO uint32_t DPHY_INT_EN1_REG;                   /*!< Offset 0x064  */
    __IO uint32_t DPHY_INT_EN2_REG;                   /*!< Offset 0x068  */
         RESERVED(0x06C[0x0070 - 0x006C], uint8_t)
    __IO uint32_t DPHY_INT_PD0_REG;                   /*!< Offset 0x070  */
    __IO uint32_t DPHY_INT_PD1_REG;                   /*!< Offset 0x074  */
    __IO uint32_t DPHY_INT_PD2_REG;                   /*!< Offset 0x078  */
         RESERVED(0x07C[0x00E0 - 0x007C], uint8_t)
    __IO uint32_t DPHY_DBG0_REG;                      /*!< Offset 0x0E0  */
    __IO uint32_t DPHY_DBG1_REG;                      /*!< Offset 0x0E4  */
    __IO uint32_t DPHY_DBG2_REG;                      /*!< Offset 0x0E8  */
    __IO uint32_t DPHY_DBG3_REG;                      /*!< Offset 0x0EC  */
    __IO uint32_t DPHY_DBG4_REG;                      /*!< Offset 0x0F0  */
    __IO uint32_t DPHY_DBG5_REG;                      /*!< Offset 0x0F4  */
    __IO uint32_t DPHY_TX_SKEW_REG0;                  /*!< Offset 0x0F8  */
    __IO uint32_t DPHY_TX_SKEW_REG1;                  /*!< Offset 0x0FC  */
    __IO uint32_t DPHY_TX_SKEW_REG2;                  /*!< Offset 0x100  */
    __IO uint32_t DPHY_PLL_REG0;                      /*!< Offset 0x104  */
    __IO uint32_t DPHY_PLL_REG1;                      /*!< Offset 0x108  */
    __IO uint32_t DPHY_PLL_REG2;                      /*!< Offset 0x10C  */
    __IO uint32_t COMBO_PHY_REG0;                     /*!< Offset 0x110 The TCON LCD0 PHY0 is controlled by COMBO_PHY_REG (reg0x1110, reg0x1114) */
    __IO uint32_t COMBO_PHY_REG1;                     /*!< Offset 0x114 The TCON LCD0 PHY0 is controlled by COMBO_PHY_REG (reg0x1110, reg0x1114) */
    __IO uint32_t COMBO_PHY_REG2;                     /*!< Offset 0x118  */
} DSI_DPHY_TypeDef; /* size of structure = 0x11C */
/*
 * @brief DSP_CFG
 */
/*!< DSP_CFG  */
typedef struct DSP_CFG_Type
{
    __IO uint32_t DSP_ALT_RESET_VEC_REG;              /*!< Offset 0x000 DSP Reset Control Register */
    __IO uint32_t DSP_CTRL_REG0;                      /*!< Offset 0x004 DSP Control Register0 */
         RESERVED(0x008[0x000C - 0x0008], uint8_t)
    __IO uint32_t DSP_PRID_REG;                       /*!< Offset 0x00C DSP PRID Register */
    __IO uint32_t DSP_STAT_REG;                       /*!< Offset 0x010 DSP STAT Register */
    __IO uint32_t DSP_BIST_CTRL_REG;                  /*!< Offset 0x014 DSP BIST CTRL Register */
         RESERVED(0x018[0x001C - 0x0018], uint8_t)
    __IO uint32_t DSP_JTRST_REG;                      /*!< Offset 0x01C DSP JTAG CONFIG RESET Register */
    __IO uint32_t DSP_VER_REG;                        /*!< Offset 0x020 DSP Version Register */
} DSP_CFG_TypeDef; /* size of structure = 0x024 */
/*
 * @brief DSP_INTC
 */
/*!< DSP_INTC XTensa HiFi4 Interrupt Controller */
typedef struct DSP_INTC_Type
{
    __IO uint32_t VECTOR;                             /*!< Offset 0x000  */
    __IO uint32_t BASE_ADDR;                          /*!< Offset 0x004  */
         RESERVED(0x008[0x000C - 0x0008], uint8_t)
    __IO uint32_t CONTROL;                            /*!< Offset 0x00C  */
         RESERVED(0x010[0x0014 - 0x0010], uint8_t)
    __IO uint32_t PENDING [0x003];                    /*!< Offset 0x014  */
         RESERVED(0x020[0x0040 - 0x0020], uint8_t)
    __IO uint32_t ENABLE [0x003];                     /*!< Offset 0x040  */
         RESERVED(0x04C[0x0050 - 0x004C], uint8_t)
    __IO uint32_t MASK [0x003];                       /*!< Offset 0x050  */
         RESERVED(0x05C[0x0070 - 0x005C], uint8_t)
    __IO uint32_t FAST_FORCING;                       /*!< Offset 0x070  */
         RESERVED(0x074[0x0080 - 0x0074], uint8_t)
    __IO uint32_t PRIORITY [0x002];                   /*!< Offset 0x080  */
         RESERVED(0x088[0x00C0 - 0x0088], uint8_t)
    __IO uint32_t GROUP_CONFIG [0x004];               /*!< Offset 0x0C0  */
} DSP_INTC_TypeDef; /* size of structure = 0x0D0 */
/*
 * @brief EMAC
 */
/*!< EMAC  */
typedef struct EMAC_Type
{
    __IO uint32_t EMAC_BASIC_CTL0;                    /*!< Offset 0x000 EMAC Basic Control Register0 */
    __IO uint32_t EMAC_BASIC_CTL1;                    /*!< Offset 0x004 EMAC Basic Control Register1 */
    __IO uint32_t EMAC_INT_STA;                       /*!< Offset 0x008 EMAC Interrupt Status Register */
    __IO uint32_t EMAC_INT_EN;                        /*!< Offset 0x00C EMAC Interrupt Enable Register */
    __IO uint32_t EMAC_TX_CTL0;                       /*!< Offset 0x010 EMAC Transmit Control Register0 */
    __IO uint32_t EMAC_TX_CTL1;                       /*!< Offset 0x014 EMAC Transmit Control Register1 */
         RESERVED(0x018[0x001C - 0x0018], uint8_t)
    __IO uint32_t EMAC_TX_FLOW_CTL;                   /*!< Offset 0x01C EMAC Transmit Flow Control Register */
    __IO uint32_t EMAC_TX_DMA_DESC_LIST;              /*!< Offset 0x020 EMAC Transmit Descriptor List Address Register */
    __IO uint32_t EMAC_RX_CTL0;                       /*!< Offset 0x024 EMAC Receive Control Register0 */
    __IO uint32_t EMAC_RX_CTL1;                       /*!< Offset 0x028 EMAC Receive Control Register1 */
         RESERVED(0x02C[0x0034 - 0x002C], uint8_t)
    __IO uint32_t EMAC_RX_DMA_DESC_LIST;              /*!< Offset 0x034 EMAC Receive Descriptor List Address Register */
    __IO uint32_t EMAC_RX_FRM_FLT;                    /*!< Offset 0x038 EMAC Receive Frame Filter Register */
         RESERVED(0x03C[0x0040 - 0x003C], uint8_t)
    __IO uint32_t EMAC_RX_HASH0;                      /*!< Offset 0x040 EMAC Hash Table Register0 */
    __IO uint32_t EMAC_RX_HASH1;                      /*!< Offset 0x044 EMAC Hash Table Register1 */
    __IO uint32_t EMAC_MII_CMD;                       /*!< Offset 0x048 EMAC Management Interface Command Register */
    __IO uint32_t EMAC_MII_DATA;                      /*!< Offset 0x04C EMAC Management Interface Data Register */
    struct
    {
        __IO uint32_t HIGH;                           /*!< Offset 0x050 EMAC MAC Address High Register */
        __IO uint32_t LOW;                            /*!< Offset 0x054 EMAC MAC Address Low Register */
    } EMAC_ADDR [0x008];                              /*!< Offset 0x050 EMAC MAC Address N (N=0-7) */
         RESERVED(0x090[0x00B0 - 0x0090], uint8_t)
    __IO uint32_t EMAC_TX_DMA_STA;                    /*!< Offset 0x0B0 EMAC Transmit DMA Status Register */
    __IO uint32_t EMAC_TX_CUR_DESC;                   /*!< Offset 0x0B4 EMAC Current Transmit Descriptor Register */
    __IO uint32_t EMAC_TX_CUR_BUF;                    /*!< Offset 0x0B8 EMAC Current Transmit Buffer Address Register */
         RESERVED(0x0BC[0x00C0 - 0x00BC], uint8_t)
    __IO uint32_t EMAC_RX_DMA_STA;                    /*!< Offset 0x0C0 EMAC Receive DMA Status Register */
    __IO uint32_t EMAC_RX_CUR_DESC;                   /*!< Offset 0x0C4 EMAC Current Receive Descriptor Register */
    __IO uint32_t EMAC_RX_CUR_BUF;                    /*!< Offset 0x0C8 EMAC Current Receive Buffer Address Register */
         RESERVED(0x0CC[0x00D0 - 0x00CC], uint8_t)
    __IO uint32_t EMAC_RGMII_STA;                     /*!< Offset 0x0D0 EMAC RGMII Status Register */
} EMAC_TypeDef; /* size of structure = 0x0D4 */
/*
 * @brief G2D_BLD
 */
/*!< G2D_BLD Graphic 2D (G2D) Engine Blender */
typedef struct G2D_BLD_Type
{
    __IO uint32_t BLD_FILL_COLOR_CTL;                 /*!< Offset 0x000 BLD_EN_CTL BLD_FILL_COLOR_CTL Allwinner_DE2.0_Spec_V1.0.pdf page 106   */
         RESERVED(0x004[0x0010 - 0x0004], uint8_t)
    __IO uint32_t BLD_FILL_COLOR [0x002];             /*!< Offset 0x010 BLD_FILLC  N=0..1  */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IO uint32_t BLD_CH_ISIZE [0x002];               /*!< Offset 0x020 BLD_CH_ISIZE N=0..1   */
         RESERVED(0x028[0x0030 - 0x0028], uint8_t)
    __IO uint32_t BLD_CH_OFFSET [0x002];              /*!< Offset 0x030 BLD_CH_OFFSET N=0..1 */
         RESERVED(0x038[0x0040 - 0x0038], uint8_t)
    __IO uint32_t BLD_PREMUL_CTL;                     /*!< Offset 0x040 BLD_PREMUL_CTL      */
    __IO uint32_t BLD_BK_COLOR;                       /*!< Offset 0x044 BLD_BK_COLOR        */
    __IO uint32_t BLD_SIZE;                           /*!< Offset 0x048 BLD_SIZE            */
    __IO uint32_t BLD_CTL;                            /*!< Offset 0x04C BLD_CTL             */
    __IO uint32_t BLD_KEY_CTL;                        /*!< Offset 0x050 BLD_KEY_CTL         */
    __IO uint32_t BLD_KEY_CON;                        /*!< Offset 0x054 BLD_KEY_CON         */
    __IO uint32_t BLD_KEY_MAX;                        /*!< Offset 0x058 BLD_KEY_MAX         */
    __IO uint32_t BLD_KEY_MIN;                        /*!< Offset 0x05C BLD_KEY_MIN         */
    __IO uint32_t BLD_OUT_COLOR;                      /*!< Offset 0x060 BLD_OUT_COLOR       */
         RESERVED(0x064[0x0080 - 0x0064], uint8_t)
    __IO uint32_t ROP_CTL;                            /*!< Offset 0x080 ROP_CTL             */
    __IO uint32_t ROP_INDEX [0x004];                  /*!< Offset 0x084 ROP_INDEX N=0..1 ch3_index0 (tina linux sources has two elements) */
         RESERVED(0x094[0x0100 - 0x0094], uint8_t)
    __IO uint32_t BLD_CSC_CTL;                        /*!< Offset 0x100 BLD_CSC_CTL         */
         RESERVED(0x104[0x0110 - 0x0104], uint8_t)
    __IO uint32_t BLD_CSC0_COEF00;                    /*!< Offset 0x110 BLD_CSC0_COEF00     */
    __IO uint32_t BLD_CSC0_COEF01;                    /*!< Offset 0x114 BLD_CSC0_COEF01     */
    __IO uint32_t BLD_CSC0_COEF02;                    /*!< Offset 0x118 BLD_CSC0_COEF02     */
    __IO uint32_t BLD_CSC0_CONST0;                    /*!< Offset 0x11C BLD_CSC0_CONST0     */
    __IO uint32_t BLD_CSC0_COEF10;                    /*!< Offset 0x120 BLD_CSC0_COEF10     */
    __IO uint32_t BLD_CSC0_COEF11;                    /*!< Offset 0x124 BLD_CSC0_COEF11     */
    __IO uint32_t BLD_CSC0_COEF12;                    /*!< Offset 0x128 BLD_CSC0_COEF12     */
    __IO uint32_t BLD_CSC0_CONST1;                    /*!< Offset 0x12C BLD_CSC0_CONST1     */
    __IO uint32_t BLD_CSC0_COEF20;                    /*!< Offset 0x130 BLD_CSC0_COEF20     */
    __IO uint32_t BLD_CSC0_COEF21;                    /*!< Offset 0x134 BLD_CSC0_COEF21     */
    __IO uint32_t BLD_CSC0_COEF22;                    /*!< Offset 0x138 BLD_CSC0_COEF22     */
    __IO uint32_t BLD_CSC0_CONST2;                    /*!< Offset 0x13C BLD_CSC0_CONST2     */
    __IO uint32_t BLD_CSC1_COEF00;                    /*!< Offset 0x140 BLD_CSC1_COEF00     */
    __IO uint32_t BLD_CSC1_COEF01;                    /*!< Offset 0x144 BLD_CSC1_COEF01     */
    __IO uint32_t BLD_CSC1_COEF02;                    /*!< Offset 0x148 BLD_CSC1_COEF02     */
    __IO uint32_t BLD_CSC1_CONST0;                    /*!< Offset 0x14C BLD_CSC1_CONST0     */
    __IO uint32_t BLD_CSC1_COEF10;                    /*!< Offset 0x150 BLD_CSC1_COEF10     */
    __IO uint32_t BLD_CSC1_COEF11;                    /*!< Offset 0x154 BLD_CSC1_COEF11     */
    __IO uint32_t BLD_CSC1_COEF12;                    /*!< Offset 0x158 BLD_CSC1_COEF12     */
    __IO uint32_t BLD_CSC1_CONST1;                    /*!< Offset 0x15C BLD_CSC1_CONST1     */
    __IO uint32_t BLD_CSC1_COEF20;                    /*!< Offset 0x160 BLD_CSC1_COEF20     */
    __IO uint32_t BLD_CSC1_COEF21;                    /*!< Offset 0x164 BLD_CSC1_COEF21     */
    __IO uint32_t BLD_CSC1_COEF22;                    /*!< Offset 0x168 BLD_CSC1_COEF22     */
    __IO uint32_t BLD_CSC1_CONST2;                    /*!< Offset 0x16C BLD_CSC1_CONST2     */
    __IO uint32_t BLD_CSC2_COEF00;                    /*!< Offset 0x170 BLD_CSC2_COEF00     */
    __IO uint32_t BLD_CSC2_COEF01;                    /*!< Offset 0x174 BLD_CSC2_COEF01     */
    __IO uint32_t BLD_CSC2_COEF02;                    /*!< Offset 0x178 BLD_CSC2_COEF02     */
    __IO uint32_t BLD_CSC2_CONST0;                    /*!< Offset 0x17C BLD_CSC2_CONST0     */
    __IO uint32_t BLD_CSC2_COEF10;                    /*!< Offset 0x180 BLD_CSC2_COEF10     */
    __IO uint32_t BLD_CSC2_COEF11;                    /*!< Offset 0x184 BLD_CSC2_COEF11     */
    __IO uint32_t BLD_CSC2_COEF12;                    /*!< Offset 0x188 BLD_CSC2_COEF12     */
    __IO uint32_t BLD_CSC2_CONST1;                    /*!< Offset 0x18C BLD_CSC2_CONST1     */
    __IO uint32_t BLD_CSC2_COEF20;                    /*!< Offset 0x190 BLD_CSC2_COEF20     */
    __IO uint32_t BLD_CSC2_COEF21;                    /*!< Offset 0x194 BLD_CSC2_COEF21     */
    __IO uint32_t BLD_CSC2_COEF22;                    /*!< Offset 0x198 BLD_CSC2_COEF22     */
    __IO uint32_t BLD_CSC2_CONST2;                    /*!< Offset 0x19C BLD_CSC2_CONST2     */
} G2D_BLD_TypeDef; /* size of structure = 0x1A0 */
/*
 * @brief G2D_MIXER
 */
/*!< G2D_MIXER Graphic 2D (G2D) Engine Video Mixer */
typedef struct G2D_MIXER_Type
{
    __IO uint32_t G2D_MIXER_CTRL;                     /*!< Offset 0x000 G2D mixer control */
    __IO uint32_t G2D_MIXER_INTERRUPT;                /*!< Offset 0x004 G2D mixer interrupt */
    __IO uint32_t G2D_MIXER_CLK;                      /*!< Offset 0x008 G2D mixer clock */
} G2D_MIXER_TypeDef; /* size of structure = 0x00C */
/*
 * @brief G2D_ROT
 */
/*!< G2D_ROT Graphic 2D Rotate */
typedef struct G2D_ROT_Type
{
    __IO uint32_t ROT_CTL;                            /*!< Offset 0x000 ROT_CTL						 */
    __IO uint32_t ROT_INT;                            /*!< Offset 0x004 ROT_INT				 */
    __IO uint32_t ROT_TIMEOUT;                        /*!< Offset 0x008 ROT_TIMEOUT */
         RESERVED(0x00C[0x0020 - 0x000C], uint8_t)
    __IO uint32_t ROT_IFMT;                           /*!< Offset 0x020 Input data attribute register */
    __IO uint32_t ROT_ISIZE;                          /*!< Offset 0x024 Input data size register */
         RESERVED(0x028[0x0030 - 0x0028], uint8_t)
    __IO uint32_t ROT_IPITCH0;                        /*!< Offset 0x030 Input Y/RGB/ARGB memory pitch register */
    __IO uint32_t ROT_IPITCH1;                        /*!< Offset 0x034 Input U/UV memory pitch register */
    __IO uint32_t ROT_IPITCH2;                        /*!< Offset 0x038 Input V memory pitch register */
         RESERVED(0x03C[0x0040 - 0x003C], uint8_t)
    __IO uint32_t ROT_ILADD0;                         /*!< Offset 0x040 Input Y/RGB/ARGB memory address register0 */
    __IO uint32_t ROT_IHADD0;                         /*!< Offset 0x044 Input Y/RGB/ARGB memory address register1 */
    __IO uint32_t ROT_ILADD1;                         /*!< Offset 0x048 Input U/UV memory address register0 */
    __IO uint32_t ROT_IHADD1;                         /*!< Offset 0x04C Input U/UV memory address register1 */
    __IO uint32_t ROT_ILADD2;                         /*!< Offset 0x050 Input V memory address register0 */
    __IO uint32_t ROT_IHADD2;                         /*!< Offset 0x054 Input V memory address register1 */
         RESERVED(0x058[0x0084 - 0x0058], uint8_t)
    __IO uint32_t ROT_OSIZE;                          /*!< Offset 0x084 Output data size register */
         RESERVED(0x088[0x0090 - 0x0088], uint8_t)
    __IO uint32_t ROT_OPITCH0;                        /*!< Offset 0x090 Output Y/RGB/ARGB memory pitch register */
    __IO uint32_t ROT_OPITCH1;                        /*!< Offset 0x094 Output U/UV memory pitch register */
    __IO uint32_t ROT_OPITCH2;                        /*!< Offset 0x098 Output V memory pitch register */
         RESERVED(0x09C[0x00A0 - 0x009C], uint8_t)
    __IO uint32_t ROT_OLADD0;                         /*!< Offset 0x0A0 Output Y/RGB/ARGB memory address register0 */
    __IO uint32_t ROT_OHADD0;                         /*!< Offset 0x0A4 Output Y/RGB/ARGB memory address register1 */
    __IO uint32_t ROT_OLADD1;                         /*!< Offset 0x0A8 Output U/UV memory address register0 */
    __IO uint32_t ROT_OHADD1;                         /*!< Offset 0x0AC Output U/UV memory address register1 */
    __IO uint32_t ROT_OLADD2;                         /*!< Offset 0x0B0 Output V memory address register0 */
    __IO uint32_t ROT_OHADD2;                         /*!< Offset 0x0B4 Output V memory address register1 */
         RESERVED(0x0B8[0x00C8 - 0x00B8], uint8_t)
    __IO uint32_t LBC_ENC_CTL;                        /*!< Offset 0x0C8 Encode control (bit 31 - is lossy) */
    __IO uint32_t LBC_CTL;                            /*!< Offset 0x0CC  */
    __IO uint32_t LBC_DEC_CTL;                        /*!< Offset 0x0D0 Decode control (bit 31 - is lossy)  */
} G2D_ROT_TypeDef; /* size of structure = 0x0D4 */
/*
 * @brief G2D_TOP
 */
/*!< G2D_TOP Graphic 2D top */
typedef struct G2D_TOP_Type
{
    __IO uint32_t G2D_SCLK_GATE;                      /*!< Offset 0x000 G2D SCLK gate   */
    __IO uint32_t G2D_HCLK_GATE;                      /*!< Offset 0x004 g2d HCLK gate   */
    __IO uint32_t G2D_AHB_RST;                        /*!< Offset 0x008 G2D AHB reset   */
    __IO uint32_t G2D_SCLK_DIV;                       /*!< Offset 0x00C G2D SCLK div    */
    __I  uint32_t G2D_VERSION;                        /*!< Offset 0x010  */
         RESERVED(0x014[0x0020 - 0x0014], uint8_t)
    __IO uint32_t RCQ_IRQ_CTL;                        /*!< Offset 0x020  */
    __IO uint32_t RCQ_STATUS;                         /*!< Offset 0x024  */
    __IO uint32_t RCQ_CTRL;                           /*!< Offset 0x028  */
    __IO uint32_t RCQ_HEADER_LOW_ADDR;                /*!< Offset 0x02C Register Configuration Queue Header bits 31..0 */
    __IO uint32_t RCQ_HEADER_HIGH_ADDR;               /*!< Offset 0x030 Register Configuration Queue Header bits 39..32 */
    __IO uint32_t RCQ_HEADER_LEN;                     /*!< Offset 0x034 Register Configuration Queue length bits 15..0 */
} G2D_TOP_TypeDef; /* size of structure = 0x038 */
/*
 * @brief G2D_UI
 */
/*!< G2D_UI Graphic 2D UI surface */
typedef struct G2D_UI_Type
{
    __IO uint32_t UI_ATTR;                            /*!< Offset 0x000 UIx_ATTR */
    __IO uint32_t UI_MBSIZE;                          /*!< Offset 0x004 UIx_MBSIZE */
    __IO uint32_t UI_COOR;                            /*!< Offset 0x008 UIx_COOR */
    __IO uint32_t UI_PITCH;                           /*!< Offset 0x00C UIx_PITCH */
    __IO uint32_t UI_LADD;                            /*!< Offset 0x010 UIx_LADD */
    __IO uint32_t UI_FILLC;                           /*!< Offset 0x014 UIx_FILLC */
    __IO uint32_t UI_HADD;                            /*!< Offset 0x018 UIx_HADD */
    __IO uint32_t UI_SIZE;                            /*!< Offset 0x01C UIx_SIZE */
         RESERVED(0x020[0x0800 - 0x0020], uint8_t)
} G2D_UI_TypeDef; /* size of structure = 0x800 */
/*
 * @brief G2D_VI
 */
/*!< G2D_VI Graphic 2D VI surface */
typedef struct G2D_VI_Type
{
    __IO uint32_t V0_ATTCTL;                          /*!< Offset 0x000 V0_ATTCTL */
    __IO uint32_t V0_MBSIZE;                          /*!< Offset 0x004 Source rectangle size (may be empty) */
    __IO uint32_t V0_COOR;                            /*!< Offset 0x008 Target window position */
    __IO uint32_t V0_PITCH0;                          /*!< Offset 0x00C V0_PITCH0 */
    __IO uint32_t V0_PITCH1;                          /*!< Offset 0x010 V0_PITCH1 */
    __IO uint32_t V0_PITCH2;                          /*!< Offset 0x014 V0_PITCH2 */
    __IO uint32_t V0_LADD0;                           /*!< Offset 0x018 V0_LADD0 */
    __IO uint32_t V0_LADD1;                           /*!< Offset 0x01C V0_LADD1 */
    __IO uint32_t V0_LADD2;                           /*!< Offset 0x020 V0_LADD2 */
    __IO uint32_t V0_FILLC;                           /*!< Offset 0x024 V0_FILLC */
    __IO uint32_t V0_HADD;                            /*!< Offset 0x028 [31:24]: 0, [23:16]: LADD2 bits 40..32, [15:8]: LADD1 bits 40..32, [7:0]: LADD0 bits 40..32 */
    __IO uint32_t V0_SIZE;                            /*!< Offset 0x02C Source window size (may not be empty) */
    __IO uint32_t V0_HDS_CTL0;                        /*!< Offset 0x030 V0_HDS_CTL0 */
    __IO uint32_t V0_HDS_CTL1;                        /*!< Offset 0x034 V0_HDS_CTL1 */
    __IO uint32_t V0_VDS_CTL0;                        /*!< Offset 0x038 V0_VDS_CTL0 */
    __IO uint32_t V0_VDS_CTL1;                        /*!< Offset 0x03C V0_VDS_CTL1 */
         RESERVED(0x040[0x0800 - 0x0040], uint8_t)
} G2D_VI_TypeDef; /* size of structure = 0x800 */
/*
 * @brief G2D_VSU
 */
/*!< G2D_VSU Graphic 2D Video Scaler */
typedef struct G2D_VSU_Type
{
    __IO uint32_t VS_CTRL;                            /*!< Offset 0x000 VS_CTRL */
         RESERVED(0x004[0x0040 - 0x0004], uint8_t)
    __IO uint32_t VS_OUT_SIZE;                        /*!< Offset 0x040 VS_OUT_SIZE */
    __IO uint32_t VS_GLB_ALPHA;                       /*!< Offset 0x044 VS_GLB_ALPHA */
         RESERVED(0x048[0x0080 - 0x0048], uint8_t)
    __IO uint32_t VS_Y_SIZE;                          /*!< Offset 0x080 VS_Y_SIZE */
         RESERVED(0x084[0x0088 - 0x0084], uint8_t)
    __IO uint32_t VS_Y_HSTEP;                         /*!< Offset 0x088 VS_Y_HSTEP */
    __IO uint32_t VS_Y_VSTEP;                         /*!< Offset 0x08C VS_Y_VSTEP */
    __IO uint32_t VS_Y_HPHASE;                        /*!< Offset 0x090 VS_Y_HPHASE */
         RESERVED(0x094[0x0098 - 0x0094], uint8_t)
    __IO uint32_t VS_Y_VPHASE0;                       /*!< Offset 0x098 VS_Y_VPHASE0 */
         RESERVED(0x09C[0x00C0 - 0x009C], uint8_t)
    __IO uint32_t VS_C_SIZE;                          /*!< Offset 0x0C0 VS_C_SIZE */
         RESERVED(0x0C4[0x00C8 - 0x00C4], uint8_t)
    __IO uint32_t VS_C_HSTEP;                         /*!< Offset 0x0C8 VS_C_HSTEP */
    __IO uint32_t VS_C_VSTEP;                         /*!< Offset 0x0CC VS_C_VSTEP */
    __IO uint32_t VS_C_HPHASE;                        /*!< Offset 0x0D0 VS_C_HPHASE */
         RESERVED(0x0D4[0x00D8 - 0x00D4], uint8_t)
    __IO uint32_t VS_C_VPHASE0;                       /*!< Offset 0x0D8 VS_C_VPHASE0 */
         RESERVED(0x0DC[0x0200 - 0x00DC], uint8_t)
    __IO uint32_t VS_Y_HCOEF [0x040];                 /*!< Offset 0x200 VS_Y_HCOEF[N]. N=0..31 */
    __IO uint32_t VS_Y_VCOEF [0x040];                 /*!< Offset 0x300 VS_Y_VCOEF[N]. N=0..31 */
    __IO uint32_t VS_C_HCOEF [0x040];                 /*!< Offset 0x400 VS_C_HCOEF[N]. N=0..31 */
} G2D_VSU_TypeDef; /* size of structure = 0x500 */
/*
 * @brief G2D_WB
 */
/*!< G2D_WB Graphic 2D (G2D) Engine Write Back */
typedef struct G2D_WB_Type
{
    __IO uint32_t WB_ATT;                             /*!< Offset 0x000 WB_ATT */
    __IO uint32_t WB_SIZE;                            /*!< Offset 0x004 WB_SIZE */
    __IO uint32_t WB_PITCH0;                          /*!< Offset 0x008 WB_PITCH0 */
    __IO uint32_t WB_PITCH1;                          /*!< Offset 0x00C WB_PITCH1 */
    __IO uint32_t WB_PITCH2;                          /*!< Offset 0x010 WB_PITCH2 */
    __IO uint32_t WB_LADD0;                           /*!< Offset 0x014 WB_LADD0 */
    __IO uint32_t WB_HADD0;                           /*!< Offset 0x018 WB_HADD0 */
    __IO uint32_t WB_LADD1;                           /*!< Offset 0x01C WB_LADD1 */
    __IO uint32_t WB_HADD1;                           /*!< Offset 0x020 WB_HADD1 */
    __IO uint32_t WB_LADD2;                           /*!< Offset 0x024 WB_LADD2 */
    __IO uint32_t WB_HADD2;                           /*!< Offset 0x028 WB_HADD2 */
} G2D_WB_TypeDef; /* size of structure = 0x02C */
/*
 * @brief GICV
 */
/*!< GICV  */
typedef struct GICV_Type
{
    __IO uint32_t GICH_HCR;                           /*!< Offset 0x000 RW 0x00000000 Hypervisor Control Register */
    __IO uint32_t GICH_VTR;                           /*!< Offset 0x004 RO 0x90000003 VGIC Type Register, GICH_VTR on page 3-13 */
    __IO uint32_t GICH_VMCR;                          /*!< Offset 0x008 RW 0x004C0000 Virtual Machine Control Register */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IO uint32_t GICH_MISR;                          /*!< Offset 0x010 RO 0x00000000 Maintenance Interrupt Status Register */
         RESERVED(0x014[0x0020 - 0x0014], uint8_t)
    __IO uint32_t GICH_EISR0;                         /*!< Offset 0x020 RO 0x00000000 End of Interrupt Status Register */
         RESERVED(0x024[0x0030 - 0x0024], uint8_t)
    __IO uint32_t GICH_ELSR0;                         /*!< Offset 0x030 RO 0x0000000F Empty List register Status Register */
         RESERVED(0x034[0x00F0 - 0x0034], uint8_t)
    __IO uint32_t GICH_APR0;                          /*!< Offset 0x0F0 RW 0x00000000 Active Priority Register */
         RESERVED(0x0F4[0x0100 - 0x00F4], uint8_t)
    __IO uint32_t GICH_LR0;                           /*!< Offset 0x100 RW 0x00000000 List Register 0 */
    __IO uint32_t GICH_LR1;                           /*!< Offset 0x104 RW 0x00000000 List Register 1 */
    __IO uint32_t GICH_LR2;                           /*!< Offset 0x108 RW 0x00000000 List Register 2 */
    __IO uint32_t GICH_LR3;                           /*!< Offset 0x10C RW 0x00000000 List Register 3 */
         RESERVED(0x110[0x0200 - 0x0110], uint8_t)
} GICV_TypeDef; /* size of structure = 0x200 */
/*
 * @brief GPADC
 */
/*!< GPADC  */
typedef struct GPADC_Type
{
    __IO uint32_t GP_SR_CON;                          /*!< Offset 0x000 GPADC Sample Rate Configure Register */
    __IO uint32_t GP_CTRL;                            /*!< Offset 0x004 GPADC Control Register */
    __IO uint32_t GP_CS_EN;                           /*!< Offset 0x008 GPADC Compare and Select Enable Register */
    __IO uint32_t GP_FIFO_INTC;                       /*!< Offset 0x00C GPADC FIFO Interrupt Control Register */
    __IO uint32_t GP_FIFO_INTS;                       /*!< Offset 0x010 GPADC FIFO Interrupt Status Register */
    __I  uint32_t GP_FIFO_DATA;                       /*!< Offset 0x014 GPADC FIFO Data Register */
    __IO uint32_t GP_CDATA;                           /*!< Offset 0x018 GPADC Calibration Data Register */
         RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    __IO uint32_t GP_DATAL_INTC;                      /*!< Offset 0x020 GPADC Data Low Interrupt Configure Register */
    __IO uint32_t GP_DATAH_INTC;                      /*!< Offset 0x024 GPADC Data High Interrupt Configure Register */
    __IO uint32_t GP_DATA_INTC;                       /*!< Offset 0x028 GPADC Data Interrupt Configure Register */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __IO uint32_t GP_DATAL_INTS;                      /*!< Offset 0x030 GPADC Data Low Interrupt Status Register */
    __IO uint32_t GP_DATAH_INTS;                      /*!< Offset 0x034 GPADC Data High Interrupt Status Register */
    __IO uint32_t GP_DATA_INTS;                       /*!< Offset 0x038 GPADC Data Interrupt Status Register */
         RESERVED(0x03C[0x0040 - 0x003C], uint8_t)
    __IO uint32_t GP_CH0_CMP_DATA;                    /*!< Offset 0x040 GPADC CH0 Compare Data Register */
         RESERVED(0x044[0x0080 - 0x0044], uint8_t)
    __I  uint32_t GP_CH0_DATA;                        /*!< Offset 0x080 GPADC CH0 Data Register */
} GPADC_TypeDef; /* size of structure = 0x084 */
/*
 * @brief GPIO
 */
/*!< GPIO  */
typedef struct GPIO_Type
{
    __IO uint32_t CFG [0x004];                        /*!< Offset 0x000 Configure Register */
    __IO uint32_t DATA;                               /*!< Offset 0x010 Data Register */
    __IO uint32_t DRV [0x004];                        /*!< Offset 0x014 Multi_Driving Register */
    __IO uint32_t PULL [0x002];                       /*!< Offset 0x024 Pull Register */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
} GPIO_TypeDef; /* size of structure = 0x030 */
/*
 * @brief GPIOBLOCK
 */
/*!< GPIOBLOCK  */
typedef struct GPIOBLOCK_Type
{
    struct
    {
        __IO uint32_t CFG [0x004];                    /*!< Offset 0x000 Configure Register */
        __IO uint32_t DATA;                           /*!< Offset 0x010 Data Register */
        __IO uint32_t DRV [0x004];                    /*!< Offset 0x014 Multi_Driving Register */
        __IO uint32_t PULL [0x002];                   /*!< Offset 0x024 Pull Register */
             RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    } GPIO_PINS [0x007];                              /*!< Offset 0x000 GPIO pin control */
         RESERVED(0x150[0x0200 - 0x0150], uint8_t)
    struct
    {
        __IO uint32_t EINT_CFG [0x004];               /*!< Offset 0x200 External Interrupt Configure Registers */
        __IO uint32_t EINT_CTL;                       /*!< Offset 0x210 External Interrupt Control Register */
        __IO uint32_t EINT_STATUS;                    /*!< Offset 0x214 External Interrupt Status Register */
        __IO uint32_t EINT_DEB;                       /*!< Offset 0x218 External Interrupt Debounce Register */
             RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    } GPIO_INTS [0x007];                              /*!< Offset 0x200 GPIO interrupt control */
         RESERVED(0x2E0[0x0340 - 0x02E0], uint8_t)
    __IO uint32_t PIO_POW_MOD_SEL;                    /*!< Offset 0x340 PIO Group Withstand Voltage Mode Select Register */
    __IO uint32_t PIO_POW_MS_CTL;                     /*!< Offset 0x344 PIO Group Withstand Voltage Mode Select Control Register */
    __IO uint32_t PIO_POW_VAL;                        /*!< Offset 0x348 PIO Group Power Value Register */
} GPIOBLOCK_TypeDef; /* size of structure = 0x34C */
/*
 * @brief GPIOINT
 */
/*!< GPIOINT  */
typedef struct GPIOINT_Type
{
    __IO uint32_t EINT_CFG [0x004];                   /*!< Offset 0x000 External Interrupt Configure Registers */
    __IO uint32_t EINT_CTL;                           /*!< Offset 0x010 External Interrupt Control Register */
    __IO uint32_t EINT_STATUS;                        /*!< Offset 0x014 External Interrupt Status Register */
    __IO uint32_t EINT_DEB;                           /*!< Offset 0x018 External Interrupt Debounce Register */
         RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
} GPIOINT_TypeDef; /* size of structure = 0x020 */
/*
 * @brief HDMI_PHY
 */
/*!< HDMI_PHY  */
typedef struct HDMI_PHY_Type
{
    __IO uint32_t DBG_CTRL;                           /*!< Offset 0x000 DBG_CTRL */
    __IO uint32_t REXT_CTRL;                          /*!< Offset 0x004 REXT_CTRL */
         RESERVED(0x008[0x0010 - 0x0008], uint8_t)
    __IO uint32_t READ_EN;                            /*!< Offset 0x010 READ_EN */
    __IO uint32_t UNSCRAMBLE;                         /*!< Offset 0x014 UNSCRAMBLE */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IO uint32_t ANA_CFG1;                           /*!< Offset 0x020 ANA_CFG1 */
    __IO uint32_t ANA_CFG2;                           /*!< Offset 0x024 ANA_CFG2 */
    __IO uint32_t ANA_CFG3;                           /*!< Offset 0x028 ANA_CFG3 */
    __IO uint32_t PLL_CFG1;                           /*!< Offset 0x02C ANA_CFG1 */
    __IO uint32_t PLL_CFG2;                           /*!< Offset 0x030 PLL_CFG2 */
    __IO uint32_t PLL_CFG3;                           /*!< Offset 0x034 PLL_CFG3 */
    __IO uint32_t ANA_STS;                            /*!< Offset 0x038 ANA_STS */
    __IO uint32_t CEC;                                /*!< Offset 0x03C CEC */
         RESERVED(0x040[0x0FF8 - 0x0040], uint8_t)
    __IO uint32_t CEC_VERSION;                        /*!< Offset 0xFF8 Controller Version Register(Default Value: 0x0100_0000) */
    __IO uint32_t VERSION;                            /*!< Offset 0xFFC PHY Version Register(Default Value: 0x0101_0000) */
} HDMI_PHY_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief HDMI_TX
 */
/*!< HDMI_TX  */
typedef struct HDMI_TX_Type
{
    __I  uint8_t  HDMI_DESIGN_ID;                     /*!< Offset 0x000  */
    __I  uint8_t  HDMI_REVISION_ID;                   /*!< Offset 0x001  */
    __I  uint8_t  HDMI_PRODUCT_ID0;                   /*!< Offset 0x002  */
    __I  uint8_t  HDMI_PRODUCT_ID1;                   /*!< Offset 0x003  */
    __I  uint8_t  HDMI_CONFIG0_ID;                    /*!< Offset 0x004  */
    __I  uint8_t  HDMI_CONFIG1_ID;                    /*!< Offset 0x005  */
    __I  uint8_t  HDMI_CONFIG2_ID;                    /*!< Offset 0x006  */
    __I  uint8_t  HDMI_CONFIG3_ID;                    /*!< Offset 0x007  */
         RESERVED(0x008[0x0100 - 0x0008], uint8_t)
    __IO uint8_t  HDMI_IH_FC_STAT0;                   /*!< Offset 0x100  */
    __IO uint8_t  HDMI_IH_FC_STAT1;                   /*!< Offset 0x101  */
    __IO uint8_t  HDMI_IH_FC_STAT2;                   /*!< Offset 0x102  */
    __IO uint8_t  HDMI_IH_AS_STAT0;                   /*!< Offset 0x103  */
    __IO uint8_t  HDMI_IH_PHY_STAT0;                  /*!< Offset 0x104  */
    __IO uint8_t  HDMI_IH_I2CM_STAT0;                 /*!< Offset 0x105  */
    __IO uint8_t  HDMI_IH_CEC_STAT0;                  /*!< Offset 0x106  */
    __IO uint8_t  HDMI_IH_VP_STAT0;                   /*!< Offset 0x107  */
    __IO uint8_t  HDMI_IH_I2CMPHY_STAT0;              /*!< Offset 0x108  */
    __IO uint8_t  HDMI_IH_AHBDMAAUD_STAT0;            /*!< Offset 0x109  */
         RESERVED(0x10A[0x0180 - 0x010A], uint8_t)
    __IO uint8_t  HDMI_IH_MUTE_FC_STAT0;              /*!< Offset 0x180  */
    __IO uint8_t  HDMI_IH_MUTE_FC_STAT1;              /*!< Offset 0x181  */
    __IO uint8_t  HDMI_IH_MUTE_FC_STAT2;              /*!< Offset 0x182  */
    __IO uint8_t  HDMI_IH_MUTE_AS_STAT0;              /*!< Offset 0x183  */
    __IO uint8_t  HDMI_IH_MUTE_PHY_STAT0;             /*!< Offset 0x184  */
    __IO uint8_t  HDMI_IH_MUTE_I2CM_STAT0;            /*!< Offset 0x185  */
    __IO uint8_t  HDMI_IH_MUTE_CEC_STAT0;             /*!< Offset 0x186  */
    __IO uint8_t  HDMI_IH_MUTE_VP_STAT0;              /*!< Offset 0x187  */
    __IO uint8_t  HDMI_IH_MUTE_I2CMPHY_STAT0;         /*!< Offset 0x188  */
    __IO uint8_t  HDMI_IH_MUTE_AHBDMAAUD_STAT0;       /*!< Offset 0x189  */
         RESERVED(0x18A[0x01FF - 0x018A], uint8_t)
    __IO uint8_t  HDMI_IH_MUTE;                       /*!< Offset 0x1FF  */
    __IO uint8_t  HDMI_TX_INVID0;                     /*!< Offset 0x200  */
    __IO uint8_t  HDMI_TX_INSTUFFING;                 /*!< Offset 0x201  */
    __IO uint8_t  HDMI_TX_GYDATA0;                    /*!< Offset 0x202  */
    __IO uint8_t  HDMI_TX_GYDATA1;                    /*!< Offset 0x203  */
    __IO uint8_t  HDMI_TX_RCRDATA0;                   /*!< Offset 0x204  */
    __IO uint8_t  HDMI_TX_RCRDATA1;                   /*!< Offset 0x205  */
    __IO uint8_t  HDMI_TX_BCBDATA0;                   /*!< Offset 0x206  */
    __IO uint8_t  HDMI_TX_BCBDATA1;                   /*!< Offset 0x207  */
         RESERVED(0x208[0x0800 - 0x0208], uint8_t)
    __IO uint8_t  HDMI_VP_STATUS;                     /*!< Offset 0x800  */
    __IO uint8_t  HDMI_VP_PR_CD;                      /*!< Offset 0x801  */
    __IO uint8_t  HDMI_VP_STUFF;                      /*!< Offset 0x802  */
    __IO uint8_t  HDMI_VP_REMAP;                      /*!< Offset 0x803  */
    __IO uint8_t  HDMI_VP_CONF;                       /*!< Offset 0x804  */
    __IO uint8_t  HDMI_VP_STAT;                       /*!< Offset 0x805  */
    __IO uint8_t  HDMI_VP_INT;                        /*!< Offset 0x806  */
    __IO uint8_t  HDMI_VP_MASK;                       /*!< Offset 0x807  */
    __IO uint8_t  HDMI_VP_POL;                        /*!< Offset 0x808  */
         RESERVED(0x809[0x1000 - 0x0809], uint8_t)
    __IO uint8_t  HDMI_FC_INVIDCONF;                  /*!< Offset 0x1000  */
    __IO uint8_t  HDMI_FC_INHACTV0;                   /*!< Offset 0x1001  */
    __IO uint8_t  HDMI_FC_INHACTV1;                   /*!< Offset 0x1002  */
    __IO uint8_t  HDMI_FC_INHBLANK0;                  /*!< Offset 0x1003  */
    __IO uint8_t  HDMI_FC_INHBLANK1;                  /*!< Offset 0x1004  */
    __IO uint8_t  HDMI_FC_INVACTV0;                   /*!< Offset 0x1005  */
    __IO uint8_t  HDMI_FC_INVACTV1;                   /*!< Offset 0x1006  */
    __IO uint8_t  HDMI_FC_INVBLANK;                   /*!< Offset 0x1007  */
    __IO uint8_t  HDMI_FC_HSYNCINDELAY0;              /*!< Offset 0x1008  */
    __IO uint8_t  HDMI_FC_HSYNCINDELAY1;              /*!< Offset 0x1009  */
    __IO uint8_t  HDMI_FC_HSYNCINWIDTH0;              /*!< Offset 0x100A  */
    __IO uint8_t  HDMI_FC_HSYNCINWIDTH1;              /*!< Offset 0x100B  */
    __IO uint8_t  HDMI_FC_VSYNCINDELAY;               /*!< Offset 0x100C  */
    __IO uint8_t  HDMI_FC_VSYNCINWIDTH;               /*!< Offset 0x100D  */
    __IO uint8_t  HDMI_FC_INFREQ0;                    /*!< Offset 0x100E  */
    __IO uint8_t  HDMI_FC_INFREQ1;                    /*!< Offset 0x100F  */
    __IO uint8_t  HDMI_FC_INFREQ2;                    /*!< Offset 0x1010  */
    __IO uint8_t  HDMI_FC_CTRLDUR;                    /*!< Offset 0x1011  */
    __IO uint8_t  HDMI_FC_EXCTRLDUR;                  /*!< Offset 0x1012  */
    __IO uint8_t  HDMI_FC_EXCTRLSPAC;                 /*!< Offset 0x1013  */
    __IO uint8_t  HDMI_FC_CH0PREAM;                   /*!< Offset 0x1014  */
    __IO uint8_t  HDMI_FC_CH1PREAM;                   /*!< Offset 0x1015  */
    __IO uint8_t  HDMI_FC_CH2PREAM;                   /*!< Offset 0x1016  */
    __IO uint8_t  HDMI_FC_AVICONF3;                   /*!< Offset 0x1017  */
    __IO uint8_t  HDMI_FC_GCP;                        /*!< Offset 0x1018  */
    __IO uint8_t  HDMI_FC_AVICONF0;                   /*!< Offset 0x1019  */
    __IO uint8_t  HDMI_FC_AVICONF1;                   /*!< Offset 0x101A  */
    __IO uint8_t  HDMI_FC_AVICONF2;                   /*!< Offset 0x101B  */
    __IO uint8_t  HDMI_FC_AVIVID;                     /*!< Offset 0x101C  */
    __IO uint8_t  HDMI_FC_AVIETB0;                    /*!< Offset 0x101D  */
    __IO uint8_t  HDMI_FC_AVIETB1;                    /*!< Offset 0x101E  */
    __IO uint8_t  HDMI_FC_AVISBB0;                    /*!< Offset 0x101F  */
    __IO uint8_t  HDMI_FC_AVISBB1;                    /*!< Offset 0x1020  */
    __IO uint8_t  HDMI_FC_AVIELB0;                    /*!< Offset 0x1021  */
    __IO uint8_t  HDMI_FC_AVIELB1;                    /*!< Offset 0x1022  */
    __IO uint8_t  HDMI_FC_AVISRB0;                    /*!< Offset 0x1023  */
    __IO uint8_t  HDMI_FC_AVISRB1;                    /*!< Offset 0x1024  */
    __IO uint8_t  HDMI_FC_AUDICONF0;                  /*!< Offset 0x1025  */
    __IO uint8_t  HDMI_FC_AUDICONF1;                  /*!< Offset 0x1026  */
    __IO uint8_t  HDMI_FC_AUDICONF2;                  /*!< Offset 0x1027  */
    __IO uint8_t  HDMI_FC_AUDICONF3;                  /*!< Offset 0x1028  */
    __IO uint8_t  HDMI_FC_VSDIEEEID0;                 /*!< Offset 0x1029  */
    __IO uint8_t  HDMI_FC_VSDSIZE;                    /*!< Offset 0x102A  */
         RESERVED(0x102B[0x1030 - 0x102B], uint8_t)
    __IO uint8_t  HDMI_FC_VSDIEEEID1;                 /*!< Offset 0x1030  */
    __IO uint8_t  HDMI_FC_VSDIEEEID2;                 /*!< Offset 0x1031  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD0;                /*!< Offset 0x1032  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD1;                /*!< Offset 0x1033  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD2;                /*!< Offset 0x1034  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD3;                /*!< Offset 0x1035  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD4;                /*!< Offset 0x1036  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD5;                /*!< Offset 0x1037  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD6;                /*!< Offset 0x1038  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD7;                /*!< Offset 0x1039  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD8;                /*!< Offset 0x103A  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD9;                /*!< Offset 0x103B  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD10;               /*!< Offset 0x103C  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD11;               /*!< Offset 0x103D  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD12;               /*!< Offset 0x103E  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD13;               /*!< Offset 0x103F  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD14;               /*!< Offset 0x1040  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD15;               /*!< Offset 0x1041  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD16;               /*!< Offset 0x1042  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD17;               /*!< Offset 0x1043  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD18;               /*!< Offset 0x1044  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD19;               /*!< Offset 0x1045  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD20;               /*!< Offset 0x1046  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD21;               /*!< Offset 0x1047  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD22;               /*!< Offset 0x1048  */
    __IO uint8_t  HDMI_FC_VSDPAYLOAD23;               /*!< Offset 0x1049  */
    __IO uint8_t  HDMI_FC_SPDVENDORNAME0;             /*!< Offset 0x104A  */
    __IO uint8_t  HDMI_FC_SPDVENDORNAME1;             /*!< Offset 0x104B  */
    __IO uint8_t  HDMI_FC_SPDVENDORNAME2;             /*!< Offset 0x104C  */
    __IO uint8_t  HDMI_FC_SPDVENDORNAME3;             /*!< Offset 0x104D  */
    __IO uint8_t  HDMI_FC_SPDVENDORNAME4;             /*!< Offset 0x104E  */
    __IO uint8_t  HDMI_FC_SPDVENDORNAME5;             /*!< Offset 0x104F  */
    __IO uint8_t  HDMI_FC_SPDVENDORNAME6;             /*!< Offset 0x1050  */
    __IO uint8_t  HDMI_FC_SPDVENDORNAME7;             /*!< Offset 0x1051  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME0;            /*!< Offset 0x1052  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME1;            /*!< Offset 0x1053  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME2;            /*!< Offset 0x1054  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME3;            /*!< Offset 0x1055  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME4;            /*!< Offset 0x1056  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME5;            /*!< Offset 0x1057  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME6;            /*!< Offset 0x1058  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME7;            /*!< Offset 0x1059  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME8;            /*!< Offset 0x105A  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME9;            /*!< Offset 0x105B  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME10;           /*!< Offset 0x105C  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME11;           /*!< Offset 0x105D  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME12;           /*!< Offset 0x105E  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME13;           /*!< Offset 0x105F  */
    __IO uint8_t  HDMI_FC_SDPPRODUCTNAME14;           /*!< Offset 0x1060  */
    __IO uint8_t  HDMI_FC_SPDPRODUCTNAME15;           /*!< Offset 0x1061  */
    __IO uint8_t  HDMI_FC_SPDDEVICEINF;               /*!< Offset 0x1062  */
    __IO uint8_t  HDMI_FC_AUDSCONF;                   /*!< Offset 0x1063  */
    __IO uint8_t  HDMI_FC_AUDSSTAT;                   /*!< Offset 0x1064  */
         RESERVED(0x1065[0x1070 - 0x1065], uint8_t)
    __IO uint8_t  HDMI_FC_DATACH0FILL;                /*!< Offset 0x1070  */
    __IO uint8_t  HDMI_FC_DATACH1FILL;                /*!< Offset 0x1071  */
    __IO uint8_t  HDMI_FC_DATACH2FILL;                /*!< Offset 0x1072  */
    __IO uint8_t  HDMI_FC_CTRLQHIGH;                  /*!< Offset 0x1073  */
    __IO uint8_t  HDMI_FC_CTRLQLOW;                   /*!< Offset 0x1074  */
    __IO uint8_t  HDMI_FC_ACP0;                       /*!< Offset 0x1075  */
    __IO uint8_t  HDMI_FC_ACP28;                      /*!< Offset 0x1076  */
    __IO uint8_t  HDMI_FC_ACP27;                      /*!< Offset 0x1077  */
    __IO uint8_t  HDMI_FC_ACP26;                      /*!< Offset 0x1078  */
    __IO uint8_t  HDMI_FC_ACP25;                      /*!< Offset 0x1079  */
    __IO uint8_t  HDMI_FC_ACP24;                      /*!< Offset 0x107A  */
    __IO uint8_t  HDMI_FC_ACP23;                      /*!< Offset 0x107B  */
    __IO uint8_t  HDMI_FC_ACP22;                      /*!< Offset 0x107C  */
    __IO uint8_t  HDMI_FC_ACP21;                      /*!< Offset 0x107D  */
    __IO uint8_t  HDMI_FC_ACP20;                      /*!< Offset 0x107E  */
    __IO uint8_t  HDMI_FC_ACP19;                      /*!< Offset 0x107F  */
    __IO uint8_t  HDMI_FC_ACP18;                      /*!< Offset 0x1080  */
    __IO uint8_t  HDMI_FC_ACP17;                      /*!< Offset 0x1081  */
    __IO uint8_t  HDMI_FC_ACP16;                      /*!< Offset 0x1082  */
    __IO uint8_t  HDMI_FC_ACP15;                      /*!< Offset 0x1083  */
    __IO uint8_t  HDMI_FC_ACP14;                      /*!< Offset 0x1084  */
    __IO uint8_t  HDMI_FC_ACP13;                      /*!< Offset 0x1085  */
    __IO uint8_t  HDMI_FC_ACP12;                      /*!< Offset 0x1086  */
    __IO uint8_t  HDMI_FC_ACP11;                      /*!< Offset 0x1087  */
    __IO uint8_t  HDMI_FC_ACP10;                      /*!< Offset 0x1088  */
    __IO uint8_t  HDMI_FC_ACP9;                       /*!< Offset 0x1089  */
    __IO uint8_t  HDMI_FC_ACP8;                       /*!< Offset 0x108A  */
    __IO uint8_t  HDMI_FC_ACP7;                       /*!< Offset 0x108B  */
    __IO uint8_t  HDMI_FC_ACP6;                       /*!< Offset 0x108C  */
    __IO uint8_t  HDMI_FC_ACP5;                       /*!< Offset 0x108D  */
    __IO uint8_t  HDMI_FC_ACP4;                       /*!< Offset 0x108E  */
    __IO uint8_t  HDMI_FC_ACP3;                       /*!< Offset 0x108F  */
    __IO uint8_t  HDMI_FC_ACP2;                       /*!< Offset 0x1090  */
    __IO uint8_t  HDMI_FC_ACP1;                       /*!< Offset 0x1091  */
    __IO uint8_t  HDMI_FC_ISCR1_0;                    /*!< Offset 0x1092  */
    __IO uint8_t  HDMI_FC_ISCR1_16;                   /*!< Offset 0x1093  */
    __IO uint8_t  HDMI_FC_ISCR1_15;                   /*!< Offset 0x1094  */
    __IO uint8_t  HDMI_FC_ISCR1_14;                   /*!< Offset 0x1095  */
    __IO uint8_t  HDMI_FC_ISCR1_13;                   /*!< Offset 0x1096  */
    __IO uint8_t  HDMI_FC_ISCR1_12;                   /*!< Offset 0x1097  */
    __IO uint8_t  HDMI_FC_ISCR1_11;                   /*!< Offset 0x1098  */
    __IO uint8_t  HDMI_FC_ISCR1_10;                   /*!< Offset 0x1099  */
    __IO uint8_t  HDMI_FC_ISCR1_9;                    /*!< Offset 0x109A  */
    __IO uint8_t  HDMI_FC_ISCR1_8;                    /*!< Offset 0x109B  */
    __IO uint8_t  HDMI_FC_ISCR1_7;                    /*!< Offset 0x109C  */
    __IO uint8_t  HDMI_FC_ISCR1_6;                    /*!< Offset 0x109D  */
    __IO uint8_t  HDMI_FC_ISCR1_5;                    /*!< Offset 0x109E  */
    __IO uint8_t  HDMI_FC_ISCR1_4;                    /*!< Offset 0x109F  */
    __IO uint8_t  HDMI_FC_ISCR1_3;                    /*!< Offset 0x10A0  */
    __IO uint8_t  HDMI_FC_ISCR1_2;                    /*!< Offset 0x10A1  */
    __IO uint8_t  HDMI_FC_ISCR1_1;                    /*!< Offset 0x10A2  */
    __IO uint8_t  HDMI_FC_ISCR2_15;                   /*!< Offset 0x10A3  */
    __IO uint8_t  HDMI_FC_ISCR2_14;                   /*!< Offset 0x10A4  */
    __IO uint8_t  HDMI_FC_ISCR2_13;                   /*!< Offset 0x10A5  */
    __IO uint8_t  HDMI_FC_ISCR2_12;                   /*!< Offset 0x10A6  */
    __IO uint8_t  HDMI_FC_ISCR2_11;                   /*!< Offset 0x10A7  */
    __IO uint8_t  HDMI_FC_ISCR2_10;                   /*!< Offset 0x10A8  */
    __IO uint8_t  HDMI_FC_ISCR2_9;                    /*!< Offset 0x10A9  */
    __IO uint8_t  HDMI_FC_ISCR2_8;                    /*!< Offset 0x10AA  */
    __IO uint8_t  HDMI_FC_ISCR2_7;                    /*!< Offset 0x10AB  */
    __IO uint8_t  HDMI_FC_ISCR2_6;                    /*!< Offset 0x10AC  */
    __IO uint8_t  HDMI_FC_ISCR2_5;                    /*!< Offset 0x10AD  */
    __IO uint8_t  HDMI_FC_ISCR2_4;                    /*!< Offset 0x10AE  */
    __IO uint8_t  HDMI_FC_ISCR2_3;                    /*!< Offset 0x10AF  */
    __IO uint8_t  HDMI_FC_ISCR2_2;                    /*!< Offset 0x10B0  */
    __IO uint8_t  HDMI_FC_ISCR2_1;                    /*!< Offset 0x10B1  */
    __IO uint8_t  HDMI_FC_ISCR2_0;                    /*!< Offset 0x10B2  */
    __IO uint8_t  HDMI_FC_DATAUTO0;                   /*!< Offset 0x10B3  */
    __IO uint8_t  HDMI_FC_DATAUTO1;                   /*!< Offset 0x10B4  */
    __IO uint8_t  HDMI_FC_DATAUTO2;                   /*!< Offset 0x10B5  */
    __IO uint8_t  HDMI_FC_DATMAN;                     /*!< Offset 0x10B6  */
    __IO uint8_t  HDMI_FC_DATAUTO3;                   /*!< Offset 0x10B7  */
    __IO uint8_t  HDMI_FC_RDRB0;                      /*!< Offset 0x10B8  */
    __IO uint8_t  HDMI_FC_RDRB1;                      /*!< Offset 0x10B9  */
    __IO uint8_t  HDMI_FC_RDRB2;                      /*!< Offset 0x10BA  */
    __IO uint8_t  HDMI_FC_RDRB3;                      /*!< Offset 0x10BB  */
    __IO uint8_t  HDMI_FC_RDRB4;                      /*!< Offset 0x10BC  */
    __IO uint8_t  HDMI_FC_RDRB5;                      /*!< Offset 0x10BD  */
    __IO uint8_t  HDMI_FC_RDRB6;                      /*!< Offset 0x10BE  */
    __IO uint8_t  HDMI_FC_RDRB7;                      /*!< Offset 0x10BF  */
         RESERVED(0x10C0[0x10D0 - 0x10C0], uint8_t)
    __IO uint8_t  HDMI_FC_STAT0;                      /*!< Offset 0x10D0  */
    __IO uint8_t  HDMI_FC_INT0;                       /*!< Offset 0x10D1  */
    __IO uint8_t  HDMI_FC_MASK0;                      /*!< Offset 0x10D2  */
    __IO uint8_t  HDMI_FC_POL0;                       /*!< Offset 0x10D3  */
    __IO uint8_t  HDMI_FC_STAT1;                      /*!< Offset 0x10D4  */
    __IO uint8_t  HDMI_FC_INT1;                       /*!< Offset 0x10D5  */
    __IO uint8_t  HDMI_FC_MASK1;                      /*!< Offset 0x10D6  */
    __IO uint8_t  HDMI_FC_POL1;                       /*!< Offset 0x10D7  */
    __IO uint8_t  HDMI_FC_STAT2;                      /*!< Offset 0x10D8  */
    __IO uint8_t  HDMI_FC_INT2;                       /*!< Offset 0x10D9  */
    __IO uint8_t  HDMI_FC_MASK2;                      /*!< Offset 0x10DA  */
    __IO uint8_t  HDMI_FC_POL2;                       /*!< Offset 0x10DB  */
         RESERVED(0x10DC[0x10E0 - 0x10DC], uint8_t)
    __IO uint8_t  HDMI_FC_PRCONF;                     /*!< Offset 0x10E0  */
         RESERVED(0x10E1[0x1100 - 0x10E1], uint8_t)
    __IO uint8_t  HDMI_FC_GMD_STAT;                   /*!< Offset 0x1100  */
    __IO uint8_t  HDMI_FC_GMD_EN;                     /*!< Offset 0x1101  */
    __IO uint8_t  HDMI_FC_GMD_UP;                     /*!< Offset 0x1102  */
    __IO uint8_t  HDMI_FC_GMD_CONF;                   /*!< Offset 0x1103  */
    __IO uint8_t  HDMI_FC_GMD_HB;                     /*!< Offset 0x1104  */
    __IO uint8_t  HDMI_FC_GMD_PB0;                    /*!< Offset 0x1105  */
    __IO uint8_t  HDMI_FC_GMD_PB1;                    /*!< Offset 0x1106  */
    __IO uint8_t  HDMI_FC_GMD_PB2;                    /*!< Offset 0x1107  */
    __IO uint8_t  HDMI_FC_GMD_PB3;                    /*!< Offset 0x1108  */
    __IO uint8_t  HDMI_FC_GMD_PB4;                    /*!< Offset 0x1109  */
    __IO uint8_t  HDMI_FC_GMD_PB5;                    /*!< Offset 0x110A  */
    __IO uint8_t  HDMI_FC_GMD_PB6;                    /*!< Offset 0x110B  */
    __IO uint8_t  HDMI_FC_GMD_PB7;                    /*!< Offset 0x110C  */
    __IO uint8_t  HDMI_FC_GMD_PB8;                    /*!< Offset 0x110D  */
    __IO uint8_t  HDMI_FC_GMD_PB9;                    /*!< Offset 0x110E  */
    __IO uint8_t  HDMI_FC_GMD_PB10;                   /*!< Offset 0x110F  */
    __IO uint8_t  HDMI_FC_GMD_PB11;                   /*!< Offset 0x1110  */
    __IO uint8_t  HDMI_FC_GMD_PB12;                   /*!< Offset 0x1111  */
    __IO uint8_t  HDMI_FC_GMD_PB13;                   /*!< Offset 0x1112  */
    __IO uint8_t  HDMI_FC_GMD_PB14;                   /*!< Offset 0x1113  */
    __IO uint8_t  HDMI_FC_GMD_PB15;                   /*!< Offset 0x1114  */
    __IO uint8_t  HDMI_FC_GMD_PB16;                   /*!< Offset 0x1115  */
    __IO uint8_t  HDMI_FC_GMD_PB17;                   /*!< Offset 0x1116  */
    __IO uint8_t  HDMI_FC_GMD_PB18;                   /*!< Offset 0x1117  */
    __IO uint8_t  HDMI_FC_GMD_PB19;                   /*!< Offset 0x1118  */
    __IO uint8_t  HDMI_FC_GMD_PB20;                   /*!< Offset 0x1119  */
    __IO uint8_t  HDMI_FC_GMD_PB21;                   /*!< Offset 0x111A  */
    __IO uint8_t  HDMI_FC_GMD_PB22;                   /*!< Offset 0x111B  */
    __IO uint8_t  HDMI_FC_GMD_PB23;                   /*!< Offset 0x111C  */
    __IO uint8_t  HDMI_FC_GMD_PB24;                   /*!< Offset 0x111D  */
    __IO uint8_t  HDMI_FC_GMD_PB25;                   /*!< Offset 0x111E  */
    __IO uint8_t  HDMI_FC_GMD_PB26;                   /*!< Offset 0x111F  */
    __IO uint8_t  HDMI_FC_GMD_PB27;                   /*!< Offset 0x1120  */
         RESERVED(0x1121[0x1200 - 0x1121], uint8_t)
    __IO uint8_t  HDMI_FC_DBGFORCE;                   /*!< Offset 0x1200  */
    __IO uint8_t  HDMI_FC_DBGAUD0CH0;                 /*!< Offset 0x1201  */
    __IO uint8_t  HDMI_FC_DBGAUD1CH0;                 /*!< Offset 0x1202  */
    __IO uint8_t  HDMI_FC_DBGAUD2CH0;                 /*!< Offset 0x1203  */
    __IO uint8_t  HDMI_FC_DBGAUD0CH1;                 /*!< Offset 0x1204  */
    __IO uint8_t  HDMI_FC_DBGAUD1CH1;                 /*!< Offset 0x1205  */
    __IO uint8_t  HDMI_FC_DBGAUD2CH1;                 /*!< Offset 0x1206  */
    __IO uint8_t  HDMI_FC_DBGAUD0CH2;                 /*!< Offset 0x1207  */
    __IO uint8_t  HDMI_FC_DBGAUD1CH2;                 /*!< Offset 0x1208  */
    __IO uint8_t  HDMI_FC_DBGAUD2CH2;                 /*!< Offset 0x1209  */
    __IO uint8_t  HDMI_FC_DBGAUD0CH3;                 /*!< Offset 0x120A  */
    __IO uint8_t  HDMI_FC_DBGAUD1CH3;                 /*!< Offset 0x120B  */
    __IO uint8_t  HDMI_FC_DBGAUD2CH3;                 /*!< Offset 0x120C  */
    __IO uint8_t  HDMI_FC_DBGAUD0CH4;                 /*!< Offset 0x120D  */
    __IO uint8_t  HDMI_FC_DBGAUD1CH4;                 /*!< Offset 0x120E  */
    __IO uint8_t  HDMI_FC_DBGAUD2CH4;                 /*!< Offset 0x120F  */
    __IO uint8_t  HDMI_FC_DBGAUD0CH5;                 /*!< Offset 0x1210  */
    __IO uint8_t  HDMI_FC_DBGAUD1CH5;                 /*!< Offset 0x1211  */
    __IO uint8_t  HDMI_FC_DBGAUD2CH5;                 /*!< Offset 0x1212  */
    __IO uint8_t  HDMI_FC_DBGAUD0CH6;                 /*!< Offset 0x1213  */
    __IO uint8_t  HDMI_FC_DBGAUD1CH6;                 /*!< Offset 0x1214  */
    __IO uint8_t  HDMI_FC_DBGAUD2CH6;                 /*!< Offset 0x1215  */
    __IO uint8_t  HDMI_FC_DBGAUD0CH7;                 /*!< Offset 0x1216  */
    __IO uint8_t  HDMI_FC_DBGAUD1CH7;                 /*!< Offset 0x1217  */
    __IO uint8_t  HDMI_FC_DBGAUD2CH7;                 /*!< Offset 0x1218  */
    __IO uint8_t  HDMI_FC_DBGTMDS0;                   /*!< Offset 0x1219  */
    __IO uint8_t  HDMI_FC_DBGTMDS1;                   /*!< Offset 0x121A  */
    __IO uint8_t  HDMI_FC_DBGTMDS2;                   /*!< Offset 0x121B  */
         RESERVED(0x121C[0x3000 - 0x121C], uint8_t)
    __IO uint8_t  HDMI_PHY_CONF0;                     /*!< Offset 0x3000  */
    __IO uint8_t  HDMI_PHY_TST0;                      /*!< Offset 0x3001  */
    __IO uint8_t  HDMI_PHY_TST1;                      /*!< Offset 0x3002  */
    __IO uint8_t  HDMI_PHY_TST2;                      /*!< Offset 0x3003  */
    __IO uint8_t  HDMI_PHY_STAT0;                     /*!< Offset 0x3004  */
    __IO uint8_t  HDMI_PHY_INT0;                      /*!< Offset 0x3005  */
    __IO uint8_t  HDMI_PHY_MASK0;                     /*!< Offset 0x3006  */
    __IO uint8_t  HDMI_PHY_POL0;                      /*!< Offset 0x3007  */
         RESERVED(0x3008[0x3020 - 0x3008], uint8_t)
    __IO uint8_t  HDMI_PHY_I2CM_SLAVE_ADDR;           /*!< Offset 0x3020  */
    __IO uint8_t  HDMI_PHY_I2CM_ADDRESS_ADDR;         /*!< Offset 0x3021  */
    __IO uint8_t  HDMI_PHY_I2CM_DATAO_1_ADDR;         /*!< Offset 0x3022  */
    __IO uint8_t  HDMI_PHY_I2CM_DATAO_0_ADDR;         /*!< Offset 0x3023  */
    __IO uint8_t  HDMI_PHY_I2CM_DATAI_1_ADDR;         /*!< Offset 0x3024  */
    __IO uint8_t  HDMI_PHY_I2CM_DATAI_0_ADDR;         /*!< Offset 0x3025  */
    __IO uint8_t  HDMI_PHY_I2CM_OPERATION_ADDR;       /*!< Offset 0x3026  */
    __IO uint8_t  HDMI_PHY_I2CM_INT_ADDR;             /*!< Offset 0x3027  */
    __IO uint8_t  HDMI_PHY_I2CM_CTLINT_ADDR;          /*!< Offset 0x3028  */
    __IO uint8_t  HDMI_PHY_I2CM_DIV_ADDR;             /*!< Offset 0x3029  */
    __IO uint8_t  HDMI_PHY_I2CM_SOFTRSTZ_ADDR;        /*!< Offset 0x302A  */
    __IO uint8_t  HDMI_PHY_I2CM_SS_SCL_HCNT_1_ADDR;   /*!< Offset 0x302B  */
    __IO uint8_t  HDMI_PHY_I2CM_SS_SCL_HCNT_0_ADDR;   /*!< Offset 0x302C  */
    __IO uint8_t  HDMI_PHY_I2CM_SS_SCL_LCNT_1_ADDR;   /*!< Offset 0x302D  */
    __IO uint8_t  HDMI_PHY_I2CM_SS_SCL_LCNT_0_ADDR;   /*!< Offset 0x302E  */
    __IO uint8_t  HDMI_PHY_I2CM_FS_SCL_HCNT_1_ADDR;   /*!< Offset 0x302F  */
    __IO uint8_t  HDMI_PHY_I2CM_FS_SCL_HCNT_0_ADDR;   /*!< Offset 0x3030  */
    __IO uint8_t  HDMI_PHY_I2CM_FS_SCL_LCNT_1_ADDR;   /*!< Offset 0x3031  */
    __IO uint8_t  HDMI_PHY_I2CM_FS_SCL_LCNT_0_ADDR;   /*!< Offset 0x3032  */
         RESERVED(0x3033[0x3100 - 0x3033], uint8_t)
    __IO uint8_t  HDMI_AUD_CONF0;                     /*!< Offset 0x3100  */
    __IO uint8_t  HDMI_AUD_CONF1;                     /*!< Offset 0x3101  */
    __IO uint8_t  HDMI_AUD_INT;                       /*!< Offset 0x3102  */
    __IO uint8_t  HDMI_AUD_CONF2;                     /*!< Offset 0x3103  */
         RESERVED(0x3104[0x3200 - 0x3104], uint8_t)
    __IO uint8_t  HDMI_AUD_N1;                        /*!< Offset 0x3200  */
    __IO uint8_t  HDMI_AUD_N2;                        /*!< Offset 0x3201  */
    __IO uint8_t  HDMI_AUD_N3;                        /*!< Offset 0x3202  */
    __IO uint8_t  HDMI_AUD_CTS1;                      /*!< Offset 0x3203  */
    __IO uint8_t  HDMI_AUD_CTS2;                      /*!< Offset 0x3204  */
    __IO uint8_t  HDMI_AUD_CTS3;                      /*!< Offset 0x3205  */
    __IO uint8_t  HDMI_AUD_INPUTCLKFS;                /*!< Offset 0x3206  */
         RESERVED(0x3207[0x3302 - 0x3207], uint8_t)
    __IO uint8_t  HDMI_AUD_SPDIFINT;                  /*!< Offset 0x3302  */
         RESERVED(0x3303[0x3400 - 0x3303], uint8_t)
    __IO uint8_t  HDMI_AUD_CONF0_HBR;                 /*!< Offset 0x3400  */
    __IO uint8_t  HDMI_AUD_HBR_STATUS;                /*!< Offset 0x3401  */
    __IO uint8_t  HDMI_AUD_HBR_INT;                   /*!< Offset 0x3402  */
    __IO uint8_t  HDMI_AUD_HBR_POL;                   /*!< Offset 0x3403  */
    __IO uint8_t  HDMI_AUD_HBR_MASK;                  /*!< Offset 0x3404  */
         RESERVED(0x3405[0x3500 - 0x3405], uint8_t)
    __IO uint8_t  HDMI_GP_CONF0;                      /*!< Offset 0x3500  */
    __IO uint8_t  HDMI_GP_CONF1;                      /*!< Offset 0x3501  */
    __IO uint8_t  HDMI_GP_CONF2;                      /*!< Offset 0x3502  */
    __IO uint8_t  HDMI_GP_STAT;                       /*!< Offset 0x3503  */
    __IO uint8_t  HDMI_GP_INT;                        /*!< Offset 0x3504  */
    __IO uint8_t  HDMI_GP_MASK;                       /*!< Offset 0x3505  */
    __IO uint8_t  HDMI_GP_POL;                        /*!< Offset 0x3506  */
         RESERVED(0x3507[0x3600 - 0x3507], uint8_t)
    __IO uint8_t  HDMI_AHB_DMA_CONF0;                 /*!< Offset 0x3600  */
    __IO uint8_t  HDMI_AHB_DMA_START;                 /*!< Offset 0x3601  */
    __IO uint8_t  HDMI_AHB_DMA_STOP;                  /*!< Offset 0x3602  */
    __IO uint8_t  HDMI_AHB_DMA_THRSLD;                /*!< Offset 0x3603  */
    __IO uint8_t  HDMI_AHB_DMA_STRADDR0;              /*!< Offset 0x3604  */
    __IO uint8_t  HDMI_AHB_DMA_STRADDR1;              /*!< Offset 0x3605  */
    __IO uint8_t  HDMI_AHB_DMA_STRADDR2;              /*!< Offset 0x3606  */
    __IO uint8_t  HDMI_AHB_DMA_STRADDR3;              /*!< Offset 0x3607  */
    __IO uint8_t  HDMI_AHB_DMA_STPADDR0;              /*!< Offset 0x3608  */
    __IO uint8_t  HDMI_AHB_DMA_STPADDR1;              /*!< Offset 0x3609  */
    __IO uint8_t  HDMI_AHB_DMA_STPADDR2;              /*!< Offset 0x360A  */
    __IO uint8_t  HDMI_AHB_DMA_STPADDR3;              /*!< Offset 0x360B  */
    __IO uint8_t  HDMI_AHB_DMA_BSTADDR0;              /*!< Offset 0x360C  */
    __IO uint8_t  HDMI_AHB_DMA_BSTADDR1;              /*!< Offset 0x360D  */
    __IO uint8_t  HDMI_AHB_DMA_BSTADDR2;              /*!< Offset 0x360E  */
    __IO uint8_t  HDMI_AHB_DMA_BSTADDR3;              /*!< Offset 0x360F  */
    __IO uint8_t  HDMI_AHB_DMA_MBLENGTH0;             /*!< Offset 0x3610  */
    __IO uint8_t  HDMI_AHB_DMA_MBLENGTH1;             /*!< Offset 0x3611  */
    __IO uint8_t  HDMI_AHB_DMA_STAT;                  /*!< Offset 0x3612  */
    __IO uint8_t  HDMI_AHB_DMA_INT;                   /*!< Offset 0x3613  */
    __IO uint8_t  HDMI_AHB_DMA_MASK;                  /*!< Offset 0x3614  */
    __IO uint8_t  HDMI_AHB_DMA_POL;                   /*!< Offset 0x3615  */
    __IO uint8_t  HDMI_AHB_DMA_CONF1;                 /*!< Offset 0x3616  */
    __IO uint8_t  HDMI_AHB_DMA_BUFFSTAT;              /*!< Offset 0x3617  */
    __IO uint8_t  HDMI_AHB_DMA_BUFFINT;               /*!< Offset 0x3618  */
    __IO uint8_t  HDMI_AHB_DMA_BUFFMASK;              /*!< Offset 0x3619  */
    __IO uint8_t  HDMI_AHB_DMA_BUFFPOL;               /*!< Offset 0x361A  */
         RESERVED(0x361B[0x4000 - 0x361B], uint8_t)
    __IO uint8_t  HDMI_MC_SFRDIV;                     /*!< Offset 0x4000  */
    __IO uint8_t  HDMI_MC_CLKDIS;                     /*!< Offset 0x4001  */
    __IO uint8_t  HDMI_MC_SWRSTZ;                     /*!< Offset 0x4002  */
    __IO uint8_t  HDMI_MC_OPCTRL;                     /*!< Offset 0x4003  */
    __IO uint8_t  HDMI_MC_FLOWCTRL;                   /*!< Offset 0x4004  */
    __IO uint8_t  HDMI_MC_PHYRSTZ;                    /*!< Offset 0x4005  */
    __IO uint8_t  HDMI_MC_LOCKONCLOCK;                /*!< Offset 0x4006  */
    __IO uint8_t  HDMI_MC_HEACPHY_RST;                /*!< Offset 0x4007  */
         RESERVED(0x4008[0x4100 - 0x4008], uint8_t)
    __IO uint8_t  HDMI_CSC_CFG;                       /*!< Offset 0x4100  */
    __IO uint8_t  HDMI_CSC_SCALE;                     /*!< Offset 0x4101  */
    __IO uint8_t  HDMI_CSC_COEF_A1_MSB;               /*!< Offset 0x4102  */
    __IO uint8_t  HDMI_CSC_COEF_A1_LSB;               /*!< Offset 0x4103  */
    __IO uint8_t  HDMI_CSC_COEF_A2_MSB;               /*!< Offset 0x4104  */
    __IO uint8_t  HDMI_CSC_COEF_A2_LSB;               /*!< Offset 0x4105  */
    __IO uint8_t  HDMI_CSC_COEF_A3_MSB;               /*!< Offset 0x4106  */
    __IO uint8_t  HDMI_CSC_COEF_A3_LSB;               /*!< Offset 0x4107  */
    __IO uint8_t  HDMI_CSC_COEF_A4_MSB;               /*!< Offset 0x4108  */
    __IO uint8_t  HDMI_CSC_COEF_A4_LSB;               /*!< Offset 0x4109  */
    __IO uint8_t  HDMI_CSC_COEF_B1_MSB;               /*!< Offset 0x410A  */
    __IO uint8_t  HDMI_CSC_COEF_B1_LSB;               /*!< Offset 0x410B  */
    __IO uint8_t  HDMI_CSC_COEF_B2_MSB;               /*!< Offset 0x410C  */
    __IO uint8_t  HDMI_CSC_COEF_B2_LSB;               /*!< Offset 0x410D  */
    __IO uint8_t  HDMI_CSC_COEF_B3_MSB;               /*!< Offset 0x410E  */
    __IO uint8_t  HDMI_CSC_COEF_B3_LSB;               /*!< Offset 0x410F  */
    __IO uint8_t  HDMI_CSC_COEF_B4_MSB;               /*!< Offset 0x4110  */
    __IO uint8_t  HDMI_CSC_COEF_B4_LSB;               /*!< Offset 0x4111  */
    __IO uint8_t  HDMI_CSC_COEF_C1_MSB;               /*!< Offset 0x4112  */
    __IO uint8_t  HDMI_CSC_COEF_C1_LSB;               /*!< Offset 0x4113  */
    __IO uint8_t  HDMI_CSC_COEF_C2_MSB;               /*!< Offset 0x4114  */
    __IO uint8_t  HDMI_CSC_COEF_C2_LSB;               /*!< Offset 0x4115  */
    __IO uint8_t  HDMI_CSC_COEF_C3_MSB;               /*!< Offset 0x4116  */
    __IO uint8_t  HDMI_CSC_COEF_C3_LSB;               /*!< Offset 0x4117  */
    __IO uint8_t  HDMI_CSC_COEF_C4_MSB;               /*!< Offset 0x4118  */
    __IO uint8_t  HDMI_CSC_COEF_C4_LSB;               /*!< Offset 0x4119  */
         RESERVED(0x411A[0x5000 - 0x411A], uint8_t)
    __IO uint8_t  HDMI_A_HDCPCFG0;                    /*!< Offset 0x5000  */
    __IO uint8_t  HDMI_A_HDCPCFG1;                    /*!< Offset 0x5001  */
    __IO uint8_t  HDMI_A_HDCPOBS0;                    /*!< Offset 0x5002  */
    __IO uint8_t  HDMI_A_HDCPOBS1;                    /*!< Offset 0x5003  */
    __IO uint8_t  HDMI_A_HDCPOBS2;                    /*!< Offset 0x5004  */
    __IO uint8_t  HDMI_A_HDCPOBS3;                    /*!< Offset 0x5005  */
    __IO uint8_t  HDMI_A_APIINTCLR;                   /*!< Offset 0x5006  */
    __IO uint8_t  HDMI_A_APIINTSTAT;                  /*!< Offset 0x5007  */
    __IO uint8_t  HDMI_A_APIINTMSK;                   /*!< Offset 0x5008  */
    __IO uint8_t  HDMI_A_VIDPOLCFG;                   /*!< Offset 0x5009  */
    __IO uint8_t  HDMI_A_OESSWCFG;                    /*!< Offset 0x500A  */
    __IO uint8_t  HDMI_A_TIMER1SETUP0;                /*!< Offset 0x500B  */
    __IO uint8_t  HDMI_A_TIMER1SETUP1;                /*!< Offset 0x500C  */
    __IO uint8_t  HDMI_A_TIMER2SETUP0;                /*!< Offset 0x500D  */
    __IO uint8_t  HDMI_A_TIMER2SETUP1;                /*!< Offset 0x500E  */
    __IO uint8_t  HDMI_A_100MSCFG;                    /*!< Offset 0x500F  */
    __IO uint8_t  HDMI_A_2SCFG0;                      /*!< Offset 0x5010  */
    __IO uint8_t  HDMI_A_2SCFG1;                      /*!< Offset 0x5011  */
    __IO uint8_t  HDMI_A_5SCFG0;                      /*!< Offset 0x5012  */
    __IO uint8_t  HDMI_A_5SCFG1;                      /*!< Offset 0x5013  */
    __IO uint8_t  HDMI_A_SRMVERLSB;                   /*!< Offset 0x5014  */
    __IO uint8_t  HDMI_A_SRMVERMSB;                   /*!< Offset 0x5015  */
    __IO uint8_t  HDMI_A_SRMCTRL;                     /*!< Offset 0x5016  */
    __IO uint8_t  HDMI_A_SFRSETUP;                    /*!< Offset 0x5017  */
    __IO uint8_t  HDMI_A_I2CHSETUP;                   /*!< Offset 0x5018  */
    __IO uint8_t  HDMI_A_INTSETUP;                    /*!< Offset 0x5019  */
    __IO uint8_t  HDMI_A_PRESETUP;                    /*!< Offset 0x501A  */
         RESERVED(0x501B[0x5020 - 0x501B], uint8_t)
    __IO uint8_t  HDMI_A_SRM_BASE;                    /*!< Offset 0x5020  */
         RESERVED(0x5021[0x7D00 - 0x5021], uint8_t)
    __IO uint8_t  HDMI_CEC_CTRL;                      /*!< Offset 0x7D00  */
    __IO uint8_t  HDMI_CEC_STAT;                      /*!< Offset 0x7D01  */
    __IO uint8_t  HDMI_CEC_MASK;                      /*!< Offset 0x7D02  */
    __IO uint8_t  HDMI_CEC_POLARITY;                  /*!< Offset 0x7D03  */
    __IO uint8_t  HDMI_CEC_INT;                       /*!< Offset 0x7D04  */
    __IO uint8_t  HDMI_CEC_ADDR_L;                    /*!< Offset 0x7D05  */
    __IO uint8_t  HDMI_CEC_ADDR_H;                    /*!< Offset 0x7D06  */
    __IO uint8_t  HDMI_CEC_TX_CNT;                    /*!< Offset 0x7D07  */
    __IO uint8_t  HDMI_CEC_RX_CNT;                    /*!< Offset 0x7D08  */
         RESERVED(0x7D09[0x7D10 - 0x7D09], uint8_t)
    __IO uint8_t  HDMI_CEC_TX_DATA0;                  /*!< Offset 0x7D10  */
    __IO uint8_t  HDMI_CEC_TX_DATA1;                  /*!< Offset 0x7D11  */
    __IO uint8_t  HDMI_CEC_TX_DATA2;                  /*!< Offset 0x7D12  */
    __IO uint8_t  HDMI_CEC_TX_DATA3;                  /*!< Offset 0x7D13  */
    __IO uint8_t  HDMI_CEC_TX_DATA4;                  /*!< Offset 0x7D14  */
    __IO uint8_t  HDMI_CEC_TX_DATA5;                  /*!< Offset 0x7D15  */
    __IO uint8_t  HDMI_CEC_TX_DATA6;                  /*!< Offset 0x7D16  */
    __IO uint8_t  HDMI_CEC_TX_DATA7;                  /*!< Offset 0x7D17  */
    __IO uint8_t  HDMI_CEC_TX_DATA8;                  /*!< Offset 0x7D18  */
    __IO uint8_t  HDMI_CEC_TX_DATA9;                  /*!< Offset 0x7D19  */
    __IO uint8_t  HDMI_CEC_TX_DATA10;                 /*!< Offset 0x7D1A  */
    __IO uint8_t  HDMI_CEC_TX_DATA11;                 /*!< Offset 0x7D1B  */
    __IO uint8_t  HDMI_CEC_TX_DATA12;                 /*!< Offset 0x7D1C  */
    __IO uint8_t  HDMI_CEC_TX_DATA13;                 /*!< Offset 0x7D1D  */
    __IO uint8_t  HDMI_CEC_TX_DATA14;                 /*!< Offset 0x7D1E  */
    __IO uint8_t  HDMI_CEC_TX_DATA15;                 /*!< Offset 0x7D1F  */
    __IO uint8_t  HDMI_CEC_RX_DATA0;                  /*!< Offset 0x7D20  */
    __IO uint8_t  HDMI_CEC_RX_DATA1;                  /*!< Offset 0x7D21  */
    __IO uint8_t  HDMI_CEC_RX_DATA2;                  /*!< Offset 0x7D22  */
    __IO uint8_t  HDMI_CEC_RX_DATA3;                  /*!< Offset 0x7D23  */
    __IO uint8_t  HDMI_CEC_RX_DATA4;                  /*!< Offset 0x7D24  */
    __IO uint8_t  HDMI_CEC_RX_DATA5;                  /*!< Offset 0x7D25  */
    __IO uint8_t  HDMI_CEC_RX_DATA6;                  /*!< Offset 0x7D26  */
    __IO uint8_t  HDMI_CEC_RX_DATA7;                  /*!< Offset 0x7D27  */
    __IO uint8_t  HDMI_CEC_RX_DATA8;                  /*!< Offset 0x7D28  */
    __IO uint8_t  HDMI_CEC_RX_DATA9;                  /*!< Offset 0x7D29  */
    __IO uint8_t  HDMI_CEC_RX_DATA10;                 /*!< Offset 0x7D2A  */
    __IO uint8_t  HDMI_CEC_RX_DATA11;                 /*!< Offset 0x7D2B  */
    __IO uint8_t  HDMI_CEC_RX_DATA12;                 /*!< Offset 0x7D2C  */
    __IO uint8_t  HDMI_CEC_RX_DATA13;                 /*!< Offset 0x7D2D  */
    __IO uint8_t  HDMI_CEC_RX_DATA14;                 /*!< Offset 0x7D2E  */
    __IO uint8_t  HDMI_CEC_RX_DATA15;                 /*!< Offset 0x7D2F  */
    __IO uint8_t  HDMI_CEC_LOCK;                      /*!< Offset 0x7D30  */
    __IO uint8_t  HDMI_CEC_WKUPCTRL;                  /*!< Offset 0x7D31  */
         RESERVED(0x7D32[0x7E00 - 0x7D32], uint8_t)
    __IO uint8_t  HDMI_I2CM_SLAVE;                    /*!< Offset 0x7E00  */
    __IO uint8_t  HDMI_I2CM_ADDRESS;                  /*!< Offset 0x7E01  */
    __IO uint8_t  HDMI_I2CM_DATAO;                    /*!< Offset 0x7E02  */
    __IO uint8_t  HDMI_I2CM_DATAI;                    /*!< Offset 0x7E03  */
    __IO uint8_t  HDMI_I2CM_OPERATION;                /*!< Offset 0x7E04  */
    __IO uint8_t  HDMI_I2CM_INT;                      /*!< Offset 0x7E05  */
    __IO uint8_t  HDMI_I2CM_CTLINT;                   /*!< Offset 0x7E06  */
    __IO uint8_t  HDMI_I2CM_DIV;                      /*!< Offset 0x7E07  */
    __IO uint8_t  HDMI_I2CM_SEGADDR;                  /*!< Offset 0x7E08  */
    __IO uint8_t  HDMI_I2CM_SOFTRSTZ;                 /*!< Offset 0x7E09  */
    __IO uint8_t  HDMI_I2CM_SEGPTR;                   /*!< Offset 0x7E0A  */
    __IO uint8_t  HDMI_I2CM_SS_SCL_HCNT_1_ADDR;       /*!< Offset 0x7E0B  */
    __IO uint8_t  HDMI_I2CM_SS_SCL_HCNT_0_ADDR;       /*!< Offset 0x7E0C  */
    __IO uint8_t  HDMI_I2CM_SS_SCL_LCNT_1_ADDR;       /*!< Offset 0x7E0D  */
    __IO uint8_t  HDMI_I2CM_SS_SCL_LCNT_0_ADDR;       /*!< Offset 0x7E0E  */
    __IO uint8_t  HDMI_I2CM_FS_SCL_HCNT_1_ADDR;       /*!< Offset 0x7E0F  */
    __IO uint8_t  HDMI_I2CM_FS_SCL_HCNT_0_ADDR;       /*!< Offset 0x7E10  */
    __IO uint8_t  HDMI_I2CM_FS_SCL_LCNT_1_ADDR;       /*!< Offset 0x7E11  */
    __IO uint8_t  HDMI_I2CM_FS_SCL_LCNT_0_ADDR;       /*!< Offset 0x7E12  */
} HDMI_TX_TypeDef; /* size of structure = 0x7E13 */
/*
 * @brief HSTIMER
 */
/*!< HSTIMER High Speed Timer (HSTimer) */
typedef struct HSTIMER_Type
{
    __IO uint32_t HS_TMR_IRQ_EN_REG;                  /*!< Offset 0x000 HS Timer IRQ Enable Register */
    __IO uint32_t HS_TMR_IRQ_STAS_REG;                /*!< Offset 0x004 HS Timer Status Register */
         RESERVED(0x008[0x0020 - 0x0008], uint8_t)
    __IO uint32_t HS_TMR0_CTRL_REG;                   /*!< Offset 0x020 HS Timer0 Control Register */
    __IO uint32_t HS_TMR0_INTV_LO_REG;                /*!< Offset 0x024 HS Timer0 Interval Value Low Register */
    __IO uint32_t HS_TMR0_INTV_HI_REG;                /*!< Offset 0x028 HS Timer0 Interval Value High Register */
    __IO uint32_t HS_TMR0_CURNT_LO_REG;               /*!< Offset 0x02C HS Timer0 Current Value Low Register */
    __IO uint32_t HS_TMR0_CURNT_HI_REG;               /*!< Offset 0x030 HS Timer0 Current Value High Register */
         RESERVED(0x034[0x0040 - 0x0034], uint8_t)
    __IO uint32_t HS_TMR1_CTRL_REG;                   /*!< Offset 0x040 HS Timer1 Control Register */
    __IO uint32_t HS_TMR1_INTV_LO_REG;                /*!< Offset 0x044 HS Timer1 Interval Value Low Register */
    __IO uint32_t HS_TMR1_INTV_HI_REG;                /*!< Offset 0x048 HS Timer1 Interval Value High Register */
    __IO uint32_t HS_TMR1_CURNT_LO_REG;               /*!< Offset 0x04C HS Timer1 Current Value Low Register */
    __IO uint32_t HS_TMR1_CURNT_HI_REG;               /*!< Offset 0x050 HS Timer1 Current Value High Register */
         RESERVED(0x054[0x1000 - 0x0054], uint8_t)
} HSTIMER_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief I2S_PCM
 */
/*!< I2S_PCM  */
typedef struct I2S_PCM_Type
{
    __IO uint32_t I2S_PCM_CTL;                        /*!< Offset 0x000 I2S/PCM Control Register */
    __IO uint32_t I2S_PCM_FMT0;                       /*!< Offset 0x004 I2S/PCM Format Register 0 */
    __IO uint32_t I2S_PCM_FMT1;                       /*!< Offset 0x008 I2S/PCM Format Register 1 */
    __IO uint32_t I2S_PCM_ISTA;                       /*!< Offset 0x00C I2S/PCM Interrupt Status Register */
    __IO uint32_t I2S_PCM_RXFIFO;                     /*!< Offset 0x010 I2S/PCM RXFIFO Register */
    __IO uint32_t I2S_PCM_FCTL;                       /*!< Offset 0x014 I2S/PCM FIFO Control Register */
    __IO uint32_t I2S_PCM_FSTA;                       /*!< Offset 0x018 I2S/PCM FIFO Status Register */
    __IO uint32_t I2S_PCM_INT;                        /*!< Offset 0x01C I2S/PCM DMA & Interrupt Control Register */
    __IO uint32_t I2S_PCM_TXFIFO;                     /*!< Offset 0x020 I2S/PCM TXFIFO Register */
    __IO uint32_t I2S_PCM_CLKD;                       /*!< Offset 0x024 I2S/PCM Clock Divide Register */
    __IO uint32_t I2S_PCM_TXCNT;                      /*!< Offset 0x028 I2S/PCM TX Sample Counter Register */
    __IO uint32_t I2S_PCM_RXCNT;                      /*!< Offset 0x02C I2S/PCM RX Sample Counter Register */
    __IO uint32_t I2S_PCM_CHCFG;                      /*!< Offset 0x030 I2S/PCM Channel Configuration Register */
    __IO uint32_t I2S_PCM_TX0CHSEL;                   /*!< Offset 0x034 I2S/PCM TX0 Channel Select Register */
    __IO uint32_t I2S_PCM_TX1CHSEL;                   /*!< Offset 0x038 I2S/PCM TX1 Channel Select Register */
    __IO uint32_t I2S_PCM_TX2CHSEL;                   /*!< Offset 0x03C I2S/PCM TX2 Channel Select Register */
    __IO uint32_t I2S_PCM_TX3CHSEL;                   /*!< Offset 0x040 I2S/PCM TX3 Channel Select Register */
    __IO uint32_t I2S_PCM_TX0CHMAP0;                  /*!< Offset 0x044 I2S/PCM TX0 Channel Mapping Register0 */
    __IO uint32_t I2S_PCM_TX0CHMAP1;                  /*!< Offset 0x048 I2S/PCM TX0 Channel Mapping Register1 */
    __IO uint32_t I2S_PCM_TX1CHMAP0;                  /*!< Offset 0x04C I2S/PCM TX1 Channel Mapping Register0 */
    __IO uint32_t I2S_PCM_TX1CHMAP1;                  /*!< Offset 0x050 I2S/PCM TX1 Channel Mapping Register1 */
    __IO uint32_t I2S_PCM_TX2CHMAP0;                  /*!< Offset 0x054 I2S/PCM TX2 Channel Mapping Register0 */
    __IO uint32_t I2S_PCM_TX2CHMAP1;                  /*!< Offset 0x058 I2S/PCM TX2 Channel Mapping Register1 */
    __IO uint32_t I2S_PCM_TX3CHMAP0;                  /*!< Offset 0x05C I2S/PCM TX3 Channel Mapping Register0 */
    __IO uint32_t I2S_PCM_TX3CHMAP1;                  /*!< Offset 0x060 I2S/PCM TX3 Channel Mapping Register1 */
    __IO uint32_t I2S_PCM_RXCHSEL;                    /*!< Offset 0x064 I2S/PCM RX Channel Select Register */
    __IO uint32_t I2S_PCM_RXCHMAP [0x004];            /*!< Offset 0x068 I2S/PCM RX Channel Mapping Register0..3 */
         RESERVED(0x078[0x0080 - 0x0078], uint8_t)
    __IO uint32_t MCLKCFG;                            /*!< Offset 0x080 ASRC MCLK Configuration Register */
    __IO uint32_t FsoutCFG;                           /*!< Offset 0x084 ASRC Out Sample Rate Configuration Register */
    __IO uint32_t FsinEXTCFG;                         /*!< Offset 0x088 ASRC Input Sample Pulse Extend Configuration Register */
    __IO uint32_t ASRCCFG;                            /*!< Offset 0x08C ASRC Enable Register */
    __IO uint32_t ASRCMANCFG;                         /*!< Offset 0x090 ASRC Manual Ratio Configuration Register */
    __IO uint32_t ASRCRATIOSTAT;                      /*!< Offset 0x094 ASRC Status Register */
    __IO uint32_t ASRCFIFOSTAT;                       /*!< Offset 0x098 ASRC FIFO Level Status Register */
    __IO uint32_t ASRCMBISTCFG;                       /*!< Offset 0x09C ASRC MBIST Test Configuration Register */
    __IO uint32_t ASRCMBISTSTAT;                      /*!< Offset 0x0A0 ASRC MBIST Test Status Register */
} I2S_PCM_TypeDef; /* size of structure = 0x0A4 */
/*
 * @brief IOMMU
 */
/*!< IOMMU  */
typedef struct IOMMU_Type
{
         RESERVED(0x000[0x0010 - 0x0000], uint8_t)
    __IO uint32_t IOMMU_RESET_REG;                    /*!< Offset 0x010 IOMMU Reset Register */
         RESERVED(0x014[0x0020 - 0x0014], uint8_t)
    __IO uint32_t IOMMU_ENABLE_REG;                   /*!< Offset 0x020 IOMMU Enable Register */
         RESERVED(0x024[0x0030 - 0x0024], uint8_t)
    __IO uint32_t IOMMU_BYPASS_REG;                   /*!< Offset 0x030 IOMMU Bypass Register */
         RESERVED(0x034[0x0040 - 0x0034], uint8_t)
    __IO uint32_t IOMMU_AUTO_GATING_REG;              /*!< Offset 0x040 IOMMU Auto Gating Register */
    __IO uint32_t IOMMU_WBUF_CTRL_REG;                /*!< Offset 0x044 IOMMU Write Buffer Control Register */
    __IO uint32_t IOMMU_OOO_CTRL_REG;                 /*!< Offset 0x048 IOMMU Out of Order Control Register */
    __IO uint32_t IOMMU_4KB_BDY_PRT_CTRL_REG;         /*!< Offset 0x04C IOMMU 4KB Boundary Protect Control Register */
    __IO uint32_t IOMMU_TTB_REG;                      /*!< Offset 0x050 IOMMU Translation Table Base Register */
         RESERVED(0x054[0x0060 - 0x0054], uint8_t)
    __IO uint32_t IOMMU_TLB_ENABLE_REG;               /*!< Offset 0x060 IOMMU TLB Enable Register */
         RESERVED(0x064[0x0070 - 0x0064], uint8_t)
    __IO uint32_t IOMMU_TLB_PREFETCH_REG;             /*!< Offset 0x070 IOMMU TLB Prefetch Register */
         RESERVED(0x074[0x0080 - 0x0074], uint8_t)
    __IO uint32_t IOMMU_TLB_FLUSH_ENABLE_REG;         /*!< Offset 0x080 IOMMU TLB Flush Enable Register */
    __IO uint32_t IOMMU_TLB_IVLD_MODE_SEL_REG;        /*!< Offset 0x084 IOMMU TLB Invalidation Mode Select Register */
    __IO uint32_t IOMMU_TLB_IVLD_STA_ADDR_REG;        /*!< Offset 0x088 IOMMU TLB Invalidation Start Address Register */
    __IO uint32_t IOMMU_TLB_IVLD_END_ADDR_REG;        /*!< Offset 0x08C IOMMU TLB Invalidation End Address Register */
    __IO uint32_t IOMMU_TLB_IVLD_ADDR_REG;            /*!< Offset 0x090 IOMMU TLB Invalidation Address Register */
    __IO uint32_t IOMMU_TLB_IVLD_ADDR_MASK_REG;       /*!< Offset 0x094 IOMMU TLB Invalidation Address Mask Register */
    __IO uint32_t IOMMU_TLB_IVLD_ENABLE_REG;          /*!< Offset 0x098 IOMMU TLB Invalidation Enable Register */
    __IO uint32_t IOMMU_PC_IVLD_MODE_SEL_REG;         /*!< Offset 0x09C IOMMU PC Invalidation Mode Select Register */
    __IO uint32_t IOMMU_PC_IVLD_ADDR_REG;             /*!< Offset 0x0A0 IOMMU PC Invalidation Address Register */
    __IO uint32_t IOMMU_PC_IVLD_STA_ADDR_REG;         /*!< Offset 0x0A4 IOMMU PC Invalidation Start Address Register */
    __IO uint32_t IOMMU_PC_IVLD_ENABLE_REG;           /*!< Offset 0x0A8 IOMMU PC Invalidation Enable Register */
    __IO uint32_t IOMMU_PC_IVLD_END_ADDR_REG;         /*!< Offset 0x0AC IOMMU PC Invalidation End Address Register */
    __IO uint32_t IOMMU_DM_AUT_CTRL0_REG;             /*!< Offset 0x0B0 IOMMU Domain Authority Control 0 Register */
    __IO uint32_t IOMMU_DM_AUT_CTRL1_REG;             /*!< Offset 0x0B4 IOMMU Domain Authority Control 1 Register */
    __IO uint32_t IOMMU_DM_AUT_CTRL2_REG;             /*!< Offset 0x0B8 IOMMU Domain Authority Control 2 Register */
    __IO uint32_t IOMMU_DM_AUT_CTRL3_REG;             /*!< Offset 0x0BC IOMMU Domain Authority Control 3 Register */
    __IO uint32_t IOMMU_DM_AUT_CTRL4_REG;             /*!< Offset 0x0C0 IOMMU Domain Authority Control 4 Register */
    __IO uint32_t IOMMU_DM_AUT_CTRL5_REG;             /*!< Offset 0x0C4 IOMMU Domain Authority Control 5 Register */
    __IO uint32_t IOMMU_DM_AUT_CTRL6_REG;             /*!< Offset 0x0C8 IOMMU Domain Authority Control 6 Register */
    __IO uint32_t IOMMU_DM_AUT_CTRL7_REG;             /*!< Offset 0x0CC IOMMU Domain Authority Control 7 Register */
    __IO uint32_t IOMMU_DM_AUT_OVWT_REG;              /*!< Offset 0x0D0 IOMMU Domain Authority Overwrite Register */
         RESERVED(0x0D4[0x0100 - 0x00D4], uint8_t)
    __IO uint32_t IOMMU_INT_ENABLE_REG;               /*!< Offset 0x100 IOMMU Interrupt Enable Register */
    __IO uint32_t IOMMU_INT_CLR_REG;                  /*!< Offset 0x104 IOMMU Interrupt Clear Register */
    __IO uint32_t IOMMU_INT_STA_REG;                  /*!< Offset 0x108 IOMMU Interrupt Status Register */
         RESERVED(0x10C[0x0110 - 0x010C], uint8_t)
    __IO uint32_t IOMMU_INT_ERR_ADDR0_REG;            /*!< Offset 0x110 IOMMU Interrupt Error Address 0 Register */
    __IO uint32_t IOMMU_INT_ERR_ADDR1_REG;            /*!< Offset 0x114 IOMMU Interrupt Error Address 1 Register */
    __IO uint32_t IOMMU_INT_ERR_ADDR2_REG;            /*!< Offset 0x118 IOMMU Interrupt Error Address 2 Register */
    __IO uint32_t IOMMU_INT_ERR_ADDR3_REG;            /*!< Offset 0x11C IOMMU Interrupt Error Address 3 Register */
    __IO uint32_t IOMMU_INT_ERR_ADDR4_REG;            /*!< Offset 0x120 IOMMU Interrupt Error Address 4 Register */
    __IO uint32_t IOMMU_INT_ERR_ADDR5_REG;            /*!< Offset 0x124 IOMMU Interrupt Error Address 5 Register */
    __IO uint32_t IOMMU_INT_ERR_ADDR6_REG;            /*!< Offset 0x128 IOMMU Interrupt Error Address 6 Register */
         RESERVED(0x12C[0x0130 - 0x012C], uint8_t)
    __IO uint32_t IOMMU_INT_ERR_ADDR7_REG;            /*!< Offset 0x130 IOMMU Interrupt Error Address 7 Register */
    __IO uint32_t IOMMU_INT_ERR_ADDR8_REG;            /*!< Offset 0x134 IOMMU Interrupt Error Address 8 Register */
         RESERVED(0x138[0x0150 - 0x0138], uint8_t)
    __IO uint32_t IOMMU_INT_ERR_DATA0_REG;            /*!< Offset 0x150 IOMMU Interrupt Error Data 0 Register */
    __IO uint32_t IOMMU_INT_ERR_DATA1_REG;            /*!< Offset 0x154 IOMMU Interrupt Error Data 1 Register */
    __IO uint32_t IOMMU_INT_ERR_DATA2_REG;            /*!< Offset 0x158 IOMMU Interrupt Error Data 2 Register */
    __IO uint32_t IOMMU_INT_ERR_DATA3_REG;            /*!< Offset 0x15C IOMMU Interrupt Error Data 3 Register */
    __IO uint32_t IOMMU_INT_ERR_DATA4_REG;            /*!< Offset 0x160 IOMMU Interrupt Error Data 4 Register */
    __IO uint32_t IOMMU_INT_ERR_DATA5_REG;            /*!< Offset 0x164 IOMMU Interrupt Error Data 5 Register */
    __IO uint32_t IOMMU_INT_ERR_DATA6_REG;            /*!< Offset 0x168 IOMMU Interrupt Error Data 6 Register */
         RESERVED(0x16C[0x0170 - 0x016C], uint8_t)
    __IO uint32_t IOMMU_INT_ERR_DATA7_REG;            /*!< Offset 0x170 IOMMU Interrupt Error Data 7 Register */
    __IO uint32_t IOMMU_INT_ERR_DATA8_REG;            /*!< Offset 0x174 IOMMU Interrupt Error Data 8 Register */
         RESERVED(0x178[0x0180 - 0x0178], uint8_t)
    __IO uint32_t IOMMU_L1PG_INT_REG;                 /*!< Offset 0x180 IOMMU L1 Page Table Interrupt Register */
    __IO uint32_t IOMMU_L2PG_INT_REG;                 /*!< Offset 0x184 IOMMU L2 Page Table Interrupt Register */
         RESERVED(0x188[0x0190 - 0x0188], uint8_t)
    __IO uint32_t IOMMU_VA_REG;                       /*!< Offset 0x190 IOMMU Virtual Address Register */
    __IO uint32_t IOMMU_VA_DATA_REG;                  /*!< Offset 0x194 IOMMU Virtual Address Data Register */
    __IO uint32_t IOMMU_VA_CONFIG_REG;                /*!< Offset 0x198 IOMMU Virtual Address Configuration Register */
         RESERVED(0x19C[0x0200 - 0x019C], uint8_t)
    __IO uint32_t IOMMU_PMU_ENABLE_REG;               /*!< Offset 0x200 IOMMU PMU Enable Register */
         RESERVED(0x204[0x0210 - 0x0204], uint8_t)
    __IO uint32_t IOMMU_PMU_CLR_REG;                  /*!< Offset 0x210 IOMMU PMU Clear Register */
         RESERVED(0x214[0x0230 - 0x0214], uint8_t)
    __IO uint32_t IOMMU_PMU_ACCESS_LOW0_REG;          /*!< Offset 0x230 IOMMU PMU Access Low 0 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_HIGH0_REG;         /*!< Offset 0x234 IOMMU PMU Access High 0 Register */
    __IO uint32_t IOMMU_PMU_HIT_LOW0_REG;             /*!< Offset 0x238 IOMMU PMU Hit Low 0 Register */
    __IO uint32_t IOMMU_PMU_HIT_HIGH0_REG;            /*!< Offset 0x23C IOMMU PMU Hit High 0 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_LOW1_REG;          /*!< Offset 0x240 IOMMU PMU Access Low 1 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_HIGH1_REG;         /*!< Offset 0x244 IOMMU PMU Access High 1 Register */
    __IO uint32_t IOMMU_PMU_HIT_LOW1_REG;             /*!< Offset 0x248 IOMMU PMU Hit Low 1 Register */
    __IO uint32_t IOMMU_PMU_HIT_HIGH1_REG;            /*!< Offset 0x24C IOMMU PMU Hit High 1 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_LOW2_REG;          /*!< Offset 0x250 IOMMU PMU Access Low 2 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_HIGH2_REG;         /*!< Offset 0x254 IOMMU PMU Access High 2 Register */
    __IO uint32_t IOMMU_PMU_HIT_LOW2_REG;             /*!< Offset 0x258 IOMMU PMU Hit Low 2 Register */
    __IO uint32_t IOMMU_PMU_HIT_HIGH2_REG;            /*!< Offset 0x25C IOMMU PMU Hit High 2 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_LOW3_REG;          /*!< Offset 0x260 IOMMU PMU Access Low 3 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_HIGH3_REG;         /*!< Offset 0x264 IOMMU PMU Access High 3 Register */
    __IO uint32_t IOMMU_PMU_HIT_LOW3_REG;             /*!< Offset 0x268 IOMMU PMU Hit Low 3 Register */
    __IO uint32_t IOMMU_PMU_HIT_HIGH3_REG;            /*!< Offset 0x26C IOMMU PMU Hit High 3 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_LOW4_REG;          /*!< Offset 0x270 IOMMU PMU Access Low 4 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_HIGH4_REG;         /*!< Offset 0x274 IOMMU PMU Access High 4 Register */
    __IO uint32_t IOMMU_PMU_HIT_LOW4_REG;             /*!< Offset 0x278 IOMMU PMU Hit Low 4 Register */
    __IO uint32_t IOMMU_PMU_HIT_HIGH4_REG;            /*!< Offset 0x27C IOMMU PMU Hit High 4 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_LOW5_REG;          /*!< Offset 0x280 IOMMU PMU Access Low 5 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_HIGH5_REG;         /*!< Offset 0x284 IOMMU PMU Access High 5 Register */
    __IO uint32_t IOMMU_PMU_HIT_LOW5_REG;             /*!< Offset 0x288 IOMMU PMU Hit Low 5 Register */
    __IO uint32_t IOMMU_PMU_HIT_HIGH5_REG;            /*!< Offset 0x28C IOMMU PMU Hit High 5 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_LOW6_REG;          /*!< Offset 0x290 IOMMU PMU Access Low 6 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_HIGH6_REG;         /*!< Offset 0x294 IOMMU PMU Access High 6 Register */
    __IO uint32_t IOMMU_PMU_HIT_LOW6_REG;             /*!< Offset 0x298 IOMMU PMU Hit Low 6 Register */
    __IO uint32_t IOMMU_PMU_HIT_HIGH6_REG;            /*!< Offset 0x29C IOMMU PMU Hit High 6 Register */
         RESERVED(0x2A0[0x02D0 - 0x02A0], uint8_t)
    __IO uint32_t IOMMU_PMU_ACCESS_LOW7_REG;          /*!< Offset 0x2D0 IOMMU PMU Access Low 7 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_HIGH7_REG;         /*!< Offset 0x2D4 IOMMU PMU Access High 7 Register */
    __IO uint32_t IOMMU_PMU_HIT_LOW7_REG;             /*!< Offset 0x2D8 IOMMU PMU Hit Low 7 Register */
    __IO uint32_t IOMMU_PMU_HIT_HIGH7_REG;            /*!< Offset 0x2DC IOMMU PMU Hit High 7 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_LOW8_REG;          /*!< Offset 0x2E0 IOMMU PMU Access Low 8 Register */
    __IO uint32_t IOMMU_PMU_ACCESS_HIGH8_REG;         /*!< Offset 0x2E4 IOMMU PMU Access High 8 Register */
    __IO uint32_t IOMMU_PMU_HIT_LOW8_REG;             /*!< Offset 0x2E8 IOMMU PMU Hit Low 8 Register */
    __IO uint32_t IOMMU_PMU_HIT_HIGH8_REG;            /*!< Offset 0x2EC IOMMU PMU Hit High 8 Register */
         RESERVED(0x2F0[0x0300 - 0x02F0], uint8_t)
    __IO uint32_t IOMMU_PMU_TL_LOW0_REG;              /*!< Offset 0x300 IOMMU Total Latency Low 0 Register */
    __IO uint32_t IOMMU_PMU_TL_HIGH0_REG;             /*!< Offset 0x304 IOMMU Total Latency High 0 Register */
    __IO uint32_t IOMMU_PMU_ML0_REG;                  /*!< Offset 0x308 IOMMU Max Latency 0 Register */
         RESERVED(0x30C[0x0310 - 0x030C], uint8_t)
    __IO uint32_t IOMMU_PMU_TL_LOW1_REG;              /*!< Offset 0x310 IOMMU Total Latency Low 1 Register */
    __IO uint32_t IOMMU_PMU_TL_HIGH1_REG;             /*!< Offset 0x314 IOMMU Total Latency High 1 Register */
    __IO uint32_t IOMMU_PMU_ML1_REG;                  /*!< Offset 0x318 IOMMU Max Latency 1 Register */
         RESERVED(0x31C[0x0320 - 0x031C], uint8_t)
    __IO uint32_t IOMMU_PMU_TL_LOW2_REG;              /*!< Offset 0x320 IOMMU Total Latency Low 2 Register */
    __IO uint32_t IOMMU_PMU_TL_HIGH2_REG;             /*!< Offset 0x324 IOMMU Total Latency High 2 Register */
    __IO uint32_t IOMMU_PMU_ML2_REG;                  /*!< Offset 0x328 IOMMU Max Latency 2 Register */
         RESERVED(0x32C[0x0330 - 0x032C], uint8_t)
    __IO uint32_t IOMMU_PMU_TL_LOW3_REG;              /*!< Offset 0x330 IOMMU Total Latency Low 3 Register */
    __IO uint32_t IOMMU_PMU_TL_HIGH3_REG;             /*!< Offset 0x334 IOMMU Total Latency High 3 Register */
    __IO uint32_t IOMMU_PMU_ML3_REG;                  /*!< Offset 0x338 IOMMU Max Latency 3 Register */
         RESERVED(0x33C[0x0340 - 0x033C], uint8_t)
    __IO uint32_t IOMMU_PMU_TL_LOW4_REG;              /*!< Offset 0x340 IOMMU Total Latency Low 4 Register */
    __IO uint32_t IOMMU_PMU_TL_HIGH4_REG;             /*!< Offset 0x344 IOMMU Total Latency High 4 Register */
    __IO uint32_t IOMMU_PMU_ML4_REG;                  /*!< Offset 0x348 IOMMU Max Latency 4 Register */
         RESERVED(0x34C[0x0350 - 0x034C], uint8_t)
    __IO uint32_t IOMMU_PMU_TL_LOW5_REG;              /*!< Offset 0x350 IOMMU Total Latency Low 5 Register */
    __IO uint32_t IOMMU_PMU_TL_HIGH5_REG;             /*!< Offset 0x354 IOMMU Total Latency High 5 Register */
    __IO uint32_t IOMMU_PMU_ML5_REG;                  /*!< Offset 0x358 IOMMU Max Latency 5 Register */
         RESERVED(0x35C[0x0360 - 0x035C], uint8_t)
    __IO uint32_t IOMMU_PMU_TL_LOW6_REG;              /*!< Offset 0x360 IOMMU Total Latency Low 6 Register */
    __IO uint32_t IOMMU_PMU_TL_HIGH6_REG;             /*!< Offset 0x364 IOMMU Total Latency High 6 Register */
    __IO uint32_t IOMMU_PMU_ML6_REG;                  /*!< Offset 0x368 IOMMU Max Latency 6 Register */
} IOMMU_TypeDef; /* size of structure = 0x36C */
/*
 * @brief LEDC
 */
/*!< LEDC LED Lamp Controller */
typedef struct LEDC_Type
{
    __IO uint32_t LEDC_CTRL_REG;                      /*!< Offset 0x000 LEDC Control Register */
    __IO uint32_t LED_T01_TIMING_CTRL_REG;            /*!< Offset 0x004 LEDC T0 & T1 Timing Control Register */
    __IO uint32_t LEDC_DATA_FINISH_CNT_REG;           /*!< Offset 0x008 LEDC Data Finish Counter Register */
    __IO uint32_t LED_RESET_TIMING_CTRL_REG;          /*!< Offset 0x00C LEDC Reset Timing Control Register */
    __IO uint32_t LEDC_WAIT_TIME0_CTRL_REG;           /*!< Offset 0x010 LEDC Wait Time0 Control Register */
    __IO uint32_t LEDC_DATA_REG;                      /*!< Offset 0x014 LEDC Data Register */
    __IO uint32_t LEDC_DMA_CTRL_REG;                  /*!< Offset 0x018 LEDC DMA Control Register */
    __IO uint32_t LEDC_INT_CTRL_REG;                  /*!< Offset 0x01C LEDC Interrupt Control Register */
    __IO uint32_t LEDC_INT_STS_REG;                   /*!< Offset 0x020 LEDC Interrupt Status Register */
         RESERVED(0x024[0x0028 - 0x0024], uint8_t)
    __IO uint32_t LEDC_WAIT_TIME1_CTRL_REG;           /*!< Offset 0x028 LEDC Wait Time1 Control Register */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __IO uint32_t LEDC_FIFO_DATA_REG [0x020];         /*!< Offset 0x030 LEDC FIFO Data Registers array */
} LEDC_TypeDef; /* size of structure = 0x0B0 */
/*
 * @brief MCTL_COM
 */
/*!< MCTL_COM  */
typedef struct MCTL_COM_Type
{
    __IO uint32_t MCTL_COM_WORK_MODE0;                /*!< Offset 0x000 Reg_000 */
    __IO uint32_t MCTL_COM_WORK_MODE1;                /*!< Offset 0x004 Reg_004 */
    __IO uint32_t MCTL_COM_DBGCR;                     /*!< Offset 0x008 Reg_008 */
    __IO uint32_t MCTL_COM_TMR;                       /*!< Offset 0x00C Reg_00C */
         RESERVED(0x010[0x0014 - 0x0010], uint8_t)
    __IO uint32_t MCTL_COM_CCCR;                      /*!< Offset 0x014 Reg_014 */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IO uint32_t MCTL_COM_MAER0;                     /*!< Offset 0x020 Reg_020 */
    __IO uint32_t MCTL_COM_MAER1;                     /*!< Offset 0x024 Reg_024 */
    __IO uint32_t MCTL_COM_MAER2;                     /*!< Offset 0x028 Reg_028 */
         RESERVED(0x02C[0x0500 - 0x002C], uint8_t)
    __IO uint32_t MCTL_COM_REMAP0;                    /*!< Offset 0x500 REMAP0 */
    __IO uint32_t MCTL_COM_REMAP1;                    /*!< Offset 0x504 REMAP1 */
    __IO uint32_t MCTL_COM_REMAP2;                    /*!< Offset 0x508 REMAP2 */
    __IO uint32_t MCTL_COM_REMAP3;                    /*!< Offset 0x50C REMAP3 */
} MCTL_COM_TypeDef; /* size of structure = 0x510 */
/*
 * @brief MCTL_PHY
 */
/*!< MCTL_PHY  */
typedef struct MCTL_PHY_Type
{
    __IO uint32_t MCTL_PHY_PIR;                       /*!< Offset 0x000  */
    __IO uint32_t MCTL_PHY_PWRCTL;                    /*!< Offset 0x004  */
    __IO uint32_t MCTL_PHY_MRCTRL0;                   /*!< Offset 0x008  */
    __IO uint32_t MCTL_PHY_CLKEN;                     /*!< Offset 0x00C  */
    __IO uint32_t MCTL_PHY_PGSR0;                     /*!< Offset 0x010  */
    __IO uint32_t MCTL_PHY_PGSR1;                     /*!< Offset 0x014  */
    __IO uint32_t MCTL_PHY_STATR;                     /*!< Offset 0x018  */
         RESERVED(0x01C[0x002C - 0x001C], uint8_t)
    __IO uint32_t MCTL_PHY_LP3MR11;                   /*!< Offset 0x02C  */
    __IO uint32_t MCTL_PHY_DRAM_MR0;                  /*!< Offset 0x030  */
    __IO uint32_t MCTL_PHY_DRAM_MR1;                  /*!< Offset 0x034  */
    __IO uint32_t MCTL_PHY_DRAM_MR2;                  /*!< Offset 0x038  */
    __IO uint32_t MCTL_PHY_DRAM_MR3;                  /*!< Offset 0x03C  */
         RESERVED(0x040[0x0044 - 0x0040], uint8_t)
    __IO uint32_t MCTL_PHY_PTR0;                      /*!< Offset 0x044  */
         RESERVED(0x048[0x004C - 0x0048], uint8_t)
    __IO uint32_t MCTL_PHY_PTR2;                      /*!< Offset 0x04C  */
    __IO uint32_t MCTL_PHY_PTR3;                      /*!< Offset 0x050  */
    __IO uint32_t MCTL_PHY_PTR4;                      /*!< Offset 0x054  */
    __IO uint32_t MCTL_PHY_DRAMTMG0;                  /*!< Offset 0x058  */
    __IO uint32_t MCTL_PHY_DRAMTMG1;                  /*!< Offset 0x05C  */
    __IO uint32_t MCTL_PHY_DRAMTMG2;                  /*!< Offset 0x060  */
    __IO uint32_t MCTL_PHY_DRAMTMG3;                  /*!< Offset 0x064  */
    __IO uint32_t MCTL_PHY_DRAMTMG4;                  /*!< Offset 0x068  */
    __IO uint32_t MCTL_PHY_DRAMTMG5;                  /*!< Offset 0x06C  */
    __IO uint32_t MCTL_PHY_DRAMTMG6;                  /*!< Offset 0x070  */
    __IO uint32_t MCTL_PHY_DRAMTMG7;                  /*!< Offset 0x074  */
    __IO uint32_t MCTL_PHY_DRAMTMG8;                  /*!< Offset 0x078  */
    __IO uint32_t MCTL_PHY_ODTCFG;                    /*!< Offset 0x07C  */
    __IO uint32_t MCTL_PHY_PITMG0;                    /*!< Offset 0x080  */
    __IO uint32_t MCTL_PHY_PITMG1;                    /*!< Offset 0x084  */
    __IO uint32_t MCTL_PHY_LPTPR;                     /*!< Offset 0x088  */
    __IO uint32_t MCTL_PHY_RFSHCTL0;                  /*!< Offset 0x08C  */
    __IO uint32_t MCTL_PHY_RFSHTMG;                   /*!< Offset 0x090  */
    __IO uint32_t MCTL_PHY_RFSHCTL1;                  /*!< Offset 0x094  */
    __IO uint32_t MCTL_PHY_PWRTMG;                    /*!< Offset 0x098  */
    __IO uint32_t MCTL_PHY_ASRC;                      /*!< Offset 0x09C  */
    __IO uint32_t MCTL_PHY_ASRTC;                     /*!< Offset 0x0A0  */
         RESERVED(0x0A4[0x00B8 - 0x00A4], uint8_t)
    __IO uint32_t MCTL_PHY_VTFCR;                     /*!< Offset 0x0B8  */
    __IO uint32_t MCTL_PHY_DQSGMR;                    /*!< Offset 0x0BC  */
    __IO uint32_t MCTL_PHY_DTCR;                      /*!< Offset 0x0C0  */
    __IO uint32_t MCTL_PHY_DTAR0;                     /*!< Offset 0x0C4  */
         RESERVED(0x0C8[0x0100 - 0x00C8], uint8_t)
    __IO uint32_t MCTL_PHY_PGCR0;                     /*!< Offset 0x100  */
    __IO uint32_t MCTL_PHY_PGCR1;                     /*!< Offset 0x104  */
    __IO uint32_t MCTL_PHY_PGCR2;                     /*!< Offset 0x108  */
    __IO uint32_t MCTL_PHY_PGCR3;                     /*!< Offset 0x10C  */
    __IO uint32_t MCTL_PHY_IOVCR0;                    /*!< Offset 0x110  */
    __IO uint32_t MCTL_PHY_IOVCR1;                    /*!< Offset 0x114  */
         RESERVED(0x118[0x011C - 0x0118], uint8_t)
    __IO uint32_t MCTL_PHY_DXCCR;                     /*!< Offset 0x11C  */
    __IO uint32_t MCTL_PHY_ODTMAP;                    /*!< Offset 0x120  */
    __IO uint32_t MCTL_PHY_ZQCTL0;                    /*!< Offset 0x124  */
    __IO uint32_t MCTL_PHY_ZQCTL1;                    /*!< Offset 0x128  */
         RESERVED(0x12C[0x0140 - 0x012C], uint8_t)
    __IO uint32_t MCTL_PHY_ZQCR;                      /*!< Offset 0x140  */
    __IO uint32_t MCTL_PHY_ZQSR;                      /*!< Offset 0x144  */
    __IO uint32_t MCTL_PHY_ZQDR0;                     /*!< Offset 0x148  */
    __IO uint32_t MCTL_PHY_ZQDR1;                     /*!< Offset 0x14C  */
    __IO uint32_t MCTL_PHY_ZQDR2;                     /*!< Offset 0x150  */
         RESERVED(0x154[0x01C0 - 0x0154], uint8_t)
    __IO uint32_t MCTL_PHY_SCHED;                     /*!< Offset 0x1C0  */
    __IO uint32_t MCTL_PHY_PERFHPR0;                  /*!< Offset 0x1C4  */
    __IO uint32_t MCTL_PHY_PERFHPR1;                  /*!< Offset 0x1C8  */
    __IO uint32_t MCTL_PHY_PERFLPR0;                  /*!< Offset 0x1CC  */
    __IO uint32_t MCTL_PHY_PERFLPR1;                  /*!< Offset 0x1D0  */
    __IO uint32_t MCTL_PHY_PERFWR0;                   /*!< Offset 0x1D4  */
    __IO uint32_t MCTL_PHY_PERFWR1;                   /*!< Offset 0x1D8  */
         RESERVED(0x1DC[0x0200 - 0x01DC], uint8_t)
    __IO uint32_t MCTL_PHY_ACMDLR;                    /*!< Offset 0x200  */
    __IO uint32_t MCTL_PHY_ACLDLR;                    /*!< Offset 0x204  */
    __IO uint32_t MCTL_PHY_ACIOCR0;                   /*!< Offset 0x208  */
         RESERVED(0x20C[0x0210 - 0x020C], uint8_t)
    __IO uint32_t MCTL_PHY_ACIOCR1 [0x010];           /*!< Offset 0x210 0x210 + 0x4 * x */
} MCTL_PHY_TypeDef; /* size of structure = 0x250 */
/*
 * @brief MSGBOX
 */
/*!< MSGBOX Message Box */
typedef struct MSGBOX_Type
{
         RESERVED(0x000[0x0020 - 0x0000], uint8_t)
    struct
    {
        __IO uint32_t MSGBOX_RD_IRQ_EN_REG;           /*!< Offset 0x020 0x0020+N*0x0100 (N=0-1) MSGBOX Read IRQ Enable Register */
        __IO uint32_t MSGBOX_RD_IRQ_STATUS_REG;       /*!< Offset 0x024 0x0024+N*0x0100 (N=0-1) MSGBOX Read IRQ Status Register */
             RESERVED(0x008[0x0010 - 0x0008], uint8_t)
        __IO uint32_t MSGBOX_WR_IRQ_EN_REG;           /*!< Offset 0x030 0x0030+N*0x0100 (N=0-1) MSGBOX Write IRQ Enable Register */
        __IO uint32_t MSGBOX_WR_IRQ_STATUS_REG;       /*!< Offset 0x034 0x0034+N*0x0100 (N=0-1) MSGBOX Write IRQ Status Register */
             RESERVED(0x018[0x0020 - 0x0018], uint8_t)
        __IO uint32_t MSGBOX_DEBUG_REG;               /*!< Offset 0x040 0x0040+N*0x0100 (N=0-1) MSGBOX Debug Register */
             RESERVED(0x024[0x0030 - 0x0024], uint8_t)
        __IO uint32_t MSGBOX_FIFO_STATUS_REG [0x004]; /*!< Offset 0x050 0x0050+N*0x0100+P*0x0004 (N=0-1)(P=0-3) MSGBOX FIFO Status Register */
        __IO uint32_t MSGBOX_MSG_STATUS_REG [0x004];  /*!< Offset 0x060 0x0060+N*0x0100+P*0x0004 (N=0-1)(P=0-3) MSGBOX Message Status Register */
        __IO uint32_t MSGBOX_MSG_REG [0x004];         /*!< Offset 0x070 0x0070+N*0x0100+P*0x0004 (N=0-1)(P=0-3) MSGBOX Message Queue Register */
        __IO uint32_t MSGBOX_WR_INT_THRESHOLD_REG [0x004];/*!< Offset 0x080 0x0080+N*0x0100+P*0x0004 (N=0-1)(P=0-3) MSGBOX Write IRQ Threshold Register */
             RESERVED(0x070[0x0100 - 0x0070], uint8_t)
    } N [0x002];                                      /*!< Offset 0x020 MSGBOX (N=0-1) */
} MSGBOX_TypeDef; /* size of structure = 0x220 */
/*
 * @brief OWA
 */
/*!< OWA One Wire Audio (TX only) */
typedef struct OWA_Type
{
    __IO uint32_t OWA_GEN_CTL;                        /*!< Offset 0x000 OWA General Control Register */
    __IO uint32_t OWA_TX_CFIG;                        /*!< Offset 0x004 OWA TX Configuration Register */
         RESERVED(0x008[0x000C - 0x0008], uint8_t)
    __IO uint32_t OWA_ISTA;                           /*!< Offset 0x00C OWA Interrupt Status Register */
         RESERVED(0x010[0x0014 - 0x0010], uint8_t)
    __IO uint32_t OWA_FCTL;                           /*!< Offset 0x014 OWA FIFO Control Register */
    __IO uint32_t OWA_FSTA;                           /*!< Offset 0x018 OWA FIFO Status Register */
    __IO uint32_t OWA_INT;                            /*!< Offset 0x01C OWA Interrupt Control Register */
    __IO uint32_t OWA_TX_FIFO;                        /*!< Offset 0x020 OWA TX FIFO Register */
    __IO uint32_t OWA_TX_CNT;                         /*!< Offset 0x024 OWA TX Counter Register */
         RESERVED(0x028[0x002C - 0x0028], uint8_t)
    __IO uint32_t OWA_TX_CHSTA0;                      /*!< Offset 0x02C OWA TX Channel Status Register0 */
    __IO uint32_t OWA_TX_CHSTA1;                      /*!< Offset 0x030 OWA TX Channel Status Register1 */
} OWA_TypeDef; /* size of structure = 0x034 */
/*
 * @brief PWM
 */
/*!< PWM Pulse Width Modulation module */
typedef struct PWM_Type
{
    __IO uint32_t PIER;                               /*!< Offset 0x000 PWM IRQ Enable Register */
    __IO uint32_t PISR;                               /*!< Offset 0x004 PWM IRQ Status Register */
         RESERVED(0x008[0x0010 - 0x0008], uint8_t)
    __IO uint32_t CIER;                               /*!< Offset 0x010 Capture IRQ Enable Register */
    __IO uint32_t CISR;                               /*!< Offset 0x014 Capture IRQ Status Register */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IO uint32_t PCCR [0x004];                       /*!< Offset 0x020 PWM01, PWM23, PWM45, PWM67 Clock Configuration Register */
         RESERVED(0x030[0x0040 - 0x0030], uint8_t)
    __IO uint32_t PCGR;                               /*!< Offset 0x040 PWM Clock Gating Register */
         RESERVED(0x044[0x0060 - 0x0044], uint8_t)
    __IO uint32_t PDZCR [0x004];                      /*!< Offset 0x060 PWM01, PWM23, PWM45, PWM67 Dead Zone Control Register */
         RESERVED(0x070[0x0080 - 0x0070], uint8_t)
    __IO uint32_t PER;                                /*!< Offset 0x080 PWM Enable Register */
         RESERVED(0x084[0x0090 - 0x0084], uint8_t)
    __IO uint32_t PGR0 [0x004];                       /*!< Offset 0x090 PWM Group0, Group1, Group2, Group3 Register */
         RESERVED(0x0A0[0x00C0 - 0x00A0], uint8_t)
    __IO uint32_t CER;                                /*!< Offset 0x0C0 Capture Enable Register */
         RESERVED(0x0C4[0x0100 - 0x00C4], uint8_t)
    struct
    {
        __IO uint32_t PCR;                            /*!< Offset 0x100 PWM Control Register */
        __IO uint32_t PPR;                            /*!< Offset 0x104 PWM Period Register */
        __IO uint32_t PCNTR;                          /*!< Offset 0x108 PWM Count Register */
        __IO uint32_t PPCNTR;                         /*!< Offset 0x10C PWM Pulse Count Register */
        __IO uint32_t CCR;                            /*!< Offset 0x110 Capture Control Register */
        __IO uint32_t CRLR;                           /*!< Offset 0x114 Capture Rise Lock Register */
        __IO uint32_t CFLR;                           /*!< Offset 0x118 Capture Fall Lock Register */
             RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    } CH [0x008];                                     /*!< Offset 0x100 Channels[0..7] */
} PWM_TypeDef; /* size of structure = 0x200 */
/*
 * @brief RISC_CFG
 */
/*!< RISC_CFG RISC-V core configuration register */
typedef struct RISC_CFG_Type
{
         RESERVED(0x000[0x0004 - 0x0000], uint8_t)
    __IO uint32_t RISC_STA_ADD0_REG;                  /*!< Offset 0x004 RISC Start Address0 Register */
    __IO uint32_t RISC_STA_ADD1_REG;                  /*!< Offset 0x008 RISC Start Address1 Register */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IO uint32_t RF1P_CFG_REG;                       /*!< Offset 0x010 RF1P Configuration Register */
         RESERVED(0x014[0x001C - 0x0014], uint8_t)
    __IO uint32_t ROM_CFG_REG;                        /*!< Offset 0x01C ROM Configuration Register */
    __IO uint32_t WAKEUP_EN_REG;                      /*!< Offset 0x020 Wakeup Enable Register */
    __IO uint32_t WAKEUP_MASK_REG [0x005];            /*!< Offset 0x024 Wakeup Mask0..Mask4 Register */
         RESERVED(0x038[0x0040 - 0x0038], uint8_t)
    __IO uint32_t TS_TMODE_SEL_REG;                   /*!< Offset 0x040 Timestamp Test Mode Select Register */
    __IO uint32_t SRAM_ADDR_TWIST_REG;                /*!< Offset 0x044 SRAM Address Twist Register */
    __IO uint32_t WORK_MODE_REG;                      /*!< Offset 0x048 Work Mode Register */
         RESERVED(0x04C[0x0050 - 0x004C], uint8_t)
    __IO uint32_t RETITE_PC0_REG;                     /*!< Offset 0x050 Retire PC0 Register */
    __IO uint32_t RETITE_PC1_REG;                     /*!< Offset 0x054 Retire PC1 Register */
         RESERVED(0x058[0x0060 - 0x0058], uint8_t)
    __IO uint32_t IRQ_MODE_REG [0x005];               /*!< Offset 0x060 IRQ Mode0..Mode4 Register */
         RESERVED(0x074[0x0104 - 0x0074], uint8_t)
    __IO uint32_t RISC_AXI_PMU_CTRL;                  /*!< Offset 0x104 RISC AXI PMU Control Register */
    __IO uint32_t RISC_AXI_PMU_PRD;                   /*!< Offset 0x108 RISC AXI PMU Period Register */
    __IO uint32_t RISC_AXI_PMU_LAT_RD;                /*!< Offset 0x10C RISC AXI PMU Read Latency Register */
    __IO uint32_t RISC_AXI_PMU_LAT_WR;                /*!< Offset 0x110 RISC AXI PMU Write Latency Register */
    __IO uint32_t RISC_AXI_PMU_REQ_RD;                /*!< Offset 0x114 RISC AXI PMU Read Request Register */
    __IO uint32_t RISC_AXI_PMU_REQ_WR;                /*!< Offset 0x118 RISC AXI PMU Write Request Register */
    __IO uint32_t RISC_AXI_PMU_BW_RD;                 /*!< Offset 0x11C RISC AXI PMU Read Bandwidth Register */
    __IO uint32_t RISC_AXI_PMU_BW_WR;                 /*!< Offset 0x120 RISC AXI PMU Write Bandwidth Register */
} RISC_CFG_TypeDef; /* size of structure = 0x124 */
/*
 * @brief RTC
 */
/*!< RTC Real Time Clock */
typedef struct RTC_Type
{
    __IO uint32_t LOSC_CTRL_REG;                      /*!< Offset 0x000 Low Oscillator Control Register */
    __IO uint32_t LOSC_AUTO_SWT_STA_REG;              /*!< Offset 0x004 Low Oscillator Auto Switch Status Register */
    __IO uint32_t INTOSC_CLK_PRESCAL_REG;             /*!< Offset 0x008 Internal OSC Clock Pre-scaler Register */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IO uint32_t RTC_DAY_REG;                        /*!< Offset 0x010 RTC Year-Month-Day Register */
    __IO uint32_t RTC_HH_MM_SS_REG;                   /*!< Offset 0x014 RTC Hour-Minute-Second Register */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IO uint32_t ALARM0_DAY_SET_REG;                 /*!< Offset 0x020 Alarm 0 Day Setting Register */
    __IO uint32_t ALARM0_CUR_VLU_REG;                 /*!< Offset 0x024 Alarm 0 Counter Current Value Register */
    __IO uint32_t ALARM0_ENABLE_REG;                  /*!< Offset 0x028 Alarm 0 Enable Register */
    __IO uint32_t ALARM0_IRQ_EN;                      /*!< Offset 0x02C Alarm 0 IRQ Enable Register */
    __IO uint32_t ALARM0_IRQ_STA_REG;                 /*!< Offset 0x030 Alarm 0 IRQ Status Register */
         RESERVED(0x034[0x0050 - 0x0034], uint8_t)
    __IO uint32_t ALARM_CONFIG_REG;                   /*!< Offset 0x050 Alarm Configuration Register */
         RESERVED(0x054[0x0060 - 0x0054], uint8_t)
    __IO uint32_t F32K_FOUT_CTRL_GATING_REG;          /*!< Offset 0x060 32K Fanout Control Gating Register */
         RESERVED(0x064[0x0100 - 0x0064], uint8_t)
    __IO uint32_t GP_DATA_REG [0x008];                /*!< Offset 0x100 General Purpose Register (N=0 to 7) */
    __IO uint32_t FBOOT_INFO_REG0;                    /*!< Offset 0x120 Fast Boot Information Register0 */
    __IO uint32_t FBOOT_INFO_REG1;                    /*!< Offset 0x124 Fast Boot Information Register1 */
         RESERVED(0x128[0x0160 - 0x0128], uint8_t)
    __IO uint32_t DCXO_CTRL_REG;                      /*!< Offset 0x160 DCXO Control Register */
         RESERVED(0x164[0x0190 - 0x0164], uint8_t)
    __IO uint32_t RTC_VIO_REG;                        /*!< Offset 0x190 RTC_VIO Regulation Register */
         RESERVED(0x194[0x01F0 - 0x0194], uint8_t)
    __IO uint32_t IC_CHARA_REG;                       /*!< Offset 0x1F0 IC Characteristic Register */
    __IO uint32_t VDD_OFF_GATING_CTRL_REG;            /*!< Offset 0x1F4 VDD Off Gating Control Register */
         RESERVED(0x1F8[0x0204 - 0x01F8], uint8_t)
    __IO uint32_t EFUSE_HV_PWRSWT_CTRL_REG;           /*!< Offset 0x204 Efuse High Voltage Power Switch Control Register */
         RESERVED(0x208[0x0310 - 0x0208], uint8_t)
    __IO uint32_t RTC_SPI_CLK_CTRL_REG;               /*!< Offset 0x310 RTC SPI Clock Control Register */
         RESERVED(0x314[0x1000 - 0x0314], uint8_t)
} RTC_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief R_CPUCFG
 */
/*!< R_CPUCFG  */
typedef struct R_CPUCFG_Type
{
    __IO uint32_t REGxx;                              /*!< Offset 0x000 Bit 31 and bit 0 R/W, bit 0 can be related to RISC-C vore */
         RESERVED(0x004[0x01C0 - 0x0004], uint8_t)
    __IO uint32_t HOTPLUGFLAG;                        /*!< Offset 0x1C0 The Hotplug Flag Register is 0x070005C0. */
    __IO uint32_t SOFTENTRY [0x004];                  /*!< Offset 0x1C4 The Soft Entry Address Register of CPUx (x=0..1) */
    __IO uint32_t SUP_STAN_FLAG;                      /*!< Offset 0x1D4 Super Standby Flag (bit 16) */
         RESERVED(0x1D8[0x01DC - 0x01D8], uint8_t)
    __IO uint32_t RV_HOTPLUGFLAG;                     /*!< Offset 0x1DC RV The Hotplug Flag Register (key value 0xFA50392F) */
    __IO uint32_t RC_SOFTENTRY [0x004];               /*!< Offset 0x1E0 RV The Soft Entry Address Register */
} R_CPUCFG_TypeDef; /* size of structure = 0x1F0 */
/*
 * @brief R_PRCM
 */
/*!< R_PRCM  */
typedef struct R_PRCM_Type
{
         RESERVED(0x000[0x0250 - 0x0000], uint8_t)
    __IO uint32_t VDD_SYS_PWROFF_GATING_REG;          /*!< Offset 0x250  */
    __IO uint32_t ANALOG_PWROFF_GATING_REG;           /*!< Offset 0x254  */
} R_PRCM_TypeDef; /* size of structure = 0x258 */
/*
 * @brief SID
 */
/*!< SID Security ID */
typedef struct SID_Type
{
    __IO uint32_t SID_RKEY0;                          /*!< Offset 0x000 Securiy root key[31:0] */
    __IO uint32_t SID_RKEY1;                          /*!< Offset 0x004 Securiy root key[63:32] */
    __IO uint32_t SID_RKEY2;                          /*!< Offset 0x008 Securiy root key[95:64] */
    __IO uint32_t SID_RKEY3;                          /*!< Offset 0x00C Securiy root key[127:96] */
         RESERVED(0x010[0x0014 - 0x0010], uint8_t)
    __IO uint32_t SID_THS;                            /*!< Offset 0x014 [27:16]: The calibration value of the T-sensor. */
         RESERVED(0x018[0x0200 - 0x0018], uint8_t)
    __IO uint32_t SID_DATA [0x004];                   /*!< Offset 0x200 SID data (xfel display as 'sid' replay) */
    __IO uint32_t BOOT_MODE;                          /*!< Offset 0x210 [27:16]: eFUSE boot select status, [0]: 0: GPIO boot select, 1: eFuse boot select */
    __IO uint32_t SID_UNDOC [0x1FB];                  /*!< Offset 0x214  */
         RESERVED(0xA00[0x1000 - 0x0A00], uint8_t)
} SID_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief SMC
 */
/*!< SMC Secure Memory Control (SMC) - Sets secure area of DRAM */
typedef struct SMC_Type
{
         RESERVED(0x000[0x1000 - 0x0000], uint8_t)
} SMC_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief SMHC
 */
/*!< SMHC SD-MMC Host Controller */
typedef struct SMHC_Type
{
    __IO uint32_t SMHC_CTRL;                          /*!< Offset 0x000 Control Register */
    __IO uint32_t SMHC_CLKDIV;                        /*!< Offset 0x004 Clock Control Register */
    __IO uint32_t SMHC_TMOUT;                         /*!< Offset 0x008 Time Out Register */
    __IO uint32_t SMHC_CTYPE;                         /*!< Offset 0x00C Bus Width Register */
    __IO uint32_t SMHC_BLKSIZ;                        /*!< Offset 0x010 Block Size Register */
    __IO uint32_t SMHC_BYTCNT;                        /*!< Offset 0x014 Byte Count Register */
    __IO uint32_t SMHC_CMD;                           /*!< Offset 0x018 Command Register */
    __IO uint32_t SMHC_CMDARG;                        /*!< Offset 0x01C Command Argument Register */
    __IO uint32_t SMHC_RESP0;                         /*!< Offset 0x020 Response 0 Register */
    __IO uint32_t SMHC_RESP1;                         /*!< Offset 0x024 Response 1 Register */
    __IO uint32_t SMHC_RESP2;                         /*!< Offset 0x028 Response 2 Register */
    __IO uint32_t SMHC_RESP3;                         /*!< Offset 0x02C Response 3 Register */
    __IO uint32_t SMHC_INTMASK;                       /*!< Offset 0x030 Interrupt Mask Register */
    __IO uint32_t SMHC_MINTSTS;                       /*!< Offset 0x034 Masked Interrupt Status Register */
    __IO uint32_t SMHC_RINTSTS;                       /*!< Offset 0x038 Raw Interrupt Status Register */
    __IO uint32_t SMHC_STATUS;                        /*!< Offset 0x03C Status Register */
    __IO uint32_t SMHC_FIFOTH;                        /*!< Offset 0x040 FIFO Water Level Register */
    __IO uint32_t SMHC_FUNS;                          /*!< Offset 0x044 FIFO Function Select Register */
    __IO uint32_t SMHC_TCBCNT;                        /*!< Offset 0x048 Transferred Byte Count between Controller and Card */
    __IO uint32_t SMHC_TBBCNT;                        /*!< Offset 0x04C Transferred Byte Count between Host Memory and Internal FIFO */
    __IO uint32_t SMHC_DBGC;                          /*!< Offset 0x050 Current Debug Control Register */
    __IO uint32_t SMHC_CSDC;                          /*!< Offset 0x054 CRC Status Detect Control Registers */
    __IO uint32_t SMHC_A12A;                          /*!< Offset 0x058 Auto Command 12 Argument Register */
    __IO uint32_t SMHC_NTSR;                          /*!< Offset 0x05C SD New Timing Set Register */
         RESERVED(0x060[0x0078 - 0x0060], uint8_t)
    __IO uint32_t SMHC_HWRST;                         /*!< Offset 0x078 Hardware Reset Register */
         RESERVED(0x07C[0x0080 - 0x007C], uint8_t)
    __IO uint32_t SMHC_IDMAC;                         /*!< Offset 0x080 IDMAC Control Register */
    __IO uint32_t SMHC_DLBA;                          /*!< Offset 0x084 Descriptor List Base Address Register */
    __IO uint32_t SMHC_IDST;                          /*!< Offset 0x088 IDMAC Status Register */
    __IO uint32_t SMHC_IDIE;                          /*!< Offset 0x08C IDMAC Interrupt Enable Register */
         RESERVED(0x090[0x0100 - 0x0090], uint8_t)
    __IO uint32_t SMHC_THLD;                          /*!< Offset 0x100 Card Threshold Control Register */
    __IO uint32_t SMHC_SFC;                           /*!< Offset 0x104 Sample FIFO Control Register */
    __IO uint32_t SMHC_A23A;                          /*!< Offset 0x108 Auto Command 23 Argument Register */
    __IO uint32_t EMMC_DDR_SBIT_DET;                  /*!< Offset 0x10C eMMC4.5 DDR Start Bit Detection Control Register */
         RESERVED(0x110[0x0138 - 0x0110], uint8_t)
    __IO uint32_t SMHC_EXT_CMD;                       /*!< Offset 0x138 Extended Command Register */
    __IO uint32_t SMHC_EXT_RESP;                      /*!< Offset 0x13C Extended Response Register */
    __IO uint32_t SMHC_DRV_DL;                        /*!< Offset 0x140 Drive Delay Control Register */
    __IO uint32_t SMHC_SAMP_DL;                       /*!< Offset 0x144 Sample Delay Control Register */
    __IO uint32_t SMHC_DS_DL;                         /*!< Offset 0x148 Data Strobe Delay Control Register */
    __IO uint32_t SMHC_HS400_DL;                      /*!< Offset 0x14C HS400 Delay Control Register */
         RESERVED(0x150[0x0200 - 0x0150], uint8_t)
    __IO uint32_t SMHC_FIFO;                          /*!< Offset 0x200 Read/Write FIFO */
         RESERVED(0x204[0x1000 - 0x0204], uint8_t)
} SMHC_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief SPC
 */
/*!< SPC Secure Peripherals Control (SPC) - Sets secure property of peripherals */
typedef struct SPC_Type
{
         RESERVED(0x000[0x0400 - 0x0000], uint8_t)
} SPC_TypeDef; /* size of structure = 0x400 */
/*
 * @brief SPI
 */
/*!< SPI Serial Peripheral Interface */
typedef struct SPI_Type
{
         RESERVED(0x000[0x0004 - 0x0000], uint8_t)
    __IO uint32_t SPI_GCR;                            /*!< Offset 0x004 SPI Global Control Register */
    __IO uint32_t SPI_TCR;                            /*!< Offset 0x008 SPI Transfer Control Register */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IO uint32_t SPI_IER;                            /*!< Offset 0x010 SPI Interrupt Control Register */
    __IO uint32_t SPI_ISR;                            /*!< Offset 0x014 SPI Interrupt Status Register */
    __IO uint32_t SPI_FCR;                            /*!< Offset 0x018 SPI FIFO Control Register */
    __IO uint32_t SPI_FSR;                            /*!< Offset 0x01C SPI FIFO Status Register */
    __IO uint32_t SPI_WCR;                            /*!< Offset 0x020 SPI Wait Clock Register */
    __IO uint32_t SPI_CCR;                            /*!< Offset 0x024 SPI Clock Control Register (not documented) */
    __IO uint32_t SPI_SAMP_DL;                        /*!< Offset 0x028 SPI Sample Delay Control Register */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __IO uint32_t SPI_MBC;                            /*!< Offset 0x030 SPI Master Burst Counter Register */
    __IO uint32_t SPI_MTC;                            /*!< Offset 0x034 SPI Master Transmit Counter Register */
    __IO uint32_t SPI_BCC;                            /*!< Offset 0x038 SPI Master Burst Control Register */
         RESERVED(0x03C[0x0040 - 0x003C], uint8_t)
    __IO uint32_t SPI_BATCR;                          /*!< Offset 0x040 SPI Bit-Aligned Transfer Configure Register */
    __IO uint32_t SPI_BA_CCR;                         /*!< Offset 0x044 SPI Bit-Aligned Clock Configuration Register */
    __IO uint32_t SPI_TBR;                            /*!< Offset 0x048 SPI TX Bit Register */
    __IO uint32_t SPI_RBR;                            /*!< Offset 0x04C SPI RX Bit Register */
         RESERVED(0x050[0x0088 - 0x0050], uint8_t)
    __IO uint32_t SPI_NDMA_MODE_CTL;                  /*!< Offset 0x088 SPI Normal DMA Mode Control Register */
         RESERVED(0x08C[0x0100 - 0x008C], uint8_t)
    __IO uint32_t DBI_CTL_0;                          /*!< Offset 0x100 DBI Control Register 0 */
    __IO uint32_t DBI_CTL_1;                          /*!< Offset 0x104 DBI Control Register 1 */
    __IO uint32_t DBI_CTL_2;                          /*!< Offset 0x108 DBI Control Register 2 */
    __IO uint32_t DBI_TIMER;                          /*!< Offset 0x10C DBI Timer Control Register */
    __IO uint32_t DBI_VIDEO_SZIE;                     /*!< Offset 0x110 DBI Video Size Configuration Register */
         RESERVED(0x114[0x0120 - 0x0114], uint8_t)
    __IO uint32_t DBI_INT;                            /*!< Offset 0x120 DBI Interrupt Register */
    __IO uint32_t DBI_DEBUG_0;                        /*!< Offset 0x124 DBI BEBUG 0 Register */
    __IO uint32_t DBI_DEBUG_1;                        /*!< Offset 0x128 DBI BEBUG 1 Register */
         RESERVED(0x12C[0x0200 - 0x012C], uint8_t)
    __IO uint32_t SPI_TXD;                            /*!< Offset 0x200 SPI TX Data Register */
         RESERVED(0x204[0x0300 - 0x0204], uint8_t)
    __IO uint32_t SPI_RXD;                            /*!< Offset 0x300 SPI RX Data Register */
         RESERVED(0x304[0x1000 - 0x0304], uint8_t)
} SPI_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief SPINLOCK
 */
/*!< SPINLOCK Spin Lock module */
typedef struct SPINLOCK_Type
{
    __IO uint32_t SPINLOCK_SYSTATUS_REG;              /*!< Offset 0x000 Spinlock System Status Register */
         RESERVED(0x004[0x0010 - 0x0004], uint8_t)
    __IO uint32_t SPINLOCK_STATUS_REG;                /*!< Offset 0x010 Spinlock Status Register */
         RESERVED(0x014[0x0020 - 0x0014], uint8_t)
    __IO uint32_t SPINLOCK_IRQ_EN_REG;                /*!< Offset 0x020 Spinlock Interrupt Enable Register */
         RESERVED(0x024[0x0040 - 0x0024], uint8_t)
    __IO uint32_t SPINLOCK_IRQ_STA_REG;               /*!< Offset 0x040 Spinlock Interrupt Status Register */
         RESERVED(0x044[0x0080 - 0x0044], uint8_t)
    __IO uint32_t SPINLOCK_LOCKID0_REG;               /*!< Offset 0x080 Spinlock Lockid0 Register */
    __IO uint32_t SPINLOCK_LOCKID1_REG;               /*!< Offset 0x084 Spinlock Lockid1 Register */
    __IO uint32_t SPINLOCK_LOCKID2_REG;               /*!< Offset 0x088 Spinlock Lockid2 Register */
    __IO uint32_t SPINLOCK_LOCKID3_REG;               /*!< Offset 0x08C Spinlock Lockid3 Register */
    __IO uint32_t SPINLOCK_LOCKID4_REG;               /*!< Offset 0x090 Spinlock Lockid4 Register */
         RESERVED(0x094[0x0100 - 0x0094], uint8_t)
    __IO uint32_t SPINLOCK_LOCK_REG [0x020];          /*!< Offset 0x100 Spinlock Register N (N = 0 to 31) */
} SPINLOCK_TypeDef; /* size of structure = 0x180 */
/*
 * @brief SYS_CFG
 */
/*!< SYS_CFG  */
typedef struct SYS_CFG_Type
{
         RESERVED(0x000[0x0008 - 0x0000], uint8_t)
    __IO uint32_t DSP_BOOT_RAMMAP_REG;                /*!< Offset 0x008 DSP Boot SRAM Remap Control Register */
         RESERVED(0x00C[0x0024 - 0x000C], uint8_t)
    __I  uint32_t VER_REG;                            /*!< Offset 0x024 Version Register */
         RESERVED(0x028[0x0030 - 0x0028], uint8_t)
    __IO uint32_t EMAC_EPHY_CLK_REG0;                 /*!< Offset 0x030 EMAC-EPHY Clock Register 0 */
         RESERVED(0x034[0x0150 - 0x0034], uint8_t)
    __IO uint32_t SYS_LDO_CTRL_REG;                   /*!< Offset 0x150 System LDO Control Register */
         RESERVED(0x154[0x0160 - 0x0154], uint8_t)
    __IO uint32_t RESCAL_CTRL_REG;                    /*!< Offset 0x160 Resistor Calibration Control Register */
         RESERVED(0x164[0x0168 - 0x0164], uint8_t)
    __IO uint32_t RES240_CTRL_REG;                    /*!< Offset 0x168 240ohms Resistor Manual Control Register */
    __IO uint32_t RESCAL_STATUS_REG;                  /*!< Offset 0x16C Resistor Calibration Status Register */
         RESERVED(0x170[0x021C - 0x0170], uint8_t)
    __IO uint32_t SYS_LDOB_SID;                       /*!< Offset 0x21C  */
         RESERVED(0x220[0x0228 - 0x0220], uint8_t)
    __IO uint32_t SYS_EFUSE_REG;                      /*!< Offset 0x228  */
         RESERVED(0x22C[0x1000 - 0x022C], uint8_t)
} SYS_CFG_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief TCON_LCD
 */
/*!< TCON_LCD Timing Controller_LCD (TCON_LCD) */
typedef struct TCON_LCD_Type
{
    __IO uint32_t LCD_GCTL_REG;                       /*!< Offset 0x000 LCD Global Control Register */
    __IO uint32_t LCD_GINT0_REG;                      /*!< Offset 0x004 LCD Global Interrupt Register0 */
    __IO uint32_t LCD_GINT1_REG;                      /*!< Offset 0x008 LCD Global Interrupt Register1 */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IO uint32_t LCD_FRM_CTL_REG;                    /*!< Offset 0x010 LCD FRM Control Register */
    __IO uint32_t LCD_FRM_SEED_REG [0x006];           /*!< Offset 0x014 LCD FRM Seed Register (N=0,1,2,3,4,5) */
    __IO uint32_t LCD_FRM_TAB_REG [0x004];            /*!< Offset 0x02C LCD FRM Table Register (N=0,1,2,3) */
    __IO uint32_t LCD_3D_FIFO_REG;                    /*!< Offset 0x03C LCD 3D FIFO Register */
    __IO uint32_t LCD_CTL_REG;                        /*!< Offset 0x040 LCD Control Register */
    __IO uint32_t LCD_DCLK_REG;                       /*!< Offset 0x044 LCD Data Clock Register */
    __IO uint32_t LCD_BASIC0_REG;                     /*!< Offset 0x048 LCD Basic Timing Register0 */
    __IO uint32_t LCD_BASIC1_REG;                     /*!< Offset 0x04C LCD Basic Timing Register1 */
    __IO uint32_t LCD_BASIC2_REG;                     /*!< Offset 0x050 LCD Basic Timing Register2 */
    __IO uint32_t LCD_BASIC3_REG;                     /*!< Offset 0x054 LCD Basic Timing Register3 */
    __IO uint32_t LCD_HV_IF_REG;                      /*!< Offset 0x058 LCD HV Panel Interface Register */
         RESERVED(0x05C[0x0060 - 0x005C], uint8_t)
    __IO uint32_t LCD_CPU_IF_REG;                     /*!< Offset 0x060 LCD CPU Panel Interface Register */
    __IO uint32_t LCD_CPU_WR_REG;                     /*!< Offset 0x064 LCD CPU Panel Write Data Register */
    __IO uint32_t LCD_CPU_RD0_REG;                    /*!< Offset 0x068 LCD CPU Panel Read Data Register0 */
    __IO uint32_t LCD_CPU_RD1_REG;                    /*!< Offset 0x06C LCD CPU Panel Read Data Register1 */
         RESERVED(0x070[0x0084 - 0x0070], uint8_t)
    __IO uint32_t LCD_LVDS_IF_REG;                    /*!< Offset 0x084 LCD LVDS Configure Register */
    __IO uint32_t LCD_IO_POL_REG;                     /*!< Offset 0x088 LCD IO Polarity Register */
    __IO uint32_t LCD_IO_TRI_REG;                     /*!< Offset 0x08C LCD IO Control Register */
         RESERVED(0x090[0x00FC - 0x0090], uint8_t)
    __IO uint32_t LCD_DEBUG_REG;                      /*!< Offset 0x0FC LCD Debug Register */
    __IO uint32_t LCD_CEU_CTL_REG;                    /*!< Offset 0x100 LCD CEU Control Register */
         RESERVED(0x104[0x0110 - 0x0104], uint8_t)
    __IO uint32_t LCD_CEU_COEF_MUL_REG [0x003];       /*!< Offset 0x110 LCD CEU Coefficient Register0 0x0110+N*0x04 (N=0..10) N=0: Rr, N=1: Rg, N=2: Rb, N=4: Gr, N=5: Gg, N=6: Gb, N=8: Br, N=9: Bg, N=10: Bb */
    __IO uint32_t LCD_CEU_COEF_ADD0_REG;              /*!< Offset 0x11C LCD CEU Coefficient Register1 0x011C+N*0x10 (N=0,1,2) N=0: Rc, N=1: Gc, N=2: Bc */
         RESERVED(0x120[0x012C - 0x0120], uint8_t)
    __IO uint32_t LCD_CEU_COEF_ADD1_REG;              /*!< Offset 0x12C LCD CEU Coefficient Register1 0x011C+N*0x10 (N=0,1,2) */
         RESERVED(0x130[0x013C - 0x0130], uint8_t)
    __IO uint32_t LCD_CEU_COEF_ADD2_REG;              /*!< Offset 0x13C LCD CEU Coefficient Register1 0x011C+N*0x10 (N=0,1,2) */
    __IO uint32_t LCD_CEU_COEF_RANG_REG [0x003];      /*!< Offset 0x140 LCD CEU Coefficient Register2 0x0140+N*0x04 (N=0,1,2) */
         RESERVED(0x14C[0x0160 - 0x014C], uint8_t)
    __IO uint32_t LCD_CPU_TRIx_REG [0x006];           /*!< Offset 0x160 LCD CPU Panel Trigger Register0 */
         RESERVED(0x178[0x0180 - 0x0178], uint8_t)
    __IO uint32_t LCD_CMAP_CTL_REG;                   /*!< Offset 0x180 LCD Color Map Control Register */
         RESERVED(0x184[0x0190 - 0x0184], uint8_t)
    __IO uint32_t LCD_CMAP_ODD0_REG;                  /*!< Offset 0x190 LCD Color Map Odd Line Register0 */
    __IO uint32_t LCD_CMAP_ODD1_REG;                  /*!< Offset 0x194 LCD Color Map Odd Line Register1 */
    __IO uint32_t LCD_CMAP_EVEN0_REG;                 /*!< Offset 0x198 LCD Color Map Even Line Register0 */
    __IO uint32_t LCD_CMAP_EVEN1_REG;                 /*!< Offset 0x19C LCD Color Map Even Line Register1 */
         RESERVED(0x1A0[0x01F0 - 0x01A0], uint8_t)
    __IO uint32_t LCD_SAFE_PERIOD_REG;                /*!< Offset 0x1F0 LCD Safe Period Register */
         RESERVED(0x1F4[0x0220 - 0x01F4], uint8_t)
    __IO uint32_t LCD_LVDS_ANA_REG [0x002];           /*!< Offset 0x220 LCD LVDS Analog Register N */
         RESERVED(0x228[0x0230 - 0x0228], uint8_t)
    __IO uint32_t LCD_SYNC_CTL_REG;                   /*!< Offset 0x230 LCD Sync Control Register */
    __IO uint32_t LCD_SYNC_POS_REG;                   /*!< Offset 0x234 LCD Sync Position Register */
    __IO uint32_t LCD_SLAVE_STOP_POS_REG;             /*!< Offset 0x238 LCD Slave Stop Position Register */
         RESERVED(0x23C[0x0400 - 0x023C], uint8_t)
    __IO uint32_t LCD_GAMMA_TABLE_REG [0x100];        /*!< Offset 0x400 LCD Gamma Table Register */
         RESERVED(0x800[0x1000 - 0x0800], uint8_t)
} TCON_LCD_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief TCON_TV
 */
/*!< TCON_TV  */
typedef struct TCON_TV_Type
{
    __IO uint32_t TV_GCTL_REG;                        /*!< Offset 0x000 TV Global Control Register */
    __IO uint32_t TV_GINT0_REG;                       /*!< Offset 0x004 TV Global Interrupt Register0 */
    __IO uint32_t TV_GINT1_REG;                       /*!< Offset 0x008 TV Global Interrupt Register1 */
         RESERVED(0x00C[0x0040 - 0x000C], uint8_t)
    __IO uint32_t TV_SRC_CTL_REG;                     /*!< Offset 0x040 TV Source Control Register */
         RESERVED(0x044[0x0090 - 0x0044], uint8_t)
    __IO uint32_t TV_CTL_REG;                         /*!< Offset 0x090 TV Control Register */
    __IO uint32_t TV_BASIC0_REG;                      /*!< Offset 0x094 TV Basic Timing Register0 */
    __IO uint32_t TV_BASIC1_REG;                      /*!< Offset 0x098 TV Basic Timing Register1 */
    __IO uint32_t TV_BASIC2_REG;                      /*!< Offset 0x09C TV Basic Timing Register2 */
    __IO uint32_t TV_BASIC3_REG;                      /*!< Offset 0x0A0 TV Basic Timing Register3 */
    __IO uint32_t TV_BASIC4_REG;                      /*!< Offset 0x0A4 TV Basic Timing Register4 */
    __IO uint32_t TV_BASIC5_REG;                      /*!< Offset 0x0A8 TV Basic Timing Register5 */
         RESERVED(0x0AC[0x00B8 - 0x00AC], uint8_t)
    __IO uint32_t TV_IO_POL_REG;                      /*!< Offset 0x0B8 TV SYNC Signal Polarity Register was: 0x0088 */
    __IO uint32_t TV_IO_TRI_REG;                      /*!< Offset 0x0BC TV SYNC Signal IO Control Register was: 0x008C */
         RESERVED(0x0C0[0x00FC - 0x00C0], uint8_t)
    __IO uint32_t TV_DEBUG_REG;                       /*!< Offset 0x0FC TV Debug Register */
    __IO uint32_t TV_CEU_CTL_REG;                     /*!< Offset 0x100 TV CEU Control Register */
         RESERVED(0x104[0x0110 - 0x0104], uint8_t)
    __IO uint32_t TV_CEU_COEF_MUL_REG [0x00B];        /*!< Offset 0x110 TV CEU Coefficient Register0 0x0110+N*0x04(N=0-10) */
         RESERVED(0x13C[0x0140 - 0x013C], uint8_t)
    __IO uint32_t TV_CEU_COEF_RANG_REG [0x003];       /*!< Offset 0x140 TV CEU Coefficient Register2 0x0140+N*0x04(N=0-2) */
         RESERVED(0x14C[0x01F0 - 0x014C], uint8_t)
    __IO uint32_t TV_SAFE_PERIOD_REG;                 /*!< Offset 0x1F0 TV Safe Period Register */
         RESERVED(0x1F4[0x0300 - 0x01F4], uint8_t)
    __IO uint32_t TV_FILL_CTL_REG;                    /*!< Offset 0x300 TV Fill Data Control Register */
    struct
    {
        __IO uint32_t TV_FILL_BEGIN_REG;              /*!< Offset 0x304 TV Fill Data Begin Register 0x0304+N*0x0C(N=0–2) */
        __IO uint32_t TV_FILL_END_REG;                /*!< Offset 0x308 TV Fill Data End Register 0x0308+N*0x0C(N=0–2) */
        __IO uint32_t TV_FILL_DATA_REG;               /*!< Offset 0x30C TV Fill Data Value Register 0x030C+N*0x0C(N=0–2) */
    } TV_FILL [0x003];                                /*!< Offset 0x304 TV Fill Channel [0..2] */
         RESERVED(0x328[0x0330 - 0x0328], uint8_t)
    __IO uint32_t TV_DATA_IO_POL0_REG;                /*!< Offset 0x330 TCON Data IO Polarity Control0 */
    __IO uint32_t TV_DATA_IO_POL1_REG;                /*!< Offset 0x334 TCON Data IO Polarity Control1 */
    __IO uint32_t TV_DATA_IO_TRI0_REG;                /*!< Offset 0x338 TCON Data IO Enable Control0 */
    __IO uint32_t TV_DATA_IO_TRI1_REG;                /*!< Offset 0x33C TCON Data IO Enable Control1 */
    __IO uint32_t TV_PIXELDEPTH_MODE_REG;             /*!< Offset 0x340 TV Pixeldepth Mode Control Register */
         RESERVED(0x344[0x1000 - 0x0344], uint8_t)
} TCON_TV_TypeDef; /* size of structure = 0x1000 */
/*
 * @brief THS
 */
/*!< THS Thermal Sensor */
typedef struct THS_Type
{
    __IO uint32_t THS_CTRL;                           /*!< Offset 0x000 THS Control Register */
    __IO uint32_t THS_EN;                             /*!< Offset 0x004 THS Enable Register */
    __IO uint32_t THS_PER;                            /*!< Offset 0x008 THS Period Control Register */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IO uint32_t THS_DATA_INTC;                      /*!< Offset 0x010 THS Data Interrupt Control Register */
    __IO uint32_t THS_SHUT_INTC;                      /*!< Offset 0x014 THS Shut Interrupt Control Register */
    __IO uint32_t THS_ALARM_INTC;                     /*!< Offset 0x018 THS Alarm Interrupt Control Register */
         RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    __IO uint32_t THS_DATA_INTS;                      /*!< Offset 0x020 THS Data Interrupt Status Register */
    __IO uint32_t THS_SHUT_INTS;                      /*!< Offset 0x024 THS Shut Interrupt Status Register */
    __IO uint32_t THS_ALARMO_INTS;                    /*!< Offset 0x028 THS Alarm off Interrupt Status Register */
    __IO uint32_t THS_ALARM_INTS;                     /*!< Offset 0x02C THS Alarm Interrupt Status Register */
    __IO uint32_t THS_FILTER;                         /*!< Offset 0x030 THS Median Filter Control Register */
         RESERVED(0x034[0x0040 - 0x0034], uint8_t)
    __IO uint32_t THS_ALARM_CTRL;                     /*!< Offset 0x040 THS Alarm Threshold Control Register */
         RESERVED(0x044[0x0080 - 0x0044], uint8_t)
    __IO uint32_t THS_SHUTDOWN_CTRL;                  /*!< Offset 0x080 THS Shutdown Threshold Control Register */
         RESERVED(0x084[0x00A0 - 0x0084], uint8_t)
    __IO uint32_t THS_CDATA;                          /*!< Offset 0x0A0 THS Calibration Data */
         RESERVED(0x0A4[0x00C0 - 0x00A4], uint8_t)
    __IO uint32_t THS_DATA;                           /*!< Offset 0x0C0 THS Data Register */
} THS_TypeDef; /* size of structure = 0x0C4 */
/*
 * @brief TIMER
 */
/*!< TIMER  */
typedef struct TIMER_Type
{
    __IO uint32_t TMR_IRQ_EN_REG;                     /*!< Offset 0x000 Timer IRQ Enable Register */
    __IO uint32_t TMR_IRQ_STA_REG;                    /*!< Offset 0x004 Timer Status Register */
         RESERVED(0x008[0x0010 - 0x0008], uint8_t)
    struct
    {
        __IO uint32_t CTRL_REG;                       /*!< Offset 0x010 Timer n Control Register */
        __IO uint32_t INTV_VALUE_REG;                 /*!< Offset 0x014 Timer n Interval Value Register */
        __IO uint32_t CUR_VALUE_REG;                  /*!< Offset 0x018 Timer n Current Value Register */
             RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    } TMR [0x002];                                    /*!< Offset 0x010 Timer */
         RESERVED(0x030[0x00A0 - 0x0030], uint8_t)
    __IO uint32_t WDOG_IRQ_EN_REG;                    /*!< Offset 0x0A0 Watchdog IRQ Enable Register */
    __IO uint32_t WDOG_IRQ_STA_REG;                   /*!< Offset 0x0A4 Watchdog Status Register */
    __IO uint32_t WDOG_SOFT_RST_REG;                  /*!< Offset 0x0A8 Watchdog Software Reset Register */
         RESERVED(0x0AC[0x00B0 - 0x00AC], uint8_t)
    __IO uint32_t WDOG_CTRL_REG;                      /*!< Offset 0x0B0 Watchdog Control Register */
    __IO uint32_t WDOG_CFG_REG;                       /*!< Offset 0x0B4 Watchdog Configuration Register */
    __IO uint32_t WDOG_MODE_REG;                      /*!< Offset 0x0B8 Watchdog Mode Register */
    __IO uint32_t WDOG_OUTPUT_CFG_REG;                /*!< Offset 0x0BC Watchdog Output Configuration Register */
    __IO uint32_t AVS_CNT_CTL_REG;                    /*!< Offset 0x0C0 AVS Control Register */
    __IO uint32_t AVS_CNT0_REG;                       /*!< Offset 0x0C4 AVS Counter 0 Register */
    __IO uint32_t AVS_CNT1_REG;                       /*!< Offset 0x0C8 AVS Counter 1 Register */
    __IO uint32_t AVS_CNT_DIV_REG;                    /*!< Offset 0x0CC AVS Divisor Register */
         RESERVED(0x0D0[0x0400 - 0x00D0], uint8_t)
} TIMER_TypeDef; /* size of structure = 0x400 */
/*
 * @brief TPADC
 */
/*!< TPADC  */
typedef struct TPADC_Type
{
    __IO uint32_t TP_CTRL_REG0;                       /*!< Offset 0x000 TP Control Register 0 */
    __IO uint32_t TP_CTRL_REG1;                       /*!< Offset 0x004 TP Control Register 1 */
    __IO uint32_t TP_CTRL_REG2;                       /*!< Offset 0x008 TP Control Register 2 */
    __IO uint32_t TP_CTRL_REG3;                       /*!< Offset 0x00C TP Control Register 3 */
    __IO uint32_t TP_INT_FIFO_CTRL_REG;               /*!< Offset 0x010 TP Interrupt FIFO Control Register */
    __IO uint32_t TP_INT_FIFO_STAT_REG;               /*!< Offset 0x014 TP Interrupt FIFO Status Register */
         RESERVED(0x018[0x001C - 0x0018], uint8_t)
    __IO uint32_t TP_CALI_DATA_REG;                   /*!< Offset 0x01C TP Calibration Data Register */
         RESERVED(0x020[0x0024 - 0x0020], uint8_t)
    __I  uint32_t TP_DATA_REG;                        /*!< Offset 0x024 TP Data Register */
} TPADC_TypeDef; /* size of structure = 0x028 */
/*
 * @brief TVD0
 */
/*!< TVD0 Video Decoding */
typedef struct TVD0_Type
{
    __IO uint32_t TVD_EN;                             /*!< Offset 0x000 TVD MODULE CONTROL Register */
    __IO uint32_t TVD_MODE;                           /*!< Offset 0x004 TVD MODE CONTROL Register */
    __IO uint32_t TVD_CLAMP_AGC1;                     /*!< Offset 0x008 TVD CLAMP & AGC CONTROL Register1 */
    __IO uint32_t TVD_CLAMP_AGC2;                     /*!< Offset 0x00C TVD CLAMP & AGC CONTROL Register2 */
    __IO uint32_t TVD_HLOCK1;                         /*!< Offset 0x010 TVD HLOCK CONTROL Register1 */
    __IO uint32_t TVD_HLOCK2;                         /*!< Offset 0x014 TVD HLOCK CONTROL Register2 */
    __IO uint32_t TVD_HLOCK3;                         /*!< Offset 0x018 TVD HLOCK CONTROL Register3 */
    __IO uint32_t TVD_HLOCK4;                         /*!< Offset 0x01C TVD HLOCK CONTROL Register4 */
    __IO uint32_t TVD_HLOCK5;                         /*!< Offset 0x020 TVD HLOCK CONTROL Register5 */
    __IO uint32_t TVD_VLOCK1;                         /*!< Offset 0x024 TVD VLOCK CONTROL Register1 */
    __IO uint32_t TVD_VLOCK2;                         /*!< Offset 0x028 TVD VLOCK CONTROL Register2 */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __IO uint32_t TVD_CLOCK1;                         /*!< Offset 0x030 TVD CHROMA LOCK CONTROL Register1 */
    __IO uint32_t TVD_CLOCK2;                         /*!< Offset 0x034 TVD CHROMA LOCK CONTROL Register2 */
         RESERVED(0x038[0x0040 - 0x0038], uint8_t)
    __IO uint32_t TVD_YC_SEP1;                        /*!< Offset 0x040 TVD YC SEPERATION CONROL Register1 */
    __IO uint32_t TVD_YC_SEP2;                        /*!< Offset 0x044 TVD YC SEPERATION CONROL Register2 */
         RESERVED(0x048[0x0050 - 0x0048], uint8_t)
    __IO uint32_t TVD_ENHANCE1;                       /*!< Offset 0x050 TVD ENHANCEMENT CONTROL Register1 */
    __IO uint32_t TVD_ENHANCE2;                       /*!< Offset 0x054 TVD ENHANCEMENT CONTROL Register2 */
    __IO uint32_t TVD_ENHANCE3;                       /*!< Offset 0x058 TVD ENHANCEMENT CONTROL Register3 */
         RESERVED(0x05C[0x0060 - 0x005C], uint8_t)
    __IO uint32_t TVD_WB1;                            /*!< Offset 0x060 TVD WB DMA CONTROL Register1 */
    __IO uint32_t TVD_WB2;                            /*!< Offset 0x064 TVD WB DMA CONTROL Register2 */
    __IO uint32_t TVD_WB3;                            /*!< Offset 0x068 TVD WB DMA CONTROL Register3 */
    __IO uint32_t TVD_WB4;                            /*!< Offset 0x06C TVD WB DMA CONTROL Register4 */
         RESERVED(0x070[0x0080 - 0x0070], uint8_t)
    __IO uint32_t TVD_IRQ_CTL;                        /*!< Offset 0x080 TVD DMA Interrupt Control Register */
         RESERVED(0x084[0x0090 - 0x0084], uint8_t)
    __IO uint32_t TVD_IRQ_STATUS;                     /*!< Offset 0x090 TVD DMA Interrupt Status Register */
         RESERVED(0x094[0x0100 - 0x0094], uint8_t)
    __IO uint32_t TVD_DEBUG1;                         /*!< Offset 0x100 TVD DEBUG CONTROL Register1 */
         RESERVED(0x104[0x0180 - 0x0104], uint8_t)
    __IO uint32_t TVD_STATUS1;                        /*!< Offset 0x180 TVD DEBUG STATUS Register1 */
    __IO uint32_t TVD_STATUS2;                        /*!< Offset 0x184 TVD DEBUG STATUS Register2 */
    __IO uint32_t TVD_STATUS3;                        /*!< Offset 0x188 TVD DEBUG STATUS Register3 */
    __IO uint32_t TVD_STATUS4;                        /*!< Offset 0x18C TVD DEBUG STATUS Register4 */
    __IO uint32_t TVD_STATUS5;                        /*!< Offset 0x190 TVD DEBUG STATUS Register5 */
    __IO uint32_t TVD_STATUS6;                        /*!< Offset 0x194 TVD DEBUG STATUS Register6 */
} TVD0_TypeDef; /* size of structure = 0x198 */
/*
 * @brief TVD_TOP
 */
/*!< TVD_TOP Video Decoding */
typedef struct TVD_TOP_Type
{
    __IO uint32_t TVD_TOP_MAP;                        /*!< Offset 0x000 TVD TOP MAP Register */
         RESERVED(0x004[0x0008 - 0x0004], uint8_t)
    __IO uint32_t TVD_3D_CTL1;                        /*!< Offset 0x008 TVD 3D DMA CONTROL Register1 */
    __IO uint32_t TVD_3D_CTL2;                        /*!< Offset 0x00C TVD 3D DMA CONTROL Register2 */
    __IO uint32_t TVD_3D_CTL3;                        /*!< Offset 0x010 TVD 3D DMA CONTROL Register3 */
    __IO uint32_t TVD_3D_CTL4;                        /*!< Offset 0x014 TVD 3D DMA CONTROL Register4 */
    __IO uint32_t TVD_3D_CTL5;                        /*!< Offset 0x018 TVD 3D DMA CONTROL Register5 */
         RESERVED(0x01C[0x0020 - 0x001C], uint8_t)
    struct
    {
             RESERVED(0x000[0x0004 - 0x0000], uint8_t)
        __IO uint32_t TVD_TOP_CTL;                    /*!< Offset 0x024 TVD TOP CONTROL Register */
        __IO uint32_t TVD_ADC_CTL;                    /*!< Offset 0x028 TVD ADC CONTROL Register */
        __IO uint32_t TVD_ADC_CFG;                    /*!< Offset 0x02C TVD ADC CONFIGURATION Register */
             RESERVED(0x010[0x0020 - 0x0010], uint8_t)
    } TVD_ADC [0x004];                                /*!< Offset 0x020 TVD ADC Registers N (N = 0 to 3) */
} TVD_TOP_TypeDef; /* size of structure = 0x0A0 */
/*
 * @brief TVE_TOP
 */
/*!< TVE_TOP TV Output TV Encoder (display out interface = CVBS OUT) */
typedef struct TVE_TOP_Type
{
         RESERVED(0x000[0x0020 - 0x0000], uint8_t)
    __IO uint32_t TVE_DAC_MAP;                        /*!< Offset 0x020 TV Encoder DAC MAP Register */
    __IO uint32_t TVE_DAC_STATUS;                     /*!< Offset 0x024 TV Encoder DAC STAUTS Register */
    struct
    {
        __IO uint32_t TVE_DAC_CFG0;                   /*!< Offset 0x028 TV Encoder DAC CFG0 Register */
        __IO uint32_t TVE_DAC_CFG1;                   /*!< Offset 0x02C TV Encoder DAC CFG1 Register */
        __IO uint32_t TVE_DAC_CFG2;                   /*!< Offset 0x030 TV Encoder DAC CFG2 Register */
        __IO uint32_t TVE_DAC_CFG3;                   /*!< Offset 0x034 TV Encoder DAC CFG3 Register */
             RESERVED(0x010[0x0020 - 0x0010], uint8_t)
    } CH [0x001];                                     /*!< Offset 0x028 Channel [0..?] */
         RESERVED(0x048[0x00F0 - 0x0048], uint8_t)
    __IO uint32_t TVE_DAC_TEST;                       /*!< Offset 0x0F0 TV Encoder DAC TEST Register */
} TVE_TOP_TypeDef; /* size of structure = 0x0F4 */
/*
 * @brief TV_Encoder
 */
/*!< TV_Encoder TV Encoder (display out interface = CVBS OUT) */
typedef struct TV_Encoder_Type
{
    __IO uint32_t TVE_000_REG;                        /*!< Offset 0x000 TV Encoder Clock Gating Register */
    __IO uint32_t TVE_004_REG;                        /*!< Offset 0x004 TV Encoder Configuration Register */
    __IO uint32_t TVE_008_REG;                        /*!< Offset 0x008 TV Encoder DAC Register1 */
    __IO uint32_t TVE_00C_REG;                        /*!< Offset 0x00C TV Encoder Notch and DAC Delay Register */
    __IO uint32_t TVE_010_REG;                        /*!< Offset 0x010 TV Encoder Chroma Frequency Register */
    __IO uint32_t TVE_014_REG;                        /*!< Offset 0x014 TV Encoder Front/Back Porch Register */
    __IO uint32_t TVE_018_REG;                        /*!< Offset 0x018 TV Encoder HD Mode VSYNC Register */
    __IO uint32_t TVE_01C_REG;                        /*!< Offset 0x01C TV Encoder Line Number Register */
    __IO uint32_t TVE_020_REG;                        /*!< Offset 0x020 TV Encoder Level Register */
    __IO uint32_t TVE_024_REG;                        /*!< Offset 0x024 TV Encoder DAC Register2 */
         RESERVED(0x028[0x0030 - 0x0028], uint8_t)
    __IO uint32_t TVE_030_REG;                        /*!< Offset 0x030 TV Encoder Auto Detection Enable Register */
    __IO uint32_t TVE_034_REG;                        /*!< Offset 0x034 TV Encoder Auto Detection Interrupt Status Register */
    __IO uint32_t TVE_038_REG;                        /*!< Offset 0x038 TV Encoder Auto Detection Status Register */
    __IO uint32_t TVE_03C_REG;                        /*!< Offset 0x03C TV Encoder Auto Detection De-bounce Setting Register */
         RESERVED(0x040[0x00F8 - 0x0040], uint8_t)
    __IO uint32_t TVE_0F8_REG;                        /*!< Offset 0x0F8 TV Encoder Auto Detect Configuration Register0 */
    __IO uint32_t TVE_0FC_REG;                        /*!< Offset 0x0FC TV Encoder Auto Detect Configuration Register1 */
    __IO uint32_t TVE_100_REG;                        /*!< Offset 0x100 TV Encoder Color Burst Phase Reset Configuration Register */
    __IO uint32_t TVE_104_REG;                        /*!< Offset 0x104 TV Encoder VSYNC Number Register */
    __IO uint32_t TVE_108_REG;                        /*!< Offset 0x108 TV Encoder Notch Filter Frequency Register */
    __IO uint32_t TVE_10C_REG;                        /*!< Offset 0x10C TV Encoder Cb/Cr Level/Gain Register */
    __IO uint32_t TVE_110_REG;                        /*!< Offset 0x110 TV Encoder Tint and Color Burst Phase Register */
    __IO uint32_t TVE_114_REG;                        /*!< Offset 0x114 TV Encoder Burst Width Register */
    __IO uint32_t TVE_118_REG;                        /*!< Offset 0x118 TV Encoder Cb/Cr Gain Register */
    __IO uint32_t TVE_11C_REG;                        /*!< Offset 0x11C TV Encoder Sync and VBI Level Register */
    __IO uint32_t TVE_120_REG;                        /*!< Offset 0x120 TV Encoder White Level Register */
    __IO uint32_t TVE_124_REG;                        /*!< Offset 0x124 TV Encoder Video Active Line Register */
    __IO uint32_t TVE_128_REG;                        /*!< Offset 0x128 TV Encoder Video Chroma BW and CompGain Register */
    __IO uint32_t TVE_12C_REG;                        /*!< Offset 0x12C TV Encoder Register */
    __IO uint32_t TVE_130_REG;                        /*!< Offset 0x130 TV Encoder Re-sync Parameters Register */
    __IO uint32_t TVE_134_REG;                        /*!< Offset 0x134 TV Encoder Slave Parameter Register */
    __IO uint32_t TVE_138_REG;                        /*!< Offset 0x138 TV Encoder Configuration Register0 */
    __IO uint32_t TVE_13C_REG;                        /*!< Offset 0x13C TV Encoder Configuration Register1 */
         RESERVED(0x140[0x0380 - 0x0140], uint8_t)
    __IO uint32_t TVE_380_REG;                        /*!< Offset 0x380 TV Encoder Low Pass Control Register */
    __IO uint32_t TVE_384_REG;                        /*!< Offset 0x384 TV Encoder Low Pass Filter Control Register */
    __IO uint32_t TVE_388_REG;                        /*!< Offset 0x388 TV Encoder Low Pass Gain Register */
    __IO uint32_t TVE_38C_REG;                        /*!< Offset 0x38C TV Encoder Low Pass Gain Control Register */
    __IO uint32_t TVE_390_REG;                        /*!< Offset 0x390 TV Encoder Low Pass Shoot Control Register */
    __IO uint32_t TVE_394_REG;                        /*!< Offset 0x394 TV Encoder Low Pass Coring Register */
         RESERVED(0x398[0x03A0 - 0x0398], uint8_t)
    __IO uint32_t TVE_3A0_REG;                        /*!< Offset 0x3A0 TV Encoder Noise Reduction Register */
} TV_Encoder_TypeDef; /* size of structure = 0x3A4 */
/*
 * @brief TWI
 */
/*!< TWI  */
typedef struct TWI_Type
{
    __IO uint32_t TWI_ADDR;                           /*!< Offset 0x000 TWI Slave Address Register */
    __IO uint32_t TWI_XADDR;                          /*!< Offset 0x004 TWI Extended Slave Address Register */
    __IO uint32_t TWI_DATA;                           /*!< Offset 0x008 TWI Data Byte Register */
    __IO uint32_t TWI_CNTR;                           /*!< Offset 0x00C TWI Control Register */
    __IO uint32_t TWI_STAT;                           /*!< Offset 0x010 TWI Status Register */
    __IO uint32_t TWI_CCR;                            /*!< Offset 0x014 TWI Clock Control Register */
    __IO uint32_t TWI_SRST;                           /*!< Offset 0x018 TWI Software Reset Register */
    __IO uint32_t TWI_EFR;                            /*!< Offset 0x01C TWI Enhance Feature Register */
    __IO uint32_t TWI_LCR;                            /*!< Offset 0x020 TWI Line Control Register */
         RESERVED(0x024[0x0200 - 0x0024], uint8_t)
    __IO uint32_t TWI_DRV_CTRL;                       /*!< Offset 0x200 TWI_DRV Control Register */
    __IO uint32_t TWI_DRV_CFG;                        /*!< Offset 0x204 TWI_DRV Transmission Configuration Register */
    __IO uint32_t TWI_DRV_SLV;                        /*!< Offset 0x208 TWI_DRV Slave ID Register */
    __IO uint32_t TWI_DRV_FMT;                        /*!< Offset 0x20C TWI_DRV Packet Format Register */
    __IO uint32_t TWI_DRV_BUS_CTRL;                   /*!< Offset 0x210 TWI_DRV Bus Control Register */
    __IO uint32_t TWI_DRV_INT_CTRL;                   /*!< Offset 0x214 TWI_DRV Interrupt Control Register */
    __IO uint32_t TWI_DRV_DMA_CFG;                    /*!< Offset 0x218 TWI_DRV DMA Configure Register */
    __IO uint32_t TWI_DRV_FIFO_CON;                   /*!< Offset 0x21C TWI_DRV FIFO Content Register */
         RESERVED(0x220[0x0300 - 0x0220], uint8_t)
    __IO uint32_t TWI_DRV_SEND_FIFO_ACC;              /*!< Offset 0x300 TWI_DRV Send Data FIFO Access Register */
    __IO uint32_t TWI_DRV_RECV_FIFO_ACC;              /*!< Offset 0x304 TWI_DRV Receive Data FIFO Access Register */
         RESERVED(0x308[0x0400 - 0x0308], uint8_t)
} TWI_TypeDef; /* size of structure = 0x400 */
/*
 * @brief TZMA
 */
/*!< TZMA  */
typedef struct TZMA_Type
{
         RESERVED(0x000[0x0400 - 0x0000], uint8_t)
} TZMA_TypeDef; /* size of structure = 0x400 */
/*
 * @brief UART
 */
/*!< UART  */
typedef struct UART_Type
{
    __IO uint32_t UART_RBR_THR_DLL;                   /*!< Offset 0x000 UART Receive Buffer Register/Transmit Holding Register */
    __IO uint32_t UART_DLH_IER;                       /*!< Offset 0x004  */
    __IO uint32_t UART_IIR_FCR;                       /*!< Offset 0x008 UART Interrupt Identity Register/UART FIFO Control Register */
    __IO uint32_t UART_LCR;                           /*!< Offset 0x00C UART Line Control Register */
    __IO uint32_t UART_MCR;                           /*!< Offset 0x010 UART Modem Control Register */
    __IO uint32_t UART_LSR;                           /*!< Offset 0x014 UART Line Status Register */
    __IO uint32_t UART_MSR;                           /*!< Offset 0x018 UART Modem Status Register */
    __IO uint32_t UART_SCH;                           /*!< Offset 0x01C UART Scratch Register */
         RESERVED(0x020[0x007C - 0x0020], uint8_t)
    __IO uint32_t UART_USR;                           /*!< Offset 0x07C UART Status Register */
    __IO uint32_t UART_TFL;                           /*!< Offset 0x080 UART Transmit FIFO Level Register */
    __IO uint32_t UART_RFL;                           /*!< Offset 0x084 UART Receive FIFO Level Register */
    __IO uint32_t UART_HSK;                           /*!< Offset 0x088 UART DMA Handshake Configuration Register */
    __IO uint32_t UART_DMA_REQ_EN;                    /*!< Offset 0x08C UART DMA Request Enable Register */
         RESERVED(0x090[0x00A4 - 0x0090], uint8_t)
    __IO uint32_t UART_HALT;                          /*!< Offset 0x0A4 UART Halt TX Register */
         RESERVED(0x0A8[0x00B0 - 0x00A8], uint8_t)
    __IO uint32_t UART_DBG_DLL;                       /*!< Offset 0x0B0 UART Debug DLL Register */
    __IO uint32_t UART_DBG_DLH;                       /*!< Offset 0x0B4 UART Debug DLH Register */
         RESERVED(0x0B8[0x00F0 - 0x00B8], uint8_t)
    __IO uint32_t UART_A_FCC;                         /*!< Offset 0x0F0 UART FIFO Clock Control Register */
         RESERVED(0x0F4[0x0100 - 0x00F4], uint8_t)
    __IO uint32_t UART_A_RXDMA_CTRL;                  /*!< Offset 0x100 UART RXDMA Control Register */
    __IO uint32_t UART_A_RXDMA_STR;                   /*!< Offset 0x104 UART RXDMA Start Register */
    __IO uint32_t UART_A_RXDMA_STA;                   /*!< Offset 0x108 UART RXDMA Status Register */
    __IO uint32_t UART_A_RXDMA_LMT;                   /*!< Offset 0x10C UART RXDMA Limit Register */
    __IO uint32_t UART_A_RXDMA_SADDRL;                /*!< Offset 0x110 UART RXDMA Buffer Start Address Low Register */
    __IO uint32_t UART_A_RXDMA_SADDRH;                /*!< Offset 0x114 UART RXDMA Buffer Start Address High Register */
    __IO uint32_t UART_A_RXDMA_BL;                    /*!< Offset 0x118 UART RXDMA Buffer Length Register */
         RESERVED(0x11C[0x0120 - 0x011C], uint8_t)
    __IO uint32_t UART_A_RXDMA_IE;                    /*!< Offset 0x120 UART RXDMA Interrupt Enable Register */
    __IO uint32_t UART_A_RXDMA_IS;                    /*!< Offset 0x124 UART RXDMA Interrupt Status Register */
    __IO uint32_t UART_A_RXDMA_WADDRL;                /*!< Offset 0x128 UART RXDMA Write Address Low Register */
    __IO uint32_t UART_A_RXDMA_WADDRH;                /*!< Offset 0x12C UART RXDMA Write Address high Register */
    __IO uint32_t UART_A_RXDMA_RADDRL;                /*!< Offset 0x130 UART RXDMA Read Address Low Register */
    __IO uint32_t UART_A_RXDMA_RADDRH;                /*!< Offset 0x134 UART RXDMA Read Address high Register */
    __IO uint32_t UART_A_RXDMA_DCNT;                  /*!< Offset 0x138 UART RADMA Data Count Register */
         RESERVED(0x13C[0x0400 - 0x013C], uint8_t)
} UART_TypeDef; /* size of structure = 0x400 */
/*
 * @brief USBEHCI
 */
/*!< USBEHCI  */
typedef struct USBEHCI_Type
{
    __IO uint16_t E_CAPLENGTH;                        /*!< Offset 0x000 EHCI Capability Register Length Register */
    __IO uint16_t E_HCIVERSION;                       /*!< Offset 0x002 EHCI Host Interface Version Number Register */
    __IO uint32_t E_HCSPARAMS;                        /*!< Offset 0x004 EHCI Host Control Structural Parameter Register */
    __IO uint32_t E_HCCPARAMS;                        /*!< Offset 0x008 EHCI Host Control Capability Parameter Register */
    __IO uint32_t E_HCSPPORTROUTE;                    /*!< Offset 0x00C EHCI Companion Port Route Description */
    __IO uint32_t E_USBCMD;                           /*!< Offset 0x010 EHCI USB Command Register */
    __IO uint32_t E_USBSTS;                           /*!< Offset 0x014 EHCI USB Status Register */
    __IO uint32_t E_USBINTR;                          /*!< Offset 0x018 EHCI USB Interrupt Enable Register */
    __IO uint32_t E_FRINDEX;                          /*!< Offset 0x01C EHCI USB Frame Index Register */
    __IO uint32_t E_CTRLDSSEGMENT;                    /*!< Offset 0x020 EHCI 4G Segment Selector Register */
    __IO uint32_t E_PERIODICLISTBASE;                 /*!< Offset 0x024 EHCI Frame List Base Address Register */
    __IO uint32_t E_ASYNCLISTADDR;                    /*!< Offset 0x028 EHCI Next Asynchronous List Address Register */
         RESERVED(0x02C[0x0050 - 0x002C], uint8_t)
    __IO uint32_t E_CONFIGFLAG;                       /*!< Offset 0x050 EHCI Configured Flag Register */
    __IO uint32_t E_PORTSC;                           /*!< Offset 0x054 EHCI Port Status/Control Register */
         RESERVED(0x058[0x0400 - 0x0058], uint8_t)
    __IO uint32_t O_HcRevision;                       /*!< Offset 0x400 OHCI Revision Register (not documented) */
    __IO uint32_t O_HcControl;                        /*!< Offset 0x404 OHCI Control Register */
    __IO uint32_t O_HcCommandStatus;                  /*!< Offset 0x408 OHCI Command Status Register */
    __IO uint32_t O_HcInterruptStatus;                /*!< Offset 0x40C OHCI Interrupt Status Register */
    __IO uint32_t O_HcInterruptEnable;                /*!< Offset 0x410 OHCI Interrupt Enable Register */
    __IO uint32_t O_HcInterruptDisable;               /*!< Offset 0x414 OHCI Interrupt Disable Register */
    __IO uint32_t O_HcHCCA;                           /*!< Offset 0x418 OHCI HCCA Base */
    __IO uint32_t O_HcPeriodCurrentED;                /*!< Offset 0x41C OHCI Period Current ED Base */
    __IO uint32_t O_HcControlHeadED;                  /*!< Offset 0x420 OHCI Control Head ED Base */
    __IO uint32_t O_HcControlCurrentED;               /*!< Offset 0x424 OHCI Control Current ED Base */
    __IO uint32_t O_HcBulkHeadED;                     /*!< Offset 0x428 OHCI Bulk Head ED Base */
    __IO uint32_t O_HcBulkCurrentED;                  /*!< Offset 0x42C OHCI Bulk Current ED Base */
    __IO uint32_t O_HcDoneHead;                       /*!< Offset 0x430 OHCI Done Head Base */
    __IO uint32_t O_HcFmInterval;                     /*!< Offset 0x434 OHCI Frame Interval Register */
    __IO uint32_t O_HcFmRemaining;                    /*!< Offset 0x438 OHCI Frame Remaining Register */
    __IO uint32_t O_HcFmNumber;                       /*!< Offset 0x43C OHCI Frame Number Register */
    __IO uint32_t O_HcPerioddicStart;                 /*!< Offset 0x440 OHCI Periodic Start Register */
    __IO uint32_t O_HcLSThreshold;                    /*!< Offset 0x444 OHCI LS Threshold Register */
    __IO uint32_t O_HcRhDescriptorA;                  /*!< Offset 0x448 OHCI Root Hub Descriptor Register A */
    __IO uint32_t O_HcRhDesriptorB;                   /*!< Offset 0x44C OHCI Root Hub Descriptor Register B */
    __IO uint32_t O_HcRhStatus;                       /*!< Offset 0x450 OHCI Root Hub Status Register */
    __IO uint32_t O_HcRhPortStatus [0x001];           /*!< Offset 0x454 OHCI Root Hub Port Status Register */
} USBEHCI_TypeDef; /* size of structure = 0x458 */
/*
 * @brief USBOTG
 */
/*!< USBOTG  */
typedef struct USBOTG_Type
{
    __IO uint32_t USB_EPFIFO [0x006];                 /*!< Offset 0x000 USB_EPFIFO [0..5] USB FIFO Entry for Endpoint N */
         RESERVED(0x018[0x0040 - 0x0018], uint8_t)
    __IO uint32_t USB_GCS;                            /*!< Offset 0x040 USB_POWER, USB_DEVCTL, USB_EPINDEX, USB_DMACTL USB Global Control and Status Register */
    __IO uint16_t USB_INTTX;                          /*!< Offset 0x044 USB_INTTX USB_EPINTF USB Endpoint Interrupt Flag Register */
    __IO uint16_t USB_INTRX;                          /*!< Offset 0x046 USB_INTRX USB_EPINTF */
    __IO uint16_t USB_INTTXE;                         /*!< Offset 0x048 USB_INTTXE USB_EPINTE USB Endpoint Interrupt Enable Register */
    __IO uint16_t USB_INTRXE;                         /*!< Offset 0x04A USB_INTRXE USB_EPINTE */
    __IO uint32_t USB_INTUSB;                         /*!< Offset 0x04C USB_INTUSB USB_BUSINTF USB Bus Interrupt Flag Register */
    __IO uint32_t USB_INTUSBE;                        /*!< Offset 0x050 USB_INTUSBE USB_BUSINTE USB Bus Interrupt Enable Register */
    __IO uint32_t USB_FNUM;                           /*!< Offset 0x054 USB Frame Number Register */
         RESERVED(0x058[0x007C - 0x0058], uint8_t)
    __IO uint32_t USB_TESTC;                          /*!< Offset 0x07C USB_TESTC USB Test Control Register */
    __IO uint16_t USB_TXMAXP;                         /*!< Offset 0x080 USB_TXMAXP USB EP1~5 Tx Control and Status Register */
    __IO uint16_t USB_TXCSRHI;                        /*!< Offset 0x082 [15:8]: USB_TXCSRH, [7:0]: USB_TXCSRL */
    __IO uint16_t USB_RXMAXP;                         /*!< Offset 0x084 USB_RXMAXP USB EP1~5 Rx Control and Status Register */
    __IO uint16_t USB_RXCSRHI;                        /*!< Offset 0x086 USB_RXCSR */
    __IO uint16_t USB_RXCOUNT;                        /*!< Offset 0x088 USB_RXCOUNT */
    __IO uint16_t USB_RXPKTCNT;                       /*!< Offset 0x08A USB_RXPKTCNT */
    __IO uint32_t USB_EPATTR;                         /*!< Offset 0x08C USB_EPATTR USB EP0 Attribute Register, USB EP1~5 Attribute Register */
    __IO uint32_t USB_TXFIFO;                         /*!< Offset 0x090 USB_TXFIFO (bits 28:16 Start address of the endpoint FIFO is in units of 8 bytes) */
    __IO uint32_t USB_RXFIFO;                         /*!< Offset 0x094 USB_RXFIFO (bits 28:16 Start address of the endpoint FIFO is in units of 8 bytes) */
    struct
    {
        __IO uint16_t USB_TXFADDR;                    /*!< Offset 0x098 USB_TXFADDR */
        __IO uint8_t  USB_TXHADDR;                    /*!< Offset 0x09A USB_TXHADDR */
        __IO uint8_t  USB_TXHUBPORT;                  /*!< Offset 0x09B USB_TXHUBPORT */
        __IO uint8_t  USB_RXFADDR;                    /*!< Offset 0x09C USB_RXFADDR */
             RESERVED(0x005[0x0006 - 0x0005], uint8_t)
        __IO uint8_t  USB_RXHADDR;                    /*!< Offset 0x09E USB_RXHADDR */
        __IO uint8_t  USB_RXHUBPORT;                  /*!< Offset 0x09F USB_RXHUBPORT */
    } FIFO [0x010];                                   /*!< Offset 0x098 FIFOs [0..5] */
         RESERVED(0x118[0x0400 - 0x0118], uint8_t)
    __IO uint32_t USB_ISCR;                           /*!< Offset 0x400 HCI Interface Register (HCI_Interface) */
    __IO uint32_t USBPHY_PHYCTL;                      /*!< Offset 0x404 USBPHY_PHYCTL */
    __IO uint32_t HCI_CTRL3;                          /*!< Offset 0x408 HCI Control 3 Register (bist) */
         RESERVED(0x40C[0x0410 - 0x040C], uint8_t)
    __IO uint32_t PHY_CTRL;                           /*!< Offset 0x410 PHY Control Register (PHY_Control) */
         RESERVED(0x414[0x0420 - 0x0414], uint8_t)
    __IO uint32_t PHY_OTGCTL;                         /*!< Offset 0x420 Control PHY routing to EHCI or OTG */
    __IO uint32_t PHY_STATUS;                         /*!< Offset 0x424 PHY Status Register */
    __IO uint32_t USB_SPDCR;                          /*!< Offset 0x428 HCI SIE Port Disable Control Register */
         RESERVED(0x42C[0x0500 - 0x042C], uint8_t)
    __IO uint32_t USB_DMA_INTE;                       /*!< Offset 0x500 USB DMA Interrupt Enable Register */
    __IO uint32_t USB_DMA_INTS;                       /*!< Offset 0x504 USB DMA Interrupt Status Register */
         RESERVED(0x508[0x0540 - 0x0508], uint8_t)
    struct
    {
        __IO uint32_t CHAN_CFG;                       /*!< Offset 0x540 USB DMA Channel Configuration Register */
        __IO uint32_t SDRAM_ADD;                      /*!< Offset 0x544 USB DMA SDRAM Start Address Register  */
        __IO uint32_t BC;                             /*!< Offset 0x548 USB DMA Byte Counter Register */
        __I  uint32_t RESIDUAL_BC;                    /*!< Offset 0x54C USB DMA RESIDUAL Byte Counter Register */
    } USB_DMA [0x008];                                /*!< Offset 0x540  */
} USBOTG_TypeDef; /* size of structure = 0x5C0 */
/*
 * @brief USBPHYC
 */
/*!< USBPHYC  */
typedef struct USBPHYC_Type
{
    __IO uint32_t USB_CTRL;                           /*!< Offset 0x000 HCI Interface Register (HCI_Interface) */
    __IO uint32_t USBPHY_PHYCTL;                      /*!< Offset 0x004 USBPHY_PHYCTL */
    __IO uint32_t HCI_CTRL3;                          /*!< Offset 0x008 HCI Control 3 Register (bist) */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IO uint32_t PHY_CTRL;                           /*!< Offset 0x010 PHY Control Register (PHY_Control) */
         RESERVED(0x014[0x0020 - 0x0014], uint8_t)
    __IO uint32_t PHY_OTGCTL;                         /*!< Offset 0x020 Control PHY routing to EHCI or OTG */
    __IO uint32_t PHY_STATUS;                         /*!< Offset 0x024 PHY Status Register */
    __IO uint32_t USB_SPDCR;                          /*!< Offset 0x028 HCI SIE Port Disable Control Register */
} USBPHYC_TypeDef; /* size of structure = 0x02C */
/*
 * @brief USB_EHCI_Capability
 */
/*!< USB_EHCI_Capability  */
typedef struct USB_EHCI_Capability_Type
{
    __I  uint32_t HCCAPBASE;                          /*!< Offset 0x000 EHCI Capability Register (HCIVERSION and CAPLENGTH) register */
    __I  uint32_t HCSPARAMS;                          /*!< Offset 0x004 EHCI Host Control Structural Parameter Register */
    __I  uint32_t HCCPARAMS;                          /*!< Offset 0x008 EHCI Host Control Capability Parameter Register */
    __IO uint32_t HCSPPORTROUTE;                      /*!< Offset 0x00C EHCI Companion Port Route Description */
    __IO uint32_t USBCMD;                             /*!< Offset 0x010 EHCI USB Command Register */
    __IO uint32_t USBSTS;                             /*!< Offset 0x014 EHCI USB Status Register */
    __IO uint32_t USBINTR;                            /*!< Offset 0x018 EHCI USB Interrupt Enable Register */
    __IO uint32_t FRINDEX;                            /*!< Offset 0x01C EHCI USB Frame Index Register */
    __IO uint32_t CTRLDSSEGMENT;                      /*!< Offset 0x020 EHCI 4G Segment Selector Register */
    __IO uint32_t PERIODICLISTBASE;                   /*!< Offset 0x024 EHCI Frame List Base Address Register */
    __IO uint32_t ASYNCLISTADDR;                      /*!< Offset 0x028 EHCI Next Asynchronous List Address Register */
} USB_EHCI_Capability_TypeDef; /* size of structure = 0x02C */
/*
 * @brief USB_OHCI_Capability
 */
/*!< USB_OHCI_Capability  */
typedef struct USB_OHCI_Capability_Type
{
    __IO uint32_t O_HcRevision;                       /*!< Offset 0x000 OHCI Revision Register (not documented) */
    __IO uint32_t O_HcControl;                        /*!< Offset 0x004 OHCI Control Register */
    __IO uint32_t O_HcCommandStatus;                  /*!< Offset 0x008 OHCI Command Status Register */
    __IO uint32_t O_HcInterruptStatus;                /*!< Offset 0x00C OHCI Interrupt Status Register */
    __IO uint32_t O_HcInterruptEnable;                /*!< Offset 0x010 OHCI Interrupt Enable Register */
    __IO uint32_t O_HcInterruptDisable;               /*!< Offset 0x014 OHCI Interrupt Disable Register */
    __IO uint32_t O_HcHCCA;                           /*!< Offset 0x018 OHCI HCCA Base */
    __IO uint32_t O_HcPeriodCurrentED;                /*!< Offset 0x01C OHCI Period Current ED Base */
    __IO uint32_t O_HcControlHeadED;                  /*!< Offset 0x020 OHCI Control Head ED Base */
    __IO uint32_t O_HcControlCurrentED;               /*!< Offset 0x024 OHCI Control Current ED Base */
    __IO uint32_t O_HcBulkHeadED;                     /*!< Offset 0x028 OHCI Bulk Head ED Base */
    __IO uint32_t O_HcBulkCurrentED;                  /*!< Offset 0x02C OHCI Bulk Current ED Base */
    __IO uint32_t O_HcDoneHead;                       /*!< Offset 0x030 OHCI Done Head Base */
    __IO uint32_t O_HcFmInterval;                     /*!< Offset 0x034 OHCI Frame Interval Register */
    __IO uint32_t O_HcFmRemaining;                    /*!< Offset 0x038 OHCI Frame Remaining Register */
    __IO uint32_t O_HcFmNumber;                       /*!< Offset 0x03C OHCI Frame Number Register */
    __IO uint32_t O_HcPerioddicStart;                 /*!< Offset 0x040 OHCI Periodic Start Register */
    __IO uint32_t O_HcLSThreshold;                    /*!< Offset 0x044 OHCI LS Threshold Register */
    __IO uint32_t O_HcRhDescriptorA;                  /*!< Offset 0x048 OHCI Root Hub Descriptor Register A */
    __IO uint32_t O_HcRhDesriptorB;                   /*!< Offset 0x04C OHCI Root Hub Descriptor Register B */
    __IO uint32_t O_HcRhStatus;                       /*!< Offset 0x050 OHCI Root Hub Status Register */
    __IO uint32_t O_HcRhPortStatus [0x001];           /*!< Offset 0x054 OHCI Root Hub Port Status Register */
} USB_OHCI_Capability_TypeDef; /* size of structure = 0x058 */
/*
 * @brief VE
 */
/*!< VE Video Encoding */
typedef struct VE_Type
{
    __IO uint32_t VE_CTRL;                            /*!< Offset 0x000 Sub-Engine Select and RAM type select */
    __IO uint32_t VE_RESET;                           /*!< Offset 0x004 Sub-Engines Reset */
    __IO uint32_t VE_CYCLES_COUNTER;                  /*!< Offset 0x008 Clock Cycles counter */
    __IO uint32_t VE_TIMEOUT;                         /*!< Offset 0x00C VE Timeout value */
    __IO uint32_t VE_MMCREQ_WNUM;                     /*!< Offset 0x010  */
    __IO uint32_t VE_CACHEREG_WNUM;                   /*!< Offset 0x014  */
         RESERVED(0x018[0x001C - 0x0018], uint8_t)
    __IO uint32_t VE_STATUS;                          /*!< Offset 0x01C Busy status */
    __IO uint32_t VE_RDDATA_COUNTER;                  /*!< Offset 0x020 DRAM Read counter */
    __IO uint32_t VE_WRDATA_COUNTER;                  /*!< Offset 0x024 DRAM Write counter */
    __IO uint32_t VE_ANAGLYPH_CTRL;                   /*!< Offset 0x028 Anaglyph mode control */
         RESERVED(0x02C[0x0030 - 0x002C], uint8_t)
    __IO uint32_t VE_MAF_CTRL;                        /*!< Offset 0x030 Motion adaptive filter config */
    __IO uint32_t VE_MAF_CLIP_TH;                     /*!< Offset 0x034  */
    __IO uint32_t VE_MAFREF1_LUMA_BUF;                /*!< Offset 0x038 Reference luma buffer {unsure} */
    __IO uint32_t VE_MAFREF1_CHROMA_BUF;              /*!< Offset 0x03C Reference chroma buffer {unsure} */
    __IO uint32_t VE_MAFCUR_ADDR;                     /*!< Offset 0x040 current maf output address {unsure} */
    __IO uint32_t VE_MAFREF1_ADDR;                    /*!< Offset 0x044 reference maf input address {unsure} */
    __IO uint32_t VE_MAFREF2_ADDR;                    /*!< Offset 0x048 second reference maf input address {unsure} */
    __IO uint32_t VE_MAFDIFF_GROUP_MAX;               /*!< Offset 0x04C  */
    __IO uint32_t VE_IPD_DBLK_BUF_CTRL;               /*!< Offset 0x050 deblocking and intra prediction dram buffer config register (required for A13+ SoC for H264 decoding or on A10 for video with width >= 2048) */
    __IO uint32_t VE_IPD_BUF;                         /*!< Offset 0x054 Intra prediction buffer (needed on A13+ or (width >= 2048)) */
    __IO uint32_t VE_DBLK_BUF;                        /*!< Offset 0x058 Deblocking buffer (needed on A13+ or (width >= 2048)) */
    __IO uint32_t VE_ARGB_QUEUE_START;                /*!< Offset 0x05C ARGB command queue */
    __IO uint32_t VE_ARGB_BLK_SRC1_ADDR;              /*!< Offset 0x060 ARGB source 1 address */
    __IO uint32_t VE_ARGB_BLK_SRC2_ADDR;              /*!< Offset 0x064 ARGB source 2 addres */
    __IO uint32_t VE_ARGB_BLK_DST_ADDR;               /*!< Offset 0x068 ARGB destination address */
    __IO uint32_t VE_ARGB_SRC_STRIDE;                 /*!< Offset 0x06C ARGB source strides for src1 and src2 */
    __IO uint32_t VE_ARGB_DST_STRIDE;                 /*!< Offset 0x070 ARGB destination stride */
    __IO uint32_t VE_ARGB_BLK_SIZE;                   /*!< Offset 0x074 ARGB size */
    __IO uint32_t VE_ARGB_BLK_FILL_VALUE;             /*!< Offset 0x078 ARGB fill value */
    __IO uint32_t VE_ARGB_BLK_CTRL;                   /*!< Offset 0x07C ARGB control */
    __IO uint32_t VE_LUMA_HIST_THR [0x004];           /*!< Offset 0x080 Luma histogram thresholds [0-3] */
    __IO uint32_t VE_LUMA_HIST_VAL [0x010];           /*!< Offset 0x090 Luma histogram output values [0-15] */
    __IO uint32_t VE_ANGL_R_BUF;                      /*!< Offset 0x0D0 Anaglyph red output buffer */
    __IO uint32_t VE_ANGL_G_BUF;                      /*!< Offset 0x0D4 Anaglyph green output buffer */
    __IO uint32_t VE_ANGL_B_BUF;                      /*!< Offset 0x0D8 Anaglyph blue output buffer */
         RESERVED(0x0DC[0x00E8 - 0x00DC], uint8_t)
    __IO uint32_t VE_EXTRA_OUT_FMT_OFFSET;            /*!< Offset 0x0E8 Extra output format and chroma offset (not available on A10/A13/A20) */
    __IO uint32_t VE_OUTPUT_FORMAT;                   /*!< Offset 0x0EC Output formats (since H3?) */
    __IO uint32_t VE_VERSION;                         /*!< Offset 0x0F0 IP Version register */
         RESERVED(0x0F4[0x00F8 - 0x00F4], uint8_t)
    __IO uint32_t VE_DBG_CTRL;                        /*!< Offset 0x0F8 Debug control */
    __IO uint32_t VE_DBG_OUTPUT;                      /*!< Offset 0x0FC Debug output */
    __IO uint32_t MPEG_PHDR;                          /*!< Offset 0x100 MPEG12 Picture Header register */
    __IO uint32_t MPEG_VOPHDR;                        /*!< Offset 0x104 MPEG Video Object Plane Header register (MPEG4 Header) */
    __IO uint32_t MPEG_SIZE;                          /*!< Offset 0x108 Frame size in MPEG macroblocks (16x16) */
    __IO uint32_t MPEG_FRAME_SIZE;                    /*!< Offset 0x10C Frame size in pixels */
    __IO uint32_t MPEG_MBA;                           /*!< Offset 0x110 MPEG Macro Block Address register */
    __IO uint32_t MPEG_CTRL;                          /*!< Offset 0x114 MPEG Control Register */
    __IO uint32_t MPEG_TRIG;                          /*!< Offset 0x118 MPEG Decoding Trigger */
    __IO uint32_t MPEG_STATUS;                        /*!< Offset 0x11C MACC MPEG Status register */
    __IO uint32_t MPEG_FRAME_DIST;                    /*!< Offset 0x120 MPEG P and B Frame distance */
    __IO uint32_t MPEG_TRBTRDFLD;                     /*!< Offset 0x124 Temporal References(TRB(B-VOP) and TRD) */
    __IO uint32_t MPEG_VLD_ADDR;                      /*!< Offset 0x128 MPEG Variable Length Decoding Address */
    __IO uint32_t MPEG_VLD_OFFSET;                    /*!< Offset 0x12C MPEG Variable Length Decoding Offset */
    __IO uint32_t MPEG_VLD_LEN;                       /*!< Offset 0x130 MPEG Variable Length Decoding Length */
    __IO uint32_t MPEG_VBV_END;                       /*!< Offset 0x134 MPEG VBV end - video source buffer end */
    __IO uint32_t MPEG_MBH_ADDR;                      /*!< Offset 0x138 MBH buffer address */
    __IO uint32_t MPEG_DCAC_ADDR;                     /*!< Offset 0x13C DCAC Buffer address */
    __IO uint32_t MPEG_BLK_OFFSET;                    /*!< Offset 0x140 MPEG Block address??? */
    __IO uint32_t MPEG_NCF_ADDR;                      /*!< Offset 0x144 NFC buffer address */
    __IO uint32_t MPEG_REC_LUMA;                      /*!< Offset 0x148 MPEG Luma reconstruct buffer */
    __IO uint32_t MPEG_REC_CHROMA;                    /*!< Offset 0x14C MPEG Chroma reconstruct buffer */
    __IO uint32_t MPEG_FWD_LUMA;                      /*!< Offset 0x150 MPEG Luma forward buffer */
    __IO uint32_t MPEG_FWD_CHROMA;                    /*!< Offset 0x154 MPEG forward buffer */
    __IO uint32_t MPEG_BACK_LUMA;                     /*!< Offset 0x158 MPEG Luma Back buffer */
    __IO uint32_t MPEG_BACK_CHROMA;                   /*!< Offset 0x15C MPEG Chroma Back buffer */
    __IO uint32_t MPEG_SOCX;                          /*!< Offset 0x160 MS-MPEG related */
    __IO uint32_t MPEG_SOCY;                          /*!< Offset 0x164 MS-MPEG related */
    __IO uint32_t MPEG_SOL;                           /*!< Offset 0x168 MS-MPEG related */
    __IO uint32_t MPEG_SDLX;                          /*!< Offset 0x16C MS-MPEG related */
    __IO uint32_t MPEG_SDLY;                          /*!< Offset 0x170 MS-MPEG related */
    __IO uint32_t MPEG_SPRITESHFT;                    /*!< Offset 0x174 MS-MPEG related */
    __IO uint32_t MPEG_SDCX;                          /*!< Offset 0x178 MS-MPEG related */
    __IO uint32_t MPEG_SDCY;                          /*!< Offset 0x17C MS-MPEG related */
    __IO uint32_t MPEG_IQ_MIN_INPUT;                  /*!< Offset 0x180 MPEG Inverse Quantization minimum input level */
    __IO uint32_t MPEG_IQ_INPUT;                      /*!< Offset 0x184 MPEG Inverse Quantization input level */
    __IO uint32_t MPEG_MSMPEG4_HDR;                   /*!< Offset 0x188 MPEG MS-Mpeg-4 header */
    __IO uint32_t MPEG_VP6_HDR;                       /*!< Offset 0x18C MPEG VP6 Header */
    __IO uint32_t MPEG_IQ_IDCT_INPUT;                 /*!< Offset 0x190 MPEG Inverse Quantization and Inverse Discrete Cosine Transform input */
    __IO uint32_t MPEG_MB_HEIGHT;                     /*!< Offset 0x194 MPEG Macro Block Height */
    __IO uint32_t MPEG_MB_V1;                         /*!< Offset 0x198 MPEG Macro Block Vector 1 */
    __IO uint32_t MPEG_MB_V2;                         /*!< Offset 0x19C MPEG Macro Block Vector 2 */
    __IO uint32_t MPEG_MB_V3;                         /*!< Offset 0x1A0 MPEG Macro Block Vector 3 */
    __IO uint32_t MPEG_MB_V4;                         /*!< Offset 0x1A4 MPEG Macro Block Vector 4 */
    __IO uint32_t MPEG_MB_V5;                         /*!< Offset 0x1A8 MPEG Macro Block Vector 5 */
    __IO uint32_t MPEG_MB_V6;                         /*!< Offset 0x1AC MPEG Macro Block Vector 6 */
    __IO uint32_t MPEG_MB_V7;                         /*!< Offset 0x1B0 MPEG Macro Block Vector 7 */
    __IO uint32_t MPEG_MB_V8;                         /*!< Offset 0x1B4 MPEG Macro Block Vector 8 */
    __IO uint32_t MPEG_JPEG_SIZE;                     /*!< Offset 0x1B8 JPEG Size */
    __IO uint32_t MPEG_JPEG_MCU;                      /*!< Offset 0x1BC JPEG Minimum Coded Unit */
    __IO uint32_t MPEG_JPEG_RES_INT;                  /*!< Offset 0x1C0 JPEG Restart Interval */
    __IO uint32_t MPEG_ERROR;                         /*!< Offset 0x1C4 MPEG Error flags */
    __IO uint32_t MPEG_CTR_MB;                        /*!< Offset 0x1C8 (Macroblock Control??) */
    __IO uint32_t MPEG_ROT_LUMA;                      /*!< Offset 0x1CC MPEG Rotate-Scale Luma buffer */
    __IO uint32_t MPEG_ROT_CHROMA;                    /*!< Offset 0x1D0 MPEG Rotate-Scale Chroma buffer */
    __IO uint32_t MPEG_ROTSCALE_CTRL;                 /*!< Offset 0x1D4 Control Rotate/Scale Buffer */
    __IO uint32_t MPEG_JPEG_MCU_START;                /*!< Offset 0x1D8 JPEG Macro Cell Unit Start */
    __IO uint32_t MPEG_JPEG_MCU_END;                  /*!< Offset 0x1DC JPEG Macro Cell Unit End */
    __IO uint32_t MPEG_SRAM_RW_OFFSET;                /*!< Offset 0x1E0 Auto incremental pointer for read/write VE SRAM */
    __IO uint32_t MPEG_SRAM_RW_DATA;                  /*!< Offset 0x1E4 FIFO Like Data register for write/read VE SRAM */
         RESERVED(0x1E8[0x01F0 - 0x01E8], uint8_t)
    __IO uint32_t MPEG_START_CODE_BITOFFSET;          /*!< Offset 0x1F0 MPEG start code search result */
         RESERVED(0x1F4[0x0200 - 0x01F4], uint8_t)
    __IO uint32_t H264_SEQ_HDR;                       /*!< Offset 0x200 H264 Sequence header */
    __IO uint32_t H264_PIC_HDR;                       /*!< Offset 0x204 H264 Picture header */
    __IO uint32_t H264_SLICE_HDR;                     /*!< Offset 0x208 H264 Slice header */
    __IO uint32_t H264_SLICE_HDR2;                    /*!< Offset 0x20C H264 Slice header */
    __IO uint32_t H264_PRED_WEIGHT;                   /*!< Offset 0x210 H264 weighted prediction parameters */
    __IO uint32_t H264_VP8_HDR;                       /*!< Offset 0x214 H264 VP8 Picture header */
    __IO uint32_t H264_QINDEX;                        /*!< Offset 0x218 H264 Quantizer settings (VP8) */
    __IO uint32_t H264_VP8_PART_OFFSET_H264_QP;       /*!< Offset 0x21C H264 QP parameters (VP8 partition offset) */
    __IO uint32_t H264_CTRL;                          /*!< Offset 0x220 H264 Control Register */
    __IO uint32_t H264_TRIG;                          /*!< Offset 0x224 H264 Trigger Register */
    __IO uint32_t H264_STATUS;                        /*!< Offset 0x228 H264 Status Register */
    __IO uint32_t H264_CUR_MBNUM;                     /*!< Offset 0x22C H264 current Macroblock */
    __IO uint32_t H264_VLD_ADDR;                      /*!< Offset 0x230 H264 Variable Length Decoder Address */
    __IO uint32_t H264_VLD_OFFSET;                    /*!< Offset 0x234 H264 Variable Length Decoder Bit Offset */
    __IO uint32_t H264_VLD_LEN;                       /*!< Offset 0x238 H264 Variable Length Decoder Bit Length */
    __IO uint32_t H264_VLD_END;                       /*!< Offset 0x23C H264 Variable Length Decoder End Address */
    __IO uint32_t H264_SDROT_CTRL;                    /*!< Offset 0x240 H264 Scale Rotate buffer control */
    __IO uint32_t H264_SDROT_LUMA;                    /*!< Offset 0x244 H264 Scale Rotate buffer Luma color component */
    __IO uint32_t H264_SDROT_CHROMA;                  /*!< Offset 0x248 H264 Scale Rotate buffer Chroma color component */
    __IO uint32_t H264_OUTPUT_FRAME_INDEX;            /*!< Offset 0x24C H264 output frame index in dpb */
    __IO uint32_t H264_FIELD_INTRA_INFO_BUF_H264_VP8_ENTROPY_PROBS;/*!< Offset 0x250 H264 field intra info buffer address (VP8 entropy brobabilities table address) */
    __IO uint32_t H264_NEIGHBOR_INFO_BUF_H264_VP8_FSTDATA_PARTLEN;/*!< Offset 0x254 H264 neighbor info buffer address (VP8 First partition length) */
    __IO uint32_t H264_PIC_MBSIZE;                    /*!< Offset 0x258 H264 Picture size in macroblocks */
    __IO uint32_t H264_PIC_BOUNDARYSIZE;              /*!< Offset 0x25C H264 Picture size in pixels */
    __IO uint32_t H264_MB_ADDR;                       /*!< Offset 0x260 H264 Current macroblock position */
    __IO uint32_t H264_MB_NB1;                        /*!< Offset 0x264 H264 ??? MbNeightbour1 */
    __IO uint32_t H264_MB_NB2;                        /*!< Offset 0x268 H264 MbNeightbour2 */
    __IO uint32_t H264_MB_NB3;                        /*!< Offset 0x26C H264 ??? */
    __IO uint32_t H264_MB_NB4;                        /*!< Offset 0x270 H264 ??? */
    __IO uint32_t H264_MB_NB5;                        /*!< Offset 0x274 H264 ??? */
    __IO uint32_t H264_MB_NB6;                        /*!< Offset 0x278 H264 ??? */
    __IO uint32_t H264_MB_NB7;                        /*!< Offset 0x27C H264 ??? */
    __IO uint32_t H264_MB_NB8;                        /*!< Offset 0x280 H264 ??? */
    __IO uint32_t H264_0x0284;                        /*!< Offset 0x284 H264 ??? */
    __IO uint32_t H264_0x0288;                        /*!< Offset 0x288 H264 ??? */
    __IO uint32_t H264_0x028c;                        /*!< Offset 0x28C H264 ??? */
    __IO uint32_t H264_MB_QP;                         /*!< Offset 0x290 H264 ??? */
    __IO uint32_t H264_0x0294;                        /*!< Offset 0x294 H264 ??? */
    __IO uint32_t H264_0x0298;                        /*!< Offset 0x298 H264 ??? */
    __IO uint32_t H264_0x029c;                        /*!< Offset 0x29C H264 ??? */
    __IO uint32_t H264_0x02a0;                        /*!< Offset 0x2A0 H264 ??? */
    __IO uint32_t H264_0x02a4;                        /*!< Offset 0x2A4 H264 ??? */
    __IO uint32_t H264_0x02a8;                        /*!< Offset 0x2A8 H264 ??? */
    __IO uint32_t H264_REC_LUMA;                      /*!< Offset 0x2AC H264 Luma reconstruct buffer */
    __IO uint32_t H264_FWD_LUMA;                      /*!< Offset 0x2B0 H264 Luma forward buffer */
    __IO uint32_t H264_BACK_LUMA;                     /*!< Offset 0x2B4 H264 Luma back buffer */
    __IO uint32_t H264_ERROR;                         /*!< Offset 0x2B8 H264 Error */
    __IO uint32_t H264_0x02bc;                        /*!< Offset 0x2BC H264 ??? */
    __IO uint32_t H264_0x02c0;                        /*!< Offset 0x2C0 H264 ??? */
    __IO uint32_t H264_0x02c4;                        /*!< Offset 0x2C4 H264 ??? */
    __IO uint32_t H264_0x02c8;                        /*!< Offset 0x2C8 H264 ??? */
    __IO uint32_t H264_0x02cc;                        /*!< Offset 0x2CC H264 ??? */
    __IO uint32_t H264_REC_CHROMA;                    /*!< Offset 0x2D0 H264 Chroma reconstruct buffer */
    __IO uint32_t H264_FWD_CHROMA;                    /*!< Offset 0x2D4 H264 Chroma forward buffer */
    __IO uint32_t H264_BACK_CHROMA;                   /*!< Offset 0x2D8 H264 Chroma back buffer */
    __IO uint32_t H264_BASIC_BITS_DATA;               /*!< Offset 0x2DC H264 Basic bits data */
    __IO uint32_t H264_RAM_WRITE_PTR;                 /*!< Offset 0x2E0 H264 ram write pointer */
    __IO uint32_t H264_RAM_WRITE_DATA;                /*!< Offset 0x2E4 H264 ram write data */
    __IO uint32_t H264_ALT_LUMA;                      /*!< Offset 0x2E8 H264 Alternate Luma buffer */
    __IO uint32_t H264_ALT_CHROMA;                    /*!< Offset 0x2EC H264 Alternate Chroma buffer */
    __IO uint32_t H264_SEG_MB_LV0;                    /*!< Offset 0x2F0 H264 ??? Segment Mb Level 0 */
    __IO uint32_t H264_SEG_MB_LV1;                    /*!< Offset 0x2F4 H264 ??? Segment Mb Level 1 */
    __IO uint32_t H264_REF_LF_DELTA;                  /*!< Offset 0x2F8 H264 ??? (VP8 ref lf deltas) */
    __IO uint32_t H264_MODE_LF_DELTA;                 /*!< Offset 0x2FC H264 ??? (VP8 mode lf deltas) */
    __IO uint32_t VC1_EPHS;                           /*!< Offset 0x300 VC1 ??? */
    __IO uint32_t VC1_PIC_CTRL;                       /*!< Offset 0x304 VC1 ??? */
    __IO uint32_t VC1_PIC_QP;                         /*!< Offset 0x308 VC1 ??? */
    __IO uint32_t VC1_PIC_MV;                         /*!< Offset 0x30C VC1 ??? */
    __IO uint32_t VC1_PIC_INTEN_COMP;                 /*!< Offset 0x310 VC1 ??? */
    __IO uint32_t VC1_PIC_INTERLANCE;                 /*!< Offset 0x314 VC1 ??? */
    __IO uint32_t VC1_HDR_LEN;                        /*!< Offset 0x318 VC1 ??? */
    __IO uint32_t VC1_FSIZE;                          /*!< Offset 0x31C VC1 ??? */
    __IO uint32_t VC1_PIC_SIZE;                       /*!< Offset 0x320 VC1 ??? */
    __IO uint32_t VC1_CTRL;                           /*!< Offset 0x324 VC1 Decoder Control */
    __IO uint32_t VC1_START_TYPE;                     /*!< Offset 0x328 VC1 ??? */
    __IO uint32_t VC1_STATUS;                         /*!< Offset 0x32C VC1 Status */
    __IO uint32_t VC1_VBV_BASE_ADDR;                  /*!< Offset 0x330 VC1 Source buffer address */
    __IO uint32_t VC1_VLD_OFFSET;                     /*!< Offset 0x334 VC1 Variable Length Decoder Offset */
    __IO uint32_t VC1_VBV_LEN;                        /*!< Offset 0x338 VC1 length of source video buffer */
    __IO uint32_t VC1_VBV_END_ADDR;                   /*!< Offset 0x33C VC1 last address of source video buffer */
    __IO uint32_t VC1_REC_FRAME_CHROMA;               /*!< Offset 0x340 VC1 Chroma Reconstruct frame */
    __IO uint32_t VC1_REC_FRAME_LUMA;                 /*!< Offset 0x344 VC1 Luma Reconstruct frame */
    __IO uint32_t VC1_FWD_FRAME_CHROMA;               /*!< Offset 0x348 VC1 Chroma Forward Frame */
    __IO uint32_t VC1_FWD_FRAME_LUMA;                 /*!< Offset 0x34C VC1 Luma Forward Frame */
    __IO uint32_t VC1_BACK_CHROMA;                    /*!< Offset 0x350 VC1 Chroma back buffer */
    __IO uint32_t VC1_BACK_LUMA;                      /*!< Offset 0x354 VC1 Luma back buffer */
    __IO uint32_t VC1_MBHADDR;                        /*!< Offset 0x358 VC1 ??? */
    __IO uint32_t VC1_DCAPRED_ADDR;                   /*!< Offset 0x35C VC1 ??? */
    __IO uint32_t VC1_BITPLANE_ADDR;                  /*!< Offset 0x360 VC1 ??? */
    __IO uint32_t VC1_MBINFO_ADDR;                    /*!< Offset 0x364 VC1 ???(or COLMVINFOADDR) */
    __IO uint32_t VC1_0x0368;                         /*!< Offset 0x368 VC1 ??? */
    __IO uint32_t VC1_0x036c;                         /*!< Offset 0x36C VC1 ??? */
    __IO uint32_t VC1_MBA;                            /*!< Offset 0x370 VC1 ??? */
    __IO uint32_t VC1_MBHDR;                          /*!< Offset 0x374 VC1 ??? */
    __IO uint32_t VC1_LUMA_TRANSFORM;                 /*!< Offset 0x378 VC1 ??? */
    __IO uint32_t VC1_MBCBF;                          /*!< Offset 0x37C VC1 ??? */
    __IO uint32_t VC1_MBM_V1;                         /*!< Offset 0x380 VC1 ??? */
    __IO uint32_t VC1_MBM_V2;                         /*!< Offset 0x384 VC1 ??? */
    __IO uint32_t VC1_MBM_V3;                         /*!< Offset 0x388 VC1 ??? */
    __IO uint32_t VC1_MBM_V4;                         /*!< Offset 0x38C VC1 ??? */
    __IO uint32_t VC1_MBM_V5;                         /*!< Offset 0x390 VC1 ??? */
    __IO uint32_t VC1_MBM_V6;                         /*!< Offset 0x394 VC1 ??? */
    __IO uint32_t VC1_MBM_V7;                         /*!< Offset 0x398 VC1 ??? */
    __IO uint32_t VC1_MBM_V8;                         /*!< Offset 0x39C VC1 ??? */
    __IO uint32_t VC1_0x03a0;                         /*!< Offset 0x3A0 VC1 ??? */
    __IO uint32_t VC1_0x03a4;                         /*!< Offset 0x3A4 VC1 ??? */
    __IO uint32_t VC1_0x03a8;                         /*!< Offset 0x3A8 VC1 ??? */
    __IO uint32_t VC1_0x03ac;                         /*!< Offset 0x3AC VC1 ??? */
    __IO uint32_t VC1_0x03b0;                         /*!< Offset 0x3B0 VC1 ??? */
    __IO uint32_t VC1_0x03b4;                         /*!< Offset 0x3B4 VC1 ??? */
    __IO uint32_t VC1_ERROR;                          /*!< Offset 0x3B8 VC1 Error result code */
    __IO uint32_t VC1_CRT_MB_NUM;                     /*!< Offset 0x3BC VC1 ??? */
    __IO uint32_t VC1_EXTRA_CTRL;                     /*!< Offset 0x3C0 VC1 ??? */
    __IO uint32_t VC1_EXTRA_CBUF_ADDR;                /*!< Offset 0x3C4 VC1 EXTRA Chroma DRAM address */
    __IO uint32_t VC1_EXTRA_YBUF_ADDR;                /*!< Offset 0x3C8 VC1 EXTRA Luma DRAM address */
         RESERVED(0x3CC[0x03D0 - 0x03CC], uint8_t)
    __IO uint32_t VC1_OVERLAP_UP_ADDR;                /*!< Offset 0x3D0 VC1 ??? */
    __IO uint32_t VC1_DBLK_ABOVE_ADDR;                /*!< Offset 0x3D4 VC1 ??? */
    __IO uint32_t VC1_0x03d8;                         /*!< Offset 0x3D8 VC1 ??? */
    __IO uint32_t VC1_BITS_RETDATA;                   /*!< Offset 0x3DC VC1 ??? */
         RESERVED(0x3E0[0x03FC - 0x03E0], uint8_t)
    __IO uint32_t VC1_DEBUG_BUF_ADDR;                 /*!< Offset 0x3FC VC1 ??? */
    __IO uint32_t RMVB_SLC_HDR;                       /*!< Offset 0x400 Header */
    __IO uint32_t RMVB_FRM_SIZE;                      /*!< Offset 0x404 Framesize (in macroblocks ?) */
    __IO uint32_t RMVB_DIR_MODE_RATIO;                /*!< Offset 0x408  */
    __IO uint32_t RMVB_DIR_MB_ADDR;                   /*!< Offset 0x40C  */
    __IO uint32_t RMVB_QC_INPUT;                      /*!< Offset 0x410  */
    __IO uint32_t RMVB_CTRL;                          /*!< Offset 0x414 RMVB IRQ Control */
    __IO uint32_t RMVB_TRIG;                          /*!< Offset 0x418 Trigger register */
    __IO uint32_t RMVB_STATUS;                        /*!< Offset 0x41C RMVB Status */
         RESERVED(0x420[0x0428 - 0x0420], uint8_t)
    __IO uint32_t RMVB_VBV_BASE;                      /*!< Offset 0x428 Video source buffer base */
    __IO uint32_t RMVB_VLD_OFFSET;                    /*!< Offset 0x42C Video source buffer DRAM address */
    __IO uint32_t RMVB_VLD_LEN;                       /*!< Offset 0x430 Video source buffer length in bytes */
    __IO uint32_t RMVB_VBV_END;                       /*!< Offset 0x434 Video source buffer last DRAM address */
         RESERVED(0x438[0x043C - 0x0438], uint8_t)
    __IO uint32_t RMVB_HUFF_TABLE_ADDR;               /*!< Offset 0x43C Huffman table DRAM address */
    __IO uint32_t RMVB_CUR_Y_ADDR;                    /*!< Offset 0x440 Luma Current buffer DRAM address */
    __IO uint32_t RMVB_CUR_C_ADDR;                    /*!< Offset 0x444 Chroma Current buffer DRAM address */
    __IO uint32_t RMVB_FOR_Y_ADDR;                    /*!< Offset 0x448 Luma Forward buffer DRAM address */
    __IO uint32_t RMVB_FOR_C_ADDR;                    /*!< Offset 0x44C Chroma Forward buffer DRAM address */
    __IO uint32_t RMVB_BAC_Y_ADDR;                    /*!< Offset 0x450 Luma Back buffer DRAM address */
    __IO uint32_t RMVB_BAC_C_ADDR;                    /*!< Offset 0x454 Chroma Back buffer DRAM address */
    __IO uint32_t RMVB_ROT_Y_ADDR;                    /*!< Offset 0x458 Luma Rot buffer DRAM address */
    __IO uint32_t RMVB_ROT_C_ADDR;                    /*!< Offset 0x45C Chroma Rot Buffer DRAM address */
    __IO uint32_t RMVB_MBH_ADDR;                      /*!< Offset 0x460  */
    __IO uint32_t RMVB_MV_ADDR;                       /*!< Offset 0x464  */
         RESERVED(0x468[0x0470 - 0x0468], uint8_t)
    __IO uint32_t RMVB_MBH_INFO;                      /*!< Offset 0x470  */
    __IO uint32_t RMVB_MV0;                           /*!< Offset 0x474 Mountion vector 0 */
    __IO uint32_t RMVB_MV1;                           /*!< Offset 0x478 Mountion vector 1 */
    __IO uint32_t RMVB_MV2;                           /*!< Offset 0x47C Mountion vector 2 */
    __IO uint32_t RMVB_MV3;                           /*!< Offset 0x480 Mountion vector 3 */
         RESERVED(0x484[0x0490 - 0x0484], uint8_t)
    __IO uint32_t RMVB_DBLK_COEF;                     /*!< Offset 0x490  */
         RESERVED(0x494[0x04B0 - 0x0494], uint8_t)
    __IO uint32_t RMVB_ERROR;                         /*!< Offset 0x4B0 Decode error result code */
         RESERVED(0x4B4[0x04B8 - 0x04B4], uint8_t)
    __IO uint32_t RMVB_BITS_DATA;                     /*!< Offset 0x4B8  */
         RESERVED(0x4BC[0x04C0 - 0x04BC], uint8_t)
    __IO uint32_t RMVB_SLC_QUEUE_ADDR;                /*!< Offset 0x4C0  */
    __IO uint32_t RMVB_SLC_QUEUE_LEN;                 /*!< Offset 0x4C4  */
    __IO uint32_t RMVB_SLC_QUEUE_TRIG;                /*!< Offset 0x4C8  */
    __IO uint32_t RMVB_SLC_QUEUE_STATUS;              /*!< Offset 0x4CC  */
    __IO uint32_t RMVB_SCALE_ROT_CTRL;                /*!< Offset 0x4D0  */
         RESERVED(0x4D4[0x04E0 - 0x04D4], uint8_t)
    __IO uint32_t RMVB_SRAM_RW_OFFSET;                /*!< Offset 0x4E0 SRAM Fifo like index register */
    __IO uint32_t RMVB_SRAM_RW_DATA;                  /*!< Offset 0x4E4 SRAM Fifo like data register */
         RESERVED(0x4E8[0x0500 - 0x04E8], uint8_t)
    __IO uint32_t HEVC_NAL_HDR;                       /*!< Offset 0x500 HEVC NAL header */
    __IO uint32_t HEVC_SPS;                           /*!< Offset 0x504 HEVC sequence parameter set */
    __IO uint32_t HEVC_PIC_SIZE;                      /*!< Offset 0x508 HEVC picture size */
    __IO uint32_t HEVC_PCM_HDR;                       /*!< Offset 0x50C HEVC PCM header */
    __IO uint32_t HEVC_PPS0;                          /*!< Offset 0x510 HEVC picture parameter set */
    __IO uint32_t HEVC_PPS1;                          /*!< Offset 0x514 HEVC picture parameter set */
    __IO uint32_t HEVC_SCALING_LIST_CTRL;             /*!< Offset 0x518 HEVC scaling list control register */
         RESERVED(0x51C[0x0520 - 0x051C], uint8_t)
    __IO uint32_t HEVC_SLICE_HDR0;                    /*!< Offset 0x520 HEVC slice header */
    __IO uint32_t HEVC_SLICE_HDR1;                    /*!< Offset 0x524 HEVC slice header */
    __IO uint32_t HEVC_SLICE_HDR2;                    /*!< Offset 0x528 HEVC slice header */
    __IO uint32_t HEVC_CTB_ADDR;                      /*!< Offset 0x52C HEVC CTB address */
    __IO uint32_t HEVC_CTRL;                          /*!< Offset 0x530 HEVC control register */
    __IO uint32_t HEVC_TRIG;                          /*!< Offset 0x534 HEVC trigger register */
    __IO uint32_t HEVC_STATUS;                        /*!< Offset 0x538 HEVC status register */
    __IO uint32_t HEVC_CTU_NUM;                       /*!< Offset 0x53C HEVC current CTU number */
    __IO uint32_t HEVC_BITS_ADDR;                     /*!< Offset 0x540 HEVC bitstream address */
    __IO uint32_t HEVC_BITS_OFFSET;                   /*!< Offset 0x544 HEVC bitstream offset */
    __IO uint32_t HEVC_BITS_LEN;                      /*!< Offset 0x548 HEVC bitstream length */
    __IO uint32_t HEVC_BITS_END_ADDR;                 /*!< Offset 0x54C HEVC bitstream end address */
    __IO uint32_t HEVC_EXTRA_OUT_CTRL;                /*!< Offset 0x550 HEVC extra output control register */
    __IO uint32_t HEVC_EXTRA_OUT_LUMA_ADDR;           /*!< Offset 0x554 HEVC extra output luma address */
    __IO uint32_t HEVC_EXTRA_OUT_CHROMA_ADDR;         /*!< Offset 0x558 HEVC extra output chroma address */
    __IO uint32_t HEVC_REC_BUF_IDX;                   /*!< Offset 0x55C HEVC reconstruct buffer index */
    __IO uint32_t HEVC_NEIGHBOR_INFO_ADDR;            /*!< Offset 0x560 HEVC neighbor info buffer address */
    __IO uint32_t HEVC_TILE_LIST_ADDR;                /*!< Offset 0x564 HEVC tile entry point list address */
    __IO uint32_t HEVC_TILE_START_CTB;                /*!< Offset 0x568 HEVC tile start CTB */
    __IO uint32_t HEVC_TILE_END_CTB;                  /*!< Offset 0x56C HEVC tile end CTB */
         RESERVED(0x570[0x0578 - 0x0570], uint8_t)
    __IO uint32_t HEVC_SCALING_LIST_DC_COEF0;         /*!< Offset 0x578 HEVC scaling list DC coefficients */
    __IO uint32_t HEVC_SCALING_LIST_DC_COEF1;         /*!< Offset 0x57C HEVC scaling list DC coefficients */
         RESERVED(0x580[0x05DC - 0x0580], uint8_t)
    __IO uint32_t HEVC_BITS_DATA;                     /*!< Offset 0x5DC HEVC bitstream data */
    __IO uint32_t HEVC_SRAM_ADDR;                     /*!< Offset 0x5E0 HEVC SRAM address */
    __IO uint32_t HEVC_SRAM_DATA;                     /*!< Offset 0x5E4 HEVC SRAM data */
         RESERVED(0x5E8[0x0A00 - 0x05E8], uint8_t)
    __IO uint32_t ISP_PIC_SIZE;                       /*!< Offset 0xA00 ISP source picture size in macroblocks (16x16) */
    __IO uint32_t ISP_PIC_STRIDE;                     /*!< Offset 0xA04 ISP source picture stride */
    __IO uint32_t ISP_CTRL;                           /*!< Offset 0xA08 ISP IRQ Control */
    __IO uint32_t ISP_TRIG;                           /*!< Offset 0xA0C ISP Trigger */
         RESERVED(0xA10[0x0A2C - 0x0A10], uint8_t)
    __IO uint32_t ISP_SCALER_SIZE;                    /*!< Offset 0xA2C ISP scaler frame size/16 */
    __IO uint32_t ISP_SCALER_OFFSET_Y;                /*!< Offset 0xA30 ISP scaler picture offset for luma */
    __IO uint32_t ISP_SCALER_OFFSET_C;                /*!< Offset 0xA34 ISP scaler picture offset for chroma */
    __IO uint32_t ISP_SCALER_FACTOR;                  /*!< Offset 0xA38 ISP scaler picture scale factor */
         RESERVED(0xA3C[0x0A44 - 0x0A3C], uint8_t)
    __IO uint32_t ISP_BUF_0x0a44;                     /*!< Offset 0xA44 ISP PHY Buffer offset */
    __IO uint32_t ISP_BUF_0x0a48;                     /*!< Offset 0xA48 ISP PHY Buffer offset */
    __IO uint32_t ISP_BUF_0x0a4C;                     /*!< Offset 0xA4C ISP PHY Buffer offset */
         RESERVED(0xA50[0x0A70 - 0x0A50], uint8_t)
    __IO uint32_t ISP_OUTPUT_LUMA;                    /*!< Offset 0xA70 ISP Output LUMA Address */
    __IO uint32_t ISP_OUTPUT_CHROMA;                  /*!< Offset 0xA74 ISP Output CHROMA Address */
    __IO uint32_t ISP_WB_THUMB_LUMA;                  /*!< Offset 0xA78 ISP THUMB WriteBack PHY LUMA Address */
    __IO uint32_t ISP_WB_THUMB_CHROMA;                /*!< Offset 0xA7C ISP THUMB WriteBack PHY CHROMA Adress */
         RESERVED(0xA80[0x0AE0 - 0x0A80], uint8_t)
    __IO uint32_t ISP_SRAM_INDEX;                     /*!< Offset 0xAE0 ISP VE SRAM Index */
    __IO uint32_t ISP_SRAM_DATA;                      /*!< Offset 0xAE4 ISP VE SRAM Data */
         RESERVED(0xAE8[0x0B00 - 0x0AE8], uint8_t)
    __IO uint32_t AVC_PICINFO;                        /*!< Offset 0xB00 unk(not used in blob) */
    __IO uint32_t AVC_JPEG_CTRL_MACC_AVC_H264_CTRL;   /*!< Offset 0xB04 jpeg / h264 different settings */
    __IO uint32_t AVC_H264_QP;                        /*!< Offset 0xB08 H264 quantization parameters */
         RESERVED(0xB0C[0x0B10 - 0x0B0C], uint8_t)
    __IO uint32_t AVC_H264_MOTION_EST;                /*!< Offset 0xB10 Motion estimation parameters */
    __IO uint32_t AVC_CTRL;                           /*!< Offset 0xB14 AVC Encoder IRQ Control */
    __IO uint32_t AVC_TRIG;                           /*!< Offset 0xB18 AVC Encoder trigger */
    __IO uint32_t AVC_STATUS;                         /*!< Offset 0xB1C AVC Encoder Busy Status */
    __IO uint32_t AVC_BITS_DATA;                      /*!< Offset 0xB20 AVC Encoder Bits Data */
         RESERVED(0xB24[0x0B50 - 0x0B24], uint8_t)
    __IO uint32_t AVC_H264_MAD;                       /*!< Offset 0xB50 AVC H264 Encoder Mean Absolute Difference */
    __IO uint32_t AVC_H264_RESIDUAL_BITS;             /*!< Offset 0xB54 AVC H264 Encoder Residual Bits */
    __IO uint32_t AVC_H264_HEADER_BITS;               /*!< Offset 0xB58 AVC H264 Encoder Header Bits */
    __IO uint32_t AVC_H264_0x0b5c;                    /*!< Offset 0xB5C AVC H264 Encoder unknown statistical data, maybe motion vectors */
    __IO uint32_t AVC_H264_0x0b60;                    /*!< Offset 0xB60 AVC H264 Encoder unknown buffer */
         RESERVED(0xB64[0x0B80 - 0x0B64], uint8_t)
    __IO uint32_t AVC_VLE_ADDR;                       /*!< Offset 0xB80 AVC Variable Length Encoder Start Address */
    __IO uint32_t AVC_VLE_END;                        /*!< Offset 0xB84 AVC Variable Length Encoder End Address */
    __IO uint32_t AVC_VLE_OFFSET;                     /*!< Offset 0xB88 AVC Variable Length Encoder Bit Offset */
    __IO uint32_t AVC_VLE_MAX;                        /*!< Offset 0xB8C AVC Variable Length Encoder Maximum Bits */
    __IO uint32_t AVC_VLE_LENGTH;                     /*!< Offset 0xB90 AVC Variable Length Encoder Bit Length */
         RESERVED(0xB94[0x0BA0 - 0x0B94], uint8_t)
    __IO uint32_t AVC_REF_LUMA;                       /*!< Offset 0xBA0 Luma reference buffer */
    __IO uint32_t AVC_REF_CHROMA;                     /*!< Offset 0xBA4 Chroma reference buffer */
         RESERVED(0xBA8[0x0BB0 - 0x0BA8], uint8_t)
    __IO uint32_t AVC_REC_LUMA;                       /*!< Offset 0xBB0 Luma reconstruct buffer */
    __IO uint32_t AVC_REC_CHROMA;                     /*!< Offset 0xBB4 Chroma reconstruct buffer */
    __IO uint32_t AVC_REF_SLUMA;                      /*!< Offset 0xBB8 Smaller luma reference buffer ? */
    __IO uint32_t AVC_REC_SLUMA;                      /*!< Offset 0xBBC Smaller luma reconstruct buffer ? */
    __IO uint32_t AVC_MB_INFO;                        /*!< Offset 0xBC0 Temporary buffer with macroblock information */
         RESERVED(0xBC4[0x0BE0 - 0x0BC4], uint8_t)
    __IO uint32_t AVC_SRAM_INDEX;                     /*!< Offset 0xBE0 AVC VE SRAM Index */
    __IO uint32_t AVC_SRAM_DATA;                      /*!< Offset 0xBE4 AVC VE SRAM Data */
         RESERVED(0xBE8[0x2000 - 0x0BE8], uint8_t)
} VE_TypeDef; /* size of structure = 0x2000 */


/* Defines */



/* Access pointers */

#define DSP0_CFG ((DSP_CFG_TypeDef *) DSP0_CFG_BASE)  /*!< DSP0_CFG  register set access pointer */
#define DSP_INTC ((DSP_INTC_TypeDef *) DSP_INTC_BASE) /*!< DSP_INTC XTensa HiFi4 Interrupt Controller register set access pointer */
#define DSP_MSGBOX ((MSGBOX_TypeDef *) DSP_MSGBOX_BASE)/*!< DSP_MSGBOX Message Box register set access pointer */
#define VENCODER ((VE_TypeDef *) VENCODER_BASE)       /*!< VENCODER Video Encoding register set access pointer */
#define GPIOBLOCK ((GPIOBLOCK_TypeDef *) GPIOBLOCK_BASE)/*!< GPIOBLOCK  register set access pointer */
#define GPIOB ((GPIO_TypeDef *) GPIOB_BASE)           /*!< GPIOB  register set access pointer */
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE)           /*!< GPIOC  register set access pointer */
#define GPIOD ((GPIO_TypeDef *) GPIOD_BASE)           /*!< GPIOD  register set access pointer */
#define GPIOE ((GPIO_TypeDef *) GPIOE_BASE)           /*!< GPIOE  register set access pointer */
#define GPIOF ((GPIO_TypeDef *) GPIOF_BASE)           /*!< GPIOF  register set access pointer */
#define GPIOG ((GPIO_TypeDef *) GPIOG_BASE)           /*!< GPIOG  register set access pointer */
#define GPIOINTB ((GPIOINT_TypeDef *) GPIOINTB_BASE)  /*!< GPIOINTB  register set access pointer */
#define GPIOINTC ((GPIOINT_TypeDef *) GPIOINTC_BASE)  /*!< GPIOINTC  register set access pointer */
#define GPIOINTD ((GPIOINT_TypeDef *) GPIOINTD_BASE)  /*!< GPIOINTD  register set access pointer */
#define GPIOINTE ((GPIOINT_TypeDef *) GPIOINTE_BASE)  /*!< GPIOINTE  register set access pointer */
#define GPIOINTF ((GPIOINT_TypeDef *) GPIOINTF_BASE)  /*!< GPIOINTF  register set access pointer */
#define GPIOINTG ((GPIOINT_TypeDef *) GPIOINTG_BASE)  /*!< GPIOINTG  register set access pointer */
#define SPC ((SPC_TypeDef *) SPC_BASE)                /*!< SPC Secure Peripherals Control (SPC) - Sets secure property of peripherals register set access pointer */
#define PWM ((PWM_TypeDef *) PWM_BASE)                /*!< PWM Pulse Width Modulation module register set access pointer */
#define CCU ((CCU_TypeDef *) CCU_BASE)                /*!< CCU Clock Controller Unit (CCU) register set access pointer */
#define CIR_TX ((CIR_TX_TypeDef *) CIR_TX_BASE)       /*!< CIR_TX  register set access pointer */
#define TZMA ((TZMA_TypeDef *) TZMA_BASE)             /*!< TZMA  register set access pointer */
#define LEDC ((LEDC_TypeDef *) LEDC_BASE)             /*!< LEDC LED Lamp Controller register set access pointer */
#define GPADC ((GPADC_TypeDef *) GPADC_BASE)          /*!< GPADC  register set access pointer */
#define THS ((THS_TypeDef *) THS_BASE)                /*!< THS Thermal Sensor register set access pointer */
#define TPADC ((TPADC_TypeDef *) TPADC_BASE)          /*!< TPADC  register set access pointer */
#define IOMMU ((IOMMU_TypeDef *) IOMMU_BASE)          /*!< IOMMU  register set access pointer */
#define AUDIO_CODEC ((AUDIO_CODEC_TypeDef *) AUDIO_CODEC_BASE)/*!< AUDIO_CODEC Audio Codec register set access pointer */
#define DMIC ((DMIC_TypeDef *) DMIC_BASE)             /*!< DMIC  register set access pointer */
#define I2S1 ((I2S_PCM_TypeDef *) I2S1_BASE)          /*!< I2S1  register set access pointer */
#define I2S2 ((I2S_PCM_TypeDef *) I2S2_BASE)          /*!< I2S2  register set access pointer */
#define OWA ((OWA_TypeDef *) OWA_BASE)                /*!< OWA One Wire Audio (TX only) register set access pointer */
#define TIMER ((TIMER_TypeDef *) TIMER_BASE)          /*!< TIMER  register set access pointer */
#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0  register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1  register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2  register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3  register set access pointer */
#define UART4 ((UART_TypeDef *) UART4_BASE)           /*!< UART4  register set access pointer */
#define UART5 ((UART_TypeDef *) UART5_BASE)           /*!< UART5  register set access pointer */
#define TWI0 ((TWI_TypeDef *) TWI0_BASE)              /*!< TWI0  register set access pointer */
#define TWI1 ((TWI_TypeDef *) TWI1_BASE)              /*!< TWI1  register set access pointer */
#define TWI2 ((TWI_TypeDef *) TWI2_BASE)              /*!< TWI2  register set access pointer */
#define TWI3 ((TWI_TypeDef *) TWI3_BASE)              /*!< TWI3  register set access pointer */
#define CAN0 ((CAN_TypeDef *) CAN0_BASE)              /*!< CAN0 CAN (see Allwinner_T3_User_Manual_V1.0_cleaned.pdf as part of documentation) register set access pointer */
#define CAN1 ((CAN_TypeDef *) CAN1_BASE)              /*!< CAN1 CAN (see Allwinner_T3_User_Manual_V1.0_cleaned.pdf as part of documentation) register set access pointer */
#define SYS_CFG ((SYS_CFG_TypeDef *) SYS_CFG_BASE)    /*!< SYS_CFG  register set access pointer */
#define DMAC ((DMAC_TypeDef *) DMAC_BASE)             /*!< DMAC  register set access pointer */
#define CPUX_MSGBOX ((MSGBOX_TypeDef *) CPUX_MSGBOX_BASE)/*!< CPUX_MSGBOX Message Box register set access pointer */
#define SPINLOCK ((SPINLOCK_TypeDef *) SPINLOCK_BASE) /*!< SPINLOCK Spin Lock module register set access pointer */
#define SID ((SID_TypeDef *) SID_BASE)                /*!< SID Security ID register set access pointer */
#define SMC ((SMC_TypeDef *) SMC_BASE)                /*!< SMC Secure Memory Control (SMC) - Sets secure area of DRAM register set access pointer */
#define HSTIMER ((HSTIMER_TypeDef *) HSTIMER_BASE)    /*!< HSTIMER High Speed Timer (HSTimer) register set access pointer */
#define GICVSELF ((GICV_TypeDef *) GICVSELF_BASE)     /*!< GICVSELF  register set access pointer */
#define GICV ((GICV_TypeDef *) GICV_BASE)             /*!< GICV  register set access pointer */
#define CE_NS ((CE_TypeDef *) CE_NS_BASE)             /*!< CE_NS Crypto Engine (CE) register set access pointer */
#define CE_S ((CE_TypeDef *) CE_S_BASE)               /*!< CE_S Crypto Engine (CE) register set access pointer */
#define MCTL_COM ((MCTL_COM_TypeDef *) MCTL_COM_BASE) /*!< MCTL_COM  register set access pointer */
#define DDRPHYC ((DDRPHYC_TypeDef *) DDRPHYC_BASE)    /*!< DDRPHYC  register set access pointer */
#define MCTL_PHY ((MCTL_PHY_TypeDef *) MCTL_PHY_BASE) /*!< MCTL_PHY  register set access pointer */
#define SMHC0 ((SMHC_TypeDef *) SMHC0_BASE)           /*!< SMHC0 SD-MMC Host Controller register set access pointer */
#define SMHC1 ((SMHC_TypeDef *) SMHC1_BASE)           /*!< SMHC1 SD-MMC Host Controller register set access pointer */
#define SMHC2 ((SMHC_TypeDef *) SMHC2_BASE)           /*!< SMHC2 SD-MMC Host Controller register set access pointer */
#define SPI0 ((SPI_TypeDef *) SPI0_BASE)              /*!< SPI0 Serial Peripheral Interface register set access pointer */
#define SPI1 ((SPI_TypeDef *) SPI1_BASE)              /*!< SPI1 Serial Peripheral Interface register set access pointer */
#define SPI_DBI ((SPI_TypeDef *) SPI_DBI_BASE)        /*!< SPI_DBI Serial Peripheral Interface register set access pointer */
#define USBOTG0 ((USBOTG_TypeDef *) USBOTG0_BASE)     /*!< USBOTG0  register set access pointer */
#define USBPHY0 ((USBPHYC_TypeDef *) USBPHY0_BASE)    /*!< USBPHY0  register set access pointer */
#define USBEHCI0 ((USB_EHCI_Capability_TypeDef *) USBEHCI0_BASE)/*!< USBEHCI0  register set access pointer */
#define USBOHCI0 ((USB_OHCI_Capability_TypeDef *) USBOHCI0_BASE)/*!< USBOHCI0  register set access pointer */
#define USBEHCI1 ((USB_EHCI_Capability_TypeDef *) USBEHCI1_BASE)/*!< USBEHCI1  register set access pointer */
#define USBOHCI1 ((USB_OHCI_Capability_TypeDef *) USBOHCI1_BASE)/*!< USBOHCI1  register set access pointer */
#define USBPHY1 ((USBPHYC_TypeDef *) USBPHY1_BASE)    /*!< USBPHY1  register set access pointer */
#define EMAC ((EMAC_TypeDef *) EMAC_BASE)             /*!< EMAC  register set access pointer */
#define DE_TOP ((DE_TOP_TypeDef *) DE_TOP_BASE)       /*!< DE_TOP Display Engine (DE) TOP register set access pointer */
#define DE_MIXER0_GLB ((DE_GLB_TypeDef *) DE_MIXER0_GLB_BASE)/*!< DE_MIXER0_GLB Display Engine (DE) - Global Control register set access pointer */
#define DE_MIXER0_BLD ((DE_BLD_TypeDef *) DE_MIXER0_BLD_BASE)/*!< DE_MIXER0_BLD Display Engine (DE) - Blender register set access pointer */
#define DE_MIXER0_VI1 ((DE_VI_TypeDef *) DE_MIXER0_VI1_BASE)/*!< DE_MIXER0_VI1 Display Engine (DE) - VI surface register set access pointer */
#define DE_MIXER0_UI1 ((DE_UI_TypeDef *) DE_MIXER0_UI1_BASE)/*!< DE_MIXER0_UI1 Display Engine (DE) - UI surface register set access pointer */
#define DE_MIXER0_VEP0 ((DE_VEP_TypeDef *) DE_MIXER0_VEP0_BASE)/*!< DE_MIXER0_VEP0 Fresh and Contrast Enhancement (FCE), Peak, Chrominance transient improvement (CTI) LCTI, Blue Level Stretch (BLS), ancy color curvature (FCC), VEP_TOP blocks register set access pointer */
#define DE_MIXER0_VSU1 ((DE_VSU_TypeDef *) DE_MIXER0_VSU1_BASE)/*!< DE_MIXER0_VSU1 Video Scaler Unit (VSU), VS register set access pointer */
#define DE_MIXER0_UIS1 ((DE_UIS_TypeDef *) DE_MIXER0_UIS1_BASE)/*!< DE_MIXER0_UIS1 UI Scaler(UIS) provides RGB format image resizing function register set access pointer */
#define DE_MIXER0_VEP1 ((DE_VEP_TypeDef *) DE_MIXER0_VEP1_BASE)/*!< DE_MIXER0_VEP1 Fresh and Contrast Enhancement (FCE), Peak, Chrominance transient improvement (CTI) LCTI, Blue Level Stretch (BLS), ancy color curvature (FCC), VEP_TOP blocks register set access pointer */
#define DE_MIXER0_VSU2 ((DE_VSU_TypeDef *) DE_MIXER0_VSU2_BASE)/*!< DE_MIXER0_VSU2 Video Scaler Unit (VSU), VS register set access pointer */
#define DE_MIXER0_UIS2 ((DE_UIS_TypeDef *) DE_MIXER0_UIS2_BASE)/*!< DE_MIXER0_UIS2 UI Scaler(UIS) provides RGB format image resizing function register set access pointer */
#define DE_MIXER0_UIS3 ((DE_UIS_TypeDef *) DE_MIXER0_UIS3_BASE)/*!< DE_MIXER0_UIS3 UI Scaler(UIS) provides RGB format image resizing function register set access pointer */
#define DE_MIXER0_DEP ((DE_DEP_TypeDef *) DE_MIXER0_DEP_BASE)/*!< DE_MIXER0_DEP DRC (dynamic range controller) register set access pointer */
#define DE_MIXER1_GLB ((DE_GLB_TypeDef *) DE_MIXER1_GLB_BASE)/*!< DE_MIXER1_GLB Display Engine (DE) - Global Control register set access pointer */
#define DE_MIXER1_BLD ((DE_BLD_TypeDef *) DE_MIXER1_BLD_BASE)/*!< DE_MIXER1_BLD Display Engine (DE) - Blender register set access pointer */
#define DE_MIXER1_VI1 ((DE_VI_TypeDef *) DE_MIXER1_VI1_BASE)/*!< DE_MIXER1_VI1 Display Engine (DE) - VI surface register set access pointer */
#define DE_MIXER1_VEP0 ((DE_VEP_TypeDef *) DE_MIXER1_VEP0_BASE)/*!< DE_MIXER1_VEP0 Fresh and Contrast Enhancement (FCE), Peak, Chrominance transient improvement (CTI) LCTI, Blue Level Stretch (BLS), ancy color curvature (FCC), VEP_TOP blocks register set access pointer */
#define DE_MIXER1_VSU1 ((DE_VSU_TypeDef *) DE_MIXER1_VSU1_BASE)/*!< DE_MIXER1_VSU1 Video Scaler Unit (VSU), VS register set access pointer */
#define DE_MIXER1_UIS1 ((DE_UIS_TypeDef *) DE_MIXER1_UIS1_BASE)/*!< DE_MIXER1_UIS1 UI Scaler(UIS) provides RGB format image resizing function register set access pointer */
#define G2D_TOP ((G2D_TOP_TypeDef *) G2D_TOP_BASE)    /*!< G2D_TOP Graphic 2D top register set access pointer */
#define G2D_MIXER ((G2D_MIXER_TypeDef *) G2D_MIXER_BASE)/*!< G2D_MIXER Graphic 2D (G2D) Engine Video Mixer register set access pointer */
#define G2D_BLD ((G2D_BLD_TypeDef *) G2D_BLD_BASE)    /*!< G2D_BLD Graphic 2D (G2D) Engine Blender register set access pointer */
#define G2D_V0 ((G2D_VI_TypeDef *) G2D_V0_BASE)       /*!< G2D_V0 Graphic 2D VI surface register set access pointer */
#define G2D_UI0 ((G2D_UI_TypeDef *) G2D_UI0_BASE)     /*!< G2D_UI0 Graphic 2D UI surface register set access pointer */
#define G2D_UI1 ((G2D_UI_TypeDef *) G2D_UI1_BASE)     /*!< G2D_UI1 Graphic 2D UI surface register set access pointer */
#define G2D_UI2 ((G2D_UI_TypeDef *) G2D_UI2_BASE)     /*!< G2D_UI2 Graphic 2D UI surface register set access pointer */
#define G2D_WB ((G2D_WB_TypeDef *) G2D_WB_BASE)       /*!< G2D_WB Graphic 2D (G2D) Engine Write Back register set access pointer */
#define G2D_VSU ((G2D_VSU_TypeDef *) G2D_VSU_BASE)    /*!< G2D_VSU Graphic 2D Video Scaler register set access pointer */
#define G2D_ROT ((G2D_ROT_TypeDef *) G2D_ROT_BASE)    /*!< G2D_ROT Graphic 2D Rotate register set access pointer */
#define DSI0 ((DSI_TypeDef *) DSI0_BASE)              /*!< DSI0 MIPI DSI Display Interface register set access pointer */
#define DSI_DPHY ((DSI_DPHY_TypeDef *) DSI_DPHY_BASE) /*!< DSI_DPHY MIPI DSI Physical Interface register set access pointer */
#define DISPLAY_TOP ((DISPLAY_TOP_TypeDef *) DISPLAY_TOP_BASE)/*!< DISPLAY_TOP display interface top (DISPLAY_TOP) register set access pointer */
#define TCON_LCD0 ((TCON_LCD_TypeDef *) TCON_LCD0_BASE)/*!< TCON_LCD0 Timing Controller_LCD (TCON_LCD) register set access pointer */
#define TCON_TV0 ((TCON_TV_TypeDef *) TCON_TV0_BASE)  /*!< TCON_TV0  register set access pointer */
#define HDMI_TX0 ((HDMI_TX_TypeDef *) HDMI_TX0_BASE)  /*!< HDMI_TX0  register set access pointer */
#define HDMI_PHY ((HDMI_PHY_TypeDef *) HDMI_PHY_BASE) /*!< HDMI_PHY  register set access pointer */
#define TVE_TOP ((TVE_TOP_TypeDef *) TVE_TOP_BASE)    /*!< TVE_TOP TV Output TV Encoder (display out interface = CVBS OUT) register set access pointer */
#define TVE0 ((TV_Encoder_TypeDef *) TVE0_BASE)       /*!< TVE0 TV Encoder (display out interface = CVBS OUT) register set access pointer */
#define CSIC_CCU ((CSIC_CCU_TypeDef *) CSIC_CCU_BASE) /*!< CSIC_CCU  register set access pointer */
#define CSIC_TOP ((CSIC_TOP_TypeDef *) CSIC_TOP_BASE) /*!< CSIC_TOP  register set access pointer */
#define CSIC_PARSER0 ((CSIC_PARSER_TypeDef *) CSIC_PARSER0_BASE)/*!< CSIC_PARSER0  register set access pointer */
#define CSIC_DMA0 ((CSIC_DMA_TypeDef *) CSIC_DMA0_BASE)/*!< CSIC_DMA0  register set access pointer */
#define CSIC_DMA1 ((CSIC_DMA_TypeDef *) CSIC_DMA1_BASE)/*!< CSIC_DMA1  register set access pointer */
#define TVD_TOP ((TVD_TOP_TypeDef *) TVD_TOP_BASE)    /*!< TVD_TOP Video Decoding register set access pointer */
#define TVD0 ((TVD0_TypeDef *) TVD0_BASE)             /*!< TVD0 Video Decoding register set access pointer */
#define RISC_CFG ((RISC_CFG_TypeDef *) RISC_CFG_BASE) /*!< RISC_CFG RISC-V core configuration register register set access pointer */
#define R_CPUCFG ((R_CPUCFG_TypeDef *) R_CPUCFG_BASE) /*!< R_CPUCFG  register set access pointer */
#define R_CCU ((R_PRCM_TypeDef *) R_CCU_BASE)         /*!< R_CCU  register set access pointer */
#define R_PRCM ((R_PRCM_TypeDef *) R_PRCM_BASE)       /*!< R_PRCM  register set access pointer */
#define CIR_RX ((CIR_RX_TypeDef *) CIR_RX_BASE)       /*!< CIR_RX  register set access pointer */
#define RTC ((RTC_TypeDef *) RTC_BASE)                /*!< RTC Real Time Clock register set access pointer */
#define CPU_SUBSYS_CTRL ((CPU_SUBSYS_CTRL_TypeDef *) CPU_SUBSYS_CTRL_BASE)/*!< CPU_SUBSYS_CTRL  register set access pointer */
#define C0_CPUX_CFG ((C0_CPUX_CFG_TypeDef *) C0_CPUX_CFG_BASE)/*!< C0_CPUX_CFG  register set access pointer */

#ifdef __cplusplus
 }
#endif /* __cplusplus */

#endif /* HEADER__E88EF34D_BB95_4cfd_9438_7C6E40DDDD90__INCLUDED */
